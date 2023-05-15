/* $Header: $ */
/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Error Code Definition
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FDI_USB_H_
#define _FDI_USB_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fdi_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum
{
	FDI_USB_EVENT_INSERT,
	FDI_USB_EVENT_EXTRACT,
	FDI_USB_EVENT_MAX
} FDI_USB_EVENT_e;

typedef struct FDI_USBInfo
{	
	unsigned long ulUsbId;
	char szUsbDevName[20];
	unsigned long speed;
} FDI_USB_Info_t;

typedef FDI_ERR_CODE (*pfnFDI_USB_NotifyFunc)(FDI_USB_EVENT_e eEventType, FDI_USB_Info_t *ptUsbInfo);

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
FDI_ERR_CODE FDI_USB_Open(void);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_USB_HOST_CTRL_SetPower (unsigned char ucPowerOn);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_USB_GetCapability(unsigned long *pulNumOfUsb);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_USB_RegisterCallback(pfnFDI_USB_NotifyFunc pfnCallback);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_USB_SerialOpen(void);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_USB_SerialSend(unsigned char *pucData, unsigned long ulSize);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_USB_SerialReceive(unsigned char *pucData, unsigned long ulSize, unsigned long ulTimeoutMs);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_USB_SerialClose(void);

#endif

