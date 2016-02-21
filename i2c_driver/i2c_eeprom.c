/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-16
 *
 *---------------------------------------------------------------------------
 */


#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>


#include "i2c_devices.h"
#include "i2c_eeprom.h"
#include "macros.h"



/**
 * Initialises a eeprom device
 *
 * @param[in] dev eeprom device to initialise
 */
int i2c_eeprom_init(i2c_dev *dev) {
	dev->type = eeprom;

	//nothing to do here.

	return 0;
}



/**
 * Polls a eeprom device
 *
 * @param[in] dev eeprom device to poll
 */
int i2c_eeprom_poll(i2c_dev *dev) {
	assert(dev->type == eeprom);

	//nothing to do here.
	//eeprom is a static device.

	return 0;
}




/**
 * Reports a eeprom device.
 * Dumps all memory pages
 *
 * @param[in] dev eeprom device to report
 */
int i2c_eeprom_report_all(i2c_dev *dev) {
	assert(dev->type == eeprom);

	uint16_t page;
  for (page = 0; page<I2C_EEPROM_PAGECOUNT; page++) {
    if (i2c_eeprom_report_page(dev, page)) {
      printf("dumping eeprom memory failed on page %d\n", page);
      return -1;
    }
  }

  return 0;
}




/**
 * Reports a eeprom device.
 * Dumps cwthe first memory page
 * 
 * not really useful, just to have something to show when asked to report.
 *
 * @param[in] dev eeprom device to report
 */
int i2c_eeprom_report_page_zero(i2c_dev *dev) {
	assert(dev->type == eeprom);

	return i2c_eeprom_report_page(dev, 0);
}



/**
 * Reports a page of a eeprom device
 *
 * @param[in] dev        eeprom device to read the page from
 * @param[in] pagenumber number of the page to report
 */
int i2c_eeprom_report_page(i2c_dev *dev, uint8_t pagenumber) {
	assert(dev->type == eeprom);

  uint16_t address_start = pagenumber * I2C_EEPROM_PAGESIZE;
  uint16_t bytecount = I2C_EEPROM_PAGESIZE;
  unsigned char buffer[bytecount];

  if(i2c_eeprom_read(dev, address_start, buffer, bytecount)) {
    printf("eeprom read failed.\n");
    return -1;
  }

  printf("0x%04X to 0x%04X:  ", (pagenumber * I2C_EEPROM_PAGESIZE), (((pagenumber + 1) * I2C_EEPROM_PAGESIZE) - 1) );

  int i;
  for (i = 0; i<(bytecount-1); i++) {
    printf("%X,", buffer[i]);
  }
  printf("%X\n", buffer[i]);

  return 0;
}










/**
 * Writes a single byte to a eeprom device
 *
 * @param[in] dev     eeprom device to write to
 * @param[in] address memory address to write to
 * @param[in] byte    the byte to write
 */
int i2c_eeprom_write_byte(i2c_dev *dev, uint16_t address, uint8_t byte) {
	assert(dev->type == eeprom);

	return i2c_eeprom_write(dev, address, &byte, 1);
}




/**
 * Writes a full page to a eeprom device
 *
 * @param[in] dev     eeprom device to write to
 * @param[in] address memory address to write to
 * @param[in] page    the page of bytes to write
 */
int i2c_eeprom_write_page(i2c_dev *dev, uint16_t address, uint8_t page[I2C_EEPROM_PAGESIZE]) {
	assert(dev->type == eeprom);

	return i2c_eeprom_write(dev, address, page, I2C_EEPROM_PAGESIZE);
}









/**
 * reads x bytes from a eeprom device
 *
 * @param[in]  dev           eeprom device to read from
 * @param[in]  address_start memory address to start reading from
 * @param[out] buffer        bytes array to be read from the eeprom
 * @param[in]  bytecount     amount of bytes to be read from the eeprom
 */
int i2c_eeprom_read(i2c_dev *dev, uint16_t address_start, unsigned char *buffer, uint16_t bytecount) {
	assert(dev->type == eeprom);

  uint8_t address_start_cut[2];

  if ((address_start + (bytecount-1)) >= (8192)) {
    printf("address range out of bounds.\n");
    printf("requested bytes from 0x%04X to 0x%04X\n", address_start, (address_start + bytecount-1));
    return -1;
  }

  address_start_cut[0] = (address_start & 0xFF00) >> 8;
  address_start_cut[1] = (address_start & 0x00FF) >> 0;

  //printf("address bytes: %x, %x\n", adress_start_cut[0], adress_start_cut[1]);

  //if (i2c_write_and_read(address, address_start_cut, 2, buffer, bytecount)) {
  if (i2c_devices_write_and_read(dev, address_start_cut, 2, buffer, bytecount)) {
    printf("could not read memory.\n");
    return -1;
  }

  return 0;
}





/**
 * writes to a eeprom device.
 * 
 * DO NOT USE
 *
 * do not call this function directly.
 * this function is written according to the datasheet specifications
 * but the eeprom does not conform to the specifications
 * 
 * @param[in] dev           eeprom device to write to
 * @param[in] address_start memory address to start writing at
 * @param[in] buffer        bytes array to be written to the device
 * @param[in] bytecount     amount of bytes to be written to the device
 */
int i2c_eeprom_write(i2c_dev *dev, uint16_t address_start, uint8_t *buffer, uint16_t bytecount) {
	assert(dev->type == eeprom);

  if ((address_start + (bytecount-1)) >= (I2C_EEPROM_BYTESIZE)) {
    printf("address range out of bounds.\n");
    printf("requested from 0x%04X to 0x%04X\n", address_start, (address_start + bytecount));
    return -1;
  }

  if ((address_start / I2C_EEPROM_PAGESIZE) != ((address_start + (bytecount-1)) / I2C_EEPROM_PAGESIZE)) {
    printf("address may not cross page boundries.\n");
    return -1;
  }

	if ((bytecount != 1) && (bytecount != I2C_EEPROM_PAGESIZE)) {
		//EEPROM will report succes but will fail anyway.
		//do not allow write although the datasheet says it should work.
    printf("Not allow eeprom write with bytecount unequal to 1 or the page size\n");
		return -1;
	}

  //need to write memory address and data bytes in one run, create a combined buffer.
  uint8_t write_buffer[(2 + bytecount)];

  write_buffer[0] = (address_start & 0xFF00) >> 8;
  write_buffer[1] = (address_start & 0x00FF) >> 0;

  //append data to memory address.
  memcpy(write_buffer + 2, buffer, bytecount * sizeof(uint8_t));

	/*
  printf("Write buffer:\n");
  int i;
  for (i = 0; i<(bytecount+2); i++) {
    printf("%02X,", write_buffer[i]);
  }
  printf("\n");
	*/

  //if (i2c_write(address, write_buffer, 2 + bytecount)) {
  if (i2c_devices_write(dev, write_buffer, 2 + bytecount)) {
    printf("could not write memory.\n");
    return -1;
  }

  //TODO: possibly check if memory is actually set correctly afterwards

  //printf("written memory succesfully.\n");

  return 0;
}













