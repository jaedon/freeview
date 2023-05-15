/** **********************************************************************************************************
	File 		: napi_agent.c
	author 		:
	comment		:
	date    	: 2013/12/15
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

#include "napi_agent_common.h"
#include "napi_cache.h"
#include "napi_protocol.h"
#include "napi_parser.h"
#include "napi_def.h"
#include "../napi_debug.h"

#include <vkernel.h>
#include <hlib.h>
#include <papi.h>

napiagent_t *napi_agent_start(const HCHAR *addr)
{
	HINT32 i = 0;
	napiagent_info_t *obj = NULL;
	napiagent_t *agent = NULL;
	HxLOG_Debug("sec on mode !!! \n");
	FuncEnter;

	if ( !addr ) return NULL;

#ifndef USE_NAPI_CONSTRUCTOR /* ALM #297855: Dont use __attribute__((destructor)) by napi_deinit() crash */
	napi_agent_create_instance();
#endif
	agent = napi_agent_get_instance();
	obj = __agent_info(agent);
	if(obj == NULL)
	{
		obj = HLIB_STD_MemAlloc(sizeof(napiagent_info_t));
		if(obj == NULL)
		{
			return NULL;
		}
		HxSTD_MemSet(obj, 0x0, sizeof(*obj));
	}
	obj->bInit = FALSE;
	obj->bDBLoad = FALSE;
	obj->system_status = NAPI_SYSTEM_STATUS_Operation;
	for( i=0; i<eAGENT_EVENT_TYPE_MAX; i++ )
		obj->callbacks[i] = 0;
	HxSTD_MemSet(obj->uds_path, 0, sizeof(obj->uds_path));

	obj->socket = HLIB_EXTRA_OpenClient("nima");

	if ( obj->socket == 0 )
	{
		if( NULL != obj ) HLIB_STD_MemFree(obj);
		return NULL;
	}

	if ( VK_TASK_Create(napi_client, VK_TASK_PRIORITY_MW_REF, SIZE_64K, "NAPIClient",
			(void *)obj, &obj->tid, 0) != VK_OK
		|| VK_TASK_Start(obj->tid) != VK_OK ) return NULL;
	HxSTD_StrNCpy(obj->uds_path, addr, HxSTD_StrLen(addr)+1);

	FuncLeave;
	return (napiagent_t *)obj;
}

HERROR napi_agent_stop(napiagent_t *agent)
{
	HERROR eRet = ERR_OK;
	napiagent_info_t *obj = NULL;

	FuncEnter;
	obj = __agent_info(agent);
	if(obj == NULL)
		return ERR_FAIL;

	if( agent!= NULL )
		VK_TASK_Destroy(obj->tid);

	eRet = HLIB_SOCKET_Close(obj->socket);
#ifndef USE_NAPI_CONSTRUCTOR /* ALM #297855: Dont use __attribute__((destructor)) by napi_deinit() crash */
	napi_agent_destroy_instance();
#else
	HLIB_STD_MemFree(obj);
#endif
	obj = NULL;
	agent = NULL;

	FuncLeave;
	return eRet;
}

HERROR napi_agent_write_data(napiagent_t *agent, const HCHAR *data, HINT32 len)
{
	HERROR eRet = ERR_OK;
	napiagent_info_t *obj = NULL;
	napipacket_t * packet = (napipacket_t*) data;

	FuncEnter;

	HxLOG_Debug("+++++++++++++ REQUEST +++++++++++++++ \n");
	HxLOG_Debug("packet->sync: %s \n", packet->sync == TRUE?"true":"false");
	HxLOG_Debug("packet->code: %d \n", packet->code);
	HxLOG_Debug("packet->dlen: %d \n", packet->dlen);

	obj = __agent_info(agent);

	if(obj == NULL)
	{
		eRet = ERR_FAIL;
	}
	else
	{
		HLIB_SOCKET_Write(obj->socket, (void *)data, len, 0);
		HxLOG_Debug("  (#)data len : %d\n", len);
	}
	FuncLeave;
	return eRet;
}

HERROR napi_agent_init_wait()
{
	HERROR eRet = ERR_OK;
	HUINT32 starttick = 0;
	HUINT32 duration = 0;
	napiagent_info_t *obj = NULL;
	napiagent_t *agent = NULL;
	FuncEnter;

	agent = napi_agent_get_instance();
	obj = __agent_info(agent);

	if( obj->bInit )
		return ERR_OK;

	starttick = HLIB_STD_GetSystemTick();
	while( TRUE )
	{
		if(duration > NAPI_AGENT_TIMEOUT)
		{
			HxLOG_Debug("  (#)init timeout \n");
			eRet = ERR_FAIL;
			break;
		}

		if( obj->bInit )
		{
			HxLOG_Debug("  (#)init complete \n");
			eRet = ERR_OK;
			break;
		}

		duration = HLIB_STD_GetSystemTick() - starttick;
		HLIB_STD_TaskSleep(10);
	}

	FuncLeave;
	return eRet;
}

