TOPDIR=..

default: native

PROGRAM = predict
SRCFILES = predict.c test_bench.c mpi.c ompss.c
HDRFILES = predict.h vms_const.h vms_tb.h

DEPS = $(SRCFILES) $(HDRFILES) Makefile

native: $(PROGRAM)
	./$(PROGRAM) 2 2

EXTRA_FLAGS = -O3 -march=native -g

$(PROGRAM): $(DEPS)
	        $(CC) $(EXTRA_FLAGS) $(CFLAGS) $(SRCFILES) -o $@ $(LDFLAGS)

.PHONY: clean

clean:
	rm -f *.o $(PROGRAM)

