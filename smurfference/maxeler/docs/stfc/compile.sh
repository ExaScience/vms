#!/bin/sh

# name
#$ -N lr_compile
# shell
#$ -S /bin/sh

# execute script from the current directory
#$ -cwd

# set up the necessary environment
source /panfs/panfs.maxeler/maxeler/maxcompiler-2014.1.1/settings.sh
export PATH=/panfs/panfs.maxeler/maxeler/altera/13.1/quartus/bin:${PATH}
export MAXELEROSDIR=/opt/maxeler/maxeleros

# build the code
pushd LinearRegression_group/RunRules/DFE
# set display to :99 because that is where Xvfb is running
export DISPLAY=":99"
make RUNRULE=DFE build
popd