/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-11-24
 *
 *---------------------------------------------------------------------------
 */

#ifndef I2C_PSU_H
#define I2C_PSU_H



typedef struct i2c_psu_data{
	char         *name;
}i2c_psu_data;





//int i2c_psu_detect();

int i2c_psu_init(i2c_dev *dev);

int i2c_psu_poll(i2c_dev *dev);

int i2c_psu_data_ptr(i2c_dev *dev, i2c_psu_data **data_ptr);
int i2c_psu_name_set(i2c_dev *dev, char *name);





int i2c_psu_enable_takeover(i2c_dev *dev);
int i2c_psu_enable_on(i2c_dev *dev);
int i2c_psu_enable_off(i2c_dev *dev);
int i2c_psu_enable_off_hard(i2c_dev *dev);
int i2c_psu_status_reset(i2c_dev *dev);

int i2c_psu_voltage_set_high(i2c_dev *dev);
int i2c_psu_voltage_set_low(i2c_dev *dev);
int i2c_psu_voltage_set_default(i2c_dev *dev);
int i2c_psu_voltage_set_mv(i2c_dev *dev, uint16_t milivolts);
int i2c_psu_voltage_set_enable(i2c_dev *dev);

int i2c_psu_voltage_regulation_configuration_get(i2c_dev *dev, uint8_t *data);

int i2c_psu_voltage_get(i2c_dev *dev, uint16_t *Vout);
int i2c_psu_current_get(i2c_dev *dev, uint16_t *Iout);
int i2c_psu_power_get(  i2c_dev *dev, uint16_t *Pout);
//int i2c_psu_vip_avg_get(i2c_dev *dev, int (*vip_get)(i2c_dev*, uint16_t*), uint16_t *vip, uint16_t rounds);

int i2c_psu_status_vout_get(i2c_dev *dev, uint8_t *status_vout);
int i2c_psu_status_iout_get(i2c_dev *dev, uint8_t *status_iout);
int i2c_psu_status_word_get(i2c_dev *dev, uint16_t *status_word);

int i2c_psu_write_protect_get(i2c_dev *dev, uint8_t *write_protect);
int i2c_psu_enable_get(i2c_dev *dev, uint8_t *data);
int i2c_psu_operation_get(i2c_dev *dev, uint8_t *data);

int i2c_psu_voltage_vid_to_mv(i2c_dev *dev, uint8_t VID, uint16_t *milivolts);
int i2c_psu_voltage_mv_to_vid(i2c_dev *dev, uint16_t milivolts, uint8_t *VID);




#endif /* I2C_PSU_H */
