#!/usr/bin/env python

import matrix_io as mio
import smurff
import numpy as np
import scipy as sp
import sys
import os.path as pth
import io
import argparse

prefix = "smurff_"

def gen_file(suffix, content):
    with open(prefix + suffix, "w") as os:
        os.write(content)


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
const U_type sample_1_U0_latents[4][3] = {
    { 0.22, 1.40, 1.15 },
    { -0.26, 0.74, 2.48 },
    { -0.62, 0.29, 0.17 },
    { 1.61, 3.64, 5.08 },
};


    """
    hdr = gen_int(varname + "_rows", M.shape[0], indent)
    hdr += gen_int(varname + "_cols", M.shape[1], indent)
    hdr += indent + "const " + typename + " " + varname + "[%d][%d] = {" % M.shape
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

def gen_vec(V, typename, varname, indent = ""):
    """Generate C code for vector V,
    Something like:

const int gate_1_U0_latents_size = 3;
const U_type gate_1_U0_latents[3] = {
    { 0.22, 1.40, 1.15 }
};


    """
    decl  = gen_int(varname + "_size", V.shape[0], indent)
    decl += indent + "extern const " + typename + " " + varname + "[%d];\n" % V.shape

    hdr = indent + "const " + typename + " " + varname + "[%d] = {" % V.shape
    ftr = indent + "};"
    fmt = indent + " %+.8f,"

    f = io.StringIO() 
    np.savetxt(f, V,
            fmt = fmt,
            header = hdr,
            footer = ftr,
            newline = "\n",
            comments = "")

    return decl + f.getvalue()



def gen_sample(i, U, mu, B):
    # (nc x np) = ((nc x nf) * (nf x nl)) * (nl x np)

    return  "namespace sample_%d {\n\n" % i \
        + gen_mat(U, "U_type", "U", "  ") + "\n" \
        + gen_vec(mu, "mu_type", "mu", "  ") + "\n" \
        + gen_mat(B, "B_type", "B", "  ") + "\n" \
        + "} // end namespace sample_%d\n" % i

def gen_const(num_compounds, num_proteins, num_features, num_latent, num_samples):
    const_output = "#pragma once\n"
    const_output += gen_int("num_compounds", num_compounds)
    const_output += gen_int("num_proteins",  num_proteins)
    const_output += gen_int("num_features",  num_features)
    const_output += gen_int("num_latent",    num_latent)
    const_output += gen_int("num_samples",   num_samples)
    gen_file("const.h", const_output)

def gen_session(root):
    # read model
    session = smurff.PredictSession(root)
    num_latent = session.num_latent
    num_compounds, num_proteins = session.data_shape
    num_features = session.beta_shape[0]

    # read input features
    tb_file = pth.join(session.root_dir, session.options.get("side_info_0_0", "file"))
    tb_in_matrix = mio.read_matrix(tb_file)
    if sp.sparse.issparse(tb_in_matrix):
        tb_in_matrix = np.array(tb_in_matrix.todense())

    #generate model
    num_samples = 0
    predictions = []
    model_output = ""
    for sample in session.samples():
        U = sample.latents[1]
        mu = sample.mus[0]
        F = sample.betas[0]
        P = np.matmul(np.matmul(tb_in_matrix, F.transpose()) + mu, U)
        predictions.append(P)

        gen_file("sample_%d.h" % num_samples, gen_sample(num_samples, U, mu, F))
        model_output += '#include "%ssample_%d.h"\n' % (prefix, num_samples)

        num_samples += 1

    model_output += "\nsample samples[%d] = {\n" % num_samples
    for i in range(num_samples):
        model_output += " { sample_%d::U, sample_%d::mu, sample_%d::B },\n" % (i, i, i) 
    model_output += "};\n\n"

    gen_file("model.h", model_output)

    #generate testbench

    (num_compounds, tb_num_features) = tb_in_matrix.shape
    print(predictions[-1])
    predictions  = np.stack(predictions)
    Pavg = np.mean(predictions, axis=0)
    tb_output = gen_mat(tb_in_matrix, "F_type", "tb_input")
    tb_output += gen_mat(Pavg, "P_type", "tb_ref")
    gen_file("tb.h", tb_output)

    assert tb_num_features == num_features

    gen_const(num_compounds, num_proteins, num_features, num_latent, num_samples)

parser = argparse.ArgumentParser(description='Generate SMURFF HLS inferencer C-code')
parser.add_argument('--root', metavar='root-file', dest="root_file", type=str, help='root file')
args = parser.parse_args()

if args.root_file:
    gen_session(args.root_file)


