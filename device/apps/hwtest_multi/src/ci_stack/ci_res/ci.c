/***************************************************************
* $Workfile:   ci.c  $
* $Modtime:   13 Dec 2004 09:27:32  $
* Auther : Jaehee Cho
*
* Desc :
*
* Copyright (c) 2002 by Humax Co., Ltd.
* All right reserved
****************************************************************/

/***************************************************************
* #ifdef statements and include
***************************************************************/
#include "cb_list.h"

#include "ci.h"
#include "ci_reshandler.h"
#include "ci_adapt.h"
#include "ci_ai.h"
#include "ci_cas.h"
#include "ci_mmi.h"
#include "pal_ci.h"
#ifdef _INCLUDE_BD_CI_
	#include "bd_ci.h"
#endif

#ifdef CI_PLUS_ENABLE
	#include "ci_plus.h" // for CIPLUS_Init()
#endif

/***************************************************************
* definition
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
CALLBACK_HANDLE				g_hCbNotifyCamState;
CALLBACK_HANDLE				g_hCbNotifyAppInfo;
CALLBACK_HANDLE				g_hCbNotifyCaSysId;
CALLBACK_HANDLE				g_hCbNotifyMmiReq;
CALLBACK_HANDLE				g_hCbNotifyCasState;

unsigned char g_aucIsCaSysId[CI_MAX_NUM_OF_CAM]; /*for CI HW_TEST*/
/***************************************************************
* static variables and structures
***************************************************************/


/***************************************************************
* static function prototypes
***************************************************************/


/***************************************************************
* function bodies
***************************************************************/

/********************************************************************
 Function   :	CI_Init
 Description :	Initialize CI
 Input      :
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_Init(void)
{
	/* init linked list to notify functions.. */
	g_hCbNotifyCamState = Callback_CreateList();
	g_hCbNotifyAppInfo = Callback_CreateList();
	g_hCbNotifyCaSysId = Callback_CreateList();
	g_hCbNotifyMmiReq = Callback_CreateList();
	g_hCbNotifyCasState = Callback_CreateList();

#ifdef CI_PLUS_ENABLE
	CIPLUS_Init();
#endif

	if (CIRH_Init() != 0)	// CIRH(CI Resource Handler) 초기화
	{
		PrintErr(("[CI]---> CI_Init()---> Error !!!\n"));
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
	VK_Print("CI_ResetSlot(%d) called...\n", usSlotId);

	if (PAL_CI_ResetModule(usSlotId) != ERR_OK)
	{
		PrintErr(("[CI]---> CI_ResetSlot()---> Error : fail to reset slot %d !!!\n", usSlotId));
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
		PrintErr(("[CI]---> CI_GetAppInfo()---> Error : Null ppInfo pointer !!!\n"));
		return -1;
	}

	/* get app info structure */
	*ppInfo = CiAi_GetAppInfo(usSlotId);
	if ((*ppInfo == NULL) || ((*ppInfo)->ucIsInitialized == 0)) // if (*ppInfo == NULL)
	{
		PrintErr(("[CI]---> CI_GetAppInfo()---> Error : No AppInfo !!!\n"));
		return -1;
	}

	return ERR_OK;
}


/********************************************************************
 Function   :	CI_DeleteTc
 Description :	Delete TC of CAM
 Input      :	usSlotId - CI Slot ID
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_DeleteTc(CI_SLOT_ID usSlotId)
{
	int	nRet;

	nRet = CIRH_DeleteTc(usSlotId);
	if (nRet != 0)
	{
		PrintErr(("[CI]---> CI_DeleteTc()---> Error : CIRH_DeleteTc() return[%d]\n", nRet));
		return nRet;
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

	usSsNo = CiAi_GetSsNo(usSlotId);
	if (usSsNo == 0)
	{
		PrintErr(("[CI]---> CI_EnterMenu()---> Error : incorrect slot id[%d] !!!\n", usSlotId));
		return -1;
	}

	if (CiAi_EnterMenu(usSsNo) < 0)
	{
		PrintErr(("[CI]---> CI_EnterMenu()---> Error : incorrect SsNo[%d] !!!\n", usSsNo));
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
	MMI_INSTANCE *pInst;

	pInst = (MMI_INSTANCE *)hMmi;

	PrintDbg(("CI_MmiClose() called...\n"));

	if(pInst == NULL)
	{
		PrintErr(("[CI]---> CI_MmiClose()---> Error : Null MMI handle !!!\n"));
		return -1;
	}

	if (CiMmi_SendCloseMmi(pInst->usSsNo) != ERR_OK)
	{
		PrintErr(("[CI]---> CI_MmiClose()---> Error : incorrect SsNo[%d] !!!\n", pInst->usSsNo));
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
	MMI_INSTANCE *pInst;

	pInst = (MMI_INSTANCE *)hMmi;

	if(pInst == NULL)
	{
		PrintErr(("[CI]---> CI_MmiAnswer()---> Error : Null MMI handle !!!\n"));
		return -1;
	}

	PrintDbg(("[CI]---> CI_MmiAnswer()---> SsNo[%d]\n", pInst->usSsNo));
	if(pInst->usSsNo == 0)
	{
		PrintErr(("[CI]---> CI_MmiAnswer()---> Error : Session is zero !!!\n"));
		return -1;
	}

	if(CiMmi_SendMenuAnsw(pInst->usSsNo, nMmiReq, ucChoiceNo) != ERR_OK)
	{
		PrintErr(("[CI]---> CI_MmiAnswer()---> Error : incorrect SsNo[%d] !!!\n", pInst->usSsNo));
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
	MMI_INSTANCE *hTempMmi = NULL;

	hTempMmi = (MMI_INSTANCE *)hMmi;
	if(hTempMmi == NULL)
	{
		PrintErr(("[CI]---> CI_MmiAnswerString()---> Error : Null MMI handle !!!\n"));
//		CA_MEM_Free(pszStr); /* 이 경우 number일텐데 통상 alloc된 메모리가 아니라 배열이다. 확인하자... -> OCTO는 alloc된 메모리 사용함 -> static array 사용으로 변경됨 */
		return -1;
	}

	PrintDbg(("[CI]---> CI_MmiAnswerString()---> hTempMmi[%p] SsNo[%d]\n", hTempMmi, hTempMmi->usSsNo));

	if(CiMmi_SendAnswString(hTempMmi->usSsNo, pszStr, usStrSize) != ERR_OK)
	{
		PrintErr(("[CI]---> CI_MmiAnswerString()---> Error : incorrect SsNo[%d] !!!\n", hTempMmi->usSsNo));
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
	MMI_INSTANCE		*pMmiInst;
	CI_SS_HANDLE	hSs;

	pMmiInst = (MMI_INSTANCE *)hMmi;
	if(pMmiInst == NULL)
	{
		PrintErr(("[CI]---> CI_MmiGetSlotId()---> Error : Null MMI handle !!!\n"));
		return -1;
	}

	hSs = CIRH_GetSs(pMmiInst->usSsNo);
	if (hSs == NULL)
	{
		PrintErr(("[CI]---> CI_MmiGetSlotId()---> Error : SsNo[%d] - NULL hSs !!!\n", pMmiInst->usSsNo));
		return -1;
	}
	if (hSs->hTc == NULL)
	{
		PrintErr(("[CI]---> CI_MmiGetSlotId()---> Error : SsNo[%d] - NULL hTc !!!\n", pMmiInst->usSsNo));
		return -1;
	}
	if (hSs->hTc->ucTcId == 0)
	{
		PrintErr(("[CI]---> CI_MmiGetSlotId()---> Error : SsNo[%d] - Invalid TcId !!!\n", pMmiInst->usSsNo));
		return -1;
	}
	if (hSs->hTc->usSlotId >= PAL_CI_GetNumberOfModule())
	{
		PrintErr(("[CI]---> CI_MmiGetSlotId()---> Error : SsNo[%d] - Invalid SlotId !!!\n", pMmiInst->usSsNo));
		return -1;
	}

	*pusSlotId = hSs->hTc->usSlotId;
	*pusSsNo = pMmiInst->usSsNo;

	return ERR_OK;
}


/********************************************************************
 Function   :	CI_SendCaPmt
 Description :	send ca_pmt to cam
 Input      :	usSlotId - CI Slot ID
 				pucCaPmt - pointer of ca_pmt() data
 				usCaPmtLen - length of pucCaPmt. number of byte
 Output     :
 Return     : 	0 for OK, -1 for Failure
 ********************************************************************/
int CI_SendCaPmt(CI_SLOT_ID usSlotId, unsigned char *pucCaPmt, unsigned short usCaPmtLen)
{
	unsigned short usSsNo;

	PrintDbg(("CI_SendCaPmt (%d) Pmt Length (%d)\n", usSlotId, usCaPmtLen));

	usSsNo = CiCas_GetSsNo(usSlotId);
	if (usSsNo == 0)
	{
		PrintErr(("[CI]---> CI_SendCaPmt()---> Error : incorrect slot id[%d] !!!\n", usSlotId));
		return -1;
	}

	/* send ca_pmt to cam */
	if(CiCas_SendCaPmt(usSsNo, pucCaPmt, usCaPmtLen) != ERR_OK)
	{
		PrintErr(("[CI]---> CI_SendCaPmt()---> Error : incorrect SsNo[%d] !!!\n", usSsNo));
		return -1;
	}

	return ERR_OK;
}


/******** CI Extension add/delete Call Back API *********/
int CI_AddNotifyFunc(CI_NOTIFY_FUNC_LIST notifyList)
{
	/* cam state */
	if(notifyList.pfnNotifyCamState != NULL)
	{
		if(Callback_AddFunc(g_hCbNotifyCamState, (void *)notifyList.pfnNotifyCamState) == NULL)
		{
			PrintErr(("[CI]---> CI_AddNotifyFunc()---> Error : cam state !!!\n"));
			return -1;
		}

		PrintDbg(("[CI]---> CI_AddNotifyFunc()---> add cam state notify success...\n"));
	}

	/* app info */
	if(notifyList.pfnNotifyAppInfo != NULL)
	{
		if(Callback_AddFunc(g_hCbNotifyAppInfo, (void *)notifyList.pfnNotifyAppInfo) == NULL)
		{
			PrintErr(("[CI]---> CI_AddNotifyFunc()---> Error : app info !!!\n"));
			return -1;
		}

		PrintDbg(("[CI]---> CI_AddNotifyFunc()---> add app info notify success...\n"));
	}

	/* ca system id */
	if(notifyList.pfnNotifyCaSysId != NULL)
	{
		if(Callback_AddFunc(g_hCbNotifyCaSysId, (void *)notifyList.pfnNotifyCaSysId) == NULL)
		{
			PrintErr(("[CI]---> CI_AddNotifyFunc()---> Error : ca system id !!!\n"));
			return -1;
		}

		PrintDbg(("[CI]---> CI_AddNotifyFunc()---> add ca system id notify success...\n"));
	}

	/* mmi request */
	if(notifyList.pfnNotifyMmiReq != NULL)
	{
		if(Callback_AddFunc(g_hCbNotifyMmiReq, (void *)notifyList.pfnNotifyMmiReq) == NULL)
		{
			PrintErr(("[CI]---> CI_AddNotifyFunc()---> Error : mmi req !!!\n"));
			return -1;
		}

		PrintDbg(("[CI]---> CI_AddNotifyFunc()---> add mmi req notify success...\n"));
	}

	/* cas state */
	if(notifyList.pfnNotifyCasState != NULL)
	{
		if(Callback_AddFunc(g_hCbNotifyCasState, (void *)notifyList.pfnNotifyCasState) == NULL)
		{
			PrintErr(("[CI]---> CI_AddNotifyFunc()---> Error : cas state !!!\n"));
			return -1;
		}

		PrintDbg(("[CI]---> CI_AddNotifyFunc()---> add cas state notify success...\n"));
	}

	return ERR_OK;
}


int CI_DeleteNotifyFunc(CI_NOTIFY_FUNC_LIST notifyList)
{
	/* cam state */
	if(notifyList.pfnNotifyCamState != NULL)
		Callback_RemoveFunc(g_hCbNotifyCamState, (void *)notifyList.pfnNotifyCamState);

	/* app info */
	if(notifyList.pfnNotifyAppInfo != NULL)
		Callback_RemoveFunc(g_hCbNotifyAppInfo, (void *)notifyList.pfnNotifyAppInfo);

	/* ca system id */
	if(notifyList.pfnNotifyCaSysId != NULL)
		Callback_RemoveFunc(g_hCbNotifyCaSysId, (void *)notifyList.pfnNotifyCaSysId);

	/* mmi request */
	if(notifyList.pfnNotifyMmiReq != NULL)
		Callback_RemoveFunc(g_hCbNotifyMmiReq, (void *)notifyList.pfnNotifyMmiReq);

	/* cas state */
	if(notifyList.pfnNotifyCasState != NULL)
		Callback_RemoveFunc(g_hCbNotifyCasState, (void *)notifyList.pfnNotifyCasState);

	return ERR_OK;
}

