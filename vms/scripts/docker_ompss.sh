#!/bin/sh

SCRIPT=$(realpath "${BASH_SOURCE[0]}")
WORK=$(dirname $SCRIPT)/..
SUB=$(realpath --relative-to="$WORK" $PWD)

docker run -v $WORK:/work -w /work/$SUB --tty --rm bscpm/ompss $@
