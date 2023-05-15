/**************************************************************************************/
/**
 * @file oplcas.cpp
 *
 * oplcas
 *
 * @author  Kyungkoo Kim(kkkim@humaxdigital.com)
 * @date    2012/01/04
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/
#include "oplcas.h"

#include "oapi.h"
#include "apk.h"

#include <hlib.h>

#include <stdlib.h>
#include <string.h>

/******************************************************************
	Define
******************************************************************/
/******************************************************************
	Typedef
******************************************************************/
typedef struct
{
	HUINT32 				objId;
	oplCasUiEventCallback 	cb;
} _opl_casui_cb_t;

/******************************************************************
	Static variables
******************************************************************/
static HxList_t *_opl_casui_cbList = NULL;


/******************************************************************
	Static function prototypes
******************************************************************/
static void _opl_casui_Notifier(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

/******************************************************************
	local functions
******************************************************************/


static void _opl_casui_triggerEvent(int module, int slot, int type, int session, int eventType, void *pstData)
{
	HxList_t *pzcbItem = NULL;
	_opl_casui_cb_t *pzcbData = NULL;

	pzcbItem = HLIB_LIST_First(_opl_casui_cbList);
	while(pzcbItem != NULL)
	{
		pzcbData = (_opl_casui_cb_t *)HLIB_LIST_Data(pzcbItem);
		if((pzcbData != NULL) && (pzcbData->cb != NULL))
		{
			(pzcbData->cb)(module, slot, type, session, eventType, pstData);
		}
		pzcbItem = pzcbItem->next;
		pzcbData = NULL;
	}
	return;
}

static void _opl_casui_Notifier(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	const char						*pzJsonProperty = NULL;
	DxCAS_MMI_SessionEventType_e		eEventType = eDxCASMMI_EVENTTYPE_NONE;
	DxCAS_MMI_Event_t			*pstEvent = NULL;
	DxCAS_MmiSessionType_e		eType = eDxCAS_SESSIONTYPE_NONE;
	Handle_t						hSession = 0;

	eEventType = (DxCAS_MMI_SessionEventType_e)ulParam1;
	eType = (DxCAS_MmiSessionType_e)ulParam2;
	hSession = (Handle_t)ulParam3;

	switch(eEventType)
	{
		case eDxCASMMI_EVENTTYPE_DISPLAYUI:
			if(HLIB_LIST_Length(_opl_casui_cbList) == 0)
			{
				HxLOG_Error("OPLCASUI callback null type(%d)\n", eType);
			}
			else
			{
				//hSession = s_hSessionOplCas[eType]; // TODO: 임시
				APK_CAS_MMI_GetUiEvent(eType, hSession, &pstEvent);
				if(pstEvent != NULL)
				{
					if(pstEvent->pszEncodedUiEvent != NULL)
					{
						pzJsonProperty = pstEvent->pszEncodedUiEvent;
						_opl_casui_triggerEvent(pstEvent->eModule, pstEvent->nSlot, (int)eType, (int)hSession, (int)eEventType, (void *)pzJsonProperty);
					}
					APK_CAS_MMI_RemoveUiEvent(eType, pstEvent);
				}
			}
			break;
		case eDxCASMMI_EVENTTYPE_REQUESTSESSIONSTOP:
			if(HLIB_LIST_Length(_opl_casui_cbList) == 0)
			{
				HxLOG_Error("OPLCASUI callback null type(%d)\n", eType);
			}
			else
			{
					pzJsonProperty = "{\"uidata\": \"\"}";
					_opl_casui_triggerEvent(0, 0, (int)eType, (int)hSession, (int)eEventType, (void *)pzJsonProperty);
			}
			break;
#if 0// TODO: 임시
		case eDxCASMMI_EVENTTYPE_REQUESTSESSIONSTART:
			break;

		case eDxCASMMI_EVENTTYPE_REQUESTSESSIONSTOP:
			break;

		case eDxCASMMI_EVENTTYPE_SESSIONSTARTED:
			s_hSessionOplCas[eType] = hSession;
			break;
#endif

		default:
			break;
	}

	return;
}

/******************************************************************
	public functions
******************************************************************/
BPLStatus	OPL_CasUi_SendUiResponse(const int nModule, const int nSlot, DxCAS_MmiSessionType_e eType, Handle_t hSession, const char *pcInputData)
{


	DxCAS_GroupId_e	eModule = eDxCAS_GROUPID_NONE;

	if(pcInputData == NULL)
	{
		HxLOG_Error("OPLCASUI pcInputData NULL\n");
		return BPL_STATUS_OK;
	}

	eModule = (DxCAS_GroupId_e)nModule;

	// TODO: hSession으로 control
	//hSession = s_hSessionOplCas[eType]; // TODO: 임시
	APK_CAS_MMI_SendUiResponse(eModule, nSlot, eType, hSession, pcInputData);
	return BPL_STATUS_OK;
}

BPLStatus	OPL_CasUi_CloseSession(const int nModule, const int nSlot, DxCAS_MmiSessionType_e eType, Handle_t hSession)
{
	DxCAS_GroupId_e	eModule = eDxCAS_GROUPID_NONE;

	eModule = (DxCAS_GroupId_e)nModule;

	APK_CAS_MMI_CloseSession(eModule, nSlot, eType, hSession);
	return BPL_STATUS_OK;
}

Handle_t	OPL_CasUi_OpenSession(const int nModule, const int nSlot, DxCAS_MmiSessionType_e eType, const char *pcParamData)
{
	Handle_t	hSession;
	DxCAS_GroupId_e	eModule = eDxCAS_GROUPID_NONE;

	eModule = (DxCAS_GroupId_e)nModule;

	hSession = APK_CAS_MMI_OpenSession(eModule, nSlot, eType, pcParamData);

	return hSession;
}

BPLStatus	OPL_CasUi_UnRegisterListener(HUINT32 objId)
{
	HxList_t *pzcbItem = NULL;
	_opl_casui_cb_t *pzcbData = NULL;

	pzcbItem = HLIB_LIST_First(_opl_casui_cbList);
	while(pzcbItem != NULL)
	{
		pzcbData = (_opl_casui_cb_t *)HLIB_LIST_Data(pzcbItem);
		if(pzcbData != NULL)
		{
			if(pzcbData->objId == objId)
			{
				break;
			}
		}
		pzcbItem = pzcbItem->next;
		pzcbData = NULL;
	}

	if(pzcbData != NULL)
	{
		_opl_casui_cbList = HLIB_LIST_Remove(_opl_casui_cbList, (const void*)pzcbData);
		HLIB_STD_MemFree((void *)pzcbData);
	}

	APK_CAS_MMI_UnRegisterSessionNotifier(eDxCAS_SESSIONTYPE_MENU);
	APK_CAS_MMI_UnRegisterSessionNotifier(eDxCAS_SESSIONTYPE_EXTENSION);

	return BPL_STATUS_OK;
}

BPLStatus	OPL_CasUi_RegisterListener(HUINT32 objId, oplCasUiEventCallback cbFunc)
{
	HxList_t *pzcbItem = NULL;
	_opl_casui_cb_t *pzcbData = NULL;

	pzcbItem = HLIB_LIST_First(_opl_casui_cbList);
	while(pzcbItem != NULL)
	{
		pzcbData = (_opl_casui_cb_t *)HLIB_LIST_Data(pzcbItem);
		if(pzcbData != NULL)
		{
			if(pzcbData->objId == objId)
			{
				break;
			}
		}
		pzcbItem = pzcbItem->next;
		pzcbData = NULL;
	}

	if(pzcbData != NULL)
	{
		pzcbData->cb = cbFunc;
	}
	else
	{
		pzcbData = (_opl_casui_cb_t *)HLIB_STD_MemAlloc(sizeof(_opl_casui_cb_t));
		if(pzcbData != NULL)
		{
			pzcbData->objId = objId;
			pzcbData->cb = cbFunc;
			_opl_casui_cbList = HLIB_LIST_Append(_opl_casui_cbList, (void*)pzcbData);
		}
	}

	APK_CAS_MMI_RegisterSessionNotifier(eDxCAS_SESSIONTYPE_MENU, _opl_casui_Notifier);
	APK_CAS_MMI_RegisterSessionNotifier(eDxCAS_SESSIONTYPE_EXTENSION, _opl_casui_Notifier);


	return BPL_STATUS_OK;
}

/* EOF */

