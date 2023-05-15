/**
 * drv_multicast.h
*/

/**
 * @defgroup		DRV_MULTICAST DRV_MULTICAST : DRV_MULTICAST APIs
 * @author		Choi Hyun Seok	
 * @note		DRV_MULTICAST APIs
 * @brief		Define DRV_MULTICAST APIs
 * @file 		drv_multicast.h
*/

#ifndef __DRV_MULTICAST_H___
#define __DRV_MULTICAST_H___


/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "drv_err.h"
#include "di_video.h"
#include "di_audio.h"

/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/

void DRV_MULTICAST_Stop(void);
DRV_Error DRV_MULTICAST_Start(HUINT32 type, HUINT32 vPid, HUINT32 aPid, DI_VIDEO_FORMAT eVideoCodec, DI_AUDIO_FORMAT eAudioCodec);
DRV_Error DRV_MULTICAST_GetStatus(void);
#endif /* !__DRV_MULTICAST_H__ */

