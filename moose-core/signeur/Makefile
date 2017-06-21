#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment.
#**           Copyright (C) 2003-2012 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU Lesser General Public License version 2.1
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/

CXXFLAGS += -I..
# the line below are for easy inclusion of libxml++
#CXXFLAGS += $(shell pkg-config libxml++-2.6 --cflags) 

TARGET = _signeur.o

OBJ = \
	Adaptor.o	\
	testSigNeur.o


HEADERS = \
	../basecode/header.h \

default: $(TARGET)

$(OBJ)	: $(HEADERS)
Adaptor.o:	Adaptor.h
testSigNeur.o:	Adaptor.h

.cpp.o:
	$(CXX) $(CXXFLAGS) -I.. -I../basecode $< -c

$(TARGET): $(OBJ) $(HEADERS)
	$(LD) -r -o $(TARGET) $(OBJ) 
clean:
	-rm -f *.o $(TARGET) core core.*
