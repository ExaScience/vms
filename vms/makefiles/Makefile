SHELL=/bin/bash

TOPDIR=.

all: native ompss-smp-x86 ompss-smp-arm64 hls zcu102

include config.mk
include Makefile.common

INPUT_DATA=test.sdm train.sdm features.ddm
ROOTFILE=model/root.ini
OPTIONS=$(SRCDIR)/data/$(DATASET)/options.ini

.PHONY: hls clean cleanall model native all

CONDA_BIN = $(dir $(CONDA_EXE))

$(INPUT_DATA): $(SRCDIR)/data/$(DATASET)/make.py
	python3 $<

$(ROOTFILE): $(INPUT_DATA) $(OPTIONS)
	mkdir -p model
	source $(CONDA_BIN)/activate smurff-0.14 && \
		OMP_NUM_THREADS=1 smurff --ini $(OPTIONS) --num-latent $(NUM_LATENT) --nsamples $(NUM_SAMPLES) --save-prefix model

$(FULLMODELFILES): $(ROOTFILE) $(SRCDIR)/codegen/gen_dat.py
	mkdir -p code 
	source $(CONDA_BIN)/activate smurff-0.14 && \
	    python3 $(SRCDIR)/codegen/gen_dat.py --root $(ROOTFILE) --output code


native: $(ALLCODE)
	module load Vitis/2020.1 && $(MAKE) -C native

ompss-smp-x86: $(ALLCODE)
	module load gcc-7.3.0-gcc-9.3.0-mk2k4ie ompss-19.06-gcc-7.3.0-jseith3 && $(MAKE) -C smp-x86

ompss-smp-arm64: $(ALLCODE)
	cd smp-arm64 && dockfpga -w $(BUILDDIR) make

hls: $(ALLCODE)
	module load Vitis/2020.1 && $(MAKE) -C hls

zcu102: $(ALLCODE)
	cd fpga-zcu102 && dockfpga -w $(BUILDDIR) make

clean: cleancommon
	rm -rf model code
	$(MAKE) -C fpga-zcu102 clean
	$(MAKE) -C native clean
	$(MAKE) -C smp-arm64 clean
	$(MAKE) -C smp-x86 clean