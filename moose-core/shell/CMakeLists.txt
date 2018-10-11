cmake_minimum_required(VERSION 2.8)
include( ${CMAKE_CURRENT_SOURCE_DIR}/../CheckCXXCompiler.cmake)

if(LIBSBML_FOUND)
    add_definitions(-DUSE_SBML)
endif()

add_library(shell
    Shell.cpp
    ShellCopy.cpp
    ShellThreads.cpp
    LoadModels.cpp
    SaveModels.cpp
    Neutral.cpp
    Wildcard.cpp
    testShell.cpp
    )

## version is set by top-level script ../CMakeLists.txt .
#if(NOT MOOSE_VERSION)
#    set(MOOSE_VERSION "undefined")
#endif()
#
#set_target_properties(shell
#    PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS} -DMOOSE_VERSION=\"${MOOSE_VERSION}\"
#    )
