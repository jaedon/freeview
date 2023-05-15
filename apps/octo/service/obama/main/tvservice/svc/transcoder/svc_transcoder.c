/**************************************************************
*	@file		mw_transcoder.c
*
*	http://www.humaxdigital.com
*
*	@data			2012/09/17
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

#include <octo_common.h>

#include <pal_transcoder.h>
#include <pal_ringbuf.h>
#include <pal_pipe.h>
#include <pal_sys.h>
#include <pal_pipe.h>
#include <pal_rec.h>
#include <svc_transcoder.h>


typedef enum
{
	eCMD_TRANS_OPEN = 0,
	eCMD_TRANS_START,
	eCMD_TRANS_STOP,
	eCMD_TRANS_CHANGEPID,
	eCMD_TRANS_CHANGEPROFILE,
	eCMD_TRANS_SET_PSITABLE,

	eCMD_TRANS_EVT_PROBE_FAIL,
	eCMD_TRANS_EVT_PROBE_SUCCESS,
	eCMD_TRANS_EVT_GET_SEQHEADER_INFO,

	eCMD_TRANS_MAX
} SvcTranscoder_Cmd_e;

typedef struct
{
	Handle_t			hAction;
	SvcTranscoder_Cmd_e	eCommand;
	ApiSyncMode_t	eApiSyncMode;
	void 			*pParam;
} SvcTranscoder_Msg_t;

typedef struct
{
	HBOOL					bStarted;
	HBOOL					bProbe;

	Handle_t 				hAction;
	HINT32					nUniqueId;
	Handle_t	 				hRingBuf;
	PalTransc_Setup_t	stSetupInfo;

} SvcTranscoder_Context_t;

typedef struct
{
	/*managerment*/
	HBOOL				bInitialize;
	HULONG				ulSyncSema;
	HULONG				ulCmdMsgQId;
	HULONG				ulCmdMsgQTaskId;

	/*contexts*/
	HUINT32				ulContextCnt;		/*number of Transcoder*/
	SvcTranscoder_Context_t	*pstContexts;

} SvcTranscoder_Status_t;

/*******************************************************************/
/********************      Static Variables     ***************************/
/*******************************************************************/
STATIC SvcTranscoder_Status_t		 s_stTranscoderStatus;
STATIC PostMsgToMgrCb_t				 s_fnSvcTranscoderPostMsgCb = NULL;

/*******************************************************************/
/********************      Macro     ***********************************/
/*******************************************************************/
#if defined(CONFIG_DEBUG)
#define	LOG_BEGIN							HxLOG_Debug	("%s(+)\n", __FUNCTION__)
#define	LOG_END								HxLOG_Debug	("%s(-)\n", __FUNCTION__)
#else
#define	LOG_BEGIN							Hx_NULL_PRINT
#define	LOG_END								Hx_NULL_PRINT
#endif


#define	DO_SYNCCALL(x) if(eSyncMode == x)  VK_SEM_Get(s_stTranscoderStatus.ulSyncSema)
/*******************************************************************/
/********************      Functions     ********************************/
/*******************************************************************/
#define ___INTERNAL_FUNCTIONS___
STATIC HBOOL svc_transcoder_SendMsg(SvcTranscoder_Cmd_e eCommand, Handle_t hAction, void *pParam, HINT32 nParamSize, ApiSyncMode_t eApiSyncMode);


#define __TR_STATUS__
STATIC SvcTranscoder_Status_t *svc_transcoder_GetStatus(void)
{
	return &s_stTranscoderStatus;
}

#define __TR_CONVERT__
STATIC PalTransc_EncodeMode_e svc_transcoder_ConvertSvc2PalEncodeMode ( SvcTranscoder_EncodeMode_e  eSvcTransEncodeMode)
{
	switch( eSvcTransEncodeMode )
	{
		case eSVC_TRANSCODER_ENCODEMODE_AV:		return ePAL_TRANSC_ENCODEMODE_AV;
		case eSVC_TRANSCODER_ENCODEMODE_AUDIO:	return ePAL_TRANSC_ENCODEMODE_AUDIO;
		default:	break;
	}
	return ePAL_TRANSC_ENCODEMODE_NONE;
}
STATIC PalTransc_OutputType_e svc_transcoder_ConvertSvc2PalOutputType ( SvcTranscoder_OutputType_e  eSvcTransOutputType)
{
	switch( eSvcTransOutputType )
	{
		case eSVC_TRANSCODER_OUTPUTTYPE_FILE:		return ePAL_TRANSC_OUTPUTTYPE_FILE;
		case eSVC_TRANSCODER_OUTPUTTYPE_BUFFER:	return ePAL_TRANSC_OUTPUTTYPE_BUFFER;
		default:	break;
	}
	return ePAL_TRANSC_OUTPUTTYPE_NONE;
}

#define __TR_CONTEXT__
STATIC HBOOL svc_transcoder_AllocContexts(const HUINT32 ulCount)
{
	if( (ulCount > 0) && (NULL == s_stTranscoderStatus.pstContexts ) )
	{
		s_stTranscoderStatus.pstContexts = (SvcTranscoder_Context_t*)HLIB_STD_MemAlloc( sizeof(SvcTranscoder_Context_t) * ulCount );
		if( s_stTranscoderStatus.pstContexts != NULL )
			return TRUE;
	}

	return FALSE;
}

STATIC SvcTranscoder_Context_t *svc_transcoder_GetContext(const HUINT32 ulIndex)
{
	if ( ulIndex < s_stTranscoderStatus.ulContextCnt )
	{
		if( s_stTranscoderStatus.pstContexts != NULL )
			return &(s_stTranscoderStatus.pstContexts[ulIndex]);
	}

	return NULL;
}

STATIC SvcTranscoder_Context_t* svc_transcoder_RegisterHandleToContext(const Handle_t hAction)
{
	HERROR				hResult;
	HUINT32 			ulActionId	= 	0;
	HUINT32		ulDeviceId	= 	RxRSCID_NULL;
	SvcTranscoder_Context_t	*pContext	=	NULL;

	ulActionId = PAL_PIPE_GetActionId( hAction );

	hResult = PAL_PIPE_GetResourceId(ulActionId, eRxRSCTYPE_TRANSCODER, &ulDeviceId );
	if( ERR_OK != hResult  )
	{
		HxLOG_Error (" PAL_PIPE_GetResourceId() failed : @ulActionId(%d), @RET(%d) \n", ulActionId, hResult);
		return NULL;
	}

	pContext = svc_transcoder_GetContext((HUINT32)ulDeviceId);
	if( NULL  == pContext )
	{
		HxLOG_Error (" can't get a context : @transcoderID (%d) \n", ulDeviceId);
		return NULL;
	}

	pContext->hAction = hAction;
	return pContext;
}

STATIC SvcTranscoder_Context_t* svc_transcoder_FindContextByHandle(const Handle_t hAction)
{
	HINT32 i=0;

	if( NULL == s_stTranscoderStatus.pstContexts  )
		return NULL;

	for( i=0 ; i<s_stTranscoderStatus.ulContextCnt ; i++ )
	{
		if( s_stTranscoderStatus.pstContexts[i].hAction  == hAction )
			return &s_stTranscoderStatus.pstContexts[i];
	}

	return NULL;
}

STATIC SvcTranscoder_Context_t* svc_transcoder_FindContextByUniqueId(HINT32	nUniqueId)
{
	HINT32 i=0;

	if( NULL== s_stTranscoderStatus.pstContexts )
		return NULL;

	for( i=0 ; i<s_stTranscoderStatus.ulContextCnt ; i++ )
	{
		if( s_stTranscoderStatus.pstContexts[i].nUniqueId  == nUniqueId )
			return &s_stTranscoderStatus.pstContexts[i];
	}

	return NULL;
}

STATIC void svc_transcoder_ResetContext(HUINT32 ulIndex)
{
	SvcTranscoder_Status_t *pMainStatus = svc_transcoder_GetStatus();

	if( NULL  != pMainStatus )
	{
		if ( ulIndex < pMainStatus->ulContextCnt )
		{
			if( NULL != pMainStatus->pstContexts )
			{
				/*reset*/
				pMainStatus->pstContexts[ulIndex].nUniqueId	=	-1;
				pMainStatus->pstContexts[ulIndex].hRingBuf	=	HANDLE_NULL;
				pMainStatus->pstContexts[ulIndex].bStarted		=	FALSE;
				pMainStatus->pstContexts[ulIndex].bProbe		=	FALSE;
			}
		}
	}

	return;
}

#define __TR_CALLBACK__
STATIC void	svc_transcoder_HandleProbeEvent(HUINT32 ulEvent, HUINT32 ulDeviceId, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	HBOOL	bResult = TRUE;
	switch( ulEvent )
	{
		case ePAL_TRANSC_PROBE_EVT_SEQUENCE_HEADER_INFO:
			bResult = svc_transcoder_SendMsg(eCMD_TRANS_EVT_GET_SEQHEADER_INFO, HANDLE_NULL, (void*)&ulParam1, sizeof(HINT32), eAsyncMode);
			break;
		case ePAL_TRANSC_PROBE_EVT_PROBE_SUCCESS:
			bResult = svc_transcoder_SendMsg(eCMD_TRANS_EVT_PROBE_SUCCESS, HANDLE_NULL, (void*)&ulParam1, sizeof(HINT32), eAsyncMode);
			break;
		case ePAL_TRANSC_PROBE_EVT_PROBE_FAIL:
			bResult = svc_transcoder_SendMsg(eCMD_TRANS_EVT_PROBE_FAIL, HANDLE_NULL, (void*)&ulParam1, sizeof(HINT32), eAsyncMode);
			break;
		default :
			HxLOG_Error ("[EVENT] Unknown event(0x%X), ulInstance:0x%X\n", ulEvent, ulParam1);
			return;
	}
	if (bResult != TRUE)
	{
		HxLOG_Error("sendMsg failed. ulEvent(%d) \n",ulEvent);
	}

	return ;
}

STATIC void svc_transcoder_HandleRecMem(HUINT32 ulDeviceId, HUINT32 ulUniqueId,const void *pData, HUINT32 ulLength)
{
	HERROR 			hResult		=	ERR_OK;
	SvcTranscoder_Context_t	*pContext	=	svc_transcoder_FindContextByUniqueId(ulUniqueId);

	if( NULL != pContext  )
	{
		HUINT32 ulWrittenSize=0;

		if( HANDLE_NULL != pContext->hRingBuf )
		{
#if 0
			if(ulLength < 50000)
			{
				HCHAR 		pszFileName[256] = {0, };
				HxFILE_t	*fp = NULL;

				HxSTD_snprintf(pszFileName, 255, "HLS_memio_%d_%d_%d.ts", (HINT32)pContext->hRingBuf, ulLength, HLIB_STD_GetSystemTime());
				HxLOG_Warning("hRingBuf = %d, ulUniqueId = %d, %s, ulLength = %d\n", pContext->hRingBuf, ulUniqueId, pszFileName, ulLength);

				fp = HLIB_FILE_Open(pszFileName, "wb");
				if(fp)
				{
					HLIB_FILE_Write(fp, (HUINT8 *)pData, ulLength, 1);
					HLIB_FILE_Close(fp);
				}
			}
#endif
			hResult = PAL_RINGBUF_WriteData ( pContext->hRingBuf, ulLength, (HUINT8*)pData, &ulWrittenSize);
			if( ERR_OK == hResult )
			{
				HxLOG_Debug ("ulDeviceId(%d) ulUniqueId(%d) pData(%x) ulLength(%d) (+)\r\n", ulDeviceId, ulUniqueId, (HINT32)pData, ulLength);
			}
			else
			{
				HxLOG_Error ("Error SVC_RINGBUF_WriteData @RET : %d\n", hResult);
			}
		}
		else
		{
			HxLOG_Error ("Error> incorrect ringbuffer handle[hRingBuf:%d]\n", pContext->hRingBuf);
		}
	}
	return ;
}

#if defined(SUPPORT_IDX_RECMEM)
STATIC void svc_transcoder_HandleIdxRecMem(HUINT32 ulDeviceId, HUINT32 ulUniqueId, unsigned long long ulLength)
{
	HERROR 			hResult		=	ERR_OK;
	SvcTranscoder_Context_t	*pContext	=	svc_transcoder_FindContextByUniqueId(ulUniqueId);

	HxLOG_Error ("ulLength : %llx\n", ulLength);

	return ;
}
#endif
STATIC void	svc_transcoder_HandleRecEvent(HUINT32 ulEvent, HUINT32 ulDeviceId, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	switch( ulEvent )
	{
		case ePAL_REC_EVENT_TIMECODE_CHANGED:
			HxLOG_Debug ("[EVENT] TIMECODE_CHANGED p1:0x%X, p2:0x%X, p3:0x%X\n", ulEvent, ulParam1, ulParam2, ulParam3);
			break;

		case ePAL_REC_EVENT_DISKFULL:
			HxLOG_Debug ("[EVENT] DISKFULL p1:0x%X, p2:0x%X, p3:0x%X\n", ulEvent, ulParam1, ulParam2, ulParam3);
			break;

		case ePAL_REC_EVENT_STREAM_UNSCRAMBLED:
			HxLOG_Debug ("[EVENT] STREAM_UNSCRAMBLED p1:0x%X, p2:0x%X, p3:0x%X\n", ulEvent, ulParam1, ulParam2, ulParam3);
			break;

		case ePAL_REC_EVENT_STREAM_SCRAMBLED:
			HxLOG_Debug ("[EVENT] STREAM_SCRAMBLED p1:0x%X, p2:0x%X, p3:0x%X\n", ulEvent, ulParam1, ulParam2, ulParam3);
			break;

		case ePAL_REC_EVENT_DELAYED_RECORD_STATUS_CHANGED:
			HxLOG_Debug ("[EVENT] DELAYED_RECORD_STATUS_CHANGED p1:0x%X, p2:0x%X, p3:0x%X\n", ulEvent, ulParam1, ulParam2, ulParam3);
			break;

		default:
			HxLOG_Debug ("[EVENT] Unknown event(0x%X), ulInstance:0x%X\n", ulEvent, ulParam1);
			return;
	}

	/* send message queue */
	// N/A
	return ;
}


#define __TR_LOGIC__
STATIC HERROR  svc_transcoder_ProbeStart(SvcTranscoder_Context_t* pContext )
{
	HERROR hResult		=	ERR_OK;
	HUINT32 ulDeviceId	=	0;

	ulDeviceId = pContext->stSetupInfo.ulDeviceId;

	/*register probe event callback function*/
	hResult == PAL_TRANSC_RegisterHandleProbeEvent( ulDeviceId, svc_transcoder_HandleProbeEvent );
	if( ERR_OK != hResult  )
	{
		HxLOG_Error (" PAL_TRANSC_RegisterHandleProbeEvent(ulDeviceId : %d) failed : @RET(%d) \n", ulDeviceId, hResult);
		return hResult;
	}

	hResult = PAL_TRANSC_ProbeStart(ulDeviceId);
	if( ERR_OK != hResult  )
	{
		HxLOG_Error (" PAL_TRANSC_ProbeStart(ulDeviceId : %d) failed : @RET(%d) \n", ulDeviceId, hResult);
		return hResult;
	}

	return hResult;
}

STATIC HERROR  svc_transcoder_OpenTranscoder(PalTransc_Setup_t *pstDstSetting)
{
	HERROR hResult	= ERR_OK;

	if( NULL != pstDstSetting  )
	{
		HUINT32 ulDeviceId	=	pstDstSetting->ulDeviceId;
		HUINT32 ulRecDeviceId	=	pstDstSetting->ulRecDeviceId;

		hResult = PAL_TRANSC_Setup(ulDeviceId, pstDstSetting);
		if( ERR_OK != hResult  )
		{
			HxLOG_Error (" PAL_TRANSC_Setup() failed : @RET(%d) \n", hResult);
		}

		if( ePAL_TRANSC_OUTPUTTYPE_BUFFER == pstDstSetting->eTransOutputType  )
		{
			/*when it's buffer mode, register callback function*/
			hResult = PAL_TRANSC_RegisterHandleRecMem(ulRecDeviceId, svc_transcoder_HandleRecMem);
			if( ERR_OK != hResult  )
			{
				HxLOG_Error (" PAL_TRANSC_RegisterHandleRecMem() failed : @RET(%d) \n", hResult);
			}
#if defined(SUPPORT_IDX_RECMEM)
			hResult = PAL_TRANSC_RegisterHandleIdxRecMem(ulRecDeviceId, svc_transcoder_HandleIdxRecMem);
			if( ERR_OK != hResult  )
			{
				HxLOG_Error (" PAL_TRANSC_RegisterHandleIdxRecMem() failed : @RET(%d) \n", hResult);
			}
#endif
		}

		/*register rec callback function*/
		PAL_REC_RegisterNotifier (ulRecDeviceId, svc_transcoder_HandleRecEvent);
	}
	else
	{
		hResult = ERR_FAIL;
		HxLOG_Error (" PAL_TRANSC_Setup() failed : @RET(%d) \n",  hResult);
	}

	return hResult;
}

STATIC HERROR  svc_transcoder_StartTranscoder(SvcTranscoder_Context_t* pContext )
{
	HERROR hResult		=	ERR_OK;
	HUINT32 ulDeviceId	=	0;
	HUINT32 ulUniqueId	=	0;

	ulDeviceId = pContext->stSetupInfo.ulDeviceId;

	hResult = PAL_TRANSC_Start(ulDeviceId, &ulUniqueId);
	if( ERR_OK != hResult  )
	{
		HxLOG_Error (" PAL_TRANSC_Start(ulDeviceId : %d) failed : @RET(%d) \n", ulDeviceId, hResult);
		return hResult;
	}

	/*save uniqueid*/
	pContext->nUniqueId = ulUniqueId;

	return hResult;
}

STATIC HERROR  svc_transcoder_StopTranscoder(SvcTranscoder_Context_t* pContext )
{
	HERROR hResult		=	ERR_OK;
	HUINT32 ulDeviceId	=	0;
	HUINT32 ulRecDeviceId	=	0;

	ulDeviceId = pContext->stSetupInfo.ulDeviceId;
	ulRecDeviceId = pContext->stSetupInfo.ulRecDeviceId;

	/*when it's buffer mode, unregister callback function*/
	if( ePAL_TRANSC_OUTPUTTYPE_BUFFER == pContext->stSetupInfo.eTransOutputType )
	{
		hResult = PAL_TRANSC_UnRegisterHandleRecMem(ulRecDeviceId, svc_transcoder_HandleRecMem);
		if( ERR_OK != hResult  )
		{
			HxLOG_Error (" PAL_TRANSC_RegisterHandleRecMem() failed : @RET(%d) \n", hResult);
			return hResult;
		}
#if defined(SUPPORT_IDX_RECMEM)
		hResult = PAL_TRANSC_UnRegisterHandleIdxRecMem(ulRecDeviceId, svc_transcoder_HandleIdxRecMem);
		if( ERR_OK != hResult  )
		{
			HxLOG_Error (" PAL_TRANSC_UnRegisterHandleIdxRecMem() failed : @RET(%d) \n", hResult);
			return hResult;
		}
#endif
	}

	/*stop*/
	hResult = PAL_TRANSC_Stop(ulDeviceId);
	if( ERR_OK != hResult )
	{
		HxLOG_Error ("PAL_TRANSC_Stop(ulDeviceId : %d) failed : @RET(%d) \n",  ulDeviceId, hResult);
		return hResult;
	}

	/*unregister rec callback function*/
	PAL_REC_UnregisterNotifier (ulRecDeviceId, svc_transcoder_HandleRecEvent);

	return hResult;
}

#define __TR_TASK_COMMAND__


STATIC HERROR  svc_transcoder_CmdPostEvent(SvcTranscoder_Context_t* pContext, HINT32 nEvtType)
{
	if( NULL == pContext )
		return ERR_FAIL;

	if (s_fnSvcTranscoderPostMsgCb)
	{
		(void)(* s_fnSvcTranscoderPostMsgCb)((HINT32)nEvtType, (Handle_t)pContext->hAction, (HINT32)pContext->hRingBuf, (HINT32)pContext->nUniqueId, (HINT32)0);
	}

	return ERR_OK;
}

STATIC HERROR  svc_transcoder_CmdOpen(Handle_t hAction, void *pParam )
{
	HERROR 					hResult		=	ERR_OK;
	SvcTranscoder_Context_t			*pContext	=	NULL;
	SvcTranscoder_Settings_t	*pstSettings	=	NULL;

	pstSettings =	(SvcTranscoder_Settings_t *)pParam;
	if( NULL  == pstSettings )
		return ERR_FAIL;

	pContext = svc_transcoder_RegisterHandleToContext(hAction);
	if( NULL == pContext  )
		return ERR_FAIL;

	/*setting context*/
	{
		pContext->hRingBuf = pstSettings->hRingBuf;
	}

	/*setting transcode information*/
	{
		/*set env*/
		switch(pstSettings->eTransSourceType)
		{
			case eSVC_TRANSCODER_SRCTYPE_Tsr:
			case eSVC_TRANSCODER_SRCTYPE_Pvr:
			case eSVC_TRANSCODER_SRCTYPE_Media:
			case eSVC_TRANSCODER_SRCTYPE_Ip:
				pContext->stSetupInfo.eTransSourceType	=	ePAL_TRANSC_SRCTYPE_PVR;
				break;
			case eSVC_TRANSCODER_SRCTYPE_Live:
			default:
				pContext->stSetupInfo.eTransSourceType	=	ePAL_TRANSC_SRCTYPE_LIVE;
				break;
		}


		pContext->stSetupInfo.eTransEncodeMode	=	svc_transcoder_ConvertSvc2PalEncodeMode(pstSettings->eTransEncodeMode);
		pContext->stSetupInfo.eTransOutputType	=	svc_transcoder_ConvertSvc2PalOutputType(pstSettings->eTransOutputType);

		// 당장 Src type을 보고 설정할 부분은 없음
		// 추후 로직 이 필요하면 추가하도록 함
#if 0
		switch(pstSettings->eTransSourceType)
		{
			case eSVC_TRANSCODER_SRCTYPE_Live:
			case eSVC_TRANSCODER_SRCTYPE_Tsr:
			case eSVC_TRANSCODER_SRCTYPE_Pvr:
			case eSVC_TRANSCODER_SRCTYPE_Media:
			case eSVC_TRANSCODER_SRCTYPE_Ip:
			default:
		}
#endif
		// Normal TS: TS Packet이 일반적인 188인가, HumaxPVR의 192인가.
		// 이는 결과물이 PVR 결과물인지 아니면 외부로 보낼 데이터인지에 따라 달라진다.
		// 여기서는 일단 eTransOutputType 을 보고 판단하기로 하고 추후 더 많은 요구 조건이 생길 때 따지기로 한다.
		switch(pstSettings->eTransOutputType)
		{
			case eSVC_TRANSCODER_OUTPUTTYPE_FILE:
				// 192 packet
				pContext->stSetupInfo.bTimestamp		=	TRUE;
				break;
			case eSVC_TRANSCODER_OUTPUTTYPE_BUFFER:
			default:
				// 188 packet
				pContext->stSetupInfo.bTimestamp		=	FALSE;
				break;
		}

		/*src*/
		pContext->stSetupInfo.usVideoPid				=	pstSettings->stAvPid.usVideoPid;
		pContext->stSetupInfo.usAudioPid				=	pstSettings->stAvPid.usAudioPid;
		pContext->stSetupInfo.usPcrPid					=	pstSettings->stAvPid.usPcrPid;
		pContext->stSetupInfo.eInputVideoFormat		=	pstSettings->eVideoCodec;
		pContext->stSetupInfo.eInputAudioFormat		=	pstSettings->eAudioCodec;

		/*resource id*/
		pContext->stSetupInfo.ulDemuxId				=	pstSettings->ulDemuxId;
		pContext->stSetupInfo.ulDeviceId			=	pstSettings->ulDeviceId;
		pContext->stSetupInfo.ulRecDeviceId			=	pstSettings->ulRecDeviceId;

		/*device setting*/
		pContext->stSetupInfo.bPcrTsPacketInsert		=	pstSettings->bPcrTsPacketInsert;

		/*set profile*/
		{
			HxSTD_memset(&pContext->stSetupInfo.stProfile, 0 ,sizeof(pContext->stSetupInfo.stProfile) );

			pContext->stSetupInfo.stProfile.nIndex = -1; //custom profile

			pContext->stSetupInfo.stProfile.eVideoCodec 		=	pstSettings->stProfile.eVideoCodec;
			pContext->stSetupInfo.stProfile.eAudioCodec 		=	pstSettings->stProfile.eAudioCodec;

			pContext->stSetupInfo.stProfile.nHdVideoBitrateKbps =	pstSettings->stProfile.nHdVideoBitrate_kbps;
			pContext->stSetupInfo.stProfile.nVideoBitrateKbps 	=	pstSettings->stProfile.nVideoBitrate_kbps;
			pContext->stSetupInfo.stProfile.nAudioBitrateKbps 	=	pstSettings->stProfile.nAudioBitrate_kbps;
			pContext->stSetupInfo.stProfile.nFrameRate 			=	pstSettings->stProfile.nFrameRate;
			pContext->stSetupInfo.stProfile.nScreenWidth 		=	pstSettings->stProfile.nScreenWidth;
			pContext->stSetupInfo.stProfile.nScreenHeight 		=	pstSettings->stProfile.nScreenHeight;
			pContext->stSetupInfo.stProfile.bInterlaceMode		=	pstSettings->stProfile.bIsInteraceMode;

			pContext->stSetupInfo.stProfile.eVideoAspectRatio	=	pstSettings->stProfile.eVideoAspectRatio;

			/*video-codec extension*/
			pContext->stSetupInfo.stProfile.eVideoCodecLevel		= 	pstSettings->stProfile.eVideoCodecLevel;
			pContext->stSetupInfo.stProfile.eVideoCodecProfile		=	pstSettings->stProfile.eVideoCodecProfile;
			pContext->stSetupInfo.stProfile.bUsingLiveAudioCodec	=	pstSettings->stProfile.bUsingLiveAudioCodec;
			pContext->stSetupInfo.stProfile.bUsingLiveAspectRatio	=	pstSettings->stProfile.bUsingLiveAspectRatio;
		}

		/*set profil-extension*/
		if( TRUE == pContext->stSetupInfo.stProfile.bUsingLiveAudioCodec )
		{
			pContext->stSetupInfo.stProfile.eAudioCodec	= pContext->stSetupInfo.eInputAudioFormat;
		}

		if( TRUE == pContext->stSetupInfo.stProfile.bUsingLiveAspectRatio )
		{
			/*will get it after probe*/
			pContext->bProbe =	TRUE;
		}


		/*setting encryption*/
		HxSTD_memset (&pContext->stSetupInfo.stCrypto, 0, sizeof(PalTransc_Crypto_t));
		pContext->stSetupInfo.stCrypto.eEncType = ePAL_TRANSC_CRYPTOTYPE_NONE;

		if( ePAL_TRANSC_OUTPUTTYPE_FILE == pContext->stSetupInfo.eTransOutputType )
		{
			/* Encryption of Transcoder is running if output-type is file */
			if (TRUE == pstSettings->bEncryption)
			{
				HUINT8  *pucKey = PAL_SYS_GetEncryptionKey();
				if (NULL != pucKey)
				{
					pContext->stSetupInfo.stCrypto.eEncType = ePAL_TRANSC_CRYPTOTYPE_3DES;
					HxSTD_memcpy (pContext->stSetupInfo.stCrypto.ucKey, pucKey, MAX_CRYPTO_KEY_LEN);
				}
			}

			HxSTD_memcpy(pContext->stSetupInfo.acuFilename, pstSettings->acuFilename, sizeof(pContext->stSetupInfo.acuFilename));
		}
	}

	hResult = svc_transcoder_OpenTranscoder(&pContext->stSetupInfo);
	return hResult;
}


STATIC HERROR  svc_transcoder_CmdStart(Handle_t hAction)
{
	HERROR hResult				=	ERR_OK;
	SvcTranscoder_Context_t* pContext	=	NULL;

	hResult = PAL_PIPE_IsActionHandleLatest(hAction);
	if( ERR_OK != hResult )
	{
		HxLOG_Error ("Version(0x%x)\n", hAction);
		return hResult;
	}

	pContext = svc_transcoder_FindContextByHandle(hAction);
	if( NULL == pContext )
		return ERR_FAIL;

	if( TRUE == pContext->bStarted )
	{
		HxLOG_Error (" transcoder(ulDeviceId : %d) already started \n", pContext->stSetupInfo.ulDeviceId);
		return ERR_FAIL;
	}

	hResult = PAL_PIPE_SetOutputResource (PAL_PIPE_GetActionId (pContext->hAction), ePAL_PIPE_OUTPUT_TRANSCODER, pContext->stSetupInfo.ulDeviceId);
	if (ERR_OK != hResult)
	{
		HxLOG_Error (" PAL_PIPE_SetOutputResource (ulDeviceId : %d) failed : @RET(%d) \n", pContext->stSetupInfo.ulDeviceId, hResult);
		return hResult;
	}

	if( TRUE == pContext->bProbe )
	{
		hResult = svc_transcoder_ProbeStart( pContext );
		if( ERR_OK != hResult )
		{
			svc_transcoder_CmdPostEvent( pContext, eSEVT_TRANSCODER_PROBE_FAILED );
			return hResult;
		}
		svc_transcoder_CmdPostEvent( pContext, eSEVT_TRANSCODER_PROBE_START );
	}
	else
	{
		hResult = svc_transcoder_StartTranscoder( pContext );
		if( ERR_OK != hResult )
		{
			svc_transcoder_CmdPostEvent( pContext, eSEVT_TRANSCODER_START_FAILED );
			return hResult;
		}

		/*start*/
		pContext->bStarted = TRUE;
		svc_transcoder_CmdPostEvent( pContext, eSEVT_TRANSCODER_STARTED );
	}

	return hResult;
}

STATIC HERROR  svc_transcoder_CmdStop(Handle_t hAction)
{
	HERROR hResult				=	ERR_OK;
	SvcTranscoder_Context_t* pContext	=	NULL;

	pContext = svc_transcoder_FindContextByHandle(hAction);
	if( NULL == pContext )
		return ERR_FAIL;

	hResult = svc_transcoder_StopTranscoder( pContext );
	if( ERR_OK != hResult )
	{
		svc_transcoder_CmdPostEvent( pContext, eSEVT_TRANSCODER_STOP_FAILED );
		return hResult;
	}
	svc_transcoder_CmdPostEvent( pContext, eSEVT_TRANSCODER_STOPED );

	svc_transcoder_ResetContext(pContext->stSetupInfo.ulDeviceId);
	return hResult;
}

STATIC HERROR  svc_transcoder_CmdChangePid(Handle_t hAction, void *pParam)
{
	SvcTranscoder_Context_t		*pContext	=	NULL;
	SvcTranscoder_AvPid_t	*pPids		=	NULL;

	pPids = (SvcTranscoder_AvPid_t *)pParam;
	if( NULL == pPids )
		return ERR_FAIL;

	pContext = svc_transcoder_FindContextByHandle(hAction);
	if( NULL == pContext  )
		return ERR_FAIL;

	return ERR_OK;
}

STATIC HERROR  svc_transcoder_CmdChangeProfile(Handle_t hAction, void *pParam)
{
	HERROR 				hResult		=	ERR_OK;
	SvcTranscoder_Context_t		*pContext	=	NULL;
	SvcTranscoder_Profile_t *pProfile 	=	NULL;
	HBOOL 					bChangeProfile 	=	FALSE;

	pProfile = (SvcTranscoder_Profile_t *)pParam;
	if( NULL == pProfile )
		return ERR_FAIL;

	pContext = svc_transcoder_FindContextByHandle(hAction);
	if( NULL  == pContext )
		return ERR_FAIL;


	/*check profile*/
	do{
		if( pContext->stSetupInfo.stProfile.eVideoCodec !=  pProfile->eVideoCodec )				{ bChangeProfile = TRUE; break; }
		if( pContext->stSetupInfo.stProfile.eAudioCodec !=  pProfile->eAudioCodec )				{ bChangeProfile = TRUE; break; }
		if( pContext->stSetupInfo.stProfile.nAudioBitrateKbps !=  pProfile->nAudioBitrate_kbps )	{ bChangeProfile = TRUE; break; }
		if( pContext->stSetupInfo.stProfile.nVideoBitrateKbps !=  pProfile->nVideoBitrate_kbps )	{ bChangeProfile = TRUE; break; }
		if( pContext->stSetupInfo.stProfile.nScreenWidth !=  pProfile->nScreenWidth )			{ bChangeProfile = TRUE; break; }
		if( pContext->stSetupInfo.stProfile.nScreenHeight !=  pProfile->nScreenHeight )			{ bChangeProfile = TRUE; break; }
		if( pContext->stSetupInfo.stProfile.nFrameRate !=  pProfile->nFrameRate )				{ bChangeProfile = TRUE; break; }
		if( pContext->stSetupInfo.stProfile.eVideoAspectRatio !=  pProfile->eVideoAspectRatio )	{ bChangeProfile = TRUE; break; }
		if( pContext->stSetupInfo.stProfile.bInterlaceMode	!=  pProfile->bIsInteraceMode )	{ bChangeProfile = TRUE; break; }
	}while(0);

	if( bChangeProfile == FALSE )
	{
		HxLOG_Error ("It's same profile information \n");
		return ERR_FAIL;
	}

	#if defined(CONFIG_DEBUG)
	//PL_MediaProfile_ItemFullNameDump((void *)&pContext->stSetupInfo.stProfile); // jyhwang - json 얻어오도록 수정해야 됨.
	#endif

	/*check stated flag*/
	if(pProfile->eVideoCodec)			pContext->stSetupInfo.stProfile.eVideoCodec 		= pProfile->eVideoCodec;
	if(pProfile->eAudioCodec)			pContext->stSetupInfo.stProfile.eAudioCodec	 		= pProfile->eAudioCodec;
	if(pProfile->nAudioBitrate_kbps)	pContext->stSetupInfo.stProfile.nAudioBitrateKbps 	= pProfile->nAudioBitrate_kbps;
	if(pProfile->nVideoBitrate_kbps)	pContext->stSetupInfo.stProfile.nVideoBitrateKbps 	= pProfile->nVideoBitrate_kbps;
	if(pProfile->nScreenWidth)			pContext->stSetupInfo.stProfile.nScreenWidth	 	= pProfile->nScreenWidth;
	if(pProfile->nScreenHeight)			pContext->stSetupInfo.stProfile.nScreenHeight	 	= pProfile->nScreenHeight;
	if(pProfile->nFrameRate)			pContext->stSetupInfo.stProfile.nFrameRate	 		= pProfile->nFrameRate;
	if(pProfile->eVideoAspectRatio)		pContext->stSetupInfo.stProfile.eVideoAspectRatio	= pProfile->eVideoAspectRatio;
	if(pProfile->bIsInteraceMode)		pContext->stSetupInfo.stProfile.bInterlaceMode		= pProfile->bIsInteraceMode;

	pContext->stSetupInfo.stProfile.nIndex		= -1;	//custom setting

	hResult = PAL_TRANSC_ChangeProfile( pContext->stSetupInfo.ulDeviceId, &pContext->stSetupInfo.stProfile);
	if( hResult != ERR_OK )
	{
		HxLOG_Error (" PAL_TRANSC_ChangeProfile() failed : @RET(%d) \n", hResult);
		return hResult;
	}

	return hResult;
}

STATIC HERROR  svc_transcoder_CmdSetPSITable(Handle_t hAction, void *pParam)
{
	HERROR 					hResult		=	ERR_OK;
	SvcTranscoder_Context_t			*pContext	=	NULL;
	SvcTranscoder_PSITable_t	*pPSITable	=	NULL;
	HUINT32 					ulDeviceId	=	0;
	PalTransc_PSI_t 		*pstPSI		=	NULL;

	pPSITable = (SvcTranscoder_PSITable_t *)pParam;
	if( NULL == pPSITable )
		return ERR_FAIL;

	pContext = svc_transcoder_FindContextByHandle(hAction);
	if( NULL  == pContext )
		return ERR_FAIL;

	pstPSI = (PalTransc_PSI_t*)HLIB_STD_MemAlloc( sizeof(PalTransc_PSI_t) );
	if( NULL == pstPSI )
		return ERR_FAIL;


	ulDeviceId = pContext->stSetupInfo.ulDeviceId;

	pstPSI->usPid = pPSITable->usPid;
	switch(pPSITable->eSiType)
	{
		case eSI_TableType_PAT: 	pstPSI->eTransPSITableType = ePAL_TRANSC_PSITABLE_PAT;	break;
		case eSI_TableType_PMT:	pstPSI->eTransPSITableType = ePAL_TRANSC_PSITABLE_PMT;	break;
		case eSI_TableType_SIT:		pstPSI->eTransPSITableType = ePAL_TRANSC_PSITABLE_SIT;	break;
		default :	HLIB_STD_MemFree(pstPSI); return ERR_FAIL;
	}

	/*raw section*/
	pstPSI->bContinue		=	pPSITable->bContinue;
	pstPSI->usPeriodTime	=	pPSITable->usPeriodTime;
	pstPSI->stPSIData.ulSize	=	pPSITable->ulRawSectionSize;
	pstPSI->stPSIData.usFlag	=	pPSITable->usFlag;

	HxSTD_memset( &pstPSI->stPSIData.aucRawSectionBuffer, 0,  sizeof( pstPSI->stPSIData.aucRawSectionBuffer) );
	HxSTD_memcpy( &pstPSI->stPSIData.aucRawSectionBuffer, &pPSITable->aucRawSectionBuffer, sizeof(pstPSI->stPSIData.aucRawSectionBuffer));

	hResult = PAL_TRANSC_SetPSI( ulDeviceId, pstPSI);
	if( ERR_OK != hResult )
	{
		HxLOG_Error (" PAL_TRANSC_SetPSI() failed : @RET(%d) \n", hResult);
	}

	HLIB_STD_MemFree(pstPSI);

	return hResult;
}

STATIC HERROR  svc_transcoder_CmdEvtProbeFail(void *pParam)
{
	HERROR 					hResult		=	ERR_OK;
	SvcTranscoder_Context_t			*pContext	=	NULL;
	HUINT32						ulDeviceId	=	*((HUINT32*)pParam);

	pContext = svc_transcoder_GetContext(ulDeviceId);
	if( NULL  == pContext )
		return ERR_FAIL;

	hResult= PAL_TRANSC_ProbeFail(ulDeviceId);
	if( ERR_OK != hResult )
	{
		HxLOG_Error (" PAL_TRANSC_ProbeFail() failed : @RET(%d) \n", hResult);
	}

	svc_transcoder_CmdPostEvent( pContext, eSEVT_TRANSCODER_PROBE_FAILED );

	return hResult;
}

STATIC HERROR  svc_transcoder_CmdEvtProbeSuccess(void *pParam)
{
	HERROR 	hResult		=	ERR_OK;
	SvcTranscoder_Context_t	*pContext	=	NULL;
	HUINT32	ulDeviceId	=	*((HUINT32*)pParam);
	HUINT32	ulFlags =	0;

	pContext = svc_transcoder_GetContext(ulDeviceId);
	if( NULL  == pContext )
		return ERR_FAIL;

	/*setting flags*/
	ulFlags =	0;
	if( TRUE == pContext->stSetupInfo.stProfile.bUsingLiveAspectRatio )
		ulFlags |= ePAL_TRANSC_PROBEMASK_ASPECTRATIO;

	hResult= PAL_TRANSC_ProbeApply(ulDeviceId, ulFlags);
	if( ERR_OK != hResult )
	{
		HxLOG_Error (" PAL_TRANSC_ProbeApply() failed : @RET(%d) \n", hResult);
	}

	hResult= PAL_TRANSC_ProbeStop(ulDeviceId);
	if( ERR_OK != hResult )
	{
		HxLOG_Error (" PAL_TRANSC_ProbeStop() failed : @RET(%d) \n", hResult);
	}

	hResult = svc_transcoder_StartTranscoder(pContext);
	if( ERR_OK != hResult )
	{
		HxLOG_Error (" svc_transcoder_StartTranscoder() failed : @RET(%d) \n", hResult);
	}

	pContext->bStarted = TRUE;
	svc_transcoder_CmdPostEvent( pContext, eSEVT_TRANSCODER_STARTED );

	return hResult;
}

STATIC HERROR  svc_transcoder_CmdEvtGetSeqheaderInfo(void *pParam)
{
	HERROR 					hResult		=	ERR_OK;
	SvcTranscoder_Context_t			*pContext	=	NULL;
	HUINT32						ulDeviceId	=	*((HUINT32*)pParam);

	pContext = svc_transcoder_GetContext(ulDeviceId);
	if( NULL  == pContext )
		return ERR_FAIL;

	return hResult;
}

#define __TR_TRANS_TASK__
STATIC void svc_transcoder_HandleMsg(SvcTranscoder_Msg_t  *pstMsg)
{
	HERROR hResult	= ERR_OK;

	if( NULL == pstMsg )
		return ;

	switch (pstMsg->eCommand)
	{
		case eCMD_TRANS_OPEN:
		{
			hResult =  svc_transcoder_CmdOpen(pstMsg->hAction, pstMsg->pParam);
			break;
		}
		case eCMD_TRANS_START:
		{
			hResult = svc_transcoder_CmdStart(pstMsg->hAction);
			break;
		}
		case eCMD_TRANS_STOP:
		{
			hResult = svc_transcoder_CmdStop(pstMsg->hAction);
			break;
		}
		case eCMD_TRANS_CHANGEPID:
		{
			hResult = svc_transcoder_CmdChangePid(pstMsg->hAction, pstMsg->pParam);
			break;
		}
		case eCMD_TRANS_CHANGEPROFILE:
		{
			hResult = svc_transcoder_CmdChangeProfile(pstMsg->hAction, pstMsg->pParam);
			break;
		}
		case eCMD_TRANS_SET_PSITABLE:
		{
			hResult = svc_transcoder_CmdSetPSITable(pstMsg->hAction, pstMsg->pParam);
			break;
		}
		case eCMD_TRANS_EVT_PROBE_FAIL:
		{
			hResult = svc_transcoder_CmdEvtProbeFail(pstMsg->pParam);
			break;
		}
		case eCMD_TRANS_EVT_PROBE_SUCCESS:
		{
			hResult = svc_transcoder_CmdEvtProbeSuccess(pstMsg->pParam);
			break;
		}
		case eCMD_TRANS_EVT_GET_SEQHEADER_INFO:
		{
			hResult = svc_transcoder_CmdEvtGetSeqheaderInfo(pstMsg->pParam);
			break;
		}
		default:
			HxLOG_Warning ("unknown command\n");
			break;
	}

	if( ERR_OK != hResult )
	{
		HxLOG_Warning ("command can't run! message(%d)\n", pstMsg->eCommand);
	}

	return;
}

STATIC HBOOL svc_transcoder_SendMsg(SvcTranscoder_Cmd_e eCommand, Handle_t hAction, void *pParam, HINT32 nParamSize, ApiSyncMode_t eApiSyncMode)
{
	SvcTranscoder_Status_t *pMainStatus = svc_transcoder_GetStatus();

	if( NULL != pMainStatus )
	{
		HINT32	nResult;
		SvcTranscoder_Msg_t stMsg;

		stMsg.eCommand = eCommand;
		stMsg.hAction = hAction;
		stMsg.eApiSyncMode = eApiSyncMode;

		if( (nParamSize > 0) && (pParam != NULL) )
		{
			stMsg.pParam = HLIB_STD_MemAlloc(nParamSize);
			HxSTD_memcpy( stMsg.pParam, pParam, nParamSize);
		}
		else
		{
			 stMsg.pParam = NULL;
		}

		nResult = VK_MSG_Send(pMainStatus->ulCmdMsgQId, &stMsg, sizeof(SvcTranscoder_Msg_t));
		if(nResult != VK_OK)
		{
			HxLOG_Warning ("VK_MSG_Send() failed : @RET(%d) \n", nResult);
			return FALSE;
		}
	}

	return TRUE;
}

STATIC void svc_transcoder_MsgQTask(void *pParam )
{
	HINT32 				nResult	=	0;
	SvcTranscoder_Msg_t 		stMsg	=	{0,};
	SvcTranscoder_Status_t 	*pMainStatus = svc_transcoder_GetStatus();

	while (1)
	{
		/* receive message */
		nResult = VK_MSG_Receive(pMainStatus->ulCmdMsgQId, &stMsg, sizeof(SvcTranscoder_Msg_t));
		if(nResult == VK_OK)
		{
			if(stMsg.eCommand < eCMD_TRANS_MAX) /* 메시지를 받음 */
			{
				svc_transcoder_HandleMsg(&stMsg);
				if( stMsg.pParam != NULL )
					HLIB_STD_MemFree(stMsg.pParam);
			}
			else
			{
				HxLOG_Warning ("\n");
			}

			if(stMsg.eApiSyncMode == eSyncMode)
			{
				VK_SEM_Release(pMainStatus->ulSyncSema);
			}
		}
	}

	return ;
}

#define __TR_INITALIZE__
STATIC HERROR svc_transcoder_InitContext(SvcTranscoder_Status_t *pMainStatus)
{
	HERROR hResult 			= 	ERR_OK;
	HUINT32 ulNumOfDevice	=	0;
	HINT32 i	=	0;

	HxSTD_memset(pMainStatus, 0 , sizeof(SvcTranscoder_Status_t) );

	hResult  = PAL_TRANSC_GetNumOfDevice(&ulNumOfDevice);
	if (hResult  != ERR_OK)
	{
		HxLOG_Error (" PAL_TRANSC_GetNumOfDevice() failed : @RET (%d) \n", (HINT32)hResult);
		return hResult ;
	}

	if( TRUE == svc_transcoder_AllocContexts(ulNumOfDevice) )
		pMainStatus->ulContextCnt = ulNumOfDevice;

	for( i=0 ; i<ulNumOfDevice ; i++ )
		svc_transcoder_ResetContext(i);

	return hResult ;
}

STATIC HERROR svc_transcoder_InitSystem(SvcTranscoder_Status_t *pMainStatus)
{
	HINT32 nResult = VK_OK;
	HERROR	hResult = ERR_OK;

	/* create semaphore for processing sync mode api */
	nResult = VK_SEM_Create(&pMainStatus->ulSyncSema, "TranscoderSyncSem", VK_SUSPENDTYPE_PRIORITY);
	if (nResult != VK_OK)
	{
		HxLOG_Error (" VK_SEM_Create() failed : @RET (%d) \n", (HINT32)nResult);
		return ERR_FAIL;
	}

	/* create message que */
	nResult = VK_MSG_Create(MW_PVR_MSGQ_SIZE, sizeof(SvcTranscoder_Msg_t), "TranscoderQ", &pMainStatus->ulCmdMsgQId, VK_SUSPENDTYPE_FIFO);
	if( nResult != VK_OK )
	{
		HxLOG_Error (" VK_MSG_Create() failed : @RET (%d) \n", (HINT32)nResult);
		return ERR_FAIL;
	}

	/* create task */
	nResult = VK_TASK_Create(	svc_transcoder_MsgQTask,
								MW_AV_TASK_PRIORITY,
								MW_AV_TASK_STACK_SIZE,
								"TranscoderT", NULL, &pMainStatus->ulCmdMsgQTaskId, 0);
	if( nResult != VK_OK )
	{
		HxLOG_Error (" VK_TASK_Create() failed : @RET (%d) \n", (HINT32)nResult);
		return ERR_FAIL;
	}

	return hResult;
}

STATIC HERROR svc_transcoder_InitProfile(SvcTranscoder_Status_t *pMainStatus)
{
	HERROR hResult = ERR_OK;

	//hResult = PL_MediaProfile_Init();	// jyhwang - json 얻어오도록 수정해야 됨.
	return hResult;
}


#define ___PUBLIC_FUNCTION___
HERROR	SVC_TRANSCODER_Init(PostMsgToMgrCb_t fnSvcTranscoderPostMsgCb)
{
	HERROR	hResult					= 	ERR_OK;
	SvcTranscoder_Status_t *pMainStatus	=	NULL;

	LOG_BEGIN;

	s_fnSvcTranscoderPostMsgCb = fnSvcTranscoderPostMsgCb;

	pMainStatus = svc_transcoder_GetStatus();
	if( NULL == pMainStatus )
		return ERR_OK;

	hResult = svc_transcoder_InitContext(pMainStatus);
	if( ERR_OK  != hResult)
		return hResult;


	hResult = svc_transcoder_InitSystem(pMainStatus);
	if( ERR_OK  != hResult)
		return hResult;

	hResult = svc_transcoder_InitProfile(pMainStatus);
	if( ERR_OK  != hResult)
		return hResult;


	/*save data*/
	pMainStatus->bInitialize = TRUE;

	/* 세마포를 get 해 놓아야 sync API가 동작한다. */
	VK_SEM_Get(pMainStatus->ulSyncSema);

	/*start task*/
	VK_TASK_Start(pMainStatus->ulCmdMsgQTaskId);

	LOG_END;
	return ERR_OK;
}

HERROR	SVC_TRANSCODER_Open(Handle_t hAction, SvcTranscoder_Settings_t *pSettings, ApiSyncMode_t eApiSyncMode)
{
	LOG_BEGIN;

	if( (HANDLE_NULL != hAction) && (NULL != pSettings) )
	{
		/*make msg*/
		svc_transcoder_SendMsg(	eCMD_TRANS_OPEN,
							hAction,
							(void*)pSettings,
							sizeof(SvcTranscoder_Settings_t), eApiSyncMode);


		DO_SYNCCALL(eApiSyncMode);
	}

	LOG_END;
	return ERR_OK;
}

HERROR	SVC_TRANSCODER_StartService(Handle_t hAction, ApiSyncMode_t eApiSyncMode)
{
	LOG_BEGIN;

	if( HANDLE_NULL != hAction )
	{
		/*make msg*/
		svc_transcoder_SendMsg(	eCMD_TRANS_START,
							hAction,
							NULL,
							0, eApiSyncMode);

		DO_SYNCCALL(eApiSyncMode);
	}

	LOG_END;
	return ERR_OK;
}

HERROR	SVC_TRANSCODER_StopService(Handle_t hAction, ApiSyncMode_t eApiSyncMode)
{
	LOG_BEGIN;

	if( HANDLE_NULL != hAction )
	{
		/*make msg*/
		svc_transcoder_SendMsg(	eCMD_TRANS_STOP,
							hAction,
							NULL,
							0, eApiSyncMode);

		DO_SYNCCALL(eApiSyncMode);
	}

	LOG_END;
	return ERR_OK;
}

HERROR	SVC_TRANSCODER_ChangePid(Handle_t hAction, SvcTranscoder_AvPid_t *pPids, ApiSyncMode_t eApiSyncMode)
{
	LOG_BEGIN;

	if( (HANDLE_NULL != hAction) && (NULL != pPids) )
	{
		/*make msg*/
		svc_transcoder_SendMsg(	eCMD_TRANS_CHANGEPID,
							hAction,
							(void*)pPids,
							sizeof(SvcTranscoder_AvPid_t), eApiSyncMode);

		DO_SYNCCALL(eApiSyncMode);
	}

	LOG_END;
	return ERR_OK;
}

HERROR	SVC_TRANSCODER_ChangeProfile(Handle_t hAction, SvcTranscoder_Profile_t *pProfile, ApiSyncMode_t eApiSyncMode)
{
	LOG_BEGIN;

	if( (HANDLE_NULL != hAction) && (NULL != pProfile) )
	{
		/*make msg*/
		svc_transcoder_SendMsg(	eCMD_TRANS_CHANGEPROFILE,
							hAction,
							(void*)pProfile,
							sizeof(SvcTranscoder_Profile_t), eApiSyncMode);


		DO_SYNCCALL(eApiSyncMode);
	}

	LOG_END;
	return ERR_OK;
}

HERROR	SVC_TRANSCODER_SetPSITable(Handle_t hAction, SvcTranscoder_PSITable_t *pTable, ApiSyncMode_t eApiSyncMode)
{
	LOG_BEGIN;

	if( (HANDLE_NULL != hAction) && (NULL != pTable) )
	{
		/*make msg*/
		svc_transcoder_SendMsg(	eCMD_TRANS_SET_PSITABLE,
							hAction,
							(void*)pTable,
							sizeof(SvcTranscoder_PSITable_t), eApiSyncMode);


		DO_SYNCCALL(eApiSyncMode);
	}

	LOG_END;
	return ERR_OK;
}

HERROR	SVC_TRANSCODER_RegisterPSITable(Handle_t hAction, SvcTranscoder_PSITable_t *pTable, ApiSyncMode_t eApiSyncMode)
{
	LOG_BEGIN;

	if( (HANDLE_NULL != hAction) && (NULL != pTable) )
	{
		/*set dummy-data*/
		pTable->ulRawSectionSize =	1;
		pTable->aucRawSectionBuffer[0] = 0xff;

		/*make msg*/
		svc_transcoder_SendMsg(	eCMD_TRANS_SET_PSITABLE,
							hAction,
							(void*)pTable,
							sizeof(SvcTranscoder_PSITable_t), eApiSyncMode);


		DO_SYNCCALL(eApiSyncMode);
	}

	LOG_END;
	return ERR_OK;
}
/*********************** End of File ******************************/
