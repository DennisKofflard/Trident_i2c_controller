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

#include "i2c_switch.h"
#include "i2c_psu.h"
#include "i2c_psu_report.h"
#include "i2c_das.h"
//#include "i2c_gpio.h"
#include "i2c_gpio_trident1.h"
#include "i2c_gpio_trident2.h"
#include "i2c_transceiver.h"

#include "reports.h"
#include "macros.h"



extern uint32_t poll_counter;
extern pthread_mutex_t  mutex_data_usage;






int report_all_wrapper() {
	static int report_number = 1;

	struct timespec t_start, t_end;
	clock_gettime(CLOCK_MONOTONIC, &t_start);

	pthread_mutex_lock(&mutex_data_usage);
		if(report_all()) {
			fprintf(stderr, "Reporting failed.\n");
			pthread_mutex_unlock(&mutex_data_usage);
			return -1;
		}
	pthread_mutex_unlock(&mutex_data_usage);


	printf("\n");
	printf("report number: %8d\n", report_number++);


	clock_gettime(CLOCK_MONOTONIC, &t_end);
	unsigned long time_us =
	((t_end.tv_sec  - t_start.tv_sec)  * (1000*1000)) +
	((t_end.tv_nsec - t_start.tv_nsec) / (     1000));

	printf("\n");
	printf("reporting took: %6ldus\n", time_us);

	return 0;
}



int report_all() {
	printf("Device structure:\n");
	DO_AND_CHECK(i2c_devices_dev_array_dump());
	printf("\n");

	printf("DAS reports:\n");
	DO_AND_CHECK(i2c_devices_call_on_type(das,  i2c_das_report_all));
	printf("\n");

	printf("GPIO reports: \n");
	DO_AND_CHECK(i2c_devices_call_on_type(gpio_trident1, i2c_gpio_trident1_report_all));
	DO_AND_CHECK(i2c_devices_call_on_type(gpio_trident2, i2c_gpio_trident2_report_all));
	printf("\n");

	printf("TRANSCEIVER reports: \n");
	DO_AND_CHECK(i2c_devices_call_on_type(transceiver, i2c_transceiver_report_all));
	printf("\n");

	printf("PSU reports: \n");
	//DO_AND_CHECK(i2c_devices_call_on_type(psu, i2c_psu_report_all));
	DO_AND_CHECK(i2c_devices_call_on_type(psu, psu_reports));
	printf("\n");

	return 0;
}



int psu_reports(i2c_dev *dev) {
	DO_AND_CHECK(i2c_psu_voltage_report(dev));
	DO_AND_CHECK(i2c_psu_current_report(dev));
	DO_AND_CHECK(i2c_psu_power_report(dev));

	return 0;
}



