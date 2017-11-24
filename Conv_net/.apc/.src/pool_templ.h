#include <hls_stream.h>
#include <ap_int.h>
#include <ap_axi_sdata.h>

using namespace hls;

typedef short my_data_type;

template<int IN_WIDTH, int IN_HEIGHT, int IN_DEPTH, int POOL_SIZE>inline void pooling_template(
		hls::stream< ap_uint< IN_DEPTH*sizeof(my_data_type)*8> > &input,
		hls::stream< ap_uint< IN_DEPTH*sizeof(my_data_type)*8> > &output
		)

/**
 * A template továbbtöltése miatt szükséges hogy minden rétegen átmenjen.
 */
//		hls::stream< My_Temlpate_Struct > &weight_in,
//		hls::stream< My_Temlpate_Struct > &weight_out,
//		int template_load
{
	my_data_type memory[IN_WIDTH/POOL_SIZE][IN_DEPTH];
	ap_uint< IN_DEPTH*sizeof(my_data_type)*8> input_tmp;
	my_data_type image_register[IN_DEPTH];

	int pixel_count;

	/*
	 * A memória melyik helyére írunk.
	 */
	int address=0;
	/*
	 * A teljes bemenet bejárása.
	 */
	for(int h=0;h<IN_HEIGHT;h++){
		for(int w=0;w<IN_WIDTH;w++){

			pixel_count=IN_WIDTH*h+w;

			input_tmp = input.read();
			for(int i=0;i<IN_DEPTH;i++){
				//Pipeline miatt itt jó?
				/**
				 * ap_uint-ből tömbbe olvassuk az adatokat.
				 */
				image_register[i]=input_tmp((i+1)*sizeof(my_data_type)*8-1,i*sizeof(my_data_type)*8);
				if(w%POOL_SIZE==0){
					/**
					 * Amennyiben az első sor első oszlopában vagyunk a maximum érték 0.
					 * A memóriába 0 beírása helyett az addigi memória tartalmat felülírjuk az új értékkel.
					 * Amennyiben az 0, elértük a célunkat.
					 * Amennyiben az nagyobb mint 0 a következő lépésben amúgy is felülírtuk volna ezzel az értékkel.
					 * Kisebb nem lehet a beépített relu réteg miatt.
					 */
					if(h%POOL_SIZE==0){
						memory[address][i] =image_register[i];
					}
					else{
						if(image_register[i] > memory[address][i]){
							memory[address][i] = image_register[i];
						}
					}
				}
				/**
				 * Minden más esetben ha nagyobb felülírjuk ha nem akkor meghagyjuk.
				 */
				else {
					if(image_register[i] > memory[address][i]){
						memory[address][i] = image_register[i];
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
				if(h%POOL_SIZE == POOL_SIZE-1 && w%POOL_SIZE == POOL_SIZE-1){
					ap_uint<IN_DEPTH*sizeof(my_data_type)*8> output_tmp;
					for(int h=0;h<IN_DEPTH;h++){
						output_tmp((i+1)*sizeof(my_data_type)*8-1,i*sizeof(my_data_type)*8)=memory[address][i];
					}
					if(i==IN_DEPTH-1){
						output.write(output_tmp);
					}
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
}
