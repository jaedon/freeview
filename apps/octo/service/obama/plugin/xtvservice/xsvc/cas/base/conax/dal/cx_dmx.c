/*******************************************************************
	File Description
*******************************************************************/
/*++
	@file   	 cx_dmx.c
	@brief  	 Demux Adaptation Layer

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
--*/
/*******************************************************************/
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
/*******************************************************************/
#define ________CX_DMX_Private_Include________________________________________
/*******************************************************************/
#include <conax.h>
#include <pal_sef.h>
#include <_svc_cas_mgr_main.h>
#include <_svc_cas_sub_svc_api.h>
#include <_svc_cas_mgr_svc_port.h>
//#include <_svc_cas_mgr_common.h>
#include "../local_include/cx_dmx.h"

#include <cxmgr_action.h>

/*******************************************************************/
#define ________CX_DMX_Golbal_Value___________________________________________
/*******************************************************************/
/*******************************************************************/
#define ________CX_DMX_Private_Define_________________________________________
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

/*******************************************************************/
#define ________CX_DMX_Private_Static_Value___________________________________
/*******************************************************************/
STATIC Cx_DmxEventNotifyCB_t	s_cbEcmFunc	= NULL;
STATIC Cx_DmxEventNotifyCB_t	s_cbEmmFunc	= NULL;

/*******************************************************************/
#define ________CX_DMX_Global_Value___________________________________________
/*******************************************************************/
/*******************************************************************/
#define ________CX_DMX_Private_Static_Prototype_______________________________
/*******************************************************************/
/*******************************************************************/
#define ________CX_DMX_Private_Static_Prototype_Body__________________________
/*******************************************************************/
void local_cxDalDmx_EcmCallback(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, HUINT32 hAction)
{
	PalSef_RawData_t* 	pstSiRawData	= (PalSef_RawData_t*)ulParam;

	HUINT16				usIdx;
	HUINT16				usTableId;
	HUINT16				usFilterId;
	HUINT16				usTsUniqId;
	HUINT16				usFilterResult;
	HUINT16				usSecNum;

	DMX_MACRO_GET_TABLEID_N_RESULT(ulTableIdAndResult, usTableId, usFilterResult);
	DMX_MACRO_GET_FILTERID_N_TSUNIQID(ulFilterAndUniqId, usFilterId, usTsUniqId);

	HxLOG_Print("[Enter]Action(0x%x) TableId(0x%x), FilterId(0x%x), UniqueId(0x%x)\n", hAction, usTableId, usFilterId, usTsUniqId);

	if (hAction == HANDLE_NULL)
	{
		HxLOG_Print("Input param is invalid\n");
		return;
	}

	//	if (usFilterId == 0xFFFF || usFilterResult == eSiFilterResult_Timeout || usFilterResult == eSiFilterResult_Error)
	if (usFilterResult != eSiFilterResult_Complete)
	{
		HxLOG_Print("Input param is invalid\n");
		return;
	}


	pstSiRawData = (PalSef_RawData_t *)ulParam;

	for (usIdx = 0, usSecNum = 0; usIdx < CAS_MAX_SECTION_NUM && usSecNum < pstSiRawData->num_sec; usIdx++)
	{
		if (pstSiRawData->buf[usIdx] != NULL)
		{
			// 안에 들어가서 copy 할거니 그냥 넘기셔
			if (s_cbEcmFunc != NULL)
			{
				s_cbEcmFunc((HUINT32)usFilterId, pstSiRawData->buf[usIdx]);
			}
			usSecNum++;
		}
	}

	HxLOG_Print("[Exit][UniqueId(0x%x)] is Received \r\n", usTsUniqId);
}

void local_cxDalDmx_EmmCallback(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, HUINT32 hAction)
{
	PalSef_RawData_t* 	pstSiRawData	= (PalSef_RawData_t*)ulParam;

	HUINT16				usIdx;
	HUINT16				usTableId;
	HUINT16				usFilterId;
	HUINT16				usTsUniqId;
	HUINT16				usFilterResult;
	HUINT16				usSecNum;

	DMX_MACRO_GET_TABLEID_N_RESULT(ulTableIdAndResult, usTableId, usFilterResult);
	DMX_MACRO_GET_FILTERID_N_TSUNIQID(ulFilterAndUniqId, usFilterId, usTsUniqId);

	HxLOG_Print("[Enter]Action(0x%x) TableId(0x%x), FilterId(0x%x), UniqueId(0x%x)\n", hAction, usTableId, usFilterId, usTsUniqId);

	if (hAction == HANDLE_NULL)
	{
		HxLOG_Print("Input param is invalid\n");
		return;
	}

	//	if (usFilterId == 0xFFFF || usFilterResult == eSiFilterResult_Timeout || usFilterResult == eSiFilterResult_Error)
	if (usFilterResult != eSiFilterResult_Complete)
	{
		HxLOG_Print("Input param is invalid\n");
		return;
	}


	pstSiRawData = (PalSef_RawData_t *)ulParam;

	for (usIdx = 0, usSecNum = 0; usIdx < CAS_MAX_SECTION_NUM && usSecNum < pstSiRawData->num_sec; usIdx++)
	{
		if (pstSiRawData->buf[usIdx] != NULL)
		{
			// 안에 들어가서 copy 할거니 그냥 넘기셔
			if (s_cbEmmFunc != NULL)
			{
				s_cbEmmFunc((HUINT32)usFilterId, pstSiRawData->buf[usIdx]);
			}
			usSecNum++;
		}
	}

	HxLOG_Print("[Exit][UniqueId(0x%x)] is Received \r\n", usTsUniqId);
}

HERROR cxDalDmx_RetrieveEcmSection(Handle_t hStbHandle, HUINT16 usPid, HUINT8* pucFilter, HUINT8* pucMask, HUINT32* pulFilterId)
{
	Handle_t				hAction;
	HUINT32					ulActionId;
	HUINT32					ulDemuxId;
	HUINT32					ulErrCode;
	HUINT16					usTsUniqueId, usFilterId;
	SvcCas_FilterParam_t	stCasMgrFilterParam;
	SvcCas_Context_t* 		pContext	= NULL;

	hAction = CXACT_GetActionHandle(hStbHandle);
	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("Action is HANDLE_NULL\n");
		return ERR_FAIL;
	}

	ulActionId = svc_cas_SubSvcRmGetActionId(hAction);
	pContext = svc_cas_MgrSubGetActionContextByActionId(ulActionId);
	if (pContext == NULL)
	{
		HxLOG_Error("CAS context is NULL\n");
		return ERR_FAIL;
	}

	usTsUniqueId = pContext->usUniqueId;

	ulErrCode = svc_cas_SubSvcRmGetResourceId(ulActionId, eCAS_RM_ITEM_DEMUX, &ulDemuxId);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Error("svc_cas_SubSvcRmGetResourceId error:0x%x, ts_id:0x%x\n", ulErrCode, hAction);
		return ulErrCode;
	}

	ulErrCode = svc_cas_SvcPortSiSetDefaultRetrieveParam(&stCasMgrFilterParam, hAction, usTsUniqueId, (HUINT32)ulDemuxId);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Critical("Critical Eror!!!\n");
	}

	stCasMgrFilterParam.bTableFilter = FALSE;
	stCasMgrFilterParam.bCrc = FALSE;
	stCasMgrFilterParam.bUseVerNum = FALSE;
	stCasMgrFilterParam.usPid = usPid;

	HxLOG_Print("Filter info :  Mode[0x%x], TableFilter[0x%x], Crc[%d], Use_Ver_Num[%d], Pid [%x]\n", stCasMgrFilterParam.ucMode, stCasMgrFilterParam.bTableFilter, stCasMgrFilterParam.bCrc, stCasMgrFilterParam.bUseVerNum, stCasMgrFilterParam.usPid);

	ulErrCode = svc_cas_SvcPortSiRetrieveSectionBase(&stCasMgrFilterParam,	//
												pucFilter[0],			//
												pucFilter,				//
												pucMask,				//
												8,						//
												0,						// timeout
												(void*)local_cxDalDmx_EcmCallback, //
												NULL,					//
												&usFilterId);			//
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Error("svc_cas_SvcPortSiRetrieveSectionBase return ERROR (%x)\n", ulErrCode);
		return ulErrCode;
	}

	*pulFilterId = usFilterId;
	HxLOG_Print("[Exit] Filter_Id (0x%x)\n", usFilterId);
	return ERR_OK;
}

HERROR cxDalDmx_StopEcmSection(HUINT32 ulFilterId)
{
	HERROR	nErr;

	HxLOG_Print("[Enter]Filter_Id (0x%x)\n", ulFilterId);

	nErr = svc_cas_SvcPortSiStopOneSection((HUINT16)ulFilterId);

	HxLOG_Print("[Exit]\n");
	return nErr;
}

HERROR cxDalDmx_RetrieveEmmSection(Handle_t hStbHandle, HUINT16 usPid, HUINT8* pucFilter, HUINT8* pucMask, HUINT32* pulFilterId)
{
	Handle_t				hAction;
	HUINT32					ulActionId;
	HUINT32					ulDemuxId;
	HUINT32					ulErrCode;
	HUINT16					usTsUniqueId, usFilterId;
	SvcCas_FilterParam_t	stCasMgrFilterParam;
	SvcCas_Context_t* 		pContext	= NULL;

	hAction = CXACT_GetActionHandle(hStbHandle);
	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("Action is HANDLE_NULL\n");
		return ERR_FAIL;
	}

	ulActionId = svc_cas_SubSvcRmGetActionId(hAction);
	pContext = svc_cas_MgrSubGetActionContextByActionId(ulActionId);
	if (pContext == NULL)
	{
		HxLOG_Error("CAS context is NULL\n");
		return ERR_FAIL;
	}

	usTsUniqueId = pContext->usUniqueId;

	ulErrCode = svc_cas_SubSvcRmGetResourceId(ulActionId, eCAS_RM_ITEM_DEMUX, &ulDemuxId);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Error("svc_cas_SubSvcRmGetResourceId() returns fail ERR : 0x%08x, ts_id:0x%x\n", ulErrCode, hAction);
		return ulErrCode;
	}

	ulErrCode = svc_cas_SvcPortSiSetDefaultRetrieveParam(&stCasMgrFilterParam, hAction, usTsUniqueId, (HUINT32)ulDemuxId);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Critical("Critical Eror!!!\n");
	}

	stCasMgrFilterParam.bTableFilter = FALSE;
	stCasMgrFilterParam.ucMode = eSiFilterMode_Continue;
	stCasMgrFilterParam.ucTimeoutMode = eContinueTimeout_Repeat;
	stCasMgrFilterParam.bCrc = FALSE;
	stCasMgrFilterParam.bUseVerNum = FALSE;
	stCasMgrFilterParam.usPid = usPid;

	HxLOG_Print("Filter info :  Mode[0x%x], TableFilter[%s], Crc[%d], Use_Ver_Num[%d], Pid [%x]\n", stCasMgrFilterParam.ucMode, (stCasMgrFilterParam.bTableFilter == TRUE ? "TRUE" : "FALSE"), stCasMgrFilterParam.bCrc, stCasMgrFilterParam.bUseVerNum, stCasMgrFilterParam.usPid);
	HxLOG_Print("[FILTER] %02X %02X %02X %02X %02X %02X %02X %02X\n", pucFilter[0], pucFilter[1], pucFilter[2], pucFilter[3], pucFilter[4], pucFilter[5], pucFilter[6], pucFilter[7]);
	HxLOG_Print("[ MASK ] %02X %02X %02X %02X %02X %02X %02X %02X\n", pucMask[0], pucMask[1], pucMask[2], pucMask[3], pucMask[4], pucMask[5], pucMask[6], pucMask[7]);

	ulErrCode = svc_cas_SvcPortSiRetrieveSectionBase(&stCasMgrFilterParam,	//
												pucFilter[0],			//
												pucFilter,				//
												pucMask,				//
												8,						//
												0,						// timeout
												(void*)local_cxDalDmx_EmmCallback, //
												NULL,					//
												&usFilterId);			//
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Error("svc_cas_SvcPortSiRetrieveSectionBase return ERROR (%x)\n", ulErrCode);
		return ulErrCode;
	}

	*pulFilterId = usFilterId;
	HxLOG_Print("[Exit] Filter_Id (0x%x)\n", usFilterId);
	return ERR_OK;
}

HERROR cxDalDmx_StopEmmSection(HUINT32 ulFilterId)
{
	HERROR	nErr;
	ENTER_FUNCTION;

	HxLOG_Print("Filter_Id (0x%x)\n", ulFilterId);
	nErr = svc_cas_SvcPortSiStopOneSection((HUINT16)ulFilterId);

	LEAVE_FUNCTION;
	return nErr;
}

void cxDalDmx_Init(void)
{
	ENTER_FUNCTION;
	LEAVE_FUNCTION;
}
/*******************************************************************/
#define ________CX_DMX_Public_Functions_Body__________________________________
/*******************************************************************/
void CXDMX_Init(void)
{
	return cxDalDmx_Init();
}
HERROR CXDMX_RetrieveEcmSection(Handle_t hStbHandle, HUINT16 usPid, HUINT8* pucFilter, HUINT8* pucMask, HUINT32* pulFilterId)
{
	return cxDalDmx_RetrieveEcmSection(hStbHandle, usPid, pucFilter, pucMask, pulFilterId);
}
HERROR CXDMX_StopEcmSection(HUINT32 ulFilterId)
{
	return cxDalDmx_StopEcmSection(ulFilterId);
}
HERROR CXDMX_RetrieveEmmSection(Handle_t hStbHandle, HUINT16 usPid, HUINT8* pucFilter, HUINT8* pucMask, HUINT32* pulFilterId)
{
	return cxDalDmx_RetrieveEmmSection(hStbHandle, usPid, pucFilter, pucMask, pulFilterId);
}
HERROR CXDMX_StopEmmSection(HUINT32 ulFilterId)
{
	return cxDalDmx_StopEmmSection(ulFilterId);
}
void CXDMX_RegisterEcmCb(Cx_DmxEventNotifyCB_t cbEcmFunc)
{
	s_cbEcmFunc = cbEcmFunc;
}
void CXDMX_RegisterEmmCb(Cx_DmxEventNotifyCB_t cbEmmFunc)
{
	s_cbEmmFunc = cbEmmFunc;
}
