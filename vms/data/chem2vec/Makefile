TOPDIR=../..


SRCDIR=$(TOPDIR)/c++
CXXFILES=$(wildcard $(SRCDIR)/*.cpp)
INCLUDES=-I$(SRCDIR) -Imodel
CXXFLAGS=$(INCLUDES) -pthread -g -O0 
LDFLAGS=

# CXXFLAGS+=-DDT_OBSERVED_FLOAT -std=c++11
# BINARY=sim_observed_float

CXXFLAGS+=-DDT_FLOAT
BINARY=sim_float

# CXXFLAGS+=-DDT_FIXED
# CXXFLAGS+=-I$(XILINX_VIVADO)/lnx64/tools/systemc/include
# LDFLAGS+=-L$(XILINX_VIVADO)/lnx64/tools/systemc/lib -lsystemc
# BINARY=sim_fixed

BINARIES=sim_float sim_observed_float sim_fixed
ROOTFILE=model/root.ini
INPUTDATA=test.sdm train.sdm side_c2v.ddm
MODELFILES=model/smurff_const.h model/smurff_model.h model/smurff_tb.h

.PHONY: run clean

run: $(BINARY)
	./$(BINARY)

$(INPUTDATA):
	python make.py

$(ROOTFILE): $(INPUTDATA)
	mkdir -p model
	smurff --ini options.ini --save-prefix model

$(MODELFILES): $(ROOTFILE)
	python $(TOPDIR)/codegen/gen_dat.py --root $(ROOTFILE) --output model

$(BINARY): $(MODELFILES) $(CXXFILES)
	$(CXX) $(CXXFLAGS) $(CXXFILES) -o $@ $(LDFLAGS)
	$(CXX) $(CXXFLAGS)  -MM $(CXXFILES) > Makefile.dep

clean:
	rm -rf model
	rm -f Makefile.dep
	rm -f $(BINARIES)
	rm -rf $(addsuffix .dSYM,$(BINARIES))


-include Makefile.dep

