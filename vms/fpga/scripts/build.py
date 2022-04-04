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
import csv
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

def resource_utilization(dir = ".", kernel = "predict_one_block"):
    report_files = glob.glob(f'{dir}/**/{kernel}_csynth.xml', recursive=True)
    reports = {}
    for report_file in report_files:
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

        build_logger().info("Resource utilization of %s:\n%s:\n%s\n%s", report_file, kernel, header, as_string)
        reports[report_file] = merged

    return reports

def latency_estimation(dir = ".", kernel = "predict_one_block"):
    report_files = glob.glob(f'{dir}/**/{kernel}_csynth.xml', recursive=True)
    reports = {}
    for report_file in report_files:
        root = cET.parse(report_file).getroot()
        latency_node = root.find('PerformanceEstimates/SummaryOfOverallLatency/Average-caseLatency')
        cycles = int(latency_node.text) 
        latency_milliseconds_node = root.find('PerformanceEstimates/SummaryOfOverallLatency/Average-caseRealTimeLatency')
        milliseconds = latency_milliseconds_node.text
        build_logger().info("Average latency of %s:\n%s: %d cycles - %s", report_file, kernel, cycles, milliseconds)
        reports[report_file] = [ cycles, milliseconds ]

    return reports

def log_xtasks_config(dir = "."):
    config_files = glob.glob(f'{dir}/**/*xtasks.config', recursive=True) 
    for config_file in config_files:
        with open (config_file, "r") as f:
            build_logger().info("%s:\n%s", config_file, f.read())

def read_summary_csv(filename):
    current_section = None
    current_data = []
    opencl_summary_data = dict()
    with open(filename) as f:
        csvreader = csv.reader(f)
        for row in csvreader:
            if current_section is None and len(row) == 1:
                current_section = row[0]
            elif len(row) == 0 and current_section is not None:
                opencl_summary_data[current_section] = current_data
                current_section = None
                current_data = []
            else:
                current_data.append(row)

    return opencl_summary_data

def to_table(list_of_lists):
    row_format = "{:>20}" * len(list_of_lists[0])
    return "\n".join([ row_format.format(*row) for row in list_of_lists ])

def read_config(builddir):
    config = ConfigParser()
    with open(os.path.join(builddir, "config.mk")) as stream:
        config.read_string("[top]\n" + stream.read())  # add [top] section
    return config["top"]

def action_report(builddir):
    if builddir is None:
        builddir = "."

    config = read_config(builddir)

    hlsdir = os.path.join(builddir, "opencl", "hw_emu", "**", "hls.app")
    hlsdir = glob.glob(hlsdir, recursive=True).pop()
    hlsdir = os.path.dirname(hlsdir)
    resource_reports = resource_utilization(hlsdir)
    latency_reports = latency_estimation(hlsdir)

    cycles, milliseconds = list(latency_reports.values()).pop()

    num_latent = int(config["NUM_LATENT"])
    num_samples = int(config["NUM_SAMPLES"])
    block_size = int(config["BLOCK_SIZE"])
    num_proteins = 114 # FIXME
    num_features = 469 # FIXME
    hz = 100e6

    gops = num_samples * block_size * num_latent * (num_features + num_proteins) / 1e9;
    secs = cycles / hz
    build_logger().info("%.2f giga-ops; %.2f giga-ops/second\n", gops, gops/secs);

    opencl_summary_file = os.path.join(builddir, "opencl/hw_emu/opencl_summary.csv")
    opencl_summary = read_summary_csv(opencl_summary_file)
    kernel_exec = opencl_summary["Kernel Execution (includes estimated device time)"]
    build_logger().info("Kernel executions:\n" + to_table(kernel_exec))

def action_update(builddir):
    if builddir is None:
        builddir = "."

    config = read_config(builddir)
    sourcedir = config["srcdir"]
    makefiles_dir = os.path.abspath(os.path.join(sourcedir, "makefiles"))

    def update_symlink(src, dst):
        if not os.path.exists(dst):
            os.symlink(src, dst)

    shutil.copytree(makefiles_dir, builddir, copy_function=update_symlink, dirs_exist_ok=True)

    logging.info("Updated links in %s", builddir)
    return builddir            


def action_populate(sourcedir, basedir, dataset, num_latent, num_samples, block_size, datatype, dataflow, filter_pragmas, disable_bursts, enable_mpi):
    logging.info("Populating with: %s", (dataset, num_latent, num_samples, block_size, datatype, dataflow, filter_pragmas, disable_bursts, enable_mpi))

    dataflow_str = "df" if dataflow else "nodf"
    filter_str = "nohls" if filter_pragmas else "hls"
    bursts_str = "nobursts" if disable_bursts else "bursts"
    mpi_str = "_mpi" if enable_mpi else ""
    pragmas_to_filter = [ "pragma HLS" ]
    filter_patterns = ",".join(pragmas_to_filter) if filter_pragmas else ""
    builddir = os.path.join(basedir, "%s_%dl_%ds_%db_%s_%s_%s_%s%s" % 
        (dataset, num_latent, num_samples, block_size, datatype, dataflow_str, filter_str, bursts_str, mpi_str))

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
DISABLE_BURSTS = {int(disable_bursts)}
USE_MPI = {int(enable_mpi)}
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
        execute("make", modules=["Vitis/2020.2"], workdir="native")
        execute("make", modules=["GCC/7.3.0" ,"ompss/19.06"], workdir="smp-x86")
        execute(f"dockfpga -w {absdir} make", workdir="smp-arm64")

        execute("make hls", modules = ["Vitis/2020.2"])
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

def full_space():
    space = {
        "num_latent"     : [ 4, 8, 16, 32, ],
        "num_samples"    : [ 4, 8, 16, ],
        "block_size"     : [ 32, 4096, ],
        "datatype"       : ["float", "fixed", ],
        "dataflow"       : [ True, False ],
        "filter_pragmas" : [ True, False ],
        "disable_bursts" : [ True, False ],
    }

    from itertools import product
    return space.keys(), product(*space.values())

def limited_space():
    space = {
        "num_latent"     : [ 4,   4,  4,  4,  4, 32,   32, ],
        "num_samples"    : [ 4,   4,  4,  4,  4, 16,   16, ],
        "block_size"     : [ 32, 32, 32, 32, 32, 32, 4096, ],
        "datatype"       : ["float", "fixed", "fixed", "fixed", "fixed", "fixed", "fixed", ],
        "filter_pragmas" : [ True, True, False, False, False, False, False, ],
        "dataflow"       : [ False, False, False, True, True, True, True, ],
        "disable_bursts" : [ True, True, True, True, False, False, False, ],
    }

    return space.keys(), zip(*space.values())

def action_explore(srcdir, basedir, dataset, full = False):
    keys, values = full_space() if full else limited_space()
    for v in values:
        assert len(v) == len(keys), f"len({v}) != len({keys})"
        kwargs = dict(zip(keys, v))
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
    parser.add_argument("--no-bursts", default=False, action="store_true", help="Disable bursts reads")

    parser.add_argument("--enable-mpi", default=False, action="store_true", help="Enable the use of MPI")

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
                    args.datatype, not args.no_dataflow, args.no_pragmas, args.no_bursts,
                    args.enable_mpi)
    else:
        builddir = args.builddir

    if args.do_build or args.action == "build":
        action_build(builddir)
    
if __name__ == "__main__":
    do_main()
