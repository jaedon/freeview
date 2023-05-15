/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_action.h>

#include <mgr_live.h>
#include <mgr_cas.h>
#include <mgr_output.h>

#include <svc_pipe.h>
#include <svc_si.h>
#include <svc_cas.h>
#include <svc_epg.h>

#include <namgr_main.h>

#include "local_include/_xmgr_cas_na_util.h"
#include "local_include/_xmgr_cas_na_adapt.h"
#include "local_include/_xmgr_cas_na.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Definitions         **************************/
/*******************************************************************/

#define	UPC_7DAYS_TIMER_ID			(0x01020304)
#define	UPC_7DAYS_TIMER_PERIOD		(1000 * 1)
#ifdef CONFIG_DEBUG
#define	UPC_7DAYS_TIMER_EXPIRED		(60 * 60 * 4) 		/* 4-hours */
#else
#define	UPC_7DAYS_TIMER_EXPIRED		(60 * 60 * 24 * 7)	/* 7-days */
#endif

#define VALID_UNIXTIME				(24 * 60 * 60)

typedef enum
{
	eCasState7Daytimer_Wait = 0,
	eCasState7Daytimer_Ok,
	eCasState7Daytimer_Fail
} XmgrCas_NaState7Daytimer_e;

typedef struct tagUPCSpecContents
{
	HBOOL						bFlagStartTimer;
	HBOOL						bFlagIsExpired;
	HBOOL						bIsRsvTotTdt;
	HBOOL						bIsWaitTotTdt;
	HBOOL						bIsWaitStartAv;
	UNIXTIME					ulCurrentTime;
	UNIXTIME					ulSavedTime;
	XmgrCas_NaState7Daytimer_e	eCasState;
} XmgrCas_NaUpcSpecContents_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC XmgrCas_NaUpcSpecContents_t	s_stUPCSpecContents;

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/

#define _____XMGR_CAS_NA_UPC_LOCAL_FUNCTION______________________________________________________
STATIC XmgrCas_NaUpcSpecContents_t* xmgr_cas_NaGetContents_Upc(void)
{
	return &s_stUPCSpecContents;
}

STATIC HERROR		xmgr_cas_NaRead7DaysTimerTime_Upc (UNIXTIME *pulSavedTime)
{
	HERROR			hErr;

	if (pulSavedTime == NULL)
	{
		return ERR_FAIL;
	}

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_UPC_7DAY_TIMEDATA, (HUINT32 *)pulSavedTime, sizeof(UNIXTIME));
	if (hErr != ERR_OK)
	{
		*pulSavedTime = 0;
	}

	return hErr;
}

STATIC HERROR		xmgr_cas_NaWrite7DaysTimerTime_Upc (UNIXTIME ulSavedTime)
{
	HERROR			hErr;

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_UPC_7DAY_TIMEDATA, ulSavedTime, sizeof(UNIXTIME));
	if (hErr == ERR_OK)
	{
		DB_PARAM_Sync();
	}

	return hErr;
}

STATIC void			xmgr_cas_NaPrintTime_Upc (HUINT32 ulCurrTime, HUINT32 ulSavedTime)
{
#ifdef CONFIG_DEBUG
	HxDATETIME_t 	t1, t2;
	HLIB_DATETIME_ConvertUnixTimeToDateTime (ulCurrTime, &t1);
	HLIB_DATETIME_ConvertUnixTimeToDateTime (ulSavedTime, &t2);

	HxLOG_Print("CurrentTime : %d/%d\t %d:%d:%d\n", t1.stDate.ucMonth, t1.stDate.ucDay, t1.stTime.ucHour, t1.stTime.ucMinute, t1.stTime.ucSecond);
	HxLOG_Print("SavedTime   : %d/%d\t %d:%d:%d\n", t2.stDate.ucMonth, t2.stDate.ucDay, t2.stTime.ucHour, t2.stTime.ucMinute, t2.stTime.ucSecond);
	HxLOG_Print("DiffTime : %d\n", ulCurrTime - ulSavedTime);
#endif
}

STATIC HBOOL		xmgr_cas_NaIsExpired_Upc (UNIXTIME ulSavedTime, UNIXTIME ulCurrTime)
{
	xmgr_cas_NaPrintTime_Upc (ulCurrTime, ulSavedTime);

	return (((ulCurrTime - ulSavedTime) >= UPC_7DAYS_TIMER_EXPIRED) ? TRUE : FALSE);
}

STATIC HERROR		xmgr_cas_NaGetCurrentTime_Upc(HUINT32 *pulCurrTime)
{
	HERROR			hErr;

	hErr = VK_CLOCK_GetTime ((unsigned long*)pulCurrTime);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical ("VK_CLOCK_GetTime Error\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HBOOL		xmgr_cas_NaCheckValidUnixTime_Upc (HUINT32 ulTime)
{
	return ((ulTime > VALID_UNIXTIME) ? TRUE : FALSE);
}

STATIC HERROR		xmgr_cas_NaStopTimer_Upc (XmgrCas_NaUpcSpecContents_t *pstContents, HBOOL bClearTime)
{
	HxLOG_Print("+++\n");

	pstContents->bFlagStartTimer = FALSE;

	BUS_KillTimer (UPC_7DAYS_TIMER_ID);

	if (bClearTime == TRUE)
	{
		pstContents->bFlagIsExpired = FALSE;
		xmgr_cas_NaWrite7DaysTimerTime_Upc (0);
		BUS_SendMessage(NULL, eMEVT_XMGR_CAS_NA_7DAYTIMER_AV_CONTROL, MGR_ACTION_GetMainActionId(), 0, 0, 0);
	}

	HxLOG_Print("---\n");

	return ERR_OK;
}

STATIC HERROR		xmgr_cas_NaStartTimer_Upc (XmgrCas_NaUpcSpecContents_t *pstContents, Handle_t hSvc)
{
	HBOOL				bIsScrambledSvc = FALSE;
	HBOOL				bIsStartTimer = FALSE;
	SvcCas_ChannelType_e	eChannelType = eCasChannel_FTA;
	Handle_t			hActionHandle;
	HUINT32				ulCurrTime = 0;
	HUINT32				ulSaveTime = 0;
	HERROR				hErr;

	HxLOG_Print("+++\n");

	if (hSvc == NULL)
	{
		hActionHandle = SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId());
		if (hActionHandle != (Handle_t)NULL)
		{
			hErr = SVC_CAS_GetChannelType(hActionHandle, &eChannelType);
			if (hErr != ERR_OK)		return ERR_FAIL;
		}
		else
		{
			HxLOG_Critical ("SVC_PIPE_GetActionHandle Error\n");
			return ERR_FAIL;
		}
	}
	else
	{
		// Zapping 시작 시 channel type
		DbSvc_Info_t stSvcInfo;

		hErr = DB_SVC_GetServiceInfo (hSvc, &stSvcInfo);
		if (hErr == ERR_OK)
		{
			if (DbSvc_GetCasType(&stSvcInfo) == eDxCAS_TYPE_FTA)		eChannelType = eCasChannel_FTA;
			else 													eChannelType = eCasChannel_Scramble;
		}
	}

	HxLOG_Print("eCasState : %d, bFlagStartTimer : %d, Channel Type : %d\n",
							pstContents->eCasState, pstContents->bFlagStartTimer, eChannelType));

	if (eChannelType == eCasChannel_Scramble)
	{
		bIsScrambledSvc = TRUE;
	}

	if (bIsScrambledSvc == FALSE) // FTA일 경우..
	{
		if (pstContents->bIsWaitStartAv == TRUE) // MSG_APP_AV_STARTED가 Called 되면...
		{
			HxLOG_Print("FTA Channel, Start AV!\n");
			bIsStartTimer = TRUE;
		}
	}
	else
	{
		if (pstContents->eCasState != eCasState7Daytimer_Wait) // haven't received any cas event yet.
		{
			if (pstContents->eCasState == eCasState7Daytimer_Ok) // EVT_CAS_OK가 올라오면, timer stop!
			{
				HxLOG_Print("eMEVT_CAS_OK Received, Stop Timer!\n");
				xmgr_cas_NaStopTimer_Upc (pstContents, TRUE);
			}
			else
			{
				HxLOG_Print("Scramble Channel, eMEVT_CAS_FAIL Received!\n");
				bIsStartTimer = TRUE; // EVT_CAS_FAIL이면 start!
			}
		}
	}

	// Condition 1: not stated timer,
	// Condition 2: not expired timer,
	if ((bIsStartTimer == TRUE) && (pstContents->bFlagStartTimer == FALSE) &&
		(pstContents->bFlagIsExpired == FALSE))
	{
		HxLOG_Print("Enter Timer Process!!!\n");

		// 현재 시간을 불러옴.
		hErr = xmgr_cas_NaGetCurrentTime_Upc (&ulCurrTime);
		if (hErr != ERR_OK)		return ERR_FAIL;

		// 저장된 시간을 불러옴.
		xmgr_cas_NaRead7DaysTimerTime_Upc (&ulSaveTime);

		// 저장된 시간이 없을 경우.
		if ((ulSaveTime == 0) || (hErr != ERR_OK))
		{
			HxLOG_Print("saved time is 0 or error\n");

			ulSaveTime = ulCurrTime;
			xmgr_cas_NaWrite7DaysTimerTime_Upc (ulCurrTime);
		}

		xmgr_cas_NaPrintTime_Upc (ulCurrTime, ulSaveTime);

		pstContents->bFlagStartTimer = TRUE;
		pstContents->bFlagIsExpired = FALSE;
		pstContents->ulSavedTime = ulSaveTime;

		// 현재 시간이 valid하면 Counting을 바로 시작함.
		if (xmgr_cas_NaCheckValidUnixTime_Upc (ulCurrTime) == TRUE)
		{
			HxLOG_Print("Start Timer!!!\n");

			BUS_KillTimer (UPC_7DAYS_TIMER_ID);
			BUS_SetTimer (UPC_7DAYS_TIMER_ID, UPC_7DAYS_TIMER_PERIOD);
		}
		else
		{
			// 현재 시간이 invalid하더라도 saved된 시간이 invalid면
			// Counting을 시작함.
			if (xmgr_cas_NaCheckValidUnixTime_Upc (ulSaveTime) == FALSE)
			{
				BUS_KillTimer (UPC_7DAYS_TIMER_ID);
				BUS_SetTimer (UPC_7DAYS_TIMER_ID, UPC_7DAYS_TIMER_PERIOD);
			}
			else
			{
				// Waitting TDT/TOT
				HxLOG_Print("Waiting TOT/TDT!!!\n");
				pstContents->bIsWaitTotTdt = TRUE;
			}
		}

	}

	HxLOG_Print("---\n");

	return ERR_OK;
}

STATIC HERROR		xmgr_cas_NaRestartTimer_Upc (XmgrCas_NaUpcSpecContents_t *pstContents, HUINT32 ulPrevUnixTime)
{
	HUINT32		ulCurrTime = 0;
	HUINT32	 	ulSaveTime = 0;
	HINT32		nElapsedTime = 0;
	HBOOL		bStartedTimer = FALSE;
	HERROR		hErr;

	HxLOG_Print("+++\n");

	// 현재 시간을 불러옴.
	hErr = xmgr_cas_NaGetCurrentTime_Upc (&ulCurrTime);
	if (hErr != ERR_OK) 	return ERR_FAIL;

	bStartedTimer = pstContents->bFlagStartTimer;

	// Timer동작 중이면 stop
	if (bStartedTimer)
	{
		xmgr_cas_NaStopTimer_Upc (pstContents, FALSE);
	}
	else
	{
		return ERR_OK;
	}

	hErr = xmgr_cas_NaRead7DaysTimerTime_Upc (&ulSaveTime);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical ("Read7DaysTimerTime Error\n");
	}

	if (pstContents->bIsWaitTotTdt == TRUE)
	{
		// Current TIme이 invalid이고,
		// saved time이 valid이면 처음 tot/tdt를 받고
		// 다음부터 무조건 offset을 더해야함
		pstContents->bIsWaitTotTdt = FALSE;
	}
	else
	{
		// 경과된 시간을 가져옴..
		nElapsedTime = ulPrevUnixTime - ulSaveTime;

		// Elapsed Time이 minus이면 안됨.
		if (nElapsedTime < 0)
		{
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}

		// 현재 시간에서 경과 시간을 더함..
		ulSaveTime = ulCurrTime - nElapsedTime;

		xmgr_cas_NaPrintTime_Upc (ulCurrTime, ulSaveTime);

		if ((ulPrevUnixTime - ulCurrTime) != 0)
		{
			xmgr_cas_NaWrite7DaysTimerTime_Upc (ulSaveTime);
		}
	}

	if (bStartedTimer)
	{
		xmgr_cas_NaStartTimer_Upc (pstContents, NULL);
	}

	HxLOG_Print("---\n");

	return ERR_OK;
}



// eMEVT_BUS_CREATE
STATIC BUS_Result_t xmgr_cas_NaMsgCreate_Upc (XmgrCas_NaUpcSpecContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulCurrTime = 0;

	// Initialize all variable
	HxSTD_memset (pstContents, 0, sizeof(XmgrCas_NaUpcSpecContents_t));

	pstContents->eCasState = eCasState7Daytimer_Wait;

	xmgr_cas_NaGetCurrentTime_Upc (&ulCurrTime);
	if (xmgr_cas_NaCheckValidUnixTime_Upc (ulCurrTime))
	{
		pstContents->bIsRsvTotTdt = TRUE;
	}

	return MESSAGE_BREAK;
}

// eSEVT_SI_TOT / eSEVT_SI_TDT
STATIC BUS_Result_t xmgr_cas_NaMsgTdtTot_Upc (XmgrCas_NaUpcSpecContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	pstContents->bIsRsvTotTdt = TRUE;

	xmgr_cas_NaRestartTimer_Upc (pstContents, (HUINT32)p1);

	return ERR_BUS_NO_ERROR;
}


// eMEVT_CAS_OK
STATIC BUS_Result_t xmgr_cas_NaEvtCasOk_Upc (XmgrCas_NaUpcSpecContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	pstContents->eCasState = eCasState7Daytimer_Ok;

	xmgr_cas_NaStartTimer_Upc (pstContents, NULL);

	return ERR_BUS_NO_ERROR;
}

// eMEVT_CAS_FAIL
STATIC BUS_Result_t xmgr_cas_NaEvtCasFail_Upc (XmgrCas_NaUpcSpecContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	pstContents->eCasState = eCasState7Daytimer_Fail;

	xmgr_cas_NaStartTimer_Upc (pstContents, NULL);

	return ERR_BUS_NO_ERROR;
}

// eMEVT_BUS_TIMER
STATIC BUS_Result_t xmgr_cas_NaMsgTimer_Upc (XmgrCas_NaUpcSpecContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		hErr;
	HBOOL		bIsExpired = FALSE;
	HBOOL		bIsBothInvalidTime = FALSE;
	HUINT32		ulCurrTime = 0;
	HUINT32		ulSaveTime = 0;

	if (p1 == UPC_7DAYS_TIMER_ID)
	{
		hErr = xmgr_cas_NaGetCurrentTime_Upc (&ulCurrTime);
		if (hErr != ERR_OK) 	return ERR_FAIL;

		ulSaveTime = pstContents->ulSavedTime;

		// expired 되었는지 체크함
		bIsExpired = xmgr_cas_NaIsExpired_Upc (ulSaveTime, ulCurrTime);

		// Invalid time인지 체크함.
		if (xmgr_cas_NaCheckValidUnixTime_Upc (ulCurrTime) == FALSE &&
			xmgr_cas_NaCheckValidUnixTime_Upc (ulSaveTime) == FALSE)
		{
			// Tot를 받지 못하고 expired 된 경우 체크함
			bIsBothInvalidTime = TRUE;
		}

		if ((bIsExpired == TRUE) && (
			(pstContents->bIsRsvTotTdt == TRUE) || (bIsBothInvalidTime == TRUE)))
		{
			HxLOG_Print("EXPIRED!\n");

			pstContents->bFlagIsExpired = TRUE;
			pstContents->bFlagStartTimer = FALSE;

			BUS_KillTimer (p1);

			BUS_SendMessage(NULL, eMEVT_XMGR_CAS_NA_7DAYTIMER_AV_CONTROL, MGR_ACTION_GetMainActionId(), 1, 0, 0);
		}

		return MESSAGE_BREAK;
	}
	return ERR_BUS_NO_ERROR;
}


// eMEVT_LIVE_NOTIFY_SVC_CHANGED
STATIC BUS_Result_t xmgr_cas_NaMsgSvcChanged_Upc (XmgrCas_NaUpcSpecContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	pstContents->bIsWaitStartAv = FALSE;

	xmgr_cas_NaStartTimer_Upc (pstContents, (Handle_t)p1);

	return ERR_BUS_NO_ERROR;
}


// eMEVT_OUTPUT_AV_STARTED
STATIC BUS_Result_t xmgr_cas_NaMsgAvStarted_Upc (XmgrCas_NaUpcSpecContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	pstContents->bIsWaitStartAv = TRUE;

	xmgr_cas_NaStartTimer_Upc (pstContents, (Handle_t)p1);

	return ERR_BUS_NO_ERROR;
}


// eMEVT_BUS_DESTROY
STATIC BUS_Result_t xmgr_cas_NaMsgDestroy_Upc (XmgrCas_NaUpcSpecContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_KillTimer (UPC_7DAYS_TIMER_ID);

	return MESSAGE_BREAK;
}


BUS_Result_t xmgr_cas_Na7DayMonitorProc_Upc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		eGwmResult = ERR_BUS_NO_ERROR;
	XmgrCas_NaUpcSpecContents_t *pstContents =  xmgr_cas_NaGetContents_Upc();

	switch(message)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Print("eMEVT_BUS_CREATE [p1:0x%x]\n", p1);
			eGwmResult = xmgr_cas_NaMsgCreate_Upc (pstContents, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_TIMER :
			HxLOG_Print("eMEVT_BUS_TIMER [p1:0x%x]\n", p1);
			eGwmResult = xmgr_cas_NaMsgTimer_Upc (pstContents, hAction, p1, p2, p3);
			break;

		case eSEVT_SI_TOT :
		case eSEVT_SI_TDT :
			HxLOG_Print("eSEVT_SI_TOT / eSEVT_SI_TDT [p1:0x%x]\n", p1);
			eGwmResult = xmgr_cas_NaMsgTdtTot_Upc (pstContents, hAction, p1, p2, p3);
			break;

		case eMEVT_CAS_OK :
			HxLOG_Print("eMEVT_CAS_OK [p1:0x%x]\n", p1);
			eGwmResult = xmgr_cas_NaEvtCasOk_Upc (pstContents, hAction, p1, p2, p3);
			break;

		case eMEVT_CAS_FAIL :
			HxLOG_Print("eMEVT_CAS_FAIL [p1:0x%x]\n", p1);
			eGwmResult = xmgr_cas_NaEvtCasFail_Upc (pstContents, hAction, p1, p2, p3);
			break;

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED :
			HxLOG_Print("eMEVT_LIVE_NOTIFY_SVC_CHANGED [p1:0x%x]\n", p1);
			eGwmResult = xmgr_cas_NaMsgSvcChanged_Upc (pstContents, hAction, p1, p2, p3);
			break;

		case eMEVT_OUTPUT_AV_STARTED :
			HxLOG_Print("eMEVT_OUTPUT_AV_STARTED [p1:0x%x]\n", p1);
			eGwmResult = xmgr_cas_NaMsgAvStarted_Upc (pstContents, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_GO_OPERATION :
			HxLOG_Print("eMEVT_BUS_GO_OPERATION [p1:0x%x]\n", p1);
			eGwmResult = ERR_BUS_NO_ERROR;
			BUS_MGR_Destroy (NULL);
			break;

		case eMEVT_BUS_DESTROY :
			HxLOG_Print("eMEVT_BUS_DESTROY [p1:0x%x]\n", p1);
			eGwmResult = xmgr_cas_NaMsgDestroy_Upc (pstContents, hAction, p1, p2, p3);
			break;

		default :
			break;
	}

	if( (eGwmResult == MESSAGE_BREAK) || (eGwmResult == MESSAGE_PASS) )
	{
		return eGwmResult;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


BUS_Result_t	xproc_cas_NaCreate7DayTimer_Upc(HINT32 nBootUiType)
{
	if (BUS_MGR_Get(xmgr_cas_Na7DayMonitorProc_Upc) != NULL)
		BUS_MGR_Destroy (xmgr_cas_Na7DayMonitorProc_Upc);

	return BUS_MGR_Create ("xmgr_cas_Na7DayMonitorProc_Upc", APP_MONITORING_MGR_PRIORITY,
							xmgr_cas_Na7DayMonitorProc_Upc, 0, nBootUiType, 0, 0);
}


/* end of file */

