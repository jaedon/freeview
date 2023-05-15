/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Error Code Definition
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FDI_DEMUX_H_
#define _FDI_DEMUX_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fdi_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum
{
	FDI_DEMUX_CHANNELTYPE_VIDEO = 0,
	FDI_DEMUX_CHANNELTYPE_AUDIO,
	FDI_DEMUX_CHANNELTYPE_AUDIO_MAIN = 1,
	FDI_DEMUX_CHANNELTYPE_AUDIO_DESCRIPTION,
	FDI_DEMUX_CHANNELTYPE_AUDIO_SUB = 2,
	FDI_DEMUX_CHANNELTYPE_AUDIO_SPDIF,
	FDI_DEMUX_CHANNELTYPE_PCR,
	FDI_DEMUX_CHANNELTYPE_RECORD,
	FDI_DEMUX_CHANNELTYPE_ALLPASS,
	FDI_DEMUX_CHANNELTYPE_MAX
} FDI_DEMUX_CHANNELTYPE_e;

typedef enum
{
	FDI_DEMUX_INPUT_PATH_0 = 0,
	FDI_DEMUX_INPUT_PATH_1,
	FDI_DEMUX_INPUT_PATH_2,
	FDI_DEMUX_INPUT_PATH_3,
	FDI_DEMUX_INPUT_PATH_MAX
} FDI_DEMUX_INPUT_PATH_e;

typedef enum
{
	FDI_DEMUX_CI_BYPASS,
	FDI_DEMUX_CI_PASSTHROUGH
} FDI_DEMUX_CI_PATH_e;

typedef struct FDI_DEMUX_capability
{
	unsigned long ulCapability;
	unsigned long NumOfPESFilter;
	unsigned long NumOfSectoinFilter;
	unsigned long NumOfMaximumFilterLength;
} FDI_DEMUX_CAPABILITY;

/******************************************************************************
* Global variables and structures
******************************************************************************/

/******************************************************************************
* External variables and functions
******************************************************************************/

/******************************************************************************
* Static variables and structures
******************************************************************************/

/******************************************************************************
* Static function prototypes
******************************************************************************/

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_DEMUX_GetNumOfDevice(unsigned long *pulNumOfDevice);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_DEMUX_GetCapability(unsigned long ulDeviceId, FDI_DEMUX_CAPABILITY *tpCapability);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_DEMUX_Open(unsigned long ulDeviceId);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_DEMUX_SetPath(unsigned long ulDeviceId, FDI_DEMUX_INPUT_PATH_e ePath, FDI_DEMUX_CI_PATH_e eCiPath);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_DEMUX_ReleasePath(unsigned long ulDeviceId);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_DEMUX_StartPID(unsigned long ulDeviceId, FDI_DEMUX_CHANNELTYPE_e eChannelType, unsigned short usPid, unsigned long *pulChannelId);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_DEMUX_StopPID(unsigned long ulDeviceId, unsigned long ulChannelId);

#endif

