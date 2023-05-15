/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Error Code Definition
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FDI_AUDIO_H_
#define _FDI_AUDIO_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fdi_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum
{
	FDI_AUDIO_FORMAT_MPEG				= 0x00000001,
	FDI_AUDIO_FORMAT_MP3				= 0x00000002,
	FDI_AUDIO_FORMAT_AAC				= 0x00000004,
	FDI_AUDIO_FORMAT_AAC_PLUS		= 0x00000008,
	FDI_AUDIO_FORMAT_AC3				= 0x00000010,
	FDI_AUDIO_FORMAT_AC3_PLUS		= 0x00000020,
	FDI_AUDIO_FORMAT_PCM				= 0x00000040
}FDI_AUDIO_FORMAT;

typedef enum
{
	FDI_AUDIO_RUNNING_STATE_CHANGED = 0,
	FDI_AUDIO_FORMAT_CHANGED,
	FDI_AUDIO_MEMORY_STREAM_STARTED,
	FDI_AUDIO_MEMORY_STREAM_COMPLETED,
	FDI_AUDIO_EVENT_NOTIFY_END
} FDI_AUDIO_NOTIFY_EVT;

typedef enum
{
	FDI_AUDIO_STATE_UNKNOWN = 0,
	FDI_AUDIO_STATE_DECODING,
	FDI_AUDIO_STATE_NODATA		// NO DATA evnet should be notified to MW
								// when there is no decoded frame for more than 2 seconds
} FDI_AUDIO_RUNNING_STATE;

typedef	struct
{
	unsigned long			ulDecId;
	FDI_AUDIO_NOTIFY_EVT	eEvent;
	void					*pvEventData;	// (DI_AUDIO_FORMAT_CHANGED_EVT_t *) or (DI_AUDIO_RUNNING_STATE_EVT_t *)
} FDI_AUDIO_CALLBACK_DATA_t;

typedef struct FDI_AUDIO_RUNNING_STATE_EVT_s {
	FDI_AUDIO_RUNNING_STATE eState;
} FDI_AUDIO_RUNNING_STATE_EVT_t;

typedef FDI_ERR_CODE (*FDI_AUDIO_EventCallback)(FDI_AUDIO_NOTIFY_EVT eEvent, FDI_AUDIO_RUNNING_STATE eState);

typedef struct
{
	unsigned char ucLeft;
	unsigned char ucRight;
} FDI_AUDIO_RCAStatus;

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
FDI_ERR_CODE FDI_AUDIO_GetNumOfDevice(unsigned long *pulNumOfDevice);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_AUDIO_GetCapability();

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_AUDIO_Open(unsigned long ulDeviceId);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_AUDIO_Start(unsigned long ulDeviceId, FDI_AUDIO_FORMAT eAudioType);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_AUDIO_Stop(unsigned long ulDeviceId);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_AUDIO_SetMute(unsigned long ulDeviceId, BOOL bMute);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_AUDIO_SetHWtMute(BOOL bMute);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_AUDIO_SetBeepSound(unsigned long ulDeviceId, BOOL bOnOff);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_AUDIO_SetVolume(unsigned long ulDeviceId, unsigned long ulVolume);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_AUDIO_RegisterEventCallback(unsigned long ulDeviceId, FDI_AUDIO_EventCallback pfnCallback);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_AUDIO_CheckRCAJack(unsigned long ulOutputPortId, FDI_AUDIO_RCAStatus *tStatus);

#endif

