CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(moose)

## Default install prefix. When building packages, such as on launchpad, we need
## to modify installation prefix.
if(DEVELOPER)
    option(CMAKE_INSTALL_PREFIX "Install prefix" ${CMAKE_BINARY_DIR}/_install)
else()
    option(CMAKE_INSTALL_PREFIX "Install prefix" /usr)
endif()

include(ExternalProject)
message("[INFO] Installtion directory is ${CMAKE_INSTALL_PREFIX}")
ExternalProject_Add( moose-core
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/_moose-core
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/moose-core
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    TEST_BEFORE_INSTALL 1
    BUILD_IN_SOURCE 0
) 

## moose-gui

set(MOOSE_GUI_DIR ${CMAKE_SOURCE_DIR}/moose-gui)
add_custom_target(moose-gui ALL)
add_custom_command(TARGET moose-gui
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${MOOSE_GUI_DIR} 
    ${CMAKE_INSTALL_PREFIX}/share/moose/gui
    )
