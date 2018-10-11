cmake_minimum_required(VERSION 2.8)
include( ${CMAKE_CURRENT_SOURCE_DIR}/../CheckCXXCompiler.cmake)
add_library(hsolve
    	HSolveStruct.cpp
	HinesMatrix.cpp
        Cell.cpp
	HSolvePassive.cpp
	RateLookup.cpp
	HSolveActive.cpp
	HSolveActiveSetup.cpp
	HSolveInterface.cpp
	HSolve.cpp
	HSolveUtils.cpp
	testHSolve.cpp
	ZombieCompartment.cpp
	ZombieCaConc.cpp
	ZombieHHChannel.cpp
)

