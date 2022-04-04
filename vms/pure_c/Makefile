
.PHONY: test all trace

trace: ompss/predict.prv mpi/predict.prv

ompss/predict.prv: ompss/predict
	mpirun -np 2 ./trace.sh ompss/predict 2 10000
	mpi2prv set-0/*
	mv predict.prv predict.pcf predict.row ompss/

mpi/predict.prv: mpi/predict
	mpirun -np 2 ./trace.sh mpi/predict 2 10000
	mpi2prv set-0/*
	mv predict.prv predict.pcf predict.row mpi/

ompss/predict:
	$(MAKE) -f Makefile.ompss

mpi/predict:
	$(MAKE) -f Makefile.mpi

argo/predict:
	${MAKE} -f Makefile.argo

all:
	$(MAKE) -f Makefile.plain
	$(MAKE) -f Makefile.omp
	$(MAKE) -f Makefile.gpi
	$(MAKE) -f Makefile.mpi
	$(MAKE) -f Makefile.ompss
	$(MAKE) -f Makefile.argo

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
