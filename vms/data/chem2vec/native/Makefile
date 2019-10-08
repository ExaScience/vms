TOPDIR=..

default: native

include ../Makefile.common

native: $(PROGRAM_)_fx $(PROGRAM_)_fp $(PROGRAM_)_mx
	./$(PROGRAM_)_fx
	./$(PROGRAM_)_fp
	./$(PROGRAM_)_mx

CFLAGS_ += -fsanitize=address


$(PROGRAM_)_fp: $(DEPS)
	        $(CXX) -DDT_FLOAT $(CFLAGS_) $(FULLSRCFILES) -o $@ $(LDFLAGS_)

$(PROGRAM_)_fx: $(DEPS)
	        $(CXX) -DDT_FIXED $(CFLAGS_) $(FULLSRCFILES) -o $@ $(LDFLAGS_)

$(PROGRAM_)_mx: $(DEPS)
	        $(CXX) -DDT_MIXED $(CFLAGS_) $(SC_CXXFLAGS) $(FULLSRCFILES) -o $@ $(LDFLAGS_) $(SC_LDFLAGS)