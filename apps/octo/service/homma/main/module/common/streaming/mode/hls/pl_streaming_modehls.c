/**************************************************************
*
*	http://www.humaxdigital.com
*
*	@data			2012/10/24
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include "../../int_streaming.h"

#include <sapi.h>
/**********************************************************************************************************/
#define ____DEFINE________________________
/**********************************************************************************************************/
#define HLS_MAX_CHUNK   20

/**********************************************************************************************************/
#define ____TYPEDEF________________________
/**********************************************************************************************************/

typedef enum _plStreaming_M3U8Type_e
{
	plStreaming_M3U8Type_None = 0,

	plStreaming_M3U8Type_Root,

	plStreaming_M3U8Type_Bitrate,

	plStreaming_M3U8Type_Max
} plStreaming_M3U8Type_e;



/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/


/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/
STATIC void _plstreaming_ModeHLS_DestroyRingbuf(PLStreaming_ModeInst_t *pstModeInst);



STATIC plStreaming_M3U8Type_e _plstreaming_ModeHLS_GetM3U8Type(const PLStreaming_ModeInst_t *pstInst, const HCHAR *pRequestPath)
{
	HxLOG_Print("pRequestPath(%s)\r\n", pRequestPath);

	if(NULL != HxSTD_StrRChar(pRequestPath, '_'))
	{
		return plStreaming_M3U8Type_Bitrate;
	}

	return plStreaming_M3U8Type_Root;
}

STATIC void _plstreaming_ModeHLS_SignalCallback(HxRingBuf_SignalType_e eType, Handle_t hRingBuf, HxRingBuf_Status_t *pstStatus)
{
	switch(eType)
	{
		case eHxRINGBUF_SIG_Read:
			HxLOG_Print("Received callback[ ulStartChunkId(%d) ulEndChunkId(%d) ulUnreadSize(%d) ulChunkNum(%d) ]\r\n",
						pstStatus->ulStartChunkId, pstStatus->ulEndChunkId, pstStatus->ulUnreadSize, pstStatus->ulChunkNum);
			break;

		default:
			break;
	}
}

STATIC Handle_t _plstreaming_ModeHLS_CreateRingbuf(PLStreaming_ModeInst_t *pstInst)
{
	Handle_t			hRingbuf = HANDLE_NULL;
	HINT32				nChunkSize = 0;
	HINT32				nChunkMaxSize = 0;
	HINT32 				nBitreate = 0;
	HxRingBuf_Setup_t	stRingbufSetup;
	HINT32				nSvcUid = 0;
	CO_CHANNEL_t		stCoCh;
	HERROR				hErr = ERR_FAIL;
	HBOOL				bIsRadio = FALSE;

	if(NULL == pstInst)
	{
		HxLOG_Error("pstInst is NULL\n");
		return HANDLE_NULL;
	}

	HxLOG_Print("pstInst->stSetupInfo.eStreamType = 0x%x\n", pstInst->stSetupInfo.eStreamType);
	if((DxRSVSTREAM_TYPE_HLS_FILE_PVR_AUDIO == pstInst->stSetupInfo.eStreamType)
		|| (DxRSVSTREAM_TYPE_HLS_LIVE_AUDIO == pstInst->stSetupInfo.eStreamType)
		|| (DxRSVSTREAM_TYPE_NTLS_LIVE_AUDIO == pstInst->stSetupInfo.eStreamType)
		|| (DxRSVSTREAM_TYPE_NTLS_FILE_PVR_AUDIO == pstInst->stSetupInfo.eStreamType))
	{
		bIsRadio = TRUE;
		HxLOG_Print("bIsRadio = %d\n", bIsRadio);
	}

	if (0)
	{
		/* Fixed PVR record HLS error by rev.110587
			when getting exactly matched uid between previous recording Video PVR and current live channel */
		if ((pstInst->stSetupInfo.eStreamType != DxRSVSTREAM_TYPE_HLS_FILE_MEDIA) &&
			(pstInst->stSetupInfo.eStreamType != DxRSVSTREAM_TYPE_HLS_FILE_TSR) &&
			(pstInst->stSetupInfo.eStreamType != DxRSVSTREAM_TYPE_HLS_FILE_PVR) &&
			(pstInst->stSetupInfo.eStreamType != DxRSVSTREAM_TYPE_HLS_FILE_PVR_AUDIO) &&
			(pstInst->stSetupInfo.eStreamType != DxRSVSTREAM_TYPE_HLS_FILE_MUSIC) &&
			(pstInst->stSetupInfo.eStreamType != DxRSVSTREAM_TYPE_NTLS_FILE_MEDIA) &&
			(pstInst->stSetupInfo.eStreamType != DxRSVSTREAM_TYPE_NTLS_FILE_TSR) &&
			(pstInst->stSetupInfo.eStreamType != DxRSVSTREAM_TYPE_NTLS_FILE_PVR) &&
			(pstInst->stSetupInfo.eStreamType != DxRSVSTREAM_TYPE_NTLS_FILE_PVR_AUDIO) &&
			(pstInst->stSetupInfo.eStreamType != DxRSVSTREAM_TYPE_NTLS_FILE_MUSIC))
		{
			nSvcUid = (HINT32)(pstInst->stSetupInfo.uUID & 0xffffffff);
			HxSTD_memset(&stCoCh, 0, sizeof(CO_CHANNEL_t));
			hErr = PL_COMMON_GetChannel(nSvcUid, &stCoCh);
			if ((ERR_OK == hErr) && (TRUE == stCoCh.bValid))
			{
				if (NULL != stCoCh.svc) /* <-- NULL?? */
				{
					HxLOG_Print("stCoCh.svc->svcType = 0x%x\n", stCoCh.svc->svcType);
					if(eDxSVC_TYPE_RADIO == stCoCh.svc->svcType)
					{
						bIsRadio = TRUE;
					}
				}
			}
		}
	}

	nBitreate = pstInst->nBitrate;
	if (40 > nBitreate)
	{
		nBitreate = (160);
		HxLOG_Error("Bitreate error!! \r\n");
		#if defined(CONFIG_DEBUG)
		HxLOG_Assert(0);
		#endif

		return hRingbuf;
	}


	// TODO: radio chunk size에 도달하지 못해, radio hls 안 되는 문제가 있어 chunk size 조정함 - 2014-02-16
	if(bIsRadio)
	{
		nChunkMaxSize = PL_Streaming_Spec_HLS_GetBufferSize();
		if(nChunkMaxSize > 0)
		{
			nChunkMaxSize /= (20*4);
			nChunkMaxSize -= (nChunkMaxSize % 188);
		}
		else
		{
			nChunkMaxSize = (512*188);
		}

		nChunkSize = (nBitreate / 64) * pstInst->stSetupInfo.nChunkMultiply;
		if(nChunkSize > nChunkMaxSize)
		{
			nChunkSize = nChunkMaxSize;
		}
	}
	else
	{
		nChunkMaxSize = PL_Streaming_Spec_HLS_GetBufferSize();
		if(nChunkMaxSize > 0)
		{
			nChunkMaxSize /= 20;
			nChunkMaxSize -= (nChunkMaxSize % 188);
		}
		else
		{
			nChunkMaxSize = (8192*188);   // max (30mb / 20)
		}

		nChunkSize = (nBitreate / 4) * pstInst->stSetupInfo.nChunkMultiply;
		if(nChunkSize > nChunkMaxSize)
		{
			nChunkSize = nChunkMaxSize;
		}
	}
	HxLOG_Print("Radio?(%d) ChunkSize(%d) nBitreate(%d) nChunkMultiply(%d)\r\n", bIsRadio, nChunkSize, nBitreate, pstInst->stSetupInfo.nChunkMultiply);

	HxSTD_memset(&stRingbufSetup, 0, sizeof(HxRingBuf_Setup_t));
	stRingbufSetup.eBufType 		= eHxRINGBUF_TYPE_CHUNK;
    stRingbufSetup.ulMaxSize		= nChunkSize * HLS_MAX_CHUNK;	/* 20개를 가지도록 함. */
	stRingbufSetup.ulChunkSize		= nChunkSize;
	stRingbufSetup.bBlockOverflow	= FALSE;			/* true:overflow시 write block 됨. */
	stRingbufSetup.eRuleAfterRead	= eHxRINGBUF_READRULE_REMAIN;
	stRingbufSetup.cbSignal			= _plstreaming_ModeHLS_SignalCallback;

	hRingbuf = PL_RINGBUF_Create(&stRingbufSetup);
	if (hRingbuf == HANDLE_NULL)
	{
		HxLOG_Error("Error> Cant create ringbuffer!\n");
		return HANDLE_NULL;
	}

	HxLOG_Print("===================== set ringbuf =====================\r\n");
	HxLOG_HexIntDump(stRingbufSetup.eBufType);
	HxLOG_DecIntDump(stRingbufSetup.ulMaxSize);
	HxLOG_DecIntDump(stRingbufSetup.ulChunkSize);
	HxLOG_BoolDump(stRingbufSetup.bBlockOverflow);
	HxLOG_HexIntDump(stRingbufSetup.eRuleAfterRead);
	HxLOG_HexIntDump(hRingbuf);
	HxLOG_Print("==========================================\r\n");
	return hRingbuf;
}

STATIC HOM_Result_e _plstreaming_ModeHLS_ReadRingbuf(PLStreaming_ModeInst_t *pstInst, HINT32 nTsIndex, HUINT32 *pulDataSize, HUINT8 **ppucData)
{
	HERROR					hErr = ERR_OK;
	HINT32					nLoopCnt = 0;
	HINT32					nSleepTime = 0;
	HINT32					ulDataSize = 0;
	HUINT8					*pucData = NULL;
	HOM_Result_e				eHomResult = eHOM_Result_Ok;
	HxRingBuf_Status_t			stStatus;
	PLStreaming_SetupStatus_e	eSetupStatus = ePLStreaming_SetupStatus_None;
	HxLOG_Print("++[nTsIndex:%d]\n", nTsIndex);

	HxSTD_MemSet(&stStatus, 0, sizeof(HxRingBuf_Status_t));

	while(1)
	{
		eHomResult = PL_Streaming_Resource_GetStatus(pstInst->hResHandle, &eSetupStatus);
		if( eHOM_Result_Ok != eHomResult || ePLStreaming_SetupStatus_Conflict == eSetupStatus)
		{
			HxLOG_Error("PL_Streaming_Resource_GetStatus error!!\r\n");
			return ePLStreaming_SetupStatus_Conflict;
		}

		hErr = PL_RINGBUF_GetBufferingStatus(pstInst->hRingbuf, &stStatus);
		if(ERR_OK != hErr)
		{
			HxLOG_Error("PL_RINGBUF_GetBufferingStatus(hRingBuf:0x%x) failed!!\r\n", pstInst->hResHandle);
			return eHOM_Result_Error_Resource_Notfound;
		}

		HxLOG_Print("[hRingBuf:0x%x] nLoopCnt(%d) nTsIndex(%d) ulStartChunkId(%d) ulEndChunkId(%d) ulUnreadSize(%d) ulChunkNum(%d)\r\n",
								pstInst->hRingbuf, nLoopCnt, nTsIndex, stStatus.ulStartChunkId, stStatus.ulEndChunkId, stStatus.ulUnreadSize, stStatus.ulChunkNum);

		if( nTsIndex < stStatus.ulStartChunkId)
		{
			HxLOG_Error("[hRingBuf:0x%x] nTsIndex"HxANSI_COLOR_YELLOW("(%d)")" < ulStartChunkId"HxANSI_COLOR_YELLOW("(%d)")"!!, ulEndChunkId(%d)\r\n",
									pstInst->hRingbuf, nTsIndex, stStatus.ulStartChunkId, stStatus.ulEndChunkId);
			return eHOM_Result_Error_Resource_Notfound;
		}

		if( nTsIndex < stStatus.ulEndChunkId )
		{
			break;
		}

		nSleepTime = nTsIndex - stStatus.ulEndChunkId;
		if(nSleepTime > 8)
		{
			HxLOG_Error("[hRingBuf:0x%x] nSleepTime(%d) > 8!, nTsIndex(%d) ulStartChunkId(%d) ulEndChunkId(%d)\r\n",
									pstInst->hRingbuf, nSleepTime, nTsIndex, stStatus.ulStartChunkId, stStatus.ulEndChunkId);
			return eHOM_Result_Error_Resource_Notfound;
		}
		else if(nSleepTime > 0)
		{
			nLoopCnt += nSleepTime;
			nSleepTime *= 300;
		}
		else
		{
			nLoopCnt++;
			nSleepTime = 300;
		}
		VK_TASK_Sleep(nSleepTime);

		if(nLoopCnt > 100)
		{
			HxLOG_Warning("Error> [hRingBuf:0x%x] [nLoopCnt > 100], nTsIndex(%d) ulStartChunkId(%d) ulEndChunkId(%d) nSleepTime(%d)\r\n",
									pstInst->hRingbuf, nTsIndex, stStatus.ulStartChunkId, stStatus.ulEndChunkId, nSleepTime);
			return eHOM_Result_Error_Resource_Notfound;
		}
	}

	hErr = PL_RINGBUF_GetChunkSize(pstInst->hRingbuf,  nTsIndex, &ulDataSize);
	if(ERR_OK != hErr || 0 == ulDataSize)
	{
		HxLOG_Error("PL_RINGBUF_GetChunkSize error!!\r\n");
		return eHOM_Result_Error_Resource_Notfound;
	}

	pucData = HLIB_STD_MemAlloc(ulDataSize);
	if(NULL == pucData)
	{
		HxLOG_Error("PL_RINGBUF_GetChunkSize error!!\r\n");
		return eHOM_Result_Error_OutofMem;
	}

	hErr = PL_RINGBUF_ReadChunk(pstInst->hRingbuf,  nTsIndex, pucData, &ulDataSize);
	if(ERR_OK != hErr)
	{
		if(NULL != pucData)
		{
			HLIB_STD_MemFree(pucData);
		}
		HxLOG_Error("PL_RINGBUF_ReadChunk(hRingBuf:0x%x)  error!!\r\n", pstInst->hRingbuf);
		return eHOM_Result_Error_Resource_Notfound;
	}

	HxLOG_Print(HxANSI_COLOR_YELLOW("PL_RINGBUF_ReadChunk(hRingBuf:0x%x) OK!! ulDataSize = %d\r\n"), pstInst->hRingbuf, ulDataSize);

	if(eHOM_Result_Ok == eHomResult)
	{
		if(ePLStreaming_OutType_DumpFile & pstInst->stSetupInfo.eOutType)
		{
			HCHAR 		pszFileName[256] = {0, };
			HxFILE_t	*fp = NULL;

			HxSTD_snprintf(pszFileName, 255, "HLS_%s_%d_%08d_%x.ts", pstInst->stSetupInfo.pszDeviceID, (HUINT32)pstInst->stSetupInfo.uUID, nTsIndex, HLIB_STD_GetSystemTime());

			fp = HLIB_FILE_Open(pszFileName, "wb");
			if(fp)
			{
				HLIB_FILE_Write(fp, pucData, ulDataSize, 1);
				HLIB_FILE_Close(fp);
				eHomResult = eHOM_Result_Ok;
			}
		}

	}

	*pulDataSize 	= ulDataSize;
	*ppucData 		= pucData;

	HxLOG_Print("--[hRingBuf:0x%x, nTsIndex:%d, eHomResult:%d]\n", pstInst->hRingbuf, nTsIndex, eHomResult);
	return eHomResult;
}

/**
  @brief RequestTS 가  RingBuf 안에 있는지 확인합니다.
  @retval 0  Available Chunk Exists.
  @retval -1 Need to seek backward
  @retval 1  Neet to seek forward
*/

STATIC int _plstreaming_ModeHLS_IsAvailableChunk(PLStreaming_ModeInst_t *pstModeInst, HxRingBuf_Status_t stRingBufStatus)
{
    if ((pstModeInst->Mode.stHLS.StartTsIndex + stRingBufStatus.ulStartChunkId <= pstModeInst->Mode.stHLS.RequestTS) &&
        (pstModeInst->Mode.stHLS.RequestTS < pstModeInst->Mode.stHLS.StartTsIndex + stRingBufStatus.ulEndChunkId))
    {
        return 0;
    }

    if (pstModeInst->Mode.stHLS.RequestTS < pstModeInst->Mode.stHLS.StartTsIndex + stRingBufStatus.ulStartChunkId)
    {
        return -1;
    }

    return 1;
}

STATIC int _plstreaming_ModeHLS_AvailableChunkCount(PLStreaming_ModeInst_t *pstModeInst, HxRingBuf_Status_t stRingBufStatus)
{
    if ((pstModeInst->Mode.stHLS.StartTsIndex + stRingBufStatus.ulStartChunkId <= pstModeInst->Mode.stHLS.RequestTS) &&
        (pstModeInst->Mode.stHLS.RequestTS < pstModeInst->Mode.stHLS.StartTsIndex + stRingBufStatus.ulEndChunkId))
    {
        return (pstModeInst->Mode.stHLS.StartTsIndex + stRingBufStatus.ulEndChunkId - pstModeInst->Mode.stHLS.RequestTS);
    }

    return 0;
}

STATIC HOM_Result_e _plstreaming_ModeHLS_ReadRingbuf_ForPVR(PLStreaming_ModeInst_t *pstModeInst, HINT32 nTsIndex, HUINT32 *pulDataSize, HUINT8 **ppucData)
{
    HERROR					hErr = ERR_OK;
    HINT32					ulDataSize = 0;
    HUINT8					*pucData = NULL;
    HOM_Result_e				eHomResult = eHOM_Result_Ok;
    HxRingBuf_Status_t			stStatus;

    HxSTD_MemSet(&stStatus, 0, sizeof(HxRingBuf_Status_t));
    int RetryCnt;


    if (pstModeInst->eSetupStatus == ePLStreaming_SetupStatus_None)
    {
        // new
        eHomResult = PL_StreamingContextUtil_MakeTuningParamAndDisplayString(pstModeInst);
        if(eHOM_Result_Ok != eHomResult)
        {
            HxLOG_Error("Error> PL_StreamingContextUtil_MakeTuningParamAndDisplayString\r\n");
            return eHomResult;
        }

        if (pstModeInst->hResHandle == HANDLE_NULL)
        {
            pstModeInst->hResHandle = PL_Streaming_Resource_New(pstModeInst->hStreamingId);
            if (pstModeInst->hResHandle == NULL)
            {
                HxLOG_Error("PL_Streaming_Resource_New(hStreamingId:%d) failed\n");
                return eHOM_Result_Error_Resource_Notfound;
            }
        }

        if(HANDLE_NULL == pstModeInst->hRingbuf || 0 == pstModeInst->hRingbuf)
        {
            pstModeInst->hRingbuf	= _plstreaming_ModeHLS_CreateRingbuf(pstModeInst);
            if(HANDLE_NULL == pstModeInst->hRingbuf || 0 == pstModeInst->hRingbuf)
            {
                HxLOG_Error("Error> ringbuf create() failed!\r\n");
                return eHOM_Result_Error;
            }
        }

        eHomResult = PL_Streaming_Resource_Start(pstModeInst->hResHandle, pstModeInst, 0);
        if (eHomResult != eHOM_Result_Ok)
        {
            HxLOG_Error("PL_Streaming_Resource_Start failed(eHomResult:%d)\n", eHomResult);
            return eHomResult;
        }
    }

    if (pstModeInst->eSetupStatus == ePLStreaming_SetupStatus_HlsPaused)
    {
#if 0
        /* 2015/3/16 mhkang: PVR HLS Pause후 Resume했을 때 현재 RingBuf 마지막 위치 Ts Index부터 AddSchedule하여 요청한다.
           Android/IOS 기본 Player들은 AddSchedule 한 후에 전달받은 TS를 재생할 무렵에 재생이 종료된다.
         */
        hErr = PL_RINGBUF_GetBufferingStatus(pstModeInst->hRingbuf, &stStatus);
        if(ERR_OK != hErr)
        {
            HxLOG_Error("PL_RINGBUF_GetBufferingStatus(hRingBuf:0x%x) failed(hErr:%d)\n", pstModeInst->hResHandle, hErr);
            return eHOM_Result_Error_Resource_Notfound;
        }

        if (_plstreaming_ModeHLS_AvailableChunkCount(pstModeInst, stStatus) < 5)  //  잔여 chunk가 설정 개수 미만일때부터 AddSchedule한다
        {
            HxLOG_Info(HxANSI_COLOR_YELLOW("===============================================\n"));
            HxLOG_Info(HxANSI_COLOR_YELLOW("  !!! PL_Streaming_Resource_Resume !!!\n"));
            HxLOG_Info(HxANSI_COLOR_YELLOW("===============================================\n"));

            HxLOG_Debug(HxANSI_COLOR_YELLOW("[LAST BUFFER INFO] StartTsIndex:%d, RingStart:%d ringEnd:%d TsIndex:%d\n"), pstModeInst->Mode.stHLS.StartTsIndex, stStatus.ulStartChunkId, stStatus.ulEndChunkId, nTsIndex);

            pstModeInst->hResHandle = PL_Streaming_Resource_New(pstModeInst->hStreamingId);
            if (pstModeInst->hResHandle == NULL)
            {
                HxLOG_Error("PL_Streaming_Resource_New(hStreamingId:%d) failed\n");
                return eHOM_Result_Error_Resource_Notfound;
            }

            // mhkang: Calculate the PlayPosition with stStatus of RingBuf
            // if (nTsIndex  != pstModeInst->RequestTS) nTsIndex위치부터 Resource_Start!
            eHomResult = PL_Streaming_Resource_Start(pstModeInst->hResHandle, pstModeInst, (pstModeInst->Mode.stHLS.StartTsIndex + stStatus.ulEndChunkId) * 2);
            if (eHomResult != eHOM_Result_Ok)
            {
                HxLOG_Error("PL_Streaming_Resource_Start failed(eHomResult:%d)\n", eHomResult);
                return eHomResult;
            }
        }
        else
        {
            HxLOG_Info(HxANSI_COLOR_YELLOW("  Read Chunk(%d) but No Streamig Starts. !!!\n"), pstModeInst->Mode.stHLS.RequestTS);
        }
#else
        /* 2015/3/16 mhkang: PVR HLS Pause후 Resume했을 때 RingBuf Clear한 후 AddSchedule한다.
          Android/IOS 기본 Player들은 AddSchedule 한 후에 전달받은 TS를 재생할 무렵에 재생이 종료된다.
        */
        HxLOG_Info(HxANSI_COLOR_YELLOW("===============================================\n"));
        HxLOG_Info(HxANSI_COLOR_YELLOW("  !!! PL_Streaming_Resource_Resume !!!\n"));
        HxLOG_Info(HxANSI_COLOR_YELLOW("===============================================\n"));
        pstModeInst->hResHandle = PL_Streaming_Resource_New(pstModeInst->hStreamingId);
        if (pstModeInst->hResHandle == NULL)
        {
            HxLOG_Error("PL_Streaming_Resource_New(hStreamingId:%d) failed\n");
            return eHOM_Result_Error_Resource_Notfound;
        }

        pstModeInst->Mode.stHLS.StartTsIndex = nTsIndex;

        _plstreaming_ModeHLS_DestroyRingbuf(pstModeInst);

        HxLOG_Info(HxANSI_COLOR_RED("Destroy ringBuf(%x)\n"), pstModeInst->hRingbuf);
        pstModeInst->hRingbuf = _plstreaming_ModeHLS_CreateRingbuf(pstModeInst);
        if(HANDLE_NULL == pstModeInst->hRingbuf || 0 == pstModeInst->hRingbuf)
        {
            HxLOG_Error("_plstreaming_ModeHLS_CreateRingbuf(hRingBuf:0x%x) error\r\n", pstModeInst->hRingbuf);
            return eHOM_Result_Error_CreateFail;
        }

        eHomResult = PL_Streaming_Resource_Start(pstModeInst->hResHandle, pstModeInst, nTsIndex * 2);
        if (eHomResult != eHOM_Result_Ok)
        {
            HxLOG_Error("PL_Streaming_Resource_Start failed(eHomResult:%d)\n", eHomResult);
            return eHomResult;
        }
#endif
    }


    RetryCnt = 0;
    while(1)
    {
        hErr = PL_RINGBUF_GetBufferingStatus(pstModeInst->hRingbuf, &stStatus);
        if(ERR_OK != hErr)
        {
            HxLOG_Error("PL_RINGBUF_GetBufferingStatus(hRingBuf:0x%x) failed(hErr:%d)\n", pstModeInst->hResHandle, hErr);
            return eHOM_Result_Error_Resource_Notfound;
        }

        //HxLOG_Debug(HxANSI_COLOR_YELLOW("[BUFFER INFO] StartTsIndex:%d, RingStart:%d ringEnd:%d TsIndex:%d\n"), pstModeInst->Mode.stHLS.StartTsIndex, stStatus.ulStartChunkId, stStatus.ulEndChunkId, nTsIndex);

        // TsIndex in RingBuf Range
        if ((pstModeInst->Mode.stHLS.StartTsIndex + stStatus.ulStartChunkId <= nTsIndex) && (nTsIndex < pstModeInst->Mode.stHLS.StartTsIndex + stStatus.ulEndChunkId))
        {
            HxLOG_Debug(HxANSI_COLOR_YELLOW("[BUFFER INFO] Ok!!!\n"));
            break;
        }

        // SEEK Backward
        if ((nTsIndex < pstModeInst->Mode.stHLS.StartTsIndex + stStatus.ulStartChunkId) || (pstModeInst->Mode.stHLS.StartTsIndex + stStatus.ulEndChunkId < nTsIndex))
        {
            if (nTsIndex < pstModeInst->Mode.stHLS.StartTsIndex + stStatus.ulStartChunkId)
            {
                HxLOG_Debug(HxANSI_COLOR_GREEN("[SEEK] <<<<<<<<\n"));
            }
            else
            {
                HxLOG_Debug(HxANSI_COLOR_PURPLE("[SEEK] >>>>>>>>\n"));
            }

            if (pstModeInst->eSetupStatus == ePLStreaming_SetupStatus_Running) // must update schedule when status is Running .
            {
                pstModeInst->Mode.stHLS.StartTsIndex = nTsIndex;

                _plstreaming_ModeHLS_DestroyRingbuf(pstModeInst);

                pstModeInst->hRingbuf = _plstreaming_ModeHLS_CreateRingbuf(pstModeInst);
                if(HANDLE_NULL == pstModeInst->hRingbuf || 0 == pstModeInst->hRingbuf)
                {
                    HxLOG_Error("_plstreaming_ModeHLS_CreateRingbuf(hRingBuf:0x%x) error\r\n", pstModeInst->hRingbuf);
                    return eHOM_Result_Error_CreateFail;
                }

                PL_Streaming_Resource_UpdateSchedulePosition(pstModeInst->hResHandle, nTsIndex * 2, pstModeInst->hRingbuf);
            }
        }

        VK_TASK_Sleep(500);

        // Timeout!
        if (RetryCnt++ > 100)
        {
            HxLOG_Debug(HxANSI_COLOR_RED("[BUFFER INFO] Timeout!\n"));
            return eHOM_Result_Error_Resource_Notfound;
        }
    }

    hErr = PL_RINGBUF_GetChunkSize(pstModeInst->hRingbuf,  nTsIndex - pstModeInst->Mode.stHLS.StartTsIndex, &ulDataSize);
    if(ERR_OK != hErr || 0 == ulDataSize)
    {
        HxLOG_Error("PL_RINGBUF_GetChunkSize failed(hErr:%d, ulDataSize:%d)\n", hErr, ulDataSize);
        return eHOM_Result_Error_Resource_Notfound;
    }

    pucData = HLIB_STD_MemAlloc(ulDataSize);
    if(NULL == pucData)
    {
        HxLOG_Error("PL_RINGBUF_GetChunkSize error!!\r\n");
        return eHOM_Result_Error_OutofMem;
    }

    hErr = PL_RINGBUF_ReadChunk(pstModeInst->hRingbuf,  nTsIndex - pstModeInst->Mode.stHLS.StartTsIndex, pucData, &ulDataSize);
    if(ERR_OK != hErr)
    {
        if(NULL != pucData)
        {
            HLIB_STD_MemFree(pucData);
        }
        HxLOG_Error("PL_RINGBUF_ReadChunk(hRingBuf:0x%x)  error!!\r\n", pstModeInst->hRingbuf);
        return eHOM_Result_Error_Resource_Notfound;
    }

    HxLOG_Print(HxANSI_COLOR_YELLOW("PL_RINGBUF_ReadChunk(hRingBuf:0x%x) OK!! ulDataSize = %d\r\n"), pstModeInst->hRingbuf, ulDataSize);

    if(eHOM_Result_Ok == eHomResult)
    {
        if(ePLStreaming_OutType_DumpFile & pstModeInst->stSetupInfo.eOutType)
        {
            HCHAR 		pszFileName[256] = {0, };
            HxFILE_t	*fp = NULL;

            HxSTD_snprintf(pszFileName, 255, "HLS_%s_%d_%08d_%x.ts", pstModeInst->stSetupInfo.pszDeviceID, (HUINT32)pstModeInst->stSetupInfo.uUID, nTsIndex, HLIB_STD_GetSystemTime());

            fp = HLIB_FILE_Open(pszFileName, "wb");
            if(fp)
            {
                HLIB_FILE_Write(fp, pucData, ulDataSize, 1);
                HLIB_FILE_Close(fp);
                eHomResult = eHOM_Result_Ok;
            }
        }
    }

    *pulDataSize 	= ulDataSize;
    *ppucData 		= pucData;

    HxLOG_Print("--[hRingBuf:0x%x, nTsIndex:%d, eHomResult:%d]\n", pstModeInst->hRingbuf, nTsIndex, eHomResult);
    return eHomResult;
}

STATIC void _plstreaming_ModeHLS_DestroyRingbuf(PLStreaming_ModeInst_t *pstModeInst)
{
    if(HANDLE_NULL != pstModeInst->hRingbuf && 0 != pstModeInst->hRingbuf)
	{
        PL_RINGBUF_Destroy(pstModeInst->hRingbuf);
        pstModeInst->hRingbuf = HANDLE_NULL;
	}
}


STATIC HINT32 _plstreaming_ModeHLS_GetTSIndex(PLStreaming_ModeInst_t *pstInst, const HCHAR *pRequestPath)
{
	HINT32 		nTsIndex = 0;
	HCHAR		*pStrPos = NULL;

#if 0
	pStrPos = (HCHAR *)pRequestPath + PL_StreamingKey_Length(pstInst->Mode.stHLS.pstBitrateStreamingKey);
#endif
	pStrPos = (HCHAR *)pRequestPath;

	pStrPos = HxSTD_StrRChar(pStrPos, '_');
	if( NULL == pStrPos)
	{
		HxLOG_Error("pStrPos error pRequestPath(%s)\r\n", pRequestPath);
		return -1;
	}
	if('_' == *pStrPos)
	{
		pStrPos++;
	}

	nTsIndex = HLIB_STD_StrToINT32(pStrPos);

	return nTsIndex;
}

STATIC HOM_Result_e _plstreaming_ModeHLS_GenerationRootM3U8(PLStreaming_ModeInst_t *pstInst, const HCHAR *pRequestPath, PL_M3U8_t 	*pstM3U8)
{
	HINT32 			i = 0;
	HINT32			nProfileCount = 0;
	HINT32			nProfileStartNum = 0;
	HINT32			nBandWidth = 0;
	HERROR			hErr = ERR_OK;
	HINT32			nBitrateKbps = 0;

	HOM_Result_e			eHomResult = eHOM_Result_Ok;
	PLStreaming_SetupInfo_t	*pSetupInfo = NULL;
	PLMediaProfile_Item_t	*pstProfileItem = NULL;
	PL_StreamingKey_t		*pstDupStreamingKey = NULL;

	(void)pRequestPath;

	pSetupInfo  = &(pstInst->stSetupInfo);

	eHomResult = PL_MediaProfile_GetItemCount(pSetupInfo->pstMediaProfile, &nProfileCount);
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("profile list get error!!\r\n");
		return eHOM_Result_Error;
	}

	eHomResult = PL_MediaProfile_GetStartNumber(pSetupInfo->pstMediaProfile, &nProfileStartNum);
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("profile start number get error!!\r\n");
		return eHOM_Result_Error;
	}

	HxLOG_Debug("nProfileCount(%d) nProfileStartNum(%x) pszStreamingKey(%s)\r\n",
										nProfileCount, nProfileStartNum, PL_StreamingKey_GetBuffer(pstInst->pstStreamingKey));

	for(i=0; i < nProfileCount; i++)
	{
		eHomResult = PL_MediaProfile_GetItem(pSetupInfo->pstMediaProfile, (nProfileStartNum+i), &pstProfileItem);
		if(eHOM_Result_Ok != eHomResult || NULL == pstProfileItem)
		{
			HxLOG_Error("profile item  get error (nProfileStartNum+i) = %d!!\r\n", (nProfileStartNum+i));
			continue;
		}

		eHomResult = PL_MediaProfile_Item_GetBandwidthKBitrate(pstProfileItem->hItem, &nBandWidth);
		if(eHOM_Result_Ok != eHomResult || 0 == nBandWidth)
		{
			HxLOG_Error("nBandWidth  get error (nProfileStartNum+i) = %d!!\r\n", (nProfileStartNum+i));
			continue;
		}

		hErr = PL_M3U8_AppendStreamInf(pstM3U8, 1, nBandWidth, NULL, NULL);
		if(ERR_OK != hErr)
		{
			HxLOG_Error("PL_M3U8_AppendStreamInf  get error (nProfileStartNum+i) = %d!!\r\n", (nProfileStartNum+i));
		}

		pstDupStreamingKey = PL_StreamingKey_ConditionalDup(pstInst->pstStreamingKey, NULL, 0, pstProfileItem->hItem, NULL);
		if(NULL == pstDupStreamingKey)
		{
			HxLOG_Error("Memory Check, stop  hls!!\r\n");
			return eHOM_Result_Error;
		}

		nBitrateKbps = (TRUE == pstInst->stSetupInfo.bIsIncludeVideo) ? pstProfileItem->nVideoBitrateKbps : pstProfileItem->nAudioBitrateKbps;

		hErr = PL_M3U8_AppendM3U8(pstM3U8, PL_StreamingKey_GetBuffer(pstDupStreamingKey), nBitrateKbps);
		if(ERR_OK != hErr)
		{
			HxLOG_Error("PL_M3U8_AppendM3U8  get error (nProfileStartNum+i) = %d!!\r\n", (nProfileStartNum+i));
		}

		PL_StreamingKey_Delete(pstDupStreamingKey);
		pstDupStreamingKey = NULL;
	}

	//hErr = PL_M3U8_AppendEndList(pstM3U8);
	if(ERR_OK != hErr)
	{
		HxLOG_Error("PL_M3U8_AppendM3U8  get error (nProfileStartNum+i) = %d!!\r\n", (nProfileStartNum+i));
	}


	return eHOM_Result_Ok;
}

STATIC HOM_Result_e _plstreaming_ModeHLS_GetRootM3U8(PLStreaming_ModeInst_t *pstInst, const HCHAR *pRequestPath, HINT32 *pnM3U8Size, HUINT8 **ppucM3U8Buf)
{
	HxSTR_t			*pM3U8Str = NULL;
	HINT32			nSize = 0;
	HUINT8			*pBuffer = NULL;
	PL_M3U8_t 		*pstM3U8 = NULL;
	HOM_Result_e	eHomResult = eHOM_Result_Ok;

	Handle_t					hProfileHandle = HANDLE_NULL;
	PLStreaming_SetupInfo_t		*pSetupInfo = NULL;

	pSetupInfo  = &(pstInst->stSetupInfo);

	pstM3U8 = PL_M3U8_New(PLM3U8_QUEUESIZE_INFINITY);
	if(NULL == pstM3U8)
	{
		HxLOG_Error("m3u8 create error!! mem check!!\r\n");
		return eHOM_Result_Error_OutofMem;
	}

	if( NULL == pstInst->pstStreamingKey)
	{
		if(NULL != pSetupInfo->pstMediaProfile)
		{
			PL_MediaProfile_GetHandle(pSetupInfo->pstMediaProfile, &hProfileHandle);
		}
		else
		{
			HxLOG_Warning(" NULL == pSetupInfo->pstMediaProfile!! \r\n");
		}

		pstInst->pstStreamingKey	=  PL_StreamingKey_New(pSetupInfo->pszPrefix, pSetupInfo->uUID, hProfileHandle, pSetupInfo->pszDeviceID);
		if(NULL == pstInst->pstStreamingKey)
		{
			HxLOG_Error(" PL_StreamingKey_New error!! \r\n");
			eHomResult = eHOM_Result_Error;
			goto err_rtn;
		}
	}

	eHomResult = _plstreaming_ModeHLS_GenerationRootM3U8(pstInst, pRequestPath, pstM3U8);
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("_plstreaming_ModeHLS_GenerationRootM3U8 fail!! \r\n");
		eHomResult = eHOM_Result_Error_OutofMem;
		goto err_rtn;
	}

	pM3U8Str = PL_M3U8_ToBuffer(pstM3U8);
	if(NULL == pM3U8Str)
	{
		HxLOG_Error("pM3U8Str error!! m3u8 check!!\r\n");
		eHomResult = eHOM_Result_Error_Resource_Notfound;
		goto err_rtn;
	}

	nSize 			= HLIB_STR_GetLength(pM3U8Str);
	pBuffer 		= HLIB_STR_toUTF8(pM3U8Str);
	*pnM3U8Size 	= nSize;
	*ppucM3U8Buf	= HLIB_STD_StrDup(pBuffer);

	PL_M3U8_Delete(pstM3U8);	pstM3U8 = NULL;
	HLIB_STR_Delete(pM3U8Str);	pM3U8Str = NULL;

	return eHOM_Result_Ok;


err_rtn:
	if(NULL != pstM3U8)
	{
		PL_M3U8_Delete(pstM3U8);
	}
	return eHomResult;
}

STATIC HOM_Result_e _plstreaming_ModeHLS_GenerationLiveBitrateM3U8(PLStreaming_ModeInst_t *pstModeInst, const HCHAR *pRequestPath, PL_M3U8_t *pstM3U8)
{
	HERROR					hErr = ERR_OK;
	HINT32					nStart = 0, nEnd = 0;
	HINT32					nGapChunkCount = 0;
	HINT32					nBitrateKbps = 0;
	HOM_Result_e			eHomResult = eHOM_Result_Ok;
	PLMediaProfile_Item_t	*pstProfileItem = NULL;
	HxRingBuf_Status_t		stStatus;

	Handle_t				hProfileHandle = HANDLE_NULL;
	PL_StreamingKey_t		*pstStreamingKey = NULL;
	PLStreaming_SetupInfo_t	*pSetupInfo = NULL;

    if (pstModeInst == NULL)
    {
        HxLOG_Error("Invalid arg(pstInst is null)\n");
        return eHOM_Result_Error_Invalid_Arguments;
    }

    pSetupInfo  = &(pstModeInst->stSetupInfo);
	HxSTD_MemSet(&stStatus, 0, sizeof(HxRingBuf_Status_t));

    if(ePLStreaming_SetupStatus_None != pstModeInst->eSetupStatus)
	{
        hErr = PL_RINGBUF_GetBufferingStatus(pstModeInst->hRingbuf, &stStatus);
		if(ERR_OK != hErr)
		{
			HxLOG_Error("pM3U8Str error!! m3u8 check!!\r\n");
			return eHOM_Result_Error_Resource_Notfound;
		}
	}

	hErr = PL_MediaProfile_GetGapChunkCount(pSetupInfo->pstMediaProfile, &nGapChunkCount);
	if(ERR_OK != hErr)
	{
		HxLOG_Error("PL_MediaProfile_GetGapChunkCount error!! root m3u8 check!!\r\n");
		return eHOM_Result_Error_Resource_Notfound;
	}

	nStart	= stStatus.ulStartChunkId;
	nEnd	= stStatus.ulEndChunkId;

	if((nEnd - nStart) <= nGapChunkCount)
	{
		nEnd = nStart + nGapChunkCount;
	}
	else if((nEnd - nStart) > 10)
	{
		nStart++;
	}

    HxLOG_Print(HxANSI_COLOR_YELLOW("hStreamingId(0x%x), hRingbuf(%d), start(%d) end(%d) gap(%d)\n"), pstModeInst->hStreamingId, pstModeInst->hRingbuf, nStart, nEnd, nGapChunkCount);

	pstStreamingKey =  PL_StreamingKey_NewByString(pRequestPath);
	if(NULL == pstStreamingKey)
	{
		HxLOG_Error(" PL_StreamingKey_NewByString error!! pRequestPath(%s) !!\r\n", pRequestPath);
		return eHOM_Result_Error_Resource_Notfound;
	}

	eHomResult = PL_StreamingKey_GetItem(pstStreamingKey, NULL, NULL, &hProfileHandle, NULL);
	if(eHOM_Result_Ok != eHomResult || HANDLE_NULL == hProfileHandle || 0 == hProfileHandle)
	{
		PL_StreamingKey_Delete(pstStreamingKey);
		HxLOG_Error("PL_StreamingKey_GetItem fail!! \r\n");
		return eHOM_Result_Error;
	}

	eHomResult = PL_MediaProfile_Item_Get(hProfileHandle, &pstProfileItem);
	if(eHOM_Result_Ok != eHomResult)
	{
		PL_StreamingKey_Delete(pstStreamingKey);
		HxLOG_Error("PL_MediaProfile_Item_Get fail!! \r\n");
		return eHOM_Result_Error;
	}

	PL_M3U8_AppendTargetDuration(pstM3U8, 10);
	PL_M3U8_AppendMediaSequence(pstM3U8, nStart);

    nBitrateKbps = (TRUE == pstModeInst->stSetupInfo.bIsIncludeVideo) ? pstProfileItem->nVideoBitrateKbps : pstProfileItem->nAudioBitrateKbps;

    if(0 == nStart)
    {
        PL_M3U8_AppendInf(pstM3U8, 4, NULL); // 2013/12/26 mhkang: iOS7에서 첫번재 TS요청할 때, 링버퍼에 TS가 준비되지 않으면 4초동안 기다리다가 timeout처리한다.
        PL_M3U8_AppendTs(pstM3U8, PL_StreamingKey_GetBuffer(pstStreamingKey), nBitrateKbps, nStart);
        nStart++;
    }

	PL_M3U8_AppendInfWithTS(pstM3U8, 2, NULL, PL_StreamingKey_GetBuffer(pstStreamingKey), nBitrateKbps, nStart, nEnd);

	/* bitrate에서 hls는 setting */
    pstModeInst->nBitrate 				= nBitrateKbps;
    pstModeInst->pstMediaProfileItem	= pstProfileItem;
    pstModeInst->Mode.stHLS.pstBitrateStreamingKey 	= pstStreamingKey;
    pstModeInst->Mode.stHLS.hProfileHandle			= hProfileHandle;

	return eHOM_Result_Ok;
}

STATIC HOM_Result_e _plstreaming_ModeHLS_GenerationPvrBitrateM3U8(PLStreaming_ModeInst_t *pstModeInst, const HCHAR *pRequestPath, PL_M3U8_t *pstM3U8)
{
    HINT32					nStart = 0, nEnd = 0;
    HINT32					nBitrateKbps = 0;
    HOM_Result_e			eHomResult = eHOM_Result_Ok;
    PLMediaProfile_Item_t	*pstProfileItem = NULL;
    Handle_t				hProfileHandle = HANDLE_NULL;
    PL_StreamingKey_t		*pstStreamingKey = NULL;
    HINT32                  RecDuration = 0;

    if (pstModeInst == NULL)
    {
        HxLOG_Error("Invalid arg(pstInst is null)\n");
        return eHOM_Result_Error_Invalid_Arguments;
    }

    eHomResult = PL_StreamingContextUtil_GetRecDuration(pstModeInst->stSetupInfo.uUID, &RecDuration);
    if (eHomResult != eHOM_Result_Ok)
    {
        HxLOG_Error("PL_StreamingContextUtil_GetRecDuration(uid:%d,&Dur) failed\n", pstModeInst->stSetupInfo.uUID);
        return eHomResult;
    }

    //HxLOG_Print(HxANSI_COLOR_YELLOW("hStreamingId(0x%x), hRingbuf(%d), start(%d) end(%d)\n"), pstModeInst->hStreamingId, pstModeInst->hRingbuf, nStart, nEnd);

    pstStreamingKey =  PL_StreamingKey_NewByString(pRequestPath);
    if(NULL == pstStreamingKey)
    {
        HxLOG_Error(" PL_StreamingKey_NewByString error!! pRequestPath(%s) !!\n", pRequestPath);
        return eHOM_Result_Error_Resource_Notfound;
    }

    eHomResult = PL_StreamingKey_GetItem(pstStreamingKey, NULL, NULL, &hProfileHandle, NULL);
    if(eHOM_Result_Ok != eHomResult || HANDLE_NULL == hProfileHandle || 0 == hProfileHandle)
    {
        PL_StreamingKey_Delete(pstStreamingKey);
        HxLOG_Error("PL_StreamingKey_GetItem fail!! \n");
        return eHOM_Result_Error;
    }

    eHomResult = PL_MediaProfile_Item_Get(hProfileHandle, &pstProfileItem);
    if(eHOM_Result_Ok != eHomResult)
    {
        PL_StreamingKey_Delete(pstStreamingKey);
        HxLOG_Error("PL_MediaProfile_Item_Get fail!! \n");
        return eHOM_Result_Error;
    }

    nStart = 0;
    nEnd = (RecDuration / 2);  // 2015/1/25 mhkang:

    HxLOG_Info(HxANSI_COLOR_YELLOW("M3U8: nStart=%d nEnd=%d RecDur=%d\n"), nStart, nEnd, RecDuration);

    PL_M3U8_AppendTargetDuration(pstM3U8, 10);
    PL_M3U8_AppendAllowCacheNo(pstM3U8);  // 2015/1/29 mhkang: http://svn.humaxdigital.com:3000/issues/100201#note-7
    PL_M3U8_AppendMediaSequence(pstM3U8, nStart);

    nBitrateKbps = (TRUE == pstModeInst->stSetupInfo.bIsIncludeVideo) ? pstProfileItem->nVideoBitrateKbps : pstProfileItem->nAudioBitrateKbps;


	if(0 == nStart)
	{
		/* 2015/11/18 mhkang:  mhkang: 4초에서 8초로 늘린 이유:  http://svn.humaxdigital.com:3000/issues/107737#note-15
		  ios default player가 ios 버전업 될 때마다 특성이 달라짐.
		  ios 일때만 8초로 하고 그 이외는 2초로 하자(미수정)
		 */
		if( ePLStreamingClientType_Ios == pstModeInst->stSetupInfo.etClientType) /* iOS */
		{
			PL_M3U8_AppendInf(pstM3U8, 8, NULL);
		}
		else /* Other kinds of device */
		{
			PL_M3U8_AppendInf(pstM3U8, 4, NULL);
		}

		PL_M3U8_AppendTs(pstM3U8, PL_StreamingKey_GetBuffer(pstStreamingKey), nBitrateKbps, nStart);
		nStart++;
	}

    PL_M3U8_AppendInfWithTS(pstM3U8, 2, NULL, PL_StreamingKey_GetBuffer(pstStreamingKey), nBitrateKbps, nStart, nEnd);

    PL_M3U8_AppendEndList(pstM3U8);

    /* bitrate에서 hls는 setting */
    pstModeInst->nBitrate 				= nBitrateKbps;
    pstModeInst->pstMediaProfileItem	= pstProfileItem;
    pstModeInst->Mode.stHLS.pstBitrateStreamingKey 	= pstStreamingKey;
    pstModeInst->Mode.stHLS.hProfileHandle			= hProfileHandle;

    return eHOM_Result_Ok;
}


STATIC HOM_Result_e _plstreaming_ModeHLS_GetBitrateM3U8(PLStreaming_ModeInst_t *pstModeInst, const HCHAR *pRequestPath, HINT32 *pnM3U8Size, HUINT8 **ppucM3U8Buf)
{
	HxSTR_t			*pM3U8Str = NULL;
	HINT32			nSize = 0;
	HUINT8			*pBuffer = NULL;
	PL_M3U8_t 		*pstM3U8 = NULL;
	HOM_Result_e	eHomResult = eHOM_Result_Ok;

	pstM3U8 = PL_M3U8_New(PLM3U8_QUEUESIZE_INFINITY);
	if(NULL == pstM3U8)
	{
		HxLOG_Error("m3u8 create error!! mem check!!\r\n");
		return eHOM_Result_Error_OutofMem;
	}

#ifdef SUPPORT_PVR_HLS_SEEK
    if (pstModeInst->stSetupInfo.eStreamType == DxRSVSTREAM_TYPE_HLS_FILE_PVR)
    {
        eHomResult = _plstreaming_ModeHLS_GenerationPvrBitrateM3U8(pstModeInst, pRequestPath, pstM3U8);
    }
    else
#endif
    {
        eHomResult = _plstreaming_ModeHLS_GenerationLiveBitrateM3U8(pstModeInst, pRequestPath, pstM3U8);
    }

	if(eHOM_Result_Ok != eHomResult)
	{
		PL_M3U8_Delete(pstM3U8);
		HxLOG_Error("_plstreaming_ModeHLS_GenerationRootM3U8 fail!! \r\n");
		return eHOM_Result_Error_OutofMem;
	}

	pM3U8Str = PL_M3U8_ToBuffer(pstM3U8);
	PL_M3U8_Delete(pstM3U8);
	pstM3U8 = NULL;

	if(NULL == pM3U8Str)
	{
		HxLOG_Error("pM3U8Str error!! m3u8 check!!\r\n");
		return eHOM_Result_Error_Resource_Notfound;
	}

    HxLOG_Debug(HxANSI_COLOR_RED("eSetupStatus:%d hRingBuf:%x hResHandle:%p hStreamingId:%p\n"), pstModeInst->eSetupStatus, pstModeInst->hRingbuf, pstModeInst->hResHandle ,pstModeInst->hStreamingId);
#ifdef SUPPORT_PVR_HLS_SEEK
    if (pstModeInst->stSetupInfo.eStreamType == DxRSVSTREAM_TYPE_HLS_FILE_PVR)
    {
        // do nothing
    }
    else
#endif
    {
        if(ePLStreaming_SetupStatus_None == pstModeInst->eSetupStatus)
        {
            eHomResult = PL_StreamingContextUtil_MakeTuningParamAndDisplayString(pstModeInst);
            if(eHOM_Result_Ok != eHomResult)
            {
                HxLOG_Error("Error> PL_StreamingContextUtil_MakeTuningParamAndDisplayString\r\n");
                return eHomResult;
            }

            if (pstModeInst->hResHandle == HANDLE_NULL)
            {
                pstModeInst->hResHandle = PL_Streaming_Resource_New(pstModeInst->hStreamingId);
                if (pstModeInst->hResHandle == NULL)
                {
                    HxLOG_Error("PL_Streaming_Resource_New(hStreamingId:%d) failed\n");
                    return eHOM_Result_Error_Resource_Notfound;
                }
            }

            if(HANDLE_NULL == pstModeInst->hRingbuf || 0 == pstModeInst->hRingbuf)
            {
                pstModeInst->hRingbuf	= _plstreaming_ModeHLS_CreateRingbuf(pstModeInst);
                if(HANDLE_NULL == pstModeInst->hRingbuf || 0 == pstModeInst->hRingbuf)
                {
                    HxLOG_Error("Error> ringbuf create() failed!\r\n");
                    return eHOM_Result_Error;
                }
            }

            eHomResult = PL_Streaming_Resource_Start(pstModeInst->hResHandle, pstModeInst, 0);
            if(eHOM_Result_Ok != eHomResult)
            {
                HxLOG_Error("PL_Streaming_Resource_Start fail\r\n");
            }

        }
    }


	nSize 			= HLIB_STR_GetLength(pM3U8Str);
	pBuffer 		= HLIB_STR_toUTF8(pM3U8Str);
	*pnM3U8Size 	= nSize;
	*ppucM3U8Buf	= HLIB_STD_StrDup(pBuffer);
	HLIB_STR_Delete(pM3U8Str);
	pM3U8Str = NULL;

	return eHomResult;
}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_ModeHLS_Init(void)
{
	HOM_Result_e	eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;


	LEAVE_FUNCTION;

	return eHomResult;
}


/**
 *
 *
 *
 * @param
 * @return
 */
void PL_Streaming_ModeHLS_DeInit(void)
{
	ENTER_FUNCTION;


	LEAVE_FUNCTION;
}


/**
 *
 *
 *
 * @param
 * @return
 */
PLStreaming_ModeInst_t *PL_Streaming_ModeHLS_SetupInstance(PLStreaming_ModeInst_t *pstInst)
{
	PLStreaming_SetupInfo_t		*pSetupInfo = NULL;
	Handle_t					hProfileHandle = HANDLE_NULL;

	ENTER_FUNCTION;

	if(NULL == pstInst)
	{
		HxLOG_Error("Invalid_Arguments pstSetup(0x%x)\r\n", pstInst);
		return NULL;
	}
	pSetupInfo 	= (&pstInst->stSetupInfo);

	pstInst->eSetupStatus = ePLStreaming_SetupStatus_None;
	if(TRUE == pSetupInfo->bNotUseResource)
	{
		return pstInst;
	}

	if( NULL == pstInst->pstStreamingKey)
	{
		if(NULL != pSetupInfo->pstMediaProfile)
		{
			PL_MediaProfile_GetHandle(pSetupInfo->pstMediaProfile, &hProfileHandle);
		}
		else
		{
			HxLOG_Warning(" NULL == pSetupInfo->pstMediaProfile!! \r\n");
		}

		pstInst->pstStreamingKey	=  PL_StreamingKey_New(pSetupInfo->pszPrefix, pSetupInfo->uUID, hProfileHandle, pSetupInfo->pszDeviceID);
		if(NULL == pstInst->pstStreamingKey)
		{
			HxLOG_Error(" PL_StreamingKey_New error!! \r\n");
			return NULL;
		}
	}

	LEAVE_FUNCTION;

	return pstInst;
}


/**
 *
 *
 *
 * @param
 * @return
 */
void 	PL_Streaming_ModeHLS_DeleteInstance(PLStreaming_ModeInst_t *pstInst)
{
	PLStreaming_ModeHLS_t		*pHLS = NULL;

	ENTER_FUNCTION;

	if(NULL == pstInst)
	{
		HxLOG_Error("Invalid_Arguments pstInst(0x%x) \r\n", pstInst);
		return ;
	}
	pHLS 		= (&pstInst->Mode.stHLS);

	if(FALSE == pstInst->stSetupInfo.bNotUseResource)
	{
		_plstreaming_ModeHLS_DestroyRingbuf(pstInst);

		if(pHLS->pstBitrateStreamingKey)
		{
			PL_StreamingKey_Delete(pHLS->pstBitrateStreamingKey);
			pHLS->pstBitrateStreamingKey = NULL;
		}

		PL_StreamingKey_Delete(pstInst->pstStreamingKey);
		pstInst->pstStreamingKey = NULL;
	}

	LEAVE_FUNCTION;

	return ;
}

/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_ModeHLS_ResConnect (PLStreaming_ModeInst_t *pstInst)
{
	HOM_Result_e  				eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	(void)pstInst;

	LEAVE_FUNCTION;

	return eHomResult;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_ModeHLS_ResConflict (PLStreaming_ModeInst_t *pstInst, HINT32 eNotifyType, HINT32 nRequestPid, HCHAR *fromProcessName)
{
	HUINT32				ulPid = 0;
	HOM_Result_e  		eHomResult = eHOM_Result_Ok;
	SAPI_NotifyType_e	eSapiNotifyType = (SAPI_NotifyType_e)eNotifyType;

	ENTER_FUNCTION;

	switch(eSapiNotifyType)
	{
	case eSAPI_NOTIFY_TYPE_ConflictScheduleAdded:
	case eSAPI_NOTIFY_TYPE_ScheduleRemoved:
		ulPid = HLIB_STD_GetPID();
		// TODO: sama에서 ready의 경우, 알아서 체크해서 conflict schedule로 오는 때, 이때 process는 "sama"가 될 것임. HLS를 유지해야하는데 stop 시킬것인가??
#if defined(CONFIG_LXC_ONLY) || defined(CONFIG_CHROOT_AND_LXC)
        if (nRequestPid != eSAPI_UNIQUE_PROCESS_Indicator_Homma)
#else
        if (ulPid != nRequestPid) // homma가 아니고,
#endif
		{
			eHomResult = eHOM_Result_Error_Conflict;
		}
		else
		{
			eHomResult = eHOM_Result_Ok;
		}
		break;

	default:
		eHomResult = eHOM_Result_Ok;
		break;
	}

	if (eHomResult != eHOM_Result_Ok)
	{
		HxLOG_Error("Error> conflict! pstInst(0x%x), (ulPid:%d, nRequestPid:%d)\r\n", pstInst, ulPid, nRequestPid);
	}

	LEAVE_FUNCTION;

	return eHomResult;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e 	PL_Streaming_ModeHLS_GetM3U8(PLStreaming_ModeInst_t *pstInst, const HCHAR *pRequestPath, HUINT32 *pulDataSize, HUINT8 **ppucData)
{
	HINT32						ulM3U8BufSize = 0;
	HUINT8						*pM3U8Buf = NULL;
	HOM_Result_e  				eHomResult = eHOM_Result_Ok;
	PLStreaming_SetupInfo_t		*pSetupInfo = NULL;
	plStreaming_M3U8Type_e		eM3U8Type = plStreaming_M3U8Type_None;

	ENTER_FUNCTION;

	if(NULL == ppucData || NULL == pstInst || NULL == pRequestPath)
	{
		HxLOG_Error("Invalid_Arguments pstInst(0x%x) ppucData(0x%x) pRequestPath(%s)\r\n", pstInst, ppucData, pRequestPath);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	HxLOG_Print("Request Streaming Path (%s)\r\n", pRequestPath);

	pSetupInfo 	= (&pstInst->stSetupInfo);

	pM3U8Buf = NULL;
	eHomResult = eHOM_Result_Error;
	if(0 != pSetupInfo->uUID)
	{
		eM3U8Type = _plstreaming_ModeHLS_GetM3U8Type(pstInst, pRequestPath);
		switch(eM3U8Type)
		{
		case plStreaming_M3U8Type_Root:
			HxLOG_Print("plStreaming_M3U8Type_Root\r\n");
			eHomResult = _plstreaming_ModeHLS_GetRootM3U8(pstInst, pRequestPath, &ulM3U8BufSize, &pM3U8Buf);
			break;

		case plStreaming_M3U8Type_Bitrate:
			HxLOG_Print("plStreaming_M3U8Type_Bitrate\r\n");
			eHomResult = _plstreaming_ModeHLS_GetBitrateM3U8(pstInst, pRequestPath, &ulM3U8BufSize, &pM3U8Buf);
			break;

		default:
			break;
		}
	}


	if(NULL == pM3U8Buf || eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("plz Check Arguments pstInst(0x%x) ppucData(0x%x) pRequestPath(%s)\r\n", pstInst, ppucData, pRequestPath);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	*ppucData 		= pM3U8Buf;
	*pulDataSize 	= ulM3U8BufSize;

	LEAVE_FUNCTION;

	return eHOM_Result_Ok;
}



/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e 	PL_Streaming_ModeHLS_ReleaseM3U8(PLStreaming_ModeInst_t *pstInst, const HCHAR *pRequestPath, HUINT8 *pucData)
{
	ENTER_FUNCTION;

	if(NULL == pucData || NULL == pstInst || NULL == pRequestPath)
	{
		HxLOG_Error("Invalid_Arguments pstInst(0x%x) pucData(0x%x) pRequestPath(%s)\r\n", pstInst, pucData, pRequestPath);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	HxLOG_Print("Request Streaming Path (%s)\r\n", pRequestPath);

	HLIB_STD_MemFree(pucData);

	LEAVE_FUNCTION;

	return eHOM_Result_Ok;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e 	PL_Streaming_ModeHLS_GetInfo(PLStreaming_ModeInst_t *pstInst, PLStreaming_InfoType_e eInfoType, const HCHAR *pRequestPath, HUINT32 *pulDataSize, HCHAR **ppucData)
{

	ENTER_FUNCTION;

	(void)pstInst;
	(void)eInfoType;
	(void)pRequestPath;
	(void)pulDataSize;
	(void)ppucData;

	LEAVE_FUNCTION;

	return eHOM_Result_Ok;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e 	PL_Streaming_ModeHLS_ReleaseInfo(PLStreaming_ModeInst_t *pstInst, PLStreaming_InfoType_e eInfoType, const HCHAR *pRequestPath, HCHAR *pucData)
{
	ENTER_FUNCTION;

	(void)pstInst;
	(void)eInfoType;
	(void)pRequestPath;
	(void)pucData;

	if(NULL != pucData)
	{
		HLIB_STD_MemFree(pucData);
	}

	LEAVE_FUNCTION;

	return eHOM_Result_Ok;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e 	PL_Streaming_ModeHLS_GetTS(PLStreaming_ModeInst_t *pstModeInst,  const HCHAR *pRequestPath, HUINT32 *pulDataSize, HUINT8 **ppucData)
{
	HINT32 						nTsIndex  = 0;
	HOM_Result_e				eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

    if(NULL == ppucData || NULL == pstModeInst || NULL == pRequestPath)
	{
        HxLOG_Error("Invalid_Arguments pstInst(0x%x) ppucData(0x%x) pRequestPath(%s)\r\n", pstModeInst, ppucData, pRequestPath);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	HxLOG_Print("Request Streaming Path (%s)\r\n", pRequestPath);


    nTsIndex = _plstreaming_ModeHLS_GetTSIndex(pstModeInst, pRequestPath);
	if(nTsIndex < 0)
	{
        HxLOG_Error("_plstreaming_ModeHLS_GetTSIndex failed : pRequestPath("HxANSI_COLOR_YELLOW("%s")") , nTsIndex:%d, eSetupStatus:%d\n", pRequestPath, nTsIndex, pstModeInst->eSetupStatus);
		return eHOM_Result_Error;
	}

	HxLOG_Print("pRequestPath(%s) nTsIndex(%d)\r\n", pRequestPath, nTsIndex);

    pstModeInst->Mode.stHLS.RequestTS = nTsIndex;

#ifdef SUPPORT_PVR_HLS_SEEK
    if (pstModeInst->stSetupInfo.eStreamType == DxRSVSTREAM_TYPE_HLS_FILE_PVR)  // PVR HLS
    {
        eHomResult = _plstreaming_ModeHLS_ReadRingbuf_ForPVR(pstModeInst, nTsIndex, pulDataSize, ppucData);
    }
    else    // Live HLS
#endif
    {
        eHomResult = _plstreaming_ModeHLS_ReadRingbuf(pstModeInst, nTsIndex, pulDataSize, ppucData);
    }
	if(eHOM_Result_Ok != eHomResult)
	{
        HxLOG_Error("_plstreaming_ModeHLS_ReadRingbuf failed : pRequestPath("HxANSI_COLOR_YELLOW("%s")"), eHomResult(%d), eSetupStatus:%d\n", pRequestPath, eHomResult, pstModeInst->eSetupStatus);
		return eHOM_Result_Error;
	}

#if 0  // PVR HLS Seek Test
    {
	  // static int Cnt= 0;

        Cnt++;

        if (Cnt==10) // 1.5 x 10 = 15 second
        {
            HxLOG_Debug("**************************************\n");
            HxLOG_Debug("*       playPosition=30             *\n");
            HxLOG_Debug("**************************************\n");
            PL_Streaming_Resource_UpdateSchedulePosition(pstModeInst->hResHandle, 30, pstModeInst->hRingbuf);
            Cnt=0;
        }
    }
#endif
	LEAVE_FUNCTION;

	return eHomResult;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e 	PL_Streaming_ModeHLS_ReleaseTS(PLStreaming_ModeInst_t *pstInst, const HCHAR *pRequestPath, HUINT8 *pucData)
{
	ENTER_FUNCTION;

	if(NULL == pucData || NULL == pstInst || NULL == pRequestPath)
	{
		HxLOG_Error("Invalid_Arguments pstInst(0x%x) pucData(0x%x) pRequestPath(%s)\r\n", pstInst, pucData, pRequestPath);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	HxLOG_Print("Request Streaming Path (%s)\r\n", pRequestPath);

	HLIB_STD_MemFree(pucData);

	LEAVE_FUNCTION;

	return eHOM_Result_Ok;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e 	PL_Streaming_ModeHLS_ExecCmd(PLStreaming_ModeInst_t *pstInst,  const HCHAR *pRequestPath)
{
	HOM_Result_e				eHomResult = eHOM_Result_Ok;
//	Handle_t					hRingBuf = HANDLE_NULL;

	ENTER_FUNCTION;

	if(NULL == pstInst)//   || NULL == pRequestPath)
	{
		HxLOG_Error("Invalid_Arguments pstInst(0x%x) pRequestPath(%s)\r\n", pstInst, pRequestPath);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	// TODO: position 값을 넘겨 받던가 알아와야 함. 아직 어떤 형태로 받을지는 논의 전

	HxLOG_Print("pstInst->hRingbuf = %d, pRequestPath = %s\r\n", pstInst->hRingbuf, pRequestPath);

#if 0
	_plstreaming_ModeHLS_DestroyRingbuf(pstInst);
	hRingBuf = _plstreaming_ModeHLS_CreateRingbuf(pstInst);
	if(HANDLE_NULL == hRingBuf || 0 == hRingBuf)
	{
		PL_Streaming_ModeHLS_DeleteInstance(pstInst);
		HxLOG_Error("hRingBuf:0x%x error\r\n", hRingBuf);
		return eHOM_Result_Error_CreateFail;
	}

    eHomResult = PL_Streaming_Resource_UpdateSchedulePids(pstInst, pstInst->nPIDsCount, pstInst->usPIDs, pstInst->hRingbuf);
	if(eHOM_Result_Ok != eHomResult)
	{
		PL_Streaming_ModeHLS_DeleteInstance(pstInst);
		HxLOG_Error("PL_Streaming_Resource_UpdateSchedulePids error!\r\n");
		return eHomResult;
	}

	pstInst->hRingbuf = hRingBuf;
#endif

	{
		HERROR				hErr = ERR_OK;
		HCHAR				*pszPlayPosition = NULL;
		HCHAR 				*pszStreamingDupPath = NULL;
		HCHAR				*pszTokenPath = NULL;
		HxTOKEN_Handle_t	hToken = NULL;

		pszStreamingDupPath = HLIB_STD_StrDup(pRequestPath);
		if(NULL == pszStreamingDupPath)
		{
			HxLOG_Error("HLIB_STD_StrDup error!\r\n");
			return FALSE;
		}

		//http://192.168.50.11:9000/web/transcoding/148.m3u8?playPosition=1

		pszTokenPath = HLIB_STD_StrReplaceChar(pszStreamingDupPath, pRequestPath, '?', '&');

		hErr = HLIB_TOKEN_Parse(eTokenStrType_URL, pszTokenPath, &hToken);
		if(ERR_OK != hErr)
		{
			HxLOG_Print("pszTokenPath = %s, hErr = %d \r\n", pszTokenPath, hErr);
			HLIB_STD_MemFree(pszStreamingDupPath);
			HxLOG_Error("HLIB_TOKEN_Parse error!\r\n");
			return eHOM_Result_Error;
		}

		pszPlayPosition = HLIB_TOKEN_GetValueString(hToken, "playPosition");
		if(NULL != pszPlayPosition)
		{
			HUINT32		ulPlayPosition = 0;

			ulPlayPosition = HLIB_STD_StrToUINT32(pszPlayPosition);
			HxLOG_Print("pstInst->hRingbuf = %d, ulPlayPosition = %d \r\n", pstInst->hRingbuf, ulPlayPosition);

			eHomResult = PL_Streaming_Resource_UpdateSchedulePosition(pstInst->hResHandle, ulPlayPosition, pstInst->hRingbuf);
			if(eHOM_Result_Ok != eHomResult)
			{
				PL_Streaming_ModeHLS_DeleteInstance(pstInst);
				HxLOG_Error("PL_Streaming_Resource_UpdateSchedulePosition error!\r\n");
			}
		}
		else
		{
			HxLOG_Print("pstInst->hRingbuf = %d, pszPlayPosition is NULL\r\n", pstInst->hRingbuf);
		}

		HLIB_STD_MemFree(pszStreamingDupPath);

		if(NULL != hToken)
		{
			HLIB_TOKEN_FreeHandle(hToken);
		}

	}


	(void)pstInst;
	(void)pRequestPath;

	LEAVE_FUNCTION;

	return eHomResult;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e 	PL_Streaming_ModeHLS_StartInfoToSetupInfo(const HCHAR *pszPrefix, const PLStreaming_StartInfo_t *pstStartInfo, PLStreaming_SetupInfo_t *ret_pstSetupInfo)
{
	HOM_Result_e  				eHomResult = eHOM_Result_Ok;
	HUID						uUID = 0;
	PLStreaming_SetupInfo_t		*pSetupInfo = ret_pstSetupInfo;

	ENTER_FUNCTION;

	if(NULL == pstStartInfo || NULL == pSetupInfo || NULL == pstStartInfo->pszStreamingPath)
	{
		HxLOG_Error("Invalid_Arguments pstStartInfo(0x%x) pSetupInfo(0x%x) \r\n", pstStartInfo, pSetupInfo);
		return eHOM_Result_Error_Invalid_Arguments;
	}

#if 1
	HxLOG_DecIntDump(pstStartInfo->eStreamType);
	HxLOG_DecIntDump(pstStartInfo->eOutType);
	HxLOG_StringDump(pstStartInfo->pszDeviceName);
	HxLOG_StringDump(pstStartInfo->pszDeviceID);
	HxLOG_StringDump(pstStartInfo->pszUserAgent);
	HxLOG_StringDump(pstStartInfo->pszIPAddress);
	HxLOG_StringDump(pstStartInfo->pszStreamingPath);
#endif

	eHomResult = PL_StreamingContextUtil_GetUID(pstStartInfo, &uUID);
	if(eHOM_Result_Ok != eHomResult)
	{
		HxLOG_Error("PL_StreamingContextUtil_GetUID Fail \r\n");
		return eHOM_Result_Error_Invalid_Arguments;
	}

	if(0 == uUID)
	{
		HxLOG_Error("0 == uUID fail, check uUID, path(%s) \r\n", pstStartInfo->pszStreamingPath);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	pSetupInfo->uUID = uUID;

	LEAVE_FUNCTION;

	return eHOM_Result_Ok;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e 	PL_Streaming_ModeHLS_GetProfileItemHandle(PLStreaming_ModeInst_t *pstInst, Handle_t *ret_phProfileItem)
{
	PLStreaming_ModeHLS_t		*pHLS = NULL;
#if 0
	Handle_t					hProfileHandle = HANDLE_NULL;
	HOM_Result_e  				eHomResult = eHOM_Result_Ok;
#endif

	ENTER_FUNCTION;

	if(NULL == pstInst || NULL == ret_phProfileItem)
	{
		HxLOG_Error("Invalid_Arguments pstInst(0x%x) ret_phProfileItem(%s)\r\n", pstInst, ret_phProfileItem);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	pHLS 		= (&pstInst->Mode.stHLS);

#if 0
	eHomResult = PL_StreamingKey_GetItem(pHLS->pstBitrateStreamingKey, NULL, NULL, &hProfileHandle, NULL);
	if(eHOM_Result_Ok != eHomResult || HANDLE_NULL == hProfileHandle || 0 == hProfileHandle)
	{
		HxLOG_Error("PL_StreamingKey_GetItem fail!! \r\n");
		return eHOM_Result_Error;
	}
#endif
	*ret_phProfileItem = pHLS->hProfileHandle;


	LEAVE_FUNCTION;

	return eHOM_Result_Ok;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_ModeHLS_GetPIDs(PLStreaming_ModeInst_t *pstInst, HINT32 *ret_pnPIDsCount, HUINT16 *ret_arrusPIDs)
{
	ENTER_FUNCTION;

	if(NULL == pstInst || NULL == ret_pnPIDsCount || NULL == ret_arrusPIDs)
	{
		HxLOG_Error("Invalid_Arguments pstInst(0x%x) ret_pnPIDsCount(0x%x) ret_arrusPIDs(%s)\r\n", pstInst, ret_pnPIDsCount, ret_arrusPIDs);
		return eHOM_Result_Error_Invalid_Arguments;
	}

	*ret_pnPIDsCount = 1;
	ret_arrusPIDs[0] = 0x0000;

	LEAVE_FUNCTION;

	return eHOM_Result_Ok;
}

/**
* @date 2014/6/25
* @author mhkang
* @brief 이 함수는 Resource Task Thread에서 10 msec 주기로 호출되는 함수이다.
* Background로 해야할 작업이 있을때 이 함수에서 추가한다.
* 오랫동안 sem lock하는 코드는 작성하지 않아야 한다.
*
* @param
* @return
*/
HOM_Result_e PL_Streaming_ModeHLS_Task(plStreamingInst_t *pstInst)
{
    HOM_Result_e                eHomResult = eHOM_Result_Ok;
    HERROR                      hErr = ERR_OK;
    HxRingBuf_Status_t          stStatus;
    PLStreaming_ModeHLS_t		*pHLS = NULL;
    HUINT32                     uSamaSlot;

    if (pstInst == NULL)
    {
        HxLOG_Error("Invalid args(pstInst is null)\n");
        return eHOM_Result_Error_Invalid_Arguments;
    }

    if (pstInst->pstModeInst == NULL)
    {
        HxLOG_Error("Invalid args(pstInst->pstModeInst is null)\n");
        return eHOM_Result_Error_Invalid_Arguments;
    }

//    HxLOG_Debug(HxANSI_COLOR_PURPLE("hStreamingId:%p eSetupStatus:%s hResHandle:%x hRingBuf:%x\n"),
//           pstInst->pstModeInst->hStreamingId, PL_Streaming_Context_GetSetupStatusStr(pstInst->pstModeInst->eSetupStatus), pstInst->pstModeInst->hResHandle, pstInst->pstModeInst->hRingbuf);

    pHLS = (&pstInst->pstModeInst->Mode.stHLS);

    if (pstInst->pstModeInst->eSetupStatus == ePLStreaming_SetupStatus_Running)
    {
        hErr = PL_RINGBUF_GetBufferingStatus(pstInst->pstModeInst->hRingbuf, &stStatus);
        if(ERR_OK != hErr)
        {
            HxLOG_Error("PL_RINGBUF_GetBufferingStatus(hRingBuf:0x%x) failed(hErr:%d)\n", pstInst->pstModeInst->hResHandle, hErr);
            return eHOM_Result_Error_Resource_Notfound;
        }

        // Ringbuf StartIdx, EndIdx외 현재 TsIndex값을 보고 SAPI_CancelSchedule 호출해야하는지 결정한다.
        //HxLOG_Debug(HxANSI_COLOR_CYAN("[BUFFER INFO] StartTsIndex:%d, RingStart:%d ringEnd:%d TsIndex:%d\n"), pHLS->StartTsIndex, stStatus.ulStartChunkId, stStatus.ulEndChunkId, pHLS->RequestTS);

        if ((stStatus.ulEndChunkId - stStatus.ulStartChunkId >= (HLS_MAX_CHUNK - 2)) && (pHLS->RequestTS -  (pHLS->StartTsIndex +  stStatus.ulStartChunkId + 1) <= 0))
        {
            pstInst->pstModeInst->eSetupStatus = ePLStreaming_SetupStatus_HlsPausing;
            uSamaSlot = PL_Streaming_Resource_Delete(pstInst->pstModeInst->hResHandle, TRUE);
            if (uSamaSlot == SAMASLOT_INITVAL)
            {
                HxLOG_Error("PL_Streaming_Resource_Delete(hResHandle:%08x) failed\n");
            }
            pstInst->pstModeInst->hResHandle = HANDLE_NULL;
        }
    }


    return eHomResult;

}

/**********************************************************************************************************/
#define ____DEBUG_FUNCTIONS_______________________
/**********************************************************************************************************/

/**
 *
 *
 *
 * @param
 * @return
 */
void 	PL_Streaming_ModeHLS_Dump(PLStreaming_ModeInst_t *pstInst)
{
	if(NULL == pstInst)
	{
		HxLOG_Error("Invalid_Arguments pstInst(0x%x) \r\n", pstInst);
		return ;
	}
	HxLOG_Print("pstInst->pstModeInst->(&pstInst->Mode.stHLS)\n");
}

