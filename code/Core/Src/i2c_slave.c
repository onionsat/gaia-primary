/*
 * i2c_slave.c
 *
 *  Created on: Oct 22, 2024
 *      Author: tamas
 */

#include "main.h"
#include "i2c_slave.h"
#include <stdio.h>
#include "stdlib.h"
#include <string.h>
#include "stm32f1xx_hal.h"

#define I2CslaveWritePacketsize 8
#define I2CslaveGenerallcallPacketsize 8
#define I2CslaveReadPacketsize 8

#define UARTdebug 1 // 1 -> enabled, 0 -> disabled

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern UART_HandleTypeDef huart1;

// stores the number of different I2C errors
struct I2Cerrors
{
	uint32_t generallcallError;
	uint32_t receiveError;
	uint32_t transmitError;
	uint32_t countError;
};

struct I2Cdatas
{
	uint8_t rxData[8];
	uint8_t txData[8];
	uint32_t rxCount;
	uint32_t txCount;
};

struct I2Cerrors I2CerrorsI2C1 = {0,0,0,0};
struct I2Cerrors I2CerrorsI2C2 = {0,0,0,0};
struct I2Cdatas I2CdatasI2C1 = {.rxCount = 0, .txCount = 0, .txData = {0x01}};
char UARTbuffer[50];

// I2C listen callback - Here we reactivate i2c, for being in listening mode
void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
    HAL_I2C_EnableListen_IT(hi2c);
}

void processData(void)
{
	// processing write data
	switch (I2CdatasI2C1.rxData[0])
	{

	}
}

// I2C address callback
void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
	if (TransferDirection == I2C_DIRECTION_TRANSMIT) // If the direction is write
	{
    	if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_GENCALL) == 0) // Normal write
		{
    		if(HAL_I2C_Slave_Sequential_Receive_IT(hi2c, I2CdatasI2C1.rxData, I2CslaveWritePacketsize, I2C_FIRST_AND_LAST_FRAME) != HAL_OK)
    		{
    			I2CerrorsI2C1.receiveError++;
    		}
		}
		else // Generalcall
		{
			if(HAL_I2C_Slave_Sequential_Receive_IT(hi2c, I2CdatasI2C1.rxData, I2CslaveGenerallcallPacketsize, I2C_FIRST_AND_LAST_FRAME) != HAL_OK)
			{
				I2CerrorsI2C1.generallcallError++;
			}
		}
	}
    else // If the direction is read
    {
    	if(HAL_I2C_Slave_Seq_Transmit_IT(hi2c, I2CdatasI2C1.txData, I2CslaveReadPacketsize, I2C_FIRST_AND_LAST_FRAME) != HAL_OK)
    	{
    		I2CerrorsI2C1.transmitError++;
    	}
    }
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	I2CdatasI2C1.txCount++;
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	I2CdatasI2C1.rxCount++;
	processData();
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
	I2CerrorsI2C1.countError++;
	uint32_t errorcode = HAL_I2C_GetError(hi2c);

	// on which i2c instance is the error
	uint8_t errorI2C;

	if(hi2c -> Instance == I2C1)
	{
		errorI2C = 1;
	}
	else if(hi2c -> Instance == I2C2)
	{
		errorI2C = 2;
	}

	if(errorcode != HAL_I2C_ERROR_NONE)
	{
		// Sending errorcode on UART if UARTdebug is enabled(1 -> enabled, 0 -> disabled)
		if(UARTdebug == 1)
		{
			uint32_t snprintfWritten = snprintf(UARTbuffer, sizeof(UARTbuffer), "I2C error(I2C%u): %lu\nErrorcount: %lu\n", errorI2C, errorcode, I2CerrorsI2C1.countError);
			HAL_UART_Transmit(&huart1, (uint8_t *)UARTbuffer, snprintfWritten+1, 1000);
		}

		if(errorcode & HAL_I2C_ERROR_BERR) // Bus error
		{
			__HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_BERR);

			//HAL_I2C_DeInit(hi2c);
			__HAL_RCC_I2C1_FORCE_RESET();
			__HAL_RCC_I2C1_RELEASE_RESET();

			if(HAL_I2C_Init(hi2c) != HAL_OK)
			{
				NVIC_SystemReset(); // Reset
			}
		}

		if(errorcode & HAL_I2C_ERROR_AF) // Acknowledge Failure
		{

		}

		if (errorcode & HAL_I2C_ERROR_ARLO) // Arbitration lost
		{
		    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_ARLO);
		}

		if(errorcode & HAL_I2C_ERROR_OVR) // Overrun error
		{
			__HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_OVR);

			//HAL_I2C_DeInit(hi2c);
			__HAL_RCC_I2C1_FORCE_RESET();
			__HAL_RCC_I2C1_RELEASE_RESET();

			if(HAL_I2C_Init(hi2c) != HAL_OK)
			{
				NVIC_SystemReset(); // Reset
			}
		}

		if(errorcode & HAL_I2C_ERROR_TIMEOUT) // Timeout Error
		{
			//HAL_I2C_DeInit(hi2c);
            __HAL_RCC_I2C1_FORCE_RESET();
            __HAL_RCC_I2C1_RELEASE_RESET();

			if(HAL_I2C_Init(hi2c) != HAL_OK)
			{
				NVIC_SystemReset(); // Reset
			}
		}
	}

    HAL_I2C_EnableListen_IT(hi2c);
}

