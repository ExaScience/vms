#!/usr/bin/env python

import os
import re
import subprocess
import difflib
import logging
import datetime
import itertools
import xml.etree.cElementTree as cET

parameter_space = {
    "datasets" : [ "chem2vec" ],
    "num_latents" : [ 8, 16, 32, ],
    "num_samples" : [ 8, 16, 32, ],
    "types" : [ "float", "fixed", "mixed", "half", ]
}

# git_repo = "git@github.com:euroexa/smurff.git"
git_repo = "/home/vanderaa/euroexa/eurosmurff"
basedir = "/scratch/vms/ci_" + datetime.datetime.today().strftime("%Y%m%d-%H%M%S")

log_format = '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
logging.basicConfig(level = logging.INFO, format = log_format)

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

def run(dataset, num_latent, num_samples, datatype):

    # change dir and log to file
    newdir = "%s/%s_%d_%d_%s" % (basedir, dataset, num_latent, num_samples, datatype)
    os.makedirs(newdir)
    os.chdir(newdir)
    file_logger = logging.FileHandler('ci.log')
    file_logger.setFormatter(logging.Formatter(log_format))
    file_logger = logging.getLogger().addHandler(file_logger)

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

    return True

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument("-f", "--fail-fast",  action = "store_true")
    args = parser.parse_args()
    
    for params in itertools.product(*parameter_space.values()):
        success = run(*params)
        if args.fail_fast and not success:
            break
    
