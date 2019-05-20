set -x 
g++ -g -O0 -I$XILINX_VIVADO/include *.cpp -o run -DDT_DOUBLE \
	-L /usr/local/opt/systemc/lib -lsystemc
