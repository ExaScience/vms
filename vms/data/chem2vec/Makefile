
TOPDIR=../..
SRCDIR=$(TOPDIR)/c++
CXXFILES=$(wildcard $(SRCDIR)/*.cpp)
HEADERFILES=$(wildcard $(SRCDIR)/*.h)

INCLUDES=-I$(SRCDIR) -Icode
CXXFLAGS=$(INCLUDES) -pthread -Wall -Wno-unknown-pragmas
# CXXFLAGS +=-g -O0 -fsanitize=address 
CXXFLAGS +=-g -O3
LDFLAGS=

ROOTFILE=model/root.ini

MODELFILES=code/vms_const.h code/vms_tb.h

model: $(MODELFILES)

.PHONY: hls run clean model

CONDA_PREFIX ?= $(dir $(CONDA_EXE))..

$(ROOTFILE): $(INPUTDATA) options.ini
	mkdir -p model
	source $(CONDA_PREFIX)/bin/activate smurff-0.14 && \
		smurff --ini options.ini --save-prefix model

$(MODELFILES): $(ROOTFILE) $(TOPDIR)/codegen/gen_dat.py
	mkdir -p code 
	ln -sf $(addprefix ../,$(CXXFILES) $(HEADERFILES)) code/
	source $(CONDA_PREFIX)/bin/activate smurff-0.14 && \
	    python $(TOPDIR)/codegen/gen_dat.py --root $(ROOTFILE) --output code

clean:
	rm -rf model code hls
	$(MAKE) -C fpga-zcu102 clean
	$(MAKE) -C native clean
	$(MAKE) -C smp-arm64 clean
	$(MAKE) -C smp-x86 clean

hls: $(MODELFILES) $(CXXFILES) $(HEADERFILES)
	ln -sf $(addprefix ../,$(HEADERFILES)) code/
	ln -sf $(addprefix ../,$(CXXFILES)) code/
	vivado_hls $(TOPDIR)/codegen/run.tcl

