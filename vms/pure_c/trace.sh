#!/usr/bin/env bash

# Copyright (C) 2021  Jimmy Aguilar Mena

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


export EXTRAE_ON=1
export EXTRAE_CONFIG_FILE=extrae.xml

export NANOS6_CONFIG_OVERRIDE="version.debug=false,version.instrument="extrae",cluster.disable_remote=false"

echo "Deleting old traces"
rm -rf TRACE.* set-0/ *.prv *.row *.pcf

if [[ "$1" == *"mpi/"* ]]; then
	echo "Preloading libompitrace.so" >&1
	export LD_PRELOAD=${EXTRAE_HOME}/lib/libompitrace.so
	export OMP_NUM_THREADS=4
elif [[ "$1" == *"ompss/"* ]]; then
	echo "Preloading libnanosmpitrace.so" >&1
	export LD_PRELOAD=${EXTRAE_HOME}/lib/libnanosmpitrace.so
else
	echo "error: Could not deterimine what library to preload" >&1
	exit -1
fi

$@
unset LD_PRELOAD