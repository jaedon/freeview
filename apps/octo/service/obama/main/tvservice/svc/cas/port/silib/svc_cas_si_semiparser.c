/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

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
#include <util.h>
#include <octo_common.h>

#include <pal_sef.h>

/** @brief local header file  */
#include "../local_include/_svc_cas_si_api.h"
#include "_svc_cas_si_desmgr.h"
#include "_svc_cas_si_desparse.h"

#include <db_svc.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
/*
 * 아래의 Basic function API를 사용하여 또 다른 함수를 구성해야 할 경우
 * 반드시 sema를 사용해야 한다. 즉 아래 함수들을 연속적으로 사용하는
 * 동안에는 raw data가 raw pool에 삽입 되거나 하면 index등이 효력을
 * 잃기 때문에 오류가(bug) 발생할 가능성이 생긴다. hcyoo
 */


/**
  @brief     LAYER_MODULE_DoSomething 함수.

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/

#define ________STATIC_PROTOTYPES________

// ======== CAT ======== //
STATIC INLINE void svc_cas_SiGetFirstCatDescriptorPos(HUINT8 *pucSection, HUINT8 **ppFirstProgDes);
STATIC HERROR svc_cas_SiCountCatSectionInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 *pulCnt);
STATIC HERROR svc_cas_SiCountCatDescriptorInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 ulTag, HUINT32 *pnCnt);
STATIC HERROR svc_cas_SiGetCatDescriptorTagInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 nDesIdx, HUINT8	*pucTag);
STATIC HERROR svc_cas_SiGetCatDescriptorInternal(svcCas_RawPool_t *pstRawPool, SIxTextEncoding_e eTextEnc, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 ulTag, HUINT32 nDesIdx, void **ppvDes);
STATIC HERROR svc_cas_SiGetCatDescriptorRawInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 ulTag, HUINT32 nDesIdx, HUINT32 *pulDesRawSize, HUINT8 **ppucDesRaw);
STATIC HUINT8 *svc_cas_SiFindCatSectionTable(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulIdx);
STATIC HERROR svc_cas_SiCountCatDescriptorFromRaw(HUINT8 *pucRaw, HUINT32 ulTag, HUINT32 *pulDesCnt);
STATIC HERROR svc_cas_SiGetCatDescriptorTagFromRaw(HUINT8 *pucRaw, HUINT32 ulDesIdx, HUINT8 *pucTag);
STATIC HERROR svc_cas_SiGetCatDescriptorFromRaw(HUINT8 *pucRaw, SIxTextEncoding_e eTextEnc, HUINT32 ulTag, HUINT32 ulDesIdx, void **ppvDes);
STATIC HERROR svc_cas_SiGetCatDescriptorRawFromRaw(HUINT8 *pucRaw, HUINT32 ulTag, HUINT32 ulDesIdx, HUINT32 *pulDesRawSize, HUINT8 **ppucDesRaw);

// ======== PAT ======== //

STATIC INLINE void svc_cas_SiGetFirstProgramInfoPos(HUINT8 *pucSection, HUINT8 **ppFirstProg);
STATIC INLINE void svc_cas_SiGetNextProgramInfoPos(HUINT8 *pucProg, HUINT8 **ppNextProg);
STATIC HUINT8 *svc_cas_SiFindPatSectinTable(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulIdx);
STATIC HERROR svc_cas_SiCountPatSectionInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 *pulCnt);
STATIC HERROR svc_cas_SiGetPatSectionFromRawInternal(HUINT8 *pucRaw, svcCas_SiPatSectionInfo_t *pstPatInfo);
STATIC HERROR svc_cas_SiGetPatSectionInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulIdx, svcCas_SiPatSectionInfo_t *pstPatInfo);
STATIC HERROR svc_cas_SiCountPatProgramFromRawInternal(HUINT8 *pucRaw, HUINT32 *pulCnt);
STATIC HERROR svc_cas_SiCountPatProgramInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 *pulCnt);
STATIC HERROR svc_cas_SiGetPatProgramInfoFromRawInternal(HUINT8 *pucRaw, HUINT32 ulInfoIdx, svcCas_SiPatProgInfo_t *pstInfo);

// ======== PAT SVC ID ======== //

STATIC HERROR svc_cas_SiGetPmtPidByServiceIdFromRawInternal(HUINT8 *pucRaw, HUINT16 usSvcId, HUINT16 *pusPmtPid);
STATIC HERROR svc_cas_SiGetPmtPidByServiceIdInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT16 usSvcId, HUINT16 *pusPmtPid);

// ======== PMT ======== //

STATIC INLINE void svc_cas_SiGetFirstProgDescriptorPos(HUINT8 *pucSection, HUINT8 **ppFirstProgDes);
STATIC INLINE void svc_cas_SiGetNextProgDescriptorPos(HUINT8 *pucProgDes, HUINT8 **ppNextProgDes, HUINT32 ulDesLen);
STATIC INLINE void svc_cas_SiGetFirstProgElemInfoPos(HUINT8 *pucSection, HUINT8 **ppFirstElement, HINT32 nProgDesLen);
STATIC INLINE void svc_cas_SiGetNextProgElemInfoPos(HUINT8 *pucElement, HUINT8 **ppNextElement, HUINT32 ulElemDesLen);
STATIC INLINE void svc_cas_SiGetFirstProgElemDesPos(HUINT8 *pucElement, HUINT8 **ppFirstElemDes);
STATIC INLINE void svc_cas_SiGetNextProgElemDesPos(HUINT8 *pucElemDes, HUINT8 **ppNextElemDes, HUINT32 ulElemDesLen);
STATIC HUINT8 *svc_cas_SiFindPmtSectionTable(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulIdx);
STATIC HERROR svc_cas_SiCountPmtSectionInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 *pulSecCnt);
STATIC HERROR svc_cas_SiGetPmtProgramInfoFromRawInternal(HUINT8 *pucRaw, svcCas_SiPmtProgramInfo_t *pstInfo);
STATIC HERROR svc_cas_SiGetPmtProgramInfoInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulIdx, svcCas_SiPmtProgramInfo_t *pstInfo);
STATIC HERROR svc_cas_SiCountPmtProgramDescriptorFromRawInternal(HUINT8 *pucRaw, HUINT32 ulTag, HUINT32 *pulDesCnt);
STATIC HERROR svc_cas_SiCountPmtProgramDescriptorInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 ulTag, HUINT32 *pnCnt);
STATIC HERROR svc_cas_SiGetPmtProgramDescriptorFromRawInternal(HUINT8 *pucRaw, SIxTextEncoding_e eTextEnc, HUINT32 ulTag, HUINT32 ulDesIdx, void **ppvDes);
STATIC HERROR svc_cas_SiGetPmtProgramDescriptorInternal(svcCas_RawPool_t *pstRawPool, SIxTextEncoding_e eTextEnc, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 ulTag, HUINT32 nDesIdx, void **ppvDes);
STATIC HERROR svc_cas_SiGetPmtProgramDesPtrFromRaw(HUINT8 *pucRaw, HUINT32 ulTag, HUINT32 ulDesIdx, HUINT32 *pulDesRawSize, HUINT8 **ppucDesRaw);
STATIC HERROR svc_cas_SiGetPmtProgramDesPtr(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 ulTag, HUINT32 ulDesIdx, HUINT32 *pulDesRawSize, HUINT8 **ppucDesRaw);
STATIC HERROR svc_cas_SiCountPmtElementFromRawInternal(HUINT8 *pucRaw, HUINT32 *pulElemCnt);
STATIC HERROR svc_cas_SiCountPmtElementInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 *pnCnt);
STATIC HERROR svc_cas_SiGetPmtElementInfoFromRawInternal(HUINT8 *pucRaw, HUINT32 ulElemIdx, svcCas_SiPmtElementInfo_t *pstInfo);
STATIC HERROR svc_cas_SiGetPmtElementInfoInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 nSubIdx, svcCas_SiPmtElementInfo_t *pstInfo);
STATIC HERROR svc_cas_SiCountPmtElementDescriptorFromRawInternal(HUINT8 *pucRaw, HUINT32 ulElemIdx, HUINT32 ulTag, HUINT32 *pulDesCnt);
STATIC HERROR svc_cas_SiCountPmtElementDescriptorInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 nSubIdx, HUINT32 ulTag, HUINT32 *pnDesCnt);
STATIC HERROR svc_cas_SiGetPmtElementDescriptorFromRawInternal(HUINT8 *pucRaw, SIxTextEncoding_e eTextEnc, HUINT32 ulElemIdx, HUINT32 ulTag, HUINT32 ulDesIdx, void **ppvDes);
STATIC HERROR svc_cas_SiGetPmtElementDescriptorInternal(svcCas_RawPool_t *pstRawPool, SIxTextEncoding_e eTextEnc, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 nSubIdx, HUINT32 ulTag, HUINT32 nDesIdx, void **ppvDes);
STATIC HERROR svc_cas_SiGetPmtElementDesPtrFromRaw(HUINT8 *pucRaw, HUINT32 ulElemIdx, HUINT32 ulTag, HUINT32 ulDesIdx, HUINT32 *pulDesRawSize, HUINT8 **ppucDesRaw);
STATIC HERROR svc_cas_SiGetPmtElementDesPtr(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 nSubIdx, HUINT32 ulTag, HUINT32 nDesIdx, HUINT32 *pulDesRawSize, HUINT8 **ppucDesRaw);

// ======== PMT SVC ID ======== //

STATIC HERROR svc_cas_SiGetPmtProgNumFromRawInternal(HUINT8 *pucRaw, HUINT16 *pusProgNum);
STATIC HERROR svc_cas_SiGetPmtIndexByServiceIdInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT16 usSvcId, HUINT32 *pulIdx);

// ======== SDT ======== //

STATIC INLINE void svc_cas_SiGetFirstServicePos(HUINT8 *pucSection, HUINT8 **ppFirstService);
STATIC INLINE void  svc_cas_SiGetNextServicePos(HUINT8 *pucService, HUINT8 **ppNextService, HUINT32 ulDesLooplenth);
STATIC INLINE void svc_cas_SiGetFirstSdtDescriptorPos(HUINT8 *pucService, HUINT8 **ppFirstSdtDescriptor);
STATIC INLINE void svc_cas_SiGetNextSdtDescriptorPos(HUINT8 *pucSdtDescriptor, HUINT8 **ppNextSdtDescriptor, HUINT32 ulDesLenth);

STATIC HUINT8 *svc_cas_SiFindSdtSectionTable(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulHead, HUINT32 ulIdx);
#if 0
STATIC HERROR svc_cas_SiCountSdtSectionInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulHead, HUINT32 *pulCnt);
#endif
STATIC HERROR svc_cas_SiGetSdtSectionInfoFromRawInternal(HUINT8 *pucRaw, svcCas_SiSdtSectionInfo_t *pstInfo);
STATIC HERROR svc_cas_SiGetSdtSectionInfoInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulHead, HUINT32 ulIdx, svcCas_SiSdtSectionInfo_t *pstInfo);
STATIC HERROR svc_cas_SiCountSdtServiceFromRawInternal(HUINT8 *pucRaw, HUINT32 *pulCnt);
STATIC HERROR svc_cas_SiCountSdtServiceInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulHead, HUINT32 ulIdx, HUINT32 *pulCnt);
STATIC HERROR svc_cas_SiGetSdtServiceInfoFromRawInternal(HUINT8 *pucRaw, HUINT32 ulSvcIdx, svcCas_SiSdtServiceInfo_t *pstInfo);
STATIC HERROR svc_cas_SiGetSdtServiceInfoInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulHead, HUINT32 ulIdx, HUINT32 ulSubIdx, svcCas_SiSdtServiceInfo_t *pstInfo);
STATIC HERROR svc_cas_SiCountSdtServiceDescriptorFromRawInternal(HUINT8 *pucRaw, HUINT32 ulSvcIdx, HUINT32 ulTag, HUINT32 *pnDesCnt);
#if 0
STATIC HERROR svc_cas_SiCountSdtServiceDescriptorInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulHead, HUINT32 ulIdx, HUINT32 ulSubIdx, HUINT32 ulTag, HUINT32 *pnDesCnt);
#endif
STATIC HERROR svc_cas_SiGetSdtServiceDescriptorFromRawInternal(HUINT8 *pucRaw, SIxTextEncoding_e eTextEnc, HUINT32 ulSvcIdx, HUINT32 ulTag, HUINT32 ulDesIdx, void **ppvDes);
#if 0
STATIC HERROR svc_cas_SiGetSdtServiceDescriptorInternal(svcCas_RawPool_t *pstRawPool, SIxTextEncoding_e eTextEnc, HUINT16 usTsUniqId, HUINT32 ulHead, HUINT32 ulIdx, HUINT32 ulSubIdx, HUINT32 ulTag, HUINT32 nDesIdx, void **ppvDes);
#endif
STATIC HERROR svc_cas_SiGetSdtServiceDesPtrFromRaw(HUINT8 *pucRaw, HUINT32 ulSvcIdx, HUINT32 ulTag, HUINT32 ulDesIdx, HUINT32 *pulDesRawSize, HUINT8 **ppucDesRaw);
STATIC HERROR svc_cas_SiGetSdtServiceDesPtr(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulHead, HUINT32 ulIdx, HUINT32 ulSubIdx, HUINT32 ulTag, HUINT32 nDesIdx, HUINT32 *pulDesRawSize, HUINT8 **ppucDesRaw);

// ======== Common Relative ======== //

STATIC INLINE HBOOL svc_cas_SiIsSameDesTag(HUINT32 ulSecDesTag, HUINT32 ulArgDesTag);
STATIC HERROR svc_cas_SiGetDesTagFromDesIndexInternal(svcCas_SiDesIndex_t *pstDesIndex, HUINT32 *pulTag);
STATIC HERROR svc_cas_SiGetDesFromDesIndexInternal(SIxTextEncoding_e eTextEnc, svcCas_SiDesIndex_t *pstDesIndex, HUINT32 ulTag, void **ppvDes, svcCas_SiDesIndex_t *pstNextDesIndex);
STATIC HERROR svc_cas_SiGetDesRawFromDesIndexInternal(svcCas_SiDesIndex_t *pstDesIndex, HUINT32 ulTag, HUINT32 *pulDesRawSize, HUINT8 **ppucDesRaw, svcCas_SiDesIndex_t *pstNextDesIndex);
STATIC HERROR svc_cas_SiSetDesIndexToFirstDesInternal(svcCas_SiDesIndex_t *pstCurrDesIndex,
												HUINT32 ulTag,
												svcCas_SiDesIndex_t *pstNextDesIndex);
STATIC HERROR svc_cas_SiSetDesIndexToNextDesInternal(svcCas_SiDesIndex_t *pstCurrDesIndex,
												HUINT32 ulTag,
												svcCas_SiDesIndex_t *pstNextDesIndex);

// ======== PMT Relative ======== //
#if 0 // Not Used
STATIC HERROR svc_cas_SiSetDesIndexToPmtProgDescInternal(HUINT8 *pucRaw, svcCas_SiDesIndex_t *pstDesIndex);
STATIC HERROR svc_cas_SiSetInfoIndexToPmtElementInfoInternal(HUINT8 *pucRaw, svcCas_SiInfoIndex_t *pstInfoIndex);
STATIC HERROR svc_cas_SiSetInfoIndexToNextPmtElementInfoInternal(svcCas_SiInfoIndex_t *pstCurrIndex, svcCas_SiInfoIndex_t *pstNextIndex);
STATIC HERROR svc_cas_SiGetPmtElementInfoFromInfoIndexInternal(svcCas_SiInfoIndex_t *pstInfoIndex,
																	svcCas_SiPmtElementInfo_t *pstInfo,
																	svcCas_SiInfoIndex_t *pstNextIndex);
STATIC HERROR svc_cas_SiSetDesIndexToPmtElementDesInternal(svcCas_SiInfoIndex_t *pstInfoIndex, svcCas_SiDesIndex_t *pstDesIndex);
#endif

////////////////////////////////////////////
#define _____________________________________
#define _________BASIC_FUNCTION___________
#define _____________________________________

STATIC HERROR svc_cas_SiDuplicateDesRaw (HUINT32 ulDesRawSize, HUINT8 *pucDesRaw, HUINT32 *pulDesRawSize, HUINT8 **ppucDesRaw)
{
	HUINT8		*pucDstRaw;

	if (pulDesRawSize != NULL)
	{
		*pulDesRawSize = ulDesRawSize;
	}

	if (ppucDesRaw != NULL)
	{
		pucDstRaw =(HUINT8 *)OxCAS_Malloc (ulDesRawSize);
		if (pucDstRaw == NULL)
		{
			return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
		}

		HxSTD_memcpy (pucDstRaw, pucDesRaw, ulDesRawSize);
		*ppucDesRaw = pucDstRaw;
	}

	return ERR_CAS_SI_RESULT_OK;
}

void *svc_cas_SiParseDescriptor(SIxTextEncoding_e eTextEnc, HUINT8 *pucRaw)
{
	HUINT8		*pucDes;
	HERROR		 hErr;

	if (pucRaw != NULL)
	{
		pucDes = svc_cas_SiAllocDes(pucRaw[0], pucRaw[1]);
		if (pucDes != NULL)
		{
			hErr = svc_cas_SiParseDes(eTextEnc, pucRaw, pucDes);
			if (hErr == ERR_CAS_SI_RESULT_OK)
			{
				return (void *)pucDes;
			}

			svc_cas_SiFreeDes(pucDes);
		}
	}

	return NULL;
}

HERROR svc_cas_SiFreeDescriptor(void *pvDes)
{
	svc_cas_SiFreeDes(pvDes);
	return ERR_CAS_SI_RESULT_OK;
}

HERROR svc_cas_SiFreeDescriptorRaw(HUINT8 *pucDesRaw)
{
	if (pucDesRaw == NULL)			{ return ERR_CAS_SI_SOURCE_NULL; }
	OxCAS_Free (pucDesRaw);
	return ERR_CAS_SI_RESULT_OK;
}

HERROR svc_cas_SiGetDescriptorTag (void *pvDes, HUINT32 *pulDesTag)
{
	svcCas_BaseDesForm_t	*pstBaseDes;

	if (pvDes == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulDesTag == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	pstBaseDes = (svcCas_BaseDesForm_t *)pvDes;
	*pulDesTag = (HUINT32)pstBaseDes->ucDesTag;


	return ERR_CAS_SI_RESULT_OK;
}

HBOOL svc_cas_SiIsUserDefinedDescriptor (HUINT32 ulTag)
{
	return MACRO_IS_USER_DEFINED_DESTAG(ulTag);
}

#define ________________CAT_________________

STATIC INLINE void svc_cas_SiGetFirstCatDescriptorPos(HUINT8 *pucSection, HUINT8 **ppFirstCatDes)
{
	*ppFirstCatDes = pucSection + 8;
}

STATIC INLINE void svc_cas_SiGetNextCatDescriptorPos(HUINT8 *pucCatDes, HUINT8 **ppNextCatDes, HUINT32 ulDesLen)
{
	*ppNextCatDes = pucCatDes + 2 + (HUINT32)ulDesLen;
}

STATIC HERROR svc_cas_SiCountCatSectionInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 *pulCnt)
{
	HUINT16			 usCount;
	HUINT16			 usAllUniqueId;
	HUINT32			 ulSecCnt;
	svcCas_RawSecTable_t	*pstTable;

	if (pstRawPool == NULL)		{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulCnt == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	usAllUniqueId = DB_SVC_GetAllUniqueId();
	pstTable = svc_cas_RawGetRawTable (pstRawPool, eSI_TableType_CAT);

	ulSecCnt = 0;
	for (usCount = pstTable->usStartPos; usCount < pstTable->usInsertPos; usCount++)
	{
		if (pstTable->phIdx[usCount] == INVALID_INDEX_HANDLE)			{ continue; }

		if (usTsUniqId == usAllUniqueId || usTsUniqId == MACRO_GET_UNIQUEID_FROM_IDXHANDLE(pstTable->phIdx[usCount]))
		{
			ulSecCnt++;
		}
	}

	*pulCnt = ulSecCnt;
	HxLOG_Print("Pat section count = %d\n", ulSecCnt);

	return ERR_CAS_SI_RESULT_OK;
}

STATIC HERROR svc_cas_SiCountCatDescriptorInternal(svcCas_RawPool_t *pstRawPool,
															HUINT16 usTsUniqId,
															HUINT32 ulIdx,
															HUINT32 ulTag,
															HUINT32 *pnCnt)
{
	HUINT8		*pucRaw;

	if (pstRawPool == NULL)			{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pnCnt == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindCatSectionTable(pstRawPool, usTsUniqId, ulIdx);
	return svc_cas_SiCountCatDescriptorFromRaw(pucRaw, ulTag, pnCnt);
}

STATIC HERROR svc_cas_SiGetCatDescriptorTagInternal(svcCas_RawPool_t *pstRawPool,
															HUINT16 usTsUniqId,
															HUINT32 ulIdx,
															HUINT32 nDesIdx,
															HUINT8	*pucTag)
{
	HUINT8		*pucRaw;

	if (pstRawPool == NULL)			{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pucTag == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindCatSectionTable(pstRawPool, usTsUniqId, ulIdx);
	return svc_cas_SiGetCatDescriptorTagFromRaw(pucRaw, nDesIdx, pucTag);
}

STATIC HERROR svc_cas_SiGetCatDescriptorInternal(svcCas_RawPool_t *pstRawPool,
															SIxTextEncoding_e eTextEnc,
															HUINT16 usTsUniqId,
															HUINT32 ulIdx,
															HUINT32 ulTag,
															HUINT32 nDesIdx,
															void **ppvDes)
{
	HUINT8		*pucRaw;

	if (pstRawPool == NULL)			{ return ERR_CAS_SI_SOURCE_NULL; }
	if (ppvDes == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindCatSectionTable(pstRawPool, usTsUniqId, ulIdx);
	return svc_cas_SiGetCatDescriptorFromRaw(pucRaw, eTextEnc, ulTag, nDesIdx, ppvDes);
}

STATIC HERROR svc_cas_SiGetCatDescriptorRawInternal(svcCas_RawPool_t *pstRawPool,
															HUINT16 usTsUniqId,
															HUINT32 ulIdx,
															HUINT32 ulTag,
															HUINT32 nDesIdx,
															HUINT32 *pulDesRawSize,
															HUINT8 **ppucDesRaw)
{
	HUINT8		*pucRaw;

	if (pstRawPool == NULL)						{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulDesRawSize == NULL || ppucDesRaw)	{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindCatSectionTable (pstRawPool, usTsUniqId, ulIdx);
	return svc_cas_SiGetCatDescriptorRawFromRaw (pucRaw, ulTag, nDesIdx, pulDesRawSize, ppucDesRaw);
}

STATIC HUINT8 *svc_cas_SiFindCatSectionTable(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulIdx)
{
	HUINT16			 usCount, usAllUniqueId;
	HUINT16			 usRawIndex;
	HUINT32			 ulCatSecCnt;
	HUINT8			*pucRaw;
	svcCas_RawSecTable_t	*pstTable;

	if (pstRawPool == NULL)		{ return NULL; }

	usAllUniqueId = DB_SVC_GetAllUniqueId();
	pstTable = svc_cas_RawGetRawTable (pstRawPool, eSI_TableType_CAT);
	if (pstTable == NULL)		{ return NULL; }

	ulCatSecCnt = 0;
	for (usCount = pstTable->usStartPos; usCount < pstTable->usInsertPos; usCount++)
	{
		if (pstTable->phIdx[usCount] == INVALID_INDEX_HANDLE)				{ continue; }

		// If there is no match in the unique ID, then it shall be skipped.
		if ((usTsUniqId != usAllUniqueId)
			&& (usTsUniqId != MACRO_GET_UNIQUEID_FROM_IDXHANDLE(pstTable->phIdx[usCount])))				{ continue; }

		if (ulCatSecCnt >= ulIdx)
		{
			usRawIndex = MACRO_GET_INDEX_FROM_IDXHANDLE(pstTable->phIdx[usCount]);
			pucRaw = svc_cas_RawGetRawSection (pstRawPool, (HUINT32)usRawIndex);
			if (pucRaw != NULL)
			{
				return pucRaw;
			}
		}
		else
		{
			ulCatSecCnt++;
		}
	}

	return NULL;
}

STATIC HERROR svc_cas_SiCountCatDescriptorFromRaw(HUINT8 *pucRaw, HUINT32 ulTag, HUINT32 *pulDesCnt)
{
	HUINT8		 ucDesTag, ucDesLen;
	HUINT32		 ulDesCnt;
	HINT32		 nSecLen;
	HUINT8		*pucCatDes;

	if (pucRaw == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulDesCnt == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	ulDesCnt = 0;

	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (9 + 4); 									// program info descriptor 바로 앞까지의 table fileds과 crc를 제거.

	svc_cas_SiGetFirstCatDescriptorPos (pucRaw, &pucCatDes);

	while ( (nSecLen > 0) && (nSecLen <MAX_SECTION_LIMIT) )
	{
		ucDesTag = HxMACRO_Get8Bit(pucCatDes);
		ucDesLen = HxMACRO_Get8Bit(pucCatDes + 1);

		nSecLen -= (2 + ucDesLen);

		if (svc_cas_SiIsSameDesTag ((HUINT32)ucDesTag, ulTag) == TRUE)
		{
			ulDesCnt++;
		}

		svc_cas_SiGetNextCatDescriptorPos (pucCatDes, &pucCatDes, (HUINT32)ucDesLen);
	}

	*pulDesCnt = ulDesCnt;
	HxLOG_Print("CAT descriptor count = %d\n",ulDesCnt);

	return ERR_CAS_SI_RESULT_OK;
}

STATIC HERROR svc_cas_SiGetCatDescriptorTagFromRaw(HUINT8 *pucRaw,
																	HUINT32 ulDesIdx,
																	HUINT8 *pucTag)
{
	HUINT8		 ucDesTag, ucDesLen;
	HUINT32		 ulDesCnt;
	HINT32		 nSecLen;
	HUINT8		*pucCatDes;

	if (pucRaw == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pucTag == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	ulDesCnt = 0;

	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (7 + 4); 									// network descriptor length까지 나머지 table fields와  CRC 제거.

	svc_cas_SiGetFirstCatDescriptorPos(pucRaw, &pucCatDes);
	while ( (nSecLen > 0) && (nSecLen <MAX_SECTION_LIMIT ) )
	{
		ucDesTag = HxMACRO_Get8Bit(pucCatDes);
		ucDesLen = HxMACRO_Get8Bit(pucCatDes + 1);

		nSecLen -= (2 + ucDesLen);

		if (ulDesCnt != ulDesIdx)		// Different Index
		{
			ulDesCnt++;
			svc_cas_SiGetNextCatDescriptorPos (pucCatDes, &pucCatDes, (HUINT32)ucDesLen);
			continue;
		}

		*pucTag = ucDesTag;
		return ERR_CAS_SI_RESULT_OK;
	}

	return ERR_CAS_SI_RESULT_FAIL;
}

STATIC HERROR svc_cas_SiGetCatDescriptorFromRaw(HUINT8 *pucRaw,
												SIxTextEncoding_e eTextEnc,
												HUINT32 ulTag,
												HUINT32 ulDesIdx,
												void **ppvDes)
{
	HUINT8		 ucDesTag, ucDesLen;
	HUINT32		 ulDesCnt;
	HINT32		 nSecLen;
	HUINT8		*pucCatDes;
	HUINT8		*pucDes;
	HERROR		 hErr;

	if (pucRaw == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (ppvDes == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	ulDesCnt = 0;

	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (7 + 4); 									// network descriptor length까지 나머지 table fields와  CRC 제거.

	svc_cas_SiGetFirstCatDescriptorPos(pucRaw, &pucCatDes);
	while ( (nSecLen > 0) && (nSecLen <MAX_SECTION_LIMIT ) )
	{
		ucDesTag = HxMACRO_Get8Bit(pucCatDes);
		ucDesLen = HxMACRO_Get8Bit(pucCatDes + 1);

		nSecLen -= (2 + ucDesLen);

		if (svc_cas_SiIsSameDesTag ((HUINT32)ucDesTag, ulTag) != TRUE)		// Different TAG
		{
			svc_cas_SiGetNextCatDescriptorPos (pucCatDes, &pucCatDes, (HUINT32)ucDesLen);
			continue;
		}

		if (ulDesCnt != ulDesIdx)		// Different Index
		{
			ulDesCnt++;
			svc_cas_SiGetNextCatDescriptorPos (pucCatDes, &pucCatDes, (HUINT32)ucDesLen);
			continue;
		}

		HxLOG_Print("CAT Descriptor Tag = 0x%x\n",ucDesTag);
		pucDes = svc_cas_SiAllocDes(ucDesTag, ucDesLen);
		if (pucDes == NULL) 						{ return ERR_CAS_SI_ALLOC_DESCRIPTOR_FAIL; }
		hErr = svc_cas_SiParseDes(eTextEnc, pucCatDes, pucDes);
		if (hErr != ERR_CAS_SI_RESULT_OK)
		{
			svc_cas_SiFreeDes(pucDes);
			if (hErr == ERR_CAS_SI_INVALID_TAG || hErr == ERR_CAS_SI_INVALID_DESCRIPTOR)
			{
				svc_cas_SiGetNextCatDescriptorPos (pucCatDes, &pucCatDes, (HUINT32)ucDesLen);
				continue;
			}
			else
			{
				return ERR_CAS_SI_CANNOT_FIND_DESCRIPTOR;
			}
		}
		*ppvDes = (void *)pucDes;
		return ERR_CAS_SI_RESULT_OK;
	}

	return ERR_CAS_SI_RESULT_FAIL;
}

STATIC HERROR svc_cas_SiGetCatDescriptorRawFromRaw(HUINT8 *pucRaw,
														HUINT32 ulTag,
														HUINT32 ulDesIdx,
														HUINT32 *pulDesRawSize,
														HUINT8 **ppucDesRaw)
{
	HUINT8		 ucDesTag, ucDesLen;
	HUINT32		 ulDesCnt;
	HUINT32		 ulRawSize;
	HINT32		 nSecLen;
	HUINT8		*pucCatDes;
	HUINT8		*pucDes;

	if (pucRaw == NULL)										{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulDesRawSize == NULL || ppucDesRaw == NULL)		{ return ERR_CAS_SI_TARGET_NULL; }

	ulDesCnt = 0;

	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (7 + 4); 									// network descriptor length까지 나머지 table fields와  CRC 제거.

	svc_cas_SiGetFirstCatDescriptorPos(pucRaw, &pucCatDes);
	while ( (nSecLen > 0) && (nSecLen <MAX_SECTION_LIMIT ) )
	{
		ucDesTag = HxMACRO_Get8Bit(pucCatDes);
		ucDesLen = HxMACRO_Get8Bit(pucCatDes + 1);

		nSecLen -= (2 + ucDesLen);

		if (svc_cas_SiIsSameDesTag ((HUINT32)ucDesTag, ulTag) != TRUE)		// Different TAG
		{
			svc_cas_SiGetNextCatDescriptorPos (pucCatDes, &pucCatDes, (HUINT32)ucDesLen);
			continue;
		}

		if (ulDesCnt != ulDesIdx)		// Different Index
		{
			ulDesCnt++;
			svc_cas_SiGetNextCatDescriptorPos (pucCatDes, &pucCatDes, (HUINT32)ucDesLen);
			continue;
		}

		HxLOG_Print("CAT Descriptor Tag = 0x%x\n",ucDesTag);

		ulRawSize = (HUINT32)ucDesLen + 2;
		pucDes =(HUINT8 *)OxCAS_Malloc (ulRawSize);
		if (pucDes == NULL)
		{
			return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
		}

		HxSTD_memcpy (pucDes, pucCatDes, ulRawSize);

		*pulDesRawSize	= ulRawSize;
		*ppucDesRaw 	= pucDes;
		return ERR_CAS_SI_RESULT_OK;
	}

	return ERR_CAS_SI_RESULT_FAIL;
}

HERROR svc_cas_SiCountCatSection(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 *pulCnt)
{
	HUINT32 	ulRet;

	HxLOG_Info("[svc_cas_SiCountCatSection] (+) TsUnique Id = 0x%x\n", usTsUniqId);
	ulRet = svc_cas_SiCountCatSectionInternal (pstRawPool, usTsUniqId, pulCnt);
	HxLOG_Info("[svc_cas_SiCountCatSection] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiCountCatDescriptor(svcCas_RawPool_t *pstRawPool,
										HUINT16 usTsUniqId,
										HUINT32 ulIdx,
										HUINT32 ulTag,
										HUINT32 *pnCnt)
{
	HUINT32 	ulRet;

	HxLOG_Info("[svc_cas_SiCountCatDescriptor] (+) TsUnique Id = 0x%x\n", usTsUniqId);
	ulRet = svc_cas_SiCountCatDescriptorInternal (pstRawPool, usTsUniqId, ulIdx, ulTag, pnCnt);
	HxLOG_Info("[svc_cas_SiCountCatDescriptor] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiGetCatDescriptorTag(svcCas_RawPool_t *pstRawPool,
										HUINT16 usTsUniqId,
										HUINT32 ulIdx,
										HUINT32 nDesIdx,
										HUINT8 *pucTag)
{
	HUINT32 	ulRet;

	HxLOG_Info("[svc_cas_SiGetCatDescriptorTag] (+) TsUnique Id = 0x%x\n", usTsUniqId);
	ulRet = svc_cas_SiGetCatDescriptorTagInternal(pstRawPool, usTsUniqId, ulIdx, nDesIdx, pucTag);
	HxLOG_Info("[svc_cas_SiGetCatDescriptorTag] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiGetCatDescriptor(svcCas_RawPool_t *pstRawPool,
									SIxTextEncoding_e eTextEnc,
									HUINT16 usTsUniqId,
									HUINT32 ulIdx,
									HUINT32 ulTag,
									HUINT32 nDesIdx,
									void **ppvDes)
{
	HUINT32 	ulRet;

	HxLOG_Info("[svc_cas_SiGetCatDescriptor] (+) TsUnique Id = 0x%x\n", usTsUniqId);
	ulRet = svc_cas_SiGetCatDescriptorInternal(pstRawPool, eTextEnc, usTsUniqId, ulIdx, ulTag, nDesIdx, ppvDes );
	HxLOG_Info("[svc_cas_SiGetCatDescriptor] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiGetCatDescriptorRaw(svcCas_RawPool_t *pstRawPool,
										HUINT16 usTsUniqId,
										HUINT32 ulIdx,
										HUINT32 ulTag,
										HUINT32 nDesIdx,
										HUINT32 *pulDesRawSize,
										HUINT8 **ppucDesRaw)
{
	HERROR		 hRet;

	HxLOG_Info("[svc_cas_SiGetCatDescriptorRaw] (+) TsUnique Id = 0x%x\n", usTsUniqId);
	hRet = svc_cas_SiGetCatDescriptorRawInternal(pstRawPool, usTsUniqId, ulIdx, ulTag, nDesIdx, pulDesRawSize, ppucDesRaw);
	HxLOG_Info("[svc_cas_SiGetCatDescriptorRaw] (-)\n");

	return hRet;
}



#define ________________PAT_________________
STATIC INLINE void svc_cas_SiGetFirstProgramInfoPos(HUINT8 *pucSection, HUINT8 **ppFirstProg)
{
	*ppFirstProg = pucSection + 8;
}

STATIC INLINE void svc_cas_SiGetNextProgramInfoPos(HUINT8 *pucProg, HUINT8 **ppNextProg)
{
	*ppNextProg = pucProg + 4;
}

STATIC HUINT8 *svc_cas_SiFindPatSectinTable(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulIdx)
{
	HUINT16			 usCount, usSecIndex;
	HUINT16			 usAllUniqueId;
	HUINT32			 ulSecCnt;
	HUINT8			*pucRaw;
	svcCas_RawSecTable_t	*pstTable;

	if (pstRawPool == NULL)			return NULL;

	usAllUniqueId = DB_SVC_GetAllUniqueId();
	pstTable = svc_cas_RawGetRawTable (pstRawPool, eSI_TableType_PAT);

	ulSecCnt = 0;

	for (usCount = pstTable->usStartPos; usCount < pstTable->usInsertPos; usCount++)
	{
		if (pstTable->phIdx[usCount] == INVALID_INDEX_HANDLE)			{ continue; }

		if (usTsUniqId == usAllUniqueId || usTsUniqId == MACRO_GET_UNIQUEID_FROM_IDXHANDLE(pstTable->phIdx[usCount]))
		{
			if (ulSecCnt == ulIdx)
			{
				usSecIndex = MACRO_GET_INDEX_FROM_IDXHANDLE(pstTable->phIdx[usCount]);
				pucRaw = svc_cas_RawGetRawSection (pstRawPool, (HUINT32)usSecIndex);
				if (pucRaw != NULL)
				{
					return pucRaw;
				}
			}
			else
			{
				ulSecCnt++;
			}
		}
	}


	// elsewhere, return NULL;
	return NULL;
}

STATIC HERROR svc_cas_SiCountPatSectionInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 *pulCnt)
{
	HUINT16			 usCount;
	HUINT16			 usAllUniqueId;
	HUINT32			 ulSecCnt;
	svcCas_RawSecTable_t	*pstTable;

	if (pstRawPool == NULL)		{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulCnt == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	usAllUniqueId = DB_SVC_GetAllUniqueId();
	pstTable = svc_cas_RawGetRawTable (pstRawPool, eSI_TableType_PAT);

	ulSecCnt = 0;
	for (usCount = pstTable->usStartPos; usCount < pstTable->usInsertPos; usCount++)
	{
		if (pstTable->phIdx[usCount] == INVALID_INDEX_HANDLE)			{ continue; }

		if (usTsUniqId == usAllUniqueId || usTsUniqId == MACRO_GET_UNIQUEID_FROM_IDXHANDLE(pstTable->phIdx[usCount]))
		{
			ulSecCnt++;
		}
	}

	*pulCnt = ulSecCnt;
	HxLOG_Print("Pat section count = %d\n", ulSecCnt);

	return ERR_CAS_SI_RESULT_OK;
}

STATIC HERROR svc_cas_SiGetPatSectionFromRawInternal(HUINT8 *pucRaw, svcCas_SiPatSectionInfo_t *pstPatInfo)
{
	HUINT32			 ulSecLen;

	if (pucRaw == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pstPatInfo == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	if (pucRaw[0] != PAT_TABLE_ID)		{ return ERR_CAS_SI_INVALID_TAG; }

	HxSTD_memset (pstPatInfo, 0, sizeof(svcCas_SiPatSectionInfo_t));
	ulSecLen = (HUINT32)HxMACRO_Get12Bit(pucRaw + 1);

	pstPatInfo->usTsId	= HxMACRO_Get16Bit(pucRaw + 3);
	pstPatInfo->ucVer	= (pucRaw[5] & 0x3e) >> 1;
	pstPatInfo->ulCrc32	= HxMACRO_Get32Bit(pucRaw + ulSecLen - 1);		// +3 - 4 = -1: Position of the CRC

	HxLOG_Print("Pat TsId = 0x%x\n",	pstPatInfo->usTsId);
	HxLOG_Print("Pat ucVer = 0x%x\n",	pstPatInfo->ucVer);
	HxLOG_Print("Pat ulCrc = 0x%x\n",	pstPatInfo->ulCrc32);

	return ERR_CAS_SI_RESULT_OK;
}

STATIC HERROR svc_cas_SiGetPatSectionInternal(svcCas_RawPool_t *pstRawPool,
											HUINT16 usTsUniqId,
											HUINT32 ulIdx,
											svcCas_SiPatSectionInfo_t *pstPatInfo)
{
	HUINT8			*pucRaw;

	if (pstRawPool == NULL)		{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pstPatInfo == NULL)		{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindPatSectinTable (pstRawPool, usTsUniqId, ulIdx);
	return svc_cas_SiGetPatSectionFromRawInternal(pucRaw, pstPatInfo);
}

STATIC HERROR svc_cas_SiCountPatProgramFromRawInternal(HUINT8 *pucRaw, HUINT32 *pulCnt)
{
	HUINT8		*pucProgSection;
	HINT32		 nSecLen, nPatProgCnt;


	if (pucRaw == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulCnt == NULL)					{ return ERR_CAS_SI_TARGET_NULL; }

	if (pucRaw[0] != PAT_TABLE_ID)		{ return ERR_CAS_SI_INVALID_TAG; }

	*pulCnt = 0;
	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (5 + 4);					//  나머지 table fields와  CRC 제거.
	nPatProgCnt = 0;
	svc_cas_SiGetFirstProgramInfoPos (pucRaw, &pucProgSection);
	while (nSecLen > 0)
	{
		//  if program number == 0, that indicate NIT PID.
		if (HxMACRO_Get16Bit(pucProgSection) != 0)
		{
			nPatProgCnt++;
		}

		//  program Info 내용은 4byte
		nSecLen -= 4;
		svc_cas_SiGetNextProgramInfoPos (pucProgSection, &pucProgSection);
	}

	*pulCnt = nPatProgCnt;
	HxLOG_Print("Pat Program count = %d\n",nPatProgCnt);

	return ERR_CAS_SI_RESULT_OK;
}

STATIC HERROR svc_cas_SiCountPatProgramInternal(svcCas_RawPool_t *pstRawPool,
												HUINT16 usTsUniqId,
												HUINT32 ulIdx,
												HUINT32 *pulCnt)
{
	HUINT8		*pucRaw;

	if (pstRawPool == NULL)		{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulCnt == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindPatSectinTable (pstRawPool, usTsUniqId, ulIdx);
	return svc_cas_SiCountPatProgramFromRawInternal(pucRaw, pulCnt);
}

STATIC HERROR svc_cas_SiGetPatProgramInfoFromRawInternal(HUINT8 *pucRaw,
															HUINT32 ulInfoIdx,
															svcCas_SiPatProgInfo_t *pstInfo)
{
	HUINT8		*pucProgSection;
	HUINT32		 ulPatProgCnt;
	HINT32		 nSecLen;


	if (pucRaw == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pstInfo == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	if (pucRaw[0] != PAT_TABLE_ID)		{ return ERR_CAS_SI_INVALID_TAG; }

	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (5 + 4);					//  나머지 table fields와  CRC 제거.
	ulPatProgCnt = 0;
	svc_cas_SiGetFirstProgramInfoPos (pucRaw, &pucProgSection);

	while (nSecLen > 0)
	{
		if (nSecLen < 4)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_CAS_SI_RESULT_FAIL;
		}

		if (ulInfoIdx == ulPatProgCnt && HxMACRO_Get16Bit(pucProgSection) != 0)
		{
			HxSTD_memset(pstInfo, 0, sizeof(svcCas_SiPatProgInfo_t));

			pstInfo->usProgNum = HxMACRO_Get16Bit(pucProgSection);
			pstInfo->usProgMapPid = HxMACRO_Get13Bit(pucProgSection + 2);

			HxLOG_Print("Program num = 0x%x, Program Map Pid = 0x%x\n", pstInfo->usProgNum,pstInfo->usProgMapPid);

			return ERR_CAS_SI_RESULT_OK;
		}

		//  if program number == 0, that indicate NIT PID.
		if ( HxMACRO_Get16Bit(pucProgSection) != 0 )
		{
			ulPatProgCnt++;
		}

		//  program Info 내용은 4byte
		nSecLen -= 4;
		svc_cas_SiGetNextProgramInfoPos(pucProgSection, &pucProgSection);
	}

	return ERR_CAS_SI_RESULT_FAIL;
}

STATIC HERROR svc_cas_SiGetPatProgramInfoInternal(svcCas_RawPool_t *pstRawPool,
												HUINT16 usTsUniqId,
												HUINT32 ulIdx,
												HUINT32 ulSubIdx,
												svcCas_SiPatProgInfo_t *pstInfo)
{
	HUINT8			*pucRaw;

	if (pstRawPool == NULL)		{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pstInfo == NULL)		{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindPatSectinTable (pstRawPool, usTsUniqId, ulIdx);
	return svc_cas_SiGetPatProgramInfoFromRawInternal(pucRaw, ulSubIdx, pstInfo);
}

HERROR svc_cas_SiCountPatSection(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 *pulCnt)
{
	HUINT32 	ulRet;

	HxLOG_Info("[svc_cas_SiCountPatSection] (+) TsUnique Id = 0x%x\n", usTsUniqId);
	ulRet = svc_cas_SiCountPatSectionInternal(pstRawPool, usTsUniqId, pulCnt);
	HxLOG_Info("[svc_cas_SiCountPatSection] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiGetPatSectionFromRaw(HUINT8 *pucRaw, svcCas_SiPatSectionInfo_t *pstInfo)
{
	HUINT32 	ulRet;

	HxLOG_Info("[svc_cas_SiGetPatSectionFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	ulRet = svc_cas_SiGetPatSectionFromRawInternal(pucRaw, pstInfo);
	HxLOG_Info("[svc_cas_SiGetPatSectionFromRaw] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiGetPatSection(svcCas_RawPool_t *pstRawPool,
									HUINT16 usTsUniqId,
									HUINT32 ulIdx,
									svcCas_SiPatSectionInfo_t *pstInfo)
{
	HUINT32 	ulRet;

	HxLOG_Info("[svc_cas_SiGetPatSection] (+) TsUnique Id = 0x%x\n", usTsUniqId);
	ulRet = svc_cas_SiGetPatSectionInternal(pstRawPool, usTsUniqId, ulIdx, pstInfo);
	HxLOG_Info("[svc_cas_SiGetPatSection] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiCountPatProgramFromRaw(HUINT8 *pucRaw, HUINT32 *pulCnt)
{
	HUINT32 	ulRet;

	HxLOG_Info("[svc_cas_SiCountPatProgramFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	ulRet = svc_cas_SiCountPatProgramFromRawInternal(pucRaw, pulCnt);
	HxLOG_Info("[svc_cas_SiCountPatProgramFromRaw] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiCountPatProgram(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 *pulCnt)
{
	HUINT32 	ulRet;

	HxLOG_Info("[svc_cas_SiCountPatProgram] (+) TsUnique Id = 0x%x\n", usTsUniqId);
	ulRet = svc_cas_SiCountPatProgramInternal(pstRawPool, usTsUniqId, ulIdx, pulCnt);
	HxLOG_Info("[svc_cas_SiCountPatProgram] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiGetPatProgramInfoFromRaw(HUINT8 *pucRaw, HUINT32 ulInfoIdx, svcCas_SiPatProgInfo_t *pstInfo)
{
	HUINT32 	ulRet;

	HxLOG_Info("[svc_cas_SiGetPatProgramInfoFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	ulRet = svc_cas_SiGetPatProgramInfoFromRawInternal(pucRaw, ulInfoIdx, pstInfo);
	HxLOG_Info("[svc_cas_SiGetPatProgramInfoFromRaw] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiGetPatProgramInfo(svcCas_RawPool_t *pstRawPool,
											HUINT16 usTsUniqId,
											HUINT32 ulIdx,
											HUINT32 ulSubIdx,
											svcCas_SiPatProgInfo_t *pstInfo)
{
	HUINT32 	ulRet;

	HxLOG_Info("[svc_cas_SiGetPatProgramInfo] (+) TsUnique Id = 0x%x\n", usTsUniqId);
	ulRet = svc_cas_SiGetPatProgramInfoInternal(pstRawPool, usTsUniqId, ulIdx, ulSubIdx, pstInfo);
	HxLOG_Info("[svc_cas_SiGetPatProgramInfo] (-)\n");

	return ulRet;
}

#define ___________PAT_SVCID____________

STATIC HERROR svc_cas_SiGetPmtPidByServiceIdFromRawInternal(HUINT8 *pucRaw, HUINT16 usSvcId, HUINT16 *pusPmtPid)
{
	HUINT32			 ulSvcIdx, ulSvcCnt;
	svcCas_SiPatProgInfo_t	 stInfo;
	HERROR			 hErr;

	if (pucRaw == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusPmtPid == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	if (pucRaw[0] != PAT_TABLE_ID)		{ return ERR_CAS_SI_INVALID_TAG; }

	hErr = svc_cas_SiCountPatProgramFromRawInternal(pucRaw, &ulSvcCnt);
	if (hErr != ERR_CAS_SI_RESULT_OK)					{ ulSvcCnt = 0; }

	for (ulSvcIdx = 0; ulSvcIdx < ulSvcCnt; ulSvcIdx++)
	{
		hErr = svc_cas_SiGetPatProgramInfoFromRawInternal(pucRaw, ulSvcIdx, &stInfo);
		if (hErr == ERR_CAS_SI_RESULT_OK && (HUINT16)usSvcId == stInfo.usProgNum)
		{
			*pusPmtPid = stInfo.usProgMapPid;
			HxLOG_Print("[svc_cas_SiGetPmtPidByServiceIdFromRawInternal] Pmt PID = 0x%x\n", *pusPmtPid);
			return ERR_CAS_SI_RESULT_OK;
		}
	}

	return ERR_CAS_SI_RESULT_FAIL;
}

STATIC HERROR svc_cas_SiGetPmtPidByServiceIdInternal(svcCas_RawPool_t *pstRawPool,
													HUINT16 usTsUniqId,
													HUINT16 usSvcId,
													HUINT16 *pusPmtPid)
{
	HUINT16			 usPmtPid;
	HUINT32			 ulSecIdx, ulSecNum;
	HUINT8			*pucRaw;
	HERROR			 hErr;

	if (pstRawPool == NULL)		{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusPmtPid == NULL)		{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_SiCountPatSectionInternal(pstRawPool, usTsUniqId, &ulSecNum);
	if (hErr != ERR_CAS_SI_RESULT_OK)					{ ulSecNum = 0; }

	for (ulSecIdx = 0; ulSecIdx < ulSecNum; ulSecIdx++)
	{
		pucRaw = svc_cas_SiFindPatSectinTable (pstRawPool, usTsUniqId, ulSecIdx);
		if (pucRaw != NULL)
		{
			hErr = svc_cas_SiGetPmtPidByServiceIdFromRawInternal(pucRaw, usSvcId, &usPmtPid);
			if (hErr == ERR_CAS_SI_RESULT_OK)
			{
				*pusPmtPid = usPmtPid;
				return ERR_CAS_SI_RESULT_OK;
			}
		}
	}

	return ERR_CAS_SI_RESULT_FAIL;
}

STATIC HERROR svc_cas_SiGetFirstPmtPidFromRaw(HUINT8 *pucRaw, HUINT16 *pusProgNum, HUINT16 *pusPmtPid)
{
	HUINT32			 ulSvcIdx, ulSvcCnt;
	svcCas_SiPatProgInfo_t	 stInfo;
	HERROR			 hErr;

	if (pucRaw == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusPmtPid == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	if (pucRaw[0] != PAT_TABLE_ID)		{ return ERR_CAS_SI_INVALID_TAG; }

	hErr = svc_cas_SiCountPatProgramFromRawInternal(pucRaw, &ulSvcCnt);
	if (hErr != ERR_CAS_SI_RESULT_OK)					{ ulSvcCnt = 0; }

	for (ulSvcIdx = 0; ulSvcIdx < ulSvcCnt; ulSvcIdx++)
	{
		hErr = svc_cas_SiGetPatProgramInfoFromRawInternal(pucRaw, ulSvcIdx, &stInfo);
		if (hErr == ERR_CAS_SI_RESULT_OK)
		{
			*pusPmtPid = stInfo.usProgMapPid;
			*pusProgNum = stInfo.usProgNum;
			HxLOG_Info("[svc_cas_SiGetFirstPmtPidFromRaw] Pmt PID = 0x%x\n", *pusPmtPid);
			return ERR_CAS_SI_RESULT_OK;
		}
	}

	return ERR_CAS_SI_RESULT_FAIL;
}

STATIC HERROR svc_cas_SiGetFirstPmtPidInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT16 *pusProgNum, HUINT16 *pusPmtPid)
{
	HUINT16 		 usPmtPid, usProgNum;
	HUINT32 		 ulSecIdx, ulSecNum;
	HUINT8			*pucRaw;
	HERROR			 hErr;

	if (pstRawPool == NULL)		{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusPmtPid == NULL)		{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_SiCountPatSectionInternal(pstRawPool, usTsUniqId, &ulSecNum);
	if (hErr != ERR_CAS_SI_RESULT_OK)					{ ulSecNum = 0; }

	for (ulSecIdx = 0; ulSecIdx < ulSecNum; ulSecIdx++)
	{
		pucRaw = svc_cas_SiFindPatSectinTable (pstRawPool, usTsUniqId, ulSecIdx);
		if (pucRaw != NULL)
		{
			hErr = svc_cas_SiGetFirstPmtPidFromRaw (pucRaw, &usProgNum, &usPmtPid);
			if (hErr == ERR_CAS_SI_RESULT_OK)
			{
				*pusPmtPid = usPmtPid;
				*pusProgNum = usProgNum;
				return ERR_CAS_SI_RESULT_OK;
			}
		}
	}

	return ERR_CAS_SI_RESULT_FAIL;
}

HERROR svc_cas_SiGetPmtPidByServiceIdFromRaw(HUINT8 *pucRaw, HUINT16 usSvcId, HUINT16 *pusPmtPid)
{
	HERROR			 hErr;

	HxLOG_Info("[svc_cas_SiGetPmtPidByServiceIdFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	hErr = svc_cas_SiGetPmtPidByServiceIdFromRawInternal(pucRaw, usSvcId, pusPmtPid);
	HxLOG_Info("[svc_cas_SiGetPmtPidByServiceIdFromRaw] (-)\n");

	return hErr;
}

HERROR svc_cas_SiGetPmtPidByServiceId(svcCas_RawPool_t *pstRawPool,
											HUINT16 usTsUniqId,
											HUINT16 usSvcId,
											HUINT16 *pusPmtPid)
{
	HERROR			 hErr;

	HxLOG_Info("[svc_cas_SiGetPmtPidByServiceId] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	hErr = svc_cas_SiGetPmtPidByServiceIdInternal(pstRawPool, usTsUniqId, usSvcId, pusPmtPid);
	HxLOG_Info("[svc_cas_SiGetPmtPidByServiceId] (-)\n");

	return hErr;
}

HERROR svc_cas_SiGetFirstPmtPid(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT16 *pusProgNum, HUINT16 *pusPmtPid)
{
	HERROR			 hErr;

	HxLOG_Info("[svc_cas_SiGetFirstPmtPid] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	hErr = svc_cas_SiGetFirstPmtPidInternal(pstRawPool, usTsUniqId, pusProgNum, pusPmtPid);
	HxLOG_Info("[svc_cas_SiGetFirstPmtPid] (-)\n");

	return hErr;
}

#define ___________PMT_INDEX____________

STATIC INLINE void svc_cas_SiGetFirstProgDescriptorPos(HUINT8 *pucSection, HUINT8 **ppFirstProgDes)
{
	*ppFirstProgDes = pucSection + 12;
}

STATIC INLINE void svc_cas_SiGetNextProgDescriptorPos(HUINT8 *pucProgDes, HUINT8 **ppNextProgDes, HUINT32 ulDesLen)
{
	*ppNextProgDes = pucProgDes + 2 + (HUINT32)ulDesLen;
}

STATIC INLINE void svc_cas_SiGetFirstProgElemInfoPos(HUINT8 *pucSection, HUINT8 **ppFirstElement, HINT32 nProgDesLen)
{
	*ppFirstElement = pucSection + 12 + nProgDesLen;
}

STATIC INLINE void svc_cas_SiGetNextProgElemInfoPos(HUINT8 *pucElement, HUINT8 **ppNextElement, HUINT32 ulElemDesLen)
{
	*ppNextElement = pucElement + 5 + ulElemDesLen;
}

STATIC INLINE void svc_cas_SiGetFirstProgElemDesPos(HUINT8 *pucElement, HUINT8 **ppFirstElemDes)
{
	*ppFirstElemDes = pucElement + 5;
}

STATIC INLINE void svc_cas_SiGetNextProgElemDesPos(HUINT8 *pucElemDes, HUINT8 **ppNextElemDes, HUINT32 ulElemDesLen)
{
	*ppNextElemDes = pucElemDes + 2 + ulElemDesLen;
}

STATIC HUINT8 *svc_cas_SiFindPmtSectionTable(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulIdx)
{
	HUINT16			 usCount, usAllUniqueId;
	HUINT16			 usRawIndex;
	HUINT32			 ulPmtSecCnt;
	HUINT8			*pucRaw;
	svcCas_RawSecTable_t	*pstTable;

	if (pstRawPool == NULL)		{ return NULL; }


	usAllUniqueId = DB_SVC_GetAllUniqueId();
	pstTable = svc_cas_RawGetRawTable (pstRawPool, eSI_TableType_PMT);
	if (pstTable == NULL)		{ return NULL; }

	ulPmtSecCnt = 0;
	for (usCount = pstTable->usStartPos; usCount < pstTable->usInsertPos; usCount++)
	{
		if (pstTable->phIdx[usCount] == INVALID_INDEX_HANDLE)				{ continue; }

		// If there is no match in the unique ID, then it shall be skipped.
		if ((usTsUniqId != usAllUniqueId)
			&& (usTsUniqId != MACRO_GET_UNIQUEID_FROM_IDXHANDLE(pstTable->phIdx[usCount])))				{ continue; }

		if (ulPmtSecCnt >= ulIdx)
		{
			usRawIndex = MACRO_GET_INDEX_FROM_IDXHANDLE(pstTable->phIdx[usCount]);
			pucRaw = svc_cas_RawGetRawSection (pstRawPool, (HUINT32)usRawIndex);
			if (pucRaw != NULL)
			{
				return pucRaw;
			}
		}
		else
		{
			ulPmtSecCnt++;
		}
	}

	return NULL;
}

STATIC HERROR svc_cas_SiCountPmtSectionInternal(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 *pulSecCnt)
{
	HUINT16			 usCount, usAllUniqueId;
	HUINT32			 ulPmtSecCnt;
	svcCas_RawSecTable_t	*pstTable;

	if (pstRawPool == NULL)		{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulSecCnt == NULL)		{ return ERR_CAS_SI_TARGET_NULL; }

	usAllUniqueId = DB_SVC_GetAllUniqueId();
	pstTable = svc_cas_RawGetRawTable (pstRawPool, eSI_TableType_PMT);
	if (pstTable == NULL)		{ return ERR_CAS_SI_SOURCE_INVALID; }

	ulPmtSecCnt = 0;
	for (usCount = pstTable->usStartPos; usCount < pstTable->usInsertPos; usCount++)
	{
		if (pstTable->phIdx[usCount] == INVALID_INDEX_HANDLE)				{ continue; }

		// If there is no match in the unique ID, then it shall be skipped.
		if ((usTsUniqId != usAllUniqueId)
			&& (usTsUniqId != MACRO_GET_UNIQUEID_FROM_IDXHANDLE(pstTable->phIdx[usCount])))				{ continue; }

		ulPmtSecCnt++;
	}

	*pulSecCnt = ulPmtSecCnt;
	HxLOG_Print("Pmt section count = %d\n", ulPmtSecCnt);

	return ERR_CAS_SI_RESULT_OK;
}

STATIC HERROR svc_cas_SiGetPmtProgramInfoFromRawInternal(HUINT8 *pucRaw, svcCas_SiPmtProgramInfo_t *pstInfo)
{
	HUINT16		 nSecLen;

	if (pucRaw == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pstInfo == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	HxSTD_memset (pstInfo, 0, sizeof(svcCas_SiPmtProgramInfo_t));
	nSecLen = HxMACRO_Get12Bit(pucRaw + 1);

	if (nSecLen < 5)
	{
		HxLOG_Critical("\n\n\n");
		return ERR_CAS_SI_RESULT_FAIL;
	}

	pstInfo->usProgNum	 = HxMACRO_Get16Bit(pucRaw + 3);
	pstInfo->ucVer		 = (pucRaw[5] & 0x3e) >> 1;
	pstInfo->bCurNextInd = pucRaw[5] & 0x01;
	pstInfo->usPCRPid	 = HxMACRO_Get13Bit(pucRaw + 8);
	pstInfo->ulCrc32	 = HxMACRO_Get32Bit(pucRaw + nSecLen - 1);			// +3 (After the length) - 4 (CRC) = -1

	HxLOG_Print("ProgramNum = 0x%x\n", pstInfo->usProgNum);
	HxLOG_Print("Version num = 0x%x\n", pstInfo->ucVer);
	HxLOG_Print("Current next indicator = 0x%x\n", pstInfo->bCurNextInd);
	HxLOG_Print("PCR Pid = 0x%x\n", pstInfo->usPCRPid);
	HxLOG_Print("Crc 32 = 0x%X\n", pstInfo->ulCrc32);

	return ERR_CAS_SI_RESULT_OK;
}

STATIC HERROR svc_cas_SiGetPmtProgramInfoInternal(svcCas_RawPool_t *pstRawPool,
													HUINT16 usTsUniqId,
													HUINT32 ulIdx,
													svcCas_SiPmtProgramInfo_t *pstInfo)
{
	HUINT8		*pucRaw;

	if (pstRawPool == NULL)			{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pstInfo == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindPmtSectionTable (pstRawPool, usTsUniqId, ulIdx);
	return svc_cas_SiGetPmtProgramInfoFromRawInternal(pucRaw, pstInfo);
}

STATIC HERROR svc_cas_SiCountPmtProgramDescriptorFromRawInternal(HUINT8 *pucRaw,
																		HUINT32 ulTag,
																		HUINT32 *pulDesCnt)
{
	HUINT8		 ucDesTag, ucDesLen;
	HUINT32		 ulDesCnt;
	HINT32		 nSecLen, nProgInfoLen;
	HUINT8		*pucProgDes;

	if (pucRaw == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulDesCnt == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	ulDesCnt = 0;

	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (9 + 4); 									// program info descriptor 바로 앞까지의 table fileds과 crc를 제거.
	nProgInfoLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 10);

	svc_cas_SiGetFirstProgDescriptorPos (pucRaw, &pucProgDes);
	while ((nSecLen > 0) && (nProgInfoLen > 0) && (nSecLen > nProgInfoLen))
	{
		if (nSecLen < 2)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_CAS_SI_RESULT_FAIL;
		}

		ucDesTag = HxMACRO_Get8Bit(pucProgDes);
		ucDesLen = HxMACRO_Get8Bit(pucProgDes + 1);

		nSecLen -= (2 + ucDesLen);
		nProgInfoLen -= (2 + ucDesLen);

		if (nSecLen < 0)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_CAS_SI_RESULT_FAIL;
		}

		if (svc_cas_SiIsSameDesTag ((HUINT32)ucDesTag, ulTag) == TRUE)
		{
			ulDesCnt++;
		}

		svc_cas_SiGetNextProgDescriptorPos (pucProgDes, &pucProgDes, (HUINT32)ucDesLen);
	}

	*pulDesCnt = ulDesCnt;
	HxLOG_Print("Pmt program descriptor count = %d\n",ulDesCnt);

	return ERR_CAS_SI_RESULT_OK;
}


STATIC HERROR svc_cas_SiCountPmtProgramDescriptorInternal(svcCas_RawPool_t *pstRawPool,
															HUINT16 usTsUniqId,
															HUINT32 ulIdx,
															HUINT32 ulTag,
															HUINT32 *pnCnt)
{
	HUINT8		*pucRaw;

	if (pstRawPool == NULL)			{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pnCnt == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindPmtSectionTable (pstRawPool, usTsUniqId, ulIdx);
	return svc_cas_SiCountPmtProgramDescriptorFromRawInternal(pucRaw, ulTag, pnCnt);
}

STATIC HERROR svc_cas_SiGetPmtProgramDescriptorFromRawInternal(HUINT8 *pucRaw,
																	SIxTextEncoding_e eTextEnc,
																	HUINT32 ulTag,
																	HUINT32 ulDesIdx,
																	void **ppvDes)
{
	HUINT8		 ucDesTag, ucDesLen;
	HUINT32		 ulDesCnt;
	HINT32		 nSecLen, nProgInfoLen;
	HUINT8		*pucProgDes, *pucDes;
	HERROR		 hErr;

	if (pucRaw == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (ppvDes == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (9 + 4); 									// program info descriptor 바로 앞까지의 table fileds과 crc를 제거.
	nProgInfoLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 10);

	ulDesCnt = 0;
	svc_cas_SiGetFirstProgDescriptorPos (pucRaw, &pucProgDes);
	while ((nSecLen > 0) && (nProgInfoLen > 0) && (nSecLen > nProgInfoLen))
	{
		if (nSecLen < 2)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_CAS_SI_RESULT_FAIL;
		}

		ucDesTag = HxMACRO_Get8Bit(pucProgDes);
		ucDesLen = HxMACRO_Get8Bit(pucProgDes + 1);

		nSecLen -= (2 + ucDesLen);
		nProgInfoLen -= (2 + ucDesLen);

		if (nSecLen < 0)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_CAS_SI_RESULT_FAIL;
		}

		if (svc_cas_SiIsSameDesTag ((HUINT32)ucDesTag, ulTag) != TRUE)			// Different TAG
		{
			svc_cas_SiGetNextProgDescriptorPos (pucProgDes, &pucProgDes, (HUINT32)ucDesLen);
			continue;
		}

		if (ulDesCnt != ulDesIdx)		// Different Index
		{
			ulDesCnt++;
			svc_cas_SiGetNextProgDescriptorPos (pucProgDes, &pucProgDes, (HUINT32)ucDesLen);
			continue;
		}

		HxLOG_Print("Pmt program Descriptor Tag = 0x%x\n",ucDesTag);

		pucDes = svc_cas_SiAllocDes(ucDesTag, ucDesLen);
		if (pucDes == NULL)							{ return ERR_CAS_SI_ALLOC_DESCRIPTOR_FAIL; }
		hErr = svc_cas_SiParseDes(eTextEnc, pucProgDes, pucDes);
		if (hErr != ERR_CAS_SI_RESULT_OK)
		{
			svc_cas_SiFreeDes(pucDes);
			if (hErr == ERR_CAS_SI_INVALID_TAG || hErr == ERR_CAS_SI_INVALID_DESCRIPTOR)
			{
				svc_cas_SiGetNextProgDescriptorPos (pucProgDes, &pucProgDes, (HUINT32)ucDesLen);
				continue;
			}
			else
			{
				return ERR_CAS_SI_CANNOT_FIND_DESCRIPTOR;
			}
		}

		*ppvDes = (void *)pucDes;
		return ERR_CAS_SI_RESULT_OK;
	}

	return ERR_CAS_SI_RESULT_FAIL;
}

STATIC HERROR svc_cas_SiGetPmtProgramDescriptorInternal(svcCas_RawPool_t *pstRawPool,
															SIxTextEncoding_e eTextEnc,
															HUINT16 usTsUniqId,
															HUINT32 ulIdx,
															HUINT32 ulTag,
															HUINT32 nDesIdx,
															void **ppvDes)
{
	HUINT8		*pucRaw;

	if (pstRawPool == NULL)			{ return ERR_CAS_SI_SOURCE_NULL; }
	if (ppvDes == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindPmtSectionTable (pstRawPool, usTsUniqId, ulIdx);
	return svc_cas_SiGetPmtProgramDescriptorFromRawInternal(pucRaw, eTextEnc, ulTag, nDesIdx, ppvDes);
}

STATIC HERROR svc_cas_SiGetPmtProgramDesPtrFromRaw(HUINT8 *pucRaw,
													HUINT32 ulTag,
													HUINT32 ulDesIdx,
													HUINT32 *pulDesRawSize,
													HUINT8 **ppucDesRaw)
{
	HUINT8		 ucDesTag, ucDesLen;
	HUINT32		 ulDesCnt;
	HINT32		 nSecLen, nProgInfoLen;
	HUINT8		*pucProgDes;

	if (pucRaw == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulDesRawSize == NULL)		{ return ERR_CAS_SI_TARGET_NULL; }
	if (ppucDesRaw == NULL) 		{ return ERR_CAS_SI_TARGET_NULL; }

	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (9 + 4); 									// program info descriptor 바로 앞까지의 table fileds과 crc를 제거.
	nProgInfoLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 10);

	ulDesCnt = 0;
	svc_cas_SiGetFirstProgDescriptorPos (pucRaw, &pucProgDes);
	while ((nSecLen > 0) && (nProgInfoLen > 0) && (nSecLen > nProgInfoLen))
	{
		if (nSecLen < 2)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_CAS_SI_RESULT_FAIL;
		}

		ucDesTag = HxMACRO_Get8Bit(pucProgDes);
		ucDesLen = HxMACRO_Get8Bit(pucProgDes + 1);

		nSecLen -= (2 + ucDesLen);
		nProgInfoLen -= (2 + ucDesLen);

		if (nSecLen < 0)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_CAS_SI_RESULT_FAIL;
		}

		if (svc_cas_SiIsSameDesTag ((HUINT32)ucDesTag, ulTag) != TRUE)			// Different TAG
		{
			svc_cas_SiGetNextProgDescriptorPos (pucProgDes, &pucProgDes, (HUINT32)ucDesLen);
			continue;
		}

		if (ulDesCnt != ulDesIdx)		// Different Index
		{
			ulDesCnt++;
			svc_cas_SiGetNextProgDescriptorPos (pucProgDes, &pucProgDes, (HUINT32)ucDesLen);
			continue;
		}

		HxLOG_Print("Pmt program Descriptor Tag = 0x%x\n",ucDesTag);

		*pulDesRawSize	= (HUINT32)ucDesLen + 2;
		*ppucDesRaw		= pucProgDes;
		return ERR_CAS_SI_RESULT_OK;
	}

	return ERR_CAS_SI_RESULT_FAIL;
}

STATIC HERROR svc_cas_SiGetPmtProgramAllDesPtrFromRaw(HUINT8 *pucRaw,
														HUINT32 *pulDesRawSize,
														HUINT8 **ppucDesRaw)
{
	HINT32		 nSecLen, nProgInfoLen;
	HUINT8		*pucProgDes;

	if (pucRaw == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulDesRawSize == NULL)		{ return ERR_CAS_SI_TARGET_NULL; }
	if (ppucDesRaw == NULL) 		{ return ERR_CAS_SI_TARGET_NULL; }

	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (9 + 4); 									// program info descriptor 바로 앞까지의 table fileds과 crc를 제거.
	nProgInfoLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 10);

	svc_cas_SiGetFirstProgDescriptorPos (pucRaw, &pucProgDes);

	*pulDesRawSize = nProgInfoLen;
	*ppucDesRaw = pucProgDes;

	return ERR_CAS_SI_RESULT_OK;
}

STATIC HERROR svc_cas_SiGetPmtProgramDesPtr(svcCas_RawPool_t *pstRawPool,
												HUINT16 usTsUniqId,
												HUINT32 ulIdx,
												HUINT32 ulTag,
												HUINT32 ulDesIdx,
												HUINT32 *pulDesRawSize,
												HUINT8 **ppucDesRaw)
{
	HUINT8		*pucRaw;

	if (pstRawPool == NULL)			{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulDesRawSize == NULL)		{ return ERR_CAS_SI_TARGET_NULL; }
	if (ppucDesRaw == NULL) 		{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindPmtSectionTable (pstRawPool, usTsUniqId, ulIdx);
	return svc_cas_SiGetPmtProgramDesPtrFromRaw (pucRaw, ulTag, ulDesIdx, pulDesRawSize, ppucDesRaw);
}

STATIC HERROR svc_cas_SiGetPmtProgramAllDesPtr(svcCas_RawPool_t *pstRawPool,
												HUINT16 usTsUniqId,
												HUINT32 ulIdx,
												HUINT32 *pulDesRawSize,
												HUINT8 **ppucDesRaw)
{
	HUINT8		*pucRaw;

	if (pstRawPool == NULL)			{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulDesRawSize == NULL)		{ return ERR_CAS_SI_TARGET_NULL; }
	if (ppucDesRaw == NULL) 		{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindPmtSectionTable (pstRawPool, usTsUniqId, ulIdx);
	return svc_cas_SiGetPmtProgramAllDesPtrFromRaw (pucRaw, pulDesRawSize, ppucDesRaw);
}

STATIC HERROR svc_cas_SiCountPmtElementFromRawInternal(HUINT8 *pucRaw, HUINT32 *pulElemCnt)
{
	HUINT16		 usEsInfoLen;
	HUINT32		 ulElemCnt;
	HINT32		 nSecLen, nProgInfoLen;
	HUINT8		*pucElement;

	if (pucRaw == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulElemCnt == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	ulElemCnt = 0;
	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (9 + 4); 									// program info descriptor 바로 앞까지의 table fileds과 crc를 제거.
	nProgInfoLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 10);

	nSecLen -= nProgInfoLen;
	svc_cas_SiGetFirstProgElemInfoPos (pucRaw, &pucElement, nProgInfoLen);
	while (nSecLen > 0)
	{
		if (nSecLen < 5)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_CAS_SI_RESULT_FAIL;
		}

		usEsInfoLen = HxMACRO_Get12Bit(pucElement + 3);			// The length of the Descriptors
		nSecLen -= (usEsInfoLen + 5);
		ulElemCnt++;
		svc_cas_SiGetNextProgElemInfoPos (pucElement, &pucElement, (HUINT32)usEsInfoLen);
	}

	*pulElemCnt = ulElemCnt;
	HxLOG_Print("Pmt Element count = %d\n",ulElemCnt);

	return ERR_CAS_SI_RESULT_OK;
}

STATIC HERROR svc_cas_SiCountPmtElementInternal(svcCas_RawPool_t *pstRawPool,
												HUINT16 usTsUniqId,
												HUINT32 ulIdx,
												HUINT32 *pnCnt)
{
	HUINT8		*pucRaw;

	if (pstRawPool == NULL)			{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pnCnt == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindPmtSectionTable (pstRawPool, usTsUniqId, ulIdx);
	return svc_cas_SiCountPmtElementFromRawInternal(pucRaw, pnCnt);
}

STATIC HERROR svc_cas_SiGetPmtElementInfoFromRawInternal(HUINT8 *pucRaw,
															HUINT32 ulElemIdx,
															svcCas_SiPmtElementInfo_t *pstInfo)
{
	HUINT16		 usEsInfoLen;
	HUINT32		 ulElemCnt;
	HINT32		 nSecLen, nProgInfoLen;
	HUINT8		*pucElement;

	if (pucRaw == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pstInfo == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	ulElemCnt = 0;
	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (9 + 4); 									// program info descriptor 바로 앞까지의 table fileds과 crc를 제거.
	nProgInfoLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 10);

	nSecLen -= nProgInfoLen;
	svc_cas_SiGetFirstProgElemInfoPos (pucRaw, &pucElement, nProgInfoLen);
	while (nSecLen > 0)
	{
		if (nSecLen < 5)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_CAS_SI_RESULT_FAIL;
		}

		usEsInfoLen = HxMACRO_Get12Bit(pucElement + 3);			// The length of the Descriptors
		nSecLen -= (usEsInfoLen + 5);

		if (ulElemCnt != ulElemIdx)
		{
			ulElemCnt++;
			svc_cas_SiGetNextProgElemInfoPos (pucElement, &pucElement, (HUINT32)usEsInfoLen);
			continue;
		}

		if (nSecLen < 0)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_CAS_SI_RESULT_FAIL;
		}

		HxSTD_memset (pstInfo, 0, sizeof(svcCas_SiPmtElementInfo_t));
		pstInfo->ucStreamType = HxMACRO_Get8Bit(pucElement);
		pstInfo->usElemPid = HxMACRO_Get13Bit(pucElement + 1);

		HxLOG_Print("StreamType = 0x%x, ElemPid = 0x%x\n", pstInfo->ucStreamType, pstInfo->usElemPid);

		return ERR_CAS_SI_RESULT_OK;
	}

	return ERR_CAS_SI_RESULT_FAIL;
}


STATIC HERROR svc_cas_SiGetPmtElementInfoInternal(svcCas_RawPool_t *pstRawPool,
													HUINT16 usTsUniqId,
													HUINT32 ulIdx,
													HUINT32 nSubIdx,
													svcCas_SiPmtElementInfo_t *pstInfo)
{
	HUINT8		*pucRaw;

	if (pstRawPool == NULL)			{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pstInfo == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindPmtSectionTable (pstRawPool, usTsUniqId, ulIdx);
	return svc_cas_SiGetPmtElementInfoFromRawInternal(pucRaw, nSubIdx, pstInfo);
}

STATIC HERROR svc_cas_SiCountPmtElementDescriptorFromRawInternal(HUINT8 *pucRaw,
																		HUINT32 ulElemIdx,
																		HUINT32 ulTag,
																		HUINT32 *pulDesCnt)
{
	HUINT8		 ucDesTag, ucDesLen;
	HUINT32		 ulElemCnt;
	HUINT32		 ulDesCnt;
	HINT32		 nSecLen, nProgInfoLen, nEsInfoLen;
	HUINT8		*pucElement, *pucElemDes;

	if (pucRaw == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulDesCnt == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (9 + 4); 									// program info descriptor 바로 앞까지의 table fileds과 crc를 제거.
	nProgInfoLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 10);

	nSecLen -= nProgInfoLen;
	svc_cas_SiGetFirstProgElemInfoPos (pucRaw, &pucElement, nProgInfoLen);

	ulElemCnt = 0;
	while (nSecLen > 0)
	{
		nEsInfoLen = (HINT32)HxMACRO_Get12Bit(pucElement + 3);			// The length of the Descriptors
		nSecLen -= (nEsInfoLen + 5);

		if (nSecLen < 0)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_CAS_SI_RESULT_FAIL;
		}

		if (ulElemCnt != ulElemIdx)
		{
			ulElemCnt++;
			svc_cas_SiGetNextProgElemInfoPos (pucElement, &pucElement, (HUINT32)nEsInfoLen);
			continue;
		}

		ulDesCnt = 0;
		svc_cas_SiGetFirstProgElemDesPos (pucElement, &pucElemDes);

		while (nEsInfoLen > 0)
		{
			if (nEsInfoLen < 2)
			{
				HxLOG_Critical("\n\n\n");
				return ERR_CAS_SI_RESULT_FAIL;
			}

			ucDesTag = HxMACRO_Get8Bit(pucElemDes);
			ucDesLen = HxMACRO_Get8Bit(pucElemDes + 1);

			nEsInfoLen -= (ucDesLen + 2);
			if (nEsInfoLen < 0)
			{
				HxLOG_Critical("\n\n\n");
				return ERR_CAS_SI_RESULT_FAIL;
			}

			if (svc_cas_SiIsSameDesTag ((HUINT32)ucDesTag, ulTag) == TRUE)			// Same TAG
			{
				ulDesCnt++;
			}

			svc_cas_SiGetNextProgElemDesPos (pucElemDes, &pucElemDes, (HUINT32)ucDesLen);
		}

		*pulDesCnt = ulDesCnt;
		HxLOG_Print("Pmt Element descriptor count = %d\n", ulDesCnt);

		return ERR_CAS_SI_RESULT_OK;
	}

	return ERR_CAS_SI_RESULT_FAIL;
}

STATIC HERROR svc_cas_SiCountPmtElementDescriptorInternal(svcCas_RawPool_t *pstRawPool,
															HUINT16 usTsUniqId,
															HUINT32 ulIdx,
															HUINT32 nSubIdx,
															HUINT32 ulTag,
															HUINT32 *pnDesCnt)
{
	HUINT8		*pucRaw;

	if (pstRawPool == NULL)			{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pnDesCnt == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindPmtSectionTable (pstRawPool, usTsUniqId, ulIdx);
	return svc_cas_SiCountPmtElementDescriptorFromRawInternal(pucRaw, nSubIdx, ulTag, pnDesCnt);
}

STATIC HERROR svc_cas_SiGetPmtElementDescriptorFromRawInternal(HUINT8 *pucRaw,
																	SIxTextEncoding_e eTextEnc,
																	HUINT32 ulElemIdx,
																	HUINT32 ulTag,
																	HUINT32 ulDesIdx,
																	void **ppvDes)
{
	HUINT8		 ucDesTag, ucDesLen;
	HUINT32		 ulElemCnt;
	HUINT32		 ulDesCnt;
	HINT32		 nSecLen, nProgInfoLen, nEsInfoLen;
	HUINT8		*pucElement, *pucElemDes;
	HUINT8		*pucDes;
	HERROR		 hErr;

	if (pucRaw == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (ppvDes == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (9 + 4); 									// program info descriptor 바로 앞까지의 table fileds과 crc를 제거.
	nProgInfoLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 10);

	nSecLen -= nProgInfoLen;
	svc_cas_SiGetFirstProgElemInfoPos (pucRaw, &pucElement, nProgInfoLen);
	ulElemCnt = 0;
	while (nSecLen > 0)
	{
		nEsInfoLen = (HINT32)HxMACRO_Get12Bit(pucElement + 3);			// The length of the Descriptors
		nSecLen -= (nEsInfoLen + 5);

		if (nSecLen < 0)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_CAS_SI_RESULT_FAIL;
		}

		if (ulElemCnt != ulElemIdx)
		{
			ulElemCnt++;
			svc_cas_SiGetNextProgElemInfoPos (pucElement, &pucElement, (HUINT32)nEsInfoLen);
			continue;
		}

		ulDesCnt = 0;
		svc_cas_SiGetFirstProgElemDesPos (pucElement, &pucElemDes);
		while (nEsInfoLen > 0)
		{
			if (nEsInfoLen < 2)
			{
				HxLOG_Critical("\n\n\n");
				return ERR_CAS_SI_RESULT_FAIL;
			}

			ucDesTag = HxMACRO_Get8Bit(pucElemDes);
			ucDesLen = HxMACRO_Get8Bit(pucElemDes + 1);

			nEsInfoLen -= (ucDesLen + 2);
			if (nEsInfoLen < 0)
			{
				HxLOG_Critical("\n\n\n");
				return ERR_CAS_SI_RESULT_FAIL;
			}

			if (svc_cas_SiIsSameDesTag((HUINT32)ucDesTag, ulTag) != TRUE)			// Different TAG
			{
				svc_cas_SiGetNextProgElemDesPos (pucElemDes, &pucElemDes, (HUINT32)ucDesLen);
				continue;
			}

			if (ulDesCnt != ulDesIdx)		// Different Index
			{
				ulDesCnt++;
				svc_cas_SiGetNextProgElemDesPos (pucElemDes, &pucElemDes, (HUINT32)ucDesLen);
				continue;
			}

			pucDes = svc_cas_SiAllocDes(ucDesTag, ucDesLen);
			if (pucDes == NULL) 						{ return ERR_CAS_SI_ALLOC_DESCRIPTOR_FAIL; }
			hErr = svc_cas_SiParseDes(eTextEnc, pucElemDes, pucDes);
			if (hErr != ERR_CAS_SI_RESULT_OK)
			{
				svc_cas_SiFreeDes(pucDes);
				if (hErr == ERR_CAS_SI_INVALID_TAG || hErr == ERR_CAS_SI_INVALID_DESCRIPTOR)
				{
					svc_cas_SiGetNextProgElemDesPos (pucElemDes, &pucElemDes, (HUINT32)ucDesLen);
					continue;
				}
				else
				{
					return ERR_CAS_SI_CANNOT_FIND_DESCRIPTOR;
				}
			}

			*ppvDes = (void *)pucDes;
			return ERR_CAS_SI_RESULT_OK;
		}

		break;
	}

	return ERR_CAS_SI_RESULT_FAIL;
}

STATIC HERROR svc_cas_SiGetPmtElementDescriptorInternal(svcCas_RawPool_t *pstRawPool,
															SIxTextEncoding_e eTextEnc,
															HUINT16 usTsUniqId,
															HUINT32 ulIdx,
															HUINT32 nSubIdx,
															HUINT32 ulTag,
															HUINT32 nDesIdx,
															void **ppvDes)
{
	HUINT8		*pucRaw;

	if (pstRawPool == NULL)			{ return ERR_CAS_SI_SOURCE_NULL; }
	if (ppvDes == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindPmtSectionTable(pstRawPool, usTsUniqId, ulIdx);
	return svc_cas_SiGetPmtElementDescriptorFromRawInternal(pucRaw, eTextEnc, nSubIdx, ulTag, nDesIdx, ppvDes);
}

STATIC HERROR svc_cas_SiGetPmtElementDesPtrFromRaw(HUINT8 *pucRaw,
																	HUINT32 ulElemIdx,
																	HUINT32 ulTag,
																	HUINT32 ulDesIdx,
																	HUINT32 *pulDesRawSize,
																	HUINT8 **ppucDesRaw)
{
	HUINT8		 ucDesTag, ucDesLen;
	HUINT32		 ulElemCnt;
	HUINT32		 ulDesCnt;
	HUINT32		 ulDesRawSize;
	HINT32		 nSecLen, nProgInfoLen, nEsInfoLen;
	HUINT8		*pucElement, *pucElemDes;

	if (pucRaw == NULL)										{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulDesRawSize == NULL || ppucDesRaw == NULL)		{ return ERR_CAS_SI_TARGET_NULL; }

	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (9 + 4); 									// program info descriptor 바로 앞까지의 table fileds과 crc를 제거.
	nProgInfoLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 10);

	nSecLen -= nProgInfoLen;
	svc_cas_SiGetFirstProgElemInfoPos (pucRaw, &pucElement, nProgInfoLen);
	ulElemCnt = 0;
	while (nSecLen > 0)
	{
		nEsInfoLen = (HINT32)HxMACRO_Get12Bit(pucElement + 3);			// The length of the Descriptors
		nSecLen -= (nEsInfoLen + 5);

		if (nSecLen < 0)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_CAS_SI_RESULT_FAIL;
		}

		if (ulElemCnt != ulElemIdx)
		{
			ulElemCnt++;
			svc_cas_SiGetNextProgElemInfoPos (pucElement, &pucElement, (HUINT32)nEsInfoLen);
			continue;
		}

		ulDesCnt = 0;
		svc_cas_SiGetFirstProgElemDesPos (pucElement, &pucElemDes);
		while (nEsInfoLen > 0)
		{
			if (nEsInfoLen < 2)
			{
				HxLOG_Critical("\n\n\n");
				return ERR_CAS_SI_RESULT_FAIL;
			}

			ucDesTag = HxMACRO_Get8Bit(pucElemDes);
			ucDesLen = HxMACRO_Get8Bit(pucElemDes + 1);

			nEsInfoLen -= (ucDesLen + 2);
			if (nEsInfoLen < 0)
			{
				HxLOG_Critical("\n\n\n");
				return ERR_CAS_SI_RESULT_FAIL;
			}

			if (svc_cas_SiIsSameDesTag ((HUINT32)ucDesTag, ulTag) != TRUE)			// Different TAG
			{
				svc_cas_SiGetNextProgElemDesPos (pucElemDes, &pucElemDes, (HUINT32)ucDesLen);
				continue;
			}

			if (ulDesCnt != ulDesIdx)		// Different Index
			{
				ulDesCnt++;
				svc_cas_SiGetNextProgElemDesPos (pucElemDes, &pucElemDes, (HUINT32)ucDesLen);
				continue;
			}

			ulDesRawSize = (HUINT32)ucDesLen + 2;

			*pulDesRawSize	= ulDesRawSize;
			*ppucDesRaw		= pucElemDes;
			return ERR_CAS_SI_RESULT_OK;
		}

		break;
	}

	return ERR_CAS_SI_RESULT_FAIL;
}

STATIC HERROR svc_cas_SiGetPmtElementAllDesPtrFromRaw(HUINT8 *pucRaw,
																	HUINT32 ulElemIdx,
																	HUINT32 *pulDesRawSize,
																	HUINT8 **ppucDesRaw)
{
	HUINT32		 ulElemCnt;
	HINT32		 nSecLen, nProgInfoLen, nEsInfoLen;
	HUINT8		*pucElement, *pucElemDes;

	if (pucRaw == NULL)										{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulDesRawSize == NULL || ppucDesRaw == NULL)		{ return ERR_CAS_SI_TARGET_NULL; }

	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (9 + 4); 									// program info descriptor 바로 앞까지의 table fileds과 crc를 제거.
	nProgInfoLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 10);

	nSecLen -= nProgInfoLen;
	svc_cas_SiGetFirstProgElemInfoPos (pucRaw, &pucElement, nProgInfoLen);
	ulElemCnt = 0;
	while (nSecLen > 0)
	{
		nEsInfoLen = (HINT32)HxMACRO_Get12Bit(pucElement + 3);			// The length of the Descriptors
		nSecLen -= (nEsInfoLen + 5);

		if (nSecLen < 0)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_CAS_SI_RESULT_FAIL;
		}

		if (ulElemCnt != ulElemIdx)
		{
			ulElemCnt++;
			svc_cas_SiGetNextProgElemInfoPos (pucElement, &pucElement, (HUINT32)nEsInfoLen);
			continue;
		}

		svc_cas_SiGetFirstProgElemDesPos (pucElement, &pucElemDes);
		*pulDesRawSize	= nEsInfoLen;
		*ppucDesRaw		= pucElemDes;

		return ERR_CAS_SI_RESULT_OK;
	}

	return ERR_CAS_SI_RESULT_FAIL;
}

STATIC HERROR svc_cas_SiGetPmtElementDesPtr(svcCas_RawPool_t *pstRawPool,
															HUINT16 usTsUniqId,
															HUINT32 ulIdx,
															HUINT32 nSubIdx,
															HUINT32 ulTag,
															HUINT32 nDesIdx,
															HUINT32 *pulDesRawSize,
															HUINT8 **ppucDesRaw)
{
	HUINT8		*pucRaw;

	if (pstRawPool == NULL)			{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulDesRawSize == NULL)		{ return ERR_CAS_SI_TARGET_NULL; }
	if (ppucDesRaw == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindPmtSectionTable (pstRawPool, usTsUniqId, ulIdx);
	return svc_cas_SiGetPmtElementDesPtrFromRaw (pucRaw, nSubIdx, ulTag, nDesIdx, pulDesRawSize, ppucDesRaw);
}

STATIC HERROR svc_cas_SiGetPmtElementAllDesPtr(svcCas_RawPool_t *pstRawPool,
															HUINT16 usTsUniqId,
															HUINT32 ulIdx,
															HUINT32 nSubIdx,
															HUINT32 *pulDesRawSize,
															HUINT8 **ppucDesRaw)
{
	HUINT8		*pucRaw;

	if (pstRawPool == NULL)			{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulDesRawSize == NULL)		{ return ERR_CAS_SI_TARGET_NULL; }
	if (ppucDesRaw == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindPmtSectionTable (pstRawPool, usTsUniqId, ulIdx);
	return svc_cas_SiGetPmtElementAllDesPtrFromRaw (pucRaw, nSubIdx, pulDesRawSize, ppucDesRaw);
}

HERROR svc_cas_SiCountPmtSection(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 *pnCnt)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiCountPmtSection] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	ulRet = svc_cas_SiCountPmtSectionInternal(pstRawPool, usTsUniqId, pnCnt);
	HxLOG_Info("[svc_cas_SiCountPmtSection] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiGetPmtProgramInfoFromRaw(HUINT8 *pucRaw, svcCas_SiPmtProgramInfo_t *pstInfo)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiGetPmtProgramInfoFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	ulRet = svc_cas_SiGetPmtProgramInfoFromRawInternal(pucRaw, pstInfo);
	HxLOG_Info("[svc_cas_SiGetPmtProgramInfoFromRaw] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiGetPmtProgramInfo(svcCas_RawPool_t *pstRawPool,
											HUINT16 usTsUniqId,
											HUINT32 ulIdx,
											svcCas_SiPmtProgramInfo_t *pstInfo)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiGetPmtProgramInfo] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	ulRet = svc_cas_SiGetPmtProgramInfoInternal(pstRawPool, usTsUniqId, ulIdx, pstInfo);
	HxLOG_Info("[svc_cas_SiGetPmtProgramInfo] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiCountPmtProgramDescriptorFromRaw(HUINT8 *pucRaw, HUINT32 ulTag, HUINT32 *pnCnt)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiCountPmtProgramDescriptorFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	ulRet = svc_cas_SiCountPmtProgramDescriptorFromRawInternal(pucRaw, ulTag, pnCnt);
	HxLOG_Info("[svc_cas_SiCountPmtProgramDescriptorFromRaw] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiCountPmtProgramDescriptor(svcCas_RawPool_t *pstRawPool,
														HUINT16 usTsUniqId,
														HUINT32 ulIdx,
														HUINT32 ulTag,
														HUINT32 *pnCnt)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiCountPmtProgramDescriptor] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	ulRet = svc_cas_SiCountPmtProgramDescriptorInternal(pstRawPool, usTsUniqId, ulIdx, ulTag, pnCnt);
	HxLOG_Info("[svc_cas_SiCountPmtProgramDescriptor] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiGetPmtProgramDescriptorTagFromRaw(HUINT8 *pucRaw,
														HUINT32 ulDesIdx,
														HUINT8 *pucTag)
{
	HUINT32		 ulDesRawSize;
	HUINT8		*pucDesRaw;
	HERROR		 hErr;

	HxLOG_Info("[svc_cas_SiGetPmtProgramDescriptorTagFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	hErr = svc_cas_SiGetPmtProgramDesPtrFromRaw (pucRaw, TAG_ALL_DESCRIPTORS, ulDesIdx, &ulDesRawSize, &pucDesRaw);
	if (hErr == ERR_CAS_SI_RESULT_OK)
	{
		*pucTag = *pucDesRaw;
	}
	HxLOG_Info("[svc_cas_SiGetPmtProgramDescriptorTagFromRaw] (-)\n" );

	return hErr;
}

HERROR svc_cas_SiGetPmtProgramDescriptorTag(svcCas_RawPool_t *pstRawPool,
													HUINT16 usTsUniqId,
													HUINT32 ulIdx,
													HUINT32 ulDesIdx,
													HUINT8 *pucTag)
{
	HUINT32		 ulDesRawSize;
	HUINT8		*pucDesRaw;
	HERROR		 hErr;

	HxLOG_Info("[svc_cas_SiGetPmtProgramDescriptorTag] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	hErr = svc_cas_SiGetPmtProgramDesPtr (pstRawPool, usTsUniqId, ulIdx, TAG_ALL_DESCRIPTORS, ulDesIdx, &ulDesRawSize, &pucDesRaw);
	if (hErr == ERR_CAS_SI_RESULT_OK)
	{
		*pucTag = *pucDesRaw;
	}
	HxLOG_Info("[svc_cas_SiGetPmtProgramDescriptorTag] (-)\n" );

	return hErr;
}

HERROR svc_cas_SiGetPmtProgramDescriptorFromRaw(HUINT8 *pucRaw,
													SIxTextEncoding_e eTextEnc,
													HUINT32 ulTag,
													HUINT32 nDesIdx,
													void **ppvDes)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiGetPmtProgramDescriptorFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	ulRet = svc_cas_SiGetPmtProgramDescriptorFromRawInternal(pucRaw, eTextEnc, ulTag, nDesIdx, ppvDes);
	HxLOG_Info("[svc_cas_SiGetPmtProgramDescriptorFromRaw] (-)\n" );

	return 	ulRet;
}

HERROR svc_cas_SiGetPmtProgramDescriptor(svcCas_RawPool_t *pstRawPool,
													SIxTextEncoding_e eTextEnc,
													HUINT16 usTsUniqId,
													HUINT32 ulIdx,
													HUINT32 ulTag,
													HUINT32 nDesIdx,
													void **ppvDes)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiGetPmtProgramDescriptor] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	ulRet = svc_cas_SiGetPmtProgramDescriptorInternal(pstRawPool, eTextEnc, usTsUniqId, ulIdx, ulTag, nDesIdx, ppvDes);
	HxLOG_Info("[svc_cas_SiGetPmtProgramDescriptor] (-)\n" );

	return 	ulRet;
}

HERROR svc_cas_SiGetPmtProgramDesRawFromRaw(HUINT8 *pucRaw,
												HUINT32 ulTag,
												HUINT32 nDesIdx,
												HUINT32 *pulDesRawSize,
												HUINT8 **ppucDesRaw)
{
	HUINT32		 ulDesRawSize;
	HUINT8		*pucDesRaw;
	HERROR		 hErr;

	HxLOG_Info("[svc_cas_SiGetPmtProgramDesRawFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	hErr = svc_cas_SiGetPmtProgramDesPtrFromRaw (pucRaw, ulTag, nDesIdx, &ulDesRawSize, &pucDesRaw);
	if (hErr == ERR_CAS_SI_RESULT_OK)
	{
		hErr = svc_cas_SiDuplicateDesRaw (ulDesRawSize, pucDesRaw, pulDesRawSize, ppucDesRaw);
	}
	HxLOG_Info("[svc_cas_SiGetPmtProgramDesRawFromRaw] (-)\n" );

	return 	hErr;
}

HERROR svc_cas_SiGetPmtProgramAllDesRawFromRaw(HUINT8 *pucRaw,
													HUINT32 *pulDesRawSize,
													HUINT8 **ppucDesRaw)
{
	HUINT32		 ulDesRawSize;
	HUINT8		*pucDesRaw;
	HERROR		 hErr;

	HxLOG_Info("[svc_cas_SiGetPmtProgramAllDesRawFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	hErr = svc_cas_SiGetPmtProgramAllDesPtrFromRaw (pucRaw, &ulDesRawSize, &pucDesRaw);
	if (hErr == ERR_CAS_SI_RESULT_OK)
	{
		hErr = svc_cas_SiDuplicateDesRaw (ulDesRawSize, pucDesRaw, pulDesRawSize, ppucDesRaw);
	}
	HxLOG_Info("[svc_cas_SiGetPmtProgramAllDesRawFromRaw] (-)\n" );

	return 	hErr;
}

HERROR svc_cas_SiGetPmtProgramDesRaw(svcCas_RawPool_t *pstRawPool,
													HUINT16 usTsUniqId,
													HUINT32 ulIdx,
													HUINT32 ulTag,
													HUINT32 nDesIdx,
													HUINT32 *pulDesRawSize,
													HUINT8 **ppucDesRaw)
{
	HUINT32		 ulDesRawSize;
	HUINT8		*pucDesRaw;
	HERROR		 hErr;

	HxLOG_Info("[svc_cas_SiGetPmtProgramDesRaw] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	hErr = svc_cas_SiGetPmtProgramDesPtr (pstRawPool, usTsUniqId, ulIdx, ulTag, nDesIdx, &ulDesRawSize, &pucDesRaw);
	if (hErr == ERR_CAS_SI_RESULT_OK)
	{
		hErr = svc_cas_SiDuplicateDesRaw (ulDesRawSize, pucDesRaw, pulDesRawSize, ppucDesRaw);
	}
	HxLOG_Info("[svc_cas_SiGetPmtProgramDesRaw] (-)\n" );

	return 	hErr;
}

HERROR svc_cas_SiGetPmtProgramAllDesRaw(svcCas_RawPool_t *pstRawPool,
													HUINT16 usTsUniqId,
													HUINT32 ulIdx,
													HUINT32 *pulDesRawSize,
													HUINT8 **ppucDesRaw)
{
	HUINT32		 ulDesRawSize;
	HUINT8		*pucDesRaw;
	HERROR		 hErr;

	HxLOG_Info("[svc_cas_SiGetPmtProgramDesRaw] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	hErr = svc_cas_SiGetPmtProgramAllDesPtr (pstRawPool, usTsUniqId, ulIdx, &ulDesRawSize, &pucDesRaw);
	if (hErr == ERR_CAS_SI_RESULT_OK)
	{
		hErr = svc_cas_SiDuplicateDesRaw (ulDesRawSize, pucDesRaw, pulDesRawSize, ppucDesRaw);
	}
	HxLOG_Info("[svc_cas_SiGetPmtProgramDesRaw] (-)\n" );

	return 	hErr;
}

HERROR svc_cas_SiCountPmtElementFromRaw(HUINT8 *pucRaw, HUINT32 *pnCnt)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiCountPmtElementFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	ulRet = svc_cas_SiCountPmtElementFromRawInternal(pucRaw, pnCnt);
	HxLOG_Info("[svc_cas_SiCountPmtElementFromRaw] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiCountPmtElement(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 *pnCnt)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiCountPmtElement] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	ulRet = svc_cas_SiCountPmtElementInternal(pstRawPool, usTsUniqId, ulIdx, pnCnt);
	HxLOG_Info("[svc_cas_SiCountPmtElement] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiGetPmtElementInfoFromRaw(HUINT8 *pucRaw, HUINT32 ulElemIdx, svcCas_SiPmtElementInfo_t *pstInfo)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiGetPmtElementInfoFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	ulRet = svc_cas_SiGetPmtElementInfoFromRawInternal(pucRaw, ulElemIdx, pstInfo);
	HxLOG_Info("[svc_cas_SiGetPmtElementInfoFromRaw] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiGetPmtElementInfo(svcCas_RawPool_t *pstRawPool,
											HUINT16 usTsUniqId,
											HUINT32 ulIdx,
											HUINT32 nSubIdx,
											svcCas_SiPmtElementInfo_t *pstInfo)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiGetPmtElementInfo] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	ulRet = svc_cas_SiGetPmtElementInfoInternal(pstRawPool, usTsUniqId, ulIdx, nSubIdx, pstInfo);
	HxLOG_Info("[svc_cas_SiGetPmtElementInfo] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiCountPmtElementDescriptorFromRaw(HUINT8 *pucRaw,
													HUINT32 ulElemIdx,
													HUINT32 ulTag,
													HUINT32 *pnDesCnt)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiCountPmtElementDescriptorFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	ulRet = svc_cas_SiCountPmtElementDescriptorFromRawInternal(pucRaw, ulElemIdx, ulTag, pnDesCnt);
	HxLOG_Info("[svc_cas_SiCountPmtElementDescriptorFromRaw] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiCountPmtElementDescriptor(svcCas_RawPool_t *pstRawPool,
													HUINT16 usTsUniqId,
													HUINT32 ulIdx,
													HUINT32 nSubIdx,
													HUINT32 ulTag,
													HUINT32 *pnDesCnt)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiCountPmtElementDescriptor] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	ulRet = svc_cas_SiCountPmtElementDescriptorInternal(pstRawPool, usTsUniqId, ulIdx, nSubIdx, ulTag, pnDesCnt);
	HxLOG_Info("[svc_cas_SiCountPmtElementDescriptor] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiGetPmtElementDescriptorTagFromRaw(HUINT8 *pucRaw,
														HUINT32 ulElemIdx,
														HUINT32 ulDesIdx,
														HUINT8 *pucTag)
{
	HUINT32		 ulDesRawSize;
	HUINT8		*pucDesRaw;
	HERROR		 hErr;

	if (pucTag == NULL)					{ return ERR_CAS_SI_TARGET_NULL; }

	HxLOG_Info("[svc_cas_SiGetPmtElementDescriptorTagFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	hErr = svc_cas_SiGetPmtElementDesPtrFromRaw (pucRaw, ulElemIdx, TAG_ALL_DESCRIPTORS, ulDesIdx, &ulDesRawSize, &pucDesRaw);
	if (hErr == ERR_CAS_SI_RESULT_OK)
	{
		*pucTag = *pucDesRaw;
	}
	HxLOG_Info("[svc_cas_SiGetPmtElementDescriptorTagFromRaw] (-)\n" );

	return hErr;
}

HERROR svc_cas_SiGetPmtElementDescriptorTag(svcCas_RawPool_t *pstRawPool,
													HUINT16 usTsUniqId,
													HUINT32 ulIdx,
													HUINT32 ulSubIdx,
													HUINT32 ulDesIdx,
													HUINT8 *pucTag)
{
	HUINT32		 ulDesRawSize;
	HUINT8		*pucDesRaw;
	HERROR		 hErr;

	if (pucTag == NULL)					{ return ERR_CAS_SI_TARGET_NULL; }

	HxLOG_Info("[svc_cas_SiGetPmtElementDescriptorTag] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	hErr = svc_cas_SiGetPmtElementDesPtr (pstRawPool, usTsUniqId, ulIdx, ulSubIdx, TAG_ALL_DESCRIPTORS, ulDesIdx, &ulDesRawSize, &pucDesRaw);
	if (hErr == ERR_CAS_SI_RESULT_OK)
	{
		*pucTag = *pucDesRaw;
	}
	HxLOG_Info("[svc_cas_SiGetPmtElementDescriptorTag] (-)\n");

	return hErr;
}

HERROR svc_cas_SiGetPmtElementDescriptorFromRaw(HUINT8 *pucRaw,
													SIxTextEncoding_e eTextEnc,
													HUINT32 ulElemIdx,
													HUINT32 ulTag,
													HUINT32 nDesIdx,
													void **ppvDes)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiGetPmtElementDescriptorFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	ulRet = svc_cas_SiGetPmtElementDescriptorFromRawInternal(pucRaw, eTextEnc, ulElemIdx, ulTag, nDesIdx, ppvDes);
	HxLOG_Info("[svc_cas_SiGetPmtElementDescriptorFromRaw] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiGetPmtElementDescriptor(svcCas_RawPool_t *pstRawPool,
													SIxTextEncoding_e eTextEnc,
													HUINT16 usTsUniqId,
													HUINT32 ulIdx,
													HUINT32 nSubIdx,
													HUINT32 ulTag,
													HUINT32 nDesIdx,
													void **ppvDes)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiGetPmtElementDescriptor] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	ulRet = svc_cas_SiGetPmtElementDescriptorInternal(pstRawPool, eTextEnc, usTsUniqId, ulIdx, nSubIdx, ulTag, nDesIdx, ppvDes);
	HxLOG_Info("[svc_cas_SiGetPmtElementDescriptor] (-)\n");

	return ulRet;
}

#if 0
HERROR svc_cas_SiGetPmtElementExtDescriptor(svcCas_RawPool_t *pstRawPool,
											HUINT16 usTsUniqId,
											HUINT32 ulIdx,
											HUINT32 nSubIdx,
											HUINT32 ulExtTag,
											HUINT32 nDesExtIdx,
											void **ppvDes)
{
	HUINT32			ulRet;
	svcCas_ExtensionDes_t		*pExtensionDes;
	HUINT32			extCnt, i;
	HUINT8 			*ucExtDesPtr;
	HUINT32			extTagIdx = 0;

	extCnt = 0;
	ulRet = svc_cas_SiCountPmtElementDescriptorInternal(pstRawPool, usTsUniqId, ulIdx, nSubIdx, TAG_EXTENSION, &extCnt);
	for (i = 0; i < extCnt; i++)
	{
		pExtensionDes = NULL;
		ulRet = svc_cas_SiGetPmtElementDescriptorInternal(pstRawPool, usTsUniqId, ulIdx, nSubIdx, TAG_EXTENSION, i, (void **)&pExtensionDes);
		if ((ulRet == ERR_CAS_SI_RESULT_OK) && (pExtensionDes != NULL))
		{
			if (pExtensionDes->ucExtDesTag != ulExtTag)
			{
				svc_si_DesFree(pExtensionDes);
				continue;
			}
			if (extTagIdx != nDesExtIdx)
			{
				svc_si_DesFree(pExtensionDes);
				continue;
			}
			extTagIdx++;

			ucExtDesPtr =  svc_si_DesParseExtDes(pExtensionDes->ucExtDesTag, pExtensionDes->ucExtDesLen, pExtensionDes->pExtDesData);
			svc_si_DesFree(pExtensionDes);

			if (ucExtDesPtr == NULL)
			{
				continue;
			}

			*ppvDes = (void *) ucExtDesPtr;
			return ERR_OK;

		}
	}

	return ERR_FAIL;
}
#endif

HERROR svc_cas_SiGetPmtElementDesRawFromRaw(HUINT8 *pucRaw,
												HUINT32 ulElemIdx,
												HUINT32 ulTag,
												HUINT32 nDesIdx,
												HUINT32 *pulDesRawSize,
												HUINT8 **ppucDesRaw)
{
	HUINT32		 ulDesRawSize;
	HUINT8		*pucDesRaw;
	HERROR		 hErr;

	HxLOG_Info("[svc_cas_SiGetPmtElementDesRawFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	hErr = svc_cas_SiGetPmtElementDesPtrFromRaw (pucRaw, ulElemIdx, ulTag, nDesIdx, &ulDesRawSize, &pucDesRaw);
	if (hErr == ERR_CAS_SI_RESULT_OK)
	{
		hErr = svc_cas_SiDuplicateDesRaw (ulDesRawSize, pucDesRaw, pulDesRawSize, ppucDesRaw);
	}
	HxLOG_Info("[svc_cas_SiGetPmtElementDesRawFromRaw] (-)\n");

	return 	hErr;
}

HERROR svc_cas_SiGetPmtElementAllDesRawFromRaw(HUINT8 *pucRaw,
													HUINT32 ulElemIdx,
													HUINT32 *pulDesRawSize,
													HUINT8 **ppucDesRaw)
{
	HUINT32		 ulDesRawSize;
	HUINT8		*pucDesRaw;
	HERROR		 hErr;

	HxLOG_Info("[svc_cas_SiGetPmtElementAllDesRawFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	hErr = svc_cas_SiGetPmtElementAllDesPtrFromRaw (pucRaw, ulElemIdx, &ulDesRawSize, &pucDesRaw);
	if (hErr == ERR_CAS_SI_RESULT_OK)
	{
		hErr = svc_cas_SiDuplicateDesRaw (ulDesRawSize, pucDesRaw, pulDesRawSize, ppucDesRaw);
	}
	HxLOG_Info("[svc_cas_SiGetPmtElementAllDesRawFromRaw] (-)\n");

	return 	hErr;
}

HERROR svc_cas_SiGetPmtElementDesRaw(svcCas_RawPool_t *pstRawPool,
													HUINT16 usTsUniqId,
													HUINT32 ulIdx,
													HUINT32 nSubIdx,
													HUINT32 ulTag,
													HUINT32 ulDesIdx,
													HUINT32 *pulDesRawSize,
													HUINT8 **ppucDesRaw)
{
	HUINT32		 ulDesRawSize;
	HUINT8		*pucDesRaw;
	HERROR		 hErr;

	HxLOG_Info("[svc_cas_SiGetPmtElementDesRaw] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	hErr = svc_cas_SiGetPmtElementDesPtr (pstRawPool, usTsUniqId, ulIdx, nSubIdx, ulTag, ulDesIdx, &ulDesRawSize, &pucDesRaw);
	if (hErr == ERR_CAS_SI_RESULT_OK)
	{
		hErr = svc_cas_SiDuplicateDesRaw (ulDesRawSize, pucDesRaw, pulDesRawSize, ppucDesRaw);
	}
	HxLOG_Info("[svc_cas_SiGetPmtElementDesRaw] (-)\n");

	return 	hErr;
}

HERROR svc_cas_SiGetPmtElementAllDesRaw(svcCas_RawPool_t *pstRawPool,
													HUINT16 usTsUniqId,
													HUINT32 ulIdx,
													HUINT32 nSubIdx,
													HUINT32 *pulDesRawSize,
													HUINT8 **ppucDesRaw)
{
	HUINT32		 ulDesRawSize;
	HUINT8		*pucDesRaw;
	HERROR		 hErr;

	HxLOG_Info("[svc_cas_SiGetPmtElementDesRaw] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	hErr = svc_cas_SiGetPmtElementAllDesPtr (pstRawPool, usTsUniqId, ulIdx, nSubIdx, &ulDesRawSize, &pucDesRaw);
	if (hErr == ERR_CAS_SI_RESULT_OK)
	{
		hErr = svc_cas_SiDuplicateDesRaw (ulDesRawSize, pucDesRaw, pulDesRawSize, ppucDesRaw);
	}
	HxLOG_Info("[svc_cas_SiGetPmtElementDesRaw] (-)\n");

	return 	hErr;
}


#define ___________PMT_SVCID____________

STATIC HERROR svc_cas_SiGetPmtProgNumFromRawInternal(HUINT8 *pucRaw, HUINT16 *pusProgNum)
{
	if (pucRaw == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pusProgNum == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	if (pucRaw[0] != PMT_TABLE_ID)	{ return ERR_CAS_SI_SOURCE_INVALID; }

	*pusProgNum = HxMACRO_Get16Bit(pucRaw + 3);
	return ERR_CAS_SI_RESULT_OK;
}

STATIC HERROR svc_cas_SiGetPmtIndexByServiceIdInternal(svcCas_RawPool_t *pstRawPool,
														HUINT16 usTsUniqId,
														HUINT16 usSvcId,
														HUINT32 *pulIdx)
{
	HUINT32				 ulSecIdx, ulSecCnt;
	svcCas_SiPmtProgramInfo_t	 stInfo;
	HERROR				 hErr;

	if (pstRawPool == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulIdx == NULL)					{ return ERR_CAS_SI_TARGET_NULL; }

	hErr = svc_cas_SiCountPmtSectionInternal(pstRawPool, usTsUniqId, &ulSecCnt);
	if (hErr != ERR_CAS_SI_RESULT_OK)		{ ulSecCnt = 0; }

	for (ulSecIdx = 0; ulSecIdx < ulSecCnt; ulSecIdx++)
	{
		hErr = svc_cas_SiGetPmtProgramInfoInternal(pstRawPool, usTsUniqId, ulSecIdx, &stInfo);
		if (hErr == ERR_CAS_SI_RESULT_OK && usSvcId == stInfo.usProgNum)
		{
			*pulIdx = ulSecIdx;
			return ERR_CAS_SI_RESULT_OK;
		}
	}

	return ERR_CAS_SI_RESULT_FAIL;
}

HERROR svc_cas_SiGetPmtProgNumFromRaw(HUINT8 *pucRaw, HUINT16 *pusProgNum)
{
	HERROR		 hErr;

	HxLOG_Info("[svc_cas_SiGetPmtProgNumFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	hErr = svc_cas_SiGetPmtProgNumFromRawInternal(pucRaw, pusProgNum);
	HxLOG_Info("[svc_cas_SiGetPmtProgNumFromRaw] (-)\n");

	return hErr;
}

HERROR svc_cas_SiGetPmtIndexByServiceId(svcCas_RawPool_t *pstRawPool,
													HUINT16 usTsUniqId,
													HUINT16 usSvcId,
													HUINT32 *pulPmtIndex)
{
	HERROR		 hErr;

	HxLOG_Info("[svc_cas_SiGetPmtIndexByServiceId] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	hErr = svc_cas_SiGetPmtIndexByServiceIdInternal(pstRawPool, usTsUniqId, usSvcId, pulPmtIndex);
	HxLOG_Info("[svc_cas_SiGetPmtIndexByServiceId] (-)\n");

	return hErr;
}

HERROR svc_cas_SiGetPmtProgramInfoByServiceId(svcCas_RawPool_t *pstRawPool,
												HUINT16 usTsUniqId,
												HUINT16 usSvcId,
												svcCas_SiPmtProgramInfo_t *pstInfo)
{
	HUINT32		 ulPmtIndex;
	HERROR		 hErr;

	HxLOG_Info("[svc_cas_SiGetPmtProgramInfoByServiceId] (+) Ts Unique Id = 0x%x\n", usTsUniqId);

	hErr = svc_cas_SiGetPmtIndexByServiceIdInternal(pstRawPool, usTsUniqId, usSvcId, &ulPmtIndex);
	if (hErr == ERR_CAS_SI_RESULT_OK)
	{
		hErr = svc_cas_SiGetPmtProgramInfoInternal(pstRawPool, usTsUniqId, ulPmtIndex, pstInfo);
	}

	HxLOG_Info("[svc_cas_SiGetPmtProgramInfoByServiceId] (-)\n");

	return hErr;
}


#define ___________SDT_INDEX____________

STATIC INLINE void svc_cas_SiGetFirstServicePos(HUINT8 *pucSection, HUINT8 **ppFirstService)
{
	*ppFirstService = pucSection + 11;
}

STATIC INLINE void  svc_cas_SiGetNextServicePos(HUINT8 *pucService, HUINT8 **ppNextService, HUINT32 ulDesLooplenth)
{
	*ppNextService = pucService + 5 + ulDesLooplenth;
}

STATIC INLINE void svc_cas_SiGetFirstSdtDescriptorPos(HUINT8 *pucService, HUINT8 **ppFirstSdtDescriptor)
{
	*ppFirstSdtDescriptor = pucService + 5;
}

STATIC INLINE void svc_cas_SiGetNextSdtDescriptorPos(HUINT8 *pucSdtDescriptor, HUINT8 **ppNextSdtDescriptor, HUINT32 ulDesLenth)
{
	*ppNextSdtDescriptor = pucSdtDescriptor + 2 + ulDesLenth;
}

STATIC HUINT8 *svc_cas_SiFindSdtSectionTable(svcCas_RawPool_t *pstRawPool,
													HUINT16 usTsUniqId,
													HUINT32 ulHead,
													HUINT32 ulIdx)
{
	HUINT16			 usAllUniqueId, usCount;
	HUINT16			 usRawIndex;
	HUINT32			 ulSdtSecCnt;
	HUINT8			*pucRaw;
	svcCas_RawSecTable_t	*pstTable;

	if (pstRawPool == NULL)						{ return NULL; }

	usAllUniqueId = DB_SVC_GetAllUniqueId();
	pstTable = svc_cas_RawGetRawTable (pstRawPool, eSI_TableType_SDT_ACT + ulHead);
	if (pstTable == NULL)						{ return NULL; }

	ulSdtSecCnt = 0;
	for (usCount = pstTable->usStartPos; usCount < pstTable->usInsertPos; usCount++)
	{
		if (pstTable->phIdx[usCount] == INVALID_INDEX_HANDLE)				{ continue; }

		// If there is no match in the unique ID, then it shall be skipped.
		if ((usTsUniqId != usAllUniqueId)
			&& (usTsUniqId != MACRO_GET_UNIQUEID_FROM_IDXHANDLE(pstTable->phIdx[usCount])))				{ continue; }

		if (ulSdtSecCnt >= ulIdx)
		{
			usRawIndex = MACRO_GET_INDEX_FROM_IDXHANDLE(pstTable->phIdx[usCount]);
			pucRaw = svc_cas_RawGetRawSection (pstRawPool, (HUINT32)usRawIndex);
			if (pucRaw != NULL)
			{
				return pucRaw;
			}
		}
		else
		{
			ulSdtSecCnt++;
		}
	}

	return NULL;
}

#if 0
STATIC HERROR svc_cas_SiCountSdtSectionInternal(svcCas_RawPool_t *pstRawPool,
												HUINT16 usTsUniqId,
												HUINT32 ulHead,
												HUINT32 *pulCnt)
{
	HUINT16			 usAllUniqueId, usCount;
	HUINT32			 ulSdtSecCnt;
	svcCas_RawSecTable_t	*pstTable;

	if (pstRawPool == NULL)			{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulCnt == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	usAllUniqueId = DB_SVC_GetAllUniqueId();
	pstTable = svc_cas_RawGetRawTable (pstRawPool, eSI_TableType_SDT_ACT + ulHead);

	ulSdtSecCnt = 0;
	for (usCount = pstTable->usStartPos; usCount < pstTable->usInsertPos; usCount++)
	{
		if (pstTable->phIdx[usCount] == INVALID_INDEX_HANDLE)				{ continue; }

		// If there is no match in the unique ID, then it shall be skipped.
		if ((usTsUniqId != usAllUniqueId)
			&& (usTsUniqId != MACRO_GET_UNIQUEID_FROM_IDXHANDLE(pstTable->phIdx[usCount])))				{ continue; }

		ulSdtSecCnt++;
	}

	*pulCnt = ulSdtSecCnt;
	HxLOG_Print("Sdt section count = %d\n", ulSdtSecCnt);

	return ERR_CAS_SI_RESULT_OK;
}
#endif

STATIC HERROR svc_cas_SiGetSdtSectionInfoFromRawInternal(HUINT8 *pucRaw, svcCas_SiSdtSectionInfo_t *pstInfo)
{
	HUINT16		 nSecLen;

	if (pucRaw == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pstInfo == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	HxSTD_memset (pstInfo, 0, sizeof(svcCas_SiSdtSectionInfo_t));
	nSecLen = HxMACRO_Get12Bit(pucRaw + 1);

	if (nSecLen < 5)
	{
		HxLOG_Critical("\n\n\n");
		return ERR_CAS_SI_RESULT_FAIL;
	}

	pstInfo->usTsId		 = HxMACRO_Get16Bit(pucRaw + 3);
	pstInfo->ucVer		 = (pucRaw[5] & 0x3e) >> 1;
	pstInfo->usOrgNetId	 = HxMACRO_Get16Bit(pucRaw + 8);
	pstInfo->ulCrc32	 = HxMACRO_Get32Bit(pucRaw + nSecLen - 1);			// +3 (After the length) - 4 (CRC) = -1

	HxLOG_Print("Sdt TsId = 0x%x\n", pstInfo->usTsId);
	HxLOG_Print("Sdt ucVer = 0x%x\n", pstInfo->ucVer);
	HxLOG_Print("Sdt usOrgNetId = 0x%x\n", pstInfo->usOrgNetId);
	HxLOG_Print("Sdt ulCrc32 = 0x%x\n", pstInfo->ulCrc32);

	return ERR_CAS_SI_RESULT_OK;
}

STATIC HERROR svc_cas_SiGetSdtSectionInfoInternal(svcCas_RawPool_t *pstRawPool,
												HUINT16 usTsUniqId,
												HUINT32 ulHead,
												HUINT32 ulIdx,
												svcCas_SiSdtSectionInfo_t *pstInfo)
{
	HUINT8		*pucRaw;

	if (pstRawPool == NULL)			{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pstInfo == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindSdtSectionTable(pstRawPool, usTsUniqId, ulHead, ulIdx);
	return svc_cas_SiGetSdtSectionInfoFromRawInternal(pucRaw, pstInfo);
}

STATIC HERROR svc_cas_SiCountSdtServiceFromRawInternal(HUINT8 *pucRaw, HUINT32 *pulCnt)
{
	HUINT16		 usDesLoopLen;
	HUINT32		 ulSvcCnt;
	HINT32		 nSecLen;
	HUINT8		*pucSvcInfo;

	if (pucRaw == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulCnt == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	ulSvcCnt = 0;
	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (8 + 4); 									// 나머지 table fields 와 CRC 제거

	svc_cas_SiGetFirstServicePos (pucRaw, &pucSvcInfo);
	while (nSecLen > 0)
	{
		if (nSecLen < 5)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_CAS_SI_RESULT_FAIL;
		}

		ulSvcCnt++;

		usDesLoopLen = HxMACRO_Get12Bit(pucSvcInfo + 3);
		svc_cas_SiGetNextServicePos (pucSvcInfo, &pucSvcInfo, (HUINT32)usDesLoopLen);
		nSecLen -= (usDesLoopLen + 5);
	}

	*pulCnt = ulSvcCnt;
	HxLOG_Print("Sdt service count = %d\n", ulSvcCnt);

	return ERR_CAS_SI_RESULT_OK;
}

STATIC HERROR svc_cas_SiCountSdtServiceInternal(svcCas_RawPool_t *pstRawPool,
												HUINT16 usTsUniqId,
												HUINT32 ulHead,
												HUINT32 ulIdx,
												HUINT32 *pulCnt)
{
	HUINT8		*pucRaw;

	if (pstRawPool == NULL)			{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulCnt == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindSdtSectionTable(pstRawPool, usTsUniqId, ulHead, ulIdx);
	return svc_cas_SiCountSdtServiceFromRawInternal(pucRaw, pulCnt);
}

STATIC HERROR svc_cas_SiGetSdtServiceInfoFromRawInternal(HUINT8 *pucRaw,
															HUINT32 ulSvcIdx,
															svcCas_SiSdtServiceInfo_t *pstInfo)
{
	HUINT16		 usDesLoopLen;
	HUINT32		 ulSvcCnt;
	HINT32		 nSecLen;
	HUINT8		*pucSvcInfo;

	if (pucRaw == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pstInfo == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	ulSvcCnt = 0;
	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (8 + 4); 									// 나머지 table fields 와 CRC 제거

	svc_cas_SiGetFirstServicePos (pucRaw, &pucSvcInfo);
	while (nSecLen > 0)
	{
		if (nSecLen < 5)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_CAS_SI_RESULT_FAIL;
		}

		if (ulSvcCnt != ulSvcIdx)
		{
			ulSvcCnt++;
			usDesLoopLen = HxMACRO_Get12Bit(pucSvcInfo + 3);
			svc_cas_SiGetNextServicePos (pucSvcInfo, &pucSvcInfo, (HUINT32)usDesLoopLen);
			nSecLen -= (usDesLoopLen + 5);
			continue;
		}

		HxSTD_memset (pstInfo, 0, sizeof(svcCas_SiSdtServiceInfo_t));
		pstInfo->usSvcId		 = HxMACRO_Get16Bit(pucSvcInfo);
		pstInfo->bEitSched		 = (pucSvcInfo[2] & 0x02) >> 1;
		pstInfo->bEitPreFollow	 = (pucSvcInfo[2] & 0x01);
		pstInfo->ucRunningStatus = (pucSvcInfo[3] & 0xe0) >> 5;
		pstInfo->bCaFree		 = (pucSvcInfo[3] & 0x10) >> 4;

		HxLOG_Print("Sdt SvcId = 0x%x\n", pstInfo->usSvcId);
		HxLOG_Print("Sdt bEitSched = 0x%x\n", pstInfo->bEitSched);
		HxLOG_Print("Sdt bEitPreFollow = 0x%x\n", pstInfo->bEitPreFollow);
		HxLOG_Print("Sdt ucRunningStatus = 0x%x\n", pstInfo->ucRunningStatus);
		HxLOG_Print("Sdt bCaFree = 0x%x\n", pstInfo->bCaFree);

		return ERR_CAS_SI_RESULT_OK;
	}

	return  ERR_FAIL;
}


STATIC HERROR svc_cas_SiGetSdtServiceInfoInternal(svcCas_RawPool_t *pstRawPool,
												HUINT16 usTsUniqId,
												HUINT32 ulHead,
												HUINT32 ulIdx,
												HUINT32 ulSubIdx,
												svcCas_SiSdtServiceInfo_t *pstInfo)
{
	HUINT8		*pucRaw;

	if (pstRawPool == NULL)			{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pstInfo == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindSdtSectionTable(pstRawPool, usTsUniqId, ulHead, ulIdx);
	return svc_cas_SiGetSdtServiceInfoFromRawInternal(pucRaw, ulSubIdx, pstInfo);
}

STATIC HERROR svc_cas_SiCountSdtServiceDescriptorFromRawInternal(HUINT8 *pucRaw,
																		HUINT32 ulSvcIdx,
																		HUINT32 ulTag,
																		HUINT32 *pnDesCnt)
{
	HUINT8		 ucDesTag, ucDesLen;
	HUINT32		 ulSvcCnt, ulDesCnt;
	HINT32		 nSecLen, nDesLoopLen;
	HUINT8		*pucSvcInfo, *pucSvcDes;

	if (pucRaw == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pnDesCnt == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	ulSvcCnt = 0;
	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (8 + 4); 									// 나머지 table fields 와 CRC 제거

	svc_cas_SiGetFirstServicePos (pucRaw, &pucSvcInfo);
	while (nSecLen > 0)
	{
		if (nSecLen < 5)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_CAS_SI_RESULT_FAIL;
		}

		nDesLoopLen = (HINT32)HxMACRO_Get12Bit(pucSvcInfo + 3);
		nSecLen -= (nDesLoopLen + 5);
		if (nSecLen < 0)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_CAS_SI_RESULT_FAIL;
		}

		if (ulSvcCnt != ulSvcIdx)
		{
			ulSvcCnt++;
			svc_cas_SiGetNextServicePos (pucSvcInfo, &pucSvcInfo, (HUINT32)nDesLoopLen);
			continue;
		}

		svc_cas_SiGetFirstSdtDescriptorPos (pucSvcInfo, &pucSvcDes);

		ulDesCnt = 0;
		// From here, there are SDT descriptors
		while (nDesLoopLen > 0)
		{
			if (nDesLoopLen < 2)
			{
				HxLOG_Critical("\n\n\n");
				return ERR_CAS_SI_RESULT_FAIL;
			}

			ucDesTag = HxMACRO_Get8Bit(pucSvcDes);
			ucDesLen = HxMACRO_Get8Bit(pucSvcDes + 1);

			nDesLoopLen -= (ucDesLen + 2);
			if (nDesLoopLen < 0)
			{
				HxLOG_Critical("\n\n\n");
				return ERR_CAS_SI_RESULT_FAIL;
			}

			svc_cas_SiGetNextSdtDescriptorPos (pucSvcDes, &pucSvcDes, (HUINT32)ucDesLen);
			if (svc_cas_SiIsSameDesTag ((HUINT32)ucDesTag, ulTag) == TRUE)			// Same TAG
			{
				ulDesCnt++;
			}
		}

		*pnDesCnt = ulDesCnt;
		HxLOG_Print("Sdt service Desctiptor count(tag = 0x%x) : %d\n", ulTag, ulDesCnt);

		return ERR_CAS_SI_RESULT_OK;
	}

	return ERR_CAS_SI_RESULT_FAIL;
}

#if 0
STATIC HERROR svc_cas_SiCountSdtServiceDescriptorInternal(svcCas_RawPool_t *pstRawPool,
															HUINT16 usTsUniqId,
															HUINT32 ulHead,
															HUINT32 ulIdx,
															HUINT32 ulSubIdx,
															HUINT32 ulTag,
															HUINT32 *pnDesCnt)
{
	HUINT8		*pucRaw;

	if (pstRawPool == NULL)			{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pnDesCnt == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindSdtSectionTable(pstRawPool, usTsUniqId, ulHead, ulIdx);
	return svc_cas_SiCountSdtServiceDescriptorFromRawInternal(pucRaw, ulSubIdx, ulTag, pnDesCnt);
}
#endif

STATIC HERROR svc_cas_SiGetSdtServiceDescriptorFromRawInternal(HUINT8 *pucRaw,
																SIxTextEncoding_e eTextEnc,
																HUINT32 ulSvcIdx,
																HUINT32 ulTag,
																HUINT32 ulDesIdx,
																void **ppvDes)
{
	HUINT8		 ucDesTag, ucDesLen;
	HUINT32		 ulSvcCnt, ulDesCnt;
	HINT32		 nSecLen, nDesLoopLen;
	HUINT8		*pucSvcInfo, *pucSvcDes;
	HUINT8		*pucDes;
	HERROR		 hErr;

	if (pucRaw == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (ppvDes == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	ulSvcCnt = 0;
	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (8 + 4); 									// 나머지 table fields 와 CRC 제거

	svc_cas_SiGetFirstServicePos (pucRaw, &pucSvcInfo);
	while (nSecLen > 0)
	{
		if (nSecLen < 5)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_CAS_SI_RESULT_FAIL;
		}

		nDesLoopLen = (HINT32)HxMACRO_Get12Bit(pucSvcInfo + 3);
		nSecLen -= (nDesLoopLen + 5);
		if (nSecLen < 0)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_CAS_SI_RESULT_FAIL;
		}

		if (ulSvcCnt != ulSvcIdx)
		{
			ulSvcCnt++;
			svc_cas_SiGetNextServicePos (pucSvcInfo, &pucSvcInfo, (HUINT32)nDesLoopLen);
			continue;
		}

		svc_cas_SiGetFirstSdtDescriptorPos (pucSvcInfo, &pucSvcDes);

		ulDesCnt = 0;
		// From here, there are SDT descriptors
		while (nDesLoopLen > 0)
		{
			if (nDesLoopLen < 2)
			{
				HxLOG_Critical("\n\n\n");
				return ERR_CAS_SI_RESULT_FAIL;
			}

			ucDesTag = HxMACRO_Get8Bit(pucSvcDes);
			ucDesLen = HxMACRO_Get8Bit(pucSvcDes + 1);

			nDesLoopLen -= (ucDesLen + 2);
			if (nDesLoopLen < 0)
			{
				HxLOG_Critical("\n\n\n");
				return ERR_CAS_SI_RESULT_FAIL;
			}

			// Different TAG
			if (svc_cas_SiIsSameDesTag ((HUINT32)ucDesTag, ulTag) != TRUE)
			{
				svc_cas_SiGetNextSdtDescriptorPos (pucSvcDes, &pucSvcDes, (HUINT32)ucDesLen);
				continue;
			}

			// Different Index
			if (ulDesIdx != ulDesCnt)
			{
				ulDesCnt++;
				svc_cas_SiGetNextSdtDescriptorPos (pucSvcDes, &pucSvcDes, (HUINT32)ucDesLen);
				continue;
			}

			pucDes = svc_cas_SiAllocDes(ucDesTag, ucDesLen);
			if (pucDes == NULL)							{ return ERR_CAS_SI_ALLOC_DESCRIPTOR_FAIL; }

			hErr = svc_cas_SiParseDes(eTextEnc, pucSvcDes, pucDes);
			if (hErr != ERR_CAS_SI_RESULT_OK)
			{
				svc_cas_SiFreeDes(pucDes);
				if (hErr == ERR_CAS_SI_INVALID_TAG || hErr == ERR_CAS_SI_INVALID_DESCRIPTOR)
				{
					svc_cas_SiGetNextSdtDescriptorPos (pucSvcDes, &pucSvcDes, (HUINT32)ucDesLen);
					continue;
				}
				else
				{
					return ERR_CAS_SI_CANNOT_FIND_DESCRIPTOR;
				}
			}

			*ppvDes = (void *)pucDes;
			return ERR_CAS_SI_RESULT_OK;
		}

		break;
	}

	return ERR_CAS_SI_RESULT_FAIL;
}

#if 0
STATIC HERROR svc_cas_SiGetSdtServiceDescriptorInternal(svcCas_RawPool_t *pstRawPool,
														SIxTextEncoding_e eTextEnc,
														HUINT16 usTsUniqId,
														HUINT32 ulHead,
														HUINT32 ulIdx,
														HUINT32 ulSubIdx,
														HUINT32 ulTag,
														HUINT32 nDesIdx,
														void **ppvDes)
{
	HUINT8		*pucRaw;

	if (pstRawPool == NULL)			{ return ERR_CAS_SI_SOURCE_NULL; }
	if (ppvDes == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindSdtSectionTable(pstRawPool, usTsUniqId, ulHead, ulIdx);
	return svc_cas_SiGetSdtServiceDescriptorFromRawInternal(pucRaw, eTextEnc, ulSubIdx, ulTag, nDesIdx, ppvDes);
}
#endif

STATIC HERROR svc_cas_SiGetSdtServiceDesPtrFromRaw(HUINT8 *pucRaw,
													HUINT32 ulSvcIdx,
													HUINT32 ulTag,
													HUINT32 ulDesIdx,
													HUINT32 *pulDesRawSize,
													HUINT8 **ppucDesRaw)
{
	HUINT8		 ucDesTag, ucDesLen;
	HUINT32		 ulSvcCnt, ulDesCnt;
	HINT32		 nSecLen, nDesLoopLen;
	HUINT8		*pucSvcInfo, *pucSvcDes;

	if (pucRaw == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulDesRawSize == NULL)		{ return ERR_CAS_SI_TARGET_NULL; }
	if (ppucDesRaw == NULL) 		{ return ERR_CAS_SI_TARGET_NULL; }

	ulSvcCnt = 0;
	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (8 + 4); 									// 나머지 table fields 와 CRC 제거

	svc_cas_SiGetFirstServicePos (pucRaw, &pucSvcInfo);
	while (nSecLen > 0)
	{
		if (nSecLen < 5)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_CAS_SI_RESULT_FAIL;
		}

		nDesLoopLen = (HINT32)HxMACRO_Get12Bit(pucSvcInfo + 3);
		nSecLen -= (nDesLoopLen + 5);
		if (nSecLen < 0)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_CAS_SI_RESULT_FAIL;
		}

		if (ulSvcCnt != ulSvcIdx)
		{
			ulSvcCnt++;
			svc_cas_SiGetNextServicePos (pucSvcInfo, &pucSvcInfo, (HUINT32)nDesLoopLen);
			continue;
		}

		svc_cas_SiGetFirstSdtDescriptorPos (pucSvcInfo, &pucSvcDes);

		ulDesCnt = 0;
		// From here, there are SDT descriptors
		while (nDesLoopLen > 0)
		{
			if (nDesLoopLen < 2)
			{
				HxLOG_Critical("\n\n\n");
				return ERR_CAS_SI_RESULT_FAIL;
			}

			ucDesTag = HxMACRO_Get8Bit(pucSvcDes);
			ucDesLen = HxMACRO_Get8Bit(pucSvcDes + 1);

			nDesLoopLen -= (ucDesLen + 2);
			if (nDesLoopLen < 0)
			{
				HxLOG_Critical("\n\n\n");
				return ERR_CAS_SI_RESULT_FAIL;
			}

			// Different TAG
			if (svc_cas_SiIsSameDesTag ((HUINT32)ucDesTag, ulTag) != TRUE)
			{
				svc_cas_SiGetNextSdtDescriptorPos (pucSvcDes, &pucSvcDes, (HUINT32)ucDesLen);
				continue;
			}

			// Different Index
			if (ulDesIdx != ulDesCnt)
			{
				ulDesCnt++;
				svc_cas_SiGetNextSdtDescriptorPos (pucSvcDes, &pucSvcDes, (HUINT32)ucDesLen);
				continue;
			}

			*pulDesRawSize	= (HUINT32)ucDesLen + 2;
			*ppucDesRaw 	= pucSvcDes;
			return ERR_CAS_SI_RESULT_OK;

		}

		break;
	}

	return ERR_CAS_SI_RESULT_FAIL;
}

STATIC HERROR svc_cas_SiGetSdtServiceDesPtr(svcCas_RawPool_t *pstRawPool,
														HUINT16 usTsUniqId,
														HUINT32 ulHead,
														HUINT32 ulIdx,
														HUINT32 ulSubIdx,
														HUINT32 ulTag,
														HUINT32 nDesIdx,
														HUINT32 *pulDesRawSize,
														HUINT8 **ppucDesRaw)
{
	HUINT8		*pucRaw;

	if (pstRawPool == NULL)			{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulDesRawSize == NULL)		{ return ERR_CAS_SI_TARGET_NULL; }
	if (ppucDesRaw == NULL) 		{ return ERR_CAS_SI_TARGET_NULL; }

	pucRaw = svc_cas_SiFindSdtSectionTable(pstRawPool, usTsUniqId, ulHead, ulIdx);
	return svc_cas_SiGetSdtServiceDesPtrFromRaw(pucRaw, ulSubIdx, ulTag, nDesIdx, pulDesRawSize, ppucDesRaw);
}

#if 0
HERROR svc_cas_SiCountSdtSection(svcCas_RawPool_t *pstRawPool,
										HUINT16 usTsUniqId,
										HUINT32 ulHead,
										HUINT32 *pulCnt)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiCountSdtSection] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	ulRet = svc_cas_SiCountSdtSectionInternal(pstRawPool, usTsUniqId, ulHead, pulCnt);
	HxLOG_Info("[svc_cas_SiCountSdtSection] (-)\n");

	return ulRet;
}
#endif

HERROR svc_cas_SiGetSdtSectionInfoFromRaw(HUINT8 *pucRaw, svcCas_SiSdtSectionInfo_t *pstInfo)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiGetSdtSectionInfoFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	ulRet = svc_cas_SiGetSdtSectionInfoFromRawInternal(pucRaw, pstInfo);
	HxLOG_Info("[svc_cas_SiGetSdtSectionInfoFromRaw] (-)\n");

	return ulRet;
}

#if 0
HERROR svc_cas_SiGetSdtSectionInfo(svcCas_RawPool_t *pstRawPool,
											HUINT16 usTsUniqId,
											HUINT32 ulHead,
											HUINT32 ulIdx,
											svcCas_SiSdtSectionInfo_t *pstInfo)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiGetSdtSectionInfo] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	ulRet = svc_cas_SiGetSdtSectionInfoInternal(pstRawPool, usTsUniqId, ulHead, ulIdx, pstInfo);
	HxLOG_Info("[svc_cas_SiGetSdtSectionInfo] (-)\n");

	return ulRet;
}
#endif

HERROR svc_cas_SiCountSdtServiceFromRaw(HUINT8 *pucRaw, HUINT32 *pulCnt)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiCountSdtServiceFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	ulRet = svc_cas_SiCountSdtServiceFromRawInternal(pucRaw, pulCnt);
	HxLOG_Info("[svc_cas_SiCountSdtServiceFromRaw] (-)\n");

	return ulRet;
}

#if 0
HERROR svc_cas_SiCountSdtService(svcCas_RawPool_t *pstRawPool,
										HUINT16 usTsUniqId,
										HUINT32 ulHead,
										HUINT32 ulIdx,
										HUINT32 *pulCnt)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiCountSdtService] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	ulRet = svc_cas_SiCountSdtServiceInternal(pstRawPool, usTsUniqId, ulHead, ulIdx, pulCnt);
	HxLOG_Info("[svc_cas_SiCountSdtService] (-)\n");

	return ulRet;
}
#endif

HERROR svc_cas_SiGetSdtServiceInfoFromRaw(HUINT8 *pucRaw, HUINT32 ulSvcIdx, svcCas_SiSdtServiceInfo_t *pstInfo)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiGetSdtServiceInfoFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	ulRet = svc_cas_SiGetSdtServiceInfoFromRawInternal(pucRaw, ulSvcIdx,pstInfo);
	HxLOG_Info("[svc_cas_SiGetSdtServiceInfoFromRaw] (-)\n");

	return ulRet;
}

#if 0
HERROR svc_cas_SiGetSdtServiceInfo(svcCas_RawPool_t *pstRawPool,
											HUINT16 usTsUniqId,
											HUINT32 ulHead,
											HUINT32 ulIdx,
											HUINT32 ulSubIdx,
											svcCas_SiSdtServiceInfo_t *pstInfo)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiGetSdtServiceInfo] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	ulRet = svc_cas_SiGetSdtServiceInfoInternal(pstRawPool, usTsUniqId, ulHead, ulIdx, ulSubIdx, pstInfo);
	HxLOG_Info("[svc_cas_SiGetSdtServiceInfo] (-)\n");

	return ulRet;
}
#endif

HERROR svc_cas_SiCountSdtServiceDescriptorFromRaw(HUINT8 *pucRaw,
																HUINT32 ulSvcIdx,
																HUINT32 ulTag,
																HUINT32 *pnDesCnt)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiCountSdtServiceDescriptorFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	ulRet = svc_cas_SiCountSdtServiceDescriptorFromRawInternal(pucRaw, ulSvcIdx, ulTag, pnDesCnt);
	HxLOG_Info("[svc_cas_SiCountSdtServiceDescriptorFromRaw] (-)\n");

	return ulRet;
}

#if 0
HERROR svc_cas_SiCountSdtServiceDescriptor(svcCas_RawPool_t *pstRawPool,
													HUINT16 usTsUniqId,
													HUINT32 ulHead,
													HUINT32 ulIdx,
													HUINT32 ulSubIdx,
													HUINT32 ulTag,
													HUINT32 *pnDesCnt)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiCountSdtServiceDescriptor] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	ulRet = svc_cas_SiCountSdtServiceDescriptorInternal(pstRawPool, usTsUniqId, ulHead, ulIdx, ulSubIdx, ulTag, pnDesCnt);
	HxLOG_Info("[svc_cas_SiCountSdtServiceDescriptor] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiGetSdtServiceDescriptorTagFromRaw(HUINT8 *pucRaw,
															HUINT32 ulSvcIdx,
															HUINT32 ulDesIdx,
															HUINT8 *pucTag)
{
	HUINT32		 ulDesRawSize;
	HUINT8		*pucDesRaw;
	HERROR		 hErr;

	if (pucTag == NULL)					{ return ERR_CAS_SI_TARGET_NULL; }

	HxLOG_Info("[svc_cas_SiGetSdtServiceDescriptorTagFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	hErr = svc_cas_SiGetSdtServiceDesPtrFromRaw (pucRaw, ulSvcIdx, TAG_ALL_DESCRIPTORS, ulDesIdx, &ulDesRawSize, &pucDesRaw);
	if (hErr == ERR_CAS_SI_RESULT_OK)
	{
		*pucTag = *pucDesRaw;
	}
	HxLOG_Info("[svc_cas_SiGetSdtServiceDescriptorTagFromRaw] (-)\n" );

	return hErr;
}

HERROR svc_cas_SiGetSdtServiceDescriptorTag(svcCas_RawPool_t *pstRawPool,
													HUINT16 usTsUniqId,
													HUINT32 ulHead,
													HUINT32 ulIdx,
													HUINT32 ulSubIdx,
													HUINT32 nDesIdx,
													HUINT8 *pucTag)
{
	HUINT32		 ulDesRawSize;
	HUINT8		*pucDesRaw;
	HERROR		 hErr;

	if (pucTag == NULL)					{ return ERR_CAS_SI_TARGET_NULL; }

	HxLOG_Info("[svc_cas_SiGetSdtServiceDescriptorTag] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	hErr = svc_cas_SiGetSdtServiceDesPtr(pstRawPool, usTsUniqId, ulHead, ulIdx, ulSubIdx, TAG_ALL_DESCRIPTORS, nDesIdx, &ulDesRawSize, &pucDesRaw);
	if (hErr == ERR_CAS_SI_RESULT_OK)
	{
		*pucTag = *pucDesRaw;
	}
	HxLOG_Info("[svc_cas_SiGetSdtServiceDescriptorTag] (-)\n");

	return hErr;
}
#endif

HERROR svc_cas_SiGetSdtServiceDescriptorFromRaw(HUINT8 *pucRaw,
													SIxTextEncoding_e eTextEnc,
													HUINT32 ulSvcIdx,
													HUINT32 ulTag,
													HUINT32 ulDesIdx,
													void **ppvDes)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiGetSdtServiceDescriptorFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	ulRet = svc_cas_SiGetSdtServiceDescriptorFromRawInternal(pucRaw, eTextEnc, ulSvcIdx, ulTag, ulDesIdx, ppvDes);
	HxLOG_Info("[svc_cas_SiGetSdtServiceDescriptorFromRaw] (-)\n");

	return ulRet;
}

#if 0
HERROR svc_cas_SiGetSdtServiceDescriptor(svcCas_RawPool_t *pstRawPool,
											SIxTextEncoding_e eTextEnc,
											HUINT16 usTsUniqId,
											HUINT32 ulHead,
											HUINT32 ulIdx,
											HUINT32 ulSubIdx,
											HUINT32 ulTag,
											HUINT32 nDesIdx,
											void **ppvDes)
{
	HUINT32		ulRet;

	HxLOG_Info("[svc_cas_SiGetSdtServiceDescriptor] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	ulRet = svc_cas_SiGetSdtServiceDescriptorInternal(pstRawPool, eTextEnc, usTsUniqId, ulHead, ulIdx, ulSubIdx, ulTag, nDesIdx, ppvDes);
	HxLOG_Info("[svc_cas_SiGetSdtServiceDescriptor] (-)\n");

	return ulRet;
}
#endif

HERROR svc_cas_SiGetSdtServiceDesRawFromRaw(HUINT8 *pucRaw,
											HUINT32 ulSvcIdx,
											HUINT32 ulTag,
											HUINT32 ulDesIdx,
											HUINT32 *pulDesRawSize,
											HUINT8 **ppucDesRaw)
{
	HUINT32		 ulDesRawSize;
	HUINT8		*pucDesRaw;
	HERROR		 hErr;

	HxLOG_Info("[svc_cas_SiGetSdtServiceDesRawFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	hErr = svc_cas_SiGetSdtServiceDesPtrFromRaw(pucRaw, ulSvcIdx, ulTag, ulDesIdx, &ulDesRawSize, &pucDesRaw);
	if (hErr == ERR_CAS_SI_RESULT_OK)
	{
		hErr = svc_cas_SiDuplicateDesRaw(ulDesRawSize, pucDesRaw, pulDesRawSize, ppucDesRaw);
	}
	HxLOG_Info("[svc_cas_SiGetSdtServiceDesRawFromRaw] (-)\n");

	return 	hErr;
}

HERROR svc_cas_SiGetSdtServiceDesRaw(svcCas_RawPool_t *pstRawPool,
													HUINT16 usTsUniqId,
													HUINT32 ulHead,
													HUINT32 ulIdx,
													HUINT32 ulSubIdx,
													HUINT32 ulTag,
													HUINT32 nDesIdx,
													HUINT32 *pulDesRawSize,
													HUINT8 **ppucDesRaw)
{
	HUINT32		 ulDesRawSize;
	HUINT8		*pucDesRaw;
	HERROR		 hErr;

	HxLOG_Info("[svc_cas_SiGetSdtServiceDesRaw] (+) Ts Unique Id = 0x%x\n", usTsUniqId);
	hErr = svc_cas_SiGetSdtServiceDesPtr(pstRawPool, usTsUniqId, ulHead, ulIdx, ulSubIdx, ulTag, nDesIdx, &ulDesRawSize, &pucDesRaw);
	if (hErr == ERR_CAS_SI_RESULT_OK)
	{
		hErr = svc_cas_SiDuplicateDesRaw (ulDesRawSize, pucDesRaw, pulDesRawSize, ppucDesRaw);
	}
	HxLOG_Info("[svc_cas_SiGetSdtServiceDesRaw] (-)\n");

	return 	hErr;
}


#define _____________________________________
#define _______SPECIFIC_FUNCTION__________
#define _____________________________________
/*
 * Specific function은 위에 구현 된 basic한 function 이외의 것을
 * 요구할 때 만들어 추가한다. 단 여기서 정의된 함수들은 silibapi.h
 * 에 정의된 SI 기본 structure을 사용하는 범위 내에서만 한정된다.
 */


/*
HERROR svc_cas_SiGetSdtServiceRunningStatus(HUINT16 usTsUniqId,
														HUINT32 ulHead,
														HUINT16 usTsId,
														HUINT16 usOrgNetId,
														HUINT16 usSvcId,
														HUINT8 *pucRunningStatus)
{


}
HERROR svc_cas_SiGetSdtEitFlag(HUINT16 usTsUniqId,
									HUINT32 ulHead,
									HUINT16 usTsId,
									HUINT16 usOrgNetId,
									HUINT16 usSvcId,
									HUINT8 *pucEitFlag)
{


}
*/

#define ________COMMON_RELATIVE________

STATIC INLINE HBOOL svc_cas_SiIsSameDesTag (HUINT32 ulSecDesTag, HUINT32 ulArgDesTag)
{
  	return ((ulArgDesTag == TAG_ALL_DESCRIPTORS) || ((ulArgDesTag & 0xff) == (ulSecDesTag & 0xff))) ? TRUE : FALSE;
}

STATIC HERROR svc_cas_SiGetDesTagFromDesIndexInternal(svcCas_SiDesIndex_t *pstDesIndex, HUINT32 *pulTag)
{
	HUINT8		 ucDesTag, ucDesLen;
	HINT32		 nLoopLen;
	HUINT8		*pucCurrPos;

	if (pstDesIndex == NULL)						{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulTag == NULL)								{ return ERR_CAS_SI_TARGET_NULL; }

	pucCurrPos	= pstDesIndex->pucCurrPos;
	nLoopLen	= pstDesIndex->nLeftDesLoop;

	if (pucCurrPos == NULL)								{ return ERR_CAS_SI_SOURCE_INVALID; }
	if (nLoopLen < 2)									{ return ERR_CAS_SI_CANNOT_FIND_DESCRIPTOR; }

	ucDesTag = HxMACRO_Get8Bit(pucCurrPos);
	ucDesLen = HxMACRO_Get8Bit(pucCurrPos + 1);

	if (nLoopLen < (HINT32)ucDesLen + 2)				{ return ERR_CAS_SI_CANNOT_FIND_DESCRIPTOR; }

	*pulTag = (HUINT32)ucDesTag;
	return ERR_CAS_SI_RESULT_OK;
}

STATIC HERROR svc_cas_SiGetDesFromDesIndexInternal(SIxTextEncoding_e eTextEnc, svcCas_SiDesIndex_t *pstDesIndex, HUINT32 ulTag, void **ppvDes, svcCas_SiDesIndex_t *pstNextDesIndex)
{
	HUINT8		 ucDesTag, ucDesLen;
	HINT32		 nLoopLen;
	HUINT8		*pucCurrPos, *pucDesPos;
	HUINT8		*pucDes;
	HERROR		 hErr;

	if (pstDesIndex == NULL)						{ return ERR_CAS_SI_SOURCE_NULL; }
	if (ppvDes == NULL)								{ return ERR_CAS_SI_TARGET_NULL; }

	pucCurrPos	= pstDesIndex->pucCurrPos;
	nLoopLen	= pstDesIndex->nLeftDesLoop;

	if (pucCurrPos == NULL)								{ return ERR_CAS_SI_SOURCE_INVALID; }
	if (nLoopLen < 2)									{ return ERR_CAS_SI_CANNOT_FIND_DESCRIPTOR; }

	// Check from the next descriptor :
	while (nLoopLen > 0)
	{
		pucDesPos = pucCurrPos;

		ucDesTag = HxMACRO_Get8Bit(pucDesPos);
		ucDesLen = HxMACRO_Get8Bit(pucDesPos + 1);

		if (nLoopLen < (HINT32)ucDesLen + 2)				{ return ERR_CAS_SI_CANNOT_FIND_DESCRIPTOR; }

		nLoopLen -= (2 + ucDesLen);
		pucCurrPos += (2 + ucDesLen);

		if (svc_cas_SiIsSameDesTag ((HUINT32)ucDesTag, ulTag) == TRUE)
		{
			pucDes = svc_cas_SiAllocDes(ucDesTag, ucDesLen);
			if (pucDes != NULL)
			{
				hErr = svc_cas_SiParseDes(eTextEnc, pucDesPos, pucDes);
				if (hErr == ERR_CAS_SI_RESULT_OK)
				{
					*ppvDes = (void *)pucDes;
					if (pstNextDesIndex != NULL)
					{
						pstNextDesIndex->pucCurrPos		= pucCurrPos;
						pstNextDesIndex->nLeftDesLoop	= nLoopLen;
					}

					return ERR_CAS_SI_RESULT_OK;
				}
				else
				{
					svc_cas_SiFreeDes(pucDes);			// Free and skip to the next item.
				}
			}
		}
	}

	return ERR_CAS_SI_CANNOT_FIND_DESCRIPTOR;
}

STATIC HERROR svc_cas_SiGetDesRawFromDesIndexInternal(svcCas_SiDesIndex_t *pstDesIndex, HUINT32 ulTag, HUINT32 *pulDesRawSize, HUINT8 **ppucDesRaw, svcCas_SiDesIndex_t *pstNextDesIndex)
{
	HUINT8		 ucDesTag, ucDesLen;
	HINT32		 nLoopLen;
	HUINT32		 ulRawSize;
	HUINT8		*pucCurrPos, *pucDesPos;
	HUINT8		*pucDes;

	if (pstDesIndex == NULL)							{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pulDesRawSize == NULL || ppucDesRaw == NULL)	{ return ERR_CAS_SI_TARGET_NULL; }

	pucCurrPos	= pstDesIndex->pucCurrPos;
	nLoopLen	= pstDesIndex->nLeftDesLoop;

	if (pucCurrPos == NULL)								{ return ERR_CAS_SI_SOURCE_INVALID; }
	if (nLoopLen < 2)									{ return ERR_CAS_SI_CANNOT_FIND_DESCRIPTOR; }

	// Check from the next descriptor :
	while (nLoopLen > 0)
	{
		pucDesPos = pucCurrPos;

		ucDesTag = HxMACRO_Get8Bit(pucDesPos);
		ucDesLen = HxMACRO_Get8Bit(pucDesPos + 1);

		if (nLoopLen < (HINT32)ucDesLen + 2)				{ return ERR_CAS_SI_CANNOT_FIND_DESCRIPTOR; }

		nLoopLen -= (2 + ucDesLen);
		pucCurrPos += (2 + ucDesLen);

		if (svc_cas_SiIsSameDesTag ((HUINT32)ucDesTag, ulTag) == TRUE)
		{
			ulRawSize = (HUINT32)ucDesLen + 2;
			pucDes =(HUINT8 *)OxCAS_Malloc (ulRawSize);
			if (pucDes == NULL)
			{
				return ERR_CAS_SI_NOT_ENOUGH_MEMORY;
			}

			HxSTD_memcpy (pucDes, pucDesPos, ulRawSize);

			*pulDesRawSize	= ulRawSize;
			*ppucDesRaw		= pucDes;
			return ERR_CAS_SI_RESULT_OK;
		}
	}

	return ERR_CAS_SI_CANNOT_FIND_DESCRIPTOR;
}

STATIC HERROR svc_cas_SiSetDesIndexToFirstDesInternal(svcCas_SiDesIndex_t *pstCurrDesIndex,
												HUINT32 ulTag,
												svcCas_SiDesIndex_t *pstNextDesIndex)
{
	HUINT8		 ucDesTag, ucDesLen;
	HINT32		 nLoopLen;
	HUINT8		*pucCurrPos;

	if (pstCurrDesIndex == NULL)						{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pstNextDesIndex == NULL)						{ return ERR_CAS_SI_TARGET_NULL; }

	pucCurrPos	= pstCurrDesIndex->pucCurrPos;
	nLoopLen	= pstCurrDesIndex->nLeftDesLoop;

	if (pucCurrPos == NULL)								{ return ERR_CAS_SI_SOURCE_INVALID; }
	if (nLoopLen < 2)									{ return ERR_CAS_SI_CANNOT_FIND_DESCRIPTOR; }

	// Check whether the tag is same or not from the next descriptors
	while (nLoopLen >= 2)		// At least, it shall be possible to read des_tag and des_len.
	{
		ucDesTag = HxMACRO_Get8Bit(pucCurrPos);
		ucDesLen = HxMACRO_Get8Bit(pucCurrPos + 1);

		if (nLoopLen < (HINT32)ucDesLen + 2)			{ return ERR_CAS_SI_CANNOT_FIND_DESCRIPTOR; }

		if (svc_cas_SiIsSameDesTag ((HUINT32)ucDesTag, ulTag) == TRUE)
		{
			pstNextDesIndex->pucCurrPos		= pucCurrPos;
			pstNextDesIndex->nLeftDesLoop	= nLoopLen;
			return ERR_CAS_SI_RESULT_OK;
		}

		nLoopLen -= (2 + ucDesLen);
		pucCurrPos += (2 + ucDesLen);
	}

	return ERR_CAS_SI_CANNOT_FIND_DESCRIPTOR;
}

STATIC HERROR svc_cas_SiSetDesIndexToNextDesInternal(svcCas_SiDesIndex_t *pstCurrDesIndex,
												HUINT32 ulTag,
												svcCas_SiDesIndex_t *pstNextDesIndex)
{
	HUINT8		 ucDesTag, ucDesLen;
	HINT32		 nLoopLen;
	HUINT8		*pucCurrPos;

	if (pstCurrDesIndex == NULL)						{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pstNextDesIndex == NULL)						{ return ERR_CAS_SI_TARGET_NULL; }

	pucCurrPos	= pstCurrDesIndex->pucCurrPos;
	nLoopLen	= pstCurrDesIndex->nLeftDesLoop;

	if (pucCurrPos == NULL)								{ return ERR_CAS_SI_SOURCE_INVALID; }
	if (nLoopLen < 2)									{ return ERR_CAS_SI_CANNOT_FIND_DESCRIPTOR; }

	// Go to the next descriptor.
	ucDesTag = HxMACRO_Get8Bit(pucCurrPos);
	ucDesLen = HxMACRO_Get8Bit(pucCurrPos + 1);

	if (nLoopLen < (HINT32)ucDesLen + 2)				{ return ERR_CAS_SI_CANNOT_FIND_DESCRIPTOR; }

	nLoopLen -= (2 + ucDesLen);
	pucCurrPos += (2 + ucDesLen);

	// Check whether the tag is same or not from the next descriptors
	while (nLoopLen > 2)		// At least, it shall be possible to read des_tag and des_len.
	{
		ucDesTag = HxMACRO_Get8Bit(pucCurrPos);
		ucDesLen = HxMACRO_Get8Bit(pucCurrPos + 1);

		if (nLoopLen < (HINT32)ucDesLen + 2)			{ return ERR_CAS_SI_CANNOT_FIND_DESCRIPTOR; }

		if (svc_cas_SiIsSameDesTag ((HUINT32)ucDesTag, ulTag) == TRUE)
		{
			pstNextDesIndex->pucCurrPos		= pucCurrPos;
			pstNextDesIndex->nLeftDesLoop	= nLoopLen;
			return ERR_CAS_SI_RESULT_OK;
		}

		nLoopLen -= (2 + ucDesLen);
		pucCurrPos += (2 + ucDesLen);
	}

	return ERR_CAS_SI_CANNOT_FIND_DESCRIPTOR;
}

HERROR svc_cas_SiGetDesTagFromDesIndex(svcCas_SiDesIndex_t *pstDesIndex, HUINT32 *pulTag)
{
	HUINT32		 ulRet;

	HxLOG_Info("[svc_cas_SiGetDesTagFromDesIndex] (+)\n");
	ulRet  = svc_cas_SiGetDesTagFromDesIndexInternal(pstDesIndex, pulTag);
	HxLOG_Info("[svc_cas_SiGetDesTagFromDesIndex] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiGetDesFromDesIndex(SIxTextEncoding_e eTextEnc,
												svcCas_SiDesIndex_t *pstDesIndex,
												HUINT32 ulTag,
												void **ppvDes,
												svcCas_SiDesIndex_t *pstNextDesIndex)
{
	HUINT32		 ulRet;

	HxLOG_Info("[svc_cas_SiGetDesFromDesIndex] (+)\n");
	ulRet  = svc_cas_SiGetDesFromDesIndexInternal(eTextEnc, pstDesIndex, ulTag, ppvDes, pstNextDesIndex);
	HxLOG_Info("[svc_cas_SiGetDesFromDesIndex] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiGetDesRawFromDesIndex(svcCas_SiDesIndex_t *pstDesIndex,
												HUINT32 ulTag,
												HUINT32 *pulDesRawSize,
												HUINT8 **ppucDesRaw,
												svcCas_SiDesIndex_t *pstNextDesIndex)
{
	HUINT32		 ulRet;

	HxLOG_Info("[svc_cas_SiGetDesRawFromDesIndex] (+)\n");
	ulRet  = svc_cas_SiGetDesRawFromDesIndexInternal(pstDesIndex, ulTag, pulDesRawSize, ppucDesRaw, pstNextDesIndex);
	HxLOG_Info("[svc_cas_SiGetDesRawFromDesIndex] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiSetDesIndexToFirstDes(svcCas_SiDesIndex_t *pstStartDesIndex,
												HUINT32 ulTag,
												svcCas_SiDesIndex_t *pstFirstDesIndex)
{
	HUINT32		 ulRet;

	HxLOG_Info("[svc_cas_SiSetDesIndexToFirstDes] (+)\n");
	ulRet  = svc_cas_SiSetDesIndexToFirstDesInternal(pstStartDesIndex, ulTag, pstFirstDesIndex);
	HxLOG_Info("[svc_cas_SiSetDesIndexToFirstDes] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiSetDesIndexToNextDes(svcCas_SiDesIndex_t *pstCurrDesIndex,
												HUINT32 ulTag,
												svcCas_SiDesIndex_t *pstNextDesIndex)
{
	HUINT32		 ulRet;

	HxLOG_Info("[svc_cas_SiSetDesIndexToNextDes] (+)\n");
	ulRet  = svc_cas_SiSetDesIndexToNextDesInternal(pstCurrDesIndex, ulTag, pstNextDesIndex);
	HxLOG_Info("[svc_cas_SiSetDesIndexToNextDes] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiSetBookmarkToFirstSection(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, SiTableType_e eTableType, svcCas_RawSecBookmark_t *pstSecBkmk)
{
	HUINT16				 usIndex;
	HUINT16				 usAllUniqueId;
	svcCas_RawSecTable_t		*pstTable;


	if (pstRawPool == NULL)						{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pstSecBkmk == NULL)						{ return ERR_CAS_SI_TARGET_NULL; }
	if (eTableType >= MAX_SI_TABLE_TYPE)		{ return ERR_CAS_SI_SOURCE_INVALID; }

	usAllUniqueId = DB_SVC_GetAllUniqueId();

	pstTable = &(pstRawPool->astSiTable[eTableType]);

	for (usIndex = pstTable->usStartPos; usIndex < pstTable->usInsertPos; usIndex++)
	{
		// empty index
		if (pstTable->phIdx[usIndex] == INVALID_INDEX_HANDLE)				{ continue; }

		// other TS UniqueID
		if (usTsUniqId != usAllUniqueId && usTsUniqId != MACRO_GET_UNIQUEID_FROM_IDXHANDLE(pstTable->phIdx[usIndex]))
		{
			continue;
		}

		// We found the first section:
		HxSTD_memset (pstSecBkmk, 0, sizeof(svcCas_RawSecBookmark_t));

		pstSecBkmk->pstPool		= pstRawPool;
		pstSecBkmk->usTsUniqId	= usTsUniqId;
		pstSecBkmk->eTableType	= eTableType;
		pstSecBkmk->bUseExtId	= FALSE;
		pstSecBkmk->usCurrIdx	= usIndex;

		return ERR_CAS_SI_RESULT_OK;
	}

	return ERR_CAS_SI_RESULT_FAIL;
}

HERROR svc_cas_SiSetBookmarkToNextSection(svcCas_RawSecBookmark_t *pstSrcBmk, svcCas_RawSecBookmark_t *pstDstBmk)
{
	HUINT16				 usIndex;
	HUINT16				 usTsUniqId, usAllUniqueId;
	SiTableType_e		 eTableType;
	svcCas_RawSecTable_t		*pstTable;
	svcCas_RawPool_t			*pstRawPool;

	if (pstSrcBmk == NULL)			{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pstDstBmk == NULL)			{ return ERR_CAS_SI_TARGET_NULL; }

	pstRawPool = pstSrcBmk->pstPool;
	eTableType = pstSrcBmk->eTableType;

	if (pstRawPool == NULL)						{ return ERR_CAS_SI_SOURCE_INVALID; }
	if (eTableType >= MAX_SI_TABLE_TYPE)		{ return ERR_CAS_SI_SOURCE_INVALID; }

	usTsUniqId	  = pstSrcBmk->usTsUniqId;
	usAllUniqueId = DB_SVC_GetAllUniqueId();

	pstTable = &(pstRawPool->astSiTable[eTableType]);

	for (usIndex = pstSrcBmk->usCurrIdx + 1; usIndex < pstTable->usInsertPos; usIndex++)
	{
		// empty index
		if (pstTable->phIdx[usIndex] == INVALID_INDEX_HANDLE)				{ continue; }

		// other TS UniqueID
		if (usTsUniqId != usAllUniqueId && usTsUniqId != MACRO_GET_UNIQUEID_FROM_IDXHANDLE(pstTable->phIdx[usIndex]))
		{
			continue;
		}

		// We found the next section:
		HxSTD_memcpy (pstDstBmk, pstSrcBmk, sizeof(svcCas_RawSecBookmark_t));
		pstDstBmk->usCurrIdx = usIndex;

		return ERR_CAS_SI_RESULT_OK;
	}

	// cannot find the next section.
	return ERR_CAS_SI_RESULT_FAIL;
}

HERROR svc_cas_SiSetBookmarkToFirstSectionByExtId(svcCas_RawPool_t *pstRawPool, HUINT16 usTsUniqId, SiTableType_e eTableType, HUINT16 usExtId, svcCas_RawSecBookmark_t *pstSecBkmk)
{
	HUINT16				 usIndex;
	HUINT16				 usAllUniqueId;
	HUINT16				 usRawExtId;
	HUINT8				*pucRaw;
	svcCas_RawSecTable_t		*pstTable;


	if (pstRawPool == NULL)						{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pstSecBkmk == NULL)						{ return ERR_CAS_SI_TARGET_NULL; }
	if (eTableType >= MAX_SI_TABLE_TYPE)		{ return ERR_CAS_SI_SOURCE_INVALID; }

	usAllUniqueId = DB_SVC_GetAllUniqueId();

	pstTable = &(pstRawPool->astSiTable[eTableType]);

	for (usIndex = pstTable->usStartPos; usIndex < pstTable->usInsertPos; usIndex++)
	{
		// empty index
		if (pstTable->phIdx[usIndex] == INVALID_INDEX_HANDLE)				{ continue; }

		// other TS UniqueID
		if (usTsUniqId != usAllUniqueId && usTsUniqId != MACRO_GET_UNIQUEID_FROM_IDXHANDLE(pstTable->phIdx[usIndex]))
		{
			continue;
		}

		// Get EXT ID of the raw data
		pucRaw = svc_cas_RawGetRawSection (pstRawPool, (HUINT32)MACRO_GET_INDEX_FROM_IDXHANDLE(pstTable->phIdx[usIndex]));
		if (pucRaw == NULL)													{ continue; }

		usRawExtId	 = HxMACRO_Get16Bit(pucRaw + 3);

		// Other EXT ID
		if (usRawExtId != usExtId)											{ continue; }

		// We found the first section:
		HxSTD_memset (pstSecBkmk, 0, sizeof(svcCas_RawSecBookmark_t));

		pstSecBkmk->pstPool		= pstRawPool;
		pstSecBkmk->usTsUniqId	= usTsUniqId;
		pstSecBkmk->eTableType	= eTableType;
		pstSecBkmk->bUseExtId	= TRUE;
		pstSecBkmk->usExtId		= usExtId;
		pstSecBkmk->usCurrIdx	= usIndex;

		return ERR_CAS_SI_RESULT_OK;
	}

	return ERR_CAS_SI_RESULT_FAIL;
}

HERROR svc_cas_SiSetBookmarkToNextSectionByExtId (svcCas_RawSecBookmark_t *pstSrcBmk, svcCas_RawSecBookmark_t *pstDstBmk)
{
	HUINT16				 usIndex;
	HUINT16				 usTsUniqId, usAllUniqueId;
	HUINT16				 usRawExtId;
	SiTableType_e		 eTableType;
	HUINT8				*pucRaw;
	svcCas_RawSecTable_t		*pstTable;
	svcCas_RawPool_t			*pstRawPool;

	if (pstSrcBmk == NULL)						{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pstDstBmk == NULL)						{ return ERR_CAS_SI_TARGET_NULL; }

	if (pstSrcBmk->bUseExtId != TRUE)			{ return ERR_CAS_SI_SOURCE_INVALID; }

	pstRawPool = pstSrcBmk->pstPool;
	eTableType = pstSrcBmk->eTableType;

	if (pstRawPool == NULL)						{ return ERR_CAS_SI_SOURCE_INVALID; }
	if (eTableType >= MAX_SI_TABLE_TYPE)		{ return ERR_CAS_SI_SOURCE_INVALID; }

	usTsUniqId	  = pstSrcBmk->usTsUniqId;
	usAllUniqueId = DB_SVC_GetAllUniqueId();

	pstTable = &(pstRawPool->astSiTable[eTableType]);

	for (usIndex = pstSrcBmk->usCurrIdx + 1; usIndex < pstTable->usInsertPos; usIndex++)
	{
		// empty index
		if (pstTable->phIdx[usIndex] == INVALID_INDEX_HANDLE)				{ continue; }

		// other TS UniqueID
		if (usTsUniqId != usAllUniqueId && usTsUniqId != MACRO_GET_UNIQUEID_FROM_IDXHANDLE(pstTable->phIdx[usIndex]))
		{
			continue;
		}

		// Get EXT ID of the raw data
		pucRaw = svc_cas_RawGetRawSection (pstRawPool, (HUINT32)MACRO_GET_INDEX_FROM_IDXHANDLE(pstTable->phIdx[usIndex]));
		if (pucRaw == NULL)													{ continue; }

		usRawExtId	 = HxMACRO_Get16Bit(pucRaw + 3);

		// Other EXT ID
		if (usRawExtId != pstSrcBmk->usExtId)								{ continue; }

		// We found the next section:
		HxSTD_memcpy (pstDstBmk, pstSrcBmk, sizeof(svcCas_RawSecBookmark_t));
		pstDstBmk->usCurrIdx = usIndex;

		return ERR_CAS_SI_RESULT_OK;
	}

	// cannot find the next section.
	return ERR_CAS_SI_RESULT_FAIL;
}



HUINT8 *svc_cas_SiGetRawSectionBySectionBookmark (svcCas_RawSecBookmark_t *pstSecBmk)
{
	HUINT16				 usIndex;
	svcCas_RawSecTable_t		*pstTable;
	svcCas_RawPool_t			*pstRawPool;

	if (pstSecBmk)
	{
		pstRawPool	= pstSecBmk->pstPool;
		usIndex		= pstSecBmk->usCurrIdx;
		pstTable	= &(pstRawPool->astSiTable[pstSecBmk->eTableType]);

		if (pstTable->phIdx[usIndex] != INVALID_INDEX_HANDLE)
		{
			return svc_cas_RawGetRawSection (pstRawPool, (HUINT32)MACRO_GET_INDEX_FROM_IDXHANDLE(pstTable->phIdx[usIndex]));
		}
	}

	return NULL;
}

#define __________PMT_RELATIVE____________
#if 0 // Not Used
STATIC HERROR svc_cas_SiSetDesIndexToPmtProgDescInternal(HUINT8 *pucRaw, svcCas_SiDesIndex_t *pstDesIndex)
{
	HINT32		 nSecLen, nProgInfoLen;
	HUINT8		*pucProgDesPos;

	if (pucRaw == NULL)				{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pstDesIndex == NULL)		{ return ERR_CAS_SI_TARGET_NULL; }

	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (9 + 4); 									// program info descriptor 바로 앞까지의 table fileds과 crc를 제거.
	nProgInfoLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 10);

	if (nSecLen > 0 && nProgInfoLen > 0 && nSecLen >= nProgInfoLen)
	{
		svc_cas_SiGetFirstProgDescriptorPos (pucRaw, &pucProgDesPos);

		pstDesIndex->pucCurrPos		= pucProgDesPos;
		pstDesIndex->nLeftDesLoop 	= nProgInfoLen;

		return ERR_CAS_SI_RESULT_OK;
	}

	return ERR_CAS_SI_RESULT_FAIL;
}

STATIC HERROR svc_cas_SiSetInfoIndexToPmtElementInfoInternal(HUINT8 *pucRaw, svcCas_SiInfoIndex_t *pstInfoIndex)
{
	HINT32		 nSecLen, nProgInfoLen;
	HUINT8		*pucElement;

	if (pucRaw == NULL)						{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pstInfoIndex == NULL)				{ return ERR_CAS_SI_TARGET_NULL; }

	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (9 + 4); 									// program info descriptor 바로 앞까지의 table fileds과 crc를 제거.
	nProgInfoLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 10);

	nSecLen -= nProgInfoLen;
	svc_cas_SiGetFirstProgElemInfoPos (pucRaw, &pucElement, nProgInfoLen);

	if (nSecLen > 0)
	{
		pstInfoIndex->nLeftInfoLoop	= nSecLen;
		pstInfoIndex->pucCurrPos	= pucElement;

		return ERR_CAS_SI_RESULT_OK;
	}

	return ERR_CAS_SI_RESULT_FAIL;
}

STATIC HERROR svc_cas_SiSetInfoIndexToNextPmtElementInfoInternal(svcCas_SiInfoIndex_t *pstCurrIndex, svcCas_SiInfoIndex_t *pstNextIndex)
{
	HUINT16			 usDesLoopLen;
	HINT32			 nLeftInfoLoop;
	HUINT8			*pucCurrPos;

	if (pstCurrIndex == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pstNextIndex == NULL)					{ return ERR_CAS_SI_TARGET_NULL; }

	// check whether it can get the proper element info or not: At least, a element info requires 5+a bytes.
	if (pstCurrIndex->pucCurrPos == NULL)		{ return ERR_CAS_SI_RESULT_FAIL; }
	if (pstCurrIndex->nLeftInfoLoop < 5)		{ return ERR_CAS_SI_RESULT_FAIL; }

	pucCurrPos	  = pstCurrIndex->pucCurrPos;
	nLeftInfoLoop = pstCurrIndex->nLeftInfoLoop;

	usDesLoopLen = HxMACRO_Get12Bit(pucCurrPos + 3);

	pucCurrPos	  += (5 + usDesLoopLen);
	nLeftInfoLoop -= (5 + usDesLoopLen);

	// check whether it can get the proper element info or not: At least, a element info requires 5+a bytes.
	if (nLeftInfoLoop < 5)						{ return ERR_CAS_SI_RESULT_FAIL; }

	pstNextIndex->pucCurrPos	= pucCurrPos;
	pstNextIndex->nLeftInfoLoop	= nLeftInfoLoop;

	return ERR_CAS_SI_RESULT_OK;
}

STATIC HERROR svc_cas_SiGetPmtElementInfoFromInfoIndexInternal(svcCas_SiInfoIndex_t *pstInfoIndex,
																	svcCas_SiPmtElementInfo_t *pstInfo,
																	svcCas_SiInfoIndex_t *pstNextIndex)
{
	HUINT16			 usDesLoopLen;
	HINT32			 nLeftInfoLoop;
	HUINT8			*pucCurrPos;

	if (pstInfoIndex == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pstInfo == NULL)						{ return ERR_CAS_SI_TARGET_NULL; }

	// check whether it can get the proper element info or not: At least, a element info requires 5+a bytes.
	if (pstInfoIndex->pucCurrPos == NULL)		{ return ERR_CAS_SI_RESULT_FAIL; }
	if (pstInfoIndex->nLeftInfoLoop < 5)		{ return ERR_CAS_SI_RESULT_FAIL; }

	pucCurrPos	  = pstInfoIndex->pucCurrPos;
	nLeftInfoLoop = pstInfoIndex->nLeftInfoLoop;

	pstInfo->ucStreamType	= HxMACRO_Get8Bit(pucCurrPos);
	pstInfo->usElemPid		= HxMACRO_Get13Bit(pucCurrPos + 1);

	if (pstNextIndex != NULL)
	{
		usDesLoopLen = HxMACRO_Get12Bit(pucCurrPos + 3);

		pucCurrPos	  += (5 + usDesLoopLen);
		nLeftInfoLoop -= (5 + usDesLoopLen);

		pstNextIndex->pucCurrPos	= pucCurrPos;
		pstNextIndex->nLeftInfoLoop	= nLeftInfoLoop;
	}

	return ERR_CAS_SI_RESULT_OK;
}

STATIC HERROR svc_cas_SiSetDesIndexToPmtElementDesInternal(svcCas_SiInfoIndex_t *pstInfoIndex, svcCas_SiDesIndex_t *pstDesIndex)
{
	HINT32			 nEsInfoLen;
	HUINT8			*pucDesPos;

	if (pstInfoIndex == NULL)					{ return ERR_CAS_SI_SOURCE_NULL; }
	if (pstDesIndex == NULL)					{ return ERR_CAS_SI_TARGET_NULL; }

	// check whether it can get the proper element info or not: At least, a element info requires 5+a bytes.
	if (pstInfoIndex->pucCurrPos == NULL)		{ return ERR_CAS_SI_RESULT_FAIL; }
	if (pstInfoIndex->nLeftInfoLoop < 5)		{ return ERR_CAS_SI_RESULT_FAIL; }

	svc_cas_SiGetFirstProgElemDesPos (pstInfoIndex->pucCurrPos, &pucDesPos);
	nEsInfoLen = (HINT32)HxMACRO_Get12Bit(pstInfoIndex->pucCurrPos + 3);

	pstDesIndex->pucCurrPos	  = pucDesPos;
	pstDesIndex->nLeftDesLoop = nEsInfoLen;

	return ERR_CAS_SI_RESULT_OK;
}

HERROR svc_cas_SiSetDesIndexToPmtProgDescFromRaw(HUINT8 *pucRaw, svcCas_SiDesIndex_t *pstDesIndex)
{
	HUINT32		 ulRet;

	HxLOG_Info("[svc_cas_SiSetDesIndexToPmtProgDescFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	ulRet  = svc_cas_SiSetDesIndexToPmtProgDescInternal(pucRaw, pstDesIndex);
	HxLOG_Info("[svc_cas_SiSetDesIndexToPmtProgDescFromRaw] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiSetDesIndexToPmtProgDesc(svcCas_RawSecBookmark_t *pstSecBmk, svcCas_SiDesIndex_t *pstDesIndex)
{
	HUINT8		*pucRaw;
	HUINT32		 ulRet;

	HxLOG_Info("[svc_cas_SiSetDesIndexToPmtProgDesc] (+) Ts Unique Id = 0x%x\n", pstSecBmk->usTsUniqId);
	pucRaw = svc_cas_SiGetRawSectionBySectionBookmark(pstSecBmk);
	ulRet = svc_cas_SiSetDesIndexToPmtProgDescInternal(pucRaw, pstDesIndex);
	HxLOG_Info("[svc_cas_SiSetDesIndexToPmtProgDesc] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiSetInfoIndexToPmtElementInfoFromRaw(HUINT8 *pucRaw, svcCas_SiInfoIndex_t *pstInfoIndex)
{
	HUINT32		 ulRet;

	HxLOG_Info("[svc_cas_SiSetInfoIndexToPmtElementInfoFromRaw] (+) pucRaw = 0x%08x\n", (HUINT32)pucRaw);
	ulRet  = svc_cas_SiSetInfoIndexToPmtElementInfoInternal(pucRaw, pstInfoIndex);
	HxLOG_Info("[svc_cas_SiSetInfoIndexToPmtElementInfoFromRaw] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiSetInfoIndexToPmtElementInfo(svcCas_RawSecBookmark_t *pstSecBmk, svcCas_SiInfoIndex_t *pstInfoIndex)
{
	HUINT8		*pucRaw;
	HUINT32		 ulRet;

	HxLOG_Info("[svc_cas_SiSetInfoIndexToPmtElementInfo] (+) Ts Unique Id = 0x%x\n", pstSecBmk->usTsUniqId);
	pucRaw = svc_cas_SiGetRawSectionBySectionBookmark(pstSecBmk);
	ulRet  = svc_cas_SiSetInfoIndexToPmtElementInfoInternal(pucRaw, pstInfoIndex);
	HxLOG_Info("[svc_cas_SiSetInfoIndexToPmtElementInfo] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiSetInfoIndexToNextPmtElementInfo(svcCas_SiInfoIndex_t *pstCurrIndex, svcCas_SiInfoIndex_t *pstNextIndex)
{
	HUINT32		 ulRet;

	HxLOG_Info("[svc_cas_SiSetInfoIndexToNextPmtElementInfo] (+)\n");
	ulRet = svc_cas_SiSetInfoIndexToNextPmtElementInfoInternal(pstCurrIndex, pstNextIndex);
	HxLOG_Info("[svc_cas_SiSetInfoIndexToNextPmtElementInfo] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiGetPmtProgramInfoFromBookmark(svcCas_RawSecBookmark_t *pstSecBmk, svcCas_SiPmtProgramInfo_t *pstSecInfo)
{
	HUINT8		*pucRaw;
	HUINT32		 ulRet;

	HxLOG_Info("[svc_cas_SiGetPmtProgramInfoFromBookmark] (+)\n");
	pucRaw = svc_cas_SiGetRawSectionBySectionBookmark(pstSecBmk);
	ulRet  = svc_cas_SiGetPmtProgramInfoFromRawInternal(pucRaw, pstSecInfo);
	HxLOG_Info("[svc_cas_SiGetPmtProgramInfoFromBookmark] (-)\n");

	return ulRet;
}

HERROR svc_cas_SiGetPmtElementInfoFromInfoIndex(svcCas_SiInfoIndex_t *pstInfoIndex,
															svcCas_SiPmtElementInfo_t *pstInfo,
															svcCas_SiInfoIndex_t *pstNextIndex)
{
	HUINT32		 ulRet;

	HxLOG_Info("[svc_cas_SiGetPmtElementInfoFromInfoIndex] (+)\n");
	ulRet = svc_cas_SiGetPmtElementInfoFromInfoIndexInternal(pstInfoIndex, pstInfo, pstNextIndex);
	HxLOG_Info("[svc_cas_SiGetPmtElementInfoFromInfoIndex] (-)\n");

	return ulRet;
}

// svc_cas_SiSetDesIndexToPmtElementDes
// InfoIndex 가 가리키는 Pmt Element Info 로부터 첫 Pmt Element Descriptor가 위치하는 DesIndex를 지정하는 함수
HERROR svc_cas_SiSetDesIndexToPmtElementDes(svcCas_SiInfoIndex_t *pstInfoIndex, svcCas_SiDesIndex_t *pstDesIndex)
{
	HUINT32		 ulRet;

	HxLOG_Info("[svc_cas_SiSetDesIndexToPmtElementDes] (+)\n");
	ulRet = svc_cas_SiSetDesIndexToPmtElementDesInternal(pstInfoIndex, pstDesIndex);
	HxLOG_Info("[svc_cas_SiSetDesIndexToPmtElementDes] (-)\n");

	return ulRet;
}
#endif

HUINT32 svc_cas_SiUtilStrlen(const HUINT8 *pszStr)
{
	if (pszStr == NULL) return 0;

	return HxSTD_StrLen(pszStr);
}


/* end of file */
