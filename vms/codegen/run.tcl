open_project -reset hls
set_top predict_compound_block_c
add_files "predict.cpp smurff_const.h smurff_model.h smurff_tb.h"
add_files -tb main.cpp
open_solution "solution1"
set_part {xczu9eg-ffvb1156-1l-i} -tool vivado
create_clock -period 5 -name default
csim_design -compiler clang
csynth_design
#cosim_design
#export_design -rtl verilog -format ip_catalog
