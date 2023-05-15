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
#include <fcntl.h>

/* chip manufacture's  header files */

/* humax header files */
#include "htype.h"

#include "fvk.h"
#include "fdi_common.h"
#include "fdi_uart.h"
#include "fdi_system.h"

#include "di_err.h"
#if !defined(FACTORY_USE_DI20)
#include "di_factory.h"
#endif
#include "vkernel.h"
#include "fdi_audio.h"
#include "fdi_video.h"
#include "drv_multicast.h"

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

/*******************************************************************************
* function : FDI_IPSTREAM_Init
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_IPSTREAM_Init(void)
{
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_IPSTREAM_Open
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_IPSTREAM_Open(void)
{
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_IPSTREAM_Close
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_IPSTREAM_Close(void)
{
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_IPSTREAM_Start
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_IPSTREAM_Start(HUINT32 type, HUINT32 videoPid, HUINT32 audioPid, FDI_VIDEO_FORMAT eVideoCodec, FDI_AUDIO_FORMAT eAudioCodec)
{
	DI_ERR_CODE eDiErr=DI_ERR_OK;
	DI_VIDEO_FORMAT eDiVideoCodec;
	DI_AUDIO_FORMAT eDiAudioCodec;

	switch(eVideoCodec)
	{
		case FDI_VIDEO_FORMAT_MPEG2 :
			eDiVideoCodec = DI_VIDEO_FORMAT_MPEG2;
			break;
		case FDI_VIDEO_FORMAT_H264 :
			eDiVideoCodec = DI_VIDEO_FORMAT_H264;
			break;
		default :
			eDiVideoCodec = DI_VIDEO_FORMAT_MPEG2;
			break;
	}

	switch(eAudioCodec)
	{
		case FDI_AUDIO_FORMAT_MPEG :
			eDiAudioCodec = DI_AUDFORMAT_MPEG;
			break;
		case FDI_AUDIO_FORMAT_AC3 :
			eDiAudioCodec = DI_AUDFORMAT_AC3;
			break;
		default :
			eDiAudioCodec = DI_AUDFORMAT_MPEG;
			break;
	}		

	eDiErr=DRV_MULTICAST_Start(type, videoPid, audioPid, eDiVideoCodec, eDiAudioCodec);
	if( eDiErr != DI_ERR_OK )
	{
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_IPSTREAM_Stop()
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_IPSTREAM_Stop()
{
	DRV_MULTICAST_Stop();
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_IPSTREAM_GetLockStatus()
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_IPSTREAM_GetLockStatus()
{
	DI_ERR_CODE eDiErr=DI_ERR_OK;
	eDiErr=DRV_MULTICAST_GetStatus();
	if(eDiErr != DI_ERR_OK)
		return FDI_ERR;
	else
		return FDI_NO_ERR;
}
