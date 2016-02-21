/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-17
 *
 *---------------------------------------------------------------------------
 */


#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>


#include "i2c_devices.h"
#include "i2c_gpio_trident1.h"
#include "macros.h"


/** \file i2c_gpio_trident1.c
 * This is the i2c driver for the trident 1 pcf8574 i2c gpio expander.
 * This driver is very different from the trident 2 gpio driver
 * as the chips are very different and are not unifiable in their operations. 
 */





/**
 * Initialises a trident 1 gpio device
 *
 * @param[in] dev trident 1 gpio device to initialise
 */
int i2c_gpio_trident1_init(i2c_dev *dev) {
	dev->type = gpio_trident1;


	i2c_gpio_trident1_data *data = malloc(sizeof(*data));
	if (data == NULL) {
		printf("ERROR: Could not allocate memory for gpio_trident1 data\n");
		return -1;
	}

	dev->data = data;

	if (i2c_gpio_trident1_read_mask(dev)) {
		fprintf(stderr, "Could not read input mask of gpio_trident1\n");
		return -1;
	}

	return 0;
}



/**
 * Polls a trident 1 gpio device
 *
 * @param[in] dev trident 1 gpio device to poll
 */
int i2c_gpio_trident1_poll(i2c_dev *dev) {
	assert(dev->type == gpio_trident1);

	//nothing to do here.
	//The bitmask is encoded in soldered on resistors.
	//this value should not change

	return 0;
}



/**
 * Reads the resistor configuration identifing this device using GPIO.
 *
 * @param[in] dev trident 1 gpio device
 */
int i2c_gpio_trident1_read_mask(i2c_dev *dev) {
	assert(dev->type == gpio_trident1);

	i2c_gpio_trident1_data *data = dev->data;

	return i2c_devices_read(dev, &(data->input_mask), 1);
}









/**
 * Reports a trident 1 gpio device.
 * Writes the device ID to stdout.
 *
 * @param[in] dev trident 1 gpio device to report
 */
int i2c_gpio_trident1_report_all(i2c_dev *dev) {
	assert(dev->type == gpio_trident1);

	uint8_t id;

	DO_AND_CHECK(i2c_gpio_trident1_get_devid(dev, &id));

	printf("GPIO device ID: 0x%02X\n", id);

  return 0;
}



/**
 * Gets the device ID from a trident 1 gpio device.
 *
 * @param[in]  dev  trident 1 gpio device 
 * @param[out] devid device id
 */
int i2c_gpio_trident1_get_devid(i2c_dev *dev, uint8_t *devid) {
	assert(dev->type == gpio_trident1);

	i2c_gpio_trident1_data *data = dev->data;

	*devid = data->input_mask;

	return 0;
}
