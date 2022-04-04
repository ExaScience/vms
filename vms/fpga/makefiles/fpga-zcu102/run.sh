#!/bin/bash

set -x
set -e

cmp BOOT.BIN /boot/BOOT.BIN || {
    sudo cp BOOT.BIN /boot/
    echo "Press any key to reboot"
    read a
    sudo reboot
}

export LD_LIBRARY_PATH=/opt/install-arm64/nanox/lib/debug
export XTASKS_CONFIG_FILE=predict.xtasks.config
#export NX_ARGS="--fpga-enable --verbose"
#export NX_ARGS="--instrumentation=extrae"
export EXTRAE_CONFIG_FILE=extrae.xml

time ./predict-i $* | tee stdout.log
