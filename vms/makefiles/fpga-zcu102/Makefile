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

$(SRCFILES): $(FULLSRCFILES)
	ln -sf $(FULLSRCFILES) .

$(HDRFILES): $(FULLHDRFILES)
	ln -sf $(FULLHDRFILES) .

$(MODELFILES): $(FULLMODELFILES)
	ln -sf $(FULLMODELFILES) .

$(PROGRAM_): $(OBJFILES)
	$(MCXX) $(CFLAGS_) $(MCXX_FLAGS_) $(MCXX_FLAGS_D_) $? -o $@ $(LDFLAGS_)

%.o: %.cpp $(HDRFILES) $(MODELFILES)
	$(MCXX) $(CFLAGS_) $(MCXX_FLAGS_) $(MCXX_FLAGS_D_) $(BITSTREAM_FLAGS) -c $^ -o $@

$(BITSTREAM): $(SRCFILES) $(HDRFILES) $(MODELFILES)
	mkdir -p $(AVDIR)
	$(MCXX) $(CFLAGS_) $(MCXX_FLAGS_) $(MCC_FLAGS_D_) $(BITSTREAM_FLAGS) $(SRCFILES) -o $(PROGRAM_) $(LDFLAGS_)

clean:
	rm -f *.cpp *.h
	rm -f *.log
	rm -f *.o $(PROGS_) $(PROGRAM_) $(PROGRAM_)_fp $(PROGRAM_)_fx
	rm -rf zcu102
	rm -f $(PROGRAM_).xtasks.config $(PROGRAM_).bit BOOT.BIN
