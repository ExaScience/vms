
CXXFLAGS=-g -O0 -fsanitize=address 

# observed double
#CXXFLAGS+=-std=c++11 -DDT_OBSERVED_DOUBLE 

# systemc types
CXXFLAGS+=-DDT_SC_FIXED 
LDFLAGS=-lsystemc

BINARY  = native
FILES   = $(wildcard *.cpp)
HEADERS = $(wildcard *.h)
 
all: $(BINARY)
 
$(BINARY): $(FILES) $(HEADERS) Makefile
	$(CXX) $(CXXFLAGS) $(FILES) -o $(BINARY) $(LDFLAGS)

