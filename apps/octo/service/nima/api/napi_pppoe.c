/** **********************************************************************************************************
	File 		: napi_pppoe.c
	author 		:
	comment		: Network Configuration
	date    	: 2013/11/11
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

#include "napi_pppoe.h"
#include "napi_cache.h"
#include "napi_agent_common.h"
#include "napi_protocol.h"
#include "napi_parser.h"
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
HERROR NAPI_PPPOE_RegisterCallback(NAPI_PPPOECallback callback)
{
	HERROR	eRet = ERR_OK;
	FuncEnter;

	eRet = napi_agent_register_callback(napi_agent_get_instance(), callback, eAGENT_EVENT_TYPE_PPPOE);

	FuncLeave;
	return eRet;
}

HERROR NAPI_PPPOE_Connect(const NAPIPPPOEInfo_t *info)
{
	HERROR	eRet = ERR_OK;
	napipacket_t *packet = NULL;
	NAPIProtocol_PPPOE_info_t *pppoe_info = NULL;
#if 0 // if pppoe off, it have not to connect.
	NAPIProtocol_PPPOE_info_t *cache_pppoe_info = NULL;
	napicache_t *cache = NULL;
#endif
	HINT32 data_size = 0;

	FuncEnter;

	if( !info ) return ERR_FAIL;

#if 0 // if pppoe off, it have not to connect.
	// for fast boot
	napi_agent_init_wait();
	cache = napi_cache_get_instance();
	if ( !cache ) return ERR_FAIL;
	cache_pppoe_info = napi_cache_pppoe_get(cache);
	if( cache_pppoe_info->bOn == NULL )
		return ERR_FAIL;
	if( cache_pppoe_info->bOn == FALSE )
	{
		HxLOG_Debug("	(#) pppoe off mode !!! \n");
		return ERR_FAIL;
	}
#endif

	data_size = sizeof(NAPIProtocol_PPPOE_info_t);
	pppoe_info = HLIB_STD_MemAlloc(data_size);
	if ( !pppoe_info ) return ERR_FAIL;
	HxSTD_MemSet(pppoe_info, 0x0, data_size);

	pppoe_info->dev_id = info->dev_id;
	HxSTD_MemCopy(pppoe_info->szUserID, info->szUserID, NAPI_PPPOE_USER_ID_LEN_MAX);
	HxSTD_MemCopy(pppoe_info->szUserPW, info->szUserPW, NAPI_PPPOE_USER_PW_LEN_MAX);

	packet = HLIB_STD_MemAlloc(sizeof(napipacket_t) + data_size);
	if ( !packet )
	{
		HLIB_STD_MemFree(pppoe_info);
		return ERR_FAIL;
	}
	HxSTD_MemSet(packet, 0x0, sizeof(napipacket_t) + data_size);
	packet->type = NAPI_PROTOTYPE_REQUEST;
	packet->code = NAPI_PROTO_PPPOE_CONNECT;
	packet->dlen = data_size;

	HxSTD_MemCopy(packet->data, pppoe_info, packet->dlen);

	eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, sizeof(napipacket_t)+packet->dlen);

	if( pppoe_info ) HLIB_STD_MemFree( pppoe_info );
	if( packet ) HLIB_STD_MemFree(packet);

	FuncLeave;
	return eRet;
}


HERROR NAPI_PPPOE_Disconnect(HINT32 id)
{
	HERROR	eRet = ERR_OK;
	napipacket_t *packet = NULL;
	HINT32 dev_id = 0;

	FuncEnter;

	packet = HLIB_STD_MemAlloc(sizeof(napipacket_t) + sizeof(HINT32));
	if ( !packet ) return ERR_FAIL;
	HxSTD_MemSet(packet, 0x0, sizeof(napipacket_t) + sizeof(HINT32));
	packet->type = NAPI_PROTOTYPE_REQUEST;
	packet->code = NAPI_PROTO_PPPOE_DISCONNECT;
	packet->dlen = sizeof(HINT32);

	dev_id = id;
	HxSTD_MemCopy(packet->data, &dev_id, packet->dlen);

	eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, sizeof(napipacket_t)+packet->dlen);

	HLIB_STD_MemFree(packet);

	return eRet;
}

HERROR NAPI_PPPOE_GetInfo(NAPIPPPOEInfo_t *info)
{
	HERROR	eRet = ERR_OK;
	NAPIPPPOEInfo_t *pppoe_info = NULL;
	napicache_t *cache = NULL;
	FuncEnter;

	// for fast boot
	napi_agent_init_wait();

	if(!info) return ERR_FAIL;
	cache = napi_cache_get_instance();

	if ( !cache ) return ERR_FAIL;
	pppoe_info = napi_cache_pppoe_get(cache);

	if(!pppoe_info) return ERR_FAIL;
	HxSTD_MemCopy(info, pppoe_info, sizeof(NAPIPPPOEInfo_t));

	FuncLeave;
	return eRet;
}

HERROR NAPI_PPPOE_SetInfo(const NAPIPPPOEInfo_t *info)
{
	HERROR	eRet = ERR_OK;
	napipacket_t *packet = NULL;
	NAPIProtocol_PPPOE_info_t *pppoe_info = NULL;

	HINT32 data_size = 0;
	FuncEnter;

	if(!info)
		return ERR_FAIL;

	data_size = sizeof(NAPIProtocol_PPPOE_info_t);

	pppoe_info = HLIB_STD_MemAlloc(data_size);

	if ( !pppoe_info ) return ERR_FAIL;
	HxSTD_MemSet(pppoe_info, 0x0, data_size);

	pppoe_info->dev_id  = info->dev_id;

	HxSTD_MemCopy(pppoe_info->szUserID, info->szUserID, NAPI_PPPOE_USER_ID_LEN_MAX);
	HxSTD_MemCopy(pppoe_info->szUserPW, info->szUserPW, NAPI_PPPOE_USER_PW_LEN_MAX);

	packet = HLIB_STD_MemAlloc(sizeof(napipacket_t) + sizeof(NAPIProtocol_PPPOE_info_t));
	if ( !packet )
	{
		if( pppoe_info ) HLIB_STD_MemFree(pppoe_info);
		return ERR_FAIL;
	}
	HxSTD_MemSet(packet, 0x0, sizeof(napipacket_t) + sizeof(NAPIProtocol_PPPOE_info_t));
	packet->type = NAPI_PROTOTYPE_REQUEST;
	packet->code = NAPI_PROTO_PPPOE_SET_INFO;
	packet->dlen = sizeof(NAPIProtocol_PPPOE_info_t);
	HxSTD_MemCopy(packet->data, pppoe_info, packet->dlen);
	eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, sizeof(napipacket_t)+packet->dlen);

	if( packet ) HLIB_STD_MemFree(packet);
	if( pppoe_info ) HLIB_STD_MemFree(pppoe_info);

	FuncLeave;
	return eRet;
}

HERROR NAPI_PPPOE_GetOnOff(HINT32 id, HBOOL *bOn)
{
	HERROR	eRet = ERR_OK;
	NAPIPPPOEInfo_t *pppoe_info = NULL;
	napicache_t *cache = NULL;

	FuncEnter;

	// for fast boot
	napi_agent_init_wait();

	cache = napi_cache_get_instance();
	if ( cache == NULL ) return ERR_FAIL;

	pppoe_info = (NAPIPPPOEInfo_t *)napi_cache_pppoe_get(cache);
	if( !pppoe_info )
	{
		HxLOG_Debug("	(#)have no pppoe info !!! \n");
		return ERR_FAIL;
	}

	if( !bOn )
	{
		HxLOG_Debug("	(#)pointer null \n");
		return ERR_FAIL;
	}
	*bOn = pppoe_info->bOn;

	HxLOG_Debug("	(#)get bOn : %d \n", *bOn);

	FuncLeave;
	return eRet;
}

HERROR NAPI_PPPOE_SetOnOff(HINT32 id, HBOOL bOn)
{
	HERROR	eRet = ERR_OK;
	napipacket_t *packet = NULL;

	FuncEnter;

	packet = HLIB_STD_MemAlloc(sizeof(napipacket_t) + sizeof(HINT32) + sizeof(HBOOL));
	if( packet == NULL)
	{
		HxLOG_Error("	(#)pointer null \n");
		return ERR_FAIL;
	}

	packet->type = NAPI_PROTOTYPE_REQUEST;
	packet->code = NAPI_PROTO_PPPOE_SET_ONOFF;
	packet->dlen = sizeof(HINT32) + sizeof(HBOOL);

	HxSTD_MemCopy(packet->data, &id, sizeof(HINT32));
	HxSTD_MemCopy(packet->data + sizeof(HINT32), &bOn, sizeof(HBOOL));
	eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, sizeof(napipacket_t)+packet->dlen);

	HLIB_STD_MemFree(packet);

	FuncLeave;
	return eRet;
}




