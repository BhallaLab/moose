TARGET = _device.o

OBJ = PulseGen.o	\
	DiffAmp.o	\
	PIDController.o	\
	RC.o	

HEADERS = \
	../basecode/header.h 	\

default: $(TARGET)

$(OBJ): $(HEADERS)
PulseGen.o: PulseGen.h
DiffAmp.o: DiffAmp.h
PIDController.o: PIDController.h
RC.o: RC.h

.cpp.o:
	$(CXX) $(CXXFLAGS) $(SMOLDYN_FLAGS) -I. -I../basecode -I../msg $< -c

$(TARGET): $(OBJ) $(SMOLDYN_OBJ) $(HEADERS)
	$(LD) -r -o $(TARGET) $(OBJ) $(SMOLDYN_OBJ) $(SMOLDYN_LIB_PATH) $(SMOLDYN_LIBS) $(GSL_LIBS)

clean:
	rm -f *.o $(TARGET) core core.*
