/**
	@file     xmgr_epggathering_base.c
	@brief    file_name & simple comment.

	Description: \n
	Module: \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n


	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <svc_pipe.h>

#include <bus.h>


#include <mgr_action.h>
#include <mgr_live.h>
#include <mgr_common.h>

#include <svc_si.h>


#include "../local_include/_xmgr_epggathering.h"
#include <_mgr_epggathering.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define EPG_GATHERING_TIMER_ID				0x7864
#if defined(CONFIG_DEBUG)
#define EPG_GATHERING_TIMEOUT				(30*1000)
#else
#define EPG_GATHERING_TIMEOUT				(3*60*1000)
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct tagEpgGatheringContent_Base
{
	EpgGathering_Content_t		stBaseContents;

	//1 향별 추가되는 filed는 여기에. 밑으로.
}EpgGathering_Content_Base_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
STATIC EpgGathering_Content_Base_t	s_stEpgGatheringContent_Base;


/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/
STATIC BUS_Result_t	proc_epggathering_Base(HINT32 lMessage, Handle_t hAction, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);
STATIC HERROR		xmgr_EpgGathering_InitContext_Base(EpgGathering_Content_Base_t *pstContents);
INLINE STATIC EpgGathering_Content_Base_t *xmgr_EpgGathering_GetContent_Base (void);


#define	________________Static_Function_________________________

/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/

INLINE STATIC EpgGathering_Content_Base_t *xmgr_EpgGathering_GetContent_Base (void)
{
	return &s_stEpgGatheringContent_Base;
}

STATIC HERROR	xmgr_EpgGathering_InitContext_Base(EpgGathering_Content_Base_t *pstContents)
{
	HERROR	hErr = ERR_FAIL;
	DbSvc_TsCondition_t	stCond;

	HxLOG_Trace();

	if(pstContents == NULL)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

	HxSTD_memset(pstContents, 0x00, sizeof(EpgGathering_Content_t));

	// 다 찾는다.
	DB_SVC_InitTsFindCondition(&stCond);
	hErr = DB_SVC_FindTsInfoList(&stCond, &(pstContents->stBaseContents.nTsCnt), &(pstContents->stBaseContents.pstTsInfo));
	if (hErr != ERR_OK)
	{
		HxLOG_Error("DB_SVC_FindTsInfoList failed. \n");
	}



	return hErr;
}

STATIC HERROR	xmgr_EpgGathering_DeinitContext_Base(EpgGathering_Content_Base_t *pstContents)
{
	HERROR	hErr = ERR_FAIL;

	HxLOG_Trace();

	if(pstContents == NULL)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

	if(pstContents->stBaseContents.pstTsInfo)
	{
		DB_SVC_FreeTsInfoList(pstContents->stBaseContents.pstTsInfo);
	}

	HxSTD_memset(pstContents, 0x00, sizeof(EpgGathering_Content_t));

	return hErr;
}




STATIC BUS_Result_t	proc_epggathering_Base(HINT32 lMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	EpgGathering_Content_Base_t		*pstContents = xmgr_EpgGathering_GetContent_Base();
	HERROR							hErr = ERR_FAIL;
//	HBOOL							bUpdated = FALSE;

	switch(lMessage)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Print("eMEVT_BUS_CREATE\n");
			BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
			(void)xmgr_EpgGathering_InitContext_Base(pstContents);

			hErr = xmgr_epggathering_setupBackGroundAction((EpgGathering_Content_t *)pstContents);
			HxLOG_Print("xmgr_epggathering_setupBackGroundAction() retrun hErr: %d \n", hErr);

			(void)BUS_SetTimer(EPG_GATHERING_TIMER_ID, EPG_GATHERING_TIMEOUT);

			break;

		case eMEVT_BUS_DESTROY :
			HxLOG_Print("eMEVT_BUS_DESTROY !!\n");
			(void)xmgr_EpgGathering_DeinitContext_Base(pstContents);
			(void)BUS_KillTimer(EPG_GATHERING_TIMER_ID);
			hErr = xmgr_epggathering_releaseBackGroundAction((EpgGathering_Content_t *)pstContents);
			HxLOG_Print("xmgr_epggathering_releaseBackGroundAction() retrun hErr: %d \n", hErr);

			break;

		// timer.
		case eMEVT_BUS_TIMER :
			HxLOG_Print("eMEVT_BUS_TIMER !!\n");
			if(p1 == EPG_GATHERING_TIMER_ID)
			{
				if(pstContents->stBaseContents.bManageBgAction == FALSE)
				{
					hErr = xmgr_epggathering_setupBackGroundAction((EpgGathering_Content_t *)pstContents);
					HxLOG_Print("xmgr_epggathering_setupBackGroundAction() retrun hErr: %d \n", hErr);
				}
				else
				{
					hErr = xmgr_epggathering_releaseBackGroundAction((EpgGathering_Content_t *)pstContents);
					HxLOG_Print("xmgr_epggathering_releaseBackGroundAction() retrun hErr: %d \n", hErr);
				}
			}

			break;

		// action releated.
		case eMEVT_ACTION_NOTIFY_TAKEN:
			HxLOG_Print("eMEVT_ACTION_NOTIFY_TAKEN !!, hAction: 0x%x\n", hAction);
			HxLOG_Print("pstContents->hBGAction: 0x%x, pstContents->ulBGActionId: 0x%x \n", pstContents->stBaseContents.hBGAction, pstContents->stBaseContents.ulBGActionId);
			hErr = xmgr_epggathering_checkActionIdAndType (&(pstContents->stBaseContents), hAction, p1, p2, p3);
			if(hErr == ERR_OK)
			{
				xmgr_epggathering_startTune((EpgGathering_Content_t *)pstContents);
			}
			else
			{
				HxLOG_Print("it's not my action\n");
			}
			break;

		case eMEVT_ACTION_NOTIFY_RELEASED:
			HxLOG_Print("eMEVT_ACTION_NOTIFY_RELEASED !!, hAction: 0x%x\n", hAction);
			HxLOG_Print("pstContents->hBGAction: 0x%x, pstContents->ulBGActionId: 0x%x \n", pstContents->stBaseContents.hBGAction, pstContents->stBaseContents.ulBGActionId);
			hErr = xmgr_epggathering_checkActionIdAndType (&(pstContents->stBaseContents), hAction, p1, p2, p3);
			if(hErr == ERR_OK)
			{
				hErr = xmgr_epggathering_setupBackGroundAction((EpgGathering_Content_t *)pstContents);
				HxLOG_Print("xmgr_epggathering_setupBackGroundAction() retrun hErr: %d \n", hErr);
			}
			else
			{
				HxLOG_Print("it's not my action\n");
			}
			break;

		// channel event.
		case eSEVT_CH_LOCKED :
			HxLOG_Print("eSEVT_CH_LOCKED !!, hAction: 0x%x\n", hAction);
			HxLOG_Print("pstContents->hBGAction: 0x%x, pstContents->ulBGActionId: 0x%x \n", pstContents->stBaseContents.hBGAction, pstContents->stBaseContents.ulBGActionId);
			hErr = xmgr_epggathering_checkActionIdAndType (&(pstContents->stBaseContents), hAction, p1, p2, p3);
			if(hErr == ERR_OK)
			{
				hErr = xmgr_epggathering_evtChLocked (&(pstContents->stBaseContents), hAction, p1, p2, p3);
				HxLOG_Print("xmgr_epggathering_evtChLocked() retrun hErr: %d \n", hErr);
			}
			else
			{
				HxLOG_Print("it's not my action\n");
			}
			break;

		case eSEVT_CH_NO_SIGNAL:
			HxLOG_Print("eSEVT_CH_NO_SIGNAL !!, hAction: 0x%x\n", hAction);
			HxLOG_Print("pstContents->hBGAction: 0x%x, pstContents->ulBGActionId: 0x%x \n", pstContents->stBaseContents.hBGAction, pstContents->stBaseContents.ulBGActionId);
			hErr = xmgr_epggathering_checkActionIdAndType (&(pstContents->stBaseContents), hAction, p1, p2, p3);
			if(hErr == ERR_OK)
			{
				if(pstContents->stBaseContents.bManageBgAction == FALSE)
				{
					hErr = xmgr_epggathering_setupBackGroundAction((EpgGathering_Content_t *)pstContents);
					HxLOG_Print("xmgr_epggathering_setupBackGroundAction() retrun hErr: %d \n", hErr);
				}
				else
				{
					hErr = xmgr_epggathering_releaseBackGroundAction((EpgGathering_Content_t *)pstContents);
					HxLOG_Print("xmgr_epggathering_releaseBackGroundAction() retrun hErr: %d \n", hErr);
				}

			}
			else
			{
				HxLOG_Print("it's not my action\n");
			}
			break;

	}

	return BUS_ProcessMessageDefault(lMessage, hAction, p1, p2, p3);
}


#define	________________Global_Function_________________________

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/

HERROR	xmgr_epggathering_Start_Base(void)
{
	BUS_Result_t		eRes = ERR_BUS_SOURCE_NULL;

	eRes = BUS_MGR_Create((char*)"proc_epggathering_Base", APP_ACTION_PRIORITY, proc_epggathering_Base, HANDLE_NULL, 0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR	xmgr_epggathering_Stop_Base(void)
{
	BUS_Result_t		eRes = ERR_BUS_SOURCE_NULL;

	eRes = BUS_MGR_Destroy(proc_epggathering_Base);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

// for batch
BUS_Callback_t	xmgr_epggathering_GetProc_Base(void)
{
	return proc_epggathering_Base;
}

/*********************** End of File ******************************/
/*******************************************************************/
