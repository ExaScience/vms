#!/usr/bin/env python3

import os
import glob
import shutil
import difflib
import re
import subprocess
import logging
import datetime
import xml.etree.cElementTree as cET
from configparser import ConfigParser

global_build_logger = None
LOGFORMAT = '%(asctime)s - %(process)d - %(levelname)s - %(message)s'

def build_logger():
    global global_build_logger
    return global_build_logger

def make_build_logger(dir):
    global global_build_logger
    global_build_logger = logging.getLogger(os.path.basename(dir))
    fh = logging.FileHandler('build.log')
    fh.setFormatter(logging.Formatter(LOGFORMAT))
    fh.setLevel(logging.INFO)
    build_logger().addHandler(fh)
    return build_logger()

def reset_build_logger():
    global global_build_logger
    global_build_logger = None

def execute(cmd, modules = None, workdir = None, log_name = "make.log"):
    if modules is not None:
        modules_str = " ".join(modules)
        build_logger().info("Loading modules \"%s\"", modules_str)
        cmd = "module load " + modules_str + " && " + cmd

    try:
        owd = os.getcwd()
        if workdir is not None:
            os.chdir(workdir)

        build_logger().info("Command: \"%s\" in dir \"%s\"", cmd, os.getcwd())
        with open(log_name, "w") as log_file:
            result = subprocess.run(cmd, shell=True, stdout=log_file, stderr=subprocess.STDOUT, executable='/bin/bash')

        if result.returncode != 0:
            # with open(log_file, "r") as f:
            #    last_lines = f.readlines()[-10:]

            build_logger().error("Command \"%s\" exited with code %d\n", cmd, result.returncode)
            raise subprocess.CalledProcessError(result.returncode, f"'{cmd}' in {os.getcwd()}")
        else:
            build_logger().info("Command \"%s\" exited with code %d", cmd, result.returncode)

    finally:
        os.chdir(owd)

def patch_file(fname, pattern, repl):
    build_logger().info("patching %s: %s -> %s ", fname, repr(pattern), repr(repl))

    with open(fname, "r") as f:
        content = f.read()
        new_content = re.sub(pattern, repl, content)
       
    build_logger().debug("  diff: %s", difflib.ndiff(content, new_content))
        
    with open(fname, "w") as f:
        f.write(new_content)

def resource_utilization(name):
    report_file, = glob.glob(f'**/{name}_csynth.xml', recursive=True)
    root = cET.parse(report_file).getroot()
    area = root.find('AreaEstimates')

    ar = { r.tag : int(r.text) for r in area.find('AvailableResources') }
    ur = { r.tag : int(r.text) for r in area.find('Resources') }
    merged = { t : ( ar[t], ur[t] ) for t in ar.keys() }
    build_logger().info("Resource utilization of %s:\n%s", name, merged)

    return merged

def latency_estimation(name):
    report_files = glob.glob(f'**/{name}_csynth.xml', recursive=True)
    for report_file in report_files:
        root = cET.parse(report_file).getroot()
        latency_node = root.find('PerformanceEstimates/SummaryOfOverallLatency/Average-caseLatency')
        latency = int(latency_node.text)
        build_logger().info("Average latency of %s: %d (file %s)", name, latency, report_file)

def log_xtasks_config():
    config_files = glob.glob(f'**/*xtasks.config', recursive=True) 
    for config_file in config_files:
        with open (config_file, "r") as f:
            build_logger().info("%s:\n%s", config_file, f.read())


def action_update(builddir):
    if builddir is None:
        builddir = "."

    config = ConfigParser()
    with open(os.path.join(builddir, "config.mk")) as stream:
        config.read_string("[top]\n" + stream.read())  # add [top] section
    sourcedir = config["top"]["srcdir"]
    makefiles_dir = os.path.abspath(os.path.join(sourcedir, "makefiles"))

    def update_symlink(src, dst):
        if not os.path.exists(dst):
            os.symlink(src, dst)

    shutil.copytree(makefiles_dir, builddir, copy_function=update_symlink, dirs_exist_ok=True)

    logging.info("Updated links in %s", builddir)
    return builddir            

def action_populate(sourcedir, basedir, dataset, num_latent, num_samples, datatype):
    if basedir is None:
        basedir = "ci_" + datetime.datetime.today().strftime("%Y%m%d-%H%M%S")

    builddir = os.path.join(basedir, "%s_%dl_%ds_%s" % (dataset, num_latent, num_samples, datatype))

    if os.path.isdir(builddir):
        logging.warning("Not populating %s: already exists", builddir)
        return builddir

    makefiles_dir = os.path.abspath(os.path.join(sourcedir, "makefiles"))
    shutil.copytree(makefiles_dir, builddir, copy_function=os.symlink)

    timestamp = datetime.datetime.now()
    abs_source_dir = os.path.abspath(sourcedir)
    abs_build_dir = os.path.abspath(builddir)
    with open(os.path.join(builddir, "config.mk"), "w") as config_file:
        config_file.write(
            f"""# generated on {timestamp}
SRCDIR = {abs_source_dir}
BUILDDIR = {abs_build_dir}
DATASET = {dataset}
NUM_LATENT = {num_latent}
NUM_SAMPLES = {num_samples}
DATATYPE = {datatype}
""")

    logging.info("Populated %s", builddir)
    return builddir

def action_build(dir):
    os.chdir(dir)
    absdir = os.getcwd()
    make_build_logger(dir)

    try:
        build_logger().info("Building %s", dir)

        execute("make model")
        execute("make", modules=["Vitis/2020.1"], workdir="native")
        execute("make", modules=["GCC/7.3.0" ,"ompss/19.06"], workdir="smp-x86")
        execute(f"dockfpga -w {absdir} make", workdir="smp-arm64")

        execute("make hls", modules = ["Vitis/2020.1"])
        resource_utilization("predict_one_block")
        latency_estimation("predict_one_compound")

        execute(f"dockfpga -w {absdir} make bitstream", workdir="fpga-crdb")
        resource_utilization("predict_one_block")
        latency_estimation("predict_one_compound")
        log_xtasks_config()
    
    except KeyboardInterrupt: 
        build_logger().info("Caught Ctrl-C, aborting")
        raise

    except Exception as e:
        build_logger().error("Failed with %s: %s", type(e), e)
        return False

    finally:
        reset_build_logger()

    print("Done ...")
    return True

def action_ci(*args):
    return action_build(action_populate(*args))

if __name__ == "__main__":

    import argparse

    parser = argparse.ArgumentParser()

    srcdir = os.path.join(os.path.dirname(os.path.realpath(__file__)), '..')

    parser.add_argument("action", type=str, default = "populate", choices = [ "populate", "build", "update" ], help="Action to perform")
    parser.add_argument("--srcdir", type=str, metavar="DIR", default = srcdir, help="Source directory")
    parser.add_argument("--builddir", type=str, metavar="DIR", default = None, help="Actual build directory")
    parser.add_argument("--basedir", type=str, metavar="DIR", default = None, help="Base of builds directory")
    parser.add_argument("--dataset", type=str, default="chem2vec", choices=["chem2vec", "random", ], help="Dataset to use")
    parser.add_argument("--datatype", type=str, default="fixed", choices=["float", "fixed", "half", "mixed"], help="Internal data-type")
    parser.add_argument("--num-latent", type=int, default=4, help="Number of latent dimensions")
    parser.add_argument("--num-samples", type=int, default=4, help="Number of samples to collect")
    parser.add_argument("--do-build", action="store_true", help="Perform actual `make` in build directory")
    parser.add_argument("--verbose", action="store_true", help="Verbose logging on stderr")
    args = parser.parse_args()

    if args.verbose:
        logging.basicConfig(format=LOGFORMAT, level=logging.INFO)

    if args.action == "update":
        action_update(args.builddir)

    if args.builddir is None:
        builddir = action_populate(args.srcdir, args.basedir, args.dataset, args.num_latent, args.num_samples, args.datatype)
    else:
        builddir = args.builddir

    if args.do_build or args.action == "build":
        action_build(builddir)
    



