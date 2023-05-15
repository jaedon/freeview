/**************************************************************************************************/
#define ________Conax_ctrl_Private_Include__________________________________________________
/**************************************************************************************************/
#include <_svc_cas_mgr_control.h>
#include <cxmgr_action.h>
#include <conax_ctrl.h>



/**************************************************************************************************/
#define ________Conax_ctrl_Golbal_Value____________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________Conax_ctrl_Private_Define___________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define ________Conax_ctrl_Private_Type_Define_____________________________________________
/**************************************************************************************************/




/**************************************************************************************************/
#define ________Conax_ctrl_Private_Static_Value______________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________Conax_ctrl_Private_Static_Prototype__________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________Conax_ctrl_Private_Static_Prototype_Body_____________________________________
/**************************************************************************************************/
static HERROR local_CX_GetVersionInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam);
static HERROR local_CX_GetCurMatRatLevel(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam);
static HERROR local_CX_ChangeMaturityRating(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam);
static HERROR local_CX_ReqStatusInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam);
static HERROR local_CX_GetUserText(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam);
static HERROR local_CX_GetCountryIndicator(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam);
static HERROR local_CX_GetSCState(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam);
static HERROR local_CX_ChangeScPin(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam);
static HERROR local_CX_SetUserTextDisplayed(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam);
static HERROR local_CX_SetScPinForTokenPpv(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam);
static HERROR local_CX_GetCachedPin(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam);
static HERROR local_CX_SetScPin(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam);
static HERROR local_CX_OperateCachedPin(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam);
static HERROR local_CX_SetMainAction(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam);
static HERROR local_CX_SetFingerPrintDisplayed(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam);
static HERROR local_CX_GetScResetReason(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam);
static HERROR local_CX_SetCxMenuDisplayed(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam);
static HERROR local_CX_SetPinMmiDisplayed(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam);

static const SvcCas_Ctrl_t	s_astConaxCtrlTable[eCACTRL_CX_MAX]	=
{
	{
	eCACTRL_CX_GetVersionInfo,
	eCasCtrl_Get,
	NULL,	// Control
	local_CX_GetVersionInfo,	// Get
	NULL,	// Set
	NULL,	// Request
	NULL,	// Noti
	}, {
	eCACTRL_CX_GetCurMatRatLevel,
	eCasCtrl_Get,
	NULL,	// Control
	local_CX_GetCurMatRatLevel,	// Get
	NULL,	// Set
	NULL,	// Request
	NULL,	// Noti
	}, {
	eCACTRL_CX_ChangeMaturityRating,
	eCasCtrl_Control,
	local_CX_ChangeMaturityRating,	// Control
	NULL,	// Get
	NULL,	// Set
	NULL,	// Request
	NULL,	// Noti
	}, {
	eCACTRL_CX_ReqStatusInfo,
	eCasCtrl_Req,
	NULL,	// Control
	NULL,	// Get
	NULL,	// Set
	local_CX_ReqStatusInfo,	// Request
	NULL,	// Noti
	}, {
	eCACTRL_CX_GetUserText,
	eCasCtrl_Get,
	NULL,	// Control
	local_CX_GetUserText,	// Get
	NULL,	// Set
	NULL,	// Request
	NULL,	// Noti
	}, {
	eCACTRL_CX_GetCountryIndicator,
	eCasCtrl_Get,
	NULL,	// Control
	local_CX_GetCountryIndicator,	// Get
	NULL,	// Set
	NULL,	// Request
	NULL,	// Noti
	}, {
	eCACTRL_CX_GetSCState,
	eCasCtrl_Get,
	NULL,	// Control
	local_CX_GetSCState,	// Get
	NULL,	// Set
	NULL,	// Request
	NULL,	// Noti
	}, {
	eCACTRL_CX_ChangeScPin,
	eCasCtrl_Set,
	NULL,	// Control
	NULL,	// Get
	local_CX_ChangeScPin,	// Set
	NULL,	// Request
	NULL,	// Noti
	}, {
	eCACTRL_CX_SetUserTextDisplayed,
	eCasCtrl_Set,
	NULL,	// Control
	NULL,	// Get
	local_CX_SetUserTextDisplayed,	// Set
	NULL,	// Request
	NULL,	// Noti
	}, {
	eCACTRL_CX_SetScPinForTokenPpv,
	eCasCtrl_Set,
	NULL,	// Control
	NULL,	// Get
	local_CX_SetScPinForTokenPpv,	// Set
	NULL,	// Request
	NULL,	// Noti
	}, {
	eCACTRL_CX_GetCachedPin,
	eCasCtrl_Get,
	NULL,	// Control
	local_CX_GetCachedPin,	// Get
	NULL,	// Set
	NULL,	// Request
	NULL,	// Noti
	}, {
	eCACTRL_CX_SetScPin,
	eCasCtrl_Set,
	NULL,	// Control
	NULL,	// Get
	local_CX_SetScPin,	// Set
	NULL,	// Request
	NULL,	// Noti
	}, {
	eCACTRL_CX_OperateCachedPin,
	eCasCtrl_Set,
	NULL,	// Control
	NULL,	// Get
	local_CX_OperateCachedPin,	// Set
	NULL,	// Request
	NULL,	// Noti
	},{
	eCACTRL_CX_SetMainAction,
	eCasCtrl_Set,
	NULL,	// Control
	NULL,	// Get
	local_CX_SetMainAction,	// Set
	NULL,	// Request
	NULL,	// Noti
	},{
	eCACTRL_CX_SetFingerPrintDisplayed,
	eCasCtrl_Set,
	NULL,	// Control
	NULL,	// Get
	local_CX_SetFingerPrintDisplayed,	// Set
	NULL,	// Request
	NULL,	// Noti
	},{
	eCACTRL_CX_GetScResetReason,
	eCasCtrl_Get,
	NULL,	// Control
	local_CX_GetScResetReason,	// Get
	NULL,	// Set
	NULL,	// Request
	NULL,	// Noti
	},{
	eCACTRL_CX_SetCxMenuDisplayed,
	eCasCtrl_Set,
	NULL,	// Control
	NULL,	// Get
	local_CX_SetCxMenuDisplayed,	// Set
	NULL,	// Request
	NULL,	// Noti
	},{
	eCACTRL_CX_SetPinMmiDisplayed,
	eCasCtrl_Set,
	NULL,	// Control
	NULL,	// Get
	local_CX_SetPinMmiDisplayed,	// Set
	NULL,	// Request
	NULL,	// Noti
	},
};



HERROR CA_CX_RegisterCtrlList(void)
{
	HERROR	hErr;

	hErr = svc_cas_MgrSubInitControl(eDxCAS_GROUPID_CX);
	if (hErr == ERR_OK)
	{
		hErr = svc_cas_MgrSubRegisterControl(eDxCAS_GROUPID_CX, eCACTRL_CX_MAX, (SvcCas_Ctrl_t *)&s_astConaxCtrlTable[0]);
	}

	return hErr;
}

//inline void *local_getInParam(SvcCas_CtrlParam_t *pstParam)
inline Cx_InParam_t* local_getInParam(SvcCas_CtrlParam_t* pstParam)
{
#if 0
	return (Cx_InParam_t *)pstParam->unArg.pstNa->putIn;
#else
	return (Cx_InParam_t *)pstParam->pvIn;
#endif
}

//inline void *local_getOutParam(SvcCas_CtrlParam_t *pstParam)
inline Cx_OutParam_t* local_getOutParam(SvcCas_CtrlParam_t* pstParam)
{
#if 0
	return (Cx_OutParam_t *)pstParam->unArg.pstNa->putOut;
#else
	return (Cx_OutParam_t *)pstParam->pvOut;
#endif
}

//eCACTRL_CX_GetVersionInfo
static HERROR local_CX_GetVersionInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam)
{
	HERROR						hErr;
	CX_GetVersionInfoInParam_t* pstInParam;
	CX_GetVersionInfoOutParam_t* 									pstOutParam;

	if (ulControlType >= eCACTRL_CX_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}
	pstInParam = (CX_GetVersionInfoInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (CX_GetVersionInfoOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}
	hErr = CXAPI_UiGetVersionInfo(pstInParam->lScSlot, (pstOutParam->pstInfo));

	return hErr;
}

//eCACTRL_CX_GetCurMatRatLevel
static HERROR local_CX_GetCurMatRatLevel(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam)
{
	HERROR							hErr;
	CX_GetCurMatRatLevelInParam_t* 	pstInParam;
	CX_GetCurMatRatLevelOutParam_t* pstOutParam;

	if (ulControlType >= eCACTRL_CX_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}
	pstInParam = (CX_GetCurMatRatLevelInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (CX_GetCurMatRatLevelOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}
	hErr = CXAPI_UiGetMaturityRatingLevel(pstInParam->lScSlot, pstOutParam->pucCurMatRatLevel);

	return hErr;
}

//eCACTRL_CX_ChangeMaturityRating
static HERROR local_CX_ChangeMaturityRating(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam)
{
	HERROR								hErr;
	CX_ChangeMaturityRatingInParam_t* 	pstInParam;

	if (ulControlType >= eCACTRL_CX_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CX_ChangeMaturityRatingInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}
	hErr = CXAPI_UiChangeMaturityRating(pstInParam->lScSlot, pstInParam->ucMaturityRatLevel, pstInParam->pucPin);

	return hErr;
}

//eCACTRL_CX_GetStatusInfo
static HERROR local_CX_ReqStatusInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam)
{
	//	HERROR						hErr;
	CX_GetStatusInfoInParam_t* 	pstInParam;
	CX_GetStatusInfoOutParam_t* pstOutParam;
	//CX_GetStatusInfoOutParam_t		*pstOutParamTemp;
	CxStatusList_t				pstList;
	//CxStatusList_t *pstListTemp;
	CxStatusList_t				null;

	memset(&pstList, 0, sizeof(CxStatusList_t));
	memset(&null, 0, sizeof(CxStatusList_t));
	if (ulControlType >= eCACTRL_CX_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}
	pstInParam = (CX_GetStatusInfoInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (CX_GetStatusInfoOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}
	//pstOutParamTemp->pstList = CXAPI_UiGetStatusInfo(pstInParam->lScSlot, pstInParam->eType, pstInParam->ulRef,pstOutParam->pstList);

	//pstOutParam->pstList = &pstList;
	//CXAPI_UiGetStatusInfo(pstInParam->lScSlot, pstInParam->eType, pstInParam->ulRef);
	pstOutParam->pstList = CXAPI_UiGetStatusInfo(pstInParam->lScSlot, pstInParam->eType, pstInParam->ulRef);
	if (pstOutParam->pstList == NULL)
	{
		HxLOG_Error("CONAX==========================get StatusInfo data -> fail!\n\n");
	}
#if 0	// OK
	if(pstOutParam->pstList == NULL)
	{
		HxLOG_Error("CONAX==========================null....??\n");
		return ERR_FAIL;
	}
	pstOutParamTemp = pstOutParam;
	HxLOG_Error("CONAX==========================local_CX_ReqStatusInfo get data...!\n");
	//pstOutParamTemp = pstOutParam;
	while(1)
	{
		if(pstOutParamTemp->pstList == NULL)
			break;

		HxLOG_Error("CONAX==========================local_CX_ReqStatusInfo(%s | %d/%d/%d // %d/%d/%d | %d)\n",
			pstOutParamTemp->pstList->stStatus.stSubscription.szLabel,
			pstOutParamTemp->pstList->stStatus.stSubscription.stStartDate[0].usYear,
			pstOutParamTemp->pstList->stStatus.stSubscription.stStartDate[0].ucMonth,
			pstOutParamTemp->pstList->stStatus.stSubscription.stStartDate[0].ucDay,
			pstOutParamTemp->pstList->stStatus.stSubscription.stEndDate[0].usYear,
			pstOutParamTemp->pstList->stStatus.stSubscription.stEndDate[0].ucMonth,
			pstOutParamTemp->pstList->stStatus.stSubscription.stEndDate[0].ucDay,
			pstOutParamTemp->pstList->stStatus.stSubscription.szEntitlemnet);

		pstOutParamTemp->pstList = pstOutParamTemp->pstList->pstNext;
	}

#endif
	return ERR_OK;
}

//eCACTRL_CX_GetUserText
static HERROR local_CX_GetUserText(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam)
{
	HERROR						hErr;
	CX_GetUserTextOutParam_t* 	pstOutParam;

	if (ulControlType >= eCACTRL_CX_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (CX_GetUserTextOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}
	hErr = CXAPI_UiGetUserText(pstOutParam->pulNumMsg, pstOutParam->pstList);

	return hErr;
}

//eCACTRL_CX_GetCountryIndicator
static HERROR local_CX_GetCountryIndicator(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam)
{
	HERROR								hErr;
	CX_GetCountryIndicatorInParam_t* 	pstInParam;
	CX_GetCountryIndicatorOutParam_t* 	pstOutParam;

	if (ulControlType >= eCACTRL_CX_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}
	pstInParam = (CX_GetCountryIndicatorInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (CX_GetCountryIndicatorOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}
	hErr = CXAPI_UiGetCountryIndicator(pstInParam->lScSlot, pstOutParam->lCountryIdx);

	return hErr;
}
//eCACTRL_CX_GetSCState
static HERROR local_CX_GetSCState(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam)
{
	HERROR						hErr;
	CX_GetSCStateInParam_t* 	pstInParam;

	if (ulControlType >= eCACTRL_CX_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CX_GetSCStateInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}
	hErr = CXAPI_UiGetSCState(pstInParam->lScSlot);

	return hErr;
}

static HERROR local_CX_ChangeScPin(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam)
{
	HERROR						hErr;
	CX_ChangeScPinInParam_t*	pstInParam;

	if (ulControlType >= eCACTRL_CX_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CX_ChangeScPinInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = CXAPI_UiChangePin(pstInParam->nScSlot, pstInParam->pucOldPin, pstInParam->pucNewPin, pstInParam->pucNewPin);

	return hErr;
}

static HERROR local_CX_SetUserTextDisplayed(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam)
{
	HERROR								hErr;
	CX_SetUserTextDisplayedInParam_t*	pstInParam;

	if (ulControlType >= eCACTRL_CX_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CX_SetUserTextDisplayedInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = CXAPI_UiUserTextDisplayed(pstInParam->ucSeqNo, pstInParam->ulOpt);

	return hErr;
}

static HERROR local_CX_SetScPinForTokenPpv(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam)
{
	HERROR					hErr;
	CX_SetScPinInParam_t*	pstInParam;
	CX_SetScPinOutParam_t*	pstOutParam;

	if (ulControlType >= eCACTRL_CX_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CX_SetScPinInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (CX_SetScPinOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = CXAPI_SetScPinForTokenPPV(pstInParam->hHandle, pstInParam->pucPin, &pstOutParam->eReturnMsg);

	return hErr;
}

static HERROR local_CX_GetCachedPin(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam)
{
	CX_GetCachedPinOutParam_t *pstOutParam;

	if (ulControlType >= eCACTRL_CX_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (CX_GetCachedPinOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	return CXAPI_PVR_GetCaPin(pstOutParam->pucPin);
}

static HERROR local_CX_SetScPin(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam)
{
	CX_SetScPinInParam_t*	pstInParam;
	CX_SetScPinOutParam_t*	pstOutParam;
#if defined(CONFIG_MW_MM_PVR)
	Handle_t				hSession;
#endif

	if (ulControlType >= eCACTRL_CX_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CX_SetScPinInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (CX_SetScPinOutParam_t *)local_getOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	if(CXAPI_SetScPin(pstInParam->hHandle, pstInParam->pucPin, pstInParam->eMsgType, &pstOutParam->eReturnMsg) != ERR_OK)
	{
		HxLOG_Error("CXAPI_SetScPin() error\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_MM_PVR)
	if(pstInParam->eMsgType != eCxUi_GroupMr_Ask)
	{
		return ERR_OK;
	}

	switch(CXACT_GetActionTypeFromActionHandle(pstInParam->hHandle))
	{
		case eCAS_ActionType_Tsr_Play:
			break;

		default:
			return ERR_OK;
	}

	hSession = CXSESSION_GetMainHandle();
	if(hSession == HANDLE_NULL)
	{
		HxLOG_Error("hSession HANDLE_NULL\n");
		return ERR_FAIL;
	}

	if(CXSESSION_GetCaPinPass(hSession) == TRUE)
	{
		return ERR_OK;
	}

	if(CXAPI_SetScPin(HANDLE_NULL, pstInParam->pucPin, pstInParam->eMsgType, &pstOutParam->eReturnMsg) != ERR_OK)
	{
		HxLOG_Error("CXAPI_SetScPin() error\n");
		return ERR_FAIL;
	}
#endif

	return ERR_OK;
}

static HERROR local_CX_OperateCachedPin(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam)
{
	CX_SetScPinInParam_t*	pstInParam;
	HUINT8	szCachedPin[5];
	HUINT8*	szDefaultPin = "1234";
	HUINT8	ucPinStatus = 0;
	HERROR	hErr = ERR_OK;

	if (ulControlType >= eCACTRL_CX_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CX_SetScPinInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	if(CXAPI_UiGetSCState(0) != ERR_OK)
	{
		return ERR_OK;
	}

	if (pstInParam->pucPin == NULL)
	{
		hErr = CXAPI_PVR_GetCaPin(szCachedPin);
		if(hErr == ERR_OK)
		{
			ucPinStatus = CXAPI_UiEnterCaPin(0, szCachedPin);
			if(ucPinStatus == ERR_OK)
			{
				HxLOG_Info("** accept cached PIN[0x%x] \n", ucPinStatus);
				return ERR_OK;
			}
		}

		ucPinStatus = CXAPI_UiEnterCaPin(0, szDefaultPin);
		if(ucPinStatus == ERR_OK)
		{
			(void)CXAPI_PVR_SetCaPin(szDefaultPin);
			HxLOG_Info("** accept PIN (1234) [0x%x] \n", ucPinStatus);
			return ERR_OK;
		}
	}
	else
	{
		// Operate Cached CA PIN에 PIN code를 인자로 넣으면, 해당 PIN 을 cached PIN으로 저장만 한다. 즉, 해당 PIN이 올바른지에 대한 판단은 이미 이뤄졌어야 함.
		hErr = CXAPI_PVR_SetCaPin(pstInParam->pucPin);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("** Cached PIN ERROR!! PIN is not correct...\n");
			return ERR_FAIL;
		}
	}

	return ERR_FAIL;
}

static HERROR local_CX_SetMainAction(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam)
{
#if defined(CONFIG_SUPPORT_FCC)
	CX_SetMainActionInParam_t *pstInParam;

	if (ulControlType >= eCACTRL_CX_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CX_SetMainActionInParam_t *)local_getInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	return CXACT_SetMainAction(pstInParam->hMainAction, pstInParam->hPrevAction);
#else
	return ERR_FAIL;
#endif
}

static HERROR local_CX_SetFingerPrintDisplayed(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam)
{
	HERROR								hErr;

	if (ulControlType >= eCACTRL_CX_MAX)
	{
		HxLOG_Error("ERROR: check ulControlType!!\n");
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("ERROR: check pstParam!!\n");
		return ERR_FAIL;
	}

	hErr = CXAPI_UiFingerprintDisplayed();

	return hErr;
}

static HERROR local_CX_GetScResetReason(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam)
{
	CX_GetScResetReasonInParam_t *pstInParam;
	CX_GetScResetReasonOutParam_t *pstOutParam;

	if (ulControlType >= eCACTRL_CX_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (CX_GetScResetReasonInParam_t *)local_getInParam(pstParam);
	if(pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (CX_GetScResetReasonOutParam_t *)local_getOutParam(pstParam);
	if(pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	return CXAPI_ScGetResetReason(pstInParam->nScSlot, &pstOutParam->eResetReason);
}

static HERROR local_CX_SetCxMenuDisplayed(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam)
{
	CX_SetCxDisplayedInParam_t *pstInParam;
	HERROR hErr;

	if (ulControlType >= eCACTRL_CX_MAX)
	{
		HxLOG_Error("ERROR: check ulControlType!!\n");
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("ERROR: check pstParam!!\n");
		return ERR_FAIL;
	}

	pstInParam = (CX_SetCxDisplayedInParam_t *)local_getInParam(pstParam);
	if(pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = CXAPI_UiCxMenuDisplayed(pstInParam->bDisplayed);

	return hErr;
}

static HERROR local_CX_SetPinMmiDisplayed(HUINT32 ulControlType, SvcCas_CtrlParam_t* pstParam)
{
	CX_SetCxDisplayedInParam_t *pstInParam;
	HERROR hErr;

	if (ulControlType >= eCACTRL_CX_MAX)
	{
		HxLOG_Error("ERROR: check ulControlType!!\n");
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Error("ERROR: check pstParam!!\n");
		return ERR_FAIL;
	}

	pstInParam = (CX_SetCxDisplayedInParam_t *)local_getInParam(pstParam);
	if(pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = CXAPI_UiPinMmiDisplayed(pstInParam->bDisplayed);

	return hErr;
}

