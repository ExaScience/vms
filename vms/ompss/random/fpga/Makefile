TOPDIR=..

default: bitstream

include $(TOPDIR)/Makefile.common

MCXX   = fpgacxx
CFLAGS_ += -DOMPSS_FPGA

BOARD=zcu102

bitstream: $(SRCFILES)
	mkdir -p ${PWD}/$(BOARD)
	$(MCXX_) $(CFLAGS_) $(MCXX_FLAGS_) $(MCXX_FLAGS_I_) --bitstream-generation \
	--Wf,"--name=$(PROGRAM_),--dir=${PWD}/$(BOARD),--board=$(BOARD),--task_manager,--clock=200" \
	$^ -o $(PROGRAM_)-i $(LDFLAGS_)

export PETALINUX_BUILD = $(PWD)/zcu102/petalinux
BSP = /home/vanderaa/local/software/Xilinx/Downloads/Xilinx-ZCU102-v2016.3-final.bsp
PETALINUX_PROJECT = $(PETALINUX_BUILD)/config.project

$(PETALINUX_BUILD): $(BSP)
	petalinux-create -t project -s $(BSP) -n $(notdir $(PETALINUX_BUILD)) --out $(dir $(PETALINUX_BUILD))

$(PETALINUX_PROJECT): $(PETALINUX_BUILD)
	petalinux-config --oldconfig -p $(PETALINUX_BUILD)

boot: $(PETALINUX_PROJECT)
	autoVivado --verbose --name=$(PROGRAM_) --dir=${PWD}/$(BOARD) --board=$(BOARD) --task_manager --clock=200 --from_step=boot --to_step=boot
