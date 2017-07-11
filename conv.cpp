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

	//hls:stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > Layer2_tmp_out;

	Layer1 : conv_t<4,8,3,512,512>(x_in, x_out, t_in1, tload);
}

void pool(hls::stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > &x_in, hls::stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > &x_out)
{
	pool_t<8,2,512,512>(x_in, x_out);
}
