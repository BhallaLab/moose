set(CMAKE_LEGACY_CYGWIN_WIN32 0)
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)


project(pymoose)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
    cmake_policy(SET CMP0004 NEW)
    if(POLICY CMP0050)
        cmake_policy(SET CMP0050 OLD)
    endif(POLICY CMP0050)
endif(COMMAND cmake_policy)

project(moose-core)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")
include(CheckCXXCompiler.cmake)
include(CheckIncludeFileCXX)
include(FindPkgConfig)

# getgit revision number
find_program( GIT_EXEC git )
if(GIT_EXEC)
    execute_process(
        COMMAND ${GIT_EXEC} rev-parse --short HEAD
        OUTPUT_VARIABLE GIT_HEAD
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else(GIT_EXEC)
    set(GIT_HEAD 'HEAD')
    message(STATUS "Couldn't fetch version from git repo" )
endif()

# If from command line, version info is not passed, use the git to generate a
# version file. If GIT fails, use the previous known version.
if(NOT VERSION_MOOSE)
    set(VERSION_MOOSE "3.1.3-${GIT_HEAD}")
endif()

add_definitions( -DMOOSE_VERSION="${VERSION_MOOSE}")
message( STATUS "MOOSE Version ${VERSION_MOOSE}" )

# Write VERSION to a file VERSION so that setup.cmake.py can use it.
set(VERSION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/python/VERSION)
file(WRITE ${VERSION_FILE} ${VERSION_MOOSE} )
message(STATUS "| Writing ${VERSION_MOOSE} to ${VERSION_FILE}" )

# This snippet is from LLVM project.
# Sanity check our source directory to make sure that we are not trying to
# generate an in-tree build (unless on MSVC_IDE, where it is ok), and to make
# sure that we don't have any stray generated files lying around in the tree
# (which would end up getting picked up by header search, instead of the correct
# versions).
if( CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR AND NOT MSVC_IDE )
    message(FATAL_ERROR
        "======================================================================\n"
        "In-source builds are not allowed. Remove CMakeCache.txt and CMakeFiles\n"
        "directory and do something like this inside this directory \n"
        "    $ mkdir _build_dir \n"
        "    $ cd _build_dir  \n"
        "    $ cmake ..  \n"
        "===================================================================== \n"
        )
endif()

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY
    )

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    )

# This is for testing purpose.
link_directories(${CMAKE_CURRENT_BINARY_DIR})

################################# OS Specific ##################################
message(STATUS "Operating system: ${CMAKE_SYSTEM_NAME}")
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    CMAKE_POLICY(SET CMP0042 NEW)
    set(MACOSX TRUE)
else()
    set(MACOSX FALSE)
endif()

################################ CMAKE OPTIONS ##################################
option(DEBUG "Build with debug support" OFF)
option(GPROF "Build for profiling using gprof" OFF)
option(ENABLE_UNIT_TESTS "Enable unit tests (DEBUG should also be ON)" OFF)
option(WITH_MPI  "Enable Openmpi support" OFF)
option(WITH_BOOST "Use boost library instead of GSL" OFF)
option(WITH_GSL  "Use gsl-library. Alternative is WITH_BOOST" ON)
option(PARALLELIZED_SOLVERS "Use parallel version of GSOLVE. (alpha)" OFF )
option(PARALLELIZED_CLOCK "High level parallelization of moose::Clock (alpha)" OFF )


############################ BUILD CONFIGURATION #################################

# Default macros
add_definitions(-DUSE_GENESIS_PARSER)

if(DEBUG)
    message(STATUS "Building for Debug/Unit testing")
    add_definitions(-DDO_UNIT_TESTS)
    set(CMAKE_BUILD_TYPE Debug)
elseif(ENABLE_UNIT_TESTS)
    MESSAGE(STATUS "Enabled Unit tests")
    add_definitions(-DDO_UNIT_TESTS)
    set(CMAKE_BUILD_TYPE Debug)
else()
    message(STATUS "Building for Release/No unit tests.")
    set(CMAKE_BUILD_TYPE Release)
    add_definitions(-UDO_UNIT_TESTS -DQUIET_MODE -O3)
endif()

if(GPROF AND DEBUG)
    message(STATUS "Compiling with profiling with gprof")
    add_definitions(-pg)
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-pg")
endif()


################################### TARGETS ####################################

add_library(libmoose SHARED basecode/main.cpp)
add_executable(moose.bin basecode/main.cpp)

################################### SETUP BUILD ################################

# default include paths.
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

if(WITH_BOOST)
    set(WITH_GSL OFF)
endif(WITH_BOOST)

include_directories(msg basecode)

set_target_properties(libmoose PROPERTIES COMPILE_DEFINITIONS  "MOOSE_LIB")
set_target_properties(libmoose PROPERTIES PREFIX "")

## Variable to collect all static libraries.
set(STATIC_LIBRARIES "" )
# Collect all shared libraries here.
set(SYSTEM_SHARED_LIBS ${LibXML2_LIBRARIES})

if(WITH_GSL)
    find_package(GSL 1.16 REQUIRED)
    if(NOT GSL_FOUND)
        message(FATAL_ERROR
            "=====================================================================\n"
            " FATAL gsl(>=1.16) not found.\n\n"
            " MOOSE requires Gnu Scientific Library (GSL) 1.16 or higher. \n"
            " Please install the `dev` or `devel` package e.g. \n"
            "     $ sudo apt-get install libgsl0-dev \n"
            "     $ sudo yum install libgsl-devel \n"
            "     $ brew install gsl \n\n"
            " Or build and install gsl from source code \n"
            "     https://www.gnu.org/software/gsl/ \n"
            " After installing gsl, rerun cmake.\n\n"
            " If you install gsl in non-standard place, set the GSL_ROOT_DIR environment \n"
            " variable. CMAKE use this to search for required files. \n"
            "====================================================================\n"
            )
    endif(NOT GSL_FOUND)
    add_definitions(-DUSE_GSL)
    # GSL is also used in RNG (whenever applicable), therefore include paths are
    # top level.
    include_directories( ${GSL_INCLUDE_DIRS} )
elseif(WITH_BOOST)
    find_package(Boost 1.44 COMPONENTS system filesystem random REQUIRED)
    find_package( LAPACK REQUIRED )
    add_definitions( -DUSE_BOOST -UUSE_GSL )
    include_directories( ${Boost_INCLUDE_DIRS} )
    check_include_file_cxx(
        ${Boost_INCLUDE_DIRS}/boost/random/random_device.hpp
        BOOST_RANDOM_DEVICE_EXISTS
        )
    if(BOOST_RANDOM_DEVICE_EXISTS)
        add_definitions(-DBOOST_RANDOM_DEVICE_EXISTS)
    endif(BOOST_RANDOM_DEVICE_EXISTS)
endif()


find_package(HDF5 COMPONENTS CXX HL)
if(NOT HDF5_FOUND)
    message(
        "==================================================================\n"
        " HDF5 not found. Disabling support. Required for nsdf. \n\n"
        " If you need hdf5 support, please install hdf5-dev or hdf5-devel\n"
        " package or equivalent.\n\n"
        "     $ sudo apt-get install libhdf5-dev \n"
        "     $ sudo yum install libhdf5-devel \n"
        "     $ brew install hdf5 \n\n"
        " Otherwise, continue with 'make' and 'make install' \n"
        " If you install hdf5 to non-standard path, export environment \n"
        " variable HDF5_ROOT to the location. Rerun cmake \n"
        "================================================================ \n"
        )
endif(NOT HDF5_FOUND)

if(HDF5_FOUND)
    include_directories( ${HDF5_INCLUDE_DIRS} )
    add_definitions( -DUSE_HDF5 )
    if(HDF5_USE_STATIC_LIBRARIES)
	message(STATUS "Finding static HDF5 libraries in $ENV{HDF5_ROOT}")
        find_library(HDF5_CXX_LIBRARIES NAMES libhdf5.a
            PATHS $ENV{HDF5_ROOT}/lib $ENV{HDF5_ROOT}/lib64
            )
        find_library(HDF5_HL_LIBRARIES NAMES libhdf5_hl.a
            PATHS $ENV{HDF5_ROOT}/lib $ENV{HDF5_ROOT}/lib64
            )
        set(HDF5_LIBRARIES ${HDF5_CXX_LIBRARIES} ${HDF5_HL_LIBRARIES})
    endif()


    # Make sure, HDF5_HL_LIBRARIES are set. The COMPONENTS in find_package may
    # or may not work. See BhallaLab/moose-core#163.
    if(NOT HDF5_HL_LIBRARIES)
        set(HDF5_HL_LIBRARIES ${HDF5_HL_LIBRARIES})
    endif(NOT HDF5_HL_LIBRARIES)
    list(APPEND HDF5_LIBRARIES ${HDF5_HL_LIBRARIES})

    message(STATUS "MOOSE will use following HDF5 ${HDF5_LIBRARIES}" )
    foreach(HDF5_LIB ${HDF5_LIBRARIES})
        if(HDF5_LIB)
            get_filename_component( HDF5_LIB_EXT ${HDF5_LIB} EXT )
            if(HDF5_LIB_EXT)
                if(${HDF5_LIB_EXT} STREQUAL ".a")
                    list(APPEND STATIC_LIBRARIES ${HDF5_LIB} )
                else( )
                    list(APPEND SYSTEM_SHARED_LIBS ${HDF5_LIB} )
                endif( )
            endif()
        endif( )
    endforeach( )
else( HDF5_FOUND )
    message(STATUS "HDF5 is not found" )
endif( HDF5_FOUND )

# This is a fix for new HDF5 package on Debian/Ubuntu which installs hdf5
# headers in non-standard path. issue #80.
if(HDF5_LIBRARY_DIRS)
    set_target_properties( libmoose PROPERTIES LINK_FLAGS "-L${HDF5_LIBRARY_DIRS}" )
endif()

# Openmpi
if(WITH_MPI)
    find_package(MPI REQUIRED)
    if(MPI_CXX_FOUND)
        message(STATUS "Using MPI from ${MPI_CXX_INCLUDE_PATH}")
        include_directories(${MPI_CXX_INCLUDE_PATH})
        set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
        add_definitions(-DUSE_MPI)
        SET(CMAKE_CXX_COMPILER ${MPI_CXX_COMPILER})
        SET(CMAKE_C_COMPILER ${MPI_C_COMPILER})
    else()
        message(STATUS "Cound not find MPI")
        add_definitions(-UUSE_MPI)
    endif()
endif(WITH_MPI)

if(WITH_BOOST)
    list(APPEND SYSTEM_SHARED_LIBS ${LAPACK_LIBRARIES})
    list(APPEND SYSTEM_SHARED_LIBS ${Boost_LIBRARIES})
endif(WITH_BOOST)

# These libraries could be static of dynamic. We need to discrimate between
# these two types because of --whole-archive option. See
# BhallaLab/moose-core#66,
if(WITH_GSL)
    if(GSL_STATIC_LIBRARIES)
       message( STATUS "Using static libraries ${GSL_STATIC_LIBRARIES}" )
       list(APPEND STATIC_LIBRARIES ${GSL_STATIC_LIBRARIES})
    else( )
       message(STATUS "Using gsl libraries: ${GSL_LIBRARIES}")
       foreach(GSL_LIB ${GSL_LIBRARIES} )
           if(GSL_LIB)
               get_filename_component( GSL_LIB_EXT ${GSL_LIB} EXT )
               if(GSL_LIB_EXT)
                   if(GSL_LIB_EXT STREQUAL ".a" )
                       list(APPEND STATIC_LIBRARIES ${GSL_LIB})
                   else()
                       list(APPEND SYSTEM_SHARED_LIBS ${GSL_LIB})
                   endif( )
               endif( )
           endif( )
       endforeach( )
   endif( )
endif()

if(WITH_MPI)
    if(MPI_CXX_FOUND)
        list(APPEND SYSTEM_SHARED_LIBS ${MPI_CXX_LIBRARIES})
    endif()
endif(WITH_MPI)

# Always use private version of muparser. We have some custom edits.
add_subdirectory(external/muparser)
list(APPEND MOOSE_LIBRARIES muparser)

# Add subdirectroeis
add_subdirectory(basecode)
add_subdirectory(msg)
add_subdirectory(shell)
add_subdirectory(randnum)
add_subdirectory(scheduling)
add_subdirectory(biophysics)
add_subdirectory(builtins)
add_subdirectory(utility)
add_subdirectory(mesh)
add_subdirectory(mpi)
add_subdirectory(signeur)
add_subdirectory(ksolve)
add_subdirectory(hsolve)
add_subdirectory(diffusion)
add_subdirectory(device)
add_subdirectory(benchmarks)
add_subdirectory(kinetics)
add_subdirectory(synapse)
add_subdirectory(intfire)

###################################### LINKING #################################
list(APPEND MOOSE_LIBRARIES
    moose_builtins
    msg
    benchmarks
    shell
    randnum
    scheduling
    moose_mpi
    biophysics
    utility
    kinetics
    synapse
    intfire
    hsolve
    mesh
    signeur
    diffusion
    ksolve
    device
    basecode
    )

# Make sure to remove duplicates.
list(REMOVE_DUPLICATES STATIC_LIBRARIES)
if(SYSTEM_SHARED_LIBS)
    list(REMOVE_DUPLICATES SYSTEM_SHARED_LIBS)
endif( )

# MAC linker does not understand many of gnu-ld options.
message( STATUS "Shared libs: ${SYSTEM_SHARED_LIBS}")
if(MACOSX)
    target_link_libraries(libmoose
        "-Wl,-all_load"
        ${MOOSE_LIBRARIES}
        ${STATIC_LIBRARIES}
        )

    target_link_libraries(libmoose
        ${SYSTEM_SHARED_LIBS}
        ${CMAKE_DL_LIBS}
        )
ELSE(MACOSX)
    target_link_libraries(libmoose
        "-Wl,--whole-archive"
        ${MOOSE_LIBRARIES}
        ${STATIC_LIBRARIES}
        "-Wl,--no-whole-archive"
        ${SYSTEM_SHARED_LIBS}
        )
endif(MACOSX)

add_dependencies(moose.bin libmoose)
target_link_libraries(moose.bin moose ${CMAKE_DL_LIBS})
if( WITH_BOOST )
    target_link_libraries( moose.bin ${Boost_LIBRARIES} )
endif( WITH_BOOST )


######################### BUILD PYMOOSE ########################################
# This target is built by pymoose/CMakeLists.txt file.
add_subdirectory( pymoose )

# always override debian default installation directory. It will be installed in
# site-packages instead of dist-packages.
# See https://bugs.launchpad.net/ubuntu/+source/python2.6/+bug/362570
# HACK: Get platform information from python and use it to fix the layout.
execute_process(COMMAND ${PYTHON_EXECUTABLE} -mplatform OUTPUT_VARIABLE _platform_desc)
message(STATUS "Platform: ${_platform_desc}")

# DISTUTILS_EXTRA_ARGS may come of top-level cmake script. On DEBIAN/UBUNTU, it
# is most likely to be --install-layout=deb .
set(EXTRA_ARGS "--prefix ${CMAKE_INSTALL_PREFIX} ${DISTUTILS_EXTRA_ARGS}")

# On Debian/Ubuntu install using debian layout.
# NOTE: Also create setup.cfg file which setup prefix and install-layout
# suitable for DEBIAN systems.
if(${_platform_desc} MATCHES ".*(Ubuntu|Debian).*")
    list(APPEND EXTRA_ARGS "--install-layout=deb")
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/python/setup.cfg
         "[install]\nprefix=/usr\ninstall-layout=deb"
       )
endif()

# Set PYMOOSE binary distribution build and install directories.
# Target for creating bdist. This is handy for creating bdist for packaging.
# Save the binary distribution (tar.gz) to PYMOOSE_BDIST_DIR.
find_package(PythonInterp REQUIRED)
if(NOT PYMOOSE_BDIST_DIR)
    set(PYMOOSE_BDIST_DIR ${CMAKE_BINARY_DIR}/bdist)
endif( )
set(PYMOOSE_BDIST_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/pymoose_bdist_install)
file(MAKE_DIRECTORY ${PYMOOSE_BDIST_INSTALL_DIR})

# We need a custom name for bdist; same on all platform.
set(_platform "CMAKE" )
set(PYMOOSE_BDIST_FILE ${PYMOOSE_BDIST_DIR}/pymoose-${VERSION_MOOSE}.${_platform}.tar.gz)
message(STATUS "binary distribution file ${PYMOOSE_BDIST_FILE}")
add_custom_target(bdist ALL DEPENDS  ${PYMOOSE_BDIST_FILE} )

# Any command using setup.cmake.py must run in the same directory.
add_custom_command( OUTPUT ${PYMOOSE_BDIST_FILE}
    COMMAND ${PYTHON_EXECUTABLE} setup.cmake.py bdist_dumb -p ${_platform}
        -d ${PYMOOSE_BDIST_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/python
    COMMENT "bdist is saved to ${PYMOOSE_BDIST_DIR}"
    VERBATIM
    )
add_custom_command(TARGET bdist POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E chdir ${PYMOOSE_BDIST_INSTALL_DIR} tar xvf ${PYMOOSE_BDIST_FILE}
    COMMENT "Unarchiving bdist file ${PYMOOSE_BDIST_FILE} to ${PYMOOSE_BDIST_INSTALL_DIR}"
    VERBATIM
    )

# Copy python files from source to current binary directory. Make sure to call
# this before building bdist.
file(GLOB_RECURSE PYTHON_SRCS "${CMAKE_SOURCE_DIR}/python/*")
add_custom_target(copy_pymoose DEPENDS ${PYTHON_SRCS}
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/python ${CMAKE_CURRENT_BINARY_DIR}/python
        COMMENT "Copying required python files from
        ${CMAKE_CURRENT_SOURCE_DIR}/python to  to build directory"
    VERBATIM
    )

add_dependencies( bdist copy_pymoose )
add_dependencies( _moose bdist )

######################### INSTALL ##############################################

install(TARGETS moose.bin DESTINATION bin CONFIGURATIONS Debug)

install(TARGETS libmoose DESTINATION lib CONFIGURATIONS Debug)

install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/moose
    DESTINATION bin CONFIGURATIONS Debug
    )

# install pymoose bdist.
install(DIRECTORY ${PYMOOSE_BDIST_INSTALL_DIR}/usr/
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    CONFIGURATIONS Release
    )

# Print message to start build process
if(${CMAKE_BUILD_TOOL} MATCHES "make")
    message(
        "=======================================\n"
        "If cmake did not report any error, run \n"
	" 'make' to build  MOOSE \n"
        "=======================================\n"
        )
endif()


############################ CTEST ######################################
include( CTest )
# If CTEST_OUTPUT_ON_FAILURE environment variable is set, the output is printed
# onto the console if a test fails.
set(ENV{CTEST_OUTPUT_ON_FAILURE} ON)

if(DEBUG)
    # Run this test in debug mode. In Release mode, this does not do anything.
    set(MOOSE_BIN_LOCATION $<TARGET_FILE:moose.bin>)
    message(STATUS "Executable moose.bin will be at ${MOOSE_BIN_LOCATION}" )

    add_test(NAME moose.bin-raw-run COMMAND moose.bin -u -q)
endif(DEBUG)

## PyMOOSE tests.
set(PYMOOSE_TEST_DIRECTORY ${CMAKE_SOURCE_DIR}/tests/python)

# Collect all python script in tests folder and run them.
file(GLOB PY_TEST_SCRIPTS "${PYMOOSE_TEST_DIRECTORY}/test_*.py" )

# Run python tests.
foreach( _test_script ${PY_TEST_SCRIPTS} )
    get_filename_component( _test_name ${_test_script} NAME_WE)
    add_test( NAME ${_test_name}
        COMMAND ${PYTHON_EXECUTABLE} ${_test_script}
        WORKING_DIRECTORY ${PYMOOSE_TEST_DIRECTORY}
        )
     set_tests_properties( ${_test_name}
         PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/python"
        )
endforeach( )

############################ CPACK ######################################
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake_moose_cpack.cmake)
