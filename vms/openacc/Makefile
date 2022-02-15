default: run

PROGRAM = predict
SRCFILES = predict.c test_bench.c
HDRFILES = predict.h 
MODELFILES = vms_const.h vms_tb.h

OBJFILES := ${SRCFILES:.c=.o} 

DEPS = $(HDRFILES) $(MODELFILES) Makefile 

PGCC = pgimcc
CCFLAGS += -g
CCFLAGS += --openacc --ompss-2 -O3
# CCFLAGS += -fast -Minfo=accel -ta=tesla
CCFLAGS += -DBLOCK_SIZE=10000
CCFLAGS += -DOMPSS_OPENACC

run: $(PROGRAM)
	./$(PROGRAM) $(ARGS)

$(PROGRAM): $(OBJFILES)
	        $(PGCC) $(CCFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c $(DEPS)
	        $(PGCC) $(CCFLAGS)  -c -o $@ $<

.PHONY: clean

clean: 
	rm -f $(PROGRAM) *.o
