#!/usr/bin/env python

import matrix_io as mio
import numpy as np
import sys
import os.path as pth
import io

def gen_dat(M, typename, varname):
    hdr = "const int %s_rows = %d;\n" % (varname, M.shape[0])
    hdr += "const int %s_cols = %d;\n" % (varname, M.shape[1])
    hdr += typename + " " + varname + "[%d][%d] = {" % M.shape
    ftr = "};"
    fmt = "    { " + "%.2f, " * (M.shape[1] - 1) + "%.2f },"

    f = io.StringIO() 

    np.savetxt(f, M,
            fmt = fmt,
            header = hdr,
            footer = ftr,
            newline = "\n",
            comments = "")

    ret = f.getvalue()

    return ret

# main
for f in sys.argv[1:]:
    M = mio.read_matrix(f)
    varname = pth.basename(pth.splitext(f)[0]).replace("-", "_")
    s = gen_dat(M, "const double", varname)
    print(s)

