CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(MOOSE)
set(MOOSE_VERSION "3.1.1")

# Options to pass down to moose-core
option(WITH_DOC "Build documentation" OFF)
option(DEBUG "Build with DEBUG support" OFF)
option(WITH_BOOST "Use Boost libraries instead of GSL" OFF)
option(WITH_CUDA "Use CUDA/GPU" OFF)
option(WITH_MPI "Use MPI" OFF)

# Required packages.
find_package(PythonInterp REQUIRED)

add_custom_target(moose ALL)

## intialize paths
set(PYMOOSE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/moose-core")
set(PYMOOSE_BUILD_DIR ${CMAKE_BINARY_DIR}/__moose-core_build)
set(PYMOOSE_INSTALL_DIR ${CMAKE_BINARY_DIR}/__moose-core_install)

set(MOOSE_GUI_DIR ${CMAKE_SOURCE_DIR}/moose-gui)
set(MOOSE_GUI_INSTALL_DIR ${CMAKE_BINARY_DIR}/__moose-gui_install)

set(MOOSE_EXAMPLE_DIR ${CMAKE_SOURCE_DIR}/moose-examples)
set(MOOSE_EXAMPLE_INSTALL_DIR ${CMAKE_BINARY_DIR}/__moose-examples_install)

file(MAKE_DIRECTORY ${PYMOOSE_BUILD_DIR})

# Its a good target since we can not be sure of python module extension on
# different platform.
set(OUTPUT_MOOSEBIN ${PYMOOSE_BUILD_DIR}/moose.bin)

configure_file( ${CMAKE_SOURCE_DIR}/build_moose-core.sh.in 
    ${CMAKE_BINARY_DIR}/build_moose-core.sh )

# Build pymoose module.
add_custom_target( moose-core ALL DEPENDS ${OUTPUT_MOOSEBIN} )
add_custom_command( OUTPUT ${OUTPUT_MOOSEBIN}
    COMMAND MAKE=$(MAKE) bash -c ${CMAKE_BINARY_DIR}/build_moose-core.sh
    VERBATIM
    )

# Now build moose-gui and moose-examples.
# Make sure each script is compilable. Else report an error.
add_custom_target( gui ALL 
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${MOOSE_GUI_DIR} ${MOOSE_GUI_INSTALL_DIR} 
    COMMAND ${PYTHON_EXECUTABLE} -m compileall -q ${MOOSE_GUI_INSTALL_DIR}
    COMMENT "Building moose-gui"
    VERBATIM
    )

add_custom_target( examples ALL 
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${MOOSE_EXAMPLE_DIR} ${MOOSE_EXAMPLE_INSTALL_DIR}
    COMMAND ${PYTHON_EXECUTABLE} -m compileall -q ${MOOSE_EXAMPLE_INSTALL_DIR}
    VERBATIM
    )

add_dependencies(moose moose-core gui examples)
add_dependencies(examples  gui)


## moose-gui
## TODO: moose-gui should be a python module.
set(MOOSE_GUI_DIR ${CMAKE_SOURCE_DIR}/moose-gui)

install(DIRECTORY ${PYMOOSE_INSTALL_DIR}/
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    PATTERN ".git" EXCLUDE
    PATTERN "*.pyc" EXCLUDE
    )

install(DIRECTORY ${MOOSE_GUI_INSTALL_DIR}/
    DESTINATION lib/moose/gui
    PATTERN ".git*" EXCLUDE
    PATTERN "*.pyc" EXCLUDE
    )

install(DIRECTORY ${MOOSE_EXAMPLE_DIR}/
    DESTINATION lib/moose/moose-examples
    PATTERN ".git*" EXCLUDE 
    PATTERN "_travis*" EXCLUDE
    PATTERN ".travis*" EXCLUDE
    PATTERN "*.pyc" EXCLUDE
    )

configure_file( ${CMAKE_SOURCE_DIR}/cmake/moose.in ${CMAKE_BINARY_DIR}/moose )
install(PROGRAMS ${CMAKE_BINARY_DIR}/moose DESTINATION bin)

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
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_INSTALL_PREFIX}/bin/moose
    )

################################################################################
# CTEST 
#################################################################################
enable_testing( )
add_test( NAME test_pymoose_sanity 
    COMMAND ${PYTHON_EXECUTABLE} -c "import moose"
    )
add_test( NAME test_gui_sanity 
    COMMAND ${PYTHON_EXECUTABLE} ${MOOSE_GUI_INSTALL_DIR}/mgui.py 
    WORKING_DIRECTORY ${MOOSE_GUI_INSTALL_DIR}
    )

set_tests_properties( test_pymoose_sanity 
    PROPERTIES ENVIRONMENT "PYTHONPATH=${PYMOOSE_BUILD_DIR}/python"
    )
