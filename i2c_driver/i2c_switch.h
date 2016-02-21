/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-11-24
 *
 *---------------------------------------------------------------------------
 */


#ifndef I2C_SWITCH_H
#define I2C_SWITCH_H


int i2c_switch_init(i2c_dev *dev);

int i2c_switch_poll(i2c_dev *dev);

int i2c_switch_close(i2c_dev *dev);
int i2c_switch_set(i2c_dev *dev, uint8_t channel_mask);
int i2c_switch_set_stateless(i2c_dev *dev, uint8_t channel_mask);
int i2c_switch_select(i2c_dev *dev, uint8_t channel);


int i2c_switch_data_dump(i2c_dev *dev);

typedef struct i2c_switch_data{
	uint8_t channel_mask;
	uint8_t state_valid;
}i2c_switch_data;


#endif /* I2C_SWITCH_H */
