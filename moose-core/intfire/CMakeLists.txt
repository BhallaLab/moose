cmake_minimum_required(VERSION 2.6)
INCLUDE_DIRECTORIES(../basecode)

ADD_LIBRARY(intfire
    AdExIF.cpp
    AdThreshIF.cpp
    ExIF.cpp
    IntFireBase.cpp
    IzhIF.cpp
    LIF.cpp
    QIF.cpp
    testIntFire.cpp
    )

