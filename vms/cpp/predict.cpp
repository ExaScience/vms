#include "predict.fpga.h"

extern "C"
void predict_one_block(
		const F_base *features,    //[block_size*num_features]
		      P_base *predictions, //[block_size*num_proteins*num_samples]
		const int model_nr,
		const U_base *u,           // [num_samples][num_proteins][num_latent];
		const M_base *m,	   // [num_samples][num_latent];
		const B_base *b            // [num_samples][num_features][num_latent]
		)
{
#pragma HLS INTERFACE m_axi port=features    offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=u           offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=m           offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=b           offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=predictions offset=slave bundle=gmem

	load_model(model_nr, u, m, b);
	predict_dataflow(features, predictions, model_cache);
} // end function
