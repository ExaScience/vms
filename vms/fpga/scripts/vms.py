#!/usr/bin/env python3

import os
import logging
import datetime
import itertools
from dask import distributed
import build

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
    parser.add_argument("--dataset", type=str, default="chem2vec", choices=["chem2vec", "random", ], help="Dataset to use")
    parser.add_argument("--datatype", type=str, default="fixed", choices=["float", "fixed", "half", "mixed"], help="Internal data-type")
    parser.add_argument("--num-latent", type=int, default=4, help="Number of latent dimensions")
    parser.add_argument("--num-samples", type=int, default=4, help="Number of samples to collect")
    parser.add_argument("--verbose", action="store_true", help="Verbose logging on stderr")
    parser.add_argument("--dask", default = None, help="DASK URL")
    parser.add_argument("command", choices = ["build", "status", "generate", "populate"], help="Command to execute")
    parser.add_argument("dirs", nargs='*', help="")
    args = parser.parse_args()

    if args.verbose:
        logging.basicConfig(format=build.LOGFORMAT, level=logging.INFO)

    if args.builddir is None:
        builddir = action_populate('.', args.basedir, args.dataset, args.num_latent, args.num_samples, args.datatype)
    else:
        builddir = args.builddir

    if args.do_build:
        action_build(builddir)
    args = parser.parse_args()

    if args.basedir:
        basedir = args.basedir
    else:
        basedir = "/scratch/vms/ci_" + datetime.datetime.today().strftime("%Y%m%d-%H%M%S")

    os.makedirs(basedir, exist_ok=True)
    print("basedir: ", basedir)

    logging.basicConfig(format=LOGFORMAT, level=logging.INFO, filename=os.path.join(basedir, 'ci.log')) 

        # $ dask-ssh --nthreads 1 --nprocs 12 --hostfile $PBS_NODEFILE 
        client = distributed.Client(args.dask)

        xprod = list(itertools.product(*parameter_space.values()))
        print("space: ", xprod)

        results = [ client.submit(action_ci, source_dir, basedir, *params) for params in xprod ]
        client.gather(results)
        print("results: ", results)
    
