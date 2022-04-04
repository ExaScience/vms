TOPDIR=..

default: native

HOST_IF = ompss

include ../config.mk
include ../Makefile.common

native: $(PROGRAM_).out 

$(PROGRAM_).out: $(PROGRAM_)
	./$(PROGRAM_) >$(PROGRAM_).out 2>&1

CFLAGS_ += -fsanitize=address -std=c++11 -Wno-unknown-pragmas

$(PROGRAM_): $(ALLCODE)
	        $(CXX) $(CFLAGS_) $(FULLSRCFILES) -o $@ $(LDFLAGS_)

clean: cleancommon
