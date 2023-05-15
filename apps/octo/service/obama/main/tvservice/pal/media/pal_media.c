/** **********************************************************************************************************
	@file 		pal_media.c

	@date		2012/10/22
	@author		IPM-SW1T (tipmsw1@humaxdigital.com)
	@breif

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. ("Humax") and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*********************************************************************************************************** */


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <dlib.h>
#include <vkernel.h>
#include <octo_common.h>
#include <di_err.h>

#include <di_media_types.h>
#include <di_media20.h>
#include <di_drm.h>
#include <di_video.h>
#include <di_crypt.h>
#include <pal_video.h>
#include <pal_audio.h>
#include <pal_media.h>
#include <pal_output.h>

#include "pal_media_sub.h"

#include <get_enum_str.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#undef	HxLOG_DOMAIN
#define HxLOG_DOMAIN	DOMAIN_TVSVC_PAL_MEDIA

#if defined(CONFIG_DEBUG)
#define FuncEnter           HxLOG_Info("(+)Enter!!\n");
#define FuncLeave 		    HxLOG_Info("(-)Leave -- \n");
#else
#define FuncEnter           OCTO_NULL_PRINT
#define FuncLeave	        OCTO_NULL_PRINT
#endif

#define PAL_MEDIA_MAX_VIEW_NUM	1
#define PAL_MEDIA_AV_SEMAPHORE_NAME			"PalMediaAVSem"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#define PAL_MEDIA_ASSERT(x)     \
	    if ( x == NULL )\
	    {\
		        HxLOG_Error("PAL_MEDIA: the param is NULL.\n");\
		        return ERR_FAIL;\
	    }\

typedef struct _pal_media_info_t {
	PAL_MEDIA_HANDLE 	handle;
	void				*sessionId;
	HUINT32				mediaId;
} pal_media_info_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
/** Static Event Callnack for mw */
static DI_MEDIA_ProbeCallback_t			s_tProbeCallback;
static PAL_MEDIA_EventCallback_t   		s_tEventCallback;
static DI_MEDIA_BufferCallback_t		s_tBufferingCallback;

/* to save playing media handle now */
//static PAL_MEDIA_t 						*s_hSessionPlayInfo[PAL_MEDIA_MAX_VIEW_NUM];

static pal_media_info_t			*s_media_infos[PAL_MEDIA_MAX_VIEW_NUM];

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
static int __pmedia_info_count(void)
{
	HINT32 i = 0;
	HINT32 count = 0;

	for ( i=0; i<PAL_MEDIA_MAX_VIEW_NUM; ++i )
	{
		if ( s_media_infos[i] ) count++;
	}

	return count;
}


static void __pmedia_add_info(PAL_MEDIA_HANDLE handle, void *hSession)
{
	HINT32 i = 0;

	for ( i=0; i<PAL_MEDIA_MAX_VIEW_NUM; ++i )
	{
		if ( s_media_infos[i] == NULL )
		{
			pal_media_info_t *info = HLIB_STD_MemAlloc(sizeof(pal_media_info_t));
			if ( info == NULL ) return;

			info->handle = handle;
			info->sessionId = hSession;
			info->mediaId = 0;
			s_media_infos[i] = info;

			__pmedia_info_count();

			return;
		}
	}
			__pmedia_info_count();
}

static void __pmedia_del_info(PAL_MEDIA_HANDLE handle)
{
	HINT32 i = 0;

	for ( i=0; i<PAL_MEDIA_MAX_VIEW_NUM; ++i )
	{
		if ( s_media_infos[i] && s_media_infos[i]->handle == handle )
		{
			HLIB_STD_MemFree(s_media_infos[i]);
			s_media_infos[i] = NULL;
			__pmedia_info_count();
			return;
		}
	}
			__pmedia_info_count();
}

static void __pmedia_del_all(void)
{
	HINT32 i = 0;

	for ( i=0; i<PAL_MEDIA_MAX_VIEW_NUM; ++i )
	{
		if ( s_media_infos[i] )
		{
			HLIB_STD_MemFree(s_media_infos[i]);
			s_media_infos[i] = NULL;
		}
	}

}

static pal_media_info_t *__pmedia_find_info(PAL_MEDIA_HANDLE handle)
{
	HINT32 i = 0;

	for ( i=0; i<PAL_MEDIA_MAX_VIEW_NUM; ++i )
	{
		if ( s_media_infos[i] && s_media_infos[i]->handle == handle )
		{
			return s_media_infos[i];
		}
	}

	return NULL;
}

static PAL_MEDIA_HANDLE __pal_media_get_handle_by_mediaid(HUINT32 unMediaId)
{
	HINT32 i = 0;

	for ( i=0; i<PAL_MEDIA_MAX_VIEW_NUM; ++i )
	{
		if ( s_media_infos[i] && s_media_infos[i]->mediaId == unMediaId )
		{
			return s_media_infos[i]->handle;
		}
	}

	return NULL;
}

static PAL_MEDIA_HANDLE __pal_media_get_handle_by_sessionid(HUINT32 unSessionId)
{
	HINT32 i = 0;

	for ( i=0; i<PAL_MEDIA_MAX_VIEW_NUM; ++i )
	{
		if ( s_media_infos[i] && s_media_infos[i]->sessionId == (void *)unSessionId )
		{
			return s_media_infos[i]->handle;
		}
	}

	return NULL;
}

static DI_VIDEO_FORMAT __pal_media_video_codec_to_di(DxVideoCodec_e codec)
{
	DI_VIDEO_FORMAT eDiVideoCodec;

	/* Codec Conversion */
	switch(codec)
	{
		case eDxVIDEO_CODEC_MPEG1:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_MPEG1;
			break;
		case eDxVIDEO_CODEC_MPEG2:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_MPEG2;
			break;
		case eDxVIDEO_CODEC_MPEG4_PART2:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_MPEG4Part2;
			break;
		case eDxVIDEO_CODEC_H263:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_H263;
			break;
		case eDxVIDEO_CODEC_H264:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_H264;
			break;
		case eDxVIDEO_CODEC_H265:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_H265;
			break;
		case eDxVIDEO_CODEC_H264_SVC:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_H264;
			break;
		case eDxVIDEO_CODEC_H264_MVC:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_H264;
			break;
		case eDxVIDEO_CODEC_VC1:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_VC1;
			break;
		case eDxVIDEO_CODEC_VC1_SIMPLEMAIN:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_VC1SimpleMain;
			break;
		case eDxVIDEO_CODEC_AVS:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_AVS;
			break;
		case eDxVIDEO_CODEC_RV40:
			eDiVideoCodec = DI_VIDEO_FORMAT_NONE;
			break;
		case eDxVIDEO_CODEC_VP6:
			eDiVideoCodec = DI_VIDEO_FORMAT_NONE;
			break;
		case eDxVIDEO_CODEC_DIVX311:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_DIVX_311;
			break;
		default:
			eDiVideoCodec = DI_VIDEO_FORMAT_NONE;
			HxLOG_Print("not defined or supported pal codec (%s) \n", OTL_ENUM2STR_VideoCodec(codec));
			break;
	}

	return eDiVideoCodec;
}

static DxVideoCodec_e __pal_media_video_codec_to_ap(DI_VIDEO_FORMAT codec)
{
	DxVideoCodec_e eApVideoCodec;

	switch(codec)
	{
		case DI_VIDEO_FORMAT_MEDIA_MPEG1:
			eApVideoCodec = eDxVIDEO_CODEC_MPEG1;
			break;
		case DI_VIDEO_FORMAT_MEDIA_MPEG2:
			eApVideoCodec = eDxVIDEO_CODEC_MPEG2;
			break;
		case DI_VIDEO_FORMAT_MEDIA_MPEG4Part2:
			eApVideoCodec = eDxVIDEO_CODEC_MPEG4_PART2;
			break;
		case DI_VIDEO_FORMAT_MEDIA_H261:
		case DI_VIDEO_FORMAT_MEDIA_H263:
			eApVideoCodec = eDxVIDEO_CODEC_H263;
			break;
		case DI_VIDEO_FORMAT_MEDIA_H264:
			eApVideoCodec = eDxVIDEO_CODEC_H264;
			break;
		case DI_VIDEO_FORMAT_MEDIA_H265:
			eApVideoCodec = eDxVIDEO_CODEC_H265;
			break;
		case DI_VIDEO_FORMAT_MEDIA_VC1SimpleMain:
			eApVideoCodec = eDxVIDEO_CODEC_VC1_SIMPLEMAIN;
			break;
		case DI_VIDEO_FORMAT_MEDIA_VC1:
			eApVideoCodec = eDxVIDEO_CODEC_VC1;
			break;
		case DI_VIDEO_FORMAT_MEDIA_AVS:
			eApVideoCodec = eDxVIDEO_CODEC_AVS;
			break;
		case DI_VIDEO_FORMAT_MEDIA_DIVX_311:
			eApVideoCodec = eDxVIDEO_CODEC_DIVX311;
			break;
		default:
			eApVideoCodec = eDxVIDEO_CODEC_UNKNOWN;
			HxLOG_Print("not defined or supported video codec (%s) \n", codec);
			break;
	}

	return eApVideoCodec;
}

static DI_AUDIO_FORMAT __pal_media_audio_codec_to_di(DxAudioCodec_e codec)
{
	DI_AUDIO_FORMAT eDiCodec;

	switch(codec)
	{
		case eDxAUDIO_CODEC_MPEG:
			eDiCodec = DI_AUDFORMAT_MPEG;
			break;
		case eDxAUDIO_CODEC_MP3:
			eDiCodec = DI_AUDFORMAT_MP3;
			break;
		case eDxAUDIO_CODEC_DOLBY_AC3:
			eDiCodec = DI_AUDFORMAT_AC3;
			break;
		case eDxAUDIO_CODEC_DOLBY_AC3P:
			eDiCodec = DI_AUDFORMAT_AC3_PLUS;
			break;
		case eDxAUDIO_CODEC_AAC:
			eDiCodec = DI_AUDFORMAT_AAC;
			break;
		case eDxAUDIO_CODEC_AAC_LOAS:
			eDiCodec = DI_AUDFORMAT_AAC_LOAS;
			break;
		case eDxAUDIO_CODEC_AAC_PLUS:
			eDiCodec = DI_AUDFORMAT_AAC_PLUS;
			break;
		case eDxAUDIO_CODEC_AAC_PLUS_ADTS:
			eDiCodec = DI_AUDFORMAT_AAC_PLUS_ADTS;
			break;
		case eDxAUDIO_CODEC_DTS:
			eDiCodec = DI_AUDFORMAT_DTS;
			break;
		case eDxAUDIO_CODEC_DTS_HD:
			eDiCodec = DI_AUDFORMAT_DTS_HD;
			break;
		case eDxAUDIO_CODEC_LPCM_DVD:
			eDiCodec = DI_AUDFORMAT_LPCM_DVD;
			break;
		case eDxAUDIO_CODEC_LPCM_HD_DVD:
			eDiCodec = DI_AUDFORMAT_LPCM_HD_DVD;
			break;
		case eDxAUDIO_CODEC_LPCM_BLUERAY:
			eDiCodec = DI_AUDFORMAT_LPCM_BLUERAY;
			break;
		case eDxAUDIO_CODEC_DRA:
			eDiCodec = DI_AUDFORMAT_DRA;
			break;
		case eDxAUDIO_CODEC_WMA_STD:
			eDiCodec = DI_AUDFORMAT_WMA_STD;
			break;
		case eDxAUDIO_CODEC_WMA_PRO:
			eDiCodec = DI_AUDFORMAT_WMA_PRO;
			break;
		case eDxAUDIO_CODEC_PCMWAVE:
			eDiCodec = DI_AUDFORMAT_PCMWAVE;
			break;
		case eDxAUDIO_CODEC_AIFF:
		case eDxAUDIO_CODEC_PCM:
			eDiCodec = DI_AUDFORMAT_PCM;
			break;
		default:
			HxLOG_Print("not supported codec (%s) \n",  OTL_ENUM2STR_AudioCodec(codec));
			eDiCodec = DI_AUDFORMAT_MPEG;
			break;
	}

	return eDiCodec;

}

static DxAudioCodec_e __pal_media_audio_codec_to_ap(DI_AUDIO_FORMAT codec)
{
	DxAudioCodec_e eApAudioCodec;

	switch(codec)
	{
		case DI_AUDFORMAT_MPEG:
			eApAudioCodec = eDxAUDIO_CODEC_MPEG;
			break;
		case DI_AUDFORMAT_MP3:
			eApAudioCodec = eDxAUDIO_CODEC_MP3;
			break;
		case DI_AUDFORMAT_AAC:
			eApAudioCodec = eDxAUDIO_CODEC_AAC;
			break;
		case DI_AUDFORMAT_AAC_LOAS:
			eApAudioCodec = eDxAUDIO_CODEC_AAC_LOAS;
			break;
		case DI_AUDFORMAT_AAC_PLUS:
			eApAudioCodec = eDxAUDIO_CODEC_AAC_PLUS;
			break;
		case DI_AUDFORMAT_AAC_PLUS_ADTS:
			eApAudioCodec = eDxAUDIO_CODEC_AAC_PLUS_ADTS;
			break;
		case DI_AUDFORMAT_AC3:
			eApAudioCodec = eDxAUDIO_CODEC_DOLBY_AC3;
			break;
		case DI_AUDFORMAT_AC3_PLUS:
			eApAudioCodec = eDxAUDIO_CODEC_DOLBY_AC3P;
			break;
		case DI_AUDFORMAT_PCM:
			eApAudioCodec = eDxAUDIO_CODEC_PCM;
			break;
		case DI_AUDFORMAT_DTS:
			eApAudioCodec = eDxAUDIO_CODEC_DTS;
			break;
		case DI_AUDFORMAT_LPCM_HD_DVD:
			eApAudioCodec = eDxAUDIO_CODEC_LPCM_HD_DVD;
			break;
		case DI_AUDFORMAT_LPCM_BLUERAY:
			eApAudioCodec = eDxAUDIO_CODEC_LPCM_BLUERAY;
			break;
		case DI_AUDFORMAT_DTS_HD:
			eApAudioCodec = eDxAUDIO_CODEC_DTS_HD;
			break;
		case DI_AUDFORMAT_WMA_STD:
			eApAudioCodec = eDxAUDIO_CODEC_WMA_STD;
			break;
		case DI_AUDFORMAT_WMA_PRO:
			eApAudioCodec = eDxAUDIO_CODEC_WMA_PRO;
			break;
		case DI_AUDFORMAT_LPCM_DVD:
			eApAudioCodec = eDxAUDIO_CODEC_LPCM_DVD;
			break;
		case DI_AUDFORMAT_PCMWAVE:
			eApAudioCodec = eDxAUDIO_CODEC_PCMWAVE;
			break;
		case DI_AUDFORMAT_DRA:
			eApAudioCodec = eDxAUDIO_CODEC_DRA;
			break;
		default:
			HxLOG_Print("not supported codec (%s) \n",	OTL_ENUM2STR_AudioCodec(codec));
			eApAudioCodec = DI_AUDFORMAT_MPEG;
			break;
	}

	return eApAudioCodec;
}

static HINT32 __pal_media_speed_to_ap(DI_MEDIA_PlaySpeed_e eSpeed)
{
	HINT32 nApSpeed;

	switch ( eSpeed )
	{
	case eDI_MEDIA_Speed_Pause:
		nApSpeed = 0;
		break;
	case eDI_MEDIA_Speed_RW_X1:
		nApSpeed = -100;
		break;
	case eDI_MEDIA_Speed_RW_X2:
		nApSpeed = -200;
		break;
	case eDI_MEDIA_Speed_RW_X4:
		nApSpeed = -400;
		break;
	case eDI_MEDIA_Speed_RW_X8:
		nApSpeed = -800;
		break;
	case eDI_MEDIA_Speed_RW_X16:
		nApSpeed = -1600;
		break;
	case eDI_MEDIA_Speed_RW_X32:
		nApSpeed = -3200;
		break;
	case eDI_MEDIA_Speed_RW_X64:
		nApSpeed = -6400;
		break;
	case eDI_MEDIA_Speed_RW_X128:
		nApSpeed = -12800;
		break;
	case eDI_MEDIA_Speed_Normal:
		nApSpeed = 100;
		break;
	case eDI_MEDIA_Speed_FF_X2:
		nApSpeed = 200;
		break;
	case eDI_MEDIA_Speed_FF_X4:
		nApSpeed = 400;
		break;
	case eDI_MEDIA_Speed_FF_X8:
		nApSpeed = 800;
		break;
	case eDI_MEDIA_Speed_FF_X16:
		nApSpeed = 1600;
		break;
	case eDI_MEDIA_Speed_FF_X32:
		nApSpeed = 3200;
		break;
	case eDI_MEDIA_Speed_FF_X64:
		nApSpeed = 6400;
		break;
	case eDI_MEDIA_Speed_FF_X128:
		nApSpeed = 12800;
		break;
	default:
		HxLOG_Print("not supported speed (%d) \n", eSpeed);
		nApSpeed = 100;
		break;
	}

	HxLOG_Print("Speed Convert(DI->PAL): (%d -> %d) \n", eSpeed, nApSpeed);

	return nApSpeed;
}


static DI_MEDIA_PlaySpeed_e __pal_media_speed_to_di(HINT32 nSpeed)
{
	DI_MEDIA_PlaySpeed_e eDiSpeed;

	switch ( nSpeed )
	{
	case 0:
		eDiSpeed = eDI_MEDIA_Speed_Pause;
		break;
	case -100:
		eDiSpeed = eDI_MEDIA_Speed_RW_X1;
		break;
	case -200:
		eDiSpeed = eDI_MEDIA_Speed_RW_X2;
		break;
	case -400:
		eDiSpeed = eDI_MEDIA_Speed_RW_X4;
		break;
	case -800:
		eDiSpeed = eDI_MEDIA_Speed_RW_X8;
		break;
	case -1600:
		eDiSpeed = eDI_MEDIA_Speed_RW_X16;
		break;
	case -3200:
		eDiSpeed = eDI_MEDIA_Speed_RW_X32;
		break;
	case -6400:
		eDiSpeed = eDI_MEDIA_Speed_RW_X64;
		break;
	case -12800:
		eDiSpeed = eDI_MEDIA_Speed_RW_X128;
		break;
	case 100:
		eDiSpeed = eDI_MEDIA_Speed_Normal;
		break;
	case 200:
		eDiSpeed = eDI_MEDIA_Speed_FF_X2;
		break;
	case 400:
		eDiSpeed = eDI_MEDIA_Speed_FF_X4;
		break;
	case 800:
		eDiSpeed = eDI_MEDIA_Speed_FF_X8;
		break;
	case 1600:
		eDiSpeed = eDI_MEDIA_Speed_FF_X16;
		break;
	case 3200:
		eDiSpeed = eDI_MEDIA_Speed_FF_X32;
		break;
	case 6400:
		eDiSpeed = eDI_MEDIA_Speed_FF_X64;
		break;
	case 12800:
		eDiSpeed = eDI_MEDIA_Speed_FF_X128;
		break;
	default:
		HxLOG_Print("not supported speed (%d) \n", nSpeed);
		eDiSpeed = eDI_MEDIA_Speed_None;
		break;
	}

	HxLOG_Print("Speed Convert(PAL->DI): (%d -> %d) \n", nSpeed, eDiSpeed);

	return eDiSpeed;
}

static HERROR pal_media_ConvertDi2Pal_AspectRatio(DI_VIDEO_AspectRatio_t eDiAspectRatio,
		DxAspectRatio_e *pePalAspectRatio)
{
	if ( pePalAspectRatio == NULL )
	{
		return ERR_FAIL;
	}

	switch ( eDiAspectRatio )
	{
		case DI_VIDEO_ASPECTRATIO_16_9:
			*pePalAspectRatio = eDxASPECTRATIO_16X9;
			break;
		case DI_VIDEO_ASPECTRATIO_4_3:
			*pePalAspectRatio = eDxASPECTRATIO_4X3;
			break;
		case DI_VIDEO_ASPECTRATIO_14_9:
			*pePalAspectRatio = eDxASPECTRATIO_14X9;
			break;
		case DI_VIDEO_ASPECTRATIO_1_1:
			*pePalAspectRatio = eDxASPECTRATIO_1X1;
			break;
		default:
			*pePalAspectRatio = eDxASPECTRATIO_UNKNOWN;
			break;
	}

	return ERR_OK;
}

static void __pal_media_set_audio_type(void)
{
	(void)DI_AUDIO_SetMediaType(PAL_MEDIA_AUDIO_DECODER_MAIN, DI_AUDIOTYPE_MEDIA);
	(void)DI_AUDIO_SetMediaType(PAL_MEDIA_AUDIO_DECODER_SUB, DI_AUDIOTYPE_MEDIA);
}

static void __pal_media_revert_audio_type(void)
{
	(void)DI_AUDIO_SetMediaType(PAL_MEDIA_AUDIO_DECODER_MAIN, DI_AUDIOTYPE_STREAM);
	(void)DI_AUDIO_SetMediaType(PAL_MEDIA_AUDIO_DECODER_SUB, DI_AUDIOTYPE_STREAM);
}

static HERROR __pal_media_video_start(PAL_MEDIA_HANDLE phMedia)
{
	HERROR eRet = ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);

	if ( pstMediaHdl->bVideoStarted == TRUE )
	{
		HxLOG_Print("[%s] DI_VIDEO_Stop: Already Started.\n", __FUNCTION__);
		return ERR_OK;
	}

	eRet = DI_VIDEO_Start(pstMediaHdl->eVideoDecoderType,
			__pal_media_video_codec_to_di(pstMediaHdl->tMediaInfo.video_codec));
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("[%s] Fail DI_VIDEO_Start\n", __FUNCTION__);
		return ERR_FAIL;
	}

	pstMediaHdl->bVideoStarted = TRUE;

	FuncLeave;

	return ERR_OK;
}

static HERROR __pal_media_video_stop(PAL_MEDIA_HANDLE phMedia)
{
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);

	if ( pstMediaHdl->bVideoStarted == FALSE )
	{
		HxLOG_Print("[%s] DI_VIDEO_Stop: Already Stopped.\n", __FUNCTION__);
		return ERR_OK;
	}

	(void)DI_VIDEO_Stop(pstMediaHdl->eVideoDecoderType);
	pstMediaHdl->bVideoStarted = FALSE;

	FuncLeave;

	return ERR_OK;
}

static HERROR __pal_media_audio_start(PAL_MEDIA_HANDLE phMedia)
{
	HERROR eRet = ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);

	if ( pstMediaHdl->bAudioStarted == TRUE )
	{
		HxLOG_Print("[%s] DI_AUDIO_Stop: Already Started.\n", __FUNCTION__);
		return ERR_OK;
	}

	eRet = DI_AUDIO_Start(pstMediaHdl->eAudioDecoderType,
			__pal_media_audio_codec_to_di(pstMediaHdl->tMediaInfo.audio_codec));
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("[%s] Fail DI_AUDIO_Start(Main)\n", __FUNCTION__);
		return ERR_FAIL;
	}

	pstMediaHdl->bAudioStarted = TRUE;

	FuncLeave;

	return ERR_OK;
}

static HERROR __pal_media_audio_stop(PAL_MEDIA_HANDLE phMedia)
{
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);

	if ( pstMediaHdl->bAudioStarted == FALSE )
	{
		HxLOG_Print("[%s] DI_AUDIO_Stop: Already Stopped.\n", __FUNCTION__);
		return ERR_OK;
	}

	(void)DI_AUDIO_Stop(pstMediaHdl->eAudioDecoderType);
	pstMediaHdl->bAudioStarted = FALSE;

	FuncLeave;

	return ERR_OK;
}
static void __pal_media_video_callback(void *pvParam)
{
	DI_VIDEO_CALLBACK_DATA_t        *pstCallback = NULL;
	DI_VIDEO_RUNNING_STATE_EVT_t    *pstRunningStateEvt = NULL;
	PAL_MEDIA_HANDLE	hMedia = NULL;

	FuncEnter;

	hMedia = __pal_media_get_handle_by_mediaid(0);
	if ( hMedia == NULL ) return;

	pstCallback = (DI_VIDEO_CALLBACK_DATA_t *)pvParam;
	if ( pstCallback == NULL ) return;

	switch (pstCallback->eEvent)
	{
	case DI_VIDEO_NOTIFY_EVT_RUNNING_STATE_CHANGED:
		pstRunningStateEvt = (DI_VIDEO_RUNNING_STATE_EVT_t *)pstCallback->pvEventData;

		switch ( pstRunningStateEvt->state )
		{
		case DI_VIDEO_RUNNING_STATE_DECODING:
			{
				DI_VIDEO_StreamInfo_t stVideoStreamInfo;

				DI_VIDEO_GetSequenceHeaderInfo(PAL_MEDIA_VIDEO_DECODER_MAIN, &stVideoStreamInfo);

				hMedia->ulVideoStreamWidth = stVideoStreamInfo.ulWidth;
				hMedia->ulVideoStreamHeight = stVideoStreamInfo.ulHeight;
				pal_media_ConvertDi2Pal_AspectRatio(stVideoStreamInfo.eAspectRatio, &(hMedia->eVideoAspectRatio));

				s_tEventCallback(hMedia->pIdentifier, PAL_MEDIA_EVT_VIDEO_FRAMESTART, 0);
			}
			break;

		case DI_VIDEO_RUNNING_STATE_NODATA:
			s_tEventCallback(hMedia->pIdentifier, PAL_MEDIA_EVT_VIDEO_UNDERRUN, 0);
			break;

		default:
			s_tEventCallback(hMedia->pIdentifier, PAL_MEDIA_EVT_VIDEO_ERROR_NONE, 0);
			break;
		}

	default:
		break;
	}

	FuncLeave;

	return;
}

static void __pal_media_audio_callback(void *pvParam)
{
	DI_AUDIO_CALLBACK_DATA_t *pstCallback = NULL;
	PAL_MEDIA_HANDLE hMedia = NULL;

	FuncEnter;

	hMedia = __pal_media_get_handle_by_mediaid(0);
	if ( hMedia == NULL ) return;

	pstCallback = (DI_AUDIO_CALLBACK_DATA_t *)pvParam;
	if ( pstCallback == NULL ) return;

	switch (pstCallback->nEvent)
	{
	case DI_AUDIO_RUNNING_STATE_CHANGED:
		switch (((DI_AUDIO_RUNNING_STATE_EVT_t*)pstCallback->pvEventData)->state)
		{
		case DI_AUDIO_STATE_DECODING:
			s_tEventCallback(hMedia->pIdentifier, PAL_MEDIA_eEVT_AUDIO_NEWFRAME, 0);
			break;

		case DI_AUDIO_STATE_NODATA:
			s_tEventCallback(hMedia->pIdentifier, PAL_MEDIA_eEVT_AUDIO_NODATA, 0);
			break;

		default:
			break;
		}
		break;

	case DI_AUDIO_FORMAT_CHANGED:
		s_tEventCallback(hMedia->pIdentifier, PAL_MEDIA_eEVT_AUDIO_FORMAT_CHANGED, 0);
		break;

	default:
		break;
	}

	FuncLeave;
}

static void __pal_media_probe_complete_callback(HUINT32 id)
{
	HxLOG_Info("Probe is completed.  \n");
}

static void __pal_media_probe_failure_callback(HUINT32 id)
{
	HxLOG_Info("Probe is failed.  \n");
}

static void __pal_media_event_callback(HUINT32 id, HINT32 ulEvent, HUINT32 Value)
{
	PAL_MEDIA_HANDLE hMedia = __pal_media_get_handle_by_mediaid(id);
	if ( hMedia == NULL ) return;

	switch( ulEvent )
	{
	case DI_MEDIA_EVENT_CHANGEPLAYTIME:
		s_tEventCallback(hMedia->pIdentifier, PAL_MEDIA_EVT_MEDIA_CHANGEPLAYTIME, Value);
		break;

	case DI_MEDIA_EVENT_BEGINOFSTREAM:
		s_tEventCallback(hMedia->pIdentifier, PAL_MEDIA_EVT_MEDIA_BEGINOFSTREAM, 0);
		break;

	case DI_MEDIA_EVENT_ERROROFSTREAM:
		s_tEventCallback(hMedia->pIdentifier, PAL_MEDIA_EVT_MEDIA_ERROROFSTREAM, 0);
		break;

	case DI_MEDIA_EVENT_ENDOFSTREAM:
		s_tEventCallback(hMedia->pIdentifier, PAL_MEDIA_EVT_MEDIA_ENDOFSTREAM, 0);
		break;

	case DI_MEDIA_EVENT_START:
		s_tEventCallback(hMedia->pIdentifier, PAL_MEDIA_EVT_MEDIA_START, 0);
		break;

	case DI_MEDIA_EVENT_PAUSE:
		s_tEventCallback(hMedia->pIdentifier, PAL_MEDIA_EVT_MEDIA_PAUSE, 0);
		break;

	case DI_MEDIA_EVENT_PLAYING:
		s_tEventCallback(hMedia->pIdentifier, PAL_MEDIA_EVT_MEDIA_PLAYING, 0);
		break;

	case DI_MEDIA_EVENT_TRICK:
		s_tEventCallback(hMedia->pIdentifier, PAL_MEDIA_EVT_MEDIA_TRICK, 0);
		break;

	case DI_MEDIA_EVENT_STOP:
		s_tEventCallback(hMedia->pIdentifier, PAL_MEDIA_EVT_MEDIA_STOP, 0);
		break;

	default:
		HxLOG_Error("Unknown event(0x%X)n",  ulEvent);
		return;
	}
}

static void	__pal_media_buffering_start_callback(HUINT32 id)
{
	PAL_MEDIA_HANDLE hMedia = __pal_media_get_handle_by_sessionid(id);
	HxLOG_Info("Buffering is started.  \n");
	if ( hMedia == NULL )
	{
		HxLOG_Error("The Player(PAL_MEDIA_HANDLE) is NULL.  \n");
	   	return;
	}

	s_tEventCallback(hMedia->pIdentifier, PAL_MEDIA_EVT_MEDIA_BUFFER_START, 0);
}

static void __pal_media_buffering_canplay_callback(HUINT32 id)
{
	PAL_MEDIA_HANDLE hMedia = __pal_media_get_handle_by_sessionid(id);
	if ( hMedia == NULL ) return;

	s_tEventCallback(hMedia->pIdentifier, PAL_MEDIA_EVT_MEDIA_BUFFER_CANPLAY, 0);
}

static void	__pal_media_buffering_underflow_callback(HUINT32 id)
{
	PAL_MEDIA_HANDLE hMedia = __pal_media_get_handle_by_sessionid(id);
	if ( hMedia == NULL ) return;

	s_tEventCallback(hMedia->pIdentifier, PAL_MEDIA_EVT_MEDIA_BUFFER_UNDERFLOW, 0);
}

static void __pal_media_buffering_enough_callback(HUINT32 id)
{
	PAL_MEDIA_HANDLE hMedia = __pal_media_get_handle_by_sessionid(id);
	if ( hMedia == NULL ) return;

	s_tEventCallback(hMedia->pIdentifier, PAL_MEDIA_EVT_MEDIA_BUFFER_ENOUGH, 0);
}

static void	__pal_media_buffering_complete_callback(HUINT32 id)
{
	PAL_MEDIA_HANDLE hMedia = __pal_media_get_handle_by_sessionid(id);
	if ( hMedia == NULL ) return;

	s_tEventCallback(hMedia->pIdentifier, PAL_MEDIA_EVT_MEDIA_BUFFER_COMPLETE, 0);
}

static void	__pal_media_buffering_failure_callback(HUINT32 id)
{
	HxLOG_Info("PAL_MEDIA: %s : Buffering is failed.  \n", __FUNCTION__);
}

static void	__pal_media_buffering_adaptive_callback(HUINT32 id)
{
	HxLOG_Info("PAL_MEDIA: %s : Buffering Adaptive Changed.  \n", __FUNCTION__);
}

static void __pal_media_subtitle_callback(PAL_MEDIA_HANDLE phMedia, HUINT32 eStatus, HUINT8 *szStr)
{
	switch(eStatus)
	{
		case PAL_MEDIA_EVT_MEDIA_SUB_UPDATE:
			s_tEventCallback(phMedia->pIdentifier, PAL_MEDIA_EVT_MEDIA_SUB_UPDATE, (HINT32) szStr);
			break;
		default:
			break;
	}
}

static void __pal_media_error_event_callback(HUINT32 id, HUINT32 ulCategory, HUINT32 ulReason)
{
	PAL_MEDIA_HANDLE hMedia = __pal_media_get_handle_by_sessionid(id);
	if ( hMedia == NULL ) return;

	hMedia->uErrorlReason = ulReason;
}

static void __pal_media_drm_license_callback(SESSION_HANDLE id, HUINT8 *param1, HUINT32 param2)
{
	PAL_MEDIA_HANDLE hMedia = __pal_media_get_handle_by_sessionid((HUINT32)id);
	if ( hMedia == NULL ) return;

	DI_MEDIA_SetLicenseData(hMedia->pIdentifier, hMedia->szDrmLicenseData);
}

static void __pal_media_drm_output_default_process(SESSION_HANDLE id)
{
	PAL_MEDIA_HANDLE hMedia = __pal_media_get_handle_by_sessionid((HUINT32)id);
	if ( hMedia == NULL ) return;

	hMedia->stOutputControl.bUsed = TRUE;
	hMedia->stOutputControl.bHdcp = TRUE;
	hMedia->stOutputControl.bCgms = TRUE;
	hMedia->stOutputControl.bScms = TRUE;
	hMedia->stOutputControl.bMacrovision = TRUE;

	s_tEventCallback(hMedia->pIdentifier, PAL_MEDIA_EVT_DRM_OUTPUT_CONTROL, (HINT32)NULL);
}

static void __pal_media_drm_output_process_playready(SESSION_HANDLE id, OutputControl_t *outputcontrol)
{
	PAL_MEDIA_HANDLE hMedia = __pal_media_get_handle_by_sessionid((HUINT32)id);
	if ( hMedia == NULL ) return;

	hMedia->stOutputControl.bUsed = TRUE;

	if ( outputcontrol->data.playready.minOPL.wUncompressedDigitalVideo >= 101 )
		hMedia->stOutputControl.bHdcp = TRUE;

	if ( outputcontrol->data.playready.minOPL.wAnalogVideo >= 101 )
		hMedia->stOutputControl.bCgms = TRUE;

	if ( outputcontrol->data.playready.minOPL.wCompressedDigitalAudio >= 201
			&& outputcontrol->data.playready.minOPL.wCompressedDigitalAudio <= 300 )
	{
		hMedia->stOutputControl.bHdcp = TRUE;
		hMedia->stOutputControl.bScms = TRUE;
	}
}

static void __pal_media_drm_output_callback(SESSION_HANDLE id, OutputControl_t *outputcontrol)
{
	PAL_MEDIA_HANDLE hMedia = __pal_media_get_handle_by_sessionid((HUINT32)id);
	if ( hMedia == NULL ) return;

	/* current software supports only playReady DRM */
	switch ( outputcontrol->drm_type )
	{
	case DI_DRM_TYPE_PLAYREADY:
		__pal_media_drm_output_process_playready(id, outputcontrol);
		s_tEventCallback(hMedia->pIdentifier, PAL_MEDIA_EVT_DRM_OUTPUT_CONTROL, (HINT32)NULL);
		break;
	default:
		break;
	}
}

HERROR PAL_MEDIA_Init(PAL_MEDIA_CONFIG_t *stConfig)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = DI_ERR_OK;
	HINT32 i = 0;
	HUINT32 ulVideoCaller = 0;
	HUINT32 ulAudioCaller = 0;
	DI_MEDIA_CONFIG_t default_Config;

	const HUINT8 videoEvents[4] = {
		DI_VIDEO_NOTIFY_EVT_SEQUENCE_HEADER_INFO_CHANGED,
		DI_VIDEO_NOTIFY_EVT_FORMAT_CHANGED,
		DI_VIDEO_NOTIFY_EVT_RUNNING_STATE_CHANGED,
		DI_VIDEO_NOTIFY_EVT_AFD_CHANGED
	};

	const HUINT8 audioEvents[3] = {
		DI_AUDIO_RUNNING_STATE_CHANGED,
		DI_AUDIO_FORMAT_CHANGED,
		DI_AUDIO_MEMORY_STREAM_COMPLETED
	};

	FuncEnter;

	VK_MEM_Memset(&default_Config, 0x00, sizeof(DI_MEDIA_CONFIG_t));
	default_Config.bInit = stConfig->bInit;
	default_Config.bUseStorage = stConfig->bUseStorage;
	default_Config.nStreamBufferingSecond = stConfig->nStreamBufferingSecond;
	default_Config.nChunkBaseExponent = stConfig->nChunkBaseExponent;

	eRet = DI_MEDIA_Init(default_Config);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("DI_MEDIA_Init Failed.  \n");
		return ERR_FAIL;
	}

	for ( i=0; i<4; ++i )
	{
		eRet = DI_VIDEO_RegisterEventCallback(PAL_MEDIA_VIDEO_DECODER_MAIN, videoEvents[i],
				__pal_media_video_callback, &ulVideoCaller);
		if ( eRet != DI_ERR_OK )
		{
			HxLOG_Error("PAL_MEDIA: %s : DI_VIDEO_RegisterEventCallback() failed.\n", __FUNCTION__);
			return ERR_FAIL;
		}
	}

	for ( i=0; i<3; ++i )
	{
		eRet = DI_AUDIO_RegisterEventCallback(PAL_MEDIA_AUDIO_DECODER_MAIN, audioEvents[i],
				__pal_media_audio_callback, &ulAudioCaller);
		if ( eRet != DI_ERR_OK )
		{
			HxLOG_Error("PAL_MEDIA: %s : DI_AUDIO_RegisterEventCallback() failed.\n", __FUNCTION__);
			return ERR_FAIL;
		}
	}

	for ( i= 0 ; i< PAL_MEDIA_MAX_VIEW_NUM ; ++i )
	{
		s_media_infos[i] = NULL;
	}

	PAL_MEDIA_SUB_Init(__pal_media_subtitle_callback);

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_GetDefaultConfiguration(PAL_MEDIA_CONFIG_t *stConfig)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = DI_ERR_OK;
	DI_MEDIA_CONFIG_t default_Config;

	FuncEnter;

	VK_MEM_Memset(&default_Config, 0x00, sizeof(DI_MEDIA_CONFIG_t));

	eRet =	DI_MEDIA_GetDefaultConfiguration((DI_MEDIA_CONFIG_t *)&default_Config);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_GetDefaultConfiguration() failed.\n", __FUNCTION__);
		return ERR_FAIL;
	}

	stConfig->bInit					 = default_Config.bInit;
	stConfig->bUseStorage 			 = default_Config.bUseStorage;
	stConfig->nStreamBufferingSecond = default_Config.nStreamBufferingSecond;
	stConfig->nChunkBaseExponent 	 = default_Config.nChunkBaseExponent;

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_Uninit()
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = ERR_OK;
	HINT32 i = 0;

	FuncEnter;

	eRet = DI_MEDIA_Uninit();
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("DI_MEDIA_Uninit \n");
		return ERR_FAIL;
	}

	for ( i=0 ; i<PAL_MEDIA_MAX_VIEW_NUM ; i++ )
	{
		s_media_infos[i] = NULL;
	}

	if ( PAL_MEDIA_SUB_Release() != ERR_OK)
	{
		HxLOG_Error("PAL_MEDIA_SUB_Release\n");
		return ERR_FAIL;
	}

	FuncLeave;
#endif

	return ERR_OK;
}

PAL_MEDIA_HANDLE PAL_MEDIA_Create(void)
{
	HERROR eRet = ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = NULL;

	FuncEnter;

	pstMediaHdl = HLIB_STD_MemAlloc(sizeof(PAL_MEDIA_t));
	if ( pstMediaHdl == NULL )
	{
		HxLOG_Critical("\n\n");
		return NULL;
	}

	HxSTD_memset(pstMediaHdl, 0x00, sizeof(PAL_MEDIA_t));

	eRet = PAL_MEDIA_SUB_Create(pstMediaHdl);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("Subtitle Context Create Fail... \n");
	}

	pstMediaHdl->uErrorlReason = PAL_MEDIA_ERROR_UNKNOWN;

	FuncLeave;

	return pstMediaHdl;
}

HERROR PAL_MEDIA_Destroy(PAL_MEDIA_HANDLE phMedia)
{
	HERROR eRet = ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = NULL;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);

	pstMediaHdl = phMedia;

	if ( pstMediaHdl->pUrl != NULL )
	{
		HLIB_STD_MemFree(pstMediaHdl->pUrl);
		pstMediaHdl->pUrl = NULL;
	}

	eRet = PAL_MEDIA_SUB_Destroy(pstMediaHdl);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("Subtitle Context Destroy Fail... \n");
	}

	HLIB_STD_MemFree((PAL_MEDIA_t *)phMedia);

	FuncLeave;

	return ERR_OK;
}

HINT8 *__pal_media_is_dlna(const HINT8 *pUrl, DI_MEDIA_SessionSettings_t *pSet)
{
	HCHAR szBuf[1024] = {0,};
	HCHAR szPar[1024] = {0,};
	HCHAR *pIter = NULL, *pToken = NULL, *pRet;
	HINT32 i, pos = 0;
	HCHAR *class[4]={"_Rate=", "_Dura=", "_Size=", "_Prot="};

	if( strlen(pUrl) > strlen(szPar) ){
		HxLOG_Error("[%s:%d] url length error. \n", __FUNCTION__, __LINE__);
		return NULL;
	}

	strncpy(szPar, pUrl, strlen(pUrl));
	pIter = (HCHAR *)pUrl;


	for(i=0; i<4; i++)
	{
		memset(szBuf, 0x00, 1024);
		memset(szPar, 0x00, 1024);
		strncpy(szPar, pUrl, strlen(pIter)-pos);

		pToken = strstr(szPar, class[i]);
		if(pToken == NULL)
			return (HINT8 *)pUrl;

		strncpy(szBuf, pToken+6, strlen(pToken)-6);
		pos += strlen(pToken);

		switch(i)
		{
			case 0:
				pSet->tDlnaSetting.nByteRate = atoi(szBuf);
				break;
			case 1:
				pSet->tDlnaSetting.unDuration = atoi(szBuf);
				break;
			case 2:
				pSet->tDlnaSetting.nFileSize = atoll(szBuf);
				break;
			case 3:
				pSet->pProtocoalInfo = HLIB_STD_StrDup(szBuf);
				pSet->bSuccesiveSeekForTrick = TRUE;
				break;
			default :
				return NULL;
		}
	}

	memset(szBuf, 0x00, 1024);
	strncpy(szBuf, pUrl, strlen(pUrl)-pos);

	HxLOG_Print("[%s:%d] This is DLNA Media. \n", __FUNCTION__, __LINE__);
	pRet = HLIB_STD_StrDup(szBuf);
	return pRet;
}

HERROR PAL_MEDIA_CreateSession(PAL_MEDIA_HANDLE phMedia, const HINT8 *pURL)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = DI_ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;
	DI_MEDIA_SessionSettings_t tSettings, tDlnaSetting;
   	HINT8 *url= (HINT8 *)pURL;
	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);
	PAL_MEDIA_ASSERT(pURL);

	HxSTD_memset(&tSettings, 0x00, sizeof(tSettings));
	HxSTD_memset(&tDlnaSetting, 0x00, sizeof(tDlnaSetting));
	HxSTD_memset(&(pstMediaHdl->stOutputControl), 0, sizeof(pstMediaHdl->stOutputControl));

	url = __pal_media_is_dlna(pURL, &tDlnaSetting);
	if ( url == NULL )
	{
		HxLOG_Error("Error> url is null. \n");
		goto error;
	}

	eRet = DI_MEDIA_CreateSession((HINT8 *)url, &(pstMediaHdl->pIdentifier));
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("DI_MEDIA_CreateSession \n");
		goto error;
	}

	DI_MEDIA_GetSessionSettings(pstMediaHdl->pIdentifier, &tSettings);

	eRet = DI_MEDIA_RegisterErrorEventCB(pstMediaHdl->pIdentifier, __pal_media_error_event_callback);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_RegisterErrorEventCB() failed.\n", __FUNCTION__);
		goto error;
	}

	eRet = DI_MEDIA_RegisterLicenseDataCallback(pstMediaHdl->pIdentifier, __pal_media_drm_license_callback);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_RegisterErrorEventCB() failed.\n", __FUNCTION__);
		goto error;
	}

	eRet = DI_DRM_RegisterOutputControlCallback(pstMediaHdl->pIdentifier, __pal_media_drm_output_callback);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_RegisterErrorEventCB() failed.\n", __FUNCTION__);
		goto error;
	}

	if ( pstMediaHdl->pUrl ) HLIB_STD_MemFree(pstMediaHdl->pUrl);
	pstMediaHdl->pUrl = HLIB_STD_MemAlloc(strlen(url)+1);
	if ( pstMediaHdl->pUrl == NULL )
	{
		HxLOG_Critical("\n\n");
		goto error;
	}

	HxSTD_memset(pstMediaHdl->pUrl, 0x00, strlen(url)+1);
	HxSTD_memcpy(pstMediaHdl->pUrl, url, strlen(url)+1);

	/* Dlna Media */
	if(tDlnaSetting.pProtocoalInfo != NULL)
	{
		tSettings.tDlnaSetting.nByteRate = tDlnaSetting.tDlnaSetting.nByteRate;
		tSettings.tDlnaSetting.unDuration = tDlnaSetting.tDlnaSetting.unDuration;
		tSettings.tDlnaSetting.nFileSize = tDlnaSetting.tDlnaSetting.nFileSize;
		tSettings.pProtocoalInfo = tDlnaSetting.pProtocoalInfo;
		tSettings.bSuccesiveSeekForTrick = TRUE;
		pstMediaHdl->bDlna = TRUE;
	}
	else
	{
		pstMediaHdl->bDlna = FALSE;
	}

	/* This codes are for improvment of Video starting time */
	tSettings.eBufferingMode = DI_MEDIA_MODE_UNLIMITED;
	if ( pstMediaHdl->stBufferingMode.bAuto )
	{
		tSettings.eUnderflowMode = DI_MEDIA_UNDERFLOW_AUTO;
		HxLOG_Debug("PAL_MEDIA: %s : BufferingMode:MEDIA_UNDERFLOW_AUTO\n", __FUNCTION__);
	}
	else
	{
		tSettings.eUnderflowMode = DI_MEDIA_UNDERFLOW_THRESHOLD;
		tSettings.tPlaybackStartParams.ulStartConstant= 0;
		tSettings.tPlaybackStartParams.ulStartThreshold = pstMediaHdl->stBufferingMode.nBufferThreshold;
		HxLOG_Debug("PAL_MEDIA: %s : BufferingMode:MEDIA_UNDERFLOW_THRESHOLD(%d)\n",
			__FUNCTION__, pstMediaHdl->stBufferingMode.nBufferThreshold);
	}

	if(pstMediaHdl->cbVerifyExternal.verifycallback != NULL)
	{
		tSettings.tTlsSetting.cbTlsCertficate.cbVerify = pstMediaHdl->cbVerifyExternal.verifycallback;
		tSettings.tTlsSetting.cbTlsCertficate.pUserdata = pstMediaHdl->cbVerifyExternal.userdata;
	}

#if 0
#if defined(CONFIG_MW_MM_DLNA_DMP_SI)
	tSettings.nDemuxId = pstMediaHdl->nDemuxId;
#endif
#endif

	tSettings.tPRDYSetting.pCertificatePath = DxMEDIA_CERT_PATH;
	tSettings.tPRDYSetting.pPrivateKeyPath = DxMEDIA_KEY_PATH;

	(void)DI_MEDIA_SetSessionSettings(pstMediaHdl->pIdentifier, &tSettings);

#if 1	/* This is really really workaround code by octo's view limitation */
	__pmedia_del_all();
#endif
	__pmedia_add_info(pstMediaHdl, pstMediaHdl->pIdentifier);

	if((url != NULL) && (url != pURL))
	{
		HLIB_STD_MemFree(url);
	}

	FuncLeave;

	return ERR_OK;


error:

	if((url != NULL) && (url != pURL)) 	HLIB_STD_MemFree(url);
	if(tSettings.pProtocoalInfo) HLIB_STD_MemFree(tSettings.pProtocoalInfo);

	FuncLeave;

	return ERR_FAIL;

#else
	return ERR_OK;
#endif
}

HERROR PAL_MEDIA_DestroySession(PAL_MEDIA_HANDLE phMedia)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = DI_ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);

	__pal_media_audio_stop(phMedia);
	__pal_media_video_stop(phMedia);
	__pal_media_revert_audio_type();

	eRet = DI_DRM_UnregisterOutputControlCallback(pstMediaHdl->pIdentifier);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_DRM_UnRegisterOutputControlCallback() failed.\n", __FUNCTION__);
		return ERR_FAIL;
	}

	eRet = DI_MEDIA_UnregisterLicenseDataCallback(pstMediaHdl->pIdentifier);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_UnregisterLicenseDataCallback() failed.\n", __FUNCTION__);
		return ERR_FAIL;
	}

	eRet = DI_MEDIA_UnregisterErrorEventCB(pstMediaHdl->pIdentifier);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_UnregisterErrorEventCB() failed.\n", __FUNCTION__);
		return ERR_FAIL;
	}

	eRet = DI_MEDIA_DestroySession(pstMediaHdl->pIdentifier);
	if (eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_DestroySession() failed.\n", __FUNCTION__);
		return ERR_FAIL;
	}

	__pmedia_del_info(pstMediaHdl);

	HxSTD_memset(pstMediaHdl->szDrmLicenseData, 0, sizeof(pstMediaHdl->szDrmLicenseData));

	pstMediaHdl->pIdentifier = NULL;

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_OpenSession(PAL_MEDIA_HANDLE phMedia)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = DI_ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);

	eRet = DI_MEDIA_OpenSession(pstMediaHdl->pIdentifier);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_OpenSession() failed.\n", __FUNCTION__);
		return ERR_FAIL;
	}

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_CloseSession(PAL_MEDIA_HANDLE phMedia)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = DI_ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);

	eRet = DI_MEDIA_CloseSession(pstMediaHdl->pIdentifier);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_CloseSession() failed.\n", __FUNCTION__);
		return ERR_FAIL;
	}

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_ProbeSession(PAL_MEDIA_HANDLE phMedia)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = DI_ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);

	s_tProbeCallback.cbProbeComplete = __pal_media_probe_complete_callback;
	s_tProbeCallback.cbProbeFailure = __pal_media_probe_failure_callback;

	eRet = DI_MEDIA_RegisterProbeEventCallback(pstMediaHdl->pIdentifier, &s_tProbeCallback);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_RegisterProbeEventCallback() failed.\n", __FUNCTION__);
		return ERR_FAIL;
	}

	eRet = DI_MEDIA_ProbeSession(pstMediaHdl->pIdentifier);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_ProbeSession() failed.\n", __FUNCTION__);
		return ERR_FAIL;
	}

	DI_MEDIA_UnregisterProbeEventCallback(pstMediaHdl->pIdentifier);

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_ProbeCancel(PAL_MEDIA_HANDLE phMedia)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = DI_ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);

	eRet = DI_MEDIA_ProbeCancel(pstMediaHdl->pIdentifier);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_ProbeCancel() failed.\n", __FUNCTION__);
		return ERR_FAIL;
	}

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_SetupSession(PAL_MEDIA_HANDLE phMedia)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = DI_ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;
	pal_media_info_t *info = NULL;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);

	eRet = DI_MEDIA_SetupSession(pstMediaHdl->pIdentifier, &(pstMediaHdl->ulMediaId));
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_SetupSession() failed.\n", __FUNCTION__);
		return ERR_FAIL;
	}

	if ( pstMediaHdl->stOutputControl.bUsed == FALSE
			&& pstMediaHdl->tMediaInfo.is_drm == TRUE )
	{
		__pal_media_drm_output_default_process(pstMediaHdl->pIdentifier);
	}

	eRet = DI_MEDIA_RegisterMediaEventCallback(pstMediaHdl->pIdentifier, pstMediaHdl->ulMediaId,
			__pal_media_event_callback);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_RegisterMediaEventCallback() failed.\n", __FUNCTION__);
		return ERR_FAIL;
	}

	info = __pmedia_find_info(pstMediaHdl);
	if ( info )
	{
		info->mediaId = pstMediaHdl->ulMediaId;
	}

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_ReleaseSession(PAL_MEDIA_HANDLE phMedia)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = DI_ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);

	eRet = DI_MEDIA_UnregisterMediaEventCallback(pstMediaHdl->pIdentifier, pstMediaHdl->ulMediaId);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_UnregisterMediaEventCallback() failed.\n", __FUNCTION__);
		return ERR_FAIL;
	}

	eRet = DI_MEDIA_ReleaseSession(pstMediaHdl->pIdentifier, pstMediaHdl->ulMediaId);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_ReleaseSession() failed.\n", __FUNCTION__);
		return ERR_FAIL;
	}

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_StartSession(PAL_MEDIA_HANDLE phMedia, HUINT32 ulStartTime)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = DI_ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);

	pstMediaHdl->eAudioDecoderType = PAL_MEDIA_AUDIO_DECODER_MAIN;
	pstMediaHdl->eVideoDecoderType = PAL_MEDIA_VIDEO_DECODER_MAIN;

	__pal_media_video_stop(phMedia);
	__pal_media_audio_stop(phMedia);
	__pal_media_set_audio_type();

	if ( pstMediaHdl->tMediaInfo.audio_stream_id == 0
			&& pstMediaHdl->tMediaInfo.audio_codec     == 0
			&& pstMediaHdl->tMediaInfo.video_stream_id == 0
			&& pstMediaHdl->tMediaInfo.video_codec     == 0 )
	{
		HxLOG_Print("Invalid Audio and Video PID(audio:%x:%x / video:%x:%x\n",
				pstMediaHdl->tMediaInfo.audio_stream_id , pstMediaHdl->tMediaInfo.audio_codec,
				pstMediaHdl->tMediaInfo.video_stream_id, pstMediaHdl->tMediaInfo.video_codec);
		return ERR_FAIL;
	}

	if ( pstMediaHdl->tMediaInfo.video_stream_id != 0 && pstMediaHdl->tMediaInfo.video_codec != 0 )
	{
		__pal_media_video_start(phMedia);
	}

	if ( pstMediaHdl->tMediaInfo.audio_stream_id != 0 && pstMediaHdl->tMediaInfo.audio_codec != 0 )
	{
		__pal_media_audio_start(phMedia);
	}

	eRet = DI_MEDIA_StartSession(pstMediaHdl->pIdentifier, pstMediaHdl->ulMediaId, (HULONG)ulStartTime);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_StartSession() failed.\n", __FUNCTION__);
		return  ERR_FAIL;
	}

	pstMediaHdl->uErrorlReason = PAL_MEDIA_ERROR_UNKNOWN;

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_StopSession(PAL_MEDIA_HANDLE phMedia)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);

	if ( pstMediaHdl->tMediaInfo.audio_stream_id != 0 && pstMediaHdl->tMediaInfo.audio_codec != 0 )
	{
		__pal_media_audio_stop(phMedia);
		__pal_media_revert_audio_type();
	}

	if ( pstMediaHdl->tMediaInfo.video_stream_id != 0 && pstMediaHdl->tMediaInfo.video_codec != 0 )
	{
		__pal_media_video_stop(phMedia);
	}

	eRet = DI_MEDIA_StopSession(pstMediaHdl->pIdentifier, pstMediaHdl->ulMediaId);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_StopSession() failed.\n", __FUNCTION__);
		return  ERR_FAIL;
	}

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_PauseSession(PAL_MEDIA_HANDLE phMedia)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);

	eRet = DI_MEDIA_PauseSession(pstMediaHdl->pIdentifier, pstMediaHdl->ulMediaId);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_PauseSession() failed.\n", __FUNCTION__);
		return  ERR_FAIL;
	}

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_ResumeSession(PAL_MEDIA_HANDLE phMedia)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);

	eRet = DI_MEDIA_ResumeSession(pstMediaHdl->pIdentifier, pstMediaHdl->ulMediaId);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_ResumeSession() failed.\n", __FUNCTION__);
		return  ERR_FAIL;
	}

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_StartBuffering(PAL_MEDIA_HANDLE phMedia, HINT8 *pSaveFileName)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);

	s_tBufferingCallback.cbStart = __pal_media_buffering_start_callback;
	s_tBufferingCallback.cbCanPlay = __pal_media_buffering_canplay_callback;
	s_tBufferingCallback.cbPause = __pal_media_buffering_underflow_callback;
	s_tBufferingCallback.cbResume = __pal_media_buffering_enough_callback;
	s_tBufferingCallback.cbComplete = __pal_media_buffering_complete_callback;
	s_tBufferingCallback.cbFailure = __pal_media_buffering_failure_callback;
	s_tBufferingCallback.cbAdaptiveChange = __pal_media_buffering_adaptive_callback;

	eRet = DI_MEDIA_RegisterBufferingEventCallback(pstMediaHdl->pIdentifier, &s_tBufferingCallback);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("DI_MEDIA_RegisterBufferingEventCallback Error,pucStreamPath=%s pIdentifier=%p\n",
				pSaveFileName, pstMediaHdl->pIdentifier);
		return ERR_FAIL;
	}

	eRet = DI_MEDIA_StartBuffering(pstMediaHdl->pIdentifier, pSaveFileName);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_StartBuffering() failed.\n", __FUNCTION__);
		return ERR_FAIL;
	}

	if ( pstMediaHdl->pSaveFileName ) HLIB_STD_MemFree( pstMediaHdl->pSaveFileName);
	pstMediaHdl->pSaveFileName = HLIB_STD_MemAlloc(strlen(pSaveFileName)+1);
	if ( pstMediaHdl->pSaveFileName == NULL )
	{
		HxLOG_Error("PAL_MEDIA: %s : SaveFileName is NULL. \n", __FUNCTION__);
		return ERR_FAIL;
	}
	HxSTD_memset(pstMediaHdl->pSaveFileName, 0x00, strlen(pSaveFileName)+1);
	HxSTD_memcpy(pstMediaHdl->pSaveFileName, pSaveFileName, strlen(pSaveFileName)+1);

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_PauseBuffering(PAL_MEDIA_HANDLE phMedia)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);

	eRet = DI_MEDIA_PauseBuffering(pstMediaHdl->pIdentifier);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_PauseBuffering() failed.\n", __FUNCTION__);
		return ERR_FAIL;
	}

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_ResumeBuffering(PAL_MEDIA_HANDLE phMedia)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);

	eRet = DI_MEDIA_ResumeBuffering(pstMediaHdl->pIdentifier);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_ResumeBuffering() failed.\n", __FUNCTION__);
		return ERR_FAIL;
	}

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_StopBuffering(PAL_MEDIA_HANDLE phMedia)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = DI_ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);

	eRet = DI_MEDIA_StopBuffering(pstMediaHdl->pIdentifier);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_StopBuffering() failed.\n", __FUNCTION__);
		return ERR_FAIL;
	}

	eRet = DI_MEDIA_UnregisterBufferingEventCallback(pstMediaHdl->pIdentifier);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_UnregisterBufferingEventCallback() failed.\n", __FUNCTION__);
		return ERR_FAIL;
	}

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_GetProbeInfo(PAL_MEDIA_HANDLE phMedia, PAL_MEDIA_INFO_t *pstSetup)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = DI_ERR_OK;
	HUINT32	nCount = 0;
	DI_MEDIA_INFO_t *pstMediaInfo = NULL;
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);
	PAL_MEDIA_ASSERT(pstSetup);

	pstMediaInfo = OxMW_Malloc(sizeof(DI_MEDIA_INFO_t));
	if ( pstMediaInfo == NULL )
	{
		HxLOG_Critical("[%s] Fail Mem Alloc\n", __FUNCTION__);
		return ERR_FAIL;
	}
	HxSTD_memset(pstMediaInfo, 0x0, sizeof(DI_MEDIA_INFO_t));

	eRet = DI_MEDIA_GetProbeInfo(pstMediaHdl->pIdentifier, pstMediaInfo);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_GetProbeInfo() failed.\n", __FUNCTION__);
		OxMW_Free(pstMediaInfo);
		return ERR_FAIL;
	}

	pstSetup->demuxId = (HUINT32)pstMediaInfo->demuxId;
	pstSetup->duration = pstMediaInfo->duration;

	pstSetup->audio_codec = __pal_media_audio_codec_to_ap(pstMediaInfo->audio_codec);
	pstSetup->audio_pes_id = pstMediaInfo->audio_pes_id;
	pstSetup->audio_stream_id = pstMediaInfo->audio_stream_id;

	if(pstMediaInfo->audio_codec == 0 && pstMediaInfo->stAudioTrack[0].audio_codec != 0)
	{
		pstSetup->audio_codec = __pal_media_audio_codec_to_ap(pstMediaInfo->stAudioTrack[0].audio_codec);
		pstSetup->audio_pes_id = pstMediaInfo->stAudioTrack[0].audio_pes_id;
		pstSetup->audio_stream_id = pstMediaInfo->stAudioTrack[0].audio_stream_id;
	}

	pstSetup->video_codec = __pal_media_video_codec_to_ap(pstMediaInfo->video_codec);
	pstSetup->video_pes_id = pstMediaInfo->video_pes_id;
	pstSetup->video_stream_id = pstMediaInfo->video_stream_id;

	if(pstMediaInfo->video_codec == 0 && pstMediaInfo->stVideoTrack[0].video_codec != 0)
	{
		pstSetup->video_codec = __pal_media_video_codec_to_ap(pstMediaInfo->stVideoTrack[0].video_codec);
		pstSetup->video_pes_id = pstMediaInfo->stVideoTrack[0].video_pes_id;
		pstSetup->video_stream_id = pstMediaInfo->stVideoTrack[0].video_stream_id;
	}

	if ( pstMediaInfo->eDrmType != DI_MEDIA_DRM_TYPE_NONE )
		pstSetup->is_drm = 1;
	pstSetup->is_expired = pstMediaInfo->is_expired;

	pstSetup->max_bitrate = pstMediaInfo->max_bitrate;
	pstSetup->nprograms = pstMediaInfo->nprograms;
	pstSetup->ntracks = pstMediaInfo->ntracks;
	pstSetup->pcr_pid = pstMediaInfo->pcr_pid;

	if(pstMediaInfo->ucTotalAudioTrack > PAL_MEDIA_MAX_AUDIO_TRACK)
		pstMediaInfo->ucTotalAudioTrack = PAL_MEDIA_MAX_AUDIO_TRACK;

	for(nCount = 0; nCount<pstMediaInfo->ucTotalAudioTrack; nCount++)
	{
		pstSetup->stAudioTrack[nCount].audio_pes_id = pstMediaInfo->stAudioTrack[nCount].audio_pes_id;
		pstSetup->stAudioTrack[nCount].audio_stream_id = pstMediaInfo->stAudioTrack[nCount].audio_stream_id;
		pstSetup->stAudioTrack[nCount].audio_codec = __pal_media_audio_codec_to_ap(pstMediaInfo->stAudioTrack[nCount].audio_codec);
		HxSTD_memcpy(pstSetup->stAudioTrack[nCount].uacAudioLang, pstMediaInfo->stAudioTrack[nCount].uacAudioLang, (sizeof(HUINT8)*256));
	}

	if(pstMediaInfo->ucTotalXsubTrack > PAL_MEDIA_MAX_VIDEO_TRACK)
		pstMediaInfo->ucTotalXsubTrack = PAL_MEDIA_MAX_VIDEO_TRACK;

	for(nCount = 0; nCount<pstMediaInfo->ucTotalXsubTrack; nCount++)
	{
		pstSetup->stVideoTrack[nCount].video_pes_id = pstMediaInfo->stVideoTrack[nCount].video_pes_id;
		pstSetup->stVideoTrack[nCount].video_stream_id = pstMediaInfo->stVideoTrack[nCount].video_stream_id;
		pstSetup->stVideoTrack[nCount].video_codec = __pal_media_video_codec_to_ap(pstMediaInfo->stVideoTrack[nCount].video_codec);
	}

	if( pstMediaInfo->ucTotalXsubTrack > PAL_MEDIA_MAX_XSUB_TRACK)
		pstMediaInfo->ucTotalXsubTrack = PAL_MEDIA_MAX_XSUB_TRACK;

	for(nCount = 0;nCount < pstMediaInfo->ucTotalXsubTrack ;nCount++)
	{
		pstSetup->stXsubTrack[nCount].eXsubType = pstMediaInfo->stXsubTrack[nCount].eXsubType;
		HxSTD_memcpy(pstSetup->stXsubTrack[nCount].uacXsubLang, pstMediaInfo->stXsubTrack[nCount].uacXsubLang, (sizeof(HUINT8)*256));
	}

	pstSetup->stream_type = pstMediaInfo->stream_type;
	pstSetup->support_seek = pstMediaInfo->support_seek;
	pstSetup->support_trick = pstMediaInfo->support_trick;
	pstSetup->ucTotalAudioTrack = pstMediaInfo->ucTotalAudioTrack;
	pstSetup->ucTotalVideoTrack = pstMediaInfo->ucTotalVideoTrack;
	pstSetup->ucTotalXsubTrack = pstMediaInfo->ucTotalXsubTrack;

	pstSetup->eStartCondition = pstMediaInfo->eStartCondition;
	pstSetup->ulStartTime = pstMediaInfo->ulStartTime;

	pstSetup->usVideoHeight = pstMediaInfo->usVideoHeight;
	pstSetup->usVideoWidth = pstMediaInfo->usVideoWidth;

	pstSetup->isSuccesiveSeekMode = pstMediaInfo->bSuccesiveSeekMode;

	HxLOG_Info("Audio[codec(%d) pes_id(%d) stream_id(%d)] Video[codec(%d), pes_id(%d) stream_id(%d)]  duration=%d\n",
			pstMediaInfo->audio_codec, pstMediaInfo->audio_pes_id, pstMediaInfo->audio_stream_id,
			pstMediaInfo->video_codec, pstMediaInfo->video_pes_id, pstMediaInfo->video_stream_id, pstMediaInfo->duration);

	if(pstMediaInfo->ucTotalAudioTrack > 0)
	{
		for( nCount=0; nCount<pstMediaInfo->ucTotalAudioTrack ; nCount++)
		{
			HxLOG_Print("## MultiAudio Info  [%d] ped_id(%d) audio_stream_id(%d) audio_codec(%d) \n", nCount,
					pstSetup->stAudioTrack[nCount].audio_pes_id,
					pstSetup->stAudioTrack[nCount].audio_stream_id,
					pstSetup->stAudioTrack[nCount].audio_codec);

		}
	}

	pstSetup->isIcyProtocol = pstMediaInfo->isIcyProtocol;

	HxSTD_memcpy(&(pstMediaHdl->tMediaInfo), pstSetup, (sizeof(PAL_MEDIA_INFO_t)));
	OxMW_Free(pstMediaInfo);

	pstMediaHdl->bProbe = TRUE;

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_SetPosition(PAL_MEDIA_HANDLE phMedia, HUINT64 ullPosition)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);

	eRet = DI_MEDIA_SetPosition (pstMediaHdl->pIdentifier, pstMediaHdl->ulMediaId, ullPosition);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_SetPosition(%d) failed.\n", __FUNCTION__, ullPosition);
		return ERR_FAIL;
	}

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_GetPosition(PAL_MEDIA_HANDLE phMedia, HUINT64 *pullPosition)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	//FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);
	PAL_MEDIA_ASSERT(pullPosition);

	eRet = DI_MEDIA_GetPosition(pstMediaHdl->pIdentifier, pstMediaHdl->ulMediaId, pullPosition);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_GetPosition() failed.\n", __FUNCTION__);
		return ERR_FAIL;
	}

	//FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_GetDuration(PAL_MEDIA_HANDLE phMedia, HUINT64 *pullDuration)
{
#ifdef CONFIG_SUPPORT_IMMA
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	//FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);
	PAL_MEDIA_ASSERT(pullDuration);

	*pullDuration = pstMediaHdl->tMediaInfo.duration;

	//FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_SetSpeed(PAL_MEDIA_HANDLE phMedia, HINT32 nSpeed )
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;
	DI_MEDIA_PlaySpeed_e eSpeed;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);

	eSpeed = __pal_media_speed_to_di(nSpeed);
	if ( eSpeed == eDI_MEDIA_Speed_None ) return ERR_FAIL;

	if(pstMediaHdl->bDlna == TRUE)
	{
		if(nSpeed != 100)
		{
			if ( nSpeed != 0 )
			{
				if((pstMediaHdl->tMediaInfo.stream_type == eMEDIA_Type_MPEG_TS)
					|| (pstMediaHdl->tMediaInfo.stream_type == eMEDIA_Type_MPEG_TS_192))
				{
					__pal_media_audio_stop(phMedia);
				}
				else if((eSpeed < eDI_MEDIA_Speed_Pause) || (eSpeed > eDI_MEDIA_Speed_FF_X4))
				{
					__pal_media_audio_stop(phMedia);
				}
			}
		}
		else
		{
			__pal_media_audio_start(phMedia);
		}
	}

	eRet = DI_MEDIA_SetSpeed(pstMediaHdl->pIdentifier, pstMediaHdl->ulMediaId, eSpeed);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_SetSpeed() failed.\n", __FUNCTION__);
		return  ERR_FAIL;
	}

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_GetSpeed(PAL_MEDIA_HANDLE phMedia, HINT32 *pnSpeed)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;
	DI_MEDIA_PlaySpeed_e eSpeed;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);
	PAL_MEDIA_ASSERT(pnSpeed);

	eRet = DI_MEDIA_GetSpeed(pstMediaHdl->pIdentifier, pstMediaHdl->ulMediaId, &eSpeed);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_GetSpeed() failed.\n", __FUNCTION__);
		return  ERR_FAIL;
	}
	*pnSpeed = __pal_media_speed_to_ap(eSpeed);

	FuncLeave;
#endif

	return ERR_OK;
}

#define MAX_SPEED_NUM		15
HERROR PAL_MEDIA_GetSupportedSpeeds(PAL_MEDIA_HANDLE phMedia, HUINT32 *pulSpeedNum, HINT32 **ppnSpeedArray)
{
#ifdef CONFIG_SUPPORT_IMMA
	static int s_supported_speeds[MAX_SPEED_NUM] = {-6400, -3200, -1600, -800, -400, -200, -100,
													0, 100, 200, 400, 800, 1600, 3200, 6400};

	PAL_MEDIA_ASSERT(phMedia);
	PAL_MEDIA_ASSERT(pulSpeedNum);

	*pulSpeedNum = MAX_SPEED_NUM;
	*ppnSpeedArray = s_supported_speeds;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_GetBufferingStatus(PAL_MEDIA_HANDLE phMedia, PAL_MEDIA_BufferStatus_t *pstBufferStatus)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;
	DI_MEDIA_BufferStatus_t buffer_status;

	//FuncEnter;

	PAL_MEDIA_ASSERT(pstBufferStatus);

	eRet = DI_MEDIA_GetBufferingStatus(pstMediaHdl->pIdentifier, &buffer_status);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : PAL_MEDIA_GetBufferingStatus() failed.\n", __FUNCTION__);
		return  ERR_FAIL;
	}

	pstBufferStatus->nBufferedBytes = buffer_status.llBufferedBytes;
	pstBufferStatus->nTotalBytesRemaining = buffer_status.llTotalBytesRemaining;
	pstBufferStatus->nTotalStreamBytes = buffer_status.llTotalStreamBytes;
	pstBufferStatus->nArrivalBytedPerSecond = buffer_status.nArrivalBytedPerSecond;
	pstBufferStatus->nStreamBytesPerSecond = buffer_status.nStreamBytesPerSecond;
	pstBufferStatus->nBufferedMilliseconds = buffer_status.nBufferedMilliseconds;
	pstBufferStatus->uiBufferPercentage = buffer_status.uiBufferPercentage;
	pstBufferStatus->uiRemainBufferSec =buffer_status.uiRemainBufferSec;
	//FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_GetBufferingChunkInfo(PAL_MEDIA_HANDLE phMedia, HUINT32 *pulChunks, HUINT32 **ulStartOffsets, HUINT32 **ulEndOffsets)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	PAL_MEDIA_ASSERT(phMedia);
	PAL_MEDIA_ASSERT(pulChunks);

	eRet = DI_MEDIA_GetBufferInfo(pstMediaHdl->pIdentifier, pulChunks,
					(HINT32 **)ulStartOffsets, (HINT32 **)ulEndOffsets);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_GetBufferInfo() failed.\n", __FUNCTION__);
		return  ERR_FAIL;
	}
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_SetBufferingMode(PAL_MEDIA_HANDLE phMedia, PAL_MEDIA_BufferingModeParams_t *pstParam)
{
#ifdef CONFIG_SUPPORT_IMMA
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	FuncEnter;

	PAL_MEDIA_ASSERT(phMedia);

	pstMediaHdl->stBufferingMode.bAuto = pstParam->bAuto;
	pstMediaHdl->stBufferingMode.nBufferThreshold= pstParam->nBufferThreshold;

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_RegisterMediaEventCallback(PAL_MEDIA_EventCallback_t MediaEventCallback)
{
#ifdef CONFIG_SUPPORT_IMMA
	FuncEnter;

	PAL_MEDIA_ASSERT(MediaEventCallback);
	s_tEventCallback = MediaEventCallback;

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_UnregisterMediaEventCallback(void)
{
#ifdef CONFIG_SUPPORT_IMMA
	FuncEnter;

	if ( s_tEventCallback ) s_tEventCallback = NULL;

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_SetUserAgentString(PAL_MEDIA_HANDLE phMedia, HINT8 *pUAString, HINT32 nSize)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	PAL_MEDIA_ASSERT(phMedia);
	PAL_MEDIA_ASSERT(pUAString);

	FuncEnter;

	eRet = DI_MEDIA_SetUserAgentString(pstMediaHdl->pIdentifier, pUAString, nSize);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_SetUserAgentString() failed.\n", __FUNCTION__);
		return  ERR_FAIL;
	}

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_SetDrmLicenseData(PAL_MEDIA_HANDLE phMedia, HINT8 *pString, HINT32 nSize)
{
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	PAL_MEDIA_ASSERT(phMedia);
	PAL_MEDIA_ASSERT(pString);

	FuncEnter;

	HxSTD_memset(pstMediaHdl->szDrmLicenseData, 0, sizeof(pstMediaHdl->szDrmLicenseData));
	VK_strncpy(pstMediaHdl->szDrmLicenseData, pString, strlen(pString)+1);

	FuncLeave;

	return ERR_OK;
}

HERROR PAL_MEDIA_GetOutputControlInfo(PAL_MEDIA_HANDLE phMedia, PAL_MEDIA_OutputControl_t *pstOutputControl)
{
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	PAL_MEDIA_ASSERT(phMedia);
	PAL_MEDIA_ASSERT(pstOutputControl);

	pstOutputControl->bHdcp = pstMediaHdl->stOutputControl.bHdcp;
	pstOutputControl->bScms = pstMediaHdl->stOutputControl.bScms;
	pstOutputControl->bCgms = pstMediaHdl->stOutputControl.bCgms;
	pstOutputControl->bMacrovision = pstMediaHdl->stOutputControl.bMacrovision;

	return ERR_OK;
}


HERROR PAL_MEDIA_SetAudioTrack(PAL_MEDIA_HANDLE phMedia, HUINT8 ucAudioTrack)
{
#ifdef CONFIG_SUPPORT_IMMA
	DI_ERR_CODE eRet = ERR_OK;
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	PAL_MEDIA_ASSERT(phMedia);

	FuncEnter;

	eRet = DI_MEDIA_SetAudioTrack(pstMediaHdl->pIdentifier, pstMediaHdl->ulMediaId, ucAudioTrack);
	if ( eRet != DI_ERR_OK )
	{
		HxLOG_Error("PAL_MEDIA: %s : DI_MEDIA_SetAudioTrack() failed.\n", __FUNCTION__);
		return  ERR_FAIL;
	}

	__pal_media_audio_stop(phMedia);

	if ( pstMediaHdl->tMediaInfo.stAudioTrack[ucAudioTrack].audio_stream_id != 0
			&& pstMediaHdl->tMediaInfo.stAudioTrack[ucAudioTrack].audio_codec != 0 )
	{
		if ( __pal_media_audio_start(phMedia) != ERR_OK )
		{
			return ERR_FAIL;
		}
	}

	FuncLeave;
#endif

	return ERR_OK;
}

HERROR PAL_MEDIA_SetTLS(PAL_MEDIA_HANDLE phMedia, PAL_MEDIA_VerifyExternalCallback_t *VerifyExternalCb)
{
	PAL_MEDIA_t *pstMediaHdl = phMedia;

	PAL_MEDIA_ASSERT(phMedia);

	if ( VerifyExternalCb == NULL || VerifyExternalCb->verifycallback == NULL )
		return ERR_FAIL;

	pstMediaHdl->cbVerifyExternal.verifycallback = VerifyExternalCb->verifycallback;
	pstMediaHdl->cbVerifyExternal.userdata = VerifyExternalCb->userdata;

	return ERR_OK;
}

HUINT32 PAL_MEDIA_GetLastErrorReason(PAL_MEDIA_HANDLE phMedia)
{
	PAL_MEDIA_ASSERT(phMedia);

	return phMedia->uErrorlReason;
}
#if 0
HERROR PAL_MEDIA_UpdateMultiTrack(PAL_MEDIA_HANDLE phMedia, PAL_MEDIA_MultiTrack_t *pTracks)
{
	PAL_MEDIA_t 	*pstMediaHdl = phMedia;
	DI_MEDIA_INFO_t	update_info;
	DI_ERR_CODE 	eRet = DI_ERR_OK;
	int i;

	PAL_MEDIA_ASSERT(phMedia);
	PAL_MEDIA_ASSERT(pTracks);

	memset(&update_info, 0x00, sizeof(DI_MEDIA_INFO_t));

	if(pTracks->track_count > PAL_MEDIA_MAX_AUDIO_TRACK)
		return ERR_FAIL;

	/* current playing track info */
	update_info.video_pes_id = pstMediaHdl->tMediaInfo.video_pes_id;
	update_info.video_stream_id = pstMediaHdl->tMediaInfo.video_stream_id;
	update_info.video_codec = __pal_media_video_codec_to_di(pstMediaHdl->tMediaInfo.video_codec);

	update_info.audio_pes_id = pstMediaHdl->tMediaInfo.audio_pes_id;
	update_info.audio_stream_id = pstMediaHdl->tMediaInfo.audio_stream_id;
	update_info.audio_codec = __pal_media_audio_codec_to_di(pstMediaHdl->tMediaInfo.audio_codec);

	/* update track info */
	switch(pTracks->type)
	{
		case PAL_MEDIA_TRACK_TYPE_AUTIO:
			update_info.ucTotalAudioTrack = pTracks->track_count;
			pstMediaHdl->tMediaInfo.ucTotalAudioTrack = pTracks->track_count;
			for(i=0; i<pTracks->track_count; i++)
			{
				update_info.stAudioTrack[i].audio_pes_id = pTracks->track_info.audio[i].audio_pes_id;
				update_info.stAudioTrack[i].audio_stream_id = pTracks->track_info.audio[i].audio_stream_id;
				update_info.stAudioTrack[i].audio_codec = __pal_media_audio_codec_to_di(pTracks->track_info.audio[i].audio_codec);
				HxSTD_memcpy(update_info.stAudioTrack[i].uacAudioLang, pTracks->track_info.audio[i].uacAudioLang, 256);

				pstMediaHdl->tMediaInfo.stAudioTrack[i].audio_pes_id = pTracks->track_info.audio[i].audio_pes_id;
				pstMediaHdl->tMediaInfo.stAudioTrack[i].audio_stream_id = pTracks->track_info.audio[i].audio_stream_id;
				pstMediaHdl->tMediaInfo.stAudioTrack[i].audio_codec = __pal_media_audio_codec_to_di(pTracks->track_info.audio[i].audio_codec);
				HxSTD_memcpy(pstMediaHdl->tMediaInfo.stAudioTrack[i].uacAudioLang, pTracks->track_info.audio[i].uacAudioLang, 256);

			}
			break;

		case PAL_MEDIA_TRACK_TYPE_VIDEO:
			update_info.ucTotalVideoTrack = pTracks->track_count;
			pstMediaHdl->tMediaInfo.ucTotalVideoTrack = pTracks->track_count;
			for(i=0; i<pTracks->track_count; i++)
			{
				update_info.stVideoTrack[i].video_pes_id = pTracks->track_info.video[i].video_pes_id;
				update_info.stVideoTrack[i].video_stream_id = pTracks->track_info.video[i].video_stream_id;
				update_info.stVideoTrack[i].video_codec = __pal_media_video_codec_to_di(pTracks->track_info.video[i].video_codec);

				pstMediaHdl->tMediaInfo.stVideoTrack[i].video_pes_id = pTracks->track_info.video[i].video_pes_id;
				pstMediaHdl->tMediaInfo.stVideoTrack[i].video_stream_id = pTracks->track_info.video[i].video_stream_id;
				pstMediaHdl->tMediaInfo.stVideoTrack[i].video_codec = __pal_media_video_codec_to_di(pTracks->track_info.video[i].video_codec);
			}
			break;

#if 0
		case PAL_MEDIA_TRACK_TYPE_XSUB:
			update_info.ucTotalXsubTrack = pTracks->track_count;
			pstMediaHdl->tMediaInfo.ucTotalXsubTrack = pTracks->track_count;
			for(i=0; i<pTracks->track_count; i++)
			{
				update_info.stXsubTrack[i].eXsubType = pTracks->track_info.xsub[i].eXsubType;
				HxSTD_memcpy(update_info.stXsubTrack[i].uacXsubLang, pTracks->track_info.xsub[i].uacXsubLang, 256);

				pstMediaHdl->tMediaInfo.stXsubTrack[i].eXsubType = pTracks->track_info.xsub[i].eXsubType;
				HxSTD_memcpy(pstMediaHdl->tMediaInfo.stXsubTrack[i].uacXsubLang, pTracks->track_info.xsub[i].uacXsubLang, 256);
			}
			break;
#endif

		default:
			return ERR_FAIL;
	}

	eRet = DI_MEDIA_UpdatePmtInfo(pstMediaHdl->pIdentifier, pstMediaHdl->ulMediaId, &update_info);
	if(eRet != ERR_OK)
	{
		HxLOG_Print("[%s] Fail DI_MEDIA_UpdatePmtInfo. \n", __FUNCTION__);
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif

HERROR PAL_MEDIA_SetAudioStereoConfig(PAL_MEDIA_HANDLE phMedia, DxStereoSelect_e eStereoConfig)
{
	HERROR					err = ERR_OK;
	PAL_MEDIA_t 			*pstMediaHdl = phMedia;
	DI_AUDIO_STEREO_MODE	eDiStereoMode = DI_AUDIO_DUAL_Stereo;

	PAL_MEDIA_ASSERT(phMedia);

	if (eStereoConfig == eDxSTEREO_SELECT_UNKNOWN)
	{
		HxLOG_Error("\tERROR eStereoConfig is : 0x%X\n",eStereoConfig);
		err = ERR_FAIL;
		goto exit;
	}

	switch (eStereoConfig)
	{
		case eDxSTEREO_SELECT_STEREO:
			eDiStereoMode = DI_AUDIO_DUAL_Stereo;
			break;
		case eDxSTEREO_SELECT_MONOLEFT:
			eDiStereoMode = DI_AUDIO_DUAL_LeftOnly;
			break;
		case eDxSTEREO_SELECT_MONORIGHT:
			eDiStereoMode = DI_AUDIO_DUAL_RightOnly;
			break;
		case eDxSTEREO_SELECT_MONOMIXED:
			eDiStereoMode = DI_AUDIO_DUAL_Mix;
			break;
		default:
			HxLOG_Error("\tERROR not supported mode\n");
			break;
	}

	err = DI_AUDIO_SetStereo(pstMediaHdl->eAudioDecoderType, eDiStereoMode);
	if(err != ERR_OK)
	{
		HxLOG_Error("\tERROR DI_AUDIO_SetStereo ret : 0x%X\n",err);
	}

exit:

	return err;
}

HERROR PAL_MEDIA_GetAudioStreamInfo(PAL_MEDIA_HANDLE phMedia, PAL_MEDIA_AudioStreamInfo_t *pstStreamInfo)
{
	HERROR					err = ERR_OK;
	PAL_MEDIA_t 			*pstMediaHdl = phMedia;
	DI_AUDIO_STREAM_INFO_t	stDiStreamInfo;

	PAL_MEDIA_ASSERT(phMedia);
	PAL_MEDIA_ASSERT(pstStreamInfo);

	err = DI_AUDIO_GetStreamInfo(pstMediaHdl->eAudioDecoderType, &stDiStreamInfo);
	if (err == DI_ERR_OK)
	{
		pstStreamInfo->eCodec = __pal_media_audio_codec_to_ap(stDiStreamInfo.eFormat);
		switch(stDiStreamInfo.eMode)
		{
			case DI_AUDIO_MODE_STEREO :
				pstStreamInfo->eStereoType = eDxSTEREO_FORMAT_2CH;
				break;
			case DI_AUDIO_MODE_MUTI_STEREO :
				pstStreamInfo->eStereoType = eDxSTEREO_FORMAT_MULTI;
				break;
			case DI_AUDIO_MODE_MONO :
				pstStreamInfo->eStereoType = eDxSTEREO_FORMAT_MONO;
				break;
			case DI_AUDIO_MODE_DUAL_MONO :
				pstStreamInfo->eStereoType = eDxSTEREO_FORMAT_DUALMONO;
				break;
			default:
				HxLOG_Error("\tERROR not supported DI audio mode (%d) \n", stDiStreamInfo.eMode);
				pstStreamInfo->eStereoType = eDxSTEREO_FORMAT_UNKNOWN;
				break;
		}
		pstStreamInfo->ulSampleRate = stDiStreamInfo.ulSampleRate;
	}

	return err;
}


