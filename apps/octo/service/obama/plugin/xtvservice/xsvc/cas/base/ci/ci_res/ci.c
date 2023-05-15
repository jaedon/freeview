/***************************************************************
* $Workfile:   ci.c  $
* $Modtime:   13 Dec 2004 09:27:32  $
* Auther : Jaehee Cho / hmkim (CI+)
*
* Desc :
*
* Copyright (c) 2002 by Humax Co., Ltd.
* All right reserved
****************************************************************/

/***************************************************************
* #ifdef statements and include
***************************************************************/
#include <cb_list.h>

#include "ci.h"
#include "ci_reshandler.h"
#include "ci_adapt.h"
#include "ci_ai.h"
#include "ci_cas.h"
#include "ci_mmi.h"

#ifdef CONFIG_MW_CI_PLUS
	#include "ci_plus.h" // for CI_Init()'s CIPLUS_Init()
#endif

#ifdef CI_PLUS_PVR_ENABLE // 원칙상 ci_plus.c 에 있어야 하나 ci_capvr 모듈이 ci_cas 모듈과 유사하므로 일단 여기에 위치하도록 함.
	#include "ci_capvr.h" // for CI_SendCaPvrCmd()
#endif

#include <_svc_cas_sub_dev_api.h>

#include "../../../local_include/_xsvc_cas.h"

/***************************************************************
* local definition
***************************************************************/




/***************************************************************
* typedef
***************************************************************/


/***************************************************************
* external variables and structures
***************************************************************/


/***************************************************************
* global variables and structures
***************************************************************/
CALLBACK_HANDLE		g_hCbNotifyCamState = NULL;
CALLBACK_HANDLE		g_hCbNotifyAppInfo = NULL;
CALLBACK_HANDLE		g_hCbNotifyCaInfo = NULL;
#ifdef CI_PLUS_PVR_ENABLE // 원칙상 ci_plus.c 에 있어야 하나 ci_capvr 모듈이 ci_cas 모듈과 유사하므로 일단 여기에 위치하도록 함.
CALLBACK_HANDLE		g_hCbNotifyCaPvrInfo = NULL;
#endif
CALLBACK_HANDLE		g_hCbNotifyMmiReq = NULL;
CALLBACK_HANDLE		g_hCbNotifyCasState = NULL;

/***************************************************************
* static variables and structures
***************************************************************/


/***************************************************************
* static function prototypes
***************************************************************/
extern int PHYS_SetRS(unsigned short slot);

/***************************************************************
* function bodies
***************************************************************/

/********************************************************************
 Function   :	xsvc_cas_CiInit
 Description :	Initialize CI
 Input      :
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
HERROR xsvc_cas_CiInit(void)
{
	/* init linked list to notify functions.. */
	g_hCbNotifyCamState = Callback_CreateList();
	g_hCbNotifyAppInfo = Callback_CreateList();
	g_hCbNotifyCaInfo = Callback_CreateList();
#ifdef CI_PLUS_PVR_ENABLE // 원칙상 ci_plus.c 에 있어야 하나 ci_capvr 모듈이 ci_cas 모듈과 유사하므로 일단 여기에 위치하도록 함.
	g_hCbNotifyCaPvrInfo = Callback_CreateList();
#endif
	g_hCbNotifyMmiReq = Callback_CreateList();
	g_hCbNotifyCasState = Callback_CreateList();

#ifdef CONFIG_MW_CI_PLUS
	CIPLUS_Init();
#endif

	if (CIRH_Init() != 0)	// CIRH(CI Resource Handler) 초기화
	{
		HxLOG_Error("[CI]---> xsvc_cas_CiInit()---> Error !!!\n");
		return -1;
	}

	return ERR_OK;
}

/********************************************************************
 Function   :	CI_ResetSlot
 Description :	Reset CAM
 Input      :	usSlotId - CI Slot ID
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_ResetSlot(CI_SLOT_ID usSlotId)
{
	HxLOG_Print("CI_ResetSlot(%d) called...\n", usSlotId);

	if (svc_cas_DevCiResetModule(usSlotId) != ERR_OK)
	{
		HxLOG_Error("[CI]---> CI_ResetSlot()---> Error : fail to reset slot %d !!!\n", usSlotId);
		return -1;
	}

	return ERR_OK;
}

int CI_SetRS(CI_SLOT_ID usSlotId)
{
	HxLOG_Print("CI_SetRS(%d) called...\n", usSlotId);

	if (PHYS_SetRS(usSlotId) != ERR_OK)
	{
		HxLOG_Error("Error : fail to reset slot %d !!!\n", usSlotId);
		return -1;
	}

	return ERR_OK;
}


/********************************************************************
 Function   :	CI_GetAppInfo
 Description :	Get CAM Application Info.
 Input      :	usSlotId - CI Slot ID
 Output     :	ppInfo - pointer of pointer of CAM application Info. data.
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_GetAppInfo(CI_SLOT_ID usSlotId, CI_APP_INFO **ppInfo)
{
	if(ppInfo == NULL)
	{
		HxLOG_Error("[CI]---> CI_GetAppInfo()---> Error : Null ppInfo pointer !!!\n");
		return -1;
	}

	/* get app info structure */
	*ppInfo = CiAi_GetAppInfo(usSlotId);
	if ((*ppInfo == NULL) || ((*ppInfo)->ucIsInitialized == 0)) // if (*ppInfo == NULL)
	{
		HxLOG_Error("[CI]---> CI_GetAppInfo()---> Error : No AppInfo !!!\n");
		return -1;
	}

	return ERR_OK;
}

/********************************************************************
 Function   :	CI_EnterMenu
 Description :	Enter CAM menu
 Input      :	usSlotId - CI Slot ID
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_EnterMenu(CI_SLOT_ID usSlotId)
{
	unsigned short usSsNo;

	HxLOG_Info("CI_EnterMenu() called...\n");

	usSsNo = CiAi_GetSsNo(usSlotId);
	if (usSsNo == 0)
	{
		HxLOG_Error("[CI]---> CI_EnterMenu()---> Error : incorrect slot id[%d] !!!\n", usSlotId);
		return -1;
	}

	if (CiAi_EnterMenu(usSsNo) < 0)
	{
		HxLOG_Error("[CI]---> CI_EnterMenu()---> Error : incorrect SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	return ERR_OK;
}

/********************************************************************
 Function   :	CI_MmiClose
 Description :	Close CI MMI
 Input      :	hMmi - CI MMI handle
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_MmiClose(MMI_HANDLE hMmi)
{
	HERROR			hErr;
	MMI_INSTANCE	*pInst;

	HxLOG_Info("CI_MmiClose() called...\n");

	hErr = CI_CheckMmiInstance(hMmi);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("[CI]---> CI_MmiClose()---> Error : CI_CheckMmiInstance() failed  hMmi = 0x%x!!!\n", hMmi);
		return -1;
	}

	pInst = (MMI_INSTANCE *)hMmi;

	if (CiMmi_SendCloseMmi(pInst->usSsNo) != ERR_OK)
	{
		HxLOG_Error("[CI]---> CI_MmiClose()---> Error : incorrect SsNo[%d] !!!\n", pInst->usSsNo);
		return -1;
	}

	return ERR_OK;
}

/********************************************************************
 Function   :	CI_MmiAnswer
 Description :	Answer to MMI_MENU, MMI_LIST
 Input      :	hMmi - CI MMI handle
 				nMmiReq - MMI request
 				ucChoiceNo - Choice number. if nMmiReq is MMI_LIST, this is don't-care value
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_MmiAnswer(MMI_HANDLE hMmi, MMI_REQ nMmiReq, unsigned char ucChoiceNo)
{
	HERROR			hErr;
	MMI_INSTANCE	*pInst;

	HxLOG_Info("CI_MmiAnswer() called...\n");

	hErr = CI_CheckMmiInstance(hMmi);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("[CI]---> CI_MmiAnswer()---> Error : CI_CheckMmiInstance() failed  hMmi = 0x%x!!!\n", hMmi);
		return -1;
	}

	pInst = (MMI_INSTANCE *)hMmi;

	HxLOG_Print("[CI]---> CI_MmiAnswer()---> SsNo[%d]\n", pInst->usSsNo);
	if(pInst->usSsNo == 0)
	{
		HxLOG_Error("[CI]---> CI_MmiAnswer()---> Error : Session is zero !!!\n");
		return -1;
	}

	if(CiMmi_SendMenuAnsw(pInst->usSsNo, nMmiReq, ucChoiceNo) != ERR_OK)
	{
		HxLOG_Error("[CI]---> CI_MmiAnswer()---> Error : incorrect SsNo[%d] !!!\n", pInst->usSsNo);
		return -1;
	}

	return ERR_OK;
}

/********************************************************************
 Function   :	CI_MmiAnswerString
 Description :	Answer string to MMI_ENQ
 Input      :	hMmi - CI MMI handle
 				pszStr - pointer of Answer String. must be freed by caller
 				ulStrSize - size of pszStr. number of byte
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_MmiAnswerString(MMI_HANDLE hMmi, unsigned char *pszStr, unsigned short usStrSize)
{
	HERROR			hErr;
	MMI_INSTANCE	*hTempMmi = NULL;

	HxLOG_Info("CI_MmiAnswerString() called...\n");
	#if defined(CONFIG_DEBUG)
	{
		HUINT32		i;

		HxLOG_Print("====================================\n");
		HxLOG_Print("CI_MmiAnswerString () answer string : ");
		for(i  = 0; i < usStrSize; i++)
		{
			HxLOG_Print("0x%x ", pszStr[i]);
		}
		HxLOG_Print("====================================\n");
	}
	#endif

	hErr = CI_CheckMmiInstance(hMmi);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("[CI]---> CI_MmiAnswerString()---> Error : CI_CheckMmiInstance() failed  hMmi = 0x%x!!!\n", hMmi);
		return -1;
	}

	hTempMmi = (MMI_INSTANCE *)hMmi;

	HxLOG_Info("[CI]---> CI_MmiAnswerString()---> hTempMmi[%p] SsNo[%d]\n", hTempMmi, hTempMmi->usSsNo);

	if(CiMmi_SendAnswString(hTempMmi->usSsNo, pszStr, usStrSize) != ERR_OK)
	{
		HxLOG_Error("[CI]---> CI_MmiAnswerString()---> Error : incorrect SsNo[%d] !!!\n", hTempMmi->usSsNo);
		return -1;
	}

	return ERR_OK;
}

/********************************************************************
 Function   :	CI_MmiGetSlotId
 Description :	Get Slot ID by MMI_HANDLE
 Input      :	hMmi - CI MMI handle
 Output     :	pusSlotId - pointer of Slot ID
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_MmiGetSlotId(MMI_HANDLE hMmi, CI_SLOT_ID *pusSlotId, unsigned short	*pusSsNo)
{
	HERROR			hErr;
	MMI_INSTANCE	*pMmiInst;
	CI_SLOT_ID		usSlotId;

	hErr = CI_CheckMmiInstance(hMmi);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("[CI]---> CI_MmiGetSlotId()---> Error : CI_CheckMmiInstance() failed  hMmi = 0x%x!!!\n", hMmi);
		return -1;
	}

	pMmiInst = (MMI_INSTANCE *)hMmi;
	if(pMmiInst == NULL)
	{
		HxLOG_Error("[CI]---> CI_MmiGetSlotId()---> Error : Null MMI handle !!!\n");
		return -1;
	}

	if (CIRH_GetSlotIdBySsNo(pMmiInst->usSsNo, &usSlotId) < 0)
	{
		HxLOG_Error("[CI]---> CI_MmiGetSlotId()---> Error : CIRH_GetSlotIdBySsNo(SsNo[%d]) !!!\n", pMmiInst->usSsNo);
		return -1;
	}

	*pusSlotId = usSlotId;
	*pusSsNo = pMmiInst->usSsNo;

	return ERR_OK;
}

/********************************************************************
 Function   :	CI_SendCaPmt
 Description :	send ca_pmt to cam
 Input      :	usSlotId - CI Slot ID
 				pucCaPmt - pointer of ca_pmt() data to be sent to cam
 				usCaPmtLen - length of pucCaPmt. number of byte
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_SendCaPmt(CI_SLOT_ID usSlotId, unsigned char *pucCaPmt, unsigned short usCaPmtLen)
{
	unsigned short usSsNo;

	HxLOG_Info("CI_SendCaPmt (%d) : CaPmt Length (%d)\n", usSlotId, usCaPmtLen);

	usSsNo = CiCas_GetSsNo(usSlotId);
	if (usSsNo == 0)
	{
		HxLOG_Error("[CI]---> CI_SendCaPmt()---> Error : incorrect SlotId[%d] or Ss not opened !!!\n", usSlotId);
		return -1;
	}

	/* send ca_pmt to cam */
	if(CiCas_SendCaPmt(usSsNo, pucCaPmt, usCaPmtLen) != ERR_OK)
	{
		HxLOG_Error("[CI]---> CI_SendCaPmt()---> Error : incorrect SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	return ERR_OK;
}

#ifdef CI_PLUS_PVR_ENABLE // 원칙상 ci_plus.c 에 있어야 하나 ci_capvr 모듈이 ci_cas 모듈과 유사하므로 일단 여기에 위치하도록 함.
/********************************************************************
 Function   :	CI_SendCaPvrCmd
 Description :	send ca_pvr_pmt / ca_pvr_cat / ca_pvr_emm_cmd / ca_pvr_ecm_cmd / ca_pvr_PINcode_cmd to cam
 Input      :	usSlotId - CI Slot ID
 				pucCaPvrCmd - pointer of data or cmd to be sent to cam
 				usCaPvrCmdLen - length of pucCaPvrCmd. number of byte
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_SendCaPvrCmd(CI_SLOT_ID usSlotId, unsigned char *pucCaPvrCmd, unsigned short usCaPvrCmdLen)
{
	unsigned short usSsNo;

	HxLOG_Print("CI_SendCaPvrCmd (%d) : CaPvrCmd Length (%d)\n", usSlotId, usCaPvrCmdLen);

	usSsNo = CiCaPvr_GetSsNo(usSlotId);
	if (usSsNo == 0)
	{
		HxLOG_Error("[CI]---> CI_SendCaPvrCmd()---> Error : incorrect SlotId[%d] or Ss not opened !!!\n", usSlotId);
		return -1;
	}

	/* send ca_pvr data or cmd to cam */
	if(CiCaPvr_SendCaPvrCmd(usSsNo, pucCaPvrCmd, usCaPvrCmdLen) != ERR_OK)
	{
		HxLOG_Error("[CI]---> CI_SendCaPvrCmd()---> Error : incorrect SsNo[%d] !!!\n", usSsNo);
		return -1;
	}

	return ERR_OK;
}
#endif

/********************************************************************
 Function   :	CI_AddNotifyFunc
 Description :	CI Extension add callback APIs
 Input      :	notifyList - callback functions list to be added
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_AddNotifyFunc(CI_NOTIFY_FUNC_LIST notifyList)
{
	/* cam state */
	if(notifyList.pfnNotifyCamState != NULL)
	{
		if(Callback_AddFunc(g_hCbNotifyCamState, (void *)notifyList.pfnNotifyCamState) == NULL)
		{
			HxLOG_Error("[CI]---> CI_AddNotifyFunc()---> Error : cam state !!!\n");
			return -1;
		}

		HxLOG_Print("[CI]---> CI_AddNotifyFunc()---> add cam state notify success...\n");
	}

	/* app info */
	if(notifyList.pfnNotifyAppInfo != NULL)
	{
		if(Callback_AddFunc(g_hCbNotifyAppInfo, (void *)notifyList.pfnNotifyAppInfo) == NULL)
		{
			HxLOG_Error("[CI]---> CI_AddNotifyFunc()---> Error : app info !!!\n");
			return -1;
		}

		HxLOG_Print("[CI]---> CI_AddNotifyFunc()---> add app info notify success...\n");
	}

	/* ca info */
	if(notifyList.pfnNotifyCaInfo != NULL)
	{
		if(Callback_AddFunc(g_hCbNotifyCaInfo, (void *)notifyList.pfnNotifyCaInfo) == NULL)
		{
			HxLOG_Error("[CI]---> CI_AddNotifyFunc()---> Error : ca info !!!\n");
			return -1;
		}

		HxLOG_Print("[CI]---> CI_AddNotifyFunc()---> add ca info notify success...\n");
	}

#ifdef CI_PLUS_PVR_ENABLE // 원칙상 ci_plus.c 에 있어야 하나 ci_capvr 모듈이 ci_cas 모듈과 유사하므로 일단 여기에 위치하도록 함.
	/* ca pvr info */
	if(notifyList.pfnNotifyCaPvrInfo != NULL)
	{
		if(Callback_AddFunc(g_hCbNotifyCaPvrInfo, (void *)notifyList.pfnNotifyCaPvrInfo) == NULL)
		{
			HxLOG_Error("[CI]---> CI_AddNotifyFunc()---> Error : ca pvr info !!!\n");
			return -1;
		}

		HxLOG_Print("[CI]---> CI_AddNotifyFunc()---> add ca pvr info notify success...\n");
	}
#endif

	/* mmi request */
	if(notifyList.pfnNotifyMmiReq != NULL)
	{
		if(Callback_AddFunc(g_hCbNotifyMmiReq, (void *)notifyList.pfnNotifyMmiReq) == NULL)
		{
			HxLOG_Error("[CI]---> CI_AddNotifyFunc()---> Error : mmi req !!!\n");
			return -1;
		}

		HxLOG_Print("[CI]---> CI_AddNotifyFunc()---> add mmi req notify success...\n");
	}

	/* cas state */
	if(notifyList.pfnNotifyCasState != NULL)
	{
		if(Callback_AddFunc(g_hCbNotifyCasState, (void *)notifyList.pfnNotifyCasState) == NULL)
		{
			HxLOG_Error("[CI]---> CI_AddNotifyFunc()---> Error : cas state !!!\n");
			return -1;
		}

		HxLOG_Print("[CI]---> CI_AddNotifyFunc()---> add cas state notify success...\n");
	}

	return ERR_OK;
}

/********************************************************************
 Function   :	CI_DeleteNotifyFunc
 Description :	CI Extension delete callback APIs
 Input      :	notifyList - callback functions list to be deleted
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_DeleteNotifyFunc(CI_NOTIFY_FUNC_LIST notifyList)
{
	/* cam state */
	if(notifyList.pfnNotifyCamState != NULL)
		Callback_RemoveFunc(g_hCbNotifyCamState, (void *)notifyList.pfnNotifyCamState);

	/* app info */
	if(notifyList.pfnNotifyAppInfo != NULL)
		Callback_RemoveFunc(g_hCbNotifyAppInfo, (void *)notifyList.pfnNotifyAppInfo);

	/* ca info */
	if(notifyList.pfnNotifyCaInfo != NULL)
		Callback_RemoveFunc(g_hCbNotifyCaInfo, (void *)notifyList.pfnNotifyCaInfo);

#ifdef CI_PLUS_PVR_ENABLE // 원칙상 ci_plus.c 에 있어야 하나 ci_capvr 모듈이 ci_cas 모듈과 유사하므로 일단 여기에 위치하도록 함.
	/* ca pvr info */
	if(notifyList.pfnNotifyCaPvrInfo != NULL)
		Callback_RemoveFunc(g_hCbNotifyCaPvrInfo, (void *)notifyList.pfnNotifyCaPvrInfo);
#endif

	/* mmi request */
	if(notifyList.pfnNotifyMmiReq != NULL)
		Callback_RemoveFunc(g_hCbNotifyMmiReq, (void *)notifyList.pfnNotifyMmiReq);

	/* cas state */
	if(notifyList.pfnNotifyCasState != NULL)
		Callback_RemoveFunc(g_hCbNotifyCasState, (void *)notifyList.pfnNotifyCasState);

	return ERR_OK;
}

