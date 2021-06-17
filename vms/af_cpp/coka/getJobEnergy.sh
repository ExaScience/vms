#!/bin/bash

sacct -j $1 --format=User,partition,state,elapsed,nnodes,ncpus,nodelist,ConsumedEnergy,ConsumedEnergyRaw
