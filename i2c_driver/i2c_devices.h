/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-11-24
 *
 *---------------------------------------------------------------------------
 */


#ifndef I2C_ADDRESSES_H
#define I2C_ADDRESSES_H


#include <stdint.h>



//UPDATE THIS COUNTER WHEN ADDING DEVICES
#define I2C_DEV_TYPE_COUNT 7
typedef enum i2c_dev_type {
	switch_dev, //cannot be called just 'switch' due to reserved c names
	das,
	psu,
	transceiver,
	eeprom,
	gpio_trident1,
	gpio_trident2
}i2c_dev_type;


//forward declaration to alow self reference
typedef struct i2c_dev i2c_dev;
struct i2c_dev {
	uint16_t        dev_id;
	i2c_dev_type    type;
	uint8_t         adress;
	i2c_dev         *switch_dev;
	uint8_t         channel;
	void            *data;
};


typedef struct i2c_dev_array {
	uint16_t dev_count;
	i2c_dev  **devs;  //pointer to array of pointers to i2c_dev's
}i2c_dev_array;













int i2c_devices_init();
int i2c_devices_dev_new_and_add(i2c_dev **dev, int (*init)(i2c_dev*), uint8_t address);
int i2c_devices_dev_new(i2c_dev **dev);
int i2c_devices_dev_add(i2c_dev *dev);



int i2c_devices_dev_get(i2c_dev **dev_handle, i2c_dev_type type, uint16_t list_id);
int i2c_devices_call_on_type(i2c_dev_type type, int (*func)(i2c_dev*));

int i2c_devices_dev_dump(i2c_dev *dev);
int i2c_devices_dev_array_dump();


int i2c_devices_write_and_read(i2c_dev *dev,
											 uint8_t *buffer_write, uint16_t bytecount_write,
											 uint8_t *buffer_read,  uint16_t bytecount_read );


int i2c_devices_read(i2c_dev *dev, uint8_t *buffer, uint16_t bytecount);
int i2c_devices_read_silent(i2c_dev *dev, uint8_t *buffer, uint16_t bytecount);
int i2c_devices_write(i2c_dev *dev, uint8_t *buffer, uint16_t bytecount);
int i2c_devices_write_and_check(i2c_dev *dev, uint8_t *buffer, uint16_t bytecount);



int i2c_devices_command_and_read_uint8(i2c_dev *dev, uint8_t command, uint8_t *data);
int i2c_devices_command_and_read_uint16(i2c_dev *dev, uint8_t command, uint16_t *data);
int i2c_devices_command_and_write(i2c_dev *dev, uint8_t command, uint8_t *data, size_t bytecount);
int i2c_devices_command_and_write_uint8(i2c_dev *dev, uint8_t command, uint8_t *data);
int i2c_devices_command_and_write_uint16(i2c_dev *dev, uint8_t command, uint16_t *data);


#endif /* I2C_ADDRESSES_H */
