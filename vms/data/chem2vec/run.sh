#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $SCRIPT_DIR

ml autoVivado/1.2.3

DATE=$(date +%Y%m%d_%H%M)
exec  >stdout_$DATE.log 2>stderr_$DATE.log

make clean
make model
cd hls
bash make.sh

