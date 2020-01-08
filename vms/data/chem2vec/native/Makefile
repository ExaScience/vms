TOPDIR=..

default: native

include ../Makefile.common

native: $(PROGRAM_)
	./$(PROGRAM_)

CFLAGS_ += -fsanitize=address -std=c++11

$(PROGRAM_): $(DEPS)
	        $(CXX) $(CFLAGS_) $(FULLSRCFILES) -o $@ $(LDFLAGS_)
