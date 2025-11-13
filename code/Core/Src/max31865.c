/*
 * max31865.c
 *
 *  Created on: Nov 9, 2025
 *      Author: Robert Mihalyffy
 */
#include <math.h>


/**
  * @brief Converts the ADCcode into resistance
  * @param A pointer to an instance of MAX31865
  * @param Raw data
  * @retval The resistance
  */
float MAX31865rawToResistance(MAX31865* sensorInstance, uint16_t adcCode)
{
	return (adcCode * sensorinstance->rRef) / pow(2, 15);
}

/**
  * @brief Converts resistance into kelvins using the Callendar-Van Dusen equation
  * @param A pointer to an instance of MAX31865
  * @param Resitance of the RTD
  * @retval The temperature in kelvins
  */
float MAX31865resistanceToTemperature(MAX31865* sensorInstance, float rtdResistance)
{
	const float a = 3.90830 * pow(10, -3);
	const float b = -5.77500 * pow(10, -7);

	float temperature = (-a + sqrt(pow(a, 2) - 4*b*(1 - rtdResistance / sensorInstance->rRef))) / (2 * b);

	return temperature;
}


/**
  * @brief Switches bias
  * @param A pointer to an instance of MAX31865
  * @param Desired state, 1 -> on, 0 -> off
  * @retval 1 -> success, 0 -> failure
  */
uint8_t MAX31865switchBias(MAX31865* sensorInstance, uint8_t state)
{
	uint8_t currentConfiguratonRegister;

	// reading the current status of the register
	sensorInstance->cs(0);
	uint8_t retVal = sensorInstance->SpiRead(configuration, 1, &currentConfiguratonRegister);
	sensorInstance->cs(1);

	if(retVal != 1)
	{
		return 0;
	}

	uint8_t configurationRegisterDesired = configurationRegister | (status << 7);

	// writing the desired status to the register
	sensorInstance->cs(0);
	retVal = sensorInstance->SpiWrite((uint8_t)configuration | 0x80, 1, &configurationRegisterDesired);
	sensorInstance->cs(1);

	if(retVal != 1)
	{
		return 0;
	}

	return 1;
}


/**
  * @brief Sets conversion mode
  * @param A pointer to an instance of MAX31865
  * @param Desired mode, 1 -> automatic, 0 -> normally off
  * @retval 1 -> success, 0 -> failure
  */
uint8_t MAX31865setConverionMode(MAX31865* sensorInstance, uint8_t mode)
{
	uint8_t currentConfiguratonRegister;

	// reading the current status of the register
	sensorInstance->cs(0);
	uint8_t retVal = sensorInstance->SpiRead(configuration, 1, &currentConfiguratonRegister);
	sensorInstance->cs(1);

	if(retVal != 1)
	{
		return 0;
	}

	uint8_t configurationRegisterDesired = configurationRegister | (mode << 6);

	// writing the desired status to the register
	sensorInstance->cs(0);
	retVal = sensorInstance->SpiWrite((uint8_t)configuration | 0x80, 1, &configurationRegisterDesired);
	sensorInstance->cs(1);

	if(retVal != 1)
	{
		return 0;
	}

	return 1;
}


/**
  * @brief Sets operation mode
  * @param A pointer to an instance of MAX31865
  * @param Desired mode, 1 -> 3 wire, 0 -> 2/4 wire
  * @retval 1 -> success, 0 -> failure
  */
uint8_t MAX31865setOperationMode(MAX31865* sensorInstance, uint8_t operationmode)
{
	uint8_t currentConfiguratonRegister;

	// reading the current status of the register
	sensorInstance->cs(0);
	uint8_t retVal = sensorInstance->SpiRead(configuration, 1, &currentConfiguratonRegister);
	sensorInstance->cs(1);

	if(retVal != 1)
	{
		return 0;
	}

	uint8_t configurationRegisterDesired = configurationRegister | (operationmode << 4);

	// writing the desired status to the register
	sensorInstance->cs(0);
	retVal = sensorInstance->SpiWrite((uint8_t)configuration | 0x80, 1, &configurationRegisterDesired);
	sensorInstance->cs(1);

	if(retVal != 1)
	{
		return 0;
	}

	return 1;
}


/**
  * @brief Sets notch frequency
  * @param A pointer to an instance of MAX31865
  * @param 1 -> 50Hz, 0 -> 60Hz
  * @retval 1 -> success, 0 -> failure
  */
uint8_t MAX31865setNotchfrequency(MAX31865* sensorInstance, uint8_t value)
{
	uint8_t currentConfiguratonRegister;

	// reading the current status of the register
	sensorInstance->cs(0);
	uint8_t retVal = sensorInstance->SpiRead(configuration, 1, &currentConfiguratonRegister);
	sensorInstance->cs(1);

	if(retVal != 1)
	{
		return 0;
	}

	uint8_t configurationRegisterDesired = configurationRegister | (value << 0);

	// writing the desired status to the register
	sensorInstance->cs(0);
	retVal = sensorInstance->SpiWrite((uint8_t)configuration | 0x80, 1, &configurationRegisterDesired);
	sensorInstance->cs(1);

	if(retVal != 1)
	{
		return 0;
	}

	return 1;
}
