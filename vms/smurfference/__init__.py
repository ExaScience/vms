
from .np import predict as np_predict
from .tf import predict as tf_predict
from .smurff import predict_cmd as smurff_predict_cmd
from .smurff import predict_im as smurff_predict_im
from .smurff import predict_py as smurff_predict_py
from .utils import train_session, calc_rmse, select_test

predict = smurff_predict_py
