#!/usr/bin/env python

import matrix_io as mio
import smurff
import numpy as np
import sys
import os.path as pth
import io
import argparse


def gen_int(name, value):
    """Generates code for an int:
    
    const int sample_1_U0_latents_rows = 4;
    """

    return "const int %s = %d;\n" % (name, value)
 

def gen_mat(M, typename, varname):
    """Generate C code for matrix M,
    Something like:

const int sample_1_U0_latents_rows = 4;
const int sample_1_U0_latents_cols = 3;
const double sample_1_U0_latents[4][3] = {
    { 0.22, 1.40, 1.15 },
    { -0.26, 0.74, 2.48 },
    { -0.62, 0.29, 0.17 },
    { 1.61, 3.64, 5.08 },
};


    """
    hdr = gen_int(varname + "_rows", M.shape[0])
    hdr += gen_int(varname + "_cols", M.shape[1])
    hdr += typename + " " + varname + "[%d][%d] = {" % M.shape
    ftr = "};"
    fmt = "    { " + "%+.2f, " * (M.shape[1] - 1) + "%+.2f },"

    f = io.StringIO() 

    np.savetxt(f, M,
            fmt = fmt,
            header = hdr,
            footer = ftr,
            newline = "\n",
            comments = "")

    return f.getvalue()

def gen_sample(sample):
    ret = ""
    for i, U in enumerate(sample.latents):
        ret += gen_mat(U, "const double", "s%d_U%d" % (sample.iter, i))

    return ret



parser = argparse.ArgumentParser(description='Generate SMURFF HLS inferencer C-code')

parser.add_argument('root', metavar='root-file', type=str, help='root file')

parser.add_argument('--samples', dest='samples', default='all',
                    help='sample or samples to include (e.g. 1, 2-10, all)')

args = parser.parse_args()

session = smurff.PredictSession.fromRootFile(args.root)

for sample in session.samples:
    print(gen_sample(sample))



