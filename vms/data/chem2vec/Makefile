TOPDIR=../..


SRCDIR=$(TOPDIR)/c++
CXXFILES=$(wildcard $(SRCDIR)/*.cpp)
INCLUDES=-I$(SRCDIR) -Icode
CXXFLAGS=$(INCLUDES) -pthread -g -O0 
LDFLAGS=

# Vivado Xilinx sc_fixed<>
# SYSTEMCROOT=$(XILINX_VIVADO)/lnx64/tools/systemc
# Homebrew sc_fixed<>
SYSTEMCROOT=/usr/local/opt/systemc

SC_CXXFLAGS=-I$(SYSTEMCROOT)/include
SC_LDFLAGS=-L$(SYSTEMCROOT)/lib -lsystemc

BINARIES=sim_float sim_observed_float sim_fixed
ROOTFILE=model/root.ini
INPUTDATA=test.sdm train.sdm side_c2v.ddm
MODELFILES=code/smurff_const.h code/smurff_tb.h

.PHONY: run clean

run: $(BINARIES)
	./sim_float && ./sim_fixed

$(INPUTDATA):
	python make.py

$(ROOTFILE): $(INPUTDATA)
	mkdir -p model
	smurff --ini options.ini --save-prefix model

$(MODELFILES): $(ROOTFILE)
	mkdir -p code
	python $(TOPDIR)/codegen/gen_dat.py --root $(ROOTFILE) --output code

sim_float: $(MODELFILES) $(CXXFILES)
	$(CXX) -DDT_FLOAT $(CXXFLAGS) $(CXXFILES) -o $@ $(LDFLAGS)

sim_observed_float: $(MODELFILES) $(CXXFILES)
	$(CXX) -DDT_OBSERVED_FLOAT -std=c++11 $(CXXFLAGS) $(CXXFILES) -o $@ $(LDFLAGS)

sim_fixed: $(MODELFILES) $(CXXFILES)
	$(CXX) -DDT_FIXED $(SC_CXXFLAGS) $(CXXFLAGS) $(CXXFILES) -o $@ $(LDFLAGS) $(SC_LDFLAGS)

clean:
	rm -rf model code
	rm -f $(BINARIES)
	rm -rf $(addsuffix .dSYM,$(BINARIES))

$(BINARIES): \
 	code/smurff_const.h \
	code/smurff_tb.h \
	../../c++/types.h \
	../../c++/predict.h
