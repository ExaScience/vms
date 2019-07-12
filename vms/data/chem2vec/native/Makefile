TOPDIR=..

default: native

include ../Makefile.common

native: $(PROGRAM_)_fx $(PROGRAM_)_fp
	./$(PROGRAM_)_fx
	./$(PROGRAM_)_fp

CFLAGS_ += -fsanitize=address

$(PROGRAM_)_fp: $(FULLSRCFILES) $(FULLHDRFILES) $(FULLMODELFILES)
	        $(CXX) -DDT_FLOAT $(CFLAGS_) $(FULLSRCFILES) -o $@ $(LDFLAGS_)

$(PROGRAM_)_fx: $(FULLSRCFILES) $(FULLHDRFILES) $(FULLMODELFILES)
	        $(CXX) -DDT_FIXED $(CFLAGS_) $(FULLSRCFILES) -o $@ $(LDFLAGS_)


