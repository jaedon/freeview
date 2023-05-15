/*************************************************************************************************************
	File 		: nico_operator_cm.c
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2013/05/16
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
#include "nico_operator_cm.h"
#include "di_err.h"
#include "di_cm.h"

/*******************************************************************/
/*******************      template         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	CM_RUNNING,
	CM_STOPPED,
} e_cm_state;

typedef struct _nico_cm_context
{
	e_cm_state state;
	HINT32 device_count;
	NICO_NetDevice_Info_t **deviceinfo;
} st_cm_context;

typedef st_cm_context* pst_cm_context;

typedef struct _nico_cm_object
{
	st_cm_context obj_ctx;

	void  (*listener)(void *dimsg);
} st_cm_object;
typedef st_cm_object* pst_cm_object;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC st_cm_object g_cm_object;

/*******************************************************************/
/********************      Public 			  ******************/
/*******************************************************************/
pst_cm_object cm_object = &g_cm_object;

/*******************************************************************/
/********************      Private Functions     ********************/
/*******************************************************************/
STATIC pst_cm_context __nico_cm_getcontext(void)
{
	return &cm_object->obj_ctx;
}
STATIC void __nico_cm_di_callback(DI_CM_LinkInfo_e eLinkState)
{
	//2 #### Callup with CM info
	if(cm_object->listener)
	{
		cm_object->listener(&eLinkState);
	}
}
STATIC HERROR __nico_cm_set_freq_downstream(NICO_CM_DownStream_t *downstreamInfo)
{
	HERROR hRet = ERR_OK;
#if defined(CONFIG_MW_INET_CM)
	DI_ERR_CODE errcode;
	if ( DI_ERR_OK !=  (errcode=DI_CM_SetScanDSFreq( (HUINT32 *)&downstreamInfo->freq )))
	{
		HxLOG_Error("DI_CM_SetScanDSFreq(). ERROR(%d)\n", errcode);
		hRet = ERR_FAIL;
	}
#endif
	return hRet;
}

psyncreturn_msg __nico_cm_operate_sync(command_msg *params)
{
	psyncreturn_msg retmsg = HLIB_STD_MemCalloc(sizeof(syncreturn_msg));

	if(NULL == retmsg)
	{
		HxLOG_Error("Out of Memory +++ \n");
	}
	else
	{
		Cmd_Cm_t *pcommand = (Cmd_Cm_t*)&params->cmd_protocol.cmd_cm;
		HxSTD_MemCopy(retmsg, params, sizeof(syncreturn_msg));
		retmsg->type.evt_type = params->type.cmd_type;

		switch(params->type.cmd_type)
		{
			default:
				(void*)(pcommand);
				break;
		}
	}
	return retmsg;
}
void __nico_cm_operate_async(command_msg *params)
{
	Cmd_Cm_t *pcommand = (Cmd_Cm_t*)&params->cmd_protocol.cmd_cm;

	switch(params->type.cmd_type)
	{
		case CMD_SET_CM_DOWNSTREAM_INFO:
			{
				Cmd_Cm_SetDownStreamInfo_t *cmdinfo = &pcommand->cmd.cmd_set_downstreaminfo;
				__nico_cm_set_freq_downstream(&cmdinfo->downstreamInfo);
			}
			break;
		default:
			break;
	}
}

/*******************************************************************/
/********************      Public Functions       ******************/
/*******************************************************************/
#if defined(CONFIG_MW_INET_CM)
HERROR nico_cm_init( HINT32 dev_count, NICO_NetDevice_Info_t *devices[] )
{
	HERROR hRet = ERR_OK;
	pst_cm_context ctx = __nico_cm_getcontext();
	ENTRY;

	if ( DI_ERR_OK != DI_CM_RegisterCallback(__nico_cm_di_callback) )
	{
		HxLOG_Error("DI_CM_RegisterCallback(). ERROR(%d)\n", hRet);
		hRet = ERR_FAIL;
	}
	ctx->device_count = dev_count;
	ctx->deviceinfo = devices;
	EXIT;
	return hRet;
}

psyncreturn_msg nico_cm_operate( command_msg *params )
{
	HERROR hRet = ERR_OK;
	psyncreturn_msg retmsg = NULL;

	ENTRY;
	if(TRUE == params->sync_call)
	{
		retmsg = __nico_cm_operate_sync(params);
	}
	else
	{
		__nico_cm_operate_async(params);
	}
	EXIT;
	return retmsg;
}

HERROR nico_cm_deinit( void )
{
	HERROR hRet = ERR_OK;
	ENTRY;

	if( cm_object ) cm_object->listener = NULL;

	EXIT;
	return hRet;
}

HERROR nico_cm_register_callback( void *callback )
{	
	ENTRY;

	if( cm_object )
		cm_object->listener = callback;
	
	EXIT;
	return ERR_OK;
}

#else
HERROR nico_cm_init( HINT32 dev_count, NICO_NetDevice_Info_t *devices[] )
{
	(void)__nico_cm_getcontext;
	(void)__nico_cm_di_callback;
	(void)__nico_cm_set_freq_downstream;
	return ERR_FAIL;
}
psyncreturn_msg nico_cm_operate( command_msg *params ){return NULL;}
HERROR nico_cm_deinit( void ){return ERR_FAIL;}
HERROR nico_cm_register_callback( void *callback ){return ERR_FAIL;}

#endif

