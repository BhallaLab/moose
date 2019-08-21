cmake_minimum_required(VERSION 2.8)

add_executable( normal_dist
    ${CMAKE_CURRENT_SOURCE_DIR}/test_normal_dist.cpp
    )

add_library(randnum RNG.cpp)

enable_testing()
add_test( NAME test_normal_dist COMMAND $<TARGET_FILE:normal_dist> )
