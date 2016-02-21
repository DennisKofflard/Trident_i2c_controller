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
#include "i2c_trident1.h"
#include "i2c_switch.h"
#include "i2c_das.h"
#include "i2c_gpio_trident1.h"
#include "i2c_transceiver.h"
#include "i2c_eeprom.h"


i2c_dev *switch1;


/**
 * Initialises all devices in a trident 1
 */
int trident1_init() {


	//switches
	DO_AND_CHECK(trident1_init_switch());

	printf("i2c switch init done.\n");

	//DAS
	DO_AND_CHECK(trident1_init_das());

	printf("das init done.\n");

	//GPIO
	DO_AND_CHECK(trident1_init_gpio());
	printf("gpio init done.\n");

	//TRANSCEIVER
	DO_AND_CHECK(trident1_init_cxps());
	printf("transceiver init done.\n");

	//EEPROM
	DO_AND_CHECK(trident1_init_eeprom());
	printf("eeprom init done.\n");

	return 0;
}


/**
 * Initialises the gpio device
 */
int trident1_init_gpio() {
	i2c_dev *dev;

	DO_AND_CHECK(i2c_devices_dev_new_and_add(&dev, i2c_gpio_trident1_init, I2C_TRIDENT1_GPIO_ADDRESS));
	//DO_AND_CHECK(i2c_devices_dev_new_and_add(&dev, i2c_gpio_init, I2C_TRIDENT1_GPIO_ADDRESS));

	return 0;
}



/**
 * Initialises all cxp's.
 *
 * The cxp's are identical to the transceiver's used in trident 2 from a i2c point of view so the same library is used.
 */
int trident1_init_cxps() {
	i2c_dev *dev;

	//TRANSCEIVER 01
		DO_AND_CHECK(i2c_devices_dev_new(&dev));
		dev->switch_dev = I2C_TRIDENT1_CXP_01_SWITCH_DEV;
		dev->channel    = I2C_TRIDENT1_CXP_01_CHANNEL;
		dev->adress     = I2C_TRIDENT1_CXP_01_ADDRESS;
		DO_AND_CHECK(i2c_transceiver_init(dev));
		DO_AND_CHECK(i2c_devices_dev_add(dev));

	//TRANSCEIVER 02
		DO_AND_CHECK(i2c_devices_dev_new(&dev));
		dev->switch_dev = I2C_TRIDENT1_CXP_02_SWITCH_DEV;
		dev->channel    = I2C_TRIDENT1_CXP_02_CHANNEL;
		dev->adress     = I2C_TRIDENT1_CXP_02_ADDRESS;
		DO_AND_CHECK(i2c_transceiver_init(dev));
		DO_AND_CHECK(i2c_devices_dev_add(dev));

	//TRANSCEIVER 03
		DO_AND_CHECK(i2c_devices_dev_new(&dev));
		dev->switch_dev = I2C_TRIDENT1_CXP_03_SWITCH_DEV;
		dev->channel    = I2C_TRIDENT1_CXP_03_CHANNEL;
		dev->adress     = I2C_TRIDENT1_CXP_03_ADDRESS;
		DO_AND_CHECK(i2c_transceiver_init(dev));
		DO_AND_CHECK(i2c_devices_dev_add(dev));

	//TRANSCEIVER 04
		DO_AND_CHECK(i2c_devices_dev_new(&dev));
		dev->switch_dev = I2C_TRIDENT1_CXP_04_SWITCH_DEV;
		dev->channel    = I2C_TRIDENT1_CXP_04_CHANNEL;
		dev->adress     = I2C_TRIDENT1_CXP_04_ADDRESS;
		DO_AND_CHECK(i2c_transceiver_init(dev));
		DO_AND_CHECK(i2c_devices_dev_add(dev));

	//TRANSCEIVER 05
		DO_AND_CHECK(i2c_devices_dev_new(&dev));
		dev->switch_dev = I2C_TRIDENT1_CXP_05_SWITCH_DEV;
		dev->channel    = I2C_TRIDENT1_CXP_05_CHANNEL;
		dev->adress     = I2C_TRIDENT1_CXP_05_ADDRESS;
		DO_AND_CHECK(i2c_transceiver_init(dev));
		DO_AND_CHECK(i2c_devices_dev_add(dev));

	//TRANSCEIVER 06
		DO_AND_CHECK(i2c_devices_dev_new(&dev));
		dev->switch_dev = I2C_TRIDENT1_CXP_06_SWITCH_DEV;
		dev->channel    = I2C_TRIDENT1_CXP_06_CHANNEL;
		dev->adress     = I2C_TRIDENT1_CXP_06_ADDRESS;
		DO_AND_CHECK(i2c_transceiver_init(dev));
		DO_AND_CHECK(i2c_devices_dev_add(dev));


	// Only one transceiver added?? There are 11 transceivers (or 6 cxps)

	return 0;
}



/**
 * Initialises the i2c_switch
 */
int trident1_init_switch() {
	DO_AND_CHECK(i2c_devices_dev_new_and_add(&switch1, i2c_switch_init, I2C_TRIDENT1_SWITCH_1_ADDRESS));

	DO_AND_CHECK(i2c_devices_call_on_type(switch_dev, i2c_switch_close));

	return 0;
}



/**
 * Initialises the eeprom
 */
int trident1_init_eeprom() {
	i2c_dev *dev;

	return i2c_devices_dev_new_and_add(&dev, i2c_eeprom_init, I2C_TRIDENT1_EEPROM_ADDRESS);
}


/**
 * Initialises the das, data acquisition systems, used for measuring voltages.
 */
int trident1_init_das() {
	i2c_dev *dev;

	DO_AND_CHECK(i2c_devices_dev_new_and_add(&dev, i2c_das_init, I2C_TRIDENT1_DAS_ADDRESS));

	i2c_das_data *data = ((i2c_das_data*) dev->data);

	DO_AND_CHECK(i2c_das_channel_set(data->channels + 1, 1, 1, 0, "VDD5V"));
	DO_AND_CHECK(i2c_das_channel_set(data->channels + 2, 1, 0, 0, "VDD3V3"));
	DO_AND_CHECK(i2c_das_channel_set(data->channels + 7, 1, 0, 0, "VDD1V"));

	return 0;
}
