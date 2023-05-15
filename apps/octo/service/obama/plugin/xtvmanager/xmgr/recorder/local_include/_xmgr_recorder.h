/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xmgr_recorder.h
	@brief	  internal player ��� ����

	Description: player internal use only. \n
	Module: XMGR Recorder			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XMGR_RECORDER_INT_H__
#define	__XMGR_RECORDER_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <mgr_recorder.h>

#define	REC_PAUSE_TYPE_SIGNAL	0x40000000	// reserved : No signal �� ���� TS �� ��� pause �� ȿ���� ����. JC-8500BR �� BD �� �̰��� �䱸����.

#define	INSTANT_RECORD_END_TIMER_ID			0x1000		// Instant Recording �� Endtimer
#define	SECOND								1000
#define	PANEL_STRING_TIMER_ID				0x1001		// "  START RECORD  " �� 2�ʰ� ������� �Ѵ�.
#define	PANEL_STRING_TIME_OUT				2000		// 2 ��
#define RECORDING_RGBLED_DIMM_TIMER_ID		0x1002		// Recording ������ ���� �ð� �� RGBLED�� Dimm �ؾ� �Ѵ�.
#define RECORDING_RGBLED_DIMM_10SEC			10000		// 10 ��
#define CIPLUS_RATING_REC_STOP_TIMER_ID		0x3031
#define CIPLUS_RATING_REC_STOP_TIME_OUT		4000		// 4 ��
#define DELAYED_FTA_CMG_TIMER_ID			0xe620
#define DELAYED_FTA_CMG_TIME_OUT			10000		// 10 ��


#define MAX_REC_URL_LEN			512


#define MIN_REQUIRED_RECORD_TIMES			30
#define RECORD_DURATION_CHECK_TIME			10


#if defined(CONFIG_MW_MM_TSR_DURATION)
	#define DEFAULT_DURATION_TSR_RECORD			CONFIG_MW_MM_TSR_DURATION
#else
	#define DEFAULT_DURATION_TSR_RECORD			(5400)	// 90 minutes
#endif


#if	defined(CONFIG_DEBUG)
	#define PVR_MAX_RECV_EVENT_NUM	3			//	Max 1 event per 5 minutes.
#else
	#define	PVR_MAX_RECV_EVENT_NUM	30			//	Max 1 event per 5 minutes.
#endif


typedef enum
{
	eRecCmd_Init,
	eRecCmd_Setup,
	eRecCmd_Start,
	eRecCmd_StartDelayed,
	eRecCmd_Stop,
	eRecCmd_Close,
	eRecCmd_Pause,
	eRecCmd_Resume,
	eRecCmd_StartSI,
	eRecCmd_StartCas,
	eRecCmd_SetDrmInfo,

	eRecCmd_Max
} xmgrRec_ReqCmd_e;

typedef enum
{
	eRecWait_None		= 0x0000,
	eRecWait_PMT		= 0x0001,
	eRecWait_CAS		= 0x0002,
	eRecWait_DRM		= 0x0004,
	eRecWait_ALL		= 0xffff,
} xmgrRec_WaitFlag_e;

typedef enum
{
	eRecPause_None	= 0x0000,
	eRecPause_Start	= 0x0001,
	eRecPause_CAS	= 0x0002,
	eRecPause_DRM	= 0x0004,
	eRecPause_ALL	= 0xffff,
} xmgrRec_PauseFlag_e;



/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#if defined(CONFIG_DEBUG)
#define CHECK_REC_ACTION_HANDLE()	\
	do {																	\
		HUINT32			 ulMsgActionId = SVC_PIPE_GetActionId (hAction);		\
		if (SVC_PIPE_GetActionId (pstContext->hAction) != ulMsgActionId)		\
		{																	\
			return MESSAGE_PASS;											\
		}																	\
											\
		if ( SVC_PIPE_IsActionHandleLatest(hAction) != ERR_OK)				\
		{																	\
			HxLOG_Error("Not the latest action(0x%x). context(0x%x) p1(%d) p2(%d) p3(%d). \n",hAction,pstContext->hAction, p1, p2, p3);	\
		}																	\
																\
	} while(0)

#else
#define CHECK_REC_ACTION_HANDLE()	\
	do {																	\
		HUINT32			 ulMsgActionId = SVC_PIPE_GetActionId (hAction);		\
		if (SVC_PIPE_GetActionId (pstContext->hAction) != ulMsgActionId)		\
		{																	\
			return MESSAGE_PASS;											\
		}																	\
	} while(0)
#endif

// ������ Recording �ϴ� ����. (Record + TSR)
// ���� CONFIG���� ���� �ִ� �ɷ� ��ģ��.

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef	struct
{
	HINT32			nEventId;
	HUINT16			usParentalRating;
	HUINT32			ulStartTime;
} xmgrRec_RecvEventInfo_t;

typedef struct
{
	HUINT32				ulActionId;
	Handle_t			hAction;

	MgrRec_State_e		eRecState;
	SvcRec_PIDList_t	stRecPidInfo;

	HUINT32				ulContentId;
	MgrRec_RecSetup_t	stSetup;
	SvcMeta_Record_t 	stMetaInfo;

	HBOOL				bCasOk;				// CAS���� ��û �㰡
	HINT32				nCasErrCode;		// CAS error code

	HUINT32				ulPlaytime;			//stMetaInfo update ������ ���� �ð� ����

	HBOOL				bPMTValid;
	HBOOL				bCasDrmValid;		// CAS�κ��� Drm&Copy protection ������ detect ��. SI���ٴ� CAS�� �켱 ������ ����.
	HBOOL				bCasDrmNone;		// cas�κ��� Drm ������ ���ٴ� �޽����� ����
 	DxCopyrightControl_t			stNewDrmInfo;		// SI, CAS�� ���� ������ drm info, Split �� drm
	xmgrRec_PauseFlag_e				ePauseFlag;	// pause �� ������ bit flag ��
	xmgrRec_WaitFlag_e				eWaitFlag;	// wait �� ������ bit flag ��

	HINT32				usBaseEventId;		//	recording name�� ���� Base event id.
	UNIXTIME			ulStartTimeFromTimeCode;
	UNIXTIME			ulTsrtoDelayTime;
	HINT32				nRecvEventNum;
	HUINT8				szDelayCopyFileName[FS_MAX_FILE_NAME_LEN];
	HBOOL				bDelayedCopy;

	xmgrRec_RecvEventInfo_t astRecvEventInfo[PVR_MAX_RECV_EVENT_NUM];
	HBOOL				bDestroybyDrm;
	UNIXTIME			ulScrambleStartTime;

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
	HBOOL			bEnableCcRecording; 	//	CI+ 1.3 Spec�� CC ver.2 Recording�� ���Ͽ� ���� Record Start�� �ص� �Ǵ��� �Ǵ��ϱ� ���� */
#endif

#if defined(CONFIG_MW_CAS_CONAX)
	UNIXTIME			ulDrmTimeStamp;				/**< recording time, duration fot TSR*/
	HBOOL				bNotAccessGranted;
#endif

#if defined(CONFIG_DEBUG)
	MgrRec_State_e		eLastRecState;		// for debug log
#endif

//	HBOOL				bSiStarted;
//	HBOOL				bCasStarted;
//	HUINT8				ucEndCondition;
//	HBOOL				bSiDrmValid;
//	DxCopyrightControl_t			stSiDrmInfo;		// SI drm info
//	HBOOL				bStartPauseMode;
//	HUINT32 			ulSourceContentId; //S2D Recording �� Service ������ ���� ���� ���� playback ContentId

} xmgrRec_Context_t;

#define _________PLUGIN_FUNC__________
/* BASE */
HERROR			xmgr_recorder_Init_Base(void);
HCHAR*			xmgr_recorder_GetProcName_Base (HUINT32 ulActionId);
BUS_Callback_t	xmgr_recorder_GetProcFunc_Base (HUINT32 ulActionId);

HUINT32			xmgr_recorder_GetRecorderParam_Base (MgrRec_ParamType_e eType);
HERROR 			xmgr_recorder_SetRecorderParam_Base (MgrRec_ParamType_e eType, HUINT32 ulParam);
HERROR			xmgr_recorder_SetRecSetup_Base (HINT32 nRecIdx, MgrRec_RecSetup_t *pstRecSetup);
HERROR			xmgr_recorder_SetRecordFailReason_Base (HUINT32 ulContentsId, MgrRec_EndReasonType_e eEndReason);
HERROR			xmgr_recorder_GetRecordFailReason_Base (HUINT32 ulContentsId, MgrRec_EndReasonType_e *peEndReason);
HERROR			xmgr_recorder_CreateRecordFailReason_Base (MgrRec_RecSetup_t *pstRecSetup, MgrRec_EndReasonType_e eEndReason, DxSchedule_t *pstSchedule);
HUINT8*			xmgr_recorder_GetRecordPath_Base (HUINT32 ulPvrIdx);
HBOOL			xmgr_recorder_IsTsrLockForDelayedRecord_Base (HUINT32 ulContentId, HINT32	nEventId, HUINT16 nParental);
HBOOL			xmgr_recorder_IsDuringDelayedRecCopy_Base (Handle_t hAction);
HBOOL			xmgr_recorder_IsScrambleToDescrambleRecord_Base (MgrRec_RecInfo_t *pstInfo);
HERROR			xmgr_recorder_FindRecordingSvcInfo_Base (Handle_t hSvc, HUINT32 *pulActionId, MgrRec_RecInfo_t *pstInfo);
HERROR			xmgr_recorder_FindRecordingMasterSvcInfo_Base (Handle_t hMasterSvcHandle, HUINT32 *pulActionId, MgrRec_RecInfo_t *pstInfo);
HERROR			xmgr_recorder_FindTsrRecordingSvcInfo_Base (Handle_t hSvc, HUINT32 *pulActionId, MgrRec_RecInfo_t *pstInfo);
HERROR			xmgr_recorder_GetRecordSvcHandleByContentId_Base (HUINT32 ulContentId, Handle_t *phHandle);
HERROR			xmgr_recorder_GetRecordingSvcInfo_Base (HUINT32 ulActionId, MgrRec_RecInfo_t *pstInfo);
HERROR			xmgr_recorder_GetTsrLimitForUsbFlash_Base (MgrRec_TsrLimitForUsbFlash_t *pstUsbFlash);
HBOOL			xmgr_recorder_IsDelayedRecCopyInProgress_Base(void);
HBOOL			xmgr_recorder_GetEncryptionMode_Base (MgrRec_RecSetup_t *pstSetup);
HERROR			xmgr_recorder_SetRecordSetup_Base (MgrRec_RecSetup_t *pstSetup, SvcRec_Setup_t *pstSvcRecSetup, SvcMeta_Record_t * pstMetaInfo,Handle_t hAction);


/* CI_PLUS */
BUS_Result_t	xmgr_rec_CiPlusProto_Base (xmgrRec_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

/*IRDETO*/
BUS_Result_t	xmgr_rec_IrdetoProtoProc_Base (xmgrRec_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
HBOOL			xmgr_recorder_IrdetoGetEncryptionMode_Base (MgrRec_RecSetup_t *pstSetup);


/* NAGRA */
BUS_Result_t	xmgr_rec_NagraProto_Base (xmgrRec_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

/* CONAX */
BUS_Result_t	xmgr_rec_ConaxProto_Base (xmgrRec_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR			xmgr_recorder_ConaxInit_Base(void);
HERROR 			xmgr_recorder_ConaxSetRecordSetup_Base (MgrRec_RecSetup_t *pstSetup, SvcRec_Setup_t *pstSvcRecSetup, SvcMeta_Record_t * pstMetaInfo,Handle_t hAction);

/* VERIMATRIX */
BUS_Result_t 	xmgr_rec_VerimatrixProto_Base(xmgrRec_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

/* SES */
BUS_Callback_t	xmgr_recorder_GetProcFunc_Ses (HUINT32 ulActionId);

/* Ertelecom */
HERROR			xmgr_recorder_SetRecSetup_Ertelecom(HINT32 nRecIdx, MgrRec_RecSetup_t *pstRecSetup);

/* SES */
BUS_Callback_t	xmgr_recorder_GetProcFunc_Ses (HUINT32 ulActionId);

/* Ukdtt */
BUS_Callback_t	xmgr_recorder_GetProcFunc_Ukdtt (HUINT32 ulActionId);
HCHAR*			xmgr_recorder_GetProcName_Ukdtt (HUINT32 ulActionId);
HERROR			xmgr_recorder_CreateRecordFailReason_Ukdtt (MgrRec_RecSetup_t *pstRecSetup, MgrRec_EndReasonType_e eEndReason, DxSchedule_t *pstSchedule);

/* Tdc */
HERROR			xmgr_recorder_CreateRecordFailReason_Tdc (MgrRec_RecSetup_t *pstRecSetup, MgrRec_EndReasonType_e eEndReason, DxSchedule_t *pstSchedule);


#endif /* __XMGR_RECORDER_INT_H__ */

