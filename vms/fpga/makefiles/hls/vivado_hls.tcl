open_project -reset hls
set_top predict_one_block
add_files ompss.cpp  -cflags "-std=c++11"
add_files -tb main.cpp -cflags "-std=c++11"
open_solution "solution1"
# zcu102
# set_part {xczu9eg-ffvb1156-1l-i} -tool vivado
# crdb
set_part {xcvu9p-fsgd2104-2-i} -tool vivado
create_clock -period 5 -name default
csim_design -compiler clang
csynth_design
cosim_design -enable_dataflow_profiling
export_design -rtl verilog -format ip_catalog
