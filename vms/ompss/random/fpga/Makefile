TOPDIR=..

default: bitstream

include ../Makefile

MCXX   = fpgacxx
CFLAGS_ += -DOMPSS_FPGA

BOARD=zcu102

bitstream: $(SRCFILES)
	mkdir -p ${PWD}/$(BOARD)
	$(MCXX_) $(CFLAGS_) $(MCXX_FLAGS_) $(MCXX_FLAGS_I_) --bitstream-generation \
	--Wf,"--name=$(PROGRAM_),--dir=${PWD}/$(BOARD),--board=$(BOARD),--task_manager,--clock=200" \
	$^ -o $(PROGRAM_)-i $(LDFLAGS_)

export PETALINUX_BUILD = $(PWD)/zcu102/petalinux
boot:
	mkdir -p $(PETALINUX_BUILD)
	autoVivado --verbose --name=$(PROGRAM_) --dir=${PWD}/$(BOARD) --board=$(BOARD) --task_manager --clock=200 --from_step=boot --to_step=boot

clean:
	rm -f *.o $(PROGS_)
