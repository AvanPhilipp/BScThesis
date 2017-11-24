#include <hls_stream.h>
#include <ap_int.h>
#include <ap_axi_sdata.h>

using namespace hls;

typedef short my_data_type;

typedef short my_templ_type;

struct My_Temlpate_Struct{
	my_templ_type weight;
	ap_uint<1> last;
};

template<int IN_WIDTH, int IN_HEIGHT, int IN_DEPTH, int OUT_SIZE> inline void fully_connected_template(
		hls::stream< ap_uint< IN_DEPTH*sizeof(my_data_type)*8> > &input,
		hls::stream< ap_uint< OUT_SIZE*sizeof(my_data_type)*8> > &output,
		hls::stream< My_Temlpate_Struct > &weight_in,
		hls::stream< My_Temlpate_Struct > &weight_out,
		int template_load)
{
	int IN_SIZE = IN_WIDTH*IN_HEIGHT;
	my_templ_type weights[IN_SIZE][IN_DEPTH][OUT_SIZE];
	my_templ_type bias[OUT_SIZE];
	my_data_type input_register[IN_DEPTH];
	ap_uint<IN_DEPTH*sizeof(my_data_type)*8> input_tmp;

	/*
	 * Súlyok betöltése.
	 */
	if(template_load == 1){

		for(int i =0;i<IN_SIZE;i++){
			for(int h=0; h<IN_DEPTH;h++){
				for(int o = 0; o<OUT_SIZE;o++){
					My_Temlpate_Struct temp = weight_in.read();
					if(temp.last == 0) weights[i][h][o]=temp.weight;
				}
			}
		}
		for(int o = 0; o<OUT_SIZE;o++){
			My_Temlpate_Struct temp = weight_in.read();
			if(temp.last == 0) bias[o] = temp.weight;
		}
		bool exit = false;
		while(!exit){
			if(weight_in.empty()) break;
			My_Temlpate_Struct temp = weight_in.read();
			weight_out.write(temp);
			if(temp.last == 1) exit = true;
		}
	}

	for (int i=0; i<IN_SIZE-2; i++) {
		my_data_type temp_out;
		input_tmp = input.read();
		for (int h = 0; h < IN_DEPTH; h++) {
			input_register[h] = input_tmp((h+1)*sizeof(my_data_type)*8-1,h*sizeof(my_data_type)*8);
			for(int o=0;o<OUT_SIZE;o++){
				if (i == 0 && o == 0) {
					temp_out = bias[o] + input_register[h] * weights[i][h][o];
				} else {
					temp_out += input_register[h] * weights[i][h][o];
				}
				if(i == IN_SIZE-3 && h==IN_DEPTH-1){
					output.write(temp_out);
				}
			}
		}
	}
}

template<int SIZE> inline void max(
	hls::stream< ap_uint< SIZE*sizeof(my_data_type)*8>  > &input,
	int &output){

	int max_index = -1;
	my_data_type max = -10000;

	for(int i=0;i<SIZE;i++){
		my_data_type temp = input.read();

//		printf("Max running: %d,\tdata: %d\n", i, temp);
		if(temp > max){
			max_index = i;
			max = temp;
		}
	}
	output = max_index;
}
