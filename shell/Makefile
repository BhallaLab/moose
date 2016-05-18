#/**********************************************************************
#** This program is part of 'MOOSE', the
#** Messaging Object Oriented Simulation Environment.
#**           copyright (C) 2007 Upinder S. Bhalla. and NCBS
#** It is made available under the terms of the
#** GNU Lesser General Public License version 2.1
#** See the file COPYING.LIB for the full notice.
#**********************************************************************/

TARGET = _shell.o

OBJ = \
	Shell.o	\
	ShellCopy.o	\
	ShellThreads.o	\
	LoadModels.o \
	SaveModels.o \
	Neutral.o	\
	Wildcard.o	\
	testShell.o	\

HEADERS = \
	../basecode/header.h \


default: $(TARGET)

$(OBJ)	: $(HEADERS)
Shell.o:	Shell.h Neutral.h ../scheduling/Clock.h ../sbml/MooseSbmlWriter.h ../sbml/MooseSbmlReader.h
ShellCopy.o:	Shell.h Neutral.h ../scheduling/Clock.h
ShellSetGet.o:	Shell.h
ShellThreads.o:	Shell.h Neutral.h ../scheduling/Clock.h
LoadModels.o:	Shell.h Neutral.h ../biophysics/SwcSegment.h ../biophysics/ReadSwc.h
SaveModels.o:	Shell.h Neutral.h
Neutral.o:	Neutral.h ../basecode/ElementValueFinfo.h
Wildcard.o:	Wildcard.h Shell.h Neutral.h ../basecode/ElementValueFinfo.h
testShell.o:	Wildcard.h Shell.h Neutral.h ../builtins/Arith.h ../basecode/SparseMatrix.h ../msg/SparseMsg.h ../msg/SingleMsg.h ../basecode/SetGet.h ../basecode/HopFunc.h ../basecode/OpFuncBase.h ../basecode/OpFunc.h

#../kinetics/ReadKkit.h ../biophysics/ReadCell.h

.cpp.o:
	$(CXX) $(CXXFLAGS) $(SMOLDYN_FLAGS) -I. -I.. -I../basecode -I../msg $< -c

$(TARGET):		$(OBJ) $(SMOLDYN_OBJ) $(HEADERS)
	$(LD) -r -o $(TARGET) $(OBJ) $(SMOLDYN_OBJ) $(SMOLDYN_LIB_PATH) $(SMOLDYN_LIBS) 

clean:
	-rm -f *.o $(TARGET) core core.*
