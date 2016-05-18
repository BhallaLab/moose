CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(MOOSE)
set(MOOSE_VERSION "3.0.2")
include( ExternalProject )


# Options to pass down to moose-core
option(WITH_DOC "Build documentation" OFF)
option(DEBUG "Build with DEBUG support" OFF)
option(WITH_BOOST "Use Boost libraries instead of GSL" OFF)
option(WITH_CUSA "Use CUDA/GPU" OFF)
option(WITH_MPI "Use MPI" OFF)

## intialize paths
set(MOOSE_CORE_DIR "${CMAKE_SOURCE_DIR}/moose-core")
set(MOOSE_CORE_BUILD_DIR ${CMAKE_BINARY_DIR}/_build_moose_core)
set(MOOSE_CORE_INSTALL_DIR ${CMAKE_BINARY_DIR}/_install_moose_core)
FILE(MAKE_DIRECTORY ${MOOSE_CORE_BUILD_DIR})
set(DATA_DIR ${CMAKE_SOURCE_DIR}/data)

set(OUTPUT_MOOSEBIN ${MOOSE_CORE_BUILD_DIR}/moose.bin)
set(OUTPUT_MOOSESDIST ${MOOSE_CORE_BUILD_DIR}/moose-${MOOSE_VERSION}.tar.gz)

find_package(PythonInterp REQUIRED)
include(ExternalProject)
ExternalProject_Add(
   moose-core
   PREFIX ${CMAKE_CURRENT_BINARY_DIR}
   SOURCE_DIR  ${CMAKE_SOURCE_DIR}/moose-core
   BINARY_DIR ${MOOSE_CORE_BUILD_DIR}
   CMAKE_ARGS -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} 
       -DCMAKE_INSTALL_PREFIX=${MOOSE_CORE_INSTALL_DIR}
       -DPYTHON_EXECUTABLE=${PYTHON_EXECUTABLE}
       -DDEBUG=${DEBUG}
       -DWITH_DOC=${WITH_DOC}
       -DWITH_BOOST=${WITH_BOOST}
       -DWITH_MPI=${WITH_MPI}
       -DWITH_CUDA=${WITH_CUDA}
   TEST_COMMAND ""     # No tests
) 

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}}/moose-core/include)
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}}/moose-core/lib)

## moose-gui
## TODO: moose-gui should be a python module.
set(MOOSE_GUI_DIR ${CMAKE_SOURCE_DIR}/moose-gui)

install(DIRECTORY ${MOOSE_CORE_INSTALL_DIR}/
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    )

install(DIRECTORY ${MOOSE_GUI_DIR}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/moose/gui
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
    COMMAND pip uninstall -y moose 
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_INSTALL_PREFIX}/bin/moose.bin
    )

add_custom_command(OUTPUT  __uninstall_moose-gui__
    COMMAND ${CMAKE_COMMAND} -E remove_directory
    ${CMAKE_INSTALL_PREFIX}/lib/moose/gui
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_INSTALL_PREFIX}/bin/moosegui
    )
