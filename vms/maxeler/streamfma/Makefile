
PLATFORM=5
SRCDIR=src

CLASSPATH=$(MAXCOMPILERDIR)/lib/Max$(PLATFORM)Platform.jar:$(MAXCOMPILERDIR)/lib/MaxCompiler.jar:la4j.jar:src:build

MAIN=streamfma.StreamFMAManager 
MAXFILE=streamfma
DFE_NAME=MAIA

SRCFILES=$(wildcard $(SRCDIR)/*.maxj)
CLSFILES=$(patsubst %.maxj,%.class,$(SRCFILES))

DFEMAXFILE=./streamfma_$(DFE_NAME)_DFE/results/streamfma.max
SIMMAXFILE=./streamfma_$(DFE_NAME)_DFE_SIM/results/streamfma.max
SIMFILE=

sim: $(SIMMAXFILE)
hw: $(DFEMAXFILE)

$(DFEMAXFILE): $(CLSFILES)
	java -XX:+UseSerialGC -Xmx2048m -cp $(CLASSPATH) $(MAIN) DFEModel=MAIA maxFileName=streamfma target=DFE_NAME  enableMPCX=true

$(SIMMAXFILE): $(CLSFILES)
	java -XX:+UseSerialGC -Xmx2048m -cp $(CLASSPATH) $(MAIN) DFEModel=MAIA maxFileName=streamfma target=DFE_SIM  enableMPCX=true

$(CLSFILES): la4j.jar $(SRCFILES)
	maxjc -d build -1.7 -cp $(CLASSPATH) $(SRCFILES)

la4j.jar:
	wget -O la4j.jar http://central.maven.org/maven2/org/la4j/la4j/0.6.0/la4j-0.6.0.jar



