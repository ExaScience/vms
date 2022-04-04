
#include <sys/time.h>

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

#ifdef VMS_DT_OBSERVED_FLOAT
const char *typenames[] = {"U", "mu", "F", "P", "B", "S", "T"};
std::vector<float> values[ntypes];
#endif

#ifdef VMS_DATAFLOW
#define VMS_DATAFLOW_STR "on"
#else
#define VMS_DATAFLOW_STR "off"
#endif


int verbose = 0;

double tick() {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    double ms = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    return ms / 1000.;
}

void prepare_tb_input(
    int num_compounds,
    const float in[tb_num_compounds][num_features],
    F_base out[][block_size][num_features])
{
    for (int c = 0; c < num_compounds; c++)
        for (int p = 0; p < num_features; p++)
            out[c/block_size][c%block_size][p] = F_base(F_type(in [c%tb_num_compounds][p]));
}

void prepare_tb_output(
    int num_compounds,
    P_base out[][block_size][num_proteins][num_samples])
{
    for (int c = 0; c < num_compounds; c++)
        for (int p = 0; p < num_proteins; p++)
            for (int s = 0; s < num_samples; s++)
                out[c/block_size][c%block_size][p][s] = 0;
}

Model prepare_model(
    const float U_in[num_samples][num_proteins][num_latent],
    const float M_in[num_samples][num_latent],
    const float B_in[num_samples][num_features][num_latent]
    )
{
    static int model_counter = 1;

    Model m;
    m.nr = model_counter++;

    for (int i = 0; i < num_samples; i++)
    {
        for (int j = 0; j < num_proteins; j++)
            for (int k = 0; k < num_latent; k++)
            {
                m.U[i][j][k] = U_base(U_type(U_in[i][j][k]));
            }

        for (int j = 0; j < num_latent; j++)
        {
            m.M[i][j] = M_base(M_type(M_in[i][j]));
        }

        for (int j = 0; j < num_features; j++)
            for (int k = 0; k < num_latent; k++)
            {
                m.B[i][j][k] = B_base(B_type(B_in[i][j][k]));
            }
    }

    return m;
}


int check_result(
    int num_compounds,
    const P_base out[][block_size][num_proteins][num_samples],
    const float ref[tb_num_compounds][num_proteins])
{
    int nerrors = 0;
    for (int c = 0; c < num_compounds; c++)
        for (int p = 0; p < num_proteins; p++)
        {

            // compute average predictions accross samples
            S_type sum = S_type();
            for (int s = 0; s < num_samples; s++) sum = sum + P_type(out[c/block_size][c%block_size][p][s]);
            float o = (float)sum / (float)num_samples;

            float r = ref[c % tb_num_compounds][p];
            if (std::abs(o - r) < epsilon)
            {
                ; //printf("ok at [%d][%d]: %f == %f\n", c, p, o, r);
            }
            else
            {
                if (nerrors <= 10) printf("error at [%d][%d]: %f != %f\n", c, p, o, r);
                if (nerrors == 10) printf("too many errors - stopping printing\n");
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
  {"num-blocks", 'b', "NUM",      0,  "Number of blocks (2)." },
  {"num-repeat", 'r', "NUM",      0,  "Number of time to repeat (1)." },
  {"no-check",   'n',     0,      0,  "Do not check for errors." },
  {"check",      'd',     0,      0,  "Do check for errors." },
  {"verbose",    'v',     0,      0,  "Verbose messages" },
  { 0 }
};

/* Used by main to communicate with parse_opt. */
struct arguments
{
  int num_repeat = 2;
  int num_blocks = 2;
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
    case 'b': arguments->num_blocks = std::atoi(arg); break;
    case 'n': arguments->check = false; break;
    case 'd': arguments->check = true; break;
    case 'v': verbose++; break;
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

    int num_compounds = args.num_blocks * block_size;
    int mpi_world_size, mpi_world_rank;

    mpi_init(mpi_world_size, mpi_world_rank);

    assert(mpi_world_size > 0);
    assert(args.num_blocks % mpi_world_size == 0);

    printf("  dt:    %s\n", VMS_DT_NAME);
    printf("  filt:  %s\n", VMS_FILTER_PRAGMAS);
    printf("  df:    %s\n", VMS_DATAFLOW_STR);
    printf("  nprot: %d\n", num_proteins);
    printf("  nfeat: %d\n", num_features);
    printf("  nlat:  %d\n", num_latent);
    printf("  nsmpl: %d\n", num_samples);
    printf("  blks:  %d\n", block_size);
    printf("  nblks: %d\n", args.num_blocks);
    printf("  flen:  %d\n", F_vec_len);
    printf("  nrep:  %d\n", args.num_repeat);
    printf("  ncmps: %d\n", num_compounds);
    printf("  nproc: %d\n", mpi_world_size);
    printf("  proc:  %d\n", mpi_world_rank);

    P_base(*tb_output_base)[block_size][num_proteins][num_samples];
    F_base(*tb_input_base)[block_size][num_features];

    posix_memalign((void **)&tb_output_base, 4096, num_compounds * num_proteins * num_samples * sizeof(P_base));
    posix_memalign((void **)&tb_input_base, 4096, num_compounds * num_features * sizeof(F_base));

    prepare_tb_input(num_compounds, tb_input, tb_input_base);
    prepare_tb_output(num_compounds, tb_output_base);
    Model m = prepare_model(U, M, B);

    // determine what blocks this rank should process, update pointer to input and output
    int blocks_per_rank = args.num_blocks / mpi_world_size;
    auto tb_input = &tb_input_base[mpi_world_rank * blocks_per_rank];
    auto tb_output = &tb_output_base[mpi_world_rank * blocks_per_rank];

    printf("Predicting\n");
    int nerrors = 0;
    double start = tick();
    for (int n = 0; n < args.num_repeat; n++)
    {
        printf(" Repeat %d/%d\n", n, args.num_repeat);
        predict_compounds(blocks_per_rank, tb_input, tb_output, m);
    }

    mpi_combine_results(args.num_blocks, tb_output_base);

    double stop = tick();
    if (args.check && mpi_world_rank == 0)
        nerrors += check_result(num_compounds, tb_output_base, tb_ref);
    else
        printf( "Error checking skipped\n");

    double elapsed = stop - start;
    double compounds_per_sec = (double)num_compounds * args.num_repeat / elapsed;

    printf( "took: %.2f sec; %.2f compounds/second\n", elapsed, compounds_per_sec);

    // terra-ops aka 10^12 ops
    double gops = (double)args.num_repeat * (double)num_samples * (double)num_compounds * (double)num_latent * (double)(num_features + num_proteins) / 1e9;
    double tops = gops / 1e3;

    printf( "%.2f tera-ops; %.2f tera-ops/second\n", tops, tops/elapsed);
    printf( "%.2f giga-ops; %.2f giga-ops/second\n", gops, gops/elapsed);

    int num_kernel_executions = double(args.num_blocks) * args.num_repeat;
    double kernel_ms = 1e3 * elapsed / num_kernel_executions;
    printf( "%d kernel executions of  %.2f ms each (avg)\n", num_kernel_executions, kernel_ms);

    double clock_ns = 2.0; // assuming a clock of 500Mhz
    int cycles_per_kernel_execution =  kernel_ms * 1e6 / clock_ns;
    int cycles_per_compound = cycles_per_kernel_execution / block_size;
    printf( "%d cycles per kernel executions\n", cycles_per_kernel_execution);
    printf( "%d cycles per compound\n", cycles_per_compound);
    
    free(tb_output_base);
    free(tb_input_base);

    mpi_finit();

    return nerrors;
}