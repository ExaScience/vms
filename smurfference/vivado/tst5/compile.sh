#!/bin/sh
XILINX_VIVADO_HLS=/home/vanderaa/Xilinx/Vivado_HLS/2017.2

set -x 
g++ -g -O0 -std=c++11 -I$XILINX_VIVADO_HLS/include *.cpp -o run
