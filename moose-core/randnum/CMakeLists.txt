cmake_minimum_required(VERSION 2.8)

if( ${CMAKE_SOURCE_DIR} EQUAL ${CMAKE_CURRENT_SOURCE_DIR} )
    add_definitions( -std=c++11 -g -Wall )
endif()

add_executable( normal_dist
    ${CMAKE_CURRENT_SOURCE_DIR}/test_normal_dist.cpp)

enable_testing()
add_test( NAME test_normal_dist COMMAND $<TARGET_FILE:normal_dist> )
