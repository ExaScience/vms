#!/usr/bin/env python

import os
import re
import subprocess
import difflib
import logging
import datetime

parameter_space = {
    "datasets" : [ "chem2vec" ],
    "num_latents" : [ 8, 16, 32, ],
    "num_samples" : [ 8, 16, 32, ],
    "types" : [ "float", "fixed", "mixed", "half", ]
}

# git_repo = "git@github.com:euroexa/smurff.git"
git_repo = "/home/vanderaa/euroexa/eurosmurff"
basedir = "/scratch/vms/ci_" + datetime.datetime.today().strftime("%Y%m%d-%H%M%S")

logging.basicConfig(level = logging.INFO)

def execute(cmd, modules = None):
    if modules is not None:
        modules_str = " ".join(modules)
        logging.info("Loading modules \"%s\"", modules_str)
        cmd = "module load " + modules_str + " && " + cmd

    logging.info("$ %s", cmd)
    result = subprocess.run(cmd, shell=True, stdout=subprocess.PIPE,stderr=subprocess.STDOUT, executable='/bin/bash')

    logging.info("%s", result.stdout.decode("utf-8"))
    if result.returncode != 0:
        logging.error("Command \"%s\" exited with code %d", cmd, result.returncode)
        raise subprocess.CalledProcessError(result.returncode, cmd)
    else:
        logging.info("Command \"%s\" exited with code %d", cmd, result.returncode)


def patch_file(fname, pattern, repl):
    logging.info("patching %s: %s -> %s ", fname, pattern, repl)

    with open(fname, "r") as f:
        content = f.read()
        new_content = re.sub(pattern, repl, content)
       
    logging.debug("  diff: %s", difflib.ndiff(content, new_content))
        
    with open(fname, "w") as f:
        f.write(new_content)


class FPGATask:
    def run(self, dataset, num_latent, num_samples, datatype):
        newdir = "%s/%s_%d_%d_%s" % (basedir, dataset, num_latent, num_samples, datatype)

        # checkout 
        os.makedirs(newdir)
        os.chdir(newdir)
        execute("git clone %s repo" % git_repo)
        os.chdir("repo/vms/")

        # patch
        patch_file("data/%s/options.ini" % dataset, "num_latent = \d+", "num_latent = %d" % num_latent)
        patch_file("data/%s/options.ini" % dataset, "nsamples = \d+", "nsamples = %d" % num_samples)
        patch_file("c++/predict.h", "#define DT_FLOAT\s*\n", "#define DT_%s\n" % datatype.upper())

        # make data - model - hls
        os.chdir("data/%s" % dataset)
        execute("python make.py")
        execute("make model")
        execute("cd native && make", modules=["ompss"])
        execute("cd smp-x86 && make")
        execute("cd smp-arm64 && make", modules=["QEMU",  "petalinux/2016.4", "mcxx-arm64"])
        execute("make hls", modules = ["Vivado"])





FPGATask().run("chem2vec", 11, 16, "float")

