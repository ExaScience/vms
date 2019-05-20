#include <iostream>

#include <Eigen/Dense>

#include "data.c"

Eigen::Matrix<double,sample_1_U0_latents_cols,sample_1_U1_latents_cols> W;

void predict_all()
{
   Eigen::Map<const Eigen::Matrix<double, sample_1_U0_latents_rows, sample_1_U0_latents_cols>> U0(&sample_1_U0_latents[0][0]);
   Eigen::Map<const Eigen::Matrix<double, sample_1_U1_latents_rows, sample_1_U1_latents_cols>> U1(&sample_1_U1_latents[0][0]);


   W = U0.transpose() * U1;
}

int main()
{
   predict_all();

   std::cout << W << std::endl;

   return 0;
}