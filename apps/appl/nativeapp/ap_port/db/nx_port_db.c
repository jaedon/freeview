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

/******************************************************************************/
/**
 * @file	  		nx_port_db.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#include <nx_port.h>


#define ________________RELATED_SYSTEM_VOLUME_________________

HERROR NX_PORT_DB_Remove(const HCHAR *pszKey)
{
	if (APK_DB_Remove(pszKey) != ERR_OK)
		return ERR_FAIL;

	return ERR_OK;
}

HERROR	NX_PORT_DB_SetInt(const HCHAR *pszKey, HINT32 nValue)
{
	if (APK_DB_SetInt(pszKey, nValue) != ERR_OK)
		return ERR_FAIL;

	return ERR_OK;
}

HERROR	NX_PORT_DB_GetInt(const HCHAR *pszKey, HINT32 *pnValue)
{
	if (APK_DB_GetInt(pszKey, pnValue) != ERR_OK)
		return ERR_FAIL;

	return ERR_OK;
}

HERROR	NX_PORT_DB_SetStr(const HCHAR *pszKey, const HCHAR *pszString)
{
	if (APK_DB_SetStr(pszKey, pszString) != ERR_OK)
		return ERR_FAIL;

	return ERR_OK;
}

HERROR	NX_PORT_DB_GetStr(const HCHAR *pszKey, HCHAR *pszString, HINT32 nMax)
{
	if (APK_DB_GetStr(pszKey, pszString, nMax) != ERR_OK)
		return ERR_FAIL;

	return ERR_OK;
}
/*
* APK_DB_SetInt_SYNC
* @Desc
* Install Wizard 종료후 APPKIT_UDB_MODULE 를 세팅을 DAPI_LazySet 으로 날리게 되면
* dama 에게 전달 1초 , dama 의 DB sync 2초 의 시간이 소비된다.
* 이 시간안에 A/c Power on off 를 하게 되면 APPKIT_UDB_MODULE 는 세팅되지 않는다..
* 아래와 같은 함수를Install Wizard  종료후인 USERDB_SYSTEM_B_FIRSTTIME_BOOT 호출에서 만 사용 하도록 함 ..
* SYNC 납용을 하면 dama 에 부담을 준다.
*/
HERROR	NX_PORT_DB_SetInt_SYNC(const HCHAR *pszKey, HINT32 nValue)
{
	/*if (APK_DB_SetInt_SYNC(pszKey, nValue) != ERR_OK)
		return ERR_FAIL;
	*/
	return ERR_OK;
}

