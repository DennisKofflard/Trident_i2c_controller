/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-11-24
 *
 *---------------------------------------------------------------------------
 */

#ifndef I2C_PSU_REPORT_H
#define I2C_PSU_REPORT_H



int i2c_psu_status_report_generic(uint8_t bitcount,
																	uint16_t data,
																	int (*bit_to_str)(uint8_t, uint8_t, char*, size_t));

int i2c_psu_status_word_report(uint16_t status_word);
int i2c_psu_status_vout_report(uint8_t status_vout);
int i2c_psu_status_iout_report(uint8_t status_iout);
int i2c_psu_status_report();

int i2c_psu_write_protect_report();
int i2c_psu_enable_report();
int i2c_psu_operation_report();
int i2c_psu_voltage_regulation_configuration_report();
int i2c_psu_voltage_report();
int i2c_psu_current_report();
int i2c_psu_power_report();



#endif /* I2C_PSU_REPORT_H */
