cmake_minimum_required(VERSION 2.8)
include(${CMAKE_CURRENT_SOURCE_DIR}/../CheckCXXCompiler.cmake)

if(WITH_GSL)
    find_package(GSL 1.16)
    include_directories(${GSL_INCLUDE_DIRS})
elseif(WITH_BOOST_ODE)
    find_package(Boost)
    include_directories(${Boost_INCLUDE_DIRS})
endif(WITH_GSL)

set(BIOPHYSICS_SRCS
    IntFire.cpp
    SpikeGen.cpp
    RandSpike.cpp
    CompartmentDataHolder.cpp
    CompartmentBase.cpp
    Compartment.cpp
    SymCompartment.cpp
    GapJunction.cpp
    ChanBase.cpp
    ChanCommon.cpp
    HHChannel.cpp
    HHChannelBase.cpp
    HHChannel2D.cpp
    HHGate.cpp
    HHGate2D.cpp
    HHChannel2D.cpp
    CaConcBase.cpp
    CaConc.cpp
    MgBlock.cpp
    Nernst.cpp
    Neuron.cpp
    ReadCell.cpp
    SwcSegment.cpp
    ReadSwc.cpp
    SynChan.cpp
    NMDAChan.cpp
    IzhikevichNrn.cpp
    DifShellBase.cpp
    DifShell.cpp
    DifBufferBase.cpp
    DifBuffer.cpp
    MMPump.cpp
    Leakage.cpp
    VectorTable.cpp
    MarkovRateTable.cpp
    MarkovChannel.cpp
    MatrixOps.cpp
    MarkovSolverBase.cpp
    MarkovSolver.cpp
    VClamp.cpp
    Spine.cpp
    MarkovOdeSolver.cpp
    testBiophysics.cpp
    )

add_library(biophysics ${BIOPHYSICS_SRCS})

