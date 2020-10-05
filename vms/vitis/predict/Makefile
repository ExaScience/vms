.PHONY: help

help::
	$(ECHO) "Makefile Usage:"
	$(ECHO) "  make all TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform> VER=<host_cpp/host_c>"
	$(ECHO) "      Command to pick the specific files and generates the design for specified Target and Device."
	$(ECHO) "      e.g. make DEVICE=xilinx_u200_xdma_201830_2 all"
	$(ECHO) ""
	$(ECHO) "  make clean "
	$(ECHO) "      Command to remove the generated non-hardware files."
	$(ECHO) ""
	$(ECHO) "  make cleanall"
	$(ECHO) "      Command to remove all the generated files."
	$(ECHO) ""
	$(ECHO) "  make check TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform>"
	$(ECHO) "      Command to run application in emulation."
	$(ECHO) ""

TARGETS := hw
TARGET := $(TARGETS)
DEVICE := xilinx_u200_xdma_201830_2
XCLBIN := xclbin
KERNEL_NAME := predict_or_update_model

DSA := $(call device2sandsa, $(DEVICE))
BUILD_DIR := /dev/shm

BUILD_DIR_predict = $(BUILD_DIR)/$(KERNEL_NAME)

# The C++ Compiler to use: g++
CXX := g++

# The kernel Compiler to use : V++
VPP := $(XILINX_VITIS)/bin/v++

# The below are compile flags are passed to the C++ Compiler
opencl_CXXFLAGS += -g -I./ -I$(XILINX_XRT)/include -I$(XILINX_VIVADO)/include -Wall -O0 -g -std=c++17
# The below are linking flags for C++ Comnpiler
opencl_LDFLAGS += -L$(XILINX_XRT)/lib -lOpenCL -lpthread

CXXFLAGS += $(opencl_CXXFLAGS)
LDFLAGS += $(opencl_LDFLAGS)

#Host CPP FILE
HOST_CPP_SRCS += src/host.cpp

# Enable Profiling
REPORT := no
PROFILE:= yes

# Host compiler global settings
CXXFLAGS += -fmessage-length=0
LDFLAGS += -lrt -lstdc++

# Kernel compiler global settings
CLFLAGS += -t $(TARGET) --config design.cfg --save-temps

EXECUTABLES = host_sw_emu host_hw_emu host_hw
EXECUTABLE = host_$(TARGET)
EMCONFIG_DIR = $(XCLBIN)

BINARY_CONTAINERS += $(XCLBIN)/$(KERNEL_NAME).$(TARGET).xclbin
BINARY_CONTAINER_predict_OBJS += $(XCLBIN)/$(KERNEL_NAME).$(TARGET).xo

CP = cp -rf

.PHONY: all clean cleanall docs emconfig
all: $(EXECUTABLES) emconfig Makefile

.PHONY: exe
exe: $(EXECUTABLES)

KERNEL_BIN = $(XCLBIN)/$(KERNEL_NAME).$(TARGET).xclbin
KERNEL_INC = $(XCLBIN)/$(KERNEL_NAME).$(TARGET).inc
KERNEL_VAR = $(subst .,_,$(subst /,_,$(KERNEL_BIN)))

CXXFLAGS += -include $(KERNEL_INC) -DKERNEL_VAR=$(KERNEL_VAR) -DKERNEL_VAR_LEN=$(KERNEL_VAR)_len

# Building kernel
$(XCLBIN)/$(KERNEL_NAME).$(TARGET).xo: src/predict.cpp
	mkdir -p $(XCLBIN)
	$(VPP) $(CLFLAGS) --temp_dir $(BUILD_DIR_predict) -c -k $(KERNEL_NAME) -I'$(<D)' -o'$@' '$<'

$(KERNEL_BIN): $(BINARY_CONTAINER_predict_OBJS)
	mkdir -p $(XCLBIN)
	$(VPP) $(CLFLAGS) --temp_dir $(BUILD_DIR_predict) -l $(LDCLFLAGS) -o'$@' $(+)

$(KERNEL_INC): $(KERNEL_BIN)
	xxd -i $< $@

$(EXECUTABLE): $(HOST_CPP_SRCS) $(HOST_CPP_HDRS) $(KERNEL_INC)
	$(CXX) $(CXXFLAGS) $(HOST_CPP_SRCS) $(HOST_CPP_HDRS) -o '$@' $(LDFLAGS)

emconfig: $(EMCONFIG_DIR)/emconfig.json
$(EMCONFIG_DIR)/emconfig.json:
	emconfigutil --platform $(DEVICE) --od $(EMCONFIG_DIR)

check: all
ifeq ($(TARGET),$(filter $(TARGET),sw_emu hw_emu))
	$(CP) $(EMCONFIG_DIR)/emconfig.json .
	XCL_EMULATION_MODE=$(TARGET) ./$(EXECUTABLE)
#$(DEVICE)
else
	 ./$(EXECUTABLE)
#$(DEVICE)
endif

# Cleaning stuff
RMDIR = rm -rf

clean:
	-$(RMDIR) $(EXECUTABLE) $(XCLBIN)
	-$(RMDIR) TempConfig system_estimate.xtxt *.rpt
	-$(RMDIR) *.protoinst _v++_* .Xil emconfig.json dltmp* xmltmp* *.log *.jou

ECHO := @echo

#'estimate' for estimate report generation
#'system' for system report generation
ifneq ($(REPORT), no)
CLFLAGS += --report estimate
CLLDFLAGS += --report system
endif

#Generates profile summary report
ifeq ($(PROFILE), yes)
LDCLFLAGS += --profile_kernel data:all:all:all
LDCFLAGS += --profile_kernel  stall:all:all:all
LDCFLAGS += --profile_kernel exec:all:all:all
endif
