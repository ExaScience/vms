#!/usr/bin/env python

import os
from os import chdir
import shutil
import re
import subprocess
import logging
import datetime
import xml.etree.cElementTree as cET

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

def populate_dir(source_dir, build_dir, dataset, num_latent, num_samples, datatype):
    if build_dir is None:
        basedir = "ci_" + datetime.datetime.today().strftime("%Y%m%d-%H%M%S")
        build_dir = os.path.join(basedir, "%s_%d_%d_%s" % (dataset, num_latent, num_samples, datatype))

    if os.path.isdir(build_dir):
        logging.warning("Not populating %s: already exists", build_dir)
        return

    makefiles_dir = os.path.abspath(os.path.join(source_dir, "makefiles"))
    shutil.copytree(makefiles_dir, build_dir, copy_function=os.symlink)

    timestamp = datetime.datetime.now()
    abs_source_dir = os.path.abspath(source_dir)
    abs_build_dir = os.path.abspath(build_dir)
    with open(os.path.join(build_dir, "config.mk"), "w") as config_file:
        config_file.write(
            f"""# generated on {timestamp}
TOPDIR = {abs_source_dir}
BUILDDIR = {abs_build_dir}
DATASET = {dataset}
NUM_LATENT = {num_latent}
NUM_SAMPLES = {num_samples}
DATATYPE = {datatype}
""")

    logging.info("Populated %s", build_dir)

def build_dir(dir):
    os.chdir(dir)
    file_logger = logging.FileHandler('build.log')
    file_logger.setFormatter(logging.Formatter(log_format))
    logging.getLogger().addHandler(file_logger)

    try:
        logging.info("Building %s", dir)

        execute("make model")
        execute("make", modules=["Vitis/2020.1"], workdir="native")
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
    parser.add_argument("--build-dir", type=str, metavar="DIR", default = None, help="Build directory")
    parser.add_argument("--dataset", type=str, default="chem2vec", choices=["chem2vec", "random", ], help="Dataset to use")
    parser.add_argument("--datatype", type=str, default="fixed", choices=["float", "fixed", "half", "mixed"], help="Internal data-type")
    parser.add_argument("--num-latent", type=int, default=4, help="Number of latent dimensions")
    parser.add_argument("--num-samples", type=int, default=4, help="Number of samples to collect")
    parser.add_argument("--build", action="store_true", help="Also build")
    args = parser.parse_args()

    log_format = '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    logging.basicConfig(level = logging.INFO, format = log_format)

    populate_dir('.', args.build_dir, args.dataset, args.num_latent, args.num_samples, args.datatype)

    if args.build:
        build_dir(args.build_dir)
    



