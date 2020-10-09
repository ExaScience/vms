#!/usr/bin/env python

import os
import re
import subprocess
import difflib
import logging
import datetime
import itertools
import xml.etree.cElementTree as cET
from dask import distributed

# parameter_space = {
#     "datasets" : [ "chem2vec" ],
#     "num_latents" : [ 8, 16, 32, ],
#     "num_samples" : [ 8, 16, 32, ],
#     "types" : [ "float", "fixed", "mixed", "half", ]
# }

parameter_space = {
    "datasets" : [ "chem2vec" ],
    "num_latents" : [ 16, 32, 48, 64 ],
    "num_samples" : [ 16, 32, 48, 64 ],
    "types" : [ "fixed", ]
}

# git_repo = "git@github.com:euroexa/smurff.git"
git_repo = "/home/vanderaa/euroexa/eurosmurff"

def execute(cmd, modules = None, workdir = None, log_name = None):
    if log_name is None:
        log_name = cmd.translate(str.maketrans(" .", "__")) + ".log"

    if modules is not None:
        modules_str = " ".join(modules)
        logging.info("Loading modules \"%s\"", modules_str)
        cmd = "module load " + modules_str + " && " + cmd

    try:
        owd = os.getcwd()
        if workdir is not None:
            os.chdir(workdir)

        logging.info("Command: \"%s\" in dir \"%s\"", cmd, os.getcwd())
        with open(log_name, "w") as log_file:
            result = subprocess.run(cmd, shell=True, stdout=log_file, stderr=subprocess.STDOUT, executable='/bin/bash')

        if result.returncode != 0:
            logging.error("Command \"%s\" exited with code %d", cmd, result.returncode)
            raise subprocess.CalledProcessError(result.returncode, cmd)
        else:
            logging.info("Command \"%s\" exited with code %d", cmd, result.returncode)

    finally:
        os.chdir(owd)

def patch_file(fname, pattern, repl):
    logging.info("patching %s: %s -> %s ", fname, repr(pattern), repr(repl))

    with open(fname, "r") as f:
        content = f.read()
        new_content = re.sub(pattern, repl, content)
       
    logging.debug("  diff: %s", difflib.ndiff(content, new_content))
        
    with open(fname, "w") as f:
        f.write(new_content)

def resource_utilization(name):
    report_file = f'hls/solution1/syn/report/{name}_csynth.xml'
    root = cET.parse(report_file).getroot()
    area = root.find('AreaEstimates')

    ar = { r.tag : int(r.text) for r in area.find('AvailableResources') }
    ur = { r.tag : int(r.text) for r in area.find('Resources') }
    merged = { t : ( ar[t], ur[t] ) for t in ar.keys() }
    logging.info("Resource utilization of %s:\n%s", name, merged)

    return merged

def latency_estimation(name):
    report_file = f'hls/solution1/syn/report/{name}_csynth.xml'
    root = cET.parse(report_file).getroot()
    latency_node = root.find('PerformanceEstimates/SummaryOfOverallLatency/Average-caseLatency')
    latency = int(latency_node.text)
    logging.info("Average latency of %s: %d", name, latency)
    return latency

def log_xtasks_config():
    with open ("fpga-zcu102/predict.xtasks.config", "r") as f:
        logging.info("predict.xtasks.config:\n%s", f.read())

def run(basedir, dataset, num_latent, num_samples, datatype):
    # change dir
    newdir = "%s/%s_%d_%d_%s" % (basedir, dataset, num_latent, num_samples, datatype)

    if os.path.isdir(newdir):
        logging.warning("Skipping dataset=%s num_latent=%d num_samples=%d datatype=%s -- %s already exists" %
            (dataset, num_latent, num_samples, datatype, newdir))
        return False

    os.makedirs(newdir)
    os.chdir(newdir)
    file_logger = logging.FileHandler('ci.log')
    file_logger.setFormatter(logging.Formatter(log_format))
    logging.getLogger().addHandler(file_logger)

    try:
        logging.info("Starting dataset=%s num_latent=%d num_samples=%d datatype=%s in %s",
            dataset, num_latent, num_samples, datatype, os.getcwd())

        # checkout 
        execute("git clone %s repo" % git_repo, log_name = "clone.log")
        os.chdir("repo/vms/")

        # patch
        patch_file("data/%s/options.ini" % dataset, "num_latent = \d+", "num_latent = %d" % num_latent)
        patch_file("data/%s/options.ini" % dataset, "nsamples = \d+", "nsamples = %d" % num_samples)
        patch_file("c++/predict.h", "#define DT_FLOAT\s*\n", "#define DT_%s\n" % datatype.upper())

        # make data - model - hls
        os.chdir("data/%s" % dataset)
        execute("python make.py")
        execute("make model")
        execute("make", modules=["ompss"], workdir="native")
        execute("make", modules=["ompss"], workdir="smp-x86")
        execute("make", modules=["QEMU",  "petalinux/2016.4", "mcxx-arm64"], workdir="smp-arm64")

        execute("make hls", modules = ["Vivado/2017.4"])
        resource_utilization("predict_or_update_model")
        latency_estimation("dataflow_in_loop")

        execute("make bitstream", modules = ["autoVivado"], workdir="fpga-zcu102")
        log_xtasks_config()
    
    except KeyboardInterrupt: 
        logging.info("Caught Ctrl-C, aborting")
        raise

    except Exception as e:
        logging.error("Failed with %s: %s", type(e), e)
        return False

    finally:
        logging.getLogger().removeHandler(file_logger)

    print("Done ...")
    return True

if __name__ == "__main__":

    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument("-f", "--fail-fast",  action = "store_true")
    parser.add_argument("--basedir", default = None)
    parser.add_argument("--dask", default = None)
    parser.add_argument("--single", nargs=4, meta="dataset num_latent num_samples data_type" help="Run single point in exploration space")
    args = parser.parse_args()


    log_format = '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    logging.basicConfig(level = logging.INFO, format = log_format, filename = os.path.join(basedir, 'ci.log'))
    
    if args.basedir:
        basedir = args.basedir
    else:
        basedir = "/scratch/vms/ci_" + datetime.datetime.today().strftime("%Y%m%d-%H%M%S")

    os.makedirs(basedir, exist_ok=True)
    print("basedir: ", basedir)

    if args.single:
        dataset, num_latent, num_samples, data_type = args.single
        num_latent = int(num_latent)
        num_samples = int(num_samples)
        run(basedir, dataset, num_latent, num_samples, data_type)
    else:
        # $ dask-ssh --nthreads 1 --nprocs 12 --hostfile $PBS_NODEFILE 
        client = distributed.Client(args.dask)

        xprod = list(itertools.product(*parameter_space.values()))
        print("space: ", xprod)

        results = [ client.submit(run, basedir, *params) for params in xprod ]
        client.gather(results)
        print("results: ", results)
    
