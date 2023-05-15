/**************************************************************
 *	@file	pl_usercp.c
 *	http://www.humaxdigital.com
 *	@author			humax
 *	@brief			DLNA / USERCP
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
#include <stdio.h>
#include <signal.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <hlib.h>

#include "mxdlna_usercp.h"
#include "mxfile.h"
#include "addressmanager_c.h"


#define	________________Define_______________________________
#define USER_DEVICE_TYPE	"urn:schemas-upnp-org:device:HumaxOtt:1"
#define ADDRESS_XML_PATH	"/var/lib/humaxtv_user/allowedmaclist.xml"
#define MAX_ITEM_COUNT 256


#define ________________Static_Variables___________________________
static MXDLNA_CP_HANDLE g_cp = NULL;


#define ________________Internal_APIs_Prototype____________________
void PL_USERCP_Recovery(void);


#define ________________Internal_Functions_________________________
static void __pl_usercp_cbUserDeviceAddRemove(MXDLNA_CP_HANDLE hCp, MXDLNA_DEVICE *device, int added)
{
	//char ip[100]={0,};
	//int port =80;
	//char page[100]={0,};
	char addr[5][100];
	char mac[100] ={0,};

	HxLOG_Debug("UerCP: name:%s udn:%s added:%d\n", device->FriendlyName, device->UDN, added);

	//sscanf(device->LocationURL, "http://%99[^:]:%99d/%99[^\n]]", ip, &port, page);
	sscanf(device->UDN, "%99[^-]-%99[^-]-%99[^-]-%99[^-]-%99[^\n]", addr[0], addr[1],addr[2],addr[3],addr[4]);

	sprintf(mac, "%c%c:%c%c:%c%c:%c%c:%c%c:%c%c",
			addr[4][0], addr[4][1], addr[4][2], addr[4][3], addr[4][4], addr[4][5], addr[4][6],	addr[4][7], addr[4][8], addr[4][9], addr[4][10], addr[4][11]);

	if (added)
	{
		if(ADDRMGR_GetItemCount() >= MAX_ITEM_COUNT)
		{
			HxLOG_Warning("Current item count is bigger than max item count -> Recovery\n", ADDRESS_XML_PATH);
			PL_USERCP_Recovery();
		}
		else if(ADDRMGR_AddAddress(mac) == TRUE)
		{
			if(ADDRMGR_SaveAsFile() != TRUE)
			{
				HxLOG_Error("Fail to write XML file -> Recovery\n");
				PL_USERCP_Recovery();
			}
			else
			{
				HxLOG_Warning("Success save as file (%s)\n", ADDRESS_XML_PATH);
			}
		}
		else
		{
			HxLOG_Warning("Mac address is duplicated (%s)\n", mac);
		}
	}
}

static HERROR __pl_usercp_createUserCp(void)
{
	MXDLNA_CP_CONFIG cp_config = { NULL,};

	cp_config.DeviceAddOrRemove  = __pl_usercp_cbUserDeviceAddRemove;
	cp_config.UserDeviceTypeName = USER_DEVICE_TYPE;

	mxDlnaUserCp_configure(g_cp, &cp_config);

	g_cp =  mxDlnaUserCp_createUserCp();
	if (g_cp == NULL)
	{
		HxLOG_Error("Cannot create usercp!\n");
		return ERR_FAIL;
	}

	mxDlnaUserCp_start(g_cp);

	return ERR_OK;
}

static HERROR __pl_usercp_destroyUserCp(void)
{
	if (g_cp == NULL)
	{
		HxLOG_Error("There is no usercp!\n");
		return ERR_FAIL;
	}

	mxDlnaUserCp_end(g_cp);
	mxDlnaUserCp_destroyUserCp(g_cp);
	mxDlnaUserCp_unconfigure(g_cp);
	g_cp = NULL;

	return ERR_OK;
}

#define ________________Public_Interfaces__________________________
HERROR PL_USERCP_Init(void)
{
	HBOOL bRet = FALSE;

	// Init manager
	if(ADDRMGR_Init(ADDRESS_XML_PATH) != TRUE)
	{
		HxLOG_Error("ADDRMGR_Init() fail\n");
		return ERR_FAIL;
	}

	// Parsing XML to FILE
	bRet = ADDRMGR_Parse();
	HxLOG_Debug("bRet : %d\n", bRet);

	(void)__pl_usercp_createUserCp();

	if(bRet != TRUE)
	{
		HxLOG_Error("Fail to parse XML file -> Recovery\n");
		PL_USERCP_Recovery();
	}

	return ERR_OK;
}

void PL_USERCP_Deinit(void)
{
	(void)__pl_usercp_destroyUserCp();

	ADDRMGR_Deinit();
}

void PL_USERCP_Recovery(void)
{
	if(g_cp != NULL)
	{
		HxLOG_Debug("Delete file (%s)\n", ADDRESS_XML_PATH);
		HLIB_FILE_Delete(ADDRESS_XML_PATH);

		(void)__pl_usercp_destroyUserCp();

		ADDRMGR_Reset();

		(void)__pl_usercp_createUserCp();

		HxLOG_Debug("UPnP Device Discovery\n");
	}
}

