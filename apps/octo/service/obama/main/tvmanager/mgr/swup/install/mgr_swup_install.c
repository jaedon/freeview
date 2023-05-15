/**
	@file     mgr_swup_install.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2008/09/05		Satellite part is divided		jinh\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <db_param.h>
#include <db_svc.h>
#include <svc_si.h>
#include <svc_swup.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_swup.h>

#include <dlib_swupdate.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



#define INSTALL_TIMER_LOCKING_ID					0x2101
#define INSTALL_TIMER_LOCKING_TIME					(4*1000)			// 4sec

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eINSTALL_Init = 0,
	eINSTALL_Writing,
	eINSTALL_Error,
	eINSTALL_Finish,
} INSTALL_State_e;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

typedef struct tagInstallContents_t
{
	HUINT32						 ulState;

	Handle_t					 hAction;				// Action handle

	Handle_t					 hSwUpdate;
	HUINT32						 ulSWUPTag;

	HUINT8						 szSourceFile[512];			// Source HDF File Path and File Name

	SvcSwUpdate_EmergencyUpdateInfo_t stEmergencyUpdateInfo;	// Emergency Update Info

} INSTALL_Contents_t;

static INSTALL_Contents_t	s_stInstallContents;

static INSTALL_Contents_t *mgr_install_GetContents (void)
{
	return &s_stInstallContents;
}

static HERROR mgr_install_InitContents (INSTALL_Contents_t *pstContents, Handle_t hAction, Handle_t hSWUpdate, HUINT8 *pucSourceFile, SvcSwUpdate_EmergencyUpdateInfo_t *pstSourceInfo)
{
	// 1. Clear all the contents.
	HxSTD_memset (pstContents, 0, sizeof(INSTALL_Contents_t));

	// 2. Action Handle and Action ID
	pstContents->hAction	= hAction;

	pstContents->hSwUpdate	= hSWUpdate;
	if (pucSourceFile)
	{
		VK_strncpy(pstContents->szSourceFile, pucSourceFile, 512-1);
	}
	pstContents->szSourceFile[512-1] = '\0';

	// 3. Set Emergency Update Info
	if (pstSourceInfo)
	{
		HxSTD_memcpy(&pstContents->stEmergencyUpdateInfo, pstSourceInfo, sizeof(SvcSwUpdate_EmergencyUpdateInfo_t));
	}
	// TODO: Get More Info

	// 3. Set State
	pstContents->ulState	= eINSTALL_Init;

	return ERR_BUS_NO_ERROR;
}

//eMEVT_BUS_CREATE
static HERROR mgr_install_MsgGwmCreate (INSTALL_Contents_t *pstContents, Handle_t hAction, Handle_t hSWUpdate, HUINT8 *pucSourceFile, SvcSwUpdate_EmergencyUpdateInfo_t *pstSourceInfo)
{
	HERROR		 hErr;

	// Create시 해 주어야 하는 Initialize 등이 실패할 경우에는 이 Action 자체가 살아 있어서는 안된다.
	// 곧바로 Destroy 해 주도록 한다.

	// 1. Initialize the contents
	hErr = mgr_install_InitContents (pstContents, hAction, hSWUpdate, pucSourceFile, pstSourceInfo);
	if (hErr == ERR_BUS_NO_ERROR)
	{
		// 2. Start SW Image Write to RAWFS area
		hErr = SVC_SWUPDATE_InstallWriteImage(eDxSWUPDATE_SOURCE_File, pucSourceFile);
		if (hErr == ERR_OK)
		{
			return ERR_BUS_NO_ERROR;
		}
	}

	MGR_SWUP_PostMessage(pstContents->ulSWUPTag, eMEVT_SWUP_NOTIFY_INSTALL_FAIL, hAction, 0, 0, 0);
	BUS_MGR_Destroy (0);
	return hErr;
}

// eMEVT_BUS_TIMER:
static HERROR mgr_install_MsgGwmTimer (INSTALL_Contents_t *pstContents, HINT32 nTimerId)
{
	HERROR		hRetValue;

	hRetValue = ERR_BUS_NO_ERROR;

	switch (nTimerId)
	{
#if 0
	case SWUP_TIMER_LOCKING_ID:
		BUS_KillTimer (nTimerId);

		hRetValue = MESSAGE_BREAK;
		break;

	case SWUP_TIMER_SI_ID:
		BUS_KillTimer (nTimerId);

		hRetValue = MESSAGE_BREAK;
		break;
#endif

	default:
		break;
	}

	return hRetValue;
}


static HERROR mgr_install_DestroyContents (INSTALL_Contents_t *pstContents)
{
	// TODO: Do Something : Free Memory or Destroy Link

	// pstContents->

	return ERR_BUS_NO_ERROR;
}


// eMEVT_BUS_DESTROY:
static HERROR mgr_install_MsgGwmDestroyed (INSTALL_Contents_t *pstContents)
{
	// Destroy the contents
	mgr_install_DestroyContents (pstContents);

	return MESSAGE_BREAK;
}

static HERROR mgr_install_MsgGwmQuitStandby  (INSTALL_Contents_t *pstContents)
{
	BUS_MGR_Destroy (NULL);

	return ERR_OK;
}


STATIC BUS_Result_t proc_INSTALL(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	static HBOOL			 s_bFirstStart = TRUE;
	INSTALL_Contents_t		*pstContents;
	HERROR					 hErr;

	pstContents = mgr_install_GetContents();
	if (s_bFirstStart == TRUE)
	{
		s_bFirstStart = FALSE;
		HxSTD_memset (pstContents, 0, sizeof(INSTALL_Contents_t));
	}

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		{
			Handle_t						hSWUpdate = (Handle_t)p1;
			HUINT8 							*pucSourceFile = (HUINT8 *)p2;
			SvcSwUpdate_EmergencyUpdateInfo_t	*pstSourceInfo = (SvcSwUpdate_EmergencyUpdateInfo_t *)p3;

			if (((hSWUpdate != HANDLE_NULL) && (hSWUpdate)) && (NULL != pucSourceFile))
			{
				BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "proc_INSTALL");
				hErr = mgr_install_MsgGwmCreate (pstContents, hAction, hSWUpdate, pucSourceFile, pstSourceInfo);
				if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
			}
		}
		break;

	case eMEVT_BUS_TIMER:
		hErr = mgr_install_MsgGwmTimer (pstContents, p1);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_DESTROY:
		hErr = SVC_SWUPDATE_InstallCancel();
		hErr = mgr_install_MsgGwmDestroyed (pstContents);
		//if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_GO_OPERATION :
		hErr = mgr_install_MsgGwmQuitStandby (pstContents);
		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_FINISH, HANDLE_NULL, (HINT32)pstContents->hSwUpdate, 0, 0);
		break;

	case eMEVT_BUS_READY_SHUTDOWN :
		hErr = mgr_install_MsgGwmQuitStandby (pstContents);
		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_FINISH, HANDLE_NULL, (HINT32)pstContents->hSwUpdate, 0, 0);
		//if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eSEVT_SWUP_INSTALL_WRITE_IMAGE_SUCCESS:
		HxLOG_Print("\t[eSEVT_SWUP_INSTALL_WRITE_IMAGE_SUCCESS] ");
		{
			SvcSwUpdate_DetectResult_t	*pstDetectResult = NULL;

			pstDetectResult = (SvcSwUpdate_DetectResult_t*)HLIB_STD_MemCalloc(sizeof(SvcSwUpdate_DetectResult_t));
			if(pstDetectResult == NULL)
			{
				HxLOG_Error("\n\n\n    eSEVT_SWUP_INSTALL_WRITE_IMAGE_SUCCESS, but Alloc fail!!!\n\n\n");
				break;
			}
			MGR_SWUP_GetDetectInfo(pstContents->hSwUpdate, pstDetectResult);

			hErr = SVC_SWUPDATE_InstallFinalize(pstDetectResult->ulDataVersion);

			if (hErr == ERR_OK)
			{
				if ( pstDetectResult->eSource == eDxSWUPDATE_SOURCE_Rf)
				{
					hErr = SVC_SWUPDATE_InstallWriteEmergencySettingAfterWriteSucc(pstDetectResult->data.stRfInfo.eDeliType);
				}
				else if ( pstDetectResult->eSource == eDxSWUPDATE_SOURCE_Ip)
				{
					hErr = SVC_SWUPDATE_InstallWriteEmergencySettingAfterWriteSucc(eDxDELIVERY_TYPE_MULTICAST);
				}
				else
				{
					SVC_SWUPDATE_InstallClearEmergencySettingFlag(0xFF);
					hErr = ERR_OK;
				}
			}
			if (ERR_OK == hErr)
			{
				HxLOG_Debug("\n\n\n    SUCCESSED EMERGENCY COMPLETE!!!\n\n\n");

				// Notify Event To Application
				hErr = MGR_SWUP_PostMessage(pstContents->hSwUpdate, eMEVT_SWUP_INSTALL_SUCCESS, hAction, 0, 0, 0);
			}
			else
			{
				HxLOG_Error("\n\n\n    ERROR EMERGENCY COMPLETE!!!\n\n\n");

				// Notify Event To Application
				hErr = MGR_SWUP_PostMessage(pstContents->hSwUpdate, eMEVT_SWUP_NOTIFY_INSTALL_FAIL, hAction, 0, 0, 0);
			}
			
			if (pstDetectResult)
			{
				HLIB_STD_MemFree(pstDetectResult);
				pstDetectResult = NULL;
			}
		}
		break;

	case eSEVT_SWUP_INSTALL_WRITE_IMAGE_FAIL:
		HxLOG_Print("\t[eSEVT_SWUP_INSTALL_WRITE_IMAGE_FAIL] ");

		hErr = SVC_SWUPDATE_InstallCancel();

		MGR_SWUP_PostMessage(pstContents->hSwUpdate, eMEVT_SWUP_NOTIFY_INSTALL_FAIL, hAction, 0, 0, 0);
		// Notify Event To Application
//		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_INSTALL_FAIL, HANDLE_NULL, (HINT32)pstContents->hSwUpdate, 0, 0);
		break;

	case eSEVT_SWUP_INSTALL_PROGRESS:
		HxLOG_Print("\t[eSEVT_SWUP_INSTALL_PROGRESS] ");
		MGR_SWUP_PostMessage(pstContents->hSwUpdate, eMEVT_SWUP_NOTIFY_INSTALL_PROGRESS, hAction, p1, p2, 0);
		// Notify Event To Application
//		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_INSTALL_PROGRESS, HANDLE_NULL, (HINT32)pstContents->hSwUpdate, p1, p2);
		break;

	default:
		break;
	}

	HxLOG_Print("proc_INSTALL() out\n");

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

BUS_Callback_t mgr_swup_InstallGetProc(void)
{
	return proc_INSTALL;
}

HERROR MGR_SWUP_InstallStart(Handle_t hSWUpdate, HUINT8 *pszSourceFile, SvcSwUpdate_EmergencyUpdateInfo_t *pstSourceInfo)
{
	BUS_Result_t	eRes;

	eRes = BUS_MGR_Create("proc_INSTALL", APP_ACTION_PRIORITY, proc_INSTALL, 0, (HINT32)hSWUpdate, (HINT32)pszSourceFile, (HINT32)pstSourceInfo);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR MGR_SWUP_InstallStop(void)
{
	BUS_Result_t	eRes;

	eRes = BUS_MGR_Destroy(proc_INSTALL);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

/* End of File. ---------------------------------------------------------- */

