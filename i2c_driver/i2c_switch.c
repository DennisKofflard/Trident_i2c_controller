/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-11-24
 *
 *---------------------------------------------------------------------------
 */


#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

#include "i2c_lowlevel.h"
#include "i2c_devices.h"
#include "i2c_switch.h"



/**
 * Initialises the i2c_switch device
 *
 * @param[in] dev memory structure of i2c_switch device
 */
int i2c_switch_init(i2c_dev *dev) {
	dev->type = switch_dev;

	i2c_switch_data *data = malloc(sizeof(*data));
	if (data == NULL) {
		printf("ERROR: Could not allocate memory for switch data\n");
		return -1;
	}

	data->state_valid = 0;

	dev->data = data;

	return 0;
}


/**
 * Polls the i2c_switch device
 *
 * @param[in] dev memory structure of i2c_switch device
 */
int i2c_switch_poll(i2c_dev *dev) {
	assert(dev->type == switch_dev);

	//nothing to do here. switch is a static device.

	return 0;
}


/**
 * Dumps debugging data of the i2c_switch device
 *
 * @param[in] dev memory structure of i2c_switch device
 */
int i2c_switch_data_dump(i2c_dev *dev) {
	assert(dev->type == switch_dev);

	i2c_switch_data *data = ((i2c_switch_data*) dev->data);

	printf("data at %p, state valid: %1u, statemask: 0x%02X\n", data, data->state_valid, data->channel_mask);

	return 0;
}



/**
 * Closes all i2c ports of the i2c_switch device
 *
 * @param[in] dev memory structure of i2c_switch device
 */
int i2c_switch_close(i2c_dev *dev) {
	return i2c_switch_set(dev, 0);
}


/**
 * Sets a channel mask on all i2c ports of the i2c_switch device.
 * Caches switchstate ignoring useless writes.
 *
 * @param[in] dev          memory structure of i2c_switch device
 * @param[in] channel_mask bitmask to set channels to
 */
int i2c_switch_set(i2c_dev *dev, uint8_t channel_mask) {
	assert(dev->type == switch_dev);

	i2c_switch_data *data = ((i2c_switch_data*) dev->data);

	if ((data->channel_mask == channel_mask) &&
			(data->state_valid)) {
		//channel mask already set correctly.
		return 0;
	}

	if (i2c_switch_set_stateless(dev, channel_mask)) {
		//setting switch failed.
		data->state_valid = 0;
		return -1;
	}

	data->state_valid = 1;
	data->channel_mask = channel_mask;

	return 0;
}



/**
 * Sets a channel mask on all i2c ports of the i2c_switch device.
 * Does not caches switchstate, used for initialisation.
 *
 * @param[in] dev          memory structure of i2c_switch device
 * @param[in] channel_mask bitmask to set channels to
 */
int i2c_switch_set_stateless(i2c_dev *dev, uint8_t channel_mask) {
	assert(dev->type == switch_dev);

	if (i2c_lowlevel_write_and_check(dev->adress, &channel_mask, 1)) {
		printf("could not set switch.\n");
		return -1;
	}

	return 0;
}


/**
 * Opens a single i2c ports on a i2c_switch device and closes all others.
 * Caches switchstate ignoring useless writes.
 *
 * @param[in] dev     memory structure of i2c_switch device
 * @param[in] channel port to open
 */
int i2c_switch_select(i2c_dev *dev, uint8_t channel) {
	assert(dev->type == switch_dev);

	if (!( (0 <= channel) && (channel <= 7) )) {
		printf("channel must be between 0 and 7\n");
		return -1;
	}

	return i2c_switch_set(dev, (1 << channel));
}
