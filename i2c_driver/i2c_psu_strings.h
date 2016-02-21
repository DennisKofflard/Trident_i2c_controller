/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-11-24
 *
 *---------------------------------------------------------------------------
 */


#ifndef I2C_PSU_STRINGS_H
#define I2C_PSU_STRINGS_H


int psu_status_word_bit_to_str_flipped(uint8_t bitpos, uint8_t bitset, char str[], size_t maxlen);
int psu_status_word_bit_to_str(uint8_t bitpos, uint8_t bitset, char str[], size_t maxlen);
int psu_status_vout_bit_to_str(uint8_t bitpos, uint8_t bitset, char str[], size_t maxlen);
int psu_status_iout_bit_to_str(uint8_t bitpos, uint8_t bitset, char str[], size_t maxlen);


#endif /* I2C_PSU_STRINGS_H */
