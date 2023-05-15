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
#include <dlib.h>
#include <dapi.h>
#include "thma_int.h"
#include <oapi.h>

/**************************************************************************************************/
#define _________THMA_Private_Macro_________________________________________________
/**************************************************************************************************/
#define	THMA_PORT_RECORDING_MINIMUM		10		//	sec

#define	THMA_PORT_RECORDING_FRAME_WIDTH		THMA_THUMBNAIL_WIDTH
#define	THMA_PORT_RECORDING_FRAME_HEIGHT	THMA_THUMBNAIL_HEIGHT
/**************************************************************************************************/
#define _________THMA_Private_Struct_________________________________________________
typedef struct
{
	HCHAR	szUrl[1024];
	HUINT16	usVideoPid;
	HUINT32	ulVideoCodec;
	HBOOL	bEncrypt;
	HUINT32	ulRecDuration;
} thma_port_recording_info_t;

/**************************************************************************************************/
#define _________THMA_Private_Value_________________________________________________
/**************************************************************************************************/
static HxCBUF_t	s_pstCBuf = NULL;

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

/**************************************************************************************************/
#define _________THMA_Public_Proc____________________________________________________
/**************************************************************************************************/
extern HERROR	THMA_PORT_ExtractHJM(const HCHAR *pszURL, const HCHAR *pszCacheURL, HUINT32 ulTime);
extern HERROR	THMA_PORT_ExtractHJMSequential(const HCHAR *pszURL, const HCHAR *pszCacheURL, HUINT32 ulStepTime);

/**************************************************************************************************/
#define _________THMA_Public_Functions__________________________________________________________
/**************************************************************************************************/
HERROR	THMA_PORT_RECORDING_Init(HxHASH_t *pstHashTable)
{
	s_pstCBuf = HLIB_CBUF_New(8192);

	HLIB_HASH_Insert(pstHashTable, (void*)"hjm", (void*)THMA_PORT_ExtractHJM);
	HLIB_HASH_Insert(pstHashTable, (void*)"hjm_a", (void*)THMA_PORT_ExtractHJMSequential);

	return ERR_OK;
}

const thma_port_recording_info_t*	thma_port_recording_loadURL(const HCHAR *pszURL)
{

#if defined(CONFIG_PROD_OS_EMULATOR)
//TODO;
	return NULL;
#else
	static thma_port_recording_info_t	*s_pstRecData = NULL;
	struct stat	fstat;
	HCHAR	*pszFileName = NULL;
	FILE	*filep = NULL;
	HCHAR	*pszBuf = NULL;

	HxLOG_Debug("s_pstRecData [%p]\n", s_pstRecData);
	if (s_pstRecData)
	{
		if (HxSTD_StrCmp(s_pstRecData->szUrl, pszURL) == 0)
			return s_pstRecData;
	}

	if (s_pstRecData)
		TH_MEM_Free(s_pstRecData);
	s_pstRecData = NULL;

	pszFileName = (HCHAR*)pszURL;
	if (HLIB_STD_StrCaseStartWith(pszURL, "HPVR:"))
		pszFileName = (HCHAR*)pszURL + HxSTD_StrLen("HPVR:/");

	HxLOG_Debug("check file [%s]\n", pszFileName);
	filep = fopen(pszFileName, "rb");
	if (filep && (stat(pszFileName, &fstat) == 0)) /* Changed fopen and stat calling order for flush */
	{
		if (!fstat.st_size)
		{
			fsync(fileno(filep)); /* flush */
			}

		if (fstat.st_size) /* Prevent DLIB_Decode() assertion by obtaining the abnormal stat infos(size, ...) : e.g... hjm filep is created immediately (before fflush), ... */
			{
			pszBuf = TH_MEM_Calloc(fstat.st_size + 1);
			if (fread(pszBuf, 1, fstat.st_size, filep) == fstat.st_size)
			{
				HINT32	nSize;
				DxRecListData_t	*pstTempData = NULL;

#if !defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
				if (DLIB_Decode(DxNAMEOF(DxRecListData_t), 0, (void*)pszBuf, fstat.st_size, (const void**)&pstTempData, &nSize) == ERR_OK)
#else
				if (DLIB_CreateDecodedData(DxNAMEOF(DxRecListData_t), 0, (void*)pszBuf, fstat.st_size, (const void**)&pstTempData, &nSize) == ERR_OK)
#endif
				{
					HxLOG_Debug("URL : [%s] ulTime : [%d]\n", pstTempData->szUrl, pstTempData->ulRecDuration);
					if ((pstTempData->ulRecDuration > THMA_PORT_RECORDING_MINIMUM) && (pstTempData->bScrambled == FALSE))
					{
						s_pstRecData = (thma_port_recording_info_t*)TH_MEM_Calloc(sizeof(thma_port_recording_info_t));

						HLIB_STD_StrNCpySafe(s_pstRecData->szUrl, pszURL, 1024);
						s_pstRecData->usVideoPid = pstTempData->stMeta.usVideoPid;
						s_pstRecData->ulVideoCodec = pstTempData->stMeta.eVideoCodec;
						s_pstRecData->bEncrypt = pstTempData->bEncrypted;
						s_pstRecData->ulRecDuration = pstTempData->ulRecDuration;
					}
				}

#if defined(CONFIG_TEMP_DLIB_NO_AUTOMATIC_TRASHING)
				if (NULL != pstTempData)
				{
					DLIB_DestroyDecodedData(DxNAMEOF(DxRecListData_t), (const void *)pstTempData);
				}
#endif
			} else
				HxLOG_Error("error : DxRecListData Decode [%p, size:%d, URL:%s]\n", filep, fstat.st_size, pszURL);
			}
			else
			{
			HxLOG_Error("error : abnormal stat [%p, size:%d, URL:%s]\n", filep, fstat.st_size, pszURL);
			}
		}
	else
	{
		HxLOG_Error("error : open failed [null, size:%d, URL:%s]\n",  fstat.st_size, pszURL);
	}

	if (filep)
		fclose(filep);
	if (pszBuf)
		TH_MEM_Free(pszBuf);

	HxLOG_Debug("return [%p]\n", s_pstRecData);
	return s_pstRecData;
#endif /* CONFIG_PROD_OS_EMULATOR */
}

extern HUINT8*	MW_PVR_ExtractFrameFromFile(HCHAR *pszFileName, HUINT32 ulTime, HINT32 nWidth, HINT32 nHeight, HUINT16 usVideoPid, HUINT32 ulVideoCodec, HBOOL bEncryption);

HERROR	THMA_PORT_ExtractHJM(const HCHAR *pszURL, const HCHAR *pszCacheURL, HUINT32 ulTime)
{
	HCHAR	*pszFileName;
	HUINT8	*pucFrame;
	thma_port_recording_info_t	*pstRecData;

	HxLOG_Trace();
	pstRecData = (thma_port_recording_info_t*)thma_port_recording_loadURL(pszURL);
	if (pstRecData == NULL)
		return ERR_FAIL;

	pszFileName = (HCHAR*)pszURL;
	if (HLIB_STD_StrCaseStartWith(pszURL, "HPVR:"))
		pszFileName = (HCHAR*)pszURL + HxSTD_StrLen("HPVR:/");

	{
		OAPI_PLATFORM_VIDEO_ExtractFrame((HCHAR*)pszFileName, ulTime, THMA_PORT_RECORDING_FRAME_WIDTH, THMA_PORT_RECORDING_FRAME_HEIGHT,
								pstRecData->usVideoPid, pstRecData->ulVideoCodec, pstRecData->bEncrypt, (HCHAR*)pszCacheURL);
		(void)(pucFrame);
	}

	return ERR_FAIL;
}

HERROR	THMA_PORT_ExtractHJMSequential(const HCHAR *pszURL, const HCHAR *pszCacheURL, HUINT32 ulStepTime)
{
	HCHAR	*pszFileName;
	HUINT8	*pucFrame;
	thma_port_recording_info_t	*pstRecData;
	HUINT32	ulTime;
	HINT32	nLen;
	HCHAR	*pszSaveName = NULL;
//	HCHAR	*pszTimePtr;					// unused value

	HxLOG_Trace();
	pstRecData = (thma_port_recording_info_t*)thma_port_recording_loadURL(pszURL);
	if (pstRecData == NULL)
		return ERR_FAIL;

	pszFileName = (HCHAR*)pszURL;
	if (HLIB_STD_StrCaseStartWith(pszURL, "HPVR:"))
		pszFileName = (HCHAR*)pszURL + HxSTD_StrLen("HPVR:/");

	pszSaveName = (HCHAR*)TH_MEM_Calloc(1024);
	HxSTD_StrNCpy(pszSaveName, pszCacheURL, 1024);
//	pszTimePtr = HLIB_STD_StrCaseStr(pszSaveName, ".?.jpg");				// unused value
	nLen = 1024 - HxSTD_StrLen(pszFileName);

	ulTime = 0;
	while (ulTime < pstRecData->ulRecDuration)
	{
		OAPI_PLATFORM_VIDEO_ExtractFrame((HCHAR*)pszFileName, ulTime, THMA_PORT_RECORDING_FRAME_WIDTH, THMA_PORT_RECORDING_FRAME_HEIGHT,
								pstRecData->usVideoPid, pstRecData->ulVideoCodec, pstRecData->bEncrypt, (HCHAR*)pszCacheURL);
		ulTime += ulStepTime;
	}
	(void)(pucFrame);
	TH_MEM_Free(pszSaveName);

	return ERR_OK;
}

/**************************************************************************************************/
#define _________THMA_End_Of_File____________________________________________________
/**************************************************************************************************/

