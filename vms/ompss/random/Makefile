
CFLAGS_      = $(CFLAGS) -O1 -std=c++11 
MCXX_FLAGS_   = --ompss --variable=disable_final_clause_transformation:1 
MCXX_FLAGS_   += -k --verbose
MCXX_FLAGS_I_ = --instrument
MCXX_FLAGS_D_ = --debug -g
LDFLAGS_     = $(LDFLAGS)

SRCFILES = $(TOPDIR)/predict.cpp $(TOPDIR)/main.cpp
PROGRAM_ = predict
PROGS_   = $(PROGRAM_)-p $(PROGRAM_)-i $(PROGRAM_)-d

MCXX_  = $(CROSS_COMPILE)$(MCXX)
GCXX_  = $(CROSS_COMPILE)gxx

.PHONY: smp run bitstream boot clean

smp:
	$(MAKE) -C smp

fpga:
	$(MAKE) -C fpga

run: $(PROGS_)
	./$(PROGRAM_)-p
	./$(PROGRAM_)-d
	./$(PROGRAM_)-i

$(PROGRAM): $(SRCFILES)
	$(CXX) $(CFLAGS_) $^ -o $@ $(LDFLAGS_)

$(PROGRAM_)-p: $(SRCFILES)
	$(MCXX_) $(CFLAGS_) $(MCXX_FLAGS_) $^ -o $@ $(LDFLAGS_)

$(PROGRAM_)-i: $(SRCFILES)
	$(MCXX_) $(CFLAGS_) $(MCXX_FLAGS_) $(MCXX_FLAGS_I_) $^ -o $@ $(LDFLAGS_)

$(PROGRAM_)-d: $(SRCFILES)
	$(MCXX_) $(CFLAGS_) $(MCXX_FLAGS_) $(MCXX_FLAGS_D_) $^ -o $@ $(LDFLAGS_)

clean:
	rm -f *.o $(PROGS_)
