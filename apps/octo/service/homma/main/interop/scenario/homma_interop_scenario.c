/**************************************************************
*	@file	homma_interop.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			HOMMA / interoperation supporting instead
*					of using APK
**************************************************************/

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

/* ------------------------------------------------------------- //
 * Author : tjjang
 * Description : This is for replacing apk methods
 *  - HOMMA_INTEROP prefix is used for external methods
 *  - s_HOMMA_INTEROP prefix is used to represent static methods
 */

#include <homma_interop.h>

#ifdef HOMMA_USE_APPKIT_DIRECTLY
#else
#include <hapi.h>
#endif

/*
 * methods
 *  - static & external
 */


/* ############################################################################
                                     _
                                    (_)
 ___   ___  ___  _ __    __ _  _ __  _   ___
/ __| / __|/ _ \| '_ \  / _` || '__|| | / _ \
\__ \| (__|  __/| | | || (_| || |   | || (_) |
|___/ \___|\___||_| |_| \__,_||_|   |_| \___/
 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  scenario  xxxxxxxx
############################################################################ */


// unwilling to use apps/octo/appkit/
#ifdef HOMMA_USE_APPKIT_DIRECTLY

HERROR HOMMA_INTEROP_SCENARIO_Init(void)
{
	return APK_SCENARIO_Init();
}

HERROR HOMMA_INTEROP_SCENARIO_GetAntennaConnection(DxAntennaConnection_e *peConnectionType)
{
	return APK_SCENARIO_GetAntennaConnection(peConnectionType);
}

#else // #ifdef HOMMA_USE_APPKIT_DIRECTLY else

//APK_SCENARIO_Init();
HERROR HOMMA_INTEROP_SCENARIO_Init(void)
{
	return OAPI_SCENARIO_Init();
}

//APK_SCENARIO_GetAntennaConnection(&eAntCon);
HERROR HOMMA_INTEROP_SCENARIO_GetAntennaConnection(DxAntennaConnection_e *peConnectionType)
{
	return HAPI_GetRegistryEnum(OAPI_SCENARIO_ANTENNACONNECTION, "DxAntennaConnection_e", (HINT32*)peConnectionType);
}

#endif // #ifdef HOMMA_USE_APPKIT_DIRECTLY endif
