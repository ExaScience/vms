#include "predict.h"
#include <MaxSLiCInterface.h>	// Simple Live CPU interface

float dataIn[8] = { 1, 0, 2, 0, 4, 1, 8, 3 };
float dataOut[8];

/*
void predict(
        uint64_t ticks_predictKernel,
        const void *instream_features,
        size_t instream_size_features,
        void *outstream_predictions,
        size_t outstream_size_predictions);
*/ 

int main()
{
	printf("Running DFE\n");
	predict(8, dataIn, sizeof(float) * 8, dataOut, sizeof(float) * 8);

	for (int i = 1; i < 7; i++) // Ignore edge values
		printf("dataOut[%d] = %f\n", i, dataOut[i]);

	return 0;
}
