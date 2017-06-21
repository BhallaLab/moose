cmake_minimum_required(VERSION 2.6)
include_directories(../basecode ../utility ../ksolve)
include_directories(${GSL_INCLUDE_DIRS})
add_library(diffusion
	FastMatrixElim.cpp
	DiffPoolVec.cpp
	Dsolve.cpp
        testDiffusion.cpp
    )
