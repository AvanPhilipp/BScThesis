#include <hls_stream.h>

using namespace hls;

typedef short my_data_type;

void mnistNet(
		hls::stream< ap_uint< sizeof(my_data_type)*8> > &input,
		int &output,
		stream<My_Temlpate_Struct> &templ,
		int tload);

void convol(
		hls::stream< ap_uint< sizeof(my_data_type)*8> > &input,
		int &output,
		stream<My_Temlpate_Struct> &templ,
		int tload);

void pooling(
		hls::stream< ap_uint< sizeof(my_data_type)*8> > &input,
		int &output,
		stream<My_Temlpate_Struct> &templ,
		int tload);

void conv_pool(
		hls::stream< ap_uint< sizeof(my_data_type)*8> > &input,
		int &output,
		stream<My_Temlpate_Struct> &templ,
		int tload);

//void VGGNet(
//		hls::stream< ap_uint< 1*sizeof(my_data_type)*8> > &input,
//		hls::stream< ap_uint< 1000*sizeof(my_data_type)*8> > &output,
//		stream<my_templ_type> &templ,
//		int tload);
