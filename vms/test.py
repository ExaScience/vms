import unittest
from multipredict import multipredict


class TestMultiPredict(unittest.TestCase):
    def test_small_model(self):
        results = multipredict(
            "test_data/small/sideinfo.mm", "test_data/small/test.mm", modeldir="test_data/small/model")
        reference = {'macau in matrix    ': 0.9637726940027302, 'macau out-of-matrix': 0.9346726169938081, 'smurff_cmd ': 0.934672616993808,
                     'tf         ': 0.9670494454501363, 'np         ': 0.9670494399014289}

        for name in reference.keys():
            res = results[name]
            ref = reference[name]
            self.assertAlmostEqual(
                res, ref, places=2, msg="method: " + name)
