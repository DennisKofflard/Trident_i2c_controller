/*-------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-14
 *
 *---------------------------------------------------------------------------
 */


#include <stdio.h>
#include <stdlib.h>
#include <fcgiapp.h>
#include <assert.h>
#include <string.h>

#include "fcgi_helper.h"
#include "macros.h"

#include "i2c_devices.h"
#include "i2c_das.h"

#include "fcgi.h"
#include "fcgi_request.h"
#include "fcgi_request_das.h"
#include "fcgi_debug.h"





int fcgi_request_das_dispatch(FCGX_Request *request, char *url) {
	FCGI_NEXT_PARAM();

	if (fcgi_request_strcmp(url, "all")) {
		return fcgi_das_all(request);

	} else {
		printf("command not found.\n");
		return -1;
	}
}



int fcgi_das_all(FCGX_Request *request) {
	return fcgi_i2c_das_do_on_channels(request, fcgi_das_single_output);
}




int fcgi_das_single_output(i2c_dev* dev, FCGX_Request *request, uint8_t channel) {
	assert(dev->type == das);

	i2c_das_channel_data *channel_data;
	DO_AND_CHECK(i2c_das_get_channel_data(dev, channel, &channel_data));

	printf("%s:%d\n", channel_data->name, channel_data->milivolts);

	return 0;
}





int fcgi_i2c_das_do_on_channels(FCGX_Request *request,
																int (*func)(i2c_dev*, FCGX_Request*, uint8_t)) {
	int idev = 0;
	i2c_dev *dev;

	while (!(i2c_devices_dev_get(&dev, das, idev))) {
		assert(dev->type == das);

		i2c_das_data *data = ((i2c_das_data*) dev->data);

		int i;
		for (i = 0; i < I2C_DAS_CHANNEL_COUNT; i++) {
			i2c_das_channel_data *channel_data = data->channels + i;
			if (channel_data->enabled) {
				DO_AND_CHECK( (*func)(dev, request, i) );
			}
		}

		idev++;
	}

	return 0;
}

