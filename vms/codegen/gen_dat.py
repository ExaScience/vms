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
    assert max > 0

    shift = 0
    while (2*max < dtinfo.max and 2*min > dtinfo.min):
        max *= 2
        min *= 2
        shift += 1
    
    M = M * (2**shift)
    M = M.round().astype(dt)
    return M, dtinfo.bits, dtinfo.bits - shift


def gen_session(root, outputdir, fixed_type):
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

    #generate testbench
    Pavg = np.mean(np.stack(P), axis=0)

    if fixed_type:
        mu, mu_wl, mu_iwl = map_to_int(mu, np.int8)
        U, U_wl, U_iwl = map_to_int(U, np.int16)
        B, B_wl, B_iwl = map_to_int(B, np.int16)

        tb_in_matrix, F_wl, F_iwl  = map_to_int(tb_in_matrix, np.int16)
        Pavg, P_wl, P_iwl = map_to_int(Pavg, np.int8)

        format = "%d"
    else:
        format = "%.4f"

    tb_output = gen_int("num_compounds", num_compounds)
    tb_output += \
        gen_array(U,  "U_base", "U", "  ", format = format) + "\n" \
        + gen_array(mu, "mu_base", "mu", indent = "  ",  format = format) + "\n" \
        + gen_array(B,  "B_base", "B", "  ", format = format) + "\n"

    tb_output += gen_array(tb_in_matrix, "F_base", "tb_input", format = format) + "\n"
    tb_output += gen_array(Pavg, "P_base", "tb_ref", format = format) + "\n"
    gen_file(outputdir, "tb.h", tb_output)

    assert tb_num_features == num_features

    const_output = gen_const(num_proteins, num_features, num_latent, len(samples)) + "\n"

    if fixed_type:
        types_output = "#define DT_FLOAT\n"
        types_output += '#include "fxp.h"\n\n'
        types = {
            8 : "signed char",
            16 : "signed short",
            32 : "signed int",
        }
        for name, wl, iwl in zip(
            [ "U", "mu", "B", "F", "P" ],
            [ U_wl, mu_wl, B_wl, F_wl, P_wl ],
            [ U_iwl, mu_iwl, B_iwl, F_iwl, P_iwl ],
        ):
            types_output += gen_int(name + "_wl", wl)
            types_output += gen_int(name + "_iwl", iwl)
            types_output += gen_int(name + "_shift", wl - iwl)
            types_output += "typedef %s %s_base ;\n" % (types[wl], name)
            types_output += "typedef fxp<%s, %d> %s_type;\n\n" % (types[wl], iwl, name)

        types_output += "typedef fxp<int, 10> L_type;\n"
        types_output += "typedef fxp<int, 10> S_type;\n"
        types_output += "const float epsilon = 0.5;\n"
    else:
        types_output = "#define DT_FIXED\n"
        for name in [ "U", "mu", "B", "F", "P" ]:
            types_output += "typedef float %s_base ;\n" % name
            types_output += "typedef float %s_type;\n\n" % name

        types_output += "typedef float L_type;\n"
        types_output += "typedef float S_type;\n"
        types_output += "const float epsilon = 0.5;\n"

    gen_file(outputdir, "const.h", const_output)
    gen_file(outputdir, "types.h", types_output)


parser = argparse.ArgumentParser(description='Generate SMURFF HLS inferencer C-code')
parser.add_argument('--root', metavar='root-file', dest="root_file", type=str, help='root file', default="root.ini")
parser.add_argument('--output', metavar='DIR', type=str, help='output directory', default=".")
parser.add_argument('--fxp', action='store_true', help='use fixed point')
args = parser.parse_args()

gen_session(args.root_file, args.output, args.fxp)


