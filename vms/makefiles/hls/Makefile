TOPDIR=..

default: hls

include ../Makefile.common
include ../Makefile.ompss

.PHONY: hls clean

KERNELNAME := predict_or_update_model
IPZIP = hls/solution1/impl/ip/xilinx_com_hls_$(KERNELNAME)_1_0.zip

hls: $(IPZIP)

$(IPZIP): $(ALLCODE)
	ln -sf $(ALLCODE) .
	vitis_hls vitis_hls.tcl

clean:
	rm -f $(LOCALFILES)
	rm -rf hls
