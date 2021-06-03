TOPDIR=..

BOARD=euroexa_crdb

default: bitstream

include $(TOPDIR)/Makefile.common
include $(TOPDIR)/Makefile.ompss

MCXX   = aarch64-linux-gnu-fpgacxx
CFLAGS_ += -DOMPSS_FPGA -std=c++11

BUILDDIR ?= $(BOARD)
BITSTREAM = $(PROGRAM_).bit
AVDIR=$(BUILDDIR)/$(PROGRAM_)_autoVivado
BITSTREAM_FLAGS=--bitstream-generation --Wf,"--name=$(PROGRAM_),--dir=$(BUILDDIR),--board=$(BOARD)"

bitstream: $(BITSTREAM)

.PHONY: bitstream boot default

$(PROGRAM_): $(OBJFILES)
	$(MCXX) $(CFLAGS_) $(MCXX_FLAGS_) $(MCXX_FLAGS_D_) $? -o $@ $(LDFLAGS_)

%.o: $(SRCDIR)/%.cpp
	$(MCXX) $(CFLAGS_) $(MCXX_FLAGS_) $(MCXX_FLAGS_D_) $(BITSTREAM_FLAGS) -c $^ -o $@

$(BITSTREAM): $(OBJFILES) 
	mkdir -p $(BUILDDIR) && [ $(BUILDDIR) == $(BOARD) ] || ln -sf $(BUILDDIR) $(BOARD)
	ait --verbose --name=$(PROGRAM_) --dir=$(BUILDDIR) --board=$(BOARD) --hwruntime=som --from_step=HLS --to_step=bitstream
	cp $(AVDIR)/$(PROGRAM_).xtasks.config .
	cp $(AVDIR)/$(PROGRAM_).bit .
	cp $(AVDIR)/$(PROGRAM_).autoVivado.log $(AVDIR)/$(PROGRAM_).autoVivado.bitstream.log
	cp $(AVDIR)/Vivado/$(PROGRAM_)/$(PROGRAM_).sdk/*.hdf .
