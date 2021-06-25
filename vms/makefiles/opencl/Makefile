
TOPDIR=..

include ../config.mk
include ../Makefile.common

.PHONY:  clean all

all: 
	$(MAKE) -C sw_emu check
	$(MAKE) -C hw_emu check
	$(MAKE) -C hw

clean:
	$(MAKE) -C sw_emu cleancommon
	$(MAKE) -C hw_emu cleancommon
	$(MAKE) -C hw cleancommon