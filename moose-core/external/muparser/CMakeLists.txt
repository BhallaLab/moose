cmake_minimum_required(VERSION 2.8)

file(GLOB files_SRC "src/*.cpp")

include_directories(../msg)
include_directories(../basecode ./include)

add_library(muparser ${files_SRC})
set_target_properties( muparser
    PROPERTIES COMPILE_FLAGS "-Wno-switch -Wno-unknown-pragmas"
    )
