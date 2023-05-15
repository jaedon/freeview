/** **********************************************************************************************************
	File 		: nima.c
	author 		: STB Component tstbcomp@humaxdigital.com
	comment		:
	date    	: 2013/05/15
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

#include <vkernel.h>
#include <hlib.h>
#include <unistd.h>
#include <hapi.h>

#include "nima_server.h"
#if defined(CONFIG_HUMAX_CRASHREPORT)
#include "hcrlib.h"
#endif
#ifndef CONFIG_ENHANCED_SECURITY
#include "nico.h"
#include "papi.h"
#endif
//
/*-----------------------------------------------------------------------------------------------
 *	pre-definition
 *----------------------------------------------------------------------------------------------*/
#define NIMA_SERVER_UDS_PATH		"nima"

/*-----------------------------------------------------------------------------------------------
 *	internal functions
 *----------------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------------------------
 *	main
 *----------------------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	HERROR eRet = ERR_OK;
	nimaserver_t *server = NULL;

	VK_Init();
	HAPI_Init();
	HLIB_DBG_Init();
	HxLOG_Debug("######################################################\n");
	HxLOG_Debug("######################################################\n");
	HxLOG_Debug("######################################################\n");
	HxLOG_Debug("######################################################\n");
	HxLOG_Debug("######################################################\n");
	HxLOG_Debug("It's me, NIMA 18:27 !!!!!!!!!! \n");
	HxLOG_Debug("######################################################\n");
	HxLOG_Debug("######################################################\n");
	HxLOG_Debug("######################################################\n");
	HxLOG_Debug("######################################################\n");
	HxLOG_Debug("######################################################\n\n");
#if defined(CONFIG_HUMAX_CRASHREPORT)
	HCR_Init(NULL);
#endif
#ifndef CONFIG_ENHANCED_SECURITY
	nico_init();
#endif

	server = nima_server_create(MIMA_SERV_MAGIC_NO);
	if ( !server )
	{
		HxLOG_Critical("NIMA::Fatal: Cannot create NIMA Server object.\n");
		return -1;
	}

	eRet = nima_server_start(server, NIMA_SERVER_UDS_PATH);
	if ( eRet != ERR_OK )
	{
		nima_server_destroy(server);
		HxLOG_Critical("NIMA::Fatal: Cannot start NIMA Server.\n");
		return -1;
	}

	HAPI_SendReadySignal(HLIB_STD_GetPID());

	HLIB_EXTRA_DropRootPrivileges("nima");
	HxLOG_Critical("NIMA::MAIN: NIMA server started successfully.\n");

	for ( ;; )
	{
		usleep(1000000);
	}

	return 0;
}
