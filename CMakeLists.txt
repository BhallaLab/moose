CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(moose-all)
set(MOOSE_VERSION "3.0.2")

## intialize paths
set(MOOSE_CORE_DIR "${CMAKE_SOURCE_DIR}/moose-core")
set(MOOSE_CORE_BUILD_DIR ${CMAKE_BINARY_DIR}/_build_moose_core)
FILE(MAKE_DIRECTORY ${MOOSE_CORE_BUILD_DIR})
set(DATA_DIR ${CMAKE_SOURCE_DIR}/data)

set(OUTPUT_MOOSEBIN ${MOOSE_CORE_BUILD_DIR}/moose.bin)
set(OUTPUT_MOOSESDIST ${MOOSE_CORE_BUILD_DIR}/moose-${MOOSE_VERSION}.tar.gz)
add_custom_command(OUTPUT ${OUTPUT_MOOSEBIN} ${OUTPUT_MOOSESDIST}
    COMMAND ${CMAKE_COMMAND} 
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DDEBUG=OFF
        ${MOOSE_CORE_DIR}
    COMMAND $(MAKE)
    WORKING_DIRECTORY ${MOOSE_CORE_BUILD_DIR}
    VERBATIM
    )
add_custom_target(moose-all ALL
    DEPENDS ${OUTPUT_MOOSESDIST} ${OUTPUT_MOOSEBIN}
    )

install(CODE
    "
    execute_process(COMMAND ctest --output-on-failure
        WORKING_DIRECTORY ${MOOSE_CORE_BUILD_DIR})

    execute_process(COMMAND make install
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
    COMMAND xargs rm -rf < install_manifest.txt
    )

add_custom_command(OUTPUT  __uninstall_moose-gui__
    COMMAND ${CMAKE_COMMAND} -E remove_directory
    ${CMAKE_INSTALL_PREFIX}/lib/moose/gui
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_INSTALL_PREFIX}/bin/moosegui
    )
