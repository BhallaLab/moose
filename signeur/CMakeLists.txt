cmake_minimum_required(VERSION 2.8)
include( ${CMAKE_CURRENT_SOURCE_DIR}/../CheckCXXCompiler.cmake)
add_library(signeur
    Adaptor.cpp
    testSigNeur.cpp
    )
