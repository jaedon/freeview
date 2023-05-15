/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _svc_cas_mgr_instance.h						\n
	@brief	  cas module header file		\n

	Description: cas module. \n
	Module: MW/CAS									 					\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__CAS_MGR_INSTANCE_H__
#define	__CAS_MGR_INSTANCE_H__


/********************************************************************
* Header Files :  ��� ������ ������ .c ���� include �� �� !
********************************************************************/
#define ___01_Header_Files___

#include <octo_common.h>

#include <_svc_cas_mgr_common.h>

/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___02_Extern_Vars_Func_Prototypes___


/********************************************************************
* Module Internal Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Module_Internal_Definitions___

typedef enum
{
	eCAS_NOTI_Start,
	eCAS_NOTI_Stop,
	eCAS_NOTI_SetDsc,			// set descrambler, pid setting.
	eCAS_NOTI_ManageInst,

	eCAS_NOTI_UpdatePmt,
	eCAS_NOTI_UpdateCat,
	eCAS_NOTI_UpdateSdt,
	eCAS_NOTI_UpdateNit,
	eCAS_NOTI_UpdateEit,
	eCAS_NOTI_UpdateTdt,
	eCAS_NOTI_UpdateBat,

	eCAS_NOTI_Max
} SvcCas_InstNotiType_e;

typedef struct
{
	SvcCas_InstanceType_e 		eCasInstType;									// cas instance type, (embedded cas, ci)
	DxCAS_GroupId_e				eCasId;											// ��ϵ� cas �� ca system id mask.
	HUINT32						ulSlot;											// �켱�� CAM�� slot ������ ���ϱ� ���ؼ� ���� �;�.. embedded CAS�� ���� ������ ����.
	HBOOL						bAllocated;										// cas instance is allcated by sub cas.
	HBOOL						bUsed;											// cas instance is used? session descramble ����.
	//bAvailable : ready for descramble?

	SvcCas_CbFuncList_t			stCbList;										// callback function list
	SvcCas_InstNotiType_e		eLastNotify;										// last sent callback.

	// CI �� ������ 1�� Slot �̸�, SC Slot�� 2�� �̻��� ��쿡��, Embeded CAS �� ���� �ʿ�
	HUINT32						ulNumOfSlot;									//slot�� ���� dev�ϳ��� �ΰ��� ����Ʈ ī�尡 �� ���? �켱 �߰� �س���. IR���� ��쿡 ����..
	HBOOL						bHaveDevice[CAS_MAX_NUM_OF_SLOT+1];									// cas instance device flag (inserted card?, inserted cam?)
	HUINT32						ulNumCaSysId[CAS_MAX_NUM_OF_SLOT+1];								// number of supported ca system id's
	HUINT16						ausCaSysId[CAS_MAX_NUM_OF_SLOT+1][CAS_MAX_NUM_OF_CA_SYSID];		// supported ca system id's list

	HUINT32						ulNumOfMaxSvc;								// �ִ� descramble ������ service �� CI �� ������ �Ѱ��� �ǵ��� ����..
	HUINT32 					ulCurRefCnt;									// ���� ��ϵ�(Descramble ��?) session count.
	HUINT32 					aulActionId[CAS_MAX_NUM_OF_ACTION]; //����� CAS_Context_t�� ActionId�� ������ �ִ�.

	SvcCas_CamState_e			eCamState;
	SvcCas_CiAppInfo_t			stCamAppInfo;
} SvcCas_DevInstance_t;


/********************************************************************
* Module Internal Function Prototypes
********************************************************************/
#define ___04_Module_Internal_Func_Prototypes___
HERROR svc_cas_InstanceInitModule(void);
SvcCas_DevInstance_t *svc_cas_InstanceGetCasDevInst(HUINT32 ulCasInstId);
SvcCas_InstanceType_e svc_cas_InstanceGetCasDevInstType(HUINT32 ulCasInstId);
HBOOL svc_cas_InstanceCompareCasId(DxCAS_GroupId_e eSourceId, DxCAS_GroupId_e eTargetId);
HBOOL svc_cas_InstanceCheckSmartCardValidityByCasId(DxCAS_GroupId_e eCasId);
HUINT32 svc_cas_InstanceGetNumOfCasDevInst(void);
HERROR svc_cas_InstanceGetCamState(HUINT16 usSlotId, SvcCas_CamState_e *eCamState);
HERROR svc_cas_InstanceGetCamName(HUINT16 usSlotId, HUINT8 **ppszCamInfo);
HERROR svc_cas_InstanceGetOpName(HUINT16 usSlotId, HUINT8 **ppszOpName);
HERROR svc_cas_InstanceRegisterCb(SvcCas_InstanceType_e	eCasInstType,
									DxCAS_GroupId_e		eCasId,
									HUINT32				ulSlot,
									SvcCas_CbFuncList_t	*pCbList,
									HUINT32				ulNumOfSlot,
									HUINT32				*pulCasInstanceId);
HERROR svc_cas_InstanceStartSession(SvcCas_Context_t *pstContext);
HERROR svc_cas_InstanceStopSession(SvcCas_Context_t *pstContext);
HERROR svc_cas_InstanceSetDeScramble(SvcCas_Context_t *pstContext);
HERROR svc_cas_InstanceManageCasInstance(SvcCas_Context_t *pstContext, HBOOL bConnect);
HERROR svc_cas_InstanceUpdatePmt(SvcCas_Context_t *pstContext);
#if defined (CAS_SUPPORT_CAT)
HERROR svc_cas_InstanceUpdateCat(SvcCas_Context_t *pstContext);
#endif
#if defined (CAS_SUPPORT_SDT)
HERROR svc_cas_InstanceUpdateSdt(SvcCas_Context_t *pContext);
#endif
HERROR svc_cas_InstanceUpdateCasInfo(SvcCas_CheckDeviceMsg_t *pInstInfo);
HERROR svc_cas_InstanceUpdateCamState(SvcCas_CheckDeviceMsg_t *pstMsg);
HERROR svc_cas_InstanceUpdateCamAppInfo(SvcCas_CheckDeviceMsg_t *pstMsg);
HERROR svc_cas_InstanceConnectCasInstance(SvcCas_Context_t *pstContext, HUINT32 ulCasInstId);
HERROR svc_cas_InstanceDisconnectCasInstance(SvcCas_Context_t *pstContext);
HBOOL svc_cas_InstanceIsAdaptedSCInserted(HUINT32 ulCaInstanceId, HUINT32 ulSlotId);

#if defined(CONFIG_MW_CI) && defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
void svc_cas_InstanceFindNotConnectPathCiNConnect(HUINT32 ulSearchedCasInstId);
#endif

#if defined(CONFIG_DEBUG)
HERROR svc_cas_InstancePrintCaInstTable(void);
HERROR svc_cas_InstancePrintCaInstTableById(HUINT32 ulCaInstanceId);
#endif

#endif /* !__CAS_MGR_INSTANCE_H__ */
