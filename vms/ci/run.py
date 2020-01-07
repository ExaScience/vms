#!/usr/bin/env python

import os
import re
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

def patch_file(fname, pattern, repl):
    with open(fname, "r") as f:
        content = f.read()
        new_content = re.sub(pattern, repl, content)
        
    with open(fname, "w") as f:
        f.write(new_content)


class FPGATask:
    def run(self, dataset, num_latent, num_samples, datatype):
        newdir = "%s/%s_%d_%d_%s" % (basedir, dataset, num_latent, num_samples, datatype)

        # checkout 
        os.makedirs(newdir)
        os.chdir(newdir)
        os.system("git clone %s repo" % git_repo)
        os.chdir("repo/vms/")

        # patch
        patch_file("data/%s/options.ini" % dataset, "num_latent = \d+", "num_latent = %d" % num_latent)
        patch_file("data/%s/options.ini" % dataset, "nsamples = \d+", "nsamples = %d" % num_samples)
        patch_file("c++/predict.h", "#define DT_FLOAT\s*\n", "#define DT_%s\n" % datatype.upper())

        # make data - model - hls
        os.chdir("data/%s" % dataset)
        os.system("python make.py")
        os.system("make model")
        os.system("make hls")


FPGATask().run("chem2vec", 128, 25, "half")

