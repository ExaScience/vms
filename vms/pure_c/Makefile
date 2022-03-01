
.PHONY: test all


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

include Makefile.common