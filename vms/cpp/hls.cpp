#define HOSTIF_HLS

#include "predict.fpga.h"

extern "C" 
void predict_one_block(
		int num_compounds,
		const F_blk features,
		      P_blk predictions,
		const Model &model
		)
{
#pragma HLS INTERFACE m_axi  port=features    
#pragma HLS INTERFACE m_axi  port=model           
#pragma HLS INTERFACE m_axi  port=predictions 

	load_model(model.nr, &model.U[0][0][0], &model.M[0][0], &model.B[0][0][0]);
	predict_dataflow(num_compounds, (F_base *)features, (P_base*)predictions, model_cache);
} // end function

void predict_compounds(
		int num_compounds,
		const F_blks features,    //[block_size][num_features]
		      P_blks predictions, //[block_size][num_proteins][num_samples]
		const Model &model)
{
    static int model_counter = 0;

    for(int i=0, j=0; i<num_compounds; j++,i+=block_size)
    {
        int left = std::min(block_size, num_compounds-i);
        const F_blk &f = features[i];
	      P_blk &p = predictions[i];
        predict_one_block(left, f, p, model);
    }

    model_counter++;
}

