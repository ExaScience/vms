#!/usr/bin/env python

import urllib.request
import matrix_io as mio
import os
from hashlib import sha256
import smurff

urls = [
        (
            "http://homes.esat.kuleuven.be/~jsimm/chembl-IC50-346targets.mm",
            "10c3e1f989a7a415a585a175ed59eeaa33eff66272d47580374f26342cddaa88",
            "chembl-IC50-346targets.mm",
            ),
        (
            "http://homes.esat.kuleuven.be/~jsimm/chembl-IC50-compound-feat.mm",
            "f9fe0d296272ef26872409be6991200dbf4884b0cf6c96af8892abfd2b55e3bc",
            "chembl-IC50-compound-feat.mm", 
            ),
        ]

for url, expected_sha, output in urls:
    if os.path.isfile(output):
        actual_sha = sha256(open(output, "rb").read()).hexdigest()
        if (expected_sha == actual_sha):
            continue

    print("download %s" % output)
    urllib.request.urlretrieve(url, output)

ic50 = mio.read_matrix("chembl-IC50-346targets.mm")
ic50_train, ic50_test = smurff.make_train_test(ic50, 0.2)

mio.write_matrix("chembl-IC50-346targets-train.mm", ic50_train)
mio.write_matrix("chembl-IC50-346targets-test.mm", ic50_test)
