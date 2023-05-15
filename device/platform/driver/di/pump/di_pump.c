/**
 * di_pumb.c
*/

/**
 * @defgroup		DI_PVR : Playpumb module
 * @author		
 * @note			DI_PUMB APIs
 * @brief			Define DI_PUMB APIs
 * @file 			di_pb.c
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include "linden_trace.h"
#include "nexus_platform.h"

#include "nexus_playpump.h"
#include "nexus_security.h"
#include "nexus_timebase.h"

#include "drv_err.h"
#include "di_err.h"
#include "di_pump.h"
#include "di_media_types.h"

#include "drv_pvr.h"

#if defined(CONFIG_BRCM_MOD_MSDRM_PRDY_25_SUPPORT)
#include "sage_srai.h"
#endif
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define MAX_PUMP_INST_NUM 5

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
BKNI_EventHandle event = NULL;


typedef	struct	P_PUMP_INSTANCE
{
	HBOOL	bUsed;	
	HBOOL	bStarted;
	HUINT8	*pSecureBuffer;
	NEXUS_PlaypumpHandle playpump ;
	NEXUS_PidChannelHandle PidChannel;
	DI_PUMP_PID_TYPE_e PidType;
}P_PUMP_INSTANCE;

static P_PUMP_INSTANCE	g_astPUMPInst[MAX_PUMP_INST_NUM];

/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/
DRV_Error DRV_PUMP_Init(void)
{
	VK_MEM_Memset(g_astPUMPInst,0x00, sizeof(g_astPUMPInst));
	BKNI_CreateEvent(&event);
	return DRV_OK;
}

HBOOL DRV_PUMP_CheckVideoStart(void)
{
	HUINT32 ulIndex = 0;

	for(ulIndex = 0; ulIndex < MAX_PUMP_INST_NUM; ulIndex++)
	{
		if(g_astPUMPInst[ulIndex].PidType == DI_PUMP_PID_TYPE_VIDEO)
		{
			return g_astPUMPInst[ulIndex].bUsed == FALSE ? FALSE : TRUE;
		}
	}
	return FALSE;
}

HBOOL DRV_PUMP_CheckAudioStart(void)
{
	HUINT32 ulIndex = 0;

	for(ulIndex = 0; ulIndex < MAX_PUMP_INST_NUM; ulIndex++)
	{
		if((g_astPUMPInst[ulIndex].PidType == DI_PUMP_PID_TYPE_AUDIO)||(g_astPUMPInst[ulIndex].PidType == DI_PUMP_PID_TYPE_AUDIO_SUB) )
		{
			return g_astPUMPInst[ulIndex].bUsed == FALSE ? FALSE : TRUE;
		}
	}
	return FALSE;
}



NEXUS_PidChannelHandle DRV_PUMP_GetPlaypumpVideoPidChannel(DI_MEDIA_VIDEO_TYPE_e eVideoDecoderType)
{
	HUINT32 ulIndex = 0;

	for(ulIndex = 0; ulIndex < MAX_PUMP_INST_NUM; ulIndex++)
	{
		if(g_astPUMPInst[ulIndex].PidType == DI_PUMP_PID_TYPE_VIDEO)
		{
			return g_astPUMPInst[ulIndex].PidChannel;
		}
	}

	return NULL;
}

NEXUS_PidChannelHandle DRV_PUMP_GetPlaypumpAudioPidChannel(DI_MEDIA_AUDIO_TYPE_e eAudioDecoderType)
{
	HUINT32 ulIndex = 0;
	DI_PUMP_PID_TYPE_e TmpPidType;

	switch(eAudioDecoderType)
	{
		case DI_MEDIA_AUDIO_DECODER_MAIN:
		case DI_MEDIA_AUDIO_DECODER_SPDIF:
			TmpPidType = DI_PUMP_PID_TYPE_AUDIO;
			break;
		case DI_MEDIA_AUDIO_DECODER_SUB:
			TmpPidType = DI_PUMP_PID_TYPE_AUDIO_SUB;
			break;
		default:
			return NULL;
	}
	
	for(ulIndex = 0; ulIndex < MAX_PUMP_INST_NUM; ulIndex++)
	{
		if(g_astPUMPInst[ulIndex].PidType == TmpPidType)
		{
			return g_astPUMPInst[ulIndex].PidChannel;
		}
	}

	return NULL;
}

NEXUS_PidChannelHandle DRV_PUMP_GetPlaypumpVideoStcChannel()
{
	return DRV_DEMUX_GetStcChannelHandle();
}

NEXUS_PidChannelHandle DRV_PUMP_GetPlaypumpAudioStcChannel()
{
	return DRV_DEMUX_GetStcChannelHandle();
}


DI_ERR_CODE DI_PUMP_Open(HUINT32 ulPumpId,DI_PUMP_OPEN_t tOpen)
{
	NEXUS_Error rc;

	NEXUS_PlaypumpHandle Playpump = NULL;
	NEXUS_PlaypumpOpenSettings PlaypumpOpenSettings;
	NEXUS_PlaypumpSettings playpumpSettings;	
	NEXUS_PlaypumpOpenPidChannelSettings PidSettings;
	NEXUS_PidChannelHandle PidChannel = NULL;
	HUINT32 ulPID = 0x1FFF;
	HUINT8	*pSecureBuffer = NULL;

	if(tOpen.bSecurityPath == TRUE)
	{
		DRV_PVR_GetPlaypumpHandleByClass(&Playpump,BUFF_GROUP_4);
	}
	else
	{
		DRV_PVR_GetPlaypumpHandleByClass(&Playpump,BUFF_GROUP_1);
	}

	NEXUS_Playpump_GetSettings(Playpump, &playpumpSettings);

	switch(tOpen.eTransPortType)
	{
		case DI_PUMP_TRANSPORT_TYPE_ES:			
			playpumpSettings.transportType = NEXUS_TransportType_eEs;
			break;
		case DI_PUMP_TRANSPORT_TYPE_PES:			
			playpumpSettings.transportType = NEXUS_TransportType_eMpeg2Pes;
			break;
		case DI_PUMP_TRANSPORT_TYPE_TS:
			playpumpSettings.transportType = NEXUS_TransportType_eTs;
			break;
		default:
			/*error*/
			return DI_ERR_ERROR;
	}
	
	if(tOpen.eTransPortType == DI_PUMP_TRANSPORT_TYPE_PES)
	{
		switch(tOpen.ePidType)
		{
			case DI_PUMP_PID_TYPE_VIDEO:
				ulPID = 0xE0;
				break;
			case DI_PUMP_PID_TYPE_AUDIO:
			case DI_PUMP_PID_TYPE_AUDIO_SUB:
				if((tOpen.eCodecType == DI_PUMP_CODEC_TYPE_AC3)||(tOpen.eCodecType == DI_PUMP_CODEC_TYPE_AC3PLUS)) 
				{
					ulPID = 0xBD;
				}
				else
				{
					ulPID = 0xC0;
				}
				break;
			default:
				ulPID = tOpen.ulPID;
				break;
		}
	}
	else
	{
		ulPID = tOpen.ulPID;
	}

	rc = NEXUS_Playpump_SetSettings(Playpump, &playpumpSettings);
	if(rc != NEXUS_SUCCESS)
	{
		return DI_ERR_ERROR;
	}
	
    NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&PidSettings);
	if(tOpen.ePidType==DI_PUMP_PID_TYPE_VIDEO)
	{
	}

    PidChannel = NEXUS_Playpump_OpenPidChannel(Playpump, ulPID, &PidSettings);
	if(tOpen.bSecurityPath == TRUE)
	{
		rc = NEXUS_SetPidChannelBypassKeyslot(PidChannel, NEXUS_BypassKeySlot_eGR2R);
		if(rc != NEXUS_SUCCESS)
		{
			return DI_ERR_ERROR;
		}
	}	

	g_astPUMPInst[ulPumpId].bUsed = TRUE;
	g_astPUMPInst[ulPumpId].bStarted = FALSE;
	g_astPUMPInst[ulPumpId].playpump	= Playpump;
	g_astPUMPInst[ulPumpId].PidChannel = PidChannel;
	g_astPUMPInst[ulPumpId].pSecureBuffer = pSecureBuffer;
	g_astPUMPInst[ulPumpId].PidType = tOpen.ePidType;
	
	return DI_ERR_OK;
}

DI_ERR_CODE DI_PUMP_Close(HUINT32 ulPumpId)
{
	NEXUS_Error rc;

	rc = NEXUS_Playpump_ClosePidChannel(g_astPUMPInst[ulPumpId].playpump,g_astPUMPInst[ulPumpId].PidChannel);
	if(rc != NEXUS_SUCCESS)
	{
	
	}
	DRV_PVR_ReleasePlaypumpHandle(g_astPUMPInst[ulPumpId].playpump);

	g_astPUMPInst[ulPumpId].bUsed 		= FALSE;
	g_astPUMPInst[ulPumpId].playpump	= NULL;
	g_astPUMPInst[ulPumpId].PidChannel 	= NULL;	
	g_astPUMPInst[ulPumpId].PidType 	= DI_PUMP_PID_TYPE_MAX;
	if(g_astPUMPInst[ulPumpId].pSecureBuffer)
	{
#if defined(CONFIG_BRCM_MOD_MSDRM_PRDY_25_SUPPORT)		
		SRAI_Memory_Free(g_astPUMPInst[ulPumpId].pSecureBuffer);
#endif
		g_astPUMPInst[ulPumpId].pSecureBuffer = NULL;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_PUMP_Start(HUINT32 ulPumpId)
{	
	NEXUS_Error rc;

	if(g_astPUMPInst[ulPumpId].bUsed  == FALSE)
	{
		return DI_ERR_ERROR;
	}

	if(g_astPUMPInst[ulPumpId].bStarted  == TRUE)
	{
		return DI_ERR_ERROR;
	}

	rc = NEXUS_Playpump_Start(g_astPUMPInst[ulPumpId].playpump);
	if(rc != NEXUS_SUCCESS)
	{
		return DI_ERR_ERROR;
	}
	g_astPUMPInst[ulPumpId].bStarted = TRUE;

	return DI_ERR_OK;
}

DI_ERR_CODE DI_PUMP_Stop(HUINT32 ulPumpId)
{	
	if(g_astPUMPInst[ulPumpId].bUsed  == FALSE)
	{
		return DI_ERR_ERROR;
	}
	
	if(g_astPUMPInst[ulPumpId].bStarted  == FALSE)
	{
		return DI_ERR_ERROR;
	}

	NEXUS_Playpump_Stop(g_astPUMPInst[ulPumpId].playpump);
	g_astPUMPInst[ulPumpId].bStarted = FALSE;
	
	return DI_ERR_OK;
}


DI_ERR_CODE DI_PUMP_Flush(HUINT32 ulPumpId)
{	
	if(g_astPUMPInst[ulPumpId].bUsed  == FALSE)
	{
		return DI_ERR_ERROR;
	}
	
	if(g_astPUMPInst[ulPumpId].bStarted  == FALSE)
	{
		return DI_ERR_ERROR;
	}

	NEXUS_Playpump_Flush(g_astPUMPInst[ulPumpId].playpump);
	
	return DI_ERR_OK;
}

DI_ERR_CODE DI_PUMP_GetStatus(HUINT32 ulPumpId, DI_PUMP_Status *ptPumpStatus)
{
	NEXUS_PlaypumpStatus playpumpStatus;

	NEXUS_Playpump_GetStatus(g_astPUMPInst[ulPumpId].playpump, &playpumpStatus);

	ptPumpStatus->started = playpumpStatus.started;
	ptPumpStatus->fifoDepth = playpumpStatus.fifoDepth;
	ptPumpStatus->fifoSize = playpumpStatus.fifoSize;
	ptPumpStatus->descFifoDepth = playpumpStatus.descFifoDepth;
	ptPumpStatus->descFifoSize = playpumpStatus.descFifoSize;
	ptPumpStatus->bufferBase = playpumpStatus.bufferBase;
	ptPumpStatus->bytesPlayed = playpumpStatus.bytesPlayed;
	ptPumpStatus->index = playpumpStatus.index;
	ptPumpStatus->pacingTsRangeError = playpumpStatus.pacingTsRangeError;
	ptPumpStatus->syncErrors = playpumpStatus.syncErrors;
	ptPumpStatus->resyncEvents = playpumpStatus.resyncEvents;
	ptPumpStatus->streamErrors = playpumpStatus.streamErrors;
	ptPumpStatus->mediaPtsType = playpumpStatus.mediaPtsType;
	ptPumpStatus->mediaPts = playpumpStatus.mediaPts;

	return DI_ERR_OK;
}


DI_ERR_CODE DI_PUMP_GetBuffer(HUINT32 ulPumpId, void **ppBuffer, HUINT32 *pulSize)
{
	NEXUS_Error rc;

	if((g_astPUMPInst[ulPumpId].bUsed  == FALSE) || (g_astPUMPInst[ulPumpId].bStarted  == FALSE))
	{
		return DI_ERR_ERROR;
	}

	rc = NEXUS_Playpump_GetBuffer(g_astPUMPInst[ulPumpId].playpump,ppBuffer,pulSize);
	if(rc != NEXUS_SUCCESS)
	{
		return DI_ERR_ERROR;
	}
	
	return DI_ERR_OK;
}

DI_ERR_CODE DI_PUMP_WriteComplete(HUINT32 ulPumpId, HUINT32 ulSkip, HUINT32 ulSize)
{
	NEXUS_Error rc;

	if((g_astPUMPInst[ulPumpId].bUsed  == FALSE) || (g_astPUMPInst[ulPumpId].bStarted  == FALSE))
	{
		return DI_ERR_ERROR;
	}

	rc = NEXUS_Playpump_WriteComplete(g_astPUMPInst[ulPumpId].playpump, ulSkip, ulSize);
	if(rc != NEXUS_SUCCESS)
	{
		return DI_ERR_ERROR;
	}
	return DI_ERR_OK;
}

DI_ERR_CODE DI_PUMP_SetSync()
{
	NEXUS_StcChannelSettings stcSettings;
	NEXUS_Error nResult;

	NEXUS_StcChannel_GetDefaultSettings(DRV_DEMUX_GetStcChannelHandle(), &stcSettings);

	stcSettings.mode = NEXUS_StcChannelMode_eAuto;
	stcSettings.modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eVideoMaster;

	nResult = NEXUS_StcChannel_SetSettings(DRV_DEMUX_GetStcChannelHandle(), &stcSettings);
	if(nResult != 0)
	{
		//PrintError(" failed NEXUS_StcChannel_SetSettings.\n");
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}



