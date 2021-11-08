default: run

PROGRAM = predict
SRCFILES = predict.c test_bench.c
HDRFILES = ../predict.h 
MODELFILES = ../vms_const.h ../vms_tb.h

OBJFILES := ${SRCFILES:.c=.o} 

DEPS = $(HDRFILES) $(MODELFILES) Makefile 

CFLAGS += -g -O3
CFLAGS += -DBLOCK_SIZE=4096

run: $(PROGRAM)
	./$(PROGRAM) $(ARGS)

$(PROGRAM): $(OBJFILES)
	        $(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: ../%.c $(DEPS)
	        $(CC) $(CFLAGS)  -c -o $@ $<

.PHONY: clean

clean: 
	rm -f $(PROGRAM) *.o
