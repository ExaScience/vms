TOPDIR=..

default: native

include ../Makefile.common

native: $(PROGRAM_)
	./$(PROGRAM_)

$(PROGRAM_): $(SRCFILES)
	        $(CXX) $(CFLAGS_)  $^ -o $@ $(LDFLAGS_)


