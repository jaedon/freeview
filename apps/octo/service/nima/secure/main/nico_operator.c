/*************************************************************************************************************
	File 		: nico_operator.c
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2013/05/15
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Humax) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED 징째AS IS징짹 AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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
#include "nico_debug.h"
#include "nico_operator.h"
#include "nico_operator_wifi.h"
#include "nico_operator_netconf.h"
#include "nico_operator_cm.h"
#include "nico_operator_bluetooth.h"
#include "nico_operator_pppoe.h"
#include "nico_parser.h"


#include "di_err.h"
#include "di_bt.h"
#include "di_network.h"
#include "di_cm.h"
#include "di_wifi.h"

/*******************************************************************/
/*******************      template         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	ENTER_CRITICAL		HxSEMT_Get(operator_object->obj_ctx.semaphore)
#define	LEAVE_CRITICAL		HxSEMT_Release(operator_object->obj_ctx.semaphore)

#define CHECK_DEVICE(cmd)    \
	{ \
		pst_operator_context ctx = __nico_operator_getcontext(); \
		if ( cmd->device_id < 0 || cmd->device_id > ctx->device_count-1 ) \
		{ \
			HxLOG_Error("Invalid Device Setting !!! \n"); \
			return NULL; \
		} \
	} \

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
enum {
	OP_MSGQ_SIZE	= 32,
	OP_MSGQ_TIMEOUT	= 500,
};

typedef enum
{
	OPERATOR_RUN,
	OPERATOR_STOP,
} e_operator_state;

typedef struct _nico_operator_context
{
	e_operator_state state;
	HULONG cmd_qid;
	HULONG cmd_tid;

	HULONG evt_qid;
	HULONG evt_tid;

	HINT32 semaphore;
	HINT32 device_count;
	NICO_NetDevice_Info_t *deviceinfo[NICO_MAX_DEVICE_COUNT];
} st_operator_context;

typedef st_operator_context* pst_operator_context;

typedef struct _nico_operator_object
{
	char obj_name[OBJ_NAME_LEN];
	st_operator_context obj_ctx;

	void  (*listener)(event_msg *msg);
} st_operator_object;
typedef st_operator_object* pst_operator_object;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC st_operator_object g_operator_object =
{
	.obj_ctx.state = OPERATOR_RUN,
};

/*******************************************************************/
/********************      Public Functions       ******************/
/*******************************************************************/
pst_operator_object operator_object = &g_operator_object;

/*******************************************************************/
/********************      Private Functions     ********************/
/*******************************************************************/
STATIC pst_operator_context __nico_operator_getcontext(void)
{
	return &operator_object->obj_ctx;
}

STATIC void __nico_wifi_listener(void *dimsg)
{
	pst_operator_context ctx = __nico_operator_getcontext();
	nico_dievtmsg_t evt;
	ENTRY;
	evt.type = EVT_WIFI;
	evt.len = sizeof(DI_WIFI_CALLBACK_DATA_t);
	evt.data = HLIB_STD_MemDup(dimsg, sizeof(DI_WIFI_CALLBACK_DATA_t));
	if( VK_OK != VK_MSG_Send ( ctx->evt_qid, (void*)&evt, sizeof(nico_dievtmsg_t) ))
	{
		HxLOG_Error("Fail to Send Event \n");
	}

	EXIT;
}

STATIC void __nico_netconf_listener(void *dimsg)
{
	pst_operator_context ctx = __nico_operator_getcontext();
	nico_dievtmsg_t evt;
	ENTRY;
	evt.type = EVT_NETCONF;
	evt.len = sizeof(DI_NETWORK_CALLBACK_DATA_t);
	evt.data = HLIB_STD_MemDup(dimsg, sizeof(DI_NETWORK_CALLBACK_DATA_t));
	if( VK_OK != VK_MSG_Send ( ctx->evt_qid, (void*)&evt, sizeof(nico_dievtmsg_t) ))
	{
		HxLOG_Error("Fail to Send Event \n");
	}

	EXIT;
}

STATIC void __nico_cm_listener(void *dimsg)
{
	pst_operator_context ctx = __nico_operator_getcontext();
	nico_dievtmsg_t evt;
	ENTRY;
	evt.type = EVT_CM;
	evt.len = sizeof(DI_CM_LinkInfo_e);
	evt.data = HLIB_STD_MemDup(dimsg, sizeof(DI_CM_LinkInfo_e));
	if( VK_OK != VK_MSG_Send ( ctx->evt_qid, (void*)&evt, sizeof(nico_dievtmsg_t) ))
	{
		HxLOG_Error("Fail to Send Event \n");
	}

	EXIT;
}
STATIC void __nico_bluetooth_listener(void *dimsg)
{
	pst_operator_context ctx = __nico_operator_getcontext();
	nico_dievtmsg_t evt;
	ENTRY;
	evt.type = EVT_BT;
	evt.len = sizeof(DI_BT_CALLBACK_DATA_t);
	evt.data = HLIB_STD_MemDup(dimsg, sizeof(DI_BT_CALLBACK_DATA_t));
	if( VK_OK != VK_MSG_Send ( ctx->evt_qid, (void*)&evt, sizeof(nico_dievtmsg_t) ))
	{
		HxLOG_Error("Fail to Send Event \n");
	}
	EXIT;
}

STATIC void __nico_pppoe_listener(void *dimsg)
{
	pst_operator_context ctx = __nico_operator_getcontext();
	nico_dievtmsg_t evt;
	ENTRY;
	evt.type = EVT_PPPOE;
	evt.len = sizeof(DI_PPPOE_CALLBACK_DATA_t);
	evt.data = HLIB_STD_MemDup(dimsg, sizeof(DI_PPPOE_CALLBACK_DATA_t));
	if( VK_OK != VK_MSG_Send ( ctx->evt_qid, (void*)&evt, sizeof(nico_dievtmsg_t) ))
	{
		HxLOG_Error("Fail to Send Event \n");
	}
	EXIT;
}

STATIC void __nico_operator_do_sendevent(nico_dievtmsg_t *di_event)
{
#if 1
	pst_operator_context ctx = __nico_operator_getcontext();

	pevent_msg event = (pevent_msg)HLIB_STD_MemCalloc(sizeof(event_msg));
	event->sync_call = FALSE,
	event->request_id = 0;

	nico_parser_encode(di_event, event);
	nico_debug_print_eventtype(event);

#if defined(CONFIG_USES_NFSROOT)

#else
	nico_op_common_set_routetable(event, ctx->device_count, ctx->deviceinfo, di_event);
#endif

	if(operator_object->listener)
	{
		operator_object->listener(event);
	}
	SAFE_FREE(event);
#else
	event_msg event = {0,};
	event.sync_call = FALSE;
	event.request_id = 0;
	nico_parser_encode(di_event, &event);
	if(operator_object->listener)
	{
		operator_object->listener(&event);
	}
#endif
	(void)ctx;
}

STATIC psyncreturn_msg __nico_operator_do_process(pcommand_msg cmd)
{
	psyncreturn_msg ret_msg = NULL;
	pst_operator_context ctx = __nico_operator_getcontext();

	if(0 <= cmd->device_id  && cmd->device_id < ctx->device_count)
	{
		//@@TT:KTH Save Device's current state informations into global context
		nico_op_common_update_devinfo(cmd->device_id, ctx->deviceinfo[cmd->device_id]);

	switch(cmd->type.cmd_type & 0xffff0000)
	{
		case CMD_NETCONF:

			ret_msg = nico_netconf_operate(cmd);
			break;
		case CMD_WIFI:
			ret_msg = nico_wifi_operate(cmd);
			break;
		case CMD_CM:
			ret_msg = nico_cm_operate(cmd);
			break;
		case CMD_BT:
			ret_msg = nico_bluetooth_operate(cmd);
			break;
		case CMD_PPPOE:
			ret_msg = nico_pppoe_operate(cmd);
			break;
		case CMD_UNKNOWN:
		default:
			break;
	}
	}
	return ret_msg;
}
STATIC void __nico_operator_event_task(void *args)
{
	pst_operator_context ctx = (pst_operator_context)args;
	nico_dievtmsg_t evt;

	while ( OPERATOR_RUN == ctx->state )
	{
		HxSTD_MemSet(&evt, 0x00, sizeof(nico_dievtmsg_t));
		if ( VK_OK == VK_MSG_ReceiveTimeout(ctx->evt_qid, (void*)&evt,
											sizeof(nico_dievtmsg_t), OP_MSGQ_TIMEOUT))
		{
			__nico_operator_do_sendevent(&evt);
			SAFE_FREE(evt.data);
		}
		else
		{
			VK_TASK_Sleep(10);
		}
	}
}

STATIC void __nico_operator_cmd_task(void *args)
{
	pst_operator_context ctx = (pst_operator_context)args;
	nico_cmdmsg_t cmd;
	psyncreturn_msg pRetMsg = NULL;

	while ( OPERATOR_RUN == ctx->state )
	{
		HxSTD_MemSet(&cmd, 0x00, sizeof(nico_cmdmsg_t));
		if ( VK_OK == VK_MSG_ReceiveTimeout(ctx->cmd_qid, (void*)&cmd,
											sizeof(nico_cmdmsg_t), OP_MSGQ_TIMEOUT))
		{
			pcommand_msg cmd_msg = cmd.data;
			ENTRY;
			pRetMsg = __nico_operator_do_process(cmd_msg);

			SAFE_FREE(cmd_msg);
			SAFE_FREE(pRetMsg);
			EXIT;
		}
		else
		{
			VK_TASK_Sleep(10);
		}
	}
}

STATIC HERROR __nico_operator_create_event_task(void)
{
	pst_operator_context ctx = __nico_operator_getcontext();
	ENTRY;
	if ( VK_OK != VK_MSG_Create(OP_MSGQ_SIZE, sizeof(nico_dievtmsg_t),
								"eventMsg",
								&ctx->evt_qid, VK_SUSPENDTYPE_FIFO))
	{
		return ERR_FAIL;
	}

	HxLOG_Debug("Made Msg Queue(0x%x) for event \n", ctx->evt_qid);
	if( VK_OK != VK_TASK_Create(__nico_operator_event_task,
								VK_TASK_PRIORITY_MW_REF, SIZE_64K,
								"eventManager",
								(void *)ctx, &ctx->evt_tid, VK_SUSPENDTYPE_FIFO))
	{
		return ERR_FAIL;
	}
	if(VK_OK != VK_TASK_Start(ctx->evt_tid))
	{
		return ERR_FAIL;
	}
	EXIT;
	return ERR_OK;
}

STATIC HERROR __nico_operator_create_cmd_task(void)
{
	pst_operator_context ctx = __nico_operator_getcontext();
	ENTRY;
	if ( VK_OK != VK_MSG_Create(OP_MSGQ_SIZE, sizeof(nico_cmdmsg_t),
								"operationMsg",
								&ctx->cmd_qid, VK_SUSPENDTYPE_FIFO))
	{
		return ERR_FAIL;
	}

	HxLOG_Debug("Made Msg Queue(0x%x) for command \n", ctx->cmd_qid);
	if ( VK_OK != VK_TASK_Create(__nico_operator_cmd_task,
								VK_TASK_PRIORITY_MW_REF, SIZE_64K,
								"operationManager",
								(void *)ctx, &ctx->cmd_tid, VK_SUSPENDTYPE_FIFO))
	{
		return ERR_FAIL;
	}
	if( VK_OK != VK_TASK_Start(ctx->cmd_tid))
	{
		return ERR_FAIL;
	}
	return ERR_OK;
}

STATIC HERROR __nico_operator_set_profile(HINT32 dev_count, NICO_NetDevice_Info_t **devinfo)
{
	HERROR eRet = ERR_OK;
	HINT32 i =0;
	HxXMLDOC hXmlDocHandle = NULL;
	HxXMLELEMENT hRoot = NULL, element_name = NULL, element_priority = NULL;
	HxList_t *child_device = NULL, *child_name = NULL, *child_priority = NULL, *pTemp = NULL;
	HUINT8 *contents_name = NULL, *contents_priority = NULL;

	if( NULL == devinfo )
	{
		HxLOG_Error("nico error : device is null \n");
		return ERR_FAIL;
	}

	eRet = HLIB_XML_InitParser();
	if( ERR_OK != eRet )
	{
		HxLOG_Error("nico error : xml parser init\n");
		return ERR_FAIL;
	}

	eRet = HLIB_XML_ParseFile(NICO_PRIORITY_FILE_PATH, &hXmlDocHandle);
	if( ERR_OK != eRet )
	{
		HxLOG_Error("nico error : have no file path\n");
		return ERR_FAIL;
	}

	eRet = HLIB_XML_GetRootElement(hXmlDocHandle, &hRoot);
	if( ERR_OK != eRet )
	{
		HxLOG_Error("nico error : have no root element \n");
		return ERR_FAIL;
	}

	eRet = HLIB_XML_FindChildElementByName(hRoot, "device", &child_device);
	if( (ERR_OK != eRet) || (NULL == child_device) )
	{
		HxLOG_Error("nico error : have no child element, device \n");
		if( NULL != child_device ) HLIB_LIST_RemoveAll(child_device);
		return ERR_FAIL;
	}

	pTemp = child_device;
	while( TRUE )
	{
		eRet = HLIB_XML_FindChildElementByName((HxXMLELEMENT)HLIB_LIST_Data(pTemp), "name", &child_name);
		if( (ERR_OK != eRet) || (NULL == child_name) )
		{
			if( NULL != child_name ) HLIB_LIST_RemoveAll(child_name);
			if( NULL != child_device ) HLIB_LIST_RemoveAll(child_device);
			if( NULL != child_priority ) HLIB_LIST_RemoveAll(child_priority);
			HxLOG_Error("nico error : have no child element, name \n");
			return ERR_FAIL;
		}

		element_name = (HxXMLELEMENT)HLIB_LIST_Data(child_name);
		eRet = HLIB_XML_GetElementContents(hXmlDocHandle,element_name, &contents_name);
		for( i=0; i<dev_count; i++ )
		{
			if( 0 == HxSTD_StrCmp(devinfo[i]->netinfo.name, contents_name) )
			{
				eRet = HLIB_XML_FindChildElementByName((HxXMLELEMENT)HLIB_LIST_Data(pTemp), "priority", &child_priority);
				if( (ERR_OK != eRet) || (NULL == child_priority) )
				{
					if( NULL != child_name ) HLIB_LIST_RemoveAll(child_name);
					if( NULL != child_device ) HLIB_LIST_RemoveAll(child_device);
					if( NULL != child_priority ) HLIB_LIST_RemoveAll(child_priority);
					HxLOG_Error("nico error : have no child element, name \n");
					return ERR_FAIL;
				}

				element_priority = (HxXMLELEMENT)HLIB_LIST_Data(child_priority);
				eRet = HLIB_XML_GetElementContents(hXmlDocHandle, element_priority, &contents_priority);
				devinfo[i]->netinfo.priority = atoi(contents_priority);
				HxLOG_Print("nico print : set device priority ( %s ) : %d \n", contents_name, atoi(contents_priority));
				HLIB_LIST_RemoveAll(child_priority);
			}
		}

		HLIB_LIST_RemoveAll(child_name);
		pTemp = HLIB_LIST_Next(pTemp);
		if( NULL == pTemp )
		{
			if( NULL != child_device ) HLIB_LIST_RemoveAll(child_device);
			HxLOG_Error("nico error : have no next element \n");
			return ERR_OK;
		}
	}

	return ERR_OK;
}

STATIC HERROR __nico_operator_init_netdevice(void)
{
	HERROR hRet = ERR_OK;
	HUINT32	id = 0;
	pst_operator_context ctx = __nico_operator_getcontext();
	ENTRY;
 	if(DI_ERR_ERROR == DI_NETWORK_GetDevCount(&ctx->device_count))
	{
		HxLOG_Error("Fail to get network device count \n");
		hRet = ERR_FAIL;
	}
	else
	{
		HxLOG_Debug("######################################################## \n");
		HxLOG_Debug("######################################################## \n");
		HxLOG_Debug("##########    __nico_operator_init_netdevice   ######### \n");
		HxLOG_Debug("######################################################## \n");
		HxLOG_Debug("######################################################## \n");

 		HxLOG_Debug("Network Device Count : %d \n", ctx->device_count);
		for(id = 0; id < ctx->device_count; id++)
		{
			ctx->deviceinfo[id] = (NICO_NetDevice_Info_t*)HLIB_STD_MemCalloc(sizeof(NICO_NetDevice_Info_t));
			nico_op_common_update_devinfo(id, ctx->deviceinfo[id]);
			{
				HCHAR szCmd[128] = {0,};
				HxSTD_snprintf(szCmd, 128, "ifconfig %s up", ctx->deviceinfo[id]->name);
				VK_SYSTEM_Command(szCmd);
			}
			//2 #### 초기 Device 의 Priority 는 Device ID 순서 대로 설정, 이후 필요 하면, Priority 를 설정하도록 API 지원
			ctx->deviceinfo[id]->netinfo.priority = ctx->deviceinfo[id]->netinfo.id;
		}

		__nico_operator_set_profile(ctx->device_count, ctx->deviceinfo);

#if defined(CONFIG_PROD_DEVICE_BLUETOOTH)
		ctx->deviceinfo[ctx->device_count] = (NICO_NetDevice_Info_t*)HLIB_STD_MemCalloc(sizeof(NICO_NetDevice_Info_t));
		nico_op_common_update_devinfo(ctx->device_count, ctx->deviceinfo[ctx->device_count]);
		ctx->deviceinfo[ctx->device_count]->netinfo.priority = ctx->deviceinfo[ctx->device_count]->netinfo.id;
#endif
	}
	EXIT;
	return hRet;
}

/*******************************************************************/
/********************      Public Functions       ******************/
/*******************************************************************/
HERROR nico_operator_init( void )
{
	HERROR hRet = ERR_OK;
	pst_operator_context ctx = __nico_operator_getcontext();

	ENTRY;
	if(ERR_OK == __nico_operator_create_cmd_task()
		&& ERR_OK == __nico_operator_create_event_task())
	{
		HxSEMT_Create(&ctx->semaphore, "nico_op_sem", 0);

		if(VK_OK != VK_SYSTEM_Command("ifconfig lo up"))
		{
			hRet = ERR_FAIL;
			HxLOG_Error("system command fail to run \n");
		}
		else
		{
			__nico_operator_init_netdevice();
			nico_netconf_register_callback(__nico_netconf_listener);
			nico_wifi_register_callback(__nico_wifi_listener);
			nico_cm_register_callback(__nico_cm_listener);
			nico_bluetooth_register_callback(__nico_bluetooth_listener);
			nico_pppoe_register_callback(__nico_pppoe_listener);

			nico_parser_init(ctx->device_count, ctx->deviceinfo);
			nico_netconf_init(ctx->device_count, ctx->deviceinfo);
			nico_wifi_init(ctx->device_count, ctx->deviceinfo);
			nico_cm_init(ctx->device_count, ctx->deviceinfo);
			nico_bluetooth_init(ctx->device_count, ctx->deviceinfo);
			nico_pppoe_init(ctx->device_count, ctx->deviceinfo);
			{
				char szcmd[256] = {0,};
				HxSTD_snprintf(szcmd, 256, "touch %s/resolv.conf", CONFIG_PRODUCT_DATA_DIR);
				VK_SYSTEM_Command(szcmd);
			}
		}
	}
	else
	{
		hRet = ERR_FAIL;
		HxLOG_Debug("Fail to init Operator\n");
	}

	EXIT;
	return hRet;
}

psyncreturn_msg nico_operator_operate( command_msg *params )
{
	nico_cmdmsg_t msg;
	pst_operator_context ctx = __nico_operator_getcontext();

	ENTRY;
	nico_parser_decode(params);

	CHECK_DEVICE(params);

	if(FALSE == params->sync_call)
	{
		msg.data = HLIB_STD_MemDup(params, sizeof(command_msg));
		if( VK_OK != VK_MSG_Send ( ctx->cmd_qid, &msg, sizeof(nico_cmdmsg_t) ))
		{
			HxLOG_Error("Fail to Send Operation \n");
		}
		else
		{
			HxLOG_Debug("Success to Send Operation [cmd:0x%x]\n", params->type.cmd_type);
		}
	}
	else
	{
		return __nico_operator_do_process(params);
	}
	EXIT;
	return NULL;
}

HERROR nico_operator_stop(void)
{
	HERROR hRet = ERR_OK;
	pst_operator_context ctx = __nico_operator_getcontext();
	ENTRY;
	ctx->state = OPERATOR_STOP;
	EXIT;
	return hRet;
}
HERROR nico_operator_deinit( void )
{
	HERROR hRet = ERR_OK;
	pst_operator_context ctx = __nico_operator_getcontext();
	HxSEMT_Destroy(ctx->semaphore);

	ENTRY;

#if 0 // cus of DI Bugs
	if(VK_OK != VK_TASK_Stop(ctx->cmd_tid) ||
		VK_OK != VK_TASK_Destroy(ctx->cmd_tid) ||
		VK_OK != VK_TASK_Stop(ctx->evt_tid) ||
		VK_OK != VK_TASK_Destroy(ctx->evt_tid) ||
		VK_OK != VK_MSG_Destroy(ctx->cmd_qid) ||
		VK_OK != VK_MSG_Destroy(ctx->evt_qid))

	{
		hRet = ERR_FAIL;
	}
#endif
	nico_parser_deinit();
	nico_wifi_deinit();
	nico_netconf_deinit();
	nico_cm_deinit();
	nico_bluetooth_deinit();
	nico_pppoe_deinit();

	if( operator_object ) operator_object->listener = NULL;

	EXIT;
	return hRet;
}

HERROR nico_operator_register_callback( void *callback )
{
	ENTRY;

	if( operator_object )
		operator_object->listener = callback;

	EXIT;
	return ERR_OK;
}

