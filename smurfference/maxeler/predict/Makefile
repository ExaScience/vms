
PLATFORM=5
SRCDIR=src


CLASSPATH=$(MAXCOMPILERDIR)/lib/Max$(PLATFORM)Platform.jar:$(MAXCOMPILERDIR)/lib/MaxCompiler.jar:la4j.jar:src

MAIN=predict.predictManager

max: java
	java -XX:+UseSerialGC -Xmx2048m -cp $(CLASSPATH) $(MAIN) DFEModel=MAIA maxFileName=predict target=DFE_SIM  enableMPCX=true

java: la4j.jar
	maxjc -1.8 -cp $(CLASSPATH) $(SRCDIR)/

la4j.jar:
	wget -O la4j.jar http://central.maven.org/maven2/org/la4j/la4j/0.6.0/la4j-0.6.0.jar

