TOPDIR=..
HOST_IF=hls

default: hls

include ../Makefile.common

.PHONY: hls clean

KERNELNAME := predict_one_block
IPZIP = hls/solution1/impl/ip/xilinx_com_hls_$(KERNELNAME)_1_0.zip

hls: $(IPZIP)

$(IPZIP): $(ALLCODE)
	ln -sf $(ALLCODE) .
	vitis_hls vitis_hls.tcl

sim: $(IPZIP)
	vitis_hls vitis_hls_sim.tcl

clean:
	rm -f $(LOCALFILES)
	rm -rf hls
