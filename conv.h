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
typedef short my_data_type;
//typedef float my_data_type;
//typedef ap_uint<INLAYERS*sizeof(my_data_type)*8> my_data_in_type;
//typedef ap_uint<OUTLAYERS*sizeof(my_data_type)*8> my_data_out_type;
//typedef float my_templ_type;
typedef short my_templ_type;
//hls::stream<ap_axiu<LAYERS*sizeof(my_data_type)*8,1,1,1> > &x_in,
typedef ap_axiu<4*sizeof(my_data_type)*8,1,1,1> my_data_in_type;
typedef ap_axiu<8*sizeof(my_data_type)*8,1,1,1> my_data_out_type;
//typedef ap_axiu<1*sizeof(my_data_type)*8,1,1,1> my_data_in_type;
typedef stream<my_data_in_type> my_data_in_type_s;
typedef stream<my_data_out_type> my_data_out_type_s;
/*
struct Data {
	// egyben kezeli a struktúrát. egy for ciklust meg tudok spórolni
	// minden konvolúció előtt definiálni kell a méretét
	// mindig meg kell adni a méreteket... Kipróbálni classal???
	size_t size;
	my_data_type *field;
	Data(size_t size): size(size), field(new my_data_type[size-1]){}
};
*/
void conv(
		hls::stream<ap_axiu<4*sizeof(my_data_type)*8,1,1,1> > &x_in, hls::stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > &x_out, stream<my_templ_type> &t_in1, stream<my_templ_type> &t_in2, int tload);
void conv_alt(
		hls::stream<ap_axiu<4*sizeof(my_data_type)*8,1,1,1> > &x_in, hls::stream<ap_axiu<8*sizeof(my_data_type)*8,1,1,1> > &x_out, stream<my_templ_type> &t_in1, int tload);
void pool(
		hls::stream<ap_axiu<1*sizeof(my_data_type)*8,1,1,1> > &x_in, hls::stream<ap_axiu<1*sizeof(my_data_type)*8,1,1,1> > &x_out);

void convolution_template(
		hls::stream< my_data_type[] > &input,
		hls::stream< my_data_type[] > &output,
		hls::stream< my_templ_type[] > &templ,
		hls::stream< my_templ_type[] > &templ_b);

void pooling_template(
		hls::stream< my_data_type [] > &input,
		hls::stream< my_data_type []> &output);

void fully_connected_template(
		hls::stream<my_data_type> &input,
		hls::stream<my_data_type> &output);

void relu_template(
		hls::stream< my_data_type > &input,
		hls::stream< my_data_type > &output);

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
					{my_data_type
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

template<int IN_WIDTH, int IN_HEIGHT, int IN_DEPTH, int TEMPLATE_SIZE> inline void convolution_template(
		//hls::stream<ap_uint< sizeof(my_data_typemy_data_type)*8*IN_DEPTH> >input,  // ez biztos hogy jó. próbáljuk ki tömbbel...
		hls::stream< my_data_type[IN_DEPTH] > &input,
		hls::stream< my_data_type[IN_DEPTH] > &output,
		hls::stream< my_templ_type[IN_DEPTH] > &templ,
		hls::stream< my_templ_type[IN_DEPTH] > &templ_b)
{
	my_data_type image[IN_WIDTH][TEMPLATE_SIZE-1][IN_DEPTH]; //teljes két sort tartalmazza
	my_data_type image_register = templ.read();
	//my_data_type mixer[TEMPLATE_SIZE][TEMPLATE_SIZE][IN_DEPTH];

	my_data_type weigts[TEMPLATE_SIZE][TEMPLATE_SIZE][IN_DEPTH];
	my_data_type bias[IN_DEPTH] = templ_b.read();

	int actual=0;
/// ellenőrizni kellene hogy mindent jó helyről szedek-e...
	FOR_IN_HEIGHT: for (int i = 0; i < IN_HEIGHT; i++) {
		FOR_IN_WIDTH: for (int j = 0; j < IN_WIDTH; j++) {

			for(int i=0; i<TEMPLATE_SIZE;i++){
				for(int j=0; j<TEMPLATE_SIZE-1;j++){
					for(int h=0;h<IN_DEPTH;h++){
						weigts[i][j][h] = weigts[i][j+1][h];
					}
				}
				for(int j=0;j<TEMPLATE_SIZE;j++){
					weigts[i][j][TEMPLATE_SIZE-1] = image[i][actual][TEMPLATE_SIZE-1];
				}
			}
			for(int i=0;i<TEMPLATE_SIZE-1;i++){
				for(int h=0;h<IN_DEPTH;h++){
					image[0][i][h] = weigts[TEMPLATE_SIZE-1][i][h];
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
	}
}

template<int IN_WIDTH, int IN_HEIGHT, int IN_DEPTH, int POOL_SIZE>inline void pooling_template(
		hls::stream< my_data_type [IN_DEPTH] > &input,
		hls::stream< my_data_type [IN_DEPTH]> &output)
{
	my_data_type image[IN_WIDTH][POOL_SIZE-1][IN_DEPTH];
	my_data_type window[POOL_SIZE][POOL_SIZE][IN_DEPTH];

	FOR_IN_HEIGHT: for (int i = 0; i<IN_HEIGHT; i++) {
		FOR_IN_WIDTH: for (int j = 0; j<IN_WIDTH; j++) {

			for(int p=0;p<POOL_SIZE-1;p++){
				image[0][p] = window[i][p];
				window[i][p] = window[i][p+1];
			}

			image[IN_WIDTH][POOL_SIZE-1] = input.read();

			my_data_type max = 0;
			for(int i=0;i<POOL_SIZE;i++){
				for(int j=0;j<POOL_SIZE;j++){
					for(int h=0;h<IN_DEPTH;h++){
						if(window[i][j][h]>max){
							max = window[i][j][h];
						}
					}
				}
			}

			output.write(max);
		}
	}
}


template<int IN_SIZE, int OUT_SIZE> inline void fully_connected_template(
		hls::stream<my_data_type> &input,
		hls::stream<my_data_type> &output)
{
	my_templ_type weights[IN_SIZE][OUT_SIZE];
	my_templ_type bias[OUT_SIZE];

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


// ReLu réteg a nagyobb modulatitás elérés érdekében... (jobban megértettem a működést.)
template<int SIZE> inline void relu_template(
	hls::stream< my_data_type > &input,
	hls::stream< my_data_type > &output)
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
