CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(MOOSE)
set(MOOSE_VERSION "3.1.1")
include( ExternalProject )


# Options to pass down to moose-core
option(WITH_DOC "Build documentation" OFF)
option(DEBUG "Build with DEBUG support" OFF)
option(WITH_BOOST "Use Boost libraries instead of GSL" OFF)
option(WITH_CUDA "Use CUDA/GPU" OFF)
option(WITH_MPI "Use MPI" OFF)

## intialize paths
set(MOOSE_CORE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/moose-core")
set(MOOSE_CORE_BUILD_DIR ${CMAKE_BINARY_DIR}/_build_moose_core)
set(MOOSE_CORE_INSTALL_DIR ${CMAKE_BINARY_DIR}/_install_moose_core)
file(MAKE_DIRECTORY ${MOOSE_CORE_BUILD_DIR})
set(DATA_DIR ${CMAKE_SOURCE_DIR}/data)

set(OUTPUT_MOOSEBIN ${MOOSE_CORE_BUILD_DIR}/moose.bin)

configure_file( ${CMAKE_SOURCE_DIR}/build_moose_core.sh.in 
    ${CMAKE_BINARY_DIR}/build_moose_core.sh )

add_custom_target( moose-core ALL DEPENDS ${OUTPUT_MOOSEBIN} )
add_custom_command( OUTPUT ${OUTPUT_MOOSEBIN}
    COMMAND MAKE=$(MAKE) bash -c ${CMAKE_BINARY_DIR}/build_moose_core.sh
    VERBATIM
    )

find_package(PythonInterp REQUIRED)

## moose-gui
## TODO: moose-gui should be a python module.
set(MOOSE_GUI_DIR ${CMAKE_SOURCE_DIR}/moose-gui)

install(DIRECTORY ${MOOSE_CORE_INSTALL_DIR}/
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    PATTERN ".git" EXCLUDE
    )

install(DIRECTORY ${MOOSE_GUI_DIR}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/moose/gui
    PATTERN ".git" EXCLUDE
    )

configure_file( ${CMAKE_SOURCE_DIR}/cmake/moose.in ${CMAKE_BINARY_DIR}/moose )
install(PROGRAMS ${CMAKE_BINARY_DIR}/moose
    DESTINATION bin
    )

## UNINSTALL
add_custom_target(uninstall 
    DEPENDS __uninstall_moose-core__ __uninstall_moose-gui__
    )

add_custom_command(OUTPUT __uninstall_moose-core__
    COMMAND xargs rm -rf < install_manifest.txt
    COMMAND pip uninstall -y moose 
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_INSTALL_PREFIX}/bin/moose.bin
    )

add_custom_command(OUTPUT  __uninstall_moose-gui__
    COMMAND ${CMAKE_COMMAND} -E remove_directory
    ${CMAKE_INSTALL_PREFIX}/lib/moose/gui
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_INSTALL_PREFIX}/bin/moosegui
    )

################################################################################
# CTEST 
#################################################################################
enable_testing( )
add_test( NAME test_pymoose_sanity COMMAND ${PYTHON_EXECUTABLE} -c "import moose")
add_test( NAME test_moogli_sanity COMMAND ${PYTHON_EXECUTABLE} -c "import moogli")

set_tests_properties( test_pymoose_sanity test_moogli_sanity  
    PROPERTIES ENVIRONMENT "PYTHONPATH=${MOOSE_CORE_BUILD_DIR}/python"
    )
