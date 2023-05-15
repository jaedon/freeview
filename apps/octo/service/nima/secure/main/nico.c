/*************************************************************************************************************
	File 		: nico.h
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

#include "nico.h"
#include "nico_manager.h"
//#define NORMAL_EXIT

pst_manager_context manager_ctx = NULL;
void nico_init(void)
{
	HLIB_DBG_Init();

	if(ERR_OK == nico_manager_init())
	{
		signal(SIGPIPE, SIG_IGN);
		HxLOG_Info("######################################################\n");
		HxLOG_Info("It's me, NICO !!!!!!!!!! \n");
		HxLOG_Info("######################################################\n\n");

		HxLOG_Warning("~~~~~~~~~~~~~~ START NICO v1.0 ~~~~~~~~~~~~~~~~ \n");
		HxLOG_Warning(" (c) 2012-2013 Network Interface COre  \n");
		HxLOG_Warning("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n\n");

		HxLOG_Debug("++ Check Message Size for IPC +++++ \n");
		HxLOG_Debug("sizeof Cmd_Wifi_t		: %8d \n", sizeof(Cmd_Wifi_t));
		HxLOG_Debug("sizeof Evt_Wifi_t		: %8d \n", sizeof(Evt_Wifi_t));
		HxLOG_Debug("sizeof Cmd_Cm_t		: %8d \n", sizeof(Cmd_Cm_t));
		HxLOG_Debug("sizeof Evt_Cm_t		: %8d \n", sizeof(Evt_Cm_t));
		HxLOG_Debug("sizeof Cmd_NetConf_t	: %8d \n", sizeof(Cmd_NetConf_t));
		HxLOG_Debug("sizeof Evt_NetConf_t	: %8d \n", sizeof(Evt_NetConf_t));
		HxLOG_Debug("sizeof Cmd_Bluetooth_t : %8d \n", sizeof(Cmd_Bluetooth_t));
		HxLOG_Debug("sizeof Evt_Bluetooth_t : %8d \n", sizeof(Evt_Bluetooth_t));
		HxLOG_Debug("sizeof Cmd_PPPoE_t 	: %8d \n", sizeof(Cmd_PPPoE_t));
		HxLOG_Debug("sizeof Evt_PPPoE_t 	: %8d \n", sizeof(Evt_PPPoE_t));
		HxLOG_Debug("+++++++++++++++++++++++++++++++++++ \n");

		//HLIB_STD_TaskSleep(2*1000);
		manager_ctx = nico_manager_start();
		if(MANAGER_RUNNING == manager_ctx->state)
		{

		}
		else
		{
			HxLOG_Error("Nico Couldn't initialized, and so exit \n");
		}
#ifdef NORMAL_EXIT
		nico_manager_deinit();
#endif
	}
}

#ifdef CONFIG_ENHANCED_SECURITY
int main(int argc, char *argv[])
{
	/* Assume below nico_init must be success */
	nico_init();

	HAPI_Init();

	HAPI_SendReadySignal(HLIB_STD_GetPID());

	HxLOG_Info("######################################################\n");
	HxLOG_Info("NICO TURN ON SUCCESSFULLY \n");
	HxLOG_Info("######################################################\n\n");

	while (MANAGER_RUNNING == manager_ctx->state)
	{
		HLIB_STD_TaskSleep(3000);
	}

	return 0;
}
#endif

