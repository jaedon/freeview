/**************************************************************
*	@file	cmd_usercp.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			DLNA / RC
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
#include "pl_usercp.h"
#include "addressmanager_c.h"

#define MAX_ITEM_COUNT	256
#define ADDRESS_XML_PATH	"/var/lib/humaxtv_user/allowedmaclist.xml"

#define ________________Internal_Functions_________________________
#if defined(CONFIG_DEBUG)
HINT32   cmd_usercp_set_action(void *szArgList)
{
	HCHAR    *aucArg=NULL, aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if (VK_strcmp(aucStr, "ADDMAC") == 0)
	{
		HINT32 i = 0;
		HCHAR tmpMac[256] = {0,};

		HLIB_CMD_Printf("MAC Address add!! (00:01:02:03:12:00 ~ 00:01:02:03:12:ff)\n");

		HLIB_FILE_Delete(ADDRESS_XML_PATH);

		for(i = 0; i<MAX_ITEM_COUNT; i++)
		{
			HxSTD_snprintf(tmpMac, sizeof(tmpMac), "00:01:02:03:12:%02x", i);
			ADDRMGR_AddAddress(tmpMac);
			ADDRMGR_SaveAsFile();
			HxSTD_MemSet(tmpMac, 0x0, sizeof(tmpMac));
		}
	}
	else if(VK_strcmp(aucStr, "RECOVERY") == 0)
	{
		HLIB_CMD_Printf("Recovery~~\n");
		PL_USERCP_Recovery();
	}
	else
	{

	}

	return HxCMD_OK;
}

HINT32   cmd_usercp_get_action(void *szArgList)
{
	HCHAR    *aucArg=NULL, aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if(VK_strcmp(aucStr, "MACLIST") == 0)
	{
		// TODO:
	}
	else
	{

	}

	return HxCMD_OK;
}
#endif

#define ________________Public_Interfaces__________________________

void	HOMMA_CMD_Register_USERCP(void)
{
#if defined (CONFIG_DEBUG)
	HLIB_CMD_RegisterWord( "usercp", cmd_usercp_get_action, "get", "maclist", "usercp get action");
	HLIB_CMD_RegisterWord( "usercp", cmd_usercp_set_action, "set", "addmac | recovery", "usercp set action");
#endif
}
#define _____END_OF_FILE_________
