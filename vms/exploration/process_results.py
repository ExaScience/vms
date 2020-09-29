import re, os
from glob import glob
import pandas as pd
from datetime import datetime

results = []
fname = "ci_20200723-165624.log"
time = None

with open(fname, "r") as f:  
    for line in f:
        #Starting dataset=chem2vec num_latent=8 num_samples=8 datatype=float in /scratch/vms/ci_20200723-165624/chem2vec_8_8_float
        expr = "Starting dataset=(\w+) num_latent=(\d+) num_samples=(\d+) datatype=(\w+)"
        m = re.search(expr, line)           
        if m:
            prev_time = time
            time = line.split(',')[0]
            time = datetime.strptime(time, "%Y-%m-%d %H:%M:%S")
            if prev_time:
                elapsed = time - prev_time
                print(dataset, num_latent, num_samples, datatype, elapsed, failed, utilization)
                      
            dataset, num_latent, num_samples, datatype = m.groups()
            failed = "SUCCESS"
            utilization = None
                                          
        expr = "^{'"
        m = re.search(expr, line)           
        if m:         
            utilization = eval(line)

        expr = "ERROR"
        m = re.search(expr, line)           
        if m:         
            failed = "ERROR"
