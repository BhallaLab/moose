# Makefile for including muParser into moose.
# Author: Subhasis Ray
#
TARGET = _muparser.o

OBJ = \
    src/muParserBase.o \
    src/muParserBytecode.o \
    src/muParserCallback.o \
    src/muParser.o  \
    src/muParserDLL.o \
    src/muParserError.o \
    src/muParserInt.o \
    src/muParserTokenReader.o \

HEADERS = \
	include/muParser.h	\
	include/muParserBase.h	\
	include/muParserBytecode.h	\
	include/muParserCallback.h	\
	include/muParserDef.h	\
	include/muParserError.h	\
	include/muParserFixes.h	\
	include/muParserStack.h	\
	include/muParserToken.h	\
	include/muParserTokenReader.h		\
	include/muParserTemplateMagic.h	\


default: $(TARGET)

$(OBJ): $(HEADERS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I. -I../../ -Iinclude $< -c -o $@

$(TARGET):	$(OBJ) $(HEADERS)
	$(LD) -r -o $(TARGET) $(OBJ)

clean:
	-rm -f *.o $(TARGET) core core.*

