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

template<int IN_WIDTH, int IN_HEIGHT, int IN_DEPTH, int TEMPLATE_SIZE, int OUT_DEPTH> inline void convolution_template(
		hls::stream< ap_uint< IN_DEPTH*sizeof(my_data_type)*8> > &input,
		hls::stream< ap_uint< OUT_DEPTH*sizeof(my_data_type)*8> > &output,
		hls::stream< My_Temlpate_Struct > &templ_in,
		hls::stream< My_Temlpate_Struct > &templ_out,
		int template_load)
{
	my_data_type image[IN_WIDTH][TEMPLATE_SIZE-1][IN_DEPTH];
	my_data_type image_register[IN_DEPTH];
	my_templ_type weigts[TEMPLATE_SIZE][TEMPLATE_SIZE][IN_DEPTH][OUT_DEPTH];
	my_templ_type bias[OUT_DEPTH];
	ap_uint<IN_DEPTH*sizeof(my_data_type)*8> input_tmp;
	ap_uint<OUT_DEPTH*sizeof(my_data_type)*8> output_tmp;
	My_Temlpate_Struct temp;

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
						temp = templ_in.read();
						if(temp.last == 0) weigts[i][j][h][o] = temp.weight;
					}
				}

			}
		}

		for(int o=0;o<OUT_DEPTH;o++){
			temp = templ_in.read();
			if(temp.last == 0) bias[o] = temp.weight;
		}
		bool exit = false;
		while(!exit){
			if(templ_in.empty()) break;
			My_Temlpate_Struct temp = templ_in.read();
			templ_out.write(temp);
			if(temp.last) {
				exit = true;
			}
		}
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
			int sum[IN_DEPTH];

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

							ap_int<16> tmp_image = image[read_address][th][h];
							ap_int<16> tmp_weight = weigts[tw][th][h][o];

							if(tw == 0 and th == 0 and h==0){
								/**
								 * A bias az accumlator kezdeti értéke.
								 */
								sum[o] = bias[o] + tmp_image * tmp_weight;
							}
							else {
								sum[o] += tmp_image * tmp_weight;
							}
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
				if(i==0 || j==0 || (i==IN_HEIGHT-1) || (j==IN_WIDTH-1) ){
					output_tmp = 0;
				}
				else{
					for(int o=0;o<OUT_DEPTH;o++){
						output_tmp((o+1)*sizeof(my_data_type)*8-1,o*sizeof(my_data_type)*8)=(sum[o] >> 15);
					}
				}


				/**
				 * A blokkramban egyel arrébb lépünk.
				 * Ez a tempalte következő oszlopát jelenti.
				 */
				read_address++;

				if(read_address-address == TEMPLATE_SIZE){
					output.write(output_tmp);
				}


				/**
				 * Amikor végére értünk a template-nek.
				 * A memóriában tovább lépünk egyel, a templatet egy pixellel csúsztatjuk.
				 * A végleges megvalósításban itt a lépés méretét kell majd állíthatóvá tenni.
				 */
				if(tw == TEMPLATE_SIZE-1){
					if(address == IN_WIDTH-TEMPLATE_SIZE){
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
}
