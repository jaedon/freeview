#ifndef __XMGR_RC_BASE_H__
#define __XMGR_RC_BASE_H__

#define RC_MMGR_MAX_SERVICE_NAME_LEN		64
#define RC_MGR_MAX_STR						256
#define RC_MGR_MAX_URL						2048


#define RC_MGR_MAX_COMPONENT				8
#define RC_MGR_MAX_COMPONENT_DES			64

#define	RC_MGR_MAX_CA_STR					64




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
}xMgrRC_Outputs_Setup_t;

typedef enum
{
	eRC_COMPONENT_AUDIO			= 0,
	eRC_COMPONENT_SUBTITLE
}xMgrRC_Component_type_e;

typedef struct
{
	xMgrRC_Component_type_e		eType;		// audio or subtitle
	HCHAR						szLang[4];
	HCHAR						szDescription[RC_MGR_MAX_COMPONENT_DES];
}xMgrRC_Outputs_Component_t;


typedef struct
{
	HBOOL						bAvailable;
	HUINT16						usSourceId;	// Definition and management is under the responsibility of the Manufacturer. => use service index
	HINT32						nContentId;		// Definition and management is under the responsibility of the Manufacturer. => use Event Id

	HUINT32						ulComponentNum;
	xMgrRC_Outputs_Component_t	stEsInfo[RC_MGR_MAX_COMPONENT];
}xMgrRC_Outputs_Program_t;

typedef struct
{
	HBOOL		bAvailable;
	HCHAR		szEvtName[RC_MGR_MAX_STR];
	HUINT32		ulStartTime;
	HUINT32		ulDuration;
	HCHAR		szDescription[RC_MGR_MAX_STR];
}xMgrRC_Outputs_EIT_t;

typedef struct
{
	HBOOL		bAvailable;
	HUINT32		ulTimeStamp;
	HUINT32		ulOffset;
	HINT32		nSpeed;
	HUINT32 	ulBufferDuration;
}xMgrRC_Outputs_TSR_t;

typedef struct
{
	HBOOL		bAvailable;
	HUINT32		ulCasId;
	HUINT32		ulStatus;
	HUINT32		ulErrorCode;
}xMgrRC_Outputs_CAS_t;


typedef struct
{
	HBOOL		bAvailable;
	HUINT32		ulCurPos;
	HINT32		nSpeed;
	HUINT32 	ulDuration;
}xMgrRC_Outputs_Playback_t;


typedef enum
{
	eRC_APPS_EMBEDDED			= 0,
	eRC_APPS_NATIVE,
	eRC_APPS_MHEG,
	eRC_APPS_HBBTV,
	eRC_APPS_DIAL,
	eRC_APPS_SATIP,
	eRC_APPS_OTHER,
}xMgrRC_Apps_type_e;


typedef struct
{
	HBOOL					bAvailable;
	HCHAR					szSourceId[RC_MGR_MAX_URL];	
	HCHAR					szContentId[RC_MGR_MAX_URL];
	xMgrRC_Apps_type_e		eAppType;
	//HUINT32 				ulStatus;
}xMgrRC_Outputs_Apps_t;

typedef struct
{
	xMgrRC_Outputs_Setup_t		stSetupInfo;
	xMgrRC_Outputs_Program_t	stSvcInfo;
	
	xMgrRC_Outputs_EIT_t		stEitInfo;
	xMgrRC_Outputs_TSR_t		stTsrInfo;
	xMgrRC_Outputs_Playback_t	stPbInfo;
	xMgrRC_Outputs_CAS_t		stCasInfo;
	xMgrRC_Outputs_Apps_t		stAppsInfo;
}xMgrRC_Outputs_Info_t;


typedef struct
{
	HUINT16		usSid;  // Use usSvcIdx
	HUINT16		usLcn;
	HINT8		szSvcName[RC_MMGR_MAX_SERVICE_NAME_LEN];	
	//HUINT16		usCid;	// Event ID	
}xMgrRC_Sources_Info_t;



typedef struct
{
	HUINT8		aucCsId[RC_MGR_MAX_CA_STR];  		// Chipset unique ID
	HUINT8		aucCsType[RC_MGR_MAX_CA_STR]; 		// Chipset type
	HUINT8		aucCsVer[RC_MGR_MAX_CA_STR];		// Chipset Version

	HUINT8		aucCaVender[RC_MGR_MAX_CA_STR];
	HUINT8		aucCaVer[RC_MGR_MAX_CA_STR];		// CAS Version
	HUINT8		aucCaSN[RC_MGR_MAX_CA_STR];			// CAS Serial Number

	HBOOL		bScInserted;	// Smartcard inserted	
	HBOOL		bScSuitable;
	HUINT8		aucScType[RC_MGR_MAX_CA_STR];
	HUINT8		aucScNUm[RC_MGR_MAX_CA_STR];
	HUINT32		ulScStatus;	
}xMgrRC_CA_Info_t;


typedef struct
{	
	xMgrRCState_e				eState;	
	BUS_Callback_t				pfActionProc;

	HUINT32						ulActionId;
	xMgrRC_Outputs_Info_t		stOutputInfo;	
}xMgrRCContext_t;

HERROR			xmgr_rc_Init_BASE (void);
HERROR			xmgr_rc_Start_BASE (void);
//HERROR			xmgr_rc_ChangePids_BASE (HUINT32 ulActionId, HINT32 nPidsCount, HUINT16 *pusPids);
HERROR			xmgr_rc_Stop_BASE (void);

#endif /* __XMGR_RC_BASE_H__ */
