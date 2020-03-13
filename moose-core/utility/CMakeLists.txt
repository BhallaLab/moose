cmake_minimum_required(VERSION 2.8)
include( ${CMAKE_CURRENT_SOURCE_DIR}/../CheckCXXCompiler.cmake)

IF(WITH_BOOST)
include(CheckIncludeFiles)
check_include_files( ${Boost_INCLUDE_DIRS}/boost/random/random_device.hpp
    BOOST_RANDOM_DEVICE_EXISTS
    )
endif(WITH_BOOST)

add_library(utility
    strutil.cpp
    types.cpp
    setupenv.cpp
    numutil.cpp
    Annotator.cpp
    Vec.cpp
    cnpy.cpp
    fileutils.cpp
    utility.cpp
    )
