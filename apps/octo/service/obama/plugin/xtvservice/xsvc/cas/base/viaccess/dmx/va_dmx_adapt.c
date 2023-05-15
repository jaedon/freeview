/*
 * (c) 2011-2012 Humax Co., Ltd.
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
/**
	@file     va_dmx_adapt.c
	@brief    va_dmx_adapt.c (VIACCESS demux)

	Description: . demux adaptation layer \n
	Module: MW/CAS/ VIACCESS \n
	Remarks :\n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include "va_def.h"

#include "vamgr_main.h"
#include "_svc_cas_mgr_main.h"

#include "va_dmx_adapt.h"
#include <util.h>

#include "pal_sef.h"
#include <pal_pipe.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/




#define DMX_MACRO_GET_TABLEID_N_RESULT(a, b, c)			\
{														\
	c = (a & 0x0000ffff); 								\
	b = (a & 0xffff0000) >> 16;							\
}

#define DMX_MACRO_GET_FILTERID_N_TSUNIQID(a, b, c)		\
{														\
	c = a & 0x0000ffff; 								\
	b = (a & 0xffff0000) >> 16;							\
}

/*****************************************************************************
							typedef
******************************************************************************/

/*****************************************************************************
							external variable declaration
******************************************************************************/

/*****************************************************************************
							global/static variable declaration
******************************************************************************/


/*****************************************************************************
							static function prototype
******************************************************************************/

/*****************************************************************************
							global function body
******************************************************************************/

static void local_va_dmx_EcmCallback(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, HUINT32 hAction)
{
	PalSef_RawData_t 	*pstSiRawData = (PalSef_RawData_t*)ulParam;

	HUINT16 usIdx;
	HUINT16	usTableId;
	HUINT16 usFilterId;
	HUINT16	usTsUniqId;
	HUINT16	usFilterResult;
	HUINT16 usSecNum;

	DMX_MACRO_GET_TABLEID_N_RESULT(ulTableIdAndResult, usTableId, usFilterResult);
	DMX_MACRO_GET_FILTERID_N_TSUNIQID(ulFilterAndUniqId, usFilterId, usTsUniqId);

	HxLOG_Print("[Enter][Action(0x%x) TableId(0x%x), FilterId(0x%x), UniqueId(0x%x)]\n", hAction, usTableId, usFilterId, usTsUniqId);

	if (hAction == HANDLE_NULL)
	{
		HxLOG_Error("Input param is invalid\n");
		return;
	}

	if (usFilterId == 0xFFFF) /* VA_DMX_FILTER_LOADID_INVALID */
	{
		HxLOG_Error("Input param is invalid\n");
		return;
	}

	if (usFilterResult != eSiFilterResult_Complete)
	{
		HxLOG_Error("Input param is invalid (%d)\n", usFilterResult);
		return;
	}

	pstSiRawData = (PalSef_RawData_t*)ulParam;

	for (usIdx = 0, usSecNum = 0; usIdx < CAS_MAX_SECTION_NUM && usSecNum < pstSiRawData->num_sec; usIdx++)
	{
		HxLOG_Info("NumSec = %d : usIdx = %d, usSecNum = %d\n", pstSiRawData->num_sec, usIdx, usSecNum);

		if (pstSiRawData->buf[usIdx] != NULL)
		{
			VA_DMX_PutEcmToCa(pstSiRawData->buf[usIdx], usFilterId);
			usSecNum++;
		}
	}

	HxLOG_Print("[Exit][UniqueId(0x%x)] is Received \r\n", usTsUniqId);
}

static void local_va_dmx_EmmCallback(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, HUINT32 hAction)
{
	PalSef_RawData_t 	*pstSiRawData = (PalSef_RawData_t*)ulParam;

	HUINT16 usIdx;
	HUINT16	usTableId;
	HUINT16 usFilterId;
	HUINT16	usTsUniqId;
	HUINT16	usFilterResult;
	HUINT16 usSecNum;

#if defined(CONFIG_PROD_CPU_BCM)
	HUINT8	*pucSection = NULL;
	HUINT16	usLength = 0;
	HUINT32	ulOldCrc = 0, ulNewCrc = 0;
#endif

	DMX_MACRO_GET_TABLEID_N_RESULT(ulTableIdAndResult, usTableId, usFilterResult);
	DMX_MACRO_GET_FILTERID_N_TSUNIQID(ulFilterAndUniqId, usFilterId, usTsUniqId);

	HxLOG_Print("[Enter][Action(0x%x) TableId(0x%x), FilterId(0x%x), UniqueId(0x%x)]\n", hAction, usTableId, usFilterId, usTsUniqId);

	if (hAction == HANDLE_NULL)
	{
		HxLOG_Error("Input param is invalid\n");
		return;
	}

	if (usFilterId == 0xFFFF) /* VA_DMX_FILTER_LOADID_INVALID */
	{
		HxLOG_Error("Input param is invalid\n");
		return;
	}

	if (usFilterResult != eSiFilterResult_Complete)
	{
		HxLOG_Error("Input param is invalid (%d)\n", usFilterResult);
		return;
	}

	pstSiRawData = (PalSef_RawData_t*)ulParam;

	for (usIdx = 0, usSecNum = 0; usIdx < CAS_MAX_SECTION_NUM && usSecNum < pstSiRawData->num_sec; usIdx++)
	{
		HxLOG_Info("NumSec = %d : usIdx = %d, usSecNum = %d\n", pstSiRawData->num_sec, usIdx, usSecNum);

		if (pstSiRawData->buf[usIdx] != NULL)
		{
#if defined(CONFIG_PROD_CPU_BCM)
			/**
			  *   BRCM에서는 set filter할 때 crc check true로 하면 section syntax indicator=0인 경우에도 crc error로 처리되어 section이 올라오지 않는다
			  *   그래서 crc check false로 set filter를 하고, section syntax indicator=1인 경우엔 crc를 별도로 체크해 준다
			**/

			pucSection = pstSiRawData->buf[usIdx];
			usLength = Get12bit(pucSection + 1) + 3;  // section length
			ulOldCrc = ulNewCrc = 0;

			if ((pucSection[1] & 0x80) != 0)
			{
				// section syntax indicator = 1, private data with crc
				ulOldCrc = Get32bit(&pucSection[usLength-4]);
				ulNewCrc = HLIB_MATH_GetCrc32((void*)pucSection, usLength-4);	// crc32 부분을 제외하기 위해 -4

				HxLOG_Info("pucSection[1](0x%X) Len(%d) ulOldCrc(0x%X) ulNewCrc(0x%X)\n", pucSection[1], usLength, ulOldCrc, ulNewCrc);
			}

			if (ulOldCrc == ulNewCrc)	// crc가 valid한 경우에만 ca로 전달
#endif
			{
				VA_DMX_PutEmmToCa(pstSiRawData->buf[usIdx], usFilterId);
				usSecNum++;
			}

		}
	}

	HxLOG_Print("[Exit][UniqueId(0x%x)] is Received \r\n", usTsUniqId);
}

HERROR VA_DMX_RetrieveEcmSection(Handle_t hStbHandle, VA_DMX_Filter_t *pFilter)
{
	Handle_t				 hAction;
	HUINT32					 ulDemuxId;
	HUINT32					 ulErrCode;
	HUINT16					 usTsUniqueId;//, usFilterId;
	SvcCas_FilterParam_t	 stFilterParam;
	SvcCas_Context_t 		*pContext = NULL;
	HUINT8					 ucFilterLength;

	if (pFilter == NULL)
	{
		HxLOG_Error("pFilter is null !!\r\n");
		return ERR_FAIL;
	}

//## action handle 얻어오는 것은 수정해야됨
	hAction = VAMGR_GetActionHandle(hStbHandle);
	HxLOG_Print("[Enter] Action[0x%x], Pid[0x%x]\n", hAction, pFilter->usPid);

	pContext = svc_cas_MgrGetActionContextByAction(hAction);
	if (hAction == HANDLE_NULL || pContext == NULL)
	{
		HxLOG_Error("Handle is invalid\r\n");
		return ERR_FAIL;
	}

	usTsUniqueId = pContext->usUniqueId;

	ulErrCode = svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId(hAction), eRxRSCTYPE_DEMUX, &ulDemuxId);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Error("svc_cas_SubSvcRmGetResourceId error:0x%x, ts_id:0x%x\n", ulErrCode, hAction);
		return ulErrCode;
	}

	ulErrCode = svc_cas_SubSvcDmxSetDefaultRetrieveParam(&stFilterParam, hAction, usTsUniqueId, ulDemuxId);
	VA_ASSERT(ulErrCode == ERR_OK);

	stFilterParam.usPid = pFilter->usPid;
	stFilterParam.bTableFilter = FALSE;
#if defined(CONFIG_PROD_CPU_BCM)
	stFilterParam.bCrc = FALSE; // see VA_DMX_RetrieveEmmSection()
#else
	stFilterParam.bCrc = TRUE; // FALSE;
#endif

	HxLOG_Info("Filter info : Pid[%x], Mode[%d], Timeout[%d], TableFilter[%x], bUseVerNum[%d], Crc[%d]\n",
		stFilterParam.usPid, stFilterParam.ucMode, stFilterParam.ucTimeoutMode, stFilterParam.bTableFilter, stFilterParam.bUseVerNum, stFilterParam.bCrc);

	/* OCTO 에서는 처리가 되나 통상 DI 제약 사항으로 8 bytes 까지만 처리하도록 한다. 일반적으로 문제되지 않음. */
	ucFilterLength = (pFilter->ucFilterLength == 0 || pFilter->ucFilterLength > 8) ? 8 : pFilter->ucFilterLength;

	ulErrCode = svc_cas_SubSvcDmxRetrieveSectionBase(&stFilterParam,
												pFilter->aucValue[0],
												pFilter->aucValue,
												pFilter->aucMask,
												ucFilterLength,
												0,	// timeout
												(void*)local_va_dmx_EcmCallback,
												NULL,
												&pFilter->usLoadId);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Error("CARM_SIDMX_RetrieveSectionBase return ERROR (%x)\n", ulErrCode);
		return ulErrCode;
	}

	HxLOG_Print("[Exit] Filter_Id (0x%x)\n", pFilter->usLoadId);
	return ERR_OK;
}

HERROR VA_DMX_StopEcmSection( HUINT16 usFilterId )
{
	HERROR nErr;

	HxLOG_Print("[Enter] Filter_Id (0x%x)\n", usFilterId);

	nErr = svc_cas_SubSvcDmxStopOneSection(usFilterId);

	HxLOG_Print("[Exit]\n");
	return nErr;
}

HERROR VA_DMX_RetrieveEmmSection(Handle_t hStbHandle, VA_DMX_Filter_t *pFilter)
{
	Handle_t				 hAction;
	HUINT32					 ulDemuxId;
	HUINT32					 ulErrCode;
	HUINT16					 usTsUniqueId;//, usFilterId;
	SvcCas_FilterParam_t	 stFilterParam;
	SvcCas_Context_t 		*pContext = NULL;
	HUINT8					 ucFilterLength;

	if (pFilter == NULL)
	{
		HxLOG_Error("pFilter is null !!\r\n");
		return ERR_FAIL;
	}

//## action handle 얻어오는 것은 수정해야됨
	hAction = VAMGR_GetActionHandle(hStbHandle);
	HxLOG_Print("[Enter] Action[0x%x], Pid[0x%x]\n", hAction, pFilter->usPid);

	pContext = svc_cas_MgrGetActionContextByAction(hAction);
	if (hAction == HANDLE_NULL || pContext == NULL)
	{
		HxLOG_Error("Handle is invalid\r\n");
		return ERR_FAIL;
	}

	usTsUniqueId = pContext->usUniqueId;

	ulErrCode = svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId(hAction), eRxRSCTYPE_DEMUX, &ulDemuxId);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Error("svc_cas_SubSvcRmGetResourceId error:0x%x, ts_id:0x%x\n", ulErrCode, hAction);
		return ulErrCode;
	}

	ulErrCode = svc_cas_SubSvcDmxSetDefaultRetrieveParam(&stFilterParam, hAction, usTsUniqueId, ulDemuxId);
	VA_ASSERT(ulErrCode == ERR_OK);

	stFilterParam.usPid = pFilter->usPid;
	stFilterParam.ucMode = eSiFilterMode_Continue;
	stFilterParam.ucTimeoutMode = eContinueTimeout_Repeat;
	stFilterParam.bTableFilter = FALSE;
#if defined(CONFIG_PROD_CPU_BCM) // see local_va_dmx_EmmCallback()
	stFilterParam.bCrc = FALSE;
#else
	stFilterParam.bCrc = TRUE; // FALSE;
#endif

	HxLOG_Info("Filter info : Pid[%x], Mode[%d], Timeout[%d], TableFilter[%x], bUseVerNum[%d], Crc[%d]\n",
		stFilterParam.usPid, stFilterParam.ucMode, stFilterParam.ucTimeoutMode, stFilterParam.bTableFilter, stFilterParam.bUseVerNum, stFilterParam.bCrc);

	/* OCTO 에서는 처리가 되나 통상 DI 제약 사항으로 8 bytes 까지만 처리하도록 한다. 일반적으로 문제되지 않음. */
	ucFilterLength = (pFilter->ucFilterLength == 0 || pFilter->ucFilterLength > 8) ? 8 : pFilter->ucFilterLength;

	ulErrCode = svc_cas_SubSvcDmxRetrieveSectionBase(&stFilterParam,
												pFilter->aucValue[0],
												pFilter->aucValue,
												pFilter->aucMask,
												ucFilterLength,
												0,	// timeout
												(void*)local_va_dmx_EmmCallback,
												NULL,
												&pFilter->usLoadId);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Error("CARM_SIDMX_RetrieveSectionBase return ERROR (%x)\n", ulErrCode);
		return ulErrCode;
	}

	HxLOG_Print("[Exit] Filter_Id (0x%x)\n", pFilter->usLoadId);
	return ERR_OK;
}

HERROR VA_DMX_StopEmmSection( HUINT16 usFilterId )
{
	HERROR nErr;

	HxLOG_Print("[Enter] Filter_Id (0x%x)\n", usFilterId);

	nErr = svc_cas_SubSvcDmxStopOneSection(usFilterId);

	HxLOG_Print("[Exit]\n");
	return nErr;
}
