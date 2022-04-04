TOPDIR=..
HOST_IF=ompss

default: run

include ../Makefile.common

MCXX          = mcxx
CFLAGS_      += -std=c++11
MCXX_FLAGS_   = -K --ompss-2 -Wno-deprecated-declarations -DOMPSS_TWO
MCXX_FLAGS_  += -Wno-unused-local-typedefs -Wno-unknown-pragmas

PROGS_   = $(PROGRAM_)

$(PROGRAM_): $(ALLCODE)
	$(MCXX) $(CFLAGS_) $(MCXX_FLAGS_) $(FULLSRCFILES) -o $@ $(LDFLAGS_)

run: $(PROGRAM_)
	./$(PROGRAM_)

clean: cleancommon

