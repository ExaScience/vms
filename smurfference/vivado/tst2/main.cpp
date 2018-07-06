#include <iostream>

#include <Eigen/Dense>

#include "predict_tst2.h"

typedef const Eigen::Matrix<double, num_latent, num_feat, Eigen::RowMajor> MatrixF0;
typedef const Eigen::Matrix<double, num_latent, num_proteins, Eigen::RowMajor> MatrixU1;
typedef const Eigen::Matrix<double, num_comp, num_feat, Eigen::RowMajor> MatrixIn;
typedef       Eigen::Matrix<double, num_comp, num_proteins, Eigen::RowMajor> MatrixOut;

typedef Eigen::Map<MatrixF0> MapF0;
typedef Eigen::Map<MatrixU1> MapU1;
typedef Eigen::Map<MatrixIn> MapIn;
typedef Eigen::Map<MatrixOut> MapOut;


void predict_compound_block_eigen(
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

   std::cout << __func__ << ":\n" << map_out << std::endl;
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

    double output_pred1[num_comp][num_proteins];
    double output_pred2[num_comp][num_proteins];

    predict_compound_block_eigen(input_features, output_pred1);
    predict_compound_block_c(input_features, output_pred2);

    MapOut map_out1(&output_pred1[0][0]);
    MapOut map_out2(&output_pred2[0][0]);

    std::cout << "diff: " << (map_out1 - map_out2).norm() << std::endl;

    return 0;
}
