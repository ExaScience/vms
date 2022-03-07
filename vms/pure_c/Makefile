
.PHONY: test all TRACE

trace: predict.prv

predict.prv: ompss/predict
	mpirun -np 2 taskset -c 0-8 ./trace.sh ./ompss/predict 2 10000
	mpi2prv set-0/*

ompss/predict:
	$(MAKE) -f Makefile.ompss

all:
	$(MAKE) -f Makefile.plain
	$(MAKE) -f Makefile.omp
	$(MAKE) -f Makefile.gpi
	$(MAKE) -f Makefile.mpi
	$(MAKE) -f Makefile.ompss

test:
	$(MAKE) -f Makefile.ompss
	$(MAKE) -f Makefile.omp

	taskset -c 0-3 ./ompss/predict 2 20000
	taskset -c 0-3 ./omp/predict 2 20000

.PHONY: clean

clean:
	rm -f */*.o */predict mcc_*.c *out
	rm -f *.mpits *.prv *.pcf *.row
	rm -rf set-0
