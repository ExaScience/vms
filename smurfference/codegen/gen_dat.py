#!/usr/bin/env python

import matrix_io as mio
import smurff
import numpy as np
import scipy as sp
import sys
import os.path as pth
import io
import argparse

def gen_int(name, value, indent = ""):
    """Generates code for an int:
    
    const int sample_1_U0_latents_rows = 4;
    """

    return indent + "const int %s = %d;\n" % (name, value)
 

def gen_mat(M, typename, varname, indent = ""):
    """Generate C code for matrix M,
    Something like:

const int sample_1_U0_latents_rows = 4;
const int sample_1_U0_latents_cols = 3;
const U0_type sample_1_U0_latents[4][3] = {
    { 0.22, 1.40, 1.15 },
    { -0.26, 0.74, 2.48 },
    { -0.62, 0.29, 0.17 },
    { 1.61, 3.64, 5.08 },
};


    """
    hdr = gen_int(varname + "_rows", M.shape[0], indent)
    hdr += gen_int(varname + "_cols", M.shape[1], indent)
    hdr += indent + typename + " " + varname + "[%d][%d] = {" % M.shape
    ftr = "};"
    fmt = indent + "  { " + "%+.8f, " * (M.shape[1] - 1) + "%+.8f },"

    f = io.StringIO() 

    np.savetxt(f, M,
            fmt = fmt,
            header = hdr,
            footer = ftr,
            newline = "\n",
            comments = "")

    return f.getvalue()

def gen_sample(sample, tb_in_matrix):
    ret = "namespace sample_%d {\n\n" % sample.iter
    for i, U in enumerate(sample.latents):
        ret += gen_mat(U, "Uin_type", "U%d" % i, "  ") + "\n"

    for i, F in enumerate(sample.betas):
        ret += gen_mat(F, "Fin_type", "F%d" % i, "  ") + "\n"

    predictions = []
    for comp in range(tb_in_matrix.shape[0]):
        feat = np.array(tb_in_matrix[comp, :])
        feat = np.squeeze(feat)
        predictions.append(sample.predict((feat, None)))

    tb_ref_matrix = np.stack(predictions)
    ret += gen_mat(tb_ref_matrix, "Pout_type", "s%d_tb_ref" % sample.iter, "  ") + "\n"

    ret += "} // end namespace sample_%d\n" % sample.iter

    return ret

def gen_types():
    output = "#pragma once\n"
    output += "typedef double Fin_type;\n"
    output += "typedef double Uin_type;\n"
    output += "typedef double Uout_type;\n"
    output += "typedef double Pout_type;\n"
    return output

parser = argparse.ArgumentParser(description='Generate SMURFF HLS inferencer C-code')
parser.add_argument('--out-prefix', dest='prefix', default='smurff_', help='output file prefix')
parser.add_argument('--root', metavar='root-file', type=str, help='root file', required=True)

args = parser.parse_args()

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
tb_output += gen_mat(tb_in_matrix, "Fin_type", "tb_input")

def gen_file(suffix, content):
    with open(args.prefix + suffix, "w") as os:
        os.write(content)

#generate model
for sample in session.samples:
    gen_file("sample_%d.h" % sample.iter, gen_sample(sample, tb_in_matrix))

gen_file("const.h", const_output)
gen_file("tb.h", tb_output)
gen_file("types.h", gen_types())

