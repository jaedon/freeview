/**
	@file     xmgr_pvr_tsr_base.c
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
#include <dlib.h>
#include	<otl.h>
#include <octo_common.h>

#include <db_svc.h>

#include <svc_pipe.h>
#include <svc_resources.h>
#include <svc_ch.h>
#include <svc_si.h>
#include <svc_cas.h>
#include <svc_av.h>
#include <svc_sbtl.h>
#include <svc_ttx.h>
#include <svc_epg.h>
#include <svc_sys.h>
#include <svc_output.h>

#include <bus.h>


#include <isosvc.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_live.h>

#include <svc_fs.h>
#include <svc_rec.h>
#include <svc_pb.h>
#include <svc_meta.h>
#include <mgr_storage.h>
#include <mgr_pvr_action.h>
#include <mgr_recorder.h>
#include <mgr_playback.h>
#include <mgr_transcoder.h>

#include <uapi.h>

#include "../local_include/_xmgr_pvr_action.h"
#if defined(CONFIG_PROD_DEVICE_2ND_INTERNAL_HDD)
#include <sapi.h>
#endif

#define CMD_PRINT			HLIB_CMD_Printf

#define PVR_MGR_DELAY_TSRREC_TIMERID			0x00000100
#define PVR_MGR_DELAY_TSRREC_TIMERID_MASK		0xFFFFFF00
#define PVR_MGR_DELAY_TSRREC_TIMERID_ACTIDMASK	0x000000FF

#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE) || defined(CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HGS1000S)
#define PVR_MGR_DELAY_TSRREC_TIMEOUT			(1 * 1000)
#else
#define PVR_MGR_DELAY_TSRREC_TIMEOUT			(5 * 1000)
#endif

#if defined(CONFIG_PROD_DEVICE_2ND_INTERNAL_HDD)
#define SPACE_PER_MIN_FOR_SD					(15 * 1024 / 10)	// 1.5GB per hour
#define SPACE_PER_MIN_FOR_HD					(4 * 1024)			// 4GB per hour

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
/************************ Static Function *******************************/
/*******************************************************************/

#define _____PVR_ACTION_SCENARIO_FUNCTIONS_____
// Ư�� Action�� �װų� ��� ���⼭ ó���Ѵ�, ��� �ó������� ���⿡ ��� ������ �Ѵ�.
// ����� �ó������� �°� Ư�� PVR Action�� ���ְų� ������ ���, ���޾� Ȥ�� �� ���Ŀ� �ؾ� �� �ൿ�� �����ϱ�
// ������ �ڵ� ������ recursive �ϰ� �� �� �ۿ� ����.

STATIC XmgrPvr_Action_t *xmgr_tsrtrackbase_getPvrActionForPb (XmgrPvr_Context_t *pstContext, HUINT32 ulActionId)
{
	HUINT32				 ulIndex;
	XmgrPvr_Action_t		*pstAction;

	if (pstContext == NULL)						{ return NULL; }

	// ���� ����ϴ� PVR Action �� Action ID�� ���� Action�� ������ �ϴ� �װ��� ���ְ� �����Ѵ�.
	for (ulIndex = 0; ulIndex < PVRMGR_ACTION_PB_NUM; ulIndex++)
	{
		pstAction = &(pstContext->astPbActItem[ulIndex]);
		if ((pstAction->eState != ePvrActState_None) && (pstAction->ulActionId == ulActionId))
		{
			xmgr_pvr_DestroyPvrAction(pstAction);

			return pstAction;
		}
	}

	// ��� �ִ� Action�� �⵵�� �Ѵ�.
	for (ulIndex = 0; ulIndex < PVRMGR_ACTION_PB_NUM; ulIndex++)
	{
		pstAction = &(pstContext->astPbActItem[ulIndex]);
		if (pstAction->eState == ePvrActState_None)
		{
			return pstAction;
		}
	}

	return NULL;
}

#define _____TSR_CONTROL_FUNCTIONS_____
STATIC HERROR xmgr_tsrtrackbase_tryTsrNormal (HUINT32 ulTsrStartDelayTime)
{
	HUINT32						 ulViewId, ulActionId;
	MgrAction_Type_e			 eActType;
	Handle_t					 hMasterSvcHandle = HANDLE_NULL, hSuppleSvcHandle = HANDLE_NULL;
	HERROR						 hErr;
	MgrLiveView_SessionType_t	 eLiveSessionType = eLiveViewSession_NORMAL;

#if defined(CONFIG_MW_MM_DELAYED_REC)
	// Copy �� Tsr �ٽ� �������� �ʵ���....
	{
		XmgrPvr_Context_t *pstContext = xmgr_pvr_GetContext();
		if(MGR_RECORDER_IsDelayedRecCopyInProgress()==TRUE)
		{
			 HxLOG_Debug("Copying .... Don't Try TSR Recording ... \n");
			if(pstContext)
			{
				pstContext->bTsrToBeStarted = TRUE;
				return FALSE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			pstContext->bTsrToBeStarted = TRUE;
		}
	}
#endif

	// View ID�� ������ TSR�� �ؾ� �ϴ� ä���� ��� TSR�� ���۽�Ű���� �Ѵ�.
	for (ulViewId = 0; ulViewId < NUM_VIEW_ACTION; ulViewId++)
	{
		hErr = MGR_ACTION_GetActionIdByViewId(ulViewId, &ulActionId);
		if (ERR_OK != hErr)					{ continue; }

		eActType = MGR_ACTION_GetType(ulActionId);
		if (eActType == eViewType_WTV)
		{
			hErr  = MGR_LIVE_GetSessionType(ulActionId, &eLiveSessionType);
			hErr |= MGR_LIVE_GetServiceHandle(ulActionId, eLiveViewPlaySvc_MASTER, &hMasterSvcHandle);
			hErr |= MGR_LIVE_GetServiceHandle(ulActionId, eLiveViewPlaySvc_ACTUAL, &hSuppleSvcHandle);
			if (hErr == ERR_OK)
			{
				HBOOL			 bAutoTrack = FALSE;

				hErr = MGR_PVR_GetTsrAutoTracking(ulViewId, &bAutoTrack);
				if ((ERR_OK == hErr) && (TRUE == bAutoTrack))
				{
					XmgrPvr_Action_t	*pstPvrAction = xmgr_pvr_FindPvrActionByRelatedViewId(ePvrActType_TSR_Normal, ulViewId);

					if (NULL != pstPvrAction)
					{
						if ((pstPvrAction->hMasterSvcHandle == hMasterSvcHandle) &&
							(pstPvrAction->hCurrSvcHandle == hSuppleSvcHandle))
						{
							// Already the same service is TSRed.
							continue;
						}

						xmgr_pvr_DestroyPvrAction(pstPvrAction);
					}

					pstPvrAction = xmgr_pvr_FindPvrActionByServiceHandle(ePvrActType_REC,eLiveSessionType,hMasterSvcHandle,hSuppleSvcHandle);
					if (pstPvrAction != NULL)
					{
						HxLOG_Debug("Currently recording is going on hMasterSvc(%d) hSupple(%d) \n",hMasterSvcHandle,hSuppleSvcHandle);
						// ��ȭ ���� ä�ο��� tsr �� ���� �ʴ´�.
						continue;
					}

					hErr = MGR_PVR_StartTsrRecording(ulViewId, hMasterSvcHandle, hSuppleSvcHandle, eLiveSessionType, eTsrType_LiveTracing, ulTsrStartDelayTime);
					if (ERR_OK != hErr)
					{
						HxLOG_Error("xmgr_pvr_StartTsrRecording_Base return :0x%X\n\n", hErr);
					}
				}
			}
		}
	}

	return ERR_OK;
}


#define _____INTERNAL_MSG_FUNCTIONS_____
STATIC BUS_Result_t xmgr_tsrtrackbase_msgBusCreate (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_tsrtrackbase_msgBusDestroy (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{

	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_tsrtrackbase_msgBusTimer (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_tsrtrackbase_msgWtvSvcChanged (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32				 		 ulViewId = 0;
	HUINT32						 ulActionId = SVC_PIPE_GetActionId(hAction);
	XmgrPvr_Context_t			*pstContext;
	HERROR						 hErr;

	pstContext = xmgr_pvr_GetContext();
	if (pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL\n");
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("ActionId(%d) not alligned to the viewID...\n", ulActionId);
		return MESSAGE_PASS;
	}

	// ������ ���� �� �ܿ��� ����� �����ؾ� �Ѵ�.
	xmgr_tsrtrackbase_tryTsrNormal(PVR_MGR_DELAY_TSRREC_TIMEOUT);

	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_tsrtrackbase_msgTsrSetLiveTsr (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulActionId = 0xFFFF;
	HUINT32					 ulViewId = (HUINT32)p1;
	HBOOL					 bTsrAutoTrack = (HBOOL)p2;
	XmgrPvr_Action_t		*pstAction;
	XmgrPvr_Context_t		*pstContext = xmgr_pvr_GetContext();
	HERROR					 hErr;

	if (bTsrAutoTrack == pstContext->bTsrAutoTrack[ulViewId])
	{
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_GetActionIdByViewId(ulViewId, &ulActionId);
	if ((ERR_OK != hErr) || (ulActionId >= eActionId_MAX))
	{
		HxLOG_Error("No Action ID for the view ID(%d).\n", ulActionId);
		return MESSAGE_PASS;
	}

	switch (bTsrAutoTrack)
	{
	case TRUE:
		pstContext->bTsrAutoTrack[ulViewId] = TRUE;
		pstAction = xmgr_pvr_FindPvrActionByRelatedActionId(ePvrActType_TSR_Normal, ulActionId);
		if (NULL == pstAction)
		{
			xmgr_tsrtrackbase_tryTsrNormal(100);
		}

		break;

	case FALSE:
		pstContext->bTsrAutoTrack[ulViewId] = FALSE;
		pstAction = xmgr_pvr_FindPvrActionByRelatedActionId(ePvrActType_TSR_Normal, ulActionId);
		if (NULL != pstAction)
		{
			xmgr_pvr_DestroyPvrAction(pstAction);
		}

		break;

	default:
		break;
	}

	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_tsrtrackbase_msgAmActionSetup (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrAction_Type_e 	 eActType = (MgrAction_Type_e)p1;
	XmgrPvr_Context_t	*pstContext;
	HERROR				 hErr;

	pstContext = xmgr_pvr_GetContext();
	if (pstContext == NULL)					{ return MESSAGE_PASS; }

	switch (eActType)
	{
	// ���� �ó������� �߰��Ǹ� ���⿡ �ֵ��� �Ѵ�.
	case eViewType_WTV:
		// ���� TSR�� �������ε�, �װ��� WatchTV�� �浹�Ѵٸ� �����ؾ� �Ѵ�.
		// ���� �׼��� ��� ���� ������ ��������� Live Tuner�� �� �ٲ�� ������ ���´�.
		{
			HUINT32			 ulViewId = 0xFF;
			HUINT32 		 ulActionId = SVC_PIPE_GetActionId(hAction);
			Handle_t		 hSvcHandle = (Handle_t)p2;

			hErr = MGR_ACTION_GetViewIdByActionId(ulActionId, &ulViewId);
			if ((ERR_OK == hErr) && (ulViewId < NUM_VIEW_ACTION))
			{
				XmgrPvr_Action_t	*pstAction;

				pstAction = xmgr_pvr_FindPvrActionByRelatedViewId(ePvrActType_TSR_Normal, ulViewId);
				if ((NULL != pstAction) && (pstAction->hCurrSvcHandle != hSvcHandle))
				{
					xmgr_pvr_DestroyPvrAction(pstAction);
				}
			}
		}

		break;

	default:
		break;
	}

	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_tsrtrackbase_msgAmActionTaken (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t	xmgr_tsrtrackbase_msgAmActionReleased (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					 ulActionId = SVC_PIPE_GetActionId (hAction);
	MgrAction_Type_e		 eActType = (MgrAction_Type_e)p1;
	XmgrPvr_Action_t		*pstAction, *pstRelAction;

	switch (eActType)
	{
	case eViewType_WTV:
		pstAction = xmgr_pvr_FindPvrActionByRelatedActionId(ePvrActType_TSR_Normal, ulActionId);
		if (pstAction != NULL)
		{
			HBOOL		 bRemainTsr = FALSE;

			// WTV -> TSR PB ��ȯ �̿��� ��쿡 WTV Action�� Stop�ϸ� �ݵ�� TSR�� ���� ���ֵ��� �Ѵ�.
			pstRelAction = xmgr_pvr_FindPvrActionByActionId(ePvrActType_PB, ulActionId);
			if (pstRelAction != NULL)
			{
				if (pstRelAction->eType == ePvrActType_PB_Tsr && pstRelAction->eState == ePvrActState_SettingUp)
				{
					bRemainTsr = TRUE;
				}
			}

			if (bRemainTsr != TRUE)
			{
				xmgr_pvr_DestroyPvrAction(pstAction);
			}
		}

		break;

	case eRecType_REC:
		// Record�� ���߰� TSR�� ������ �� �ִ�.
		xmgr_tsrtrackbase_tryTsrNormal(100);
		break;

	default:
		break;
	}

	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_tsrtrackbase_msgAmActionTypeChanged (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_tsrtrackbase_evtStoragePvrEnabled (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL					 bTsr = FALSE;
	HUINT32 				 ulPartIdx = 0, ulPartNum = 0, ulPvrIdx = 0xffff;
	SvcFs_DevIdx_e			 eDevIdx = (SvcFs_DevIdx_e)p1;
	SvcFs_DevInfo_t 		 stDevInfo;
	SvcFs_PartitionInfo_t	*pstPartInfo, *pstPartArray = NULL;
	HERROR					 hErr;

	hErr = SVC_FS_GetDeviceInfo (eDevIdx, &stDevInfo);
	if (hErr == ERR_OK)
	{
		// HDD�� �����Ѵ�, ������ Partition�� �ִ°�, Format�� �Ǿ� �ִ°� Ȯ���Ѵ�.
		hErr = SVC_FS_GetPartitionInfoArray (eDevIdx, &ulPartNum, &pstPartArray);
		if (hErr == ERR_OK && ulPartNum > 0 && pstPartArray != NULL)
		{
			for (ulPartIdx = 0; ulPartIdx < ulPartNum; ulPartIdx++)
			{
				pstPartInfo = &(pstPartArray[ulPartIdx]);

				if (pstPartInfo->bAvailable == TRUE && pstPartInfo->bMounted == TRUE && pstPartInfo->bPvrFlag == TRUE)
				{
					ulPvrIdx = SVC_FS_GetDefaultPvrIdx();
					if (ulPvrIdx != 0xffff && ulPvrIdx == pstPartInfo->ulPvrIdx)
					{
						bTsr = 	TRUE;
					}

					ulPvrIdx = pstPartInfo->ulPvrIdx;
					break;
				}
			}
		}
	}
	if (pstPartArray != NULL)
	{
		SVC_FS_FreePartitionInfoArray (pstPartArray);
	}

	// TSR�� �����Ѵ�. �� ������ TSR Buffer�� �������´�.
	if (TRUE == bTsr)
	{
		HCHAR			 szTsrPath[128]={0,};
		HCHAR			 szDelPath[128+8]={0,};
		HCHAR			 szRootPath[128]={0,};

		hErr = SVC_FS_GetPvrPath (ulPvrIdx, eSVC_FS_UsageType_Root, szRootPath);
		if( SVC_FS_UseTsrOnlyDevice() == TRUE )
		{
			hErr |= SVC_FS_GetTsrOnlyPath(szTsrPath);
		}
		else
		{
			hErr |= SVC_FS_GetPvrPath (ulPvrIdx, eSVC_FS_UsageType_Tsr, szTsrPath);
		}
		HxSTD_snprintf(szDelPath, 128+8,"%s/.recycleBin/%d_del",HLIB_PATH_Normalize(szRootPath), (HLIB_STD_GetSystemTick()%1000));
		if (ERR_OK == hErr)
		{
			/*
			 *  DMS���� TSR->DEL Move������ DMS���� db���� �Ͽ� ��ġ �ʴ� ������ �о� �� ��츦 �����ϱ� ���� TSR ���� ���θ� ����.
			 */
			HxLOG_Debug("Move Tsr Folder:[%s]->[%s] \n",szTsrPath,szDelPath);
			(void)HLIB_DIR_Move(szTsrPath, szDelPath);

			if (HLIB_DIR_IsExist(szTsrPath) == FALSE)
			{
				HLIB_DIR_Make((const HCHAR *)szTsrPath);
				HxLOG_Debug("Make Tsr Folder:[%s]\n",szTsrPath);
			}

#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
			if (HLIB_EXTRA_ChownWithParam(szTsrPath, UID_obama, GID_pvr) != ERR_OK)
			{
				HxLOG_Error("Fail to chown path = %s \n", szTsrPath);
			}

			if (HLIB_EXTRA_Chmod(szTsrPath, 0770) != ERR_OK)
			{
				HxLOG_Error("	Fail to chmod path = %s \n", szTsrPath);
			}
#endif
		}

		xmgr_tsrtrackbase_tryTsrNormal(100);
	}

	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

#if defined(CONFIG_MW_MM_DELAYED_REC)
STATIC BUS_Result_t xmgr_tsrtrackbase_evtRecDelayedRecordStatusChange (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrPvr_Context_t		*pstContext = xmgr_pvr_GetContext();

	if(pstContext)
	{
		if (pstContext->bTsrToBeStarted == TRUE)
		{
			HxLOG_Debug("Copy Completed .... Try TSR Recording ... \n");
			pstContext->bTsrToBeStarted = FALSE;
			xmgr_tsrtrackbase_tryTsrNormal(0);
		}
	}

	//--------------------------------------------------------
	NOT_USED_PROC_ARGUMENT(hAction, p1, p2, p3);
	return MESSAGE_PASS;
}
#endif


static HBOOL	xmgr_tsrtrackbase_ComparePath (const DxRecListData_t *rec, void *path)
{
	return HLIB_STD_StrStartWith(rec->stMeta.szPath, path);
}

STATIC BUS_Result_t xmgr_tsrtrackbase_evtSiTdtTot (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrPvr_Context_t	*pstContext;

	pstContext = xmgr_pvr_GetContext();
	if (pstContext == NULL)					{ return MESSAGE_PASS; }

	if (FALSE == pstContext->bTimeAvailable)
	{
		pstContext->bTimeAvailable = TRUE;
		xmgr_tsrtrackbase_tryTsrNormal(100);
	}

	return MESSAGE_PASS;
}

STATIC BUS_Result_t xproc_tsrtrackbase (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		 eGwmResult = ERR_BUS_NO_ERROR;

	switch (message)
	{
	case eMEVT_BUS_CREATE :
		HxLOG_Print("MESSAGE : eMEVT_BUS_CREATE [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_tsrtrackbase_msgBusCreate (hAction, p1, p2, p3);
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Print("MESSAGE : eMEVT_BUS_DESTROY [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_tsrtrackbase_msgBusDestroy (hAction, p1, p2, p3);
		break;

	case eMEVT_BUS_TIMER:
		HxLOG_Print("MESSAGE : eMEVT_BUS_TIMER [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_tsrtrackbase_msgBusTimer (hAction, p1, p2, p3);
		break;

/*
	Service Change : TSR ����
*/
	case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
		HxLOG_Print("MESSAGE : eMEVT_LIVE_NOTIFY_SVC_CHANGED [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_tsrtrackbase_msgWtvSvcChanged (hAction, p1, p2, p3);
		break;


	case eMEVT_PVR_TSR_SET_LIVE_TSR:
		HxLOG_Print("MESSAGE : eMEVT_PVR_TSR_SET_LIVE_TSR [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_tsrtrackbase_msgTsrSetLiveTsr (hAction, p1, p2, p3);
		break;

/*
	Action Message
*/
	case eMEVT_ACTION_SETUP_REQUESTED:
		HxLOG_Print("MESSAGE : eMEVT_ACTION_SETUP_REQUESTED [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_tsrtrackbase_msgAmActionSetup (hAction, p1, p2, p3);
		break;

	case eMEVT_ACTION_NOTIFY_TAKEN:
		HxLOG_Print("MESSAGE : eMEVT_ACTION_NOTIFY_TAKEN [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_tsrtrackbase_msgAmActionTaken (hAction, p1, p2, p3);
		break;

	case eMEVT_ACTION_NOTIFY_RELEASED:
		HxLOG_Print("MESSAGE : eMEVT_ACTION_NOTIFY_RELEASED [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_tsrtrackbase_msgAmActionReleased (hAction, p1, p2, p3);
		break;

	case eMEVT_ACTION_NOTIFY_TYPE_CHANGED:
		HxLOG_Print("MESSAGE : eMEVT_ACTION_NOTIFY_TYPE_CHANGED [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_tsrtrackbase_msgAmActionTypeChanged (hAction, p1, p2, p3);
		break;

/*
	System Power Off / Reset
*/
	case eMEVT_STORAGE_NOTIFY_PVR_ENABLED:
		HxLOG_Print("MESSAGE : eMEVT_STORAGE_NOTIFY_PVR_ENABLED [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_tsrtrackbase_evtStoragePvrEnabled (hAction, p1, p2, p3);
		break;

#if defined(CONFIG_MW_MM_DELAYED_REC)
	// Normal Action Manager�� �����Ǿ� �ִ� PVR �׸�
	case eSEVT_REC_DELAYED_RECORD_STATUS_CHANGE:
		HxLOG_Print("MESSAGE : eSEVT_REC_DELAYED_RECORD_STATUS_CHANGE [0x%08x] (%d,%d,%d)\n", hAction, p1, p2, p3);
		eGwmResult = xmgr_tsrtrackbase_evtRecDelayedRecordStatusChange (hAction, p1, p2, p3);
		break;
#endif

/*
	�ð��� Enable�Ǿ�� ���� ���
*/
	case eSEVT_SI_TDT:
	case eSEVT_SI_TOT:
		eGwmResult = xmgr_tsrtrackbase_evtSiTdtTot (hAction, p1, p2, p3);
		break;

	default:
		eGwmResult = ERR_BUS_NO_ERROR;
		break;
	}

	return (eGwmResult == MESSAGE_BREAK) ? MESSAGE_BREAK : MESSAGE_PASS;
}

#define _____MEMBER_FUNCTIONS_____
HERROR xmgr_pvr_CreateAutoTrackProc_Base (void)
{
	BUS_Result_t 	eRes;

	// PVR Manager ���ٴ� Priority�� ���ƾ� �Ѵ�.
	eRes = BUS_MGR_Create ("xproc_tsrtrackbase", APP_MONITORING_MGR_PRIORITY, xproc_tsrtrackbase, 0, 0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR xmgr_pvr_DestroyAutoTrackProc_Base (void)
{
	BUS_Result_t 	eRes;

	eRes = BUS_MGR_Destroy(xproc_tsrtrackbase);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

