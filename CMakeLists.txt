cmake_minimum_required(VERSION 2.8)

project(moose)
if(POLICY CMP0048)
    cmake_policy(SET CMP0048 OLD )
endif(POLICY CMP0048)

# If from command line, version info is not passed, use the git to generate a
# version file. If GIT fails, use the previous known version.
find_program(GIT_EXEC "git")
message( STATUS "Looking for git ${GIT_EXEC}" )
if(VERSION)
    message( STATUS "MOOSE version is set to ${VERSION} at build time" )
elseif(GIT_EXEC)
    message( STATUS "Getting version by reading git tags" )
    execute_process( 
        COMMAND ${GIT_EXEC} describe --tags --long
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif( )
message( STATUS "Building version ${VERSION}" )

# Prefix
message( STATUS "CMAKE_INSTALL_PREFIX= ${CMAKE_INSTALL_PREFIX}" )

# Options to pass down to moose-core
option(WITH_DOC   "Build documentation"                         OFF)
option(DEBUG      "Build with DEBUG support"                    OFF)
option(WITH_BOOST "Use Boost libraries instead of GSL"          OFF)
option(WITH_CUDA  "Use CUDA/GPU"                                OFF)
option(WITH_MPI   "Use MPI (experimental)"                      OFF)
option(WITH_GUI   "Install moose-gui. Works only with python2." ON )

# Required packages.
find_package(PythonInterp REQUIRED)
if(PYTHON_VERSION_MAJOR VERSION_EQUAL 3)
    set(WITH_GUI OFF)
endif( )

add_custom_target(moose ALL)

## intialize paths
set(PYMOOSE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/moose-core")
set(PYMOOSE_BUILD_DIR ${CMAKE_BINARY_DIR}/__moose-core_build)
set(PYMOOSE_INSTALL_DIR ${CMAKE_BINARY_DIR}/__moose-core_install)
file(MAKE_DIRECTORY ${PYMOOSE_INSTALL_DIR})

if(WITH_GUI)
    set(MOOSE_GUI_DIR ${CMAKE_SOURCE_DIR}/moose-gui)
    set(MOOSE_GUI_INSTALL_DIR ${CMAKE_BINARY_DIR}/__moose-gui_install)
endif()

set(MOOSE_EXAMPLE_DIR ${CMAKE_SOURCE_DIR}/moose-examples)
set(MOOSE_EXAMPLE_INSTALL_DIR ${CMAKE_BINARY_DIR}/__moose-examples_install)

file(MAKE_DIRECTORY ${PYMOOSE_BUILD_DIR})

# Its a good target since we can not be sure of python module extension on
# different platform.
set(OUTPUT_MOOSEBIN ${PYMOOSE_BUILD_DIR}/moose.bin)

configure_file( 
    ${CMAKE_SOURCE_DIR}/cmake/build_moose-core.sh.in 
    ${PYMOOSE_BUILD_DIR}/build_moose-core.sh 
    )

# Build pymoose module, create bdist and  and unarchive the bdist to
# PYMOOSE_INSTALL_DIR.
add_custom_target( moose-core DEPENDS ${OUTPUT_MOOSEBIN} )
add_custom_command( OUTPUT ${OUTPUT_MOOSEBIN}
    COMMAND ${PYMOOSE_BUILD_DIR}/build_moose-core.sh
    WORKING_DIRECTORY ${PYMOOSE_BUILD_DIR}
    COMMENT "Building pymoose in ${PYMOOSE_BUILD_DIR}"
    VERBATIM
    )
add_custom_command( TARGET moose-core POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E chdir ${PYMOOSE_INSTALL_DIR} tar xvf
        ${PYMOOSE_BUILD_DIR}/bdist/pymoose-${VERSION}.CMAKE.tar.gz
    COMMENT "Unarchiving pymoose bdist"
    VERBATIM
    )

if(WITH_GUI)
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
endif()

add_dependencies(moose moose-core)

if(WITH_GUI)
    add_dependencies(moose gui examples)
    add_dependencies(examples  gui)
endif()



install(DIRECTORY ${PYMOOSE_INSTALL_DIR}/usr/
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    PATTERN ".git" EXCLUDE
    PATTERN "*.pyc" EXCLUDE
    )

if(WITH_GUI)
    # moose-gui
    set(MOOSE_GUI_DIR ${CMAKE_SOURCE_DIR}/moose-gui)
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

    configure_file( ${CMAKE_SOURCE_DIR}/cmake/moosegui.in ${CMAKE_BINARY_DIR}/moosegui )

    install(PROGRAMS ${CMAKE_BINARY_DIR}/moosegui DESTINATION bin)
endif()

## UNINSTALL
add_custom_target(uninstall DEPENDS __uninstall_moose-core__ )

add_custom_command(OUTPUT __uninstall_moose-core__
    COMMAND xargs rm -rf < install_manifest.txt
    COMMAND pip uninstall -y moose 
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_INSTALL_PREFIX}/bin/moosegui
    )

if(WITH_GUI)
    add_custom_target( __uninstall_moose-gui__ )
    add_custom_command(TARGET __uninstall_moose-gui__ 
        COMMAND ${CMAKE_COMMAND} -E remove_directory
        ${CMAKE_INSTALL_PREFIX}/lib/moose/gui
        COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_INSTALL_PREFIX}/bin/moosegui
        )
    add_dependencies( uninstall __uninstall_moose-gui__ )
endif()
