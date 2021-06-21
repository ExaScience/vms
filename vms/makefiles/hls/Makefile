TOPDIR=..

default: hls

include ../Makefile.common
include ../Makefile.ompss

.PHONY: hls
hls:
	mkdir -p code 
	ln -sf $(addprefix ../,$(FULLMODELFILES)) code/
	ln -sf $(FULLHEADERFILES) $(FULLSRCFILES) code/
	vivado_hls vivado_hls.tcl
