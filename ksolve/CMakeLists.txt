cmake_minimum_required(VERSION 2.6)
include(CheckIncludeFileCXX)

include_directories(../builtins ../basecode ../utility ../kinetics)
include_directories(../ )
include_directories(../mesh)
include_directories(../external/muparser/include )
IF(WITH_BOOST)
    check_include_file_cxx( ${Boost_INCLUDE_DIRS}/boost/numeric/odeint.hpp
        ODEINT_EXISTS)
    # If boost distribution does not have its own odeint library, use the
    # private one.
    if(NOT ODEINT_EXISTS)
        include_directories( ../external/odeint-v2/include )
    endif(NOT ODEINT_EXISTS)
    include_directories( ../external/boost-numeric-bindings )
elseif(WITH_GSL)
    include_directories( ${GSL_INCLUDE_DIRS} )
endif(WITH_BOOST)

if(PARALLELIZED_SOLVERS)
    message( STATUS "Parallel version of KSolve and Gsolve" )
    add_definitions( -DPARALLELIZE_KSOLVE_WITH_CPP11_ASYNC )
    add_definitions( -DPARALLELIZE_GSOLVE_WITH_CPP11_ASYNC )
endif(PARALLELIZED_SOLVERS)

set(KSOLVE_SRCS
	KinSparseMatrix.cpp
	ZombiePool.cpp
        ZombieFunction.cpp
        ZombieBufPool.cpp
	ZombieReac.cpp
	ZombieEnz.cpp
	ZombieMMenz.cpp
        VoxelPoolsBase.cpp
	VoxelPools.cpp
        GssaVoxelPools.cpp
	RateTerm.cpp
        FuncTerm.cpp
	Stoich.cpp
	Ksolve.cpp
        Gsolve.cpp
        ZombiePoolInterface.cpp
        testKsolve.cpp
    )

if(WITH_GSL)
    list(APPEND KSOLVE_SRCS SteadyStateGsl.cpp )
elseif(WITH_BOOST)
    list(APPEND KSOLVE_SRCS SteadyStateBoost.cpp )
    list(APPEND KSOLVE_SRCS BoostSys.cpp)
endif(WITH_GSL)

add_library( ksolve ${KSOLVE_SRCS} )
