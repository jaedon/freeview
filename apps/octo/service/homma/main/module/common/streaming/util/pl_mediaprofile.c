/**************************************************************
*	@file		pal_hls_profile.c
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


/*
 * pstProfile 관련 참조 site
 * + nhn
 *   - http://helloworld.naver.com/helloworld/7122
 *
 * + apple   :
 *   - http://developer.apple.com/library/ios/#documentation/networkinginternet/conceptual/streamingmediaguide/UsingHTTPLiveStreaming/UsingHTTPLiveStreaming.html
 *
 * + android :
 *    -(?) 추가 시켜 주세요.
 */
/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/

#include "../int_streaming.h"




/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/

#undef	HxLOG_DOMAIN
#define	HxLOG_DOMAIN	"HLIB_PROFILE"

#define PL_MediaProfile_VERSIONFILE_PATH		"/usr/etc/transcoder_profile.hxm"
#define PL_MediaProfile_LOWPROFILE_APPENDSTR	"_##LOW##"


#define PL_MediaProfile_CHECKARGUMENT(x)	if(((void *)x==(void *)HANDLE_NULL) || ((void *)x == (void *)NULL) || (x->hProfileHandle==HANDLE_NULL)) \
										{																				\
											HxLOG_Error("error arg!!\r\n");				\
											return eHOM_Result_Error;															\
										}

typedef struct
{
	HxHASH_t 			*load_profileHash;
	HxVERFILE_t			*load_versionFile;
	HBOOL				is_init;
	HINT32				last_profile_handle;
} _PL_MediaProfile_Mgr_t;

struct _PL_MediaProfile_t
{
	Handle_t			hProfileHandle;
	HCHAR  				pszDeviceName[HxMediaProfile_NAME_MAX_LEN];

	HBOOL				bIsInsertPCR;
	HBOOL				bUseReference;
	/* high */
	HINT32				nStartNum;
	HINT32				nLastNum;
	HCHAR				*pHighRefName;
	HxList_t 			*pItemTableList; /* PLMediaProfile_Item_t */

	/* low */
	HCHAR				*pLowRefName;

	/* chunkd */
	HINT32 				nBitrateM3U8ChunkWaitCnt;
	HINT32 				nTsChunkWaitTime;
	HINT32 				nGapChunkCount;
	HINT32				nVideoChunkMultiply;
	HINT32				nAudioChunkMultiply;
	HINT32				nM3U8PrintChunkTime;
};


STATIC _PL_MediaProfile_Mgr_t		s_stProfileMgr = {0, };


/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/

STATIC HCHAR *_plmediaprofile_VideoCodecToAscii(DxVideoCodec_e codec)
{
	switch (codec)
	{
	case eDxVIDEO_CODEC_H264:	return "h264";
/*
		ENUM_TO_STR(eDxVIDEO_CODEC_UNKNOWN);
		ENUM_TO_STR(eDxVIDEO_CODEC_MPEG1);
		ENUM_TO_STR(eDxVIDEO_CODEC_MPEG2);
		ENUM_TO_STR(eDxVIDEO_CODEC_MPEG4_PART2);
		ENUM_TO_STR(eDxVIDEO_CODEC_H263);
		ENUM_TO_STR(eDxVIDEO_CODEC_H264);
		ENUM_TO_STR(eDxVIDEO_CODEC_H264_SVC);
		ENUM_TO_STR(eDxVIDEO_CODEC_H264_MVC);
		ENUM_TO_STR(eDxVIDEO_CODEC_VC1);
		ENUM_TO_STR(eDxVIDEO_CODEC_VC1_SIMPLEMAIN);
		ENUM_TO_STR(eDxVIDEO_CODEC_AVS);
		ENUM_TO_STR(eDxVIDEO_CODEC_RV40);
		ENUM_TO_STR(eDxVIDEO_CODEC_VP6);
		ENUM_TO_STR(eDxVIDEO_CODEC_DIVX311);
		ENUM_TO_STR(eDxVIDEO_CODEC_FromLIVE);
*/
	default:
		break;
	}
	return NULL;
}

STATIC DxVideoCodec_e _plmediaprofile_AsciiToVideoCodec(const HCHAR *ascii)
{
	HINT32		i = 0;
	HINT32		max = (HINT32)eDxVIDEO_CODEC_MAX_NUM;
	HCHAR		*text = NULL;

	for(i=0; i < max; i++)
	{
		text = _plmediaprofile_VideoCodecToAscii((DxVideoCodec_e)i);
		if(text == NULL)
		{
			continue;
		}

		if(HxSTD_StrCmp(text, ascii) == 0)
		{
			return (DxVideoCodec_e)i;
		}
	}

	return eDxVIDEO_CODEC_UNKNOWN;
}

STATIC HCHAR *_plmediaprofile_AudioCodecToAscii(DxAudioCodec_e codec)
{
	switch (codec)
	{
	case eDxAUDIO_CODEC_MP3: return "mp3";
	case eDxAUDIO_CODEC_AAC: return "aac";
/*
		ENUM_TO_STR(eDxAUDIO_CODEC_UNKNOWN);
		ENUM_TO_STR(eDxAUDIO_CODEC_MPEG);
		ENUM_TO_STR(eDxAUDIO_CODEC_MP3);
		ENUM_TO_STR(eDxAUDIO_CODEC_DOLBY_AC3);
		ENUM_TO_STR(eDxAUDIO_CODEC_DOLBY_AC3P);
		ENUM_TO_STR(eDxAUDIO_CODEC_PCM);

		ENUM_TO_STR(eDxAUDIO_CODEC_AAC);
		ENUM_TO_STR(eDxAUDIO_CODEC_AAC_LOAS);
		ENUM_TO_STR(eDxAUDIO_CODEC_AAC_PLUS);
		ENUM_TO_STR(eDxAUDIO_CODEC_AAC_PLUS_ADTS);
		ENUM_TO_STR(eDxAUDIO_CODEC_DTS);
		ENUM_TO_STR(eDxAUDIO_CODEC_DTS_HD);
		ENUM_TO_STR(eDxAUDIO_CODEC_LPCM_DVD);
		ENUM_TO_STR(eDxAUDIO_CODEC_LPCM_HD_DVD);
		ENUM_TO_STR(eDxAUDIO_CODEC_LPCM_BLUERAY);
		ENUM_TO_STR(eDxAUDIO_CODEC_WMA_STD);
		ENUM_TO_STR(eDxAUDIO_CODEC_WMA_PRO);
		ENUM_TO_STR(eDxAUDIO_CODEC_PCMWAVE);
		ENUM_TO_STR(eDxAUDIO_CODEC_DRA);

		ENUM_TO_STR(eDxAUDIO_CODEC_AIFF);
		ENUM_TO_STR(eDxAUDIO_CODEC_RIFF);
		ENUM_TO_STR(eDxAUDIO_CODEC_FromLIVE);
*/
	default:
		break;
	}
	return NULL;
}

STATIC HBOOL _plmediaprofile_AsciiToCheckAsterisk(const HCHAR *ascii)
{
	/* if property is '*', bUsingLiveXXX is TRUE */
	if(HxSTD_StrCmp("*", ascii) == 0)
	{
		return TRUE;
	}

	return FALSE;
}

STATIC DxAudioCodec_e _plmediaprofile_AsciiToAudioCodec(const HCHAR *ascii)
{
	HINT32		i = 0;
	HINT32		max = (HINT32)eDxAUDIO_CODEC_MAX_NUM;
	HCHAR		*text = NULL;

	for(i=0; i < max; i++)
	{
		text = _plmediaprofile_AudioCodecToAscii((DxAudioCodec_e)i);
		if(text == NULL)
		{
			continue;
		}

		if(HxSTD_StrCmp(text, ascii) == 0)
		{
			return (DxAudioCodec_e)i;
		}
	}

	return eDxVIDEO_CODEC_UNKNOWN;
}

STATIC HCHAR *_plmediaprofile_VideoAspectRatioToAscii(DxAspectRatio_e eAspectRatio)
{
	switch (eAspectRatio)
	{
	case eDxASPECTRATIO_4X3:	return "4:3";
	case eDxASPECTRATIO_14X9:	return "14:9";
	case eDxASPECTRATIO_16X9:	return "16:9";
/*
		ENUM_TO_STR(eDxASPECTRATIO_UNKNOWN);
		ENUM_TO_STR(eDxASPECTRATIO_4X3);
		ENUM_TO_STR(eDxASPECTRATIO_16X9);
		ENUM_TO_STR(eDxASPECTRATIO_1X1);
		ENUM_TO_STR(eDxASPECTRATIO_14X9);
		ENUM_TO_STR(eHxASPECTRATIO_FromLIVE);
		ENUM_TO_STR(eDxASPECTRATIO_MAX_NUM);
*/
		default: break;
	}

	return NULL;
}

STATIC DxAspectRatio_e _plmediaprofile_AsciiToVideoAspectRatio(const HCHAR *ascii)
{
	HINT32		i = 0;
	HINT32		max = (HINT32)eDxASPECTRATIO_MAX_NUM;
	HCHAR		*text = NULL;

	for(i=0; i < max; i++)
	{
		text = _plmediaprofile_VideoAspectRatioToAscii((DxAspectRatio_e)i);
		if(text == NULL)
		{
			continue;
		}

		if(HxSTD_StrCmp(text, ascii) == 0)
		{
			return (DxAspectRatio_e)i;
		}
	}

	return eDxVIDEO_CODEC_UNKNOWN;
}


STATIC HCHAR *_plmediaprofile_VidoeEncCodecLevelToAscii(DxVideoEncodeLevel_e level)
{
	switch (level)
	{
	case eDxVIDEO_ENC_LEVEL_30:	return "30";
	case eDxVIDEO_ENC_LEVEL_31:	return "31";
/*
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_00);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_10);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_1B);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_11);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_12);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_13);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_20);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_21);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_22);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_30);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_31);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_32);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_40);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_41);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_42);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_50);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_51);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_60);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_62);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_LOW);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_MAIN);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_HIGH);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_HIGH1440);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_AUTO);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_NONE);
*/
		default: break;
	}

	return NULL;
}


STATIC DxVideoEncodeLevel_e _plmediaprofile_AsciiToVidoeEncCodecLevel(const HCHAR *ascii)
{
	HINT32		i = 0;
	HINT32		max = (HINT32)eDxVIDEO_ENC_LEVEL_MAX;
	HCHAR		*text = NULL;

	for(i=0; i < max; i++)
	{
		text = _plmediaprofile_VidoeEncCodecLevelToAscii((DxVideoEncodeLevel_e)i);
		if(text == NULL)
		{
			continue;
		}

		if(HxSTD_StrCmp(text, ascii) == 0)
		{
			return (DxVideoEncodeLevel_e)i;
		}
	}

	return eDxVIDEO_ENC_LEVEL_30;
}



STATIC HCHAR *_plmediaprofile_VidoeEncCodecProfileToAscii(DxVideoEncodeProfile_e level)
{
	switch (level)
	{
		case eDxVIDEO_ENC_PROFILE_MAIN:		return "main";
		case eDxVIDEO_ENC_PROFILE_HIGH:		return "high";
		case eDxVIDEO_ENC_PROFILE_BASELINE:	return "baseline";
/*
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_SIMPLE);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_MAIN);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_HIGH);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_ADVANCED);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_JIZHUN);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_SNRSCALABLE);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_SPATIALLYSCALABLE);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_ADVANCEDSIMPLE);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_BASELINE);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_AUTO);
*/
		default: break;
	}

	return NULL;
}


STATIC DxVideoEncodeProfile_e _plmediaprofile_AsciiToVidoeEncCodecProfile(const HCHAR *ascii)
{
	HINT32		i = 0;
	HINT32		max = (HINT32)eDxVIDEO_ENC_PROFILE_MAX_NUM;
	HCHAR		*text = NULL;

	for(i=0; i < max; i++)
	{
		text = _plmediaprofile_VidoeEncCodecProfileToAscii((DxVideoEncodeProfile_e)i);
		if(text == NULL)
		{
			continue;
		}

		if(HxSTD_StrCmp(text, ascii) == 0)
		{
			return (DxVideoEncodeProfile_e)i;
		}
	}

	return eDxVIDEO_ENC_PROFILE_BASELINE;
}

STATIC HxVERFILE_t *_plmediaprofile_Load(HCHAR *path)
{
	HxFILE_t		file = NULL;
	HxVERFILE_t		*load_versionFile = NULL;

	HxLOG_Debug("(+)\n");

	file = HLIB_FILE_Open(path, "rb");
	if(file == NULL)
	{
		HxLOG_Error("open err (%s) (-)\n", path);
		return NULL;
	}

	load_versionFile = HLIB_VERFILE_NewFromFile(file);
	HLIB_FILE_Close(file);

	if(load_versionFile == NULL)
	{
		HxLOG_Error("HxVersionFile Parse Error (%s) (-)\n", path);
		return NULL;
	}

	HxLOG_Info("(-)\n");

	return load_versionFile;
}


#define ____PARSE_VERSION_1_0_FUNCTIONS_______________________

#define DEF_CHUNK_SIZE		(188)
#define DEF_AUDIO_MULTIPLY	(DEF_CHUNK_SIZE * 1024 * 4)
#define DEF_VIDEO_MULTIPLY	(DEF_CHUNK_SIZE * 1024)

#define HxMediaProfile_HANDLE_STARTNUM 2

#define HxMediaProfile_FLAG_MASK 	0xF0000000
	#define HxMediaProfile_FLAG_HIGH 		0x00000000
	#define HxMediaProfile_FLAG_LOW 		0x10000000

#define HxMediaProfile_MASK 		0xFFFF0000
#define HxMediaProfile_ITEM_MASK 	0x0000FFFF



#define ITEMNUMBER_TO_ITEMHANDLE(x) ((Handle_t)x)
#define ITEMHANDLE_TO_ITEMNUMBER(x) ((HINT32)x)

#define HxMediaProfile_MAKE_HANDLE(a,b) 	(((a) & HxMediaProfile_MASK) | ((b) & HxMediaProfile_ITEM_MASK))
#define HxMediaProfile_MAKE_NUMBER(a,b) 	(((a) & HxMediaProfile_MASK) | ((b) & HxMediaProfile_ITEM_MASK))


STATIC HOM_Result_e _plmediaprofile_ParseV21Title(_PL_MediaProfile_Mgr_t	*pMgr)
{
	HxSTR_t		*title = NULL;

	HxLOG_Assert(pMgr);

	HLIB_VERFILE_GetTitleToHxSTR(pMgr->load_versionFile, &title);

	return eHOM_Result_Ok;
}

STATIC HBOOL _plmediaprofile_ParseV21Contents_DecodeJson_IsProfileRoot(HxJSON_t pJsonRoot)
{
	const HCHAR 	*profile_name = NULL;

	profile_name = HLIB_JSON_Object_GetString(pJsonRoot, "profile_name");
	if(profile_name)
	{
		return TRUE;
	}

	return FALSE;
}

STATIC HBOOL _plmediaprofile_ParseV21Contents_DecodeJson_GetProfileRoot(PL_MediaProfile_t *pstProfile, HxJSON_t pJsonRoot, HINT32 unique_number)
{
	const HCHAR 	*profile_name = NULL;
	const HCHAR 	*high_reference_profile_name = NULL;
	const HCHAR		*low_reference_profile_name = NULL;

	HxLOG_Debug("(+)\n");

	profile_name = HLIB_JSON_Object_GetString(pJsonRoot, "profile_name");
	HxLOG_Assert(profile_name);
	HxLOG_Print("++[profile_name:%s]\n", profile_name);

	HLIB_STD_StrNCaseCpy(pstProfile->pszDeviceName, profile_name, (HxMediaProfile_NAME_MAX_LEN-12), TRUE);
	pstProfile->nStartNum 	= HxMediaProfile_MAKE_HANDLE(pstProfile->hProfileHandle, unique_number);
	pstProfile->nLastNum	= HxMediaProfile_MAKE_HANDLE(pstProfile->hProfileHandle, unique_number);

	pstProfile->nBitrateM3U8ChunkWaitCnt 	= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "bitrate_wait_chunk_count", 2);
	pstProfile->nTsChunkWaitTime 		= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "ts_wait_chunk_count", 2);
	pstProfile->nGapChunkCount 			= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "gap_chunk_count", 2);
	pstProfile->bIsInsertPCR					= HLIB_JSON_Object_GetBoolean(pJsonRoot, "pcr");
	pstProfile->nM3U8PrintChunkTime		= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "m3u8_print_chunktime", 3);
	pstProfile->nAudioChunkMultiply		= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "audio_chunk_multiply", DEF_AUDIO_MULTIPLY);
	pstProfile->nAudioChunkMultiply -= (pstProfile->nVideoChunkMultiply % DEF_CHUNK_SIZE);
	pstProfile->nVideoChunkMultiply		= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "video_chunk_multiply", DEF_VIDEO_MULTIPLY);
	pstProfile->nVideoChunkMultiply -= (pstProfile->nVideoChunkMultiply % DEF_CHUNK_SIZE);

	high_reference_profile_name = HLIB_JSON_Object_GetString(pJsonRoot, "high_reference_profile_name");
	HxLOG_Print(" + BitrateM3U8ChunkWaitCnt :%d\n", pstProfile->nBitrateM3U8ChunkWaitCnt);
	HxLOG_Print(" + TsChunkWaitTime         :%d\n", pstProfile->nTsChunkWaitTime);
	HxLOG_Print(" + GapChunkCount           :%d\n", pstProfile->nGapChunkCount);
	HxLOG_Print(" + InsertPCR               :%d\n", pstProfile->bIsInsertPCR);
	HxLOG_Print(" + high_reference_profile  :%d\n", high_reference_profile_name);
	if(high_reference_profile_name == NULL)
	{
		return FALSE;
	}
	pstProfile->pHighRefName = HLIB_STD_StrDup(high_reference_profile_name);

	low_reference_profile_name = HLIB_JSON_Object_GetString(pJsonRoot, "low_reference_profile_name");
	if(low_reference_profile_name != NULL)
	{
		pstProfile->pLowRefName = HLIB_STD_StrDup(low_reference_profile_name);
	}
	pstProfile->bUseReference = TRUE;

	HxLOG_Info("(-)\n");

	return TRUE;
}

STATIC PLMediaProfile_Item_t *_plmediaprofile_ParseV21Contents_DecodeJson_GetItem(PL_MediaProfile_t *pstProfile, HxJSON_t pJsonRoot, HCHAR *pszStr, HINT32 unique_number)
{
	const HCHAR			*json_str = NULL;
	PLMediaProfile_Item_t	*item = NULL;

	HxLOG_Debug("(+)\n");

	item = (PLMediaProfile_Item_t *)HLIB_STD_MemAlloc(sizeof(PLMediaProfile_Item_t));
	HxLOG_Assert(item);
	HxSTD_MemSet(item, 0, sizeof(PLMediaProfile_Item_t));

	item->hItem		= HxMediaProfile_MAKE_HANDLE(pstProfile->hProfileHandle, unique_number);
	item->nIndex	= ITEMHANDLE_TO_ITEMNUMBER(item->hItem);
	pstProfile->nLastNum	= HxMediaProfile_MAKE_NUMBER(pstProfile->hProfileHandle, unique_number+1);

	item->nVideoBitrateKbps		= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "video_bitrate_kbps", 1120);
	item->nSdVideoBitrateKbps		= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "sd_video_bitrate_kbps", item->nVideoBitrateKbps);
	item->nHdVideoBitrateKbps		= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "hd_video_bitrate_kbps", item->nVideoBitrateKbps);
	item->nScreenWidth				= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "screen_width", 640);
	item->nScreenHeight				= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "screen_height", 360);

	json_str = HLIB_JSON_Object_GetString(pJsonRoot, "aspect_ratio");
	HxLOG_Assert(json_str);
	item->eVideoAspectRatio			= _plmediaprofile_AsciiToVideoAspectRatio(json_str);
	item->bUsingLiveAspectRatio		= _plmediaprofile_AsciiToCheckAsterisk(json_str);

	item->bInterlaceMode			= HLIB_JSON_Object_GetBoolean(pJsonRoot, "interlace_mode");

	json_str = HLIB_JSON_Object_GetString(pJsonRoot, "video_enc_codec");
	HxLOG_Assert(json_str);
	item->eVideoCodec	= _plmediaprofile_AsciiToVideoCodec(json_str);

	json_str = HLIB_JSON_Object_GetString(pJsonRoot, "audio_enc_codec");
	HxLOG_Assert(json_str);
	item->nAudioBitrateKbps		= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "audio_bitrate_kbps", 64);
	item->nFrameRate				= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "frame_rate", 50);

	item->eAudioCodec			= _plmediaprofile_AsciiToAudioCodec(json_str);
	item->bUsingLiveAudioCodec	= _plmediaprofile_AsciiToCheckAsterisk(json_str);

	json_str = HLIB_JSON_Object_GetString(pJsonRoot, "video_enc_codec_level");
	HxLOG_Assert(json_str);
	item->eVideoCodecLevel		= _plmediaprofile_AsciiToVidoeEncCodecLevel(json_str);

	json_str = HLIB_JSON_Object_GetString(pJsonRoot, "video_enc_codec_profile");
	HxLOG_Assert(json_str);
	item->eVideoCodecProfile	= _plmediaprofile_AsciiToVidoeEncCodecProfile(json_str);

	item->bUseTranscoder		= HLIB_JSON_Object_GetBoolean(pJsonRoot, "use_transcoding");

	item->pszString				= HLIB_STD_StrDup(pszStr);

	HxLOG_Info("(-)\n");

	return item;
}



STATIC HOM_Result_e _plmediaprofile_ParseV21Contents(_PL_MediaProfile_Mgr_t	*pMgr)
{
	HxSTR_t				*contents = NULL;
	HCHAR				*buf = NULL;
	HINT32				size = 0;
	HINT32				unique_number = 0;
	HINT32				len = 0;
	HCHAR				*line = NULL;
	HxJSON_t			pJsonRoot = NULL;
	HBOOL				bUseReference = FALSE, is_profile_root = FALSE;
	HINT32				profile_index = HxMediaProfile_HANDLE_STARTNUM;
	PL_MediaProfile_t 		*pstProfile = NULL;
	PLMediaProfile_Item_t	*profile_item = NULL;

	HxLOG_Assert(pMgr);

	HxLOG_Debug("(+)\n");

	line = HLIB_STD_MemAlloc(5120);
	if(line == NULL)
	{
		HxLOG_Error("out of memory (-)\n");
		return eHOM_Result_Error;
	}

	HLIB_VERFILE_GetContentsToHxSTR(pMgr->load_versionFile, &contents);
	if (contents == NULL)
	{
		HLIB_STD_MemFree(line);
		HxLOG_Error("HLIB_VERFILE_GetContentsToHxSTR failed.\n");
		return eHOM_Result_Error;
	}

	size = HLIB_STR_GetLength(contents);
	buf = HLIB_STR_toUTF8(contents);

	while(size > 0)
	{
		if(pJsonRoot != NULL)
		{
			HLIB_JSON_Delete(pJsonRoot);
			pJsonRoot = NULL;
		}

		VK_memset(line, 0, sizeof(HCHAR) * 5120);
		len = HLIB_STD_StrNLine(line, buf, 5120);

		HxLOG_Print("line(%s)\r\n", line);

		if(len < 5)
		{
			while(!HxSTD_IsReturn(*buf))
			{
				size--;
				buf++;
			}

			while(HxSTD_IsReturn(*buf))
			{
				size--;
				buf++;
			}
			continue;
		}
		size -= len;
		buf += len;

		if(line[0] == '/' && line[1] == '/')
		{
			continue;
		}

		pJsonRoot = HLIB_JSON_Decode(line);
		if(pJsonRoot == NULL)
		{
			HxLOG_Print("HLIB_JSON_Decode error line(%s) \n", line);
			continue;
		}

		is_profile_root = _plmediaprofile_ParseV21Contents_DecodeJson_IsProfileRoot(pJsonRoot);
		if(is_profile_root == TRUE)
		{
			if(pstProfile != NULL)
			{
				HLIB_HASH_Insert(pMgr->load_profileHash, pstProfile->pszDeviceName, pstProfile);
				#if defined(CONFIG_DEBUG)
				PL_MediaProfile_Dump(pstProfile);
				#endif
				pstProfile = NULL;
			}

			pstProfile = (PL_MediaProfile_t *)HLIB_STD_MemAlloc(sizeof(PL_MediaProfile_t));
			if(pstProfile == NULL)
			{
				HxLOG_Assert(pstProfile)
				HxLOG_Error("Out of memory \n");
				return eHOM_Result_Error;
			}
			VK_memset(pstProfile, 0, sizeof(PL_MediaProfile_t));
			pstProfile->hProfileHandle = profile_index << 16;
			profile_index++;

			bUseReference = _plmediaprofile_ParseV21Contents_DecodeJson_GetProfileRoot(pstProfile, pJsonRoot, unique_number);
			if(bUseReference == TRUE)
			{
				HLIB_HASH_Insert(pMgr->load_profileHash, pstProfile->pszDeviceName, pstProfile);
				pstProfile = NULL;
			}
		}
		else if(pstProfile != NULL)
		{
			profile_item = _plmediaprofile_ParseV21Contents_DecodeJson_GetItem(pstProfile, pJsonRoot, line, unique_number);
			if(profile_item != NULL)
			{
				unique_number++;
				pstProfile->pItemTableList = HLIB_LIST_Append(pstProfile->pItemTableList, profile_item);
				profile_item = NULL;
			}
		}

	}

	pMgr->last_profile_handle = profile_index;

	if(profile_item)
	{
		HLIB_STD_MemFree(profile_item);
		profile_item = NULL;
	}

	if(pstProfile)
	{
		HLIB_STD_MemFree(pstProfile);
		pstProfile = NULL;
	}

	if(pJsonRoot != NULL)
	{
		HLIB_JSON_Delete(pJsonRoot);
		pJsonRoot = NULL;
	}

	if(line)
	{
		HLIB_STD_MemFree(line);
		line = NULL;
	}

	HxLOG_Info("(-)\n");

	return eHOM_Result_Ok;
}


#define ____PRIVAETE_FUNCTIONS_______________________

STATIC HOM_Result_e _plmediaprofile_Parse(_PL_MediaProfile_Mgr_t	*pMgr)
{
	HOM_Result_e	hErr = eHOM_Result_Ok;
	HINT32	major = 0;
	HINT32	minor = 0;

	HLIB_VERFILE_GetVersion(pMgr->load_versionFile, &major,  &minor);

	HxLOG_Print("major(%d), minor(%d) (+)\n", major, minor);

	if(major == 2 && minor == 1)
	{

		hErr = _plmediaprofile_ParseV21Title(pMgr);
		if(hErr != eHOM_Result_Ok)
		{
			return eHOM_Result_Error;
		}

		hErr = _plmediaprofile_ParseV21Contents(pMgr);
		if(hErr != eHOM_Result_Ok)
		{
			return eHOM_Result_Error;
		}

		return eHOM_Result_Ok;
	}

	HxLOG_Error("Unkwon Version Number \n");
#if defined(CONFIG_DEBUG)
	HLIB_VERFILE_Dump(pMgr->load_versionFile);
#endif

	return eHOM_Result_Error;
}

STATIC void	_plmediaprofile_ListItemDelete(void* data)
{
	PLMediaProfile_Item_t *item = (PLMediaProfile_Item_t *)data;
	if(item)
	{
		if(item->pszString)
		{
			HLIB_STD_MemFree(item->pszString);
		}

		HLIB_STD_MemFree(item);
	}
}


STATIC void	_plmediaprofile_Delete(void* data)
{
	PL_MediaProfile_t *pstProfile = (PL_MediaProfile_t *)data;

	if(pstProfile)
	{
		if((pstProfile->hProfileHandle & HxMediaProfile_FLAG_MASK) == HxMediaProfile_FLAG_HIGH)
		{
			if(pstProfile->pHighRefName)
			{
				HLIB_STD_MemFree(pstProfile->pHighRefName);
				pstProfile->pHighRefName = NULL;
			}

			if(pstProfile->pLowRefName)
			{
				HLIB_STD_MemFree(pstProfile->pLowRefName);
				pstProfile->pLowRefName = NULL;
			}

			if(pstProfile->pItemTableList)
			{
				HLIB_LIST_RemoveAllFunc(pstProfile->pItemTableList, _plmediaprofile_ListItemDelete);
			}
		}

		HLIB_STD_MemFree(pstProfile);
	}
}


STATIC HBOOL	_plmediaprofile_FindProfileItemByNumber(void* key , void* value , void* user_data )
{
	HINT32	nItemNumber = *(HINT32 *)user_data;

	PL_MediaProfile_t *pstProfile = (PL_MediaProfile_t *)value;

	if((pstProfile->hProfileHandle & HxMediaProfile_MASK) != (nItemNumber & HxMediaProfile_MASK))
	{
		return FALSE;
	}

	if(pstProfile->nStartNum <= nItemNumber && nItemNumber < pstProfile->nLastNum)
	{
		return TRUE;
	}

	return FALSE;
}

STATIC HBOOL	_plmediaprofile_FindProfileByHandle(void* key , void* value , void* user_data )
{
	Handle_t			hProfileHandle = *((Handle_t *)user_data);
	PL_MediaProfile_t 	*pstProfile = (PL_MediaProfile_t *)value;

	if((pstProfile->hProfileHandle & HxMediaProfile_MASK) == (hProfileHandle & HxMediaProfile_MASK))
	{
		return TRUE;
	}

	return FALSE;
}


STATIC HBOOL	_plmediaprofile_FindProfileByProfileName	(void*  key , void*  value , void*  user_data)
{
	PL_MediaProfile_t	*pstProfile = (PL_MediaProfile_t *)value;
	HCHAR *pProfileName = (HCHAR*)user_data;

	if(pstProfile == NULL)
	{
		return FALSE;
	}

	if(HLIB_STD_StrCaseCmp( pstProfile->pszDeviceName, pProfileName) == 0 )
	{
		return TRUE;
	}

	return FALSE;
}

STATIC HBOOL	_plmediaprofile_FindProfileByUserAgent	(void*  key , void*  value , void*  user_data)
{
	HCHAR	*pszUserAgent = (HCHAR *)user_data;

	PL_MediaProfile_t	*pstProfile = (PL_MediaProfile_t *)value;

	if(pstProfile == NULL || pszUserAgent == NULL)
	{
		return FALSE;
	}

	if(HxSTD_StrStr(pszUserAgent, pstProfile->pszDeviceName) > 0)
	{
		return TRUE;
	}

	return FALSE;
}


STATIC void	_plmediaprofile_ForeachCount(void* key , void* value , void* user_data )
{
	HINT32			*count = (HINT32 *)user_data;
	PL_MediaProfile_t	*pstProfile = (PL_MediaProfile_t *)value;

	if(pstProfile->bUseReference == FALSE)
	{
		*count += (pstProfile->nLastNum - pstProfile->nStartNum);
	}

	NOT_USED_PARAM(key);
	NOT_USED_PARAM(value);
}


STATIC PL_MediaProfile_t *_plmediaprofile_ChooseHighProfile( const HCHAR *pszDeviceName, const HCHAR *pszUserAgent )
{
	HINT32			tok_count = 0;
	PL_MediaProfile_t	*pstProfile = NULL;

	HxLOG_Print("device name (%s) pszUserAgent(%s)\r\n", pszDeviceName, pszUserAgent);

	/* 1. if user-agent inserted from Humax live app by model name */
	tok_count = HLIB_STD_StrCountChar(pszUserAgent, ';');
	if(tok_count >= 4)
	{
		pstProfile = HLIB_HASH_Find(s_stProfileMgr.load_profileHash,
									_plmediaprofile_FindProfileByUserAgent,
									(void *)pszUserAgent);
		if(pstProfile != NULL)
		{
			HxLOG_Print("pstProfile(%s, %s)\r\n", pstProfile->pszDeviceName, pstProfile->pHighRefName);
			return pstProfile;
		}
	}

	/* 2. if user-agent inserted from Humax live app by os name */
	if(tok_count >= 4)
	{
		HCHAR *temp_user_agent = NULL;

		if(HxSTD_StrStr(pszUserAgent, "android 4.0."))
		{
			temp_user_agent = "gt-p75";	// 4.0.x는 겔탭10.1로
		}
		else if(HxSTD_StrStr(pszUserAgent, "android"))
		{
			temp_user_agent = "gt-n80";
		}
		else if(HxSTD_StrStr(pszUserAgent, "iphone"))
		{
			temp_user_agent = "iphone4";
		}
		else if(HxSTD_StrStr(pszUserAgent, "ipad"))
		{
			temp_user_agent = "ipad3";
		}

		if(temp_user_agent)
		{
			pstProfile = HLIB_HASH_Find(s_stProfileMgr.load_profileHash,
										_plmediaprofile_FindProfileByUserAgent,
										(void *)temp_user_agent);
			if(pstProfile != NULL)
			{
				HxLOG_Print("pstProfile(%s, %s)\r\n", pstProfile->pszDeviceName, pstProfile->pHighRefName);
				return pstProfile;
			}
		}
	}

	/* 3. find device name */
	if(HxSTD_StrCmp(pszDeviceName, "unknown") != 0)
	{
		pstProfile = HLIB_HASH_Lookup(s_stProfileMgr.load_profileHash, pszDeviceName);
		if(pstProfile != NULL)
		{
			HxLOG_Print("pstProfile(%s, %s)\r\n", pstProfile->pszDeviceName, pstProfile->pHighRefName);
			return pstProfile;
		}
	}

	/* 4. find user-agent device */
	if(HxSTD_StrLen(pszUserAgent) > 3)
	{
		pstProfile = HLIB_HASH_Find(s_stProfileMgr.load_profileHash,
									_plmediaprofile_FindProfileByUserAgent,
									(void *)pszUserAgent);
		if(pstProfile != NULL)
		{
			HxLOG_Print("pstProfile(%s, %s)\r\n", pstProfile->pszDeviceName, pstProfile->pHighRefName);
			return pstProfile;
		}
	}

	/* 5. find unknown device */
	HxLOG_Print("Error> Can not find device name (%s) pszUserAgent(%s)\r\n", pszDeviceName, pszUserAgent);
	pstProfile = HLIB_HASH_Lookup(s_stProfileMgr.load_profileHash, "unknown");

	return pstProfile;
}

STATIC PL_MediaProfile_t *_plmediaprofile_ChooseLowProfile( PL_MediaProfile_t 	*high_profile, const HCHAR *pszUserAgent )
{
	HCHAR				low_pszDeviceName[HxMediaProfile_NAME_MAX_LEN] = {0, };
	PL_MediaProfile_t 	*pstProfile = high_profile;

	if( HxSTD_StrCmp( high_profile->pszDeviceName, "unknown") == 0 )
	{
		return pstProfile;
	}

	if(HLIB_STD_StrCaseStr(pszUserAgent, "external") != NULL
		|| HLIB_STD_StrCaseStr(pszUserAgent, ";3g;") != NULL)
	{
		HxSTD_snprintf(low_pszDeviceName, HxMediaProfile_NAME_MAX_LEN, "%s"PL_MediaProfile_LOWPROFILE_APPENDSTR, high_profile->pszDeviceName);

		pstProfile = HLIB_HASH_Lookup(s_stProfileMgr.load_profileHash, low_pszDeviceName);
		if(pstProfile == NULL)
		{
			pstProfile = high_profile;
		}
	}

	return pstProfile;
}

STATIC PL_MediaProfile_t *_plmediaprofile_ChooseProfile( const HCHAR *pszDeviceName, const HCHAR *pszUserAgent )
{
	HCHAR				lower_pszDeviceName[HxMediaProfile_NAME_MAX_LEN] = {0, };
	HCHAR				lower_user_agent[HxMediaProfile_USERAGENT_MAX_LEN] = {0, };
	PL_MediaProfile_t 	*pstProfile = NULL;

	if(pszDeviceName != NULL)
	{
		HLIB_STD_StrNCaseCpy(lower_pszDeviceName, pszDeviceName, HxMediaProfile_NAME_MAX_LEN-1, TRUE);
	}

	if(pszUserAgent != NULL)
	{
		HLIB_STD_StrNCaseCpy(lower_user_agent, pszUserAgent, HxMediaProfile_USERAGENT_MAX_LEN-1, TRUE);
	}

	pstProfile = _plmediaprofile_ChooseHighProfile( lower_pszDeviceName, lower_user_agent );
	if(pstProfile == NULL)
	{
		HxLOG_Error("_plmediaprofile_ChooseHighProfile error!!\r\n");
		return NULL;
	}

	pstProfile = _plmediaprofile_ChooseLowProfile( pstProfile, lower_user_agent );
	if(pstProfile == NULL)
	{
		HxLOG_Error("_plmediaprofile_ChooseHighProfile error!!\r\n");
		return NULL;
	}

	HxLOG_Print("UserAgent:%s, high_profile->Device:%s, lower_Device:%s\n",
			pszUserAgent, lower_user_agent, lower_pszDeviceName);
	return pstProfile;
}
/**********************************************************************************************************/
#define ____INTERNAL_FUNCTIONS_______________________
/**********************************************************************************************************/
STATIC HOM_Result_e _plmediaprofile_ConnectLink(_PL_MediaProfile_Mgr_t *pMgr)
{
	HxHASH_TableIter_t	iter;
	PL_MediaProfile_t	*pstProfile = NULL;
	HxList_t			*dup_list = NULL;
	HxList_t			*temp_list = NULL;
	PL_MediaProfile_t	*dup_profile = NULL;
	PL_MediaProfile_t	*high_ref_profile = NULL;
	PL_MediaProfile_t	*low_ref_profile = NULL;

	HLIB_HASH_IterInit(&iter, pMgr->load_profileHash);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&pstProfile))
	{
		if(pstProfile->pLowRefName)
		{
			low_ref_profile = HLIB_HASH_Lookup(s_stProfileMgr.load_profileHash, pstProfile->pLowRefName);
			if(low_ref_profile)
			{
				dup_profile = HLIB_STD_MemDup(pstProfile, sizeof(PL_MediaProfile_t));
				if(dup_profile == NULL)
				{
					HxLOG_Error("out of mem\n");
					if (dup_list)
						HLIB_LIST_RemoveAll(dup_list);
					return eHOM_Result_Error;
				}

				dup_profile->hProfileHandle = (pstProfile->hProfileHandle | HxMediaProfile_FLAG_LOW);

				HxSTD_StrNCat(dup_profile->pszDeviceName, PL_MediaProfile_LOWPROFILE_APPENDSTR , HxMediaProfile_NAME_MAX_LEN-1);
				dup_profile->nStartNum 	= HxMediaProfile_MAKE_HANDLE(dup_profile->hProfileHandle, low_ref_profile->nStartNum);
				dup_profile->nLastNum  	= HxMediaProfile_MAKE_HANDLE(dup_profile->hProfileHandle, low_ref_profile->nLastNum);
				dup_profile->pItemTableList		= low_ref_profile->pItemTableList;

				dup_list = HLIB_LIST_Append(dup_list, dup_profile);
				dup_profile = NULL;
			}
		}

		if(pstProfile->pHighRefName)
		{
			high_ref_profile = HLIB_HASH_Lookup(s_stProfileMgr.load_profileHash, pstProfile->pHighRefName);
			if(high_ref_profile)
			{
				pstProfile->nStartNum 	= HxMediaProfile_MAKE_HANDLE(pstProfile->hProfileHandle, high_ref_profile->nStartNum);
				pstProfile->nLastNum  	= HxMediaProfile_MAKE_HANDLE(pstProfile->hProfileHandle, high_ref_profile->nLastNum);
				pstProfile->pItemTableList		= high_ref_profile->pItemTableList;
			}
		}
	}


	/*
		dup_list 풀면서 hash에 add 시키는 코드
	*/
	temp_list = dup_list;
	while(temp_list)
	{
		dup_profile = HLIB_LIST_Data(temp_list);
		if(dup_profile)
		{
			HLIB_HASH_Insert(s_stProfileMgr.load_profileHash, dup_profile->pszDeviceName, dup_profile);
		}
		temp_list = HLIB_LIST_Next(temp_list);
	}

	HLIB_LIST_RemoveAll(dup_list);
	return eHOM_Result_Ok;
}

STATIC PL_MediaProfile_t *_plmediaprofile_GetProfileByHandle( const Handle_t hProfileHandle )
{
	PL_MediaProfile_t	*pstProfile = NULL;

	if((hProfileHandle & HxMediaProfile_ITEM_MASK) == 0 )
	{
		HxLOG_Error("Handle Error!!  (-)\n", hProfileHandle);
		return NULL;
	}

	pstProfile = HLIB_HASH_Find(s_stProfileMgr.load_profileHash, _plmediaprofile_FindProfileByHandle, (void *)&hProfileHandle);
	if(pstProfile == NULL)
	{
		HxLOG_Error("pstProfile not found!  nItemNumber(0x%08x)  (-)\n", hProfileHandle);
		return NULL;
	}

	return pstProfile;
}

STATIC PL_MediaProfile_t *_plmediaprofile_GetProfileByItemNumber( const HINT32 nItemNumber )
{
	PL_MediaProfile_t	*pstProfile = NULL;

	if((nItemNumber & HxMediaProfile_ITEM_MASK) == 0 )
	{
		HxLOG_Error("Handle Error!!  (-)\n", nItemNumber);
		return NULL;
	}

	pstProfile = HLIB_HASH_Find(s_stProfileMgr.load_profileHash, _plmediaprofile_FindProfileItemByNumber, (void *)&nItemNumber);
	if(pstProfile == NULL)
	{
		HxLOG_Error("pstProfile not found!  nItemNumber(0x%08x)  (-)\n", nItemNumber);
		return NULL;
	}

	return pstProfile;
}

STATIC PL_MediaProfile_t *_plmediaprofile_GetProfile( const Handle_t hItem )
{
	return _plmediaprofile_GetProfileByItemNumber(ITEMHANDLE_TO_ITEMNUMBER(hItem));
}

STATIC PLMediaProfile_Item_t *_plmediaprofile_Item_GetByItemNumber( const HINT32 nItemNumber )
{
	HxList_t				*list_item = NULL;
	PL_MediaProfile_t		*pstProfile = NULL;
	PLMediaProfile_Item_t	*profile_item = NULL;

	HxLOG_Debug("(+)\n");

	pstProfile = _plmediaprofile_GetProfileByItemNumber(nItemNumber);
	if(pstProfile == NULL)
	{
		HxLOG_Error("pstProfile not found!  nItemNumber(0x%08x)  (-)\n", nItemNumber);
		return NULL;
	}

	list_item = HLIB_LIST_GetListItem(pstProfile->pItemTableList, (nItemNumber - pstProfile->nStartNum));
	if(list_item == NULL)
	{
		HxLOG_Error("profile_item not found!  nItemNumber(0x%08x)  (-)\n", nItemNumber);
		return NULL;
	}

	profile_item = (PLMediaProfile_Item_t *)HLIB_LIST_Data(list_item);

	HxLOG_Info("(-)\n");

	return profile_item;

}

STATIC PLMediaProfile_Item_t *_plmediaprofile_Item_Get( const Handle_t hItem )
{
	return _plmediaprofile_Item_GetByItemNumber(ITEMHANDLE_TO_ITEMNUMBER(hItem));
}


STATIC PLMediaProfile_Item_t *_plmediaprofile_Get( const Handle_t hItem )
{
	return _plmediaprofile_Item_GetByItemNumber(ITEMHANDLE_TO_ITEMNUMBER(hItem));
}


STATIC HOM_Result_e _plmediaprofile_Init(void)
{
	HOM_Result_e			hErr = eHOM_Result_Ok;
	HxVERFILE_t		*load_versionFile = NULL;


	if(s_stProfileMgr.is_init == TRUE)
	{
		HxLOG_Print("Duplicated Init(-)\n");
		return eHOM_Result_Ok;
	}

	load_versionFile = _plmediaprofile_Load(PL_MediaProfile_VERSIONFILE_PATH);
	if(load_versionFile == NULL)
	{
		HxLOG_Error("\n\n\nInit Profile Load fail : %s(-)\n", PL_MediaProfile_VERSIONFILE_PATH);
		HxLOG_Error("\t Don't use Profile on system\n\n\n");
		return eHOM_Result_Error;
	}

	HxSTD_MemSet(&s_stProfileMgr, 0, sizeof(_PL_MediaProfile_Mgr_t));
	s_stProfileMgr.load_versionFile = load_versionFile;
	s_stProfileMgr.load_profileHash = HLIB_HASH_NewFull(	HLIB_HASH_StrHash,
														HLIB_HASH_StrEqual,
														NULL,
														_plmediaprofile_Delete);

	hErr = _plmediaprofile_Parse(&s_stProfileMgr);
	if(eHOM_Result_Ok != hErr)
	{
		PL_MediaProfile_DeInit();

		HxLOG_Error("\n\n\nInit Profile Parse fail : %s(-)\n", PL_MediaProfile_VERSIONFILE_PATH);
		HxLOG_Error("\t Don't use Profile on system\n\n\n");

		return eHOM_Result_Error;
	}

	hErr = _plmediaprofile_ConnectLink(&s_stProfileMgr);
	if(eHOM_Result_Ok != hErr)
	{
		PL_MediaProfile_DeInit();

		HxLOG_Error("\n\n\nInit Profile _plmediaprofile_ConnectLink fail : %s(-)\n", PL_MediaProfile_VERSIONFILE_PATH);
		HxLOG_Error("\t Don't use Profile on system\n\n\n");

		return eHOM_Result_Error;
	}

	s_stProfileMgr.is_init = TRUE;

	return eHOM_Result_Ok;
}

STATIC HOM_Result_e _plmediaprofile_DeInit(void)
{
	if(s_stProfileMgr.is_init == FALSE)
	{
		HxLOG_Print("DoNotInit (-)\n");
		return eHOM_Result_Error;
	}

	if(s_stProfileMgr.load_versionFile)
	{
		HLIB_VERFILE_Delete(s_stProfileMgr.load_versionFile);
		s_stProfileMgr.load_versionFile = NULL;
	}

	if(s_stProfileMgr.load_profileHash)
	{
		HLIB_HASH_Destroy(s_stProfileMgr.load_profileHash);
		s_stProfileMgr.load_profileHash = NULL;
	}

	return eHOM_Result_Ok;
}

STATIC void _plmediaprofile_CheckInit(void)
{
	if(s_stProfileMgr.is_init == FALSE)
	{
		_plmediaprofile_Init();
	}
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
HOM_Result_e	PL_MediaProfile_Init (void)
{
	HOM_Result_e hErr = eHOM_Result_Ok;

	HxLOG_Debug("(+)\n");

	hErr = _plmediaprofile_Init();
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Error> Fail to init media profile.\n");
	}

	HxLOG_Info("(-)\n");

	return eHOM_Result_Ok;
}


/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_DeInit (void)
{
	HOM_Result_e hErr = eHOM_Result_Ok;

	HxLOG_Debug("(+)\n");

	hErr = _plmediaprofile_DeInit();
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Error> Fail to de-init media profile.\n");
	}

	HxLOG_Info("(-)\n");

	return eHOM_Result_Ok;
}



/**
*
*
*
* @param
* @return
*/
HOM_Result_e      PL_MediaProfile_Open( const HCHAR *pszDeviceName, const HCHAR *pszUserAgent, PL_MediaProfile_t **ret_pstProfile )
{
	PL_MediaProfile_t	*pstProfile = NULL;

	HxLOG_Debug("(+)\n");

	(void)_plmediaprofile_CheckInit();

	HxLOG_Print("pszDeviceName(%s)  pszUserAgent(%s)\r\n", pszDeviceName, pszUserAgent);

	/* Select Device Profile */
	pstProfile = _plmediaprofile_ChooseProfile(pszDeviceName, pszUserAgent);
	if(pstProfile == NULL)
	{
		HxLOG_Assert(pstProfile)
		HxLOG_Error("can't find pstProfile(-)\n");
		return eHOM_Result_Error;
	}

	*ret_pstProfile = pstProfile;

	HxLOG_Info("(-)\n");

	return eHOM_Result_Ok;
}

/**
*
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_Close( const PL_MediaProfile_t *pstProfile )
{

	HxLOG_Debug("(+)\n");

	/*
	 * close에서는 할 것이 없다. 의미상 만들어 둠.
	 */

	HxLOG_Info("(-)\n");

	return eHOM_Result_Ok;

}


/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_GetCount( HINT32 *ret_count )
{
	HINT32 count = 0;

	HxLOG_Debug("(+)\n");

	(void)_plmediaprofile_CheckInit();

	HLIB_HASH_Foreach(s_stProfileMgr.load_profileHash, _plmediaprofile_ForeachCount, &count);

	*ret_count = count;

	HxLOG_Info("(-)\n");

	return eHOM_Result_Ok;

}

/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_ForEach(void (*func)(void* key , void* value , void* user_data), void *userdata)
{
	HxLOG_Debug("(+)\n");

	(void)_plmediaprofile_CheckInit();

	HLIB_HASH_Foreach(s_stProfileMgr.load_profileHash, func, userdata);

	HxLOG_Info("(-)\n");

	return eHOM_Result_Ok;
}


/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_GetStartNumber( const PL_MediaProfile_t *pstProfile, HINT32 *ret_nStartNumber)
{
	PL_MediaProfile_CHECKARGUMENT(pstProfile)
	*ret_nStartNumber = pstProfile->nStartNum;
	return eHOM_Result_Ok;
}

/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_GetLastNumber( const PL_MediaProfile_t *pstProfile, HINT32 *ret_nLastNumber)
{
	PL_MediaProfile_CHECKARGUMENT(pstProfile)
	*ret_nLastNumber = pstProfile->nLastNum;
	return eHOM_Result_Ok;
}



/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_GetIsInsertPCR( const PL_MediaProfile_t *pstProfile, HBOOL *ret_bIsInsertPCR)
{
	PL_MediaProfile_CHECKARGUMENT(pstProfile)
	*ret_bIsInsertPCR = pstProfile->bIsInsertPCR;
	return eHOM_Result_Ok;
}


/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_GetBitrateM3U8WaitChunkCount( const PL_MediaProfile_t *pstProfile, HINT32 *ret_wait_chunk_count)
{
	PL_MediaProfile_CHECKARGUMENT(pstProfile)
	*ret_wait_chunk_count = pstProfile->nBitrateM3U8ChunkWaitCnt;
	return eHOM_Result_Ok;
}


/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_GetTSWaitChunkTime( const PL_MediaProfile_t *pstProfile, HINT32 *ret_wait_chunk_time)
{
	PL_MediaProfile_CHECKARGUMENT(pstProfile)
	*ret_wait_chunk_time = pstProfile->nTsChunkWaitTime;
	return eHOM_Result_Ok;
}

/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_GetM3U8PrintChunkTime( const PL_MediaProfile_t *pstProfile, HINT32 *ret_nM3U8PrintChunkTime)
{
	PL_MediaProfile_CHECKARGUMENT(pstProfile)
	*ret_nM3U8PrintChunkTime = pstProfile->nM3U8PrintChunkTime;
	return eHOM_Result_Ok;
}



/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_GetGapChunkCount( const PL_MediaProfile_t *pstProfile, HINT32 *ret_nGapChunkCount)
{
	PL_MediaProfile_CHECKARGUMENT(pstProfile)
	*ret_nGapChunkCount = pstProfile->nGapChunkCount;
	return eHOM_Result_Ok;
}


/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_GetVideoChunkMultiply( const PL_MediaProfile_t *pstProfile, HINT32 *ret_nVideoChunkMultiply)
{
	PL_MediaProfile_CHECKARGUMENT(pstProfile)
	*ret_nVideoChunkMultiply = pstProfile->nVideoChunkMultiply;
	return eHOM_Result_Ok;
}


/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_GetAudioChunkMultiply( const PL_MediaProfile_t *pstProfile, HINT32 *ret_nAudioChunkMultiply)
{
	PL_MediaProfile_CHECKARGUMENT(pstProfile)
	*ret_nAudioChunkMultiply = pstProfile->nAudioChunkMultiply;
	return eHOM_Result_Ok;
}


/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_GetList( const PL_MediaProfile_t *pstProfile , HxList_t **ret_list )
{
	PL_MediaProfile_CHECKARGUMENT(pstProfile)
	*ret_list  = pstProfile->pItemTableList;
	return eHOM_Result_Ok;

}

/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_GetHandle( const PL_MediaProfile_t *pstProfile , Handle_t *ret_phProfileHandle)
{
	PL_MediaProfile_CHECKARGUMENT(pstProfile)
	*ret_phProfileHandle  = pstProfile->hProfileHandle;
	return eHOM_Result_Ok;
}

/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_GetByHandle( const  Handle_t hProfileHandle, PL_MediaProfile_t **ppstProfile)
{
	if(NULL == ppstProfile)
	{
		return eHOM_Result_Error;
	}
	*ppstProfile = _plmediaprofile_GetProfileByHandle(hProfileHandle);
	return eHOM_Result_Ok;
}


/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_GetItem( const PL_MediaProfile_t *pstProfile, HINT32 nItemNumber, PLMediaProfile_Item_t **ret_item)
{
	HxList_t 				*list = NULL;
	PLMediaProfile_Item_t	*profile_item = NULL;

	PL_MediaProfile_CHECKARGUMENT(pstProfile)

	list = HLIB_LIST_GetListItem(pstProfile->pItemTableList, (nItemNumber-pstProfile->nStartNum));
	if(list == NULL)
	{
		HxLOG_Error("Get Error nItemNumber(0x%08x) (-)\n", nItemNumber);
		return eHOM_Result_Error;
	}

	profile_item = HLIB_LIST_Data(list);
	if(profile_item == NULL)
	{
		HxLOG_Error("Get Error (-)\n");
		return eHOM_Result_Error;
	}

	*ret_item = profile_item;

	return eHOM_Result_Ok;
}

/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_GetItemCount( const PL_MediaProfile_t *pstProfile, HINT32 *ret_count)
{
	PL_MediaProfile_CHECKARGUMENT(pstProfile)
	*ret_count = HLIB_LIST_Length(pstProfile->pItemTableList);
	return eHOM_Result_Ok;
}

/**
*
* nItemNumber => GetStartNumber + index
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_Item_Get( const Handle_t hMediaProfileItem, PLMediaProfile_Item_t **ret_item)
{
	PLMediaProfile_Item_t	*profile_item = NULL;

	HxLOG_Debug("(+)\n");

	profile_item = _plmediaprofile_Item_Get(hMediaProfileItem);
	if(profile_item == NULL)
	{
		HxLOG_Error("Get Error (-)\n");
		return eHOM_Result_Error;
	}

	*ret_item = profile_item;

	HxLOG_Info("(-)\n");

	return eHOM_Result_Ok;
}

/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_Item_GetBandwidthKBitrate( const Handle_t hMediaProfileItem, HINT32 *ret_bandwidth_kbyte)
{
	PLMediaProfile_Item_t	*profile_item = NULL;

	HxLOG_Debug("(+)\n");

	profile_item = _plmediaprofile_Item_Get(hMediaProfileItem);
	if(profile_item == NULL)
	{
		HxLOG_Error("Get Error (-)\n");
		return eHOM_Result_Error;
	}

	*ret_bandwidth_kbyte = (profile_item->nVideoBitrateKbps + profile_item->nAudioBitrateKbps) * 1024 * 12 / 10;

	HxLOG_Info("(-)\n");

	return eHOM_Result_Ok;
}

/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_Item_GetStartNumber( const Handle_t hMediaProfileItem, HINT32 *ret_nStartNumber)
{
	PL_MediaProfile_t		*pstProfile = NULL;

	HxLOG_Debug("(+)\n");

	pstProfile = _plmediaprofile_GetProfile(hMediaProfileItem);
	if(NULL == pstProfile)
	{
		HxLOG_Error("Get Error (-)\n");
		return eHOM_Result_Error;
	}

	*ret_nStartNumber = pstProfile->nStartNum;

	HxLOG_Info("(-)\n");

	return eHOM_Result_Ok;
}

/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_Item_GetLastNumber( const Handle_t hMediaProfileItem, HINT32 *ret_nLastNumber)
{
	PL_MediaProfile_t		*pstProfile = NULL;

	HxLOG_Debug("(+)\n");

	pstProfile = _plmediaprofile_GetProfile(hMediaProfileItem);
	if(NULL == pstProfile)
	{
		HxLOG_Error("Get Error (-)\n");
		return eHOM_Result_Error;
	}

	*ret_nLastNumber = pstProfile->nLastNum;

	HxLOG_Info("(-)\n");

	return eHOM_Result_Ok;
}



/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_Item_GetIsInsertPCR( const Handle_t hMediaProfileItem, HBOOL *ret_bIsInsertPCR)
{
	PL_MediaProfile_t		*pstProfile = NULL;

	HxLOG_Debug("(+)\n");

	pstProfile = _plmediaprofile_GetProfile(hMediaProfileItem);
	if(NULL == pstProfile)
	{
		HxLOG_Error("Get Error (-)\n");
		return eHOM_Result_Error;
	}

	*ret_bIsInsertPCR = pstProfile->bIsInsertPCR;

	HxLOG_Print("*ret_bIsInsertPCR (%d) (-)\n", *ret_bIsInsertPCR);

	return eHOM_Result_Ok;
}


/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_Item_GetBitrateM3U8WaitChunkCount( const Handle_t hMediaProfileItem, HINT32 *ret_wait_chunk_count)
{
	PL_MediaProfile_t		*pstProfile = NULL;

	HxLOG_Debug("(+)\n");

	pstProfile = _plmediaprofile_GetProfile(hMediaProfileItem);
	if(NULL == pstProfile)
	{
		HxLOG_Error("Get Error (-)\n");
		return eHOM_Result_Error;
	}

	*ret_wait_chunk_count = pstProfile->nBitrateM3U8ChunkWaitCnt;

	HxLOG_Print("*ret_wait_chunk_count (%d) (-)\n", *ret_wait_chunk_count);

	return eHOM_Result_Ok;
}

/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_Item_GetTSWaitChunkTime( const Handle_t hMediaProfileItem, HINT32 *ret_wait_chunk_time)
{
	PL_MediaProfile_t		*pstProfile = NULL;

	HxLOG_Debug("(+)\n");

	pstProfile = _plmediaprofile_GetProfile(hMediaProfileItem);
	if(NULL == pstProfile)
	{
		HxLOG_Error("Get Error (-)\n");
		return eHOM_Result_Error;
	}

	*ret_wait_chunk_time = pstProfile->nTsChunkWaitTime;

	HxLOG_Print("*ret_wait_chunk_count (%d) (-)\n", *ret_wait_chunk_time);

	return eHOM_Result_Ok;
}

/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_Item_GetM3U8PrintChunkTime( const Handle_t hMediaProfileItem, HINT32 *ret_m3u8_print_time)
{
	PL_MediaProfile_t		*pstProfile = NULL;

	HxLOG_Debug("(+)\n");

	pstProfile = _plmediaprofile_GetProfile(hMediaProfileItem);
	if(NULL == pstProfile)
	{
		HxLOG_Error("Get Error (-)\n");
		return eHOM_Result_Error;
	}

	*ret_m3u8_print_time = pstProfile->nM3U8PrintChunkTime;

	HxLOG_Print("*ret_wait_chunk_count (%d) (-)\n", *ret_m3u8_print_time);

	return eHOM_Result_Ok;
}

/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_Item_GetGapChunkCount( const Handle_t hMediaProfileItem, HINT32 *ret_nGapChunkCount)
{
	PL_MediaProfile_t		*pstProfile = NULL;

	HxLOG_Debug("(+)\n");

	pstProfile = _plmediaprofile_GetProfile(hMediaProfileItem);
	if(NULL == pstProfile)
	{
		HxLOG_Error("Get Error (-)\n");
		return eHOM_Result_Error;
	}

	*ret_nGapChunkCount = pstProfile->nGapChunkCount;

	HxLOG_Print("*ret_nGapChunkCount (%d) (-)\n", *ret_nGapChunkCount);

	return eHOM_Result_Ok;
}


/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_Item_GetVideoChunkSize( const Handle_t hMediaProfileItem, HINT32 *ret_nVideoChunkMultiply)
{
	PL_MediaProfile_t		*pstProfile = NULL;

	HxLOG_Debug("(+)\n");

	pstProfile = _plmediaprofile_GetProfile(hMediaProfileItem);
	if(NULL == pstProfile)
	{
		HxLOG_Error("Get Error (-)\n");
		return eHOM_Result_Error;
	}

	*ret_nVideoChunkMultiply = pstProfile->nVideoChunkMultiply;

	HxLOG_Print("*ret_nVideoChunkMultiply(%d) (-)\n", *ret_nVideoChunkMultiply);

	return eHOM_Result_Ok;
}


/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_Item_GetAudioChunkSize( const Handle_t hMediaProfileItem, HINT32 *ret_nAudioChunkMultiply)
{
	PL_MediaProfile_t		*pstProfile = NULL;

	HxLOG_Debug("(+)\n");

	pstProfile = _plmediaprofile_GetProfile(hMediaProfileItem);
	if(NULL == pstProfile)
	{
		HxLOG_Error("Get Error (-)\n");
		return eHOM_Result_Error;
	}

	*ret_nAudioChunkMultiply = pstProfile->nAudioChunkMultiply;

	HxLOG_Print("*ret_nAudioChunkMultiply(%d) (-)\n", *ret_nAudioChunkMultiply);

	return eHOM_Result_Ok;
}


/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_Item_GetList( const Handle_t hMediaProfileItem , HxList_t **ret_list )
{
	PL_MediaProfile_t		*pstProfile = NULL;

	HxLOG_Debug("(+)\n");

	pstProfile = _plmediaprofile_GetProfile(hMediaProfileItem);
	if(NULL == pstProfile)
	{
		HxLOG_Error("Get Error (-)\n");
		return eHOM_Result_Error;
	}

	*ret_list  = pstProfile->pItemTableList;

	HxLOG_Info("(-)\n");

	return eHOM_Result_Ok;

}

/**
*
*
*
* @param
* @return
*/
HOM_Result_e	PL_MediaProfile_Item_GetCount( const Handle_t hMediaProfileItem, HINT32 *ret_count)
{
	PL_MediaProfile_t		*pstProfile = NULL;

	HxLOG_Debug("(+)\n");

	pstProfile = _plmediaprofile_GetProfile(hMediaProfileItem);
	if(NULL == pstProfile)
	{
		HxLOG_Error("Get Error (-)\n");
		return eHOM_Result_Error;
	}

	*ret_count = HLIB_LIST_Length(pstProfile->pItemTableList);

	HxLOG_Info("(-)\n");

	return eHOM_Result_Ok;
}


/**********************************************************************************************************/
#define ____DEBUG_FUNCTIONS_______________________
/**********************************************************************************************************/


//#if defined(CONFIG_DEBUG)
//for debug
STATIC HBOOL	_plmediaprofile_ItemDump (void * data,void * userdata)
{
	PLMediaProfile_Item_t	*item = (PLMediaProfile_Item_t	*)data;

	HLIB_CMD_Printf(
		"\n\titem (0x%x) %x -  %s, %s, %s, %s, %d, %d, %d, %d, %d, %s, %d, %d\r\n"
		, (HUINT32)item, item->nIndex
		, _plmediaprofile_VideoCodecToAscii(item->eVideoCodec)
		, _plmediaprofile_VidoeEncCodecLevelToAscii(item->eVideoCodecLevel)
		, _plmediaprofile_VidoeEncCodecProfileToAscii(item->eVideoCodecProfile)
		, ((item->bInterlaceMode == TRUE) ? "I" : "P")
		, item->nAudioBitrateKbps
		, item->nVideoBitrateKbps
		, item->nFrameRate
		, item->nScreenWidth
		, item->nScreenHeight
		, _plmediaprofile_VideoAspectRatioToAscii(item->eVideoAspectRatio)
		, item->eVideoCodecProfile
		, item->eVideoCodecLevel);

	(void)userdata;

	return FALSE;
}

STATIC HBOOL	_plmediaprofile_ItemFullNameDump (void * data,void * userdata)
{
	PLMediaProfile_Item_t	*item = (PLMediaProfile_Item_t	*)data;

	HLIB_CMD_Printf("\titem (0x%x)\r\n", (HUINT32)item);
	if(item)
	{
		HLIB_CMD_Printf("\tindex : %3d\r\n", item->nIndex);
		HLIB_CMD_Printf("\teVideoCodec      : %s\r\n", _plmediaprofile_VideoCodecToAscii(item->eVideoCodec));
		HLIB_CMD_Printf("\teAudioCodec      : %s\r\n", _plmediaprofile_AudioCodecToAscii(item->eAudioCodec));
		HLIB_CMD_Printf("\taudio_bitrate    : %dkbps\r\n", item->nAudioBitrateKbps);
		HLIB_CMD_Printf("\tvideo_bitrate    : %dkbps\r\n", item->nVideoBitrateKbps);
		HLIB_CMD_Printf("\tfFrameRate       : %d\r\n", item->nFrameRate);
		HLIB_CMD_Printf("\tscreen_width     : %d\r\n", item->nScreenWidth);
		HLIB_CMD_Printf("\tscreen_height    : %d\r\n", item->nScreenHeight);
		HLIB_CMD_Printf("\tinterlace        : %d\r\n", ((item->bInterlaceMode == TRUE) ? "TRUE (I)" : "FALSE (P)"));
		HLIB_CMD_Printf("\tAspectRatio      : %s\r\n", _plmediaprofile_VideoAspectRatioToAscii(item->eVideoAspectRatio));
		HLIB_CMD_Printf("\tVideoCodecLevel  : %s\r\n", _plmediaprofile_VidoeEncCodecLevelToAscii(item->eVideoCodecLevel));
		HLIB_CMD_Printf("\tVideoCodecProfile: %s\r\n", _plmediaprofile_VidoeEncCodecProfileToAscii(item->eVideoCodecProfile));
	}

	(void)userdata;

	return FALSE;
}


void	PL_MediaProfile_ItemDump(void * data)
{
	_plmediaprofile_ItemDump(data, NULL);
}

void	PL_MediaProfile_ItemFullNameDump(void * data)
{
	_plmediaprofile_ItemFullNameDump(data, NULL);
}

void	PL_MediaProfile_ItemFullNameNumberDump(void * data)
{
	HINT32				num = *((HINT32 *)data);
	PLMediaProfile_Item_t	*item = NULL;

	item = _plmediaprofile_Item_Get(num);
	_plmediaprofile_ItemFullNameDump(item, NULL);
}



HOM_Result_e	PL_MediaProfile_Dump (void *pstProfile)
{
	PL_MediaProfile_t	*thiz = (PL_MediaProfile_t *)pstProfile;

	if(thiz == NULL)
	{
		return eHOM_Result_Error;
	}

	HLIB_CMD_Printf("============================================================\r\n");
	HLIB_CMD_Printf("thiz(0x%x)\n", (HUINT32)thiz);
	HLIB_CMD_Printf("  + device name    : %s\r\n", thiz->pszDeviceName);
	HLIB_CMD_Printf("  + nStartNum      : %x\r\n", (HINT32)thiz->nStartNum);
	HLIB_CMD_Printf("  + nLastNum       : %x\r\n", (HINT32)thiz->nLastNum);
	HLIB_CMD_Printf("  + bIsInsertPCR     : %s\r\n", (thiz->bIsInsertPCR ? "TRUE" : "FALSE"));
	HLIB_CMD_Printf("  + bitrate_wait   : %d\r\n", (HINT32)thiz->nBitrateM3U8ChunkWaitCnt);
	HLIB_CMD_Printf("  + ts_wait        : %d\r\n", (HINT32)thiz->nTsChunkWaitTime);
	HLIB_CMD_Printf("  + gap_chunk      : %d\r\n", (HINT32)thiz->nGapChunkCount);
	HLIB_CMD_Printf("  + v_chunk_mul    : %d\r\n", (HINT32)thiz->nVideoChunkMultiply);
	HLIB_CMD_Printf("  + a_chunk_mul    : %d\r\n", (HINT32)thiz->nAudioChunkMultiply);
	HLIB_CMD_Printf("  + m3u8_chunktime : %d\r\n", (HINT32)thiz->nM3U8PrintChunkTime);
	HLIB_CMD_Printf("  + bUseReference  : %s\r\n", (thiz->bUseReference ? "TRUE" : "FALSE"));
	HLIB_CMD_Printf("  + high_reference : %s\r\n", thiz->pHighRefName);
	HLIB_CMD_Printf("  + low_reference  : %s\r\n", thiz->pLowRefName);
	HLIB_CMD_Printf("  __item_______________________\r\n");
	HLIB_LIST_ForeachEx(thiz->pItemTableList, _plmediaprofile_ItemDump, thiz);
	HLIB_CMD_Printf("============================================================\r\n");

	return eHOM_Result_Ok;
}
//#endif


void	PL_MediaProfile_NotUsedFunc_RemoveWarning (void)
{
	(void)_plmediaprofile_FindProfileByProfileName;
	(void)_plmediaprofile_Get;
}

