/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-09
 *
 *---------------------------------------------------------------------------
 */

#ifndef I2C_TRANSCEIVER_H
#define I2C_TRANSCEIVER_H



/// @todo check channel count is 12 for cxp's as well as transceiver's
//#define I2C_TRANSCEIVER_CHANNEL_COUNT   8
#define I2C_TRANSCEIVER_CHANNEL_COUNT   12




#define I2C_TRANSCEIVER_ADDRESS_TX   0xA0
#define I2C_TRANSCEIVER_ADDRESS_RX   0xA8

//#define I2C_TRANSCEIVER_BYTE_
#define I2C_TRANSCEIVER_BYTE_READY            2
#define I2C_TRANSCEIVER_BYTE_READY_BIT_READY  0

#define I2C_TRANSCEIVER_BYTE_XX_TEMP1                   22
#define I2C_TRANSCEIVER_BYTE_XX_TEMP2                   24
#define I2C_TRANSCEIVER_BYTE_TX_TEMP1                   I2C_TRANSCEIVER_BYTE_XX_TEMP1
#define I2C_TRANSCEIVER_BYTE_TX_TEMP2                   I2C_TRANSCEIVER_BYTE_XX_TEMP2
#define I2C_TRANSCEIVER_BYTE_RX_TEMP1                   I2C_TRANSCEIVER_BYTE_XX_TEMP1
#define I2C_TRANSCEIVER_BYTE_RX_TEMP2                   I2C_TRANSCEIVER_BYTE_XX_TEMP2

#define I2C_TRANSCEIVER_BYTE_XX_VCC33                   26
#define I2C_TRANSCEIVER_BYTE_TX_VCC33                   I2C_TRANSCEIVER_BYTE_XX_VCC33
#define I2C_TRANSCEIVER_BYTE_RX_VCC33                   I2C_TRANSCEIVER_BYTE_XX_VCC33

#define I2C_TRANSCEIVER_BYTE_XX_DISABLE_CHANNEL         52
#define I2C_TRANSCEIVER_BYTE_TX_DISABLE_CHANNEL         I2C_TRANSCEIVER_BYTE_XX_DISABLE_CHANNEL
#define I2C_TRANSCEIVER_BYTE_RX_DISABLE_CHANNEL         I2C_TRANSCEIVER_BYTE_XX_DISABLE_CHANNEL
#define I2C_TRANSCEIVER_BYTE_XX_DISABLE_OUTPUT          54
#define I2C_TRANSCEIVER_BYTE_TX_DISABLE_OUTPUT          I2C_TRANSCEIVER_BYTE_XX_DISABLE_OUTPUT
#define I2C_TRANSCEIVER_BYTE_RX_DISABLE_OUTPUT          I2C_TRANSCEIVER_BYTE_XX_DISABLE_OUTPUT

#define I2C_TRANSCEIVER_BYTE_RX_LOSS_OF_SIGNAL          7

#define I2C_TRANSCEIVER_BYTE_RX_OUTPUT_AMPLITUDE        62
#define I2C_TRANSCEIVER_BYTE_RX_OUTPUT_EMPHASIS         68

#define I2C_TRANSCEIVER_BYTE_XX_CABLELENGTH             150

#define I2C_TRANSCEIVER_BYTE_XX_VENDOR_NAME             152
#define I2C_TRANSCEIVER_BYTE_XX_VENDOR_OUI              168
#define I2C_TRANSCEIVER_BYTE_XX_VENDOR_PART_NUMBER      171
#define I2C_TRANSCEIVER_BYTE_XX_VENDOR_REVISION_NUMBER  187
#define I2C_TRANSCEIVER_BYTE_XX_VENDOR_SERIAL_NUMBER    189



/*
typedef struct i2c_transceiver_channel_data{
	//char    *name;
	timespec              timestamp;
}i2c_transceiver_channel_data;
*/

///all temperatures in microcelcius
typedef struct i2c_transceiver_data_temperature{
	timespec              timestamp;
	int32_t               TX1;
	int32_t               TX2;
	int32_t               RX1;
	int32_t               RX2;
}i2c_transceiver_data_temperature;

///all temperatures in 100uV
typedef struct i2c_transceiver_data_voltage{
	timespec              timestamp;
	uint16_t               TX;
	uint16_t               RX;
}i2c_transceiver_data_voltage;



typedef struct i2c_transceiver_data_vendor{
	char              name[17];
	uint32_t          OUI; //IEEE ID
	char              part_number[17];
	char              revision[3];
	char              serial[17];
}i2c_transceiver_data_vendor;

typedef struct i2c_transceiver_data_tx{
//	timespec                  timestamp;
	uint16_t					enable_channel;
	uint16_t					enable_output;
}i2c_transceiver_data_tx;

typedef struct i2c_transceiver_data_rx{
	uint16_t					enable_channel;
	uint16_t					enable_output;
	timespec          timestamp;
	uint16_t          loss_of_signal;
}i2c_transceiver_data_rx;


typedef struct i2c_transceiver_data{
	timespec                  timestamp;
	uint8_t                   connected;
	uint8_t                   ready;
//	i2c_transceiver_channel_data      channels[I2C_TRANSCEIVER_CHANNEL_COUNT];
	i2c_transceiver_data_temperature  temperature;
	i2c_transceiver_data_voltage      voltage;
	i2c_transceiver_data_tx						data_tx;
	i2c_transceiver_data_rx						data_rx;
	uint16_t                  cablelength; ///<units of 0.5m, datasheet possibly incorrect
	i2c_transceiver_data_vendor       vendor_info;
}i2c_transceiver_data;



/*
const static i2c_transceiver_channel_data
i2c_transceiver_channel_data_DEFAULT = {
	//"",
	{0,0}
};
*/

const static i2c_transceiver_data_temperature
i2c_transceiver_data_temperature_DEFAULT = {
	{0,0},
	0,
	0,
	0,
	0
};





const static timespec
i2c_transceiver_timestamp_offset_default_slow = {
	0,
	250*NSEC_IN_A_MSEC
};

const static timespec
i2c_transceiver_timestamp_offset_default_fast = {
	0,
	100*NSEC_IN_A_MSEC
};

//uses defines instead of assignment as assigning a constant struct is not allowed
#define i2c_transceiver_data_timestamp_offset              i2c_transceiver_timestamp_offset_default_fast
#define i2c_transceiver_data_temperature_voltage_offset    i2c_transceiver_timestamp_offset_default_fast
#define i2c_transceiver_data_temperature_timestamp_offset  i2c_transceiver_timestamp_offset_default_slow
#define i2c_transceiver_data_channel_timestamp_offset      i2c_transceiver_timestamp_offset_default_slow




/*
const static timespec
i2c_transceiver_init_timestamp_wait_max = {
	1,
	0
};
*/
















#define i2c_transceiver_bit_get(var,bitnum)   ((var >> bitnum) & 1)
#define i2c_transceiver_bit_set(var,bitnum)   (var |=  (1 << bitnum))
#define i2c_transceiver_bit_clear(var,bitnum) (var &= ~(1 << bitnum))







int i2c_transceiver_init(i2c_dev *dev);
int i2c_transceiver_init_memory(i2c_dev *dev);
int i2c_transceiver_init_timestamp(i2c_dev *dev);
int i2c_transceiver_init_data(i2c_dev *dev);
int i2c_transceiver_init_data_static(i2c_dev *dev);
int i2c_transceiver_poll_enable(i2c_dev *dev);
int i2c_transceiver_ready_wait(i2c_dev *dev);
int i2c_transceiver_ready(i2c_dev *dev);
int i2c_transceiver_mode_set_tx(i2c_dev *dev);
int i2c_transceiver_mode_set_rx(i2c_dev *dev);
int i2c_transceiver_probe(i2c_dev *dev);
int i2c_transceiver_poll(i2c_dev *dev);
int i2c_transceiver_poll_connected_and_ready(i2c_dev *dev);
int i2c_transceiver_poll_voltage(i2c_dev *dev);
int i2c_transceiver_poll_temperature(i2c_dev *dev);
int i2c_transceiver_poll_rx(i2c_dev *dev);
int i2c_transceiver_poll_temperature_single(i2c_dev *dev, int32_t *temp, uint8_t byte_number);
int i2c_transceiver_temperature_bytes_to_uc(uint16_t *data, int32_t *temp);
int i2c_transceiver_temperature_to_c_and_uc(const int32_t temp, int16_t *temp_c, uint32_t *temp_uc);
int i2c_transceiver_temperature_to_c_and_mc(const int32_t temp, int16_t *temp_c, uint16_t *temp_mc);
int i2c_transceiver_poll_vendor(i2c_dev *dev);


int i2c_transceiver_rx_set_output_amplitude(i2c_dev *dev, uint8_t channel, uint8_t amplitude);
int i2c_transceiver_rx_set_output_emphasis(i2c_dev *dev, uint8_t channel, uint8_t emphasis);
int i2c_transceiver_enable_all(i2c_dev *dev, uint8_t enable);
int i2c_transceiver_rx_enable_channel(i2c_dev *dev, uint8_t channel, uint8_t enable);
int i2c_transceiver_rx_enable_output(i2c_dev *dev, uint8_t channel, uint8_t enable);
int i2c_transceiver_tx_enable_channel(i2c_dev *dev, uint8_t channel, uint8_t enable);
int i2c_transceiver_tx_enable_output(i2c_dev *dev, uint8_t channel, uint8_t enable);
int i2c_transceiver_bitmask_set(i2c_dev *dev, uint8_t channel, uint8_t enable, uint8_t command, uint16_t *bitmask);


int i2c_transceiver_report_vendor(i2c_dev *dev);
int i2c_transceiver_report_temperature(i2c_dev *dev);
int i2c_transceiver_report_voltage(i2c_dev *dev);
int i2c_transceiver_report_connected_and_ready(i2c_dev *dev);
int i2c_transceiver_report_devid(i2c_dev *dev);
int i2c_transceiver_report_cablelength(i2c_dev *dev);
int i2c_transceiver_report_channel_numbers(i2c_dev *dev);
int i2c_transceiver_report_enable(i2c_dev *dev);
int i2c_transceiver_report_los(i2c_dev *dev);
int i2c_transceiver_report_all(i2c_dev *dev);



#endif /* I2C_TRANSCEIVER_H */
