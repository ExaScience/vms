TOPDIR=..

default: native

include ../Makefile.common

native: $(PROGRAM_)_fx $(PROGRAM_)_fp
	./$(PROGRAM_)_fx
	./$(PROGRAM_)_fp

CFLAGS_ += -fsanitize=address

$(PROGRAM_)_fp: $(SRCFILES) $(HEADERFILES)
	        $(CXX) -DDT_FLOAT $(CFLAGS_)  $^ -o $@ $(LDFLAGS_)

$(PROGRAM_)_fx: $(SRCFILES) $(HEADERFILES)
	        $(CXX) -DDT_FIXED $(CFLAGS_)  $^ -o $@ $(LDFLAGS_)


