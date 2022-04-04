import requests
import os, shutil
from hashlib import sha256

def download_file_from_google_drive(id, destination):
    URL = "https://docs.google.com/uc?export=download"

    session = requests.Session()

    response = session.get(URL, params = { 'id' : id }, stream = True)
    token = get_confirm_token(response)

    if token:
        params = { 'id' : id, 'confirm' : token }
        response = session.get(URL, params = params, stream = True)

    save_response_content(response, destination)    

    session.close()

def get_confirm_token(response):
    for key, value in response.cookies.items():
        if key.startswith('download_warning'):
            return value

    return None

def save_response_content(response, destination):
    CHUNK_SIZE = 32768

    with open(destination, "wb") as f:
        for chunk in response.iter_content(CHUNK_SIZE):
            if chunk: # filter out keep-alive new chunks
                f.write(chunk)

def sha256_from(filename):
    with open(filename, "rb") as f:
        sha = sha256(f.read()).hexdigest()

    return sha


cache_dir = os.path.join(os.environ["HOME"], ".cache")

def download_if_needed(id, expected_sha, output):
    global cache_dir

    if os.path.isfile(output):
        actual_sha = sha256_from(output)
        print("Found %s in output directory" % output)
        assert expected_sha == actual_sha
        return

    cache_file = os.path.join(cache_dir, expected_sha)
    if os.path.isfile(cache_file):
        print("Found %s in cache" % output)
        shutil.copyfile(cache_file, output)
    else:
        print("Downloading %s" % output)
        download_file_from_google_drive(id, output)
        actual_sha = sha256_from(output)
        assert (expected_sha == actual_sha)
        shutil.copyfile(output, cache_file)

def download():
    urls = [
            ( 
                "1MI2gXqc0-PM77qxReYQrmO1QyAITMBZ0",
                "f49c480076ee43f5635bd957a07c44a8a06133d10985c683b9260930831eb163",
                "features.ddm",
            ),
            ( 
                "1OpvOLh0fwFQQRDyp8vYdAGU_CIy9WNCP",
                "9b8e458612a72d7051463d761248c54edfcb8bbfc73266536a5791fa5b047da2",
                "test.sdm",
            ),
            ( 
                "1TGq9qSkKa7fvnwdJ2g5drTTaNTwlnOYF",
                "f6d9315f2c905146275caa1e1b03e380d870c33166caec81c2bdb35a7efe77ef",
                "train.sdm",
            ),
    ]
 
    for id, expected_sha, output in urls:
        download_if_needed(id, expected_sha, output)

if __name__ == "__main__":
    download()

# shapes:
# side_c2v.ddm : (46738, 469)
# side_ecfp6_counts_var005.sdm : (46738, 13132) nnz: 1663083 (0.27%)
# side_ecfp6_folded_dense.ddm : (46738, 1024)
# test.sdm : (46738, 114) 21740
# train.sdm : (46738, 114) 37640
