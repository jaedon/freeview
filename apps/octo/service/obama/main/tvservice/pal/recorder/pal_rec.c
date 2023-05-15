/**
	@file    	pal_rec.c
	@brief   	adaptation for recorder

	Description:	implements recording feature.	\n
	Module: 		pal/pvr			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <vkernel.h>

#include <di_err.h>
#include <di_pvr.h>
#include <di_demux.h>

#include <pal_rec.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	unsigned long			 ullSemId;				// 추후 문제되면 Semaphore를 사용하도록 한다.

	PalRec_Callback_t	 pfCallback;			// M/W로 올리기 위한 Callback
	PalRec_MemCallback_t	 pfMemCbFunc;			// Rec2Mem 의 경우를 위한 Callback
} palRec_Context_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HUINT32					 s_ulPalRec_NumRecord = 0;
STATIC palRec_Context_t		*s_pstPalRec_Context = NULL;
STATIC HUINT32					 s_ulDelayedRecInstanceId = 0;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
STATIC palRec_Context_t *pal_rec_getContext(HUINT32 ulDeviceId)
{
	if ((ulDeviceId < s_ulPalRec_NumRecord) && (NULL != s_pstPalRec_Context))
	{
		return &(s_pstPalRec_Context[ulDeviceId]);
	}

	return NULL;
}

STATIC void pal_rec_HandleEvent(void *pMsg)
{
	PalFs_Msg_t  			*pstDiMsg = NULL;
	palRec_Context_t		*pstContext = NULL;

	pstDiMsg = (PalFs_Msg_t *)pMsg;
	if( pstDiMsg == NULL )
	{
		HxLOG_Print("[pal_rec_HandleEvent] PAL_PVR_MSG is NULL\n");
		return;
	}

	if (pstDiMsg->ulEvent == DI_PVR_EVENT_DELAYED_RECORD_STATUS_CHANGED)
	{
		// 여기서는 pstDiMsg->ulInstanceId 값이 ulDeviceId가 아니다. 별도로 저장해뒀다가 사용
		pstContext = pal_rec_getContext (s_ulDelayedRecInstanceId);
	}
	else
	{
		pstContext = pal_rec_getContext (pstDiMsg->ulInstanceId);
	}

	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context\n");
		return;
	}

	if (NULL == pstContext->pfCallback)
		return;

	switch( pstDiMsg->ulEvent )
	{
	case DI_PVR_EVENT_TIMECODE_CHANGED:
		HxLOG_Info("[%s:%d] DI_PVR_EVENT_TIMECODE_CHANGED : ulInstanceId(%d), ulParam1:%d, ulParam2:%d\n\r",
					__FUNCTION__, __LINE__, pstDiMsg->ulInstanceId, pstDiMsg->ulParam1, pstDiMsg->ulParam2);
		pstContext->pfCallback (ePAL_REC_EVENT_TIMECODE_CHANGED, pstDiMsg->ulInstanceId, pstDiMsg->ulParam1, pstDiMsg->ulParam2 , pstDiMsg->ulParam3);
		break;

	case DI_PVR_EVENT_DISK_FULL:
		HxLOG_Info("DI_PVR_EVENT_DISK_FULL : ulInstanceId(%d)\r\n", pstDiMsg->ulInstanceId);
		pstContext->pfCallback (ePAL_REC_EVENT_DISKFULL, pstDiMsg->ulInstanceId, pstDiMsg->ulParam1, pstDiMsg->ulParam2, pstDiMsg->ulParam3);
		break;

	case DI_PVR_EVENT_STREAM_UNSCRAMBLED:
		HxLOG_Info("DI_PVR_EVENT_STREAM_UNSCRAMBLED : ulInstanceId(%d)\r\n", pstDiMsg->ulInstanceId);
		pstContext->pfCallback (ePAL_REC_EVENT_STREAM_UNSCRAMBLED, pstDiMsg->ulInstanceId, pstDiMsg->ulParam1, pstDiMsg->ulParam2, pstDiMsg->ulParam3);
		break;

	case DI_PVR_EVENT_STREAM_SCRAMBLED:
		HxLOG_Info("DI_PVR_EVENT_STREAM_SCRAMBLED : ulInstanceId(%d)\r\n", pstDiMsg->ulInstanceId);
		pstContext->pfCallback (ePAL_REC_EVENT_STREAM_SCRAMBLED, pstDiMsg->ulInstanceId, pstDiMsg->ulParam1, pstDiMsg->ulParam2, pstDiMsg->ulParam3);
		break;

	case DI_PVR_EVENT_DELAYED_RECORD_STATUS_CHANGED:
		HxLOG_Info("DI_PVR_EVENT_DELAYED_RECORD_STATUS_CHANGED : ulInstanceId(%d)\r\n", pstDiMsg->ulInstanceId);
		pstContext->pfCallback (ePAL_REC_EVENT_DELAYED_RECORD_STATUS_CHANGED, pstDiMsg->ulInstanceId, pstDiMsg->ulParam1, pstDiMsg->ulParam2, pstDiMsg->ulParam3);
		break;

	default:
		HxLOG_Print("[%s:%d] Unknown event : 0x%X\n", __FUNCTION__, __LINE__, pstDiMsg->ulEvent);
		break;
	}
}

STATIC void pal_rec_HandleRecMem (int deviceId, const void *buf, unsigned int length)
{
	palRec_Context_t		*pstContext = pal_rec_getContext (deviceId);

	if ((NULL == pstContext) || (NULL == pstContext->pfMemCbFunc))
	{
		HxLOG_Error ("No Context or No Callback\n");
		return;
	}

	if ((NULL == buf) || (0 == length))
	{
		HxLOG_Error ("wring buffer or size\n");
		return;
	}

	pstContext->pfMemCbFunc ((HUINT32)deviceId, (HUINT8 *)buf, (HUINT32)length);
}

HUINT32			PAL_REC_Init(void)
{
	HUINT32					 i, ulNumRecorder = 0;
	palRec_Context_t		*pstContextArray = NULL;
	DI_ERR_CODE				 eDiErr;

	/* init recorder */
	eDiErr = DI_PVR_REC_GetCapability(&ulNumRecorder);
	if ((DI_ERR_OK != eDiErr) || (0 == ulNumRecorder))
	{
		HxLOG_Error ("DI_PVR_PLAY_GetCapability err or Player=0\n");
		return ERR_FAIL;
	}

	pstContextArray = (palRec_Context_t *)OxMW_Calloc (sizeof(palRec_Context_t) * ulNumRecorder);
	if (NULL == pstContextArray)
	{
		HxLOG_Error ("Context Allocation failed\n");
		return ERR_FAIL;
	}

	s_ulPalRec_NumRecord = ulNumRecorder;
	s_pstPalRec_Context = pstContextArray;

	for (i = 0; i < ulNumRecorder; i++)
	{
		eDiErr = DI_PVR_REC_RegisterEventCallback (i, DI_PVR_EVENT_TIMECODE_CHANGED, pal_rec_HandleEvent);
		eDiErr |= DI_PVR_REC_RegisterEventCallback (i, DI_PVR_EVENT_STREAM_UNSCRAMBLED, pal_rec_HandleEvent);
		eDiErr |= DI_PVR_REC_RegisterEventCallback (i, DI_PVR_EVENT_STREAM_SCRAMBLED, pal_rec_HandleEvent);
		eDiErr |= DI_PVR_REC_RegisterEventCallback (i, DI_PVR_EVENT_DISK_FULL, pal_rec_HandleEvent);
#if defined(CONFIG_MW_MM_DELAYED_REC)
		eDiErr |= DI_PVR_REC_RegisterEventCallback (i, DI_PVR_EVENT_DELAYED_RECORD_STATUS_CHANGED, pal_rec_HandleEvent);
#endif
		if (eDiErr != DI_ERR_OK)
		{
			HxLOG_Error ("fail to register recorder callback\n");
			return ERR_FAIL;
		}

		eDiErr = DI_PVR_REC_RegisterMemIOCallback (i, pal_rec_HandleRecMem);
		if (DI_ERR_OK != eDiErr)
		{
			HxLOG_Error ("DI_PVR_REC_RegisterMemIOCallback failed:\n");
		}
	}

	return ERR_OK;
}

void PAL_REC_RegisterMemoryCallback (HUINT32 ulDeviceId, PalRec_MemCallback_t pfMemCb)
{
	palRec_Context_t		*pstContext = pal_rec_getContext (ulDeviceId);

	if ((NULL == pstContext) || (NULL == pfMemCb))
	{
		HxLOG_Error ("DeviceId Error or Callback NULL\n");
		return;
	}

	pstContext->pfMemCbFunc= pfMemCb;
}

void PAL_REC_UnregisterMemoryCallback (HUINT32 ulDeviceId, PalRec_MemCallback_t pfMemCb)
{
	palRec_Context_t		*pstContext = pal_rec_getContext(ulDeviceId);

	if ((NULL == pstContext) || (pfMemCb != pstContext->pfMemCbFunc))
	{
		HxLOG_Error ("DeviceId Error or Callback not same\n");
		return;
	}

	pstContext->pfMemCbFunc = NULL;
}

void PAL_REC_RegisterNotifier (HUINT32 ulDeviceId, PalRec_Callback_t pfRecCallback)
{
	palRec_Context_t		*pstContext = pal_rec_getContext (ulDeviceId);

	if ((NULL == pstContext) || (NULL == pfRecCallback))
	{
		HxLOG_Error ("DeviceId Error or Callback NULL\n");
		return;
	}

	pstContext->pfCallback = pfRecCallback;
}

void PAL_REC_UnregisterNotifier (HUINT32 ulDeviceId, PalRec_Callback_t pfRecCallback)
{
	palRec_Context_t		*pstContext = pal_rec_getContext(ulDeviceId);

	if ((NULL == pstContext) || (pfRecCallback != pstContext->pfCallback))
	{
		HxLOG_Error ("DeviceId Error or Callback not same\n");
		return;
	}

	pstContext->pfCallback = NULL;
}

HUINT32		PAL_REC_GetCapability(HUINT32 *pulNumOfDeviceId)
{
	HUINT32 ulRet = ERR_OK;

	ulRet = DI_PVR_REC_GetCapability( pulNumOfDeviceId );

	return ulRet;
}

HERROR		PAL_REC_GetType(HUINT32 ulDeviceId, PalRec_DeviceType_e *pRecType)
{
	DI_PVR_INST_TYPE eInstType;

	if(ulDeviceId >= s_ulPalRec_NumRecord)
		return ERR_FAIL;

	if(pRecType == NULL)
		return ERR_FAIL;

	(void)DI_PVR_REC_GetDeviceType (ulDeviceId, &eInstType);

	switch (eInstType)
	{
		case DI_PVR_INST_TYPE_NONE:
			*pRecType = eREC_Type_Unknown;
			break;
		case DI_PVR_INST_TYPE_REC:
			*pRecType = eREC_Type_Rec;
			break;
		case DI_PVR_INST_TYPE_TRANS:
			*pRecType = eREC_Type_Trans;
			break;
		case DI_PVR_INST_TYPE_SATIP:
			*pRecType = eREC_Type_SatIp;
			break;
	}
	return ERR_OK;
}

HUINT32	 	PAL_REC_Setup (HUINT32 ulDeviceId, PalRec_Setup_t *pstSetup )
{
	HUINT32 ulRet = ERR_OK;
	DI_PVR_REC_SETUP_t	stDiSetup;

	if( ulDeviceId > s_ulPalRec_NumRecord )
	{
		HxLOG_Print("[Setup] DeviceId Error, ulDeviceId:%d, MAX:%d\n", ulDeviceId, s_ulPalRec_NumRecord);
		return ERR_FAIL;
	}

	HxSTD_memset(&stDiSetup,0,sizeof(DI_PVR_REC_SETUP_t));

	stDiSetup.ulDemuxId = pstSetup->ulDemuxId;

	switch (pstSetup->eFileType)
	{
		case ePAL_FS_FILETYPE_LINEAR:
			stDiSetup.eFileType = eDI_PVR_FileType_Linear;
			break;
		case ePAL_FS_FILETYPE_CIRCULAR:
			stDiSetup.eFileType = eDI_PVR_FileType_Circular;
			break;
		default:
			HxLOG_Print("[%s:%d] FileType Error:0x%X\n", __FUNCTION__,__LINE__,stDiSetup.eFileType);
			return ERR_FAIL;
			break;
	}
	switch (pstSetup->eStreamType)
	{
		case eSTREAMTYPE_VIDEO:
			stDiSetup.eStreamType = eDI_PVR_Stream_Video;
			switch (pstSetup->eVideoType)
			{
				case eDxVIDEO_CODEC_MPEG1:
					stDiSetup.eVideoType = eDI_PVR_VideoType_Mpeg1;
					break;
				case eDxVIDEO_CODEC_MPEG2:
					stDiSetup.eVideoType = eDI_PVR_VideoType_Mpeg2;
					break;
				case eDxVIDEO_CODEC_MPEG4_PART2:
					stDiSetup.eVideoType = eDI_PVR_VideoType_Mpeg4_Part2;
					break;

				case eDxVIDEO_CODEC_H263:
					stDiSetup.eVideoType = eDI_PVR_VideoType_H263;
					break;

				case eDxVIDEO_CODEC_H264:
				case eDxVIDEO_CODEC_H264_SVC:
				case eDxVIDEO_CODEC_H264_MVC:
					stDiSetup.eVideoType = eDI_PVR_VideoType_H264;
					break;

				case eDxVIDEO_CODEC_H265:
					stDiSetup.eVideoType = eDI_PVR_VideoType_H265;
					break;

				case eDxVIDEO_CODEC_VC1:
					stDiSetup.eVideoType = eDI_PVR_VideoType_Vc1;
					break;

				case eDxVIDEO_CODEC_VC1_SIMPLEMAIN:
					stDiSetup.eVideoType = eDI_PVR_VideoType_Vc1_Sm;
					break;

				case eDxVIDEO_CODEC_AVS:
					stDiSetup.eVideoType = eDI_PVR_VideoType_Avs;
					break;

				case eDxVIDEO_CODEC_DIVX311:
					stDiSetup.eVideoType = eDI_PVR_VideoType_Divx_311;
					break;

				case eDxVIDEO_CODEC_RV40:
				case eDxVIDEO_CODEC_VP6:
				default:
					HxLOG_Print("[%s:%d] Video Codec Error:0x%X\n", __FUNCTION__,__LINE__, stDiSetup.eVideoType);
					return ERR_FAIL;
					break;
			}
			break;
		case eSTREAMTYPE_AUDIO:
			stDiSetup.eStreamType = eDI_PVR_Stream_Audio;
			stDiSetup.eVideoType = eDI_PVR_VideoType_None;
			break;
		default:
			HxLOG_Print("[%s:%d] Stream Type Error:0x%X\n", __FUNCTION__,__LINE__,stDiSetup.eStreamType);
			return ERR_FAIL;
			break;
	}

	VK_strncpy(stDiSetup.aucFileName,pstSetup->aucFileName,(DI_PVR_MAX_NAME_LEN-1));

	HxLOG_Print("[%s] Encryption type for Record [%d]\n", __FUNCTION__, pstSetup->stEncryptSettings.eEncType);

	// TODO: We have to check difference of DI & PAL's structure!!!
	switch (pstSetup->stEncryptSettings.eEncType)
	{
		case ePAL_PVR_ENCRYPTION_TYPE_3DES:
			stDiSetup.stEncryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_3DES;
			break;
		case ePAL_PVR_ENCRYPTION_TYPE_AES:
			stDiSetup.stEncryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_AES;
			break;
		case ePAL_PVR_ENCRYPTION_TYPE_CURRENT:
			stDiSetup.stEncryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_CURRENT;
			break;
		case ePAL_PVR_ENCRYPTION_TYPE_NONE:
			stDiSetup.stEncryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_NONE;
			break;

		default:
			HxLOG_Print("[%s:%d] Encryption Type Error:0x%X\n", __FUNCTION__,__LINE__,
						pstSetup->stEncryptSettings.eEncType);
			return ERR_FAIL;
			break;
	}

	HxSTD_memcpy (stDiSetup.stEncryptSettings.ucKey, pstSetup->stEncryptSettings.aucKey, DI_PVR_CRYPTO_KEY_LEN);
	HxSTD_memcpy (stDiSetup.stEncryptSettings.ucIv, pstSetup->stEncryptSettings.aucIv, DI_PVR_CRYPTO_KEY_LEN);
	HxSTD_memcpy (stDiSetup.stEncryptSettings.ucProtectionKey, pstSetup->stEncryptSettings.aucProtectionKey, DI_PVR_CRYPTO_KEY_LEN);


	if(pstSetup->bTimestamp == FALSE)
		stDiSetup.ulPacketSize = PACKET_SIZE_TS;
	else stDiSetup.ulPacketSize = PACKET_SIZE_PVR;

	stDiSetup.bIsAllPidChannel = pstSetup->bIsAllPidChannel;


	/*
	 * note :
	 		Recording Target이 USB일 경우 Hard Limit Size를 추가 전달함.
	 */
	{
		stDiSetup.stUsbFlash.bIsUsbFlashRecord = pstSetup->stUsbFlash.bIsUsbFlashRecord;/* Set TRUE if recording content into USB Flash */
		stDiSetup.stUsbFlash.ulLimitTimeSecond = pstSetup->stUsbFlash.ulLimitTimeSecond;/* Time limitation for TSR */
		stDiSetup.stUsbFlash.ullLimitDataSize  = pstSetup->stUsbFlash.ullLimitDataSize;	/* Data limitation for TSR */
		stDiSetup.stUsbFlash.ullLimitIndexSize = pstSetup->stUsbFlash.ullLimitIndexSize;/* Index limitation for TSR */

		HxLOG_Debug ("[PAL_REC] stDiSetup.stUsbFlash.bIsUsbFlashRecord : %d\n",	stDiSetup.stUsbFlash.bIsUsbFlashRecord);
		HxLOG_Debug ("[PAL_REC] stDiSetup.stUsbFlash.ulLimitTimeSecond : %d\n",	stDiSetup.stUsbFlash.ulLimitTimeSecond);
		HxLOG_Debug ("[PAL_REC] stDiSetup.stUsbFlash.ullLimitDataSize  : %llu\n",	stDiSetup.stUsbFlash.ullLimitDataSize);
		HxLOG_Debug ("[PAL_REC] stDiSetup.stUsbFlash.ullLimitIndexSize : %llu\n",	stDiSetup.stUsbFlash.ullLimitIndexSize);
	}

	ulRet = DI_PVR_REC_Setup( ulDeviceId, &stDiSetup );

	return ulRet;
}

HUINT32 		PAL_REC_Start(HUINT32 ulDeviceId )
{
	HUINT32 ulRet = ERR_OK;

	if( ulDeviceId > s_ulPalRec_NumRecord )
	{
		HxLOG_Error("[Start] DeviceId Error, ulDeviceId:%d, MAX:%d\n", ulDeviceId, s_ulPalRec_NumRecord);
		return ERR_FAIL;
	}

	ulRet = DI_PVR_REC_Start( ulDeviceId );

	return ulRet;
}

HUINT32	 	PAL_REC_StartDelayed(HUINT32 ulDeviceId, PalRec_DelayedSetup_t *pstSetup )
{
	HUINT32 ulRet = ERR_OK;
#if defined(CONFIG_MW_MM_DELAYED_REC)
	DI_PVR_REC_DELAYRECORD_SETUP_t	stDIStruct;
#endif

#if defined(CONFIG_MW_MM_DELAYED_REC)
	if( ulDeviceId > s_ulPalRec_NumRecord )
	{
		HxLOG_Print("[StartDelayed] DeviceId Error, ulDeviceId:%d, MAX:%d\n", ulDeviceId, s_ulPalRec_NumRecord);
		return ERR_FAIL;
	}

	HxSTD_memset(&stDIStruct,0,sizeof(DI_PVR_REC_DELAYRECORD_SETUP_t));

	VK_strncpy(stDIStruct.aucFileName,pstSetup->aucFileName,(DI_PVR_MAX_NAME_LEN-1));
	VK_strncpy(stDIStruct.aucTempFileName,pstSetup->aucTempFileName,(DI_PVR_MAX_NAME_LEN-1));

	stDIStruct.ulStartTime		= pstSetup->ulStartTime;
	stDIStruct.ulRequestId		= pstSetup->ulInstanceId;

	s_ulDelayedRecInstanceId	= ulDeviceId;

	HxLOG_Print("[%s] Encryption type for Record [%d]\n", __FUNCTION__, pstSetup->stEncryptSettings.eEncType);

	// TODO: We have to check difference of DI & PAL's structure!!!
	switch (pstSetup->stEncryptSettings.eEncType)
	{
		case ePAL_PVR_ENCRYPTION_TYPE_3DES:
			stDIStruct.stEncryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_3DES;
			break;
		case ePAL_PVR_ENCRYPTION_TYPE_AES:
			stDIStruct.stEncryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_AES;
			break;
		case ePAL_PVR_ENCRYPTION_TYPE_CURRENT:
			stDIStruct.stEncryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_CURRENT;
			break;
		case ePAL_PVR_ENCRYPTION_TYPE_NONE:
			stDIStruct.stEncryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_NONE;
			break;

		default:
			HxLOG_Print("[%s:%d] Encryption Type Error:0x%X\n", __FUNCTION__,__LINE__,
						pstSetup->stEncryptSettings.eEncType);
			return ERR_FAIL;
			break;
	}
	HxSTD_memcpy (stDIStruct.stEncryptSettings.ucKey, pstSetup->stEncryptSettings.aucKey, DI_PVR_CRYPTO_KEY_LEN);
	HxSTD_memcpy (stDIStruct.stEncryptSettings.ucIv, pstSetup->stEncryptSettings.aucIv, DI_PVR_CRYPTO_KEY_LEN);
	HxSTD_memcpy (stDIStruct.stEncryptSettings.ucProtectionKey, pstSetup->stEncryptSettings.aucProtectionKey, DI_PVR_CRYPTO_KEY_LEN);

	ulRet = DI_PVR_REC_SetDelayRecording( ulDeviceId, &stDIStruct );
#endif

	return ulRet;
}

HUINT32	 	PAL_REC_StartDelayedSplit( PalRec_DelayedSplitSetup_t *pstSetup )
{
	HUINT32 ulRet = ERR_OK;
#if defined(CONFIG_MW_MM_DELAYED_REC)
	DI_PVR_REC_DELAYSPLIT_SETUP_t	stDiSetup;

	HxSTD_memset(&stDiSetup,0,sizeof(DI_PVR_REC_DELAYSPLIT_SETUP_t));

	VK_strncpy(stDiSetup.aucFileName,pstSetup->aucFileName,(DI_PVR_MAX_NAME_LEN-1));
	VK_strncpy(stDiSetup.aucTempFileName,pstSetup->aucTempFileName,(DI_PVR_MAX_NAME_LEN-1));

	stDiSetup.ulStartTime		= pstSetup->ulStartTime;
	stDiSetup.ulEndTime			= pstSetup->ulEndTime;
	stDiSetup.ulRequestId		= pstSetup->ulInstanceId;

	HxLOG_Print("[%s] Encryption type for Record [%d]\n", __FUNCTION__, pstSetup->stEncryptSettings.eEncType);

	// TODO: We have to check difference of DI & PAL's structure!!!
	switch (pstSetup->stEncryptSettings.eEncType)
	{
		case ePAL_PVR_ENCRYPTION_TYPE_3DES:
			stDiSetup.stEncryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_3DES;
			break;
		case ePAL_PVR_ENCRYPTION_TYPE_AES:
			stDiSetup.stEncryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_AES;
			break;
		case ePAL_PVR_ENCRYPTION_TYPE_CURRENT:
			stDiSetup.stEncryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_CURRENT;
			break;
		case ePAL_PVR_ENCRYPTION_TYPE_NONE:
			stDiSetup.stEncryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_NONE;
			break;

		default:
			HxLOG_Print("[%s:%d] Encryption Type Error:0x%X\n", __FUNCTION__,__LINE__,
						pstSetup->stEncryptSettings.eEncType);
			return ERR_FAIL;
			break;
	}

	HxSTD_memcpy (stDiSetup.stEncryptSettings.ucKey, pstSetup->stEncryptSettings.aucKey, DI_PVR_CRYPTO_KEY_LEN);
	HxSTD_memcpy (stDiSetup.stEncryptSettings.ucIv, pstSetup->stEncryptSettings.aucIv, DI_PVR_CRYPTO_KEY_LEN);
	HxSTD_memcpy (stDiSetup.stEncryptSettings.ucProtectionKey, pstSetup->stEncryptSettings.aucProtectionKey, DI_PVR_CRYPTO_KEY_LEN);

	ulRet = DI_PVR_REC_StartDelaySplit( (DI_PVR_REC_DELAYSPLIT_SETUP_t *)pstSetup );
#endif

	return ulRet;
}


HUINT32		PAL_REC_GetDelayRecordStatus( HUINT32 ulRequestId, HUINT32 *pulStatus )
{
	HUINT32	 ulRet = ERR_OK;

#if defined(CONFIG_MW_MM_DELAYED_REC)
	if( pulStatus == NULL ) return ERR_FAIL;

	ulRet = DI_PVR_REC_GetDelayRecordingStatus( ulRequestId, pulStatus );

#endif
	return ulRet;
}


HUINT32 		PAL_REC_Stop( HUINT32 ulDeviceId )
{
	HUINT32 ulRet = ERR_OK;

	if( ulDeviceId > s_ulPalRec_NumRecord )
	{
		HxLOG_Print("[Stop] DeviceId Error, ulDeviceId:%d, MAX:%d\n", ulDeviceId, s_ulPalRec_NumRecord);
		return ERR_FAIL;
	}

	ulRet = DI_PVR_REC_Stop( ulDeviceId );

	return ulRet;
}


HUINT32 		PAL_REC_Pause( HUINT32 ulDeviceId )
{
	HUINT32 ulRet = ERR_OK;

	if( ulDeviceId > s_ulPalRec_NumRecord )
	{
		HxLOG_Error("[Pause] DeviceId Error, ulDeviceId:%d, MAX:%d\n", ulDeviceId, s_ulPalRec_NumRecord);
		return ERR_FAIL;
	}

	ulRet = DI_PVR_REC_Pause( ulDeviceId );

	return ulRet;
}


HUINT32 		PAL_REC_Resume(HUINT32 ulDeviceId )
{
	HUINT32 ulRet = ERR_OK;

	if( ulDeviceId > s_ulPalRec_NumRecord )
	{
		HxLOG_Error("[Resume] DeviceId Error, ulDeviceId:%d, MAX:%d\n", ulDeviceId, s_ulPalRec_NumRecord);
		return ERR_FAIL;
	}

	ulRet = DI_PVR_REC_Resume( ulDeviceId );

	return ulRet;
}


HUINT32 		PAL_REC_GetDuration(HUINT32 ulDeviceId, HUINT32 *pulTime )
{
	HUINT32 ulRet = ERR_OK;

	if( ulDeviceId > s_ulPalRec_NumRecord )
	{
		HxLOG_Print("[Pause] DeviceId Error, ulDeviceId:%d, MAX:%d\n", ulDeviceId, s_ulPalRec_NumRecord);
		return ERR_FAIL;
	}

	ulRet = DI_PVR_REC_GetDuration( ulDeviceId, pulTime );

	return ulRet;
}

HUINT32 		PAL_REC_StartPid(HUINT32 ulDeviceId, HUINT16 usPid, EsType_t eEsType, HUINT32 *pulRequestId )
{
	HUINT32 ulRet = ERR_OK;
	DI_PVR_REC_PID_PES_TYPE	eDiPvrPesType;

	if( pulRequestId == NULL )
		return ERR_FAIL;

	if( ulDeviceId > s_ulPalRec_NumRecord )
	{
//		HxLOG_Print("[StartPid] DeviceId Error, ulDeviceId:%d, MAX:%d\n", ulDeviceId, s_ulPvrNumRecor);
		return ERR_FAIL;
	}

	if( usPid == PID_NULL )
	{
		HxLOG_Print("[StartPid] PID is NULL, DeviceId:%d\n", ulDeviceId);
		return ERR_FAIL;
	}
	switch (eEsType)
	{
		case eEsType_None:
			eDiPvrPesType = DI_PVR_REC_PID_PES_NONE;
			break;

		case eEsType_Video:
			eDiPvrPesType = DI_PVR_REC_PID_PES_TYPE_VIDEO;
			break;

		case eEsType_Audio:
			eDiPvrPesType = DI_PVR_REC_PID_PES_TYPE_AUDIO;
			break;

		case eEsType_PCR:
			eDiPvrPesType = DI_PVR_REC_PID_PES_TYPE_PCR;
			break;

		case eEsType_Subtitle:
			eDiPvrPesType = DI_PVR_REC_PID_PES_TYPE_SUBTITLE;
			break;

		case eEsType_Teletext:
			eDiPvrPesType = DI_PVR_REC_PID_PES_TYPE_TELETEXT;
			break;

		case eEsType_ClosedCaption:	/* 기존 Octo1.0에서 NONE으로 처리 했다. */
			eDiPvrPesType = DI_PVR_REC_PID_PES_NONE;
			break;

		case eEsType_All:
			eDiPvrPesType = DI_PVR_REC_PID_PES_TYPE_ALL;
			usPid = 0;
			break;

		default:
			HxLOG_Print("[%s:%d] Invalid ES Type:0x%X\n", __FUNCTION__,__LINE__,eEsType);
			return ERR_FAIL;
			break;
	}

	ulRet = DI_PVR_REC_AddPidChannel( ulDeviceId, usPid, eDiPvrPesType, pulRequestId);
	if( ulRet != ERR_OK )
	{
		HxLOG_Print("[StartPid] DI_PVR_REC_AddPidChannel, DeviceId:%d, ulRet:0x%X\n", ulDeviceId, ulRet);
		return ERR_FAIL;
	}

	HxLOG_Print("[%s:%d] ADD OK - ulDeviceId(%d), usPid(%d), eEsType(%d), *pulRequestId(%d)\n", __FUNCTION__, __LINE__,
		ulDeviceId, usPid, eEsType, *pulRequestId);

	return ulRet;
}


HUINT32 		PAL_REC_StopPid(HUINT32 ulDeviceId, HUINT32 ulRequestId )
{
	HUINT32 ulRet = ERR_OK;

	if( ulRequestId == 0xFFFF )
		return ERR_FAIL;

	if( ulDeviceId > s_ulPalRec_NumRecord )
	{
		HxLOG_Print("[StopPid] DeviceId Error, ulDeviceId:%d, MAX:%d\n", ulDeviceId, s_ulPalRec_NumRecord);
		return ERR_FAIL;
	}


	ulRet = DI_PVR_REC_RemovePidChannel( ulDeviceId, ulRequestId );
	if( ulRet != ERR_OK )
	{
		HxLOG_Print("[StopPid] DI_PVR_REC_RemovePidChannel, DeviceId:%d, ulRet:0x%X\n", ulDeviceId, ulRet);
		return ERR_FAIL;
	}

	HxLOG_Print("[%s:%d] REMOVE OK - ulDeviceId(%d), ulRequestId(%d)\n", __FUNCTION__, __LINE__,
		ulDeviceId, ulRequestId);

	return ulRet;
}

HUINT32		PAL_REC_SetEncryption( HUINT32 ulDeviceId, PalCrypt_EncryptionType_e eEncType, HUINT8 *pucKey, HUINT8 *pucIv )
{
	HUINT32 ulRet = ERR_OK;
	DI_PVR_CRYPTO_SETTINGS encryptSettings;

	if( ulDeviceId > s_ulPalRec_NumRecord )
	{
		HxLOG_Print("[SetEncryption] DeviceId Error, ulDeviceId:%d, MAX:%d\n", ulDeviceId, s_ulPalRec_NumRecord);
		return ERR_FAIL;
	}

	HxSTD_memset(&encryptSettings,0, sizeof(DI_PVR_CRYPTO_SETTINGS));

	HxLOG_Print("[%s] Encryption type for Record [%d]\n", __FUNCTION__, eEncType);

	// TODO: We have to check difference of DI & PAL's structure!!!
	switch (eEncType)
	{
		case ePAL_PVR_ENCRYPTION_TYPE_3DES:
			encryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_3DES;
			break;
		case ePAL_PVR_ENCRYPTION_TYPE_AES:
			encryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_AES;
			break;
		case ePAL_PVR_ENCRYPTION_TYPE_CURRENT:
			encryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_CURRENT;
			break;
		case ePAL_PVR_ENCRYPTION_TYPE_NONE:
			encryptSettings.eEncType = DI_PVR_ENCRYPTION_TYPE_NONE;
			break;

		default:
			HxLOG_Print("[%s:%d] Encryption Type Error:0x%X\n", __FUNCTION__,__LINE__,	eEncType);
			return ERR_FAIL;
			break;
	}
	if(pucKey != NULL)
		HxSTD_memcpy(encryptSettings.ucKey,pucKey,DI_PVR_CRYPTO_KEY_LEN);
	if(pucIv != NULL)
		HxSTD_memcpy(encryptSettings.ucIv,pucIv,DI_PVR_CRYPTO_KEY_LEN);

	ulRet = DI_PVR_REC_SetEncryption( ulDeviceId, &encryptSettings );

	return ulRet;
}

/* For PAT, PMT */
HUINT32		PAL_REC_ReplacePacketModeOn(HUINT32 ulDemuxId, HUINT16 usVirtualPid, HUINT16 usPid, HUINT8 *pucSection, HUINT32 ulSectionSize, HUINT32 *pulHandle)
{
	HUINT32 ulRet = DI_ERR_ERROR;

	if ((pucSection == NULL) || (pulHandle == NULL))
	{
		return ERR_FAIL;
	}
	ulRet = DI_DEMUX_ReplacePacketModeOn(ulDemuxId, usVirtualPid, usPid, pucSection, ulSectionSize, pulHandle);

	return (ulRet == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HUINT32		PAL_REC_ReplacePacketModeOff(HUINT32 ulHandle)
{
	HUINT32 ulRet = DI_ERR_ERROR;

	ulRet = DI_DEMUX_ReplacePacketModeOff(ulHandle);

	return (ulRet == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

/* For special table as SIT */
HUINT32		PAL_REC_InsertPacketModeOn(HUINT32 ulDemuxId, HUINT16 uPid, HUINT8 *pucSection, HUINT32 ulSectionSize, HUINT32 ulInterval, HUINT32 *pulHandle)
{
	HUINT32 ulRet = DI_ERR_ERROR;

	if ((pucSection == NULL) || (pulHandle == NULL))
	{
		return ERR_FAIL;
	}
	ulRet = DI_DEMUX_InsertPacketModeOn(ulDemuxId, uPid, pucSection, ulSectionSize, ulInterval, pulHandle);

	return (ulRet == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

HUINT32		PAL_REC_InsertPacketModeOff(HUINT32 ulHandle)
{
	HUINT32 ulRet = DI_ERR_ERROR;

	ulRet = DI_DEMUX_InsertPacketModeOff(ulHandle);

	return (ulRet == DI_ERR_OK) ? ERR_OK : ERR_FAIL;
}

/*********************** End of File ******************************/
