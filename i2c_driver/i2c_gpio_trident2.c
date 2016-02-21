/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-04
 *
 *---------------------------------------------------------------------------
 */



/** \file i2c_gpio_trident2.c
 * This is the i2c driver for the trident 2 TCA9555 i2c gpio expander.
 * This driver is very different from the trident 1 gpio driver
 * as the chips are very different and are not unifiable in their operations. 
 */


 

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>


#include "i2c_devices.h"
#include "i2c_gpio_trident2.h"
#include "macros.h"
#include "i2c_trident2.h" //used to check hub cpu enable port




/**
 * Initialises a trident 2 gpio device
 *
 * @param[in] dev trident 2 gpio device to initialise
 */
int i2c_gpio_trident2_init(i2c_dev *dev) {
	dev->type = gpio_trident2;

	i2c_gpio_trident2_data *data = malloc(sizeof(*data));
	if (data == NULL) {
		printf("ERROR: Could not allocate memory for gpio data\n");
		return -1;
	}

	dev->data = data;

	int bank;
	for (bank = 0; bank < I2C_GPIO_BANKS; bank++) {
		int channel;
		for (channel = 0; channel < I2C_GPIO_CHANNELS_PER_BANK; channel++) {
			(data->bank[bank]).name[channel] = NULL;
		}

		DO_AND_CHECK(i2c_gpio_trident2_inout_mask_get(dev, bank));
		DO_AND_CHECK(i2c_gpio_trident2_output_mask_get(dev, bank));
	}

	//read devID resistor configuration
	DO_AND_CHECK(i2c_gpio_trident2_input_mask_get(dev, 0));

	return 0;
}



/**
 * Polls a trident 2 gpio device
 *
 * @param[in] dev trident 2 gpio device to poll
 */
int i2c_gpio_trident2_poll(i2c_dev *dev) {
	assert(dev->type == gpio_trident2);

	//nothing to do here. gpio is a static device.

	return 0;
}



/**
 * Names a trident 2 gpio inout port
 *
 * @param[in] dev  trident 2 gpio device to name a port on
 * @param[in] port number of the port to set name on
 * @param[in] name name to set
 */
int i2c_gpio_trident2_name_port(i2c_dev *dev, uint8_t port, char *name) {
	i2c_gpio_trident2_data *data;
	DO_AND_CHECK(i2c_gpio_trident2_data_ptr(dev, &data));

	if (port >= I2C_GPIO_PORTS) {
		printf("port %3d outside of range for gpio device\n", port);
		return -1;
	}

	uint8_t bank    = port / I2C_GPIO_CHANNELS_PER_BANK;
	uint8_t channel = port % I2C_GPIO_CHANNELS_PER_BANK;

	data->bank[bank].name[channel] = strdup(name);

	return 0;
}



/**
 * Writes the output settings for a named port.
 *
 * @param[in] dev  trident 2 gpio device to report
 * @param[in] port number of the port to report on
 */
int i2c_gpio_trident2_report(i2c_dev *dev, uint8_t port) {
	i2c_gpio_trident2_data *data;
	DO_AND_CHECK(i2c_gpio_trident2_data_ptr(dev, &data));

	if (port >= I2C_GPIO_PORTS) {
		printf("port %3d outside of range for gpio device\n", port);
		return -1;
	}

	uint8_t bank    = port / I2C_GPIO_CHANNELS_PER_BANK;
	uint8_t channel = port % I2C_GPIO_CHANNELS_PER_BANK;

	char *name = data->bank[bank].name[channel];

	if (name == NULL) {
		printf("port unnamed, not reporting\n");
		return -1;
	}

	uint8_t output_high;
	DO_AND_CHECK(i2c_gpio_trident2_output_get(dev, port, &output_high));
	char *on_off_str = output_high ? "HIGH" : "LOW";

	uint8_t output_enabled;
	DO_AND_CHECK(i2c_gpio_trident2_inout_get(dev, port, &output_enabled));
	char *inout_str = output_enabled ? "ENABLED" : "DISABLED";

	//printf("%-12s is set to %4s, %8s\n", name, on_off_str, inout_str);
	printf("%-12s is %8s, set to %4s, \n", name, inout_str, on_off_str);

	return 0;
}


/**
 * Reports a trident 2 gpio device.
 * Writes the device ID to stdout and the output settings for all names ports.
 *
 * @param[in] dev trident 2 gpio device to report
 */
int i2c_gpio_trident2_report_all(i2c_dev *dev) {
	i2c_gpio_trident2_data *data;
	DO_AND_CHECK(i2c_gpio_trident2_data_ptr(dev, &data));

	uint8_t id;
	DO_AND_CHECK(i2c_gpio_trident2_get_devid(dev, &id));
	printf("GPIO device ID: 0x%02X\n", id);

	int port;
	for (port = 0; port < I2C_GPIO_PORTS; port++) {
		uint8_t bank    = port / I2C_GPIO_CHANNELS_PER_BANK;
		uint8_t channel = port % I2C_GPIO_CHANNELS_PER_BANK;

		char *name = data->bank[bank].name[channel];
		if (name) {
			DO_AND_CHECK(i2c_gpio_trident2_report(dev, port));
		}
	}

	return 0;
}





/*
///
/// DO NOT USE.
/// UNIMPLEMENTED.
/// 
/// @param[in] dev trident 2 gpio device to report
///
int i2c_gpio_trident2_report_all_masks(i2c_dev *dev) {
	i2c_gpio_trident2_data *data;
	DO_AND_CHECK(i2c_gpio_trident2_data_ptr(dev, &data));

	int bank;
	for (bank = 0; bank < I2C_GPIO_BANKS; bank++) {
		//TODO add functionality
		//printf("bank %d reads: 0x%02X\n", data->bank.)
	}

	return 0;
}
*/






/**
 * Writes a bitmask to a trident 2 gpio device.
 * used to write enable/disable and high/low settings for both banks
 *
 * @param[in] dev trident 2 gpio device
 * @param[in] command to use
 * @param[in] mask to write
 */
int i2c_gpio_trident2_mask_write(i2c_dev *dev, uint8_t command, uint8_t mask) {
	uint8_t buf[2];
	buf[0] = command;
	buf[1] = mask;
	return i2c_devices_write(dev, buf, 2);
}


/**
 * Reads a bitmask from a trident 2 gpio device.
 * used to read high/low input from both banks
 *
 * @param[in]  dev trident 2 gpio device
 * @param[in]  command to use
 * @param[out] mask to read
 */
int i2c_gpio_trident2_mask_read(i2c_dev *dev, uint8_t command, uint8_t *mask) {
	DO_AND_CHECK(i2c_devices_write(dev, &command, 1));
	return i2c_devices_read(dev, mask, 1);
}


/**
 * Reads a bitmask from a trident 2 gpio device.
 * used to read high/low input from both banks
 *
 * @param[in]  dev      trident 2 gpio device
 * @param[out] data_ptr data pointer
 */
int i2c_gpio_trident2_data_ptr(i2c_dev *dev, i2c_gpio_trident2_data **data_ptr) {
	assert(dev->type == gpio_trident2);

	*data_ptr = ((i2c_gpio_trident2_data*) dev->data);

	return 0;
}




/**
 * Reads the output enabled/disabled bitmask from the trident 2 gpio device
 *
 * @param[in] dev  trident 2 gpio device
 * @param[in] bank number of the bank to read
 */
int i2c_gpio_trident2_output_mask_get(i2c_dev *dev, uint8_t bank) {
	//TODO, i2c hub cpu shutdown check

	uint8_t command;
	switch (bank) {
		case 0:
			command = I2C_GPIO_COMMAND_OUTPUT_BANK_0;
			break;
		case 1:
			command = I2C_GPIO_COMMAND_OUTPUT_BANK_1;
			break;
		default:
			printf("invalid gpio bank selected\n");
			return -1;
	}

	i2c_gpio_trident2_data *data;
	DO_AND_CHECK(i2c_gpio_trident2_data_ptr(dev, &data));

	return i2c_gpio_trident2_mask_read(dev, command, &(data->bank[bank].mask_high));
}


/**
 * Reads the input/output bitmask from the trident 2 gpio device
 *
 * @param[in] dev  trident 2 gpio device
 * @param[in] bank number of the bank to read
 */
int i2c_gpio_trident2_inout_mask_get(i2c_dev *dev, uint8_t bank) {
	//TODO, i2c hub cpu shutdown check

	uint8_t command;
	switch (bank) {
		case 0:
			command = I2C_GPIO_COMMAND_INOUT_BANK_0;
			break;
		case 1:
			command = I2C_GPIO_COMMAND_INOUT_BANK_1;
			break;
		default:
			printf("invalid gpio bank selected\n");
			return -1;
	}

	i2c_gpio_trident2_data *data;
	DO_AND_CHECK(i2c_gpio_trident2_data_ptr(dev, &data));

	return i2c_gpio_trident2_mask_read(dev, command, &(data->bank[bank].mask_enable));
}



/**
 * Reads the high/low bitmask from the trident 2 gpio device
 *
 * @param[in] dev  trident 2 gpio device
 * @param[in] bank number of the bank to read
 */
int i2c_gpio_trident2_input_mask_get(i2c_dev *dev, uint8_t bank) {
	//TODO, i2c hub cpu shutdown check

	uint8_t command;
	switch (bank) {
		case 0:
			command = I2C_GPIO_COMMAND_READ_BANK_0;
			break;
		case 1:
			command = I2C_GPIO_COMMAND_READ_BANK_1;
			break;
		default:
			printf("invalid gpio bank selected\n");
			return -1;
	}

	i2c_gpio_trident2_data *data;
	DO_AND_CHECK(i2c_gpio_trident2_data_ptr(dev, &data));

	if (data->bank[bank].mask_enable != 0xFF) {
		printf("GPIO devid bank not set to input only, cannot read resistor configuration.\n");
		return -1;
	}

	return i2c_gpio_trident2_mask_read(dev, command, &(data->bank[bank].mask_input));
}






/**
 * Sets the high/low bitmask on the trident 2 gpio device
 *
 * @param[in] dev  trident 2 gpio device
 * @param[in] bank number of the bank to read
 * @param[in] mask_high high/low bitmask to set
 */
int i2c_gpio_trident2_output_mask_set(i2c_dev *dev, uint8_t bank, uint8_t mask_high) {
	//TODO, i2c hub cpu shutdown check

	uint8_t command;
	switch (bank) {
		case 0:
			command = I2C_GPIO_COMMAND_OUTPUT_BANK_0;
			break;
		case 1:
			command = I2C_GPIO_COMMAND_OUTPUT_BANK_1;
			break;
		default:
			printf("invalid gpio bank selected\n");
			return -1;
	}

	i2c_gpio_trident2_data *data;
	DO_AND_CHECK(i2c_gpio_trident2_data_ptr(dev, &data));

	if (data->bank[bank].mask_high == mask_high) {
		//requested mask already set
		return 0;
	}

	if (i2c_gpio_trident2_mask_write(dev, command, mask_high)) {
		//something went wrong.
		//update state with gpio chip state.
		if (i2c_gpio_trident2_output_mask_get(dev, bank)) {
			//could not even get state, unrecoverable.
			assert(NULL);
		}

		//error, but recovered.
		return -1;
	} else {
		//update mask in state model
		data->bank[bank].mask_high = mask_high;

		return 0;
	}
}



/**
 * Sets the input/output bitmask on the trident 2 gpio device
 *
 * @param[in] dev  trident 2 gpio device
 * @param[in] bank number of the bank to set
 * @param[in] mask_enabled input/output bitmask to set
 */
int i2c_gpio_trident2_inout_mask_set(i2c_dev *dev, uint8_t bank, uint8_t mask_enabled) {
	//TODO, i2c hub cpu shutdown check

	uint8_t command;
	switch (bank) {
		case 0:
			command = I2C_GPIO_COMMAND_INOUT_BANK_0;
			break;
		case 1:
			command = I2C_GPIO_COMMAND_INOUT_BANK_1;
			break;
		default:
			printf("invalid gpio bank selected\n");
			return -1;
	}

	i2c_gpio_trident2_data *data;
	DO_AND_CHECK(i2c_gpio_trident2_data_ptr(dev, &data));

	if (data->bank[bank].mask_enable == mask_enabled) {
		//requested mask already set
		return 0;
	}

	if (i2c_gpio_trident2_mask_write(dev, command, mask_enabled)) {
		//something went wrong.
		//update state with gpio chip state.
		if (i2c_gpio_trident2_output_mask_get(dev, bank)) {
			//could not even get state, unrecoverable.
			assert(NULL);
		}

		//error, but recovered.
		return -1;
	} else {
		//update mask in state model
		data->bank[bank].mask_enable = mask_enabled;
	}

	return 0;
}







/**
 * Sets the high/low bitmask on a single port of the trident 2 gpio device
 *
 * @param[in] dev         trident 2 gpio device
 * @param[in] port        number of the port to set
 * @param[in] output_high high/low bitmask to set
 */
int i2c_gpio_trident2_output_set(i2c_dev *dev, uint8_t port, uint8_t output_high) {
	i2c_gpio_trident2_data *data;
	DO_AND_CHECK(i2c_gpio_trident2_data_ptr(dev, &data));

	if (port >= I2C_GPIO_PORTS) {
		printf("port %3d outside of range for gpio device\n", port);
		return -1;
	}

	if ((port == I2C_TRIDENT2_GPIO_PORT_I2C_HUB_EN1) &&
			(!(output_high))) {
		printf("WARNING: Not allowing the turnoff of the cpu i2c connection to the hub.\n");
		return -1;
	}


	uint8_t bank    = port / I2C_GPIO_CHANNELS_PER_BANK;
	uint8_t channel = port % I2C_GPIO_CHANNELS_PER_BANK;

	uint8_t mask_high = data->bank[bank].mask_high;
	uint8_t bitmask = (1 << channel);

	//set or clear port bit depending request
	if (output_high) {
		mask_high |=  bitmask;
	} else {
		mask_high &= ~bitmask;
	}

	return i2c_gpio_trident2_output_mask_set(dev, bank, mask_high);
}



/**
 * Sets the input/output bitmask on a single port of the trident 2 gpio device
 *
 * @param[in] dev            trident 2 gpio device
 * @param[in] port           number of the port to set
 * @param[in] output_enabled 0: disabled, 1: enabled
 */
int i2c_gpio_trident2_inout_set(i2c_dev *dev, uint8_t port, uint8_t output_enabled) {
	i2c_gpio_trident2_data *data;
	DO_AND_CHECK(i2c_gpio_trident2_data_ptr(dev, &data));

	if (port >= I2C_GPIO_PORTS) {
		printf("port %3d outside of range for gpio device\n", port);
		return -1;
	}


	if ((port == I2C_TRIDENT2_GPIO_PORT_I2C_HUB_EN1) &&
			(output_enabled)) {
		uint8_t output_high;
		DO_AND_CHECK(i2c_gpio_trident2_output_get(dev, port, &output_high));

		if (!(output_high)) {
			printf("WARNING: Not allowing the turnoff of the cpu i2c connection to the hub.\n");
			return -1;
		}
	}

	uint8_t bank    = port / I2C_GPIO_CHANNELS_PER_BANK;
	uint8_t channel = port % I2C_GPIO_CHANNELS_PER_BANK;

	uint8_t mask_enable = data->bank[bank].mask_enable;
	uint8_t bitmask = (1 << channel);

	//set or clear enabled bit depending request
	//watch out! bit=0 indicated the output is on
	if (output_enabled) {
		mask_enable &= ~bitmask;
	} else {
		mask_enable |=  bitmask;
	}

	return i2c_gpio_trident2_inout_mask_set(dev, bank, mask_enable);
}







/**
 * Gets the high/low bitmask of a single port of the trident 2 gpio device
 *
 * @param[in] dev         trident 2 gpio device
 * @param[in] port        number of the port to get
 * @param[in] output_high 0: disabled, 1: enabled
 */
int i2c_gpio_trident2_output_get(i2c_dev *dev, uint8_t port, uint8_t *output_high) {
	i2c_gpio_trident2_data *data;
	DO_AND_CHECK(i2c_gpio_trident2_data_ptr(dev, &data));

	if (port >= I2C_GPIO_PORTS) {
		printf("port %3d outside of range for gpio device\n", port);
		return -1;
	}

	uint8_t bank    = port / I2C_GPIO_CHANNELS_PER_BANK;
	uint8_t channel = port % I2C_GPIO_CHANNELS_PER_BANK;

	uint8_t mask_high = data->bank[bank].mask_high;

	*output_high = 1 & (mask_high >> channel);
	return 0;
}


/**
 * Gets the input/output bitmask of a single port of the trident 2 gpio device
 *
 * @param[in] dev            trident 2 gpio device
 * @param[in] port           number of the port to get
 * @param[in] output_enabled 0: disabled, 1: enabled
 */
int i2c_gpio_trident2_inout_get(i2c_dev *dev, uint8_t port, uint8_t *output_enabled) {
	i2c_gpio_trident2_data *data;
	DO_AND_CHECK(i2c_gpio_trident2_data_ptr(dev, &data));

	if (port >= I2C_GPIO_PORTS) {
		printf("port %3d outside of range for gpio device\n", port);
		return -1;
	}

	uint8_t bank    = port / I2C_GPIO_CHANNELS_PER_BANK;
	uint8_t channel = port % I2C_GPIO_CHANNELS_PER_BANK;

	uint8_t mask_enable = data->bank[bank].mask_enable;

	//watch out! bit=0 indicated the output is on
	//flip bit so output on -> output_enabled=1
	uint8_t output_bit = 1 & (mask_enable >> channel);
	*output_enabled = output_bit ? 0 : 1;
	return 0;
}















/**
 * Gets the device ID of the trident 2 using the gpio device
 *
 * @param[in]  dev   trident 2 gpio device
 * @param[out] devid device ID of the trident 2
 */
int i2c_gpio_trident2_get_devid(i2c_dev *dev, uint8_t *devid) {
	assert(dev->type == gpio_trident2);

	i2c_gpio_trident2_data *data;
	DO_AND_CHECK(i2c_gpio_trident2_data_ptr(dev, &data));

	*devid = data->bank[0].mask_input;

	return 0;
}

