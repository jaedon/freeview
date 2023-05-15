#define __HEAD___________________________________________________________________________

#include <napi.h>
#include "atkit.h"

#define __DEFINE_________________________________________________________________________

#define __STATIC_VARIABLES_______________________________________________________________

static HxJSON_t s_manifest = NULL;
static APKS_NETAP_Info_t *pstApInfo = NULL;
static HINT32 lNumberAP = 0;
#define __STATIC_FUNCTIONS_______________________________________________________________

static void TC_WIFI_EventHandler(HINT32 event, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	//APKS_NETDEVINFO_t stApkDevInfo;
	//HINT32 nIfIndex = ulParam1;

	//APK_NETWORK_CONF_GetDevInfo(nIfIndex, &stApkDevInfo);

	switch (event)
	{
		case eNETWORK_STATUS_WIFI_SCANNING_END:
		{
			printf("[%s] Receive an event: eNETWORK_STATUS_WIFI_SCANNING_END\n", __FUNCTION__);
			lNumberAP = ulParam3;
			pstApInfo = (APKS_NETAP_Info_t *)ulParam2;
			break;
		}
		case eNETWORK_STATUS_WIFI_SCANNING_FAIL:
		{
			printf("[%s] TEST FAIL : AP List is NULL!\n", __FUNCTION__);
			break;
		}
		default:
			break;
	}

	return;
}

#define __TESTCASE_______________________________________________________________________

HINT32 TC_WIFI_GetAPList(HxJSON_t j_args)
{
	/* ************ Scenario ****************
		* Get list of devices
		* Check whether device interface is wlan type
		* Call to APK_NETWORK_WIFI_StartScan()
	*****************************************/
	APKS_NETDEVINFO_t stApkDevInfo;
	HINT32 lCount = 0;
	HERROR hErr = ERR_FAIL;
	HINT32 lIndex = 0;
	HBOOL bWlanSupport = FALSE;
	HxJSON_t j_argument = NULL;
	HINT32 lTimeOut = 0;

	printf("TC_NAME ( %s )\n", HLIB_JSON_Object_GetString(j_args, "tc_name"));

	printf("[%s] TESTING.. : Get AP List\n", __FUNCTION__);

	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	if (j_argument != NULL)
	{
		lTimeOut = HLIB_JSON_Object_GetInteger(j_argument, "timeout");
	}
	else
	{
		printf("[%s] TESTING.. : argument is NULL! ( %d )\n", __FUNCTION__, lTimeOut);
		fflush(stdout);
		return FALSE;
	}

	if (lTimeOut < 10000)
	{
		printf("[%s] %s\n", __FUNCTION__, "Timeout is too small! Reset to 10000!\n");
		lTimeOut = 10000; // Random number
	}

	/* Smooth lTimeOut */
	lTimeOut -= lTimeOut % 100;

	hErr = APK_NETWORK_CONF_GetNumOfNetDev(&lCount);
	if (ERR_OK != hErr)
	{
		printf("[%s] TEST FAIL : Fail to APK_NETWORK_CONF_GetNumOfNetDev() - Err ( %d )\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	else
	{
		printf("################ [%s] ################\n", "APK_NETWORK_CONF_GetNumOfNetDev()");
		printf("%s - %d\n", "Number of devices", lCount);
		fflush(stdout);
	}

	for (lIndex = 0; lIndex < lCount; lIndex++)
	{
		hErr = APK_NETWORK_CONF_GetDevInfo(lIndex, &stApkDevInfo);
		if (ERR_OK != hErr)
		{
			printf("[%s] TEST FAIL : Fail to APK_NETWORK_CONF_GetDevInfo() - Err ( %d )\n", __FUNCTION__, hErr);
			fflush(stdout);
			return FALSE;
		}

		if (1 == stApkDevInfo.eType /* eNxNet_INTERFACE_TYPE_WLAN */)
		{
			bWlanSupport = TRUE;
			hErr = APK_NETWORK_WIFI_StartScan(lIndex);
			//HLIB_STD_TaskSleep(2000); // I'm not sure! Wait for data from DI layer
			/* DI layer receives a start_scan cmd from APPKIT layer -> Get data from HW -> DI sends data back along with scanning_end event
			-> APPKIT layer gets data by call the callback function which registered by APK_NETWORK_RegisterNotifer(EvenHandler) */
			if (ERR_OK != hErr)
			{
				printf("[%s] TEST FAIL : Fail to APK_NETWORK_WIFI_StartScan() - Err ( %d )\n", __FUNCTION__, hErr);
				fflush(stdout);
				return FALSE;
			}
		}
	}	

	if (!bWlanSupport)	
	{
		printf("[%s] TEST FAIL : No Support For WLAN!\n", __FUNCTION__);
		fflush(stdout);
		return FALSE;
	}

	while (lTimeOut != 0) { printf("WAITING...\n"); lTimeOut -= 100; HLIB_STD_TaskSleep(100); } // wait for data from DI

	if (lNumberAP > 0)
	{
		for (lIndex = 0; lIndex < lNumberAP; lIndex++)
		{
			//printf("################ [%s] ################\n", "List of AP");
			printf("%s ( %d ) - %s\n", "ESSID", lIndex, pstApInfo[lIndex].szESSID);
			fflush(stdout);
		}
	}
	else
	{
		printf("[%s] TEST FAIL : Fail to get AP list\n", __FUNCTION__);
		fflush(stdout);
		return FALSE;
	}

	printf("[%s] TEST SUCCESS : TC_WIFI_GetAPList()\n", __FUNCTION__);
	fflush(stdout);
	return TRUE;
}

HINT32 TC_WIFI_ConnectAP(HxJSON_t j_args)
{
	/*
	 * Scan the ap list
	 * Try to connect/disconnect each ap
	 */	
	HINT32 lIndex = 0;
	HERROR hErr = ERR_FAIL;	
	HxJSON_t j_argument = NULL;
	HBOOL bAvailableAP = FALSE;
	const HCHAR *szESSID = NULL;
	const HCHAR *szKey = NULL;

	printf("TC NAME ( %s )\n", HLIB_JSON_Object_GetString(j_args, "tc_name"));

	printf("[%s] TESTING.. : Connect/Disconnect AP\n", __FUNCTION__);
	/* Require to initialize AP list before continue */
	if (0 == lNumberAP || NULL == pstApInfo)
	{
		printf("[%s] TEST FAIL : There is no AP!\n", __FUNCTION__);
		fflush(stdout);
		return FALSE;
	}

	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	if (j_argument != NULL)
	{
		szESSID = HLIB_JSON_Object_GetString(j_argument, "essid");
		szKey = HLIB_JSON_Object_GetString(j_argument, "key");
	}
	else
	{
		printf("[%s] TESTING.. : argument is NULL! ( %s ) - ( %s )\n", __FUNCTION__, szESSID, szKey);
		fflush(stdout);
		return FALSE;
	}

	/* Get the right essid */
	for (lIndex = 0; lIndex < lNumberAP; lIndex++)
	{
		if (strncmp(pstApInfo[lIndex].szESSID, szESSID, 48) == 0)
		{
			bAvailableAP = TRUE;
			hErr = APK_NETWORK_WIFI_ConnectAP(lIndex, &pstApInfo[lIndex], (HCHAR *)szKey);
			if (ERR_OK != hErr)
			{
				printf("[%s] TEST FAIL : Fail to APK_NETWORK_WIFI_ConnectAP() - Err( %d )\n", __FUNCTION__, hErr);
				fflush(stdout);
				return FALSE;
			}
			else
			{
				printf("################ [%s] ################\n", "APK_NETWORK_WIFI_ConnectAP()");
				printf("[%s] %s\n", __FUNCTION__, "Successfully connect to AP");
				fflush(stdout);
			}
			/* How to check status of current AP?? */
			HLIB_STD_TaskSleep(2000);

			hErr = APK_NETWORK_WIFI_DisconnectAP(lIndex);
			if (ERR_OK != hErr)
			{
				printf("[%s] TEST FAIL : Fail to APK_NETWORK_WIFI_DisconnectAP() - Err( %d )\n", __FUNCTION__, hErr);
				fflush(stdout);
				return FALSE;
			}
			else
			{
				printf("################ [%s] ################\n", "APK_NETWORK_WIFI_DisconnectAP()");
				printf("[%s] %s\n", __FUNCTION__, "Successfully disconnect AP");
				fflush(stdout);
			}

			HLIB_STD_TaskSleep(2000); // wait for completing disconnect process
		}
	}
	if (!bAvailableAP)
	{
		printf("[%s] TEST FAIL : There is no matched ESSID\n", __FUNCTION__);
		fflush(stdout);
		return FALSE;
	}

	printf("[%s] TEST SUCCESS : TC_WIFI_ConnectAP()\n", __FUNCTION__);
	fflush(stdout);
	return TRUE;
}

void TC_WIFI_Init(HxJSON_t jManifest)
{
	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);

	s_manifest = jManifest;

	//NAPI_Init();
	APK_NETWORK_Init();
	printf("Initialized APK_NETWORK_Init()\n");
	/* Register the EventHandler() to store the aplist */
	APK_NETWORK_RegisterNotifier(TC_WIFI_EventHandler);
	printf("Registered TC_WIFI_EventHandler()\n");

	ADD_TC(TC_WIFI_GetAPList);
	ADD_TC(TC_WIFI_ConnectAP);
}

void TC_WIFI_DeInit(void)
{
	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);
}