cmake_minimum_required(VERSION 2.8)
include(CheckIncludeFileCXX)
include( ${CMAKE_CURRENT_SOURCE_DIR}/../CheckCXXCompiler.cmake)

if(WITH_BOOST)
    find_package(Boost 1.53 REQUIRED COMPONENTS thread)
    add_definitions(-DUSE_BOOST )
    set(WITH_BOOST_ODE ON)
    include_directories( ${Boost_INCLUDE_DIRS} )
    # This is still not part of official bindings.
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../external/boost-numeric-bindings)
endif()

# if boost ode is being used, don't use GSL.
if(WITH_BOOST_ODE)
    # This is still not part of official bindings.
    set(WITH_GSL OFF)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../external/boost-numeric-bindings)
    add_definitions(-DUSE_BOOST_ODE -UUSE_GSL)
    include_directories(${Boost_INCLUDE_DIRS})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../external/boost-numeric-bindings)
elseif(WITH_GSL)
    include_directories( ${GSL_INCLUDE_DIRS} )
    add_definitions(-DUSE_GSL -UUSE_BOOST_ODE -UUSE_BOOST)
endif(WITH_BOOST_ODE)

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
elseif(WITH_BOOST_ODE)
    list(APPEND KSOLVE_SRCS SteadyStateBoost.cpp )
endif(WITH_GSL)

add_library( ksolve ${KSOLVE_SRCS} )
target_link_libraries( ksolve ${Boost_LIBRARIES} )
