#!/bin/sh

set -x

export LD_LIBRARY_PATH=/opt/install-arm64/nanox/lib/instrumentation
export XTASKS_CONFIG_FILE=predict.xtasks.config
#export NX_ARGS="--fpga-enable --verbose --instrumentation=extrae"
export NX_ARGS="--instrumentation=extrae"
export EXTRAE_CONFIG_FILE=extrae.xml

strace -o strace.log ./predict-i
