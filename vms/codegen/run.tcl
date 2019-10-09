open_project -reset hls
set_top predict_or_update_model
set flags "-DT_"
append flags [lindex $argv 2]
add_files code/predict.cpp -cflags $flags
add_files -tb code/test_bench.cpp -cflags $flags
open_solution "solution1"
set_part {xczu9eg-ffvb1156-1l-i} -tool vivado
create_clock -period 5 -name default
csim_design -compiler clang
csynth_design
cosim_design
export_design -rtl verilog -format ip_catalog
