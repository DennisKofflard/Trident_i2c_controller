/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-11
 *
 *---------------------------------------------------------------------------
 */



#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#include <pthread.h>

#include <fcgiapp.h>

#include "i2c_devices.h"
#include "sleepdelay.h"

#include "i2c_switch.h"
#include "i2c_psu.h"
#include "i2c_psu_report.h"
#include "i2c_das.h"
//#include "i2c_gpio.h"
#include "i2c_transceiver.h"

#include "reports.h"
#include "macros.h"


#include "fcgi_helper.h"
#include "fcgi_debug.h"



extern uint32_t poll_counter;
extern pthread_mutex_t  mutex_data_usage;


int fcgi_html_header(FCGX_Request *request) {
	const static char header[] =
		"<html>"
		"  <head>"
		"    <meta http-equiv='refresh' content='1'>"
		"  </head>"
		"  <body>"
		"    <pre>";

	printf(header);

	return 0;
}

int fcgi_html_footer(FCGX_Request *request) {
	const static char footer[] =
		"    </pre>"
		"  </body>"
		"</html>";

	printf(footer);

	return 0;
}





int fcgi_debug_dump(FCGX_Request *request) {
	pthread_mutex_lock(&mutex_data_usage);
		if(fcgi_das(request)) {
			fprintf(stderr, "Reporting failed.\n");
			pthread_mutex_unlock(&mutex_data_usage);
			return -1;
		}
	pthread_mutex_unlock(&mutex_data_usage);

	return 0;
}

int fcgi_das(FCGX_Request *request) {
	//TODO: replace quick fix
	i2c_dev *dev;

	DO_AND_CHECK(i2c_devices_dev_get(&dev, das, 0));

	assert(dev->type == das);

	i2c_das_data *data = ((i2c_das_data*) dev->data);

	int channel;
	for (channel = 0; channel < I2C_DAS_CHANNEL_COUNT; channel++) {
		i2c_das_channel_data *channel_data = data->channels + channel;
		if (channel_data->enabled) {
			//DO_AND_CHECK( (*func)(dev, i) );
			printf("DAS channel %d named %-12.12s has voltage: %6.3fV\n", channel, channel_data->name, (channel_data->milivolts / 1000.0));
		}
	}

	return 0;
}
