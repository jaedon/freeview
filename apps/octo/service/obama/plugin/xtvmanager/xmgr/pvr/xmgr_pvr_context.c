/**
	@file     xmgr_pvr_context.c
	@brief
	Description:
	Module:

	Copyright (c) 2013 HUMAX Co., Ltd.
	All rights reserved.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <octo_common.h>

#include <svc_fs.h>
#include <mgr_action.h>
#include <mgr_live.h>

#include "local_include/_xmgr_pvr_action.h"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
STATIC XmgrPvr_Context_t	*s_pstPvrMgr_Context = NULL;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Static Function *******************************/
/*******************************************************************/
#define ___INTERNAL_FUNCTIONS___
#define _____CONTEXT_MANAGING_FUNCTIONS_____

STATIC INLINE XmgrPvr_Context_t *xmgr_pvr_getContext (void)
{
	return s_pstPvrMgr_Context;
}

STATIC HERROR xmgr_pvr_initContext (void)
{
	HUINT32 	i;

	if (s_pstPvrMgr_Context == NULL)
	{
		s_pstPvrMgr_Context = (XmgrPvr_Context_t *)OxAP_Malloc (sizeof(XmgrPvr_Context_t));
		if (s_pstPvrMgr_Context == NULL)
		{
			return ERR_FAIL;
		}
	}

	HxSTD_MemSet (s_pstPvrMgr_Context, 0, sizeof(XmgrPvr_Context_t));

	for(i=0 ; i < MAX_CONNECTABLE_PVR_STORAGE_NUM ; i++)
	{
		s_pstPvrMgr_Context->ePvrDevState[i]	= ePvrDevState_NoDevice;
		s_pstPvrMgr_Context->ePvrDevIdx[i]		= eSVC_FS_DEV_NULL;
	}

	for (i = 0; i < PVRMGR_ACTION_PB_NUM; i++)
	{
		s_pstPvrMgr_Context->astPbActItem[i].ulPvrIdx = 0xffff;
	}
	for (i = 0; i < PVRMGR_ACTION_REC_NUM; i++)
	{
		s_pstPvrMgr_Context->astRecActItem[i].ulPvrIdx = 0xffff;
	}
	for (i = 0; i < NUM_BG_ACTION; i++)
	{
		s_pstPvrMgr_Context->astTrPbAction[i].ulPvrIdx = 0xffff;
	}

	return ERR_OK;
}


#define _____ACTION_FINDING_FILTERS_____
STATIC HBOOL xmgr_pvr_filterSamePvrActType (XmgrPvr_Action_t *pstActItem, XmgrPvr_Action_t *pstFilter)
{
	if (pstActItem != NULL && pstFilter != NULL)
	{
		if (ePvrActType_ALL == pstFilter->eType)
		{
			return TRUE;
		}

		// Filter의 Type에 대전제를 넣었으면 대전제에 해당하는 것들을 선택하고,
		// 소전제일 경우엔 정확히 해당되는 것들만 넣도록 한다.
		if (0 != (pstFilter->eType & ePvrActType_SUBITEMMASK))
		{
			if (pstFilter->eType == pstActItem->eType)
			{
				return TRUE;
			}
		}
		else
		{
			if ((pstFilter->eType & ePvrActType_MASK) == (pstActItem->eType & ePvrActType_MASK))
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

STATIC HBOOL xmgr_pvr_filterSameActionId (XmgrPvr_Action_t *pstActItem, XmgrPvr_Action_t *pstFilter)
{
	if (pstActItem != NULL && pstFilter != NULL)
	{
		if (TRUE == xmgr_pvr_filterSamePvrActType(pstActItem, pstFilter))
		{
			if (pstActItem->ulActionId == pstFilter->ulActionId)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

STATIC HBOOL xmgr_pvr_filterSameRelatedActionId(XmgrPvr_Action_t *pstActItem, XmgrPvr_Action_t *pstFilter)
{
	if (pstActItem != NULL && pstFilter != NULL)
	{
		if (TRUE == xmgr_pvr_filterSamePvrActType(pstActItem, pstFilter))
		{
			if (pstActItem->ulRelatedActId == pstFilter->ulRelatedActId)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

STATIC HBOOL xmgr_pvr_filterSameRelatedViewId(XmgrPvr_Action_t *pstActItem, XmgrPvr_Action_t *pstFilter)
{
	if (pstActItem != NULL && pstFilter != NULL)
	{
		if (TRUE == xmgr_pvr_filterSamePvrActType(pstActItem, pstFilter))
		{
			if (pstActItem->ulRelatedViewId == pstFilter->ulRelatedViewId)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

STATIC HBOOL xmgr_pvr_filterSameTimerId(XmgrPvr_Action_t *pstActItem, XmgrPvr_Action_t *pstFilter)
{
	if (pstActItem != NULL && pstFilter != NULL)
	{
		if (TRUE == xmgr_pvr_filterSamePvrActType(pstActItem, pstFilter))
		{
			if (pstActItem->ulTimerId == pstFilter->ulTimerId)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

STATIC HBOOL xmgr_pvr_filterSameSvcHandle_NormalSvc (XmgrPvr_Action_t *pstActItem, XmgrPvr_Action_t *pstFilter)
{
	HERROR			 		 hErr;

	if (pstActItem != NULL && pstFilter != NULL)
	{
		if (TRUE == xmgr_pvr_filterSamePvrActType(pstActItem, pstFilter))
		{
			switch (pstActItem->eType & ePvrActType_MASK)
			{
			case ePvrActType_REC:
			case ePvrActType_TSR:
				{
					MgrRec_RecInfo_t	 stRecInfo;

					HxSTD_MemSet (&stRecInfo, 0, sizeof(MgrRec_RecInfo_t));
					hErr = MGR_RECORDER_GetRecordingSvcInfo (pstActItem->ulActionId, &stRecInfo);
					if ((hErr == ERR_OK) && (stRecInfo.hMasterSvc == pstFilter->hMasterSvcHandle)
						&& (stRecInfo.hSvc == pstFilter->hMasterSvcHandle))
					{
						return TRUE;
					}
					else if (pstActItem->hCurrSvcHandle == pstFilter->hCurrSvcHandle
							&& pstActItem->hMasterSvcHandle== pstFilter->hMasterSvcHandle)
					{
						// timer가 걸려서 대기 중인 TSR은 여기서 다시 한번 걸러진다..
						return TRUE;
					}
				}

				break;

			default:
				break;
			}
		}
	}

	return FALSE;
}

STATIC HBOOL xmgr_pvr_filterSameSvcHandle_SuppleSvc (XmgrPvr_Action_t *pstActItem, XmgrPvr_Action_t *pstFilter)
{
	HERROR			 		 hErr;

	if (pstActItem != NULL && pstFilter != NULL)
	{
		if (TRUE == xmgr_pvr_filterSamePvrActType(pstActItem, pstFilter))
		{
			switch (pstActItem->eType & ePvrActType_MASK)
			{
			case ePvrActType_REC:
			case ePvrActType_TSR:
				{
					MgrRec_RecInfo_t	 stRecInfo;

					HxSTD_MemSet (&stRecInfo, 0, sizeof(MgrRec_RecInfo_t));
					hErr = MGR_RECORDER_GetRecordingSvcInfo (pstActItem->ulActionId, &stRecInfo);
					if ((hErr == ERR_OK) && (stRecInfo.hMasterSvc == pstFilter->hMasterSvcHandle)
						&& (stRecInfo.hSvc == pstFilter->hCurrSvcHandle))
					{
						return TRUE;
					}
				}

				break;

			default:
				break;
			}
		}
	}

	return FALSE;
}



#define ___MEMBER_FUNCTIONS___

#define _____PVR_CONTEXT_____
XmgrPvr_Context_t *xmgr_pvr_GetContext (void)
{
	if (s_pstPvrMgr_Context == NULL)
	{
		xmgr_pvr_initContext();
	}
	
	return s_pstPvrMgr_Context;
}

#define _____PVR_ACTION_ITEM_____
HERROR xmgr_pvr_ConvertAmActTypeToPvrActType (MgrAction_Type_e eAmActType, XmgrPvr_ActionType_e *pePvrActType)
{
	if (pePvrActType == NULL)				{ return ERR_FAIL; }

	switch (eAmActType)
	{
	// Playback 관련 Action Type
	case eViewType_PB:
		*pePvrActType = ePvrActType_PB_Normal;
		break;

	case eViewType_TSR_PB:
		*pePvrActType = ePvrActType_PB_Tsr;
		break;

	case eViewType_CHASE_PB:
		*pePvrActType = ePvrActType_PB_Chase;
		break;

	// Record 관련 Action Type
	case eRecType_REC:
		*pePvrActType = ePvrActType_REC_Normal;
		break;

	// TSR 관련 Action Type
	case eRecType_TSR_REC:
		*pePvrActType = ePvrActType_TSR_Normal;
		break;

	// BD 관련 Action Type
	case eRecType_EXT_REC:
		*pePvrActType = ePvrActType_REC_ExtTsLine;
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR xmgr_pvr_ConvertPvrActTypeToAmActType (XmgrPvr_ActionType_e ePvrActType, MgrAction_Type_e *peAmActType)
{
	if (peAmActType == NULL)				{ return ERR_FAIL; }

	switch (ePvrActType)
	{
	// Playback
	case ePvrActType_PB_Normal:
		*peAmActType = eViewType_PB;
		break;

	case ePvrActType_PB_Chase:
		*peAmActType = eViewType_CHASE_PB;
		break;

	case ePvrActType_PB_Tsr:
		*peAmActType = eViewType_TSR_PB;
		break;

	// Recording
	case ePvrActType_REC_Normal:
	case ePvrActType_REC_EvtReserved:
	case ePvrActType_REC_Delayed:
	case ePvrActType_REC_MemBuffer:
		*peAmActType = eRecType_REC;
		break;

	case ePvrActType_REC_ExtTsLine:
		*peAmActType = eRecType_EXT_REC;
		break;

	// TSR
	case ePvrActType_TSR_Normal:
	case ePvrActType_TSR_LiveStream:
	case ePvrActType_TSR_Extern:
		*peAmActType = eRecType_TSR_REC;
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

XmgrPvr_Action_t *xmgr_pvr_GetEmptyPvrAction (XmgrPvr_ActionType_e ePvrActType)
{
	HUINT32					 ulIndex;
	XmgrPvr_Action_t			*pstAction;
	XmgrPvr_Context_t		*pstContext;

	pstContext = xmgr_pvr_GetContext();
	if (pstContext == NULL)					{ return NULL; }

	if ((ePvrActType == ePvrActType_ALL) || ((ePvrActType & ePvrActType_MASK) == ePvrActType_PB))
	{
		for (ulIndex = 0; ulIndex < PVRMGR_ACTION_PB_NUM; ulIndex++)
		{
			pstAction = &(pstContext->astPbActItem[ulIndex]);
			if (pstAction->eState == ePvrActState_None)
			{
				return pstAction;
			}
		}
	}

	if ((ePvrActType == ePvrActType_ALL) ||
		((ePvrActType & ePvrActType_MASK) == ePvrActType_REC) ||
		((ePvrActType & ePvrActType_MASK) == ePvrActType_TSR))
	{
		for (ulIndex = 0; ulIndex < PVRMGR_ACTION_REC_NUM; ulIndex++)
		{
			pstAction = &(pstContext->astRecActItem[ulIndex]);
			if (pstAction->eState == ePvrActState_None)
			{
				return pstAction;
			}
		}
	}

	return NULL;
}

XmgrPvr_Action_t *xmgr_pvr_FindPvrActionWithFilter (XmgrPvr_ActionType_e ePvrActType, XmgrPvr_Action_t *pstFilter, XmgrPvr_ActionFilter_Cb pfFilterFunc)
{
	HUINT32					 ulIndex;
	HBOOL					 bMatch;
	XmgrPvr_Action_t			*pstAction;
	XmgrPvr_Context_t		*pstContext;

	pstContext = xmgr_pvr_GetContext();
	if (pstContext == NULL)					{ return NULL; }

	if ((ePvrActType == ePvrActType_ALL) || ((ePvrActType & ePvrActType_MASK) == ePvrActType_PB))
	{
		for (ulIndex = 0; ulIndex < PVRMGR_ACTION_PB_NUM; ulIndex++)
		{
			pstAction = &(pstContext->astPbActItem[ulIndex]);
			if (pstAction->eState != ePvrActState_None)
			{
				if (pfFilterFunc != NULL && pstFilter != NULL)
				{
					bMatch = pfFilterFunc (pstAction, pstFilter);
					if (bMatch == TRUE)
					{
						return pstAction;
					}
				}
			}
		}
	}

	if ((ePvrActType == ePvrActType_ALL) ||
		((ePvrActType & ePvrActType_MASK) == ePvrActType_REC) || ((ePvrActType & ePvrActType_MASK) == ePvrActType_TSR))
	{
		for (ulIndex = 0; ulIndex < PVRMGR_ACTION_REC_NUM; ulIndex++)
		{
			pstAction = &(pstContext->astRecActItem[ulIndex]);
			if (pstAction->eState != ePvrActState_None)
			{
				if ((ePvrActType == ePvrActType_ALL) ||
					((pstAction->eType & ePvrActType_MASK) == (ePvrActType & ePvrActType_MASK)))
				{
					if (pfFilterFunc != NULL && pstFilter != NULL)
					{
						bMatch = pfFilterFunc(pstAction, pstFilter);
						if (bMatch == TRUE)
						{
							return pstAction;
						}
					}
				}
			}
		}
	}

	return NULL;
}

XmgrPvr_Action_t *xmgr_pvr_FindPvrActionByServiceHandle (XmgrPvr_ActionType_e ePvrActType, MgrLiveView_SessionType_t eSessionType, Handle_t hMasterSvc, Handle_t hCurrSvc)
{
	XmgrPvr_Action_t			 stActFilterItem;
	XmgrPvr_Action_t			*pstAction;

	stActFilterItem.eType				= ePvrActType;
	stActFilterItem.hMasterSvcHandle	= hMasterSvc;
	stActFilterItem.hCurrSvcHandle		= hCurrSvc;

	if (eSessionType == eLiveViewSession_NORMAL)
	{
		pstAction = xmgr_pvr_FindPvrActionWithFilter(ePvrActType, &stActFilterItem, xmgr_pvr_filterSameSvcHandle_NormalSvc);
	}
	else
	{
		pstAction = xmgr_pvr_FindPvrActionWithFilter(ePvrActType, &stActFilterItem, xmgr_pvr_filterSameSvcHandle_SuppleSvc);
	}

	return pstAction;
}

XmgrPvr_Action_t *xmgr_pvr_FindPvrActionByActionId (XmgrPvr_ActionType_e ePvrActType, HUINT32 ulActionId)
{
	XmgrPvr_Action_t			 stActFilterItem;

	stActFilterItem.eType		= ePvrActType;
	stActFilterItem.ulActionId	= ulActionId;
	return xmgr_pvr_FindPvrActionWithFilter(ePvrActType, &stActFilterItem, xmgr_pvr_filterSameActionId);
}

XmgrPvr_Action_t *xmgr_pvr_FindPvrActionByRelatedActionId (XmgrPvr_ActionType_e ePvrActType, HUINT32 ulActionId)
{
	XmgrPvr_Action_t			 stActFilterItem;

	stActFilterItem.eType			= ePvrActType;
	stActFilterItem.ulRelatedActId	= ulActionId;
	return xmgr_pvr_FindPvrActionWithFilter(ePvrActType, &stActFilterItem, xmgr_pvr_filterSameRelatedActionId);
}

XmgrPvr_Action_t *xmgr_pvr_FindPvrActionByRelatedViewId (XmgrPvr_ActionType_e ePvrActType, HUINT32 ulViewId)
{
	XmgrPvr_Action_t			 stActFilterItem;

	stActFilterItem.eType			= ePvrActType;
	stActFilterItem.ulRelatedViewId	= ulViewId;
	return xmgr_pvr_FindPvrActionWithFilter(ePvrActType, &stActFilterItem, xmgr_pvr_filterSameRelatedViewId);
}

XmgrPvr_Action_t *xmgr_pvr_FindPvrActionByTimerId (XmgrPvr_ActionType_e ePvrActType, HUINT32 ulTimerId)
{
	XmgrPvr_Action_t			 stActFilterItem;

	stActFilterItem.eType		= ePvrActType;
	stActFilterItem.ulTimerId	= ulTimerId;
	return xmgr_pvr_FindPvrActionWithFilter(ePvrActType, &stActFilterItem, xmgr_pvr_filterSameTimerId);
}

HERROR xmgr_pvr_DestroyPvrAction (XmgrPvr_Action_t *pstAction)
{
	return xmgr_pvr_DestroyPvrAction_BASE(pstAction);
}

HERROR xmgr_pvr_ResetPvrAction (XmgrPvr_Action_t *pstAction)
{
	if (NULL == pstAction)					{ return ERR_FAIL; }

	HxSTD_MemSet (pstAction, 0, sizeof(XmgrPvr_Action_t));

	pstAction->eState = ePvrActState_None;
	pstAction->ulRelatedActId = HANDLE_NULL;
	pstAction->ulRelatedViewId = HANDLE_NULL;

	return ERR_OK;
}

#define ___END_OF_FILE___
