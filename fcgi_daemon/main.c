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

#include "i2c_lowlevel.h"
#include "i2c_devices.h"
#include "i2c_switch.h"
#include "i2c_psu.h"
#include "i2c_psu_report.h"
#include "i2c_das.h"
#include "macros.h"
#include "sleepdelay.h"
#include "i2c_trident1.h"
#include "i2c_trident2.h"
#include "i2c_gpio_trident1.h"
#include "i2c_gpio_trident2.h"
#include "i2c_transceiver.h"

#include "poll.h"
#include "fcgi.h"





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

	
	thread_poll_start();

	DO_AND_CHECK(fcgi_start());


	printf("succes\n");
  return 0;
}











