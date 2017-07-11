#include <hls_stream.h>
#include <ap_int.h>
#include <ap_axi_sdata.h>

using namespace hls;

#define NEIGH_T (1)
//#define NEIGH_T (2)
#define TSIZE_T (2*NEIGH_T+1)
//#define WIDTH_T (16)
//#define HEIGHT_T (16)
#define WIDTH_T (512)
#define HEIGHT_T (512)
#define INLAYERS_T (4)
#define OUTLAYERS_T (8)
//#define TSIZE_T (2)
//#define INLAYERS_T (8)
//#define OUTLAYERS_T (8)

//#define NEIGH (1)
//#define NEIGH (2)
//#define TSIZE (2*NEIGH+1)
//#define WIDTH (16)
//#define HEIGHT (16)
//#define WIDTH (512)
//#define HEIGHT (512)
//#define INLAYERS (4)
//#define OUTLAYERS (8)

//typedef int my_data_type;
//typedef short my_data_type;
typedef float my_data_type;
//typedef ap_uint<INLAYERS*sizeof(my_data_type)*8> my_data_in_type;
//typedef ap_uint<OUTLAYERS*sizeof(my_data_type)*8> my_data_out_type;
typedef float my_templ_type;
//typedef short my_templ_type;

typedef ap_axiu<4*sizeof(my_data_type)*8,1,1,1> my_data_in_type;
typedef ap_axiu<8*sizeof(my_data_type)*8,1,1,1> my_data_out_type;
//typedef ap_axiu<1*sizeof(my_data_type)*8,1,1,1> my_data_in_type;
//typedef ap_axiu<1*sizeof(my_data_type)*8,1,1,1> my_data_out_type;
typedef stream<my_data_in_type> my_data_in_type_s;
typedef stream<my_data_out_type> my_data_out_type_s;

void conv(hls::stream<ap_axiu<4*sizeof(my_data_type)*8,1,1,1> > &x_in, hls::stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > &x_out, stream<my_templ_type> &t_in1, stream<my_templ_type> &t_in2, int tload);
void conv_alt(hls::stream<ap_axiu<4*sizeof(my_data_type)*8,1,1,1> > &x_in, hls::stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > &x_out, stream<my_templ_type> &t_in1, int tload);
void pool(hls::stream<ap_axiu<1*sizeof(my_data_type)*8,1,1,1> > &x_in, hls::stream<ap_axiu<1*sizeof(my_data_type)*8,1,1,1> > &x_out);

inline ap_uint<sizeof(my_data_type)*8> float2ap_uint(my_data_type data)
{
	ap_uint<sizeof(my_data_type)*8> data_i;
	unsigned *data_d=(unsigned *)&data;
	data_i(sizeof(my_data_type)*8-1,0)=data_d[0];
	return data_i;
}

inline my_data_type ap_uint2float(ap_uint<sizeof(my_data_type)*8> data)
{
	my_data_type data_d;
	unsigned *data_i=(unsigned *)&data_d;
	data_i[0] = data(sizeof(my_data_type)*8-1,0);
	return data_d;
}

template<int INLAYERS, int OUTLAYERS, int TSIZE, int WIDTH, int HEIGHT> inline void conv_t(hls::stream<ap_axiu<INLAYERS*sizeof(my_data_type)*8,1,1,1> > &x_in, hls::stream<ap_axiu<OUTLAYERS*sizeof(my_data_type)*8,1,1,1> > &x_out, stream<my_templ_type> &t_in, int tload)
{
	ap_axiu<INLAYERS*sizeof(my_data_type)*8,1,1,1> tmp_in_v;
	my_data_type tmp_in[INLAYERS];
	my_data_type mem[INLAYERS][TSIZE-1][WIDTH];
	my_data_type mix[INLAYERS][TSIZE][TSIZE];
	my_templ_type tmpt;
	my_templ_type tmem[OUTLAYERS][INLAYERS][TSIZE][TSIZE];
	my_templ_type tconst[OUTLAYERS];
	int wr_addr;
	int rd_addr;
	my_data_type memtmp[OUTLAYERS];
	my_data_type tmp_RELU;
	ap_axiu<OUTLAYERS*sizeof(my_data_type)*8,1,1,1> tmp_out;
	int i,j,k,l,m,n;
	int out_data_cnt=0;
	if (tload==1)
	{
		TLOAD5 : for(n=0;n<OUTLAYERS;n++)
		{
			TLOAD3 : for(m=0;m<INLAYERS;m++)
			{
				TLOAD2 : for(k=0;k<TSIZE;k++)
				{
					TLOAD1 : for(l=0;l<TSIZE;l++)
					{
						tmpt=t_in.read();
						tmem[n][m][k][l]=tmpt;
					}
				}
			}
		}
		TLOAD4 : for(n=0;n<OUTLAYERS;n++)
		{
			tmpt=t_in.read();
			tconst[n]=tmpt;
		}
	}
	FOR_HEIGHT : for(j=0,wr_addr=0,rd_addr=1;j<HEIGHT;j++)
	{
		FOR_WIDTH : for(i=0;i<WIDTH;i++)
		{
			tmp_in_v=x_in.read();
			FOR_IN_LAYERS : for(m=0;m<INLAYERS;m++)
			{
				tmp_in[m]=ap_uint2float(tmp_in_v.data((m+1)*sizeof(my_data_type)*8-1,m*sizeof(my_data_type)*8));
				for(k=0;k<TSIZE;k++)
				{
					for(l=0;l<TSIZE-1;l++)
					{
						mix[m][k][l]=mix[m][k][l+1];
					}
				}
				FOR_MIX : for(k=0;k<TSIZE-1;k++)
				{
					mix[m][k][TSIZE-1]=mem[m][k][rd_addr];
					mem[m][k][wr_addr]=mix[m][k+1][TSIZE-1];
				}
				mem[m][TSIZE-2][wr_addr]=mix[m][TSIZE-1][TSIZE-1];
				mix[m][TSIZE-1][TSIZE-1]=tmp_in[m];

				for(k=0;k<TSIZE;k++)
				{
					for(l=0;l<TSIZE;l++)
					{
						FOR_OUT_LAYERS : for(n=0;n<OUTLAYERS;n++)
						{
							if ((k==0)&&(l==0)&&(m==0))
							{
								memtmp[n]=tconst[n]+mix[m][k][l]*tmem[n][m][k][l];
							}
							else
							{
								memtmp[n]+=mix[m][k][l]*tmem[n][m][k][l];
							}
						}
					}
				}
			}
			if ((j>=TSIZE-1)&&(i>=TSIZE-1))
			{
				for(n=0;n<OUTLAYERS;n++)
				{
					//tmp_out((n+1)*sizeof(my_data_type)*8-1,n*sizeof(my_data_type)*8)=float2ap_uint(memtmp[n]);
					tmp_RELU=memtmp[n]>0?memtmp[n]:0;
					tmp_out.data((n+1)*sizeof(my_data_type)*8-1,n*sizeof(my_data_type)*8)=float2ap_uint(tmp_RELU);
				}
				x_out.write(tmp_out);
				out_data_cnt++;
			}
			if (wr_addr==WIDTH-1)
			{
				wr_addr=0;
			}
			else
			{
				wr_addr++;
			}
			if (rd_addr==WIDTH-1)
			{
				rd_addr=0;
			}
			else
			{
				rd_addr++;
			}
		}
	}
}

template<int LAYERS, int TSIZE, int WIDTH, int HEIGHT> inline void pool_t(hls::stream<ap_axiu<LAYERS*sizeof(my_data_type)*8,1,1,1> > &x_in, hls::stream<ap_axiu<LAYERS*sizeof(my_data_type)*8,1,1,1> > &x_out)
{
	ap_axiu<LAYERS*sizeof(my_data_type)*8,1,1,1> tmp_in_v;
	my_data_type tmp_in[LAYERS];
	my_data_type mem[LAYERS][(WIDTH+TSIZE)/TSIZE];
	my_data_type tmp_max[LAYERS];
	int i,j,n,imodtsize,jmodtsize;
	int wr_addr;
	int rd_addr;
	ap_axiu<LAYERS*sizeof(my_data_type)*8,1,1,1> tmp_out;
	int out_data_cnt=0;
	FOR_HEIGHT : for(j=0,wr_addr=0,rd_addr=0,jmodtsize=0;j<HEIGHT;j++)
	{
		FOR_WIDTH : for(i=0,imodtsize=0;i<WIDTH;i++)
		{
			tmp_in_v=x_in.read();
/*			FOR_IN_LAYERS : for(n=0;n<LAYERS;n++)
			{
				tmp_in[n]=ap_uint2float(tmp_in_v.data((n+1)*sizeof(my_data_type)*8-1,n*sizeof(my_data_type)*8));
			}*/
			FOR_IN_LAYERS : for(n=0;n<LAYERS;n++)
			{
				tmp_in[n]=ap_uint2float(tmp_in_v.data((n+1)*sizeof(my_data_type)*8-1,n*sizeof(my_data_type)*8));
				if ((imodtsize==0)&&(jmodtsize==0))
				{
					tmp_max[n]=tmp_in[n];
				}
				else
				{
					if (imodtsize==0)
					{
						tmp_max[n]=mem[n][rd_addr];
					}
					if (tmp_max[n]<tmp_in[n])
					{
						tmp_max[n]=tmp_in[n];
					}
				}
				if ((imodtsize==TSIZE-1)&&(jmodtsize==TSIZE-1))
				{
					tmp_out.data((n+1)*sizeof(my_data_type)*8-1,n*sizeof(my_data_type)*8)=float2ap_uint(tmp_max[n]);
/*					if (n==LAYERS-1)
					{
						x_out.write(tmp_out);
						out_data_cnt++;
					}*/
				}
				else
				{
					if (imodtsize==TSIZE-1)
					{
						mem[n][wr_addr]=tmp_max[n];
					}
				}
			}
			if ((imodtsize==TSIZE-1)&&(jmodtsize==TSIZE-1))
			{
				x_out.write(tmp_out);
				out_data_cnt++;
			}
			if (imodtsize==TSIZE-1)
			{
				imodtsize=0;
				if (wr_addr==WIDTH/TSIZE-1)
				{
					wr_addr=0;
				}
				else
				{
					wr_addr++;
				}
				if (rd_addr==WIDTH/TSIZE-1)
				{
					rd_addr=0;
				}
				else
				{
					rd_addr++;
				}
			}
			else
			{
				imodtsize++;
			}
			if (i==WIDTH-1)
			{
				if (jmodtsize==TSIZE-1)
				{
					jmodtsize=0;
				}
				else
				{
					jmodtsize++;
				}
			}
		}
	}
}
