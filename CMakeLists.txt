set(CMAKE_LEGACY_CYGWIN_WIN32 0)
cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
    cmake_policy(SET CMP0004 OLD)
    if(POLICY CMP0050)
        cmake_policy(SET CMP0050 OLD)
    endif(POLICY CMP0050)
endif(COMMAND cmake_policy)

project(MOOSE)

include(CheckCXXCompiler.cmake)
include(CheckIncludeFileCXX)
include(FindPkgConfig)

# If from command line, version info is not passed, use the git to generate a
# version file. If GIT fails, use the previous known version.
set(VERSION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/VERSION)
find_program(GIT_EXEC "git")
message( STATUS "Looking for git ${GIT_EXEC}" )
if( (NOT MOOSE_VERSION) AND GIT_EXEC)
    execute_process( 
        COMMAND ${GIT_EXEC} describe --tags --long
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE MOOSE_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    message(STATUS "+ Writing ${MOOSE_VERSION} to ${VERSION_FILE}" )
    file(WRITE ${VERSION_FILE} ${MOOSE_VERSION})
elseif( (NOT MOOSE_VERSION) AND (NOT GIT_EXEC) )
    message(STATUS "+ Reading ${VERSION_FILE}" )
    file(READ ${VERSION_FILE} GIT_VERSION_OUTPUT )
elseif(MOOSE_VERSION)
    message(STATUS "+ Using user specified VERSION = ${MOOSE_VERSION}" )
    file(WRITE ${VERSION_FILE} ${MOOSE_VERSION})
else()
    message(FATAL_ERROR "Could not determine MOOSE_VERSION" )
endif( )

add_definitions( -DMOOSE_VERSION="${MOOSE_VERSION}")
message( STATUS "MOOSE Version ${MOOSE_VERSION}" )

# This snippet is from LLVM project.
# Sanity check our source directory to make sure that we are not trying to
# generate an in-tree build (unless on MSVC_IDE, where it is ok), and to make
# sure that we don't have any stray generated files lying around in the tree
# (which would end up getting picked up by header search, instead of the correct
# versions).

message(STATUS "CMAKE_INSTALL_PREFIX : ${CMAKE_INSTALL_PREFIX}")
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

option(VERBOSITY "Set MOOSE verbosity level (deprecated)" 0)
## Unit testing and debug mode.
option(DEBUG "Build with debug support" OFF)
option(GPROF "Build for profiling using gprof" OFF)
option(ENABLE_UNIT_TESTS "Enable unit tests (DEBUG should also be ON)" OFF)
option(WITH_PYTHON "Build native python extension" ON)
option(WITH_MPI  "Enable Openmpi support" OFF)
option(WITH_BOOST "Use boost library instead of GSL" OFF)
option(WITH_GSL  "Use gsl-library. Alternative is WITH_BOOST" ON)

################################# CMKAE MACROS #################################

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

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
    find_package(GSL 1.16)
    if(NOT GSL_FOUND)
        message(FATAL_ERROR 
            "=====================================================================\n"
            " FATAL gsl(>1.16) not found.\n\n"
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

find_package(Termcap)
find_package(Readline)

if(READLINE_FOUND AND TERMCAP_FOUND)
    add_definitions(-DUSE_READLINE)
    include_directories(${Readline_INCLUDE_DIR})
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
endif()

if(WITH_MPI)
    if(MPI_CXX_FOUND)
        list(APPEND SYSTEM_SHARED_LIBS ${MPI_CXX_LIBRARIES})
    endif()
endif(WITH_MPI)

pkg_check_modules(MUPARSER muparser)
if(MUPARSER_FOUND)
    message(STATUS "Using system muparser ${MUPARSER_VERSION}")
    list(APPEND SYSTEM_SHARED_LIBS ${MUPARSER_LIBRARIES})
else()
    message(STATUS "Using private muparser")
    add_subdirectory(external/muparser)
    list(APPEND MOOSE_LIBRARIES muparser)
endif()

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
# Root of all python module.
if(WITH_PYTHON)
    add_subdirectory( pymoose )
endif(WITH_PYTHON)

######################### INSTALL ##############################################

install(TARGETS moose.bin
    DESTINATION bin
    )

install(TARGETS libmoose
    DESTINATION lib
    )

install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/moose
    DESTINATION bin
    )

# This target is built by pymoose/CMakeLists.txt file.
if(WITH_PYTHON)
    install(CODE 
        "
        execute_process(
            COMMAND ${PYTHON_EXECUTABLE} setup.py install --prefix=${CMAKE_INSTALL_PREFIX}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/python
            )
        "
        )

endif(WITH_PYTHON)

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

include(${CMAKE_CURRENT_SOURCE_DIR}/MooseTests.cmake)

############################ CPACK ######################################
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake_moose_cpack.cmake)
