/**************************************************************************************/
/**
 * @file oplparser.c
 *
 * Defines DVB programme routine
 *
 * @author
 * @date
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <apk.h>
#include <oplparser.h>

#define	return_if_void(expr)	do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return;}}while(0)
#define	return_if(expr, err)	do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return (err);}}while(0)
#define	goto_if(expr, pos)		do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);goto pos;}}while(0)
#define PRIVATE_DATA_SPECIFIER_FOR_UK_DTT		0x0000233A /* UK_DTT */

typedef HUINT8* (*descAlloc_t)(void);
typedef void  	(*descFree_t)(void *pucDesc);
typedef HERROR (*descParse_t)(HUINT8 *pucRaw, HUINT8 *pucDes);
typedef void (*descPrint_t)(HUINT8 *pucDes);

typedef struct tagOPLParsingTable
{
	HINT32		tag;
	descAlloc_t	alloc;
	descFree_t 	free;
	descParse_t	parse;
	descPrint_t	print;
}OPLParsingTable;
static OPLParsingTable s_parsingTable[] = {
	{
		eSIxDESCTAG_UKDTT_GUIDANCE,
		OPL_PARSER_UKDTT_AllocGuidanceDes,
		OPL_PARSER_UKDTT_FreeGuidanceDes,
		OPL_PARSER_UKDTT_ParseGuidanceDes,
		OPL_PARSER_UKDTT_PrintGuidanceDes
	},
	{
		eSIxDESCTAG_CONTENT,
		OPL_PARSER_AllocContentDes,
		OPL_PARSER_FreeContentDes,
		OPL_PARSER_ParseContentDes,
		OPL_PARSER_PrintContentDes
	},
	{
		eSIxDESCTAG_LINKAGE,
		OPL_PARSER_AllocLinkageDes,
		OPL_PARSER_FreeLinkageDes,
		OPL_PARSER_ParseLinkageDes,
		OPL_PARSER_PrintLinkageDes
	},

};

OPLParsingTable *OPL_PARSER_GetParsingDesc( HINT32 tag )
{
	HINT32			max =0, i=0;
	OPLParsingTable	*pRetTable = NULL;

	max = sizeof( s_parsingTable ) / sizeof( OPLParsingTable );

	if( max > 0 )
	{
		for( i =0; i< max; i++)
		{
			if( s_parsingTable[i].tag == tag )
			{

				pRetTable = &s_parsingTable[i];
				break;
			}
		}
	}

	return pRetTable;
}

#define ______0x89_eSIxDESCTAG_UKDTT_GUIDANCE_____
void  OPL_PARSER_UKDTT_FreeGuidanceDes(void *pucDesc)
{
	APKS_UkDttGuidanceDes_t 	*pstDes = (APKS_UkDttGuidanceDes_t *)pucDesc;

	if(pstDes == NULL || pstDes->ucDesTag != eSIxDESCTAG_UKDTT_GUIDANCE)
	{
		HxLOG_Error(" pstDes(0x%x) ucDesTag(0x%X) invalid\n" ,pstDes,(pstDes==NULL)?0: pstDes->ucDesTag);
		return;
	}

	if(pstDes != NULL)
	{
		if(pstDes->pUkDttGuidanceInfo!= NULL)
		{
			HLIB_STD_MemFree(pstDes->pUkDttGuidanceInfo);
			pstDes->pUkDttGuidanceInfo = NULL;
		}

		if(pstDes->pPrivateData!= NULL)
		{
			HLIB_STD_MemFree(pstDes->pPrivateData);
			pstDes->pPrivateData = NULL;
		}
		HLIB_STD_MemFree (pstDes);
		pstDes = NULL;
	}
}

HUINT8*  OPL_PARSER_UKDTT_AllocGuidanceDes(void)
{
	APKS_UkDttGuidanceDes_t	*pstDes = NULL;

	pstDes = (APKS_UkDttGuidanceDes_t *)HLIB_STD_MemAlloc(sizeof(APKS_UkDttGuidanceDes_t));
	if(pstDes != NULL)
	{
		HxSTD_MemSet(pstDes, 0, sizeof(APKS_UkDttGuidanceDes_t));
		pstDes->pUkDttGuidanceInfo = NULL;
	}

	return (HUINT8 *)pstDes;
}
APKS_UkDttGuidanceInfo_t *OPL_PARSER_UKDTT_AllocGuidanceInfo(void)
{
	APKS_UkDttGuidanceInfo_t *pstInfo = NULL;

	pstInfo = (APKS_UkDttGuidanceInfo_t *)HLIB_STD_MemAlloc(sizeof(APKS_UkDttGuidanceInfo_t));
	if (NULL == pstInfo)
	{
		return NULL;
	}

	HxSTD_MemSet(pstInfo->szIso639LangCode, 0, 4);
	HxSTD_MemSet(pstInfo->szGuidance, 0, 256);

	return pstInfo;
}
HERROR  OPL_PARSER_UKDTT_ParseGuidanceDes(HUINT8 *pucRaw, HUINT8 *pucDes)
{
	HUINT8				 	ucDesTag, ucDesLen;
	APKS_UkDttGuidanceDes_t			*pstDes;
	APKS_UkDttGuidanceInfo_t		*pstUkDttGuidanceInfo = NULL;
	HUINT8					szIso639LangCode[4];
	HUINT8					ucStrLen,ucAssignedGuidanceLen;
	HUINT8					*pszText = NULL;
	HUINT32					nTempTextLen;
	HUINT8					ucGuidanceMode;
	APKS_PrivateDataInfo_t	*pNewPrivateDataInfo = NULL;

	if (pucRaw == NULL)					{ return ERR_FAIL; }
	if (pucDes == NULL)					{ return ERR_FAIL; }

	pstDes = (APKS_UkDttGuidanceDes_t *)pucDes;

	ucDesTag = pucRaw[0]; // 1Byte
	ucDesLen = pucRaw[1]; // 1 Byte
	pucRaw += 2;

	if((ucDesTag != eSIxDESCTAG_UKDTT_GUIDANCE) || (ucDesLen < 1))
	{
		return ERR_FAIL;
	}

	pstDes->ucDesTag = eSIxDESCTAG_UKDTT_GUIDANCE;

	// Guidance Type (2 bit)
	pstDes->ucGuidanceType = (*pucRaw & 0x3);
	pucRaw += 1;
	ucDesLen --;

	if (0 == pstDes->ucGuidanceType)
	{
		pstUkDttGuidanceInfo = OPL_PARSER_UKDTT_AllocGuidanceInfo();
		if (NULL != pstUkDttGuidanceInfo)
		{
			// Guidance Mode
			pstUkDttGuidanceInfo->ucGuidanceMode = 0xFF;

			// ISO 639 language code (3byte)
			HxSTD_MemCopy(szIso639LangCode, pucRaw, 3);
			pucRaw += 3;
			ucDesLen -= 3;
			ucStrLen = ucDesLen;
			ucAssignedGuidanceLen = (ucStrLen >= MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : ucStrLen);

			HxSTD_MemCopy(pstUkDttGuidanceInfo->szIso639LangCode, szIso639LangCode, 3);
			pstUkDttGuidanceInfo->szIso639LangCode[3] = '\0';

			// Guidance string.
			pszText = (HUINT8 *)SILIB_TEXT_MakeUtf8Ex(eSIxTEXT_ENC_300468_UK, (const HCHAR *)pstUkDttGuidanceInfo->szIso639LangCode, (const HUINT8 *)pucRaw, (HUINT32)ucAssignedGuidanceLen, NULL, NULL);
			if(pszText)
			{
				nTempTextLen = HxSTD_StrLen((HCHAR *)pszText);
				if(nTempTextLen >= MAX_DESCRIPTOR_LIMIT)
				{
					nTempTextLen = MAX_DESCRIPTOR_LIMIT;
				}
				HxSTD_MemSet(pstUkDttGuidanceInfo->szGuidance, 0, (sizeof(HUINT8) * (nTempTextLen + 1)));
				HxSTD_MemCopy(pstUkDttGuidanceInfo->szGuidance, pszText, HxSTD_StrLen((HCHAR *)pszText));
				pstUkDttGuidanceInfo->szGuidance[HxSTD_StrLen((HCHAR *)pszText)] = UKDTT_DES_EOS;

				HLIB_STD_MemFree(pszText);
			}
			pstDes->pUkDttGuidanceInfo = pstUkDttGuidanceInfo;

		}
	}
	else if(1 == pstDes->ucGuidanceType)
	{
		pstUkDttGuidanceInfo = OPL_PARSER_UKDTT_AllocGuidanceInfo();
		if (NULL != pstUkDttGuidanceInfo)
		{
			ucGuidanceMode = *pucRaw++;
			ucDesLen--;
			ucGuidanceMode &= 1;
			// Guidance Mode
			pstUkDttGuidanceInfo->ucGuidanceMode = ucGuidanceMode;

			HxSTD_MemCopy(szIso639LangCode, pucRaw, 3);
			pucRaw += 3;
			ucDesLen -= 3;
			ucStrLen = ucDesLen;
			ucAssignedGuidanceLen = (ucStrLen >= MAX_DESCRIPTOR_LIMIT ? MAX_DESCRIPTOR_LIMIT : ucStrLen);

			// ISO 639 language code (3byte)
			HxSTD_MemCopy(pstUkDttGuidanceInfo->szIso639LangCode, szIso639LangCode, 3);
			pstUkDttGuidanceInfo->szIso639LangCode[3] = UKDTT_DES_EOS;

			// Guidance string.
			pszText = (HUINT8 *)SILIB_TEXT_MakeUtf8Ex(eSIxTEXT_ENC_300468_UK, (const HCHAR *)pstUkDttGuidanceInfo->szIso639LangCode,(const HUINT8 *)pucRaw, (HUINT32)ucAssignedGuidanceLen, NULL, NULL);
			if(pszText)
			{
				nTempTextLen = HxSTD_StrLen((HCHAR *)pszText);
				if(nTempTextLen >= MAX_DESCRIPTOR_LIMIT)
				{
					nTempTextLen = MAX_DESCRIPTOR_LIMIT;
				}
				HxSTD_MemSet(pstUkDttGuidanceInfo->szGuidance, 0, (sizeof(HUINT8) * (nTempTextLen + 1)));
				HxSTD_MemCopy(pstUkDttGuidanceInfo->szGuidance, pszText, HxSTD_StrLen((HCHAR *)pszText));
				pstUkDttGuidanceInfo->szGuidance[HxSTD_StrLen((HCHAR *)pszText)] = UKDTT_DES_EOS;

				HLIB_STD_MemFree(pszText);
			}
			pstDes->pUkDttGuidanceInfo = pstUkDttGuidanceInfo;

		}
	}
	else
	{
		pNewPrivateDataInfo = (APKS_PrivateDataInfo_t *)HLIB_STD_MemAlloc (sizeof(APKS_PrivateDataInfo_t));
		if (pNewPrivateDataInfo != NULL)
		{
			pNewPrivateDataInfo->ucPrivateDataLen	= (HUINT8) ucDesLen;
			pNewPrivateDataInfo->pPrivateData = HLIB_STD_MemAlloc(ucDesLen+1);

			if(pNewPrivateDataInfo->pPrivateData != NULL)
			{
				HxSTD_memcpy(pNewPrivateDataInfo->pPrivateData, pucRaw, ucDesLen);
			}

			pstDes->pPrivateData = pNewPrivateDataInfo;
		}
	}
	return ERR_OK;
}

void  OPL_PARSER_UKDTT_PrintGuidanceDes(HUINT8 *pucDes)
{
 // TODO :
}


#define ______0x4A_eSIxDESCTAG_LINKAGE_____
/************************************************************************/
/*	function : opl_parser_FreePrivateDataInfo										*/
/*  																	*/
/*  Description : 메모리 헤제.											*/
/************************************************************************/

STATIC 	void 	opl_parser_FreePrivateDataInfo(APKS_PrivateDataInfo_t *pPrivateDataInfo)
{
	if(pPrivateDataInfo == NULL)
	{
		return;
	}

	if (pPrivateDataInfo->pPrivateData != NULL)
	{
		HLIB_STD_MemFree(pPrivateDataInfo->pPrivateData);
	}

	HLIB_STD_MemFree(pPrivateDataInfo);
}
/************************************************************************/
/*	function : opl_parser_InitLinkDes												*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	opl_parser_InitLinkDes(APKS_LinkageDes_t *pLinkDes)
{
	HxSTD_MemSet(pLinkDes, 0, sizeof(APKS_LinkageDes_t));
}

/************************************************************************/
/*	function : opl_parser_InitPrivateDataInfo										*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	opl_parser_InitPrivateDataInfo(APKS_PrivateDataInfo_t *pPrivateDataInfo)
{
	pPrivateDataInfo->ucPrivateDataLen	= 0;
	pPrivateDataInfo->pPrivateData		= NULL;
}
/************************************************************************/
/*	function : opl_parser_AllocPrivateDataInfo										*/
/*  																	*/
/*  Description : 메모리 할당.											*/
/************************************************************************/

STATIC APKS_PrivateDataInfo_t *opl_parser_AllocPrivateDataInfo(void)
{
	APKS_PrivateDataInfo_t*	pPrivateDataInfo;

	pPrivateDataInfo = (APKS_PrivateDataInfo_t *)HLIB_STD_MemAlloc(sizeof(APKS_PrivateDataInfo_t));
	if (pPrivateDataInfo == NULL)
	{
		HxLOG_Error("[ERROR] HLIB_STD_MemAlloc(APKS_PrivateDataInfo_t) Error\n");
		return NULL;
	}

	opl_parser_InitPrivateDataInfo(pPrivateDataInfo);

	return ((APKS_PrivateDataInfo_t *)pPrivateDataInfo);
}

/************************************************************************/
/*	function : opl_parser_SetPrivateDataInfo										*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

STATIC void	opl_parser_SetPrivateDataInfo(APKS_PrivateDataInfo_t *pPrivateDataInfo, HUINT8 *pDesData, HUINT16 usPrivateDataLen)
{
	pPrivateDataInfo->ucPrivateDataLen	= (HUINT8) usPrivateDataLen;
	pPrivateDataInfo->pPrivateData = HLIB_STD_MemAlloc(usPrivateDataLen+1);

	if( pPrivateDataInfo->pPrivateData != NULL )
	{
		HxSTD_MemCopy(pPrivateDataInfo->pPrivateData, pDesData, usPrivateDataLen);

	}
}
void  OPL_PARSER_FreeLinkageDes(void *pucDesc)
{
	APKS_LinkageDes_t 	*pstDes = (APKS_LinkageDes_t *)pucDesc;

	if (pstDes == NULL ||pstDes->ucDesTag != eSIxDESCTAG_LINKAGE)
	{
		HxLOG_Error("  pstDes(0x%x) ucDesTag(0x%X) invalid\n" ,pstDes,(pstDes==NULL)?0: pstDes->ucDesTag);
		return;
	}

	if (pstDes->pPrivateDataInfo != NULL)
	{
		opl_parser_FreePrivateDataInfo(pstDes->pPrivateDataInfo);
	}

	HLIB_STD_MemFree(pucDesc);
}

HUINT8*  OPL_PARSER_AllocLinkageDes(void)
{
	APKS_LinkageDes_t	*pLinkDes = NULL;

	pLinkDes = (APKS_LinkageDes_t *)HLIB_STD_MemAlloc(sizeof(APKS_LinkageDes_t));
	if (pLinkDes == NULL)
	{
		HxLOG_Error("HLIB_STD_MemAlloc(APKS_LinkageDes_t) Error\n");
		return NULL;
	}

	opl_parser_InitLinkDes(pLinkDes);

	return ((HUINT8 *)pLinkDes);
}

HERROR  OPL_PARSER_ParseLinkageDes(HUINT8 *pucRaw, HUINT8 *pucDes)
{
	APKS_LinkageDes_t			*pLinkDes = NULL;
	APKS_PrivateDataInfo_t			*pNewPrivateDataInfo = NULL;
	HUINT8							ucDesTag;
	HUINT16							ucDesLen;

	//printf("parseLinkDes() Entered!");
	if (pucRaw == NULL)					{ return ERR_FAIL; }
	if (pucDes == NULL)					{ return ERR_FAIL; }

	pLinkDes = (APKS_LinkageDes_t *)pucDes;

	/** < descriptor tag (1byte) */
	ucDesTag = pucRaw[0]; // 1Byte
	/** < descriptor length (1byte) */
	ucDesLen = pucRaw[1]; // 1 Byte
	pucRaw += 2;

	if((ucDesTag != eSIxDESCTAG_LINKAGE) || (ucDesLen < 7))
	{
		return ERR_FAIL;
	}

	pLinkDes->ucDesTag = eSIxDESCTAG_LINKAGE;

	/** < transport stream id (2byte) */
	pLinkDes->usTsId = (HUINT16) HxMACRO_Get16Bit(pucRaw);
	pucRaw += 2;
	ucDesLen -= 2;
	//printf("\t\t(transport stream id = 0x%x)\n", pLinkDes->usTsId);

	/** < original network id (2byte) */
	pLinkDes->usOrgNetId = (HUINT16) HxMACRO_Get16Bit (pucRaw);
	pucRaw += 2;
	ucDesLen -= 2;
	//printf("\t\t(original network id = 0x%x)\n", pLinkDes->usOrgNetId);

	/** < service id (2byte) */
	pLinkDes->usSvcId = (HUINT16) HxMACRO_Get16Bit(pucRaw);
	pucRaw += 2;
	ucDesLen -= 2;
	//printf("\t\t(service id = 0x%x)\n", pLinkDes->usSvcId);

	/** < linkage type (1byte) */
	pLinkDes->ucLinkType = *pucRaw++;
	ucDesLen--;
	//printf("\t\t(linkage type = 0x%x)\n", pLinkDes->ucLinkType);

	/** < Private Data */
	if (ucDesLen > 0)
	{
		pNewPrivateDataInfo = (APKS_PrivateDataInfo_t *) opl_parser_AllocPrivateDataInfo();
		if (pNewPrivateDataInfo == NULL)
			return ERR_FAIL;
		opl_parser_SetPrivateDataInfo (pNewPrivateDataInfo, pucRaw, ucDesLen);
	}
	/** < Descriptor 데이타 구성. */
	pLinkDes->pPrivateDataInfo = pNewPrivateDataInfo;

	//printf("parseLinkDes() Leaved!");

	return ERR_OK;
}

void  OPL_PARSER_PrintLinkageDes(HUINT8 *pucDes)
{
 // TODO :
}
#define ______0x54_eSIxDESCTAG_CONTENT_____

/************************************************************************/
/*	function : opl_parser_FreeContentInfo											*/
/*  																	*/
/*  Description : 메모리 해제.											*/
/************************************************************************/

STATIC 	void 	opl_parser_FreeContentInfo(APKS_ContentInfo_t *pContentInfo)
{
	if(pContentInfo)
	{
		HLIB_STD_MemFree(pContentInfo);
	}
}
/************************************************************************/
/*	function : opl_parser_InitContentDes											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	opl_parser_InitContentDes(APKS_ContentDes_t *pContentDes)
{
	pContentDes->pContentInfo = NULL;
}
/************************************************************************/
/*	function : opl_parser_initContentInfo											*/
/*  																	*/
/*  Description : 초기화.												*/
/************************************************************************/

STATIC 	void 	opl_parser_initContentInfo(APKS_ContentInfo_t *pContentInfo)
{
	pContentInfo->ucContentNibble	= 0;
	pContentInfo->ucUserNibble		= 0;
	pContentInfo->pNextInfo			= NULL;
}
/************************************************************************/
/*	function : opl_parser_SetContentInfo											*/
/*  																	*/
/*  Description : value 설정.											*/
/************************************************************************/

STATIC void 	opl_parser_SetContentInfo(APKS_ContentInfo_t *pContentInfo, HUINT8 ucContentNibble, HUINT8 ucUserNibble)
{
	pContentInfo->ucContentNibble	= ucContentNibble;
	pContentInfo->ucUserNibble		= ucUserNibble;
}

/************************************************************************/
/*	function : opl_parser_GetPrevContentInfo										*/
/*  																	*/
/*  Description : 마지막 작업한 Info의 poHUINT32er를 얻는다.				*/
/************************************************************************/

STATIC APKS_ContentInfo_t 	*opl_parser_GetPrevContentInfo(APKS_ContentDes_t *pContentDes)
{
	APKS_ContentInfo_t	*pContentInfo;


	pContentInfo = pContentDes->pContentInfo;
	if (pContentInfo == NULL)
	{
		return NULL;
	}

	while (pContentInfo->pNextInfo != NULL)
	{
		pContentInfo = pContentInfo->pNextInfo;
	}

	return ((APKS_ContentInfo_t *)pContentInfo);
}
/************************************************************************/
/*	function : opl_parser_LinkContentInfoHead										*/
/*  																	*/
/*  Description : head의 link 설정.										*/
/************************************************************************/

STATIC void 	opl_parser_LinkContentInfoHead(APKS_ContentDes_t *pContentDes, APKS_ContentInfo_t *pContentInfo)
{
	pContentDes->pContentInfo = pContentInfo;
}
/************************************************************************/
/*	function : opl_parser_LinkContentInfoNode										*/
/*  																	*/
/*  Description : node의 link 설정.										*/
/************************************************************************/

STATIC void 	opl_parser_LinkContentInfoNode(APKS_ContentInfo_t *pNewContentInfo, APKS_ContentInfo_t *pNextContentInfo, APKS_ContentInfo_t *pPrevContentInfo)
{
	if (pPrevContentInfo)
	{
		pPrevContentInfo->pNextInfo = pNewContentInfo;
	}

	pNewContentInfo->pNextInfo = pNextContentInfo;
}

void  OPL_PARSER_FreeContentDes(void *pucDesc)
{
	APKS_ContentDes_t 	*pstContentDes = (APKS_ContentDes_t 	*)pucDesc;
	APKS_ContentInfo_t	*pIndexContentInfo = NULL, *pDestContentInfo = NULL;

	if(pstContentDes == NULL || pstContentDes->ucDesTag != eSIxDESCTAG_CONTENT)
	{
		HxLOG_Error("pstDes(0x%x) ucDesTag(0x%X) invalid\n",pstContentDes,(pstContentDes==NULL)?0: pstContentDes->ucDesTag);
		return;
	}

	if(pstContentDes != NULL)
	{
		pIndexContentInfo = pstContentDes->pContentInfo;
		while (pIndexContentInfo)
		{
			pDestContentInfo = pIndexContentInfo;
			pIndexContentInfo = pIndexContentInfo->pNextInfo;
			opl_parser_FreeContentInfo(pDestContentInfo);
		}

		HLIB_STD_MemFree (pstContentDes);
		pstContentDes = NULL;
	}
}


HUINT8*  OPL_PARSER_AllocContentDes(void)
{
	APKS_ContentDes_t	*pContentDes = NULL;

	pContentDes = (APKS_ContentDes_t *)HLIB_STD_MemAlloc(sizeof(APKS_ContentDes_t));
	if (pContentDes == NULL)
	{
		HxLOG_Error("HLIB_STD_MemAlloc(APKS_ContentDes_t) Error\n");
		return NULL;
	}

	opl_parser_InitContentDes(pContentDes);

	return ((HUINT8 *)pContentDes);
}

APKS_ContentInfo_t *OPL_PARSER_AllocContentInfo(void)
{
	APKS_ContentInfo_t	*pContentInfo = NULL;

	pContentInfo = (APKS_ContentInfo_t *)HLIB_STD_MemAlloc(sizeof(APKS_ContentInfo_t));
	if (pContentInfo == NULL)
	{
		HxLOG_Error("HLIB_STD_MemAlloc(APKS_ContentInfo_t) Error\n");
		return NULL;
	}
	opl_parser_initContentInfo(pContentInfo);

	return ((APKS_ContentInfo_t *)pContentInfo);
}


HERROR  OPL_PARSER_ParseContentDes(HUINT8 *pucRaw, HUINT8 *pucDes)
{
	APKS_ContentDes_t		*pContentDes = (APKS_ContentDes_t *)pucDes;
	APKS_ContentInfo_t	*pPrevContentInfo = NULL, *pNewContentInfo = NULL, *pNextContentInfo = NULL;
	HUINT8			ucDesTag;
	HUINT8			ucContentNibble, ucUserNibble;
	HUINT16			usDesLen;

	//printf("parseContentDes() Entered!\n");

	/** < descriptor tag (1byte) */
	ucDesTag = *pucRaw; // 1Byte
	pucRaw += 1;

	//printf("\t\t(descriptor tag = 0x%x)\n", ucDesTag);

	/** < descriptor length (1byte) */
	usDesLen = *pucRaw; // 1Byte
	pucRaw += 1;

	//printf("\t\t(descriptor length = %d)\n", usDesLen);
	if((ucDesTag != eSIxDESCTAG_CONTENT) || (usDesLen < 1))
	{
		return ERR_FAIL;
	}

	if ((usDesLen % 2) != 0 || usDesLen == 0)
	{
		//printf("[WARN] Unknown ContentDes Format!\n");
		return ERR_FAIL;
	}

	pContentDes->ucDesTag = eSIxDESCTAG_CONTENT;

	while (usDesLen > 0 && usDesLen < MAX_DESCRIPTOR_LIMIT)
	{
		/** < content nibble (1byte) */
		ucContentNibble = *pucRaw; // 1Byte
		pucRaw += 1;
		usDesLen--;
		//printf("\t\t\t(content nibble = 0x%x)\n", ucContentNibble);

		ucUserNibble = *pucRaw; // 1Byte
		pucRaw += 1;
		usDesLen--;
		//printf("\t\t\t(user nibble = 0x%x)\n", ucUserNibble);

		/** < Descriptor 데이타 구성. */
		pPrevContentInfo = opl_parser_GetPrevContentInfo(pContentDes);
		pNewContentInfo = (APKS_ContentInfo_t *)OPL_PARSER_AllocContentInfo ();
		if (pNewContentInfo == NULL)
			goto ErrBlock;
		opl_parser_SetContentInfo(pNewContentInfo, ucContentNibble, ucUserNibble);
		if (pPrevContentInfo == NULL)
			opl_parser_LinkContentInfoHead(pContentDes, pNewContentInfo);
		opl_parser_LinkContentInfoNode(pNewContentInfo, pNextContentInfo, pPrevContentInfo);
	}

	//printf(" parseContentDes() Leaved!\n");

	return ERR_OK;

ErrBlock:
	return ERR_FAIL;
}

void  OPL_PARSER_PrintContentDes(HUINT8 *pucDes)
{
 // TODO :
}

#define ___________________________PARSING________________________________________________________

HxVector_t	*OPL_PARSER_ParsingDesc( HxVector_t *descriptor, HINT32 tag , HBOOL bPrivate )
{
	HxVector_t 					*list = NULL;
	const HUINT8					*desc = NULL;
	void							*comp = NULL;
	HERROR						hErr = ERR_FAIL;
	OPLParsingTable				*pstDesc = NULL;
	HUINT32 					ulPrivateSpecifyValue = 0;
	HBOOL						bPrivateConfirm = FALSE;
	HUINT8 					ucContentNibble = 0;

	//printf("[%s:%d] \n",__FUNCTION__,__LINE__);
	HxLOG_Trace();
	return_if(descriptor == NULL , NULL);

	//printf("[%s:%d] \n",__FUNCTION__,__LINE__);
	pstDesc = OPL_PARSER_GetParsingDesc( tag );
	//printf("[%s:%d] \n",__FUNCTION__,__LINE__);
	return_if(pstDesc == NULL , NULL);

	//printf("[%s:%d] \n",__FUNCTION__,__LINE__);
	list = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))pstDesc->free, NULL);
	return_if(list == NULL, NULL);
	//printf("[%s:%d] \n",__FUNCTION__,__LINE__);

	if(descriptor)
	{
		//printf("[%s:%d] \n",__FUNCTION__,__LINE__);
		HxVECTOR_EACH(descriptor, const HUINT8 *, desc, {
			if (desc == NULL)
				continue;

			//printf("[%s:%d] desc[0]:%x\n",__FUNCTION__,__LINE__,desc[0]);
			if(desc[0] == eSIxDESCTAG_PRIVATE_DATA_SPECIFIER/* && bPrivate == TRUE*/)
			{
				ulPrivateSpecifyValue = HxMACRO_Get32Bit(&(desc[2]));
				if(ulPrivateSpecifyValue == PRIVATE_DATA_SPECIFIER_FOR_UK_DTT)
				{
					bPrivateConfirm = TRUE;
				}
				continue;
			}

			if(desc[0] == eSIxDESCTAG_PRIVATE_DATA_INDICATOR && bPrivate == TRUE)
			{
				bPrivateConfirm = FALSE;
				continue;
			}

			if( tag != desc[0] )
				continue;

			if(bPrivate== TRUE)
			{
				if(bPrivateConfirm == FALSE)
					continue;
			}

			if(desc[0] == eSIxDESCTAG_CONTENT && bPrivateConfirm == FALSE)
			{
				ucContentNibble = HxMACRO_Get8Bit(&(desc[2]));
				if((ucContentNibble & 0xf0) == 0xf0)
				{
					continue;
				}
			}

			comp =pstDesc->alloc();
			if( comp == NULL )
				continue;

			hErr = pstDesc->parse( (HUINT8 *)desc , (HUINT8 *)comp );
			if( hErr == ERR_OK && comp )
			{
				(void) HLIB_VECTOR_Add(list, comp);
			}
		});
	}

	if (HLIB_VECTOR_Length(list) == 0)
	{
		HLIB_VECTOR_Delete(list);
		return NULL;
	}

	return list;
}

