#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment.
#**           copyright (C) 2007 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU Lesser General Public License version 2.1
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/

TARGET = _diffusion.o

OBJ = \
	FastMatrixElim.o	\
	DiffPoolVec.o	\
	Dsolve.o	\
	testDiffusion.o	\

HEADERS = \
	../basecode/header.h \

default: $(TARGET)

$(OBJ)	: $(HEADERS)
FastMatrixElim.o: ../basecode/SparseMatrix.h FastMatrixElim.h
Dsolve.o:	../basecode/SparseMatrix.h ../kinetics/PoolBase.h ../kinetics/lookupVolumeFromMesh.h ../mesh/ChemCompt.h ../ksolve/XferInfo.h ../ksolve/ZombiePoolInterface.h 
DiffPoolVec.o: DiffPoolVec.h ../ksolve/ZombiePoolInterface.h
testDiffusion.o:	Dsolve.h ../ksolve/ZombiePoolInterface.h

.cpp.o:
	$(CXX) $(CXXFLAGS) $(GSL_FLAGS) $(SMOLDYN_FLAGS) -I.. -I../basecode -I../ksolve $< -c

$(TARGET):		$(OBJ) $(GSL_OBJ) $(SMOLDYN_OBJ) $(HEADERS)
	$(LD) -r -o $(TARGET) $(OBJ) $(GSL_OBJ) $(GSL_LIBS) $(SMOLDYN_OBJ) $(SMOLDYN_LIB_PATH) $(SMOLDYN_LIBS) 

clean:
	-rm -f *.o $(TARGET) core core.*
