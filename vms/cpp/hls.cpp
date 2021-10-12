#define HOSTIF_HLS

#include "predict.fpga.h"

extern "C" 
void predict_one_block(
		int num_compounds,
		const uint8_t *features,    //[block_size*num_features]
		      uint8_t *predictions, //[block_size*num_proteins*num_samples]
		const int model_nr,
		const uint8_t *u,           //[num_samples][num_proteins][num_latent];
		const uint8_t *m,	    //[num_amples][num_latent];
		const uint8_t *b            // [num_samples][num_features][num_latent]
		)
{
	const int f_depth = sizeof(F_base)*block_size*num_features;
	const int u_depth = sizeof(U_base)*num_samples*num_latent*num_proteins;
	const int m_depth = sizeof(M_base)*num_samples*num_latent;
	const int b_depth = sizeof(B_base)*num_samples*num_latent*num_features;
	const int p_depth = sizeof(P_base)*block_size*num_proteins;

#pragma HLS INTERFACE m_axi depth=f_depth bundle=bundle_features    port=features    
#pragma HLS INTERFACE m_axi depth=u_depth bundle=bundle_u           port=u           
#pragma HLS INTERFACE m_axi depth=m_depth bundle=bundle_m           port=m           
#pragma HLS INTERFACE m_axi depth=b_depth bundle=bundle_b           port=b           
#pragma HLS INTERFACE m_axi depth=p_depth bundle=bundle_predictions port=predictions 

	load_model(model_nr, (U_base *)u, (M_base *)m, (B_base *)b);
	predict_dataflow(num_compounds, (F_base *)features, (P_base*)predictions, model_cache);
} // end function

void predict_compounds(
		int num_compounds,
		const F_flx features,    //[][num_features]
		      P_flx predictions, //[][num_proteins][num_samples]
		const Model &model)
{
    static int model_counter = 0;

    const uint8_t *u = (uint8_t *)(&model.U[0][0][0]);
    const uint8_t *m = (uint8_t *)(&model.M[0][0]);
    const uint8_t *b = (uint8_t *)(&model.B[0][0][0]);

    for(int i=0; i<num_compounds; i+=block_size)
    {
        int left = std::min(block_size, num_compounds-i);
        const uint8_t *f = (uint8_t *)(&features[i][0]);
              uint8_t *p = (uint8_t *)(&predictions[i][0]); 
        predict_one_block(left, f, p, model_counter, u, m, b);
    }

    model_counter++;
}

