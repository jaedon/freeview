/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  vmx_vr_iptvh_dscr.c
	@brief

	Description:  									\n
	Module: 	OBAMA / TvService				\n

	Copyright (c) 2014 HUMAX Co., Ltd.				\n
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
//#include <pal_pipe.h>
//#include <_svc_cas_mgr_common.h>

#include "_xsvc_cas_vmx_dscr.h"
#include "vmx_vr_iptvh_dscr.h"
#include "vmx_vr_iptvh_mgr.h"
#include "_xsvc_cas_vmx_os.h"



/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


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

HUINT8 *enEncryptionAlgorithm_str[] =
{
	"NULL",
	"RC4",
	"AESECBT",
	"proprietary1",
	"AESECBL",
	"DVBCSA",
	"proprietary2",
	"AESCBC1"
};

int vmx_vr_iptvh_dscr_ControlWordCallback(void *pUserParam, descramble_param_t *dscParam)
{
	Vmx_Instance_t 		*pstInst = NULL;
	Vmx_ActionTable_t 	*pstAction = NULL;
	HUINT16 			 i = 0;
	HBOOL				 bFound = FALSE;

	HxLOG_Print("[Enter]\n");

	pstInst = xsvc_vmx_main_GetInstance();
	if(pstInst == NULL)
	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

	if ((pUserParam != NULL) && (dscParam != NULL))
	{
		HxLOG_Print("UserParam is exist\n");
		pstAction = (Vmx_ActionTable_t *)pUserParam;
		for (i = 0; i < VMX_CAS_MAX_NUM_OF_ACTION; i++)
		{
			if(pstAction == &pstInst->stActionList[i])
			{
				bFound = TRUE;
				break;
			}
		}

		if (bFound)
		{
			pstInst->stActionList[i].eEncrAlgo = dscParam->encAlg;

			HxLOG_Print("idx[%d] : keylength = %d\n", i, dscParam->keylength);
			HxLOG_Print("idx[%d] : iCwp_data_length = %d\n", i, dscParam->iCwp_data_length);
			HxLOG_Print("idx[%d] : encAlg[%d] = %s\n", i, dscParam->encAlg, enEncryptionAlgorithm_str[dscParam->encAlg]);

			xsvc_vmx_dscr_SetEncryptionAlgorithm(pstInst->stActionList[i].ucVmxServiceIdx, pstInst->stActionList[i].eEncrAlgo);
		}
		else
		{
			HxLOG_Error("Error! not found action! encAlg(%d)\n", dscParam->encAlg);
		}
	}
	else
	{
		HxLOG_Error("Error! Wrong param! pUserParam(%p) dscParam(%p) \n", pUserParam, dscParam);
	}

	HxLOG_Print("[Exit]\n");

	return ERR_OK;
}

/* end of file */