TOPDIR=../..


SRCDIR=$(TOPDIR)/c++
CXXFILES=$(wildcard $(SRCDIR)/*.cpp)
HEADERFILES=$(wildcard $(SRCDIR)/*.h)

INCLUDES=-I$(SRCDIR) -Icode
CXXFLAGS=$(INCLUDES) -pthread -g -O0 -Wall -Wno-unknown-pragmas -fsanitize=address 
LDFLAGS=

ROOTFILE=model/root.ini
INPUTDATA=test.mtx train.mtx feat_0_0.mtx
MODELFILES=code/smurff_const.h code/smurff_tb.h

.PHONY: hls run clean

run: sim
	./sim

$(ROOTFILE): $(INPUTDATA) options.ini
	mkdir -p model
	smurff --ini options.ini --save-prefix model

$(MODELFILES): $(ROOTFILE) $(TOPDIR)/codegen/gen_dat.py
	mkdir -p code
	python $(TOPDIR)/codegen/gen_dat.py --root $(ROOTFILE) --output code

sim: $(MODELFILES) $(CXXFILES) $(HEADERFILES) Makefile
	$(CXX) $(CXXFLAGS) $(CXXFILES) -o $@ $(LDFLAGS) 

clean:
	rm -rf model code hls
	rm -f sim
	rm -rf sim.dSYM

hls: $(MODELFILES) $(CXXFILES) $(HEADERFILES)
	ln -sf $(addprefix ../,$(HEADERFILES)) code/
	ln -sf $(addprefix ../,$(CXXFILES)) code/
	vivado_hls $(TOPDIR)/codegen/run.tcl
