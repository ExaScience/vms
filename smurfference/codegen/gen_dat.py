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


def gen_body(i, M, indent):
    output = indent + "{\n"
    if i > 0:
        output = ",\n" + output 

    if (len(M.shape) <= 1):
        f = io.StringIO() 
        np.savetxt(f, M, fmt = "%+.8f", newline=", ")
        output += indent + "  " + f.getvalue()
    else:
        for i, subM in enumerate(np.split(M, M.shape[0])):
            output += gen_body(i, np.squeeze(subM), indent + "  ") 
            
    return output + "\n" + indent + "}"

def gen_array(M, typename, varname, indent = ""):
    hdr = indent + "const " + typename + " " + varname + "[%s]" * len(M.shape) + " = \n"
    hdr = hdr % M.shape
    ftr = ";"

    return hdr + gen_body(0, M, indent) + ftr

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
    samples = []
    P = []
    for sample in session.samples():
        U = sample.latents[1]
        mu = sample.mus[0]
        F = sample.betas[0]
        P.append(np.matmul(np.matmul(tb_in_matrix, F.transpose()) + mu, U))
        samples.append((U, mu, F))

    U, mu, B = [ np.stack(s) for s in zip(*samples) ]

    model_out = gen_array(U, "U_type", "U", "  ") + "\n" \
        + gen_array(mu, "mu_type", "mu", "  ") + "\n" \
        + gen_array(B, "B_type", "B", "  ") + "\n"

    gen_file("model.h",  model_out)

    #generate testbench
    (num_compounds, tb_num_features) = tb_in_matrix.shape
    P  = np.stack(P)
    Pavg = np.mean(P, axis=0)
    tb_output = gen_array(tb_in_matrix, "F_type", "tb_input")
    tb_output += gen_array(Pavg, "P_type", "tb_ref")
    gen_file("tb.h", tb_output)

    assert tb_num_features == num_features

    gen_const(num_compounds, num_proteins, num_features, num_latent, len(samples))

parser = argparse.ArgumentParser(description='Generate SMURFF HLS inferencer C-code')
parser.add_argument('--root', metavar='root-file', dest="root_file", type=str, help='root file')
args = parser.parse_args()

if args.root_file:
    gen_session(args.root_file)


