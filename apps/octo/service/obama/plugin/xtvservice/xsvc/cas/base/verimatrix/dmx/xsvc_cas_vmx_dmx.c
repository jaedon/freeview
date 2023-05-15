/*
 * (c) 2011-2015 Humax Co., Ltd. 
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
	@file     xsvc_cas_vmx_dmx.c
	@brief    xsvc_cas_vmx_dmx.c (Verimatrix DMX)

	Description:   \n
	Module: MW/CAS/ VERIMATRIX \n
	Remarks :\n

	Copyright (c) 2015 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "util.h"
#include "pal_sef.h"

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
#include "_xsvc_cas_vmx_iptv_main.h"
#elif defined(CONFIG_MW_CAS_VERIMATRIX_DVB)
#include "_xsvc_cas_vmx_bc_main.h"
#endif
#include <pal_pipe.h>
#include "_svc_cas_mgr_main.h"
#include "_svc_cas_mgr_common.h"
#include "_svc_cas_mgr_svc_port.h"

#include "_xsvc_cas_vmx_os.h"
#include "_xsvc_cas_vmx_dmx.h"


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


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/*
typedef enum
{
	eSvcXxx_Type,

} xsvcXxx_Type_e;

typedef  struct
{
	...
} xsvcXxx_Type_t;
*/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
/*	
// Local function
STATIC HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/

/*
// Global function
HERROR xsvc_xxx_DoSomething_Yyy( ... )
{
	...
}
*/
STATIC void vmx_dmx_EcmCallback(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, HUINT32 hAction)
{
	PalSef_RawData_t 	*pstSiRawData = (PalSef_RawData_t*)ulParam;
	HUINT16 	usIdx;
	HUINT16		usTableId;
	HUINT16 	usFilterId;
	HUINT16		usTsUniqId;
	HUINT16		usFilterResult;
	HUINT16  	usSecNum;

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

	pstSiRawData = (PalSef_RawData_t*)ulParam;

	for (usIdx = 0, usSecNum = 0; usIdx < CAS_MAX_SECTION_NUM && usSecNum < pstSiRawData->num_sec; usIdx++)
	{
		if (pstSiRawData->buf[usIdx] != NULL)
		{
			// 안에 들어가서 copy 할거니 그냥 넘기셔
			xsvc_vmx_psi_ecm_OnEcmReceived((HUINT32)usFilterId, pstSiRawData->buf[usIdx], hAction);

			usSecNum++;
		}
	}

	HxLOG_Print("[Exit][UniqueId(0x%x)] is Received \r\n", usTsUniqId);
}

STATIC void vmx_dmx_EmmCallback(HUINT32 ulTableIdAndResult, HUINT32 ulFilterAndUniqId, HUINT32 ulParam, HUINT32 hAction)
{
	PalSef_RawData_t 	*pstSiRawData = (PalSef_RawData_t*)ulParam;
	HUINT16 	usIdx;
	HUINT16		usTableId;
	HUINT16 	usFilterId;
	HUINT16		usTsUniqId;
	HUINT16		usFilterResult;
	HUINT16  	usSecNum;

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

	pstSiRawData = (PalSef_RawData_t*)ulParam;

	HxLOG_Print("pstSiRawData->num_sec = %d, 0x%x, 0x%x, 0x%x\n", pstSiRawData->num_sec, pstSiRawData->buf[0][0], pstSiRawData->buf[0][1], pstSiRawData->buf[0][2]);

	for (usIdx = 0, usSecNum = 0; usIdx < CAS_MAX_SECTION_NUM && usSecNum < pstSiRawData->num_sec; usIdx++)
	{
		if (pstSiRawData->buf[usIdx] != NULL)
		{
			// 안에 들어가서 copy 할거니 그냥 넘기셔
			xsvc_vmx_psi_emm_OnEmmReceived((HUINT32)usFilterId, pstSiRawData->buf[usIdx]);

			usSecNum++;
		}
	}

	HxLOG_Print("[Exit][UniqueId(0x%x)] is Received \r\n", usTsUniqId);
}

STATIC HERROR vmx_dmx_RetrieveEcmSection(Handle_t hStbHandle, HUINT16 usPid, HUINT8 *pucFilter, HUINT8 *pucMask, HUINT32 *pulFilterId)
{
	SvcCas_FilterParam_t 	stFilterParam;
	SvcCas_Context_t 		*pContext = NULL;
	Handle_t		hAction;
	HUINT32 	ulActionId;
	HUINT32		ulDemuxId;
	HUINT32		ulErrCode;
	HUINT16		usTsUniqueId, usFilterId;

	hAction = xsvc_vmx_main_GetActionHandleFromStbHdl(hStbHandle);
	ulActionId = GET_ACTION_ID_FROM_STBHANDLE(hStbHandle);
	HxLOG_Print("[Enter] Action[0x%x], Pid[0x%x]\n", hAction, usPid);

	pContext = svc_cas_MgrSubGetActionContextByActionId(ulActionId);
	if (hAction == HANDLE_NULL || pContext == NULL)
	{
		HxLOG_Print("Handle is invalid\r\n");
		return ERR_FAIL;
	}

	usTsUniqueId = pContext->usUniqueId;

	ulErrCode = svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId(hAction), eRxRSCTYPE_DEMUX, &ulDemuxId);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Print("RmGetResourceId error:0x%x, ts_id:0x%x\n", ulErrCode, hAction);
		return ulErrCode;
	}

	ulErrCode = svc_cas_SvcPortSiSetDefaultRetrieveParam(&stFilterParam, hAction, usTsUniqueId, (HUINT32)ulDemuxId);
	if(ulErrCode != ERR_OK)			HxLOG_Error("Critical Eror!!!\n");

	stFilterParam.bTableFilter = FALSE;
	stFilterParam.ucMode = eSiFilterMode_Continue;
	stFilterParam.ucTimeoutMode = eContinueTimeout_Repeat;
	stFilterParam.bCrc = FALSE;
	stFilterParam.bUseVerNum = FALSE;
	stFilterParam.usPid = usPid;

	HxLOG_Print("Filter info :  Mode[0x%x], TableFilter[0x%x], Crc[%d], Use_Ver_Num[%d], Pid [%x]\n", stFilterParam.ucMode,
																		stFilterParam.bTableFilter, stFilterParam.bCrc, stFilterParam.bUseVerNum, stFilterParam.usPid);
	ulErrCode = svc_cas_SvcPortSiRetrieveSectionBase(&stFilterParam,
												pucFilter[0],
												pucFilter,
												pucMask,
												8,
												0,	// timeout
												(void*)vmx_dmx_EcmCallback,
												NULL,
												&usFilterId);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Print("CARM_SIDMX_RetrieveSectionBase return ERROR (%x)\n", ulErrCode);
		return ulErrCode;
	}

	*pulFilterId = usFilterId;
	HxLOG_Print("[Exit] Filter_Id (0x%x)\n", usFilterId);
	return ERR_OK;
}

STATIC HERROR vmx_dmx_StopEcmSection( HUINT32 ulFilterId )
{
	HERROR nErr;

	HxLOG_Print("[Enter]Filter_Id (0x%x)\n", ulFilterId);

	nErr = svc_cas_SvcPortSiStopOneSection((HUINT16)ulFilterId);

	HxLOG_Print("[Exit] nErr(%d)\n", nErr);
	return nErr;
}

STATIC HERROR mx_dmx_RetrieveEmmSection(Handle_t hStbHandle, HUINT16 usPid, HUINT8 *pucFilter, HUINT8 *pucMask, HUINT32 *pulFilterId)
{
	SvcCas_FilterParam_t 	stFilterParam;
	SvcCas_Context_t 		*pContext = NULL;
	Handle_t		hAction;
	HUINT32 	ulActionId;	
	HUINT32		ulDemuxId;
	HUINT32		ulErrCode;
	HUINT16		usTsUniqueId, usFilterId;

	hAction = xsvc_vmx_main_GetActionHandleFromStbHdl(hStbHandle);
	ulActionId = GET_ACTION_ID_FROM_STBHANDLE(hStbHandle);	

	HxLOG_Print("[Enter] Action[0x%x], Pid[0x%x]\n", hAction, usPid);

	pContext = svc_cas_MgrSubGetActionContextByActionId(ulActionId);
	if (hAction == HANDLE_NULL || pContext == NULL)
	{
		HxLOG_Print("Handle is invalid\r\n");
		return ERR_FAIL;
	}

	usTsUniqueId = pContext->usUniqueId;

	ulErrCode = svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId(hAction), eRxRSCTYPE_DEMUX, &ulDemuxId);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Print("RmGetResourceId error:0x%x, ts_id:0x%x\n", ulErrCode, hAction);
		return ulErrCode;
	}

	ulErrCode = svc_cas_SvcPortSiSetDefaultRetrieveParam(&stFilterParam, hAction, usTsUniqueId, (HUINT32)ulDemuxId);
	if(ulErrCode != ERR_OK)			HxLOG_Error("Critical Eror!!!\n");

	stFilterParam.bTableFilter = FALSE;
	stFilterParam.ucMode = eSiFilterMode_Continue;
	stFilterParam.ucTimeoutMode = eContinueTimeout_Repeat;
	stFilterParam.bCrc = FALSE;
	stFilterParam.bUseVerNum = FALSE;
	stFilterParam.usPid = usPid;

	HxLOG_Print("Filter info :  Mode[0x%x], TableFilter[0x%x], Crc[%d], Use_Ver_Num[%d], Pid [%x]\n", stFilterParam.ucMode,
																		stFilterParam.bTableFilter, stFilterParam.bCrc, stFilterParam.bUseVerNum, stFilterParam.usPid);
	ulErrCode = svc_cas_SvcPortSiRetrieveSectionBase(&stFilterParam,
												pucFilter[0],
												pucFilter,
												pucMask,
												8,
												0,	// timeout
												(void*)vmx_dmx_EmmCallback,
												NULL,
												&usFilterId);
	if (ulErrCode != ERR_OK)
	{
		HxLOG_Print("CARM_SIDMX_RetrieveSectionBase return ERROR (%x)\n", ulErrCode);
		return ulErrCode;
	}

	*pulFilterId = usFilterId;
	HxLOG_Print("[Exit] Filter_Id (0x%x)\n", usFilterId);
	return ERR_OK;
}

STATIC HERROR vmx_dmx_StopEmmSection( HUINT32 ulFilterId )
{
	HERROR nErr;

	HxLOG_Print("[Enter]Filter_Id (0x%x)\n", ulFilterId);

	nErr = svc_cas_SvcPortSiStopOneSection((HUINT16)ulFilterId);

	HxLOG_Print("[Exit]\n");
	return nErr;
}

HERROR xsvc_vmx_dmx_RetrieveEcmSection(Handle_t hStbHandle, HUINT16 usPid, HUINT8 *pucFilter, HUINT8 *pucMask, HUINT32 *pulFilterId)
{
	return vmx_dmx_RetrieveEcmSection(hStbHandle, usPid, pucFilter, pucMask, pulFilterId);
}

HERROR xsvc_vmx_dmx_StopEcmSection( HUINT32 ulFilterId )
{
	return vmx_dmx_StopEcmSection(ulFilterId);
}

HERROR xsvc_vmx_dmx_RetrieveEmmSection(Handle_t hStbHandle, HUINT16 usPid, HUINT8 *pucFilter, HUINT8 *pucMask, HUINT32 *pulFilterId)
{
	return mx_dmx_RetrieveEmmSection(hStbHandle, usPid, pucFilter, pucMask, pulFilterId);
}

HERROR xsvc_vmx_dmx_StopEmmSection( HUINT32 ulFilterId )
{
	return vmx_dmx_StopEmmSection(ulFilterId);
}

