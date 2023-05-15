/**
* drv_audio_hra.c
*/

/**
* @author			
* @modified by		
* @note			
* @brief			HRA(High Resoultion Audio) or HD Audio Module Driver
* @                          - Sampling Rate Converter
* @                          - High-end DAC
* @file 			drv_audio_hra.c
*/




#include <stdlib.h>

#include "linden_trace.h"
#include "di_uart.h"
#include "drv_i2c.h"

//module config.
//not needed, just memo
//when became needed, these should go to def config. 
#define HRA_DAC_WM8742
#define HRA_SRC_CS8422
#define HRA_I2C_MODE


//device I2C Addr
#if defined(HRA_I2C_MODE)

	#define I2C_CH_HRA 4


	#if defined (HRA_DAC_WM8742)
	#define ADDR_DAC (0x34 >> 1)
	#endif

	#if defined (HRA_SRC_CS8422)
	//Sampling rate converter
	#define ADDR_SRC (0x20 >> 1)
	#endif
	
#endif

#if defined (HRA_SRC_CS8422)
static HUINT8 InitSrcTable[][2] =
{
	{0x08, 0x20},
	{0x09, 0x00},
	{0x0A, 0x88},
	{0x0B, 0x08},
	{0x0C, 0xC4},
	{0x0D, 0xC4},
	{0x02, 0x48},
};
#endif

#if defined (HRA_DAC_WM8742)
static HUINT8 InitDacTable[][2] =
{
	{0x04, 0x61},
	{0x05, 0x0A},
	{0x06, 0x03},
	{0x07, 0x44},
	{0x08, 0x02}
};
#endif




DRV_Error P_AUDIO_HRA_DAC_Write(HUINT8 ucAddr, HUINT8* pucData, HUINT8 ucSize)
{
	DRV_Error dResult = DRV_OK;
	
	dResult = DRV_I2c_WriteA8(I2C_CH_HRA, ADDR_DAC, ucAddr, pucData, ucSize);	

	return dResult;
}

DRV_Error P_AUDIO_HRA_DAC_Read(HUINT8 ucAddr, HUINT8* pucData, HUINT8 ucSize)
{
	DRV_Error dResult = DRV_OK;
	
	dResult = DRV_I2c_ReadA8(I2C_CH_HRA, ADDR_DAC, ucAddr, pucData, ucSize);	

	return dResult;
}

DRV_Error P_AUDIO_HRA_SRC_Write(HUINT8 ucAddr, HUINT8* pucData, HUINT8 ucSize)
{
	DRV_Error dResult = DRV_OK;
	
	dResult = DRV_I2c_WriteA8(I2C_CH_HRA, ADDR_SRC, ucAddr, pucData, ucSize);	

	return dResult;
}

DRV_Error P_AUDIO_HRA_SRC_Read(HUINT8 ucAddr, HUINT8* pucData, HUINT8 ucSize)
{
	DRV_Error dResult = DRV_OK;
	
	dResult = DRV_I2c_ReadA8(I2C_CH_HRA, ADDR_SRC, ucAddr, pucData, ucSize);	

	return dResult;
}

DRV_Error DRV_AUDIO_HRA_DAC_Init(void)
{
	DRV_Error dResult = DRV_OK;
	unsigned i = 0;
	
	for(i=0; i<sizeof(InitDacTable)/2; i++)
	{
		dResult = P_AUDIO_HRA_DAC_Write(InitDacTable[i][0], &InitDacTable[i][1], 1);
		
		if(dResult != DRV_OK) return dResult;
	}

	return dResult;
}

DRV_Error DRV_AUDIO_HRA_SRC_Init(void)
{
	DRV_Error dResult = DRV_OK;
	unsigned i = 0;
	
	for(i=0; i<sizeof(InitSrcTable)/2; i++)
	{
		dResult = P_AUDIO_HRA_SRC_Write(InitSrcTable[i][0], &InitSrcTable[i][1], 1);
		if(dResult != DRV_OK) return dResult;
	}

	return dResult;
}

DRV_Error DRV_AUDIO_HRA_Init(void)
{
	DRV_Error dResult;

	dResult = DRV_AUDIO_HRA_DAC_Init();
	dResult |= DRV_AUDIO_HRA_SRC_Init();

	return dResult;
}


void DRV_AUDIO_HRA_Print(void)
{
	HUINT8 ucRead[16];
	int i=0;

	
	if(DRV_OK == P_AUDIO_HRA_SRC_Read(0x81, &ucRead[0], 16))
	{
		DI_UART_Print("Sampling Rate Converter READ \nAddr : ");
		
		for(i=1; i<17; i++)
		DI_UART_Print("%02x ", i);

		DI_UART_Print("\nData : ");
		for(i=0; i<16; i++)
		DI_UART_Print("%02x ", ucRead[i]);

		DI_UART_Print("\n");
	}
	else DI_UART_Print("\nReading Sampling Rate Converter Error\n");

#if 0
	for(i=0; i<10; i++)
	{
		if(DRV_OK != P_AUDIO_HRA_DAC_Read(i, &ucRead[i], 1))
		{
			DI_UART_Print("\nReading DAC Register(%02d)Error\n", i);
		}
	}

	if(DRV_OK != P_AUDIO_HRA_DAC_Read(32, &ucRead[11], 1))
	{
			DI_UART_Print("\nReading DAC Register(32)Error\n");
	}	

	DI_UART_Print("\nDAC READ \nAddr : ");
	for(i=0; i<10; i++)
	DI_UART_Print("%02d ", i);
	DI_UART_Print("32\n ");


	DI_UART_Print("Data : ");
	for(i=0; i<11; i++)
	DI_UART_Print("%02x ", ucRead[i]);

	DI_UART_Print("\n");
#endif	
	return;
}





