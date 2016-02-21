/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-17
 *
 *---------------------------------------------------------------------------
 */

/**************
 * Driver for the trident 1 pcf8574 i2c gpio expander
 *************/

 
 
 
#ifndef I2C_GPIO_PCF8574_H
#define I2C_GPIO_PCF8574_H



typedef struct i2c_gpio_trident1_data{
	uint8_t     input_mask;
}i2c_gpio_trident1_data;



int i2c_gpio_trident1_init(i2c_dev *dev);
int i2c_gpio_trident1_poll(i2c_dev *dev);

int i2c_gpio_trident1_read_mask(i2c_dev *dev);

int i2c_gpio_trident1_report_all(i2c_dev *dev);
int i2c_gpio_trident1_get_devid(i2c_dev *dev, uint8_t *devid);



#endif /* I2C_GPIO_PCF8574_H */


