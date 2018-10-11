cmake_minimum_required(VERSION 2.8)
include( ${CMAKE_CURRENT_SOURCE_DIR}/../CheckCXXCompiler.cmake)

set( SYNAPSE_SRCS
    GraupnerBrunel2012CaPlasticitySynHandler.cpp
    RollingMatrix.cpp
    SeqSynHandler.cpp
    SimpleSynHandler.cpp
    STDPSynapse.cpp
    STDPSynHandler.cpp
    Synapse.cpp
    SynHandlerBase.cpp
    testSynapse.cpp
    )

add_library(synapse ${SYNAPSE_SRCS} )
