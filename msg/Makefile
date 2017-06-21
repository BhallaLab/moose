#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment.
#**           copyright (C) 2007 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU Lesser General Public License version 2.1
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/

TARGET = _msg.o

OBJ = \
	Msg.o	\
	DiagonalMsg.o	\
	OneToAllMsg.o	\
	OneToOneMsg.o	\
	SingleMsg.o	\
	SparseMsg.o	\
	OneToOneDataIndexMsg.o	\
	testMsg.o	\

HEADERS = \
	../basecode/header.h \


default: $(TARGET)

$(OBJ)	: $(HEADERS)
DiagonalMsg.o:	DiagonalMsg.h
OneToAll.o:	OneToAll.h
OneToOne.o:	OneToOne.h
OneToOneDataIndex.o:	OneToOneDataIndex.h
SingleMsg.o:	SingleMsg.h
SparseMsg.o:	SparseMsg.h
testMsg.o: DiagonalMsg.h OneToAllMsg.h OneToOneMsg.h SingleMsg.h SparseMsg.h OneToOneDataIndexMsg.h ../basecode/SetGet.h

.cpp.o:
	$(CXX) $(CXXFLAGS) $(SMOLDYN_FLAGS) -I. -I../basecode $< -c

$(TARGET):		$(OBJ) $(SMOLDYN_OBJ) $(HEADERS)
	$(LD) -r -o $(TARGET) $(OBJ) $(SMOLDYN_OBJ) $(SMOLDYN_LIB_PATH) $(SMOLDYN_LIBS) 

clean:
	-rm -f *.o $(TARGET) core core.*
