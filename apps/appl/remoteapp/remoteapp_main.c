/**************************************************************
 *	@file		remoteapp_main.c
 *	RemoteApp Server Application for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/07/25
 *	@author			humax
 *	@brief			Remote App Receiver
 ***************************************************************/
#include <ondk.h>
#include	<stdarg.h>
#include 	<sys/socket.h>

#include	<oapi.h>
#include	<hapi.h>
#include 	<apk.h>

#if 0//defined(CONFIG_ENHANCED_SECURITY)
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/resource.h>
#endif

#include "remoteapp_udp.h"
#include "remoteapp_parser.h"

#if defined(CONFIG_SUPPORT_REMOTEAPP_ON_USERDEVICE)
#include "mxdlna_userdevice.h"
#define	REMOTEAPP_MAX_MACADDR_LEN		20
#define REMOTEAPP_DEFAULT_UDN_PREFIX	"1E96FF1A-6771-2F44-G0C5-"
#define REMOTEAPP_MAX_UDN_LEN			48
#endif

typedef struct
{
#if defined(CONFIG_SUPPORT_REMOTEAPP_ON_USERDEVICE)
	MXUSER_DEVICE_HANDLE  pvDSHandle;
#endif
}RemoteAppInst_t;

#if defined(CONFIG_SUPPORT_REMOTEAPP_ON_USERDEVICE)
static	RemoteAppInst_t* remoteapp_getInstance(void)
{
	static	RemoteAppInst_t *s_pstRemoteApp = NULL;

	if(s_pstRemoteApp == NULL)
	{
		s_pstRemoteApp = (RemoteAppInst_t*)HLIB_STD_MemCalloc(sizeof(RemoteAppInst_t));
	}
	return s_pstRemoteApp;
}
#endif


HINT32	runRemoteAppReceiver(void)
{
	HERROR		hErr = 0;
	HINT32		fd = -1;
	HINT32		isNonBlock = 0;

	RemoteRCU_Rcu_t		stRemoteApp;
	HINT32			nLen = 0;
	HINT32			nReadLen = 0;
	void*			pvResult = NULL;
	RemoteData_e	peResultType = eRemote_Max;
//	HINT32			uKeySymbol = 0;

	HxLOG_Info("%s() +\n", __FUNCTION__);

	nLen = sizeof(RemoteRCU_Rcu_t);

	while (1)
	{
		if (fd == -1)
		{
			hErr = remoteapp_socket_open(SOCK_DGRAM, &fd, isNonBlock);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("%s() Fail to open socket\n", __FUNCTION__);
				break;
			}
			HxLOG_Info("%s() open remoteapp udp socket\n", __FUNCTION__);

			hErr = remoteapp_socket_bind(fd);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("%s() Fail to bind socket\n", __FUNCTION__);
				break;
			}
		}

		VK_memset(&stRemoteApp, 0, nLen);
		//recv
		hErr = remoteapp_socket_recvfrom(fd, nLen, (HUINT8*)&stRemoteApp, NULL, NULL, &nReadLen);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("%s() Fail to recv\n", __FUNCTION__);
			remoteapp_socket_close(fd);
			fd = -1;
		}
		else
		{
			if (nReadLen != nLen)
			{
				HxLOG_Error("%s() Data \n", __FUNCTION__);
			}
			//if (check data is error) sendto err_msg;
			//else

			hErr = remoteapp_parser(&stRemoteApp, &pvResult, &peResultType);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("%s() Fail to parsing\n", __FUNCTION__);
				if(pvResult != NULL)
				{
					HLIB_STD_MemFree(pvResult);
					pvResult = NULL;
				}
				continue;
			}

			if(peResultType == eRemote_RCU || peResultType == eRemote_Keyboard_Code)
			{
				HxLOG_Error("%s() Send Key %d\n", __FUNCTION__, *((HUINT32 *)pvResult));
				hErr = HAPI_SendRemoteRCUKey(*((HUINT32 *)pvResult));
				hErr |= HAPI_SendRemoteRCUKeyRelease(*((HUINT32 *)pvResult));
				if(hErr != ERR_OK)
				{
					HxLOG_Error("%s() Send RCU Key Error\n", __FUNCTION__);
				}
			}
			else if(peResultType == eRemote_Keyboard_Char || peResultType == eRemote_Keyboard_String)
			{
				HxLOG_Error("%s() Send String %s\n", __FUNCTION__, (HUINT8 *)pvResult);

				HAPI_BroadcastSignal("signal:onRemoteAppString", 0, "s", pvResult);
			}
			else
			{
				HxLOG_Error("%s() Unknown Type\n", __FUNCTION__);
			}

			if(pvResult != NULL)
			{
				HLIB_STD_MemFree(pvResult);
				pvResult = NULL;
			}
		}

		//HLIB_STD_TaskSleep(10);
	}

	if (fd != -1)
	{
		remoteapp_socket_close(fd);
	}

	HxLOG_Info("%s() -\n", __FUNCTION__);

	return 0;
}

#if defined(CONFIG_SUPPORT_REMOTEAPP_ON_USERDEVICE)
static HERROR	remoteapp_GetMacAddr(HINT32 *pnIndex, HCHAR *pszMacAddress)
{
	HERROR 	hErr = ERR_OK;
	HINT32	i = 0, j = 0, nMacLen = 0;
	HCHAR	szTempMacAddress[REMOTEAPP_MAX_MACADDR_LEN] = { 0, };

	HxLOG_Info("@@@@@ [%s:%d] %s (+)\n", __func__, __LINE__, __func__);

	if(pnIndex == NULL)
	{
		HxLOG_Error("@@@@@ [%s:%d] pnIndex is NULL\n", __func__, __LINE__);
		hErr = ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	if(pszMacAddress == NULL)
	{
		HxLOG_Error("@@@@@ [%s:%d] pszMacAddress is NULL\n", __func__, __LINE__);
		hErr = ERR_FAIL;
		goto FUNCTION_EXIT;
	}

	hErr = APK_NETWORK_CONF_GetDefaultDev(pnIndex);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("@@@@@ [%s:%d] Getting default dev is failed\n", __func__, __LINE__);
		goto FUNCTION_EXIT;
	}

	hErr = APK_NETWORK_CONF_GetMacAddr(*pnIndex, szTempMacAddress, REMOTEAPP_MAX_MACADDR_LEN);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("@@@@@ [%s:%d] Getting MAC Addr is failed\n", __func__, __LINE__);
		goto FUNCTION_EXIT;
	}

	nMacLen = HxSTD_StrLen(szTempMacAddress);

	for(i = 0, j = 0; i < nMacLen; i++, j++)
	{
		if(szTempMacAddress[i] == ':')
		{
			pszMacAddress[j] = szTempMacAddress[i + 1];
			i++;
		}
		else
		{
			pszMacAddress[j] = szTempMacAddress[i];
		}

		if(pszMacAddress[j] >= 'a' && pszMacAddress[j] <= 'z')
		{
			pszMacAddress[j] += ('A' - 'a');
		}
	}

FUNCTION_EXIT:
	HxLOG_Info("@@@@@ [%s:%d] %s (-)\n", __func__, __LINE__, __func__);

	return hErr;
}

static HERROR	remoteapp_CreateUserDevice(MXUSER_DEVICE_INFO_t *deviceInfo)
{
	MXUSER_DEVICE_INFO_t    *pstInfo = NULL;
    MXUSER_DEVICECAP        *pstUserDevCap = NULL;
	HCHAR					*pszModelName = NULL;
	HCHAR					szMacAddress[REMOTEAPP_MAX_MACADDR_LEN] = { 0, };
	HINT32					nIndex = 0;
	HERROR					hErr = ERR_OK;
	HCHAR					szUDN[REMOTEAPP_MAX_UDN_LEN] = { 0, };

	RemoteAppInst_t* remoteInst = remoteapp_getInstance();

	if (remoteInst == NULL)
		return ERR_FAIL;

	// Initialize value
    pstInfo = (MXUSER_DEVICE_INFO_t *)HLIB_STD_MemAlloc(sizeof(MXUSER_DEVICE_INFO_t));
    memset( (void *)pstInfo, 0, sizeof(MXUSER_DEVICE_INFO_t) );
    pstUserDevCap = (MXUSER_DEVICECAP *)HLIB_STD_MemAlloc(sizeof(MXUSER_DEVICECAP));
    memset( (void *)pstUserDevCap, 0, sizeof(MXUSER_DEVICECAP) );

	pszModelName = (HCHAR*)HLIB_STD_MemCalloc(sizeof(HCHAR) * 128);

    // Setup user device capability
    pstUserDevCap->NamespacePrefix = "hmx";
    pstUserDevCap->Namespace = "urn:schemas-humax-co-kr:metadata-1-0";
    pstUserDevCap->AttributeName = "X_HMXCAP";
#ifdef CONFIG_PROD_HD9000I
    pstUserDevCap->Value = "RCUTYPE_LIME";
#else
    pstUserDevCap->Value = "RCUTYPE_LEGACY_SPP";
#endif
    pstUserDevCap->next = NULL;

    // Setup user device info
    pstInfo->pUserDeviceCapability = pstUserDevCap;
    pstInfo->devicetype = "urn:schemas-upnp-org:device:HMXRCUServer:1";
    // TODO : Add postfix to friendlyName with IP addr
    pstInfo->friendlyName = "HMX RCU Server";
    pstInfo->manufacturer = "HUMAX Co., Ltd.";
    pstInfo->manufacturerURL = "www.humaxdigital.com";

	/* UDN Setting */
	hErr = remoteapp_GetMacAddr(&nIndex, szMacAddress);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("@@@@@ [%s:%d] Getting MAC Addr is failed, can't set RCU Server UDN.\n", __func__, __LINE__);
	}
	else
	{
		snprintf(szUDN, REMOTEAPP_MAX_UDN_LEN, "%s%s", REMOTEAPP_DEFAULT_UDN_PREFIX, szMacAddress);
		pstInfo->UDN = szUDN;
	}

	APK_SYSINFO_STB_GetModelName(pszModelName, 128);

	pstInfo->modelName = pszModelName;

	HxLOG_Debug("[%s.%d] Device Info Set.. model name : %s\n",__FUNCTION__,__LINE__, pszModelName);

	deviceInfo = pstInfo;

	remoteInst->pvDSHandle = mxUserDevice_createDevice(pstInfo);

	return ERR_OK;
}

static HERROR	remoteapp_DestroyDeviceInfo(MXUSER_DEVICE_INFO_t *pstInfo)
{
	if (pstInfo == NULL)
		return ERR_FAIL;

	if (pstInfo->pUserDeviceCapability)
	{
		MXUSER_DEVICECAP    *pstUserDevCapCurrent = NULL, *pstUserDevCapNext = NULL;
		pstUserDevCapCurrent = pstInfo->pUserDeviceCapability;
		while(pstUserDevCapCurrent)
        {
            pstUserDevCapNext = pstUserDevCapCurrent->next;
            HLIB_STD_MemFree(pstUserDevCapCurrent);
            pstUserDevCapCurrent = pstUserDevCapNext;
        }
	}

	HLIB_STD_MemFree(pstInfo);

	return ERR_FAIL;
}

static HERROR	remoteapp_startUserDevice(void)
{
	MXUSER_DEVICE_INFO_t    *pstInfo = NULL;

	if (remoteapp_CreateUserDevice(pstInfo) == ERR_OK)
	{
		RemoteAppInst_t* remoteInst = remoteapp_getInstance();

		if (remoteInst == NULL)
		{
			HxLOG_Critical("[%s.%d] Can't Create remoteapp instance..\n",__FUNCTION__,__LINE__);
			return ERR_FAIL;
		}

		mxUserDevice_startDevice(remoteInst->pvDSHandle);
		remoteapp_DestroyDeviceInfo(pstInfo);
		return ERR_OK;
	}
	else
	{
		if (pstInfo)
			remoteapp_DestroyDeviceInfo(pstInfo);
		return ERR_FAIL;
	}
	return ERR_FAIL;
}
#endif

static HERROR	remoteapp_onMainApplicationReady(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	HxLOG_Trace();

#if defined(CONFIG_SUPPORT_REMOTEAPP_ON_USERDEVICE)
	if (remoteapp_startUserDevice() == ERR_OK)
	{
		HxLOG_Debug("[%s.%d] start remote app on user device..\n",__FUNCTION__,__LINE__);
	}
#endif

	return ERR_OK;
}

int	main(int argc, char **argv)
{
	//	Ignore BROKEN PIPE signal.
	signal(SIGPIPE, SIG_IGN);

#if defined(CONFIG_ENHANCED_SECURITY)
	HLIB_EXTRA_DropRootPrivileges("remoteapp");
#endif

	VK_Init();

	APK_SYSINFO_STB_Init();

	/*
	현재는 Remote RCU Server가 종료되는 일이 없는 걸로 알고 있어서, APK_NETWORK_DeInit() 함수를 따로 호출하지는 않습니다.
	하지만 추후에 APK_NETWORK_DeInit()을 호출할 일이 생길 수 있을 것 같아 주석으로 남겨놓습니다.
	*/
	APK_NETWORK_Init();


	HAPI_ConnectSignal("signal:onMainApplicationReady", NULL, remoteapp_onMainApplicationReady);

	HxLOG_Info("%s() +\n", __FUNCTION__);

	runRemoteAppReceiver();

	HxLOG_Info("%s() -\n", __FUNCTION__);

	return 0;
}

