#!/usr/bin/env python3

import argparse
import os
from os.path import join
import shutil
import logging
import itertools
import datetime
from subprocess import check_call, STDOUT
import re

SCRIPT=os.path.realpath(__file__)
SCRIPTPATH=os.path.dirname(SCRIPT)

logging.basicConfig(level=logging.DEBUG)

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
   "ompss-argo"    : (True,  "impl=ompss ^nanos6@argodsm " + MPI, ),
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
        shutil.copyfile(join(SCRIPTPATH, "nanos6.cluster.toml"), join(dir, "nanos6.toml"))
    if name == "ompss-argo":
        shutil.copyfile(join(SCRIPTPATH, "nanos6.argo.toml"), join(dir, "nanos6.toml"))

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
    check_call(script)

    #0: 149.2480 giga-ops; 59.7004 giga-ops/second (32-bit floating point ops)
    giga_ops = float(find_re(join(dir, "vms.out"), "^0:.+ ([0-9.]+) giga-ops/second"))
    logging.info(f" env = {name}, nodes = {nodes}, args = {args}, perf = {giga_ops} giga-ops/second") 

def main():
    parser = argparse.ArgumentParser(description="Explore multiple datasets, spack envs and #nodes")
    parser.add_argument("--nodes", default="1,2,4,8,16")
    parser.add_argument("--envs", default=",".join(ENVS))
    parser.add_argument("--basedir", default="run_" + datetime.datetime.today().strftime("%Y%m%d-%H%M%S"))
    parser.add_argument("command", default="run", choices=["install", "populate", "run", "spec"])
    parser.add_argument("args", nargs="*", default="2 20000000")

    args = parser.parse_args()

    basedir = os.path.realpath(args.basedir)
    envs = args.envs.split(",")
    nodes = map(int, args.nodes.split(","))
    func_map = { "run" : run, "populate" : populate, "install" : install, "spec" : spec }
    func = func_map[args.command]

    if args.command in [ "run",  "populate" ]:
        logging.info(f"basedir = {args.basedir}")
        for env, nodes in itertools.product(envs, nodes):
            func(basedir, env, nodes, args.args)
    else:
        for e in envs:
            func(e)

if __name__ == "__main__":
    main()