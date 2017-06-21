# Makefile
# 
# Author: Subhasis Ray
# Created: 2011-03-10 12:10:32 (+0530)
TARGET = _pymoose.o
# CXXFLAGS += -I../basecode -I../msg -I../shell
CXXFLAGS += -DQUIET_MODE
NUMPY_WARNING_FLAG = -DNPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION
.PHONY: clean

HEADERS = moosemodule.h ../basecode/global.h PyRun.h

OBJ = moosemodule.o mfield.o vec.o melement.o test_moosemodule.o PyRun.o

$(TARGET): $(OBJ) $(HEADERS) ../basecode/main.cpp ../utility/_utility.o ../basecode/_basecode.o
	$(LD) -r -o $(TARGET) $(OBJ) $(LDFLAGS)

.cpp.o: $(HEADERS)
	$(CXX) $(CXXFLAGS) $(NUMPY_WARNING_FLAG) -I.. -I../basecode $< -c
clean: 
	-rm -f *.o

