/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  rest_datatype.h
	@brief

	Description:  									\n
	Module: node/binding/datatype		\n

	Copyright (c) 2014 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
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
#ifndef __REST_DATATYPE_H__
#define __REST_DATATYPE_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define REST_MEDIA_URL_LEN			256
#define REST_MEDIA_MIME_LEN			64

#define MAX_CBUFFER_SIZE			(64 * 1024)
#define MAX_TEMP_BUFFER_SIZE		(32 * 1024)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HCHAR		*szEnumStr;
	HINT32		 nEnumValue;
} RestEnumItem_t;

typedef	struct
{
	HCHAR			*szEnumType;
	RestEnumItem_t	 astEnumItems[];
} RestEnumStringTable_t;

// bind_datatype_channel.c
typedef struct
{
	HUINT32						 ulSvcAllNum;
	HUINT32						 ulSvcTvNum;
	HUINT32						 ulSvcRadioNum;

	HINT32						 nMinTvLcn, nMaxTvLcn;
	HINT32						 nMinRadioLcn, nMaxRadioLcn;
} RestChanListStatus_t;

typedef struct
{
	HUINT32						 ulTsInfoNum;
} RestTsListStatus_t;

typedef struct
{
	HUINT32						 ulAntInfoNum;
	HINT32						 nMinAntUid;
	HINT32						 nMaxAntUid;
	HINT32						 nAntConnectType;	// 'antennaConnectionType' : DxAntennaConnection_e, -1: N/A
} RestAntListStatus_t;

typedef struct
{
	HCHAR						szMimeType[64];
	HCHAR						szLiveStreamUrl[REST_MEDIA_URL_LEN];
} RestLiveStreamInfo_t;

typedef struct
{
	HCHAR						szMimeType[64];
	HCHAR						szThumbnailUrl[REST_MEDIA_URL_LEN];
} RestThumbnailInfo_t;

typedef struct
{
	HBOOL						 bLiveStream;
	HBOOL						 bThumbnail;
	
	RestLiveStreamInfo_t		 stLiveStreamInfo;
	// Thumbnail
	RestThumbnailInfo_t			 stThumbnailInfo;
} RestChanExtInfo_t;

typedef struct
{
	DxService_t					 stSvcInfo;

	// EXT Info:
	// Live Streaming
	RestLiveStreamInfo_t		 stLiveStreamInfo;
	// Thumbnail
	RestThumbnailInfo_t			 stThumbnailInfo;
	
} RestChannelExt_t;

typedef enum
{
	eTUNE_CHECK_DST_LiveMain,
	eTUNE_CHECK_DST_Rec,
	eTUNE_CHECK_DST_LiveStream,

	eTUNE_CHECK_DST_MaxCase
} RestTuneCheckDst_e;

typedef struct
{
	HUINT32					 ulTsUid;

	HBOOL					 abTunable[eTUNE_CHECK_DST_MaxCase];
} RestTsTunable_t;

// bind_datatype_player.c
typedef struct
{
	OxMediaPlay_MediaType_e		 eMediaType;
	HUINT32						 ulSvcUid;
} RestPlayerLiveSrc_t;

typedef struct
{
	OxMediaPlay_MediaType_e		 eMediaType;
	HUINT32						 ulContentsId;
} RestPlayerPvrPbSrc_t;

typedef struct
{
	OxMediaPlay_MediaType_e		 eMediaType;
	HCHAR						 szUrl[REST_MEDIA_URL_LEN];
} RestPlayerMediaPbSrc_t;

typedef union
{
	OxMediaPlay_MediaType_e		 eMediaType;
	RestPlayerLiveSrc_t			 stLive;
	RestPlayerPvrPbSrc_t		 stPvrPb;
	RestPlayerMediaPbSrc_t		 stMediaPb;	
} RestPlayerSrc_t;

typedef struct
{
	OxMediaPlay_Component_e		 eCompType;

	HINT32						 nCurrIndex;
	HINT32						 nNumComponent;
	OxMediaPlay_Component_t		*pstCompArray;
} RestPlayerComponent_t;

typedef struct
{
	HUINT32						 ulViewId;
	OxMediaPlay_PlayState_e		 ePlayState;
	RestPlayerSrc_t				 unSource;
	HINT32						 nSpeed;
	HUINT32						 ulStartTime;
	HUINT32						 ulDuration;
	OxMediaPlay_LiveLockState_e	 eLockState;
	RestPlayerComponent_t		 astComponents[eOxMP_COMPONENT_ENDOFCASE];
} RestPlayerStatus_t;

// bind_datatype_chanscan.c
typedef enum
{
	eREST_SCAN_TYPE_None			= 0,
		
	eREST_SCAN_TYPE_ChannelScan,
	eREST_SCAN_TYPE_AutoAntenna,
	eREST_SCAN_TYPE_AutoConnection,

	eREST_SCAN_TYPE_EndOfCase
} RestScanType_e;

typedef enum
{
	eREST_SCAN_CMD_None				= 0,

	eREST_SCAN_CMD_Start,
	eREST_SCAN_CMD_Stop,
	eREST_SCAN_CMD_Pause,
	eREST_SCAN_CMD_Resume,
	eREST_SCAN_CMD_Save,

	eREST_SCAN_CMD_EndOfCase	
} RestScanCmd_e;

typedef enum
{
	eREST_SCAN_STATE_None			= 0,

	eREST_SCAN_STATE_Idle,
	eREST_SCAN_STATE_Ready,
	eREST_SCAN_STATE_Starting,
	eREST_SCAN_STATE_Running,
	eREST_SCAN_STATE_Paused,
	eREST_SCAN_STATE_Finished,

	eREST_SCAN_STATE_EndOfCase	
} RestScanState_e;

typedef enum
{
	eREST_SCAN_CHANSEND_None			= 0,
		
	eREST_SCAN_CHANSEND_Simple,
	eREST_SCAN_CHANSEND_Detailed,

	eREST_SCAN_CHANSEND_EndOfCase
} RestScanChanSend_e;

typedef struct
{
	HUINT32					 ulScannerUid;
	RestScanType_e			 eScanType;
	RestScanCmd_e			 eCmd;
	RestScanState_e			 eState;
} RestScanBaseParam_t;

typedef struct
{
	HUINT32					 ulScannerUid;
	RestScanType_e			 eScanType;
	RestScanCmd_e			 eCmd;
	RestScanState_e			 eState;
	
	DxDeliveryType_e		 eDeliType;
	HINT32					 nFlagNetworkScan;
	HINT32					 nNetworkId;
	DxSvcType_e				 eSvcType;
	DxCasType_e				 eCasType;
	HINT32					 nReplaceChannel;

	HxList_t				*pstTpList;

	Handle_t				 hBuilderHandle;

	// Temporary Scanned Channels:
	HUINT32					 ulChannelNum;
	HUINT32					 ulTvNum;
	HUINT32					 ulRadioNum;
	HxList_t				*pstAllocedBufferList;		// DxService_t *(array)
	void					*pvFreeSvcObject;			// DxService_t : Do Not de-allocate!
	
	HxList_t				*pstAllSvcList, *pstLastSvcAll;				// DxService_t : Do Not de-allocate!
	HxList_t				*pstTvSvcList, *pstLastSvcTv;				// DxService_t : Do Not de-allocate!
	HxList_t				*pstRadioSvcList, *pstLastSvcRadio;			// DxService_t : Do Not de-allocate!
} RestScanChanParams_t;

typedef union
{
	RestScanBaseParam_t		 stBase;
	RestScanChanParams_t	 stChanScan;
} RestScanParams_t;

typedef struct
{
	HUINT32					 ulScannerUid;
	RestScanType_e			 eScanType;
} RestScanBaseResult_t;

typedef struct
{
	HUINT32					 ulScannerUid;
	RestScanType_e			 eScanType;

	HUINT32					 ulTotalTuningNum;
	HUINT32					 ulCompleteTuningNum;
	HUINT32					 ulSearchedChannelNum;
	HUINT32					 ulSearchedTvNum;
	HUINT32					 ulSearchedRadioNum;
	DxTuneParam_t			*pstCurrTuneParam;
	HUINT32					 ulCurrSignalStrength;
	HUINT32					 ulCurrSignalQuality;

	// Searched Channel Lists:
	RestScanChanSend_e		 eChannelSendType;
	
	DxSvcType_e				 eListSvcType;
	HUINT32					 ulListSvcNum;
	HxList_t				*pstSvcListHead;	
} RestScanChanResult_t;

typedef union
{
	RestScanBaseResult_t	 stBase;
	RestScanChanResult_t	 stChanScan;
} RestScanResult_t;

// bind_datatype_schedule.c
typedef enum
{
	eREST_SCH_ACTION_None			= 0,
		
	eREST_SCH_ACTION_Live,
	eREST_SCH_ACTION_Record,
	eREST_SCH_ACTION_SatRecord,
	eREST_SCH_ACTION_PowerOn,
	eREST_SCH_ACTION_PowerOff,
	eREST_SCH_ACTION_SwUpdate,
	eREST_SCH_ACTION_StreamingViaLive,
	eREST_SCH_ACTION_StreamingViaFile,	

	eREST_SCH_ACTION_EndOfCase	
} RestSchAction_e;

typedef enum
{
	eREST_SCH_TYPE_None				= 0,

	eREST_SCH_TYPE_TimeBased,
	eREST_SCH_TYPE_EventBased,
	eREST_SCH_TYPE_SeriesBased,
	eREST_SCH_TYPE_Instant,
	eREST_SCH_TYPE_Delayed,

	eREST_SCH_TYPE_EndOfCase	
} RestSchType_e;

// bind_datatype_record.c
typedef enum
{
	eREST_REC_SRCTYPE_None			= 0,

	eREST_REC_SRCTYPE_Channel,

	eREST_REC_SRCTYPE_EndOfCase	
} RestRecSrcType_e;

typedef enum
{
	eREST_REC_TYPE_None				= 0,

	eREST_REC_TYPE_Instant,
	eREST_REC_TYPE_Delayed,

	eREST_REC_TYPE_EndOfCase	
} RestRecType_e;

typedef struct
{
	RestRecSrcType_e		 eSrcType;

	union
	{
		OxMediaRec_StartInfo_t		 stChanRec;
	} unArgs;
} RestRecRequest_t;

// bind_datatype_recorder.c
typedef struct
{
	HUINT32						 ulInstIndex;
	OxMediaRec_RecState_e		 eState;

	HUINT32						 ulSlot;
	HUINT32						 ulContentsId;
	void						*pvReqMsgObj;			// RestMsg_t

	RestRecRequest_t			 stRecStartInfo;
} RestBindRecorderInst_t;

// bind_datatype_sysenv.c
typedef struct
{
	HCHAR			 szMainLangCode[4];			// ISO639, '\0': N/A
	HCHAR			 szAudioLangCode[4];		// ISO639, '\0': N/A
	HCHAR			 szSubtitleLangCode[4];		// ISO639, '\0': N/A
	HINT32			 nHardOfHearing;			// Boolean, -1: N/A
	HINT32			 nMaturityRating;			// DxRatingAge_e, -1: N/A
	HxCountry_e		 eCountry;					// eCountryID_NONE: N/A
	DxOperator_e	 eOperater;					// eDxOPERATOR_NOT_DEFINED: N/A
	HINT32			 nAntConnectType;			// DxAntennaConnection_e, -1: N/A
	HINT32			 nStartPaddingTime;			// -1: N/A
	HINT32			 nEndPaddingTime;			// -1: N/A
	HINT32			 nAutoDelete;				// Boolean, -1: N/A
	HINT32			 nGmtOffset;				// -255: N/A
	
} RestSysEnv_t;

// bind_datatype_settings.c
typedef struct
{
	HINT32		 nCountry;					// 'country' : HxCountry_e, -1:N/A
	HINT32		 nOperator;					// 'operator' : DxOperator_e, -1:N/A
	HINT32		 nGmtOffset;				// 'gmtOffset' : -255: N/A

	// Language
	HCHAR		 szMainLangCode[4];			// 'mainLanguage' : ISO639, '\0': N/A
	HCHAR		 szAudioLangCode[4];		// 'audioLanguage' : ISO639, '\0': N/A
	HCHAR		 szSubtitleLangCode[4];		// 'subtitleLanguage' : ISO639, '\0': N/A

	// Parental Control
	HINT32		 nMaturityRating;			// 'maturityRating' : DxRatingAge_e, -1: N/A

	// Output Video
	HINT32		 nVideoOnOff;				// 'videoOn' : HBOOL, -1:N/A
	HINT32		 nResolutionSetting;		// 'resolutionSetting' : DxResolutionSelect_e, -1:N/A
	HINT32		 nAppliedResolution;		// 'appliedResolution' : DxResolutionStatus_e, -1:N/A
	HINT32		 nTvAspectRatio;			// 'tvAspectRatio' : DxAspectRatio_e, -1:N/A
	HINT32		 nPictureRate;				// 'pictureRate' : DxPictureRate_e, -1:N/A
	HINT32		 nDisplayFormatSetting;		// 'displayFormatSetting' : DxDfcSelect_e, -1:N/A
	HINT32		 nAppliedDisplayFormat;		// 'appliedDisplayFormat' : DxDfcStatus_b, -1:N/A
	HINT32		 nBgColorRed;				// 'bgColorRed' : HUINT8, -1:N/A
	HINT32		 nBgColorGreen;				// 'bgColorGreen' : HUINT8, -1:N/A
	HINT32		 nBgColorBlue;				// 'bgColorBlue' : HUINT8, -1:N/A

	// Output Audio
	HINT32		 nAudioMute;				// 'audioMute' : HBOOL, -1: N/A
	HINT32		 nMaxVolume;				// 'maxVolume' : HUINT32, -1:N/A
	HINT32		 nVolume;					// 'volume' : HUINT32, -1:N/A
	HINT32		 nLipSyncDelay;				// 'lipSyncDelay' : HUINT32, -1,000,000:N/A
	HINT32		 nAudioDescription;			// 'audioDescription' : HBOOL, -1: N/A
	HINT32		 nStereoSelect;				// 'stereoSelect' : DxStereoSelect_e, -1:N/A
	HINT32		 nDualMonoSelect;			// 'dualMonoSelect' : DxDualMonoSelect_e, -1:N/A

	// Composite Port
	HINT32		 nCompositeSupport;			// 'compositeSupport' : HBOOL, -1: N/A
	HINT32		 nCompositeStandard;		// 'compositeStandard' : DxCompositeStandard_e, -1:N/A

	// Scart Port
	HINT32		 nTvScartSupportedFormat;	// 'tvScartSupportedFormat' : DxScartFormat_e, -1:N/A
	HINT32		 nVcrScartSupportedFormat;	// 'vcrScartSupportedFormat' : DxScartFormat_e, -1:N/A
	HINT32		 nTvScartFormat;			// 'tvScartFormat' : DxScartFormat_e, -1:N/A
	HINT32		 nVcrScartFormat;			// 'vcrScartFormat' : DxScartFormat_e, -1:N/A
	HINT32		 nScartPath;				// 'scartPath' : DxScartPath_e, -1:N/A

	// HDMI port
	HINT32		 nHdmiDigitalAudioFormat;	// 'hdmiDigitalAudioFormat' : DxDigitalAudioFormat_e, -1:N/A
	HINT32		 nCecSupport;				// 'cecSupport' : HBOOL, -1:N/A
	HINT32		 nCecOnOff;					// 'cecOn' : HBOOL, -1:N/A
	HINT32		 nHdcpState;				// 'hdcpState' : DxHdcpStatus_e, -1:N/A
	HINT32		 nHdmi3dSelect;				// 'hdmi3dSelect' : DxHdmi3DSelect_e, -1:N/A
	HINT32		 nHdmi3dState;				// 'hdmi3dState' : DxVideo3DFormat_e, -1:N/A
	HINT32		 nEdid3dFormatCap;			// 'edid3dFormatCapability' : DxVideo3DFormat_e, -1:N/A
	HINT32		 nEdidResolCap;				// 'edidResolutionCapability' : DxResolutionStatus_e, -1:N/A

	// SPDIF
	HINT32		 nSpdifSupport;				// 'spdifSupport' : HBOOL, -1: N/A
	HINT32		 nSpdifDigitalAudioFormat;	// 'spdifDigitalAudioFormat' : DxDigitalAudioFormat_e, -1:N/A

	// Tuner-related
	HINT32		 nAntConnectType;			// 'antennaConnectionType' : DxAntennaConnection_e, -1: N/A

} RestSetting_t;

// RestSettingAdminFlag_t 와 RestSetting_t는 반드시 항목별로 sync가 맞아야 한다.
typedef RestSetting_t 	RestSettingAdminFlag_t;


typedef struct
{
	// apk_sysinfo_tuner.c
	HINT32					 nNumTuner;

	// apk_scenario.c
	DxAntennaConnection_e	 eAntConnectType;
} RestTunersStaus_t;

typedef struct
{
	HINT32				 nTunerId;
	DxDeliveryType_e	 eTunerType;			// DxDeliveryType_e. -1:N/A

	HBOOL				 bLocked;
	HINT32				 nSignalLevel;
	HINT32				 nSignalQuality;
	HINT32				 nAgc;
	HFLOAT32			 fBer;
	HFLOAT32			 fSnr;
	HFLOAT32			 fInputPower;
	DxTuneParam_t		 stTuneParam;	
} RestTunerInst_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Member Functions     ********************/
/*******************************************************************/
// bind_datatype.c
extern HERROR	rest_datatype_Init (void);
extern HERROR	rest_datatype_RegisterEnumTable (RestEnumStringTable_t *pstTable);
extern HCHAR*	rest_datatype_ConvertEnum2Str (HCHAR *szEnumType, HINT32 nEnumValue);
extern HERROR	rest_datatype_ConvertStr2Enum (HCHAR *szEnumType, HCHAR *szEnumName, HINT32 *pnEnumValue);
extern INLINE HBOOL	rest_datatype_CheckWriteSelected (HCHAR *szKey, HCHAR *szSelect);
extern HERROR	rest_datatype_GetJsonInteger (HxJSON_t jsData, HCHAR *szKey, HINT32 *pnValue);
extern HERROR	rest_datatype_SetJsonInteger (HINT32 nWriter, HCHAR *szKey, HINT32 nValue, HCHAR *szSelect);
extern HERROR	rest_datatype_GetJsonUnsigned (HxJSON_t jsData, HCHAR *szKey, HUINT32 *pulValue);
extern HERROR	rest_datatype_SetJsonUnsigned (HINT32 nWriter, HCHAR *szKey, HUINT32 ulValue, HCHAR *szSelect);
extern HERROR	rest_datatype_GetJsonString (HxJSON_t *pstJson, HCHAR *szKey, HCHAR *szString, HUINT32 ulStrLen);
extern HERROR	rest_datatype_SetJsonString (HINT32 nWriter, HCHAR *szKey, HCHAR *szString, HCHAR *szSelect);
extern HERROR	rest_datatype_GetJsonEnum (HxJSON_t *pstJson, HCHAR *szKey, HCHAR *szEnumType, HINT32 *pnValue);
extern HERROR	rest_datatype_SetJsonEnum (HINT32 nWriter, HCHAR *szKey, HCHAR *szEnumType, HINT32 nValue, HCHAR *szSelect);
extern HERROR	rest_datatype_SetJsonBitwise (HINT32 nWriter, HCHAR *szKey, HCHAR *szEnumType, HINT32 nValue, HCHAR *szSelect);
extern HUINT8 *	rest_datatype_GetTempBuffer (HUINT32 ulSize);
extern void		rest_datatype_AppendStringBufferToTemporary (HCHAR *szString);

// rest_datatype_channel.c
extern HINT32	rest_datatype_JsonizeServiceWithoutObject (HINT32 nJsonWriter, DxService_t *pstSvc, HCHAR *szSelect);
extern HINT32	rest_datatype_JsonizeService (HINT32 nJsonWriter, DxService_t *pstSvc, RestChanExtInfo_t *pstFlag, HCHAR *szSelect);
extern HERROR	rest_datatype_DejsonizeService (HxJSON_t *pstJson, DxService_t *pstSvc, HBOOL bClearObj);
extern HINT32	rest_datatype_JsonizeLiveStreamInfo (HINT32 nJsonWriter, RestLiveStreamInfo_t *pstLiveStreamInfo, HCHAR *szSelect);
extern HINT32	rest_datatype_JsonizeThumbnailInfo (HINT32 nJsonWriter, RestThumbnailInfo_t *pstThumbnailInfo, HCHAR *szSelect);
extern HINT32	rest_datatype_JsonizeChannelExt (HINT32 nJsonWriter, RestChannelExt_t *pstChanExt, HCHAR *szSelect);
extern HINT32	rest_datatype_JsonizeTransponder (HINT32 nJsonWriter, DxTransponder_t *pstTsInfo, HCHAR *szSelect);
extern HERROR	rest_datatype_DejsonizeTransponder (HxJSON_t *pstJson, DxTransponder_t *pstTsInfo, HBOOL bClearObj);
extern HINT32	rest_datatype_JsonizeTuningParam (HINT32 nJsonWriter, DxTuneParam_t *pstTuneParam, HCHAR *szSelect);
extern HERROR	rest_datatype_DejsonizeTuningParam (HxJSON_t *pstJson, DxTuneParam_t *pstTuneParam, HBOOL bClearObj);
extern HINT32	rest_datatype_JsonizeSatTuningParam (HINT32 nJsonWriter, DxTuneParam_t *pstTuneParam, HCHAR *szSelect);
extern HERROR	rest_datatype_DejsonizeSatTuningParam (HxJSON_t *pstJson, DxSatTuningInfo_t *pstTuneInfo, HBOOL bClearObj);
extern HINT32	rest_datatype_JsonizeCabTuningParam (HINT32 nJsonWriter, DxTuneParam_t *pstTuneParam, HCHAR *szSelect);
extern HERROR	rest_datatype_DejsonizeCabTuningParam (HxJSON_t *pstJson, DxCabTuningInfo_t *pstTuneInfo, HBOOL bClearObj);
extern HINT32	rest_datatype_JsonizeTerTuningParam (HINT32 nJsonWriter, DxTuneParam_t *pstTuneParam, HCHAR *szSelect);
extern HERROR	rest_datatype_DejsonizeTerTuningParam (HxJSON_t *pstJson, DxTerTuningInfo_t *pstTuneInfo, HBOOL bClearObj);
extern HBOOL	rest_datatype_CheckAntennaTuningParamSame (DxAntInfo_t *pstAnt1, DxAntInfo_t *pstAnt2);
extern HBOOL	rest_datatype_CheckSatTuningParamSame (DxSatTuningInfo_t *pstTune1, DxSatTuningInfo_t *pstTune2);
extern HBOOL	rest_datatype_CheckCabTuningParamSame (DxCabTuningInfo_t *pstTune1, DxCabTuningInfo_t *pstTune2);
extern HBOOL	rest_datatype_CheckTerTuningParamSame (DxTerTuningInfo_t *pstTune1, DxTerTuningInfo_t *pstTune2);
extern HBOOL	rest_datatype_CheckTuningParamSame (DxTuneParam_t *pstRfTune, DxTuneParam_t *pstTpTune);
extern HINT32	rest_datatype_JsonizeTpTunable (HINT32 nJsonWriter, RestTsTunable_t *pstTunableArray, HUINT32 ulArrayLen, HCHAR *szObjectName, HCHAR *szSelect);
extern HINT32	rest_datatype_JsonizeAntenna (HINT32 nJsonWriter, DxAntInfo_t *pstAntInfo, HCHAR *szSelect);
extern HERROR	rest_datatype_DejsonizeAntenna (HxJSON_t *pstJson, DxAntInfo_t *pstAntInfo, HBOOL bClearObj);
extern HINT32	rest_datatype_JsonizeLnbAntenna (HINT32 nJsonWriter, DxAntInfo_t *pstAntInfo, HCHAR *szSelect);
extern HERROR	rest_datatype_DejsonizeLnbAntenna (HxJSON_t *pstJson, DxLnbAntenna_t *pstLnbAnt, HBOOL bClearObj);
extern HINT32	rest_datatype_JsonizeDiseqcAntenna (HINT32 nJsonWriter, DxAntInfo_t *pstAntInfo, HCHAR *szSelect);
extern HERROR	rest_datatype_DejsonizeDiseqcAntenna (HxJSON_t *pstJson, DxDiseqcAntenna_t *pstDiseqcAnt, HBOOL bClearObj);
extern HINT32	rest_datatype_JsonizeMotorAntenna (HINT32 nJsonWriter, DxAntInfo_t *pstAntInfo, HCHAR *szSelect);
extern HERROR	rest_datatype_DejsonizeMotorAntenna (HxJSON_t *pstJson, DxDiseqcAntenna_t *pstDiseqcAnt, HBOOL bClearObj);
extern HINT32	rest_datatype_JsonizeScdAntenna (HINT32 nJsonWriter, DxAntInfo_t *pstAntInfo, HCHAR *szSelect);
extern HERROR	rest_datatype_DejsonizeScdAntenna (HxJSON_t *pstJson, DxScdAntenna_t *pstScdAnt, HBOOL bClearObj);
extern HINT32	rest_datatype_JsonizeChanListStatus (HINT32 nJsonWriter, RestChanListStatus_t *pstStatus, HCHAR *szSelect);
extern HERROR	rest_datatype_InitChannel (void);

// rest_datatype_chanscan.c
extern HINT32	rest_datatype_JsonizeScanParams (HINT32 nJsonWriter, RestScanParams_t *punScanStatus, HCHAR *szSelect);
extern HERROR	rest_datatype_DejsonizeScanParams (HxJSON_t jsObject, RestScanParams_t *punScanStatus, HBOOL bClearObj);
extern HINT32	rest_datatype_JsonizeScanParams_None (HINT32 nJsonWriter, RestScanParams_t *punScanStatus, HCHAR *szSelect);
extern HINT32	rest_datatype_JsonizeScanParams_ChannelScan (HINT32 nJsonWriter, RestScanChanParams_t *pstParams, HCHAR *szSelect);
extern HERROR	rest_datatype_DejsonizeScanParams_ChannelScan (HxJSON_t jsObject, RestScanChanParams_t *pstParams, HBOOL bClearObj);
extern HINT32	rest_datatype_JsonizeScanResult (HINT32 nJsonWriter, RestScanResult_t *punScanResult, HCHAR *szSelect);
extern HINT32	rest_datatype_JsonizeScanResult_ChannelScan (HINT32 nJsonWriter, RestScanChanResult_t *pstResult, HCHAR *szSelect);
extern HERROR	rest_datatype_InitScan (void);

// rest_datatype_player.c
extern HINT32	rest_datatype_JsonizePlayerStatus (HINT32 nJsonWriter, RestPlayerStatus_t *pstStatus, HCHAR *szSelect);
extern HINT32	rest_datatype_JsonizePlayerStatus_None (HINT32 nJsonWriter, RestPlayerStatus_t *pstStatus, HCHAR *szSelect);
extern HINT32	rest_datatype_JsonizePlayerStatus_Live (HINT32 nJsonWriter, RestPlayerStatus_t *pstStatus, HCHAR *szSelect);
extern HINT32	rest_datatype_JsonizePlayerStatus_PvrPb (HINT32 nJsonWriter, RestPlayerStatus_t *pstStatus, HCHAR *szSelect);
extern HINT32	rest_datatype_JsonizePlayerStatus_TsrPb (HINT32 nJsonWriter, RestPlayerStatus_t *pstStatus, HCHAR *szSelect);
extern HINT32	rest_datatype_JsonizeMediaComponent (HINT32 nJsonWriter, OxMediaPlay_Component_t *pstComponent, HCHAR *szSelect);
extern HINT32	rest_datatype_JsonizeMediaComponent_Video (HINT32 nJsonWriter, OxMediaPlay_VideoComp_t *pstVideo, HCHAR *szSelect);
extern HINT32	rest_datatype_JsonizeMediaComponent_Audio (HINT32 nJsonWriter, OxMediaPlay_AudioComp_t *pstAudio, HCHAR *szSelect);
extern HINT32	rest_datatype_JsonizeMediaComponent_Subtitle (HINT32 nJsonWriter, OxMediaPlay_SubtitleComp_t *pstSubtitle, HCHAR *szSelect);
extern HINT32	rest_datatype_JsonizeMediaComponent_Data (HINT32 nJsonWriter, OxMediaPlay_DataComp_t *pstData, HCHAR *szSelect);
extern HERROR	rest_datatype_InitPlayer (void);

// rest_datatype_recorder.c
extern HINT32	rest_datatype_JsonizeRecorderInstance (HINT32 nJsonWriter, RestBindRecorderInst_t *pstInst, HCHAR *szObjectName, HCHAR *szSelect);
extern HERROR	rest_datatype_InitRecorder (void);

// rest_datatype_epg.c
extern HINT32 	rest_datatype_JsonizeEvent (HINT32 nJsonWriter, DxEvent_t *pstEvent, HCHAR *szSelect);
extern HERROR	rest_datatype_DejsonizeEvent (HxJSON_t *pstJson, DxEvent_t *pstEvent, HBOOL bClearObj);
extern HERROR	rest_datatype_InitEpg (void);

// rest_datatype_schedule.c
extern HINT32 	rest_datatype_JsonizeSchedule (HINT32 nJsonWriter, DxSchedule_t *pstSchedule, HCHAR *szSelect);
extern HERROR	rest_datatype_DejsonizeSchedule (HxJSON_t jsData, DxSchedule_t *pstSchedule, HBOOL bClearObj);
extern HERROR	rest_datatype_InitSchedule (void);

// rest_datatype_record.c
extern HINT32	rest_datatype_JsonizeRecord (HINT32 nJsonWriter, DxRecListData_t *pstRecData, HCHAR *szSelect);
extern HINT32	rest_datatype_JsonizeRecRequest (HINT32 nJsonWriter, RestRecRequest_t *pstRequest, HCHAR *szObjectName, HCHAR *szSelect);
extern HERROR	rest_datatype_DejsonizeRecRequest (HxJSON_t *pstJson, RestRecRequest_t *pstRequest, HBOOL bClearObj);
extern HERROR	rest_datatype_DejsonizeChannelRecRequest (HxJSON_t *pstJson, RestRecRequest_t *pstRequest, HBOOL bClearObj);
extern HERROR	rest_datatype_InitRecord (void);

// rest_datatype_sysenv.c
extern HERROR	rest_datatype_CleanSysEnv (RestSysEnv_t *pstSysEnv);
extern HINT32	rest_datatype_JsonizeSysEnv (HINT32 nJsonWriter, RestSysEnv_t *pstSysEnv, HCHAR *szObjectName, HCHAR *szSelect);
extern HERROR	rest_datatype_DejsonizeSysEnv (HxJSON_t *pstJson, RestSysEnv_t *pstSysEnv, HBOOL bClearObj);
extern HERROR	rest_datatype_InitSysEnv (void);

// rest_datatype_settings.c
extern HERROR	rest_datatype_CleanSettings (RestSetting_t *pstSetting);
extern HERROR	rest_datatype_CleanAdminSettingFlag (RestSettingAdminFlag_t *pstAdminFlag);
extern HINT32	rest_datatype_JsonizeSettings (HINT32 nJsonWriter, RestSetting_t *pstSetting, HCHAR *szObjectName, HCHAR *szSelect);
extern HERROR	rest_datatype_DejsonizeSettings (HxJSON_t *pstJson, RestSetting_t *pstSetting, HBOOL bClearObj);
extern HINT32	rest_datatype_JsonizeAdminSettingItems (HINT32 nJsonWriter, RestSettingAdminFlag_t *pstAdminFlag, HCHAR *szObjectName, HCHAR *szSelect);
extern HERROR	rest_datatype_InitSettings (void);

// rest_datatype_tuner.c
extern HINT32 	rest_datatype_JsonizeTunerStatus(HINT32 nJsonWriter, RestTunerInst_t *pstStatus, HCHAR *szSelect);
extern HINT32 	rest_datatype_JsonizeTunerOverview(HINT32 nJsonWriter, HINT32 nTunerNum, DxAntennaConnection_e eConType, HCHAR *szSelect);
extern HERROR 	rest_datatype_InitTuner (void);

#endif	// __REST_DATATYPE_H__
