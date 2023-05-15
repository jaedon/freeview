/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :		 Factory System Driver Interface
*
* Copyright (c) 2009 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#include "fdi_common.h"
#include "fdi_led.h"

#include "di_err.h"
#include "di_led.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
static unsigned long g_ulNumOfLed;
unsigned long g_ulLedMask=0;

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_LED_Open(void)
{
	unsigned long ulLedMask, i;
	
	DI_LED_GetCapability((HUINT32*)&g_ulLedMask);
	for( i = 0; i < 32; i++ )
	{
		if( g_ulLedMask&(1<<i) )
		{
			g_ulNumOfLed++;
		}
	}
	g_ulNumOfLed++; /* for power led */
	
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_LED_GetCapability(unsigned long *pulNumOfLed)
{
	*pulNumOfLed = g_ulNumOfLed;

	return FDI_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_LED_Control(unsigned char ucLedId, BOOL bOnOff)
{
	DI_ERR_CODE	eDiErr;
	DI_LED_ID ledId;
	
	#if 0
	if( ucLedId >= g_ulNumOfLed )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_LED_Control] Invalid Led ID (%d)!!\n", ucLedId));
		return FDI_ERR;
	}
	#endif
	
	if( ucLedId == 0 )
	{
#if defined(STANDBY_LED_CONTROL_SWAP)	
		if( bOnOff == TRUE )
		{
			eDiErr = DI_LED_SetPowerLedState(DI_LED_STANDBY);
		}
		else
		{
			eDiErr = DI_LED_SetPowerLedState(DI_LED_WATCHING);
		}
		if( eDiErr != DI_ERR_OK )
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_LED_Control] DI_LED_SetPowerLedState Error : %d\n", eDiErr));
			return FDI_ERR; 
		}
#else
		if( bOnOff == TRUE )
		{
			eDiErr = DI_LED_SetPowerLedState(DI_LED_WATCHING);
		}
		else
		{
			eDiErr = DI_LED_SetPowerLedState(DI_LED_STANDBY);
		}
		if( eDiErr != DI_ERR_OK )
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_LED_Control] DI_LED_SetPowerLedState Error : %d\n", eDiErr));
			return FDI_ERR; 
		}
#endif
	}
	else
	{
		ledId = 1<<(ucLedId-1);
		//ledId = 1<<ucLedId;
		
		if( bOnOff == TRUE )
		{
			eDiErr = DI_LED_SetOn(ledId);
		}
		else
		{
			eDiErr = DI_LED_SetOff(ledId);
		}
		if( eDiErr != DI_ERR_OK )
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_LED_Control] DI_LED_SetPowerLedState Error : %d\n", eDiErr));
			return FDI_ERR; 		
		}
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_LED_ControlAll(BOOL bOnOff)
{
#if (FDI_TYPE==FDI_TYPE_FACTORY20)
	DI_ERR_CODE	eDiErr;

	if( bOnOff == TRUE )
	{
		eDiErr = DI_LED_SetAll();		
		eDiErr |= DI_LED_SetPowerLedState(DI_LED_STANDBY);

	}
	else
	{
		eDiErr = DI_LED_ClearAll();
		eDiErr |= DI_LED_SetPowerLedState(DI_LED_WATCHING);
	}

	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_LED_Control] DI_LED_SetPowerLedState Error : %d\n", eDiErr));
		return FDI_ERR; 		
	}

	return FDI_NO_ERR;
#else
	return FDI_ERR;
#endif
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_LED_DimmingLevel (FDI_LED_DIMMING_LEVEL uclevel)
{
	DI_ERR_CODE	eDiErr;
	DI_LED_DIMMING_LEVEL level;


	switch(uclevel)
	{
		case FDI_LED_DIMMING_LOW :
#if defined(LED_DIMMING_LOW)
			level = LED_DIMMING_LOW;
#else
			level = DI_LED_DIMMING_LOW;
#endif
			break;
	
		case FDI_LED_DIMMING_MID :
#if defined(LED_DIMMING_MID)
			level = LED_DIMMING_MID;
#else
			level = DI_LED_DIMMING_MID;
#endif
			break;
	
		case FDI_LED_DIMMING_HIGH :
		default :
#if defined(LED_DIMMING_HIGH)
			level = LED_DIMMING_HIGH;
#else
			level = DI_LED_DIMMING_HIGH;
#endif
			break;
	}

	eDiErr = DI_LED_SetPowerLedDimmingLevel(level);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_LED_DimmingLevel] DI_LED_SetPowerLedDimmingLevel Error : %d\n", eDiErr));
		return FDI_ERR; 		
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/

FDI_ERR_CODE FDI_LED_SetLedsDimmingLevel (unsigned char ucLedId, unsigned char ucDimLevel)
{
	DI_LED_ID eLedId;

	if(ucLedId > 31)
		return FDI_ERR_INVALID_PARAMETER;
	
	/*convert ucLedId to */
	eLedId = (0x01 << ucLedId);
	return (FDI_ERR_CODE)DI_LED_SetLedsDimmingLevel (eLedId, ucDimLevel);
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_LED_SetRGBOn (unsigned char ucLedId, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue)
{
	DI_LED_ID eLedId;

	if(ucLedId > 31)
		return FDI_ERR_INVALID_PARAMETER;
	
	/*convert ucLedId to */
	eLedId = (0x01 << ucLedId);
	return (FDI_ERR_CODE)DI_LED_SetRGBOn(eLedId, ucRed, ucGreen, ucBlue);
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_LED_SetPowerLedState (FDI_LED_STATE state)
{
	DI_LED_STATE diLedState = DI_LED_OFF;
	
	if(state == FDI_LED_WATCHING)
		diLedState = DI_LED_WATCHING;
	else if(state == FDI_LED_STANDBY)
		diLedState = DI_LED_STANDBY;
	else if(state == FDI_LED_WATCHING_LEDLOW)
		diLedState = DI_LED_WATCHING_LEDLOW;
	else if(state == FDI_LED_STANDBY_LEDLOW)
		diLedState = DI_LED_STANDBY_LEDLOW;
	else 
		diLedState = DI_LED_OFF;
		
	return (FDI_ERR_CODE)DI_LED_SetPowerLedState(diLedState);
}

