/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-02-21
 *
 *---------------------------------------------------------------------------
 */




#include <stdio.h>
#include <stdlib.h>
#include <fcgiapp.h>
#include <assert.h>
#include <string.h>

#include "i2c_devices.h"

#include "fcgi_helper.h"
#include "macros.h"

#include "fcgi.h"
#include "fcgi_request.h"
#include "fcgi_request_all.h"

#include "timestamp.h"
#include "i2c_transceiver.h"






int fcgi_request_all_dispatch(FCGX_Request *request, char *url) {
	FCGI_NEXT_PARAM();

	if (fcgi_request_strcmp(url, "dump")) {
		//logging:
		fprintf(stderr, "Dispatching to /all/dump\n");
		return fcgi_request_all_dump(request);
	
	} else {
		printf("command not found.\n");
		return -1;
	}
}



int fcgi_request_all_dump(FCGX_Request *request) {
	//printf("/all/dump here.\n");

	printf("Content-Type: application/json; charset=UTF-8\n\n");

	printf("{\n");
		DO_AND_CHECK(fcgi_request_all_general(request));
		printf(",\n");

		DO_AND_CHECK(fcgi_request_transceivers(request));
		
	printf("\n}");

	return 0;
}



int fcgi_request_all_general(FCGX_Request *request) {
	//more information could be printed here like:
	// trident version
	// device counts by type.

	printf("\"debug\": true");

	return 0;
}





int fcgi_request_transceivers(FCGX_Request *request) {
	printf("\"transceivers\": [\n");
		DO_AND_CHECK(fcgi_i2c_devices_call_on_type(transceiver, request, fcgi_request_transceiver_single));
		printf("\n{\"filler\": null}\n");
	printf("\n]");

	return 0;
}



int fcgi_request_transceiver_single(i2c_dev* dev, FCGX_Request *request) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);



	printf("{\n");
	
	printf("\"connected\": %s,\n", fcgi_request_bit_to_bool(data->connected));
	printf("\"ready\": %s",        fcgi_request_bit_to_bool(data->ready));

	if (data->connected && data->ready) {
		printf(",\n");


		//vendor info
		printf("\"vendor_info\": {"
						"\"name\": \"%16s\", "
						"\"OUI\": \"%5d\", "
						"\"part\": \"%16s\", "
						"\"revision\": \"%2s\", "
						"\"serial\": \"%16s\"},\n",
			     data->vendor_info.name,
			     data->vendor_info.OUI,
			     data->vendor_info.part_number,
			     data->vendor_info.revision,
				   data->vendor_info.serial);
	




		//voltages
		printf("\"VDD33_TX\": %01d.%04d,\n", (data->voltage.TX) / 10000, (data->voltage.TX) % 10000);
		printf("\"VDD33_RX\": %01d.%04d,\n",    (data->voltage.RX) / 10000, (data->voltage.RX) % 10000);






		//temperatures
		int16_t temp_c[4];
		uint16_t temp_mc[4];

		DO_AND_CHECK(i2c_transceiver_temperature_to_c_and_mc(data->temperature.TX1, &(temp_c[0]), &(temp_mc[0])));
		DO_AND_CHECK(i2c_transceiver_temperature_to_c_and_mc(data->temperature.TX2, &(temp_c[1]), &(temp_mc[1])));
		DO_AND_CHECK(i2c_transceiver_temperature_to_c_and_mc(data->temperature.RX1, &(temp_c[2]), &(temp_mc[2])));
		DO_AND_CHECK(i2c_transceiver_temperature_to_c_and_mc(data->temperature.RX2, &(temp_c[3]), &(temp_mc[3])));

		printf("\"temp_TX1\": %3d.%03d,\n", temp_c[0], temp_mc[0]);
		printf("\"temp_TX2\": %3d.%03d,\n", temp_c[1], temp_mc[1]);
		printf("\"temp_RX1\": %3d.%03d,\n", temp_c[2], temp_mc[2]);
		printf("\"temp_RX2\": %3d.%03d,\n", temp_c[3], temp_mc[3]);






		//channel data
		printf("\"channels\": [\n");

		int i;
		for (i = 0; i < I2C_TRANSCEIVER_CHANNEL_COUNT; i++) {
			if (i != 0) {
				printf(",\n");
			}
			
			printf("{\"channel\": %d, ", i);

			printf("\"TX_enable_channel\": %s, ", 
						fcgi_request_bit_to_bool(
							i2c_transceiver_bit_get(
								data->data_tx.enable_channel, i)));

			printf("\"TX_enable_output\": %s, ", 
						fcgi_request_bit_to_bool(
							i2c_transceiver_bit_get(
								data->data_tx.enable_output, i)));

			printf("\"RX_enable_channel\": %s, ", 
						fcgi_request_bit_to_bool(
							i2c_transceiver_bit_get(
								data->data_rx.enable_channel, i)));

			printf("\"RX_enable_output\": %s, ", 
						fcgi_request_bit_to_bool(
							i2c_transceiver_bit_get(
								data->data_rx.enable_output, i)));

			printf("\"RX_LOS\": %s}",
						fcgi_request_bit_to_bool(
							i2c_transceiver_bit_get(
								data->data_rx.loss_of_signal, i)));
		}
		printf("\n]\n");


	}

	printf("\n},");

	return 0;
}



























