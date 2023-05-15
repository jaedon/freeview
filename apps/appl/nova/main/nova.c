/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
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

/**************************************************************************************************/
#define _________Nova_Private_Include_________________________________________________
/**************************************************************************************************/
#include <uuid/uuid.h>
#include <nova_int.h>
#include <apk_network.h>
#include <nova_cas.h>
#include "configmanager_c.h"
#include "nova_utils.h"

/**************************************************************************************************/
#define _________Nova_Private_Macro_________________________________________________
/**************************************************************************************************/
#define NOVA_DEFAULT_MODELNAME			"NovaspreadServer"

#define NOVA_CONFIG_VERSION				"2016-01-30"		// If ConfigVersion Info is different from the version of /var/lib/humaxlib/nova_config.xml, default config must be loaded.  ConfigVersion string is  any free string.


#define NOVA_CONFIG_FILEPATH			"/var/lib/humaxtv/tara/nova_config.xml"
#define NOVA_CONFIG_UDN					"UDN"
#define NOVA_CONFIG_NUID				"NUID"
#define NOVA_CONFIG_SATIP_EXTENSION		"SATIP_EXTENSION"	// In case of using Transcoding & DRM functions for sat>ip streaming, Set this field as TRUE.

#define NOVA_LXC_IP						"10.255.128.216"
#define NOVA_UPNP_HTTP_PORT				50555  // 2015/10/5 mhakng: This port must be bigger than 49152(homma sat>ip와 동일한 port number로 설정함)
#ifdef CONFIG_ENHANCED_SECURITY
	#define NOVA_RTSP_PORT				50554	// 2016/1/6 mhkang: eth0 554port으로 수신되는 패킷을 50554 port로 포워딩하도록 iptables 수정해야 한다(http://svn.humaxdigital.com:3000/issues/110020)
#else
	#define NOVA_RTSP_PORT				554
#endif


/**************************************************************************************************/
#define _________Nova_Private_Struct_________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _________Nova_Private_Value_________________________________________________
/**************************************************************************************************/
NOVA_Server_t g_ServerContext = {false, false, false, false, NOVA_DEFAULT_MODELNAME};
HANDLE_CONIFG	g_hConfig = NULL;

/**************************************************************************************************/
#define _________Nova_Private_Prototype_________________________________________________
/**************************************************************************************************/
static void			nova_GenerateUDN(char* aUdnBuffer );
static char*		nova_GetIpAddress(void);
static char*		nova_GetFriendlyName(char* ModelName);
static bool			nova_CanServerStart(void);
static bool			nova_CanServerStop(void);
static bool			nova_CanServerRename(void);
static bool			nova_IsServerRunning(void);
static bool			nova_CanServerRestart(void);
static bool			nova_LoadDefaultConfig(void);

/**************************************************************************************************/
#define _________Nova_Public_Functions__________________________________________________________
/**************************************************************************************************/
bool NOVA_ServerInit(void)
{
	NovaspreadTServerInitParameters serverInitParameters;

	HxLOG_Info(HxANSI_COLOR_YELLOW("[%s:%d]\n"), __FUNCTION__, __LINE__);

	g_hConfig = CONFIG_Create(NOVA_CONFIG_VERSION);

	if (CONFIG_Read(g_hConfig, NOVA_CONFIG_FILEPATH) == false)
	{
		if (nova_LoadDefaultConfig() == false)
		{
			HxLOG_Error(HxANSI_COLOR_RED("nova_LoadDefaultConfig failed\n"));
			return false;
		}
	}

	if (g_ServerContext.isInitialzed == true)
	{
		HxLOG_Warning("NovaspreadServer is already initialized!\n");
		return true;
	}

	memset(&g_ServerContext, 0, sizeof(g_ServerContext));

#ifdef CONFIG_DEBUG	// 2016/1/7 mhkang: in case of using debug mode, eNETWORK_STATUS_CON_SUCCESS event does not occur!!! So in case of using debug mode, isNetworkActive must set true focibly!
	g_ServerContext.isNetworkActive = true;
#endif

	memset( &serverInitParameters, 0, sizeof( serverInitParameters ));
	serverInitParameters.DataPath				= "/var/lib/humaxtv/tara";
	serverInitParameters.SatIpRtspPort			= NOVA_RTSP_PORT;
	serverInitParameters.UpnpHttpPort			= NOVA_UPNP_HTTP_PORT;

	serverInitParameters.UniqueDeviceName		= CONFIG_GetString(g_hConfig, NOVA_CONFIG_UDN);
	serverInitParameters.EnableSatIpExtensions	= CONFIG_GetValue(g_hConfig, NOVA_CONFIG_SATIP_EXTENSION);
	serverInitParameters.ChipsetUid				= (NovaspreadTUInt32)CONFIG_GetValue(g_hConfig, NOVA_CONFIG_NUID);

	serverInitParameters.EnableMemoryChecks		= NOVASPREAD_TRUE;

	if(NovaspreadServerInit( &serverInitParameters ) != NOVASPREAD_TRUE)
	{
		HxLOG_Error("NovaspreadServerInit failed!\n");
		goto ErrBlock;
	}

	g_ServerContext.isInitialzed = true;

	return true;

ErrBlock:
	return false;
}

bool NOVA_ServerDeinit(void)
{
	HxLOG_Info(HxANSI_COLOR_YELLOW("[%s:%d]\n"), __FUNCTION__, __LINE__);

	if (g_ServerContext.isInitialzed == false)
	{
		return true;
	}

	NovaspreadServerDone();

	g_ServerContext.isInitialzed = false;

	CONFIG_Save(g_hConfig);
	CONFIG_Close(g_hConfig);
	g_hConfig = NULL;

	return true;
}

bool NOVA_ServerStart(char* ModelName)
{
	char*	FriendlyName= NULL;
	char*	IPAddress = NULL;

	HxLOG_Info(HxANSI_COLOR_YELLOW("[%s:%d]\n"), __FUNCTION__, __LINE__);

	if (nova_CanServerStart() == false)
	{
		goto ErrBlock;
	}

	FriendlyName = nova_GetFriendlyName(ModelName);
	if (FriendlyName == NULL)
	{
		HxLOG_Error("Cannot make FriendlyName.\n");
		goto ErrBlock;
	}

	if(NovaspreadServerSetFriendlyName(FriendlyName) != NOVASPREAD_TRUE)
	{
		HxLOG_Error("NovaspreadServerSetFriendlyName() Error..\n");
		goto ErrBlock;
	}

	IPAddress = nova_GetIpAddress();
	if (IPAddress == NULL)
	{
		HxLOG_Error("nova_GetIpAddress failed\n");
		goto ErrBlock;
	}

	/* 2016/1/28 mhkang: [SES JIRA] https://application.ses-ps.com/jira/browse/MUL-73
		ServerStart하기 전에 NovaspreadServerSetHostIpAddress 호출하지 않으면, SAT>IP Streaming중에 DESCRIBE응답 SDP 패킷이 Malformed Packet으로 생성되는 버그가 있음.
	*/
#ifdef CONFIG_CHROOT_AND_LXC
	if (NovaspreadServerSetHostIpAddress(NOVA_LXC_IP) == NOVASPREAD_FALSE)
	{
		HxLOG_Error("NovaspreadServerSetHostIpAddress(%s) failed\n", IPAddress);
		goto ErrBlock;
	}

	if (NovaspreadServerSetExternalIpAddress(IPAddress, NOVA_UPNP_HTTP_PORT) == NOVASPREAD_FALSE)
	{
		HxLOG_Error("NovaspreadServerSetExternalIpAddress(%s, %d) failed\n", IPAddress, NOVA_UPNP_HTTP_PORT);
		goto ErrBlock;
	}
#else
	if (NovaspreadServerSetHostIpAddress((const char*)IPAddress) == NOVASPREAD_FALSE)
	{
		HxLOG_Error("NovaspreadServerSetHostIpAddress(%s) failed\n", IPAddress);
		goto ErrBlock;
	}
#endif

	if (  NovaspreadServerStart() != NOVASPREAD_TRUE)
	{
		HxLOG_Error("NovaspreadServerStart failed\n");
		goto ErrBlock;
	}

	if (FriendlyName != NULL)
	{
		HLIB_STD_MemFree(FriendlyName);
	}

	if (IPAddress != NULL)
	{
		HLIB_STD_MemFree(IPAddress);
		IPAddress = NULL;
	}

	g_ServerContext.isRunnging = true;

	return true;

ErrBlock:
	if (FriendlyName != NULL)
	{
		HLIB_STD_MemFree(FriendlyName);
	}

	if (IPAddress != NULL)
	{
		HLIB_STD_MemFree(IPAddress);
		IPAddress = NULL;
	}

	return false;
}

bool  NOVA_ServerShutdown(void)
{
	HxLOG_Info(HxANSI_COLOR_YELLOW("[%s:%d]\n"), __FUNCTION__, __LINE__);

	if (nova_CanServerStop() == false)
	{
		goto ErrBlock;
	}

	NovaspreadServerStop();

	g_ServerContext.isRunnging = false;

	return true;

ErrBlock:
	return false;
}

bool NOVA_ServerSetFriendlyName(char* ModelName)
{
	char* FriendlyName = NULL;

	HxLOG_Info(HxANSI_COLOR_YELLOW("[%s:%d]\n"), __FUNCTION__, __LINE__);

	if (nova_CanServerRename() == false)
	{
		goto ErrBlock;
	}

	FriendlyName = nova_GetFriendlyName(ModelName);
	if (FriendlyName == NULL)
	{
		HxLOG_Error("Cannot make FriendlyName.\n");
		goto ErrBlock;
	}

	NovaspreadServerStop();

	if(NovaspreadServerSetFriendlyName(FriendlyName) != NOVASPREAD_TRUE)
	{
		HxLOG_Error("NovaspreadServerSetFriendlyName Failed\n");
		goto ErrBlock;
	}

	if (  NovaspreadServerStart() != NOVASPREAD_TRUE)
	{
		HxLOG_Error("NovaspreadServerStart failed\n");
		goto ErrBlock;
	}

	if (FriendlyName != NULL)
	{
		HLIB_STD_MemFree(FriendlyName);
	}

	return true;

ErrBlock:
	if (FriendlyName != NULL)
	{
		HLIB_STD_MemFree(FriendlyName);
	}
	return false;
}

bool NOVA_ServerRestart(void)
{
	char* IPAddress = NULL;
	char* FriendlyName = NULL;

	HxLOG_Info(HxANSI_COLOR_YELLOW("[%s:%d] +\n"), __FUNCTION__, __LINE__);

	if (nova_CanServerRestart() == false)
	{
		goto ErrBlock;
	}

	if (nova_IsServerRunning() == true)
	{
		NovaspreadServerStop();
		g_ServerContext.isRunnging = false;
	}

	FriendlyName = nova_GetFriendlyName(g_ServerContext.modelName);
	if (FriendlyName == NULL)
	{
		HxLOG_Error("Cannot make FriendlyName.\n");
		goto ErrBlock;
	}

	if(NovaspreadServerSetFriendlyName(FriendlyName) != NOVASPREAD_TRUE)
	{
		HxLOG_Error("NovaspreadServerSetFriendlyName Failed\n");
		goto ErrBlock;
	}

	IPAddress = nova_GetIpAddress();
	if (IPAddress == NULL)
	{
		HxLOG_Error("nova_GetIpAddress failed\n");
		goto ErrBlock;
	}

#ifdef CONFIG_CHROOT_AND_LXC
	if (NovaspreadServerSetHostIpAddress(NOVA_LXC_IP) == NOVASPREAD_FALSE)
	{
		HxLOG_Error("NovaspreadServerSetHostIpAddress(%s) failed\n", IPAddress);
		goto ErrBlock;
	}

	if (NovaspreadServerSetExternalIpAddress(IPAddress, NOVA_UPNP_HTTP_PORT) == NOVASPREAD_FALSE)
	{
		HxLOG_Error("NovaspreadServerSetExternalIpAddress(%s, %d) failed\n", IPAddress, NOVA_UPNP_HTTP_PORT);
		goto ErrBlock;
	}
#else
	if (NovaspreadServerSetHostIpAddress((const char*)IPAddress) == NOVASPREAD_FALSE)
	{
		HxLOG_Error("NovaspreadServerSetHostIpAddress(%s) failed\n", IPAddress);
		goto ErrBlock;
	}
#endif

	if (NovaspreadServerStart() == NOVASPREAD_FALSE)
	{
		HxLOG_Error("NovaspreadServerStart() failed\n");
		goto ErrBlock;
	}

	if (IPAddress != NULL)
	{
		HLIB_STD_MemFree(IPAddress);
		IPAddress = NULL;
	}

	if (FriendlyName != NULL)
	{
		HLIB_STD_MemFree(FriendlyName);
		FriendlyName = NULL;
	}

	g_ServerContext.isRunnging = true;

	return true;

ErrBlock:

	if (IPAddress != NULL)
	{
		HLIB_STD_MemFree(IPAddress);
		IPAddress = NULL;
	}

	if (FriendlyName != NULL)
	{
		HLIB_STD_MemFree(FriendlyName);
		FriendlyName = NULL;
	}

	return false;
}

bool NOVA_ServerFactoryReset(void)
{
	bool ret = true;

	if ( NovaspreadServerFactoryReset() == NOVASPREAD_FALSE)
	{
		ret = false;
	}

	if ( nova_LoadDefaultConfig() == false)
	{
		ret = false;
	}

	return ret;
}


HBOOL NOVA_IsActiveNetwork(void)
{
	HINT32				ulDevCnt, i;
	HERROR				hErr;
	APKS_NETIFADDR_t	Info;

	hErr = APK_NETWORK_CONF_GetNumOfNetDev(&ulDevCnt);
	if ( hErr != ERR_OK )
	{
		HxLOG_Error("APK_NETWORK_CONF_GetNumOfNetDev failed(eRet:%d)\n", hErr);
		return FALSE;
	}

	for (i = 0 ; i < ulDevCnt ; i++)
	{
		HxSTD_MemSet(&Info, 0x00, sizeof(APKS_NETIFADDR_t));

		hErr = APK_NETWORK_CONF_GetAddrInfo(i, &Info);

		if ( hErr == ERR_OK )
		{
			if (Info.bActive == TRUE) //  IP address had been allocated.
			{
				return TRUE;
			}
		}
		else
		{
			HxLOG_Error("NAPI_NetConf_GetInfo(idx:%d) failed(eRet:%d)\n", i, hErr);
		}
	}

	return FALSE;
}

/**************************************************************************************************/
#define _________Nova_Private_functions________________________________________________
/**************************************************************************************************/
HCHAR*   nova_GetIpAddress(void)
{
	HINT32				ulDevCnt, i;
	HERROR				hErr;
	APKS_NETIFADDR_t	Info;
	HCHAR				IPAddress[APKD_NETWORK_MAX_IP4ADDR];

	hErr = APK_NETWORK_CONF_GetNumOfNetDev(&ulDevCnt);
	if ( hErr != ERR_OK )
	{
		HxLOG_Error("APK_NETWORK_CONF_GetNumOfNetDev failed(eRet:%d)\n", hErr);
		return NULL;
	}

	for (i = 0 ; i < ulDevCnt ; i++)
	{
		HxSTD_MemSet(&Info, 0x00, sizeof(APKS_NETIFADDR_t));

		hErr = APK_NETWORK_CONF_GetAddrInfo(i, &Info);

		if ( hErr == ERR_OK )
		{
			if (Info.bActive == TRUE) //  IP address had been allocated.
			{
				APK_NETWORK_CONF_AddrN2A( Info.ulIpAddr, IPAddress, APKD_NETWORK_MAX_IP4ADDR );
				return HLIB_STD_StrDup(IPAddress);
			}
		}
		else
		{
			HxLOG_Error("NAPI_NetConf_GetInfo(idx:%d) failed(eRet:%d)\n", i, hErr);
		}
	}

	return NULL;
}

static void	nova_GenerateUDN (char* aUdnBuffer )
{
	char uuid_str[50];
	uuid_t uuid;

	uuid_generate(uuid);

	uuid_unparse(uuid, uuid_str);

	sprintf( aUdnBuffer, "uuid:%s", uuid_str);
}


char* nova_GetFriendlyName(HCHAR* ModelName)
{
	char*	IpAddress= NULL;
	char	SatIpServerName[NOVA_FRIENDLY_NAME_LENGTH];

	IpAddress = nova_GetIpAddress();
	if (IpAddress == NULL)
	{
		IpAddress = strdup("?.?.?.?");
	}

	snprintf(SatIpServerName, NOVA_FRIENDLY_NAME_LENGTH, "%s SAT>IP SERVER %s", ModelName, IpAddress);

	if (IpAddress != NULL)
	{
		HLIB_STD_MemFree(IpAddress);
	}

	return strdup(SatIpServerName);
}

bool nova_CanServerStart(void)
{
	if (g_ServerContext.isInitialzed	== false)	return false;
	if (g_ServerContext.isNetworkActive == false)	return false;
	if (g_ServerContext.isServerActive	== false)	return false;	// NetworkServer menu must be 'ON'

	if (g_ServerContext.isRunnging		== true)	return false;

	return true;
}

bool nova_CanServerStop(void)
{
	if (g_ServerContext.isInitialzed	== false)	return false;
	if (g_ServerContext.isRunnging		== false)	return false;

	return true;
}

bool nova_CanServerRename(void)
{
	if (g_ServerContext.isInitialzed	== false)	return false;
	if (g_ServerContext.isRunnging		== false)	return false;

	return true;
}

bool nova_CanServerRestart(void)
{
	if (g_ServerContext.isInitialzed	== false)	return false;
	if (g_ServerContext.isNetworkActive == false)	return false;
	if (g_ServerContext.isServerActive	== false)	return false;

	return true;
}

bool nova_IsServerRunning(void)
{
	return g_ServerContext.isRunnging;
}

bool nova_LoadDefaultConfig(void)
{
	char UDN[100];
	HUINT32 ulChipsetNUID = 0;
	HERROR hErr;
	char Value[1024];
	int ret;

	HxLOG_Info(HxANSI_COLOR_YELLOW("[%s:%d] Nova process will load default config\n"), __FUNCTION__, __LINE__);

	// create directory : /var/lib/humaxtv/tara
	sprintf(Value, "%s/tara", CONFIG_HUMAXTV_PATH);
	ret = mkdir(Value, 0777);
	if (ret != 0)
	{
		HxLOG_Warning("mkdir(%s, 0777) failed(errno:%d)\n", Value, errno);
	}


	// Set ConfigVersion
	CONFIG_SetConfigVersion(g_hConfig, NOVA_CONFIG_VERSION);

	// Set UDN
	nova_GenerateUDN(UDN);
	CONFIG_SetString(g_hConfig, NOVA_CONFIG_UDN, UDN);

	// Set ChipsetUID
	hErr = NOVA_CAS_CA_GetNuid(&ulChipsetNUID);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("NOVA_CAS_CA_GetNuid failed(hErr:%d)\n", hErr);
		return false;
	}

	sprintf(Value, "%u", ulChipsetNUID);
	CONFIG_SetString(g_hConfig, NOVA_CONFIG_NUID, Value);

	// Set SAT>IP Extention Function :  Support Transcoding and DRM Streaming
	CONFIG_SetValue(g_hConfig, NOVA_CONFIG_SATIP_EXTENSION, NOVASPREAD_TRUE);

	// Save Config
	if (CONFIG_Save(g_hConfig) == false)
	{
		HxLOG_Error("CONFIG_Save failed(errno:%d)\n", errno);
	}

	CONFIG_PrintAll(g_hConfig);

	// Copy files from /usr/tara/*  to /var/lib/humaxtv/tara/*
	if (CONFIG_CopyFilesToFlash() == false)
	{
		HxLOG_Error(HxANSI_COLOR_RED("CONFIG_CopyFilesToFlash failed\n"));
	}

	return true;
}

/**************************************************************************************************/
#define _________Nova__End_Of_File____________________________________________________
/**************************************************************************************************/


