/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-11-24
 *
 *---------------------------------------------------------------------------
 */

#ifndef I2C_DAS_H
#define I2C_DAS_H


#include "timestamp.h"



#define I2C_DAS_CHANNEL_COUNT   8
#define I2C_DAS_RANGE_LOW_MAX   5
#define I2C_DAS_RANGE_HIGH_MAX  10
#define I2C_DAS_BITS            12

#define I2C_DAS_V_TO_UV(x)  ((x) * 1000*1000)
#define I2C_DAS_UV_TO_MV(x) (((x) + 500) / 1000)



typedef struct i2c_das_channel_data{
	char    *name;
	uint8_t enabled;
	uint8_t range;
	uint8_t bipolar;
	int16_t milivolts;
	timespec timestamp;
}i2c_das_channel_data;

const static i2c_das_channel_data
i2c_das_channel_data_DEFAULT = {
	"",
	0,
	1,
	1,
	0,
	{0,0}
};

typedef struct i2c_das_data{
	i2c_das_channel_data  channels[I2C_DAS_CHANNEL_COUNT];
	uint8_t               powerdown;
}i2c_das_data;


const static timespec
i2c_das_channel_timestamp_offset = {
	0,
	100*NSEC_IN_A_MSEC //poll every 100ms
};




int i2c_das_init(i2c_dev *dev);


int i2c_das_channel_set(i2c_das_channel_data *channel_data,
												uint8_t enabled,
												uint8_t range,
												uint8_t bipolar,
												char *name);


int i2c_das_read(i2c_dev *dev, uint8_t channel);
int i2c_das_dacval_to_mv(i2c_das_channel_data *channel_data, uint16_t dacval);

int i2c_das_report(i2c_dev *dev, uint8_t channel);
int i2c_das_report_all(i2c_dev *dev);
int i2c_das_do_on_channels(i2c_dev *dev, int (*func)(i2c_dev*, uint8_t));

int i2c_das_get_channel_data(i2c_dev *dev, uint8_t channel, i2c_das_channel_data **channel_data_ptr);

int i2c_das_poll_channel(i2c_dev *dev, uint8_t channel);
int i2c_das_poll(i2c_dev *dev);


#endif /* I2C_DAS_H */
