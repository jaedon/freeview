/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_scenario_pincode.c
	@brief

	Description:  									\n
	Module: APPKITv2			\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
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

#include <hapi.h>
#include <oapi.h>

#include <apk.h>

#define ___INTERNAL_STRUCTURES___

#define ___INTERNAL_VARIABLES___
#define ___PRIVATE_FUNCTION___


#define _____NOTIFIER_FUNCTION_____

#define ___MEMBER_FUNCTION___

#define ___API_FUNCTION___
HERROR	APK_SCENARIO_PINCODE_Init(void)
{
	return OAPI_SCENARIO_PINCODE_Init();
}

HBOOL	APK_SCENARIO_PINCODE_Check(const HCHAR *pszPinStr)
{
	HBOOL	bChecked;

	if (OAPI_SCENARIO_PINCODE_Check(pszPinStr, &bChecked) == ERR_OK)
		return bChecked;

	return FALSE;
}

HBOOL	APK_SCENARIO_PINCODE_Verify(const HCHAR *pszPinStr)
{
	HBOOL	bVerified;

	if (OAPI_SCENARIO_PINCODE_Verify(pszPinStr, &bVerified) == ERR_OK)
		return bVerified;

	return FALSE;
}

HBOOL	APK_SCENARIO_PINCODE_Set(const HCHAR *pszOldPinStr, const HCHAR *pszNewPinStr)
{
	if (OAPI_SCENARIO_PINCODE_Set(pszOldPinStr, pszNewPinStr) == ERR_OK)
		return TRUE;

	return FALSE;
}

HBOOL	APK_SCENARIO_PINCODE_Reset(const HCHAR *pszNewPinStr)
{
	if (OAPI_SCENARIO_PINCODE_Reset(pszNewPinStr) == ERR_OK)
		return TRUE;

	return FALSE;
}

HBOOL	APK_SCENARIO_PINCODE_SetTemporaryBlock(const HCHAR *pszPinStr, HBOOL bBlocked)
{
	if (OAPI_SCENARIO_PINCODE_SetTemporaryBlock(pszPinStr, bBlocked) == ERR_OK)
		return TRUE;

	return FALSE;
}

HBOOL	APK_SCENARIO_PINCODE_GetTemporaryBlock(void)
{
	HBOOL	bBlocked;

	if (OAPI_SCENARIO_PINCODE_GetTemporaryBlock(&bBlocked) == ERR_OK)
		return bBlocked;

	return FALSE;
}
/* end of file */
