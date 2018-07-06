#include "predict_tst2.h"

void predict_compound_block_c(
   const double in[num_comp][num_feat],
   double out[num_comp][num_proteins]
)
{
    int c, d, k;
    double tmp[num_comp][num_latent];

    //       ((nc x nf) * (nf * nl)) * (nl * np)
    //       (nc        x        nl) * (nl * np)
    //       (nc             x               np)
    // out = (in * f0.transpose()) * u1;

    // tmp = in * F0
    for (c = 0; c < num_comp; c++)
    {
        for (d = 0; d < num_latent; d++)
        {
            double sum = .0;
            for (k = 0; k < num_feat; k++)
            {
                sum = sum + in[c][k] * F0[d][k];
            }

            tmp[c][d] = sum;
        }
    }
    // out = tmp * u1
    for (c = 0; c < num_comp; c++)
    {
        for (d = 0; d < num_proteins; d++)
        {
            double sum = .0;
            for (k = 0; k < num_latent; k++)
            {
                sum = sum + tmp[c][k] * U1[k][d];
            }

            out[c][d] = sum;
        }
    }
#if 0
    MapOut map_out(&out[0][0]);
    std::cout << __func__ << ":\n" <<  map_out << std::endl;
#endif
}
