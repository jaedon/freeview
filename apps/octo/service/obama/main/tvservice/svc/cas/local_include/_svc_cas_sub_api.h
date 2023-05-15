/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _svc_cas_sub_api.h						\n
	@brief	  cas module header file		\n

	Description: cas module. \n
	Module: MW/CAS									 					\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__SVC_CAS_SUB_API_H__
#define	__SVC_CAS_SUB_API_H__

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************************
* Header Files :  헤더 파일은 가능한 .c 에서 include 할 것 !
********************************************************************/
#define ___01_Header_Files___

#include <octo_common.h>

#include <svc_cas.h>

/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___02_Extern_Vars_Func_Prototypes___

/********************************************************************
* Global Public Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Global_Public_Definitions___


#define _____ACT_____________

typedef enum
{
	eCAS_DscState_Unknown,
	eCAS_DscState_Scrambled,
	eCAS_DscState_Descrambled,
	eCAS_DscState_Max
} SvcCas_DscState_e;

typedef enum
{
	eCAS_MSG_TYPE_None,
	eCAS_MSG_TYPE_Ok,
	eCAS_MSG_TYPE_Fail,
	eCAS_MSG_TYPE_Command,
	eCAS_MSG_TYPE_Check,
	eCAS_MSG_TYPE_Max
} SvcCas_MsgType_e;

// New defined
typedef void (* SvcCas_MgrStartSessionCb_t)(Handle_t hAction, Handle_t hSvc, SvcCas_ActionType_e eActType);	// Action Type 전달 해준다.
typedef void (* SvcCas_MgrStopSessionCb_t)(Handle_t hAction, Handle_t hNextSvc);
typedef void (* SvcCas_MgrSetDscCb_t)(Handle_t hAction, SvcCas_SetPidInfo_t *pstCaDscInfo);		// Live, Rec 구분 해서 보낸다.
typedef void (* SvcCas_MgrManageInstCb_t)(Handle_t hAction, HUINT32 ulCasInstanceId, HBOOL bConnect);

typedef void (* SvcCas_MgrUpdatePmtCb_t)(Handle_t hAction, HUINT8 *pucPmt);
typedef void (* SvcCas_MgrUpdateCatCb_t)(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pucCat);
typedef void (* SvcCas_MgrUpdateSdtCb_t)(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pucSdt);
typedef void (* SvcCas_MgrUpdateNitCb_t)(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pucNit);
typedef void (* SvcCas_MgrUpdateEitCb_t)(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pucEitP, HUINT8 *pucEitF);
typedef void (* SvcCas_MgrUpdateTdtCb_t)(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pucTdt);
typedef void (* SvcCas_MgrUpdateBatCb_t)(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pucBat);

typedef void (* SvcCas_MgrUpdateStateOfSvcCb_t)(Handle_t hAction, SvcCas_DscState_e eState);

/*
*	sub cas에서 last cas message를 보관하고, cas mgr이 필요 시 이를 get 하는 callback. 정형화된 api 정의를 위해서 callback을 정의함.
*/
typedef void (* SvcCas_MgrCheckLastMsgCb_t)(Handle_t hAction, SvcCas_MsgType_e *peMsgType, DxCAS_GroupId_e *peCasId, void **ppMessage, SvcCas_ShowType_e *peShowType);

typedef struct
{
	SvcCas_MgrStartSessionCb_t			pfnCbStartSession;
	SvcCas_MgrStopSessionCb_t			pfnCbStopSession;
	SvcCas_MgrSetDscCb_t				pfnCbSetDSC;
	SvcCas_MgrManageInstCb_t			pfnCbInstSelected;

	SvcCas_MgrUpdatePmtCb_t				pfnCbUpdatePmt;
	SvcCas_MgrUpdateCatCb_t				pfnCbUpdateCat;
	SvcCas_MgrUpdateSdtCb_t				pfnCbUpdateSdt;
	SvcCas_MgrUpdateNitCb_t				pfnCbUpdateNit;
	SvcCas_MgrUpdateEitCb_t				pfnCbUpdateEit;
	SvcCas_MgrUpdateTdtCb_t				pfnCbUpdateTdt;
	SvcCas_MgrUpdateBatCb_t				pfnCbUpdateBat;

	SvcCas_MgrCheckLastMsgCb_t			pfnCbCheckLastMsg;

} SvcCas_CbFuncList_t;

typedef enum
{
	eCAS_CAM_STATE_Removed,	// 캠이 제거된 경우 또는 캠 모듈을 power off 한 경우 (DD로부터 notify됨)
	eCAS_CAM_STATE_Inserted,	// 캠이 삽입된 직후 (DD로부터 notify됨)
	eCAS_CAM_STATE_Initializing,	// 캠이 삽입되어 RM 세션이 오픈된 상태 (CI 스택으로부터 notify됨)
	eCAS_CAM_STATE_Initialized	// 캠 정보(Application Information)까지 수신이 된 상태 (CI 스택으로부터 notify됨)
} SvcCas_CamState_e;

typedef enum
{
	eCiAppType_Ca				= 0x01,	// Application Information v1 (CI_AI_RES_V1_ID) : currently only supported
	eCiAppType_Epg				= 0x02,	//
	eCiAppType_SwUpgrade		= 0x03,	// Application Information v2 (CI_AI_RES_V2_ID)
	eCiAppType_NetInterface		= 0x04,	//
	eCiAppType_AccessAids		= 0x05,	//
	eCiAppType_Unclassified		= 0x06	//
} SvcCas_CiAppType_e; 						// application_type in application_info() from CI_APPL_INFO_REPLY : 스펙에 정해진 값으로 enum 값 수정하지 말 것!

typedef struct
{
	SvcCas_CiAppType_e		eCiAppType;
	unsigned short			usManufacturer;
	unsigned short			usManufacturerCode;
	unsigned char			szText[MAX_CI_STR_LEN+1];
	unsigned char			ucIsInitialized;
} SvcCas_CiAppInfo_t;	// from CI_APPL_INFO_REPLY

#if defined(CONFIG_MW_CI_PLUS)

// pfnNotifyHostTune ---
typedef enum
{
	eCiPlus_Tune_Start_v1_TuneByTripleId,		// HCI v1 에 의한 tune (즉, CI_TUNE 에 의한)
	eCiPlus_Tune_Start_v2_TuneByTripleId,		// HCI v2 에 의한 tune, CI+ v1.3
	eCiPlus_Tune_Start_v2_TuneByLocation,		// HCI v2 CI_TUNE_BROADCAST_REQ 에 의한 tune, CI+ v1.3
	eCiPlus_Tune_Stop,		// 이 경우는 CI/CI+ 스택에서 처리하지 않고 CAS AP 쪽에서 처리하도록 한다.
	eCiPlus_Tune_Cancel		// 이 경우는 CI/CI+ 스택에서 처리하지 않고 CAS AP 쪽에서 처리하도록 한다.
} SvcCas_CiPlusTuneMode_e;

typedef struct
{
	SvcCas_CiPlusTuneMode_e eTuneMode;

	union
	{
		struct
		{
			// CI_TUNE 에 의한 동작
			// eMode 가 eCasTuneMode_Start_v1_TuneByTripleId & eCasTuneMode_Start_v2_TuneByTripleId 일 때 사용된다.
			// HCI v2일 경우에 tuning 결과를 CI_TuneReply() 를 통해 CAM 에 알려주어야 한다.
			HUINT16	usNetId;
			HUINT16	usOnId;
			HUINT16	usTsId;
			HUINT16	usSvcId;
		} stTripleId;
		struct
		{
			// CI_TUNE_BROADCAST_REQ 에 의한 동작 (CI+ v1.3)
			// eMode 가 eCasTuneMode_Start_v2_TuneByLocation 일 때 사용된다.
			// tuning 결과는 CI_TuneReply() 를 통해 CAM 에 알려주어야 한다.
			// 이하 포인터들은 OxCAS_Malloc() 되어 전달되는 것으로 받는 쪽에서 반드시 OxCAS_Free() 해줘야 한다.
			HBOOL	bPmtFlag;
			HUINT16	usServiceId; // usServiceId 가 0 인 경우 bPmtFlag 는 FALSE 이고 pucDescriptors 를 참고하여 해당 TP 로 tuning 만 한다 (이 경우 ca_pmt 보내지 않음). usServiceId 가 0 이 아니면 PMT 내의 program_number 과 동일하다.
			HUINT16	usDescriptorsLen;
			HUINT8	*pucDescriptors; // delivery_system_descriptor 들과 service_descriptor, short_event_descriptor, component descriptor, parental_rating_descriptor, content_descriptor 등이 있을 수 있다.
			HUINT8	*pucPmt; // bPmtFlag 가 FALSE 인 경우 NULL, bPmtFlag 가 TRUE 인 경우 PMT 가 존재한다. PMT 자체에 length 가 존재하므로 pucPmt 의 length 를 별도로 전달하지는 않는다 (즉, 받는 쪽에서 bPmtFlag 보고 pucPmt 파싱해서 처리할 것)
		} stBroadcastReq;
	} utInfo;

} SvcCas_CiPlusTuneInfo_t;
#endif

/* multi cas 일 경우에 어떤 CAS를 선택 해야 할지에 관여하는 항목들을 구룹으로 묶어 놓았다.*/
typedef enum eItemOfPriority
{
	/* Cas System ID의 match 여부
		embedded든 CAM이든 match가 안되도 선택되는 시나리오가 기본임..
		eItemOfPriority_G_DeleteNotMatchCaSystemId 요게 들어 가면 match가 안되면 선택이 안되는 시나리오임..
	*/
	eItemOfPriority_G_CasSystemIdMatch = 1,
		eItemOfPriority_G_DeleteNotMatchCaSystemId,

	/* device의 존재 여부
		CAM)
			CAM인 경우 device가 존재 하지 않으면 아예 선택 tree에서 삭제 되어야한다.
		Embedded CAS)
			Embedded CAS인 경우 CAS마다 device가 없어도 전달 해야 하는 경우가 발생하기 때문에 확인 필요..
			우선은 device가 없어도 전달하는것을 기본으로 한다.

		확인 할 부분...
		device가 존재 하지 않아도 우선 순위에서 높을 수 있다.
		NA 서비스이면 device가 있는 CX inst보다 device가 없는 NA가 일반적으로 우선 순위가 높다.
	*/
	eItemOfPriority_G_ExistDevice,

	/* 	Cas의 종류
		Multi CAS일 경우 서비스와 같은 종류의 CAS는 선택되고 같지 않은 경우 어떤 CAS를 선택 해야한는가? 에 대한 우선 순위
	*/
	eItemOfPriority_G_KindOfCas,
		eItemOfPriority_UnknownCas,
		eItemOfPriority_BCas,
		eItemOfPriority_Conax,
		eItemOfPriority_Irdeto,
		/* iruc는 우선 집어 넣기 애매 하다. DxCAS_GroupId_e 하고 일대일 매칭이 되지 않는다. SvcCas_DevInstance_t.eCasId 요놈하고 비교 해야 하는데..
			ㅜㅜ 우선 우선 주석 처리 하자... 추후에 생각 하자능.. SvcCas_DevInstance_t.eCasId 요놈 고쳐야 할수도 있다능...
		*/
		//eItemOfPriority_IrdetoUc,
		eItemOfPriority_Nagra,
		eItemOfPriority_satsa,
		eItemOfPriority_Viaccess,
		eItemOfPriority_MG,
		eItemOfPriority_CR,
		eItemOfPriority_ND,
		eItemOfPriority_CAM,

	/*	resource의 많고 적음 여부
		같은 CAS가 있을 경우 resoure의 상태로 적절하게 배분하기 위함
		기본 시나리오는 resource 많이 남은 것을 선택한다. resource가 많은 놈이 아니다 주의 할것 !!
		resource가 없어서 선택이 안되는 부분은 eItemOfPriority_G_KindOfAction 여기서 결정한다.
	*/
	eItemOfPriority_G_Resource,

	/*	device의 종류
	*/
	eItemOfPriority_G_KindOfDevice, // 사용자 수정가능
		eItemOfPriority_ECAS,
		eItemOfPriority_CI,

	/*	slot의 순서에 따라서 우선 순위가 결정될 수 있다.
		smc같은 경우 simulcrypt 상황에서 slot의 순서가 우선순위가 될수 있다.
		conax같은 경우는 두개가 될 수도 있다.
	*/
	eItemOfPriority_G_OrderOfSlot, // 사용자 수정가능
		eItemOfPriority_SC0,
		eItemOfPriority_SC1,
		eItemOfPriority_CAM0,
		eItemOfPriority_CAM1,

	/*	action의 종류
		요놈은 좀 특이하다... 항상 우선 순위 마지막에 와야 한다. 그리고 반드시 넣도록 하자~

		1. 리소스가 없는데 Ca system Id가 일치하는 현재 play되고 있는 녀석이 요청된 서비스보다 우선 순위가 낮으면 선택된다.
		2. 리소스가 없는데 우선 순위도 안되는 녀석은 삭제 한다. -> 특정 cas에서 이런 경우에도 받기를 원하면 수정해야 하는 부분..
		3. 리소스가 없는데 기존에 play되고 있던 녀석이 descramble가 제대로 되고 있지 않은 상태이면 걍 뺏어 오도록 하자.

		어찌 되었든 action 우선 순위때문에 결정되어서 선택된 녀석은 play되고 있더라도 stop되어야만 한다.

		typedef enum
		{
			eCAS_ActionType_None,
			eCAS_ActionType_Live,
			eCAS_ActionType_Playback,
			eCAS_ActionType_Recording,
			eCAS_ActionType_Descramble_a_File,		// Descramble a file 시 recording 전용.
			eCAS_ActionType_Tsr_Play,
			eCAS_ActionType_Tsr_Rec,
			eCAS_ActionType_Data,
			eCAS_ActionType_EngineeringTS,			// japan arib 에서 사용하는, svc handle 없이 ts 만 존재하는 cas용 engineering ts
			eCAS_ActionType_Max
		}SvcCas_ActionType_e;

		현재 우선 순위는 아래와 같다. 수정되는 부분이 있으면 추가기입하자.
			일반적으로 live, playback, TsrPlay가 우선 순위가 높다.
			recording, Descramble_a_File, TsrRec은 우선 순위가 낮다.
			live, playback, TsrPlay					-> 우선 순위 같다.
			recording, Descramble_a_File, TsrRec		-> 우선 순위 같다.
	*/
	eItemOfPriority_G_KindOfAction,
		eItemOfPriority_Live,
		eItemOfPriority_PlayBack,
		eItemOfPriority_Tsr_Play,
		eItemOfPriority_Recording,
		eItemOfPriority_Descramble_a_File,
		eItemOfPriority_Tsr_Rec,

	/*	CAM ID가 일치하는 경우에만 선택하도록 하고 나머지는 무조건 무시하도록 한다.
	*/
	eItemOfPriority_G_CamIdMatch,

	eItemOfPriority_unKnownItem,
	eItemOfPriority_End,
} SvcCas_ItemOfPriority_e;

#define MW_CAS_MAX_PRIORITY_ITEM_COUNT 10

typedef struct tagSvcCas_MgrPriorityTreeItem_t
{
	HUINT32		unCasInstTableId;

	/* 우선 순위 search후 우선 순위때문에 stop이 되어야 할 serivce에 대한 return 값.
	*/
	HBOOL		bStop;
	HUINT32		ulStopActionId;

	struct tagSvcCas_MgrPriorityTreeItem_t	*pstParent;
	struct tagSvcCas_MgrPriorityTreeItem_t	*pstNextSilbing;
	struct tagSvcCas_MgrPriorityTreeItem_t	*pstPrevSilbing;
	struct tagSvcCas_MgrPriorityTreeItem_t	*pstChild;
} SvcCas_MgrPriorityTreeItem_t;

typedef struct
{
	SvcCas_MgrPriorityTreeItem_t	*pstHead;
} SvcCas_MgrPriorityTreeMgr_t;


typedef void (*SvcCas_FindCasInstCb_t)(SvcCas_MgrPriorityTreeMgr_t *pstTreeMgr, SvcCas_MgrPriorityTreeItem_t *pstItem, HUINT32 unPriorityTableId, HUINT32 ulActionId);

typedef struct tag_sGroupOfPriority_t
{
	SvcCas_ItemOfPriority_e		eItemOfPriority_G;

	HUINT32						ulNumOfPriority;
	SvcCas_ItemOfPriority_e		aeItemOfPriority[MW_CAS_MAX_PRIORITY_ITEM_COUNT]; // 우선 순위가 될수도 있고 특정 시나리오가 될수도 있다.

	/* 일반적으로는 aeItemOfPriority[]의 순서가 priority인데 우선 순위가 동일한 것이 존재 해서
		aeItemOfPriority[]로만으로 부족할때 aulPriority[] 를 사용하도록 한다. eItemOfPriority_G_KindOfAction을 참고 할것.
	*/
	HUINT32						aulPriority[MW_CAS_MAX_PRIORITY_ITEM_COUNT];

	SvcCas_FindCasInstCb_t		pfnFindCasInst;
} SvcCas_GroupOfPriority_t;

/********************************************************************
* Global Public Function Prototypes
********************************************************************/
#define ___04_Global_Public_Func_Prototypes___

HERROR svc_cas_MgrSubUpdateCasEtcEvent(Handle_t		hAction,
									Handle_t		ulCaInstanceId,
									DxCAS_GroupId_e	eCasGrpId,
									HINT32			nMsg,
									HINT32			p1,
									HINT32			p2,
									HINT32			p3);

HERROR svc_cas_MgrSubUpdateCasEtcEventData (Handle_t		hAction,
										Handle_t		ulCaInstanceId,
										DxCAS_GroupId_e	eCasGrpId,
										HINT32			nMsg,
										HINT32			p1,
										HINT32			p2,
										void				*pData,
										HUINT32			ulSize);

HERROR svc_cas_MgrSubRegisterCb(SvcCas_InstanceType_e	eCasInstType,
								DxCAS_GroupId_e			eCasId,
								HUINT32					ulSlot,
								SvcCas_CbFuncList_t		*pstCbList,
								HUINT32					ulNumOfSlot,
								HUINT32					*pulCasInstanceId);

HERROR svc_cas_MgrSubUpdateCasInfo(Handle_t		hCasInstanceId,
								DxCAS_GroupId_e	eCasGroupId,
								HUINT16			usSlotId,
								HUINT16			*pusCaSysId,
								HUINT32			ulNumCaSysId,
								HUINT32			ulMaxNumSvc,
								HBOOL			bAvailable);


#if defined(CONFIG_MW_CI)
HERROR svc_cas_MgrSubUpdateCamState(HUINT16 usSlotId, SvcCas_CamState_e eCamState);
HERROR svc_cas_MgrSubUpdateCamAppinfo(HUINT16 usSlotId, SvcCas_CiAppInfo_t *pstCasCiAppInfo);
#endif

#if defined(CONFIG_SUPPORT_NOVA_APP)
HERROR svc_cas_MgrSubUpdateMultiscreen(Handle_t			 hAction,
									SvcCas_MsgType_e	 eMsgType,
									DxCAS_GroupId_e		 eCasId,
									void				*pvMessage,
									SvcCas_ShowType_e	 eShowType);
#endif

HERROR svc_cas_MgrSubUpdateCasMessage(Handle_t			 hAction,
									SvcCas_MsgType_e	 eMsgType,
									DxCAS_GroupId_e		 eCasId,
									void				*pvMessage,
									SvcCas_ShowType_e	 eShowType);

HERROR svc_cas_MgrSubUpdateDrmInfo(	Handle_t		 hAction,
									Handle_t		 hCaInstanceId,
									void			*pvData,
									DxCopyrightControl_CasCcType_e	 eDrmType,
									HUINT32			 ulVersion);

HERROR svc_cas_MgrSubUpdateMmi(SvcCas_MmiDataType_e eDataType, HUINT16 usSlotId, HUINT32 ulVersion);

SvcCas_PmtInfo_t *svc_cas_MgrSubGetPmtInfo(Handle_t hAction);

HERROR svc_cas_MgrSubGetPmtSectionState(Handle_t hAction, HUINT32 *pulPmtState);
HERROR svc_cas_MgrSubGetSiTableState(Handle_t hAction, SiTableType_e eType, SvcCas_SiTableState_e *peState);

HERROR svc_cas_MgrSubNotifyRecordOperation(Handle_t hAction, DxCAS_GroupId_e eCasId, HUINT8 ucRecStatus, HUINT16 usSlotId);

SvcCas_InstanceType_e svc_cas_MgrSubGetCasDevInstType(HUINT32 ulCasInstId);
#if defined(CONFIG_MW_CAS_NUM_SC_SLOT)
HERROR svc_cas_MgrSubSendScCmd2MgrTask(DxCAS_GroupId_e eGroupId, HUINT32 ulCommand, HUINT32 ulMsgKind, HUINT8* pDataToSend);
#endif

/********************************************************
아래 부터는 control 관련..  함수들임~

**********************************************************/
HERROR svc_cas_MgrSubInitControl(DxCAS_GroupId_e eCasId);

HERROR svc_cas_MgrSubRegisterControl(DxCAS_GroupId_e eCasId, HUINT32 ulNum, SvcCas_Ctrl_t *pstCasCtrlList);

/********************************************************
아래 부터는 시나리오 관련..  함수들임~

**********************************************************/
HERROR svc_cas_MgrSubRegisterPriorityTable(SvcCas_GroupOfPriority_t *pstGroupOfPriority);

HERROR svc_cas_MgrSubIsExistDevice(SvcCas_MgrPriorityTreeItem_t *pstPriorityTree, SvcCas_InstanceType_e *peCasInstType);

SvcCas_GroupOfPriority_t *svc_cas_MgrSubGetPriorityTable(HUINT32 unTableId);

HERROR	svc_cas_MgrSubIsCasSystemIdMatch(SvcCas_MgrPriorityTreeItem_t *pstItem, void *pstCasContext);

HERROR svc_cas_MgrSubIsCamIdMatch(SvcCas_MgrPriorityTreeItem_t *pstItem, void *pstCasContext);

void *svc_cas_MgrSubGetActionContextByActionId(HUINT32 ulActionId);

DxCAS_GroupId_e svc_cas_MgrSubGetCasGroupIdFromCasInst(HUINT32 ulCasInstId);

HERROR svc_cas_MgrSubIsDeviceMatchItem(SvcCas_MgrPriorityTreeItem_t *pstFirstItem, SvcCas_ItemOfPriority_e eItem);

HERROR svc_cas_MgrSubIsSameDefSlot(SvcCas_MgrPriorityTreeItem_t *pstItem, SvcCas_InstanceType_e eCasInstType, HUINT32 ulSlot);

HUINT32 svc_cas_MgrSubGetLargestResourceCount(SvcCas_MgrPriorityTreeItem_t *pstPriorityTree);

HUINT32	svc_cas_MgrSubGetNumOfLessItemFromLargestResourceCount(SvcCas_MgrPriorityTreeItem_t *pstItem, HUINT32 ulLargestNum);

HERROR svc_cas_MgrSubIsLessThenInputArg(SvcCas_MgrPriorityTreeItem_t *pstItem, HUINT32 ulLargestNum);

void svc_cas_MgrSubMakePTFromGSlotByDB(SvcCas_MgrPriorityTreeMgr_t		*pstTreeMgr,
										SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
										HUINT32							 ulPriorityTableId,
										HUINT32							 ulReqActionId);

void svc_cas_MgrSubMakePTFromGKindOfAction(SvcCas_MgrPriorityTreeMgr_t		*pstTreeMgr,
											SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
											HUINT32							 ulPriorityTableId,
											HUINT32							 ulReqActionId);

HERROR svc_cas_MgrSubGetRequestedCamIdByActionId(HUINT32 ulActionId, HUINT8 *pucCamId);

HERROR svc_cas_MgrSubIsLastLiveInst(SvcCas_MgrPriorityTreeItem_t *pstItem, void *pstCasContext);

HUINT32 svc_cas_MgrUtilStrLen(const HUINT8 *pszStr);
HERROR svc_cas_SendCaPmt(HUINT16 usSlotId, HUINT8 *pucCaPmt, HUINT16 usCaPmtLen);
#ifdef __cplusplus
}
#endif


#endif /* !__SVC_CAS_SUB_API_H__ */
