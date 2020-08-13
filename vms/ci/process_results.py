import re
from datetime import datetime

fname = "/scratch/vms/last/ci.log"



result_data = {}
curent = ""

def extract_time(line):  
    m = re.search("\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}", line)
    time_str = m.group()
    time_obj = datetime. strptime(time_str, '%Y-%m-%d %H:%M:%S')
    return time_obj

def process_start(m, line, file):
    global current, result_data
    dataset, num_latent, num_samples, datatype = m.group(1, 2, 3, 4)
    num_latent = int(num_latent)
    num_samples = int(num_samples)
    current = f"{dataset}-{num_latent:03d}-{num_samples:03d}-{datatype}"
    result_data[current] = {
        "start_time" : extract_time(line),
        "dataset" : dataset,
        "num_latent" : num_latent,
        "num_samples" : num_samples,
        "datatype" : datatype,
    }
   
def process_error(m, line, file):
    global current, result_data
    result_data[current]["status"] = "failed"
    result_data[current]["end_time"] = extract_time(line)
    
    
def process_success(m, line, file):
    global current, result_data
    result_data[current]["status"] = "success"
    result_data[current]["end_time"] = extract_time(line)
    
regexes = {
    "Starting dataset=(\w+) num_latent=(\d+) num_samples=(\d+) datatype=(\w+)" : process_start,
    "ERROR" : process_error,
    "predict.xtasks.config:" : process_success,
}

with open(fname, "r") as f:  
    for line in f:
        for regex, func in regexes.items():
            m = re.search(regex, line)
            if m: 
                func(m, line, f)
              

from datetime import timedelta

for k,v in result_data.items():
    if v["datatype"] in ["mixed", "half" ]:
        continue
    delta = v["end_time"] - v["start_time"]
    hours = int(delta.total_seconds() // 3600)
    status = v["status"]
    print(f"{k} {status} after {hours} hours")


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
