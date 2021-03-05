#!/bin/sh

set -x -e

CORES="0"
for C in $(seq 47)
do
	srun ./af_predict --backend eigen --block 1000 --limit 100000 --repeat 2 --devices $CORES
	CORES="$CORES $C"
done
