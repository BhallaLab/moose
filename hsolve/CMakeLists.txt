cmake_minimum_required(VERSION 2.6)
include_directories(../basecode ../utility ../kinetics ../external/debug)
add_library(hsolve
    	HSolveStruct.cpp
	HinesMatrix.cpp
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

