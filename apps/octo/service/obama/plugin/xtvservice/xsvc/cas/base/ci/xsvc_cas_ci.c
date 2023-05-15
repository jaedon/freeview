/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
/** @brief global header file  */
#include <octo_common.h>
#include <pal_ci.h>
#include <pal_pipe.h>

#include <_svc_cas_mgr_common.h>
#include <_svc_cas_mgr_main.h>
#include <_svc_cas_sub_api.h>
#include <svc_cas.h>
#include <psvc_cas.h>

#include "include/ci.h"
#include "include/ci_plus.h"
#include "control/ci_ctrl.h"
#include "../../local_include/_xsvc_cas.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

/**
  @brief     LAYER_MODULE_DoSomething 함수.

  simple comment \n

  @return    return 값.
*/



HERROR xsvc_cas_CiGetSlotId(Handle_t hMmi, CI_SLOT_ID *pusSlotId)
{
	CI_SLOT_ID	usSlotId;
	HINT32		errRet;
	HUINT16		usSsNo;

	if (NULL == pusSlotId)
	{
		HxLOG_Error(" : pusSlotId == NULL !!\n");
		return ERR_FAIL;
	}

	errRet = CI_MmiGetSlotId(hMmi, &usSlotId, &usSsNo);
	if (errRet != ERR_OK )
	{
		HxLOG_Error(" : hMmi = 0x%x has wrong slot id !!\n", hMmi);
		return ERR_FAIL;
	}

	if(usSlotId >= PAL_CI_GetNumberOfModule())
	{
		HxLOG_Error(" : hMmi = 0x%x has wrong slot id (%d) !!\n", hMmi, usSlotId);
		return ERR_FAIL;
	}

	*pusSlotId = usSlotId;

	return ERR_OK;
}

HERROR xsvc_cas_CiEnterMenu(CI_SLOT_ID usSlotId)
{
	if (CI_EnterMenu(usSlotId))
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR xsvc_cas_CiMmiClose(Handle_t hMmi)
{
	if (CI_MmiClose(hMmi))
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR xsvc_cas_CiMmiSelect(Handle_t hMmi, HUINT8 ucItemNumber)
{
	if (CI_MmiAnswer((MMI_HANDLE)hMmi, 0, ucItemNumber))
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}
	return ERR_OK;
}

HERROR xsvc_cas_CiMmiAnswerString(Handle_t hMmi, unsigned char *pszStr, unsigned short usStrSize)
{
	if (CI_MmiAnswerString((MMI_HANDLE)hMmi, pszStr, usStrSize))
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}
	return ERR_OK;
}

STATIC HERROR xsvc_cas_CiUpdateMmiData(HUINT16 usSlotId, HUINT32 ulVersion, Handle_t hMmi, MMI_REQ nMmiReq, void *pvParam, HULONG ulParamSize)
{
	SvcCas_MmiData_t stMmiData;
	HUINT32 nResult = 0;

//HxLOG_Print("slot(%d), ver(%d), hMmi(0x%x), mmi(%s), param(%d) \n",
//		usSlotId, ulVersion, (HUINT32)hMmi, svc_cas_DbgMakeStrMmiCmd(nMmiReq), (HUINT32)ulParamSize));
	HxLOG_Print("slot(%d), ver(%d), hMmi(0x%x), param(%d) \n",
		usSlotId, ulVersion, (HUINT32)hMmi, (HUINT32)ulParamSize);

	// Mapping Data
	switch(nMmiReq)
	{
	case MMI_OPEN:
		stMmiData.eDataType = eCasMmiType_Open;
		stMmiData.eCasInstType = eCasInst_CI;
		stMmiData.hMmi = hMmi;
		stMmiData.usSlotId = usSlotId;
		stMmiData.ulVersion = ulVersion;
		stMmiData.pvMmiData = NULL;
		stMmiData.ulMmiDataSize = ulParamSize;
		break;

	case MMI_CLOSE:
		stMmiData.eDataType = eCasMmiType_Close;
		stMmiData.eCasInstType = eCasInst_CI;
		stMmiData.hMmi = hMmi;
		stMmiData.usSlotId = usSlotId;
		stMmiData.ulVersion = ulVersion;
		stMmiData.pvMmiData = NULL;
		stMmiData.ulMmiDataSize = ulParamSize;
		break;

	case MMI_ENQ:
		{
			MMI_ENQ_PARAM *pEnq = NULL;

			pEnq = (MMI_ENQ_PARAM *)pvParam;
			if(pEnq == NULL)
			{
				break;
			}

#ifdef CONFIG_DEBUG
			{
				HxLOG_Print("[MMI_ENQ] \n\t [%s] --> blind = %d, answer length = %d \n", pEnq->pszPrompt, pEnq->ucBlindOnOff, pEnq->ucAnswerLen);
			}
#endif

			stMmiData.eDataType = eCasMmiType_Enq;
			stMmiData.eCasInstType = eCasInst_CI;
			stMmiData.hMmi = hMmi;
			stMmiData.usSlotId = usSlotId;
			stMmiData.ulVersion = ulVersion;
			stMmiData.pvMmiData = (MMI_ENQ_PARAM *)pvParam;
			stMmiData.ulMmiDataSize = ulParamSize;
		}
		break;

	case MMI_MENU:
		stMmiData.eDataType = eCasMmiType_Menu;
		stMmiData.eCasInstType = eCasInst_CI;
		stMmiData.hMmi = hMmi;
		stMmiData.usSlotId = usSlotId;
		stMmiData.ulVersion = ulVersion;
		stMmiData.pvMmiData = (MMI_MENU_LIST_PARAM *)pvParam;

#ifdef CONFIG_DEBUG
		{
			MMI_MENU_LIST_PARAM *pTemp = (MMI_MENU_LIST_PARAM *)pvParam;
			int i;
			HxLOG_Print("[MMI_MENU] \n");
			HxLOG_Print("\t [%s] \n", pTemp->pszTitle);
			HxLOG_Print("\t [%s] \n", pTemp->pszSubTitle);
			for (i = 0; i < pTemp->ulNumOfItem; i++)
				HxLOG_Print("\t\t [%02d:%s] \n", i+1, pTemp->ppszItem[i]);
			HxLOG_Print("\t [%s] \n", pTemp->pszBottom);
		}
#endif

		stMmiData.ulMmiDataSize = ulParamSize;
		break;

	case MMI_LIST:
		stMmiData.eDataType = eCasMmiType_List;
		stMmiData.eCasInstType = eCasInst_CI;
		stMmiData.hMmi = hMmi;
		stMmiData.usSlotId = usSlotId;
		stMmiData.ulVersion = ulVersion;
		stMmiData.pvMmiData = (MMI_MENU_LIST_PARAM *)pvParam;

#ifdef CONFIG_DEBUG
		{
			MMI_MENU_LIST_PARAM *pTemp = (MMI_MENU_LIST_PARAM *)pvParam;
			int i;
			HxLOG_Print("[MMI_LIST] \n");
			HxLOG_Print("\t [%s] \n", pTemp->pszTitle);
			HxLOG_Print("\t [%s] \n", pTemp->pszSubTitle);
			for (i = 0; i < pTemp->ulNumOfItem; i++)
				HxLOG_Print("\t\t [%02d:%s] \n", i+1, pTemp->ppszItem[i]);
			HxLOG_Print("\t [%s] \n", pTemp->pszBottom);
		}
#endif

		stMmiData.ulMmiDataSize = ulParamSize;
		break;

	case MMI_TEXT:
		stMmiData.eDataType = eCasMmiType_Text;
		stMmiData.eCasInstType = eCasInst_CI;
		stMmiData.hMmi = hMmi;
		stMmiData.usSlotId = usSlotId;
		stMmiData.ulVersion = ulVersion;
		stMmiData.pvMmiData = (HUINT8 *)pvParam;

#ifdef CONFIG_DEBUG
		{
			HxLOG_Print("[MMI_TEXT] \n");
			HxLOG_Print("\t [%s] \n", (HUINT8 *)pvParam);
		}
#endif
		stMmiData.ulMmiDataSize = ulParamSize;
		break;

	default :
		return ERR_FAIL;
	}

	nResult = xsvc_cas_CiMgrAddMmiDataList(usSlotId, &stMmiData);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("failure in add mmi data in list\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HUINT32 				s_ulMmiVersion;

// MMI Data Table 관리?
STATIC HINT32 xsvc_cas_CiUpdateMmiRequest(Handle_t hMmi, MMI_REQ nMmiReq, void *pvParam, HULONG ulParamSize)
{
	CI_SLOT_ID usSlotId;
	HERROR nResult = ERR_FAIL;

	HxLOG_Info(" \n");

	nResult =  psvc_cas_CiGetSlotId(hMmi, &usSlotId);
	if (ERR_OK != nResult)
		return ERR_FAIL;

//	ENTER_CRITICAL_MMI;

	// increase mmi data verison
	++s_ulMmiVersion;

	// Mapping Data
	nResult = xsvc_cas_CiUpdateMmiData((HUINT32)usSlotId, s_ulMmiVersion, hMmi, nMmiReq, pvParam, ulParamSize);
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	// Send To Msg
	switch(nMmiReq)
	{
/* fix for prevent #149741 */
#if defined(CONFIG_MW_CAS_NUM_SC_SLOT)
	case MMI_OPEN:
		svc_cas_MgrSubSendScCmd2MgrTask(CMD_CAS_CHECKMMI, eCasMmiType_Open, usSlotId, (HUINT8 *)&s_ulMmiVersion);
		break;
	case MMI_CLOSE:
		svc_cas_MgrSubSendScCmd2MgrTask(CMD_CAS_CHECKMMI, eCasMmiType_Close, usSlotId, (HUINT8 *)&s_ulMmiVersion);
		break;
	case MMI_ENQ:
		svc_cas_MgrSubSendScCmd2MgrTask(CMD_CAS_CHECKMMI, eCasMmiType_Enq, usSlotId, (HUINT8 *)&s_ulMmiVersion);
		break;
	case MMI_MENU:
		svc_cas_MgrSubSendScCmd2MgrTask(CMD_CAS_CHECKMMI, eCasMmiType_Menu, usSlotId, (HUINT8 *)&s_ulMmiVersion);
		break;
	case MMI_LIST:
		svc_cas_MgrSubSendScCmd2MgrTask(CMD_CAS_CHECKMMI, eCasMmiType_List, usSlotId, (HUINT8 *)&s_ulMmiVersion);
		break;
	case MMI_TEXT:
		svc_cas_MgrSubSendScCmd2MgrTask(CMD_CAS_CHECKMMI, eCasMmiType_Text, usSlotId, (HUINT8 *)&s_ulMmiVersion);
		break;
#endif

	default :
		HxLOG_Critical("\n\n");
		break;
	}

//	LEAVE_CRITICAL_MMI;

	return ERR_OK;
}

#if defined(CONFIG_MW_CI_PLUS)

STATIC SvcCas_CiPlusUpgrade_t	s_stCiPlusUpgradeStatus[PAL_MAX_CISLOT_NUMBER];
STATIC SvcCas_CcReport_t		s_stCcReportStatus[PAL_MAX_CISLOT_NUMBER];
STATIC HUINT32 				s_ulCiPlusUpgradeVersion;
STATIC HUINT32 				s_ulCcReportVersion;

STATIC HINT32 xsvc_cas_CiRequestCcReport(CI_SLOT_ID usSlotId, CIPLUS_CcReportInfo_t *pstCcReportInfo);
STATIC HINT32 xsvc_cas_CiRequestCiPlusCamUpgrade(CI_SLOT_ID usSlotId, CIPLUS_CamUpgradeInfo_t *pstCamUpgradeInfo);

STATIC HINT32 xsvc_cas_CiRequestCcReport(CI_SLOT_ID usSlotId, CIPLUS_CcReportInfo_t *pstCcReportInfo)
{
	SvcCas_CcReport_t *pData = NULL;

	if(usSlotId >= PAL_CI_GetNumberOfModule())
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if(pstCcReportInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if(pstCcReportInfo->eCcReport == eCiPlus_CcReport_AuthenticationDoneAndStartSacEstablishment
		|| pstCcReportInfo->eCcReport == eCiPlus_CcReport_UriVersionNegotiationDone)
	{
		// 이들은 CAS/CI mgr 에서 사용하기 위한 용도로 여기서는 처리할 것이 없다.
		return ERR_OK;
	}

	pData = &s_stCcReportStatus[usSlotId];

	pData->eCcReportType = pstCcReportInfo->eCcReport;
	pData->enArcErrType = pstCcReportInfo->enArcErr;
	pData->ulVer = ++s_ulCcReportVersion;

	/*
		p1 : Slot Id
		p2 : Data Version,
		p3 : NULL
	*/
	svc_cas_MgrSubUpdateCasEtcEvent(HANDLE_NULL, HANDLE_NULL, eDxCAS_GROUPID_CAM,
										eSEVT_CAS_CIPLUS_CC_REPORT, (HUINT32)usSlotId, eDxCAS_GROUPID_CAM, pData->ulVer);

	return ERR_OK;
}

/*
	CU Resource Handling
*/
STATIC HINT32 xsvc_cas_CiRequestCiPlusCamUpgrade(CI_SLOT_ID usSlotId, CIPLUS_CamUpgradeInfo_t *pstCamUpgradeInfo)
{
	SvcCas_CiPlusUpgrade_t *pData = NULL;

	if(usSlotId >= PAL_CI_GetNumberOfModule())
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if(pstCamUpgradeInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pData = &s_stCiPlusUpgradeStatus[usSlotId];

	pData->eCamUpgradeMode = pstCamUpgradeInfo->eCamUpgradeMode;
	pData->ucDownloadProgressInPercentage = pstCamUpgradeInfo->ucDownloadProgressInPercentage;
	pData->eCamUpgradeResetMode = pstCamUpgradeInfo->eCamUpgradeResetMode;
	pData->ulVer = ++s_ulCiPlusUpgradeVersion;

	HxLOG_Print("slot id (%d), UpgradeMode (%d), DownloadProgress (%d), ResetMode (%d), Version (%d) \n",
		usSlotId, pData->eCamUpgradeMode, pData->ucDownloadProgressInPercentage, pData->eCamUpgradeResetMode, pData->ulVer);

	/*
		p1 : Slot Id
		p2 : Data Version,
		p3 : NULL
	*/
	svc_cas_MgrSubUpdateCasEtcEvent(HANDLE_NULL, HANDLE_NULL, eDxCAS_GROUPID_CAM,
									eSEVT_CAS_CIPLUS_UPGRADE, (HUINT32)usSlotId, eDxCAS_GROUPID_CAM, pData->ulVer);

	return ERR_OK;
}
#endif // #ifdef CONFIG_MW_CI_PLUS


HERROR xsvc_cas_CiUiRegisterCallback(void)
{
#if defined(CONFIG_MW_CI)
{
	CI_NOTIFY_FUNC_LIST AppCiNotiFuncList;
	HxSTD_memset(&AppCiNotiFuncList, 0, sizeof(CI_NOTIFY_FUNC_LIST));
	AppCiNotiFuncList.pfnNotifyMmiReq = (CI_NOTIFY_MMI_REQ_FUNC)xsvc_cas_CiUpdateMmiRequest;
	CI_AddNotifyFunc(AppCiNotiFuncList);
}
#endif

#if defined(CONFIG_MW_CI_PLUS)
{
	CIPLUS_NOTIFY_FUNC_LIST CiPlusNotiCbList;
	HxSTD_memset(&CiPlusNotiCbList, 0, sizeof(CIPLUS_NOTIFY_FUNC_LIST));
	CiPlusNotiCbList.pfnNotifyCcReport = (CI_NOTIFY_CC_REPORT_FUNC)xsvc_cas_CiRequestCcReport; // 이건 APP 로 notify 하기 위한 용도.
	CiPlusNotiCbList.pfnNotifyCamUpgrade = (CI_NOTIFY_CAM_UPGRADE_FUNC)xsvc_cas_CiRequestCiPlusCamUpgrade;
	CIPLUS_AddNotifyFunc(CiPlusNotiCbList);
}
#endif

	return ERR_OK;
}


HERROR xsvc_cas_CiUiGetCcReport(HUINT16 usSlotId, HUINT32 ulVersion, SvcCas_CcReport_t **ppstCcReport)
{
	SvcCas_CcReport_t *pData = NULL;

	if (NULL == ppstCcReport)
		return ERR_FAIL;

	*ppstCcReport = NULL;

	if(usSlotId >= PAL_CI_GetNumberOfModule())
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pData = &s_stCcReportStatus[usSlotId];

	if(pData->ulVer != ulVersion)
	{
		return ERR_FAIL;
	}

	*ppstCcReport = pData;

	return ERR_OK;
}

#ifdef CONFIG_MW_CI_PLUS
HERROR xsvc_cas_CiGetCiPlusUpgradeStatus(HUINT16 usSlotId, HUINT32 ulVersion, SvcCas_CiPlusUpgrade_t **ppstCiPlusUpgrade)
{
	SvcCas_CiPlusUpgrade_t *pData = NULL;

	if (NULL == ppstCiPlusUpgrade)
		return ERR_FAIL;

	*ppstCiPlusUpgrade = NULL;

	if(usSlotId >= PAL_CI_GetNumberOfModule())
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pData = &s_stCiPlusUpgradeStatus[usSlotId];

	if(pData->ulVer != ulVersion)
	{
		return ERR_FAIL;
	}

	*ppstCiPlusUpgrade = pData;

	return ERR_FAIL;
}
#endif

HERROR xsvc_cas_CiChangePath(Handle_t hAction, HUINT16 usSlotId, HBOOL bConnect)
{
#ifdef CONFIG_MW_MM_PVR // for 2 tuner PVR 모델
	HUINT32			ulActionId;

	ulActionId = PAL_PIPE_GetActionId (hAction);

	if (TRUE == bConnect)
	{
		PAL_PIPE_SetCi (ulActionId, (HINT32)usSlotId);
	}
	else
	{
		PAL_PIPE_SetCi (ulActionId, -1);
	}
#else
	// 2 tuner PVR 모델이 아니면 nothing to do.
#endif

	return ERR_OK;
}

STATIC CI_CAM_STATE		s_CamState[PAL_MAX_CISLOT_NUMBER] = {CI_CAM_EXTRACTED, };
STATIC CI_APP_INFO		s_CamAppInfo[PAL_MAX_CISLOT_NUMBER];

STATIC HERROR xsvc_cas_CiUpdateCamState(HUINT16 usSlotId, HUINT32 camState)
{
	HxLOG_Info(" \n");

	s_CamState[usSlotId] = camState;

	return ERR_OK;
}

HERROR xsvc_cas_CiGetCamState(HUINT16 usSlotId, SvcCas_CamState_e *eCamState)
{
	switch(s_CamState[usSlotId])
	{
	case CI_CAM_EXTRACTED:
		*eCamState = eCAS_CAM_STATE_Removed;
		break;
	case CI_CAM_INSERTED:
		*eCamState = eCAS_CAM_STATE_Inserted;
		break;
	case CI_CAM_INITIALIZING:
		// 사용안함.
		*eCamState = eCAS_CAM_STATE_Inserted;
		break;
	case CI_CAM_INITIALIZED:
		*eCamState = eCAS_CAM_STATE_Initialized;
		break;
	default:
		HxLOG_Critical("\n\n");
		break;
	}

	return ERR_OK;
}

STATIC HINT32 xsvc_cas_CiNotifyCamState(CI_SLOT_ID usSlotId, CI_CAM_STATE eCamState)
{
	HERROR nResult = ERR_FAIL;
	SvcCas_CamState_e	eSvcCamState = eCAS_CAM_STATE_Removed;

	HxLOG_Info(" \n");

	/* CAM Slot ID Check */
	if(usSlotId >= PAL_CI_GetNumberOfModule())
	{
		HxLOG_Error("Wrong slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

	switch(eCamState)
	{
	case CI_CAM_EXTRACTED :
		xsvc_cas_CiUpdateCamState(usSlotId, CI_CAM_EXTRACTED);
		eSvcCamState = eCAS_CAM_STATE_Removed;
		break;
	case CI_CAM_INSERTED :
		xsvc_cas_CiUpdateCamState(usSlotId, CI_CAM_INSERTED);
		eSvcCamState = eCAS_CAM_STATE_Inserted;
		break;
	case CI_CAM_INITIALIZING : // 사용안함.
		xsvc_cas_CiUpdateCamState(usSlotId, CI_CAM_INITIALIZING);
		eSvcCamState = eCAS_CAM_STATE_Initializing;
		return ERR_OK; // break;
	case CI_CAM_INITIALIZED :
		xsvc_cas_CiUpdateCamState(usSlotId, CI_CAM_INITIALIZED);
		eSvcCamState = eCAS_CAM_STATE_Initialized;
		break;
	default :
		HxLOG_Critical("\n\n");
		break;
	}

	HxLOG_Print("usSlotId = %d, eCamState = %d\n", usSlotId, eCamState);
	// send msg
//	nResult = svc_cas_InstanceSendCmd2MgrTask(CMD_CAS_CHECKDEVICE, eCAS_DEV_EVT_CamState, usSlotId, HANDLE_NULL);
	nResult = svc_cas_MgrSubUpdateCamState(usSlotId, eSvcCamState);
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	return ERR_OK;
}



HERROR xsvc_cas_CiGetCamName(HUINT16 usSlotId, HUINT8 **ppszName)
{
	if(usSlotId >= PAL_CI_GetNumberOfModule())
	{
		HxLOG_Error("Wrong slot id (%d) !!\n", usSlotId);
		return ERR_FAIL;
	}

	*ppszName = (HUINT8 *)&s_CamAppInfo[usSlotId].szText;

	return ERR_OK;
}

HERROR xsvc_cas_CiGetOpName(HUINT16 usSlotId, HUINT8 **ppszOpName)
{
#if defined(CONFIG_MW_CI_PLUS_V_1_3)
	CI_GetOperatorNameInParam_t		stInParam;
	CI_GetOperatorNameOutParam_t	stOutParam;
	SvcCas_CtrlParam_t				stParam;
	static 	HUINT8					s_szOpName[256];

	HxLOG_Info("+[%s]...\n", __FUNCTION__);

	stInParam.usSlotId		= (HUINT16)usSlotId;
	stOutParam.pucOpName	= s_szOpName;
	stParam.pvIn			= &stInParam;
	stParam.pvOut			= &stOutParam;
	stParam.ulControlType	= eCACTRL_CI_GetOpName;

	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_CAM, eCACTRL_CI_GetOpName, &stParam) != ERR_OK)
	{
		HxLOG_Print("[%s:%d] SVC_CAS_CtrlGet - eCACTRL_CI_GetOpName error\n", __FUNCTION__, __HxLINE__);
		return ERR_FAIL;
	}

	*ppszOpName = s_szOpName;

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}


HERROR xsvc_cas_CiUpdateCamAppInfo(HUINT16 usSlotId, CI_APP_INFO *pAppInfo)
{
	HxLOG_Info(" \n");

	if(usSlotId >= PAL_CI_GetNumberOfModule())
	{
		HxLOG_Error("Wrong slot id (%d) !!\n", usSlotId);
		return ERR_FAIL;
	}

	HxSTD_memset(&s_CamAppInfo[usSlotId], 0x00, sizeof(CI_APP_INFO));
	HxSTD_memcpy(&s_CamAppInfo[usSlotId], pAppInfo, sizeof(CI_APP_INFO));

	return ERR_OK;
}

STATIC HINT32 xsvc_cas_CiNotifyCamAppInfo(CI_SLOT_ID usSlotId, CI_APP_INFO *pInfo)
{
	HERROR nResult = ERR_FAIL;
	SvcCas_CiAppInfo_t	stCiInfo;

	HxLOG_Info(" \n");

	/* CAM Slot ID Check */
	if(usSlotId >= PAL_CI_GetNumberOfModule())
	{
		HxLOG_Error("NotifyAppInfo : wrong slot id (%d) !!\n", usSlotId);
		return ERR_FAIL;
	}

#ifdef CONFIG_DEBUG
	HxLOG_Print("============================================\n");
	HxLOG_Print("NotifyAppInfo\n");
	HxLOG_Print("\t nType = 0x%02X\n", pInfo->nType);
	HxLOG_Print("\t usManufacturer = 0x%04X\n", pInfo->usManufacturer);
	HxLOG_Print("\t usManufacturerCode = 0x%04X\n", pInfo->usManufacturerCode);
	HxLOG_Print("\t szText = %s\n", pInfo->szText);
	HxLOG_Print("\t ucIsInitialized = %d\n", pInfo->ucIsInitialized);
	HxLOG_Print("============================================\n");
#endif

	xsvc_cas_CiUpdateCamAppInfo(usSlotId, pInfo);

	switch (pInfo->nType)
	{
	case CI_APP_TYPE_CA: 				stCiInfo.eCiAppType = eCiAppType_Ca;				break;
	case CI_APP_TYPE_EPG: 				stCiInfo.eCiAppType = eCiAppType_Epg;				break;
	case CI_APP_TYPE_SW_UPGRADE:		stCiInfo.eCiAppType = eCiAppType_SwUpgrade;			break;
	case CI_APP_TYPE_NET_INTERFACE:		stCiInfo.eCiAppType = eCiAppType_NetInterface;		break;
	case CI_APP_TYPE_ACCESS_AIDS:		stCiInfo.eCiAppType = eCiAppType_AccessAids;		break;
	case CI_APP_TYPE_UNCLASSIFIED:		stCiInfo.eCiAppType = eCiAppType_Unclassified;		break;
	default:							stCiInfo.eCiAppType = eCiAppType_Ca;				break;
	}

	HLIB_STD_StrNCpySafe((HCHAR *)stCiInfo.szText, (const HCHAR *)pInfo->szText, MAX_CI_STR_LEN);

	stCiInfo.ucIsInitialized		= pInfo->ucIsInitialized;
	stCiInfo.usManufacturer			= pInfo->usManufacturer;
	stCiInfo.usManufacturerCode		= pInfo->usManufacturerCode;

	//send msg.
//	nResult = svc_cas_InstanceSendCmd2MgrTask(CMD_CAS_CHECKDEVICE, eCAS_DEV_EVT_CamAppInfo, usSlotId, HANDLE_NULL);
	nResult = svc_cas_MgrSubUpdateCamAppinfo(usSlotId, &stCiInfo);
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	return ERR_OK;
}

HERROR xsvc_cas_CiRegisterCamCallback(void)
{
	CI_NOTIFY_FUNC_LIST AppCiNotiFuncList;
	HxSTD_memset(&AppCiNotiFuncList, 0, sizeof(CI_NOTIFY_FUNC_LIST));
	AppCiNotiFuncList.pfnNotifyCamState = (CI_NOTIFY_CAM_STATE_FUNC)xsvc_cas_CiNotifyCamState; // 이건 APP 로 notify 하기 위한 용도.
	AppCiNotiFuncList.pfnNotifyAppInfo = (CI_NOTIFY_APP_INFO_FUNC)xsvc_cas_CiNotifyCamAppInfo;
	CI_AddNotifyFunc(AppCiNotiFuncList);
	return ERR_OK;
}

extern int CiCc_GetBindedCamId(CI_SLOT_ID usSlotId, unsigned char *pucBindedCamId);
HINT32 xsvc_cas_CiGetBindedCamId(CI_SLOT_ID usSlotId, HUINT8 *pucBindedCamId)
{
	return CiCc_GetBindedCamId(usSlotId, pucBindedCamId);
}

/*********************** End of File ******************************/
