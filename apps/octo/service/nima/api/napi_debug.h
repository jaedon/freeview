/** **********************************************************************************************************
	File 		: napi_degub.h
	author 		:
	comment		: DEBUG
	date    	: 2013/08/12
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. ("Humax") and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*********************************************************************************************************** */


#ifndef __NAPI_DEBUG_H__
#define __NAPI_DEBUG_H__

#include <hlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------------------------------
 *	pre-definition
 *----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/
#if 0 //Enable for debugging in flash mode (vdkmai)
#define FuncEnter printf("[%s][%s][%d]  (+)Enter!!\n",HLIB_STD_GetCurProcessName(),__FUNCTION__, __LINE__);
#define FuncLeave printf("[%s][%s][%d]  (-)Leave!!\n",HLIB_STD_GetCurProcessName(),__FUNCTION__, __LINE__);
#else
#define FuncEnter //HxLOG_Debug("  (+)Enter!!\n")
#define FuncLeave //HxLOG_Debug("  (-)Leave -- \n")
#endif





void napi_debug_log_netconf(HINT32 id, void *info);
void napi_debug_log_ap(HINT32 id, HINT32 count, void *info);


#ifdef __cplusplus
}
#endif

#endif	/* __NAPI_DEBUG_H__ */


