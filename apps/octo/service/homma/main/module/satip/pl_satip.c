/**************************************************************
*	@file	pl_satip.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			SATIP
**************************************************************/

/*
* ¨Ï 2011-2013 Humax Co., Ltd.
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
#include <hapi.h>
#include "pl_satip.h"
#include "pl_common.h"
#include "mxsatip_upnp.h"
#include "mxsatip_server.h"
#include "pl_streaming.h"

#define ________________Typedef____________________________________
//#define	PL_SATIP_DEFAULT_UDN_PREFIX				"1E96FF1A-6771-2F44-F0C5-"

// SES issue 2, Assigned in version 1.00.12
// The server as a UPnP device does not respond to an M-SEARCH request with "ST: upnp:device-UUID" if the character
// case of the hexadecimal values "a" through "f" are modified.
// Each field in a device-UUID is treated as an integer and has its value printed as a zero-filled hexadecimal digit
// string with the most significant digit first. The hexadecimal values "a" through "f" are case insensitive on input.
// http://pubs.opengroup.org/onlinepubs/9629399/apdxa.htm
#define	PL_SATIP_DEFAULT_UDN_PREFIX				"031AFB00-2C16-11E4-8C21-"

#if defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HDNEO)
  #if defined(CONFIG_LXC_ONLY) || defined(CONFIG_CHROOT_AND_LXC)
	#define PL_SATIP_DEFAULT_RTST_PORT				50554
  #else
	#define PL_SATIP_DEFAULT_RTST_PORT				554   // 2014/3/10 mhkang: icordpro_nosecure_defconfigs
  #endif
#else
	#define PL_SATIP_DEFAULT_RTST_PORT				554
#endif

#define PL_SATIP_DEFAULT_UPNP_WEB_PORT			50555

#define PL_SATIP_DEFAULT_UPNP_SSDP_UDP_SEND_PORT	49554

#define PL_SATIP_DEFAULT_RTST_MAX_CONNECTION 	5

typedef struct
{
	HCHAR 	*pMimeType;
	HINT32 	nWidth;
	HINT32 	nHeight;
	HINT32 	nDepth;
	HCHAR 	*pUrl;
	HCHAR 	*pFullPath;
} PL_SATIP_ICON_t;

typedef struct
{
	HBOOL						is_init;
	HCHAR 						szFriendlyName[128];
} PL_SATIP_Context_t;

#define ________________Static_Variables___________________________
static PL_SATIP_Context_t s_PL_SATIP_Context;

static PL_SATIP_ICON_t s_stIcon[] =
{
	/*==================================================================*/
	/*	{ "mime type", 	width,	height,	depth,	"icon url", "fullpath"} */
	/*==================================================================*/
	{ "image/jpeg", 48,		48, 	32,	"/icon/satip_48.jpg", "/usr/dlna/satip_48.jpg"	},
	{ "image/png",	48, 	48, 	32,	"/icon/satip_48.png", "/usr/dlna/satip_48.png"	},
	{ "image/jpeg",	120, 	120, 	32, "/icon/satip_120.jpg", "/usr/dlna/satip_120.jpg"	},
	{ "image/png",	120, 	120, 	32, "/icon/satip_120.png", "/usr/dlna/satip_120.png"	}
};

#define ________________Internal_APIs_Prototype____________________
static HERROR __save_device_id(HINT32 id);
static HERROR __load_device_id(HINT32 *id);

//>> 2014-07-04
static HERROR __save_boot_id(HINT32 id);
static HERROR __load_boot_id(HINT32 *id);
//<< 2014-07-04

#define ________________Internal_Functions_________________________
static PL_SATIP_Context_t *__GetContext( void )
{
	return &s_PL_SATIP_Context;
}

static void __negotiation_device_cb(void *handle, int deviceId, int status)
{
	HxLOG_Debug("##########################################################################\n");
	HxLOG_Debug("[%s%d] handle %p deviceId %d status %d\n", __func__, __LINE__, handle, deviceId, status);
	HxLOG_Debug("##########################################################################\n");

	__save_device_id(deviceId);
}

#define SATIP_DEVICE_ID_KEY		"SATIP_DEVICE_ID_KEY"

static HERROR __save_device_id(HINT32 id)
{
	if(ERR_OK != HAPI_SetRegistryInt(SATIP_DEVICE_ID_KEY, id))
	{
		HxLOG_Warning("ERROR Write DB (%s)\n", SATIP_DEVICE_ID_KEY);
		return ERR_FAIL;
	}

	HxLOG_Debug("SAVE %s : %d \n", SATIP_DEVICE_ID_KEY, id);

	return ERR_OK;
}

static HERROR __load_device_id(HINT32 *id)
{
	HINT32	_id= 0;

	if(ERR_OK != HAPI_GetRegistryInt(SATIP_DEVICE_ID_KEY, &_id))
	{
		HxLOG_Warning("ERROR READ DB (%s)\n", SATIP_DEVICE_ID_KEY);
		return ERR_FAIL;
	}

	HxLOG_Debug("LOAD %s : %d \n", SATIP_DEVICE_ID_KEY, _id);

	*id = _id;

	return ERR_OK;
}

//>> 2014-07-04
#define SATIP_BOOT_ID_KEY		"SATIP_BOOT_ID_KEY"

static HERROR __save_boot_id(HINT32 id)
{
	if(ERR_OK != HAPI_SetRegistryInt(SATIP_BOOT_ID_KEY, id))
	{
		HxLOG_Warning("ERROR Write DB (%s)\n", SATIP_BOOT_ID_KEY);
		return ERR_FAIL;
	}

	HxLOG_Debug("SAVE %s : %d \n", SATIP_BOOT_ID_KEY, id);

	return ERR_OK;
}

static HERROR __load_boot_id(HINT32 *id)
{
	HINT32	_id= 1;

	if(ERR_OK != HAPI_GetRegistryInt(SATIP_BOOT_ID_KEY, &_id))
	{
		HxLOG_Warning("ERROR READ DB (%s)\n", SATIP_BOOT_ID_KEY);
		return ERR_FAIL;
	}

	HxLOG_Debug("LOAD %s : %d \n", SATIP_BOOT_ID_KEY, _id);

	*id = _id;

	return ERR_OK;
}
//<< 2014-07-04


static HERROR __satip_server_init(const HCHAR *pszFriendlyName)
{
	int ret = 0, i = 0, device_id = 0, boot_id = 0;					// 2014-07-04

	HCHAR	szUdn[48] = {0, };
	HCHAR	szFriendlyName[128] = {0, };

	PL_COMMON_NetworkInfo_t	net_info;
	PL_COMMON_ProductInfo_t	product_info;

	MXSATIP_DEVICE 	device_info;
	MXSATIP_ICON 	Icons[4];

	 HxLOG_Info("[%s:%d] (+) FriendlyName = %s \n", __func__, __LINE__, szFriendlyName);

	HxSTD_MemSet(&device_info, 0x00, sizeof(MXSATIP_DEVICE));
	HxSTD_MemSet(&Icons, 0x00, sizeof(MXSATIP_ICON));

	__load_boot_id(&boot_id);											// 2014-07-04
	__load_device_id(&device_id);

	/* if(device_id == 0) 	device_id = 1; */

	device_info.bootId = boot_id;										// 2014-07-04
	device_info.configId = 0;
	device_info.deviceId = device_id;

	PL_COMMON_GetProductInfo(&product_info);
	PL_COMMON_GetDefaultNetwork(&net_info);

	if(pszFriendlyName == NULL)
		HxSTD_snprintf(szFriendlyName, sizeof(szFriendlyName), "%s %s", product_info.szModelName, "SAT>IP");

	HxSTD_snprintf(szFriendlyName, sizeof(szFriendlyName), "%s %s", pszFriendlyName, net_info.szIp);

	HxSTD_snprintf(szUdn, sizeof(szUdn), "%s%s", PL_SATIP_DEFAULT_UDN_PREFIX, net_info.szMacAddr);

	device_info.friendlyName = szFriendlyName;
	device_info.manufacturer  = product_info.szManufacturer;
	device_info.manufacturerURL  = product_info.szManufacturerUrl;
	device_info.modelDescription  = product_info.szModelDescription;
	device_info.modelName  = product_info.szModelName;
	device_info.modelNumber  = product_info.szModelNumber;
	device_info.modelURL = product_info.szModelUrl;
	device_info.serialNumber = product_info.szSerialNumber;
	device_info.UDN = szUdn;

	device_info.IconsLength = 4;
	device_info.Icons = Icons;

	for(i=0; i<device_info.IconsLength; i++)
	{
		device_info.Icons[i].mimeType = s_stIcon[i].pMimeType;
		device_info.Icons[i].width = s_stIcon[i].nWidth;
		device_info.Icons[i].height = s_stIcon[i].nHeight;
		device_info.Icons[i].colorDepth = s_stIcon[i].nDepth;
		device_info.Icons[i].url = s_stIcon[i].pUrl;
		device_info.Icons[i].full_path = s_stIcon[i].pFullPath;
	}

	device_info.presentationURL = "";

	//-------------------------------------------------------------------------------------------
	// SES issue 5 | Assigned in version 1.00.12
	//-------------------------------------------------------------------------------------------
	// The XML <X_SATIPCAP> element content shall be modified from "DVBS2-2,DVBS-2" to "DVBS2-1" as the server is only able to provide one independent front-end to one SAT>IP client.
	//device_info.X_SATIPCAP = "DVBS2-2,DVBS-2";
	device_info.X_SATIPCAP = "DVBS2-1";

	device_info.searchport = PL_SATIP_DEFAULT_UPNP_SSDP_UDP_SEND_PORT;

	ret = mxsatip_server_create(&device_info,\
			PL_SATIP_DEFAULT_UPNP_WEB_PORT, \
			PL_SATIP_DEFAULT_RTST_PORT,\
			PL_SATIP_DEFAULT_RTST_MAX_CONNECTION,\
			PL_STREAMING_MAX_NUM,\
			&__negotiation_device_cb);

	if (ret != MX_RETURN_OK)
	{
		HxLOG_Error("[%s:%d] mxsatip_server_create failed : ret(%d)\n", __func__, __LINE__, ret);
		goto ErrBlock;

	}

	boot_id = boot_id + 1;				// 2014-07-04
	__save_boot_id(boot_id);			// 2014-07-04

	HxLOG_Info("[%s:%d] (-) FriendlyName = %s \n", __func__, __LINE__, szFriendlyName);

	return ERR_OK;

ErrBlock:
	HxLOG_Error("[%s:%d] (-) \n", __func__, __LINE__);
	return ERR_FAIL;
}

static HERROR __satip_server_deinit(void)
{
	HxLOG_Info("[%s:%d] (+) \n", __func__, __LINE__);

	mxsatip_server_destroy();

	HxLOG_Info("[%s:%d] (-) \n", __func__, __LINE__);
	return ERR_OK;
}

#define ________________Public_Interfaces__________________________
HERROR	PL_SATIP_Start( const HCHAR *pszFriendlyName)
{
	HERROR ret = ERR_FAIL;
	PL_SATIP_Context_t *ctx = __GetContext();

	ret = __satip_server_init(pszFriendlyName);

	if(ret != ERR_OK)
		return ERR_FAIL;

	ctx->is_init = TRUE;

	return ERR_OK;
}

HERROR	PL_SATIP_Stop( void )
{
	PL_SATIP_Context_t *ctx = __GetContext();

	ctx->is_init = FALSE;

	return __satip_server_deinit();
}

HERROR	PL_SATIP_SetFriendlyName( const HCHAR *pszFriendlyName)
{
	PL_SATIP_Context_t 	*pCtx = __GetContext();
	HINT32 			nFriendlyNameLen = sizeof(pCtx->szFriendlyName);

	if(HxSTD_StrEmpty(pszFriendlyName))
	{
		HxLOG_Error("[%s:%d] FriendlyName is empty. \n", __func__, __LINE__);
		return ERR_FAIL;
	}

	HxSTD_MemSet(pCtx->szFriendlyName, 0x00, nFriendlyNameLen);
	HxSTD_snprintf(pCtx->szFriendlyName, nFriendlyNameLen, "%s", pszFriendlyName);

	//mxsatip_server_rename(pszFriendlyName);

	return ERR_OK;
}


//>> 2014-07-04
HERROR	PL_SATIP_ChangeBootID(void)
{
	int boot_id = 0;
	MXSATIP_SERVER_CHANGE_DATA change_data = {0};

	__load_boot_id(&boot_id);
	change_data.bootId = boot_id;
	mxsatip_server_change_data(&change_data);
	boot_id = boot_id + 1;
	__save_boot_id(boot_id);

	return ERR_OK;
}
//<<

#define _____END_OF_FILE_________

