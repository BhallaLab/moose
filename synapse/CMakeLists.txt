cmake_minimum_required(VERSION 2.6)
include_directories(../basecode ../utility ../kinetics)
add_library(synapse
    SimpleSynHandler.cpp
    SynHandlerBase.cpp
    STDPSynHandler.cpp
    GraupnerBrunel2012CaPlasticitySynHandler.cpp
    Synapse.cpp
    STDPSynapse.cpp
    testSynapse.cpp
    )
