/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_si_mheg.c
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
#include <util.h>

#include <pal_sirawpool.h>
#include <pal_pipe.h>
#include <svc_si.h>

#include <_svc_si.h>
#include <_svc_si_filtering.h>
#include <_svc_si_mheg.h>


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


#define ___LOCAL_FUNCTIONS___
STATIC HERROR svc_si_MhegGetRawSection(HUINT32 ulActionId, SIxTable_Type_e eSiType, HUINT32 ulRawArrayMax, HUINT8 **ppucRawArray, HUINT32 *pulListCount)
{
	HxLOG_Print("[%s:%d] Get Si RAW section +++ : ulRawArrayMax(%d) \n", __FUNCTION__, __LINE__, ulRawArrayMax);
	if ((pulListCount == NULL) || (ppucRawArray == NULL))			{ return ERR_FAIL; }

	*ppucRawArray = NULL;
	*pulListCount = 0;

	if (PAL_SIRAWPOOL_GetRawArrays(ulActionId, eSiType, FALSE, 0, ulRawArrayMax, ppucRawArray, pulListCount) != ERR_OK)
	{
		HxLOG_Print("[%s:%d] Fail to get ait raw section \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	HxLOG_Print("[%s:%d] Get Si RAW section --- : count(%d) \n", __FUNCTION__, __LINE__, pulListCount);
	return ERR_OK;
}

#define ___MEMBER_FUNCTIONS___

HERROR svc_si_MhegGetPatRawSection(HUINT32 ulActionId, HUINT32 ulRawArrayMax, HUINT8 **ppucRawArray, HUINT32 *pulListCount)
{
	return svc_si_MhegGetRawSection(ulActionId, eSIxTABLETYPE_Pat, ulRawArrayMax, ppucRawArray, pulListCount);
}

HERROR svc_si_MhegGetPmtRawSection(HUINT32 ulActionId, HUINT32 ulRawArrayMax, HUINT8 **ppucRawArray, HUINT32 *pulListCount)
{
	return svc_si_MhegGetRawSection(ulActionId, eSIxTABLETYPE_Pmt, ulRawArrayMax, ppucRawArray, pulListCount);
}

/* end of file */

