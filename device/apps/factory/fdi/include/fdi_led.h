/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory System Driver Interface
*
* Copyright (c) 2009 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FDI_LED_H_
#define _FDI_LED_H_

/******************************************************************************
* header and definitions
******************************************************************************/

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum
{
	FDI_LED_OFF,
	FDI_LED_WATCHING,
	FDI_LED_STANDBY,
	FDI_LED_STANDBY_LEDLOW,
	FDI_LED_WATCHING_LEDLOW
} FDI_LED_STATE ;

typedef enum
{
	FDI_LED_DIMMING_HIGH = 0,
	FDI_LED_DIMMING_MID	= 1,
	FDI_LED_DIMMING_LOW	= 2
} FDI_LED_DIMMING_LEVEL;


/******************************************************************************
* Global variables and structures
******************************************************************************/
/* global variables and structures */

/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */

/******************************************************************************
* Static variables and structures
******************************************************************************/
/* static variables and structures */


/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_LED_Open(void);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_LED_GetCapability(unsigned long *pulNumOfLed);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_LED_Control(unsigned char ucLedId, BOOL bOnOff);


/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_LED_DimmingLevel (FDI_LED_DIMMING_LEVEL uclevel);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_LED_SetRGBOn (unsigned char ucLedId, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/

FDI_ERR_CODE FDI_LED_SetLedsDimmingLevel (unsigned char ucLedId, unsigned char ucDimLevel);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_LED_SetPowerLedState (FDI_LED_STATE state);

#endif

