#!/usr/bin/env python

from scipy import sparse
import numpy as np
import smurff
import matrix_io as mio
import unittest
import sys
import tempfile
from os.path import join
import subprocess

from .utils import calc_rmse, ext, add_globalmean

global_verbose = False

def predict_im(modeldir, _, test):
    predict_session = smurff.PredictSession(join(modeldir, "root.ini"))
    return predict_session.predict_some(test)

def predict_py(modeldir, sideinfo, test):
    predict_session = smurff.PredictSession(join(modeldir, "root.ini"))
    return [
            predict_session.predict_one((sideinfo[i, :], j), v)
            for i,j,v in zip(*sparse.find(test))
            ]

def predict_cmd(modeldir, sideinfo):
    root = join(modeldir, "root.ini")

    with tempfile.TemporaryDirectory() as tmpdir:
        sideinfo_file = join(tmpdir, "sideinfo" + ext(sideinfo))
        mio.write_matrix(sideinfo_file, sideinfo)
        cmd = "smurff --root %s --save-prefix %s --row-features %s --save-freq -1" % (root, tmpdir, sideinfo_file)
        subprocess.call(cmd, shell=True)

        predictions = mio.read_matrix(join(tmpdir, "predictions-average.ddm"))

    predictions = add_globalmean(predictions, modeldir)

    return predictions
