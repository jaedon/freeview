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
#ifndef _FDI_MMC_H_
#define _FDI_MMC_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fdi_err.h"

/******************************************************************************
* Definitions, typdef & const data

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
typedef enum
{
	FDI_MMC_EVENT_INSERT,
	FDI_MMC_EVENT_EXTRACT,
	FDI_MMC_EVENT_MAX
} FDI_MMC_EVENT_e;

typedef struct 
{
	char 	dev_name[16];
	int		portnum;

} FDI_MMC_INFO_t;


typedef enum {
	FDI_MMC_PORT_1 = 1,
	FDI_MMC_PORT_MAX
} FDI_MMC_PORT_E;


typedef enum {
	FDI_MMC_SVC_EVT_FORMAT_DONE = 0,
	FDI_MMC_SVC_EVT_FORMAT_FAIL,
	FDI_MMC_SVC_EVT_MOUNT_DONE,
	FDI_MMC_SVC_EVT_MOUNT_FAIL,
	FDI_MMC_SVC_EVT_UMOUNT_DONE,
	FDI_MMC_SVC_EVT_UMOUNT_FAIL,
	FDI_MMC_SVC_EVT_MAX
} FDI_MMC_SVC_EVT_e;

typedef FDI_ERR_CODE (*pfnFDI_MMC_NotifyFunc)(FDI_MMC_EVENT_e eEventType, FDI_MMC_INFO_t *ptSDMEMInfo);
typedef FDI_ERR_CODE	(*pfnFDI_MMC_SvcCallbackFunc)(FDI_MMC_SVC_EVT_e evt);

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
FDI_ERR_CODE FDI_MMC_Open(void);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_MMC_Mount(FDI_MMC_PORT_E port, unsigned char *mount_dir);

FDI_ERR_CODE FDI_MMC_GetInfo(FDI_MMC_INFO_t *tMMCInfo);

FDI_ERR_CODE FDI_MMC_GetWriteLockState(BOOL *pbState);

FDI_ERR_CODE FDI_MMC_GetDataLineWidth(unsigned char *pcWidth);

FDI_ERR_CODE FDI_MMC_GetSerialNumber(unsigned char *pucSN);

FDI_ERR_CODE FDI_MMC_RegisterCallback(pfnFDI_MMC_NotifyFunc pfnCallback);

#endif




