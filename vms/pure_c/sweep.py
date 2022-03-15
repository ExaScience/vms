#!/usr/bin/env python3

import argparse
import os
from glob import glob
from os.path import join
import shutil
import logging
import itertools
import datetime
from subprocess import CalledProcessError, check_call, STDOUT
import re
import pandas as pd


SCRIPT=os.path.realpath(__file__)
SCRIPTPATH=os.path.dirname(SCRIPT)

logging.basicConfig(level=logging.INFO)

# MPI = " ^openmpi+pmix+thread_multiple fabrics=ofi,ucx schedulers=tm ^openpbs ^ucx+mlx5-dv"
MPI = " ^intel-oneapi-mpi"

GPI = " ^gpi-2 fabrics=infiniband"

SPECS = { 
   # name          multinode? spec
   "plain"         : (False, "impl=plain", ),
   "omp"           : (False, "impl=omp", ),
   "mpi"           : (True,  "impl=mpi" + MPI, ),
   "gpi"           : (True,  "impl=gpi" + GPI + MPI, ),
   "ompss-cluster" : (True,  "impl=ompss ^nanos6@cluster " + MPI, ),
}

ENVS = SPECS.keys()

def is_multinode(name):
    return SPECS[name][0]

def get_spec(name):
    return SPECS[name][1]

def find_re(fname, expr):
    with open(fname, "r") as f:  
        for line in f:
            m = re.match(expr, line)
            if m: 
                return m.group(1)
    return -1

def find_all(fname, expr):
    matches = []
    with open(fname, "r") as f:  
        for line in f:
            m = re.search(expr, line)
            if m:
                matches.append(m)
            
    return matches

def spec(name):
    spec = get_spec(name)

    logging.info(f"{name}: {spec}")

def install(name):
    spec = get_spec(name)

    cmds = f"""\
        . ${{HOME}}/spack/share/spack/setup-env.sh
        spack uninstall --yes vms {spec} || true
        spack install vms {spec} || true
"""
    logging.info(f"Installing {name} spec: {spec}")

    check_call(cmds, shell=True)

def populate(basedir, name, nodes, args):
    if (not is_multinode(name)) and nodes > 0:
        return None 

    dir = os.path.realpath(join(basedir, name, f"nodes_{nodes}"))
    os.makedirs(dir)

    if name == "ompss-cluster":
        shutil.copyfile(join(SCRIPTPATH, "nanos6.toml"), join(dir, "nanos6.toml"))

    for bin in glob(join(SCRIPTPATH, "*.bin")):
        shutil.copy(bin, dir)

    mpirun = f"mpirun -np {nodes} " if nodes > 1 else ""
    time = "/usr/bin/time -v -o vms_resource_$(hostname -s).out"
    spec = get_spec(name)

    cmds = f"""\
#!/bin/bash

#PBS -N vms_{name}_{nodes}
#PBS -A DD-21-28
#PBS -l select={nodes}:ncpus=128:mpiprocs=1:ompthreads=128
#PBS -l walltime=00:15:00
#PBS -j oe
#PBS -o {dir}/combined.out

cd {dir}        

set -e

# needed for argodsm + vms with intel mpi
export ARGO_MPI_WIN_GRANULARITY={16 * 1024}

# needed on karolina and wildcat ()
export I_MPI_HYDRA_BOOTSTRAP=ssh
export I_MPI_HYDRA_BOOTSTRAP_EXEC=/usr/bin/ssh

. ${{HOME}}/spack/share/spack/setup-env.sh
spack load vms {spec}
{mpirun} {time} predict {args} >vms.out 2>vms.err
"""

    script = join(dir, "run.pbs")
    with open(script, "w") as f:
        f.write(cmds)

    os.chmod(script, 0o755)

    return script

def run(basedir, name, nodes, args):
    script = populate(basedir, name, nodes, args)
    if not script:
        return

    dir = os.path.dirname(script)

    logging.debug("Executing '%s'", script)
    try:
        check_call(script)

        #0: 149.2480 giga-ops; 59.7004 giga-ops/second (32-bit floating point ops)
        giga_ops = float(find_re(join(dir, "vms.out"), "^0:.+ ([0-9.]+) giga-ops/second"))
        logging.info(f" env = {name}, nodes = {nodes}, args = {args}, perf = {giga_ops} giga-ops/second") 
    except CalledProcessError as e:
        logging.info(f" env = {name}, nodes = {nodes}, args = {args}, FAILED = {e}") 

def plot(df):
    import matplotlib.pyplot as plt
    import matplotlib.ticker as ticker
    import matplotlib.style as style

    plt.rcParams["font.family"] = "serif"
    plt.style.use('ggplot')

    df = df.pivot(index='nodes', columns='name')
    df.sort_index(inplace=True)
    logging.info(df)    

    ax = df['giga-ops'].plot(legend=True)

    plt.xscale('log', base=2)
    plt.yscale('log', base=10)
        
    ax.set_xlabel("Number of Nodes")
    k_formatter = ticker.FuncFormatter(lambda s, x: str(s) if s < 1024 else str(int(s/1024)) + "K")
    ax.xaxis.set_major_formatter(k_formatter)
    ax.xaxis.set_ticks([pow(2,i) for i in range(0,8)])
        
    ax.set_ylabel("Giga-Ops/sec")

    plt.savefig("giga_ops.svg")
    plt.savefig("giga_ops.pgf")

def report(basedir):
    output_files = glob(join(basedir,'*', '*', 'vms.out'), recursive=True)
    results = []
    for file in output_files:
        name, nodes = file.split("/")[-3:-1]
        nodes = int(re.search("nodes_(\d+)", nodes).group(1))
        #0: 149.2480 giga-ops; 59.7004 giga-ops/second (32-bit floating point ops)
        giga_ops = float(find_re(file, "^0:.+ ([0-9.]+) giga-ops/second"))
        logging.info(f" env = {name}, nodes = {nodes}, perf = {giga_ops} giga-ops/second") 
        results.append([name, nodes, giga_ops])

    df = pd.DataFrame(results, columns=["name", "nodes", "giga-ops"])

    plot(df)

    return results



def main():
    parser = argparse.ArgumentParser(description="Explore multiple datasets, spack envs and #nodes")
    parser.add_argument("--nodes", default="1,2,4,8,16")
    parser.add_argument("--envs", default=",".join(ENVS))
    parser.add_argument("--basedir", default="run_" + datetime.datetime.today().strftime("%Y%m%d-%H%M%S"))
    parser.add_argument("command", default="run", choices=["install", "populate", "run", "spec", "report"])
    parser.add_argument("args", nargs="*", default="3 1000000")

    args = parser.parse_args()

    basedir = os.path.realpath(args.basedir)
    envs = args.envs.split(",")
    nodes = map(int, args.nodes.split(","))
    func_map = { "run" : run, "populate" : populate, "install" : install, "spec" : spec, "report" : report }
    func = func_map[args.command]

    if args.command in [ "run",  "populate" ]:
        logging.info(f"basedir = {basedir}")
        for env, nodes in itertools.product(envs, nodes):
            func(basedir, env, nodes, args.args)
    elif args.command in [ "report" ]:
        logging.info(f"basedir = {basedir}")
        func(basedir)
    else:
        for e in envs:
            func(e)

if __name__ == "__main__":
    main()
