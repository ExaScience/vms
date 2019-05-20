
# CXXFLAGS=-g -O0 -fsanitize=address 
CXXFLAGS=-g -O3 -march=native
CXX=icpc

# observed double
CXXFLAGS+=-std=c++11
CXXFLAGS+=-I/opt/software/Eigen/3.3.4/include

# systemc types
#LDFLAGS=-lsystemc

BINARY  = native
FILES   = $(wildcard *.cpp)
HEADERS = $(wildcard *.h)
 
all: $(BINARY)
 
$(BINARY): $(FILES) $(HEADERS) Makefile
	$(CXX) $(CXXFLAGS) $(FILES) -o $(BINARY) $(LDFLAGS)

clean:
	rm -f *.o *.exe $(BINARY)
