TOPDIR=../..


SRCDIR=$(TOPDIR)/c++
CXXFILES=$(wildcard $(SRCDIR)/*.cpp)
HEADERFILES=$(wildcard $(SRCDIR)/*.h)

INCLUDES=-I$(SRCDIR) -Icode 
CXXFLAGS=$(INCLUDES) -pthread -Wall -Wno-unknown-pragmas 
CXXFLAGS+=-g -O0 -fsanitize=address 
LDFLAGS=

ROOTFILE=model/root.ini
INPUTDATA=test.mtx train.mtx feat_0_0.mtx
MODELFILES=code/smurff_const.h code/smurff_tb.h

.PHONY: hls run clean

run: sim_fx sim_float
	./sim_float
	./sim_fx

$(ROOTFILE): $(INPUTDATA) options.ini
	mkdir -p model
	smurff --ini options.ini --save-prefix model

$(MODELFILES): $(ROOTFILE) $(TOPDIR)/codegen/gen_dat.py
	mkdir -p code
	python $(TOPDIR)/codegen/gen_dat.py --root $(ROOTFILE) --output code --block 3

sim_fx: $(MODELFILES) $(CXXFILES) $(HEADERFILES) Makefile
	$(CXX) -DDT_FIXED $(CXXFLAGS) $(CXXFILES) -o $@ $(LDFLAGS) 

sim_float: $(MODELFILES) $(CXXFILES) $(HEADERFILES) Makefile
	$(CXX) -DDT_FLOAT $(CXXFLAGS) $(CXXFILES) -o $@ $(LDFLAGS) 

clean:
	rm -rf model code hls
	rm -f sim
	rm -rf sim.dSYM

hls: $(MODELFILES) $(CXXFILES) $(HEADERFILES)
	ln -sf $(addprefix ../,$(HEADERFILES)) code/
	ln -sf $(addprefix ../,$(CXXFILES)) code/
	vivado_hls $(TOPDIR)/codegen/run.tcl
