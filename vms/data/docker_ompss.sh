#!/bin/sh

docker run -v $PWD:/work -w /work --tty --rm bscpm/ompss $@
