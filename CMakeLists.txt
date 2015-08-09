CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(moose-all)

## intialize paths
set(MOOSE_CORE_DIR "${CMAKE_SOURCE_DIR}/moose-core")
set(MOOSE_CORE_BUILD_DIR ${CMAKE_BINARY_DIR}/_build_moose_core)
FILE(MAKE_DIRECTORY ${MOOSE_CORE_BUILD_DIR})
set(DATA_DIR ${CMAKE_SOURCE_DIR}/data)

add_custom_command(OUTPUT __moose_core_built__
    COMMAND ${CMAKE_COMMAND} 
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DDEBUG=OFF
        ${MOOSE_CORE_DIR}
    COMMAND $(MAKE)
    WORKING_DIRECTORY ${MOOSE_CORE_BUILD_DIR}
    VERBATIM
    )

add_custom_target(moose-all ALL
    DEPENDS __moose_core_built__
    )

install(CODE
    "
    execute_process(COMMAND ctest --output-on-failure
        WORKING_DIRECTORY ${MOOSE_CORE_BUILD_DIR})

    execute_process(COMMAND ${CMAKE_COMMAND} install
        WORKING_DIRECTORY ${MOOSE_CORE_BUILD_DIR})
    "
    )

## moose-gui
## TODO: moose-gui should be a python module.
set(MOOSE_GUI_DIR ${CMAKE_SOURCE_DIR}/moose-gui)

install(DIRECTORY ${MOOSE_GUI_DIR}
    DESTINATION lib/moose/gui
    PATTERN ".git" EXCLUDE
    )

install(PROGRAMS ${DATA_DIR}/moosegui
    DESTINATION bin
    )

## UNINSTALL
add_custom_target(uninstall 
    DEPENDS __uninstall_moose-core__ __uninstall_moose-gui__
    )

add_custom_command(OUTPUT __uninstall_moose-core__
    COMMAND make uninstall
    WORKING_DIRECTORY ${MOOSE_CORE_BUILD_DIR}
    )

add_custom_command(OUTPUT  __uninstall_moose-gui__
    COMMAND ${CMAKE_COMMAND} -E remove_directory
    ${CMAKE_INSTALL_PREFIX}/lib/moose
    COMMAND ${CMAKE_COMMAND} -R remove ${CMAKE_INSTALL_PREFIX}/bin/moosegui
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
