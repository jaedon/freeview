/**************************************************************
*	http://www.humaxdigital.com
*   @title		Contents Categorized Library
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
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
#define _________CLIB_PORT_Private_Include_________________________________________________
/**************************************************************************************************/
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <hlib.h>
#include <dlib.h>

#include "clib_int.h"

/**************************************************************************************************/
#define _________CLIB_PORT_Private_Macro_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________CLIB_PORT_Private_Struct_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________CLIB_PORT_Private_Value_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________CLIB_PORT_Private_Prototype_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________CLIB_PORT_Public_Functions__________________________________________________________
/**************************************************************************************************/
HUINT32		CLIB_PORT_StartSession(HxVector_t *pstSvcList, HUINT32 ulContentsType, HUINT32 ulOperatorType, HUINT32 ulExt)
{
	CListAttr_t		*pstAttr = NULL;

	switch (ulOperatorType)
	{
		case CLIB_OP_AUSDTT:
			pstAttr = CLIB_PORT_InitSessionAusDtt(ulContentsType, ulOperatorType, ulExt);
			break;
		case CLIB_OP_ME:
			pstAttr = CLIB_PORT_InitSessionMiddleEast(ulContentsType, ulOperatorType, ulExt);
			break;
		case CLIB_OP_ERTELECOM:
			pstAttr = CLIB_PORT_InitSessionErtelecom(ulContentsType, ulOperatorType, ulExt);
			break;
		case CLIB_OP_SES:
			pstAttr = CLIB_PORT_InitSessionSes(pstSvcList, ulContentsType, ulOperatorType, ulExt);
			break;
		case CLIB_OP_ZIGGO:
			pstAttr = CLIB_PORT_InitSessionZiggo(ulContentsType, ulOperatorType, ulExt);
			break;
		case CLIB_OP_UK_DTT:
			pstAttr = CLIB_PORT_InitSessionUkDtt(ulContentsType, ulOperatorType, ulExt);
			break;
		case CLIB_OP_ITA_DTT:
			pstAttr = CLIB_PORT_InitSessionItaDtt(ulContentsType, ulOperatorType, ulExt);
			break;
		case CLIB_OP_ORF:
			pstAttr = CLIB_PORT_InitSessionOrf(ulContentsType, ulOperatorType, ulExt);
			break;
		case CLIB_OP_TELENOR:
			pstAttr = CLIB_PORT_InitSessionTelenor(ulContentsType, ulOperatorType, ulExt);
			break;
		case CLIB_OP_TDC:
			pstAttr = CLIB_PORT_InitSessionTDC(ulContentsType, ulOperatorType, ulExt);
			break;
		case CLIB_OP_MBC:
			pstAttr = CLIB_PORT_InitSessionMBC(ulContentsType, ulOperatorType, ulExt);
			break;
		case CLIB_OP_AMX:
			pstAttr = CLIB_PORT_InitSessionAMX(ulContentsType, ulOperatorType, ulExt);
			break;
		case CLIB_OP_NULL:
		default:
			pstAttr = CLIB_PORT_InitSessionDefault(ulContentsType, ulOperatorType, ulExt);
			break;
	}

	HxLOG_Assert(pstAttr);

	pstAttr->ulContentsType = ulContentsType;
	pstAttr->ulOperatorType = ulOperatorType;
	pstAttr->ulExt          = ulExt;

	return (HUINT32)pstAttr;
}

void		CLIB_PORT_EndSession(HUINT32 ulSessionId)
{
	CListAttr_t		*pstAttr = (CListAttr_t*)ulSessionId;

	HLIB_STD_MemFree(pstAttr);
}

HBOOL		CLIB_PORT_FilterIn(HUINT32 ulSessionId, DxService_t *pstSvcInfo)
{
	CListAttr_t	*pstAttr = (CListAttr_t*)ulSessionId;

	switch (pstAttr->ulOperatorType)
	{
		case CLIB_OP_AUSDTT:
			return	CLIB_PORT_FilterInAusDtt(pstAttr, pstSvcInfo);
		case CLIB_OP_ME:
			return	CLIB_PORT_FilterInMiddleEast(pstAttr, pstSvcInfo);
		case CLIB_OP_ERTELECOM:
			return	CLIB_PORT_FilterInErtelecom(pstAttr, pstSvcInfo);
		case CLIB_OP_SES:
			return	CLIB_PORT_FilterInSes(pstAttr, pstSvcInfo);
		case CLIB_OP_ZIGGO:
			return	CLIB_PORT_FilterInZiggo(pstAttr, pstSvcInfo);
		case CLIB_OP_UK_DTT:
			return	CLIB_PORT_FilterInUkDtt(pstAttr, pstSvcInfo);
		case CLIB_OP_ITA_DTT:
			return	CLIB_PORT_FilterInItaDtt(pstAttr, pstSvcInfo);
		case CLIB_OP_ORF:
			return 	CLIB_PORT_FilterInOrf(pstAttr, pstSvcInfo);
		case CLIB_OP_TELENOR:
			return 	CLIB_PORT_FilterInTelenor(pstAttr, pstSvcInfo);
		case CLIB_OP_TDC:
			return 	CLIB_PORT_FilterInTDC(pstAttr, pstSvcInfo);
		case CLIB_OP_MBC:
			return 	CLIB_PORT_FilterInMBC(pstAttr, pstSvcInfo);
		case CLIB_OP_AMX:
			return 	CLIB_PORT_FilterInAMX(pstAttr, pstSvcInfo);
		case CLIB_OP_NULL:
		default:
			return CLIB_PORT_FilterInDefault(pstAttr, pstSvcInfo);
	}
}

HINT32		CLIB_PORT_GetServiceNumber(HUINT32 ulSessionId, DxService_t *pstSvcInfo, HINT32 idx)
{
	CListAttr_t	*pstAttr = (CListAttr_t*)ulSessionId;

	switch (pstAttr->ulOperatorType)
	{
		case CLIB_OP_AUSDTT:
			return CLIB_PORT_GetServiceNumberAusDtt(pstAttr, pstSvcInfo, idx);
		case CLIB_OP_ME:
			return CLIB_PORT_GetServiceNumberMiddleEast(pstAttr, pstSvcInfo, idx);
		case CLIB_OP_ERTELECOM:
			return CLIB_PORT_GetServiceNumberErtelecom(pstAttr, pstSvcInfo, idx);
		case CLIB_OP_SES:
			return CLIB_PORT_GetServiceNumberSes(pstAttr, pstSvcInfo, idx);
		case CLIB_OP_ZIGGO:
			return CLIB_PORT_GetServiceNumberZiggo(pstAttr, pstSvcInfo, idx);
		case CLIB_OP_UK_DTT:
			return CLIB_PORT_GetServiceNumberUkDtt(pstAttr, pstSvcInfo, idx);
		case CLIB_OP_ITA_DTT:
			return CLIB_PORT_GetServiceNumberItaDtt(pstAttr, pstSvcInfo, idx);
		case CLIB_OP_ORF:
			return CLIB_PORT_GetServiceNumberOrf(pstAttr, pstSvcInfo, idx);
		case CLIB_OP_TELENOR:
			return CLIB_PORT_GetServiceNumberTelenor(pstAttr, pstSvcInfo, idx);
		case CLIB_OP_TDC:
			return CLIB_PORT_GetServiceNumberTDC(pstAttr, pstSvcInfo, idx);
		case CLIB_OP_MBC:
			return CLIB_PORT_GetServiceNumberMBC(pstAttr, pstSvcInfo, idx);
		case CLIB_OP_AMX:
			return CLIB_PORT_GetServiceNumberAMX(pstAttr, pstSvcInfo, idx);
		case CLIB_OP_NULL:
		default:
			return CLIB_PORT_GetServiceNumberDefault(pstAttr, pstSvcInfo, idx);
	}
}

/**************************************************************************************************/
#define _________CLIB_PORT_CMD_FUNCTION_________________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________CLIB_PORT_End_Of_File____________________________________________________
/**************************************************************************************************/



