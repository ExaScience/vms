SHELL=/bin/bash

TOPDIR=.
HOST_IF=ompss

all: native ompss-smp-x86 ompss-smp-arm64 hls crdb opencl

include config.mk
include Makefile.common

INPUT_DATA=test.sdm train.sdm features.ddm
ROOTFILE=model/root.ini
OPTIONS=$(SRCDIR)/data/$(DATASET)/options.ini

.PHONY: hls clean cleanall model native all opencl

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
	    python3 $(SRCDIR)/codegen/gen_dat.py --root $(ROOTFILE) --output code --datatype $(DATATYPE)

model: $(FULLMODELFILES)

native: $(ALLCODE)
	module load Vitis/2020.1 && cd native && $(MAKE) >make.log 2>&1

ompss-smp-x86: $(ALLCODE)
	module load Vitis/2021.1 GCC/7.3.0 ompss/19.06 && cd smp-x86 && $(MAKE) >make.log 2>&1

ompss-smp-arm64: $(ALLCODE)
	cd smp-arm64 && dockfpga -w $(BUILDDIR) $(MAKE) >make.log 2>&1

hls: $(ALLCODE)
	module load Vitis/2020.1 && cd hls && $(MAKE) >make.log 2>&1

zcu102: $(ALLCODE)
	cd fpga-zcu102 && dockfpga -w $(BUILDDIR) $(MAKE) >make.log 2>&1

crdb: $(ALLCODE)
	cd fpga-crdb && dockfpga -w $(BUILDDIR) $(MAKE) >make.log 2>&1

opencl: $(ALLCODE)
	module load Vitis/2021.1 && $(MAKE) -C opencl

clean:
	rm -rf model code
	$(MAKE) -C native cleancommon
	$(MAKE) -C smp-arm64 cleancommon
	$(MAKE) -C smp-x86 cleancommon
	$(MAKE) -C fpga-crdb cleancommon
	$(MAKE) -C fpga-zcu102 cleancommon
	$(MAKE) -C opencl clean
