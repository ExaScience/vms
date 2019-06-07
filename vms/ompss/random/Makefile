CXXFILES=main.cpp predict.cpp
HEADERFILES=$(wildcard $(SRCDIR)/*.h)

CXXFLAGS=-g -O0 -Wall -Wno-unknown-pragmas -fsanitize=address 
LDFLAGS=

.PHONY: run clean

run: sim
	./sim

sim: $(CXXFILES) $(HEADERFILES) Makefile
	$(CXX) $(CXXFLAGS) $(CXXFILES) -o $@ $(LDFLAGS)

clean:
	rm -f sim
	rm -rf sim.dSYM

