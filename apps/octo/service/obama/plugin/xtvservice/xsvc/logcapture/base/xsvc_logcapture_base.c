/**
	@file     log_capture_manager.c
	@brief    log_capture_manager.c

	Description: \n
	Module: \n
	Remarks :\n

	Copyright (c) 2010 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <octo_config.h>
#include <pal_video.h>
#include <pal_audio.h>
#include <pal_channel.h>
#include <pal_sys.h>
#include <pal_cm.h>

#include <svc_logcapture.h>
#include <psvc_logcapture.h>

#include "local_include/_xsvc_logcapture.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define LOG_CAPTURE_MSGQ_SIZE			64
#define LOG_CAPTURE_TASK_PRIORITY		VK_TASK_PRIORITY_MW_REF
#define LOG_CAPTURE_TASK_STACK_SIZE		SIZE_64K

#define LOG_CAPTURE_FILESIZE_THRESHOLD	204800	//100K

//-------------------- MONITORING TIMER TIME VALUE --------------------//
#define LOG_CAPTURE_MONITORING_TIMER_CPU						300000			//5min
#define LOG_CAPTURE_MONITORING_TIMER_CM							300000			//5min
#define LOG_CAPTURE_MONITORING_TIMER_MEMORY						180000			//3min
#define LOG_CAPTURE_MONITORING_TIMER_INBAND_RS					5000			//5Sec		// receive sensitivity
#define LOG_CAPTURE_MONITORING_TIMER_CM_UPSTREAM_RS				5000
#define LOG_CAPTURE_MONITORING_TIMER_CM_DOWNSTREAM_RS			5000

//-------------------- TIMER TIME VALUE --------------------//
#define LOG_CAPTURE_TIMER_AUDIOERROR_CHECK						3000			// 3sec
#define LOG_CAPTURE_TIMER_VIDEOERROR_CHECK						3000			// 3sec
#define LOG_CAPTURE_TIMER_CPU_TEMPERATURE_CHECK					60000			// 1min

#define LC_TIMER_ID_AUDIOERROR_CHECK								20006
#define LC_TIMER_ID_VIDEOERROR_CHECK								20008
#define LC_TIMER_ID_CPU_TEMPERATURE_CHECK						20010


//-------------------- CONDITIONS --------------------//
#define LOG_CAPTURE_THRESHOLD_CPU_TEMPERATURE					100
#define LOG_CAPTURE_THRESHOLD_CPU_TEMPERATURE_RECHECK			108
#define LOG_CAPTURE_THRESHOLD_CM_TEMPERATURE					100				//	100.0 도
#define LOG_CAPTURE_THRESHOLD_MEMORY_CONSUMPTION				10240			//	10240kb

//-------------------- Receive Sensitivity CONDITIONS --------------------//
#define LOG_CAPTURE_THRESHOLD_RS_SNR_64QAM						25
#define LOG_CAPTURE_THRESHOLD_RS_SNR_256QAM						30
#define LOG_CAPTURE_THRESHOLD_RS_BER							(0.0001)


#define USBHDD_VENDOR_LEN			50
#define USBHDD_PRODUCT_LEN			50
#define USBHDD_SERIALNUM_LEN		50

#if 0
#define LOG_CAPTURE_DLNA_USERAGENT_NAME				64
#define LOG_CAPTURE_DLNA_USERAGENT_CONTENTS_URL		512
#define LOG_CAPTURE_DLNA_USERAGENT_IPADDRESS		16

#define LOG_CAPTURE_3PA_WIDGET_NAME		128
#define LOG_CAPTURE_3PA_WIDGET_URL		512

#define SIZE_OF_LOGFORMAT_HEADER		12
#endif


#define LC_CHANNEL_STATE_NO_SIGNAL		-1

#define LOG_CAPTURE_PACKETERROR_CHECK_THERSHOLD		130		// 100개를 기준점으로 봄,

#define LOG_CAPTURE_AERROR_PACKAGE_INDEX			256
#define LOG_CAPTURE_AERROR_PACKAGE_LENGTH			64
#define AERR_INDEX									LOG_CAPTURE_AERROR_PACKAGE_INDEX
#define AERR_LENGTH									LOG_CAPTURE_AERROR_PACKAGE_LENGTH

#define LOG_CAPTURE_VERROR_PACKAGE_INDEX			1024	// 1024개 이상의 Error Data는 Ignore
#define LOG_CAPTURE_VERROR_PACKAGE_LENGTH			64
#define VERR_INDEX									LOG_CAPTURE_VERROR_PACKAGE_INDEX
#define VERR_LENGTH									LOG_CAPTURE_VERROR_PACKAGE_LENGTH


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eCMD_LC_LOAD_LOG_DATA,
	eCMD_LC_UPDATE_LOG,
	eCMD_LC_NOTIFY_LOG_ACTION,
	eCMD_LC_TIMER_MONITORING,
	eCMD_LC_TIMER_WAITING,


	/* MW Message */
	eeMSG_LC_VIDEO_ERROR_CHECK,
	eMSG_LC_VIDEO_ERROR,
	eMSG_LC_AUDIO_ERROR,

	CMD_LC_Max,

} XsvcLogCapture_Cmd_e;

typedef enum
{
	eLC_ACTION_NONE = 0x00,
	eLC_ACTION_CPU_TEMPERATURE = 0x01,
	eLC_ACTION_CM_TEMPERATURE,
	eLC_ACTION_MEMORY_CONSUMPTION,
	eLC_ACTION_INBAND_RS_STATUS,				//Receive Sensitivity
//5
	eLC_ACTION_CM_DOWNSTREAM_STATUS,			//Receive Sensitivity
	eLC_ACTION_CM_UPSTREAM_STATUS,			//Receive Sensitivity

	eLC_ACTION_MAX,
} XsvcLogCapture_Action_e;

typedef enum
{
	eLC_MONITORING_TIMER_ID_CPU=0,			// For Temperature
	eLC_MONITORING_TIMER_ID_MEMORY,			// For Consumption
	eLC_MONITORING_TIMER_ID_CM,				// For Temperature
	eLC_MONITORING_TIMER_ID_INBAND_RS,		// For Receive Sensitivity
#if defined(NOT_USE_LOGCAPTURE)
	eLC_MONITORING_TIMER_ID_CM_UPSTREAM_RS,			// For Receive Sensitivity
#endif
	eLC_MONITORING_TIMER_ID_CM_DOWNSTREAM_RS,		// For Receive Sensitivity
	eLC_MONITORING_TIMER_ID_MAX,

} XsvcLogCapture_MonitoringTimerId_e;

typedef struct
{
	HUINT32			ulCmd;
	Handle_t		hAction;
	HUINT32			ulParam1;		// Log Type
	HUINT32			ulParam2;
	HUINT32			ulParam3;
	HUINT32			ulParam4;
	HUINT32			ulParam5;
	HUINT32			ulParam6;
	HUINT32			ulParam7;
} XsvcLogCapture_Message_t;

typedef struct
{
	HUINT32			ulCurrentTunerGroupId;		// 수신감도 Monitoring 에서 signal Info 가져올 때 사용
	HINT32			nConstellation;				// 64QAM, 256QAM

	// Monitoring Timer ID...
	HULONG			ulMemInfoTimerID;
	HULONG			ulCPUInfoTimerID;
	HULONG			ulCPUInfoReTimerID;
	HULONG			ulCMInfoTimerID;
	HULONG			ulInBandRSInfoTimerID;			// receive sensitivity
	HULONG			ulCMUpStreamRSInfoTimerID;			// receive sensitivity
	HULONG			ulCMDownStreamRSInfoTimerID;			// receive sensitivity
	HULONG			ulAudioErrorTimerID;
	HULONG			ulVideoErrorTimerID;
	// In-Band
	HBOOL			bLoggingForInBand;
	HINT32			nInBandPacketError;
	HINT32			nInBandSnr;
	HINT32			nInBandBer;

	//Audio
	HUINT32			nAPtsCountErr;
	HUINT32			nAFrameErr;

	HINT32			nAErrorPackIndex;
	HINT32			nAErrorAllCount;
	HINT32			nAErrorAllOldCount;
	HUINT8			aucAErrorPack[AERR_INDEX][AERR_LENGTH];
	HUINT8			aucAErrorPackBuffer[AERR_INDEX][AERR_LENGTH];

	HULONG			ulAudioErrorTimeStamp;

	//Video
	HUINT32			nVDecoderErr;
	HUINT32			nVDiplayErr;

	HINT32			nVErrorPackIndex;
	HINT32			nVErrorAllCount;
	HINT32			nVErrorAllOldCount;
	HUINT8			aucVErrorPack[VERR_INDEX][VERR_LENGTH];
	HUINT8			aucVErrorPackBuffer[VERR_INDEX][VERR_LENGTH];

	HULONG			ulVideoErrorTimeStamp;


	//CM - Down Stream
	HBOOL			bLoggingForDnStream;
	HINT32			nCMPacketError;
	HINT32			nCMSnr;

	// Signal
	HINT32			nChannelState;
	HINT32			nChannelChangeAction;

	//CPU
	HUINT32			ulSavedCpuTemperature;
	HBOOL			bCpuLogWrittenFlag;

} XsvcLogCapture_Context_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC HLONG 				s_ulLogCaptureMsgQId;
STATIC HLONG				s_ulLogCaptureTaskQid;
STATIC HLONG				s_ulLogCaptureSemaId;
STATIC HLONG				s_ulLogCapture2SemaId;
STATIC XsvcLogCapture_Context_t		s_stLogCaptureContext;

STATIC XsvcLogCapture_Context_t*	xsvc_logcapture_GetContext(void);
STATIC HERROR	xsvc_logcapture_InitContext(void);
STATIC HERROR	xsvc_logcapture_GetLogUSBInfo_Base(HUINT32 ulDeviceId);
STATIC HERROR	xsvc_logcapture_SetTunerGroupId_Base(HUINT32 ulTunerGroupId, HINT32 nConstellation);

STATIC HERROR	xsvc_logcapture_SendMsgLogAction(XsvcLogCapture_Action_e eType, HUINT32 param1, HUINT32 param2, HUINT32 param3, HUINT32 param4, HUINT32 param5);
STATIC void		xsvc_logcapture_TimerCallbackForMonitoring(HULONG ulTimerId, void *pLocal );
STATIC void		xsvc_logcapture_TimerCallbackForWaiting(HULONG ulTimerId, void *pLocal );


STATIC HERROR	xsvc_logcapture_CheckCpuInfo(HUINT32 *pulCpuTemp);
STATIC HERROR	xsvc_logcapture_CheckMemInfo(HULONG *pulRemainMem);
STATIC HERROR	xsvc_logcapture_CheckCMTempInfo(HUINT32* pulCurrentTemp);
STATIC HERROR	xsvc_logcapture_CheckInBandReceiveSensitivityInfo(HUINT32* pulBer, HUINT32* pulSnr, HUINT32* pulUncorrectedNo, HUINT32* pulSignalPower);
STATIC HERROR	xsvc_logcapture_CheckCMUpStreamReceiveSensitivityInfo(HUINT32 *pulFreq, HUINT32 *pulSignalPower, HUINT32* pulSymbolrate, HUINT32* pulMod);
STATIC HERROR	xsvc_logcapture_CheckCMDownStreamReceiveSensitivityInfo(HUINT32 *pulFreq, HUINT32* pulSnr, HUINT32 *pulSignalPower, HUINT32* pulMod, HUINT32* pulUnCorrectedNo);
STATIC HERROR	xsvc_logcapture_KillTimer(HULONG ulTimerId);
STATIC HERROR	xsvc_logcapture_SetTimer( Handle_t hAct, HUINT32 ulType, HULONG *pulTimerId, HULONG ulMilliSecond );
STATIC HERROR	xsvc_logcapture_SetTimerForWaiting( Handle_t hAct, HUINT32 ulType, HULONG *pulTimerId, HULONG ulMilliSecond );
STATIC HERROR	xsvc_logcapture_TimerReceived(HUINT32 ulType);
STATIC HERROR	xsvc_logcapture_RegisterTimer(void);
STATIC void		xsvc_logcapture_TimerReceivedForWaiting(HUINT32 ulType);
STATIC HERROR	xsvc_logcapture_ClassifyLogAction(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3, HUINT32 ulParam4, HUINT32 ulParam5, HUINT32 ulParam6,HUINT32 ulParam7);
STATIC HERROR	xsvc_logcapture_HandleMsg(XsvcLogCapture_Message_t* pstMsg);
STATIC void		xsvc_logcapture_CreateTask(void);
STATIC void		xsvc_logcapture_Task(void);
STATIC void		xsvc_logcapture_GetVideoError(HUINT32 ulDecoderError, HUINT32 ulDisplayError);
STATIC void		xsvc_logcapture_GeAudioError(HUINT32 ulPtsCountError, HUINT32 ulFrameError);

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define _____LOCAL_APIs_______________________________________________________________
STATIC XsvcLogCapture_Context_t*	xsvc_logcapture_GetContext(void)
{
	return &s_stLogCaptureContext;
}

STATIC HERROR			xsvc_logcapture_InitContext(void)
{
	HxSTD_MemSet(&s_stLogCaptureContext, 0x00, sizeof(XsvcLogCapture_Context_t));
	s_stLogCaptureContext.nChannelState = LC_CHANNEL_STATE_NO_SIGNAL;
	return ERR_OK;
}

#define _____SEND_TASK_MSG_____
STATIC HERROR xsvc_logcapture_SendMsgLogAction(XsvcLogCapture_Action_e eType, HUINT32 param1, HUINT32 param2, HUINT32 param3, HUINT32 param4, HUINT32 param5)
{
	HINT32			 nRet;
	XsvcLogCapture_Message_t		stMsg;
	unsigned long	ulUnixTime = 0;


    VK_CLOCK_GetTime(&ulUnixTime) ;
	HxSTD_MemSet(&stMsg, 0x00, sizeof(XsvcLogCapture_Message_t));

	stMsg.ulCmd = eCMD_LC_NOTIFY_LOG_ACTION;
	stMsg.hAction = 	0xffffffff;
	stMsg.ulParam1 = 	(HUINT32)eType;
	stMsg.ulParam2 = 	ulUnixTime;
	stMsg.ulParam3 = 	(HUINT32)param1;
	stMsg.ulParam4 = 	(HUINT32)param2;
	stMsg.ulParam5 = 	(HUINT32)param3;
	stMsg.ulParam6 = 	(HUINT32)param4;
	stMsg.ulParam7 = 	(HUINT32)param5;


	nRet = VK_MSG_SendTimeout (s_ulLogCaptureMsgQId, &stMsg, sizeof(XsvcLogCapture_Message_t), 0);
	if (VK_OK != nRet)
	{
		HxLOG_Assert(0);
	}

	return ERR_OK;
}

#define _____FILE_MANAGEMENT_____


#define _____MONITORING_TIMER_____

STATIC void xsvc_logcapture_TimerCallbackForMonitoring(HULONG ulTimerId, void *pLocal )
{

	XsvcLogCapture_Message_t stMsg, *pstLocal = NULL;
	HINT32 nRet = 0;

	HxSTD_MemSet(&stMsg, 0x00, sizeof(stMsg));

	pstLocal = (XsvcLogCapture_Message_t *) pLocal;
	if( pstLocal != NULL )
	{
		stMsg.hAction = pstLocal->hAction;
	}

	stMsg.ulCmd = eCMD_LC_TIMER_MONITORING;
	stMsg.ulParam1 = ulTimerId;

	nRet = VK_MSG_SendTimeout (s_ulLogCaptureMsgQId, &stMsg, sizeof(XsvcLogCapture_Message_t), 0);
	if (VK_OK != nRet)
	{
		HxLOG_Assert(0);
	}
}

STATIC void xsvc_logcapture_TimerCallbackForWaiting(HULONG ulTimerId, void *pLocal )
{

	XsvcLogCapture_Message_t stMsg, *pstLocal = NULL;
	HINT32 nRet = 0;

	HxSTD_MemSet(&stMsg, 0x00, sizeof(stMsg));

	pstLocal = (XsvcLogCapture_Message_t *) pLocal;
	if( pstLocal != NULL )
	{
		stMsg.hAction = pstLocal->hAction;
	}

	stMsg.ulCmd = eCMD_LC_TIMER_WAITING;
	stMsg.ulParam1 = ulTimerId;

	nRet = VK_MSG_SendTimeout (s_ulLogCaptureMsgQId, &stMsg, sizeof(XsvcLogCapture_Message_t), 0);
	if (VK_OK != nRet)
	{
		HxLOG_Assert(0);
	}
}

STATIC HERROR xsvc_logcapture_CheckCpuInfo(HUINT32 *pulCpuTemp)
{
	HERROR hErr = ERR_FAIL;
	HUINT32 ulCpuTemp = 0;
	XsvcLogCapture_Context_t* pContext = xsvc_logcapture_GetContext();

	hErr = PAL_SYS_GetCPUTemperature(&ulCpuTemp);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Error, Can't get Memory Info\r\n");
		return ERR_FAIL;
	}

	/* float celcius = temperature/1000 */
	ulCpuTemp = ulCpuTemp/1000;

	HxLOG_Debug("[CPU] Threshold(%d), Temperature(%d)\r\n",LOG_CAPTURE_THRESHOLD_CPU_TEMPERATURE,ulCpuTemp);

	if(ulCpuTemp >= LOG_CAPTURE_THRESHOLD_CPU_TEMPERATURE)
	{
		if(pContext->ulSavedCpuTemperature == ulCpuTemp)
		{
			// 같은 온도가 유지되면, Ignore
			*pulCpuTemp = (HUINT32)NULL;
			return ERR_FAIL;
		}

		// New 1min. Timer Start
		xsvc_logcapture_KillTimer(pContext->ulCPUInfoReTimerID);
		xsvc_logcapture_SetTimerForWaiting(HANDLE_NULL, LC_TIMER_ID_CPU_TEMPERATURE_CHECK, &pContext->ulCPUInfoReTimerID, (HULONG)LOG_CAPTURE_TIMER_CPU_TEMPERATURE_CHECK);

		pContext->ulSavedCpuTemperature = ulCpuTemp;
		*pulCpuTemp = ulCpuTemp;
		return ERR_OK;
	}

	pContext->ulSavedCpuTemperature = 0;
	*pulCpuTemp = (HUINT32)NULL;
	return ERR_FAIL;

}

STATIC HERROR xsvc_logcapture_ReCheckCpuInfo(void)
{
	HERROR hErr = ERR_FAIL;
	HUINT32 ulCpuTemp = 0;
	XsvcLogCapture_Context_t* pContext = xsvc_logcapture_GetContext();

	hErr = PAL_SYS_GetCPUTemperature(&ulCpuTemp);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Error, Can't get Memory Info\r\n");
		return ERR_FAIL;
	}

	/* float celcius = temperature/1000 */
	ulCpuTemp = ulCpuTemp/1000;

	HxLOG_Debug("[CPU Re-Check] Threshold(%d), Temperature(%d)\r\n",LOG_CAPTURE_THRESHOLD_CPU_TEMPERATURE_RECHECK,ulCpuTemp);

	if(ulCpuTemp >= LOG_CAPTURE_THRESHOLD_CPU_TEMPERATURE_RECHECK)
	{
		// 110도에 STB Reboot 되니, 108도에 다다르면, 우선 Log Write
		HLIB_CMD_SyncLog();

		return ERR_OK;
	}

	if(pContext->ulSavedCpuTemperature < ulCpuTemp)
	{
		// New 1min. Timer Start
		xsvc_logcapture_KillTimer(pContext->ulCPUInfoReTimerID);
		xsvc_logcapture_SetTimerForWaiting(HANDLE_NULL, LC_TIMER_ID_CPU_TEMPERATURE_CHECK, &pContext->ulCPUInfoReTimerID, (HULONG)LOG_CAPTURE_TIMER_CPU_TEMPERATURE_CHECK);
	}

	return ERR_FAIL;

}

STATIC HERROR xsvc_logcapture_CheckMemInfo(HULONG *pulRemainMem)
{
	HULONG	ulRemainMem = 0;
	VK_MEM_INFO_t stMemInfo;
	HERROR hErr = ERR_FAIL;

	HxSTD_MemSet(&stMemInfo, 0x00, sizeof(VK_MEM_INFO_t));
	hErr = VK_MEM_GetInfo(&stMemInfo);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Error, Can't get Memory Info\r\n");
		return ERR_FAIL;
	}

	/*
		단위 : KB
		ulCommitted_AS 는 실제로 malloc 된 총 사이즈 입니다.
		ulMemFree 는 (ulMemTotal - 실제 사용하는 메모리) 입니다.
		ulCommitted_AS 는 Total Memory Size 이상 늘어나지 않습니다.
	*/

	HxLOG_Debug("ulMemTotal(%d), ulMemFree(%d), ulCommitted_AS(%d)\r\n",
			stMemInfo.ulMemTotal, stMemInfo.ulMemFree, stMemInfo.ulCommitted_AS);

	ulRemainMem = (stMemInfo.ulMemFree -  stMemInfo.ulCommitted_AS);

	HxLOG_Debug("[MEMORY] Threshold(%d), ulRemainMem(%d)\r\n",
				LOG_CAPTURE_THRESHOLD_MEMORY_CONSUMPTION, ulRemainMem);

	if(LOG_CAPTURE_THRESHOLD_MEMORY_CONSUMPTION >= ulRemainMem)
	{
		*pulRemainMem = (HULONG)ulRemainMem;
		return ERR_OK;
	}

	HxLOG_Debug("[MEM]Go ahead, For next period....!\r\n");

	*pulRemainMem = (HULONG)NULL;
	return ERR_FAIL;


}
STATIC HERROR xsvc_logcapture_CheckCMTempInfo(HUINT32* pulCurrentTemp)
{
	HERROR hErr = ERR_FAIL;
	HINT32 nSavedTemp = 0, nCurrentTemp = 0;
	HFLOAT32 fTemp = 0;

	HxLOG_Debug("xsvc_logcapture_CheckMemInfo()\r\n");
	hErr = PAL_CM_GetCMTemporature(&nSavedTemp, &nCurrentTemp);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Error, Can't get CM Temporature!\r\n");
		return ERR_FAIL;
	}

	fTemp = (HFLOAT32)(nCurrentTemp/10);
	if(fTemp >= LOG_CAPTURE_THRESHOLD_CM_TEMPERATURE)
	{
		*pulCurrentTemp = (HUINT32)fTemp;
		return ERR_OK;
	}

	HxLOG_Debug("[Cable Modem] Threshold(%d), nSavedTemp(%d), fTemp(%d)\r\n",
				LOG_CAPTURE_THRESHOLD_CM_TEMPERATURE, nSavedTemp, (HUINT32)fTemp);

	HxLOG_Debug("[CM]Go ahead, For next period....!\r\n");

	*pulCurrentTemp = (HUINT32)NULL;
	return ERR_FAIL;
}


STATIC HERROR xsvc_logcapture_CheckInBandReceiveSensitivityInfo(HUINT32* pulBer, HUINT32* pulSnr, HUINT32* pulUncorrectedNo, HUINT32* pulSignalPower)
{
	HERROR hErr = ERR_FAIL;
	HFLOAT32	fBer = 0;
	HFLOAT32	fSnr = 0;
	HFLOAT32	fSignalInputPower = 0;
	HUINT32		ulUnCorrectedNo = 0;
	HBOOL		bLogging = FALSE;

	XsvcLogCapture_Context_t* pContext = xsvc_logcapture_GetContext();
	if(pContext->nChannelState == LC_CHANNEL_STATE_NO_SIGNAL)
	{
		//HxLOG_Error("Can't check channel info, No Signal!\r\n");
		return ERR_FAIL;
	}

	hErr = PAL_CH_CAB_GetSignalInfoForLogCapture(pContext->ulCurrentTunerGroupId, &fBer, &fSnr, &ulUnCorrectedNo, &fSignalInputPower);
	if(hErr != ERR_OK)
	{
		//HxLOG_Error("Error, Can't get signal info\r\n");
		return ERR_FAIL;
	}

	HxLOG_Debug("[InBand] TunerGroupId(%d),nConstellation(%d),fBer(%.4f)\r\n",
				pContext->ulCurrentTunerGroupId,pContext->nConstellation,fBer);
	HxLOG_Debug("[InBand] fSnr(%.2f),ulUnCorrectedNo(%d),fSignalInputPower(%d)\r\n",
				fSnr,ulUnCorrectedNo,(HUINT32)fSignalInputPower);


	//SNR Check
	if(pContext->nConstellation == eDxCAB_CONSTELLATION_64QAM)
	{
		if(fSnr < LOG_CAPTURE_THRESHOLD_RS_SNR_64QAM)
		{
			// Logging
			bLogging = TRUE;
		}
	}
	else if(pContext->nConstellation == eDxCAB_CONSTELLATION_256QAM)
	{
		if(fSnr < LOG_CAPTURE_THRESHOLD_RS_SNR_256QAM)
		{
			// Logging
			bLogging = TRUE;
		}
	}
	else
	{
		//HxLOG_Error("Check, Constellation(%d)\r\n",pContext->nConstellation);
	}

	HxLOG_Debug("Current BER(%.4f), Condition BER (%.4f)\r\n",fBer,LOG_CAPTURE_THRESHOLD_RS_BER);
	// BER Check
	if(fBer > LOG_CAPTURE_THRESHOLD_RS_BER)
	{
		// Logging
		bLogging = TRUE;
	}



	if(pContext->nInBandPacketError != (HINT32)ulUnCorrectedNo)
	{
		// 최초 한번 기록
		if((pContext->nInBandPacketError == 0)&&(((HINT32)ulUnCorrectedNo - pContext->nInBandPacketError) > 10))
		{
			bLogging = TRUE;
			pContext->bLoggingForInBand = TRUE;
		}
		else if((pContext->nInBandPacketError < 20)&&((HINT32)ulUnCorrectedNo - pContext->nInBandPacketError) > 100)
		{
			bLogging = TRUE;
			pContext->bLoggingForInBand = TRUE;
		}
		else if(((HINT32)ulUnCorrectedNo == 0) && (pContext->bLoggingForDnStream == TRUE))
		{
			// 정상으로 회복 한 시점에 기록
			bLogging = TRUE;
			pContext->bLoggingForInBand = FALSE;
		}
	}

	if(bLogging == TRUE)
	{
		if(((HINT32)fSnr == pContext->nInBandSnr) && ((HINT32)ulUnCorrectedNo == pContext->nInBandPacketError) && (((HINT32)fBer == pContext->nInBandBer)))
		{
			if((HINT32)ulUnCorrectedNo > LOG_CAPTURE_PACKETERROR_CHECK_THERSHOLD)
			{
				//계속 증가하므로, Threshold 이상의 값은 기록하지 않기로 함
				//HxLOG_Debug("[PacketError] Old(%d), New(%d)\r\n",pContext->nCMPacketError,ulUnCorrectedNo);
				return ERR_FAIL;
			}

			// 동일한 조건은 기록하지 않는다
			return ERR_FAIL;
		}

		pContext->nInBandPacketError = (HINT32)fSnr;
		pContext->nInBandBer = (HINT32)fBer;
		pContext->nInBandPacketError = (HINT32)ulUnCorrectedNo;

		*pulBer = (HUINT32)fBer;
		*pulSnr = (HUINT32)fSnr;
		*pulUncorrectedNo = ulUnCorrectedNo;
		*pulSignalPower = (HUINT32)fSignalInputPower;
		return ERR_OK;
	}

	return ERR_FAIL;
}

STATIC HERROR xsvc_logcapture_CheckCMUpStreamReceiveSensitivityInfo(HUINT32 *pulFreq, HUINT32 *pulSignalPower, HUINT32* pulSymbolrate, HUINT32* pulMod)
{
	HERROR hErr = ERR_FAIL;
	HINT32		nFreq = 0;
	HINT32		nSignalPower = 0;
	HINT32		nSymbolRate = 0;
	INET_CM_UpMod_t eUpMod = 0;
	HBOOL		bLogging = FALSE;

	XsvcLogCapture_Context_t* pContext = xsvc_logcapture_GetContext();

	// TODO: 비교 조건이 뭔지 확인 해야함

	hErr = PAL_CM_GetUpStreamFreq30(pContext->ulCurrentTunerGroupId, &nFreq);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Error, Can't get Up Stream Frequency info\r\n");
		nFreq = 0;
		//return ERR_FAIL;
	}

	hErr = PAL_CM_GetUpStreamSignalLevel30(pContext->ulCurrentTunerGroupId, &nSignalPower);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Error, Can't get Up Stream SignalPower info\r\n");
		nSignalPower = 0;
		//return ERR_FAIL;
	}

	hErr = PAL_CM_GetUpStreamSymbolRate30(pContext->ulCurrentTunerGroupId, &nSymbolRate);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Error, Can't get Up Stream SymbolRate info\r\n");
		nSymbolRate = 0;
		//return ERR_FAIL;
	}
#if 0
	hErr = PAL_CM_GetUpStreamMod30(pContext->ulCurrentTunerGroupId, &eUpMod);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Error, Can't get Up Stream Mod info \r\n");
		return ERR_FAIL;
	}
#endif

	HxLOG_Debug("[UpStream Info]\r\n");
	HxLOG_Debug("# nFreq(%d), nSignalPower(%d), eUpMod(%d), nSymbolRate(%d)\r\n",
				nFreq,nSignalPower,eUpMod,nSymbolRate);

	if(bLogging == TRUE)
	{
		*pulFreq = (HUINT32)nFreq;
		*pulSignalPower = (HUINT32)nSignalPower;
		*pulSymbolrate = (HUINT32)nSymbolRate;
		*pulMod = (HUINT32)eUpMod;
		return ERR_OK;
	}

	return ERR_FAIL;
}

STATIC HERROR xsvc_logcapture_CheckCMDownStreamReceiveSensitivityInfo(HUINT32 *pulFreq, HUINT32* pulSnr, HUINT32 *pulSignalPower, HUINT32* pulMod, HUINT32* pulUnCorrectedNo)
{
	HERROR hErr = ERR_FAIL;
	HINT32		nFreq = 0;
	HINT32		nSignalPower = 0;
	INET_CM_DownMod_t eDownMod = 0;
	HINT32		nSnr = 0;
	HINT32		nUnCorrectedNo = 0;
	HBOOL		bLogging = FALSE;
	INET_CM_Status_t eStatus;

	XsvcLogCapture_Context_t* pContext = xsvc_logcapture_GetContext();
	hErr = PAL_CM_GetStatus(&eStatus);
	if ( ERR_OK != hErr )
	{
		//HxLOG_Error("\nPAL_CM_GetStatus() hErr(%d). !!!! \n", hErr);
		return ERR_FAIL;
	}
	if(eStatus != eCM_OPERATION)
	{
		//HxLOG_Debug("Check Cable Modem, don't working - eStatus(0x%x)\r\n",eStatus);
		return ERR_FAIL;
	}

	hErr = PAL_CM_GetDownStreamMod30Type(pContext->ulCurrentTunerGroupId, &eDownMod);
	if(hErr != ERR_OK)
	{
		//HxLOG_Error("\nPAL_CM_GetDownStreamMod30Type() hErr(%d). !!!! \n", hErr);
		return ERR_FAIL;
	}

	hErr = PAL_CM_GetDownStreamSNR30(pContext->ulCurrentTunerGroupId, &nSnr);
	if(hErr != ERR_OK)
	{
		//HxLOG_Error("\nPAL_CM_GetDownStreamSNR30() hErr(%d). !!!! \n", hErr);
		return ERR_FAIL;
	}
	nSnr = (nSnr*(0.1));

	//SNR Check
	if(eDownMod == eCM_DOWN_MOD_QAM64)
	{
		if(nSnr < LOG_CAPTURE_THRESHOLD_RS_SNR_64QAM)
		{
			// Logging
			bLogging = TRUE;
		}
	}
	else if(eDownMod == eCM_DOWN_MOD_QAM256)
	{
		if(nSnr < LOG_CAPTURE_THRESHOLD_RS_SNR_256QAM)
		{
			// Logging
			bLogging = TRUE;
		}
	}
	else
	{
		//HxLOG_Error("Check, eDownMod(%d)\r\n",eDownMod);
		return ERR_FAIL;
	}

	/*
		-Online 조건
		-PacketError가 10개, 100개 발생 시점에 로그 기록
		-PacketError가 정상으로 돌아온 시점에 로그 기록
		-채널 변경하지 않은 상황(PacketError가 reset 되지 않은 상황)에서 PacketError가 늘어나면 계속 기록
		-PacketError는 채널 전환으로 Reset 된다
		-PacketError가 균일하게 들어오지 않으니, 아래의 조건처럼 구현 함.
	*/
	hErr = PAL_CM_GetDownStreamUnCorrected30(pContext->ulCurrentTunerGroupId, &nUnCorrectedNo);
	if(hErr != ERR_OK)
	{
		//HxLOG_Error("Error, Can't get Down Stream UnCorrected info\r\n");
		return ERR_FAIL;
	}
	if(pContext->nCMPacketError != nUnCorrectedNo)
	{
		// 최초 한번 기록
		if((pContext->nCMPacketError == 0)&&((nUnCorrectedNo - pContext->nCMPacketError) > 10))
		{
			bLogging = TRUE;
			pContext->bLoggingForDnStream = TRUE;
		}
		else if((pContext->nCMPacketError < 20)&&(nUnCorrectedNo - pContext->nCMPacketError) > 100)
		{
			bLogging = TRUE;
			pContext->bLoggingForDnStream = TRUE;
		}
		else if((nUnCorrectedNo == 0) && (pContext->bLoggingForDnStream == TRUE))
		{
			// 정상으로 회복 한 시점에 기록
			bLogging = TRUE;
			pContext->bLoggingForDnStream = FALSE;
		}
	}

	hErr = PAL_CM_GetDownStreamFreq30(pContext->ulCurrentTunerGroupId, &nFreq);
	if(hErr != ERR_OK)
	{
		//HxLOG_Error("Error, Can't get Down Stream Frequency info\r\n");
		return ERR_FAIL;
	}

	hErr = PAL_CM_GetDownStreamSignalLevel30(pContext->ulCurrentTunerGroupId, &nSignalPower);
	if(hErr != ERR_OK)
	{
		//HxLOG_Error("Error, Can't get Down Stream Signal Power info\r\n");
		return ERR_FAIL;
	}
	nSignalPower = (nSignalPower*(0.1));


	HxLOG_Debug("[DownStream Info]\r\n");
	HxLOG_Debug("# bLogging(%d), nFreq(%d), nsnr(%d), nSignalPower(%d), eDownMod(%d), ulUnCorrectedNo(%d)\r\n",
				bLogging, nFreq,nSnr,nSignalPower,eDownMod,nUnCorrectedNo);


	if(bLogging == TRUE)
	{
		if((nSnr == pContext->nCMSnr) && (nUnCorrectedNo == pContext->nCMPacketError)) // BER ?
		{
			if(nUnCorrectedNo > LOG_CAPTURE_PACKETERROR_CHECK_THERSHOLD)
			{
				//계속 증가하므로, Threshold 이상의 값은 기록하지 않기로 함
				//HxLOG_Debug("[PacketError] Old(%d), New(%d)\r\n",pContext->nCMPacketError,nUnCorrectedNo);
				return ERR_FAIL;
			}

			// 동일한 조건은 기록하지 않는다
			return ERR_FAIL;
		}

		pContext->nCMSnr = nSnr;
		pContext->nCMPacketError = nUnCorrectedNo;

		*pulFreq = (HUINT32)nFreq;
		*pulSnr = (HUINT32)nSnr;
		*pulSignalPower = (HUINT32)nSignalPower;
		*pulMod = (HUINT32)eDownMod;
		*pulUnCorrectedNo = (HUINT32)nUnCorrectedNo;
		return ERR_OK;
	}

	return ERR_FAIL;
}

STATIC void xsvc_logcapture_SendAudioErrorMsg(void)
{
	XsvcLogCapture_Context_t *pContext = xsvc_logcapture_GetContext();
	HINT32 i = 0;

	if(pContext->nAErrorPackIndex > 0)
	{
		HxSTD_memcpy(pContext->aucAErrorPack, pContext->aucAErrorPackBuffer, sizeof(pContext->aucAErrorPackBuffer));

		for(i = 0; i < pContext->nAErrorPackIndex; i++)
		{
			HLIB_CMD_CaptureLog("%s",pContext->aucAErrorPack[i]);

			HLIB_STD_TaskSleep(200);
		}
	}
	else
	{
		HLIB_CMD_CaptureLog("%s",pContext->aucVErrorPack[0]);
	}

}

STATIC void xsvc_logcapture_SendVideoErrorMsg(void)
{
	XsvcLogCapture_Context_t *pContext = xsvc_logcapture_GetContext();
	HINT32 i = 0;

	if(pContext->nVErrorPackIndex > 0)
	{
		HxSTD_memcpy(pContext->aucVErrorPack, pContext->aucVErrorPackBuffer, sizeof(pContext->aucVErrorPackBuffer));

		for(i = 0; i < pContext->nVErrorPackIndex; i++)
		{
			HLIB_CMD_CaptureLog("%s",pContext->aucVErrorPack[i]);

			HLIB_STD_TaskSleep(200);
		}
	}
	else
	{
		HLIB_CMD_CaptureLog("%s",pContext->aucVErrorPack[0]);
	}

}

STATIC HERROR	xsvc_logcapture_KillTimer(HULONG ulTimerId)
{
	VK_TIMER_Cancel( ulTimerId );

	return ERR_OK;
}

STATIC HERROR xsvc_logcapture_SetTimer( Handle_t hAct, HUINT32 ulType, HULONG *pulTimerId, HULONG ulMilliSecond )
{
	XsvcLogCapture_Message_t stMsg;
	HUINT32 ulRet=0;
	HINT32	nTimerId = (HINT32)pulTimerId;

	if (nTimerId < eLC_MONITORING_TIMER_ID_MAX) {
		xsvc_logcapture_KillTimer(nTimerId);
	}

	HxSTD_MemSet( &stMsg, 0, sizeof(XsvcLogCapture_Message_t) );
	stMsg.hAction = hAct;
	stMsg.ulParam1 = ulType;

	ulRet = VK_TIMER_EventEvery( ulMilliSecond, xsvc_logcapture_TimerCallbackForMonitoring, (void*)&stMsg, sizeof(XsvcLogCapture_Message_t), pulTimerId );

	return ulRet;
}

STATIC HERROR xsvc_logcapture_SetTimerForWaiting( Handle_t hAct, HUINT32 ulType, HULONG *pulTimerId, HULONG ulMilliSecond )
{
	XsvcLogCapture_Message_t stMsg;
	HUINT32 ulRet=0;

	HxSTD_MemSet( &stMsg, 0, sizeof(XsvcLogCapture_Message_t) );
	stMsg.hAction = hAct;
	stMsg.ulParam1 = ulType;

	ulRet = VK_TIMER_EventAfter( ulMilliSecond, xsvc_logcapture_TimerCallbackForWaiting, NULL, 0, pulTimerId );

	return ERR_OK;
}

STATIC HERROR xsvc_logcapture_TimerReceived(HUINT32 ulType)
{
	HERROR			nResult = ERR_FAIL;
	HUINT32			ulVal = 0, ulVal1 = 0, ulVal2 = 0, ulVal3 = 0, ulVal4 = 0;
	HUINT32			ulLogType = eLC_ACTION_NONE;
	HULONG			ulRemainMem = 0;
	XsvcLogCapture_Context_t* pContext = xsvc_logcapture_GetContext();

	HxLOG_Debug("[MONITORING Start]################################\r\n");


	if(ulType == pContext->ulCPUInfoTimerID)
	{
		// Get CPU Temperature & Compare
		nResult = xsvc_logcapture_CheckCpuInfo(&ulVal);
		if(nResult != ERR_OK)
		{
			return nResult;
		}
		ulLogType = eLC_ACTION_CPU_TEMPERATURE;
	}
	else if(ulType == pContext->ulMemInfoTimerID)
	{
		// Get Memory Consumption & Compare
		nResult = xsvc_logcapture_CheckMemInfo(&ulRemainMem);
		if(nResult != ERR_OK)
		{
			return nResult;
		}
		ulLogType = eLC_ACTION_MEMORY_CONSUMPTION;
		ulVal = (HUINT32)ulRemainMem;
	}
	else if(ulType == pContext->ulCMInfoTimerID)
	{
		// Get CM Temperature & Compare
		nResult = xsvc_logcapture_CheckCMTempInfo(&ulVal);
		if(nResult != ERR_OK)
		{
			return nResult;
		}
		ulLogType = eLC_ACTION_CM_TEMPERATURE;
	}
	else if(ulType == pContext->ulInBandRSInfoTimerID)
	{
		// Get Receive Sensitivity & Compare
		nResult = xsvc_logcapture_CheckInBandReceiveSensitivityInfo(&ulVal, &ulVal1, &ulVal2, &ulVal3);
		if(nResult != ERR_OK)
		{
			return nResult;
		}
		ulLogType = eLC_ACTION_INBAND_RS_STATUS;
	}
	else if(ulType == pContext->ulCMUpStreamRSInfoTimerID)
	{
		nResult = xsvc_logcapture_CheckCMUpStreamReceiveSensitivityInfo(&ulVal, &ulVal1, &ulVal2, &ulVal3);
		if(nResult != ERR_OK)
		{
			return nResult;
		}
		ulLogType = eLC_ACTION_CM_UPSTREAM_STATUS;
	}
	else if(ulType == pContext->ulCMDownStreamRSInfoTimerID)
	{
		nResult = xsvc_logcapture_CheckCMDownStreamReceiveSensitivityInfo(&ulVal, &ulVal1, &ulVal2, &ulVal3, &ulVal4);
		if(nResult != ERR_OK)
		{
			return nResult;
		}
		ulLogType = eLC_ACTION_CM_DOWNSTREAM_STATUS;
	}
	else
	{
		HxLOG_Debug("[%s] check timer id(%d)\r\n",__func__,ulType);
	}

	// 정상적으로 처리된 경우만, 진행
	if(ulLogType != eLC_ACTION_NONE)
	{
		xsvc_logcapture_SendMsgLogAction(ulLogType,ulVal,ulVal1,ulVal2,ulVal3,ulVal4);
	}

	HxLOG_Debug("[MONITORING End]################################\r\n");

	return ERR_OK;
}

STATIC HERROR xsvc_logcapture_RegisterTimer()
{
	HUINT32 i = 0;
	Handle_t hAct = 0xffffffff;
	XsvcLogCapture_Context_t* pContext = xsvc_logcapture_GetContext();

	for(i = 0; i < eLC_MONITORING_TIMER_ID_MAX; i++)
	{
		switch(i)
		{
			case eLC_MONITORING_TIMER_ID_CPU:
				xsvc_logcapture_SetTimer(hAct, i, &pContext->ulCPUInfoTimerID, (HULONG)LOG_CAPTURE_MONITORING_TIMER_CPU);
				break;
			case eLC_MONITORING_TIMER_ID_MEMORY:
				xsvc_logcapture_SetTimer(hAct, i, &pContext->ulMemInfoTimerID, (HULONG)LOG_CAPTURE_MONITORING_TIMER_MEMORY);
				break;
			case eLC_MONITORING_TIMER_ID_CM:
				xsvc_logcapture_SetTimer(hAct, i, &pContext->ulCMInfoTimerID, (HULONG)LOG_CAPTURE_MONITORING_TIMER_CM);
				break;
			case eLC_MONITORING_TIMER_ID_INBAND_RS:
				xsvc_logcapture_SetTimer(hAct, i, &pContext->ulInBandRSInfoTimerID, (HULONG)LOG_CAPTURE_MONITORING_TIMER_INBAND_RS);
				break;
#if defined(NOT_USE_LOGCAPTURE)
			case eLC_MONITORING_TIMER_ID_CM_UPSTREAM_RS:
				xsvc_logcapture_SetTimer(hAct, i, &pContext->ulCMUpStreamRSInfoTimerID, (HULONG)LOG_CAPTURE_MONITORING_TIMER_CM_UPSTREAM_RS);
				break;
#endif
			case eLC_MONITORING_TIMER_ID_CM_DOWNSTREAM_RS:
				xsvc_logcapture_SetTimer(hAct, i, &pContext->ulCMDownStreamRSInfoTimerID, (HULONG)LOG_CAPTURE_MONITORING_TIMER_CM_DOWNSTREAM_RS);
				break;
			default:
				HxLOG_Error("Check timer id(%d)\r\n",i);
				break;

		}

	}
	return ERR_OK;
}

STATIC void xsvc_logcapture_TimerReceivedForWaiting(HUINT32 ulType)
{
	XsvcLogCapture_Context_t* pContext = xsvc_logcapture_GetContext();

	if(ulType == pContext->ulAudioErrorTimerID)
	{
		if(pContext->nAErrorAllOldCount == pContext->nAErrorAllCount)
		{
			VK_SEM_Get(s_ulLogCapture2SemaId);

			xsvc_logcapture_SendAudioErrorMsg();

			HxSTD_MemSet(pContext->aucAErrorPack, 0x00, sizeof(pContext->aucAErrorPack));
			HxSTD_MemSet(pContext->aucAErrorPackBuffer, 0x00, sizeof(pContext->aucAErrorPackBuffer));

			VK_SEM_Release(s_ulLogCapture2SemaId);

			pContext->nAErrorAllOldCount = 0;
			pContext->nAErrorAllCount = 0;
			pContext->nAErrorPackIndex = 0;
			pContext->nAPtsCountErr= 0;
			pContext->nAFrameErr= 0;
			pContext->ulAudioErrorTimerID = 0;



		}
		else
		{
			//Start Timer
			xsvc_logcapture_SetTimerForWaiting(HANDLE_NULL, LC_TIMER_ID_AUDIOERROR_CHECK, &pContext->ulAudioErrorTimerID, (HULONG)LOG_CAPTURE_TIMER_AUDIOERROR_CHECK);

			pContext->nAErrorAllOldCount = pContext->nAErrorAllCount;
		}
	}
	else if(ulType == pContext->ulVideoErrorTimerID)
	{
		if(pContext->nVErrorAllOldCount == pContext->nVErrorAllCount)
		{
			VK_SEM_Get(s_ulLogCaptureSemaId);

			xsvc_logcapture_SendVideoErrorMsg();

			HxSTD_MemSet(pContext->aucVErrorPack, 0x00, sizeof(pContext->aucVErrorPack));
			HxSTD_MemSet(pContext->aucVErrorPackBuffer, 0x00, sizeof(pContext->aucVErrorPackBuffer));

			VK_SEM_Release(s_ulLogCaptureSemaId);

			pContext->nVErrorAllOldCount = 0;
			pContext->nVErrorAllCount = 0;
			pContext->nVErrorPackIndex = 0;
			pContext->nVDecoderErr = 0;
			pContext->nVDiplayErr = 0;
			pContext->ulVideoErrorTimerID = 0;
		}
		else
		{
			//Start Timer
			xsvc_logcapture_SetTimerForWaiting(HANDLE_NULL, LC_TIMER_ID_VIDEOERROR_CHECK, &pContext->ulVideoErrorTimerID, (HULONG)LOG_CAPTURE_TIMER_VIDEOERROR_CHECK);

			pContext->nVErrorAllOldCount = pContext->nVErrorAllCount;
		}
	}
	else if(ulType == pContext->ulCPUInfoReTimerID)
	{
		HxLOG_Debug("Do Re Check. CPU Temp. per 1min.\r\n");
		xsvc_logcapture_ReCheckCpuInfo();
	}
	else
	{
		HxLOG_Error("[%s]Check timer id(%d)\r\n",__func__,ulType);
	}

}


#define _____FROM_CALLBACK_FUCNTION_____
#if 0
STATIC HERROR xsvc_logcapture_SetCurrentTunerId(HUINT32 ulParam1)
{
	HUINT32				actId;
	Handle_t 			hAction;

	hAction = (Handle_t)ulParam1;

	actId = PAL_PIPE_GetActionId(hAction);
	if ( actId >= NUM_MAX_ACTION )
	{
		HxLOG_Error("Fail, Get Action Id for TunerID, actId(%d)\r\n",actId);
		return ERR_FAIL;
	}
	return ERR_OK;
}
#endif

STATIC HERROR xsvc_logcapture_ClassifyLogAction(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3, HUINT32 ulParam4, HUINT32 ulParam5, HUINT32 ulParam6, HUINT32 ulParam7)
{
	HUINT32		ulRet = ERR_OK;
	XsvcLogCapture_Context_t *pContext = xsvc_logcapture_GetContext();

	HxLOG_Debug("[%s] ulParam1(0x%x), ulParam2(%d), ulParam3(%d), ulParam4(0x%x), ulParam5(%d), ulParam6(%d), ulParam7(%d)\r\n",__func__,
					ulParam1,ulParam2,ulParam3,ulParam4,ulParam5,ulParam6,ulParam7);

	switch(ulParam1)
	{
		case eLC_ACTION_CPU_TEMPERATURE:
			HLIB_CMD_CaptureLog("Cpu Temperature=%d",ulParam3);
			break;
		case eLC_ACTION_CM_TEMPERATURE:
			HLIB_CMD_CaptureLog("CM Temperature=%d",ulParam3);
			break;
		case eLC_ACTION_INBAND_RS_STATUS:
			HLIB_CMD_CaptureLog("InBand,Mod=%s(%d),BER=%f,SNR=%f,SignalPower=%d,PacketError=%d",
												(pContext->nConstellation==eDxCAB_CONSTELLATION_64QAM)?"64QAM":((pContext->nConstellation==eDxCAB_CONSTELLATION_256QAM)?"256QAM":"Unknown"),
												pContext->nConstellation,(HFLOAT32)ulParam3,(HFLOAT32)ulParam4,ulParam5,ulParam6);
			break;
		case eLC_ACTION_CM_DOWNSTREAM_STATUS:
			HLIB_CMD_CaptureLog("CM DownStream,Frequency=%d,SNR=%d,SignalPower=%d,PacketError=%d,Mod=%s",
											ulParam3,ulParam4,ulParam5,ulParam6,(ulParam7==eDxCAB_CONSTELLATION_64QAM)?"64QAM":"256QAM");
			break;
		case eLC_ACTION_CM_UPSTREAM_STATUS:
			HLIB_CMD_CaptureLog("CM UpStream,Frequency=%d,SignalPower=%d,Symbolrat=%d,Mod=%d",
											ulParam3,ulParam4,ulParam5,ulParam6);
			break;
		case eLC_ACTION_MEMORY_CONSUMPTION:
			HLIB_CMD_CaptureLog("Memory Consumption,RemainMemory=%dKb",ulParam3);
			break;

		default:
			HxLOG_Error("Check param(%d)\r\n",ulParam1);
			break;
	}

	return ulRet;
}

STATIC HERROR xsvc_logcapture_HandleMsg(XsvcLogCapture_Message_t* pstMsg)
{

	switch(pstMsg->ulCmd)
	{
		case eCMD_LC_NOTIFY_LOG_ACTION:
			xsvc_logcapture_ClassifyLogAction(pstMsg->ulParam1,pstMsg->ulParam2,pstMsg->ulParam3,pstMsg->ulParam4,pstMsg->ulParam5,pstMsg->ulParam6,pstMsg->ulParam7);
			break;

		case eCMD_LC_TIMER_MONITORING:
			xsvc_logcapture_TimerReceived(pstMsg->ulParam1);
			break;

		case eCMD_LC_TIMER_WAITING:
			xsvc_logcapture_TimerReceivedForWaiting(pstMsg->ulParam1);
			break;

		default:
			HxLOG_Error("[%s] Check command(%d)\r\n",__func__,pstMsg->ulCmd);
			HxLOG_Assert(0);
			break;
	}


	return ERR_OK;
}
#define _____DEBUG_PRINT_____

STATIC void xsvc_logcapture_GetVideoError(HUINT32 ulDecoderError, HUINT32 ulDisplayError)
{
	XsvcLogCapture_Context_t *pContext = xsvc_logcapture_GetContext();
	unsigned long	ulUnixTime = 0;

	VK_SEM_Get(s_ulLogCaptureSemaId);

	// 3초에 한번 체크해서, Error Count가 동일하면, HLIB로 send, VideoErrorPack, Count 초기화
	if(pContext->nVErrorAllCount== 0)
	{
		// Get Time Stamp (Don't error check)
	    VK_CLOCK_GetTime(&(pContext->ulVideoErrorTimeStamp));

		//Start Timer
		xsvc_logcapture_SetTimerForWaiting(HANDLE_NULL, LC_TIMER_ID_VIDEOERROR_CHECK, &pContext->ulVideoErrorTimerID, (HULONG)LOG_CAPTURE_TIMER_VIDEOERROR_CHECK);
	}

	// Get Time Stamp (Don't error check)
	VK_CLOCK_GetTime(&ulUnixTime);

	pContext->nVDiplayErr += ulDisplayError;
	pContext->nVDecoderErr += ulDecoderError;

	if(pContext->nVErrorPackIndex < VERR_INDEX)
	{
		HxSTD_snprintf(pContext->aucVErrorPackBuffer[pContext->nVErrorPackIndex], sizeof(pContext->aucVErrorPackBuffer[pContext->nVErrorPackIndex]),
						"Video Out,Time=%d,DecoderError=%d,ulDisplayError=%d", (HINT32)ulUnixTime,(HINT32)pContext->nVDecoderErr,(HINT32)pContext->nVDiplayErr);
	}

	if((ulUnixTime - pContext->ulVideoErrorTimeStamp) == 0 )
	{
		// do nothing... just stack..
	}
	else
	{
		pContext->ulVideoErrorTimeStamp = ulUnixTime;
		pContext->nVErrorPackIndex++;

		pContext->nVDiplayErr = 0;
		pContext->nVDecoderErr = 0;
	}

	pContext->nVErrorAllCount++;

	VK_SEM_Release(s_ulLogCaptureSemaId);

}

STATIC void xsvc_logcapture_GeAudioError(HUINT32 ulPtsCountError, HUINT32 ulFrameError)
{
	XsvcLogCapture_Context_t *pContext = xsvc_logcapture_GetContext();
	unsigned long	ulUnixTime = 0;

	VK_SEM_Get(s_ulLogCapture2SemaId);

	// 3초에 한번 체크해서, Error Count가 동일하면, HLIB로 send, AudioErrorPack, Count 초기화
	if(pContext->nAErrorAllCount== 0)
	{
		// Get Time Stamp (Don't error check)
	    VK_CLOCK_GetTime(&(pContext->ulAudioErrorTimeStamp));

		//Start Timer
		xsvc_logcapture_SetTimerForWaiting(HANDLE_NULL, LC_TIMER_ID_AUDIOERROR_CHECK, &pContext->ulAudioErrorTimerID, (HULONG)LOG_CAPTURE_TIMER_AUDIOERROR_CHECK);
	}

	// Get Time Stamp (Don't error check)
	VK_CLOCK_GetTime(&ulUnixTime);

	pContext->nAPtsCountErr += ulPtsCountError;
	pContext->nAFrameErr += ulFrameError;

	if(pContext->nAErrorPackIndex < AERR_INDEX)
	{
		HxSTD_snprintf(pContext->aucAErrorPackBuffer[pContext->nAErrorPackIndex], sizeof(pContext->aucAErrorPackBuffer[pContext->nAErrorPackIndex]),
						"Audio Out,Time=%d,PtsCountError=%d,ulFrameError=%d", (HINT32)ulUnixTime,(HINT32)pContext->nAPtsCountErr,(HINT32)pContext->nAFrameErr);
	}

	if((ulUnixTime - pContext->ulAudioErrorTimeStamp) == 0 )
	{
		// do nothing... just stack..
	}
	else
	{
		pContext->ulAudioErrorTimeStamp = ulUnixTime;
		pContext->nAErrorPackIndex++;

		pContext->nAPtsCountErr = 0;
		pContext->nAFrameErr = 0;
	}

	pContext->nAErrorAllCount++;


	VK_SEM_Release(s_ulLogCapture2SemaId);
}

STATIC void xsvc_logcapture_CreateTask(void)
{
	HUINT32		ulRet;

	ulRet = (HUINT32)VK_MSG_Create(LOG_CAPTURE_MSGQ_SIZE, (unsigned long)sizeof(XsvcLogCapture_Message_t), (char *) "LogCaptureMsgQ", &s_ulLogCaptureMsgQId, VK_SUSPENDTYPE_FIFO);
    if(ulRet != ERR_OK)
    {
		VK_Print("[xsvc_logcapture_CreateTask] ========================== VK_MSG_Create error \n");
        return ;
    }
	ulRet = (HUINT32)VK_TASK_Create((void *)xsvc_logcapture_Task, LOG_CAPTURE_TASK_PRIORITY, LOG_CAPTURE_TASK_STACK_SIZE, "tLogMsg", NULL, (HULONG *)&s_ulLogCaptureTaskQid, 0);
    if(ulRet != ERR_OK)
    {
		VK_Print("[xsvc_logcapture_CreateTask] ========================== VK_TASK_Create error \n");
        return ;
    }
	VK_SEM_Create(&s_ulLogCaptureSemaId, "LogCaptureSema", VK_SUSPENDTYPE_PRIORITY);
	VK_SEM_Create(&s_ulLogCapture2SemaId, "LogCapture2Sema", VK_SUSPENDTYPE_PRIORITY);

	VK_TASK_Start(s_ulLogCaptureTaskQid);


}

#define _____PUBLIC_APIs_______________________________________________________________

HERROR xsvc_logcapture_Init_Base(void)
{
	// Create Manager Task
	xsvc_logcapture_CreateTask();

	xsvc_logcapture_InitContext();

	// Set Monitoring Timer
	xsvc_logcapture_RegisterTimer();

	// Video Output Error Receive용 Callback 등록
	PAL_VIDEO_RegisterEventCallbackForVideoError(xsvc_logcapture_GetVideoError);

	// Audio Output Error Receive용 Callback 등록
	PAL_AUDIO_RegisterEventCallbackForAudioError(xsvc_logcapture_GeAudioError);

	return ERR_OK;
}

HERROR xsvc_logcapture_GetLogUSBInfo_Base(HUINT32 ulDeviceId)
{
#if 0
	HINT32 nLen = 0;
	HUINT8 aucUsbHddVendor[USBHDD_VENDOR_LEN];
	HUINT8 aucUsbHddProduct[USBHDD_PRODUCT_LEN];
	HUINT8 aucUsbHddSerialNum[USBHDD_SERIALNUM_LEN];

	HxSTD_MemSet(aucUsbHddVendor, 0, sizeof(aucUsbHddVendor));
	HxSTD_MemSet(aucUsbHddProduct, 0, sizeof(aucUsbHddProduct));
	HxSTD_MemSet(aucUsbHddSerialNum, 0, sizeof(aucUsbHddSerialNum));

	// Get USB Product Information
	PAL_FS_USBHDD_GetProductInfo(ulDeviceId,aucUsbHddVendor,aucUsbHddProduct,aucUsbHddSerialNum);

	HxLOG_Debug("[%s]ulDeviceId(%d), aucUsbHddVendor(%s/%d), aucUsbHddProduct(%s/%d), aucUsbHddSerialNum(%s/%d)\r\n",__func__,
						ulDeviceId,
						aucUsbHddVendor,HxSTD_StrLen(aucUsbHddVendor),
						aucUsbHddProduct,HxSTD_StrLen(aucUsbHddProduct),
						aucUsbHddSerialNum,HxSTD_StrLen(aucUsbHddSerialNum));

	nLen = HxSTD_StrLen(aucUsbHddVendor);
	aucUsbHddVendor[nLen+1] = '\0';
	nLen = HxSTD_StrLen(aucUsbHddProduct);
	aucUsbHddProduct[nLen+1] = '\0';
	nLen = HxSTD_StrLen(aucUsbHddSerialNum);
	aucUsbHddSerialNum[nLen+1] = '\0';

	HLIB_CMD_CaptureLog("USB Device Insertion,DeviceId=%d,Vendor=%s,Product=%s,SerialNum=%s",
				ulDeviceId,aucUsbHddVendor,aucUsbHddProduct,aucUsbHddSerialNum);
#else
	HLIB_CMD_CaptureLog("USB Device Insertion,DeviceId=%d",ulDeviceId);

#endif

	return ERR_OK;
}

HERROR xsvc_logcapture_SetTunerGroupId_Base(HUINT32 ulTunerGroupId, HINT32 nConstellation)
{
	XsvcLogCapture_Context_t*	pContext = xsvc_logcapture_GetContext();

	HxLOG_Debug("Current TunerGroupId(%d), nConstellation(%d)\r\n",ulTunerGroupId, nConstellation);

	pContext->ulCurrentTunerGroupId = ulTunerGroupId;
	pContext->nConstellation = nConstellation;

	pContext->nChannelState = 0;

	pContext->nChannelChangeAction = FALSE;

	return ERR_OK;
}

HERROR xsvc_logcapture_SetChannelState_Base(HINT32 nState)
{
	XsvcLogCapture_Context_t*	pContext = xsvc_logcapture_GetContext();

	pContext->nChannelState = nState;

	return ERR_OK;
}

HERROR xsvc_logcapture_SetChannelChangeAction_Base(void)
{
	XsvcLogCapture_Context_t*	pContext = xsvc_logcapture_GetContext();

	pContext->nChannelChangeAction = TRUE;

	return ERR_OK;
}

HERROR xsvc_logcapture_LogFileBackup_Base(void)
{
	HINT32 i = 0;
	HCHAR szMountPath[512];
	HCHAR szDefaultPath[16] = "/media/drive";
	HCHAR szPath[512];

	for(i = 1; i < 20; i ++)
	{
		szPath[0]='\0';
		szMountPath[0]='\0';

		HxSTD_snprintf(szPath, sizeof(szPath), "%s%d", szDefaultPath, i);
		if(HLIB_DIR_GetMountPath(szPath, szMountPath, 512) == ERR_OK)
		{
			HLIB_CMD_BackupLog(szMountPath);
			HLIB_CMD_Printf("USB Backup Success!, PATH:%s\r\n",szPath);
			break;
		}
		else
		{
			HLIB_CMD_Printf("USB Backup Fail!, PATH:%s\r\n",szPath);
		}
	}

	return ERR_OK;
}

#define _____TASK_______________________________________________________________

STATIC void xsvc_logcapture_Task(void)
{
	HUINT32 			nResult = 0;
	XsvcLogCapture_Message_t 		stMsg;

	while (1)
	{
		nResult = VK_MSG_Receive (s_ulLogCaptureMsgQId, &stMsg, sizeof(XsvcLogCapture_Message_t));
		if(nResult == ERR_OK && stMsg.ulCmd < CMD_LC_Max)
		{
			xsvc_logcapture_HandleMsg(&stMsg);
		}
		else
		{
			HxLOG_Info("GetMessage Error !!! : 0x%X\n", nResult);
			continue;
		}
	}

	return;
}

