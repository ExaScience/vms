TOPDIR=..

default: hls

include ../Makefile.common
include ../Makefile.ompss

.PHONY: hls clean

KERNELNAME := predict_or_update_model
IPZIP = hls/solution1/impl/ip/xilinx_com_hls_$(KERNELNAME)_1_0.zip

hls: $(IPZIP)

$(IPZIP):
	ln -sf $(ALLCODE) .
	vivado_hls vivado_hls.tcl

clean:
	rm -f $(LOCALCODE)
	rm -rf hls
