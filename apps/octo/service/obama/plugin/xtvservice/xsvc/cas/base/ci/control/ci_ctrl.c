/**************************************************************************************************/
#define ________Ci_ctrl_Private_Include__________________________________________________
/**************************************************************************************************/
#include <octo_common.h>

#include <svc_cas.h>
#include <_svc_cas_mgr_control.h>

#include <ci_mgr_int.h>
#include <ci_ctrl.h>
#include <ci.h>

#ifdef CONFIG_MW_CI_PLUS
#include <ci_plus.h>
#endif

/**************************************************************************************************/
#define ________Ci_ctrl_Golbal_Value____________________________________________________
/**************************************************************************************************/




/**************************************************************************************************/
#define ________Ci_ctrl_Private_Define___________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define ________Ci_ctrl_Private_Type_Define_____________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define ________Ci_ctrl_Private_Static_Value______________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________Ci_ctrl_Private_Static_Prototype__________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________Ci_ctrl_Private_Static_Prototype_Body_____________________________________
/**************************************************************************************************/
static HERROR local_CI_MmiOpen(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_MmiClose(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_MmiSelect(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_MmiAnswer(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_MmiGetData(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_GetSlotIdByActionId(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);

#ifdef CONFIG_MW_CI_PLUS
static HERROR local_CI_GetHostId(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_GetBindedCamId(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_GetCcReport(HUINT32 ulControlType, SvcCas_CtrlParam_t *pData);
static HERROR local_CI_InitAuthContexts(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
//static HERROR local_CI_DownloadCiPlusData(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_RequestFileOrData(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_RequsetAppAbort(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#if 0 //not define
static HERROR local_CI_AddNotifyFuncForDownloading(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
static HERROR local_CI_ChangeHostCountrySetting(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_ChangeHostLanguageSetting(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#if defined(CONFIG_MW_CI_PLUS)
static HERROR local_CI_NotifyDrmInfoConsumed(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_GetUpgradeStatus(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
#if defined(CONFIG_OP_CANALREADY)
static HERROR local_CI_CanalReadyGetInitInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_CanalReadyMoralityLevelUnlock(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif	// #if defined(CONFIG_OP_CANALREADY)

#if defined(CONFIG_MW_CI_PLUS_HCI_V_2)
static HERROR local_CI_NotifyTuneStatus(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_NotifyHciPmtUse(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif	// #if defined(CONFIG_MW_CI_PLUS_HCI_V_2)

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
static HERROR local_CI_SetParentalCtrl(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_SetLicense(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_SetPinCode(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_GetPinCapa(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif	// #if defined(CONFIG_MW_CI_PLUS_CC_V_2)

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
static HERROR local_CI_OperatorPrepareStatus(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_OperatorTuneStatus(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_GetOperatorNetNum(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_GetOperatorNetInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_GetOperatorTsNum(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_GetOperatorTsInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_GetOperatorSvcNum(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_GetOperatorSvcInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_GetOperatorName(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_GetOperatorReservedTime(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_GetOperatorSupportCis(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_CI_GetOperatorErrStatus(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
#endif	// #ifdef CONFIG_MW_CI_PLUS


/**************************************************************************************************/
#define ________Ci_ctrl_External_Functions_Body__________________________________________
/**************************************************************************************************/

static const SvcCas_Ctrl_t s_astCiCtrlTable[eCACTRL_CI_MAX] =
{
	{
		eCACTRL_CI_MMI_Open,
		eCasCtrl_Control,
		local_CI_MmiOpen,	//Control
		NULL,	//Get
		NULL,	//Set
		NULL,	//Req
		NULL,	//Noti
	},

	{
		eCACTRL_CI_MMI_Close,
		eCasCtrl_Control,
		local_CI_MmiClose,	//Control
		NULL,	//Get
		NULL,	//Set
		NULL,	//Req
		NULL,	//Noti
	},

	{
		eCACTRL_CI_MMI_Select,
		eCasCtrl_Control,
		local_CI_MmiSelect,	//Control
		NULL,	//Get
		NULL,	//Set
		NULL,	//Req
		NULL,	//Noti
	},

	{
		eCACTRL_CI_MMI_Answer,
		eCasCtrl_Control,
		local_CI_MmiAnswer,	//Control
		NULL,	//Get
		NULL,	//Set
		NULL,	//Req
		NULL,	//Noti
	},

	{
		eCACTRL_CI_MMI_GetData,
		eCasCtrl_Get,
		NULL,	//Control
		local_CI_MmiGetData,	//Get
		NULL,	//Set
		NULL,	//Req
		NULL,	//Noti
	},

	{
		eCACTRL_CI_GetSlotIdByActionId,
		eCasCtrl_Get,
		NULL,	// Control
		local_CI_GetSlotIdByActionId,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
#if defined(CONFIG_MW_CI_PLUS)
	{
		eCACTRL_CI_GetHostId,
		eCasCtrl_Get,
		NULL,	// Control
		local_CI_GetHostId,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_CI_GetBindedCamId,
		eCasCtrl_Get,
		NULL,	// Control
		local_CI_GetBindedCamId,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_CI_GetCcReport,
		eCasCtrl_Get,
		NULL,	// Control
		local_CI_GetCcReport,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_CI_InitAuthContexts,
		eCasCtrl_Control,
		local_CI_InitAuthContexts,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_CI_ReqFileOrData,
		eCasCtrl_Req,
		NULL,	// Control
		NULL,	// Get
		NULL,	// Set
		local_CI_RequestFileOrData,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_CI_ReqAppAbort,
		eCasCtrl_Req,
		NULL,	// Control
		NULL,	// Get
		NULL,	// Set
		local_CI_RequsetAppAbort, // Req
		NULL,	// Noti
	},

	#if 0
	{
		eCACTRL_CI_DownloadCiPlusData,
		eCasCtrl_Control,
		local_CI_DownloadCiPlusData,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},


	{
		eCACTRL_CI_AddNotifyFuncForDownloading,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		local_CI_AddNotifyFuncForDownloading,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
	#endif

	{
		eCACTRL_CI_ChangeHostCountrySetting,
		eCasCtrl_Control,
		local_CI_ChangeHostCountrySetting,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_CI_ChangeHostLanguageSetting,
		eCasCtrl_Control,
		local_CI_ChangeHostLanguageSetting,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	#if defined(CONFIG_MW_CI_PLUS)
	{
		eCACTRL_CI_NotifyDrmInfoConsumed,
		eCasCtrl_Noti,
		NULL,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		local_CI_NotifyDrmInfoConsumed,	// Noti
	},

	{
		eCACTRL_CI_GetUpgradeStatus,
		eCasCtrl_Get,
		NULL,	// Control
		local_CI_GetUpgradeStatus,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
	#endif

	#if defined(CONFIG_OP_CANALREADY)
	{
		eCACTRL_CI_CanalReadyGetInitInfo,
		eCasCtrl_Get,
		NULL,	// Control
		local_CI_CanalReadyGetInitInfo,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_CI_CanalReadyMoralityLevelUnlock,
		eCasCtrl_Control,
		local_CI_CanalReadyMoralityLevelUnlock,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
	#endif

	#if defined(CONFIG_MW_CI_PLUS_HCI_V_2)
	{
		eCACTRL_CI_NotifyTuneStatus,
		eCasCtrl_Noti,
		NULL,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		local_CI_NotifyTuneStatus,	// Noti
	},

	{
		eCACTRL_CI_NotifyHciPmtUse,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		local_CI_NotifyHciPmtUse,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
	#endif

	#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
	{
		eCACTRL_CI_SetParentalCtrl,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		local_CI_SetParentalCtrl,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_CI_SetLicense,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		local_CI_SetLicense,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_CI_SetPinCode,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		local_CI_SetPinCode,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_CI_GetPinCapa,
		eCasCtrl_Get,
		NULL,	// Control
		local_CI_GetPinCapa,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
	#endif

	#if defined(CONFIG_MW_CI_PLUS_V_1_3)
	{
		eCACTRL_CI_OperatorPrepareStatus,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		local_CI_OperatorPrepareStatus,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_CI_SetOperatorTuneStatus,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		local_CI_OperatorTuneStatus,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_CI_GetOpNetNum,
		eCasCtrl_Get,
		NULL,	// Control
		local_CI_GetOperatorNetNum,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_CI_GetOpNetInfo,
		eCasCtrl_Get,
		NULL,	// Control
		local_CI_GetOperatorNetInfo,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_CI_GetOpTsNum,
		eCasCtrl_Get,
		NULL,	// Control
		local_CI_GetOperatorTsNum,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_CI_GetOpTsInfo,
		eCasCtrl_Get,
		NULL,	// Control
		local_CI_GetOperatorTsInfo,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_CI_GetOpSvcNum,
		eCasCtrl_Get,
		NULL,	// Control
		local_CI_GetOperatorSvcNum,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_CI_GetOpSvcInfo,
		eCasCtrl_Get,
		NULL,	// Control
		local_CI_GetOperatorSvcInfo,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_CI_GetOpName,
		eCasCtrl_Get,
		NULL,	// Control
		local_CI_GetOperatorName,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_CI_GetOpRsvTime,
		eCasCtrl_Get,
		NULL,	// Control
		local_CI_GetOperatorReservedTime,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
	#endif

	{
		eCACTRL_CI_GetOpSupportCis,
		eCasCtrl_Get,
		NULL,	// Control
		local_CI_GetOperatorSupportCis,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_CI_GetOpErrStatus,
		eCasCtrl_Get,
		NULL,	// Control
		local_CI_GetOperatorErrStatus,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	#if 0
	{
		eCACTRL_CI_MAX,
		eCasCtrl_None,
		NULL,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
	#endif

#endif

};

HERROR CI_Ctrl_RegisterCtrlList(void)
{
	HERROR	hErr;

	hErr = svc_cas_MgrSubInitControl(eDxCAS_GROUPID_CAM);
	if (hErr == ERR_OK)
	{
		hErr = svc_cas_MgrSubRegisterControl(eDxCAS_GROUPID_CAM, eCACTRL_CI_MAX, (SvcCas_Ctrl_t *)&s_astCiCtrlTable[0]);
	}

	return hErr;
}

//inline void *local_getInParam(SvcCas_CtrlParam_t *pstParam)
inline Ci_InParam_t *local_getInParam(SvcCas_CtrlParam_t *pstParam)
{
#if 0
	return (Ci_InParam_t *)pstParam->unArg.pstCi->putIn;
#else
	return (Ci_InParam_t *)pstParam->pvIn;
#endif
}

//inline void *local_getOutParam(SvcCas_CtrlParam_t *pstParam)
inline Ci_OutParam_t *local_getOutParam(SvcCas_CtrlParam_t *pstParam)
{
#if 0
	return (Ci_OutParam_t *)pstParam->unArg.pstCi->putOut;
#else
	return (Ci_OutParam_t *)pstParam->pvOut;
#endif
}

#if 0
static HERROR local_XXX(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	XXXInParam_t	*pstInParam;
	XXXOutParam_t	*pstOutParam;

	if (ulControlType >= eCACTRL_CI_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (XXXInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (XXXOutParam_t *)local_getInParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

#if 0
	hErr = MW_CA_XXX(pstInParam-> , pstOutParam->);
#endif

	return hErr;
}
#endif

static HERROR local_CI_MmiOpen(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_MmiOpenInParam_t		*pstInParam;

	if (ulControlType >= eCACTRL_CI_MAX)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pstInParam = (CI_MmiOpenInParam_t *)local_getInParam(pstParam);

	if(CI_EnterMenu(pstInParam->usSlotId))
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR local_CI_MmiClose(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_MmiCloseInParam_t 	*pstInParam;

	if (ulControlType >= eCACTRL_CI_MAX)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pstInParam = (CI_MmiCloseInParam_t *)local_getInParam(pstParam);

	if (CI_MmiClose(pstInParam->hMmi))
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR local_CI_MmiAnswer(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HUINT16					usSlotId = 0xffff;
	HUINT16					usSsId = 0;
	HINT32					nResult;
	CI_MmiAnswerInParam_t 	*pstInParam;

	if (ulControlType >= eCACTRL_CI_MAX)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pstInParam = (CI_MmiAnswerInParam_t *)local_getInParam(pstParam);

	nResult = CI_MmiGetSlotId(pstInParam->hMmi, &usSlotId, &usSsId);
	if(nResult != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_OK;
	}

	if (pstInParam->usStringLength > MAX_CI_STR_LEN) // answer가 이렇게 되는 경우는 없겠지만 일단 안전을 위해 아래와 같이 해둔다. 캠에 전달될 데이터라 길이를 자르는게 좋지는 않지만...
	{
		HxLOG_Error("usStringLength %d > %d !!! \n", pstInParam->usStringLength, MAX_CI_STR_LEN);
		pstInParam->usStringLength = MAX_CI_STR_LEN;
	}

	if (CI_MmiAnswerString(pstInParam->hMmi, pstInParam->pucString, pstInParam->usStringLength))
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR local_CI_MmiSelect(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_MmiSelectInParam_t 	*pstInParam;

	if (ulControlType >= eCACTRL_CI_MAX)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pstInParam = (CI_MmiSelectInParam_t *)local_getInParam(pstParam);

	if (CI_MmiAnswer(pstInParam->hMmi, 0, pstInParam->ucItemNumber))
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR local_CI_MmiGetData(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR					hErr;
	SvcCas_MmiData_t			*pData = NULL;
	CI_GetDataInParam_t 		*pstInParam;
	CI_GetDataOutParam_t 	*pstOutParam;

	if (ulControlType >= eCACTRL_CI_MAX)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pstInParam = (CI_GetDataInParam_t *)local_getInParam(pstParam);
	pstOutParam = (CI_GetDataOutParam_t *)local_getOutParam(pstParam);

	hErr = CiMgr_GetMmiData(pstInParam->usSlotId, pstInParam->ulDataVer, &pData);
	if(hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	*pstOutParam->ppMmiData = pData;

	return ERR_OK;
}

static HERROR local_CI_GetSlotIdByActionId(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_GetSlotIdByActionIdInParam_t		*pstInParam;
	CI_GetSlotIdByActionIdOutParam_t	*pstOutParam;

	if (ulControlType >= eCACTRL_CI_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_GetSlotIdByActionIdInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (CI_GetSlotIdByActionIdOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	if (CiMgr_GetSlotIdByActionId(pstInParam->ulActionId, &pstOutParam->usSlotId) != ERR_OK)
		return ERR_FAIL;
	else
		return ERR_OK;
}

#ifdef CONFIG_MW_CI_PLUS
//eCACTRL_CI_GetHostId
static HERROR local_CI_GetHostId(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_GetHostIdOutParam_t	*pstOutParam;

	if (ulControlType >= eCACTRL_CI_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (CI_GetHostIdOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	if (CI_GetHostId(pstOutParam->pucHost))
		return ERR_FAIL;
	else
		return ERR_OK;
}

//eCACTRL_CI_GetBindedCamId
static HERROR local_CI_GetBindedCamId(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_GetBindedCamIdInParam_t	*pstInParam;
	CI_GetBindedCamIdOutParam_t	*pstOutParam;

	if (ulControlType >= eCACTRL_CI_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_GetBindedCamIdInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (CI_GetBindedCamIdOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	if (CI_GetBindedCamId((unsigned short)pstInParam->usSlotId, pstOutParam->pucBindedCamId))
		return ERR_FAIL;
	else
		return ERR_OK;
}

//eCACTRL_CI_GetBindedCamId
static HERROR local_CI_GetCcReport(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR						hErr;
	CI_GetCcReportOutParam_t		*pstOutParam;

	if (ulControlType >= eCACTRL_CI_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (CI_GetCcReportOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = CiMgr_GetCcReport(pstOutParam->usSlotId, pstOutParam->ulEvtVersion, &pstOutParam->pData);
	if(hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

//eCACTRL_CI_InitAuthContexts
static HERROR local_CI_InitAuthContexts(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_InitAuthContextsInParam_t	*pstInParam;

	if (ulControlType >= eCACTRL_CI_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_InitAuthContextsInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

#if defined(CONFIG_CI_PLUS_AUTH_STORE_VER2)
	if (CI_InitAuthContexts())
#else
	if (CI_InitAuthContexts(pstInParam->usSlotId))
#endif
		return ERR_FAIL;
	else
		return ERR_OK;
}

static HERROR local_CI_RequestFileOrData(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR						hErr;
	CI_RequestFileOrDataInParam_t	*pstInParam;

	if (ulControlType >= eCACTRL_CI_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_RequestFileOrDataInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = CI_RequestFileorData(pstInParam->usSessionId, (CI_AMMI_REQUEST_TYPE)pstInParam->enReqType, pstInParam->pucReqName, pstInParam->ulReqNameLen);
	if(hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR local_CI_RequsetAppAbort(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR						hErr;
	CI_RequestAppAbort_t			*pstInParam;

	if (ulControlType >= eCACTRL_CI_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_RequestAppAbort_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = CI_RequestAppAbort(pstInParam->usSessionId, (CI_AMMI_ABORT_REQUEST_CODE)pstInParam->enAbortReqCode);
	if(hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

#if 0
//eCACTRL_CI_DownloadCiPlusData
static HERROR local_CI_DownloadCiPlusData(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_DownloadCiPlusDataInParam_t	*pstInParam;

	if (ulControlType >= eCACTRL_CI_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_DownloadCiPlusDataInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	if (CI_DownloadHostLicenseConstants(pstInParam->ucValue))
		return ERR_FAIL;
	else
		return ERR_OK;
}

//eCACTRL_CI_AddNotifyFuncForDownloading
static HERROR local_CI_AddNotifyFuncForDownloading(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_AddNotifyFuncForDownloadingInParam_t	*pstInParam;

	if (ulControlType >= eCACTRL_CI_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_AddNotifyFuncForDownloadingInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	return CI_AddNotifyFuncForDownloading(pstInParam->fnFacNotify);

}
#endif

/*
	CI Plus의 HLC Session이 Open 상태면, 변경 시 마다 Notify 해주어야 한다.
	변경 값에 대해서는 전달하지 않고, Timing 만 알려주면 된다.
*/
static HERROR local_CI_ChangeHostCountrySetting(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	if (ulControlType >= eCACTRL_CI_MAX)
	{
		return ERR_FAIL;
	}

	(void)(pstParam);

	/* 가능한 모든 Slot에 */
	CI_NotifyHostCountrySettingChangedToCam(0);
	CI_NotifyHostCountrySettingChangedToCam(1);
	return ERR_OK;
}

static HERROR local_CI_ChangeHostLanguageSetting(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	if (ulControlType >= eCACTRL_CI_MAX)
	{
		return ERR_FAIL;
	}
	(void)(pstParam);

	CI_NotifyHostLanguageSettingChangedToCam(0);
	CI_NotifyHostLanguageSettingChangedToCam(1);
	return ERR_OK;
}

#if defined(CONFIG_MW_CI_PLUS)
static HERROR local_CI_NotifyDrmInfoConsumed(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_NotifyDrmInfoConsumedInParam_t	*pstInParam;

	HxLOG_Info("%s \n", __FUNCTION__);

	if (ulControlType >= eCACTRL_CI_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_NotifyDrmInfoConsumedInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	CiMgr_ReleaseDrmInfo(pstInParam->ucDrmInfoVer, pstInParam->hConsumedStatus);

	return ERR_OK;
}

static HERROR local_CI_GetUpgradeStatus(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	SvcCas_CiPlusUpgrade_t	**ppstUpgrade;
	HUINT16				usSlotId;
//	void				*pvInParam, *pvOutParam;

	if (ulControlType >= eCACTRL_CI_MAX || pstParam == NULL)
	{
		return ERR_FAIL;
	}

	if(pstParam->pvIn == NULL || pstParam->pvOut == NULL)
	{
		return ERR_FAIL;
	}

	usSlotId = *((HUINT16 *)pstParam->pvIn);
	ppstUpgrade = (SvcCas_CiPlusUpgrade_t **)pstParam->pvOut;

	return CiMgr_GetUpgradeStatus(usSlotId, ppstUpgrade);
}
#endif

#if defined(CONFIG_OP_CANALREADY)
static HERROR local_CI_CanalReadyGetInitInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_CanalReadyGetInitInfoInParam_t	*pstInParam;
	CI_CanalReadyGetInitInfoOutParam_t	*pstOutParam;
#ifdef WIN32
	static HBOOL			s_tmpTestFlag = FALSE;
#endif

	if (ulControlType >= eCACTRL_CI_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_CanalReadyGetInitInfoInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (CI_CanalReadyGetInitInfoOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}


#ifdef WIN32
	if(s_tmpTestFlag == TRUE)
	{
		pstOutParam->pstCiAuthInfo->usCaSysId = 0x500;
		pstOutParam->pstCiAuthInfo->bSavedFlag = TRUE;
		pstOutParam->pstCiAuthInfo->usNumOfCaOpId = 1;
		pstOutParam->pstCiAuthInfo->ulCaOpId[0] = 0x030b00;
	}
	else
	{
		HxSTD_memset(pstOutParam->pstCiAuthInfo, 0x00, sizeof(SvcCas_CiAuthInfo_t));
	}

	return ERR_OK;
#else
	if (CI_CanalReadyGetInitInfo(pstInParam->usSlotId, pstOutParam->pstCiAuthInfo))
	{
		return ERR_FAIL;
	}
	else
	{
		return ERR_OK;
	}
#endif
}

static HERROR local_CI_CanalReadyMoralityLevelUnlock(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_CanalReadyMoralityLevelUnlockInParam_t	*pstInParam;

	if (ulControlType >= eCACTRL_CI_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_CanalReadyMoralityLevelUnlockInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	if (CI_CanalReadyMoralityLevelUnlock(pstInParam->usSlotId))
		return ERR_FAIL;
	else
		return ERR_OK;
}

#endif	// #if defined(CONFIG_OP_CANALREADY)

#if defined(CONFIG_MW_CI_PLUS_HCI_V_2)
static HERROR local_CI_NotifyTuneStatus(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_NotifyTuneStatusInParam_t	*pstInParam;

	HxLOG_Info("%s \n", __FUNCTION__);

	if (ulControlType >= eCACTRL_CI_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_NotifyTuneStatusInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	if (CI_TuneReply(pstInParam->usSlotId, pstInParam->ulTargetResource, pstInParam->ucTuneResult))
		return ERR_FAIL;
	else
		return ERR_OK;

}

static HERROR local_CI_NotifyHciPmtUse(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_NotifyHciPmtUseInParam_t 	*pstInParam;

	HxLOG_Info("%s \n", __FUNCTION__);

	if (ulControlType >= eCACTRL_CI_MAX || pstParam == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pstInParam = (CI_NotifyHciPmtUseInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return CiMgr_NotifyHciPmtUse(pstInParam->hAction, pstInParam->usSlotId);
}

#endif	// if defined(CONFIG_MW_CI_PLUS_HCI_V_2)

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
static HERROR local_CI_SetParentalCtrl(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_SetParentalCtrlInParam_t	*pstInParam;

	HxLOG_Info("%s \n", __FUNCTION__);

	if (ulControlType >= eCACTRL_CI_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_SetParentalCtrlInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	if (CI_PinPlayback(pstInParam->usSlotId, pstInParam->unMetaIndex, pstInParam->hPbAction, pstInParam->ucAgeLimit, pstInParam->pucPrivateData))
		return ERR_FAIL;
	else
		return ERR_OK;

}

static HERROR local_CI_SetLicense(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_SetLicenseInParam_t	*pstInParam;

	HxLOG_Info("%s \n", __FUNCTION__);

	if (ulControlType >= eCACTRL_CI_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_SetLicenseInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	if (CI_SendHostLicense(pstInParam->usSlotId, pstInParam->unMetaIndex, pstInParam->hPbAction, pstInParam->usProgramNum, pstInParam->pucLicenseData, pstInParam->usLicenseLength))
		return ERR_FAIL;
	else
		return ERR_OK;

}

static HERROR local_CI_SetPinCode(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_SetPinByUserInParam_t	*pstInParam;
	UNIXTIME					stCurTime = 0;

	HxLOG_Info("%s \n", __FUNCTION__);

	if (ulControlType >= eCACTRL_CI_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_SetPinByUserInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	if(VK_CLOCK_GetTime((HULONG*)&stCurTime) != VK_OK)
	{
		HxLOG_Error(": Time was not set \n");
	}

	if (CI_StoreCicamPinInfo(pstInParam->usSlotId, pstInParam->pucPinData, stCurTime))
		return ERR_FAIL;
	else
		return ERR_OK;
}

static HERROR local_CI_GetPinCapa(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_GetPinCapabilityInParam_t	*pstInParam;
	CI_GetPinCapabilityOutParam_t	*pstOutParam;
	SvcCas_CiPlusPinCapaInfo_t		stPinCapaInfo;

	HxLOG_Info("%s \n", __FUNCTION__);

	if (ulControlType >= eCACTRL_CI_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_GetPinCapabilityInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (CI_GetPinCapabilityOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	if(CI_GetPinCapaInfo(pstInParam->usSlotId, (CI_PIN_CAPA_INFO *)&stPinCapaInfo))
	{
		HxLOG_Error("ERROR: CI_GetPinCapaInfo()...\n");
		return ERR_FAIL;
	}
	else
	{
		HxLOG_Info("PIN Capability: [%02x]\n", (HUINT8)stPinCapaInfo.enPinCapa);
		pstOutParam->stPinCapaInfo = stPinCapaInfo;
		return ERR_OK;
	}
}
#endif	// if defined(CONFIG_MW_CI_PLUS_CC_V_2)

static HERROR local_CI_OperatorPrepareStatus(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_OperatorPrepareStatus_t	*pstStatusParam;

	HxLOG_Info("%s \n", __FUNCTION__);

	if ( (ulControlType >= eCACTRL_CI_MAX) || (pstParam == NULL) )
	{
		return ERR_FAIL;
	}

	pstStatusParam = (CI_OperatorPrepareStatus_t *)local_getInParam(pstParam);
	if (pstStatusParam == NULL)
	{
		HxLOG_Error("pstStatusParam NULL!!!\n");
		return ERR_FAIL;
	}

	if (CI_OperatorPrepareStatus(pstStatusParam->usSlotId, pstStatusParam->bSearchAvailable))
		return ERR_FAIL;
	else
		return ERR_OK;
}

static HERROR local_CI_OperatorTuneStatus(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_SetOperatorTuneStatus_t	*pstTuneStatus;

	HxLOG_Info("%s \n", __FUNCTION__);

	if ( (ulControlType >= eCACTRL_CI_MAX) || (pstParam == NULL) )
	{
		return ERR_FAIL;
	}

	pstTuneStatus = (CI_SetOperatorTuneStatus_t *)local_getInParam(pstParam);
	if (pstTuneStatus == NULL)
	{
		return ERR_FAIL;
	}

	if (CI_OperatorTuneStatus(pstTuneStatus->usSlotId, pstTuneStatus->ucStrength, pstTuneStatus->ucQuality, pstTuneStatus->ucStatus, pstTuneStatus->pucDes, pstTuneStatus->ucDesLen, pstTuneStatus->ucNextDesNum))
		return ERR_FAIL;
	else
		return ERR_OK;
}

static HERROR local_CI_GetOperatorNetNum(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_GetOperatorInParam_t	*pstInParam;
	void					*pvInParam, *pvOutParam;

	if ( (ulControlType >= eCACTRL_CI_MAX) || (pstParam == NULL) )
	{
		return ERR_FAIL;
	}

	pvInParam = local_getInParam(pstParam);
	pvOutParam = local_getOutParam(pstParam);
	if(pvInParam == NULL || pvOutParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_GetOperatorInParam_t *)pvInParam;

	return CiMgr_GetOpNetNum(pstInParam->usSlotId, pvOutParam);
}

static HERROR local_CI_GetOperatorNetInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_GetOperatorInParam_t	*pstInParam;
	void					*pvInParam, *pvOutParam;

	if ( (ulControlType >= eCACTRL_CI_MAX) || (pstParam == NULL) )
	{
		return ERR_FAIL;
	}

	pvInParam = local_getInParam(pstParam);
	pvOutParam = local_getOutParam(pstParam);
	if(pvInParam == NULL || pvOutParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_GetOperatorInParam_t *)pvInParam;

	return CiMgr_GetOpNetInfo(pstInParam->usSlotId, pstInParam->usNetIdx, pvOutParam);
}

static HERROR local_CI_GetOperatorTsNum(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_GetOperatorInParam_t	*pstInParam;
	void					*pvInParam, *pvOutParam;

	if ( (ulControlType >= eCACTRL_CI_MAX) || (pstParam == NULL) )
	{
		return ERR_FAIL;
	}

	pvInParam = local_getInParam(pstParam);
	pvOutParam = local_getOutParam(pstParam);
	if(pvInParam == NULL || pvOutParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_GetOperatorInParam_t *)pvInParam;

	return CiMgr_GetOpTsNum(pstInParam->usSlotId, pstInParam->usNetIdx, pvOutParam);
}

static HERROR local_CI_GetOperatorTsInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_GetOperatorInParam_t	*pstInParam;
	void					*pvInParam, *pvOutParam;

	if ( (ulControlType >= eCACTRL_CI_MAX) || (pstParam == NULL) )
	{
		return ERR_FAIL;
	}

	pvInParam = local_getInParam(pstParam);
	pvOutParam = local_getOutParam(pstParam);
	if(pvInParam == NULL || pvOutParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_GetOperatorInParam_t *)pvInParam;

	return CiMgr_GetOpTsInfo(pstInParam->usSlotId, pstInParam->usNetIdx, pstInParam->usTsIdx, pvOutParam);
}

static HERROR local_CI_GetOperatorSvcNum(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_GetOperatorInParam_t	*pstInParam;
	void					*pvInParam, *pvOutParam;

	if ( (ulControlType >= eCACTRL_CI_MAX) || (pstParam == NULL) )
	{
		return ERR_FAIL;
	}

	pvInParam = local_getInParam(pstParam);
	pvOutParam = local_getOutParam(pstParam);
	if(pvInParam == NULL || pvOutParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_GetOperatorInParam_t *)pvInParam;

	return CiMgr_GetOpSvcNum(pstInParam->usSlotId, pstInParam->usNetIdx, pstInParam->usTsIdx, pvOutParam);
}

static HERROR local_CI_GetOperatorSvcInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_GetOperatorInParam_t	*pstInParam;
	void					*pvInParam, *pvOutParam;

	if ( (ulControlType >= eCACTRL_CI_MAX) || (pstParam == NULL) )
	{
		return ERR_FAIL;
	}

	pvInParam = local_getInParam(pstParam);
	pvOutParam = local_getOutParam(pstParam);
	if(pvInParam == NULL || pvOutParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_GetOperatorInParam_t *)pvInParam;

	return CiMgr_GetOpSvcInfo(pstInParam->usSlotId, pstInParam->usNetIdx, pstInParam->usTsIdx, pvOutParam);
}

static HERROR local_CI_GetOperatorName(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_GetOperatorNameInParam_t		*pstInParam;
	CI_GetOperatorNameOutParam_t	*pstOutParam;

	if ( (ulControlType >= eCACTRL_CI_MAX) || (pstParam == NULL) )
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_GetOperatorNameInParam_t *)local_getInParam(pstParam);
	pstOutParam = (CI_GetOperatorNameOutParam_t *)local_getOutParam(pstParam);

	if(pstInParam == NULL || pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	return CiMgr_GetOpName(pstInParam->usSlotId, pstOutParam->pucOpName);
}

static HERROR local_CI_GetOperatorReservedTime(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_GetOperatorInParam_t	*pstInParam;
	void					*pvInParam, *pvOutParam;

	if ( (ulControlType >= eCACTRL_CI_MAX) || (pstParam == NULL) )
	{
		return ERR_FAIL;
	}

	pvInParam = local_getInParam(pstParam);
	pvOutParam = local_getOutParam(pstParam);
	if(pvInParam == NULL || pvOutParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_GetOperatorInParam_t *)pvInParam;

	return CI_GetOperatorReservedTime(pstInParam->usSlotId, pvOutParam);
}

static HERROR local_CI_GetOperatorSupportCis(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_GetOperatorInParam_t	*pstInParam;
	void					*pvInParam, *pvOutParam;

	if ( (ulControlType >= eCACTRL_CI_MAX) || (pstParam == NULL) )
	{
		return ERR_FAIL;
	}

	pvInParam = local_getInParam(pstParam);
	pvOutParam = local_getOutParam(pstParam);
	if(pvInParam == NULL || pvOutParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_GetOperatorInParam_t *)pvInParam;

	*(HBOOL *)pvOutParam = CI_OperatorProfileSupported(pstInParam->usSlotId);

	return ERR_OK;
}

static HERROR local_CI_GetOperatorErrStatus(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	CI_GetOperatorInParam_t	*pstInParam;
	void					*pvInParam, *pvOutParam;

	if ( (ulControlType >= eCACTRL_CI_MAX) || (pstParam == NULL) )
	{
		return ERR_FAIL;
	}

	pvInParam = local_getInParam(pstParam);
	pvOutParam = local_getOutParam(pstParam);
	if(pvInParam == NULL || pvOutParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CI_GetOperatorInParam_t *)pvInParam;

	*(HBOOL *)pvOutParam = CI_GetOperatorProfileErrStatus(pstInParam->usSlotId);

	return ERR_OK;
}

#endif	// #ifdef CONFIG_MW_CI_PLUS


