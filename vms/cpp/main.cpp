
#include <cassert>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include <chrono>
#include <iostream>

#include <argp.h>

#include "predict.h"
#include "vms_tb.h"

#ifdef DT_OBSERVED_FLOAT
const char *typenames[] = {"U", "mu", "F", "P", "B", "S", "T"};
std::vector<float> values[ntypes];
#endif

double tick() {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    double ms = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    return ms / 1000.;
}

void prepare_tb_input(
    int num_compounds,
    const float in[tb_num_compounds][num_features],
    F_base out[][num_features])
{
    for (int c = 0; c < num_compounds; c++)
        for (int p = 0; p < num_features; p++)
            out[c][p] = F_base(F_type(in [c%tb_num_compounds][p]));
}

void prepare_model(
    const float U_in[num_samples][num_proteins][num_latent],
    const float M_in[num_samples][num_latent],
    const float B_in[num_samples][num_features][num_latent],
    U_base U_out[num_samples][num_proteins][num_latent],
    M_base M_out[num_samples][num_latent],
    B_base B_out[num_samples][num_features][num_latent]
    )
{
    for (int i = 0; i < num_samples; i++)
    {
        for (int j = 0; j < num_proteins; j++)
            for (int k = 0; k < num_latent; k++)
            {
                U_out[i][j][k] = U_base(U_type(U_in[i][j][k]));
            }

        for (int j = 0; j < num_latent; j++)
        {
            M_out[i][j] = M_base(M_type(M_in[i][j]));
        }

        for (int j = 0; j < num_features; j++)
            for (int k = 0; k < num_latent; k++)
            {
                B_out[i][j][k] = B_base(B_type(B_in[i][j][k]));
            }
    }
}


int check_result(
    int num_compounds,
    const P_vec out[][num_proteins],
    const float ref[tb_num_compounds][num_proteins])
{
    int nerrors = 0;
    for (int c = 0; c < num_compounds; c++)
        for (int p = 0; p < num_proteins; p++)
        {

            // compute average predictions accross samples
            S_type sum = S_type();
            for (int s = 0; s < num_samples; s++) sum = sum + P_type(out[c][p][s]);
            float o = (float)sum / (float)num_samples;

            float r = ref[c % tb_num_compounds][p];
            if (std::abs(o - r) < epsilon)
            {
                ; //printf("ok at [%d][%d]: %f == %f\n", c, p, o, r);
            }
            else
            {
                printf("error at [%d][%d]: %f != %f\n", c, p, o, r);
                nerrors++;
            }
        }

    printf("%d errors (out of %d)\n", nerrors, num_compounds * num_proteins);
    return nerrors;
}

/* Program documentation. */
static char doc[] = "Virtual Molecule Screening.";

/* A description of the arguments we accept. */
static char args_doc[] = "";

/* The options we understand. */
static struct argp_option options[] = {
  {"num-compounds",  'c', "NUM",      0,  "Number of compounds (10)." },
  {"num-repeat",     'r', "NUM",      0,  "Number of time to repeat (1)." },
  {"no-check",       'n',     0,      0,  "Do not check for errors." },
  {"check",          'd',     0,      0,  "Do check for errors." },
  { 0 }
};

/* Used by main to communicate with parse_opt. */
struct arguments
{
  int num_repeat = 2;
  int num_compounds = 100;
  bool check = true;
};

/* Parse a single option. */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  struct arguments *arguments = (struct arguments *)(state->input);

  switch (key)
    {
    case 'r': arguments->num_repeat = std::atoi(arg); break;
    case 'c': arguments->num_compounds = std::atoi(arg); break;
    case 'n': arguments->check = false; break;
    case 'd': arguments->check = true; break;
    default: return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };

int
main (int argc, char **argv)
{
    struct arguments args;

    /* Parse our arguments; every option seen by parse_opt will
     be reflected in arguments. */
    argp_parse(&argp, argc, argv, 0, 0, &args);

    // divide and round up
    int num_blocks = (args.num_compounds + block_size - 1) / block_size;
    int num_compounds_alloc = num_blocks * block_size;
    
    printf("  dt:    %s\n", DT_NAME);
    printf("  nprot: %d\n", num_proteins);
    printf("  nfeat: %d\n", num_features);
    printf("  nlat:  %d\n", num_latent);
    printf("  nsmpl: %d\n", num_samples);
    printf("  blks:  %d\n", block_size);
    printf("  nrep:  %d\n", args.num_repeat);
    printf("  ncmps: %d\n", args.num_compounds);
    printf("  alloc: %d\n", num_compounds_alloc);

    P_vec(*tb_output_base)[num_proteins];
    F_base(*tb_input_base)[num_features];
    U_base(*Ub)[num_proteins][num_latent];
    M_base(*Mb)[num_latent];
    B_base(*Bb)[num_features][num_latent];

    posix_memalign((void **)&tb_output_base, 4096, num_compounds_alloc * num_proteins * sizeof(P_vec));
    posix_memalign((void **)&tb_input_base, 4096, num_compounds_alloc * num_features * sizeof(F_base));
    posix_memalign((void **)&Ub, 4096, num_samples * num_proteins * num_latent * sizeof(U_base));
    posix_memalign((void **)&Mb, 4096, num_samples * num_latent * sizeof(M_base));
    posix_memalign((void **)&Bb, 4096, num_samples * num_features * num_latent * sizeof(B_base));

    prepare_tb_input(args.num_compounds, tb_input, tb_input_base);
    prepare_model(U, M, B, Ub, Mb, Bb);

    int nerrors = 0;

    printf("Predicting\n");
    double start = tick();
    for (int n = 0; n < args.num_repeat; n++)
    {
        predict_compounds(args.num_compounds, tb_input_base, tb_output_base, Ub, Mb, Bb);
    }
    double stop = tick();
    if (args.check)
        nerrors += check_result(args.num_compounds, tb_output_base, tb_ref);
    else
        printf( "Error checking skipped\n");

    double elapsed = stop - start;
    double compounds_per_sec = (double)args.num_compounds * args.num_repeat / elapsed;

    printf( "took: %.2f sec; %.2f compounds/second\n", elapsed, compounds_per_sec);

    // terra-ops aka 10^12 ops
    double gops = (double)args.num_repeat * (double)num_samples * (double)args.num_compounds * (double)num_latent * (double)(num_features + num_proteins) / 1e9;
    double tops = gops / 1e3;

    printf( "%.2f tera-ops; %.2f tera-ops/second\n", tops, tops/elapsed);
    printf( "%.2f giga-ops; %.2f giga-ops/second\n", gops, gops/elapsed);

    int num_kernel_executions = ceil((args.num_compounds) / double(block_size)) * args.num_repeat;
    double kernel_ms = 1e3 * elapsed / num_kernel_executions;
    printf( "%d kernel executions of  %.2f ms each (avg)\n", num_kernel_executions, kernel_ms);

    double clock_ns = 2.0; // assuming a clock of 500Mhz
    int cycles_per_kernel_execution =  kernel_ms * 1e6 / clock_ns;
    int cycles_per_compound = cycles_per_kernel_execution / block_size;
    printf( "%d cycles per kernel executions\n", cycles_per_kernel_execution);
    printf( "%d cycles per compound\n", cycles_per_compound);
    
    free(tb_output_base);
    free(tb_input_base);
    free(Ub);
    free(Mb);
    free(Bb);

    return nerrors;
}