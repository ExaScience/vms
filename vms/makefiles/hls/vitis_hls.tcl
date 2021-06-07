open_project -reset hls
set_top predict_or_update_model
add_files "code/predict.cpp code/non_ocl.cpp"
add_files -tb code/host.cpp -cflags "-std=c++0x"
open_solution "solution1"
set_part xcu200-fsgd2104-2-e
create_clock -period 5 -name default
csim_design
csynth_design
cosim_design
export_design -rtl verilog -format ip_catalog
