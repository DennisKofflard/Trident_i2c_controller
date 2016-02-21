/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-04
 *
 *---------------------------------------------------------------------------
 */

 /**************
 * Driver for the trident 2 TCA9555 i2c gpio expander
 *************/

 
 

#ifndef I2C_GPIO_H
#define I2C_GPIO_H


#define I2C_GPIO_BANKS              2
//I2C_GPIO_CHANNELS_PER_BANK MUST BE <=8, to fit masks in uint8_t
#define I2C_GPIO_CHANNELS_PER_BANK  8
#define I2C_GPIO_PORTS  (I2C_GPIO_BANKS * I2C_GPIO_CHANNELS_PER_BANK)


#define I2C_GPIO_COMMAND_READ_BANK_0 0x00
#define I2C_GPIO_COMMAND_READ_BANK_1 0x01
#define I2C_GPIO_COMMAND_OUTPUT_BANK_0 0x02
#define I2C_GPIO_COMMAND_OUTPUT_BANK_1 0x03
#define I2C_GPIO_COMMAND_INOUT_BANK_0  0x06
#define I2C_GPIO_COMMAND_INOUT_BANK_1  0x07



typedef struct i2c_gpio_trident2_bank_data{
	uint8_t               mask_enable;
	uint8_t               mask_high;
	uint8_t               mask_input;
	//char								*(name[I2C_GPIO_CHANNELS_PER_BANK]);
	char*                 name[8];
}i2c_gpio_trident2_bank_data;


typedef struct i2c_gpio_trident2_data{
	//i2c_gpio_trident2_bank_data  bank[I2C_GPIO_BANKS];
	i2c_gpio_trident2_bank_data  bank[I2C_GPIO_BANKS];
}i2c_gpio_trident2_data;






int i2c_gpio_trident2_init(i2c_dev *dev);
int i2c_gpio_trident2_poll(i2c_dev *dev);

int i2c_gpio_trident2_name_port(i2c_dev *dev, uint8_t port, char *name);


int i2c_gpio_trident2_report(i2c_dev *dev, uint8_t port);
int i2c_gpio_trident2_report_all(i2c_dev *dev);
int i2c_gpio_trident2_report_all_masks(i2c_dev *dev);


int i2c_gpio_trident2_mask_write(i2c_dev *dev, uint8_t command, uint8_t mask);
int i2c_gpio_trident2_mask_read(i2c_dev *dev, uint8_t command, uint8_t *mask);

int i2c_gpio_trident2_data_ptr(i2c_dev *dev, i2c_gpio_trident2_data **data_ptr);

int i2c_gpio_trident2_output_mask_get(i2c_dev *dev, uint8_t bank);
int i2c_gpio_trident2_inout_mask_get(i2c_dev *dev, uint8_t bank);
int i2c_gpio_trident2_input_mask_get(i2c_dev *dev, uint8_t bank);
int i2c_gpio_trident2_output_mask_set(i2c_dev *dev, uint8_t bank, uint8_t mask_high);
int i2c_gpio_trident2_inout_mask_set(i2c_dev *dev, uint8_t bank, uint8_t mask_enabled);

int i2c_gpio_trident2_output_set(i2c_dev *dev, uint8_t port, uint8_t output_high);
int i2c_gpio_trident2_inout_set(i2c_dev *dev, uint8_t port, uint8_t output_enabled);

int i2c_gpio_trident2_output_get(i2c_dev *dev, uint8_t port, uint8_t *output_high);
int i2c_gpio_trident2_inout_get(i2c_dev *dev, uint8_t port, uint8_t *output_enabled);

int i2c_gpio_trident2_get_devid(i2c_dev *dev, uint8_t *devid);

#endif /* I2C_GPIO_H */


