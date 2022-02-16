TOPDIR=..

default: native

PROGRAM = predict
SRCFILES = predict.c test_bench.c mpi.c ompss.c
OBJFILES = $(SRCFILES:.c=.o)
HDRFILES = predict.h vms_const.h vms_tb.h

PROGDEPS =  counters.o $(OBJFILES) $(HDRFILES) Makefile

native: $(PROGRAM)
	./$(PROGRAM) 2 2

EXTRA_FLAGS = -O3 -march=native -g

$(PROGRAM): $(PROGDEPS)
	$(CXX) $(OBJFILES) counters.o -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(EXTRA_FLAGS) $(CFLAGS) $(CSRCFILES) -c $^ -o $@ 

counters.o: counters.cpp
	$(CXX) $(CFLAGS) -c $^ -o $@ 

.PHONY: clean

clean:
	rm -f *.o $(PROGRAM)

