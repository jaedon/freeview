/*************************************************************************************************************
	File 		: nico_manager.c
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
#include "nico_manager.h"
#include "nico_communicator.h"
#include "nico_operator.h"

/*******************************************************************/
/*******************      template         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct 
{
	st_manager_context obj_ctx;
	void  (*listener)(pevent_msg msg);
} st_manager_object;
typedef st_manager_object* pst_manager_object;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
STATIC st_manager_object g_manager_object =
{
	.obj_ctx.state = MANAGER_STOPPED,
};

/*******************************************************************/
/********************      Public Object          ******************/
/*******************************************************************/

pst_manager_object manager_object = &g_manager_object;

/*******************************************************************/
/********************      Private Functions     ********************/
/*******************************************************************/
STATIC pst_manager_context __nico_manager_getcontext(void)
{
	return &manager_object->obj_ctx;
}

STATIC psyncreturn_msg __nico_manager_communicator_listener(command_msg *msg)
{
	ENTRY;
	return nico_operator_operate(msg);
}

STATIC void 	__nico_manager_listener(pevent_msg msg)
{
	ENTRY;
	switch(msg->type.evt_type)
	{
		case EVENT_NET_CONNECT_SUCCESS:
		case EVENT_NET_PPPOE_CONNECT_SUCCESS:
			//HAPI_BroadcastSignal("signal:onNetworkConnected", 0, "i", 1);
			break;
		case EVENT_NET_WIFI_AP_CONNECT_FAIL:
		case EVENT_NET_WIFI_USB_EXTRACTED:
		case EVENT_NET_STOP_CONNECTING:
		case EVENT_NET_PPPOE_DISCONNECT_SUCCESS:
			//HAPI_BroadcastSignal("signal:onNetworkConnected", 0, "i", 0);
			break;
		default:
			break;
	}
	EXIT;
}

STATIC void __nico_manager_operator_listener(event_msg *msg)
{
	ENTRY;
	//1 ### report to main(or hama)
	__nico_manager_listener(msg);
	//2 ### 2. send result
	if(ERR_OK != nico_communicator_sendevent(msg))
	{
		HxLOG_Error("Fail to send event \n");
		HxLOG_Error("eventtype : 0x%x     \n", msg->type.evt_type);
	}

	EXIT;
}

/*******************************************************************/
/********************      Public Functions       ******************/
/*******************************************************************/
HERROR nico_manager_init( void )
{
	HERROR hRet = ERR_OK;

	ENTRY;
	if( VK_OK == VK_Init() &&
		DI_ERR_OK == DI_NETWORK_Init()
#if defined(CONFIG_MW_INET_WLAN)
		&& DRV_OK == DRV_HOTPLUG_Init()
		&& DRV_OK == DRV_WIFI_Init()
#endif
#if defined(CONFIG_PROD_DEVICE_BLUETOOTH)
		&& DI_ERR_OK == DI_BT_Init()
#endif
#if defined(CONFIG_MW_INET_PPPOE)
		&& DRV_OK == DRV_PPPOE_Init()
#endif
		)
	{
		nico_communicator_registercallback(__nico_manager_communicator_listener);
		nico_operator_register_callback(__nico_manager_operator_listener);

		if(ERR_OK == nico_communicator_init() &&
			ERR_OK == nico_operator_init())
		{
			hRet = ERR_OK;
		}
		else
		{
			hRet = ERR_FAIL;
		}
	}
	EXIT;
	return hRet;
}

pst_manager_context nico_manager_start( void )
{
	pst_manager_context ctx = __nico_manager_getcontext();
	ENTRY;
	if(ERR_OK == nico_communicator_start())
	{
		ctx->state = MANAGER_RUNNING;
	}
	else
	{
		ctx->state = MANAGER_STOPPED;
	}
	EXIT;
	return ctx;
}

HERROR nico_manager_stop( void )
{
	HERROR hRet = ERR_OK;
	pst_manager_context ctx = __nico_manager_getcontext();
	ENTRY;
	if(ERR_OK == nico_communicator_stop() &&
		ERR_OK == nico_operator_stop())
	{
		ctx->state = MANAGER_STOPPED;
	}
	else
	{
		hRet = ERR_FAIL;
	}
	EXIT;
	return hRet;
}

HERROR nico_manager_deinit( void )
{
	HERROR hRet = ERR_OK;
	ENTRY;
	nico_communicator_deinit();
	nico_operator_deinit();

	manager_object->listener = NULL;
	EXIT;
	return hRet;
}

