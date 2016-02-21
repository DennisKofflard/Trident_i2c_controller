/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-11-24
 *
 *---------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdint.h>

#include "i2c_devices.h"
#include "i2c_psu.h"
#include "i2c_psu_strings.h"




int i2c_psu_status_report_generic(uint8_t bitcount,
																	uint16_t data,
																	int (*bit_to_str)(uint8_t, uint8_t, char*, size_t)) {
  //printf("data block: 0x%04X\n", data);

  int i;
  for (i = (bitcount-1); i >= 0; i--) {
    int bitset = (data >> i) & 1;

    if (bitset) {
      size_t strlen = 128;
      char str[strlen];

      if ((*bit_to_str)(i, bitset, str, strlen)) {
        printf("failed to lookup bit to string\n");
        return -1;
      }

      printf("bit %2d set - %s\n", i, str);
    }
  }

  return 0;
}



int i2c_psu_status_word_report(uint16_t status_word) {
  return i2c_psu_status_report_generic(16, status_word, psu_status_word_bit_to_str);
}


int i2c_psu_status_vout_report(uint8_t status_vout) {
  return i2c_psu_status_report_generic(8, status_vout, psu_status_vout_bit_to_str);
}


int i2c_psu_status_iout_report(uint8_t status_iout) {
  return i2c_psu_status_report_generic(8, status_iout, psu_status_iout_bit_to_str);
}




int i2c_psu_status_report(i2c_dev *dev) {
  uint16_t status_word;
  if (i2c_psu_status_word_get(dev, &status_word)) {
    printf("could not get psu status word.\n");
    return -1;
  }

  if (i2c_psu_status_word_report(status_word)) {
    printf("failed to print out status word\n");
  }

  //if Vout bit set report Vout status
  if ((status_word >> 15) & 1) {
    uint8_t status_vout;

    if (i2c_psu_status_vout_get(dev, &status_vout)) {
      printf("could not get psu status vout but the bit is set.\n");
      return -1;
    }

    if (i2c_psu_status_vout_report(status_vout)) {
      printf("failed to print out status vout\n");
    }
  }

  //if Iout bit set report Iout status
  if ((status_word >> 14) & 1) {
    uint8_t status_iout;

    if (i2c_psu_status_iout_get(dev, &status_iout)) {
      printf("could not get psu status iout but the bit is set.\n");
      return -1;
    }

    if (i2c_psu_status_iout_report(status_iout)) {
      printf("failed to print out status iout\n");
    }
  }

  return 0;
}


int i2c_psu_write_protect_report(i2c_dev *dev) {
  uint8_t data;
  if (i2c_psu_write_protect_get(dev, &data)) {
    printf("could not read write protect byte\n");
    return -1;
  }

  printf("write protect byte: 0x%02X\n",data);

  return 0;
}


int i2c_psu_enable_report(i2c_dev *dev) {
  uint8_t data;
  if (i2c_psu_enable_get(dev, &data)) {
    printf("could not read ON_OFF_Config byte\n");
    return -1;
  }

  printf("ON_OFF_config byte: 0x%02X\n", data);

  return 0;
}


int i2c_psu_operation_report(i2c_dev *dev) {
  uint8_t data;
  if (i2c_psu_operation_get(dev, &data)) {
    printf("could not read operations byte\n");
    return -1;
  }

  printf("\"operation\" byte: 0x%02X\n", data);

	printf("Operation status: ");

  if        (0x00 == (0xC0 & data)) {
		printf("Immediate Off\n");
  } else if (0x40 == (0xC0 & data)) {
		printf("Soft Off\n");
  } else if (0x80 == (0xF0 & data)) {
		printf("On\n");
  } else if (0x98 == (0xFC & data)) {
		printf("Margin Low, act on fault\n");
  } else if (0xA8 == (0xFC & data)) {
		printf("Margin High, act on fault\n");
  }

  return 0;
}


int i2c_psu_voltage_regulation_configuration_report(i2c_dev *dev) {
  uint8_t data;
  if (i2c_psu_voltage_regulation_configuration_get(dev, &data)) {
    printf("could not read VRbyte byte\n");
    return -1;
  }

  printf("VRconf byte: 0x%02X\n", data);

  return 0;
}





int i2c_psu_voltage_report(i2c_dev *dev) {
  uint16_t data;
  if (i2c_psu_voltage_get(dev, &data)) {
    printf("could not read Read_Vout byte\n");
    return -1;
  }

  printf("Voltage bytes: 0x%04X\n", data);

  return 0;
}




int i2c_psu_current_report(i2c_dev *dev) {
  uint16_t data;
  if (i2c_psu_current_get(dev, &data)) {
    printf("could not read Read_Iout byte\n");
    return -1;
  }

  printf("Current bytes: 0x%04X\n", data);

  return 0;
}


int i2c_psu_power_report(i2c_dev *dev) {
  uint16_t data;
  if (i2c_psu_power_get(dev, &data)) {
    printf("could not read Read_Iout byte\n");
    return -1;
  }

  printf("Powers bytes: 0x%04X\n", data);

  return 0;
}





