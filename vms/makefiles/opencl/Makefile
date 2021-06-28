
TOPDIR=..
.PHONY:  clean all check

all: 
	$(MAKE) -C sw_emu 
	$(MAKE) -C hw_emu 
	$(MAKE) -C hw

check: 
	$(MAKE) -C sw_emu check
	$(MAKE) -C hw_emu check
	$(MAKE) -C hw check

clean:
	$(MAKE) -C sw_emu cleancommon
	$(MAKE) -C hw_emu cleancommon
	$(MAKE) -C hw cleancommon