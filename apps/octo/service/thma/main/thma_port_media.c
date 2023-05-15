/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding ?» »ç¿ëÇÏ´Â source file ?Ô´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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

/**************************************************************************************************/
#define _________THMA_Private_Include_________________________________________________
/**************************************************************************************************/
#include <vkernel.h>
#include <hlib.h>
#include "thma_int.h"

#define	THMA_THUMBNAIL_LARGE

/**************************************************************************************************/
#define _________THMA_Private_Macro_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THMA_Private_Struct_________________________________________________

/**************************************************************************************************/
#define _________THMA_Private_Value_________________________________________________
/**************************************************************************************************/
#ifdef	THMA_THUMBNAIL_LARGE
static HCHAR	s_szLargeImageCacheName[1024];
#endif
/**************************************************************************************************/
#define _________THMA_Private_Prototype_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THMA_Private_Init__________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THMA_Private_Destroy__________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THMA_Private_Process__________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________THMA_Private_Proc_MSG_RUNNIG__________________________________________________________
/**************************************************************************************************/
#if 1
#define	THMA_OUTPUT_RES_WIDTH		960
#define	THMA_OUTPUT_RES_HEIGHT		540
#else
#define	THMA_OUTPUT_RES_WIDTH		640
#define	THMA_OUTPUT_RES_HEIGHT		360
#endif

#define	THMA_OUTPUT_LIMIT_WIDTH		1280
#define	THMA_OUTPUT_LIMIT_HEIGHT	720
#define	THMA_OUTPUT_LIMIT_SIZE		(THMA_OUTPUT_LIMIT_WIDTH * THMA_OUTPUT_LIMIT_HEIGHT * 4)

static HBOOL	thma_port_recalcRect2(HINT32 *pnWidth, HINT32 *pnHeight, HINT32 nOrgWidth, HINT32 nOrgHeight)
{
	HINT32	nSize;
	HINT32	width, height;

	if ((nOrgWidth < THMA_THUMBNAIL_WIDTH) && (nOrgHeight < THMA_THUMBNAIL_HEIGHT))
	{
		*pnWidth = nOrgWidth;
		*pnHeight = nOrgHeight;
		return FALSE;
	}

	nSize = nOrgWidth * nOrgHeight * 4;
	if (nSize < THMA_OUTPUT_LIMIT_SIZE)
	{
		height = THMA_THUMBNAIL_HEIGHT;
		width  = nOrgWidth * THMA_THUMBNAIL_HEIGHT / nOrgHeight;
		if (width > THMA_THUMBNAIL_WIDTH)
		{
			height = nOrgHeight * THMA_THUMBNAIL_WIDTH / nOrgWidth;
			width = THMA_THUMBNAIL_WIDTH;
		}
	} else
	{
		height = THMA_OUTPUT_RES_HEIGHT;
		width  = nOrgWidth * THMA_OUTPUT_RES_HEIGHT / nOrgHeight;
		if (width > THMA_OUTPUT_RES_WIDTH)
		{
			height = nOrgHeight * THMA_OUTPUT_RES_WIDTH / nOrgWidth;
			width = THMA_OUTPUT_RES_WIDTH;
		}
	}

	if (height == 0)
		height = nOrgHeight;
	if (width == 0)
		width = nOrgWidth;

	*pnWidth = width;
	*pnHeight = height;

	return TRUE;
}

static HBOOL	thma_port_recalcRect(HINT32 *pnWidth, HINT32 *pnHeight, HINT32 nOrgWidth, HINT32 nOrgHeight)
{
	HINT32	width, height;

	if ((nOrgWidth < THMA_THUMBNAIL_WIDTH) && (nOrgHeight < THMA_THUMBNAIL_HEIGHT))
	{
		*pnWidth = nOrgWidth;
		*pnHeight = nOrgHeight;
		return FALSE;
	}

	height = THMA_THUMBNAIL_HEIGHT;
	width = nOrgWidth * THMA_THUMBNAIL_HEIGHT / nOrgHeight;
	if (width > THMA_THUMBNAIL_WIDTH)
	{
		height = nOrgHeight * THMA_THUMBNAIL_WIDTH / nOrgWidth;
		width = THMA_THUMBNAIL_WIDTH;
	}

	if (height == 0)
	{
		height = nOrgHeight;
		HxLOG_Debug("[%s] force to resize Org Height [%d] -> Resized Height [%d] \n", __FUNCTION__, nOrgHeight, height);
	}
	if (width == 0)
	{
		width = nOrgWidth;
		HxLOG_Debug("[%s] force to resize Org Width [%d] -> Resized Width [%d] \n", __FUNCTION__, nOrgWidth, width);
	}

	HxLOG_Debug("[%s] Original X x H = %d x %d \n", __FUNCTION__, nOrgWidth, nOrgHeight);
	HxLOG_Debug("[%s] New      X x H = %d x %d \n", __FUNCTION__, width, height);

	*pnWidth = width;
	*pnHeight = height;

	return TRUE;
}

static HBOOL	thma_port_checkImageSize(HUINT32 ulWidth, HUINT32 ulHeight)
{
	return TRUE;
}


static HBOOL thma_port_existmeta(const HCHAR* pszFileName)
{
	HINT32 	 i;
	HCHAR	 szRecMetalist[3][8] = { "hjm","hjtsm","hmt"};
	HCHAR	 *p,*pszExt,*pszBuffer;

	pszExt =HxSTD_StrRChar(pszFileName,'.');
    pszExt += 1;

	if(HLIB_STD_StrCaseCmp("ts",pszExt) ==0 ) // ts Ȯ���� �̶��
	{
		pszBuffer = TH_MEM_StrDup(pszFileName);
		p =HxSTD_StrRChar(pszBuffer,'.');

		if(p!=NULL)
			*p = 0; //  /mnt/hd2/aaa.ts  ->  /mnt/hd2/aaa ����

		for(i=0; i< 3;i++)
		{
			HCHAR szMetaName[768]={0,};
			HxSTD_snprintf(szMetaName, 768, "%s.%s", pszBuffer, szRecMetalist[i]);

			if(HLIB_FILE_Exist(szMetaName))
			{
				HxLOG_Debug("HLIB_FILE_Exist(%s) \n",szMetaName);
				TH_MEM_Free(pszBuffer);
				return TRUE;
			}
		}
		TH_MEM_Free(pszBuffer);
	}
	return FALSE;
}



/**************************************************************************************************/
#define _________THMA_Public_Proc____________________________________________________
/**************************************************************************************************/
extern HERROR	THMA_PORT_ExtractMP3(const HCHAR *pszURL, const HCHAR *pszCacheURL, HUINT32 ulTime);
extern HERROR	THMA_PORT_ExtractFLAC(const HCHAR *pszURL, const HCHAR *pszCacheURL, HUINT32 ulTime);
extern HERROR	THMA_PORT_ExtractJPG(const HCHAR *pszURL, const HCHAR *pszCacheURL, HUINT32 ulTime);
extern HERROR	THMA_PORT_ExtractTS (const HCHAR *pszURL, const HCHAR *pszCacheURL, HUINT32 ulTime);
extern HERROR	THMA_PORT_ExtractAV (const HCHAR *pszURL, const HCHAR *pszCacheURL, HUINT32 ulTime);
extern HERROR	THMA_PORT_ExtractAVSequential (const HCHAR *pszURL, const HCHAR *pszCacheURL, HUINT32 ulStepTime);

/**************************************************************************************************/
#define _________THMA_Public_Functions__________________________________________________________
/**************************************************************************************************/
HERROR	THMA_PORT_MEDIA_Init(HxHASH_t *pstHashTable)
{
	HLIB_HASH_Insert(pstHashTable, (void*)"mp3", (void*)THMA_PORT_ExtractMP3);
	HLIB_HASH_Insert(pstHashTable, (void*)"flac", (void*)THMA_PORT_ExtractFLAC);
	HLIB_HASH_Insert(pstHashTable, (void*)"jpg", (void*)THMA_PORT_ExtractJPG);
	HLIB_HASH_Insert(pstHashTable, (void*)"jpeg",(void*)THMA_PORT_ExtractJPG);
	HLIB_HASH_Insert(pstHashTable, (void*)"png", (void*)THMA_PORT_ExtractJPG);

	HLIB_HASH_Insert(pstHashTable, (void*)"ts",  (void*)THMA_PORT_ExtractAV);
	HLIB_HASH_Insert(pstHashTable, (void*)"tp",  (void*)THMA_PORT_ExtractAV);
	HLIB_HASH_Insert(pstHashTable, (void*)"avi", (void*)THMA_PORT_ExtractAV);
	HLIB_HASH_Insert(pstHashTable, (void*)"mp4", (void*)THMA_PORT_ExtractAV);
	HLIB_HASH_Insert(pstHashTable, (void*)"mkv", (void*)THMA_PORT_ExtractAV);
	HLIB_HASH_Insert(pstHashTable, (void*)"asf", (void*)THMA_PORT_ExtractAV);
	HLIB_HASH_Insert(pstHashTable, (void*)"wmv", (void*)THMA_PORT_ExtractAV);
	HLIB_HASH_Insert(pstHashTable, (void*)"m4v", (void*)THMA_PORT_ExtractAV);
	HLIB_HASH_Insert(pstHashTable, (void*)"m2v", (void*)THMA_PORT_ExtractAV);
	HLIB_HASH_Insert(pstHashTable, (void*)"mts", (void*)THMA_PORT_ExtractAV);

	HLIB_HASH_Insert(pstHashTable, (void*)"mpe", (void*)THMA_PORT_ExtractAV);
	HLIB_HASH_Insert(pstHashTable, (void*)"mpg", (void*)THMA_PORT_ExtractAV);
	HLIB_HASH_Insert(pstHashTable, (void*)"mpeg", (void*)THMA_PORT_ExtractAV);
	HLIB_HASH_Insert(pstHashTable, (void*)"vob", (void*)THMA_PORT_ExtractAV);
	HLIB_HASH_Insert(pstHashTable, (void*)"divx", (void*)THMA_PORT_ExtractAV);

	HLIB_HASH_Insert(pstHashTable, (void*)"3gp", (void*)THMA_PORT_ExtractAV);
	HLIB_HASH_Insert(pstHashTable, (void*)"3g2", (void*)THMA_PORT_ExtractAV);
	HLIB_HASH_Insert(pstHashTable, (void*)"mov", (void*)THMA_PORT_ExtractAV);
	HLIB_HASH_Insert(pstHashTable, (void*)"flv", (void*)THMA_PORT_ExtractAV);
	HLIB_HASH_Insert(pstHashTable, (void*)"swf", (void*)THMA_PORT_ExtractAV);
	HLIB_HASH_Insert(pstHashTable, (void*)"m2ts", (void*)THMA_PORT_ExtractAV);

	HLIB_HASH_Insert(pstHashTable, (void*)"rmvb", (void*)THMA_PORT_ExtractAV);
	HLIB_HASH_Insert(pstHashTable, (void*)"ogm", (void*)THMA_PORT_ExtractAV);
	HLIB_HASH_Insert(pstHashTable, (void*)"ogg", (void*)THMA_PORT_ExtractAV);
	HLIB_HASH_Insert(pstHashTable, (void*)"dat", (void*)THMA_PORT_ExtractAV);
	HLIB_HASH_Insert(pstHashTable, (void*)"rm", (void*)THMA_PORT_ExtractAV);

	HLIB_HASH_Insert(pstHashTable, (void*)"ts_a",  (void*)THMA_PORT_ExtractAVSequential);
	HLIB_HASH_Insert(pstHashTable, (void*)"tp_a",  (void*)THMA_PORT_ExtractAVSequential);
	HLIB_HASH_Insert(pstHashTable, (void*)"avi_a", (void*)THMA_PORT_ExtractAVSequential);
	HLIB_HASH_Insert(pstHashTable, (void*)"mp4_a", (void*)THMA_PORT_ExtractAVSequential);
	HLIB_HASH_Insert(pstHashTable, (void*)"mkv_a", (void*)THMA_PORT_ExtractAVSequential);
	HLIB_HASH_Insert(pstHashTable, (void*)"asf_a", (void*)THMA_PORT_ExtractAVSequential);
	HLIB_HASH_Insert(pstHashTable, (void*)"wmv_a", (void*)THMA_PORT_ExtractAVSequential);
	HLIB_HASH_Insert(pstHashTable, (void*)"m4v_a", (void*)THMA_PORT_ExtractAVSequential);
	HLIB_HASH_Insert(pstHashTable, (void*)"m2v_a", (void*)THMA_PORT_ExtractAVSequential);

	return ERR_OK;
}

HERROR	THMA_PORT_ExtractMP3(const HCHAR *pszURL, const HCHAR *pszCacheURL, HUINT32 ulTime)
{
	HINT32			nWidth, nHeight;
	HINT32			nNewWidth, nNewHeight;
	HxID3_Picture_t	stInfo;

	HxSTD_MemSet(&stInfo,0x0, sizeof(HxID3_Picture_t));
	HxLOG_Debug("pszURL[%s] pszCacheURL[%s]\n", pszURL, pszCacheURL);
	if (HLIB_ID3_GetID3Picture(pszURL, &stInfo) != ERR_OK)
		return ERR_FAIL;

	if (HLIB_JPEG_GetBufferInfo(stInfo.pucData, stInfo.ulPictureSize, &nWidth, &nHeight) == ERR_OK)
	{
		if (thma_port_checkImageSize(nWidth, nHeight) != TRUE)
			goto err_rtn;

		if (thma_port_recalcRect(&nNewWidth, &nNewHeight, nWidth, nHeight) == TRUE)
		{
			HLIB_JPEG_CreateScaledJPEGMEM(stInfo.pucData, stInfo.ulPictureSize, pszCacheURL, nNewWidth, nNewHeight);
		} else
			HLIB_FILE_WriteFileWithLock(pszCacheURL, stInfo.pucData, stInfo.ulPictureSize);

		HLIB_ID3_MemFree(stInfo.pucData);

		return ERR_OK;
	}

err_rtn:
	if(stInfo.pucData)
		TH_MEM_Free(stInfo.pucData);

	(void)(ulTime);

	return ERR_FAIL;
}

HERROR	THMA_PORT_ExtractFLAC(const HCHAR *pszURL, const HCHAR *pszCacheURL, HUINT32 ulTime)
{
	HINT32			nWidth, nHeight;
	HINT32			nNewWidth, nNewHeight;
	HxID3_Picture_t	stInfo;

	HxLOG_Debug("pszURL[%s] pszCacheURL[%s]\n", pszURL, pszCacheURL);
	HxSTD_MemSet(&stInfo,0x0, sizeof(HxID3_Picture_t));

	if (HLIB_ID3_GetFlacPicture(pszURL, &stInfo) != ERR_OK)
		return ERR_FAIL;

	if (HLIB_JPEG_GetBufferInfo(stInfo.pucData, stInfo.ulPictureSize, &nWidth, &nHeight) == ERR_OK)
	{
		if (thma_port_checkImageSize(nWidth, nHeight) != TRUE)
			goto err_rtn;

		if (thma_port_recalcRect(&nNewWidth, &nNewHeight, nWidth, nHeight) == TRUE)
		{
			HLIB_JPEG_CreateScaledJPEGMEM(stInfo.pucData, stInfo.ulPictureSize, pszCacheURL, nNewWidth, nNewHeight);
		} else
			HLIB_FILE_WriteFileWithLock(pszCacheURL, stInfo.pucData, stInfo.ulPictureSize);

		HLIB_MEM_Free(stInfo.pucData);

		return ERR_OK;
	}

err_rtn:
	if(stInfo.pucData)
		TH_MEM_Free(stInfo.pucData);

	(void)(ulTime);

	return ERR_FAIL;
}

HERROR	THMA_PORT_ExtractJPG(const HCHAR *pszURL, const HCHAR *pszCacheURL, HUINT32 ulTime)
{
	HUINT8	*pucData;
	HINT32	nSize;
	HUINT32	ulWidth, ulHeight;
	HINT32	nNewWidth, nNewHeight;
	HERROR	err = ERR_OK;

	HxLOG_Debug("pszURL[%s] pszCacheURL[%s]\n", pszURL, pszCacheURL);
	pucData = HLIB_EXIF_GetThumbnail(pszURL, &nSize);
	if (pucData)
	{
		if (HLIB_JPEG_GetBufferInfo(pucData, nSize, &ulWidth, &ulHeight) == ERR_OK)
		{
			if (thma_port_recalcRect(&nNewWidth, &nNewHeight, ulWidth, ulHeight) == TRUE)
				err |= HLIB_JPEG_CreateScaledJPEGMEM(pucData, nSize, pszCacheURL, nNewWidth, nNewHeight);
			else
				err |= HLIB_FILE_WriteFileWithLock(pszCacheURL, pucData, nSize);
		}
		HLIB_EXIF_FreeBuffer(pucData);
	} else
	{
		if (HLIB_JPEG_GetFileInfo(pszURL, &ulWidth, &ulHeight) == ERR_OK)
		{
			if (thma_port_recalcRect(&nNewWidth, &nNewHeight, ulWidth, ulHeight) == TRUE)
				err |= HLIB_JPEG_CreateScaledJPEG(pszURL, pszCacheURL, nNewWidth, nNewHeight);
			else
				err |= HLIB_FILE_CopyWithLock(pszURL, pszCacheURL);
		}
	}

#ifdef	THMA_THUMBNAIL_LARGE
	if (HLIB_JPEG_GetFileInfo(pszURL, &ulWidth, &ulHeight) == ERR_OK)
	{
		HCHAR	*pszExt;
		HCHAR	*pszLargeThumbnailUrl;

		HLIB_STD_StrNCpySafe(s_szLargeImageCacheName, pszCacheURL, 1024);
		pszExt = HLIB_PATH_Extension(s_szLargeImageCacheName);
		if (pszExt == NULL)
		{
			HxLOG_Critical("can't find extension [%s] <-- Critical!!!!\n", s_szLargeImageCacheName);
			err |= ERR_FAIL;
		} else
		{
			pszLargeThumbnailUrl = thma_getLargeThumbnailName(s_szLargeImageCacheName, 1024);
			if (thma_port_recalcRect2(&nNewWidth, &nNewHeight, ulWidth, ulHeight) == TRUE)
			{
				err |= HLIB_JPEG_CreateScaledJPEG(pszURL, s_szLargeImageCacheName, nNewWidth, nNewHeight);
			}
			//HLIB_FILE_Delete(pszLargeThumbnailUrl);
		}
	}
#endif

	return err;
}

HERROR	THMA_PORT_ExtractAV (const HCHAR *pszURL, const HCHAR *pszCacheURL, HUINT32 ulTime)
{
	HERROR	err;
	HxAV_ImageInfo_t	*pstBuffer;
	HxAV_SteamInfo_t stStreamInfo;

	HxLOG_Debug("pszURL[%s] pszCacheURL[%s]\n", pszURL, pszCacheURL);
	err = ERR_FAIL;

	if(thma_port_existmeta(pszURL) == TRUE)
		return err;

	pstBuffer = HLIB_AV_GetVideoThumbnail(pszURL, &stStreamInfo, ulTime, thma_port_recalcRect, NULL, NULL);
	if (pstBuffer)
	{
		if (HLIB_JPEG_CreateFile(pszCacheURL, pstBuffer->nWidth, pstBuffer->nHeight, pstBuffer->data, NULL) == ERR_OK)
			err = ERR_OK;

		HLIB_AV_FreeImageInfo(pstBuffer);
		pstBuffer = NULL;
	}

	return err;
}

//	sequential name prefix --> [FILENAME].?.jpg  --> ? is time (sec)
HERROR	THMA_PORT_ExtractAVSequential(const HCHAR *pszURL, const HCHAR *pszCacheURL, HUINT32 ulStepTime)
{
#if	1
	HxLOG_Error("Not supported YET!!!\n");
#else
	HxAV_t	*pstInfo;
	HUINT8	*pucBuffer;
	HCHAR	*pszFileName = TH_MEM_Calloc(1024);
	HCHAR	*pszTimePtr;
	HINT32	nLen;
	HUINT32	ulTime;

	HxLOG_Debug("pszURL [%s] pszCacheURL[%s]\n", pszURL, pszCacheURL);

	ulTime = 0;
	HxSTD_StrNCpy(pszFileName, pszCacheURL, 1024);
	pszTimePtr = HLIB_STD_StrCaseStr(pszFileName, ".?.jpg");
	nLen = 1024 - HxSTD_StrLen(pszFileName);

	pstInfo = HxAV_Open(pszURL);
	if (pstInfo)
	{
		HxAV_SteamInfo_t	stStreamInfo;

		if (HLIB_AV_GetVideoStreamInfo(pstInfo, &stStreamInfo) == ERR_OK)
		{
			HINT32		width, height;

			thma_port_recalcRect(&width, &height, stStreamInfo.nWidth, stStreamInfo.nHeight);

			while (ulTime < stStreamInfo.nDuration)
			{
				HxSTD_snprintf(pszTimePtr, nLen, ".%d.jpg", ulTime);
				HxLOG_Debug("Write file time[%d] name : [%s]\n", ulTime, pszFileName);

				if (HLIB_FILE_Exist(pszFileName) == FALSE)
				{
					pucBuffer = HLIB_AV_GetVideoThumbnail(pstInfo, ulTime, width, height);
					if (pucBuffer)
					{
						HLIB_JPEG_CreateFile(pszFileName, width, height, pucBuffer, NULL);
						HLIB_STD_MemFree(pucBuffer);
					}
				} else
				{
					HxLOG_Debug("file is already exist skip [%s]\n", pszFileName);
				}
				pucBuffer = NULL;

				ulTime += ulStepTime;

				HLIB_STD_TaskSleep(1);
			}
		}

		HxAV_Close(pstInfo);
		TH_MEM_Free(pszFileName);

		return ERR_OK;
	}

	TH_MEM_Free(pszFileName);
#endif

	return ERR_FAIL;
}

HERROR	THMA_PORT_ExtractTS (const HCHAR *pszURL, const HCHAR *pszCacheURL, HUINT32 ulTime)
{
	return ERR_FAIL;
}

/**************************************************************************************************/
#define _________THMA_End_Of_File____________________________________________________
/**************************************************************************************************/

