/**
	@file     xsvc_cas_scenario_default.c
	@brief   xsvc_cas_scenario_default.c (CAS Service)

	Description:
	Module:  \n
	Remarks :\n

	Copyright (c) 2012 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/********************************************************************
* Header Files
********************************************************************/
#define ___01_Header_Files___

#include <svc_cas.h>

#include "../../local_include/_xsvc_cas.h"
#include <_svc_cas_mgr_scenario.h>
#include "_xsvc_cas_scenario_default.h"
#include <_svc_cas_mgr_instance.h>

// TODO: swlee3 should be deleted
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
#define TEMP_ADD_SCENARIO // 검증후 define 정리하자.
#endif

/********************************************************************
* For Debugging
********************************************************************/
#define ___02_Debugging___


/********************************************************************
* Local Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Local_Definitions___


/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___04_Extern_Vars_Func_Prototypes___

/********************************************************************
* Static Variables / Function Prototypes
********************************************************************/
#define ___05_Local_Vars_Func_Prototypes___


/*	아래 시나리오들을 참고하여 해당 모델의 sub cas 선택 시나리오를 만들어야 한다.
	자신과 가장 가까운 시나리오를 모델로 참고 하면 될듯
	각 모델 별로 원하는 우선 순위로 그룹을 나열한 다음 각 그룹의 서브 아이템들을 원하는 우선순위로 다시 나열한다.
*/

#if defined(CAS_SELECT_SUBCAS_SCENARIO_1)
/* scenario 1
	embedded CAS가 한개 연결되어 있다.

*/
SvcCas_GroupOfPriority_t	sDefaultPriorityTbl_1[] =
{
	// 1순위 group 액션의 종류
	{	eItemOfPriority_G_KindOfAction,
		6,
		{ eItemOfPriority_Live, eItemOfPriority_PlayBack, eItemOfPriority_Tsr_Play,
			eItemOfPriority_Recording, eItemOfPriority_Descramble_a_File, eItemOfPriority_Tsr_Rec},
		{ 2, 2, 2, 1, 1, 1},
		xsvc_cas_DefScenarioMakePTFromGKindOfAction
	},
};
#elif defined(CAS_SELECT_SUBCAS_SCENARIO_2)
/* scenario 2
	CAM 한 개 있는 경우.
*/
SvcCas_GroupOfPriority_t	sDefaultPriorityTbl_2[] =
{
	// 1순위 Device의 유무
	{	eItemOfPriority_G_ExistDevice,
		0,
		{ },
		{ },
		xsvc_cas_DefScenarioMakePTFromGExistDevice
	},

	// 2순위 group 액션의 종류
	{	eItemOfPriority_G_KindOfAction,
		6,
		{ eItemOfPriority_Live, eItemOfPriority_PlayBack, eItemOfPriority_Tsr_Play,
			eItemOfPriority_Recording, eItemOfPriority_Descramble_a_File, eItemOfPriority_Tsr_Rec},
		{ 2, 2, 2, 1, 1, 1},
		xsvc_cas_DefScenarioMakePTFromGKindOfAction
	},
};
#elif defined(CAS_SELECT_SUBCAS_SCENARIO_3)
/* scenario 3
	CAM 한 개 embedded CAS 한 개 있는 경우.
*/
SvcCas_GroupOfPriority_t	sDefaultPriorityTbl_3[] =
{
	// 1순위 CAS System ID의 match
	{	eItemOfPriority_G_CasSystemIdMatch,
		0,
		{ },
		{ },
		xsvc_cas_DefScenarioMakePTFromGCasSystemIdMatch
	},

	// 2순위 Device의 유무
	{	eItemOfPriority_G_ExistDevice,
		0,
		{ },
		{ },
		xsvc_cas_DefScenarioMakePTFromGExistDevice
	},

	// 3순위 resource
	{	eItemOfPriority_G_Resource,
		0,
		{ },
		{ },
		xsvc_cas_DefScenarioMakePTFromGResource
	},

	// 4순위 group 디바이스의 종류
	{	eItemOfPriority_G_KindOfDevice,
		2,
		{ eItemOfPriority_ECAS, eItemOfPriority_CAM },
		{ },
		xsvc_cas_DefScenarioMakePTFromGKindOfDevice
	},

	// 5순위 group 액션의 종류 - aulPriority[] 가 높을 수록 priority가 높다.
	{	eItemOfPriority_G_KindOfAction,
		6,
		{ eItemOfPriority_Live, eItemOfPriority_PlayBack, eItemOfPriority_Tsr_Play,
			eItemOfPriority_Recording, eItemOfPriority_Descramble_a_File, eItemOfPriority_Tsr_Rec},
		{ 2, 2, 2, 1, 1, 1},
		xsvc_cas_DefScenarioMakePTFromGKindOfAction
	},
};
#elif defined(CAS_SELECT_SUBCAS_SCENARIO_4)
/* scenario 4
	Embedded CAS 두 개 있는 경우.
*/
SvcCas_GroupOfPriority_t	sDefaultPriorityTbl_4[] =
{
	// 1순위 CAS System ID의 match
	{	eItemOfPriority_G_CasSystemIdMatch,
		0,
		{ },
		{ },
		xsvc_cas_DefScenarioMakePTFromGCasSystemIdMatch
	},

	// 2순위 Device의 유무
	{	eItemOfPriority_G_ExistDevice,
		0,
		{ },
		{ },
		xsvc_cas_DefScenarioMakePTFromGExistDevice
	},

	// 3순위 group CAS 의 종류
	{	eItemOfPriority_G_KindOfCas,
		2,
		// 아이템 우선 순위가 높은 순서로 배열한다.
		{ eItemOfPriority_Nagra, eItemOfPriority_Conax },
		{ },
		xsvc_cas_DefScenarioMakePTFromGKindOfCas
	},

	// 4순위 resource
	{	eItemOfPriority_G_Resource,
		0,
		{ },
		{ },
		xsvc_cas_DefScenarioMakePTFromGResource
	},

	// 5순위 group 슬롯의 순서
	{	eItemOfPriority_G_OrderOfSlot,
		2,
		{ eItemOfPriority_SC0, eItemOfPriority_SC1 },
		{ },
		xsvc_cas_DefScenarioMakePTFromGSlot
	},

	// 6순위 group 액션의 종류 - aulPriority[] 가 높을 수록 priority가 높다.
	{	eItemOfPriority_G_KindOfAction,
		6,
		{ eItemOfPriority_Live, eItemOfPriority_PlayBack, eItemOfPriority_Tsr_Play,
			eItemOfPriority_Recording, eItemOfPriority_Descramble_a_File, eItemOfPriority_Tsr_Rec},
		{ 2, 2, 2, 1, 1, 1},
		xsvc_cas_DefScenarioMakePTFromGKindOfAction
	},
}
#elif defined(CAS_SELECT_SUBCAS_SCENARIO_5)
/* scenario 5
	CAM이 두개 있는경우.
*/
SvcCas_GroupOfPriority_t	sDefaultPriorityTbl_5[] =
{
	// 1순위 Device의 유무
	{	eItemOfPriority_G_ExistDevice,
		0,
		{ },
		{ },
		xsvc_cas_DefScenarioMakePTFromGExistDevice
	},


	// 2순위 CAM ID의 match
	{	eItemOfPriority_G_CamIdMatch,
		0,
		{ },
		{ },
		xsvc_cas_DefScenarioMakePTFromGCamIdMatch
	},

	// 3순위 CAS System ID의 match
	{	eItemOfPriority_G_CasSystemIdMatch,
		0,
		{ },
		{ },
		xsvc_cas_DefScenarioMakePTFromGCasSystemIdMatch
	},

	// 4순위 group 슬롯의 순서
	{	eItemOfPriority_G_OrderOfSlot,
		2,
		{ eItemOfPriority_CAM0, eItemOfPriority_CAM1 },
		{ },
#if 0
		xsvc_cas_DefScenarioMakePTFromGSlot
#else
		xsvc_cas_DefScenarioMakePTFromGSlotByDB
#endif
	},

	// 5순위 group 액션의 종류 - aulPriority[] 가 높을 수록 priority가 높다.
	{	eItemOfPriority_G_KindOfAction,
		6,
		{ eItemOfPriority_Live, eItemOfPriority_PlayBack, eItemOfPriority_Tsr_Play,
			eItemOfPriority_Recording, eItemOfPriority_Descramble_a_File, eItemOfPriority_Tsr_Rec},
		{ 2, 2, 2, 1, 1, 1},
		xsvc_cas_DefScenarioMakePTFromGKindOfAction
	},
};
#elif defined(CAS_SELECT_SUBCAS_SCENARIO_6)
/* scenario 6
	CAM두개 embedded CAS 한개 있는 경우.
*/
SvcCas_GroupOfPriority_t	sDefaultPriorityTbl_6[] =
{
	// 1순위 CAS System ID의 match
	{	eItemOfPriority_G_CasSystemIdMatch,
		0,
		{ },
		{ },
		xsvc_cas_DefScenarioMakePTFromGCasSystemIdMatch
	},

	// 2순위 Device의 유무
	{	eItemOfPriority_G_ExistDevice,
		0,
		{ },
		{ },
		xsvc_cas_DefScenarioMakePTFromGExistDevice
	},

	// 3순위 group CAS 의 종류
	{	eItemOfPriority_G_KindOfCas,
		5,
		// 아이템 우선 순위가 높은 순서로 배열한다.
		{ eItemOfPriority_CAM, eItemOfPriority_Nagra, eItemOfPriority_Conax, eItemOfPriority_Irdeto, eItemOfPriority_Viaccess, eItemOfPriority_UnknownCas},
		{ },
		xsvc_cas_DefScenarioMakePTFromGKindOfCas
	},

	// 4순위 resource
	{	eItemOfPriority_G_Resource,
		0,
		{ },
		{ },
		xsvc_cas_DefScenarioMakePTFromGResource
	},

	// 5순위 group 디바이스의 종류
	{	eItemOfPriority_G_KindOfDevice,
		2,
		{ eItemOfPriority_ECAS, eItemOfPriority_CAM },
		{ },
		xsvc_cas_DefScenarioMakePTFromGKindOfDevice
	},

	// 6순위 group 슬롯의 순서
	{	eItemOfPriority_G_OrderOfSlot,
		3,
		{ eItemOfPriority_SC0, eItemOfPriority_CAM0, eItemOfPriority_CAM1 },
		{ },
		xsvc_cas_DefScenarioMakePTFromGSlot
	},

	// 7순위 group 액션의 종류 - aulPriority[] 가 높을 수록 priority가 높다.
	{	eItemOfPriority_G_KindOfAction,
		6,
		{ eItemOfPriority_Live, eItemOfPriority_PlayBack, eItemOfPriority_Tsr_Play,
			eItemOfPriority_Recording, eItemOfPriority_Descramble_a_File, eItemOfPriority_Tsr_Rec},
		{ 2, 2, 2, 1, 1, 1},
		xsvc_cas_DefScenarioMakePTFromGKindOfAction
	},
};
#elif defined(CAS_SELECT_SUBCAS_SCENARIO_7)
/* scenario 7
	CAM 한 개 embedded CAS 두개 있는 경우.
*/
SvcCas_GroupOfPriority_t	sDefaultPriorityTbl_7[] =
{
	// 1순위 CAS System ID의 match
	{	eItemOfPriority_G_CasSystemIdMatch,
		0,
		{ },
		{ },
		xsvc_cas_DefScenarioMakePTFromGCasSystemIdMatch
	},

	// 2순위 Device의 유무
	{	eItemOfPriority_G_ExistDevice,
		0,
		{ },
		{ },
		xsvc_cas_DefScenarioMakePTFromGExistDevice
	},

	// 3순위 group CAS 의 종류
	{	eItemOfPriority_G_KindOfCas,
		5,
		// 아이템 우선 순위가 높은 순서로 배열한다.
		{ eItemOfPriority_CAM, eItemOfPriority_Nagra, eItemOfPriority_Conax, eItemOfPriority_Irdeto, eItemOfPriority_Viaccess, eItemOfPriority_UnknownCas},
		{ },
		xsvc_cas_DefScenarioMakePTFromGKindOfCas
	},

	// 4순위 resource
	{	eItemOfPriority_G_Resource,
		0,
		{ },
		{ },
		xsvc_cas_DefScenarioMakePTFromGResource
	},

	// 5순위 group 디바이스의 종류
	{	eItemOfPriority_G_KindOfDevice,
		2,
		{ eItemOfPriority_ECAS, eItemOfPriority_CAM },
		{ },
		xsvc_cas_DefScenarioMakePTFromGKindOfDevice
	},

	// 6순위 group 슬롯의 순서
	{	eItemOfPriority_G_OrderOfSlot,
		3,
		{ eItemOfPriority_SC0, eItemOfPriority_SC1, eItemOfPriority_CAM0 },
		{ },
		xsvc_cas_DefScenarioMakePTFromGSlot
	},

	// 7순위 group 액션의 종류 - aulPriority[] 가 높을 수록 priority가 높다.
	{	eItemOfPriority_G_KindOfAction,
		6,
		{ eItemOfPriority_Live, eItemOfPriority_PlayBack, eItemOfPriority_Tsr_Play,
			eItemOfPriority_Recording, eItemOfPriority_Descramble_a_File, eItemOfPriority_Tsr_Rec},
		{ 2, 2, 2, 1, 1, 1},
		xsvc_cas_DefScenarioMakePTFromGKindOfAction
	},
};
#elif defined(CAS_SELECT_SUBCAS_SCENARIO_8)
/* scenario 8
	CAM 두 개 embedded CAS 두개 있는 경우.
*/
SvcCas_GroupOfPriority_t	sDefaultPriorityTbl_8[] =
{
	// 1순위 CAS System ID의 match
	{	eItemOfPriority_G_CasSystemIdMatch,
		0,
		{ },
		{ },
		xsvc_cas_DefScenarioMakePTFromGCasSystemIdMatch
	},

	// 2순위 Device의 유무
	{	eItemOfPriority_G_ExistDevice,
		0,
		{ },
		{ },
		xsvc_cas_DefScenarioMakePTFromGExistDevice
	},

	// 3순위 group CAS 의 종류
	{	eItemOfPriority_G_KindOfCas,
		5,
		// 아이템 우선 순위가 높은 순서로 배열한다.
		{ eItemOfPriority_CAM, eItemOfPriority_Nagra, eItemOfPriority_Conax, eItemOfPriority_Irdeto, eItemOfPriority_Viaccess, eItemOfPriority_UnknownCas},
		{ },
		xsvc_cas_DefScenarioMakePTFromGKindOfCas
	},

	// 4순위 resource
	{	eItemOfPriority_G_Resource,
		0,
		{ },
		{ },
		xsvc_cas_DefScenarioMakePTFromGResource
	},

	// 5순위 group 디바이스의 종류
	{	eItemOfPriority_G_KindOfDevice,
		2,
		{ eItemOfPriority_ECAS, eItemOfPriority_CAM },
		{ },
		xsvc_cas_DefScenarioMakePTFromGKindOfDevice
	},

	// 6순위 group 슬롯의 순서
	{	eItemOfPriority_G_OrderOfSlot,
		4,
		{ eItemOfPriority_SC0, eItemOfPriority_SC1, eItemOfPriority_CAM0, eItemOfPriority_CAM1 },
		{ },
		xsvc_cas_DefScenarioMakePTFromGSlot
	},

	// 7순위 group 액션의 종류 - aulPriority[] 가 높을 수록 priority가 높다.
	{	eItemOfPriority_G_KindOfAction,
		6,
		{ eItemOfPriority_Live, eItemOfPriority_PlayBack, eItemOfPriority_Tsr_Play,
			eItemOfPriority_Recording, eItemOfPriority_Descramble_a_File, eItemOfPriority_Tsr_Rec},
		{ 2, 2, 2, 1, 1, 1},
		xsvc_cas_DefScenarioMakePTFromGKindOfAction
	},
};

#else
/* default 는 scenario 1 하고 같다.
	embedded CAS가 한개 연결되어 있다.
*/
SvcCas_GroupOfPriority_t	sDefaultPriorityTbl[] =
{
	/* 1순위 Device의 유무
	*/
	{	eItemOfPriority_G_ExistDevice,
		0,
		{ },
		{ },
		xsvc_cas_DefScenarioMakePTFromGExistDevice,
	},

	// 2순위 group 액션의 종류
	{	eItemOfPriority_G_KindOfAction,
		6,
		{ eItemOfPriority_Live, eItemOfPriority_PlayBack, eItemOfPriority_Tsr_Play,
			eItemOfPriority_Recording, eItemOfPriority_Descramble_a_File, eItemOfPriority_Tsr_Rec},
		{ 2, 2, 2, 1, 1, 1},
		xsvc_cas_DefScenarioMakePTFromGKindOfAction
	},
};
#endif

/********************************************************************
* Static Functions
* Naming rule : "local"_"CasDefScenario" _ "Function Name"
********************************************************************/
#define ___06_Local_APIs___

#if defined(CAS_SELECT_SUBCAS_SCENARIO_1)
HERROR xsvc_cas_Scenario1(void)
{
	HUINT32			i;
	HUINT32			unNumOfTable;
	HERROR			hErr;

	unNumOfTable = (sizeof(sDefaultPriorityTbl_1) / sizeof(SvcCas_GroupOfPriority_t));

	for(i = 0; i < unNumOfTable; i++)
	{
		hErr = svc_cas_MgrSubRegisterPriorityTable(&sDefaultPriorityTbl_1[i]);
	}

	(void)(hErr);

	return ERR_OK;
}
#elif defined(CAS_SELECT_SUBCAS_SCENARIO_2)
HERROR xsvc_cas_Scenario2(void)
{
	HUINT32			i;
	HUINT32			unNumOfTable;
	HERROR			hErr;

	unNumOfTable = (sizeof(sDefaultPriorityTbl_2) / sizeof(SvcCas_GroupOfPriority_t));

	for(i = 0; i < unNumOfTable; i++)
	{
		hErr = svc_cas_MgrSubRegisterPriorityTable(&sDefaultPriorityTbl_2[i]);
	}

	return ERR_OK;
}
#elif defined(CAS_SELECT_SUBCAS_SCENARIO_3)
HERROR xsvc_cas_Scenario3(void)
{
	HUINT32			i;
	HUINT32			unNumOfTable;
	HERROR			hErr;

	unNumOfTable = (sizeof(sDefaultPriorityTbl_3) / sizeof(SvcCas_GroupOfPriority_t));

	for(i = 0; i < unNumOfTable; i++)
	{
		hErr = svc_cas_MgrSubRegisterPriorityTable(&sDefaultPriorityTbl_3[i]);
	}

	return ERR_OK;
}
#elif defined(CAS_SELECT_SUBCAS_SCENARIO_4)
HERROR xsvc_cas_Scenario4(void)
{
	HUINT32			i;
	HUINT32			unNumOfTable;
	HERROR			hErr;

	unNumOfTable = (sizeof(sDefaultPriorityTbl_4) / sizeof(SvcCas_GroupOfPriority_t));

	for(i = 0; i < unNumOfTable; i++)
	{
		hErr = svc_cas_MgrSubRegisterPriorityTable(&sDefaultPriorityTbl_4[i]);
	}

	return ERR_OK;
}
#elif defined(CAS_SELECT_SUBCAS_SCENARIO_5)
HERROR xsvc_cas_Scenario5(void)
{
	HUINT32			i;
	HUINT32			unNumOfTable;
	HERROR			hErr;

	unNumOfTable = (sizeof(sDefaultPriorityTbl_5) / sizeof(SvcCas_GroupOfPriority_t));

	HxLOG_Info("unNumOfTable = %d\n", unNumOfTable);

	for(i = 0; i < unNumOfTable; i++)
	{
		hErr = svc_cas_MgrSubRegisterPriorityTable(&sDefaultPriorityTbl_5[i]);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("(%d) register priority table error\n", i);
		}
	}

	return ERR_OK;
}
#elif defined(CAS_SELECT_SUBCAS_SCENARIO_6)
HERROR xsvc_cas_Scenario6(void)
{
	HUINT32			i;
	HUINT32			unNumOfTable;
	HERROR			hErr;

	unNumOfTable = (sizeof(sDefaultPriorityTbl_6) / sizeof(SvcCas_GroupOfPriority_t));

	for(i = 0; i < unNumOfTable; i++)
	{
		hErr = svc_cas_MgrSubRegisterPriorityTable(&sDefaultPriorityTbl_6[i]);
	}

	return ERR_OK;
}
#elif defined(CAS_SELECT_SUBCAS_SCENARIO_7)
HERROR xsvc_cas_Scenario7(void)
{
	HUINT32			i;
	HUINT32			unNumOfTable;
	HERROR			hErr;

	unNumOfTable = (sizeof(sDefaultPriorityTbl_7) / sizeof(SvcCas_GroupOfPriority_t));

	for(i = 0; i < unNumOfTable; i++)
	{
		hErr = svc_cas_MgrSubRegisterPriorityTable(&sDefaultPriorityTbl_7[i]);
	}

	return ERR_OK;
}
#elif defined(CAS_SELECT_SUBCAS_SCENARIO_8)
HERROR xsvc_cas_Scenario8(void)
{
	HUINT32			i;
	HUINT32			unNumOfTable;
	HERROR			hErr;

	unNumOfTable = (sizeof(sDefaultPriorityTbl_8) / sizeof(SvcCas_GroupOfPriority_t));

	for(i = 0; i < unNumOfTable; i++)
	{
		hErr = svc_cas_MgrSubRegisterPriorityTable(&sDefaultPriorityTbl_8[i]);
	}

	return ERR_OK;
}
#else
HERROR xsvc_cas_ScenarioDefault(void)
{
	HUINT32			i;
	HUINT32			unNumOfTable;
	HERROR			hErr;

	unNumOfTable = (sizeof(sDefaultPriorityTbl) / sizeof(SvcCas_GroupOfPriority_t));

	for(i = 0; i < unNumOfTable; i++)
	{
		hErr = svc_cas_MgrSubRegisterPriorityTable(&sDefaultPriorityTbl[i]);
	}

	return ERR_OK;
}
#endif

STATIC DxCAS_GroupId_e xsvc_cas_ScenarioGetCasGroupIdByItemOfPriority(SvcCas_ItemOfPriority_e eItemOfPriority)
{
	switch(eItemOfPriority)
	{
	case eItemOfPriority_Conax:	return eDxCAS_GROUPID_CX;
	case eItemOfPriority_Irdeto:	return eDxCAS_GROUPID_IR;
	case eItemOfPriority_Nagra:	return eDxCAS_GROUPID_NA;
	case eItemOfPriority_satsa:		return eDxCAS_GROUPID_SATSA;
	case eItemOfPriority_Viaccess:	return eDxCAS_GROUPID_VA;
	case eItemOfPriority_MG:		return eDxCAS_GROUPID_MG;
	case eItemOfPriority_CR:		return eDxCAS_GROUPID_CR;
	case eItemOfPriority_ND:		return eDxCAS_GROUPID_ND;
	case eItemOfPriority_CAM:		return eDxCAS_GROUPID_CAM;

	case eItemOfPriority_UnknownCas:
	default: return eDxCAS_GROUPID_NONE;
	}
}

STATIC HERROR	xsvc_cas_ScenarioIsCasGroupIdMatchByCasPriority(SvcCas_MgrPriorityTreeItem_t *pstItem, SvcCas_ItemOfPriority_e eItemOfPriority)
{
	DxCAS_GroupId_e			eCasGroupId;
	DxCAS_GroupId_e			eCasInstGroupId;

	eCasInstGroupId = svc_cas_MgrSubGetCasGroupIdFromCasInst(pstItem->unCasInstTableId);

	eCasGroupId = xsvc_cas_ScenarioGetCasGroupIdByItemOfPriority(eItemOfPriority);

	if(eCasInstGroupId == eCasGroupId)
	{
		return ERR_OK;
	}

	return ERR_FAIL;
}

#if 0
STATIC HUINT32  xsvc_cas_ScenarioNumOfCaSystemMatchItemByPmt(SvcCas_MgrPriorityTreeItem_t *pstPriorityTree, SvcCas_Context_t *pstCasContext)
{
	HUINT32				ulNum = 0;
	HERROR				hErr;

	while(pstPriorityTree)
	{
		hErr = xsvc_cas_ScenarioIsCasSystemIdMatch(pstPriorityTree, pstCasContext);
		if(hErr == ERR_OK)
		{
			ulNum++;
		}

		pstPriorityTree = pstPriorityTree->pstNextSilbing;
	}

	return ulNum;
}
#endif

STATIC HUINT32  xsvc_cas_ScenarioNumOfCasGIdNotMatchItemByCasPriority(SvcCas_MgrPriorityTreeItem_t *pstPriorityTree, SvcCas_ItemOfPriority_e eItemOfPriority)
{
	HUINT32		unNumOfMatch = 0;
	HERROR		hErr;

	while(pstPriorityTree)
	{
		hErr = xsvc_cas_ScenarioIsCasGroupIdMatchByCasPriority(pstPriorityTree, eItemOfPriority);
		if(hErr != ERR_OK)
		{
			unNumOfMatch++;
		}

		pstPriorityTree = pstPriorityTree->pstNextSilbing;
	}

	return unNumOfMatch;
}

STATIC HUINT32 xsvc_cas_ScenarioCountPriorityTreeItemFromSameLevel(SvcCas_MgrPriorityTreeItem_t *pstPriorityTree)
{
	HUINT32		ulNumOfItem = 0;

	while(pstPriorityTree != NULL)
	{
		ulNumOfItem++;
		pstPriorityTree = pstPriorityTree->pstNextSilbing;
	}

	return ulNumOfItem;
}

STATIC SvcCas_InstanceType_e xsvc_cas_ScenarioGetInstType(SvcCas_ItemOfPriority_e eItemOfPriority, HUINT32 *pulSlot)
{
	switch(eItemOfPriority)
	{
	case eItemOfPriority_SC0:		*pulSlot = 0; return eCasInst_Embedded;
	case eItemOfPriority_SC1:		*pulSlot = 1; return eCasInst_Embedded;
	case eItemOfPriority_CAM0:		*pulSlot = 0; return eCasInst_CI;
	case eItemOfPriority_CAM1:		*pulSlot = 1; return eCasInst_CI;
	default: 						*pulSlot = 0; return eCasInst_None;
	}
}

STATIC HUINT32 xsvc_cas_ScenarioGetNumOfDefSlot(SvcCas_MgrPriorityTreeItem_t *pstItem, SvcCas_ItemOfPriority_e eItemOfPriority)
{
	HUINT32					ulNumOfDefSlot = 0;
	HERROR					hErr;
	HUINT32					ulSlot;
	SvcCas_InstanceType_e	eCasInstType;

	eCasInstType = xsvc_cas_ScenarioGetInstType(eItemOfPriority, &ulSlot);

	HxLOG_Info("eCasInstType = %d, ulSlot = %d\n", eCasInstType, ulSlot);

	while(pstItem)
	{
		hErr = svc_cas_MgrSubIsSameDefSlot(pstItem, eCasInstType, ulSlot);
		if(hErr == ERR_FAIL)
		{
			ulNumOfDefSlot++;
		}

		pstItem = pstItem->pstNextSilbing;
	}

	return ulNumOfDefSlot;
}

STATIC HERROR xsvc_cas_ScenarioIsSameDefSlotFromItem(SvcCas_MgrPriorityTreeItem_t *pstItem, SvcCas_ItemOfPriority_e eItemOfPriority)
{
	HUINT32					ulSlot;
	SvcCas_InstanceType_e	eCasInstType;

	eCasInstType = xsvc_cas_ScenarioGetInstType(eItemOfPriority, &ulSlot);

	return svc_cas_MgrSubIsSameDefSlot(pstItem, eCasInstType, ulSlot);
}

STATIC HUINT32 xsvc_cas_ScenarioGetNumOfDeviceMatchItem(SvcCas_MgrPriorityTreeItem_t *pstFirstItem, SvcCas_ItemOfPriority_e eItemOfPriority)
{
	HUINT32		ulNumOfMatchItem = 0;
	HERROR		hErr;

	while(pstFirstItem)
	{
		hErr = svc_cas_MgrSubIsDeviceMatchItem(pstFirstItem, eItemOfPriority);
		if(hErr == ERR_OK)
		{
			ulNumOfMatchItem++;
		}

		pstFirstItem = pstFirstItem->pstNextSilbing;
	}

	return ulNumOfMatchItem;
}

STATIC void xsvc_cas_ScenarioMakePTFromGKindOfCas (SvcCas_MgrPriorityTreeMgr_t		*pstTreeMgr,
													SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
													SvcCas_ItemOfPriority_e			 eItemOfPriority)
{
	HUINT32 						ulNumOfItem;
	HUINT32 						ulNumOfMoveItem;
	HERROR							hErr;
	SvcCas_MgrPriorityTreeItem_t	*pstTmpFirstItem = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstTmpItem = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstMoveItems = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstDeleteItem = NULL;

	if(pstTreeMgr == NULL || pstFirstItem == NULL)
	{
		HxLOG_Info("pstTreeMgr = 0x%x, pstFirstItem = 0x%x\n", pstTreeMgr, pstFirstItem);
		return;
	}

/* 현재 tree level에 우선 순위가 낮아서 아래로 옮겨야 할 Item이 있는가?
	없으면 다음 level에서 찾도록 recursive 호출을 하도록 하자.
	옮겨야할 아이템과 개수와  현재 level의 아이템 개수가 같으면 마찬 가지로  recursive 호출
*/
	pstTmpFirstItem = pstFirstItem;
	ulNumOfItem = xsvc_cas_ScenarioCountPriorityTreeItemFromSameLevel(pstTmpFirstItem);
	ulNumOfMoveItem = xsvc_cas_ScenarioNumOfCasGIdNotMatchItemByCasPriority(pstTmpFirstItem, eItemOfPriority);

	HxLOG_Info("ulNumOfItem = %d, ulNumOfMoveItem = %d\n", ulNumOfItem, ulNumOfMoveItem);
	if(ulNumOfMoveItem == 0 || ulNumOfItem == ulNumOfMoveItem)
	{
		/* 다음 레벨로 이동.*/
		xsvc_cas_ScenarioMakePTFromGKindOfCas(pstTreeMgr, pstFirstItem->pstChild, eItemOfPriority);
		return;
	}

/* 옮겨야 할 item을 찾는다.
	아이템이 찾아 지면 삭제 한다. match가 되지 않은 놈은 tree에서 제외 하도록 함.
*/
	pstTmpItem = pstTmpFirstItem;
	while(pstTmpItem)
	{
		hErr = xsvc_cas_ScenarioIsCasGroupIdMatchByCasPriority(pstTmpItem, eItemOfPriority);
		if(hErr != ERR_OK)
		{
			pstDeleteItem = pstTmpItem;
		}
		pstTmpItem = pstTmpItem->pstNextSilbing;

		if(pstDeleteItem != NULL)
		{
			/*	match 되지 않으면 tree에서 제거후 pstMoveItems 으로 옮긴 뒤 후에 한꺼 번에 아래 level로 옮긴다.
			*/
			pstTmpFirstItem = svc_cas_ScenarioMakeMoveItem(pstTreeMgr, pstTmpFirstItem, pstDeleteItem, &pstMoveItems);
			pstDeleteItem = NULL;
		}
	}

/* 옮길 아이템들을 바로 아래 tree로 붙인다. - 실제 옮기는 작업임..*/
	if(pstMoveItems != NULL)
	{
		pstTmpFirstItem = svc_cas_ScenarioMovePriorityTreeItems(pstTmpFirstItem, pstMoveItems);
	}

/* 다음 레벨로 이동.*/
	if(pstTmpFirstItem != NULL)
	{
		xsvc_cas_ScenarioMakePTFromGKindOfCas(pstTreeMgr, pstTmpFirstItem->pstChild, eItemOfPriority);
	}

	return;
}

STATIC void xsvc_cas_ScenarioMakePTFromGSlot (SvcCas_MgrPriorityTreeMgr_t		*pstTreeMgr,
											SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
											SvcCas_ItemOfPriority_e			 eItemOfPriority)
{
	HUINT32 					ulNumOfItem;
	HUINT32 					ulNumOfMoveItem;
	HERROR					hErr;
	SvcCas_MgrPriorityTreeItem_t	*pstTmpFirstItem = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstTmpItem = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstMoveItems = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstDeleteItem = NULL;

	if(pstTreeMgr == NULL || pstFirstItem == NULL)
	{
		HxLOG_Info("pstTreeMgr = 0x%x, pstFirstItem = 0x%x\n", pstTreeMgr, pstFirstItem);
		return;
	}

/* 현재 tree level에 우선 순위가 낮아서 아래로 옮겨야 할 Item이 있는가?
	없으면 다음 level에서 찾도록 recursive 호출을 하도록 하자.
	옮겨야할 아이템과 개수와  현재 level의 아이템 개수가 같으면 마찬 가지로  recursive 호출
*/
	pstTmpFirstItem = pstFirstItem;
	ulNumOfItem = xsvc_cas_ScenarioCountPriorityTreeItemFromSameLevel(pstTmpFirstItem);
	ulNumOfMoveItem = xsvc_cas_ScenarioGetNumOfDefSlot(pstTmpFirstItem, eItemOfPriority);

	HxLOG_Info("ulNumOfItem = %d, ulNumOfMoveItem = %d\n", ulNumOfItem, ulNumOfMoveItem);
	if(ulNumOfMoveItem == 0 || ulNumOfItem == ulNumOfMoveItem)
	{
		/* 다음 레벨로 이동.*/
		xsvc_cas_ScenarioMakePTFromGSlot(pstTreeMgr, pstTmpFirstItem->pstChild, eItemOfPriority);
		return;
	}

/* 옮겨야 할 item을 찾는다.
	우선 순위에 따라서 slot을 정렬 하도록 한다.
*/
	pstTmpItem = pstTmpFirstItem;
	while(pstTmpItem)
	{
		hErr = xsvc_cas_ScenarioIsSameDefSlotFromItem(pstTmpItem, eItemOfPriority);
		if(hErr != ERR_OK)
		{
			pstDeleteItem = pstTmpItem;
		}
		pstTmpItem = pstTmpItem->pstNextSilbing;

		if(pstDeleteItem != NULL)
		{
			/*	match 되지 않으면 tree에서 제거후 pstMoveItems 으로 옮긴 뒤 후에 한꺼 번에 아래 level로 옮긴다.
			*/
			pstTmpFirstItem = svc_cas_ScenarioMakeMoveItem(pstTreeMgr, pstTmpFirstItem, pstDeleteItem, &pstMoveItems);
			pstDeleteItem = NULL;
		}
	}

/* 옮길 아이템들을 바로 아래 tree로 붙인다. - 실제 옮기는 작업임..*/
	if(pstMoveItems != NULL)
	{
		pstTmpFirstItem = svc_cas_ScenarioMovePriorityTreeItems(pstTmpFirstItem, pstMoveItems);
	}

/* 다음 레벨로 이동.*/
	if(pstTmpFirstItem != NULL)
	{
		xsvc_cas_ScenarioMakePTFromGSlot(pstTreeMgr, pstTmpFirstItem->pstChild, eItemOfPriority);
	}

	return;
}

/********************************************************************
* Module Internal Functions
* Naming rule : "CasDefScenario" _ "Function Name"
********************************************************************/
#define ___07_Module_Internal_APIs___

void xsvc_cas_DefScenarioMakePTFromGExistDevice (SvcCas_MgrPriorityTreeMgr_t		*pstTreeMgr,
													SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
													HUINT32							 ulPriorityTableId,
													HUINT32							 ulActionId)
{
	//HUINT32					ulNumOfItem;
	//HUINT32					ulNumOfMoveItem;
	HERROR							hErr;
	SvcCas_InstanceType_e			eCasInstType;
	SvcCas_MgrPriorityTreeItem_t	*pstTmpFirstItem = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstTmpItem = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstDeleteItem = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstMoveItems = NULL;

	HxLOG_Info("++++++++++++++++++++++++++++++++\n");
	HxLOG_Info("xsvc_cas_DefScenarioMakePTFromGExistDevice() +\n");
	HxLOG_Info("++++++++++++++++++++++++++++++++\n");

	if(pstTreeMgr == NULL || pstFirstItem == NULL)
	{
		HxLOG_Info("pstTreeMgr = 0x%x, pstFirstItem = 0x%x\n", pstTreeMgr, pstFirstItem);
		return;
	}

	pstTmpFirstItem = pstFirstItem;

#if 0
/* 현재 tree level에 우선 순위가 낮아서 아래로 옮겨야 할 Item이 있는가?
	없으면 다음 level에서 찾도록 recursive 호출을 하도록 하자.
	옮겨야할 아이템과 개수와  현재 level의 아이템 개수가 같으면 마찬 가지로  recursive 호출
*/
	ulNumOfItem = svc_cas_InstanceCountPriorityTreeItemFromSameLevel(pstTmpFirstItem);
	ulNumOfMoveItem = xsvc_cas_ScenarioGetNumOfNotExistDevice(pstTmpFirstItem);

	HxLOG_Info("ulNumOfItem = %d, ulNumOfMoveItem = %d\n", ulNumOfItem, ulNumOfMoveItem);
	if(ulNumOfMoveItem == 0 || ulNumOfItem == ulNumOfMoveItem)
	{
		xsvc_cas_ScenarioMakePTFromGExistDevice(pstTreeMgr, pstTmpFirstItem->pstChild, ulPriorityTableId, ulActionId);

		HxLOG_Info("xsvc_cas_ScenarioMakePTFromGExistDevice() -\n");
		return;
	}
#endif

/* 옮겨야 할 item을 찾는다.
	insert되지 않은 cam은 tree에서 제외 하도록 함.
	embedded는 우선 순위가 밀리도록함.
*/
	pstTmpItem = pstTmpFirstItem;
	while(pstTmpItem)
	{
		hErr = svc_cas_MgrSubIsExistDevice(pstTmpItem, &eCasInstType);
		if(hErr != ERR_OK)
		{
			pstDeleteItem = pstTmpItem;
		}
		pstTmpItem = pstTmpItem->pstNextSilbing;

		if(pstDeleteItem != NULL)
		{
			if(eCasInstType == eCasInst_Embedded)
			{
				/*	embedded cas는 smart card가 없으면 우선 순위에서 밀리도록한다.
				*/
				pstTmpFirstItem = svc_cas_ScenarioMakeMoveItem(pstTreeMgr, pstTmpFirstItem, pstDeleteItem, &pstMoveItems);
			}
			else
			{
				/*	CAM은 device가 없으면 아예 삭제 되도록 한다.
				*/
				pstTmpFirstItem = svc_cas_ScenarioDeletePriorityTreeItem(pstTreeMgr, pstTmpFirstItem, pstDeleteItem);
			}
			pstDeleteItem = NULL;
		}
	}

	if(pstMoveItems != NULL)
	{
		pstTmpFirstItem = svc_cas_ScenarioMovePriorityTreeItems(pstTmpFirstItem, pstMoveItems);
	}

	if(pstTmpFirstItem != NULL)
	{
		xsvc_cas_DefScenarioMakePTFromGExistDevice(pstTreeMgr, pstTmpFirstItem->pstChild, ulPriorityTableId, ulActionId);
	}

	HxLOG_Info("xsvc_cas_ScenarioMakePTFromGExistDevice() -\n");

	return;
}

void xsvc_cas_DefScenarioMakePTFromGCamIdMatch (SvcCas_MgrPriorityTreeMgr_t		*pstTreeMgr,
												SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
												HUINT32							 ulPriorityTableId,
												HUINT32							 ulActionId)
{
	//HUINT32 					ulNumOfItem;
	//HUINT32 					ulNumOfMoveItem;
	HERROR					hErr;
	void 					*pvoid = NULL;
	HUINT8					aucRequestedCamId[8];
	SvcCas_MgrPriorityTreeItem_t	*pstTmpFirstItem = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstTmpItem = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstDeleteItem = NULL;

	HxLOG_Info("++++++++++++++++++++++++++++++++\n");
	HxLOG_Info("xsvc_cas_DefScenarioMakePTFromGCamIdMatch() +\n");
	HxLOG_Info("++++++++++++++++++++++++++++++++\n");

	if(pstTreeMgr == NULL || pstFirstItem == NULL)
	{
		HxLOG_Info("pstTreeMgr = 0x%x, pstFirstItem = 0x%x\n", pstTreeMgr, pstFirstItem);
		return;
	}

	/* CAS start시 request 된 CAM ID가 없으면 여기는 PASS 하도록 한다. */
	hErr = svc_cas_MgrSubGetRequestedCamIdByActionId(ulActionId, aucRequestedCamId);
	if(hErr != ERR_OK)
	{
		HxLOG_Info("requested cam id doesn't exist\n");
		return;
	}

	HxLOG_Info(" 0x(%x, %x, %x, %x,%x,%x,%x,%x)", aucRequestedCamId[0], aucRequestedCamId[1], aucRequestedCamId[2], aucRequestedCamId[3],
		aucRequestedCamId[4], aucRequestedCamId[5], aucRequestedCamId[6], aucRequestedCamId[7]);

	pvoid = svc_cas_MgrSubGetActionContextByActionId(ulActionId);
	if(pvoid == NULL)
	{
		HxLOG_Error("pvoid is NULL\n");
		return;
	}

	pstTmpFirstItem = pstFirstItem;

/* 삭제 해야 할 item을 찾는다.
	아이템이 찾아 지면 삭제 한다. level을 낮추어서 tree에 붙인다.
*/
	pstTmpItem = pstTmpFirstItem;
	while(pstTmpItem)
	{
		hErr = svc_cas_MgrSubIsCamIdMatch(pstTmpItem, pvoid);
		if(hErr != ERR_OK)
		{
			pstDeleteItem = pstTmpItem;
		}
		pstTmpItem = pstTmpItem->pstNextSilbing;

		if(pstDeleteItem != NULL)
		{
			/*	CAM ID가 일치 하지 않으면 아예 삭제 되도록 한다.
			*/
			pstTmpFirstItem = svc_cas_ScenarioDeletePriorityTreeItem(pstTreeMgr, pstTmpFirstItem, pstDeleteItem);

			pstDeleteItem = NULL;
		}
	}

/* 다음 레벨로 이동.*/
	if(pstTmpFirstItem != NULL)
	{
		xsvc_cas_DefScenarioMakePTFromGCamIdMatch(pstTreeMgr, pstTmpFirstItem->pstChild, ulPriorityTableId, ulActionId);
	}

	HxLOG_Info("xsvc_cas_ScenarioMakePTFromGCamIdMatch() -\n");
	return;
}

void xsvc_cas_DefScenarioMakePTFromGCasSystemIdMatch (SvcCas_MgrPriorityTreeMgr_t		*pstTreeMgr,
														SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
														HUINT32							 ulPriorityTableId,
														HUINT32							 ulActionId)
{
	//HUINT32 					ulNumOfItem;
	//HUINT32 					ulNumOfMoveItem;
	HERROR					hErr;
	void 					*pvoid = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstTmpFirstItem = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstTmpItem = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstMoveItems = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstDeleteItem = NULL;
#if defined(TEMP_ADD_SCENARIO)
	SvcCas_Context_t * pstCasContext = NULL;
#endif

	HxLOG_Info("++++++++++++++++++++++++++++++++\n");
	HxLOG_Info("xsvc_cas_DefScenarioMakePTFromGCasSystemIdMatch() +\n");
	HxLOG_Info("++++++++++++++++++++++++++++++++\n");

	if(pstTreeMgr == NULL || pstFirstItem == NULL)
	{
		HxLOG_Info("pstTreeMgr = 0x%x, pstFirstItem = 0x%x\n", pstTreeMgr, pstFirstItem);
		return;
	}

	pvoid = svc_cas_MgrSubGetActionContextByActionId(ulActionId);
	if(pvoid == NULL)
	{
		HxLOG_Error("pvoid is NULL\n");
		return;
	}

	pstTmpFirstItem = pstFirstItem;

/* CAM에 FTA 서비스도 당근 보내야 한다.. 특별한 이유가 아니면 CAM에는 항상 TS가 들어가야 한다.
*/
#if defined(TEMP_ADD_SCENARIO)
		/* FTA 면 CAM은 우선 순위에서 제외 된다..
			Embedded CAS는 FTA도 우선순위에 들어가서 선택되도록하자..
			추후 이 시나리오를 변경하려면 eItemOfPriority_G_CasSystemIdMatch 에 시나리오 우선 순위를 추가하도록 하자~
		*/
		pstCasContext = (SvcCas_Context_t *)pvoid;
		if(pstCasContext)
		{
			if(pstCasContext->eChType == eCasChannel_FTA)
			{
				SvcCas_DevInstance_t	*pstCasInst = NULL;

				pstTmpItem = pstTmpFirstItem;
				while(pstTmpItem)
				{
					pstCasInst = svc_cas_InstanceGetCasDevInst(pstTmpItem->unCasInstTableId);
					if((pstCasInst != NULL) && (pstCasInst->eCasInstType == eCasInst_CI))
					{
						pstDeleteItem = pstTmpItem;
					}
					pstTmpItem = pstTmpItem->pstNextSilbing;

					if(pstDeleteItem != NULL)
					{
						pstTmpFirstItem = svc_cas_ScenarioDeletePriorityTreeItem(pstTreeMgr, pstTmpFirstItem, pstDeleteItem);
					}
					pstDeleteItem = NULL;
				}

				return;
			}
		}
#endif

#if 0
/* 현재 tree level에 우선 순위가 낮아서 아래로 옮겨야 할 Item이 있는가?
	없으면 다음 level에서 찾도록 recursive 호출을 하도록 하자.
	옮겨야할 아이템과 개수와  현재 level의 아이템 개수가 같으면 마찬 가지로  recursive 호출
	옮겨야할 아이템은 ->System Id가 일치하지 않은 것들
*/
	ulNumOfItem = svc_cas_InstanceCountPriorityTreeItemFromSameLevel(pstTmpFirstItem);
	ulNumOfMoveItem = xsvc_cas_ScenarioNumOfCaSystemMatchItemByPmt(pstTmpFirstItem, pstCasContext);

	HxLOG_Info("ulNumOfItem = %d, ulNumOfMoveItem = %d\n", ulNumOfItem, ulNumOfMoveItem);
	if(ulNumOfMoveItem == 0 || ulNumOfItem == ulNumOfMoveItem)
	{
		/* 다음 레벨로 이동.*/
		xsvc_cas_DefScenarioMakePTFromGCasSystemIdMatch(pstTreeMgr, pstTmpFirstItem->pstChild, ulPriorityTableId, ulActionId);
		HxLOG_Info("xsvc_cas_ScenarioMakePTFromGCasSystemIdMatch() -\n");
		return;
	}
#endif

/* 옮겨야 할 item을 찾는다.
	아이템이 찾아 지면 삭제 한다. level을 낮추어서 tree에 붙인다.
*/
	pstTmpItem = pstTmpFirstItem;
	while(pstTmpItem)
	{
		hErr = svc_cas_MgrSubIsCasSystemIdMatch(pstTmpItem, pvoid);
		if(hErr != ERR_OK)
		{
			pstDeleteItem = pstTmpItem;
		}
		pstTmpItem = pstTmpItem->pstNextSilbing;

		if(pstDeleteItem != NULL)
		{
			SvcCas_InstanceType_e	eCasInstType;

			eCasInstType = svc_cas_MgrSubGetCasDevInstType(pstDeleteItem->unCasInstTableId);

			if(eCasInstType == eCasInst_Embedded)
			{
				/*	match 되지 않으면 tree에서 제거후 pstMoveItems 으로 옮긴 뒤 후에 한꺼 번에 아래 level로 옮긴다.
				*/
				pstTmpFirstItem = svc_cas_ScenarioMakeMoveItem(pstTreeMgr, pstTmpFirstItem,
																pstDeleteItem, &pstMoveItems);
			}
			else
			{
				/*	CAM은 Cas system id가 일치 하지 않으면 아예 삭제 되도록 한다.
				*/
				pstTmpFirstItem = svc_cas_ScenarioDeletePriorityTreeItem(pstTreeMgr, pstTmpFirstItem, pstDeleteItem);
			}

			pstDeleteItem = NULL;
		}
	}

/* 옮길 아이템들을 바로 아래 tree로 붙인다. - 실제 옮기는 작업임..*/
	if(pstMoveItems != NULL)
	{
		pstTmpFirstItem = svc_cas_ScenarioMovePriorityTreeItems(pstTmpFirstItem, pstMoveItems);
	}

/* 다음 레벨로 이동.*/
	if(pstTmpFirstItem != NULL)
	{
		xsvc_cas_DefScenarioMakePTFromGCasSystemIdMatch(pstTreeMgr, pstTmpFirstItem->pstChild, ulPriorityTableId, ulActionId);
	}

	HxLOG_Info("xsvc_cas_DefScenarioMakePTFromGCasSystemIdMatch() -\n");
	return;
}

void xsvc_cas_DefScenarioMakePTFromGKindOfCas (SvcCas_MgrPriorityTreeMgr_t		*pstTreeMgr,
												SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
												HUINT32							 ulPriorityTableId,
												HUINT32							 ulActionId)
{
	HUINT32					i;
	SvcCas_GroupOfPriority_t			*pstGroupOfPriority = NULL;

	if(pstTreeMgr == NULL || pstFirstItem == NULL)
	{
		return;
	}

/* priority table를 가져오도록 한다.*/
	pstGroupOfPriority = svc_cas_MgrSubGetPriorityTable(ulPriorityTableId);
	if(pstGroupOfPriority == NULL)
	{
		return;
	}

/* priority 아이템 수 만큼 tree를 반복해서 확인한다.
*/
	for(i = 0; i < pstGroupOfPriority->ulNumOfPriority; i++)
	{
		xsvc_cas_ScenarioMakePTFromGKindOfCas(pstTreeMgr, pstFirstItem, pstGroupOfPriority->aeItemOfPriority[i]);
	}

	return;
}

void xsvc_cas_DefScenarioMakePTFromGResource (SvcCas_MgrPriorityTreeMgr_t		*pstTreeMgr,
												SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
												HUINT32							 ulPriorityTableId,
												HUINT32							 ulActionId)
{
	HUINT32							ulLargestResourceCount;
	HUINT32 						ulNumOfItem;
	HUINT32 						ulNumOfMoveItem;
	HERROR							hErr;
	SvcCas_MgrPriorityTreeItem_t	*pstTmpFirstItem = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstTmpItem = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstMoveItems = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstDeleteItem = NULL;

	if(pstTreeMgr == NULL || pstFirstItem == NULL)
	{
		HxLOG_Info("pstTreeMgr = 0x%x, pstFirstItem = 0x%x\n", pstTreeMgr, pstFirstItem);
		return;
	}
	pstTmpFirstItem = pstFirstItem;

/* 현재 tree level에 우선 순위가 낮아서 아래로 옮겨야 할 Item이 있는가?
	없으면 다음 level에서 찾도록 recursive 호출을 하도록 하자.
	옮겨야할 아이템과 개수와  현재 level의 아이템 개수가 같으면 마찬 가지로  recursive 호출
*/
	ulNumOfItem = xsvc_cas_ScenarioCountPriorityTreeItemFromSameLevel(pstTmpFirstItem);
	ulLargestResourceCount = svc_cas_MgrSubGetLargestResourceCount(pstTmpFirstItem);
	ulNumOfMoveItem = svc_cas_MgrSubGetNumOfLessItemFromLargestResourceCount(pstTmpFirstItem, ulLargestResourceCount);

	HxLOG_Info("ulNumOfItem = %d, ulNumOfMoveItem = %d\n", ulNumOfItem, ulNumOfMoveItem);
	if(ulNumOfMoveItem == 0 || ulNumOfItem == ulNumOfMoveItem)
	{
		xsvc_cas_DefScenarioMakePTFromGResource(pstTreeMgr, pstTmpFirstItem->pstChild, ulPriorityTableId, ulActionId);
		return;
	}

/* 옮겨야 할 item을 찾는다.
	아이템이 찾아 지면 삭제 후  삭제 된 것들을 모아서 아래 level로 옮긴다.
*/
	pstTmpItem = pstTmpFirstItem;
	while(pstTmpItem)
	{
		hErr = svc_cas_MgrSubIsLessThenInputArg(pstTmpItem, ulLargestResourceCount);
		if(hErr == ERR_OK)
		{
			pstDeleteItem = pstTmpItem;
		}
		pstTmpItem = pstTmpItem->pstNextSilbing;

		if(pstDeleteItem != NULL)
		{
			/*	match 되지 않으면 tree에서 제거후 pstMoveItems 으로 옮긴 뒤 후에 한꺼 번에 아래 level로 옮긴다.
			*/
			pstTmpFirstItem = svc_cas_ScenarioMakeMoveItem( pstTreeMgr,
															pstTmpFirstItem,
															pstDeleteItem,
															&pstMoveItems);
			pstDeleteItem = NULL;
		}
	}

/* 옮길 아이템들을 바로 아래 tree로 붙인다. - 실제 옮기는 작업임..*/
	if(pstMoveItems != NULL)
	{
		pstTmpFirstItem = svc_cas_ScenarioMovePriorityTreeItems(pstTmpFirstItem, pstMoveItems);
	}

/* 다음 레벨로 이동.*/
	if(pstTmpFirstItem != NULL)
	{
		xsvc_cas_DefScenarioMakePTFromGResource(pstTreeMgr, pstTmpFirstItem->pstChild, ulPriorityTableId, ulActionId);
	}

	return;
}

void xsvc_cas_DefScenarioMakePTFromGKindOfDevice (SvcCas_MgrPriorityTreeMgr_t		*pstTreeMgr,
													SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
													HUINT32							 ulPriorityTableId,
													HUINT32							 ulActionId)
{
	HUINT32 					ulNumOfItem;
	HUINT32 					ulNumOfMoveItem;
	HERROR					hErr;
	SvcCas_ItemOfPriority_e			eDeviceId;
	SvcCas_GroupOfPriority_t			*pstGroupOfPriority = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstTmpFirstItem = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstTmpItem = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstMoveItems = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstDeleteItem = NULL;

	if(pstTreeMgr == NULL || pstFirstItem == NULL)
	{
		HxLOG_Info("pstTreeMgr = 0x%x, pstFirstItem = 0x%x\n", pstTreeMgr, pstFirstItem);
		return;
	}
	pstTmpFirstItem = pstFirstItem;

/* priority table를 가져오도록 한다.*/
	pstGroupOfPriority = svc_cas_MgrSubGetPriorityTable(ulPriorityTableId);
	if(pstGroupOfPriority == NULL)
	{
		HxLOG_Error("get priority table fail\n");
		return;
	}
	eDeviceId = pstGroupOfPriority->aeItemOfPriority[1];

/* 현재 tree level에 우선 순위가 낮아서 아래로 옮겨야 할 Item이 있는가?
	없으면 다음 level에서 찾도록 recursive 호출을 하도록 하자.
	옮겨야할 아이템과 개수와  현재 level의 아이템 개수가 같으면 마찬 가지로  recursive 호출
*/
	ulNumOfItem = xsvc_cas_ScenarioCountPriorityTreeItemFromSameLevel(pstTmpFirstItem);
	ulNumOfMoveItem	= xsvc_cas_ScenarioGetNumOfDeviceMatchItem(pstTmpFirstItem, eDeviceId);

	HxLOG_Info("ulNumOfItem = %d, ulNumOfMoveItem = %d\n", ulNumOfItem, ulNumOfMoveItem);
	if(ulNumOfMoveItem == 0 || ulNumOfItem == ulNumOfMoveItem)
	{
		xsvc_cas_DefScenarioMakePTFromGKindOfDevice(pstTreeMgr, pstTmpFirstItem->pstChild, ulPriorityTableId, ulActionId);
		return;
	}

	/* 옮겨야 할 item을 찾는다.
		아이템이 찾아 지면 삭제 후 삭제 된 것들을 모아서 아래 level로 옮긴다.
	*/
	pstTmpItem = pstTmpFirstItem;
	while(pstTmpItem)
	{
		hErr = svc_cas_MgrSubIsDeviceMatchItem(pstTmpItem, eDeviceId);
		if(hErr == ERR_OK)
		{
			pstDeleteItem = pstTmpItem;
		}
		pstTmpItem = pstTmpItem->pstNextSilbing;

		if(pstDeleteItem != NULL)
		{
			/* tree에서 제거후 pstMoveItems 으로 옮긴 뒤 후에 한꺼 번에 아래 level로 옮긴다.
			*/
			pstTmpFirstItem = svc_cas_ScenarioMakeMoveItem(pstTreeMgr, pstTmpFirstItem, pstDeleteItem, &pstMoveItems);
			pstDeleteItem = NULL;
		}
	}

/* 옮길 아이템들을 바로 아래 tree로 붙인다. - 실제 옮기는 작업임..*/
	if(pstMoveItems != NULL)
	{
		pstTmpFirstItem = svc_cas_ScenarioMovePriorityTreeItems(pstTmpFirstItem, pstMoveItems);
	}

/* 다음 레벨로 이동.*/
	if(pstTmpFirstItem != NULL)
	{
		xsvc_cas_DefScenarioMakePTFromGKindOfDevice(pstTreeMgr, pstTmpFirstItem->pstChild, ulPriorityTableId, ulActionId);
	}

	return;
}

void xsvc_cas_DefScenarioMakePTFromGSlot (SvcCas_MgrPriorityTreeMgr_t		*pstTreeMgr,
											SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
											HUINT32							 ulPriorityTableId,
											HUINT32							 ulActionId)
{
	HUINT32 				i;
	SvcCas_GroupOfPriority_t		*pstGroupOfPriority = NULL;

	HxLOG_Info("++++++++++++++++++++++++++++++++\n");
	HxLOG_Info("xsvc_cas_DefScenarioMakePTFromGSlot() +\n");
	HxLOG_Info("++++++++++++++++++++++++++++++++\n");

	if(pstTreeMgr == NULL || pstFirstItem == NULL)
	{
		HxLOG_Error("pstTreeMgr = 0x%x, pstFirstItem = 0x%x", pstTreeMgr, pstFirstItem);
		return;
	}

/* priority table를 가져오도록 한다.*/
	pstGroupOfPriority = svc_cas_MgrSubGetPriorityTable(ulPriorityTableId);
	if(pstGroupOfPriority == NULL)
	{
		HxLOG_Error("get group priority fail\n");
		return;
	}

/* priority 아이템 수 만큼 tree를 반복해서 확인한다.
*/
	for(i = 0; i < pstGroupOfPriority->ulNumOfPriority; i++)
	{
		HxLOG_Info("pstGroupOfPriority->ulNumOfPriority = %d, i = %d\n", pstGroupOfPriority->ulNumOfPriority, i);

		xsvc_cas_ScenarioMakePTFromGSlot(pstTreeMgr, pstFirstItem, pstGroupOfPriority->aeItemOfPriority[i]);
	}

	HxLOG_Info("xsvc_cas_DefScenarioMakePTFromGSlot() -\n");

	return;
}

void xsvc_cas_DefScenarioMakePTFromGSlotByDB (SvcCas_MgrPriorityTreeMgr_t		*pstTreeMgr,
												SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
												HUINT32							 ulPriorityTableId,
												HUINT32							 ulReqActionId)
{
	svc_cas_ScenarioMakePTFromGSlotByDB(pstTreeMgr, pstFirstItem, ulPriorityTableId, ulReqActionId);
}

void xsvc_cas_DefScenarioMakePTFromGKindOfAction (SvcCas_MgrPriorityTreeMgr_t		*pstTreeMgr,
													SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
													HUINT32							 ulPriorityTableId,
													HUINT32							 ulReqActionId)
{
	svc_cas_ScenarioMakePTFromGKindOfAction(pstTreeMgr, pstFirstItem, ulPriorityTableId, ulReqActionId);
}

/********************************************************************
* For Debugging
********************************************************************/
#define ___08_Debugging_APIs___


/*********************** End of File ******************************/
