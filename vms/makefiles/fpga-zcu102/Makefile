TOPDIR=..

BOARD=zcu102

default: bitstream

include ../Makefile.common
include ../Makefile.ompss

MCXX   = aarch64-linux-gnu-fpgacxx
CFLAGS_ += -DOMPSS_FPGA -std=c++11

AVDIR = $(PWD)/$(BOARD)
BITSTREAM = $(PROGRAM_).bit
BITSTREAM_FLAGS=--bitstream-generation --Wf,"--name=$(PROGRAM_),--dir=$(AVDIR),--board=$(BOARD),--clock=200,--hwruntime=som"

bitstream: $(BITSTREAM)

.PHONY: bitstream boot default

$(PROGRAM_): $(OBJFILES)
	$(MCXX) $(CFLAGS_) $(MCXX_FLAGS_) $(MCXX_FLAGS_D_) $? -o $@ $(LDFLAGS_)

%.o: $(SRCDIR)/%.cpp
	$(MCXX) $(CFLAGS_) $(MCXX_FLAGS_) $(MCXX_FLAGS_D_) $(BITSTREAM_FLAGS) -c $^ -o $@

$(BITSTREAM): $(FULLSRCFILES)
	mkdir -p $(AVDIR)
	$(MCXX) $(CFLAGS_) $(MCXX_FLAGS_) $(MCC_FLAGS_D_) $(BITSTREAM_FLAGS) $(FULLSRCFILES) -o $(PROGRAM_) $(LDFLAGS_)