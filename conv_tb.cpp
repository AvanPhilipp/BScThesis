#include <hls_stream.h>
#include <ap_int.h>
#include <ap_axi_sdata.h>
#include "conv.h"

using namespace hls;

int main()
{
	stream<my_data_in_type> x_in;
	stream<my_data_out_type> x_out;
	stream<my_templ_type> t_in;
	my_data_type tmp_in[INLAYERS_T];
	my_data_in_type tmp_x_in;
	int i,j,k;
	int tload=1;
	my_data_type tmp;
	for(i=0;i<INLAYERS_T*OUTLAYERS_T*TSIZE_T*TSIZE_T+OUTLAYERS_T;i++)
	{
		t_in.write(i+1);vivado hls;
		//t_in.write(-i+1);
	}
	for(i=0,k=1;i<WIDTH_T*HEIGHT_T;i++)
	{
		for(j=0;j<INLAYERS_T;j++,k++)
		{
			tmp_in[j]=k;
			tmp_x_in.data((j+1)*sizeof(my_data_type)*8-1,j*sizeof(my_data_type)*8)=float2ap_uint(tmp_in[j]);
		}
		x_in.write(tmp_x_in);
	}
	tload=1;
	conv_alt(x_in, x_out, t_in, tload);
	//pool(x_in, x_out);
	for(i=0;i<(WIDTH_T-TSIZE_T+1)*(HEIGHT_T-TSIZE_T+1);i++)
	{
		tmp=x_out.read().data;
	}
	return 1;
}
