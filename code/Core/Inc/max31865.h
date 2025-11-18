/*
 * max31865.h
 *
 *  Created on: Nov 9, 2025
 *      Author: Robert Mihalyffy
 */

#ifndef INC_MAX31865_H_
#define INC_MAX31865_H_

// defining registers
#define configuration 0x0
#define rtdMSBs 0x1
#define rtdLSBs 0x2
#define HighFaultThresholdMSB 0x3
#define HighFaultThresholdLSB 0x4
#define LowFaultThresholdMSB 0x5
#define LowFaultThresholdLSB 0x6
#define faultStatus 0x7


typedef struct
{
	// interface function pointers
	void(*delay)(uint32_t); // milliseconds delay
	void(*cs)(uint8_t); // sets the CS pin's state
	uint8_t(*SpiWrite)(uint8_t, uint8_t, uint8_t*); // SPI write
	uint8_t(*SpiRead)(uint8_t, uint8_t, uint8_t*); // SPI read
	uint8_t(*DRDY)(void); // gets the state of DRDY

	// sensor's parameters
	uint16_t inputCapacitor; // input capacitor's value in nF
	uint16_t rRef; // reference resistor's value in ohms
	uint16_t rtdValue; // rtd's resistance in ohms at 0celsius
	uint8_t bias; // 1 -> on, 0 -> off
	uint8_t notchFrequency; // 1 -> 50Hz, 0-> 60Hz
	uint8_t operationMode; // 1 -> 3 wire, 0 -> 2/4 wire
	uint8_t conversionMode; // 1 -> automatic, 0 -> normally off
} MAX31865;

#endif /* INC_MAX31865_H_ */
