//#include <hls_stream.h>
//#include <ap_int.h>
//#include <ap_axi_sdata.h>
//#include "conv.h"
//
//using namespace hls;
//
//int main()
//{
//	stream<my_data_in_type> x_in;
//	stream<my_data_out_type> x_out;
//	stream<my_templ_type> t_in_1;
//	stream<my_templ_type> t_in_2;
//	my_data_type tmp_in[INLAYERS_T];
//	my_data_in_type tmp_x_in;
//	int i,j,k;
//	int tload=1;
//	my_data_type tmp;
//	for(i=0;i<INLAYERS_T*OUTLAYERS_T*TSIZE_T*TSIZE_T+OUTLAYERS_T;i++)
//	{
//		t_in_1.write(i+1);
//		t_in_2.write(i+1);
//		//t_in.write(-i+1);
//	}
//	for(i=0,k=1;i<WIDTH_T*HEIGHT_T;i++)
//	{
//		for(j=0;j<INLAYERS_T;j++,k++)
//		{
//			tmp_in[j]=k;
//			//tmp_x_in.data((j+1)*sizeof(my_data_type)*8-1,j*sizeof(my_data_type)*8)=float2ap_uint(tmp_in[j]);
//			tmp_x_in((j+1)*sizeof(my_data_type)*8-1,j*sizeof(my_data_type)*8)=float2ap_uint(tmp_in[j]);
//		}
//		x_in.write(tmp_x_in);
//	}
//	tload=1;
//	//conv_alt(x_in, x_out, t_in, tload);
//	//pool(x_in, x_out);
//	network(x_in,x_out,t_in_1,t_in_2,tload);
//	for(i=0;i<(WIDTH_T-TSIZE_T+1)*(HEIGHT_T-TSIZE_T+1);i++)
//	{
//		//tmp=x_out.read().data;
//		tmp=x_out.read();
//	}
//	return 1;
//}


#include <iostream>
#include "conv.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <fstream>

uint32_t reverseBits(uint32_t n) {
//        n = (n >> 1) & 0x55555555 | (n << 1) & 0xaaaaaaaa;
//        n = (n >> 2) & 0x33333333 | (n << 2) & 0xcccccccc;
//        n = (n >> 4) & 0x0f0f0f0f | (n << 4) & 0xf0f0f0f0;
        n = (n >> 8) & 0x00ff00ff | (n << 8) & 0xff00ff00;
        n = (n >> 16) & 0x0000ffff | (n << 16) & 0xffff0000;
        return n;
}

int main(){


	std::ifstream in_image("/home/kotfu/Downloads/t10k-images.idx3-ubyte");
	std::ifstream in_weights("/.almafa");

	bool t_load;
//	hls::stream<my_templ_type> templ;
//	hls::stream<ap_uint< 1*sizeof(my_data_type)*8>> input;
//	hls::stream<ap_uint< 10*sizeof(my_data_type)*8>> output;

	if(in_weights.is_open()){

	}

	if(in_image.is_open()){

		unsigned char value;
		uint32_t magic;
		uint32_t counter;
		uint32_t width;
		uint32_t height;

		in_image.read((char*)&magic,sizeof(uint32_t));
		magic = reverseBits(magic);
		in_image.read((char*)&counter,sizeof(uint32_t));
		counter = reverseBits(counter);
		in_image.read((char*)&height,sizeof(uint32_t));
		height = reverseBits(height);
		in_image.read((char*)&width,sizeof(uint32_t));
		width = reverseBits(width);

		for(int w=0; w<width;w++){
			for(int h=0; h<width;h++){
				in_image.read((char*)&value,sizeof(unsigned char));

//				input.write(value);

				printf("Readed value: %X\n",value);

			}
		}
	}

//	mnistNet(input,output,templ, t_load);
	return 0;
}
