/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
/** @brief global header file  */
#include <stdio.h>
#include "isosvc.h"
#include <octo_common.h>

/** @brief local header file  */
#include "../local_include/_svc_cas_si_api.h"
#include "_svc_cas_si_desmgr.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



#define SIZE_USER_DEFINED_DES_CB_ARRAY				128				// 0x80 ~ 0xFE

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/** @brief struct에 대한 간단한 설명  */

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/


/************************************************************************
 *
 * String 관련...
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitMem													*/
/*  																	*/
/*  Description : 메모리 초기화.										*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitMem(HUINT8 *pszStr, HUINT32 ulStrLen)
{
	HxSTD_MemSet(pszStr, '\0', ulStrLen);
}


/************************************************************************/
/*	function : allocMem													*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

HUINT8	*svc_cas_SiAllocMem(HUINT32 ulStrLen)
{
	HUINT8*	pucStr;

	if (ulStrLen > 0)
	{
		pucStr = (HUINT8 *)OxCAS_Malloc (sizeof(HUINT8) * ulStrLen);
	}
	else
	{
		pucStr = NULL;
	}

	if (pucStr == NULL)
	{
		HxLOG_Print("[ERROR] String allocation error!\n");
		return NULL;
	}

	svc_cas_SiDesInitMem(pucStr, ulStrLen);

	return ((HUINT8 *)pucStr);
}

HUINT8 *	svc_cas_SiDupMem(const HUINT8 *pucMem, HUINT32 ulSize)
{
	return (HUINT8 *)HLIB_STD_MemDupEx(pucMem, ulSize, (void *(*)(HUINT32))svc_cas_SiAllocMem);
}


/************************************************************************/
/*	function : FreeMem													*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

void 	svc_cas_SiFreeMem(HUINT8 *pucStr)
{
	if (pucStr != NULL)
	{
		OxCAS_Free (pucStr);
	}
}


/************************************************************************
 *
 * Private Data 관련...
 *
 * 	(ps) private data 구조는 MPEG or DVD 규정의 구조체가 아님.
 *		 임의의 크기의 data를 처리하기 위하여 define한 구조체.
 ************************************************************************/
/************************************************************************/
/*	function : InitPrivateDataInfo										*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitPrivateDataInfo(svcCas_PrivateDataInfo_t *pstPrivateDataInfo)
{
	pstPrivateDataInfo->ucPrivateDataLen	= 0;
	pstPrivateDataInfo->pPrivateData		= NULL;
}


/************************************************************************/
/*	function : AllocPrivateDataInfo										*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC svcCas_PrivateDataInfo_t *svc_cas_SiDesAllocPrivateDataInfo(void)
{
	svcCas_PrivateDataInfo_t*	pstPrivateDataInfo;

	pstPrivateDataInfo = (svcCas_PrivateDataInfo_t *)OxCAS_Malloc(sizeof(svcCas_PrivateDataInfo_t));
	if (pstPrivateDataInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocPrivateDataInfo()\n");
		return NULL;
	}
	svc_cas_SiDesInitPrivateDataInfo(pstPrivateDataInfo);

	return ((svcCas_PrivateDataInfo_t *)pstPrivateDataInfo);
}


/************************************************************************/
/*	function : FreePrivateDataInfo										*/
/*  																	*/
/*  Description : 메모리 헤제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreePrivateDataInfo(svcCas_PrivateDataInfo_t *pstPrivateDataInfo)
{
	if(pstPrivateDataInfo == NULL)
	{
		return;
	}

	if (pstPrivateDataInfo->pPrivateData != NULL)
	{
		OxCAS_Free(pstPrivateDataInfo->pPrivateData);
	}

	OxCAS_Free(pstPrivateDataInfo);
}


/************************************************************************/
/*	function : SetPrivateDataInfo										*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void	svc_cas_SiSetPrivateDataInfo(svcCas_PrivateDataInfo_t *pstPrivateDataInfo, HUINT8 *pucDesData, HUINT16 usPrivateDataLen)
{
	pstPrivateDataInfo->ucPrivateDataLen	= (HUINT8) usPrivateDataLen;
	pstPrivateDataInfo->pPrivateData = svc_cas_SiAllocMem(usPrivateDataLen+1);

	if( pstPrivateDataInfo->pPrivateData != NULL )
	{
		HxSTD_MemCopy(pstPrivateDataInfo->pPrivateData, pucDesData, usPrivateDataLen);
	}
}


/************************************************************************
 *
 * Video Stream Descriptor 관련... (TAG = 0x02)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitVideoStmDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitVideoStmDes(svcCas_VideoStmDes_t *pstVideoStmDes)
{
	pstVideoStmDes->bMultiFrameRate	= FALSE;
	pstVideoStmDes->ucFrameRateCode	= 0;
	pstVideoStmDes->bMPEG2			= FALSE;
	pstVideoStmDes->bConstrainParam	= FALSE;
	pstVideoStmDes->bStillPic		= FALSE;
}

/************************************************************************/
/*	function : AllocVideoStmDes											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_VideoStmDes_t 	*svc_cas_SiDesAllocVideoStmDes(void)
{
	svcCas_VideoStmDes_t	*pstVideoStmDes;

	pstVideoStmDes = (svcCas_VideoStmDes_t *)OxCAS_Malloc(sizeof(svcCas_VideoStmDes_t));
	if (pstVideoStmDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocVideoStmDes()\n");
		return NULL;
	}
	svc_cas_SiDesInitVideoStmDes(pstVideoStmDes);

	return ((svcCas_VideoStmDes_t *)pstVideoStmDes);
}


/************************************************************************/
/*	function : FreeVideoStmDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreeVideoStmDes(svcCas_VideoStmDes_t *pstVideoStmDes)
{
	if(pstVideoStmDes != NULL)
	{
		OxCAS_Free(pstVideoStmDes);
	}
}


/************************************************************************/
/*	function : SetVideoStmDes											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetVideoStmDes(svcCas_VideoStmDes_t *pstVideoStmDes, HBOOL bMultiFrameRate, HUINT8 ucFrameRateCode, HBOOL bMPEG2, HBOOL bConstrainParam, HBOOL bStillPic)
{
	pstVideoStmDes->bMultiFrameRate 	= bMultiFrameRate;
	pstVideoStmDes->ucFrameRateCode 	= ucFrameRateCode;
	pstVideoStmDes->bMPEG2 				= bMPEG2;
	pstVideoStmDes->bConstrainParam 	= bConstrainParam;
	pstVideoStmDes->bStillPic 			= bStillPic;
}


/************************************************************************
 *
 * Audio Stream Descriptor 관련... (TAG = 0x03)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitAudioStmDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitAudioStmDes(svcCas_AudioStmDes_t *pstAudioStmDes)
{
	pstAudioStmDes->bFreeFormat				= FALSE;
	pstAudioStmDes->ucId						= 0;
	pstAudioStmDes->ucLayer					= 0;
	pstAudioStmDes->ucVarRateAudioIndicator	= 0;
}


/************************************************************************/
/*	function : AllocAudioStmDes											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_AudioStmDes_t 	*svc_cas_SiDesAllocAudioStmDes(void)
{
	svcCas_AudioStmDes_t	*pstAudioStmDes;

	pstAudioStmDes = (svcCas_AudioStmDes_t *)OxCAS_Malloc(sizeof(svcCas_AudioStmDes_t));
	if (pstAudioStmDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocAudioStmDes()\n");
		return NULL;
	}
	svc_cas_SiDesInitAudioStmDes(pstAudioStmDes);

	return ((svcCas_AudioStmDes_t *)pstAudioStmDes);
}


/************************************************************************/
/*	function : FreeAudioStmDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreeAudioStmDes(svcCas_AudioStmDes_t *pstAudioStmDes)
{
	if(pstAudioStmDes != NULL)
	{
		OxCAS_Free(pstAudioStmDes);
	}
}


/************************************************************************/
/*	function : SetAudioStmDes											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/
void 	svc_cas_SiSetAudioStmDes(svcCas_AudioStmDes_t *pstAudioStmDes, HBOOL bFreeFormat, HUINT8 ucId, HUINT8 ucLayer, HUINT8 ucVarRateAudioIndicator)
{
	pstAudioStmDes->bFreeFormat 				= bFreeFormat;
	pstAudioStmDes->ucId 						= ucId;
	pstAudioStmDes->ucLayer 					= ucLayer;
	pstAudioStmDes->ucVarRateAudioIndicator 	= ucVarRateAudioIndicator;
}


/************************************************************************
 *
 * Hierarchy Descriptor 관련... (TAG = 0x04)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitHierDes												*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitHierDes(svcCas_HierDes_t *pstHierDes)
{
	pstHierDes->ucHierType				= 0;
	pstHierDes->ucHierLayerIndex			= 0;
	pstHierDes->ucHierEmbedLayerIndex	= 0;
	pstHierDes->ucHierChannel			= 0;
}


/************************************************************************/
/*	function : AllocHierDes												*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_HierDes_t 	*svc_cas_SiDesAllocHierDes(void)
{
	svcCas_HierDes_t	*pstHierDes;

	pstHierDes = (svcCas_HierDes_t *)OxCAS_Malloc(sizeof(svcCas_HierDes_t));
	if (pstHierDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocHierDes()\n");
		return NULL;
	}
	svc_cas_SiDesInitHierDes(pstHierDes);

	return ((svcCas_HierDes_t *)pstHierDes);
}


/************************************************************************/
/*	function : FreeHierDes												*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreeHierDes(svcCas_HierDes_t *pstHierDes)
{
	if(pstHierDes != NULL)
	{
		OxCAS_Free(pstHierDes);
	}
}


/************************************************************************/
/*	function : SetHierDes												*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetHierDes(svcCas_HierDes_t *pstHierDes, HUINT8 ucHierType, HUINT8 ucHierLayerIndex, HUINT8 ucHierEmbedLayerIndex, HUINT8 ucHierChannel)
{
	pstHierDes->ucHierType 				= ucHierType;
	pstHierDes->ucHierLayerIndex 		= ucHierLayerIndex;
	pstHierDes->ucHierEmbedLayerIndex 	= ucHierEmbedLayerIndex;
	pstHierDes->ucHierChannel 			= ucHierChannel;
}


/************************************************************************
 *
 * Registration Descriptor 관련... (TAG = 0x05)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitRegDes												*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitRegDes(svcCas_RegDes_t *pstRegDes)
{
	pstRegDes->ulFormatIdent		= 0;
	pstRegDes->pPrivateDataInfo	= NULL;
}


/************************************************************************/
/*	function : AllocRegDes												*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_RegDes_t 	*svc_cas_SiDesAllocRegDes(void)
{
	svcCas_RegDes_t	*pstRegDes;

	pstRegDes = (svcCas_RegDes_t *)OxCAS_Malloc(sizeof(svcCas_RegDes_t));
	if (pstRegDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocRegDes()\n");
		return NULL;
	}
	svc_cas_SiDesInitRegDes(pstRegDes);

	return ((svcCas_RegDes_t *)pstRegDes);
}


/************************************************************************/
/*	function : FreeRegDes												*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreeRegDes(svcCas_RegDes_t *pstRegDes)
{
	if (pstRegDes == NULL)
	{
		return;
	}

	if (pstRegDes->pPrivateDataInfo != NULL)
	{
		svc_cas_SiDesFreePrivateDataInfo(pstRegDes->pPrivateDataInfo);
	}

	OxCAS_Free(pstRegDes);
}


/************************************************************************/
/*	function : SetRegDes												*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetRegDes(svcCas_RegDes_t *pstRegDes, HUINT32 ulFormatIdent, svcCas_PrivateDataInfo_t *pstPrivateDataInfo)
{
	pstRegDes->ulFormatIdent		= ulFormatIdent;
	pstRegDes->pPrivateDataInfo		= pstPrivateDataInfo;
}


/************************************************************************
 *
 * Data Stream Align Descriptor 관련... (TAG = 0x06)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitDataStmAlignDes										*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitDataStmAlignDes(svcCas_DataStmAlignDes_t *pstDataStmAlignDes)
{
	pstDataStmAlignDes->ucAlignType = 0;
}


/************************************************************************/
/*	function : AllocDataStmAlignDes										*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_DataStmAlignDes_t 	*svc_cas_SiDesAllocDataStmAlignDes(void)
{
	svcCas_DataStmAlignDes_t	*pstDataStmAlignDes;

	pstDataStmAlignDes = (svcCas_DataStmAlignDes_t *)OxCAS_Malloc(sizeof(svcCas_DataStmAlignDes_t));
	if (pstDataStmAlignDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocDataStmAlignDes()\n");
		return NULL;
	}

	svc_cas_SiDesInitDataStmAlignDes(pstDataStmAlignDes);

	return ((svcCas_DataStmAlignDes_t *)pstDataStmAlignDes);
}


/************************************************************************/
/*	function : FreeDataStmAlignDes										*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreeDataStmAlignDes(svcCas_DataStmAlignDes_t *pstDataStmAlignDes)
{
	if(pstDataStmAlignDes != NULL)
	{
		OxCAS_Free(pstDataStmAlignDes);
	}
}


/************************************************************************/
/*	function : SetDataStmAlignDes										*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetDataStmAlignDes(svcCas_DataStmAlignDes_t *pstDataStmAlignDes, HUINT8 ucAlignType)
{
	pstDataStmAlignDes->ucAlignType = ucAlignType;
}


/************************************************************************
 *
 * Target Background Grid Descriptor 관련... (TAG = 0x07)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitBackGridDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitBackGridDes(svcCas_BackGridDes_t *pstBackGridDes)
{
	pstBackGridDes->usHoriSize		= 0;
	pstBackGridDes->usVertSize		= 0;
	pstBackGridDes->ucAspectRatio	= 0;
}


/************************************************************************/
/*	function : AllocBackGridDes											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_BackGridDes_t 	*svc_cas_SiDesAllocBackGridDes(void)
{
	svcCas_BackGridDes_t	*pstBackGridDes;


	pstBackGridDes = (svcCas_BackGridDes_t *)OxCAS_Malloc(sizeof(svcCas_BackGridDes_t));
	if (pstBackGridDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocBackGridDes()\n");
		return NULL;
	}
	svc_cas_SiDesInitBackGridDes(pstBackGridDes);

	return ((svcCas_BackGridDes_t *)pstBackGridDes);
}


/************************************************************************/
/*	function : FreeBackGridDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreeBackGridDes(svcCas_BackGridDes_t *pstBackGridDes)
{
	if(pstBackGridDes != NULL)
	{
		OxCAS_Free(pstBackGridDes);
	}
}


/************************************************************************/
/*	function : SetBackGridDes											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetBackGridDes(svcCas_BackGridDes_t *pstBackGridDes, HUINT16 usHoriSize, HUINT16 usVertSize, HUINT8 ucAspectRatio)
{
	pstBackGridDes->usHoriSize 	= usHoriSize;
	pstBackGridDes->usVertSize 	= usVertSize;
	pstBackGridDes->ucAspectRatio = ucAspectRatio;
}


/************************************************************************
 *
 * Video Window Descriptor 관련... (TAG = 0x08)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitVideoWindowDes										*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitVideoWindowDes(svcCas_VideoWindowDes_t *pstVideoWindowDes)
{
	pstVideoWindowDes->usHoriOff			= 0;
	pstVideoWindowDes->usVertOff			= 0;
	pstVideoWindowDes->ucWindowPriority		= 0;
}


/************************************************************************/
/*	function : AllocVideoWindowDes										*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_VideoWindowDes_t 	*svc_cas_SiDesAllocVideoWindowDes(void)
{
	svcCas_VideoWindowDes_t	*pstVideoWindowDes;

	pstVideoWindowDes = (svcCas_VideoWindowDes_t *)OxCAS_Malloc(sizeof(svcCas_VideoWindowDes_t));
	if (pstVideoWindowDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocVideoWindowDes()\n");
		return NULL;
	}
	svc_cas_SiDesInitVideoWindowDes(pstVideoWindowDes);

	return ((svcCas_VideoWindowDes_t *)pstVideoWindowDes);
}


/************************************************************************/
/*	function : FreeVideoWindowDes										*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreeVideoWindowDes(svcCas_VideoWindowDes_t *pstVideoWindowDes)
{
	if(pstVideoWindowDes != NULL)
	{
		OxCAS_Free(pstVideoWindowDes);
	}
}


/************************************************************************/
/*	function : SetVideoWindowDes										*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetVideoWindowDes(svcCas_VideoWindowDes_t *pstVideoWindowDes, HUINT16 usHoriOff, HUINT16 usVertOff, HUINT8 ucWindowPriority)
{
	pstVideoWindowDes->usHoriOff			= usHoriOff;
	pstVideoWindowDes->usVertOff			= usVertOff;
	pstVideoWindowDes->ucWindowPriority		= ucWindowPriority;
}


/************************************************************************
 *
 * Conditional Access Descriptor 관련... (TAG = 0x09)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitCaDes												*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitCaDes(svcCas_CaDes_t *pstCaDes)
{
	pstCaDes->usCaSysId 			= 0;
	pstCaDes->usCaPid 				= 0;
	pstCaDes->pPrivateDataInfo		= NULL;
}


/************************************************************************/
/*	function : AllocCaDes												*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_CaDes_t 	*svc_cas_SiDesAllocCaDes(void)
{
	svcCas_CaDes_t	*pstCaDes;

	pstCaDes = (svcCas_CaDes_t *)OxCAS_Malloc(sizeof(svcCas_CaDes_t));
	if (pstCaDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocCaDes()\n");
		return NULL;
	}
	svc_cas_SiDesInitCaDes(pstCaDes);

	return ((svcCas_CaDes_t *)pstCaDes);
}


/************************************************************************/
/*	function : FreeCaDes												*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreeCaDes(svcCas_CaDes_t *pstCaDes)
{
	if(pstCaDes == NULL)
	{
		return;
	}

	if (pstCaDes->pPrivateDataInfo != NULL)
	{
		svc_cas_SiDesFreePrivateDataInfo(pstCaDes->pPrivateDataInfo);
	}
	OxCAS_Free(pstCaDes);
}


/************************************************************************/
/*	function : SetCaDes													*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetCaDes(svcCas_CaDes_t *pstCaDes, HUINT16 usCaSysId, HUINT16 usCaPid, svcCas_PrivateDataInfo_t *pstPrivateDataInfo)
{
	pstCaDes->usCaSysId 			= usCaSysId;
	pstCaDes->usCaPid 				= usCaPid;
	pstCaDes->pPrivateDataInfo		= pstPrivateDataInfo;
}


/************************************************************************
 *
 * ISO 639 Language Descriptor 관련... (TAG = 0x0a)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitIso639LangDes										*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitIso639LangDes(svcCas_Iso639LangDes_t *pstIso639LangDes)
{
	pstIso639LangDes->pLangInfo = NULL;
}


/************************************************************************/
/*	function : InitLangInfo												*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitLangInfo(svcCas_LangInfo_t *pstLangInfo)
{
	HxSTD_MemSet(pstLangInfo->szIso639LangCode, '\0', 4);
	pstLangInfo->ucAudioType = 0;
}


/************************************************************************/
/*	function : AllocIso639LangDes										*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_Iso639LangDes_t 	*svc_cas_SiDesAllocIso639LangDes(void)
{
	svcCas_Iso639LangDes_t	*pstIso639LangDes;


	pstIso639LangDes = (svcCas_Iso639LangDes_t *)OxCAS_Malloc(sizeof(svcCas_Iso639LangDes_t));
	if (pstIso639LangDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocIso639LangDes()\n");
		return NULL;
	}
	svc_cas_SiDesInitIso639LangDes(pstIso639LangDes);

	return ((svcCas_Iso639LangDes_t *)pstIso639LangDes);
}


/************************************************************************/
/*	function : AllocLangInfo											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_LangInfo_t 	*svc_cas_SiDesAllocLangInfo(void)
{
	svcCas_LangInfo_t	*pstLangInfo;

	pstLangInfo = (svcCas_LangInfo_t *)OxCAS_Malloc(sizeof(svcCas_LangInfo_t));
	if (pstLangInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocLangInfo()\n");
		return NULL;
	}
	svc_cas_SiDesInitLangInfo(pstLangInfo);

	return ((svcCas_LangInfo_t *)pstLangInfo);
}


/************************************************************************/
/*	function : FreeIso639LangDes										*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreeIso639LangDes(svcCas_Iso639LangDes_t *pstIso639LangDes)
{
	svcCas_LangInfo_t			*pstIndexLangInfo, *pstDestLangInfo;

	if(pstIso639LangDes == NULL)
	{
		return;
	}

	pstIndexLangInfo = pstIso639LangDes->pLangInfo;

	while (pstIndexLangInfo)
	{
		pstDestLangInfo = pstIndexLangInfo;
		pstIndexLangInfo = pstIndexLangInfo->pNextInfo;
		svc_cas_SiFreeDesInfo(TAG_ISO_639_LANGUAGE, 0, (HUINT8 *)pstDestLangInfo);
	}

	OxCAS_Free(pstIso639LangDes);
}


/************************************************************************/
/*	function : FreeLangInfo												*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreeLangInfo(svcCas_LangInfo_t *pstLangInfo)
{
	if(pstLangInfo)
	{
		OxCAS_Free(pstLangInfo);
	}
}


/************************************************************************/
/*	function : SetLangInfo												*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetLangInfo(svcCas_LangInfo_t *pstLangInfo, HUINT8 *pszIso639LangCode, HUINT8 ucAudioType)
{
	HxSTD_MemCopy(pstLangInfo->szIso639LangCode, pszIso639LangCode, svc_cas_SiUtilStrlen(pszIso639LangCode));
	pstLangInfo->szIso639LangCode[svc_cas_SiUtilStrlen(pszIso639LangCode)] = '\0';
	pstLangInfo->ucAudioType = ucAudioType;
}


/************************************************************************/
/*	function : GetPrevLangInfo											*/
/*  																	*/
/*  Description : 마지막 작업한 Info의 poHUINT32er를 얻는다.				*/
/************************************************************************/

svcCas_LangInfo_t 	*svc_cas_SiGetPrevLangInfo(svcCas_Iso639LangDes_t *pstIso639LangDes)
{
	svcCas_LangInfo_t	*pstLangInfo;


	pstLangInfo = pstIso639LangDes->pLangInfo;
	if (pstLangInfo == NULL)
	{
		return NULL;
	}

	while (pstLangInfo->pNextInfo != NULL)
	{
		pstLangInfo = pstLangInfo->pNextInfo;
	}

	return ((svcCas_LangInfo_t *)pstLangInfo);
}


/************************************************************************/
/*	function : LinkLangInfoHead											*/
/*  																	*/
/*  Description : head의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkLangInfoHead(svcCas_Iso639LangDes_t *pstIso639LangDes, svcCas_LangInfo_t *pstNewLangInfo)
{
	pstIso639LangDes->pLangInfo = pstNewLangInfo;
}


/************************************************************************/
/*	function : LinkLangInfoNode											*/
/*  																	*/
/*  Description : link 설정.											*/
/************************************************************************/

void 	svc_cas_SiLinkLangInfoNode(svcCas_LangInfo_t *pstNewLangInfo, svcCas_LangInfo_t *pstNextLangInfo, svcCas_LangInfo_t *pstPrevLangInfo)
{
	if (pstPrevLangInfo)
	{
		pstPrevLangInfo->pNextInfo = pstNewLangInfo;
	}

	pstNewLangInfo->pNextInfo = pstNextLangInfo;
}


/************************************************************************
 *
 * System Clock Descriptor 관련... (TAG = 0x0b)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitSysClockDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitSysClockDes(svcCas_SysClockDes_t *pstSysClockDes)
{
	pstSysClockDes->ucExtClockRef		= 0;
	pstSysClockDes->ucClockAccuInt	= 0;
	pstSysClockDes->ucClockAccuExp	= 0;
}


/************************************************************************/
/*	function : AllocSysClockDes											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_SysClockDes_t 	*svc_cas_SiDesAllocSysClockDes(void)
{
	svcCas_SysClockDes_t	*pstSysClockDes;

	pstSysClockDes = (svcCas_SysClockDes_t *)OxCAS_Malloc(sizeof(svcCas_SysClockDes_t));
	if (pstSysClockDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocSysClockDes()\n");
		return NULL;
	}
	svc_cas_SiDesInitSysClockDes(pstSysClockDes);

	return ((svcCas_SysClockDes_t *)pstSysClockDes);
}


/************************************************************************/
/*	function : FreeSysClockDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreeSysClockDes(svcCas_SysClockDes_t *pstSysClockDes)
{
	if(pstSysClockDes != NULL)
	{
		OxCAS_Free(pstSysClockDes);
	}
}


/************************************************************************/
/*	function : SetSysClockDes											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetSysClockDes(svcCas_SysClockDes_t *pstSysClockDes, HUINT8 ucExtClockRef, HUINT8 ucClockAccuInt, HUINT8 ucClockAccuExp)
{
	pstSysClockDes->ucExtClockRef	= ucExtClockRef;
	pstSysClockDes->ucClockAccuInt	= ucClockAccuInt;
	pstSysClockDes->ucClockAccuExp	= ucClockAccuExp;
}


/************************************************************************
 *
 * Multiplex Buffer Utilization Descriptor 관련... (TAG = 0x0c)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitMultiplexBufUtilDes									*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitMultiplexBufUtilDes(svcCas_MultiplexBufUtilDes_t *pstMultiplexBufUtilDes)
{
	pstMultiplexBufUtilDes->bBoundValid		= 0;
	pstMultiplexBufUtilDes->usLtwOffLowBound	= 0;
	pstMultiplexBufUtilDes->usLtwOffUpBound	= 0;
}


/************************************************************************/
/*	function : AllocMultiplexBufUtilDes									*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_MultiplexBufUtilDes_t 	*svc_cas_SiDesAllocMultiplexBufUtilDes(void)
{
	svcCas_MultiplexBufUtilDes_t	*pstMultiplexBufUtilDes;

	pstMultiplexBufUtilDes = (svcCas_MultiplexBufUtilDes_t *)OxCAS_Malloc(sizeof(svcCas_MultiplexBufUtilDes_t));
	if (pstMultiplexBufUtilDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocMultiplexBufUtilDes()\n");
		return NULL;
	}
	svc_cas_SiDesInitMultiplexBufUtilDes(pstMultiplexBufUtilDes);

	return ((svcCas_MultiplexBufUtilDes_t *)pstMultiplexBufUtilDes);
}


/************************************************************************/
/*	function : FreeMultiplexBufUtilDes									*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreeMultiplexBufUtilDes(svcCas_MultiplexBufUtilDes_t *pstMultiplexBufUtilDes)
{
	if(pstMultiplexBufUtilDes != NULL)
	{
		OxCAS_Free(pstMultiplexBufUtilDes);
	}
}


/************************************************************************/
/*	function : SetMultiplexBufUtilDes									*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetMultiplexBufUtilDes(svcCas_MultiplexBufUtilDes_t *pstMultiplexBufUtilDes, HBOOL bBoundValid, HUINT16 usLtwOffLowBound, HUINT16 usLtwOffUpBound)
{
	pstMultiplexBufUtilDes->bBoundValid			= bBoundValid;
	pstMultiplexBufUtilDes->usLtwOffLowBound	= usLtwOffLowBound;
	pstMultiplexBufUtilDes->usLtwOffUpBound		= usLtwOffUpBound;
}


/************************************************************************
 *
 * Copyright Descriptor 관련... (TAG = 0x0d)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitCopyrightDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitCopyrightDes(svcCas_CopyRightDes_t *pstCopyrightDes)
{
	pstCopyrightDes->ulCopyrightIdent	= 0;
	pstCopyrightDes->pPrivateDataInfo	= NULL;
}


/************************************************************************/
/*	function : AllocCopyrightDes										*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_CopyRightDes_t 	*svc_cas_SiDesAllocCopyrightDes(void)
{
	svcCas_CopyRightDes_t	*pstCopyrightDes;

	pstCopyrightDes = (svcCas_CopyRightDes_t *)OxCAS_Malloc(sizeof(svcCas_CopyRightDes_t));
	if (pstCopyrightDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocCopyrightDes()\n");
		return NULL;
	}
	svc_cas_SiDesInitCopyrightDes(pstCopyrightDes);

	return ((svcCas_CopyRightDes_t *)pstCopyrightDes);
}


/************************************************************************/
/*	function : FreeCopyrightDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreeCopyrightDes(svcCas_CopyRightDes_t *pstCopyrightDes)
{
	if(pstCopyrightDes == NULL)
	{
		return;
	}

	if (pstCopyrightDes->pPrivateDataInfo != NULL)
	{
		svc_cas_SiDesFreePrivateDataInfo(pstCopyrightDes->pPrivateDataInfo);
	}

	OxCAS_Free(pstCopyrightDes);
}


/************************************************************************/
/*	function : SetCopyrightDes											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetCopyrightDes(svcCas_CopyRightDes_t *pstCopyrightDes, HUINT32 ulCopyrightIdent, svcCas_PrivateDataInfo_t *pstPrivateDataInfo)
{
	pstCopyrightDes->ulCopyrightIdent	= ulCopyrightIdent;
	pstCopyrightDes->pPrivateDataInfo	= pstPrivateDataInfo;
}


/************************************************************************
 *
 * Maximum Bitrate Descriptor 관련... (TAG = 0x0e)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitMaxBitrateDes										*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitMaxBitrateDes(svcCas_MaxBitrateDes_t *pstMaxBitrateDes)
{
	pstMaxBitrateDes->ulMaxBitrate	= 0;
}


/************************************************************************/
/*	function : AllocMaxBitrateDes										*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_MaxBitrateDes_t 	*svc_cas_SiDesAllocMaxBitrateDes(void)
{
	svcCas_MaxBitrateDes_t	*pstMaxBitrateDes;

	pstMaxBitrateDes = (svcCas_MaxBitrateDes_t *)OxCAS_Malloc(sizeof(svcCas_MaxBitrateDes_t));
	if (pstMaxBitrateDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocMaxBitrateDes()\n");
		return NULL;
	}
	svc_cas_SiDesInitMaxBitrateDes(pstMaxBitrateDes);

	return ((svcCas_MaxBitrateDes_t *)pstMaxBitrateDes);
}


/************************************************************************/
/*	function : FreeMaxBitrateDes										*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreeMaxBitrateDes(svcCas_MaxBitrateDes_t *pstMaxBitrateDes)
{
	if(pstMaxBitrateDes != NULL)
	{
		OxCAS_Free(pstMaxBitrateDes);
	}
}


/************************************************************************/
/*	function : SetMaxBitrateDes											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetMaxBitrateDes(svcCas_MaxBitrateDes_t *pMaxBitrateDes, HUINT32 ulMaxBitrate)
{
	pMaxBitrateDes->ulMaxBitrate	= ulMaxBitrate;
}


/************************************************************************
 *
 * Private Data Indicator Descriptor 관련... (TAG = 0x0f)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitPrivateDataIndicatorDes								*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitPrivateDataIndicatorDes(svcCas_PrivateDataIndicatorDes_t *pstPrivateDataIndicatorDes)
{
	pstPrivateDataIndicatorDes->ulPrivateDataIndicatorDes = 0;
}


/************************************************************************/
/*	function : AllocPrivateDataIndicatorDes								*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_PrivateDataIndicatorDes_t 	*svc_cas_SiDesAllocPrivateDataIndicatorDes(void)
{
	svcCas_PrivateDataIndicatorDes_t	*pstPrivateDataIndicatorDes;

	pstPrivateDataIndicatorDes = (svcCas_PrivateDataIndicatorDes_t *)OxCAS_Malloc(sizeof(svcCas_PrivateDataIndicatorDes_t));
	if (pstPrivateDataIndicatorDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocPrivateDataIndicatorDes()\n");
		return NULL;
	}
	svc_cas_SiDesInitPrivateDataIndicatorDes(pstPrivateDataIndicatorDes);

	return ((svcCas_PrivateDataIndicatorDes_t *)pstPrivateDataIndicatorDes);
}


/************************************************************************/
/*	function : FreePrivateDataIndicatorDes								*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreePrivateDataIndicatorDes(svcCas_PrivateDataIndicatorDes_t *pstPrivateDataIndicatorDes)
{
	if(pstPrivateDataIndicatorDes != NULL)
	{
		OxCAS_Free(pstPrivateDataIndicatorDes);
	}
}


/************************************************************************/
/*	function : SetPrivateDataIndicatorDes								*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetPrivateDataIndicatorDes(svcCas_PrivateDataIndicatorDes_t *pstPrivateDataIndicatorDes, HUINT32 ulPrivateDataIndicatorDes)
{
	pstPrivateDataIndicatorDes->ulPrivateDataIndicatorDes = ulPrivateDataIndicatorDes;
}


/************************************************************************
 *
 * Smoothing Buffer Descriptor 관련... (TAG = 0x10)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitSmoothBufDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitSmoothBufDes(svcCas_SmoothBufDes_t *pstSmoothBufDes)
{
	pstSmoothBufDes->ulLeakRate 	= 0;
	pstSmoothBufDes->ulSize 		= 0;
}


/************************************************************************/
/*	function : AllocSmoothBufDes										*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_SmoothBufDes_t 	*svc_cas_SiDesAllocSmoothBufDes(void)
{
	svcCas_SmoothBufDes_t	*pstSmoothBufDes;

	pstSmoothBufDes = (svcCas_SmoothBufDes_t *)OxCAS_Malloc(sizeof(svcCas_SmoothBufDes_t));
	if (pstSmoothBufDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocSmoothBufDes()\n");
		return NULL;
	}
	svc_cas_SiDesInitSmoothBufDes(pstSmoothBufDes);

	return ((svcCas_SmoothBufDes_t *)pstSmoothBufDes);
}


/************************************************************************/
/*	function : FreeSmoothBufDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreeSmoothBufDes(svcCas_SmoothBufDes_t *pstSmoothBufDes)
{
	if(pstSmoothBufDes != NULL)
	{
		OxCAS_Free(pstSmoothBufDes);
	}
}


/************************************************************************/
/*	function : SetSmoothBufDes											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetSmoothBufDes(svcCas_SmoothBufDes_t *pstSmoothBufDes, HUINT32 ulLeakRate, HUINT32 ulSize)
{
	pstSmoothBufDes->ulLeakRate 	= ulLeakRate;
	pstSmoothBufDes->ulSize 		= ulSize;
}


/************************************************************************
 *
 * STD Descriptor 관련... (TAG = 0x11)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitStdDes												*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitStdDes(svcCas_StdDes_t *pstStdDes)
{
	pstStdDes->bLeakValid	= 0;
}


/************************************************************************/
/*	function : AllocStdDes												*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/
STATIC 	svcCas_StdDes_t 	*svc_cas_SiDesAllocStdDes(void)
{
	svcCas_StdDes_t	*pstStdDes;

	pstStdDes = (svcCas_StdDes_t *)OxCAS_Malloc(sizeof(svcCas_StdDes_t));
	if (pstStdDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocStdDes()\n");
		return NULL;
	}
	svc_cas_SiDesInitStdDes(pstStdDes);

	return ((svcCas_StdDes_t *)pstStdDes);
}


/************************************************************************/
/*	function : FreeStdDes												*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreeStdDes(svcCas_StdDes_t *pstStdDes)
{
	if(pstStdDes != NULL)
	{
		OxCAS_Free(pstStdDes);
	}
}


/************************************************************************/
/*	function : SetStdDes												*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetStdDes(svcCas_StdDes_t *pstStdDes, HBOOL bLeakValid)
{
	pstStdDes->bLeakValid	= bLeakValid;
}


/************************************************************************/
/*	function : svc_cas_SiDesInitCarouselIdDesInfo												*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitCarouselIdDesInfo(svcCas_CarouselIdDesInfo_t *pstCarouselIdDesInfo)
{
	pstCarouselIdDesInfo->ucModuleVersion = 0;
	pstCarouselIdDesInfo->usModuleId = 0;
	pstCarouselIdDesInfo->usBlockSize = 0;
	pstCarouselIdDesInfo->ulModuleSize = 0;
	pstCarouselIdDesInfo->ucCompressionMethod = 0;
	pstCarouselIdDesInfo->ulOriginalSize = 0;
	pstCarouselIdDesInfo->ucTimeout = 0;
	HxSTD_MemSet(pstCarouselIdDesInfo->aucObjectKeyData, 0, 4);
}


/************************************************************************/
/*	function : svc_cas_SiDesAllocCarouselIdDes										*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/
STATIC 	svcCas_CarouselIdDesInfo_t	*svc_cas_SiDesAllocCarouselIdDesInfo(void)
{
	svcCas_CarouselIdDesInfo_t	*pstCarouselIdDesInfo;

	pstCarouselIdDesInfo = (svcCas_CarouselIdDesInfo_t *)OxCAS_Malloc(sizeof(svcCas_CarouselIdDesInfo_t));
	if (pstCarouselIdDesInfo == NULL)
	{
		HxLOG_Print("[ERROR] svc_cas_SiDesAllocCarouselIdDesInfo()\n");
		return NULL;
	}
	svc_cas_SiDesInitCarouselIdDesInfo(pstCarouselIdDesInfo);

	return ((svcCas_CarouselIdDesInfo_t *)pstCarouselIdDesInfo);
}


/************************************************************************/
/*	function : svc_cas_SiDesFreeCarouselIdDesInfo									*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreeCarouselIdDesInfo(svcCas_CarouselIdDesInfo_t *pstCarouselIdDesInfo)
{
	if (pstCarouselIdDesInfo != NULL)
	{
		OxCAS_Free(pstCarouselIdDesInfo);
	}
}


/************************************************************************/
/*	function : svc_cas_SiSetCarouselIdDesInfo										*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetCarouselIdDesInfo(svcCas_CarouselIdDesInfo_t *pstCarouselIdDesInfo
	, HUINT8 ucModuleVersion, HUINT16 usModuleId,HUINT8 usBlockSize
	, HUINT32 ulModuleSize,HUINT8 ucCompressionMethod,HUINT32 ulOriginalSize
	, HUINT8 ucTimeout, HUINT8 aucObjectKeyData[])
{
	pstCarouselIdDesInfo->ucModuleVersion = ucModuleVersion;
	pstCarouselIdDesInfo->usModuleId = usModuleId;
	pstCarouselIdDesInfo->usBlockSize = usBlockSize;
	pstCarouselIdDesInfo->ulModuleSize = ulModuleSize;
	pstCarouselIdDesInfo->ucCompressionMethod = ucCompressionMethod;
	pstCarouselIdDesInfo->ulOriginalSize = ulOriginalSize;
	pstCarouselIdDesInfo->ucTimeout = ucTimeout;
	HxSTD_MemCopy(pstCarouselIdDesInfo->aucObjectKeyData, aucObjectKeyData, 4);
}


/************************************************************************/
/*	function : svc_cas_SiDesInitCarouselIdDes										*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitCarouselIdDes(svcCas_CarouselIdDes_t *pstCarouselIdDesInfo)
{
	pstCarouselIdDesInfo->ulCarouselId = 0;
	pstCarouselIdDesInfo->ucFormatId = 0;
	pstCarouselIdDesInfo->pCarouselIdDesInfo = NULL;
}


/************************************************************************/
/*	function : svc_cas_SiDesAllocCarouselIdDes										*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/
STATIC 	svcCas_CarouselIdDes_t 	*svc_cas_SiDesAllocCarouselIdDes(void)
{
	svcCas_CarouselIdDes_t	*pstCarouselIdDes;

	pstCarouselIdDes = (svcCas_CarouselIdDes_t *)OxCAS_Malloc(sizeof(svcCas_CarouselIdDes_t));
	if (pstCarouselIdDes == NULL)
	{
		HxLOG_Print("[ERROR] svc_cas_SiDesAllocCarouselIdDes()\n");
		return NULL;
	}
	svc_cas_SiDesInitCarouselIdDes(pstCarouselIdDes);

	return ((svcCas_CarouselIdDes_t *)pstCarouselIdDes);
}


/************************************************************************/
/*	function : svc_cas_SiDesFreeCarouselIdDes										*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreeCarouselIdDes(svcCas_CarouselIdDes_t *pstCarouselIdDes)
{
	if(pstCarouselIdDes == NULL)
	{
		return;
	}

	if(pstCarouselIdDes->pCarouselIdDesInfo)
	{
		svc_cas_SiDesFreeCarouselIdDesInfo(pstCarouselIdDes->pCarouselIdDesInfo);
	}

	OxCAS_Free(pstCarouselIdDes);
}



/************************************************************************/
/*	function : svc_cas_SiSetCarouselIdDes											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetCarouselIdDes(svcCas_CarouselIdDes_t *pstCarouselIdDes, HUINT32 ulCarouselId
	, HUINT8 ucFormatId, svcCas_CarouselIdDesInfo_t *pstCarouselIDesInfo)
{
	pstCarouselIdDes->ulCarouselId			= ulCarouselId;
	pstCarouselIdDes->ucFormatId 			= ucFormatId;
	pstCarouselIdDes->pCarouselIdDesInfo	= pstCarouselIDesInfo;
}


/************************************************************************/
/*	function : svc_cas_SiDesInitAssocTagDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitAssocTagDes(svcCas_AssocTagDes_t *pstAssocTagDes)
{
	pstAssocTagDes->usAssocTag = 0;
	pstAssocTagDes->usUse = 0;
	pstAssocTagDes->ulTransactionId = 0;
	pstAssocTagDes->ulTimeout = 0;
}


/************************************************************************/
/*	function : svc_cas_SiDesAllocAssocTagDes											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/
STATIC 	svcCas_AssocTagDes_t 	*svc_cas_SiDesAllocAssocTagDes(void)
{
	svcCas_AssocTagDes_t	*pstAssocTagDes;

	pstAssocTagDes = (svcCas_AssocTagDes_t *)OxCAS_Malloc(sizeof(svcCas_AssocTagDes_t));
	if (pstAssocTagDes == NULL)
	{
		HxLOG_Print("[ERROR] svc_cas_SiDesAllocAssocTagDes()\n");
		return NULL;
	}
	svc_cas_SiDesInitAssocTagDes(pstAssocTagDes);

	return ((svcCas_AssocTagDes_t *)pstAssocTagDes);
}


/************************************************************************/
/*	function : svc_cas_SiDesFreeAssocTagDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreeAssocTagDes(svcCas_AssocTagDes_t *pstAssocTagDes)
{
	if(pstAssocTagDes != NULL)
	{
		OxCAS_Free(pstAssocTagDes);
	}
}


/************************************************************************/
/*	function : svc_cas_SiSetAssocTagDes											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetAssocTagDes(svcCas_AssocTagDes_t *pstAssocTagDes, HUINT16 usAssocTag
	, HUINT16 usUse, HUINT32 ulTransactionId, HUINT32 ulTimeout)
{
	pstAssocTagDes->usAssocTag			= usAssocTag;
	pstAssocTagDes->usUse				= usUse;
	pstAssocTagDes->ulTransactionId		= ulTransactionId;
	pstAssocTagDes->ulTimeout			= ulTimeout;
}

/************************************************************************/
/*	function : InitDeferAssocTagDes										*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitAvcVideoDes(svcCas_AvcVideoDes_t *pstAvcVideoDes)
{
	HxSTD_MemSet(pstAvcVideoDes, 0, sizeof(svcCas_AvcVideoDes_t));
}


/************************************************************************/
/*	function : svc_cas_SiDesAllocDeferAssocTagDes									*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/
STATIC 	svcCas_AvcVideoDes_t 	*svc_cas_SiDesAllocAvcVideoDes(void)
{
	svcCas_AvcVideoDes_t	*pstAvcVideoDes;

	pstAvcVideoDes = (svcCas_AvcVideoDes_t *)OxCAS_Malloc(sizeof(svcCas_AvcVideoDes_t));
	if (pstAvcVideoDes == NULL)
	{
		HxLOG_Print("[ERROR] svc_cas_SiDesAllocAvcVideoDes()\n");
		return NULL;
	}
	svc_cas_SiDesInitAvcVideoDes(pstAvcVideoDes);

	return ((svcCas_AvcVideoDes_t *)pstAvcVideoDes);
}


/************************************************************************/
/*	function : svc_cas_SiDesFreeDeferAssocTagDes										*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreeAvcVideoDes(svcCas_AvcVideoDes_t *pstAvcVideoDes)
{
	if(pstAvcVideoDes != NULL)
	{
		OxCAS_Free(pstAvcVideoDes);
	}
}


/************************************************************************/
/*	function : InitDeferAssocTagDes										*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesInitDeferAssocTagDes(svcCas_DeferAssocTagDes_t *pstDeferAssocTagDes)
{
	pstDeferAssocTagDes->ucAssocTagSize 		= 0;
	pstDeferAssocTagDes->pusAssocTags 			= NULL;
	pstDeferAssocTagDes->usTransportStreamId 	= 0;
	pstDeferAssocTagDes->usProgNum 				= 0;
	pstDeferAssocTagDes->usOriginalNetworkId 	= 0;
}


/************************************************************************/
/*	function : svc_cas_SiDesAllocDeferAssocTagDes									*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/
STATIC 	svcCas_DeferAssocTagDes_t 	*svc_cas_SiDesAllocDeferAssocTagDes(void)
{
	svcCas_DeferAssocTagDes_t	*pstDeferAssocTagDes;

	pstDeferAssocTagDes = (svcCas_DeferAssocTagDes_t *)OxCAS_Malloc(sizeof(svcCas_DeferAssocTagDes_t));
	if (pstDeferAssocTagDes == NULL)
	{
		HxLOG_Print("[ERROR] svc_cas_SiDesAllocDeferAssocTagDes()\n");
		return NULL;
	}
	svc_cas_SiDesInitDeferAssocTagDes(pstDeferAssocTagDes);

	return ((svcCas_DeferAssocTagDes_t *)pstDeferAssocTagDes);
}


/************************************************************************/
/*	function : svc_cas_SiDesFreeDeferAssocTagDes										*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreeDeferAssocTagDes(svcCas_DeferAssocTagDes_t *pstDeferAssocTagDes)
{
	if(pstDeferAssocTagDes != NULL)
	{
		OxCAS_Free(pstDeferAssocTagDes);
	}
}


/************************************************************************/
/*	function : SetDeferAssocTagDes										*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetDeferAssocTagDes(svcCas_DeferAssocTagDes_t *pstDeferAssocTagDes
	, HUINT8 ucAssocTagSize, HUINT16 *pusAssocTags, HUINT16 usTransportStreamId
	, HUINT16 usProgNum, HUINT16 usOriginalNetworkId)
{
	pstDeferAssocTagDes->ucAssocTagSize = ucAssocTagSize;
	HxSTD_MemCopy(pstDeferAssocTagDes->pusAssocTags, pusAssocTags, ucAssocTagSize);
	pstDeferAssocTagDes->usTransportStreamId = usTransportStreamId;
	pstDeferAssocTagDes->usProgNum = usProgNum;
	pstDeferAssocTagDes->usOriginalNetworkId = usOriginalNetworkId;
}


/************************************************************************
 *
 * Network Name Descriptor 관련... (TAG = 0x40)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitNetNameDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesIinitNetNameDes(svcCas_NetNameDes_t *pstNetNameDes)
{
	pstNetNameDes->pszNetName = NULL;
}


/************************************************************************/
/*	function : AllocNetNameDes											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_NetNameDes_t 	*svc_cas_SiDesAllocNetNameDes(void)
{
	svcCas_NetNameDes_t	*pstNetNameDes;

	pstNetNameDes = (svcCas_NetNameDes_t *)OxCAS_Malloc(sizeof(svcCas_NetNameDes_t));
	if (pstNetNameDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocNetNameDes()\n");
		return NULL;
	}
	svc_cas_SiDesIinitNetNameDes(pstNetNameDes);

	return ((svcCas_NetNameDes_t *)pstNetNameDes);
}

/************************************************************************/
/*	function : FreeNetNameDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	svc_cas_SiDesFreeNetNameDes(svcCas_NetNameDes_t *pstNetNameDes)
{
	if(pstNetNameDes == NULL)
	{
		return;
	}

	if(pstNetNameDes->pszNetName)
	{
		svc_cas_SiFreeMem(pstNetNameDes->pszNetName);
	}
	OxCAS_Free(pstNetNameDes);
}


/************************************************************************/
/*	function : SetNetNameDes											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetNetNameDes(svcCas_NetNameDes_t *pstNetNameDes, HUINT8 *pszNetName)
{
	if(pszNetName != NULL)
		HxSTD_MemCopy(pstNetNameDes->pszNetName, pszNetName, svc_cas_SiUtilStrlen(pszNetName));
	pstNetNameDes->pszNetName[svc_cas_SiUtilStrlen(pszNetName)] = '\0';
}


/************************************************************************
 *
 * Service List Descriptor 관련... (TAG = 0x42)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitServListInfo											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initServListInfo(svcCas_SvcListInfo_t *pServListInfo)
{
	pServListInfo->usSvcId 		= 0;
	pServListInfo->ucServType 	= 0;
	pServListInfo->pNextInfo 	= NULL;
}


/************************************************************************/
/*	function : AllocServListInfo										*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_SvcListInfo_t 	*allocServListInfo(void)
{
	svcCas_SvcListInfo_t	*pServListInfo;

	pServListInfo = (svcCas_SvcListInfo_t *)OxCAS_Malloc(sizeof(svcCas_SvcListInfo_t));
	if (pServListInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocServListInfo()\n");
		return NULL;
	}
	initServListInfo(pServListInfo);

	return ((svcCas_SvcListInfo_t *)pServListInfo);
}


/************************************************************************/
/*	function : FreeServListInfo											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeServListInfo(svcCas_SvcListInfo_t *pServListInfo)
{
	if(pServListInfo != NULL)
	{
		OxCAS_Free(pServListInfo);
	}
}


/************************************************************************/
/*	function : SetServListInfo											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetServListInfo(svcCas_SvcListInfo_t *pstServListInfo, HUINT16 usServId, HUINT8 ucServType)
{
	pstServListInfo->usSvcId 		= usServId;
	pstServListInfo->ucServType	= ucServType;
}


/************************************************************************/
/*	function : GetPrevServListInfo										*/
/*  																	*/
/*  Description : 마지막 작업한 Info의 poHUINT32er를 얻는다.				*/
/************************************************************************/

svcCas_SvcListInfo_t 	*svc_cas_SiGetPrevServListInfo(svcCas_SvcListDes_t *pstServListDes)
{
	svcCas_SvcListInfo_t	*pstServListInfo;


	pstServListInfo = pstServListDes->pServListInfo;
	if (pstServListInfo == NULL)
	{
		return NULL;
	}

	while (pstServListInfo->pNextInfo != NULL)
	{
		pstServListInfo = pstServListInfo->pNextInfo;
	}

	return ((svcCas_SvcListInfo_t *)pstServListInfo);
}


/************************************************************************/
/*	function : LinkServListInfoHead										*/
/*  																	*/
/*  Description : head의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkServListInfoHead(svcCas_SvcListDes_t *pstServListDes, svcCas_SvcListInfo_t *pstServListInfo)
{
	pstServListDes->pServListInfo = pstServListInfo;
}


/************************************************************************/
/*	function : LinkServListInfoNode										*/
/*  																	*/
/*  Description : node의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkServListInfoNode(svcCas_SvcListInfo_t *pstNewServListInfo, svcCas_SvcListInfo_t *pstNextServListInfo, svcCas_SvcListInfo_t *pstPrevServListInfo)
{
	if (pstPrevServListInfo)
	{
		pstPrevServListInfo->pNextInfo = pstNewServListInfo;
	}
	pstNewServListInfo->pNextInfo = pstNextServListInfo;
}


/************************************************************************/
/*	function : InitServListDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initServListDes(svcCas_SvcListDes_t *pstServListDes)
{
	pstServListDes->pServListInfo = NULL;
}


/************************************************************************/
/*	function : AllocServListDes											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_SvcListDes_t 	*allocServListDes(void)
{
	svcCas_SvcListDes_t	*pServListDes;

	pServListDes = (svcCas_SvcListDes_t *)OxCAS_Malloc(sizeof(svcCas_SvcListDes_t));
	if (pServListDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocServListDes()\n");
		return NULL;
	}
	initServListDes(pServListDes);

	return ((svcCas_SvcListDes_t *)pServListDes);
}


/************************************************************************/
/*	function : FreeServListDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeServListDes(svcCas_SvcListDes_t *pServListDes)
{
	svcCas_SvcListInfo_t	*pstIndexServListInfo, *pstDestServListInfo;

	if(pServListDes == NULL)
	{
		return;
	}

	pstIndexServListInfo = pServListDes->pServListInfo;
	while (pstIndexServListInfo)
	{
		pstDestServListInfo = pstIndexServListInfo;
		pstIndexServListInfo = pstIndexServListInfo->pNextInfo;
		if(pstDestServListInfo)
		{
			freeServListInfo(pstDestServListInfo);
		}
	}

	OxCAS_Free(pServListDes);
}


/************************************************************************
 *
 * Satellite Delivery System Descriptor 관련... (TAG = 0x43)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitSatDeliSystemDes										*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initSatDeliSystemDes(svcCas_SatDeliSystemDes_t *pSatDeliSystemDes)
{
	HxSTD_MemSet(pSatDeliSystemDes, 0, sizeof(svcCas_SatDeliSystemDes_t));
	pSatDeliSystemDes->ulFreq 		= 0;
	pSatDeliSystemDes->usOrbitPos 	= 0;
	pSatDeliSystemDes->bWestEastFlag 	= FALSE;
	pSatDeliSystemDes->ucPolar 		= 0;
	pSatDeliSystemDes->ucModul 		= 0;
	pSatDeliSystemDes->ulSymbolRate 	= 0;
	pSatDeliSystemDes->ucFECInner 	= 0;
	/* DVB S2 */
	pSatDeliSystemDes->ucTransSpec	= 0;
	pSatDeliSystemDes->ucRollOff		= 0;
}


/************************************************************************/
/*	function : AllocSatDeliSystemDes									*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_SatDeliSystemDes_t 	*allocSatDeliSystemDes(void)
{
	svcCas_SatDeliSystemDes_t	*pSatDeliSystemDes;

	pSatDeliSystemDes = (svcCas_SatDeliSystemDes_t *)OxCAS_Malloc(sizeof(svcCas_SatDeliSystemDes_t));
	if (pSatDeliSystemDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocSatDeliSystemDes()\n");
		return NULL;
	}
	initSatDeliSystemDes(pSatDeliSystemDes);

	return ((svcCas_SatDeliSystemDes_t *)pSatDeliSystemDes);
}


/************************************************************************/
/*	function : FreeSatDeliSystemDes										*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeSatDeliSystemDes(svcCas_SatDeliSystemDes_t *pSatDeliSystemDes)
{
	if(pSatDeliSystemDes != NULL)
	{
		OxCAS_Free(pSatDeliSystemDes);
	}
}


/************************************************************************/
/*	function : SetSatDeliSystemDes										*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/
void svc_cas_SiSetSatDeliSystemDes(svcCas_SatDeliSystemDes_t *pSatDeliSystemDes, HUINT32 ulFreq, HUINT16 usOrbitPos, HBOOL bWestEastFlag, HUINT8 ucPolar, HUINT8 ucModul, HUINT8 ucModType, HUINT32 ulSymbolRate, HUINT8 ucFECInner, HUINT8 ucTransSpec, HUINT8 ucRollOff)
{
	pSatDeliSystemDes->ulFreq 			= ulFreq;
	pSatDeliSystemDes->usOrbitPos 		= usOrbitPos;
	pSatDeliSystemDes->bWestEastFlag 	= bWestEastFlag;
	pSatDeliSystemDes->ucPolar 			= ucPolar;
	pSatDeliSystemDes->ucModul 			= ucModul;
	pSatDeliSystemDes->ucModType		= ucModType;
	pSatDeliSystemDes->ulSymbolRate 	= ulSymbolRate;
	pSatDeliSystemDes->ucFECInner 		= ucFECInner;
	/* DVB S2 */
	pSatDeliSystemDes->ucTransSpec 		= ucTransSpec;
	pSatDeliSystemDes->ucRollOff		= ucRollOff;
}



/************************************************************************
 *
 * Cable Delivery System Descriptor 관련... (TAG = 0x44)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitCabDeliSystemDes										*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initCabDeliSystemDes(svcCas_CabDeliSystemDes_t *pstCabDeliSystemDes)
{
	pstCabDeliSystemDes->ulFreq 		= 0;
	pstCabDeliSystemDes->ucFECOuter		= 0;
	pstCabDeliSystemDes->ucModul 		= 0;
	pstCabDeliSystemDes->ulSymbolRate	= 0;
	pstCabDeliSystemDes->ucFECInner 	= 0;
}


/************************************************************************/
/*	function : AllocCabDeliSystemDes									*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_CabDeliSystemDes_t 	*allocCabDeliSystemDes(void)
{
	svcCas_CabDeliSystemDes_t	*pstCabDeliSystemDes;

	pstCabDeliSystemDes = (svcCas_CabDeliSystemDes_t *)OxCAS_Malloc(sizeof(svcCas_CabDeliSystemDes_t));
	if (pstCabDeliSystemDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocCabDeliSystemDes()\n");
		return NULL;
	}
	initCabDeliSystemDes(pstCabDeliSystemDes);

	return ((svcCas_CabDeliSystemDes_t *)pstCabDeliSystemDes);
}


/************************************************************************/
/*	function : FreeCabDeliSystemDes										*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeCabDeliSystemDes(svcCas_CabDeliSystemDes_t *pstCabDeliSystemDes)
{
	if(pstCabDeliSystemDes != NULL)
	{
		OxCAS_Free(pstCabDeliSystemDes);
	}
}


/************************************************************************/
/*	function : SetCabDeliSystemDes										*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetCabDeliSystemDes(svcCas_CabDeliSystemDes_t *pstCabDeliSystemDes, HUINT32 ulFreq, HUINT8 ucFECOuter, HUINT8 ucModul, HUINT32 ulSymbolRate, HUINT8 ucFECInner)
{
	pstCabDeliSystemDes->ulFreq 		= ulFreq;
	pstCabDeliSystemDes->ucFECOuter 	= ucFECOuter;
	pstCabDeliSystemDes->ucModul 		= ucModul;
	pstCabDeliSystemDes->ulSymbolRate	= ulSymbolRate;
	pstCabDeliSystemDes->ucFECInner 	= ucFECInner;
}


/************************************************************************
 *
 * Bouquet Name Descriptor 관련... (TAG = 0x47)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitBouqNameDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initBouqNameDes(svcCas_BouqNameDes_t *pstBouqNameDes)
{
	pstBouqNameDes->pszBouqName = NULL;
}


/************************************************************************/
/*	function : AllocBouqNameDes											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_BouqNameDes_t 	*allocBouqNameDes(void)
{
	svcCas_BouqNameDes_t	*pstBouqNameDes;

	pstBouqNameDes = (svcCas_BouqNameDes_t *)OxCAS_Malloc(sizeof(svcCas_BouqNameDes_t));
	if (pstBouqNameDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocBouqNameDes()\n");
		return NULL;
	}
	initBouqNameDes(pstBouqNameDes);

	return ((svcCas_BouqNameDes_t *)pstBouqNameDes);
}


/************************************************************************/
/*	function : FreeBouqNameDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeBouqNameDes(svcCas_BouqNameDes_t *pstBouqNameDes)
{
	if(pstBouqNameDes == NULL)
	{
		return;
	}

	if(pstBouqNameDes->pszBouqName)
	{
		svc_cas_SiFreeMem(pstBouqNameDes->pszBouqName);
	}
	OxCAS_Free(pstBouqNameDes);
}



/************************************************************************/
/*	function : SetBouqNameDes											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetBouqNameDes(svcCas_BouqNameDes_t *pstBouqNameDes, HUINT8 *pszBouqName)
{
    if(pszBouqName != NULL)
    	HxSTD_MemCopy(pstBouqNameDes->pszBouqName, pszBouqName, svc_cas_SiUtilStrlen(pszBouqName));
	pstBouqNameDes->pszBouqName[svc_cas_SiUtilStrlen(pszBouqName)] = '\0';
}


/************************************************************************
 *
 * Service Descriptor 관련... (TAG = 0x48)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitServDes												*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initServDes(svcCas_SvcDes_t *pstServDes)
{
	pstServDes->ucServType 			= 0;
    pstServDes->pszProviderName 	= NULL;
    pstServDes->pszServName			= NULL;
}


/************************************************************************/
/*	function : AllocServDes												*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_SvcDes_t 	*allocServDes(void)
{
	svcCas_SvcDes_t	*pstServDes;

	pstServDes = (svcCas_SvcDes_t *)OxCAS_Malloc(sizeof(svcCas_SvcDes_t));
	if (pstServDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocServDes()\n");
		return NULL;
	}
	initServDes(pstServDes);

	return ((svcCas_SvcDes_t *)pstServDes);
}


/************************************************************************/
/*	function : FreeServDes												*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeServDes(svcCas_SvcDes_t *pstServDes)
{
	if(pstServDes == NULL)
	{
		return;
	}

	if(pstServDes->pszProviderName)
	{
		svc_cas_SiFreeMem(pstServDes->pszProviderName);
	}

	if(pstServDes->pszServName)
	{
		svc_cas_SiFreeMem(pstServDes->pszServName);
	}

	OxCAS_Free(pstServDes);
}


/************************************************************************/
/*	function : SetServDes												*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetServDes(svcCas_SvcDes_t *pstServDes, HUINT8 ucServType, HUINT8 *pszProviderName, HUINT8 *pszServName)
{
	pstServDes->ucServType = ucServType;
	if(pszProviderName != NULL)
	    HxSTD_MemCopy(pstServDes->pszProviderName, pszProviderName, svc_cas_SiUtilStrlen(pszProviderName));
	pstServDes->pszProviderName[svc_cas_SiUtilStrlen(pszProviderName)] = '\0';
	if(pszServName != NULL)
	    HxSTD_MemCopy(pstServDes->pszServName, pszServName, svc_cas_SiUtilStrlen(pszServName));
	pstServDes->pszServName[svc_cas_SiUtilStrlen(pszServName)] = '\0';
}


/************************************************************************
 *
 * Country Availability Descriptor 관련... (TAG = 0x49)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitCountryAvailInfo										*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initCountryAvailInfo(svcCas_CountryAvailInfo_t *pstCountryAvailInfo)
{
	HxSTD_MemSet(pstCountryAvailInfo->szCountryAvailCode, '\0', 4);
	pstCountryAvailInfo->pNextInfo = NULL;
}


/************************************************************************/
/*	function : AllocCountryAvailInfo									*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_CountryAvailInfo_t 	*allocCountryAvailInfo(void)
{
	svcCas_CountryAvailInfo_t	*pstCountryAvailInfo;

	pstCountryAvailInfo = (svcCas_CountryAvailInfo_t *)OxCAS_Malloc(sizeof(svcCas_CountryAvailInfo_t));
	if (pstCountryAvailInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocCountryAvailInfo()\n");
		return NULL;
	}
	initCountryAvailInfo(pstCountryAvailInfo);

	return ((svcCas_CountryAvailInfo_t *)pstCountryAvailInfo);
}


/************************************************************************/
/*	function : FreeCountryAvailInfo										*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeCountryAvailInfo(svcCas_CountryAvailInfo_t *pstCountryAvailInfo)
{
	if(pstCountryAvailInfo != NULL)
	{
		OxCAS_Free(pstCountryAvailInfo);
	}
}


/************************************************************************/
/*	function : SetCountryAvailInfo										*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetCountryAvailInfo(svcCas_CountryAvailInfo_t *pstCountryAvailInfo, HUINT8 *pszCountryAvailCode)
{
	HxSTD_MemCopy(pstCountryAvailInfo->szCountryAvailCode, pszCountryAvailCode, svc_cas_SiUtilStrlen(pszCountryAvailCode));
	pstCountryAvailInfo->szCountryAvailCode[svc_cas_SiUtilStrlen(pszCountryAvailCode)] = '\0';
}


/************************************************************************/
/*	function : GetPrevCountryAvailInfo									*/
/*  																	*/
/*  Description : 마지막 작업한 Info의 poHUINT32er를 얻는다.				*/
/************************************************************************/

svcCas_CountryAvailInfo_t 	*svc_cas_SiGetPrevCountryAvailInfo(svcCas_CountryAvailDes_t *pstCountryAvailDes)
{
	svcCas_CountryAvailInfo_t	*pstCountryAvailInfo;


	pstCountryAvailInfo = pstCountryAvailDes->pCountryAvailInfo;
	if (pstCountryAvailInfo == NULL)
	{
		return NULL;
	}

	while (pstCountryAvailInfo->pNextInfo != NULL)
	{
		pstCountryAvailInfo = pstCountryAvailInfo->pNextInfo;
	}

	return ((svcCas_CountryAvailInfo_t *)pstCountryAvailInfo);
}


/************************************************************************/
/*	function : LinkCountryAvailInfoHead									*/
/*  																	*/
/*  Description : head의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkCountryAvailInfoHead(svcCas_CountryAvailDes_t *pstCountryAvailDes, svcCas_CountryAvailInfo_t *pstCountryAvailInfo)
{
	pstCountryAvailDes->pCountryAvailInfo = pstCountryAvailInfo;
}


/************************************************************************/
/*	function : LinkCountryAvailInfoNode									*/
/*  																	*/
/*  Description : node의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkCountryAvailInfoNode(svcCas_CountryAvailInfo_t *pstNewCountryAvailInfo, svcCas_CountryAvailInfo_t *pstNextCountryAvailInfo, svcCas_CountryAvailInfo_t *pstPrevCountryAvailInfo)
{
	if (pstPrevCountryAvailInfo)
	{
		pstPrevCountryAvailInfo->pNextInfo = pstNewCountryAvailInfo;
	}
	pstNewCountryAvailInfo->pNextInfo = pstNextCountryAvailInfo;
}


/************************************************************************/
/*	function : InitCountryAvailDes										*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initCountryAvailDes(svcCas_CountryAvailDes_t *pstCountryAvailDes)
{
	pstCountryAvailDes->bCountryAvail 	= FALSE;
	pstCountryAvailDes->pCountryAvailInfo = NULL;
}


/************************************************************************/
/*	function : AllocCountryAvailDes										*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_CountryAvailDes_t 	*allocCountryAvailDes(void)
{
	svcCas_CountryAvailDes_t	*pstCountryAvailDes;

	pstCountryAvailDes = (svcCas_CountryAvailDes_t *)OxCAS_Malloc(sizeof(svcCas_CountryAvailDes_t));
	if (pstCountryAvailDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocCountryAvailDes()\n");
		return NULL;
	}
	initCountryAvailDes(pstCountryAvailDes);

	return ((svcCas_CountryAvailDes_t *)pstCountryAvailDes);
}


/************************************************************************/
/*	function : FreeCountryAvailDes										*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeCountryAvailDes(svcCas_CountryAvailDes_t *pstCountryAvailDes)
{
	svcCas_CountryAvailInfo_t	*pstIndexCountryAvailInfo, *pstDestCountryAvailInfo;

	if(pstCountryAvailDes == NULL)
	{
		return;
	}

	pstIndexCountryAvailInfo = pstCountryAvailDes->pCountryAvailInfo;
	while (pstIndexCountryAvailInfo)
	{
		pstDestCountryAvailInfo = pstIndexCountryAvailInfo;
		pstIndexCountryAvailInfo = pstIndexCountryAvailInfo->pNextInfo;
		if(pstDestCountryAvailInfo)
		{
			freeCountryAvailInfo(pstDestCountryAvailInfo);
		}
	}

	OxCAS_Free(pstCountryAvailDes);
}


/************************************************************************
 *
 * Linkage Descriptor 관련... (TAG = 0x4a)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitLinkDes												*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initLinkDes(svcCas_LinkDes_t *pstLinkDes)
{
	HxSTD_MemSet(pstLinkDes, 0, sizeof(svcCas_LinkDes_t));
}


/************************************************************************/
/*	function : AllocLinkDes												*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_LinkDes_t 	*allocLinkDes(void)
{
	svcCas_LinkDes_t	*pstLinkDes;

	pstLinkDes = (svcCas_LinkDes_t *)OxCAS_Malloc(sizeof(svcCas_LinkDes_t));
	if (pstLinkDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocLinkDes()\n");
		return NULL;
	}
	initLinkDes(pstLinkDes);

	return ((svcCas_LinkDes_t *)pstLinkDes);
}


/************************************************************************/
/*	function : FreeLinkDes												*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeLinkDes(svcCas_LinkDes_t *pstLinkDes)
{

	if (pstLinkDes == NULL)
	{
		return;
	}

	if (pstLinkDes->pPrivateDataInfo != NULL)
	{
		svc_cas_SiDesFreePrivateDataInfo(pstLinkDes->pPrivateDataInfo);
	}

	OxCAS_Free(pstLinkDes);
}


/************************************************************************
 *
 * NVOD refernence Descriptor 관련... (TAG = 0x4b)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitNvodRefInfo											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initNvodRefInfo(svcCas_NvodRefInfo_t *pstNvodRefInfo)
{
	pstNvodRefInfo->usTsId 		= 0;
	pstNvodRefInfo->usOrgNetId 	= 0;
	pstNvodRefInfo->usSvcId 	= 0;
	pstNvodRefInfo->pNextInfo 	= NULL;
}


/************************************************************************/
/*	function : AllocNvodRefInfo											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_NvodRefInfo_t 	*allocNvodRefInfo(void)
{
	svcCas_NvodRefInfo_t	*pstNvodRefInfo;

	pstNvodRefInfo = (svcCas_NvodRefInfo_t *)OxCAS_Malloc(sizeof(svcCas_NvodRefInfo_t));
	if (pstNvodRefInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocNvodRefInfo()\n");
		return NULL;
	}
	initNvodRefInfo(pstNvodRefInfo);

	return ((svcCas_NvodRefInfo_t *)pstNvodRefInfo);
}


/************************************************************************/
/*	function : FreeCountryInfo											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeNvodRefInfo(svcCas_NvodRefInfo_t *pstNvodRefInfo)
{
	if(pstNvodRefInfo != NULL)
	{
		OxCAS_Free(pstNvodRefInfo);
	}
}

/************************************************************************/
/*	function : SetNvodRefInfo											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetNvodRefInfo(svcCas_NvodRefInfo_t *pstNvodRefInfo, HUINT16 usTsId
	, HUINT16 usOrgNetId, HUINT16 usSvcId)
{
	pstNvodRefInfo->usTsId 		= usTsId;
	pstNvodRefInfo->usOrgNetId 	= usOrgNetId;
	pstNvodRefInfo->usSvcId 		= usSvcId;
}


/************************************************************************/
/*	function : GetPrevNvodRefInfo										*/
/*  																	*/
/*  Description : 마지막 작업한 Info의 poHUINT32er를 얻는다.				*/
/************************************************************************/

svcCas_NvodRefInfo_t 	*svc_cas_SiGetPrevNvodRefInfo(svcCas_NvodRefDes_t *pstNvodRefDes)
{
	svcCas_NvodRefInfo_t	*pstNvodRefInfo;


	pstNvodRefInfo = pstNvodRefDes->pNvodRefInfo;
	if (pstNvodRefInfo == NULL)
	{
		return NULL;
	}

	while (pstNvodRefInfo->pNextInfo != NULL)
	{
		pstNvodRefInfo = pstNvodRefInfo->pNextInfo;
	}

	return ((svcCas_NvodRefInfo_t *)pstNvodRefInfo);
}


/************************************************************************/
/*	function : LinkNvodRefInfoHead										*/
/*  																	*/
/*  Description : head의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkNvodRefInfoHead(svcCas_NvodRefDes_t *pstNvodRefDes, svcCas_NvodRefInfo_t *pstNvodRefInfo)
{
	pstNvodRefDes->pNvodRefInfo = pstNvodRefInfo;
}


/************************************************************************/
/*	function : LinkNvodRefInfoNode										*/
/*  																	*/
/*  Description : node의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkNvodRefInfoNode(svcCas_NvodRefInfo_t *pstNewNvodRefInfo, svcCas_NvodRefInfo_t *pstNextNvodRefInfo, svcCas_NvodRefInfo_t *pstPrevNvodRefInfo)
{
	if (pstPrevNvodRefInfo)
	{
		pstPrevNvodRefInfo->pNextInfo = pstNewNvodRefInfo;
	}

	pstNewNvodRefInfo->pNextInfo = pstNextNvodRefInfo;
}


/************************************************************************/
/*	function : InitNvodRefDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initNvodRefDes(svcCas_NvodRefDes_t *pstNvodRefDes)
{
	pstNvodRefDes->pNvodRefInfo = NULL;
}


/************************************************************************/
/*	function : AllocNvodRefDes											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_NvodRefDes_t 	*allocNvodRefDes(void)
{
	svcCas_NvodRefDes_t	*pstNvodRefDes;

	pstNvodRefDes = (svcCas_NvodRefDes_t *)OxCAS_Malloc(sizeof(svcCas_NvodRefDes_t));
	if (pstNvodRefDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocNvodRefDes()\n");
		return NULL;
	}
	initNvodRefDes(pstNvodRefDes);

	return ((svcCas_NvodRefDes_t *)pstNvodRefDes);
}


/************************************************************************/
/*	function : FreeNvodRefDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeNvodRefDes(svcCas_NvodRefDes_t *pstNvodRefDes)
{
	svcCas_NvodRefInfo_t	*pstIndexNvodRefInfo, *pstDestNvodRefInfo;

	if(pstNvodRefDes == NULL)
	{
		return;
	}

	pstIndexNvodRefInfo = pstNvodRefDes->pNvodRefInfo;
	while (pstIndexNvodRefInfo)
	{
		pstDestNvodRefInfo = pstIndexNvodRefInfo;
		pstIndexNvodRefInfo = pstIndexNvodRefInfo->pNextInfo;
		if(pstDestNvodRefInfo)
		{
			freeNvodRefInfo(pstDestNvodRefInfo);
		}
	}

	OxCAS_Free(pstNvodRefDes);
}



/************************************************************************
 *
 * Time Shifted Service Descriptor 관련... (TAG = 0x4c)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitTimeShiftServDes										*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initTimeShiftServDes(svcCas_TimeShiftSvcDes_t *pstTimeShiftServDes)
{
	pstTimeShiftServDes->usRefSvcId = 0;
}


/************************************************************************/
/*	function : AllocTimeShiftServDes									*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_TimeShiftSvcDes_t 	*allocTimeShiftServDes(void)
{
	svcCas_TimeShiftSvcDes_t	*pstTimeShiftServDes;

	pstTimeShiftServDes = (svcCas_TimeShiftSvcDes_t *)OxCAS_Malloc(sizeof(svcCas_TimeShiftSvcDes_t));
	if (pstTimeShiftServDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocTimeShiftServDes()\n");
		return NULL;
	}
	initTimeShiftServDes(pstTimeShiftServDes);

	return ((svcCas_TimeShiftSvcDes_t *)pstTimeShiftServDes);
}


/************************************************************************/
/*	function : FreeTimeShiftServDes										*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeTimeShiftServDes(svcCas_TimeShiftSvcDes_t *pstTimeShiftServDes)
{
	if(pstTimeShiftServDes != NULL)
	{
		OxCAS_Free(pstTimeShiftServDes);
	}
}


/************************************************************************/
/*	function : SetTimeShiftServDes										*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetTimeShiftServDes(svcCas_TimeShiftSvcDes_t *pstTimeShiftServDes, HUINT16 usRefServId)
{
	pstTimeShiftServDes->usRefSvcId = usRefServId;
}


/************************************************************************
 *
 * Short Event Descriptor 관련... (TAG = 0x4d)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitShortEvtDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initShortEvtDes(svcCas_ShortEventDes_t *pstShortEvtDes)
{
	HxSTD_MemSet(pstShortEvtDes->szIso639LangCode, '\0', 4);
	pstShortEvtDes->pszEvtName 			= NULL;
	pstShortEvtDes->pszText 				= NULL;
}


/************************************************************************/
/*	function : AllocShortEvtDes											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_ShortEventDes_t 	*allocShortEvtDes(void)
{
	svcCas_ShortEventDes_t	*pstShortEvtDes;

	pstShortEvtDes = (svcCas_ShortEventDes_t *)OxCAS_Malloc(sizeof(svcCas_ShortEventDes_t));
	if (pstShortEvtDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocShortEvtDes()\n");
		return NULL;
	}
	initShortEvtDes(pstShortEvtDes);

	return ((svcCas_ShortEventDes_t *)pstShortEvtDes);
}


/************************************************************************/
/*	function : FreeShortEvtDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeShortEvtDes(svcCas_ShortEventDes_t *pstShortEvtDes)
{
	if(pstShortEvtDes == NULL)
	{
		return;
	}

	if(pstShortEvtDes->pszEvtName)
	{
		svc_cas_SiFreeMem(pstShortEvtDes->pszEvtName);
	}

	if(pstShortEvtDes->pszText)
	{
		svc_cas_SiFreeMem(pstShortEvtDes->pszText);
	}

	OxCAS_Free(pstShortEvtDes);
}

/************************************************************************/
/*	function : SetShortEvtDes											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetShortEvtDes(svcCas_ShortEventDes_t *pstShortEvtDes, HUINT8 *pszIso639LangCode, HUINT8 *pszEvtName, HUINT8 *pszText)
{
    if(pszIso639LangCode != NULL)
    	HxSTD_MemCopy(pstShortEvtDes->szIso639LangCode, pszIso639LangCode, 3);
	pstShortEvtDes->szIso639LangCode[3] = '\0';
    if(pszEvtName != NULL)
	    HxSTD_MemCopy(pstShortEvtDes->pszEvtName, pszEvtName, svc_cas_SiUtilStrlen(pszEvtName));
	pstShortEvtDes->pszEvtName[svc_cas_SiUtilStrlen(pszEvtName)] = '\0';
    if(pszText != NULL)
	    HxSTD_MemCopy(pstShortEvtDes->pszText, pszText, svc_cas_SiUtilStrlen(pszText));
	pstShortEvtDes->pszText[svc_cas_SiUtilStrlen(pszText)] = '\0';
}

/************************************************************************
 *
 * Extended Event Descriptor 관련... (TAG = 0x4e)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitExtEvtInfo											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initExtEvtInfo(svcCas_ExtEvtInfo_t *pstExtEvtInfo)
{
	pstExtEvtInfo->pszDescription	= NULL;
	pstExtEvtInfo->pszItem 			= NULL;
	pstExtEvtInfo->pNextInfo 		= NULL;
}


/************************************************************************/
/*	function : AllocExtEvtInfo											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_ExtEvtInfo_t 	*allocExtEvtInfo(void)
{
	svcCas_ExtEvtInfo_t	*pstExtEvtInfo;

	pstExtEvtInfo = (svcCas_ExtEvtInfo_t *)OxCAS_Malloc(sizeof(svcCas_ExtEvtInfo_t));
	if (pstExtEvtInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocExtEvtInfo()\n");
		return NULL;
	}
	initExtEvtInfo(pstExtEvtInfo);

	return ((svcCas_ExtEvtInfo_t *)pstExtEvtInfo);
}


/************************************************************************/
/*	function : FreeExtEvtInfo											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeExtEvtInfo(svcCas_ExtEvtInfo_t *pstExtEvtInfo)
{
	if(pstExtEvtInfo == NULL)
	{
		return;
	}

	if(pstExtEvtInfo->pszDescription)
	{
		svc_cas_SiFreeMem(pstExtEvtInfo->pszDescription);
	}
	if(pstExtEvtInfo->pszItem)
	{
		svc_cas_SiFreeMem(pstExtEvtInfo->pszItem);
	}

	OxCAS_Free(pstExtEvtInfo);
}

/************************************************************************/
/*	function : SetExtEvtInfo											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetExtEvtInfo(svcCas_ExtEvtInfo_t *pstExtEvtInfo, HUINT8 *pszDescription, HUINT8 *pszItem)
{
    if(pszDescription != NULL)
	    HxSTD_MemCopy(pstExtEvtInfo->pszDescription, pszDescription, svc_cas_SiUtilStrlen(pszDescription));
	pstExtEvtInfo->pszDescription[svc_cas_SiUtilStrlen(pszDescription)] = '\0';
	if(pszItem != NULL)
	    HxSTD_MemCopy(pstExtEvtInfo->pszItem, pszItem, svc_cas_SiUtilStrlen(pszItem));
	pstExtEvtInfo->pszItem[svc_cas_SiUtilStrlen(pszItem)] = '\0';
}


/************************************************************************/
/*	function : GetPrevExtEvtInfo										*/
/*  																	*/
/*  Description : 마지막 작업한 Info의 poHUINT32er를 얻는다.				*/
/************************************************************************/

svcCas_ExtEvtInfo_t 	*svc_cas_SiGetPrevExtEvtInfo(svcCas_ExtEvtDes_t *pstExtEvtDes)
{
	svcCas_ExtEvtInfo_t	*pstExtEvtInfo;

	pstExtEvtInfo = pstExtEvtDes->pExtEvtInfo;
	if (pstExtEvtInfo == NULL)
	{
		return NULL;
	}

	while (pstExtEvtInfo->pNextInfo != NULL)
	{
		pstExtEvtInfo = pstExtEvtInfo->pNextInfo;
	}

	return ((svcCas_ExtEvtInfo_t *)pstExtEvtInfo);
}


/************************************************************************/
/*	function : LinkExtEvtInfoHead										*/
/*  																	*/
/*  Description : head의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkExtEvtInfoHead(svcCas_ExtEvtDes_t *pstExtEvtDes, svcCas_ExtEvtInfo_t *pstExtEvtInfo)
{
	pstExtEvtDes->pExtEvtInfo = pstExtEvtInfo;
}


/************************************************************************/
/*	function : LinkExtEvtInfoNode										*/
/*  																	*/
/*  Description : node의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkExtEvtInfoNode(svcCas_ExtEvtInfo_t *pstNewExtEvtInfo, svcCas_ExtEvtInfo_t *pstNextExtEvtInfo, svcCas_ExtEvtInfo_t *pstPrevExtEvtInfo)
{
	if (pstPrevExtEvtInfo)
	{
		pstPrevExtEvtInfo->pNextInfo = pstNewExtEvtInfo;
	}

	pstNewExtEvtInfo->pNextInfo = pstNextExtEvtInfo;
}


/************************************************************************/
/*	function : InitExtEvtDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initExtEvtDes(svcCas_ExtEvtDes_t *pstExtEvtDes)
{
	pstExtEvtDes->ucDesNum		= 0;
	pstExtEvtDes->ucLastDesNum	= 0;
	HxSTD_MemSet(pstExtEvtDes->szIso639LangCode, '\0', 4);
	pstExtEvtDes->pExtEvtInfo 	= NULL;
	pstExtEvtDes->pszChar		= NULL;
}


/************************************************************************/
/*	function : AllocExtEvtDes											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/


STATIC 	svcCas_ExtEvtDes_t 	*allocExtEvtDes(void)
{
	svcCas_ExtEvtDes_t	*pstExtEvtDes;

	pstExtEvtDes = (svcCas_ExtEvtDes_t *)OxCAS_Malloc(sizeof(svcCas_ExtEvtDes_t));
	if (pstExtEvtDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocExtEvtDes()\n");
		return NULL;
	}
	initExtEvtDes(pstExtEvtDes);

	return ((svcCas_ExtEvtDes_t *)pstExtEvtDes);
}


/************************************************************************/
/*	function : FreeExtEvtDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeExtEvtDes(svcCas_ExtEvtDes_t *pstExtEvtDes)
{
	svcCas_ExtEvtInfo_t	*pstIndexExtEvtInfo, *pstDestExtEvtInfo;

	if(pstExtEvtDes == NULL)
	{
		return;
	}

	pstIndexExtEvtInfo = pstExtEvtDes->pExtEvtInfo;
	while (pstIndexExtEvtInfo)
	{
		pstDestExtEvtInfo = pstIndexExtEvtInfo;
		pstIndexExtEvtInfo = pstIndexExtEvtInfo->pNextInfo;
		if(pstDestExtEvtInfo)
		{
			freeExtEvtInfo(pstDestExtEvtInfo);
		}
	}

	if(pstExtEvtDes->pucTextChar)
	{
		svc_cas_SiFreeMem(pstExtEvtDes->pucTextChar);
	}

	if(pstExtEvtDes->pszChar)
	{
		svc_cas_SiFreeMem(pstExtEvtDes->pszChar);
	}

	OxCAS_Free(pstExtEvtDes);
}


/************************************************************************/
/*	function : SetExtEvtDes												*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetExtEvtDes(svcCas_ExtEvtDes_t *pstExtEvtDes, HUINT8 ucDesNum, HUINT8 ucLastDesNum, HUINT8 *pszIso639LangCode, HUINT8 *pszChar)
{
	pstExtEvtDes->ucDesNum = ucDesNum;
	pstExtEvtDes->ucLastDesNum = ucLastDesNum;
	if(pszIso639LangCode != NULL)
	    HxSTD_MemCopy(pstExtEvtDes->szIso639LangCode, pszIso639LangCode, svc_cas_SiUtilStrlen(pszIso639LangCode));
	pstExtEvtDes->szIso639LangCode[svc_cas_SiUtilStrlen(pszIso639LangCode)] = '\0';
	if(pszChar != NULL)
	    HxSTD_MemCopy(pstExtEvtDes->pszChar, pszChar, svc_cas_SiUtilStrlen(pszChar));
	pstExtEvtDes->pszChar[svc_cas_SiUtilStrlen(pszChar)] = '\0';
}



/************************************************************************
 *
 * Time Shifted Event Descriptor 관련... (TAG = 0x4f)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitTimeShiftEvtDes										*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initTimeShiftEvtDes(svcCas_TimeShiftEvtDes_t *pstTimeShiftEvtDes)
{
	pstTimeShiftEvtDes->usRefSvcId 	= 0;
	pstTimeShiftEvtDes->usRefEvtId 	= 0;
}


/************************************************************************/
/*	function : AllocTimeShiftEvtDes										*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_TimeShiftEvtDes_t 	*allocTimeShiftEvtDes(void)
{
	svcCas_TimeShiftEvtDes_t	*pstTimeShiftEvtDes;

	pstTimeShiftEvtDes = (svcCas_TimeShiftEvtDes_t *)OxCAS_Malloc(sizeof(svcCas_TimeShiftEvtDes_t));
	if (pstTimeShiftEvtDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocTimeShiftEvtDes()\n");
		return NULL;
	}
	initTimeShiftEvtDes(pstTimeShiftEvtDes);

	return ((svcCas_TimeShiftEvtDes_t *)pstTimeShiftEvtDes);
}


/************************************************************************/
/*	function : FreeTimeShiftEvtDes										*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeTimeShiftEvtDes(svcCas_TimeShiftEvtDes_t *pstTimeShiftEvtDes)
{
	if(pstTimeShiftEvtDes != NULL)
	{
		OxCAS_Free(pstTimeShiftEvtDes);
	}
}


/************************************************************************/
/*	function : SetTimeShiftEvtDes										*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetTimeShiftEvtDes(svcCas_TimeShiftEvtDes_t *pstTimeShiftEvtDes, HUINT16 usRefServId, HUINT16 usRefEvtId)
{
	pstTimeShiftEvtDes->usRefSvcId 	= usRefServId;
	pstTimeShiftEvtDes->usRefEvtId 	= usRefEvtId;
}


/************************************************************************
 *
 * Component Descriptor 관련... (TAG = 0x50)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitCompDes												*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initCompDes(svcCas_CompDes_t *pstCompDes)
{
	pstCompDes->ucStmContent 	= 0;
	pstCompDes->ucCompType	 	= 0;
	pstCompDes->ucCompTag 		= 0;
	HxSTD_MemSet(pstCompDes->szIso639LangCode, '\0', 4);
	pstCompDes->pszText 		= NULL;
}


/************************************************************************/
/*	function : AllocCompDes												*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_CompDes_t 	*allocCompDes(void)
{
	svcCas_CompDes_t	*pstCompDes;

	pstCompDes = (svcCas_CompDes_t *)OxCAS_Malloc(sizeof(svcCas_CompDes_t));
	if (pstCompDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocCompDes()\n");
		return NULL;
	}
	initCompDes(pstCompDes);

	return ((svcCas_CompDes_t *)pstCompDes);
}


/************************************************************************/
/*	function : FreeCompDes												*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeCompDes(svcCas_CompDes_t *pstCompDes)
{
	if(pstCompDes == NULL)
	{
		return;
	}

	if(pstCompDes->pszText)
	{
		svc_cas_SiFreeMem(pstCompDes->pszText);
	}

	OxCAS_Free(pstCompDes);
}


/************************************************************************/
/*	function : SetCompDes												*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetCompDes(svcCas_CompDes_t *pstCompDes, HUINT8 ucStmContent, HUINT8 ucCompType, HUINT8 ucCompTag, HUINT8 *pszIso639LangCode, HUINT8 *pszText)
{
	pstCompDes->ucStmContent = ucStmContent;
	pstCompDes->ucCompType = ucCompType;
	pstCompDes->ucCompTag = ucCompTag;
	if(pszIso639LangCode != NULL)
    	HxSTD_MemCopy(pstCompDes->szIso639LangCode, pszIso639LangCode, svc_cas_SiUtilStrlen(pszIso639LangCode));
	pstCompDes->szIso639LangCode[svc_cas_SiUtilStrlen(pszIso639LangCode)] = '\0';
	if(pszText != NULL)
    	HxSTD_MemCopy(pstCompDes->pszText, pszText, svc_cas_SiUtilStrlen(pszText));
	pstCompDes->pszText[svc_cas_SiUtilStrlen(pszText)] = '\0';
}


/************************************************************************
 *
 * Stream Identifier Descriptor 관련... (TAG = 0x52)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitStreamIdentDes										*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initStmIdentDes(svcCas_StmIdentDes_t *pstStmIdentDes)
{
	pstStmIdentDes->ucCompTag = 0;
}


/************************************************************************/
/*	function : AllocStmIdentDes											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_StmIdentDes_t 	*allocStmIdentDes(void)
{
	svcCas_StmIdentDes_t	*pstStmIdentDes;

	pstStmIdentDes = (svcCas_StmIdentDes_t *)OxCAS_Malloc(sizeof(svcCas_StmIdentDes_t));
	if (pstStmIdentDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocStmIdentDes()\n");
		return NULL;
	}
	initStmIdentDes(pstStmIdentDes);

	return ((svcCas_StmIdentDes_t *)pstStmIdentDes);
}


/************************************************************************/
/*	function : FreeStmIdentDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeStmIdentDes(svcCas_StmIdentDes_t *pstStmIdentDes)
{
	if(pstStmIdentDes != NULL)
	{
		OxCAS_Free(pstStmIdentDes);
	}
}


/************************************************************************/
/*	function : SetStmIdentDes											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetStmIdentDes(svcCas_StmIdentDes_t *pstStmIdentDes, HUINT8 ucCompTag)
{
	pstStmIdentDes->ucCompTag = ucCompTag;
}


/************************************************************************
 *
 * CA Identifier Descriptor 관련... (TAG = 0x53)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitCaIdentInfo											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initCaIdentInfo(svcCas_CaIdentInfo_t *pstCaIdentInfo)
{
	pstCaIdentInfo->usCaStmId 	= 0;
	pstCaIdentInfo->pNextInfo 	= NULL;
}


/************************************************************************/
/*	function : AllocCaIdentInfo											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_CaIdentInfo_t 	*allocCaIdentInfo(void)
{
	svcCas_CaIdentInfo_t	*pstCaIdentInfo;

	pstCaIdentInfo = (svcCas_CaIdentInfo_t *)OxCAS_Malloc(sizeof(svcCas_CaIdentInfo_t));
	if (pstCaIdentInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocCaIdentInfo()\n");
		return NULL;
	}
	initCaIdentInfo(pstCaIdentInfo);

	return ((svcCas_CaIdentInfo_t *)pstCaIdentInfo);
}


/************************************************************************/
/*	function : FreeCaIdentInfo											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeCaIdentInfo(svcCas_CaIdentInfo_t *pstCaIdentInfo)
{
	if(pstCaIdentInfo != NULL)
	{
		OxCAS_Free(pstCaIdentInfo);
	}
}


/************************************************************************/
/*	function : SetCaIdentInfo											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetCaIdentInfo(svcCas_CaIdentInfo_t *pstCaIdentInfo, HUINT16 usCaStmId)
{
	pstCaIdentInfo->usCaStmId = usCaStmId;
}


/************************************************************************/
/*	function : GetPrevCaIdentInfo										*/
/*  																	*/
/*  Description : 마지막 작업한 Info의 poHUINT32er를 얻는다.				*/
/************************************************************************/

svcCas_CaIdentInfo_t 	*svc_cas_SiGetPrevCaIdentInfo(svcCas_CaIdentDes_t *pstCaIdentDes)
{
	svcCas_CaIdentInfo_t	*pstCaIdentInfo;


	pstCaIdentInfo = pstCaIdentDes->pCaIdentInfo;
	if (pstCaIdentInfo == NULL)
	{
		return NULL;
	}

	while (pstCaIdentInfo->pNextInfo != NULL)
	{
		pstCaIdentInfo = pstCaIdentInfo->pNextInfo;
	}

	return ((svcCas_CaIdentInfo_t *)pstCaIdentInfo);
}


/************************************************************************/
/*	function : LinkCaIdentInfoHead										*/
/*  																	*/
/*  Description : head의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkCaIdentInfoHead(svcCas_CaIdentDes_t *pstCaIdentDes, svcCas_CaIdentInfo_t *pstCaIdentInfo)
{
	pstCaIdentDes->pCaIdentInfo = pstCaIdentInfo;
}


/************************************************************************/
/*	function : LinkCaIdentInfoNode										*/
/*  																	*/
/*  Description : node의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkCaIdentInfoNode(svcCas_CaIdentInfo_t *pstNewCaIdentInfo, svcCas_CaIdentInfo_t *pstNextCaIdentInfo, svcCas_CaIdentInfo_t *pstPrevCaIdentInfo)
{
	if (pstPrevCaIdentInfo)
	{
		pstPrevCaIdentInfo->pNextInfo = pstNewCaIdentInfo;
	}

	pstNewCaIdentInfo->pNextInfo = pstNextCaIdentInfo;
}


/************************************************************************/
/*	function : InitCaIdentDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initCaIdentDes(svcCas_CaIdentDes_t *pstCaIdentDes)
{
	pstCaIdentDes->pCaIdentInfo = NULL;
}


/************************************************************************/
/*	function : AllocCaIdentDes											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_CaIdentDes_t 	*allocCaIdentDes(void)
{
	svcCas_CaIdentDes_t	*pstCaIdentDes;

	pstCaIdentDes = (svcCas_CaIdentDes_t *)OxCAS_Malloc(sizeof(svcCas_CaIdentDes_t));
	if (pstCaIdentDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocCaIdentDes()\n");
		return NULL;
	}
	initCaIdentDes(pstCaIdentDes);

	return ((svcCas_CaIdentDes_t *)pstCaIdentDes);
}


/************************************************************************/
/*	function : FreeCaIdentDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeCaIdentDes(svcCas_CaIdentDes_t *pstCaIdentDes)
{
	svcCas_CaIdentInfo_t	*pstIndexCaIdentInfo, *pstDestCaIdentInfo;

	if(pstCaIdentDes == NULL)
	{
		return;
	}

	pstIndexCaIdentInfo = pstCaIdentDes->pCaIdentInfo;
	while (pstIndexCaIdentInfo)
	{
		pstDestCaIdentInfo = pstIndexCaIdentInfo;
		pstIndexCaIdentInfo = pstIndexCaIdentInfo->pNextInfo;
		if(pstDestCaIdentInfo)
		{
			freeCaIdentInfo(pstDestCaIdentInfo);
		}
	}

	OxCAS_Free(pstCaIdentDes);
}


/************************************************************************
 *
 * Content Descriptor 관련... (TAG = 0x54)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitContentInfo											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initContentInfo(svcCas_ContentInfo_t *pstContentInfo)
{
	pstContentInfo->ucContentNibble		= 0;
	pstContentInfo->ucUserNibble		= 0;
	pstContentInfo->pNextInfo			= NULL;
}


/************************************************************************/
/*	function : AllocContentInfo											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_ContentInfo_t 	*allocContentInfo(void)
{
	svcCas_ContentInfo_t	*pstContentInfo;

	pstContentInfo = (svcCas_ContentInfo_t *)OxCAS_Malloc(sizeof(svcCas_ContentInfo_t));
	if (pstContentInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocContentInfo()\n");
		return NULL;
	}
	initContentInfo(pstContentInfo);

	return ((svcCas_ContentInfo_t *)pstContentInfo);
}


/************************************************************************/
/*	function : FreeContentInfo											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeContentInfo(svcCas_ContentInfo_t *pstContentInfo)
{
	if(pstContentInfo != NULL)
	{
		OxCAS_Free(pstContentInfo);
	}
}


/************************************************************************/
/*	function : SetContentInfo											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetContentInfo(svcCas_ContentInfo_t *pstContentInfo, HUINT8 ucContentNibble, HUINT8 ucUserNibble)
{
	pstContentInfo->ucContentNibble	= ucContentNibble;
	pstContentInfo->ucUserNibble		= ucUserNibble;
}


/************************************************************************/
/*	function : GetPrevContentInfo										*/
/*  																	*/
/*  Description : 마지막 작업한 Info의 poHUINT32er를 얻는다.				*/
/************************************************************************/

svcCas_ContentInfo_t 	*svc_cas_SiGetPrevContentInfo(svcCas_ContentDes_t *pstContentDes)
{
	svcCas_ContentInfo_t	*pstContentInfo;


	pstContentInfo = pstContentDes->pContentInfo;
	if (pstContentInfo == NULL)
	{
		return NULL;
	}

	while (pstContentInfo->pNextInfo != NULL)
	{
		pstContentInfo = pstContentInfo->pNextInfo;
	}

	return ((svcCas_ContentInfo_t *)pstContentInfo);
}


/************************************************************************/
/*	function : LinkContentInfoHead										*/
/*  																	*/
/*  Description : head의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkContentInfoHead(svcCas_ContentDes_t *pstContentDes, svcCas_ContentInfo_t *pstContentInfo)
{
	pstContentDes->pContentInfo = pstContentInfo;
}


/************************************************************************/
/*	function : LinkContentInfoNode										*/
/*  																	*/
/*  Description : node의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkContentInfoNode(svcCas_ContentInfo_t *pstNewContentInfo, svcCas_ContentInfo_t *pstNextContentInfo, svcCas_ContentInfo_t *pstPrevContentInfo)
{
	if (pstPrevContentInfo)
	{
		pstPrevContentInfo->pNextInfo = pstNewContentInfo;
	}

	pstNewContentInfo->pNextInfo = pstNextContentInfo;
}


/************************************************************************/
/*	function : InitContentDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initContentDes(svcCas_ContentDes_t *pstContentDes)
{
	pstContentDes->pContentInfo = NULL;
}


/************************************************************************/
/*	function : AllocContentDes											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_ContentDes_t 	*allocContentDes(void)
{
	svcCas_ContentDes_t	*pstContentDes;

	pstContentDes = (svcCas_ContentDes_t *)OxCAS_Malloc(sizeof(svcCas_ContentDes_t));
	if (pstContentDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocContentDes()\n");
		return NULL;
	}
	initContentDes(pstContentDes);

	return ((svcCas_ContentDes_t *)pstContentDes);
}


/************************************************************************/
/*	function : FreeContentDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeContentDes(svcCas_ContentDes_t *pstContentDes)
{
	svcCas_ContentInfo_t	*pstIndexContentInfo, *pstDestContentInfo;

	if(pstContentDes == NULL)
	{
		return;
	}

	pstIndexContentInfo = pstContentDes->pContentInfo;
	while (pstIndexContentInfo)
	{
		pstDestContentInfo = pstIndexContentInfo;
		pstIndexContentInfo = pstIndexContentInfo->pNextInfo;
		freeContentInfo(pstDestContentInfo);
	}

	OxCAS_Free(pstContentDes);
}



/************************************************************************
 *
 * Parental Rating Descriptor 관련... (TAG = 0x55)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitParentInfo											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initParentInfo(svcCas_ParentInfo_t *pstParentInfo)
{
	HxSTD_MemSet(pstParentInfo->szCountryCode, '\0', 4);
	pstParentInfo->ucRate		= 0;
	pstParentInfo->pNextInfo	= NULL;
}


/************************************************************************/
/*	function : FreeParentInfo											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeParentInfo(svcCas_ParentInfo_t *pstParentInfo)
{
	if(pstParentInfo != NULL)
	{
		OxCAS_Free(pstParentInfo);
	}
}

/************************************************************************/
/*	function : AllocParentInfo											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_ParentInfo_t 	*allocParentInfo(void)
{
	svcCas_ParentInfo_t	*pstParentInfo;

	pstParentInfo = (svcCas_ParentInfo_t *)OxCAS_Malloc(sizeof(svcCas_ParentInfo_t));
	if (pstParentInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocParentInfo()\n");
		return NULL;
	}
	initParentInfo(pstParentInfo);

	return ((svcCas_ParentInfo_t *)pstParentInfo);
}


/************************************************************************/
/*	function : SetParentInfo											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetParentInfo(svcCas_ParentInfo_t *pstParentInfo, HUINT8 *pszCountryCode, HUINT8 ucRate)
{
	HxSTD_MemCopy(pstParentInfo->szCountryCode, pszCountryCode, svc_cas_SiUtilStrlen(pszCountryCode));
	pstParentInfo->szCountryCode[svc_cas_SiUtilStrlen(pszCountryCode)] = '\0';
	pstParentInfo->ucRate = ucRate;
}


/************************************************************************/
/*	function : GetPrevParentInfo										*/
/*  																	*/
/*  Description : 마지막 작업한 Info의 poHUINT32er를 얻는다.				*/
/************************************************************************/

svcCas_ParentInfo_t 	*svc_cas_SiGetPrevParentInfo(svcCas_ParentDes_t *pstParentDes)
{
	svcCas_ParentInfo_t	*pstParentInfo;


	pstParentInfo = pstParentDes->pParentInfo;
	if (pstParentInfo == NULL)
	{
		return NULL;
	}

	while (pstParentInfo->pNextInfo != NULL)
	{
		pstParentInfo = pstParentInfo->pNextInfo;
	}

	return ((svcCas_ParentInfo_t *)pstParentInfo);
}


/************************************************************************/
/*	function : LinkParentInfoHead										*/
/*  																	*/
/*  Description : head의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkParentInfoHead(svcCas_ParentDes_t *pstParentDes, svcCas_ParentInfo_t *pstParentInfo)
{
	pstParentDes->pParentInfo = pstParentInfo;
}


/************************************************************************/
/*	function : LinkParentInfoNode										*/
/*  																	*/
/*  Description : node의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkParentInfoNode(svcCas_ParentInfo_t *pstNewParentInfo, svcCas_ParentInfo_t *pstNextParentInfo, svcCas_ParentInfo_t *pstPrevParentInfo)
{
	if (pstPrevParentInfo)
	{
		pstPrevParentInfo->pNextInfo = pstNewParentInfo;
	}

	pstNewParentInfo->pNextInfo = pstNextParentInfo;
}


/************************************************************************/
/*	function : InitParentDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initParentDes(svcCas_ParentDes_t *pstParentDes)
{
	pstParentDes->pParentInfo = NULL;
}


/************************************************************************/
/*	function : AllocParentDes											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_ParentDes_t 	*allocParentDes(void)
{
	svcCas_ParentDes_t	*pstParentDes;

	pstParentDes = (svcCas_ParentDes_t *)OxCAS_Malloc(sizeof(svcCas_ParentDes_t));
	if (pstParentDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocParentDes()\n");
		return NULL;
	}
	initParentDes(pstParentDes);

	return ((svcCas_ParentDes_t *)pstParentDes);
}


/************************************************************************/
/*	function : FreeParentDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeParentDes(svcCas_ParentDes_t *pstParentDes)
{
	svcCas_ParentInfo_t	*pstIndexParentInfo, *pstDestParentInfo;

	if(pstParentDes == NULL)
	{
		return;
	}

	pstIndexParentInfo = pstParentDes->pParentInfo;
	while (pstIndexParentInfo)
	{
		pstDestParentInfo = pstIndexParentInfo;
		pstIndexParentInfo = pstIndexParentInfo->pNextInfo;
		freeParentInfo(pstDestParentInfo);
	}

	OxCAS_Free(pstParentDes);
}


/************************************************************************
 *
 * Teletext Descriptor 관련... (TAG = 0x56)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitTeletextInfo											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initTeletextInfo(svcCas_TeletextInfo_t *pstTeletextInfo)
{
	HxSTD_MemSet(pstTeletextInfo->szIso639LangCode, '\0', 4);
	pstTeletextInfo->ucType 		= 0;
	pstTeletextInfo->ucMagazineNum 	= 0;
	pstTeletextInfo->ucPageNum 		= 0;
	pstTeletextInfo->pNextInfo 		= NULL;
}


/************************************************************************/
/*	function : AllocTeletextInfo										*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_TeletextInfo_t 	*allocTeletextInfo(void)
{
	svcCas_TeletextInfo_t	*pstTeletextInfo;

	pstTeletextInfo = (svcCas_TeletextInfo_t *)OxCAS_Malloc(sizeof(svcCas_TeletextInfo_t));
	if (pstTeletextInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocTeletextInfo()\n");
		return NULL;
	}
	initTeletextInfo(pstTeletextInfo);

	return ((svcCas_TeletextInfo_t *)pstTeletextInfo);
}


/************************************************************************/
/*	function : FreeTeletextInfo											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeTeletextInfo(svcCas_TeletextInfo_t *pstTeletextInfo)
{
	if(pstTeletextInfo != NULL)
	{
		OxCAS_Free(pstTeletextInfo);
	}
}


/************************************************************************/
/*	function : SetTeletextInfo											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetTeletextInfo(svcCas_TeletextInfo_t *pstTeletextInfo, HUINT8 *pszIso639LangCode, HUINT8 ucType, HUINT8 ucMagazineNum, HUINT8 ucPageNum)
{
	HxSTD_MemCopy(pstTeletextInfo->szIso639LangCode, pszIso639LangCode, svc_cas_SiUtilStrlen(pszIso639LangCode));
	pstTeletextInfo->szIso639LangCode[svc_cas_SiUtilStrlen(pszIso639LangCode)] = '\0';
	pstTeletextInfo->ucType 			= ucType;
	pstTeletextInfo->ucMagazineNum 	= ucMagazineNum;
	pstTeletextInfo->ucPageNum 		= ucPageNum;
}


/************************************************************************/
/*	function : GetPrevTeletextInfo										*/
/*  																	*/
/*  Description : 마지막 작업한 Info의 poHUINT32er를 얻는다.				*/
/************************************************************************/

svcCas_TeletextInfo_t 	*svc_cas_SiGetPrevTeletextInfo(svcCas_TeletextDes_t *pstTeletextDes)
{
	svcCas_TeletextInfo_t	*pstTeletextInfo;


	pstTeletextInfo = pstTeletextDes->pTeletextInfo;
	if (pstTeletextInfo == NULL)
	{
		return NULL;
	}

	while (pstTeletextInfo->pNextInfo != NULL)
	{
		pstTeletextInfo = pstTeletextInfo->pNextInfo;
	}

	return ((svcCas_TeletextInfo_t *)pstTeletextInfo);
}


/************************************************************************/
/*	function : LinkTeletextInfoHead										*/
/*  																	*/
/*  Description : head의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkTeletextInfoHead(svcCas_TeletextDes_t *pstTeletextDes, svcCas_TeletextInfo_t *pstTeletextInfo)
{
	pstTeletextDes->pTeletextInfo = pstTeletextInfo;
}


/************************************************************************/
/*	function : LinkTeletextInfoNode										*/
/*  																	*/
/*  Description : node의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkTeletextInfoNode(svcCas_TeletextInfo_t *pstNewTeletextInfo, svcCas_TeletextInfo_t *pstNextTeletextInfo, svcCas_TeletextInfo_t *pstPrevTeletextInfo)
{
	if (pstPrevTeletextInfo)
	{
		pstPrevTeletextInfo->pNextInfo = pstNewTeletextInfo;
	}

	pstNewTeletextInfo->pNextInfo = pstNextTeletextInfo;
}

/************************************************************************/
/*	function : InitTeletextDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initTeletextDes(svcCas_TeletextDes_t *pstTeletextDes)
{
	pstTeletextDes->pTeletextInfo = NULL;
}


/************************************************************************/
/*	function : AllocTeletextDes											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_TeletextDes_t 	*allocTeletextDes(void)
{
	svcCas_TeletextDes_t	*pstTeletextDes;

	pstTeletextDes = (svcCas_TeletextDes_t *)OxCAS_Malloc(sizeof(svcCas_TeletextDes_t));
	if (pstTeletextDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocTeletextDes()\n");
		return NULL;
	}
	initTeletextDes(pstTeletextDes);

	return ((svcCas_TeletextDes_t *)pstTeletextDes);
}


/************************************************************************/
/*	function : FreeTeletextDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeTeletextDes(svcCas_TeletextDes_t *pstTeletextDes)
{
	svcCas_TeletextInfo_t	*pstIndexTeletextInfo, *pstDestTeletextInfo;

	if(pstTeletextDes == NULL)
	{
		return;
	}

	pstIndexTeletextInfo = pstTeletextDes->pTeletextInfo;
	while (pstIndexTeletextInfo)
	{
		pstDestTeletextInfo = pstIndexTeletextInfo;
		pstIndexTeletextInfo = pstIndexTeletextInfo->pNextInfo;
		freeTeletextInfo(pstDestTeletextInfo);
	}

	OxCAS_Free(pstTeletextDes);
}



/************************************************************************
 *
 * Telephone Descriptor 관련... (TAG = 0x57)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitTelephoneDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initTelephoneDes(svcCas_TelephoneDes_t *pstTelephoneDes)
{
	pstTelephoneDes->ucForeignAvail 			= 0;
	pstTelephoneDes->ucConnectionType 			= 0;
	pstTelephoneDes->pszCountryPrefix 			= NULL;
	pstTelephoneDes->pszInternationalAreaCode	= NULL;
	pstTelephoneDes->pszOperatorCode 			= NULL;
	pstTelephoneDes->pszNationalAreaCode		= NULL;
	pstTelephoneDes->pszCoreNumber 				= NULL;
}


/************************************************************************/
/*	function : AllocTelephoneDes										*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_TelephoneDes_t 	*allocTelephoneDes(void)
{
	svcCas_TelephoneDes_t	*pstTelephoneDes;

	pstTelephoneDes = (svcCas_TelephoneDes_t *)OxCAS_Malloc(sizeof(svcCas_TelephoneDes_t));
	if (pstTelephoneDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocTelephoneDes()\n");
		return NULL;
	}
	initTelephoneDes(pstTelephoneDes);

	return ((svcCas_TelephoneDes_t *)pstTelephoneDes);
}


/************************************************************************/
/*	function : FreeTelephoneDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeTelephoneDes(svcCas_TelephoneDes_t *pstTelephoneDes)
{
	if(pstTelephoneDes == NULL)
	{
		return;
	}

	if(pstTelephoneDes->pszCountryPrefix)
	{
		svc_cas_SiFreeMem(pstTelephoneDes->pszCountryPrefix);
	}

	if(pstTelephoneDes->pszInternationalAreaCode)
	{
		svc_cas_SiFreeMem(pstTelephoneDes->pszInternationalAreaCode);
	}

	if(pstTelephoneDes->pszOperatorCode)
	{
		svc_cas_SiFreeMem(pstTelephoneDes->pszOperatorCode);
	}

	if(pstTelephoneDes->pszNationalAreaCode)
	{
		svc_cas_SiFreeMem(pstTelephoneDes->pszNationalAreaCode);
	}

	if(pstTelephoneDes->pszCoreNumber)
	{
		svc_cas_SiFreeMem(pstTelephoneDes->pszCoreNumber);
	}

	OxCAS_Free(pstTelephoneDes);
}


/************************************************************************/
/*	function : SetTelephoneDes											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetTelephoneDes(svcCas_TelephoneDes_t *pstTelephoneDes, HUINT8 ucForeignAvail, HUINT8 ucConnectionType, HUINT8 *pszCountryPrefix, HUINT8 *pszInternationalAreaCode, HUINT8 *pszOperatorCode, HUINT8 *pszNationalAreaCode, HUINT8 *pszCoreNumber)
{
	pstTelephoneDes->ucForeignAvail = ucForeignAvail;
	pstTelephoneDes->ucConnectionType = ucConnectionType;
	if(pszCountryPrefix != NULL)
	    HxSTD_MemCopy(pstTelephoneDes->pszCountryPrefix, pszCountryPrefix, svc_cas_SiUtilStrlen(pszCountryPrefix));
	pstTelephoneDes->pszCountryPrefix[svc_cas_SiUtilStrlen(pszCountryPrefix)] = '\0';
	if(pszInternationalAreaCode != NULL)
	    HxSTD_MemCopy(pstTelephoneDes->pszInternationalAreaCode, pszInternationalAreaCode, svc_cas_SiUtilStrlen(pszInternationalAreaCode));
	pstTelephoneDes->pszInternationalAreaCode[svc_cas_SiUtilStrlen(pszInternationalAreaCode)] = '\0';
	if(pszOperatorCode != NULL)
	    HxSTD_MemCopy(pstTelephoneDes->pszOperatorCode, pszOperatorCode, svc_cas_SiUtilStrlen(pszOperatorCode));
	pstTelephoneDes->pszOperatorCode[svc_cas_SiUtilStrlen(pszOperatorCode)] = '\0';
	if(pszNationalAreaCode != NULL)
	    HxSTD_MemCopy(pstTelephoneDes->pszNationalAreaCode, pszNationalAreaCode, svc_cas_SiUtilStrlen(pszNationalAreaCode));
	pstTelephoneDes->pszNationalAreaCode[svc_cas_SiUtilStrlen(pszNationalAreaCode)] = '\0';
	if(pszCoreNumber != NULL)
	    HxSTD_MemCopy(pstTelephoneDes->pszCoreNumber, pszCoreNumber, svc_cas_SiUtilStrlen(pszCoreNumber));
	pstTelephoneDes->pszCoreNumber[svc_cas_SiUtilStrlen(pszCoreNumber)] = '\0';
}


/************************************************************************
 *
 * Local Time Offset Descriptor 관련... (TAG = 0x58)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitLocalTimeInfo										*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initLocalTimeInfo(svcCas_LocalTimeInfo_t *pstLocalTimeInfo)
{
	HxSTD_MemSet(pstLocalTimeInfo, 0, sizeof(svcCas_LocalTimeInfo_t));
}


/************************************************************************/
/*	function : AllocLocalTimeInfo										*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_LocalTimeInfo_t 	*allocLocalTimeInfo(void)
{
	svcCas_LocalTimeInfo_t	*pstLocalTimeInfo;


	pstLocalTimeInfo = (svcCas_LocalTimeInfo_t *)OxCAS_Malloc(sizeof(svcCas_LocalTimeInfo_t));
	if (pstLocalTimeInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocLocalTimeInfo()\n");
		return NULL;
	}
	initLocalTimeInfo(pstLocalTimeInfo);

	return ((svcCas_LocalTimeInfo_t *)pstLocalTimeInfo);
}


/************************************************************************/
/*	function : FreeLocalTimeInfo										*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeLocalTimeInfo(svcCas_LocalTimeInfo_t *pstLocalTimeInfo)
{
	if(pstLocalTimeInfo != NULL)
	{
		OxCAS_Free(pstLocalTimeInfo);
	}
}

/************************************************************************/
/*	function : SetLocalTimeInfo											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetLocalTimeInfo(svcCas_LocalTimeInfo_t *pstLocalTimeInfo, HUINT8 *pszCountryCode, HUINT8 ucRegionId, HUINT8 ucLocalTimeOffPolar, HUINT8 *pucLocalTimeOff, UNIXTIME timeOfChange, HUINT8 *pucNextTimeOffset)
{
	HxSTD_MemCopy(pstLocalTimeInfo->szCountryCode, pszCountryCode, 3);
	pstLocalTimeInfo->szCountryCode[3]		= '\0';
	pstLocalTimeInfo->ucRegionId 			= ucRegionId;
	pstLocalTimeInfo->ucLocalTimeOffPolar	= ucLocalTimeOffPolar;
	HxSTD_MemCopy(pstLocalTimeInfo->pucLocalTimeOff, pucLocalTimeOff, 2);
	pstLocalTimeInfo->timeOfChange 			= timeOfChange;
	HxSTD_MemCopy(pstLocalTimeInfo->pucNextTimeOffset, pucNextTimeOffset, 2);
}

/************************************************************************/
/*	function : GetPrevLocalTimeInfo										*/
/*  																	*/
/*  Description : 마지막 작업한 Info의 poHUINT32er를 얻는다.				*/
/************************************************************************/

svcCas_LocalTimeInfo_t 	*svc_cas_SiGetPrevLocalTimeInfo(svcCas_LocalTimeDes_t *pstLocalTimeDes)
{
	svcCas_LocalTimeInfo_t	*pstLocalTimeInfo;

	pstLocalTimeInfo = pstLocalTimeDes->pLocalTimeInfo;
	if (pstLocalTimeInfo == NULL)
	{
		return NULL;
	}

	while (pstLocalTimeInfo->pNextInfo != NULL)
	{
		pstLocalTimeInfo = pstLocalTimeInfo->pNextInfo;
	}

	return ((svcCas_LocalTimeInfo_t *)pstLocalTimeInfo);
}


/************************************************************************/
/*	function : LinkLocalTimeInfoHead									*/
/*  																	*/
/*  Description : head의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkLocalTimeInfoHead(svcCas_LocalTimeDes_t *pstLocalTimeDes, svcCas_LocalTimeInfo_t *pstLocalTimeInfo)
{
	pstLocalTimeDes->pLocalTimeInfo = pstLocalTimeInfo;
}


/************************************************************************/
/*	function : LinkLocalTimeInfoNode									*/
/*  																	*/
/*  Description : node의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkLocalTimeInfoNode(svcCas_LocalTimeInfo_t *pstNewLocalTimeInfo, svcCas_LocalTimeInfo_t *pstNextLocalTimeInfo, svcCas_LocalTimeInfo_t *pstPrevLocalTimeInfo)
{
	if (pstPrevLocalTimeInfo)
	{
		pstPrevLocalTimeInfo->pNextInfo = pstNewLocalTimeInfo;
	}

	pstNewLocalTimeInfo->pNextInfo = pstNextLocalTimeInfo;
}


/************************************************************************/
/*	function : InitLocalTimeDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initLocalTimeDes(svcCas_LocalTimeDes_t *pstLocalTimeDes)
{
	pstLocalTimeDes->pLocalTimeInfo = NULL;
}


/************************************************************************/
/*	function : AllocLocalTimeDes										*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_LocalTimeDes_t 	*allocLocalTimeDes(void)
{
	svcCas_LocalTimeDes_t	*pstLocalTimeDes;


	pstLocalTimeDes = (svcCas_LocalTimeDes_t *)OxCAS_Malloc(sizeof(svcCas_LocalTimeDes_t));
	if (pstLocalTimeDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocLocalTimeDes()\n");
		return NULL;
	}
	initLocalTimeDes(pstLocalTimeDes);

	return ((svcCas_LocalTimeDes_t *)pstLocalTimeDes);
}


/************************************************************************/
/*	function : FreeLocalTimeDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeLocalTimeDes(svcCas_LocalTimeDes_t *pstLocalTimeDes)
{
	svcCas_LocalTimeInfo_t	*pstIndexLocalTimeInfo, *pstDestLocalTimeInfo;

	if(pstLocalTimeDes == NULL)
	{
		return;
	}

	pstIndexLocalTimeInfo = pstLocalTimeDes->pLocalTimeInfo;
	while (pstIndexLocalTimeInfo)
	{
		pstDestLocalTimeInfo = pstIndexLocalTimeInfo;
		pstIndexLocalTimeInfo = pstIndexLocalTimeInfo->pNextInfo;
		freeLocalTimeInfo(pstDestLocalTimeInfo);
	}

	OxCAS_Free(pstLocalTimeDes);
}


/************************************************************************
 *
 * Subtitling Descriptor 관련... (TAG = 0x59)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitSubTitleInfo											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initSubTitleInfo(svcCas_SubTitleInfo_t *pstSubTitleInfo)
{
	HxSTD_MemSet(pstSubTitleInfo->szIso639LangCode, '\0', 4);
	pstSubTitleInfo->ucSubTitleType 		= 0;
	pstSubTitleInfo->usCompositionPageId 	= 0;
	pstSubTitleInfo->usAncillaryPageId 	= 0;
	pstSubTitleInfo->pNextInfo 			= NULL;
}


/************************************************************************/
/*	function : AllocSubTitleInfo										*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_SubTitleInfo_t 	*allocSubTitleInfo(void)
{
	svcCas_SubTitleInfo_t	*pstSubTitleInfo;


	pstSubTitleInfo = (svcCas_SubTitleInfo_t *)OxCAS_Malloc(sizeof(svcCas_SubTitleInfo_t));
	if (pstSubTitleInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocSubTitleInfo()\n");
		return NULL;
	}
	initSubTitleInfo(pstSubTitleInfo);

	return ((svcCas_SubTitleInfo_t *)pstSubTitleInfo);
}


/************************************************************************/
/*	function : FreeSubTitleInfo											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeSubTitleInfo(svcCas_SubTitleInfo_t *pstSubTitleInfo)
{
	if(pstSubTitleInfo != NULL)
	{
		OxCAS_Free(pstSubTitleInfo);
	}
}


/************************************************************************/
/*	function : SetSubTitleInfo											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetSubTitleInfo(svcCas_SubTitleInfo_t *pstSubTitleInfo, HUINT8 *pszIso639LangCode, HUINT8 ucSubTitleType, HUINT16 usCompositionPageId, HUINT16 usAncillaryPageId)
{
	HxSTD_MemCopy(pstSubTitleInfo->szIso639LangCode, pszIso639LangCode, svc_cas_SiUtilStrlen(pszIso639LangCode));
	pstSubTitleInfo->szIso639LangCode[svc_cas_SiUtilStrlen(pszIso639LangCode)] = '\0';
	pstSubTitleInfo->ucSubTitleType 		= ucSubTitleType;
	pstSubTitleInfo->usCompositionPageId 	= usCompositionPageId;
	pstSubTitleInfo->usAncillaryPageId 		= usAncillaryPageId;
}


/************************************************************************/
/*	function : GetPrevSubTitleInfo										*/
/*  																	*/
/*  Description : 마지막 작업한 Info의 poHUINT32er를 얻는다.				*/
/************************************************************************/

svcCas_SubTitleInfo_t 	*svc_cas_SiGetPrevSubTitleInfo(svcCas_SubTitleDes_t *pstSubTitleDes)
{
	svcCas_SubTitleInfo_t	*pstSubTitleInfo;


	pstSubTitleInfo = pstSubTitleDes->pSubTitleInfo;
	if (pstSubTitleInfo == NULL)
	{
		return NULL;
	}

	while (pstSubTitleInfo->pNextInfo != NULL)
	{
		pstSubTitleInfo = pstSubTitleInfo->pNextInfo;
	}

	return ((svcCas_SubTitleInfo_t *)pstSubTitleInfo);
}


/************************************************************************/
/*	function : LinkSubTitleInfoHead										*/
/*  																	*/
/*  Description : head의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkSubTitleInfoHead(svcCas_SubTitleDes_t *pstSubTitleDes, svcCas_SubTitleInfo_t *pstSubTitleInfo)
{
	pstSubTitleDes->pSubTitleInfo = pstSubTitleInfo;
}


/************************************************************************/
/*	function : LinkSubTitleInfoNode										*/
/*  																	*/
/*  Description : node의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkSubTitleInfoNode(svcCas_SubTitleInfo_t *pstNewSubTitleInfo, svcCas_SubTitleInfo_t *pstNextSubTitleInfo, svcCas_SubTitleInfo_t *pstPrevSubTitleInfo)
{
	if (pstPrevSubTitleInfo)
	{
		pstPrevSubTitleInfo->pNextInfo = pstNewSubTitleInfo;
	}

	pstNewSubTitleInfo->pNextInfo = pstNextSubTitleInfo;
}


/************************************************************************/
/*	function : InitSubTitleDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initSubTitleDes(svcCas_SubTitleDes_t *pstSubTitleDes)
{
	pstSubTitleDes->pSubTitleInfo = NULL;
}

/************************************************************************/
/*	function : AllocSubTitleDes											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_SubTitleDes_t 	*allocSubTitleDes(void)
{
	svcCas_SubTitleDes_t	*pstSubTitleDes;


	pstSubTitleDes = (svcCas_SubTitleDes_t *)OxCAS_Malloc(sizeof(svcCas_SubTitleDes_t));
	if (pstSubTitleDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocSubTitleDes()\n");
		return NULL;
	}
	initSubTitleDes(pstSubTitleDes);

	return ((svcCas_SubTitleDes_t *)pstSubTitleDes);
}


/************************************************************************/
/*	function : FreeSubTitleDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeSubTitleDes(svcCas_SubTitleDes_t *pstSubTitleDes)
{
	svcCas_SubTitleInfo_t	*pstIndexSubTitleInfo, *pstDestSubTitleInfo;

	if(pstSubTitleDes == NULL)
	{
		return;
	}

	pstIndexSubTitleInfo = pstSubTitleDes->pSubTitleInfo;
	while (pstIndexSubTitleInfo)
	{
		pstDestSubTitleInfo = pstIndexSubTitleInfo;
		pstIndexSubTitleInfo = pstIndexSubTitleInfo->pNextInfo;
		freeSubTitleInfo(pstDestSubTitleInfo);
	}

	OxCAS_Free(pstSubTitleDes);
}



/************************************************************************
 *
 * Terrestrial Delivery System Descriptor 관련... (TAG = 0x5a)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitTerDeliSystemDes										*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initTerDeliSystemDes(svcCas_TerDeliSystemDes_t *pstTerDeliSystemDes)
{
	pstTerDeliSystemDes->ulFreq 				= 0;
	pstTerDeliSystemDes->ucBandWidth 			= 0;
	pstTerDeliSystemDes->ucPriority				= 0;
	pstTerDeliSystemDes->ucConstellation	 	= 0;
	pstTerDeliSystemDes->ucHierInfo 			= 0;
	pstTerDeliSystemDes->ucCodeRateHPStream 	= 0;
	pstTerDeliSystemDes->ucCodeRateLPStream 	= 0;
	pstTerDeliSystemDes->ucGuardInterval 		= 0;
	pstTerDeliSystemDes->ucTransmissionMode 	= 0;
	pstTerDeliSystemDes->bOtherFreqFlag 		= FALSE;
}


/************************************************************************/
/*	function : AllocTerDeliSystemDes									*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_TerDeliSystemDes_t 	*allocTerDeliSystemDes(void)
{
	svcCas_TerDeliSystemDes_t	*pstTerDeliSystemDes;

	pstTerDeliSystemDes = (svcCas_TerDeliSystemDes_t *)OxCAS_Malloc(sizeof(svcCas_TerDeliSystemDes_t));
	if (pstTerDeliSystemDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocTerDeliSystemDes()\n");
		return NULL;
	}
	initTerDeliSystemDes(pstTerDeliSystemDes);

	return ((svcCas_TerDeliSystemDes_t *)pstTerDeliSystemDes);
}


/************************************************************************/
/*	function : FreeTerDeliSystemDes										*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeTerDeliSystemDes(svcCas_TerDeliSystemDes_t *pstTerDeliSystemDes)
{
	if(pstTerDeliSystemDes != NULL)
	{
		OxCAS_Free(pstTerDeliSystemDes);
	}
}


/************************************************************************/
/*	function : SetTerDeliSystemDes										*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetTerDeliSystemDes(svcCas_TerDeliSystemDes_t *pstTerDeliSystemDes, HUINT32 ulFreq
	, HUINT8 ucBandWidth, HUINT8 ucPriority, HUINT8 ucConstellation, HUINT8 ucHierInfo
	, HUINT8 ucCodeRateHPStream, HUINT8 ucCodeRateLPStream, HUINT8 ucGuardInterval
	, HUINT8 ucTransmissionMode, HBOOL bOtherFreqFlag)
{
	pstTerDeliSystemDes->ulFreq 				= ulFreq;
	pstTerDeliSystemDes->ucBandWidth 			= ucBandWidth;
	pstTerDeliSystemDes->ucPriority				= ucPriority;
	pstTerDeliSystemDes->ucConstellation	 	= ucConstellation;
	pstTerDeliSystemDes->ucHierInfo 			= ucHierInfo;
	pstTerDeliSystemDes->ucCodeRateHPStream 	= ucCodeRateHPStream;
	pstTerDeliSystemDes->ucCodeRateLPStream 	= ucCodeRateLPStream;
	pstTerDeliSystemDes->ucGuardInterval 		= ucGuardInterval;
	pstTerDeliSystemDes->ucTransmissionMode 	= ucTransmissionMode;
	pstTerDeliSystemDes->bOtherFreqFlag 		= bOtherFreqFlag;
}


/************************************************************************
 *
 * Multilingual Network Name Descriptor 관련... (TAG = 0x5b)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitMultiNetNameInfo										*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initMultiNetNameInfo(svcCas_MultiNetNameInfo_t *pstMultiNetNameInfo)
{
	HxSTD_MemSet(pstMultiNetNameInfo->szIso639LangCode, '\0', 4);
	pstMultiNetNameInfo->pszNetName 	= NULL;
	pstMultiNetNameInfo->pNextInfo	= NULL;
}


/************************************************************************/
/*	function : AllocMultiNetNameInfo									*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_MultiNetNameInfo_t 	*allocMultiNetNameInfo(void)
{
	svcCas_MultiNetNameInfo_t	*pstMultiNetNameInfo;


	pstMultiNetNameInfo = (svcCas_MultiNetNameInfo_t *)OxCAS_Malloc(sizeof(svcCas_MultiNetNameInfo_t));
	if (pstMultiNetNameInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocMultiNetNameInfo()\n");
		return NULL;
	}
	initMultiNetNameInfo(pstMultiNetNameInfo);

	return ((svcCas_MultiNetNameInfo_t *)pstMultiNetNameInfo);
}


/************************************************************************/
/*	function : FreeMultiNetNameInfo										*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeMultiNetNameInfo(svcCas_MultiNetNameInfo_t *pstMultiNetNameInfo)
{
	if(pstMultiNetNameInfo == NULL)
	{
		return;
	}

	if(pstMultiNetNameInfo->pszNetName)
	{
		svc_cas_SiFreeMem(pstMultiNetNameInfo->pszNetName);
	}

	OxCAS_Free(pstMultiNetNameInfo);
}


/************************************************************************/
/*	function : SetMultiNetNameInfo										*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetMultiNetNameInfo(svcCas_MultiNetNameInfo_t *pstMultiNetNameInfo
	, HUINT8 *pszIso639LangCode, HUINT8 *pszNetName)
{
    if(pszIso639LangCode != NULL)
	    HxSTD_MemCopy(pstMultiNetNameInfo->szIso639LangCode, pszIso639LangCode, svc_cas_SiUtilStrlen(pszIso639LangCode));
	pstMultiNetNameInfo->szIso639LangCode[svc_cas_SiUtilStrlen(pszIso639LangCode)] = '\0';
	if(pszNetName != NULL)
	    HxSTD_MemCopy(pstMultiNetNameInfo->pszNetName, pszNetName, svc_cas_SiUtilStrlen(pszNetName));
	pstMultiNetNameInfo->pszNetName[svc_cas_SiUtilStrlen(pszNetName)] = '\0';
}


/************************************************************************/
/*	function : GetPrevMultiNetNameInfo									*/
/*  																	*/
/*  Description : 마지막 작업한 Info의 poHUINT32er를 얻는다.				*/
/************************************************************************/

svcCas_MultiNetNameInfo_t 	*svc_cas_SiGetPrevMultiNetNameInfo(svcCas_MultiNetNameDes_t *pstMultiNetNameDes)
{
	svcCas_MultiNetNameInfo_t	*pstMultiNetNameInfo;


	pstMultiNetNameInfo = pstMultiNetNameDes->pMultiNetNameInfo;
	if (pstMultiNetNameInfo == NULL)
	{
		return NULL;
	}

	while (pstMultiNetNameInfo->pNextInfo != NULL)
	{
		pstMultiNetNameInfo = pstMultiNetNameInfo->pNextInfo;
	}

	return ((svcCas_MultiNetNameInfo_t *)pstMultiNetNameInfo);
}


/************************************************************************/
/*	function : LinkMultiNetNameInfoHead									*/
/*  																	*/
/*  Description : head의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkMultiNetNameInfoHead(svcCas_MultiNetNameDes_t *pstMultiNetNameDes
	, svcCas_MultiNetNameInfo_t *pstMultiNetNameInfo)
{
	pstMultiNetNameDes->pMultiNetNameInfo = pstMultiNetNameInfo;
}


/************************************************************************/
/*	function : LinkMultiNetNameInfoNode									*/
/*  																	*/
/*  Description : node의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkMultiNetNameInfoNode(svcCas_MultiNetNameInfo_t *pstNewMultiNetNameInfo
	, svcCas_MultiNetNameInfo_t *pstNextMultiNetNameInfo
	, svcCas_MultiNetNameInfo_t *pstPrevMultiNetNameInfo)
{
	if (pstPrevMultiNetNameInfo)
	{
		pstPrevMultiNetNameInfo->pNextInfo = pstNewMultiNetNameInfo;
	}

	pstNewMultiNetNameInfo->pNextInfo = pstNextMultiNetNameInfo;
}


/************************************************************************/
/*	function : InitMultiNetNameDes										*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initMultiNetNameDes(svcCas_MultiNetNameDes_t *pstMultiNetNameDes)
{
	pstMultiNetNameDes->pMultiNetNameInfo = NULL;
}


/************************************************************************/
/*	function : AllocMultiNetNameDes										*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_MultiNetNameDes_t 	*allocMultiNetNameDes(void)
{
	svcCas_MultiNetNameDes_t	*pstMultiNetNameDes;


	pstMultiNetNameDes = (svcCas_MultiNetNameDes_t *)OxCAS_Malloc(sizeof(svcCas_MultiNetNameDes_t));
	if (pstMultiNetNameDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocMultiNetNameDes()\n");
		return NULL;
	}
	initMultiNetNameDes(pstMultiNetNameDes);

	return ((svcCas_MultiNetNameDes_t *)pstMultiNetNameDes);
}


/************************************************************************/
/*	function : FreeMultiNetNameDes										*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeMultiNetNameDes(svcCas_MultiNetNameDes_t *pstMultiNetNameDes)
{
	svcCas_MultiNetNameInfo_t	*pstIndexMultiNetNameInfo, *pstDestMultiNetNameInfo;

	if(pstMultiNetNameDes == NULL)
	{
		return;
	}

	pstIndexMultiNetNameInfo = pstMultiNetNameDes->pMultiNetNameInfo;
	while (pstIndexMultiNetNameInfo)
	{
		pstDestMultiNetNameInfo = pstIndexMultiNetNameInfo;
		pstIndexMultiNetNameInfo = pstIndexMultiNetNameInfo->pNextInfo;
		freeMultiNetNameInfo(pstDestMultiNetNameInfo);
	}

	OxCAS_Free(pstMultiNetNameDes);
}



/************************************************************************
 *
 * Multilingual Bouquet Name Descriptor 관련... (TAG = 0x5c)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitMultiBouqNameInfo									*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initMultiBouqNameInfo(svcCas_MultiBouqNameInfo_t *pstMultiBouqNameInfo)
{
	HxSTD_MemSet(pstMultiBouqNameInfo->szIso639LangCode, '\0', 4);
	pstMultiBouqNameInfo->pszBouqName = NULL;
	pstMultiBouqNameInfo->pNextInfo	= NULL;
}


/************************************************************************/
/*	function : AllocMultiBouqNameInfo									*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_MultiBouqNameInfo_t 	*allocMultiBouqNameInfo(void)
{
	svcCas_MultiBouqNameInfo_t	*pstMultiBouqNameInfo;


	pstMultiBouqNameInfo = (svcCas_MultiBouqNameInfo_t *)OxCAS_Malloc(sizeof(svcCas_MultiBouqNameInfo_t));
	if (pstMultiBouqNameInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocMultiBouqNameInfo()\n");
		return NULL;
	}
	initMultiBouqNameInfo(pstMultiBouqNameInfo);

	return ((svcCas_MultiBouqNameInfo_t *)pstMultiBouqNameInfo);
}


/************************************************************************/
/*	function : FreeMultiBouqNameInfo									*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeMultiBouqNameInfo(svcCas_MultiBouqNameInfo_t *pstMultiBouqNameInfo)
{
	if (pstMultiBouqNameInfo == NULL)
	{
		return;
	}

	if(pstMultiBouqNameInfo->pszBouqName)
	{
		svc_cas_SiFreeMem(pstMultiBouqNameInfo->pszBouqName);
	}

	OxCAS_Free(pstMultiBouqNameInfo);
}


/************************************************************************/
/*	function : SetMultiBouqNameInfo										*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetMultiBouqNameInfo(svcCas_MultiBouqNameInfo_t *pstMultiBouqNameInfo, HUINT8 *pszIso639LangCode, HUINT8 *pszBouqName)
{
    if(pszIso639LangCode != NULL)
    	HxSTD_MemCopy(pstMultiBouqNameInfo->szIso639LangCode, pszIso639LangCode, svc_cas_SiUtilStrlen(pszIso639LangCode));
	pstMultiBouqNameInfo->szIso639LangCode[svc_cas_SiUtilStrlen(pszIso639LangCode)] = '\0';
	if(pszBouqName != NULL)
    	HxSTD_MemCopy(pstMultiBouqNameInfo->pszBouqName, pszBouqName, svc_cas_SiUtilStrlen(pszBouqName));
	pstMultiBouqNameInfo->pszBouqName[svc_cas_SiUtilStrlen(pszBouqName)] = '\0';
}


/************************************************************************/
/*	function : GetPrevMultiBouqNameInfo									*/
/*  																	*/
/*  Description : 마지막 작업한 Info의 poHUINT32er를 얻는다.				*/
/************************************************************************/

svcCas_MultiBouqNameInfo_t 	*svc_cas_SiGetPrevMultiBouqNameInfo(svcCas_MultiBouqNameDes_t *pstMultiBouqNameDes)
{
	svcCas_MultiBouqNameInfo_t	*pstMultiBouqNameInfo;


	pstMultiBouqNameInfo = pstMultiBouqNameDes->pMultiBouqNameInfo;
	if (pstMultiBouqNameInfo == NULL)
	{
		return NULL;
	}

	while (pstMultiBouqNameInfo->pNextInfo != NULL)
	{
		pstMultiBouqNameInfo = pstMultiBouqNameInfo->pNextInfo;
	}

	return ((svcCas_MultiBouqNameInfo_t *)pstMultiBouqNameInfo);
}


/************************************************************************/
/*	function : LinkMultiBouqNameInfoHead								*/
/*  																	*/
/*  Description : head의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkMultiBouqNameInfoHead(svcCas_MultiBouqNameDes_t *pstMultiBouqNameDes, svcCas_MultiBouqNameInfo_t *pstMultiBouqNameInfo)
{
	pstMultiBouqNameDes->pMultiBouqNameInfo = pstMultiBouqNameInfo;
}


/************************************************************************/
/*	function : LinkMultiBouqNameInfoNode								*/
/*  																	*/
/*  Description : node의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkMultiBouqNameInfoNode(svcCas_MultiBouqNameInfo_t *pstNewMultiBouqNameInfo, svcCas_MultiBouqNameInfo_t *pstNextMultiBouqNameInfo, svcCas_MultiBouqNameInfo_t *pstPrevMultiBouqNameInfo)
{
	if (pstPrevMultiBouqNameInfo)
	{
		pstPrevMultiBouqNameInfo->pNextInfo = pstNewMultiBouqNameInfo;
	}

	pstNewMultiBouqNameInfo->pNextInfo = pstNextMultiBouqNameInfo;
}


/************************************************************************/
/*	function : InitMultiBouqNameDes										*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initMultiBouqNameDes(svcCas_MultiBouqNameDes_t *pstMultiBouqNameDes)
{
	pstMultiBouqNameDes->pMultiBouqNameInfo = NULL;
}


/************************************************************************/
/*	function : AllocMultiBouqNameDes									*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_MultiBouqNameDes_t 	*allocMultiBouqNameDes(void)
{
	svcCas_MultiBouqNameDes_t	*pstMultiBouqNameDes;


	pstMultiBouqNameDes = (svcCas_MultiBouqNameDes_t *)OxCAS_Malloc(sizeof(svcCas_MultiBouqNameDes_t));
	if (pstMultiBouqNameDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocMultiBouqNameDes()\n");
		return NULL;
	}
	initMultiBouqNameDes(pstMultiBouqNameDes);

	return ((svcCas_MultiBouqNameDes_t *)pstMultiBouqNameDes);
}


/************************************************************************/
/*	function : FreeMultiBouqNameDes										*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeMultiBouqNameDes(svcCas_MultiBouqNameDes_t *pstMultiBouqNameDes)
{
	svcCas_MultiBouqNameInfo_t	*pstIndexMultiBouqNameInfo, *pstDestMultiBouqNameInfo;

	if (pstMultiBouqNameDes == NULL)
	{
		return;
	}

	pstIndexMultiBouqNameInfo = pstMultiBouqNameDes->pMultiBouqNameInfo;
	while (pstIndexMultiBouqNameInfo)
	{
		pstDestMultiBouqNameInfo = pstIndexMultiBouqNameInfo;
		pstIndexMultiBouqNameInfo = pstIndexMultiBouqNameInfo->pNextInfo;
		freeMultiBouqNameInfo(pstDestMultiBouqNameInfo);
	}

	OxCAS_Free(pstMultiBouqNameDes);
}


/************************************************************************
 *
 * Multilingual Service Name Descriptor 관련... (TAG = 0x5d)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitMultiServNameInfo									*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initMultiServNameInfo(svcCas_MultiSvcNameInfo_t *pstMultiServNameInfo)
{
	HxSTD_MemSet(pstMultiServNameInfo->szIso639LangCode, '\0', 4);
	pstMultiServNameInfo->pszServProviderName	= NULL;
	pstMultiServNameInfo->pszServName 			= NULL;
	pstMultiServNameInfo->pNextInfo				= NULL;
}


/************************************************************************/
/*	function : AllocMultiServNameInfo									*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_MultiSvcNameInfo_t 	*allocMultiServNameInfo(void)
{
	svcCas_MultiSvcNameInfo_t	*pstMultiServNameInfo;

	pstMultiServNameInfo = (svcCas_MultiSvcNameInfo_t *)OxCAS_Malloc(sizeof(svcCas_MultiSvcNameInfo_t));
	if (pstMultiServNameInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocMultiServNameInfo()\n");
		return NULL;
	}
	initMultiServNameInfo(pstMultiServNameInfo);

	return ((svcCas_MultiSvcNameInfo_t *)pstMultiServNameInfo);
}


/************************************************************************/
/*	function : FreeMultiServNameInfo									*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeMultiServNameInfo(svcCas_MultiSvcNameInfo_t *pstMultiServNameInfo)
{
	if (pstMultiServNameInfo == NULL)
	{
		return;
	}

	if (pstMultiServNameInfo->pszServProviderName)
	{
		svc_cas_SiFreeMem(pstMultiServNameInfo->pszServProviderName);
	}

	if (pstMultiServNameInfo->pszServName)
	{
		svc_cas_SiFreeMem(pstMultiServNameInfo->pszServName);
	}

	OxCAS_Free(pstMultiServNameInfo);
}


/************************************************************************/
/*	function : SetMultiServNameInfo										*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetMultiServNameInfo(svcCas_MultiSvcNameInfo_t *pstMultiServNameInfo
	, HUINT8 *pszIso639LangCode, HUINT8 *pszServProviderName, HUINT8 *pszServName)
{
	HxSTD_MemCopy(pstMultiServNameInfo->szIso639LangCode, pszIso639LangCode
		, svc_cas_SiUtilStrlen(pszIso639LangCode));
	pstMultiServNameInfo->szIso639LangCode[svc_cas_SiUtilStrlen(pszIso639LangCode)] = '\0';
	HxSTD_MemCopy(pstMultiServNameInfo->pszServProviderName, pszServProviderName
		, svc_cas_SiUtilStrlen(pszServProviderName));
	pstMultiServNameInfo->pszServProviderName[svc_cas_SiUtilStrlen(pszServProviderName)] = '\0';
	HxSTD_MemCopy(pstMultiServNameInfo->pszServName, pszServName
		, svc_cas_SiUtilStrlen(pszServName));
	pstMultiServNameInfo->pszServName[svc_cas_SiUtilStrlen(pszServName)] = '\0';
}


/************************************************************************/
/*	function : GetPrevMultiServNameInfo									*/
/*  																	*/
/*  Description : 마지막 작업한 Info의 poHUINT32er를 얻는다.				*/
/************************************************************************/

svcCas_MultiSvcNameInfo_t *svc_cas_SiGetPrevMultiServNameInfo(svcCas_MultiSvcNameDes_t *pstMultiServNameDes)
{
	svcCas_MultiSvcNameInfo_t	*pstMultiServNameInfo;

	pstMultiServNameInfo = pstMultiServNameDes->pMultiServNameInfo;
	if (pstMultiServNameInfo == NULL)
	{
		return NULL;
	}

	while (pstMultiServNameInfo->pNextInfo != NULL)
	{
		pstMultiServNameInfo = pstMultiServNameInfo->pNextInfo;
	}

	return ((svcCas_MultiSvcNameInfo_t *)pstMultiServNameInfo);
}


/************************************************************************/
/*	function : LinkMultiServNameInfoHead								*/
/*  																	*/
/*  Description : head의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkMultiServNameInfoHead(svcCas_MultiSvcNameDes_t *pstMultiServNameDes
	, svcCas_MultiSvcNameInfo_t *pstMultiServNameInfo)
{
	pstMultiServNameDes->pMultiServNameInfo = pstMultiServNameInfo;
}


/************************************************************************/
/*	function : LinkMultiServNameInfoNode								*/
/*  																	*/
/*  Description : node의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkMultiServNameInfoNode(svcCas_MultiSvcNameInfo_t *pstNewMultiServNameInfo
	, svcCas_MultiSvcNameInfo_t *pstNextMultiServNameInfo
	, svcCas_MultiSvcNameInfo_t *pstPrevMultiServNameInfo)
{
	if (pstPrevMultiServNameInfo)
	{
		pstPrevMultiServNameInfo->pNextInfo = pstNewMultiServNameInfo;
	}

	pstNewMultiServNameInfo->pNextInfo = pstNextMultiServNameInfo;
}


/************************************************************************/
/*	function : InitMultiServNameDes										*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initMultiServNameDes(svcCas_MultiSvcNameDes_t *pstMultiServNameDes)
{
	pstMultiServNameDes->pMultiServNameInfo = NULL;
}

/************************************************************************/
/*	function : AllocMultiServNameDes									*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_MultiSvcNameDes_t 	*allocMultiServNameDes(void)
{
	svcCas_MultiSvcNameDes_t	*pstMultiServNameDes;

	pstMultiServNameDes = (svcCas_MultiSvcNameDes_t *)OxCAS_Malloc(sizeof(svcCas_MultiSvcNameDes_t));
	if (pstMultiServNameDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocMultiServNameDes()\n");
		return NULL;
	}
	initMultiServNameDes(pstMultiServNameDes);

	return ((svcCas_MultiSvcNameDes_t *)pstMultiServNameDes);
}


/************************************************************************/
/*	function : FreeMultiServNameDes										*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeMultiServNameDes(svcCas_MultiSvcNameDes_t *pstMultiServNameDes)
{
	svcCas_MultiSvcNameInfo_t	*pstIndexMultiServNameInfo, *pstDestMultiServNameInfo;

	if (pstMultiServNameDes == NULL)
	{
		return;
	}

	pstIndexMultiServNameInfo = pstMultiServNameDes->pMultiServNameInfo;
	while (pstIndexMultiServNameInfo)
	{
		pstDestMultiServNameInfo = pstIndexMultiServNameInfo;
		pstIndexMultiServNameInfo = pstIndexMultiServNameInfo->pNextInfo;
		freeMultiServNameInfo(pstDestMultiServNameInfo);
	}

	OxCAS_Free(pstMultiServNameDes);
}



/************************************************************************
 *
 * Multilingual Component Descriptor 관련... (TAG = 0x5e)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitMultiCompInfo										*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initMultiCompInfo(svcCas_MultiCompInfo_t *pstMultiCompInfo)
{
	HxSTD_MemSet(pstMultiCompInfo->szIso639LangCode, '\0', 4);
	pstMultiCompInfo->pszTextDes 	= NULL;
	pstMultiCompInfo->pNextInfo	= NULL;
}


/************************************************************************/
/*	function : AllocMultiCompInfo										*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_MultiCompInfo_t 	*allocMultiCompInfo(void)
{
	svcCas_MultiCompInfo_t	*pstMultiCompInfo;

	pstMultiCompInfo = (svcCas_MultiCompInfo_t *)OxCAS_Malloc(sizeof(svcCas_MultiCompInfo_t));
	if (pstMultiCompInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocMultiCompInfo()\n");
		return NULL;
	}
	initMultiCompInfo(pstMultiCompInfo);

	return ((svcCas_MultiCompInfo_t *)pstMultiCompInfo);
}


/************************************************************************/
/*	function : FreeMultiCompInfo										*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeMultiCompInfo(svcCas_MultiCompInfo_t *pstMultiCompInfo)
{
	if (pstMultiCompInfo == NULL)
	{
		return;
	}

	if (pstMultiCompInfo->pszTextDes)
	{
		svc_cas_SiFreeMem(pstMultiCompInfo->pszTextDes);
	}

	OxCAS_Free(pstMultiCompInfo);
}


/************************************************************************/
/*	function : SetMultiCompInfo											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetMultiCompInfo(svcCas_MultiCompInfo_t *pstMultiCompInfo, HUINT8 *pszIso639LangCode, HUINT8 *pszTextDes)
{
    if(pszIso639LangCode != NULL)
	    HxSTD_MemCopy(pstMultiCompInfo->szIso639LangCode, pszIso639LangCode, svc_cas_SiUtilStrlen(pszIso639LangCode));
	pstMultiCompInfo->szIso639LangCode[svc_cas_SiUtilStrlen(pszIso639LangCode)] = '\0';
	if(pszTextDes != NULL)
	    HxSTD_MemCopy(pstMultiCompInfo->pszTextDes, pszTextDes, svc_cas_SiUtilStrlen(pszTextDes));
	pstMultiCompInfo->pszTextDes[svc_cas_SiUtilStrlen(pszTextDes)] = '\0';
}


/************************************************************************/
/*	function : GetPrevMultiCompInfo										*/
/*  																	*/
/*  Description : 마지막 작업한 Info의 poHUINT32er를 얻는다.				*/
/************************************************************************/

svcCas_MultiCompInfo_t 	*svc_cas_SiGetPrevMultiCompInfo(svcCas_MultiCompDes_t *pstMultiCompDes)
{
	svcCas_MultiCompInfo_t	*pstMultiCompInfo;


	pstMultiCompInfo = pstMultiCompDes->pMultiCompInfo;
	if (pstMultiCompInfo == NULL)
	{
		return NULL;
	}

	while (pstMultiCompInfo->pNextInfo != NULL)
	{
		pstMultiCompInfo = pstMultiCompInfo->pNextInfo;
	}

	return ((svcCas_MultiCompInfo_t *)pstMultiCompInfo);
}


/************************************************************************/
/*	function : LinkMultiCompInfoHead									*/
/*  																	*/
/*  Description : head의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkMultiCompInfoHead(svcCas_MultiCompDes_t *pstMultiCompDes, svcCas_MultiCompInfo_t *pstMultiCompInfo)
{
	pstMultiCompDes->pMultiCompInfo = pstMultiCompInfo;
}


/************************************************************************/
/*	function : LinkMultiCompInfoNode									*/
/*  																	*/
/*  Description : node의 link 설정.										*/
/************************************************************************/

void 	svc_cas_SiLinkMultiCompInfoNode(svcCas_MultiCompInfo_t *pstNewMultiCompInfo
	, svcCas_MultiCompInfo_t *pstNextMultiCompInfo, svcCas_MultiCompInfo_t *pstPrevMultiCompInfo)
{
	if (pstPrevMultiCompInfo)
	{
		pstPrevMultiCompInfo->pNextInfo = pstNewMultiCompInfo;
	}

	pstNewMultiCompInfo->pNextInfo = pstNextMultiCompInfo;
}


/************************************************************************/
/*	function : InitMultiCompDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initMultiCompDes(svcCas_MultiCompDes_t *pstMultiCompDes)
{
	pstMultiCompDes->ucCompTag		= 0;
	pstMultiCompDes->pMultiCompInfo 	= NULL;
}


/************************************************************************/
/*	function : AllocMultiCompDes										*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_MultiCompDes_t 	*allocMultiCompDes(void)
{
	svcCas_MultiCompDes_t	*pstMultiCompDes;


	pstMultiCompDes = (svcCas_MultiCompDes_t *)OxCAS_Malloc(sizeof(svcCas_MultiCompDes_t));
	if (pstMultiCompDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocMultiCompDes()\n");
		return NULL;
	}
	initMultiCompDes(pstMultiCompDes);

	return ((svcCas_MultiCompDes_t *)pstMultiCompDes);
}


/************************************************************************/
/*	function : FreeMultiCompDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeMultiCompDes(svcCas_MultiCompDes_t *pstMultiCompDes)
{
	svcCas_MultiCompInfo_t	*pstIndexMultiCompInfo, *pstDestMultiCompInfo;

	if (pstMultiCompDes == NULL)
	{
		return;
	}

	pstIndexMultiCompInfo = pstMultiCompDes->pMultiCompInfo;
	while (pstIndexMultiCompInfo)
	{
		pstDestMultiCompInfo = pstIndexMultiCompInfo;
		pstIndexMultiCompInfo = pstIndexMultiCompInfo->pNextInfo;
		freeMultiCompInfo(pstDestMultiCompInfo);
	}

	OxCAS_Free(pstMultiCompDes);
}


/************************************************************************/
/*	function : SetMultiCompDes											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetMultiCompDes(svcCas_MultiCompDes_t *pstMultiCompDes, HUINT8 ucCompTag)
{
	pstMultiCompDes->ucCompTag = ucCompTag;
}


/************************************************************************
 *
 * Private Data Specifier Descriptor 관련... (TAG = 0x5f)
 *
 ************************************************************************/

/************************************************************************/
/*	function : InitPrivateDataSpecpDes									*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initPrivateDataSpecDes(svcCas_PrivateDataSpecDes_t *pstPrivateDataSpecpDes)
{
	pstPrivateDataSpecpDes->ulPrivateDataSpec = 0;
}


/************************************************************************/
/*	function : AllocPrivateDataSpecpDes									*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_PrivateDataSpecDes_t 	*allocPrivateDataSpecDes(void)
{
	svcCas_PrivateDataSpecDes_t	*pstPrivateDataSpecDes;

	pstPrivateDataSpecDes = (svcCas_PrivateDataSpecDes_t *)OxCAS_Malloc(sizeof(svcCas_PrivateDataSpecDes_t));
	if (pstPrivateDataSpecDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocPrivateDataSpecDes()\n");
		return NULL;
	}
	initPrivateDataSpecDes(pstPrivateDataSpecDes);

	return ((svcCas_PrivateDataSpecDes_t *)pstPrivateDataSpecDes);
}


/************************************************************************/
/*	function : FreePrivateDataSpecDes									*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freePrivateDataSpecDes(svcCas_PrivateDataSpecDes_t *pstPrivateDataSpecDes)
{
	if (pstPrivateDataSpecDes != NULL)
	{
		OxCAS_Free(pstPrivateDataSpecDes);
	}
}


/************************************************************************/
/*	function : SetPrivateDataSpecDes									*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetPrivateDataSpecDes(svcCas_PrivateDataSpecDes_t *pstPrivateDataSpecDes
	, HUINT32 ulPrivateDataSpec)
{
	pstPrivateDataSpecDes->ulPrivateDataSpec = ulPrivateDataSpec;
}


/************************************************************************
 *
 * Service Move Descriptor 관련... (TAG = 0x60)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitServMoveDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initServMoveDes(svcCas_SvcMoveDes_t *pstServMoveDes)
{
	pstServMoveDes->usNewOrgNetId = 0;
	pstServMoveDes->usNewTsId 	= 0;
	pstServMoveDes->usNewServId 	= 0;
}


/************************************************************************/
/*	function : AllocServMoveDes											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_SvcMoveDes_t 	*allocServMoveDes(void)
{
	svcCas_SvcMoveDes_t	*pstServMoveDes;


	pstServMoveDes = (svcCas_SvcMoveDes_t *)OxCAS_Malloc(sizeof(svcCas_SvcMoveDes_t));
	if (pstServMoveDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocServMoveDes()\n");
		return NULL;
	}
	initServMoveDes(pstServMoveDes);

	return ((svcCas_SvcMoveDes_t *)pstServMoveDes);
}


/************************************************************************/
/*	function : FreeServMoveDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeServMoveDes(svcCas_SvcMoveDes_t *pstServMoveDes)
{
	if (pstServMoveDes != NULL)
	{
		OxCAS_Free(pstServMoveDes);
	}
}


/************************************************************************/
/*	function : SetServMoveDes											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetServMoveDes(svcCas_SvcMoveDes_t *pstServMoveDes, HUINT16 usNewOrgNetId
	, HUINT16 usNewTsId, HUINT16 usNewServId)
{
	pstServMoveDes->usNewOrgNetId	= usNewOrgNetId;
	pstServMoveDes->usNewTsId 		= usNewTsId;
	pstServMoveDes->usNewServId 	= usNewServId;
}


/************************************************************************
 *
 * Short Smoothing Buffer Descriptor 관련... (TAG = 0x61)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitShortSmoothBufDes									*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initShortSmoothBufDes(svcCas_ShortSmoothBufDes_t *pstShortSmoothBufDes)
{
	pstShortSmoothBufDes->ucSbSize 			= 0;
	pstShortSmoothBufDes->ucSbLeakRate		= 0;
	pstShortSmoothBufDes->pPrivateDataInfo	= NULL;
}


/************************************************************************/
/*	function : AllocShortSmoothBufDes									*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_ShortSmoothBufDes_t 	*allocShortSmoothBufDes(void)
{
	svcCas_ShortSmoothBufDes_t	*pstShortSmoothBufDes;

	pstShortSmoothBufDes = (svcCas_ShortSmoothBufDes_t *)OxCAS_Malloc(sizeof(svcCas_ShortSmoothBufDes_t));
	if (pstShortSmoothBufDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocShortSmoothBufDes()\n");
		return NULL;
	}
	initShortSmoothBufDes(pstShortSmoothBufDes);

	return ((svcCas_ShortSmoothBufDes_t *)pstShortSmoothBufDes);
}


/************************************************************************/
/*	function : FreeShortSmoothBufDes									*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeShortSmoothBufDes(svcCas_ShortSmoothBufDes_t *pstShortSmoothBufDes)
{
	if (pstShortSmoothBufDes == NULL)
	{
		return;
	}

	if (pstShortSmoothBufDes->pPrivateDataInfo != NULL)
	{
		svc_cas_SiDesFreePrivateDataInfo(pstShortSmoothBufDes->pPrivateDataInfo);
	}

	OxCAS_Free(pstShortSmoothBufDes);
}


/************************************************************************/
/*	function : SetShortSmoothBufDes										*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetShortSmoothBufDes(svcCas_ShortSmoothBufDes_t *pstShortSmoothBufDes
	, HUINT8 ucSbSize, HUINT8 ucSbLeakRate, svcCas_PrivateDataInfo_t *pstPrivateDataInfo)
{
	pstShortSmoothBufDes->ucSbSize 			= ucSbSize;
	pstShortSmoothBufDes->ucSbLeakRate		= ucSbLeakRate;
	pstShortSmoothBufDes->pPrivateDataInfo	= pstPrivateDataInfo;
}

/************************************************************************
 *
 * Frequency List Descriptor 관련... (TAG = 0x62)
 *
 ************************************************************************/

STATIC void initFrequencyListDes(svcCas_FrequencyListDes_t *pstDes)
{
	pstDes->codingType = 0;
	pstDes->numFreq = 0;
	pstDes->centreFreq = NULL;
}


STATIC svcCas_FrequencyListDes_t *allocFrequencyListDes(void)
{
	svcCas_FrequencyListDes_t *pstFrequencyListDes;

	pstFrequencyListDes = (svcCas_FrequencyListDes_t *) OxCAS_Malloc (sizeof (svcCas_FrequencyListDes_t));
	if (pstFrequencyListDes == NULL)
	{
		HxLOG_Print("[ERROR] allocFrequencyListDes ()\n");
		return NULL;
	}

	initFrequencyListDes (pstFrequencyListDes);
	return ((svcCas_FrequencyListDes_t *) pstFrequencyListDes);
}


STATIC void freeFrequencyListDes(svcCas_FrequencyListDes_t *pstDes)
{
	if (pstDes == NULL)
	{
		return;
	}

	if (pstDes->centreFreq != NULL)
	{
		OxCAS_Free(pstDes->centreFreq);
	}

	OxCAS_Free(pstDes);
}

/************************************************************************
 *
 * Partial Transport Stream Descriptor 관련... (TAG = 0x63)
 *
 ************************************************************************/
STATIC void initPartialTransportStreamDes(svcCas_PartialTsDes *pstDes)
{
	pstDes->ulPeakRate = 0;
	pstDes->ulMinimumOverallSmoothingRate = 0;
	pstDes->ulMaximumOverallSmoothingBuffer = 0;
}

STATIC svcCas_PartialTsDes *allocPartialTransportStreamDes(void)
{
	svcCas_PartialTsDes *pPartialTransportStreamDes;

	pPartialTransportStreamDes = (svcCas_PartialTsDes *) OxCAS_Malloc (sizeof (svcCas_PartialTsDes));
	if (pPartialTransportStreamDes == NULL)
	{
		HxLOG_Print("[ERROR] allocPartialTransportStreamDes ()\n");
		return NULL;
	}

	initPartialTransportStreamDes (pPartialTransportStreamDes);
	return ((svcCas_PartialTsDes *) pPartialTransportStreamDes);
}

STATIC void freePartialTransportStreamDes(svcCas_PartialTsDes *pstDes)
{
	if (pstDes == NULL)
	{
		return;
	}

	OxCAS_Free(pstDes);
}

/************************************************************************
 *
 * Data Broadcast Descriptor 관련... (TAG = 0x64)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitDataBroadcastInfo									*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initDataBroadcastInfo(svcCas_DataCarouselInfo_t *pstDataBroadcastInfo)
{
	pstDataBroadcastInfo->ucCarouselId = 0;
	pstDataBroadcastInfo->ulTransactionId = 0;
	pstDataBroadcastInfo->ulTimeOutValueDSI = 0;
	pstDataBroadcastInfo->ulTimeOutValueDII = 0;
	pstDataBroadcastInfo->ulLeakRate = 0;
}


/************************************************************************/
/*	function : AllocDataBroadcastInfo									*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_DataCarouselInfo_t 	*allocDataBroadcastInfo(void)
{
	svcCas_DataCarouselInfo_t	*pstDataBroadcastInfo;

	pstDataBroadcastInfo = (svcCas_DataCarouselInfo_t *)OxCAS_Malloc(sizeof(svcCas_DataCarouselInfo_t));
	if (pstDataBroadcastInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocDataBroadcastInfo()\n");
		return NULL;
	}
	initDataBroadcastInfo(pstDataBroadcastInfo);

	return ((svcCas_DataCarouselInfo_t *)pstDataBroadcastInfo);
}


/************************************************************************/
/*	function : SetDataBroadcastInfo										*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetDataBroadcastInfo(svcCas_DataCarouselInfo_t *pstDataBroadcastInfo
	, HUINT8 ucCarouselId, HUINT32 ulTransactionId, HUINT32 ulTimeOutValueDSI
	, HUINT32 ulTimeOutValueDII, HUINT32 ulLeakRate)
{
	pstDataBroadcastInfo->ucCarouselId = ucCarouselId;
	pstDataBroadcastInfo->ulTransactionId = ulTransactionId;
	pstDataBroadcastInfo->ulTimeOutValueDSI = ulTimeOutValueDSI;
	pstDataBroadcastInfo->ulTimeOutValueDII = ulTimeOutValueDII;
	pstDataBroadcastInfo->ulLeakRate = ulLeakRate;
}


/************************************************************************/
/*	function : FreeDataBroadcastInfo									*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeDataBroadcastInfo(svcCas_DataCarouselInfo_t *pstDataBroadcastInfo)
{
	if(pstDataBroadcastInfo != NULL)
	{
		OxCAS_Free(pstDataBroadcastInfo);
	}
}


/************************************************************************/
/*	function : initObjectCarouselInfo									*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initObjectCarouselInfo(svcCas_ObjectCarouselInfo_t *pstObjectCarouselInfo)
{
	pstObjectCarouselInfo->ucCarouselId = 0;
	pstObjectCarouselInfo->ulTransactionId = 0;
	pstObjectCarouselInfo->ulTimeOutValueDSI = 0;
	pstObjectCarouselInfo->ulTimeOutValueDII = 0;
	pstObjectCarouselInfo->ulLeakRate = 0;
	HxSTD_MemSet(pstObjectCarouselInfo->szIso639LangCode, '\0', 4);
	pstObjectCarouselInfo->pszObjName = NULL;
}


/************************************************************************/
/*	function : svcCas_ObjectCarouselInfo_t										*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_ObjectCarouselInfo_t 	*allocObjectCarouselInfo(void)
{
	svcCas_ObjectCarouselInfo_t	*pstObjectCarouselInfo;


	pstObjectCarouselInfo = (svcCas_ObjectCarouselInfo_t *)OxCAS_Malloc(sizeof(svcCas_ObjectCarouselInfo_t));
	if (pstObjectCarouselInfo == NULL)
	{
		HxLOG_Print("[ERROR] allocObjectCarouselInfo()\n");
		return NULL;
	}
	initObjectCarouselInfo(pstObjectCarouselInfo);

	return ((svcCas_ObjectCarouselInfo_t *)pstObjectCarouselInfo);
}


/************************************************************************/
/*	function : freeObjectCarouselInfo									*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeObjectCarouselInfo(svcCas_ObjectCarouselInfo_t *pstObjectCarouselInfo)
{
	if (pstObjectCarouselInfo == NULL)
	{
		return;
	}

	if (pstObjectCarouselInfo->pszObjName != NULL)
	{
		OxCAS_Free(pstObjectCarouselInfo->pszObjName);
	}

	OxCAS_Free(pstObjectCarouselInfo);
}


/************************************************************************/
/*	function : svc_cas_SiSetObjectCarouselInfo									*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetObjectCarouselInfo(svcCas_ObjectCarouselInfo_t *pstObjCarouselInfo
	, HUINT8 ucCarouselId, HUINT32 ulTransactionId, HUINT32 ulTimeOutValueDSI
	, HUINT32 ulTimeOutValueDII, HUINT32 ulLeakRate
	, HUINT8 *pszIso639LangCode, HUINT8 *pszObjName)
{
	pstObjCarouselInfo->ucCarouselId = ucCarouselId;
	pstObjCarouselInfo->ulTransactionId = ulTransactionId;
	pstObjCarouselInfo->ulTimeOutValueDSI = ulTimeOutValueDSI;
	pstObjCarouselInfo->ulTimeOutValueDII = ulTimeOutValueDII;
	pstObjCarouselInfo->ulLeakRate = ulLeakRate;
	if(pszIso639LangCode != NULL)
	    HxSTD_MemCopy(pstObjCarouselInfo->szIso639LangCode, pszIso639LangCode, svc_cas_SiUtilStrlen(pszIso639LangCode));
	if(pszObjName != NULL)
    	HxSTD_MemCopy(pstObjCarouselInfo->pszObjName, pszObjName, svc_cas_SiUtilStrlen(pszObjName));
}


/************************************************************************/
/*	function : InitDataBroadcastDes									*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initDataBroadcastDes(svcCas_DataBroadcastDes_t *pstDataBroadcastDes)
{
	pstDataBroadcastDes->usDataBroadcastId = 0;
	pstDataBroadcastDes->ucCompTag = 0;
	pstDataBroadcastDes->pszSelector = NULL;
	HxSTD_MemSet(pstDataBroadcastDes->szIso639LangCode, '\0', 4);
	pstDataBroadcastDes->pszText = NULL;
}


/************************************************************************/
/*	function : AllocDataBroadcastDes									*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_DataBroadcastDes_t 	*allocDataBroadcastDes(void)
{
	svcCas_DataBroadcastDes_t	*pstDataBroadcastDes;

	pstDataBroadcastDes = (svcCas_DataBroadcastDes_t *)OxCAS_Malloc(sizeof(svcCas_DataBroadcastDes_t));
	if (pstDataBroadcastDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocDataBroadcastDes()\n");
		return NULL;
	}
	initDataBroadcastDes(pstDataBroadcastDes);

	return ((svcCas_DataBroadcastDes_t *)pstDataBroadcastDes);
}


/************************************************************************/
/*	function : FreeDataBroadcastDes									*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeDataBroadcastDes(svcCas_DataBroadcastDes_t *pstDataBroadcastDes)
{
	if (pstDataBroadcastDes == NULL)
	{
		return;
	}

	if (pstDataBroadcastDes->usDataBroadcastId == 0x0006)
	{
		freeDataBroadcastInfo ((svcCas_DataCarouselInfo_t *)pstDataBroadcastDes->pszSelector);
	}

	if (pstDataBroadcastDes->usDataBroadcastId == 0x0007)
	{
		freeObjectCarouselInfo ((svcCas_ObjectCarouselInfo_t *)pstDataBroadcastDes->pszSelector);
	}

	if (pstDataBroadcastDes->pszText != NULL)
	{
		OxCAS_Free(pstDataBroadcastDes->pszText);
	}

	OxCAS_Free(pstDataBroadcastDes);
}


/************************************************************************/
/*	function : SetDataBroadcastDes										*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void 	svc_cas_SiSetDataBroadcastDes(svcCas_DataBroadcastDes_t *pstDataBroadcastDes
	, HUINT16 usDataBroadcastId, HUINT8 ucCompTag, HUINT8 *pszSelector
	, HUINT8 *pszIso639LangCode, HUINT8 *pszText )
{
	pstDataBroadcastDes->usDataBroadcastId = usDataBroadcastId;
	pstDataBroadcastDes->ucCompTag = ucCompTag;
	pstDataBroadcastDes->pszSelector = pszSelector;
	HxSTD_MemCopy(pstDataBroadcastDes->szIso639LangCode, pszIso639LangCode, svc_cas_SiUtilStrlen(pszIso639LangCode));
	pstDataBroadcastDes->szIso639LangCode[svc_cas_SiUtilStrlen(pszIso639LangCode)] = '\0';
	pstDataBroadcastDes->pszText = pszText;
}


/************************************************************************
 *
 * Data Broadcast ID Descriptor 관련... (TAG = 0x66)
 *
 ************************************************************************/
/************************************************************************/
/*	function : InitDataBroadcastIdDes									*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initDataBroadcastIdDes(svcCas_DataBroadcastIdDes_t *pstDataBroadcastIdDes)
{
	pstDataBroadcastIdDes->usDataBroadcastId = 0;
	pstDataBroadcastIdDes->usSelectorLen = 0;
	pstDataBroadcastIdDes->pszIdSelector = NULL;
}

/************************************************************************/
/*	function : AllocDataBroadcastIdDes									*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_DataBroadcastIdDes_t 	*allocDataBroadcastIdDes(void)
{
	svcCas_DataBroadcastIdDes_t	*pstDataBroadcastIdDes;

	pstDataBroadcastIdDes = (svcCas_DataBroadcastIdDes_t *)OxCAS_Malloc(sizeof(svcCas_DataBroadcastIdDes_t));
	if (pstDataBroadcastIdDes == NULL)
	{
		HxLOG_Print("[ERROR] allocDataBroadcastIdDes()\n");
		return NULL;
	}
	initDataBroadcastIdDes(pstDataBroadcastIdDes);

	return ((svcCas_DataBroadcastIdDes_t *)pstDataBroadcastIdDes);
}


/************************************************************************/
/*	function : FreeDataBroadcastIdDes										*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeDataBroadcastIdDes(svcCas_DataBroadcastIdDes_t *pstDataBroadcastIdDes)
{
	if (pstDataBroadcastIdDes == NULL)
	{
		return;
	}

	svc_cas_SiFreeMem(pstDataBroadcastIdDes->pszIdSelector);
	OxCAS_Free(pstDataBroadcastIdDes);
}

/************************************************************************
 *
 * PDC Descriptor 관련... (TAG = 0x69)
 *
 ************************************************************************/
/************************************************************************/
/*	function : AllocPDCDes											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_PDCDes_t 	*allocPDCDes (void)
{
	svcCas_PDCDes_t	*pstPDCDes;

	pstPDCDes = (svcCas_PDCDes_t *) OxCAS_Malloc (sizeof (svcCas_PDCDes_t));
	if (pstPDCDes == NULL)
	{
		HxLOG_Print("[ERROR] allocPDCDes ()\n");
		return NULL;
	}

	HxSTD_MemSet(pstPDCDes, 0, sizeof(svcCas_PDCDes_t));

	return ((svcCas_PDCDes_t *) pstPDCDes);
}

/************************************************************************/
/*	function : FreePDCDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freePDCDes (svcCas_PDCDes_t *pstPDCDes)
{
	if (pstPDCDes != NULL)
	{
		OxCAS_Free (pstPDCDes);
	}
}


/************************************************************************
 *
 * Dolby AC3 Descriptor 관련... (TAG = 0x6a)
 *
 ************************************************************************/
/************************************************************************/
/*	function : AllocDolbyAC3Des											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_DolbyAc3Des_t 	*allocDolbyAC3Des (void)
{
	svcCas_DolbyAc3Des_t	*pstDolbyAC3Des;

	pstDolbyAC3Des = (svcCas_DolbyAc3Des_t *) OxCAS_Malloc (sizeof (svcCas_DolbyAc3Des_t));
	if (pstDolbyAC3Des == NULL)
	{
		HxLOG_Print("[ERROR] AllocDolbyAC3Des ()\n");
		return NULL;
	}

	return ((svcCas_DolbyAc3Des_t *) pstDolbyAC3Des);
}

/************************************************************************/
/*	function : FreeDolbyAC3Des											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeDolbyAC3Des (svcCas_DolbyAc3Des_t *pstDolbyAC3Des)
{
	if (pstDolbyAC3Des != NULL)
	{
		OxCAS_Free (pstDolbyAC3Des);
	}
}

/************************************************************************
 *
 * Application Signalling Descriptor 관련... (TAG = 0x6f)
 *
 ************************************************************************/

STATIC	void	initAppsignallingInfo (svcCas_AppSignallingInfo_t *pAppSiganllingInfo)
{
	pAppSiganllingInfo->usAppType = 0;
	pAppSiganllingInfo->ucAitVersion = 0;
	pAppSiganllingInfo->pNext = NULL;
}

STATIC	svcCas_AppSignallingInfo_t			*allocAppSignallingInfo (void)
{
	svcCas_AppSignallingInfo_t	*pstAppSignallingInfo;

	pstAppSignallingInfo = (svcCas_AppSignallingInfo_t *) OxCAS_Malloc (sizeof (svcCas_AppSignallingInfo_t));
	if (pstAppSignallingInfo == NULL)
	{
		HxLOG_Print("[ERROR] AllocAppSignallingInfo ()\n");
		return NULL;
	}

	initAppsignallingInfo (pstAppSignallingInfo);
	return ((svcCas_AppSignallingInfo_t *) pstAppSignallingInfo);
}

STATIC	void	freeAppSignallingInfo (svcCas_AppSignallingInfo_t *pstAppSignallingInfo)
{
	if (pstAppSignallingInfo != NULL)
	{
		OxCAS_Free (pstAppSignallingInfo);
	}
}

STATIC	void	initAppsignallingDes (svcCas_AppSignallingDes_t *pstAppSiganllingDes)
{
	pstAppSiganllingDes->pInfo = NULL;
}

STATIC	svcCas_AppSignallingDes_t			*allocAppsignallingDes (void)
{
	svcCas_AppSignallingDes_t	*pstAppSignallingDes;

	pstAppSignallingDes = (svcCas_AppSignallingDes_t *) OxCAS_Malloc (sizeof (svcCas_AppSignallingDes_t));
	if (pstAppSignallingDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocAppSignallingDes ()\n");
		return NULL;
	}

	initAppsignallingDes (pstAppSignallingDes);
	return ((svcCas_AppSignallingDes_t *) pstAppSignallingDes);
}


STATIC	void	freeAppSignallingDes (svcCas_AppSignallingDes_t *pstAppSignallingDes)
{
	svcCas_AppSignallingInfo_t *pstInfo, *pstNext;

	if (pstAppSignallingDes == NULL)
	{
		return;
	}

	pstInfo = pstAppSignallingDes->pInfo;
	while (pstInfo != NULL)
	{
		pstNext = pstInfo->pNext;
		freeAppSignallingInfo (pstInfo);
		pstInfo = pstNext;
	}

	OxCAS_Free (pstAppSignallingDes);
}

#define	______TV_ANYTIME_DESCRIPTOR______________
/************************************************************************
 *
 * Default Authority Descriptor (TAG = 0x73)
 *
 ************************************************************************/
/************************************************************************/
/*	function : allocDefaultAuthorityDes									*/
/*  																	*/
/*  Description : 메모리 할당.								*/
/************************************************************************/
STATIC	svcCas_DefaultAuthorityDes_t	*allocDefaultAuthorityDes(void)
{
	svcCas_DefaultAuthorityDes_t	*pstDefaultAuthorityDes;

	pstDefaultAuthorityDes = (svcCas_DefaultAuthorityDes_t*)OxCAS_Malloc(sizeof(svcCas_DefaultAuthorityDes_t));
	if (pstDefaultAuthorityDes == NULL)
	{
		HxLOG_Print("[ERROR] allocDefaultAuthorityDes()\n");
		return NULL;
	}

	HxSTD_MemSet(pstDefaultAuthorityDes, 0, sizeof(svcCas_DefaultAuthorityDes_t));

	return ((svcCas_DefaultAuthorityDes_t*)pstDefaultAuthorityDes);
}

/************************************************************************/
/*	function : freeDefaultAuthorityDes									*/
/*  																	*/
/*  Description : 메모리 해제.								*/
/************************************************************************/
STATIC	void	freeDefaultAuthorityDes(svcCas_DefaultAuthorityDes_t *pstDefaultAuthorityDes)
{
	if (pstDefaultAuthorityDes)
	{
		if (pstDefaultAuthorityDes->pszDefaultAuthority != NULL)
			OxCAS_Free(pstDefaultAuthorityDes->pszDefaultAuthority);

		OxCAS_Free(pstDefaultAuthorityDes);
	}
}

/************************************************************************
 *
 * Related Content Descriptor (TAG = 0x74)
 *
 ************************************************************************/
/************************************************************************/
/*	function : allocRelatedContentDes									*/
/*  																	*/
/*  Description : 메모리 할당.								*/
/************************************************************************/
STATIC	svcCas_RelatedContentDes_t	*allocRelatedContentDes(void)
{
	svcCas_RelatedContentDes_t	*pstRelatedContentDes;

	pstRelatedContentDes = (svcCas_RelatedContentDes_t*)OxCAS_Malloc(sizeof(svcCas_RelatedContentDes_t));
	if (pstRelatedContentDes == NULL)
	{
		HxLOG_Print("[ERROR] allocDefaultAuthorityDes()\n");
		return NULL;
	}

	HxSTD_MemSet(pstRelatedContentDes, 0, sizeof(svcCas_RelatedContentDes_t));

	return ((svcCas_RelatedContentDes_t*)pstRelatedContentDes);
}

/************************************************************************/
/*	function : freeRelatedContentDes									*/
/*  																	*/
/*  Description : 메모리 해제.								*/
/************************************************************************/
STATIC	void	freeRelatedContentDes(svcCas_RelatedContentDes_t *pstRelatedContentDes)
{
	if (NULL != pstRelatedContentDes)
	{
		if (pstRelatedContentDes->pPrivateData != NULL)
			OxCAS_Free(pstRelatedContentDes->pPrivateData);

		OxCAS_Free(pstRelatedContentDes);
	}
}

/************************************************************************
 *
 * Content Identifier Desc (TAG = 0x76)
 *
 ************************************************************************/
STATIC	void	initCridInfo(svcCas_CridInfo_t *pstCridInfo)
{
	HxSTD_MemSet(pstCridInfo, 0, sizeof(svcCas_CridInfo_t));
}

STATIC	svcCas_CridInfo_t	*allocCridInfo(void)
{
	svcCas_CridInfo_t	*pstCridInfo;

	pstCridInfo = (svcCas_CridInfo_t *)OxCAS_Malloc(sizeof(svcCas_CridInfo_t));
	if (pstCridInfo == NULL)
	{
		HxLOG_Print("[ERROR] allocCridInfo ()\n");
		return NULL;
	}

	initCridInfo(pstCridInfo);

	return ((svcCas_CridInfo_t *)pstCridInfo);
}

STATIC	void	freeCridInfo(svcCas_CridInfo_t *pstCridInfo)
{
	if (pstCridInfo)
	{
		if (pstCridInfo->pszCridBytes != NULL)
			OxCAS_Free(pstCridInfo->pszCridBytes);

		OxCAS_Free(pstCridInfo);
	}
}

STATIC	svcCas_ContentIdentifierDes_t	*allocContentIdentifierDes(void)
{
	svcCas_ContentIdentifierDes_t	*pstContentIdentifierDes;

	pstContentIdentifierDes = (svcCas_ContentIdentifierDes_t *)OxCAS_Malloc(sizeof(svcCas_ContentIdentifierDes_t));
	if (pstContentIdentifierDes == NULL)
	{
		HxLOG_Print("[ERROR] allocContentIdentifierDes()\n");
		return NULL;
	}

	HxSTD_MemSet(pstContentIdentifierDes, 0, sizeof(svcCas_ContentIdentifierDes_t));

	return ((svcCas_ContentIdentifierDes_t *)pstContentIdentifierDes);
}

STATIC	void	freeContentIdentifierDes(svcCas_ContentIdentifierDes_t *pstContentIdentifierDes)
{
	svcCas_CridInfo_t	*pstCridInfo, *pstNext;

	if (pstContentIdentifierDes == NULL)
		return;

	pstCridInfo = pstContentIdentifierDes->pInfo;
	while (pstCridInfo != NULL)
	{
		pstNext = pstCridInfo->pNext;
		freeCridInfo(pstCridInfo);
		pstCridInfo = pstNext;
	}

	OxCAS_Free(pstContentIdentifierDes);
}

#if 0
/************************************************************************
 *
 * Relative Channel Descriptor 관련... (TAG = 0x9D)
 *
 ************************************************************************/

STATIC mwRelativeChannelDes *allocRelativeChannelDes(void)
{
	mwRelativeChannelDes *pRelativeChannelDes;

	pRelativeChannelDes = (mwRelativeChannelDes *) OxCAS_Malloc (sizeof (mwRelativeChannelDes));
	if (pRelativeChannelDes == NULL)
	{
		HxLOG_Print("[ERROR] allocRelativeChannelDes ()\n");
		return NULL;
	}

	initRelativeChannelDes (pRelativeChannelDes);
	return ((mwRelativeChannelDes *) pRelativeChannelDes);
}

STATIC void initRelativeChannelDes(mwRelativeChannelDes *pstDes)
{
	pstDes->numRelativeChannel = 0;
	pstDes->chInfo = NULL;
}

STATIC void freeRelativeChannelDes(mwRelativeChannelDes *pstDes)
{
	if (pstDes == NULL)
	{
		return;
	}

	if (pstDes->chInfo != NULL)
	{
		OxCAS_Free(pstDes->chInfo);
	}

	OxCAS_Free(pstDes);
}


/************************************************************************
 *
 * OTA Group Descriptor 관련... (TAG = 0xC5)
 *
 ************************************************************************/
/************************************************************************/
/*	function : allocOtaGroupDes											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/
STATIC	mwOtaGroupInfo			*allocOtaGroupInfo (void)
{
	mwOtaGroupInfo	*pOtaGroupInfo;

	pOtaGroupInfo = (mwOtaGroupInfo *) OxCAS_Malloc (sizeof (mwOtaGroupInfo));
	if (pOtaGroupInfo == NULL)
	{
		HxLOG_Print("[ERROR] allocOtaGroupInfo ()\n");
		return NULL;
	}

	initOtaGroupInfo (pOtaGroupInfo);
	return ((mwOtaGroupInfo *) pOtaGroupInfo);
}

STATIC 	mwOtaGroupDes 	*allocOtaGroupDes(void)
{
	mwOtaGroupDes	*pOtaGroupDes;

	pOtaGroupDes = (mwOtaGroupDes *) OxCAS_Malloc(sizeof(mwOtaGroupDes));
	if (pOtaGroupDes == NULL)
	{
		HxLOG_Print("[ERROR] allocOtaGroupDes()\n");
		return NULL;
	}
	initOtaGroupDes (pOtaGroupDes);

	return ((mwOtaGroupDes *) pOtaGroupDes);
}

/************************************************************************/
/*	function : freeOtaGroupDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeOtaGroupInfo(mwOtaGroupInfo *pOtaGroupInfo)
{
	if(pOtaGroupInfo == NULL)
	{
		return;
	}

	if(pOtaGroupInfo->pucPrivData)
	{
		svc_cas_SiFreeMem (pOtaGroupInfo->pucPrivData);
	}

	if(pOtaGroupInfo->pucGameName)
	{
		svc_cas_SiFreeMem (pOtaGroupInfo->pucGameName);
	}

	if(pOtaGroupInfo->pucGameDesc)
	{
		svc_cas_SiFreeMem (pOtaGroupInfo->pucGameDesc);
	}

	OxCAS_Free(pOtaGroupInfo);
}

/************************************************************************/
/*	function : InitOtaGroupDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initOtaGroupInfo (mwOtaGroupInfo *pOtaGroupInfo)
{
	pOtaGroupInfo->usGroupId = 0;
	pOtaGroupInfo->ulProdId_Start = 0;
	pOtaGroupInfo->ulProdId_End = 0;
	pOtaGroupInfo->usHdfVersion = 0;
	pOtaGroupInfo->usDataType = 0;
	pOtaGroupInfo->ulDataVersion = 0;
	pOtaGroupInfo->usDownloadLevel = 0;

	pOtaGroupInfo->pucPrivData = NULL;
	pOtaGroupInfo->ucPrivDataLen = 0;

	pOtaGroupInfo->pucGameName = NULL;
	pOtaGroupInfo->ucGameNameLen = 0;

	pOtaGroupInfo->pucGameDesc = NULL;
	pOtaGroupInfo->ucGameDescLen = 0;

	pOtaGroupInfo->pNextInfo = NULL;
}

STATIC	void	freeOtaGroupDes (mwOtaGroupDes *pOtaGroupDes)
{
	mwOtaGroupInfo *pInfo, *pstNext; // JPROH

	if(pOtaGroupDes == NULL)
	{
		return;
	}

	pInfo = pOtaGroupDes->pInfo;
	while (pInfo != NULL)
	{
		pstNext = pInfo->pNextInfo;
		freeOtaGroupInfo (pInfo);
		pInfo = pstNext;
	}

	OxCAS_Free (pOtaGroupDes);
}

STATIC	void	initOtaGroupDes (mwOtaGroupDes *pOtaGroupDes)
{
	pOtaGroupDes->pInfo = NULL;
}




/************************************************************************/
/*	function : svc_cas_SiSetOtaGroupDes											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

void	svc_cas_SiSetOtaGroupInfo (mwOtaGroupInfo *pOtaGroupInfo, HUINT16 usGroupId,
	HUINT32 ulProdId_Start, HUINT32 ulProdId_End, HUINT16 usHdfVersion,
	HUINT16	usDataType, HUINT32 ulDataVersion, HUINT16 usDownloadLevel,
	HUINT8	*pucPrivData, HUINT8 ucPrivDataLen)
{
	pOtaGroupInfo->usGroupId = usGroupId;
	pOtaGroupInfo->ulProdId_Start = ulProdId_Start;
	pOtaGroupInfo->ulProdId_End = ulProdId_End;
	pOtaGroupInfo->usHdfVersion = usHdfVersion;
	pOtaGroupInfo->usDataType = usDataType;
	pOtaGroupInfo->ulDataVersion = ulDataVersion;
	pOtaGroupInfo->usDownloadLevel = usDownloadLevel;
	pOtaGroupInfo->pucPrivData = pucPrivData;
	pOtaGroupInfo->ucPrivDataLen = ucPrivDataLen;
}
#endif

/************************************************************************
 *
 * Unknown descriptor
 *
 ************************************************************************/

STATIC void initS2SatDeliSystemDes(svcCas_S2SatDeliSystemDes_t *pstS2SatDeliSystemDes)
{
	pstS2SatDeliSystemDes->ucBackCompatibilityIndicator	= 0;
	pstS2SatDeliSystemDes->ucInputStreamIdentifier		= 0;
	pstS2SatDeliSystemDes->ucMultipleInStreamFlag		= 0;
	pstS2SatDeliSystemDes->ucScrambleSeqSelect			= 0;
	pstS2SatDeliSystemDes->ulScramblingSeqIndex			= 0;
}

STATIC svcCas_S2SatDeliSystemDes_t *allocS2SatDeliSystemDes(void)
{
	svcCas_S2SatDeliSystemDes_t	*pstS2SatDeliSystemDes;

	pstS2SatDeliSystemDes = (svcCas_S2SatDeliSystemDes_t *)OxCAS_Malloc(sizeof(svcCas_S2SatDeliSystemDes_t));
	if (pstS2SatDeliSystemDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocS2SatDeliSystemDes()\n");
		return NULL;
	}

	initS2SatDeliSystemDes(pstS2SatDeliSystemDes);

	return pstS2SatDeliSystemDes;
}

STATIC void freeS2SatDeliSystemDes(svcCas_S2SatDeliSystemDes_t *pstS2SatDeliSystemDes)
{
	if (pstS2SatDeliSystemDes != NULL)
	{
		OxCAS_Free (pstS2SatDeliSystemDes);
	}
}

STATIC void initEnhancedAC3Des(svcCas_EnhancedAC3Des_t *pstEnhancedAC3Des)
{
	pstEnhancedAC3Des->bComponentTypeFlag 	= FALSE;
	pstEnhancedAC3Des->bBsidFlag			= FALSE;
	pstEnhancedAC3Des->bMainIdFlag			= FALSE;
	pstEnhancedAC3Des->bAsvcFlag			= FALSE;
	pstEnhancedAC3Des->bMixinfoexists 		= FALSE;
	pstEnhancedAC3Des->bSubStream1Flag 		= FALSE;
	pstEnhancedAC3Des->bSubStream2Flag 		= FALSE;
	pstEnhancedAC3Des->bSubStream3Flag 		= FALSE;
	pstEnhancedAC3Des->ucComponentType		= 0;
	pstEnhancedAC3Des->ucBsId				= 0;
	pstEnhancedAC3Des->ucMainId				= 0;
	pstEnhancedAC3Des->ucAsvc				= 0;
	pstEnhancedAC3Des->ucSubStream1			= 0;
	pstEnhancedAC3Des->ucSubStream2			= 0;
	pstEnhancedAC3Des->ucSubStream3			= 0;

}

STATIC svcCas_EnhancedAC3Des_t *allocEnhancedAC3Des(void)
{
	svcCas_EnhancedAC3Des_t	*pstEnhancedAC3Des;

	pstEnhancedAC3Des = (svcCas_EnhancedAC3Des_t *)OxCAS_Malloc(sizeof(svcCas_EnhancedAC3Des_t));
	if (pstEnhancedAC3Des == NULL)
	{
		HxLOG_Print("[ERROR] AllocEnhancedAC3Des()\n");
		return NULL;
	}

	initEnhancedAC3Des(pstEnhancedAC3Des);

	return pstEnhancedAC3Des;
}

STATIC void initAACDes(svcCas_AACDes_t *pstAACDes)
{
	pstAACDes->ucProfileAndLevel	= 0;
	pstAACDes->bAACTypeFlag			= FALSE;
	pstAACDes->ucAACType			= 0;
}

STATIC svcCas_AACDes_t *allocAACDes(void)
{
	svcCas_AACDes_t	*pstAACDes;

	pstAACDes = (svcCas_AACDes_t *)OxCAS_Malloc(sizeof(svcCas_AACDes_t));
	if (pstAACDes == NULL)
	{
		HxLOG_Print("[ERROR] AllocAACDes()\n");
		return NULL;
	}

	initAACDes(pstAACDes);

	return pstAACDes;
}


STATIC void freeEnhancedAC3Des(svcCas_EnhancedAC3Des_t *pstEnhancedAC3Des)
{
	if(pstEnhancedAC3Des != NULL)
		OxCAS_Free (pstEnhancedAC3Des);
}

STATIC void freeAACDes(svcCas_AACDes_t *pstAACDes)
{
	if(pstAACDes != NULL)
		OxCAS_Free (pstAACDes);
}

#define ____TAG_CELL_LIST____

STATIC void initSubCellListInfo(svcCas_SubCellListInfo_t *pstSubCellListInfo)
{
	pstSubCellListInfo->ucCellIdExtension = 0;
	pstSubCellListInfo->usSubCellLatitude = 0;
	pstSubCellListInfo->usSubCellLongitude = 0;
	pstSubCellListInfo->usSubCellExtentOfLatitude = 0;
	pstSubCellListInfo->usSubCellExtentOfLongitude = 0;
	pstSubCellListInfo->pNextInfo = NULL;
}

STATIC void initCellListInfo(svcCas_CellListInfo_t *pstCellListInfo)
{
	pstCellListInfo->usCellId = 0;
	pstCellListInfo->usCellLatitude = 0;
	pstCellListInfo->usCellLongitude = 0;
	pstCellListInfo->usCellExtentOfLatitude = 0;
	pstCellListInfo->usCellExtentOfLongitude = 0;
	pstCellListInfo->pSubCellListInfo = NULL;
	pstCellListInfo->pNextInfo = NULL;
}

STATIC void initCellListDes(svcCas_CellListDes_t *pstCellListDes)
{
	pstCellListDes->pCellListInfo				= NULL;
}

svcCas_SubCellListInfo_t *svc_cas_SiAllocSubCellListInfo(void)
{
	svcCas_SubCellListInfo_t	*pstSubCellListInfo;

	pstSubCellListInfo = (svcCas_SubCellListInfo_t *)OxCAS_Malloc(sizeof(svcCas_SubCellListInfo_t));
	if (NULL == pstSubCellListInfo)
	{
		HxLOG_Print("[ERROR] svc_cas_SiAllocSubCellListInfo()\n");
		return NULL;
	}

	initSubCellListInfo(pstSubCellListInfo);

	return pstSubCellListInfo;
}

STATIC svcCas_CellListInfo_t *allocCellListInfo(void)
{
	svcCas_CellListInfo_t	*pstCellListInfo;

	pstCellListInfo = (svcCas_CellListInfo_t *)OxCAS_Malloc(sizeof(svcCas_CellListInfo_t));
	if (NULL == pstCellListInfo)
	{
		HxLOG_Print("[ERROR] allocCellListInfo()\n");
		return NULL;
	}

	initCellListInfo (pstCellListInfo);

	return pstCellListInfo;
}

STATIC svcCas_CellListDes_t *allocCellListDes(void)
{
	svcCas_CellListDes_t	*pstCellListDes;

	pstCellListDes = (svcCas_CellListDes_t *)OxCAS_Malloc(sizeof(svcCas_CellListDes_t));
	if (NULL == pstCellListDes)
	{
		HxLOG_Print("[ERROR] allocCellListDes()\n");
		return NULL;
	}

	initCellListDes(pstCellListDes);

	return pstCellListDes;
}

STATIC void freeSubCellListInfo(svcCas_SubCellListInfo_t *pstSubCellListInfo)
{
	svcCas_SubCellListInfo_t	*pstTmpSubCellListInfo;

	while(pstSubCellListInfo)
	{
		pstTmpSubCellListInfo = pstSubCellListInfo;
		pstSubCellListInfo = pstSubCellListInfo->pNextInfo;
		OxCAS_Free (pstTmpSubCellListInfo);

	}
}

STATIC void freeCellListInfo(svcCas_CellListInfo_t *pstCellListInfo)
{
	svcCas_CellListInfo_t	*pstTmpCellListInfo;
	while(pstCellListInfo)
	{
		pstTmpCellListInfo = pstCellListInfo;
		pstCellListInfo = pstCellListInfo->pNextInfo;

		if (pstTmpCellListInfo->pSubCellListInfo)
		{
			freeSubCellListInfo(pstTmpCellListInfo->pSubCellListInfo);
		}
		OxCAS_Free (pstTmpCellListInfo);
	}
}

STATIC void freeCellListDes(svcCas_CellListDes_t *pstCellListDes)
{
	if(pstCellListDes)
	{
		if (pstCellListDes->pCellListInfo)
		{
			freeCellListInfo(pstCellListDes->pCellListInfo);
		}
		OxCAS_Free (pstCellListDes);
	}
}
#define ____FTA_CONTENTMANAGEMENT_DES____

STATIC void initFtaContentManageDes(svcCas_FtaContentManagementDes_t *pstFtaContentManageDes)
{
	pstFtaContentManageDes->ucDoNotScramble = 0xff;
	pstFtaContentManageDes->ucDoNotApplyRevocation = 0xff;
	pstFtaContentManageDes->ucCtrlRAccOverINet = 0xff;
}

STATIC svcCas_FtaContentManagementDes_t *allocFtaContentManageDes (void)
{
	svcCas_FtaContentManagementDes_t	*pstFtaContentManageDes;

	pstFtaContentManageDes = (svcCas_FtaContentManagementDes_t *)OxCAS_Malloc(sizeof(svcCas_FtaContentManagementDes_t));
	if (NULL == pstFtaContentManageDes)
	{
		HxLOG_Print("[ERROR] allocFtaContentManageDes()\n");
		return NULL;
	}
	initFtaContentManageDes(pstFtaContentManageDes);

	return ((svcCas_FtaContentManagementDes_t *)pstFtaContentManageDes);
}

STATIC void freeFtaContentManageDes (svcCas_FtaContentManagementDes_t *pstFtaContentManageDes)
{
	if (NULL != pstFtaContentManageDes)
	{
		OxCAS_Free(pstFtaContentManageDes);
	}
}

#define ____EXTENSION_DES____

/************************************************************************
 *
 * Extension Descriptor 관련... (TAG = 0x7F)
 *
 * 이 descriptor는 EN 300 468 1.10.1버전이상의 문서에서
 * 6.2.16   Extension descriptor.
 * 6.3        Extended descriptor identification and location
 * 6.4        Extended descriptor coding
 * 항목을 참조해야 한다. 기존에 사용되지 않던 새로운 방법임. 2009.09.09 by ultracat
 ************************************************************************/

STATIC void initExtensionDes(svcCas_ExtensionDes_t *pstExtensionDes)
{
	pstExtensionDes->ucExtDesTag = 0xff;
	pstExtensionDes->ucExtDesLen = 0;
	pstExtensionDes->pExtDesData = NULL;
}

STATIC svcCas_ExtensionDes_t *allocExtensionDes (void)
{
	svcCas_ExtensionDes_t	*pstExtensionDes;

	pstExtensionDes = (svcCas_ExtensionDes_t *)OxCAS_Malloc(sizeof(svcCas_ExtensionDes_t));
	if (NULL == pstExtensionDes)
	{
		HxLOG_Print("[ERROR] allocExtensionDes()\n");
		return NULL;
	}
	initExtensionDes(pstExtensionDes);

	return ((svcCas_ExtensionDes_t *)pstExtensionDes);
}

STATIC void freeExtensionDes (svcCas_ExtensionDes_t *pstExtensionDes)
{
	if (NULL != pstExtensionDes)
	{
		if (NULL != pstExtensionDes->pExtDesData)
		{
			OxCAS_Free(pstExtensionDes->pExtDesData);
		}
		OxCAS_Free(pstExtensionDes);
	}
}

#define ____USER_DEFINED_DES____

STATIC HUINT8 *allocUserDefinedDes (HUINT32 ulDesLen)
{
	HUINT8		*pucDes;

	pucDes = (HUINT8 *)OxCAS_Malloc (ulDesLen + 2);
	if (pucDes != NULL)
	{
		HxSTD_MemSet(pucDes, 0, ulDesLen + 2);
	}

	return pucDes;
}

STATIC void freeUserDefinedDes (HUINT8 *pucDes)
{
	if (pucDes != NULL)
	{
		OxCAS_Free (pucDes);
	}
}

#define ____COMMON_CODE____
/************************************************************************
 *
 * Descriptor 관련....
 *
 ************************************************************************/

/************************************************************************/
/*	function : AllocDes													*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

HUINT8 	*svc_cas_SiAllocDes(HINT32 lDesTag, HUINT8 ucDesLen)
{
	HUINT8					*pstNewDes;
	svcCas_SiBaseDesForm_t	*pstBaseForm;

	pstNewDes = NULL;

	if (MACRO_IS_USER_DEFINED_DESTAG(lDesTag))
	{
		pstNewDes = allocUserDefinedDes ((HUINT32)ucDesLen);
		if (pstNewDes != NULL)
		{
			pstNewDes[0] = (HUINT8)lDesTag;
		}

		return pstNewDes;
	}

	switch (lDesTag)
	{
	case TAG_VIDEO_STREAM:		/** < tag = 0x02 */
		pstNewDes = (HUINT8 *)svc_cas_SiDesAllocVideoStmDes();
		break;

	case TAG_AUDIO_STREAM:		/** < tag = 0x03 */
		pstNewDes = (HUINT8 *)svc_cas_SiDesAllocAudioStmDes();
		break;

	case TAG_HIERARCHY:			/** < tag = 0x04 */
		pstNewDes = (HUINT8 *)svc_cas_SiDesAllocHierDes();
		break;

	case TAG_REGISTRATION:		/** < tag = 0x05 */
		pstNewDes = (HUINT8 *)svc_cas_SiDesAllocRegDes();
		break;

	case TAG_DATA_STREAM_ALIGNMENT:		/** < tag = 0x06 */
		pstNewDes = (HUINT8 *)svc_cas_SiDesAllocDataStmAlignDes();
		break;

	case TAG_TARGET_BACKGROUND_GRID:	/** < tag = 0x07 */
		pstNewDes = (HUINT8 *)svc_cas_SiDesAllocBackGridDes();
		break;

	case TAG_VIDEO_WINDOW:		/** < tag = 0x08 */
		pstNewDes = (HUINT8 *)svc_cas_SiDesAllocVideoWindowDes();
		break;

	case TAG_CA:				/** < tag = 0x09 */
		pstNewDes = (HUINT8 *)svc_cas_SiDesAllocCaDes();
		break;

	case TAG_ISO_639_LANGUAGE:	/** < tag = 0x0A */
		pstNewDes = (HUINT8 *)svc_cas_SiDesAllocIso639LangDes();
		break;

	case TAG_SYSTEM_CLOCK:		/** < tag = 0x0B */
		pstNewDes = (HUINT8 *)svc_cas_SiDesAllocSysClockDes();
		break;

	case TAG_MULTIPLEX_BUFFER_UTILIZATION:		/** < tag = 0x0C */
		pstNewDes = (HUINT8 *)svc_cas_SiDesAllocMultiplexBufUtilDes();
		break;

	case TAG_COPYRIGHT:			/** < tag = 0x0D */
		pstNewDes = (HUINT8 *)svc_cas_SiDesAllocCopyrightDes();
		break;

	case TAG_MAXIMUM_BITRATE:	/** < tag = 0xE */
		pstNewDes = (HUINT8 *)svc_cas_SiDesAllocMaxBitrateDes();
		break;

	case TAG_PRIVATE_DATA_INDICATOR:	/** < tag = 0xF */
		pstNewDes = (HUINT8 *)svc_cas_SiDesAllocPrivateDataIndicatorDes();
		break;

	case TAG_SMOOTHING_BUFFER:			/** < tag = 0x10 */
		pstNewDes = (HUINT8 *)svc_cas_SiDesAllocSmoothBufDes();
		break;

	case TAG_STD:			/** < tag = 0x11 */
		pstNewDes = (HUINT8 *)svc_cas_SiDesAllocStdDes();
		break;

	case TAG_CAROUSEL_ID:		/** < tag = 0x13 */
		pstNewDes = (HUINT8 *)svc_cas_SiDesAllocCarouselIdDes();
		break;

	case TAG_ASSOCIATION_TAG:	/** < tag = 0x14 */
		pstNewDes = (HUINT8 *)svc_cas_SiDesAllocAssocTagDes();
		break;

	case TAG_DEFERRED_ASSOCIATION_TAG:		/** < tag = 0x15 */
		pstNewDes = (HUINT8 *)svc_cas_SiDesAllocDeferAssocTagDes();
		break;

	case TAG_AVC_VIDEO:		/** < tag = 0x28 */
		pstNewDes = (HUINT8 *)svc_cas_SiDesAllocAvcVideoDes();
		break;

	case TAG_NETWORK_NAME:		/** < tag = 0x40 */
		pstNewDes = (HUINT8 *)svc_cas_SiDesAllocNetNameDes();
		break;

	case TAG_SERVICE_LIST:		/** < tag = 0x41 */
		pstNewDes = (HUINT8 *)allocServListDes();
		break;

	case TAG_SATELLITE_DELIVERY_SYSTEM:		/** < tag = 0x43 */
		pstNewDes = (HUINT8 *)allocSatDeliSystemDes();
		break;

	case TAG_CABLE_DELIVERY_SYSTEM:			/** < tag = 0x44 */
		pstNewDes = (HUINT8 *)allocCabDeliSystemDes();
		break;

	case TAG_BOUQUET_NAME:			/** < tag = 0x47 */
		pstNewDes = (HUINT8 *)allocBouqNameDes();
		break;

	case TAG_SERVICE:		/** < tag = 0x48 */
		pstNewDes = (HUINT8 *)allocServDes();
		break;

	case TAG_COUNTRY_AVAILABILITY:		/** < tag = 0x49 */
		pstNewDes = (HUINT8 *)allocCountryAvailDes();
		break;

	case TAG_LINKAGE:		/** < tag = 0x4A */
		pstNewDes = (HUINT8 *)allocLinkDes();
		break;

	case TAG_NVOD_REFERENCE:		/** < tag = 4B */
		pstNewDes = (HUINT8 *)allocNvodRefDes();
		break;

	case TAG_TIME_SHIFTED_SERVICE:		/** < tag = 4C */
		pstNewDes = (HUINT8 *)allocTimeShiftServDes();
		break;

	case TAG_SHORT_EVENT:		/** < tag = 4D */
		pstNewDes = (HUINT8 *)allocShortEvtDes();
		break;

	case TAG_EXTENDED_EVENT:		/** < tag = 4E */
		pstNewDes = (HUINT8 *)allocExtEvtDes();
		break;

	case TAG_TIME_SHIFTED_EVENT:		/** < tag = 4F */
		pstNewDes = (HUINT8 *)allocTimeShiftEvtDes();
		break;

	case TAG_COMPONENT:		/** < tag = 0x50 */
		pstNewDes = (HUINT8 *)allocCompDes();
		break;

	case TAG_STREAM_IDENTIFIER:		/** < tag = 0x52 */
		pstNewDes = (HUINT8 *)allocStmIdentDes();
		break;

	case TAG_CA_IDENTIFIER:		/** < tag = 0x53 */
		pstNewDes = (HUINT8 *)allocCaIdentDes();
		break;

	case TAG_CONTENT:		/** < tag = 0x54 */
		pstNewDes = (HUINT8 *)allocContentDes();
		break;

	case TAG_PARENTAL_RATING:		/** < tag = 0x55 */
		pstNewDes = (HUINT8 *)allocParentDes();
		break;

	case TAG_TELETEXT:		/** < tag = 0x56 */
		pstNewDes = (HUINT8 *)allocTeletextDes();
		break;

	case TAG_TELEPHONE:		/** < tag = 0x57 */
		pstNewDes = (HUINT8 *)allocTelephoneDes();
		break;

	case TAG_LOCAL_TIME_OFFSET:		/** < tag = 0x58 */
		pstNewDes = (HUINT8 *)allocLocalTimeDes();
		break;

	case TAG_SUBTITLING:		/** < tag = 0x59 */
		pstNewDes = (HUINT8 *)allocSubTitleDes();
		break;

	case TAG_TERRESTRIAL_DELIVERY_SYSTEM:		/** < tag = 0x5A */
		pstNewDes = (HUINT8 *)allocTerDeliSystemDes();
		break;

	case TAG_MULTILINGUAL_NETWORK_NAME:			/** < tag = 0x5B */
		pstNewDes = (HUINT8 *)allocMultiNetNameDes();
		break;

	case TAG_MULTILINGUAL_BOUQUET_NAME:			/** < tag = 0x5C */
		pstNewDes = (HUINT8 *)allocMultiBouqNameDes();
		break;

	case TAG_MULTILINGUAL_SERVICE_NAME:			/** < tag = 0x5D */
		pstNewDes = (HUINT8 *)allocMultiServNameDes();
		break;

	case TAG_MULTILINGUAL_COMPONENT:			/** < tag = 0x5E */
		pstNewDes = (HUINT8 *)allocMultiCompDes();
		break;

	case TAG_PRIVATE_DATA_SPECIFIER:			/** < tag = 0x5F */
		pstNewDes = (HUINT8 *)allocPrivateDataSpecDes();
		break;

	case TAG_SERVICE_MOVE:			/** < tag = 0x60 */
		pstNewDes = (HUINT8 *)allocServMoveDes();
		break;

	case TAG_SHORT_SMOOTHING_BUFFER:		/** < tag = 0x61 */
		pstNewDes = (HUINT8 *)allocShortSmoothBufDes();
		break;

	case TAG_FREQUENCY_LIST:		/** < tag = 0x62 */
		pstNewDes = (HUINT8 *)allocFrequencyListDes();
		break;

	case TAG_PARTIAL_TRANSPORT_STREAM:
		pstNewDes = (HUINT8 *)allocPartialTransportStreamDes();
		break;

	case TAG_DATA_BROADCAST: 		/** < tag = 0x64 */
		pstNewDes = (HUINT8 *)allocDataBroadcastDes();
		break;

	case TAG_DATA_BROADCAST_ID:		/** < tag = 0x66 */
		pstNewDes = (HUINT8 *)allocDataBroadcastIdDes();
		break;

	case TAG_PDC:			/** < tag = 0x69 */
		pstNewDes = (HUINT8 *)allocPDCDes();
		break;

	case TAG_DOLBY_AC3:			/** < tag = 0x6A */
		pstNewDes = (HUINT8 *)allocDolbyAC3Des();
		break;

	case TAG_APPLICATION_SIGNALLING:		/** < tag = 0x6F */
		pstNewDes = (HUINT8 *)allocAppsignallingDes();
		break;

	case TAG_DEFAULT_AUTORITY:	/** < tag = 0x73 */
		pstNewDes = (HUINT8 *)allocDefaultAuthorityDes();
		break;

	case TAG_RELATED_CONTENT:	/** < tag = 0x74 */
		pstNewDes = (HUINT8 *)allocRelatedContentDes();
		break;

	case TAG_CONTENT_IDENTIFIER:	/** < tag = 0x76 */
		pstNewDes = (HUINT8 *)allocContentIdentifierDes();
		break;

	case TAG_S2_SATELLITE_DELIVERY_SYSTEM:		/** < tag = 0x79 */
		pstNewDes = (HUINT8 *)allocS2SatDeliSystemDes();
		break;

	case TAG_ENHANCED_AC3:			/** < tag = 0x7A */
		pstNewDes =(HUINT8 *)allocEnhancedAC3Des();
		break;

	case TAG_AAC:			/** < tag = 0x7C */
		pstNewDes =(HUINT8 *)allocAACDes();
		break;

	case TAG_CELL_LIST:
		pstNewDes =(HUINT8 *)allocCellListDes();
		break;

	case TAG_FTA_CONTENT_MANAGEMENT:
		pstNewDes = (HUINT8 *)allocFtaContentManageDes();
		break;

	case TAG_EXTENSION:		/** < tag = 0x7F */
		pstNewDes = (HUINT8 *)allocExtensionDes();
		break;

	default:
		// Not the user-defined descriptors, not defined:
		// then return NULL.
		pstNewDes = NULL;
		break;
	}

	pstBaseForm = (svcCas_SiBaseDesForm_t *)pstNewDes;
	if (pstBaseForm != NULL)
	{
		pstBaseForm->ucDesTag = (HUINT8)lDesTag;
	}

	return ((HUINT8 *)pstNewDes);
}

/************************************************************************/
/*	function : FreeDes													*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/
void 	svc_cas_SiFreeDes(HUINT8 *pucDesPtr)
{
	if (pucDesPtr == NULL)
	{
		return;
	}

	if (MACRO_IS_USER_DEFINED_DESTAG(pucDesPtr[0]))
	{
		freeUserDefinedDes (pucDesPtr);
		return;
	}

	switch (pucDesPtr[0])
	{
	case TAG_VIDEO_STREAM:		/** < tag = 0x02 */
		svc_cas_SiDesFreeVideoStmDes((svcCas_VideoStmDes_t *)pucDesPtr);
		break;

	case TAG_AUDIO_STREAM:		/** < tag = 0x03 */
		svc_cas_SiDesFreeAudioStmDes((svcCas_AudioStmDes_t *)pucDesPtr);
		break;

	case TAG_HIERARCHY:			/** < tag = 0x04 */
		svc_cas_SiDesFreeHierDes((svcCas_HierDes_t *)pucDesPtr);
		break;

	case TAG_REGISTRATION:		/** < tag = 0x05 */
		svc_cas_SiDesFreeRegDes((svcCas_RegDes_t *)pucDesPtr);
		break;

	case TAG_DATA_STREAM_ALIGNMENT:		/** < tag = 0x06 */
		svc_cas_SiDesFreeDataStmAlignDes((svcCas_DataStmAlignDes_t *)pucDesPtr);
		break;

	case TAG_TARGET_BACKGROUND_GRID:	/** < tag = 0x07 */
		svc_cas_SiDesFreeBackGridDes((svcCas_BackGridDes_t *)pucDesPtr);
		break;

	case TAG_VIDEO_WINDOW:		/** < tag = 0x08 */
		svc_cas_SiDesFreeVideoWindowDes((svcCas_VideoWindowDes_t *)pucDesPtr);
		break;

	case TAG_CA:				/** < tag = 0x09 */
		svc_cas_SiDesFreeCaDes((svcCas_CaDes_t *)pucDesPtr);
		break;

	case TAG_ISO_639_LANGUAGE:	/** < tag = 0x0A */
		svc_cas_SiDesFreeIso639LangDes((svcCas_Iso639LangDes_t *)pucDesPtr);
		break;

	case TAG_SYSTEM_CLOCK:		/** < tag = 0x0B */
		svc_cas_SiDesFreeSysClockDes((svcCas_SysClockDes_t *)pucDesPtr);
		break;

	case TAG_MULTIPLEX_BUFFER_UTILIZATION:		/** < tag = 0x0C */
		svc_cas_SiDesFreeMultiplexBufUtilDes((svcCas_MultiplexBufUtilDes_t *)pucDesPtr);
		break;

	case TAG_COPYRIGHT:			/** < tag = 0x0D */
		svc_cas_SiDesFreeCopyrightDes((svcCas_CopyRightDes_t *)pucDesPtr);
		break;

	case TAG_MAXIMUM_BITRATE:	/** < tag = 0xE */
		svc_cas_SiDesFreeMaxBitrateDes((svcCas_MaxBitrateDes_t *)pucDesPtr);
		break;

	case TAG_PRIVATE_DATA_INDICATOR:	/** < tag = 0xF */
		svc_cas_SiDesFreePrivateDataIndicatorDes((svcCas_PrivateDataIndicatorDes_t *)pucDesPtr);
		break;

	case TAG_SMOOTHING_BUFFER:			/** < tag = 0x10 */
		svc_cas_SiDesFreeSmoothBufDes((svcCas_SmoothBufDes_t *)pucDesPtr);
		break;

	case TAG_STD:			/** < tag = 0x11 */
		svc_cas_SiDesFreeStdDes((svcCas_StdDes_t *)pucDesPtr);
		break;

	case TAG_CAROUSEL_ID:		/** < tag = 0x13 */
		svc_cas_SiDesFreeCarouselIdDes((svcCas_CarouselIdDes_t *)pucDesPtr);
		break;

	case TAG_ASSOCIATION_TAG:	/** < tag = 0x14 */
		svc_cas_SiDesFreeAssocTagDes((svcCas_AssocTagDes_t *)pucDesPtr);
		break;

	case TAG_DEFERRED_ASSOCIATION_TAG:		/** < tag = 0x15 */
		svc_cas_SiDesFreeDeferAssocTagDes((svcCas_DeferAssocTagDes_t *)pucDesPtr);
		break;

	case TAG_AVC_VIDEO:		/** < tag = 0x28 */
		svc_cas_SiDesFreeAvcVideoDes((svcCas_AvcVideoDes_t *) pucDesPtr);
		break;

	case TAG_NETWORK_NAME:		/** < tag = 0x40 */
		svc_cas_SiDesFreeNetNameDes((svcCas_NetNameDes_t *)pucDesPtr);
		break;

	case TAG_SERVICE_LIST:		/** < tag = 0x41 */
		freeServListDes((svcCas_SvcListDes_t *)pucDesPtr);
		break;

	case TAG_SATELLITE_DELIVERY_SYSTEM:		/** < tag = 0x43 */
		freeSatDeliSystemDes((svcCas_SatDeliSystemDes_t *)pucDesPtr);
		break;

	case TAG_CABLE_DELIVERY_SYSTEM:			/** < tag = 0x44 */
		freeCabDeliSystemDes((svcCas_CabDeliSystemDes_t *)pucDesPtr);
		break;

	case TAG_BOUQUET_NAME:			/** < tag = 0x47 */
		freeBouqNameDes((svcCas_BouqNameDes_t *)pucDesPtr);
		break;

	case TAG_SERVICE:		/** < tag = 0x48 */
		freeServDes((svcCas_SvcDes_t *)pucDesPtr);
		break;

	case TAG_COUNTRY_AVAILABILITY:		/** < tag = 0x49 */
		freeCountryAvailDes((svcCas_CountryAvailDes_t *)pucDesPtr);
		break;

	case TAG_LINKAGE:		/** < tag = 0x4A */
		freeLinkDes((svcCas_LinkDes_t *)pucDesPtr);
		break;

	case TAG_NVOD_REFERENCE:		/** < tag = 4B */
		freeNvodRefDes((svcCas_NvodRefDes_t *)pucDesPtr);
		break;

	case TAG_TIME_SHIFTED_SERVICE:		/** < tag = 4C */
		freeTimeShiftServDes((svcCas_TimeShiftSvcDes_t *)pucDesPtr);
		break;

	case TAG_SHORT_EVENT:		/** < tag = 4D */
		freeShortEvtDes((svcCas_ShortEventDes_t *)pucDesPtr);
		break;

	case TAG_EXTENDED_EVENT:		/** < tag = 4E */
		freeExtEvtDes((svcCas_ExtEvtDes_t *)pucDesPtr);
		break;

	case TAG_TIME_SHIFTED_EVENT:		/** < tag = 4F */
		freeTimeShiftEvtDes((svcCas_TimeShiftEvtDes_t *)pucDesPtr);
		break;

	case TAG_COMPONENT:		/** < tag = 0x50 */
		freeCompDes((svcCas_CompDes_t *)pucDesPtr);
		break;

	case TAG_STREAM_IDENTIFIER:		/** < tag = 0x52 */
		freeStmIdentDes((svcCas_StmIdentDes_t *)pucDesPtr);
		break;

	case TAG_CA_IDENTIFIER:		/** < tag = 0x53 */
		freeCaIdentDes((svcCas_CaIdentDes_t *)pucDesPtr);
		break;

	case TAG_CONTENT:		/** < tag = 0x54 */
		freeContentDes((svcCas_ContentDes_t *)pucDesPtr);
		break;

	case TAG_PARENTAL_RATING:		/** < tag = 0x55 */
		freeParentDes((svcCas_ParentDes_t *)pucDesPtr);
		break;

	case TAG_TELETEXT:		/** < tag = 0x56 */
		freeTeletextDes((svcCas_TeletextDes_t *)pucDesPtr);
		break;

	case TAG_TELEPHONE:		/** < tag = 0x57 */
		freeTelephoneDes((svcCas_TelephoneDes_t *)pucDesPtr);
		break;

	case TAG_LOCAL_TIME_OFFSET:		/** < tag = 0x58 */
		freeLocalTimeDes((svcCas_LocalTimeDes_t *)pucDesPtr);
		break;

	case TAG_SUBTITLING:		/** < tag = 0x59 */
		freeSubTitleDes((svcCas_SubTitleDes_t *)pucDesPtr);
		break;

	case TAG_TERRESTRIAL_DELIVERY_SYSTEM:		/** < tag = 0x5A */
		freeTerDeliSystemDes((svcCas_TerDeliSystemDes_t *)pucDesPtr);
		break;

	case TAG_MULTILINGUAL_NETWORK_NAME:			/** < tag = 0x5B */
		freeMultiNetNameDes((svcCas_MultiNetNameDes_t *)pucDesPtr);
		break;

	case TAG_MULTILINGUAL_BOUQUET_NAME:			/** < tag = 0x5C */
		freeMultiBouqNameDes((svcCas_MultiBouqNameDes_t *)pucDesPtr);
		break;

	case TAG_MULTILINGUAL_SERVICE_NAME:			/** < tag = 0x5D */
		freeMultiServNameDes((svcCas_MultiSvcNameDes_t *)pucDesPtr);
		break;

	case TAG_MULTILINGUAL_COMPONENT:			/** < tag = 0x5E */
		freeMultiCompDes((svcCas_MultiCompDes_t *)pucDesPtr);
		break;

	case TAG_PRIVATE_DATA_SPECIFIER:			/** < tag = 0x5F */
		freePrivateDataSpecDes((svcCas_PrivateDataSpecDes_t *)pucDesPtr);
		break;

	case TAG_SERVICE_MOVE:			/** < tag = 0x60 */
		freeServMoveDes((svcCas_SvcMoveDes_t *)pucDesPtr);
		break;

	case TAG_SHORT_SMOOTHING_BUFFER:		/** < tag = 0x61 */
		freeShortSmoothBufDes((svcCas_ShortSmoothBufDes_t *)pucDesPtr);
		break;

	case TAG_FREQUENCY_LIST:		/** < tag = 0x62 */
		freeFrequencyListDes((svcCas_FrequencyListDes_t *)pucDesPtr);
		break;

	case TAG_PARTIAL_TRANSPORT_STREAM:
		freePartialTransportStreamDes((svcCas_PartialTsDes *)pucDesPtr);
		break;

	case TAG_DATA_BROADCAST:		/** < tag = 0x64 */
		freeDataBroadcastDes((svcCas_DataBroadcastDes_t *)pucDesPtr);
		break;

	case TAG_DATA_BROADCAST_ID:		/** < tag = 0x66 */
		freeDataBroadcastIdDes((svcCas_DataBroadcastIdDes_t *)pucDesPtr);
		break;

	case TAG_PDC:			/** < tag = 0x69 */
		freePDCDes ((svcCas_PDCDes_t *)pucDesPtr);
		break;

	case TAG_DOLBY_AC3: 		/** < tag = 0x6A */
		freeDolbyAC3Des((svcCas_DolbyAc3Des_t *)pucDesPtr);
		break;

	case TAG_APPLICATION_SIGNALLING:		/** < tag = 0x6F */
		freeAppSignallingDes ((svcCas_AppSignallingDes_t *)pucDesPtr);
		break;

	case TAG_DEFAULT_AUTORITY:	/** < tag = 0x73 */
		freeDefaultAuthorityDes((svcCas_DefaultAuthorityDes_t *)pucDesPtr);
		break;

	case TAG_RELATED_CONTENT:	/** < tag = 0x74 */
		freeRelatedContentDes((svcCas_RelatedContentDes_t *)pucDesPtr);
		break;

	case TAG_CONTENT_IDENTIFIER:	/** < tag = 0x76 */
		freeContentIdentifierDes((svcCas_ContentIdentifierDes_t *)pucDesPtr);
		break;

	case TAG_S2_SATELLITE_DELIVERY_SYSTEM:		/** < tag = 0x79 */
		freeS2SatDeliSystemDes((svcCas_S2SatDeliSystemDes_t*)pucDesPtr);
		break;

	case TAG_ENHANCED_AC3:			/** < tag = 0x7A */
		freeEnhancedAC3Des((svcCas_EnhancedAC3Des_t *)pucDesPtr);
		break;

	case TAG_AAC:			/** < tag = 0x7C */
		freeAACDes((svcCas_AACDes_t *)pucDesPtr);
		break;

	case TAG_CELL_LIST:
		freeCellListDes((svcCas_CellListDes_t *)pucDesPtr);
		break;

	case TAG_FTA_CONTENT_MANAGEMENT:
		freeFtaContentManageDes((svcCas_FtaContentManagementDes_t *)pucDesPtr);
		break;

	case TAG_EXTENSION:
		freeExtensionDes((svcCas_ExtensionDes_t*)pucDesPtr);
		break;

	default:
		break;
	}
}

/************************************************************************/
/*	function : AllocDesInfo												*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/
HUINT8 	*svc_cas_SiAllocDesInfo(HINT32 lDesTag, HUINT16 usSubTag)
{
	HUINT8	*pstNewDesInfo = NULL;

	switch (lDesTag)
	{
	case TAG_PRIVATE_DATA:
		pstNewDesInfo = (HUINT8 *)svc_cas_SiDesAllocPrivateDataInfo();
		break;

	case TAG_CAROUSEL_ID:
		pstNewDesInfo = (HUINT8 *)svc_cas_SiDesAllocCarouselIdDesInfo();
		break;

	case TAG_ISO_639_LANGUAGE:
		pstNewDesInfo = (HUINT8 *)svc_cas_SiDesAllocLangInfo();
		break;

	case TAG_SERVICE_LIST:
		pstNewDesInfo = (HUINT8 *)allocServListInfo();
		break;

	case TAG_COUNTRY_AVAILABILITY:
		pstNewDesInfo = (HUINT8 *)allocCountryAvailInfo();
		break;

	case TAG_NVOD_REFERENCE:
		pstNewDesInfo = (HUINT8 *)allocNvodRefInfo();
		break;

	case TAG_EXTENDED_EVENT:
		pstNewDesInfo = (HUINT8 *)allocExtEvtInfo();
		break;

	case TAG_CA_IDENTIFIER:
		pstNewDesInfo = (HUINT8 *)allocCaIdentInfo();
		break;

	case TAG_CONTENT:
		pstNewDesInfo = (HUINT8 *)allocContentInfo();
		break;

	case TAG_PARENTAL_RATING:
		pstNewDesInfo = (HUINT8 *)allocParentInfo();
		break;

	case TAG_TELETEXT:
		pstNewDesInfo = (HUINT8 *)allocTeletextInfo();
		break;

	case TAG_LOCAL_TIME_OFFSET:
		pstNewDesInfo = (HUINT8 *)allocLocalTimeInfo();
		break;

	case TAG_SUBTITLING:
		pstNewDesInfo = (HUINT8 *)allocSubTitleInfo();
		break;

	case TAG_MULTILINGUAL_NETWORK_NAME:
		pstNewDesInfo = (HUINT8 *)allocMultiNetNameInfo();
		break;

	case TAG_MULTILINGUAL_BOUQUET_NAME:
		pstNewDesInfo = (HUINT8 *)allocMultiBouqNameInfo();
		break;

	case TAG_MULTILINGUAL_SERVICE_NAME:
		pstNewDesInfo = (HUINT8 *)allocMultiServNameInfo();
		break;

	case TAG_MULTILINGUAL_COMPONENT:
		pstNewDesInfo = (HUINT8 *)allocMultiCompInfo();
		break;

	case TAG_APPLICATION_SIGNALLING:
		pstNewDesInfo = (HUINT8 *)allocAppSignallingInfo ();
		break;

	case TAG_CONTENT_IDENTIFIER:
		pstNewDesInfo = (HUINT8 *)allocCridInfo();
		break;

	case TAG_DATA_BROADCAST:
		if (usSubTag == 0x0006)
		{
			pstNewDesInfo = (HUINT8 *)allocDataBroadcastInfo();
		}
		else if (usSubTag == 0x0007)
		{
			pstNewDesInfo = (HUINT8 *)allocObjectCarouselInfo();
		}
		break;

 	case TAG_CELL_LIST:
		pstNewDesInfo = (HUINT8 *)allocCellListInfo ();
		break;

	default :
		HxLOG_Print("[ERROR] alloc fail Tag = 0x%x\n",lDesTag);
		break;
	}

	return ((HUINT8 *)pstNewDesInfo);
}

/************************************************************************/
/*	function : FreeDesInfo												*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/
void 	svc_cas_SiFreeDesInfo(HINT32 lDesTag, HUINT16 usSubTag, HUINT8 *pDesInfoPtr)
{
	if (pDesInfoPtr == NULL)
	{
		return;
	}

	switch (lDesTag)
	{
	case TAG_PRIVATE_DATA:
		svc_cas_SiDesFreePrivateDataInfo((svcCas_PrivateDataInfo_t *)pDesInfoPtr);
		break;

	case TAG_ISO_639_LANGUAGE:
		svc_cas_SiDesFreeLangInfo((svcCas_LangInfo_t *)pDesInfoPtr);
		break;

	case TAG_SERVICE_LIST:
		freeServListInfo((svcCas_SvcListInfo_t *)pDesInfoPtr);
		break;

	case TAG_COUNTRY_AVAILABILITY:
		freeCountryAvailInfo((svcCas_CountryAvailInfo_t *)pDesInfoPtr);
		break;

	case TAG_NVOD_REFERENCE:
		freeNvodRefInfo((svcCas_NvodRefInfo_t *)pDesInfoPtr);
		break;

	case TAG_EXTENDED_EVENT:
		freeExtEvtInfo((svcCas_ExtEvtInfo_t *)pDesInfoPtr);
		break;

	case TAG_CA_IDENTIFIER:
		freeCaIdentInfo((svcCas_CaIdentInfo_t *)pDesInfoPtr);
		break;

	case TAG_CONTENT:
		freeContentInfo((svcCas_ContentInfo_t *)pDesInfoPtr);
		break;

	case TAG_PARENTAL_RATING:
		freeParentInfo((svcCas_ParentInfo_t *)pDesInfoPtr);
		break;

	case TAG_TELETEXT:
		freeTeletextInfo((svcCas_TeletextInfo_t *)pDesInfoPtr);
		break;

	case TAG_LOCAL_TIME_OFFSET:
		freeLocalTimeInfo((svcCas_LocalTimeInfo_t *)pDesInfoPtr);
		break;

	case TAG_SUBTITLING:
		freeSubTitleInfo((svcCas_SubTitleInfo_t *)pDesInfoPtr);
		break;

	case TAG_MULTILINGUAL_NETWORK_NAME:
		freeMultiNetNameInfo((svcCas_MultiNetNameInfo_t *)pDesInfoPtr);
		break;

	case TAG_MULTILINGUAL_BOUQUET_NAME:
		freeMultiBouqNameInfo((svcCas_MultiBouqNameInfo_t *)pDesInfoPtr);
		break;

	case TAG_MULTILINGUAL_SERVICE_NAME:
		freeMultiServNameInfo((svcCas_MultiSvcNameInfo_t *)pDesInfoPtr);
		break;

	case TAG_MULTILINGUAL_COMPONENT:
		freeMultiCompInfo((svcCas_MultiCompInfo_t *)pDesInfoPtr);
		break;

	case TAG_APPLICATION_SIGNALLING:
		freeAppSignallingInfo ((svcCas_AppSignallingInfo_t *)pDesInfoPtr);
		break;

	case TAG_CONTENT_IDENTIFIER:
		freeCridInfo((svcCas_CridInfo_t*)pDesInfoPtr);
		break;

	case TAG_DATA_BROADCAST:
		if (usSubTag == 0x0006)
		{
			freeDataBroadcastInfo((svcCas_DataCarouselInfo_t *)pDesInfoPtr);
		}
		else if (usSubTag == 0x0007)
		{
			freeObjectCarouselInfo((svcCas_ObjectCarouselInfo_t *)pDesInfoPtr);
		}
		break;

	case TAG_CELL_LIST:
		freeCellListInfo((svcCas_CellListInfo_t *)pDesInfoPtr);
		break;

	default :
		HxLOG_Print("[ERROR] Free fail Tag = 0x%x\n", lDesTag);
		break;
	}
}

#define ____FOR_EXTENSION_DESCRIPTOR____
/************************************************************************
 *
 * Extension Descriptor 관련....
 *
 ************************************************************************/

#define ____0X00_IMAGE_ICON_DES____

/************************************************************************
 *
 * TAG_EXT_T2_DELIVERY_SYSTEM Descriptor 관련... (TAG = 0x04)
 *
 ************************************************************************/
/************************************************************************/
/*	function : initImgIconDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/
INLINE STATIC 	void 	initImgIconDes(svcCas_ImageIconExtDes_t *pstImgIconDes)
{
	pstImgIconDes->ucDesExtTag = 0x00;
	pstImgIconDes->ucDesNum = 0xff;
	pstImgIconDes->ucLastDesNum = 0xff;
	pstImgIconDes->ucIconId = 0xff;
	pstImgIconDes->pstImgHeadInfo = NULL;
	pstImgIconDes->pstImgIconInfo = NULL;
}

/************************************************************************/
/*	function : allocExtImgIconDes											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_ImageIconExtDes_t *allocExtImgIconDes(void)
{
	svcCas_ImageIconExtDes_t	*pstImgIconDes;

	pstImgIconDes = (svcCas_ImageIconExtDes_t *)OxCAS_Malloc(sizeof(svcCas_ImageIconExtDes_t));
	if (NULL == pstImgIconDes)
	{
		HxLOG_Print("[ERROR] %s()\n",__FUNCTION__);
		return NULL;
	}
	initImgIconDes(pstImgIconDes);

	return ((svcCas_ImageIconExtDes_t *)pstImgIconDes);
}


/************************************************************************/
/*	function : freeExtImgIconDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/
STATIC 	void 	freeExtImgIconDes(svcCas_ImageIconExtDes_t *pstImgIconDes)
{
	ImageIconHeadInfo	*pstImgHeadInfo;
	ImageIconDataInfo	*pstImgIconInfo;
	ImageIconTypeInfo	*pstImgTypeInfo;
	ImageIconURLInfo	*pstImgUrlInfo;

	if( NULL != pstImgIconDes )
	{
		if (NULL != pstImgIconDes->pstImgHeadInfo)
		{
			pstImgHeadInfo = pstImgIconDes->pstImgHeadInfo;

			if (pstImgHeadInfo->pstImgTypeInfo)
			{
				pstImgTypeInfo = pstImgHeadInfo->pstImgTypeInfo;
				if ( NULL != pstImgTypeInfo->pucImgType)
				{
					OxCAS_Free(pstImgTypeInfo->pucImgType);
				}
				OxCAS_Free(pstImgTypeInfo);
			}
			if (pstImgHeadInfo->pstImgUrlInfo)
			{
				pstImgUrlInfo = pstImgHeadInfo->pstImgUrlInfo;
				if ( NULL != pstImgUrlInfo->pucImgUrl)
				{
					OxCAS_Free(pstImgUrlInfo->pucImgUrl);
				}
				OxCAS_Free(pstImgUrlInfo);
			}
			if (pstImgHeadInfo->pstImgIconInfo)
			{
				pstImgIconInfo = pstImgHeadInfo->pstImgIconInfo;
				if ( NULL != pstImgIconInfo->pucImgData)
				{
					OxCAS_Free(pstImgIconInfo->pucImgData);
				}
				OxCAS_Free(pstImgIconInfo);
			}
			OxCAS_Free(pstImgHeadInfo);
		}
		if (NULL != pstImgIconDes->pstImgIconInfo)
		{
			pstImgIconInfo = pstImgIconDes->pstImgIconInfo;
			if ( NULL != pstImgIconInfo->pucImgData )
			{
				OxCAS_Free(pstImgIconInfo->pucImgData );
			}
			OxCAS_Free(pstImgIconInfo);
		}
		OxCAS_Free(pstImgIconDes);
	}
}

#define ____0X04_T2_DELIVERY_SYSTEM_DES____

/************************************************************************
 *
 * TAG_EXT_IMG_ICON Descriptor 관련... (TAG = 0x00)
 *
 ************************************************************************/
/************************************************************************/
/*	function : initT2DeliverySystemDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

INLINE STATIC 	void 	initT2DeliverySystemDes(svcCas_T2DeliverySystemExtDes_t *pstT2DelSysDes)
{
	pstT2DelSysDes->ucDesExtTag = 0x04;
	pstT2DelSysDes->ucPlpId = 0xff;
	pstT2DelSysDes->usT2SystemId = 0xffff;
	pstT2DelSysDes->pstDetailInfo = NULL;
}

/************************************************************************/
/*	function : allocExtT2DeliverySystemDes											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_T2DeliverySystemExtDes_t *allocExtT2DeliverySystemDes(void)
{
	svcCas_T2DeliverySystemExtDes_t	*pstT2DelSysDes;

	pstT2DelSysDes = (svcCas_T2DeliverySystemExtDes_t *)OxCAS_Malloc(sizeof(svcCas_T2DeliverySystemExtDes_t));
	if (NULL == pstT2DelSysDes)
	{
		HxLOG_Print("[ERROR] allocExtT2DeliverySystemDes()\n");
		return NULL;
	}
	initT2DeliverySystemDes(pstT2DelSysDes);

	return ((svcCas_T2DeliverySystemExtDes_t *)pstT2DelSysDes);
}


/************************************************************************/
/*	function : freeExtT2DeliverySystemDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeExtT2DeliverySystemDes(svcCas_T2DeliverySystemExtDes_t *pstT2DelSysDes)
{
	svcCas_T2DeliveryDetailInfo_t	*pstT2DetailInfo;
	svcCas_T2DeliveryCellInfo_t		*pstCellInfo,*pstTmpCellInfo;
	if(pstT2DelSysDes)
	{
		if (NULL != pstT2DelSysDes->pstDetailInfo)
		{
			pstT2DetailInfo = pstT2DelSysDes->pstDetailInfo;

			if (NULL != pstT2DetailInfo)
			{
				pstCellInfo = pstT2DetailInfo->pstCellInfo;
				while(pstCellInfo)
				{
					pstTmpCellInfo = pstCellInfo;
					pstCellInfo = pstCellInfo->pNextInfo;
					if (pstTmpCellInfo->pstSubCellInfo)
					{
						OxCAS_Free(pstTmpCellInfo->pstSubCellInfo);
					}
					OxCAS_Free(pstTmpCellInfo);
				}
			}
			OxCAS_Free(pstT2DetailInfo);
		}
		OxCAS_Free(pstT2DelSysDes);
	}
}

/************************************************************************/
/*	function : initExtT2DeliverySystemInfo									*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/
STATIC 	void 	initExtT2DeliverySystemInfo(svcCas_T2DeliveryCellInfo_t *pT2DelSysCellInfo)
{
	HxSTD_MemSet((void *)pT2DelSysCellInfo, 0, sizeof(svcCas_T2DeliveryCellInfo_t));
	pT2DelSysCellInfo->usCellId = 0xffff;
}

/************************************************************************/
/*	function : freeExtT2DeliverySystemInfo									*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/
STATIC 	void 	freeExtT2DeliverySystemInfo(svcCas_T2DeliveryCellInfo_t *pT2DelSysCellInfo)
{
	if (pT2DelSysCellInfo)
	{
		if(pT2DelSysCellInfo->pulCentreFreq != NULL)
			OxCAS_Free(pT2DelSysCellInfo->pulCentreFreq);		
		if(pT2DelSysCellInfo->pstSubCellInfo != NULL)
			OxCAS_Free(pT2DelSysCellInfo->pstSubCellInfo);
		OxCAS_Free(pT2DelSysCellInfo);
	}
}

/************************************************************************/
/*	function : allocExtT2DeliverySystemInfo									*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_T2DeliveryCellInfo_t 	*allocExtT2DeliverySystemInfo(void)
{
	svcCas_T2DeliveryCellInfo_t	*pT2DelSysCellInfo;

	pT2DelSysCellInfo = (svcCas_T2DeliveryCellInfo_t *)OxCAS_Malloc(sizeof(svcCas_T2DeliveryCellInfo_t));
	if (NULL == pT2DelSysCellInfo)
	{
		HxLOG_Print("[ERROR] allocExtT2DeliverySystemInfo()\n");
		return NULL;
	}
	initExtT2DeliverySystemInfo(pT2DelSysCellInfo);

	return ((svcCas_T2DeliveryCellInfo_t *)pT2DelSysCellInfo);
}

#define ____0X07_NETWORK_CHANGE_NOTIFY_DES____

/************************************************************************
 *
 * TAG_EXT_NETWORK_CHANGE_NOTIFY Descriptor 관련... (TAG = 0x07)
 *
 ************************************************************************/

/************************************************************************/
/*	function :  freeExtNetChangeNotifySubInfo									*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeExtNetChangeNotifySubInfo(svcCas_NetChangeNotifySubInfo_t *pstNetChNotifySubInfo)
{
	svcCas_NetChangeNotifySubInfo_t 	*pstTmpNode;

	while(pstNetChNotifySubInfo)
	{
		pstTmpNode = pstNetChNotifySubInfo;
		pstNetChNotifySubInfo = pstNetChNotifySubInfo->pNextInfo;
		OxCAS_Free(pstTmpNode);
	}
}

/************************************************************************/
/*	function : freeExtNetChangeNotifyCellInfo									*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeExtNetChangeNotifyCellInfo(svcCas_NetChangeNotifyCellInfo_t *pstNetChNotifyCellInfo)
{
	svcCas_NetChangeNotifyCellInfo_t 	*pstTmpNode;

	while(pstNetChNotifyCellInfo)
	{
		pstTmpNode = pstNetChNotifyCellInfo;
		pstNetChNotifyCellInfo = pstNetChNotifyCellInfo->pNextInfo;
		freeExtNetChangeNotifySubInfo(pstTmpNode->pstNetChNotifySubInfo);
		OxCAS_Free(pstTmpNode);
	}
}

/************************************************************************/
/*	function : freeExtNetworkChangeNotifyDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeExtNetworkChangeNotifyDes(svcCas_NetChangeNotifyExtDes_ *pstNetChNotifyDes)
{
	if(pstNetChNotifyDes)
	{
		freeExtNetChangeNotifyCellInfo(pstNetChNotifyDes->pstNetChNotifyCellInfo);
		OxCAS_Free(pstNetChNotifyDes);
	}
}

/************************************************************************/
/*	function : allocExtNetChangeNotifySubInfo											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/
STATIC 	svcCas_NetChangeNotifySubInfo_t 	*allocExtNetChangeNotifySubInfo(void)
{
	svcCas_NetChangeNotifySubInfo_t	*pstNetChNotifySubInfo;

	pstNetChNotifySubInfo = (svcCas_NetChangeNotifySubInfo_t *)OxCAS_Malloc(sizeof(svcCas_NetChangeNotifySubInfo_t));
	if (NULL == pstNetChNotifySubInfo)
	{
		HxLOG_Print("[ERROR] allocExtNetChangeNotifySubInfo()\n");
		return NULL;
	}
	HxSTD_MemSet(pstNetChNotifySubInfo,0,sizeof(svcCas_NetChangeNotifySubInfo_t));
	pstNetChNotifySubInfo->ucNetChangeId = 0xff;
	pstNetChNotifySubInfo->ucNetChangeVer = 0xff;
	pstNetChNotifySubInfo->ucReceiverCategory = 0xff;
	pstNetChNotifySubInfo->ucChangeType = 0xff;

	return ((svcCas_NetChangeNotifySubInfo_t *)pstNetChNotifySubInfo);
}

/************************************************************************/
/*	function : allocExtNetChangeNotifyCellInfo											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/
STATIC 	svcCas_NetChangeNotifyCellInfo_t 	*allocExtNetChangeNotifyCellInfo(void)
{
	svcCas_NetChangeNotifyCellInfo_t	*pstNetChNotifyCellInfo;

	pstNetChNotifyCellInfo = (svcCas_NetChangeNotifyCellInfo_t *)OxCAS_Malloc(sizeof(svcCas_NetChangeNotifyCellInfo_t));
	if (NULL == pstNetChNotifyCellInfo)
	{
		HxLOG_Print("[ERROR] allocExtNetworkChangeNotifyDes()\n");
		return NULL;
	}
	pstNetChNotifyCellInfo->usCellId = 0xffff;
	pstNetChNotifyCellInfo->pstNetChNotifySubInfo = NULL;

	return ((svcCas_NetChangeNotifyCellInfo_t *)pstNetChNotifyCellInfo);
}

/************************************************************************/
/*	function : allocExtNetworkChangeNotifyDes									*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/
STATIC 	svcCas_NetChangeNotifyExtDes_ 	*allocExtNetworkChangeNotifyDes(void)
{
	svcCas_NetChangeNotifyExtDes_	*pstNetChNotifyDes;

	pstNetChNotifyDes = (svcCas_NetChangeNotifyExtDes_ *)OxCAS_Malloc(sizeof(svcCas_NetChangeNotifyExtDes_));
	if (NULL == pstNetChNotifyDes)
	{
		HxLOG_Print("[ERROR] allocExtNetworkChangeNotifyDes()\n");
		return NULL;
	}
	pstNetChNotifyDes->ucDesExtTag = TAG_EXT_NETWORK_CHANGE_NOTIFY;
	pstNetChNotifyDes->pstNetChNotifyCellInfo = NULL;

	return ((svcCas_NetChangeNotifyExtDes_ *)pstNetChNotifyDes);
}

#define ____0X08_MESSAGE_DES____

/************************************************************************
 *
 * TAG_EXT_MESSAGE Descriptor 관련... (TAG = 0x08)
 *
 ************************************************************************/
/************************************************************************/
/*	function : initMessageDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	initExtMessageDes(svcCas_MessageExtDes_t *pstExtMessageDes)
{
	pstExtMessageDes->ucMessageId = 0xff;
	HxSTD_MemSet(pstExtMessageDes->szIso639LangCode,0,3);
	pstExtMessageDes->pszMessageText = NULL;
}

/************************************************************************/
/*	function : allocExtMessageDes											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC 	svcCas_MessageExtDes_t 	*allocExtMessageDes(void)
{
	svcCas_MessageExtDes_t	*pstExtMessageDes;

	pstExtMessageDes = (svcCas_MessageExtDes_t *)OxCAS_Malloc(sizeof(svcCas_MessageExtDes_t));
	if (NULL == pstExtMessageDes)
	{
		HxLOG_Print("[ERROR] allocExtMessageDes()\n");
		return NULL;
	}
	initExtMessageDes(pstExtMessageDes);

	return ((svcCas_MessageExtDes_t *)pstExtMessageDes);
}


/************************************************************************/
/*	function : freeExtMessageDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeExtMessageDes(svcCas_MessageExtDes_t *pstExtMessageDes)
{
	if(pstExtMessageDes)
	{
		if (pstExtMessageDes->pszMessageText != NULL)
			OxCAS_Free(pstExtMessageDes->pszMessageText);
		OxCAS_Free(pstExtMessageDes);
	}
}

/************************************************************************/
/*	function : svc_cas_SiSetExtMessageDes											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/
void 	svc_cas_SiSetExtMessageDes(svcCas_MessageExtDes_t *pstMsgExtDes, HUINT8 *pszIso639LangCode, HUINT8 *pszText)
{
	HxSTD_MemCopy(pstMsgExtDes->szIso639LangCode, pszIso639LangCode
		, svc_cas_SiUtilStrlen(pszIso639LangCode));
	pstMsgExtDes->szIso639LangCode[svc_cas_SiUtilStrlen(pszIso639LangCode)] = '\0';
	if(pszText != NULL)
	{
		HxSTD_MemCopy(pstMsgExtDes->pszMessageText, pszText, svc_cas_SiUtilStrlen(pszText));
		pstMsgExtDes->pszMessageText[svc_cas_SiUtilStrlen(pszText)] = '\0';
	}
	else
	{
		pstMsgExtDes->pszMessageText[0] = '\0';
	}
}

#define ____0X06_SUPPLEMENTARY_AUDIO_DES____

/************************************************************************
 *
 * TAG_EXT_SUPPLEMENTARY_AUDIO Descriptor 관련... (TAG = 0x06)
 *
 ************************************************************************/
/************************************************************************/
/*	function : initExtSupplementaryAudioDes								*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/
INLINE STATIC void initExtSupplementaryAudioDes(svcCas_SupplementaryAudioExtDes *pstDes)
{
	if (pstDes != NULL)
	{
		HxSTD_MemSet(pstDes, 0, sizeof(svcCas_SupplementaryAudioExtDes));
		pstDes->ucDesExtTag = TAG_EXT_SUPPLEMENTARY_AUDIO;
	}
}

/************************************************************************/
/*	function : allocExtSupplementaryAudioDes							*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/
STATIC 	svcCas_SupplementaryAudioExtDes *allocExtSupplementaryAudioDes(void)
{
	svcCas_SupplementaryAudioExtDes	*pstDes;

	pstDes = (svcCas_SupplementaryAudioExtDes *)OxCAS_Malloc(sizeof(svcCas_SupplementaryAudioExtDes));
	if (NULL == pstDes)
	{
		HxLOG_Print("[ERROR] allocExtSupplementaryAudioDes()\n");
		return NULL;
	}
	initExtSupplementaryAudioDes(pstDes);

	return ((svcCas_SupplementaryAudioExtDes *) pstDes);
}

/************************************************************************/
/*	function : svcCas_SupplementaryAudioExtDes									*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/
STATIC void freeExtSupplementaryAudioDes(svcCas_SupplementaryAudioExtDes *pstDes)
{
	if (pstDes != NULL)
	{
		if (pstDes->pPrivateDataByte != NULL)
		{
			OxCAS_Free(pstDes->pPrivateDataByte);
		}
		OxCAS_Free(pstDes);
	}
}

#define ____0X09_TARGET_REGION_DES____

/************************************************************************
 *
 * TAG_EXT_TARGET_REG Descriptor 관련... (TAG = 0x09)
 *
 ************************************************************************/

/************************************************************************/
/*	function :  freeExtTargetRegionDesInfo									*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeExtTargetRegionDesInfo(svcCas_TargetRegionExtDesInfo *pstTargetRegionDesInfo)
{
	svcCas_TargetRegionExtDesInfo 	*pstTmpNode;

	while(pstTargetRegionDesInfo)
	{
		pstTmpNode = pstTargetRegionDesInfo;
		pstTargetRegionDesInfo = pstTargetRegionDesInfo->pNextInfo;
		if(pstTmpNode != NULL)
			OxCAS_Free(pstTmpNode);
	}
}

/************************************************************************/
/*	function : freeExtTargetRegionDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeExtTargetRegionDes(svcCas_TargetRegionExtDes *pTargetRegionDes)
{
	if(pTargetRegionDes != NULL)
	{
		freeExtTargetRegionDesInfo(pTargetRegionDes->pstTargetRegionDesInfo);
		OxCAS_Free(pTargetRegionDes);
	}
}

/************************************************************************/
/*	function : allocExtTargetRegionDesInfo											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/
STATIC 	svcCas_TargetRegionExtDesInfo 	*allocExtTargetRegionDesInfo(void)
{
	svcCas_TargetRegionExtDesInfo		*pstTargetRegionDesInfo;

	pstTargetRegionDesInfo = (svcCas_TargetRegionExtDesInfo *)OxCAS_Malloc(sizeof(svcCas_TargetRegionExtDesInfo));
	if (NULL == pstTargetRegionDesInfo)
	{
		HxLOG_Print("[ERROR] allocExtTargetRegionDesInfo()\n");
		return NULL;
	}
	HxSTD_MemSet(pstTargetRegionDesInfo,0,sizeof(svcCas_TargetRegionExtDesInfo));

	return ((svcCas_TargetRegionExtDesInfo *)pstTargetRegionDesInfo);
}

/************************************************************************/
/*	function : allocExtTargetRegionDes									*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/
STATIC 	svcCas_TargetRegionExtDes 	*allocExtTargetRegionDes(void)
{
	svcCas_TargetRegionExtDes	*pstTargetRegionDes;

	pstTargetRegionDes = (svcCas_TargetRegionExtDes *)OxCAS_Malloc(sizeof(svcCas_TargetRegionExtDes));
	if (NULL == pstTargetRegionDes)
	{
		HxLOG_Print("[ERROR] allocExtTargetRegionDes()\n");
		return NULL;
	}
	pstTargetRegionDes->ucDesExtTag = TAG_EXT_TARGET_REG;
	pstTargetRegionDes->pstTargetRegionDesInfo = NULL;

	return ((svcCas_TargetRegionExtDes *)pstTargetRegionDes);
}

#define ____0X0A_TARGET_REGION_NAME_DES____

/************************************************************************
 *
 * TAG_EXT_TARGET_REG_NAME Descriptor 관련... (TAG = 0x0A)
 *
 ************************************************************************/

/************************************************************************/
/*	function :  freeExtTargetRegionNameDesInfo									*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeExtTargetRegionNameDesInfo(TargetRegionNameExtDesInfo *pstTargetRegionNameDesInfo)
{
	TargetRegionNameExtDesInfo 	*pstTmpNode;

	while(pstTargetRegionNameDesInfo)
	{
		pstTmpNode = pstTargetRegionNameDesInfo;
		pstTargetRegionNameDesInfo = pstTargetRegionNameDesInfo->pNextInfo;
		OxCAS_Free(pstTmpNode);
	}
}

/************************************************************************/
/*	function : freeExtTargetRegionNameDes											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	freeExtTargetRegionNameDes(svcCas_TargetRegionNameExtDes *pstTargetRegionNameDes)
{
	if(pstTargetRegionNameDes != NULL)
	{
		freeExtTargetRegionNameDesInfo(pstTargetRegionNameDes->pstTargetRegionNameDesInfo);
		OxCAS_Free(pstTargetRegionNameDes);
	}
}

/************************************************************************/
/*	function : allocExtTargetRegionNameDesInfo											*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/
STATIC 	TargetRegionNameExtDesInfo 	*allocExtTargetRegionNameDesInfo(void)
{
	TargetRegionNameExtDesInfo		*pstTargetRegionNameDesInfo;

	pstTargetRegionNameDesInfo = (TargetRegionNameExtDesInfo *)OxCAS_Malloc(sizeof(TargetRegionNameExtDesInfo));
	if (NULL == pstTargetRegionNameDesInfo)
	{
		HxLOG_Print("[ERROR] allocExtTargetRegionNameDesInfo()\n");
		return NULL;
	}
	HxSTD_MemSet(pstTargetRegionNameDesInfo,0,sizeof(TargetRegionNameExtDesInfo));

	return ((TargetRegionNameExtDesInfo *)pstTargetRegionNameDesInfo);
}

/************************************************************************/
/*	function : allocExtTargetRegionNameDes									*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/
STATIC 	svcCas_TargetRegionNameExtDes 	*allocExtTargetRegionNameDes(void)
{
	svcCas_TargetRegionNameExtDes	*pstTargetRegionNameDes;

	pstTargetRegionNameDes = (svcCas_TargetRegionNameExtDes *)OxCAS_Malloc(sizeof(svcCas_TargetRegionNameExtDes));
	if (NULL == pstTargetRegionNameDes)
	{
		HxLOG_Print("[ERROR] allocExtTargetRegionNameDes()\n");
		return NULL;
	}
	pstTargetRegionNameDes->ucDesExtTag = TAG_EXT_TARGET_REG_NAME;
	pstTargetRegionNameDes->pstTargetRegionNameDesInfo = NULL;

	return ((svcCas_TargetRegionNameExtDes *)pstTargetRegionNameDes);
}

/*********************** End of File ******************************/
