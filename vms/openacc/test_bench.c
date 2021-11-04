#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <argp.h>

#include "predict.h"
#include "vms_tb.h"

// call this function to end a timer, returning nanoseconds elapsed as a long
double tick() {
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (double)(t.tv_sec) +  (double)(t.tv_nsec) / 1e9; 
}

void prepare_tb_input(
    int num_blocks,
    const float in[][num_features],
    float out[][block_size][num_features])
{
    for (int b = 0; b < num_blocks; b++)
        for (int c = 0; c < block_size; c++)
            for (int p = 0; p < num_features; p++)
                out[b][c][p] = in[c % tb_num_compounds][p];
}


int check_result(
    int num_blocks,
    const float out[][block_size][num_proteins],
    const float ref[tb_num_compounds][num_proteins])
{
    int nerrors = 0;
    for (int b = 0; b < num_blocks; b++)
        for (int c = 0; c < block_size; c++)
            for (int p = 0; p < num_proteins; p++)
            {
                float o = out[b][c][p];
                float r = ref[c % tb_num_compounds][p];
                if (fabs(o - r) < epsilon)
                {
                    ; //printf("ok at [%d][%d]: %f == %f\n", c, p, o, r);
                }
                else
                {
                    if (nerrors < 10)
                        printf("error at [%d][%d]: %f != %f\n", c, p, o, r);
                    else if (nerrors == 10)
                        printf("More than 10 errors. Printing stopped.\n");
                    nerrors++;
                }
            }

    int num_compounds = num_blocks*block_size;
    printf("%d errors (out of %d)\n", nerrors, num_compounds * num_proteins);
    return nerrors;
}


/* Program documentation. */
static char doc[] = "Virtual Molecule Screening.";

/* A description of the arguments we accept. */
static char args_doc[] = "";

/* The options we understand. */
static struct argp_option options[] = {
  {"num-blocks",     'b', "NUM",      0,  "Number of blocks (1)." },
  {"num-repeat",     'r', "NUM",      0,  "Number of time to repeat (1)." },
  {"no-check",       'n',     0,      0,  "Do not check for errors." },
  {"check",          'd',     0,      0,  "Do check for errors." },
  {"verbose",        'v',     0,      0,  "Verbose messages" },
  { 0 }
};

static int verbose = 0;

/* Used by main to communicate with parse_opt. */
struct arguments
{
  int num_repeat;
  int num_blocks;
  int check;
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
    case 'r': arguments->num_repeat = atoi(arg); break;
    case 'b': arguments->num_blocks = atoi(arg); break;
    case 'n': arguments->check = 0; break;
    case 'd': arguments->check = 1; break;
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
    args.num_repeat = 1;
    args.num_blocks = 1;
    args.check = 1;

    /* Parse our arguments; every option seen by parse_opt will
     be reflected in arguments. */
    argp_parse(&argp, argc, argv, 0, 0, &args);

    printf("  nrep:  %d\n", args.num_repeat);
    printf("  nblk:  %d\n", args.num_blocks);
    printf("  blksz: %d\n", block_size);
    printf("  nprot: %d\n", num_proteins);
    printf("  nfeat: %d\n", num_features);
    printf("  nlat:  %d\n", num_latent);
    printf("  nsmpl: %d\n", num_samples);

    float (*tb_output_blocks)[block_size][num_proteins] = (float *) malloc(args.num_blocks*block_size*num_proteins*sizeof(float));
    float (*tb_input_blocks)[block_size][num_features] = (float *)malloc(args.num_blocks*block_size*num_features*sizeof(float));

    prepare_tb_input(args.num_blocks, tb_input, tb_input_blocks);

    int nerrors = 0;

    printf("Predicting\n");
    double elapsed = 1e10;
    for (int i = 0; i< args.num_repeat; ++i)
    {
        double start = tick();
        predict_blocks(args.num_blocks, tb_input_blocks, tb_output_blocks, U, M, B);
        double stop = tick();
	if (stop-start < elapsed) elapsed = stop-start;
        printf("%d: took %.2f sec; %.2f compounds/sec\n", i, stop-start, block_size * args.num_blocks / elapsed);
    }

    if (args.check)
        nerrors += check_result(args.num_blocks, tb_output_blocks, tb_ref);

    printf("minimum time %.2f sec; %.2f compounds/sec\n", elapsed, block_size * args.num_blocks / elapsed);

    // terra-ops aka 10^12 ops
    double tops = (double)args.num_blocks * (double)num_samples * (double)block_size * (double)num_latent * (double)(num_features + num_proteins) / 1e12;
    const int op_size = sizeof(float) * 8;

    printf("%.4f tera-ops; %.4f tera-ops/second (%d-bit float ops)\n", tops, tops/elapsed, op_size);
    printf("%.4f giga-ops; %.4f giga-ops/second (%d-bit float ops)\n", 1e3 * tops, 1e3 * tops/elapsed, op_size);

    free(tb_output_blocks);
    free(tb_input_blocks);

    return nerrors;
}
