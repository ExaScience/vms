#!/bin/bash

set -e

DATADIR=../..
TOPDIR=$DATADIR/../..

function make_hls {
    DIR=$1
    FLAGS=${DIR^^}

    rm -fr $DIR && mkdir $DIR && cd $DIR
    ln -s $DATADIR/code .
    vivado_hls -f $TOPDIR/codegen/run.tcl $FLAGS

    cd ..
}

DIRS=$*
DIRS=${DIRS:-"float half fixed mixed"}

for d in $DIRS
do
    make_hls $d
done
