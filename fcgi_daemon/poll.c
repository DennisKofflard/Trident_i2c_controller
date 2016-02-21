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

#include "i2c_devices.h"
#include "sleepdelay.h"
#include "macros.h"

#include "i2c_switch.h"
#include "i2c_psu.h"
#include "i2c_psu_report.h"
#include "i2c_das.h"
//#include "i2c_gpio.h"
#include "i2c_gpio_trident1.h"
#include "i2c_gpio_trident2.h"
#include "i2c_transceiver.h"



pthread_t pthread_thread_poll;
pthread_mutex_t  mutex_data_usage    = PTHREAD_MUTEX_INITIALIZER;


uint32_t poll_counter = 0;



int poll_all() {
	DO_AND_CHECK(i2c_devices_call_on_type(switch_dev,    i2c_switch_poll));
	DO_AND_CHECK(i2c_devices_call_on_type(das,           i2c_das_poll   ));
	DO_AND_CHECK(i2c_devices_call_on_type(psu,           i2c_psu_poll   ));
	DO_AND_CHECK(i2c_devices_call_on_type(gpio_trident1, i2c_gpio_trident1_poll));
	DO_AND_CHECK(i2c_devices_call_on_type(gpio_trident2, i2c_gpio_trident2_poll));
	DO_AND_CHECK(i2c_devices_call_on_type(transceiver,           i2c_transceiver_poll   ));

	poll_counter++;

	return 0;
}



void *thread_poll_wrapper(void *data_ptr) {
	int retval = 0;

	while (retval == 0) {
		pthread_mutex_lock(&mutex_data_usage);
			retval = poll_all();
		pthread_mutex_unlock(&mutex_data_usage);

		sleepdelay_ms(10);
	}

	fprintf(stderr, "ERROR: polling failed! stopping thread.\n");
	assert("POLLING FAILED" == NULL);

	return NULL;
}

void thread_poll_start() {
	if (pthread_create(&pthread_thread_poll, NULL, thread_poll_wrapper, NULL) != 0) {
		fprintf(stderr, "ERROR: could not start polling thread.\n");
		exit(-1);
	}
}


