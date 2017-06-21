#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment.
#**           copyright (C) 2007 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU Lesser General Public License version 2.1
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/

TARGET = _kinetics.o

OBJ = \
	PoolBase.o	\
	Pool.o	\
	BufPool.o	\
	ReacBase.o	\
	Reac.o	\
	EnzBase.o	\
	CplxEnzBase.o	\
	Enz.o	\
	MMenz.o	\
	Species.o	\
	ReadKkit.o	\
	WriteKkit.o	\
	ReadCspace.o	\
	lookupVolumeFromMesh.o	\
	testKinetics.o	\

HEADERS = \
	../basecode/header.h \

default: $(TARGET)

$(OBJ)	: $(HEADERS)
Pool.o:	PoolBase.h Pool.h lookupVolumeFromMesh.h
BufPool.o:	PoolBase.h Pool.h BufPool.h lookupVolumeFromMesh.h
ReacBase.o:	ReacBase.h lookupVolumeFromMesh.h
Reac.o:	ReacBase.h Reac.h lookupVolumeFromMesh.h
EnzBase.o:	EnzBase.h lookupVolumeFromMesh.h
CplxEnzBase.o:	EnzBase.h CplxEnzBase.h lookupVolumeFromMesh.h
Enz.o:	EnzBase.h CplxEnzBase.h Enz.h lookupVolumeFromMesh.h
MMenz.o:	EnzBase.h MMenz.h  lookupVolumeFromMesh.h
Boundary.o:	Boundary.h
Species.o:	Species.h
ReadKkit.o:	lookupVolumeFromMesh.h ReadKkit.h PoolBase.h Pool.h ../shell/Shell.h 
WriteKkit.o:	lookupVolumeFromMesh.h PoolBase.h
ReadCspace.o:	lookupVolumeFromMesh.h ReadCspace.h ../shell/Shell.h
lookupVolumeFromMesh.o: lookupVolumeFromMesh.h
testKinetics.o:	ReadKkit.h

.cpp.o:
	$(CXX) $(CXXFLAGS) $(SMOLDYN_FLAGS) -I.. -I../basecode -I../msg $< -c

$(TARGET):		$(OBJ) $(SMOLDYN_OBJ) $(HEADERS)
	$(LD) -r -o $(TARGET) $(OBJ) $(SMOLDYN_OBJ) $(SMOLDYN_LIB_PATH) $(SMOLDYN_LIBS) 

clean:
	-rm -f *.o $(TARGET) core core.*
