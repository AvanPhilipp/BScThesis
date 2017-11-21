#include <hls_stream.h>
#include <ap_int.h>
#include <ap_axi_sdata.h>

using namespace hls;

#define NEIGH_T (1)
//#define NEIGH_T (2)
#define TSIZE_T (2*NEIGH_T+1)
//#define WIDTH_T (16)
//#define HEIGHT_T (16)
//#define WIDTH_T (512)
//#define HEIGHT_T (512)
#define WIDTH_T (28)
#define HEIGHT_T (28)
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
typedef short my_data_type;
//typedef float my_data_type;
//typedef ap_uint<INLAYERS*sizeof(my_data_type)*8> my_data_in_type;
//typedef ap_uint<OUTLAYERS*sizeof(my_data_type)*8> my_data_out_type;
//typedef float my_templ_type;
typedef short my_templ_type;
//hls::stream<ap_axiu<LAYERS*sizeof(my_data_type)*8,1,1,1> > &x_in,
//typedef ap_axiu<4*sizeof(my_data_type)*8,1,1,1> my_data_in_type;
//typedef ap_axiu<8*sizeof(my_data_type)*8,1,1,1> my_data_out_type;

typedef ap_uint<4*sizeof(my_data_type)*8> my_data_in_type;
typedef ap_uint<8*sizeof(my_data_type)*8> my_data_out_type;

//typedef ap_axiu<1*sizeof(my_data_type)*8,1,1,1> my_data_in_type;
typedef stream<my_data_in_type> my_data_in_type_s;
typedef stream<my_data_out_type> my_data_out_type_s;

void conv(
		hls::stream<ap_uint<4*sizeof(my_data_type)*8> > &x_in,
		hls::stream<ap_uint<8*sizeof(my_data_type)*8> > &x_out,
		stream<my_templ_type> &t_in1,
		stream<my_templ_type> &t_in2,
		int tload);
/*void conv(
		hls::stream<ap_axiu<4*sizeof(my_data_type)*8,1,1,1> > &x_in,
		hls::stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > &x_out,
		stream<my_templ_type> &t_in1,
		stream<my_templ_type> &t_in2,
		int tload);*/
void conv_alt(
		hls::stream<ap_uint<4*sizeof(my_data_type)*8> > &x_in,
		hls::stream<ap_uint<8*sizeof(my_data_type)*8> > &x_out,
		stream<my_templ_type> &t_in1,
		int tload);
/*void conv_alt(
		hls::stream<ap_axiu<4*sizeof(my_data_type)*8,1,1,1> > &x_in,
		hls::stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > &x_out,
		stream<my_templ_type> &t_in1,
		int tload);
void pool(
		hls::stream<ap_axiu<1*sizeof(my_data_type)*8,1,1,1> > &x_in,
		hls::stream<ap_axiu<1*sizeof(my_data_type)*8,1,1,1> > &x_out);
*/
//void network (
//		hls::stream< ap_uint< 1*sizeof(my_data_type)*8> > &input,
//		hls::stream< ap_uint< 8*sizeof(my_data_type)*8> > &output,
//		stream<my_templ_type> &templ_1,
//		stream<my_templ_type> &templ_2,
//		int tload);

void mnistNet(
		hls::stream< ap_uint< sizeof(my_data_type)*8> > &input,
		hls::stream< ap_uint< sizeof(my_data_type)*8> > &output,
		stream<my_templ_type> &templ,
		int tload);
void VGGNet(
		hls::stream< ap_uint< 1*sizeof(my_data_type)*8> > &input,
		hls::stream< ap_uint< 1000*sizeof(my_data_type)*8> > &output,
		stream<my_templ_type> &templ,
		int tload);

inline ap_uint<sizeof(my_data_type)*8> float2ap_uint(
		my_data_type data)
{
	ap_uint<sizeof(my_data_type)*8> data_i;
	unsigned *data_d=(unsigned *)&data;
	data_i(sizeof(my_data_type)*8-1,0)=data_d[0];
	return data_i;
}

inline my_data_type ap_uint2float(
		ap_uint<sizeof(my_data_type)*8> data)
{
	my_data_type data_d;
	unsigned *data_i=(unsigned *)&data_d;
	data_i[0] = data(sizeof(my_data_type)*8-1,0);
	return data_d;
}

template<int INLAYERS, int OUTLAYERS, int TSIZE, int WIDTH, int HEIGHT> inline void conv_t(
		hls::stream<ap_uint<INLAYERS*sizeof(my_data_type)*8> > &x_in,
		hls::stream<ap_uint<OUTLAYERS*sizeof(my_data_type)*8> > &x_out,
		hls::stream<my_templ_type> &t_in,
		int tload)
{
	ap_uint<INLAYERS*sizeof(my_data_type)*8> tmp_in_v;
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
	ap_uint<OUTLAYERS*sizeof(my_data_type)*8> tmp_out;
	int i,j,k,l,m,n;
	int out_data_cnt=0;
	if (tload==1)
		//template betöltés
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
				//tmp_in[m]=ap_uint2float(tmp_in_v.data((m+1)*sizeof(my_data_type)*8-1,m*sizeof(my_data_type)*8));
				//tmp_in[m]=tmp_in_v.data((m+1)*sizeof(my_data_type)*8-1,m*sizeof(my_data_type)*8);
				tmp_in[m]=tmp_in_v((m+1)*sizeof(my_data_type)*8-1,m*sizeof(my_data_type)*8);
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
					//mem[m][k][wr_addr]=mix[m][k+1][TSIZE-1];
				}
				//mem[m][TSIZE-2][wr_addr]=mix[m][TSIZE-1][TSIZE-1];
				mix[m][TSIZE-1][TSIZE-1]=tmp_in[m];

				//Memóriát kétszer töltené be (az utolsó helyet ezért et egyel kevesebb
				// 3x3 rétegnél pol csak egyszer fut
				FOR_MEM : for(k=0;k<TSIZE-2;k++)
				{
					//mix[m][k][TSIZE-1]=mem[m][k][rd_addr];
					mem[m][k][wr_addr]=mix[m][k+1][TSIZE-1];
				}
				mem[m][TSIZE-2][wr_addr]=mix[m][TSIZE-1][TSIZE-1];
				//mix[m][TSIZE-1][TSIZE-1]=tmp_in[m];

				for(k=0;k<TSIZE;k++)
				{
					for(l=0;l<TSIZE;l++)
					{
						FOR_OUT_LAYERS : for(n=0;n<OUTLAYERS;n++)
						{
							if ((k==0)&&(l==0)&&(m==0))
							{
								//memtmp[n]=tconst[n]+mix[m][k][l]*tmem[n][m][k][l];
								memtmp[n]=mix[m][k][l]*tmem[n][m][k][l];
							}
							else
							{
								memtmp[n]+=mix[m][k][l]*tmem[n][m][k][l];
							}
						}
					}
				}
			}

			// egy sort meg egy oszlopot kihagy az elején.
			if ((j>=TSIZE-1)&&(i>=TSIZE-1))
			{
				for(n=0;n<OUTLAYERS;n++)
				{
					//tmp_out((n+1)*sizeof(my_data_type)*8-1,n*sizeof(my_data_type)*8)=float2ap_uint(memtmp[n]);
					tmp_RELU=memtmp[n]>0?memtmp[n]:0;
					//tmp_out.data((n+1)*sizeof(my_data_type)*8-1,n*sizeof(my_data_type)*8)=float2ap_uint(tmp_RELU);
					//tmp_out.data((n+1)*sizeof(my_data_type)*8-1,n*sizeof(my_data_type)*8)=tmp_RELU;
					tmp_out((n+1)*sizeof(my_data_type)*8-1,n*sizeof(my_data_type)*8)=tmp_RELU;
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
template<int LAYERS, int TSIZE, int WIDTH, int HEIGHT> inline void pool_t(
		hls::stream<ap_uint<LAYERS*sizeof(my_data_type)*8> > &x_in,
		hls::stream<ap_uint<LAYERS*sizeof(my_data_type)*8> > &x_out)
{
	ap_uint<LAYERS*sizeof(my_data_type)*8> tmp_in_v;
	my_data_type tmp_in[LAYERS];
	my_data_type mem[LAYERS][(WIDTH+TSIZE)/TSIZE];
	my_data_type tmp_max[LAYERS];
	int i,j,n,imodtsize,jmodtsize;
	int wr_addr;
	int rd_addr;
	ap_uint<LAYERS*sizeof(my_data_type)*8> tmp_out;
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
				//tmp_in[n]=ap_uint2float(tmp_in_v.data((n+1)*sizeof(my_data_type)*8-1,n*sizeof(my_data_type)*8));
				tmp_in[n]=tmp_in_v((n+1)*sizeof(my_data_type)*8-1,n*sizeof(my_data_type)*8);
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
				// Ha az utolsó pixel (jobb lent)
				if ((imodtsize==TSIZE-1)&&(jmodtsize==TSIZE-1))
				{
					//tmp_out.data((n+1)*sizeof(my_data_type)*8-1,n*sizeof(my_data_type)*8)=float2ap_uint(tmp_max[n]);
					tmp_out((n+1)*sizeof(my_data_type)*8-1,n*sizeof(my_data_type)*8)=tmp_max[n];
/*					if (n==LAYERS-1)
					{my_data_type
						x_out.write(tmp_out);
						out_data_cnt++;
					}*/
				}
				//Minden egyéb esetben
				else
				{
					// Ha a sor vége
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
/*
template<int INLAYERS, int OUTLAYERS, int TSIZE, int WIDTH, int HEIGHT> inline void conv_t(
		hls::stream<ap_axiu<INLAYERS*sizeof(my_data_type)*8,1,1,1> > &x_in,
		hls::stream<ap_axiu<OUTLAYERS*sizeof(my_data_type)*8,1,1,1> > &x_out,
		hls::stream<my_templ_type> &t_in,
		int tload)
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
		//template betöltés
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
				//tmp_in[m]=ap_uint2float(tmp_in_v.data((m+1)*sizeof(my_data_type)*8-1,m*sizeof(my_data_type)*8));
				tmp_in[m]=tmp_in_v.data((m+1)*sizeof(my_data_type)*8-1,m*sizeof(my_data_type)*8);
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
					//mem[m][k][wr_addr]=mix[m][k+1][TSIZE-1];
				}
				//mem[m][TSIZE-2][wr_addr]=mix[m][TSIZE-1][TSIZE-1];
				mix[m][TSIZE-1][TSIZE-1]=tmp_in[m];

				//Memóriát kétszer töltené be (az utolsó helyet ezért et egyel kevesebb
				// 3x3 rétegnél pol csak egyszer fut
				FOR_MEM : for(k=0;k<TSIZE-2;k++)
				{
					//mix[m][k][TSIZE-1]=mem[m][k][rd_addr];
					mem[m][k][wr_addr]=mix[m][k+1][TSIZE-1];
				}
				mem[m][TSIZE-2][wr_addr]=mix[m][TSIZE-1][TSIZE-1];
				//mix[m][TSIZE-1][TSIZE-1]=tmp_in[m];

				for(k=0;k<TSIZE;k++)
				{
					for(l=0;l<TSIZE;l++)
					{
						FOR_OUT_LAYERS : for(n=0;n<OUTLAYERS;n++)
						{
							if ((k==0)&&(l==0)&&(m==0))
							{
								//memtmp[n]=tconst[n]+mix[m][k][l]*tmem[n][m][k][l];
								memtmp[n]=mix[m][k][l]*tmem[n][m][k][l];
							}
							else
							{
								memtmp[n]+=mix[m][k][l]*tmem[n][m][k][l];
							}
						}
					}
				}
			}

			// egy sort meg egy oszlopot kihagy az elején.
			if ((j>=TSIZE-1)&&(i>=TSIZE-1))
			{
				for(n=0;n<OUTLAYERS;n++)
				{
					//tmp_out((n+1)*sizeof(my_data_type)*8-1,n*sizeof(my_data_type)*8)=float2ap_uint(memtmp[n]);
					tmp_RELU=memtmp[n]>0?memtmp[n]:0;
					//tmp_out.data((n+1)*sizeof(my_data_type)*8-1,n*sizeof(my_data_type)*8)=float2ap_uint(tmp_RELU);
					tmp_out.data((n+1)*sizeof(my_data_type)*8-1,n*sizeof(my_data_type)*8)=tmp_RELU;
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

template<int LAYERS, int TSIZE, int WIDTH, int HEIGHT> inline void pool_t(
		hls::stream<ap_axiu<LAYERS*sizeof(my_data_type)*8,1,1,1> > &x_in,
		hls::stream<ap_axiu<LAYERS*sizeof(my_data_type)*8,1,1,1> > &x_out)
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
//			FOR_IN_LAYERS : for(n=0;n<LAYERS;n++)
//			{
//				tmp_in[n]=ap_uint2float(tmp_in_v.data((n+1)*sizeof(my_data_type)*8-1,n*sizeof(my_data_type)*8));
//			}
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
//					if (n==LAYERS-1)
//					{my_data_type
//						x_out.write(tmp_out);
//						out_data_cnt++;
//					}
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
*/


template<int IN_WIDTH, int IN_HEIGHT, int IN_DEPTH, int TEMPLATE_SIZE, int OUT_DEPTH> inline void convolution_template(
		hls::stream< ap_uint< IN_DEPTH*sizeof(my_data_type)*8> > &input,
		hls::stream< ap_uint< OUT_DEPTH*sizeof(my_data_type)*8> > &output,
		hls::stream< my_templ_type > &templ_in,
		hls::stream< my_templ_type > &templ_out,
		int template_load)
{
	my_data_type image[IN_WIDTH][TEMPLATE_SIZE-1][IN_DEPTH];
	my_data_type image_register[IN_DEPTH];
	my_templ_type weigts[TEMPLATE_SIZE][TEMPLATE_SIZE][IN_DEPTH][OUT_DEPTH];
	my_templ_type bias[OUT_DEPTH];
	ap_uint<IN_DEPTH*sizeof(my_data_type)*8> input_tmp;
	ap_uint<OUT_DEPTH*sizeof(my_data_type)*8> output_tmp;


	/*
	 * Minden réteget egy template-vel konvolválunk végig.
	 * A template betöltése utána ezt többet nem kell újratölteni
	 */
	if(template_load == 1){
		/*
		 * OUT_DEPTH emeli meg a harmadik dimenzióját a képnek.
		 */
		LOAD_WEIGHTS:for(int o=0;o<OUT_DEPTH;o++){
			for(int h=0;h<IN_DEPTH;h++){
				for(int i=0;i<TEMPLATE_SIZE;i++){
					for(int j=0;j<TEMPLATE_SIZE;j++){
						weigts[i][j][h][o] = templ_in.read();
					}
				}

			}
		}

		for(int o=0;o<OUT_DEPTH;o++){
			bias[o] = templ_in.read();
		}
		templ_out.write(templ_in.read());
	}


	/*
	 * Alternatív image loading mechanizmus....
	 * kísérlet alatt. Megnézzük így megy-e.
	 * Ha jó akkor ez hatékonyabb mint az alsó
	 */
	int address = 0;
	for (int i = 0; i < IN_HEIGHT; i++) {
		for (int j = 0; j < IN_WIDTH; j++) {

			/**
			 * Blockramos megvalósítás esetén mindig csak három pixelt veszünk ki.
			 * Ezt háromszor tesszük meg templatenként.
			 * A súlyokat így sebességveszteség nélkül lehet három memóriába eltárolni a kilenc helyett.
			 * Cserébe a memóriacímekkel kell megoldani a megfelelő pixel betöltését.
			 */

			/**
			 * read_address csak a template-n belüli mozgásra kell.
			 * A ciklus futása után eldobjuk és újra elkérjük az aktuális memóriacímet.
			 * Pipeline miatt gondod okozhat, de nem lehet betenni egyel beljebb.
			 */
			int read_address = address;
			my_data_type sum[IN_DEPTH];

			// Erre adunk majd Pipeline-t
			for(int tw=0;tw<TEMPLATE_SIZE;tw++){

				// ap_uintből kell csinálni tömböt.
				if(tw == 0){
					input_tmp = input.read();
					for(int h=0;h<IN_DEPTH;h++){
						image_register[h]=input_tmp((h+1)*sizeof(my_data_type)*8-1,h*sizeof(my_data_type)*8);
					}

				}


				for(int th=0; th<TEMPLATE_SIZE;th++){

					/**
					 * Az utolsó oszlopba be kell tölteni egy új értéket.
					 * Ott kicsit más lesz az eljárás.
					 */
					if(tw==TEMPLATE_SIZE-1){
						/**
						 * Minden pixelt egyel feljebbi memóriacímre tol.
						 * At utolsó pixel feljebbtolása után a legalsó pixel üres lesz.
						 * Ide betöltjük a regiszterben lévő értéket.
						 */

						for(int h=0;h<IN_DEPTH;h++){
						image[read_address][th][h] = image[read_address][th+1][h];
						image[read_address][TEMPLATE_SIZE-1][h] = image_register[h];
						}
					}

					/**
					 * Minden bemenetet összeszorzunk a hozzá rendelt súllyal. (IN_DEPTH)
					 * Ha a kimenet szélesebb akkor itt több súllyal is összeszorozzuk. (OUT_DEPTH)
					 */
					for(int h=0;h<IN_DEPTH;h++){
						for(int o=0;o<OUT_DEPTH;o++){
							/**
							 * Ha az első futása akkor adunk neki kezdeti értéket.
							 * Ha a többedik akkor már csak növeljük.
							 */
							if(tw == 0 and th == 0 and h==0)
								/**
								 * A bias az accumlator kezdeti értéke.
								 */
								sum[o] = bias[o] + image[read_address][th][h] * weigts[j][th][h][o];
							else
								sum[o] += image[read_address][th][h] * weigts[j][th][h][o];
						}
					}


				}

				/**
				 * Utólagos megfontolás miatt beépített ReLu réteg a konvolúciós rétegbe.
				 */
				for(int o = 0; o < OUT_DEPTH;o++){
					if(sum[o] < 0){
						sum[o] = 0;
					}
				}


				/**
				 * A kimenetre már OUT_DEPTH mennyiségű adatot írunk.
				 */
				// AP_uintbe összefogni.
//				ap_uint<OUT_DEPTH*sizeof(my_data_type)*8> output_tmp;
				for(int o=0;o<OUT_DEPTH;o++){
					output_tmp((o+1)*sizeof(my_data_type)*8-1,o*sizeof(my_data_type)*8)=sum[o];
				}

				output.write(output_tmp);


				/**
				 * A blokkramban egyel arrébb lépünk.
				 * Ez a tempalte következő oszlopát jelenti.
				 */
				read_address++;


				/**
				 * Amikor végére értünk a template-nek.
				 * A memóriában tovább lépünk egyel, a templatet egy pixellel csúsztatjuk.
				 * A végleges megvalósításban itt a lépés méretét kell majd állíthatóvá tenni.
				 */
				if(tw == TEMPLATE_SIZE-1){
					if(address == IN_WIDTH-TEMPLATE_SIZE){ //((TEMPLATE_SIZE-1)/2) //= 3
						address = 0;
					}
					else{
						address++;
					}


				}
	/*
	 * A pipeline-olt for ciklusok záró bajszai.
	 * Ezek egymás után kell hogy jöjjenek.
	 * Közötte semmilyen utasítás nem futhat.
	 */
			}
		}
	}


	/*
	 * Eredeti ötlet. Majdnem biztos hogy nem jó megoldás. Ha a fenti nem jó akkor ezt kell kikupálni.
	 *
	int actual=0;
/// ellenőrizni kellene hogy mindent jó helyről szedek-e...
	FOR_IN_HEIGHT: for (int i = 0; i < IN_HEIGHT; i++) {
		// ide kell egy pipeline
		FOR_IN_WIDTH: for (int j = 0; j < IN_WIDTH; j++) {

			for(int i=0; i<TEMPLATE_SIZE;i++){
				for(int j=0; j<TEMPLATE_SIZE-1;j++){
					for(int h=0;h<IN_DEPTH;h++){
						image[i][j][h] = image[i][j+1][h];
					}
				}
				for(int j=0;j<TEMPLATE_SIZE;j++){
					image[i][j][TEMPLATE_SIZE-1] = image[i][actual][TEMPLATE_SIZE-1];
				}
			}
			for(int i=0;i<TEMPLATE_SIZE-1;i++){
				for(int h=0;h<IN_DEPTH;h++){
					image[0][i][h] = image[TEMPLATE_SIZE-1][i][h];
				}
			}

			//template és a környezet betöltve. Most kell a szorzást számolni
			for(int i=0; i<TEMPLATE_SIZE;i++){
				for(int j=0; j<TEMPLATE_SIZE-1;j++){
					for(int h=0;h<IN_DEPTH;h++){
						my_data_type temp_out;
						temp_out = weigts[i][j][h] * image[i][j][h] + bias[h];
						output.write(temp_out);
					}
				}
			}

		}
	}*/
}

template<int IN_WIDTH, int IN_HEIGHT, int IN_DEPTH, int POOL_SIZE>inline void pooling_template(
		hls::stream< ap_uint< IN_DEPTH*sizeof(my_data_type)*8> > &input,
		hls::stream< ap_uint< IN_DEPTH*sizeof(my_data_type)*8> > &output)
{
	my_data_type memory[IN_WIDTH/POOL_SIZE][IN_DEPTH];
	my_data_type input_tmp;
	my_data_type image_register[IN_DEPTH];
	/*
	 * A memória melyik helyére írunk.
	 */
	int address=0;
	/*
	 * A teljes bemenet bejárása.
	 */
	for(int w=0;w<IN_WIDTH;w++){
		for(int h=0;h<IN_HEIGHT;h++){
			for(int h=0;h<IN_DEPTH;h++){
				//Pipeline miatt itt jó?
				if(h == 0) {
					input_tmp = input.read();
				}
				/**
				 * ap_uint-ből tömbbe olvassuk az adatokat.
				 */
				image_register[h]=input_tmp;
				if(w%POOL_SIZE==0){
					/**
					 * Amennyiben az első sor első oszlopában vagyunk a maximum érték 0.
					 * A memóriába 0 beírása helyett az addigi memória tartalmat felülírjuk az új értékkel.
					 * Amennyiben az 0, elértük a célunkat.
					 * Amennyiben az nagyobb mint 0 a következő lépésben amúgy is felülírtuk volna ezzel az értékkel.
					 * Kisebb nem lehet a beépített relu réteg miatt.
					 */
					if(h%POOL_SIZE==0){
						memory[address][h] =image_register[h];
					}
				}
				/**
				 * Minden más esetben ha nagyobb felülírjuk ha nem akkor meghagyjuk.
				 */
				else {
					if(image_register[h] > memory[address][h]){
						memory[address][h] = image_register[h];
					}
				}

				/**
				 * Ha a pool template végére érünk akkor a meglévő tömböt kiírjuk a kimeneti streamre.
				 * A memória IN_DEPTH mennyiségű adatot tárol minden address-edik elemén.
				 * Ezt csak akkor kell kiírni ha a sor és az oszlop is a pool ablak jobb alsó csücskében van.
				 * Ezek után a memóriát felül lehet ütni a következő értékkel.
				 * Ez így csak az egymás mellett lévő pool rétegekre működik. a sorok miatt.
				 * 		Ha egy sor nem változtatja az előző sor értékét annaka a sornaka
				 */
				if(h%POOL_SIZE == POOL_SIZE-1 && h%POOL_SIZE == POOL_SIZE-1){
					ap_uint<IN_DEPTH*sizeof(my_data_type)*8> output_tmp;
					for(int h=0;h<IN_DEPTH;h++){
						output_tmp((h+1)*sizeof(my_data_type)*8-1,h*sizeof(my_data_type)*8)=memory[address][h];
					}
					output.write(output_tmp);
				}

				/**
				 * A memóriacímet egyel növeljük.
				 * Így minden pixelt bejárunk és minden oszlop bele lesz számolva a maximumba.
				 * Az eltolást a POOL_SIZE-onkénti kiírással érjük el.
				 */
				address++;
				if(address == IN_WIDTH/POOL_SIZE){
					address = 0;
				}
			}
		}
	}

/**
 * Régi kód.
 * Ha működika z új akkor nem tartjuk meg.
 * Nem optimalizált, Jó lenne újat írni mindenképpen.
 */
//	my_data_type image[IN_WIDTH][POOL_SIZE][IN_DEPTH];
//	my_data_type image_register[IN_DEPTH];
//	ap_uint< IN_DEPTH*sizeof(my_data_type)*8> input_tmp;
//	ap_uint< IN_DEPTH*sizeof(my_data_type)*8> output_tmp;
//
//
//	int address = 0;
//
//	FOR_IN_HEIGHT: for (int i = 0; i<IN_HEIGHT; i++) {
//		FOR_IN_WIDTH: for (int j = 0; j<IN_WIDTH; j++) {
//			/*
//			 * Reading the new value
//			 */
//			input_tmp = input.read();
//			for(int h=0;h<IN_DEPTH;h++){
//				image_register[h]=input_tmp((h+1)*sizeof(my_data_type)*8-1,h*sizeof(my_data_type)*8);
//			}
//			/*
//			 * Reshape pooling mixer
//			 * Mixing in the new value
//			 */
//			for(int ph=0;ph<POOL_SIZE;ph++){
//				for(int pw=0;pw<POOL_SIZE;pw++){
//					for(int pd=0;pd<IN_DEPTH;pd++){
//						image[pw][ph][pd] = image[pw][ph+1][pd];
//						image[pw][POOL_SIZE][pd] = image_register[pd];
//					}
//				}
//			}
//			for(int pd=0;pd<IN_DEPTH;pd++){
//				image[POOL_SIZE][POOL_SIZE][pd] = image_register[pd];
//			}
//
//			my_data_type max = 0;
//			int read_address = address;
//
//			for(int ph=0;ph<POOL_SIZE;ph++){
//				for(int pw=0;pw<POOL_SIZE;pw++){
//					for(int pd=0;pd<IN_DEPTH;pd++){
//						if(image[ph][read_address+pw][pd]>max){
//							max = image[ph][read_address+pw][pd];
//						}
//					}
//				}
//			}
//			output.write(max);
//		}
//	}
}

template<int IN_SIZE, int OUT_SIZE> inline void fully_connected_template(
		hls::stream< ap_uint< IN_SIZE*sizeof(my_data_type)*8> > &input,
		hls::stream< ap_uint< OUT_SIZE*sizeof(my_data_type)*8> > &output,
		hls::stream< my_templ_type > &weight_in,
		hls::stream< my_templ_type > &weight_out,
		int template_load)
{
	my_templ_type weights[IN_SIZE][OUT_SIZE];
	my_templ_type bias[OUT_SIZE];

	/*
	 * Súlyok betöltése.
	 */
	if(template_load == 1){
		for(int o = 0; o<OUT_SIZE;o++){
			for(int i =0;i<IN_SIZE;i++){
				weights[i][o]=weight_in.read();
			}
			bias[o] = weight_in.read();
		}
		weight_out.write(weight_in.read());
	}
	// Minden kimenetre ad valami értéket.
	for(int o = 0; o<OUT_SIZE;o++){

			// Minden neuronnak minden bemenetén végigmegy
			for(int i =0;i<IN_SIZE;i++){

				my_data_type temp_out;
				temp_out = input.read()*weights[i][o] + bias[i];
				output.write(temp_out);
			}
		}
	}

/**
 * ReLu réteg a nagyobb modulatitás elérés érdekében... (jobban megértettem a működést.)
 */
template<int SIZE> inline void relu_template(
	hls::stream< ap_uint< SIZE*sizeof(my_data_type)*8>  > &input,
	hls::stream< ap_uint< SIZE*sizeof(my_data_type)*8>  > &output)
{
	FOR_INPUTS : for(int i =0;i<SIZE;i++){
		my_data_type temp = input.read();
		if(temp < 0){
			output.write(0);
		}
		else{
			output.write(temp);
		}
	}
}
