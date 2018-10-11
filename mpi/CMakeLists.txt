cmake_minimum_required(VERSION 2.8)
include( ${CMAKE_CURRENT_SOURCE_DIR}/../CheckCXXCompiler.cmake)
IF(USE_MPI)
    find_package(MPI REQUIRED)
    include_directories(MPI_INCLUDE_PATH)
    ADD_DEFINITIONS(-DUSE_MPI)
ENDIF(USE_MPI)

add_library(moose_mpi
    PostMaster.cpp
    testMpi.cpp
    )

