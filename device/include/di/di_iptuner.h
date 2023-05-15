/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:       $Workfile:   di_iptuner.h  $
 * Version:         $Revision:   1.0  $
 * Original Author: SungYoung Jeon
 * Current Author:  $Author: junsy@humaxdigital.com $
 * Date:            $Date:  May 20 03:08:21 KST 2009 $
 * File Description:	     IP Tuner Main Header File
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef __DI_IPTUNER_H__
#define __DI_IPTUNER_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include "di_media.h"
/* End Including Headers*/


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */

/* End Extern variable */


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
/* Start Macro definition */
#define DLNA_FILESEEK_SET			0
#define DLNA_FILESEEK_CUR			1
#define DLNA_FILESEEK_END			2

#define DLNA_FILEACCESS_NONE 0
#define DLNA_FILEACCESS_FILE 1
#define DLNA_FILEACCESS_HTTP 2
#define DLNA_FILEACCESS_WMSP 3
#define DLNA_FILEACCESS_RTSP 4
#define DLNA_FILEACCESS_USER 10

#define DI_IPTUNER_SSTR_MAX_STREAM 32
#define DI_IPTUNER_SSTR_MAX_TRACK 32
#define DI_IPTUNER_SSTR_MAX_CATTR 12

#ifdef DI_IPTUNER_URL_LENGTH
#define IPTUNER_CONST_URL_LENGTH DI_IPTUNER_URL_LENGTH
#else
#define IPTUNER_CONST_URL_LENGTH (512)
#endif

/* End Macro definition */


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

/**
 * @brief 	IPTUNER Download Method
 * 			Download 방식을 결정한다.
 */
typedef enum
{
	/** The Content-Item must be fully downloaded and stored before playback */
	DI_IPTUNER_FULL_DOWNLOAD,
	/** The Content-Item is available for playback whilst it is being downloaded and stored. */
	DI_IPTUNER_PLAYABLE_DOWNLOAD,
	/** The Content-Item is streamed and should not be stored. */
	DI_IPTUNER_STREAMING,
	/** DLNA */
	DI_IPTUNER_DLNA
} DI_IPTUNER_DownloadType_e;

/**
 * @brief 	IPTUNER Protocols enum.
 * 			Download & Streaming Protocol type
 */
typedef enum
{
	DI_IPTUNER_PROTOCOL_HTTP,	/* implemented */
	DI_IPTUNER_PROTOCOL_RTSP,	/* not implemented */
	DI_IPTUNER_PROTOCOL_MMS,	/* implemented */
	DI_IPTUNER_PROTOCOL_FTP,	/* not implemented */
	DI_IPTUNER_PROTOCOL_SMB,	/* not implemented */
	DI_IPTUNER_PROTOCOL_SSTR,	/* not implemented */ /* for SSTR */
	DI_IPTUNER_PROTOCOL_HLS,	/* not implemented */ /* for SSTR */	
	DI_IPTUNER_PROTOCOL_UNKNOWN,
} DI_IPTUNER_ProtocolType_e;

/**
 * @brief 	IPTUNER Index enum
 * 			IPTUNER index number.
 */
typedef enum
{							/* ----- download type ---------	, ------ protocol type -- 	, -- Supported ----*/
	DI_IPTUNER_DNP_HTTP,	/* DI_IPTUNER_PLAYABKE_DOWNLOAD		, DI_PROTOCOL_HTTP 			,	yes		*/
	DI_IPTUNER_DNP_MMS,		/* DI_IPTUNER_PLAYABKE_DOWNLOAD		, DI_PROTOCOL_MMS 			, 	yes		*/
	DI_IPTUNER_DNP_RTSP,	/* DI_IPTUNER_PLAYABKE_DOWNLOAD		, DI_PROTOCOL_RTSP 			,  	not yet */
	DI_IPTUNER_DNP_FTP,		/* DI_IPTUNER_PLAYABKE_DOWNLOAD		, DI_PROTOCOL_FTP 			, 	not yet */
	DI_IPTUNER_DLNA_DNP,	/* DI_IPTUNER_DLNA_DNP 				, DI_PROTOCOL_DNP 			, 	yes		*/
	DI_IPTUNER_STREAM_HTTP,	/* DI_IPTUNER_STREAMING 			, DI_PROTOCOL_HTTP 			,	yes 	*/
	DI_IPTUNER_STREAM_MMS,	/* DI_IPTUNER_STREAMING 			, DI_PROTOCOL_MMS 			,	yes		*/
	DI_IPTUNER_STREAM_RTSP,	/* DI_IPTUNER_STREAMING 			, DI_PROTOCOL_RTSP 			, 	not yet	*/
	DI_IPTUNER_STREAM_SSTR, /* DI_IPTUNER_STREAMING			, DI_PROTOCOL_SSTR			,	not yet 	*/
	DI_IPTUNER_STREAM_HLS, /* DI_IPTUNER_STREAMING				, DI_PROTOCOL_HLS			,	not yet 	*/	
	DI_IPTUNER_MAX_NUM		/* Current MAXIMUM Tuners */
} DI_IPTUNER_Index_e ;

/**
 * @brief 	IPTUNER Custom Callback Type enum
 */
typedef enum
{							
	DI_IPTUNER_CUSTOM_VMX, /* Verimatrix Key Retrieve Callback */	
	DI_IPTUNER_CUSTOM_MAX	/* MAXIMUM Custom CallBack */
} DI_IPTUNER_CUSTOM_e;

typedef void (*DI_IPTUNER_VMXKeyRetrvCallback) (const void *pUrl, void *pKey, HINT32 *puKeySize, DI_ERR_CODE *errCode);

typedef struct
{
	//	DI_IPTUNER_CUSTOMCB_e eType;
	/* Type ( DI_IPTUNER_CUSTOM_e  ) 과 Callback을 Pair로 추가하여 사용 한다. */
	DI_IPTUNER_VMXKeyRetrvCallback cbVmxKeyRetrv; /* Verimatrix Key Retrieve Callback */	
	
} DI_IPTUNER_CustomCallback_t;

typedef enum 
{
	DI_IPTUNER_SSTR_SUBTYPE_UNKNOWN = 0,
	DI_IPTUNER_SSTR_SUBTYPE_SCMD,
	DI_IPTUNER_SSTR_SUBTYPE_CHAP,
	DI_IPTUNER_SSTR_SUBTYPE_SUBT,
	DI_IPTUNER_SSTR_SUBTYPE_CAPT,	
	DI_IPTUNER_SSTR_SUBTYPE_DESC,
	DI_IPTUNER_SSTR_SUBTYPE_CTRL,
	DI_IPTUNER_SSTR_SUBTYPE_DATA
} DI_IPTUNER_SSTR_SUBTYPE_e;

typedef enum 
{
	DI_IPTUNER_SSTR_TYPE_VIDEO,	
	DI_IPTUNER_SSTR_TYPE_AUDIO,
	DI_IPTUNER_SSTR_TYPE_TEXT,
} DI_IPTUNER_SSTR_TYPE_e;

typedef struct DI_IPTUNER_SSTR_CATTR_s
{
	HINT32 nNameLen;
	HINT8 const *pName;
	HINT32 nValueLen;
	HINT8 const *pValue;
} DI_IPTUNER_SSTR_CATTR_t;

typedef struct DI_IPTUNER_SSTR_TRACK_s
{
	HUINT32 ulIndex;
	HUINT32 ulBitrate;
	HINT8 const *pFourCC;
	HINT32 nCustomAttrCnt;
	DI_IPTUNER_SSTR_CATTR_t stCustomAttr[DI_IPTUNER_SSTR_MAX_CATTR];
} DI_IPTUNER_SSTR_TRACK_t;

typedef struct DI_IPTUNER_SSTR_STREAM_s
{
	HUINT32 ulIndex;
	DI_IPTUNER_SSTR_TYPE_e eType;
	HINT32 nNameLen;
	HINT8 const *pName;
	HINT32 nLangLen;
	HINT8 const *pLanguage;	
	HINT32 nTrackCnt;
	DI_IPTUNER_SSTR_TRACK_t stTrack[DI_IPTUNER_SSTR_MAX_TRACK];
} DI_IPTUNER_SSTR_STREAM_t;

typedef struct DI_IPTUNER_SSTR_Info_s
{
	HINT32 nStreamCnt;
	DI_IPTUNER_SSTR_STREAM_t stStream[DI_IPTUNER_SSTR_MAX_STREAM];
} DI_IPTUNER_SSTR_Info_t;

typedef enum
{
	DI_IPTUNER_PROPERTY_SSTR_VIDEO_STREAM = 0,
	DI_IPTUNER_PROPERTY_SSTR_AUDIO_STREAM,
	DI_IPTUNER_PROPERTY_SSTR_TEXT_STREAM,    
	DI_IPTUNER_PROPERTY_SSTR_VIDEO_TRACK,
	DI_IPTUNER_PROPERTY_SSTR_AUDIO_TRACK,
	DI_IPTUNER_PROPERTY_SSTR_TEXT_TRACK,
	DI_IPTUNER_PROPERTY_MAX
} DI_IPTUNER_ADAPTIVE_PROPERTY_e;

typedef struct 
{
           unsigned int uiIndex;
           unsigned char ucStreams[DI_IPTUNER_SSTR_MAX_STREAM]; /* not supported yet */
} DI_IPTUNER_ADAPTIVE_PROPERTY_DATA_t;

/**
 * @brief 	IPTUNER Streaming Mode enum
 * 			IPTUNER Streaming Mode number.
 */
typedef enum
{
	DI_IPTUNER_STREAMING_MODE_STORAGE,	/** USE HDD */
	DI_IPTUNER_STREAMING_MODE_RAMBUFFER,	/** USE RAM Buffer */
	DI_IPTUNER_STREAMING_MODE_REALTIME,	/** USE Non-Buffer(Realtime) */
	DI_IPTUNER_STREAMING_MODE_MAX	/** MAX */
}DI_IPTUNER_STREAMING_MODE_e;

/**
 * @brief 	IPTUNER Structure
 * 			IPTUNER Open을 위한 structure defination
 * 			mediaInfo는 Open된 Stream의 MEDIA 정보를 제공한다.
 */
typedef struct DI_IPTUNER_s
{
	HINT8 *url;
	HBOOL isAvailable;		/* TRUE or FALSE State */
	DI_IPTUNER_DownloadType_e downType;
//	DI_IPTUNER_ProtocolType_e protocolType;
	DI_MEDIA_INFO_t	mediaInfo;
} DI_IPTUNER_t;
/**
 * @brief 	IPTUNER infomation
 *
 */
typedef struct DI_IPTUNER_Info_s
{
	DI_IPTUNER_t *ptIPTuner;
	HUINT64  nDownloadLength;	/* for DNP tuner : bytes*/
	HUINT64  nContentLength;	/* for DNP tuner : bytes*/

	/* MAXDOME DNP INFO */
	HUINT32 nBufferingPercent;
	HUINT32 nRemainBufferingSec;
	void * pDownloadedChunkArray;
	HUINT32 nLastChunkIndex;
	union
	{
		DI_IPTUNER_SSTR_Info_t *pSSTRInfo; /* only use as a memory reference. Do not allocation. */
	} adaptiveInfo;
} DI_IPTUNER_Info_t;

typedef struct
{
	HUINT32 unMsg;
	HUINT32 unCmd;
	HUINT32 unParam1;
	HUINT32 unParam2;
} DI_IPTUNER_Msg_t;

typedef enum
{
	CMD_CONTROL_PAUSE,	/** param no */
	CMD_CONTROL_RESUME,	/** param no */
	CMD_CONTROL_SPEED,	/** param yes (HUINT64)*/
	CMD_CONTROL_SEEK	/** param yes (HUINT64)*/
}DI_IPTUNER_CMD_e;

//#if STREAM_BUFFERING
typedef void (*DI_IPTUNER_PlayerPauseCallback) (DI_IPTUNER_Index_e nTunerIndex);
typedef void (*DI_IPTUNER_PlayerPlayCallback) (DI_IPTUNER_Index_e nTunerIndex);

typedef enum {
	PLAYER_BUFFERING_CBTYPE_PLAY,
	PLAYER_BUFFERING_CBTYPE_PAUSE,
	PLAYER_BUFFERING_CBTYPE_SEEK,
	PLAYER_BUFFERING_CBTYPE_TRICK,
} DI_IPTUNER_PBCallbackType_e;

typedef void (*DI_IPTUNER_PlayerBufferingCallback) (DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_PBCallbackType_e type);

typedef struct 
{
	DI_IPTUNER_PlayerPauseCallback		cbPauseCallback;
	DI_IPTUNER_PlayerPlayCallback	    cbPlayCallback;
	DI_IPTUNER_PlayerBufferingCallback	cbBufferCallback;
} DI_IPTUNER_PlayCallback_t;
//#endif


typedef void (*DI_IPTUNER_ChangeTitleCallback) (DI_IPTUNER_Index_e nTunerIndex, HINT8 *pcTitleString);

typedef struct 
{
	DI_IPTUNER_ChangeTitleCallback	    cbChangeTitleCallback;
} DI_IPTUNER_EventCallback_t;


typedef struct DLNA_FILECANCLE_TAG
{
	unsigned char cancel_code;

} DI_IPTUNER_DLNA_FILECANCEL_OP  ;

typedef struct DLNA_FILEDESC_TAG
{
	char *url;   // read only 로만 쓰시오

	long
			access_type;
	long			linear_access;
	DI_IPTUNER_DLNA_FILECANCEL_OP *cancel_op;
	long			option;
	unsigned char	user_data[32];
} DI_IPTUNER_DLNA_FILEDESC;

typedef struct DLNA_FILEHANDLE_TAG
{
	void		*apis;
	DI_IPTUNER_DLNA_FILEDESC desc;
	long		file_type;

	DI_IPTUNER_DLNA_FILECANCEL_OP static_cancel_op;
	DI_IPTUNER_DLNA_FILECANCEL_OP *cancel_op;
	long		error;
	long		eof;
	long		linear_access;
	long long	filesize;
	void *extra_data;
} *DI_IPTUNER_DLNA_FILEHANDLE;


typedef HUINT32 (*DLNA_OpenFile) (DI_IPTUNER_DLNA_FILEDESC *ptFileDescription, DI_IPTUNER_DLNA_FILEHANDLE *ptDLNAHandle);
typedef HUINT32 (*DLNA_CloseFile) (DI_IPTUNER_DLNA_FILEHANDLE tDLNAHandle);

typedef HUINT32 (*DLNA_ReadFile) (DI_IPTUNER_DLNA_FILEHANDLE tDLNAHandle, void *pBuffer, HUINT32 unBuffSize, HUINT32 *punReadSize);
typedef HUINT32 (*DLNA_SeekFile) (DI_IPTUNER_DLNA_FILEHANDLE tDLNAHandle, HINT64 offset, HINT32 Origin);
typedef HUINT32 (*DLNA_TellFile) (DI_IPTUNER_DLNA_FILEHANDLE tDLNAHandle, HUINT64 *plCurOffset);
typedef HUINT32 (*DLNA_IsEOFFile) (DI_IPTUNER_DLNA_FILEHANDLE tDLNAHandle);
typedef HUINT32 (*DLNA_GetFileSize) (DI_IPTUNER_DLNA_FILEHANDLE tDLNAHandle, HINT64 *pulFileSize);
typedef HINT64 (*DLNA_SeekForNexusFile) (DI_IPTUNER_DLNA_FILEHANDLE tDLNAHandle, HINT64 offset, HINT32 Origin);

typedef struct {
	DLNA_OpenFile			Open;
	DLNA_CloseFile			Close;
	DLNA_ReadFile			Read;
	DLNA_SeekFile			Seek;
	DLNA_TellFile			Tell;
	DLNA_IsEOFFile			IsEOF;
	DLNA_GetFileSize		GetFileSize;
	DLNA_OpenFile			OpenForNexus;
	DLNA_SeekForNexusFile		SeekForNexus;
	DLNA_GetFileSize		BoundsForNexus;
} DLNA_FileFunction_t;

typedef enum {
	KEYRETRV_TYPE_NONE			= 0x00000000,
	KEYRETRV_TYPE_VERIMATRIX	= 0x00000010,
} DI_IPTUNER_KEYRETRV_TYPE;

typedef void (*DI_IPTUNER_KeyRetrvCallback) (const void *pUrl, void *pKey, HINT32 *puKeySize, void *errCode);


/**
 * Define Callback Function
 * moved here from di_iptuner_dnp.h 
 */
typedef void (*fn_readystate_callback)(HUINT32 deviceId, HINT32 ev, HUINT32 param1);


/* End typedef */


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */

DI_ERR_CODE DI_IPTUNER_Init (void);
DI_IPTUNER_Index_e  DI_IPTUNER_Open ( DI_IPTUNER_DownloadType_e nDownloadType , HINT8 *url , DI_MEDIA_INFO_t *pMfpInfo );
DI_ERR_CODE DI_IPTUNER_Close (DI_IPTUNER_Index_e nTunerIndex);
DI_ERR_CODE DI_IPTUNER_Start (DI_IPTUNER_Index_e nTunerIndex);
DI_ERR_CODE DI_IPTUNER_Stop (DI_IPTUNER_Index_e nTunerIndex);
DI_ERR_CODE DI_IPTUNER_DnpPauseReceive (DI_IPTUNER_Index_e nTunerIndex);
DI_ERR_CODE DI_IPTUNER_DnpResumeReceive (DI_IPTUNER_Index_e nTunerIndex);
DI_ERR_CODE DI_IPTUNER_Control (DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2);
DI_ERR_CODE DI_IPTUNER_SetAdaptiveProperty(DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_ADAPTIVE_PROPERTY_e eCmd, DI_IPTUNER_ADAPTIVE_PROPERTY_DATA_t *pData);
DI_ERR_CODE DI_IPTUNER_GetAdaptiveProperty(DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_ADAPTIVE_PROPERTY_e eCmd, DI_IPTUNER_ADAPTIVE_PROPERTY_DATA_t *pData);
DI_ERR_CODE DI_IPTUNER_GetInfo (DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_Info_t *pInfo );
//#if STREAM_BUFFERING
DI_ERR_CODE DI_IPTUNER_RegisterBufferingCallback (DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_PlayCallback_t *pCallback);
void DI_IPTUNER_UnregisterBufferingCallback (DI_IPTUNER_Index_e nTunerIndex);

DI_ERR_CODE DI_IPTUNER_RegisterCustomCallback ( DI_IPTUNER_CUSTOM_e eType, DI_IPTUNER_CustomCallback_t Callback);
void DI_IPTUNER_UnregisterCustomCallback  ( DI_IPTUNER_CUSTOM_e eType );

DI_ERR_CODE DI_IPTUNER_StreamingMode (DI_IPTUNER_STREAMING_MODE_e *pnStreamingMode);
//#endif

DI_ERR_CODE DI_IPTUNER_InstallReadyStateCallback(HUINT32 ulDeviceId, fn_readystate_callback pFn);
DI_ERR_CODE DI_IPTUNER_InstallDLNAFunction (DLNA_FileFunction_t stDLNAFileFn);

DI_ERR_CODE DI_IPTUNER_ICY_RegisterEventCallback (DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_EventCallback_t *pCallback);

void DI_IPTUNER_ICY_UnregisterEventCallback (DI_IPTUNER_Index_e nTunerIndex);

DI_ERR_CODE DI_IPTUNER_ICYProbe (const HINT8 *path,  DI_MEDIA_INFO_t *pstMediaInfo, HBOOL *pbIsICY);

DI_ERR_CODE DI_IPTUNER_SetUserAgentInfo (HINT8 *pUserAgentInfo);
DI_ERR_CODE DI_IPTUNER_ForceMoveBufferingPosition (DI_IPTUNER_t *pHTuner);
/* End global function prototypes */

#ifdef  __cplusplus
}
#endif

#endif /* ! __DI_IPTUNER_H__ */
