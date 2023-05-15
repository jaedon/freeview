/*******************************************************************
	File Description
********************************************************************/
/**
	@file	pal_demux.c
	@brief	pal_demux.c

	Description: Demux의 Pid filter, Scramble bit check 담당.		\n
	Module: PAL /Demux 												\n

	Copyright (c) 2008 HUMAX Co., Ltd.								\n
	All rights reserved.											\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <di_demux.h>
#include <pal_demux.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

//#define FCC_DEBUG
#if defined(FCC_DEBUG)
#define HxLOG_FCC			MMMP
#else
#define HxLOG_FCC			HxLOG_Debug
#endif

#define palDmx_VALID_PID(x)				(x > 0 && x < PID_NULL)
#define palDmx_VALID_PCRPID(x)				(x > 0 && x < PID_STUFFING)
#define	palDmx_ENTER_CRITICAL			VK_SEM_Get(s_ulPalDmxSemId)
#define	palDmx_LEAVE_CRITICAL			VK_SEM_Release(s_ulPalDmxSemId)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	ePAL_DMX_TscPidType_Video = 0,
	ePAL_DMX_TscPidType_Audio,
	ePAL_DMX_TscPidType_Max
} PalDmx_TscPidType_e;

typedef struct
{
	PalDmx_PidInfo_t		stPid;
	PalDmx_FilterStatus_t	stStatus;
	HBOOL					bSupportTscMonitor;

#if !defined(CONFIG_MW_DISABLE_TSC_MONITOR)
	/* PID for checking scrambled bit */
	HUINT16					ausTscPid[ePAL_DMX_TscPidType_Max];
	HUINT32					aulTscHandle[ePAL_DMX_TscPidType_Max];
#endif
} PalDmx_Status_t;



/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC HULONG				 s_ulPalDmxSemId;
STATIC PalDmx_Status_t		*s_astDmxStatus;
STATIC HUINT32				 s_ulNumOfDmx;
STATIC HBOOL				 s_bMainAvDemuxId;


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#define ____LOCAL_FUNCTIONS____________________________________

STATIC void pal_dmx_ResetPidInfo(HUINT32 ulDemuxId)
{
	s_astDmxStatus[ulDemuxId].stPid.usPcrPid = PID_NULL;
	s_astDmxStatus[ulDemuxId].stPid.usVideoPid = PID_NULL;
	s_astDmxStatus[ulDemuxId].stPid.usMainAudioPid = PID_NULL;
	s_astDmxStatus[ulDemuxId].stPid.usSubAudioPid = PID_NULL;

	return;
}

STATIC void pal_dmx_ResetFilterStatus(HUINT32 ulDemuxId)
{
	s_astDmxStatus[ulDemuxId].stStatus.bMainPcrFilter = FALSE;
	s_astDmxStatus[ulDemuxId].stStatus.bSubPcrFilter = FALSE;

	s_astDmxStatus[ulDemuxId].stStatus.bMainVideoFilter = FALSE;
	s_astDmxStatus[ulDemuxId].stStatus.bSubVideoFilter = FALSE;

	s_astDmxStatus[ulDemuxId].stStatus.bMainAudioFilter = FALSE;
	s_astDmxStatus[ulDemuxId].stStatus.bSubAudioFilter = FALSE;

	return;
}

STATIC void pal_dmx_ResetStatus(HUINT32 ulDemuxId)
{
	pal_dmx_ResetPidInfo(ulDemuxId);
	pal_dmx_ResetFilterStatus(ulDemuxId);

	return;
}


#define ______TSC_MONITORING_________________________

STATIC HERROR pal_dmx_ResetTscData(HUINT32 ulDemuxId)
{
#if !defined(CONFIG_MW_DISABLE_TSC_MONITOR)
	HINT32	i;

	if(ulDemuxId >= s_ulNumOfDmx)
	{
		HxLOG_Error("invalid param - ulDemuxId(%d of %d) \n", ulDemuxId, s_ulNumOfDmx);
		return ERR_FAIL;
	}

	for(i = 0; i < ePAL_DMX_TscPidType_Max; i++)
	{
		s_astDmxStatus[ulDemuxId].ausTscPid[i] = PID_NULL;
		s_astDmxStatus[ulDemuxId].aulTscHandle[i] = HANDLE_NULL;
	}
#endif

	return ERR_OK;
}

STATIC HERROR pal_dmx_StartTscMonitor(HUINT32 ulDemuxId, PalDmx_TscPidType_e eTscPidType, HUINT16 usPid)
{
#if !defined(CONFIG_MW_DISABLE_TSC_MONITOR)
	DI_ERR_CODE eDiErr = DI_ERR_ERROR;

	if((ulDemuxId >= s_ulNumOfDmx) || (eTscPidType >= ePAL_DMX_TscPidType_Max))
	{
		HxLOG_Error("invalid param - ulDemuxId(%d of %d), eTscPidType (%d)\n", ulDemuxId, s_ulNumOfDmx, eTscPidType);
		return ERR_FAIL;
	}

	if(FALSE == s_astDmxStatus[ulDemuxId].bSupportTscMonitor)
	{
		return ERR_OK;
	}

	if(eTscPidType >= ePAL_DMX_TscPidType_Max)
	{
		HxLOG_Error("eTscPidType (%d/%d)\n", eTscPidType, ePAL_DMX_TscPidType_Max);
		return ERR_FAIL;
	}

	if (HANDLE_NULL != s_astDmxStatus[ulDemuxId].aulTscHandle[eTscPidType])
	{
		HxLOG_Error("aulTscHandle is not null \n");
		return ERR_FAIL;
	}

	if(PID_NULL != usPid)
	{
		eDiErr = DI_DEMUX_StartTSCMonitor(ulDemuxId, usPid, &s_astDmxStatus[ulDemuxId].aulTscHandle[eTscPidType]);
		if(DI_ERR_OK != eDiErr)
		{
			HxLOG_Error("Error in DI_DEMUX_StartTSCMonitor (s_astDmxStatus[%d].aulTscHandle[%d] = 0x%x)\n", ulDemuxId, eTscPidType, s_astDmxStatus[ulDemuxId].aulTscHandle[eTscPidType]);
			return ERR_FAIL;
		}

		HxLOG_Print(HxANSI_COLOR_YELLOW("[TSC Start] s_astDmxStatus[%d].aulTscHandle[%d] = 0x%x, ausTscPid = 0x%x\n"), ulDemuxId, eTscPidType
			, s_astDmxStatus[ulDemuxId].aulTscHandle[eTscPidType], usPid);
	}

	s_astDmxStatus[ulDemuxId].ausTscPid[eTscPidType] = usPid;
#endif

	return ERR_OK;
}

STATIC HERROR pal_dmx_StopTscMonitor(HUINT32 ulDemuxId, PalDmx_TscPidType_e eTscPidType)
{
#if !defined(CONFIG_MW_DISABLE_TSC_MONITOR)
	DI_ERR_CODE eDiErr = DI_ERR_ERROR;

	if((ulDemuxId >= s_ulNumOfDmx) || (eTscPidType >= ePAL_DMX_TscPidType_Max))
	{
		HxLOG_Error("invalid param - ulDemuxId(%d of %d), eTscPidType (%d)\n", ulDemuxId, s_ulNumOfDmx, eTscPidType);
		return ERR_FAIL;
	}

	if(FALSE == s_astDmxStatus[ulDemuxId].bSupportTscMonitor)
	{
		HxLOG_Warning("s_astDmxStatus[%d].bSupportTscMonitor = %d\n", ulDemuxId, s_astDmxStatus[ulDemuxId].bSupportTscMonitor);
		return ERR_OK;
	}

	if(HANDLE_NULL != s_astDmxStatus[ulDemuxId].aulTscHandle[eTscPidType])
	{
		HxLOG_Print(HxANSI_COLOR_YELLOW("[TSC Stop] s_astDmxStatus[%d].aulTscHandle[%d] = 0x%x, ausTscPid = 0x%x\n"), ulDemuxId, eTscPidType
			, s_astDmxStatus[ulDemuxId].aulTscHandle[eTscPidType], s_astDmxStatus[ulDemuxId].ausTscPid[eTscPidType]);

		eDiErr = DI_DEMUX_StopTSCMonitor(s_astDmxStatus[ulDemuxId].aulTscHandle[eTscPidType]);
		if(DI_ERR_OK != eDiErr)
		{
			HxLOG_Error("Error in DI_DEMUX_StopTSCMonitor (s_astDmxStatus[%d].aulTscHandle[%d] = 0x%x)\n", ulDemuxId, eTscPidType, s_astDmxStatus[ulDemuxId].aulTscHandle[eTscPidType]);
			return ERR_FAIL;
		}
		s_astDmxStatus[ulDemuxId].aulTscHandle[eTscPidType] = HANDLE_NULL;
		s_astDmxStatus[ulDemuxId].ausTscPid[eTscPidType] = PID_NULL;
	}
#endif

	return ERR_OK;
}

STATIC HERROR pal_dmx_StopAllTscMonitor(HUINT32 ulDemuxId)
{
#if !defined(CONFIG_MW_DISABLE_TSC_MONITOR)
	HINT32	i;

	if(ulDemuxId >= s_ulNumOfDmx)
	{
		HxLOG_Error("invalid param - ulDemuxId(%d of %d) \n", ulDemuxId, s_ulNumOfDmx);
		return ERR_FAIL;
	}

	if(FALSE == s_astDmxStatus[ulDemuxId].bSupportTscMonitor)
	{
		HxLOG_Warning("s_astDmxStatus[%d].bSupportTscMonitor = %d\n", ulDemuxId, s_astDmxStatus[ulDemuxId].bSupportTscMonitor);
		return ERR_OK;
	}

	for(i = 0; i < ePAL_DMX_TscPidType_Max; i++)
	{
		(void)pal_dmx_StopTscMonitor(ulDemuxId, i);
	}
#endif

	return ERR_OK;
}

STATIC HERROR pal_dmx_StartAllTscMonitoring(HUINT32 ulDemuxId, HUINT16 usVideoPid, HUINT16 usMainAudioPid)
{
#if !defined(CONFIG_MW_DISABLE_TSC_MONITOR)
	if(ulDemuxId >= s_ulNumOfDmx)
	{
		HxLOG_Error("invalid param - ulDemuxId(%d of %d) \n", ulDemuxId, s_ulNumOfDmx);
		return ERR_FAIL;
	}

	if(FALSE == s_astDmxStatus[ulDemuxId].bSupportTscMonitor)
	{
		HxLOG_Warning("s_astDmxStatus[%d].bSupportTscMonitor = %d\n", ulDemuxId, s_astDmxStatus[ulDemuxId].bSupportTscMonitor);
		return ERR_OK;
	}

	/* 1. main video program (tv) */
	if(TRUE == palDmx_VALID_PID(usVideoPid))
	{
		(void)pal_dmx_StartTscMonitor(ulDemuxId, ePAL_DMX_TscPidType_Video, usVideoPid);
	}

	/* in case of using a brcm rave,
		many fcc decodings or its tsc monitoring may cause 'not enough rave context'.
		If do that, pvr functions may not work.
		Therefore, when fcc is supported, tsc monitoring starts of only one pid.
	*/
#if defined(CONFIG_SUPPORT_FCC)
	else
#endif

	/* 2. main audio only program (radio) */
	if(TRUE == palDmx_VALID_PID(usMainAudioPid))
	{
		(void)pal_dmx_StartTscMonitor(ulDemuxId, ePAL_DMX_TscPidType_Audio, usMainAudioPid);
	}
#endif

	return ERR_OK;
}

STATIC HERROR pal_dmx_RestartAllTscMonitoring(HUINT32 ulDemuxId, HUINT16 usVideoPid, HUINT16 usMainAudioPid)
{
#if !defined(CONFIG_MW_DISABLE_TSC_MONITOR)
	if(ulDemuxId >= s_ulNumOfDmx)
	{
		HxLOG_Error("invalid param - ulDemuxId(%d of %d) \n", ulDemuxId, s_ulNumOfDmx);
		return ERR_FAIL;
	}

	if(FALSE == s_astDmxStatus[ulDemuxId].bSupportTscMonitor)
	{
		HxLOG_Warning("s_astDmxStatus[%d].bSupportTscMonitor = %d\n", ulDemuxId, s_astDmxStatus[ulDemuxId].bSupportTscMonitor);
		return ERR_OK;
	}

	(void)pal_dmx_StopAllTscMonitor(ulDemuxId);
	(void)pal_dmx_StartAllTscMonitoring(ulDemuxId, usVideoPid, usMainAudioPid);
#endif

	return ERR_OK;
}

#define __________________________________________________________


#if defined(CONFIG_SUPPORT_FCC)
STATIC void pal_dmx_FccResetFilterStatus(HUINT32 ulDemuxId)
{
	s_astDmxStatus[ulDemuxId].stStatus.bFccMainPcrFilter = FALSE;
	s_astDmxStatus[ulDemuxId].stStatus.bFccMainVideoFilter = FALSE;
	s_astDmxStatus[ulDemuxId].stStatus.bFccMainAudioFilter = FALSE;

	return;
}

STATIC void pal_dmx_FccResetStatus(HUINT32 ulDemuxId)
{
	pal_dmx_ResetPidInfo(ulDemuxId);
	pal_dmx_FccResetFilterStatus(ulDemuxId);

	HxLOG_Debug("(%d) bFccMainPcrFilter(%d) bFccMainVideoFilter(%d)\n"
		, ulDemuxId
		, s_astDmxStatus[ulDemuxId].stStatus.bFccMainPcrFilter
		, s_astDmxStatus[ulDemuxId].stStatus.bFccMainVideoFilter
		);

	return;
}
#endif

STATIC HERROR pal_dmx_UpdateFilterStatus(HUINT32 ulDemuxId, HUINT32 filter_state_mask)
{
	if(ulDemuxId >= s_ulNumOfDmx)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	if(filter_state_mask & PID_BITMASK_PCR)
	{
		s_astDmxStatus[ulDemuxId].stStatus.bMainPcrFilter = TRUE;
	}

	if(filter_state_mask & PID_BITMASK_PCR_SUB)
	{
		s_astDmxStatus[ulDemuxId].stStatus.bSubPcrFilter = TRUE;
	}

	if(filter_state_mask & PID_BITMASK_VIDEO)
	{
		s_astDmxStatus[ulDemuxId].stStatus.bMainVideoFilter = TRUE;
	}

	if(filter_state_mask & PID_BITMASK_VIDEO_SUB)
	{
		s_astDmxStatus[ulDemuxId].stStatus.bSubVideoFilter = TRUE;
	}

	if(filter_state_mask & PID_BITMASK_AUDIO_MAIN)
	{
		s_astDmxStatus[ulDemuxId].stStatus.bMainAudioFilter = TRUE;
	}

	if(filter_state_mask & PID_BITMASK_AUDIO_SUB)
	{
		s_astDmxStatus[ulDemuxId].stStatus.bSubAudioFilter = TRUE;
	}

#if defined(CONFIG_SUPPORT_FCC)
	if(filter_state_mask & PID_BITMASK_PCR_PRIMER)
	{
		s_astDmxStatus[ulDemuxId].stStatus.bFccMainPcrFilter = TRUE;
	}

	if(filter_state_mask & PID_BITMASK_VIDEO_PRIMER)
	{
		s_astDmxStatus[ulDemuxId].stStatus.bFccMainVideoFilter = TRUE;
	}

	if(filter_state_mask & PID_BITMASK_AUDIO_PRIMER)
	{
		s_astDmxStatus[ulDemuxId].stStatus.bFccMainAudioFilter = TRUE;
	}
#endif

	return ERR_OK;
}


#define ____EXTERNAL_FUNCTIONS___________________________

HERROR PAL_DMX_Init(void)
{
	DI_DEMUX_CAPABILITY di_cap;
	HUINT32 i = 0;
	HINT32 nResult = 0;

	DI_DEMUX_GetNumOfDemux(&s_ulNumOfDmx);

	HxLOG_Print("number of demux %d  \n", s_ulNumOfDmx);

	s_astDmxStatus = (PalDmx_Status_t *)OxMW_Malloc(sizeof(PalDmx_Status_t) * s_ulNumOfDmx);
	if(s_astDmxStatus == NULL)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	for(i=0; i<s_ulNumOfDmx; i++)
	{
		pal_dmx_ResetStatus(i);
		(void)pal_dmx_ResetTscData(i);

		DI_DEMUX_GetCapability(i, &di_cap);

		HxLOG_Print("di_cap.ulCapability (0x%x) \n", di_cap.ulCapability);
		/*
			capability로 scarmble bit check 가능 여부.
		*/
		if(di_cap.ulCapability & DI_DEMUX_CAP_NOTIFY_SCRAMBLE_TS)
		{
			s_astDmxStatus[i].bSupportTscMonitor = TRUE;
		}
	}

	nResult = VK_SEM_Create(&s_ulPalDmxSemId, "PalDmx", VK_SUSPENDTYPE_PRIORITY);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("Failure in Semaphore Creation \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_DMX_StartAVPid(HUINT32 ulDemuxId, PalDmx_PidInfo_t *pstPidInfo, HBOOL bMainAV)
{
	HUINT32 			nResult = 0;
	Pid_Info 			pid_info;
	HUINT32 			filter_start_mask;

	if(pstPidInfo == NULL || ulDemuxId >= s_ulNumOfDmx)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	HxSTD_memset(&pid_info, 0x00, sizeof(Pid_Info));

	palDmx_ENTER_CRITICAL;

	if(bMainAV == TRUE)
	{
		s_bMainAvDemuxId = ulDemuxId;
	}

	if(palDmx_VALID_PCRPID(pstPidInfo->usPcrPid) == TRUE)
	{
		if(bMainAV == TRUE)
		{
			pid_info.pidBitmask |= PID_BITMASK_PCR;
		}
		else
		{
			pid_info.pidBitmask |= PID_BITMASK_PCR_SUB;
		}

		pid_info.usPCRPid = pstPidInfo->usPcrPid;
		s_astDmxStatus[ulDemuxId].stPid.usPcrPid = pstPidInfo->usPcrPid;
		HxLOG_Debug("pcr = 0x%x\n", pstPidInfo->usPcrPid);
	}

	if(palDmx_VALID_PID(pstPidInfo->usVideoPid) == TRUE)
	{
		if(bMainAV == TRUE)
		{
			pid_info.pidBitmask |= PID_BITMASK_VIDEO;
		}
		else
		{
			pid_info.pidBitmask |= PID_BITMASK_VIDEO_SUB;
		}

		pid_info.usVideoPid = pstPidInfo->usVideoPid;
		s_astDmxStatus[ulDemuxId].stPid.usVideoPid = pstPidInfo->usVideoPid;
		HxLOG_Debug("video = 0x%x\n", pstPidInfo->usVideoPid);
	}

	if(bMainAV == TRUE)
	{
		if(palDmx_VALID_PID(pstPidInfo->usMainAudioPid) == TRUE)
		{
			pid_info.pidBitmask |= PID_BITMASK_AUDIO_MAIN;
			pid_info.usAudioMainPid = pstPidInfo->usMainAudioPid;
			s_astDmxStatus[ulDemuxId].stPid.usMainAudioPid = pstPidInfo->usMainAudioPid;
			HxLOG_FCC("main audio = 0x%x\n", pstPidInfo->usMainAudioPid);
		}

		if(palDmx_VALID_PID(pstPidInfo->usSubAudioPid) == TRUE)
		{
			pid_info.pidBitmask |= PID_BITMASK_AUDIO_SUB;
			pid_info.usAudioSubPid = pstPidInfo->usSubAudioPid;
			s_astDmxStatus[ulDemuxId].stPid.usSubAudioPid = pstPidInfo->usSubAudioPid;
			HxLOG_FCC("sub audio = 0x%x\n", pstPidInfo->usSubAudioPid);
		}
	}

	filter_start_mask = pid_info.pidBitmask;

	HxLOG_FCC("s_bMainAvDemuxId(%d) bitMask(0x%X) vap(0x%X 0x%X 0x%X) \n",
				s_bMainAvDemuxId,
				pid_info.pidBitmask,
				pid_info.usVideoPid,
				pid_info.usAudioMainPid,
				pid_info.usPCRPid);
	HxLOG_FCC(" >>>>>>> DI_DEMUX_StartPID ulDemuxId(%d) <<<<<<<<<<<<<< \n", ulDemuxId);

	HxLOG_Debug("++ pid_info.pidBitmask = 0x%x\n", pid_info.pidBitmask);
	HxLOG_Print(HxANSI_COLOR_YELLOW("++ ulDemuxId(%d) bMainAV(%d) mask(%c%c%c%c%c%c%c%c%c)\n")
		, ulDemuxId
		, bMainAV
		, (0 != (pid_info.pidBitmask & PID_BITMASK_PCR))          ? 'P' : '_'
		, (0 != (pid_info.pidBitmask & PID_BITMASK_VIDEO))        ? 'V' : '_'
		, (0 != (pid_info.pidBitmask & PID_BITMASK_AUDIO_MAIN))   ? 'A' : '_'
		, (0 != (pid_info.pidBitmask & PID_BITMASK_PCR_PRIMER))   ? 'P' : '_'
		, (0 != (pid_info.pidBitmask & PID_BITMASK_VIDEO_PRIMER)) ? 'V' : '_'
		, (0 != (pid_info.pidBitmask & PID_BITMASK_AUDIO_PRIMER)) ? 'A' : '_'
		, (0 != (pid_info.pidBitmask & PID_BITMASK_PCR_SUB))      ? 'P' : '_'
		, (0 != (pid_info.pidBitmask & PID_BITMASK_VIDEO_SUB))    ? 'V' : '_'
		, (0 != (pid_info.pidBitmask & PID_BITMASK_AUDIO_SUB))    ? 'A' : '_'
		);


	nResult = DI_DEMUX_StartPID(ulDemuxId, &pid_info);
	if(nResult != ERR_OK)
	{
		pal_dmx_ResetStatus(ulDemuxId);
		HxLOG_Error("ERROR from DI_DEMUX_StartPID(). filter_start_mask (0x%x), pid_info.pidBitmask (0x%x) \n"
					, filter_start_mask, pid_info.pidBitmask);
		palDmx_LEAVE_CRITICAL;
		return ERR_FAIL;
	}

	if(pid_info.pidBitmask != filter_start_mask)
	{
		HxLOG_Error(" \n");
	}

	nResult = pal_dmx_UpdateFilterStatus(ulDemuxId, pid_info.pidBitmask);
	if(nResult != ERR_OK)
	{
		HxLOG_Error(" \n");
		palDmx_LEAVE_CRITICAL;
		return ERR_FAIL;
	}

	(void)pal_dmx_RestartAllTscMonitoring(ulDemuxId, pstPidInfo->usVideoPid, pstPidInfo->usMainAudioPid);

	palDmx_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_DMX_ChangeAVPid(HUINT32 ulDemuxId, PalDmx_PidInfo_t *pstPidInfo)
{
	HUINT32			nResult = 0;
	Pid_Info		pid_info;
	HUINT32			filter_stop_mask = 0;
	HUINT32			filter_start_mask = 0;

	if(pstPidInfo == NULL || ulDemuxId >= s_ulNumOfDmx)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	HxSTD_memset(&pid_info, 0x00, sizeof(Pid_Info));

	palDmx_ENTER_CRITICAL;

	(void)pal_dmx_StopAllTscMonitor(ulDemuxId);

//	if(s_astDmxStatus[ulDemuxId].stPid.usPcrPid != pstPidInfo->usPcrPid)
	if((s_astDmxStatus[ulDemuxId].stPid.usPcrPid != pstPidInfo->usPcrPid) && (palDmx_VALID_PCRPID(pstPidInfo->usPcrPid) == TRUE))
	{
		if(s_astDmxStatus[ulDemuxId].stStatus.bMainPcrFilter == TRUE)
		{
			filter_stop_mask |= PID_BITMASK_PCR;
		}
		else if(s_astDmxStatus[ulDemuxId].stStatus.bSubPcrFilter == TRUE)
		{
			filter_stop_mask |= PID_BITMASK_PCR_SUB;
		}

		s_astDmxStatus[ulDemuxId].stPid.usPcrPid = pstPidInfo->usPcrPid;

		//if(palDmx_VALID_PCRPID(pstPidInfo->usPcrPid) == TRUE)
		{
			if(ulDemuxId == s_bMainAvDemuxId)
			{
				pid_info.pidBitmask |= PID_BITMASK_PCR;
			}
			else
			{
				pid_info.pidBitmask |= PID_BITMASK_PCR_SUB;
			}

			pid_info.usPCRPid = pstPidInfo->usPcrPid;
		}
	}

//	if(s_astDmxStatus[ulDemuxId].stPid.usVideoPid != pstPidInfo->usVideoPid)
	if((s_astDmxStatus[ulDemuxId].stPid.usVideoPid != pstPidInfo->usVideoPid) && (palDmx_VALID_PID(pstPidInfo->usVideoPid) == TRUE))
	{
		if(s_astDmxStatus[ulDemuxId].stStatus.bMainVideoFilter == TRUE)
		{
			filter_stop_mask |= PID_BITMASK_VIDEO;
		}
		else if(s_astDmxStatus[ulDemuxId].stStatus.bSubVideoFilter == TRUE)
		{
			filter_stop_mask |= PID_BITMASK_VIDEO_SUB;
		}

		s_astDmxStatus[ulDemuxId].stPid.usVideoPid = pstPidInfo->usVideoPid;

		//if(palDmx_VALID_PID(pstPidInfo->usVideoPid) == TRUE)
		{
			if(ulDemuxId == s_bMainAvDemuxId)
			{
				pid_info.pidBitmask |= PID_BITMASK_VIDEO;
				pid_info.usVideoPid = pstPidInfo->usVideoPid;
			}
			else
			{
				pid_info.pidBitmask |= PID_BITMASK_VIDEO_SUB;
				pid_info.usVideoPid = pstPidInfo->usVideoPid;
			}
		}
	}

	if(ulDemuxId == s_bMainAvDemuxId)
	{
		//if(s_astDmxStatus[ulDemuxId].stPid.usMainAudioPid != pstPidInfo->usMainAudioPid)
		if((s_astDmxStatus[ulDemuxId].stPid.usMainAudioPid != pstPidInfo->usMainAudioPid) && (palDmx_VALID_PID(pstPidInfo->usMainAudioPid) == TRUE))
		{
			if(s_astDmxStatus[ulDemuxId].stStatus.bMainAudioFilter == TRUE)
			{
				filter_stop_mask |= PID_BITMASK_AUDIO_MAIN;
			}

			s_astDmxStatus[ulDemuxId].stPid.usMainAudioPid = pstPidInfo->usMainAudioPid;

			if(palDmx_VALID_PID(pstPidInfo->usMainAudioPid) == TRUE)
			{
				pid_info.pidBitmask |= PID_BITMASK_AUDIO_MAIN;
				pid_info.usAudioMainPid = pstPidInfo->usMainAudioPid;
			}
		}

		//if(s_astDmxStatus[ulDemuxId].stPid.usSubAudioPid != pstPidInfo->usSubAudioPid)
		if((s_astDmxStatus[ulDemuxId].stPid.usSubAudioPid != pstPidInfo->usSubAudioPid) && (palDmx_VALID_PID(pstPidInfo->usSubAudioPid) == TRUE))
		{
			if(s_astDmxStatus[ulDemuxId].stStatus.bSubAudioFilter == 1)
			{
				filter_stop_mask |= PID_BITMASK_AUDIO_SUB;
			}

			s_astDmxStatus[ulDemuxId].stPid.usSubAudioPid = pstPidInfo->usSubAudioPid;

			if(palDmx_VALID_PID(pstPidInfo->usSubAudioPid) == TRUE)
			{
				pid_info.pidBitmask |= PID_BITMASK_AUDIO_SUB;
				pid_info.usAudioSubPid = pstPidInfo->usSubAudioPid;
			}
		}
	}

	if (filter_stop_mask)
	{
		HxLOG_FCC("Stop demux pid :: dmxId(%d) bitMask(0x%X) \n", ulDemuxId, filter_stop_mask);

		HxLOG_Print(HxANSI_COLOR_YELLOW("-- ulDemuxId = %d, bMainAV = %d\n"), ulDemuxId, ulDemuxId == s_bMainAvDemuxId);

	    nResult = DI_DEMUX_StopPID(ulDemuxId, filter_stop_mask);
	    if(nResult != ERR_OK)
	    {
		    HxLOG_Error("ERROR from DI_DEMUX_StopPID()..... CHECK PLEASE !!!!! \n");
		    palDmx_LEAVE_CRITICAL;
		    return ERR_FAIL;
	    }
	}

	if (filter_stop_mask & PID_BITMASK_PCR)
		s_astDmxStatus[ulDemuxId].stStatus.bMainPcrFilter = FALSE;

	if (filter_stop_mask & PID_BITMASK_PCR_SUB)
		s_astDmxStatus[ulDemuxId].stStatus.bSubPcrFilter = FALSE;

	if (filter_stop_mask & PID_BITMASK_VIDEO)
		s_astDmxStatus[ulDemuxId].stStatus.bMainVideoFilter = FALSE;

	if (filter_stop_mask & PID_BITMASK_VIDEO_SUB)
		s_astDmxStatus[ulDemuxId].stStatus.bSubVideoFilter = FALSE;

	if (filter_stop_mask & PID_BITMASK_AUDIO_MAIN)
		s_astDmxStatus[ulDemuxId].stStatus.bMainAudioFilter = FALSE;

	if (filter_stop_mask & PID_BITMASK_AUDIO_SUB)
		s_astDmxStatus[ulDemuxId].stStatus.bSubAudioFilter = FALSE;

	filter_start_mask = pid_info.pidBitmask;

	if (filter_start_mask)
	{
		HxLOG_FCC("bitMask(0x%X) allBitMask(0x%X) avp(0x%X 0x%X 0x%X) \n",
				pid_info.pidBitmask, (PID_BITMASK_VIDEO | PID_BITMASK_AUDIO_MAIN | PID_BITMASK_PCR),
				pid_info.usVideoPid,
				pid_info.usAudioMainPid,
				pid_info.usPCRPid);
		HxLOG_FCC(" >>>>>>> DI_DEMUX_StartPID ulDemuxId(%d) <<<<<<<<<<<<<< \n", ulDemuxId);

		HxLOG_Print(HxANSI_COLOR_YELLOW("++ ulDemuxId = %d, bMainAV = %d\n"), ulDemuxId, ulDemuxId == s_bMainAvDemuxId);

	    nResult = DI_DEMUX_StartPID(ulDemuxId, &pid_info);
	    if(nResult != ERR_OK)
	    {
			HxLOG_Error("ERROR from DI_DEMUX_StartPID(). filter_start_mask (0x%x), pid_info.pidBitmask (0x%x) \n"
						, filter_start_mask, pid_info.pidBitmask);
		    palDmx_LEAVE_CRITICAL;
		    return ERR_FAIL;
	    }
	}

	if(pid_info.pidBitmask != filter_start_mask)
	{
		HxLOG_Error(" \n");
	}

	nResult = pal_dmx_UpdateFilterStatus(ulDemuxId, pid_info.pidBitmask);
	if(nResult != ERR_OK)
	{
		HxLOG_Error(" \n");
		palDmx_LEAVE_CRITICAL;
		return ERR_FAIL;
	}

	(void)pal_dmx_StartAllTscMonitoring(ulDemuxId, pstPidInfo->usVideoPid, pstPidInfo->usMainAudioPid);

	palDmx_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_DMX_StopAllAVPid(HUINT32 ulDemuxId)
{
	HUINT32 			nResult = 0;
	HUINT32				filter_stop_mask = 0;

	if(ulDemuxId >= s_ulNumOfDmx)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	palDmx_ENTER_CRITICAL;

	(void)pal_dmx_StopAllTscMonitor(ulDemuxId);

	if(s_astDmxStatus[ulDemuxId].stStatus.bMainPcrFilter == TRUE)
	{
		filter_stop_mask |= PID_BITMASK_PCR;
	}

	if(s_astDmxStatus[ulDemuxId].stStatus.bSubPcrFilter == TRUE)
	{
		filter_stop_mask |= PID_BITMASK_PCR_SUB;
	}

	if(s_astDmxStatus[ulDemuxId].stStatus.bMainVideoFilter == TRUE)
	{
		filter_stop_mask |= PID_BITMASK_VIDEO;
	}

	if(s_astDmxStatus[ulDemuxId].stStatus.bSubVideoFilter == TRUE)
	{
		filter_stop_mask |= PID_BITMASK_VIDEO_SUB;
	}

	if(s_astDmxStatus[ulDemuxId].stStatus.bMainAudioFilter == TRUE)
	{
		filter_stop_mask |= PID_BITMASK_AUDIO_MAIN;
	}

	if(s_astDmxStatus[ulDemuxId].stStatus.bSubAudioFilter == TRUE)
	{
		filter_stop_mask |= PID_BITMASK_AUDIO_SUB;
	}

	HxLOG_FCC("Stop demux pid :: dmxId(%d) bitMask(0x%X)\n", ulDemuxId, filter_stop_mask);

	HxLOG_Print(HxANSI_COLOR_YELLOW("-- ulDemuxId(%d) bMainAV(%d) mask(%c%c%c%c%c%c%c%c%c)\n")
		, ulDemuxId
		, s_astDmxStatus[ulDemuxId].stStatus.bMainVideoFilter
		, (0 != (filter_stop_mask & PID_BITMASK_PCR))          ? 'P' : '_'
		, (0 != (filter_stop_mask & PID_BITMASK_VIDEO))        ? 'V' : '_'
		, (0 != (filter_stop_mask & PID_BITMASK_AUDIO_MAIN))   ? 'A' : '_'
		, (0 != (filter_stop_mask & PID_BITMASK_PCR_PRIMER))   ? 'P' : '_'
		, (0 != (filter_stop_mask & PID_BITMASK_VIDEO_PRIMER)) ? 'V' : '_'
		, (0 != (filter_stop_mask & PID_BITMASK_AUDIO_PRIMER)) ? 'A' : '_'
		, (0 != (filter_stop_mask & PID_BITMASK_PCR_SUB))      ? 'P' : '_'
		, (0 != (filter_stop_mask & PID_BITMASK_VIDEO_SUB))    ? 'V' : '_'
		, (0 != (filter_stop_mask & PID_BITMASK_AUDIO_SUB))    ? 'A' : '_'
		);

	nResult = DI_DEMUX_StopPID(ulDemuxId, filter_stop_mask);
	if(nResult != ERR_OK)
	{
		HxLOG_Error(" \n");
		palDmx_LEAVE_CRITICAL;
		return ERR_FAIL;
	}

#if !defined(CONFIG_SUPPORT_FCC)
	pal_dmx_ResetStatus(ulDemuxId);
#else
	if(filter_stop_mask & PID_BITMASK_PCR)
	{
		s_astDmxStatus[ulDemuxId].stPid.usPcrPid = PID_NULL;
		s_astDmxStatus[ulDemuxId].stStatus.bMainPcrFilter = FALSE;
	}

	if(filter_stop_mask & PID_BITMASK_PCR_SUB)
	{
		s_astDmxStatus[ulDemuxId].stStatus.bSubPcrFilter = FALSE;
	}

	if(filter_stop_mask & PID_BITMASK_VIDEO)
	{
		s_astDmxStatus[ulDemuxId].stPid.usVideoPid = PID_NULL;
		s_astDmxStatus[ulDemuxId].stStatus.bMainVideoFilter = FALSE;
	}

	if(filter_stop_mask & PID_BITMASK_VIDEO_SUB)
	{
		s_astDmxStatus[ulDemuxId].stStatus.bSubVideoFilter = FALSE;
	}

	if(filter_stop_mask & PID_BITMASK_AUDIO_MAIN)
	{
		s_astDmxStatus[ulDemuxId].stPid.usMainAudioPid = PID_NULL;
		s_astDmxStatus[ulDemuxId].stStatus.bMainAudioFilter = FALSE;
	}

	if(filter_stop_mask & PID_BITMASK_AUDIO_SUB)
	{
		s_astDmxStatus[ulDemuxId].stPid.usSubAudioPid = PID_NULL;
		s_astDmxStatus[ulDemuxId].stStatus.bSubAudioFilter = FALSE;
	}
#endif

	palDmx_LEAVE_CRITICAL;

	return ERR_OK;
}

PalDmx_PidInfo_t *PAL_DMX_GetAVPidStatus(HUINT32 ulDemuxId)
{
	PalDmx_PidInfo_t *pDmxInfo = (PalDmx_PidInfo_t *)NULL;

	if(ulDemuxId >= s_ulNumOfDmx)
	{
		HxLOG_Error(" \n");
		return (PalDmx_PidInfo_t *)NULL;
	}

	palDmx_ENTER_CRITICAL;
	pDmxInfo = &s_astDmxStatus[ulDemuxId].stPid;
	palDmx_LEAVE_CRITICAL;

	return pDmxInfo;
}

#define ______TSC_MONITORING_______________________

HERROR PAL_DMX_CheckScrambleBitByPid(HUINT32 ulDemuxId, PalDmx_TsState_e *peState)
{
#if !defined(CONFIG_MW_DISABLE_TSC_MONITOR)
	DI_DEMUX_TsState_t video_state = eDI_DMX_TS_Clean, audio_state = eDI_DMX_TS_Clean;
	HBOOL bExistVideo = FALSE, bExistAudio = FALSE;

	if(ulDemuxId >= s_ulNumOfDmx)
	{
		HxLOG_Error("ulDemuxId(%d) >= s_ulNumOfDmx(%d) \n", ulDemuxId, s_ulNumOfDmx);
		return ERR_FAIL;
	}

	palDmx_ENTER_CRITICAL;

	if(s_astDmxStatus[ulDemuxId].bSupportTscMonitor != TRUE)
	{
		HxLOG_Error("s_astDmxStatus[ulDemuxId].bSupportTscMonitor != TRUE \n");
		palDmx_LEAVE_CRITICAL;
		return ERR_FAIL;
	}

	HxLOG_Debug("s_astDmxStatus[%d].ausTscPid[0]: 0x%x, .aulTscHandle[0]: 0x%x \n", ulDemuxId, s_astDmxStatus[ulDemuxId].ausTscPid[0], s_astDmxStatus[ulDemuxId].aulTscHandle[0]);
	if(s_astDmxStatus[ulDemuxId].ausTscPid[0] != PID_NULL && s_astDmxStatus[ulDemuxId].aulTscHandle[0] != HANDLE_NULL)
	{
		DI_DEMUX_GetTSCStatus(s_astDmxStatus[ulDemuxId].aulTscHandle[0], &video_state);
		HxLOG_Debug("video_state: %d \n", video_state);
		bExistVideo = TRUE;
	}

	HxLOG_Debug("s_astDmxStatus[%d].ausTscPid[1]: 0x%x, .aulTscHandle[1]: 0x%x \n", ulDemuxId, s_astDmxStatus[ulDemuxId].ausTscPid[1], s_astDmxStatus[ulDemuxId].aulTscHandle[1]);
	if(s_astDmxStatus[ulDemuxId].ausTscPid[1] != PID_NULL && s_astDmxStatus[ulDemuxId].aulTscHandle[1] != HANDLE_NULL)
	{
		DI_DEMUX_GetTSCStatus(s_astDmxStatus[ulDemuxId].aulTscHandle[1], &audio_state);
		HxLOG_Debug("audio_state: %d \n", audio_state);
		bExistAudio = TRUE;
	}


	if(bExistAudio == TRUE && bExistVideo == TRUE)
	{
		if(video_state == eDI_DMX_TS_Clean && audio_state == eDI_DMX_TS_Clean)
		{
			*peState = ePAL_DMX_TSSTATE_CLEAN;
		}
		else if (video_state == eDI_DMX_TS_Scramble || audio_state == eDI_DMX_TS_Scramble)
		{
			*peState = ePAL_DMX_TSSTATE_SCRAMBLED;
		}
		else
		{
			*peState = ePAL_DMX_TSSTATE_UNKNOWN;
		}
	}
	else
	{
		*peState = ePAL_DMX_TSSTATE_UNKNOWN;
	}

	HxLOG_Debug("demux id (%d), state (%d) \n", ulDemuxId, *peState);

	palDmx_LEAVE_CRITICAL;
#else
	if(NULL != peState)
	{
		*peState = ePAL_DMX_TSSTATE_UNKNOWN;
	}
#endif

	return ERR_OK;
}

HBOOL PAL_DMX_IsSupportScrambleBitChecking(HUINT32 ulDemuxId)
{
#if !defined(CONFIG_MW_DISABLE_TSC_MONITOR)
	if(ulDemuxId >= s_ulNumOfDmx)
	{
		HxLOG_Error(" \n");
		return FALSE;
	}

	palDmx_ENTER_CRITICAL;

	if(s_astDmxStatus[ulDemuxId].bSupportTscMonitor == TRUE)
	{
		palDmx_LEAVE_CRITICAL;
		return TRUE;
	}

	palDmx_LEAVE_CRITICAL;
#endif

	return FALSE;
}

#define _________________________________________________________________


HERROR PAL_DMX_GetPlayBackId(HUINT32 unDmxId, HUINT32 *punPlayBackId)
{
	if(punPlayBackId == NULL)
	{
		return ERR_FAIL;
	}

	return DI_DEMUX_GetPlaybackId(unDmxId , punPlayBackId);
}

#if defined(CONFIG_SUPPORT_FCC)
HERROR PAL_DMX_StartAVPrimerPid(HUINT32 ulDemuxId, PalDmx_PidInfo_t *pstPidInfo, HBOOL bMainAV, HUINT32 ulPrimerIdx)
{
	HUINT32 			nResult = 0;
	Pid_Info 			pid_info;
	HUINT32 			filter_start_mask;
	PrimerPid_Info 		stPrimerPidInfo;

	if(pstPidInfo == NULL || ulDemuxId >= s_ulNumOfDmx)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	HxSTD_memset(&pid_info, 0x00, sizeof(Pid_Info));

	palDmx_ENTER_CRITICAL;

	if(bMainAV == TRUE)
	{
		s_bMainAvDemuxId = ulDemuxId;
	}

	if(palDmx_VALID_PCRPID(pstPidInfo->usPcrPid) == TRUE)
	{
		pid_info.pidBitmask |= PID_BITMASK_PCR_PRIMER;

		pid_info.usPCRPid = pstPidInfo->usPcrPid;
		s_astDmxStatus[ulDemuxId].stPid.usPcrPid = pstPidInfo->usPcrPid;
		HxLOG_Print(HxANSI_COLOR_CYAN("(D_%d) pcr (0x%x)\n"), ulDemuxId, pstPidInfo->usPcrPid);
	}

	if(palDmx_VALID_PID(pstPidInfo->usVideoPid) == TRUE)
	{
		pid_info.pidBitmask |= PID_BITMASK_VIDEO_PRIMER;

		pid_info.usVideoPid = pstPidInfo->usVideoPid;
		s_astDmxStatus[ulDemuxId].stPid.usVideoPid = pstPidInfo->usVideoPid;
		HxLOG_Print(HxANSI_COLOR_CYAN("(D_%d) video (0x%x)\n"), ulDemuxId, pstPidInfo->usVideoPid);
	}

	if(palDmx_VALID_PID(pstPidInfo->usMainAudioPid) == TRUE)
	{
		pid_info.pidBitmask |= PID_BITMASK_AUDIO_PRIMER;
		pid_info.usAudioMainPid = pstPidInfo->usMainAudioPid;
		s_astDmxStatus[ulDemuxId].stPid.usMainAudioPid = pstPidInfo->usMainAudioPid;
		HxLOG_Print(HxANSI_COLOR_CYAN("(D_%d) main audio (0x%x)\n"), ulDemuxId, pstPidInfo->usMainAudioPid);
	}

	filter_start_mask = pid_info.pidBitmask;

#if 0
	nResult = DI_DEMUX_StartPID(ulDemuxId, &pid_info);
#endif

	HxSTD_MemSet(&stPrimerPidInfo, 0x00, sizeof(stPrimerPidInfo));
	stPrimerPidInfo.ulPrimerIndex = ulPrimerIdx;
	stPrimerPidInfo.ulPidBitmask = pid_info.pidBitmask;
	stPrimerPidInfo.usVideoPid = pid_info.usVideoPid;
	stPrimerPidInfo.usAudioPid = pid_info.usAudioMainPid;
	stPrimerPidInfo.usPcrPid = pid_info.usPCRPid;

	HxLOG_FCC("primerIdx(%d) s_bMainAvDemuxId(%d) bitMask(0x%X) avp(0x%X 0x%X 0x%X) \n",
				stPrimerPidInfo.ulPrimerIndex,
				s_bMainAvDemuxId,
				stPrimerPidInfo.ulPidBitmask,
				stPrimerPidInfo.usVideoPid,
				stPrimerPidInfo.usAudioPid,
				stPrimerPidInfo.usPcrPid);
	HxLOG_FCC(" >>>>>>> DI_DEMUX_StartPrimerPID ulDemuxId(%d) ulPrimerIndex(%d)	<<<<<<<<<<<<<< \n", ulDemuxId, stPrimerPidInfo.ulPrimerIndex);

	HxLOG_Debug("++ pid_info.pidBitmask = 0x%x\n", pid_info.pidBitmask);
	HxLOG_Print(HxANSI_COLOR_YELLOW("++ ulDemuxId(%d) bMainAV(%d) ulPrimerIdx(%d) mask(%c%c%c%c%c%c%c%c%c)\n")
		, ulDemuxId
		, bMainAV
		, ulPrimerIdx
		, (0 != (stPrimerPidInfo.ulPidBitmask & PID_BITMASK_PCR))          ? 'P' : '_'
		, (0 != (stPrimerPidInfo.ulPidBitmask & PID_BITMASK_VIDEO))        ? 'V' : '_'
		, (0 != (stPrimerPidInfo.ulPidBitmask & PID_BITMASK_AUDIO_MAIN))   ? 'A' : '_'
		, (0 != (stPrimerPidInfo.ulPidBitmask & PID_BITMASK_PCR_PRIMER))   ? 'P' : '_'
		, (0 != (stPrimerPidInfo.ulPidBitmask & PID_BITMASK_VIDEO_PRIMER)) ? 'V' : '_'
		, (0 != (stPrimerPidInfo.ulPidBitmask & PID_BITMASK_AUDIO_PRIMER)) ? 'A' : '_'
		, (0 != (stPrimerPidInfo.ulPidBitmask & PID_BITMASK_PCR_SUB))      ? 'P' : '_'
		, (0 != (stPrimerPidInfo.ulPidBitmask & PID_BITMASK_VIDEO_SUB))    ? 'V' : '_'
		, (0 != (stPrimerPidInfo.ulPidBitmask & PID_BITMASK_AUDIO_SUB))    ? 'A' : '_'
		);


	nResult = DI_DEMUX_StartPrimerPID(ulDemuxId, &stPrimerPidInfo);
	if(nResult != ERR_OK)
	{
		pal_dmx_ResetStatus(ulDemuxId);
		HxLOG_Error("ERROR from DI_DEMUX_StartPID(). filter_start_mask (0x%x), pid_info.pidBitmask (0x%x) \n"
					, filter_start_mask, pid_info.pidBitmask);
		palDmx_LEAVE_CRITICAL;
		return ERR_FAIL;
	}

	if(pid_info.pidBitmask != filter_start_mask)
	{
		HxLOG_Error(" \n");
	}

	nResult = pal_dmx_UpdateFilterStatus(ulDemuxId, pid_info.pidBitmask);
	if(nResult != ERR_OK)
	{
		HxLOG_Error(" \n");
		palDmx_LEAVE_CRITICAL;
		return ERR_FAIL;
	}

	(void)pal_dmx_RestartAllTscMonitoring(ulDemuxId, pstPidInfo->usVideoPid, pstPidInfo->usMainAudioPid);

	palDmx_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_DMX_StopAllAVPrimerPid(HUINT32 ulDemuxId, HUINT32 ulPrimerIdx)
{
	HUINT32 			nResult = 0;
	HUINT32				filter_stop_mask = 0;
	PrimerPid_Info 		stPrimerPidInfo;

	if(ulDemuxId >= s_ulNumOfDmx)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	palDmx_ENTER_CRITICAL;

	(void)pal_dmx_StopAllTscMonitor(ulDemuxId);

	if(s_astDmxStatus[ulDemuxId].stStatus.bFccMainPcrFilter == TRUE)
	{
		filter_stop_mask |= PID_BITMASK_PCR_PRIMER;
		HxLOG_Print("(D_%d) pcr (0x%x)\n", ulDemuxId, s_astDmxStatus[ulDemuxId].stPid.usPcrPid);
	}

/* TODO: something..
	if(s_astDmxStatus[ulDemuxId].stStatus.bSubPcrFilter == TRUE)
	{
		filter_stop_mask |= PID_BITMASK_PCR_SUB;
	}
*/

	if(s_astDmxStatus[ulDemuxId].stStatus.bFccMainVideoFilter == TRUE)
	{
		filter_stop_mask |= PID_BITMASK_VIDEO_PRIMER;
		HxLOG_Print("(D_%d) video (0x%x)\n", ulDemuxId, s_astDmxStatus[ulDemuxId].stPid.usVideoPid);
	}

/* TODO: something..
	if(s_astDmxStatus[ulDemuxId].stStatus.bSubVideoFilter == TRUE)
	{
		filter_stop_mask |= PID_BITMASK_VIDEO_SUB;
	}
*/
	if(s_astDmxStatus[ulDemuxId].stStatus.bFccMainAudioFilter == TRUE)
	{
		filter_stop_mask |= PID_BITMASK_AUDIO_PRIMER;
		HxLOG_Print("(D_%d) audio (0x%x)\n", ulDemuxId, s_astDmxStatus[ulDemuxId].stPid.usMainAudioPid);
	}

/* TODO: something..
	if(s_astDmxStatus[ulDemuxId].stStatus.bSubAudioFilter == TRUE)
	{
		filter_stop_mask |= PID_BITMASK_AUDIO_SUB;
	}
*/

	HxSTD_MemSet(&stPrimerPidInfo, 0x00, sizeof(PrimerPid_Info));
	stPrimerPidInfo.ulPrimerIndex = ulPrimerIdx;
	stPrimerPidInfo.ulPidBitmask = filter_stop_mask;

	HxLOG_FCC("Stop primer pid :: dmxId(%d) primerIdx(%d) bitMask(0x%X) \n", ulDemuxId, stPrimerPidInfo.ulPrimerIndex, stPrimerPidInfo.ulPidBitmask);

	HxLOG_Debug("++ filter_stop_mask = 0x%x\n", filter_stop_mask);
	HxLOG_Print(HxANSI_COLOR_YELLOW("-- ulDemuxId(%d) bMainAV(%d) ulPrimerIdx(%d) mask(%c%c%c%c%c%c%c%c%c)\n")
		, ulDemuxId
		, s_astDmxStatus[ulDemuxId].stStatus.bMainVideoFilter
		, ulPrimerIdx
		, (0 != (stPrimerPidInfo.ulPidBitmask & PID_BITMASK_PCR))          ? 'P' : '_'
		, (0 != (stPrimerPidInfo.ulPidBitmask & PID_BITMASK_VIDEO))        ? 'V' : '_'
		, (0 != (stPrimerPidInfo.ulPidBitmask & PID_BITMASK_AUDIO_MAIN))   ? 'A' : '_'
		, (0 != (stPrimerPidInfo.ulPidBitmask & PID_BITMASK_PCR_PRIMER))   ? 'P' : '_'
		, (0 != (stPrimerPidInfo.ulPidBitmask & PID_BITMASK_VIDEO_PRIMER)) ? 'V' : '_'
		, (0 != (stPrimerPidInfo.ulPidBitmask & PID_BITMASK_AUDIO_PRIMER)) ? 'A' : '_'
		, (0 != (stPrimerPidInfo.ulPidBitmask & PID_BITMASK_PCR_SUB))      ? 'P' : '_'
		, (0 != (stPrimerPidInfo.ulPidBitmask & PID_BITMASK_VIDEO_SUB))    ? 'V' : '_'
		, (0 != (stPrimerPidInfo.ulPidBitmask & PID_BITMASK_AUDIO_SUB))    ? 'A' : '_'
		);


	nResult = DI_DEMUX_StopPrimerPID(ulDemuxId, &stPrimerPidInfo);
	if(nResult != ERR_OK)
	{
		HxLOG_Error(" \n");
		pal_dmx_FccResetStatus(ulDemuxId);
		palDmx_LEAVE_CRITICAL;
		return ERR_FAIL;
	}

	pal_dmx_FccResetStatus(ulDemuxId);

	palDmx_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_DMX_ChangePrimerAVPid(HUINT32 ulDemuxId, PalDmx_PidInfo_t *pstPidInfo, HUINT32 ulPrimerIdx)
{
	HUINT32			nResult = 0;
	Pid_Info		pid_info;
	HUINT32			filter_stop_mask = 0;
	HUINT32			filter_start_mask = 0;
	PrimerPid_Info 	stPrimerPidInfo;

	if(pstPidInfo == NULL || ulDemuxId >= s_ulNumOfDmx)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	HxLOG_Print("ulDemuxId = %d, ulPrimerIdx = %d\n", ulDemuxId, ulPrimerIdx);

	HxSTD_memset(&pid_info, 0x00, sizeof(Pid_Info));
	palDmx_ENTER_CRITICAL;

	(void)pal_dmx_StopAllTscMonitor(ulDemuxId);

	if((s_astDmxStatus[ulDemuxId].stPid.usPcrPid != pstPidInfo->usPcrPid) && (palDmx_VALID_PCRPID(pstPidInfo->usPcrPid) == TRUE))
	//if(s_astDmxStatus[ulDemuxId].stPid.usPcrPid != pstPidInfo->usPcrPid)
	{
		if(s_astDmxStatus[ulDemuxId].stStatus.bFccMainPcrFilter == TRUE)
		{
			filter_stop_mask |= PID_BITMASK_PCR_PRIMER;
		}
		/* TODO: ...
		else if(s_astDmxStatus[ulDemuxId].stStatus.bSubPcrFilter == TRUE)
		{
			filter_stop_mask |= PID_BITMASK_PCR_SUB;
		}
		*/

		s_astDmxStatus[ulDemuxId].stPid.usPcrPid = pstPidInfo->usPcrPid;

		//if(palDmx_VALID_PCRPID(pstPidInfo->usPcrPid) == TRUE)
		{
			pid_info.pidBitmask |= PID_BITMASK_PCR_PRIMER;
			pid_info.usPCRPid = pstPidInfo->usPcrPid;
		}
	}

	//if(s_astDmxStatus[ulDemuxId].stPid.usVideoPid != pstPidInfo->usVideoPid)
	if((s_astDmxStatus[ulDemuxId].stPid.usVideoPid != pstPidInfo->usVideoPid) && (palDmx_VALID_PID(pstPidInfo->usVideoPid) == TRUE))
	{
		if(s_astDmxStatus[ulDemuxId].stStatus.bFccMainVideoFilter == TRUE)
		{
			filter_stop_mask |= PID_BITMASK_VIDEO_PRIMER;
		}
		/*
		else if(s_astDmxStatus[ulDemuxId].stStatus.bSubVideoFilter == TRUE)
		{
			filter_stop_mask |= PID_BITMASK_VIDEO_SUB;
		}
		*/

		s_astDmxStatus[ulDemuxId].stPid.usVideoPid = pstPidInfo->usVideoPid;

		//if(palDmx_VALID_PID(pstPidInfo->usVideoPid) == TRUE)
		{
			pid_info.pidBitmask |= PID_BITMASK_VIDEO_PRIMER;
			pid_info.usVideoPid = pstPidInfo->usVideoPid;
		}
	}

	if((s_astDmxStatus[ulDemuxId].stPid.usMainAudioPid != pstPidInfo->usMainAudioPid) && (palDmx_VALID_PID(pstPidInfo->usMainAudioPid) == TRUE))
	{
		if(s_astDmxStatus[ulDemuxId].stStatus.bFccMainAudioFilter == TRUE)
		{
			filter_stop_mask |= PID_BITMASK_AUDIO_PRIMER;
		}

		s_astDmxStatus[ulDemuxId].stPid.usMainAudioPid = pstPidInfo->usMainAudioPid;

		if(palDmx_VALID_PID(pstPidInfo->usMainAudioPid) == TRUE)
		{
			pid_info.pidBitmask |= PID_BITMASK_AUDIO_PRIMER;
			pid_info.usAudioMainPid = pstPidInfo->usMainAudioPid;
		}
	}

	if (filter_stop_mask)
	{
		HxSTD_MemSet(&stPrimerPidInfo, 0x00, sizeof(PrimerPid_Info));
		stPrimerPidInfo.ulPrimerIndex = ulPrimerIdx;
		stPrimerPidInfo.ulPidBitmask = filter_stop_mask;

		HxLOG_FCC("Stop primer pid :: dmxId(%d) primerIdx(%d) bitMask(0x%X) \n", ulDemuxId, stPrimerPidInfo.ulPrimerIndex, stPrimerPidInfo.ulPidBitmask);

	    nResult = DI_DEMUX_StopPrimerPID(ulDemuxId, &stPrimerPidInfo);
	    if(nResult != ERR_OK)
	    {
		    HxLOG_Error("ERROR from DI_DEMUX_StopPID()..... CHECK PLEASE !!!!! \n");
			palDmx_LEAVE_CRITICAL;
			return ERR_FAIL;
	    }
	}

	if (filter_stop_mask & PID_BITMASK_PCR_PRIMER)
	{
		s_astDmxStatus[ulDemuxId].stStatus.bFccMainPcrFilter = FALSE;
	}

	if (filter_stop_mask & PID_BITMASK_VIDEO_PRIMER)
	{
		s_astDmxStatus[ulDemuxId].stStatus.bFccMainVideoFilter = FALSE;
	}

	if (filter_stop_mask & PID_BITMASK_AUDIO_PRIMER)
	{
		s_astDmxStatus[ulDemuxId].stStatus.bFccMainAudioFilter = FALSE;
	}

	filter_start_mask = pid_info.pidBitmask;

	if (filter_start_mask)
	{
		HxSTD_MemSet(&stPrimerPidInfo, 0x00, sizeof(PrimerPid_Info));
		stPrimerPidInfo.ulPrimerIndex 	= ulPrimerIdx;
		stPrimerPidInfo.ulPidBitmask 	= filter_start_mask;
		stPrimerPidInfo.usVideoPid 		= pid_info.usVideoPid;
		stPrimerPidInfo.usAudioPid 		= pid_info.usAudioMainPid;
		stPrimerPidInfo.usPcrPid 		= pid_info.usPCRPid;

		HxLOG_FCC("primerIdx(%d) bitMask(0x%X) allBitMask(0x%X) avp(0x%X 0x%X 0x%X) \n",
						stPrimerPidInfo.ulPrimerIndex,
						stPrimerPidInfo.ulPidBitmask, (PID_BITMASK_VIDEO_PRIMER | PID_BITMASK_AUDIO_PRIMER | PID_BITMASK_PCR_PRIMER),
						stPrimerPidInfo.usVideoPid,
						stPrimerPidInfo.usAudioPid,
						stPrimerPidInfo.usPcrPid);

		HxLOG_FCC(" >>>>>>> DI_DEMUX_StartPrimerPID ulDemuxId(%d) ulPrimerIndex(%d)	<<<<<<<<<<<<<< \n", ulDemuxId, stPrimerPidInfo.ulPrimerIndex);

		HxLOG_Print(HxANSI_COLOR_YELLOW("++ ulDemuxId = %d, bMainAV = %d, ulPrimerIdx = %d\n"), ulDemuxId, s_astDmxStatus[ulDemuxId].stStatus.bFccMainVideoFilter == TRUE, ulPrimerIdx);

	    nResult = DI_DEMUX_StartPrimerPID(ulDemuxId, &stPrimerPidInfo);
	    if(nResult != ERR_OK)
	    {
			HxLOG_Error("ERROR from DI_DEMUX_StartPID(). filter_start_mask (0x%x), pid_info.pidBitmask (0x%x) \n"
						, filter_start_mask, pid_info.pidBitmask);
		    palDmx_LEAVE_CRITICAL;
		    return ERR_FAIL;
	    }
	}

	if(pid_info.pidBitmask != filter_start_mask)
	{
		HxLOG_Error(" \n");
	}

	nResult = pal_dmx_UpdateFilterStatus(ulDemuxId, pid_info.pidBitmask);
	if(nResult != ERR_OK)
	{
		HxLOG_Error(" \n");
		palDmx_LEAVE_CRITICAL;
		return ERR_FAIL;
	}

	(void)pal_dmx_StartAllTscMonitoring(ulDemuxId, pstPidInfo->usVideoPid, pstPidInfo->usMainAudioPid);

	palDmx_LEAVE_CRITICAL;

	return ERR_OK;
}


#endif

/*********************** End of File ******************************/
