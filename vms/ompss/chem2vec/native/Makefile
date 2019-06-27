TOPDIR=..

default: native

include ../Makefile.common

native: $(PROGRAM_)_fx
	./$(PROGRAM_)_fx

$(PROGRAM_)_fx: $(SRCFILES)
	        $(CXX) -DDT_FIXED $(CFLAGS_)  $^ -o $@ $(LDFLAGS_)


