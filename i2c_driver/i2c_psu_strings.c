/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-11-24
 *
 *---------------------------------------------------------------------------
 */


#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "i2c_psu_strings.h"

int psu_status_word_bit_to_str_flipped(uint8_t bitpos, uint8_t bitset, char str[], size_t maxlen) {
  char *strptr;
  switch(bitpos) {
    case 15:
      strptr = "reserved, bit 15 / high-7";
      break;
    case 14:
      strptr = "OFF - not switching";
      break;
    case 13:
      strptr = "VOUT_OV - over voltage protection";
      break;
    case 12:
      strptr = "IOUT_OC - latched off due to over current protection";
      break;
    case 11:
      strptr = "reserved, bit 11 / high-2";
      break;
    case 10:
      strptr = "reserved, bit 10 / high-2";
      break;
    case  9:
      strptr = "CML - communication, memory or logic fault occured";
      break;
    case  8:
      strptr = "none of the above - a fault has occured which does not fall into any other category";
      break;
    case  7:
      strptr = "Vout - voltage outside power good limits, bits set in Vout";
      break;
    case  6:
      strptr = "Iout/Pout - current and/or power exceed limits, bits set in Iout";
      break;
    case  5:
      strptr = "reserved, bit 5 / low-5";
      break;
    case  4:
      strptr = "MFR - manufacturer specific warning or fault";
      break;
    case  3:
      strptr = "not power good - power good signal deasserted";
      break;
    case  2:
      strptr = "reserved, bit 2 / low-2";
      break;
    case  1:
      strptr = "OTHER - bits set in status other";
      break;
    case  0:
      strptr = "reserved, bit 0 / low-0";
      break;
    default:
      return -1;
      break;
   }

  //printf("status string length: %zd\n", strlen(strptr));

  if ((strlen(strptr)+1) > maxlen) {
    printf("string buffer to small\n");
    return -1;
  }

  strcpy(str, strptr);

  return 0;
}


int psu_status_word_bit_to_str(uint8_t bitpos, uint8_t bitset, char str[], size_t maxlen) {
  char *strptr;
  switch(bitpos) {
    case 15:
      strptr = "Vout - voltage outside power good limits, bits set in Vout";
      break;
    case 14:
      strptr = "Iout/Pout - current and/or power exceed limits, bits set in Iout";
      break;
    case 13:
      strptr = "reserved, bit 13 / high-6";
      break;
    case 12:
      strptr = "MFR - manufacturer specific warning or fault";
      break;
    case 11:
      strptr = "not power good - power good signal deasserted";
      break;
    case 10:
      strptr = "reserved, bit 10 / high-2";
      break;
    case  9:
      strptr = "OTHER - bits set in status other";
      break;
    case  8:
      strptr = "reserved, bit 8 / high-0";
      break;
    case  7:
      strptr = "reserved, bit 7 / low-7";
      break;
    case  6:
      strptr = "OFF - not switching";
      break;
    case  5:
      strptr = "VOUT_OV - over voltage protection";
      break;
    case  4:
      strptr = "IOUT_OC - latched off due to over current protection";
      break;
    case  3:
      strptr = "reserved, bit 2 / low-2";
      break;
    case  2:
      strptr = "reserved, bit 2 / low-2";
      break;
    case  1:
      strptr = "CML - communication, memory or logic fault occured";
      break;
    case  0:
      strptr = "none of the above - a fault has occured which does not fall into any other category";
      break;
    default:
      return -1;
      break;
   }

  //printf("status string length: %zd\n", strlen(strptr));

  if ((strlen(strptr)+1) > maxlen) {
    printf("string buffer to small\n");
    return -1;
  }

  strcpy(str, strptr);

  return 0;
}


int psu_status_vout_bit_to_str(uint8_t bitpos, uint8_t bitset, char str[], size_t maxlen) {
  char *strptr;
  switch(bitpos) {
    case 7:
      strptr = "Vout over voltage fault";
      break;
    case 6:
      strptr = "Vout over voltage warning";
      break;
    case 5:
      strptr = "Vout under voltage warning";
      break;
    case 4:
      strptr = "Vout under voltage limit - not applicable";
      break;
    case 3:
      strptr = "reserved, bit 3";
      break;
    case 2:
      strptr = "reserved, bit 2";
      break;
    case 1:
      strptr = "reserved, bit 1";
      break;
    case 0:
      strptr = "reserved, bit 0";
      break;
    default:
      return -1;
      break;
   }

  //printf("status string length: %zd\n", strlen(strptr));

  if ((strlen(strptr)+1) > maxlen) {
    printf("string buffer to small\n");
    return -1;
  }

  strcpy(str, strptr);

  return 0;
}


int psu_status_iout_bit_to_str(uint8_t bitpos, uint8_t bitset, char str[], size_t maxlen) {
  char *strptr;
  switch(bitpos) {
    case 7:
      strptr = "Iout over current fault";
      break;
    case 6:
      strptr = "reserved, bit 6";
      break;
    case 5:
      strptr = "Iout over current warning";
      break;
    case 4:
      strptr = "reserved, bit 4";
      break;
    case 3:
      strptr = "reserved, bit 3";
      break;
    case 2:
      strptr = "reserved, bit 2";
      break;
    case 1:
      strptr = "Pout over power warning - Pout exceeds not-fault limit";
      break;
    case 0:
      strptr = "Pout over power warning fault - Pout exceeds warn limit";
      break;
    default:
      return -1;
      break;
   }

  //printf("status string length: %zd\n", strlen(strptr));

  if ((strlen(strptr)+1) > maxlen) {
    printf("string buffer to small\n");
    return -1;
  }

  strcpy(str, strptr);

  return 0;
}


int psu_status_operation_bit_to_str(uint8_t bitpos, uint8_t bitset, char str[], size_t maxlen) {
  char *strptr;
  switch(bitpos) {
    case 7:
      strptr = "reserved, bit 7";
      break;
    case 6:
      strptr = "reserved, bit 6";
      break;
    case 5:
      strptr = "reserved, bit 5";
      break;
    case 4:
      strptr = "reserved, bit 4";
      break;
    case 3:
      strptr = "";
      break;
    case 2:
      strptr = "";
      break;
    case 1:
      strptr = "";
      break;
    case 0:
      strptr = "";
      break;
    default:
      return -1;
      break;
   }

  //printf("status string length: %zd\n", strlen(strptr));

  if ((strlen(strptr)+1) > maxlen) {
    printf("string buffer to small\n");
    return -1;
  }

  strcpy(str, strptr);

  return 0;
}
