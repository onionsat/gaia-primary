/*
 * max31865.c
 *
 *  Created on: Nov 9, 2025
 *      Author: Robert Mihalyffy
 */
#include <math.h>
#include "max31865.h"
#include <stdio.h>


/**
  * @brief Converts the ADCcode into resistance
  * @param A pointer to an instance of MAX31865
  * @param 15bit ADC code
  * @retval The resistance
  */
float MAX31865rawToResistance(MAX31865* sensorInstance, uint16_t adcCode)
{
	return (adcCode * sensorInstance->rRef) / pow(2, 15);
}

/**
  * @brief Converts resistance into kelvins using the Callendar-Van Dusen equation
  * @param A pointer to an instance of MAX31865
  * @param Resistance of the RTD
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
  * @param 1 -> on, 0 -> off
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

	uint8_t configurationRegisterDesired = currentConfiguratonRegister;

	if(state == 1)
	{
		configurationRegisterDesired |= (1 << 7);
	}
	else
	{
		configurationRegisterDesired &= ~(1 << 7);
	}

	// writing the desired status to the register
	sensorInstance->cs(0);
	retVal = sensorInstance->SpiWrite((uint8_t)configuration | 0x80, 1, &configurationRegisterDesired);
	sensorInstance->cs(1);

	if(retVal != 1)
	{
		return 0;
	}

	// if setting bias to the desired value was successful setting it in the sensor instance
	sensorInstance->bias = state;

	return 1;
}


/**
  * @brief Sets conversion mode
  * @param A pointer to an instance of MAX31865
  * @param Desired mode, 1 -> automatic, 0 -> normally off
  * @retval 1 -> success, 0 -> failure
  */
uint8_t MAX31865setConversionMode(MAX31865* sensorInstance, uint8_t conversionmode)
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

	uint8_t configurationRegisterDesired = currentConfiguratonRegister;

	if(conversionmode == 1)
	{
		configurationRegisterDesired |= (1 << 6);
	}
	else
	{
		configurationRegisterDesired &= ~(1 << 6);
	}

	// writing the desired status to the register
	sensorInstance->cs(0);
	retVal = sensorInstance->SpiWrite((uint8_t)configuration | 0x80, 1, &configurationRegisterDesired);
	sensorInstance->cs(1);

	if(retVal != 1)
	{
		return 0;
	}

	// if setting conversion mode to the desired value was successful setting it in the sensor instance
	sensorInstance->conversionMode = conversionmode;

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

	uint8_t configurationRegisterDesired = currentConfiguratonRegister;

	if(operationmode == 1)
	{
		configurationRegisterDesired |= (1 << 4);
	}
	else
	{
		configurationRegisterDesired &= ~(1 << 4);
	}

	// writing the desired status to the register
	sensorInstance->cs(0);
	retVal = sensorInstance->SpiWrite((uint8_t)configuration | 0x80, 1, &configurationRegisterDesired);
	sensorInstance->cs(1);

	if(retVal != 1)
	{
		return 0;
	}

	// if setting the operation mode to the desired value was successful setting it in the sensor instance
	sensorInstance->operationMode = operationmode;

	return 1;
}


/**
  * @brief Sets notch frequency
  * @param A pointer to an instance of MAX31865
  * @param 1 -> 50Hz, 0 -> 60Hz
  * @retval 1 -> success, 0 -> failure
  */
uint8_t MAX31865setNotchfrequency(MAX31865* sensorInstance, uint8_t notchfrequency)
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

	uint8_t configurationRegisterDesired = currentConfiguratonRegister;

	if(notchfrequency == 1)
	{
		configurationRegisterDesired |= (1 << 0);
	}
	else
	{
		configurationRegisterDesired &= ~(1 << 0);
	}

	// writing the desired status to the register
	sensorInstance->cs(0);
	retVal = sensorInstance->SpiWrite((uint8_t)configuration | 0x80, 1, &configurationRegisterDesired);
	sensorInstance->cs(1);

	if(retVal != 1)
	{
		return 0;
	}

	// if setting bias to the desired value was successful setting it in the sensor instance
	sensorInstance->notchFrequency = notchfrequency;

	return 1;
}


/**
  * @brief Does a one shot conversion, if bias is off it will turn it on, it also implements a delay for the conversion to be ready
  * @param A pointer to an instance of MAX31865
  * @retval 1 -> success, 0 -> failure, 2 -> unable to switch bias on
  */
uint8_t MAX31865_1shot(MAX31865* sensorInstance)
{
	// if the bias isn't on, switching it on
	if(sensorInstance->bias == 0)
	{
		// trying to switch bias on
		if(MAX31865switchBias(sensorInstance, 1) != 1)
		{
			return 2; // error switching bias on
		}

		/*
		 * Wait to ensure a precise conversion after bias power up
		 * Calculated for 100nF input capacitor + 4k reference resistor (tau * 10.5 = 4200us, delay = 1ms + tau * 10.5 = 5200us)
		 */
		sensorInstance->delay(5200);

		sensorInstance->bias = 1;
	}

	uint8_t currentConfiguratonRegister;

	// reading the current status of the register
	sensorInstance->cs(0);
	uint8_t retVal = sensorInstance->SpiRead(configuration, 1, &currentConfiguratonRegister);
	sensorInstance->cs(1);

	if(retVal != 1)
	{
		return 0;
	}

	uint8_t configurationRegisterDesired = currentConfiguratonRegister | (1 << 5);

	// writing the desired status to the register
	sensorInstance->cs(0);
	retVal = sensorInstance->SpiWrite((uint8_t)configuration | 0x80, 1, &configurationRegisterDesired);
	sensorInstance->cs(1);

	if(retVal != 1)
	{
		return 0;
	}

	// waiting for the conversion to be ready
	if(sensorInstance->notchFrequency == 1) // 50Hz
	{
		sensorInstance->delay(62500);
	}
	else
	{
		sensorInstance->delay(52000);
	}

	return 1;
}


/**
  * @brief Sets one of the temperature fault registers
  * @param A pointer to an instance of MAX31865
  * @param 1 -> High fault, 0 -> low fault
  * @param The desired value, 15 bit ADC value(same as in the read function)
  * @retval 1 -> success, 0 -> failure
  */
uint8_t MAX31865setTemperatureFault(MAX31865* sensorInstance, uint8_t highOrLow, uint16_t temperature)
{
	uint8_t dataToSend[2];

	uint16_t temperatureRegisterCompatible = temperature << 1; // to make it compatible with the register

	dataToSend[0] = (uint8_t)(temperatureRegisterCompatible >> 8);
	dataToSend[1] = (uint8_t)(temperatureRegisterCompatible);

	if(highOrLow == 1) // high fault
	{
		sensorInstance->cs(0);
		uint8_t retVal = sensorInstance->SpiWrite((uint8_t)HighFaultThresholdMSB | 0x80, 2, dataToSend);
		sensorInstance->cs(1);

		if(retVal != 1)
		{
			return 0;
		}
		else
		{
			sensorInstance->highFaultTemperature = temperature; // if setting it in the register was successful, set it in the sensor instance
		}
	}
	else
	{
		sensorInstance->cs(0);
		uint8_t retVal = sensorInstance->SpiWrite((uint8_t)LowFaultThresholdMSB | 0x80, 2, dataToSend);
		sensorInstance->cs(1);

		if(retVal != 1)
		{
			return 0;
		}
		else
		{
			sensorInstance->lowFaultTemperature = temperature; // if setting it in the register was successful, set it in the sensor instance
		}
	}

	return 1;
}

/**
  * @brief Reads the fault status register, and calls the error callback implemented by the user with the
  * @param A pointer to an instance of MAX31865
  * @retval 1 -> success, 0 -> failure(only if it couldn't read fault status register)
  */
uint8_t MAX31865readFault(MAX31865* sensorInstance)
{
	uint8_t faultStatusRegister;

	if(sensorInstance->SpiRead(faultStatus, 1, &faultStatusRegister) != 1)
	{
		return 0;
	}

	sensorInstance->errorCallback(faultStatusRegister);

	return 1;
}


/**
  * @brief Reads the raw 15bit ADC value from the data registers, if the fault bit is 1 it will call the error handler
  * @param A pointer to an instance of MAX31865
  * @param A pointer to a variable where the data read should be stored(raw 15 bit ADC value)
  * @retval 1 -> success, 0 -> failure while reading the data registers, 2 -> error bit was set and fault status register could be read(user implemented error handler was called), 3 -> error bit was set and fault status register couldn't be read(user implemented error handler wasn't called)
  */
uint8_t MAX31865readData(MAX31865* sensorInstance, uint16_t* data)
{
	uint8_t readBuffer[2];

	sensorInstance->cs(0);
	uint8_t retVal = sensorInstance->SpiRead(rtdMSBs, 2, readBuffer);
	sensorInstance->cs(1);

	if(retVal != 1)
	{
		return 0;
	}

	// checking error bit
	if(readBuffer[1] & 0x01)
	{
		// if error bit set, calling read fault status function
		if(MAX31865readFault(sensorInstance) != 1)
		{
			return 3; // if unable to read fault status register
		}

		return 2;
	}

	*data = ( ( (uint16_t)readBuffer[0] << 8 ) | (uint16_t)readBuffer[1] ) >> 1;

	return 1;
}


/**
  * @brief Runs automatic or manual fault detection depending on the time constant of the input filter. At the beginning of the function saves the current state of the configuration register and after the cycle completes it writes it back
  * @param A pointer to an instance of MAX31865
  * @retval 1 -> success, 0 -> couldn't read the initial state of the configuration register, 2 -> couldn't start the automatic fault detection, 3 -> couldn't write back initial configuration register state
  */
uint8_t MAX31865Faultdetection(MAX31865* sensorInstance)
{
	// reading the initial state of the configuration register
	uint8_t configurationRegisterInitial;

	if(sensorInstance->SpiRead(configuration, 1, &configurationRegisterInitial) != 1)
	{
		return 0;
	}

	if(sensorInstance->timeConstant > 100)
	{
		// manual fault detection
		if(sensorInstance->bias == 0) // if vbias is off, switching it on and wait for 5 time constant
		{
			MAX31865switchBias(1);
		}
	}
	else
	{
		// starting the automatic fault detection cycle value for the configuration register, 100X010X
		uint8_t conRegStartAutomaticFaultDet = 0b10000100;

		// setting the two X to the values in the sensorInstance
		if(sensorInstance->notchFrequency == 1)
		{
			conRegStartAutomaticFaultDet |= (1 << 0);
		}

		if(sensorInsatnce->operationMode == 1)
		{
			conRegStartAutomaticFaultDet |= (1 << 4);
		}

		sensorInstance->cs(0);
		uint8_t retVal = sensorInstance->SpiWrite(configuration, 1, &conRegStartAutomaticFaultDet);
		sensorInstance->cs(1);

		if(retVal != 1)
		{
			return 2;
		}

		// waiting for the required 600us(plus extra 50us)
		sensorInstance->delay(650);

		// writing back the initial state of the configuration register
		if(sensorInstance->SpiWrite(configuration, 1, &configurationRegisterBeginning) != 1)
		{
			return 3;
		}
	}

	return 1;
}


/**
  * @brief Initialzies an instance of MAX31865
  * @param sensorInstance -> Pointer to a MAX31865 instance which should be initialised
  * @param delay -> Function pointer to user provided delay function
  * @param cs -> Function pointer to user provided chipselect function
  * @param SpiWrite -> Function pointer to user provided spi write function
  * @param SpiRead -> Function pointer to user provided spi read function
  * @param DRDY -> Function pointer to user provided DRDY function
  * @param errorCallback -> Function pointer to user provided error callback function
  * @param referenceResistor -> The reference resistor's value in ohms
  * @param inputCapacitor -> The value of the capacitor used in the input RC filter in ohms
  * @param rtdResistance -> The resistance of the RTD in ohms at 0celsius
  * @param notchFrequency -> The desired notch frequency, 1 -> 50Hz, 0 -> 60Hz
  * @param operationMode -> The desired operation mode, 1 -> 3 wire, 0 -> 2/4 wire
  * @param conversionMode -> The desired conversion mode, 1 -> automatic, 0 -> normally off
  * @param lowFaultTemperature -> The desired low temperature fault, 15bit ADC format
  * @param highFaultTemperature -> The desired high temperature fault, 15bit ADC format
  * @retval For each error case there is a bit in the 8bit return value, the bit is set if the error occurred
  * 		bit D0 -> error setting operation mode
  * 		bit D1 -> error setting low temperature fault
  * 		bit D2 -> error setting hogh temperature fault
  * 		bit D3 -> error setting notch frequency
  * 		bit D4 -> error setting conversion mode
  * 		bit D5 -> error setting bias
  */
uint8_t MAX31865init(MAX31865* sensorInstance, void(*delay)(uint32_t), void(*cs)(uint8_t), uint8_t(*SpiWrite)(uint8_t, uint8_t, uint8_t*), uint8_t(*SpiRead)(uint8_t, uint8_t, uint8_t*), uint8_t(*DRDY)(void), void(*errorCallback)(uint8_t), uint16_t referenceResistor, uint16_t inputCapacitor, uint16_t rtdResistance, uint8_t notchFrequency, uint8_t operationMode, uint8_t conversionMode, uint16_t lowFaultTemperature, uint16_t highFaultTemperature)
{
	uint8_t returnValue = 0;

	// passing in the function pointers of the interface functions
	sensorInstance->delay = delay;
	sensorInstance->cs = cs;
	sensorInstance->SpiWrite = SpiWrite;
	sensorInstance->SpiRead = SpiRead;
	sensorInstance->DRDY = DRDY;
	sensorInstance->errorCallback = errorCallback;


	// setting the external components' parameters
	sensorInstance->referenceResistor = referenceResistor;
	sensorInstance->inputCapacitor = inputCapacitor;
	sensorInstance->rtdResistance = rtdResistance;

	if(sensorInstance->referenceResistor * sensorInstance->inputCapacitor % 1000 == 0)
	{
		sensorInstance->timeconstant = (sensorInstance->referenceResistor * sensorInstance->inputCapacitor) / 1000;
	}
	else
	{
		sensorInstance->timeconstant = ((sensorInstance->referenceResistor * sensorInstance->inputCapacitor) / 1000) + 1;
	}


	// setting operation mode, 3wire or 2/4 wire
	if(MAX31865setOperationMode(sensorInstance, userOperationMode) != 1)
	{
		returnValue |= (1 << 0);
	}

	// setting high temperature fault and low temperature fault
	if(MAX31865setTemperatureFault(sensorInstance, 0, lowTempFault) != 1) // low fault
	{
		returnValue |= (1 << 1);
	}

	if(MAX31865setTemperatureFault(sensorInstance, 1, highTempFault) != 1) // high fault
	{
		returnValue |= (1 << 2);
	}

	// setting notch frequency
	if(MAX31865setNotchfrequency(sensorInstance, notchFreq) != 1)
	{
		returnValue |= (1 << 3);
	}

	// setting conversion mode, automatic or normally off
	if(MAX31865setConversionMode(sensorInstance, userConversionMode) != 1)
	{
		returnValue |= (1 << 4);
	}

	// turning on bias
	if(MAX31865switchBias(sensorInstance, 1) != 1)
	{
		returnValue |= (1 << 5);
	}

	return returnValue;
}


