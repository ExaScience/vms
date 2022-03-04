#pragma once

#include "counters.h"

#include "vms_const.h"

#define DT_NAME "float"

typedef float U_base;
typedef float M_base;
typedef float B_base;
typedef float F_base;
typedef float P_base;
typedef float L_base;
typedef float S_base;

typedef float U_type;
typedef float M_type;
typedef float B_type;
typedef float F_type;
typedef float P_type;
typedef float L_type;
typedef float S_type;


typedef F_base F_flx[][num_features];
typedef P_base P_flx[][num_proteins];

typedef U_base U_arr[num_samples][num_proteins][num_latent];
typedef M_base M_arr[num_samples][num_latent];
typedef B_base B_arr[num_samples][num_features][num_latent];

struct Model {
	U_arr U;
	M_arr M;
	B_arr B;
};

void predict_compounds(
		int start,
		int num_compounds, 
		const F_flx features,
		      P_flx predictions,
		const struct Model *m);

/* MPI functions and variables */
extern int mpi_world_size;
extern int mpi_world_rank;

void mpi_init(); 
void mpi_finit();
void barrier();

void send_inputs(int, F_flx);
void combine_results(int, P_flx);

void send_features(int compound, const F_base data[num_features]);
void send_predictions(int compound, const P_base data[num_proteins]);

/* GASPI functions */
enum {
	features_seg = 0,
	predictions_seg = 1, 
	model_seg = 2,
	errors_seg = 3
};

void *gaspi_malloc(int seg, unsigned long size);

/* OmpSS-2 */
void *lmalloc(unsigned long size, int seg);
void *dmalloc(unsigned long size, int seg);

#define UNUSED(x) (void)(x)
