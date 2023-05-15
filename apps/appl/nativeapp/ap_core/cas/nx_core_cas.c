/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/******************************************************************************/
/**
 * @file	  		nx_core_cas.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_common.h>
#include <nx_port.h>
#include <nx_port_cas.h>
#include <nx_core_cas.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct
{
	HxVector_t		*hCasSessionList;
} NxCASSessionListContext_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static NxCASSessionListContext_t	*s_stCASSessionListContext = NULL;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static NxCASSessionListContext_t	*nx_cas_GetContext(void);
static HBOOL	nx_cas_FindSessionDataCB(void *data, void *userdata);
static HERROR	nx_cas_AddSessionData(NX_CasSessionItemContext *pstCasSessionItem);
static HERROR	nx_cas_RemoveSessionData(HUINT32 ulSessionKey);

//static void		nx_cas_OnCasUiEvent(HUINT32 ulSessionHandle, DxCAS_GroupId_e eCasGroupId, HUINT32 ulSlot, DxCAS_MmiSessionType_e eSessionType, void *pszParamData);

static void		nx_cas_notifyCasMessageFunc(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
static void		nx_cas_notifyCasUiFunc(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
static void		nx_cas_notifyCasActionFunc(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static NxCASSessionListContext_t	*nx_cas_GetContext(void)
{
	if (s_stCASSessionListContext == NULL)
	{
		s_stCASSessionListContext = (NxCASSessionListContext_t *)NX_APP_Calloc(sizeof(NxCASSessionListContext_t));
		if (s_stCASSessionListContext != NULL)
		{
			s_stCASSessionListContext->hCasSessionList = HLIB_VECTOR_NewEx(NULL, 0, (void *(*)(void *))NULL,
													(void *(*)(void *))HLIB_STD_MemFree_CB, NULL);
		}
		else
		{
			NX_APP_Error("Error!!! NX_APP_Calloc() Fail!! \n");
		}
	}

	return	s_stCASSessionListContext;
}

static HBOOL	nx_cas_FindSessionDataCB(void *data, void *userdata)
{
	HUINT32 		ulSessionKey = (HUINT32)userdata;
	NX_CasSessionItemContext 	*pstCasSessionData	= (NX_CasSessionItemContext *)data;

	if (pstCasSessionData->ulSessionKey == ulSessionKey)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static HERROR	nx_cas_AddSessionData(NX_CasSessionItemContext *pstCasSessionItem)
{
	HERROR		hError = ERR_FAIL;
	NxCASSessionListContext_t *pstContext = nx_cas_GetContext();

	if (pstContext == NULL)
	{
		NX_APP_Error("Error!!! pstContext is NULL\n");
		return ERR_FAIL;
	}

	if (pstCasSessionItem == NULL)
	{
		NX_APP_Error("Error!!! pstCasSessionItem is NULL\n");
		return ERR_FAIL;
	}

	hError = HLIB_VECTOR_Add(pstContext->hCasSessionList, (void*)pstCasSessionItem);

	return hError;
}

static HERROR	nx_cas_RemoveSessionData(HUINT32 ulSessionKey)
{
	HINT32		lRemovedCount = 0;
	NxCASSessionListContext_t *pstContext = nx_cas_GetContext();

	if (pstContext == NULL)
	{
		NX_APP_Error("Error!!! pstContext is NULL\n");
		return ERR_FAIL;
	}

	lRemovedCount = HLIB_VECTOR_RemoveIf(pstContext->hCasSessionList, nx_cas_FindSessionDataCB, (void*)ulSessionKey);
	if (lRemovedCount > 0)
	{
		return ERR_OK;
	}
	else
	{
		NX_APP_Error("Error!!! HLIB_VECTOR_RemoveIf() ulSessionKey:(0x%x) Fail!!\n", ulSessionKey);
		return ERR_FAIL;
	}
}

static void		nx_cas_notifyCasMessageFunc(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	Handle_t						hSession = HANDLE_NULL;
	DxCAS_MMI_SessionEventType_e	eEventType = eDxCASMMI_EVENTTYPE_NONE;
	DxCAS_MmiSessionType_e			eSessionType = eDxCAS_SESSIONTYPE_NONE;


	eEventType = (DxCAS_MMI_SessionEventType_e)ulParam1;
	eSessionType = (DxCAS_MmiSessionType_e)ulParam2;
	hSession = (Handle_t)ulParam3;

	NX_APP_Error("\n=====> Notify CAS MSG!!! eEventType:(%d), eSessionType:(%d), hSession:(0x%x) =====\n", eEventType, eSessionType, hSession);

	switch (eEventType)
	{
		case eDxCASMMI_EVENTTYPE_DISPLAYUI:
			switch (eSessionType)
			{
				case eDxCAS_SESSIONTYPE_BANNER:
					ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE, GWM_APK_CAS_MESSAGE_EVENT, (HINT32)ulParam1, (HINT32)ulParam2, (HINT32)ulParam3);
					break;

				default:
					NX_APP_Error("Error!!! eDxCASMMI_EVENTTYPE_DISPLAYUI - eSessionType:(0x%x) Fail!!\n", eSessionType);
					break;
			}
			break;

		case eDxCASMMI_EVENTTYPE_REQUESTSESSIONSTART:
			switch(eSessionType)
			{
				case eDxCAS_SESSIONTYPE_EXTENSION:
					// TODO: Not to do - delete code after check
					// ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE, GWM_APK_CAS_UI_TRIGGERED, (HINT32)ulParam1, (HINT32)ulParam2, (HINT32)ulParam3);
					break;

				default:
					NX_APP_Error("Error!!! eDxCASMMI_EVENTTYPE_REQUESTSESSIONSTART - eSessionType:(0x%x) Fail!!\n", eSessionType);
					break;
			}
			break;

		default:
			NX_APP_Error("Error!!! Invalid Event - eEventType:(0x%x) Fail!!\n", eEventType);
			break;
	}

}

static void		nx_cas_notifyCasUiFunc(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HBOOL							bIsValidSession = FALSE;
	Handle_t						hSession = HANDLE_NULL;
	DxCAS_MmiSessionType_e			eSessionType = eDxCAS_SESSIONTYPE_NONE;
	DxCAS_MMI_SessionEventType_e	eEventType = eDxCASMMI_EVENTTYPE_NONE;


	eEventType = (DxCAS_MMI_SessionEventType_e)ulParam1;
	eSessionType = (DxCAS_MmiSessionType_e)ulParam2;
	hSession = (Handle_t)ulParam3;

	NX_APP_Error("=====> Notify CAS UI!!! eEventType:(%d), eSessionType:(%d) hSession:(0x%x) =====\n", eEventType, eSessionType, hSession);

	switch (eEventType)
	{
		case eDxCASMMI_EVENTTYPE_DISPLAYUI:
			bIsValidSession = NX_CAS_FindSessionWithHandleAndType((HUINT32)hSession, eSessionType);
			if (bIsValidSession == TRUE)
			{
				ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE, GWM_APK_CAS_UI_TRIGGERED, (HINT32)ulParam1, (HINT32)ulParam2, (HINT32)ulParam3);
			}
			break;

		case eDxCASMMI_EVENTTYPE_REQUESTSESSIONSTART:
			switch(eSessionType)
			{
				case eDxCAS_SESSIONTYPE_EXTENSION:
					break;
				default:
					break;
			}
			break;

		case eDxCASMMI_EVENTTYPE_REQUESTSESSIONSTOP:
			switch(eSessionType)
			{
				case eDxCAS_SESSIONTYPE_EXTENSION:
					break;
				default:
					break;
			}
			//ONDK_GWM_RECV_PostMsgToGWM(MSG_GWM_APK_MESSAGE, GWM_APK_CAS_UI_TRIGGERED, (HINT32)ulParam1, (HINT32)ulParam2, (HINT32)ulParam3);
			break;

		default:
			break;
	}

}

static void		nx_cas_notifyCasActionFunc(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	// TODO: Add Code
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

HERROR	NX_CAS_Init(void)
{
	NxCASSessionListContext_t	*pstContext = NULL;

	pstContext = nx_cas_GetContext();
	if (pstContext == NULL)
	{
		NX_APP_Error("Error!!! (pstContext == NULL) Fail!! \n");
		return	ERR_FAIL;
	}

	NX_CAS_MMI_RegisterSessionNotifier(eDxCAS_SESSIONTYPE_BANNER, nx_cas_notifyCasMessageFunc);
	NX_CAS_MMI_RegisterSessionNotifier(eDxCAS_SESSIONTYPE_MENU, nx_cas_notifyCasUiFunc);
	NX_CAS_MMI_RegisterSessionNotifier(eDxCAS_SESSIONTYPE_EXTENSION, nx_cas_notifyCasUiFunc);

	//NX_CAS_ACTION_RegisterNotifier(nx_cas_notifyCasActionFunc);

#if defined(CONFIG_MW_CAS_NAGRA)
	NX_CAS_NAGRA_Init();
#endif
#if defined(CONFIG_MW_CAS_IRDETO)
	NX_CAS_IRDETO_Init();
#endif

	return	ERR_OK;
}

HERROR	NX_CAS_OpenSession(HUINT32 ulSessionHandleKey, DxCAS_GroupId_e eCasGroupId, HUINT32 ulSlot, DxCAS_MmiSessionType_e eSessionType, HUINT8 *pszParamData)
{
	HERROR		hError = ERR_FAIL;
	Handle_t	hSessionHandle = HANDLE_NULL;
	NX_CasSessionItemContext	*pstCasSessionItem = NULL;

	hSessionHandle = NX_PORT_CAS_MMI_OpenSession(eCasGroupId, ulSlot, eSessionType, pszParamData);
	if ((hSessionHandle != HANDLE_NULL) && (hSessionHandle != 0))
	{
		pstCasSessionItem = (NX_CasSessionItemContext *)NX_APP_Calloc(sizeof(NX_CasSessionItemContext));
		if (pstCasSessionItem == NULL)
		{
			NX_APP_Error("Error!!! NX_APP_Calloc() Fail!! \n");
			return ERR_FAIL;
		}

		pstCasSessionItem->ulSessionKey = ulSessionHandleKey;
		pstCasSessionItem->ulSlot = ulSlot;
		pstCasSessionItem->eCasGroupId = eCasGroupId;
		pstCasSessionItem->eCasSessionType = eSessionType;
		pstCasSessionItem->ulSessionHandle = hSessionHandle;

		hError = nx_cas_AddSessionData(pstCasSessionItem);
	}
	else
	{
		NX_APP_Error("Error!!! NX_CAS_RequestBannerSession() Fail!! \n");
		hError = ERR_FAIL;
	}

	return	hError;
}

HERROR	NX_CAS_CloseSession(HUINT32 ulSessionHandleKey, DxCAS_GroupId_e eCasGroupId)
{
	HERROR		hError = ERR_FAIL;
	NX_CasSessionItemContext	*pstCasSessionItem = NULL;

	pstCasSessionItem = NX_CAS_FindSessionItem(ulSessionHandleKey, eCasGroupId);
	if (pstCasSessionItem != NULL)
	{
		NX_PORT_CAS_MMI_CloseSession(pstCasSessionItem->eCasGroupId, pstCasSessionItem->ulSlot, pstCasSessionItem->eCasSessionType, pstCasSessionItem->ulSessionHandle);

		hError = nx_cas_RemoveSessionData(ulSessionHandleKey);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! nx_cas_RemoveSessionData() - ulSessionHandleKey : (0x%x) Fail!! \n", ulSessionHandleKey);
		}
	}
	else
	{
		NX_APP_Error("Error!!! NX_CAS_FindSessionItem() - ulSessionHandleKey : (0x%x) Fail!! \n", ulSessionHandleKey);
		hError = ERR_FAIL;
	}

	return	hError;
}

HERROR	NX_CAS_CloseSessionWithHandleAndHandleKey(HUINT32 ulSessionHandleKey, HUINT32 ulSessionHandle, DxCAS_GroupId_e eCasGroupId)
{
	HERROR		hError = ERR_FAIL;
	NX_CasSessionItemContext	*pstCasSessionItem = NULL;

	pstCasSessionItem = NX_CAS_FindSessionItemWithHandlekeyAndHandleInGroup(ulSessionHandleKey, ulSessionHandle, eCasGroupId);
	if (pstCasSessionItem != NULL)
	{
		NX_PORT_CAS_MMI_CloseSession(pstCasSessionItem->eCasGroupId, pstCasSessionItem->ulSlot, pstCasSessionItem->eCasSessionType, pstCasSessionItem->ulSessionHandle);

		hError = nx_cas_RemoveSessionData(ulSessionHandleKey);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! nx_cas_RemoveSessionData() - ulSessionHandleKey : (0x%x) Fail!! \n", ulSessionHandleKey);
		}
	}
	else
	{
		NX_APP_Error("Error!!! NX_CAS_FindSessionItem() - ulSessionHandleKey : (0x%x) Fail!! \n", ulSessionHandleKey);
		hError = ERR_FAIL;
	}

	return	hError;
}

NX_CasSessionItemContext	*NX_CAS_FindSessionItem(HUINT32 ulSessionHandleKey, DxCAS_GroupId_e eCasGroupId)
{
	HBOOL		bFound = FALSE;
	HINT32		i = 0, lItemCount = 0;
	NxCASSessionListContext_t	*pstContext = NULL;
	NX_CasSessionItemContext	*pstSessionItem = NULL;

	pstContext = nx_cas_GetContext();
	if (pstContext == NULL)
	{
		NX_APP_Error("Error!!! (pstContext == NULL) Fail!! \n");
		return	NULL;
	}

	if (pstContext->hCasSessionList == NULL)
	{
		NX_APP_Error("Error!!! (pstContext->hCasSessionList == NULL) Fail!! \n");
		return	NULL;
	}

	lItemCount = HLIB_VECTOR_Length(pstContext->hCasSessionList);
	for (i = 0; i < lItemCount; i++)
	{
		pstSessionItem = HLIB_VECTOR_ItemAt(pstContext->hCasSessionList, i);
		if (pstSessionItem == NULL)
		{
			NX_APP_Debug("Error!!! HLIB_VECTOR_ItemAt() - i:(%u) Fail!! \n", i);
			continue;
		}

		if ((pstSessionItem->ulSessionKey == ulSessionHandleKey) && (pstSessionItem->eCasGroupId == eCasGroupId))
		{
			bFound = TRUE;
			break;
		}
	}

	if (bFound == TRUE)
	{
		return	pstSessionItem;
	}
	else
	{
		return	NULL;
	}
}

NX_CasSessionItemContext	*NX_CAS_FindSessionItemWithHandlekeyAndHandleInGroup(HUINT32 ulSessionHandleKey, HUINT32 ulSessionHandle, DxCAS_GroupId_e eCasGroupId)
{
	HBOOL		bFound = FALSE;
	HINT32		i = 0, lItemCount = 0;
	NxCASSessionListContext_t	*pstContext = NULL;
	NX_CasSessionItemContext	*pstSessionItem = NULL;

	pstContext = nx_cas_GetContext();
	if (pstContext == NULL)
	{
		NX_APP_Error("Error!!! (pstContext == NULL) Fail!! \n");
		return	NULL;
	}

	if (pstContext->hCasSessionList == NULL)
	{
		NX_APP_Error("Error!!! (pstContext->hCasSessionList == NULL) Fail!! \n");
		return	NULL;
	}

	lItemCount = HLIB_VECTOR_Length(pstContext->hCasSessionList);
	for (i = 0; i < lItemCount; i++)
	{
		pstSessionItem = HLIB_VECTOR_ItemAt(pstContext->hCasSessionList, i);
		if (pstSessionItem == NULL)
		{
			NX_APP_Debug("Error!!! HLIB_VECTOR_ItemAt() - i:(%u) Fail!! \n", i);
			continue;
		}

		if ((pstSessionItem->ulSessionKey == ulSessionHandleKey) && (pstSessionItem->ulSessionHandle == ulSessionHandle) && (pstSessionItem->eCasGroupId == eCasGroupId))
		{
			bFound = TRUE;
			break;
		}
	}

	if (bFound == TRUE)
	{
		return	pstSessionItem;
	}
	else
	{
		return	NULL;
	}
}

HBOOL	NX_CAS_FindSessionWithHandleAndType(HUINT32 ulSessionHandle, DxCAS_MmiSessionType_e eSessionType)
{
	HBOOL		bFound = FALSE;
	HINT32		i = 0, lItemCount = 0;
	NxCASSessionListContext_t	*pstContext = NULL;
	NX_CasSessionItemContext	*pstSessionItem = NULL;

	pstContext = nx_cas_GetContext();
	if (pstContext == NULL)
	{
		NX_APP_Error("Error!!! (pstContext == NULL) Fail!! \n");
		return	FALSE;
	}

	if (pstContext->hCasSessionList == NULL)
	{
		NX_APP_Error("Error!!! (pstContext->hCasSessionList == NULL) Fail!! \n");
		return	FALSE;
	}

	bFound = FALSE;
	lItemCount = HLIB_VECTOR_Length(pstContext->hCasSessionList);
	for (i = 0; i < lItemCount; i++)
	{
		pstSessionItem = HLIB_VECTOR_ItemAt(pstContext->hCasSessionList, i);
		if (pstSessionItem == NULL)
		{
			NX_APP_Debug("Error!!! HLIB_VECTOR_ItemAt() - i:(%u) Fail!! \n", i);
			continue;
		}

		if ((pstSessionItem->eCasSessionType == eSessionType) && (pstSessionItem->ulSessionHandle == ulSessionHandle))
		{
			bFound = TRUE;
			break;
		}
	}

	if (bFound == FALSE)
	{
		NX_APP_Error("Error!!! Do not fond session item - ulSessionHandle:(0x%x),  eSessionType:(0x%x) Fail!! \n", ulSessionHandle, eSessionType);
	}

	return	bFound;
}

HBOOL	NX_CAS_FindSessionWithHandleAndTypeInGroup(HUINT32 ulSessionHandle, DxCAS_MmiSessionType_e eSessionType, DxCAS_GroupId_e eCasGroupId)
{
	HBOOL		bFound = FALSE;
	HINT32		i = 0, lItemCount = 0;
	NxCASSessionListContext_t	*pstContext = NULL;
	NX_CasSessionItemContext	*pstSessionItem = NULL;

	pstContext = nx_cas_GetContext();
	if (pstContext == NULL)
	{
		NX_APP_Error("Error!!! (pstContext == NULL) Fail!! \n");
		return	FALSE;
	}

	if (pstContext->hCasSessionList == NULL)
	{
		NX_APP_Error("Error!!! (pstContext->hCasSessionList == NULL) Fail!! \n");
		return	FALSE;
	}

	bFound = FALSE;
	lItemCount = HLIB_VECTOR_Length(pstContext->hCasSessionList);
	for (i = 0; i < lItemCount; i++)
	{
		pstSessionItem = HLIB_VECTOR_ItemAt(pstContext->hCasSessionList, i);
		if (pstSessionItem == NULL)
		{
			NX_APP_Debug("Error!!! HLIB_VECTOR_ItemAt() - i:(%u) Fail!! \n", i);
			continue;
		}

		if ((pstSessionItem->eCasSessionType == eSessionType) && (pstSessionItem->ulSessionHandle == ulSessionHandle) && (pstSessionItem->eCasGroupId == eCasGroupId))
		{
			bFound = TRUE;
			break;
		}
	}

	if (bFound == FALSE)
	{
		NX_APP_Error("Error!!! Do not fond session item - ulSessionHandle:(0x%x),  eSessionType:(0x%x) Fail!! \n", ulSessionHandle, eSessionType);
	}

	return	bFound;
}


HBOOL	NX_CAS_IsValidSession(HUINT32 ulSessionHandleKey, HUINT32 ulSessionHandle, DxCAS_GroupId_e eCasGroupId)
{
	HBOOL		bFound = FALSE;
	HINT32		i = 0, lItemCount = 0;
	NxCASSessionListContext_t	*pstContext = NULL;
	NX_CasSessionItemContext	*pstSessionItem = NULL;

	pstContext = nx_cas_GetContext();
	if (pstContext == NULL)
	{
		NX_APP_Error("Error!!! (pstContext == NULL) Fail!! \n");
		return	FALSE;
	}

	if (pstContext->hCasSessionList == NULL)
	{
		NX_APP_Error("Error!!! (pstContext->hCasSessionList == NULL) Fail!! \n");
		return	FALSE;
	}

	bFound = FALSE;
	lItemCount = HLIB_VECTOR_Length(pstContext->hCasSessionList);
	for (i = 0; i < lItemCount; i++)
	{
		pstSessionItem = HLIB_VECTOR_ItemAt(pstContext->hCasSessionList, i);
		if (pstSessionItem == NULL)
		{
			NX_APP_Debug("Error!!! HLIB_VECTOR_ItemAt() - i:(%u) Fail!! \n", i);
			continue;
		}

		if ((pstSessionItem->ulSessionKey == ulSessionHandleKey) && (pstSessionItem->eCasGroupId == eCasGroupId) && (pstSessionItem->ulSessionHandle == ulSessionHandle))
		{
			bFound = TRUE;
			break;
		}
	}

	if (bFound == FALSE)
	{
		NX_APP_Error("Error!!! Do not fond session item - ulSessionHandleKey:(0x%x), eCasGroupId:(0x%x), ulSessionHandle:(0x%x) Fail!! \n", ulSessionHandleKey, eCasGroupId, ulSessionHandle);
	}

	return	bFound;
}

NX_CasSessionItemContext	*NX_CAS_GetSessionItem(HINT32 lIndex)
{
	HINT32		lItemCount = 0;
	NxCASSessionListContext_t	*pstContext = NULL;
	NX_CasSessionItemContext	*pstSessionItem = NULL;

	pstContext = nx_cas_GetContext();
	if (pstContext == NULL)
	{
		NX_APP_Error("Error!!! (pstContext == NULL) Fail!! \n");
		return	NULL;
	}

	if (pstContext->hCasSessionList == NULL)
	{
		NX_APP_Error("Error!!! (pstContext->hCasSessionList == NULL) Fail!! \n");
		return	NULL;
	}

	lItemCount = HLIB_VECTOR_Length(pstContext->hCasSessionList);
	if (lItemCount < 1)
	{
		NX_APP_Error("Error!!! HLIB_VECTOR_Length == 0 Fail!! \n");
		return	NULL;
	}

	pstSessionItem = HLIB_VECTOR_ItemAt(pstContext->hCasSessionList, lIndex);
	if (pstSessionItem == NULL)
	{
		NX_APP_Debug("Error!!! HLIB_VECTOR_ItemAt() - lIndex:(%u) Fail!! \n", lIndex);
		return	NULL;
	}
	else
	{
		return	pstSessionItem;
	}
}

HINT32	NX_CAS_GetSessionCount(void)
{
	HINT32		lItemCount = 0;
	NxCASSessionListContext_t	*pstContext = NULL;

	pstContext = nx_cas_GetContext();
	if (pstContext == NULL)
	{
		NX_APP_Error("Error!!! (pstContext == NULL) Fail!! \n");
		return	lItemCount;
	}

	if (pstContext->hCasSessionList == NULL)
	{
		NX_APP_Error("Error!!! (pstContext->hCasSessionList == NULL) Fail!! \n");
		return	lItemCount;
	}

	lItemCount = HLIB_VECTOR_Length(pstContext->hCasSessionList);

	return	lItemCount;
}

HERROR	NX_CAS_GetCasMessage(HUINT32 ulSession, DxCAS_GroupId_e *peCasGroupId, HUINT32 *pulSlot, HUINT8 **ppszMessage)
{
	HERROR				hError = ERR_OK;
	DxCAS_MMI_Event_t	*pstEvent = NULL;


	*peCasGroupId = eDxCAS_GROUPID_NONE;
	*pulSlot = 0;

	NX_CAS_MMI_GetUiEvent(eDxCAS_SESSIONTYPE_BANNER, ulSession, &pstEvent);
	if (pstEvent == NULL)
	{
		hError = ERR_FAIL;
		*ppszMessage = NULL;
		NX_APP_Error("Error!!! (pstEvent == NULL) Fail!! \n");
	}
	else
	{
		*peCasGroupId = pstEvent->eModule;
		*pulSlot = pstEvent->nSlot;
		if (pstEvent->pszEncodedUiEvent == NULL)
		{
			*ppszMessage = NULL;
			NX_APP_Error("Error!!! (pstEvent->pszEncodedUiEvent == NULL) Fail!! \n");
		}
		else
		{
			*ppszMessage = (HUINT8*)HLIB_STD_StrDup(pstEvent->pszEncodedUiEvent);
		}

		NX_CAS_MMI_RemoveUiEvent(eDxCAS_SESSIONTYPE_BANNER, pstEvent);
	}

	return hError;
}

HERROR	NX_CAS_GetCasUiEvtInfo(HUINT32 *pulzEvtType, DxCAS_GroupId_e *peCasGroupId, HUINT32 *pulSlot, HUINT32 *pulSession)
{
	DxCAS_MMI_Event_t		*pstEvent = NULL;


	if ((pulzEvtType == NULL) || (peCasGroupId == NULL) || (pulSlot == NULL) || (pulSession == NULL))
	{
		NX_APP_Error("Error!!! Init Fail!! \n");
		return	ERR_FAIL;
	}

	NX_CAS_MMI_GetEvtInfo(&pstEvent);
	if (pstEvent != NULL)
	{
		*pulzEvtType = pstEvent->eventType;
		*peCasGroupId = pstEvent->eModule;
		*pulSlot = pstEvent->nSlot;
		*pulSession = pstEvent->hSession;

		NX_CAS_MMI_RemoveEvtInfo(pstEvent);
		return	ERR_OK;
	}
	else
	{
		return	ERR_FAIL;
	}
}

HERROR	NX_CAS_ReleaseCasMessage(HUINT8 *pszMessage)
{
	if (pszMessage != NULL)
	{
		HLIB_STD_MemFree(pszMessage);
	}

	return ERR_OK;
}

HERROR	NX_CAS_RequestCasData(Nx_CasSessionInfo_t *pstCasSessionInfo)
{
	HERROR		hError = ERR_FAIL;

	if (pstCasSessionInfo == NULL)
	{
		NX_APP_Error("Error!!! (pstCasSessionInfo == NULL) \n");
		return	ERR_FAIL;
	}

	NX_APP_Info("Request NA CAS Data - ulSessionHandleKey:(0x%x), eCasGroupId:(0x%x), ulSlot:(0x%x), eSessionType:(0x%x)\n",
		pstCasSessionInfo->ulSessionHandleKey, pstCasSessionInfo->eCasGroupId, pstCasSessionInfo->ulSlot, pstCasSessionInfo->eSessionType);

	hError = NX_CAS_OpenSession(pstCasSessionInfo->ulSessionHandleKey, pstCasSessionInfo->eCasGroupId, pstCasSessionInfo->ulSlot,
								pstCasSessionInfo->eSessionType, (HUINT8*)pstCasSessionInfo->pszParamData);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_CAS_OpenSession() - eSessionType:(0x%x) Fail!! \n", pstCasSessionInfo->eSessionType);
	}

	return	hError;
}


/******************************************************************************/
#define __CAS_MMI_API__________________________________________________________
/******************************************************************************/

void	NX_CAS_MMI_RemoveEvtInfo(DxCAS_MMI_Event_t *pstEvent)
{
	NX_PORT_CAS_MMI_RemoveEvtInfo(pstEvent);
}

void	NX_CAS_MMI_GetEvtInfo(DxCAS_MMI_Event_t **ppstEvent)
{
	NX_PORT_CAS_MMI_GetEvtInfo(ppstEvent);
}

void	NX_CAS_MMI_RemoveUiEvent(DxCAS_MmiSessionType_e eSessionType, DxCAS_MMI_Event_t *pstEvent)
{
	NX_PORT_CAS_MMI_RemoveUiEvent(eSessionType, pstEvent);
}

void	NX_CAS_MMI_GetUiEvent(DxCAS_MmiSessionType_e eSession, Handle_t hSession, DxCAS_MMI_Event_t **ppstEvent)
{
	NX_PORT_CAS_MMI_GetUiEvent(eSession, hSession, ppstEvent);
}

void	NX_CAS_MMI_SendUiResponse(DxCAS_GroupId_e eCasGroupId, HUINT32 ulSlot, DxCAS_MmiSessionType_e eSession, Handle_t hSession, HUINT8 *pzInputData)
{
	NX_PORT_CAS_MMI_SendUiResponse(eCasGroupId, ulSlot, eSession, hSession, pzInputData);
}

void	NX_CAS_MMI_UnRegisterSessionNotifier(DxCAS_MmiSessionType_e eSession)
{
	NX_PORT_CAS_MMI_UnRegisterSessionNotifier(eSession);
}

void	NX_CAS_MMI_RegisterSessionNotifier(DxCAS_MmiSessionType_e eSession, APK_CAS_MMI_Notifier_t fnNotifier)
{
	NX_PORT_CAS_MMI_RegisterSessionNotifier(eSession, (APK_CAS_MMI_Notifier_t)fnNotifier);
}


/******************************************************************************/
#define __CAS_ACTION_API_______________________________________________________
/******************************************************************************/

void	NX_CAS_ACTION_RegisterNotifier(APK_CAS_ACTION_Notifier_t fnNotfiler)
{
	NX_PORT_CAS_ACTION_RegisterNotifier(fnNotfiler);
}

void	NX_CAS_ACTION_UnRegisterNotifier(APK_CAS_ACTION_Notifier_t fnNotfiler)
{
	NX_PORT_CAS_ACTION_UnRegisterNotifier(fnNotfiler);
}

HERROR 	NX_CAS_ACTION_GetActionType(HUINT8 *pucActionType)
{
	return	NX_PORT_CAS_ACTION_GetActionType(pucActionType);
}

HERROR	NX_CAS_ACTION_GetSlotId(HUINT8 *pucSlotId)
{
	return	NX_PORT_CAS_ACTION_GetSlotId(pucSlotId);
}

HERROR 	NX_CAS_ACTION_GetCasId(HUINT8 *pucCasId)
{
	return	NX_PORT_CAS_ACTION_GetCasId(pucCasId);
}

HERROR 	NX_CAS_ACTION_GetData(HUINT8 *pucData)
{
	return	NX_PORT_CAS_ACTION_GetData(pucData);
}


/******************************************************************************/
#define __CAS_UTIL_API_______________________________________________________
/******************************************************************************/

DxCAS_GroupId_e		NX_CAS_GetCasGroupIdByCasTypeText(HUINT8 *pszCasType)
{
	HCHAR		*pszString = NULL;
	HINT32		lStrLen = 0;
	DxCAS_GroupId_e	eCasGroupId = eDxCAS_GROUPID_NONE;


	if (pszCasType == NULL)
	{
		NX_APP_Error("Error!!! (pszCasType == NULL) \n");
		return	eDxCAS_GROUPID_NONE;
	}

	lStrLen = sizeof(pszCasType);
	if (lStrLen < 0)
	{
		NX_APP_Error("Error!!! (lStrLen < 0) \n");
		return	eDxCAS_GROUPID_NONE;
	}

	pszString = HLIB_STD_StrLower((HCHAR*)pszCasType);

	if (0 == HxSTD_StrNCmp((char*)NX_CAS_TYPE_NAGRA_STRING_ID, (char*)pszString,  lStrLen))
	{
		eCasGroupId = eDxCAS_GROUPID_NA;
	}
	else if (0 == HxSTD_StrNCmp((char*)NX_CAS_TYPE_IRDETO_STRING_ID, (char*)pszString,  lStrLen))
	{
		eCasGroupId = eDxCAS_GROUPID_IR;
	}
	else if (0 == HxSTD_StrNCmp((char*)NX_CAS_TYPE_CONAX_STRING_ID, (char*)pszString,  lStrLen))
	{
		eCasGroupId = eDxCAS_GROUPID_CX;
	}
	else if (0 == HxSTD_StrNCmp((char*)NX_CAS_TYPE_CAM_STRING_ID, (char*)pszString,  lStrLen))
	{
		eCasGroupId = eDxCAS_GROUPID_CAM;
	}
	else if (0 == HxSTD_StrNCmp((char*)NX_CAS_TYPE_VERIMATRIX_STRING_ID, (char*)pszString,  lStrLen))
	{
		eCasGroupId = eDxCAS_GROUPID_VX;
	}
	// TODO: Add Cas Type
	else
	{
		eCasGroupId = eDxCAS_GROUPID_NONE;
	}

	return	eCasGroupId;
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
