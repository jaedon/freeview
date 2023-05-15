/**
	@file     casmgr_control.c
	@brief    casmgr_control.c (CAS Service)

	Description:
	Module: \n
	Remarks :\n

	Copyright (c) 2012 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/********************************************************************
* Header Files
********************************************************************/
#define ___01_Header_Files___

#include <octo_common.h>

#include <svc_cas.h>

/********************************************************************
* For Debugging
********************************************************************/
#define ___02_Debugging___

/********************************************************************
* Local Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Local_Definitions___

typedef enum
{
	eCAS_Ctrl_CAS_Na = 0,
	eCAS_Ctrl_CAS_Ir,
	eCAS_Ctrl_CAS_Va,
	eCAS_Ctrl_CAS_Cx,
	eCAS_Ctrl_CAS_Bcas,
	eCAS_Ctrl_CAS_Ci,
	eCAS_Ctrl_CAS_Vx,
	eCAS_Ctrl_CAS_MAX,
} CAS_CtrlCaIdx_t;

typedef struct tagCAS_CtrlItem_t
{
	HUINT32				ulNum;
	SvcCas_Ctrl_t			*astCtrl;
} CAS_CtrlItem_t;

typedef struct tagCAS_CtrlTable_t
{
	CAS_CtrlItem_t		astCtrlItem[eCAS_Ctrl_CAS_MAX];
} CAS_CtrlTable_t;

/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___04_Extern_Vars_Func_Prototypes___

/********************************************************************
* Static Variables / Function Prototypes
********************************************************************/
#define ___05_Local_Vars_Func_Prototypes___

STATIC CAS_CtrlTable_t	s_astCasCtrl_Table;

/********************************************************************
* Static Functions
* Naming rule : "local"_"CasMgrMain" _ "Function Name"
********************************************************************/
#define ___06_Local_APIs___

static CAS_CtrlCaIdx_t svc_cas_ctrl_CaGroupIdToCtrlIdx(DxCAS_GroupId_e eCasId)
{
	switch (eCasId)
	{
	case eDxCAS_GROUPID_NA:			return eCAS_Ctrl_CAS_Na;
	case eDxCAS_GROUPID_IR:			return eCAS_Ctrl_CAS_Ir;
	case eDxCAS_GROUPID_VA:			return eCAS_Ctrl_CAS_Va;
	case eDxCAS_GROUPID_CX:			return eCAS_Ctrl_CAS_Cx;
	case eDxCAS_GROUPID_CAM:		return eCAS_Ctrl_CAS_Ci;
	case eDxCAS_GROUPID_VX:			return eCAS_Ctrl_CAS_Vx;
	default:					break;
	}

	return eCAS_Ctrl_CAS_MAX;
}


static CAS_CtrlItem_t *svc_cas_ctrl_getCtrlItem(DxCAS_GroupId_e eCasId)
{
	CAS_CtrlCaIdx_t	eCasIdx;

	eCasIdx = svc_cas_ctrl_CaGroupIdToCtrlIdx(eCasId);
	if (eCasIdx >= eCAS_Ctrl_CAS_MAX)
	{
		return NULL;
	}

	return &s_astCasCtrl_Table.astCtrlItem[eCasIdx];
}

STATIC HERROR svc_cas_ctrl_initCtrl(CAS_CtrlCaIdx_t eCasIdx, HBOOL bFree)
{
	HxLOG_Info("%s (+)\n", __FUNCTION__);
	if(eCasIdx >= eCAS_Ctrl_CAS_MAX)
	{
		HxLOG_Assert(0);
		return ERR_FAIL;
	}
	if (bFree)
	{
		if (s_astCasCtrl_Table.astCtrlItem[eCasIdx].astCtrl != NULL)
		{
			OxMW_Free(s_astCasCtrl_Table.astCtrlItem[eCasIdx].astCtrl);
		}
	}

	s_astCasCtrl_Table.astCtrlItem[eCasIdx].astCtrl = NULL;
	s_astCasCtrl_Table.astCtrlItem[eCasIdx].ulNum = 0;
	HxSTD_memset(&s_astCasCtrl_Table.astCtrlItem[eCasIdx], 0, sizeof(CAS_CtrlItem_t));

	HxLOG_Info("%s (-)\n", __FUNCTION__);
	return ERR_OK;
}

static SvcCas_Ctrl_t *svc_cas_ctrl_getCtrl(DxCAS_GroupId_e eCasId, HUINT32 ulControlType)
{
	CAS_CtrlItem_t	*pstCtrlItem;

	pstCtrlItem = svc_cas_ctrl_getCtrlItem(eCasId);
	if (pstCtrlItem == NULL)
	{
		return NULL;
	}

	if (pstCtrlItem->ulNum <= ulControlType)
	{
		return NULL;
	}


	return &pstCtrlItem->astCtrl[ulControlType];
}

STATIC HINT32 svc_cas_ctrl_CompareCotrolType(const void *pvArg1, const void *pvArg2)
{
	SvcCas_Ctrl_t *pst1 = (SvcCas_Ctrl_t *)pvArg1;
	SvcCas_Ctrl_t *pst2 = (SvcCas_Ctrl_t *)pvArg2;

	return pst1->ulControlType - pst2->ulControlType;
}

static HERROR svc_cas_ctrl_setCtrlTable(DxCAS_GroupId_e eCasId, HUINT32 ulNum, SvcCas_Ctrl_t *pstCasCtrlList)
{
	HERROR				hErr;
	CAS_CtrlCaIdx_t	eCasIdx;

	HxLOG_Info("%s (+)\n", __FUNCTION__);

	eCasIdx = svc_cas_ctrl_CaGroupIdToCtrlIdx(eCasId);
	if (eCasIdx >= eCAS_Ctrl_CAS_MAX)
	{
		return ERR_FAIL;
	}

	hErr = svc_cas_ctrl_initCtrl(eCasIdx, TRUE);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	s_astCasCtrl_Table.astCtrlItem[eCasIdx].astCtrl = (SvcCas_Ctrl_t *)OxMW_Malloc(sizeof(SvcCas_Ctrl_t)*ulNum);
	if (s_astCasCtrl_Table.astCtrlItem[eCasIdx].astCtrl == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_memcpy(s_astCasCtrl_Table.astCtrlItem[eCasIdx].astCtrl, pstCasCtrlList, sizeof(SvcCas_Ctrl_t)*ulNum);

#if 1
	{
		HUINT32 i;
		SvcCas_Ctrl_t *pstCtrl;

		HxLOG_Print("#############befor sorting#################\n");
		for(i = 0; i < ulNum; i++)
		{
			pstCtrl = &s_astCasCtrl_Table.astCtrlItem[eCasIdx].astCtrl[i];
			HxLOG_Print("idx[%d], ctrl[%d], Flag[0x%x]\n", i, pstCtrl->ulControlType, pstCtrl->eCtrlFlag);
		}
	}
#endif
	qsort(s_astCasCtrl_Table.astCtrlItem[eCasIdx].astCtrl, ulNum, sizeof(SvcCas_Ctrl_t), svc_cas_ctrl_CompareCotrolType);
#if 1
	{
		HUINT32 i;
		SvcCas_Ctrl_t *pstCtrl;

		HxLOG_Print("#############after sorting#################\n");
		for(i = 0; i < ulNum; i++)
		{
			pstCtrl = &s_astCasCtrl_Table.astCtrlItem[eCasIdx].astCtrl[i];
			HxLOG_Print("idx[%d], ctrl[%d], Flag[0x%x]\n", i, pstCtrl->ulControlType, pstCtrl->eCtrlFlag);
		}
	}
#endif

	s_astCasCtrl_Table.astCtrlItem[eCasIdx].ulNum = ulNum;

	HxLOG_Info("%s (-)\n", __FUNCTION__);

	return ERR_OK;
}

/********************************************************************
* Module Internal Functions
* Naming rule : "CasMgrMain" _ "Function Name"
********************************************************************/
#define ___07_Module_Internal_APIs___

HERROR svc_cas_CtrlInit(DxCAS_GroupId_e eCasId)
{
	HERROR				hErr;
	CAS_CtrlCaIdx_t	eCasIdx;

	HxLOG_Info("%s (+)\n", __FUNCTION__);
	eCasIdx = svc_cas_ctrl_CaGroupIdToCtrlIdx(eCasId);
	if (eCasIdx >= eCAS_Ctrl_CAS_MAX)
	{
		return ERR_FAIL;
	}

	hErr = svc_cas_ctrl_initCtrl(eCasIdx, FALSE);

	HxLOG_Info("%s (-)\n", __FUNCTION__);
	return hErr;
}

HERROR svc_cas_CtrlRegister(DxCAS_GroupId_e eCasId, HUINT32 ulNum, SvcCas_Ctrl_t *pstCasCtrlList)
{
	HERROR		hErr;

	HxLOG_Info("%s (+)\n", __FUNCTION__);

	if (pstCasCtrlList == NULL)
	{
		return ERR_FAIL;
	}

	if (ulNum == 0)
	{
		return ERR_FAIL;
	}

	hErr = svc_cas_ctrl_setCtrlTable(eCasId, ulNum, pstCasCtrlList);

	HxLOG_Info("%s (-)\n", __FUNCTION__);

	return hErr;
}

HERROR svc_cas_CtrlCall(DxCAS_GroupId_e eCasId, HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	SvcCas_Ctrl_t 		*pstCasCtrl;

	HxLOG_Info("%s (+)\n", __FUNCTION__);

	pstCasCtrl = svc_cas_ctrl_getCtrl(eCasId, ulControlType);

	if (pstCasCtrl == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (pstCasCtrl->pfnCtrlFunc == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if ((pstCasCtrl->eCtrlFlag & eCasCtrl_Control) == 0)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (pstCasCtrl->ulControlType != ulControlType)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	hErr = (* pstCasCtrl->pfnCtrlFunc)(ulControlType, pstParam);

	HxLOG_Info("%s (-)\n", __FUNCTION__);

	return hErr;
}

HERROR svc_cas_CtrlGet(DxCAS_GroupId_e eCasId, HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	SvcCas_Ctrl_t 		*pstCasCtrl;

	HxLOG_Info("%s (+)\n", __FUNCTION__);

	pstCasCtrl = svc_cas_ctrl_getCtrl(eCasId, ulControlType);

	if (pstCasCtrl == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (pstCasCtrl->pfnGetFunc == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if ((pstCasCtrl->eCtrlFlag & eCasCtrl_Get) == 0)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (pstCasCtrl->ulControlType != ulControlType)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	hErr = (* pstCasCtrl->pfnGetFunc)(ulControlType, pstParam);

	HxLOG_Info("%s (-)\n", __FUNCTION__);

	return hErr;
}

HERROR svc_cas_CtrlSet(DxCAS_GroupId_e eCasId, HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	SvcCas_Ctrl_t 		*pstCasCtrl;

	HxLOG_Info("%s (+)\n", __FUNCTION__);

	pstCasCtrl = svc_cas_ctrl_getCtrl(eCasId, ulControlType);

	if (pstCasCtrl == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (pstCasCtrl->pfnSetFunc == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if ((pstCasCtrl->eCtrlFlag & eCasCtrl_Set) == 0)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (pstCasCtrl->ulControlType != ulControlType)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	hErr = (* pstCasCtrl->pfnSetFunc)(ulControlType, pstParam);

	HxLOG_Info("%s (-)\n", __FUNCTION__);

	return hErr;
}

HERROR svc_cas_CtrlReq(DxCAS_GroupId_e eCasId, HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	SvcCas_Ctrl_t 		*pstCasCtrl;

	HxLOG_Info("%s (+)\n", __FUNCTION__);

	pstCasCtrl = svc_cas_ctrl_getCtrl(eCasId, ulControlType);

	if (pstCasCtrl == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (pstCasCtrl->pfnReqFunc == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if ((pstCasCtrl->eCtrlFlag & eCasCtrl_Req) == 0)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (pstCasCtrl->ulControlType != ulControlType)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	hErr = (* pstCasCtrl->pfnReqFunc)(ulControlType, pstParam);

	HxLOG_Info("%s (-)\n", __FUNCTION__);

	return hErr;
}

HERROR svc_cas_CtrlNoti(DxCAS_GroupId_e eCasId, HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	SvcCas_Ctrl_t 		*pstCasCtrl;

	HxLOG_Info("%s (+)\n", __FUNCTION__);

	pstCasCtrl = svc_cas_ctrl_getCtrl(eCasId, ulControlType);

	if (pstCasCtrl == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (pstCasCtrl->pfnNotiFunc == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if ((pstCasCtrl->eCtrlFlag & eCasCtrl_Noti) == 0)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (pstCasCtrl->ulControlType != ulControlType)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	hErr = (* pstCasCtrl->pfnNotiFunc)(ulControlType, pstParam);

	HxLOG_Info("%s (-)\n", __FUNCTION__);

	return hErr;
}

/********************************************************************
* For Debugging
********************************************************************/
#define ___09_Debugging_APIs___

