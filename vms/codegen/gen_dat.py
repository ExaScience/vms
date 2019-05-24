#!/usr/bin/env python

import matrix_io as mio
import smurff
import numpy as np
import scipy as sp
import sys
import os.path as pth
import glob
import shutil
import io
import argparse



def gen_file(dir, suffix, content):
    with open(pth.join(dir, "smurff_" + suffix), "w") as os:
        os.write(content)


def gen_int(name, value, indent = ""):
    """Generates code for an int:
    
    const int sample_1_U0_latents_rows = 4;
    """

    return indent + "const int %s = %d;\n" % (name, value)


def gen_body(i, M, indent, format):
    output = indent + "{\n"
    if i > 0:
        output = ",\n" + output 

    if (len(M.shape) <= 1):
        f = io.StringIO() 
        np.savetxt(f, M, fmt = format, newline=", ")
        output += indent + "  " + f.getvalue()
    else:
        for i, subM in enumerate(np.split(M, M.shape[0])):
            output += gen_body(i, np.squeeze(subM), indent + "  ", format) 
            
    return output + "\n" + indent + "}"

def gen_array(M, typename, varname, indent = "", format = "%+.8f"):
    hdr = indent + "const " + typename + " " + varname + "[%s]" * len(M.shape) + " = \n"
    hdr = hdr % M.shape
    ftr = ";\n"

    return hdr + gen_body(0, M, indent, format) + ftr

def gen_const(num_proteins, num_features, num_latent, num_samples):
    const_output = "#pragma once\n"
    const_output += gen_int("num_proteins",  num_proteins)
    const_output += gen_int("num_features",  num_features)
    const_output += gen_int("num_latent",    num_latent)
    const_output += gen_int("num_samples",   num_samples)
    log_num_samples = num_samples.bit_length() - 1
    const_output += gen_int("log_num_samples", log_num_samples)

    assert 2**log_num_samples == num_samples

    return const_output

def map_to_int(M, dt):
    dtinfo = np.iinfo(dt)

    min = np.min(M)
    max = np.max(M)
    avg = np.mean(M)
    print("min: %4f, max %.4f, avg: %.4f" % (min, max, avg))
    print(M.flatten()[:10])
    assert max > 0

    shift = 0
    while (2*max < dtinfo.max and 2*min > dtinfo.min):
        max *= 2
        min *= 2
        shift += 1
    
    M = M * (2**shift)

    min = np.min(M)
    max = np.max(M)
    avg = np.mean(M)
    print("min: %4f, max %.4f, avg: %.4f" % (min, max, avg))
    print(M.flatten()[:10])

    M = M.round().astype(dt)

    min = np.min(M)
    max = np.max(M)
    avg = np.mean(M)
    print("min: %4f, max %.4f, avg: %.4f" % (min, max, avg))
    print(M.flatten()[:10])
    
    return M, shift


def gen_session(root, outputdir):
    # read model
    session = smurff.PredictSession(root)
    num_latent = session.num_latent
    num_compounds, num_proteins = session.data_shape
    num_features = session.beta_shape[0]

    # read input features
    tb_file = session.options.get("side_info_0_0", "file")
    tb_in_matrix = mio.read_matrix(tb_file)
    if sp.sparse.issparse(tb_in_matrix):
        tb_in_matrix = np.array(tb_in_matrix.todense())
    (num_compounds, tb_num_features) = tb_in_matrix.shape
    if (num_compounds > 10):
        tb_in_matrix = tb_in_matrix[:10,:]
    (num_compounds, tb_num_features) = tb_in_matrix.shape


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
    U = np.transpose(U, axes = (0,2,1))
    B = np.transpose(B, axes = (0,2,1))

    mu, mu_shift = map_to_int(mu, np.int8)
    U, U_shift = map_to_int(U, np.int16)
    B, B_shift = map_to_int(B, np.int16)

    tb_output = gen_int("num_compounds", num_compounds)
    tb_output += \
          gen_array(U, "std::int16_t", "U", "  ", format = "%d") + "\n" \
        + gen_array(mu, "std::int8_t", "mu", indent = "  ",  format = "%d") + "\n" \
        + gen_array(B, "std::int16_t", "B", "  ", format = "%d") + "\n"

    #generate testbench
    Pavg = np.mean(np.stack(P), axis=0)

    tb_in_matrix, F_shift = map_to_int(tb_in_matrix, np.int16)
    Pavg, P_shift = map_to_int(Pavg, np.int8)

    tb_output += gen_array(tb_in_matrix, "std::int16_t", "tb_input", format = "%d") + "\n"
    tb_output += gen_array(Pavg, "std::int8_t", "tb_ref", format = "%d") + "\n"
    gen_file(outputdir, "tb.h", tb_output)

    assert tb_num_features == num_features

    const_output = gen_const(num_proteins, num_features, num_latent, len(samples)) + "\n"

    const_output += gen_int("mu_shift", mu_shift)
    const_output += gen_int("mu_wl", 8)
    const_output += "const int mu_iwl = mu_wl - mu_shift;\n"

    const_output += gen_int("U_shift", U_shift)
    const_output += gen_int("U_wl", 16)
    const_output += "const int U_iwl = U_wl - U_shift;\n"

    const_output += gen_int("B_shift", B_shift)
    const_output += gen_int("B_wl", 16)
    const_output += "const int B_iwl = B_wl - B_shift;\n"

    const_output += "\n"
    const_output += gen_int("F_shift", F_shift)
    const_output += gen_int("F_wl", 16)
    const_output += "const int F_iwl = F_wl - F_shift;\n"

    const_output += gen_int("P_shift", P_shift)
    const_output += gen_int("P_wl", 8)
    const_output += "const int P_iwl = P_wl - P_shift;\n"

    gen_file(outputdir, "const.h", const_output)

    codedir = pth.join(pth.dirname(pth.realpath(__file__)), "code")
    codefiles = glob.glob(pth.join(codedir, '*.cpp')) + glob.glob(pth.join(codedir, '*.h'))
    for c in codefiles:
        pass
        #shutil.copy(c, outputdir)


parser = argparse.ArgumentParser(description='Generate SMURFF HLS inferencer C-code')
parser.add_argument('--root', metavar='root-file', dest="root_file", type=str, help='root file', default="root.ini")
parser.add_argument('--output', metavar='DIR', type=str, help='output directory', default=".")
args = parser.parse_args()

gen_session(args.root_file, args.output)


