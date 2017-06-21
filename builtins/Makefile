#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment.
#**           copyright (C) 2007 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU Lesser General Public License version 2.1
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/

TARGET = _builtins.o

OBJ = \
      Arith.o	\
      Group.o	\
      Mstring.o	\
      Func.o \
      Function.o	\
      Variable.o	\
      InputVariable.o	\
      TableBase.o	\
      Table.o	\
      Interpol.o	\
      StimulusTable.o	\
      TimeTable.o	\
      Stats.o	\
      SpikeStats.o	\
      Interpol2D.o \
      HDF5WriterBase.o	\
      HDF5DataWriter.o	\
      StreamerBase.o \
      Streamer.o \
      NSDFWriter.o	\
      testNSDF.o	\
      testBuiltins.o	\

HEADERS = \
	  ../basecode/header.h \


default: $(TARGET)

$(OBJ)	: $(HEADERS)
Arith.o:	Arith.h
Group.o:	Group.h
Mstring.o:		Mstring.h
Func.o:	Func.h
Function.o: Function.h
Variable.o: Variable.h
TableBase.o:		TableBase.h
Table.o:		TableBase.h Table.h
Interpol.o:		TableBase.h Interpol.h
StimulusTable.o:		TableBase.h StimulusTable.h
TimeTable.o:	TimeTable.h TableBase.h
Stats.o:	Stats.h
SpikeStats.o:	Stats.h SpikeStats.h
Interpol2D.o:	Interpol2D.h
HDF5WriterBase.o: HDF5WriterBase.h
HDF5DataWriter.o: HDF5DataWriter.h HDF5WriterBase.h
testBuiltins.o:	Group.h Arith.h Stats.h ../msg/DiagonalMsg.h ../basecode/SetGet.h

.cpp.o:
	$(CXX) $(CXXFLAGS) $(SMOLDYN_FLAGS) -I. -I..  -I../scheduling -I../basecode -I../msg -I../external/muparser/include $< -c

$(TARGET):		$(OBJ) $(SMOLDYN_OBJ) $(HEADERS)
	$(LD) -r -o $(TARGET) $(OBJ) $(SMOLDYN_OBJ) $(SMOLDYN_LIB_PATH) $(SMOLDYN_LIBS) 

clean:
	-rm -f *.o $(TARGET) core core.*
