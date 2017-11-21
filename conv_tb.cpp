//#include <hls_stream.h>
//#include <ap_int.h>
//#include <ap_axi_sdata.h>
//#include "conv.h"

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
#include <sstream>
#include <string>
#include <stdexcept>
//#include <typeinfo>

using namespace hls;

int reverseBits(int n) {
//        n = (n >> 1) & 0x55555555 | (n << 1) & 0xaaaaaaaa;
//        n = (n >> 2) & 0x33333333 | (n << 2) & 0xcccccccc;
//        n = (n >> 4) & 0x0f0f0f0f | (n << 4) & 0xf0f0f0f0;
        n = (n >> 8) & 0x00ff00ff | (n << 8) & 0xff00ff00;
        n = (n >> 16) & 0x0000ffff | (n << 16) & 0xffff0000;
        return n;
}

int main(){

	hls::stream< ap_uint<sizeof(my_data_type)*8> > input;
	hls::stream< ap_uint<sizeof(my_data_type)*8> > output;
	hls::stream<my_templ_type> templ;

	const char *image_path = "/home/kotfu/BSCThesis/data/t10k-images.idx3-ubyte";
	const char *label_path = "/home/kotfu/BSCThesis/data/t10k-labels.idx1-ubyte";
	const char *weight_path = "/home/kotfu/BSCThesis/Weights/weight_array.wgt";

	printf("Reading weights from %s\n",weight_path);
	std::ifstream in_weights(weight_path);
	printf("Weights file %s\n", in_weights.is_open() ? "is opened" : "failed to open");

	bool t_load = true;
	for(std::string line; std::getline(in_weights, line); ){
		std::istringstream in(line);

		float weight;
		in >> weight;

		templ.write(weight);
//		printf("type: %s, value: %f\n", typeid(weight).name(),weight);

	}
	printf("Weights loaded\n");



	printf("Reading image from %s\n", image_path);
	std::ifstream in_image(image_path);
	printf("Image file %s\n",in_image.is_open() ? " is opened" : "failed to open");

	t_load = false;
	if(in_image.is_open()){

		unsigned char value;
		int magic;
		int counter;
		int width;
		int height;

		in_image.read((char*)&magic,sizeof(magic));
		magic = reverseBits(magic);
//		printf("Magic: %d\n",magic);
		if(magic != 2051) throw std::runtime_error("Invalid MNIST image file!");
		in_image.read((char*)&counter,sizeof(counter));
		counter = reverseBits(counter);
		in_image.read((char*)&height,sizeof(height));
		height = reverseBits(height);
		in_image.read((char*)&width,sizeof(width));
		width = reverseBits(width);

		for(int w=0; w<width;w++){
			for(int h=0; h<8;h++){

				in_image.read((char*)&value,sizeof(unsigned char));

				input.write(value);

//				printf("Readed value: %X\n",value);

			}
		}
	}

	printf("Reading labels from %s\n",label_path);
	std::ifstream in_labels(label_path);
	printf("Label file %s\n",in_labels.is_open() ? " is opened" : "failed to open");

	unsigned int label;
	if(in_labels.is_open()){

		int magic;
		int counter;

		in_labels.read((char*)&magic,sizeof(magic));
		magic = reverseBits(magic);
		if(magic != 2049) throw std::runtime_error("Invalid MNIST label file!");
		in_labels.read((char*)&counter,sizeof(counter));
		counter = reverseBits(counter);

//		for(int w=0; w<10;w++){
//		in_labels.read((char*)&value,sizeof(unsigned char));
			in_labels >> label;
//			printf("Readed value %d\n", label);
//		}
	}

	mnistNet(input,output,templ, t_load);
	int out = output.read();
	printf("Predicted: %d\tActual: %d\n", out,label);

	return 0;
}
