CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(moose)

## Default install prefix. When building packages, such as on launchpad, we need
## to modify installation prefix.
if(DEVELOPER)
    option(CMAKE_INSTALL_PREFIX "Install prefix" ${CMAKE_BINARY_DIR}/_install)
else()
    option(CMAKE_INSTALL_PREFIX "Install prefix" /usr)
endif()

## intialize paths
set(MOOSE_CORE_DIR "${CMAKE_SOURCE_DIR}/moose-core")
set(MOOSE_CORE_BUILD_DIR ${CMAKE_BINARY_DIR}/_build_moose_core)
set(MOOSE_CORE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")
set(DATA_DIR ${CMAKE_SOURCE_DIR}/data)

include(ExternalProject)
message("[INFO] Installtion directory is ${CMAKE_INSTALL_PREFIX}")
ExternalProject_Add( moose-core
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/__moose-core
    SOURCE_DIR ${MOOSE_CORE_DIR}
    # Extremely useful in packaging
    #BINARY_DIR ${MOOSE_CORE_INSTALL_DIR}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    TEST_COMMAND ctest --output-on-failure
    TEST_BEFORE_INSTALL 1
    BUILD_IN_SOURCE 0
) 

## moose-gui
## TODO: moose-gui should be a python module.
set(MOOSE_GUI_DIR ${CMAKE_SOURCE_DIR}/moose-gui)
set(MOOSE_GUI_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share/moose/gui)
add_custom_target(moose-gui ALL)
add_custom_command(TARGET moose-gui
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${MOOSE_GUI_DIR} ${MOOSE_GUI_INSTALL_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${DATA_DIR}/moosegui ${CMAKE_INSTALL_PREFIX}/bin/
    )

