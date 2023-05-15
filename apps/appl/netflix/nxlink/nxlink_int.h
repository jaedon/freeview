/**************************************************************************************
 * @file nxlink.h
 * defines APIs's internal header
 *
 * NXLINK (NetfliX external LINK API for OCTO interface)
 *
 * @author
 * @date
 * @attention   Copyright (c) 2015-2016 Humax - All rights reserved.
 *
 **************************************************************************************/
 
#ifndef __NXLINK_INT_H__
#define __NXLINK_INT_H__

#include <stdint.h>
#include <nxlink.h>
#include <assert.h>

/******************************************************************
	Define
******************************************************************/
#ifdef	CONFIG_DEBUG
#if defined(__NXLINK_INTIPC_APKV1__)
    #define NXL_LOG_DBG(...)            HxLog_Print(__VA_ARGS__)
    #define NXL_LOG_WRN(...)            HxLog_Warning(__VA_ARGS__)
    #define NXL_LOG_ERR(...)            HxLog_Error(__VA_ARGS__)
    #define NXL_Assert(x)               HxLog_Assert(x)
#elif defined(__NXLINK_INTIPC_APKV2__) || defined(__NXLINK_INTIPC_PROAPI__)
    #define NXL_LOG_DBG                 HxLOG_Print
    #define NXL_LOG_WRN                 HxLOG_Warning
    #define NXL_LOG_ERR                 HxLOG_Error
    #define NXL_Assert(x)               HxLOG_Assert(x)
#else
    #define NXL_LOG_DBG(...)            do { printf(__VA_ARGS__); } while(0)
    #define NXL_LOG_WRN(...)            do { printf(__VA_ARGS__); } while(0)
    #define NXL_LOG_ERR(...)            do { printf(__VA_ARGS__); } while(0)
    #define NXL_Assert(x)               assert(x)
#endif
#else
    #define NXL_LOG_DBG(...)            while (0) ((int (*)(const char *, ...)) 0)
    #define NXL_LOG_WRN(...)            while (0) ((int (*)(const char *, ...)) 0)
    #define NXL_LOG_ERR(...)            while (0) ((int (*)(const char *, ...)) 0)
    #define NXL_Assert(x)               assert(x)
#endif

#ifndef _NAMEOF
#define	_NAMEOF(type)                   #type
#endif

#ifndef _ENUM_TO_STR
#define _ENUM_TO_STR(enumVal)           case enumVal: return #enumVal;
#endif

#ifndef _VALUE_TO_STR
#define _VALUE_TO_STR(x)                #x
#endif

#ifndef _BOOL_TO_STR
#define	_BOOL_TO_STR(b)                 ((b == TRUE) ? "TRUE" : "FALSE")
#endif

#ifndef _NOT_USED_PARAM
#define _NOT_USED_PARAM(x)              (void)(x)
#endif

#define	_NXL_BLEN_MAX                   (512)
#define _NXL_BLEN_128                   (128)
#define _NXL_BLEN_64                    (64)
#define _NXL_BLEN_32                    (32)

#define __CHECK_PARAM_PTR(a)            { if (a == NULL) {NXL_LOG_ERR("Error> NULL params [%s:%d]\n", __FUNCTION__, __LINE__); return NXL_ERR_INVALID_PARAM;}}
#define	__CHECK_NULL_PTR(a)             { if (a == NULL) {NXL_LOG_ERR("Error> argument is NULL at [%s:%d]\n", __FUNCTION__, __LINE__); return NXL_ERR_INVALID_PARAM;} }
#define	__CHECK_VALID_INFO(a)           { if (a == NULL) {NXL_LOG_ERR("Error> argument is NULL at [%s:%d]\n", __FUNCTION__, __LINE__); return NXL_ERR_INVALID_PARAM;} }
#define __CHECK_MODULE_INIT             { if (!_nxl_module_init) {NXL_LOG_ERR("Error> Module is not Initializing...\n"); return NXL_ERR_UNINITIALIZED;} }
#define __CHECK_MODULE_INIT_NOPARAM     { if (!_nxl_module_init) {NXL_LOG_ERR("Error> Module is not Initializing...\n"); return;} }
#define	__CHECK_MODULE_DEINIT           { if (!_nxl_module_init) {NXL_LOG_ERR("Error> Module is not Initializing...\n"); return NXL_ERR_UNINITIALIZED;} }


/******************************************************************
	Enum
 ******************************************************************/
typedef enum tTaskStatus
{
	eNXL_TASK_STATUS_TERMINATED=0,  /* Task terminated */
	eNXL_TASK_STATUS_HALTING,       /* Prepare task halt */
	eNXL_TASK_STATUS_READY,         /* Ready status with looping */
	eNXL_TASK_STATUS_RUNNING,       /* Looping after oapi task start */
}NXL_TaskStatus_e;

typedef enum {
    eNXL_NOTIFY_NETFLIXPB_NA=0x0,       /* NFLIX_NOTIFY_PLAYBACK_NA       */
    eNXL_NOTIFY_NETFLIXPB_STOPPED,      /* NFLIX_NOTIFY_PLAYBACK_STOPPED  */
    eNXL_NOTIFY_NETFLIXPB_STOPPING,     /* NFLIX_NOTIFY_PLAYBACK_STOPPING */
    eNXL_NOTIFY_NETFLIXPB_STARTING,     /* NFLIX_NOTIFY_PLAYBACK_STARTING */
    eNXL_NOTIFY_NETFLIXPB_PAUSED,       /* NFLIX_NOTIFY_PLAYBACK_PAUSED   */
    eNXL_NOTIFY_NETFLIXPB_PLAYING,      /* NFLIX_NOTIFY_PLAYBACK_PLAYING  */
    eNXL_NOTIFY_NETFLIXPB_MAX           /* NFLIX_NOTIFY_PLAYBACK_MAX      */
}NXL_NotifyNetflixPBState_e;            /* NFLIX_NotifyPlaybackState_t    */

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

#endif /* __NXLINK_INT_H__ */
