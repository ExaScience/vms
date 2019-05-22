#!/usr/bin/env python

import matrix_io as mio
import argparse
import glob
import numpy as np
from os.path import join
import arrayfire as af


def read_array(filename):
    m = mio.read_matrix(filename)
    return af.Array(m.ctypes.data, m.shape, m.dtype.char)


def read_sample(s, modeldir, read_func = read_array):
    return [
             read_func(join(modeldir, "sample-%d-%s.ddm" % (s, suf)))
             for suf in ("F0-link", "U0-latents-mean", "U1-latents")
           ]

# globalmean = read_array("global_mean.ddm")

def predict(samples, modeldir, ffile):
    rowfeatures = read_array(ffile)
    ncomp, nfeat  = rowfeatures.shape
    nlatent, nprot = read_sample(1, modeldir, mio.read_matrix)[2].shape

    print("ncomp: %d" % ncomp)
    print("nprot: %d" % nprot)
    print("nlat: %d" % nlatent)
    print("nfeat: %d" % nfeat)

    pred = af.data.constant(0, ncomp, nprot)
    for sample in range(*samples):
        print("sample %d/%d" % (sample, samples[1]), end='')
        B, U0m, U1 = read_sample(sample, modeldir)
        print("before transpose: ", U0m.dims())
        U0m = af.array.transpose(U0m)
        af.eval(U0m)
        print("after transpose: ", U0m.dims())
        U0m = af.data.tile(U0m, ncomp)
        af.eval(U0m)
        print("after tile:", U0m.dims())

        U0 = af.blas.matmul(rowfeatures, B.T);
        af.eval(U0)
        U0c = U0 + U0m
        af.eval(U0c)
        pred0 = af.blas.matmul(U0, U1)
        af.eval(pred0)
        pred += pred0

    print("\ndone")

import argparse

parser = argparse.ArgumentParser(description='SMURFF perdiction using ArrayFire')
parser.add_argument('--device', metavar='N', type=int, help='Run on device N')
parser.add_argument('--samples', metavar='N,M', type=int, nargs=2, default=[1,10], help='Process samples N-M')
parser.add_argument('--modeldir', metavar='MODELDIR', type=str, default='model', help='Model dir')
parser.add_argument('features', metavar='FEATURES', type=str, help='Feature file')

args = parser.parse_args()

if (args.device is not None):
    print("Device info:\n",af.device.info_str(verbose=True))
    # af.device.set_device(args.device)

predict(args.samples, args.modeldir, args.features)
