SHELL=/bin/bash

include config.mk

SRCDIR=$(TOPDIR)/src
CXXFILES=$(wildcard $(SRCDIR)/*.cpp)
HEADERFILES=$(wildcard $(SRCDIR)/*.h)

INCLUDES=-I$(SRCDIR) -Icode
CXXFLAGS=$(INCLUDES) -pthread -Wall -Wno-unknown-pragmas
# CXXFLAGS +=-g -O0 -fsanitize=address 
CXXFLAGS +=-g -O3
LDFLAGS=

INPUT_DATA=test.sdm train.sdm features.ddm
ROOTFILE=model/root.ini

MODELFILES=code/vms_const.h code/vms_tb.h

model: $(MODELFILES)

.PHONY: hls clean cleanall model

CONDA_BIN = $(dir $(CONDA_EXE))

options.ini: $(TOPDIR)/data/$(DATASET)/options.ini
	ln -s $(TOPDIR)/data/$(DATASET)/options.ini .

$(INPUT_DATA): $(TOPDIR)/data/$(DATASET)/make.py
	python $<

$(ROOTFILE): $(INPUT_DATA) options.ini
	mkdir -p model
	source $(CONDA_BIN)/activate smurff-0.14 && \
		smurff --ini options.ini --num-latent $(NUM_LATENT) --nsamples $(NUM_SAMPLES) --save-prefix model

$(MODELFILES): $(ROOTFILE) $(TOPDIR)/codegen/gen_dat.py
	mkdir -p code 
	echo $(SRCDIR)
	echo $(CXXFILES)
	ln -sf $(addprefix ../,$(CXXFILES) $(HEADERFILES)) code/
	source $(CONDA_BIN)/activate smurff-0.14 && \
	    python $(TOPDIR)/codegen/gen_dat.py --root $(ROOTFILE) --output code

clean:
	rm -rf model code options.ini
	$(MAKE) -C fpga-zcu102 clean
	$(MAKE) -C native clean
	$(MAKE) -C smp-arm64 clean
	$(MAKE) -C smp-x86 clean

cleanall: clean
	rm -f $(INPUT_DATA) options.ini


hls: $(MODELFILES) $(CXXFILES) $(HEADERFILES)
	ln -sf $(addprefix ../,$(HEADERFILES)) code/
	ln -sf $(addprefix ../,$(CXXFILES)) code/
	vitis_hls $(TOPDIR)/codegen/run.tcl

