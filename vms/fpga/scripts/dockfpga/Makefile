
VERSION := 3.1.3-euroexa
OMPSS_FPGA_URL := https://euroexa:exaCRDBompss@ompssatfpga.bsc.es/downloads-euroexa/docker/docker_$(VERSION).tar.gz
OMPSS_FPGA_TAR := docker_$(VERSION).tar.gz
OMPSS_FPGA_IMAGE_TAR := ompss_at_fpga_$(VERSION)/ompss_at_fpga_$(VERSION).tar
OMPSS_FILES_TAR := builddir/ompss_at_fpga.tar.gz
OMPSS_IMAGE_NAME := ompss_at_fpga:$(VERSION)
CROSS_IMAGE_NAME := dockfpga

all: $(CROSS_IMAGE_NAME)

$(OMPSS_FPGA_TAR): 
	wget -O $(OMPSS_FPGA_TAR) $(OMPSS_FPGA_URL)

$(OMPSS_FPGA_IMAGE_TAR): $(OMPSS_FPGA_TAR)
	tar xmzf $(OMPSS_FPGA_TAR) 

$(OMPSS_FILES_TAR): $(OMPSS_FPGA_IMAGE_TAR) 
	docker load -i $(OMPSS_FPGA_IMAGE_TAR)
	docker run $(OMPSS_IMAGE_NAME) tar czf - /opt  >$(OMPSS_FILES_TAR) 

$(CROSS_IMAGE_NAME): $(wildcard builddir/*) $(OMPSS_FILES_TAR)
	cd builddir && docker build -t $(CROSS_IMAGE_NAME) .
	docker run $(CROSS_IMAGE_NAME) >$(CROSS_IMAGE_NAME)
	chmod +x $(CROSS_IMAGE_NAME)

