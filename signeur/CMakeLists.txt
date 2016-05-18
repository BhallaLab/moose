cmake_minimum_required(VERSION 2.6)
include_directories(../basecode ../utility ../kinetics)
add_library(signeur
    Adaptor.cpp
    testSigNeur.cpp
    )
