TOPDIR=../..


SRCDIR=$(TOPDIR)/c++
CXXFILES=$(wildcard $(SRCDIR)/*.cpp)
CXXFLAGS=-I$(SRCDIR) -Imodel
CXXFLAGS+=-pthread -g -O0 
LDFLAGS=

# CXXFLAGS+=-DDT_OBSERVED_DOUBLE -std=c++11
# BINARY=sim_observed_double

# CXXFLAGS+=-DDT_DOUBLE
# BINARY=sim_double

CXXFLAGS+=-DDT_FIXED
CXXFLAGS+=-I$(XILINX_VIVADO)/lnx64/tools/systemc/include
LDFLAGS+=-L$(XILINX_VIVADO)/lnx64/tools/systemc/lib -lsystemc
BINARY=sim_fixed

ROOTFILE=model/root.ini
INPUTDATA=test.sdm train.sdm side_c2v.ddm
MODELFILES=model/smurff_const.h model/smurff_model.h model/smurff_tb.h

.PHONY: run clean

run: $(BINARY)
	./$(BINARY)

$(ROOTFILE): $(INPUTDATA)
	mkdir -p model
	smurff --ini options.ini --save-prefix model

$(MODELFILES): $(ROOTFILE)
	python $(TOPDIR)/codegen/gen_dat.py --root $(ROOTFILE) --output model

$(BINARY): $(MODELFILES) $(CXXFILES)
	$(CXX) $(CXXFLAGS) $(CXXFILES) -o $@ $(LDFLAGS)

clean:
	rm -rf model
	rm -f $(BINARY)

