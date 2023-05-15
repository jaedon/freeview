/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  umma_iface.c
	@brief

	Description:  									\n
	Module: Umma Interface Module			\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

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
 * (c) 2011-2013 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<dirent.h>

#include	<vkernel.h>
#include	<hlib.h>
#include	<dlib.h>
#include	<dapi.h>

#include	<uapi.h>
#include	<thapi.h>
#include	<umma_int.h>
#include	<umma_fm.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#if defined(CONFIG_PROD_OS_EMULATOR)
#define stat emu_stat
#endif
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#if 0 // move to umma_fhm.h
typedef struct
{
	HCHAR			szExtension[8];
	DxFileType_e	eType;
	HERROR			(*getFileInfo)(const HCHAR *pszFileName, void **ppvInfo, HINT32 *pnSize);
	HxFreeFunc_t	freeInfo;
} UMMA_FM_IFACE_t;
#endif

typedef struct  {
    HCHAR	szMagic[6];
    HCHAR	szWidth[2];
    HCHAR	szHeight[2];
    HCHAR	szMap;
    HCHAR	szColorPlanes;
    HCHAR	szReserved;
} UMMA_GIF_t;

typedef struct  {
    HCHAR	szMagic[2];
    HCHAR	szSize[4];
    HCHAR	szReserved[4];
    HCHAR	szOffset[4];
    HCHAR	szDibBytes[4];
    HCHAR	szWidth[4];
    HCHAR	szHeight[4];
    HCHAR	szColorPlanes[2];
    HCHAR	szBpp[2];
    HCHAR	szRawSize[4];
    HCHAR	szHhorRes[4];
    HCHAR	szVerRes[4];
    HCHAR	szColors[4];
    HCHAR	scImportant[4];
} UMMA_BMP_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static HxCBUF_t			s_pstCBuf = NULL;
static HxHASH_t	*s_pstIFaceTable = NULL;

static void		umma_fm_iface_FreeMem(void *p);

static HINT32	umma_fm_iface_CbHxAVInterrupt(void *user_data);
static HERROR	umma_fm_iface_GetPNGInfo(const HCHAR *pszFileName, void **ppvData, HINT32 *pnSize);
static HERROR	umma_fm_iface_GetMP3Info(const HCHAR *pszFileName, void **ppvData, HINT32 *pnSize);
static HERROR	umma_fm_iface_GetFlacInfo(const HCHAR *pszFileName, void **ppvData, HINT32 *pnSize);
static HERROR	umma_fm_iface_GetJPEGInfo(const HCHAR *pszFileName, void **ppvData, HINT32 *pnSize);
static HERROR	umma_fm_iface_GetAVInfo (const HCHAR *pszFileName, void **ppvData, HINT32 *pnSize);
static HERROR	umma_fm_iface_GetHJMInfo(const HCHAR *pszFileName, void **ppvData, HINT32 *pnSize);
static HERROR	umma_fm_iface_GetBMPInfo(const HCHAR *pszFileName, void **ppvData, HINT32 *pnSize);
static HERROR	umma_fm_iface_GetGIFInfo(const HCHAR *pszFileName, void **ppvData, HINT32 *pnSize);
static HERROR	umma_fm_iface_GetAudioInfo(const HCHAR *pszFileName, void **ppvData, HINT32 *pnSize);

//	UMMA가 관리 하는 확장자를 등록 --> Media 기반이므로 모든 데이타를 DB화 할 필요는 없다.
//	비교는 |ext|
static HCHAR	*s_pszIFaceFilter = "|3gp|3g2|mov|flv|swf|m2ts|rm|ts|mkv|avi|mp4|asf|wmv|tp|m2v|m4v|mts|mpg|vob|divx|"
									"mpeg|mpe|dat|ogm|ogg|rmvb|smi|sami|smil|srt|sub|txt|ass|ssa|psb|"
									"jpg|jpeg|bmp|gif|tif|"
									"mp3|wav|f4a|m4a|wma|"
#if defined(CONFIG_SUPPORT_UMMA_HRA)
									"flac|"
#endif
									"hjm|";


static UMMA_FM_IFACE_t		s_pstIFaceInfo[] =
{
	{	"mp3",	eDxFILETYPE_MUSIC,		umma_fm_iface_GetMP3Info,		umma_fm_iface_FreeMem	},
	{	"png",	eDxFILETYPE_PNG,			umma_fm_iface_GetPNGInfo,		umma_fm_iface_FreeMem	},
	{	"jpg",	eDxFILETYPE_EXIF,			umma_fm_iface_GetJPEGInfo, 	umma_fm_iface_FreeMem  },
	{	"jpeg",	eDxFILETYPE_EXIF,			umma_fm_iface_GetJPEGInfo, 	umma_fm_iface_FreeMem  },
	{	"bmp", 	eDxFILETYPE_EXIF,			umma_fm_iface_GetBMPInfo,	umma_fm_iface_FreeMem  },
	{	"gif", 	eDxFILETYPE_EXIF,			umma_fm_iface_GetGIFInfo,	umma_fm_iface_FreeMem  },

	{	"ts",		eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
	{	"tp",		eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
	{	"mkv",	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
	{	"avi",	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
	{	"mp4",	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
	{	"asf",	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
	{	"wmv",	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
	{	"m2v",	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
	{	"m4v",	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
	{	"mts",	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},

	{	"mpe",	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
	{	"mpg",	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
	{	"mpeg",	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
	{	"vob",	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
	{	"divx", 	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},

	{	"3gp",	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
	{	"3g2",	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
	{	"mov",	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
	{	"flv",		eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
	{	"swf",	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
	{	"m2ts",	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},

	{	"rm",	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
	{	"rmvb",	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
	{	"ogm",	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
	{	"ogg",	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
	{	"dat",	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
	{	"wav",	eDxFILETYPE_MUSIC,			umma_fm_iface_GetAudioInfo,		umma_fm_iface_FreeMem	}, /* Check hms-1000t branch's modification for wav */
	{	"wma",	eDxFILETYPE_MUSIC,			umma_fm_iface_GetAudioInfo,		umma_fm_iface_FreeMem	},
	{	"f4a",	eDxFILETYPE_MUSIC,			umma_fm_iface_GetAudioInfo,		umma_fm_iface_FreeMem	},
	{	"m4a",	eDxFILETYPE_MUSIC,			umma_fm_iface_GetAudioInfo,		umma_fm_iface_FreeMem	}, /* Check hms-1000t branch's modification for m4a */
#if defined(CONFIG_SUPPORT_UMMA_HRA)
	{	"flac",	eDxFILETYPE_MUSIC,			umma_fm_iface_GetFlacInfo,		umma_fm_iface_FreeMem	},
#endif

#if defined(CONFIG_PROD_HMX4KBBC)
	{	"txt",	eDxFILETYPE_AVSTREAM,		umma_fm_iface_GetAVInfo, 		umma_fm_iface_FreeMem	},
#endif

#if defined(CONFIG_FILEINFO_IFACE_NO_RECORDINGS)
#if !defined(NEW_OPERATION_LIST)
	{	"hjm",	eDxFILETYPE_RECORDINGS,		umma_fm_iface_GetHJMInfo, 		umma_fm_iface_FreeMem	}
#endif
#else
	{	"hjm",	eDxFILETYPE_RECORDINGS,		umma_fm_iface_GetHJMInfo, 		NULL			  	}
#endif
};

const static HCHAR		*s_szHjmExt = "hjm";
/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
static DxFileInfo_t*	umma_fm_iface_GetBuffer(HINT32 size)
{
#define	IFACE_BUFFER_SIZE		0x28000
	if (s_pstCBuf == NULL)
		s_pstCBuf = HLIB_CBUF_New(IFACE_BUFFER_SIZE);

	return (DxFileInfo_t*)HLIB_CBUF_GetBuffer(s_pstCBuf, size);
}

static void		umma_fm_iface_FreeMem(void *p)
{
	UMMA_Free(p);
}

static HERROR	umma_fm_iface_GetPNGInfo(const HCHAR *pszFileName, void **ppvData, HINT32 *pnSize)
{
	HxPNG_Info_t		*pstPngInfo;

	pstPngInfo = (HxPNG_Info_t*)UMMA_Calloc(sizeof(HxPNG_Info_t));

	*pnSize = sizeof(HxPNG_Info_t);
	*ppvData = pstPngInfo;

	if (HLIB_PNG_GetFileInfo(pszFileName, pstPngInfo) == ERR_OK)
		return ERR_OK;

	return ERR_OK;
}

static HERROR	umma_fm_iface_GetFlacInfo(const HCHAR *pszFileName, void **ppvData, HINT32 *pnSize)
{
	HxID3_MusicInfo_t		*pstMusicInfo = NULL;
	HxAV_AudioSteamInfo_t	*pstStreamInfo;
	HINT32	nRetryCount = 0;

	pstMusicInfo = (HxID3_MusicInfo_t*)UMMA_Calloc(sizeof(HxID3_MusicInfo_t));
	pstStreamInfo = (HxAV_AudioSteamInfo_t*)UMMA_Calloc(sizeof(HxAV_SteamInfo_t));

	*pnSize = sizeof(HxID3_MusicInfo_t);
	*ppvData = (void*)pstMusicInfo;

	HLIB_AV_GetAudioStreamInfo(pszFileName, pstStreamInfo, umma_fm_iface_CbHxAVInterrupt, &nRetryCount);
	//	Set dummy data.

	HxLOG_Debug("[UMMA_FM_IFACE] Flac : %s\n", pszFileName );
	if (ERR_OK != HLIB_ID3_GetFlacMusicInfo(pszFileName, pstMusicInfo))
	{
		HLIB_STD_StrNCpySafe(pstMusicInfo->szAlbum, "", HxID3_FIELD_SIZE_LONG);
		HLIB_STD_StrNCpySafe(pstMusicInfo->szArtist, "", HxID3_FIELD_SIZE_LONG);
		HLIB_STD_StrNCpySafe(pstMusicInfo->szTitle, "", HxID3_FIELD_SIZE_LONG);
		HLIB_STD_StrNCpySafe(pstMusicInfo->szYear, "", HxID3_FIELD_SIZE_SHORT);
		HLIB_STD_StrNCpySafe(pstMusicInfo->szGenre, "", HxID3_FIELD_SIZE_SHORT);
		HLIB_STD_StrNCpySafe(pstMusicInfo->szPublisher, "", HxID3_FIELD_SIZE_LONG);
	}

	pstMusicInfo->ulDuration = pstStreamInfo->nDuration;
	pstMusicInfo->nBitRate = pstStreamInfo->nBitRate;
	pstMusicInfo->nSampleRate = pstStreamInfo->stAudioInfo.nSampleRate;


	HxLOG_Print("[umma_fm_iface] flac : %s\n", pszFileName );
	HxLOG_Print(" duration  : %ld\n",	pstMusicInfo->ulDuration );
	HxLOG_Print(" bit rate  : %d\n",	pstMusicInfo->nBitRate );
	HxLOG_Print(" sample rate  : %d\n",	pstMusicInfo->nSampleRate );

	UMMA_Free( pstStreamInfo );

	return ERR_OK;
}

static HERROR	umma_fm_iface_GetJPEGInfo(const HCHAR *pszFileName, void **ppvData, HINT32 *pnSize)
{
	HxEXIF_Info_t	*pstExifInfo;

	pstExifInfo = UMMA_Calloc(sizeof(HxEXIF_Info_t));

	*pnSize = sizeof(HxEXIF_Info_t);
	*ppvData = pstExifInfo;

	if (HLIB_JPEG_GetFileInfo(pszFileName, &pstExifInfo->nWidth, &pstExifInfo->nHeight) == ERR_OK)
		return ERR_OK;

	UMMA_Free(pstExifInfo);
	*pnSize = 0;
	*ppvData = NULL;

	return ERR_FAIL;
}

static HERROR	umma_fm_iface_GetGIFInfo(const HCHAR *pszFileName, void **ppvData, HINT32 *pnSize)
{
	HxEXIF_Info_t	*pstExifInfo;
	FILE	*file = NULL;

	pstExifInfo = UMMA_Calloc(sizeof(HxEXIF_Info_t));
	*pnSize = sizeof(HxEXIF_Info_t);
	*ppvData = pstExifInfo;

	file = fopen(pszFileName, "rb");
	if (file)
	{
		UMMA_GIF_t	pszGIFBuf;
		HxSTD_MemSet(&pszGIFBuf, 0x00, sizeof(UMMA_GIF_t));

		if (fread((HCHAR *)&pszGIFBuf, 1, sizeof(UMMA_GIF_t), file) == sizeof(UMMA_GIF_t))
		{
			if(toupper(pszGIFBuf.szMagic[0]) == 'G'
				&& toupper(pszGIFBuf.szMagic[1]) == 'I'
				&& toupper(pszGIFBuf.szMagic[2]) == 'F')
			{
				pstExifInfo->nHeight = (HINT32)(pszGIFBuf.szHeight[1] & 0x000000FF)*0x100 +(HINT32)(pszGIFBuf.szHeight[0] & 0x000000FF);
				pstExifInfo->nWidth = (HINT32)(pszGIFBuf.szWidth[1] & 0x000000FF)*0x100 +(HINT32)(pszGIFBuf.szWidth[0] & 0x000000FF);
			}
		}
		fclose(file);
	}

	if(pstExifInfo->nHeight != 0 && pstExifInfo->nWidth != 0)
	{
		return ERR_OK;
	}

	UMMA_Free(pstExifInfo);
	*pnSize = 0;
	*ppvData = NULL;

	return ERR_FAIL;
}

static HERROR	umma_fm_iface_GetBMPInfo(const HCHAR *pszFileName, void **ppvData, HINT32 *pnSize)
{
	HxEXIF_Info_t	*pstExifInfo;
	FILE	*file = NULL;

	pstExifInfo = UMMA_Calloc(sizeof(HxEXIF_Info_t));
	*pnSize = sizeof(HxEXIF_Info_t);
	*ppvData = pstExifInfo;

	file = fopen(pszFileName, "rb");
	if (file)
	{
		UMMA_BMP_t  pszBMPBuf;
		HxSTD_MemSet(&pszBMPBuf, 0x00, sizeof(UMMA_BMP_t));

		if (fread((HCHAR *)&pszBMPBuf, 1, sizeof(UMMA_BMP_t), file) == sizeof(UMMA_BMP_t))
		{
			if(toupper(pszBMPBuf.szMagic[0]) == 'B'
				&& toupper(pszBMPBuf.szMagic[1]) == 'M')
			{
				pstExifInfo->nHeight = (HINT32)(pszBMPBuf.szHeight[1] & 0x000000FF)*0x100 +(HINT32)(pszBMPBuf.szHeight[0] & 0x000000FF);
				pstExifInfo->nWidth = (HINT32)(pszBMPBuf.szWidth[1] & 0x000000FF)*0x100 +(HINT32)(pszBMPBuf.szWidth[0] & 0x000000FF);
			}
		}
		fclose(file);
	}

	if(pstExifInfo->nHeight != 0 && pstExifInfo->nWidth != 0)
	{
		return ERR_OK;
	}
	UMMA_Free(pstExifInfo);
	*pnSize = 0;
	*ppvData = NULL;
	return ERR_FAIL;
}

static HBOOL	umma_fm_iface_IsHumaxTSFile(const HCHAR *pszFileName)
{
	static HCHAR	szFileName[1024];
	static HCHAR	*pszExtList[3] = { ".hjm", ".hjtsm", ".hmt" };

	HINT32	i, nLen;
	HCHAR	*pszName;
	HCHAR	*pszFileExt;

	pszName = HLIB_PATH_SplitExt(pszFileName, szFileName, 1024);
	nLen = HxSTD_StrLen(pszName);

	pszFileExt  = pszName + nLen;
	nLen = 1024 - nLen;

	for (i = 0; i < 3; i++)
	{
		HxSTD_StrNCpy(pszFileExt, pszExtList[i], nLen);
		if (HLIB_FILE_Exist(pszName))
			return TRUE;
	}

	return FALSE;
}

#define	MAX_PROBING_COUNT		3

static HINT32	umma_fm_iface_CbHxAVInterrupt(void *user_data)
{
	HINT32		*pnTryCount = (HINT32*)user_data;
	HINT32		nTry = *pnTryCount;

	++nTry;
	*pnTryCount = nTry;

	if (nTry > MAX_PROBING_COUNT)
		return TRUE;

	return FALSE;
}

static HERROR	umma_fm_iface_GetMP3Info(const HCHAR *pszFileName, void **ppvData, HINT32 *pnSize)
{
	HxID3_MusicInfo_t		*pstMusicInfo = NULL;

	pstMusicInfo = (HxID3_MusicInfo_t*)UMMA_Calloc(sizeof(HxID3_MusicInfo_t));

	*pnSize = sizeof(HxID3_MusicInfo_t);
	*ppvData = (void*)pstMusicInfo;

	if (HLIB_ID3_GetID3MusicInfo(pszFileName, pstMusicInfo) == ERR_OK)
	{
		return ERR_OK;
	}

	//	Set dummy data.

	HLIB_STD_StrNCpySafe(pstMusicInfo->szAlbum, "", HxID3_FIELD_SIZE_LONG);
	HLIB_STD_StrNCpySafe(pstMusicInfo->szArtist, "", HxID3_FIELD_SIZE_LONG);
	HLIB_STD_StrNCpySafe(pstMusicInfo->szTitle, "", HxID3_FIELD_SIZE_LONG);
	HLIB_STD_StrNCpySafe(pstMusicInfo->szYear, "", HxID3_FIELD_SIZE_SHORT);
	HLIB_STD_StrNCpySafe(pstMusicInfo->szGenre, "", HxID3_FIELD_SIZE_SHORT);
	HLIB_STD_StrNCpySafe(pstMusicInfo->szPublisher, "", HxID3_FIELD_SIZE_LONG);
	pstMusicInfo->ulDuration = 0;

	return ERR_OK;
}

static HERROR	umma_fm_iface_GetWavInfo(const HCHAR *pszFileName, void **ppvData, HINT32 *pnSize)
{
	HxID3_MusicInfo_t		*pstMusicInfo = NULL;
	HxAV_AudioSteamInfo_t	*pstStreamInfo = NULL;
	HINT32	nRetryCount = 0;

	pstMusicInfo = (HxID3_MusicInfo_t*)UMMA_Calloc(sizeof(HxID3_MusicInfo_t));
	pstStreamInfo = (HxAV_AudioSteamInfo_t*)UMMA_Calloc(sizeof(HxAV_AudioSteamInfo_t));

	if ((NULL == pstMusicInfo) || (NULL == pstStreamInfo))
	{
		HxLOG_Error("Memory allocation failed:\n");
		if (NULL != pstMusicInfo)			{ UMMA_Free(pstMusicInfo); }
		if (NULL != pstStreamInfo)			{ UMMA_Free(pstStreamInfo); }

		return ERR_FAIL;
	}

	*pnSize = sizeof(HxID3_MusicInfo_t);
	*ppvData = (void*)pstMusicInfo;

	HxLOG_Debug("[UMMA_FM_IFACE] wav : %s\n", pszFileName );
	HLIB_AV_GetAudioStreamInfo(pszFileName, pstStreamInfo, umma_fm_iface_CbHxAVInterrupt, &nRetryCount);

	pstMusicInfo->ulDuration = pstStreamInfo->nDuration;
	pstMusicInfo->nBitRate = pstStreamInfo->nBitRate;
	pstMusicInfo->nSampleRate = pstStreamInfo->stAudioInfo.nSampleRate;

	HxLOG_Print("[umma_fm_iface] wav : %s\n", pszFileName );
	HxLOG_Print(" duration  : %ld\n",	pstMusicInfo->ulDuration );
	HxLOG_Print(" bit rate  : %d\n",	pstMusicInfo->nBitRate );
	HxLOG_Print(" sample rate  : %d\n",	pstMusicInfo->nSampleRate );

	UMMA_Free( pstStreamInfo );

	return ERR_OK;
}

static HERROR	umma_fm_iface_GetAudioInfo(const HCHAR *pszFileName, void **ppvData, HINT32 *pnSize)
{
	HxID3_MusicInfo_t		*pstMusicInfo = NULL;
	HxAV_AudioSteamInfo_t		*pstStreamInfo = NULL; /* Changed from HxAV_SteamInfo_t */
	HINT32	nRetryCount = 0;

	pstMusicInfo = (HxID3_MusicInfo_t*)UMMA_Calloc(sizeof(HxID3_MusicInfo_t));
	if(pstMusicInfo == NULL)
		return ERR_FAIL;

	pstStreamInfo = (HxAV_AudioSteamInfo_t*)UMMA_Calloc(sizeof(HxAV_AudioSteamInfo_t));
	if(pstStreamInfo == NULL)
 	{
		UMMA_Free(pstMusicInfo);
		return ERR_FAIL;
	}

	*pnSize = sizeof(HxID3_MusicInfo_t);
	*ppvData = (void*)pstMusicInfo;

	HxLOG_Debug("[UMMA_FM_IFACE] audio : %s\n", pszFileName );
	HLIB_AV_GetAudioStreamInfo(pszFileName, pstStreamInfo, umma_fm_iface_CbHxAVInterrupt, &nRetryCount);

	pstMusicInfo->ulDuration = pstStreamInfo->nDuration;
	pstMusicInfo->nBitRate = pstStreamInfo->nBitRate;
	pstMusicInfo->nSampleRate = pstStreamInfo->stAudioInfo.nSampleRate;

	HxLOG_Print("[umma_fm_iface] audio : %s\n", pszFileName );
	HxLOG_Print(" duration  : %ld\n",	pstMusicInfo->ulDuration );
	HxLOG_Print(" bit rate  : %d\n",	pstMusicInfo->nBitRate );
	HxLOG_Print(" sample rate  : %d\n",	pstMusicInfo->nSampleRate );

	UMMA_Free( pstStreamInfo );

	return ERR_OK;
}

static HERROR	umma_fm_iface_GetAVInfo (const HCHAR *pszFileName, void **ppvData, HINT32 *pnSize)
{
	HxAV_SteamInfo_t	*pstStreamInfo;
	HINT32	nRetryCount;

	if (umma_fm_iface_IsHumaxTSFile(pszFileName) == TRUE)
		return ERR_FAIL;

	pstStreamInfo = (HxAV_SteamInfo_t*)UMMA_Calloc(sizeof(HxAV_SteamInfo_t));

	nRetryCount = 0;

	*pnSize  = sizeof(HxAV_SteamInfo_t);
	*ppvData = pstStreamInfo;

	HLIB_AV_GetVideoStreamInfo(pszFileName, pstStreamInfo, umma_fm_iface_CbHxAVInterrupt, &nRetryCount);

	return ERR_OK;	// 실패한 경우도 파일 리스트를 올려준다.
}

#if defined(CONFIG_FILEINFO_IFACE_NO_RECORDINGS)
static HERROR	umma_fm_iface_GetHJMInfo(const HCHAR *pszFileName, void **ppvData, HINT32 *pnSize)
{
#if defined(CONFIG_PROD_OS_EMULATOR)
	//TODO;
	return ERR_FAIL;
#else
	HCHAR	*pszHJMBuf = NULL;
	struct stat	fstat;
	FILE	*file = NULL;
	HERROR	 hResult = ERR_FAIL;

	*pnSize = 0;
	*ppvData = NULL;

	if (stat(pszFileName, &fstat) == 0)
	{
		HUINT32	ulSize;

		pszHJMBuf = UMMA_Calloc(fstat.st_size + 1);
		if (NULL == pszHJMBuf)
		{
			goto END_FUNC;
		}

		file = fopen(pszFileName, "rb");
		if (NULL == file)
		{
			goto END_FUNC;
		}

		{
			HINT32			 nSize = 0;
			DxRecListData_t *pstRecData = NULL;
			HERROR	hErr = ERR_FAIL;

			if (fread(pszHJMBuf, 1, fstat.st_size, file) == fstat.st_size)
			{
				pszHJMBuf[fstat.st_size] = '\0';
				if ((ulSize = HxSTD_StrLen(pszHJMBuf)) > 0)
				{
#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
					hErr = DLIB_Decode(DxNAMEOF(DxRecListData_t), 0, (const void*)pszHJMBuf
						, ulSize, (const void**)&pstRecData, &nSize);
#else
					hErr = DLIB_CreateDecodedData(DxNAMEOF(DxRecListData_t), 0, (const void*)pszHJMBuf
						, ulSize, (const void**)&pstRecData, &nSize);
#endif
					if ((ERR_OK == hErr) && (NULL != pstRecData) && (nSize > 0))
					{
						HUINT32			 ulUrlLen = HxSTD_StrLen(pstRecData->szUrl);
						HCHAR			*szUrl = (HCHAR *)UMMA_Malloc(ulUrlLen + 1);

						if (NULL != szUrl)
						{
							HxSTD_MemCopy(szUrl, pstRecData->szUrl, ulUrlLen);
							szUrl[ulUrlLen] = '\0';

							*pnSize = (HINT32)(ulUrlLen + 1);
							*ppvData = (void *)szUrl;

							hResult = ERR_OK;
						}

					}
					else
					{
						HxLOG_Error("Error HJM (%s), Content (%s) \n", pszFileName, pszHJMBuf);
					}

#if defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
					if (NULL != pstRecData)
					{
						(void)DLIB_DestroyDecodedData(DxNAMEOF(DxRecListData_t), (const void *)pstRecData);
					}
#endif
				}
			}
		}
	}
END_FUNC:
	if (pszHJMBuf)
	{
		UMMA_Free(pszHJMBuf);
	}

	if (file)
	{
		fclose(file);
	}

	return hResult;
#endif /* CONFIG_PROD_OS_EMULATOR */
}
#else
static HERROR	umma_fm_iface_GetHJMInfo(const HCHAR *pszFileName, void **ppvData, HINT32 *pnSize)
{
#if defined(CONFIG_PROD_OS_EMULATOR)
	//TODO;
	return ERR_FAIL;
#else
	HCHAR	*pszHJMBuf = NULL;
	struct stat	fstat;

	if (stat(pszFileName, &fstat) == 0)
	{
		FILE	*file = NULL;
		HUINT32	ulSize;

		pszHJMBuf = UMMA_Calloc(fstat.st_size + 1);
		file = fopen(pszFileName, "rb");
		if (file)
		{
			HERROR	hErr = ERR_FAIL;

			if (fread(pszHJMBuf, 1, fstat.st_size, file) == fstat.st_size)
			{
				pszHJMBuf[fstat.st_size] = '\0';
				if ((ulSize = HxSTD_StrLen(pszHJMBuf)) > 0)
				{
#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
					hErr = DLIB_Decode(DxNAMEOF(DxRecListData_t), 0, (const void*)pszHJMBuf
						, ulSize, (const void**)ppvData, &nSize);
#else
					hErr = DLIB_CreateDecodedData(DxNAMEOF(DxRecListData_t), 0, (const void*)pszHJMBuf
						, ulSize, (const void**)ppvData, &nSize);
#endif
					if (ERR_OK != hErr)
					{
						HxLOG_Error("Error HJM (%s), Content (%s) \n", pszFileName, pszHJMBuf);
						*pnSize = 0;
						*ppvData = NULL;
					}
				}
			}

			UMMA_Free(pszHJMBuf);
			fclose(file);
			return hErr;
		}

		if (pszHJMBuf)
			UMMA_Free(pszHJMBuf);
	}

	return ERR_FAIL;
#endif /* CONFIG_PROD_OS_EMULATOR */
}
#endif

#define	NUMBER_OF_IFACEINFO		(sizeof(s_pstIFaceInfo) / sizeof(s_pstIFaceInfo[0]))

static void		umma_fm_iface_InitIFaceInfo(void)
{
	HINT32	i;

	s_pstIFaceTable = HLIB_HASH_NewFull(HLIB_HASH_StrHash, HLIB_HASH_StrEqual, NULL, NULL);
	for (i = 0; i < NUMBER_OF_IFACEINFO; i++)
	{
		HLIB_HASH_Insert(s_pstIFaceTable, s_pstIFaceInfo[i].szExtension, (void*)&s_pstIFaceInfo[i]);
	}
}

static UMMA_FM_IFACE_t*	umma_fm_iface_GetIFaceInfo(const HCHAR *pszExt)
{
	if (s_pstIFaceTable == NULL)
		umma_fm_iface_InitIFaceInfo();

	return HLIB_HASH_Lookup(s_pstIFaceTable, (void*)pszExt);
}

static HCHAR*	umma_fm_iface_GetFileExtension(const HCHAR *pszName, HCHAR *pszExtension, HINT32 n)
{
	HCHAR	*pszExt;

	pszExt = HLIB_PATH_Extension(pszName);
	if (pszExt == NULL)
		pszExtension[0] = '\0';
	else
		HLIB_STD_StrNCpySafe(pszExtension, pszExt, n);

	return pszExtension;
}

static HERROR	umma_fm_iface_SetDataToDaMa(const HCHAR *name, HUID uid, const DxFileInfo_t *data, HUINT32 size, HUINT32 term)
{
	HCHAR	szExt[16];
	HCHAR	*pszExt;

	if (s_pszIFaceFilter == NULL)
		DAPI_LazySet(name, uid, (const void*)data, size, term);
	else
	{
		pszExt = HLIB_PATH_Extension(data->szFileName);
		if (HxSTD_StrEmpty(pszExt) == TRUE)
			return ERR_OK;

		HxSTD_snprintf(szExt, 15, "|%s|", pszExt);
		szExt[15] = '\0';
		if (HLIB_STD_StrCaseStr(s_pszIFaceFilter, szExt))
			DAPI_LazySet(name, uid, (const void*)data, size, term);
		}

	return ERR_OK;
}

static HERROR	umma_fm_iface_SetBaseInfo(DxFileInfo_t *pstFileInfo, struct stat *fstat, const HCHAR *pszFullName)
{
	HCHAR	szExt[8], *pszExt;
	HCHAR	*pszTempName, *p;

	pszExt = umma_fm_iface_GetFileExtension(pszFullName, szExt, 8);
	HLIB_STD_StrLower(pszExt);

	pstFileInfo->uid = UMMA_FM_IFACE_GetNewUid(fstat);

	pstFileInfo->eInterface = eDxINTERFACE_UNKNOWN;
	DLIB_FILEINFO_ConvertStatToDxStat(fstat, &pstFileInfo->st_stat);

	pszTempName = HLIB_CBUF_GetBuffer(s_pstCBuf, 1024);
	HLIB_STD_StrNCpySafe(pszTempName, pszFullName, 1024);
	p = HLIB_PATH_BasePath(pszTempName);
	HLIB_STD_StrNCpySafe(pstFileInfo->szOnedepth, p, DLIB_FILEINFO_ONEDEPTH_LEN);

//	HLIB_STD_StrNCpySafe(pstFileInfo->szExt, pszExt, MAX_HXDIR_EXTNAME_LENGTH);
	HLIB_STD_StrNCpySafe(pstFileInfo->szFileName, /*HLIB_PATH_FileName*/(pszFullName), DLIB_FILEINFO_NAME_LEN);

	return ERR_OK;
}

HUINT64	UMMA_FM_IFACE_GetNewUid(struct stat *fstat)
{
	return HLIB_DIR_GetFileUniqueId(NULL, fstat);
}

HERROR	UMMA_FM_IFACE_AddFileInfoWoDetail(const void *pvUser, const HCHAR *pszNewFullPath, struct stat *fstat)
{
	DxFileInfo_t	*pstFileInfo;
	UMMA_FM_IFACE_t	*pstIFace;
	UMMA_MOUNTINFO_t	*pstMountInfo = (UMMA_MOUNTINFO_t*)pvUser;

	if (pstMountInfo == NULL)
	{
		HxLOG_Error("Error> wrong mount arg\n");
		return ERR_FAIL;
	}

	if (S_ISDIR(fstat->st_mode))
	{
		//	ignore delayed Recording directory
		HCHAR	*pszExt = NULL;
		pszExt = HLIB_PATH_Extension(pszNewFullPath);
		if ( pszExt && !HxSTD_StrCmp( "ts", pszExt ) )
			return ERR_OK;

		pstFileInfo = umma_fm_iface_GetBuffer(sizeof(DxFileInfo_t));
		HxSTD_MemSet(pstFileInfo, 0, sizeof(DxFileInfo_t));
		umma_fm_iface_SetBaseInfo(pstFileInfo, fstat, pszNewFullPath);
		pstFileInfo->eDataType = eDxFILETYPE_DIRECTORY;
		pstFileInfo->eInterface = pstMountInfo->eInterface;

		DAPI_LazySet( DxNAMEOF(DxFileInfo_t), pstFileInfo->uid, (const void*)pstFileInfo,  sizeof(DxFileInfo_t) + pstFileInfo->size, 100);
		return ERR_OK;
	}

	pstFileInfo = umma_fm_iface_GetBuffer(sizeof(DxFileInfo_t));
	HxSTD_MemSet(pstFileInfo, 0, sizeof(DxFileInfo_t));

	if (umma_fm_iface_SetBaseInfo(pstFileInfo, fstat, pszNewFullPath) == ERR_OK)
	{
		HCHAR	*pszExt;

		pszExt = HLIB_PATH_Extension(pszNewFullPath);
		if (!HxSTD_StrEmpty(pszExt))
		{
			if ((pstIFace = umma_fm_iface_GetIFaceInfo(pszExt)))
				pstFileInfo->eDataType = pstIFace->eType;
		}
	}

	pstFileInfo->eInterface = pstMountInfo->eInterface;

	umma_fm_iface_SetDataToDaMa(DxNAMEOF(DxFileInfo_t), pstFileInfo->uid, pstFileInfo, sizeof(DxFileInfo_t) + pstFileInfo->size, 100);


	return ERR_OK;
}

HERROR	UMMA_FM_IFACE_AddFileInfoDirect(void *pvInfo)
{
	DxFileInfo_t	*pstFileInfo = (DxFileInfo_t*)pvInfo;

	return umma_fm_iface_SetDataToDaMa(DxNAMEOF(DxFileInfo_t), pstFileInfo->uid, pstFileInfo, sizeof(DxFileInfo_t) + pstFileInfo->size, 100);
}

//bThumbnailUrgent : Thumbnail 생성 요청 API에 들어간 JOB의 우선 순위. TRUE면 THMA에서 더 높은 우선 순위로 동작함
HERROR	UMMA_FM_IFACE_AddFileInfo(const void *pvUser, const HCHAR *pszSourceName, HBOOL bThumbnailUrgent)
{
#if defined(CONFIG_PROD_OS_EMULATOR)
	//TODO;
	return ERR_OK;
#else
	HCHAR	szExt[8], *pszExt;
	void	*pvData;
	HINT32	nSize;
	UMMA_FM_IFACE_t	*pstIFaceInfo;

	DxFileInfo_t	*pstFileInfo;
	struct stat		fstat;

	UMMA_MOUNTINFO_t	*pstMountInfo = (UMMA_MOUNTINFO_t*)pvUser;

	if (stat(pszSourceName, &fstat))
		return ERR_FAIL;

	//	if target is directory, ignore it.
	if (S_ISDIR(fstat.st_mode))
	{
		HCHAR	*pszExt = NULL;
		//	ignore delayed Recording directory
		pszExt = HLIB_PATH_Extension(pszSourceName);
		if ( pszExt && !HxSTD_StrCmp( "ts", pszExt ) )
			return ERR_OK;

		pstFileInfo = umma_fm_iface_GetBuffer(sizeof(DxFileInfo_t) + 1);
		HxSTD_MemSet(pstFileInfo, 0, sizeof(DxFileInfo_t) + 1);
		umma_fm_iface_SetBaseInfo(pstFileInfo, &fstat, pszSourceName);
		pstFileInfo->eDataType = eDxFILETYPE_DIRECTORY;

		pstFileInfo->eInterface = pstMountInfo->eInterface;

		DAPI_LazySet( DxNAMEOF(DxFileInfo_t), pstFileInfo->uid, (const void*)pstFileInfo,  sizeof(DxFileInfo_t) + pstFileInfo->size, 100);
		return ERR_OK;
	}

	//	두가지 방법이 존재.
	//	Umma ScanResult에서 Cache체크를 하는 방법
	//	GetFileinfo내에서 Cache체크하는 방법
	//	Default Scan의 효율을 위해 GetFileInfo내에서 Cache Check하는 방법을 선택함.
	if (pstMountInfo)
	{
		pszExt = umma_fm_iface_GetFileExtension(pszSourceName, szExt, 8);
		HLIB_STD_StrLower(pszExt);
		if (HxSTD_StrNCmp(pszExt, s_szHjmExt, HxSTD_StrLen(s_szHjmExt)) != 0)
		{
			pstFileInfo = (DxFileInfo_t*)UMMA_FM_CACHE_GetFileInfo(pstMountInfo, pszSourceName, &fstat);
			if (pstFileInfo && (pstFileInfo->st_stat.ullMTime == fstat.st_mtime))
			{
				umma_fm_iface_SetDataToDaMa(DxNAMEOF(DxFileInfo_t), pstFileInfo->uid, pstFileInfo, sizeof(DxFileInfo_t) + pstFileInfo->size, 100);
				return ERR_OK;
			}
		}
		else
		{
			HxLOG_Debug("Don't use cache for HJM. \n");
		}
	}

	pszExt = umma_fm_iface_GetFileExtension(pszSourceName, szExt, 8);
	HLIB_STD_StrLower(pszExt);

	pstIFaceInfo = umma_fm_iface_GetIFaceInfo(pszExt);
	if (pstIFaceInfo)
	{
		if (pstIFaceInfo->getFileInfo(pszSourceName, &pvData, &nSize) == ERR_OK)
		{

			pstFileInfo = umma_fm_iface_GetBuffer(sizeof(DxFileInfo_t) + nSize + 1);
			HxSTD_MemSet(pstFileInfo, 0, sizeof(DxFileInfo_t) + nSize + 1);

			umma_fm_iface_SetBaseInfo(pstFileInfo, &fstat, pszSourceName);
			if(pstMountInfo)
				pstFileInfo->eInterface = pstMountInfo->eInterface;
			pstFileInfo->size = nSize;
			pstFileInfo->eDataType = pstIFaceInfo->eType;
			HxSTD_MemCopy(pstFileInfo->data, pvData, nSize);
			if (pstIFaceInfo->freeInfo)
				pstIFaceInfo->freeInfo(pvData);

			if (HxSTD_StrNCmp(pszExt, s_szHjmExt, HxSTD_StrLen(s_szHjmExt)) != 0)
			{
				UMMA_FM_CACHE_AddFileInfo(pstFileInfo, sizeof(DxFileInfo_t) + pstFileInfo->size);
			}
			umma_fm_iface_SetDataToDaMa(DxNAMEOF(DxFileInfo_t), pstFileInfo->uid, pstFileInfo, sizeof(DxFileInfo_t) + pstFileInfo->size, 100);

			 //Thumbnail 생성 요청
			switch (pstFileInfo->eDataType)
			{
				case	eDxFILETYPE_MUSIC:
				case	eDxFILETYPE_EXIF:
				case	eDxFILETYPE_PNG:
					THAPI_AddThumbnailCache((HCHAR*)pszSourceName, 0, NULL, bThumbnailUrgent);
					break;
				case eDxFILETYPE_AVSTREAM:
				{
					HxAV_SteamInfo_t *pstAVInfo = (HxAV_SteamInfo_t*)pstFileInfo->data;
					HUINT32 ulTime;

					ulTime = (0 < pstAVInfo->nDuration && pstAVInfo->nDuration < UMMA_FM_THUMBNAIL_EXTRACT_TIME) ? pstAVInfo->nDuration : UMMA_FM_THUMBNAIL_EXTRACT_TIME;
					THAPI_AddThumbnailCache((HCHAR*)pszSourceName, ulTime, NULL, bThumbnailUrgent);
				}
				default :
					break;
			}
		}
	}

	return ERR_OK;
#endif/* CONFIG_PROD_OS_EMULATOR */
}

/* for Direct Browsing TEST */
UMMA_FM_IFACE_t*	UMMA_FM_IFACE_GetIfaceInfo(const HCHAR *pszExt)
{
	return umma_fm_iface_GetIFaceInfo(pszExt);
}


