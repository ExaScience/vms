
import smurff
import scipy.sparse as sparse
import tempfile
import os
import shutil
import tempfile
from matrix_io import read_matrix

def ext(m):
    if sparse.issparse(m):
        return ".sdm"

    return ".ddm"

def select_test(predictions, test, verbose=False):
    # extract predictions in test matrix
    selected_predictions = [
        smurff.Prediction((i,j), v, pred_avg = predictions[i,j])
        for i,j,v in zip(*sparse.find(test))
    ]
    
    return selected_predictions

def add_globalmean(predictions, modeldir):
    fname = os.path.join(modeldir, "global_mean.ddm")
    if os.path.isfile(fname): 
        globalmean = read_matrix(fname).item()
        predictions += globalmean
    return predictions

def calc_rmse(predictions, test, verbose=False):
    selected_predictions = select_test(predictions, test, verbose)
    return smurff.calc_rmse(selected_predictions)

def train_session(modeldir, train_file, test_file, sideinfo_file = None):
    if not modeldir:
        modeldir = tempfile.mkdtemp()
    else:
        os.makedirs(modeldir)

    session = smurff.TrainSession(
                                num_latent=16,
                                burnin=800,
                                nsamples=800,
                                save_freq=1,
                                verbose = 0,
                                save_prefix = modeldir,
                                )
    train = read_matrix(train_file)
    test = read_matrix(test_file)
    
    session.addTrainAndTest(train, test, smurff.FixedNoise(1.0))
    if sideinfo_file is not None:
        sideinfo = read_matrix(sideinfo_file)
        session.addSideInfo(0, sideinfo, smurff.FixedNoise(10.), direct=True)

    predictions = session.run()
    rmse = smurff.calc_rmse(predictions)

    #print("RMSE = %.2f%s" % (rmse, "" if sideinfo is None else " (with sideinfo)" ))
    return rmse, modeldir
