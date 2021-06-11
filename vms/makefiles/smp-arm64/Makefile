TOPDIR=..

default: run

include ../Makefile.common
include ../Makefile.ompss

MCXX   = aarch64-linux-gnu-mcxx
CFLAGS_ += -DOMPSS_SMP -std=c++11

QEMU_CMD=qemu-aarch64 