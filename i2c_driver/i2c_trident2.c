/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-02
 *
 *---------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "macros.h"
#include "i2c_devices.h"
#include "i2c_trident2.h"
#include "i2c_switch.h"
#include "i2c_das.h"
#include "i2c_gpio_trident2.h"
#include "i2c_psu.h"
#include "i2c_transceiver.h"
#include "i2c_eeprom.h"


i2c_dev *switch1;
i2c_dev *switch2;


/**
 * Initialises all devices in a trident 2
 */
int trident2_init() {


	//switches
	DO_AND_CHECK(trident2_init_switches());

	//DAS
	DO_AND_CHECK(trident2_init_das());

	//GPIO
	DO_AND_CHECK(trident2_init_gpio());

	//PSU
	DO_AND_CHECK(trident2_init_psus());

	//TRANSCEIVER
	DO_AND_CHECK(trident2_init_transceivers());

	//EEPROM
	DO_AND_CHECK(trident2_init_eeprom());

	return 0;
}




/**
 * Initialises the eeprom
 */
int trident2_init_eeprom() {
	i2c_dev *dev;

	return i2c_devices_dev_new_and_add(&dev, i2c_eeprom_init, I2C_TRIDENT2_EEPROM_ADDRESS);
}


/**
 * Initialises the psu's, the power supply units.
 */
int trident2_init_psus() {
	i2c_dev *dev;

	// PSU VDD1V_DIG
	DO_AND_CHECK(i2c_devices_dev_new(&dev));
 	dev->switch_dev = I2C_TRIDENT2_PSU_1V_DIG_SWITCH_DEV;
 	dev->channel    = I2C_TRIDENT2_PSU_1V_DIG_CHANNEL;
 	dev->adress     = I2C_TRIDENT2_PSU_1V_DIG_ADDRESS;
 	DO_AND_CHECK(i2c_psu_init(dev));
 	DO_AND_CHECK(i2c_devices_dev_add(dev));
	DO_AND_CHECK(i2c_psu_name_set(dev, "1V_DIG"));

	// PSU VDD1V_ANLG
	DO_AND_CHECK(i2c_devices_dev_new(&dev));
 	dev->switch_dev = I2C_TRIDENT2_PSU_1V_ANLG_SWITCH_DEV;
 	dev->channel    = I2C_TRIDENT2_PSU_1V_ANLG_CHANNEL;
 	dev->adress     = I2C_TRIDENT2_PSU_1V_ANLG_ADDRESS;
 	DO_AND_CHECK(i2c_psu_init(dev));
 	DO_AND_CHECK(i2c_devices_dev_add(dev));
	DO_AND_CHECK(i2c_psu_name_set(dev, "1V_ANLG"));

	// PSU VDD3v3
	DO_AND_CHECK(i2c_devices_dev_new(&dev));
 	dev->switch_dev = I2C_TRIDENT2_PSU_3V3_SWITCH_DEV;
 	dev->channel    = I2C_TRIDENT2_PSU_3V3_CHANNEL;
 	dev->adress     = I2C_TRIDENT2_PSU_3V3_ADDRESS;
 	DO_AND_CHECK(i2c_psu_init(dev));
 	DO_AND_CHECK(i2c_devices_dev_add(dev));
	DO_AND_CHECK(i2c_psu_name_set(dev, "3V3"));

	return 0;
}


/**
 * Initialises the gpio device
 */
int trident2_init_gpio() {
	i2c_dev *dev;

	DO_AND_CHECK(i2c_devices_dev_new_and_add(&dev, i2c_gpio_trident2_init, I2C_TRIDENT2_GPIO_ADDRESS));

	DO_AND_CHECK(i2c_gpio_trident2_name_port(dev, I2C_TRIDENT2_GPIO_PORT_I2C_HUB_EN3,  "I2C_HUB_EN3"));
	DO_AND_CHECK(i2c_gpio_trident2_name_port(dev, I2C_TRIDENT2_GPIO_PORT_I2C_HUB_EN2,  "I2C_HUB_EN2"));
	DO_AND_CHECK(i2c_gpio_trident2_name_port(dev, I2C_TRIDENT2_GPIO_PORT_I2C_HUB_EN1,  "I2C_HUB_EN1"));
	DO_AND_CHECK(i2c_gpio_trident2_name_port(dev, I2C_TRIDENT2_GPIO_PORT_SW_BCM_RESET, "SW_BCM_RESET"));
	DO_AND_CHECK(i2c_gpio_trident2_name_port(dev, I2C_TRIDENT2_GPIO_PORT_SW_I2C_RESET, "SW_I2C_RESET"));
	DO_AND_CHECK(i2c_gpio_trident2_name_port(dev, I2C_TRIDENT2_GPIO_PORT_SW_SHDN,      "SW_SHDN"));

	DO_AND_CHECK(i2c_gpio_trident2_output_set(dev, I2C_TRIDENT2_GPIO_PORT_I2C_HUB_EN3,  1));
	DO_AND_CHECK(i2c_gpio_trident2_output_set(dev, I2C_TRIDENT2_GPIO_PORT_I2C_HUB_EN2,  1));
	DO_AND_CHECK(i2c_gpio_trident2_output_set(dev, I2C_TRIDENT2_GPIO_PORT_I2C_HUB_EN1,  1));
	DO_AND_CHECK(i2c_gpio_trident2_output_set(dev, I2C_TRIDENT2_GPIO_PORT_SW_BCM_RESET, 1));
	DO_AND_CHECK(i2c_gpio_trident2_output_set(dev, I2C_TRIDENT2_GPIO_PORT_SW_I2C_RESET, 1));
	DO_AND_CHECK(i2c_gpio_trident2_output_set(dev, I2C_TRIDENT2_GPIO_PORT_SW_SHDN,      1));

	DO_AND_CHECK(i2c_gpio_trident2_inout_set(dev, I2C_TRIDENT2_GPIO_PORT_I2C_HUB_EN3,  1));
	DO_AND_CHECK(i2c_gpio_trident2_inout_set(dev, I2C_TRIDENT2_GPIO_PORT_I2C_HUB_EN2,  1));
	DO_AND_CHECK(i2c_gpio_trident2_inout_set(dev, I2C_TRIDENT2_GPIO_PORT_I2C_HUB_EN1,  1));
	DO_AND_CHECK(i2c_gpio_trident2_inout_set(dev, I2C_TRIDENT2_GPIO_PORT_SW_BCM_RESET, 1));
	DO_AND_CHECK(i2c_gpio_trident2_inout_set(dev, I2C_TRIDENT2_GPIO_PORT_SW_I2C_RESET, 1));
	DO_AND_CHECK(i2c_gpio_trident2_inout_set(dev, I2C_TRIDENT2_GPIO_PORT_SW_SHDN,      1));


	return 0;
}



/**
 * Initialises all transceiver's.
 */
int trident2_init_transceivers() {
	i2c_dev *dev;

	//TRANSCEIVER 01
	DO_AND_CHECK(i2c_devices_dev_new(&dev));
 	dev->switch_dev = I2C_TRIDENT2_TRANSCEIVER_01_SWITCH_DEV;
 	dev->channel    = I2C_TRIDENT2_TRANSCEIVER_01_CHANNEL;
 	dev->adress     = I2C_TRIDENT2_TRANSCEIVER_01_ADDRESS;
 	DO_AND_CHECK(i2c_transceiver_init(dev));
 	DO_AND_CHECK(i2c_devices_dev_add(dev));

	//TRANSCEIVER 02
	DO_AND_CHECK(i2c_devices_dev_new(&dev));
 	dev->switch_dev = I2C_TRIDENT2_TRANSCEIVER_02_SWITCH_DEV;
 	dev->channel    = I2C_TRIDENT2_TRANSCEIVER_02_CHANNEL;
 	dev->adress     = I2C_TRIDENT2_TRANSCEIVER_02_ADDRESS;
 	DO_AND_CHECK(i2c_transceiver_init(dev));
 	DO_AND_CHECK(i2c_devices_dev_add(dev));

	//TRANSCEIVER 03
	DO_AND_CHECK(i2c_devices_dev_new(&dev));
 	dev->switch_dev = I2C_TRIDENT2_TRANSCEIVER_03_SWITCH_DEV;
 	dev->channel    = I2C_TRIDENT2_TRANSCEIVER_03_CHANNEL;
 	dev->adress     = I2C_TRIDENT2_TRANSCEIVER_03_ADDRESS;
 	DO_AND_CHECK(i2c_transceiver_init(dev));
 	DO_AND_CHECK(i2c_devices_dev_add(dev));

	//TRANSCEIVER 04
	DO_AND_CHECK(i2c_devices_dev_new(&dev));
 	dev->switch_dev = I2C_TRIDENT2_TRANSCEIVER_04_SWITCH_DEV;
 	dev->channel    = I2C_TRIDENT2_TRANSCEIVER_04_CHANNEL;
 	dev->adress     = I2C_TRIDENT2_TRANSCEIVER_04_ADDRESS;
 	DO_AND_CHECK(i2c_transceiver_init(dev));
 	DO_AND_CHECK(i2c_devices_dev_add(dev));

	//TRANSCEIVER 05
	DO_AND_CHECK(i2c_devices_dev_new(&dev));
 	dev->switch_dev = I2C_TRIDENT2_TRANSCEIVER_05_SWITCH_DEV;
 	dev->channel    = I2C_TRIDENT2_TRANSCEIVER_05_CHANNEL;
 	dev->adress     = I2C_TRIDENT2_TRANSCEIVER_05_ADDRESS;
 	DO_AND_CHECK(i2c_transceiver_init(dev));
 	DO_AND_CHECK(i2c_devices_dev_add(dev));

	//TRANSCEIVER 06
	DO_AND_CHECK(i2c_devices_dev_new(&dev));
 	dev->switch_dev = I2C_TRIDENT2_TRANSCEIVER_06_SWITCH_DEV;
 	dev->channel    = I2C_TRIDENT2_TRANSCEIVER_06_CHANNEL;
 	dev->adress     = I2C_TRIDENT2_TRANSCEIVER_06_ADDRESS;
 	DO_AND_CHECK(i2c_transceiver_init(dev));
 	DO_AND_CHECK(i2c_devices_dev_add(dev));

	//TRANSCEIVER 07
	DO_AND_CHECK(i2c_devices_dev_new(&dev));
 	dev->switch_dev = I2C_TRIDENT2_TRANSCEIVER_07_SWITCH_DEV;
 	dev->channel    = I2C_TRIDENT2_TRANSCEIVER_07_CHANNEL;
 	dev->adress     = I2C_TRIDENT2_TRANSCEIVER_07_ADDRESS;
 	DO_AND_CHECK(i2c_transceiver_init(dev));
 	DO_AND_CHECK(i2c_devices_dev_add(dev));

	//TRANSCEIVER 08
	DO_AND_CHECK(i2c_devices_dev_new(&dev));
 	dev->switch_dev = I2C_TRIDENT2_TRANSCEIVER_08_SWITCH_DEV;
 	dev->channel    = I2C_TRIDENT2_TRANSCEIVER_08_CHANNEL;
 	dev->adress     = I2C_TRIDENT2_TRANSCEIVER_08_ADDRESS;
 	DO_AND_CHECK(i2c_transceiver_init(dev));
 	DO_AND_CHECK(i2c_devices_dev_add(dev));

	//TRANSCEIVER 09
	DO_AND_CHECK(i2c_devices_dev_new(&dev));
 	dev->switch_dev = I2C_TRIDENT2_TRANSCEIVER_09_SWITCH_DEV;
 	dev->channel    = I2C_TRIDENT2_TRANSCEIVER_09_CHANNEL;
 	dev->adress     = I2C_TRIDENT2_TRANSCEIVER_09_ADDRESS;
 	DO_AND_CHECK(i2c_transceiver_init(dev));
 	DO_AND_CHECK(i2c_devices_dev_add(dev));

	//TRANSCEIVER 10
	DO_AND_CHECK(i2c_devices_dev_new(&dev));
 	dev->switch_dev = I2C_TRIDENT2_TRANSCEIVER_10_SWITCH_DEV;
 	dev->channel    = I2C_TRIDENT2_TRANSCEIVER_10_CHANNEL;
 	dev->adress     = I2C_TRIDENT2_TRANSCEIVER_10_ADDRESS;
 	DO_AND_CHECK(i2c_transceiver_init(dev));
 	DO_AND_CHECK(i2c_devices_dev_add(dev));

	//TRANSCEIVER 11
	DO_AND_CHECK(i2c_devices_dev_new(&dev));
 	dev->switch_dev = I2C_TRIDENT2_TRANSCEIVER_11_SWITCH_DEV;
 	dev->channel    = I2C_TRIDENT2_TRANSCEIVER_11_CHANNEL;
 	dev->adress     = I2C_TRIDENT2_TRANSCEIVER_11_ADDRESS;
 	DO_AND_CHECK(i2c_transceiver_init(dev));
 	DO_AND_CHECK(i2c_devices_dev_add(dev));


	return 0;
}



/**
 * Initialises the i2c_switches
 */
int trident2_init_switches() {
	DO_AND_CHECK(i2c_devices_dev_new_and_add(&switch1, i2c_switch_init, I2C_TRIDENT2_SWITCH_1_ADDRESS));
	DO_AND_CHECK(i2c_devices_dev_new_and_add(&switch2, i2c_switch_init, I2C_TRIDENT2_SWITCH_2_ADDRESS));

	DO_AND_CHECK(i2c_devices_call_on_type(switch_dev, i2c_switch_close));

	return 0;
}


/**
 * Initialises the das, data acquisition systems, used for measuring voltages.
 */
int trident2_init_das() {
	i2c_dev *dev;

	DO_AND_CHECK(i2c_devices_dev_new_and_add(&dev, i2c_das_init, I2C_TRIDENT2_DAS_ADDRESS));

	i2c_das_data *data = ((i2c_das_data*) dev->data);

	DO_AND_CHECK(i2c_das_channel_set(data->channels + 0, 1, 0, 0, "VDD1V_DIG"));
	DO_AND_CHECK(i2c_das_channel_set(data->channels + 1, 1, 0, 0, "VDD1V_ANLG"));
	DO_AND_CHECK(i2c_das_channel_set(data->channels + 2, 1, 0, 0, "VDD3V3"));
	DO_AND_CHECK(i2c_das_channel_set(data->channels + 3, 1, 0, 0, "VDD3V3_AUX"));
	DO_AND_CHECK(i2c_das_channel_set(data->channels + 4, 1, 1, 0, "VDD5V"));

	return 0;
}
