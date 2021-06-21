#!/bin/bash

set -euo pipefail

LOAD_BITSTREAM=/home/afilguer/scripts/load_bitstream
ZU_BITSTREAM=/home/afilguer/zu_ompss_base_bit/zu_ompss_base.bit
VU_BITSTREAM=predict.bit
LOAD_MODULES="sudo /opt/bsc/arm64/ompss/2.5.1/scripts/load_ompss_modules"
RESET_SCRIPT=$HOME/vms/reset.rs

#minicom -D /dev/ttyUSB1 --script=$RESET_SCRIPT

$LOAD_BITSTREAM zu $ZU_BITSTREAM
$LOAD_BITSTREAM vu $VU_BITSTREAM
ssh node1 $LOAD_MODULES


