/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-11-24
 *
 *---------------------------------------------------------------------------
 */


#ifndef I2C_H
#define I2C_H


int i2c_lowlevel_init();
int i2c_lowlevel_init_cgos();

int i2c_lowlevel_write_and_read(uint8_t adress,
											 uint8_t *buffer_write, uint16_t bytecount_write,
											 uint8_t *buffer_read,  uint16_t bytecount_read );

int i2c_lowlevel_read(uint8_t adress, uint8_t *buffer, uint16_t bytecount);
int i2c_lowlevel_read_silent(uint8_t adress, uint8_t *buffer, uint16_t bytecount);
int i2c_lowlevel_write(uint8_t adress, uint8_t *buffer, uint16_t bytecount);
int i2c_lowlevel_write_and_check(uint8_t adress, uint8_t *buffer, uint16_t bytecount);


int i2c_lowlevel_command_and_read_uint8(uint8_t adress, uint8_t command, uint8_t *data);
int i2c_lowlevel_command_and_read_uint16(uint8_t adress, uint8_t command, uint16_t *data);
int i2c_lowlevel_command_and_write(uint8_t adress,
											             uint8_t *buffer_command, uint16_t bytecount_command,
											             uint8_t *buffer_write,   uint16_t bytecount_write);
int i2c_lowlevel_command_and_write_uint8(uint8_t adress, uint8_t command, uint8_t *data);
int i2c_lowlevel_command_and_write_uint16(uint8_t adress, uint8_t command, uint16_t *data);





#endif /* I2C_H */
