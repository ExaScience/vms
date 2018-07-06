#include <iostream>

#include <Eigen/Dense>

// size of latent protein representation
const int num_latent = 4;
const int num_proteins = 3;

const double U1[num_latent][num_proteins] = {
    { 0.40, 0.36, 0.34 },
    { 1.33, 0.90, 0.94 },
    { 0.14, 0.06, -0.3 },
    { 0.96, 1.29, 1.64 },
};

const int num_feat = 2;

const double F0[num_latent][num_feat] = {
    { 0.40, 0.36 },
    { 1.33, 0.905 },
    { 0.14, 0.061 },
    { 0.96, 1.29 },
};

const int num_comp = 8;

typedef const Eigen::Matrix<double, num_latent, num_feat, Eigen::RowMajor> MatrixF0;
typedef const Eigen::Matrix<double, num_latent, num_proteins, Eigen::RowMajor> MatrixU1;
typedef const Eigen::Matrix<double, num_comp, num_feat, Eigen::RowMajor> MatrixIn;
typedef       Eigen::Matrix<double, num_comp, num_proteins, Eigen::RowMajor> MatrixOut;

typedef Eigen::Map<MatrixF0> MapF0;
typedef Eigen::Map<MatrixU1> MapU1;
typedef Eigen::Map<MatrixIn> MapIn;
typedef Eigen::Map<MatrixOut> MapOut;


void predict_compound_block(
   const double in[num_comp][num_feat],
   double out[num_comp][num_proteins]
)
{
   MapF0  map_f0 (&F0 [0][0]);
   MapU1  map_u1 (&U1 [0][0]);
   MapIn  map_in (&in [0][0]);
   MapOut map_out(&out[0][0]);

   //        ((nc x nf) * (nf * nl)) * (nl * np)
   //        (nc        x        nl) * (nl * np)
   //        (nc             x               np)
   map_out = (map_in * map_f0.transpose()) * map_u1;
}

int main()
{
    double input_features[num_comp][num_feat] = {
        {0.40, 0.36},
        {1.33, 0.905},
        {0.14, 0.061},
        {0.96, 1.29},
        {0.40, 0.36},
        {1.33, 0.905},
        {0.14, 0.061},
        {0.96, 1.29},
    };

    double output_predictions[num_comp][num_proteins];

    predict_compound_block(input_features, output_predictions);

    return 0;
}