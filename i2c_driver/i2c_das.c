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
#include <assert.h>
#include <string.h>

//#include "i2c.h"
#include "i2c_devices.h"
#include "timestamp.h"
#include "i2c_das.h"
#include "macros.h"






/**
 * Initialises the das device
 *
 * @param[in] dev memory structure of das device
 */
int i2c_das_init(i2c_dev *dev) {
	dev->type = das;

	i2c_das_data *data = malloc(sizeof(*data));
	if (data == NULL) {
		printf("ERROR: Could not allocate memory for das data\n");
		return -1;
	}

	dev->data = data;

	int i;
	for (i = 0; i < I2C_DAS_CHANNEL_COUNT; i++) {
		data->channels[i] = i2c_das_channel_data_DEFAULT;

		timestamp_get(&(data->channels[i].timestamp));
	}

	return 0;
}


/**
 * Sets the parameters of a das channel
 *
 * @param[in] channel_data memory structure of channel data
 * @param[in] enabled      
 *   - 0: disabled
 *   - 1: enabled
 * @param[in] range
 *   - 0: 0 to 5v
 *   - 1: 0 to 10v
 * @param[in] bipolar
 *   - 0: 0 to +V
 *   - 1: -V to +V
 * @param[in] name         name of the channel
 */
int i2c_das_channel_set(i2c_das_channel_data *channel_data,
												uint8_t enabled,
												uint8_t range,
												uint8_t bipolar,
												char *name) {
	channel_data->enabled = enabled;
	channel_data->range   = range;
	channel_data->bipolar = bipolar;
	channel_data->name    = strdup(name);

	return 0;
}




/**
 * Reads a channel on the das and stores the value read in milivolts
 *
 * @param[in] dev     memory structure of das device
 * @param[in] channel channel of the das to read
 */
int i2c_das_read(i2c_dev *dev, uint8_t channel) {
	assert(dev->type == das);

	i2c_das_data *data = ((i2c_das_data*) dev->data);

	if ((channel < 0) || (channel > I2C_DAS_CHANNEL_COUNT)) {
		printf("channel out of range.\n");
		return -1;
	}

	i2c_das_channel_data *channel_data = data->channels + channel;

	if (!(channel_data->enabled)) {
		printf("channel disabled.\n");
		return -1;
	}


	uint8_t control;
	//control = 0x8C; //channel 0, fullrange bypolar, no powerdown.
	//control = 0x80; //channel 0, 0v-5v, no powerdown.

	control = ( ((1                     & 0x1) << 7) |
	            ((channel               & 0x7) << 4) |
	            ((channel_data->range   & 0x1) << 3) |
	            ((channel_data->bipolar & 0x1) << 2) |
	            ((data->powerdown       & 0x3) << 0) );

	//DAS needs separate write and read cycles to initiate conversion.
	uint8_t buf[2];
	DO_AND_CHECK(i2c_devices_write(dev,&control, 1));
	DO_AND_CHECK(i2c_devices_read(dev, buf, 2));

	//dacval is 8+4 bits padded with 4 0's on the right.
  uint16_t dacval = ((buf[0] & 0xFF) << 4) | ((buf[1] & 0xF0) >> 4);

	DO_AND_CHECK(i2c_das_dacval_to_mv(channel_data, dacval));

	return 0;
}




/**
 * Converts the bytes read from the das to milivolts according to the channel settings
 *
 * @param[in] channel_data memory structure of das device
 * @param[in] dacval       bytes received from the as
 */
int i2c_das_dacval_to_mv(i2c_das_channel_data *channel_data, uint16_t dacval) {
  int32_t microvolts;
  int32_t microvolt_per_bit;
  int32_t microvolt_offset;
	uint8_t maxval;
	int32_t microvolt_maxval;
	uint16_t steps;

	steps = 1 << I2C_DAS_BITS;

	if (dacval >= steps) {
		printf("dacval out of range. steps: %u, dacval: %u\n", steps, dacval);
		return -1;
	}

	maxval = channel_data->range ? I2C_DAS_RANGE_HIGH_MAX : I2C_DAS_RANGE_LOW_MAX;
	microvolt_maxval = I2C_DAS_V_TO_UV(maxval);
	microvolt_offset = channel_data->bipolar ? (-1 * microvolt_maxval) : 0;

  microvolt_per_bit = (microvolt_maxval - microvolt_offset) / (steps - 1);

	microvolts = (dacval * microvolt_per_bit) + microvolt_offset;

	channel_data->milivolts = I2C_DAS_UV_TO_MV(microvolts);
	return 0;
}




/**
 * Reports the das channel name and read voltage
 *
 * @param[in] dev     memory structure of das device
 * @param[in] channel channel number to report
 */
int i2c_das_report(i2c_dev *dev, uint8_t channel) {
	assert(dev->type == das);

	i2c_das_channel_data *channel_data;
	DO_AND_CHECK(i2c_das_get_channel_data(dev, channel, &channel_data));

  printf("DAS channel %d named %-12.12s has voltage: %6.3fV\n", channel, channel_data->name, (channel_data->milivolts / 1000.0));

  return 0;
}




/**
 * Reports all channel on a das
 *
 * @param[in] dev memory structure of das device
 */
int i2c_das_report_all(i2c_dev *dev) {
	assert(dev->type == das);

	DO_AND_CHECK(i2c_das_do_on_channels(dev, i2c_das_report));

  return 0;
}




/**
 * Calls a function on all channel of the das
 * Skips unset channels, verified by a set name
 *
 * @param[in] dev  memory structure of das device
 * @param[in] func function to call
 */
int i2c_das_do_on_channels(i2c_dev *dev, int (*func)(i2c_dev*, uint8_t)) {
	assert(dev->type == das);

	i2c_das_data *data = ((i2c_das_data*) dev->data);

	int i;
	for (i = 0; i < I2C_DAS_CHANNEL_COUNT; i++) {
		i2c_das_channel_data *channel_data = data->channels + i;
		if (channel_data->enabled) {
			DO_AND_CHECK( (*func)(dev, i) );
		}
	}

	return 0;
}




/**
 * Return the channel data structure of a channel on a das
 *
 * @param[in]  dev              memory structure of das device
 * @param[in]  channel          channel number
 * @param[out] channel_data_ptr channel data structure
 */
int i2c_das_get_channel_data(i2c_dev *dev, uint8_t channel, i2c_das_channel_data **channel_data_ptr) {
	assert(dev->type == das);

	i2c_das_data *data = ((i2c_das_data*) dev->data);
	*channel_data_ptr = data->channels + channel;

	return 0;
}



/**
 * Polls a das channel to update the read voltage
 * This function is rate limited
 *
 * @param[in] dev     memory structure of das device
 * @param[in] channel channel number
 */
int i2c_das_poll_channel(i2c_dev *dev, uint8_t channel) {
	assert(dev->type == das);

	i2c_das_channel_data *channel_data;
	DO_AND_CHECK(i2c_das_get_channel_data(dev, channel, &channel_data));

	//int res = timestamp_compare_offset(&(channel_data->timestamp), &i2c_das_channel_timestamp_offset);
	//printf("result of timestamp compare offset: %d\n", res);
	if (timestamp_compare_offset(&(channel_data->timestamp), &i2c_das_channel_timestamp_offset) >= 0) {
	//if (res >= 0) {
		//last check at least "offset" ago, update.

		//printf("DAS: updating channel %d named %-12.12s\n", channel, channel_data->name);

    DO_AND_CHECK(i2c_das_read(dev, channel));
    DO_AND_CHECK(timestamp_get(&(channel_data->timestamp)));
	}

	return 0;
}




/**
 * Polls all das channels to update the read voltage
 *
 * @param[in] dev     memory structure of das device
 */
int i2c_das_poll(i2c_dev *dev) {
	assert(dev->type == das);

	DO_AND_CHECK(i2c_das_do_on_channels(dev, i2c_das_poll_channel));

	return 0;
}


















