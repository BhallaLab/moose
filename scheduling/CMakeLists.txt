cmake_minimum_required(VERSION 2.8)
include( ${CMAKE_CURRENT_SOURCE_DIR}/../CheckCXXCompiler.cmake)
file(GLOB scheduling_SRC  "*.cpp" )
if(NOTIFY_PROGRESS)
    add_definitions("-DNOTIFY_PROGRESS")
endif(NOTIFY_PROGRESS)

if(PARALLELIZED_CLOCK)
    message( STATUS "Enabling high-level parallelization of moose::Clock" )
    add_definitions( -DPARALLELIZE_CLOCK_USING_CPP11_ASYNC )
endif(PARALLELIZED_CLOCK)

add_library(scheduling ${scheduling_SRC})
