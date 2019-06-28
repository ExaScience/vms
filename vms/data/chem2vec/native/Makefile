TOPDIR=..

default: native

include ../Makefile.common

native: $(PROGRAM_)_fx $(PROGRAM_)_fp
	./$(PROGRAM_)_fx
	./$(PROGRAM_)_fp

$(PROGRAM_)_fp: $(SRCFILES)
	        $(CXX) -DDT_FLOAT $(CFLAGS_)  $^ -o $@ $(LDFLAGS_)

$(PROGRAM_)_fx: $(SRCFILES)
	        $(CXX) -DDT_FIXED $(CFLAGS_)  $^ -o $@ $(LDFLAGS_)


