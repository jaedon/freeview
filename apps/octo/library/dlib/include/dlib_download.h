/* $Header: $ */

#ifndef DLIB_DOWNLOAD_H__
#define DLIB_DOWNLOAD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define DxDNLD_MEDIAPLAY_URL					(2048)



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eDxDNLD_TYPE_NONE				= 0,
	eDxDNLD_TYPE_IpMediaStream,
	eDxDNLD_TYPE_EndOfCase
} DxDownload_Type_e;

typedef enum
{
	eDxDNLD_EVENT_None,						//	None

	eDxDNLD_EVENT_Started,
	eDxDNLD_EVENT_Completed,
	eDxDNLD_EVENT_Failed,
	eDxDNLD_EVENT_DataReceiving,
	eDxDNLD_EVENT_ProbeSuccess,
	eDxDNLD_EVENT_ProbeFailed,
	eDxDNLD_EVENT_DownloadInfoChanged,

	/* CONTROL */
	eDxDNLD_EVENT_COMPONENT_VIDEO_CHANGED,	//	eSEVT_SI_PMT, eSEVT_SI_EIT
	eDxDNLD_EVENT_COMPONENT_AUDIO_CHANGED,	//	eSEVT_SI_PMT, eSEVT_SI_EIT
	eDxDNLD_EVENT_COMPONENT_SUBTITLE_CHANGED, //	eSEVT_SI_PMT
	eDxDNLD_EVENT_COMPONENT_DATA_CHANGED,	//	eSEVT_SI_PMT
	eDxDNLD_EVENT_COMPONENT_SELECTED,		//	Component Changed

	eDxDNLD_EVENT_EndofCase
} DxDownload_Event_e;

typedef struct
{
	HUINT32					ulSessionId;
#if defined(CONFIG_TIMESTAMP_64)
	HUINT64					ullDuration;
	HUINT64					ullStartPos;
#else
	HUINT32 				ulDuration;
	HUINT32 				ulStartPos;
#endif


	HUINT32					ulMaxBitrate;
	HBOOL					bInfinity;

	HUINT32					ulErrorType;
	HUINT32					ulErrorReason;
} DxDownload_SessionInfo_t;

typedef struct
{
	DxDownload_Type_e		 eType;
	HBOOL					 bContentVerification;
	HBOOL					 bMhegIcsStream; // is it MHEG ICS stream? 
	HCHAR					 szUrl[DxDNLD_MEDIAPLAY_URL];
} DxDownload_StartIpMedia_t;

typedef union
{
	DxDownload_Type_e			 eType;
	DxDownload_StartIpMedia_t	 stIpMedia;
} DxDownload_Start_t;

typedef struct
{
	HUINT64						 ullBufferedBytes;
	HUINT64						 ullTotalBytesRemaining;
	HUINT64						 ullTotalStreamBytes;
	HINT32						 nArrivalBytedPerSec;
	HINT32						 nStreamBytesPerSec;
	HINT32						 nBufferPercentage;
	HINT32						 nRemainBufferSec;
} DxDownload_Status_t;

// Notifier Data
typedef union
{
	DxDownload_SessionInfo_t	stDnldSessionInfo;
	DxDownload_Start_t			stDnldStarted;
	DxDownload_Status_t			stDnldStatus;
} DxDownload_NotifierData_t;

#ifdef __cplusplus
}
#endif

#endif // DLIB_DOWNLOAD_H__

/* end of file */

