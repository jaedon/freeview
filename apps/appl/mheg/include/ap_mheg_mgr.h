#ifndef _AP_MHEG_MGR_H_
#define _AP_MHEG_MGR_H_

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <mgr_common.h>

#if defined (CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE) || defined (CONFIG_PROD_HDR1000S) || defined (CONFIG_PROD_HB1000S)
#define USE_KEY_PROCESS_BY_HAMA
#endif

#if 0
#define MHEG_UI_ORDER_BY_RUNNING_STATUS	1
#endif

/* mesasge types for Display */
enum
{
	eMEVT_MHEG_DISPLAY_AVSTREAM_PLAY = eMEVT_MHEG_START,
	eMEVT_MHEG_DISPLAY_AVSTREAM_STOP,
	eMEVT_MHEG_DISPLAY_SUBTITLE_CONTROL,
	eMEVT_MHEG_DISPLAY_SUBTITLE_CHANGED,

	eMEVT_MHEG_DISPLAY_AVSTREAM_SELECT_AUDIO,
	eMEVT_MHEG_DISPLAY_AVSTREAM_SELECT_VIDEO,
	eMEVT_MHEG_DISPLAY_AVSTREAM_DESELECT_AUDIO,
	eMEVT_MHEG_DISPLAY_AVSTREAM_DESELECT_VIDEO,
	eMEVT_MHEG_DISPLAY_AVSTREAM_RELEASE,
	eMEVT_MHEG_DISPLAY_AVSTREAM_PRELOAD,
	eMEVT_MHEG_DISPLAY_AVSTREAM_SET_SPEED,

	eMEVT_MHEG_DISPLAY_IMAGE_PLAY,
	eMEVT_MHEG_DISPLAY_IMAGE_STOP,
	eMEVT_MHEG_DISPLAY_IMAGE_RELEASE,

	eMEVT_MHEG_DISPLAY_AUDIOCLIP_PLAY,
	eMEVT_MHEG_DISPLAY_AUDIOCLIP_STOP,
	eMEVT_MHEG_DISPLAY_AUDIOCLIP_PAUSE,
	eMEVT_MHEG_DISPLAY_AUDIOCLIP_RELEASE,

	eMEVT_MHEG_DISPLAY_AVSTREAM_SET_TRIGGER,		// for ip streaming
	eMEVT_MHEG_DISPLAY_AVSTREAM_CLEAR_TRIGGER,	// for ip streaming
	eMEVT_MHEG_DISPLAY_AVSTREAM_SET_POSITION,		// for ip streaming
	eMEVT_MHEG_DISPLAY_AVSTREAM_SET_END_POSITION,	// for ip streaming

	eMEVT_MHEG_DISPLAY_KEYDOWN,						// for MHEG porting layer (Key process task)

	eMEVT_MHEG_SITABLE_NOTIFY,
	eMEVT_MHEG_SVC_RETUNE,
	eMEVT_MHEG_SVC_SET_BROADCAST_INTERRUPTION,
	eMEVT_MHEG_START_REDKEY_RETRY,
#if defined (MHEG_UI_ORDER_BY_RUNNING_STATUS)
	eMEVT_MHEG_STATUS_NOTIFY,
#endif
	eMEVT_MHEG_NVM_INITIALIZE,
	eMEVT_MHEG_NVM_INITIALIZE_RETRY,
	eMEVT_MHEG_NVM_INITIALIZE_SUCCESS,
	eMEVT_MHEG_REQUEST_VERSION,
	eMEVT_MHEG_VERSION_INFO,
	eMEVT_MHEG_ENGINE_START,
	eMEVT_MHEG_ENGINE_STOP,
	eMEVT_MHEG_DO_FACTORY_RESET,
	eMEVT_MHEG_RESTART_MHEG_APPLICATION,				// for exit key (freesat scenario)
	eOEVT_MHEG_KEYDOWN,								// for appkit (not using)

	/* - Description : Request file to MHEG DSMCCFS.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : Fully qualified file name. (ex: "DSM://...")
 		   p2 : N.A.
		   p3 : N.A.						*/
	eMEVT_MHEG_DSMCCFS_REQUEST_FILE,

	/* - Description : Request release DSMCCFS file handle.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : DsmccFs File Handle.
 		   p2 : N.A.
		   p3 : N.A.						*/
	eMEVT_MHEG_DSMCCFS_RELEASE_FILE,

	/* - Description : DSMCCFS file data.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : DsmccFs File Handle.	(NULL if request failed)
 		   p2 : File Data		(NULL if loading failed)
		   p3 : File Size		(NULL if loading failed)
	   - If both p2 and p2 are null but p1 is not null, p1 need to be released.
	*/
	eMEVT_MHEG_DSMCCFS_FILE_DATA,

	eMEVT_MHEG_MISC_PROMPT_FOR_GUIDANCE,
	eMEVT_MHEG_MISC_LAUNCH_APPLICATION,
	eMEVT_MHEG_MISC_FLIP_WINDOW,

	eMEVT_MHEG_HTTP_REGISTER_CALLBACK,

	eMEVT_MHEG_TYPE_LAUNCH_APPLICATION,
	eMEVT_MHEG_TYPE_FLIP_WINDOW,
	eMEVT_MHEG_TYPE_MHEG_APPSTATE,

	eMEVT_ITK_DISPLAY_END
};

extern	HERROR MHEG_Streaming_SI_GetAudioInfo(Handle_t hAction, HxLANG_Id_e eLangIdx, HUINT32 ulPid, HUINT16 *usAudiaIdx);

#if defined(CONFIG_3RD_MHEG_USE_SNT_ENGINE)
BUS_Result_t	xproc_mheg_Manager(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
HBOOL			xmgr_mheg_GetSubtitleSetting(void);
HUINT32			xmgr_mheg_GetTuneFlag(Handle_t hAction);
#endif

#endif

