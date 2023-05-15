/**
	@file     ap_clocksetup.c <<FOR SATELLITE>>
	@brief    Tune to ref. Tp to setup time when the clock time is invalid.

	Description:
	Module: AP/AM			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <db_param.h>

#include <svc_pipe.h>
#include <svc_si.h>
#include <svc_resources.h>
#include <svc_sys.h>
#include <bus.h>
#include <mgr_common.h>
#include <mgr_svclist.h>
#include <mgr_action.h>
#include <mgr_spec.h>
#include <mgr_rscmap.h>

#include <mgr_time.h>
#include <pmgr_time.h>
#include <pmgr_model.h>
#include <mgr_search.h>

#include <otl.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define CLOCK_SETUP_TIMEOUT				(45*1000)	/* 45 sec. */
#define CLOCK_SETUP_TIMER_ID			0x100

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HUINT32		s_ulClockRecoverActionId;
STATIC Handle_t		s_hClockRecoverAction;
STATIC HINT32		s_nCurRefIdx;

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/


/*******************************************************************/
/********************      API Functions   *************************/
/*******************************************************************/


/********************************************************************/
/********************	Application Proc.	*************************/
/********************************************************************/

MgrTime_ClockRecoverRefTpInfo_t *mgr_clockrecover_GetRefTpList(void)
{
	return pmgr_time_ClockRecoverGetRefTpList();
}

STATIC HERROR mgr_time_GetTimeupdateSiSpec (SvcSi_TimeUpdateSpec_t *pstSpec)
{
	return MGR_MODEL_TimeupdateGetSiSpec (pstSpec);
}

STATIC void mgr_time_ClockRecoverExit(HBOOL bActionFail)
{
	HxLOG_Print("[Clock Setup] mgr_time_ClockRecoverExit()\n");
	BUS_MGR_DestroyWithReason(0, bActionFail?eDESTROY_BUS_ACTION:eDESTROY_BUS_MYSELF, 0);
}

STATIC HERROR mgr_time_ClockRecoverSearchByFirstSvc(MgrTime_ClockRecoverRefMethod_e eRefMethod, DxTuneParam_t	*tuningInfo)
{
	return pmgr_time_ClockRecoverSearchByFirstSvc (eRefMethod, tuningInfo);
}

STATIC HERROR mgr_time_ClockRecoverSearchByLastSvc(DxTuneParam_t	*tuningInfo)
{
	Handle_t		hSvc;
	DbSvc_TsInfo_t	tsInfo;
	HERROR			hErr;

	HxLOG_Print("[Clock Setup] mgr_time_ClockRecoverSearchByLastSvc()\n");

	/* Find saved last(current) service handle */
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_CUR_SVC, (HUINT32 *)&hSvc, 0);
	if(hErr != ERR_FAIL)
	{
		HxLOG_Error("cannot get hSvc \n");
		return ERR_FAIL;
	}

	hErr = DB_SVC_GetTsInfoByServiceHandle(hSvc, &tsInfo);
	if(hErr == ERR_OK)
	{
		hErr = OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&tsInfo, tuningInfo);
		if (ERR_OK != hErr)
			HxLOG_Critical("Critical Eror!!!\n");

		return ERR_OK;
	}
	else
	{
		HxLOG_Error("cannot get tsInfo \n");
		return ERR_FAIL;
	}
}

STATIC HERROR mgr_time_ClockRecoverSearchByWakeUpSvc(DxTuneParam_t	*tuningInfo)
{
	HxLOG_Print("[Clock Setup] mgr_time_ClockRecoverSearchByWakeUpSvc()\n");

	// TODO: 예약 완성 후 구현 예정.

	return ERR_FAIL;
}

#if defined(CONFIG_MW_CH_SATELLITE)
STATIC HERROR mgr_time_ClockRecoverSearchBySatTp(HUINT32 ulRefTpIdx, DxTuneParam_t	*tuningInfo)
{
	HINT32				*pnAntIdxArray = NULL;
	HINT32				nNumAntenna;
	DbSvc_AntInfo_t			stAntInfo;
	HBOOL				bAntennaFound = FALSE;
	HINT32				nCount;
	HERROR				hErr;
	MgrTime_ClockRecoverRefTpInfo_t	*astTpList = NULL;

	HxSTD_MemSet(&stAntInfo, 0x00, sizeof(DbSvc_AntInfo_t));
	HxLOG_Print("[Clock Setup] mgr_time_ClockRecoverSearchBySatTp(ref TP #%d)\n", ulRefTpIdx);

	astTpList = mgr_clockrecover_GetRefTpList();
	if (NULL == astTpList)
	{
		return ERR_FAIL;
	}

	hErr = DB_SVC_FindAllAntIdxList(&nNumAntenna, &pnAntIdxArray);
	if (hErr == ERR_OK && nNumAntenna > 0)
	{ /* More than one antenna set up. */

		for(nCount = 0; nCount < nNumAntenna; nCount++)
		{ /* Find the reference antenna. */
			hErr = DB_SVC_GetAntInfo((HUINT16)pnAntIdxArray[nCount], &stAntInfo);
			if (hErr == ERR_OK)
			{
				if(stAntInfo.eSatType == astTpList[ulRefTpIdx].eRefSatType)
				{ /* Reference antenna found */
					HxLOG_Print("[mgr_time_ClockRecoverSearchBySatTp] %d\n", stAntInfo.eSatType);
					bAntennaFound = TRUE;
					break;
				}
			}
		}
	}

	DB_SVC_FreeAntList (pnAntIdxArray);

	if(bAntennaFound == TRUE)
	{ /* Reference antenna found. Tune to the TP and setup SI time action */
		/* Reset timeout timer */
		HxLOG_Print("Found reference satellite for ref #%d ^^;\n", ulRefTpIdx);
		tuningInfo->sat.deliveryType 	= eDxDELIVERY_TYPE_SAT;

		tuningInfo->sat.ulTunerGroupId	= RxRSCID_NULL;
		tuningInfo->sat.ulRfInputId		= RxRSCID_NULL;
		tuningInfo->sat.tuningInfo.ulFrequency		= astTpList[ulRefTpIdx].ulFrequency;
		tuningInfo->sat.tuningInfo.ulSymbolRate 		= astTpList[ulRefTpIdx].ulSymbolRate;
		tuningInfo->sat.tuningInfo.eFecCodeRate		= astTpList[ulRefTpIdx].fecCodeRate;
		tuningInfo->sat.tuningInfo.ePolarization		= astTpList[ulRefTpIdx].polarization;
		tuningInfo->sat.tuningInfo.eTransSpec		= astTpList[ulRefTpIdx].transSpec;
		tuningInfo->sat.tuningInfo.ePskMod			= astTpList[ulRefTpIdx].pskMod;
		tuningInfo->sat.tuningInfo.ePilot			= astTpList[ulRefTpIdx].pilot;
		tuningInfo->sat.tuningInfo.eRollOff			= astTpList[ulRefTpIdx].rollOff;

		tuningInfo->sat.antennaType		= stAntInfo.eAntennaType;
		switch(tuningInfo->sat.antennaType)
		{
			case eDxANT_TYPE_LNB_ONLY:
				tuningInfo->sat.antInfo.info.lnb.ulLnbFreq			= stAntInfo.utAntInfo.stLnbAnt.ulLnbFreq;
				tuningInfo->sat.antInfo.info.lnb.lnbVoltage 			= stAntInfo.utAntInfo.stLnbAnt.eLnbVoltage;
				break;

			case eDxANT_TYPE_DISEQC:
				tuningInfo->sat.antInfo.info.diseqc.ulLnbFreq			= stAntInfo.utAntInfo.stDiseqcAnt.ulLnbFreq;
				tuningInfo->sat.antInfo.info.diseqc.lnbVoltage 		= stAntInfo.utAntInfo.stDiseqcAnt.eLnbVoltage;
				tuningInfo->sat.antInfo.info.diseqc.b22kTone			= stAntInfo.utAntInfo.stDiseqcAnt.b22kTone;

				tuningInfo->sat.antInfo.info.diseqc.diseqcVersion		= stAntInfo.utAntInfo.stDiseqcAnt.eDiseqcVer;
				tuningInfo->sat.antInfo.info.diseqc.diseqcInput		= stAntInfo.utAntInfo.stDiseqcAnt.eDiseqcInput;
				tuningInfo->sat.antInfo.info.diseqc.toneBurst			= stAntInfo.utAntInfo.stDiseqcAnt.eToneBurst;

				tuningInfo->sat.antInfo.info.diseqc.ucMotorPosition	= stAntInfo.utAntInfo.stDiseqcAnt.ucMotorPosition;
				tuningInfo->sat.antInfo.info.diseqc.sSatLonggitude	= stAntInfo.utAntInfo.stDiseqcAnt.sSatLonggitude;
				break;

			case eDxANT_TYPE_SCD:
				tuningInfo->sat.antInfo.info.scd.ulLnbFreq 			= stAntInfo.utAntInfo.stScdAnt.ulLnbFreq;
				tuningInfo->sat.antInfo.info.scd.lnbVoltage			= stAntInfo.utAntInfo.stScdAnt.eLnbVoltage;
				tuningInfo->sat.antInfo.info.scd.b22kTone				= stAntInfo.utAntInfo.stScdAnt.b22kTone;

				tuningInfo->sat.antInfo.info.scd.scdInput				= stAntInfo.utAntInfo.stScdAnt.eScdInput;
				tuningInfo->sat.antInfo.info.scd.ulScdUserBandNum		= stAntInfo.utAntInfo.stScdAnt.ulScdUserBandNum;
				tuningInfo->sat.antInfo.info.scd.ulScdUserBandFreq	= stAntInfo.utAntInfo.stScdAnt.ulScdUserBandFreq;
#if !defined(CONFIG_APUI_SAT_1TUNER)
				tuningInfo->sat.antInfo.info.scd.ulScdUserBandNum1	= stAntInfo.utAntInfo.stScdAnt.ulScdUserBandNum1;
				tuningInfo->sat.antInfo.info.scd.ulScdUserBandFreq1	= stAntInfo.utAntInfo.stScdAnt.ulScdUserBandFreq1;
#endif
				break;

			case eDxANT_TYPE_SMATV:
				break;

			default:
				HxLOG_Critical("\n\n");
				break;
		}

		return ERR_OK;
	}
	else
	{ /* No reference antenna found! Tune to the 1st service of satellite channel list */
		HxLOG_Error("No reference satellite found for ref# %d!!\n", ulRefTpIdx);
		return ERR_FAIL;
	}
}
#endif // #if defined(CONFIG_MW_CH_SATELLITE)

#if defined(CONFIG_MW_CH_TERRESTRIAL)
STATIC HERROR mgr_time_ClockRecoverSearchByTerTp(HUINT32 ulRefTpIdx, DxTuneParam_t	*tuningInfo)
{
	return pmgr_time_ClockRecoverSearchByTerTp(ulRefTpIdx,tuningInfo);
}

#endif // #if defined(CONFIG_MW_CH_TERRESTRIAL)

STATIC HERROR mgr_time_ClockRecoverStartTimeSearch(HUINT32 ulRefTpIdx, DxTuneParam_t *tuningInfo)
{
	HERROR		hErr = ERR_FAIL;
	MgrTime_ClockRecoverRefTpInfo_t		*astTpList = NULL;

	astTpList = mgr_clockrecover_GetRefTpList();
	if (NULL == astTpList)
	{
		return ERR_FAIL;
	}

	switch(astTpList[ulRefTpIdx].eRefMethod)
	{
#if defined(CONFIG_MW_CH_SATELLITE)
		case eREF_SAT_TP:
			hErr = mgr_time_ClockRecoverSearchBySatTp(ulRefTpIdx, tuningInfo);
			break;
#endif
#if defined(CONFIG_MW_CH_TERRESTRIAL)
		case eREF_TER_TP:
			hErr = mgr_time_ClockRecoverSearchByTerTp(ulRefTpIdx, tuningInfo);
			break;

#endif

#if defined(CONFIG_MW_CH_SATELLITE)
		case eREF_FIRST_SVC_SAT:
#endif
#if defined(CONFIG_MW_CH_TERRESTRIAL)
		case eREF_FIRST_SVC_TER:
#endif
#if defined(CONFIG_MW_CH_CABLE)
		case eREF_FIRST_SVC_CAB:
#endif
			hErr = mgr_time_ClockRecoverSearchByFirstSvc(astTpList[ulRefTpIdx].eRefMethod, tuningInfo);
			break;

		case eREF_LAST_SVC:
			hErr = mgr_time_ClockRecoverSearchByLastSvc(tuningInfo);
			break;

		case eREF_WAKEUP_SVC:
			hErr = mgr_time_ClockRecoverSearchByWakeUpSvc(tuningInfo);
			break;

		default:
			hErr = ERR_FAIL;
			break;
	}
	return hErr;
}


STATIC HERROR mgr_time_ClockRecoveryStartNextTimeSearch(DxTuneParam_t	*tuningInfo)
{
	HERROR								hErr = ERR_FAIL;
	MgrTime_ClockRecoverRefTpInfo_t 	*astTpList = NULL;

	astTpList = mgr_clockrecover_GetRefTpList();
	if (NULL == astTpList)
	{
		return ERR_FAIL;
	}

	while(astTpList[s_nCurRefIdx].eRefMethod != eREF_END)
	{
		hErr = mgr_time_ClockRecoverStartTimeSearch(s_nCurRefIdx, tuningInfo);
		if (hErr == ERR_OK)
		{
			/* Successfully started ref TP clock setup. Break and wait channel/SI event or timeout. */
			return ERR_OK;
		}
		else
		{
			/* This ref TP is not supported in this STB. Move to next ref TP... */
			s_nCurRefIdx++;
		}
	}

	return ERR_FAIL;
}

STATIC BUS_Result_t proc_time_ClockRecoverAction(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		hErr;
	STATIC 	HBOOL	bActionFail = TRUE;

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			{
				DxTuneParam_t	tuningInfo;
				tuningInfo.eDeliType = eDxDELIVERY_TYPE_ALL;
				bActionFail = TRUE;
				HxLOG_Print("\n\t********** proc_time_ClockRecoverAction() Created. **********\n");
				BUS_MGR_SetMgrName(BUS_MGR_GetThis(), (HINT8*)__FUNCTION__);

				if(MGR_ACTION_GetEmptyBgActionId (&s_ulClockRecoverActionId) == ERR_OK)
				{
					s_hClockRecoverAction = SVC_PIPE_GetActionHandle (s_ulClockRecoverActionId);
					/* Search and Start reference TP */
					s_nCurRefIdx = 0;
					hErr = mgr_time_ClockRecoveryStartNextTimeSearch(&tuningInfo);

					if (hErr == ERR_OK)
					{
						if(tuningInfo.eDeliType != eDxDELIVERY_TYPE_ALL)
						{
							hErr = MGR_ACTION_SetupSearch(s_ulClockRecoverActionId, &tuningInfo, eAmPriority_VIEW_ACTION - (1<<ACTION_PRIORITY_BIT_OFFSET));
							if (ERR_OK == hErr)
							{
								hErr = MGR_ACTION_Take(s_ulClockRecoverActionId, eActionType_SEARCH, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
								if (ERR_OK == hErr)
								{
									bActionFail = FALSE;
								}
							}
						}
						else
						{
							mgr_time_ClockRecoverExit(FALSE);
							return MESSAGE_BREAK;
						}
					}
					else
					{
						mgr_time_ClockRecoverExit(FALSE);
						return MESSAGE_BREAK;
					}

				}

				if(bActionFail == FALSE)
				{
					/* Successfully started ref TP clock setup. Break and wait channel/SI event or timeout. */
					HxLOG_Print("\n\t********** proc_time_ClockRecoverAction() Action Taken Success. **********\n");
					SVC_CH_StopTune(s_hClockRecoverAction, -1, eSyncMode);
					MGR_TIME_ClockRecoverTune (&tuningInfo);
					/* Create clock setup timeout timer first */
					BUS_SetTimer(CLOCK_SETUP_TIMER_ID, CLOCK_SETUP_TIMEOUT);
					return MESSAGE_BREAK;
				}
				else
				{
					HxLOG_Print("\n\t********** proc_time_ClockRecoverAction() Action Taken Fail. **********\n");
				}

				/* Comming to here means that there is no ref. TP or ref. satellite. Exit... */
				mgr_time_ClockRecoverExit(bActionFail);
				return MESSAGE_BREAK;
			}
		case eMEVT_SEARCH_ACTION_START: //search 중인 경우 time recover 필요 없다 (shutdown인 경우 )
			mgr_time_ClockRecoverExit(bActionFail);
			return MESSAGE_BREAK;
			break;
			
		case eSEVT_CH_LOCKED:
			HxLOG_Print("[Clock Setup] eSEVT_CH_LOCKED\n");
			break;

		case eSEVT_CH_NO_SIGNAL:
			/* Can't lock to the reference TP. Notify and exit */
			if(hAction != s_hClockRecoverAction)
			{
				break;
			}
			/* fall through */
		case eMEVT_BUS_TIMER:	/* Timeout without clock setup. Notify and exit */
			{
				DxTuneParam_t	tuningInfo;
				MgrTime_ClockRecoverRefTpInfo_t 	*astTpList = NULL;
				tuningInfo.eDeliType = eDxDELIVERY_TYPE_ALL;

				HxLOG_Print("[Clock Setup] %s\n", (message == eSEVT_CH_NO_SIGNAL) ? "eSEVT_CH_NO_SIGNAL" : "eMEVT_BUS_TIMER");

				astTpList = mgr_clockrecover_GetRefTpList();
				if (NULL == astTpList)
				{
					break;
				}

				s_nCurRefIdx++; /* Move to next ref TP... */

				hErr = SVC_CH_StopTune(s_hClockRecoverAction, -1, eSyncMode);

				hErr = mgr_time_ClockRecoveryStartNextTimeSearch(&tuningInfo);
				if (hErr == ERR_OK)
				{
					MGR_TIME_ClockRecoverTune (&tuningInfo);
					/* Successfully started ref TP clock setup. Break and wait channel/SI event or timeout. */
					return MESSAGE_BREAK;
				}

				/* Comming to here means that there is no ref. TP or ref. satellite. Exit... */
				mgr_time_ClockRecoverExit(bActionFail);
				return MESSAGE_BREAK;
			}

		case eSEVT_SI_TIME_RECEIVING_FINISHED:
			{
				HBOOL	bRcvTime = (HBOOL)p1;
				DxTuneParam_t	tuningInfo;
				tuningInfo.eDeliType = eDxDELIVERY_TYPE_ALL;

				HxLOG_Print("[Clock Setup] eSEVT_SI_TIME_RECEIVING_FINISHED : bRcvTime(%d)\n", bRcvTime);

				if (bRcvTime == TRUE)
				{
					/* Time setup succeeded. Notify and exit */
					mgr_time_ClockRecoverExit(bActionFail);
					return MESSAGE_BREAK;
				}

				s_nCurRefIdx++; /* Move to next ref TP... */
				hErr = SVC_CH_StopTune(s_hClockRecoverAction, -1, eSyncMode);

				hErr |= mgr_time_ClockRecoveryStartNextTimeSearch(&tuningInfo);
				if (hErr == ERR_OK)
				{

					MGR_TIME_ClockRecoverTune (&tuningInfo);
					/* Successfully started ref TP clock setup. Break and wait channel/SI event or timeout. */
					return MESSAGE_BREAK;
				}

				/* Comming to here means that there is no ref. TP or ref. satellite. Exit... */
				mgr_time_ClockRecoverExit(bActionFail);
				return MESSAGE_BREAK;
			}
			break;

		case eMEVT_BUS_GO_OPERATION :
			{
#if defined(CONFIG_PROD_HGS1000S)
#else
				WakeUpReason_t			eWakeupReason = 0;

				SVC_SYS_GetWakeUpReason(&eWakeupReason);
				if (eWakeupReason == eWAKEUP_BY_ACPOWER)
				{
					HxLOG_Warning("First time-recovery. Ignore go operation \n");
					break;
				}
#endif
				hErr = SVC_SI_StopTimeupdate(s_hClockRecoverAction, FALSE);
				if (ERR_OK != hErr)
				{
					HxLOG_Critical("Critical Eror!!!\n");
				}

				hErr = SVC_CH_StopTune(s_hClockRecoverAction, -1, eSyncMode);
				if (ERR_OK != hErr)
				{
					HxLOG_Critical("Critical Eror!!!\n");
				}

				BUS_MGR_Destroy (NULL);
			}
			break;

		case eMEVT_ACTION_SETUP_REQUESTED:
			{
				/*************************
				  Ignore Stop / Do stop pair
				  Ignore on GO_OPERATION by AC_Power
				  Do on START_LIVE
				**************************/
				MgrAction_Type_e eActionType = p1;
				if (eActionType != eViewType_WTV)
				{
					HxLOG_Debug("Not live. ignore actionType(%d) \n",eActionType);
					break;
				}

				HxLOG_Warning("Stop time-recovery when start live \n");
				MGR_TIME_ClockRecoverStop();
			}
			break;

		case eMEVT_LIVE_START_SVC:
			{
				/*************************
				  Ignore Stop / Do stop pair
				  Ignore on GO_OPERATION by AC_Power
				  Do on START_LIVE
				**************************/
				HxLOG_Warning("Stop time-recovery when start live \n");
				MGR_TIME_ClockRecoverStop();
			}
			break;

		case eMEVT_BUS_DESTROY:
			BUS_PostMessage(NULL, eMEVT_TIME_CLOCKRECOVER_SETUP_DONE, 0, 0, 0, 0);

			/* Release resources. This should be synchronous operation */
			if(bActionFail == FALSE)
			{
				hErr = SVC_CH_StopTune(s_hClockRecoverAction, -1, eSyncMode);
				if (ERR_OK != hErr)
				{
					HxLOG_Critical("Critical Eror!!!\n");
				}

				hErr = SVC_SI_StopTimeupdate(s_hClockRecoverAction, FALSE);
				if (ERR_OK != hErr)
				{
					HxLOG_Critical("Critical Eror!!!\n");
				}

				/* Release Action */
				MGR_ACTION_Release(s_ulClockRecoverActionId, eActionType_SEARCH);
			}

			{
				VK_CLOCK_DST_MODE 	eDstMode;

				if (ERR_OK == MGR_TIME_ClockRecoverGetDstMode(&eDstMode))
				{
					VK_CLOCK_SetDstMode(eDstMode);
				}
			}

			/* kill timer */
			BUS_KillTimer(CLOCK_SETUP_TIMER_ID);
			HxLOG_Print("\n\t********** proc_time_ClockRecoverAction() Destroy. **********\n");

			return MESSAGE_BREAK;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

STATIC BUS_Callback_t	mgr_time_getTimeUpdateProc(void)
{
	return proc_time_ClockRecoverAction;
}

HERROR MGR_TIME_ClockRecoverGetDstMode (VK_CLOCK_DST_MODE *peDstMode)
{
	if (NULL == peDstMode)
		return ERR_FAIL;

	return pmgr_time_ClockRecoverGetDstMode (peDstMode);
}

HERROR MGR_TIME_ClockRecoverTune (DxTuneParam_t *pTuningInfo)
{
	SvcRsc_Info_t			stRscInfo;
	HERROR					hErr;
	HUINT32					ulDemuxId;
	SvcSi_TimeUpdateSpec_t  stTimeUpdateSpec;

	HxLOG_Print("[Clock Setup] MGR_TIME_ClockRecoverTune ()\n");

	/* action에 잡힌 상태에서 serach setup 하면 App을 죽인다. */
	/* Tune to the reference TP */

	s_hClockRecoverAction = SVC_PIPE_IncreaseActionVersion(s_ulClockRecoverActionId);

	MGR_RSC_GetResourceInfo (s_ulClockRecoverActionId, &stRscInfo);
	hErr = SVC_CH_StartTune(s_hClockRecoverAction, &stRscInfo, -1, pTuningInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("Critical Eror!!!\n");
		return hErr;
	}

	hErr = MGR_RSC_GetResourceId (s_ulClockRecoverActionId, eRxRSCTYPE_DEMUX, &ulDemuxId);
	if(hErr != ERR_OK)
	{
		return hErr;
	}

	HxSTD_MemSet(&stTimeUpdateSpec, 0x00, sizeof(SvcSi_TimeUpdateSpec_t));
	hErr = mgr_time_GetTimeupdateSiSpec(&stTimeUpdateSpec);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("Critical Eror!!!\n");
		return hErr;
	}

	if(stTimeUpdateSpec.ulActionSpec == 11) //spec 을 가져올 방법이 존재하나....
	{
		hErr = SVC_SI_StartTimeupdate(s_hClockRecoverAction, &stTimeUpdateSpec, TRUE /* TOT */, FALSE /* sync */);
	}
	else
	{
		hErr = SVC_SI_StartTimeupdate(s_hClockRecoverAction, &stTimeUpdateSpec, FALSE /* TOT */, FALSE /* sync */);
	}
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("Critical Eror!!!\n");
	}

	return hErr;
}

HERROR	MGR_TIME_ClockRecoverStart(void)
{
	BUS_Result_t	eRes = ERR_BUS_NO_ERROR;

	if (NULL == BUS_MGR_Get (mgr_time_getTimeUpdateProc()))
	{
		eRes = BUS_MGR_Create("proc_time_ClockRecover", APP_ACTION_PRIORITY, mgr_time_getTimeUpdateProc(), HANDLE_NULL, 0, 0, 0);
	}

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR	MGR_TIME_ClockRecoverStop(void)
{
	BUS_Result_t	eRes;

	eRes = BUS_MGR_Destroy(mgr_time_getTimeUpdateProc());
	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR	MGR_TIME_SetGmtOffset(HINT32 nGmtOffset, HBOOL bUpdateTzFile)
{
	return pmgr_time_SetGmtOffset(nGmtOffset, bUpdateTzFile);
}

HERROR MGR_TIME_Init (void)
{
	return pmgr_time_Init ();
}

// For Batch.
BUS_Callback_t			MGR_TIME_InitBatch(void **ppvUserData, HUINT32 *pulUserDataLength, DxBatchType_e eBatchType)
{

	HxLOG_Trace();
	HxLOG_Debug("START MGR_TIME_ClockRecoverStart \n");
	MGR_TIME_ClockRecoverStart();

	return mgr_time_getTimeUpdateProc();
}

HERROR			MGR_TIME_DeinitBatch(void *pvUserData, HUINT32 ulUserDataLength, DxBatchType_e eBatchType)
{
	HERROR	hErr = ERR_FAIL;

	MGR_TIME_ClockRecoverStop();

	return hErr;
}

HERROR			MGR_TIME_RescheduleBatch(DxBatchType_e eBatchType)
{
	HxLOG_Trace();

	return ERR_OK;
}

HERROR MGR_TIME_GetBatchTimeRecoveryCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList)
{
	HUINT32 ulUnixTime;
	HERROR	hErr;

	HxLOG_Trace();

	hErr = VK_CLOCK_GetTime((unsigned long*)&ulUnixTime) ;
	if ( VK_OK == hErr	&& HLIB_DATETIME_IsValidUnixTime(ulUnixTime) == ERR_OK )
	{
		fnBatchProcCBList->fnAppInit = NULL;
		fnBatchProcCBList->fnAppDeinit = NULL;
	}
	else
	{
		fnBatchProcCBList->fnAppInit = MGR_TIME_InitBatch;
		fnBatchProcCBList->fnAppDeinit = MGR_TIME_DeinitBatch;
	}
	fnBatchProcCBList->fnAppReSchedule = MGR_TIME_RescheduleBatch;
	return ERR_OK;
}

HERROR MGR_TIME_GetBatchAllTimeRecoveryCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList)
{
	HUINT32 ulUnixTime;
	HERROR	hErr;

	HxLOG_Trace();

	hErr = VK_CLOCK_GetTime((unsigned long*)&ulUnixTime) ;
	if (0)// VK_OK == hErr	&& HLIB_DATETIME_IsValidUnixTime(ulUnixTime) == ERR_OK )
	{
		fnBatchProcCBList->fnAppInit = NULL;
		fnBatchProcCBList->fnAppDeinit = NULL;
	}
	else
	{
		fnBatchProcCBList->fnAppInit = MGR_TIME_InitBatch;
		fnBatchProcCBList->fnAppDeinit = MGR_TIME_DeinitBatch;
	}
	fnBatchProcCBList->fnAppReSchedule = MGR_TIME_RescheduleBatch;
	return ERR_OK;
}

HERROR MGR_TIME_GetBatchTimeRecoveryTime(UNIXTIME *ulStartTime, HUINT32 *ulDuration)
{
	HxLOG_Trace();
	return ERR_OK;
}

