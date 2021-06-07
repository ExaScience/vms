open_project -reset hls
set_top predict_or_update_model
add_files "code/predict.cpp code/non_ocl.cpp"  -cflags "-std=c++11"
add_files -tb code/host.cpp -cflags "-std=c++11"
open_solution "solution1"
set_part {xczu9eg-ffvb1156-1l-i} -tool vivado
create_clock -period 5 -name default
csim_design -compiler clang
csynth_design
cosim_design
export_design -rtl verilog -format ip_catalog
