/**
	@file	ap_bdmanager.c
	@brief	\n

	Description: 					\n
	Module:						\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>

#include <octo_common.h>
#include <isosvc.h>
#include <svc_pipe.h>
#include <svc_resources.h>

#include <bus.h>
#include <mgr_common.h>
#include <mgr_action.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



#if defined(CONFIG_DEBUG)
#define	LOG				HxLOG_Print
#define	ERR_LOG			HxLOG_Error
#define MSG_LOG			HxLOG_Message
#else
#define	LOG
#define	ERR_LOG
#define MSG_LOG
#endif



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/


/*******************************************************************/
/************************** Function *********************************/
/*******************************************************************/

HERROR MGR_RSC_GetResourceInfo (HUINT32 ulActionId, SvcRsc_Info_t *pstRscInfo)
{
	RxRscType_e			 eRscItem;
	HUINT32		 ulRscId;
	SvcRsc_Item_t			*pstRscItem;
	HERROR				 hErr;

	if (pstRscInfo == NULL)					{ return ERR_OK; }

	HxSTD_memset (pstRscInfo, 0, sizeof(SvcRsc_Info_t));

	pstRscInfo->hAction = SVC_PIPE_GetActionHandle (ulActionId);				// 언제나 최신 Action Handle로
	for (eRscItem = 0; eRscItem < eRxRSCTYPE_MAX; eRscItem++)
	{
		pstRscItem = &(pstRscInfo->astRscItem[eRscItem]);

		hErr = SVC_PIPE_GetResourceId (ulActionId, eRscItem, &ulRscId);
		if (hErr == ERR_OK)
		{
			pstRscItem->bAllocated		= TRUE;
			pstRscItem->ulResourceId	= ulRscId;
		}
	}

	return ERR_OK;
}

HERROR MGR_RSC_GetResourceId (HUINT32 ulActionId, RxRscType_e eRscType, HUINT32 *peRscId)
{
	HERROR			hErr;
	SvcRsc_Info_t 	stRscInfo;

	if((eRscType >= eRxRSCTYPE_MAX) || (peRscId == NULL))
		return ERR_FAIL;

	hErr = MGR_RSC_GetResourceInfo(ulActionId, &stRscInfo);
	if(hErr != ERR_OK)
		return ERR_FAIL;

	if(stRscInfo.astRscItem[eRscType].bAllocated != TRUE)
		return ERR_FAIL;

	*peRscId = stRscInfo.astRscItem[eRscType].ulResourceId;
	return ERR_OK;
}


