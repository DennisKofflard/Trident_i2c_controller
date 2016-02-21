/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-11-24
 *
 *---------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

//#include "i2c.h"
#include "i2c_devices.h"
#include "i2c_psu.h"
//#include "i2c_psu_report.h"
#include "i2c_switch.h"
#include "macros.h"




/// @todo upgrade driver to cache data





/**
 * Initialises the psu device
 *
 * @param[in] dev memory structure of psu device
 */
int i2c_psu_init(i2c_dev *dev) {
	dev->type = psu;

	/// @todo: create memory structure to store state in

	i2c_psu_data *data = malloc(sizeof(*data));
	if (data == NULL) {
		printf("ERROR: Could not allocate memory for psu data\n");
		return -1;
	}

	dev->data = data;


	//TODO, do usefull stuff
	data->name = NULL;

	return 0;
}



/**
 * Polls the psu for data.
 * Caching is not yet implemented
 * @todo implement caching
 *
 * @param[in] dev     memory structure of psu device
 */
int i2c_psu_poll(i2c_dev *dev) {
	assert(dev->type == psu);

	/// @todo: poll data

	return 0;
}







/**
 * Internal function.
 * Checks the device type and converts the data pointer to the right type
 * 
 * @param[in]  dev      memory structure of psu device, generic pointer
 * @param[out] data_ptr memory structure of psu device, cast to right type
 */
int i2c_psu_data_ptr(i2c_dev *dev, i2c_psu_data **data_ptr) {
	assert(dev->type == psu);

	*data_ptr = ((i2c_psu_data*) dev->data);

	return 0;
}


/**
 * Sets the name of a psu device
 *
 * @param[in] dev     memory structure of psu device
 * @param[in] name    name to set
 */
int i2c_psu_name_set(i2c_dev *dev, char *name) {
	i2c_psu_data *data;
	DO_AND_CHECK(i2c_psu_data_ptr(dev, &data));

	data->name = strdup(name);

	return 0;
}




/**
 * Enables i2c voltage control of the psu
 *
 * @param[in] dev     memory structure of psu device
 */
int i2c_psu_enable_takeover(i2c_dev *dev) {
  uint8_t command = 0x02; //ON_OFF_Config
//  uint8_t data = 0x17; //default setting
//  uint8_t data = 0x1F; //default +(enable i2c control)
  uint8_t data = 0x1B; //default +(enable i2c control) +(disable enable-pin control)
  return i2c_devices_command_and_write(dev, command, &data, 1);
}


/**
 * Turns the psu on. Only works after taking over the psu.
 * @see #i2c_psu_enable_takeover
 * 
 * @param[in] dev     memory structure of psu device
 */
int i2c_psu_enable_on(i2c_dev *dev) {
  uint8_t command = 0x01; // "operation"
  uint8_t data = 0x80; //on
  return i2c_devices_command_and_write(dev, command, &data, 1);
}

/**
 * Turns the psu off. Only works after taking over the psu.
 * Executes a "soft shutdown", what this is is undefined in the datasheet.
 * This "soft shutdown" generates errors on usage
 * @see #i2c_psu_enable_takeover
 * 
 * @param[in] dev     memory structure of psu device
 */
int i2c_psu_enable_off(i2c_dev *dev) {
  uint8_t command = 0x01; // "operation"
  uint8_t data = 0x40; // soft off
  return i2c_devices_command_and_write(dev, command, &data, 1);
}

/**
 * Turns the psu off. Only works after taking over the psu.
 * Executes a "hard shutdown", what this is is undefined in the datasheet.
 * This "hard shutdown" does not generate errors on usage
 * @see #i2c_psu_enable_takeover
 * 
 * @param[in] dev     memory structure of psu device
 */
int i2c_psu_enable_off_hard(i2c_dev *dev) {
  uint8_t command = 0x01; // "operation"
  uint8_t data = 0x00; // immidiate off
  return i2c_devices_command_and_write(dev, command, &data, 1);
}

/**
 * Resets the status flags in the psu.
 * Used for clearing errors after reading them.
 * If a error is still present during reset then the flag will be immidiatly re-set by the psu.
 *
 * @param[in] dev     memory structure of psu device
 */
int i2c_psu_status_reset(i2c_dev *dev) {
  uint8_t command = 0x03; // status reset
  uint8_t data = 0x00; // bogus
  return i2c_devices_command_and_write(dev, command, &data, 0);
}




/**
 * Sets the psu voltage to 1.0000 volts.
 * Only works after enabling voltage control of the psu.
 * @see #i2c_psu_voltage_set_enable
 * 
 * @param[in] dev     memory structure of psu device
 */
int i2c_psu_voltage_set_high(i2c_dev *dev) {
  uint8_t command = 0x21; // Vout_command
  uint8_t VID = 0x62; // 1.00000 volts
  return i2c_devices_command_and_write(dev, command, &VID, 1);
}


/**
 * Sets the psu voltage to 0.5000 volts.
 * Only works after enabling voltage control of the psu.
 * @see #i2c_psu_voltage_set_enable
 * 
 * @param[in] dev     memory structure of psu device
 */
int i2c_psu_voltage_set_low(i2c_dev *dev) {
  uint8_t command = 0x21; // Vout_command
  uint8_t VID = 0xB2; // 0.50000 volts
  return i2c_devices_command_and_write(dev, command, &VID, 1);
}


/**
 * Sets the psu voltage to the default value from the psu
 * Only works after enabling voltage control of the psu.
 * @see #i2c_psu_voltage_set_enable
 * @todo figure out exact default VID
 * 
 * @param[in] dev     memory structure of psu device
 */
int i2c_psu_voltage_set_default(i2c_dev *dev) {
  uint8_t command = 0x21; // Vout_command
  uint8_t VID = 0x5B; // 1.05000 volts
  return i2c_devices_command_and_write(dev, command, &VID, 1);
}


/**
 * Sets the psu voltage in milivolts.
 * Only works after enabling voltage control of the psu.
 * @see #i2c_psu_voltage_set_enable
 * 
 * @param[in] dev       memory structure of psu device
 * @param[in] milivolts voltage to set the output to
 */
int i2c_psu_voltage_set_mv(i2c_dev *dev, uint16_t milivolts) {
  uint8_t command = 0x21; // Vout_command
  uint8_t VID;

  if (i2c_psu_voltage_mv_to_vid(dev, milivolts, &VID)) {
		printf("failed to convert %d milivolts to a VID\n", milivolts);
		return -1;
  }

  return i2c_devices_command_and_write(dev, command, &VID, 1);
}

/**
 * Enables voltage control over i2c on the psu.
 * @warning DO NOT ENABLE VOLTAGE CONTROL BEFORE SETTING A VOLTAGE.
 *          doing so sets a random voltage depending on whatever the uninitialised voltage control register is set to,
 *          likely destroying the BCM
 *
 * @todo check is a voltage is set before allowing execution of this function
 *
 * @param[in] dev     memory structure of psu device
 */
int i2c_psu_voltage_set_enable(i2c_dev *dev) {
  //uint8_t data = 0x52; //default
  uint8_t data = 0x5A; //default +VID_EN, enable VID control

  uint8_t commandA = 0xD2; // VR config A
  uint8_t commandB = 0xD3; // VR config B
  return i2c_devices_command_and_write(dev, commandA, &data, 1) ||
         i2c_devices_command_and_write(dev, commandB, &data, 1);
}







/**
 * Gets the voltage regulation configuration byte from the psu.
 *
 * @param[in]  dev  memory structure of psu device
 * @param[out] data VRconf byte
 */
int i2c_psu_voltage_regulation_configuration_get(i2c_dev *dev, uint8_t *data) {
  uint8_t command = 0xD2; // VRconf

  return i2c_devices_command_and_read_uint8(dev, command, data);
}

/**
 * Gets the output voltage bytes from the psu.
 *
 * @param[in]  dev  memory structure of psu device
 * @param[out] Vout output voltage in VID
 */
int i2c_psu_voltage_get(i2c_dev *dev, uint16_t *Vout) {
	uint8_t command = 0x8b; // Read_Vout

  return i2c_devices_command_and_read_uint16(dev, command, Vout);
}


/**
 * Gets the output current bytes from the psu.
 * The convertion from these bytes to ampere is currently unknown.
 *
 * @param[in]  dev  memory structure of psu device
 * @param[out] Iout output current in unknown format
 */
int i2c_psu_current_get(i2c_dev *dev, uint16_t *Iout) {
  uint8_t command = 0x8C; // Read_Iout

  return i2c_devices_command_and_read_uint16(dev, command, Iout);
}


/**
 * Gets the output power bytes from the psu
 * The convertion from these bytes to watts is currently unknown.
 * This scaling is probably related to the scaling of the current.
 * @see #i2c_psu_current_get
 *
 * @param[in]  dev  memory structure of psu device
 * @param[out] Pout output power in unknown format
 */
int i2c_psu_power_get(i2c_dev *dev, uint16_t *Pout) {
	uint8_t command = 0x96; // Read_Pout

  return i2c_devices_command_and_read_uint16(dev, command, Pout);
}


/*
// was used for reporting, this functionality is now moved away from the driver
//TODO: generalise averaging function to use for das as well
int i2c_psu_vip_avg_get(i2c_dev *dev, int (*vip_get)(i2c_dev*, uint16_t*), uint16_t *vip, uint16_t rounds) {
	uint32_t acumelator = 0;

	int i;
	for (i = 0; i < rounds; i++) {
		uint16_t buf;

		if ((*vip_get)(dev, &buf)) {
			printf("failed to read VIP value\n");
			return -1;
		}

		acumelator += buf;
	}

	//divide to average with rounds nearest
	*vip = (acumelator + (0.5*rounds)) / rounds;

	return 0;
}
*/


/**
 * Resets the voltage status flags in the psu.
 *
 * @param[in]  dev         memory structure of psu device
 * @param[out] status_vout vout status byte
 */
int i2c_psu_status_vout_get(i2c_dev *dev, uint8_t *status_vout) {
  uint8_t command = 0x7A; //get status vout

  return i2c_devices_command_and_read_uint8(dev, command, status_vout);
}


/**
 * Resets the current status flags in the psu.
 *
 * @param[in]  dev         memory structure of psu device
 * @param[out] status_iout iout status byte
 */
int i2c_psu_status_iout_get(i2c_dev *dev, uint8_t *status_iout) {
  uint8_t command = 0x7B; //get status vout //TODO, check command name

  return i2c_devices_command_and_read_uint8(dev, command, status_iout);
}


/**
 * Reads the generic status flags in the psu.
 *
 * @param[in]  dev         memory structure of psu device
 * @param[out] status_word status word
 */
int i2c_psu_status_word_get(i2c_dev *dev, uint16_t *status_word) {
  uint8_t command = 0x79; //get status word

  //return i2c_devices_command_and_read_uint16(dev, command, status_word);

  DO_AND_CHECK(i2c_devices_command_and_read_uint16(dev, command, status_word));



  //swap bytes.
  //either the status word spec is wrong or the read word spec is wrong
  //TODO: find cause
  //NEW CODE: do not swap ?
  //*status_word = ((*status_word & 0x00FF) << 8) | ((*status_word & 0xFF00) >> 8);

  return 0;
}


/**
 * Reads the write protect byte in the psu.
 *
 * @param[in]  dev           memory structure of psu device
 * @param[out] write_protect write protect byte
 */
int i2c_psu_write_protect_get(i2c_dev *dev, uint8_t *write_protect) {
  uint8_t command = 0x10; // write protect

  return i2c_devices_command_and_read_uint8(dev, command, write_protect);
}


/**
 * Reads the enabled byte in the psu.
 *
 * @param[in]  dev   memory structure of psu device
 * @param[out] data  ON_OFF_Config byte
 */
int i2c_psu_enable_get(i2c_dev *dev, uint8_t *data) {
  uint8_t command = 0x02; // ON_OFF_Config

  return i2c_devices_command_and_read_uint8(dev, command, data);
}


/**
 * Reads the operation byte in the psu.
 *
 * @param[in]  dev   memory structure of psu device
 * @param[out] data  operation byte
 */
int i2c_psu_operation_get(i2c_dev *dev, uint8_t *data) {
  uint8_t command = 0x01; // "operation"

  return i2c_devices_command_and_read_uint8(dev, command, data);
}







/**
 * Converts a VID, a voltage identification code, to milivolts
 *
 * @todo Expand this for PSU with voltage scaling
 *       This is relevant for the 3.3v psu controller with scaled feedback.
 *
 * @param[in]  dev       memory structure of psu device
 * @param[in]  VID       VID to convert
 * @param[out] milivolts conversion result
 */
int i2c_psu_voltage_vid_to_mv(i2c_dev *dev, uint8_t VID, uint16_t *milivolts) {
	//This function rounds

  const uint32_t voltage_base = 1600000;
  const uint32_t voltage_step =    6250;
	// VID 2   = 1.6 volt
	// VID 2+x = 1.6 - x*0.00625 volts

	if ((VID == 0) || (VID == 1) || (VID == 254) | (VID == 255)) {
		//special VID codes
		*milivolts = 0;
		return 0;
	} else if (VID > 178) {
		//UNDEFINED
		*milivolts = 0;
		return -1;
	}

  uint8_t steps = VID-2;

	uint32_t microvolts = voltage_base - (steps * voltage_step);
	*milivolts = microvolts / 1000;

	return 0;
}



/**
 * Converts milivolts to a VID, a voltage identification code.
 *
 * @todo Expand this for PSU with voltage scaling
 *       This is relevant for the 3.3v psu controller with scaled feedback.
 *
 * @param[in]  dev       memory structure of psu device
 * @param[in]  milivolts voltage to convert
 * @param[out] VID       conversion result
 */
int i2c_psu_voltage_mv_to_vid(i2c_dev *dev, uint16_t milivolts, uint8_t *VID) {
	//This function rounds

	if (milivolts == 0) {
		//pick VID closest to lowest voltage VID value for safety.
		*VID = 255;
		return 0;
	} else if ((milivolts < 500) || (milivolts > 1600)) {
		//voltage outside of VID range.
		*VID = 255;
		return -1;
	}

	const uint32_t voltage_base = 1600000;
  const uint32_t voltage_step =    6250;

	uint32_t microvolts = milivolts * 1000;
	uint32_t voltage_difference = voltage_base - microvolts;

  uint8_t steps = ((voltage_difference + (0.5*voltage_step)) / voltage_step);

	*VID = 2 + steps;

	return 0;
}



























