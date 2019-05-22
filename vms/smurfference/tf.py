#!/usr/bin/env python

import time
import matrix_io as mio
import argparse
from glob import glob
import numpy as np
import scipy.sparse
from .utils import calc_rmse
from os.path import join
import os

os.environ["TF_CPP_MIN_LOG_LEVEL"] = "1"
import tensorflow as tf

def read_tf_constant(filename):
    a = mio.read_matrix(filename)
    return tf.constant(a, dtype=tf.float32)

def predict(modeldir, sideinfo):
    if scipy.sparse.issparse(sideinfo):
        sideinfo = sideinfo.toarray()

    compounds = tf.placeholder(tf.float32)
    samples = zip(
        glob(join(modeldir,  "sample-*-F0-link.ddm")),
        glob(join(modeldir,  "sample-*-M0-hypermu.ddm")),
        glob(join(modeldir,  "sample-*-U1-latents.ddm")),
    )

    preds = []
    for F0_file, M0_file, U1_file in samples:
        F0 = read_tf_constant(F0_file)
        U0m = read_tf_constant(M0_file)
        U1  = read_tf_constant(U1_file)
        U0 = tf.matmul(F0, compounds, transpose_b=True) + U0m
        preds.append(tf.matmul(U1, U0, transpose_a=True))

    aggr = tf.add_n(preds) / len(preds)
    aggr = tf.transpose(aggr)

    with tf.Session() as sess:
        result = sess.run(aggr, feed_dict={compounds: sideinfo})

    return result

