#include <hls_stream.h>
#include <ap_int.h>
#include <ap_axi_sdata.h>
#include "conv.h"

using namespace hls;

void conv(hls::stream<ap_uint<4*sizeof(my_data_type)*8> > &x_in, hls::stream<ap_uint<8*sizeof(my_data_type)*8> > &x_out, stream<my_templ_type> &t_in1, stream<my_templ_type> &t_in2, int tload)
{
	//typedef hls:stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > my_data_out_type_s_l;
	//my_data_out_type_s_l Layer1_tmp_out;

	hls:stream<ap_uint<8*sizeof(my_data_type)*8> > Layer1_tmp_out,Layer2_tmp_out;
	//hls:stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > Layer2_tmp_out;

	Layer1 : conv_t<4,8,3,512,512>(x_in, Layer1_tmp_out, t_in1, tload);
	Layer2 : conv_t<8,8,3,512-4,512-4>(Layer1_tmp_out, Layer2_tmp_out, t_in2, tload);
	Layer3 : pool_t<8,2,512-4,512-4>(Layer2_tmp_out, x_out);
}
/*
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
*/
void conv_alt(hls::stream<ap_uint<4*sizeof(my_data_type)*8> > &x_in, hls::stream<ap_uint<8*sizeof(my_data_type)*8> > &x_out, stream<my_templ_type> &t_in1, int tload)
{
	//typedef hls:stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > my_data_out_type_s_l;
	//my_data_out_type_s_l Layer1_tmp_out;

	hls:stream<ap_uint<8*sizeof(my_data_type)*8> > Layer1_tmp_out; //, Layer2_tmp_out;

	Layer1 : conv_t<4,8,3,512,512>(x_in, Layer1_tmp_out, t_in1, tload);
	Layer2 : pool_t<8,2,512-4,512-4>(Layer1_tmp_out, x_out);
}
/*
void conv_alt(hls::stream<ap_axiu<4*sizeof(my_data_type)*8,1,1,1> > &x_in, hls::stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > &x_out, stream<my_templ_type> &t_in1, int tload)
{
	//typedef hls:stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > my_data_out_type_s_l;
	//my_data_out_type_s_l Layer1_tmp_out;

	hls:stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > Layer1_tmp_out; //, Layer2_tmp_out;

	Layer1 : conv_t<4,8,3,512,512>(x_in, Layer1_tmp_out, t_in1, tload);
	Layer2 : pool_t<8,2,512-4,512-4>(Layer1_tmp_out, x_out);
}
*/
/*
void pool(hls::stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > &x_in, hls::stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > &x_out)
{
	pool_t<8,2,512,512>(x_in, x_out);
}*/

//void VGGNet (
//		hls::stream< ap_uint< 1*sizeof(my_data_type)*8> > &input,
//		hls::stream< ap_uint< 1000*sizeof(my_data_type)*8> > &output,
//		stream<my_templ_type> &templ,
//		int tload)
//{
//	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > &conv3_64_1_temp;
//	conv3_64_1: convolution_template<224,224,1,3,64>(input,conv3_64_1_temp, templ,tload);
//	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > &conv3_64_2_temp;
//	conv3_64_1: convolution_template<224,224,64,3,64>(conv3_64_1_temp,conv3_64_2_temp, templ,tload);
//	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > &conv3_64_3_temp;
//	conv3_64_1: convolution_template<224,224,64,3,64>(conv3_64_2_temp,conv3_64_3_temp, templ,tload);
//	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > &maxpool_1_temp;
//	maxpool_1: pooling_template<224,224,64,3,64>(conv3_64_3_temp,maxpool_1_temp);
//
//
//	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > &conv3_128_1_temp;
//	conv3_128_1:  convolution_template<112,112,64,3,128>(maxpool_1_temp,conv3_128_1_temp,templ,tload);
//	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > &conv3_128_2_temp;
//	conv3_128_2: convolution_template<112,112,128,3,128>(conv3_128_1_temp,conv3_128_2_temp,templ,tload);
//	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > &maxpool_2_temp;
//	maxpool_2: pooling_template<112,112,128,3,128>(conv3_128_2_temp,maxpool_2_temp);
//
//
//	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > &conv3_256_1_temp;
//	conv3_256_1: convolution_template<56,56,128,3,256>(maxpool_2_temp,conv3_256_1_temp,templ,tload);
//	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > &conv3_256_2_temp;
//	conv3_256_2: convolution_template<56,56,256,3,256>(conv3_256_1_temp,conv3_256_2_temp,templ,tload);
//	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > &conv3_256_3_temp;
//	conv3_256_3: convolution_template<56,56,256,3,256>(conv3_256_2_temp,conv3_256_3_temp,templ,tload);
//	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > &maxpool_3_temp;
//	maxpool_3: pooling_template<56,56,256,3,256>(conv3_256_3_temp,maxpool_3_temp);
//
//
//	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > &conv3_512_11_temp;
//	conv3_512_1_1: convolution_template<28,28,256,3,512>(maxpool_3_temp,conv3_512_11_temp,templ,tload);
//	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > &conv3_512_12_temp;
//	conv3_512_1_2: convolution_template<28,28,512,3,512>(conv3_512_11_temp,conv3_512_12_temp,templ,tload);
//	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > &conv3_512_13_temp;
//	conv3_512_1_3: convolution_template<28,28,512,3,512>(conv3_512_12_temp,conv3_512_13_temp,templ,tload);
//	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > &maxpool_4_temp;
//	maxpool_4: pooling_template<28,28,512,3,512>(conv3_512_13_temp,maxpool_4_temp);
//
//
//	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > &conv3_512_21_temp;
//	conv3_512_2_1: convolution_template<14,14,512,3,512>(maxpool_4_temp,conv3_512_21_temp,templ,tload);
//	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > &conv3_512_22_temp;
//	conv3_512_2_2: convolution_template<14,14,512,3,512>(conv3_512_21_temp,conv3_512_22_temp,templ,tload);
//	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > &conv3_512_23_temp;
//	conv3_512_2_3: convolution_template<14,14,512,3,512>(conv3_512_22_temp,conv3_512_23_temp,templ,tload);
//	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > &maxpool_5_temp;
//	maxpool_5: pooling_template<14,14,512,3,512>(conv3_512_23_temp,maxpool_5_temp);
//
//
//	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > &fc_1_temp;
//	FC_1: fully_connected_template<7*7*512,4096>(maxpool_5_temp,fc_1_temp,templ,tload);
//	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > &fc_2_temp;
//	FC_2: fully_connected_template<4096,4096>(fc_1_temp,fc_2_temp,templ,tload);
//	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > &fc_3_temp;
//	FC_3: fully_connected_template<4096,1000>(fc_2_temp,fc_3_temp,templ,tload);
//	softmax: relu_template<1000>(fc_3_temp,output);
//}

void convol (
		hls::stream< ap_uint< sizeof(my_data_type)*8> > &input,
		int &output,
		hls::stream<My_Temlpate_Struct> &templ,
		int tload)
{

	const int image_size = 7;
	const int conv_size = 1;
	// 10^3=200, 10^2=100, 10^1=0, 5^3=50, 5^2=25, 5^1=0, 7^3=98, 7^2=49, 7^1=0, 28^2=784, 28^3=1568, 28^1=0
	// Template size valahol nem jó.
	// Holnap Template Size-t kell ellenőrizni.

	hls::stream< My_Temlpate_Struct > weight_conv1("conv3_1_forward");
	hls::stream< ap_uint< 32*sizeof(my_data_type)*8> > conv3_1_temp("conv3_1_output");
	conv3_1: convolution_template<image_size,image_size,1,conv_size,32>(input, conv3_1_temp,templ,weight_conv1,tload);

	hls::stream< My_Temlpate_Struct > weight_conv2("conv3_2_forward");
	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > conv3_2_temp("conv3_2_output");
	conv3_2: convolution_template<image_size,image_size,32,conv_size,64>(conv3_1_temp,conv3_2_temp,weight_conv1,weight_conv2,tload);

	hls::stream< My_Temlpate_Struct > weight_conv3("conv3_3_forward");
	hls::stream< ap_uint< 128*sizeof(my_data_type)*8> > conv3_3_temp("conv3_3_output");
	conv3_3: convolution_template<image_size,image_size,64,conv_size,128>(conv3_2_temp,conv3_3_temp,weight_conv2,weight_conv3,tload);




	while(!weight_conv3.empty()){
		weight_conv3.read();
	}
	for(int i=0;i<image_size*image_size;i++){
		conv3_3_temp.read();
	}

	while(!input.empty()){
		input.read();
	}

	printf("%d input left\n", input.size()); // Validated
	printf("%d conv3_1_temp left\n", conv3_1_temp.size()); //1568
	printf("%d conv3_2_temp left\n", conv3_2_temp.size()); //1568
	printf("%d conv3_3_temp left\n", conv3_3_temp.size()); //1568
	// feature-k nem játszanak közre.
}

void pooling(hls::stream< ap_uint< sizeof(my_data_type)*8> > &input,
		int &output,
		hls::stream<My_Temlpate_Struct> &templ,
		int tload){

	hls::stream< ap_uint< sizeof(my_data_type)*8> > maxpool_1_temp("maxpool_1_output");
	maxpool_1: pooling_template<28,28,1,2>(input,maxpool_1_temp);

	hls::stream< ap_uint< sizeof(my_data_type)*8> > maxpool_2_temp("maxpool_2_output");
	maxpool_2: pooling_template<14,14,1,2>(maxpool_1_temp,maxpool_2_temp);


	hls::stream< ap_uint< sizeof(my_data_type)*8> > maxpool_3_temp("maxpool_3_output");
	maxpool_3: pooling_template<7,7,1,2>(maxpool_2_temp,maxpool_3_temp);

	for(int i=0;i<4*4;i++){
		maxpool_3_temp.read();
	}


	printf("%d input left\n", input.size()); // Validated
	printf("%d maxpool_1_temp left\n", maxpool_1_temp.size()); //196 14*14
	printf("%d maxpool_2_temp left\n", maxpool_2_temp.size()); //49 7*7
	printf("%d maxpool_3_temp left\n", maxpool_3_temp.size()); //5 (??)
	// Valószínűleg a stride problémás.
	// Holnap meg kell nézni a memóriacímzéseket.
}

void mnistNet (
		hls::stream< ap_uint< sizeof(my_data_type)*8> > &input,
		int &output,
		hls::stream<My_Temlpate_Struct> &templ,
		int tload)
{
	hls::stream< My_Temlpate_Struct > weight_conv1("conv3_1_forward");
	hls::stream< ap_uint< 32*sizeof(my_data_type)*8> > conv3_1_temp("conv3_1_output");
	conv3_1: convolution_template<28,28,1,3,32>(input, conv3_1_temp,templ,weight_conv1,tload);

	hls::stream< ap_uint< 32*sizeof(my_data_type)*8> > maxpool_1_temp("maxpool_1_output");
	maxpool_1: pooling_template<28,28,32,2>(conv3_1_temp,maxpool_1_temp);


	hls::stream< My_Temlpate_Struct > weight_conv2("conv3_2_forward");
	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > conv3_2_temp("conv3_2_output");
	conv3_2: convolution_template<14,14,32,3,64>(maxpool_1_temp,conv3_2_temp,weight_conv1,weight_conv2,tload);

	hls::stream< ap_uint< 64*sizeof(my_data_type)*8> > maxpool_2_temp("maxpool_2_output");
	maxpool_2: pooling_template<14,14,64,2>(conv3_2_temp,maxpool_2_temp);



	hls::stream< My_Temlpate_Struct > weight_conv3("conv3_3_forward");
	hls::stream< ap_uint< 128*sizeof(my_data_type)*8> > conv3_3_temp("conv3_3_output");
	conv3_3: convolution_template<7,7,64,3,128>(maxpool_2_temp,conv3_3_temp,weight_conv2,weight_conv3,tload);

	hls::stream< ap_uint< 128*sizeof(my_data_type)*8> > maxpool_3_temp("maxpool_3_output");
	maxpool_3: pooling_template<7,7,128,2>(conv3_3_temp,maxpool_3_temp);



	hls::stream< My_Temlpate_Struct > weight_fc("FC_1_forward");
	hls::stream< ap_uint< 10*sizeof(my_data_type)*8> > fc_1_temp("FC_1_output");
	FC_1: fully_connected_template<4,4,128,10>(maxpool_3_temp,fc_1_temp,weight_conv3,weight_fc,tload);

	softmax: max<10>(fc_1_temp,output);
	weight_fc.read();

	printf("%d input left\n", input.size()); // Validated
	printf("%d conv3_1_temp left\n", conv3_1_temp.size()); //1568/32/28 = 1.75 sor marad ? vagy 1568/28/28 = 2 feature?
	printf("%d conv3_2_temp left\n", conv3_2_temp.size()); //392/64 = 6.125 pixel marad ? vagy 392/14/14 = 2 feature?
	printf("%d conv3_3_temp left\n", conv3_3_temp.size()); //98/128 = 0.765625 pixel marad ? vagy 98/7/7 = 2 feature?
	printf("%d maxpool_1_temp left\n", maxpool_1_temp.size()); //12348/28/28 = 15.75 ? 12348/14/14 = 63 (2*32)
	printf("%d maxpool_2_temp left\n", maxpool_2_temp.size()); //6223/14/14 = 31.75 ? 6223/7/7 = 127 (2*64)
	printf("%d maxpool_3_temp left\n", maxpool_3_temp.size()); //2976/7/7 = 60.7346938776 ? 2976/4/4 = 186 (???)
	printf("%d fc_1_temp left\n", fc_1_temp.size()); //Validated
	printf("%d weight left\n", weight_fc.size()); //Validated
}

