cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# Project to build MOOSE's python module.
project(PyMOOSE)

# cmake related macros.
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CheckCXXCompiler.cmake)
include(CheckIncludeFileCXX)

# We find python executable here. Though mainly used inside pymoose. 
# FIXME: When cmake 3.12 is widely available use the following:
#   find_package(Python3 COMPONENTS Interpreter Numpy)
#   set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
set(Python_ADDITIONAL_VERSIONS 2.7)
find_package(PythonInterp 3.5)
if(NOT PYTHONINTERP_FOUND)
  find_package(PythonInterp 2.7)
endif()

# Disable rpath on OSX.
if(APPLE)
  set(CMAKE_MACOSX_RPATH OFF)
endif()


# NOTE: version should be changed in setup.py file.
# If moose version is not given, use setup.py file to get the default version.
if(NOT VERSION_MOOSE)
    execute_process(COMMAND ${PYTHON_EXECUTABLE} setup.py --version
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE VERSION_MOOSE
        OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

add_definitions(-DMOOSE_VERSION="${VERSION_MOOSE}")
message(STATUS "MOOSE Version ${VERSION_MOOSE}")

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

if(WITH_GSL)
    set(WITH_BOOST OFF)
    set(WITH_BOOST_ODE OFF)
elseif(WITH_BOOST)
    set(WITH_BOOST_ODE ON)
    set(WITH_GSL OFF)
endif()

################################### TARGETS ####################################

link_directories(${CMAKE_BINARY_DIR})
add_library(libmoose SHARED basecode/main.cpp)
add_executable(moose.bin basecode/main.cpp)


################################### SETUP BUILD ################################
# default include paths.
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
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
elseif(WITH_BOOST_ODE)
    find_package(Boost 1.53 REQUIRED)
    find_package(LAPACK REQUIRED)
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
add_subdirectory(kinetics)
add_subdirectory(synapse)
add_subdirectory(intfire)
add_subdirectory(external/libsoda)

# development related.
add_subdirectory(devel)

###################################### LINKING #################################
list(APPEND MOOSE_LIBRARIES
    moose_builtins
    msg
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
    lsoda
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
add_subdirectory(pymoose)

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
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/setup.cfg
         "[install]\nprefix=/usr\ninstall-layout=deb"
       )
endif()


######################### INSTALL ##############################################

install(TARGETS moose.bin DESTINATION bin CONFIGURATIONS Debug)
install(TARGETS libmoose DESTINATION lib CONFIGURATIONS Debug)

# NOTE: Install macro for _moose (pymoose) has been moved to
# pymoose/CMakeLists.txt file.

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
set(CTEST_NIGHTLY_START_TIME "05:30:00 UTC")
set(CTEST_SUBMIT_URL "http://my.cdash.org/submit.php?project=moose")

if(DEBUG OR "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    # Run this test in debug mode. In Release mode, this does not do anything.
    set(MOOSE_BIN_LOCATION $<TARGET_FILE:moose.bin>)
    message(STATUS "Executable moose.bin will be at ${MOOSE_BIN_LOCATION}" )

    add_test(NAME moose.bin-raw-run COMMAND moose.bin -u -q)
endif()

# Core tests.
set(PYMOOSE_TEST_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/py_moose)
file(GLOB PY_TEST_SCRIPTS "${PYMOOSE_TEST_DIRECTORY}/test_*.py" )
foreach(_test_script ${PY_TEST_SCRIPTS} )
    get_filename_component(_name_we ${_test_script} NAME_WE)
    set(_test_name "core_${_name_we}")
    add_test(NAME ${_test_name}
        COMMAND ${PYTHON_EXECUTABLE} ${_test_script}
        WORKING_DIRECTORY ${PYMOOSE_TEST_DIRECTORY})
     set_tests_properties(${_test_name}
         PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/python"
        )
endforeach()

# Tests for supported formats such as neuroml, sbml etc.
set(SUPPORT_TEST_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/support)
file(GLOB PY_TEST_SCRIPTS "${SUPPORT_TEST_DIRECTORY}/test_*.py" )
foreach(_test_script ${PY_TEST_SCRIPTS} )
    get_filename_component(_name_we ${_test_script} NAME_WE)
    set(_test_name "support_${_name_we}")
    add_test(NAME ${_test_name}
        COMMAND ${PYTHON_EXECUTABLE} ${_test_script}
        WORKING_DIRECTORY ${SUPPORT_TEST_DIRECTORY})
     set_tests_properties(${_test_name}
         PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/python"
        )
endforeach()

# rdesigneur tests. These tests require matplotlib.
set(RDES_TEST_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/py_rdesigneur) 
file(GLOB RDES_TEST_SCRIPTS "${RDES_TEST_DIRECTORY}/test_*.py" )
foreach(_test_script ${RDES_TEST_SCRIPTS})
    get_filename_component(_name_we ${_test_script} NAME_WE)
    set(_test_name "rdes_${_name_we}")
    add_test(NAME ${_test_name}
        COMMAND ${PYTHON_EXECUTABLE} ${_test_script}
        WORKING_DIRECTORY ${RDES_TEST_DIRECTORY}
        )
    set_tests_properties(${_test_name} PROPERTIES 
        ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/python;MOOSE_NUM_THREADS=4"
        )
endforeach()

# FIXME TESTS. These should not run by default. We need to fix them.
set(PYMOOSE_FIXME_TEST_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/devel/fixme)
file(GLOB PY_FIXME_TEST_SCRIPTS "${PYMOOSE_FIXME_TEST_DIRECTORY}/*.py" )
foreach( _test_script ${PY_FIXME_TEST_SCRIPTS} )
    get_filename_component( _name_we ${_test_script} NAME_WE)
    set(_test_name "alpha_${_name_we}")
    add_test( NAME ${_test_name}
        COMMAND ${PYTHON_EXECUTABLE} ${_test_script}
        CONFIGURATIONS alpha
	WORKING_DIRECTORY ${PYMOOSE_ALPHA_TEST_DIRECTORY}
        )
     set_tests_properties( ${_test_name}
         PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/python"
        )
endforeach( )

# Regression and github issues. These should not run by default.
set(PYMOOSE_ISSUES_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/devel/issues)
file(GLOB PY_ISSUES_SCRIPTS "${PYMOOSE_ISSUES_DIRECTORY}/*.py" )
foreach(_test_script ${PY_ISSUES_SCRIPTS})
    get_filename_component( _test_name ${_test_script} NAME_WE)
    add_test(NAME ${_test_name}
        COMMAND ${PYTHON_EXECUTABLE} ${_test_script}
	CONFIGURATIONS Devel
	WORKING_DIRECTORY ${PYMOOSE_ISSUES_DIRECTORY}
        )
     set_tests_properties(${_test_name}
         PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/python"
        )
endforeach()

################################# COVERAGE ###################################

add_custom_target(coverage)
set(PYTEST_TEST_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
file(GLOB_RECURSE PYTEST_TEST_SCRIPTS ${PYTEST_TEST_DIRECTORY}/test_*.py)

foreach(_test_script ${PYTEST_TEST_SCRIPTS})
    get_filename_component(_name_we ${_test_script} NAME_WE)
    set(_test_name "pytest_${_name_we}")
    # message(STATUS "Adding test ${_test_name}")
    add_custom_target(${_test_name}
        # We collect coverage data but do not show report during each test.
        # We show report at the end of all tests. Set `--cov-report=` 
        COMMAND ${PYTHON_EXECUTABLE} -m pytest 
            --cov=moose --cov=rdesigneur --cov-append --cov-report= 
            ${_test_script}
        DEPENDS _moose
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Executing ${_test_script} using pytest"
        VERBATIM)
    add_dependencies(coverage  ${_test_name})
endforeach()

# Generate report at the end.
add_custom_command(TARGET coverage POST_BUILD
    COMMAND ${PYTHON_EXECUTABLE} -m coverage report 
    COMMAND ${PYTHON_EXECUTABLE} -m coverage html 
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Genearting coverage report"
    VERBATIM)


########################### RELEASE #########################################
set(PYMOOSE_SDIST_FILE ${CMAKE_BINARY_DIR}/pymoose-${VERSION_MOOSE}.tar.gz)
add_custom_target(sdist
    COMMAND ${PYTHON_EXECUTABLE} setup.py sdist --formats=gztar 
        -d ${CMAKE_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Creating sdist ${PYMOOSE_SDIST_FILE}" 
    VERBATIM)

add_custom_target(upload_test DEPENDS sdist
    COMMAND ${PYTHON_EXECUTABLE} -m twine upload 
        --user $ENV{PYMOOSE_PYPI_USER} --password $ENV{PYMOOSE_PYPI_PASSWORD}
        --repository-url https://test.pypi.org/legacy/
        ${PYMOOSE_SDIST_FILE}
    COMMENT "Uploading source distribution to test.pypi.org"
    VERBATIM)

add_custom_target(upload DEPENDS sdist
    COMMAND ${PYTHON_EXECUTABLE} -m twine upload 
        --user $ENV{PYMOOSE_PYPI_USER} --password $ENV{PYMOOSE_PYPI_PASSWORD}
        ${PYMOOSE_SDIST_FILE}
    COMMENT "Uploading source distribution to PyPI"
    VERBATIM)

####################### DEVELOPMENT ########################################

# PYLINT target.
set(PYSCRIPTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/python)
file(GLOB_RECURSE PY_SCRIPTS  "python/*.py")
add_custom_target(pylint)
foreach(_py_script ${PY_SCRIPTS})
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

