cmake_minimum_required(VERSION 2.8)
include( ${CMAKE_CURRENT_SOURCE_DIR}/../CheckCXXCompiler.cmake)
add_library(msg
    Msg.cpp
    DiagonalMsg.cpp
    OneToAllMsg.cpp
    OneToOneMsg.cpp
    SingleMsg.cpp
    SparseMsg.cpp
    OneToOneDataIndexMsg.cpp
    testMsg.cpp
    )
