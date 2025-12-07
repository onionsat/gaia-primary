/*
 * max31865.h
 *
 *  Created on: Nov 9, 2025
 *      Author: Robert Mihalyffy
 */

#ifndef INC_MAX31865_H_
#define INC_MAX31865_H_

// defining registers
#define configuration 0x0U
#define rtdMSBs 0x1U
#define rtdLSBs 0x2U
#define HighFaultThresholdMSB 0x3U
#define HighFaultThresholdLSB 0x4U
#define LowFaultThresholdMSB 0x5U
#define LowFaultThresholdLSB 0x6U
#define faultStatus 0x7U

// defining error codes for fault detection cycle(automatic or manual fault detection cycle)
#define rtdHighThresholdError 0b10000000U // if the 15 bit temperature read is bigger then or equals to the high temperature fault
#define rtdLowThresholdError 0b01000000u // if the 15 bit temperature read is smaller then or equals to the low temperature fault
#define refin1Error 0b00100000U // if refin- is bigger than 0.85 * Vbias
#define refin2Error 0b00010000U // if refin- is smaller than 0.85 * Vbias with force- open
#define rtdinError 0b00001000U // if rtdin- is smaller than 0.85 * Vbias with force- open
#define overOrUnderVoltageError 0b00000100U


typedef struct
{
	// number of the instance
	uint8_t sensorId;

	// interface function pointers
	void(*delay)(uint32_t); // milliseconds delay
	void(*cs)(uint8_t); // sets the CS pin's state
	uint8_t(*SpiWrite)(uint8_t, uint8_t, uint8_t*); // SPI write
	uint8_t(*SpiRead)(uint8_t, uint8_t, uint8_t*); // SPI read
	uint8_t(*DRDY)(void); // gets the state of DRDY
	void(*errorCallback)(uint8_t, uint8_t); // errorcallback(user implemented)

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


// functions
float MAX31865rawToResistance(MAX31865* sensorInstance, uint16_t adcCode);
float MAX31865rawToTemperatureApprox(uint16_t adcCode);
float MAX31865resistanceToTemperature(MAX31865* sensorInstance, float rtdResistance);
uint8_t MAX31865switchBias(MAX31865* sensorInstance, uint8_t state);
uint8_t MAX31865setConversionMode(MAX31865* sensorInstance, uint8_t conversionMode);
uint8_t MAX31865setOperationMode(MAX31865* sensorInstance, uint8_t operationMode);
uint8_t MAX31865setNotchfrequency(MAX31865* sensorInstance, uint8_t notchFrequency);
uint8_t MAX31865_1shot(MAX31865* sensorInstance);
uint8_t MAX31865setTemperatureFault(MAX31865* sensorInstance, uint8_t highOrLow, uint16_t temperature);
uint8_t MAX31865readFault(MAX31865* sensorInstance);
uint8_t MAX31865readData(MAX31865* sensorInstance, uint16_t* data);
uint8_t MAX31865automaticFault(MAX31865* sensorInstance);
uint8_t MAX31865faultstatusClear(MAX31865* sensorInstance);
uint8_t MAX31865init(uint8_t sensorId, MAX31865* sensorInstance, void(*delay)(uint32_t), void(*cs)(uint8_t), uint8_t(*SpiWrite)(uint8_t, uint8_t, uint8_t*), uint8_t(*SpiRead)(uint8_t, uint8_t, uint8_t*), uint8_t(*DRDY)(void), void(*errorCallback)(uint8_t, uint8_t), uint16_t referenceResistor, uint16_t inputCapacitor, uint16_t rtdResistance, uint8_t notchFrequency, uint8_t operationMode, uint8_t conversionMode, uint16_t lowFaultTemperature, uint16_t highFaultTemperature);



#endif /* INC_MAX31865_H_ */
