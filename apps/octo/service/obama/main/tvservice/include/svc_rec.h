/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_rec.h
	@brief

	Description:  									\n
	Module: SVC / REC								\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
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

#ifndef __SvcRec__
#define __SvcRec__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <db_svc.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define SvcRec_MAX_NUM_RECORD					PAL_MAX_REC_NUM


#define SvcRec_MAX_VIDEO_ES_NUM				2
#define SvcRec_MAX_AUDIO_ES_NUM				16
#define SvcRec_MAX_SUBTTL_ES_NUM				12
#define SvcRec_MAX_TELTXT_ES_NUM				8
#define SvcRec_MAX_CA_PID_NUM					20
#define SvcRec_MAX_CAPTION_ES_NUM				2	/* Super 포함 */
#define SvcRec_MAX_DATA_ES_NUM					12	/* TR-B14 제3편 2.1.2.5 ES수의 제한 */

#define MAX_FILE_LEN					256 // 'FS_MAX_FILE_NAME_LEN'
#define MAX_TITLE_LEN					256

/*******************************************************************/
/********************      Data Type (enum)    *********************/
/*******************************************************************/
typedef enum
{
	eSEVT_REC_				= eSEVT_REC_START,

	/* events comes from Recorder */
	eSEVT_REC_TIMECODE_CHANGE,
	eSEVT_REC_DELAYED_RECORD_STATUS_CHANGE,
	eSEVT_REC_SCRAMBLED,
	eSEVT_REC_REQ_COMMAND,
	eSEVT_REC_PID_CHANGED,
	eSEVT_REC_NO_VIDEO_IN_PMT

}SvcRec_EvtMsg_e;

typedef enum
{
	eRec_ActType_NONE = 0,

	eRec_ActType_Record,
	eRec_ActType_Tsr_Record,
	eRec_ActType_Delayed_Record,
	eRec_ActType_Descramble_Record,
	eRec_ActType_Transcode,
	eRec_ActType_Satip,
	eRec_ActType_Ntls,
    eRec_ActType_FullTs,

	eRec_ActType_EndOfType
} SvcRec_ActType_e;

typedef enum
{
	eRec_OutType_NONE = 0,

	eRec_OutType_File,
	eRec_OutType_MemoryBuf,

	eRec_OutType_EndOfType
} SvcRec_OutType_e;


typedef struct
{
	HUINT32		ulStartTime;
	HUINT32		ulRecTime;
	HUINT32		ulEndTime;

	HUINT32		ulDuration;

} SvcRec_RecInfo;

typedef enum
{
	eREC_SiPAT = 0,
	eREC_SiPMT,
	eREC_SiPCR,
	eREC_SiCAT,
	eREC_SiEIT,
	eREC_SiSDT,
	eREC_SiSIT,
	eREC_SiAIT,
	eSvcRec_MAX_REC_NUM
} SvcRec_SI_e;


#define __MEMBER__
typedef enum
{
	eSvcRec_CtxtState_None = 0,
	eSvcRec_CtxtState_Waiting,
	eSvcRec_CtxtState_Running
} svcRec_ContextState_e;

typedef enum
{
	/* Session */
	eRECMSG_CMD_SETUP,	//SESSION OPEN
	eRECMSG_CMD_SET_PID,
	eRECMSG_CMD_START,

	eRECMSG_CMD_PAUSE,
	eRECMSG_CMD_RESUME,

	eRECMSG_CMD_STARTDELAYED,

	eRECMSG_CMD_STOP,
	eRECMSG_CMD_CLOSE,	//SESSION CLOSE

	eRECMSG_CMD_UPDATE_VIDEOCODEC,
	eRECMSG_CMD_SET_ENCRYPTION,

	/** DD -> SVC */
	eRECMSG_CMD_EVT_RECORD,

	eRECMSG_CMD_END

}svcRec_MsgType_e;

typedef enum
{
	eRECEVT_TIMECODE,
	eRECEVT_UNSCRAMBLED,
	eRECEVT_SCRAMBLED,
	eRECEVT_DELAYEDREC_CHANGED,
	eRECEVT_DISK_FULL,

	eRECEVT_END
}svcRec_EvtType_e;

typedef enum
{
	eRECMSG_ARGTYPE_None,

	eRECMSG_ARGTYPE_ApiResultPtr,
	eRECMSG_ARGTYPE_SyncMode,
	eRECMSG_ARGTYPE_PlugIn,

	eRECMSG_ARGTYPE_Setup,
	eRECMSG_ARGTYPE_SetPid,
	eRECMSG_ARGTYPE_SetupDelayed,
	eRECMSG_ARGTYPE_SetEncryption,

	eRECMSG_ARGTYPE_RecEvtParam,

	eRECMSG_ARGTYPE_UpdateVideoCodec,

	eRECMSG_ARGTYPE_Unknown
}svcRec_MsgArgType_e;

typedef enum
{
	eRecResType_RecordId,
	eRecResType_DelayedId,

	eRecResType_Unknown,
}svcRec_ResType_e;

typedef enum
{
	eRec_PidSetType_Unknown			= 0,
	eRec_PidSetType_Known,
	eRec_PidSetType_All,

	eRec_PidSetType_EndOfCase
} SvcRec_PidSetType_e;

#define __STRUCT__
typedef struct
{
	HUINT32		ulTsrDurationTime;
	HUINT32		ulStartTime;
	HUINT32		ulRecTime;
	HUINT32		ulEndTime;
	HBOOL		bEncryption;
	HBOOL		bUseUserEncKey;
	HUINT8		aucKey[PAL_PVR_CRYPTO_KEY_LEN];
	HBOOL		bSplit;					/* inform that contents should be split by each event  */
	HUINT32		ulInstanceId;			/* copy process의 instance id */

	HUINT8		aucPath[MAX_FILE_LEN];
	HUINT8		aucFileName[MAX_FILE_LEN];
	HUINT8		aucDisplayEvtName[MAX_TITLE_LEN];

//	HUINT32		ulPvrIdx;
//	Handle_t	hCurrSvc;
} SvcRec_DelayedRecSetup_t;


typedef struct
{
	SvcRec_ActType_e	eRecActType;
	SvcRec_OutType_e	eRecOutType;

	Handle_t			hSvc;
	Handle_t			hMasterSvc;
	Handle_t			hPBAction;
	DxSvcType_e			eSvcType;
	HBOOL				bEncryption;
	HBOOL				bUseUserEncKey;
	HUINT8			 	aucKey[PAL_PVR_CRYPTO_KEY_LEN];
	DxVideoCodec_e		eVideoCodec;

	UNIXTIME			ulStartTime;

	union
	{
		/*memorybuf*/
		Handle_t	hMemoryBuf;

		/*file*/
		struct
		{
			HUINT8	aucPath[MAX_FILE_LEN];
			HUINT8	aucFileName[MAX_FILE_LEN];
			HUINT8	aucDisplayEvtName[MAX_TITLE_LEN];
		}stFile;
	} unOutArg;

	struct {
		HBOOL			bIsUsbFlashRecord;
		HUINT32 		ulLimitTimeSecond;
		HUINT64 		ullLimitDataSize;
		HUINT64 		ullLimitIndexSize;
	} stUsbFlash;

} SvcRec_Setup_t;

typedef struct
{
	HUINT32				ulRequestId;
	HUINT16				usPid;
} SvcRec_PIDDes_t;

typedef union
{
	SvcRec_PidSetType_e	 ePidSetType;

	struct
	{
		SvcRec_PidSetType_e ePidSetType;
		SvcRec_PIDDes_t		stSI[eSvcRec_MAX_REC_NUM];
		SvcRec_PIDDes_t		stVideo[SvcRec_MAX_VIDEO_ES_NUM];
		SvcRec_PIDDes_t		stAudio[SvcRec_MAX_AUDIO_ES_NUM];
		SvcRec_PIDDes_t		stSubtitle[SvcRec_MAX_SUBTTL_ES_NUM];
		SvcRec_PIDDes_t		stTeletext[SvcRec_MAX_TELTXT_ES_NUM];
		SvcRec_PIDDes_t		stCaption[SvcRec_MAX_CAPTION_ES_NUM];
		SvcRec_PIDDes_t		stEcm[SvcRec_MAX_CA_PID_NUM];
		SvcRec_PIDDes_t		stEmm[SvcRec_MAX_CA_PID_NUM];
		SvcRec_PIDDes_t		stData[SvcRec_MAX_DATA_ES_NUM];
	}stKnown;

	struct
	{
		SvcRec_PidSetType_e  ePidSetType;
		SvcRec_PIDDes_t stPIDs[DxRSV_PIDTABLE_COUNT];
	}stUnknown;

} SvcRec_PIDList_t;

typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;

	SvcRec_Setup_t	 stSetup;

} SvcRec_Arg_Setup_t;

typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;

	SvcRec_PIDList_t stPIDList;

} SvcRec_Arg_SetPid_t;

typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;

	SvcRec_DelayedRecSetup_t stDelayedSetup;

} SvcRec_Arg_DelayedSetup_t;

typedef struct
{
	svcRec_EvtType_e	eType;
	HUINT32				ulParam1;
	HUINT32				ulParam2;
	HUINT32				ulParam3;

} SvcRec_EvtParam_t;

typedef struct
{
	HBOOL			 bSyncMode;
	HERROR			*phResult;

} SvcRec_Arg_Base_t;

typedef struct
{
	SvcRec_EvtParam_t	stEvtParam;

} SvcRec_Arg_EvtParam_t;

typedef	struct
{
	svcRec_MsgType_e eMsgClass;
	Handle_t		 hAction;

	union
	{
		SvcRec_Arg_Setup_t 	 		stSetup;
		SvcRec_Arg_SetPid_t	 		stPIDList;
		SvcRec_Arg_DelayedSetup_t	stDelayedSetup;

		SvcRec_Arg_Base_t	 		stBase;

		SvcRec_Arg_EvtParam_t		stEvtParam;

	} unArg;
} SvcRec_Msg_t;


typedef void (*SvcRec_ActFunc) (SvcRec_Msg_t *pstMsg, void *pvData);

/* lifecycle */
HERROR SVC_REC_Init( PostMsgToMgrCb_t fnPostMsg  );
HERROR SVC_REC_Setup( Handle_t hAction, SvcRec_Setup_t *pstSetup, HBOOL bSync );
HERROR SVC_REC_UpdateVideoCodec( Handle_t hAction, SvcRec_Setup_t *pstSetup, HBOOL bSync );
HERROR SVC_REC_SetEncryption( Handle_t hAction, SvcRec_Setup_t *pstSetup, HBOOL bSync);
HERROR SVC_REC_SetPID( Handle_t hAction, SvcRec_PIDList_t *pstPIDList, HBOOL bSync );
HERROR SVC_REC_Start( Handle_t hAction, HBOOL bSync );
HERROR SVC_REC_Stop( Handle_t hAction, HBOOL bSync );
HERROR SVC_REC_StartDelayed( Handle_t hAction, SvcRec_DelayedRecSetup_t *pstSetupInfo, HBOOL bSync );
HERROR SVC_REC_Close( Handle_t hAction, HBOOL bSync );
HERROR SVC_REC_Pause( Handle_t hAction, HBOOL bSync );
HERROR SVC_REC_Resume( Handle_t hAction, HBOOL bSync );

/**/
HERROR SVC_REC_ReplacePSI( Handle_t hAction, HUINT16 usPid, HUINT8 *pucSection, HUINT32 ulSectionSize );
HERROR SVC_REC_CleanReqPidInfo( SvcRec_PidSetType_e ePidSetType, SvcRec_PIDList_t *pstReqInfo );
HERROR SVC_REC_CleanReqPidDes (SvcRec_PIDDes_t stPidDesArray[], HINT32 nArrayCnt );
HERROR SVC_REC_GetInfo(Handle_t hAction, SvcRec_RecInfo *pstRecInfo);
HBOOL  SVC_REC_IsRecording( HUINT8 *pucFileName );
HBOOL  SVC_REC_IsAvailableDelayed( Handle_t hAction );
HERROR SVC_REC_SetDRMInfo( Handle_t hAction, DxCopyrightControl_t *pstInfo, HBOOL bWriteTimeStamp );

#define __PLUGIN_MEMBER__
HERROR svc_rec_GetMsgArg (SvcRec_Msg_t *pstMsg, svcRec_MsgArgType_e eArgType, void *pvData);
#if	defined(CONFIG_DEBUG)
void SVC_REC_DebugPrintRecPidInfo_Base(SvcRec_PIDList_t *pstPIDList, const HCHAR *funcName, HINT32 nLine);
#endif

#endif	// __SvcRec__
/* end of file */
