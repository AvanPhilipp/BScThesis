#include <hls_stream.h>
#include <ap_int.h>
#include <ap_axi_sdata.h>
#include "conv.h"

using namespace hls;

void conv(hls::stream<ap_axiu<4*sizeof(my_data_type)*8,1,1,1> > &x_in, hls::stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > &x_out, stream<my_templ_type> &t_in1, stream<my_templ_type> &t_in2, int tload)
{
	//typedef hls:stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > my_data_out_type_s_l;
	//my_data_out_type_s_l Layer1_tmp_out;

	hls:stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > Layer1_tmp_out,Layer2_tmp_out;
	//hls:stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > Layer2_tmp_out;

	Layer1 : conv_t<4,8,3,512,512>(x_in, Layer1_tmp_out, t_in1, tload);
	Layer2 : conv_t<8,8,3,512-4,512-4>(Layer1_tmp_out, Layer2_tmp_out, t_in2, tload);
	Layer3 : pool_t<8,2,512-4,512-4>(Layer2_tmp_out, x_out);
}


void conv_alt(hls::stream<ap_axiu<4*sizeof(my_data_type)*8,1,1,1> > &x_in, hls::stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > &x_out, stream<my_templ_type> &t_in1, int tload)
{
	//typedef hls:stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > my_data_out_type_s_l;
	//my_data_out_type_s_l Layer1_tmp_out;

	hls:stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > Layer1_tmp_out; //, Layer2_tmp_out;

	Layer1 : conv_t<4,8,3,512,512>(x_in, Layer1_tmp_out, t_in1, tload);
	Layer2 : pool_t<8,2,512-4,512-4>(Layer1_tmp_out, x_out);
}

void pool(hls::stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > &x_in, hls::stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > &x_out)
{
	pool_t<8,2,512,512>(x_in, x_out);
}

void network (hls::stream< my_data_type[1] > &input, hls::stream< my_data_type[8] > &output)
{
	hls::stream < my_data_type[8] > temp_1;
	hls::stream < my_templ_type > templ_1;
	bool t_load = false;

#pragma HLS stream variable=<my_data_type> depth=<8> dim=<1> off
	layer_1: convolution_template<28,28,1,3,8>(&input, &temp_1, &templ_1, t_load);

//	hls::stream< my_data_type > temp_2;
	hls::stream< my_templ_type > templ_2;
	layer_2: convolution_template<28,28,8,3,8>(&temp_1,&output,&templ_2, t_load);



//	my_data_type temp_2[28*28];
//	layer_2: pooling_template<28,28,1,2>(temp_1,temp_2);
//
//	my_data_type temp_3[28*28];
//	layer_3: relu_template<28,28>(temp_2,temp_3);
//
//	output_layer: fully_connected_template<28*28,10>(temp_3,&output);
}
