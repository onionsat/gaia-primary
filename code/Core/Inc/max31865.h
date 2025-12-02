/*
 * max31865.h
 *
 *  Created on: Nov 9, 2025
 *      Author: Robert Mihalyffy
 */

#ifndef INC_MAX31865_H_
#define INC_MAX31865_H_

#include <stdint.h>

// defining registers
#define configuration 0x0
#define rtdMSBs 0x1
#define rtdLSBs 0x2
#define HighFaultThresholdMSB 0x3
#define HighFaultThresholdLSB 0x4
#define LowFaultThresholdMSB 0x5
#define LowFaultThresholdLSB 0x6
#define faultStatus 0x7

// defining error codes for fault detection cycle(automatic or manual fault detection cycle)
#define rtdHighThresholdError 0b10000000
#define rtdLowThresholdError 0b01000000
#define refin1Error 0b00100000 // if refin- is bigger than 0.85 * Vbias
#define refin2Error 0b00010000 // if refin- is smaller than 0.85 * Vbias with force- open
#define rtdinError 0b00001000 // if rtdin- is smaller than 0.85 * Vbias with force- open
#define overOrUnderVoltageError 0b00000100


typedef struct
{
	// interface function pointers
	void(*delay)(uint32_t); // milliseconds delay
	void(*cs)(uint8_t); // sets the CS pin's state
	uint8_t(*SpiWrite)(uint8_t, uint8_t, uint8_t*); // SPI write
	uint8_t(*SpiRead)(uint8_t, uint8_t, uint8_t*); // SPI read
	uint8_t(*DRDY)(void); // gets the state of DRDY
	void(*errorCallback)(uint8_t); // errorcallback(user implemented)

	// external components' parameters
	uint16_t referenceResistor; // reference resistor's value in ohms
	uint16_t inputCapacitor; // input capacitor's value in nanofarads
	uint16_t rtdResistance; // rtd's resistance in ohms at 0celsius
	uint32_t timeconstant; // the time constant of the input RC in us

	// configuration parameters
	uint8_t bias; // 1 -> on, 0 -> off
	uint8_t notchFrequency; // 1 -> 50Hz, 0-> 60Hz
	uint8_t operationMode; // 1 -> 3 wire, 0 -> 2/4 wire
	uint8_t conversionMode; // 1 -> automatic, 0 -> normally off
	uint16_t lowFaultTemperature; // in 15 bit ADC format(like in the read function)
	uint16_t highFaultTemperature; // in 15 bit ADC format(like in the read function)
} MAX31865;


uint8_t MAX31865init(MAX31865* sensorInstance, void(*userDelay)(uint32_t), void(*userCs)(uint8_t), uint8_t(*userSpiWrite)(uint8_t, uint8_t, uint8_t*), uint8_t(*userSpiRead)(uint8_t, uint8_t, uint8_t*), uint8_t(*userDRDY)(void), void(*userErrorCallback)(uint8_t), uint16_t userRref, uint16_t userInputCapacitor, uint16_t userRtdValue, uint8_t userOperationMode, uint16_t highTempFault, uint16_t lowTempFault, uint8_t notchFreq, uint8_t userConversionMode);
uint8_t MAX31865readData(MAX31865* sensorInstance, uint16_t* data);


#endif /* INC_MAX31865_H_ */
