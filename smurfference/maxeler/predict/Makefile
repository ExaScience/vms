
PLATFORM=5
SRCDIR=src

CLASSPATH=$(MAXCOMPILERDIR)/lib/Max$(PLATFORM)Platform.jar:$(MAXCOMPILERDIR)/lib/MaxCompiler.jar:la4j.jar:src:build

MAIN=predict.predictManager

SRCFILES=$(wildcard $(SRCDIR)/*/*.maxj)
CLSFILES=$(patsubst %.maxj,%.class,$(SRCFILES))

DFEMAXFILE=./predict_MAX5C_DFE/results/predict.max
SIMMAXFILE=./predict_MAX5C_DFE_SIM/results/predict.max
SIMFILE=

all: $(DFEMAXFILE) $(SIMMAXFILE)

$(DFEMAXFILE): $(CLSFILES)
	java -XX:+UseSerialGC -Xmx2048m -cp $(CLASSPATH) $(MAIN) DFEModel=MAIA maxFileName=predict target=DFE  enableMPCX=true

$(SIMMAXFILE): $(CLSFILES)
	java -XX:+UseSerialGC -Xmx2048m -cp $(CLASSPATH) $(MAIN) DFEModel=MAIA maxFileName=predict target=DFE_SIM  enableMPCX=true

$(CLSFILES): la4j.jar $(SRCFILES)
	maxjc -d build -1.7 -cp $(CLASSPATH) $(SRCFILES)

la4j.jar:
	wget -O la4j.jar http://central.maven.org/maven2/org/la4j/la4j/0.6.0/la4j-0.6.0.jar



