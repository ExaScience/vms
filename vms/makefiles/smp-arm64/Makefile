TOPDIR=..
HOST_IF=ompss

default: run

include ../Makefile.common
include ../Makefile.ompss

MCXX   = aarch64-linux-gnu-mcxx
CFLAGS_ += -DOMPSS_SMP -std=c++11

ifeq ($(USE_MPI),1)
MPI_BASE=/opt/bsc/arm64/mpich-3.3
CFLAGS_  += -I$(MPI_BASE)/include
LDFLAGS_ += -L$(MPI_BASE)/lib -lmpi
QEMU_LIBRARY_PATH=$(MPI_BASE)/lib
endif

QEMU_CMD=qemu-aarch64 -E LD_LIBRARY_PATH=$(QEMU_LIBRARY_PATH)
