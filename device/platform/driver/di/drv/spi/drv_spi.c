/*******************************************************************************
* File name : drv_spi.c
* Author : D.K Lee
* description :
*
* Copyright (c)  by Humax Co., Ltd. Since 2007.
* All right reserved
*******************************************************************************/

/*******************************************************************************
* Header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */
#include "nexus_base_types.h"
#include "nexus_spi.h"

/* humax header files */
#include "linden_trace.h"
#include "drv_err.h"
#include "drv_gpio.h"
#include "drv_spi.h"

/*******************************************************************************
* Debug level
*******************************************************************************/
#define DEBUG_MODULE	TRACE_MODULE_DRV_MISC


/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define SPI_MAX_BAUD						1687500    /* 1.6Mbps, SPBR = 8. 27MHZ */
#define SPI_MIN_BAUD						52734	/* 52Kbps, SPBR = 0. 27MHZ */
#define SPI_NUMBER_OF_HW_QUEUE_ENTRY		16
#define SPI_MAX_TRANSFER_SIZE				65539


/*******************************************************************************
* Global variables and structures
*******************************************************************************/


/*******************************************************************************
* External variables and functions
*******************************************************************************/


/*******************************************************************************
* Static variables and structures
*******************************************************************************/
static unsigned char s_aucDummyCmd[SPI_NUMBER_OF_HW_QUEUE_ENTRY];
static NEXUS_SpiHandle	s_ptNexusSpiHandle = NULL;


/*******************************************************************************
* Static function prototypes
*******************************************************************************/
static void INTRN_DRV_SPI_ChangePinmux(unsigned char ucUseGpio);


/*******************************************************************************
* function : DRV_SPI_Init
* description : SPI Initialize
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_SPI_Init(void)
{
	NEXUS_SpiSettings	tSpiSetting;
	int	i = 0;
	
	NEXUS_Spi_GetDefaultSettings(&tSpiSetting);
	tSpiSetting.baud = SPI_MAX_BAUD;
	tSpiSetting.bitsPerTransfer = 8;
	tSpiSetting.useUserDtlAndDsclk = FALSE;
	tSpiSetting.clockActiveLow = FALSE;			
	tSpiSetting.txLeadingCapFalling = FALSE;	

	/* change interrupt  */
	tSpiSetting.interruptMode = TRUE;
	
	s_ptNexusSpiHandle = NEXUS_Spi_Open(0, &tSpiSetting);
	if(s_ptNexusSpiHandle == NULL)
	{
		PrintError("[%s] s_ptNexusSpiHandle is NULL \n", __func__);
		return DRV_ERR;
	}

	for(i=0; i<SPI_NUMBER_OF_HW_QUEUE_ENTRY; i++)
	{
		s_aucDummyCmd[i] = 0xFF;
	}

#if defined(CONFIG_MULTI_SMARTCARD)
	INTRN_DRV_SPI_ChangePinmux(0);
#endif

	return DRV_OK;
}

/*******************************************************************************
* function : DRV_SPI_VccControl
* description : auto <-> static(by gpio) vcc control of 8024
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
void DRV_SPI_SetVccCtrlMode(unsigned char ucUseGpio)
{
	INTRN_DRV_SPI_ChangePinmux(ucUseGpio);
}

/*******************************************************************************
* function : DRV_SPI_SetPower
* description : Drive Power of TDA8024
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_SPI_SetPower(unsigned char ucOn)
{
	if(ucOn)
	{
		DRV_GPIO_Write(GPIO_ID_SC1_RESET, GPIO_HI);
		DRV_GPIO_Write(GPIO_ID_SC1_VCC, GPIO_LOW);
	}
	else
	{
		DRV_GPIO_Write(GPIO_ID_SC1_RESET, GPIO_LOW);
		DRV_GPIO_Write(GPIO_ID_SC1_VCC, GPIO_HI);
	}

	return DRV_OK;
}

/*******************************************************************************
* function : DRV_SPI_GetPower
* description : Get current Power state of TDA8024
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
void DRV_SPI_GetPower(unsigned char *pucOn)
{
	GPIO_STATE_t	eGpioState = GPIO_LOW;

	DRV_GPIO_Read(GPIO_ID_SC1_VCC, &eGpioState);
	*pucOn = eGpioState;
}


/*******************************************************************************
* function : DRV_SPI_SelectDevice
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
void DRV_SPI_SelectDevice(unsigned char ucSelect)
{
	if(ucSelect)
	{
		DRV_GPIO_Write(GPIO_ID_SC1_DATA, GPIO_LOW);
	}
	else
	{
		DRV_GPIO_Write(GPIO_ID_SC1_DATA, GPIO_HI);
	}
}

/*******************************************************************************
* function : DRV_SPI_SetBaudRate
* description : 
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_SPI_SetBaudRate(DRV_SPI_DEVICE_e eDevice, unsigned int unBaudRateInHz)
{
	NEXUS_Error	eNexusError = NEXUS_SUCCESS;
	NEXUS_SpiSettings	tSpiSetting;

	BSTD_UNUSED(eDevice);

	/* Reset System Clock : 27Mhz */
	if (unBaudRateInHz > SPI_MAX_BAUD) 
	{
		unBaudRateInHz = SPI_MAX_BAUD;
	}
	else if (unBaudRateInHz < SPI_MIN_BAUD)
	{
		unBaudRateInHz = SPI_MIN_BAUD;
	}

	NEXUS_Spi_GetSettings(s_ptNexusSpiHandle, &tSpiSetting);
	tSpiSetting.baud = unBaudRateInHz;
	eNexusError = NEXUS_Spi_SetSettings(s_ptNexusSpiHandle, &tSpiSetting);
	if(eNexusError != NEXUS_SUCCESS)
	{
		PrintError("[%s] eNexusError is 0x%x \n", __func__, eNexusError);
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_SPI_SetSpiMode(HBOOL bCPHA, HBOOL bCPOL)
{
	NEXUS_Error	eNexusError = NEXUS_SUCCESS;
	NEXUS_SpiSettings tSpiSetting;

	NEXUS_Spi_GetSettings(s_ptNexusSpiHandle, &tSpiSetting);
	tSpiSetting.clockActiveLow = bCPHA;			
	tSpiSetting.txLeadingCapFalling = bCPOL;	
	eNexusError = NEXUS_Spi_SetSettings(s_ptNexusSpiHandle, &tSpiSetting);
	if(eNexusError != NEXUS_SUCCESS)
	{
		PrintError("[%s] eNexusError is 0x%x \n", __func__, eNexusError);
		return DRV_ERR;
	}
	return DRV_OK;
}
/*******************************************************************************
* function : DRV_SPI_TransmitPacket
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_SPI_TransmitPacket(DRV_SPI_DEVICE_e eDevice, unsigned char mode, unsigned char *pucTxData, int nTxDataSize)
{
	DRV_Error eRet = DRV_OK;
	NEXUS_Error	eNexusError = NEXUS_SUCCESS;
	int	nWriteLength = 0;
	int	nCurWriteSize = 0;
	int nCurPosition = 0;
	unsigned char bitPerTransfer = 0;

	NEXUS_SpiSettings	tSpiSetting;
	
	
	BSTD_UNUSED(eDevice);

	if ((pucTxData == NULL) || (nTxDataSize <= 0) ||(nTxDataSize > SPI_MAX_TRANSFER_SIZE))
	{
		PrintError("[DRV_SPI_TransmitPacket] ERROR : Invalid Paramter!!!\n");
		return DRV_ERR_INVALID_PARAMETER;
	}

	nWriteLength = nTxDataSize;
	nCurPosition = 0;

	if(mode == 0)
	{
		NEXUS_Spi_GetSettings(s_ptNexusSpiHandle, &tSpiSetting);
		bitPerTransfer = tSpiSetting.bitsPerTransfer;
	}
	else
	{
		bitPerTransfer = mode;
	}
	
	do
	{
		if(bitPerTransfer == 8)
		{
			if(nWriteLength > SPI_NUMBER_OF_HW_QUEUE_ENTRY)
			{
				nCurWriteSize = SPI_NUMBER_OF_HW_QUEUE_ENTRY;
			}
			else
			{
				nCurWriteSize = nWriteLength;
			}
		}
		else
		{

			if(nWriteLength > SPI_NUMBER_OF_HW_QUEUE_ENTRY*2 )
			{
				nCurWriteSize = SPI_NUMBER_OF_HW_QUEUE_ENTRY*2;
			}
			else
			{
				nCurWriteSize = nWriteLength;
			}
		
		}

		if(mode == 0)
			eNexusError = NEXUS_Spi_Write(s_ptNexusSpiHandle, &pucTxData[nCurPosition], nCurWriteSize);
		else
			eNexusError = NEXUS_Spi_WritebyMode(s_ptNexusSpiHandle,mode,&pucTxData[nCurPosition], nCurWriteSize);
		
		if(eNexusError != NEXUS_SUCCESS)
		{
			PrintError("[%s] eNexusError is 0x%x \n", __func__, eNexusError);
			eRet = DRV_ERR;
		}
		
		nWriteLength = nWriteLength - nCurWriteSize;
		nCurPosition = nCurPosition + nCurWriteSize;
		
	}while(nWriteLength > 0);

	return eRet;
}

/*******************************************************************************
* function : DRV_SPI_ReceivePacket
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_SPI_ReceivePacket(DRV_SPI_DEVICE_e eDevice, unsigned char mode, unsigned char *pucRxData, int nRxDataSize)
{
	DRV_Error eRet = DRV_OK;
	NEXUS_Error	eNexusError = NEXUS_SUCCESS;	
	int	nReadLength = 0;
	int	nCurReadSize = 0;
	int nCurPosition = 0;

	unsigned char bitPerTransfer = 0;
	NEXUS_SpiSettings	tSpiSetting;

	BSTD_UNUSED(eDevice);
	
	if ((pucRxData == NULL) || (nRxDataSize <= 0) ||(nRxDataSize > SPI_MAX_TRANSFER_SIZE))
	{
		PrintError("[DRV_SPI_ReceivePacket] ERROR : Invalid Paramter!!! nRxDataSize: %d \n", nRxDataSize);
		return DRV_ERR_INVALID_PARAMETER;
	}

	nReadLength = nRxDataSize;
	nCurPosition = 0;

	if(mode == 0)
	{
		NEXUS_Spi_GetSettings(s_ptNexusSpiHandle, &tSpiSetting);
		bitPerTransfer = tSpiSetting.bitsPerTransfer;
	}
	else
	{
		bitPerTransfer = mode;
	}
	
	do
	{
		if(bitPerTransfer == 8)
		{
			if(nReadLength > SPI_NUMBER_OF_HW_QUEUE_ENTRY)
		{
				nCurReadSize = SPI_NUMBER_OF_HW_QUEUE_ENTRY;
		}
		else
		{
			nCurReadSize = nReadLength;
		}
		}
		else
		{
			if(nReadLength > SPI_NUMBER_OF_HW_QUEUE_ENTRY*2)
			{
				nCurReadSize = SPI_NUMBER_OF_HW_QUEUE_ENTRY*2;
			}
			else
			{
				nCurReadSize = nReadLength;
			}
		}
		
		if(mode == 0)
			eNexusError = NEXUS_Spi_Read(s_ptNexusSpiHandle, s_aucDummyCmd, &pucRxData[nCurPosition], nCurReadSize);
		else
			eNexusError = NEXUS_Spi_ReadbyMode(s_ptNexusSpiHandle,mode,s_aucDummyCmd, &pucRxData[nCurPosition], nCurReadSize);
		
		if(eNexusError != NEXUS_SUCCESS)
		{
			PrintError("[%s] eNexusError is 0x%x \n", __func__, eNexusError);
			eRet = DRV_ERR;
		}
		
		nReadLength = nReadLength - nCurReadSize;
		nCurPosition = nCurPosition + nCurReadSize;
	}while(nReadLength > 0);

	return eRet;
}

/*******************************************************************************
* function : INTRN_DRV_SPI_ChangePinmux
* description : change pinmux gpio<->auto sc control for upload data to sdcard by spi
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
static void INTRN_DRV_SPI_ChangePinmux(unsigned char ucUseGpio)
{
	if(ucUseGpio)
	{
		DRV_GPIO_ChangePinmux(GPIO_ID_SC1_VCC, GPIO_PINMUX_GPIO);
		DRV_GPIO_ChangePinmux(GPIO_ID_SC1_RESET, GPIO_PINMUX_GPIO);
		DRV_GPIO_ChangePinmux(GPIO_ID_SC1_DATA, GPIO_PINMUX_GPIO);
	}
	else
	{
		DRV_GPIO_ChangePinmux(GPIO_ID_SC1_VCC, GPIO_PINMUX_ALT1);
		DRV_GPIO_ChangePinmux(GPIO_ID_SC1_RESET, GPIO_PINMUX_ALT1);
		DRV_GPIO_ChangePinmux(GPIO_ID_SC1_DATA, GPIO_PINMUX_ALT1);
	}
}

/* end of file */
