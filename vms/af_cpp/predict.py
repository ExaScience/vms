#!/usr/bin/env python

from scipy import sparse
import numpy as np
import smurff
import matrix_io as mio
import unittest
import sys
import os
from tempfile import mkdtemp
from os.path import join
import subprocess

def af_cmd_oom_prediction(root_dir, sideinfo, test):
    tmpdir = mkdtemp()
    outfile = join(tmpdir, "predictions-average.ddm")
    sideinfo_file = join(tmpdir, "sideinfo.mm")
    mio.write_matrix(sideinfo_file, sideinfo.todense())

    exec_file = os.path.abspath("./af_predict")
    cmd = "%s --modeldir %s/ --out %s --features %s" % (exec_file, root_dir, outfile, sideinfo_file)
    print(cmd)
    subprocess.call(cmd, shell=True)

    return calc_rmse(outfile, test)
