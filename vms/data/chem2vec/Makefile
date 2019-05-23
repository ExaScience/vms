TOPDIR=../..


SRCDIR=$(TOPDIR)/c++
CXXFILES=$(wildcard $(SRCDIR)/*.cpp)
HEADERFILES=$(wildcard $(SRCDIR)/*.h)

INCLUDES=-I$(SRCDIR) -Icode
CXXFLAGS=$(INCLUDES) -pthread -g -O0 
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

BINARIES=sim_float sim_observed_float sim_fixed
ROOTFILE=model/root.ini
INPUTDATA=test.sdm train.sdm side_c2v.ddm
MODELFILES=code/smurff_const.h code/smurff_tb.h

.PHONY: hls run clean

run: $(BINARIES)
	./sim_float && ./sim_fixed

$(INPUTDATA): make.py
	python make.py

$(ROOTFILE): $(INPUTDATA) options.ini
	mkdir -p model
	smurff --ini options.ini --save-prefix model

$(MODELFILES): $(ROOTFILE) $(TOPDIR)/codegen/gen_dat.py
	mkdir -p code
	python $(TOPDIR)/codegen/gen_dat.py --root $(ROOTFILE) --output code

sim_float: $(MODELFILES) $(CXXFILES) $(HEADERFILES)
	$(CXX) -DDT_FLOAT $(CXXFLAGS) $(CXXFILES) -o $@ $(LDFLAGS)

sim_observed_float: $(MODELFILES) $(CXXFILES) $(HEADERFILES)
	$(CXX) -DDT_OBSERVED_FLOAT -std=c++11 $(CXXFLAGS) $(CXXFILES) -o $@ $(LDFLAGS)

sim_fixed: $(MODELFILES) $(CXXFILES) $(HEADERFILES)
	$(CXX) -DDT_FIXED $(SC_CXXFLAGS) $(CXXFLAGS) $(CXXFILES) -o $@ $(LDFLAGS) $(SC_LDFLAGS)

clean:
	rm -rf model code hls
	rm -f $(BINARIES)
	rm -rf $(addsuffix .dSYM,$(BINARIES))

hls: $(MODELFILES) $(CXXFILES) $(HEADERFILES)
	ln -sf $(addprefix ../,$(HEADERFILES)) code/
	ln -sf $(addprefix ../,$(CXXFILES)) code/
	vivado_hls $(TOPDIR)/codegen/run.tcl
