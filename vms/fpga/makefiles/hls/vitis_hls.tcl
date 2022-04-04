open_project -reset hls
set_top predict_one_block
add_files hls.cpp  -cflags "-std=c++11"
add_files -tb "main.cpp hls.cpp mpi.cpp" -cflags "-std=c++11"
open_solution "solution1" -flow_target vivado
# zcu102
# set_part {xczu9eg-ffvb1156-1l-i}
# crdb
set_part {xcvu9p-fsgd2104-2-i} 
create_clock -period 5 -name default
config_interface -m_axi_alignment_byte_size 64 -m_axi_max_widen_bitwidth 512
csim_design
csynth_design
export_design -rtl verilog -format ip_catalog
