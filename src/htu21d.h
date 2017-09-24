/*
 * htu21d.h
 *
 *  Created on: 2016-09-09
 *      Author: DMB-220
 */

#ifndef HTU21D_H_
#define HTU21D_H_

#include "i2c.h"

// Slave address for HTU21D chip (7-bit format)
#define HTU21D_DEVICE_ADDRESS		0x40
#define HTU21D_TEMP_ADDRESS			0xE3
#define HTU21D_HUM_ADDRESS			0xE5

#define CRC8_POLYNOMINAL 				0x13100 /* CRC8 polynomial for 16bit CRC8 x^8 + x^5 + x^4 + 1 */

//HTU21D
float temperature;
float humanity;
float compensated_humidity;


uint8_t htu21_checkCRC8(uint16_t data){
	uint8_t bit;

	for (bit = 0; bit < 16; bit++){
		if (data & 0x8000){
			data = (data << 1) ^ CRC8_POLYNOMINAL;}else{
			data <<= 1;
		}
	}
	data >>= 8;

	return data;
}

float htu21d_temp(){
	uint8_t temp[2];
	uint16_t raw_temperature;
	uint8_t crc;

	i2c_read_multi_with_reg(HTU21D_DEVICE_ADDRESS, HTU21D_TEMP_ADDRESS, 3, temp);
	raw_temperature = temp[0];
	raw_temperature <<= 8;
	raw_temperature |= temp[1];

	crc = temp[2];

	/* Check CRC8 */
	if(htu21_checkCRC8(raw_temperature) != crc)
		return 0.00;

	/* Calculate temperature */
	temperature = -46.85 + 0.002681 * (float)raw_temperature;
	return temperature;
}

float htu21d_hum(){
	uint8_t hum[2];
	uint16_t raw_humanity;
	uint8_t crc;

	i2c_read_multi_with_reg(HTU21D_DEVICE_ADDRESS, HTU21D_HUM_ADDRESS, 3, hum);
	raw_humanity = hum[0];
	raw_humanity <<= 8;
	raw_humanity |= hum[1];

	crc = hum[2];

	/* Check CRC8 */
	if(htu21_checkCRC8(raw_humanity) != crc)
		return 0.00;

	/* clear two last status bits */
	raw_humanity ^= 0x02;
	/* Calculate temperature */
	humanity = -6.00 + 0.001907 * (float)raw_humanity;
	return humanity;
}


#endif /* HTU21D_H_ */
