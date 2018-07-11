#!/usr/bin/env python

import matrix_io as mio
import smurff
import numpy as np
import scipy as sp
import sys
import os.path as pth
import io
import argparse

def parse_selection(str):
    s = str.split("-")
    if (str == "all"):
        selection = (0, 9999)
    elif (len(s) == 1):
        selection = (int(s[0]), int(s[0])+1)
    elif (len(s) == 2):
        if s[0] == "": s[0] = 0
        if s[1] == "": s[1] = 9999
        selection = ( int(s[0]), int(s[1]+1))
    else:
        raise ValueError("Could not parse sample selection: " + args.samples)

    return range(*selection)

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
    fmt = "    { " + "%+.8f, " * (M.shape[1] - 1) + "%+.8f },"

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
        ret += gen_mat(U, "double", "s%d_U%d" % (sample.iter, i))
    for i, F in enumerate(sample.betas):
        ret += gen_mat(F, "double", "s%d_F%d" % (sample.iter, i))

    return ret

parser = argparse.ArgumentParser(description='Generate SMURFF HLS inferencer C-code')
parser.add_argument('--out-prefix', dest='prefix', default='smurff_', help='output file prefix')
parser.add_argument('--samples', dest='sel_samples', default='all', help='sample or samples to include (e.g. 1, 2-10, all)')
parser.add_argument('--root', metavar='root-file', type=str, help='root file', required=True)

args = parser.parse_args()
selected_samples = parse_selection(args.sel_samples)

session = smurff.PredictSession(args.root)

num_latent = session.num_latent()
num_compounds, num_proteins = session.data_shape()
num_features = session.beta_shape()[0]

const_output = "#pragma once\n"
const_output += gen_int("num_latent", num_latent)
const_output += gen_int("num_compounds", num_compounds)
const_output += gen_int("num_proteins", num_proteins)
const_output += gen_int("num_features", num_features)

#generate testbench
tb_file = pth.join(session.root_dir, session.options.get("side_info_0_0", "file"))
tb_in_matrix = mio.read_matrix(tb_file)
if sp.sparse.issparse(tb_in_matrix):
    tb_in_matrix = tb_in_matrix.todense()

(tb_num_compounds, tb_num_features) = tb_in_matrix.shape
if tb_num_compounds > 10: tb_num_compounds = 10
tb_in_matrix = tb_in_matrix[:tb_num_compounds,:]

assert tb_num_features == num_features
const_output += gen_int("tb_num_compounds", tb_num_compounds)

tb_output = ""
tb_output += gen_mat(tb_in_matrix, "double", "tb_input")

#generate model
model_output = ""
for sample in session.samples:
    if (not sample.iter in selected_samples):
        model_output += "// skip sample %d\n" % sample.iter
        continue

    model_output += gen_sample(sample)

    predictions = []
    for comp in range(tb_num_compounds):
        feat = np.array(tb_in_matrix[comp, :])
        feat = np.squeeze(feat)
        predictions.append(sample.predict((feat, None)))

    tb_ref_matrix = np.stack(predictions)
    tb_output += gen_mat(tb_ref_matrix, "double", "s%d_tb_ref" % sample.iter)

with open(args.prefix + "const.h", "w") as os:
    os.write(const_output)

with open(args.prefix + "model.h", "w") as os:
    os.write(model_output)

with open(args.prefix + "tb.h", "w") as os:
    os.write(tb_output)

