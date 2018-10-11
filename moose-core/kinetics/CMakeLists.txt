cmake_minimum_required(VERSION 2.8)
include( ${CMAKE_CURRENT_SOURCE_DIR}/../CheckCXXCompiler.cmake)
add_library(kinetics
    PoolBase.cpp
    Pool.cpp
    BufPool.cpp
    ReacBase.cpp
    Reac.cpp
    EnzBase.cpp
    CplxEnzBase.cpp
    Enz.cpp
    MMenz.cpp
    Species.cpp
    ConcChan.cpp
    ReadKkit.cpp
    WriteKkit.cpp
    ReadCspace.cpp
    lookupVolumeFromMesh.cpp
    testKinetics.cpp
    )
