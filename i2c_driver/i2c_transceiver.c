/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-09
 *
 *---------------------------------------------------------------------------
 */


/** \file
 * Driver for the transceivers.
 *
 * This driver has two modes, TX mode and RX mode, and continuously switches between them internally.
 * This is done because the TRANSCEIVER has two i2c device addresses, one for TX and one for RX, and this driver need to communicate with both sub modules.
 *
 * Also works for CXP's as the driver is build from the generic InfiniBand Annex A6 eeprom map specification
 */



#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>


#include "i2c_devices.h"
#include "timestamp.h"
#include "i2c_transceiver.h"
#include "macros.h"


#define i2c_transceiver_bit_get(var,bitnum)   ((var >> bitnum) & 1)
#define i2c_transceiver_bit_set(var,bitnum)   (var |=  (1 << bitnum))
#define i2c_transceiver_bit_clear(var,bitnum) (var &= ~(1 << bitnum))


#define I2C_TRANSCEIVER_HIDE_TRIDENT1_EEPROM_CONFLICT_ERROR_MESSAGE



/**
 * Initialises the transceiver device.
 *
 * Allows the transceiver to not be present, will prepare memory anyway and start probing for a present transceiver.
 * This is especially useful for CXP's which can be hot swapped.
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_init(i2c_dev *dev) {
	DO_AND_CHECK(i2c_transceiver_init_memory(dev));

	DO_AND_CHECK(i2c_transceiver_init_data(dev));

	return 0;
}



/**
 * Sets up the memory structure.
 *
 * Does no communication with the device, so is safe to use before the transceiver is connected.
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_init_memory(i2c_dev *dev) {
	dev->type = transceiver;

	i2c_transceiver_data *data = malloc(sizeof(*data));
	if (data == NULL) {
		printf("ERROR: Could not allocate memory for transceiver data\n");
		return -1;
	}

  dev->data = data;

	data->temperature = i2c_transceiver_data_temperature_DEFAULT;

	int i;
	for (i = 0; i < I2C_TRANSCEIVER_CHANNEL_COUNT; i++) {
		//data->channels[i].name = NULL;
	}

  return 0;
}




/**
 * Initialises all timestamps with expired timestamps.
 *
 * Used to update immediately on next polling cycle.
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_init_timestamp(i2c_dev *dev) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	timespec now;
  timestamp_get(&now);

  timestamp_subtract(&now, &i2c_transceiver_data_timestamp_offset,             &(data->timestamp));
  timestamp_subtract(&now, &i2c_transceiver_data_temperature_voltage_offset,   &(data->voltage.timestamp));
  timestamp_subtract(&now, &i2c_transceiver_data_temperature_timestamp_offset, &(data->temperature.timestamp));

/*
	int i;
	for (i = 0; i < I2C_TRANSCEIVER_CHANNEL_COUNT; i++) {
		timestamp_subtract(&now, &i2c_transceiver_data_channel_timestamp_offset, &(data->channels[i].timestamp));
	}
*/

  return 0;
}



/**
 * Initialises data structure from device data
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_init_data(i2c_dev *dev) {
	DO_AND_CHECK(i2c_transceiver_init_timestamp(dev));
	DO_AND_CHECK(i2c_transceiver_poll(dev));

  return 0;
}




/**
 * Initialises all static data from device data. This mostly encompasses configuration data and vendor information.
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_init_data_static(i2c_dev *dev) {
	DO_AND_CHECK(i2c_transceiver_poll_enable(dev));
	DO_AND_CHECK(i2c_transceiver_poll_vendor(dev));

  return 0;
}



/**
 * Polls the cable length from the transceiver device.
 *
 * Cable length detection does not work, do not use.
 *
 * Data sheets says in units of ".05m" but looks like it should be "0.5m"
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_poll_cablelength(i2c_dev *dev) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	DO_AND_CHECK(i2c_devices_command_and_read_uint16(dev, I2C_TRANSCEIVER_BYTE_XX_CABLELENGTH, &(data->cablelength)));

  return 0;
}



/**
 * Polls enable data for channel and output for rx&tx.
 *
 * Data in transceiver is stored as 1: disabled, 0: enabled.
 * Flips the bits to store in ram as 1: enabled, 0: disabled.
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_poll_enable(i2c_dev *dev) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	uint16_t buf;

	DO_AND_CHECK(i2c_transceiver_mode_set_tx(dev));
	DO_AND_CHECK(i2c_devices_command_and_read_uint16(dev, I2C_TRANSCEIVER_BYTE_TX_DISABLE_CHANNEL, &buf));
	data->data_tx.enable_channel = ~buf;
	DO_AND_CHECK(i2c_devices_command_and_read_uint16(dev, I2C_TRANSCEIVER_BYTE_TX_DISABLE_OUTPUT,  &buf));
	data->data_tx.enable_output =  ~buf;

	DO_AND_CHECK(i2c_transceiver_mode_set_rx(dev));
	DO_AND_CHECK(i2c_devices_command_and_read_uint16(dev, I2C_TRANSCEIVER_BYTE_RX_DISABLE_CHANNEL, &buf));
	data->data_rx.enable_channel = ~buf;
	DO_AND_CHECK(i2c_devices_command_and_read_uint16(dev, I2C_TRANSCEIVER_BYTE_RX_DISABLE_OUTPUT,  &buf));
	data->data_rx.enable_output =  ~buf;


  return 0;
}



/*
/-**
 * Waits until the transceiver is ready
 *
 * @param[in] dev memory structure of transceiver device
 *-/
int i2c_transceiver_ready_wait(i2c_dev *dev) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	timespec starttime;
	timestamp_get(&starttime);

	//loop until either transceiver ready or wait_max-time has passed
	do {
		DO_AND_CHECK(i2c_transceiver_ready(dev));
	} while ((!data->ready) &&
				 (timestamp_compare_offset(&starttime, &i2c_transceiver_init_timestamp_wait_max) < 0));

	if (!data->ready) {
		fprintf(stderr, "TRANSCEIVER ready, initialising.\n");
		return 0;
	} else {
		fprintf(stderr, "TRANSCEIVER not ready, aborting.\n");
		return -1;
	}
}
*/






/**
 * Read the data ready bit from the transceiver.
 *
 * If the device becomes ready then it will automatically update all static data.
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_ready(i2c_dev *dev) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	//ready bit in tx mode
	DO_AND_CHECK(i2c_transceiver_mode_set_tx(dev));

	uint8_t buf;
	DO_AND_CHECK(i2c_devices_command_and_read_uint8(dev, I2C_TRANSCEIVER_BYTE_READY, &buf));

	data->ready = !(buf & (1 << I2C_TRANSCEIVER_BYTE_READY_BIT_READY));

	if (data->ready) {
		DO_AND_CHECK(i2c_transceiver_init_data_static(dev));
	}

	return 0;
}





/**
 * Sets the transceiver device address to the TX sub module.
 *
 * Used for swapping communication between TX and RX mode
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_mode_set_tx(i2c_dev *dev) {
	dev->adress = I2C_TRANSCEIVER_ADDRESS_TX;

	return 0;
}


/**
 * Sets the transceiver device address to the RX sub module.
 *
 * Used for swapping communication between TX and RX mode
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_mode_set_rx(i2c_dev *dev) {
	dev->adress = I2C_TRANSCEIVER_ADDRESS_RX;

	return 0;
}








/**
 * Probes the transceiver device to check if it is connected.
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_probe(i2c_dev *dev) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	uint8_t buf_discard;
	uint8_t rx_connected, tx_connected;

	i2c_transceiver_mode_set_rx(dev);
	rx_connected = !i2c_devices_read_silent(dev, &buf_discard, 1);

	i2c_transceiver_mode_set_tx(dev);
	tx_connected = !i2c_devices_read_silent(dev, &buf_discard, 1);

	if (rx_connected && tx_connected) {
		data->connected = 1;
		return 0;
	} else {
		data->connected = 0;

		if (rx_connected ^ tx_connected) {
			#ifndef I2C_TRANSCEIVER_HIDE_TRIDENT1_EEPROM_CONFLICT_ERROR_MESSAGE
			fprintf(stderr, "ERROR: transceiver: rx_connected != tx_connected.\n");
			fprintf(stderr, "  -in the trident 1 this happens because the eeprom has 0xA0 as i2c address as well as the RX module\n");
			//return -1;
			#endif // I2C_TRANSCEIVER_HIDE_TRIDENT1_EEPROM_CONFLICT_ERROR_MESSAGE
			return 0;
		} else {
			//rx_connected == 0 && tx_connected == 0
			return 0;
		}
	}
}







/**
 * Poll the transceiver device
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_poll(i2c_dev *dev) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);


	DO_AND_CHECK(i2c_transceiver_poll_connected_and_ready(dev));

	if (!data->connected || !data->ready) {
		//do not poll if the device is not ready.
		return 0;
	}

	if (i2c_transceiver_poll_temperature(dev) ||
			i2c_transceiver_poll_voltage(dev) ||
			i2c_transceiver_poll_rx(dev)) {
		data->connected = 0;
		data->ready = 0;
		fprintf(stderr, "transceiver disconnected.\n");
	}

	return 0;
}




/**
 * Probe and poll the transceiver device connected and ready state.
 *
 * This function is rate limited.
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_poll_connected_and_ready(i2c_dev *dev) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	if (timestamp_compare_offset(&(data->timestamp),
									&i2c_transceiver_data_timestamp_offset) >= 0) {

    DO_AND_CHECK(timestamp_get(&(data->timestamp)));

		if (!data->connected) {
			DO_AND_CHECK(i2c_transceiver_probe(dev));

			if (!data->connected) {
				//transceiver disconnected, do not update.
				return 0;
			}
		}

		if (!data->ready) {
			DO_AND_CHECK(i2c_transceiver_ready(dev));

			if (!data->ready) {
				//transceiver not ready, do not update.
				return 0;
			}
		}
	}

	return 0;
}









/**
 * Poll the transceiver device voltages.
 *
 * This function is rate limited.
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_poll_voltage(i2c_dev *dev) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	if (timestamp_compare_offset(&(data->voltage.timestamp),
								&i2c_transceiver_data_temperature_voltage_offset) >= 0) {

		DO_AND_CHECK(i2c_transceiver_mode_set_tx(dev));
		DO_AND_CHECK(i2c_devices_command_and_read_uint16(dev, I2C_TRANSCEIVER_BYTE_TX_VCC33, &(data->voltage.TX) ));

		DO_AND_CHECK(i2c_transceiver_mode_set_rx(dev));
		DO_AND_CHECK(i2c_devices_command_and_read_uint16(dev, I2C_TRANSCEIVER_BYTE_TX_VCC33, &(data->voltage.RX) ));

    DO_AND_CHECK(timestamp_get(&(data->voltage.timestamp)));
	}

	return 0;
}



/**
 * Poll the transceiver device temperatures.
 *
 * This function is rate limited.
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_poll_temperature(i2c_dev *dev) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	if (timestamp_compare_offset(&(data->temperature.timestamp),
								&i2c_transceiver_data_temperature_timestamp_offset) >= 0) {

		DO_AND_CHECK(i2c_transceiver_mode_set_tx(dev));
    DO_AND_CHECK(i2c_transceiver_poll_temperature_single(dev,
								&(data->temperature.TX1), I2C_TRANSCEIVER_BYTE_TX_TEMP1));
		DO_AND_CHECK(i2c_transceiver_poll_temperature_single(dev,
								&(data->temperature.TX2), I2C_TRANSCEIVER_BYTE_TX_TEMP2));

		DO_AND_CHECK(i2c_transceiver_mode_set_rx(dev));
    DO_AND_CHECK(i2c_transceiver_poll_temperature_single(dev,
								&(data->temperature.RX1), I2C_TRANSCEIVER_BYTE_RX_TEMP1));
		DO_AND_CHECK(i2c_transceiver_poll_temperature_single(dev,
								&(data->temperature.RX2), I2C_TRANSCEIVER_BYTE_RX_TEMP2));

    DO_AND_CHECK(timestamp_get(&(data->temperature.timestamp)));
	}

	return 0;
}



/**
 * Poll the transceiver device RX submodule.
 *
 * Polling includes:
 *   - Loss of signal
 *
 * This function is rate limited.
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_poll_rx(i2c_dev *dev) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	if (timestamp_compare_offset(&(data->data_rx.timestamp),
								&i2c_transceiver_data_channel_timestamp_offset) >= 0) {

		DO_AND_CHECK(i2c_transceiver_mode_set_rx(dev));
		DO_AND_CHECK(i2c_devices_command_and_read_uint16(dev, I2C_TRANSCEIVER_BYTE_RX_LOSS_OF_SIGNAL, &(data->data_rx.loss_of_signal)));

    DO_AND_CHECK(timestamp_get(&(data->data_rx.timestamp)));
	}

	return 0;
}


/**
 * Poll a single temperature sensor of the bao.
 *
 * @param[in]  dev memory structure of transceiver device
 * @param[out] temp Temperature in microcelcius
 * @param[in]  byte_number byte number of the temperature register
 */
int i2c_transceiver_poll_temperature_single(i2c_dev *dev, int32_t *temp, uint8_t byte_number) {
	assert(dev->type == transceiver);

	uint16_t data_bytes;

	DO_AND_CHECK(i2c_devices_command_and_read_uint16(dev, byte_number, &data_bytes));
	DO_AND_CHECK(i2c_transceiver_temperature_bytes_to_uc(&data_bytes, temp));


	return 0;
}





/**
 * converts the temperature bytes received from the transceiver to microcelcius.
 *
 * The format is two's complement signed celcius for the first byte and unsigned 1/256 celcius for the second byte.
 *
 * @param[in]  data data bytes from transceiver
 * @param[out] temp temperature in microcelcius
 *
 */
int i2c_transceiver_temperature_bytes_to_uc(uint16_t *data, int32_t *temp) {
	const uint32_t uc_in_c = 1000*1000;
	const static uint32_t microcelcius_in_a_bit = (1000*1000) / 256;

	uint8_t data_fraction = *data & 0x00FF;

	uint32_t microcelcius_offset = data_fraction * microcelcius_in_a_bit;


	//printf("temp debug: bytes: 0x%04X, fraction: %3d, uc_offset: %6d\n", *data, data_fraction, microcelcius_offset);


	*temp = (((*data & 0xFF00) >> 8) * uc_in_c) + microcelcius_offset;

	return 0;
}



















/**
 * splits a temperature to celcius and microcelcius
 *
 * @param[in]  temp temperature in microcelcius
 * @param[out] temp_c  integer part of temperature
 * @param[out] temp_uc microcelcius part of temperature
 */
int i2c_transceiver_temperature_to_c_and_uc(const int32_t temp, int16_t *temp_c, uint32_t *temp_uc) {
	const uint32_t uc_in_c = 1000*1000;

	*temp_c = (temp + (0.5*uc_in_c)) / uc_in_c;

	*temp_uc = ( (temp < 0) ? (-temp) : (temp) ) % uc_in_c;

	return 0;
}



/**
 * converts a temperature to celcius and milicelcius
 *
 * @param[in]  temp temperature in microcelcius
 * @param[out] temp_c  integer part of temperature
 * @param[out] temp_mc milicelcius part of temperature
 */
int i2c_transceiver_temperature_to_c_and_mc(const int32_t temp, int16_t *temp_c, uint16_t *temp_mc) {
	uint32_t temp_uc;

	DO_AND_CHECK(i2c_transceiver_temperature_to_c_and_uc(temp, temp_c, &temp_uc));
	*temp_mc = (temp_uc + 500) / 1000;

	return 0;
}





/**
 * Reads vendor information from the transceiver.
 *
 * @param[in]     dev     memory structure of transceiver device
 */
int i2c_transceiver_poll_vendor(i2c_dev *dev) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	uint8_t command;

	command = I2C_TRANSCEIVER_BYTE_XX_VENDOR_NAME;
	DO_AND_CHECK(i2c_devices_write_and_read(dev, &command, 1, (uint8_t *) data->vendor_info.name, 16));
	data->vendor_info.name[16] = '\0';

	uint8_t buf[3];
	command = I2C_TRANSCEIVER_BYTE_XX_VENDOR_OUI;
	DO_AND_CHECK(i2c_devices_write_and_read(dev, &command, 1, buf, 3));
	data->vendor_info.OUI = (buf[2] << 16) | (buf[2] << 8) | (buf[2] << 0);

	command = I2C_TRANSCEIVER_BYTE_XX_VENDOR_PART_NUMBER;
	DO_AND_CHECK(i2c_devices_write_and_read(dev, &command, 1, (uint8_t *) data->vendor_info.part_number, 16));
	data->vendor_info.part_number[16] = '\0';

	command = I2C_TRANSCEIVER_BYTE_XX_VENDOR_REVISION_NUMBER;
	DO_AND_CHECK(i2c_devices_write_and_read(dev, &command, 1, (uint8_t *) data->vendor_info.revision, 2));
	data->vendor_info.revision[2] = '\0';

	command = I2C_TRANSCEIVER_BYTE_XX_VENDOR_SERIAL_NUMBER;
	DO_AND_CHECK(i2c_devices_write_and_read(dev, &command, 1, (uint8_t *) data->vendor_info.serial, 16));
	data->vendor_info.serial[16] = '\0';

	return 0;
}






//==============================================================================
//
// Start of functions to write configuration to the transceiver
//
//==============================================================================






/**
 * Sets the RX output amplitude control for a single channel.
 *
 * @param[in] dev        memory structure of transceiver device
 * @param[in] channel    channel the set configuration for.
 * @param[in] amplitude  4 bits, 0000b to 0111b allowed.
 */
int i2c_transceiver_rx_set_output_amplitude(i2c_dev *dev, uint8_t channel, uint8_t amplitude) {
	assert(dev->type == transceiver);

	if (channel >= I2C_TRANSCEIVER_CHANNEL_COUNT) {
		fprintf(stderr, "channel out of range.\n");
		return -1;
	}
	
	if (amplitude > 0x7) {
		fprintf(stderr, "amplitude out of range.\n");
		return -1;
	}

	uint8_t offset = (I2C_TRANSCEIVER_CHANNEL_COUNT - 1) - channel;
	uint8_t bytecode = I2C_TRANSCEIVER_BYTE_RX_OUTPUT_AMPLITUDE + (offset / 2);

	uint8_t byte_current;

	DO_AND_CHECK(i2c_devices_command_and_read_uint8(dev, bytecode, &byte_current));

	if (channel % 2) {
		byte_current = (byte_current & 0x0F) | (amplitude << 4);
	} else {
		byte_current = (byte_current & 0xF0) | (amplitude << 0);
	}
	
	DO_AND_CHECK(i2c_devices_command_and_write_uint8(dev, bytecode, &byte_current));

	return 0;
}





/**
 * Sets the RX output pre-emphasis control for a single channel.
 *
 * @param[in] dev       memory structure of transceiver device
 * @param[in] channel   channel the set configuration for.
 * @param[in] emphasis  4 bits, 0000b to 0111b allowed.
 */
int i2c_transceiver_rx_set_output_emphasis(i2c_dev *dev, uint8_t channel, uint8_t emphasis) {
	assert(dev->type == transceiver);

	if (channel >= I2C_TRANSCEIVER_CHANNEL_COUNT) {
		fprintf(stderr, "channel out of range.\n");
		return -1;
	}
	
	if (emphasis > 0x7) {
		fprintf(stderr, "emphasis out of range.\n");
		return -1;
	}

	uint8_t offset = (I2C_TRANSCEIVER_CHANNEL_COUNT - 1) - channel;
	uint8_t bytecode = I2C_TRANSCEIVER_BYTE_RX_OUTPUT_EMPHASIS + (offset / 2);

	uint8_t byte_current;

	DO_AND_CHECK(i2c_devices_command_and_read_uint8(dev, bytecode, &byte_current));

	if (channel % 2) {
		byte_current = (byte_current & 0x0F) | (emphasis << 4);
	} else {
		byte_current = (byte_current & 0xF0) | (emphasis << 0);
	}
	
	DO_AND_CHECK(i2c_devices_command_and_write_uint8(dev, bytecode, &byte_current));

	return 0;
}








/**
 * Enables or disables all channels and outputs on both the rx and tx submodules.
 *
 * Very inefficient but gets the job done and rarely used so not a bottleneck.
 *
 * @param[in] dev     memory structure of transceiver device
 * @param[in] enable  1: enable, 0: disable
 */
int i2c_transceiver_enable_all(i2c_dev *dev, uint8_t enable) {
	assert(dev->type == transceiver);

	uint8_t i;

	for (i = 0; i < I2C_TRANSCEIVER_CHANNEL_COUNT; i++) {
		DO_AND_CHECK(i2c_transceiver_rx_enable_channel(dev, i, enable));
		DO_AND_CHECK(i2c_transceiver_rx_enable_output( dev, i, enable));
		DO_AND_CHECK(i2c_transceiver_tx_enable_channel(dev, i, enable));
		DO_AND_CHECK(i2c_transceiver_tx_enable_output( dev, i, enable));
	}

	return 0;
}




/**
 * Enables or disables a channel on the rx submodule.
 *
 * @param[in] dev     memory structure of transceiver device
 * @param[in] channel channel number to operate on
 * @param[in] enable  1: enable, 0: disable
 */
int i2c_transceiver_rx_enable_channel(i2c_dev *dev, uint8_t channel, uint8_t enable) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	DO_AND_CHECK(i2c_transceiver_mode_set_rx(dev));
	return i2c_transceiver_bitmask_set(dev, channel, enable, I2C_TRANSCEIVER_BYTE_RX_DISABLE_CHANNEL, &(data->data_rx.enable_channel));
}

/**
 * Enables or disables a output on the rx submodule.
 *
 * @param[in] dev     memory structure of transceiver device
 * @param[in] channel channel number to operate on
 * @param[in] enable  1: enable, 0: disable
 */
int i2c_transceiver_rx_enable_output(i2c_dev *dev, uint8_t channel, uint8_t enable) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	DO_AND_CHECK(i2c_transceiver_mode_set_rx(dev));
	return i2c_transceiver_bitmask_set(dev, channel, enable, I2C_TRANSCEIVER_BYTE_RX_DISABLE_OUTPUT, &(data->data_rx.enable_output));
}



/**
 * Enables or disables a channel on the tx submodule.
 *
 * @param[in] dev     memory structure of transceiver device
 * @param[in] channel channel number to operate on
 * @param[in] enable  1: enable, 0: disable
 */
int i2c_transceiver_tx_enable_channel(i2c_dev *dev, uint8_t channel, uint8_t enable) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	DO_AND_CHECK(i2c_transceiver_mode_set_tx(dev));
	return i2c_transceiver_bitmask_set(dev, channel, enable, I2C_TRANSCEIVER_BYTE_TX_DISABLE_CHANNEL, &(data->data_tx.enable_channel));
}



/**
 * Enables or disables a output on the tx submodule.
 *
 * @param[in] dev     memory structure of transceiver device
 * @param[in] channel channel number to operate on
 * @param[in] enable  1: enable, 0: disable
 */
int i2c_transceiver_tx_enable_output(i2c_dev *dev, uint8_t channel, uint8_t enable) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	DO_AND_CHECK(i2c_transceiver_mode_set_tx(dev));
	return i2c_transceiver_bitmask_set(dev, channel, enable, I2C_TRANSCEIVER_BYTE_TX_DISABLE_OUTPUT, &(data->data_tx.enable_output));
}




/**
 * Enables or disables a bit in a mask block and writes it to the transceiver.
 *
 * works on 16 bit fields.
 * Flips bitmask before writing, used for enable<->disable
 *
 * @param[in]     dev     memory structure of transceiver device
 * @param[in]     channel channel number to operate on
 * @param[in]     enable  1: enable, 0: disable
 * @param[in]     command command to write for the bitmask field
 * @param[in/out] bitmask memory block holding bitmask
 */
int i2c_transceiver_bitmask_set(i2c_dev *dev, uint8_t channel, uint8_t enable, uint8_t command, uint16_t *bitmask) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	uint16_t bitmask_new;
	bitmask_new = *bitmask;

	if (channel >= I2C_TRANSCEIVER_CHANNEL_COUNT) {
		fprintf(stderr, "channel out of range.\n");
		return -1;
	}

	if (enable) {
		i2c_transceiver_bit_set(bitmask_new, channel);
	} else {
		i2c_transceiver_bit_clear(bitmask_new, channel);
	}
	
	if (bitmask_new != *bitmask) {
		//bitmask changed, write it out.
		//DO_AND_CHECK(i2c_transceiver_mode_set_rx(dev));

		uint16_t buf = ~bitmask_new;
		if (i2c_devices_command_and_write_uint16(dev, command, &buf)) {
			data->ready     = 0;
			data->connected = 0;
			return -1;
		} else {
			*bitmask = bitmask_new;
			return 0;
		}
	}

	return 0;
}


























//==============================================================================
//
// Start of debug reporting statements
//
//==============================================================================



/**
 * reports the temperature readings of the transceiver to the terminal
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_report_vendor(i2c_dev *dev) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	printf("    Vendor info: name: %16s, OUI: %5d, part: %16s, revision: %2s, serial: %16s\n",
			   data->vendor_info.name,
			   data->vendor_info.OUI,
			   data->vendor_info.part_number,
			   data->vendor_info.revision,
				 data->vendor_info.serial);

	return 0;
}


/**
 * reports the temperature readings of the transceiver to the terminal
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_report_temperature(i2c_dev *dev) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	int16_t temp_c[4];
	uint16_t temp_mc[4];

	DO_AND_CHECK(i2c_transceiver_temperature_to_c_and_mc(data->temperature.TX1, &(temp_c[0]), &(temp_mc[0])));
	DO_AND_CHECK(i2c_transceiver_temperature_to_c_and_mc(data->temperature.TX2, &(temp_c[1]), &(temp_mc[1])));
	DO_AND_CHECK(i2c_transceiver_temperature_to_c_and_mc(data->temperature.RX1, &(temp_c[2]), &(temp_mc[2])));
	DO_AND_CHECK(i2c_transceiver_temperature_to_c_and_mc(data->temperature.RX2, &(temp_c[3]), &(temp_mc[3])));

	printf("    transceiver temperatures: TX1: %3d.%03d c, "
						  								 "TX2: %3d.%03d c, "
							  							 "RX1: %3d.%03d c, "
								  						 "RX2: %3d.%03d c\n",
									  					  temp_c[0], temp_mc[0],
											  			  temp_c[1], temp_mc[1],
												  		  temp_c[2], temp_mc[2],
													  	  temp_c[3], temp_mc[3]);

	return 0;
}


/**
 * reports the voltage readings of the transceiver to the terminal
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_report_voltage(i2c_dev *dev) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	printf("    transceiver voltages: TX: %4d.%01d mV, RX: %4d.%01d mV\n",
					  (data->voltage.TX) / 10, (data->voltage.TX) % 10,
					  (data->voltage.RX) / 10, (data->voltage.RX) % 10);

	return 0;
}


/**
 * reports the connected and ready state of the transceiver to the terminal
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_report_connected_and_ready(i2c_dev *dev) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	printf("    transceiver_connected: %01d, transceiver_ready: %01d\n", data->connected, data->ready);

	return 0;
}


/**
 * reports the device id of the transceiver to the terminal
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_report_devid(i2c_dev *dev) {
	assert(dev->type == transceiver);

	printf("  transceiver devid: %4X\n", dev->dev_id);

	return 0;
}


/**
 * reports the cable length to the terminal.
 *
 * Cable length detection does not work, do not use.
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_report_cablelength(i2c_dev *dev) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	// convert units of 0.5m to meters + half meter for easy of reading
	printf("    transceiver cable length: %5d.%01d meters\n", (data->cablelength)/2, 5*(data->cablelength & 1) );

	return 0;
}


/**
 * reports a row of channel numbers to the terminal
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_report_channel_numbers(i2c_dev *dev) {
	assert(dev->type == transceiver);

	int i;

	printf("      channel:           ");
	for (i = (I2C_TRANSCEIVER_CHANNEL_COUNT - 1); i >= 0; i--) {
		printf("%2d,", i);
	}
	printf("\n");

	return 0;
}


/**
 * reports the enabled data to the terminal
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_report_enable(i2c_dev *dev) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	int i;

	printf("      tx_enable_channel: ");
	for (i = (I2C_TRANSCEIVER_CHANNEL_COUNT - 1); i >= 0; i--) {
		printf(" %1d,", i2c_transceiver_bit_get(data->data_tx.enable_channel, i));
	}
	printf("\n");
	printf("      tx_enable_output:  ");
	for (i = (I2C_TRANSCEIVER_CHANNEL_COUNT - 1); i >= 0; i--) {
		printf(" %1d,", i2c_transceiver_bit_get(data->data_tx.enable_output, i));
	}
	printf("\n");
	printf("      rx_enable_channel: ");
	for (i = (I2C_TRANSCEIVER_CHANNEL_COUNT - 1); i >= 0; i--) {
		printf(" %1d,", i2c_transceiver_bit_get(data->data_rx.enable_channel, i));
	}
	printf("\n");
	printf("      rx_enable_output:  ");
	for (i = (I2C_TRANSCEIVER_CHANNEL_COUNT - 1); i >= 0; i--) {
		printf(" %1d,", i2c_transceiver_bit_get(data->data_rx.enable_output, i));
	}
	printf("\n");


	return 0;
}


/**
 * reports loss of signal on the RX to the terminal
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_report_los(i2c_dev *dev) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	int i;

	printf("      loss of signal:    ");
//printf("      channel:           ");
	for (i = (I2C_TRANSCEIVER_CHANNEL_COUNT - 1); i >= 0; i--) {
		printf(" %1d,", i2c_transceiver_bit_get(data->data_rx.loss_of_signal, i));
	}
	printf("\n");

	return 0;
}


/**
 * reports all readings of the transceiver to the terminal
 *
 * @param[in] dev memory structure of transceiver device
 */
int i2c_transceiver_report_all(i2c_dev *dev) {
	assert(dev->type == transceiver);
	i2c_transceiver_data *data = ((i2c_transceiver_data*) dev->data);

	DO_AND_CHECK(i2c_transceiver_report_devid(dev));
	DO_AND_CHECK(i2c_transceiver_report_connected_and_ready(dev));

	if (data->connected && data->ready) {
		DO_AND_CHECK(i2c_transceiver_report_vendor(dev));
		DO_AND_CHECK(i2c_transceiver_report_temperature(dev));
		DO_AND_CHECK(i2c_transceiver_report_voltage(dev));
		//DO_AND_CHECK(i2c_transceiver_report_cablelength(dev));
		DO_AND_CHECK(i2c_transceiver_report_channel_numbers(dev));
		DO_AND_CHECK(i2c_transceiver_report_enable(dev));
		DO_AND_CHECK(i2c_transceiver_report_los(dev));
	}

	return 0;
}




































