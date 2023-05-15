/**************************************************************************************
 * @file netflixmanager_int.h
 * defines NetflixManager(nxManager) internal header
 *
 *
 * @author
 * @date
 * @attention   Copyright (c) 2015-2016 Humax - All rights reserved.
 *
 **************************************************************************************/
 
#ifndef __NXMANAGER_INT_H__
#define __NXMANAGER_INT_H__

#include <stdint.h>

/******************************************************************
	Define
******************************************************************/
#ifndef ENUM_TO_STR
#define ENUM_TO_STR(enumVal) 			case enumVal: return #enumVal;
#endif

#ifndef VALUE_TO_STR
#define VALUE_TO_STR(x)					#x
#endif

#ifndef NOT_USED_PARAM
#define NOT_USED_PARAM(x)				(void)(x)
#endif

/******************************************************************
	Enum
 ******************************************************************/
typedef enum {
    NFLIX_NOTIFY_PLAYBACK_NA = 0x0,   /* eNXL_NOTIFY_NETFLIXPB_NA       */
    NFLIX_NOTIFY_PLAYBACK_STOPPED,    /* eNXL_NOTIFY_NETFLIXPB_STOPPED  */
    NFLIX_NOTIFY_PLAYBACK_STOPPING,   /* eNXL_NOTIFY_NETFLIXPB_STOPPING */
    NFLIX_NOTIFY_PLAYBACK_STARTING,   /* eNXL_NOTIFY_NETFLIXPB_STARTING */
    NFLIX_NOTIFY_PLAYBACK_PAUSED,     /* eNXL_NOTIFY_NETFLIXPB_PAUSED   */
    NFLIX_NOTIFY_PLAYBACK_PLAYING,    /* eNXL_NOTIFY_NETFLIXPB_PLAYING  */
    NFLIX_NOTIFY_PLAYBACK_MAX         /* eNXL_NOTIFY_NETFLIXPB_MAX      */
}NFLIX_NotifyPlaybackState_t;         /* NXL_NotifyNetflixPBState_e     */


/******************************************************************
	Typedef
******************************************************************/


/******************************************************************
	Interfaces
******************************************************************/
#ifdef  __cplusplus
extern "C" {
#endif


#ifdef  __cplusplus
}
#endif

#endif /* __NXMANAGER_INT_H__ */
