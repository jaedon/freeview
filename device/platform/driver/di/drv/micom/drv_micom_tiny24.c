/**
 * drv_micom_tiny24.c
*/

/**
 * @defgroup		DRV_MICOM DRV_KPD : DRV_MICOM apis
 * @author			Byungmin Ha
 * @note			DRV_MICOM APIs
 * @brief			Define DRV_MICOM APIs
 * @file 			drv_micom_tiny24.c
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* include  common utils headers */
#include "linden_trace.h"

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/times.h>
#include <sys/time.h>
#include <time.h>
#include "bstd.h"

#include "vkernel.h"
#include "taskdef.h"
#include "di_uart.h"
#include "drv_key.h"
#include "drv_micom.h"

#include "drv_i2c.h"
#include "drv_gpio.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE							TRACE_MODULE_DRV_MICOM

#define UNUSED(x) 								(void)(x)

#define DRV_MICOM_NORNAL_LED_ID 				0x00
#define DRV_MICOM_STANDBY_LED_ID 				0x01

#define DRV_MICOM_BLUE_LED_ON 					0x01
#define DRV_MICOM_RED_LED_ON 					0x00

/* micom i2c channel */
#define DRV_I2C_CHANNEL_MICOM	 				I2C_CHANNEL_MICOM
#define DRV_MICOM_CHIP_ADDR						0x5A

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/*******************************************************************/
/******************* constant & gloval variables *******************/
/*******************************************************************/
unsigned int ulMicomVersion = 0;

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static unsigned long s_ulMicomTxRxSema;

#define	GET_MICOM_SEMA 		VK_SEM_Get(s_ulMicomTxRxSema)
#define	REL_MICOM_SEMA 		VK_SEM_Release(s_ulMicomTxRxSema)

/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/
static void MakeCheckSum(unsigned char *pCheckSum, int CheckNum);

/********************************************************************
 Function   : DRV_MICOM_Init
 Description :
 Input      : none
 Output     : none
 Return     : DRV_Error
 ********************************************************************/
DRV_Error DRV_MICOM_Init(void)
{
	int	retval = -1;

	PrintEnter();

	retval = VK_SEM_Create(&s_ulMicomTxRxSema, "micom_rxtx", DEFAULT_SUSPEND_TYPE);
	if (retval != VK_OK)
	{
		PrintError("[DRV_MICOM_Init] VK_SEM_Create Create Error\n");

		return DRV_ERR;
	}

	retval = DRV_MICOM_GetVersion(&ulMicomVersion);
	if (retval != DRV_OK)
	{
		PrintError("Error(0x%x) In DRV_MICOM_GetVersion()\n", retval);
	}

	DI_UART_Print("\033[01;34m\nMicomVersion : %x.%2x(major.minor)\n\n\033[00m", (ulMicomVersion & 0xFF00) >> 8, ulMicomVersion & 0x00FF);

	/* Rescue reset (When pressing standby button over 3 seconds, reset) enable */
 	retval = DRV_MICOM_EnableRescueReset(TRUE);
	if (retval != DRV_OK)
	{
		PrintError("DRV_MICOM_EnableRescueReset (TRUE) Error\n");
	}

	PrintExit();

	return DRV_OK;
}

/*******************************************************************
Function   : DRV_MICOM_TX
Description :
Input      : ucTxNum, pDataBuffer
Output     : none
Return     : DRV_Error
********************************************************************/
static DRV_Error DRV_MICOM_TX(int ucTxNum, unsigned char *pDataBuffer)
{
	int retval = -1;

	PrintEnter();

	retval = DRV_I2c_WriteNoAddr(DRV_I2C_CHANNEL_MICOM, (DRV_MICOM_CHIP_ADDR >> 1), pDataBuffer, ucTxNum);
	if (retval < 0)
	{
		PrintError("[DRV_MICOM_TX] I2C Write Error(0x%x)\n", retval);

		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}

/*******************************************************************
Function   : DRV_MICOM_RX
Description :
Input	   : ucTxNum, pDataBuffer
Output	   : none
Return	   : DRV_Error
********************************************************************/
static DRV_Error DRV_MICOM_RX(int ucRxNum, unsigned char *pDataBuffer)
{
	int 	retval = -1;
	char 	buf[9] = { 0, };

	PrintEnter();

	retval = DRV_I2c_ReadNoAddr(DRV_I2C_CHANNEL_MICOM, (DRV_MICOM_CHIP_ADDR >> 1), pDataBuffer, ucRxNum);
	if (retval == 1)
	{
		PrintError("[%s(%d)] I2C Read Error\n", __func__, __LINE__);

		return DRV_ERR;
	}

	/* RX Buffer Flush */
	retval = DRV_I2c_ReadNoAddr(DRV_I2C_CHANNEL_MICOM, (DRV_MICOM_CHIP_ADDR >> 1), buf, 9);
	if (retval == 1)
	{
		PrintError("[%s(%d)] I2C Read Error\n", __func__, __LINE__);

		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}

/*******************************************************************
Function	  : DRV_MICOM_GetVersion
Description :
Input   : none
Output  : *ulVersion
Return  : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_GetVersion( unsigned int *pulVersion )
{
	unsigned char 	ucModelData[DRV_MICOM_MESSAGE_BYTES] = { 0, };
	int				result = DRV_OK;

	PrintEnter();

	ucModelData[MICOM_HEADER]	= MICOM_FRAME_HEADER;
	ucModelData[MICOM_LENGTH]	= MICOM_FRAME_LENGTH_ONE;
	ucModelData[MICOM_COMMAND]	= MICOM_FRAME_CMD_GET_VER;

	MakeCheckSum(ucModelData, CHECK_NUM_THREE);

	GET_MICOM_SEMA;

	result = DRV_MICOM_TX((int)MICOM_FRAME_LENGTH_FOUR, ucModelData);
	if (result != DRV_OK)
	{
		REL_MICOM_SEMA;

		PrintError("[DRV_MICOM_GetVersion] DRV_MICOM_TX Error\n");

		return DRV_ERR;
	}

	result = DRV_MICOM_RX((int)MICOM_FRAME_LENGTH_SIX, ucModelData);

	REL_MICOM_SEMA;

	if (result == DRV_OK)
	{
		if (MICOM_FRAME_CMD_GET_VER == ucModelData[MICOM_COMMAND])
		{
			*pulVersion = (ucModelData[MICOM_DATA_ONE] << 8) + (ucModelData[MICOM_DATA_TWO]);

			PrintExit();

			return DRV_OK;
		}
		else
		{
			PrintError("[DRV_MICOM_GetVersion] MICOM_COMMAND Error\n");

			return DRV_ERR;
		}
	}
	else
	{
		PrintError("[DRV_MICOM_GetVersion] DRV_MICOM_RX Error\n");

		return DRV_ERR;
	}
}

/*******************************************************************
  Function	  : DRV_MICOM_GetStrVersion
  Description :
  Input   : none
  Output  : *pucStrVersion
  Return  : DRV_Error
 ********************************************************************/
DRV_Error DRV_MICOM_GetStrVersion( unsigned char *pucStrVersion )
{
	unsigned char 	ucModelData[DRV_MICOM_MESSAGE_BYTES] = { 0, };
	int				result = DRV_OK;

	PrintEnter();

	ucModelData[MICOM_HEADER]	= MICOM_FRAME_HEADER;
	ucModelData[MICOM_LENGTH]	= MICOM_FRAME_LENGTH_ONE;
	ucModelData[MICOM_COMMAND]	= MICOM_FRAME_CMD_GET_STRVER;

	MakeCheckSum(ucModelData, CHECK_NUM_THREE);

	GET_MICOM_SEMA;

	result = DRV_MICOM_TX((int)MICOM_FRAME_LENGTH_FOUR, ucModelData);
	if (result != DRV_OK)
	{
		REL_MICOM_SEMA;

		PrintError("[DRV_MICOM_GetStrVersion] DRV_MICOM_TX Error\n");

		return DRV_ERR;
	}

	result = DRV_MICOM_RX((int)14, ucModelData);

	REL_MICOM_SEMA;

	if (result == DRV_OK)
	{
		if (MICOM_FRAME_CMD_GET_STRVER == ucModelData[MICOM_COMMAND])
		{
			VK_memcpy(pucStrVersion, &(ucModelData[3]), 10);

			PrintExit();

			return DRV_OK;
		}
		else
		{
			PrintError("[DRV_MICOM_GetStrVersion] MICOM_COMMAND Error\n");

			return DRV_ERR;
		}
	}
	else
	{
		PrintError("[DRV_MICOM_GetStrVersion] DRV_MICOM_RX Error\n");

		return DRV_ERR;
	}
}

/*******************************************************************
Function	  : DRV_MICOM_FrontLedControl
Description :
Input   : blue_red(blue:1, red:0)
Output  :
Return  : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_FrontLedControl(unsigned char blue_red)
{
	unsigned char 	ucSetLedControlData[DRV_MICOM_MESSAGE_BYTES] = { 0, };
	int				result = DRV_OK;

	PrintEnter();

	ucSetLedControlData[MICOM_HEADER] 		= MICOM_FRAME_HEADER;
	ucSetLedControlData[MICOM_LENGTH] 		= MICOM_FRAME_LENGTH_THREE;
	ucSetLedControlData[MICOM_COMMAND] 		= MICOM_FRAME_CMD_SET_LED;
	ucSetLedControlData[MICOM_DATA_ONE] 	= DRV_MICOM_STANDBY_LED_ID;
	if (blue_red == DRV_MICOM_BLUE_LED_ON)
		ucSetLedControlData[MICOM_DATA_TWO] = DRV_MICOM_BLUE_LED_ON;
	else
		ucSetLedControlData[MICOM_DATA_TWO] = DRV_MICOM_RED_LED_ON;

	MakeCheckSum(ucSetLedControlData, CHECK_NUM_FIVE);

	PrintError("[DRV_MICOM_SetFrontPowerLedControl] Tx Data : %x %x %x %x %x %x \n", ucSetLedControlData[0], ucSetLedControlData[1], ucSetLedControlData[2], ucSetLedControlData[3], ucSetLedControlData[4], ucSetLedControlData[5]);

	GET_MICOM_SEMA;

	result = DRV_MICOM_TX(MICOM_FRAME_LENGTH_SIX, ucSetLedControlData);
	if (result != DRV_OK)
	{
		REL_MICOM_SEMA;

		PrintError("[DRV_MICOM_SetFrontPowerLedControl] DRV_MICOM_TX Error\n");

		return DRV_ERR;
	}

	result = DRV_MICOM_RX((int)MICOM_FRAME_LENGTH_FOUR, ucSetLedControlData);

	REL_MICOM_SEMA;

	if (result == DRV_OK)
	{
		if (ucSetLedControlData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_LED)
		{
			PrintExit();

			return DRV_OK;
		}
		else
		{
			PrintError("[DRV_MICOM_SetFrontPowerLedControl] MICOM_COMMAND Error\n");

			return DRV_ERR;
		}
	}
	else
	{
		PrintError("[DRV_MICOM_SetFrontPowerLedControl] DRV_MICOM_RX Error\n");

		return DRV_ERR;
	}
}

/********************************************************************
 Function   : DRV_MICOM_SetLocalTime
 Description :
 Input      : stDateTime
 Output     : none
 Return     : DRV_OK
********************************************************************/
DRV_Error DRV_MICOM_SetLocalTime(unsigned int unixTime)
{
	unsigned char 	ucTimeData[DRV_MICOM_MESSAGE_BYTES] = { 0, };
	int				result = DRV_OK;

	PrintEnter();

	ucTimeData[MICOM_HEADER] 		= MICOM_FRAME_HEADER;
	ucTimeData[MICOM_LENGTH] 		= MICOM_FRAME_LENGTH_FIVE;
	ucTimeData[MICOM_COMMAND] 		= MICOM_FRAME_CMD_SET_TIME;
	ucTimeData[MICOM_DATA_ONE] 		= (unsigned char)((unixTime >> 24) & UNIXTIME_BYTE_MASK);
	ucTimeData[MICOM_DATA_TWO] 		= (unsigned char)((unixTime >> 16) & UNIXTIME_BYTE_MASK);
	ucTimeData[MICOM_DATA_THREE] 	= (unsigned char)((unixTime >> 8) & UNIXTIME_BYTE_MASK);
	ucTimeData[MICOM_DATA_FOUR] 	= (unsigned char)(unixTime & UNIXTIME_BYTE_MASK);

	MakeCheckSum(ucTimeData, CHECK_NUM_SEVEN);

	PrintDebug("[DRV_MICOM_SetLocalTime] after convert unixtime %x, (%x:%x:%x:%x)\n",
				unixTime, ucTimeData[MICOM_DATA_ONE], ucTimeData[MICOM_DATA_TWO], ucTimeData[MICOM_DATA_THREE], ucTimeData[MICOM_DATA_FOUR]);

	GET_MICOM_SEMA;

	result = DRV_MICOM_TX(MICOM_FRAME_LENGTH_EIGHT, ucTimeData);
	if (result != DRV_OK)
	{
		REL_MICOM_SEMA;

		PrintError("[DRV_MICOM_SetLocalTime] DRV_MICOM_TX Error\n");

		return DRV_ERR;
	}

	result = DRV_MICOM_RX(MICOM_FRAME_LENGTH_FOUR, ucTimeData);

	REL_MICOM_SEMA;

	if (result == DRV_OK)
	{
		if (ucTimeData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_TIME)
		{
			PrintExit();

			return DRV_OK;
		}
		else
		{
			PrintError("[DRV_MICOM_SetLocalTime] MICOM_COMMAND Error\n");

			return DRV_ERR;
		}
	}
	else
	{
		PrintError("[DRV_MICOM_SetLocalTime] DRV_MICOM_RX Error\n");

		return DRV_ERR;
	}
}

/********************************************************************
 Function   : DRV_MICOM_GetLocalTime
 Description :
 Input      : pstDateTime
 Output     : pstDateTime
 Return     : DRV_Error
 ********************************************************************/
DRV_Error  DRV_MICOM_GetLocalTime(unsigned int *punixTime)
{
	unsigned char 	ucTimeData[DRV_MICOM_MESSAGE_BYTES] = { 0, };
	int				result = DRV_OK;

	PrintEnter();

	ucTimeData[MICOM_HEADER] 	= MICOM_FRAME_HEADER;
	ucTimeData[MICOM_LENGTH] 	= MICOM_FRAME_LENGTH_ONE;
	ucTimeData[MICOM_COMMAND] 	= MICOM_FRAME_CMD_GET_TIME;

	MakeCheckSum(ucTimeData, CHECK_NUM_THREE);

	GET_MICOM_SEMA;

	result = DRV_MICOM_TX(MICOM_FRAME_LENGTH_FOUR, ucTimeData);
	if (result != DRV_OK)
	{
		REL_MICOM_SEMA;

		PrintError( "[DRV_MICOM_RequestLocalTime] DRV_MICOM_TX Error \n" );

		return DRV_ERR;
	}

	result = DRV_MICOM_RX(MICOM_FRAME_LENGTH_TEN, ucTimeData);

	REL_MICOM_SEMA;

	if (result == DRV_OK)
	{
		if (ucTimeData[MICOM_COMMAND] == MICOM_FRAME_CMD_GET_TIME)
		{
			*punixTime = (unsigned int)(ucTimeData[MICOM_DATA_ONE] << 24)
									| (ucTimeData[MICOM_DATA_TWO] << 16)
									| (ucTimeData[MICOM_DATA_THREE] << 8)
									| (ucTimeData[MICOM_DATA_FOUR]);

			if (*punixTime < VALID_UNIXTIME)
			{
				*punixTime = VALID_UNIXTIME;
			}

			PrintDebug("[DRV_MICOM_GetLocalTime] received unix time : %x\n", *punixTime);

			PrintExit();

			return DRV_OK;
		}
		else
		{
			PrintError("[DRV_MICOM_RequestLocalTime] MICOM_COMMAND Error\n");

			return DRV_ERR;
		}
	}
	else
	{
		PrintError("[DRV_MICOM_RequestLocalTime] DRV_MICOM_RX Error\n");

		return DRV_ERR;
	}
}

/********************************************************************
 Function   : DRV_MICOM_SetWakeupTime
 Description :
 Input      : stDateTime
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error DRV_MICOM_SetWakeupTime(unsigned int unixTime)
{
	unsigned char 	ucTimeData[DRV_MICOM_MESSAGE_BYTES] = { 0, };
	int				result = DRV_OK;

	PrintEnter();

	ucTimeData[MICOM_HEADER] 		= MICOM_FRAME_HEADER;
	ucTimeData[MICOM_LENGTH] 		= MICOM_FRAME_LENGTH_SIX;
	ucTimeData[MICOM_COMMAND] 		= MICOM_FRAME_CMD_SET_WTIME;
	ucTimeData[MICOM_DATA_ONE] 		= MICOM_FRAME_CMD_SET_WTIME_ON;
	ucTimeData[MICOM_DATA_TWO] 		= (unsigned char)((unixTime >> 24) & UNIXTIME_BYTE_MASK);
	ucTimeData[MICOM_DATA_THREE] 	= (unsigned char)((unixTime >> 16) & UNIXTIME_BYTE_MASK);
	ucTimeData[MICOM_DATA_FOUR] 	= (unsigned char)((unixTime >> 8) & UNIXTIME_BYTE_MASK);
	ucTimeData[MICOM_DATA_FIVE] 	= (unsigned char)(unixTime & UNIXTIME_BYTE_MASK);

	MakeCheckSum(ucTimeData, CHECK_NUM_EIGHT);

	GET_MICOM_SEMA;

	result = DRV_MICOM_TX(MICOM_FRAME_LENGTH_NINE, ucTimeData);
	if (result != DRV_OK)
	{
		REL_MICOM_SEMA;

		PrintError("[DRV_MICOM_SetWakeupTime] DRV_MICOM_TX Error\n");

		return DRV_ERR;
	}

	result = DRV_MICOM_RX(MICOM_FRAME_LENGTH_FOUR, ucTimeData);

	REL_MICOM_SEMA;

	if (result == DRV_OK)
	{
		if (ucTimeData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_WTIME)
		{
			PrintExit();

			return DRV_OK;
		}
		else
		{
			PrintError("[DRV_MICOM_SetWakeupTime] MICOM_COMMAND Error\n");

			return DRV_ERR;
		}
	}
	else
	{
		PrintError("[DRV_MICOM_SetWakeupTime] DRV_MICOM_TX Error\n");

		return DRV_ERR;
	}
}

/********************************************************************
Function   : DRV_MICOM_RequestWakeupTime
Description :
Input      : none
Output     : none
Return     : DRV_OK
********************************************************************/
DRV_Error  DRV_MICOM_RequestWakeupTime(unsigned int *punixTime, unsigned char *OnOff)
{
	unsigned char	ucTimeData[DRV_MICOM_MESSAGE_BYTES] = { 0, };
	int				result = DRV_OK;

	PrintEnter();

	ucTimeData[MICOM_HEADER] 	= MICOM_FRAME_HEADER;
	ucTimeData[MICOM_LENGTH] 	= MICOM_FRAME_LENGTH_ONE;
	ucTimeData[MICOM_COMMAND] 	= MICOM_FRAME_CMD_GET_WAKEUPTIME;

	MakeCheckSum(ucTimeData, CHECK_NUM_THREE);

	GET_MICOM_SEMA;

	result = DRV_MICOM_TX(MICOM_FRAME_LENGTH_FOUR, ucTimeData);
	if (result != DRV_OK)
	{
		 REL_MICOM_SEMA;

		 PrintError("[DRV_MICOM_RequestWakeupTime] DRV_MICOM_TX Error\n");

		 return DRV_ERR;
	}

	result = DRV_MICOM_RX(MICOM_FRAME_LENGTH_NINE, ucTimeData);

	REL_MICOM_SEMA;

	if (result == DRV_OK)
	{
		if (ucTimeData[MICOM_COMMAND] == MICOM_FRAME_CMD_GET_WAKEUPTIME)
		{
			*OnOff = ucTimeData[MICOM_DATA_ONE];

			*punixTime = (unsigned int)(ucTimeData[MICOM_DATA_TWO] << 24)
						| (ucTimeData[MICOM_DATA_THREE] << 16)
						| (ucTimeData[MICOM_DATA_FOUR] << 8)
						| (ucTimeData[MICOM_DATA_FIVE]);

			PrintExit();

			return DRV_OK;
		}
		else
		{
			PrintError("[DRV_MICOM_RequestWakeupTime] MICOM_COMMAND Error\n");

			return DRV_ERR;
		}
	}
	else
	{
	 	PrintError("[DRV_MICOM_RequestWakeupTime] DRV_MICOM_RX Error\n");

	 	return DRV_ERR;
	}
}

/********************************************************************
Function   : DRV_MICOM_DelWakeupTime
Description :
Input      : none
Output     : none
Return     : DRV_OK
********************************************************************/
DRV_Error  DRV_MICOM_DelWakeupTime( void )
{
	unsigned char 	ucDelWakeUpData[DRV_MICOM_MESSAGE_BYTES] = { 0, };
	int				result = DRV_OK;

	PrintEnter();

	ucDelWakeUpData[MICOM_HEADER] 	= MICOM_FRAME_HEADER;
	ucDelWakeUpData[MICOM_LENGTH] 	= MICOM_FRAME_LENGTH_TWO;
	ucDelWakeUpData[MICOM_COMMAND] 	= MICOM_FRAME_CMD_SET_WTIME;
	ucDelWakeUpData[MICOM_DATA_ONE] = MICOM_FRAME_CMD_SET_WTIME_OFF;

	MakeCheckSum(ucDelWakeUpData, CHECK_NUM_FOUR);

	GET_MICOM_SEMA;

	result = DRV_MICOM_TX(MICOM_FRAME_LENGTH_FIVE, ucDelWakeUpData);
	if (result != DRV_OK)
	{
		REL_MICOM_SEMA;

		PrintError("[DRV_MICOM_DelWakeUpTime] DRV_MICOM_TX Error\n");

		return DRV_ERR;
	}

	result = DRV_MICOM_RX(MICOM_FRAME_LENGTH_FOUR, ucDelWakeUpData);

	REL_MICOM_SEMA;

	if (result == DRV_OK)
	{
		if (ucDelWakeUpData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_WTIME)
		{
			PrintExit();

			return DRV_OK;
		}
		else
		{
			PrintError("[DRV_MICOM_DelWakeUpTime] MICOM_COMMAND Error\n");

			return DRV_ERR;
		}
	}
	else
	{
		PrintError("[DRV_MICOM_DelWakeUpTime] DRV_MICOM_RX Error\n");

		return DRV_ERR;
	}
}

 /********************************************************************
 Function   : DRV_MICOM_SetStandby
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error DRV_MICOM_SetStandby( void )
{
	unsigned char 	ucSetStandbyData[DRV_MICOM_MESSAGE_BYTES] = { 0, };
	int				result = DRV_OK;

	PrintEnter();

	ucSetStandbyData[MICOM_HEADER] 		= MICOM_FRAME_HEADER;
	ucSetStandbyData[MICOM_LENGTH] 		= MICOM_FRAME_LENGTH_TWO;
	ucSetStandbyData[MICOM_COMMAND] 	= MICOM_FRAME_CMD_SET_STANDBY;
	ucSetStandbyData[MICOM_DATA_ONE] 	= MICOM_FRAME_CMD_SET_STANDBY_ON;

	MakeCheckSum(ucSetStandbyData, CHECK_NUM_FOUR);

	GET_MICOM_SEMA;

	result = DRV_MICOM_TX(MICOM_FRAME_LENGTH_FIVE, ucSetStandbyData);
	if (result != DRV_OK)
	{
		REL_MICOM_SEMA;

		PrintError("[DRV_MICOM_SetStandby] DRV_MICOM_TX Error\n");

		return DRV_ERR;
	}

	result = DRV_MICOM_RX(MICOM_FRAME_LENGTH_FOUR, ucSetStandbyData);

	REL_MICOM_SEMA;

	if (result == DRV_OK)
	{
		if (ucSetStandbyData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_STANDBY)
		{
			PrintExit();

			return DRV_OK;
		}
		else
		{
			PrintError("[DRV_MICOM_SetStandby] MICOM_COMMAND Error\n");

			return DRV_ERR;
		}
	}
	else
	{
		PrintError("[DRV_MICOM_SetStandby] DRV_MICOM_RX Error\n");

		return DRV_ERR;
	}
}

 /********************************************************************
 Function   : DRV_MICOM_STBPowerOn
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error DRV_MICOM_STBPowerOn( void )
{
	PrintError("[DRV_MICOM_STBPowerOn] Un-supported Function.\n");

	return DRV_OK;
}

/********************************************************************
Function   : DRV_MICOM_STBPowerReboot
Description :
Input	  : none
Output	  : none
Return	  : DRV_OK
********************************************************************/
DRV_Error DRV_MICOM_STBPowerReboot( void )
{
	unsigned char	ucSTBPowerData[DRV_MICOM_MESSAGE_BYTES] = { 0, };
	int				result = DRV_OK;

	PrintEnter();

	ucSTBPowerData[MICOM_HEADER] 	= MICOM_FRAME_HEADER;
	ucSTBPowerData[MICOM_LENGTH] 	= MICOM_FRAME_LENGTH_ONE;
	ucSTBPowerData[MICOM_COMMAND] 	= MICOM_FRAME_CMD_POWER_REBOOT;

	MakeCheckSum(ucSTBPowerData, CHECK_NUM_THREE);

	GET_MICOM_SEMA;

	result = DRV_MICOM_TX(MICOM_FRAME_LENGTH_FOUR, ucSTBPowerData);
	if (result != DRV_OK)
	{
		REL_MICOM_SEMA;

		PrintError("[DRV_MICOM_STBPowerReboot] DRV_MICOM_TX Error\n");

		return DRV_ERR;
	}

	result = DRV_MICOM_RX(MICOM_FRAME_LENGTH_FOUR, ucSTBPowerData);

	REL_MICOM_SEMA;

	if (result == DRV_OK)
	{
		if (ucSTBPowerData[MICOM_COMMAND] == MICOM_FRAME_CMD_POWER_REBOOT)
		{
			PrintExit();

			return DRV_OK;
		}
		else
		{
			PrintError("[DRV_MICOM_STBPowerReboot] MICOM_COMMAND Error\n");

			return DRV_ERR;
		}
	}
	else
	{
		PrintError("[DRV_MICOM_STBPowerReboot] DRV_MICOM_RX Error\n");

		return DRV_ERR;
	}
}

 /********************************************************************
 Function   : DRV_MICOM_RequestPowerState
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_RequestPowerState(unsigned char *powerState)
{
	UNUSED(powerState);

	PrintDebug( "[DRV_MICOM_RequestPowerState] Un-supported Function. \n" );

	return DRV_OK;
}

 /********************************************************************
 Function   : DRV_MICOM_SetWatchdog
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error DRV_MICOM_SetWatchdog(HBOOL bOn_nOff, unsigned char ucTimeOut)
{
	UNUSED(bOn_nOff);
	UNUSED(ucTimeOut);

	PrintDebug( "[DRV_MICOM_SetWatchdog] Un-supported Function. \n" );

	return DRV_OK;
}

/********************************************************************
 Function   : DRV_MICOM_DisplayTime
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_DisplayTime(unsigned char on_off)
{
	UNUSED(on_off);

	PrintDebug( "[DRV_MICOM_DisplayTime] Un-supported Function. \n" );

	return DRV_OK;
}

/********************************************************************
 Function   : DRV_MICOM_GetWakeupSrc
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_GetWakeupSrc(DRV_MICOM_WAKEUP_SRC *peWakeupSrc)
{
	unsigned char 	ucGetWakeUpSrcData[DRV_MICOM_MESSAGE_BYTES] = { 0, };
	int				result = DRV_OK;

	PrintEnter();

	ucGetWakeUpSrcData[MICOM_HEADER] 	= MICOM_FRAME_HEADER;
	ucGetWakeUpSrcData[MICOM_LENGTH] 	= MICOM_FRAME_LENGTH_ONE;
	ucGetWakeUpSrcData[MICOM_COMMAND] 	= MICOM_FRAME_CMD_GET_WAKEUPSRC;

	MakeCheckSum(ucGetWakeUpSrcData, CHECK_NUM_THREE);

	GET_MICOM_SEMA;

	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_FOUR, ucGetWakeUpSrcData );
	if (result != DRV_OK)
	{
		REL_MICOM_SEMA;

		PrintError("[DRV_MICOM_GetWakeUpSrc] DRV_MICOM_TX Error\n");

		return DRV_ERR;
	}

	result = DRV_MICOM_RX( MICOM_FRAME_LENGTH_FIVE, ucGetWakeUpSrcData );

	REL_MICOM_SEMA;

	if (result == DRV_OK)
	{
		if (ucGetWakeUpSrcData[MICOM_COMMAND] == MICOM_FRAME_CMD_GET_WAKEUPSRC)
		{
			switch (ucGetWakeUpSrcData[MICOM_DATA_ONE])
			{
				case 1:
					*peWakeupSrc = DRV_MICOM_WAKEUP_BY_KEY;
					break;

				case 2:
					*peWakeupSrc = DRV_MICOM_WAKEUP_BY_RCU;
					break;

				case 3:
					*peWakeupSrc = DRV_MICOM_WAKEUP_BY_TIMER;
					break;

				case 5:
					*peWakeupSrc = DRV_MICOM_WAKEUP_BY_REBOOT;
					break;

				case 9:
				   	*peWakeupSrc = DRV_MICOM_WAKEUP_BY_WOL;
					break;

				case 4:
				default:
					*peWakeupSrc = DRV_MICOM_WAKEUP_BY_ACPOWER;
					break;
			}

			PrintExit();

			return DRV_OK;
		}
		else
		{
			PrintError("[DRV_MICOM_GetWakeUpSrc] MICOM_COMMAND Error\n");

			return DRV_ERR;
		}
	}
	else
	{
		PrintError("[DRV_MICOM_GetWakeUpSrc] DRV_MICOM_RX Error\n");

		return DRV_ERR;
	}
}

DRV_Error DRV_MICOM_SetWakeOnLAN(HBOOL bOn_nOff)
{
	UNUSED(bOn_nOff);

	PrintDebug( "[DRV_MICOM_EnableRescueReset] Un-supported Function. \n" );

	return DRV_OK;
}

DRV_Error DRV_MICOM_EnableRescueReset(HBOOL bOn_nOff)
{
	UNUSED(bOn_nOff);

	PrintDebug( "[DRV_MICOM_EnableRescueReset] Un-supported Function. \n" );

	return DRV_OK;
}

/********************************************************************
 Function   : DRV_MICOM_SET_Icon
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_SET_Icon(unsigned char position, unsigned char value)
{
	UNUSED(position);
	UNUSED(value);

	PrintDebug( "[DRV_MICOM_SET_Icon] Un-supported Function. \n" );

	return DRV_OK;
}

/********************************************************************
 Function   : DRV_MICOM_SET_Icon
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_SetLedControl(HUINT32 ulLedMask, HBOOL bOn_nOff)
{
	UNUSED(ulLedMask);
	UNUSED(bOn_nOff);

	PrintDebug( "[DRV_MICOM_SetLedControl] Un-supported Function. \n" );

	return DRV_OK;
}

/********************************************************************
 Function   : DRV_MICOM_WriteString
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_WriteString(unsigned char *string, unsigned char len)
{
	UNUSED(string);
	UNUSED(len);

	PrintDebug( "[DRV_MICOM_WriteString] Un-supported Function. \n" );

	return DRV_OK;
}

DRV_Error DRV_MICOM_SetPowerSavingMode(HBOOL bOn_nOff)
{
	UNUSED(bOn_nOff);

	PrintDebug( "[DRV_MICOM_SetPowerSavingMode] Un-supported Function. \n" );

	return DRV_OK;
}

DRV_Error DRV_MICOM_SetDisplayAni(HBOOL bOn_nOff)
{
	UNUSED(bOn_nOff);

	PrintDebug("[DRV_MICOM_SetDisplayAni] Un-supported Function. \n");

	return DRV_OK;
}

DRV_Error DRV_MICOM_SetLCDPower(HBOOL bOn_nOff)
{
	UNUSED(bOn_nOff);

	PrintDebug( "[DRV_MICOM_SetLCDPower] Un-supported Function. \n" );

	return DRV_OK;
}

DRV_Error DRV_MICOM_SetLcdDimmingLevel(unsigned char ucLcdDimmingLevel)
{
	UNUSED(ucLcdDimmingLevel);

	PrintDebug( "[DRV_MICOM_SetLcdDimmingLevel] Un-supported Function. \n" );

	return DRV_OK;
}

DRV_Error DRV_MICOM_SetLEDDimmingLevel(unsigned char ucLcdDimmingLevel)
{
	UNUSED(ucLcdDimmingLevel);

	PrintDebug( "[DRV_MICOM_SetLEDDimmingLevel] Un-supported Function. \n" );

	return DRV_OK;
}

DRV_Error DRV_MICOM_SetCustomCode(unsigned short usCCandCCB)
{
	unsigned char 	ucCustomCodeData[DRV_MICOM_MESSAGE_BYTES] = { 0, };
	int				result = DRV_OK;

	PrintEnter();

	ucCustomCodeData[MICOM_HEADER] 		= MICOM_FRAME_HEADER;
	ucCustomCodeData[MICOM_LENGTH] 		= MICOM_FRAME_LENGTH_THREE;
	ucCustomCodeData[MICOM_COMMAND] 	= MICOM_FRAME_CMD_SET_CUSTOMCODE;
	/* HUMAX Custom Code는 실제 RCU에서 오는 IR Signal과 반대로 되어 있어 */
	/* 실제 IR Signal을 인식하기 위해서는 아래와 같이 inverse를 해주어야 한다.  */
	ucCustomCodeData[MICOM_DATA_ONE] 	= (unsigned char)((usCCandCCB & 0xFF00) >> 8);
	ucCustomCodeData[MICOM_DATA_TWO] 	= (unsigned char)(usCCandCCB & 0x00FF);

	MakeCheckSum(ucCustomCodeData, CHECK_NUM_FIVE);

	GET_MICOM_SEMA;

	result = DRV_MICOM_TX( MICOM_FRAME_LENGTH_SIX, ucCustomCodeData );
	if ( result != DRV_OK )
	{
		REL_MICOM_SEMA;

		PrintError("[DRV_MICOM_SetCustomCode] DRV_MICOM_TX Error\n");

		return DRV_ERR;
	}

	result = DRV_MICOM_RX( MICOM_FRAME_LENGTH_FOUR, ucCustomCodeData );

	REL_MICOM_SEMA;

	if (result == DRV_OK)
	{
		if (ucCustomCodeData[MICOM_COMMAND] == MICOM_FRAME_CMD_SET_CUSTOMCODE)
		{
			PrintExit();

			return DRV_OK;
		}
		else
		{
			PrintError("[DRV_MICOM_SetCustomCode] MICOM_COMMAND Error\n");

			return DRV_ERR;
		}
	}
	else
	{
		PrintError("[DRV_MICOM_SetCustomCode] DRV_MICOM_RX Error\n");

		return DRV_ERR;
	}
}

/********************************************************************
Function   : DRV_MICOM_SetUnitTest
Description : This APi is testing micom functions
Input      : none
Output     : none
Return	 : DDI_MICOM_OK

********************************************************************/
DRV_Error DRV_MICOM_SetUnitTest(unsigned char ucFuncNum, HBOOL bOn_nOff)
{
	UNUSED(ucFuncNum);
	UNUSED(bOn_nOff);

	PrintDebug( "[DRV_MICOM_SetUnitTest] Un-supported Function. \n" );

	return DRV_OK;
}

/********************************************************************
 Function   : DRV_MICOM_SetWakeupRcuKey
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error DRV_MICOM_SetWakeupRcuKey(DRV_MICOM_SET_WAKEUPKEY_TYPE nSetType, unsigned char *pucKey, unsigned char ucKeyNum)
{
	UNUSED(nSetType);
	UNUSED(pucKey);
	UNUSED(ucKeyNum);

	PrintDebug("[DRV_MICOM_SetWakeupRcuKey] Un-supported Function. \n");

	return DRV_OK;
}

 /********************************************************************
Function   : DRV_MICOM_GetWakeupRcuKey
Description :
Input	   : none
Output	   : none
Return	   : DRV_OK
********************************************************************/
DRV_Error DRV_MICOM_GetWakeupRcuKey(unsigned char *pucRcuKeyCode)
{
	UNUSED(pucRcuKeyCode);

	PrintDebug("[DRV_MICOM_GetWakeupRcuKey] Un-supported Function. \n");

	return DRV_OK;
}

DRV_Error DRV_MICOM_SetRCUInfo(HBOOL bOn_nOff)
{
	UNUSED(bOn_nOff);

	PrintDebug("[DRV_MICOM_SetRCUInfo] Un-supported Function. \n");

	return DRV_OK;
}

/********************************************************************
 Function   : MakeCheckSum
 Description :
 Input      : pCheckSum, int CheckNum
 Output     : pCheckSum
 Return     : void
********************************************************************/
void MakeCheckSum(unsigned char *pCheckSum, int CheckNum)
{
	int i = 0;

	pCheckSum[CheckNum] = 0;

	for (i = 1; i < CheckNum; i++)
	{
		pCheckSum[CheckNum] += pCheckSum[i];
	}
}


