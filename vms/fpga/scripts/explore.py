#!/usr/bin/env python3

import os
import logging
import datetime
import itertools
from dask import distributed
from build import action_ci, LOGFORMAT

# parameter_space = {
#     "datasets" : [ "chem2vec" ],
#     "num_latents" : [ 8, 16, 32, ],
#     "num_samples" : [ 8, 16, 32, ],
#     "types" : [ "float", "fixed", "mixed", "half", ]
# }

parameter_space = {
    "datasets" : [ "chem2vec" ],
    "num_latents" : [ 16, 32, 48, 64 ],
    "num_samples" : [ 16, 32, 64, 128 ],
    "types" : [ "float", "fixed", ]
}

# parameter_space = {
#     "datasets" : [ "chem2vec" ],
#     "num_latents" : [ 4, 8, ],
#     "num_samples" : [ 4 ],
#     "types" : [ "float", ]
# }

source_dir = "/home/vanderaa/euroexa/eurosmurff/vms"

if __name__ == "__main__":

    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument("-f", "--fail-fast",  action = "store_true")
    parser.add_argument("--basedir", default = None)
    parser.add_argument("--dask", default = None)
    parser.add_argument("--single", nargs=4, metavar="dataset num_latent num_samples data_type", help="Run single point in exploration space")
    args = parser.parse_args()


    if args.basedir:
        basedir = args.basedir
    else:
        basedir = "/scratch/vms/ci_" + datetime.datetime.today().strftime("%Y%m%d-%H%M%S")

    os.makedirs(basedir, exist_ok=True)
    print("basedir: ", basedir)

    logging.basicConfig(format=LOGFORMAT, level=logging.INFO, filename=os.path.join(basedir, 'ci.log')) 

    if args.single:
        dataset, num_latent, num_samples, data_type = args.single
        num_latent = int(num_latent)
        num_samples = int(num_samples)
        action_ci(source_dir, basedir, dataset, num_latent, num_samples, data_type)
    else:
        # $ dask-ssh --nthreads 1 --nprocs 12 --hostfile $PBS_NODEFILE 
        client = distributed.Client(args.dask)

        xprod = list(itertools.product(*parameter_space.values()))
        print("space: ", xprod)

        results = [ client.submit(action_ci, source_dir, basedir, *params) for params in xprod ]
        client.gather(results)
        print("results: ", results)
    
