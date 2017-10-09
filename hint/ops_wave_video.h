#ifndef OPS_WAVE_H
#define OPS_WAVE_H
#include <hls_stream.h>

typedef float my_base_type;
struct my_data_type {
	my_base_type u, v;
};
typedef float my_const_base_type;
struct my_const_type {
	my_const_base_type c1;
};
//#define WIDTH 16
//#define HEIGHT 16
//#define WIDTH 512
//#define HEIGHT 512
//#define WIDTH 1280
//#define HEIGHT 1024
#define WIDTH 2048
#define HEIGHT 2048
//#define WIDTH 4096
//#define HEIGHT 4096
#define KWIDTH 3
#define KHEIGHT 3
/*
template<typename T> inline void membuff(hls::stream<T> &u,T mem[KHEIGHT-1][WIDTH],T mix[KHEIGHT][KWIDTH],int i, int j)
{
#pragma HLS INLINE
	int k,l;
	T tmp_in;
	if ((j<HEIGHT)&&(i<WIDTH))
	{
		tmp_in=u.read();
	}
	for(k=0;k<KHEIGHT;k++)
	{
		for(l=0;l<KWIDTH-1;l++)
		{
			mix[k][l]=mix[k][l+1];
		}
	}
	for(k=0;k<KHEIGHT-1;k++)
	{
		mix[k][KWIDTH-1]=mem[k][i];
	}
	mix[KHEIGHT-1][KWIDTH-1]=tmp_in;
	if (i<WIDTH)
	{
		for(k=0;k<KHEIGHT-1;k++)
		{
			mem[k][i]=mix[k+1][KWIDTH-1];
		}
		mem[KHEIGHT-2][i]=mix[KHEIGHT-1][KWIDTH-1];
	}
}
*/
my_data_type wave_kernel_stencil(my_data_type u[KHEIGHT][KWIDTH],
		my_const_type c[KHEIGHT][KWIDTH]);
void ops_wave_video(my_base_type u[HEIGHT * WIDTH],
		my_base_type v[HEIGHT * WIDTH], my_base_type u2[HEIGHT * WIDTH],
		my_base_type v2[HEIGHT * WIDTH], my_const_base_type c1[HEIGHT * WIDTH],
		my_base_type dt_2, my_base_type dt_2dx2, my_base_type dt,
		my_base_type dt_dx2, unsigned vdata[HEIGHT * WIDTH / 2], my_base_type a,
		my_base_type b, my_base_type &min, my_base_type &max);

#endif
