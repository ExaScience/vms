#!/bin/bash

set -x

BASEDIR=$PWD
DATASET=chem2vec

for NUM_LATENT in 8 16 32 
do
    for NUM_SAMPLES in 8 16 32
    do

        NEWDIR=${DATASET}_${NUM_LATENT}_${NUM_SAMPLES}
        mkdir ${NEWDIR}
        cp -r $DATASET/* $NEWDIR/
        cd $NEWDIR
        sed -i -E "s/num_latent = [0-9]+/num_latent = $NUM_LATENT/g" options.ini
        sed -i -E "s/nsamples = [0-9]+/nsamples = $NUM_LATENT/g" options.ini
        cd $BASEDIR
    done
done



