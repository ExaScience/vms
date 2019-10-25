#!/bin/bash

set -e

DATADIR=../..
TOPDIR=$DATADIR/../..

function make_hls {
    DIR=$1
    FLAGS=${DIR^^}

    rm -fr $DIR && mkdir $DIR && cd $DIR
    mkdir code
    cp $DATADIR/code/* code/

    echo "#define DT_${DIR^^}" | cat - $DATADIR/code/predict.h >code/predict.h

    vivado_hls $TOPDIR/codegen/run.tcl >/dev/null

    cd ..
}

DIRS=$*
DIRS=${DIRS:-"float half fixed mixed"}

for d in $DIRS
do
    make_hls $d &
done

wait
