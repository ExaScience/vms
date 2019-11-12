#!/bin/bash

set -x

WORKDIR=$1
NUM_LATENT=$2
NUM_SAMPLES=$3
DATASET=chem2vec

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $SCRIPT_DIR
GIT_ROOT=$(git rev-parse --show-toplevel)
GIT_PREFIX=$(git rev-parse --show-prefix)

RUNDIR=${WORKDIR}/${DATASET}_${NUM_LATENT}_${NUM_SAMPLES}_$(date +%Y%m%d_%H%M)
mkdir -p ${RUNDIR}
cd $RUNDIR

cp -r $GIT_ROOT/vms/data/$DATASET/* ./

sed -i -E "s/num_latent = [0-9]+/num_latent = $NUM_LATENT/g" options.ini
sed -i -E "s/nsamples = [0-9]+/nsamples = $NUM_LATENT/g" options.ini

make model
cd hls


