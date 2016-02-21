/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-11-24
 *
 *---------------------------------------------------------------------------
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "macros.h"
#include "i2c_devices.h"
#include "i2c_switch.h"
#include "i2c_lowlevel.h"



/** array of lists holding all the devices by device type */
i2c_dev_array i2c_dev_type_array[I2C_DEV_TYPE_COUNT];
/** counter of how many devices are created, used to assign an run time unique ID */
uint32_t dev_counter = 0;





/**
 * Initialises the device abstraction and underlying api's
 */
int i2c_devices_init() {
	DO_AND_CHECK(i2c_lowlevel_init());

	int i;
	for (i = 0; i < I2C_DEV_TYPE_COUNT; i++) {
		i2c_dev_type_array[i].dev_count = 0;
		i2c_dev_type_array[i].devs      = NULL;
	}

	return 0;
}




/**
 * Creates a new device, initialises it and then adds it to the device array
 *
 * @param[out] dev     newly created device
 * @param[in]  init    initilisation function
 * @param[in]  address i2c adress of the device
 */
int i2c_devices_dev_new_and_add(i2c_dev **dev, int (*init)(i2c_dev*), uint8_t address) {
	DO_AND_CHECK(i2c_devices_dev_new(dev));
	(*dev)->adress = address;
	DO_AND_CHECK((*init)(*dev));
	DO_AND_CHECK(i2c_devices_dev_add(*dev));

	return 0;
}


/**
 * Creates a new device
 *
 * @param[out] dev     newly created device
 */
int i2c_devices_dev_new(i2c_dev **dev) {
	i2c_dev *newptr = malloc(sizeof *newptr);
	if (newptr == NULL) {
		printf("ERROR: Could not allocate memory for device\n");
		return -1;
	}

	newptr->dev_id = dev_counter;
	dev_counter++;

	//default to no switch in i2c path
	newptr->switch_dev = NULL;
	newptr->channel    = 0;

	//init data pointer
	newptr->data       = NULL;

	*dev = newptr;

	return 0;
}



/**
 * Adds a device to the device array
 *
 * @param[in] dev The device to add
 */
int i2c_devices_dev_add(i2c_dev *dev) {
	i2c_dev_array *dev_array = i2c_dev_type_array + dev->type;

	dev_array->dev_count++;

	i2c_dev **newptr = realloc(dev_array->devs, dev_array->dev_count * sizeof(*newptr));
	if (newptr == NULL) {
		printf("ERROR: Could not allocate memory for increasing device array\n");
		return -1;
	}

	dev_array->devs = newptr;

	dev_array->devs[dev_array->dev_count -1] = dev;

	return 0;
}






/**
 * Get's the list_id'th device of a type
 *
 * @param[out] dev_handle retreived device
 * @param[in]  type       type of device to get
 * @param[in]  list_id    x'th device to get
 */
int i2c_devices_dev_get(i2c_dev **dev_handle, i2c_dev_type type, uint16_t list_id) {
	i2c_dev_array *dev_array = i2c_dev_type_array + type;

	if (list_id >= dev_array->dev_count) {
		return -1;
	}

	*dev_handle = dev_array->devs[list_id];

	return 0;
}



/**
 * Calls a function on all devices of a single type
 *
 * @param[in] type Type of the devices to call the function on
 * @param[in] func Function to call on the devices
 */
int i2c_devices_call_on_type(i2c_dev_type type, int (*func)(i2c_dev*)) {
	int i = 0;
	i2c_dev *dev;

	while (!(i2c_devices_dev_get(&dev, type, i))) {
		DO_AND_CHECK((*func)(dev));
		i++;
	}

	return 0;
}





/**
 * dumps memory structure of a device
 *
 * @param[in] dev device to dump
 */
int i2c_devices_dev_dump(i2c_dev *dev) {
	printf("    struct at %p, dev_id: %4d, type: %1d, address: 0x%02X, switch: %p, channel %3u, data: %p\n",
					dev, dev->dev_id, dev->type, dev->adress, dev->switch_dev, dev->channel, dev->data);
	return 0;
}


/**
 * Dumps memory structure of all devices
 */
int i2c_devices_dev_array_dump() {
	printf("\n");
	printf("i2c_dev array dump: \n");

	printf("  Switches: \n");
	DO_AND_CHECK(i2c_devices_call_on_type(switch_dev, i2c_devices_dev_dump));

	printf("  Das's: \n");
	DO_AND_CHECK(i2c_devices_call_on_type(das, i2c_devices_dev_dump));

	printf("  PSU's: \n");
	DO_AND_CHECK(i2c_devices_call_on_type(psu, i2c_devices_dev_dump));

	printf("  GPIO's: \n");
	DO_AND_CHECK(i2c_devices_call_on_type(gpio_trident1, i2c_devices_dev_dump));
	DO_AND_CHECK(i2c_devices_call_on_type(gpio_trident2, i2c_devices_dev_dump));

	printf("  TRANSCEIVER's: \n");
	DO_AND_CHECK(i2c_devices_call_on_type(transceiver, i2c_devices_dev_dump));

	printf("  EEPROM's: \n");
	DO_AND_CHECK(i2c_devices_call_on_type(eeprom, i2c_devices_dev_dump));

	printf("\n");

	return 0;
}








/**
 * Sets up i2c switches so that a connection is established to the device
 *
 * @param[in] dev Device to connect to
 */
int i2c_devices_switch_connect(i2c_dev *dev) {
	i2c_dev_array *switches = i2c_dev_type_array + switch_dev;

	int i;
	for (i = 0; i < switches->dev_count; i++ ) {
		i2c_dev *switchdev = switches->devs[i];

		//check if this switch is connected to our device
		if (switchdev == dev->switch_dev) {
			//open the connected switch channel
			DO_AND_CHECK(i2c_switch_select(switchdev, dev->channel));
		} else {
			//close all channels
			DO_AND_CHECK(i2c_switch_set(switchdev, 0));
		}
	}

	return 0;
}








/**
 * Writes x bytes to a device and then read y bytes from the same device in a single i2c transaction,
 *   without issuing a STOP command in between
 *
 * @param[in]  dev             Device to write to and read from
 * @param[in]  buffer_write    bytes array to be written to the device
 * @param[in]  bytecount_write amount of bytes to write to the device
 * @param[out] buffer_read     bytes array to be written from the device
 * @param[in]  bytecount_read  amount of bytes to be written from the device
 */
int i2c_devices_write_and_read(i2c_dev *dev,
																uint8_t *buffer_write, uint16_t bytecount_write,
																uint8_t *buffer_read,  uint16_t bytecount_read ) {
	DO_AND_CHECK(i2c_devices_switch_connect(dev));
	return i2c_lowlevel_write_and_read(dev->adress,
														buffer_write, bytecount_write,
														buffer_read,  bytecount_read);
}


/**
 * Reads x bytes from a device
 *
 * @param[in]  dev       Device to read from
 * @param[out] buffer    bytes array to be written from the device
 * @param[in]  bytecount amount of bytes to be written from the device
 */
int i2c_devices_read(i2c_dev *dev, uint8_t *buffer, uint16_t bytecount) {
	DO_AND_CHECK(i2c_devices_switch_connect(dev));
	return i2c_lowlevel_read(dev->adress, buffer, bytecount);
}


/**
 * Reads x bytes from a device without printing any errors.
 * Usefull for probing devices for existing.
 *
 * @param[in]  dev       Device to read from
 * @param[out] buffer    bytes array to be written from the device
 * @param[in]  bytecount amount of bytes to be written from the device
 */
int i2c_devices_read_silent(i2c_dev *dev, uint8_t *buffer, uint16_t bytecount) {
	DO_AND_CHECK(i2c_devices_switch_connect(dev));
	return i2c_lowlevel_read_silent(dev->adress, buffer, bytecount);
}


/**
 * Writes x bytes to a device
 *
 * @param[in]  dev       Device to write to
 * @param[out] buffer    bytes array to be written to the device
 * @param[in]  bytecount amount of bytes to be written to the device
 */
int i2c_devices_write(i2c_dev *dev, uint8_t *buffer, uint16_t bytecount) {
	DO_AND_CHECK(i2c_devices_switch_connect(dev));
	return i2c_lowlevel_write(dev->adress, buffer, bytecount);
}





/**
 * Writes a command to a device and then writes x bytes to the same device in a single i2c transaction.
 * This function concatanates the data to the command and then writes it as a single block of data.
 *
 * @param[in]  dev       Device to write to and read from
 * @param[in]  command   Command to write to the device
 * @param[out] data      bytes array to be written from the device
 * @param[in]  bytecount amount of bytes to be written from the device
 */
int i2c_devices_command_and_write(i2c_dev *dev, uint8_t command, uint8_t *data, size_t bytecount) {
  unsigned char buf[bytecount + 1];

	//put command into first byte
  buf[0] = command;

  //fill rest of buffer with write data
  memcpy(buf+1, data, bytecount);

  return i2c_devices_write(dev, buf, bytecount +1);
}




/**
 * Writes a command to a device and then read x bytes from the same device in a single i2c transaction,
 *   without issuing a STOP command in between.
 * 1 byte variant.
 *
 * @param[in]  dev     Device to write to and read from
 * @param[in]  command Command to write to the device
 * @param[out] data    Data read from the device
 */
int i2c_devices_command_and_read_uint8(i2c_dev *dev, uint8_t command, uint8_t *data) {
	DO_AND_CHECK(i2c_devices_switch_connect(dev));
	return i2c_lowlevel_command_and_read_uint8(dev->adress, command, data);
}


/**
 * Writes a command to a device and then read x bytes from the same device in a single i2c transaction,
 *   without issuing a STOP command in between.
 * 2 byte variant.
 *
 * @param[in]  dev     Device to write to and read from
 * @param[in]  command Command to write to the device
 * @param[out] data    Data read from the device
 */
int i2c_devices_command_and_read_uint16(i2c_dev *dev, uint8_t command, uint16_t *data) {
	DO_AND_CHECK(i2c_devices_switch_connect(dev));
	return i2c_lowlevel_command_and_read_uint16(dev->adress, command, data);
}


/**
 * Writes a command to a device and then writes x bytes to the same device in a single i2c transaction,
 *   without issuing a STOP command in between.
 * Internally just issues a single command with concatenated data.
 *
 * 2 byte variant.
 *
 * @param[in] adress  address of the device to write to and read from
 * @param[in] command Command to write to the device
 * @param[in] data    Data to write to the device
 */
int i2c_devices_command_and_write_uint16(i2c_dev *dev, uint8_t command, uint16_t *data) {
	DO_AND_CHECK(i2c_devices_switch_connect(dev));
	return i2c_lowlevel_command_and_write_uint16(dev->adress, command, data);
}






/**
 * Writes a command to a device and then writes x bytes to the same device in a single i2c transaction,
 *   without issuing a STOP command in between.
 * Internally just issues a single command with concatenated data.
 *
 * 1 byte variant.
 *
 * @param[in] adress  address of the device to write to and read from
 * @param[in] command Command to write to the device
 * @param[in] data    Data to write to the device
 */
int i2c_devices_command_and_write_uint8(i2c_dev *dev, uint8_t command, uint8_t *data) {
	DO_AND_CHECK(i2c_devices_switch_connect(dev));
	return i2c_lowlevel_command_and_write_uint8(dev->adress, command, data);
}














