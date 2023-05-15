/* $Header: $ */

#ifndef __SIxDESCPARSER_H__
#define __SIxDESCPARSER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hlib.h"
#include "../include/silib.h"

#ifdef ___DESCRIPTOR_PARSER_TEMPLATE___
// USE this template to implement descriptor parsing module

// Types
typedef struct
{
	HUINT8					ucABC;
	// ...
} SIxABC_t;

// Move these to silib_desc_[specname].h	 e.g) silib_desc_arib.h
typedef struct tagSIxXXXInfo
{
	SIxABC_t					*pstABC;
	HUINT8					*pszABC;
	HUINT32					 ulABCNum;
	HUINT8					*aucABC;
	struct tagSIxXXXInfo		*pstNext;
} SIxXXXInfo_t;

typedef struct
{
	SIxDescriptorHeader_t				 stHead;
	// Des Field1
	// Des Field2
	// Des Field3	...
	HUINT8					*pszABC;
	HUINT32					 ulABCNum;
	HUINT8					*aucABC;
	SIxXXXInfo_t				*pstInfo;
} SIxXXXDes_t;

//XXX
// Functions
// Move to sxdescparse_[specname].c		e.g) sxdescparse_arib.c
static void initXXXDes(SIxXXXDes_t *pstDes)
{
	HxSTD_MemSet(pstDes, 0, sizeof(SIxXXXDes_t));
}

static void initXXXInfo(SIxXXXInfo_t *pstInfo)
{
	HxSTD_MemSet(pstInfo, 0, sizeof(SIxXXXInfo_t));
}

static SIxXXXDes_t *allocXXXDes(void)
{
	return (SIxXXXDes_t *)silib_desc_alloc(sizeof(SIxXXXDes_t));
}

static SIxXXXDes_t *allocXXXInfo(void)
{
	SIxXXXInfo_t		*pstInfo = NULL;

	pstInfo = (SIxXXXDes_t *)silib_desc_alloc(sizeof(SIxXXXDes_t));
	if (pstInfo)
	{
		initXXXInfo(pstInfo);
	}

	return pstInfo;
}

static void freeXXXInfo(SIxXXXInfo_t *pstInfo)
{
	if (pstInfo)
	{
		// silib_desc_free(pstInfo->pszYYY);
		// silib_desc_free(pstInfo->pstZZZ);
		silib_desc_free(pstInfo);
	}
}

static void freeXXXDes(SIxXXXDes_t *pstDes)
{
	if (NULL == pstDes)		return;

	FREE_INFO_LIST_EX(pstDes->pstInfo, SIxXXXInfo_t, freeXXXInfo);
	silib_desc_free(pstDes);
}

static void setXXXInfo(SIxXXXInfo_t *pstInfo, a, b, c)
{
	pstInfo->a	= a;
	pstInfo->b	= b;
	pstInfo->c	= c;
}

static SIxError_e parseXXXDes(HUINT8 *p, SIxXXXDes_t *pstDes)
{
	SIxXXXInfo_t		*pstPrevInfo, *pstNewInfo;
	HUINT16				 usDesLen;
/*
	HUINT8				 ucXXXLen, ucAssignedXXXLen;
	HUINT16 			 usTempXXXLen;
	HUINT8				 szIso639LangCode[4], *pszXXX = NULL;
*/

	pstPrevInfo = pstNewInfo = NULL;

	// descriptor tag (1byte)
	p++;
	// descriptor length (1byte)
	usDesLen = (HUINT16) *p++;

	if (usDesLen < 6 || usDesLen > (HUINT16)MAX_DESCRIPTOR_LIMIT)
	{
		return eSIxERR_FAIL;
	}

	while (usDesLen > 0)
	{
		// 1. alloc Info
		pstNewInfo = allocXXXInfo();
		if (NULL == pstNewInfo)
		{
			goto err_exit;
		}

		// 2. Descriptor 데이타 구성.
		a = HxMACRO_Get12Bit(p);	p += 2;
		b = HxMACRO_Get8Bit(p);		p += 1;
		c = HxMACRO_Get16Bit(p);	p += 2;

		// 3. Set Info
		setXXXInfo(pstNewInfo, a, b, c);

		// 4. Link Operation
		LINK_DES_NODE(pstDes->pstInfo, pstPrevInfo, pstNewInfo);
	}
	return eSIxERR_OK;

err_exit:
	if (pstDes)
	{
		FREE_INFO_LIST_EX(pstDes->pstInfo, SIxXXXInfo_t, freeXXXInfo);
		pstDes->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

static SIxError_e copyXXXInfo(SIxXXXInfo_t *pstDest, const SIxXXXInfo_t *pstSource)
{
	if (NULL == pstDest || NULL == pstSource)
		return eSIxERR_FAIL;

	// 1. copy pointer field, (string, array, structure pointer, ...)

	// 1.1. alloc pointer

	// 1.2. memcopy pointer

	return eSIxERR_OK;

err_exit:
	// 2. free pointer field
	return eSIxERR_FAIL;
}

static SIxError_e copyXXXDes(SIxXXXDes_t *pstDest, const SIxXXXDes_t *pstSource)
{
	SIxError_e				 eErr = eSIxERR_FAIL;

	if (pstSource && pstDest)
	{
		SIxXXXInfo_t *pstSourceInfo, *pstPrevInfo = NULL, *pstNewInfo = NULL;

		// 1. copy descriptor
		HxSTD_MemCopy(pstDest, pstSource, sizeof(SIxXXXDes_t));
		pstDest->pstInfo = NULL;
		eErr = eSIxERR_OK;

		pstSourceInfo = pstSource->pstInfo;
		while (pstSourceInfo)
		{
			GET_PREV_INFO(pstPrevInfo, SIxXXXInfo_t, pstDest);

			// 2. alloc Info
			pstNewInfo = (SIxXXXInfo_t *)allocXXXInfo();
			if (pstNewInfo == NULL)
				goto err_exit;

			// 3. copy Info
			HxSTD_MemCopy(pstNewInfo, pstSourceInfo, sizeof(SIxXXXInfo_t));

			// 3.1. init Pointer (string, array, array num, ...)
			pstNewInfo->pstNext = NULL;
/*
			pstNewInfo->pszYYY = NULL;
			pstNewInfo->pstZZZ = NULL;
			pstNewInfo->aucUUU = NULL;
			pstNewInfo->ucNum = 0;
*/
			// 3.2. copy info pointer (string, array, sub info, ...)
/*
			if (copyXXXInfo(pstNewInfo, pstSourceInfo) != eSIxERR_OK)
			{
				silib_desc_free(pstNewInfo);
				goto err_exit;
			}
*/
			// 4. Link Operation
			LINK_INFO_NODE(pstDest->pstInfo, pstNewInfo, NULL, pstPrevInfo);

			pstSourceInfo = pstSourceInfo->pstNext;
		}
		eErr = eSIxERR_OK;
	}
	return eErr;

err_exit:
	if (pstDest)
	{
		FREE_INFO_LIST_EX(pstDest->pstInfo, SIxXXXInfo_t, freeXXXInfo);
		pstDest->pstInfo = NULL;
	}
	return eSIxERR_FAIL;
}

//XXX
static SIxDescriptor_t *silib_desc_allocXXXDes(void)
{
	SIxXXXDes_t	*pstDes;

	pstDes = allocXXXDes();
	if (pstDes)
	{
		initXXXDes(pstDes);
	}

	return (SIxDescriptor_t *)pstDes;
}

static SIxError_e silib_desc_freeXXXDes(SIxDescriptor_t *pstDes)
{
	freeXXXDes((SIxXXXDes_t *)pstDes);
	return eSIxERR_OK;
}

static SIxError_e silib_desc_parseXXXDes(const HUINT8 *pucRawData, SIxDescriptor_t *pstDes)
{
	return parseXXXDes((HUINT8 *)pucRawData, (SIxXXXDes_t *)pstDes);
}

static SIxError_e silib_desc_copyXXXDes(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource)
{
	return copyXXXDes((SIxXXXDes_t *)pstDest, (SIxXXXDes_t *)pstSource);
}

static void silib_desc_printXXXDes(const SIxDescriptor_t *pstDes)
{
	//(SIxXXXDes_t *)pstDes;
}
#endif


#define MAX_DESCRIPTOR_LIMIT			255

#define	LINK_INFO_NODE(headInfo, newInfo, nextInfo, prevInfo)	\
	do {														\
		if (NULL == prevInfo) { headInfo = newInfo; }			\
		else { prevInfo->pstNext = newInfo; }					\
		newInfo->pstNext = nextInfo;							\
	} while(0)

#define LINK_DES_NODE(headInfo, prevInfo, newInfo)			\
	if (NULL == prevInfo) {									\
		headInfo = newInfo;									\
		prevInfo = newInfo;									\
	} else {												\
		prevInfo->pstNext = newInfo;						\
		prevInfo = prevInfo->pstNext;						\
	}

#define GET_PREV_INFO(destInfo, type, desc)					\
	do {													\
		type *pstList = desc->pstInfo;						\
		if (pstList == NULL)	{ 							\
			destInfo = NULL;								\
		} else {											\
			while (pstList->pstNext != NULL)				\
			{												\
				pstList = pstList->pstNext;					\
			}												\
			destInfo = pstList;								\
		}													\
	} while(0)

#define GET_PREV_LINK(destInfo, type, headInfo)				\
	do {													\
		type *pstList = headInfo; 							\
		if (pstList == NULL) {								\
			destInfo = NULL; 								\
		} else {											\
			while (pstList->pstNext != NULL) 				\
			{												\
				pstList = pstList->pstNext;					\
			}												\
			destInfo = pstList; 							\
		}													\
	} while(0)

#define	FREE_INFO_LIST(headInfo, type)						\
	do {													\
		type	*pstList, *pstInfoToFree;					\
		pstList = headInfo;									\
		while (pstList)										\
		{													\
			pstInfoToFree = pstList;						\
			pstList = pstList->pstNext;						\
			silib_desc_free(pstInfoToFree);						\
		}													\
	} while(0)

#define	FREE_INFO_LIST_EX(headInfo, type, freeFunc)			\
	do {													\
		type 	*pstList, *pstInfoToFree;					\
		pstList = headInfo;									\
		while (pstList)										\
		{													\
			pstInfoToFree = pstList;						\
			pstList = pstList->pstNext;						\
			freeFunc(pstInfoToFree);						\
		}													\
	} while(0)


void *silib_desc_alloc(HLONG ulSize);
HINT32 silib_desc_free(void *p);

/*	Util */
HUINT32	silib_desc_util_uriStrlen(HUINT8 ucSrcLen, HUINT8 *pucStr);
HUINT32	silib_desc_util_uriStrcpy(HUINT8 *pucDestUri, HUINT8 *pucSrcStr, HUINT8 ucStrLen);
HULONG silib_desc_util_bcdToBinary(HULONG ulVal);
SIxError_e silib_desc_util_makeCorrct639Code(HUINT8 *p);
HUINT32 silib_desc_util_dvbStrlen(const HUINT8 *pucStr);
HUINT8 *silib_desc_util_allocStr(HUINT32 nStrLen);
SIxTextEncoding_e silib_desc_util_getTextEnc(void);
void silib_desc_util_setTextEnc(SIxTextEncoding_e eEnc);
HUINT32 silib_desc_util_textToUTF8(const HCHAR *szLangCode, SIxTextEncoding_e eEnc, HUINT32 ucStrLen, const HUINT8 *pucSiText, HUINT8 **ppucDestStr);
SIxPrivateDataInfo_t *silib_desc_util_allocPrivateDataInfo(HUINT8 ucLength);
void silib_desc_util_freePrivateDataInfo(SIxPrivateDataInfo_t *pstInfo);
SIxError_e silib_desc_util_setPrivateDataInfo(SIxPrivateDataInfo_t *pstInfo, HUINT8 *pucData, HUINT8 ucLength);
SIxError_e silib_desc_util_parsePrivateDataInfo(HUINT8 *p, SIxPrivateDataInfo_t *pstDes);
SIxError_e silib_desc_util_copyPrivateDataInfo(SIxPrivateDataInfo_t *pstDest, const SIxPrivateDataInfo_t *pstSource);
SIxPrivateDataInfo_t *silib_desc_util_dumpPrivateDataInfo(const SIxPrivateDataInfo_t *pstSource);


/*	Dvb */
SIxDescriptor_t *silib_desc_alloc_Dvb(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen);
SIxError_e silib_desc_free_Dvb(SIxDescriptor_t *pstDes);
SIxError_e silib_desc_parse_Dvb(const SIxDescriptorTag_e eTag, const SIxDescriptorHeaderParam_t *pstParam, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes);
SIxError_e silib_desc_copy_Dvb(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource);
SIxError_e silib_desc_print_Dvb(const SIxDescriptor_t *pstDes);

/*	Dvb Extend */
SIxDescriptor_t *silib_desc_alloc_DvbExt(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen);
SIxError_e silib_desc_free_DvbExt(SIxDescriptor_t *pstDes);
SIxError_e silib_desc_parse_DvbExt(const SIxDescriptorTag_e eTag, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes);
SIxError_e silib_desc_copy_DvbExt(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource);
SIxError_e silib_desc_print_DvbExt(const SIxDescriptor_t *pstDes);

/*	Dvb Extend */
SIxDescriptor_t *silib_desc_alloc_Unt(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen);
SIxError_e silib_desc_free_Unt(SIxDescriptor_t *pstDes);
SIxError_e silib_desc_parse_Unt(const SIxDescriptorTag_e eTag, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes);
SIxError_e silib_desc_copy_Unt(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource);
SIxError_e silib_desc_print_Unt(const SIxDescriptor_t *pstDes);

/*	Arib */
SIxDescriptor_t *silib_desc_alloc_Arib(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen);
SIxError_e silib_desc_free_Arib(SIxDescriptor_t *pstDes);
SIxError_e silib_desc_parse_Arib(const SIxDescriptorTag_e eTag, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes);
SIxError_e silib_desc_copy_Arib(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource);
SIxError_e silib_desc_print_Arib(const SIxDescriptor_t *pstDes);

/*	Skapa */
SIxDescriptor_t *silib_desc_alloc_Skapa(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen);
SIxError_e silib_desc_free_Skapa(SIxDescriptor_t *pstDes);
SIxError_e silib_desc_parse_Skapa(const SIxDescriptorTag_e eTag, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes);
SIxError_e silib_desc_copy_Skapa(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource);
SIxError_e silib_desc_print_Skapa(const SIxDescriptor_t *pstDes);

/*	Fsat */
SIxDescriptor_t *silib_desc_alloc_Fsat(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen);
SIxError_e silib_desc_free_Fsat(SIxDescriptor_t *pstDes);
SIxError_e silib_desc_parse_Fsat(const SIxDescriptorTag_e eTag, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes);
SIxError_e silib_desc_copy_Fsat(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource);
SIxError_e silib_desc_print_Fsat(const SIxDescriptor_t *pstDes);

/*	Ziggo */
SIxDescriptor_t *silib_desc_alloc_Ziggo(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen);
SIxError_e silib_desc_free_Ziggo(SIxDescriptor_t *pstDes);
SIxError_e silib_desc_parse_Ziggo(const SIxDescriptorTag_e eTag, const SIxDescriptorHeaderParam_t *pstParam, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes);
SIxError_e silib_desc_copy_Ziggo(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource);
SIxError_e silib_desc_print_Ziggo(const SIxDescriptor_t *pstDes);

/*	Ses */
SIxDescriptor_t *silib_desc_alloc_Ses(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen);
SIxError_e silib_desc_free_Ses(SIxDescriptor_t *pstDes);
SIxError_e silib_desc_parse_Ses(const SIxDescriptorTag_e eTag, const SIxDescriptorHeaderParam_t *pstParam, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes);
SIxError_e silib_desc_copy_Ses(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource);
SIxError_e silib_desc_print_Ses(const SIxDescriptor_t *pstDes);


/*	Ertelecom */
SIxDescriptor_t *silib_desc_alloc_Ertelecom(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen);
SIxError_e silib_desc_free_Ertelecom(SIxDescriptor_t *pstDes);
SIxError_e silib_desc_parse_Ertelecom(const SIxDescriptorTag_e eTag, const SIxDescriptorHeaderParam_t *pstParam, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes);
SIxError_e silib_desc_copy_Ertelecom(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource);
SIxError_e silib_desc_print_Ertelecom(const SIxDescriptor_t *pstDes);


/*	AusDtt */
SIxDescriptor_t *silib_desc_alloc_Ausdtt(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen);
SIxError_e silib_desc_free_Ausdtt(SIxDescriptor_t *pstDes);
SIxError_e silib_desc_parse_Ausdtt(const SIxDescriptorTag_e eTag, const SIxDescriptorHeaderParam_t *pstParam, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes);
SIxError_e silib_desc_copy_Ausdtt(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource);
SIxError_e silib_desc_print_Ausdtt(const SIxDescriptor_t *pstDes);


/*	UkDtt */
SIxDescriptor_t *silib_desc_alloc_Ukdtt(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen);
SIxError_e silib_desc_free_Ukdtt(SIxDescriptor_t *pstDes);
SIxError_e silib_desc_parse_Ukdtt(const SIxDescriptorTag_e eTag, const SIxDescriptorHeaderParam_t *pstParam, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes);
SIxError_e silib_desc_copy_Ukdtt(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource);
SIxError_e silib_desc_print_Ukdtt(const SIxDescriptor_t *pstDes);

/*	ItaDtt */
SIxDescriptor_t *silib_desc_alloc_Itadtt(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen);
SIxError_e silib_desc_free_Itadtt(SIxDescriptor_t *pstDes);
SIxError_e silib_desc_parse_Itadtt(const SIxDescriptorTag_e eTag, const SIxDescriptorHeaderParam_t *pstParam, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes);
SIxError_e silib_desc_copy_Itadtt(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource);
SIxError_e silib_desc_print_Itadtt(const SIxDescriptor_t *pstDes);

/*	NorDig */
SIxDescriptor_t *silib_desc_alloc_Nordig(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen);
SIxError_e silib_desc_free_Nordig(SIxDescriptor_t *pstDes);
SIxError_e silib_desc_parse_Nordig(const SIxDescriptorTag_e eTag, const SIxDescriptorHeaderParam_t *pstParam, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes);
SIxError_e silib_desc_copy_Nordig(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource);
SIxError_e silib_desc_print_Nordig(const SIxDescriptor_t *pstDes);

/*	MBC */
SIxDescriptor_t *silib_desc_alloc_Mbc(const SIxDescriptorTag_e eTag, HUINT32 ulRawLen);
SIxError_e silib_desc_free_Mbc(SIxDescriptor_t *pstDes);
SIxError_e silib_desc_parse_Mbc(const SIxDescriptorTag_e eTag, const SIxDescriptorHeaderParam_t *pstParam, const HUINT8 *pucRawData, SIxDescriptor_t *pstDes);
SIxError_e silib_desc_copy_Mbc(SIxDescriptor_t *pstDest, const SIxDescriptor_t *pstSource);
SIxError_e silib_desc_print_Mbc(const SIxDescriptor_t *pstDes);

#ifdef __cplusplus
}
#endif

#endif // __SIxDESCPARSER_H__



