/*************************************************************************************************************
	File 		: nico_operator_pppoe.c
	author 		: IP/OTT-IP
	comment		:
	date    	: 2013/11/12
	attention 	:

	(c) 2011-2013 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Humax) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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
*************************************************************************************************************/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "nico_operator_pppoe.h"

#include "di_pppoe.h"
#include "drv_err.h"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	PPPOE_RUNNING,
	PPPOE_STOPPED,
} e_pppoe_state;

typedef struct _nico_pppoe_context
{
	e_pppoe_state state;

	HINT32 device_count;
	NICO_NetDevice_Info_t **deviceinfo;
} st_pppoe_context;

typedef st_pppoe_context* pst_pppoe_context;

typedef struct _nico_pppoe_object
{
	st_pppoe_context obj_ctx;
	
	void  (*listener)(void *dimsg);
} st_pppoe_object;
typedef st_pppoe_object* pst_pppoe_object;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC st_pppoe_object g_pppoe_object;

/*******************************************************************/
/********************      Public			  ******************/
/*******************************************************************/
pst_pppoe_object pppoe_object = &g_pppoe_object;

/*******************************************************************/
/********************       Private Functions       ******************/
/*******************************************************************/
STATIC pst_pppoe_context __nico_pppoe_getcontext(void)
{
	return &pppoe_object->obj_ctx;
}

#if defined(CONFIG_MW_INET_PPPOE)

STATIC void __nico_pppoe_di_listener(void *param)
{
	ENTRY;

	if(pppoe_object->listener)
	{
		pppoe_object->listener(param);
	}
	EXIT;
}

STATIC void __nico_pppoe_send_extendevent(HINT32 id, HUINT32 event_type)
{
	DI_PPPOE_CALLBACK_DATA_t evt = { .ulDevId = id, .eEvent = event_type};
	ENTRY;

	switch(event_type)
	{
		case EVENT_NET_PPPOE_CONNECT_SUCCESS:
			evt.eEvent = DI_PPPOE_EVENT_CONNECT_SUCCESS;
			break;
		case EVENT_NET_PPPOE_CONNECT_FAIL:
			evt.eEvent = DI_PPPOE_EVENT_CONNECT_FAIL;
			break;
		case EVENT_NET_PPPOE_DISCONNECT_SUCCESS:
			evt.eEvent = DI_PPPOE_EVENT_DISCONNECT_SUCCESS;
			break;
		case EVENT_NET_PPPOE_DISCONNECT_FAIL:
			evt.eEvent = DI_PPPOE_EVENT_DISCONNECT_FAIL;
			break;
		case EVENT_NET_PPPOE_SET_INFO:
			evt.eEvent = DI_PPPOE_EVENT_SET_INFO;
			break;
		case EVENT_NET_PPPOE_SET_ONOFF:
			evt.eEvent = DI_PPPOE_EVENT_SET_ONOFF;
			break;
		default:
			HxLOG_Error("[%s] invalid event\n", __FUNCTION__);
			return;
	}
	
	__nico_pppoe_di_listener(&evt);
	EXIT;
}

psyncreturn_msg __nico_pppoe_operate_sync(command_msg *params)
{
	pst_pppoe_context ctx = __nico_pppoe_getcontext();
	psyncreturn_msg retmsg = HLIB_STD_MemCalloc(sizeof(syncreturn_msg));
	HINT32 i;
	HUINT32 dev_idx = 0;

	if(NULL == retmsg)
	{
		HxLOG_Error("Out of Memory +++ \n");
	}
	else
	{
		HxSTD_MemCopy(retmsg, params, sizeof(syncreturn_msg));
		retmsg->type.evt_type = params->type.cmd_type;

		for( i = 0; i < ctx->device_count; i++ )
		{
			if( ctx->deviceinfo[i]->netinfo.dev_type == eNICO_DEVTYPE_PPPOE )
			{
				dev_idx = i;
				break;
			}
		}
		
		switch(params->type.cmd_type)
		{
			case CMD_GET_PPPOE_INFO:
				{
					Cmd_PPPoE_InfoDB_t *ret_info = &retmsg->cmd_protocol.cmd_pppoe.cmd.cmd_get_infodb;
					DI_PPPOE_INFO_t pppoe_info;

					HxSTD_MemSet(&pppoe_info, 0x00, sizeof(DI_PPPOE_INFO_t));
					DI_PPPOE_GetInfo(dev_idx, &pppoe_info);
					if(retmsg)
					{
						ret_info->info.id = ctx->deviceinfo[dev_idx]->pppoeinfo.id;
						if( pppoe_info.bConnected == TRUE )
						{
							ctx->deviceinfo[dev_idx]->netinfo.active = TRUE;
							ctx->deviceinfo[dev_idx]->netinfo.linked = TRUE;
						}
						HxSTD_MemCopy(ret_info->info.connect_id, pppoe_info.aucId, NICO_PPPOE_ID_LEN);
						HxSTD_MemCopy(ret_info->info.connect_pw, pppoe_info.aucPassword, NICO_PPPOE_PW_LEN);

						HxSTD_MemCopy(ctx->deviceinfo[dev_idx]->pppoeinfo.connect_id, pppoe_info.aucId, NICO_PPPOE_ID_LEN);
						HxSTD_MemCopy(ctx->deviceinfo[dev_idx]->pppoeinfo.connect_pw, pppoe_info.aucPassword, NICO_PPPOE_PW_LEN);
 					}
 				}
				break;
			default:
				break;
		}
	}

	return retmsg;
}

void	__nico_pppoe_operate_async(command_msg *params)
{
	pst_pppoe_context ctx = __nico_pppoe_getcontext();
	Cmd_PPPoE_t *pcommand =  (Cmd_PPPoE_t*)&params->cmd_protocol.cmd_pppoe;
	HERROR hRet = ERR_OK;
	HINT32 i;
	
	HUINT32 dev_idx;

	for( i = 0; i < ctx->device_count; i++ )
	{
		if( ctx->deviceinfo[i]->netinfo.dev_type == eNICO_DEVTYPE_PPPOE )
		{
			dev_idx = i;
			break;
		}
	}

	switch(params->type.cmd_type)
	{
		case CMD_SET_PPPOE_INFO:
			{
				Cmd_PPPoE_InfoDB_t *cmdinfo = &pcommand->cmd.cmd_set_infodb;
				DI_PPPOE_INFO_t pppoe_info;
				HINT32 dev_id = cmdinfo->id;
		
				HxSTD_MemSet(&pppoe_info, 0x00, sizeof(DI_PPPOE_INFO_t));
				HxSTD_MemCopy(pppoe_info.aucId, cmdinfo->info.connect_id, sizeof(pppoe_info.aucId));
				HxSTD_MemCopy(pppoe_info.aucPassword, cmdinfo->info.connect_pw, sizeof(pppoe_info.aucPassword));
				if( DI_PPPOE_SetInfo(dev_id, &pppoe_info) == DI_ERR_OK )
				{
					HxSTD_MemCopy(ctx->deviceinfo[dev_idx]->pppoeinfo.connect_id, pppoe_info.aucId, NICO_PPPOE_ID_LEN);
					HxSTD_MemCopy(ctx->deviceinfo[dev_idx]->pppoeinfo.connect_pw, pppoe_info.aucPassword, NICO_PPPOE_PW_LEN);
				}

				__nico_pppoe_send_extendevent(dev_idx, EVENT_NET_PPPOE_SET_INFO);
			}
			break;
		
		case CMD_SET_PPPOE_ONOFF:
			{
				Cmd_PPPoE_Enable_t *cmdinfo = &pcommand->cmd.cmd_set_enable;
				ctx->deviceinfo[dev_idx]->pppoeinfo.enable = cmdinfo->enable;

				__nico_pppoe_send_extendevent(dev_idx, EVENT_NET_PPPOE_SET_ONOFF);
			}
			break;

		case CMD_SET_PPPOE_CONNECT:
			{
				Cmd_PPPoE_Connect *cmdinfo = &pcommand->cmd.cmd_set_connect;
				DI_PPPOE_INFO_t pppoe_info;
				HINT32 dev_id = cmdinfo->id;

				HxSTD_MemSet(&pppoe_info, 0x00, sizeof(DI_PPPOE_INFO_t));
				HxSTD_MemCopy(pppoe_info.aucId, cmdinfo->info.connect_id, sizeof(pppoe_info.aucId));
				HxSTD_MemCopy(pppoe_info.aucPassword, cmdinfo->info.connect_pw, sizeof(pppoe_info.aucPassword));
				hRet = DI_PPPOE_Connect(dev_id, &pppoe_info);
				if(ERR_OK != hRet)
					__nico_pppoe_send_extendevent(dev_id, EVENT_NET_PPPOE_CONNECT_FAIL);
			}
			break;
		case CMD_SET_PPPOE_DISCONNECT:
			{
				Cmd_PPPoE_Disconnect *cmdinfo = &pcommand->cmd.cmd_set_disconnect;
				HINT32 dev_id = cmdinfo->id;

				hRet = DI_PPPOE_Disconnect(dev_id);
				if(ERR_OK != hRet)
 					__nico_pppoe_send_extendevent(dev_id, EVENT_NET_PPPOE_DISCONNECT_FAIL);
			}
			break;
		default:
			break;
	}
}

HERROR nico_pppoe_init( HINT32 dev_count, NICO_NetDevice_Info_t **devices )
{
	HERROR hRet = ERR_OK;
	HUINT32	i = 0;
	pst_pppoe_context ctx = __nico_pppoe_getcontext();

	ENTRY;
	for(i=0; i < dev_count; i++)
	{
		if(eNICO_DEVTYPE_PPPOE == devices[i]->netinfo.dev_type)
		{
			hRet = DI_PPPOE_RegisterEventCallback(i, DI_PPPOE_EVENT_CONNECT_SUCCESS, __nico_pppoe_di_listener);
			hRet |= DI_PPPOE_RegisterEventCallback(i, DI_PPPOE_EVENT_CONNECT_FAIL, __nico_pppoe_di_listener);
			hRet |= DI_PPPOE_RegisterEventCallback(i, DI_PPPOE_EVENT_DISCONNECT_SUCCESS, __nico_pppoe_di_listener);
			hRet |= DI_PPPOE_RegisterEventCallback(i, DI_PPPOE_EVENT_DISCONNECT_FAIL, __nico_pppoe_di_listener);
			hRet |= DI_PPPOE_RegisterEventCallback(i, DI_PPPOE_EVENT_SET_INFO, __nico_pppoe_di_listener);
			hRet |= DI_PPPOE_RegisterEventCallback(i, DI_PPPOE_EVENT_SET_ONOFF, __nico_pppoe_di_listener);
			if(hRet != ERR_OK)
				HxLOG_Error("fail register pppoe callback function. \n");
		}
	}

	ctx->device_count = dev_count;
	ctx->deviceinfo = devices;
	EXIT;
	return hRet;
}
psyncreturn_msg nico_pppoe_operate( command_msg *params )
{
	psyncreturn_msg retmsg = NULL;
	ENTRY;
	if(TRUE == params->sync_call)
	{
		retmsg = __nico_pppoe_operate_sync(params);
	}
	else
	{
		__nico_pppoe_operate_async(params);
	}

	EXIT;
	return retmsg;
}

HERROR nico_pppoe_deinit( void )
{
	HERROR hRet = ERR_OK;
	ENTRY;
	if(pppoe_object) pppoe_object->listener = NULL;

	EXIT;
	return hRet;
}

HERROR nico_pppoe_register_callback( void *callback )
{	
	ENTRY;

	if( pppoe_object )
		pppoe_object->listener = callback;
	
	EXIT;
	return ERR_OK;
}

#else
HERROR nico_pppoe_init(  HINT32 dev_count, NICO_NetDevice_Info_t **devices ){return ERR_FAIL;}
psyncreturn_msg nico_pppoe_operate( command_msg *params ){return NULL;}
HERROR nico_pppoe_deinit( void ){return ERR_FAIL;}
HERROR nico_pppoe_register_callback( void *callback ){return ERR_FAIL;}

#endif

