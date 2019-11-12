#!/bin/bash

set -x

WORKDIR=$PWD/work
DATASET=chem2vec

GIT_ROOT=$(git rev-parse --show-toplevel)
GIT_PREFIX=$(git rev-parse --show-prefix)

for NUM_LATENT in 8 16 32 
do
    for NUM_SAMPLES in 8 16 32
    do
        cd $WORKDIR

        RUNDIR=${WORKDIR}/${DATASET}_${NUM_LATENT}_${NUM_SAMPLES}_$(date +%Y%m%d_%H%M)
        mkdir -p ${RUNDIR}
        cd $RUNDIR

        cp -r $GIT_ROOT/vms/data/$DATASET/* ./

        sed -i -E "s/num_latent = [0-9]+/num_latent = $NUM_LATENT/g" options.ini
        sed -i -E "s/nsamples = [0-9]+/nsamples = $NUM_LATENT/g" options.ini

        cat <<EOF >run.sh
        make model
        cd hls
        ./make.sh
EOF

    done
done



