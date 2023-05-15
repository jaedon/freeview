/****************************************************************************
*
* File Name   : xmgr_coldboot_me.c
*
* Description : AP cold boot processing application. No cold boot operation.
*
*
* Revision History	:
*
* Date			Modification				Name
* -----------	-------------------------------------	------------------
*
****************************************************************************/


/* ======================================================================== */
/* Include. 																*/
/* ======================================================================== */
/** @brief global header file  */
#include <octo_common.h>

#include <hapi.h>
#include <db_param.h>

#include <svc_output.h>
#include <svc_si.h>
#include <svc_sys.h>
#include <svc_pipe.h>

#include <bus.h>

#include <mgr_common.h>
#include <mgr_time.h>
#include <mgr_coldboot.h>
#include <mgr_swup.h>
#include <mgr_action.h>

#include <xmgr_swup.h>

#include "../local_include/_xmgr_coldboot.h"
/* ======================================================================== */
/* Private Macros. 															*/
/* ======================================================================== */

#define SET_NEXT_STEP(x)			(x++)
#define SKIP_STEP(x,step)			(x = step + 1)

/* ======================================================================== */
/* Global/Extern Variables.													*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.															*/
/* ======================================================================== */
typedef enum
{
	eColdBoot_Process_Init = 0,
	eColdBoot_Process_Ota_Check,
	eColdBoot_Process_Time_Update,
	eColdBoot_Process_End
} ColdBoot_Process_t;

typedef struct tagColdBootContents
{
	Handle_t			hActHandle;
	HUINT32			ulProcessStep;
} ColdBoot_Contents_t;


/* ======================================================================== */
/* Private Constants. 														*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 														*/
/* ======================================================================== */
STATIC ColdBoot_Contents_t		s_stColdBoot_Contents;
STATIC Handle_t 				s_hSwUpHandle;

/* ======================================================================== */
/* Private Function prototypes. 											*/
/* ======================================================================== */
#define _____Static_Function_ProtoType_____
STATIC INLINE ColdBoot_Contents_t *xmgr_coldboot_GetContents_MeSat (void);
STATIC HERROR xmgr_coldboot_Init_MeSat (ColdBoot_Contents_t *pstContents);
STATIC HERROR xmgr_coldboot_CheckOta_MeSat (ColdBoot_Contents_t *pstContents);
STATIC HERROR xmgr_coldboot_UpdateTime_MeSat (ColdBoot_Contents_t *pstContents);
STATIC HERROR xmgr_coldboot_CheckBat_For_MyHD(ColdBoot_Contents_t *pstContents);
STATIC BUS_Result_t xmgr_coldboot_Create_MeSat (ColdBoot_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC HERROR xmgr_coldboot_Finish_MeSat (ColdBoot_Contents_t *pstContents);
STATIC BUS_Result_t xmgr_coldboot_DestroyOtaAction_MeSat (ColdBoot_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t xmgr_coldboot_GoToNextProcess_MeSat (ColdBoot_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t xmgr_coldboot_ClockSetupDone_MeSat(ColdBoot_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
/* ======================================================================== */
/* Functions. 																*/
/* ======================================================================== */
#define _____Static_Function_____

STATIC INLINE ColdBoot_Contents_t *xmgr_coldboot_GetContents_MeSat (void)
{
	return &s_stColdBoot_Contents;
}

STATIC HERROR xmgr_coldboot_Init_MeSat (ColdBoot_Contents_t *pstContents)
{
	WakeUpReason_t	 eWakeupReason;

	SVC_SYS_GetWakeUpReason (&eWakeupReason);

	if (eWakeupReason == eWAKEUP_BY_TIMER)
	{
		BUS_MGR_Destroy(0);
		BUS_PostMessage(NULL, eMEVT_COLDBOOT_NOTIFY_DONE, 0, 0, 0, 0);
		return ERR_OK;
	}

	s_hSwUpHandle = (Handle_t)HANDLE_NULL;
	SET_NEXT_STEP (pstContents->ulProcessStep);
	BUS_PostMessage(NULL, eMEVT_COLDBOOT_NEXTPROCESS, 0, 0, 0, 0);

	return ERR_OK;
}

STATIC HERROR xmgr_coldboot_CheckOta_MeSat (ColdBoot_Contents_t *pstContents)
{
	HERROR			hErr;
	HCHAR			*pszVersion = NULL;

#if defined (CONFIG_MW_SYS_7SEG)
	SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_VIEW_ACTION, "SWUP", eDxPANEL_ALIGN_LEFT);
#else
	SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_VIEW_ACTION, "Software Update", eDxPANEL_ALIGN_LEFT);
#endif

	hErr = MGR_SWUP_Start();
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("MGR_SWUP_Start error \n");
		return ERR_FAIL;
	}

	pszVersion = XMGR_SWUP_GetSwUpdateVersionString_Humax(FALSE, FALSE, NULL, NULL);		// 시작시마다, 이전 check 정보를 삭제해준다.
	DB_PARAM_SetItem(eDB_PARAM_ITEM_SWUPDATE_VERSION, (HUINT32)pszVersion, 0);

	hErr = MGR_SWUP_SetSpec(eDxSwUpdate_SPEC_HUMAX);
	hErr |= MGR_SWUP_SetDefaultTp ();
	if(hErr == ERR_OK)
	{
		s_hSwUpHandle = MGR_SWUP_Create(NULL, eDxBAT_TYPE_COLDBOOT);
		if(s_hSwUpHandle != HANDLE_NULL)
		{
			MGR_SWUP_InitState(s_hSwUpHandle, eDxSWUPDATE_SOURCE_Ip,0,0);
			MGR_SWUP_PostMessage(s_hSwUpHandle, eMEVT_SWUP_CONNECT_START, HANDLE_NULL, 0, 0, 0);
			return ERR_OK;
		}
	}

	SET_NEXT_STEP (pstContents->ulProcessStep);
	BUS_PostMessage(NULL, eMEVT_COLDBOOT_NEXTPROCESS, 0, 0, 0, 0);

	return ERR_OK;
}

STATIC HERROR xmgr_coldboot_UpdateTime_MeSat (ColdBoot_Contents_t *pstContents)
{
	HUINT32 ulUnixTime;
	HERROR	hErr;

#if defined (CONFIG_MW_SYS_7SEG)
	SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_VIEW_ACTION, "TIME", eDxPANEL_ALIGN_LEFT);
#else
	SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_VIEW_ACTION, "Updating Time", eDxPANEL_ALIGN_LEFT);
#endif


	hErr = VK_CLOCK_GetTime(&ulUnixTime) ;
	if ( VK_OK != hErr	|| HLIB_DATETIME_IsValidUnixTime(ulUnixTime) != ERR_OK )
	{
		MGR_TIME_ClockRecoverStart();
	}
	else
	{
		SET_NEXT_STEP(pstContents->ulProcessStep);
		BUS_PostMessage(NULL, eMEVT_COLDBOOT_NEXTPROCESS, 0, 0, 0, 0);
	}

	return ERR_OK;
}

STATIC HERROR xmgr_coldboot_CheckBat_For_MyHD(ColdBoot_Contents_t *pstContents)
{
	HERROR				hErr = ERR_FAIL;
	BUS_Callback_t		pfnSrchProc = NULL;
	HINT32	nValue;
	HBOOL				bBatUpdateOnOff = FALSE;

#if defined (CONFIG_MW_SYS_7SEG)
	SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_VIEW_ACTION, "MYHD", eDxPANEL_ALIGN_LEFT);
#else
	SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_VIEW_ACTION, "Check Changed Channel", eDxPANEL_ALIGN_LEFT);
#endif

	if (HAPI_GetRegistryInt("setbatupdateonoff", &nValue) == ERR_OK)
	{
		bBatUpdateOnOff = (nValue ? TRUE : FALSE);
	}

	if (bBatUpdateOnOff == FALSE)
	{
		HxLOG_Debug("\nGet Bat UPDATE Flag is OFF!!!! \n");
		SET_NEXT_STEP(pstContents->ulProcessStep);
		BUS_PostMessage(NULL, eMEVT_COLDBOOT_NEXTPROCESS, 0, 0, 0, 0);
		return ERR_OK;
	}

	pfnSrchProc = MGR_SEARCH_GetColdBootSearchAction();
	if (pfnSrchProc == NULL)
	{
		HxLOG_Debug("\nGet ColdBoot Search Action is NULL!!!! \n");
		SET_NEXT_STEP(pstContents->ulProcessStep);
		BUS_PostMessage(NULL, eMEVT_COLDBOOT_NEXTPROCESS, 0, 0, 0, 0);
		return ERR_OK;
	}

	hErr = MGR_SEARCH_StartColdBootSearchAction(0);
	if (hErr == ERR_FAIL)
	{
		HxLOG_Error("\nFailed Start ColdBoot SearchAction!!\n");
		SET_NEXT_STEP(pstContents->ulProcessStep);
		BUS_PostMessage(NULL, eMEVT_COLDBOOT_NEXTPROCESS, 0, 0, 0, 0);
	}

	return ERR_OK;
}


STATIC HERROR xmgr_coldboot_Finish_MeSat (ColdBoot_Contents_t *pstContents)
{
	BUS_MGR_Destroy(NULL);
	BUS_PostMessage(NULL, eMEVT_COLDBOOT_NOTIFY_DONE, 0, 0, 0, 0);
	return ERR_OK;
}

STATIC BUS_Result_t xmgr_coldboot_Create_MeSat (ColdBoot_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32				 ulActionId;

	HxSTD_memset (pstContents, 0, sizeof(ColdBoot_Contents_t));

	ulActionId = MGR_ACTION_GetMainActionId();
	pstContents->hActHandle = SVC_PIPE_GetActionHandle (ulActionId);
	pstContents->ulProcessStep = eColdBoot_Process_Init;

	BUS_PostMessage(NULL, eMEVT_COLDBOOT_NEXTPROCESS, 0, 0, 0, 0);

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_coldboot_DestroyOtaAction_MeSat (ColdBoot_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			hErr;
	SvcSi_SwupSignal_t	 	stOtaInfo;
	DbSvc_TsInfo_t	 	stTsInfo;

	if (MGR_SWUPINFO_IsOtaFound (FALSE) == TRUE)
	{
		hErr = MGR_SWUPINFO_GetSearchedInfo (&stTsInfo, sizeof(SvcSi_SwupSignal_t), (void *)&stOtaInfo);
		if (hErr == ERR_OK && stOtaInfo.ucSwupType == SvcSi_SWUP_TYPE_FORCED)
		{
			// TODO: For Forced OTA
			HxLOG_Print("This is Forced OTA..............\n");
		}
	}

	SET_NEXT_STEP (pstContents->ulProcessStep);
	BUS_PostMessage(NULL, eMEVT_COLDBOOT_NEXTPROCESS, 0, 0, 0, 0);

	return MESSAGE_BREAK;
}


// MSG_APP_SHUTDOWN_NEXTPROCESS
STATIC BUS_Result_t xmgr_coldboot_GoToNextProcess_MeSat (ColdBoot_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR	 hErr = ERR_OK;

	switch ( pstContents->ulProcessStep )
	{
		case eColdBoot_Process_Init:
			HxLOG_Print("[%s][%04d]Processing eColdBoot_Process_Init..\n",  __FUNCTION__, __LINE__);
			hErr = xmgr_coldboot_Init_MeSat (pstContents);
			break;

		case eColdBoot_Process_Ota_Check:
			HxLOG_Print("[%s][%04d]Processing eColdBoot_Process_Ota_Check..\n",  __FUNCTION__, __LINE__);
			hErr = xmgr_coldboot_CheckOta_MeSat (pstContents);
			break;

		case eColdBoot_Process_Time_Update:
			HxLOG_Print("[%s][%04d]Processing eColdBoot_Process_Time_Update..\n",  __FUNCTION__, __LINE__);
			hErr = xmgr_coldboot_UpdateTime_MeSat(pstContents);
			break;
#if defined(CONFIG_SUPPORT_MYHD_BAT)
		case eColdBoot_Process_Check_Bat:

			HxLOG_Print("[%s][%04d]Processing eColdBoot_Process_Check_Bat...\n", __FUNCTION__,__LINE__);
			hErr = xmgr_coldboot_CheckBat_For_MyHD(pstContents);
			break;
#endif
		case eColdBoot_Process_End:
			HxLOG_Print("[%s][%04d]Processing eColdBoot_Process_End..\n",  __FUNCTION__, __LINE__);
			hErr = xmgr_coldboot_Finish_MeSat (pstContents);
			break;

		default:
			HxLOG_Print("[%s][%04d] Unkown Step %d\n", __FUNCTION__, __LINE__,pstContents->ulProcessStep);
			break;
	}

	if ( hErr == ERR_FAIL )
	{
		SET_NEXT_STEP(pstContents->ulProcessStep);
		BUS_PostMessage(NULL, eMEVT_COLDBOOT_NEXTPROCESS, 0, 0, 0, 0);
	}

	return MESSAGE_BREAK;

}

// MSG_APP_CLOCK_SETUP_DONE
STATIC BUS_Result_t xmgr_coldboot_ClockSetupDone_MeSat(ColdBoot_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	SET_NEXT_STEP(pstContents->ulProcessStep);
	BUS_PostMessage(NULL, eMEVT_COLDBOOT_NEXTPROCESS, 0, 0, 0, 0);

	return MESSAGE_BREAK;
}

#define _____Module_Function_____

BUS_Result_t xproc_coldboot_MeSat(HINT32 message, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t 			eRes = MESSAGE_PASS;
	ColdBoot_Contents_t		*pstContents;

	pstContents = xmgr_coldboot_GetContents_MeSat();

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Print("\n\n\t############ MiddleEast Cold Boot Processing Start !!! ##############\n");
			eRes = xmgr_coldboot_Create_MeSat(pstContents, hAct, p1, p2, p3);
			break;

		case eMEVT_ACTION_NOTIFY_RESULT:
			{
				Mgr_Event_e eMgrEvent = p1;

				HxLOG_Print("[%s][%04d] eMEVT_ACTION_NOTIFY_RESULT -- ", __FUNCTION__, __LINE__);
				if (eMgrEvent == eMGR_EVENT_SWUP_DETECT_FINISH)
				{
					HxLOG_Print("eMGR_EVENT_SWUP_DETECT_FINISH \n");
					eRes = xmgr_coldboot_DestroyOtaAction_MeSat(pstContents, hAct, p1, p2, p3);
				}
			}
			break;

		case eMEVT_COLDBOOT_NEXTPROCESS:
			HxLOG_Print("[%s][%04d] eMEVT_COLDBOOT_NEXTPROCESS \n",  __FUNCTION__, __LINE__);
			eRes = xmgr_coldboot_GoToNextProcess_MeSat (pstContents, hAct, p1, p2, p3);
			break;

		case eMEVT_TIME_CLOCKRECOVER_SETUP_DONE:
			HxLOG_Print("[%s][%04d] eMEVT_TIME_CLOCKRECOVER_SETUP_DONE\n", __FUNCTION__, __LINE__);
			eRes = xmgr_coldboot_ClockSetupDone_MeSat (pstContents, hAct, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY:
			HxLOG_Print("[%s][%04d] eMEVT_BUS_DESTROY \n", __FUNCTION__, __LINE__);
			xproc_coldboot_Base(message, hAct, p1, p2, p3);
			return MESSAGE_BREAK;

		default:
			//	Warning : PAMA Message processed in coldboot base module
			eRes = xproc_coldboot_Base(message, hAct, p1, p2, p3);
			break;
	}

	return BUS_ProcessMessageDefault(message, hAct, p1, p2, p3);
}

HERROR	xmgr_coldboot_Start_MeSat(void)
{
	BUS_Result_t	eRes;

	eRes = BUS_MGR_Create("xmgr_coldboot_MeSat", APP_ACTION_MGR_PRIORITY, xproc_coldboot_MeSat, 0, 0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;

}
/* End of File. ---------------------------------------------------------- */


