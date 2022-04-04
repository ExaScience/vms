
#include <stdio.h>
#include <stdint.h>
#include <MaxSLiCInterface.h>	// Simple Live CPU interface

#include "predict.h"

static int const num_features = 8;
//static int const num_latent = 3;
static int const num_proteins = 8;
static int const num_compounds = 2;
//static int const num_bits = 16;



int main()
{

	uint16_t features[num_compounds][num_features];
	uint16_t predictions[num_compounds][num_proteins];

	printf("Filling features with junk...\n");
	for(int d=0; d<num_compounds; ++d)
	{
		for(int f=0; f<num_features; ++f)
		{
			features[d][f] = f+d;
		}
	}

	printf("Input:\n");
	for(int d=0; d<num_compounds; ++d)
	{
		printf("features[%d][] = {", d);
		for(int f=0; f<num_features; ++f)
		{
			printf("%d, ", features[d][f]);
		}
		printf("},\n");
	}

    if(predict_has_errors()) {
		printf("DFE errors: %s\n",  predict_get_errors());
	}


    printf("Running DFE\n");
    predict(2, features, sizeof(features), predictions, sizeof(predictions));
    printf("Finished running\n");

    if(predict_has_errors()) {
		printf("DFE errors: %s\n",  predict_get_errors());
	}


	printf("Output:\n");
	for(int d=0; d<num_compounds; ++d)
	{
		printf("predictions[%d][] = {", d);
		for(int f=0; f<num_proteins; ++f)
		{
			printf("%d, ", predictions[d][f]);
		}
		printf("},\n");
	}

	return 0;
}
