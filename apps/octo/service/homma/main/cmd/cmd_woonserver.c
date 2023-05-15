/**************************************************************
*	@file	cmd_woonserver.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			DLNA / WOON
**************************************************************/

/*
* ¨Ï 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

#define	________________Header_Files_______________________________
#include <vkernel.h>
#include <hlib.h>

#include "hom_int.h"

#if defined(CONFIG_HOMMA_WOON)
#include "pl_woons.h"
#include "woons.h"
#endif

#define ________________Internal_Functions_________________________
#if defined(CONFIG_DEBUG)
HINT32   cmd_woon_set_action(void *szArgList)
{
	HCHAR    *aucArg=NULL, aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if (VK_strcmp(aucStr, "START") == 0)
	{
		HLIB_CMD_Printf("\n** woon server start **\n");
		//HERROR 	WOONS_Start(const char *szFriendlyName, const char *szMac);
	}
	else if(VK_strcmp(aucStr, "STOP") == 0)
	{
		HLIB_CMD_Printf("\n** woon server stop **\n");
		WOONS_Stop();
	}
	else if(VK_strcmp(aucStr, "TEST") == 0)
	{
		HLIB_CMD_Printf("\n** woon server test connection **\n");
		WOONS_TestConnection();
	}
	else if(VK_strcmp(aucStr, "RENAME") == 0)
	{
		HCHAR arg[20] = {0,};
		HCHAR *szArg1 = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if(szArg1) 	sscanf(szArg1, "%s", (char *)arg);

		HLIB_CMD_Printf("\n** woon server rename - %s **\n", arg);
		WOONS_Rename(arg);
	}
	else if(VK_strcmp(aucStr, "ADDID") == 0)
	{
		HCHAR arg[20] = {0,};
		HCHAR *szArg1 = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if(szArg1) 	sscanf(szArg1, "%s", (char *)arg);

		HLIB_CMD_Printf("\n** woon server add id **\n");
		WOONS_AddWoonId(arg);
	}
	else if(VK_strcmp(aucStr, "DELID") == 0)
	{
		HCHAR arg[20] = {0,};
		HCHAR *szArg1 = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if(szArg1) 	sscanf(szArg1, "%s", (char *)arg);
		HLIB_CMD_Printf("\n** woon server del id **\n");
		WOONS_DelWoonId(arg);
	}
	else
	{

	}


	return HxCMD_OK;
}

HINT32   cmd_woon_get_action(void *szArgList)
{
	HCHAR    *aucArg=NULL, aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if(VK_strcmp(aucStr, "IP") == 0)
	{
		HLIB_CMD_Printf("\n** woon server ip info **\n");
		HLIB_CMD_Printf("# ID - %s \n", WOONS_GetWoonId());
		HLIB_CMD_Printf("# LOCAL IP - %s \n", WOONS_GetLocalIp());
		HLIB_CMD_Printf("# EXT IP - %s / %d\n", WOONS_GetExternalIp(), WOONS_GetExternalPort());
		HLIB_CMD_Printf("# PUB IP - %s / %d \n", WOONS_GetPublicIp(), WOONS_GetPublicPort());
		WOONS_ShowNetworkInfo();
	}
	else if(VK_strcmp(aucStr, "DEVICE") == 0)
	{
		HLIB_CMD_Printf("\n** woon server device list **\n");
		WOONS_GetDeviceList();
	}
	else if(VK_strcmp(aucStr, "ID") == 0)
	{
		HLIB_CMD_Printf("\n** woon server id **\n");
		WOONS_ShowWidList();
	}
	else if(VK_strcmp(aucStr, "NETWORK") == 0)
	{
		HLIB_CMD_Printf("\n** woon server network status **\n");
		PL_WOONS_ShowNetworkInfo();
	}

	else if(VK_strcmp(aucStr, "PAIRINGID") == 0)
	{
		HCHAR *id = PL_WOONS_GetWoonId();
		HLIB_CMD_Printf("\n** woon server pairing id **\n");
		if(id != NULL)
			HLIB_CMD_Printf("\n** ID **", id);
	}

	else if(VK_strcmp(aucStr, "SERVER") == 0)
	{
#if 0
		WOON_SERVER_INFO_t pInfo;
		WOON_GetServerInfo(&pInfo);

		HLIB_CMD_Printf("\n** woon server info **\n");

		HLIB_CMD_Printf("# UDN - %s \n", pInfo.szUDN);
		HLIB_CMD_Printf("# ID - %s \n", pInfo.szWid);
		HLIB_CMD_Printf("# NAME - %s \n", pInfo.szServerName);
#endif
	}
	else
	{

	}

	return HxCMD_OK;
}
#endif

#define ________________Public_Interfaces__________________________

void	HOMMA_CMD_Register_WOONS(void)
{
#if defined (CONFIG_DEBUG)
	HLIB_CMD_RegisterWord( "woonserver", cmd_woon_get_action, "get", "ip | id | device | server | ", "woon server get action");
	HLIB_CMD_RegisterWord( "woonserver", cmd_woon_set_action, "set", "start | stop | test | rename | addid| delid ", "woon server set action");
#endif
}
#define _____END_OF_FILE_________
