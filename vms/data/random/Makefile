TOPDIR=../..


SRCDIR=$(TOPDIR)/c++
CXXFILES=$(wildcard $(SRCDIR)/*.cpp)
CXXFLAGS=-I$(SRCDIR) -Imodel
LDFLAGS=

CXXFLAGS+=-pthread -g -O0 -DDT_FIXED
CXXFLAGS+=-I$(XILINX_VIVADO)/lnx64/tools/systemc/include
LDFLAGS+=-L$(XILINX_VIVADO)/lnx64/tools/systemc/lib -lsystemc

BINARY=sim_random

ROOTFILE=model/root.ini
INPUTDATA=feat_0_0.mtx test.mtx train.mtx options.ini
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

