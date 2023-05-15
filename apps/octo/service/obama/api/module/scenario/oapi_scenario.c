/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_scenario.c
	@brief

	Description:  									\n
	Module: OAPI / API					\n

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
#include <vkernel.h>
#include <hlib.h>
#include <oapi.h>
#include <obama_int.h>

#include "../../oapi_mem.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	ENTER_SCENARIO		HxSEMT_Get(s_ulScenarioSemId)
#define	LEAVE_SCENARIO		HxSEMT_Release(s_ulScenarioSemId)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HUINT32		s_ulScenarioSemId = 0x0;
STATIC HINT32		s_nScenarioSr = 0x0;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___

#define _____NOTIFICATION_CALLBACKS_____

#define ___GLOBAL_FUNCTIONS___

HERROR	OAPI_SCENARIO_Init(void)
{
	HxSEMT_Create(&s_ulScenarioSemId, "scenario", HxSEMT_FIFO);

	ENTER_SCENARIO;
	s_nScenarioSr = HLIB_SERIALIZER_Open();
	LEAVE_SCENARIO;

	return ERR_OK;
}

HERROR	OAPI_SCENARIO_DeInit(void)
{
	ENTER_SCENARIO;
	if (s_nScenarioSr)
	{
		HLIB_SERIALIZER_Close(s_nScenarioSr); s_nScenarioSr = 0x0;
	}
	LEAVE_SCENARIO;
	HxSEMT_Destroy(s_ulScenarioSemId); s_ulScenarioSemId = 0x0;
	return ERR_OK;
}

HERROR	OAPI_SCENARIO_Set(const HCHAR *pszKey, const HCHAR *pszSignature, ...)
{
	void	*pvData;
	HUINT32	ulSize;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;
	va_list	ap;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	ENTER_SCENARIO;

	HLIB_SERIALIZER_Clear(s_nScenarioSr);
	HxSERIALIZER_Array(s_nScenarioSr);

	va_start(ap, pszSignature);
	while (*pszSignature != '\0')
	{
		const HCHAR *pszText;
		HINT32	nNumber;

		switch (*pszSignature++)
		{
			case 's':
				pszText = va_arg(ap, const HCHAR*);
				if (pszText == NULL)
					HxSERIALIZER_String(s_nScenarioSr, "");
				else
					HxSERIALIZER_String(s_nScenarioSr, pszText);
				break;
			case 'i':
				nNumber = va_arg(ap, HINT32);
				HxSERIALIZER_Integer(s_nScenarioSr, nNumber);
				break;
			case 'b':
				pvData = va_arg(ap, void *);
				ulSize = va_arg(ap, HUINT32);

				HxSERIALIZER_Binary(s_nScenarioSr, pvData, ulSize);
				break;
			default:
				break;
		}
	}
	va_end(ap);

	HxSERIALIZER_ArrayEnd(s_nScenarioSr);
	pvData = HLIB_SERIALIZER_Pack(s_nScenarioSr, &ulSize);
	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_SCENARIO_SET, "sb", pszKey, pvData, ulSize);

	LEAVE_SCENARIO;

	return ERR_OK;
}
