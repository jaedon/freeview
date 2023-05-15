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
//#include <octo_common.h>
#include <irldvt_stb_params.h>
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
#define	palDmx_ENTER_CRITICAL			VK_SEM_Get(s_ulPalDmxSemId)
#define	palDmx_LEAVE_CRITICAL			VK_SEM_Release(s_ulPalDmxSemId)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


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

STATIC void pal_dmx_ResetPcsb(HUINT32 ulDemuxId)
{
	s_astDmxStatus[ulDemuxId].stPcsb[0] = PID_NULL;
	s_astDmxStatus[ulDemuxId].stPcsb[1] = PID_NULL;

	s_astDmxStatus[ulDemuxId].stPcsbHandle[0] = HANDLE_NULL;
	s_astDmxStatus[ulDemuxId].stPcsbHandle[1] = HANDLE_NULL;

	return;
}

STATIC void pal_dmx_ResetStatus(HUINT32 ulDemuxId)
{
	pal_dmx_ResetPidInfo(ulDemuxId);
	pal_dmx_ResetFilterStatus(ulDemuxId);
	//pal_dmx_ResetPcsb(ulDemuxId);

	return;
}


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
		s_astDmxStatus[ulDemuxId].stStatus.bMainPcrFilter = TRUE;
	}

	if(filter_state_mask & PID_BITMASK_VIDEO_PRIMER)
	{
		s_astDmxStatus[ulDemuxId].stStatus.bMainVideoFilter = TRUE;
	}

	if(filter_state_mask & PID_BITMASK_AUDIO_PRIMER)
	{
		s_astDmxStatus[ulDemuxId].stStatus.bMainAudioFilter = TRUE;
	}
#endif

	return ERR_OK;
}


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
		pal_dmx_ResetPcsb(i);

		DI_DEMUX_GetCapability(i, &di_cap);

		HxLOG_Print("di_cap.ulCapability (0x%x) \n", di_cap.ulCapability);
		/*
			capability로 scarmble bit check 가능 여부.
		*/
		if(di_cap.ulCapability & DI_DEMUX_CAP_NOTIFY_SCRAMBLE_TS)
		{
			s_astDmxStatus[i].bSupportPcsb = TRUE;
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

	if(palDmx_VALID_PID(pstPidInfo->usPcrPid) == TRUE)
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
		HxLOG_Print("pcr \n");
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
		HxLOG_Print("video \n");
	}

	if(bMainAV == TRUE)
	{
		if(palDmx_VALID_PID(pstPidInfo->usMainAudioPid) == TRUE)
		{
			pid_info.pidBitmask |= PID_BITMASK_AUDIO_MAIN;
			pid_info.usAudioMainPid = pstPidInfo->usMainAudioPid;
			s_astDmxStatus[ulDemuxId].stPid.usMainAudioPid = pstPidInfo->usMainAudioPid;
			HxLOG_Print("main audio \n");
		}

		if(palDmx_VALID_PID(pstPidInfo->usSubAudioPid) == TRUE)
		{
			pid_info.pidBitmask |= PID_BITMASK_AUDIO_SUB;
			pid_info.usAudioSubPid = pstPidInfo->usSubAudioPid;
			s_astDmxStatus[ulDemuxId].stPid.usSubAudioPid = pstPidInfo->usSubAudioPid;
			HxLOG_Print("sub audio \n");
		}
	}

	filter_start_mask = pid_info.pidBitmask;

	nResult = DI_DEMUX_StartPID(ulDemuxId, &pid_info);
	if(nResult != ERR_OK)
	{
		pal_dmx_ResetPidInfo(ulDemuxId);
		pal_dmx_ResetFilterStatus(ulDemuxId);
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

	if(s_astDmxStatus[ulDemuxId].stPid.usPcrPid != pstPidInfo->usPcrPid)
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

		if(palDmx_VALID_PID(pstPidInfo->usPcrPid) == TRUE)
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

	if(s_astDmxStatus[ulDemuxId].stPid.usVideoPid != pstPidInfo->usVideoPid)
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

		if(palDmx_VALID_PID(pstPidInfo->usVideoPid) == TRUE)
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
		if(s_astDmxStatus[ulDemuxId].stPid.usMainAudioPid != pstPidInfo->usMainAudioPid)
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

		if(s_astDmxStatus[ulDemuxId].stPid.usSubAudioPid != pstPidInfo->usSubAudioPid)
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

	nResult = DI_DEMUX_StopPID(ulDemuxId, filter_stop_mask);
	if(nResult != ERR_OK)
	{
		HxLOG_Error(" \n");
		palDmx_LEAVE_CRITICAL;
		return ERR_FAIL;
	}

	pal_dmx_ResetStatus(ulDemuxId);

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

/*
	platform이 scramble bit check를 지원하는 경우에 valid한 result를 제공.
	그외에는 platform에서 error 처리 한다.
	최대 pid 2개 지원.
*/
HERROR PAL_DMX_RegisterPidForScrambleBit(HUINT32 ulDemuxId, HUINT16 usVideoPid, HUINT32 usAudioPid)
{
	HUINT32 nResult = 0;

	HxLOG_Debug("ulDemuxId: %d, usVideoPid: 0x%x, usAudioPid: 0x%x \n", ulDemuxId, usVideoPid, usAudioPid);
	if(ulDemuxId >= s_ulNumOfDmx)
	{
		HxLOG_Error("ulDemuxId (%d) >= s_ulNumOfDmx (%d)\n", ulDemuxId, s_ulNumOfDmx);
		return ERR_FAIL;
	}

	palDmx_ENTER_CRITICAL;

	if (s_astDmxStatus[ulDemuxId].stPcsbHandle[0] != HANDLE_NULL && s_astDmxStatus[ulDemuxId].stPcsbHandle[1] != HANDLE_NULL)
	{
		HxLOG_Error("stPcsbHandle is not null \n");
		palDmx_LEAVE_CRITICAL;
		return ERR_FAIL;
	}

	if(usVideoPid != PID_NULL)
	{
		nResult = DI_DEMUX_StartTSCMonitor(ulDemuxId, usVideoPid, &s_astDmxStatus[ulDemuxId].stPcsbHandle[0]);
		if(nResult != ERR_OK)
		{
			HxLOG_Error("DI_DEMUX_StartTSCMonitor() fail \n");
			palDmx_LEAVE_CRITICAL;
			return ERR_FAIL;
		}
	}

	if(usAudioPid != PID_NULL)
	{
		nResult = DI_DEMUX_StartTSCMonitor(ulDemuxId, usAudioPid, &s_astDmxStatus[ulDemuxId].stPcsbHandle[1]);
		if(nResult != ERR_OK)
		{
			HxLOG_Error("DI_DEMUX_StartTSCMonitor() fail \n");
			palDmx_LEAVE_CRITICAL;
			return ERR_FAIL;
		}
	}

	s_astDmxStatus[ulDemuxId].stPcsb[0] = usVideoPid;
	s_astDmxStatus[ulDemuxId].stPcsb[1] = usAudioPid;

	palDmx_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_DMX_GetScrambleBitStatus(HUINT32 ulDemuxId, HUINT16 *pusVideoPid, HUINT16 *pusAudioPid)
{
	if(ulDemuxId >= s_ulNumOfDmx)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	palDmx_ENTER_CRITICAL;

	*pusVideoPid = s_astDmxStatus[ulDemuxId].stPcsb[0];
	*pusAudioPid = s_astDmxStatus[ulDemuxId].stPcsb[1];

	palDmx_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_DMX_UnRegisterPidForScrambleBit(HUINT32 ulDemuxId)
{
	HUINT32 nResult = 0;

	if(ulDemuxId >= s_ulNumOfDmx)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	palDmx_ENTER_CRITICAL;

	if(s_astDmxStatus[ulDemuxId].stPcsbHandle[0] != HANDLE_NULL)
	{
		nResult = DI_DEMUX_StopTSCMonitor(s_astDmxStatus[ulDemuxId].stPcsbHandle[0]);
		if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");
	}

	if(s_astDmxStatus[ulDemuxId].stPcsbHandle[1] != HANDLE_NULL)
	{
		nResult = DI_DEMUX_StopTSCMonitor(s_astDmxStatus[ulDemuxId].stPcsbHandle[1]);
		if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");
	}

	pal_dmx_ResetPcsb(ulDemuxId);

	palDmx_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_DMX_CheckScrambleBitByPid(HUINT32 ulDemuxId, PalDmx_TsState_e *peState)
{
	DI_DEMUX_TsState_t video_state = eDI_DMX_TS_Clean, audio_state = eDI_DMX_TS_Clean;
	HBOOL bExistVideo = FALSE, bExistAudio = FALSE;

	if(ulDemuxId >= s_ulNumOfDmx)
	{
		HxLOG_Error("ulDemuxId(%d) >= s_ulNumOfDmx(%d) \n", ulDemuxId, s_ulNumOfDmx);
		return ERR_FAIL;
	}

	palDmx_ENTER_CRITICAL;

	if(s_astDmxStatus[ulDemuxId].bSupportPcsb != TRUE)
	{
		HxLOG_Error("s_astDmxStatus[ulDemuxId].bSupportPcsb != TRUE \n");
		palDmx_LEAVE_CRITICAL;
		return ERR_FAIL;
	}

	HxLOG_Debug("ulDemuxId: %d \n", ulDemuxId);
	HxLOG_Debug("s_astDmxStatus[ulDemuxId].stPcsb[0]: 0x%x, s_astDmxStatus[ulDemuxId].stPcsbHandle[0]: 0x%x \n", s_astDmxStatus[ulDemuxId].stPcsb[0], s_astDmxStatus[ulDemuxId].stPcsbHandle[0]);

	if(s_astDmxStatus[ulDemuxId].stPcsb[0] != PID_NULL && s_astDmxStatus[ulDemuxId].stPcsbHandle[0] != HANDLE_NULL)
	{
		DI_DEMUX_GetTSCStatus(s_astDmxStatus[ulDemuxId].stPcsbHandle[0], &video_state);
		HxLOG_Debug("video_state: %d \n", video_state);
		bExistVideo = TRUE;
	}

	HxLOG_Debug("s_astDmxStatus[ulDemuxId].stPcsb[1]: 0x%x, s_astDmxStatus[ulDemuxId].stPcsbHandle[1]: 0x%x \n", s_astDmxStatus[ulDemuxId].stPcsb[0], s_astDmxStatus[ulDemuxId].stPcsbHandle[0]);

	if(s_astDmxStatus[ulDemuxId].stPcsb[1] != PID_NULL && s_astDmxStatus[ulDemuxId].stPcsbHandle[1] != HANDLE_NULL)
	{
		DI_DEMUX_GetTSCStatus(s_astDmxStatus[ulDemuxId].stPcsbHandle[1], &audio_state);
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

	HxLOG_Print("demux id (%d), state (%d) \n", ulDemuxId, *peState);

	palDmx_LEAVE_CRITICAL;

	return ERR_OK;
}

HBOOL PAL_DMX_IsSupportScrambleBitChecking(HUINT32 ulDemuxId)
{
	if(ulDemuxId >= s_ulNumOfDmx)
	{
		HxLOG_Error(" \n");
		return FALSE;
	}

	palDmx_ENTER_CRITICAL;

	if(s_astDmxStatus[ulDemuxId].bSupportPcsb == TRUE)
	{
		palDmx_LEAVE_CRITICAL;
		return TRUE;
	}

	palDmx_LEAVE_CRITICAL;

	return FALSE;
}

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

	if(palDmx_VALID_PID(pstPidInfo->usPcrPid) == TRUE)
	{
		if(bMainAV == TRUE)
		{
			pid_info.pidBitmask |= PID_BITMASK_PCR_PRIMER;
		}
		/*
		else
		{
			pid_info.pidBitmask |= PID_BITMASK_PCR_SUB;
		}
		*/

		pid_info.usPCRPid = pstPidInfo->usPcrPid;
		s_astDmxStatus[ulDemuxId].stPid.usPcrPid = pstPidInfo->usPcrPid;
		HxLOG_Print("pcr \n");
	}

	if(palDmx_VALID_PID(pstPidInfo->usVideoPid) == TRUE)
	{
		if(bMainAV == TRUE)
		{
			pid_info.pidBitmask |= PID_BITMASK_VIDEO_PRIMER;
		}
		/*
		else
		{
			pid_info.pidBitmask |= PID_BITMASK_VIDEO_SUB;
		}
		*/

		pid_info.usVideoPid = pstPidInfo->usVideoPid;
		s_astDmxStatus[ulDemuxId].stPid.usVideoPid = pstPidInfo->usVideoPid;
		HxLOG_Print("video \n");
	}

	if(bMainAV == TRUE)
	{
		if(palDmx_VALID_PID(pstPidInfo->usMainAudioPid) == TRUE)
		{
			pid_info.pidBitmask |= PID_BITMASK_AUDIO_PRIMER;
			pid_info.usAudioMainPid = pstPidInfo->usMainAudioPid;
			s_astDmxStatus[ulDemuxId].stPid.usMainAudioPid = pstPidInfo->usMainAudioPid;
			HxLOG_Print("main audio \n");
		}

/*
		if(palDmx_VALID_PID(pstPidInfo->usSubAudioPid) == TRUE)
		{
			pid_info.pidBitmask |= PID_BITMASK_AUDIO_SUB;
			pid_info.usAudioSubPid = pstPidInfo->usSubAudioPid;
			s_astDmxStatus[ulDemuxId].stPid.usSubAudioPid = pstPidInfo->usSubAudioPid;
			HxLOG_Print("sub audio \n");
		}
*/
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

	HxLOG_FCC("primerIdx(%d) bitMask(0x%X) allBitMask(0x%X) avp(0x%X 0x%X 0x%X) ",
				stPrimerPidInfo.ulPrimerIndex,
				stPrimerPidInfo.ulPidBitmask, (PID_BITMASK_VIDEO_PRIMER | PID_BITMASK_AUDIO_PRIMER | PID_BITMASK_PCR_PRIMER),
				stPrimerPidInfo.usVideoPid,
				stPrimerPidInfo.usAudioPid,
				stPrimerPidInfo.usPcrPid);
	HxLOG_FCC(" >>>>>>> DI_DEMUX_StartPrimerPID ulDemuxId(%d) ulPrimerIndex(%d)	<<<<<<<<<<<<<< ", ulDemuxId, stPrimerPidInfo.ulPrimerIndex);

	nResult = DI_DEMUX_StartPrimerPID(ulDemuxId, &stPrimerPidInfo);
	if(nResult != ERR_OK)
	{
		pal_dmx_ResetPidInfo(ulDemuxId);
		pal_dmx_ResetFilterStatus(ulDemuxId);
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

	if(s_astDmxStatus[ulDemuxId].stStatus.bMainPcrFilter == TRUE)
	{
		filter_stop_mask |= PID_BITMASK_PCR_PRIMER;
	}

/* TODO: something..
	if(s_astDmxStatus[ulDemuxId].stStatus.bSubPcrFilter == TRUE)
	{
		filter_stop_mask |= PID_BITMASK_PCR_SUB;
	}
*/

	if(s_astDmxStatus[ulDemuxId].stStatus.bMainVideoFilter == TRUE)
	{
		filter_stop_mask |= PID_BITMASK_VIDEO_PRIMER;
	}

/* TODO: something..
	if(s_astDmxStatus[ulDemuxId].stStatus.bSubVideoFilter == TRUE)
	{
		filter_stop_mask |= PID_BITMASK_VIDEO_SUB;
	}
*/
	if(s_astDmxStatus[ulDemuxId].stStatus.bMainAudioFilter == TRUE)
	{
		filter_stop_mask |= PID_BITMASK_AUDIO_PRIMER;
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

	HxLOG_FCC("Stop primer pid :: dmxId(%d) primerIdx(%d) bitMask(0x%X) ", ulDemuxId, stPrimerPidInfo.ulPrimerIndex, stPrimerPidInfo.ulPidBitmask);

	nResult = DI_DEMUX_StopPrimerPID(ulDemuxId, &stPrimerPidInfo);
	if(nResult != ERR_OK)
	{
		HxLOG_Error(" \n");
		palDmx_LEAVE_CRITICAL;
		return ERR_FAIL;
	}

	pal_dmx_ResetStatus(ulDemuxId);

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

	HxSTD_memset(&pid_info, 0x00, sizeof(Pid_Info));
	palDmx_ENTER_CRITICAL;

	if(s_astDmxStatus[ulDemuxId].stPid.usPcrPid != pstPidInfo->usPcrPid)
	{
		if(s_astDmxStatus[ulDemuxId].stStatus.bMainPcrFilter == TRUE)
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

		if(palDmx_VALID_PID(pstPidInfo->usPcrPid) == TRUE)
		{
			if(ulDemuxId == s_bMainAvDemuxId)
			{
				pid_info.pidBitmask |= PID_BITMASK_PCR_PRIMER;
			}
			/* TODO:
			else
			{
				pid_info.pidBitmask |= PID_BITMASK_PCR_SUB;
			}
			*/

			pid_info.usPCRPid = pstPidInfo->usPcrPid;
		}
	}

	if(s_astDmxStatus[ulDemuxId].stPid.usVideoPid != pstPidInfo->usVideoPid)
	{
		if(s_astDmxStatus[ulDemuxId].stStatus.bMainVideoFilter == TRUE)
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

		if(palDmx_VALID_PID(pstPidInfo->usVideoPid) == TRUE)
		{
			if(ulDemuxId == s_bMainAvDemuxId)
			{
				pid_info.pidBitmask |= PID_BITMASK_VIDEO_PRIMER;
				pid_info.usVideoPid = pstPidInfo->usVideoPid;
			}
			/*
			else
			{
				pid_info.pidBitmask |= PID_BITMASK_VIDEO_SUB;
				pid_info.usVideoPid = pstPidInfo->usVideoPid;
			}
			*/
		}
	}

	if(ulDemuxId == s_bMainAvDemuxId)
	{
		if(s_astDmxStatus[ulDemuxId].stPid.usMainAudioPid != pstPidInfo->usMainAudioPid)
		{
			if(s_astDmxStatus[ulDemuxId].stStatus.bMainAudioFilter == TRUE)
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

/* TODO: ...
		if(s_astDmxStatus[ulDemuxId].stPid.usSubAudioPid != pstPidInfo->usSubAudioPid)
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
*/
	}

	if (filter_stop_mask)
	{
		HxSTD_MemSet(&stPrimerPidInfo, 0x00, sizeof(PrimerPid_Info));
		stPrimerPidInfo.ulPrimerIndex = ulPrimerIdx;
		stPrimerPidInfo.ulPidBitmask = filter_stop_mask;

		HxLOG_FCC("Stop primer pid :: dmxId(%d) primerIdx(%d) bitMask(0x%X) ", ulDemuxId, stPrimerPidInfo.ulPrimerIndex, stPrimerPidInfo.ulPidBitmask);

	    nResult = DI_DEMUX_StopPrimerPID(ulDemuxId, &stPrimerPidInfo);
	    if(nResult != ERR_OK)
	    {
		    HxLOG_Error("ERROR from DI_DEMUX_StopPID()..... CHECK PLEASE !!!!! \n");
		    palDmx_LEAVE_CRITICAL;
		    return ERR_FAIL;
	    }
	}

	if (filter_stop_mask & PID_BITMASK_PCR_PRIMER)
		s_astDmxStatus[ulDemuxId].stStatus.bMainPcrFilter = FALSE;

/*
	if (filter_stop_mask & PID_BITMASK_PCR_SUB)
		s_astDmxStatus[ulDemuxId].stStatus.bSubPcrFilter = FALSE;
*/
	if (filter_stop_mask & PID_BITMASK_VIDEO_PRIMER)
		s_astDmxStatus[ulDemuxId].stStatus.bMainVideoFilter = FALSE;

/*
	if (filter_stop_mask & PID_BITMASK_VIDEO_SUB)
		s_astDmxStatus[ulDemuxId].stStatus.bSubVideoFilter = FALSE;
*/
	if (filter_stop_mask & PID_BITMASK_AUDIO_PRIMER)
		s_astDmxStatus[ulDemuxId].stStatus.bMainAudioFilter = FALSE;

/*
	if (filter_stop_mask & PID_BITMASK_AUDIO_SUB)
		s_astDmxStatus[ulDemuxId].stStatus.bSubAudioFilter = FALSE;
*/
	filter_start_mask = pid_info.pidBitmask;

	if (filter_start_mask)
	{
		HxSTD_MemSet(&stPrimerPidInfo, 0x00, sizeof(PrimerPid_Info));
		stPrimerPidInfo.ulPrimerIndex 	= ulPrimerIdx;
		stPrimerPidInfo.ulPidBitmask 	= filter_start_mask;
		stPrimerPidInfo.usVideoPid 		= pid_info.usVideoPid;
		stPrimerPidInfo.usAudioPid 		= pid_info.usAudioMainPid;
		stPrimerPidInfo.usPcrPid 		= pid_info.usPCRPid;

		HxLOG_FCC("primerIdx(%d) bitMask(0x%X) allBitMask(0x%X) avp(0x%X 0x%X 0x%X) ",
						stPrimerPidInfo.ulPrimerIndex,
						stPrimerPidInfo.ulPidBitmask, (PID_BITMASK_VIDEO_PRIMER | PID_BITMASK_AUDIO_PRIMER | PID_BITMASK_PCR_PRIMER),
						stPrimerPidInfo.usVideoPid,
						stPrimerPidInfo.usAudioPid,
						stPrimerPidInfo.usPcrPid);

		HxLOG_FCC(" >>>>>>> DI_DEMUX_StartPrimerPID ulDemuxId(%d) ulPrimerIndex(%d)	<<<<<<<<<<<<<< ", ulDemuxId, stPrimerPidInfo.ulPrimerIndex);

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

	palDmx_LEAVE_CRITICAL;

	return ERR_OK;
}


#endif

/*********************** End of File ******************************/
