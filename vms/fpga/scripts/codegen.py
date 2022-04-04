#!/usr/bin/env python3

import matrix_io as mio
import smurff
import numpy as np
import scipy as sp
import os
import os.path as pth
import io
import argparse
from configparser import ConfigParser
from glob import glob

def read_config(config_file):
    #read config_file
    config = ConfigParser()
    with open(config_file) as stream:
        config.read_string("[top]\n" + stream.read())  # add [top] section
    return config["top"]

def filter_file(inputfile, outputfile, patterns):
    lines = open(inputfile, "r").readlines() 
    for p in patterns:
        lines = filter(lambda l: not p.upper() in l.upper(), lines)
    open(outputfile, "w").writelines(lines) 

def filter_files(inputdir, outputdir, patterns):
    for inputfile in glob(pth.join(inputdir, '*')):
        if pth.isdir(inputfile):
            continue

        outputfile = pth.join(outputdir, pth.basename(inputfile))
        filter_file(inputfile, outputfile, patterns)

def gen_file(dir, suffix, content):
    os.makedirs(dir, exist_ok=True)
    with open(pth.join(dir, "vms_" + suffix), "w") as f:
        f.write(content)

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

def gen_const(tb_num_compounds, datatype, dataflow, patterns, num_proteins, num_features, num_latent, num_samples, block_size, F_vec_len):
    const_output = "#pragma once\n\n"

    const_output += f"#define VMS_DT_{datatype.upper()}\n"
    dataflow_str = "VMS_DATAFLOW" if dataflow else "VMS_NODATAFLOW"
    const_output += f"#define {dataflow_str}\n"
    patterns = " ".join(patterns)
    const_output += f"#define VMS_FILTER_PRAGMAS \"{patterns}\"\n"
    const_output += "\n"


    const_output += gen_int("tb_num_compounds",
                                             tb_num_compounds)
    const_output += gen_int("num_proteins",  num_proteins)
    const_output += gen_int("num_features",  num_features)
    const_output += gen_int("num_latent",    num_latent)
    const_output += gen_int("num_samples",   num_samples)
    const_output += gen_int("block_size",    block_size)
    const_output += gen_int("F_vec_len",     F_vec_len)

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


def gen_session(root, outputdir, config_file):
    config = read_config(config_file)

    datatype = config["datatype"]
    dataflow = config.getboolean("dataflow", True)
    block_size = config.getint("block_size")
    if config.getboolean("disable_bursts"):
        F_vec_len = 1
    elif block_size < 8:
        F_vec_len = block_size 
    else:
        F_vec_len = 8 

    patterns = config["filter_patterns"].split(",")
    patterns = list(filter(len, patterns)) # remove empty patterns

    # read model
    session = smurff.PredictSession(root)
    num_latent = session.num_latent
    assert num_latent == config.getint("num_latent")

    _, num_proteins = session.data_shape
    num_features = session.beta_shape[0]

    # read input features
    tb_file = session.options.get("side_info_0_0", "file")
    tb_in_matrix = mio.read_matrix(tb_file)
    if sp.sparse.issparse(tb_in_matrix):
        tb_in_matrix = np.array(tb_in_matrix.todense())
    (tb_num_compounds, tb_num_features) = tb_in_matrix.shape
    if (tb_num_compounds > 100):
        tb_in_matrix = tb_in_matrix[:100,:]
    (tb_num_compounds, tb_num_features) = tb_in_matrix.shape


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
    U = np.transpose(U, axes = (2,0,1)) # proteins, samples, latents
    B = np.transpose(B, axes = (2,0,1)) # features, samples, latents

    #generate testbench
    Pavg = np.mean(np.stack(P), axis=0)

    M_fx, M_wl, M_iwl = map_to_int(mu, np.int8)
    U_fx, U_wl, U_iwl = map_to_int(U, np.int16)
    B_fx, B_wl, B_iwl = map_to_int(B, np.int16)

    tb_in_matrix_fx, F_wl, F_iwl  = map_to_int(tb_in_matrix, np.int16)
    Pavg_fx, P_wl, P_iwl = map_to_int(Pavg, np.int8)

    format = "%.4f"
    tb_output = \
          gen_array(U,  "float", "U", "  ", format = format) + "\n" \
        + gen_array(mu, "float", "M", indent = "  ",  format = format) + "\n" \
        + gen_array(B,  "float", "B", "  ", format = format) + "\n"

    tb_output += gen_array(tb_in_matrix, "float", "tb_input", format = format) + "\n"
    tb_output += gen_array(Pavg, "float", "tb_ref", format = format) + "\n"
    gen_file(outputdir, "tb.h", tb_output)

    U.astype(np.float32).tofile(pth.join(outputdir, "vms_U.bin"))
    mu.astype(np.float32).tofile(pth.join(outputdir, "vms_M.bin"))
    B.astype(np.float32).tofile(pth.join(outputdir, "vms_B.bin"))
    tb_in_matrix.astype(np.float32).tofile(pth.join(outputdir, "vms_tb_in.bin"))
    Pavg.astype(np.float32).tofile(pth.join(outputdir, "vms_tb_ref.bin"))

    assert tb_num_features == num_features

    num_samples = len(samples)
    assert num_samples == config.getint("num_samples")

    const_output = gen_const(tb_num_compounds, datatype, dataflow, patterns, num_proteins, num_features, num_latent, len(samples), block_size, F_vec_len) + "\n"

    types = {
        8 : "signed char",
        16 : "signed short",
        32 : "signed int",
    }
    for name, wl, iwl in zip(
        [ "U", "M", "B", "F", "P" ],
        [ U_wl, M_wl, B_wl, F_wl, P_wl ],
        [ U_iwl, M_iwl, B_iwl, F_iwl, P_iwl ],
    ):
        const_output += gen_int(name + "_wl", wl)
        const_output += gen_int(name + "_iwl", iwl)
        const_output += gen_int(name + "_shift", wl - iwl)

    gen_file(outputdir, "const.h", const_output)

    if patterns:
        srcdir = pth.join(config["srcdir"], "cpp")
        filter_files(srcdir, outputdir, patterns)


parser = argparse.ArgumentParser(description='Generate SMURFF HLS inferencer C-code')
parser.add_argument('--config', metavar='config-file', dest="config_file", type=str, help='config file', default="config.mk")
parser.add_argument('--root', metavar='root-file', dest="root_file", type=str, help='root file', default="root.ini")
parser.add_argument('--output', metavar='DIR', type=str, help='output directory', default=".")
args = parser.parse_args()

gen_session(args.root_file, args.output, args.config_file)


