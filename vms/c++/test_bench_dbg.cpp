#include <cstdio>
#include <cmath>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <string>
#include <limits>


#include "predict.h"
#include "vms_tb.h"

#ifdef DT_OBSERVED_FLOAT
const char *typenames[] = {"U", "mu", "F", "P", "B", "S", "T"};
std::vector<float> values[ntypes];
#endif

double tick() {
    return (double)clock() / CLOCKS_PER_SEC;
}

template<typename T>
std::string to_binary(T x)
{
    std::string ret;

    for(int i=0;i<sizeof(T) * 8;++i)
    {
        if ((i%4) == 0 && i!=0) ret = '_' + ret;
        if (x & 1) ret = '1' + ret; //prepend!
        else       ret = '0' + ret; 
        x >>= 1;
    }

    return ret;
}

template<typename T>
void print_fxp(T x)
{
    printf("fxp<%2d,%2d>: %8.2f (%6d -- 0x%8x -- %20s)\n", T::iwl, T::shift, (float)x, x.val, x.val, to_binary(x.val).c_str());
}


void prepare_tb_input(
    int num_compounds,
    signed short f, 
    F_base F_out[][num_features])
{
    for (int c = 0; c < num_compounds; c++)
        for (int p = 0; p < num_features; p++)
        {
            if (p==1) {
                F_type val(f);
                printf("F[%d][%d]    = ", c, p);
                print_fxp(val);
                F_out[c][p] = F_base(val);
            }
            else 
            {
                F_out[c][p] = 0;
            }
        }
}

void prepare_model(
    int b,
    U_base U_out[num_samples][num_proteins][num_latent],
    M_base M_out[num_samples][num_latent],
    B_base B_out[num_samples][num_features][num_latent],
    P_base &U_check,
    P_base &M_check,
    P_base &B_check
    )
{
	CRC_INIT(U_check);
	CRC_INIT(M_check);
	CRC_INIT(B_check);

    for (int i = 0; i < num_samples; i++)
    {
        for (int j = 0; j < num_proteins; j++)
            for (int k = 0; k < num_latent; k++)
            {
                float val;
                //val = U[i][j][k];
                if (j==0 && k==2 && i<1) 
                {
                    val = 1.;
                    printf("U[%d][%d][%d] = ", i,j,k);
                    print_fxp(U_type(val));
                } else val = 0.;
                U_out[i][j][k] = U_base(U_type(val));
                CRC_ADD(U_check, U_out[i][j][k]);
            }

        for (int j = 0; j < num_latent; j++)
        {
            float val; //M[i][j];
            /*if (i<1 && j==2) 
            {
                val = 0.25;
                printf("M[%d][%d]    = ", i,j);
                print_fxp(M_type(val));
            }
            else */
                val = .0;
            M_out[i][j] =  M_base(M_type(val));
            CRC_ADD(M_check, M_out[i][j]);
        }

        for (int j = 0; j < num_features; j++)
            for (int k = 0; k < num_latent; k++)
            {
                B_base val;
                if (j == 2 && k == 2 && i<1) {
                    val = b; //B[0][1][2]; // B[i][j][k];
                    printf("B[%d][%d][%d] = ", i,j,k);
                    print_fxp(B_type(val));
                } else val = 0;
                B_out[i][j][k] = val;
                CRC_ADD(B_check, B_out[i][j][k]);
            }

    }
}


int check_result(
    int num_compounds,
    const P_base out[][num_proteins],
    const float ref[tb_num_compounds][num_proteins])
{
    int nerrors = 0;
    int np = 8;
    for (int c = 0; c < num_compounds; c++)
        for (int p = 0; p < np; p++)
        {
            float o = P_type(out[c][p]);
            float r = ref[c % tb_num_compounds][p];
            printf("at [%d][%d]: %.2f (%d)\n", c, p, o, out[c][p]);
        }

    printf("%d errors (out of %d)\n", nerrors, num_compounds * num_proteins);
    return nerrors;
}

int main(int argc, char *argv[])
{
    int num_repeat = 1;
    int num_compounds = 1;
    int b = -1;
    int f = -1;

    if (argc > 1 && std::atoi(argv[1]))
    {
        num_repeat = std::atoi(argv[1]);
    }

    if (argc > 2 && std::atoi(argv[2]))
    {
        b = std::atoi(argv[2]);
    }

    if (argc > 3 && std::atoi(argv[3]))
    {
        f = std::atoi(argv[3]);
    }
    
    printf("  dt:    %s\n", DT_NAME);
    printf("  nrep:  %d\n", num_repeat);
    printf("  nprot: %d\n", num_proteins);
    printf("  ncmpd: %d\n", num_compounds);
    printf("  blks:  %d\n", block_size);
    printf("  nfeat: %d\n", num_features);
    printf("  nlat:  %d\n", num_latent);
    printf("  nsmpl: %d\n", num_samples);

    P_base (*tb_output_base)[num_proteins] = new P_base[num_compounds][num_proteins];
    F_base (*tb_input_base)[num_features] = new F_base[num_compounds][num_features];

    U_base (*Ub)[num_proteins][num_latent] = new U_base[num_samples][num_proteins][num_latent];
    M_base (*Mb)[num_latent]               = new M_base[num_samples][num_latent];
    B_base (*Bb)[num_features][num_latent] = new B_base[num_samples][num_features][num_latent];

    P_base  U_check_tb, M_check_tb, B_check_tb;

    int nerrors = 0;
    for (int i=0; i<num_repeat; i++) {
        prepare_tb_input(num_compounds, f, tb_input_base);
        prepare_model(b, Ub, Mb, Bb, U_check_tb, M_check_tb, B_check_tb);

        printf("Updating model\n");
        update_model(Ub, Mb, Bb);

        printf("Predicting\n");
        double start = tick();
        predict_compounds(num_compounds, tb_input_base, tb_output_base);
#pragma omp taskwait
        double stop = tick();
        nerrors += check_result(num_compounds, tb_output_base, tb_ref);
        double elapsed = stop-start;
        printf("took %.2f sec; %.2f compounds/sec\n", elapsed, num_compounds * num_repeat / elapsed);
    }

    delete[] tb_output_base;
    delete[] tb_input_base;
    delete[] Ub;
    delete[] Mb;
    delete[] Bb;

    return nerrors;
}
