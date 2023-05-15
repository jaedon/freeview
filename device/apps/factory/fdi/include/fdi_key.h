/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Error Code Definition
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FDI_KEY_H_
#define _FDI_KEY_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fdi_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum
{
	FDI_FRONT_KEY = 0,
	FDI_RCU_KEY
} FDI_KEY_DEVICE;

typedef enum
{
	FDI_KEY_PRESSED = 0,
	FDI_KEY_REPEATED,
	FDI_KEY_RELEASED
} FDI_KEY_TYPE;

typedef void (*FDI_KEY_EVENT_CALLBACK)(FDI_KEY_DEVICE eDev, unsigned int nKeyCode, FDI_KEY_TYPE eKeyType);


typedef enum
{
	FRONT_KEY_VOLDOWN	= 0x00000001,
	FRONT_KEY_VOLUP		= 0x00000002,
	FRONT_KEY_CHDOWN 	= 0x00000004,
	FRONT_KEY_CHUP		= 0x00000008,
	FRONT_KEY_MENU		= 0x00000010,
	FRONT_KEY_INPUT		= 0x00000020,
	FRONT_KEY_POWER		= 0x00000040,
	FRONT_KEY_OK		= 0x00000080,
	FRONT_KEY_PLAY		= 0x00000100,
	FRONT_KEY_STOP		= 0x00000200,
	FRONT_KEY_EJECT		= 0x00000400,
	FRONT_KEY_TVRADIO	= 0x00000800,
	FRONT_KEY_UP		= 0x00001000,
	FRONT_KEY_DOWN		= 0x00002000,
	FRONT_KEY_LEFT		= 0x00004000,
	FRONT_KEY_RIGHT		= 0x00008000,
	FRONT_KEY_GUIDE		= 0x00010000,
	FRONT_KEY_BACK		= 0x00020000,
	FRONT_KEY_INFO		= 0x00040000,
#if defined(CONFIG_DI10)
	FRONT_KEY_REC 		= 0x00080000,
	FRONT_KEY_HDD_DISC 	= 0x00100000,
	FRONT_KEY_LIST		= 0x00200000,
#endif
#if defined(CONFIG_DI20)
	FRONT_KEY_RESERVED	= 0x00080000,
	FRONT_KEY_WPS 		= 0x00100000,
	FRONT_KEY_NETWORK	= 0x00200000,
	FRONT_KEY_SW_REBOOT	= 0x00400000,
#endif	
	FRONT_KEY_NONE		= 0x80000000,
	FRONT_KEY_MAX		= 0x80000000
} FDI_FKEY_TYPE;

typedef enum
{
	RCU_KEY_STANDBY 	= 0x01,
		
	RCU_KEY_0		= 0x10,
	RCU_KEY_1		= 0x11,
	RCU_KEY_2		= 0x12,
	RCU_KEY_3		= 0x13,
	RCU_KEY_4		= 0x14,
	RCU_KEY_5		= 0x15,
	RCU_KEY_6		= 0x16,
	RCU_KEY_7		= 0x17,
	RCU_KEY_8		= 0x18,
	RCU_KEY_9		= 0x19,
	RCU_KEY_11		= 0x1A,
	RCU_KEY_12		= 0x1B,

	RCU_KEY_RED		= 0x20,
	RCU_KEY_GREEN		= 0x21,
	RCU_KEY_YELLOW		= 0x22,
	RCU_KEY_BLUE		= 0x23,

	RCU_KEY_INFO		= 0x30,
	RCU_KEY_MENU		= 0x32,	
	RCU_KEY_GUIDE		= 0x31,
	RCU_KEY_LIST		= 0x35,
	RCU_KEY_VFORMAT 	= 0x37,
	
	RCU_KEY_UP		= 0x42,
	RCU_KEY_DOWN		= 0x43,
	RCU_KEY_LEFT		= 0x44,
	RCU_KEY_RIGHT		= 0x45,
	RCU_KEY_BACK		= 0x46,	
	RCU_KEY_EXIT		= 0x47,
	RCU_KEY_OK		= 0x48,

	RCU_KEY_CHUP		= 0x50,
	RCU_KEY_CHDOWN		= 0x51,	
	RCU_KEY_LAST		= 0x52,	
	RCU_KEY_TVRADIO 	= 0x53,

	RCU_KEY_VOLUP		= 0x60,
	RCU_KEY_VOLDOWN 	= 0x61,
	RCU_KEY_MUTE		= 0x62,
	RCU_KEY_SOURCE		= 0x6A,
	
	RCU_KEY_PAUSE		= 0x82,
	RCU_KEY_PREV		= 0x87,
	
	RCU_KEY_DT		= 0x1000,
	RCU_KEY_BS		= 0x1001,
	RCU_KEY_CATV		= 0x1002,
	RCU_KEY_DATA		= 0x1005,
	
	RCU_KEY_INOUT		= 0xFFFFFFFE,	
	RCU_KEY_NONE		= 0xFFFFFFFF,	
	RCU_KEY_MAX		= 0xFFFFFFFF
} FDI_RCU_TYPE;

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
* function : FDI_KEY_Init
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_KEY_Open(void);

/*******************************************************************************
* function : FAI_APP_RegisterEventCallback
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_KEY_RegisterEventCallback(FDI_KEY_EVENT_CALLBACK pfnCallback);

#endif

