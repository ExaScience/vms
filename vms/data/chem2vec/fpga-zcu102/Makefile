TOPDIR=..

BOARD=zcu102

default: boot

include $(TOPDIR)/Makefile.common
include $(TOPDIR)/Makefile.ompss

MCXX   = aarch64-linux-gnu-fpgacxx
CFLAGS_ += -DOMPSS_FPGA -std=c++11

BUILDDIR ?= $(BOARD)
BITSTREAM = $(PROGRAM_).bit
AVDIR=$(BUILDDIR)/$(PROGRAM_)_autoVivado
BITSTREAM_FLAGS=--bitstream-generation --Wf,"--name=$(PROGRAM_),--dir=$(BUILDDIR),--board=$(BOARD),--task_manager,--clock=200"

# BSP = /home/vanderaa/local/software/Xilinx/Downloads/Xilinx-ZCU102-v2016.3-final.bsp
# BSP = /home/vanderaa/local/software/Xilinx/Downloads/xilinx-zcu102-v2016.4-final1.bsp
# BSP = $(HOME)/euroexa/ompss/ompss_petalinux/xilinx-zcu102-v2016.4-bsc1.bsp
BSP = $(HOME)/euroexa/ompss/ompss_petalinux/xilinx-zcu102-v2016.4-tva1.bsp
PETALINUX_CONFIG = $(PETALINUX_BUILD)/config.project
export PETALINUX_BUILD = $(BUILDDIR)/petalinux

bitstream: $(BITSTREAM)

boot: BOOT.BIN

.PHONY: bitstream boot default

$(PROGRAM_): $(OBJFILES)
	$(MCXX) $(CFLAGS_) $(MCXX_FLAGS_) $(MCXX_FLAGS_D_) $? -o $@ $(LDFLAGS_)

%.o: $(SRCDIR)/%.cpp
	$(MCXX) $(CFLAGS_) $(MCXX_FLAGS_) $(MCXX_FLAGS_D_) $(BITSTREAM_FLAGS) -c $^ -o $@

$(BITSTREAM): $(OBJFILES) 
	mkdir -p $(BUILDDIR) && [ $(BUILDDIR) == $(BOARD) ] || ln -sf $(BUILDDIR) $(BOARD)
	autoVivado --verbose --name=$(PROGRAM_) --dir=$(BUILDDIR) --board=$(BOARD) --task_manager --clock=200 --from_step=HLS --to_step=bitstream
	cp $(AVDIR)/$(PROGRAM_).xtasks.config .
	cp $(AVDIR)/$(PROGRAM_).bit .
	cp $(AVDIR)/$(PROGRAM_).autoVivado.log $(AVDIR)/$(PROGRAM_).autoVivado.bitstream.log

BOOT.BIN: $(PETALINUX_CONFIG) $(BITSTREAM)
	petalinux-config -p $(PETALINUX_BUILD) --oldconfig --get-hw-description $(AVDIR)/Vivado/$(PROGRAM_)/$(PROGRAM_).sdk
	petalinux-build -p $(PETALINUX_BUILD)

$(PETALINUX_CONFIG): $(BSP)
	mkdir -p $(BUILDDIR) && [ $(BUILDDIR) == $(BOARD) ] || ln -sf $(BUILDDIR) $(BOARD)
	rm -rf $(PETALINUX_BUILD)
	cd $(BUILDDIR) && petalinux-create -t project -s $(BSP) -n $(notdir $(PETALINUX_BUILD))
	touch $(PETALINUX_CONFIG)

