TOPDIR=..

default: qemu

include ../Makefile.common

MCXX   = aarch64-linux-gnu-mcxx
CFLAGS_ += -DOMPSS_SMP


QEMU_LD_LIBRARY_PATH=$(ARM64_PREFIX)/nanox/lib/instrumentation:$(ARM64_PREFIX)/libxtasks/lib:$(ARM64_PREFIX)/libxdma/lib
QEMU_LIBROOT=$(XILINX_SDK)/gnu/aarch64/lin/aarch64-linux/aarch64-linux-gnu/libc 
QEMU_CMD=qemu-aarch64 -E LD_LIBRARY_PATH=$(QEMU_LD_LIBRARY_PATH) -L $(QEMU_LIBROOT) 

qemu: $(PROGS_)
	 $(QEMU_CMD) ./predict-i
	 $(QEMU_CMD) ./predict-p
	 $(QEMU_CMD) ./predict-d
