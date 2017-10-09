#include <ap_int.h>
#include <hls_stream.h>
#include "ops_wave_video.h"

float mulpow2(float a, char logmult)
{
	float res=a;
	unsigned *tmp=(unsigned *)&res;
	ap_uint<32> tmp_int=*tmp;
	ap_uint<8> exponent=tmp_int(30,23);
	if (exponent>-logmult)
	{
		exponent+=logmult;//We do not check overflow!!!!
		tmp_int(30,23)=exponent;
	}
	else
	{
		tmp_int=0;
	}
	*tmp=tmp_int;
	return res;
}

double mulpow2(double a, short logmult)
{
	double res=a;
	unsigned *tmp=(unsigned *)&res;
	ap_uint<64> tmp_int=(tmp[1],tmp[0]);
	ap_uint<11> exponent=tmp_int(62,52);
	if (exponent>-logmult)
	{
		exponent+=logmult;//We do not check over flow!!!!
		tmp_int(62,52)=exponent;
	}
	else
	{
		tmp_int=0;
	}
	*tmp=tmp_int;
	return res;
}

my_data_type wave_deriv_stencil(my_data_type u[KHEIGHT][KWIDTH],my_const_type c[KHEIGHT][KWIDTH])
{
	my_data_type tmp;
	tmp.u=u[1][1].v;
	tmp.v=c[1][1].c1*(((u[0][1].u+u[1][0].u)+(u[2][1].u+u[1][2].u))-mulpow2(u[1][1].u,2));
	return tmp;
}

my_data_type wave_rk2_1_stencil(my_data_type u[KHEIGHT][KWIDTH],my_const_type c[KHEIGHT][KWIDTH],my_base_type dt,my_base_type dt_dx2)
{
	my_data_type tmp,deriv;
	deriv=wave_deriv_stencil(u,c);
	tmp.u=u[1][1].u+dt*deriv.u;
	tmp.v=u[1][1].v+dt_dx2*deriv.v;
	return tmp;
}

my_data_type wave_rk2_2_stencil(my_data_type u[KHEIGHT][KWIDTH],my_data_type uold[KHEIGHT][KWIDTH],my_const_type c[KHEIGHT][KWIDTH],my_base_type dt,my_base_type dt_dx2)
{
	my_data_type tmp,deriv;
	deriv=wave_deriv_stencil(u,c);
	tmp.u=uold[1][1].u+dt*deriv.u;
	tmp.v=uold[1][1].v+dt_dx2*deriv.v;
	return tmp;
}

void my_dma_in(my_base_type data_in_u[WIDTH * HEIGHT],
		my_base_type data_in_v[WIDTH * HEIGHT],
		hls::stream<my_data_type> &data_out,
		my_const_base_type data_in_c1[WIDTH * HEIGHT],
		hls::stream<my_const_type> &data_outc) {
	int i;
	my_data_type data_out_tmp;
	my_const_type data_outc_tmp;
//	my_dma_in_label1:for(i=0;i<WIDTH*HEIGHT;i++)
	my_dma_in_label1:for(i=0;i!=WIDTH*HEIGHT;i++)
	{
#pragma HLS PIPELINE
		data_out_tmp.u=data_in_u[i];
		data_out_tmp.v=data_in_v[i];
		data_outc_tmp.c1=data_in_c1[i];
		data_out.write(data_out_tmp);
		data_outc.write(data_outc_tmp);
	}
}

void my_dma_out(hls::stream<my_data_type> &data_in,
		my_base_type data_out_u[WIDTH * HEIGHT],
		my_base_type data_out_v[WIDTH * HEIGHT],
		hls::stream<my_const_type> &data_inc) {
	int i;
	my_data_type data_in_tmp;
	my_const_type data_inc_tmp;
//	my_dma_out_label0:for(i=0;i<WIDTH*HEIGHT;i++)
	my_dma_out_label0:for(i=0;i!=WIDTH*HEIGHT;i++)
	{
#pragma HLS PIPELINE
		data_in_tmp=data_in.read();
		data_inc_tmp=data_inc.read();
		data_out_u[i]=data_in_tmp.u;
		data_out_v[i]=data_in_tmp.v;
	}
}

void my_dma_out_video(hls::stream<my_data_type> &data_in,
		my_base_type data_out_u[WIDTH * HEIGHT],
		my_base_type data_out_v[WIDTH * HEIGHT],
		hls::stream<my_const_type> &data_inc,
		unsigned vdata[HEIGHT * WIDTH / 2], my_base_type a, my_base_type b) {
	int i;
	my_data_type data_in_tmp;
	my_const_type data_inc_tmp;
	ap_uint<32> v1;
	my_base_type tmp;
//	my_dma_out_label0: for (i = 0; i < WIDTH * HEIGHT; i++) {
	my_dma_out_label0: for (i = 0; i != WIDTH * HEIGHT; i++) {
#pragma HLS PIPELINE II=1 enable_flush
		data_in_tmp=data_in.read();
		data_inc_tmp=data_inc.read();
		data_out_u[i] = data_in_tmp.u;
		data_out_v[i] = data_in_tmp.v;
		tmp = data_in_tmp.u;
//		tmp = data_in_tmp.v;
//		v1(31, 16) = v1(15, 0);
//		v1(15, 0) = (unsigned short)(tmp * a + b);
		v1(31, 16) = (unsigned short)(tmp * a + b);
		v1(15, 0) = v1(31, 16);
		if (i % 2) {
			vdata[i / 2] = v1;
		}
	}
}

void my_dma_out_video1(hls::stream<my_data_type> &data_in,
		my_base_type data_out_u[WIDTH * HEIGHT],
		my_base_type data_out_v[WIDTH * HEIGHT],
		hls::stream<unsigned short> &vdata_in,
		unsigned vdata[HEIGHT * WIDTH / 2]) {
	int i;
	my_data_type data_in_tmp;
	unsigned short vdata_in_tmp;
	ap_uint<32> v1;
//	my_dma_out_label0: for (i = 0; i < WIDTH * HEIGHT; i++) {
	my_dma_out_label0: for (i = 0; i != WIDTH * HEIGHT; i++) {
#pragma HLS PIPELINE II=1 enable_flush
		data_in_tmp=data_in.read();
		vdata_in_tmp=vdata_in.read();
		data_out_u[i] = data_in_tmp.u;
		data_out_v[i] = data_in_tmp.v;
//		v1(31, 16) = v1(15, 0);
//		v1(15, 0) = vdata_in_tmp;
		v1(31, 16) = vdata_in_tmp;
		v1(15, 0) = v1(31, 16);
		if (i % 2) {
			vdata[i / 2] = v1;
		}
	}
}

#define PAR_COMP 8
void my_dma_out_minmax_video(hls::stream<my_data_type> &data_in,
		my_base_type data_out_u[WIDTH * HEIGHT],
		my_base_type data_out_v[WIDTH * HEIGHT],
		hls::stream<my_const_type> &data_inc,
		unsigned vdata[HEIGHT * WIDTH / 2], my_base_type a, my_base_type b,
		my_base_type &min, my_base_type &max) {
	int i;
	my_data_type data_in_tmp;
	my_const_type data_inc_tmp;
	my_base_type min_i[PAR_COMP],max_i[PAR_COMP];
	my_base_type min_t,max_t;
	ap_uint<32> v1;
	my_base_type tmp;
//	my_dma_out_minmax_video_label0: for (i = 0; i < WIDTH * HEIGHT; i++) {
	my_dma_out_minmax_video_label0: for (i = 0; i != WIDTH * HEIGHT; i++) {
#pragma HLS PIPELINE II=1 enable_flush
		data_in_tmp=data_in.read();
		data_inc_tmp=data_inc.read();
		data_out_u[i] = data_in_tmp.u;
		data_out_v[i] = data_in_tmp.v;
		tmp = data_in_tmp.u;
//		tmp = data_in_tmp.v;
//		v1(31, 16) = v1(15, 0);
//		v1(15, 0) = (unsigned short)(tmp * a + b);
		v1(31, 16) = (unsigned short)(tmp * a + b);
		v1(15, 0) = v1(31, 16);
		if (i % 2) {
			vdata[i / 2] = v1;
		}
		if (i>PAR_COMP-1)
		{
			if (min_i[i%PAR_COMP]>tmp) min_i[i%PAR_COMP]=tmp;
			if (max_i[i%PAR_COMP]<tmp) max_i[i%PAR_COMP]=tmp;
		} else {
			min_i[i%PAR_COMP]=tmp;
			max_i[i%PAR_COMP]=tmp;
		}
	}
	min_t=min_i[0];
	max_t=max_i[0];
	for(i=1;i<PAR_COMP;i++)
	{
		if (min_t>min_i[i]) min_t=min_i[i];
		if (max_t<max_i[i]) max_t=max_i[i];
	}
	min=min_t;
	max=max_t;
}

void wave_minmax_video(hls::stream<my_data_type> &data_in,
		hls::stream<my_data_type> &data_out,
		hls::stream<my_const_type> &data_inc,
		hls::stream<unsigned short> &vdata_out, my_base_type a, my_base_type b,
		my_base_type &min, my_base_type &max) {
	int i;
	my_data_type data_in_tmp;
	my_const_type data_inc_tmp;
	my_base_type min_i[PAR_COMP],max_i[PAR_COMP];
	my_base_type min_t,max_t;
	my_base_type tmp;
//	wave_minmax_video_label0: for (i = 0; i < WIDTH * HEIGHT; i++) {
	wave_minmax_video_label0: for (i = 0; i != WIDTH * HEIGHT; i++) {
#pragma HLS PIPELINE II=1 enable_flush
		data_in_tmp=data_in.read();
		data_inc_tmp=data_inc.read();
		tmp = data_in_tmp.u;
//		tmp = data_in_tmp.v;
		if (i>PAR_COMP-1)
		{
			if (min_i[i%PAR_COMP]>tmp) min_i[i%PAR_COMP]=tmp;
			if (max_i[i%PAR_COMP]<tmp) max_i[i%PAR_COMP]=tmp;
		} else {
			min_i[i%PAR_COMP]=tmp;
			max_i[i%PAR_COMP]=tmp;
		}
		data_out.write(data_in_tmp);
		vdata_out.write((unsigned short)(tmp * a + b));
	}
	min_t=min_i[0];
	max_t=max_i[0];
	for(i=1;i<PAR_COMP;i++)
	{
		if (min_t>min_i[i]) min_t=min_i[i];
		if (max_t<max_i[i]) max_t=max_i[i];
	}
	min=min_t;
	max=max_t;
}

void ops_waverk2_1_i(hls::stream<my_data_type> &u,
		hls::stream<my_data_type> &u2, hls::stream<my_data_type> &uold, hls::stream<my_const_type> &c,
		hls::stream<my_const_type> &c2, my_base_type dt_2, my_base_type dt_2dx2) {
	my_data_type tmp_in;
	my_const_type tmp_inc;
	my_data_type mem[KHEIGHT-1][WIDTH];
#pragma HLS ARRAY_PARTITION variable=mem complete dim=1
	my_const_type memc[KHEIGHT-1][WIDTH];
#pragma HLS ARRAY_PARTITION variable=memc complete dim=1
	my_data_type mix[KHEIGHT][KWIDTH];
	my_const_type mixc[KHEIGHT][KWIDTH];
	my_data_type memtmp;
	my_const_type memtmpc;
	int i,j,k,l;
//	compute_row: for(j=0,rd_idx=0,wr_idx=0;j<HEIGHT+KHEIGHT-2;j++)
	compute_row: for(j=0;j!=HEIGHT+KHEIGHT-2;j++)
	{
//		compute_col: for(i=0;i<WIDTH+KWIDTH-2;i++)
		compute_col: for(i=0;i!=WIDTH+KWIDTH-2;i++)
		{
#pragma HLS PIPELINE
//			membuff<my_data_type>(u,mem,mix,i,j);
//			membuff<my_const_type>(c,memc,mixc,i,j);
			if ((j<HEIGHT)&&(i<WIDTH))
			{
				tmp_in=u.read();
				tmp_inc=c.read();
			}
			for(k=0;k<KHEIGHT;k++)
			{
				for(l=0;l<KWIDTH-1;l++)
				{
					mix[k][l]=mix[k][l+1];
					mixc[k][l]=mixc[k][l+1];
				}
			}
			for(k=0;k<KHEIGHT-1;k++)
			{
				mix[k][KWIDTH-1]=mem[k][i];
				mixc[k][KWIDTH-1]=memc[k][i];
			}
			mix[KHEIGHT-1][KWIDTH-1]=tmp_in;
			mixc[KHEIGHT-1][KWIDTH-1]=tmp_inc;
			if (i<WIDTH)
			{
				for(k=0;k<KHEIGHT-1;k++)
				{
					mem[k][i]=mix[k+1][KWIDTH-1];
					memc[k][i]=mixc[k+1][KWIDTH-1];
				}
				mem[KHEIGHT-2][i]=mix[KHEIGHT-1][KWIDTH-1];
				memc[KHEIGHT-2][i]=mixc[KHEIGHT-1][KWIDTH-1];
			}
			if ((j>1)&&(j<HEIGHT)&&(i>1)&&(i<WIDTH))
			{
				memtmp=wave_rk2_1_stencil(mix,mixc,dt_2,dt_2dx2);
			} else {//border
				memtmp=mix[1][1];
				//memtmp.u=0.0f;
				//memtmp.v=0.0f;
			}
			memtmpc=mixc[1][1];
			if ((j>0)&&(i>0))
			{
				u2.write(memtmp);
				c2.write(memtmpc);
				uold.write(mix[1][1]);
			}
		}
	}
}

void ops_waverk2_2_i(hls::stream<my_data_type> &u,
		hls::stream<my_data_type> &u2, hls::stream<my_data_type> &uold, hls::stream<my_const_type> &c,
		hls::stream<my_const_type> &c2, my_base_type dt, my_base_type dt_dx2) {
	my_data_type tmp_in;
	my_data_type tmp_in_old;
	my_const_type tmp_inc;
	my_data_type mem[KHEIGHT-1][WIDTH];
#pragma HLS ARRAY_PARTITION variable=mem complete dim=1
	my_data_type mem_old[KHEIGHT-1][WIDTH];
#pragma HLS ARRAY_PARTITION variable=mem_old complete dim=1
	my_const_type memc[KHEIGHT-1][WIDTH];
#pragma HLS ARRAY_PARTITION variable=memc complete dim=1
	my_data_type mix[KHEIGHT][KWIDTH];
	my_data_type mix_old[KHEIGHT][KWIDTH];
	my_const_type mixc[KHEIGHT][KWIDTH];
	my_data_type memtmp;
	my_const_type memtmpc;
	int i,j,k,l;
//	compute_row: for(j=0,rd_idx=0,wr_idx=0;j<HEIGHT+KHEIGHT-2;j++)
	compute_row: for(j=0;j!=HEIGHT+KHEIGHT-2;j++)
	{
//		compute_col: for(i=0;i<WIDTH+KWIDTH-2;i++)
		compute_col: for(i=0;i!=WIDTH+KWIDTH-2;i++)
		{
#pragma HLS PIPELINE
//			membuff<my_data_type>(u,mem,mix,i,j);
//			membuff<my_data_type>(uold,mem_old,mix_old,i,j);
//			membuff<my_const_type>(c,memc,mixc,i,j);
			if ((j<HEIGHT)&&(i<WIDTH))
			{
				tmp_in=u.read();
				tmp_in_old=uold.read();
				tmp_inc=c.read();
			}
			for(k=0;k<KHEIGHT;k++)
			{
				for(l=0;l<KWIDTH-1;l++)
				{
					mix[k][l]=mix[k][l+1];
					mix_old[k][l]=mix_old[k][l+1];
					mixc[k][l]=mixc[k][l+1];
				}
			}
			for(k=0;k<KHEIGHT-1;k++)
			{
				mix[k][KWIDTH-1]=mem[k][i];
				mix_old[k][KWIDTH-1]=mem_old[k][i];
				mixc[k][KWIDTH-1]=memc[k][i];
			}
			mix[KHEIGHT-1][KWIDTH-1]=tmp_in;
			mix_old[KHEIGHT-1][KWIDTH-1]=tmp_in_old;
			mixc[KHEIGHT-1][KWIDTH-1]=tmp_inc;
			if (i<WIDTH)
			{
				for(k=0;k<KHEIGHT-1;k++)
				{
					mem[k][i]=mix[k+1][KWIDTH-1];
					mem_old[k][i]=mix_old[k+1][KWIDTH-1];
					memc[k][i]=mixc[k+1][KWIDTH-1];
				}
				mem[KHEIGHT-2][i]=mix[KHEIGHT-1][KWIDTH-1];
				mem_old[KHEIGHT-2][i]=mix_old[KHEIGHT-1][KWIDTH-1];
				memc[KHEIGHT-2][i]=mixc[KHEIGHT-1][KWIDTH-1];
			}
			if ((j>1)&&(j<HEIGHT)&&(i>1)&&(i<WIDTH))
			{
				memtmp=wave_rk2_2_stencil(mix,mix_old,mixc,dt,dt_dx2);
			} else {//border
				memtmp=mix[1][1];
				//memtmp.u=0.0f;
				//memtmp.v=0.0f;
			}
			memtmpc=mixc[1][1];
			if ((j>0)&&(i>0))
			{
				u2.write(memtmp);
				c2.write(memtmpc);
			}
		}
	}
}
/*
void ops_waverk2_2_i_l(my_data_type u[HEIGHT * WIDTH],
		my_data_type u2[HEIGHT * WIDTH], my_data_type uold[HEIGHT * WIDTH], my_const_type c[HEIGHT * WIDTH], my_base_type dt, my_base_type dt_dx2) {
	my_data_type tmp_in;
	my_data_type tmp_in_old;
	my_const_type tmp_inc;
	my_data_type mem[KHEIGHT-1][WIDTH];
#pragma HLS ARRAY_PARTITION variable=mem complete dim=1
	my_data_type mem_old[KHEIGHT-1][WIDTH];
#pragma HLS ARRAY_PARTITION variable=mem_old complete dim=1
	my_const_type memc[KHEIGHT-1][WIDTH];
#pragma HLS ARRAY_PARTITION variable=memc complete dim=1
	my_data_type mix[KHEIGHT][KWIDTH];
	my_data_type mix_old[KHEIGHT][KWIDTH];
	my_const_type mixc[KHEIGHT][KWIDTH];
	my_data_type memtmp;
	my_const_type memtmpc;
	int i,j,k,l,u_idx,u2_idx,rd_idx,wr_idx;
	compute_row: for(j=0,u_idx=0,u2_idx=0,rd_idx=0,wr_idx=0;j<HEIGHT+KHEIGHT-2;j++)
	{
		compute_col: for(i=0;i<WIDTH+KWIDTH-2;i++)
		{
#pragma HLS PIPELINE
			if ((j<HEIGHT)&&(i<WIDTH))
			{
				tmp_in=u[u_idx];
				tmp_in_old=uold[u_idx];
				tmp_inc=c[u_idx];
				u_idx++;
			}
			for(k=0;k<KHEIGHT;k++)
			{
				for(l=0;l<KWIDTH-1;l++)
				{
					mix[k][l]=mix[k][l+1];
					mix_old[k][l]=mix_old[k][l+1];
					mixc[k][l]=mixc[k][l+1];
				}
			}
			for(k=0;k<KHEIGHT-1;k++)
			{
				mix[k][KWIDTH-1]=mem[k][i];
				mix_old[k][KWIDTH-1]=mem_old[k][i];
				mixc[k][KWIDTH-1]=memc[k][i];
			}
			mix[KHEIGHT-1][KWIDTH-1]=tmp_in;
			mix_old[KHEIGHT-1][KWIDTH-1]=tmp_in_old;
			mixc[KHEIGHT-1][KWIDTH-1]=tmp_inc;
			if (i<WIDTH)
			{
				for(k=0;k<KHEIGHT-1;k++)
				{
					mem[k][i]=mix[k+1][KWIDTH-1];
					mem_old[k][i]=mix_old[k+1][KWIDTH-1];
					memc[k][i]=mixc[k+1][KWIDTH-1];
				}
				mem[KHEIGHT-2][i]=mix[KHEIGHT-1][KWIDTH-1];
				mem_old[KHEIGHT-2][i]=mix_old[KHEIGHT-1][KWIDTH-1];
				memc[KHEIGHT-2][i]=mixc[KHEIGHT-1][KWIDTH-1];
			}
			if ((j>1)&&(j<HEIGHT)&&(i>1)&&(i<WIDTH))
			{
				memtmp=wave_rk2_2_stencil(mix,mix_old,mixc,dt,dt_dx2);
			} else {//border
				memtmp=mix[1][1];
				//memtmp.u=0.0f;
				//memtmp.v=0.0f;
			}
			memtmpc=mixc[1][1];
			if ((j>0)&&(i>0))
			{
				u2[u2_idx]=memtmp;
				u2_idx++;
			}
			if (i<WIDTH)
			{
				if (rd_idx==WIDTH-1)
				{
					rd_idx=0;
				} else {
					rd_idx++;
				}
				if (wr_idx==WIDTH-1)
				{
					wr_idx=0;
				} else {
					wr_idx++;
				}
			}
		}
	}
}
*/
void ops_waverk2_i(hls::stream<my_data_type> &u,
		hls::stream<my_data_type> &u2, hls::stream<my_const_type> &c,
		hls::stream<my_const_type> &c2, my_base_type dt_2, my_base_type dt_2dx2, my_base_type dt, my_base_type dt_dx2) {
#pragma HLS DATAFLOW
	hls::stream<my_data_type> uold_i;
#pragma HLS RESOURCE variable=uold_i core=FIFO_SRL
#pragma HLS STREAM variable=uold_i depth=32 dim=1
	hls::stream<my_data_type> u2_i;
#pragma HLS RESOURCE variable=u2_i core=FIFO_SRL
#pragma HLS STREAM variable=u2_i depth=32 dim=1
	hls::stream<my_const_type> c2_i;
#pragma HLS RESOURCE variable=c2_i core=FIFO_SRL
#pragma HLS STREAM variable=c2_i depth=32 dim=1
	ops_waverk2_1_i(u,u2_i,uold_i,c,c2_i,dt_2,dt_2dx2);
	ops_waverk2_2_i(u2_i,u2,uold_i,c2_i,c2,dt,dt_dx2);
}

void ops_waverk2_i_2pe(hls::stream<my_data_type> &u,
		hls::stream<my_data_type> &u2, hls::stream<my_const_type> &c,
		hls::stream<my_const_type> &c2, my_base_type dt_2, my_base_type dt_2dx2, my_base_type dt, my_base_type dt_dx2) {
#pragma HLS DATAFLOW
	hls::stream<my_data_type> uold_i;
#pragma HLS RESOURCE variable=uold_i core=FIFO_SRL
#pragma HLS STREAM variable=uold_i depth=32 dim=1
	hls::stream<my_data_type> u2_i;
#pragma HLS RESOURCE variable=u2_i core=FIFO_SRL
#pragma HLS STREAM variable=u2_i depth=32 dim=1
	hls::stream<my_const_type> c2_i;
#pragma HLS RESOURCE variable=c2_i core=FIFO_SRL
#pragma HLS STREAM variable=c2_i depth=32 dim=1
	hls::stream<my_data_type> uold_i1;
#pragma HLS RESOURCE variable=uold_i1 core=FIFO_SRL
#pragma HLS STREAM variable=uold_i1 depth=32 dim=1
	hls::stream<my_data_type> u2_i1;
#pragma HLS RESOURCE variable=u2_i1 core=FIFO_SRL
#pragma HLS STREAM variable=u2_i depth=32 dim=1
	hls::stream<my_const_type> c2_i1;
#pragma HLS RESOURCE variable=c2_i1 core=FIFO_SRL
#pragma HLS STREAM variable=c2_i1 depth=32 dim=1

	hls::stream<my_data_type> u2_pe1;
#pragma HLS RESOURCE variable=u2_pe1 core=FIFO_SRL
#pragma HLS STREAM variable=u2_pe1 depth=32 dim=1
	hls::stream<my_const_type> c2_pe1;
#pragma HLS RESOURCE variable=c2_pe1 core=FIFO_SRL
#pragma HLS STREAM variable=c2_pe1 depth=32 dim=1

	ops_waverk2_1_i(u,u2_i,uold_i,c,c2_i,dt_2,dt_2dx2);
	ops_waverk2_2_i(u2_i,u2_pe1,uold_i,c2_i,c2_pe1,dt,dt_dx2);

	ops_waverk2_1_i(u2_pe1,u2_i1,uold_i1,c2_pe1,c2_i1,dt_2,dt_2dx2);
	ops_waverk2_2_i(u2_i1,u2,uold_i1,c2_i1,c2,dt,dt_dx2);
}

void ops_waverk2_i_4pe(hls::stream<my_data_type> &u,
		hls::stream<my_data_type> &u2, hls::stream<my_const_type> &c,
		hls::stream<my_const_type> &c2, my_base_type dt_2, my_base_type dt_2dx2, my_base_type dt, my_base_type dt_dx2) {
#pragma HLS DATAFLOW

	hls::stream<my_data_type> uold_i1;
#pragma HLS RESOURCE variable=uold_i1 core=FIFO_SRL
#pragma HLS STREAM variable=uold_i1 depth=32 dim=1
	hls::stream<my_data_type> uold_i2;
#pragma HLS RESOURCE variable=uold_i2 core=FIFO_SRL
#pragma HLS STREAM variable=uold_i2 depth=32 dim=1
	hls::stream<my_data_type> uold_i3;
#pragma HLS RESOURCE variable=uold_i3 core=FIFO_SRL
#pragma HLS STREAM variable=uold_i3 depth=32 dim=1
	hls::stream<my_data_type> uold_i4;
#pragma HLS RESOURCE variable=uold_i4 core=FIFO_SRL
#pragma HLS STREAM variable=uold_i4 depth=32 dim=1

	hls::stream<my_data_type> u2_i1;
#pragma HLS RESOURCE variable=u2_i1 core=FIFO_SRL
#pragma HLS STREAM variable=u2_i1 depth=32 dim=1
	hls::stream<my_data_type> u2_i2;
#pragma HLS RESOURCE variable=u2_i2 core=FIFO_SRL
#pragma HLS STREAM variable=u2_i2 depth=32 dim=1
	hls::stream<my_data_type> u2_i3;
#pragma HLS RESOURCE variable=u2_i3 core=FIFO_SRL
#pragma HLS STREAM variable=u2_i3 depth=32 dim=1
	hls::stream<my_data_type> u2_i4;
#pragma HLS RESOURCE variable=u2_i4 core=FIFO_SRL
#pragma HLS STREAM variable=u2_i4 depth=32 dim=1

	hls::stream<my_const_type> c2_i1;
#pragma HLS RESOURCE variable=c2_i1 core=FIFO_SRL
#pragma HLS STREAM variable=c2_i1 depth=32 dim=1
	hls::stream<my_const_type> c2_i2;
#pragma HLS RESOURCE variable=c2_i2 core=FIFO_SRL
#pragma HLS STREAM variable=c2_i2 depth=32 dim=1
	hls::stream<my_const_type> c2_i3;
#pragma HLS RESOURCE variable=c2_i3 core=FIFO_SRL
#pragma HLS STREAM variable=c2_i3 depth=32 dim=1
	hls::stream<my_const_type> c2_i4;
#pragma HLS RESOURCE variable=c2_i4 core=FIFO_SRL
#pragma HLS STREAM variable=c2_i4 depth=32 dim=1

	hls::stream<my_data_type> u2_pe1;
#pragma HLS RESOURCE variable=u2_pe1 core=FIFO_SRL
#pragma HLS STREAM variable=u2_pe1 depth=32 dim=1
	hls::stream<my_data_type> u2_pe2;
#pragma HLS RESOURCE variable=u2_pe2 core=FIFO_SRL
#pragma HLS STREAM variable=u2_pe2 depth=32 dim=1
	hls::stream<my_data_type> u2_pe3;
#pragma HLS RESOURCE variable=u2_pe3 core=FIFO_SRL
#pragma HLS STREAM variable=u2_pe3 depth=32 dim=1

	hls::stream<my_const_type> c2_pe1;
#pragma HLS RESOURCE variable=c2_pe1 core=FIFO_SRL
#pragma HLS STREAM variable=c2_pe1 depth=32 dim=1
	hls::stream<my_const_type> c2_pe2;
#pragma HLS RESOURCE variable=c2_pe2 core=FIFO_SRL
#pragma HLS STREAM variable=c2_pe2 depth=32 dim=1
	hls::stream<my_const_type> c2_pe3;
#pragma HLS RESOURCE variable=c2_pe3 core=FIFO_SRL
#pragma HLS STREAM variable=c2_pe3 depth=32 dim=1

	ops_waverk2_1_i(u,u2_i1,uold_i1,c,c2_i1,dt_2,dt_2dx2);
	ops_waverk2_2_i(u2_i1,u2_pe1,uold_i1,c2_i1,c2_pe1,dt,dt_dx2);

	ops_waverk2_1_i(u2_pe1,u2_i2,uold_i2,c2_pe1,c2_i2,dt_2,dt_2dx2);
	ops_waverk2_2_i(u2_i2,u2_pe2,uold_i2,c2_i2,c2_pe2,dt,dt_dx2);

	ops_waverk2_1_i(u2_pe2,u2_i3,uold_i3,c2_pe2,c2_i3,dt_2,dt_2dx2);
	ops_waverk2_2_i(u2_i3,u2_pe3,uold_i3,c2_i3,c2_pe3,dt,dt_dx2);

	ops_waverk2_1_i(u2_pe3,u2_i4,uold_i4,c2_pe3,c2_i4,dt_2,dt_2dx2);
	ops_waverk2_2_i(u2_i4,u2,uold_i4,c2_i4,c2,dt,dt_dx2);
}

/*
void ops_waverk2_i_l(my_data_type u[HEIGHT * WIDTH],
		my_data_type u2[HEIGHT * WIDTH], my_const_type c[HEIGHT * WIDTH],
		my_base_type dt_2, my_base_type dt_2dx2, my_base_type dt, my_base_type dt_dx2) {
#pragma HLS DATAFLOW
	my_data_type uold_i[WIDTH*HEIGHT];
#pragma HLS RESOURCE variable=uold_i core=FIFO_SRL
#pragma HLS STREAM variable=uold_i depth=32 dim=1
	my_data_type u2_i[WIDTH*HEIGHT];
#pragma HLS RESOURCE variable=u2_i core=FIFO_SRL
#pragma HLS STREAM variable=u2_i depth=32 dim=1
	my_const_type c2_i[WIDTH*HEIGHT];
#pragma HLS RESOURCE variable=c2_i core=FIFO_SRL
#pragma HLS STREAM variable=c2_i depth=32 dim=1
	ops_waverk2_1_i(u,u2_i,uold_i,c,c2_i,dt_2,dt_2dx2);
	ops_waverk2_2_i_l(u2_i,u2,uold_i,c2_i,dt,dt_dx2);
}
*/
void ops_wave_video(my_base_type u[HEIGHT * WIDTH], my_base_type v[HEIGHT * WIDTH],
		my_base_type u2[HEIGHT * WIDTH],
		my_base_type v2[HEIGHT * WIDTH],
		my_const_base_type c1[HEIGHT * WIDTH],my_base_type dt_2, my_base_type dt_2dx2, my_base_type dt, my_base_type dt_dx2,
		unsigned vdata[HEIGHT * WIDTH / 2], my_base_type a, my_base_type b,my_base_type &min,my_base_type &max
		) {
#pragma HLS DATAFLOW
#pragma HLS INTERFACE m_axi depth=256 port=u offset=slave bundle=u num_read_outstanding=16 num_write_outstanding=16 max_read_burst_length=32 max_write_burst_length=32
#pragma HLS INTERFACE m_axi depth=256 port=v offset=slave bundle=v num_read_outstanding=16 num_write_outstanding=16 max_read_burst_length=32 max_write_burst_length=32
#pragma HLS INTERFACE m_axi depth=256 port=u2 offset=slave bundle=u2 num_read_outstanding=16 num_write_outstanding=16 max_read_burst_length=32 max_write_burst_length=32
#pragma HLS INTERFACE m_axi depth=256 port=v2 offset=slave bundle=v2 num_read_outstanding=16 num_write_outstanding=16 max_read_burst_length=32 max_write_burst_length=32
#pragma HLS INTERFACE m_axi depth=256 port=c1 offset=slave bundle=c1 num_read_outstanding=16 num_write_outstanding=16 max_read_burst_length=32 max_write_burst_length=32
#pragma HLS INTERFACE m_axi depth=256 port=vdata offset=slave bundle=vdata num_read_outstanding=16 num_write_outstanding=16 max_read_burst_length=32 max_write_burst_length=32
#pragma HLS INTERFACE s_axilite port=return
#pragma HLS INTERFACE s_axilite port=a
#pragma HLS INTERFACE s_axilite port=b
#pragma HLS INTERFACE s_axilite port=dt_2
#pragma HLS INTERFACE s_axilite port=dt_2dx2
#pragma HLS INTERFACE s_axilite port=dt
#pragma HLS INTERFACE s_axilite port=dt_dx2
#pragma HLS INTERFACE s_axilite port=min
#pragma HLS INTERFACE s_axilite port=max

	hls::stream<my_data_type> u_i;
#pragma HLS RESOURCE variable=u_i core=FIFO_SRL
#pragma HLS STREAM variable=u_i depth=32 dim=1
	hls::stream<my_const_type> c_i;
#pragma HLS RESOURCE variable=c_i core=FIFO_SRL
#pragma HLS STREAM variable=c_i depth=32 dim=1


	hls::stream<my_data_type> u_i1;
#pragma HLS RESOURCE variable=u_i1 core=FIFO_SRL
#pragma HLS STREAM variable=u_i1 depth=32 dim=1
	hls::stream<my_const_type> c_i1;
#pragma HLS RESOURCE variable=c_i1 core=FIFO_SRL
#pragma HLS STREAM variable=c_i1 depth=32 dim=1

	hls::stream<my_data_type> u_i2;
#pragma HLS RESOURCE variable=u_i2 core=FIFO_SRL
#pragma HLS STREAM variable=u_i2 depth=32 dim=1
	hls::stream<my_data_type> u_i3;
#pragma HLS RESOURCE variable=u_i3 core=FIFO_SRL
#pragma HLS STREAM variable=u_i3 depth=32 dim=1
	hls::stream<my_const_type> c_i2;
#pragma HLS RESOURCE variable=c_i2 core=FIFO_SRL
#pragma HLS STREAM variable=c_i2 depth=32 dim=1
	hls::stream<my_const_type> c_i3;
#pragma HLS RESOURCE variable=c_i3 core=FIFO_SRL
#pragma HLS STREAM variable=c_i3 depth=32 dim=1

	hls::stream<my_data_type> u_i4;
#pragma HLS RESOURCE variable=u_i4 core=FIFO_SRL
#pragma HLS STREAM variable=u_i4 depth=32 dim=1
	hls::stream<my_data_type> u_i5;
#pragma HLS RESOURCE variable=u_i5 core=FIFO_SRL
#pragma HLS STREAM variable=u_i5 depth=32 dim=1
	hls::stream<my_data_type> u_i6;
#pragma HLS RESOURCE variable=u_i6 core=FIFO_SRL
#pragma HLS STREAM variable=u_i6 depth=32 dim=1
	hls::stream<my_data_type> u_i7;
#pragma HLS RESOURCE variable=u_i7 core=FIFO_SRL
#pragma HLS STREAM variable=u_i7 depth=32 dim=1
	hls::stream<my_const_type> c_i4;
#pragma HLS RESOURCE variable=c_i4 core=FIFO_SRL
#pragma HLS STREAM variable=c_i4 depth=32 dim=1
	hls::stream<my_const_type> c_i5;
#pragma HLS RESOURCE variable=c_i5 core=FIFO_SRL
#pragma HLS STREAM variable=c_i5 depth=32 dim=1
	hls::stream<my_const_type> c_i6;
#pragma HLS RESOURCE variable=c_i6 core=FIFO_SRL
#pragma HLS STREAM variable=c_i6 depth=32 dim=1
	hls::stream<my_const_type> c_i7;
#pragma HLS RESOURCE variable=c_i7 core=FIFO_SRL
#pragma HLS STREAM variable=c_i7 depth=32 dim=1

	hls::stream<my_data_type> u2_i;
#pragma HLS RESOURCE variable=u2_i core=FIFO_SRL
#pragma HLS STREAM variable=u2_i depth=32 dim=1
	hls::stream<my_const_type> c2_i;
#pragma HLS RESOURCE variable=c2_i core=FIFO_SRL
#pragma HLS STREAM variable=c2_i depth=32 dim=1

	hls::stream<unsigned short> vdata_out;
#pragma HLS RESOURCE variable=vdata_out core=FIFO_SRL
#pragma HLS STREAM variable=vdata_out depth=32 dim=1
	hls::stream<my_data_type> u2_i_dma;
#pragma HLS RESOURCE variable=u2_i_dma core=FIFO_SRL
#pragma HLS STREAM variable=u2_i_dma depth=32 dim=1

	//my_dma_in(u,u_i);
	my_dma_in(u,v,u_i,c1,c_i);
	//1 PE
//	ops_waverk2_i(u_i,u2_i,c_i,c2_i,dt_2,dt_2dx2,dt,dt_dx2);
	//2 PE
//	ops_waverk2_i(u_i,u_i1,c_i,c_i1,dt_2,dt_2dx2,dt,dt_dx2);
//	ops_waverk2_i(u_i1,u2_i,c_i1,c2_i,dt_2,dt_2dx2,dt,dt_dx2);
//	ops_waverk2_i_2pe(u_i,u2_i,c_i,c2_i,dt_2,dt_2dx2,dt,dt_dx2);
	//4 PE
//	ops_waverk2_i(u_i ,u_i1,c_i ,c_i1,dt_2,dt_2dx2,dt,dt_dx2);
//	ops_waverk2_i(u_i1,u_i2,c_i1,c_i2,dt_2,dt_2dx2,dt,dt_dx2);
//	ops_waverk2_i(u_i2,u_i3,c_i2,c_i3,dt_2,dt_2dx2,dt,dt_dx2);
//	ops_waverk2_i(u_i3,u2_i,c_i3,c2_i,dt_2,dt_2dx2,dt,dt_dx2);
	ops_waverk2_i_4pe(u_i,u2_i,c_i,c2_i,dt_2,dt_2dx2,dt,dt_dx2);
	//8 PE
//	ops_waverk2_i(u_i ,u_i1,c_i ,c_i1,dt_2,dt_2dx2,dt,dt_dx2);
//	ops_waverk2_i(u_i1,u_i2,c_i1,c_i2,dt_2,dt_2dx2,dt,dt_dx2);
//	ops_waverk2_i(u_i2,u_i3,c_i2,c_i3,dt_2,dt_2dx2,dt,dt_dx2);
//	ops_waverk2_i(u_i3,u_i4,c_i3,c_i4,dt_2,dt_2dx2,dt,dt_dx2);
//	ops_waverk2_i(u_i4,u_i5,c_i4,c_i5,dt_2,dt_2dx2,dt,dt_dx2);
//	ops_waverk2_i(u_i5,u_i6,c_i5,c_i6,dt_2,dt_2dx2,dt,dt_dx2);
//	ops_waverk2_i(u_i6,u_i7,c_i6,c_i7,dt_2,dt_2dx2,dt,dt_dx2);
//	ops_waverk2_i(u_i7,u2_i,c_i7,c2_i,dt_2,dt_2dx2,dt,dt_dx2);

//	my_dma_out_video(u2_i,u2,v2,c2_i,vdata,a,b);
//	wave_minmax_video(u2_i,u2_i_dma,c2_i,vdata_out,a,b,min,max);
//	my_dma_out_video1(u2_i_dma,u2,v2,vdata_out,vdata);

	my_dma_out_minmax_video(u2_i,u2,v2,c2_i,vdata,a,b,min,max);
}
