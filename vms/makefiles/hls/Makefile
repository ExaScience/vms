TOPDIR=..

default: hls

include ../Makefile.common
include ../Makefile.ompss

.PHONY: hls clean

hls: local
	vivado_hls vivado_hls.tcl

clean:
	rm -f $(LOCALCODE)
	rm -rf hls
