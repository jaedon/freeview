/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _svc_cas_mgr_main.h						\n
	@brief	  cas module header file		\n

	Description: cas module. \n
	Module: MW/CAS									 					\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__CAS_MGR_MAIN_H__
#define	__CAS_MGR_MAIN_H__

/********************************************************************
* Header Files : ��� ������ ������ .c ���� include �� �� !
********************************************************************/
#define ___01_Header_Files___

#include <_svc_cas_mgr_common.h>

/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___02_Extern_Vars_Func_Prototypes___

/********************************************************************
* Module Internal Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Module_Internal_Definitions___

#define CAS_INST_CONNECT			TRUE
#define CAS_INST_DISCONNECT			FALSE

#define ____Message______________________________

typedef enum
{
	/*
	ä�� ��ȯ�� �Ϲ����� ������ :
	CMD_CAS_STOP -> CMD_CAS_START -> CMD_CAS_SETPID -> CMD_CAS_CHECKPMT / CMD_CAS_CHECKSDT
	*/

	CMD_CAS_START = 1,				/* WTV���� Start Session �� */
	CMD_CAS_STOP,					/* WTV���� Stop Session �� */

	CMD_CAS_SETPID, 				/* Start : PID Setting. */

	CMD_CAS_CHECKEVENT, 			/* Check Event. CI or Embedded CAS Send Descramble Result */
	CMD_CAS_CHECKDEVICE,			/* SMART CARD or CAM Device Updated. */

	CMD_CAS_CHECKPAT,				/* SI Update �� */
	CMD_CAS_CHECKPMT,
	CMD_CAS_CHECKCAT,
	CMD_CAS_CHECKSDT,
	CMD_CAS_CHECKNIT,
	CMD_CAS_CHECKBAT,
	CMD_CAS_CHECKEIT,
	CMD_CAS_CHECKTDT,

	CMD_CAS_CHECKSTREAM,			/* Check Stream. Stream�� State�� ���� ��. �ʿ��� ��� instance ����. */
	CMD_CAS_CHECKDRM,
	CMD_CAS_ETCEVENT, 			/* CAS���� Application���� �����ϴ� �Ϲ�ȭ ���� �ʴ� �پ��� EVENT�� ������ ��� ��� */

	CMD_CAS_CHECKMMI,				/* MMI ���� */

	CMD_CAS_SC_PPVREPLY,			/* PPV ���� ��û�� ���� ��� */
	CMD_CAS_SC_PINCODEREPLY, 	/* Pincode ���� ��û�� ���� ��� */
	CMD_CAS_SC_UDATAREADREPLY,	/* UserData Read ��û�� ���� ��� */

	CMD_CAS_NOTIREC, 			/* Recording ���� ���� Notify */

	CMD_CAS_MULTISCREEN,	/* For SES Multiscreen */

	CMD_CAS_MAX
} SvcCas_Command_e;

/* �Ķ���ʹ� ���� command�� �ʿ��� �޽��� �Ϲ�. */
typedef struct
{
	SvcCas_Command_e		command;
	Handle_t				hAction;
} SvcCas_CommandMsg_t;

/* 1 Parameter �޽��� �Ϲ�. */
typedef struct
{
	SvcCas_Command_e		command;
	Handle_t				hAction;
	HUINT32				param;
	HUINT32				version;
} SvcCas_SimpleMsg_t;

typedef struct
{
	SvcCas_Command_e		command;
	Handle_t				hAction;
	Handle_t				hSvc;
	SvcCas_ActionType_e		actionType;
	SvcCas_StartData_t		stCasStartData;
} SvcCas_StartMsg_t;

typedef struct
{
	SvcCas_Command_e		command;
	Handle_t				hAction;
	Handle_t				hNextSvc;
	ApiSyncMode_t			syncMode; /**< Command�� ������ API�� ���� ������ sync/async�� ���� */
} SvcCas_StopMsg_t;

typedef struct
{
	SvcCas_Command_e		command;
	Handle_t				hAction;
} SvcCas_PauseMsg_t;

typedef struct
{
	SvcCas_Command_e		command;
	Handle_t				hAction;
} SvcCas_ResumeMsg_t;

typedef struct
{
	SvcCas_Command_e		command;
	Handle_t				hAction;
	SvcCas_ActionType_e		actionType;
	SvcCas_SetPidInfo_t		stEsPidInfo;
} SvcCas_SetPidMsg_t;

typedef struct
{
	SvcCas_Command_e		command;
	Handle_t				hAction;
	SvcCas_NotiType_e		notiType;
	HUINT32					notiParam;
} SvcCas_CheckNotifyMsg_t;								/* Device�� Notify Evt */

typedef struct
{
	SvcCas_Command_e		 command;
	Handle_t				 hAction;
	DxCAS_GroupId_e		 	 eCasId;
	SvcCas_MsgType_e 		 eMsgType;
	void 					*pMsg;
	SvcCas_ShowType_e	 	 eShowType;
} SvcCas_CheckSubCasMsg_t;

typedef struct
{
	SvcCas_Command_e		command;
	Handle_t				hAction;
	HUINT32					ulFilterId;
	HUINT16					usUniqueId;
	SvcCas_SiTableState_e	eSiTableState;
} SvcCas_CheckSiMsg_t;

typedef struct
{
	SvcCas_Command_e		command;
	Handle_t				hAction;
	SvcCas_ActionType_e		actionType;
	HUINT16					usVideoPid;
	HUINT16					usAudioPid;
	HUINT16					usAuxPid;
	HUINT16					usDolbyPid;
	HUINT16					usTeletextPid;
	HUINT16					usSubtitlePid;
} SvcCas_SetLivePidMsg_t;

typedef struct
{
	SvcCas_Command_e		command;
	SvcCas_MmiDataType_e	eDataType;
	HUINT16					usSlotId;
	HUINT32					ulMmiVer;
} SvcCas_CheckMmiMsg_t;

typedef enum
{
	eCAS_DEV_EVT_CamState,
	eCAS_DEV_EVT_CamAppInfo,		// CIv1/CI+ Cam�� App Info
	eCAS_DEV_EVT_CamCasInfo,		// CIv1/CI+ Cam�� Cas Info
	eCAS_DEV_EVT_CamPlusInfo,		// CI+ Cam�� Cc Info (CONFIG_MW_CI_PLUS)

	eCAS_DEV_EVT_ScState,			// smart card�� status ����.
	eCAS_DEV_EVT_ScInfo,				// sub cas�� smart card registration.

	eCAS_DEV_EVT_CasInfo			// sub cas�� cas info�� update �� ���.
} SvcCas_DevEvtType_e;

typedef struct
{
	HUINT32					ulMaxNumSvc;
	HUINT32					ulNumCaSysId;
	HUINT16					ausCaSysId[CAS_MAX_NUM_OF_CA_SYSID];
	HBOOL					bAvailable;
} SvcCas_CasInfo_t;

typedef struct
{
	SvcCas_Command_e		command;
	SvcCas_DevEvtType_e		eEvtType;
	DxCAS_GroupId_e			eCasGroupId;
	HUINT16					usSlotId;
	HUINT32 				ulCaInstanceId;

	union
	{
		SvcCas_CasInfo_t	stCasInfo;
		SvcCas_CiAppInfo_t	stCasCiAppInfo;
		SvcCas_CamState_e	eCamState;
	} utData;

} SvcCas_CheckDeviceMsg_t;


typedef struct
{
	SvcCas_Command_e		command;
	HUINT32					ulCasInstIdx;
	HUINT32					param;
	HUINT32					version;
} SvcCas_CheckInstEvtMsg_t;

typedef struct
{
	SvcCas_Command_e		command;
	Handle_t 				hAction;
	Handle_t				ulCaInstIdx;
	DxCopyrightControl_CasCcType_e			eDrmType;
	void					*pData;
	HUINT32					version;
} SvcCas_CheckDrmInfo_t;

typedef struct
{
	SvcCas_Command_e		command;
	Handle_t 				hAction;
	Handle_t				ulCaInstIdx;
	DxCAS_GroupId_e			eCasId;
	HINT32 					msg;
	HINT32 					p1;
	HINT32 					p2;
	HINT32 					p3;
} SvcCas_EtcEvent_t;

typedef struct
{
	SvcCas_Command_e			command;
	Handle_t 					hAction;
	HUINT32						usCASGroup;
	SvcCas_SmartcardReplyData_t	*pstReplyData;
} SvcCas_SmartCardReplyMsg_t;


typedef struct
{
	SvcCas_Command_e			command;
	Handle_t					hAction;
	DxCAS_GroupId_e				eCasId;
	// TODO: Status �� ���� �� CI������ �ƴ϶� ��� CAS���� �������� �� �� �ִ� recording status noti ���ִ� ������ ���� �ϴ� unsigned char�� �ӽ� ���� ��.
	//CAS_RecStatus_t			eRecStatus;
	HUINT8						ucRecStatus;
	HUINT16 					usSlotId;
} SvcCas_NotifyRecordOperationMsg_t;


/* make union with all of the message types */
typedef union
{
	SvcCas_Command_e				command;
	SvcCas_CommandMsg_t				commandMsg;
	SvcCas_SimpleMsg_t				simple;
	SvcCas_StartMsg_t				start;
	SvcCas_StopMsg_t				stop;
	SvcCas_SetPidMsg_t				setPid;
	SvcCas_CheckDeviceMsg_t			checkDev;
	SvcCas_CheckNotifyMsg_t			checkNotify;
	SvcCas_CheckSiMsg_t				checkSiTbl;
	SvcCas_CheckMmiMsg_t			checkMmi;
	SvcCas_CheckInstEvtMsg_t		checkInst;
	SvcCas_CheckSubCasMsg_t			checkMsg;
	SvcCas_CheckDrmInfo_t			checkDrm;
	SvcCas_EtcEvent_t				etcEvent;
	SvcCas_SmartCardReplyMsg_t		checkSCReply;
	SvcCas_NotifyRecordOperationMsg_t	notifyRecOperation;
} SvcCas_Message_t;

#define ____Action_Context______________________________

typedef struct
{
	HUINT32							ulDescNum;
	SvcCas_CaDescriptor_t			stDesc[CAS_MAX_NUM_OF_CAPID];
} SvcCas_CatInfo_t;

// Action Context
typedef struct
{
	HBOOL					bConnect;				//Instance�� ��� ����.
	SvcCas_State_e 			state;					/* CAS State */

// Action�� ����.
	HUINT32 				ulActionId;
	Handle_t 				hAction;					/* Input Action Handle */
	SvcCas_ActionType_e		eActType;

//serviced�� ����
	Handle_t 				hSvc;
	Handle_t				hNextSvc;
	HUINT16 				usOrgNetId;
	HUINT16 				usTsId;
	HUINT16					usSvcId;
	HUINT16					usUniqueId;
	SvcCas_ChannelType_e 	eChType;
	SvcCas_SetPidInfo_t 	stCaDscInfo;				// ������ Pid ����.

	HUINT32					ulDemuxId;

	HBOOL					bStarted;				//sub cas���� session start�� �˸� ����
	HBOOL					bDscStarted;			//sub cas���� descramble start�� ������ ���. set pid�� �� ����
	HBOOL					bSendCasMsg2Ap;		//ap���� CAS OK, FAIL �޽����� �������� �ִ°�?

	HUINT16					usCurCaSysId;			// Descramble ���� Ca System Id.
	HUINT32					ulCasInstanceId;			// Descramble�� ���ؼ� �Ҵ���� Instance Id, Id�� �˸� Device Type�� Slot Id�� �˼� �ִ�.
	HBOOL					abTryCasInst[CAS_MAX_NUM_OF_INSTANCE+1]; // �ش� CasInst�� Descramble�� �õ��� �ô°�?	// +1 : to avoid illegal zero-sized array for non CAS model.

	SvcCas_StartData_t		stCasStartData;

/* SI ���� */
	SIxTextEncoding_e		eTextEnc;
	SIxSpec_e			 	eSiSpec;

	// PAT
	HUINT32					ulPatFilterId;
	HBOOL					bPatUpdated;
	SvcCas_SiTableState_e	ePatState;

	// PMT
	HUINT32 				ulPmtFilterId;
	HUINT16 				usPmtPid;
	HBOOL					bPmtUpdated;
	SvcCas_PmtInfo_t		stPmtInfo;				/* Parsed PMT Info */
	SvcCas_SiTableState_e	ePmtState;
	HUINT8					aucPmtRaw[CAS_MAX_SECTION_LENGTH];

	// CAT
#if defined (CAS_SUPPORT_CAT)
	HUINT32 				ulCatFilterId;			// cat filter id
	HBOOL					bCatUpdated;
	SvcCas_CatInfo_t		stCatInfo;
	SvcCas_SiTableState_e	eCatState;
	HUINT8					aucCatRaw[CAS_MAX_SECTION_LENGTH];
#endif

	// SDT
#if defined (CAS_SUPPORT_SDT)
	HUINT32 				ulSdtFilterId;
	HBOOL					bSdtUpdated;
	SvcCas_SiTableState_e	eSdtState;
	HUINT8					aucSdtRaw[CAS_MAX_SECTION_LENGTH];	// SDT Raw data is cannot exceed 1024 byte (by EN 300 468 specification)
#endif

	// NIT
#if defined(CAS_SUPPORT_NIT)
	HUINT32 				ulNitFilterId;			// nit filter id
	HBOOL					bNitUpdated;
	HBOOL					bNotTpChanged;
	SvcCas_SiTableState_e	eNitState;
	HUINT8					*aucNitRaw[CAS_MAX_SECTION_NUM];
#endif

	// BAT
#if defined(CAS_SUPPORT_BAT)
	HUINT32 				ulBatFilterId;			// Bat filter id
	HBOOL					bBatUpdated;
	SvcCas_SiTableState_e	eBatState;
	HUINT8					*aucBatRaw[CAS_MAX_SECTION_NUM];
#endif

	// EIT
#if defined(CAS_SUPPORT_EIT)
	HUINT32 				ulEitFilterId;				// eit filter id
	HBOOL					bEitUpdated;
	SvcCas_SiTableState_e	eEitState;
	HUINT8					*aucEitRaw[CAS_MAX_SECTION_NUM];
#endif

	// TDT
#if defined(CAS_SUPPORT_TDT)
	HUINT32 				ulTdtFilterId;			// tdt filter id
	HBOOL					bTdtUpdated;
	SvcCas_SiTableState_e	eTdtState;
	HUINT8					aucTdtRaw[CAS_MAX_SECTION_LENGTH];
#endif

} SvcCas_Context_t;

/********************************************************************
* Module Internal Function Prototypes
********************************************************************/
#define ___04_Module_Internal_Func_Prototypes___

HERROR svc_cas_MgrInit(void);
HULONG svc_cas_MgrGetMsgQueueId(void);
HULONG svc_cas_MgrGetCasSyncSemaId(void);
HULONG svc_cas_MgrGetCasMmiDataSemaId(void);
SvcCas_Context_t *svc_cas_MgrGetActionContextByActionId(HUINT32 ulActionId);
SvcCas_Context_t *svc_cas_MgrGetActionContextByAction(Handle_t hAction);
SIxTextEncoding_e svc_cas_MgrGetTextEncoding(HUINT32 ulActionId);
SIxSpec_e svc_cas_MgrGetSiSpec(HUINT32 ulActionId);
HERROR svc_cas_MgrDisconnectService(HUINT32 ulStopActionId, HBOOL bNotifyCasStop);
#if defined(CONFIG_OP_SES)
HBOOL svc_cas_MgrGetSystemIdFoundByInstance(SvcCas_Context_t *pstCasContext, HUINT32 ulInstanceId);
HBOOL svc_cas_MgrGetWriteTimestampFlagForDrmNone(Handle_t hAction);
#endif
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
HERROR svc_cas_MgrSearchNotDesCasContextNConnect(HUINT32 ulCaInstanceId, DxCAS_GroupId_e eCasGroupId);
#else
HERROR svc_cas_MgrSearchNotDesCasContextNConnect(HUINT32 ulCaInstanceId);
#endif
HERROR svc_cas_MgrGetRequestedCamIdByActionId(HUINT32 ulActionId, HUINT8 *pucCamId);

#if defined(CONFIG_DEBUG)
const HINT8 * svc_cas_MgrCmd_MakeStrCasCmd(SvcCas_Command_e cmd);
void svc_cas_MgrCmd_Init(HxCMD_Handle_t hCmdHandle);

#endif

HERROR svc_cas_CiGetOpName(HUINT16 usSlotId, HUINT8 **ppszOpName);


#endif /* !__CAS_MGR_MAIN_H__ */
