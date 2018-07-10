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

def gen_sample(sample, selection):
    if (sample.iter < selection[0] or sample.iter > selection[1]):
        return "// skip sample %d\n" % sample.iter

    ret = ""
    for i, U in enumerate(sample.latents):
        ret += gen_mat(U, "double", "s%d_U%d" % (sample.iter, i))
    for i, F in enumerate(sample.betas):
        ret += gen_mat(F, "double", "s%d_F%d" % (sample.iter, i))

    return ret

parser = argparse.ArgumentParser(description='Generate SMURFF HLS inferencer C-code')
parser.add_argument('root', metavar='root-file', type=str, help='root file')
parser.add_argument('--selection', dest='selection', default='all', help='sample or samples to include (e.g. 1, 2-10, all)')
parser.add_argument('--output', dest='output', default='smurff_model.h', help='output file')
args = parser.parse_args()

def parse_selection(str):
    s = str.split("-")
    if (str == "all"):
        selection = (0, 9999)
    elif (len(s) == 1):
        selection = (int(s[0]), int(s[0]))
    elif (len(s) == 2):
        if s[0] == "": s[0] = 0
        if s[1] == "": s[1] = 9999
        selection = ( int(s[0]), int(s[1]))
    else:
        raise ValueError("Could not parse sample selection: " + args.samples)

    return selection

selection = parse_selection(args.selection)
session = smurff.PredictSession.fromRootFile(args.root)


output = ""
output += gen_int("num_latent", session.num_latent())
output += gen_int("num_comp", session.data_shape()[0])
output += gen_int("num_proteins", session.data_shape()[1])
output += gen_int("num_feat", session.beta_shape()[0])

for sample in session.samples:
    output += gen_sample(sample, selection)

with open(args.output, "w") as os:
    os.write(output)





