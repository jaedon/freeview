/**
	@file     mgr_logcapture.c
	@brief    file_name & simple comment.

	Description:
	Module:  \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2009/04/16		modified					wjmoh\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <octo_common.h>

#include <svc_logcapture.h>
#include <mgr_logcapture.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define LOGCAPTURE_CAS_INIT_TIMER_ID			0x40011000
#define LOGCAPTURE_CAS_INIT_TIMER				3000			//3초 타이머 설정

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	EVT_LCSA_CAS_INIT = 0,
	EVT_LCSA_CAS_READ,
	EVT_LCSA_CAS_WRITE,
	EVT_LCSA_EPG_UPDATE_START,
	EVT_LCSA_EPG_UPDATE_FINISH,
	EVT_LCSA_EPG_UPDATE_RESULT,

	EVT_LCSA_MAX,
} EVT_LC_SUB_ACTION_e;

typedef enum
{
	EVT_LC_ACTION_USB 				= 0,
	EVT_LC_ACTION_CAS_ERROR 		,
	EVT_LC_ACTION_APPLICATION_ERROR ,
	EVT_LC_ACTION_CPU_TEMPORATURE 	,
	EVT_LC_ACTION_VIDEO_OUTPUT 		,
	EVT_LC_ACTION_AUDIO_OUTPUT,
	EVT_LC_ACTION_MEMORY_CONSUMPTION,
	EVT_LC_ACTION_BOOTING_INFO,
	EVT_LC_ACTION_INBAND_STATUS,
	EVT_LC_ACTION_EPG_UPDATE,
	EVT_LC_ACTION_STB_LIFECYCLE,
	EVT_LC_ACTION_3PA_INSTALLATION,
	EVT_LC_ACTION_RMCON_EVENT,
	EVT_LC_ACTION_xxx = 0x1000,

	EVT_LC_ACTION_MAX,
} EVT_LC_ACTION_e;

typedef struct
{
	Handle_t		hAction;

	HULONG			ulCasErrorTimerID;
	HBOOL			bIgnoreCasInitError;

} Log_Context_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC Log_Context_t		s_stLogContext;


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

STATIC Log_Context_t*	mgr_logcapture_GetContext(void)
{
	return &s_stLogContext;
}

STATIC HERROR			mgr_logcapture_InitContext(void)
{
	HxSTD_MemSet(&s_stLogContext, 0x00, sizeof(Log_Context_t));

	return ERR_OK;
}
STATIC HERROR			mgr_logcapture_Destroy(void)
{
	HxSTD_MemSet(&s_stLogContext, 0x00, sizeof(Log_Context_t));
	return ERR_OK;
}

// CAS Init Error의 경우, 3번 이상 올라와서, 처음것만 유효하게 처리함
STATIC HERROR mgr_logcapture_ResetIgnoreCasInitErrorFlag(void)
{
	Log_Context_t	*pstContext = NULL;
	pstContext = mgr_logcapture_GetContext();

	HxLOG_Debug("[%s]\r\n",__func__);
	pstContext->bIgnoreCasInitError = FALSE;

	BUS_KillTimer(pstContext->ulCasErrorTimerID);

	return ERR_OK;
}

STATIC HERROR mgr_logcapture_SetTimerForIgnoreDuplicatedError(void)
{
	Log_Context_t	*pstContext = NULL;
	pstContext = mgr_logcapture_GetContext();

	BUS_SetTimer(LOGCAPTURE_CAS_INIT_TIMER_ID, LOGCAPTURE_CAS_INIT_TIMER);
	pstContext->ulCasErrorTimerID = LOGCAPTURE_CAS_INIT_TIMER_ID;

	return ERR_OK;
}

STATIC HERROR	mgr_logcapture_NotifyCasError(HINT32 p1, HINT32 p2, HINT32 p3)
{
	Log_Context_t	*pstContext = NULL;

	pstContext = mgr_logcapture_GetContext();

	HxLOG_Debug("CasType(%s), Error(%s)\r\n",
				(p1==0)?"B-CAS":"C-CAS",(p2==eLOGCAPTURE_ERR_CAS_SC_ATR)?"INIT":((p2==eLOGCAPTURE_ERR_CAS_SC_TRANSMIT)?"WRITE":"READ"));

	if(pstContext->bIgnoreCasInitError == FALSE)
	{
		pstContext->bIgnoreCasInitError = TRUE;

		HLIB_CMD_CaptureLog("CAS Init Error,%s",(p1==0)?"B-CAS":"C-CAS");

		mgr_logcapture_SetTimerForIgnoreDuplicatedError();
	}

	return ERR_OK;
}

STATIC HERROR	mgr_logcapture_NotifyUSBInsertion(HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 ulDeviceId = 0;

	ulDeviceId = (HUINT32)p1;

	SVC_LogCapture_NotifyUsbInsertion(ulDeviceId);

	return ERR_OK;
}

STATIC HERROR	mgr_logcapture_NotifySiTimeUdate(void)
{
	HLIB_CMD_CaptureLog("Si Time Update");

	return ERR_OK;
}

STATIC HERROR mgr_logcapture_SetTunerGroupID(HINT32 ulTunerGroupId, HINT32 ulConstellation)
{
	SVC_LogCapture_SetTunerGroupID((HUINT32)ulTunerGroupId, ulConstellation);
	return ERR_OK;
}

STATIC HERROR mgr_logcapture_SetChannelState(HINT32 nState, HINT32 nNoSignal)
{
	if(nState == nNoSignal)
	{
		SVC_LogCapture_SetChannelState(CHANNEL_STATE_NO_SIGNAL);
	}
	else
	{
		SVC_LogCapture_SetChannelState(nState);
	}

	return ERR_OK;
}

STATIC HERROR mgr_logcapture_SetChannelChangeAction(void)
{
	SVC_LogCapture_SetChannelChangeAction();
	return ERR_OK;
}

STATIC HERROR mgr_logcapture_LogFileBackup(void)
{
	SVC_LogCapture_LogFileBackup();
	return ERR_OK;
}

STATIC BUS_Result_t	 proc_logcapture(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	nResult = MESSAGE_PASS;
	Log_Context_t	*pstContext = NULL;

	/* get context */
	pstContext = mgr_logcapture_GetContext();
	if(pstContext == NULL)
	{
		HxLOG_Critical("\n\n");
	}

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Debug("[AP] - mgr_logcapture_Proc CREATE! \r\n");
			mgr_logcapture_InitContext();
			break;
		case eMEVT_BUS_DESTROY:
			mgr_logcapture_Destroy();
			break;
		case eMEVT_BUS_READY_SHUTDOWN :
			HLIB_CMD_CaptureLog("Ready Shutdown");
			break;
		case eMEVT_BUS_GO_OPERATION :
			HLIB_CMD_CaptureLog("eMEVT_BUS_GO_OPERATION");
			break;

		case eMEVT_BUS_TIMER:
			HxLOG_Print("eMEVT_BUS_TIMER received!!\n");
			if(p1 == LOGCAPTURE_CAS_INIT_TIMER_ID)
			{
				mgr_logcapture_ResetIgnoreCasInitErrorFlag();
				nResult = MESSAGE_BREAK;
			}
			break;

		/**********************************************************************
			APP MESSAGES
		**********************************************************************/
		case eMEVT_LOGCAPTURE_USB_DEVICE_INSERTION:
			HxLOG_Debug("[AP] - eMEVT_LOGCAPTURE_USB_DEVICE_INSERTION\r\n");
			mgr_logcapture_NotifyUSBInsertion(p1,p2,p3);
			nResult = MESSAGE_BREAK;
			break;

		/**********************************************************************
			EVENT MESSAGES
		**********************************************************************/
		case eSEVT_SI_TDT :
		case eSEVT_SI_TOT :
			HxLOG_Debug("[AP] - eSEVT_SI_TDT / eSEVT_SI_TOT\r\n");
			mgr_logcapture_NotifySiTimeUdate();
			nResult = MESSAGE_PASS;
			break;

		case eSEVT_LOGCAPTURE_CAS_ERROR:
			HxLOG_Debug("[AP] - EVT_LOGCAPTURE_CAS_ERROR_INIT\r\n");
			mgr_logcapture_NotifyCasError(p1,p2,p3);
			nResult = MESSAGE_BREAK;
			break;
		case eSEVT_LOGCAPTURE_CHANNEL_INFO:
			HxLOG_Debug("[AP] - EVT_LOGCAPTURE_TUNERGROUPID p1(%d), p2(%d)\r\n",p1,p2);
			mgr_logcapture_SetTunerGroupID(p1, p2);
			nResult = MESSAGE_BREAK;
			break;
		case eSEVT_LOGCAPTURE_CHANNEL_STATE:
			HxLOG_Debug("[AP] - eSEVT_LOGCAPTURE_CHANNEL_STATE p1(%d), p2(%d)\r\n",p1,p2);
			mgr_logcapture_SetChannelState(p1,p2);
			nResult = MESSAGE_BREAK;
			break;
		case eSEVT_LOGCAPTURE_CHANNEL_CHANGE:
			HxLOG_Debug("[AP] - eSEVT_LOGCAPTURE_CHANNEL_CHANGE \r\n");
			mgr_logcapture_SetChannelChangeAction();
			nResult = MESSAGE_BREAK;
			break;
		case eSEVT_LOGCAPTURE_LOGFILE_BACKUP:
			HxLOG_Debug("[AP] - eSEVT_LOGCAPTURE_LOGFILE_BACKUP \r\n");
			mgr_logcapture_LogFileBackup();
			nResult = MESSAGE_BREAK;
			break;
		default:
			break;

	}
	return nResult;
}

#define _____PUBLIC_APIs_______________________________________________________________


#define _____GLOBAL_APIs_______________________________________________________________

HERROR	MGR_Logcapture_Start(void)
{
#if defined(CONFIG_MW_LOGCAPTURE)
	HxLOG_Debug("[MGR] - MGR_Logcapture_Start()\r\n");
	BUS_MGR_Create ("proc_logcapture", APP_LOGCAPTURE_PRIORITY, proc_logcapture, HANDLE_NULL, 0, 0, 0);
	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}


HERROR	MGR_Logcapture_GetReport (void)
{
#if defined(CONFIG_MW_LOGCAPTURE)
	return pmgr_logcapture_GetReport();
#else
	return ERR_FAIL;
#endif
}

