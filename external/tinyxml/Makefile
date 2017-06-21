# Makefile for including muParser into moose.
# Author: Subhasis Ray
#
TARGET = _tinyxml2.o

OBJ = \
	tinyxml2.o

HEADERS = \
	tinyxml2.h	\


default: $(TARGET)

$(OBJ): $(HEADERS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -I. $< -c

$(TARGET):	$(OBJ) $(HEADERS)
	$(LD) -r -o $(TARGET) $(OBJ)

clean:
	-rm -f *.o $(TARGET) core core.*

