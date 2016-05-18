#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment.
#**           copyright (C) 2007 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU Lesser General Public License version 2.1
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/

TARGET = _biophysics.o

OBJ = \
	IntFire.o	\
	SpikeGen.o	\
	RandSpike.o	\
	CompartmentDataHolder.o	\
	CompartmentBase.o	\
	Compartment.o	\
	SymCompartment.o	\
	GapJunction.o	\
	ChanBase.o	\
	ChanCommon.o	\
	HHChannelBase.o	\
	HHChannel.o	\
	HHGate.o	\
	HHGate2D.o	\
	HHChannel2D.o	\
	CaConcBase.o	\
	CaConc.o	\
	MgBlock.o	\
	Nernst.o	\
	Neuron.o	\
	Spine.o	\
	ReadCell.o	\
	SwcSegment.o	\
	ReadSwc.o	\
	SynChan.o	\
	NMDAChan.o	\
	testBiophysics.o	\
	IzhikevichNrn.o	\
	DifShell.o	\
	Leakage.o	\
	VectorTable.o	\
	MarkovRateTable.o	\
	MarkovChannel.o	\
	MarkovGslSolver.o	\
	MatrixOps.o	\
	MarkovSolverBase.o	\
	MarkovSolver.o	\
	VClamp.o	\


	# LeakyIaF.o	\
	# GapJunction.o	\
	# Nernst.o	\
	# GHK.o		\
	# NMDAChan.o	\

# GSL_LIBS = -L/usr/lib -lgsl

HEADERS = \
	../basecode/header.h \
	../utility/numutil.h \


default: $(TARGET)

$(OBJ)	: $(HEADERS)
IntFire.o:	IntFire.h
SpikeGen.o: SpikeGen.h
RandSpike.o: RandSpike.h ../randnum/randnum.h
CompartmentDataHolder.o: CompartmentDataHolder.h
CompartmentBase.o: CompartmentBase.h CompartmentDataHolder.h
Compartment.o: CompartmentBase.h Compartment.h
SymCompartment.o: CompartmentBase.h Compartment.h SymCompartment.h
ChanBase.o: ChanBase.h
ChanCommon.o: ChanBase.h ChanCommon.h
SynChan.o: SynChan.h ChanBase.h ChanCommon.h
NMDAChan.o: NMDAChan.h SynChan.h ChanBase.h ChanCommon.h
Leakage.o: Leakage.h ChanBase.h ChanCommon.h
GapJunction.o: GapJunction.h
HHChannelBase.o: ChanBase.h HHChannelBase.h HHGate.h
HHChannel.o: ChanBase.h HHChannel.h HHGate.h
HHGate.o: HHGate.h
HHChannel2D.o: ChanBase.h HHChannel2D.h HHGate2D.h
HHGate2D.o: HHGate2D.h
CaConcBase.o:	CaConcBase.h
CaConc.o:	CaConcBase.h CaConc.h
MgBlock.o:	MgBlock.h ChanBase.h
Nernst.o:	Nernst.h
Neuron.o:	Neuron.h SwcSegment.h Spine.h
Spine.o:	Neuron.h SwcSegment.h Spine.h
ReadCell.o: CompartmentBase.h Compartment.h SymCompartment.h ReadCell.h ../shell/Shell.h ../utility/utility.h
SwcSegment.o: SwcSegment.h ../utility/Vec.h
ReadSwc.o: CompartmentBase.h Compartment.h SymCompartment.h SwcSegment.h ReadSwc.h ../shell/Shell.h ../utility/Vec.h
IzhikevichNrn.o: IzhikevichNrn.h
DifShell.o: DifShell.h
testBiophysics.o: IntFire.h CompartmentBase.h Compartment.h HHChannel.h HHGate.h 
VectorTable.o : VectorTable.h
MarkovGslSolver.o : MarkovGslSolver.h 
MatrixOps.o : MatrixOps.h
MarkovRateTable.o : VectorTable.h ../builtins/Interpol2D.h MarkovRateTable.h 
MarkovSolverBase.o : MarkovSolver.h MatrixOps.h MarkovRateTable.h
MarkovSolver.o : MarkovSolverBase.h MarkovSolver.h
MarkovChannel.o : VectorTable.h ../builtins/Interpol2D.h MarkovRateTable.h ChanBase.h MarkovChannel.h 
VClamp.o: VClamp.h

.cpp.o:
	$(CXX) $(CXXFLAGS) $(SMOLDYN_FLAGS) -I. -I../external/muparser/include -I .. -I../basecode -I../msg $< -c

$(TARGET): $(OBJ) $(SMOLDYN_OBJ) $(HEADERS)
	$(LD) -r -o $(TARGET) $(OBJ) $(SMOLDYN_OBJ) $(SMOLDYN_LIB_PATH) $(SMOLDYN_LIBS) $(GSL_LIBS)

clean:
	rm -f *.o $(TARGET) core core.*
