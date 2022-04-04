
PLATFORM=5
SRCDIR=src


MAIN=$(MAXNAME).$(MAXNAME)Manager
DFE_NAME=MAX5C
MAXNAME=predict

CLASSPATH=$(MAXCOMPILERDIR)/lib/Max$(PLATFORM)Platform.jar:$(MAXCOMPILERDIR)/lib/MaxCompiler.jar:la4j.jar:build:src

SRCFILES=$(wildcard $(SRCDIR)/*/*.maxj)
CLSFILES=$(patsubst src/%.maxj,build/%.class,$(SRCFILES))

DFEMAXFILE=./$(MAXNAME)_$(DFE_NAME)_DFE/results/$(MAXNAME).max
SIMMAXFILE=./$(MAXNAME)_$(DFE_NAME)_DFE_SIM/results/$(MAXNAME).max
SIMFILE=

.PHONY: sim hw

sim: $(SIMMAXFILE)

hw: $(DFEMAXFILE)

$(DFEMAXFILE): $(CLSFILES)
	java -cp $(CLASSPATH) $(MAIN) DFEModel=MAIA maxFileName=$(MAXNAME) target=DFE  enableMPCX=true

$(SIMMAXFILE): $(CLSFILES)
	java -cp $(CLASSPATH) $(MAIN) DFEModel=MAIA maxFileName=$(MAXNAME) target=DFE_SIM  enableMPCX=true

$(CLSFILES): la4j.jar $(SRCFILES)
	maxjc -d build -1.7 -cp $(CLASSPATH) $(SRCFILES)

la4j.jar:
	wget -O la4j.jar http://central.maven.org/maven2/org/la4j/la4j/0.6.0/la4j-0.6.0.jar



