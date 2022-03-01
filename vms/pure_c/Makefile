
.PHONY: test

test:
	$(MAKE) -f Makefile.ompss
	taskset -c 0-3 ./ompss/predict 2 20000
	$(MAKE) -f Makefile.omp
	taskset -c 0-3 ./omp/predict 2 20000

include Makefile.common