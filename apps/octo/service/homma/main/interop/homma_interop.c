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

#include "homma_interop.h"

/*
 * methods
 *  - static & external
 */


/* ############################################################################
                          _
                         | |
  ___ __   __ ___  _ __  | |_    ___  ___   _ __ ___   _ __ ___    ___   _ __
 / _ \\ \ / // _ \| '_ \ | __|  / __|/ _ \ | '_ ` _ \ | '_ ` _ \  / _ \ | '_ \
|  __/ \ V /|  __/| | | || |_  | (__| (_) || | | | | || | | | | || (_) || | | |
 \___|  \_/  \___||_| |_| \__|  \___|\___/ |_| |_| |_||_| |_| |_| \___/ |_| |_|
 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  event common  xxxxxxxxx
############################################################################ */


// unwilling to use apps/octo/appkit/
#ifdef HOMMA_USE_APPKIT_DIRECTLY

//APK_EVENT_Dispatch()
HUINT32 HOMMA_INTEROP_EVENT_Dispatch(void)
{
	return APK_EVENT_Dispatch();
}

#else // #ifdef HOMMA_USE_APPKIT_DIRECTLY else

HERROR HOMMA_INTEROP_Init (void)
{
	HINT32			 nRpcHandle = 0;
	HERROR			 hErr;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if ((ERR_OK != hErr) || (0 == nRpcHandle))
	{
		HxLOG_Error("OAPI_RpcProtocol_GetRpcHandle err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

#endif // #ifdef HOMMA_USE_APPKIT_DIRECTLY endif

