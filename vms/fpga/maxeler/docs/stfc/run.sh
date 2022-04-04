#!/bin/sh

# name
#$ -N lr_run
# shell
#$ -S /bin/sh

# execute script from the current directory
#$ -cwd

# set up the necessary environment
source /panfs/panfs.maxeler/maxeler/maxcompiler-2014.1.1/settings.sh
export PATH=/panfs/panfs.maxeler/maxeler/altera/13.1/quartus/bin:${PATH}
export MAXELEROSDIR=/opt/maxeler/maxeleros

# run the code
pushd workspace/sampler_dfe_rand2/CPUCode
maxorch -r 192.168.74.198 -c reserve -i myres -t  "MAIA"
SLIC_CONF="default_engine_resource=myres^192.168.74.198" make RUNRULE=DFE run
maxorch -r 192.168.74.198 -c unreserve -i myres
popd
