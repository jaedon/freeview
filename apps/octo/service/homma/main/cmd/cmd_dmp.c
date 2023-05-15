/**************************************************************
*	@file	cmd_dmp.c
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
#include "pl_dmp.h"

#include "dmp.h"
#if defined(CONFIG_HOMMA_DMR)
#include "dmr.h"
#endif
#if defined(CONFIG_HOMMA_DMS)
#include "dms.h"
#endif
#if defined(CONFIG_HOMMA_DIAL)
#include "dial.h"
#endif
#if defined(CONFIG_HOMMA_WOON)
#include "woons.h"
#endif
#if defined(CONFIG_HOMMA_SATIP)
#include "satip.h"
#endif

#define ________________Static_Variables___________________________
#if defined(CONFIG_DEBUG)
static char udn[512];
static unsigned int requestID = 1;
#endif

#define ________________Internal_Functions_________________________
#if defined(CONFIG_DEBUG)
HINT32   cmd_dmp_set_action(void *szArgList)
{
	HCHAR    *aucArg=NULL, aucStr[20], aucArg2[256];
	HCHAR    cid[256], keyword[256];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if (VK_strcmp(aucStr, "START") == 0)
	{
		HLIB_CMD_Printf("dmp start\n");
		DMP_Start();
	}
	else if(VK_strcmp(aucStr, "STOP") == 0)
	{
		HLIB_CMD_Printf("dmp stop\n");
		DMP_Stop();
	}
	else if(VK_strcmp(aucStr, "SETDMS") == 0)
	{
		aucArg = HLIB_CMD_GetNextArg( (HCHAR**)&szArgList );
		if( aucArg == NULL )
			return HxCMD_ERR;

		if( sscanf(aucArg, "%s", aucArg2) == 0 )
			return HxCMD_ERR;

		memset(udn, 0x00, 512);
		HxSTD_snprintf(udn, sizeof(udn), "%s", aucArg2);
		HLIB_CMD_Printf("udn = %s\n", udn);
	}
	else if(VK_strcmp(aucStr, "BROWSE") == 0)
	{
		aucArg = HLIB_CMD_GetNextArg( (HCHAR**)&szArgList );
		if( aucArg == NULL )
			return HxCMD_ERR;

		if( sscanf(aucArg, "%s", aucArg2) == 0 )
			return HxCMD_ERR;

		memset(cid, 0x00, 256);
		HxSTD_snprintf(cid, 256, "%s", aucArg2);

		PL_DMP_Browse(1, udn, aucArg2, Dmp_BrowseFlag_Children, 0, 0, 10);

		HLIB_CMD_Printf("cid = %s\n", cid);
	}
	else if(VK_strcmp(aucStr, "SEARCHITEM") == 0)
	{
		aucArg = HLIB_CMD_GetNextArg( (HCHAR**)&szArgList );
		if( aucArg == NULL )
			return HxCMD_ERR;

		if( sscanf(aucArg, "%s", aucArg2) == 0 )
			return HxCMD_ERR;

		memset(keyword, 0x00, 256);
		HxSTD_snprintf(keyword, 256, "%s", aucArg2);
		requestID++;
		PL_DMP_Search( Dmp_SearchMode_Title, udn, 0, aucArg2, /* keyword */
					0, /* nSortOrder, */
					0, /* nStartIdx, */
					100, /* nRequestedNum, */
					requestID);
		HLIB_CMD_Printf("SearchItem(udn = %s, keyword = %s)\n", udn, keyword);
	}
	else if(VK_strcmp(aucStr, "SEARCHALL") == 0)
	{
		aucArg = HLIB_CMD_GetNextArg( (HCHAR**)&szArgList );
		if( aucArg == NULL )
			return HxCMD_ERR;

		if( sscanf(aucArg, "%s", aucArg2) == 0 )
			return HxCMD_ERR;

		memset(keyword, 0x00, 256);
		HxSTD_snprintf(keyword, 256, "%s", aucArg2);
		requestID++;
		PL_DMP_SearchAll( Dmp_SearchMode_Title, 0, aucArg2, /* keyword */
					0, /* nSortOrder, */
					requestID);
		HLIB_CMD_Printf("SearchItem(udn = %s, keyword = %s)\n", udn, keyword);
	}
	else if(VK_strcmp(aucStr, "MSEARCH") == 0)
	{
		PL_DMP_M_Search();
		HLIB_CMD_Printf("** m-search\n ");
	}
	else
	{

	}

	return HxCMD_OK;
}

HINT32   cmd_dmp_get_action(void *szArgList)
{
	HCHAR    *aucArg=NULL, aucStr[20]; //aucArg2[256], cid[256];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if(VK_strcmp(aucStr, "DMSLIST") == 0)
	{
		HINT32 cnt = 0, i = 0;
		PL_DMP_DmsInfo_t *pList = NULL;

		DMP_GetDmsList(&cnt, &pList);
		HLIB_CMD_Printf("\n *** Searched DMS (%d) ** \n", cnt);
		for(i=0; i<cnt; i++)
			HLIB_CMD_Printf("[%d/%d] IP(%s) UDN(%s) NAME(%s) \n", i, cnt, pList[i].szIp, pList[i].szUDN, pList[i].szFriendlyName);
	}
	else
	{

	}

	return HxCMD_OK;
}

HINT32   cmd_homma_status_action(void *szArgList)
{
	HCHAR    *aucArg=NULL, aucStr[20]; //aucArg2[256], cid[256];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if(VK_strcmp(aucStr, "STATUS") == 0)
	{
		HLIB_CMD_Printf("\n\n*** HOMMA Service Running Status *** n\n");
		HLIB_CMD_Printf("[DMP]         %s \n", DMP_IsStart()?"Running":"Not running");
#if defined(CONFIG_HOMMA_DMR)
		HLIB_CMD_Printf("[DMR]         %s \n", DMR_IsStart()?"Running":"Not running");
#endif

#if defined(CONFIG_HOMMA_DMS)
		HLIB_CMD_Printf("[DMS]         %s \n", DMS_IsStart()?"Running":"Not running");
#endif

#if defined(CONFIG_HOMMA_DIAL)
		HLIB_CMD_Printf("[DIAL]        %s \n", DIAL_IsStart()?"Running":"Not running");
#endif

#if defined(CONFIG_HOMMA_WOON)
		HLIB_CMD_Printf("[WOON Server] %s \n", WOONS_IsStarted()?"Running":"Not running");
#endif

#if defined(CONFIG_HOMMA_SATIP)
		HLIB_CMD_Printf("[SATIP Server] %s \n", SATIP_IsStart()?"Running":"Not running");
#endif
	}
   	else if(VK_strcmp(aucStr, "NETWORK") == 0)
    {
        PL_COMMON_NetworkInfo_t info;
        HLIB_CMD_Printf("\n *** Network Status ** \n");
        PL_COMMON_GetDefaultNetwork(&info);
        HLIB_CMD_Printf("\n *** mac : %s  \n", info.szMacAddr);
        HLIB_CMD_Printf("\n *** ip  : %s  \n", info.szIp);
        HLIB_CMD_Printf("\n *** netmask : %s  \n", info.szNetmask);
        HLIB_CMD_Printf("\n *** gateway : %s  \n", info.szGateway);
    }
	else
	{

	}

	return HxCMD_OK;
}

HINT32   cmd_homma_set_action(void *szArgList)
{
	HCHAR    *aucArg=NULL, aucStr[20]; //aucArg2[256], cid[256];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) 	return HxCMD_ERR;
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )   	return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if(VK_strcmp(aucStr, "LOG") == 0)
	{
		HLIB_CMD_Printf("\n\n*** HOMMA Mxlib Log Level - MORE *** n\n");
		PL_DLNA_MxLib_LogLevel(1);
	}
	else
	{

	}

	return HxCMD_OK;
}
#endif


#define ________________Public_Interfaces__________________________

void	HOMMA_CMD_Register_DMP(void)
{
#if defined(CONFIG_DEBUG)
#define     hCmdHandle      "dmp"
	HLIB_CMD_RegisterWord( hCmdHandle, cmd_dmp_get_action, "get", "dmslist | ", "dmp get action");
	HLIB_CMD_RegisterWord( hCmdHandle, cmd_dmp_set_action, "set", "start | stop | setdms | browse ", "dmp set action");

	HLIB_CMD_RegisterWord( "all", cmd_homma_status_action, "get", "status | network", "homma service status");
	HLIB_CMD_RegisterWord( "all", cmd_homma_set_action, "set", "log", "homma service debug");
#endif
}
#define _____END_OF_FILE_________
