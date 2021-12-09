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

LOGFORMAT = '%(asctime)s - %(process)d - %(levelname)s - %(message)s'
logging.basicConfig(format = LOGFORMAT, level=logging.INFO)
global_build_logger = logging.getLogger()

def build_logger():
    global global_build_logger
    return global_build_logger

def make_build_logger(dir):
    global global_build_logger
    global_build_logger = logging.getLogger(dir)
    fh = logging.FileHandler('build.log')
    fh.setFormatter(logging.Formatter(LOGFORMAT))
    fh.setLevel(logging.INFO)
    build_logger().addHandler(fh)
    return build_logger()

def reset_build_logger():
    global global_build_logger
    global_build_logger = logging.getLogger()

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

def resource_utilization(name, dir = "."):
    report_file, *_ = glob.glob(f'{dir}/**/{name}_csynth.xml', recursive=True)
    root = cET.parse(report_file).getroot()
    area = root.find('AreaEstimates')

    ar = { r.tag : int(r.text) for r in area.find('AvailableResources') }
    ur = { r.tag : int(r.text) for r in area.find('Resources') }
    merged = { t : ( ar[t], ur[t] ) for t in ar.keys() }


    header = "{:<8} {:>8} {:>8} {:>7}%".format("", "Used", "Avail", "")
    as_string = "\n".join(
        [
            ("{:<8} {:>8} {:>8} {:>8.2%}".format(k, used, avail, used / avail))
            for k, (avail, used)  in merged.items()
        ]
    )

    build_logger().info("Resource utilization of %s:\n%s\n%s", name, header, as_string)

    return merged

def latency_estimation(name, dir = "."):
    report_file, *_ = glob.glob(f'{dir}/**/{name}_csynth.xml', recursive=True)
    root = cET.parse(report_file).getroot()
    latency_node = root.find('PerformanceEstimates/SummaryOfOverallLatency/Average-caseLatency')
    latency = int(latency_node.text)
    build_logger().info("Average latency of %s:\n%d (file %s)", name, latency, report_file)

def log_xtasks_config(dir = "."):
    config_files = glob.glob(f'{dir}/**/*xtasks.config', recursive=True) 
    for config_file in config_files:
        with open (config_file, "r") as f:
            build_logger().info("%s:\n%s", config_file, f.read())


def action_report(builddir):
    if builddir is None:
        builddir = "."

    hlsdir = os.path.join(builddir, "hls")
    resource_utilization("predict_one_block", hlsdir)
    latency_estimation("predict_one_block", hlsdir)

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


def action_populate(sourcedir, basedir, dataset, num_latent, num_samples, block_size, datatype, dataflow, filter_pragmas, disable_bursts):
    logging.info("Populating with: %s", (dataset, num_latent, num_samples, block_size, datatype, dataflow, filter_pragmas, disable_bursts))

    dataflow_str = "df" if dataflow else "nodf"
    filter_str = "hls" if filter_pragmas else "nohls"
    bursts_str = "burst" if disable_bursts else "noburst"
    pragmas_to_filter = [ "pragma HLS" ]
    filter_patterns = ",".join(pragmas_to_filter) if filter_pragmas else ""
    builddir = os.path.join(basedir, "%s_%dl_%ds_%db_%s_%s_%s_%s" % 
        (dataset, num_latent, num_samples, block_size, datatype, dataflow_str, filter_str, bursts_str))

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
BLOCK_SIZE = {block_size}
DATATYPE = {datatype}
DATAFLOW = {int(dataflow)}
FILTER_PATTERNS = {filter_patterns}
DISABLE_BURSTS = {disable_bursts}
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

def action_explore(srcdir, basedir, dataset):
    space = {
        "num_latent"     : [ 4, 8, 16, 32, ],
        "num_samples"    : [ 4, 8, 16, ],
        "block_size"     : [ 4096, ],
        "datatype"       : ["float", "fixed", ],
        "dataflow"       : [ True, False ],
        "filter_pragmas" : [ True, False ],
        "disable_bursts" : [ True, False ],
    }

    from itertools import product
    for c in product(*space.values()):
        kwargs = dict(zip(space.keys(), c))
        action_populate(srcdir, basedir, dataset, **kwargs)


def do_main():
    import argparse

    parser = argparse.ArgumentParser()

    srcdir = os.path.join(os.path.dirname(os.path.realpath(__file__)), '..')
    basedir_default = "ci_" + datetime.datetime.today().strftime("%Y%m%d-%H%M%S")

    parser.add_argument("action", type=str, default = "populate", choices = [ "explore", "populate", "build", "update", "report" ], help="Action to perform")
    parser.add_argument("--srcdir", type=str, metavar="DIR", default = srcdir, help="Source directory")
    parser.add_argument("--builddir", type=str, metavar="DIR", default = None, help="Actual build directory")
    parser.add_argument("--basedir", type=str, metavar="DIR", default = basedir_default, help="Base of builds directory")
    parser.add_argument("--dataset", type=str, default="chem2vec", choices=["chem2vec", "random", ], help="Dataset to use")

    parser.add_argument("--datatype", type=str, default="fixed", choices=["float", "fixed", "half", "mixed"], help="Internal data-type")
    parser.add_argument("--no-dataflow", default=False, action="store_true", help="Disable HLS DATAFLOW")
    parser.add_argument("--no-pragmas", default=False, action="store_true", help="Disable HLS PRAGMAS")
    parser.add_argument("--no-burst", default=False, action="store_true", help="Disable burst reads")

    parser.add_argument("--num-latent", type=int, default=4, help="Number of latent dimensions")
    parser.add_argument("--num-samples", type=int, default=4, help="Number of samples to collect")
    parser.add_argument("--block-size", type=int, default=4096, help="Number of compounds per block")

    parser.add_argument("--do-build", action="store_true", help="Perform actual `make` in build directory")
    parser.add_argument("--verbose", action="store_true", help="Verbose logging on stderr")
    args = parser.parse_args()

    if args.verbose:
        logging.basicConfig(format=LOGFORMAT, level=logging.INFO)

    if args.action == "explore":
        action_explore(args.srcdir, args.basedir, args.dataset)
        return
        
    if args.action == "update":
        action_update(args.builddir)
        return

    if args.action == "report":
        action_report(args.builddir)
        return

    if args.builddir is None:
        builddir = action_populate(args.srcdir, args.basedir,
                    args.dataset, args.num_latent, args.num_samples, args.block_size,
                    args.datatype, not args.no_dataflow, args.no_pragmas, args.no_bursts)
    else:
        builddir = args.builddir

    if args.do_build or args.action == "build":
        action_build(builddir)
    
if __name__ == "__main__":
    do_main()
