cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(pymoose)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")
include(CheckCXXCompiler.cmake)
include(CheckIncludeFileCXX)

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
    set(VERSION_MOOSE "3.2.0-${GIT_HEAD}")
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

################################ CMAKE OPTIONS ##################################
option(WITH_NSDF            "Enable NSDF support. Requires hdf5" OFF )

option(DEBUG                "Build with debug support" OFF)
option(GPROF                "Build for profiling using gprof" OFF)
option(ENABLE_UNIT_TESTS    "Enable unit tests (DEBUG should also be ON)" OFF)
option(WITH_MPI             "Enable Openmpi support" OFF)
option(WITH_BOOST           "Enable boost. Prefer boost over stl" OFF)
option(WITH_BOOST_ODE       "Use boost library ode2 library instead of GSL" OFF)
option(WITH_GSL             "Use gsl-library. Alternative is WITH_BOOST" ON)
option(PARALLELIZED_CLOCK   "High level parallelization of moose::Clock (alpha)" OFF )
option(USE_PRIVATE_RNG      "Stochastic Objects use their private RNG" ON)
option(WITH_ASAN            "Use AddressSanitizer in DEBUG mode." OFF)


############################ BUILD CONFIGURATION #################################

# Default definitions.
add_definitions(-DUSE_GENESIS_PARSER)

if(DEBUG OR "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message(STATUS "Building for Debug/Unit testing")
    add_definitions(-DDO_UNIT_TESTS -O)
    set(CMAKE_BUILD_TYPE Debug)
elseif(ENABLE_UNIT_TESTS)
    MESSAGE(STATUS "Enabled Unit tests")
    add_definitions(-DDO_UNIT_TESTS)
    set(CMAKE_BUILD_TYPE Debug)
else()
    message(STATUS "Building for Release/No unit tests.")
    set(CMAKE_BUILD_TYPE Release)
		add_definitions(-UDO_UNIT_TESTS -O3 -DDISABLE_DEBUG)

    # DO NOT Treat all warnings as errors. With some compilers and newer versions
    # this often causes headache.
    # add_definitions(-Werror)
endif()

if(GPROF AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message(STATUS "Compiling with profiling with gprof")
    add_definitions(-pg)
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-pg")
endif()

if(WITH_ASAN AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message(STATUS "Compiling with ASAN")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} \
        -fno-omit-frame-pointer -fsanitize=leak -fsanitize=address")
    set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} \
        -fno-omit-frame-pointer -fsanitize=leak -fsanitize=address")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fsanitize=address")
endif()

################################### TARGETS ####################################

add_library(libmoose SHARED basecode/main.cpp)
add_executable(moose.bin basecode/main.cpp)

################################### SETUP BUILD ################################

# default include paths.
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

if(WITH_BOOST)
    set(WITH_BOOST_ODE ON)
    set(WITH_GSL       OFF)
endif(WITH_BOOST)

# If using BOOST ODE2 library to solve ODE system, then don't use GSL.
if(WITH_BOOST_ODE)
    set(WITH_GSL OFF)
endif(WITH_BOOST_ODE)

set_target_properties(libmoose PROPERTIES COMPILE_DEFINITIONS  "MOOSE_LIB")
set_target_properties(libmoose PROPERTIES PREFIX "")

## Variable to collect all static libraries.
set(STATIC_LIBRARIES "" )
# Collect all shared libraries here.
set(SYSTEM_SHARED_LIBS ${LibXML2_LIBRARIES})

# BOOST ode library performs better than GSL and ideally should be made default.
# Unfortunately Boost does not have a very good matrix library; it has ublas
# which is not well maintained and emit a lot
# of warning during compilation. Nonetheless, WITH_BOOST_ODE works fine and
# produce results quicker than GSL. 
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
elseif(WITH_BOOST_ODE)
    find_package(Boost 1.53 REQUIRED)
    find_package(LAPACK REQUIRED)
endif()

# if boost ode is being used, don't use GSL.
if(WITH_BOOST_ODE)
    add_definitions(-DUSE_BOOST_ODE -UUSE_GSL)
    include_directories(${Boost_INCLUDE_DIRS})
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

if(WITH_BOOST_ODE)
    list(APPEND SYSTEM_SHARED_LIBS ${LAPACK_LIBRARIES})
    list(APPEND SYSTEM_SHARED_LIBS ${Boost_LIBRARIES})
endif(WITH_BOOST_ODE)

find_package( Threads )
list(APPEND SYSTEM_SHARED_LIBS ${CMAKE_THREAD_LIBS_INIT})

# These libraries could be static of dynamic. We need to discrimate between
# these two types because of --whole-archive option. See
# BhallaLab/moose-core#66,
if(WITH_GSL)
    if(GSL_STATIC_LIBRARIES)
       message( STATUS "Using static libraries ${GSL_STATIC_LIBRARIES}" )
       list(APPEND STATIC_LIBRARIES ${GSL_STATIC_LIBRARIES})
    else( )
       # message(DEBUG "Using gsl libraries: ${GSL_LIBRARIES}")
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

# cmake --help-policy CMP0042. Also in pymoose/CMakeLists.txt
# More details:
# https://gitlab.kitware.com/cmake/community/wikis/doc/cmake/RPATH-handling
# especially section 'Mac OS X and the RPATH'
# Switching is OFF since all libraries are statically linked in module.
if(APPLE)
    set_target_properties( libmoose PROPERTIES MACOSX_RPATH OFF)
endif(APPLE)

# MAC linker does not understand many of gnu-ld options.
# message( DEBUG "Shared libs: ${SYSTEM_SHARED_LIBS}")
if(APPLE)
    target_link_libraries(libmoose
        "-Wl,-all_load"
        ${MOOSE_LIBRARIES}
        ${STATIC_LIBRARIES}
        )

    target_link_libraries(libmoose
        ${SYSTEM_SHARED_LIBS}
        ${CMAKE_DL_LIBS}
        )
else(APPLE)
    target_link_libraries(libmoose
        "-Wl,--whole-archive"
        ${MOOSE_LIBRARIES}
        ${STATIC_LIBRARIES}
        "-Wl,--no-whole-archive"
        ${SYSTEM_SHARED_LIBS}
        )
endif(APPLE)

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
if(${_platform_desc} MATCHES ".*(Ubuntu|debian).*")
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

# If no one hsa set the PYMOOSE bdist installation directory, use default.
if(NOT PYMOOSE_BDIST_INSTALL_DIR)
    set(PYMOOSE_BDIST_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/pymoose_bdist_install)
endif()
file(MAKE_DIRECTORY ${PYMOOSE_BDIST_INSTALL_DIR})

# We need a custom name for bdist; same on all platform.
set(_platform "CMAKE" )
set(PYMOOSE_BDIST_FILE ${PYMOOSE_BDIST_DIR}/pymoose-${VERSION_MOOSE}.${_platform}.tar.gz)
message(STATUS "binary distribution file ${PYMOOSE_BDIST_FILE}")
add_custom_target(bdist ALL DEPENDS  ${PYMOOSE_BDIST_FILE} )

# Any command using setup.cmake.py must run in the same directory. Use option
# `--relative` to prefix is aways fixed.
add_custom_command( OUTPUT ${PYMOOSE_BDIST_FILE}
    COMMAND ${PYTHON_EXECUTABLE} setup.cmake.py bdist_dumb -p ${_platform}
        -d ${PYMOOSE_BDIST_DIR} --relative
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/python
    COMMENT "bdist is saved to ${PYMOOSE_BDIST_DIR}"
    VERBATIM
    )
add_custom_command(TARGET bdist POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E chdir ${PYMOOSE_BDIST_INSTALL_DIR} tar xf ${PYMOOSE_BDIST_FILE}
    COMMENT "Unarchiving bdist file ${PYMOOSE_BDIST_FILE} to ${PYMOOSE_BDIST_INSTALL_DIR}"
    VERBATIM
    )

# Copy python files from source to current binary directory. Make sure to call
# this before building bdist.
file(GLOB_RECURSE PYTHON_SRCS "${CMAKE_SOURCE_DIR}/python/*")
add_custom_target(copy_pymoose DEPENDS ${PYTHON_SRCS}
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/python ${CMAKE_CURRENT_BINARY_DIR}/python
    COMMENT "Copying required python files and other files to build directory"
    VERBATIM
    )

add_dependencies( bdist copy_pymoose )
add_dependencies( copy_pymoose _moose )

######################### INSTALL ##############################################

install(TARGETS moose.bin DESTINATION bin CONFIGURATIONS Debug)
install(TARGETS libmoose DESTINATION lib CONFIGURATIONS Debug)

# install pymoose bdist.
install(DIRECTORY ${PYMOOSE_BDIST_INSTALL_DIR}/
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    CONFIGURATIONS Release Debug
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

if(DEBUG OR "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    # Run this test in debug mode. In Release mode, this does not do anything.
    set(MOOSE_BIN_LOCATION $<TARGET_FILE:moose.bin>)
    message(STATUS "Executable moose.bin will be at ${MOOSE_BIN_LOCATION}" )

    add_test(NAME moose.bin-raw-run COMMAND moose.bin -u -q)
endif()

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

# Alpha tests. These should not run by default.
set(MOOSE_ALPHA_TEST_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/alpha)
file(GLOB PY_ALPHA_TEST_SCRIPTS "${PYMOOSE_ALPHA_TEST_DIRECTORY}/*.py" )
foreach( _test_script ${PY_ALPHA_TEST_SCRIPTS} )
    get_filename_component( _test_name ${_test_script} NAME_WE)
    add_test( NAME ${_test_name}
        COMMAND ${PYTHON_EXECUTABLE} ${_test_script}
	CONFIGURATIONS Devel
	WORKING_DIRECTORY ${PYMOOSE_ALPHA_TEST_DIRECTORY}
        )
     set_tests_properties( ${_test_name}
         PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/python"
        )
endforeach( )

# Regression and github issues. These should not run by default.
set(PYMOOSE_ISSUES_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/issues)
file(GLOB PY_ISSUES_SCRIPTS "${PYMOOSE_ISSUES_DIRECTORY}/*.py" )
foreach( _test_script ${PY_ISSUES_SCRIPTS} )
    get_filename_component( _test_name ${_test_script} NAME_WE)
    add_test( NAME ${_test_name}
        COMMAND ${PYTHON_EXECUTABLE} ${_test_script}
	CONFIGURATIONS Devel
	WORKING_DIRECTORY ${PYMOOSE_ISSUES_DIRECTORY}
        )
     set_tests_properties( ${_test_name}
         PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/python"
        )
endforeach( )

# add pylint target.
set(PYSCRIPTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/python )
file(GLOB_RECURSE PY_SCRIPTS  "python/*.py")
add_custom_target( pylint )
foreach( _py_script ${PY_SCRIPTS} )
    get_filename_component( _py_name ${_py_script} NAME_WE)
    file( READ ${_py_script} pytext)
    string(MD5 _md5 "${pytext}")
    set(TGT_NAME "${_py_name}-${_md5}" )
    set(PYLINT_OPTIONS --disable=no-member --disable=no-name-in-module
            --disable=invalid-unary-operand-type
            --disable=import-error
            --disable=no-method-argument
        )
    add_custom_target( ${TGT_NAME}
        COMMAND ${PYTHON_EXECUTABLE} -m pylint -E ${PYLINT_OPTIONS} ${_py_script}
        COMMENT "Running pylint on ${_py_script}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    add_dependencies(pylint ${TGT_NAME} )
endforeach( )

############################ CPACK ######################################
# This is not maintained anymore since packaging is done using OpenBuildService 
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake_moose_cpack.cmake)
