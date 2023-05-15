/* $Header: $ */

#ifndef DLIB_SWUPDATE_H__
#define DLIB_SWUPDATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>


// apk_swupdate.c 에서 사용한다..?
typedef enum
{
	eDxSWUPDATE_MAX_STEP						= 4,
	eDxSWUPDATE_MAX_URL_LENGTH				= 1024,
	eDxSWUPDATE_MAX_PATH_LENGTH				= 1024,
	eDxSWUPDATE_MAX_PIDLIST_NUM				= 9,
	eDxSWUPDATE_MAX_CAROUSEL_NUM			= 9,
	eDxSWUPDATE_MAX_ES_STREAM_NUM			= 10,
	eDxSWUPDATE_MAX_ERROR_MSG_LENGTH		= 64,
	eDxSWUPDATE_MAX_STRING					= 128,
	eDxSWUPDATE_INVALID_ID					= 0xFFFFFFFF
} DxSwUpdate_MaxSize_e;

typedef enum
{
	eDxSWUPDATE_SOURCE_None	=	0,
	eDxSWUPDATE_SOURCE_Rf,
	eDxSWUPDATE_SOURCE_Ip,
	eDxSWUPDATE_SOURCE_File,
	eDxSWUPDATE_SOURCE_Usb
} DxSwUpdate_Source_e;


typedef enum
{
	eDxSwUpdate_SPEC_NONE			= 0,
	eDxSwUpdate_SPEC_ASTRA			= 1,
	eDxSwUpdate_SPEC_DVBSSU		= 2,
	eDxSwUpdate_SPEC_DTT			= 3,
	eDxSwUpdate_SPEC_SKYD			= 4,
	eDxSwUpdate_SPEC_SES			= 5,
	eDxSwUpdate_SPEC_IRDETO		= 6,
	eDxSwUpdate_SPEC_HUMAX		= 7,
	eDxSwUpdate_SPEC_FSATSSU		= 8,
	eDxSwUpdate_SPEC_UPC			= 9,
	eDxSwUpdate_SPEC_KDG			= 10
} DxSwUpdate_Spec_e;

typedef enum
{
	eDxSWUPDATE_STEP_Unknown	= 0,
	eDxSWUPDATE_STEP_Config,
	eDxSWUPDATE_STEP_Connect,
	eDxSWUPDATE_STEP_Detect,
	eDxSWUPDATE_STEP_Download,
	eDxSWUPDATE_STEP_Install
} DxSwUpdate_Step_e;

typedef enum
{
	eDxSWUPDATE_STATE_DETECT_FAIL			=	0,
	eDxSWUPDATE_STATE_DETECT_SUCCESSED,
	eDxSWUPDATE_STATE_DETECT_FAIL_LATEST_VERSION,	// Current installed SW is latest version. (Signalled SW version is same or lower than installed SW.)

	eDxSWUPDATE_STATE_DOWNLOAD_FAIL		=	0,
	eDxSWUPDATE_STATE_DOWNLOAD_SUCCESSED,
	eDxSWUPDATE_STATE_DOWNLOAD_PROGRESS,

	eDxSWUPDATE_STATE_INSTALL_FAIL		=	0,
	eDxSWUPDATE_STATE_INSTALL_SUCCESSED,
	eDxSWUPDATE_STATE_INSTALL_PROGRESS

}DxSwUpdate_State_e;


typedef enum
{
	eDxSWUPDATE_EVENT_ConnectSuccess = 0,
	eDxSWUPDATE_EVENT_ConnectFail,
	eDxSWUPDATE_EVENT_DetectSuccess,		// menu ota
	eDxSWUPDATE_EVENT_DetectSuccessInLive,	// live ota
	eDxSWUPDATE_EVENT_DetectFail,
	eDxSWUPDATE_EVENT_DownloadSuccess,
	eDxSWUPDATE_EVENT_DownloadFail,
	eDxSWUPDATE_EVENT_DownloadProgress,
	eDxSWUPDATE_EVENT_InstallSuccess,
	eDxSWUPDATE_EVENT_InstallFail,
	eDxSWUPDATE_EVENT_InstallProgress,
	eDxSWUPDATE_EVENT_Erorr,
	eDxSWUPDATE_EVENT_Finish
} DxSwUpdate_Event_e;

typedef enum
{
	eDxSWUPDATE_METHOD_MENU,
	eDxSWUPDATE_METHOD_LIVE
} DxSwUpdate_Method_e;

typedef enum
{
	eDxSWUPDATE_SIGNAL_SINGLE = 0,	// Single TP SW Update
	eDxSWUPDATE_SIGNAL_MULTI		// Multi TP SW Update
} DxSwUpdate_Signal_e;

// 이하 부분은 opl, bpl 에서 사용하는 구제체임..
typedef struct
{
	DxSwUpdate_Source_e eSource;
	HINT32		nState;
	HINT32		nProgress;
	HINT32		nProgressMax;
	HINT32		nErrMsg;
}  DxSwUpdate_Progress_t;

typedef struct
{
	HUINT8	ucCurrentVer[eDxSWUPDATE_MAX_STRING];
	HUINT8 	ucFoundver[eDxSWUPDATE_MAX_STRING];
} DxSwUpdate_DetectResult_t;

typedef struct
{
	HUINT32		ulFrequency;
	HUINT32		ulSymbolRate;
	HINT32		nPolarization;
	HINT32		nTransSpec;
	HINT32		nPskMod;
	HINT32		nFecCodeRate;
} DxSwUpdate_DetectResultTuning_t;

#if 0
typedef struct
{
	DxSwUpdate_Source_e	eSource;
	HINT32				nState;
	HINT32				nErrMsg;
	HINT32				nSignalType;	// DxSwUpdate_Signal_e
	union
	{
		DxSwUpdate_DetectResult_t		stDetectInform;
		DxSwUpdate_DetectResultTuning_t stDetectTunningInform;
	} unInform;
} DxSwUpdate_Result_t;

typedef struct
{
	DxSwUpdate_Event_e	eEvent;
	union
	{
		DxSwUpdate_Result_t		stResult;
		DxSwUpdate_Progress_t		stProgress;
	}unEventData;

}  DxSwUpdate_EventData_t;
#endif

/**************** new s/w update event ****************/
typedef struct
{
	HCHAR	szCurrentVer[eDxSWUPDATE_MAX_STRING];
	HCHAR	szFoundVer[eDxSWUPDATE_MAX_STRING];

} DxSwUpdate_DetectEvent_t;

typedef struct
{
	HINT32		nProgress;
	HINT32		nProgressMax;
}  DxSwUpdate_ProgressEvent_t;

typedef struct
{
	HUINT8	ucSwupType;		// Forced, Normal

	HUINT32	ulStartTime;
	HUINT32	ulEndTime;
}	DxSwUpdate_ExSwupTypeData_t;

typedef struct
{
	HUINT8 ucLoadSeqNumber;
	HUINT8 ucTableId;
	HINT32 nDnTimeInSec;
}	DxSwUpdate_ExSwupIrdetoData_t;

typedef struct
{
	DxSwUpdate_Event_e		eEvent;
	DxSwUpdate_Source_e		eSourceType;
	DxSwUpdate_Signal_e		eSignalType;
	DxSwUpdate_Method_e		eMethodType;
	DxSwUpdate_State_e		eState;
	union
	{
		DxSwUpdate_DetectEvent_t	stDetectEvent;
		DxSwUpdate_ProgressEvent_t	stProgressEvent;
	}unEventData;

	union
	{
		DxSwUpdate_ExSwupTypeData_t	stExSwupTypeData;
		DxSwUpdate_ExSwupIrdetoData_t stExSwupIrdetoData;
	}unExEventData;


} DxSwUpdate_EventData_t;

/************** new s/w update event end **************/

#ifdef __cplusplus
}
#endif

#endif // DLIB_SWUPDATE_H__


