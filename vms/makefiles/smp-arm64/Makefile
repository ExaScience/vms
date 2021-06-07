TOPDIR=..

default: qemu

include ../Makefile.common
include ../Makefile.ompss

MCXX   = aarch64-linux-gnu-mcxx
CFLAGS_ += -DOMPSS_SMP -std=c++11


QEMU_CMD=qemu-aarch64 

qemu: $(PROGS_)
	 $(QEMU_CMD) ./predict-i
	 $(QEMU_CMD) ./predict-p
	 $(QEMU_CMD) ./predict-d
