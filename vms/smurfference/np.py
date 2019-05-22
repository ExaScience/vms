#!/usr/bin/env python

from glob import glob
from os.path import join
import numpy as np
import scipy.sparse

from matrix_io import read_matrix
from .utils import calc_rmse, add_globalmean

def predict(modeldir, sideinfo):
    if scipy.sparse.issparse(sideinfo):
        sideinfo = sideinfo.toarray()

    samples = zip(
                glob(join(modeldir,  "sample-*-F0-link.ddm")),
                glob(join(modeldir,  "sample-*-M0-hypermu.ddm")),
                glob(join(modeldir,  "sample-*-U1-latents.ddm")),
    )

    predictions = []
    for F0_file, M0_file, U1_file in samples:
        F0 = read_matrix(F0_file)
        M0 = read_matrix(M0_file)
        U1 = read_matrix(U1_file)

        U0 = np.matmul(sideinfo, F0.transpose()) + M0.transpose()
        P = np.matmul(U0, U1)
        predictions.append(np.asarray(P))

    predictions = np.stack(predictions)
    predictions = np.mean(predictions, axis=0)
    predictions = add_globalmean(predictions, modeldir)

    return predictions
