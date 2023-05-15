/*************************************************************************************************************
	File 		: nico_communicator.c
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
#include "nico_communicator.h"
#include "nico_server.h"

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
	COMMUNICATOR_RUNNING,
	COMMUNICATOR_STOPPED,
} e_communicator_state;

typedef struct _nico_communicator_context
{
	e_communicator_state state;
} st_communicator_context;

typedef st_communicator_context* pst_communicator_context;

typedef struct _nico_communicator_object
{
	st_communicator_context obj_ctx;

	HERROR (*secoff_send)(event_msg *msg);
	psyncreturn_msg  (*secoff_do_command)(command_msg *msg);
	psyncreturn_msg  (*listener)(command_msg *msg);
} st_communicator_object;
typedef st_communicator_object* pst_communicator_object;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Object          ******************/
/*******************************************************************/
st_communicator_object g_communicator_object;
pst_communicator_object communicator_object = &g_communicator_object;

/*******************************************************************/
/********************      Private Functions     ********************/
/*******************************************************************/
STATIC pst_communicator_context __nico_communicator_getcontext(void)
{
	return &communicator_object->obj_ctx;
}

STATIC psyncreturn_msg __nico_communicator_server_listener(command_msg *msg)
{
	//1 #### case by case
	ENTRY;
	if(communicator_object->listener)
	{
		return communicator_object->listener(msg);
	}
	EXIT;
	return NULL;
}

/*******************************************************************/
/********************      Public Functions       ******************/
/*******************************************************************/
HERROR nico_communicator_init( void )
{
	HERROR hRet = ERR_OK;

	ENTRY;
	nico_server_register_callback(__nico_communicator_server_listener);
#ifdef CONFIG_ENHANCED_SECURITY
	hRet = nico_server_init(NICO_IPC_UDN);
#endif
	(void)__nico_communicator_getcontext();
	EXIT;
	return hRet;
}

HERROR nico_communicator_start( void )
{
	HERROR hRet = ERR_OK;
	ENTRY;
#ifdef CONFIG_ENHANCED_SECURITY
	hRet = nico_server_start();
#endif
	EXIT;
	return hRet;
}

void nico_communicator_register_callback(secoff_send cb)
{
	communicator_object->secoff_send = cb;
}

psyncreturn_msg nico_communicator_do_command(command_msg *msg)
{
	ENTRY;
	if(communicator_object->listener)
	{
		return communicator_object->listener(msg);
	}
	EXIT;
	return NULL;
}

HERROR nico_communicator_sendevent(event_msg *params)
{
	HERROR hRet = ERR_OK;
	ENTRY;
#ifdef CONFIG_ENHANCED_SECURITY
	hRet = nico_server_sendevent(params);
#else
	while (communicator_object->secoff_send == NULL)
	{
		HxLOG_Debug("Waiting for initializing object->secoff_send variable.\n");
		VK_TASK_Sleep(10);
	}
	hRet = communicator_object->secoff_send(params);
#endif
	EXIT;
	return hRet;
}

HERROR nico_communicator_stop( void )
{
	HERROR hRet = ERR_OK;
	ENTRY;
	hRet = nico_server_stop();
	EXIT;
	return hRet;
}

HERROR nico_communicator_deinit( void )
{
	HERROR hRet = ERR_OK;
	ENTRY;
	hRet = nico_server_deinit();

	if( communicator_object ) communicator_object->listener = NULL;

	EXIT;
	return hRet;
}

HERROR nico_communicator_registercallback( void *callback )
{
	ENTRY;

	if( communicator_object )
		communicator_object->listener = callback;

	EXIT;
	return ERR_OK;
}

