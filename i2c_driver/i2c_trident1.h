/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-02
 *
 *---------------------------------------------------------------------------
 */
#ifndef I2C_TRIDENT1_H
#define I2C_TRIDENT1_H

#define I2C_TRIDENT1_SWITCH_COUNT 1
#define I2C_TRIDENT1_SWITCH_1_ADDRESS  0xE6

#define I2C_TRIDENT1_DAS_ADDRESS  0x50

#define I2C_TRIDENT1_GPIO_ADDRESS  0x4E		// Only one bank

#define I2C_TRIDENT1_GPIO_PORT_ID_7    7
#define I2C_TRIDENT1_GPIO_PORT_ID_6    6
#define I2C_TRIDENT1_GPIO_PORT_ID_5    5
#define I2C_TRIDENT1_GPIO_PORT_ID_4    4
#define I2C_TRIDENT1_GPIO_PORT_ID_3    3
#define I2C_TRIDENT1_GPIO_PORT_ID_2    2
#define I2C_TRIDENT1_GPIO_PORT_ID_1    1
#define I2C_TRIDENT1_GPIO_PORT_ID_0    0

#define I2C_TRIDENT1_EEPROM_ADDRESS  0xA0	// B' 1010 X00X --> A0 or A8??




//transceiver address identical for all devices
#define I2C_TRIDENT1_CXP_ADDRESS  0xA0

// cxp1 in ch7, cxp6 in ch2 and so on
#define I2C_TRIDENT1_CXP_01_SWITCH_DEV  switch1
#define I2C_TRIDENT1_CXP_01_CHANNEL     7
#define I2C_TRIDENT1_CXP_01_ADDRESS     I2C_TRIDENT1_CXP_ADDRESS

#define I2C_TRIDENT1_CXP_02_SWITCH_DEV  switch1
#define I2C_TRIDENT1_CXP_02_CHANNEL     6
#define I2C_TRIDENT1_CXP_02_ADDRESS     I2C_TRIDENT1_CXP_ADDRESS


#define I2C_TRIDENT1_CXP_03_SWITCH_DEV  switch1
#define I2C_TRIDENT1_CXP_03_CHANNEL     5
#define I2C_TRIDENT1_CXP_03_ADDRESS     I2C_TRIDENT1_CXP_ADDRESS


#define I2C_TRIDENT1_CXP_04_SWITCH_DEV  switch1
#define I2C_TRIDENT1_CXP_04_CHANNEL     4
#define I2C_TRIDENT1_CXP_04_ADDRESS     I2C_TRIDENT1_CXP_ADDRESS


#define I2C_TRIDENT1_CXP_05_SWITCH_DEV  switch1
#define I2C_TRIDENT1_CXP_05_CHANNEL     3
#define I2C_TRIDENT1_CXP_05_ADDRESS     I2C_TRIDENT1_CXP_ADDRESS

#define I2C_TRIDENT1_CXP_06_SWITCH_DEV  switch1
#define I2C_TRIDENT1_CXP_06_CHANNEL     2
#define I2C_TRIDENT1_CXP_06_ADDRESS     I2C_TRIDENT1_CXP_ADDRESS



int trident1_init();
int trident1_init_gpio();
int trident1_init_cxps();			// create i2c_cxps.h/c, use i2c_transceivers.h/c or create i2c_transceivers.h/c??
int trident1_init_switch();			// only one switch in trident1
int trident1_init_eeprom();
int trident1_init_das();


#endif /* I2C_TRIDENT1_H */



