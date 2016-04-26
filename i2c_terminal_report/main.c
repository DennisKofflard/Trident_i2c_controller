/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-11-24
 *
 *---------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include <pthread.h>

#include "i2c_devices.h"
#include "i2c_switch.h"
#include "i2c_psu.h"
#include "i2c_psu_report.h"
#include "i2c_das.h"
#include "macros.h"
#include "sleepdelay.h"
#include "i2c_trident1.h"
#include "i2c_trident2.h"
#include "i2c_transceiver.h"
#include "i2c_eeprom.h"
#include "i2c_gpio_trident1.h"
#include "i2c_gpio_trident2.h"



pthread_t pthread_thread_poll;
pthread_mutex_t  mutex_data_usage    = PTHREAD_MUTEX_INITIALIZER;
uint32_t poll_counter = 0;








void terminal_clear() {
	//ANCI excape sequence
	//clear screen
	printf("\033[2J");

	//set cursor to 1,1
	printf("\033[1;1H");
}

void terminal_home() {
	//ANCI excape sequence
	//set cursor to 1,1
	printf("\033[1;1H");
}






int poll_all() {
	DO_AND_CHECK(i2c_devices_call_on_type(switch_dev,    i2c_switch_poll));
	DO_AND_CHECK(i2c_devices_call_on_type(das,           i2c_das_poll   ));
	DO_AND_CHECK(i2c_devices_call_on_type(psu,           i2c_psu_poll   ));
	DO_AND_CHECK(i2c_devices_call_on_type(gpio_trident1, i2c_gpio_trident1_poll));
	DO_AND_CHECK(i2c_devices_call_on_type(gpio_trident2, i2c_gpio_trident2_poll));
	DO_AND_CHECK(i2c_devices_call_on_type(transceiver,           i2c_transceiver_poll   ));
	DO_AND_CHECK(i2c_devices_call_on_type(eeprom,        i2c_eeprom_poll   ));

	poll_counter++;

	return 0;
}


int report_all() {
	/*
	//Skipping device memory structure because the screen is full.
	printf("Device structure:\n");
	DO_AND_CHECK(i2c_devices_dev_array_dump());
	printf("\n");
	*/

	printf("DAS reports:\n");
	DO_AND_CHECK(i2c_devices_call_on_type(das,          i2c_das_report_all));
	printf("\n");

	printf("GPIO reports: \n");
	DO_AND_CHECK(i2c_devices_call_on_type(gpio_trident1, i2c_gpio_trident1_report_all));
	DO_AND_CHECK(i2c_devices_call_on_type(gpio_trident2, i2c_gpio_trident2_report_all));
	printf("\n");

	printf("TRANSCEIVER reports: \n");
	DO_AND_CHECK(i2c_devices_call_on_type(transceiver,          i2c_transceiver_report_all));
	printf("\n");

	//printf("PSU reports: \n");
	//DO_AND_CHECK(i2c_devices_call_on_type(psu, i2c_psu_report_all));
		//DO_AND_CHECK(i2c_devices_call_on_type(psu, psu_reports));
	//printf("\n");

	printf("EEPROM reports: \n");
	DO_AND_CHECK(i2c_devices_call_on_type(eeprom,       i2c_eeprom_report_page_zero));
	printf("\n");

	return 0;
}








int psu_reports(i2c_dev *dev) {
	DO_AND_CHECK(i2c_psu_voltage_report(dev));
	DO_AND_CHECK(i2c_psu_current_report(dev));
	DO_AND_CHECK(i2c_psu_power_report(dev));

	return 0;
}









int report_watch() {
	int report_number = 0;
	terminal_clear();

	for (;;) {
    struct timespec t_start, t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_start);

		terminal_home();


		//done in separate thread.
		//DO_AND_CHECK(poll_all());

		pthread_mutex_lock(&mutex_data_usage);
			if(report_all()) {
				fprintf(stderr, "Reporting failed.\n");
				pthread_mutex_unlock(&mutex_data_usage);
				return -1;
			}
		pthread_mutex_unlock(&mutex_data_usage);


		printf("\n");
		printf("poll   number: %8d\n", poll_counter);
		printf("report number: %8d\n", report_number++);


    clock_gettime(CLOCK_MONOTONIC, &t_end);
    unsigned long time_us =
    ((t_end.tv_sec  - t_start.tv_sec)  * (1000*1000)) +
		((t_end.tv_nsec - t_start.tv_nsec) / (     1000));

		printf("\n");
		printf("reporting took: %6ldus\n", time_us);


		sleepdelay_ms(50);
	}

	return 0;
}








void *thread_poll_wrapper(void *data_ptr) {
	int retval = 0;

	while (retval == 0) {
		pthread_mutex_lock(&mutex_data_usage);
			retval = poll_all();
		pthread_mutex_unlock(&mutex_data_usage);

		//sleepdelay_ms(10);
		sleepdelay_ms(20);
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





void timespec_tests() {

	timespec A={1,1};
	timespec B={1,0};
	timespec C={0,5};
	timespec R;

	timestamp_subtract(&A, &A, &R);
	fprintf(stderr, "timestamp: %5lds, %9ldns\n", R.tv_sec,                         R.tv_nsec);

	timestamp_subtract(&A, &B, &R);
	fprintf(stderr, "timestamp: %5lds, %9ldns\n", R.tv_sec,                         R.tv_nsec);
	
	timestamp_subtract(&A, &C, &R);
	fprintf(stderr, "timestamp: %5lds, %9ldns\n", R.tv_sec,                         R.tv_nsec);



	timespec P={2, 1000};
	timespec X={0,  100};
	timespec Y={1,    0};
	timespec Z={2,    0};
	timespec x={0,11000};

	timestamp_subtract(&P, &X, &R);
	fprintf(stderr, "timestamp: %5lds, %9ldns\n", R.tv_sec,                         R.tv_nsec);

	timestamp_subtract(&P, &Y, &R);
	fprintf(stderr, "timestamp: %5lds, %9ldns\n", R.tv_sec,                         R.tv_nsec);

	timestamp_subtract(&P, &Z, &R);
	fprintf(stderr, "timestamp: %5lds, %9ldns\n", R.tv_sec,                         R.tv_nsec);

	timestamp_subtract(&P, &x, &R);
	fprintf(stderr, "timestamp: %5lds, %9ldns\n", R.tv_sec,                         R.tv_nsec);



	int a,b;

	a=7; b=3;
	fprintf(stderr, "a%%b: %3d, a/b: %3d\n", a%b, a/b);
	
	a=-7; b=3;
	fprintf(stderr, "a%%b: %3d, a/b: %3d\n", a%b, a/b);

	a=-1; b=3;
	fprintf(stderr, "a%%b: %3d, a/b: %3d\n", a%b, a/b);



}

void print_help() {
	printf("please identify the switch version as a command line argument\n");
	printf("valid identifiers are:\n"
					"  1\n"
					"  2\n"
					"  tor1\n"
					"  tor2\n"
					"  trident1\n"
					"  trident2\n");
}



int main(int argc, char* argv[]) {
	int (*switch_init)();

  printf("starting.\n");

	if (argc != 2) {
		print_help();
		return -1;
	}

	if ((!strcmp(argv[1], "1")) ||
			(!strcmp(argv[1], "tor1")) ||
			(!strcmp(argv[1], "trident1"))) {
		switch_init = trident1_init;
	} else if ((!strcmp(argv[1], "2")) ||
					 	 (!strcmp(argv[1], "tor2")) ||
						 (!strcmp(argv[1], "trident2"))) {
		switch_init = trident2_init;
	} else {
		print_help();
		return -1;
	}


	if (i2c_devices_init()){
		printf("ERROR: init failed!\n");
		return -1;
	}

	if ((*switch_init)()) {
		printf("Could not initialize switch\n");
		return -1;
	}

	
	i2c_dev *dev;
	if (i2c_devices_dev_get(&dev, transceiver, 0) ||
			i2c_transceiver_enable_all(dev, 1) ||
			i2c_transceiver_poll_enable(dev) ) {
		fprintf(stderr, "transceiver setting settings failed.\n");
	}
		fprintf(stderr, "test\n");

	thread_poll_start();

	DO_AND_CHECK(report_watch());


	printf("succes\n");
  return 0;

	//timespec_tests();

	return 0;
}











