SRCDIR=.
CXXFILES=$(wildcard $(SRCDIR)/*.cpp)
HEADERFILES=$(wildcard $(SRCDIR)/*.h)

CXXFLAGS=-pthread -g -O0 -Wall -Wno-unknown-pragmas -fsanitize=address -std=c++11
LDFLAGS=

ifeq ($(XILINX_VIVADO),)
#Homebrew sc_fixed<>
SYSTEMCROOT=/usr/local/opt/systemc
else
# Vivado Xilinx sc_fixed<>
SYSTEMCROOT=$(XILINX_VIVADO)/lnx64/tools/systemc
CXX=$(XILINX_VIVADO)/lnx64/tools/clang-3.9/bin/clang++
endif

SC_CXXFLAGS=-I$(SYSTEMCROOT)/include
SC_LDFLAGS=-L$(SYSTEMCROOT)/lib -lsystemc

.PHONY: run clean

run: sim
	./sim

sim: $(MODELFILES) $(CXXFILES) $(HEADERFILES) Makefile
	$(CXX) -DDT_FIXED $(SC_CXXFLAGS) $(CXXFLAGS) $(CXXFILES) -o $@ $(LDFLAGS) $(SC_LDFLAGS)

clean:
	rm -f sim
	rm -rf sim.dSYM

