/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-16
 *
 *---------------------------------------------------------------------------
 */


#ifndef I2C_EEPROM_H
#define I2C_EEPROM_H

#define I2C_EEPROM_BITSIZE      64000
#define I2C_EEPROM_BYTESIZE     (I2C_EEPROM_BITSIZE / 8)
#define I2C_EEPROM_PAGESIZE     32
#define I2C_EEPROM_PAGECOUNT    (I2C_EEPROM_BYTESIZE / I2C_EEPROM_BYTESIZE)



/* no local data needed.
typedef struct i2c_eeprom_data{
	//empty
}i2c_eeprom_data;
*/



int i2c_eeprom_init(i2c_dev *dev);
int i2c_eeprom_poll(i2c_dev *dev);


int i2c_eeprom_report_all(i2c_dev *dev);
int i2c_eeprom_report_page_zero(i2c_dev *dev);
int i2c_eeprom_report_page(i2c_dev *dev, uint8_t pagenumber);

int i2c_eeprom_write_byte(i2c_dev *dev, uint16_t address, uint8_t byte);
int i2c_eeprom_write_page(i2c_dev *dev, uint16_t address, uint8_t page[I2C_EEPROM_PAGESIZE]);

int i2c_eeprom_read(i2c_dev *dev, uint16_t address_start, unsigned char *buffer, uint16_t bytecount);
int i2c_eeprom_write(i2c_dev *dev, uint16_t address_start, uint8_t *buffer, uint16_t bytecount);


#endif /* I2C_EEPROM_H */


