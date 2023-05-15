/* $Header: $ */

#ifndef _DLIB_DSMCC_H__
#define _DLIB_DSMCC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>

typedef enum
{
	eDxDSMCC_MAX_STEP						= 4,
	eDxDSMCC_MAX_URL_LENGTH				= 1024,
	eDxDSMCC_MAX_PATH_LENGTH				= 1024,
	eDxDSMCC_MAX_PIDLIST_NUM				= 9,		// Max Object Carousel * Max PID Num per Object Carousel
	eDxDSMCC_MAX_CAROUSEL_NUM			= 9,
	eDxDSMCC_MAX_ES_STREAM_NUM			= 10,
	eDxDSMCC_MAX_ERROR_MSG_LENGTH		= 64,
	eDxDSMCC_INVALID_ID					= 0xFFFFFFFF,
} DxDSMCC_MaxSize_e;

typedef enum
{
	eDxDSMCC_STEP_Unknown	= 0,
	eDxDSMCC_STEP_Config,
	eDxDSMCC_STEP_Connect,
	eDxDSMCC_STEP_Download,
	eDxDSMCC_STEP_Install,
} DxDSMCC_Step_e;

typedef enum
{
	eDxDSMCC_TYPE_Unknown	= 0,
	eDxDSMCC_TYPE_HBBTV
} DxDSMCC_Type_e;

typedef enum
{
	eDxDSMCC_EVENT_Created = 0,
	eDxDSMCC_EVENT_Started,
	eDxDSMCC_EVENT_Stopped,
	eDxDSMCC_EVENT_Progress,
	eDxDSMCC_EVENT_Changed,
	eDxDSMCC_EVENT_Fail,
	eDxDSMCC_EVENT_Success,
	eDxDSMCC_EVENT_StreamEvtFound,
	eDxDSMCC_EVENT_StreamDescriptorEvt,
	eDxDSMCC_EVENT_StreamEvtList,
	eDxDSMCC_EVENT_CarouselList,
	eDxDSMCC_EVENT_Ait,
	eDxDSMCC_EVENT_GeneralError,
	eDxDSMCC_EVENT_Unknown
} DxDSMCC_Event_e;

typedef struct
{
	HCHAR	 			*pszCachePath;
} DxDSMCC_ModuleInfo_t;

typedef struct
{
	HUINT32 			ulParam1;
	HUINT32 			ulParam2;
	HUINT32 			ulParam3;
} DxDSMCC_DownloadEvent_t;

typedef enum
{
	eDxDSMCC_EventData_None					= 0x00000000,
	eDxDSMCC_EventData_Url						= 0x00000001,
	eDxDSMCC_EventData_EventName				= 0x00000002,
	eDxDSMCC_EventData_Description				= 0x00000004,
	eDxDSMCC_EventData_ObjectInfo				= 0x00000008,
	eDxDSMCC_EventData_All						= 0xFFFFFFFF,
} DxDSMCC_EventDataFlag_e;

typedef struct
{
	HUINT16				 usPid;
	HCHAR				*pszUrl;

	// Description
	HUINT32				 ulDescLen;
	HUINT8				*szDescription;

	// Event Name
	HUINT8				*szEventName;

	// Object Info Byte
	HUINT32				 ulObjInfoByteLen;
	HUINT8				*pucObjInfoByte;

	// Event ID
	HINT32				nEventId;

	// Association TAG
	HUINT16				 usTag;
} DxDSMCC_StreamEvent_t;

typedef struct
{
	HxList_t				*pstEventList;		// DxDSMCC_StreamEvent_t
} DxDSMCC_StreamEventList_t;

typedef struct
{
	HUINT8				ucComponentTag;
	HUINT8				ucStreamType;
} DxDSMCC_StreamInfo_t;

typedef struct
{
	HUINT32					ulCarouselId;
	HUINT32					ulNum;
	DxDSMCC_StreamInfo_t		astEsInfo[eDxDSMCC_MAX_ES_STREAM_NUM];
} DxDSMCC_CarouselInfo_t;

typedef struct
{
	HUINT32					ulNum;
	DxDSMCC_CarouselInfo_t	*pstCarouselInfo;
} DxDSMCC_CarouselList_t;

typedef struct
{
	HCHAR					szMessage[eDxDSMCC_MAX_ERROR_MSG_LENGTH];
	HUINT32					ulCode;
} DxDSMCC_ErrorInfo_t;

typedef union
{
	DxDSMCC_ModuleInfo_t				stModuleInfo;
	DxDSMCC_DownloadEvent_t			stDownload;
	DxDSMCC_StreamEventList_t			stStreamEvent;
	DxDSMCC_CarouselList_t			stCarousel;
	DxDSMCC_ErrorInfo_t				stError;
} DxDSMCC_EventParam_t;

typedef struct
{
	Handle_t					hDsmcc;
	DxDSMCC_Event_e			eEvent;
	DxDSMCC_EventParam_t		unParam;
}DxDSMCC_Event_Info_t;

typedef void (*APK_DL_DSMCC_Listener_t)(DxDSMCC_Event_e eEvent, void *data);

#define	DEFAULT_CAROURSEL_ID	0xFFFFFFFE

#ifdef __cplusplus
}
#endif

#endif // _DLIB_DSMCC_H__


