import re
from datetime import datetime
import argparse

parser = argparse.ArgumentParser("Exploration log parser")
parser.add_argument("fname")
args = parser.parse_args()

result_data = {}
curent = ""

def extract_time(line):  
    m = re.search("\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}", line)
    time_str = m.group()
    time_obj = datetime. strptime(time_str, '%Y-%m-%d %H:%M:%S')
    return time_obj

def process_start(m, line, file):
    global current
    dataset, num_latent, num_samples, datatype = m.group(1, 2, 3, 4)
    num_latent = int(num_latent)
    num_samples = int(num_samples)
    current = f"{dataset}-{num_latent:03d}-{num_samples:03d}-{datatype}"
    result_data[current] = {}

    return {
        "start_time" : extract_time(line),
        "dataset" : dataset,
        "num_latent" : num_latent,
        "num_samples" : num_samples,
        "datatype" : datatype,
    }
   
def process_error(m, line, file):
    return {
        "status" : "failed",
        "end_time" : extract_time(line),
    }
    
def process_success(m, line, file):
    return {
        "status" : "success",
        "end_time" :  extract_time(line),
    }


def process_utilization(m, line, file):
    return { "utilization" : eval(line) }

def process_latency(m, line, file):
    return { "latency" : int(m.group(1)) }

def process_frequency(m, line, file):
    return { "frequency" : float(m.group(1)) }

regexes = {
    "Starting dataset=(\w+) num_latent=(\d+) num_samples=(\d+) datatype=(\w+)" : process_start,
    "ERROR" : process_error,
    "predict.xtasks.config:" : process_success,
    "^{" :  process_utilization,
    "Average latency of dataflow_in_loop: (\d+)" :  process_latency,
    "^454881815\s+\d+\s+\S+\s+([\d\.]+)" :  process_frequency,
}

with open(args.fname, "r") as f:  
    for line in f:
        for regex, func in regexes.items():
            m = re.search(regex, line)
            if m: 
                u = func(m, line, f)
                result_data[current].update(u)
              

from datetime import timedelta

num_proteins = 114;
num_features = 469;

for k,v in result_data.items():
    if v["datatype"] in ["mixed", "half" ]:
        continue

    status = v["status"]
    if (status != "success"):
        continue

    delta = v["end_time"] - v["start_time"]
    hours = delta.total_seconds() / 3600
    ops = v["num_latent"] * v["num_samples"] * (num_proteins * num_features)
    cycles = v["latency"]
    compounds_per_second = 1e6 * v["frequency"] / cycles
    gops_per_second = (ops / cycles) * (v["frequency"] / 1e3)
    tops_per_second = gops_per_second / 1e3

    print(f"{k}\n  {status} after {hours:.1f} hours, {ops} ops in {cycles} cycles")
    print(f"  {compounds_per_second:.1f} compounds per second")
    if gops_per_second > 1000:
        print(f"  {tops_per_second:.1f} tops/sec")
    else:
        print(f"  {gops_per_second:.1f} gops/sec")
    print(f"  utilization:", end='')
    for k,v in v["utilization"].items():
        if (v[1] == 0):
            continue
        percent = 100 * float(v[1]) / float(v[0])
        print(f" {k}: {percent:.0f}%", end='')
    print()


# chem2vec-008-008-float success after 0 hours
# chem2vec-008-008-fixed failed after 0 hours
# chem2vec-008-016-float success after 1 hours
# chem2vec-008-016-fixed success after 0 hours
# chem2vec-008-032-float success after 8 hours
# chem2vec-008-032-fixed success after 2 hours
# chem2vec-016-008-float success after 1 hours
# chem2vec-016-008-fixed success after 0 hours
# chem2vec-016-016-float success after 8 hours
# chem2vec-016-016-fixed success after 1 hours
# chem2vec-016-032-float failed after 111 hours
# chem2vec-016-032-fixed success after 6 hours
# chem2vec-032-008-float failed after 9 hours
# chem2vec-032-008-fixed success after 1 hours
# chem2vec-032-016-float failed after 107 hours
# chem2vec-032-016-fixed failed after 0 hours
# chem2vec-032-032-float failed after 0 hours
# chem2vec-032-032-fixed failed after 0 hours
