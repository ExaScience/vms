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
OPTIONS=$(TOPDIR)/data/$(DATASET)/options.ini

MODELFILES=code/vms_const.h code/vms_tb.h

.PHONY: hls clean cleanall model native all

CONDA_BIN = $(dir $(CONDA_EXE))

$(INPUT_DATA): $(TOPDIR)/data/$(DATASET)/make.py
	python $<

$(ROOTFILE): $(INPUT_DATA) $(OPTIONS)
	mkdir -p model
	source $(CONDA_BIN)/activate smurff-0.14 && \
		OMP_NUM_THREADS=1 smurff --ini $(OPTIONS) --num-latent $(NUM_LATENT) --nsamples $(NUM_SAMPLES) --save-prefix model

$(MODELFILES): $(ROOTFILE) $(TOPDIR)/codegen/gen_dat.py
	mkdir -p code 
	source $(CONDA_BIN)/activate smurff-0.14 && \
	    python $(TOPDIR)/codegen/gen_dat.py --root $(ROOTFILE) --output code

clean:
	rm -rf model code options.ini
	$(MAKE) -C fpga-zcu102 clean
	$(MAKE) -C native clean
	$(MAKE) -C smp-arm64 clean
	$(MAKE) -C smp-x86 clean

hls: $(MODELFILES) $(CXXFILES) $(HEADERFILES)
	mkdir -p hls/code
	ln -sf $(addprefix ../../,$(MODELFILES)) hls/code/
	ln -sf $(MODELFILES) $(HEADERFILES) $(CXXFILES) hls/code/
	module load Vitis/2020.1 && $(MAKE) -C hls

native: $(MODELFILES) $(CXXFILES) $(HEADERFILES)
	module load Vitis/2020.1 && $(MAKE) -C native

ompss-smp-x86: $(MODELFILES) $(CXXFILES) $(HEADERFILES)
	module load gcc-7.3.0-gcc-9.3.0-mk2k4ie ompss-19.06-gcc-7.3.0-jseith3 && $(MAKE) -C smp-x86

ompss-smp-arm64: $(MODELFILES) $(CXXFILES) $(HEADERFILES)
	cd smp-arm64 && euroexacross --home make
