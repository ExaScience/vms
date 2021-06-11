TOPDIR=..

default: native

include ../config.mk
include ../Makefile.common

native: $(PROGRAM_).out 

$(PROGRAM_).out: $(PROGRAM_)
	./$(PROGRAM_) >$(PROGRAM_).out 2>&1

CFLAGS_ += -fsanitize=address -std=c++11

$(PROGRAM_): $(DEPS)
	        $(CXX) $(CFLAGS_) $(FULLSRCFILES) -o $@ $(LDFLAGS_)
