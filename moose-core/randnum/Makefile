CXXFLAGS += -I..
# the line below are for easy inclusion of libxml++
#CXXFLAGS += $(shell pkg-config libxml++-2.6 --CXXFLAGS) 

TARGET = _randnum.o

OBJ = \
	mt19937ar.o	\
	RandGenerator.o	\
	UniformRng.o	\
	Uniform.o	\
	Exponential.o	\
	ExponentialRng.o	\
	Binomial.o	\
	Normal.o	\
	Poisson.o	\
	Gamma.o		\
	PoissonRng.o	\
	NormalRng.o	\
	BinomialRng.o	\
	GammaRng.o	\

HEADERS = \
	../basecode/header.h	\
	randnum.h	\
	Probability.h	\
	RandGenerator.h	\
	UniformRng.h	\
	Uniform.h	\
	Exponential.h	\
	ExponentialRng.h	\
	Binomial.h	\
	Gamma.h		\
	Poisson.h	\
	PoissonRng.h	\
	Normal.h	\
	NormalRng.h	\
	BinomialRng.h	\
	GammaRng.h	\

default: $(TARGET)

.cpp.o:
	$(CXX) $(CXXFLAGS) $< -c
#	$(CXX) $(CXXFLAGS) $(shell pkg-config libxml++-2.6 --CXXFLAGS) $< -c

$(TARGET): $(OBJ) $(HEADERS)
	$(LD) -r -o $(TARGET) $(OBJ)

clean:
	-rm -f *.o $(TARGET) core core.*
