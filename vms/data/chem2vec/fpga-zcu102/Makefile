TOPDIR=..

BOARD=zcu102

default: boot

include $(TOPDIR)/Makefile.common

MCXX   = aarch64-linux-gnu-fpgacxx
CFLAGS_ += -DOMPSS_FPGA

BUILDDIR ?= $(BOARD)
BITSTREAM = $(PROGRAM_).bit
AVDIR=$(BUILDDIR)/$(PROGRAM_)_autoVivado

bitstream: $(BITSTREAM)

boot: BOOT.BIN

.PHONY: bitstream boot default

$(BITSTREAM): $(SRCFILES)
	mkdir -p $(BUILDDIR)
	rm -f *automatic_mcxx.cpp
	$(MCXX) $(CFLAGS_) $(MCXX_FLAGS_) $(MCXX_FLAGS_I_) --bitstream-generation \
	--Wf,"--name=$(PROGRAM_),--dir=$(BUILDDIR),--board=$(BOARD),--task_manager,--clock=200" \
	$^ -o $(PROGRAM_)-i $(LDFLAGS_)
	cp $(AVDIR)/$(PROGRAM_).xtasks.config .
	cp $(AVDIR)/$(PROGRAM_).bit .
	cp $(AVDIR)/$(PROGRAM_).autoVivado.log $(AVDIR)/$(PROGRAM_).autoVivado.bitstream.log

export PETALINUX_BUILD = $(BUILDDIR)/petalinux
BSP = /home/vanderaa/local/software/Xilinx/Downloads/Xilinx-ZCU102-v2016.3-final.bsp
PETALINUX_CONFIG = $(PETALINUX_BUILD)/config.project

$(PETALINUX_CONFIG): $(BSP)
	rm -rf $(PETALINUX_BUILD)
	petalinux-create -t project -s $(BSP) -n $(notdir $(PETALINUX_BUILD)) --out $(dir $(PETALINUX_BUILD))
	petalinux-config --oldconfig -p $(PETALINUX_BUILD)
	cd $(PETALINUX_BUILD) && patch -p0 -i $(PWD)/sdcard.patch && cd -

BOOT.BIN: $(PETALINUX_CONFIG) $(BITSTREAM)
	autoVivado --verbose --name=$(PROGRAM_) --dir=$(BUILDDIR) --board=$(BOARD) --task_manager --clock=200 --from_step=boot --to_step=boot
	cp $(AVDIR)/BOOT.BIN .
	cp $(AVDIR)/$(PROGRAM_).autoVivado.log $(AVDIR)/$(PROGRAM_).autoVivado.boot.log
