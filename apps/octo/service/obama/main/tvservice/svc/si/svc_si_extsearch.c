/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_si_extsearch.c
	@brief

	Description:  									\n
	Module: SVC / SI_NEW					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

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
 * (c) 2011-2013 Humax Co., Ltd.
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <silib.h>
#include <vkernel.h>
#include <octo_common.h>
#include <isosvc.h>

#include <pal_sirawpool.h>
#include <pal_pipe.h>
#include <svc_si.h>

#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <_svc_si_extsearch.h>
#include <psvc_si.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

#define _____LOCAL_FUNCTIONS_____

#define _____SEARCH_RESULT_DATA_____

#define _____MEMBER_FUNCTIONS_____

#define _____EXT_SEARCH_RESULT_DATA_____

HERROR svc_si_ExtSrchSetSrchOption(Handle_t hAction, SvcSi_ExtSearchSpec_t *pstExtSrchSpec, SvcSi_ExtSrchOptionType_t eOptionType, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	if (pstExtSrchSpec == NULL)
	{
		HxLOG_Error("Invalid parameter~~ \n");
		return ERR_FAIL;
	}

	return psvc_si_SetExtSearchOption(hAction, pstExtSrchSpec, eOptionType, ulParam1, ulParam2, ulParam3);
}

HERROR svc_si_ExtSrchGetSrchData(Handle_t hAction, SvcSi_ExtSearchSpec_t *pstExtSrchSpec, SvcSi_ExtSrchDataType_t eDataType, HUINT32 *pulDataNum, void **ppvDataArray)
{
	if ((pstExtSrchSpec == NULL) || (pulDataNum == NULL) || (ppvDataArray == NULL))
	{
		HxLOG_Error("Invalid parameter~~ \n");
		return ERR_FAIL;
	}

	return psvc_si_GetExtSearchData(hAction, pstExtSrchSpec, eDataType, pulDataNum, ppvDataArray);
}

HERROR svc_si_ExtSrchFreeSrchData(Handle_t hAction, SvcSi_ExtSearchSpec_t *pstExtSrchSpec, SvcSi_ExtSrchDataType_t eDataType, void *pvDataArray)
{
	if ((pstExtSrchSpec == NULL) || (pvDataArray == NULL))
	{
		HxLOG_Error("Invalid parameter~~ \n");
		return ERR_FAIL;
	}

	return psvc_si_FreeExtSearchData(hAction, pstExtSrchSpec, eDataType, pvDataArray);
}

/* end of file */

