set -x 
g++ -g -O0 -I$XILINX_VIVADO/include *.cpp -o run \
	-L /usr/local/opt/systemc/lib -lsystemc
