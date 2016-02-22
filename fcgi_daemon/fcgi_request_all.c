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

	printf("{\n");
		DO_AND_CHECK(fcgi_request_all_general(request));

		printf("transceivers: {\n");
			DO_AND_CHECK(fcgi_i2c_devices_call_on_type(transceiver, request, fcgi_request_all_transceiver));
		printf("}\n");
		
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



int fcgi_request_all_transceiver(i2c_dev* dev, FCGX_Request *request) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	printf("    transceiver voltages: TX: %4d.%01d mV, RX: %4d.%01d mV\n",
					  (data->voltage.TX) / 10, (data->voltage.TX) % 10,
					  (data->voltage.RX) / 10, (data->voltage.RX) % 10);


	return 0;
}





