#!/bin/sh

set -x

export LD_LIBRARY_PATH=/opt/install-arm64/nanox/lib/debug
export XTASKS_CONFIG_FILE=zcu102/predict.xtasks.config
export NX_ARGS="--fpga-enable --verbose"
#export NX_ARGS="--instrumentation=extrae"
export EXTRAE_CONFIG_FILE=zcu102/extrae.xml

./predict-i
