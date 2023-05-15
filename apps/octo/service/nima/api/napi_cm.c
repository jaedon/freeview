/** **********************************************************************************************************
	File 		: napi_cm.c
	author 		:
	comment		: Cable Model
	date    	: 2013/05/27
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
#include "napi_cm.h"
#include "napi_protocol.h"
#include "napi_cache.h"
#include "napi_agent_common.h"
#include "napi_debug.h"

#include <hlib.h>

/*-----------------------------------------------------------------------------------------------
 *	pre-definition
 *----------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------
 *	internal functions
 *----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/
HERROR NAPI_CM_GetDownStreamInfo(NAPICMDownStream_t *info)
{
	HERROR	eRet = ERR_OK;
/*
	NAPICMInfo_t *cminfo = NULL;
	NAPICMDownStream_t *downstream = NULL;
	napicache_t *cache = NULL;

	FuncEnter;

	cache = napi_cache_get_instance();
	if ( !cache ) return ERR_FAIL;

	cminfo = (NAPICMInfo_t *)napi_cache_netconf_get(cache, 0);
	if( !cminfo ) return ERR_FAIL;

	downstream = &(cminfo->down_stream);

	HxSTD_MemCopy(info, downstream, sizeof(info));

	FuncLeave;
*/
	return eRet;
}

HERROR NAPI_CM_SetDownStreamInfo(const NAPICMDownStream_t *info)
{
	HERROR eRet = ERR_OK;
	/*
	napipacket_t packet;

	FuncEnter;

	packet.type = NAPI_PROTOTYPE_REQUEST;
	packet.code = NAPI_PROTO_SET_CM_DOWNSTREAM;
	packet.dlen = sizeof(*info);
	HxSTD_MemCopy(packet.data, info, sizeof(NAPICMDownStream_t));

	eRet = napi_agent_write_data((HCHAR *)&packet, sizeof(packet)+packet.dlen);

	FuncLeave;
*/
	return eRet;
}

HERROR NAPI_CM_GetUpStreamInfo(NAPICMUpStream_t *info)
{
	HERROR	eRet = ERR_OK;
/*
	NAPICMInfo_t *cminfo = NULL;
	NAPICMUpStream_t *upstream = NULL;
	napicache_t *cache = NULL;

	FuncEnter;

	cache = napi_cache_get_instance();
	if ( !cache ) return ERR_FAIL;

	cminfo = (NAPICMInfo_t *)napi_cache_netconf_get(cache, 0);
	if( !cminfo ) return ERR_FAIL;

	upstream = &(cminfo->up_stream);

	HxSTD_MemCopy(info, upstream, sizeof(NAPICMUpStream_t));

	FuncLeave;

*/
	return eRet;
}

eNapiCmStatus NAPI_CM_GetStatus()
{
/*
	NAPICMInfo_t *cminfo = NULL;
	napicache_t *cache = NULL;

	FuncEnter;

	cache = napi_cache_get_instance();
	if ( !cache ) return eNAPI_CM_NONE;

	cminfo = (NAPICMInfo_t *)napi_cache_netconf_get(cache, 0);
	if( !cminfo ) return eNAPI_CM_NONE;

	FuncLeave;

	return cminfo->status;
	*/
	return 0;
}
