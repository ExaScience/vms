#!/bin/sh

set -x
set -e

cmp ./zcu102/predict_autoVivado/BOOT.BIN /boot/BOOT.BIN

export LD_LIBRARY_PATH=/opt/install-arm64/nanox/lib/performance
export XTASKS_CONFIG_FILE=zcu102/predict.xtasks.config
#export NX_ARGS="--fpga-enable --verbose"
#export NX_ARGS="--instrumentation=extrae"
export EXTRAE_CONFIG_FILE=zcu102/extrae.xml

./predict-i
