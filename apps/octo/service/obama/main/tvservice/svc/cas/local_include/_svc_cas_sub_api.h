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
* Header Files :  ��� ������ ������ .c ���� include �� �� !
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
typedef void (* SvcCas_MgrStartSessionCb_t)(Handle_t hAction, Handle_t hSvc, SvcCas_ActionType_e eActType);	// Action Type ���� ���ش�.
typedef void (* SvcCas_MgrStopSessionCb_t)(Handle_t hAction, Handle_t hNextSvc);
typedef void (* SvcCas_MgrSetDscCb_t)(Handle_t hAction, SvcCas_SetPidInfo_t *pstCaDscInfo);		// Live, Rec ���� �ؼ� ������.
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
*	sub cas���� last cas message�� �����ϰ�, cas mgr�� �ʿ� �� �̸� get �ϴ� callback. ����ȭ�� api ���Ǹ� ���ؼ� callback�� ������.
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
	eCAS_CAM_STATE_Removed,	// ķ�� ���ŵ� ��� �Ǵ� ķ ����� power off �� ��� (DD�κ��� notify��)
	eCAS_CAM_STATE_Inserted,	// ķ�� ���Ե� ���� (DD�κ��� notify��)
	eCAS_CAM_STATE_Initializing,	// ķ�� ���ԵǾ� RM ������ ���µ� ���� (CI �������κ��� notify��)
	eCAS_CAM_STATE_Initialized	// ķ ����(Application Information)���� ������ �� ���� (CI �������κ��� notify��)
} SvcCas_CamState_e;

typedef enum
{
	eCiAppType_Ca				= 0x01,	// Application Information v1 (CI_AI_RES_V1_ID) : currently only supported
	eCiAppType_Epg				= 0x02,	//
	eCiAppType_SwUpgrade		= 0x03,	// Application Information v2 (CI_AI_RES_V2_ID)
	eCiAppType_NetInterface		= 0x04,	//
	eCiAppType_AccessAids		= 0x05,	//
	eCiAppType_Unclassified		= 0x06	//
} SvcCas_CiAppType_e; 						// application_type in application_info() from CI_APPL_INFO_REPLY : ���忡 ������ ������ enum �� �������� �� ��!

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
	eCiPlus_Tune_Start_v1_TuneByTripleId,		// HCI v1 �� ���� tune (��, CI_TUNE �� ����)
	eCiPlus_Tune_Start_v2_TuneByTripleId,		// HCI v2 �� ���� tune, CI+ v1.3
	eCiPlus_Tune_Start_v2_TuneByLocation,		// HCI v2 CI_TUNE_BROADCAST_REQ �� ���� tune, CI+ v1.3
	eCiPlus_Tune_Stop,		// �� ���� CI/CI+ ���ÿ��� ó������ �ʰ� CAS AP �ʿ��� ó���ϵ��� �Ѵ�.
	eCiPlus_Tune_Cancel		// �� ���� CI/CI+ ���ÿ��� ó������ �ʰ� CAS AP �ʿ��� ó���ϵ��� �Ѵ�.
} SvcCas_CiPlusTuneMode_e;

typedef struct
{
	SvcCas_CiPlusTuneMode_e eTuneMode;

	union
	{
		struct
		{
			// CI_TUNE �� ���� ����
			// eMode �� eCasTuneMode_Start_v1_TuneByTripleId & eCasTuneMode_Start_v2_TuneByTripleId �� �� ���ȴ�.
			// HCI v2�� ��쿡 tuning ����� CI_TuneReply() �� ���� CAM �� �˷��־�� �Ѵ�.
			HUINT16	usNetId;
			HUINT16	usOnId;
			HUINT16	usTsId;
			HUINT16	usSvcId;
		} stTripleId;
		struct
		{
			// CI_TUNE_BROADCAST_REQ �� ���� ���� (CI+ v1.3)
			// eMode �� eCasTuneMode_Start_v2_TuneByLocation �� �� ���ȴ�.
			// tuning ����� CI_TuneReply() �� ���� CAM �� �˷��־�� �Ѵ�.
			// ���� �����͵��� OxCAS_Malloc() �Ǿ� ���޵Ǵ� ������ �޴� �ʿ��� �ݵ�� OxCAS_Free() ����� �Ѵ�.
			HBOOL	bPmtFlag;
			HUINT16	usServiceId; // usServiceId �� 0 �� ��� bPmtFlag �� FALSE �̰� pucDescriptors �� �����Ͽ� �ش� TP �� tuning �� �Ѵ� (�� ��� ca_pmt ������ ����). usServiceId �� 0 �� �ƴϸ� PMT ���� program_number �� �����ϴ�.
			HUINT16	usDescriptorsLen;
			HUINT8	*pucDescriptors; // delivery_system_descriptor ��� service_descriptor, short_event_descriptor, component descriptor, parental_rating_descriptor, content_descriptor ���� ���� �� �ִ�.
			HUINT8	*pucPmt; // bPmtFlag �� FALSE �� ��� NULL, bPmtFlag �� TRUE �� ��� PMT �� �����Ѵ�. PMT ��ü�� length �� �����ϹǷ� pucPmt �� length �� ������ ���������� �ʴ´� (��, �޴� �ʿ��� bPmtFlag ���� pucPmt �Ľ��ؼ� ó���� ��)
		} stBroadcastReq;
	} utInfo;

} SvcCas_CiPlusTuneInfo_t;
#endif

/* multi cas �� ��쿡 � CAS�� ���� �ؾ� ������ �����ϴ� �׸���� �������� ���� ���Ҵ�.*/
typedef enum eItemOfPriority
{
	/* Cas System ID�� match ����
		embedded�� CAM�̵� match�� �ȵǵ� ���õǴ� �ó������� �⺻��..
		eItemOfPriority_G_DeleteNotMatchCaSystemId ��� ��� ���� match�� �ȵǸ� ������ �ȵǴ� �ó�������..
	*/
	eItemOfPriority_G_CasSystemIdMatch = 1,
		eItemOfPriority_G_DeleteNotMatchCaSystemId,

	/* device�� ���� ����
		CAM)
			CAM�� ��� device�� ���� ���� ������ �ƿ� ���� tree���� ���� �Ǿ���Ѵ�.
		Embedded CAS)
			Embedded CAS�� ��� CAS���� device�� ��� ���� �ؾ� �ϴ� ��찡 �߻��ϱ� ������ Ȯ�� �ʿ�..
			�켱�� device�� ��� �����ϴ°��� �⺻���� �Ѵ�.

		Ȯ�� �� �κ�...
		device�� ���� ���� �ʾƵ� �켱 �������� ���� �� �ִ�.
		NA �����̸� device�� �ִ� CX inst���� device�� ���� NA�� �Ϲ������� �켱 ������ ����.
	*/
	eItemOfPriority_G_ExistDevice,

	/* 	Cas�� ����
		Multi CAS�� ��� ���񽺿� ���� ������ CAS�� ���õǰ� ���� ���� ��� � CAS�� ���� �ؾ��Ѵ°�? �� ���� �켱 ����
	*/
	eItemOfPriority_G_KindOfCas,
		eItemOfPriority_UnknownCas,
		eItemOfPriority_BCas,
		eItemOfPriority_Conax,
		eItemOfPriority_Irdeto,
		/* iruc�� �켱 ���� �ֱ� �ָ� �ϴ�. DxCAS_GroupId_e �ϰ� �ϴ��� ��Ī�� ���� �ʴ´�. SvcCas_DevInstance_t.eCasId ����ϰ� �� �ؾ� �ϴµ�..
			�̤� �켱 �켱 �ּ� ó�� ����... ���Ŀ� ���� ���ڴ�.. SvcCas_DevInstance_t.eCasId ��� ���ľ� �Ҽ��� �ִٴ�...
		*/
		//eItemOfPriority_IrdetoUc,
		eItemOfPriority_Nagra,
		eItemOfPriority_satsa,
		eItemOfPriority_Viaccess,
		eItemOfPriority_MG,
		eItemOfPriority_CR,
		eItemOfPriority_ND,
		eItemOfPriority_CAM,

	/*	resource�� ���� ���� ����
		���� CAS�� ���� ��� resoure�� ���·� �����ϰ� ����ϱ� ����
		�⺻ �ó������� resource ���� ���� ���� �����Ѵ�. resource�� ���� ���� �ƴϴ� ���� �Ұ� !!
		resource�� ��� ������ �ȵǴ� �κ��� eItemOfPriority_G_KindOfAction ���⼭ �����Ѵ�.
	*/
	eItemOfPriority_G_Resource,

	/*	device�� ����
	*/
	eItemOfPriority_G_KindOfDevice, // ����� ��������
		eItemOfPriority_ECAS,
		eItemOfPriority_CI,

	/*	slot�� ������ ���� �켱 ������ ������ �� �ִ�.
		smc���� ��� simulcrypt ��Ȳ���� slot�� ������ �켱������ �ɼ� �ִ�.
		conax���� ���� �ΰ��� �� ���� �ִ�.
	*/
	eItemOfPriority_G_OrderOfSlot, // ����� ��������
		eItemOfPriority_SC0,
		eItemOfPriority_SC1,
		eItemOfPriority_CAM0,
		eItemOfPriority_CAM1,

	/*	action�� ����
		����� �� Ư���ϴ�... �׻� �켱 ���� �������� �;� �Ѵ�. �׸��� �ݵ�� �ֵ��� ����~

		1. ���ҽ��� ���µ� Ca system Id�� ��ġ�ϴ� ���� play�ǰ� �ִ� �༮�� ��û�� ���񽺺��� �켱 ������ ������ ���õȴ�.
		2. ���ҽ��� ���µ� �켱 ������ �ȵǴ� �༮�� ���� �Ѵ�. -> Ư�� cas���� �̷� ��쿡�� �ޱ⸦ ���ϸ� �����ؾ� �ϴ� �κ�..
		3. ���ҽ��� ���µ� ������ play�ǰ� �ִ� �༮�� descramble�� ����� �ǰ� ���� ���� �����̸� �� ���� ������ ����.

		���� �Ǿ��� action �켱 ���������� �����Ǿ ���õ� �༮�� play�ǰ� �ִ��� stop�Ǿ�߸� �Ѵ�.

		typedef enum
		{
			eCAS_ActionType_None,
			eCAS_ActionType_Live,
			eCAS_ActionType_Playback,
			eCAS_ActionType_Recording,
			eCAS_ActionType_Descramble_a_File,		// Descramble a file �� recording ����.
			eCAS_ActionType_Tsr_Play,
			eCAS_ActionType_Tsr_Rec,
			eCAS_ActionType_Data,
			eCAS_ActionType_EngineeringTS,			// japan arib ���� ����ϴ�, svc handle ���� ts �� �����ϴ� cas�� engineering ts
			eCAS_ActionType_Max
		}SvcCas_ActionType_e;

		���� �켱 ������ �Ʒ��� ����. �����Ǵ� �κ��� ������ �߰���������.
			�Ϲ������� live, playback, TsrPlay�� �켱 ������ ����.
			recording, Descramble_a_File, TsrRec�� �켱 ������ ����.
			live, playback, TsrPlay					-> �켱 ���� ����.
			recording, Descramble_a_File, TsrRec		-> �켱 ���� ����.
	*/
	eItemOfPriority_G_KindOfAction,
		eItemOfPriority_Live,
		eItemOfPriority_PlayBack,
		eItemOfPriority_Tsr_Play,
		eItemOfPriority_Recording,
		eItemOfPriority_Descramble_a_File,
		eItemOfPriority_Tsr_Rec,

	/*	CAM ID�� ��ġ�ϴ� ��쿡�� �����ϵ��� �ϰ� �������� ������ �����ϵ��� �Ѵ�.
	*/
	eItemOfPriority_G_CamIdMatch,

	eItemOfPriority_unKnownItem,
	eItemOfPriority_End,
} SvcCas_ItemOfPriority_e;

#define MW_CAS_MAX_PRIORITY_ITEM_COUNT 10

typedef struct tagSvcCas_MgrPriorityTreeItem_t
{
	HUINT32		unCasInstTableId;

	/* �켱 ���� search�� �켱 ���������� stop�� �Ǿ�� �� serivce�� ���� return ��.
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
	SvcCas_ItemOfPriority_e		aeItemOfPriority[MW_CAS_MAX_PRIORITY_ITEM_COUNT]; // �켱 ������ �ɼ��� �ְ� Ư�� �ó������� �ɼ��� �ִ�.

	/* �Ϲ������δ� aeItemOfPriority[]�� ������ priority�ε� �켱 ������ ������ ���� ���� �ؼ�
		aeItemOfPriority[]�θ����� �����Ҷ� aulPriority[] �� ����ϵ��� �Ѵ�. eItemOfPriority_G_KindOfAction�� ���� �Ұ�.
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
�Ʒ� ���ʹ� control ����..  �Լ�����~

**********************************************************/
HERROR svc_cas_MgrSubInitControl(DxCAS_GroupId_e eCasId);

HERROR svc_cas_MgrSubRegisterControl(DxCAS_GroupId_e eCasId, HUINT32 ulNum, SvcCas_Ctrl_t *pstCasCtrlList);

/********************************************************
�Ʒ� ���ʹ� �ó����� ����..  �Լ�����~

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
