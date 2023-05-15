/**************************************************************
*	@file		pal_transcoder.c
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

//DI
#include <di_demux.h>
#include <di_transcoder.h>

/*
   di_pvr.h 의 Transcoder 함수군이
   아래 Config로 막혀 있어서
   warring을 발생하기 때문에 아래 define을 선언
   */
#define CONFIG_TRANSCODER
#define CONFIG_TRANSCODER_MEMIO
#include <di_pvr.h>

//OCTO
#include <hlib.h>
#include <octo_common.h>
#include <pal_transcoder.h>

/*******************************************************************/
/********************      Internal Struct      ****************************/
/*******************************************************************/

//#define CONFIG_TRANS_CALLBACK_MSG

typedef struct
{
	HULONG			ulSemId;
	HUINT32			ulUniqueId;
	HBOOL			bStarted;
	HBOOL			bDemuxStarted;

	PalTransc_RecMemCallback_t	pfHandleRecMem;
	PalTransc_ProbeEventCallback_t 	pfHandleProbeEvent;
#if defined(SUPPORT_IDX_RECMEM)
	PalTransc_IdxRecMemCallback_t pfHandleIdxRecMem;
#endif
	PalTransc_Setup_t	stSetupInfo;
	PalTransc_PSI_t		astSetupPSIList[ePAL_TRANSC_PSITABLE_MAX];

	HBOOL	bProbed;
	HUINT32	ulProbeFlags;	/*적용할 Probe information flag*/
	DI_TRANSCODER_DEC_StreamInfo_t *pstProbeinfo;

} palTransc_Context_t;

typedef struct
{
	HUINT32		ulRecDeviceId;
	HUINT32		ulLength;
	void		*vpBuffer;
} palTransc_Msg_t;


typedef struct
{
	HULONG				ulMainSemId;
	HULONG				ulDiRecvTask;
	HULONG				ulDiRecvMsgQ;

	HUINT32				ulContextCnt; /*DI에서 지원하는 Transcoder 갯수*/
	palTransc_Context_t*	pstContexts;

} palTransc_Status_t;
/*******************************************************************/
/********************      Defines     **********************************/
/*******************************************************************/
#define palTransc_MAX_TRANSCODER_CNT		16	// 실제 갯수는 아님. (DI capablility 얻어오기 위한 배열 선언을 위해 서 사용
#define palTransc_DEFAULT_REFRESH_RATE		50000
#define palTransc_DEFAULT_INTERLACED		FALSE
#define palTransc_TRANSCODER_ID_MAIN		0
#define palTransc_TRANSCODER_ID_SUB			1
/*******************************************************************/
/********************      Static Variables     ***************************/
/*******************************************************************/
STATIC palTransc_Status_t		s_stPAL_TransStatus;
/*******************************************************************/
/********************      Macro *************************************/
/*******************************************************************/
#define	palTransc_ENTER_INST				HxLOG_Assert(s_stPAL_TransStatus.ulMainSemId);	HxSEMT_Get(s_stPAL_TransStatus.ulMainSemId)
#define	palTransc_LEAVE_INST				HxSEMT_Release(s_stPAL_TransStatus.ulMainSemId)
#define	palTransc_ENTER_CTX_INST(pstInst)	HxLOG_Assert(pstInst->ulSemId);	HxSEMT_Get(pstInst->ulSemId)
#define	palTransc_LEAVE_CTX_INST(pstInst)	HxSEMT_Release(pstInst->ulSemId)
#define	palTransc_ENTER_CTX_INST_TIMEOUT(pstInst)	HxSEMT_GetTimeout(pstInst->ulSemId, 40)

#if defined(CONFIG_DEBUG)
#define	LOG									HxLOG_Print
#define	ERR_LOG								HxLOG_Error
#define	TRA_LOG								HxLOG_Trace
#define	WAR_LOG								HxLOG_Warning
#define	LOG_BEGIN							HxLOG_Debug	("(+)\n")
#define	LOG_END								HxLOG_Debug	("(-)\n")
#else
#define	LOG									Hx_NULL_PRINT
#define	ERR_LOG								Hx_NULL_PRINT
#define	TRA_LOG								Hx_NULL_PRINT
#define	WAR_LOG								Hx_NULL_PRINT
#define	LOG_BEGIN							Hx_NULL_PRINT
#define	LOG_END								Hx_NULL_PRINT
#endif

#define	palTransc_TRACE_TIME_CHECK(x)			//HxLOG_Print("[%s:%d] Transcoder(%d) Time Tick(%d)\r\n", __FUNCTION__, __LINE__, x, (HUINT32)VK_TIMER_GetSystemTick())

#define palTransc_CHECK_TRANS_PID(x)				(x >= 0 && x < PID_NULL)

#define CONFIG_TRANSCODER_FIXED_PCRPID	0x1FFE

/*******************************************************************/
/********************      Functions      *******************************/
/*******************************************************************/
#define ___INTERNAL_FUNCTIONS___

#if defined(CONFIG_DEBUG)
STATIC void pal_transc_debugPrintProbeInfo( DI_TRANSCODER_DEC_StreamInfo_t *pstStreaminfo );
STATIC void pal_transc_debugPrintSettingInfo( DI_TRANSCODER_Settings_t *pstDstSetInfo  );
STATIC void pal_transc_debugPrintContext( palTransc_Context_t *pContext  );
STATIC void pal_transc_commandRegister(void);
STATIC HINT32 pal_transc_commandProcess(void *szArgList);
#endif


STATIC void _pal_transc_printPalTransc_PSI_t(PalTransc_PSI_t *pstSetPSI, HCHAR *szFunc, HINT32 nLine)
{
#ifdef CONFIG_DEBUG
	HxLOG_Debug("-----------------------------------------------\n");
	HxLOG_Debug("\t[%s:%d]\n", szFunc, nLine);
	HxLOG_Debug("pstSetPSI->stPSIData.ulSize = %d\n", pstSetPSI->stPSIData.ulSize);
	HxLOG_Debug("pstSetPSI->usPid            = 0x%X\n", pstSetPSI->usPid);
	HxLOG_Debug("pstSetPSI->usPeriodTime     = %d\n", pstSetPSI->usPeriodTime);
	HxLOG_Debug("pstSetPSI->bContinue        = %d\n", pstSetPSI->bContinue);
	//HLIB_LOG_Dump(pstSetPSI->stPSIData.aucRawSectionBuffer, pstSetPSI->stPSIData.ulSize, 0, 0);
	HxLOG_Debug("-----------------------------------------------\n");
#endif
}

#define __TR_CONTEXT__
STATIC palTransc_Status_t *pal_transc_getStatus(void)
{
	return &s_stPAL_TransStatus;
}

STATIC palTransc_Context_t *pal_transc_getContext(const HUINT32 ulDeviceId)
{
	if ( ulDeviceId > s_stPAL_TransStatus.ulContextCnt )
		return NULL;

	if( NULL == s_stPAL_TransStatus.pstContexts )
		return NULL;

	return &(s_stPAL_TransStatus.pstContexts[ulDeviceId]);
}

STATIC palTransc_Context_t *pal_transc_getContextByRecDeviceId(const HUINT32 ulRecDeviceId)
{
	HINT32 i;

	if( NULL == s_stPAL_TransStatus.pstContexts )
		return NULL;

	for( i=0 ; i< s_stPAL_TransStatus.ulContextCnt ; i++ )
	{
		if( s_stPAL_TransStatus.pstContexts[i].stSetupInfo.ulRecDeviceId == ulRecDeviceId )
			return &(s_stPAL_TransStatus.pstContexts[i]);
	}

	return NULL;
}

STATIC void pal_transc_resetPSI(PalTransc_PSI_t *pstPSI)
{
	if( NULL != pstPSI  )
	{
		HxSTD_memset(pstPSI, 0, sizeof(PalTransc_PSI_t));
		pstPSI->usPid = PID_NULL;
	}

	return ;
}

STATIC void pal_transc_resetContext(palTransc_Context_t *pContext )
{
	if( NULL != pContext )
	{
		HINT32 i;
		pContext->bStarted = FALSE;
		pContext->bDemuxStarted = FALSE;
		pContext->pfHandleRecMem = NULL;

		pContext->bProbed = FALSE;
		pContext->ulProbeFlags = 0;
		pContext->pstProbeinfo = NULL;

		HxSTD_memset(&pContext->stSetupInfo, 0, sizeof(PalTransc_Setup_t));

		for( i=0 ; i<ePAL_TRANSC_PSITABLE_MAX ; i++ )
		{
			pal_transc_resetPSI(&pContext->astSetupPSIList[i]);
		}

	}

	return ;
}

#define __TR_UTIL__
STATIC HINT32 pal_transc_utilFindPSIListIndexbyPid ( HUINT16 usPid, PalTransc_PSI_t * pstSetupPSIList )
{
	if( NULL != pstSetupPSIList  )
	{
		HINT32 i;
		for( i=0 ; i<ePAL_TRANSC_PSITABLE_MAX ; i++ )
		{
			if( pstSetupPSIList[i].usPid == usPid )
			{
				return i;
			}
		}
	}

	return -1;
}

#define __TR_CONVERT__
STATIC DxVideoCodec_e pal_transc_convertDi2Pal_VideoFormat ( DI_VIDEO_FORMAT eDiVideoCodec)
{
	switch (eDiVideoCodec)
	{
		case DI_VIDEO_FORMAT_MPEG2:			return eDxVIDEO_CODEC_MPEG2;
		case DI_VIDEO_FORMAT_MPEG4Part2:	return eDxVIDEO_CODEC_MPEG4_PART2;
		case DI_VIDEO_FORMAT_H264:			return eDxVIDEO_CODEC_H264;
		case DI_VIDEO_FORMAT_VC1:			return eDxVIDEO_CODEC_VC1;
		default:
			break;
	}

	return eDxVIDEO_CODEC_UNKNOWN;
}

STATIC  DxAudioCodec_e  pal_transc_convertDi2Pal_AudioFormat (DI_AUDIO_FORMAT eDiAudioCodec)
{
	switch (eDiAudioCodec)
	{
		case DI_AUDFORMAT_MPEG:					return eDxAUDIO_CODEC_MPEG;
		case DI_AUDFORMAT_MP3:					return eDxAUDIO_CODEC_MP3;
		case DI_AUDFORMAT_AC3:					return eDxAUDIO_CODEC_DOLBY_AC3;
		case DI_AUDFORMAT_AC3_PLUS:				return eDxAUDIO_CODEC_DOLBY_AC3P;
		case DI_AUDFORMAT_PCM:					return eDxAUDIO_CODEC_PCM;
		case DI_AUDFORMAT_AAC:					return eDxAUDIO_CODEC_AAC;
		case DI_AUDFORMAT_AAC_LOAS:				return eDxAUDIO_CODEC_AAC_LOAS;
		case DI_AUDFORMAT_AAC_PLUS:				return eDxAUDIO_CODEC_AAC_PLUS;
		case DI_AUDFORMAT_AAC_PLUS_ADTS:		return eDxAUDIO_CODEC_AAC_PLUS_ADTS;
		case DI_AUDFORMAT_DTS:					return eDxAUDIO_CODEC_DTS;
		case DI_AUDFORMAT_DTS_HD:				return eDxAUDIO_CODEC_DTS_HD;
		case DI_AUDFORMAT_LPCM_DVD:				return eDxAUDIO_CODEC_LPCM_DVD;
		case DI_AUDFORMAT_LPCM_HD_DVD:			return eDxAUDIO_CODEC_LPCM_HD_DVD;
		case DI_AUDFORMAT_LPCM_BLUERAY:			return eDxAUDIO_CODEC_LPCM_BLUERAY;
		case DI_AUDFORMAT_WMA_STD:				return eDxAUDIO_CODEC_WMA_STD;
		case DI_AUDFORMAT_WMA_PRO:				return eDxAUDIO_CODEC_WMA_PRO;
		case DI_AUDFORMAT_PCMWAVE:				return eDxAUDIO_CODEC_PCMWAVE;
		case DI_AUDFORMAT_DRA:					return eDxAUDIO_CODEC_DRA;
		default:
			break;
	}

	return eDxAUDIO_CODEC_UNKNOWN;
}

STATIC PalTransc_ContainerType_e pal_transc_convertDi2Pal_MuxType ( DI_TRANSCODER_MUX_TYPE  eDiTransMuxType)
{
	switch( eDiTransMuxType )
	{
		case DI_TRANSCODER_MUX_TS:		return ePAL_TRANSC_CONTAINERTYPE_TS;
		case DI_TRANSCODER_MUX_MP4:		return ePAL_TRANSC_CONTAINERTYPE_MP4;
		case DI_TRANSCODER_MUX_ASF:		return ePAL_TRANSC_CONTAINERTYPE_ASF;
		default:	break;
	}

	return ePAL_TRANSC_CONTAINERTYPE_NONE;
}

STATIC DI_PVR_VIDEOTYPE_t pal_transc_convertPal2Di_PvrVideoType (DxVideoCodec_e ePalVideoCodec)
{
        switch (ePalVideoCodec)
        {
	        case eDxVIDEO_CODEC_MPEG1:			return eDI_PVR_VideoType_Mpeg1;
	        case eDxVIDEO_CODEC_MPEG2:			return eDI_PVR_VideoType_Mpeg2;
	        case eDxVIDEO_CODEC_MPEG4_PART2:		return eDI_PVR_VideoType_Mpeg4_Part2;
	        case eDxVIDEO_CODEC_H261:				return eDI_PVR_VideoType_H261;
	        case eDxVIDEO_CODEC_H263:				return eDI_PVR_VideoType_H263;
	        case eDxVIDEO_CODEC_H264:				return eDI_PVR_VideoType_H264;
	        case eDxVIDEO_CODEC_VC1:				return eDI_PVR_VideoType_Vc1;
	        case eDxVIDEO_CODEC_VC1_SIMPLEMAIN:	return eDI_PVR_VideoType_Vc1_Sm;
	        case eDxVIDEO_CODEC_AVS:				return eDI_PVR_VideoType_Avs;
	        case eDxVIDEO_CODEC_DIVX311:			return eDI_PVR_VideoType_Divx_311;
	        default:	break;
        }

        return eDI_PVR_VideoType_None;
}

STATIC DI_VIDEO_FORMAT pal_transc_convertPal2Di_VideoFormat (DxVideoCodec_e ePalVideoCodec)
{
	switch (ePalVideoCodec)
	{
		case eDxVIDEO_CODEC_MPEG2:				return DI_VIDEO_FORMAT_MPEG2;
		case eDxVIDEO_CODEC_MPEG4_PART2:			return DI_VIDEO_FORMAT_MPEG4Part2;
		case eDxVIDEO_CODEC_H264:					return DI_VIDEO_FORMAT_H264;
		case eDxVIDEO_CODEC_VC1:					return DI_VIDEO_FORMAT_VC1;
		default: break;
	}

	return DI_VIDEO_FORMAT_NONE;
}

STATIC DI_AUDIO_FORMAT pal_transc_convertPal2Di_AudioFormat (DxAudioCodec_e ePalAudioCodec)
{
	switch (ePalAudioCodec)
	{
		case eDxAUDIO_CODEC_MPEG:					return DI_AUDFORMAT_MPEG;
		case eDxAUDIO_CODEC_MP3:					return DI_AUDFORMAT_MP3;
		case eDxAUDIO_CODEC_DOLBY_AC3:			return DI_AUDFORMAT_AC3;
		case eDxAUDIO_CODEC_DOLBY_AC3P:			return DI_AUDFORMAT_AC3_PLUS;
		case eDxAUDIO_CODEC_PCM:					return DI_AUDFORMAT_PCM;
		case eDxAUDIO_CODEC_AAC:					return DI_AUDFORMAT_AAC;
		case eDxAUDIO_CODEC_AAC_LOAS:				return DI_AUDFORMAT_AAC_LOAS;
		case eDxAUDIO_CODEC_AAC_PLUS:				return DI_AUDFORMAT_AAC_PLUS;
		case eDxAUDIO_CODEC_AAC_PLUS_ADTS:		return DI_AUDFORMAT_AAC_PLUS_ADTS;
		case eDxAUDIO_CODEC_DTS:					return DI_AUDFORMAT_DTS;
		case eDxAUDIO_CODEC_DTS_HD:				return DI_AUDFORMAT_DTS_HD;
		case eDxAUDIO_CODEC_LPCM_DVD:				return DI_AUDFORMAT_LPCM_DVD;
		case eDxAUDIO_CODEC_LPCM_HD_DVD:			return DI_AUDFORMAT_LPCM_HD_DVD;
		case eDxAUDIO_CODEC_LPCM_BLUERAY:			return DI_AUDFORMAT_LPCM_BLUERAY;
		case eDxAUDIO_CODEC_WMA_STD:				return DI_AUDFORMAT_WMA_STD;
		case eDxAUDIO_CODEC_WMA_PRO:				return DI_AUDFORMAT_WMA_PRO;
		case eDxAUDIO_CODEC_PCMWAVE:				return DI_AUDFORMAT_PCMWAVE;
		case eDxAUDIO_CODEC_DRA:					return DI_AUDFORMAT_DRA;
		default: break;
	}

	return DI_AUDFORMAT_NONE;
}


/*
	겔탭 10.1에서는 Interlace 방식으로 인코딩 한 HLS 데이터를 재생하지 못하는 것으로 확인 되었습니다.
	기존에는 driver에서 강제로 Progressive 방식으로 셋팅했었는데, 논리적으로 맞지 않아 정리 과정하면서 문제가 생겼습니다.
	겔탭 8.9의 경우에는 Interlace와 Progressive 방식 둘 다 지원해서 재생이 잘 되었던 것입니다.
	같은 삼성에서 만든 제품간에도 칩에 대해 호환성이 달라서 발생한 이슈로 확인 되었습니다.

	by jyhwang, 2013-02-25까지 수정 예정
*/

STATIC DI_VIDEO_HD_RESOLUTION pal_transc_convertPal2Di_Resolution (HUINT32 ulHeightResolution, HBOOL isInterlace)
{
	if(isInterlace == FALSE)
	{
		switch (ulHeightResolution)
		{
			case 480:	return DI_VIDEO_HD_RESOLUTION_480P;
			case 576:	return DI_VIDEO_HD_RESOLUTION_576P;
			case 720:	return DI_VIDEO_HD_RESOLUTION_720P;
			case 1080:	return DI_VIDEO_HD_RESOLUTION_1080P;
			default:	break;
		}

		return DI_VIDEO_HD_RESOLUTION_480P;
	}
	else
	{
		switch (ulHeightResolution)
		{
			case 480:	return DI_VIDEO_HD_RESOLUTION_480I;
			case 576:	return DI_VIDEO_HD_RESOLUTION_576I;
			case 720:	return DI_VIDEO_HD_RESOLUTION_720P;
			case 1080:	return DI_VIDEO_HD_RESOLUTION_1080I;
			default:	break;
		}

		return DI_VIDEO_HD_RESOLUTION_576I;
	}
}

STATIC DI_VIDEO_FrameRate_t pal_transc_convertPal2Di_FrameRate (HINT32 nFrameRate)
{
	switch (nFrameRate)
	{
		case 24:		return DI_VIDEO_FRAMERATE_24;
		case 25:		return DI_VIDEO_FRAMERATE_25;
		case 30:		return DI_VIDEO_FRAMERATE_30;
		case 50:		return DI_VIDEO_FRAMERATE_50;
		case 60:		return DI_VIDEO_FRAMERATE_60;
		default:	break;
	}

	return DI_VIDEO_FRAMERATE_25;

}

STATIC DI_TRANSCODER_SOURCE_TYPE pal_transc_convertPal2Di_SourceType (PalTransc_SourceType_e ePalTransSourceType)
{
	return DI_TRANSCODER_SOURCE_BACKGROUND;
}

STATIC DI_PVR_ENCRYPTION_TYPE pal_transc_convertPal2Di_EncodeType (PalTransc_CryptoType_t ePalTransEncodeType)
{
	switch( ePalTransEncodeType )
	{
		case ePAL_TRANSC_CRYPTOTYPE_3DES:				return DI_PVR_ENCRYPTION_TYPE_3DES;
		case ePAL_TRANSC_CRYPTOTYPE_AES:					return DI_PVR_ENCRYPTION_TYPE_AES;
		case ePAL_TRANSC_CRYPTOTYPE_CURRENT:				return DI_PVR_ENCRYPTION_TYPE_CURRENT;
		case ePAL_TRANSC_CRYPTOTYPE_3DES_SECURED_KEY:	return DI_PVR_ENCRYPTION_TYPE_3DES_SECURED_KEY;
		case ePAL_TRANSC_CRYPTOTYPE_AES_SECURED_KEY:		return DI_PVR_ENCRYPTION_TYPE_AES_SECURED_KEY;
		default:	break;
	}

	return DI_PVR_ENCRYPTION_TYPE_NONE;
}

STATIC DI_TRANSCODER_ENCODE_MODE pal_transc_convertPal2Di_EncodeMode (PalTransc_EncodeMode_e ePalTransEncodeMode)
{
	switch( ePalTransEncodeMode )
	{
		case ePAL_TRANSC_ENCODEMODE_VIDEO:	return DI_TRANSCODER_ENCODE_VIDEO;
		case ePAL_TRANSC_ENCODEMODE_AUDIO:	return DI_TRANSCODER_ENCODE_AUDIO;
		case ePAL_TRANSC_ENCODEMODE_AV:		return DI_TRANSCODER_ENCODE_AV;
		default:	break;
	}

	return DI_TRANSCODER_ENCODE_MAX;
}

STATIC DI_VIDEO_AspectRatio_t pal_transc_convertPal2Di_AspectRatio(DxAspectRatio_e eAspectRatio)
{
        switch (eAspectRatio)
        {
	        case eDxASPECTRATIO_4X3:	return DI_VIDEO_ASPECTRATIO_4_3;
	        case eDxASPECTRATIO_16X9:	return DI_VIDEO_ASPECTRATIO_16_9;
	        case eDxASPECTRATIO_1X1:	return DI_VIDEO_ASPECTRATIO_1_1;
	        case eDxASPECTRATIO_14X9:	return DI_VIDEO_ASPECTRATIO_14_9;
	        default:	break;
        }

        return DI_VIDEO_ASPECTRATIO_UNKNOWN;
}

#define __TR_SENDER__

static void pal_transc_sendToRingBuf(HUINT32	ulRecDeviceId, const void	*vpBuffer, HUINT32 	ulLength)
{
	palTransc_Context_t *pContext = NULL;
	HERROR	hErr = ERR_OK;
	HUINT32	ulDeviceId;
	HUINT32	ulUniqueId;
	PalTransc_RecMemCallback_t	pfHandleRecMem;

	palTransc_ENTER_INST;

	pContext = pal_transc_getContextByRecDeviceId(ulRecDeviceId);
	if( NULL == pContext  )
	{
		palTransc_LEAVE_INST;
		ERR_LOG ("(%d) No Context\n", ulRecDeviceId);
		return;
	}
	palTransc_LEAVE_INST;

	hErr = palTransc_ENTER_CTX_INST_TIMEOUT(pContext);
	if( ERR_OK != hErr )
	{
		ERR_LOG ("(%d) SEMT Timeout or Error! hErr(%d)\n", ulRecDeviceId, hErr);
		return;
	}

#if 0
	if (NULL == pContext->pfHandleRecMem )
	{
		palTransc_LEAVE_CTX_INST(pContext);
		ERR_LOG ("(%d) No Callback (0x%x)\n", ulRecDeviceId, pContext);
		return;
	}
#endif

	pfHandleRecMem	= pContext->pfHandleRecMem;
	ulUniqueId		= pContext->ulUniqueId;
	ulDeviceId		= pContext->stSetupInfo.ulDeviceId;

	palTransc_LEAVE_CTX_INST(pContext);

	if (NULL == pfHandleRecMem )
	{
		ERR_LOG ("(%d) No Callback (0x%x)\n", ulRecDeviceId, pContext);
		return;
	}


	pfHandleRecMem(ulDeviceId, ulUniqueId, vpBuffer, ulLength);

	LOG ("ulDeviceId(%d) ulRecDeviceId(%d) pal_buffer(%d) callback(0x%x) \n", ulDeviceId, ulUniqueId, ulLength, (HINT32)pfHandleRecMem);

	return;
}

#define __TR_TASK__

#if defined(CONFIG_TRANS_CALLBACK_MSG)
static void pal_transc_taskDiRecvSendRingBuf(void *param)
{
	HINT32				nResult = 0;
	palTransc_Msg_t		stTransMsg;
	palTransc_Status_t 	*pStatus = (palTransc_Status_t *)param;


	while (pStatus != NULL)
	{
		HxSTD_memset(&stTransMsg, 0, sizeof(palTransc_Msg_t));

		nResult = VK_MSG_Receive(pStatus->ulDiRecvMsgQ, &stTransMsg, sizeof(palTransc_Msg_t));
		if (VK_OK != nResult )
		{
			ERR_LOG ("VK_MSG_Receive ret:0x%X\n",nResult);
			continue;
		}

		palTransc_TRACE_TIME_CHECK(stTransMsg.ulRecDeviceId);

		pal_transc_sendToRingBuf(stTransMsg.ulRecDeviceId, stTransMsg.vpBuffer, stTransMsg.ulLength);
		if(stTransMsg.vpBuffer)
		{
			HLIB_STD_MemFree(stTransMsg.vpBuffer);
		}
	}
}
#endif /* CONFIG_TRANS_CALLBACK_MSG */

#define __TR_CALLBACK__
STATIC void pal_transc_HandleRecMem(int ulRecDeviceId, const void *pData, unsigned int ulLength)
{
#if defined(CONFIG_TRANS_CALLBACK_MSG)
	HINT32				nResult = 0;
	palTransc_Msg_t 	stMsg;
	palTransc_Status_t 	*pStatus = pal_transc_getStatus();

	stMsg.ulRecDeviceId	= ulRecDeviceId;
	stMsg.ulLength		= ulLength;
	stMsg.vpBuffer		= HLIB_STD_MemDup(pData, ulLength);

	nResult = VK_MSG_SendTimeout(pStatus->ulDiRecvMsgQ, &stMsg, sizeof(palTransc_Msg_t), 10);
	switch(nResult)
	{
	case VK_OK:
		palTransc_TRACE_TIME_CHECK(ulRecDeviceId);
		break;

	case VK_TIMEOUT:
		ERR_LOG ("VK_MSG_SendTimeout => VK_TIMEOUT!\n");
		break;

	default:
		ERR_LOG ("VK_MSG_SendTimeout => ERROR!\n");
		return;
	}
	return;
#else
	pal_transc_sendToRingBuf(ulRecDeviceId, pData, ulLength);
#endif
}

#if defined(SUPPORT_IDX_RECMEM)
STATIC void pal_transc_HandleIdxRecMem(HINT32 ulRecDeviceId, unsigned long long ul64Length)
{
	palTransc_Context_t *pContext = NULL;

	HUINT32	ulDeviceId;
	HUINT32	ulUniqueId;
	PalTransc_IdxRecMemCallback_t	pfHandleIdxRecMem;

	palTransc_ENTER_INST;

	pContext = pal_transc_getContextByRecDeviceId(ulRecDeviceId);
	if( NULL == pContext  )
	{
		palTransc_LEAVE_INST;
		ERR_LOG ("(%d) No Context\n", ulRecDeviceId);
		return;
	}
	palTransc_LEAVE_INST;

	palTransc_ENTER_CTX_INST(pContext);

	if (NULL == pContext->pfHandleIdxRecMem )
	{
		palTransc_LEAVE_CTX_INST(pContext);
		ERR_LOG ("(%d) No Callback (0x%x)\n",  ulRecDeviceId, pContext);
		return;
	}

	pfHandleIdxRecMem	= pContext->pfHandleIdxRecMem;
	ulUniqueId		= pContext->ulUniqueId;
	ulDeviceId		= pContext->stSetupInfo.ulDeviceId;

	palTransc_LEAVE_CTX_INST(pContext);


	pfHandleIdxRecMem(ulDeviceId, ulUniqueId, ul64Length);

	LOG ("[%s:%d] ulDeviceId(%d) ulRecDeviceId(%d) \n", __FUNCTION__, __LINE__,
						ulDeviceId, ulUniqueId );

	return;
}
#endif

STATIC void pal_transc_HandleProbeEvent(void *pCallbackData)
{
	palTransc_Context_t 			*pContext	=	NULL;
	DI_TRANSCODER_CALLBACK_DATA_t 	*pData		=	NULL;

	pData = (DI_TRANSCODER_CALLBACK_DATA_t*)pCallbackData;
	if( NULL == pData )
		return ;

	pContext = pal_transc_getContext(pData->ulDeviceId);
	if( NULL == pContext )
		return ;

	LOG ("[%s:%d] ulDeviceId(%d) Event(%d)\n", __FUNCTION__, __LINE__, 	pData->ulDeviceId, pData->eEvent);

	switch(pData->eEvent)
	{
		case DI_TRANSCODER_NOTIFY_EVT_DEC_SEQUENCE_HEADER_INFO:
			pContext->pfHandleProbeEvent(ePAL_TRANSC_PROBE_EVT_SEQUENCE_HEADER_INFO, pData->ulDeviceId, (HINT32)pData->pvEventData, 0, 0);
			break;
		case DI_TRANSCODER_NOTIFY_EVT_DEC_PROBE_FAIL:
			pContext->pfHandleProbeEvent(ePAL_TRANSC_PROBE_EVT_PROBE_FAIL, pData->ulDeviceId, (HINT32)pData->pvEventData, 0, 0);
			break;
		case DI_TRANSCODER_NOTIFY_EVT_DEC_PROBE_SUCCESS:
			pContext->pfHandleProbeEvent(ePAL_TRANSC_PROBE_EVT_PROBE_SUCCESS, pData->ulDeviceId, (HINT32)pData->pvEventData, 0, 0);
			break;

		default:
			ERR_LOG ("Incorrect decoder probe event \n");
			break;
	}

	return;
}

#define __TR_LOGIC__
STATIC HERROR pal_transc_logicInitContext(palTransc_Status_t *pStatus)
{
	DI_ERR_CODE			hDiResult = DI_ERR_OK;
	DI_TRANSCODER_CAP_t	ppCapInfo[palTransc_MAX_TRANSCODER_CNT];

	if( NULL == pStatus )
		return ERR_FAIL;

	pStatus->ulContextCnt = 0;

	HxSTD_memset(&ppCapInfo, 0, sizeof(ppCapInfo));
	hDiResult = DI_TRANSCODER_GetCapability(&pStatus->ulContextCnt, (DI_TRANSCODER_CAP_t**)&ppCapInfo);
	if( DI_ERR_OK != hDiResult )
	{
		ERR_LOG ("DI_TRANSCODER_GetCapability() : @RET (%d) \n",  (HINT32)hDiResult);
		return ERR_FAIL;
	}

	if( 0 == pStatus->ulContextCnt )
	{
		ERR_LOG ("There's not transcoder \n");
		return ERR_FAIL;
	}

	/*context alloc*/
	pStatus->pstContexts = (palTransc_Context_t*)HLIB_STD_MemAlloc( sizeof(palTransc_Context_t) * pStatus->ulContextCnt);
	if( NULL == pStatus->pstContexts )
	{
		ERR_LOG ("HLIB_STD_MemAlloc() \n");
		return ERR_FAIL;
	}

	HxLOG_Debug("pStatus->ulContextCn = %d\n", pStatus->ulContextCnt);
	if(NULL != ppCapInfo)
	{
		HxLOG_Debug("pStatus->ulGroupId          = %d\n", ppCapInfo->ulGroupId);
		HxLOG_Debug("pStatus->ulTranscoderId     = %d\n", ppCapInfo->ulTranscoderId);
		HxLOG_Debug("pStatus->eTransSourceType   = %d\n", ppCapInfo->eTransSourceType);
		HxLOG_Debug("pStatus->eVideoFormat       = %d\n", ppCapInfo->eVideoFormat);
		HxLOG_Debug("pStatus->eAudioFormat       = %d\n", ppCapInfo->eAudioFormat);
		HxLOG_Debug("pStatus->eTranscoderMuxType = %d (1:TS, 2:MP4)\n", ppCapInfo->eTranscoderMuxType);
	}

	return ERR_OK;
}

STATIC HERROR pal_transc_logicInitSystem(const palTransc_Status_t *pStatus)
{
	HINT32 i			=	0;
	HINT32	nResult	= 	VK_OK;
	HINT8	aTemp[80]	=	{0,};

	if( NULL == pStatus )
		return ERR_FAIL;


#if defined(CONFIG_TRANS_CALLBACK_MSG)
	/* transcoder callback용 message queue 생성 */
	nResult  = VK_MSG_Create(PAL_TRANSC_MSG_SIZE, sizeof(palTransc_Msg_t), "pal_transc_taskqueue", (unsigned long *)&pStatus->ulDiRecvMsgQ, VK_SUSPENDTYPE_FIFO);
	if (VK_OK != nResult )
	{
		ERR_LOG ("VK_MSG_Create ret:0x%X\n",nResult);
		return ERR_FAIL;
	}

	/* transcoder callback용 task 생성 */
	nResult  = VK_TASK_Create(pal_transc_taskDiRecvSendRingBuf,
								PAL_TRANSC_TASK_PRIORITY,
								PAL_TRANSC_TASK_STACK_SIZE,
								"pal_transc_task",
								(void *)pStatus,
								(unsigned long *)&pStatus->ulDiRecvTask,
								0);
	if (VK_OK != nResult )
	{
		ERR_LOG ("VK_TASK_Create ret:0x%X\n",nResult);
		return ERR_FAIL;
	}

	nResult = VK_TASK_Start(pStatus->ulDiRecvTask);
	if (VK_OK != nResult )
	{
		ERR_LOG ("VK_TASK_Start ret:0x%X\n",nResult);
		return ERR_FAIL;
	}
#endif

	nResult  = VK_SEM_Create ((unsigned long *)&pStatus->ulMainSemId, "PalTransc_Sem_Main", VK_SUSPENDTYPE_PRIORITY);
	if (VK_OK != nResult )
	{
		ERR_LOG ("Semaphore failed!!!\n");
		return ERR_FAIL;
	}

	for(i=0 ; i<pStatus->ulContextCnt ; i++ )
	{
		snprintf(aTemp, 80, "PalTransc_Sem_Ctx_%d", i);
		nResult  = VK_SEM_Create (&pStatus->pstContexts[i].ulSemId, aTemp, VK_SUSPENDTYPE_PRIORITY);
		if (VK_OK != nResult )
		{
			ERR_LOG ("Semaphore failed!!!\n");
			return ERR_FAIL;
		}

		pal_transc_resetContext(&pStatus->pstContexts[i]);
	}

	return ERR_OK;
}



STATIC HERROR pal_transc_logicSetVideoSettings ( DI_TRANSCODER_VideoSettings_t *pstVideoSettings, const PalTransc_ProfileItem_t *pstProfile, HBOOL bUpdate )
{
	HINT32	nFrameRate 	= 0;

	if( (NULL == pstVideoSettings ) || (NULL == pstProfile) )
		return ERR_FAIL;

	/*custom*/
	pstVideoSettings->stCustomRes.bCustomRes = TRUE;

	nFrameRate = (HINT32)pstProfile->nFrameRate;
	if( 0 != pstProfile->nScreenWidth) 		pstVideoSettings->stCustomRes.ulWidth	= pstProfile->nScreenWidth;
	if( 0 != pstProfile->nScreenHeight) 	pstVideoSettings->stCustomRes.ulHeight 	= pstProfile->nScreenHeight;
	if( 0 != pstProfile->nVideoBitrateKbps)pstVideoSettings->ulMaxBitRate			= pstProfile->nVideoBitrateKbps * 1000;
	if( 0 != nFrameRate)					pstVideoSettings->eFrameRate			= pal_transc_convertPal2Di_FrameRate(nFrameRate);
	if( 0 != pstProfile->nScreenHeight){
		pstVideoSettings->eRes	=	pal_transc_convertPal2Di_Resolution(pstProfile->nScreenHeight, pstProfile->bInterlaceMode);
		pstVideoSettings->stCustomRes.bInterlaced	=	pstProfile->bInterlaceMode;
	}

	if( 0 != pstProfile->eVideoAspectRatio)	pstVideoSettings->eAspectRatio 			= pal_transc_convertPal2Di_AspectRatio (pstProfile->eVideoAspectRatio);

	if( FALSE == bUpdate )
	{
		pstVideoSettings->stCustomIPB.bCustomIPB			= FALSE;
		pstVideoSettings->stCustomIPB.ulFrameP				= nFrameRate * 3;
		pstVideoSettings->stCustomIPB.ulFrameB				= 0;

		pstVideoSettings->stCustomRes.ulRefreshRate    		= palTransc_DEFAULT_REFRESH_RATE;
		pstVideoSettings->stCustomRes.ulSampleAspectRatioX	= pstVideoSettings->stCustomRes.ulWidth;
		pstVideoSettings->stCustomRes.ulSampleAspectRatioY	= pstVideoSettings->stCustomRes.ulHeight;
	}

	return ERR_OK;
}

STATIC HERROR pal_transc_logicSetVideoSettingsEx ( DI_TRANSCODER_Settings_t *pstDstSetInfo, const PalTransc_ProfileItem_t *pstProfile )
{
	if( NULL == pstDstSetInfo )
		return ERR_FAIL;

	/*video setting*/
	pstDstSetInfo->bVideoDefaultProfile = FALSE;

	/*codec level*/
	switch(pstProfile->eVideoCodecLevel)
	{
		case eDxVIDEO_ENC_LEVEL_00: pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_00; break;
		case eDxVIDEO_ENC_LEVEL_10: pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_10; break;
		case eDxVIDEO_ENC_LEVEL_1B: pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_1B; break;
		case eDxVIDEO_ENC_LEVEL_11: pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_11; break;
		case eDxVIDEO_ENC_LEVEL_12: pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_12; break;
		case eDxVIDEO_ENC_LEVEL_13: pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_13; break;
		case eDxVIDEO_ENC_LEVEL_20: pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_20; break;
		case eDxVIDEO_ENC_LEVEL_21: pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_21; break;
		case eDxVIDEO_ENC_LEVEL_22: pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_22; break;
		case eDxVIDEO_ENC_LEVEL_30: pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_30; break;
		case eDxVIDEO_ENC_LEVEL_31: pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_31; break;
		case eDxVIDEO_ENC_LEVEL_32: pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_32; break;
		case eDxVIDEO_ENC_LEVEL_40: pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_40; break;
		case eDxVIDEO_ENC_LEVEL_41: pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_41; break;
		case eDxVIDEO_ENC_LEVEL_42: pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_42; break;
		case eDxVIDEO_ENC_LEVEL_50: pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_50; break;
		case eDxVIDEO_ENC_LEVEL_51: pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_51; break;
		case eDxVIDEO_ENC_LEVEL_60: pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_60; break;
		case eDxVIDEO_ENC_LEVEL_62: pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_62; break;
		case eDxVIDEO_ENC_LEVEL_LOW: pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_LOW; break;
		case eDxVIDEO_ENC_LEVEL_MAIN: pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_MAIN; break;
		case eDxVIDEO_ENC_LEVEL_HIGH: pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_HIGH; break;
		case eDxVIDEO_ENC_LEVEL_HIGH1440: pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_HIGH1440; break;

		case eDxVIDEO_ENC_LEVEL_AUTO:
		{
			if( pstDstSetInfo->stVideoSettings.ulMaxBitRate <= 600000 )
				pstDstSetInfo->eCodecLevel	=	 DI_TRANSCODER_VIDEO_CODEC_LEVEL_30;
			else if( pstDstSetInfo->stVideoSettings.ulMaxBitRate <= 6500000 )
				pstDstSetInfo->eCodecLevel	=	 DI_TRANSCODER_VIDEO_CODEC_LEVEL_31;
			else if( pstDstSetInfo->stVideoSettings.ulMaxBitRate <= 8500000 )
				pstDstSetInfo->eCodecLevel	=	 DI_TRANSCODER_VIDEO_CODEC_LEVEL_40;
			else
				pstDstSetInfo->eCodecLevel	=	 DI_TRANSCODER_VIDEO_CODEC_LEVEL_30;
		}
		break;

		/*default is 30*/
		default:
			pstDstSetInfo->eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_30;
			break;
	}

	/*profile*/
	switch(pstProfile->eVideoCodecProfile)
	{
		case eDxVIDEO_ENC_PROFILE_SIMPLE:	pstDstSetInfo->eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_SIMPLE; break;
		case eDxVIDEO_ENC_PROFILE_MAIN:	pstDstSetInfo->eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_MAIN; break;
		case eDxVIDEO_ENC_PROFILE_HIGH:	pstDstSetInfo->eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_HIGH; break;
		case eDxVIDEO_ENC_PROFILE_ADVANCED:	pstDstSetInfo->eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_ADVANCED; break;
		case eDxVIDEO_ENC_PROFILE_JIZHUN:		pstDstSetInfo->eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_JIZHUN; break;
		case eDxVIDEO_ENC_PROFILE_SNRSCALABLE:	pstDstSetInfo->eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_SNRSCALABLE; break;
		case eDxVIDEO_ENC_PROFILE_SPATIALLYSCALABLE:	pstDstSetInfo->eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_SPATIALLYSCALABLE; break;
		case eDxVIDEO_ENC_PROFILE_ADVANCEDSIMPLE:	pstDstSetInfo->eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_ADVANCEDSIMPLE; break;
		case eDxVIDEO_ENC_PROFILE_BASELINE:		pstDstSetInfo->eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_BASELINE; break;

		case eDxVIDEO_ENC_PROFILE_AUTO:
		{
			if( pstDstSetInfo->stVideoSettings.ulMaxBitRate <= 1200000 )
				pstDstSetInfo->eCodecProfile	=	DI_TRANSCODER_VIDEO_CODEC_PROFILE_BASELINE;
			else if( pstDstSetInfo->stVideoSettings.ulMaxBitRate <= 6500000 )
				pstDstSetInfo->eCodecProfile	=	DI_TRANSCODER_VIDEO_CODEC_PROFILE_MAIN;
			else if( pstDstSetInfo->stVideoSettings.ulMaxBitRate <= 8500000 )
				pstDstSetInfo->eCodecProfile	=	DI_TRANSCODER_VIDEO_CODEC_PROFILE_HIGH;
			else
				pstDstSetInfo->eCodecProfile	=	DI_TRANSCODER_VIDEO_CODEC_PROFILE_BASELINE;
		}
		break;

		/*default is baseline*/
		default:
			pstDstSetInfo->eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_BASELINE;
			break;
	}

	return ERR_OK;
}

STATIC HERROR pal_transc_logicSetProbeApply ( DI_TRANSCODER_Settings_t *pstDstSetInfo,  DI_TRANSCODER_DEC_StreamInfo_t *pstProbeinfo, const PalTransc_ProfileItem_t *pstProfile, HUINT32 ulFlags)
{
	if( NULL == pstDstSetInfo )
		return ERR_FAIL;

	if( NULL == pstProbeinfo )
		return ERR_FAIL;


	pstDstSetInfo->bVideoDefaultProfile		=	FALSE;
	if( ulFlags&ePAL_TRANSC_PROBEMASK_CODECLEVEL )pstDstSetInfo->eCodecLevel		=	pstProbeinfo->eCodecLevel;
	if( ulFlags&ePAL_TRANSC_PROBEMASK_CODECPROFILE )pstDstSetInfo->eCodecProfile	=	pstProbeinfo->eCodecProfile;

	if( ulFlags&ePAL_TRANSC_PROBEMASK_HEIGHT )
	{
		pstDstSetInfo->stVideoSettings.eRes	=	pal_transc_convertPal2Di_Resolution(pstProbeinfo->ulHeight, !(pstProbeinfo->bProgressive));
		pstDstSetInfo->stVideoSettings.stCustomRes.bInterlaced	=	!pstProbeinfo->bProgressive;
	}

	if( ulFlags&ePAL_TRANSC_PROBEMASK_FRAMERATE )pstDstSetInfo->stVideoSettings.eFrameRate		=	pstProbeinfo->eFrameRate;

	pstDstSetInfo->stVideoSettings.stCustomRes.bCustomRes	=	TRUE;
	if( ulFlags&ePAL_TRANSC_PROBEMASK_WIDTH )pstDstSetInfo->stVideoSettings.stCustomRes.ulWidth	=	pstProbeinfo->ulWidth;	//pstProbeinfo->ulDisplayWidth;
	if( ulFlags&ePAL_TRANSC_PROBEMASK_HEIGHT )pstDstSetInfo->stVideoSettings.stCustomRes.ulHeight	=	pstProbeinfo->ulHeight;	//pstProbeinfo->ulDisplayHeight;

	if( ulFlags&ePAL_TRANSC_PROBEMASK_ASPECTRATIO )
	{
		pstDstSetInfo->stVideoSettings.eAspectRatio	=	pstProbeinfo->eAspectRatio;

		{
			/*
				AspectRatio
			 */
			/*check aspectratio*/
			switch(pstDstSetInfo->stVideoSettings.eAspectRatio)
			{
				case DI_VIDEO_ASPECTRATIO_16_9:
				case DI_VIDEO_ASPECTRATIO_4_3:
					break; //16:9 , 4:3 으로 한정
				default :
				{
					//http://en.wikipedia.org/wiki/Graphics_display_resolution
					HINT32	nNod, nWidth, nHeight;
					nWidth	=	(HINT32)pstProbeinfo->ulWidth;
					nHeight	=	(HINT32)pstProbeinfo->ulHeight;
					if( nHeight <= 0 ) nHeight = 1;

					nNod = (nWidth*100)/(nHeight);
					if( 170 <= nNod ) pstDstSetInfo->stVideoSettings.eAspectRatio = DI_VIDEO_ASPECTRATIO_16_9;
					else pstDstSetInfo->stVideoSettings.eAspectRatio = DI_VIDEO_ASPECTRATIO_4_3;
				}
				break;
			}

			if( DI_VIDEO_ASPECTRATIO_16_9 == pstDstSetInfo->stVideoSettings.eAspectRatio )
			{
				//차후 Table 구성으로 refactoring
				switch(pstDstSetInfo->stVideoSettings.stCustomRes.ulWidth)
				{
					case 432: pstDstSetInfo->stVideoSettings.stCustomRes.ulHeight = 240; break;
					case 640: pstDstSetInfo->stVideoSettings.stCustomRes.ulHeight = 360; break;
					case 854: pstDstSetInfo->stVideoSettings.stCustomRes.ulHeight = 480; break;
					case 960: pstDstSetInfo->stVideoSettings.stCustomRes.ulHeight = 540; break;
					case 1024: pstDstSetInfo->stVideoSettings.stCustomRes.ulHeight = 576; break;
					case 1136: pstDstSetInfo->stVideoSettings.stCustomRes.ulHeight = 640; break;
					case 1280: pstDstSetInfo->stVideoSettings.stCustomRes.ulHeight = 720; break;
					case 1366: pstDstSetInfo->stVideoSettings.stCustomRes.ulHeight = 768; break;
					case 1600: pstDstSetInfo->stVideoSettings.stCustomRes.ulHeight = 900; break;
					case 1920: pstDstSetInfo->stVideoSettings.stCustomRes.ulHeight = 1080; break;
					default: break;
				}
			}
			else
			{
				//차후 Table 구성으로 refactoring
				switch(pstDstSetInfo->stVideoSettings.stCustomRes.ulWidth)
				{
					case 160: pstDstSetInfo->stVideoSettings.stCustomRes.ulHeight = 120; break;
					case 320: pstDstSetInfo->stVideoSettings.stCustomRes.ulHeight = 240; break;
					case 640: pstDstSetInfo->stVideoSettings.stCustomRes.ulHeight = 480; break;
					case 800: pstDstSetInfo->stVideoSettings.stCustomRes.ulHeight = 600; break;
					case 1024: pstDstSetInfo->stVideoSettings.stCustomRes.ulHeight = 768; break;
					case 1152: pstDstSetInfo->stVideoSettings.stCustomRes.ulHeight = 1280; break;
					case 1280: pstDstSetInfo->stVideoSettings.stCustomRes.ulHeight = 960; break;
					case 1400: pstDstSetInfo->stVideoSettings.stCustomRes.ulHeight = 1050; break;
					case 1600: pstDstSetInfo->stVideoSettings.stCustomRes.ulHeight = 1200; break;
					case 2048: pstDstSetInfo->stVideoSettings.stCustomRes.ulHeight = 1536; break;
					default: break;
				}
			}
		}
#if 0
 		/*SAR로 내려주면 에러 발생함*/
		if( DI_VIDEO_ASPECTRATIO_SAR == pstProbeinfo->eAspectRatio )
		{
			pstDstSetInfo->stVideoSettings.stCustomRes.ulSampleAspectRatioX	=	pstDstSetInfo->stVideoSettings.stCustomRes.ulWidth;//pstProbeinfo->usSampleAspectRatioX;
			pstDstSetInfo->stVideoSettings.stCustomRes.ulSampleAspectRatioY	=	pstDstSetInfo->stVideoSettings.stCustomRes.ulHeight;//pstProbeinfo->usSampleAspectRatioY;
		}
#endif

	}

	if( ulFlags&ePAL_TRANSC_PROBEMASK_VIDEOFORMAT )pstDstSetInfo->eOutputVideoFormat		= 	pstProbeinfo->eVideoFormat;
	/*
		if( ulFlags&ePAL_TRANSC_PROBEMASK_COLORPRIMARIE ) pstProbeinfo->ucColorPrimarie;
		N/A
	*/

	/*check hd,sd*/
	if( 0 < pstProfile->nHdVideoBitrateKbps )
	{
		const HINT32 HD_HEIGHT_MIN = 720;

		if( HD_HEIGHT_MIN <= pstProbeinfo->ulHeight )
		{
			pstDstSetInfo->stVideoSettings.ulMaxBitRate = pstProfile->nHdVideoBitrateKbps * 1000;
		}
	}

	return ERR_OK;
}

STATIC HERROR pal_transc_logicSetSettings ( palTransc_Context_t *pContext )
{
	HBOOL	bVideoValidity = FALSE, bAudioValidity = FALSE;
	DI_ERR_CODE hDiResult = DI_ERR_OK ;
	DI_TRANSCODER_Settings_t stDstSetInfo;
	PalTransc_Setup_t	*pstSetupInfo;
	HUINT32 ulDeviceId;

	if( pContext == NULL )
		return ERR_FAIL;

	ulDeviceId = pContext->stSetupInfo.ulDeviceId;
	pstSetupInfo = &pContext->stSetupInfo;

	HxSTD_memset (&stDstSetInfo, 0, sizeof(DI_TRANSCODER_Settings_t));

	/*source type*/
	stDstSetInfo.eTransSourceType = pal_transc_convertPal2Di_SourceType(pstSetupInfo->eTransSourceType);
	if( stDstSetInfo.eTransSourceType == DI_TRANSCODER_NONE)
	{
		ERR_LOG (" incorrect SourceType for transcoder (%d) \n",  pstSetupInfo->eTransSourceType);
		return ERR_FAIL;
	}

	/*encode mode*/
	stDstSetInfo.eTransEncodeMode = pal_transc_convertPal2Di_EncodeMode(pstSetupInfo->eTransEncodeMode);
	if( stDstSetInfo.eTransEncodeMode == DI_TRANSCODER_ENCODE_MAX)
	{
		ERR_LOG (" incorrect EncodeMode for transcoder (%d) \n",  pstSetupInfo->eTransEncodeMode);
		return ERR_FAIL;
	}

	if( DI_TRANSCODER_ENCODE_AV == stDstSetInfo.eTransEncodeMode )
	{
		bVideoValidity = TRUE; bAudioValidity = TRUE;
	}
	else if( DI_TRANSCODER_ENCODE_VIDEO == stDstSetInfo.eTransEncodeMode ) bVideoValidity = TRUE;
	else if( DI_TRANSCODER_ENCODE_AUDIO == stDstSetInfo.eTransEncodeMode ) bAudioValidity = TRUE;

	/*source*/
	stDstSetInfo.eInputVideoFormat = pal_transc_convertPal2Di_VideoFormat(pstSetupInfo->eInputVideoFormat);
	if( stDstSetInfo.eInputVideoFormat == DI_VIDEO_FORMAT_NONE)
	{
		ERR_LOG (" incorrect InputVideoFormat for transcoder (%d) \n",  pstSetupInfo->eInputVideoFormat);
		bVideoValidity = FALSE;
	}

	stDstSetInfo.eInputAudioFormat = pal_transc_convertPal2Di_AudioFormat(pstSetupInfo->eInputAudioFormat);
	if( stDstSetInfo.eInputAudioFormat == DI_AUDFORMAT_NONE)
	{
		ERR_LOG (" incorrect eInputAudioFormat for transcoder (%d) \n",  pstSetupInfo->eInputAudioFormat);
		bAudioValidity = FALSE;
	}

	/*profile*/
	stDstSetInfo.eOutputVideoFormat = pal_transc_convertPal2Di_VideoFormat(pstSetupInfo->stProfile.eVideoCodec);
	if( stDstSetInfo.eOutputVideoFormat == DI_VIDEO_FORMAT_NONE)
	{
		ERR_LOG (" incorrect eVideoCodec for transcoder (%d) \n",  pstSetupInfo->stProfile.eVideoCodec);
		bVideoValidity = FALSE;
	}

	stDstSetInfo.eOutputAudioFormat = pal_transc_convertPal2Di_AudioFormat(pstSetupInfo->stProfile.eAudioCodec);
	if( stDstSetInfo.eOutputAudioFormat == DI_AUDFORMAT_NONE)
	{
		ERR_LOG (" incorrect eAudioCodec for transcoder (%d) \n",  pstSetupInfo->stProfile.eAudioCodec);
		bAudioValidity = FALSE;
	}

	/*cross check*/
	if( (TRUE  == bVideoValidity) && (TRUE  == bAudioValidity) ) stDstSetInfo.eTransEncodeMode = DI_TRANSCODER_ENCODE_AV;
	if( (TRUE  == bVideoValidity) && (FALSE == bAudioValidity) ) stDstSetInfo.eTransEncodeMode = DI_TRANSCODER_ENCODE_VIDEO;
	if( (FALSE == bVideoValidity) && (TRUE  == bAudioValidity) ) stDstSetInfo.eTransEncodeMode = DI_TRANSCODER_ENCODE_AUDIO;

	/*video Setting*/
	if( TRUE == bVideoValidity )
	{
		pal_transc_logicSetVideoSettings( &stDstSetInfo.stVideoSettings,  &pstSetupInfo->stProfile, FALSE);
		pal_transc_logicSetVideoSettingsEx( &stDstSetInfo ,	&pstSetupInfo->stProfile);
	}

	if( TRUE == pContext->bProbed )
	{
		pal_transc_logicSetProbeApply(&stDstSetInfo, pContext->pstProbeinfo, &pstSetupInfo->stProfile, pContext->ulProbeFlags );
	}

	/*demux*/
	stDstSetInfo.ulDemuxId = pstSetupInfo->ulDemuxId;

	/*pid*/
	stDstSetInfo.usVideoPid = pstSetupInfo->usVideoPid;
	stDstSetInfo.usAudioPid = pstSetupInfo->usAudioPid;
	stDstSetInfo.usPcrPid = pstSetupInfo->usPcrPid;




#if defined(CONFIG_DEBUG)
	pal_transc_debugPrintSettingInfo( &stDstSetInfo );
#endif

	/*run*/
	hDiResult = DI_TRANSCODER_SetSettings(ulDeviceId,&stDstSetInfo);
	if( DI_ERR_OK != hDiResult  )
	{
		ERR_LOG (" DI_TRANSCODER_SetSettings(deviceID(%d)) : @RET (%d) \n",  ulDeviceId, (HINT32)hDiResult);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR pal_transc_logicSetPSI( HUINT32 ulDeviceId, PalTransc_PSI_t *pstSetPSI )
{
	DI_ERR_CODE hDiResult = DI_ERR_OK ;

	if( pstSetPSI == NULL )
		return ERR_FAIL;

	_pal_transc_printPalTransc_PSI_t(pstSetPSI, (HCHAR *)__FUNCTION__, __LINE__);

	hDiResult = DI_TRANSCODER_SetPSIData(ulDeviceId,
										pstSetPSI->usPid,
										pstSetPSI->stPSIData.ulSize,
										pstSetPSI->stPSIData.aucRawSectionBuffer,
										(HUINT8)pstSetPSI->bContinue,
										pstSetPSI->usPeriodTime,
										&pstSetPSI->ulIndexId);

	if( DI_ERR_OK != hDiResult )
	{
		ERR_LOG (" DI_TRANSCODER_SetPSIData(ulDeviceId(%d)) : @RET (%d) \n",  ulDeviceId, (HINT32)hDiResult);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR pal_transc_logicUpdatePSI( HUINT32 ulDeviceId, PalTransc_PSI_t *pstSetPSI )
{
	DI_ERR_CODE hDiResult = DI_ERR_OK ;

	if( pstSetPSI == NULL )
		return ERR_FAIL;

	_pal_transc_printPalTransc_PSI_t(pstSetPSI, (HCHAR *)__FUNCTION__, __LINE__);

	hDiResult = DI_TRANSCODER_UpdatePSIData(	ulDeviceId,
											pstSetPSI->ulIndexId,
											pstSetPSI->stPSIData.ulSize,
											(HUINT8*)pstSetPSI->stPSIData.aucRawSectionBuffer,
											(HUINT8)pstSetPSI->stPSIData.usFlag/*payload_unit_start_indicator*/);

	if( DI_ERR_OK != hDiResult )
	{
		ERR_LOG (" DI_TRANSCODER_UpdatePSIData(ulDeviceId(%d)) : @RET (%d) \n",  ulDeviceId, (HINT32)hDiResult);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR pal_transc_logicStartDemux ( palTransc_Context_t *pContext, const HBOOL bVideoOnly)
{
	DI_ERR_CODE hDiResult = DI_ERR_OK ;
	Pid_Info stPidInfo;

	if( NULL == pContext)
		return ERR_FAIL;

	HxSTD_memset (&stPidInfo, 0, sizeof(Pid_Info));

	if( TRUE == bVideoOnly )
	{
		stPidInfo.usVideoPid			= pContext->stSetupInfo.usVideoPid;
		stPidInfo.usAudioMainPid		= PID_NULL;
		stPidInfo.usPCRPid			= PID_NULL;
	}
	else
	{
		stPidInfo.usVideoPid			= pContext->stSetupInfo.usVideoPid;
		stPidInfo.usAudioMainPid		= pContext->stSetupInfo.usAudioPid;
		stPidInfo.usPCRPid			= pContext->stSetupInfo.usPcrPid;
	}

	switch (pContext->stSetupInfo.ulDeviceId)
	{
		case palTransc_TRANSCODER_ID_MAIN:
			 if(palTransc_CHECK_TRANS_PID(stPidInfo.usVideoPid) == TRUE)		stPidInfo.pidBitmask =PID_BITMASK_VIDEO_TRANS_1;
			 if(palTransc_CHECK_TRANS_PID(stPidInfo.usAudioMainPid) == TRUE)	stPidInfo.pidBitmask |=PID_BITMASK_AUDIO_TRANS_1;
			 if(palTransc_CHECK_TRANS_PID(stPidInfo.usPCRPid) == TRUE)			stPidInfo.pidBitmask |=PID_BITMASK_PCR_TRANS_1;
			break;

		case palTransc_TRANSCODER_ID_SUB:
			if(palTransc_CHECK_TRANS_PID(stPidInfo.usVideoPid) == TRUE)			stPidInfo.pidBitmask =PID_BITMASK_VIDEO_TRANS_2;
			if(palTransc_CHECK_TRANS_PID(stPidInfo.usAudioMainPid) == TRUE)	stPidInfo.pidBitmask |=PID_BITMASK_AUDIO_TRANS_2;
			if(palTransc_CHECK_TRANS_PID(stPidInfo.usPCRPid) == TRUE)			stPidInfo.pidBitmask |=PID_BITMASK_PCR_TRANS_2;
				break;

		default:
			ERR_LOG ("not support deviceid(%d) for demux bitmask \n",  pContext->stSetupInfo.ulDeviceId);
			return ERR_FAIL;
	}

	/*save pidmask for stop*/
	pContext->stSetupInfo.ulPidBitmask = stPidInfo.pidBitmask;


	hDiResult  = DI_DEMUX_StartPID (pContext->stSetupInfo.ulDemuxId, (void *)&stPidInfo);
	if( hDiResult  != DI_ERR_OK )
	{
		ERR_LOG ("DI_DEMUX_StartPID: @SET (%d) \n",  hDiResult );
		return ERR_FAIL;
	}
	ERR_LOG ("Done DI_DEMUX_StartPID() : @RET (%d) \n", (HINT32)hDiResult);
	return ERR_OK;
}

STATIC HERROR pal_transc_logicTranscoderStart( HUINT32 ulDeviceId, const PalTransc_Setup_t *pstSetupInfo )
{
	DI_ERR_CODE hDiResult = DI_ERR_OK ;
	DI_TRANSCODER_MUX_INFO stMuxInfo;

	HxSTD_memset (&stMuxInfo, 0, sizeof(DI_TRANSCODER_MUX_INFO));

	stMuxInfo.index			=	0;//pstSetupInfo->ulDeviceId;
	stMuxInfo.videoPid		=	pstSetupInfo->usVideoPid;
	stMuxInfo.audioPid		=	pstSetupInfo->usAudioPid;
#if CONFIG_TRANSCODER_FIXED_PCRPID
	stMuxInfo.pcrPid			=	CONFIG_TRANSCODER_FIXED_PCRPID;
#else
	stMuxInfo.pcrPid			=	pstSetupInfo->usPcrPid;
#endif
	stMuxInfo.videoCodec		=	pal_transc_convertPal2Di_VideoFormat(pstSetupInfo->stProfile.eVideoCodec);
	stMuxInfo.audioCodec		=	pal_transc_convertPal2Di_AudioFormat(pstSetupInfo->stProfile.eAudioCodec);
	stMuxInfo.NonPcrInsert	=	!pstSetupInfo->bPcrTsPacketInsert;

	HxLOG_Debug("stMuxInfo.index        = %d\n", stMuxInfo.index);
	HxLOG_Debug("stMuxInfo.videoPid     = 0x%x\n", stMuxInfo.videoPid);
	HxLOG_Debug("stMuxInfo.audioPid     = 0x%x\n", stMuxInfo.audioPid);
	HxLOG_Debug("stMuxInfo.pcrPid       = 0x%x\n", stMuxInfo.videoPid);
	HxLOG_Debug("stMuxInfo.videoCodec   = %d\n", stMuxInfo.videoCodec);
	HxLOG_Debug("stMuxInfo.audioCodec   = %d\n", stMuxInfo.audioCodec);
	HxLOG_Debug("stMuxInfo.NonPcrInsert = %d\n", stMuxInfo.NonPcrInsert);


	hDiResult = DI_TRANSCODER_Start(ulDeviceId,DI_TRANSCODER_MUX_TS,&stMuxInfo);
	if( DI_ERR_OK != hDiResult )
	{
		ERR_LOG ("Error> DI_TRANSCODER_Start(deviceId:%d) : @RET (%d) \n",  ulDeviceId, (HINT32)hDiResult);
		return ERR_FAIL;
	}
	ERR_LOG ("Done DI_TRANSCODER_Start(deviceId:%d) : @RET (%d) \n",  ulDeviceId, (HINT32)hDiResult);
	return ERR_OK;
}


STATIC HERROR pal_transc_logicTranscoderRecStart( HUINT32 ulDeviceId, const PalTransc_Setup_t *pstSetupInfo )
{
	DI_ERR_CODE hDiResult = DI_ERR_OK ;
	DI_PVR_REC_SETUP_t stRecSetup;

	HxSTD_memset (&stRecSetup, 0, sizeof(DI_PVR_REC_SETUP_t));

	stRecSetup.ulDemuxId = pstSetupInfo->ulDemuxId;
	stRecSetup.eFileType = eDI_PVR_FileType_Linear;

	if( TRUE == pstSetupInfo->bTimestamp )
		stRecSetup.ulPacketSize = PACKET_SIZE_PVR;
	else	stRecSetup.ulPacketSize = PACKET_SIZE_TS;

	if( ePAL_TRANSC_ENCODEMODE_AUDIO == pstSetupInfo->eTransEncodeMode )
		stRecSetup.eStreamType = eDI_PVR_Stream_Audio;
	else	stRecSetup.eStreamType = eDI_PVR_Stream_Video;

	stRecSetup.eVideoType				=	pal_transc_convertPal2Di_PvrVideoType(pstSetupInfo->stProfile.eVideoCodec);
	stRecSetup.stEncryptSettings.eEncType	=	pal_transc_convertPal2Di_EncodeType(pstSetupInfo->stCrypto.eEncType);

	/*set encrytion*/
	if( DI_PVR_ENCRYPTION_TYPE_NONE != stRecSetup.stEncryptSettings.eEncType )
	{
		HxSTD_memcpy(&stRecSetup.stEncryptSettings.ucIv, &pstSetupInfo->stCrypto.ucIv, sizeof(pstSetupInfo->stCrypto.ucIv));
		HxSTD_memcpy(&stRecSetup.stEncryptSettings.ucKey, &pstSetupInfo->stCrypto.ucKey, sizeof(pstSetupInfo->stCrypto.ucKey));
		HxSTD_memcpy(&stRecSetup.stEncryptSettings.ucProtectionKey, &pstSetupInfo->stCrypto.ucProtectionKey, sizeof(pstSetupInfo->stCrypto.ucProtectionKey));
	}

	if( ePAL_TRANSC_OUTPUTTYPE_FILE == pstSetupInfo->eTransOutputType )
	{
		HxSTD_memcpy(stRecSetup.aucFileName, pstSetupInfo->acuFilename, sizeof(stRecSetup.aucFileName));
	}
	else if( ePAL_TRANSC_OUTPUTTYPE_BUFFER == pstSetupInfo->eTransOutputType )
	{
		snprintf(stRecSetup.aucFileName, 6,"memio%c",0);
	}

	HxLOG_Debug("pstSetupInfo->ulRecDeviceId = %d, ulDeviceId =%d\n", pstSetupInfo->ulRecDeviceId, ulDeviceId);
	HxLOG_Debug("stRecSetup.eFileType         = %d\n", stRecSetup.eFileType);
	HxLOG_Debug("stRecSetup.eStreamType       = %d\n", stRecSetup.eStreamType);
	HxLOG_Debug("stRecSetup.eVideoType        = %d\n", stRecSetup.eVideoType);
	HxLOG_Debug("stRecSetup.aucFileName       = %s\n", stRecSetup.aucFileName);
	HxLOG_Debug("stRecSetup.stEncryptSettings = %d\n", stRecSetup.stEncryptSettings);
	HxLOG_Debug("stRecSetup.ulDemuxId         = %d\n", stRecSetup.ulDemuxId);
	HxLOG_Debug("stRecSetup.ulPacketSize      = %d\n", stRecSetup.ulPacketSize);
	HxLOG_Debug("stRecSetup.bIsAllPidChannel  = %d\n", stRecSetup.bIsAllPidChannel);
	HxLOG_Debug("DI_TRANSCODE_REC_Setup call\n");

	hDiResult = DI_TRANSCODE_REC_Setup(pstSetupInfo->ulRecDeviceId, ulDeviceId, &stRecSetup);
	if( DI_ERR_OK != hDiResult )
	{
		ERR_LOG (" DI_TRANSCODE_REC_Setup(ulDeviceId(%d)) : @RET (%d) \n",  ulDeviceId, (HINT32)hDiResult);
		return ERR_FAIL;
	}

	HxLOG_Debug("DI_TRANSCODE_REC_Start call\n");
	hDiResult = DI_TRANSCODE_REC_Start(pstSetupInfo->ulRecDeviceId);
	if( DI_ERR_OK != hDiResult )
	{
		ERR_LOG (" DI_TRANSCODE_REC_Start(ulDeviceId(%d)) : @RET (%d) \n",  ulDeviceId, (HINT32)hDiResult);
		return ERR_FAIL;
	}

	return ERR_OK;
}


STATIC HERROR pal_transc_logicTranscoderStopDemux( HUINT32 ulDemuxId, HUINT32 ulPidBitmask )
{
	DI_ERR_CODE hDiResult;

	HxLOG_Debug("DI_DEMUX_StopPID call, ulDemuxId = %d, ulPidBitmask = 0x%x\n", ulDemuxId, ulPidBitmask);

	hDiResult = DI_DEMUX_StopPID(ulDemuxId, ulPidBitmask);
	if( DI_ERR_OK != hDiResult )
	{
		ERR_LOG (" DI_DEMUX_StopPID(ulDemuxId(%d)) : @RET (%d) \n",  ulDemuxId, (HINT32)hDiResult);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR pal_transc_logicTranscoderStop( HUINT32 ulDeviceId )
{
	DI_ERR_CODE hDiResult;

	HxLOG_Debug("DI_TRANSCODER_Stop call: ulDeviceId = %d\n", ulDeviceId);

	hDiResult = DI_TRANSCODER_Stop(ulDeviceId);
	if( DI_ERR_OK != hDiResult )
	{
		ERR_LOG (" DI_TRANSCODER_Stop(ulDeviceId(%d)) : @RET (%d) \n",  ulDeviceId, (HINT32)hDiResult);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR pal_transc_logicTranscoderClose( HUINT32 ulDeviceId )
{
	DI_ERR_CODE hDiResult;

	HxLOG_Debug("DI_TRANSCODER_Close call: ulDeviceId = %d\n", ulDeviceId);

	hDiResult = DI_TRANSCODER_Close(ulDeviceId);
	if( DI_ERR_OK != hDiResult )
	{
		ERR_LOG (" DI_TRANSCODER_Close(ulDeviceId(%d)) : @RET (%d) \n",  ulDeviceId, (HINT32)hDiResult);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR pal_transc_logicTranscoderRecStop( HUINT32 ulRecDeviceId, HUINT32 ulDeviceId )
{
	DI_ERR_CODE hDiResult;

	HxLOG_Debug("DI_TRANSCODE_REC_Stop call: ulRecDeviceId = %d\n", ulRecDeviceId);

	hDiResult = DI_TRANSCODE_REC_Stop(ulRecDeviceId);
	if( DI_ERR_OK != hDiResult )
	{
		ERR_LOG (" DI_TRANSCODE_REC_Stop(ulRecDeviceId(%d)) : @RET (%d) \n",  ulRecDeviceId, (HINT32)hDiResult);
		return ERR_FAIL;
	}

	HxLOG_Debug("DI_TRANSCODE_REC_Unset call: ulRecDeviceId = %d, ulDeviceId = %d\n", ulRecDeviceId, ulDeviceId);

	hDiResult = DI_TRANSCODE_REC_Unset(ulRecDeviceId,  ulDeviceId);
	if( DI_ERR_OK != hDiResult )
	{
		ERR_LOG (" DI_TRANSCODE_REC_Unset(ulRecDeviceId(%d)) : @RET (%d) \n",  ulRecDeviceId, (HINT32)hDiResult);
		return ERR_FAIL;
	}


	return ERR_OK;
}

STATIC HERROR pal_transc_logicChangeProfile( HUINT32 ulDeviceId, PalTransc_ProfileItem_t *pstChangeProfile )
{
	HERROR 		hResult 		=	ERR_OK;
	DI_ERR_CODE	hDiResult	=	DI_ERR_OK;
	HBOOL 			bChanged 	=	FALSE;
	DI_TRANSCODER_UpdateSettings_t stCurrentSettings;

	HxSTD_memset(&stCurrentSettings, 0, sizeof(DI_TRANSCODER_UpdateSettings_t));

	if( NULL == pstChangeProfile )
		return ERR_FAIL;

	HxLOG_Debug("DI_TRANSCODER_UpdateGetSettings call: ulDeviceId = %d\n", ulDeviceId);

	hDiResult = DI_TRANSCODER_UpdateGetSettings(ulDeviceId, &stCurrentSettings);
	if( DI_ERR_OK != hDiResult  )
	{
		ERR_LOG (" DI_TRANSCODER_UpdateSetSettings RET (%d) \n",   hDiResult);
		return ERR_FAIL;
	}

	/*check*/
	do
	{
		HINT32	nChangeMaxBitRate, nChangeFrameRate;

		nChangeMaxBitRate	= pstChangeProfile->nVideoBitrateKbps*1000;
		nChangeFrameRate	= (HUINT32)pstChangeProfile->nFrameRate;


#if defined(CONFIG_DEBUG)
		LOG ("[%s:%d] stCurrentSettings info \r\n", __FUNCTION__, __LINE__);
		HxLOG_DecIntDump(stCurrentSettings.stVideoSettings.ulMaxBitRate);
		HxLOG_DecIntDump(stCurrentSettings.stVideoSettings.eFrameRate);
		HxLOG_DecIntDump(stCurrentSettings.stVideoSettings.eAspectRatio);
		HxLOG_DecIntDump(stCurrentSettings.stVideoSettings.stCustomRes.ulWidth);
		HxLOG_DecIntDump(stCurrentSettings.stVideoSettings.stCustomRes.ulHeight);


		//LOG ("[%s:%d] pstChangeProfile info \r\n", __FUNCTION__, __LINE__);
		//PL_MediaProfile_ItemFullNameDump((void *)pstChangeProfile);		// jyhwang - json 출력하도록 수정해야 됨.
#endif


		if( (0!=nChangeMaxBitRate)&&(nChangeMaxBitRate != stCurrentSettings.stVideoSettings.ulMaxBitRate) )									{	bChanged = TRUE; break;	}
		if( (0!=nChangeFrameRate)&&(pal_transc_convertPal2Di_FrameRate(nChangeFrameRate) != stCurrentSettings.stVideoSettings.eFrameRate) )	{	bChanged = TRUE; break;	}
		if( (0!=pstChangeProfile->nScreenHeight) && (pal_transc_convertPal2Di_Resolution(pstChangeProfile->nScreenHeight, pstChangeProfile->bInterlaceMode) != stCurrentSettings.stVideoSettings.eRes) )	{	bChanged = TRUE; break;	}
		if( (0!=pstChangeProfile->eVideoAspectRatio) && (pal_transc_convertPal2Di_AspectRatio (pstChangeProfile->eVideoAspectRatio) != stCurrentSettings.stVideoSettings.eAspectRatio) )	{	bChanged = TRUE; break;	}

		if( TRUE == stCurrentSettings.stVideoSettings.stCustomRes.bCustomRes )
		{
			if( (0!=pstChangeProfile->nScreenWidth) && (pstChangeProfile->nScreenWidth  != stCurrentSettings.stVideoSettings.stCustomRes.ulWidth) )	{	bChanged = TRUE; break;	}
			if( (0!=pstChangeProfile->nScreenHeight) && (pstChangeProfile->nScreenHeight != stCurrentSettings.stVideoSettings.stCustomRes.ulHeight) )	{	bChanged = TRUE; break;	}
		}
		else	{	bChanged = TRUE; break;	}

	}while(0);

	if( FALSE == bChanged )
	{
		ERR_LOG ("same information \n");
		return ERR_OK;
	}

	/*logic*/
	hResult = pal_transc_logicSetVideoSettings( &stCurrentSettings.stVideoSettings, pstChangeProfile, TRUE);
	if( ERR_OK != hResult )
		return hResult;

	HxLOG_Debug("DI_TRANSCODER_UpdateSetSettings call: ulDeviceId = %d\n", ulDeviceId);

	hDiResult = DI_TRANSCODER_UpdateSetSettings(ulDeviceId, &stCurrentSettings);
	if( DI_ERR_OK != hDiResult  )
	{
		ERR_LOG ("DI_TRANSCODER_UpdateSetSettings RET (%d) \n",   hDiResult);
		return  ERR_FAIL;
	}

	return ERR_OK;
}

#define __TR_COMMAND__
STATIC HERROR pal_transc_cmdInit(palTransc_Status_t *pStatus)
{
	HERROR hResult = ERR_OK;

	/*context*/
	hResult = pal_transc_logicInitContext(pStatus);
	if( ERR_OK != hResult  )
		return hResult;

	/*main*/
	hResult = pal_transc_logicInitSystem(pStatus);
	if( ERR_OK != hResult  )
		return hResult;

	return hResult;
}

STATIC HERROR pal_transc_cmdStart( palTransc_Context_t *pContext  )
{
	HERROR hResult = ERR_OK;
	HINT32 i =0;

	if( FALSE == pContext->bStarted  )
	{
		hResult = pal_transc_logicStartDemux( pContext , FALSE);
		if( ERR_OK != hResult  )
			return hResult;

		hResult = pal_transc_logicSetSettings( pContext );
		if( ERR_OK != hResult  )
			return hResult;

		hResult = pal_transc_logicTranscoderStart( pContext->stSetupInfo.ulDeviceId, &pContext->stSetupInfo );
		if( ERR_OK != hResult  )
			return hResult;


		for( i=0 ; i<ePAL_TRANSC_PSITABLE_MAX ; i++ )
		{
			if( palTransc_CHECK_TRANS_PID( pContext->astSetupPSIList[ i ].usPid ) == TRUE )
			{
				hResult = pal_transc_logicSetPSI( pContext->stSetupInfo.ulDeviceId, &pContext->astSetupPSIList[i] );
				if( ERR_OK != hResult  )
					return hResult;
			}
		}

		hResult = pal_transc_logicTranscoderRecStart( pContext->stSetupInfo.ulDeviceId, &pContext->stSetupInfo );
		if( ERR_OK != hResult  )
			return hResult;

		pContext->bStarted = TRUE;
	}

	return hResult;
}

STATIC HERROR pal_transc_cmdStop( palTransc_Context_t *pContext  )
{
	HERROR hResult = ERR_OK;

	//TRA_LOG ();

	do
	{
		if( TRUE != pContext->bStarted )
			break;

		hResult = pal_transc_logicTranscoderRecStop(pContext->stSetupInfo.ulRecDeviceId, pContext->stSetupInfo.ulDeviceId);
		if( ERR_OK != hResult  )
			break;

		hResult = pal_transc_logicTranscoderStop(pContext->stSetupInfo.ulDeviceId);
		if( ERR_OK != hResult  )
			break;

		hResult = pal_transc_logicTranscoderStopDemux(pContext->stSetupInfo.ulDemuxId, pContext->stSetupInfo.ulPidBitmask);
		if( ERR_OK != hResult  )
			break;

		hResult = pal_transc_logicTranscoderClose(pContext->stSetupInfo.ulDeviceId);
		if( ERR_OK != hResult  )
			break;
	}while(0);

	pal_transc_resetContext(pContext);
	return hResult;
}

STATIC HERROR pal_transc_cmdChangeProfile( palTransc_Context_t *pContext, PalTransc_ProfileItem_t *pstProfile )
{
	HERROR hResult = ERR_OK;

	//TRA_LOG ();

	if( TRUE == pContext->bStarted )
	{
		hResult = pal_transc_logicChangeProfile(  pContext->stSetupInfo.ulDeviceId, pstProfile );
		if( hResult == ERR_OK )
		{
			HxSTD_memcpy(&pContext->stSetupInfo.stProfile, pstProfile,  sizeof(PalTransc_ProfileItem_t) );
		}
	}

	return hResult;
}

STATIC HERROR pal_transc_cmdGetPSI( palTransc_Context_t *pContext, HUINT16 usPid, PalTransc_PSIData_t  *pstPSIData)
{
	HERROR hResult = ERR_OK;
	HINT32 idxPSI;

	idxPSI = pal_transc_utilFindPSIListIndexbyPid(usPid, pContext->astSetupPSIList);
	if( -1 != idxPSI )
		HxSTD_memcpy( pstPSIData, &pContext->astSetupPSIList[idxPSI].stPSIData, sizeof(PalTransc_PSIData_t) );

	return hResult;
}

STATIC HERROR pal_transc_cmdSetPSI( palTransc_Context_t *pContext, PalTransc_PSI_t  *pstPSI)
{
	HERROR hResult = ERR_OK;

	if( (palTransc_CHECK_TRANS_PID( pContext->astSetupPSIList[ pstPSI->eTransPSITableType ].usPid ) == TRUE) &&
		(TRUE == pContext->bStarted) )
	{
		/*update*/
		HxSTD_memcpy( &pContext->astSetupPSIList[ pstPSI->eTransPSITableType ].stPSIData, &pstPSI->stPSIData, sizeof(PalTransc_PSIData_t) );
		pal_transc_logicUpdatePSI( pContext->stSetupInfo.ulDeviceId, &pContext->astSetupPSIList[ pstPSI->eTransPSITableType ] );
	}
	else
	{
		/*save*/
		pContext->astSetupPSIList[ pstPSI->eTransPSITableType ].usPid					=	pstPSI->usPid;
		pContext->astSetupPSIList[ pstPSI->eTransPSITableType ].bContinue				=	pstPSI->bContinue;
		pContext->astSetupPSIList[ pstPSI->eTransPSITableType ].eTransPSITableType	=	pstPSI->eTransPSITableType;
		pContext->astSetupPSIList[ pstPSI->eTransPSITableType ].usPeriodTime			=	pstPSI->usPeriodTime;

		HxSTD_memcpy( &pContext->astSetupPSIList[ pstPSI->eTransPSITableType ].stPSIData, &pstPSI->stPSIData, sizeof(PalTransc_PSIData_t) );
	}

	return hResult;
}

STATIC HERROR pal_transc_cmdResetPSI( palTransc_Context_t *pContext, HUINT16 usPid)
{
	HERROR hResult = ERR_OK;

	HINT32 idxPSI;

	idxPSI = pal_transc_utilFindPSIListIndexbyPid(usPid, pContext->astSetupPSIList);
	if( -1 == idxPSI ) //can't find item
		return ERR_FAIL;

	pContext->astSetupPSIList[idxPSI].stPSIData.aucRawSectionBuffer[0] = 0x00;

	hResult = pal_transc_logicUpdatePSI( pContext->stSetupInfo.ulDeviceId, &pContext->astSetupPSIList[idxPSI] );
	if( ERR_OK == hResult )
		pal_transc_resetPSI(&pContext->astSetupPSIList[idxPSI]);

	return hResult;
}

STATIC HERROR pal_transc_cmdProbeStart( palTransc_Context_t *pContext  )
{
	HERROR hResult = ERR_OK;
	DI_ERR_CODE hDiResult;
	DI_VIDEO_FORMAT eVideoCodec;

	hResult = pal_transc_logicStartDemux (pContext, TRUE);
	if( ERR_OK != hResult )
		return ERR_FAIL;

	eVideoCodec = pal_transc_convertPal2Di_VideoFormat(pContext->stSetupInfo.eInputVideoFormat);

	HxLOG_Debug("DI_TRANSCODER_ProbeStart call\n");

	hDiResult = DI_TRANSCODER_ProbeStart(pContext->stSetupInfo.ulDeviceId, pContext->stSetupInfo.ulDemuxId, eVideoCodec);
	if( DI_ERR_OK != hDiResult )
	{
		ERR_LOG ("DI_TRANSCODER_ProbeStart(ulDeviceId(%d)) : @RET (%d) \n",  pContext->stSetupInfo.ulDeviceId, (HINT32)hDiResult);
		return ERR_FAIL;
	}

	return hResult;
}

STATIC HERROR pal_transc_cmdProbeApply( palTransc_Context_t *pContext  )
{
	DI_ERR_CODE hDiResult;

	if( NULL == pContext->pstProbeinfo )
	{
		pContext->pstProbeinfo = (DI_TRANSCODER_DEC_StreamInfo_t*)HLIB_STD_MemAlloc(sizeof(DI_TRANSCODER_DEC_StreamInfo_t));
		if( NULL == pContext->pstProbeinfo )
			return ERR_FAIL;
	}

	HxLOG_Debug("DI_TRANSCODER_GetDecSequenceHeaderInfo call\n");

	hDiResult = DI_TRANSCODER_GetDecSequenceHeaderInfo(pContext->stSetupInfo.ulDeviceId, pContext->pstProbeinfo);
	if( DI_ERR_OK != hDiResult )
	{
		ERR_LOG ("DI_TRANSCODER_GetDecSequenceHeaderInfo(ulDeviceId(%d)) : @RET (%d) \n",   pContext->stSetupInfo.ulDeviceId, (HINT32)hDiResult);
		return ERR_FAIL;
	}

	/*apply information*/
	pContext->bProbed = TRUE;


#if defined(CONFIG_DEBUG)
	pal_transc_debugPrintProbeInfo( pContext->pstProbeinfo );
#endif

	return ERR_OK;
}

STATIC HERROR pal_transc_cmdProbeStop( palTransc_Context_t *pContext  )
{
	HERROR hResult = ERR_OK;
	DI_ERR_CODE hDiResult;

	HxLOG_Debug("DI_TRANSCODER_ProbeStop call\n");

	hDiResult = DI_TRANSCODER_ProbeStop(pContext->stSetupInfo.ulDeviceId);
	if( DI_ERR_OK != hDiResult )
	{
		ERR_LOG ("DI_TRANSCODER_ProbeStop(ulDeviceId(%d)) : @RET (%d) \n",  pContext->stSetupInfo.ulDeviceId, (HINT32)hDiResult);
		return ERR_FAIL;
	}

	hResult = pal_transc_logicTranscoderStopDemux(pContext->stSetupInfo.ulDemuxId, pContext->stSetupInfo.ulPidBitmask);
	if( ERR_OK != hResult  )
		return hResult;

	HxLOG_Debug("DI_TRANSCODER_ProbeClose call\n");

	hDiResult = DI_TRANSCODER_ProbeClose(pContext->stSetupInfo.ulDeviceId);
	if( DI_ERR_OK != hDiResult )
	{
		ERR_LOG ("DI_TRANSCODER_ProbeClose(ulDeviceId(%d)) : @RET (%d) \n",   pContext->stSetupInfo.ulDeviceId, (HINT32)hDiResult);
		return ERR_FAIL;
	}

	return hResult;
}

#define ___PUBLIC_FUNCTIONS___
/*------------------------------------------------------------------------------
	@note : initalize
*/
HERROR PAL_TRANSC_Init(void)
{
	HERROR hResult = ERR_OK;
	LOG_BEGIN;

	{
		palTransc_Status_t * pStatus = pal_transc_getStatus();

		hResult  = pal_transc_cmdInit(pStatus);
		if( ERR_OK != hResult )
		{
			hResult = ERR_FAIL;
			ERR_LOG ("Failed Init. RET(%d)\n",  hResult);
		}

#if defined(CONFIG_DEBUG)
		/*humax command*/
		pal_transc_commandRegister();
#endif
	}

	LOG_END;
	return hResult;
}

/*------------------------------------------------------------------------------
	@note : register probe event callback function
*/
HERROR PAL_TRANSC_RegisterHandleProbeEvent(HUINT32 ulDeviceId, PalTransc_ProbeEventCallback_t pfHandleProbeEvent)
{
	HERROR	 hResult = ERR_OK;
	LOG_BEGIN;

	if( NULL != pfHandleProbeEvent )
	{
		palTransc_Context_t *pContext;

		palTransc_ENTER_INST;
		pContext = pal_transc_getContext(ulDeviceId);
		palTransc_LEAVE_INST;

		if( NULL != pContext )
		{
			DI_ERR_CODE hDiResult;

			/*register probe event callback function*/
			HxLOG_Debug("DI_TRANSCODER_RegisterEventCallback call, DI_TRANSCODER_NOTIFY_EVT_DEC_SEQUENCE_HEADER_INFO\n");

			hDiResult = DI_TRANSCODER_RegisterEventCallback(ulDeviceId, DI_TRANSCODER_NOTIFY_EVT_DEC_SEQUENCE_HEADER_INFO, pal_transc_HandleProbeEvent);
			if( DI_ERR_OK != hDiResult )
			{
				ERR_LOG ("DI_TRANSCODER_RegisterEventCallback(ulDeviceId(%d), Event(%d)) : @RET (%d) \n",  ulDeviceId, DI_TRANSCODER_NOTIFY_EVT_DEC_SEQUENCE_HEADER_INFO, (HINT32)hDiResult);
				return ERR_FAIL;
			}

			HxLOG_Debug("DI_TRANSCODER_RegisterEventCallback call, DI_TRANSCODER_NOTIFY_EVT_DEC_PROBE_FAIL\n");

			hDiResult = DI_TRANSCODER_RegisterEventCallback(ulDeviceId, DI_TRANSCODER_NOTIFY_EVT_DEC_PROBE_FAIL, pal_transc_HandleProbeEvent);
			if( DI_ERR_OK != hDiResult )
			{
				ERR_LOG ("DI_TRANSCODER_RegisterEventCallback(ulDeviceId(%d), Event(%d)) : @RET (%d) \n",  ulDeviceId, DI_TRANSCODER_NOTIFY_EVT_DEC_PROBE_FAIL, (HINT32)hDiResult);
				return ERR_FAIL;
			}

			HxLOG_Debug("DI_TRANSCODER_RegisterEventCallback call, DI_TRANSCODER_NOTIFY_EVT_DEC_PROBE_SUCCESS\n");

			hDiResult = DI_TRANSCODER_RegisterEventCallback(ulDeviceId, DI_TRANSCODER_NOTIFY_EVT_DEC_PROBE_SUCCESS, pal_transc_HandleProbeEvent);
			if( DI_ERR_OK != hDiResult )
			{
				ERR_LOG ("DI_TRANSCODER_RegisterEventCallback(ulDeviceId(%d), Event(%d)) : @RET (%d) \n",  ulDeviceId, DI_TRANSCODER_NOTIFY_EVT_DEC_PROBE_SUCCESS, (HINT32)hDiResult);
				return ERR_FAIL;
			}

			if( DI_ERR_OK == hDiResult )
			{
				palTransc_ENTER_CTX_INST(pContext);
				pContext->pfHandleProbeEvent = pfHandleProbeEvent;
				palTransc_LEAVE_CTX_INST(pContext);

				LOG ("[%s:%d] Set Callback ulDeviceId(%d) pfHandleProbeEvent(0x%x)\n", __FUNCTION__, __LINE__,  ulDeviceId, pfHandleProbeEvent);
			}
			else
			{
				hResult = ERR_FAIL;
				ERR_LOG ("Failed Register Callback\n");
			}
		}
		else
		{
			hResult = ERR_FAIL;
			ERR_LOG ("No Context\n");
		}
	}
	else
	{
		hResult = ERR_FAIL;
		ERR_LOG ("Incorrect function argument\n");
	}

	LOG_END;
	return hResult;
}
/*------------------------------------------------------------------------------
	@note : register data receive callback function
*/
HERROR PAL_TRANSC_RegisterHandleRecMem( HUINT32 ulRecDeviceId,  PalTransc_RecMemCallback_t pfHandleRecMem)
{
	HERROR	 hResult = ERR_OK;
	LOG_BEGIN;

	if( NULL != pfHandleRecMem )
	{
		palTransc_Context_t *pContext;

		palTransc_ENTER_INST;
		pContext = pal_transc_getContextByRecDeviceId(ulRecDeviceId);
		palTransc_LEAVE_INST;

		if( NULL != pContext )
		{
			if (pContext->pfHandleRecMem == NULL)
			{
				DI_ERR_CODE hDiResult;
				hDiResult = DI_TRANSCODE_RegisterCallback(ulRecDeviceId, pal_transc_HandleRecMem);

				if( DI_ERR_OK == hDiResult )
				{
					palTransc_ENTER_CTX_INST(pContext);
					pContext->pfHandleRecMem = pfHandleRecMem;
					palTransc_LEAVE_CTX_INST(pContext);

					LOG ("[%s:%d] Set Callback ulRecDeviceId(%d) pfnHandleMemRec(0x%x)\n", __FUNCTION__, __LINE__,  ulRecDeviceId, pfHandleRecMem);
				}
				else
				{
					hResult = ERR_FAIL;
					ERR_LOG ("Failed Register Callback\n");
				}
			}
			else
			{
				hResult = ERR_FAIL;
				ERR_LOG ("Already register Callback\n");
			}

		}
		else
		{
			hResult = ERR_FAIL;
			ERR_LOG ("No Callback\n");
		}
	}
	else
	{
		hResult = ERR_FAIL;
		ERR_LOG ("Incorrect function argument\n");
	}

	LOG_END;
	return hResult;
}

/*------------------------------------------------------------------------------
	@note : register data receive callback function
*/
HERROR PAL_TRANSC_UnRegisterHandleRecMem( HUINT32 ulRecDeviceId,  PalTransc_RecMemCallback_t pfHandleRecMem)
{
	HERROR	 hResult = ERR_OK;
	LOG_BEGIN;

	/*logic*/
	if( NULL != pfHandleRecMem )
	{
		palTransc_Context_t *pContext;

		palTransc_ENTER_INST;
		pContext = pal_transc_getContextByRecDeviceId(ulRecDeviceId);
		palTransc_LEAVE_INST;

		if( NULL != pContext )
		{
			if (pContext->pfHandleRecMem == pfHandleRecMem)
			{
				palTransc_ENTER_CTX_INST(pContext);
				pContext->pfHandleRecMem = NULL;
				palTransc_LEAVE_CTX_INST(pContext);

			}
			else
			{
				hResult =  ERR_FAIL;
				ERR_LOG ("Incorrect Callback\n");
			}
		}
		else
		{
			hResult = ERR_FAIL;
			ERR_LOG ("No Context\n");
		}
	}
	else
	{
		hResult = ERR_FAIL;
		ERR_LOG ("Incorrect function argument\n");
	}

	/*end*/
	LOG_END;
	return hResult;
}

#if defined(SUPPORT_IDX_RECMEM)
/*------------------------------------------------------------------------------
	@note : register index data receive callback function
*/
HERROR PAL_TRANSC_RegisterHandleIdxRecMem( HUINT32 ulRecDeviceId,  PalTransc_IdxRecMemCallback_t pfHandleIdxRecMem)
{
	HERROR	 hResult = ERR_OK;
	LOG_BEGIN;

	if( NULL != pfHandleIdxRecMem )
	{
		palTransc_Context_t *pContext;

		palTransc_ENTER_INST;
		pContext = pal_transc_getContextByRecDeviceId(ulRecDeviceId);
		palTransc_LEAVE_INST;

		if( NULL != pContext )
		{
			if (pContext->pfHandleIdxRecMem == NULL)
			{
				DI_ERR_CODE hDiResult;
				hDiResult = DI_TRANSCODE_IndexRegisterCallback(ulRecDeviceId, pal_transc_HandleIdxRecMem);

				if( DI_ERR_OK == hDiResult )
				{
					palTransc_ENTER_CTX_INST(pContext);
					pContext->pfHandleIdxRecMem = pfHandleIdxRecMem;
					palTransc_LEAVE_CTX_INST(pContext);

					LOG ("[%s:%d] Set Callback ulRecDeviceId(%d) pfHandleIdxRecMem(0x%x)\n", __FUNCTION__, __LINE__,  ulRecDeviceId, pfHandleIdxRecMem);
				}
				else
				{
					hResult = ERR_FAIL;
					ERR_LOG ("Failed Register Callback\n");
				}
			}
			else
			{
				hResult = ERR_FAIL;
				ERR_LOG ("Already register Callback\n");
			}

		}
		else
		{
			hResult = ERR_FAIL;
			ERR_LOG ("No Callback\n");
		}
	}
	else
	{
		hResult = ERR_FAIL;
		ERR_LOG ("Incorrect function argument\n");
	}

	LOG_END;
	return hResult;
}

/*------------------------------------------------------------------------------
	@note : register index data receive callback function
*/
HERROR PAL_TRANSC_UnRegisterHandleIdxRecMem( HUINT32 ulRecDeviceId,  PalTransc_IdxRecMemCallback_t pfHandleIdxRecMem)
{
	HERROR	 hResult = ERR_OK;
	LOG_BEGIN;

	/*logic*/
	if( NULL != pfHandleIdxRecMem )
	{
		palTransc_Context_t *pContext;

		palTransc_ENTER_INST;
		pContext = pal_transc_getContextByRecDeviceId(ulRecDeviceId);
		palTransc_LEAVE_INST;

		if( NULL != pContext )
		{
			if (pContext->pfHandleIdxRecMem == pfHandleIdxRecMem)
			{
				palTransc_ENTER_CTX_INST(pContext);
				pContext->pfHandleIdxRecMem = NULL;
				palTransc_LEAVE_CTX_INST(pContext);

			}
			else
			{
				hResult =  ERR_FAIL;
				ERR_LOG ("Incorrect Callback\n");
			}
		}
		else
		{
			hResult = ERR_FAIL;
			ERR_LOG ("No Context\n");
		}
	}
	else
	{
		hResult = ERR_FAIL;
		ERR_LOG ("Incorrect function argument\n");
	}

	/*end*/
	LOG_END;
	return hResult;
}
#endif

/*------------------------------------------------------------------------------
	@note : get transcoder number
*/
HERROR PAL_TRANSC_GetNumOfDevice(HUINT32 *pulNumOfDevice)
{
	HERROR	 hResult = ERR_OK;
	LOG_BEGIN;

	if( NULL != pulNumOfDevice )
	{
		palTransc_Status_t * pStatus;

		palTransc_ENTER_INST;
		pStatus = pal_transc_getStatus();
		palTransc_LEAVE_INST;

		if( NULL != pStatus )
		{
			*pulNumOfDevice = pStatus->ulContextCnt;
			LOG ("[%s:%d] Transcoder number of device (%d)\n", __FUNCTION__, __LINE__,  *pulNumOfDevice);
		}
		else
		{
			hResult = ERR_FAIL;
			ERR_LOG ("PAL Transcoder isn't Initalize\n");
		}

	}
	else
	{
		hResult = ERR_FAIL;
		ERR_LOG ("Incorrect function argument\n");
	}

	LOG_END;
	return hResult;
}

/*------------------------------------------------------------------------------
	@note : get transcoder number in play
*/
HERROR PAL_TRANSC_GetNumOfDeviceInPlay(HUINT32 *pulNumOfDevice)
{
	HERROR	 hResult = ERR_OK;
	LOG_BEGIN;

	if( NULL != pulNumOfDevice )
	{
		HINT32 i;
		HINT32 nPlayCount = 0;
		palTransc_Status_t * pStatus;

		palTransc_ENTER_INST;
		pStatus = pal_transc_getStatus();
		palTransc_LEAVE_INST;

		if( NULL != pStatus )
		{
			palTransc_Context_t *pContext;

			for( i=0 ; i<pStatus->ulContextCnt ; i++ )
			{
				pContext = pal_transc_getContext(i);
				if( NULL != pContext )
				{
					if( TRUE == pContext->bStarted ) nPlayCount++;
				}
			}

			*pulNumOfDevice = nPlayCount;
		}
		else
		{
			hResult = ERR_FAIL;
			ERR_LOG ("PAL Transcoder isn't Initalize\n");
		}

	}
	else
	{
		hResult = ERR_FAIL;
		ERR_LOG ("Incorrect function argument\n");
	}

	LOG_END;
	return hResult;
}

/*------------------------------------------------------------------------------
	@note : get transcoder capability
*/
HERROR PAL_TRANSC_GetCapability(HUINT32 ulDeviceId, PalTransc_Capability_t *pstCapability)
{
	HERROR	 hResult = ERR_OK;
	LOG_BEGIN;

	if( NULL != pstCapability )
	{
		HUINT32 ulNumOfDevice = 0;
		DI_ERR_CODE hDiResult = DI_ERR_OK;
		DI_TRANSCODER_CAP_t ppCapInfo[palTransc_MAX_TRANSCODER_CNT];

		HxSTD_memset(&ppCapInfo, 0, sizeof(ppCapInfo));
		hDiResult = DI_TRANSCODER_GetCapability(&ulNumOfDevice, (DI_TRANSCODER_CAP_t**)&ppCapInfo);
		if( DI_ERR_OK  == hDiResult )
		{
			pstCapability->ulGroupId = ppCapInfo[ulDeviceId].ulGroupId; // why need ?
			pstCapability->ulDeviceId = ppCapInfo[ulDeviceId].ulTranscoderId; // why need ?
			pstCapability->eVideoFormat = pal_transc_convertDi2Pal_VideoFormat(ppCapInfo[ulDeviceId].eVideoFormat);
			pstCapability->eAudioFormat = pal_transc_convertDi2Pal_AudioFormat(ppCapInfo[ulDeviceId].eAudioFormat);
			pstCapability->eTranscoderMuxType = pal_transc_convertDi2Pal_MuxType(ppCapInfo[ulDeviceId].eTranscoderMuxType);
		}
		else
		{
			hResult =  ERR_FAIL;
			ERR_LOG (" DI_TRANSCODER_GetCapability() : @RET (%d) \n",  (HINT32)hDiResult);
		}
	}
	else
	{
		hResult = ERR_FAIL;
		ERR_LOG ("Incorrect function argument\n");
	}

	LOG_END;
	return hResult;
}

/*------------------------------------------------------------------------------
	@note : setup transcoder
*/
HERROR PAL_TRANSC_Setup( HUINT32 ulDeviceId, PalTransc_Setup_t * pstSetting)
{
	HERROR	 hResult = ERR_OK;
	LOG_BEGIN;

	if( NULL != pstSetting )
	{
		palTransc_Context_t *pContext;

		palTransc_ENTER_INST;
		pContext = pal_transc_getContext(ulDeviceId);
		palTransc_LEAVE_INST;

		if( NULL != pContext  )
		{
			/*save*/
			palTransc_ENTER_CTX_INST(pContext);
			HxSTD_memcpy( &pContext->stSetupInfo, pstSetting, sizeof(PalTransc_Setup_t) );
			palTransc_LEAVE_CTX_INST(pContext);
		}
		else
		{
			hResult = ERR_FAIL;
			ERR_LOG ("No Context\n");
		}

	}
	else
	{
		hResult = ERR_FAIL;
		ERR_LOG ("Incorrect function argument\n");
	}

	LOG_END;
	return hResult;
}

/*------------------------------------------------------------------------------
	@note : change profile
*/
HERROR PAL_TRANSC_ChangeProfile( HUINT32 ulDeviceId, PalTransc_ProfileItem_t	*pstProfile)
{
	HERROR	 hResult = ERR_OK;
	LOG_BEGIN;

	if( NULL != pstProfile )
	{
		palTransc_Context_t *pContext;

		palTransc_ENTER_INST;
		pContext = pal_transc_getContext(ulDeviceId);
		palTransc_LEAVE_INST;

		if( NULL != pContext )
		{
			palTransc_ENTER_CTX_INST(pContext);

			hResult = pal_transc_cmdChangeProfile(pContext, pstProfile);

			palTransc_LEAVE_CTX_INST(pContext);
		}
		else
		{
			hResult = ERR_FAIL;
			ERR_LOG ("No Context\n");
		}

	}
	else
	{
		hResult = ERR_FAIL;
		ERR_LOG ("Incorrect function argument\n");
	}

	LOG_END;
	return hResult;
}

/*------------------------------------------------------------------------------
	@note : get PSI
*/
HERROR PAL_TRANSC_GetPSI( HUINT32 ulDeviceId, HUINT16 usPid, PalTransc_PSIData_t  *pstPSIData)
{
	HERROR	 hResult = ERR_OK;
	LOG_BEGIN;

	if( NULL != pstPSIData )
	{
		palTransc_Context_t *pContext;

		palTransc_ENTER_INST;
		pContext = pal_transc_getContext(ulDeviceId);
		palTransc_LEAVE_INST;

		if( NULL  != pContext )
		{
			palTransc_ENTER_CTX_INST(pContext);

			hResult = pal_transc_cmdGetPSI(pContext, usPid, pstPSIData);

			palTransc_LEAVE_CTX_INST(pContext);
		}
		else
		{
			hResult = ERR_FAIL;
			ERR_LOG ("No Context\n");
		}

	}
	else
	{
		hResult = ERR_FAIL;
		ERR_LOG ("Incorrect function argument\n");
	}

	LOG_END;
	return hResult;
}

/*------------------------------------------------------------------------------
	@note : set PSI
*/
HERROR PAL_TRANSC_SetPSI( HUINT32 ulDeviceId, PalTransc_PSI_t *pstPSI)
{
	HERROR	 hResult = ERR_OK;
	LOG_BEGIN;

	if( NULL != pstPSI)
	{
		palTransc_Context_t *pContext;

		palTransc_ENTER_INST;
		pContext = pal_transc_getContext(ulDeviceId);
		palTransc_LEAVE_INST;

		if( NULL != pContext )
		{
			palTransc_ENTER_CTX_INST(pContext);

			hResult = pal_transc_cmdSetPSI(pContext, pstPSI);

			palTransc_LEAVE_CTX_INST(pContext);
		}
		else
		{
			hResult = ERR_FAIL;
			ERR_LOG ("No Context\n");
		}

	}

	LOG_END;
	return hResult;
}

/*------------------------------------------------------------------------------
	@note : reset PSI
*/
HERROR PAL_TRANSC_ResetPSI( HUINT32 ulDeviceId, HUINT16 usPid)
{
	HERROR	 hResult = ERR_OK;
	LOG_BEGIN;

	{
		palTransc_Context_t *pContext;

		palTransc_ENTER_INST;
		pContext = pal_transc_getContext(ulDeviceId);
		palTransc_LEAVE_INST;

		if( NULL != pContext )
		{
			palTransc_ENTER_CTX_INST(pContext);

			pal_transc_cmdResetPSI(pContext, usPid);

			palTransc_LEAVE_CTX_INST(pContext);
		}
		else
		{
			hResult = ERR_FAIL;
			ERR_LOG ("No Context\n");
		}

	}

	LOG_END;
	return hResult;
}


/*------------------------------------------------------------------------------
	@note : start, return uniqueid
*/
HERROR PAL_TRANSC_Start( HUINT32 ulDeviceId , HUINT32 *pulUniqueId)
{
	HERROR hResult = ERR_OK;
	LOG_BEGIN;

	if(  NULL  != pulUniqueId )
	{
		palTransc_Context_t *pContext;

		palTransc_ENTER_INST;
		pContext = pal_transc_getContext(ulDeviceId);
		palTransc_LEAVE_INST;

		if( NULL != pContext  )
		{
#if defined(CONFIG_DEBUG)
			pal_transc_debugPrintContext( pContext );
#endif
			palTransc_TRACE_TIME_CHECK(ulDeviceId);

			palTransc_ENTER_CTX_INST(pContext);

			hResult = pal_transc_cmdStart( pContext );
			/*return uniqueID*/
			*pulUniqueId = pContext->ulUniqueId = ulDeviceId; //TBD : 추후에 DI에서 UniqueID 할당 받아야함.


			palTransc_LEAVE_CTX_INST(pContext);
		}
		else
		{
			hResult = ERR_FAIL;
			ERR_LOG ("No Context\n");
		}
	}

	/*end*/
	LOG_END;
	return hResult;
}

/*------------------------------------------------------------------------------
	@note : stop
*/
HERROR PAL_TRANSC_Stop( HUINT32 ulDeviceId )
{
	HERROR hResult = ERR_OK;
	LOG_BEGIN;

	{
		palTransc_Context_t *pContext;

		palTransc_ENTER_INST;
		pContext = pal_transc_getContext(ulDeviceId);
		palTransc_LEAVE_INST;

		if( NULL != pContext )
		{
			palTransc_ENTER_CTX_INST(pContext);

			hResult = pal_transc_cmdStop(pContext);

			palTransc_LEAVE_CTX_INST(pContext);
		}
		else
		{
			hResult = ERR_FAIL;
			ERR_LOG ("No Context\n");
		}
	}

	/*end*/
	LOG_END;
	return hResult;
}

/*------------------------------------------------------------------------------
	@note : restart
*/
HERROR PAL_TRANSC_Restart( HUINT32 ulDeviceId )
{
	HERROR	 hResult = ERR_OK;
	LOG_BEGIN;

	{
		palTransc_Context_t *pContext;

		palTransc_ENTER_INST;
		pContext = pal_transc_getContext(ulDeviceId);
		palTransc_LEAVE_INST;

		if( NULL != pContext )
		{
#if defined(CONFIG_DEBUG)
			pal_transc_debugPrintContext( pContext );
#endif

			palTransc_ENTER_CTX_INST(pContext);

			hResult = pal_transc_cmdStop( pContext);

			hResult = pal_transc_cmdStart( pContext  );

			palTransc_LEAVE_CTX_INST(pContext);
		}
		else
		{
			hResult = ERR_FAIL;
			ERR_LOG ("No Context\n");
		}

	}

	LOG_END;
	return hResult;
}

/*------------------------------------------------------------------------------
	@note : probe start
*/
HERROR PAL_TRANSC_ProbeStart( HUINT32 ulDeviceId)
{
	HERROR hResult = ERR_OK;
	palTransc_Context_t *pContext	=	NULL;
	LOG_BEGIN;

	palTransc_ENTER_INST;
	pContext = pal_transc_getContext(ulDeviceId);
	palTransc_LEAVE_INST;

	if( NULL != pContext  )
	{
		palTransc_ENTER_CTX_INST(pContext);

		hResult = pal_transc_cmdProbeStart( pContext );

		palTransc_LEAVE_CTX_INST(pContext);
	}
	else
	{
		hResult = ERR_FAIL;
		ERR_LOG ("No Context\n");
	}

	/*end*/
	LOG_END;
	return hResult;
}

/*------------------------------------------------------------------------------
	@note : probe stop
*/
HERROR PAL_TRANSC_ProbeStop( HUINT32 ulDeviceId)
{
	HERROR hResult 				=	ERR_OK;
	palTransc_Context_t *pContext	=	NULL;
	LOG_BEGIN;

	palTransc_ENTER_INST;
	pContext = pal_transc_getContext(ulDeviceId);
	palTransc_LEAVE_INST;

	if( NULL != pContext  )
	{
		palTransc_ENTER_CTX_INST(pContext);

		hResult = pal_transc_cmdProbeStop( pContext );

		palTransc_LEAVE_CTX_INST(pContext);
	}
	else
	{
		hResult = ERR_FAIL;
		ERR_LOG ("No Context\n");
	}

	/*end*/
	LOG_END;
	return hResult;
}

/*------------------------------------------------------------------------------
	@note : probe apply
*/
HERROR PAL_TRANSC_ProbeApply( HUINT32 ulDeviceId, HUINT32	ulFlags)
{
	HERROR hResult = ERR_OK;
	palTransc_Context_t *pContext	=	NULL;
	LOG_BEGIN;

	palTransc_ENTER_INST;
	pContext = pal_transc_getContext(ulDeviceId);
	palTransc_LEAVE_INST;

	if( NULL != pContext  )
	{
		palTransc_ENTER_CTX_INST(pContext);

		hResult = pal_transc_cmdProbeApply( pContext );
		pContext->ulProbeFlags = ulFlags;
		palTransc_LEAVE_CTX_INST(pContext);
	}
	else
	{
		hResult = ERR_FAIL;
		ERR_LOG ("No Context\n");
	}

	/*end*/
	LOG_END;
	return hResult;
}

/*------------------------------------------------------------------------------
	@note : probe fail
*/
HERROR PAL_TRANSC_ProbeFail( HUINT32 ulDeviceId)
{
	HERROR hResult 				=	ERR_OK;
	palTransc_Context_t *pContext	=	NULL;
	LOG_BEGIN;

	palTransc_ENTER_INST;
	pContext = pal_transc_getContext(ulDeviceId);
	palTransc_LEAVE_INST;

	if( NULL != pContext  )
	{
		palTransc_ENTER_CTX_INST(pContext);

		hResult = pal_transc_cmdProbeStop( pContext );

		palTransc_LEAVE_CTX_INST(pContext);
	}
	else
	{
		hResult = ERR_FAIL;
		ERR_LOG ("No Context\n");
	}

	/*end*/
	LOG_END;
	return hResult;
}


#define __TR_DEBUG__
#if defined(CONFIG_DEBUG)

STATIC void pal_transc_debugPrintProbeInfo( DI_TRANSCODER_DEC_StreamInfo_t *pstStreaminfo )
{
	if( pstStreaminfo != NULL )
	{
		LOG ("[ProbeInfo]\n \
\t ulWidth[%d]\n \
\t ulHeight[%d]\n \
\t ulDisplayWidth[%d]\n \
\t ulDisplayHeight[%d]\n \
\t eAspectRatio[%d]\n \
\t usSampleAspectRatioX[%d]\n \
\t usSampleAspectRatioY[%d]\n \
\t eFrameRate[%d]\n \
\t eVideoFormat[%d]\n \
\t bProgressive[%d]\n \
\t ucColorPrimarie[%d]\n \
\t eCodecLevel[%d]\n \
\t eCodecProfile[%d]\n",
			pstStreaminfo->ulWidth,
			pstStreaminfo->ulHeight,
			pstStreaminfo->ulDisplayWidth,
			pstStreaminfo->ulDisplayHeight,
			pstStreaminfo->eAspectRatio,
			pstStreaminfo->usSampleAspectRatioX,
			pstStreaminfo->usSampleAspectRatioY,
			pstStreaminfo->eFrameRate,
			pstStreaminfo->eVideoFormat,
			pstStreaminfo->bProgressive,
			pstStreaminfo->ucColorPrimarie,
			pstStreaminfo->eCodecLevel,
			pstStreaminfo->eCodecProfile);
	}
	return;
}

STATIC void pal_transc_debugPrintSettingInfo( DI_TRANSCODER_Settings_t *pstDstSetInfo  )
{
	if( pstDstSetInfo != NULL )
	{
		LOG ("[DI_Transcode Setting]\n \
\t eTransSourceType[%d]\n \
\t eTransEncodeMode[%d]\n \
\t eInputVideoFormat[%d]\n \
\t eOutputVideoFormat[%d]\n \
\t eInputAudioFormat[%d]\n \
\t eOutputAudioFormat[%d]\n \
\t bVideoDefaultProfile[%d]\n \
\t eCodecLevel[%d]\n \
\t eCodecProfile[%d]\n \
\t stVideoSettings.eRes[%d]\n \
\t stVideoSettings.eFrameRate[%d]\n \
\t stVideoSettings.stCustomRes.bCustomRes[%d]\n \
\t stVideoSettings.stCustomRes.ulWidth[%d]\n \
\t stVideoSettings.stCustomRes.ulHeight[%d]\n \
\t stVideoSettings.stCustomRes.ulRefreshRate[%d]\n \
\t stVideoSettings.stCustomRes.bInterlaced[%d]\n \
\t stVideoSettings.eAspectRatio[%d]\n \
\t stVideoSettings.stCustomRes.ulSampleAspectRatioX[%d]\n \
\t stVideoSettings.stCustomRes.ulSampleAspectRatioY[%d]\n \
\t stVideoSettings.ulMaxBitRate[%d]\n \
\t stCustomIPB.stCustomIPB.bCustomIPB[%d]\n \
\t stCustomIPB.stCustomIPB.ulFrameP[%d]\n \
\t stCustomIPB.stCustomIPB.ulFrameB[%d]\n \
\t eCodecLevel[0x%x]\n \
\t eCodecProfile[0x%x]\n \
\t ulDemuxId[0x%x]\n \
\t usVideoPid[0x%x]\n \
\t usAudioPid[0x%x]\n \
\t usPcrPid[0x%x]\n\n",
		pstDstSetInfo->eTransSourceType,
		pstDstSetInfo->eTransEncodeMode,
		pstDstSetInfo->eInputVideoFormat,
		pstDstSetInfo->eOutputVideoFormat,
		pstDstSetInfo->eInputAudioFormat,
		pstDstSetInfo->eOutputAudioFormat,
		pstDstSetInfo->bVideoDefaultProfile,
		pstDstSetInfo->eCodecLevel,
		pstDstSetInfo->eCodecProfile,
		pstDstSetInfo->stVideoSettings.eRes,
		pstDstSetInfo->stVideoSettings.eFrameRate,
		pstDstSetInfo->stVideoSettings.stCustomRes.bCustomRes,
		pstDstSetInfo->stVideoSettings.stCustomRes.ulWidth,
		pstDstSetInfo->stVideoSettings.stCustomRes.ulHeight,
		pstDstSetInfo->stVideoSettings.stCustomRes.ulRefreshRate,
		pstDstSetInfo->stVideoSettings.stCustomRes.bInterlaced,
		pstDstSetInfo->stVideoSettings.eAspectRatio,
		pstDstSetInfo->stVideoSettings.stCustomRes.ulSampleAspectRatioX,
		pstDstSetInfo->stVideoSettings.stCustomRes.ulSampleAspectRatioY,
		pstDstSetInfo->stVideoSettings.ulMaxBitRate,
		pstDstSetInfo->stVideoSettings.stCustomIPB.bCustomIPB,
		pstDstSetInfo->stVideoSettings.stCustomIPB.ulFrameP,
		pstDstSetInfo->stVideoSettings.stCustomIPB.ulFrameB,
		pstDstSetInfo->eCodecLevel,
		pstDstSetInfo->eCodecProfile,
		pstDstSetInfo->ulDemuxId,
		pstDstSetInfo->usVideoPid,
		pstDstSetInfo->usAudioPid,
		pstDstSetInfo->usPcrPid);
	}
}

STATIC void pal_transc_debugPrintContext( palTransc_Context_t *pContext  )
{

	if( pContext != NULL )
	{
	LOG ("[Context]\n \
\t ulSemId[%d]\n \
\t ulUniqueId[0x%x]\n \
\t bStarted[%d]\n \
\t bDemuxStarted[%d]\n",
					pContext->ulSemId,
					pContext->ulUniqueId,
					pContext->bStarted,
					pContext->bDemuxStarted);

	LOG ("[Status]\n \
\t eTransSourceType[%d]\n \
\t eTransEncodeMode[%d]\n \
\t eTransOutputType[%d]\n",
					pContext->stSetupInfo.eTransSourceType,
					pContext->stSetupInfo.eTransEncodeMode,
					pContext->stSetupInfo.eTransOutputType);


	LOG ("[Source]\n \
\t eInputVideoFormat[%d]\n \
\t eInputAudioFormat[%d]\n \
\t usVideoPid[0x%x]\n \
\t usAudioPid[0x%x]\n \
\t usPcrPid[0x%x]\n \
\t bPcrTsPacketInsert[0x%x]\n",
					pContext->stSetupInfo.eInputVideoFormat,
					pContext->stSetupInfo.eInputAudioFormat,
					pContext->stSetupInfo.usVideoPid,
					pContext->stSetupInfo.usAudioPid,
					pContext->stSetupInfo.usPcrPid,
					pContext->stSetupInfo.bPcrTsPacketInsert);


	LOG ("[Resource]\n \
\t ulRecDeviceId[%d]\n \
\t ulDeviceId[%d]\n \
\t ulDemuxId[%d]\n",
					pContext->stSetupInfo.ulRecDeviceId,
					pContext->stSetupInfo.ulDeviceId,
					pContext->stSetupInfo.ulDemuxId);

		if( pContext->stSetupInfo.eTransOutputType == ePAL_TRANSC_OUTPUTTYPE_FILE )
		{
			LOG ("[Ouput] filename[%s]\n",
							pContext->stSetupInfo.acuFilename);
		}
		else
		{
			LOG ("[Ouput] Ringbuffer\n");
		}

		LOG ("[Encryption Info]\n \
\t eEncType[%d]\n \
\t ucKey[%s]\n \
\t ucIv[%s]\n \
\t ucProtectionKey[%s]\n",
					pContext->stSetupInfo.stCrypto.eEncType,
					pContext->stSetupInfo.stCrypto.ucKey,
					pContext->stSetupInfo.stCrypto.ucIv,
					pContext->stSetupInfo.stCrypto.ucProtectionKey);

	}

	return ;
}

#endif /*CONFIG_DEBUG*/

#define __command__
#if defined(CONFIG_DEBUG)
STATIC HINT32 pal_transc_ProcessCommand_info(void *szArgList)
{

	
	return HxCMD_OK;
}

STATIC HINT32 pal_transc_ProcessCommand_setparam(void *szArgList)
{
	char	*aucArg=NULL;
	HUINT32 ulDeviceId;
	HINT32 nBitrate=0;
	PalTransc_ProfileItem_t stProfile;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
	{
		return HxCMD_ERR;
	}

	ulDeviceId = atoi(aucArg);

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
	{
		return HxCMD_ERR;
	}
	
	nBitrate = atoi(aucArg);

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
	{
		return HxCMD_ERR;
	}
	
	stProfile.nScreenWidth= atoi(aucArg);

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
	{
		return HxCMD_ERR;
	}

	stProfile.nScreenHeight = atoi(aucArg);

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
	{
		return HxCMD_ERR;
	}

	stProfile.nFrameRate = atoi(aucArg);

	stProfile.nIndex = 1;

	stProfile.eVideoCodec=eDxVIDEO_CODEC_H264;
	stProfile.eAudioCodec=eDxAUDIO_CODEC_AAC;

	stProfile.nAudioBitrateKbps=64;
	stProfile.nVideoBitrateKbps=nBitrate;
	stProfile.eVideoAspectRatio=eDxASPECTRATIO_4X3;

	PAL_TRANSC_ChangeProfile( ulDeviceId, &stProfile);
	
	return HxCMD_OK;
}

#define	hCmdHandle		"pal_transcoder"
STATIC void pal_transc_commandRegister(void)
{
	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_transc_ProcessCommand_info,
						"info",
						"Print context",
						"info");
	
	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_transc_ProcessCommand_setparam,
						"setparam",
						"Set transcoding parameters",
						"setparam [deviceid] [bitrate] [width] [height] [framerate]");
}

HERROR PAL_TRANSC_RegisterCommand(void)
{
	pal_transc_commandRegister();
	return ERR_OK;
}

#endif /*CONFIG_DEBUG*/

/*********************** End of File ******************************/
