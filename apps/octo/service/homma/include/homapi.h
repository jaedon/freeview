/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  homapi.h
	@brief

	Description:  									\n
	Module: HOMMA / API								\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#ifndef __HOMAPI_H__
#define __HOMAPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#define	________________Header_Files_______________________________
#include <hlib.h>

#define	________________Define_______________________________
#define UDN_LEN                 ( 36 )          /* UDN?? 보통 8-4-4-4-12???????36????????? */
#define UDN_MAX                 ( 48 )          /* Prefix????????경우?????? Length */

#define URL_MAX                 1024
#define AIRPLAY_URL_MAX			(2048)			/* AIRPLAY url could be over than 1024 bytes because of YOUTUBE */
#define CID_SIZE                512
#define TITLE_SIZE              128
#define GENRE_SIZE              32
#define RES_SIZE                12
#define IP_SIZE                 20

#define	DTCP_PORT_SIZE			(6)
#define	DTCP_IPV4ADDR_SIZE		(16)

#define COOKIE_MAX				(4096)

#define HOMAPI_MEMFREE(x)      \
	    if(x)  { HLIB_STD_MemFree(x);  x = NULL;  }

#define HOMAPI_ASSERT(x)    \
	    if ( x == NULL )    \
	    {\
		 	HxLOG_Error("[ %s:%s:d ] : the param is NULL.\n", "HOMAPI", __func__, __LINE__);\
			return ERR_FAIL;\
	    }\

#define		HOMAPI_SEM_GET					HxSEMT_Get(_ulModuleSemaphoreId)
#define		HOMAPI_SEM_RELEASE				HxSEMT_Release(_ulModuleSemaphoreId)

#define ________________Typedef____________________________________
typedef enum
{
    eHOMAPI_Service_DMP = 0,
    eHOMAPI_Service_DMR,
    eHOMAPI_Service_DMS,
    eHOMAPI_Service_WoonServer,
    eHOMAPI_Service_WoonClient,
    eHOMAPI_Service_Dial,
    eHOMAPI_Service_RemoteUiServer,
	eHOMAPI_Service_AIRPLAY,
    eHOMAPI_Service_SatIpServer,
	eHOMAPI_Service_RC,
} HOMxService_e;

typedef enum
{
	eHOMAPI_Comm_Structure,
	eHOMAPI_Comm_Jason,
} HOMxCommDataType_e;

typedef struct
{
	HINT8	cmd; /* HOMxDMP_Command_e: eHOMAPI_DMP_Command_Undefined, ... */
	HINT32	size;
	HINT8	*data;
} HOMxCommData_t;

typedef enum
{
	HOMxComm_FolderType_Unknown = 1,
	HOMxComm_FolderType_Music,
	HOMxComm_FolderType_Video,
	HOMxComm_FolderType_Image,
} HOMxComm_FolderType_e;

typedef enum {
	//MediaType_Undefiend,
	HOMxComm_MediaType_Container,
	HOMxComm_MediaType_Video,
	HOMxComm_MediaType_Music,
	HOMxComm_MediaType_Photo,
	HOMxComm_MediaType_Server,
	HOMxComm_MediaType_Unknown,
	HOMxComm_MediaType_Max,
} HOMxComm_MediaType_e;

typedef enum
{
	HOMxComm_ThumbnailType_No = 0,
	HOMxComm_ThumbnailType_Media,
	HOMxComm_ThumbnailType_Album,
} HOMxComm_ThumbnailType_t;

typedef struct {
	HOMxComm_FolderType_e         eFolderType;
	HOMxComm_MediaType_e          eItemType;

	HCHAR                   	szUDN[ UDN_MAX ];
	HCHAR                   	ID[TITLE_SIZE];

	HINT32                  	ChildCount;
	HCHAR                   	IDofUpper[ TITLE_SIZE];

	/* item name, content name or directory name or file name */
	HCHAR                   	Title[TITLE_SIZE];

	/* reource */
	HCHAR                   	URI[URL_MAX];
	HCHAR                  		ThumbnailURI[URL_MAX];
	HOMxComm_ThumbnailType_t     thumbnailType;

	/* media common */
	HLONG                   	FileSize;
	HCHAR                   	Protocol[URL_MAX];

	/* video / music */
	HINT32                  	Bitrate;
	HUINT32                 	Duration;

	/* music */
	HINT32                  	NrAudioChannels;
	HINT32                  	SampleFrequency;

	HCHAR                   	AlbumTitle[TITLE_SIZE];
	HCHAR                   	Genre[GENRE_SIZE];
	HCHAR                   	Artist[TITLE_SIZE];

	/* photo */
	HCHAR                   	resolution[RES_SIZE];

	/* DLNA Relative Properties */
	HBOOL                   	ByteSeekable;
	HBOOL                   	TimeSeekable;

	/* dtcp */
	HCHAR                   	dtcpenable;
	HCHAR                   	dtcp_portnumber[DTCP_PORT_SIZE];
	HCHAR                   	dtcp_ipaddress[DTCP_IPV4ADDR_SIZE];

	/* DirectBrowse */
	HINT32                  	totalMatches;

	void                    	*res_data;
} HOMxComm_CdsInfo_t;

/*-----------------------------------------------------------------------------
 *  DMP
 *-----------------------------------------------------------------------------*/
typedef enum
{
	eHOMAPI_DMP_Command_Start=0,
	eHOMAPI_DMP_Command_Stop,

	eHOMAPI_DMP_Command_Browse,
	eHOMAPI_DMP_Command_Search,
	eHOMAPI_DMP_Command_SearchAll,
	eHOMAPI_DMP_Command_CdsRemove,

	eHOMAPI_DMP_Command_DmsList,

	eHOMAPI_DMP_Command_DmsRecDst,
	eHOMAPI_DMP_Command_DmsDiskInfo,
	eHOMAPI_DMP_Command_DmsRefresh,

	eHOMAPI_DMP_Command_DmsUploadStatus,
	eHOMAPI_DMP_Command_Undefined,  /* event notifier for query */
} HOMxDMP_Command_e;

typedef enum
{
	eHOMAPI_DMP_Event_DmsAdd,
	eHOMAPI_DMP_Event_DmsRemove,

	eHOMAPI_DMP_Event_CdsListAdd,
	eHOMAPI_DMP_Event_CdsListEnd,

	eHOMAPI_DMP_Event_DmsList,
	eHOMAPI_DMP_Event_DmsRecRst,
	eHOMAPI_DMP_Event_DmsDiskInfo,
	eHOMAPI_DMP_Event_CdsRemoved,

	eHOMAPI_DMP_Event_SearchResult,
	eHOMAPI_DMP_Event_SearchAllResult,

} HOMxDMP_Event_t;

/*  Call  */
typedef struct
{
    HCHAR   szUDN[48];
    HCHAR   szCID[512+ 128];
    HCHAR   szSort[128];
    HINT32  nStartIdx;
    HINT32  nRequestNum;
} HOMxDMP_Args_Browse_t;

typedef struct
{
    HCHAR   szUDN[48];
    HCHAR   szCID[512+ 128];
    HCHAR   szSort[128];
    HCHAR   szCategory[128];
    HCHAR   szKeyword[128];
    HINT32  nStartIdx;
    HINT32  nRequestNum;
    HINT32  nRequestId;
} HOMxDMP_Args_Search_t;

typedef struct
{
    HCHAR   szCID[512+ 128];
    HCHAR   szSort[128];
    HCHAR   szCategory[128];
    HCHAR   szKeyword[128];
    HINT32  nRequestId;
} HOMxDMP_Args_SearchAll_t;

typedef struct
{
    HCHAR   szUDN[48];
} HOMxDMP_Args_DmsRecDst_t;

typedef struct
{
    HCHAR   szUDN[48];
    HCHAR   szID[128];
} HOMxDMP_Args_DmsDiskInfo_t;

typedef struct
{
    HCHAR   szUDN[48];
} HOMxDMP_Args_DmsRefresh_t;

typedef struct
{
    HCHAR   szUDN[48];
    HCHAR   szCID[512+ 128];
} HOMxDMP_Args_CdsRemove_t;

/*  Event  */
typedef struct  {
    HCHAR   szFriendlyName[ 128 ];
    HCHAR   szUDN[ 48 ];
    HCHAR   szIp[  20];
} HOMxDMP_DmsInfo_t;

typedef struct
{
	HCHAR	id[128];
	HCHAR	name[128];
} HOMxDMP_RecordDestination_t;

typedef struct
{
    HUINT64  total; /* Byte */
    HUINT64  available; /* Byte */
} HOMxDMP_DmsDiskInfo_t;

typedef struct
{
	HCHAR	szUDN[48];
	HCHAR	szCID[512+ 128];
	HINT32	nRetVal;
} HOMxDMP_CdsRemoveResult_t;

typedef struct {

	HINT32 nReqId;
	HINT32 nTotalMatch;
	HINT32 nReturnCnt;  // num of contet item
	HINT32 nErrCode;
	HBOOL  bAllDMS; /* whether this result is come from searchItem(udn) or searchAll() */

	HCHAR   szUDN[48];
	HCHAR 	szCID[ TITLE_SIZE ]; ///< Object ID

} HOMxDMP_ContentResult_t;

/*-----------------------------------------------------------------------------
 *  DMR
 *-----------------------------------------------------------------------------*/
typedef enum
{
	eHOMAPI_DMR_Command_None=0,
	eHOMAPI_DMR_Command_Start,
	eHOMAPI_DMR_Command_Stop,

	eHOMAPI_DMR_Command_Rename,
	eHOMAPI_DMR_Command_BringIn,
	eHOMAPI_DMR_Command_OperationMode,
} HOMxDMR_Command_e;

typedef enum
{
	eHOMAPI_DMR_Event_SetUri=0,
	eHOMAPI_DMR_Event_Play,
	eHOMAPI_DMR_Event_Stop,
	eHOMAPI_DMR_Event_Pause,
	eHOMAPI_DMR_Event_Seek,
	eHOMAPI_DMR_Event_Volume,
	eHOMAPI_DMR_Event_Mute,
	eHOMAPI_DMR_Event_BringIn /* request setMediaChange(uri) to web app */
} HOMxDMR_Event_e;

/*  Call  */
typedef struct
{
	HCHAR szFriendlyName[128];
} HOMxDMR_Args_Start_t;

typedef struct
{
	HCHAR szFriendlyName[128];
} HOMxDMR_Args_Rename_t;

typedef struct
{
	HCHAR szMediaURI[1024];
	HCHAR szMediaPath[1024];
} HOMxDMR_Args_BringIn_t;

typedef struct
{
	HINT32 mode;
} HOMxDMR_Args_OperationMode_t;

/*  Event  */
typedef struct
{
	HINT32	nPlaySpeed;
} HOMxDMR_Event_Play_t;

typedef struct
{
	HLONG	nPosition;
} HOMxDMR_Event_Seek_t;

typedef struct
{
	HINT32	nVolume;
} HOMxDMR_Event_Volume_t;

typedef struct
{
	HBOOL	bMute;
} HOMxDMR_Event_Mute_t;

/*-----------------------------------------------------------------------------
 *  DMS
 *-----------------------------------------------------------------------------*/
typedef enum
{
	eHOMAPI_DMS_Command_Start,
	eHOMAPI_DMS_Command_Stop,

	eHOMAPI_DMS_Command_Rename,
	eHOMAPI_DMS_Command_Info,
	eHOMAPI_DMS_Command_GetLiveStreamInfo,
} HOMxDMS_Command_e;

typedef enum
{
	eHOMAPI_DMS_Event_DmsInfo,
	eHOMAPI_DMS_Event_DmsRunningState,
	eHOMAPI_DMS_Event_SetupStreaming,
	eHOMAPI_DMS_Event_StartStreaming,
	eHOMAPI_DMS_Event_StopStreaming,
	eHOMAPI_DMS_Event_ReleaseStreaming,
} HOMxDMS_Event_e;

/* Call */
typedef struct
{
	HCHAR szFriendlyName[128];
	HBOOL bBootTimeStart;
} HOMxDMS_Args_Start_t;

typedef struct
{
	HCHAR szFriendlyName[128];
} HOMxDMS_Args_Rename_t;

typedef struct
{
	HINT32 nSvcUid;
} HOMxDMS_Args_LiveStreamInfo_t;


/* APKS_DLNA_DMS_LiveStreamInfo_t -> HOMxDMS_LiveStreamInfo_t -> PL_DMS_LiveStreamInfo_t */
typedef	struct	{
	HCHAR	szURI[ 1024 ];
	HCHAR	szMIME[ 256 ];
} HOMxDMS_LiveStreamInfo_t;

/* Event */
typedef struct
{
    HCHAR           szFriendlyName[128];
    HCHAR           szUDN[ 48 ];

    HCHAR           szModelName[ 32 ];
    HCHAR           szModelDescription[ 128 ];
    HCHAR           szModelURL[ 512 ];
    HCHAR           szModelNumber[ 32 ];

    HINT32          upnp_port;
    HINT32          webserver_port;
    HINT32          upnp_dtcp_port;
    HINT32          uploadserver_port;
} HOMxDMS_Event_DmsInfo_t;

typedef struct
{
    HCHAR           szRunState[ 32 ];
    HINT32          nErrCode;
} HOMxDMS_Event_RunState_t;

typedef struct
{
	HUINT32		unMsgId;
	HCHAR		cHumaxKey[128];
} HOMxDMR_Event_HumaxKey_t;

typedef struct
{
	HUINT32		unMsgId;
	HCHAR		cUri[1024];
	HINT32		nStreamType;			/* 0: Stream Live, 1: Stream File, 2: SATIP */
	HUINT32		unProfileId;
	HBOOL		bTranscoding;
	HCHAR		cUseragent[64];
	HCHAR		cIP[32];
	HCHAR		cHumaxKey[128];
} HOMxDMS_Event_Streaming_t;

/*-----------------------------------------------------------------------------
 *  WoonServer
 *-----------------------------------------------------------------------------*/
typedef enum
{
	eHOMAPI_WoonServer_Command_Start,
	eHOMAPI_WoonServer_Command_Stop,

	eHOMAPI_WoonServer_Command_Rename,
	eHOMAPI_WoonServer_Command_TestConnection,
} HOMxWoonServer_Command_e;

typedef enum
{
	eHOMAPI_WoonServer_Event_Start,
	eHOMAPI_WoonServer_Event_Stop,
	eHOMAPI_WoonServer_Event_Renamed,
	eHOMAPI_WoonServer_Event_TestConnection,
	eHOMAPI_WoonServer_Event_PinCode,
} HOMxWoonServer_Event_e;

/* Call */
typedef struct
{
	HCHAR szFriendlyName[128];
} HOMxWoonServer_Args_Start_t;

typedef struct
{
	HCHAR szFriendlyName[128];
} HOMxWoonServer_Args_Rename_t;

/*  Event  */
typedef struct
{
	HINT32	isStarted;
} HOMxWoonServer_Event_Started_t;

typedef struct
{
	HINT32	isStopped;
} HOMxWoonServer_Event_Stopped_t;

typedef struct
{
	HINT32	isRenamed;
} HOMxWoonServer_Event_Renamed_t;

typedef struct
{
	HINT32	bOpen;
	HCHAR	szPinCode[5];
} HOMxWoonServer_Event_PinCode_t;

typedef struct
{
	HINT32	nState;
	HCHAR 	szLocalIp[128];
	HCHAR 	szExtIp[128];
	HCHAR 	szPubIp[128];
	HINT32 	nPubPort;
	HINT32 	nExtPort;
} HOMxWoonServer_Event_ServerInfo_t;

/*-----------------------------------------------------------------------------
 *  Dial
 *-----------------------------------------------------------------------------*/
typedef enum
{
	eHOMAPI_DIAL_Command_Start,
	eHOMAPI_DIAL_Command_Stop,
	eHOMAPI_DIAL_Command_Rename,

	eHOMAPI_DIAL_Command_AppRegist,
	eHOMAPI_DIAL_Command_AppUnregist,
	eHOMAPI_DIAL_Command_AppStatusUpdate,

	eHOMAPI_DIAL_Command_SetCookie,

	/* Neflix related */
	eHOMAPI_DIAL_Command_IsRunning,
	eHOMAPI_DIAL_Command_GetFriendlyName,
	eHOMAPI_DIAL_Command_GetAdditionalDataUrl,
} HOMxDIAL_Command_e;

typedef enum
{
	eHOMAPI_DIAL_Event_AppLaunch,
	eHOMAPI_DIAL_Event_AppStop,
	eHOMAPI_DIAL_Event_AppStatus,

	eHOMAPI_DIAL_Event_Started,
	eHOMAPI_DIAL_Event_Stopped,
} HOMxDIAL_Event_e;

/*  Call  */
typedef struct
{
	HCHAR szFriendlyName[128];
} HOMxDIAL_Args_Start_t;

typedef struct
{
	HCHAR szApplicationName[128];
} HOMxDIAL_Args_AppRegist_t;

typedef struct
{
	HCHAR szApplicationName[128];
} HOMxDIAL_Args_AppUnregist_t;

typedef struct
{
	HCHAR szApplicationName[128];
	HUINT32	nStatus;
} HOMxDIAL_Args_AppStatus_t;

typedef struct
{
	HCHAR szFriendlyName[128];
} HOMxDIAL_Args_Rename_t;

typedef struct
{
	HCHAR cookie[COOKIE_MAX];
} HOMxDIAL_Args_Cookie_t;


/*  Event  */
// Please sync to 'DIAL_EventInfo_t' in "$homma/main/service/include/dial.h"
typedef struct
{
	HCHAR szApplicationName[128];
	HCHAR szUrl[4096 + 1];
} HOMxDIAL_Event_AppLaunch_t;

/*-----------------------------------------------------------------------------
 *  AIRPLAY
 *-----------------------------------------------------------------------------*/
typedef enum
{
	eHOMAPI_AIRPLAY_Command_Start,
	eHOMAPI_AIRPLAY_Command_Stop,
	eHOMAPI_AIRPLAY_Command_SetPlaybackInfo
} HOMxAIRPLAY_Command_e;

typedef enum
{
	eHOMAPI_AIRPLAY_OP_PLAY = 0,
	eHOMAPI_AIRPLAY_OP_SETPOSITION,
	eHOMAPI_AIRPLAY_OP_SETRATE,
	eHOMAPI_AIRPLAY_OP_STOP,
	eHOMAPI_AIRPLAY_OP_SHOWPHOTO,
	eHOMAPI_AIRPLAY_OP_GET_PLAYBACK_INFO,
	eHOMAPI_AIRTUNES_OP_PLAY,
	eHOMAPI_AIRTUNES_OP_FLUSH,
	eHOMAPI_AIRTUNES_OP_STOP,
	eHOMAPI_AIRTUNES_OP_SET_METADATA,
	eHOMAPI_AIRTUNES_OP_SET_ALBUMART
} HOMxAIRPLAY_Event_t;

typedef enum
{
	eHOMAPI_AIRPLAY_PLAYBACK_NONE,
	eHOMAPI_AIRPLAY_PLAYBACK_PAUSE,
	eHOMAPI_AIRPLAY_PLAYBACK_LOADING,
	eHOMAPI_AIRPLAY_PLAYBACK_PLAYING,
	eHOMAPI_AIRPLAY_PLAYBACK_REACHED_EOF,
	eHOMAPI_AIRPLAY_PLAYBACK_STOPPED,
	eHOMAPI_AIRPLAY_PLAYBACK_ERROR
} HOMxAIRPLAY_PLAYBACK_STATUS;

typedef struct HOMxAIRPLAY_PLAYBACK_INFO {
	HOMxAIRPLAY_PLAYBACK_STATUS ePlaybackStatus;
	HINT64 ullDuration;
	HINT64 ullStartPosition;
	HINT64 ullPosition;
	HINT64 ullBufferedRange;
	HINT64 ullSeekableRange;
	HINT32 unRate;
}HOMxAIRPLAY_PLAYBACK_INFO_t;

typedef struct HOMxAIRPLAY_PARAMETER_WITH_URL {
	HINT64	decimalParam;		/* duration, position */
	HINT8	contentURL[AIRPLAY_URL_MAX];
}HOMxAIRPLAY_PARAMETER_WITH_URL_t;

typedef struct HOMxAIRPLAY_PARAMETER {
	HINT64	decimalParam;		/* duration, position */
}HOMxAIRPLAY_PARAMETER_t;

typedef struct HOMxAIRTUNES_URL {
	HINT8	contentURL[AIRPLAY_URL_MAX];
}HOMxAIRTUNES_URL_t;

typedef struct HOMxAIRTUNES_METADATA {
	HCHAR szTitle[128];
	HCHAR szAlbum[128];
	HCHAR szArtist[128];
}HOMxAIRTUNES_METADATA_t;

typedef struct HOMxAIRTUNES_ALBUMART {
	HCHAR szAlbumArtPath[512];
	HINT32 unAlbumArtSize;
}HOMxAIRTUNES_ALBUMART_t;

/*-----------------------------------------------------------------------------
 * SAT > IP
 *-----------------------------------------------------------------------------*/
typedef enum
{
	eHOMAPI_SATIP_Command_Start,
	eHOMAPI_SATIP_Command_Stop,
	eHOMAPI_SATIP_Command_Rename,
} HOMxSATIP_Command_e;

typedef enum
{
	eHOMAPI_SATIP_Event_Started,
	eHOMAPI_SATIP_Event_Stopped,
	eHOMAPI_SATIP_Event_Status,
} HOMxSATIP_Event_e;

/*  Call  */
typedef struct
{
	HCHAR szFriendlyName[128];
} HOMxSATIP_Args_Start_t;

typedef struct
{
	HCHAR szFriendlyName[128];
} HOMxSATIP_Args_Rename_t;

/*  Event  */
typedef struct
{
	HUINT32	nStatus;
} HOMxSATIP_Event_Status;

/*-----------------------------------------------------------------------------
 *  RC
 *-----------------------------------------------------------------------------*/
typedef enum
{
	eHOMAPI_RC_Command_Start,
	eHOMAPI_RC_Command_Stop,
	eHOMAPI_RC_Command_Rename,
	eHOMAPI_RC_Command_Unpair,
} HOMxRC_Command_e;

typedef enum
{
	eHOMAPI_RC_Event_Started,
	eHOMAPI_RC_Event_Stopped,
	eHOMAPI_RC_Event_Paired,
	eHOMAPI_RC_Event_Unpaired,
	eHOMAPI_RC_Event_Message,
	eHOMAPI_RC_Event_LaunchApp,
	eHOMAPI_RC_Event_SetFriendlyName,
} HOMxRC_Event_e;
/*  Call  */
typedef struct
{
	HCHAR szFriendlyName[128];
} HOMxRC_Args_Start_t;
typedef struct
{
	HCHAR szFriendlyName[128];
} HOMxRC_Args_Rename_t;
typedef struct
{
	HCHAR szName[128];
	HCHAR szData[2048]; // [message | url]
} HOMxRC_Events_t;
#define ________________Public_Functions___________________________
typedef HUINT32 (*HomApiNotifier)(HUINT32 accessCode, HUINT32 cmd, void *param, HUINT32 size);
typedef	HERROR	(*HOMAPI_EventCallback)(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
typedef void (*HOMAPI_CheckHommaStatusCallback)(HBOOL bConnected);

extern	void    HOMAPI_Call (HOMxService_e target,  void *data);
extern 	HUINT32 HOMAPI_Query (HOMxService_e target, void *data, void *userdata, HBOOL (* process) (HUINT32, void *, HUINT32, void *));
extern 	HERROR	HOMAPI_ConnectNotifier(HUINT32 ulAccessCode, HomApiNotifier pfnCallback);
extern	HERROR	HOMAPI_DisconnectNotifier(HUINT32 ulAccessCode, HUINT32 notifier_id);
extern	HERROR	HOMAPI_RegisterCallbackAndCheckHommaStatus(HOMAPI_CheckHommaStatusCallback fnCallback);
extern	HERROR	HOMAPI_UnRegisterCheckHommaStatus(void);
#ifdef	__cplusplus
}
#endif

#endif //end of __HOMAPI_H__
