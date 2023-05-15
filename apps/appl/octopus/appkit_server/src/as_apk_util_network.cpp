#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_util_network.h>

#define MAX_BUFFSIZE	1024

char const * const cApkUtilNetwork::BUSNAME_APK_UTIL_NETWORK = "Octopus.Appkit.Util.Network";
char const * const cApkUtilNetwork::BUSPATH_APK_UTIL_NETWORK = "/Octopus/Appkit/Util/Network";

cApkUtilNetwork * cApkUtilNetwork::pMainThis	=	NULL;

void cApkUtilNetwork::callback_Event ( HINT32 eEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	if (NULL == pMainThis)
		return;

	uint32_t param1=0, param2=0, param3=0;

	/* emit signal */
	param1 = (uint32_t)ulParam1;
	param2 = (uint32_t)ulParam2;
	param3 = (uint32_t)ulParam3;

	pMainThis->event ((uint32_t)eEvent, param1, param2, param3);	
}

cApkUtilNetwork::cApkUtilNetwork(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_UTIL_NETWORK)
{
	if (NULL == pMainThis)
		pMainThis = this;

	/* apkit util network */
    {	
		HERROR hErr = ERR_FAIL;
		
		APK_NETWORK_Init ();	
		//HBOOL APK_NETWORK_DeInit(void);
		
		APK_NETWORK_RegisterNotifier (cApkUtilNetwork::callback_Event);
		//void APK_NETWORK_UnRegisterNotifier(APKS_NETWORK_CbHandler pfnObjectHandler);
		
		hErr = APK_NETWORK_CM_Init ();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_NETWORK_CM_Init Error : %d", hErr);
		}
		
		//HERROR APK_NETWOKR_CM_DeInit(void);
		
		hErr = APK_NETWORK_DHCPSVR_Init ();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_NETWORK_DHCPSVR_Init Error : %d", hErr);
		}

		//HERROR APK_NETWORK_DHCPSVR_DeInit(void);
    }
}

void cApkUtilNetwork::BT_SetEnable(const bool& enable)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_NETWORK_BT_SetEnable ((HBOOL)enable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_CM_Init Error : %d", hErr);
	}

	return ;
}

bool cApkUtilNetwork::BT_GetEnable()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bEnable	=	FALSE;
		
	hErr = APK_NETWORK_BT_GetEnable (&bEnable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_CM_Init Error : %d", hErr);
	}

	return (bool)bEnable;
}

void cApkUtilNetwork::BT_Scan()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_NETWORK_BT_Scan ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_BT_Scan Error : %d", hErr);
	}

	return ;
}

void cApkUtilNetwork::BT_Connect(const int32_t& index)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_NETWORK_BT_Connect ((HINT32)index);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_BT_Connect Error : %d", hErr);
	}

	return ;
}

void cApkUtilNetwork::BT_ConnectByBdAddr(const std::string& bdAddress)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_NETWORK_BT_ConnectByBdAddr ((const HCHAR *)&bdAddress[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_BT_ConnectByBdAddr Error : %d", hErr);
	}

	return ;
}

void cApkUtilNetwork::BT_Disconnect(const int32_t& index)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_NETWORK_BT_Disconnect ((HINT32)index);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_BT_Disconnect Error : %d", hErr);
	}

	return ;
}

void cApkUtilNetwork::BT_DisconnectByBdAddr(const std::string& bdAddress)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_NETWORK_BT_DisconnectByBdAddr ((const HCHAR *)&bdAddress[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_BT_DisconnectByBdAddr Error : %d", hErr);
	}

	return ;
}

void cApkUtilNetwork::BT_Remove(const int32_t& index)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_NETWORK_BT_Remove ((HINT32)index);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_BT_Remove Error : %d", hErr);
	}

	return ;
}

void cApkUtilNetwork::BT_RemoveByBdAddr(const std::string& bdAddress)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_NETWORK_BT_RemoveByBdAddr ((const HCHAR *)&bdAddress[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_BT_RemoveByBdAddr Error : %d", hErr);
	}

	return ;
}

void cApkUtilNetwork::BT_SendPinCode(const int32_t& index, const std::vector< uint8_t >& pinCode)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_NETWORK_BT_SendPinCode ((HUINT32)index, (HCHAR *)&pinCode[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_BT_SendPinCode Error : %d", hErr);
	}

	return ;
}

void cApkUtilNetwork::BT_SendPinCodeByBdAddr(const std::string& bdAddress, const std::vector< uint8_t >& pinCode)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_NETWORK_BT_SendPinCodeByBdAddr ((const HCHAR *)&bdAddress[0], (HCHAR *)&pinCode[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_BT_SendPinCodeByBdAddr Error : %d", hErr);
	}

	return ;
}

uint32_t cApkUtilNetwork::BT_GetDeviceNum()
{
	return (uint32_t)APK_NETWORK_BT_GetDeviceNum ();
}

::DBus::Struct< uint32_t, std::string, uint32_t, std::string, uint32_t, std::string > cApkUtilNetwork::BT_GetDeviceInfo(const uint32_t& deviceId)
{
	::DBus::Struct< uint32_t, std::string, uint32_t, std::string, uint32_t, std::string >	reval;

	HERROR hErr = ERR_FAIL;
	APKS_BT_DEVICEINFO_t stDevInfoList;
	
	hErr = APK_NETWORK_BT_GetDeviceInfo ((HUINT32)deviceId, &stDevInfoList);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_BT_GetDeviceInfo Error : %d", hErr);
	}

	{
		reval._1	=	(uint32_t)stDevInfoList.ulDevId;
		reval._2	=	std::string(stDevInfoList.szDevName);
		reval._3	=	(uint32_t)stDevInfoList.eStatus;
		reval._4	=	std::string((char*)stDevInfoList.dev_class);
		reval._5	=	(uint32_t)stDevInfoList.eSupportedProfile;
		reval._6	=	std::string(stDevInfoList.aucBdAddr);
	}

	return reval;
}

std::string cApkUtilNetwork::BT_GetMacAddrInfo()
{
	HERROR hErr = ERR_FAIL;
	HCHAR szBuff[MAX_BUFFSIZE] = {0,};
	hErr = APK_NETWORK_BT_GetMacAddrInfo (szBuff);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_BT_GetMacAddrInfo Error : %d", hErr);
	}

	return std::string(szBuff);
}

uint32_t cApkUtilNetwork::BT_GetMacAddrInfoSize()
{
	return (uint32_t)APK_NETWORK_BT_GetMacAddrInfoSize ();
}

void cApkUtilNetwork::BT_GetDevInfoList(const uint32_t& listSize, uint32_t& reval, std::vector< ::DBus::Struct< uint32_t, std::string, uint32_t, std::string, uint32_t, std::string > >& deviceInfoList)
{
	APKS_BT_DEVICEINFO_t *pstDevInfoList = (APKS_BT_DEVICEINFO_t *)HLIB_STD_MemAlloc (listSize);
	if (NULL == pstDevInfoList)
		return ;

	reval = (uint32_t)APK_NETWORK_BT_GetDevInfoList ((HUINT32)listSize, pstDevInfoList);

	{
		HINT32 i;
		for (i = 0; i <(HINT32)reval; i++)
		{
			::DBus::Struct< uint32_t, std::string, uint32_t, std::string, uint32_t, std::string > item;

			{
				item._1	=	(uint32_t)pstDevInfoList[i].ulDevId;
				item._2	=	std::string(pstDevInfoList[i].szDevName);
				item._3	=	(uint32_t)pstDevInfoList[i].eStatus;
				item._4	=	std::string((HCHAR*)pstDevInfoList[i].dev_class);
				item._5	=	(uint32_t)pstDevInfoList[i].eSupportedProfile;
				item._6	=	std::string(pstDevInfoList[i].aucBdAddr);
			}
	
			deviceInfoList.push_back (item);
		}
	}

	HLIB_STD_MemFree (pstDevInfoList);

	return ;
}

uint32_t cApkUtilNetwork::CM_GetStatus()
{
	HERROR hErr = ERR_FAIL;

	APKE_NETCM_Status_e eStatus	=	eNETCM_NONE;
		
	hErr = APK_NETWORK_CM_GetStatus (&eStatus);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_CM_GetStatus Error : %d", hErr);
	}

	return (uint32_t)eStatus;
}

int32_t cApkUtilNetwork::CM_GetDownStreamInfo(const uint32_t& downInfo)
{
	HERROR hErr = ERR_FAIL;

	HINT32 nValue	=	0;
		
	hErr = APK_NETWORK_CM_GetDownStreamInfo ((APKE_NETCM_DownInfo_e)downInfo, &nValue);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_CM_GetDownStreamInfo Error : %d", hErr);
	}

	return (int32_t)nValue;
}

void cApkUtilNetwork::CM_SetDownStreamInfo(const uint32_t& downInfo, const int32_t& value)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_NETWORK_CM_SetDownStreamInfo ((APKE_NETCM_DownInfo_e)downInfo, (HINT32)value);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_CM_SetDownStreamInfo Error : %d", hErr);
	}

	return ;
}

int32_t cApkUtilNetwork::CM_GetUpStreamInfo(const uint32_t& upInfo)
{
	HERROR hErr = ERR_FAIL;

	HINT32 nValue	=	0;

	hErr = APK_NETWORK_CM_GetUpStreamInfo ((APKE_NETCM_UpInfo_e)upInfo, (HINT32 *)&nValue);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_CM_GetUpStreamInfo Error : %d", hErr);
	}

	return (int32_t)nValue;
}

void cApkUtilNetwork::DHCPSVR_UpdateInfo()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_NETWORK_DHCPSVR_UpdateInfo ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_DHCPSVR_UpdateInfo Error : %d", hErr);
	}

	return ;
}

bool cApkUtilNetwork::DHCPSVR_IsRunning()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bRunnig	=	FALSE;
		
	hErr = APK_NETWORK_DHCPSVR_IsRunning (&bRunnig);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_DHCPSVR_IsRunning Error : %d", hErr);
	}

	return (bool)bRunnig;
}

int32_t cApkUtilNetwork::DHCPSVR_GetDevIndex()
{
	HERROR hErr = ERR_FAIL;

	HINT32 nDevIndex	=	0;
		
	hErr = APK_NETWORK_DHCPSVR_GetDevIndex (&nDevIndex);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_DHCPSVR_GetDevIndex Error : %d", hErr);
	}

	return (int32_t)nDevIndex;
}

void cApkUtilNetwork::DHCPSVR_Start(const int32_t& index)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_NETWORK_DHCPSVR_Start ((HINT32)index);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_DHCPSVR_Start Error : %d", hErr);
	}

	return ;
}

void cApkUtilNetwork::DHCPSVR_Stop(const int32_t& index)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_NETWORK_DHCPSVR_Stop ((HINT32)index);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_DHCPSVR_Stop Error : %d", hErr);
	}

	return ;
}

int32_t cApkUtilNetwork::CONF_GetNumOfNetDev()
{
	HERROR hErr = ERR_FAIL;

	HINT32 nCount	=	0;
		
	hErr = APK_NETWORK_CONF_GetNumOfNetDev (&nCount);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_CONF_GetNumOfNetDev Error : %d", hErr);
	}

	return (int32_t)nCount;
}

int32_t cApkUtilNetwork::CONF_GetDefaultDev()
{
	HERROR hErr = ERR_FAIL;

	HINT32 nIndex	=	0;
		
	hErr = APK_NETWORK_CONF_GetDefaultDev (&nIndex);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_CONF_GetDefaultDev Error : %d", hErr);
	}

	return (int32_t)nIndex;
}

::DBus::Struct< uint32_t, bool, bool, bool, std::string, std::string > cApkUtilNetwork::CONF_GetDevInfo(const int32_t& index)
{
	::DBus::Struct< uint32_t, bool, bool, bool, std::string, std::string > reval;

	HERROR hErr = ERR_FAIL;
	APKS_NETDEVINFO_t stDevInfo;

	hErr = APK_NETWORK_CONF_GetDevInfo ((HINT32)index, &stDevInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_CONF_GetDevInfo Error : %d", hErr);
	}

	{
		reval._1	=	(uint32_t)stDevInfo.eType;
		reval._2	=	(bool)stDevInfo.bConnected;
		reval._3	=	(bool)stDevInfo.bNetActive;
		reval._4	=	(bool)stDevInfo.bRunning;
		reval._5	=	std::string(stDevInfo.szMacAddr);
		reval._6	=	std::string(stDevInfo.szDev_name); 
	}

	return reval;
}

std::string cApkUtilNetwork::CONF_GetMacAddr(const int32_t& index)
{
	HERROR hErr = ERR_FAIL;
	HCHAR szBuff[MAX_BUFFSIZE] = {0,};

	hErr = APK_NETWORK_CONF_GetMacAddr ((HINT32)index, szBuff, MAX_BUFFSIZE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_CONF_GetMacAddr Error : %d", hErr);
	}

	return std::string(szBuff);
}

::DBus::Struct< ::DBus::Struct< bool, bool >, ::DBus::Struct< int64_t, int64_t, int64_t, int64_t, int64_t >, ::DBus::Struct< bool, bool, uint32_t, bool, std::string, std::string, std::string, std::string, uint32_t, int64_t, int64_t > > cApkUtilNetwork::CONF_GetAddrInfo(const int32_t& index)
{
	::DBus::Struct< ::DBus::Struct< bool, bool >, ::DBus::Struct< int64_t, int64_t, int64_t, int64_t, int64_t >, ::DBus::Struct< bool, bool, uint32_t, bool, std::string, std::string, std::string, std::string, uint32_t, int64_t, int64_t > > reval;

	HERROR hErr = ERR_FAIL;
	APKS_NETIFADDR_t stNetAddr;
	
	hErr = APK_NETWORK_CONF_GetAddrInfo ((HINT32)index, &stNetAddr);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_CONF_GetAddrInfo Error : %d", hErr);
	}

	{
		reval._1._1	=	(bool)stNetAddr.bDhcpOn;
		reval._1._2	=	(bool)stNetAddr.bDnsAuto;

		reval._2._1	=	(int64_t)stNetAddr.ulIpAddr;
		reval._2._2	=	(int64_t)stNetAddr.ulNetmask;
		reval._2._3	=	(int64_t)stNetAddr.ulGateway;
		reval._2._4	=	(int64_t)stNetAddr.ulDns1;
		reval._2._5	=	(int64_t)stNetAddr.ulDns2;

		reval._3._1		=	(bool)stNetAddr.bIpv6;
		reval._3._2		=	(bool)stNetAddr.bEnable;
		reval._3._3		=	(uint32_t)stNetAddr.bIpv6Mode;
		reval._3._4		=	(bool)stNetAddr.bDns6;
		reval._3._5		=	std::string(stNetAddr.szIp6_addr);
		reval._3._6		=	std::string(stNetAddr.szIp6_gateway);
		reval._3._7		=	std::string(stNetAddr.szIp6_dns_addr_1);
		reval._3._8		=	std::string(stNetAddr.szIp6_dns_addr_2);
		reval._3._9		=	(uint32_t)stNetAddr.nPrefix;
		reval._3._10	=	(int64_t)stNetAddr.ulTunnelStartAddr;
		reval._3._11	=	(int64_t)stNetAddr.ulTunnelEndAddr; 
	}

	return reval;
}

void cApkUtilNetwork::CONF_SetAddrInfo(const int32_t& index, const ::DBus::Struct< ::DBus::Struct< bool, bool >, ::DBus::Struct< int64_t, int64_t, int64_t, int64_t, int64_t >, ::DBus::Struct< bool, bool, uint32_t, bool, std::string, std::string, std::string, std::string, uint32_t, int64_t, int64_t > >& netAddress)
{
	HERROR hErr = ERR_FAIL;
	APKS_NETIFADDR_t stNetAddr;

	HxSTD_memset (&stNetAddr, 0, sizeof(APKS_NETIFADDR_t));
	{
		stNetAddr.bDhcpOn	=	(HBOOL)netAddress._1._1;
		stNetAddr.bDnsAuto	=	(HBOOL)netAddress._1._2;

		stNetAddr.ulIpAddr		=	(HULONG)netAddress._2._1;
		stNetAddr.ulNetmask		=	(HULONG)netAddress._2._2;
		stNetAddr.ulGateway		=	(HULONG)netAddress._2._3;
		stNetAddr.ulDns1		=	(HULONG)netAddress._2._4;
		stNetAddr.ulDns2		=	(HULONG)netAddress._2._5;

		stNetAddr.bIpv6						=	(HBOOL)netAddress._3._1;
		stNetAddr.bEnable					=	(HBOOL)netAddress._3._2;
		stNetAddr.bIpv6Mode					=	(HUINT32)netAddress._3._3;
		stNetAddr.bDns6						=	(HBOOL)netAddress._3._4;
		HxSTD_StrNCpy(stNetAddr.szIp6_addr, &netAddress._3._5[0], APKD_NETWORK_MAX_IP6ADDR);
		HxSTD_StrNCpy(stNetAddr.szIp6_gateway, &netAddress._3._6[0], APKD_NETWORK_MAX_IP6ADDR);
		HxSTD_StrNCpy(stNetAddr.szIp6_dns_addr_1, &netAddress._3._7[0], APKD_NETWORK_MAX_IP6ADDR);
		HxSTD_StrNCpy(stNetAddr.szIp6_dns_addr_2, &netAddress._3._8[0], APKD_NETWORK_MAX_IP6ADDR);
		stNetAddr.nPrefix					=	(HUINT32)netAddress._3._9;
		stNetAddr.ulTunnelStartAddr			=	(HULONG)netAddress._3._10;
		stNetAddr.ulTunnelEndAddr			=	(HULONG)netAddress._3._11; 
	}
	
	hErr = APK_NETWORK_CONF_SetAddrInfo ((HINT32)index, &stNetAddr);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_CONF_SetAddrInfo Error : %d", hErr);
	}

	return ;
}

void cApkUtilNetwork::CONF_StopConnecting(const int32_t& index)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_NETWORK_CONF_StopConnecting ((HINT32)index );
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_CONF_StopConnecting Error : %d", hErr);
	}

	return ;
}

void cApkUtilNetwork::CONF_SaveEnableStatus(const int32_t& index, const bool& enable)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_NETWORK_CONF_SaveEnableStatus ((HINT32)index, (HBOOL)enable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_CONF_SaveEnableStatus Error : %d", hErr);
	}

	return ;
}

std::string cApkUtilNetwork::CONF_AddrN2A(const uint64_t& address)
{
	HERROR hErr = ERR_FAIL;
	HCHAR szBuff[MAX_BUFFSIZE] = {0,};

	hErr = APK_NETWORK_CONF_AddrN2A ((HULONG)address, szBuff, MAX_BUFFSIZE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_CONF_AddrN2A Error : %d", hErr);
	}

	return std::string(szBuff);
}

uint64_t cApkUtilNetwork::CONF_AddrA2N(const std::string& addressString)
{
	HERROR hErr = ERR_FAIL;

	HULONG ulAddr;
		
	hErr = APK_NETWORK_CONF_AddrA2N ((HCHAR *)&addressString[0], &ulAddr);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_CONF_AddrA2N Error : %d", hErr);
	}

	return (uint64_t)ulAddr;
}

bool cApkUtilNetwork::CONF_IsSupportNetwork()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bIsSupport	=	FALSE;
		
	hErr = APK_NETWORK_CONF_IsSupportNetwork (&bIsSupport);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_CONF_IsSupportNetwork Error : %d", hErr);
	}

	return (bool)bIsSupport;
}

bool cApkUtilNetwork::CONF_IsNetworkAvailable()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bIsEnable	=	FALSE;

	hErr = APK_NETWORK_CONF_IsNetworkAvailable (&bIsEnable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_CONF_IsNetworkAvailable Error : %d", hErr);
	}

	return (bool)bIsEnable;
}

void cApkUtilNetwork::WIFI_StartScan(const int32_t& index)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_NETWORK_WIFI_StartScan ((HINT32)index);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_WIFI_StartScan Error : %d", hErr);
	}

	return ;
}

void cApkUtilNetwork::WIFI_ConnectAP(const int32_t& index, const ::DBus::Struct< std::string, std::string, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t >& apInfo, const std::string& key)
{
	HERROR hErr = ERR_FAIL;

	APKS_NETAP_Info_t stApInfo;

	HxSTD_memset (&stApInfo, 0, sizeof(APKS_NETAP_Info_t));
	{
		HxSTD_StrNCpy(stApInfo.szESSID, &apInfo._1[0], 48);
		HxSTD_StrNCpy(stApInfo.szMacAddr, &apInfo._2[0], APKD_NETWORK_MAX_MACADDR);
		stApInfo.nOpMode			=	(HINT32)apInfo._3;
		stApInfo.nSecureType		=	(HINT32)apInfo._4;
		stApInfo.nAuthenType		=	(HINT32)apInfo._5;
		stApInfo.nEncryptType	=	(HINT32)apInfo._6;
		stApInfo.nWaveStrength	=	(HINT32)apInfo._7;
		stApInfo.nConnectSpeed	=	(HINT32)apInfo._8;
		stApInfo.bIsConnected	=	(HINT32)apInfo._9;
	}
		
	hErr = APK_NETWORK_WIFI_ConnectAP ((HINT32)index, &stApInfo, (HCHAR *)&key[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_WIFI_ConnectAP Error : %d", hErr);
	}

	return ;
}

void cApkUtilNetwork::WIFI_ConnectAPByWPS(const int32_t& index, const uint32_t& wpsType, const std::string& pinCode)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_NETWORK_WIFI_ConnectAPByWPS ((HINT32)index, (APKE_NETAP_WPS_Type_e)wpsType, (HCHAR *)&pinCode[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_WIFI_ConnectAPByWPS Error : %d", hErr);
	}
	
	return ;
}

void cApkUtilNetwork::WIFI_DisconnectAP(const int32_t& index)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_NETWORK_WIFI_DisconnectAP ((HINT32)index);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_WIFI_DisconnectAP Error : %d", hErr);
	}

	return ;
}

void cApkUtilNetwork::WIFI_GetConnectedInfo(const int32_t& index, ::DBus::Struct< std::string, std::string, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t >& apInfo, std::string& key)
{
	HERROR hErr = ERR_FAIL;
	HCHAR szBuff[MAX_BUFFSIZE] = {0,};
	APKS_NETAP_Info_t stApInfo;

	HxSTD_memset (&stApInfo, 0, sizeof(APKS_NETAP_Info_t));
	hErr = APK_NETWORK_WIFI_GetConnectedInfo ((HINT32)index, &stApInfo, szBuff, MAX_BUFFSIZE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_NETWORK_WIFI_GetConnectedInfo Error : %d", hErr);
	}

	{
		apInfo._1	=	std::string(stApInfo.szESSID);
		apInfo._2	=	std::string(stApInfo.szMacAddr);
		apInfo._3	=	(int32_t)stApInfo.nOpMode	;
		apInfo._4	=	(int32_t)stApInfo.nSecureType;
		apInfo._5	=	(int32_t)stApInfo.nAuthenType;
		apInfo._6	=	(int32_t)stApInfo.nEncryptType;
		apInfo._7	=	(int32_t)stApInfo.nWaveStrength;
		apInfo._8	=	(int32_t)stApInfo.nConnectSpeed;
		apInfo._9	=	(int32_t)stApInfo.bIsConnected;
	}

	return ;
}

#if 0
HERROR APK_NETWORK_PPPOE_Connect(APKS_PPPOE_Info_t *pstPPPoEInfo);
HERROR APK_NETWORK_PPPOE_Disconnect(HINT32 nIndex);
HERROR APK_NETWORK_PPPOE_GetInfo(APKS_PPPOE_Info_t *pstPPPOEInfo);
HERROR APK_NETWORK_PPPOE_SetInfo(APKS_PPPOE_Info_t *pstPPPoEInfo);
HERROR APK_NETWORK_PPPOE_GetOnOff(HINT32 nIndex, HBOOL *bOn);
HERROR APK_NETWORK_PPPOE_SetOnOff(HINT32 nIndex, HBOOL bOn);

HINT32 APK_NETWORK_CONCHK_Gateway( HINT32 nIndex, APKS_NETWORK_CbHandler pfnCallback, HINT32 nTimeout );
HINT32 APK_NETWORK_CONCHK_Dns( HINT32 nIndex, APKS_NETWORK_CbHandler pfnCallback, HINT32 nTimeout );
HINT32 APK_NETWORK_CONCHK_Arp( HINT32 nIndex, HCHAR *pszAddr, APKS_NETWORK_CbHandler pfnCallback, HINT32 nTimeout );
HINT32 APK_NETWORK_CONCHK_ResolvDomain( HINT32 nIndex, HCHAR *pszDomain, APKS_NETWORK_CbHandler pfnCallback, HINT32 nTimeout );
#endif
