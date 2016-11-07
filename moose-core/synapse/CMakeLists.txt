cmake_minimum_required(VERSION 2.6)
include_directories(../basecode ../utility ../kinetics)
include_directories( ../external/muparser/include/ )
add_library(synapse
    SynHandlerBase.cpp
    SimpleSynHandler.cpp
    STDPSynHandler.cpp
    GraupnerBrunel2012CaPlasticitySynHandler.cpp
    Synapse.cpp
    STDPSynapse.cpp
    RollingMatrix.cpp 
    SeqSynHandler.cpp
    testSynapse.cpp
    )
