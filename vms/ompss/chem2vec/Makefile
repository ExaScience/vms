SRCDIR=.
CXXFILES=$(wildcard $(SRCDIR)/*.cpp)
HEADERFILES=$(wildcard $(SRCDIR)/*.h)

CXXFLAGS=-pthread -g -O0 -Wall -Wno-unknown-pragmas -fsanitize=address 
LDFLAGS=

.PHONY: run clean

run: sim
	./sim

sim: $(MODELFILES $(CXXFILES) $(HEADERFILES) Makefile
	$(CXX) -DDT_FIXED $(SC_CXXFLAGS) $(CXXFLAGS) $(CXXFILES) -o $@ $(LDFLAGS) $(SC_LDFLAGS)

clean:
	rm -f sim
	rm -rf sim.dSYM

