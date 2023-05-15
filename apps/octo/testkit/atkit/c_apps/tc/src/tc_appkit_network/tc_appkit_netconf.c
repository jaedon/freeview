#define __HEAD___________________________________________________________________________

#include <napi.h>
#include "atkit.h"

//#define GET_ADDRESS_INFO_FILE_NAME	"/home/atkit/result/get_address_info.result"
//#define INTERNET_CONNECTION_INFO_FILE_NAME	"/home/atkit/result/connection_info.result"
#define __DEFINE_________________________________________________________________________

#define LOCAL_CHECK_INTERNET_MAX_TIME 6000

#define __STATIC_VARIABLES_______________________________________________________________

STATIC HxJSON_t s_manifest = NULL;

#define __STATIC_FUNCTIONS_______________________________________________________________


#define __TESTCASE_______________________________________________________________________

HINT32 TC_NETCONF_DeviceInfo(HxJSON_t j_args)
{
	// Get number of devices
	HINT32 lCount = 0;
	HERROR hErr = ERR_FAIL;
	HINT32 lIndex = 0;
	APKS_NETDEVINFO_t stApkDevInfo;
	APKS_NETIFADDR_t stApkItfAddr;
	HCHAR *szMacAddr = (HCHAR *)malloc(64);
	HUINT8 *p = NULL;

	printf("TC_NAME ( %s )\n", HLIB_JSON_Object_GetString(j_args, "tc_name"));

	printf("[%s] TESTING.. : Get Network Device Information\n", __FUNCTION__);
	// Device Number
	hErr = APK_NETWORK_CONF_GetNumOfNetDev(&lCount);
	if (hErr != ERR_OK)
	{
		printf("[%s] TEST FAIL : Fail to APK_NETWORK_CONF_GetNumOfNetDev() - Err( %d )\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	else
	{
		printf("################ [%s] ################\n", "APK_NETWORK_CONF_GetNumOfNetDev()");
		printf("%s - %d\n", "Number of network device", lCount);
		fflush(stdout);
	}

	// Device info
	if (lCount > 0)
	{	
		for (lIndex = 0; lIndex < lCount; lIndex++) 
		{
			hErr = APK_NETWORK_CONF_GetDevInfo(lIndex, &stApkDevInfo);
			if (ERR_OK != hErr)
			{
				printf("[%s] TEST FAIL : Fail to APK_NETWORK_CONF_GetDevInfo() - Err( %d )\n", __FUNCTION__, hErr);
				fflush(stdout);
				return FALSE;
			}
			else
			{
				printf("################ [%s] ################\n", "APK_NETWORK_CONF_GetDevInfo()");
				printf("%s - %d\n", "ID", lIndex);
				printf("%s - %d\n", "Type", stApkDevInfo.eType);
				printf("%s - %d\n", "Running", stApkDevInfo.bRunning);
				printf("%s - %s\n", "MAC Address", stApkDevInfo.szMacAddr);
				printf("%s - %s\n", "Device Name", stApkDevInfo.szDev_name);
			}		

			if (stApkDevInfo.eType == 0) // eNxNet_INTERFACE_TYPE_LAN
			{
				hErr = APK_NETWORK_CONF_GetAddrInfo(lIndex, &stApkItfAddr);
				if (ERR_OK != hErr)
				{
					printf("[%s] TEST FAIL : Fail to APK_NETWORK_CONF_GetAddrInfo() - Err( %d )\n", __FUNCTION__, hErr);
					fflush(stdout);
					return FALSE;
				}
				else
				{
					printf("################ [%s] ################\n", "APK_NETWORK_CONF_GetAddrInfo()");
					printf("%s - %d\n", "ID", lIndex);
					printf("%s - %d\n", "bDhcpOn", stApkItfAddr.bDhcpOn);
					printf("%s - %d\n", "bDnsAuto", stApkItfAddr.bDnsAuto); p = (HUINT8 *)&(stApkItfAddr.ulIpAddr);
					printf("%s - %d.%d.%d.%d\n", "ipAddr", p[0], p[1], p[2], p[3]); p = (HUINT8 *)&(stApkItfAddr.ulNetmask);
					printf("%s - %d.%d.%d.%d\n", "szNetmask", p[0], p[1], p[2], p[3]); p = (HUINT8 *)&(stApkItfAddr.ulGateway);
					printf("%s - %d.%d.%d.%d\n", "szGateway", p[0], p[1], p[2], p[3]); p = (HUINT8 *)&(stApkItfAddr.ulDns1);
					printf("%s - %d.%d.%d.%d\n", "szDns1", p[0], p[1], p[2], p[3]); p = (HUINT8 *)&(stApkItfAddr.ulDns2);
					printf("%s - %d.%d.%d.%d\n", "szDns2", p[0], p[1], p[2], p[3]); 
				}
			}

			if (stApkDevInfo.eType == 1) // eNxNet_INTERFACE_TYPE_WLAN
			{ 
				hErr = APK_NETWORK_CONF_GetMacAddr(lIndex, szMacAddr, 64);
				if (ERR_OK != hErr)
				{
					printf("[%s] TEST FAIL : Fail to APK_NETWORK_CONF_GetMacAddr() - Err( %d )\n", __FUNCTION__, hErr);
					fflush(stdout);
					return FALSE;
				}
				else
				{
					printf("################ [%s] ################\n", "APK_NETWORK_CONF_GetMacAddr()");
					printf("%s - %s\n", "MAC Address", szMacAddr);
				}
			}
		}
	}
	else
	{
		printf("[%s] TEST FAIL : There is no network device found!\n", __FUNCTION__);
		fflush(stdout);
		return FALSE;
	}	

	printf("[%s] TEST SUCCESS : TC_NETCONF_DeviceInfo()\n", __FUNCTION__);
	fflush(stdout);
	return TRUE;
}

HINT32 TC_NETCONF_InternetConnection(HxJSON_t j_args)
{
	HERROR hErr = ERR_FAIL;
	HINT32 lIsSupport = 0;
	
	printf("TC_NAME ( %s )\n", HLIB_JSON_Object_GetString(j_args, "tc_name"));

	printf("[%s] TESTING.. : Check Internet Connection\n", __FUNCTION__);

	hErr = APK_NETWORK_CONF_IsSupportNetwork(&lIsSupport);
	if (ERR_OK != hErr)
	{
		printf("[%s] TEST FAIL : Fail to APK_NETWORK_CONF_IsSupportNetwork() - Err( %d )\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	else
	{
		printf("################ [%s] ################\n", "APK_NETWORK_CONF_IsSupportNetwork()");
		printf("%s - %s\n", "Is Network Supported ?", lIsSupport ? "Yes" : "No");
	}

	hErr = APK_NETWORK_CONF_CheckInternetConnection(LOCAL_CHECK_INTERNET_MAX_TIME);
	if (ERR_OK != hErr)
	{
		printf("[%s] TEST FAIL : Fail to APK_NETWORK_CONF_CheckInternetConnection() - Err( %d )\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
		//goto END_FUNC;
	}
	else
	{
		printf("################ [%s] ################\n", "APK_NETWORK_CONF_CheckInternetConnection()");
		printf("%s\n", "InternetConnection is successfully connected!");
	}

	printf("[%s] TEST SUCCESS : TC_NETCONF_InternetConnection()\n", __FUNCTION__);
	fflush(stdout);
	return TRUE;
}

void TC_NETCONF_Init(HxJSON_t jManifest)
{
	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);
	s_manifest = jManifest;

	NAPI_Init();

	ADD_TC(TC_NETCONF_DeviceInfo);
	ADD_TC(TC_NETCONF_InternetConnection);
}

void TC_NETCONF_DeInit(void)
{
	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);
}