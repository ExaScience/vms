#!/usr/bin/env python

import matrix_io as mio
import numpy as np
import sys
import os.path as pth

def gen_dat(M, typename, varname):
    h = typename + " " + varname + "[%d][%d] = {" % M.shape
    f = "};"

    fmt = "{ " + "%.2f, " * (M.shape[1] - 1) + "%.2f }"

    np.savetxt(sys.stdout, M,
            fmt = fmt,
            header = h,
            footer = f,
            newline = "\n    ",
            comments = "");

# main
for f in sys.argv[1:]:
    M = mio.read_matrix(f)
    varname = pth.basename(pth.splitext(f)[0]).replace("-", "_")
    gen_dat(M, "double", varname)

