SHELL=/bin/bash

TOPDIR=.
HOST_IF=ompss

all: native ompss-smp-x86 ompss-smp-arm64 hls crdb opencl zip

include config.mk
include Makefile.common

INPUT_DATA=test.sdm train.sdm features.ddm
ROOTFILE=model/root.ini
OPTIONS=$(SRCDIR)/data/$(DATASET)/options.ini

.PHONY: hls clean cleanall model native all opencl zip

CONDA_BIN = $(dir $(CONDA_EXE))

$(INPUT_DATA): $(SRCDIR)/data/$(DATASET)/make.py
	python3 $<

$(ROOTFILE): $(INPUT_DATA) $(OPTIONS)
	mkdir -p model
	source $(CONDA_BIN)/activate smurff-0.14 && \
		OMP_NUM_THREADS=1 smurff --ini $(OPTIONS) --num-latent $(NUM_LATENT) --nsamples $(NUM_SAMPLES) --save-prefix model

$(FULLMODELFILES): $(ROOTFILE) $(SRCDIR)/scripts/codegen.py
	mkdir -p code 
	source $(CONDA_BIN)/activate smurff-0.14 && \
	    python3 $(SRCDIR)/scripts/codegen.py --root $(ROOTFILE) --output code

model: $(FULLMODELFILES)

native: $(ALLCODE)
	module load Vitis/2020.1 && cd native && $(MAKE)

ompss-smp-x86: $(ALLCODE)
	module load Vitis/2021.1 GCC/7.3.0 ompss/19.06 && cd smp-x86 && $(MAKE)

ompss-smp-arm64: $(ALLCODE)
	cd smp-arm64 && dockfpga -w $(BUILDDIR) $(MAKE)

ompss-2-cluster: $(ALLCODE)
	. $(HOME)/local/spack/share/spack/setup-env.sh && \
		spack load ompss-2-cluster/iamwjnb && \
		cd cluster && \
		make

hls: $(ALLCODE)
	module load Vitis/2020.1 && cd hls && $(MAKE)

zcu102: $(ALLCODE)
	cd fpga-zcu102 && dockfpga -w $(BUILDDIR) $(MAKE)

crdb: $(ALLCODE)
	cd fpga-crdb && dockfpga -w $(BUILDDIR) $(MAKE)

opencl: $(ALLCODE)
	module load Vitis/2021.1 && $(MAKE) -C opencl

ZIPFILE=$(notdir $(realpath  .)).zip
zip: $(ZIPFILE)
$(ZIPFILE): crdb
	zip $(notdir $(realpath  .)).zip fpga-crdb/* fpga-crdb/euroexa_*/*_ait/*

clean:
	rm -rf model code *.zip
	$(MAKE) -C native cleancommon
	$(MAKE) -C smp-arm64 cleancommon
	$(MAKE) -C smp-x86 cleancommon
	$(MAKE) -C cluster cleancommon
	$(MAKE) -C fpga-crdb cleancommon
	$(MAKE) -C fpga-zcu102 cleancommon
	$(MAKE) -C opencl clean

updatelinks:
	python3 $(SRCDIR)/scripts/build.py update
	   
