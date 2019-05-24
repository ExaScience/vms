TOPDIR=../..


SRCDIR=$(TOPDIR)/c++
CXXFILES=$(wildcard $(SRCDIR)/*.cpp)
HEADERFILES=$(wildcard $(SRCDIR)/*.h)

INCLUDES=-I$(SRCDIR) -Icode
CXXFLAGS=$(INCLUDES) -pthread -g -O0 -Wall -Wno-unknown-pragmas -fsanitize=address 
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

ROOTFILE=model/root.ini
INPUTDATA=test.sdm train.sdm side_c2v.ddm
MODELFILES=code/smurff_const.h code/smurff_tb.h

.PHONY: hls run clean

run: sim
	./sim

$(INPUTDATA): make.py
	python make.py

$(ROOTFILE): $(INPUTDATA) options.ini
	mkdir -p model
	smurff --ini options.ini --save-prefix model

$(MODELFILES): $(ROOTFILE) $(TOPDIR)/codegen/gen_dat.py
	mkdir -p code
	python $(TOPDIR)/codegen/gen_dat.py --root $(ROOTFILE) --output code

sim: $(MODELFILES) $(CXXFILES) $(HEADERFILES) Makefile
	$(CXX) -DDT_FIXED $(SC_CXXFLAGS) $(CXXFLAGS) $(CXXFILES) -o $@ $(LDFLAGS) $(SC_LDFLAGS)

clean:
	rm -rf model code hls
	rm -f sim
	rm -rf sim.dSYM

hls: $(MODELFILES) $(CXXFILES) $(HEADERFILES)
	ln -sf $(addprefix ../,$(HEADERFILES)) code/
	ln -sf $(addprefix ../,$(CXXFILES)) code/
	vivado_hls $(TOPDIR)/codegen/run.tcl
