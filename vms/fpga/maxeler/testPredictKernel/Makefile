CARD := MAX5C
CC := gcc

OBJDIR := obj
LIBDIR := lib
BINDIR := bin
INCDIR := inc
MAXINCDIR := $(INCDIR)/max
MAXOBJDIR := obj/max
MAXDIR := max

CFLAGS := -fPIC -std=gnu99 -Wall -Wextra
LFLAGS :=

CPUPROJECT := $(notdir $(shell pwd))
EXE := $(CPUPROJECT)
SONAME := lib$(shell echo $CPUPROJECT | tr '[:upper:]' '[:lower:]').so

TRIMEDUSER := $(shell echo $(USER) | head -c 10)
TRIMEDPROJECT := $(shell echo $(CPUPROJECT) | head -c 15)

SIMNAME := $(TRIMEDUSER)$(TRIMEDPROJECT)Sim

USE_SLIC := 1
include $(MAXCOMPILERDIR)/lib/Makefile.include
include .cproperties
include .maxproperties

CFLAGS := $(CFLAGS) -I$(MAXINCDIR) $(MAXCOMPILER_INC) $(MACROS) $(INCPATH)
LFLAGS = $(LIBPATH) $(LIBS) $(MAXCOMPILER_LIBS)

ifeq ($(MAXFILES), )
	BUILD_LOCATIONS += $(foreach build_loc, $(BUILD_FILES), $(shell tail -n 1 $(build_loc)))
	BUILD_LOCATIONS += $(BUILD_DIRS)
	MAXFILES += $(foreach build_loc, $(BUILD_LOCATIONS), $(wildcard $(build_loc)/results/*.max))
	MAXFILES += $(SELECTED_MAXFILES)
endif

MAX = $(patsubst %, $(MAXOBJDIR)/%.o, $(basename $(notdir $(MAXFILES))))
MAXINC = $(patsubst %, $(MAXINCDIR)/%.h, $(basename $(notdir $(MAXFILES))))

_OBJ = $(shell find $(SRCPATH) -name '*.c' -o -name '*.cpp')
OBJ = $(patsubst %.c, $(OBJDIR)/%.o, $(_OBJ)) $(MAX)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)

$(MAX): $(MAXFILES)
	$(eval MAXFILE = $(filter %$(basename $(@F)).max, $(MAXFILES)))
	$(if $(wildcard $(MAXFILE)), , $(error $(ACTIVE_BUILD_NOT_SELECTED)))
	@mkdir -p $(@D)
	"$(MAXCOMPILERDIR)/bin/sliccompile" "$(MAXFILE)" $(@)

$(MAXINCDIR)/%.h:
	$(eval MAXFILE = $(filter %$(basename $(@F)).max, $(MAXFILES)))
	$(if $(wildcard $(MAXFILE)), , $(error $(ACTIVE_BUILD_NOT_SELECTED)))
	@mkdir -p $(@D)
	"$(MAXCOMPILERDIR)/bin/sliccompile" -t cheader -d $(MAXINCDIR) "$(MAXFILE)"

$(BINDIR)/$(EXE): $(MAXINC) $(OBJ)
	@mkdir -p $(@D)
	$(CC) -o $(@) $(OBJ) $(CFLAGS) $(LFLAGS)

$(LIBDIR)/$(SONAME): $(MAXINC) $(OBJ)
	@mkdir -p $(@D)
	$(CC) -shared -o $(@) $(OBJ) $(CFLAGS) $(LFLAGS)

generate_maxfiles_header:
	@rm -f $(INCDIR)/Maxfiles.h
	@printf	"\
	/* File Maxfiles.h  */ \n \
	#ifndef MAXFILES_H     \n \
	#define MAXFILES_H     \n \
                               \n \
	/**                    \n \
	 * Add .max files here \n \
	 */                    \n" >> $(INCDIR)/Maxfiles.h
	$(foreach header, $(MAXINC), printf "#include \"$(notdir $(header))\"\n" >> $(INCDIR)/Maxfiles.h;)
	@printf "\n#endif /* !MAXFILES_H */ \n" >> $(INCDIR)/Maxfiles.h 

all: run_sim

build: $(BINDIR)/$(EXE)

create_so: $(LIBDIR)/$(SONAME)

run: build ; @$(BINDIR)/$(EXE)
run_dfe: export LD_LIBRARY_PATH = $(MAXELEROSDIR)/lib
run_dfe: ; @make run
run_sim: export LD_LIBRARY_PATH = $(MAXCOMPILERDIR)/lib/maxeleros-sim/lib
run_sim: export SLIC_CONF = use_simulation=$(SIMNAME)
run_sim: ; @make start_sim run stop_sim

setup_event_monitoring:
	nohup maxeventlog --name ${DEFAULT_EVENTLOG_SERVER} -v4 &

end_event_monitoring:
	$(shell [ -d ${RESULT_DIR} ] || mkdir -p ${RESULT_DIR})
	cd ${RESULT_DIR} && maxeventlogrendersvg -n ${DEFAULT_EVENTLOG_SERVER}
	maxeventlog --name ${DEFAULT_EVENTLOG_SERVER} --stop

%_with_event_monitoring: setup_event_monitoring % end_event_monitoring; @echo

start_sim: ; @'$(MAXCOMPILERDIR)/bin/maxcompilersim' -n $(SIMNAME) -c $(CARD) restart
stop_sim: ; @'$(MAXCOMPILERDIR)/bin/maxcompilersim' -n $(SIMNAME) stop
restart_sim: stop_sim start_sim

help: ; @echo "$$USAGE"

clean: ; rm -rf $(BINDIR)/* $(OBJDIR)/* $(MAXOBJDIR)/*.o *~ core*

.PHONY: all build clean help restart_sim run run_dfe run_sim $(MAX)
.PHONY: start_sim stop_sim create_so

define ACTIVE_BUILD_NOT_SELECTED_IN_MAXIDE

[ERROR] No Maxfile $(MAXFILE) found.
[HINT] Please create the Maxfile by building the DFE Project associated with it 
[HINT] (this can be done by clicking in the 'Build <target>' button in the DFE project),
[HINT] or go to Maxfiles Manager and select existing maxfiles (this can be done
[HINT] by right clicking on the CPU project and selecting 'Set up Maxfiles' option)

endef
export ACTIVE_BUILD_NOT_SELECTED_IN_MAXIDE

define ACTIVE_BUILD_NOT_SELECTED_IN_TERMINAL

[ERROR] No Maxfile $(MAXFILE) found.
[HINT] To specify different Maxfile location
[HINT] set MAXFILES environmental variable.

endef
export ACTIVE_BUILD_NOT_SELECTED_IN_TERMINAL

ifeq ($(MAXIDE),true)
  ACTIVE_BUILD_NOT_SELECTED =  $(ACTIVE_BUILD_NOT_SELECTED_IN_MAXIDE)
else
  ACTIVE_BUILD_NOT_SELECTED = $(ACTIVE_BUILD_NOT_SELECTED_IN_TERMINAL)
endif

define USAGE
Usage:
    make                     Builds Project using .max file of specified type
                             This target is equivalent to:
                             make build
    make build               Builds Project using .max file of specified type
    make clean               Cleans Project
    make create_so           Creates shared library using .max file
                             of specified type.
    make help                Prints Help message
    make restart_sim         Restart Simulation
    make run                 Run created exe file
    make run_dfe             Run Project using dfe .max file
    make run_sim             Run Project using sim .max file inside Simulation
    make start_sim           Start Simulation
    make stop_sim            Stop Simulation

Options:
    MAXFILES	The location of the .max file
endef
export USAGE