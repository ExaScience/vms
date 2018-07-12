
CXXFLAGS=-g -O0 -fsanitize=address 
CXXFLAGS+=-std=c++11 -DDT_OBSERVED_DOUBLE 

LDFLAGS=-lsystemc

BINARY  = native
FILES   = $(wildcard *.cpp)
HEADERS = $(wildcard *.h)
 
all: $(BINARY)
 
$(BINARY): $(FILES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(FILES) -o $(BINARY) $(LDFLAGS)

