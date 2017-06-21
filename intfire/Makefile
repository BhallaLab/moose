#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment.
#**           copyright (C) 2007 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU Lesser General Public License version 2.1
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/

TARGET = _intfire.o

OBJ = \
	IntFireBase.o	\
	LIF.o \
	QIF.o \
	ExIF.o \
	AdExIF.o \
	AdThreshIF.o \
	IzhIF.o \
	testIntFire.o \

# GSL_LIBS = -L/usr/lib -lgsl

HEADERS = \
	../basecode/header.h \
	../utility/numutil.h \
	IntFireBase.h \
	../biophysics/CompartmentBase.h \
	../biophysics/Compartment.h


default: $(TARGET)

$(OBJ)	: $(HEADERS)
LIF.o:	LIF.h
QIF.o:	QIF.h
ExIF.o:	ExIF.h
AdExIF.o:	AdExIF.h
AdThreshIF.o:	AdThreshIF.h
IzhIF.o:	IzhIF.h
testIntFire.o:	LIF.h

.cpp.o:
	$(CXX) $(CXXFLAGS) $(SMOLDYN_FLAGS) -I. -I../basecode -I../msg $< -c

$(TARGET): $(OBJ) $(SMOLDYN_OBJ) $(HEADERS)
	$(LD) -r -o $(TARGET) $(OBJ) $(SMOLDYN_OBJ) $(SMOLDYN_LIB_PATH) $(SMOLDYN_LIBS) $(GSL_LIBS)

clean:
	rm -f *.o $(TARGET) core core.*
