Hi Tom,

When doing compilation and running of MaxJ design we are doing it via
batch scheduling system (SGE in this case). Could you please use
attached two scripts for compiling (compile.sh) and running (run.sh) of
your designs?

For compiling in compile.sh just replace directory after pushd on line
17 with the directory where run rules for your DFE are, while in run.sh
on line 17 replace with directory where your CPU code sits. If you are
using power library you would need to remove lines 18 and 20 as
reservation is created automatically by power library in the call
power_init(). Of course in SLIC_CONF after default_engine_resource put
down name of your reservation.

To run the scripts you just need to issue command from login node:

qsub -q maxeler.q -l cores=1,memory=64 run.sh

If you are running more then one thread for calculating cost tables
(MPPRThreads parameter in RunRules.settings) then you should specify
number of cores in the above command to be equal to number specified in
MPPRThreads.

To easy development you can mount locally remote filesystem using sshfs.
The following command should work on Linux:

shfs -o allow_other,IdentityFile=~/.ssh/id_rsa
mxp43-nnm18@193.62.123.17:/panfs/panfs.maxeler/home/HCEEC005/nnm18/mpx43-nnm18
/mnt/maxeler-home

You can find the power library at
/panfs/panfs.maxeler/packages/mpcx_power. There is a README.md file that
explains what calls you need to make in order to instrument your CPU
code and how to link it against your design.

If you encounter any issues or problems please let me know.

Best,
Milos.


