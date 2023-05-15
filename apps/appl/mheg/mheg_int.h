#ifndef __MHEG_INT_H__
#define __MHEG_INT_H__

/*************************************************************
 * @file		mheg_int.h
 * @date		2013/11/16
 * @brief		Humax SnT Mheg Player
 *
 * http://www.humaxdigital.com
 *************************************************************/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	MHEG_NOT_IMPLEMENTED		do { HxLOG_Critical("[%s:%d] Not implemented...\n", __FUNCTION__, __LINE__); HxLOG_Assert(0); } while (0)

#define	MHEG_CODEC_CURRENT			0x1111

/*
* Planes
*/
#define MHEG_DISPLAY_PLANE_STILL1		0	// 별도 Still Plane
#define MHEG_DISPLAY_PLANE_STILL2		1	// 공유 Still Plane
#define MHEG_DISPLAY_PLANE_VIDEO		2	// Video Plane
#define MHEG_DISPLAY_PLANE_STILL_MAX	2
#define MHEG_DISPLAY_PLANE_VIDEO_ALL	3

#define MHEG_TASK_PRIORITY					VK_TASK_PRIORITY_MW_REF
#define MHEG_RCV_TASK_PRIORITY				VK_TASK_PRIORITY_MW_REF
#define MHEG_SI_MONITOR_TASK_PRIORITY		VK_TASK_PRIORITY_MW_REF
#define MHEG_RCV_TASK_STACK_SIZE			SIZE_64K
#define MHEG_SI_TASK_STACK_SIZE				SIZE_32K

typedef enum
{
	MHEG_ENGINE_PROFILE_NONE		= 0x0000,
	MHEG_ENGINE_PROFILE_UK			= 0x0001,
	MHEG_ENGINE_PROFILE_CI_PLUS		= 0x0002,
	MHEG_ENGINE_PROFILE_NZ			= 0x0004,
	MHEG_ENGINE_PROFILE_HK			= 0x0008,
	MHEG_ENGINE_PROFILE_UK_FSAT		= 0x0010,
	MHEG_ENGINE_PROFILE_AUSTRALIA	= 0x0020
} MHEG_EngineProfile_e;

typedef enum
{
	eITK_REFSVC_DVB = 0,		/* DVB Format : dvb://{onid}.{tsid}.{sid} */
	eITK_REFSVC_DEF,			/* DEF Format : rec://svc/def */
	eITK_REFSVC_CUR,			/* CUR Format : rec://svc/cur */
	eITK_REFSVC_LCN,			/* LCN Format : rec://svc/lcn/{lcn} */
	eITK_REFSVC_FSI,			/* FSI Format : rec://svc/fsi/{fsi} */
	eITK_REFSVC_PREV,			/* Previous Format : rec://svc/prev */
	eITK_REFSVC_PVR,			/* PVR Format : PVR:// */
	eITK_REFSVC_HTTP,			/* HTTP Format : http:// */
	eITK_REFSVC_RTSP,			/* RTSP Format : rtsp:// */
	eITK_REFSVC_UNKNOWN
} ITK_RefSvc_t;

typedef enum
{
	eMHEG_ENGINE_START				= 0x01000000,
	eMHEG_NOTIFY_STATUS				= 0x10000000,
	eMHEG_NOTIFY_APPSTATUS,
	eMHEG_RESTART_APPLICATION,

	eMHEG_DISPLAY_SUBTITLE_CONTROL,
	eMHEG_DISPLAY_IMAGE_RELEASE,
	eMHEG_DISPLAY_IMAGE_SHOW,
	eMHEG_DISPLAY_IMAGE_HIDE,
	eMHEG_HTTP_REGISTER_CALLBACK,

	eMHEG_AVSTREAM_PRELOAD,
	eMHEG_AVSTREAM_PLAY,
	eMHEG_AVSTREAM_STOP,
	eMHEG_AVSTREAM_RELEASE,
	eMHEG_AVSTREAM_SELECT_AUDIO,
	eMHEG_AVSTREAM_SELECT_VIDEO,
	eMHEG_AVSTREAM_DESELECT_AUDIO,
	eMHEG_AVSTREAM_DESELECT_VIDEO,
	eMHEG_AVSTREAM_SET_SPEED,
	eMHEG_AVSTREAM_SET_TRIGGER,
	eMHEG_AVSTREAM_CLEAR_TRIGGER,
	eMHEG_AVSTREAM_SET_POSITION,
	eMHEG_AVSTREAM_SET_END_POSITION,
	eMHEG_AVSTREAM_DOWNLOAD_START,
	eMHEG_AVSTREAM_DOWNLOAD_COMPLETE,
	eMHEG_AVSTREAM_DOWNLOAD_FAIL,
	eMHEG_AVSTREAM_DOWNLOAD_PROGRESS,
	eMHEG_AVSTREAM_DOWNLOAD_PROBE_SUCCESS,
	eMHEG_AVSTREAM_DOWNLOAD_PROBE_FAILED,
	eMHEG_AVSTREAM_DOWNLOAD_INFO_CHANGED,

	eMHEG_NETWORK_EVENT,

	eMHEG_DEMUX_SITABLE_NOTIFY,

	eMHEG_MISC_PROMPT_FOR_GUIDANCE,
	eMHEG_MISC_LAUNCH_APPLICATION,

	eMHEG_DSMCCFS_FILE_DATA,

	eMHEG_SERVICE_CHANGE_SVC,

	eMHEG_APK_SVCCHANGED,
	eMHEG_APK_SVCSTOPPED,
	eMHEG_APK_AVSTARTED,
	eMHEG_APK_AVSTOPPED,
	eMHEG_APK_AVRESUME,
	eMHEG_APK_AVUNDERRUN,
	eMHEG_APK_MONITORSTARTED,
	eMHEG_APK_MONITORSTOPPED,
	eMHEG_APK_DMXCHANGED,
	eMHEG_APK_PMTCHANGED,
	eMHEG_APK_NOSIGNAL,
	eMHEG_APK_LOCKED,
	eMHEG_APK_SUBTITLECHANGED,
	eMHEG_APK_VIDEO_STATUS,
	eMHEG_APK_PFG_RESULT,
	eMHEG_APK_SET_BROADCAST_INTERRUPTION,
	eMHEG_APK_DSMCCFS_REQUEST_FILE,
	eMHEG_APK_AUDIOCLIPCOMPLETED,

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	eMHEG_APK_MEDIA_EVENT_STARTED,
	eMHEG_APK_MEDIA_EVENT_PLAYTIME,
	eMHEG_APK_MEDIA_EVENT_PLAYSTATE,
	eMHEG_APK_MEDIA_EVENT_PLAYERROR,
	eMHEG_APK_MEDIA_EVENT_MEDIA_BufferStateChanged,
	eMHEG_APK_MEDIA_EVENT_MEDIA_BufferInfoChanged,
	eMHEG_APK_MEDIA_EVENT_VIDEOSTREAMINFO,
	eMHEG_APK_MEDIA_EVENT_STOPPED,
	eMHEG_APK_MEDIA_EVENT_SPEED_CHANGED,
	eMHEG_APK_MEDIA_EVENT_SupportedSpeedChanged,
	eMHEG_APK_MEDIA_EVENT_PlayInfo,
	eMHEG_APK_MEDIA_EVENT_MEDIA_PumpIndexChanged,
	eMHEG_APK_MEDIA_EVENT_MEDIA_SeekReady,
#endif
	eMHEG_APK_FACTORY_DEFAULT,

	eMHEG_APK_SVCLISTUPDATED,

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	eMHEG_SI_IP_STREAMING_PMT,
#endif

	eMHEG_KEY_DOWN,

	eMHEG_ENGINE_START_RETRY,

	eMHEG_APK_RCTCHANGED,
	eMHEG_SI_PMTCHANGED,
	eMHEG_NOTIFY_APPSTATUS_CHANGED,

	eMHEG_UNKNOWN
} MHEG_MSG_e;

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef HINT32	(*PostMsgCB_t)(HINT32 nMsg, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4, void *pData, HUINT32 ulDataSize);

extern HINT32		MHEG_SendMessage(HINT32 nMsg, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4);

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#endif

