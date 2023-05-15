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
#include <silib.h>

#include <octo_common.h>

#include <pal_sef.h>
#include <pal_pipe.h>

#include "../local_include/_svc_epg.h"
#include "../local_include/_svc_epg_raw_mgr.h"
#include "../local_include/_svc_epg_si_lib.h"

#include <psvc_epg.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	SvcEpg_MAKE_INDEX_HANDLE(usUniqueId, usIdx)		(HUINT32)(usUniqueId << 16 | usIdx)
#define	SvcEpg_GET_UNIQUEID_FROM_IDXHANDLE(hIdx)		(HUINT16)(hIdx >> 16 & 0xffff)
#define	SvcEpg_GET_INDEX_FROM_IDXHANDLE(hIdx)			(HUINT16)(hIdx & 0xffff)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define ____LOCAL_FUNC____
STATIC INLINE void svc_epg_SiGetFirstProgramInfoPos (HUINT8 *pucSection, HUINT8 **ppucFirstProg)
{
	*ppucFirstProg = pucSection + 8;
}

STATIC INLINE void svc_epg_SiGetNextProgramInfoPos (HUINT8 *pucProg, HUINT8 **ppucNextProg)
{
	*ppucNextProg = pucProg + 4;
}

STATIC INLINE void svc_epg_SiGetFirstProgElemInfoPos(HUINT8 *pucSection, HUINT8 **ppucFirstElement, HINT32 nProgDesLen)
{
	*ppucFirstElement = pucSection + 12 + nProgDesLen;
}

STATIC INLINE void svc_epg_SiGetNextProgElemInfoPos(HUINT8 *pucElement, HUINT8 **ppucNextElement, HUINT32 ulElemDesLen)
{
	*ppucNextElement = pucElement + 5 + ulElemDesLen;
}

STATIC INLINE void svc_epg_SiGetFirstProgElemDesPos(HUINT8 *pucElement, HUINT8 **ppucFirstElemDes)
{
	*ppucFirstElemDes = pucElement + 5;
}

STATIC INLINE void svc_epg_SiGetNextProgElemDesPos(HUINT8 *pucElemDes, HUINT8 **ppucNextElemDes, HUINT32 ulElemDesLen)
{
	*ppucNextElemDes = pucElemDes + 2 + ulElemDesLen;
}

STATIC INLINE HBOOL svc_epg_SiIsSameDesTag(SIxDescriptorTag_e eSecDesTag, SIxDescriptorTag_e eArgDesTag)
{
  	return ((eArgDesTag == eSIxDESCTAG_ALL) || ((eArgDesTag & 0xff) == (eSecDesTag & 0xff))) ? TRUE : FALSE;
}

STATIC svcEpg_RawSecTable_t *svc_epg_SiGetRawTable(svcEpg_RawPool_t *pstPool, SiTableType_e eTableType)
{
	if (pstPool == NULL)					{ return NULL; }
	if (eTableType >= MAX_SI_TABLE_TYPE)	{ return NULL; }

	return &(pstPool->astSiTable[eTableType]);
}

STATIC HUINT8 *svc_epg_SiGetRawSection(const svcEpg_RawPool_t *pstPool, HUINT32 ulSecIdx)
{
	if (pstPool == NULL)								{ return NULL; }
	if (pstPool->usMaxNumRawSec <= (HUINT16)ulSecIdx)	{ return NULL; }

	return pstPool->ppucRawSec[ulSecIdx];
}


STATIC HERROR svc_epg_SiCountPatProgramFromRaw (HUINT8 *pucRaw, HUINT32 *pulCount)
{
	HUINT8		*pucProgSection;
	HINT32		 nSecLen, nPatProgCnt;

	if (pucRaw == NULL)					{ return ERR_FAIL; }
	if (pulCount == NULL)				{ return ERR_FAIL; }

	if (pucRaw[0] != PAT_TABLE_ID)		{ return ERR_FAIL; }

	*pulCount = 0;
	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (5 + 4);					//  나머지 table fields와  CRC 제거.
	nPatProgCnt = 0;
	svc_epg_SiGetFirstProgramInfoPos(pucRaw, &pucProgSection);
	while (nSecLen > 0)
	{
		//  if program number == 0, that indicate NIT PID.
		if (HxMACRO_Get16Bit(pucProgSection) != 0)
		{
			nPatProgCnt++;
		}

		//  program Info 내용은 4byte
		nSecLen -= 4;
		svc_epg_SiGetNextProgramInfoPos (pucProgSection, &pucProgSection);
	}

	*pulCount = nPatProgCnt;
	HxLOG_Print("Pat Program count = %d\n",nPatProgCnt);

	return ERR_OK;
}

STATIC HERROR svc_epg_SiGetPatProgramInfoFromRaw (HUINT8 *pucRaw,
													HUINT32 ulInfoIdx,
													PatProgInfo_t *pstInfo)
{
	HUINT8		*pucProgSection;
	HUINT32		 ulPatProgCnt;
	HINT32		 nSecLen;

	if (pucRaw == NULL)					{ return ERR_FAIL; }
	if (pstInfo == NULL)				{ return ERR_FAIL; }

	if (pucRaw[0] != PAT_TABLE_ID)		{ return ERR_FAIL; }

	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (5 + 4);					//  나머지 table fields와  CRC 제거.
	ulPatProgCnt = 0;
	svc_epg_SiGetFirstProgramInfoPos(pucRaw, &pucProgSection);

	while (nSecLen > 0)
	{
		if (nSecLen < 4)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_FAIL;
		}

		if (ulInfoIdx == ulPatProgCnt && HxMACRO_Get16Bit(pucProgSection) != 0)
		{
			HxSTD_memset(pstInfo, 0, sizeof(PatProgInfo_t));

			pstInfo->usProgNum = HxMACRO_Get16Bit(pucProgSection);
			pstInfo->usProgMapPid = HxMACRO_Get13Bit(pucProgSection + 2);

			HxLOG_Print("Program num = 0x%x, Program Map Pid = 0x%x\n", pstInfo->usProgNum,pstInfo->usProgMapPid);

			return ERR_OK;
		}

		//  if program number == 0, that indicate NIT PID.
		if ( HxMACRO_Get16Bit(pucProgSection) != 0 )
		{
			ulPatProgCnt++;
		}

		//  program Info 내용은 4byte
		nSecLen -= 4;
		svc_epg_SiGetNextProgramInfoPos(pucProgSection, &pucProgSection);
	}

	return ERR_FAIL;
}

STATIC HERROR svc_epg_SiCountPatSection (svcEpg_RawPool_t *pstPool, HUINT16 usTsUniqId, HUINT32 *pulCount)
{
	HUINT16			 usCount;
	HUINT16			 usAllUniqueId;
	HUINT32			 ulSecCnt;
	svcEpg_RawSecTable_t	*pstTable;

	if (pstPool == NULL)		{ return ERR_FAIL; }
	if (pulCount == NULL)			{ return ERR_FAIL; }

	usAllUniqueId = DB_SVC_GetAllUniqueId();
	pstTable = svc_epg_SiGetRawTable(pstPool, eSI_TableType_PAT);

	ulSecCnt = 0;
	for (usCount = pstTable->usStartPos; usCount < pstTable->usInsertPos; usCount++)
	{
		if (pstTable->phIdx[usCount] == SvcEpg_INVALID_INDEX_HANDLE)			{ continue; }

		if (usTsUniqId == usAllUniqueId || usTsUniqId == SvcEpg_GET_UNIQUEID_FROM_IDXHANDLE(pstTable->phIdx[usCount]))
		{
			ulSecCnt++;
		}
	}

	*pulCount = ulSecCnt;
	HxLOG_Print("Pat section count = %d\n", ulSecCnt);

	return ERR_OK;
}

STATIC HUINT8 *svc_epg_SiFindPatSectinTable (svcEpg_RawPool_t *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx)
{
	HUINT16			 usCount, usSecIndex;
	HUINT16			 usAllUniqueId;
	HUINT32			 ulSecCnt;
	HUINT8			*pucRaw;
	svcEpg_RawSecTable_t	*pstTable;

	if (pstPool == NULL)			return NULL;

	usAllUniqueId = DB_SVC_GetAllUniqueId();
	pstTable = svc_epg_SiGetRawTable(pstPool, eSI_TableType_PAT);

	ulSecCnt = 0;

	for (usCount = pstTable->usStartPos; usCount < pstTable->usInsertPos; usCount++)
	{
		if (pstTable->phIdx[usCount] == SvcEpg_INVALID_INDEX_HANDLE)			{ continue; }

		if (usTsUniqId == usAllUniqueId || usTsUniqId == SvcEpg_GET_UNIQUEID_FROM_IDXHANDLE(pstTable->phIdx[usCount]))
		{
			if (ulSecCnt == ulIdx)
			{
				usSecIndex = SvcEpg_GET_INDEX_FROM_IDXHANDLE(pstTable->phIdx[usCount]);
				pucRaw = svc_epg_SiGetRawSection(pstPool, (HUINT32)usSecIndex);
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

STATIC HERROR svc_epg_SiGetPmtPidByServiceIdFromRaw (HUINT8 *pucRaw, HUINT16 usSvcId, HUINT16 *pusPmtPid)
{
	HUINT32			 ulSvcIdx, ulSvcCnt;
	PatProgInfo_t	 stInfo;
	HERROR			 hErr;

	if (pucRaw == NULL)					{ return ERR_FAIL; }
	if (pusPmtPid == NULL)				{ return ERR_FAIL; }

	if (pucRaw[0] != PAT_TABLE_ID)		{ return ERR_FAIL; }

	hErr = svc_epg_SiCountPatProgramFromRaw(pucRaw, &ulSvcCnt);
	if (hErr != ERR_OK)					{ ulSvcCnt = 0; }

	for (ulSvcIdx = 0; ulSvcIdx < ulSvcCnt; ulSvcIdx++)
	{
		hErr = svc_epg_SiGetPatProgramInfoFromRaw(pucRaw, ulSvcIdx, &stInfo);
		if (hErr == ERR_OK && (HUINT16)usSvcId == stInfo.usProgNum)
		{
			*pusPmtPid = stInfo.usProgMapPid;
			HxLOG_Print("Pmt PID = 0x%x\n", *pusPmtPid);
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

STATIC HUINT8 *svc_epg_SiFindPmtSectionTable(svcEpg_RawPool_t *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx)
{
	HUINT16			 usCount, usAllUniqueId;
	HUINT16			 usRawIndex;
	HUINT32			 ulPmtSecCnt;
	HUINT8			*pucRaw;
	svcEpg_RawSecTable_t	*pstTable;

	if (pstPool == NULL)		{ return NULL; }


	usAllUniqueId = DB_SVC_GetAllUniqueId();
	pstTable = svc_epg_SiGetRawTable(pstPool, eSI_TableType_PMT);
	if (pstTable == NULL)		{ return NULL; }

	ulPmtSecCnt = 0;
	for (usCount = pstTable->usStartPos; usCount < pstTable->usInsertPos; usCount++)
	{
		if (pstTable->phIdx[usCount] == SvcEpg_INVALID_INDEX_HANDLE)				{ continue; }

		// If there is no match in the unique ID, then it shall be skipped.
		if ((usTsUniqId != usAllUniqueId)
			&& (usTsUniqId != SvcEpg_GET_UNIQUEID_FROM_IDXHANDLE(pstTable->phIdx[usCount])))				{ continue; }

		if (ulPmtSecCnt >= ulIdx)
		{
			usRawIndex = SvcEpg_GET_INDEX_FROM_IDXHANDLE(pstTable->phIdx[usCount]);
			pucRaw = svc_epg_SiGetRawSection(pstPool, (HUINT32)usRawIndex);
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

STATIC HERROR svc_epg_SiCountPmtElementFromRaw(HUINT8 *pucRaw, HUINT32 *pulElemCnt)
{
	HUINT16		 usEsInfoLen;
	HUINT32		 ulElemCnt;
	HINT32		 nSecLen, nProgInfoLen;
	HUINT8		*pucElement;

	if (pucRaw == NULL)				{ return ERR_FAIL; }
	if (pulElemCnt == NULL)			{ return ERR_FAIL; }

	ulElemCnt = 0;
	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (9 + 4); 									// program info descriptor 바로 앞까지의 table fileds과 crc를 제거.
	nProgInfoLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 10);

	nSecLen -= nProgInfoLen;
	svc_epg_SiGetFirstProgElemInfoPos(pucRaw, &pucElement, nProgInfoLen);
	while (nSecLen > 0)
	{
		if (nSecLen < 5)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_FAIL;
		}

		usEsInfoLen = HxMACRO_Get12Bit(pucElement + 3);			// The length of the Descriptors
		nSecLen -= (usEsInfoLen + 5);
		ulElemCnt++;
		svc_epg_SiGetNextProgElemInfoPos(pucElement, &pucElement, (HUINT32)usEsInfoLen);
	}

	*pulElemCnt = ulElemCnt;
	HxLOG_Print("Pmt Element count = %d\n",ulElemCnt);

	return ERR_OK;
}

STATIC HERROR svc_epg_SiGetPmtProgramInfoFromRaw(HUINT8 *pucRaw, PmtProgramInfo_t *pstInfo)
{
	HUINT16		 nSecLen;

	if (pucRaw == NULL)				{ return ERR_FAIL; }
	if (pstInfo == NULL)			{ return ERR_FAIL; }

	HxSTD_memset (pstInfo, 0, sizeof(PmtProgramInfo_t));
	nSecLen = HxMACRO_Get12Bit(pucRaw + 1);

	if (nSecLen < 5)
	{
		HxLOG_Critical("\n\n\n");
		return ERR_FAIL;
	}

	pstInfo->usProgNum	 = HxMACRO_Get16Bit (pucRaw + 3);
	pstInfo->ucVer		 = (pucRaw[5] & 0x3e) >> 1;
	pstInfo->bCurNextInd = pucRaw[5] & 0x01;
	pstInfo->usPCRPid	 = HxMACRO_Get13Bit(pucRaw + 8);
	pstInfo->ulCrc32	 = HxMACRO_Get32Bit(pucRaw + nSecLen - 1);			// +3 (After the length) - 4 (CRC) = -1

	HxLOG_Print("ProgramNum = 0x%x\n", pstInfo->usProgNum);
	HxLOG_Print("Version num = 0x%x\n", pstInfo->ucVer);
	HxLOG_Print("Current next indicator = 0x%x\n", pstInfo->bCurNextInd);
	HxLOG_Print("PCR Pid = 0x%x\n", pstInfo->usPCRPid);
	HxLOG_Print("Crc 32 = 0x%X\n", pstInfo->ulCrc32);

	return ERR_OK;
}

STATIC HERROR svc_epg_SiGetPmtElementInfoFromRaw(HUINT8 *pucRaw,
												HUINT32 ulElemIdx,
												PmtElementInfo_t *pstInfo)
{
	HUINT16		 usEsInfoLen;
	HUINT32		 ulElemCnt;
	HINT32		 nSecLen, nProgInfoLen;
	HUINT8		*pucElement;

	if (pucRaw == NULL)				{ return ERR_FAIL; }
	if (pstInfo == NULL)			{ return ERR_FAIL; }

	ulElemCnt = 0;
	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (9 + 4); 									// program info descriptor 바로 앞까지의 table fileds과 crc를 제거.
	nProgInfoLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 10);

	nSecLen -= nProgInfoLen;
	svc_epg_SiGetFirstProgElemInfoPos (pucRaw, &pucElement, nProgInfoLen);
	while (nSecLen > 0)
	{
		if (nSecLen < 5)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_FAIL;
		}

		usEsInfoLen = HxMACRO_Get12Bit(pucElement + 3);			// The length of the Descriptors
		nSecLen -= (usEsInfoLen + 5);

		if (ulElemCnt != ulElemIdx)
		{
			ulElemCnt++;
			svc_epg_SiGetNextProgElemInfoPos (pucElement, &pucElement, (HUINT32)usEsInfoLen);
			continue;
		}

		if (nSecLen < 0)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_FAIL;
		}

		HxSTD_memset (pstInfo, 0, sizeof(PmtElementInfo_t));
		pstInfo->ucStreamType = HxMACRO_Get8Bit(pucElement);
		pstInfo->usElemPid = HxMACRO_Get13Bit(pucElement + 1);

		HxLOG_Print("StreamType = 0x%x, ElemPid = 0x%x\n", pstInfo->ucStreamType, pstInfo->usElemPid);

		return ERR_OK;
	}

	return ERR_FAIL;
}

STATIC HERROR svc_epg_SiCountPmtElementDescriptorFromRaw(HUINT8 *pucRaw,
															HUINT32 ulElemIdx,
															SIxDescriptorTag_e eTag,
															HUINT32 *pulCount)
{
	HUINT8		 ucDesTag, ucDesLen;
	HUINT32		 ulElemCnt;
	HUINT32		 ulDesCnt;
	HINT32		 nSecLen, nProgInfoLen, nEsInfoLen;
	HUINT8		*pucElement, *pucElemDes;

	if (pucRaw == NULL)				{ return ERR_FAIL; }
	if (pulCount == NULL)			{ return ERR_FAIL; }

	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (9 + 4); 									// program info descriptor 바로 앞까지의 table fileds과 crc를 제거.
	nProgInfoLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 10);

	nSecLen -= nProgInfoLen;
	svc_epg_SiGetFirstProgElemInfoPos (pucRaw, &pucElement, nProgInfoLen);

	ulElemCnt = 0;
	while (nSecLen > 0)
	{
		nEsInfoLen = (HINT32)HxMACRO_Get12Bit(pucElement + 3);			// The length of the Descriptors
		nSecLen -= (nEsInfoLen + 5);

		if (nSecLen < 0)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_FAIL;
		}

		if (ulElemCnt != ulElemIdx)
		{
			ulElemCnt++;
			svc_epg_SiGetNextProgElemInfoPos (pucElement, &pucElement, (HUINT32)nEsInfoLen);
			continue;
		}

		ulDesCnt = 0;
		svc_epg_SiGetFirstProgElemDesPos(pucElement, &pucElemDes);

		while (nEsInfoLen > 0)
		{
			if (nEsInfoLen < 2)
			{
				HxLOG_Critical("\n\n\n");
				return ERR_FAIL;
			}

			ucDesTag = HxMACRO_Get8Bit(pucElemDes);
			ucDesLen = HxMACRO_Get8Bit(pucElemDes + 1);

			nEsInfoLen -= (ucDesLen + 2);
			if (nEsInfoLen < 0)
			{
				HxLOG_Critical("\n\n\n");
				return ERR_FAIL;
			}

			if (svc_epg_SiIsSameDesTag((SIxDescriptorTag_e)ucDesTag, eTag) == TRUE)			// Same TAG
			{
				ulDesCnt++;
			}

			svc_epg_SiGetNextProgElemDesPos(pucElemDes, &pucElemDes, (HUINT32)ucDesLen);
		}

		*pulCount = ulDesCnt;
		HxLOG_Print("Pmt Element descriptor count = %d\n", ulDesCnt);

		return ERR_OK;
	}

	return ERR_FAIL;
}

STATIC HERROR svc_epg_SiGetPmtElementDescriptorFromRaw(HUINT8 *pucRaw,
														HUINT32 ulElemIdx,
														SIxDescriptorTag_e eTag,
														HUINT32 ulDesIdx,
														SIxDescriptor_t **ppstDes)
{
	HUINT8		 ucDesTag, ucDesLen;
	HUINT32		 ulElemCnt;
	HUINT32		 ulDesCnt;
	HINT32		 nSecLen, nProgInfoLen, nEsInfoLen;
	HUINT8		*pucElement, *pucElemDes;

	if (pucRaw == NULL)				{ return ERR_FAIL; }
	if (ppstDes == NULL)			{ return ERR_FAIL; }

	nSecLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 1);
	nSecLen -= (9 + 4); 									// program info descriptor 바로 앞까지의 table fileds과 crc를 제거.
	nProgInfoLen = (HINT32)HxMACRO_Get12Bit(pucRaw + 10);

	nSecLen -= nProgInfoLen;
	svc_epg_SiGetFirstProgElemInfoPos (pucRaw, &pucElement, nProgInfoLen);
	ulElemCnt = 0;
	while (nSecLen > 0)
	{
		nEsInfoLen = (HINT32)HxMACRO_Get12Bit(pucElement + 3);			// The length of the Descriptors
		nSecLen -= (nEsInfoLen + 5);

		if (nSecLen < 0)
		{
			HxLOG_Critical("\n\n\n");
			return ERR_FAIL;
		}

		if (ulElemCnt != ulElemIdx)
		{
			ulElemCnt++;
			svc_epg_SiGetNextProgElemInfoPos (pucElement, &pucElement, (HUINT32)nEsInfoLen);
			continue;
		}

		ulDesCnt = 0;
		svc_epg_SiGetFirstProgElemDesPos (pucElement, &pucElemDes);
		while (nEsInfoLen > 0)
		{
			if (nEsInfoLen < 2)
			{
				HxLOG_Critical("\n\n\n");
				return ERR_FAIL;
			}

			ucDesTag = HxMACRO_Get8Bit(pucElemDes);
			ucDesLen = HxMACRO_Get8Bit(pucElemDes + 1);

			nEsInfoLen -= (ucDesLen + 2);
			if (nEsInfoLen < 0)
			{
				HxLOG_Critical("\n\n\n");
				return ERR_FAIL;
			}

			if (svc_epg_SiIsSameDesTag((SIxDescriptorTag_e)ucDesTag, eTag) != TRUE)			// Different TAG
			{
				svc_epg_SiGetNextProgElemDesPos (pucElemDes, &pucElemDes, (HUINT32)ucDesLen);
				continue;
			}

			if (ulDesCnt != ulDesIdx)		// Different Index
			{
				ulDesCnt++;
				svc_epg_SiGetNextProgElemDesPos (pucElemDes, &pucElemDes, (HUINT32)ucDesLen);
				continue;
			}

			{
				SIxDescriptorHeaderParam_t stParam;
				const HUINT8		*pucRawData = (const HUINT8 *)pucElemDes;
				SIxError_e			 eErr;
				SIxDescriptor_t		*pstDes = NULL;
				SIxTextEncoding_e	 eEncoding = eSIxTEXT_ENC_DEFAULT;		// TODO: Get Default Encoding From Plug-in Function

				stParam.eTextEnc = eEncoding;
				stParam.pszArg = NULL;

				eErr = SILIB_DESC_New((const SIxDescriptorTag_e)eTag, eSIxSPEC_DVB, (const SIxDescriptorHeaderParam_t *)&stParam, pucRawData, &pstDes);
				if (eErr == eSIxERR_NOT_ENOUGH_MEMORY)		{ return ERR_FAIL; }
				if (eErr != eSIxERR_OK)
				{
					(void)SILIB_DESC_Delete(pstDes);
					if (eErr == eSIxERR_INVALID || eErr == eSIxERR_NOT_SUPPORT)
					{
						svc_epg_SiGetNextProgElemDesPos (pucElemDes, &pucElemDes, (HUINT32)ucDesLen);
						continue;
					}
					else
					{
						return ERR_FAIL;
					}
				}
				*ppstDes = (void *)pstDes;
				return ERR_OK;
			}
		}

		break;
	}

	return ERR_FAIL;
}

STATIC HERROR svc_epg_SiGetTimeout(Handle_t hAction, SiTableType_e eTableType, HUINT32 *pulTimeout)
{
	if (NULL == pulTimeout)
		return ERR_FAIL;

	return svc_epg_SiGetTableTimeOut(hAction, eTableType, pulTimeout);
}

#define ____GLOBAL_FUNC____

HERROR svc_epg_SiSetDefaultRetrieveParam(SiRetrieveParam_t *pstRetrieveParam,
										HUINT32 hAction,
										HUINT16 usTsUniqId,
										HUINT32 ulDemuxId)
{
	if (pstRetrieveParam == NULL)			{ return ERR_FAIL; }

	HxSTD_memset(pstRetrieveParam, 0, sizeof(SiRetrieveParam_t));

	pstRetrieveParam->hAction		 = hAction;
	pstRetrieveParam->usTsUniqId 	 = usTsUniqId;
	pstRetrieveParam->ucDemuxId	 = (HUINT8)ulDemuxId;
	pstRetrieveParam->usPid		 = PID_NULL;
	pstRetrieveParam->ucMode 		 = eSiFilterMode_Once;
	pstRetrieveParam->ucHead		 = ACT_HEAD;
	pstRetrieveParam->bTableFilter = TRUE;
	pstRetrieveParam->bUseExtId	 = FALSE;
	pstRetrieveParam->bUseVerNum	 = FALSE;
	pstRetrieveParam->bCrc		 = TRUE; /* crc 깨진 section에 대한 처리가 안되어 있는 경우도 있으므로, default가 TRUE인 것이 맞음 */
	pstRetrieveParam->usExtId 	 = 0;
	pstRetrieveParam->ucVerNum	 = 0;

	return ERR_OK;
}

HERROR svc_epg_SiStopOneSection(HUINT16 usFilterId)
{
	PAL_SEF_StopSecFilter(usFilterId);
	return ERR_OK;
}

HERROR svc_epg_SiRetrievePatSection(SiRetrieveParam_t *pstTable,
									void *pfnParse,
									void *pfnFree,
									HUINT16 *pusFilterId)
{
	HERROR					err;
	HUINT32					ulTimeout;
	HUINT16					usFilterId;
	PalSef_FilterParam_t	stFilterParam;

	if (pstTable == NULL)					{ return ERR_FAIL; }
	if (pusFilterId == NULL)				{ return ERR_FAIL; }

	err = svc_epg_SiGetTimeout(HANDLE_NULL, eSI_TableType_PAT, &ulTimeout);
	if(err != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(&stFilterParam,
										PAT_TABLE_ID,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										PAT_PID,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)pfnParse,
										(PalSef_Callback_t)pfnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_OK;
}

HERROR svc_epg_SiRetrievePmtSection (SiRetrieveParam_t *pstTable,
												void *pfnParse,
												void *pfnFree,
												HUINT16 *pusFilterId)
{
	HERROR				err;
	HUINT32				ulTimeout;
	HUINT16				usFilterId;
	PalSef_FilterParam_t		stFilterParam;

	if (pstTable == NULL)					{ return ERR_FAIL; }
	if (pusFilterId == NULL)				{ return ERR_FAIL; }

	err = svc_epg_SiGetTimeout(HANDLE_NULL, eSI_TableType_PMT, &ulTimeout);
	if(err != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(&stFilterParam,
										PMT_TABLE_ID,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										pstTable->usPid,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)pfnParse,
										(PalSef_Callback_t)pfnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_OK;
}

HERROR svc_epg_SiRetrieveDsiSection(SiRetrieveParam_t *pstTable,
									void *pfnParse,
									void *pfnFree,
									HUINT16 *pusFilterId)
{
	HERROR				err;
	HUINT32				ulTimeout;
	HUINT16				usFilterId;
	PalSef_FilterParam_t		stFilterParam;

	if (pstTable == NULL)					{ return ERR_FAIL; }
	if (pusFilterId == NULL)				{ return ERR_FAIL; }

	err = svc_epg_SiGetTimeout(HANDLE_NULL, eSI_TableType_DSI, &ulTimeout);
	if(err != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(	&stFilterParam,
										DSI_TABLE_ID,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										pstTable->usPid,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)pfnParse,
										(PalSef_Callback_t)pfnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_OK;
}

HERROR svc_epg_SiRetrieveDdbSection(SiRetrieveParam_t *pstTable,
									void *pfnParse,
									void *pfnFree,
									HUINT16 *pusFilterId)
{
	HERROR				err;
	HUINT32				ulTimeout;
	HUINT16				usFilterId;
	PalSef_FilterParam_t		stFilterParam;

	if (pstTable == NULL)					{ return ERR_FAIL; }
	if (pusFilterId == NULL)				{ return ERR_FAIL; }

	err = svc_epg_SiGetTimeout(HANDLE_NULL, eSI_TableType_DDB, &ulTimeout);
	if(err != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

	HxSTD_memset(&stFilterParam, 0 ,sizeof(PalSef_FilterParam_t));
	PAL_SEF_SiMakeDefaultFilterParam(	&stFilterParam,
										DDB_TABLE_ID,
										pstTable->ucDemuxId,
										pstTable->usTsUniqId,
										pstTable->usPid,
										pstTable->bTableFilter,
										pstTable->bUseExtId,
										pstTable->bUseVerNum,
										pstTable->bCrc,
										pstTable->usExtId,
										pstTable->ucVerNum,
										pstTable->ucMode,
										eContinueTimeout_Once,
										ulTimeout,
										pstTable->hAction,
										(PalSef_Callback_t)pfnParse,
										(PalSef_Callback_t)pfnFree);

	usFilterId = PAL_SEF_StartSecFilter(&stFilterParam);
	*pusFilterId = usFilterId;

	return ERR_OK;
}

HERROR svc_epg_SiCountPmtSection(svcEpg_RawPool_t *pstPool, HUINT16 usTsUniqId, HUINT32 *pulCount)
{
	HUINT16					 usCount, usAllUniqueId;
	HUINT32					 ulPmtSecCnt;
	svcEpg_RawSecTable_t	*pstTable;

	if (pstPool == NULL)	{ return ERR_FAIL; }
	if (pulCount == NULL)		{ return ERR_FAIL; }

	HxLOG_Info("(+) Ts Unique Id = 0x%x\n", usTsUniqId);

	usAllUniqueId = DB_SVC_GetAllUniqueId();
	pstTable = svc_epg_SiGetRawTable(pstPool, eSI_TableType_PMT);
	if (pstTable == NULL)		{ return ERR_FAIL; }

	ulPmtSecCnt = 0;
	for (usCount = pstTable->usStartPos; usCount < pstTable->usInsertPos; usCount++)
	{
		if (pstTable->phIdx[usCount] == SvcEpg_INVALID_INDEX_HANDLE)				{ continue; }

		// If there is no match in the unique ID, then it shall be skipped.
		if ((usTsUniqId != usAllUniqueId)
			&& (usTsUniqId != SvcEpg_GET_UNIQUEID_FROM_IDXHANDLE(pstTable->phIdx[usCount])))		{ continue; }

		ulPmtSecCnt++;
	}

	*pulCount = ulPmtSecCnt;
	HxLOG_Print("Pmt section count = %d\n", ulPmtSecCnt);

	HxLOG_Info("(-)\n");

	return ERR_OK;
}

HERROR svc_epg_SiGetPmtPidByServiceId(svcEpg_RawPool_t *pstPool,
									HUINT16 usTsUniqId,
									HUINT16 usSvcId,
									HUINT16 *pusPmtPid)
{
	HUINT16			 usPmtPid;
	HUINT32			 ulSecIdx, ulSecNum;
	HUINT8			*pucRaw;
	HERROR			 hErr;
	HBOOL			 bFound = FALSE;

	if (pstPool == NULL)		{ return ERR_FAIL; }
	if (pusPmtPid == NULL)		{ return ERR_FAIL; }

	HxLOG_Info("(+) Ts Unique Id = 0x%x\n", usTsUniqId);

	hErr = svc_epg_SiCountPatSection(pstPool, usTsUniqId, &ulSecNum);
	if (hErr != ERR_OK)					{ ulSecNum = 0; }

	bFound = FALSE;
	for (ulSecIdx = 0; ulSecIdx < ulSecNum; ulSecIdx++)
	{
		pucRaw = svc_epg_SiFindPatSectinTable(pstPool, usTsUniqId, ulSecIdx);
		if (pucRaw != NULL)
		{
			hErr = svc_epg_SiGetPmtPidByServiceIdFromRaw(pucRaw, usSvcId, &usPmtPid);
			if (hErr == ERR_OK)
			{
				*pusPmtPid = usPmtPid;
				bFound = TRUE;
				break;
			}
		}
	}

	hErr = (bFound) ? ERR_OK : ERR_FAIL;

	HxLOG_Info("(-) (Result : %s)\n", bFound ? "Found" : "Not Found");

	return hErr;
}

HERROR svc_epg_SiGetPmtProgramInfo(svcEpg_RawPool_t *pstPool,
											HUINT16 usTsUniqId,
											HUINT32 ulIdx,
											PmtProgramInfo_t *pstInfo)
{
	HUINT32		 hErr;
	HUINT8		*pucRaw;

	if (pstPool == NULL)			{ return ERR_FAIL; }
	if (pstInfo == NULL)			{ return ERR_FAIL; }

	HxLOG_Info("(+) Ts Unique Id = 0x%x\n", usTsUniqId);

	pucRaw = svc_epg_SiFindPmtSectionTable(pstPool, usTsUniqId, ulIdx);
	hErr = svc_epg_SiGetPmtProgramInfoFromRaw(pucRaw, pstInfo);

	HxLOG_Info("(-) (hErr : 0x%x)\n", hErr);

	return hErr;
}

HERROR svc_epg_SiCountPmtElement(svcEpg_RawPool_t *pstPool, HUINT16 usTsUniqId, HUINT32 ulIdx, HUINT32 *pulCount)
{
	HUINT32		 hErr;
	HUINT8		*pucRaw;

	if (pstPool == NULL)			{ return ERR_FAIL; }
	if (pulCount == NULL)				{ return ERR_FAIL; }

	HxLOG_Info("(+) Ts Unique Id = 0x%x\n", usTsUniqId);

	pucRaw = svc_epg_SiFindPmtSectionTable(pstPool, usTsUniqId, ulIdx);
	hErr = svc_epg_SiCountPmtElementFromRaw(pucRaw, pulCount);

	HxLOG_Info("(-) (hErr : 0x%x)\n", hErr);

	return hErr;
}

HERROR svc_epg_SiGetPmtElementInfo(svcEpg_RawPool_t *pstPool,
											HUINT16 usTsUniqId,
											HUINT32 ulIdx,
											HUINT32 ulSubIdx,
											PmtElementInfo_t *pstInfo)
{
	HERROR		 hErr;
	HUINT8		*pucRaw;

	if (pstPool == NULL)			{ return ERR_FAIL; }
	if (pstInfo == NULL)			{ return ERR_FAIL; }

	HxLOG_Info("(+) Ts Unique Id = 0x%x\n", usTsUniqId);

	pucRaw = svc_epg_SiFindPmtSectionTable(pstPool, usTsUniqId, ulIdx);
	hErr = svc_epg_SiGetPmtElementInfoFromRaw(pucRaw, ulSubIdx, pstInfo);

	HxLOG_Info("(-) (hErr : 0x%x)\n", hErr);

	return hErr;
}

HERROR svc_epg_SiCountPmtElementDescriptor(svcEpg_RawPool_t *pstPool,
													HUINT16 usTsUniqId,
													HUINT32 ulIdx,
													HUINT32 ulSubIdx,
													SIxDescriptorTag_e eTag,
													HUINT32 *pulCount)
{
	HERROR		 hErr;
	HUINT8		*pucRaw;

	if (pstPool == NULL)			{ return ERR_FAIL; }
	if (pulCount == NULL)			{ return ERR_FAIL; }

	HxLOG_Info("(+) Ts Unique Id = 0x%x\n", usTsUniqId);

	pucRaw = svc_epg_SiFindPmtSectionTable(pstPool, usTsUniqId, ulIdx);
	hErr = svc_epg_SiCountPmtElementDescriptorFromRaw(pucRaw, ulSubIdx, eTag, pulCount);

	HxLOG_Info("(-) (hErr : 0x%x)\n", hErr);

	return hErr;
}

HERROR svc_epg_SiGetPmtElementDescriptor(svcEpg_RawPool_t *pstPool,
											HUINT16 usTsUniqId,
											HUINT32 ulIdx,
											HUINT32 ulSubIdx,
											SIxDescriptorTag_e eTag,
											HUINT32 ulDesIdx,
											SIxDescriptor_t **ppstDes)
{
	HUINT32		 hErr;
	HUINT8		*pucRaw;

	if (pstPool == NULL)			{ return ERR_FAIL; }
	if (ppstDes == NULL)			{ return ERR_FAIL; }

	HxLOG_Info("(+) Ts Unique Id = 0x%x\n", usTsUniqId);

	pucRaw = svc_epg_SiFindPmtSectionTable(pstPool, usTsUniqId, ulIdx);
	hErr = svc_epg_SiGetPmtElementDescriptorFromRaw(pucRaw, ulSubIdx, eTag, ulDesIdx, ppstDes);

	HxLOG_Info("(-) (hErr : 0x%x)\n", hErr);

	return hErr;
}

/*********************** End of File ******************************/
