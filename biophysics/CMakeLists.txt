include_directories(../basecode ../synapse ../utility ../)
include_directories(../external/muparser/include)

if(WITH_GSL)
    include_directories(${GSL_INCLUDE_DIRS})
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
	testBiophysics.cpp
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
    )

if(WITH_GSL)
    list(APPEND BIOPHYSICS_SRCS MarkovGslSolver.cpp)
endif(WITH_GSL)

add_library( biophysics ${BIOPHYSICS_SRCS} )
