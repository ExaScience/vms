
TOPDIR=..
.PHONY:  clean all check

all: 
	cd sw_emu && $(MAKE) >make.log 2>&1
	cd hw_emu && $(MAKE) >make.log 2>&1
	cd hw     && $(MAKE) >make.log 2>&1

check: 
	cd sw_emu && $(MAKE) check >make.log 2>&1
	cd hw_emu && $(MAKE) check >make.log 2>&1
	cd hw     && $(MAKE) check >make.log 2>&1

clean:
	cd sw_emu && $(MAKE) cleancommon
	cd hw_emu && $(MAKE) cleancommon
	cd hw     && $(MAKE) cleancommon
