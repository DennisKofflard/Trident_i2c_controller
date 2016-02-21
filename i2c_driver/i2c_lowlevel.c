/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-11-24
 *
 *---------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "Cgos.h"
#include "i2c_lowlevel.h"
#include "macros.h"

//#define printf(format, ...) fprintf(stderr, format, __VA_ARGS__);
//#define printf(format) fprintf(stderr, format);
//allows with and without extra arguments
#define printf(format, ...) fprintf(stderr, format, ##__VA_ARGS__);


HCGOS hCgos;


/**
 * Initialises everything needed by this api
 */
int i2c_lowlevel_init() {
	DO_AND_CHECK(i2c_lowlevel_init_cgos());
	return 0;
}


/**
 * Initialises the congatec library and opens the board for i2c i/o.
 * Assumes the first board is the correct board to use for i2c, this is normally true.
 */
int i2c_lowlevel_init_cgos() {
	if (!CgosLibInitialize()) {
		printf("could not init CGOS lib\n");
		return -1;
	}

	if(!CgosBoardOpen(0, 0, 0, &hCgos)){
		printf("failed to connect to board\n");
		return -1;
	}

	return 0;
}




/**
 * Writes a command to a device and then read x bytes from the same device in a single i2c transaction,
 *   without issuing a STOP command in between.
 * 1 byte variant.
 *
 * @param[in]  adress  address of the device to write to and read from
 * @param[in]  command Command to write to the device
 * @param[out] data    Data read from the device
 */
int i2c_lowlevel_command_and_read_uint8(uint8_t adress, uint8_t command, uint8_t *data) {
	return i2c_lowlevel_write_and_read(adress, &command, 1, data, 1);
}


/**
 * Writes a command to a device and then read x bytes from the same device in a single i2c transaction,
 *   without issuing a STOP command in between.
 * 2 byte variant.
 *
 * @param[in]  adress  address of the device to write to and read from
 * @param[in]  command Command to write to the device
 * @param[out] data    Data read from the device
 */
int i2c_lowlevel_command_and_read_uint16(uint8_t adress, uint8_t command, uint16_t *data) {
	uint8_t buf[2];
	DO_AND_CHECK(i2c_lowlevel_write_and_read(adress, &command, 1, buf, 2));

	//printf("i2c_command_and_read bytes: 0x%02X, 0x%02X\n", buf[0], buf[1]);

	//Swap bytes before merge to convert endianness to x86 memory format.
	//i2c reads MSB to LSB by convention.
	*data = (buf[0] << 8) | buf[1];

	return 0;
}



/**
 * Writes x bytes to a device and then read y bytes from the same device in a single i2c transaction,
 *   without issuing a STOP command in between
 *
 * @param[in]  adress          address of the device to write to and read from
 * @param[in]  buffer_write    bytes array to be written to the device
 * @param[in]  bytecount_write amount of bytes to write to the device
 * @param[out] buffer_read     bytes array to be written from the device
 * @param[in]  bytecount_read  amount of bytes to be written from the device
 */
int i2c_lowlevel_write_and_read(uint8_t adress,
											 unsigned char *buffer_write, uint16_t bytecount_write,
											 unsigned char *buffer_read,  uint16_t bytecount_read) {
	if (!CgosI2CWriteReadCombined(hCgos, 0, adress,
																buffer_write, bytecount_write,
																buffer_read,  bytecount_read )) {
		//printf("combined read and write failed.\n");
		return -1;
	}

	return 0;
}


/**
 * Reads x bytes from a device
 *
 * @param[in]  adress    address of the device to write to and read from
 * @param[out] buffer    bytes array to be written from the device
 * @param[in]  bytecount amount of bytes to be written from the device
 */
int i2c_lowlevel_read(uint8_t adress, unsigned char *buffer, uint16_t bytecount) {
	if (!CgosI2CRead(hCgos, 0, (adress | 0x01), buffer, bytecount)) {
		printf("read failed.\n");
		return -1;
	}

	return 0;
}




/**
 * Reads x bytes from a device without printing any errors.
 * Usefull for probing devices for existing.
 *
 * @param[in]  adress    address of the device to write to and read from
 * @param[out] buffer    bytes array to be written from the device
 * @param[in]  bytecount amount of bytes to be written from the device
 */
int i2c_lowlevel_read_silent(uint8_t adress, unsigned char *buffer, uint16_t bytecount) {
	if (!CgosI2CRead(hCgos, 0, (adress | 0x01), buffer, bytecount)) {
		return -1;
	}

	return 0;
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
int i2c_lowlevel_command_and_write_uint8(uint8_t adress, uint8_t command, uint8_t *data) {
	return i2c_lowlevel_command_and_write(adress, &command, 1, data, 1);
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
int i2c_lowlevel_command_and_write_uint16(uint8_t adress, uint8_t command, uint16_t *data) {
	uint8_t buf[2];
	DO_AND_CHECK(i2c_lowlevel_write_and_read(adress, &command, 1, buf, 2));

	//Swap bytes before split to convert endianness from x86 memory format.
	buf[0] = ((*data) >> 8) & 0xFF;
	buf[1] = ((*data) >> 0) & 0xFF;

	return i2c_lowlevel_command_and_write(adress, &command, 1, buf, 2);
}

/**
 * Writes a command to a device and then writes x bytes to the same device in a single i2c transaction,
 *   without issuing a STOP command in between.
 * Internally just issues a single command with concatenated data.
 *
 * @param[in] adress  address of the device to write to and read from
 * @param[in] command Command to write to the device
 * @param[in] data    Data to write to the device
 */
int i2c_lowlevel_command_and_write(uint8_t adress,
											             uint8_t *buffer_command, uint16_t bytecount_command,
											             uint8_t *buffer_write,   uint16_t bytecount_write) {
	uint8_t *buf = malloc((bytecount_command + bytecount_write) * sizeof(uint8_t));
	if (buf == NULL) {
		return -1;
	}

	memcpy(buf,                     buffer_command, bytecount_command);
	memcpy(buf + bytecount_command, buffer_write,   bytecount_write);

	return i2c_lowlevel_write(adress, buf, (bytecount_command + bytecount_write)); 
}



/**
 * Writes x bytes to a device
 *
 * @param[in]  adress    address of the device to write to and read from
 * @param[out] buffer    bytes array to be written to the device
 * @param[in]  bytecount amount of bytes to be written to the device
 */
int i2c_lowlevel_write(uint8_t adress, unsigned char *buffer, uint16_t bytecount) {
	if (!CgosI2CWrite(hCgos, 0, adress,          buffer,          bytecount)) {
		printf("write failed.\n");
		return -1;
	}

	return 0;
}


/**
 * Writes x bytes to a device and reads x bytes back from the device checking for any differences.
 * used by the i2c switch_dev's
 *
 * @param[in]  adress    address of the device to write to and read from
 * @param[out] buffer    bytes array to be written to the device
 * @param[in]  bytecount amount of bytes to be written to the device
 */
int i2c_lowlevel_write_and_check(uint8_t adress, unsigned char *buffer, uint16_t bytecount) {
	unsigned char buffer_readback[bytecount];

	if (i2c_lowlevel_write(adress, buffer, bytecount)) {
		return -1;
	}

	if (!CgosI2CRead( hCgos, 0, (adress | 0x01), buffer_readback, bytecount)) {
		printf("read failed.\n");
		return -1;
	}

	//TODO: check for possible bug if bytecount to large.
	if (memcmp(buffer, buffer_readback, bytecount)) {
		printf("written and read byte do not match.\n");
		return -1;
	}

	return 0;
}
