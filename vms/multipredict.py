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

import smurfference as sf

def multipredict(sideinfo_file, test_file, modeldir = None, train_file = None):
    rmse_values = {}

    # if we have a train_file, we do training
    if train_file:
        rmse_values["bpmf"], _ = sf.train_session(None, train_file, test_file)
        rmse_values["macau"], modeldir = sf.train_session(modeldir, train_file, test_file, sideinfo_file)
    elif not modeldir:
        raise ValueError("Need either train or modeldir")

    py_methods = {
        "macau in matrix    " : sf.smurff_predict_im,
        "macau out-of-matrix" : sf.smurff_predict_py,
    }

    test = mio.read_matrix(test_file).tocsr()

    sideinfo = mio.read_matrix(sideinfo_file)
    if sparse.issparse(sideinfo):
        sideinfo = sideinfo.tocsr()

    for name, func in py_methods.items():
        predictions = func(modeldir, sideinfo, test)
        rmse = smurff.calc_rmse(predictions)
        rmse_values[name] = rmse

    other_methods = {
        "smurff_cmd ": sf.smurff_predict_cmd,
        "tf         ": sf.tf_predict,
        "np         ": sf.np_predict,
    }

    for name, func in other_methods.items():
        predictions = func(modeldir, sideinfo)
        rmse = sf.calc_rmse(predictions, test)
        rmse_values[name] = rmse

    return rmse_values

import unittest

class TestMultiPredict(unittest.TestCase):
    def test_small_model(self):
        results = multipredict("test_data/small/sideinfo.mm", "test_data/small/test.sdm", train_file="test_data/small/train.mm")
        print(results)
        self.assertEqual('foo'.upper(), 'FOO')

if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description='SMURFF perdiction test')
    parser.add_argument('--train', type=str, help='Populate modeldir')
    parser.add_argument('--modeldir', type=str, help='Use model dir')
    parser.add_argument('sideinfo', type=str, help='Sideinfo file')
    parser.add_argument('test', type=str, help='Test matrix file')
    args = parser.parse_args()

    print(multipredict(args.sideinfo, args.test, args.modeldir, args.train))
