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

TARGET = _utility.o

OBJ = \
	strutil.o	\
	types.o 	\
	setupenv.o	\
	numutil.o	\
	Annotator.o	\
	cnpy.o 		\
	Vec.o	\


HEADERS = \
	strutil.h 	\
	numutil.h	\
	../basecode/header.h \
	cnpy.hpp 	\

default: $(TARGET)

strutil.o: strutil.h
Annotator.o: Annotator.h
Vec.o: Vec.h ../basecode/doubleEq.h
cnpy.o : cnpy.hpp

.cpp.o:
	$(CXX) $(CXXFLAGS) -I.. -I../basecode $< -c

$(TARGET): $(OBJ) $(HEADERS)
	$(LD) -r -o $(TARGET) $(OBJ) 
clean:
	-rm -f *.o $(TARGET) core core.*
