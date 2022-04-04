
TOPDIR=..
.PHONY:  clean all check

all: 
	cd sw_emu && $(MAKE)
	cd hw_emu && $(MAKE)
	cd hw     && $(MAKE)

check: 
	cd sw_emu && $(MAKE) check
	cd hw_emu && $(MAKE) check
	cd hw     && $(MAKE) check

clean:
	cd sw_emu && $(MAKE) clean
	cd hw_emu && $(MAKE) clean
	cd hw     && $(MAKE) clean
