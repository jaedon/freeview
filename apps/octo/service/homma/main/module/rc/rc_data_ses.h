 /* ==========================================================================
@ filename :    rc_data_ses.h
@ brief	   : 	Remote Control Data Process application
@ author   :

Copyright (c) 2008 HUMAX Co., Ltd.				\n
All rights reserved.							\n
=============================================================================*/
#ifndef __Rc_freesat_data__
#define __Rc_freesat_data__

//#include <ap_actionmanager.h>

#include <directfb.h>
#include <hapi.h>
#include <common.h>
#include <pl_common.h>

typedef enum
{
	RC_RSVTYPE_RESERVED = 0 ,
	RC_RSVTYPE_WATCHING,
	RC_RSVTYPE_REC_EBR,
	RC_RSVTYPE_REC_EBR_SERIES,
	RC_RSVTYPE_REC_TBR
}SCHEDULEDLIST_RSVTYPE_e;

typedef enum
{
    RC_REPEAT_ONCE = 1,
    RC_REPEAT_DAILY,
    RC_REPEAT_WEEKLY,
    RC_REPEAT_SERIES
}SCHEDULEDLIST_REPEAT_e;

typedef enum
{
	RCLIST_RESULT_RESERVED = 0,
	RCLIST_RESULT_OK = 1,
	RCLIST_RESULT_FAIL_CONFLICT,
	RCLIST_RESULT_FAIL_SERVICE_IS_NOT_FOUND,
	RCLIST_RESULT_FAIL_EVENT_IS_NOT_FOUND,
	RCLIST_RESULT_FAIL_SLOT_ERROR,
	RCLIST_RESULT_FAIL_RESERVATION_IS_NOT_FOUND,
	RCLIST_RESULT_FAIL // 이 값은 Freesat spec에 없는 값으로, 내부에서 잘못 처리해서 생긴 결과이다. 밖으로 보내선 안된다.
}RCLIST_RESULT_e;

typedef enum
{
	RCEVT_STORAGE_PARAM1_HDD = 0,
	RCEVT_STORAGE_PARAM1_USB,
	RCEVT_STORAGE_PARAM1_MAX
} RCEvent_Storage_Param1;

typedef enum
{
	RCEVT_POWER_PARAM1_INTOSTANDBY = 0,
	RCEVT_POWER_PARAM1_STANDBY,
	RCEVT_POWER_PARAM1_OUTOFSTANDBY,
	RCEVT_POWER_PARAM1_ON,
	RCEVT_POWER_PARAM1_NO_PASSIVE_STANDBY,
	RCEVT_POWER_PARAM1_MAX
} RCEvent_POWER_Param1;

typedef enum
{
	RCEVT_TYPE_NONE = 0,
	RCEVT_TYPE_RECEIVER,
	RCEVT_TYPE_PAIRING,
	RCEVT_TYPE_CONTENTS,
	RCEVT_TYPE_SETTINGS,
	RCEVT_TYPE_TRICKMODE,
	RCEVT_TYPE_LOCALE,
	RCEVT_TYPE_CA,
	RCEVT_TYPE_POWER,
	RCEVT_TYPE_MAX
} RCEvent_Type;


typedef enum
{
	RCEVT_SUBTYPE_RESOURCE_NAME,
	RCEVT_SUBTYPE_RESOURCE_PAIR,
	RCEVT_SUBTYPE_RESOURCE_UNPAIR,
	RCEVT_SUBTYPE_OUTPUTS_PROGRAM,
	RCEVT_SUBTYPE_OUTPUTS_PROGRAM_OVERRIDE,
	RCEVT_SUBTYPE_OUTPUTS_APP,
	RCEVT_SUBTYPE_OUTPUTS_SETTINGS_VOLUME,
	RCEVT_SUBTYPE_OUTPUTS_SETTINGS_MUTE,
	RCEVT_SUBTYPE_OUTPUTS_SETTINGS_SUBTITLE,
	RCEVT_SUBTYPE_OUTPUTS_SETTINGS_AD,
	RCEVT_SUBTYPE_OUTPUTS_PROGRAM_LIVE_PAUSE,
	RCEVT_SUBTYPE_OUTPUTS_PROGRAM_PLAYBACK,
	RCEVT_SUBTYPE_LOCALE_TUNER,
	RCEVT_SUBTYPE_LOCALE_POSTCODE,
	RCEVT_SUBTYPE_LOCALE_PLATFORM,
	RCEVT_SUBTYPE_CA,
	RCEVT_SUBTYPE_POWER_STANDBY,
	RCEVT_SUBTYPE_POWER_ON,
	RCEVT_SUBTYPE_MAX
} RCEvent_SubType;

typedef struct  tagRcEventElement
{
	HUINT16			usChangeID;
	RCEvent_Type	eRCEvtType;
	RCEvent_SubType	eRCEvtSubType;
	HCHAR			szUri[256];
	UNIXTIME		ulEvtTime;	
	HUINT32			ulRepeat;
	HINT32			ulParam1;
	HINT32			ulParam2;
	HINT32			ulParam3;
} RCEvent_Element;


#define RCEVT_MAX 18
typedef struct RC_EVENT_TYPE_ELEMENT
{
	HUINT32 	 ulRcEventType;
	HUINT32 	 ulRcEventSubType;
	HUINT8		szEventDesc[256];
}RC_EVENT_TYPE_Element_t;

typedef struct RC_REMOTE_KEYMAP_ELEMENT
{
	HCHAR		*szKeyDesc;
	HUINT32	     ulStbKeyCode;
}RC_REMOTE_KeyMap_Element_t;

#define RC_OUTPUT_MAX 3
typedef struct RC_OUTPUT_TYPE_ELEMENT
{
	HCHAR		*szName;
	HCHAR		*szOid;	
}RC_OUTPUT_TYPE_Element_t;


#define RC_MAX_SERVICE_NAME_LEN			64
#define RC_MAX_STR						256
#define RC_MAX_URL						2048

#define RC_MAX_COMPONENT				8
#define RC_MAX_COMPONENT_DES			64

#define RC_MAX_CA_STR					64


typedef enum
{
	eRCState_Idle			= 0,

	eRCState_ReqStart,
	eRCState_Running,
	eRCState_Stopped,

	eRCState_EndOfCase
} xMgrRCState_e;

typedef struct
{
	HUINT32		ulVolumne;
	HBOOL		bMute;
	HBOOL		bSubtitle;
	HBOOL		bAD;		// Audio description
}RC_Outputs_Setup_t;

typedef enum
{
	eRC_COMPONENT_AUDIO			= 0,
	eRC_COMPONENT_SUBTITLE
}RC_Component_type_e;

typedef struct
{
	RC_Component_type_e			eType;		// audio or subtitle
	HCHAR						szLang[4];
	HCHAR						szDescription[RC_MAX_COMPONENT_DES];
}RC_Outputs_Component_t;


typedef struct
{
	HBOOL						bAvailable;
	HUINT16						usSourceId;				// Definition and management is under the responsibility of the Manufacturer. => use service index
	HINT32						nContentId;				// Definition and management is under the responsibility of the Manufacturer. => use Event Id

	HUINT32						ulComponentNum;
	RC_Outputs_Component_t		stEsInfo[RC_MAX_COMPONENT];
}RC_Outputs_Program_t;

typedef struct
{
	HBOOL		bAvailable;
	HCHAR		szEvtName[RC_MAX_STR];
	HUINT32		ulStartTime;
	HUINT32		ulDuration;
	HCHAR		szDescription[RC_MAX_STR];
}RC_Outputs_EIT_t;

typedef struct
{
	HBOOL		bAvailable;
	HUINT32		ulTimeStamp;
	HUINT32		ulOffset;
	HINT32		nSpeed;
	HUINT32 	ulBufferDuration;
}RC_Outputs_TSR_t;

typedef struct
{
	HBOOL		bAvailable;
	HUINT32		ulCasId;
	HUINT32		ulStatus;
	HUINT32		ulErrorCode;
}RC_Outputs_CAS_t;


typedef struct
{
	HBOOL		bAvailable;
	HUINT32		ulCurPos;
	HINT32		nSpeed;
	HUINT32 	ulDuration;
}RC_Outputs_Playback_t;


typedef enum
{
	eRC_APPS_EMBEDDED			= 0,
	eRC_APPS_NATIVE,
	eRC_APPS_MHEG,
	eRC_APPS_HBBTV,
	eRC_APPS_DIAL,
	eRC_APPS_SATIP,
	eRC_APPS_OTHER,
}RC_Apps_type_e;


typedef struct
{
	HBOOL		bAvailable;
	HCHAR					szSourceId[RC_MAX_URL];	
	HCHAR					szContentId[RC_MAX_URL];
	RC_Apps_type_e		eAppType;
	//HUINT32 				ulStatus;
}RC_Outputs_Apps_t;

typedef struct
{
	RC_Outputs_Setup_t		stSetupInfo;
	RC_Outputs_Program_t	stSvcInfo;
	RC_Outputs_EIT_t		stEitInfo;
	RC_Outputs_TSR_t		stTsrInfo;
	RC_Outputs_Playback_t	stPbInfo;
	RC_Outputs_CAS_t		stCasInfo;
	RC_Outputs_Apps_t		stAppsInfo;
}RC_Outputs_Info_t;


typedef struct
{
	HUINT8		aucCsId[RC_MAX_CA_STR];  		// Chipset unique ID
	HUINT8		aucCsType[RC_MAX_CA_STR]; 		// Chipset type
	HUINT8		aucCsVer[RC_MAX_CA_STR];		// Chipset Version

	HUINT8		aucCaVender[RC_MAX_CA_STR];
	HUINT8		aucCaVer[RC_MAX_CA_STR];		// CAS Version
	HUINT8		aucCaSN[RC_MAX_CA_STR];			// CAS Serial Number

	HBOOL		bScInserted;	// Smartcard inserted	
	HBOOL		bScSuitable;
	HUINT8		aucScType[RC_MAX_CA_STR];
	HUINT8		aucScNUm[RC_MAX_CA_STR];
	HUINT32		ulScStatus;	
}RC_CA_Info_t;


/*
typedef struct
{
	HCHAR					szSourccId[RC_MAX_SERVICE_NAME_LEN];	
	HUINT32					ulLCN;
}RC_Sources_UpdateInfo_t;
*/

typedef struct
{
	HUINT16		usSid;  // Use usSvcIdx
	HUINT16		usLcn;
	HINT8		szSvcName[RC_MAX_SERVICE_NAME_LEN];	
	//HUINT16		usCid;	// Event ID	
}RC_Sources_Info_t;


HERROR Make_RC_XML_Header(HUINT8 *pucUploadBuf,HUINT32 ulTotalBufSize,HUINT32 *ulNewTotalBufSize);
HERROR Get_RC_Outputs(HUINT8 *pucUploadBuf, HUINT8 *command, HUINT32 *ulUploadLen);
HERROR Get_RC_OutputInfo(HUINT8 *pucUploadBuf, HUINT8 *command, HUINT32 *ulUploadLen, void* pOutputInfo);

HERROR Get_RC_Srclists(HUINT8 *pucUploadBuf, HUINT8 *command, HUINT32 *ulUploadLen);
HBOOL  IsExist_RC_Srclist(HUINT8 *pszListId, HBOOL *pbModifiable);

HERROR Get_RC_Sources(HUINT8 *pucUploadBuf, HUINT8 *command, HUINT32 *ulUploadLen, void *pSrcInfo, HINT32 nSrcNum);
HERROR Get_RC_CaInfo(HUINT8 *pucUploadBuf, HUINT8 *command, HUINT32 *ulUploadLen, void *pCaInfo);


HERROR Get_RC_Time(HUINT8 *pucUploadBuf, HUINT32 *ulUploadLen, HUINT32 ulRcvTime);


HERROR Get_RC_Current_Source(HUINT8 *pucUploadBuf,HUINT8 *pUserData1,HUINT32 *ulUploadLen);
HERROR Get_RC_Storage(HUINT8 *pucUploadBuf,HUINT32 *ulUploadLen);
HERROR Get_RC_Current_Storage(HUINT8 *pucUploadBuf,HUINT8 *pUserData1,HUINT32 *ulUploadLen);
HERROR Get_RC_Locale(HUINT8 *pucUploadBuf, HUINT32 *ulUploadLen);
HERROR Get_RC_Remote(HUINT8 *pucUploadBuf, HUINT32 *ulUploadLen);
HERROR Post_RC_Remote(HUINT8* pucBuf, HUINT32 ulContentLen);
HERROR Get_RC_Power(HUINT8 *pucUploadBuf,HUINT32 *ulUploadLen, HBOOL bOperating, HBOOL bEnableActiveStandby, HUINT32 ulStandbyTime);
HERROR Post_RC_Power(HUINT8* pucBuf, HUINT32 ulContentLen, HBOOL bOperating, HBOOL bEnableActiveStandby);

#if 0

HERROR Send_RC_Outputs_Settings(HUINT8* pBody,HUINT32 ulBodyLen);
HERROR Send_RC_Outputs_Programme(HUINT32	reqid,HUINT8* command,HUINT8 *pUserData1);

HERROR Delete_RC_Storage(HUINT8 *pucUploadBuf,HUINT8 *pUserData1);
HERROR Get_RC_Scheduled(HUINT8 *pucUploadBuf,HUINT32 *ulUploadLen);
HERROR Get_RC_Current_Scheduled(HUINT8 *pucUploadBuf,HUINT8 *pUserData1,HUINT32 *ulUploadLen);
HERROR Get_RC_Current_Conflict_Scheduled(HUINT8 *pucUploadBuf,HUINT8 *Param,RSV_Item_t *rsvm_Info,AM_Conflict_Info_t conflict_Info,HUINT32 *ulUploadLen);
HERROR Get_RC_Filtered_Scheduled(HUINT8 *pucUploadBuf,HUINT8 *pUserData1,HUINT32 *ulUploadLen);
RCLIST_RESULT_e Get_RC_Put_Scheduled(HUINT8 *pucUploadBuf,HUINT8* command,HUINT32 commandLen,HUINT32 *ulUploadLen);
HERROR Get_RC_Delete_Scheduled(HUINT8* command,HUINT32 commandLen);
#endif
HERROR Get_RC_Events(HUINT8* pucUploadBuf,HUINT8 *command,HUINT32 *ulUploadLen);
HERROR Send_RC_Events(HUINT8* pBody,HUINT32 ulBodyLen);
HERROR Send_RC_Freetime(HCHAR* pBody,HUINT32 ulBodyLen,HCHAR *pszUrl);
HERROR Dial_Run_Application(HCHAR *pszUrl,HCHAR * pszAppName);
HERROR Dial_Stop_Application(HCHAR *pszUrl,HCHAR * pszAppName);
HBOOL  RemoteCtrl_IsOTARunning(void);

/* For Event logging */
HERROR RemoteCtrl_AppendEventLogs(RCEvent_Type eType,RCEvent_SubType eSubType, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
HERROR RemoteCtrl_FreeAllEventLogs(void);
RCEvent_Element * RemoteCtrl_GetEventLogByChangeId(HUINT16 ulChangeId);
RCEvent_Element * RemoteCtrl_GetNextEventLog(void);
HUINT16 RemoteCtrl_GetLastReqChangeId(void);
void RemoteCtrl_SetLastReqChangeId(HUINT16 ulChangeId);
void RemoteCtrl_SetLastReqRepeat(HUINT32 ulRepeat);

HERROR RemoteCtrl_App_GetData(HUINT32 index, HUINT16 *usFsatId,HCHAR *pszUrl/*HUINT32 index,HCHAR *pszUrl,HCHAR *pszAppName,HUINT32 *state, HBOOL *isReady*/);


#endif

