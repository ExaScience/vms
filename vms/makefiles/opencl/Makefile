
TOPDIR=..
.PHONY:  clean all check

all: 
	$(MAKE) -C sw_emu >make.log 2>&1
	$(MAKE) -C hw_emu >make.log 2>&1
	$(MAKE) -C hw >make.log 2>&1

check: 
	$(MAKE) -C sw_emu check >make.log 2>&1
	$(MAKE) -C hw_emu check >make.log 2>&1
	$(MAKE) -C hw check >make.log 2>&1

clean:
	$(MAKE) -C sw_emu cleancommon
	$(MAKE) -C hw_emu cleancommon
	$(MAKE) -C hw cleancommon