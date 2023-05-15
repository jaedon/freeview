/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <isosvc.h>
#include <db_svc.h>
#include <db_param.h>

#include <svc_si.h>
#include <svc_ch.h>
#include <svc_fs.h>
#include <svc_rec.h>
#include <svc_cas.h>
#if defined(CONFIG_MW_MM_REC_WITH_EPG)
#include <svc_epg.h>
#endif
#include <svc_sys.h>
#include <svc_ringbuf.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_storage.h>

#include <mgr_pg.h>
#include <mgr_live.h>
#include <mgr_recorder.h>
#include <mgr_rscmap.h>
#include <mgr_cas.h>
#include <_xmgr_recorder.h>
#include "./local_include/_xmgr_recorder_base.h"


#include <sapi.h>

#include <otl.h>

#include <uapi.h>
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	HUINT8			bSeriesRec;
	HUINT8			szSeriesId[EPG_TVA_CRID_TOTAL_LEN];
	HUINT32			ulEpisodeNum;
	HUINT16			ulSeason;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	DxPVR_TvaCrid_t stCrid[DxMAX_CRID_DATA_NUM];
#endif
}xmgrRecSeries_t;

typedef enum
{
	eXmgrRecSeriesLock_Init 	= 0,	// no series record file exists
	eXmgrRecSeriesLock_Some 	= 1,	// series record file exists and some series records are locked, rest are unlocked
	eXmgrRecSeriesLock_True		= 2,	// series record file exists and all series records are locked
	eXmgrRecSeriesLock_False	= 3,	// series record file exists and all series records are unlocked
} xmgrRecSeriesLock_e;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC xmgrRec_Context_t		s_astMgrRecContext[NUM_MAX_REC_ACTION];
STATIC HUINT32					s_ulTsrDurationTime	=	DEFAULT_DURATION_TSR_RECORD;

/*******************************************************************/
/************************ Static Function *******************************/
/*******************************************************************/
#if defined(CONFIG_APCORE_IP_EPG)
STATIC	HERROR xmgr_rec_FindEpisodeAndSeasonFromIPEPG(DxSchedule_t	*pstSchedule, SvcMeta_Record_t *pstMetaInfo);
#endif

STATIC HERROR xmgr_rec_InitAllContext (void)
{
	HUINT32					 ulCount;

	HxSTD_MemSet (s_astMgrRecContext, 0, sizeof(xmgrRec_Context_t) * NUM_MAX_REC_ACTION);

	for (ulCount = 0; ulCount < NUM_MAX_REC_ACTION; ulCount++)
	{
		s_astMgrRecContext[ulCount].hAction		= HANDLE_NULL;
		s_astMgrRecContext[ulCount].bPMTValid	= FALSE;
	}

	return ERR_OK;
}

STATIC xmgrRec_Context_t *xmgr_rec_GetContextByActionHandle (Handle_t hAction)
{
	HINT32 i=0;

	for( i=0 ; i< NUM_MAX_REC_ACTION ; i++ )
	{
		if (s_astMgrRecContext[i].hAction == hAction)
			return &(s_astMgrRecContext[i]);
	}

	return NULL;
}

STATIC xmgrRec_Context_t * xmgr_rec_GetContextByContentId (HUINT32 ulContentId)
{
	HINT32 i=0;
	for( i=0 ; i<NUM_MAX_REC_ACTION ; i++ )
	{
		if( s_astMgrRecContext[i].ulContentId == ulContentId )
			return &s_astMgrRecContext[i];
	}

	return NULL;
}

STATIC HERROR xmgr_rec_GetRecInfoFromCtx (xmgrRec_Context_t* pstContext, MgrRec_RecInfo_t *pstInfo)
{
	if (NULL == pstInfo || NULL == pstContext)
		return ERR_FAIL;

	pstInfo->hAction		= pstContext->hAction;
	pstInfo->ulContentId	= pstContext->ulContentId;
	pstInfo->eRecState		= pstContext->eRecState;
	pstInfo->nEventId		= pstContext->usBaseEventId;

	pstInfo->eRecDst		= pstContext->stSetup.eRecDst;
	pstInfo->eRecType		= pstContext->stSetup.eRecType;
	pstInfo->hSvc			= pstContext->stSetup.hCurrSvc;
	pstInfo->ulSlot 		= pstContext->stSetup.ulSlot;
	pstInfo->hMasterSvc 	= pstContext->stSetup.hMasterSvc;
	pstInfo->ulLiveActionId	= pstContext->stSetup.ulLiveActionId;
	pstInfo->ulPvrIdx		= pstContext->stSetup.ulPvrIdx;

	pstInfo->ulStartTime	= pstContext->stMetaInfo.ulStartTime;
	pstInfo->ulEndTime		= pstContext->stMetaInfo.ulEndTime;
	pstInfo->ulRecTime		= pstContext->stMetaInfo.ulRecTime;
	pstInfo->ulLastTime		= pstContext->stMetaInfo.ulLastTime;

	return ERR_OK;
}

STATIC HUINT8 *xmgr_rec_GetPath (HUINT32 ulPvrIdx)
{
	HERROR	hErr = ERR_FAIL;
	static HUINT8	strPath[FS_MAX_FILE_NAME_LEN];

	strPath[0] = '\0';
	hErr = SVC_FS_GetPvrPath (ulPvrIdx, eSVC_FS_UsageType_Record, strPath);
	if (hErr ==ERR_FAIL)
	{
		HxLOG_Error("Can not Get Record Path for pvr(%d) \n",ulPvrIdx);
		return NULL;
	}

	return strPath;
}

#define _________META_FUNC__________
STATIC HERROR xmgr_rec_AddMeta (SvcRec_Setup_t *pstSvcRecSetup, HUINT32 *pulContentId, xmgrRecSeries_t *pstSeriesRecInfo)
{
	HERROR hErr = ERR_FAIL;
	SvcMeta_Record_t stMetaInfo;

	if ((NULL == pstSvcRecSetup) || (NULL == pulContentId))
		return ERR_FAIL;

	HxSTD_MemSet (&stMetaInfo, 0, sizeof(SvcMeta_Record_t));

	stMetaInfo.ulStartTime = pstSvcRecSetup->ulStartTime;
	HxSTD_MemCopy(&stMetaInfo.aucPath,		pstSvcRecSetup->unOutArg.stFile.aucPath, FS_MAX_FILE_NAME_LEN);
	HxSTD_MemCopy(&stMetaInfo.aucFileName,	pstSvcRecSetup->unOutArg.stFile.aucFileName, FS_MAX_FILE_NAME_LEN);

#if defined(CONFIG_APCORE_IP_EPG)
	/* Series */
	if (NULL != pstSeriesRecInfo)
	{
		if (TRUE == pstSeriesRecInfo->bSeriesRec)
		{
			HxSTD_MemSet (stMetaInfo.szSeriesId, 0, EPG_TVA_CRID_TOTAL_LEN);
			HxSTD_MemCopy(stMetaInfo.szSeriesId, pstSeriesRecInfo->szSeriesId, EPG_TVA_CRID_TOTAL_LEN);
			HxLOG_Debug ("[%s:%d]\t[ulSeason\t:\t%d]\n",	__FUNCTION__, __LINE__, pstSeriesRecInfo->ulSeason);
			HxLOG_Debug ("[%s:%d]\t[ulEpisodeNum\t:\t%d]\n",	__FUNCTION__, __LINE__, pstSeriesRecInfo->ulEpisodeNum);

			stMetaInfo.bSeriesRec	= pstSeriesRecInfo->bSeriesRec;
			stMetaInfo.ulSeason		= pstSeriesRecInfo->ulSeason;
			stMetaInfo.ulEpisodeNum	= pstSeriesRecInfo->ulEpisodeNum;
		}

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		HxSTD_MemCopy (&(stMetaInfo.stCrid), &(pstSeriesRecInfo->stCrid), sizeof(DxPVR_TvaCrid_t)* DxMAX_CRID_DATA_NUM);
#endif
	}
#endif /*CONFIG_APCORE_IP_EPG*/


	if (TRUE == pstSvcRecSetup->bEncryption)
	{
		stMetaInfo.u1cEncryption = TRUE;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug ("------------<XMGR_REC:AddMeta>------------\n");
	HxLOG_Debug ("[%s:%d]\t[hSvc\t\t:\t0x%x]\n", 			__FUNCTION__, __LINE__, pstSvcRecSetup->hSvc);
	HxLOG_Debug ("[%s:%d]\t[ulStartTime\t:\t%d]\n", 	__FUNCTION__, __LINE__, stMetaInfo.ulStartTime);
	HxLOG_Debug ("[%s:%d]\t[aucPath\t:\t%s]\n",			__FUNCTION__, __LINE__, stMetaInfo.aucPath);
	HxLOG_Debug ("[%s:%d]\t[aucFileName\t:\t%s]\n",		__FUNCTION__, __LINE__, stMetaInfo.aucFileName);
	HxLOG_Debug ("[%s:%d]\t[ext:szSeriesId\t:\t%s]\n",	__FUNCTION__, __LINE__, stMetaInfo.szSeriesId);
	HxLOG_Debug ("[%s:%d]\t[ext:bSeriesRec\t:\t%d]\n",	__FUNCTION__, __LINE__, stMetaInfo.bSeriesRec);
	HxLOG_Debug ("[%s:%d]\t[ext:ulSeason\t:\t%d]\n",	__FUNCTION__, __LINE__, stMetaInfo.ulSeason);
	HxLOG_Debug ("[%s:%d]\t[ext:ulEpiNum\t:\t%d]\n",	__FUNCTION__, __LINE__, stMetaInfo.ulEpisodeNum);
#endif

	hErr = SVC_META_AddContentsInfo (pstSvcRecSetup->hSvc, &stMetaInfo, pulContentId);
	if( hErr != ERR_OK )
	{
		HxLOG_Error ("SVC_META_AddContentsInfo. hErr = 0x%x\n", hErr);
		return hErr;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Debug ("[%s:%d]\t[ContentId\t:\t%d]\n",	__FUNCTION__, __LINE__, *pulContentId);
	HxLOG_Debug ("-------------<AddMeta:XMGR_REC>------------\n");
#endif
	return hErr;
}

STATIC HERROR xmgr_rec_GetEventInfometa(HUINT32 ulContentId, SvcMeta_Event_t * pstEvent)
{
	HERROR hErr = ERR_FAIL;

	if( NULL == pstEvent )
		return ERR_FAIL;

	HxSTD_MemSet(pstEvent, 0, sizeof(SvcMeta_Event_t));
	hErr = SVC_META_GetEventInfo(ulContentId, pstEvent);

	if( ERR_OK != hErr )
		HxLOG_Error("can't call func'SVC_META_GetEventInfo( contentID(%d) )'\n", ulContentId);

	return hErr;
}

STATIC HERROR xmgr_rec_SetRecInfo(xmgrRec_Context_t *pstContext, SvcMeta_Record_t * pstRec)
{
	if( (NULL == pstContext) || (NULL == pstRec) )
		return ERR_FAIL;

	HxSTD_MemCopy( &pstContext->stMetaInfo, pstRec, sizeof(SvcMeta_Record_t) );

	return SVC_META_SetRecordInfo(pstContext->ulContentId, pstRec);
}

#define _________LOCAL_FUNC__________
STATIC HCHAR		*xmgr_rec_GetStateStr (MgrRec_State_e eState)
{
#if defined(CONFIG_DEBUG)
	switch(eState)
	{
		ENUM_TO_STR(eMgrRecState_IDLE);
		ENUM_TO_STR(eMgrRecState_TUNE);
		ENUM_TO_STR(eMgrRecState_NO_SIGNAL);
		ENUM_TO_STR(eMgrRecState_WAIT);
		ENUM_TO_STR(eMgrRecState_BLOCKED);
		ENUM_TO_STR(eMgrRecState_RECORDING);
		ENUM_TO_STR(eMgrRecState_STOPPED);
		default: return "UNKNOWN_STATE";
	}
#else
	return NULL;
#endif
}
STATIC void			xmgr_rec_CheckState(xmgrRec_Context_t *pstContext)
{
#if defined(CONFIG_DEBUG)
	// only debug logs. no action.
	// if (not expected result)
	//  {
	//	log(error)
	//  }
	if (pstContext->eRecState > eMgrRecState_WAIT && pstContext->eWaitFlag != 0)
	{
		HxLOG_Info("Not wait state but wait flag is still set. state(%d) wait(%x) \n",pstContext->eRecState,pstContext->eWaitFlag);
	}
	if (pstContext->eRecState > eMgrRecState_BLOCKED && pstContext->ePauseFlag != 0)
	{
		HxLOG_Info("Not blocked state but pause flag is still set. state(%d) pause(%x) \n",pstContext->eRecState,pstContext->ePauseFlag);
	}

	if ((pstContext->eWaitFlag & eRecWait_DRM) == eRecWait_None)
	{
		if ((pstContext->bCasDrmValid != TRUE) && (pstContext->bCasDrmNone != FALSE))
		{
			HxLOG_Info("Drm wait is cleared but drm is not valid \n");
		}
	}
#endif
}

STATIC BUS_Result_t xmgr_rec_PreCasProtoProc (xmgrRec_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	typedef struct
	{
		MgrRec_CasType_e	eRecCasType;
		BUS_Result_t (*pfProtoProc) (xmgrRec_Context_t *, HINT32, Handle_t, HINT32, HINT32, HINT32);
	}xmgrRecCasList_t;

	/*
	 * TODO : MODEL에서 지원하는 CAS의 PROC 선 처리를 함
	 */

	STATIC xmgrRecCasList_t s_aCasList[] =
	{
#if defined(CONFIG_MW_CI_PLUS)
		{eMgrRec_CasType_CiPlus,	xmgr_rec_CiPlusProto_Base},
#endif
#if defined(CONFIG_MW_CAS_NAGRA)
	#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
		{eMgrRec_CasType_NagraSES,	NULL},
	#else
		{eMgrRec_CasType_Nagra, 	xmgr_rec_NagraProto_Base},
	#endif
#endif
#if defined(CONFIG_MW_CAS_IRDETO_PVR) || defined(CONFIG_IRDETO_HMX_PVR)
		{eMgrRec_CasType_Irdeto,	xmgr_rec_IrdetoProtoProc_Base},
#endif
#if defined(CONFIG_MW_CAS_CONAX)
		{eMgrRec_CasType_Conax,		xmgr_rec_ConaxProto_Base},
#endif
#if defined(CONFIG_MW_CAS_VIACCESS)
		{eMgrRec_CasType_Viaccess,	NULL},
#endif
#if defined(CONFIG_MW_CAS_VERIMATRIX)
		{eMgrRec_CasType_Verimatrix,	xmgr_rec_VerimatrixProto_Base},
#endif
		{-1, NULL}
	};

	BUS_Result_t eBusResult = ERR_BUS_NO_ERROR;
	HERROR hErr;
	HUINT32			ulCasTypes=0;
	HINT32			i = 0;
	HINT32			nArraySize;

	nArraySize = sizeof(s_aCasList) / sizeof(xmgrRecCasList_t);
	if(0 == nArraySize)
	{
		return eBusResult;
	}

	hErr = MGR_RECORDER_GetRecCasTypes (&ulCasTypes);
	if((ERR_OK != hErr) || (eMgrRec_CasType_None == (MgrRec_CasType_e)ulCasTypes))
	{
		return eBusResult;
	}

	for(i = 0; i < nArraySize; i++)
	{
		/* in case of no cas, exit */
		if (-1 == s_aCasList[i].eRecCasType)
			break;

		/* There is not a matched cas. Check next */
		if(0 == (ulCasTypes & s_aCasList[i].eRecCasType))
			continue;

		/* in case of null proc, check next */
		if(NULL == s_aCasList[i].pfProtoProc)
			continue;

		eBusResult = s_aCasList[i].pfProtoProc (pstContext, message, hAction, p1, p2, p3);
		if (ERR_BUS_NO_ERROR == eBusResult)
		{
			return eBusResult;
		}
	}

	return eBusResult;
}

#if (defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HDNEO)) && defined(CONFIG_MW_SYS_COLOR_LED)
STATIC HERROR xmgr_rec_SetRecLedOnOff(HBOOL bOn)
{
	HERROR	hErr = ERR_OK;

	if(bOn)
	{
		hErr = SVC_SYS_SetLedOnOff(eDxLED_ID_BLUE_COLOR, FALSE);	// BLUE LED OFF
		hErr = SVC_SYS_SetLedOnOff(eDxLED_ID_REC, TRUE);			// RED LED ON
	}
	else
	{
		hErr = SVC_SYS_SetLedOnOff(eDxLED_ID_REC, FALSE);			// RED LED OFF
		hErr = SVC_SYS_SetLedOnOff(eDxLED_ID_BLUE_COLOR, TRUE);		// BLUE LED ON
	}

	return hErr;
}
#endif

STATIC HERROR xmgr_rec_LogicFrontpanel(MgrRec_RecordType_e eRecType, HBOOL bIsOnOff)
{
	HERROR	hErr = ERR_OK;

	if( eMgrRecType_TSR == eRecType )
		return hErr;

	if ( MGR_ACTION_GetState() != eAmState_ACTIVE_STANDBY )
	{
		HBOOL bPrintLed = TRUE;
#if (defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HDNEO)) && defined(CONFIG_MW_SYS_COLOR_LED)
		// icordpro인경우 color led (blue, red)로 만 display된다.
		if( bIsOnOff == TRUE )
		{
			hErr = xmgr_rec_SetRecLedOnOff(TRUE);
		}
		else
		{
			/* 본 Rec이 Stop 하더라도 다른 Rec의 Panel 유지 */
			if( MGR_ACTION_GetRecordCount() <= 1 )
			{
				hErr = xmgr_rec_SetRecLedOnOff(FALSE);
			}
		}
		return hErr;
#elif defined(CONFIG_PROD_YSR2000)
		if(bIsOnOff)
		{
			hErr = SVC_SYS_SetRGBLedByColor(eDxLED_ID_RGB_REC, eDxRGBLED_COLOR_RED, FALSE);
			if(hErr == ERR_OK)
			{
				(void)SVC_SYS_SetRGBLedDimmingLevel(eDxLED_ID_RGB_REC, eDxRGBLED_DIMMLEVEL_SOLID);
			}

			// Set Dimm to REC Indicator after 10 sec
			BUS_KillTimer(RECORDING_RGBLED_DIMM_TIMER_ID);
			BUS_SetTimer(RECORDING_RGBLED_DIMM_TIMER_ID, RECORDING_RGBLED_DIMM_10SEC);
		}
		else
		{
			/* 본 Rec이 Stop 하더라도 다른 Rec의 Panel 유지 */
			if( MGR_ACTION_GetRecordCount() <= 1 )
			{
				hErr = SVC_SYS_SetRGBLedOff(eDxLED_ID_RGB_REC);
			}
		}

#else

		if( TRUE != bIsOnOff )
		{
			/* 본 Rec이 Stop 하더라도 다른 Rec의 Panel 유지 */
			if( MGR_ACTION_GetRecordCount() > 1 )
				bPrintLed = FALSE;
			hErr = SVC_SYS_ClearPanelString(ePANEL_STR_PRIORITY_REC_ACTION);
			hErr = BUS_KillTimer(PANEL_STRING_TIMER_ID);
		}
		else
		{
			hErr = SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_REC_ACTION, "", eDxPANEL_ALIGN_CENTER);
			hErr = BUS_SetTimer(PANEL_STRING_TIMER_ID, PANEL_STRING_TIME_OUT);
		}

		if( TRUE == bPrintLed )
		{
#if defined(CONFIG_MW_SYS_LED_DIMMING_CTRL_ON_STBY)
			if (bIsOnOff)
			{
				SVC_SYS_SetPowerLedState(ePOWER_LED_RECORDING);
			}
			else
			{
				SVC_SYS_SetPowerLedState(ePOWER_LED_WATCHING);
			}
#else
			hErr = SVC_SYS_SetLedOnOff(eDxLED_ID_REC, bIsOnOff);
#endif
		}
#endif
	}
	else
	{
#if defined(CONFIG_PROD_YSR2000)
		if(bIsOnOff)
		{
			hErr = SVC_SYS_SetRGBLedByColor(eDxLED_ID_RGB_REC, eDxRGBLED_COLOR_RED, FALSE);
			if(hErr == ERR_OK)
			{
				hErr = SVC_SYS_SetRGBLedDimmingLevel(eDxLED_ID_RGB_REC, eDxRGBLED_DIMMLEVEL_DIMMED);
			}
		}
		else
		{
			/* 본 Rec이 Stop 하더라도 다른 Rec의 Panel 유지 */
			if( MGR_ACTION_GetRecordCount() <= 1 )
			{
				hErr = SVC_SYS_SetRGBLedOff(eDxLED_ID_RGB_REC);
			}
		}
#else

#if defined(CONFIG_MW_SYS_LED_DIMMING_CTRL_ON_STBY)
		if (MGR_ACTION_GetRecordCount() > 0)
		{
			SVC_SYS_SetPowerLedState(ePOWER_LED_RECORDING);
		}
		else
		{
			if (bIsOnOff)
			{
				SVC_SYS_SetPowerLedState(ePOWER_LED_RECORDING);
			}
			else
			{
				SVC_SYS_SetPowerLedState(ePOWER_LED_STANDBY);
			}
		}
#else
		hErr = SVC_SYS_SetLedOnOffOnStandbyMode(eDxLED_ID_REC, bIsOnOff);
#endif

#endif
	}

	return hErr;
}

STATIC HERROR xmgr_rec_PauseRec(xmgrRec_Context_t *pstContext)
{
	HERROR hErr = ERR_FAIL;

	hErr = SVC_REC_Pause(pstContext->hAction, eAsyncMode);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_REC_Pause error. hAction=(0x%x) \n",pstContext->hAction);
		return hErr;
	}

	xmgr_rec_SetState(pstContext,eMgrRecState_BLOCKED);

	return hErr;
}

STATIC HERROR xmgr_rec_ResumeRec(xmgrRec_Context_t *pstContext)
{
	HERROR hErr = ERR_FAIL;

	hErr = SVC_REC_Resume(pstContext->hAction, FALSE);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_REC_Resume error. hAction=(0x%x) \n",pstContext->hAction);
		return hErr;
	}

	xmgr_rec_SetState(pstContext,eMgrRecState_RECORDING);

	return hErr;
}

STATIC HERROR xmgr_rec_StartSI(xmgrRec_Context_t *pstContext)
{
	HERROR hErr = ERR_FAIL;
	SvcSi_RecSpec_t stSiSpec;

	// FULL TS Recording이면 SI를 걸지 않는다.
	if (eMgrRecType_FULLTSREC == pstContext->stSetup.eRecType)
	{
		HxLOG_Warning("Record Type is Full TS Record...\n");
		return ERR_OK;
	}

	hErr = MGR_RECORDER_GetRecSiSpec (&stSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("MGR_RECORDER_GetRecSiSpec err: hAction(0x%08x), hSvc(0x%08x)\n", pstContext->hAction, pstContext->stSetup.hCurrSvc);
		return ERR_FAIL;
	}

	hErr = SVC_SI_StartRec (pstContext->hAction, &stSiSpec, pstContext->stSetup.hCurrSvc, FALSE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_SI_StartRec hErr: hAction(0x%08x), hSvc(0x%08x)\n", pstContext->hAction, pstContext->stSetup.hCurrSvc);
		return ERR_FAIL;
	}

	return ERR_OK;
}

#if defined(CONFIG_MW_MM_REC_WITH_EPG)
STATIC HERROR xmgr_rec_StartEpg (xmgrRec_Context_t *pstContext)
{
	HERROR hErr = ERR_FAIL;

	switch (pstContext->stSetup.eRecType)
	{
	case eMgrRecType_INSTANT:
	case eMgrRecType_TBR:
	case eMgrRecType_EBR:
	case eMgrRecType_SBR:
	case eMgrRecType_DELAYED:
		hErr = SVC_EPG_Start(eEPG_KIND_DVB, eEPG_EVT_KIND_PF_ALL, pstContext->hAction, pstContext->stSetup.hCurrSvc, 0, 0, 0);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_EPG_Start err: hAction(0x%08x), hSvc(0x%08x)\n", pstContext->hAction, pstContext->stSetup.hCurrSvc);
			return ERR_FAIL;
		}

		hErr = SVC_EPG_Start(eEPG_KIND_DVB, eEPG_EVT_KIND_SCH_ALL, pstContext->hAction, pstContext->stSetup.hCurrSvc, 0, 0, 0);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_EPG_Start err: hAction(0x%08x), hSvc(0x%08x)\n", pstContext->hAction, pstContext->stSetup.hCurrSvc);
			return ERR_FAIL;
		}

		break;

	case eMgrRecType_FULLTSREC:
		HxLOG_Warning("Record Type is Full TS Record...\n");
		return ERR_OK;

	default:
		break;
	}

	return ERR_OK;
}

STATIC HERROR xmgr_rec_StopEpg (xmgrRec_Context_t *pstContext)
{
	HERROR hErr = ERR_FAIL;

	switch (pstContext->stSetup.eRecType)
	{
	case eMgrRecType_INSTANT:
	case eMgrRecType_TBR:
	case eMgrRecType_EBR:
	case eMgrRecType_SBR:
	case eMgrRecType_DELAYED:
		hErr = SVC_EPG_Stop(eEPG_KIND_DVB, eEPG_EVT_KIND_ALL, pstContext->hAction);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_EPG_Stop err: hAction(0x%08x)\n", pstContext->hAction);
			return ERR_FAIL;
		}

		break;

	case eMgrRecType_FULLTSREC:
		HxLOG_Warning("Record Type is Full TS Record...\n");
		return ERR_OK;

	default:
		break;
	}

	return ERR_OK;
}
#endif

STATIC HERROR xmgr_rec_StartTune(Handle_t hAction, Handle_t hSvc)
{
	HERROR		hErr		=	ERR_FAIL;
	HUINT32		ulActionId	=	SVC_PIPE_GetActionId(hAction);

	if ((HANDLE_NULL == hAction) || (HANDLE_NULL == hSvc))
	{
		HxLOG_Error ("incorrect arguments - hAction(%x), hSvc(%x) \n", hAction, hSvc);
		return ERR_FAIL;
	}

	/* tune */
	{
		DxTuneParam_t		stTuningInfo;
		SvcRsc_Info_t		stRscInfo;
		DbSvc_TsInfo_t		stTsInfo;

		HxSTD_MemSet(&stTsInfo, 0, sizeof(DbSvc_TsInfo_t));
		HxSTD_MemSet(&stRscInfo, 0, sizeof(SvcRsc_Info_t));
		HxSTD_MemSet(&stTuningInfo, 0, sizeof(DxTuneParam_t));

		hErr = DB_SVC_GetTsInfoByServiceHandle (hSvc, &stTsInfo);
		if( ERR_OK != hErr )
		{
			HxLOG_Error("Critical Error!!!\n");
			return hErr;
		}

		/* DB에서 tuning paramter 가져옴. */
		hErr = OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stTuningInfo);

		HxLOG_Info("deliveryType=%x \n", stTuningInfo.eDeliType);

		hErr = MGR_RSC_GetResourceInfo(ulActionId, &stRscInfo);
		if( ERR_OK != hErr )
		{
			HxLOG_Error("Critical Error!!!\n");
			return hErr;
		}

		hErr = SVC_CH_StartTune(hAction, &stRscInfo, stTsInfo.usUniqueId, &stTuningInfo);
		if( ERR_OK != hErr )
		{
			HxLOG_Error("Critical Error!!!\n");
			return hErr;
		}
	}

	return hErr;
}

STATIC HERROR xmgr_rec_GetServiceInfoForDescramble (HUINT32 ulContentId, HUINT32 ulSrcContentId, SvcRec_Setup_t *pstSvcRecSetup)
{
	if (NULL == pstSvcRecSetup)
		return ERR_FAIL;

	{
		HERROR hErr = ERR_FAIL;
		Handle_t hPBAction = HANDLE_NULL;
		SvcMeta_Service_t stPvrSvcInfo;

		HxSTD_MemSet (&stPvrSvcInfo, 0, sizeof(SvcMeta_Service_t) );
		hErr = SVC_META_GetServiceInfo (ulSrcContentId, &stPvrSvcInfo);
		if( hErr != ERR_OK )
		{
			HxLOG_Error ("SVC_META_GetServiceInfo hErr, hErr:0x%X, ulContentId:0x%X\n", hErr, ulSrcContentId);
			return hErr;
		}

		hErr = SVC_META_SetServiceInfo (ulContentId, &stPvrSvcInfo, TRUE);
		if( hErr != ERR_OK )
		{
			HxLOG_Error ("SVC_META_SetServiceInfo hErr, hErr:0x%X, ulContentId:0x%X\n",hErr, ulContentId);
			return hErr;
		}

		hErr = SVC_META_GetPlaybackActionByContentId (ulSrcContentId, &hPBAction);
		if ((ERR_OK != hErr) || (HANDLE_NULL == hPBAction))
		{
			HxLOG_Error ("SVC_META_GetPlaybackActionByContentId Error.\n");
			return hErr;
		}

		pstSvcRecSetup->hPBAction	= hPBAction;

		pstSvcRecSetup->eSvcType	= stPvrSvcInfo.eSvcType;
		pstSvcRecSetup->eVideoCodec	= stPvrSvcInfo.eVideoCodec;
//		pstSvcRecSetup->usPmtPid	= stPvrSvcInfo.usPmtPid;
//		pstSvcRecSetup->usSvcId		= stPvrSvcInfo.usSvcId;
	}

	return ERR_OK;
}

STATIC SvcRec_ActType_e xmgr_rec_ConvertRecTypeToMgrActionType(MgrRec_RecordType_e 	eRecType)
{
	switch (eRecType)
	{
		case eMgrRecType_TSR:		return eRec_ActType_Tsr_Record;
		case eMgrRecType_DELAYED:	return eRec_ActType_Delayed_Record;
		case eMgrRecType_SCR2DESC:	return eRec_ActType_Descramble_Record;
		case eMgrRecType_FULLTSREC:	return eRec_ActType_FullTs;
		default:					return eRec_ActType_Record;
	}

	return eRec_ActType_NONE;
}

STATIC HERROR xmgr_rec_ConvertUtf8ToByte(HUINT8 *pszEvtName, HUINT8 *pszFileName)
{
	HINT32 i=0;
#define BUFF_LEN 256
	HUINT8	szChangedString[BUFF_LEN];

	HxSTD_MemSet(&szChangedString, 0x00, sizeof(HUINT8)*BUFF_LEN);
	HLIB_STD_StrUtf8NCpy ( szChangedString,
						pszEvtName,
						MAX_REC_FILE_NAME_LEN-1);

	/* delete space in first character */
	for( i=0  ; i<BUFF_LEN ; i++ )
	{
		if( HxSTD_IsWhiteSpace( szChangedString[i] ) )
			continue;
		else break;
	}


	HLIB_STD_StrUtf8NCpy(pszFileName, &szChangedString[i], MAX_REC_FILE_NAME_LEN-1);
	return ERR_OK;
}

STATIC MgrAction_Type_e	xmgr_rec_GetActionType(MgrRec_RecSetup_t *pstRecSetup)
{
	MgrAction_Type_e eActionType = eRecType_REC;

	if (pstRecSetup->eRecType == eMgrRecType_TSR)
	{
		eActionType = eRecType_TSR_REC;
	}
	else if (pstRecSetup->eRecDst == eMgrRecOutput_EXT_TSLINE)
	{
		eActionType = eRecType_EXT_REC;
	}

	return eActionType;
}

STATIC HERROR	xmgr_rec_AddEvtInfoToEvtList(xmgrRec_Context_t *pstContext, HINT32 nEvtId, HUINT8 ucParentalRating, UNIXTIME ulStartTime)
{
	HINT32		i;

	for (i = 0; i < pstContext->nRecvEventNum; i++)
	{
		if (pstContext->astRecvEventInfo[i].nEventId == nEvtId)
			break;
	}

	if (i < pstContext->nRecvEventNum)
	{
		//	update event
		pstContext->astRecvEventInfo[i].nEventId = nEvtId;
		pstContext->astRecvEventInfo[i].usParentalRating = ucParentalRating;
		pstContext->astRecvEventInfo[i].ulStartTime = ulStartTime;
	}
	else if (i == pstContext->nRecvEventNum)
	{
		//	new event found
		i = pstContext->nRecvEventNum;
		pstContext->astRecvEventInfo[i].nEventId = nEvtId;
		pstContext->astRecvEventInfo[i].usParentalRating = ucParentalRating;
		pstContext->astRecvEventInfo[i].ulStartTime = ulStartTime;

		pstContext->nRecvEventNum++;
	}

	//	if data is full, shift all data...
	if (pstContext->nRecvEventNum == PVR_MAX_RECV_EVENT_NUM)
	{
		pstContext->nRecvEventNum--;
	}

	return ERR_OK;
}

STATIC HERROR xmgr_rec_SetupMetaInfo(xmgrRec_Context_t *pstContext, SvcMeta_Record_t * pstMetaInfo)
{
	/* svcInfo.szSvcName 는 DVB-UTF-8 임. 즉 처음에 0x15 로 시작하며 그 이후는 UTF-8 과 동일함.
	 * Event 정보는 DVB 300 468 로 Languge code 가 실려오는 경우 존재
	 * SMS Keyboard 로 입력 받은 이름은 DVB-UTF-8 임. 즉 처음에 0x15 로 시작하며 그 이후는 UTF-8 과 동일함.
	 * OCTO Base Platform 은 Kernel 에서 UTF-8 File 이름을 지원한다.
	 */

	HERROR					hErr	= ERR_OK;
	DxSchedule_t			stSchedule;
	HUINT8					szEvtName[FS_MAX_FILE_NAME_LEN];
	UNIXTIME				ulCurrTime	=	0;
	MgrRec_RecordType_e 	eRecType	=	eMgrRecType_UNKNOWN;
	HUINT32					ulSlot		=	0;
	HUINT32					ulPvrIdx	=	0;
	Handle_t				hCurrSvc	=	HANDLE_NULL;

	if ((NULL == pstContext) || (NULL == pstMetaInfo))
	{
		HxLOG_Error ("incorrect arguments \n");
		return hErr;
	}

	eRecType	=	pstContext->stSetup.eRecType;
	ulSlot		=	pstContext->stSetup.ulSlot;
	ulPvrIdx	=	pstContext->stSetup.ulPvrIdx;
	hCurrSvc	=	pstContext->stSetup.hCurrSvc;

	/* init recinfo */
	HxSTD_MemSet (pstMetaInfo, 0, sizeof(SvcMeta_Record_t));
	HxSTD_MemSet (&stSchedule, 0, sizeof(DxSchedule_t));

	/* set time */
	ulCurrTime = HLIB_STD_GetSystemTime();

	{
		UNIXTIME	ulStartTime		=	0;
		UNIXTIME	ulEndTime		=	0;
		UNIXTIME	ulRecTime		=	0;
		UNIXTIME	ulOrgDuration	=	0;

		ulStartTime	= ulCurrTime;
		ulEndTime	= ulCurrTime;

		switch (eRecType)
		{
			case eMgrRecType_TSR :
			{
				ulEndTime 	= 0xffffffff;
				break;
			}

			case eMgrRecType_INSTANT :
			case eMgrRecType_TBR:
			case eMgrRecType_EBR:
			case eMgrRecType_SBR:
			case eMgrRecType_FULLTSREC:
			{
				hErr = SAPI_GetSchedule (ulSlot, &stSchedule);
				if (hErr == ERR_OK)
				{
					(void)HLIB_DATETIME_ConvertDateTimeToUnixTime (&(stSchedule.stStartTime), &ulStartTime);

					ulEndTime = ulStartTime + stSchedule.ulDuration + stSchedule.uExtInfo.stRec.ulPostOffsetTime;
					ulStartTime -= stSchedule.uExtInfo.stRec.ulPreOffsetTime;
					ulOrgDuration = stSchedule.ulDuration;

#if !defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
					if( ulStartTime < ulCurrTime )
						ulStartTime = ulCurrTime;
#endif
				}
				break;
			}

			case eMgrRecType_DELAYED:
			{
				UNIXTIME ulPrevStartTime = 0;

				hErr = SAPI_GetSchedule (ulSlot, &stSchedule);
				if (ERR_OK != hErr)
				{
					HxLOG_Error ("SAPI_GetSchedule err: ulSlot(%d)\n", ulSlot);
					return ERR_FAIL;
				}

				(void)HLIB_DATETIME_ConvertDateTimeToUnixTime (&(stSchedule.stStartTime), &ulStartTime);
				ulEndTime = ulStartTime + stSchedule.ulDuration;
				ulRecTime = ulCurrTime - ulStartTime - 1;
				ulOrgDuration = ulEndTime - ulStartTime;

				hErr = SVC_META_GetRecordInfo(pstContext->ulContentId, pstMetaInfo);
				if (ERR_OK != hErr)
				{
					HxLOG_Error ("SVC_META_GetRecordInfo. Error\n");
					return hErr;
				}
				ulPrevStartTime = pstMetaInfo->ulStartTime;
				pstContext->ulTsrtoDelayTime = (ulPrevStartTime < ulStartTime) ? (ulStartTime - ulPrevStartTime) : 0;
				break;
			}

			case eMgrRecType_SCR2DESC:
			{
				hErr = SVC_META_GetRecordInfo (pstContext->stSetup.ulSourceContentId, pstMetaInfo);
				if (ERR_OK != hErr)
				{
					HxLOG_Error ("SVC_META_GetRecordInfo. Error\n");
					return hErr;
				}

				ulStartTime 	= pstMetaInfo->ulStartTime;
				ulEndTime		= pstMetaInfo->ulEndTime;
				ulRecTime		= 0;
				ulOrgDuration	= ulEndTime - ulStartTime;
				break;
			}
			default:
				HxLOG_Error ("can't support rectype(%d)'\n", eRecType);
				return hErr;
		}

		pstMetaInfo->ulStartTime		= ulStartTime;
		pstMetaInfo->ulEndTime		= ulEndTime;
		pstMetaInfo->ulRecTime		= ulRecTime;
		pstMetaInfo->ulOrgDuration	= ulOrgDuration;
	}

	/* set event base */
	switch (eRecType)
	{
		case eMgrRecType_TSR:
		case eMgrRecType_EBR:
		case eMgrRecType_SBR: pstMetaInfo->u2cEventBase = TRUE; break;
		default: break;
	}

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
    if(stSchedule.eScheduleSpec == eDxSchedule_Spec_TvAnyTime && (eRecType==eMgrRecType_EBR ||  eRecType==eMgrRecType_SBR))
	{
		DxRSV_TvAnyTimeRecord_t tvAnyTime = stSchedule.uExtInfo.stRec.ex.tva;

		// first crid = crid ( series or programme )
		pstMetaInfo->stCrid[0].ucType = tvAnyTime.ucCRIDType;
		HxSTD_StrNCpy(pstMetaInfo->stCrid[0].szCrid, tvAnyTime.szCRID,HxSTD_StrLen(tvAnyTime.szCRID));
		HxLOG_Debug("tvAnyTime.szCRID [%s]\n",tvAnyTime.szCRID);

		// second crid = programme crid, if the fir crid is not programme crid
		if ( !(HxSTD_StrEmpty(tvAnyTime.szProgCRID)))
		{
			pstMetaInfo->stCrid[1].ucType = CRID_TYPE_PROG;
			HxSTD_StrNCpy(pstMetaInfo->stCrid[1].szCrid, tvAnyTime.szProgCRID,HxSTD_StrLen(tvAnyTime.szProgCRID));
			HxLOG_Debug("tvAnyTime.szProgCRID [%s] \n",tvAnyTime.szProgCRID);
		}

#if defined(CONFIG_OP_TDC)
		// TDC의 경우 Series Title이 Series CRID를 통해 전달 된다.
		if(pstMetaInfo->stCrid[0].ucType == DxCRID_TYPE_SERIES)
		{
			HUINT8 szSeriesTitle[DxRSV_CRID_SERIES_TITLE_LEN] = {0, };
			xmgr_rec_ConvertUtf8ToByte(tvAnyTime.szSeriesTitle, szSeriesTitle);
			HLIB_STD_StrUtf8NCpy(pstMetaInfo->aucSeriesTitle, szSeriesTitle, DxRSV_CRID_SERIES_TITLE_LEN);

		}
#endif

	}

#endif


	/* path */
	{
		HxSTD_MemSet(&szEvtName, 0, MAX_REC_FILE_NAME_LEN);

		switch (eRecType)
		{
			case eMgrRecType_SCR2DESC:
			{
				snprintf(szEvtName, PVR_MAX_TITLE_LEN-1, "NewFile_%s", pstMetaInfo->aucDisplayEvtName);
			}
			break;
			case eMgrRecType_TSR:
			{
				HCHAR szTsrPath[SVC_FS_PVR_DIR_PATH_LEN];
				szTsrPath[0] = '\0';

				if( SVC_FS_UseTsrOnlyDevice() == TRUE )
				{
					hErr = SVC_FS_GetTsrOnlyPath(szTsrPath);
				}
				else
				{
#if defined(CONFIG_PROD_DEVICE_2ND_INTERNAL_HDD)
					hErr = SVC_FS_GetPvrPath (ulPvrIdx, eSVC_FS_UsageType_Tsr, szTsrPath);
#else
					hErr = SVC_FS_GetPvrPath (SVC_FS_GetDefaultPvrIdx(), eSVC_FS_UsageType_Tsr, szTsrPath);	//SVC_FS_DEFAULT_PVR_INDEX 맞다
#endif
				}
				HxLOG_Debug("hErr (%d) TsrPath (%s)\n", hErr, szTsrPath);
				snprintf(pstMetaInfo->aucPath,	FS_MAX_FILE_NAME_LEN, "%s", szTsrPath);
				snprintf(pstMetaInfo->aucFileName,FS_MAX_FILE_NAME_LEN, "%d", pstContext->ulActionId);
				break;
			}
			case eMgrRecType_EBR:
			case eMgrRecType_SBR:
			case eMgrRecType_TBR:
			case eMgrRecType_INSTANT:
			case eMgrRecType_FULLTSREC:
			{
				HUINT8 *pszPath = xmgr_rec_GetPath(ulPvrIdx);

				if(pszPath != NULL)
				{
					/* media info를 위한 write event information*/
					pstContext->usBaseEventId = stSchedule.uExtInfo.stRec.nEvtId;

					HxSTD_PrintToStrN (pstMetaInfo->aucPath, FS_MAX_FILE_NAME_LEN, "%s", pszPath);
					xmgr_rec_ConvertUtf8ToByte (stSchedule.uExtInfo.stRec.szEventName, szEvtName);
					HLIB_STD_StrUtf8NCpy (pstMetaInfo->aucDisplayEvtName, szEvtName, PVR_MAX_TITLE_LEN-1);
				}
				break;
			}

			case eMgrRecType_DELAYED:
			{
				xmgr_rec_ConvertUtf8ToByte (stSchedule.uExtInfo.stRec.szEventName, szEvtName);
				HLIB_STD_StrUtf8NCpy (pstMetaInfo->aucDisplayEvtName, szEvtName, PVR_MAX_TITLE_LEN );

				// Event Name, File Name을 세팅하지 못했을 경우
				if ('\0' == szEvtName[0])
				{
					DbSvc_Info_t	stSvcInfo;
					HxSTD_MemSet(&stSvcInfo, 0, sizeof(DbSvc_Info_t));
					hErr = DB_SVC_GetServiceInfo (hCurrSvc, &stSvcInfo);
					if (ERR_OK == hErr)
					{
						HxSTD_PrintToStrN (pstMetaInfo->aucDisplayEvtName, PVR_MAX_TITLE_LEN - 1, "%s", DbSvc_GetName(&stSvcInfo));
						HxSTD_PrintToStrN (szEvtName, FS_MAX_FILE_NAME_LEN - 1, "%d_%s", DbSvc_GetLcn(&stSvcInfo), DbSvc_GetName(&stSvcInfo));
					}
				}

				{
					HUINT8 *pszPath = (HUINT8 *)xmgr_rec_GetPath(ulPvrIdx);
					if(pszPath != NULL)
						snprintf(pstMetaInfo->aucPath, FS_MAX_FILE_NAME_LEN, "%s", pszPath);
				}

				break;
			}
			default:
				break;
		}
	}

	/* make file */
	{
		HBOOL	bMakeUniqueFileName = TRUE;
		UNIXTIME ulMakeFileTime	=	0;
		HUINT8 *pucSourceName	=	NULL;
		HUINT8 *pucTargetName	=	NULL;

		ulMakeFileTime	=	pstMetaInfo->ulStartTime;
		pucSourceName	=	(HUINT8 *)&szEvtName;
		pucTargetName	=	(HUINT8 *)pstMetaInfo->aucFileName;

		switch (eRecType)
		{
			case eMgrRecType_TSR:
			{
				/* filename of TSR is fixed */
				bMakeUniqueFileName = FALSE;
				break;
			}
			case eMgrRecType_EBR:
			case eMgrRecType_SBR:
			{
				if(eRecType == eMgrRecType_SBR)
					pstMetaInfo->bSeriesRec = TRUE;
#if defined(CONFIG_APCORE_IP_EPG)

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
				if (eRecType == eMgrRecType_SBR)
				{
					if (stSchedule.uExtInfo.stRec.ex.tva.eContentKind & DxRsvTva_ContentKind_SeasonEpisode_Done)
					{
						pstMetaInfo->ulSeason = stSchedule.uExtInfo.stRec.ex.tva.ulSeason;
						pstMetaInfo->ulEpisodeNum = stSchedule.uExtInfo.stRec.ex.tva.ulEpisodeNum;
					}
					else
					{
						HxLOG_Warning("fail to get season & episode info. schedule slot=(%d) \n",stSchedule.ulSlot);
					}
				}
#else
				if(stSchedule.uExtInfo.stRec.ulSeriesId != 0)
				{
					// Episode 정보와 Season정보는 가져옴...
					HxSTD_MemSet(pstMetaInfo->szSeriesId, 0, EPG_TVA_CRID_TOTAL_LEN);
					snprintf(pstMetaInfo->szSeriesId, EPG_TVA_CRID_TOTAL_LEN, "%d", stSchedule.uExtInfo.stRec.ulSeriesId);
					if(xmgr_rec_FindEpisodeAndSeasonFromIPEPG(&stSchedule, pstMetaInfo)!=ERR_OK)
					{
						pstMetaInfo->ulEpisodeNum	= 0;
						pstMetaInfo->ulSeason		= 0;
					}
					HxLOG_Debug("Season: %d, Episode: %d \n", pstMetaInfo->ulSeason, pstMetaInfo->ulEpisodeNum);
				}
#endif

#endif
				break;
			}
			case eMgrRecType_DELAYED:
				{
					/* rec duration time은 recording 시작 시간과 현재 시스템 시간의 차이로 계산하여 tick을 조정하도록 한다.*/
					HINT32	nTick = 0;
					nTick = pstMetaInfo->ulEndTime - ulCurrTime;
					if (nTick > 0)
					{
						BUS_KillTimer(INSTANT_RECORD_END_TIMER_ID);
						BUS_SetTimer(INSTANT_RECORD_END_TIMER_ID, nTick*SECOND);
					}
				}
				break;

			case eMgrRecType_INSTANT:
			case eMgrRecType_TBR:
			case eMgrRecType_SCR2DESC:
			case eMgrRecType_FULLTSREC:
			default:
				break;
		}

		if( TRUE == bMakeUniqueFileName )
		{
			hErr = SVC_META_MakeFileName(ulMakeFileTime, pucSourceName, pucTargetName, HxPATH_MAX);
			if( ERR_OK != hErr)
			{
				HxLOG_Error("can't call func'SVC_PVR_MakeFileName()'\n");
				return hErr;
			}
		}
	}

	/* 가장 긴 duration을 갖는 event를 file 이름으로 해야 한다. */
	/* 녹화 시작하자 마자 Eit changed가 올라오므로 first 이벤트를 저장할 필요는 없다. */
#if defined(CONFIG_MW_MM_AUTO_BOOKMARK)
	pstContext->nCountOfMwPvrEvtList	= 0;
	pstContext->pMwPvrEvtList			= NULL;
#endif

	return hErr;
}

STATIC HERROR xmgr_rec_UtilGetCountryCode(HxCountry_e eCountryId, HCHAR *pszIso3166Code)
{
	const HCHAR *pszIsoCountryCode = NULL;

	if (NULL == pszIso3166Code)
		return ERR_FAIL;

	pszIsoCountryCode = HLIB_COUNTRY_IdTo3166_Alpha3(eCountryId);
	if (pszIsoCountryCode)
	{
		HxSTD_MemCopy(pszIso3166Code, pszIsoCountryCode, 4);
		return ERR_OK;
	}

	return ERR_FAIL;
}

STATIC HERROR xmgr_rec_SetMeta_u1cLocked(xmgrRec_Context_t *pstContext)
{
	HERROR 				hErr = ERR_FAIL;
	HBOOL				bLock = FALSE;
	HBOOL				bFind = FALSE;
	HINT32				nParentalSetup	=	0;
	HUINT8				szIso3166Code[LANG_639_CODE_MAX+1];

	DxDeliveryType_e 	eOrgDeliType	=	eDxSVC_TYPE_All;

	SvcMeta_Record_t	*pstMetaInfo	=	NULL;
	HUINT32				ulContentId		=	0;

	if (NULL == pstContext)
		return ERR_FAIL;

	{
		pstMetaInfo	=	&(pstContext->stMetaInfo);
		ulContentId =	pstContext->ulContentId;
	}

	/* get eOrgDeliType */
	{
		DbSvc_Info_t		stSvcInfo;
		HxSTD_MemSet(&stSvcInfo, 0, sizeof(DbSvc_Info_t));
		hErr = DB_SVC_GetServiceInfo(pstContext->stSetup.hCurrSvc, &stSvcInfo);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("DB_SVC_GetServiceInfo. Error\n");
			return hErr;
		}

		eOrgDeliType = DbSvc_GetDeliType(stSvcInfo);
	}

#if defined(CONFIG_NO_PARENTAL_CONTROL_ON_RADIO_SVC)
	if(eOrgDeliType == eDxSVC_TYPE_RADIO)
	{
		/* ziggo spec 에서는 radio 모드는 pc 를 체크하지 않는다고 한다. */
		return ERR_OK;
	}
#endif

	hErr = SVC_META_GetRecordInfo (ulContentId, pstMetaInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_META_GetRecordInfo. Error\n");
		return hErr;
	}

	if (1 != pstMetaInfo->u1cLocked)
	{
		SvcMeta_Event_t stEvtInfo;
		HBOOL bMetaUpdate_u1cLocked =	FALSE;

		/*get event info*/
		hErr = xmgr_rec_GetEventInfometa(ulContentId, &stEvtInfo);
		if( ERR_OK != hErr )
		{
			HxLOG_Error ("can't get a eventInfo\n" );
			return hErr;
		}

		if( stEvtInfo.ucParentalRatingNum > 0)
		{
			HINT32 			i;
			HxCountry_e		eCountryId;

			// eCountryId = MWC_PARAM_GetCountryId();
			hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32 *)&eCountryId, 0);
			if (ERR_OK != hErr)
			{
				HxLOG_Warning ("DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID) Error!!\n");
			}

			hErr = xmgr_rec_UtilGetCountryCode(eCountryId, szIso3166Code);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("xmgr_rec_UtilGetCountryCode. Error!!\n");
			}

			for (i = 0; (i < stEvtInfo.ucParentalRatingNum) && (i < PVR_PARENTAL_RATING_MAX); i++)
			{
				MWC_UTIL_ConvertStringUpper(stEvtInfo.astParentalRating[i].aucIsoCountryCode);
				if (MWC_UTIL_DvbStrncmp(stEvtInfo.astParentalRating[i].aucIsoCountryCode, szIso3166Code, LANG_639_CODE_MAX) == 0)
				{
					bFind			= TRUE;
#if defined(CONFIG_OP_SES)
					nParentalSetup	= eDxRATING_AGE_15; // SES는 Parental Rating 나이를 fix시킴
#else
					nParentalSetup	= MGR_PG_GetRecordRatingFromSetup();
#endif
					bLock			= MGR_PG_IsRatingOk(	pstContext->ulActionId,
															pstContext->stSetup.hCurrSvc, eOrgDeliType,
															(DxRatingAge_e)nParentalSetup,
															(DxRatingAge_e)(stEvtInfo.astParentalRating[i].ucParentalRating));
					if(FALSE == bLock)
						bMetaUpdate_u1cLocked = TRUE;

					//add current parental rating and event id for Delayed recording
					xmgr_rec_AddEvtInfoToEvtList(pstContext, stEvtInfo.nEventId, stEvtInfo.astParentalRating[i].ucParentalRating, stEvtInfo.ulStartTime);
					break;
				}
			}

#if !defined(CONFIG_SET_LOCK_FOR_NO_RATENTAL_CONTROL_INFO)
			if (FALSE == bFind)
			{
				/* parental rating 정보에 해당 국가가 없는 경우에 첫번째 Parental Rate를 사용하는 경우이다.
				 * 이는 향마다 다르다.
				 */
				nParentalSetup	= MGR_PG_GetRecordRatingFromSetup();
				bLock			= MGR_PG_IsRatingOk(	pstContext->ulActionId,
														pstContext->stSetup.hCurrSvc,
														eOrgDeliType,
														(DxRatingAge_e)nParentalSetup,
														(DxRatingAge_e)(stEvtInfo.astParentalRating[0].ucParentalRating));
				if( FALSE == bLock )
					bMetaUpdate_u1cLocked = TRUE;
			}
#endif
		}

#if defined(CONFIG_SET_LOCK_FOR_NO_RATENTAL_CONTROL_INFO)
		/* ziggo spec 에서는 해당 국가와 일치하는 것만 pc 로 사용하며,
		 * 해당 국가가 없거나, PC 값이 없는 경우는 모두 lock 이라고 한다.
		 */
		if( FALSE == bFind )
			bMetaUpdate_u1cLocked = TRUE;
#endif

		if( bMetaUpdate_u1cLocked == TRUE )
		{
			/*Recording 정보 Update*/
			pstMetaInfo->u1cLocked = 1;
			SVC_META_SetRecordInfo(ulContentId, pstMetaInfo);
		}
	}

	return ERR_OK;
}

STATIC HBOOL xmgr_rec_IsSvcLocked(Handle_t hSvc)
{
	DxRatingAge_e	eParentalSetup = 0;
	HERROR			hErr = ERR_OK;
	HBOOL			bLocked = FALSE;

	/* first */
//	eParentalSetup = MWC_PARAM_GetMaturityRating();
#if defined(CONFIG_OP_SES)
	eParentalSetup = eDxRATING_AGE_15; // SES는 Parental Rating 나이를 fix시킴
#else
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_MATURITYRATING, (HUINT32 *)&eParentalSetup, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_MATURITYRATING) Error!!!\n");
	}
#endif

	if( eDxRATING_AGE_LOCK_ALL == eParentalSetup )
		return TRUE;

	/* second */
	{
		DbSvc_Info_t stSvcInfo;
		HxSTD_MemSet(&stSvcInfo, 0, sizeof(DbSvc_Info_t));

		hErr = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo);
		if( ERR_OK != hErr )
			return FALSE;

		bLocked = DbSvc_GetLocked(&stSvcInfo);
	}

	if(bLocked == TRUE)
		return TRUE;

	return FALSE;
}

STATIC HERROR	xmgr_rec_ChangeRecTSRToDelayed (xmgrRec_Context_t *pstContext, HUINT32 ulSlot, MgrRec_EndReasonType_e *pFailReason)
{
	HERROR	hErr	=	ERR_FAIL;

	SvcRec_DelayedRecSetup_t 	stNewSetup;
	MgrRec_EndReasonType_e		eFailReason	= eMgrRecFailed_NOFAIL;
	SvcMeta_Event_t 			stEvtInfo;
	HUINT8						szIso3166Code[LANG_639_CODE_MAX+1];

	HUINT32						ulContentId 	= 	0;
	SvcMeta_Record_t 			*pstMetaInfo	=	NULL;
	MgrRec_RecSetup_t			*pstMgrSetup	=	NULL;
	HUINT8						aucTsrTimeStampFileName[FS_MAX_FILE_NAME_LEN];

	if ( (NULL == pstContext) || (NULL == pFailReason) )
	{
		HxLOG_Error ("pstContext is Null. \n");
		BUS_SendMessage (NULL, eMEVT_PVR_NOTIFY_RECORD_SETUP_FAILED, 0, eMgrRecFailed_UNKNOWN, ulSlot, 0);
		return ERR_FAIL;
	}

	//초기화
	*pFailReason = eMgrRecFailed_UNKNOWN;

	ulContentId	=	pstContext->ulContentId;
	pstMetaInfo	=	&(pstContext->stMetaInfo);
	pstMgrSetup	=	&(pstContext->stSetup);

#if defined(CONFIG_OP_UK_DTT)
	// check Recording Maxium number. 
	{
		HUINT32 		*pulContentId = NULL;
		HUINT32 		ulContentNum = 0; 

		(void)SVC_META_GetContentsIdList(&ulContentNum, &pulContentId);
		if(ulContentNum >=MAX_RECORD_LIST)
		{
			*pFailReason = eMgrRecFailed_MAXRECORDLISTNUM;
			(void)SVC_META_FreeContentsIdList (pulContentId);
			return ERR_FAIL;
		}
		(void)SVC_META_FreeContentsIdList (pulContentId);
	}
#endif

	{
		SvcFs_DevIdx_e          ePvrDevIdx  = eSVC_FS_DEV_NULL;

		hErr = SVC_FS_GetPvrDevIdPartIdx (pstMgrSetup->ulPvrIdx, &ePvrDevIdx, NULL);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("FS_PVR_FindPvrDevIdPartIdx failed.\n");
			eFailReason = eMgrRecFailed_NOHDD;
		}
		else
		{
			if (TRUE != SVC_FS_IsDevicePvrPossible (ePvrDevIdx))
			{
				HxLOG_Error ("SVC_FS_IsDevicePvrPossible failed.\n");
				eFailReason = eMgrRecFailed_HDDFULL;
			}
		}

		if ( eFailReason == eMgrRecFailed_HDDFULL)
		{
			HxLOG_Error("Hdd full. can't change tsr to delayed. slot=(%d) \n",ulSlot);
			BUS_SendMessage (NULL, eMEVT_PVR_NOTIFY_RECORD_SETUP_FAILED, 0, eMgrRecFailed_HDDFULL, ulSlot, 0);
			*pFailReason = eFailReason;
			return ERR_FAIL;
		}
	}

	/*get event info*/
	hErr = xmgr_rec_GetEventInfometa(ulContentId, &stEvtInfo);
	if( ERR_OK != hErr )
	{
		HxLOG_Error("can't get a eventInfo\n" );
		BUS_SendMessage (NULL, eMEVT_PVR_NOTIFY_RECORD_SETUP_FAILED, 0, eMgrRecFailed_UNKNOWN, ulSlot, 0);
		return ERR_FAIL;
	}

	if (pstMgrSetup->eRecType == eMgrRecType_TSR)
	{
		HUINT8	i = 0;
		HxCountry_e eCountryId;

		// eCountryId = MWC_PARAM_GetCountryId();
		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32 *)&eCountryId, 0);
		if (ERR_OK != hErr)
		{
			HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID) Error!!\n");
		}
		hErr = xmgr_rec_UtilGetCountryCode(eCountryId, szIso3166Code);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("xmgr_rec_UtilGetCountryCode. Error!!\n");
		}

		for(i = 0; i < stEvtInfo.ucParentalRatingNum && i < PVR_PARENTAL_RATING_MAX; i++)
		{
			MWC_UTIL_ConvertStringUpper(stEvtInfo.astParentalRating[i].aucIsoCountryCode);
			if (MWC_UTIL_DvbStrncmp(stEvtInfo.astParentalRating[i].aucIsoCountryCode, szIso3166Code, LANG_639_CODE_MAX) == 0)
			{
				//	add current parental rating and event id for Delayed recording
				xmgr_rec_AddEvtInfoToEvtList(pstContext, stEvtInfo.nEventId, stEvtInfo.astParentalRating[i].ucParentalRating, stEvtInfo.ulStartTime);
				break;
			}
		}
	}

	HxSTD_MemSet(aucTsrTimeStampFileName, 0, FS_MAX_FILE_NAME_LEN);
	HxSTD_PrintToStrN(aucTsrTimeStampFileName, FS_MAX_FILE_NAME_LEN, "%s%s%s",
				pstMetaInfo->aucPath,
				pstMetaInfo->aucFileName, EXT_JMETA_TSMPFILE);

	/* meta update */
	hErr = xmgr_rec_SetMeta_u1cLocked(pstContext);
	if( ERR_OK != hErr )
	{
		HxLOG_Error("can't call func'xmgr_rec_SetMeta_u1cLocked()'\n");
//		return ERR_FAIL;
	}


	if (eMgrRecType_TSR != pstMgrSetup->eRecType)
	{
		HxLOG_Error ("Error: RecType isn't TSR, RecordType(%d)\n", pstMgrSetup->eRecType);
		BUS_SendMessage (NULL, eMEVT_PVR_NOTIFY_RECORD_SETUP_FAILED, 0, eMgrRecFailed_UNKNOWN, ulSlot, 0);
		return ERR_FAIL;
	}

	hErr = SVC_REC_IsAvailableDelayed(pstContext->hAction);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_REC_IsAvailableDelayed. Error\n");
		BUS_SendMessage (NULL, eMEVT_PVR_NOTIFY_RECORD_SETUP_FAILED, 0, eMgrRecFailed_NOSC_SCR, ulSlot, 0);
		return ERR_FAIL;
	}

	pstMgrSetup->ulSlot		= ulSlot;
	pstMgrSetup->eRecType	= eMgrRecType_DELAYED;

	// FileName 및 recording 정보 채우기
	hErr = xmgr_rec_SetupMetaInfo (pstContext, pstMetaInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error:\n");
		BUS_SendMessage (NULL, eMEVT_PVR_NOTIFY_RECORD_SETUP_FAILED, 0, eMgrRecFailed_UNKNOWN, ulSlot, 0);
		return ERR_FAIL;
	}

	pstMetaInfo->u1cScrambled = pstContext->stMetaInfo.u1cScrambled;
	SVC_META_SetRecordInfo(ulContentId, pstMetaInfo);

	/* startdelayed */
	{
		/* PVR record id (0~5)와 겹치게 사용하면 안됨  (delayed record의 instance id 와 중복되면 문제 발생함) */
		static HUINT32	s_ulInstanceId = 0x111;
		s_ulInstanceId++;

		stNewSetup.ulInstanceId 		= s_ulInstanceId;
		stNewSetup.ulTsrDurationTime 	= pstContext->ulTsrtoDelayTime;
		stNewSetup.ulStartTime			= pstMetaInfo->ulStartTime;
		stNewSetup.ulEndTime 			= pstMetaInfo->ulEndTime;
		stNewSetup.ulRecTime 			= pstMetaInfo->ulRecTime;
		stNewSetup.bSplit 				= FALSE;
		stNewSetup.bEncryption 			= (pstMetaInfo->u1cEncryption == 1) ? TRUE : FALSE;
		HLIB_STD_StrUtf8NCpy (stNewSetup.aucDisplayEvtName, pstMetaInfo->aucDisplayEvtName, PVR_MAX_TITLE_LEN);
		HLIB_STD_StrUtf8NCpy (stNewSetup.aucPath, pstMetaInfo->aucPath, FS_MAX_FILE_NAME_LEN );
		HxSTD_PrintToStrN(stNewSetup.aucFileName, FS_MAX_FILE_NAME_LEN, "%s", pstMetaInfo->aucFileName);	// set as hidden by '.'


#if defined(CONFIG_DEBUG)
		HxLOG_Debug ("[XMGR:%d]----------<REC:DELAYED_START>----------\n", __LINE__);
		HxLOG_Debug ("[XMGR:%d]\t[ulInstanceId\t:\t%d]\n", 		__LINE__, stNewSetup.ulInstanceId);
		HxLOG_Debug ("[XMGR:%d]\t[ulTsrDurationTime\t:\t%d]\n", __LINE__, stNewSetup.ulTsrDurationTime);
		HxLOG_Debug ("[XMGR:%d]\t[ulStartTime\t:\t%d]\n", 	__LINE__, stNewSetup.ulStartTime);
		HxLOG_Debug ("[XMGR:%d]\t[ulEndTime\t:\t%d]\n", 	__LINE__, stNewSetup.ulEndTime);
		HxLOG_Debug ("[XMGR:%d]\t[ulRecTime\t:\t%d]\n", 	__LINE__, stNewSetup.ulRecTime);
		HxLOG_Debug ("[XMGR:%d]\t[bSplit\t:\t%d]\n", 		__LINE__, stNewSetup.bSplit);
		HxLOG_Debug ("[XMGR:%d]\t[bEncryption\t:\t%d]\n", 	__LINE__, stNewSetup.bEncryption);
		HxLOG_Debug ("[XMGR:%d]\t[bUseUserEncKey\t:\t%d]\n",__LINE__, stNewSetup.bUseUserEncKey);
		HxLOG_Debug ("[XMGR:%d]\t[aucDpEvtName\t:\t%s]\n", 	__LINE__, stNewSetup.aucDisplayEvtName);
		HxLOG_Debug ("[XMGR:%d]\t[aucPath\t:\t%s]\n", 		__LINE__, stNewSetup.aucPath);
		HxLOG_Debug ("[XMGR:%d]\t[aucFileName\t:\t%s]\n", 	__LINE__, stNewSetup.aucFileName);
		HxLOG_Debug ("[XMGR:%d]----------<DELAYED_START:REC>----------\n", __LINE__);
#endif

		/* get failreason */
		{
			SvcFs_DevIdx_e			ePvrDevIdx	= eSVC_FS_DEV_NULL;

			hErr = SVC_FS_GetPvrDevIdPartIdx (pstMgrSetup->ulPvrIdx, &ePvrDevIdx, NULL);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("FS_PVR_FindPvrDevIdPartIdx failed.\n");
				eFailReason = eMgrRecFailed_NOHDD;
			}
			else
			{
				if (TRUE != SVC_FS_IsDevicePvrPossible (ePvrDevIdx))
				{
					HxLOG_Error ("SVC_FS_IsDevicePvrPossible failed.\n");
					eFailReason = eMgrRecFailed_HDDFULL;
				}
			}
		}

		//실패 이유 반환 값 복사
		*pFailReason = eFailReason;

		if (eMgrRecFailed_NOFAIL != eFailReason)
		{
			MgrRec_RecSetup_t	stErrorSetup = {0};
			DxSchedule_t		pstSchedule = {0,};

			stErrorSetup.ulSlot		= ulSlot;
			stErrorSetup.hCurrSvc 	= pstMgrSetup->hCurrSvc;
			stErrorSetup.ulPvrIdx	= pstMgrSetup->ulPvrIdx;

			hErr = SAPI_GetSchedule (stErrorSetup.ulSlot, &pstSchedule);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("SAPI_GetSchedule err: ulSlot(%d)\n", stErrorSetup.ulSlot);
				return ERR_FAIL;
			}
			MGR_RECORDER_CreateRecordFailReason (&stErrorSetup, eFailReason, &pstSchedule);
			BUS_SendMessage(NULL,eMEVT_PVR_NOTIFY_RECORD_SETUP_FAILED, 0, eFailReason, pstSchedule.ulSlot, 0);

			HxLOG_Error ("Delayed-Rec. Error (%d)\n", eFailReason);
			return ERR_FAIL;
		}

		hErr = SVC_REC_StartDelayed(pstContext->hAction, &stNewSetup, TRUE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_REC_StartDelayed. Error\n");
			BUS_SendMessage (NULL, eMEVT_PVR_NOTIFY_RECORD_SETUP_FAILED, 0, eMgrRecFailed_UNKNOWN, ulSlot, 0);
			return ERR_FAIL;
		}
	}

	hErr = SVC_META_SetAsDeleteSkipContent (ulContentId, TRUE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_META_SetAsDeleteSkipContent. Error\n");
	}

	/* actionType 을 eRecType_TSR_REC 에서 eRecType_REC 으로 변경한다. */
	MGR_ACTION_ChangeType(pstContext->ulActionId, eRecType_TSR_REC, eRecType_REC);
	MGR_ACTION_ChangePriority (pstContext->ulActionId, eAmPriority_RECORD);

	SVC_META_SaveTimeStampTsrToDelayedRec(pstContext->ulContentId, pstContext->ulTsrtoDelayTime, aucTsrTimeStampFileName);

	BUS_SendMessage(NULL, eMEVT_PVR_NOTIFY_RECORD_STARTED, pstContext->hAction, pstMgrSetup->eRecType, ulContentId, ulSlot);

	HxSTD_PrintToStrN (pstContext->szDelayCopyFileName, FS_MAX_FILE_NAME_LEN, "%s%s", stNewSetup.aucPath, stNewSetup.aucFileName);
	pstContext->bDelayedCopy = TRUE;

	xmgr_rec_LogicFrontpanel(pstMgrSetup->eRecType, TRUE);

	return ERR_OK;
}


#if defined(CONFIG_APCORE_IP_EPG)
STATIC void xmgr_rec_OnDamaSeriesDataReceived (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	SvcMeta_Record_t * pSvcRecMeta = (SvcMeta_Record_t *)userdata;
	DxEvent_t		*pstEvent;
	DAxIterator_t	iter;

	HxLOG_Trace();

	if (size == 0)
	{
		HxLOG_Error("size is 0 \n");
		return;
	}
	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		pstEvent = (DxEvent_t *)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
		if(pstEvent)
		{
			pSvcRecMeta->ulEpisodeNum = pstEvent->extension.ipEpg.episodeNum;
			pSvcRecMeta->ulSeason = pstEvent->extension.ipEpg.season;
			DLIB_FreeType(iter.name, pstEvent);
			break;
		}
	}
}

STATIC	HERROR xmgr_rec_FindEpisodeAndSeasonFromIPEPG(DxSchedule_t	*pstSchedule, SvcMeta_Record_t *pstMetaInfo)
{
	HCHAR		query[256];
	HUINT32 	id;

	if(pstSchedule==NULL || pstMetaInfo==NULL)
	{
		HxLOG_Error ("pstSchedule or pstMetaInfo is NULL.\n");
		return ERR_FAIL;
	}

	HxSTD_PrintToStrN(query, 256
		, "SELECT * FROM %s WHERE svcuid=%d AND seriesid=%d AND eventid=%d"
		, DxNAMEOF(eDxEPG_TYPE_IP)
		, pstSchedule->ulSvcUid
		, pstSchedule->uExtInfo.stRec.ulSeriesId, pstSchedule->uExtInfo.stRec.nEvtId
	);

	id = DAPI_Query(query, (DAPI_Getter)xmgr_rec_OnDamaSeriesDataReceived, (void *)pstMetaInfo);
	DAPI_Wait(10000, id);
	if (id == 0)
	{
		HxLOG_Error("Query FAIL \n");
		return ERR_FAIL;
	}
	else
	{
		HxLOG_Debug("Query Success \n");
		return ERR_OK;
	}
}

#if defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
STATIC void	xmgr_rec_CbDamaForCheckSeriesRecordLock (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	HUINT32				*pulContentId = (HUINT32 *)userdata;

	if (data && size && pulContentId)
	{
		HINT32			 nTotalContent = 0, nLockedContent = 0;
		DAxIterator_t	 iter;
		DxRecListData_t	*pstDxRecItem;

		DAPI_InitIterator(&iter, data, size, 0);
		while (DAPI_NextIterator(&iter))
		{
			pstDxRecItem = (DxRecListData_t *)iter.data;

			if (pstDxRecItem->ulContentId == *pulContentId)
			{
				continue;
			}

			if (pstDxRecItem->bUserLock)
			{
				nLockedContent++;
			}

			nTotalContent++;
		}

		if (nTotalContent > 0)
		{
			BUS_PostMessage(NULL, eMEVT_PVR_RECORD_SERIES_LOCK_FOUND, HANDLE_NULL, (HINT32)*pulContentId, nLockedContent, nTotalContent);
		}
	}

	if (pulContentId)
	{
		OxAP_Free(pulContentId);
	}
}

STATIC HERROR xmgr_rec_CheckSeriesRecordLockViaDama (HUINT32 ulContentId, HCHAR *szSeriesId)
{
	HUINT32				 ulQueryLen;
	HUINT32				*pulContentId = NULL;
	HCHAR				*pszQuery = NULL;
	HERROR				 hErr, hResult = ERR_FAIL;

	if ((NULL == szSeriesId) || ('\0' == *szSeriesId))
	{
		HxLOG_Error("No Series ID in arg\n");
		goto END_FUNC;
	}

	ulQueryLen = HxSTD_StrLen(szSeriesId) + 128;

	pulContentId = (HUINT32 *)OxAP_Malloc(sizeof(HUINT32));
	pszQuery = (HCHAR *)OxAP_Malloc(ulQueryLen + 1);
	if ((NULL == pszQuery) || (NULL == pulContentId))
	{
		HxLOG_Error("Memory allocation failed:\n");
		goto END_FUNC;
	}

	*pulContentId = ulContentId;
	HxSTD_PrintToStrN(pszQuery, ulQueryLen
		, "SELECT * FROM %s WHERE seriesId=\"%s\""
		, DxNAMEOF(DxRecListData_t)
		, szSeriesId);

	pszQuery[ulQueryLen] = '\0';

	hErr = DAPI_Query((const HCHAR *)pszQuery, xmgr_rec_CbDamaForCheckSeriesRecordLock, (void *)pulContentId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("DAPI_Query err: pszQuery=[%s]\n", pszQuery);
		goto END_FUNC;
	}

	pulContentId = NULL;
	hResult = ERR_OK;

END_FUNC:
	if (NULL != pulContentId)
	{
		OxAP_Free(pulContentId);
	}

	if (NULL != pszQuery)
	{
		OxAP_Free(pszQuery);
	}

	return hResult;
}
#else
STATIC	xmgrRecSeriesLock_e	xmgr_rec_SeriesRecCheckLock(Handle_t hSvc, HCHAR *szSeriesId)
{
	xmgrRecSeriesLock_e eSeriesLock = eXmgrRecSeriesLock_Init;
	HUINT32				i,ulNumOfContent = 0;
	HUINT32				*pulContentList = NULL;
	HERROR				hErr;
	SvcMeta_Record_t	stRecord;

	HUINT32				ulSeriesCnt = 0;
	HUINT32				ulLockCnt = 0;

	if (HxSTD_StrEmpty(szSeriesId) == TRUE)
	{
		HxLOG_Debug("Empty series id \n");
		goto END_FUNC;
	}

	hErr = SVC_META_GetContentsIdList(&ulNumOfContent, &pulContentList);
	if (hErr != ERR_OK)
	{
		HxLOG_Debug("SVC_META_GetContentsIdList failed. \n");
		goto END_FUNC;
	}

	for (i=0; i<ulNumOfContent; i++)
	{
		hErr = SVC_META_GetRecordInfo(pulContentList[i], &stRecord);
		if (hErr != ERR_OK)
		{
			continue;
		}

		// ignore different series
		// if service check should be added, then apk_meta_record_CompareSeriesID also should add service check
		if (HxSTD_StrNCmp(stRecord.szSeriesId,szSeriesId, EPG_TVA_CRID_TOTAL_LEN) != 0)
		{
			continue;
		}

		ulSeriesCnt++;

		if (stRecord.bUserLocked)
		{
			ulLockCnt++;
		}
	}

	HxLOG_Debug("SeriesCnt(%d) lockCnt(%d) \n", ulSeriesCnt, ulLockCnt);

	if (ulSeriesCnt > 0)
	{
		if (ulLockCnt == ulSeriesCnt)
		{
			eSeriesLock = eXmgrRecSeriesLock_True;
		}
		else if (ulLockCnt == 0)
		{
			eSeriesLock = eXmgrRecSeriesLock_False;
		}
		else
		{
			eSeriesLock = eXmgrRecSeriesLock_Some;
		}
	}

END_FUNC:
	if (pulContentList != NULL)
	{
		SVC_META_FreeContentsIdList(pulContentList);
	}

	return eSeriesLock;
}
#endif
#endif

STATIC HERROR	xmgr_rec_SetupPids (xmgrRec_Context_t *pstContext, DbSvc_Info_t *pstSvcInfo)
{
	HERROR 						hErr;
	HUINT8						*pucRawSectionBuffer = NULL;
	SvcMeta_Service_t			*pstMetaSvc = NULL;
	SvcSi_MakeSectionSpec_t		 stMakeSecSpec;

	pstMetaSvc = (SvcMeta_Service_t *)OxMGR_Calloc(sizeof(SvcMeta_Service_t));
	pucRawSectionBuffer = (HUINT8 *)OxMGR_Calloc(4096);
	if (!pucRawSectionBuffer || !pstMetaSvc)
	{
		HxLOG_Error("memory allocation failed:\n");
		goto END_FUNC;
	}

	hErr = SVC_META_GetServiceInfo(pstContext->ulContentId, pstMetaSvc);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("SVC_META_GetServiceInfo err: usSvcId(%d)\n", pstContext->ulContentId);
	}

	/* pid start */
	pstContext->stRecPidInfo.stKnown.stSI[eREC_SiPAT].usPid	=	PAT_PID;
	pstContext->stRecPidInfo.stKnown.stSI[eREC_SiPMT].usPid	=	DbSvc_GetPmtPid(pstSvcInfo);
	pstContext->stRecPidInfo.stKnown.stSI[eREC_SiPCR].usPid	=	DbSvc_GetPcrPid(pstSvcInfo);
	pstContext->stRecPidInfo.stKnown.stVideo[0].usPid		=	DbSvc_GetVideoPid(pstSvcInfo);

	/* replace PAT */
	{
		SvcSi_MakeSecArg_t	unArg;
		HINT32	ulRawSectionSize = 0;

		unArg.eSiType			=	eSIxTABLETYPE_Pat;
		unArg.stPat.eSiType		=	eSIxTABLETYPE_Pat;
		unArg.stPat.usTsId		=	DbSvc_GetTsId(pstSvcInfo);
		unArg.stPat.usSvcId		=	DbSvc_GetSvcId(pstSvcInfo);
		unArg.stPat.usPmtPid	=	DbSvc_GetPmtPid(pstSvcInfo);

		hErr = MGR_RECORDER_GetMakeSectionSiSpec(&stMakeSecSpec);
		if (hErr == ERR_OK)
		{
			hErr = SVC_SI_MakeSiSection(pstContext->ulActionId, &stMakeSecSpec, &unArg, pucRawSectionBuffer, &ulRawSectionSize);
		}

		if ((ERR_OK == hErr) && (ulRawSectionSize > 0))
		{
			SVC_REC_ReplacePSI(pstContext->hAction, PAT_PID, pucRawSectionBuffer, ulRawSectionSize);
		}
		else
		{
			HxLOG_Error ("SVC_SI_MakeSiSection: siType:PAT, ulActionId(%d)\n", pstContext->ulActionId);
		}
	}

	{
		HINT32 i=0;
		HINT32 nMinTrack=0;

		SvcSi_AudioList_t 		stAudioList;
		SvcSi_SubtitleList_t 	stSubtitleList;

		/*audio*/
		HxSTD_MemSet(&stAudioList, 0, sizeof(SvcSi_AudioList_t));
		SVC_REC_CleanReqPidDes(pstContext->stRecPidInfo.stKnown.stAudio, SvcSi_MAX_AUDIO_ES_NUM);
		hErr = SVC_SI_GetAudioList(pstContext->ulActionId, DbSvc_GetSvcId(pstSvcInfo), &stAudioList);
		if (ERR_OK == hErr)
		{
			nMinTrack = (stAudioList.nAudio < SvcSi_MAX_AUDIO_ES_NUM)? stAudioList.nAudio : SvcSi_MAX_AUDIO_ES_NUM;
			for (i = 0 ; i < nMinTrack ; i++)
			{
				pstContext->stRecPidInfo.stKnown.stAudio[i].usPid = stAudioList.pstAudioInfo[i].usPid;
			}

			SVC_SI_FreeAudioList(&stAudioList);	//free
		}


		/*subtitle*/
		HxSTD_MemSet(&stSubtitleList, 0, sizeof(SvcSi_SubtitleList_t));
		SVC_REC_CleanReqPidDes(pstContext->stRecPidInfo.stKnown.stSubtitle, SvcSi_MAX_SUBTTL_ES_NUM);
		hErr = SVC_SI_GetSubtitleList(pstContext->ulActionId, DbSvc_GetSvcId(pstSvcInfo), &stSubtitleList);
		if (ERR_OK == hErr)
		{
			nMinTrack = (stSubtitleList.ulSubtitleNum < SvcSi_MAX_SUBTTL_ES_NUM) ? stSubtitleList.ulSubtitleNum : SvcSi_MAX_SUBTTL_ES_NUM;
			for (i = 0; i < nMinTrack; i++)
			{
				pstContext->stRecPidInfo.stKnown.stSubtitle[i].usPid = stSubtitleList.pstSubtitleInfo[i].usPid;
			}

			SVC_SI_FreeSubtitleList(&stSubtitleList);	//free
		}

		/*ttx*/
		pstContext->stRecPidInfo.stKnown.stTeletext[0].usPid = DbSvc_GetTtxPid(pstSvcInfo);

		/*super CC*/
		HxSTD_MemSet(&stSubtitleList, 0, sizeof(SvcSi_SubtitleList_t));
		SVC_REC_CleanReqPidDes(pstContext->stRecPidInfo.stKnown.stCaption, SvcSi_MAX_CAPTION_ES_NUM);
		hErr = SVC_SI_GetSuperCcList(pstContext->ulActionId, DbSvc_GetSvcId(pstSvcInfo), &stSubtitleList);
		if (ERR_OK == hErr)
		{
			nMinTrack = (stSubtitleList.ulSubtitleNum < SvcSi_MAX_CAPTION_ES_NUM) ? stSubtitleList.ulSubtitleNum : SvcSi_MAX_CAPTION_ES_NUM;
			for (i = 0; i < nMinTrack ;i++)
			{
				pstContext->stRecPidInfo.stKnown.stCaption[i].usPid = stSubtitleList.pstSubtitleInfo[i].usPid;
			}

			SVC_SI_FreeSuperCcList(&stSubtitleList);	//free
		}
	}
#if defined (CONFIG_MW_MM_AIT_REC)
	{
		HUINT16		ulCount = 0;
		HUINT32 				 ulActionId = SVC_PIPE_GetActionId(pstContext->hAction);
		HxList_t				*pstAitList = NULL;
		HERROR					 hErr;

		hErr = SVC_SI_GetAitPidFromPMT(ulActionId, &pstAitList);
		if ((hErr != ERR_FAIL) && (pstAitList != NULL))
		{
			HxList_t	*pstAitItem = NULL;

			for (pstAitItem = pstAitList; pstAitItem != NULL; pstAitItem = pstAitItem->next)
			{
				HUINT32 *pulAitPid = (HUINT32 *)HLIB_LIST_Data(pstAitItem);
				if (pulAitPid != NULL)
				{
					/* Application type이 0x10인 AIT만을 수신한다. */
					pstContext->stRecPidInfo.stKnown.stSI[eREC_SiAIT].usPid = *pulAitPid;
				}
			}

			HLIB_LIST_RemoveAllFunc(pstAitList, xmgr_rec_FreeAitPidList);
		}

		for(ulCount = 0; ulCount < eSvcRec_MAX_REC_NUM; ulCount++)
		{
			HxLOG_Debug("[%d] PID: [0x%x] \n", ulCount, pstContext->stRecPidInfo.stKnown.stSI[ulCount].usPid);
		}
	}
#endif

END_FUNC:
	if (pucRawSectionBuffer)			{ OxMGR_Free(pucRawSectionBuffer); }
	if (pstMetaSvc)						{ OxMGR_Free(pstMetaSvc); }

	return ERR_OK;
}


HERROR xmgr_rec_UpdateMeta (xmgrRec_Context_t *pstContext, HBOOL *pbDeleted, SvcRec_RecInfo *pstRecInfo)
{
	HERROR					hErr;
	MgrRec_RecSetup_t		*pstRecSetup	= NULL;
	HUINT32					ulContentId;
	MgrAction_Type_e		eActionType 	= 0;
	HBOOL					bDeleted 		= TRUE;
	SvcFs_DevIdx_e			ePvrDevIdx 		= eSVC_FS_DEV_NULL;
	SvcMeta_Record_t		*pstMetaInfo = &(pstContext->stMetaInfo);
	SvcFs_DevInfo_t 		stDevInfo;

	pstRecSetup 	=	&pstContext->stSetup;
	ulContentId 	=	pstContext->ulContentId;
	eActionType		= xmgr_rec_GetActionType( pstRecSetup );

	hErr = SVC_FS_GetPvrDevIdPartIdx (pstRecSetup->ulPvrIdx, &ePvrDevIdx, NULL);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_FS_GetPvrDevIdPartIdx failed. pvrIdx(%d) \n",pstRecSetup->ulPvrIdx);
		goto END_FUNC;
	}

	/*
	 * Record 중이고 정상 종료일 경우
	 */
	hErr = SVC_FS_GetDeviceInfo (ePvrDevIdx, &stDevInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_FS_GetDeviceInfo. Error\n");
		goto END_FUNC;
	}

	hErr = SVC_META_GetRecordInfo (ulContentId, pstMetaInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_META_GetRecordInfo. Error cid(%d) \n",ulContentId);
		goto END_FUNC;
	}

	if ((stDevInfo.bConnected == TRUE) && (stDevInfo.bMounted == TRUE))
	{
		if (eRecType_REC == eActionType)
		{
			if( MGR_RECORDER_GetRecorderParam(eMgrRec_MIN_REQUIRED_RECORD_TIME) <= pstMetaInfo->ulRecTime )
			{
				pstMetaInfo->ulRecTime = pstRecInfo->ulDuration;
				pstMetaInfo->ulEndTime = pstMetaInfo->ulStartTime + pstRecInfo->ulDuration;

				pstMetaInfo->ucRecordStatus = eMgrRec_STS_RECORDED;
				hErr = SVC_META_SetRecordInfo (ulContentId, pstMetaInfo);
				if (ERR_OK != hErr)
					HxLOG_Error ("SVC_META_SetRecordInfo(). Error\n");

				{
					HBOOL bPlay = FALSE;
					bPlay = MGR_PLAYBACK_IsPlaybackId (ulContentId, NULL);
					if( FALSE == bPlay )
					{
						hErr = SVC_META_SetAsDeleteSkipContent (ulContentId, TRUE);
						if (ERR_OK != hErr)
							HxLOG_Error("SVC_META_SetAsDeleteSkipContent. Error\n");
					}
				}

				/* Medialist 에서 recording info 를 update 한다. */
				BUS_PostMessage (NULL, eMEVT_PVR_MEDIALIST_UPDATE, (Handle_t)0, 0, 0, 0);

				bDeleted = FALSE;
			}
			else
			{
				hErr = SVC_META_SetAsDeleteSkipContent (ulContentId, FALSE);
				if (ERR_OK != hErr)
					HxLOG_Error("SVC_META_SetAsDeleteSkipContent. Error\n");

				if( (pstRecSetup->eRecType == eMgrRecType_TBR) ||
					(pstRecSetup->eRecType == eMgrRecType_EBR) ||
					(pstRecSetup->eRecType == eMgrRecType_SBR))
				{
					if (0 < pstMetaInfo->ucEndCondition)
					{
						pstMetaInfo->ulRecTime = pstRecInfo->ulDuration;
						pstMetaInfo->ulEndTime = pstMetaInfo->ulStartTime + pstRecInfo->ulDuration;
						
						pstMetaInfo->ucRecordStatus = eMgrRec_STS_INCOMPLETED;
						hErr = SVC_META_SetRecordInfo (ulContentId, pstMetaInfo);
						if (ERR_OK != hErr)
							HxLOG_Error ("SVC_META_SetRecordInfo(). Error\n");

						bDeleted = FALSE;
					}
				}
				else if (pstRecSetup->eRecType == eMgrRecType_FULLTSREC)
				{
					bDeleted = FALSE;
				}
#if defined(CONFIG_MW_CAS_IRDETO_UCAS)
				else if (pstRecSetup->eRecType == eMgrRecType_INSTANT)
				{
					// Base는 아닌데 일단 넣어 두고 생각해 보자. - jhlee
					if (pstMetaInfo->ucEndCondition == eMgrRecFailed_NORIGHT_SCR)
					{
						pstMetaInfo->ucRecordStatus = eMgrRec_STS_INCOMPLETED;
						pstMetaInfo->ulEndTime = pstMetaInfo->ulStartTime + pstMetaInfo->ulRecTime;

						hErr = SVC_META_SetRecordInfo (ulContentId, pstMetaInfo);
						if (ERR_OK != hErr)
							HxLOG_Error ("SVC_META_SetRecordInfo(). Error\n");

						bDeleted = FALSE;
					}
				}
#endif
			}

		}
		else if ((eActionType == eRecType_TSR_REC) || (eActionType == eRecType_EXT_REC))
		{
			bDeleted = TRUE;
		}
	}
	else
	{
		pstMetaInfo->ucRecordStatus = eMgrRec_STS_INCOMPLETED;
		hErr = SVC_META_SetRecordInfoWithOption (ulContentId, pstMetaInfo, FALSE, TRUE);
		if (ERR_OK != hErr)
			HxLOG_Error ("SVC_META_SetRecordInfoWithOption. Error\n");
	}

	if (pstRecSetup->eRecType == eMgrRecType_FULLTSREC)
	{
		HxLOG_Debug("Full Ts rec status change (%d) -> (%d) \n",pstMetaInfo->ucRecordStatus,eMgrRec_STS_RECORDED);
		pstMetaInfo->ucRecordStatus = eMgrRec_STS_RECORDED;
		hErr = SVC_META_SetRecordInfo (ulContentId, pstMetaInfo);
		if (ERR_OK != hErr)
			HxLOG_Error ("SVC_META_SetRecordInfo(). Error\n");
	}

	hErr = ERR_OK;
END_FUNC:
	*pbDeleted = bDeleted;

	return hErr;
}

#define _________INTERNAL_FUNC__________
HERROR xmgr_rec_ClearContext (xmgrRec_Context_t *pstContext)
{
	HxSTD_MemSet (pstContext, 0, sizeof(xmgrRec_Context_t));

	// Initalize HANDLE;
	pstContext->hAction= HANDLE_NULL;

	return ERR_OK;
}

xmgrRec_Context_t *xmgr_rec_GetContext (HUINT32 ulIndex)
{
	if (ulIndex < NUM_MAX_REC_ACTION)
	{
		return &(s_astMgrRecContext[ulIndex]);
	}

	return NULL;
}

void 		xmgr_rec_SetState(xmgrRec_Context_t *pstContext, MgrRec_State_e eState)
{
#if defined(CONFIG_DEBUG)
	static HBOOL isValidTransition[eMgrRecState_Num][eMgrRecState_Num] = {
			/* IDLE      TUNE      NO_SIG    WAIT       BLOCK    RECORDING  STOP */
/* IDLE */		{TRUE,  TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE},
/* TUNE */		{FALSE, FALSE, FALSE, TRUE,  FALSE, FALSE, FALSE},
/* NO_SIG */	{FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE},
/* WAIT */		{FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE },
/* BLOCK */	{FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE },
/* RECORDING*/	{FALSE, FALSE, TRUE,  FALSE, TRUE,  FALSE, TRUE },
/* STOP */		{TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE}
	};

	if (pstContext->eRecState != pstContext->eLastRecState)
	{
		HxLOG_Error("state is changed outside function. curr(%d) last(%d) \n",pstContext->eRecState, pstContext->eLastRecState);
	}
	pstContext->eLastRecState = eState;

	HxLOG_Debug("State Change [%s]->[%s] \n",xmgr_rec_GetStateStr(pstContext->eRecState), xmgr_rec_GetStateStr(eState));
	if (isValidTransition[pstContext->eRecState][eState] == FALSE)
	{
		HxLOG_Error("invalid transition. state [%s]->[%s] \n",xmgr_rec_GetStateStr(pstContext->eRecState),xmgr_rec_GetStateStr(eState));
	}
	if (eState == eMgrRecState_WAIT)
	{
		HxLOG_Info("Wait by (0x%x) \n",pstContext->eWaitFlag);
	}
	if (eState == eMgrRecState_BLOCKED)
	{
		HxLOG_Info("Pause by (0x%x) \n",pstContext->ePauseFlag);
	}
#endif

	pstContext->eRecState = eState;

	xmgr_rec_CheckState(pstContext);
}

HERROR xmgr_rec_SetupRec (xmgrRec_Context_t *pstContext)
{
	HERROR 						hErr = ERR_OK;
	HUINT16						usSvcId;
	DbSvc_Info_t				*pstSvcInfo = NULL, *pstOrgSvcInfo = NULL;
	HBOOL						bChangedPid;
	SvcRec_Setup_t				*pstSvcRecSetup = NULL;

	pstOrgSvcInfo	= (DbSvc_Info_t *)OxMGR_Calloc(sizeof(DbSvc_Info_t));
	pstSvcInfo		= (DbSvc_Info_t *)OxMGR_Calloc(sizeof(DbSvc_Info_t));
	pstSvcRecSetup	= (SvcRec_Setup_t *)OxMGR_Calloc(sizeof(SvcRec_Setup_t));
	if (!pstOrgSvcInfo ||!pstSvcInfo || !pstSvcRecSetup)
	{
		HxLOG_Error("memory allocation failed:\n");
		goto END_FUNC;
	}

	hErr = DB_SVC_GetServiceInfo(pstContext->stSetup.hCurrSvc, pstOrgSvcInfo);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Fail to Get Service Info (0x%x)\n", pstContext->stSetup.hCurrSvc);
		goto END_FUNC;
	}

	hErr = MGR_RECORDER_SetRecordSetup(&pstContext->stSetup, pstSvcRecSetup, &pstContext->stMetaInfo, pstContext->hAction);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("MGR_RECORDER_SetRecordSetup failed. hAction=(0x%x) \n", pstContext->hAction);
		goto END_FUNC;
	}

	hErr = DB_SVC_GetServiceIdFromSvcHandle (pstContext->stSetup.hCurrSvc, &usSvcId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceIdFromSvcHandle failed. hSvc(%d) \n",pstContext->stSetup.hCurrSvc);
		goto END_FUNC;
	}

	hErr = SVC_SI_GetLiveServiceInfoPid (pstContext->hAction, (HUINT32)usSvcId, pstOrgSvcInfo, pstSvcInfo, &bChangedPid);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_SI_GetLiveServiceInfoPid failed \n");
		goto END_FUNC;
	}

	pstSvcRecSetup->eSvcType	= DbSvc_GetSvcType(pstSvcInfo);
	pstSvcRecSetup->eVideoCodec = DbSvc_GetVideoCodec(pstSvcInfo);

#if defined(CONFIG_PROD_ICORDPRO)
	/* SVC_REC_Setup API is called in xmgr_rec_MsgEvtChLocked_Base */
#else
	hErr = SVC_REC_Setup(pstContext->hAction, pstSvcRecSetup, TRUE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_REC_Setup. Error\n");
		goto END_FUNC;
	}
#endif

	/* set pids */
	hErr = xmgr_rec_SetupPids(pstContext, pstSvcInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("xmgr_rec_SetupPids failed. \n");
		goto END_FUNC;
	}

	/* set pid 전 pid info 확인용 */
	xmgr_rec_DebugPrintRecPidInfo_Base(&pstContext->stRecPidInfo, (HCHAR *)__FUNCTION__, __LINE__);

	hErr = SVC_REC_SetPID(pstContext->hAction, &pstContext->stRecPidInfo, TRUE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_REC_SetPID err:\n");
		goto END_FUNC;
	}

	hErr = ERR_OK;
END_FUNC:
	if (pstSvcRecSetup) 			{ OxMGR_Free(pstSvcRecSetup); }
	if (pstOrgSvcInfo) 				{ OxMGR_Free(pstOrgSvcInfo); }
	if (pstSvcInfo) 				{ OxMGR_Free(pstSvcInfo); }

	return hErr;
}
HERROR xmgr_rec_StartRec(xmgrRec_Context_t *pstContext)
{
	HERROR hErr = ERR_OK;

	if (pstContext->eWaitFlag != 0)
	{
		HxLOG_Warning("Can't start rec. still waiting. waitFlag(0x%x) \n",pstContext->eWaitFlag);
		return ERR_FAIL;
	}

	hErr = SVC_REC_Start(pstContext->hAction, TRUE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_REC_Start. Error\n");
		return hErr;
	}

	xmgr_rec_SetState(pstContext,eMgrRecState_RECORDING);

	BUS_SendMessage (NULL, eMEVT_PVR_NOTIFY_RECORD_STARTED,
							pstContext->hAction,
							pstContext->stSetup.eRecType,
							pstContext->ulContentId,
							pstContext->stSetup.ulSlot);

	xmgr_rec_LogicFrontpanel(pstContext->stSetup.eRecType, TRUE);

	return ERR_OK;
}

HERROR xmgr_rec_StartCas(xmgrRec_Context_t *pstContext)
{
	HERROR	hErr = ERR_OK;
	Handle_t 			hSvc		=	pstContext->stSetup.hCurrSvc;
	SvcCas_ActionType_e 	eActionType	=	eCAS_ActionType_Recording;

	switch( pstContext->stSetup.eRecType )
	{
		case eMgrRecType_TSR: 		eActionType = eCAS_ActionType_Tsr_Rec; break;
		case eMgrRecType_SCR2DESC : eActionType = eCAS_ActionType_Descramble_a_File;
									hSvc = (Handle_t)pstContext->stSetup.ulSourceContentId;
									break;
		case eMgrRecType_FULLTSREC:
			HxLOG_Warning("Record Type is Full TS Record...\n");
			return ERR_OK;

		default : break;
	}

	hErr = MGR_CAS_StartService( pstContext->hAction, hSvc, eActionType, eDxCAS_GROUPID_NONE, NULL);
	if( ERR_OK != hErr )
	{
		HxLOG_Error("Critical Error!!!\n");
		return hErr;
	}

	return hErr;
}

HERROR	xmgr_rec_RecPause(Handle_t hAction, xmgrRec_PauseFlag_e ePauseFlag)
{
	BUS_SendMessage(NULL, eSEVT_REC_REQ_COMMAND, hAction, eRecCmd_Pause, ePauseFlag, 0);

	return ERR_OK;
}

HERROR	xmgr_rec_RecResume(Handle_t hAction, xmgrRec_PauseFlag_e ePauseFlag)
{
	BUS_SendMessage(NULL, eSEVT_REC_REQ_COMMAND, hAction, eRecCmd_Resume, ePauseFlag, 0);

	return ERR_OK;
}

HERROR xmgr_rec_GetServiceInfoFromSvc (Handle_t hSvc, SvcRec_Setup_t *pstSvcRecSetup)
{
	if (NULL == pstSvcRecSetup)
		return ERR_FAIL;

	{
		HERROR hErr;
		DbSvc_Info_t stSvcInfo;

		HxSTD_MemSet( &stSvcInfo, 0, sizeof(DbSvc_Info_t) );
		hErr = DB_SVC_GetServiceInfo (hSvc, &stSvcInfo);
		if( hErr != ERR_OK )
		{
			HxLOG_Error ("DB_SVC_GetServiceInfo hErr, hErr:0x%X, hSvc:0x%X\n", hErr, hSvc);
			return hErr;
		}

		pstSvcRecSetup->eSvcType	= DbSvc_GetSvcType(&stSvcInfo);
		pstSvcRecSetup->eVideoCodec	= DbSvc_GetVideoCodec(&stSvcInfo);
//		pstSvcRecSetup->usPmtPid		= stSvcInfo.usPmtPid;
//		pstSvcRecSetup->usSvcId		= stSvcInfo.usSvcId;
	}

	return ERR_OK;
}

HERROR xmgr_rec_SetServiceTimeStamp (xmgrRec_Context_t *pstContext, SvcMeta_Service_t *pstMetaInfo, HBOOL bWriteMeta)
{
	HERROR				 hErr 			= ERR_FAIL;
	HUINT32				 ulRecTime		=	0;
	Handle_t			 hPlayback		=	HANDLE_NULL;
	SvcRec_RecInfo		 stRecInfo;

	if (!pstMetaInfo)
	{
		HxLOG_Critical("pstMetaInfo NULL!!\n");
		return ERR_FAIL;
	}

	if (bWriteMeta)
	{
		hErr = SVC_META_SetServiceInfo(pstContext->ulContentId, pstMetaInfo, TRUE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_META_SetServiceInfo. Error \n");
		}
	}

	// 2. Set Service TimeStamp...
	HxSTD_MemSet (&stRecInfo, 0, sizeof(SvcRec_RecInfo));

	hErr = SVC_REC_GetInfo(pstContext->hAction, &stRecInfo);
	if (ERR_OK != hErr )
	{
		HxLOG_Debug("SVC_REC_GetInfo() failed. \n");
		ulRecTime = 0;
	}

	ulRecTime = stRecInfo.ulDuration;
	if (ulRecTime == PVR_INVALID_REF_TIME)
	{
		ulRecTime = PVR_INVALID_REF_TIME+1;
	}

	hErr = SVC_META_SetServiceTimeStamp(pstContext->ulContentId, ulRecTime, pstMetaInfo);
	if (ERR_OK != hErr )
	{
		HxLOG_Error("SVC_META_SetServiceTimeStamp() error(%x).\n", hErr);
	}

	hErr = SVC_META_GetPlaybackActionByContentId(pstContext->ulContentId, &hPlayback);
	if (ERR_OK == hErr)
	{
		/* update playback-ctx*/
		BUS_SendMessage(NULL, eMEVT_PVR_RECORD_TIMESTAMP_UPDATE, hPlayback, 0, 0, 0);
	}

	return ERR_OK;
}

void xmgr_rec_SetSubtitleInfo(SvcSi_SubtitleList_t *pInput, SvcMeta_SbtlInfoList_t *pstSbtlInfoList)
{
	HUINT32 i;
	if(pInput == NULL || pstSbtlInfoList == NULL)
		return;

	pstSbtlInfoList->ulItemNum = pInput->ulSubtitleNum;

	for(i=0; i< pInput->ulSubtitleNum; i++)
	{
		switch(pInput->pstSubtitleInfo[i].eType)
		{
		case eSI_SUBTITLE_DVB:
			pstSbtlInfoList->astSbtlInfo[i].ucComponentType = pInput->pstSubtitleInfo[i].utArg.stDvbInfo.ucType;
			HxSTD_MemCopy(pstSbtlInfoList->astSbtlInfo[i].aucLangCode,pInput->pstSubtitleInfo[i].utArg.stDvbInfo.aucLangCode,LANG_639_CODE_MAX+1);
			break;
		case eSI_SUBTITLE_EBU:
			pstSbtlInfoList->astSbtlInfo[i].ucComponentType = pInput->pstSubtitleInfo[i].utArg.stEbuInfo.ucType;
			HxSTD_MemCopy(pstSbtlInfoList->astSbtlInfo[i].aucLangCode,pInput->pstSubtitleInfo[i].utArg.stEbuInfo.aucLangCode,LANG_639_CODE_MAX+1);
			break;
		case eSI_SUBTITLE_SUPERCC:
			break;
		default:
			break;
		}
		HxLOG_Debug("ucComponentType[%d]\n",pstSbtlInfoList->astSbtlInfo[i].ucComponentType);
	}
}


void	xmgr_rec_ClearPauseFlag(xmgrRec_Context_t *pstContext, xmgrRec_PauseFlag_e ePauseFlag)
{
	if (!(pstContext->ePauseFlag & ePauseFlag))
	{
		HxLOG_Info("Trying to clear Not-Setted-flag. contextFlag(0x%x) inputFlag(0x%x) \n",pstContext->ePauseFlag,ePauseFlag);
	}
	HxLOG_Info("Rec State(%d) clear pauseFlag(0x%x)->(0x%x) \n",pstContext->eRecState, pstContext->ePauseFlag,pstContext->ePauseFlag&~ePauseFlag);

	pstContext->ePauseFlag &= ~ePauseFlag;

}

void	xmgr_rec_SetPauseFlag(xmgrRec_Context_t *pstContext, xmgrRec_PauseFlag_e ePauseFlag)
{
	if ((pstContext->ePauseFlag & ePauseFlag) != 0)
	{
		HxLOG_Info("Trying to set Already-setted-flag. contextFlag(0x%x) inputFlag(0x%x) \n",pstContext->ePauseFlag,ePauseFlag);
	}
	HxLOG_Info("Rec State(%d) set pauseFlag(0x%x)->(0x%x) \n",pstContext->eRecState, pstContext->ePauseFlag,pstContext->ePauseFlag|ePauseFlag);

	pstContext->ePauseFlag |= ePauseFlag;
}


void	xmgr_rec_ClearWaitFlag(xmgrRec_Context_t *pstContext, xmgrRec_WaitFlag_e eWaitFlag)
{
	if (!(pstContext->eWaitFlag & eWaitFlag))
	{
		HxLOG_Info("Trying to clear Not-Setted-flag. contextFlag(0x%x) inputFlag(0x%x) \n",pstContext->eWaitFlag,eWaitFlag);
	}
	if (pstContext->eRecState != eMgrRecState_WAIT)
	{
		HxLOG_Info("Clear wait while not in wait state. \n");
	}
	HxLOG_Info("Rec State(%d) clear waitFlag(0x%x)->(0x%x) \n",pstContext->eRecState, pstContext->eWaitFlag,pstContext->eWaitFlag&~eWaitFlag);

	pstContext->eWaitFlag &= ~eWaitFlag;

}

void	xmgr_rec_SetWaitFlag(xmgrRec_Context_t *pstContext, xmgrRec_WaitFlag_e eWaitFlag)
{
	if ((pstContext->eWaitFlag & eWaitFlag) != 0)
	{
		HxLOG_Info("Trying to set Already-setted-flag. contextFlag(0x%x) inputFlag(0x%x) \n",pstContext->eWaitFlag,eWaitFlag);
	}
	HxLOG_Info("Rec State(%d) set waitFlag(0x%x)->(0x%x) \n",pstContext->eRecState, pstContext->eWaitFlag,pstContext->eWaitFlag|eWaitFlag);

	pstContext->eWaitFlag |= eWaitFlag;
}

void	xmgr_rec_RunState(xmgrRec_Context_t *pstContext)
{
	HERROR	hErr;
	// 궁극적으로는 start , pause, resume은 이 안에서만 한다. 밖(cas, ...)에서는 flag만 조작 한다
	if (pstContext->eRecState != eMgrRecState_RECORDING)
	{
		HxLOG_Info("eState[%s] hAction(0x%x) \n",xmgr_rec_GetStateStr(pstContext->eRecState),pstContext->hAction);
	}

	if (pstContext->eRecState == eMgrRecState_WAIT)
	{
		if (pstContext->eWaitFlag == 0)
		{
			HxLOG_Debug("Wait done. start rec. hAction(0x%x) \n",pstContext->hAction);
			hErr = xmgr_rec_SetupRec(pstContext);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("xmgr_rec_SetupRec failed. hAction(0x%x) \n",pstContext->hAction);
				return;
			}

#if defined(CONFIG_OP_SES)
			if(pstContext->stMetaInfo.u1cEncryption)
			{
				SvcRec_Setup_t stRecSetup;
				stRecSetup.bEncryption = TRUE;

				HxLOG_Debug("[%s:%d]  hAction(0x%08x) :::::::::::::::::::::::: Enc Rec :::::::::::::::::: \n",  __FUNCTION__, __LINE__, pstContext->hAction);
				if(SVC_REC_SetEncryption(pstContext->hAction, &stRecSetup, FALSE) != ERR_OK)
				{
					HxLOG_Debug("PAL_REC_SetEncryption is error\n");
				}
			}
#endif

			hErr = xmgr_rec_StartRec(pstContext);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("xmgr_rec_StartRec failed. hAction(0x%x) \n",pstContext->hAction);
				return;
			}
		}
		else
		{
			HxLOG_Debug("Wait State. wait for waitFlag(0x%x) to be cleared \n",pstContext->eWaitFlag);
		}
	}

	if (pstContext->eRecState == eMgrRecState_RECORDING)
	{
		if (pstContext->ePauseFlag != 0)
		{
			HxLOG_Debug("Pause Rec. pauseFlag(0x%x) \n",pstContext->ePauseFlag);
			xmgr_rec_PauseRec(pstContext);
		}
	}

	if (pstContext->eRecState == eMgrRecState_BLOCKED)
	{
		if (pstContext->ePauseFlag == 0)
		{
			HxLOG_Debug("Resume Rec. pauseFlag(0x%x) \n",pstContext->ePauseFlag);
			xmgr_rec_ResumeRec(pstContext);
		}
		else
		{
			HxLOG_Debug("Paused Rec. wait for pauseFlag(0x%x) to be cleared  \n",pstContext->ePauseFlag);
		}
	}

	xmgr_rec_CheckState(pstContext);
}
#define _____MSG_PROCESSING_FUNCTIONS_____
BUS_Result_t xmgr_rec_MsgBusCreate_Base (xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR				hErr		= 	ERR_OK;
	HUINT32 			ulActionId	=	SVC_PIPE_GetActionId (hAction);
	Handle_t			hNewAction	=	SVC_PIPE_IncreaseActionVersion(ulActionId);

	MgrRec_RecSetup_t	*pstSetup		=	&(pstContext->stSetup);
	SvcMeta_Record_t	*pstMetaInfo	=	&(pstContext->stMetaInfo);

	HBOOL				bSCR2DESC	=	FALSE;
	SvcRec_Setup_t		stSvcRecSetup;
	MgrRec_EndReasonType_e		eFailReason = eMgrRecFailed_UNKNOWN;

#if defined(CONFIG_APCORE_IP_EPG)
	xmgrRecSeries_t		stSeriesRecInfo;
#endif

	hErr = SVC_REC_CleanReqPidInfo(eRec_PidSetType_Known, &pstContext->stRecPidInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_REC_CleanReqPidInfo failed \n");
	}

	if (pstSetup == NULL)
	{
		HxLOG_Error("pstSetup == Null\n");
		goto ERR_FUNC;
	}

	/* check exception rectype */
	if (eMgrRecType_SCR2DESC == pstSetup->eRecType)
	{
		HxLOG_Debug ("--- START SCR2DESC ---\n");
		bSCR2DESC = TRUE;
	}

	/*resource check*/
	{
		MgrAction_Type_e eActionType;
		eActionType = xmgr_rec_GetActionType(pstSetup);

		hErr = MGR_ACTION_Take(ulActionId, eActionType, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
		if (ERR_OK != hErr)
		{
			HxLOG_Critical("ERROR : MGR_ACTION_Take() hAction=0x%x eActionType=0x%x\n", hAction, eActionType);
			eFailReason = eMgrRecFailed_MAXNUM;
			goto ERR_FUNC;
		}
	}

	/* setup recorder app pstContext */
	{
		pstContext->ulActionId			= ulActionId;
		pstContext->hAction 			= hNewAction;
		pstContext->nCasErrCode 		= 0;
		pstContext->bCasOk				= FALSE;
		pstContext->ulContentId 		= 0xffffffff;
		pstContext->usBaseEventId		= 0xffff;
		pstContext->ulScrambleStartTime	= 0;
		xmgr_rec_SetState(pstContext,eMgrRecState_IDLE);
		xmgr_rec_SetWaitFlag(pstContext,eRecWait_DRM);
		xmgr_rec_SetWaitFlag(pstContext,eRecWait_PMT);
	}

	/*start tune*/
	if (TRUE != bSCR2DESC)
	{
		hErr = xmgr_rec_StartTune (hNewAction, pstSetup->hCurrSvc);
		if( ERR_OK != hErr )
		{
			HxLOG_Error("can't start tune!\n");
			goto ERR_FUNC;
		}

		xmgr_rec_SetState(pstContext,eMgrRecState_TUNE);
	}

	/* setup */
	{
		hErr = xmgr_rec_SetupMetaInfo (pstContext, pstMetaInfo);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("can't setup stMetaInfo!\n");
			goto ERR_FUNC;
		}

		if( TRUE != bSCR2DESC  )
		{
			HBOOL	bLock = FALSE;

			bLock = xmgr_rec_IsSvcLocked (pstSetup->hCurrSvc);
#if defined(CONFIG_APCORE_IP_EPG)
#if !defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
			// lock priority: series lock > svc lock
			if ((eMgrRecType_SBR == pstSetup->eRecType) && (TRUE == pstMetaInfo->bSeriesRec))
			{
				xmgrRecSeriesLock_e eSeriesLock;
				eSeriesLock = xmgr_rec_SeriesRecCheckLock (pstSetup->hCurrSvc, (HCHAR *)&(pstMetaInfo->szSeriesId));
				switch (eSeriesLock)
				{
					// serise lock true
					case eXmgrRecSeriesLock_True:
					case eXmgrRecSeriesLock_Some:
						bLock = TRUE;
						break;

					// series lock false
					case eXmgrRecSeriesLock_False:
						bLock = FALSE;
						break;

					// series lock unknown
					case eXmgrRecSeriesLock_Init:
					default:
						// follow svc lock
						break;
				}
				if(bLock == TRUE)	{pstMetaInfo->bUserLocked = TRUE;}
			}
#endif
#endif
			if(bLock == TRUE)	pstMetaInfo->u1cLocked = TRUE;
			else				pstMetaInfo->u1cLocked = FALSE;
		}

		HxSTD_MemSet(&stSvcRecSetup, 0, sizeof(SvcRec_Setup_t));
		hErr = MGR_RECORDER_SetRecordSetup (&pstContext->stSetup, &stSvcRecSetup, pstMetaInfo, pstContext->hAction);
		if( ERR_OK != hErr )
		{
			HxLOG_Error("MGR_RECORDER_SetRecordSetup. Error \n");
			goto ERR_FUNC;
		}

#if defined(CONFIG_APCORE_IP_EPG)
// TODO: 추후 파일이 분리되면수정 요망...
/* init a recorder (add meta) */
		stSeriesRecInfo.bSeriesRec = pstMetaInfo->bSeriesRec;
		HxSTD_MemCopy(stSeriesRecInfo.szSeriesId, pstMetaInfo->szSeriesId, EPG_TVA_CRID_TOTAL_LEN);
		stSeriesRecInfo.ulEpisodeNum = pstMetaInfo->ulEpisodeNum;
		stSeriesRecInfo.ulSeason = pstMetaInfo->ulSeason;
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		HxSTD_MemCopy(&stSeriesRecInfo.stCrid,&pstMetaInfo->stCrid,sizeof(DxPVR_TvaCrid_t)* DxMAX_CRID_DATA_NUM);
#endif
		hErr = xmgr_rec_AddMeta (&stSvcRecSetup, &pstContext->ulContentId, &stSeriesRecInfo);
		if( ERR_OK != hErr )
		{
			HxLOG_Error("xmgr_rec_AddMeta. Error\n");
			goto ERR_FUNC;
		}
#else
		/* init a recorder (add meta) */
		hErr = xmgr_rec_AddMeta (&stSvcRecSetup, &pstContext->ulContentId, NULL);
		if( ERR_OK != hErr )
		{
			HxLOG_Error("xmgr_rec_AddMeta. Error\n");
			goto ERR_FUNC;
		}
#endif

#if defined(CONFIG_APCORE_IP_EPG)
#if defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
		// lock priority: series lock > svc lock
		if ((eMgrRecType_SBR == pstSetup->eRecType) && (TRUE == pstMetaInfo->bSeriesRec))
		{
			hErr = xmgr_rec_CheckSeriesRecordLockViaDama(pstContext->ulContentId, pstMetaInfo->szSeriesId);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("xmgr_rec_CheckSeriesRecordLockViaDama err: ulContentId(%d), szSeriesId(%s)\n", pstContext->ulContentId, pstMetaInfo->szSeriesId);
			}
		}
#endif
#endif

#if defined(CONFIG_OP_UK_DTT)
		pstMetaInfo->ulFtaCmVal = eDxUkdtt_CONTENT_MGMT_STATE_None;
#endif

		pstMetaInfo->ucRecordStatus	=	eMgrRec_STS_RECORD_START;
		pstMetaInfo->u1cEncryption	=	stSvcRecSetup.bEncryption;

		hErr = SVC_META_SetRecordInfo (pstContext->ulContentId, pstMetaInfo);
		if( ERR_OK != hErr )
		{
			HxLOG_Error("SVC_META_SetRecordInfo. Error\n");
			goto ERR_FUNC;
		}
	}

	if (TRUE == bSCR2DESC)
	{
		if (eRec_ActType_Descramble_Record == stSvcRecSetup.eRecActType)
				xmgr_rec_GetServiceInfoForDescramble (pstContext->ulContentId, pstSetup->ulSourceContentId, &stSvcRecSetup);
		else	xmgr_rec_GetServiceInfoFromSvc 		 (pstSetup->hCurrSvc, &stSvcRecSetup);

		hErr = SVC_REC_Setup(pstContext->hAction, &stSvcRecSetup, TRUE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_REC_Setup. Error\n");
			goto ERR_FUNC;

		}

		/*
		 * Recording 될 동안 ContentList 에 유지하도록 Skip Flag 를 TRUE 로 한다.
		 * VideoList 를 Show 시 기존 ContentList 를 SVC_PVR_UnloadContentsInfo 로 해제하는데, Skip Flag 를 TRUE 로 하면 해재되지 않고 유지된다. FALSE 이면 해제
		 */
		hErr = SVC_META_SetAsDeleteSkipContent (pstContext->ulContentId, TRUE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_META_SetAsDeleteSkipContent. Error\n");
		}
#if 0
		hErr = xmgr_rec_StartSI(pstContext);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("xmgr_rec_StartSI. Error\n");
		}


		hErr = xmgr_rec_StartCas(pstContext);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("xmgr_rec_StartCas. Error\n");
		}
#endif

{
		SvcMeta_Service_t stSvcInfo;
		hErr = SVC_META_GetServiceInfo (pstSetup->ulSourceContentId, &stSvcInfo);

		pstContext->stRecPidInfo.stKnown.stSI[eREC_SiPAT].usPid	=	PAT_PID;
		pstContext->stRecPidInfo.stKnown.stSI[eREC_SiPMT].usPid	=	stSvcInfo.usPmtPid;
		pstContext->stRecPidInfo.stKnown.stSI[eREC_SiPCR].usPid	=	stSvcInfo.usPcrPid;
		pstContext->stRecPidInfo.stKnown.stVideo[0].usPid		=	stSvcInfo.usVideoPid;
		pstContext->stRecPidInfo.stKnown.stAudio[0].usPid		=	stSvcInfo.usAudioPid;

		/* set pid 전 pid info 확인용 */
		xmgr_rec_DebugPrintRecPidInfo_Base(&pstContext->stRecPidInfo, (HCHAR *)__FUNCTION__, __LINE__);

		hErr = SVC_REC_SetPID( pstContext->hAction, &pstContext->stRecPidInfo, TRUE );
}

		xmgr_rec_ClearWaitFlag(pstContext,eRecWait_ALL);

		/* start the recorder */
		hErr = xmgr_rec_StartRec(pstContext);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("xmgr_rec_StartRec. Error\n");
			BUS_SendMessage (NULL, eMEVT_PVR_NOTIFY_RECORD_SETUP_FAILED, 0, eMgrRecFailed_UNKNOWN, pstSetup->ulSlot, 0);
		}

	}

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_BREAK;

ERR_FUNC:
	HxLOG_Error("Record setup failed. eFailReason=(%d) \n",eFailReason);
	if (pstSetup != NULL)
	{
		BUS_SendMessage (NULL, eMEVT_PVR_NOTIFY_RECORD_SETUP_FAILED, 0, eFailReason, pstSetup->ulSlot, 0);
	}

	return MESSAGE_BREAK;
}

BUS_Result_t xmgr_rec_MsgBusDestroy_Base (xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 				 ulContentId 	= 0;
	MgrRec_RecSetup_t		*pstRecSetup	= NULL;
	Handle_t				 hDestoryAction	= HANDLE_NULL;
	HBOOL					 bSentFinishMsg = FALSE;

	HERROR					hErr			= ERR_OK;
	MgrAction_Type_e		eActionType 	= 0;

	HBOOL					bDeleted 		= TRUE;
	HUINT32					ulActionId		= 0;

	SvcRec_RecInfo 			stLastRecInfo = { 0, };

	if (NULL == pstContext)
	{
		HxLOG_Error("NULL context \n");
		return MESSAGE_BREAK;
	}

	ulActionId		=	pstContext->ulActionId;
	ulContentId 	=	pstContext->ulContentId;
	hDestoryAction	= 	pstContext->hAction;
	pstRecSetup 	=	&pstContext->stSetup;

	if (eMgrRecState_IDLE == pstContext->eRecState)
	{
		HxLOG_Error("end idle state. ");
		goto END_FUNC;
	}

	eActionType = xmgr_rec_GetActionType( pstRecSetup );

	xmgr_rec_LogicFrontpanel (pstRecSetup->eRecType, FALSE);
	BUS_KillTimer(INSTANT_RECORD_END_TIMER_ID);
	BUS_KillTimer(RECORDING_RGBLED_DIMM_TIMER_ID);

	/* 모든 MW 리소스를 릴리즈 함. Sync로 해야 함. */
	hErr = MGR_CAS_StopService (hDestoryAction, HANDLE_NULL, eSyncMode);
	if (ERR_OK != hErr)
		HxLOG_Error ("MGR_CAS_StopService(). Error\n");

	/* stop the recorder */
	hErr = SVC_SI_StopRec (hDestoryAction, pstRecSetup->hCurrSvc, TRUE);
	if (ERR_OK != hErr)
		HxLOG_Error ("SVC_SI_StopLive(). Error\n");

#if defined(CONFIG_MW_MM_REC_WITH_EPG)
	hErr = xmgr_rec_StopEpg (pstContext);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("xmgr_rec_StopEpg err\n");
	}
#endif

	hErr = SVC_REC_GetInfo (pstContext->hAction, &stLastRecInfo);
	if (ERR_OK != hErr)
		HxLOG_Error ("SVC_REC_GetInfo(). Error\n");

	hErr = SVC_REC_Stop (hDestoryAction, TRUE);
	if (ERR_OK != hErr)
		HxLOG_Error ("SVC_REC_Stop(). Error\n");

	hErr = SVC_REC_Close (hDestoryAction, TRUE);
	if (ERR_OK != hErr)
		HxLOG_Error ("SVC_REC_Close(). Error\n");

	if (eMgrRecType_SCR2DESC != pstRecSetup->eRecType)
	{
		hErr = SVC_CH_StopTune (hDestoryAction, (HUINT16)-1, eSyncMode);
		if( ERR_OK != hErr)
			HxLOG_Error ("SVC_CH_StopTune(). Error\n");
	}

	hErr = xmgr_rec_UpdateMeta(pstContext, &bDeleted, &stLastRecInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Warning("xmgr_rec_UpdateMeta failed. \n");
	}

	BUS_SendMessage(NULL, eMEVT_PVR_NOTIFY_RECORD_STOPPED, hDestoryAction, pstRecSetup->eRecType, ulContentId, pstContext->stSetup.ulSlot);
	bSentFinishMsg = TRUE;

	if (eMgrRecType_SCR2DESC == pstRecSetup->eRecType)
	{
		/*
		 * SCR2DESC 완료시 SourceContent 삭제, 미완료시 NewContent 삭제
		 * REC Time 이 10초마다 한번 저장되므로 10초의 에누리는 감수...
		 */

		if (eMgrRec_STS_RECORDED == pstContext->stMetaInfo.ucRecordStatus
			&& pstContext->stMetaInfo.ulRecTime >= pstContext->stMetaInfo.ulOrgDuration - 10)
		{
			HCHAR szContUrl[MAX_REC_URL_LEN + 1];

			hErr = SVC_META_GetRecordUrl (pstContext->stSetup.ulSourceContentId, szContUrl, MAX_REC_URL_LEN);
			hErr = SVC_META_DeleteContents (pstContext->stSetup.ulSourceContentId);
			HxLOG_Debug ("[%s:%d] META Delete Contents!!!\n", __FUNCTION__, __LINE__);
		}
		else
		{
			bDeleted = TRUE;
		}
		BUS_SendMessage (NULL, eMEVT_PVR_PLAYBACK_STOP, SVC_PIPE_GetActionHandle(eActionId_VIEW_FIRST), 0, 0, 0);
	}

    if (TRUE == bDeleted)
    {
		HCHAR szContUrl[MAX_REC_URL_LEN + 1];

		hErr = SVC_META_GetRecordUrl (ulContentId, szContUrl, MAX_REC_URL_LEN);
		hErr = SVC_META_DeleteContents (ulContentId);

		HxLOG_Debug ("META Delete Contents!!!\n");
    }

	xmgr_rec_SetState(pstContext,eMgrRecState_IDLE);

	/* release context */
	MGR_ACTION_Release(ulActionId, eActionType);

	SVC_META_UnRegisterPvrAction(hDestoryAction);

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

END_FUNC:
	if (TRUE != bSentFinishMsg)
	{
		BUS_SendMessage(NULL, eMEVT_PVR_NOTIFY_RECORD_STOPPED, hDestoryAction, pstRecSetup->eRecType, ulContentId, pstContext->stSetup.ulSlot);
	}

	return MESSAGE_BREAK;
}

BUS_Result_t xmgr_rec_MsgBusTimer_Base (xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
 	HERROR hErr	=	ERR_FAIL;
 	HINT32 nTimerID = 0 ;

	/* Instant Recording 의 Endtime 처리*/
	nTimerID = p1;

	switch(nTimerID)
	{
		case INSTANT_RECORD_END_TIMER_ID:
			{
				SvcMeta_Record_t stMetaInfo;

				hErr = SVC_META_GetRecordInfo (pstContext->ulContentId, &stMetaInfo);
				if (ERR_OK != hErr)
				{
					HxLOG_Error ("SVC_META_GetRecordInfo. Error\n");
					return MESSAGE_PASS;
				}

				HxLOG_Info("INSTANT_RECORD_END_TIMER_ID OK Finish Record \n");
				BUS_MGR_Destroy(0);
			}
			return MESSAGE_BREAK;

		case PANEL_STRING_TIMER_ID:
			/*clean frontpanel*/
			SVC_SYS_ClearPanelString(ePANEL_STR_PRIORITY_REC_ACTION);
			BUS_KillTimer(PANEL_STRING_TIMER_ID);
			return MESSAGE_BREAK;

		case CIPLUS_RATING_REC_STOP_TIMER_ID:
			HxLOG_Print("CIPLUS_RATING_REC_STOP_TIMER_ID: \n");
			BUS_KillTimer(CIPLUS_RATING_REC_STOP_TIMER_ID);
			BUS_MGR_Destroy(0);
			return MESSAGE_BREAK;

		case RECORDING_RGBLED_DIMM_TIMER_ID:
			if( MGR_ACTION_GetState() != eAmState_ACTIVE_STANDBY )
			{
				(void)SVC_SYS_SetRGBLedDimmingLevel(eDxLED_ID_RGB_REC, eDxRGBLED_DIMMLEVEL_DIMMED);
			}
			BUS_KillTimer(RECORDING_RGBLED_DIMM_TIMER_ID);
			return MESSAGE_BREAK;

		default:
			break;
	}

	NOT_USED_PARAM(pstContext);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t  xmgr_rec_MsgBusGoStandby_Base (xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	/* TSR_REC 정상 종료*/
	if (eMgrRecType_TSR == pstContext->stSetup.eRecType)
	{
		BUS_SendMessage (NULL, eMEVT_PVR_PLAYBACK_STOP, HANDLE_NULL, 0, 0, 0);
		BUS_MGR_Destroy(0);

		HxLOG_Warning ("!!!!! TSR-STOP !!!!! (GoStandBy)\n");
	}

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_PASS;
}

BUS_Result_t xmgr_rec_MsgMgrRecordChangeTsrRecToDelayedRecord_Base (xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32						ulSlot = (HUINT32)p1;
	xmgrRec_Context_t			*pstBackupstContext;
	SvcMeta_Record_t			*pstBackupRecInfo;
	HERROR						hErr = ERR_FAIL;
	MgrRec_EndReasonType_e 		eFailReason = eMgrRecFailed_NOFAIL;

	// 실패시를 위한 Context 와 setup Backup이 필요:
	pstBackupstContext = (xmgrRec_Context_t *)OxAP_Malloc(sizeof(xmgrRec_Context_t));
	pstBackupRecInfo = (SvcMeta_Record_t *)OxAP_Malloc(sizeof(SvcMeta_Record_t));
	if ((NULL == pstBackupstContext) || (NULL == pstBackupRecInfo))
	{
		if (NULL != pstBackupstContext)	{ OxAP_Free (pstBackupstContext); }
		if (NULL != pstBackupRecInfo)	{ OxAP_Free (pstBackupRecInfo); }
		return MESSAGE_PASS;
	}

	HxSTD_MemCopy (pstBackupstContext, pstContext, sizeof(xmgrRec_Context_t));
	HxSTD_MemCopy (pstBackupRecInfo, &pstContext->stMetaInfo, sizeof(SvcMeta_Record_t));

	hErr = xmgr_rec_ChangeRecTSRToDelayed (pstContext, ulSlot, &eFailReason);
	if (ERR_OK != hErr)
	{
		// 실패시에는 TSR 유지이므로 상태 그대로...
		HxSTD_MemCopy (pstContext, pstBackupstContext, sizeof(xmgrRec_Context_t));
		HxSTD_MemCopy (&pstContext->stMetaInfo, pstBackupRecInfo, sizeof(SvcMeta_Record_t));
	}

	BUS_PostMessage(NULL, eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD_RESULT, pstContext->hAction, (HINT32)ulSlot, (HINT32)hErr, (HINT32)eFailReason);

	OxAP_Free (pstBackupstContext);
	OxAP_Free (pstBackupRecInfo);
	return MESSAGE_PASS;
}

BUS_Result_t xmgr_rec_MsgMgrRecordEndtimeUpdate_Base(xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			hErr 		= 	ERR_OK;
	HUINT32 		ulEndTime	=	(HUINT32)p1;

	HUINT32				ulContentId 	=	0;
	SvcMeta_Record_t	*pstMetaInfo	=	NULL;

	if (NULL == pstContext)
		return MESSAGE_PASS;

	if( (pstContext->stSetup.eRecType == eMgrRecType_TSR) ||
		(pstContext->stSetup.eRecType == eMgrRecType_SCR2DESC) ||
		(pstContext->stSetup.eRecType == eMgrRecType_UNKNOWN) )
	{
		return MESSAGE_BREAK;
	}

	{
		ulContentId = pstContext->ulContentId;
		pstMetaInfo = &(pstContext->stMetaInfo);
	}

	hErr = SVC_META_GetRecordInfo (ulContentId, pstMetaInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_META_GetRecordInfo. Error \n");
		return MESSAGE_BREAK;
	}

	/*update context*/
	pstMetaInfo->ulEndTime = ulEndTime;

	if( (pstContext->stSetup.eRecType == eMgrRecType_INSTANT) ||
		(pstContext->stSetup.eRecType == eMgrRecType_DELAYED) ||
		(pstContext->stSetup.eRecType == eMgrRecType_FULLTSREC))
	{
		HUINT32		ulCurrTime	= 0;
		HINT32		nTick		= 0;

		if(VK_CLOCK_GetTime((HULONG*)&ulCurrTime) != VK_OK)
		{
			HxLOG_Error(": Time was not set \n");
		}
		HxLOG_Info("ulEndTime=%u ulCurrTime=%u\n", ulEndTime, ulCurrTime);

		if( ulEndTime > ulCurrTime )
		{
			nTick = (HINT32)(ulEndTime - ulCurrTime);
			if (nTick <= 0)
			{
				return ERR_FAIL;
			}

#if	defined(CONFIG_REMOVE_RECORD_MARGIN)
			//	Record Margin을 삭제 하였으므로 Recording End Time을 변경해야함.
			//	REC_D_CFT_MARGIN_TIME, REC_EDIT_MARGIN_TIME, RECORD_MARGIN_ENDTIME
			//	3초 마이너스...
			nTick -= 3;
#endif
			/*time restart*/
			BUS_KillTimer(INSTANT_RECORD_END_TIMER_ID);
			BUS_SetTimer(INSTANT_RECORD_END_TIMER_ID, nTick*SECOND);


			/* update meta */
			pstMetaInfo->ulEndTime		= ulEndTime;
			pstMetaInfo->bSeriesRec	= FALSE;
			pstMetaInfo->u2cEventBase	= 0;
			pstMetaInfo->ulEpisodeNum	= 0;
			pstMetaInfo->ulSeason		= 0;
			HxSTD_MemSet(pstMetaInfo->szSeriesId, 0, sizeof(HUINT8)*EPG_TVA_CRID_TOTAL_LEN);
		}
	}
	else
	{
		pstMetaInfo->ulEndTime = ulEndTime;
	}


	SVC_META_SetRecordInfo (ulContentId, pstMetaInfo);

	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_BREAK;
}

STATIC void xmgr_rec_FreeAitPidList (void *pvData)
{
#if defined(CONFIG_MW_SI_AIT)
	HUINT32		*pulAitPid = (HUINT32 *)pvData;

	if (NULL != pulAitPid)
	{
		HLIB_STD_MemFree(pulAitPid);
	}
#endif

	return;
}

BUS_Result_t xmgr_rec_MsgEvtSiPmtReceived_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR 						 hErr;
	DbSvc_Info_t				*pstSvcInfo = NULL, *pstOrgSvcInfo = NULL, *pstDstSvcInfo = NULL;
	SvcMeta_Service_t			*pstMetaSvc = NULL, *pstOrgMetaSvc = NULL;
	SvcRec_Setup_t				*pstSvcRecSetup = NULL;
	HBOOL						 bChangedVideoCodec = FALSE, bChangedPid = FALSE;
	HUINT16 					 usSvcId = 0;
	SvcMeta_SbtlInfoList_t		 stSbtlInfoList = {0, };
	SvcSi_SubtitleList_t		 stSubtitleList = {0, };

	HxLOG_Trace();

	pstSvcInfo = (DbSvc_Info_t *)OxMGR_Calloc(sizeof(DbSvc_Info_t));
	pstOrgSvcInfo = (DbSvc_Info_t *)OxMGR_Calloc(sizeof(DbSvc_Info_t));
	pstDstSvcInfo = (DbSvc_Info_t *)OxMGR_Calloc(sizeof(DbSvc_Info_t));
	pstMetaSvc = (SvcMeta_Service_t *)OxMGR_Calloc(sizeof(SvcMeta_Service_t));
	pstOrgMetaSvc = (SvcMeta_Service_t *)OxMGR_Calloc(sizeof(SvcMeta_Service_t));
	pstSvcRecSetup = (SvcRec_Setup_t *)OxMGR_Calloc(sizeof(SvcRec_Setup_t));

	if (!pstSvcInfo || !pstOrgSvcInfo || !pstDstSvcInfo || !pstMetaSvc || !pstOrgMetaSvc || !pstSvcRecSetup)
	{
		HxLOG_Error("memory allocation failed:\n");
		goto END_FUNC;
	}


	// xmgr_live_view_base.c XMGR_LiveView_BASE_EvtSiPmt 에서 먼저 업데이트 해주면 좋겠지만, 타이밍적으로 eSEVT_SI_PMT 이벤트를 여기서 먼저 받을 수 있어,
	// 한번 긁도록 한다...다만, Recording Module에서 SVC DB를 업데이트하는 건 조큼 이상한 듯 하여, Live 쪽에서 처리하고 여기선 단지 가져와서 레코딩 관련 처리만 한다...
	hErr = DB_SVC_GetServiceInfo(pstContext->stSetup.hCurrSvc, pstOrgSvcInfo);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Fail to Get Service Info (0x%x)\n", pstContext->stSetup.hCurrSvc);
		goto END_FUNC;
	}

	hErr = SVC_META_GetServiceInfo(pstContext->ulContentId, pstMetaSvc);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("SVC_META_GetServiceInfo err: usSvcId(%d)\n", pstContext->ulContentId);
	}

	HxSTD_MemCopy(pstOrgMetaSvc, pstMetaSvc, sizeof(SvcMeta_Service_t));
	HxSTD_MemCopy(&stSbtlInfoList, &(pstMetaSvc->stSbtlInfoList), sizeof(SvcMeta_SbtlInfoList_t));

	/*update Video Codec*/
	hErr = DB_SVC_GetServiceIdFromSvcHandle (pstContext->stSetup.hCurrSvc, &usSvcId);
	if (hErr == ERR_OK)
	{
		hErr = SVC_SI_GetLiveServiceInfoPid (pstContext->hAction, (HUINT32)usSvcId, pstOrgSvcInfo, pstDstSvcInfo, &bChangedPid);

		HxLOG_Info("pstOrgSvcInfo!=pstDstSvcInfo,  bChangedPid: %d \n", bChangedPid);
		HxLOG_Info("org video PID: 0x%x, audio PID: 0x%x, usTtxPid: 0x%x,  ucSubttlIdx: 0x%x,  usPcrPid: 0x%x\n",
					DbSvc_GetVideoPid(pstOrgSvcInfo), DbSvc_GetAudioPid(pstOrgSvcInfo), DbSvc_GetTtxPid(pstOrgSvcInfo), DbSvc_GetSubttlIdx(pstOrgSvcInfo), DbSvc_GetPcrPid(pstOrgSvcInfo));

		HxLOG_Info("dst video PID: 0x%x, audio PID: 0x%x, usTtxPid: 0x%x,  ucSubttlIdx: 0x%x,  usPcrPid: 0x%x\n",
					DbSvc_GetVideoPid(pstDstSvcInfo), DbSvc_GetAudioPid(pstDstSvcInfo), DbSvc_GetTtxPid(pstDstSvcInfo), DbSvc_GetSubttlIdx(pstDstSvcInfo), DbSvc_GetPcrPid(pstDstSvcInfo));
		HxLOG_Debug("VideoCodec Org(%d), Dst(%d) \n", DbSvc_GetVideoCodec(pstOrgSvcInfo), DbSvc_GetVideoCodec(pstDstSvcInfo));

		if (ERR_OK == hErr)
		{
			DxVideoCodec_e eOrgVideoCodec;
			eOrgVideoCodec = DbSvc_GetVideoCodec(pstOrgSvcInfo);
			if (DbSvc_GetVideoPid(pstDstSvcInfo) == PID_NULL)
			{
				/* ignore codec-changed caused by removal of video pid	*/
				DbSvc_SetVideoCodec(pstDstSvcInfo, eOrgVideoCodec);
			}

			HxLOG_Debug("VideoCodec Meta Org(%d), Dst(%d) \n", pstMetaSvc->eVideoCodec, DbSvc_GetVideoCodec(pstDstSvcInfo));
			if (pstMetaSvc->eVideoCodec != DbSvc_GetVideoCodec(pstDstSvcInfo))
			{
				HxLOG_Debug("VideoCodec Chaneged !\n");
				bChangedVideoCodec = TRUE;
			}
		}
		else
		{
			HxLOG_Error ("SVC_SI_GetLiveServiceInfoPid err: usSvcId(%d)\n", usSvcId);
		}
	}
	else
	{
		HxLOG_Error("DB_SVC_GetServiceIdFromSvcHandle() err \n");
	}

	if (TRUE == bChangedVideoCodec)
	{
		if (FALSE != pstContext->bPMTValid)
		{
			HxSTD_MemSet(pstSvcRecSetup, 0, sizeof(SvcRec_Setup_t));
			MGR_RECORDER_SetRecordSetup(&pstContext->stSetup, pstSvcRecSetup, &pstContext->stMetaInfo, pstContext->hAction);
			pstSvcRecSetup->eSvcType = DbSvc_GetSvcType(pstDstSvcInfo);
			pstSvcRecSetup->eVideoCodec = DbSvc_GetVideoCodec(pstDstSvcInfo);
			hErr = SVC_REC_UpdateVideoCodec(pstContext->hAction, pstSvcRecSetup, TRUE);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("SVC_REC_UpdateVideoCodec. Error\n");
			}
		}

		HxSTD_MemCopy (pstSvcInfo, pstDstSvcInfo, sizeof(DbSvc_Info_t));
	}
	else if ((DbSvc_GetVideoPid(pstOrgSvcInfo) != DbSvc_GetVideoPid(pstDstSvcInfo)) ||
			(DbSvc_GetPcrPid(pstOrgSvcInfo) != DbSvc_GetPcrPid(pstDstSvcInfo)))
	{
		HxSTD_MemCopy (pstSvcInfo, pstDstSvcInfo, sizeof(DbSvc_Info_t));
	}
	else
	{
		HxSTD_MemCopy (pstSvcInfo, pstOrgSvcInfo, sizeof(DbSvc_Info_t));
	}

	/* set pids */
	hErr = xmgr_rec_SetupPids(pstContext, pstSvcInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("xmgr_rec_SetupPids failed. \n");
	}

	/* set pid 전 pid info 확인용 */
	xmgr_rec_DebugPrintRecPidInfo_Base(&pstContext->stRecPidInfo, (HCHAR *)__FUNCTION__, __LINE__);

	/* do call setpid on first PMT (first setpid will be called in SetupRec()*/
	if (pstContext->bPMTValid == TRUE)
	{
		hErr = SVC_REC_SetPID(pstContext->hAction, &pstContext->stRecPidInfo, TRUE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_REC_SetPID err:\n");
		}
	}

	hErr = SVC_SI_GetSubtitleList (pstContext->ulActionId, DbSvc_GetSvcId(pstSvcInfo), &stSubtitleList);
	if (ERR_OK == hErr)
	{
		xmgr_rec_SetSubtitleInfo(&stSubtitleList,&stSbtlInfoList);
		SVC_SI_FreeSubtitleList(&stSubtitleList);	//free
	}

	/* save meta */
	(void)SVC_META_MakeMetaSvcBySvcInfo(pstSvcInfo, pstMetaSvc);
	HxSTD_MemCopy(&(pstMetaSvc->stSbtlInfoList), &stSbtlInfoList, sizeof(SvcMeta_SbtlInfoList_t));

	/* DB changed or First PMT */
	if ((HxSTD_MemCmp(pstMetaSvc, pstOrgMetaSvc, sizeof(SvcMeta_Service_t)) != 0)
		|| (pstContext->bPMTValid == FALSE))
	{
		HxLOG_Info("Service Changed: Meta Write...\n");

		hErr = xmgr_rec_SetServiceTimeStamp(pstContext, pstMetaSvc, TRUE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("xmgr_rec_SetServiceTimeStamp err:\n");
		}
	}

	/* xmgr_rec_EvtPvtPmtChanged */
	{
		SvcCas_SetPidInfo_t stCasPid;
		SvcCas_ActionType_e eActType;
		SvcCas_ChannelType_e eCasType = 0;

		HxSTD_MemSet(&stCasPid, 0, sizeof(SvcCas_SetPidInfo_t));

		if(pstContext->stSetup.eRecType == eMgrRecType_SCR2DESC)
		{
			SVC_CAS_SetRecPid(pstContext->hAction, eCAS_ActionType_Descramble_a_File, &stCasPid);

			HxLOG_Trace();
			goto END_FUNC;
		}

		/* Record/Tsr_Rec 는 PMT 에 존재하는 모든 pid 들을 CAS에서 건다. pid 를 알려줄 필요 없이 CAS단에서 구하도록 변경 필요*/
		if(pstContext->stSetup.eRecType == eMgrRecType_TSR) eActType = eCAS_ActionType_Tsr_Rec;
		else										eActType = eCAS_ActionType_Recording;

		hErr = SVC_CAS_GetChannelType(pstContext->hAction, &eCasType);
		if(hErr == ERR_OK && eCasType == eCasChannel_FTA)
		{
#if defined(CONFIG_OP_SES)
			/* ses는 FTA Channel인 경우에도 drm 정보를 운용할수 있다. */
#else
			xmgr_rec_ClearWaitFlag(pstContext, eRecWait_DRM);
#endif
		}

		SVC_CAS_SetRecPid(pstContext->hAction, eActType, &stCasPid);
	}
	xmgr_rec_ClearWaitFlag(pstContext, eRecWait_PMT);

	/* save flag */
	if (FALSE == pstContext->bPMTValid)
	{
		pstContext->bPMTValid = TRUE;
	}

END_FUNC:
	if (pstSvcInfo)						{ OxMGR_Free(pstSvcInfo); }
	if (pstOrgSvcInfo)					{ OxMGR_Free(pstOrgSvcInfo); }
	if (pstDstSvcInfo)					{ OxMGR_Free(pstDstSvcInfo); }
	if (pstMetaSvc)						{ OxMGR_Free(pstMetaSvc); }
	if (pstOrgMetaSvc)					{ OxMGR_Free(pstOrgMetaSvc); }
	if (pstSvcRecSetup)					{ OxMGR_Free(pstSvcRecSetup); }

	return MESSAGE_PASS;
}

BUS_Result_t xmgr_rec_MsgEvtSiPmtTimeout_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR	hErr = ERR_OK;
	HxLOG_Trace();

	if (eMgrRecState_NO_SIGNAL == pstContext->eRecState)
	{
		return MESSAGE_PASS;
	}

	if(pstContext->stSetup.eRecType != eMgrRecType_TSR)
	{
		/* PAT/PMT Timeout, therefore recording destroy */
		hErr = MGR_RECORDER_SetRecordFailReason(pstContext->ulContentId, eMgrRecFailed_SI_TIMEOUT);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("MGR_RECORDER_SetRecordFailReason failed. cid=(%d) \n",pstContext->ulContentId);
		}
	}

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_rec_MsgEvtSiPmtPidChanged_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			hErr = ERR_OK;

	{
		DbSvc_Info_t stSvcInfo;
		HUINT16 usPmtPid;

		HxSTD_MemSet(&stSvcInfo, 0, sizeof(DbSvc_Info_t));
		hErr = DB_SVC_GetServiceInfo(pstContext->stSetup.hCurrSvc, &stSvcInfo);
		if (ERR_OK != hErr)
			return hErr;

		SVC_SI_GetPmtPidBySvcId(pstContext->hAction, DbSvc_GetSvcId(&stSvcInfo), &usPmtPid);

		DbSvc_SetPmtPid(&stSvcInfo, usPmtPid);
		DbSvc_SetPcrPid(&stSvcInfo, PID_NULL);
		DbSvc_SetTtxPid(&stSvcInfo, PID_NULL);
		DbSvc_SetVideoPid(&stSvcInfo, PID_NULL);
		DbSvc_SetAudioPid(&stSvcInfo, PID_NULL);
		DbSvc_SetDolbyPid(&stSvcInfo,PID_NULL);
		DbSvc_SetAudioAuxPid(&stSvcInfo, PID_NULL);
		{
			SvcMeta_Service_t stMetaSvc;
			HxSTD_MemSet (&stMetaSvc, 0, sizeof(SvcMeta_Service_t));

			hErr = SVC_META_GetServiceInfo (pstContext->ulContentId, &stMetaSvc);
			if (ERR_OK == hErr)
			{
				stMetaSvc.usPmtPid = DbSvc_GetPmtPid(&stSvcInfo);
				hErr = SVC_META_SetServiceInfo (pstContext->ulContentId, &stMetaSvc, TRUE);
				if (ERR_OK != hErr)
				{
					HxLOG_Error ("[%s:%d] SVC_META_SetServiceInfo. Error \n");
				}
			}
		}
		DB_SVC_UpdateServiceInfo(pstContext->stSetup.hCurrSvc, &stSvcInfo);
	}

	/* module stop */
	{
		/* cas stop */
		hErr = MGR_CAS_StopService(pstContext->hAction, HANDLE_NULL, eSyncMode);
		if( ERR_OK != hErr )
		{
			HxLOG_Error("Critical Error!!!\n");
			return MESSAGE_PASS;
		}

		/* SI stop */
		hErr = SVC_SI_StopRec (pstContext->hAction, pstContext->stSetup.hCurrSvc, TRUE);
		if( ERR_OK != hErr )
		{
			HxLOG_Error("Critical Error!!!\n");
			return MESSAGE_PASS;
		}
	}

	hErr = xmgr_rec_StartSI (pstContext);
	if( ERR_OK != hErr )
	{
		HxLOG_Error("Critical Error!!!\n");
		return MESSAGE_PASS;
	}

	/* for wating PMT */
	hErr = xmgr_rec_StartCas(pstContext);
	if( ERR_OK != hErr )
	{
		HxLOG_Error("Critical Error!!!\n");
		return MESSAGE_PASS;
	}

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_rec_MsgEvtSiCatReceived_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR hErr;
	HINT32 i, nMinTrack=0;
	HUINT32 ulActionId = pstContext->ulActionId;

	DbSvc_Info_t stSvcInfo;
	SvcSi_EmmList_t stEmmList;

	HxSTD_MemSet(&stSvcInfo, 0, sizeof(DbSvc_Info_t));
	hErr = DB_SVC_GetServiceInfo(pstContext->stSetup.hCurrSvc, &stSvcInfo);

	/*EMM*/
	HxSTD_MemSet( &stEmmList, 0, sizeof(SvcSi_EmmList_t) );
	hErr = SVC_SI_GetEmmList (ulActionId, DbSvc_GetSvcId(&stSvcInfo), &stEmmList);
	if (ERR_OK == hErr)
	{
		nMinTrack = (stEmmList.nEmm<SvcSi_MAX_CA_PID_NUM)? stEmmList.nEmm : SvcSi_MAX_CA_PID_NUM;
		for( i=0 ; i< nMinTrack ; i++ )
			pstContext->stRecPidInfo.stKnown.stEmm[i].usPid = stEmmList.pstEmmInfo[i].usPid;

		SVC_SI_FreeEmmList(&stEmmList); //free
	}

	return MESSAGE_PASS;
}

BUS_Result_t xmgr_rec_MsgEvtSiEitReceived_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Trace();

#if 0
	if(pstContext->stRecPidInfo.stKnown.stSI[eREC_SiPMT].usPid == PID_NULL)
	{
		HxLOG_Error("PMT PID is Null. Eit PID Can not Set.\n");
		return ERR_FAIL;
	}
#endif

	/* startpid eit-pid */
	{
		HERROR hErr = ERR_FAIL;
		pstContext->stRecPidInfo.stKnown.stSI[eREC_SiEIT].usPid	=	EIT_PID;

		/* set pid 전 pid info 확인용 */
		xmgr_rec_DebugPrintRecPidInfo_Base(&pstContext->stRecPidInfo, (HCHAR *)__FUNCTION__, __LINE__);

		hErr = SVC_REC_SetPID( pstContext->hAction, &pstContext->stRecPidInfo, TRUE );
		if( ERR_OK != hErr )
		{
			HxLOG_Error ("SVC_REC_StartPID hErr: hAction(%d)\n", pstContext->hAction);
		}
	}

	/* save event meta */
	{
		HERROR hErr = ERR_FAIL;
		DbSvc_Info_t	stSvcInfo;

		HxSTD_MemSet(&stSvcInfo, 0, sizeof(DbSvc_Info_t));
		hErr = DB_SVC_GetServiceInfo(pstContext->stSetup.hCurrSvc, &stSvcInfo);
		if (ERR_OK == hErr)
		{
			SvcSi_Event_t		stEvtInfo;
			SvcSi_ExtEvent_t	stExtEvtInfo;

			DbSvc_TripleId_t  stTripleId;

			HINT32				i;
			HBOOL				bLock = FALSE;
			HINT32				nParentalSetup	=	0;
			DxDeliveryType_e	eOrgDeliType	=	eDxSVC_TYPE_All;
			HBOOL				bMetaUpdate_u1cLocked = FALSE;

			stTripleId.usOnId	= DbSvc_GetOnId(&stSvcInfo);
			stTripleId.usTsId	= DbSvc_GetTsId(&stSvcInfo);
			stTripleId.usSvcId	= DbSvc_GetSvcId(&stSvcInfo);
			eOrgDeliType 		= DbSvc_GetDeliType(stSvcInfo);

			HxSTD_MemSet(&stEvtInfo, 0, sizeof(SvcSi_Event_t) );
			HxSTD_MemSet(&stExtEvtInfo, 0, sizeof(SvcSi_ExtEvent_t) );

			hErr = SVC_SI_GetEvtInfoFromPfEvent(pstContext->ulActionId, stTripleId, TRUE, TRUE, &stEvtInfo, &stExtEvtInfo);
			if (ERR_OK == hErr)
			{
				SvcMeta_Event_t stMetaEvt;
				HBOOL bSave = TRUE;
				HINT32	nExtTextLen = 0;
				HxCountry_e 	eCountryId;
				HUINT8		szIso3166Code[LANG_639_CODE_MAX+1];

				HxSTD_MemSet(&stMetaEvt, 0, sizeof(SvcMeta_Event_t) );
				stMetaEvt.pstExtEvt = NULL;

				/* convert si_eitinfo to pvr_meta */
				{
					stMetaEvt.nEventId				=	stEvtInfo.nEvtId;
					stMetaEvt.ulStartTime			=	stEvtInfo.ulStartTime;
					stMetaEvt.ulDuration			=	stEvtInfo.ulDuration;
					stMetaEvt.u3RunnigStatus		=	stEvtInfo.ucRunStatus;

					/* ParentRating */
					{
						HUINT32 ucParentalRatingNum = 0;
//						HUINT32 ulRatingListCnt = HLIB_LIST_Length(stEvtInfo.pstRatingDescList);
						HxList_t *pstRatingItem = NULL;

						SvcMeta_Rating_t *pDestMetaRating = &stMetaEvt.astParentalRating[0];

						for (pstRatingItem = stEvtInfo.pstRatingDescList ; NULL != pstRatingItem ; pstRatingItem = pstRatingItem->next)
						{
							SIxParentDes_t	*pstDesc = (SIxParentDes_t	*)HLIB_LIST_Data(pstRatingItem);
							SIxParentInfo_t	*pstParentInfo = NULL;

							if (NULL != pstDesc)
							{
								pstParentInfo = pstDesc->pstInfo;
								while (pstParentInfo != NULL)
								{
									if (PVR_PARENTAL_RATING_MAX <= ucParentalRatingNum)
									{
										HxLOG_Warning ("can't fully make metafile (EIT : ParentRatingList : not match list-size)\n");
										break;
									}

									//get data
									HxSTD_MemCopy ( pDestMetaRating->aucIsoCountryCode,
													pstParentInfo->szCountryCode,
													sizeof(pstParentInfo->szCountryCode));

									HLIB_STD_StrLower(pDestMetaRating->aucIsoCountryCode);
									pDestMetaRating->ucParentalRating = pstParentInfo->ucRate;

									//next
									pstParentInfo = pstParentInfo->pstNext;
									ucParentalRatingNum++;
									pDestMetaRating++;
								}
							}
						}

						stMetaEvt.ucParentalRatingNum = ucParentalRatingNum;
#if defined(CONFIG_DEBUG)
/*Print ParentRatingList*/
{
	HUINT32 i;
	HxLOG_Debug ("*****[EIT:ParentRatingList]*****\n");
	for (i=0 ; i<stMetaEvt.ucParentalRatingNum ; i++)
	{
		HxLOG_Debug ("[Lan:%s,Rate:%d]\n", stMetaEvt.astParentalRating[i].aucIsoCountryCode, stMetaEvt.astParentalRating[i].ucParentalRating);
	}
}
#endif
					}

					stMetaEvt.ucContentNibble		=	stEvtInfo.ucContent;
					stMetaEvt.ucUserNibble			=	stEvtInfo.ucContent2nd;
					stMetaEvt.u1FreeCaMode			=	stEvtInfo.ulCasType;
					stMetaEvt.pNext					=	NULL;//(struct tagPVR_EVT *)stEvtInfo.pNext;

					HxSTD_MemCopy(stMetaEvt.aucEventName, stEvtInfo.szName, PVR_SHORT_EVENT_NAME_LEN);
					HxSTD_MemCopy(stMetaEvt.aucEventText, stEvtInfo.szText, PVR_EVENT_TEXT);

					hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32 *)&eCountryId, 0);
					if (ERR_OK != hErr)
					{
						HxLOG_Warning ("DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID) Error!!\n");
					}

					hErr = xmgr_rec_UtilGetCountryCode(eCountryId, szIso3166Code);
					if (ERR_OK != hErr)
					{
						HxLOG_Error ("xmgr_rec_UtilGetCountryCode. Error!!\n");
					}

					for (i = 0; (i < stMetaEvt.ucParentalRatingNum) && (i < PVR_PARENTAL_RATING_MAX); i++)
					{
						if (MWC_UTIL_DvbStrncmp(stMetaEvt.astParentalRating[i].aucIsoCountryCode, szIso3166Code, LANG_639_CODE_MAX) == 0)
						{
							nParentalSetup	= MGR_PG_GetRatingFromSetup();
							bLock			= MGR_PG_IsRatingOk(	pstContext->ulActionId,
																	pstContext->stSetup.hCurrSvc, eOrgDeliType,
																	(DxRatingAge_e)nParentalSetup,
																	(DxRatingAge_e)(stMetaEvt.astParentalRating[i].ucParentalRating));
							if(FALSE == bLock)
								bMetaUpdate_u1cLocked = TRUE;

							//add current parental rating and event id for Delayed recording
							//xmgr_rec_AddEvtInfoToEvtList(pstContext, stEvtInfo.nEventId, stEvtInfo.astParentalRating[i].ucParentalRating, stEvtInfo.ulStartTime);
							break;
						}
					}

					//	stMetaEvt.ucReserved
					//	stMetaEvt.stComponent[EPG_COMPONENT_DESC_MAX];
					//	stMetaEvt.ulOffsetOfPlayTime;
					//	stMetaEvt.pstExtEvt;
					//		stEvtInfo.ulVidComponentStatus;
					//		stEvtInfo.ulAudComponentStatus;
					//		stEvtInfo.ulAuxComponentStatus;
					//		stEvtInfo.bExtEvtInfo;
					//		stEvtInfo.bComponent;

					if ( (TRUE == stEvtInfo.bExtEvtInfo) || (HxSTD_StrEmpty(pstContext->stMetaInfo.szLongText) == FALSE) )
					{
						stMetaEvt.pstExtEvt	=	(SvcMeta_ExtEvt_t*)OxMGR_Calloc(sizeof(SvcMeta_ExtEvt_t));
						stMetaEvt.pstExtEvt->pszChar	=	NULL;
						stMetaEvt.pstExtEvt->pNext		=	NULL;

//						stMetaEvt.pstExtEvt->ucDesNum;
//						stMetaEvt.pstExtEvt->ucLastDesNum;
//						stMetaEvt.pstExtEvt->ucItemNum;

						HxSTD_MemCopy(stMetaEvt.pstExtEvt->szIso639LangCode, stExtEvtInfo.aucLang, 4);

						if (NULL != stExtEvtInfo.szTextChar && HxSTD_StrEmpty(pstContext->stMetaInfo.szLongText))
						{
							nExtTextLen = HLIB_STD_StrNLen(stExtEvtInfo.szTextChar, DxRECLIST_EXT_TEXT_BUFSIZE);

							if (nExtTextLen > 0 )
							{
								if(nExtTextLen == DxRECLIST_EXT_TEXT_BUFSIZE)
								{
										// 잘라먹는건 어쩔 수 없음...
										stMetaEvt.pstExtEvt->pszChar	=		(HUINT8*)OxMGR_Calloc( nExtTextLen);
										HxSTD_MemSet(stMetaEvt.pstExtEvt->pszChar, 0, nExtTextLen);
										HxSTD_MemCopy(stMetaEvt.pstExtEvt->pszChar, stExtEvtInfo.szTextChar, nExtTextLen);
										stMetaEvt.pstExtEvt->pszChar[nExtTextLen-1] = '\0';
								}
								else
								{
										stMetaEvt.pstExtEvt->pszChar	=		(HUINT8*)OxMGR_Calloc( nExtTextLen+1 );
										HxSTD_MemSet(stMetaEvt.pstExtEvt->pszChar, 0, nExtTextLen+1);
										HxSTD_MemCopy(stMetaEvt.pstExtEvt->pszChar, stExtEvtInfo.szTextChar, nExtTextLen);
										stMetaEvt.pstExtEvt->pszChar[nExtTextLen] = '\0';
								}
							}
						}

						if(HxSTD_StrEmpty(pstContext->stMetaInfo.szLongText) == FALSE)
						{
							nExtTextLen = HLIB_STD_StrNLen(pstContext->stMetaInfo.szLongText, DxRECLIST_EXT_TEXT_BUFSIZE);

							if(nExtTextLen == DxRECLIST_EXT_TEXT_BUFSIZE)
							{
									// 잘라먹는건 어쩔 수 없음...
									stMetaEvt.pstExtEvt->pszChar	=		(HUINT8*)OxMGR_Calloc(nExtTextLen);
									HxSTD_MemSet(stMetaEvt.pstExtEvt->pszChar, 0, nExtTextLen);
									HxSTD_MemCopy(stMetaEvt.pstExtEvt->pszChar,pstContext->stMetaInfo.szLongText,nExtTextLen);
									stMetaEvt.pstExtEvt->pszChar[nExtTextLen-1] = '\0';
							}
							else
							{
									stMetaEvt.pstExtEvt->pszChar	=		(HUINT8*)OxMGR_Calloc(nExtTextLen+1);
									HxSTD_MemSet(stMetaEvt.pstExtEvt->pszChar, 0, nExtTextLen+1);
									HxSTD_MemCopy(stMetaEvt.pstExtEvt->pszChar,pstContext->stMetaInfo.szLongText,nExtTextLen);
									stMetaEvt.pstExtEvt->pszChar[nExtTextLen] = '\0';
							}
						}
					}
				}

				if( bMetaUpdate_u1cLocked == TRUE )
				{
					/*Recording 정보 Update*/
					pstContext->stMetaInfo.u1cLocked = 1;
					SVC_META_SetRecordInfo (pstContext->ulContentId, &pstContext->stMetaInfo);
				}

				SVC_META_SetEventInfo( pstContext->ulContentId, &stMetaEvt, bSave );

				/* timestamp */
				{
					SvcRec_RecInfo stMetaInfo;
					HxSTD_MemSet (&stMetaInfo, 0, sizeof(SvcRec_RecInfo));

					hErr = SVC_REC_GetInfo (hAction, &stMetaInfo);

					SVC_META_SetEventTimeStamp(pstContext->ulContentId, stMetaInfo.ulDuration, &stMetaEvt);
				}

				SVC_SI_FreeExtEventInfo(&stExtEvtInfo);

				if (NULL != stMetaEvt.pstExtEvt )
				{
					if (NULL != stMetaEvt.pstExtEvt->pszChar)
						OxMGR_Free (stMetaEvt.pstExtEvt->pszChar);

						OxMGR_Free (stMetaEvt.pstExtEvt);
				}

			}

			if (SVC_SI_FreeEventInfo(&stEvtInfo) != ERR_OK)
			{
				HxLOG_Error("SVC_SI_FreeEventInfo() failed..! \n");
			}
		}


	}

#if defined(CONFIG_OP_SES)
	xmgr_rec_SetMeta_u1cLocked(pstContext);
#endif

	return ERR_OK;
}

BUS_Result_t xmgr_rec_MsgEvtSiEitTimeout_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMgrRecState_NO_SIGNAL == pstContext->eRecState)
	{
		return MESSAGE_PASS;
	}


#if defined(CONFIG_NO_PARENTAL_CONTROL_ON_RADIO_SVC) && defined(CONFIG_SET_LOCK_FOR_NO_RATENTAL_CONTROL_INFO)
	{
		HERROR	hErr	=	ERR_FAIL;
		DxDeliveryType_e	eOrgDeliType = 0;

		/*get eOrgDeliType*/
		DbSvc_Info_t	stSvcInfo;
		HxSTD_MemSet(&stSvcInfo, 0, sizeof(DbSvc_Info_t));
		hErr = DB_SVC_GetServiceInfo(pstContext->stSetup.hCurrSvc, &stSvcInfo);

		eOrgDeliType = DbSvc_GetDeliType(stSvcInfo);

		/* ziggo spec 에서는 radio 모드는 pc 를 체크하지 않는다고 한다.*/
		if(eOrgDeliType == eDxSVC_TYPE_RADIO)
		{
			return ERR_BUS_NO_ERROR;
		}

		/* ziggo spec 에서는 eit timeout 시 lock 이라고 한다. */
		if( pstContext->stMetaInfo.u1cLocked != 1 )
		{
			pstContext->stMetaInfo.u1cLocked = 1;
			SVC_META_SetRecordInfo (pstContext->ulContentId, &pstContext->stMetaInfo);
		}
	}
#endif

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_rec_MsgEvtSiMultifeedDetected_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	/* There are changes in the multi-feed information. p1 : Changed(new or disappeared) service handle */
	/* Multifeed 가 Dectection 됨, 레코딩 중인 채널이 없어지면 recording stop 함. */
	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_rec_MsgEvtSiSdtFreeCaModeChanged_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	/* SDT의 free CA mode 변경으로 replacement service로 변경해야 함. */

	NOT_USED_PARAM(pstContext);
	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_rec_MsgEvtSiSdtRunningStatusChanged_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	/* SDT의 running status 변경으로 replacement service로 변경해야 함. */

	NOT_USED_PARAM(pstContext);
	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_rec_MsgEvtChLocked_Base (xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR					hErr 		= ERR_OK;

	MgrRec_RecordType_e 	eRecType 	= 0;
	MgrRec_RecSetup_t		*pstSetup	= NULL;

	if (NULL == pstContext)
		return MESSAGE_PASS;

	/* set local value */
	eRecType					= pstContext->stSetup.eRecType;
	pstSetup					= &(pstContext->stSetup);

	switch(pstContext->eRecState)
	{
		case eMgrRecState_TUNE :
			{
				SvcRec_Setup_t stSvcRecSetup;

				HxSTD_MemSet(&stSvcRecSetup, 0, sizeof(SvcRec_Setup_t));
				hErr = MGR_RECORDER_SetRecordSetup(&pstContext->stSetup, &stSvcRecSetup, &pstContext->stMetaInfo, pstContext->hAction);
				if (hErr != ERR_OK)
				{
					HxLOG_Error("MGR_RECORDER_SetRecordSetup. Error\n");
				}

				if (eRec_ActType_Descramble_Record == stSvcRecSetup.eRecActType)
				{
					xmgr_rec_GetServiceInfoForDescramble (pstContext->ulContentId, pstSetup->ulSourceContentId, &stSvcRecSetup);

					hErr = SVC_REC_Setup(pstContext->hAction, &stSvcRecSetup, TRUE);
					if (ERR_OK != hErr)
					{
						HxLOG_Error("SVC_REC_Setup. Error\n");
					}
				}
				else if (eRec_ActType_FullTs == stSvcRecSetup.eRecActType)
				{
					stSvcRecSetup.eSvcType		=	eDxSVC_TYPE_TV;
					stSvcRecSetup.bEncryption	=	FALSE;
					stSvcRecSetup.eVideoCodec	=	eDxVIDEO_CODEC_MPEG2;
					stSvcRecSetup.ulStartTime	=	0;

					hErr = SVC_REC_Setup(pstContext->hAction, &stSvcRecSetup, TRUE);
					if (ERR_OK != hErr)
					{
						HxLOG_Error("SVC_REC_Setup. Error\n");
					}
				}
#if defined(CONFIG_PROD_ICORDPRO)
				else
				{
					xmgr_rec_GetServiceInfoFromSvc (pstContext->stSetup.hCurrSvc, &stSvcRecSetup);

					hErr = SVC_REC_Setup(pstContext->hAction, &stSvcRecSetup, TRUE);
					if (ERR_OK != hErr)
					{
						HxLOG_Error("SVC_REC_Setup. Error\n");
					}
				}
#endif

				hErr = SVC_META_SetAsDeleteSkipContent (pstContext->ulContentId, TRUE);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("SVC_META_SetAsDeleteSkipContent. Error\n");
				}

				hErr = xmgr_rec_StartSI(pstContext);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("xmgr_rec_StartSI. Error\n");
				}

				hErr = xmgr_rec_StartCas(pstContext);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("xmgr_rec_StartCas. Error\n");
				}

#if defined(CONFIG_MW_MM_REC_WITH_EPG)
				hErr = xmgr_rec_StartEpg (pstContext);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("EPG failed!\n");
				}
#endif
				xmgr_rec_SetState(pstContext, eMgrRecState_WAIT);

				if ((eMgrRecType_SCR2DESC == eRecType) || (eMgrRecType_FULLTSREC == eRecType))
				{
					xmgr_rec_ClearWaitFlag(pstContext, eRecWait_ALL);
				}

				if (eMgrRecType_FULLTSREC == pstContext->stSetup.eRecType)
				{
					SvcRec_PIDList_t						  stRecPidInfo;

					SVC_REC_CleanReqPidInfo(eRec_PidSetType_Unknown, &stRecPidInfo);

					//ALL_PID
					stRecPidInfo.ePidSetType = eRec_PidSetType_All;

					hErr = SVC_REC_SetPID(pstContext->hAction, &stRecPidInfo, TRUE);
					if (ERR_OK != hErr)
					{
						HxLOG_Error("SVC_REC_SetPID failed in Full TS Rec...\n");
					}

					/* start the recorder */
					hErr = xmgr_rec_StartRec(pstContext);
					if (ERR_OK != hErr)
					{
						HxLOG_Error("xmgr_rec_StartRec. Error\n");
						BUS_SendMessage (NULL, eMEVT_PVR_NOTIFY_RECORD_SETUP_FAILED, 0, eMgrRecFailed_UNKNOWN, pstSetup->ulSlot, 0);
					}
				}

				return MESSAGE_PASS;
			}
		case eMgrRecState_WAIT:
			hErr = xmgr_rec_StartCas(pstContext);
			break;

		case eMgrRecState_NO_SIGNAL:
			hErr = xmgr_rec_StartCas(pstContext);
			xmgr_rec_SetState(pstContext,eMgrRecState_RECORDING);
			break;

		case eMgrRecState_BLOCKED:
			hErr = xmgr_rec_StartCas(pstContext);
			break;

		default:
			break;
	}

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_PASS;
}

BUS_Result_t xmgr_rec_MsgEvtChNoSignal_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR	hErr = ERR_OK;

	/* status update */
	switch(pstContext->eRecState)
	{
		case eMgrRecState_RECORDING:
			xmgr_rec_SetState(pstContext, eMgrRecState_NO_SIGNAL);
			break;
		case eMgrRecState_TUNE:
		case eMgrRecState_WAIT:
		default:
			break;
	}

	/* svc stop */
	hErr = MGR_CAS_StopService(pstContext->hAction, HANDLE_NULL, eSyncMode);
	if( ERR_OK != hErr )
	{
		HxLOG_Error("can't call func'MGR_CAS_StopService()' ret(%d)!\n", hErr);
		return MESSAGE_PASS;
	}

	/* flag update */
	MGR_RECORDER_SetRecordFailReason(pstContext->ulContentId, eMgrRecFailed_NOSIGNAL);
	BUS_SendMessage (NULL, eMEVT_PVR_NOTIFY_RECORD_SETUP_FAILED, 0, eMgrRecFailed_NOSIGNAL, pstContext->stSetup.ulSlot, 0);

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_PASS;
}

BUS_Result_t xmgr_rec_MsgEvtChMovingMotor_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR hErr = ERR_OK;

	/* status update */
	switch(pstContext->eRecState)
	{
		case eMgrRecState_RECORDING:
			xmgr_rec_SetState(pstContext, eMgrRecState_NO_SIGNAL);
			break;
		case eMgrRecState_TUNE:
		case eMgrRecState_WAIT:
		default:
			break;
	}

	/* svc stop */
	hErr = MGR_CAS_StopService(pstContext->hAction, HANDLE_NULL, eSyncMode);
	if( ERR_OK != hErr )
	{
		HxLOG_Error("can't call func'MGR_CAS_StopService()' ret(%d)!\n", hErr);
		return MESSAGE_BREAK;
	}

	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_BREAK;
}

BUS_Result_t xmgr_rec_MsgEvtDbCheckSvcDb_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NOT_USED_PARAM(p1);
	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_rec_MsgEvtRecPidChanged_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR hErr	=	ERR_FAIL;
	HUINT32 ulRecTime = (HUINT32)p1;

	HxLOG_Trace();

	{
		SvcMeta_Service_t stMetaSvc;
		HxSTD_MemSet(&stMetaSvc, 0, sizeof(SvcMeta_Service_t));

		hErr = SVC_META_GetServiceInfo (pstContext->ulContentId, &stMetaSvc);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_META_GetServiceInfo. Error\n");
			return hErr;
		}


		SVC_META_SetServiceTimeStamp( pstContext->ulContentId, ulRecTime, &stMetaSvc );

		/* if chase playback */
		{
			Handle_t hPBAction;
			hErr = SVC_META_GetPlaybackActionByContentId( pstContext->ulContentId, &hPBAction );
/*
			if (ERR_OK == hErr)
			{
				SVC_PVRUTIL_UpdateTimeStamp( hPBAction );
			}
*/
		}
	}

	return MESSAGE_BREAK;
}

BUS_Result_t xmgr_rec_MsgEvtRecTimecodeChange_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 ulStartTime=0, ulEndTime=0;
	ulStartTime =	p2;
	ulEndTime	=	p1; //recording

	if(pstContext->stSetup.eRecType == eMgrRecType_TSR)
	{
		HUINT32		ulDuration = MGR_RECORDER_GetRecorderParam(eMgrRec_DEFAULT_DURATION_TSR_RECORD);

		if(ulEndTime == 0xFFFFFFFF)
		{
			HxLOG_Error ("pstContext->stMetaInfo.ulEndTime:[%d] problem...\n", pstContext->stMetaInfo.ulEndTime);
			return MESSAGE_PASS;
		}

		/* DI 에서 값이 잘못 올라올 가능성 대비*/
		if( (pstContext->stMetaInfo.ulEndTime + 10) <= ulEndTime)
		{
			if (pstContext->stMetaInfo.ulEndTime == 0xFFFFFFFF)
			{
				HxLOG_Error ("pstContext->stMetaInfo.ulEndTime:[%d] problem...\n", pstContext->stMetaInfo.ulEndTime);
				pstContext->stMetaInfo.ulEndTime = ulEndTime;
			}
			//HxLOG_Error ("!!! Time is very strange ~~~~~ hAction=%x ulStartTime=%d ulEndTime=%d\n", hAction, ulStartTime, ulEndTime);
			//HxLOG_Error ("!!! Time is very strange ~~~~~ hAction=%x ulStartTime=%d ulEndTime=%d\n", hAction, ulStartTime, ulEndTime);
			return MESSAGE_PASS;
		}

		/* startTime 이 linear하게 증가하지 않고 4~7초씩 jump하면서 증가
		 * 이것 때문에 web에서의 처리가 부정확해지므로
		 * startTime은 무시하고 recTime을 기준으로 web으로 전달
		 */
		pstContext->ulStartTimeFromTimeCode		= ulStartTime;

		pstContext->stMetaInfo.ulRecTime		= ulEndTime;
		pstContext->stMetaInfo.ulEndTime		= ulEndTime;
		pstContext->stMetaInfo.ulStartTime		= 0;
		pstContext->stMetaInfo.ulLastTime		= ulEndTime;

		if( pstContext->stMetaInfo.ulRecTime > ulDuration)
		{
			pstContext->stMetaInfo.ulRecTime = ulDuration;
			pstContext->stMetaInfo.ulStartTime = pstContext->stMetaInfo.ulEndTime - ulDuration;
		}
	}
	else
	{
		HERROR			hErr		= ERR_OK;
		HBOOL			bSvcLock		= FALSE;
		SvcFs_DevIdx_e 	eDevIdx		= 0;
		HUINT32			ulPartIdx	= 0;

		DbSvc_Info_t		stSvcInfo;
		SvcMeta_Event_t 	stMetaEvt;
		HINT32				i;
		HBOOL				bPCLock = FALSE;
		HINT32				nParentalSetup	=	0;
		DxDeliveryType_e 	eOrgDeliType	=	eDxSVC_TYPE_All;
		HBOOL 				bMetaUpdate_u1cLocked =	FALSE;
		HxCountry_e			eCountryId;
		HUINT8				szIso3166Code[LANG_639_CODE_MAX+1];

		HxSTD_MemSet(&stSvcInfo, 0, sizeof(DbSvc_Info_t));
		hErr = DB_SVC_GetServiceInfo(pstContext->stSetup.hCurrSvc, &stSvcInfo);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("DB_SVC_GetServiceInfo. Error\n");
			return hErr;
		}

		eOrgDeliType 		= DbSvc_GetDeliType(stSvcInfo);

		hErr = SVC_META_GetRecordInfo(pstContext->ulContentId, &pstContext->stMetaInfo);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_META_GetRecordInfo. Error\n");
			return hErr;
		}

		pstContext->stMetaInfo.ulRecTime = ulEndTime;
		pstContext->stMetaInfo.ulLastTime = ulEndTime;
		
		if(hErr == ERR_OK)
		{
#if defined(CONFIG_APCORE_IP_EPG)
			if(pstContext->stMetaInfo.bSeriesRec == FALSE)
#endif
			if(pstContext->stMetaInfo.u1cLocked != 1)
			{
				if( eMgrRecType_SCR2DESC != pstContext->stSetup.eRecType )
				{
					bSvcLock = xmgr_rec_IsSvcLocked(pstContext->stSetup.hCurrSvc);
					if(bSvcLock == TRUE)
					{
						pstContext->stMetaInfo.u1cLocked = 1;
						SVC_META_SetRecordInfo (pstContext->ulContentId, &pstContext->stMetaInfo);
					}

					hErr = SVC_META_GetEventInfo(pstContext->ulContentId, &stMetaEvt);
					if(hErr == ERR_OK)
					{
						hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32 *)&eCountryId, 0);
						if (ERR_OK != hErr)
						{
							HxLOG_Warning ("DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID) Error!!\n");
						}

						hErr = xmgr_rec_UtilGetCountryCode(eCountryId, szIso3166Code);
						if (ERR_OK != hErr)
						{
							HxLOG_Error ("xmgr_rec_UtilGetCountryCode. Error!!\n");
						}

						for (i = 0; (i < stMetaEvt.ucParentalRatingNum) && (i < PVR_PARENTAL_RATING_MAX); i++)
						{
							if (MWC_UTIL_DvbStrncmp(stMetaEvt.astParentalRating[i].aucIsoCountryCode, szIso3166Code, LANG_639_CODE_MAX) == 0)
							{
								nParentalSetup	= MGR_PG_GetRatingFromSetup();
								bPCLock 		= MGR_PG_IsRatingOk(	pstContext->ulActionId,
																		pstContext->stSetup.hCurrSvc, eOrgDeliType,
																		(DxRatingAge_e)nParentalSetup,
																		(DxRatingAge_e)(stMetaEvt.astParentalRating[i].ucParentalRating));
								if(FALSE == bPCLock)
									bMetaUpdate_u1cLocked = TRUE;

								//add current parental rating and event id for Delayed recording
								//xmgr_rec_AddEvtInfoToEvtList(pstContext, stEvtInfo.nEventId, stEvtInfo.astParentalRating[i].ucParentalRating, stEvtInfo.ulStartTime);
								break;
							}
						}
						if( bMetaUpdate_u1cLocked == TRUE )
						{
							/*Recording 정보 Update*/
							pstContext->stMetaInfo.u1cLocked = 1;
							SVC_META_SetRecordInfo (pstContext->ulContentId, &pstContext->stMetaInfo);
						}
					}
				}
			}

			if( (pstContext->stMetaInfo.ucRecordStatus == eMgrRec_STS_RECORD_START) &&
				(pstContext->stMetaInfo.ulRecTime >= MGR_RECORDER_GetRecorderParam(eMgrRec_MIN_REQUIRED_RECORD_TIME)) )
			{
				UNIXTIME	utCurrTime;
				utCurrTime = HLIB_STD_GetSystemTime();
				pstContext->stMetaInfo.ulStartTime = utCurrTime - pstContext->stMetaInfo.ulRecTime;

				pstContext->stMetaInfo.ucRecordStatus = eMgrRec_STS_RECORDING;
				hErr = SVC_META_SetRecordInfo (pstContext->ulContentId, &pstContext->stMetaInfo);
				if (ERR_OK == hErr)
				{
					BUS_SendMessage (NULL, eMEVT_PVR_MEDIALIST_UPDATE, pstContext->hAction, pstContext->stSetup.eRecType, pstContext->ulContentId, 0);
				}
			}

			/* HDD 의 CheckFullRecordStorage 를 체크한다.*/
			hErr = SVC_FS_GetDevPartIndexByPath(pstContext->stMetaInfo.aucPath, &eDevIdx, &ulPartIdx);
			if(ERR_OK != hErr)
			{
				HxLOG_Debug("SVC_FS_GetDevPartIndexByPath() fail \n");
				hErr = MGR_RECORDER_SetRecordFailReason(pstContext->ulContentId, eMgrRecFailed_NOHDD);
				if (ERR_OK == hErr)
				{
					BUS_SendMessage (NULL, eMEVT_PVR_NOTIFY_RECORD_SETUP_FAILED, 0, eMgrRecFailed_NOHDD, pstContext->stSetup.ulSlot, 0);
					BUS_SendMessage (NULL, eMEVT_PVR_RECORD_STOP_REC,
									SVC_PIPE_GetActionHandle (pstContext->ulActionId),
									0, 0, 0);
				}
			}
			else
			{
				if (TRUE != SVC_FS_IsDevicePvrPossible (eDevIdx))
				{
					HxLOG_Debug("SVC_FS_IsDevicePvrPossible() fail \n");
					hErr = MGR_RECORDER_SetRecordFailReason(pstContext->ulContentId, eMgrRecFailed_INCOMPLETE_HDDFULL);
					if (ERR_OK == hErr)
					{
						BUS_SendMessage (NULL, eMEVT_PVR_NOTIFY_RECORD_SETUP_FAILED, 0, eMgrRecFailed_INCOMPLETE_HDDFULL, pstContext->stSetup.ulSlot, 0);
						BUS_SendMessage (NULL, eMEVT_PVR_RECORD_STOP_REC,
									SVC_PIPE_GetActionHandle (pstContext->ulActionId),
									0, 0, 0);
					}
				}
			}

			{
				HUINT32 ulCheckTime,ulDiffTime;
				ulCheckTime= MGR_RECORDER_GetRecorderParam(eMgrRec_RECORD_DURATION_CHECK_TIME);
				ulDiffTime = pstContext->stMetaInfo.ulRecTime - pstContext->ulPlaytime;
				HxLOG_Debug("ulDiffTime[%d],rectime[%d]\n",ulDiffTime,pstContext->stMetaInfo.ulRecTime);
				if( ulDiffTime > ulCheckTime)
				{

					/* update the recording time per MGR_PVR_GetTimeParam(eMgrRec_RECORD_DURATION_CHECK_TIME) (after min rec time + first chk duration*/
					hErr = SVC_META_SetRecordInfo (pstContext->ulContentId, &pstContext->stMetaInfo);
					if (ERR_OK != hErr)
					{
						HxLOG_Error ("SVC_META_SetRecordInfo. Error\n");
					}
					else
					{
						pstContext->ulPlaytime = pstContext->stMetaInfo.ulRecTime;
					}
				}
			}
		}
	}

	HxLOG_Info ("hAction(%x) StartTimeCode=%d ulStartTime=%d ulEndTime=%d ulRecTime=%d\n",
		hAction, pstContext->ulStartTimeFromTimeCode, pstContext->stMetaInfo.ulStartTime, pstContext->stMetaInfo.ulEndTime, pstContext->stMetaInfo.ulRecTime);

	NOT_USED_PARAM(p3);

	return MESSAGE_PASS;
}

BUS_Result_t xmgr_rec_MsgEvtRecScrambled_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR hErr = ERR_FAIL;
	HBOOL	bScrambled = (HBOOL)p1;
	UNIXTIME ulCurrTime = 0;

	HxLOG_Trace();

	hErr = VK_CLOCK_GetTime((unsigned long*)&ulCurrTime) ;
	if ( VK_OK != hErr	|| HLIB_DATETIME_IsValidUnixTime(ulCurrTime) != ERR_OK )
	{
		HxLOG_Error("GetCurrentTime error...\n");
		ulCurrTime = 0;
	}

	if(pstContext->stMetaInfo.u1cScrambled == TRUE)
	{
		if(pstContext->stMetaInfo.ulRecTime <= 10 && bScrambled == FALSE)
		{
			// 10초 이내에 Unscramble이 올라오면....Scramble이 올라오기 전에는 Unscramble인걸로....
			HxLOG_Debug("Recording Unscrambed... ulRecTime:[%d]...\n", pstContext->stMetaInfo.ulRecTime);
		}
		else if( (ulCurrTime != 0) && (pstContext->ulScrambleStartTime + MGR_CAS_ALLOWED_SCRAMBLE_TIME > ulCurrTime) && (bScrambled == FALSE))
		{
			// Scramble 되어있는 구간이 MGR_CAS_ALLOWED_SCRAMBLE_TIME 시간보다 작다면 Scramble 구간이 없는 Recording content로 간주하도록 함.
			// 몇초의 scramble 구간으로 인해 playback 시 파일 전체를 scramble 구간으로 간주하면 User 측면에서 불편한점이 더욱 크다고 판단하여 추가된 내용.
			HxLOG_Debug("Scrambled period is less than %d. Make the reecording Unscrambed...\n", MGR_CAS_ALLOWED_SCRAMBLE_TIME);
		}
		else
		{
			// 한번 Scramble로 녹화되던 파일은 일단 Scrambled 로 간다...
			HxLOG_Debug("UNSCRAMBLED Message is Ignored... ulRecTime:[%d]\n", pstContext->stMetaInfo.ulRecTime);
			return MESSAGE_PASS;
		}
	}

	hErr = SVC_META_GetRecordInfo (pstContext->ulContentId, &pstContext->stMetaInfo);
	if (ERR_OK == hErr)
	{
		pstContext->stMetaInfo.u1cScrambled = bScrambled;

		hErr = SVC_META_SetRecordInfo (pstContext->ulContentId, &pstContext->stMetaInfo);
		if(hErr == ERR_FAIL)
		{
			HxLOG_Error("Fail to Set Record Info.\n" );
		}

		if(bScrambled == TRUE && ulCurrTime != 0)
		{
			pstContext->ulScrambleStartTime = ulCurrTime;
		}
	}

	NOT_USED_PARAM(p2);
	NOT_USED_PARAM(p3);

	return MESSAGE_PASS;
}


BUS_Result_t xmgr_rec_MsgEvtRecDelayedRecStatusChange_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if defined(CONFIG_MW_MM_DELAYED_REC)
	HUINT32 ulPercent		= p1;

	if(eMgrRec_DelayedCopy_Completed == ulPercent)
	{
		HCHAR	szTsrPath[SVC_FS_PVR_DIR_PATH_LEN];
		HERROR	hErr;

		szTsrPath[0] = '\0';
		if( SVC_FS_UseTsrOnlyDevice() == TRUE )
		{
			hErr = SVC_FS_GetTsrOnlyPath(szTsrPath);
		}
		else
		{
#if defined(CONFIG_PROD_DEVICE_2ND_INTERNAL_HDD)
			hErr = SVC_FS_GetPvrPath (pstContext->stSetup.ulPvrIdx, eSVC_FS_UsageType_Tsr, szTsrPath);
#else
			hErr = SVC_FS_GetPvrPath (SVC_FS_GetDefaultPvrIdx(), eSVC_FS_UsageType_Tsr, szTsrPath);
#endif
		}
		if (hErr != ERR_OK)
		{
			HxLOG_Error("SVC_FS_GetPvrPath failed. \n");
		}
		HxSTD_StrNCat(szTsrPath, "*", HxSTD_StrLen("*"));
		if (UAPI_FM_Remove(szTsrPath, NULL, NULL, NULL) > 0)
		{
			HxLOG_Debug("(%s) is Cleaned.\n", szTsrPath);
		}
		else
		{
			HxLOG_Error("(%s) Could not deleted. \n", szTsrPath);
		}

		if (TRUE == pstContext->bDelayedCopy)
		{
			pstContext->bDelayedCopy = FALSE;
			HxSTD_MemSet(pstContext->szDelayCopyFileName, 0, FS_MAX_FILE_NAME_LEN);
		}

		HxLOG_Warning ("********************************************************************************\n");
		HxLOG_Warning ("*  XMGR : RECORDING : FINISH DELAYED FILE COPY!!!\n");
		HxLOG_Warning ("********************************************************************************\n");
		BUS_SendMessage(NULL,eMEVT_PVR_RECORD_TSR_COPYEND,SVC_PIPE_GetActionHandle (pstContext->ulActionId),0,0,0);
	}
	else if (eMgrRec_DelayedCopy_Error== ulPercent)
	{
		HCHAR	szTsrPath[SVC_FS_PVR_DIR_PATH_LEN];
		HERROR	hErr;

		szTsrPath[0] = '\0';
		if( SVC_FS_UseTsrOnlyDevice() == TRUE )
		{
			hErr = SVC_FS_GetTsrOnlyPath(szTsrPath);
		}
		else
		{
#if defined(CONFIG_PROD_DEVICE_2ND_INTERNAL_HDD)
			hErr = SVC_FS_GetPvrPath (pstContext->stSetup.ulPvrIdx, eSVC_FS_UsageType_Tsr, szTsrPath);
#else
			hErr = SVC_FS_GetPvrPath (SVC_FS_GetDefaultPvrIdx(), eSVC_FS_UsageType_Tsr, szTsrPath);
#endif
		}
		if (hErr != ERR_OK)
		{
			HxLOG_Error("SVC_FS_GetPvrPath failed. \n");
		}

		HxSTD_StrNCat(szTsrPath, "*", HxSTD_StrLen("*"));
		if (UAPI_FM_Remove(szTsrPath, NULL, NULL, NULL) > 0)
		{
			HxLOG_Debug("(%s) is Cleaned.\n", szTsrPath);
		}
		else
		{
			HxLOG_Error("(%s) Could not deleted. \n", szTsrPath);
		}

		if (TRUE == pstContext->bDelayedCopy)
		{
			pstContext->bDelayedCopy = FALSE;
			HxSTD_MemSet(pstContext->szDelayCopyFileName, 0, FS_MAX_FILE_NAME_LEN);
		}
		HxLOG_Warning ("********************************************************************************\n");
		HxLOG_Warning ("*  XMGR : RECORDING : FINISH DELAYED FILE ERRORRRRRRRR!!!\n");
		HxLOG_Warning ("********************************************************************************\n");
		BUS_SendMessage(NULL,eMEVT_PVR_RECORD_TSR_COPYEND,SVC_PIPE_GetActionHandle (pstContext->ulActionId),0,0,0);
	}
#endif
	return MESSAGE_PASS;
}

BUS_Result_t xmgr_rec_MsgEvtRSVDBChanged_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if ( (pstContext->stSetup.eRecType == eMgrRecType_TBR) ||
		 (pstContext->stSetup.eRecType == eMgrRecType_EBR) ||
		 (pstContext->stSetup.eRecType == eMgrRecType_SBR) )
	{
		HUINT32 ulSlot = p1;

		if(pstContext->stSetup.ulSlot == ulSlot)
		{
			UNIXTIME			 utStartTime;
			DxSchedule_t		 stSchedule;
			HUINT32 			 hErr = ERR_FAIL;

			hErr = SAPI_GetSchedule (ulSlot, &stSchedule);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("SAPI_GetSchedule err: Slot(%d)\n", ulSlot);
				return MESSAGE_PASS;
			}

			(void)HLIB_DATETIME_ConvertDateTimeToUnixTime (&stSchedule.stStartTime, &utStartTime);

			{
				UNIXTIME	utModifiedEndTime	=	0;
				utModifiedEndTime = utStartTime + stSchedule.ulDuration;

				if( pstContext->stMetaInfo.ulEndTime != utModifiedEndTime )
				{
					pstContext->stMetaInfo.ulEndTime = utModifiedEndTime;

					/* 미정 이벤트의 DB를 업데이트 할 때, Meta도 따라서 바꿔줘야 한다. */
					pstContext->stMetaInfo.ulEndTime = utModifiedEndTime;

					hErr = SVC_META_SetRecordInfo (pstContext->ulContentId, &pstContext->stMetaInfo);
					if (ERR_OK != hErr)
					{
						HxLOG_Error ("SVC_META_SetRecordInfo. Error\n");
						return MESSAGE_PASS;
					}
				}
			}
		}
	}

	return MESSAGE_PASS;
}

BUS_Result_t	xmgr_rec_MsgEvtReqCommand_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR	hErr = ERR_OK;
	xmgrRec_ReqCmd_e	eCommand = p1;

	switch (eCommand)
	{
		case eRecCmd_Resume:
		{
			if (eRecPause_Start == p2)
			{
				/*pause 가 걸리지 않은 것은 start 를 하지 않은 것이고 추후에 start 하면 pause 하지 않음.*/
				pstContext->stSetup.bStartPauseMode = FALSE;
			}
			xmgr_rec_ClearPauseFlag(pstContext, p2);
			break;
		}
		case eRecCmd_Pause:
			xmgr_rec_SetPauseFlag(pstContext, p2);
			break;
		default:
			break;
	}

	if (ERR_OK != hErr)
	{
		HxLOG_Error("can't call func'xmgr_rec_Cmd!!!()' \n");
		return ERR_BUS_INITIAL_FAILED;
	}


	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_rec_MsgEvtExtStorageDisconnected_Base( xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	/* When storage on operation disconnected, destoy recorder app by its action */
//	HERROR	hErr;
	HINT32	nDevIdx = p1;
	/* check if param1(device index) is in operation actually, discard this event if not. */
	if (SVC_FS_CheckPathIsValid(nDevIdx, pstContext->stMetaInfo.aucPath) == FALSE)
	{
		HxLOG_Info("device(%d) removed, but recorder won't stop\n", nDevIdx);
		return MESSAGE_PASS;
	}

	// external PVR 에서 Recording 중 녹화중인 USB HDD 가 Disconnect 된 경우 이를 Flash DB(rsv_fail.db) 에 저장한다.
	if(pstContext->stSetup.eRecType != eMgrRecType_TSR)
	{
#if 0
		DbRsv_FailItem_t stItem = {0,};
		HCHAR uuid[PVR_EXTERN_STORAGE_UUID_LEN];

		stItem.hSvc			= pstContext->stSetup.hCurrSvc;
		stItem.ulStartTime	= pstContext->stMetaInfo.ulStartTime;
		stItem.ulDuration	= pstContext->stMetaInfo.ulRecTime;
		stItem.ulFailReason	= eMgrRecFailed_DISCONNECTUSBHDD;

		DB_SVC_GetServiceNumber(stItem.hSvc, &stItem.ulSvcNum);

		hErr = SVC_META_GetRecordInfo(pstContext->ulContentId, &pstContext->stMetaInfo);
		snprintf(stItem.szPathName, DB_RSV_FAIL_PATH_LEN, "%s%s%s", pstContext->stMetaInfo.aucPath, pstContext->stMetaInfo.aucFileName, EXT_META_FILE);

		hErr = SVC_FS_GetDeviceUuid ((SvcFs_DevIdx_e)nDevIdx, uuid);
		snprintf(stItem.szUUID, DB_RSV_FAIL_UUID_LEN, "%s", uuid);
		MGR_PVR_RecAddRsvFailInfo(&stItem, TRUE);

		pstContext->stMetaInfo.ucEndCondition = eMgrRecFailed_DISCONNECTUSBHDD;
		SVC_META_SetRecordInfoWithoutMeta(pstContext->ulContentId, &pstContext->stMetaInfo);
		BUS_SendMessage (NULL, eMEVT_PVR_NOTIFY_RECORD_SETUP_FAILED, 0, eMgrRecFailed_DISCONNECTUSBHDD, pstContext->stSetup.ulSlot, 0);
#endif
	}

	return MESSAGE_PASS;
}

BUS_Result_t xmgr_rec_MsgEvtPvrRecSeriesLockFound_Base (xmgrRec_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
	HUINT32			 ulContentId = (HUINT32)p1;
	HINT32			 nLockedContent = p2;
	HERROR			 hErr;

	if ((ulContentId == pstContext->ulContentId) &&
		(eMgrRecType_SBR == pstContext->stSetup.eRecType) &&
		(TRUE == pstContext->stMetaInfo.bSeriesRec))
	{
		HBOOL				 bUserLocked = (nLockedContent > 0) ? TRUE : FALSE;

		if (pstContext->stMetaInfo.bUserLocked != bUserLocked)
		{
			pstContext->stMetaInfo.bUserLocked = (nLockedContent > 0) ? TRUE : FALSE;

			hErr = SVC_META_SetRecordInfo(pstContext->ulContentId, &(pstContext->stMetaInfo));
			if (ERR_OK != hErr)
			{
				HxLOG_Error("SVC_META_SetRecordInfo err: contentsId (%d)\n", pstContext->ulContentId);
			}
		}

		return MESSAGE_BREAK;
	}
#endif

	return MESSAGE_PASS;
}

#define _____PROTO_PROCEDURE_____
STATIC BUS_Result_t xmgr_rec_ProtoProc (xmgrRec_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eBusResult = ERR_BUS_NO_ERROR;
	HUINT32			ulActionId = SVC_PIPE_GetActionId(hAction);

	/* PRE-CASPROC */
	eBusResult = xmgr_rec_PreCasProtoProc (pstContext, message, hAction, p1, p2, p3);
	if (ERR_BUS_NO_ERROR != eBusResult)
		return eBusResult;

	switch (message)
	{
		/*MSG*/
		case eMEVT_BUS_READY_SHUTDOWN :
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eMEVT_BUS_READY_SHUTDOWN\n", ulActionId);
			eBusResult = xmgr_rec_MsgBusGoStandby_Base(pstContext, hAction, p1, p2, p3);
			return MESSAGE_PASS;

		case eMEVT_BUS_CREATE:
			HxLOG_Print ("********************************************************************************\n");
			HxLOG_Print ("*  HSSEO XMGR : RECORDING : CREATE \n");
			HxLOG_Print ("********************************************************************************\n");

			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eMEVT_BUS_CREATE\n", ulActionId);
			eBusResult = xmgr_rec_MsgBusCreate_Base(pstContext, hAction, p1, p2, p3);

			/* update status to svc_meta */
			{
				HERROR hErr;
				hErr = SVC_META_RegisterPvrAction(pstContext->hAction, pstContext->ulContentId, eSvcMeta_UsingActType_Record);
				if (ERR_OK != hErr)
				{
					HxLOG_Error("SVC_META_RegisterPvrAction. Error\n");
				}
			}

			break;

		case eMEVT_BUS_DESTROY:
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eMEVT_BUS_DESTROY\n", ulActionId);
			eBusResult = xmgr_rec_MsgBusDestroy_Base(pstContext, hAction, p1, p2, p3);

			HxLOG_Print ("********************************************************************************\n");
			HxLOG_Print ("*  XMGR : RECORDING : DESTROY \n");
			HxLOG_Print ("********************************************************************************\n");
			break;

		case eMEVT_BUS_TIMER:
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eMEVT_BUS_TIMER\n", ulActionId);
			eBusResult = xmgr_rec_MsgBusTimer_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eMEVT_PVR_RECORD_CHANGE_TSR_REC_TO_DELAYED_RECORD\n", ulActionId);
			eBusResult = xmgr_rec_MsgMgrRecordChangeTsrRecToDelayedRecord_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_PVR_RECORD_ENDTIME_UPDATE :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eMEVT_PVR_RECORD_ENDTIME_UPDATE\n", ulActionId);
			eBusResult = xmgr_rec_MsgMgrRecordEndtimeUpdate_Base(pstContext, hAction, p1, p2, p3);
			break;

		/*EVT-SI*/
		case eSEVT_SI_PMT:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_SI_PMT\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiPmtReceived_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_PAT_TIMEOUT :
		case eSEVT_SI_PMT_TIMEOUT :
		case eSEVT_SI_PMT_PIDREMOVED:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_SI_PAT_TIMEOUT\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiPmtTimeout_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_PMT_PIDCHANGED :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_SI_PMT_PIDCHANGED\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiPmtPidChanged_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_CAT:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_SI_CAT\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiCatReceived_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_EIT:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_SI_EIT\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiEitReceived_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_EIT_TIMEOUT:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_SI_EIT_TIMEOUT\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiEitTimeout_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_MULTIFEED_DETECTED :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_SI_MULTIFEED_DETECTED\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiMultifeedDetected_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_SDT_FREE_CA_MODE_CHANGED :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_SI_SDT_FREE_CA_MODE_CHANGED\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiSdtFreeCaModeChanged_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_SDT_RUNNING_STATUS_CHANGED :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_SI_SDT_RUNNING_STATUS_CHANGED\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtSiSdtRunningStatusChanged_Base(pstContext, hAction, p1, p2, p3);
			break;

		/*EVT-CH*/
		case eSEVT_CH_NO_SIGNAL :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_CH_NO_SIGNAL\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtChNoSignal_Base(pstContext, hAction, p1, p2, p3);
			break;
		case eSEVT_CH_MOVING_MOTOR :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_CH_MOVING_MOTOR\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtChMovingMotor_Base(pstContext, hAction, p1, p2, p3);
			break;
		case eSEVT_CH_LOCKED :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_CH_LOCKED\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtChLocked_Base(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_STORAGE_NOTIFY_UNPLUGGED:
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : EVT_FS_NOTIFY_USB_DISCONNECTED\n", ulActionId);
			if (p2 == FALSE)	// Ext Hdd
			{
				eBusResult = xmgr_rec_MsgEvtExtStorageDisconnected_Base (pstContext, hAction, p1, p2, p3);
			}
			break;

		/* EVT-DB */
		case eSEVT_DB_CHECK_SVC_DB :
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_DB_CHECK_SVC_DB\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtDbCheckSvcDb_Base (pstContext, hAction, p1, p2, p3);
			break;

		/* EVT-SVC_Rec */
		case eSEVT_REC_PID_CHANGED:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_REC_PID_CHANGED\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtRecPidChanged_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_REC_TIMECODE_CHANGE :
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Info("[XMGR:Rec(%d)] MESSAGE : eSEVT_REC_TIMECODE_CHANGE\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtRecTimecodeChange_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_REC_SCRAMBLED:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_REC_SCRAMBLED\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtRecScrambled_Base (pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_REC_DELAYED_RECORD_STATUS_CHANGE:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_REC_DELAYED_RECORD_STATUS_CHANGE\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtRecDelayedRecStatusChange_Base (pstContext, hAction, p1, p2, p3);
			break;

		/* EVT-Xmgr_Rec */
		case eSEVT_REC_REQ_COMMAND:
			CHECK_REC_ACTION_HANDLE();
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eSEVT_REC_REQ_COMMAND\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtReqCommand_Base (pstContext, hAction, p1, p2, p3);
			break;

#if defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
		case eMEVT_PVR_RECORD_SERIES_LOCK_FOUND:
			HxLOG_Debug("[XMGR:Rec(%d)] MESSAGE : eMEVT_PVR_RECORD_SERIES_LOCK_FOUND\n", ulActionId);
			eBusResult = xmgr_rec_MsgEvtPvrRecSeriesLockFound_Base (pstContext, hAction, p1, p2, p3);
			break;
#endif

		default :
			break;
	}

	HxLOG_Info("RunState message(0x%x) hAction(0x%x) p1(0x%x) p2(0x%x) p3(0x%x) \n",message, hAction, p1, p2, p3);
	xmgr_rec_RunState(pstContext);

	if(eBusResult != ERR_BUS_NO_ERROR)
	{
		return eBusResult;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_rec_Proc0 (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (0);

	if (message == eMEVT_BUS_CREATE)
	{
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}
	return xmgr_rec_ProtoProc (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_rec_Proc1 (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (1);
	if (message == eMEVT_BUS_CREATE)
	{
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return xmgr_rec_ProtoProc (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_rec_Proc2 (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (2);
	if (message == eMEVT_BUS_CREATE)
	{
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return xmgr_rec_ProtoProc (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_rec_Proc3 (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (3);
	if (message == eMEVT_BUS_CREATE)
	{
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return xmgr_rec_ProtoProc (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_rec_Proc4 (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (4);
	if (message == eMEVT_BUS_CREATE)
	{
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return xmgr_rec_ProtoProc (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_rec_Proc5 (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (5);
	if (message == eMEVT_BUS_CREATE)
	{
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return xmgr_rec_ProtoProc (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_rec_Proc6 (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (6);
	if (message == eMEVT_BUS_CREATE)
	{
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return xmgr_rec_ProtoProc (pstContext, message, hAction, p1, p2, p3);
}

STATIC BUS_Result_t xmgr_rec_Proc7 (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	xmgrRec_Context_t		*pstContext = xmgr_rec_GetContext (7);
	if (message == eMEVT_BUS_CREATE)
	{
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return xmgr_rec_ProtoProc (pstContext, message, hAction, p1, p2, p3);
}

#define _________PLUGIN_FUNC__________
HERROR xmgr_recorder_Init_Base(void)
{
	xmgr_rec_InitAllContext();

	return ERR_OK;
}

HCHAR*	xmgr_recorder_GetProcName_Base (HUINT32 ulActionId)
{
	if ((ulActionId >= eActionId_REC_FIRST) && (ulActionId <= eActionId_REC_LAST))
	{
		switch (ulActionId)
		{
		case (eActionId_REC_FIRST + 0):
			return "xmgr_rec_Proc0";

		case (eActionId_REC_FIRST + 1):
			return "xmgr_rec_Proc1";

		case (eActionId_REC_FIRST + 2):
			return "xmgr_rec_Proc2";

		case (eActionId_REC_FIRST + 3):
			return "xmgr_rec_Proc3";

		case (eActionId_REC_FIRST + 4):
			return "xmgr_rec_Proc4";

		case (eActionId_REC_FIRST + 5):
			return "xmgr_rec_Proc5";

		case (eActionId_REC_FIRST + 6):
			return "xmgr_rec_Proc6";

		case (eActionId_REC_FIRST + 7):
			return "xmgr_rec_Proc7";

		default:
			break;
		}
	}

	return NULL;
}

BUS_Callback_t xmgr_recorder_GetProcFunc_Base (HUINT32 ulActionId)
{
	// Code 내에서 Record 갯수가 최대 몇개 인지 알기 힘드므로
	// switch 함수 위에 ulActionID 영역을 확인하여 실제로 넘치는 값이어도 logic상 들어오지 않게 만든다.
	if ((ulActionId >= eActionId_REC_FIRST) && (ulActionId <= eActionId_REC_LAST))
	{
		switch (ulActionId)
		{
		case (eActionId_REC_FIRST + 0):
			return xmgr_rec_Proc0;

		case (eActionId_REC_FIRST + 1):
			return xmgr_rec_Proc1;

		case (eActionId_REC_FIRST + 2):
			return xmgr_rec_Proc2;

		case (eActionId_REC_FIRST + 3):
			return xmgr_rec_Proc3;

		case (eActionId_REC_FIRST + 4):
			return xmgr_rec_Proc4;

		case (eActionId_REC_FIRST + 5):
			return xmgr_rec_Proc5;

		case (eActionId_REC_FIRST + 6):
			return xmgr_rec_Proc6;

		case (eActionId_REC_FIRST + 7):
			return xmgr_rec_Proc7;

		default:
			break;
		}
	}

	return NULL;
}

HUINT32 xmgr_recorder_GetRecorderParam_Base (MgrRec_ParamType_e eType)
{
	switch (eType)
	{
	case eMgrRec_MIN_REQUIRED_RECORD_TIME:			return MIN_REQUIRED_RECORD_TIMES;
	case eMgrRec_RECORD_DURATION_CHECK_TIME:		return RECORD_DURATION_CHECK_TIME;
	case eMgrRec_DEFAULT_DURATION_TSR_RECORD:
	{
		// TSR Duration 은 Storage Type 이 Usbmem일 경우 가변적으로 변함
		return s_ulTsrDurationTime;//DEFAULT_DURATION_TSR_RECORD;
	}
	default:									break;
	}

	return 0;
}

HERROR xmgr_recorder_SetRecorderParam_Base (MgrRec_ParamType_e eType, HUINT32 ulParam)
{
	switch (eType)
	{
	case eMgrRec_DEFAULT_DURATION_TSR_RECORD:
	{
		HxLOG_Info ("Change RecorderParma 'DEFAULT_DURATION_TSR_RECORD' (%d) -> (%d)\n", s_ulTsrDurationTime, ulParam);
		s_ulTsrDurationTime	=	(HUINT32)ulParam;
		return ERR_OK;
	}
	case eMgrRec_MIN_REQUIRED_RECORD_TIME:
	case eMgrRec_RECORD_DURATION_CHECK_TIME:
	default:									break;
	}

	return ERR_FAIL;
}

HERROR xmgr_recorder_SetRecSetup_Base (HINT32 nRecIdx, MgrRec_RecSetup_t *pstRecSetup)
{
	(void)xmgr_recorder_GetTsrLimitForUsbFlash_Base (&(pstRecSetup->stUsbFlash));

	if( (nRecIdx >= NUM_MAX_REC_ACTION) || (NULL == pstRecSetup) )
	{
		HxLOG_Error("incorrect params(nRecIdx:%d)(pstRecSetup:0x%x)\n", nRecIdx, pstRecSetup);
		return ERR_FAIL;
	}

	/* copy setup info to context */
	{
		xmgrRec_Context_t *pstContext = xmgr_rec_GetContext(nRecIdx);

		if( NULL == pstContext )
		{
			HxLOG_Error("can't find context by nRecIdx(%d)\n", nRecIdx);
			return ERR_FAIL;
		}

		if( eMgrRecState_IDLE != pstContext->eRecState )
		{
			HxLOG_Error("already alloc rec-context by nRecIdx(%d)\n", nRecIdx);
			return ERR_FAIL;
		}

		xmgr_rec_ClearContext(pstContext);
		HxSTD_MemCopy(&pstContext->stSetup, pstRecSetup, sizeof(MgrRec_RecSetup_t));
	}

	return ERR_OK;
}

HERROR xmgr_recorder_SetRecordFailReason_Base (HUINT32 ulContentId, MgrRec_EndReasonType_e eEndReason)
{
	/* Recording 도중에 hErr 가 발생한 것을 meta file 에 update 한다. */
	HERROR	hErr	=	ERR_FAIL;
	xmgrRec_Context_t * pstContext = xmgr_rec_GetContextByContentId(ulContentId);

	if( NULL != pstContext )
	{
		HxSTD_MemSet(&pstContext->stMetaInfo, 0, sizeof(SvcMeta_Record_t));
		hErr = SVC_META_GetRecordInfo(ulContentId, &pstContext->stMetaInfo);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("[%s:%] SVC_META_GetRecordInfo. Error\n", __FUNCTION__, __LINE__);
			return hErr;
		}

		pstContext->stMetaInfo.ucEndCondition = (HUINT8)eEndReason;
		hErr = SVC_META_SetRecordInfo(ulContentId, &pstContext->stMetaInfo);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("[%s:%] SVC_META_SetRecordInfo. Error\n", __FUNCTION__, __LINE__);
			return hErr;
		}
	}

	return ERR_OK;
}

HERROR xmgr_recorder_GetRecordFailReason_Base (HUINT32 ulContentId, MgrRec_EndReasonType_e *peEndReason)
{
	HERROR			hErr	=	ERR_FAIL;
	SvcMeta_Record_t	stMetaInfo;

	if( NULL== peEndReason )
		return ERR_FAIL;

	HxSTD_MemSet(&stMetaInfo, 0, sizeof(SvcMeta_Record_t));

	hErr = SVC_META_GetRecordInfo(ulContentId, &stMetaInfo);
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("Can not Get Record Info ...\n");
		*peEndReason = eMgrRecFailed_UNKNOWN;
	}
	else
	{
		*peEndReason = stMetaInfo.ucEndCondition ;
	}
	return hErr;
}

HERROR xmgr_recorder_CreateRecordFailReason_Base (MgrRec_RecSetup_t *pstRecSetup, MgrRec_EndReasonType_e eEndReason, DxSchedule_t *pstSchedule)
{
	HUINT32			 ulContentId;
	UNIXTIME		 utStartTime;
	HCHAR			 szFileName[FS_MAX_FILE_NAME_LEN];
	SvcMeta_Record_t	 stMetaInfo;
	HERROR			 hErr;

	if (pstSchedule == NULL || pstRecSetup == NULL)
	{
		HxLOG_Error("invalid param. schedule=(0x%x) setup=(0x%x) \n", pstSchedule, pstRecSetup);
		return ERR_FAIL;
	}

	/* Recording 이 시작하기 전에 hErr 가 발생한 것을 meta 파일로 만든다.*/
	HxLOG_Debug("eEndReason: %d \n", eEndReason);
	HxSTD_MemSet (&stMetaInfo, 0, sizeof(SvcMeta_Record_t));

	(void)HLIB_DATETIME_ConvertDateTimeToUnixTime (&(pstSchedule->stStartTime), &utStartTime);

	stMetaInfo.ulStartTime		= utStartTime - pstSchedule->uExtInfo.stRec.ulPreOffsetTime;
	stMetaInfo.ulEndTime			= utStartTime + 1;
	stMetaInfo.ucEndCondition	= (HUINT8)eEndReason;
	stMetaInfo.ucRecordStatus	= eMgrRec_STS_INCOMPLETED;
	stMetaInfo.nEventId			= pstSchedule->uExtInfo.stRec.nEvtId;

	if (eMgrRecType_SBR == pstRecSetup->eRecType
		&& pstSchedule->uExtInfo.stRec.ulSeriesId != 0)
	{
		stMetaInfo.bSeriesRec = TRUE;
		HxSTD_snprintf(stMetaInfo.szSeriesId,EPG_TVA_CRID_TOTAL_LEN,"%d",pstSchedule->uExtInfo.stRec.ulSeriesId);
	}

	{
		HBOOL	bLock = FALSE;

		bLock = xmgr_rec_IsSvcLocked (pstRecSetup->hCurrSvc);
#if defined(CONFIG_APCORE_IP_EPG) && !defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
		// lock priority: series lock > svc lock
		if (eMgrRecType_SBR == pstRecSetup->eRecType)
		{
			xmgrRecSeriesLock_e eSeriesLock;
			HCHAR				szSeriesId[EPG_TVA_CRID_TOTAL_LEN] = {0,};

			HxSTD_snprintf(szSeriesId,EPG_TVA_CRID_TOTAL_LEN,"%d",pstSchedule->uExtInfo.stRec.ulSeriesId);

			eSeriesLock = xmgr_rec_SeriesRecCheckLock (pstRecSetup->hCurrSvc, szSeriesId);
			switch (eSeriesLock)
			{
				// serise lock true
				case eXmgrRecSeriesLock_True:
				case eXmgrRecSeriesLock_Some:
					bLock = TRUE;
					break;

				// series lock false
				case eXmgrRecSeriesLock_False:
					bLock = FALSE;
					break;

				// series lock unknown
				case eXmgrRecSeriesLock_Init:
				default:
					// follow svc lock
					break;
			}
			if(bLock == TRUE)	{stMetaInfo.bUserLocked = TRUE;}
		}
#endif
		if(bLock == TRUE)	stMetaInfo.u1cLocked = TRUE;
		else				stMetaInfo.u1cLocked = FALSE;
	}
	if ((DxRSVTYPE_RECORD_TBR != pstSchedule->eRsvType) && (DxRSVTYPE_RECORD_SATRECORD_TBR != pstSchedule->eRsvType))
	{
		stMetaInfo.u2cEventBase	= TRUE;
	}

	HxLOG_Info ("evtName(%s) evtId(0x%04x)\n", pstSchedule->uExtInfo.stRec.szEventName, pstSchedule->uExtInfo.stRec.nEvtId);


	if(xmgr_rec_GetPath(pstRecSetup->ulPvrIdx) != NULL)
	{
		snprintf(stMetaInfo.aucPath, FS_MAX_FILE_NAME_LEN, "%s", xmgr_rec_GetPath(pstRecSetup->ulPvrIdx));
	}
	else
	{
		HxLOG_Error("SVC_META_MakeFile error, PVR device is not avaiable!!\n");
		return ERR_FAIL;
	}

	xmgr_rec_ConvertUtf8ToByte (pstSchedule->uExtInfo.stRec.szEventName, szFileName);
	HLIB_STD_StrUtf8NCpy (stMetaInfo.aucDisplayEvtName, szFileName, PVR_MAX_TITLE_LEN - 1);

	hErr = SVC_META_MakeFileName (stMetaInfo.ulStartTime, szFileName, stMetaInfo.aucFileName, HxPATH_MAX);
	if(hErr != ERR_OK)
	{
		HxLOG_Error ("SVC_META_MakeFileName error. SVCHandle:(0x%08x), (%s)\n", pstRecSetup->hCurrSvc, szFileName);
		return ERR_FAIL;
	}
	hErr = SVC_META_AddContentsInfo (pstRecSetup->hCurrSvc, &stMetaInfo, &ulContentId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_PVR_AddContentsInfo err: SVCHandle:(0x%08x)\n", pstRecSetup->hCurrSvc);
		return ERR_FAIL;
	}

#if defined(CONFIG_APCORE_IP_EPG) && defined(SVC_META_NOT_CONTAIN_ALL_CONTENTS)
	if (eMgrRecType_SBR == pstRecSetup->eRecType)
	{
		HCHAR				szSeriesId[EPG_TVA_CRID_TOTAL_LEN] = {0,};

		HxSTD_snprintf(szSeriesId, EPG_TVA_CRID_TOTAL_LEN, "%d", pstSchedule->uExtInfo.stRec.ulSeriesId);

		hErr = xmgr_rec_CheckSeriesRecordLockViaDama(ulContentId, szSeriesId);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("xmgr_rec_CheckSeriesRecordLockViaDama err:\n");
		}
	}
#endif

	BUS_PostMessage(NULL, eSEVT_META_UPDATED, (Handle_t)HANDLE_NULL, ulContentId, 1, 0);
	return ERR_OK;
}

HUINT8 *xmgr_recorder_GetRecordPath_Base (HUINT32 ulPvrIdx)
{
	return xmgr_rec_GetPath (ulPvrIdx);
}

HBOOL	xmgr_recorder_IsTsrLockForDelayedRecord_Base (HUINT32 ulContentId, HINT32 nEventId, HUINT16 nParental)
{
	xmgrRec_Context_t	*pstContext	=	NULL;
	MgrAction_Type_e		actionType	=	0;
	HUINT32					ulActionId	=	0;

	if( eDxRATING_VIEW_ALL == nParental )
		return FALSE;

	for (ulActionId = eActionId_REC_FIRST; ulActionId <= eActionId_REC_LAST; ulActionId++)
	{
		HINT32 i=0;

		pstContext	= xmgr_rec_GetContext(ulActionId - eActionId_REC_FIRST);
		actionType	= MGR_ACTION_GetType(ulActionId);

		if ((actionType == eRecType_TSR_REC) && (ulContentId == pstContext->ulContentId) && (pstContext->ulActionId == ulActionId))
		{
			//	ZIGGO향의 경우 Event 가 없으므로 스펙에 의해 이벤트 없으면 All View표시.
#if	defined(CONFIG_NO_PARENTAL_CONTROL_ON_RADIO_SVC)
			//	ZIGGO Model이면 원칙적으로 Parental Rating무시.
			{
				DxSvcType_e eSvcType;
				hErr = xmgr_rec_GetItemFromSVCDB(pstContext->stSetup.hSvc, xmgr_SvcDB_eSvcType, (xmgr_SvcDB_Value_t*)&eSvcType);
				if( eDxSVC_TYPE_RADIO == eSvcType )
					return FALSE;
			}
#endif
			if (nParental == eDxRATING_AGE_LOCK_ALL)
				return TRUE;

			//	if TSR didn't get any event for delayed recording, we assume NO EIT in SI.
			//	for Irdeto, we make it lock forced.
			if (pstContext->nRecvEventNum == 0)
			{
				return TRUE;
			}

			for (i = 0; i < pstContext->nRecvEventNum; i++)
			{
				if (pstContext->astRecvEventInfo[i].nEventId == nEventId)
					break;
			}

			if( i == pstContext->nRecvEventNum )
				i = 0;

			for ( ; i < pstContext->nRecvEventNum; i++)
			{
#if defined(CONFIG_APCORE_BLOCK_EQUAL_RATING)
				if (pstContext->astRecvEventInfo[i].usParentalRating >= nParental)
#else
				if (pstContext->astRecvEventInfo[i].usParentalRating >  nParental)
#endif
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}


HBOOL	xmgr_recorder_IsDuringDelayedRecCopy_Base (Handle_t hAction)
{
	HERROR	hErr = ERR_FAIL;
	HUINT8 szFileName[FS_MAX_FILE_NAME_LEN];

	xmgrRec_Context_t *	pstContext = xmgr_rec_GetContextByActionHandle(hAction);

	if( TRUE == pstContext->bDelayedCopy )
	{
		HUINT8 szFileName2[FS_MAX_FILE_NAME_LEN];
		SvcMeta_Record_t stInfo;

		snprintf(szFileName, FS_MAX_FILE_NAME_LEN, "%s", pstContext->szDelayCopyFileName);

		HxSTD_MemSet(&stInfo, 0, sizeof(SvcMeta_Record_t));
		hErr = SVC_META_GetRecordInfo(pstContext->ulContentId, &stInfo);
		if(hErr == ERR_OK)
		{
			snprintf(szFileName2, FS_MAX_FILE_NAME_LEN, "%s%s", stInfo.aucPath, stInfo.aucFileName);
			if(strcmp(szFileName2, szFileName) == 0)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

HBOOL xmgr_recorder_IsScrambleToDescrambleRecord_Base (MgrRec_RecInfo_t *pstInfo)
{
	HINT32 i = 0;
	MgrAction_Type_e actType;
	xmgrRec_Context_t	*pstContext = NULL;

	for(i = 0; i < NUM_MAX_REC_ACTION; i++)
	{
		actType = MGR_ACTION_GetType(eActionId_REC_FIRST+i);
		pstContext = xmgr_rec_GetContext(i);
		if((actType == eRecType_REC || actType == eRecType_EXT_REC)
			&& (eMgrRecType_SCR2DESC == pstContext->stSetup.eRecType)
			&& (pstContext->ulActionId >= eActionId_REC_FIRST && pstContext->ulActionId <= eActionId_REC_LAST) )
		{
			xmgr_rec_GetRecInfoFromCtx(pstContext, pstInfo);
			return TRUE;
		}
	}

	return FALSE;
}

HERROR	xmgr_recorder_FindRecordingSvcInfo_Base (Handle_t hSvc, HUINT32 *pulActionId, MgrRec_RecInfo_t *pstInfo)
{
	// 서비스(hSvc)에 대한 레코딩 정보를 가져온다. ( TSR_REC 은 제외된다. )
	HINT32 i = 0;
	MgrAction_Type_e actType;
	xmgrRec_Context_t	*pstContext = NULL;

	if(hSvc == HANDLE_NULL)
		return ERR_FAIL;

	for(i = 0; i < NUM_MAX_REC_ACTION; i++)
	{
		actType = MGR_ACTION_GetType(eActionId_REC_FIRST+i);
		pstContext = xmgr_rec_GetContext(i);

		if(	(actType == eRecType_REC || actType == eRecType_EXT_REC)
			&& (hSvc == pstContext->stSetup.hCurrSvc)
			&& (pstContext->ulActionId >= eActionId_REC_FIRST && pstContext->ulActionId <= eActionId_REC_LAST) )
		{
			if(pulActionId)
			{
				*pulActionId = eActionId_REC_FIRST + i;
			}
			return xmgr_rec_GetRecInfoFromCtx(pstContext, pstInfo);
		}
	}

	return ERR_FAIL;
}

HERROR	xmgr_recorder_FindRecordingMasterSvcInfo_Base (Handle_t hMasterSvcHandle, HUINT32 *pulActionId, MgrRec_RecInfo_t *pstInfo)
{
	// Master 서비스(hSvc)에 대한 레코딩 정보를 가져온다. ( TSR_REC 은 제외된다. )
	HINT32 i = 0;
	MgrAction_Type_e actType;
	xmgrRec_Context_t	*pstContext = NULL;

	if(hMasterSvcHandle == HANDLE_NULL)
		return ERR_FAIL;

	for(i = 0; i < NUM_MAX_REC_ACTION; i++)
	{
		actType	= MGR_ACTION_GetType(eActionId_REC_FIRST +i);
		pstContext = xmgr_rec_GetContext(i);
		if((actType == eRecType_REC || actType == eRecType_EXT_REC)
			&& (hMasterSvcHandle == pstContext->stSetup.hMasterSvc)
			&& (pstContext->ulActionId >= eActionId_REC_FIRST && pstContext->ulActionId <= eActionId_REC_LAST))
		{
			if(pulActionId)
			{
				*pulActionId = eActionId_REC_FIRST + i;
			}
			return xmgr_rec_GetRecInfoFromCtx(pstContext, pstInfo);
		}
	}

	return ERR_FAIL;
}

HERROR	xmgr_recorder_FindTsrRecordingSvcInfo_Base (Handle_t hSvc, HUINT32 *pulActionId, MgrRec_RecInfo_t *pstInfo)
{
	// 서비스(hSvc)에 대한 TSR_REC 정보를 가져온다.
	MgrAction_Type_e actType;
	xmgrRec_Context_t	*pstContext = NULL;
	HUINT32			ulActionId;

	if ((hSvc == HANDLE_NULL) || (MGR_PVR_GetTsrRecActionId(hSvc, &ulActionId) != ERR_OK))
	{
		return ERR_FAIL;
	}

	if(ulActionId >= eActionId_REC_FIRST && ulActionId <= eActionId_REC_LAST)
	{
		actType = MGR_ACTION_GetType(ulActionId);
		pstContext = xmgr_rec_GetContext(ulActionId - eActionId_REC_FIRST);
		if((actType == eRecType_TSR_REC)
			&& (hSvc == pstContext->stSetup.hCurrSvc)
			&& (pstContext->ulActionId == ulActionId))
		{
			if(pulActionId)
			{
				*pulActionId = ulActionId;
			}

			if( xmgr_rec_GetRecInfoFromCtx(pstContext, pstInfo)==ERR_FAIL )
			{
				return ERR_FAIL;
			}

			// playback시 사용할 startTime은 di에서 받은 actual value를 사용
			if (pstContext->stMetaInfo.ulStartTime != pstContext->ulStartTimeFromTimeCode && pstInfo != NULL)
				pstInfo->ulStartTime = pstContext->ulStartTimeFromTimeCode;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}


HERROR	xmgr_recorder_GetRecordSvcHandleByContentId_Base (HUINT32 ulContentId, Handle_t *phHandle)
{
	HUINT32				ulActionId;
	xmgrRec_Context_t		*pstContext = NULL;
	MgrAction_Type_e		actType;

	for (ulActionId = eActionId_REC_FIRST; ulActionId <= eActionId_REC_LAST; ulActionId++)
	{
		actType = MGR_ACTION_GetType(ulActionId);
		if (actType == eRecType_REC || actType == eRecType_EXT_REC || actType == eRecType_TSR_REC)
		{
			pstContext = xmgr_rec_GetContext(ulActionId - eActionId_REC_FIRST);
			if (pstContext->ulContentId == ulContentId)
			{
				*phHandle = pstContext->stSetup.hCurrSvc;
				return ERR_OK;
			}
		}
	}

	return ERR_FAIL;
}


HERROR	xmgr_recorder_GetRecordingSvcInfo_Base (HUINT32 ulActionId, MgrRec_RecInfo_t *pstInfo)
{
	// ActionId 에 대한 레코딩 정보를 가져온다. ( TSR_REC 도 가능하다. )

	HUINT32 ulRecId = ulActionId - eActionId_REC_FIRST;
	MgrAction_Type_e actType;
	xmgrRec_Context_t	*pstContext = NULL;

	if(ulActionId < eActionId_REC_FIRST || ulActionId > eActionId_REC_LAST)
		return ERR_FAIL;

	actType = MGR_ACTION_GetType(ulActionId);
	pstContext = xmgr_rec_GetContext(ulRecId);

	if( SVC_PIPE_GetActionId (pstContext->hAction) == ulActionId )
	{
		if( (eRecType_REC		==	actType ) ||
			(eRecType_EXT_REC	==	actType ) ||
			(eRecType_TSR_REC	==	actType ) )
		{
			return xmgr_rec_GetRecInfoFromCtx(pstContext, pstInfo);
		}
	}

	HxLOG_Error("can't find rec-context of ulActionId(%d)\n", ulActionId);
	return ERR_FAIL;
}

HERROR	xmgr_recorder_GetTsrLimitForUsbFlash_Base (MgrRec_TsrLimitForUsbFlash_t *pstUsbFlash)
{
	if (NULL == pstUsbFlash)
		return ERR_FAIL;

	HxSTD_memset (pstUsbFlash, 0, sizeof(MgrRec_TsrLimitForUsbFlash_t));

	pstUsbFlash->bIsUsbFlashRecord = FALSE;

	return ERR_OK;
}

HBOOL xmgr_recorder_IsDelayedRecCopyInProgress_Base(void)
{
	HUINT16 								ulActionId = 0;
	xmgrRec_Context_t				 *pstContext= NULL;

	for (ulActionId = eActionId_VIEW_FIRST; ulActionId <= eActionId_REC_LAST ; ulActionId++)
	{
		pstContext = xmgr_rec_GetContext(ulActionId);
		if(pstContext){
			HxLOG_Debug("[%d] bDelayedCopy(%d) \n", ulActionId, pstContext->bDelayedCopy);
			if(pstContext->bDelayedCopy == TRUE)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

HERROR xmgr_recorder_SetRecordSetup_Base (MgrRec_RecSetup_t *pstSetup, SvcRec_Setup_t *pstSvcRecSetup, SvcMeta_Record_t * pstMetaInfo,Handle_t hAction)
{
	if ((NULL == pstSetup) || (NULL == pstSvcRecSetup) || (NULL == pstMetaInfo))
		return ERR_FAIL;

	NOT_USED_PARAM(hAction);

	/* setup a recorder */
	//HxSTD_MemSet(pstSvcRecSetup, 0, sizeof(SvcRec_Setup_t));
	pstSvcRecSetup->hSvc				= pstSetup->hCurrSvc;
	pstSvcRecSetup->hMasterSvc 			= pstSetup->hMasterSvc;
//	pstSvcRecSetup->ulSourceContentsId 	= pstContext->stSetup.ulSourceContentId;
	pstSvcRecSetup->eRecActType			= xmgr_rec_ConvertRecTypeToMgrActionType(pstSetup->eRecType);
	pstSvcRecSetup->eRecOutType 		= eRec_OutType_File;

	pstSvcRecSetup->ulStartTime		= pstMetaInfo->ulStartTime;

	HxSTD_MemCopy (pstSvcRecSetup->unOutArg.stFile.aucPath,	pstMetaInfo->aucPath, FS_MAX_FILE_NAME_LEN );
	HxSTD_MemCopy (pstSvcRecSetup->unOutArg.stFile.aucFileName,pstMetaInfo->aucFileName, FS_MAX_FILE_NAME_LEN );

#if defined(CONFIG_MW_MM_FULLTS_REC)
	if(pstSvcRecSetup->eRecActType == eRec_ActType_FullTs)
	{
		DbSvc_TsInfo_t		stTsInfo;
		HERROR				hErr;
		HUINT32				ulFreq;
		HUINT32				utCurrTime;
		HxDATETIME_t		stCurrentDateTime;

		HxSTD_MemSet(&stTsInfo, 0, sizeof(DbSvc_TsInfo_t));
		hErr = DB_SVC_GetTsInfoByServiceHandle(pstSetup->hCurrSvc, &stTsInfo);

		if(hErr == ERR_OK)
		{
			switch(stTsInfo.eDeliType)
			{
				case eDxDELIVERY_TYPE_SAT:
					ulFreq = stTsInfo.stTuningParam.sat.ulFrequency;
					break;

				case eDxDELIVERY_TYPE_TER:
					ulFreq = stTsInfo.stTuningParam.ter.ulFreq;
					break;

				case eDxDELIVERY_TYPE_CAB:
					ulFreq = stTsInfo.stTuningParam.cab.ulFreq;
					break;
			}

			VK_CLOCK_GetTime((unsigned long *)&utCurrTime);
			HLIB_DATETIME_ConvertUnixTimeToDateTime(utCurrTime, &stCurrentDateTime);

			HxSTD_snprintf(pstSvcRecSetup->unOutArg.stFile.aucFileName, FS_MAX_FILE_NAME_LEN, "%dKhz_%04d%02d%02d_%02d%02d",
				ulFreq,
				stCurrentDateTime.stDate.usYear,
				stCurrentDateTime.stDate.ucMonth,
				stCurrentDateTime.stDate.ucDay,
				stCurrentDateTime.stTime.ucHour,
				stCurrentDateTime.stTime.ucMinute);
		}
	}
#endif

	{
		pstSvcRecSetup->stUsbFlash.bIsUsbFlashRecord	=	pstSetup->stUsbFlash.bIsUsbFlashRecord;
		pstSvcRecSetup->stUsbFlash.ulLimitTimeSecond	=	pstSetup->stUsbFlash.ulLimitTimeSecond;;
		pstSvcRecSetup->stUsbFlash.ullLimitDataSize		=	pstSetup->stUsbFlash.ullLimitDataSize;;
		pstSvcRecSetup->stUsbFlash.ullLimitIndexSize	=	pstSetup->stUsbFlash.ullLimitIndexSize;;
	}

	if( eRec_ActType_Descramble_Record == pstSvcRecSetup->eRecActType )
	{
		pstMetaInfo->u1cEncryption = 0;
	}

	pstSvcRecSetup->bEncryption = MGR_RECORDER_GetEncryptionMode(pstSetup);

	return ERR_OK;
}

HBOOL	xmgr_recorder_GetEncryptionMode_Base (MgrRec_RecSetup_t *pstSetup)
{
	HBOOL	bEncryption = FALSE;

	HxLOG_Info("bEncryption: %d \n", bEncryption);

	return bEncryption;
}


#define _________DEBUG_FUNC__________

void xmgr_rec_DebugPrintRecPidInfo_Base(SvcRec_PIDList_t *pstPIDList, HCHAR *funcName, HINT32 nLine)
{
#ifdef CONFIG_DEBUG
	HINT32		i = 0;


	HxLOG_Print("[%s:%d] ** xmgr_rec_DebugPrintRecPidInfo_Base **\n", funcName, nLine);

	if(NULL == pstPIDList)
	{
		HxLOG_Error("\t pstPIDList is NULL\n");
		return;
	}

	HxLOG_Print("\t pstPIDList->ePidSetType      = %d \n", pstPIDList->ePidSetType);

	if(eRec_PidSetType_Known == pstPIDList->ePidSetType)
	{
		for(i = 0; i < eSvcRec_MAX_REC_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stSI[i].usPid)
			{
				HxLOG_Print("\t stKnown.stSI[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stSI[i].usPid, pstPIDList->stKnown.stSI[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_VIDEO_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stVideo[i].usPid)
			{
				HxLOG_Print("\t stKnown.stVideo[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stVideo[i].usPid, pstPIDList->stKnown.stVideo[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_AUDIO_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stAudio[i].usPid)
			{
				HxLOG_Print("\t stKnown.stAudio[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stAudio[i].usPid, pstPIDList->stKnown.stAudio[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_SUBTTL_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stSubtitle[i].usPid)
			{
				HxLOG_Print("\t stKnown.stSubtitle[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stSubtitle[i].usPid, pstPIDList->stKnown.stSubtitle[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_TELTXT_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stTeletext[i].usPid)
			{
				HxLOG_Print("\t stKnown.stTeletext[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stTeletext[i].usPid, pstPIDList->stKnown.stTeletext[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_CAPTION_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stCaption[i].usPid)
			{
				HxLOG_Print("\t stKnown.stCaption[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stCaption[i].usPid, pstPIDList->stKnown.stCaption[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_CA_PID_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stEcm[i].usPid)
			{
				HxLOG_Print("\t stKnown.stEcm[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stEcm[i].usPid, pstPIDList->stKnown.stEcm[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_CA_PID_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stEmm[i].usPid)
			{
				HxLOG_Print("\t stKnown.stEmm[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stEmm[i].usPid, pstPIDList->stKnown.stEmm[i].ulRequestId);
			}
		}
		for(i = 0; i < SvcRec_MAX_DATA_ES_NUM; i++)
		{
			if(PID_NULL != pstPIDList->stKnown.stData[i].usPid)
			{
				HxLOG_Print("\t stKnown.stData[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stKnown.stData[i].usPid, pstPIDList->stKnown.stData[i].ulRequestId);
			}
		}
	}
	else if(eRec_PidSetType_Unknown == pstPIDList->ePidSetType)
	{
		for(i = 0; i < DxRSV_PIDTABLE_COUNT; i++)
		{
			if(PID_NULL != pstPIDList->stUnknown.stPIDs[i].usPid)
			{
				HxLOG_Print("\t stUnknown.stPIDs[%d]\t= (PID:0x%04X, ReqID:%d) \n", i, pstPIDList->stUnknown.stPIDs[i].usPid, pstPIDList->stUnknown.stPIDs[i].ulRequestId);
			}
		}
	}
	else if(eRec_PidSetType_All == pstPIDList->ePidSetType)
	{
		HxLOG_Print("\t stUnknown.stPIDs\t= (PID:ALL, ReqID:%d) \n", pstPIDList->stUnknown.stPIDs[i].ulRequestId);
	}

	HxLOG_Print("---------------------------------------------------------------\n");
#endif
}

