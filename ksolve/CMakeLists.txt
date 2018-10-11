cmake_minimum_required(VERSION 2.8)
include(CheckIncludeFileCXX)
include( ${CMAKE_CURRENT_SOURCE_DIR}/../CheckCXXCompiler.cmake)

if(WITH_BOOST)
    find_package(Boost REQUIRED COMPONENTS thread)
    add_definitions( -DUSE_BOOST_ASYNC )
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
        add_definitions( -UBOOST_UBLAS_TYPE_CHECK )
    else()
        add_definitions( -DBOOST_UBLAS_TYPE_CHECK )
    endif()
    set(WITH_BOOST_ODE ON)
    include_directories( ${Boost_INCLUDE_DIRS} )
endif()

IF(WITH_BOOST_ODE)
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
