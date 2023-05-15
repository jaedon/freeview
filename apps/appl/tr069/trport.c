/**
	@file     trport.c
	@brief    trport.c CWMP library porting

	Description: 		\n
	Module: 			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <wait.h>
#include <hlib.h>
#include <apk.h>
#include <hapi.h>
#include "trport_common.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define TRPORT_DATAMODEL_ROOT_OBJECT 		"Device"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static HxJSON_t jsonroot = NULL;
static trport_download_info_t stDownInfo[eDOWNLOAD_TYPE_MAX];

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define __________STATIC_FUNCTION________________________________________
#define __________STATIC_FUNCTION_GET_DATA_______________________________________
static HERROR get_data_Device_DeviceSummary(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = TRP_DB_getData_Device_DeviceSummary(pszOutput);

	TRLEAVE(hErr);
}

static HERROR get_data_DeviceInfo_X_JCOM_SubscriberID(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = TRP_DB_getData_DeviceInfo_X_JCOM_SubscriberID(pszOutput);

	TRLEAVE(hErr);
}

static HERROR get_data_DeviceInfo_X_JCOM_ServiceID(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = TRP_DB_getData_DeviceInfo_X_JCOM_SubscriberID(pszOutput);

	TRLEAVE(hErr);
}

static HERROR get_data_DeviceInfo_X_JCOM_CorpCode(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = TRP_DB_getData_DeviceInfo_X_JCOM_SubscriberID(pszOutput);

	TRLEAVE(hErr);
}

static HERROR get_data_DeviceInfo_Manufacturer(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = APK_SQCBOXINFO_GetItem(eBoxInfo_Manu_Facturer, pszOutput);

	TRLEAVE(hErr);
}


static HERROR get_data_DeviceInfo_ManufacturerOUI(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	//hErr = TRP_DB_getData_DeviceInfo_ManufacturerOUI(pszOutput);
	HLIB_STD_StrUtf8NCpy(pszOutput, TRPORT_HUMAX_OUI, 32);

	TRLEAVE(hErr);
}

static HERROR get_data_DeviceInfo_ModelName(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = APK_SQCBOXINFO_GetItem(eBoxInfo_Model_Name, pszOutput);

	TRLEAVE(hErr);
}

static HERROR get_data_DeviceInfo_Description(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = TRP_DB_getData_DeviceInfo_Description(pszOutput);

	TRLEAVE(hErr);
}

static HERROR get_data_DeviceInfo_ProductClass(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = TRP_DB_getData_DeviceInfo_ProductClass(pszOutput);

	TRLEAVE(hErr);
}

static HERROR get_data_DeviceInfo_SerialNumber(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = APK_SQCBOXINFO_GetItem(eBoxInfo_Serial_No, pszOutput);
	//HLIB_STD_StrUtf8NCpy(pszOutput, "2222", 32);
	TRLEAVE(hErr);
}

static HERROR get_data_DeviceInfo_HardwareVersion(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = TRP_DB_getData_DeviceInfo_HardwareVersion(pszOutput);
	TRLEAVE(hErr);
}

static HERROR get_data_DeviceInfo_SoftwareVersion(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HINT32 i = 0, j = 0;
	HINT32 len = 0;
	HCHAR	pszOutputBuff[32];
	HCHAR	szNewVersion[32];
	pszOutputBuff[0]='\0';
	szNewVersion[0]='\0';

	CHECK_NULL(pszOutput);
	/*To Do : Remove pre-fix of version name*/

	//hErr = APK_SQCBOXINFO_GetItem(eBoxInfo_Software_Version, pszOutput);
	hErr = APK_SQCBOXINFO_GetItem(eBoxInfo_Software_Version, pszOutputBuff);
	len = HxSTD_StrLen(pszOutputBuff);
	HxLOG_Debug("Real Software version(%s), len(%d)\r\n",pszOutputBuff,len);
	for(i = (TRPORT_SOFTWARE_VERSION_PREFIX+1); i < len; i++)
	{
		szNewVersion[j++] = pszOutputBuff[i];
		szNewVersion[(len-TRPORT_SOFTWARE_VERSION_PREFIX)] = '\0';
	}

	HxSTD_StrCpy(pszOutput, szNewVersion);

	HxLOG_Debug("New Software version(%s/%s), len(%d)\r\n",szNewVersion,pszOutput,HxSTD_StrLen(szNewVersion));


	//HxSTD_StrCpy(pszOutput, "1.0");

	TRLEAVE(hErr);
}

static HERROR get_data_DeviceInfo_DeviceStatus(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
//STB의 상태
/*
Up
Initializing
Error
Disabled
*/
	HLIB_STD_StrUtf8NCpy(pszOutput, "Up", 32);

	TRLEAVE(hErr);
}

static HERROR get_data_DeviceInfo_UpTime(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
	HUINT32 ultic;

	CHECK_NULL(pszOutput);

	//세트의 부팅 시간을 구함. (tr069 프로세스의 동작시간)
	ultic = VK_TIMER_GetSystemTick ();
	//HxLOG_Debug(" ultic:%d, currTime:%d \n",ultic, ultic/1000);
	snprintf(pszOutput, 10, "%d", ultic/1000);


	TRLEAVE(hErr);
}

static HERROR get_data_ManagementServer_URL(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = APK_SQCSCENARIOJAPAN_GetAcsURL(pszOutput, 257);

	TRLEAVE(hErr);
}

static HERROR get_data_ManagementServer_Username(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = TRP_DB_getData_ManagementServer_Username(pszOutput);

	TRLEAVE(hErr);
}

static HERROR get_data_ManagementServer_Password(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = TRP_DB_getData_ManagementServer_Password(pszOutput);

	TRLEAVE(hErr);
}

static HERROR get_data_ManagementServer_PeriodicInformEnable(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = TRP_DB_getData_ManagementServer_PeriodicInformEnable(pszOutput);

	TRLEAVE(hErr);
}

static HERROR get_data_ManagementServer_PeriodicInformInterval(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = TRP_DB_getData_ManagementServer_PeriodicInformInterval(pszOutput);

	TRLEAVE(hErr);
}

static HERROR get_data_ManagementServer_PeriodicInformTime(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = TRP_DB_getData_ManagementServer_PeriodicInformTime(pszOutput);

	TRLEAVE(hErr);
}


static HERROR get_data_ManagementServer_ParameterKey(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = TRP_DB_getData_ManagementServer_ParameterKey(pszOutput);

	TRLEAVE(hErr);
}

static HERROR get_data_ManagementServer_ConnectionRequestURL(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;
int 	nIndex;
	APKS_NETIFADDR_t	ptNetAddr;
	CHECK_NULL(pszOutput);



	if (APK_NETWORK_CONF_GetDefaultDev(&nIndex) != ERR_OK)
	{
		HxLOG_Error("Getting the default of Network Interfaces is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	if (APK_NETWORK_CONF_GetAddrInfo(nIndex, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		HxLOG_Error("Getting the Address Information of Network is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	if (APK_NETWORK_CONF_AddrN2A(ptNetAddr.ulIpAddr, pszOutput, TRPORT_IPADDR_LEN) != ERR_OK)
	{
		HxLOG_Error("Getting the IP Address is failed.\n");
		TRLEAVE(ERR_FAIL);
	}


	//hErr = TRP_DB_getData_ManagementServer_ConnectionRequestURL(pszOutput);

	TRLEAVE(hErr);
}

static HERROR get_data_ManagementServer_ConnectionRequestUsername(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = TRP_DB_getData_ManagementServer_ConnectionRequestUsername(pszOutput);

	TRLEAVE(hErr);
}

static HERROR get_data_ManagementServer_ConnectionRequestPassword(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = TRP_DB_getData_ManagementServer_ConnectionRequestPassword(pszOutput);

	TRLEAVE(hErr);
}

static HERROR get_data_ManagementServer_UpgradesManaged(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = TRP_DB_getData_ManagementServer_UpgradesManaged(pszOutput);

	TRLEAVE(hErr);
}

static HERROR get_data_LAN_AddressingType(HCHAR *pszOutput)
{
	int 	nIndex;
	APKS_NETIFADDR_t	ptNetAddr;

	CHECK_NULL(pszOutput);

	if (APK_NETWORK_CONF_GetDefaultDev(&nIndex) != ERR_OK)
	{
		HxLOG_Error("Getting the default of Network Interfaces is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	if (APK_NETWORK_CONF_GetAddrInfo(nIndex, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		HxLOG_Error("Getting the Address Information of Network is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	if(ptNetAddr.bDhcpOn)
		HLIB_STD_StrUtf8NCpy(pszOutput, "DHCP", 256);
	else
		HLIB_STD_StrUtf8NCpy(pszOutput, "Static", 256);

	TRLEAVE(ERR_OK);
}

static HERROR get_data_LAN_IPAddress(HCHAR *pszOutput)
{
	int 	nIndex;
	APKS_NETIFADDR_t	ptNetAddr;

	CHECK_NULL(pszOutput);

	if (APK_NETWORK_CONF_GetDefaultDev(&nIndex) != ERR_OK)
	{
		HxLOG_Error("Getting the default of Network Interfaces is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	if (APK_NETWORK_CONF_GetAddrInfo(nIndex, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		HxLOG_Error("Getting the Address Information of Network is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	if (APK_NETWORK_CONF_AddrN2A(ptNetAddr.ulIpAddr, pszOutput, TRPORT_IPADDR_LEN) != ERR_OK)
	{
		HxLOG_Error("Getting the IP Address is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	//HLIB_STD_StrUtf8NCpy(pszOutput, "10.0.11.163", 256);
	TRLEAVE(ERR_OK);
}

static HERROR get_data_LAN_SubnetMask(HCHAR *pszOutput)
{
	int 	nIndex;
	APKS_NETIFADDR_t	ptNetAddr;

	CHECK_NULL(pszOutput);

	if (APK_NETWORK_CONF_GetDefaultDev(&nIndex) != ERR_OK)
	{
		HxLOG_Error("Getting the default of Network Interfaces is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	if (APK_NETWORK_CONF_GetAddrInfo(nIndex, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		HxLOG_Error("Getting the Address Information of Network is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	if (APK_NETWORK_CONF_AddrN2A(ptNetAddr.ulNetmask, pszOutput, TRPORT_IPADDR_LEN) != ERR_OK)
	{
		HxLOG_Error("Getting the Netmask is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	//HLIB_STD_StrUtf8NCpy(pszOutput, "255.255.255.0", 256);
	TRLEAVE(ERR_OK);
}

static HERROR get_data_LAN_DefaultGateway(HCHAR *pszOutput)
{
	int 	nIndex;
	APKS_NETIFADDR_t	ptNetAddr;

	CHECK_NULL(pszOutput);


	if (APK_NETWORK_CONF_GetDefaultDev(&nIndex) != ERR_OK)
	{
		HxLOG_Error("Getting the default of Network Interfaces is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	if (APK_NETWORK_CONF_GetAddrInfo(nIndex, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		HxLOG_Error("Getting the Address Information of Network is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	if (APK_NETWORK_CONF_AddrN2A(ptNetAddr.ulGateway, pszOutput, TRPORT_IPADDR_LEN) != ERR_OK)
	{
		HxLOG_Error("Getting the Gateway is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	//HLIB_STD_StrUtf8NCpy(pszOutput, "255.255.255.0", 256);
	TRLEAVE(ERR_OK);
}

static HERROR get_data_LAN_DNSServers(HCHAR *pszOutput)
{
	int 	nIndex;
	char	tmpAddr1[TRPORT_IPADDR_LEN], tmpAddr2[TRPORT_IPADDR_LEN];
	APKS_NETIFADDR_t	ptNetAddr;

	CHECK_NULL(pszOutput);


	if (APK_NETWORK_CONF_GetDefaultDev(&nIndex) != ERR_OK)
	{
		HxLOG_Error("Getting the default of Network Interfaces is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	if (APK_NETWORK_CONF_GetAddrInfo(nIndex, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		HxLOG_Error("Getting the Address Information of Network is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	if (APK_NETWORK_CONF_AddrN2A(ptNetAddr.ulDns1, tmpAddr1, TRPORT_IPADDR_LEN) != ERR_OK)
	{
		HxLOG_Error("Getting the DNS1 is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	if (APK_NETWORK_CONF_AddrN2A(ptNetAddr.ulDns1, tmpAddr2, TRPORT_IPADDR_LEN) != ERR_OK)
	{
		HxLOG_Error("Getting the DNS2 is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	snprintf(pszOutput, TRPORT_IPADDR_LEN*2+2, "%s, %s", tmpAddr1, tmpAddr2);

	//HLIB_STD_StrUtf8NCpy(pszOutput, "255.255.255.0", 256);
	TRLEAVE(ERR_OK);
}

static HERROR get_data_LAN_MACAddress(HCHAR *pszOutput)
{
	int 	nIndex;

	CHECK_NULL(pszOutput);

	if (APK_NETWORK_CONF_GetDefaultDev(&nIndex) != ERR_OK)
	{
		HxLOG_Error("Getting the default of Network Interfaces is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	if (APK_NETWORK_CONF_GetMacAddr(nIndex, pszOutput, TRPORT_MACADDR_LEN) != ERR_OK)
	{
		HxLOG_Error("Getting the Mac Address is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	TRLEAVE(ERR_OK);
}

static HERROR get_data_AudienceMeasurement_CombinedAccess(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = TRP_DB_getData_AudienceMeasurement_CombinedAccess(pszOutput);

	TRLEAVE(hErr);
}

static HERROR get_data_AudienceMeasurement_PeriodicInformationInterval(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = TRP_DB_getData_AudienceMeasurement_PeriodicInformationInterval(pszOutput);

	TRLEAVE(hErr);
}

static HERROR get_data_AudienceMeasurement_PeriodicInformationTime(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = TRP_DB_getData_AudienceMeasurement_PeriodicInformationTime(pszOutput);

	TRLEAVE(hErr);
}

static HERROR get_data_AudienceMeasurement_PermissionLevel(HCHAR *pszOutput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszOutput);
	hErr = TRP_DB_getData_AudienceMeasurement_PermissionLevel(pszOutput);

	TRLEAVE(hErr);
}



#define __________STATIC_FUNCTION_SET_DATA_______________________________________

static HERROR set_data_DeviceInfo_X_JCOM_SubscriberID(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = TRP_DB_setData_DeviceInfo_X_JCOM_SubscriberID(pszInput);

	TRLEAVE(hErr);
}

static HERROR set_data_DeviceInfo_X_JCOM_ServiceID(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = TRP_DB_setData_DeviceInfo_X_JCOM_ServiceID(pszInput);

	TRLEAVE(hErr);
}

static HERROR set_data_DeviceInfo_X_JCOM_CorpCode(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = TRP_DB_setData_DeviceInfo_X_JCOM_CorpCode(pszInput);

	TRLEAVE(hErr);
}

static HERROR set_data_ManagementServer_URL(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = APK_SQCSCENARIOJAPAN_SetAcsURL(pszInput);

	TRLEAVE(hErr);
}

static HERROR set_data_ManagementServer_Username(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = TRP_DB_setData_ManagementServer_Username(pszInput);

	TRLEAVE(hErr);
}

static HERROR set_data_ManagementServer_Password(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = TRP_DB_setData_ManagementServer_Password(pszInput);

	TRLEAVE(hErr);
}

static HERROR set_data_ManagementServer_PeriodicInformEnable(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = TRP_DB_setData_ManagementServer_PeriodicInformEnable(pszInput);

	TRLEAVE(hErr);
}

static HERROR set_data_ManagementServer_PeriodicInformInterval(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = TRP_DB_setData_ManagementServer_PeriodicInformInterval(pszInput);

	TRLEAVE(hErr);
}

static HERROR set_data_ManagementServer_PeriodicInformTime(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = TRP_DB_setData_ManagementServer_PeriodicInformTime(pszInput);

	TRLEAVE(hErr);
}

static HERROR set_data_ManagementServer_ConnectionRequestUsername(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = TRP_DB_setData_ManagementServer_ConnectionRequestUsername(pszInput);

	TRLEAVE(hErr);
}

static HERROR set_data_ManagementServer_ConnectionRequestPassword(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = TRP_DB_setData_ManagementServer_ConnectionRequestPassword(pszInput);

	TRLEAVE(hErr);
}

static HERROR set_data_ManagementServer_UpgradesManaged(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = TRP_DB_setData_ManagementServer_UpgradesManaged(pszInput);

	TRLEAVE(hErr);
}

static HERROR set_data_LAN_AddressingType(HCHAR *pszInput)
{
	int 	nIndex;
	APKS_NETIFADDR_t	ptNetAddr;

	CHECK_NULL(pszInput);

	if (APK_NETWORK_CONF_GetDefaultDev(&nIndex) != ERR_OK)
	{
		HxLOG_Error("Getting the default of Network Interfaces is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	if (APK_NETWORK_CONF_GetAddrInfo(nIndex, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		HxLOG_Error("Getting the Address Information of Network is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	if(strcmp(pszInput, "DHCP") == 0)
	{
		ptNetAddr.bDhcpOn = TRUE;
	}
	else if(strcmp(pszInput, "Static") == 0)
	{
		ptNetAddr.bDhcpOn = FALSE;
	}
	else
	{
		HxLOG_Error("wrong input! %s is not supported![%s:%d]\n", pszInput, __FUNCTION__, __LINE__);
		TRLEAVE(ERR_FAIL);
	}

	if (APK_NETWORK_CONF_SetAddrInfo( nIndex, &ptNetAddr ) != ERR_OK)
	{
		HxLOG_Error("Setting the Address Information of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
		TRLEAVE(ERR_FAIL);
	}

	TRLEAVE(ERR_OK);
}

static HERROR set_data_LAN_IPAddress(HCHAR *pszInput)
{
	int 	nIndex;
	APKS_NETIFADDR_t	ptNetAddr;

	CHECK_NULL(pszInput);

	if (APK_NETWORK_CONF_GetDefaultDev(&nIndex) != ERR_OK)
	{
		HxLOG_Error("Getting the default of Network Interfaces is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	if (APK_NETWORK_CONF_GetAddrInfo(nIndex, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		HxLOG_Error("Getting the Address Information of Network is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	APK_NETWORK_CONF_AddrA2N( pszInput, &ptNetAddr.ulIpAddr );

	if (APK_NETWORK_CONF_SetAddrInfo( nIndex, &ptNetAddr ) != ERR_OK)
	{
		HxLOG_Error("Setting the Address Information of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
		TRLEAVE(ERR_FAIL);
	}

	TRLEAVE(ERR_OK);
}

static HERROR set_data_LAN_SubnetMask(HCHAR *pszInput)
{
	int 	nIndex;
	APKS_NETIFADDR_t	ptNetAddr;

	CHECK_NULL(pszInput);

	if (APK_NETWORK_CONF_GetDefaultDev(&nIndex) != ERR_OK)
	{
		HxLOG_Error("Getting the default of Network Interfaces is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	if (APK_NETWORK_CONF_GetAddrInfo(nIndex, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		HxLOG_Error("Getting the Address Information of Network is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	APK_NETWORK_CONF_AddrA2N( pszInput, &ptNetAddr.ulNetmask );

	if (APK_NETWORK_CONF_SetAddrInfo( nIndex, &ptNetAddr ) != ERR_OK)
	{
		HxLOG_Error("Setting the Address Information of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
		TRLEAVE(ERR_FAIL);
	}

	TRLEAVE(ERR_OK);
}

static HERROR set_data_LAN_DefaultGateway(HCHAR *pszInput)
{
	int 	nIndex;
	APKS_NETIFADDR_t	ptNetAddr;

	CHECK_NULL(pszInput);


	if (APK_NETWORK_CONF_GetDefaultDev(&nIndex) != ERR_OK)
	{
		HxLOG_Error("Getting the default of Network Interfaces is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	if (APK_NETWORK_CONF_GetAddrInfo(nIndex, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		HxLOG_Error("Getting the Address Information of Network is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	APK_NETWORK_CONF_AddrA2N( pszInput, &ptNetAddr.ulGateway );

	if (APK_NETWORK_CONF_SetAddrInfo( nIndex, &ptNetAddr ) != ERR_OK)
	{
		HxLOG_Error("Setting the Address Information of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
		TRLEAVE(ERR_FAIL);
	}

	TRLEAVE(ERR_OK);
}

static HERROR set_data_LAN_DNSServers(HCHAR *pszInput)
{
	int 	nIndex;
	char	DNSAddr1[TRPORT_IPADDR_LEN], DNSAddr2[TRPORT_IPADDR_LEN];
	APKS_NETIFADDR_t	ptNetAddr;

	CHECK_NULL(pszInput);


	if (APK_NETWORK_CONF_GetDefaultDev(&nIndex) != ERR_OK)
	{
		HxLOG_Error("Getting the default of Network Interfaces is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	if (APK_NETWORK_CONF_GetAddrInfo(nIndex, (APKS_NETIFADDR_t*)&ptNetAddr) != ERR_OK)
	{
		HxLOG_Error("Getting the Address Information of Network is failed.\n");
		TRLEAVE(ERR_FAIL);
	}

	sscanf ( pszInput, "%s, %s", DNSAddr1, DNSAddr2);

	if(DNSAddr1 != NULL)
		APK_NETWORK_CONF_AddrA2N( DNSAddr1, &ptNetAddr.ulDns1 );

	if(DNSAddr2 != NULL)
		APK_NETWORK_CONF_AddrA2N( DNSAddr2, &ptNetAddr.ulDns2 );

	if (APK_NETWORK_CONF_SetAddrInfo( nIndex, &ptNetAddr ) != ERR_OK)
	{
		HxLOG_Error("Setting the Address Information of Network is failed. [%s:%d]\n", __FUNCTION__, __LINE__);
		TRLEAVE(ERR_FAIL);
	}

	TRLEAVE(ERR_OK);
}

static HERROR set_data_AudienceMeasurement_CombinedAccess(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = TRP_DB_setData_AudienceMeasurement_CombinedAccess(pszInput);

	TRLEAVE(hErr);
}

static HERROR set_data_AudienceMeasurement_PeriodicInformationInterval(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = TRP_DB_setData_AudienceMeasurement_PeriodicInformationInterval(pszInput);

	TRLEAVE(hErr);
}

static HERROR set_data_AudienceMeasurement_PeriodicInformationTime(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = TRP_DB_setData_AudienceMeasurement_PeriodicInformationTime(pszInput);

	TRLEAVE(hErr);
}

static HERROR set_data_AudienceMeasurement_PermissionLevel(HCHAR *pszInput)
{
	HERROR hErr = ERR_OK;

	CHECK_NULL(pszInput);
	hErr = TRP_DB_setData_AudienceMeasurement_PermissionLevel(pszInput);

	TRLEAVE(hErr);
}

/*
Device.DeviceInfo
- Manufacturer		string(64)
- ManufacturerOUI	string(6)
- ModelName			string(64)
- Description		string(256)
- SerialNumber		string(64)
- HardwareVersion	string(64)
- SoftwareVersion	string(64)
- DeviceStatus		string
- UpTime			unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_DeviceInfo_ObjectForm[] =
{
	{"Manufacturer",	eHxJSON_String, 64,  (pfn_getCB_t)get_data_DeviceInfo_Manufacturer, (pfn_setCB_t)NULL, NULL},
	{"ManufacturerOUI",	eHxJSON_String, 6,  (pfn_getCB_t)get_data_DeviceInfo_ManufacturerOUI, (pfn_setCB_t)NULL, NULL},
	{"ModelName", 		eHxJSON_String, 64,  (pfn_getCB_t)get_data_DeviceInfo_ModelName, (pfn_setCB_t)NULL, NULL},
	{"Description", 	eHxJSON_String, 256,  (pfn_getCB_t)get_data_DeviceInfo_Description, (pfn_setCB_t)NULL, NULL},
	{"ProductClass", 	eHxJSON_String, 64,  (pfn_getCB_t)get_data_DeviceInfo_ProductClass, (pfn_setCB_t)NULL, NULL},
	{"SerialNumber", 	eHxJSON_String, 64,  (pfn_getCB_t)get_data_DeviceInfo_SerialNumber, (pfn_setCB_t)NULL, NULL},
	{"HardwareVersion",	eHxJSON_String, 64,  (pfn_getCB_t)get_data_DeviceInfo_HardwareVersion, (pfn_setCB_t)NULL, NULL},
	{"SoftwareVersion",	eHxJSON_String, 64,  (pfn_getCB_t)get_data_DeviceInfo_SoftwareVersion, (pfn_setCB_t)NULL, NULL},
	{"DeviceStatus", 	eHxJSON_String, 256,  (pfn_getCB_t)get_data_DeviceInfo_DeviceStatus, (pfn_setCB_t)NULL, NULL},
	{"UpTime",	 		eHxJSON_String, 11,  (pfn_getCB_t)get_data_DeviceInfo_UpTime, (pfn_setCB_t)NULL, NULL},
	{"X_JCOM_SubscriberID",	eHxJSON_String, 256, (pfn_getCB_t)get_data_DeviceInfo_X_JCOM_SubscriberID, (pfn_setCB_t)set_data_DeviceInfo_X_JCOM_SubscriberID, NULL},
	{"X_JCOM_ServiceID", eHxJSON_String, 256, (pfn_getCB_t)get_data_DeviceInfo_X_JCOM_ServiceID, (pfn_setCB_t)set_data_DeviceInfo_X_JCOM_ServiceID, NULL},
	{"X_JCOM_CorpCode",	eHxJSON_String, 256, (pfn_getCB_t)get_data_DeviceInfo_X_JCOM_CorpCode, (pfn_setCB_t)set_data_DeviceInfo_X_JCOM_CorpCode, NULL},

	{NULL, 				eHxJSON_NULL,   0, NULL, NULL, NULL}
};

/*
Device.ManagementServer
- URL						string(256)
- Username					string(256)
- Password					string(256)
- PeriodicInformEnable		boolean
- PeriodicInformInterval	unsignedInt[1:]
- PeriodicInformTime		dateTime
- ParameterKey				string(32)
- ConnectionRequestURL		string(256)
- ConnectionRequestUsername	string(256)
- ConnectionRequestPassword	string(256)
- UpgradesManaged			boolean
*/
const static TRPORT_OBJECT_FORMAT_t	s_ManagmentServer_ObjectForm[] =
{
	{"URL", 					eHxJSON_String, 256,  (pfn_getCB_t)get_data_ManagementServer_URL, (pfn_setCB_t)set_data_ManagementServer_URL, NULL},
	{"Username", 				eHxJSON_String, 256,  (pfn_getCB_t)get_data_ManagementServer_Username, (pfn_setCB_t)set_data_ManagementServer_Username, NULL},
	{"Password", 				eHxJSON_String, 256,  (pfn_getCB_t)get_data_ManagementServer_Password, (pfn_setCB_t)set_data_ManagementServer_Password, NULL},
	{"PeriodicInformEnable", 	eHxJSON_String, 256,  (pfn_getCB_t)get_data_ManagementServer_PeriodicInformEnable, (pfn_setCB_t)set_data_ManagementServer_PeriodicInformEnable, NULL},
	{"PeriodicInformInterval",	eHxJSON_String, 11,  (pfn_getCB_t)get_data_ManagementServer_PeriodicInformInterval, (pfn_setCB_t)set_data_ManagementServer_PeriodicInformInterval, NULL},
	{"PeriodicInformTime", 		eHxJSON_String, 256,  (pfn_getCB_t)get_data_ManagementServer_PeriodicInformTime, (pfn_setCB_t)set_data_ManagementServer_PeriodicInformTime, NULL},
	{"ParameterKey", 			eHxJSON_String,  32,  (pfn_getCB_t)get_data_ManagementServer_ParameterKey, (pfn_setCB_t)NULL, NULL},
	{"ConnectionRequestURL", 	eHxJSON_String, 256,  (pfn_getCB_t)get_data_ManagementServer_ConnectionRequestURL, (pfn_setCB_t)NULL, NULL},
	{"ConnectionRequestUsername", eHxJSON_String, 256,  (pfn_getCB_t)get_data_ManagementServer_ConnectionRequestUsername, (pfn_setCB_t)set_data_ManagementServer_ConnectionRequestUsername, NULL},
	{"ConnectionRequestPassword",  eHxJSON_String, 256,  (pfn_getCB_t)get_data_ManagementServer_ConnectionRequestPassword, (pfn_setCB_t)set_data_ManagementServer_ConnectionRequestPassword, NULL},
	{"UpgradesManaged", 		eHxJSON_String, 256,  (pfn_getCB_t)get_data_ManagementServer_UpgradesManaged, (pfn_setCB_t)set_data_ManagementServer_UpgradesManaged, NULL},
	{NULL, 						eHxJSON_NULL, 0, NULL, NULL, NULL}
};

/*
Device.LAN
- AddressingType
- IPAddress
- SubnetMask
- DefaultGateway
- DNSServers
- MACAddress
*/
const static TRPORT_OBJECT_FORMAT_t	s_LAN_ObjectForm[] =
{
	{"AddressingType",	eHxJSON_String, 256, (pfn_getCB_t)get_data_LAN_AddressingType, (pfn_setCB_t)set_data_LAN_AddressingType, NULL},
	{"IPAddress", 		eHxJSON_String, 256, (pfn_getCB_t)get_data_LAN_IPAddress, (pfn_setCB_t)set_data_LAN_IPAddress, NULL},
	{"SubnetMask", 		eHxJSON_String, 256, (pfn_getCB_t)get_data_LAN_SubnetMask, (pfn_setCB_t)set_data_LAN_SubnetMask, NULL},
	{"DefaultGateway",	eHxJSON_String, 256, (pfn_getCB_t)get_data_LAN_DefaultGateway, (pfn_setCB_t)set_data_LAN_DefaultGateway, NULL},
	{"DNSServers", 		eHxJSON_String, 256, (pfn_getCB_t)get_data_LAN_DNSServers, (pfn_setCB_t)set_data_LAN_DNSServers, NULL},
	{"MACAddress", 		eHxJSON_String, 256, (pfn_getCB_t)get_data_LAN_MACAddress, (pfn_setCB_t)NULL, NULL},
	{NULL,				eHxJSON_NULL, 0, NULL, NULL, NULL}
};

/*
Device.X_JLABS_Management.AudienceMeasurement
- CombinedAccess				boolean
- PeriodicInformationInterval	unsignedInt[1:]
- PeriodicInformationTime		dateTime
- PermissionLevel				signedInt[0,2]
*/
const static TRPORT_OBJECT_FORMAT_t	s_AudienceMeasurement_ObjectForm[] =
{
	{"CombinedAccess", 			eHxJSON_String, 256, (pfn_getCB_t)get_data_AudienceMeasurement_CombinedAccess, 			(pfn_setCB_t)set_data_AudienceMeasurement_CombinedAccess, NULL},
	{"PeriodicInformationInterval",  eHxJSON_String, 11, (pfn_getCB_t)get_data_AudienceMeasurement_PeriodicInformationInterval, (pfn_setCB_t)set_data_AudienceMeasurement_PeriodicInformationInterval, NULL},
	{"PeriodicInformationTime",	eHxJSON_String, 256, (pfn_getCB_t)get_data_AudienceMeasurement_PeriodicInformationTime, 	(pfn_setCB_t)set_data_AudienceMeasurement_PeriodicInformationTime, NULL},
	{"PermissionLevel", 		eHxJSON_String, 256, (pfn_getCB_t)get_data_AudienceMeasurement_PermissionLevel, 			(pfn_setCB_t)set_data_AudienceMeasurement_PermissionLevel, NULL},
	{NULL, 						eHxJSON_NULL, 0, NULL, NULL, NULL}
};

/*
Device.X_JLABS_Management
+ AudienceMeasurement
*/

const static TRPORT_OBJECT_FORMAT_t	s_X_JLABS_Management_ObjectForm[] =
{
	{"AudienceMeasurement",		eHxJSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_AudienceMeasurement_ObjectForm},
	{NULL, eHxJSON_NULL, 0, NULL, NULL, NULL}
};

#if defined(NEW_DM)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// STBService //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.Capabilities.PVR.
/*
	MaxIOStreams	int[-1:]
	MaxRecordingStreams	int[-1:]
	MaxPlaybackStreams	int[-1:]
	MaxTimeDelay	int[-1:]
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_PVR_ObjectForm[] =
{
	{"MaxIOStreams",	eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"MaxRecordingStreams",	eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"MaxPlaybackStreams",	eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"MaxTimeDelay",	eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
};
//Device.Services.STBService.{i}.Capabilities.AudioDecoder.
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_AudioDecoder_ObjectForm[] =
{
	{"AudioStandards",	eHxJSON_String, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
};
////////////////////////////////////////////////// MPEG2Part2 //////////////////////////////////////////////////
//Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG2Part2.ProfileLevel.{i}.
/*
	Profile	string
	Level	string
	MaximumDecodingCapability	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_VideoDecoder_MPEG2Part2_ProfileLevel_ObjectForm[] =
{
	{"Profile",	eHxJSON_String, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"Level",	eHxJSON_String, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"MaximumDecodingCapability",	eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
};
//Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG2Part2.
/*
	AudioStandards	string
	ProfileLevelNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_VideoDecoder_MPEG2Part2_ObjectForm[] =
{
	{"AudioStandards",	eHxJSON_String, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"ProfileLevelNumberOfEntries",	eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"ProfileLevel",	eHxJSON_Array, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_VideoDecoder_MPEG2Part2_ProfileLevel_ObjectForm},
};

////////////////////////////////////////////////// MPEG4Part2 //////////////////////////////////////////////////
//Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG4Part2.ProfileLevel.{i}.
/*
	Profile	string
	Level	string
	MaximumDecodingCapability	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_VideoDecoder_MPEG4Part2_ProfileLevel_ObjectForm[] =
{
	{"Profile",	eHxJSON_String, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"Level",	eHxJSON_String, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"MaximumDecodingCapability",	eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
};
//Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG4Part2.
/*
	AudioStandards	string
	ProfileLevelNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_VideoDecoder_MPEG4Part2_ObjectForm[] =
{
	{"AudioStandards",	eHxJSON_String, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"ProfileLevelNumberOfEntries",	eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"ProfileLevel",	eHxJSON_Array, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_VideoDecoder_MPEG2Part2_ProfileLevel_ObjectForm},
};
////////////////////////////////////////////////// MPEG4Part10 //////////////////////////////////////////////////
//Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG4Part10.ProfileLevel.{i}.
/*
	Profile	string
	Level	string
	MaximumDecodingCapability	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_VideoDecoder_MPEG4Part10_ProfileLevel_ObjectForm[] =
{
	{"Profile",	eHxJSON_String, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"Level",	eHxJSON_String, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"MaximumDecodingCapability",	eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
};
//Device.Services.STBService.{i}.Capabilities.VideoDecoder.MPEG4Part10.
/*
	AudioStandards	string
	ProfileLevelNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_VideoDecoder_MPEG4Part10_ObjectForm[] =
{
	{"AudioStandards",	eHxJSON_String, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"ProfileLevelNumberOfEntries",	eHxJSON_Integer, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"ProfileLevel",	eHxJSON_Array, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_VideoDecoder_MPEG4Part10_ProfileLevel_ObjectForm},
};

////////////////////////////////////////////////// SMPTEVC1 //////////////////////////////////////////////////
//Device.Services.STBService.{i}.Capabilities.VideoDecoder.SMPTEVC1.ProfileLevel.{i}.
/*
	Profile	string
	Level	string
	MaximumDecodingCapability	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_VideoDecoder_SMPTEVC1_ProfileLevel_ObjectForm[] =
{
	{"Profile",	eHxJSON_String, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"Level",	eHxJSON_String, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"MaximumDecodingCapability",	eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
};

//Device.Services.STBService.{i}.Capabilities.VideoDecoder.SMPTEVC1.
/*
	AudioStandards	string
	ProfileLevelNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_VideoDecoder_SMPTEVC1_ObjectForm[] =
{
	{"AudioStandards",	eHxJSON_String, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"ProfileLevelNumberOfEntries",	eHxJSON_Integer, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"ProfileLevel",	eHxJSON_Array, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_VideoDecoder_SMPTEVC1_ProfileLevel_ObjectForm},
};

////////////////////////////////////////////////// VideoDecoder //////////////////////////////////////////////////
//Device.Services.STBService.{i}.Capabilities.VideoDecoder.
/*
	VideoStandards	string
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_VideoDecoder_ObjectForm[] =
{
	{"VideoStandards",	eHxJSON_String, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"MPEG2Part2",	eHxJSON_Object, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_VideoDecoder_MPEG2Part2_ObjectForm},
	{"MPEG4Part2",	eHxJSON_Object, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_VideoDecoder_MPEG4Part2_ObjectForm},
	{"MPEG4Part10",	eHxJSON_Object, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_VideoDecoder_MPEG4Part10_ObjectForm},
	{"SMPTEVC1",	eHxJSON_Object, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_VideoDecoder_SMPTEVC1_ObjectForm},
};
//Device.Services.STBService.{i}.Capabilities.AudioOutput.
/*
	AudioFormats	string
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_AudioOutput_ObjectForm[] =
{
	{"AudioFormats",	eHxJSON_String, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
};
//Device.Services.STBService.{i}.Capabilities.VideoOutput.
/*
	CompositeVideoStandards	string
	VideoFormats	string
	Macrovision	string(32)
	HDCP	string(32)
	DisplayFormats	string
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_VideoOutput_ObjectForm[] =
{
	{"CompositeVideoStandards",	eHxJSON_String, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"VideoFormats",	eHxJSON_String, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"Macrovision",	eHxJSON_String, 32, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"HDCP",	eHxJSON_String, 32, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"DisplayFormats",	eHxJSON_String, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
};
//Device.Services.STBService.{i}.Capabilities.HDMI.
/*
	SupportedResolutions	string
	CECSupport	boolean
	HDMI3D	boolean
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_HDMI_ObjectForm[] =
{
	{"SupportedResolutions",	eHxJSON_String, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"CECSupport",	eHxJSON_String, 256, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"HDMI3D",	eHxJSON_String, 32, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
};

//Device.Services.STBService.{i}.Capabilities.CA.
/*
	CASystems	string(1024)
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_CA_ObjectForm[] =
{
	{"CASystems",	eHxJSON_String, 1024, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
};

//Device.Services.STBService.{i}.Capabilities.DRM.
/*
	DRMSystems	string(1024)
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_DRM_ObjectForm[] =
{
	{"DRMSystems",	eHxJSON_String, 1024, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
};

//Device.Services.STBService.{i}.Capabilities.ServiceMonitoring.
/*
	MaxActiveMainStreams	int[-1:]
	MinSampleInterval	int[-1:]
	MaxReportSamples	int[-1:]
	HighLevelMetricNames	string(1024)
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_ServiceMonitoring_ObjectForm[] =
{
	{"MaxActiveMainStreams",	eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"MinSampleInterval",	eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"MaxReportSamples",	eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"HighLevelMetricNames",	eHxJSON_String, 1024, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
};

//Device.Services.STBService.{i}.Capabilities.
/*
	MaxActiveAVStreams	int[-1:]
	MaxActiveAVPlayers	int[-1:]
*/
const static TRPORT_OBJECT_FORMAT_t	s_Capabilities_ObjectForm[] =
{
	{"MaxActiveAVStreams",	eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"MaxActiveAVPlayers",	eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"PVR",					eHxJSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_PVR_ObjectForm},
	{"AudioDecoder",		eHxJSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_AudioDecoder_ObjectForm},
	{"VideoDecoder",		eHxJSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_VideoDecoder_ObjectForm},
	{"AudioOutput",			eHxJSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_AudioOutput_ObjectForm},
	{"VideoOutput",			eHxJSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_VideoOutput_ObjectForm},
	{"HDMI",				eHxJSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_HDMI_ObjectForm},
	{"CA",					eHxJSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_CA_ObjectForm},
	{"DRM",					eHxJSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_DRM_ObjectForm},
	{"ServiceMonitoring",	eHxJSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_ServiceMonitoring_ObjectForm},
};



//Device.Services.STBService.{i}.Components.FrontEnd.{i}.X_JCOM_ISDBC.
//Device.Services.STBService.{i}.Components.FrontEnd.{i}.X_JCOM_ISDBC.Modulation.
//Device.Services.STBService.{i}.Components.FrontEnd.{i}.X_JCOM_ISDBC.Service.
//Device.Services.STBService.{i}.Components.FrontEnd.{i}.X_JCOM_DOCSIS.
//Device.Services.STBService.{i}.Components.FrontEnd.{i}.X_JCOM_DOCSIS.download.
//Device.Services.STBService.{i}.Components.FrontEnd.{i}.X_JCOM_DOCSIS.upload.
//Device.Services.STBService.{i}.Components.FrontEnd.{i}.

//Device.Services.STBService.{i}.Components.PVR.Storage.{i}.
//Device.Services.STBService.{i}.Components.PVR.
//Device.Services.STBService.{i}.Components.AudioDecoder.{i}.
//Device.Services.STBService.{i}.Components.VideoDecoder.{i}.
//Device.Services.STBService.{i}.Components.AudioOutput.{i}.
//Device.Services.STBService.{i}.Components.SPDIF.{i}.
//Device.Services.STBService.{i}.Components.VideoOutput.{i}.
//Device.Services.STBService.{i}.Components.HDMI.{i}.
//Device.Services.STBService.{i}.Components.HDMI.{i}.DisplayDevice.
//Device.Services.STBService.{i}.Components.CA.{i}.


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Device.Services.STBService.{i}.Components.
/*
	FrontEndNumberOfEntries	unsignedInt
	AudioDecoderNumberOfEntries	unsignedInt
	VideoDecoderNumberOfEntries	unsignedInt
	AudioOutputNumberOfEntries	unsignedInt
	VideoOutputNumberOfEntries	unsignedInt
	CANumberOfEntries	unsignedInt
	HDMINumberOfEntries	unsignedInt
	SPDIFNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_Components_ObjectForm[] =
{
	{"FrontEndNumberOfEntries",	eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"AudioDecoderNumberOfEntries",	eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"VideoDecoderNumberOfEntries",	eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"AudioOutputNumberOfEntries",	eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"VideoOutputNumberOfEntries",	eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"CANumberOfEntries",	eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"HDMINumberOfEntries",	eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"SPDIFNumberOfEntries",	eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
};




const static TRPORT_OBJECT_FORMAT_t	s_STBService_ObjectForm[] =
{
	{"Capabilities",		eHxJSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Capabilities_ObjectForm},
	{"Components",		eHxJSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Components_ObjectForm},
	//add.....

	{NULL, eHxJSON_NULL, 0, NULL, NULL, NULL},
};

//Device.Services.StorageService.{i}.
/*
	Enable	boolean
	PhysicalMediumNumberOfEntries	unsignedInt
	LogicalVolumeNumberOfEntries	unsignedInt
*/
const static TRPORT_OBJECT_FORMAT_t	s_StorageService_ObjectForm[] =
{
	{"Enable",		eHxJSON_String, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"PhysicalMediumNumberOfEntries",		eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
	{"LogicalVolumeNumberOfEntries",		eHxJSON_Integer, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, NULL},
};

// Add data model....
// Add data model....
// Add data model....
// Add data model....
const static TRPORT_OBJECT_FORMAT_t	s_Service_ObjectForm[] =
{
	{"STBService",		eHxJSON_Array, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_STBService_ObjectForm},
	{"StorageService",		eHxJSON_Array, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_StorageService_ObjectForm},
	{NULL, eHxJSON_NULL, 0, NULL, NULL, NULL}
};
#endif
//+ Object
//- Parameter
/*
Device.
- DeviceSummary
+ DeviceInfo
+ ManagementServer
+ LAN
+ X_JLABS_Management
*/
const static TRPORT_OBJECT_FORMAT_t	s_Device_ObjectForm[] =
{
	{"DeviceSummary", 		eHxJSON_String, 256, (pfn_getCB_t)get_data_Device_DeviceSummary, (pfn_setCB_t)NULL, NULL},
	{"DeviceInfo", 			eHxJSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_DeviceInfo_ObjectForm},
	{"ManagementServer", 	eHxJSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_ManagmentServer_ObjectForm},
	{"LAN", 				eHxJSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_LAN_ObjectForm},
	{"X_JLABS_Management",	eHxJSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_X_JLABS_Management_ObjectForm},
#if defined(NEW_DM)
	{"Service",				eHxJSON_Object, 0, (pfn_getCB_t)NULL, (pfn_setCB_t)NULL, (TRPORT_OBJECT_FORMAT_t *)s_Service_ObjectForm},
#endif
	{NULL, eHxJSON_NULL, 0, NULL, NULL, NULL}
};

static const HCHAR *	trport_json_next_token (HCHAR *token, HINT32 n, const HCHAR *p)
{
	if (*p == '\0')
		return NULL;

	if (*p == '.')
		p++;

	while (n-- && *p != '\0' && *p != '.')
	{
		*token++ = *p++;
	}

	if (n < 0)
		return NULL;

	*token = '\0';

	return p;
}

//입력 object전체의 {parameter:value} pair를 json구조에 추가한다.
static HERROR trport_add_object2json(HxJSON_t json, const TRPORT_OBJECT_FORMAT_t *objectFormat, HCHAR *token)
{
	HERROR hErr = ERR_OK;
	HxJSON_t subjson;
	HCHAR szstr[256];
	HUINT32 retVal;
	const TRPORT_OBJECT_FORMAT_t *objectloop;

	TRENTER;
	CHECK_NULL(token);

	subjson = HxJSON_Object_New();
	if (NULL == subjson)
	{
		TRLEAVE(ERR_FAIL);
	}

	objectloop = objectFormat;
	while(objectloop->ElementName)
	{
		switch(objectloop->ElementType)
		{
			case eHxJSON_Object:
				if(objectloop->s_objectform != NULL)
				{
					HxLOG_Debug("ElementName[%s]\n", objectloop->ElementName);
					hErr |= trport_add_object2json(subjson, objectloop->s_objectform, objectloop->ElementName);
				}
				else
				{
					HxLOG_Error("s_objectform is not registered! %s\n",objectloop->ElementName);
				}
				break;

			case eHxJSON_String:
				if(objectloop->pfnCbGetData != NULL)
				{
					hErr |= objectloop->pfnCbGetData((HCHAR*)szstr);
					hErr |= HxJSON_Object_Set(subjson, objectloop->ElementName, HxJSON_String_New(szstr));
				}
				else
				{
					HxLOG_Error("pfnCbGetData is not registered! %s\n",token);
				}
				break;

			case eHxJSON_Integer:
				if(objectloop->pfnCbGetData != NULL)
				{
					hErr |= objectloop->pfnCbGetData((HUINT32*)&retVal);
					hErr |= HxJSON_Object_Set(subjson, objectloop->ElementName, HxJSON_Integer_New(retVal));
				}
				else
				{
					HxLOG_Error("pfnCbGetData is not registered! %s\n",token);
				}
				break;

			case eHxJSON_TRUE:
			case eHxJSON_FALSE:
				if(objectloop->pfnCbGetData != NULL)
				{
					hErr |= objectloop->pfnCbGetData((HUINT32*)&retVal);
					hErr |= HxJSON_Object_Set(subjson, objectloop->ElementName, HxJSON_Boolean_New(retVal));
				}
				else
				{
					HxLOG_Error("pfnCbGetData is not registered! %s\n",token);
				}
				break;

			default:
				hErr = ERR_FAIL;
				HxLOG_Error("Not Support!! name:%s, type:%d\n", objectloop->ElementName, objectloop->ElementType);
				break;
		}

		objectloop++;
	}

	hErr |= HxJSON_Object_Set(json, token, subjson);

	if (hErr != ERR_OK)
	{
		HLIB_JSON_Delete(subjson);
		subjson = NULL;
	}

	TRLEAVE(hErr);
}

//입력받은 json구조에 존재하는 {parameter:value} pair를 이용하여, db값을 수정한다.
static HERROR trport_revise_data_using_json(HxJSON_t json, const TRPORT_OBJECT_FORMAT_t *objectFormat)
{
	const HCHAR *token;
	HERROR		hErr =  ERR_OK;
	void		*iter;
	HCHAR		*pszstr;
	HxJSON_t 	value;
	HBOOL		bfind = FALSE;
	const TRPORT_OBJECT_FORMAT_t *objectloop;

	CHECK_NULL(json && objectFormat);

	iter = HLIB_JSON_Object_Iterator(json);
	PRINT_JSON(json);

	while(iter)
	{
		token = HLIB_JSON_Object_IteratorKey(iter);
		value = HLIB_JSON_Object_IteratorValue(iter);
		HxLOG_Debug("token : %s\n",token);

		bfind = FALSE;
		objectloop = objectFormat;
		while(objectloop->ElementName)
		{
			if(HxSTD_StrCmp(token, objectloop->ElementName) == 0)
			{
				bfind = TRUE;
				HxLOG_Debug("ElementName[%-32s], HxJSON_TypeOf[%d], ElementType[%d]\n",objectloop->ElementName, HLIB_JSON_TypeOf(value), objectloop->ElementType);

				if(HxJSON_IsObject(value) && (objectloop->ElementType == eHxJSON_Object) && (objectloop->s_objectform != NULL))
				{
					hErr = trport_revise_data_using_json(value, objectloop->s_objectform);
					if(hErr != ERR_OK)
					{
						HxLOG_Error("trport_revise_data_using_json returns Error![%s:%d]\n",__FUNCTION__, __LINE__);
						TRLEAVE(hErr);
					}
				}

				if(HxJSON_IsString(value) && (objectloop->ElementType == eHxJSON_String))
				{
					if(objectloop->pfnCbSetData == NULL)
					{
						HxLOG_Error("pfnCbSetData was not registered! %s\n",objectloop->ElementName);
						break;
					}

					pszstr = (HCHAR *)HxJSON_String_Get(value);
					HxLOG_Debug("token : %s, value : %s\n",token, pszstr);

					hErr = objectloop->pfnCbSetData((HCHAR*)pszstr);
					if(hErr != ERR_OK)
					{
						HxLOG_Error("pfnCbSetData returns Error! ElementName=%s [%s:%d]\n",objectloop->ElementName, __FUNCTION__, __LINE__);
						TRLEAVE(hErr);
					}
				}

				break;
			}

			objectloop++;
		}

		if(bfind == FALSE)
		{
			HxLOG_Error("token does not belong to DataModel! %s\n", token);
			TRLEAVE(ERR_FAIL);
		}

		iter = HLIB_JSON_Object_NextIterator(json, iter);
	}

	TRLEAVE(hErr);
}

//key값에 해당하는 parameter를 json구조로 만들어서 return한다.
//key값이 object이면서 .으로 긑나면 object내의 모든 {parameter:value} pair를 json구조로 만든다.
//key값이 단일 parameter일 경우 parameter에 해당하는 {parameter:value} pair를 json 구조로 만든다.
//key값 example
//ex> "Device.ManagementServer."
//ex> "Device.Lan.IPAddress"
static HERROR trport_build_json_using_key(HCHAR *key, HxJSON_t json, const TRPORT_OBJECT_FORMAT_t *objectFormat)
{
	HERROR		hErr =  ERR_OK;
	HCHAR 		token[32], pszstr[256];
	HBOOL		bfind = FALSE;
	const TRPORT_OBJECT_FORMAT_t *objectloop;

	CHECK_NULL(key && objectFormat);

	key = (HCHAR*)trport_json_next_token(token, 32, key);
	CHECK_NULL(key);
	//HxLOG_Debug("[token:%s][key:%s] strlen:%d\n", token, key, strlen(key));

	objectloop = objectFormat;
	while(objectloop->ElementName)
	{
		bfind = FALSE;
		if(HxSTD_StrCmp(token, objectloop->ElementName) == 0)
		{
			//HxLOG_Debug("ElementName[%s], ElementType[%d]\n",objectloop->ElementName, objectloop->ElementType);

			bfind = TRUE;
			switch(objectloop->ElementType)
			{
				case eHxJSON_Object:
					if(objectloop->s_objectform == NULL)
					{
						HxLOG_Error("s_objectform is NULL! [%s]", token);
						TRLEAVE(ERR_FAIL);
					}

					if(*key=='.' && strlen(key)==1)//마지막 글자가 .일 경우 object로 return된다. 하위 element들을 모두 추가한다.
					{
						hErr |= trport_add_object2json(json, objectloop->s_objectform, token);
					}
					else
					{
						HxJSON_Object_Set(json, token, HxJSON_Object_New());
						json  = HxJSON_Object_Get(json, token);
						hErr |= trport_build_json_using_key(key, json, objectloop->s_objectform);
					}
					break;

				case eHxJSON_String:
					if(objectloop->pfnCbGetData != NULL)
					{
						hErr |= objectloop->pfnCbGetData((HCHAR*)pszstr);
						//HxLOG_Debug("token : [%s], value : [%s]\n",token, pszstr);
						HxJSON_Object_Set(json, token, HxJSON_String_New(pszstr));
					}
					else
					{
						HxLOG_Error("pfnCbGetData was not registered! %s\n",objectloop->ElementName);
						TRLEAVE(ERR_FAIL);
					}
					break;
#if defined(NEW_DM)
				case eHxJSON_Array:
					if(objectloop->s_objectform == NULL)
					{
						HxLOG_Error("s_objectform is NULL! [%s]", token);
						TRLEAVE(ERR_FAIL);
					}
					if(*key=='.' && strlen(key)==1)//마지막 글자가 .일 경우 object로 return된다. 하위 element들을 모두 추가한다.
					{
						hErr |= trport_add_object2json(json, objectloop->s_objectform, token);
					}
					else
					{
						HxJSON_Object_Set(json, token, HxJSON_Object_New());
//						json  = HxJSON_Object_Get(json, token);

					}

					break;
#endif
				default:
					HxLOG_Error("Not Support!! token:[%s], type:[%d]\n", token, objectloop->ElementType);
					hErr = ERR_FAIL;
					break;
			}
			break;
		}

		objectloop++;
	}

	if(bfind == FALSE)
	{
		HxLOG_Error("key does not belong to DataModel! [%s]\n", key);
	}

	TRLEAVE(hErr);
}

#define TRPORT_STBCONFIG_ZIP_FILENAME	"/tmp/test.tar.gz"
#define TRPORT_STBCONFIG_TAR_FILENAME	"/tmp/test.tar"
#define TRPORT_STBCONFIG_UNARCHIVE_PATH "/var/lib/humaxtv/dlogger/"

HERROR TRP_UncompressGzipfile(HCHAR *gzipFilename, HCHAR *decompFilename)
{
	HxLOG_Debug("decompFilename[%s] gzipFilename[%s]\n", decompFilename, gzipFilename);

	HLIB_FILE_Delete(decompFilename);
	if(HLIB_FILE_Exist(gzipFilename) != TRUE)
	{
		HxLOG_Error(" there is no file %s[%s:%d]\n",gzipFilename, __FUNCTION__, __LINE__);
		TRLEAVE(ERR_FAIL);
	}
	if(HLIB_GZ_Uncompress(gzipFilename, decompFilename) != ERR_OK)
	{
		HxLOG_Debug("Fail to decompress file [%s] [%s:%d],\n", decompFilename, __LINE__);
		TRLEAVE(ERR_FAIL);
	}

	TRLEAVE(ERR_OK);
}

HERROR TRP_UnarchiveTarfile(HCHAR *tarFilename, HCHAR *destFullpath)
{
	int 	status = 0;
	int 	childPid = 0;
	pid_t	pid;

	HxLOG_Debug("tarFilename[%s]=>destFullpath[%s]\n", tarFilename, destFullpath);

	/* vfork and exec udhcpc */ /* insmod */
	if((pid = fork()) < 0)
	{
		HxLOG_Debug("Inside1 	 if((pid = fork()) < 0)\n");

		pthread_detach(pthread_self());
		TRLEAVE(ERR_FAIL);
	}
	else if(pid == 0)
	{
		/* child */
		HxLOG_Debug("child process start\n");
		/* child */
		if(execlp("tar", "tar", "-xf", tarFilename, "-C", destFullpath, (char *)0) < 0)
		{
			HxLOG_Error("tar error!\n");
			_exit(EXIT_FAILURE);
			pthread_detach(pthread_self());
			TRLEAVE(ERR_FAIL);
		}
	}
	else
	{
		/* parent */
		HxLOG_Debug("parent process start\n");
		//1 parent를 잡아도 OK??
		childPid = waitpid(pid, &status , 0);
		if ( -1 == childPid )
		{
			HxLOG_Error("waitpid error\n");
			TRLEAVE(ERR_FAIL);
		}
		if(status != 0)
		{
			HxLOG_Error("tar status error[%s:%d]\n", __FUNCTION__, __LINE__);
			TRLEAVE(ERR_FAIL);
		}
		HxLOG_Debug("status : %d \n", status);
	}

	pthread_detach(pthread_self());
	TRLEAVE(ERR_OK);
}

static void print_params_in_object(const TRPORT_OBJECT_FORMAT_t objectFormat[])
{
	const TRPORT_OBJECT_FORMAT_t *objectloop;

	objectloop = objectFormat;
	HxLOG_Print(" Parameter : 0x%08x\n", objectloop);
	while(objectloop->ElementName)
	{
		HxLOG_Print("%d, %-32s, %4d, 0x%08x, 0x%08x, 0x%08x\n",
		objectloop->ElementType,
		objectloop->ElementName,
		objectloop->ulen,
		objectloop->pfnCbGetData,
		objectloop->pfnCbSetData,
		objectloop->s_objectform);

		objectloop++;
	}
	HxLOG_Print("\n");
}

#define __________GLOBAL_FUNCTION________________________________________
void TRP_TEST_PrintAllObject(void)
{
	print_params_in_object(s_Device_ObjectForm);
	print_params_in_object(s_DeviceInfo_ObjectForm);
	print_params_in_object(s_ManagmentServer_ObjectForm);
	print_params_in_object(s_LAN_ObjectForm);
	print_params_in_object(s_X_JLABS_Management_ObjectForm);
}

#ifdef CONFIG_DEBUG
HERROR TRP_TEST_CreateDeviceJson(void)
{
	HERROR hErr = ERR_OK;
	TRENTER;

	jsonroot = HxJSON_Object_New()	;
	if (NULL == jsonroot)
	{
		TRLEAVE(ERR_FAIL);
	}

	hErr = trport_add_object2json(jsonroot, s_Device_ObjectForm, TRPORT_DATAMODEL_ROOT_OBJECT);

	PRINT_JSON(jsonroot);

	if(hErr != ERR_OK)
	{
		HLIB_JSON_Delete(jsonroot);
		jsonroot = NULL;
	}

	TRLEAVE(hErr);
}

HERROR TRP_TEST_DestroyDeviceJson(void)
{
	HERROR hErr = ERR_OK;

	PRINT_JSON(jsonroot);

	if(jsonroot != NULL)
	{
		HLIB_JSON_Delete(jsonroot);
		jsonroot = NULL;
	}

	TRLEAVE(hErr);

}

void TRP_TEST_PrintAllDownloadInfor(void)
{
	HUINT32 i;

	for(i=eDOWNLOAD_TYPE_FIRMWARE; i<eDOWNLOAD_TYPE_MAX; i++)
	{
		HxLOG_Debug("state:[%d] filepath&filenmae:[%s%s] filesize:[%-16d]\n", stDownInfo[i].state, stDownInfo[i].filepath, stDownInfo[i].filename, stDownInfo[i].filesize);
	}
}
#endif


HERROR TRP_SetParam(char* input)
{
	HERROR 			hErr = ERR_OK;
	void			*iter;
	HxJSON_t 		json;
	const HCHAR* 	token;

	CHECK_NULL(input)
	HxLOG_Debug("input:%s\n",input);

	json = HLIB_JSON_Decode(input);

	PRINT_JSON(json);

	iter = HLIB_JSON_Object_Iterator(json);
	token = HLIB_JSON_Object_IteratorKey(iter);

	if (!HLIB_STD_StrStartWith(token, TRPORT_DATAMODEL_ROOT_OBJECT))
	{
		HxLOG_Error("[%s:%d] Syntax Error[input SHALL start with \"Device\"]: %s\n", __FUNCTION__, __LINE__, token);
		TRLEAVE(ERR_FAIL);
	}

	json = HxJSON_Object_Get(json, token);
	hErr = trport_revise_data_using_json(json, s_Device_ObjectForm);

	TRLEAVE(hErr);
}

HCHAR* TRP_GetParam(HCHAR *key)
{
	HERROR		hErr = ERR_OK;
	HxJSON_t 	json, outroot;
	HCHAR 		token[32];
	HCHAR		*szParam;

	TRENTER;

	HxLOG_Debug("key:[%s][%s:%d]\n", key, __FUNCTION__, __LINE__);

	key = (HCHAR*)trport_json_next_token(token, 32, key);

	if (!HLIB_STD_StrStartWith(token, TRPORT_DATAMODEL_ROOT_OBJECT))
	{
		HxLOG_Error("[%s:%d] Syntax Error[key have to start with \"Device\"]: %s\n", __FUNCTION__, __LINE__, token);
		return NULL;
	}

	outroot = HxJSON_Object_New();

	/*if last character is equal to dot(.) and length is equal to 1. then input must be "Devie."*/
	if(*key=='.' && strlen(key)==1)
	{
		hErr = trport_add_object2json(outroot, s_Device_ObjectForm, TRPORT_DATAMODEL_ROOT_OBJECT);
	}
	else
	{
		HxJSON_Object_Set(outroot, TRPORT_DATAMODEL_ROOT_OBJECT, HxJSON_Object_New());
		json  = HxJSON_Object_Get(outroot, TRPORT_DATAMODEL_ROOT_OBJECT);

		hErr = trport_build_json_using_key(key, json, s_Device_ObjectForm);
	}

	szParam = HLIB_JSON_Encode(outroot, NULL, 0);
	HxLOG_Debug("output : %s\n",szParam);

	return szParam;
}

HERROR TRP_ApplyFirmware(HUINT32 ulId)
{
	HBOOL				ret;
	HERROR				hErr = ERR_OK;
	HCHAR 				szVer[256];
	trport_download_info_t *pstInfo;
	trport_download_type_e eType;

	TRENTER;

	//일단 id를 type으로 사용함. 추후 변경 가능.
	eType = (trport_download_type_e)ulId;
	if(eType != eDOWNLOAD_TYPE_FIRMWARE)
	{
		HxLOG_Error("wrong download_id type [%d]\n", eType);
		TRLEAVE(ERR_FAIL);
	}

	hErr = TRP_GetDownloadInfo(eDOWNLOAD_TYPE_FIRMWARE, &pstInfo);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("can't get download info [%s:%d]\n", __FUNCTION__, __LINE__);
		TRLEAVE(ERR_FAIL);
	}

	if(pstInfo->state != eDOWNLOAD_STATE_DOWNLOADING)
	{
		HxLOG_Error("check FIRMWARE TYPE' state!! state[%d] filename[%s] filesize[%d]\n", pstInfo->state, pstInfo->filename, pstInfo->filesize);
		TRLEAVE(ERR_FAIL);
	}

	//save current SW version to check firmware is applied.
	get_data_DeviceInfo_SoftwareVersion((HCHAR*)szVer);
	HxLOG_Debug("Software version : [%s]\n", szVer);
	TRP_DB_SetPrevVersion(szVer);

	//set ota flag
	hErr = APK_SQCSCENARIOJAPAN_FinalizeIPOTA(&ret);
	if(hErr == ERR_FAIL || ret == FALSE)
	{
		HxLOG_Error("can't command to finalize IP OTA[%s:%d]\n", __FUNCTION__, __LINE__);
		TRLEAVE(ERR_FAIL);
	}

	TRP_DB_SetResetFlag(eFLAG_NO_NEED_TO_RESET);

	pstInfo->state = eDOWNLOAD_STATE_FINISH;
	TRP_DB_StoreDownloadInfo(eType, pstInfo);

	TRLEAVE(ERR_OK);
}

HERROR TRP_ApplySTBconfig(HUINT32 ulId)
{
	HERROR		hErr = ERR_OK;
	HCHAR 		szfullpath[TRPORT_DB_FULLPATH_LEN];
	trport_download_info_t *pstInfo;
	trport_download_type_e eType;

	TRENTER;

	//일단 id를 type으로 사용함. 추후 변경.
	eType = (trport_download_type_e)ulId;
	if(eType != eDOWNLOAD_TYPE_X_JCOM_CFG)
	{
		HxLOG_Error("wrong download_id type [%d]\n", eType);
		TRLEAVE(ERR_FAIL);
	}

	hErr = TRP_GetDownloadInfo(eDOWNLOAD_TYPE_X_JCOM_CFG, &pstInfo);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("can't get download info [%s:%d]\n", __FUNCTION__, __LINE__);
		TRLEAVE(ERR_FAIL);
	}

	if(pstInfo->state != eDOWNLOAD_STATE_DOWNLOADING)
	{
		HxLOG_Error("check FIRMWARE TYPE' state!! eType[%d] state[%d] filename[%s] filesize[%d]\n", eType, pstInfo->state, pstInfo->filename, pstInfo->filesize);
		TRLEAVE(ERR_FAIL);
	}

	HxSTD_snprintf(szfullpath, TRPORT_DB_FULLPATH_LEN, "%s/%s", pstInfo->filepath, pstInfo->filename);
	HxLOG_Print("szfullpath : [%s] [%s:%d]\n", szfullpath, __FUNCTION__, __LINE__);

	//STB config적용전 사용자에게 확인받기
	//1 jcom test용으로 임시 막음.
//	TRP_UncompressGzipfile(szfullpath, TRPORT_STBCONFIG_TAR_FILENAME);
//	TRP_UnarchiveTarfile(TRPORT_STBCONFIG_TAR_FILENAME, TRPORT_STBCONFIG_UNARCHIVE_PATH);
//	HLIB_FILE_Delete(szfullpath);
//	HLIB_FILE_Delete(TRPORT_STBCONFIG_TAR_FILENAME);

	pstInfo->state = eDOWNLOAD_STATE_STANDBY;
	TRP_DB_StoreDownloadInfo(eDOWNLOAD_TYPE_FIRMWARE, pstInfo);

	TRLEAVE(ERR_OK);
}

HERROR TRP_CheckFirmware(HUINT32 ulId)
{
	HERROR				hErr = ERR_OK;
	HCHAR 				szVer[16], szPrevVer[16];
	trport_download_info_t *pstInfo;
	trport_download_type_e eType;

	TRENTER;

	eType = (trport_download_type_e)ulId;
	if(eType != eDOWNLOAD_TYPE_FIRMWARE)
	{
		HxLOG_Error("wrong download_id type [%d]\n", eType);
		TRLEAVE(ERR_FAIL);
	}

	hErr = TRP_GetDownloadInfo(eDOWNLOAD_TYPE_FIRMWARE, &pstInfo);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("can't get download info [%s:%d]\n", __FUNCTION__, __LINE__);
		TRLEAVE(ERR_FAIL);
	}

	if(pstInfo->state != eDOWNLOAD_STATE_FINISH)
	{
		HxLOG_Error("check FIRMWARE TYPE' state!! state[%d] filename[%s] filesize[%d]\n", pstInfo->state, pstInfo->filename, pstInfo->filesize);
		TRLEAVE(ERR_FAIL);
	}

	pstInfo->state = eDOWNLOAD_STATE_STANDBY;
	TRP_DB_StoreDownloadInfo(eDOWNLOAD_TYPE_FIRMWARE, pstInfo);

	//get prev version from dldb,
	hErr = TRP_DB_GetPrevVersion(szPrevVer);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("can't get previous SW version [%s:%d]\n", __FUNCTION__, __LINE__);
		TRLEAVE(ERR_FAIL);
	}

	//get current version from APPKIT
	get_data_DeviceInfo_SoftwareVersion((HCHAR*)szVer);

	HxLOG_Debug("szVer[%s] szPrevVer[%s] res:%d\n", szVer, szPrevVer, HxSTD_StrCmp(szVer, szPrevVer));

	if(HxSTD_StrCmp(szVer, szPrevVer) != 0 )//두 버젼이 다를 경우. ota가 진행되엇다고 판단.
	{
		TRLEAVE(ERR_OK);
	}

	TRLEAVE(ERR_FAIL);
}

static HUINT32			s_ultimer = 0;
#define TIME_30SEC	30000

static void TRP_RebootCallback(unsigned long TimerId, void *params)
{
	TRENTER;
	HxLOG_Debug("[SHSEO] do HAPI_SendSignalNumber() -> eHSIG_RequestReboot\r\n");
	PAPI_RequestReboot();
#if	0
	if(HAPI_SendSignalNumber(eHSIG_RequestReboot, 0) != ERR_OK)
	{
		HxLOG_Error("can't send reboot signal!![%s:%d]\n", __FUNCTION__, __LINE__);
	}
#endif
	HxLOG_Debug("[SHSEO] end HAPI_SendSignalNumber() -> eHSIG_RequestReboot\r\n");
}

void TRP_EVENT_Reboot(void)
{
	TRENTER;
	HxLOG_Debug("[SHSEO] do TRP_GFX_CreateWindow()\r\n");
	TRP_GFX_CreateWindow();
	HxLOG_Debug("[SHSEO] do TRP_GFX_DrawRebootDialog()\r\n");
	TRP_GFX_DrawRebootDialog();
	HxLOG_Debug("[SHSEO] do VK_TIMER_EventAfter()\r\n");
	VK_TIMER_EventAfter((unsigned long)TIME_30SEC, TRP_RebootCallback, (void *)NULL, (int)0, (unsigned long *)&s_ultimer);
	HxLOG_Debug("[SHSEO] end TRP_EVENT_Reboot()\r\n");
}

void	TRP_EVENT_FactoryReset(void)
{

}

//get download structure  type
HERROR TRP_GetDownloadInfo(trport_download_type_e eType, trport_download_info_t **pstInfo)
{
	if(eType >= eDOWNLOAD_TYPE_MAX || eType < eDOWNLOAD_TYPE_FIRMWARE)
	{
		HxLOG_Critical("wrong type(%d) or NULL Pointer [%s:%d]\n", eType, __FUNCTION__, __LINE__);
		TRLEAVE(ERR_FAIL);
	}

	*pstInfo = &stDownInfo[eType];
	TRLEAVE(ERR_OK);
}

HERROR TRP_GetDownloadPath(trport_download_type_e eType , HCHAR* filename, HUINT32 filesize, HCHAR **pszfullpath, HCHAR **pszid)
{
	HBOOL		ret;
	HERROR		hErr = ERR_OK;
	HCHAR 		szfullpath[TRPORT_DB_FULLPATH_LEN], szId[4];
	trport_download_info_t *pstInfo;

	TRENTER;
	CHECK_NULL(pszfullpath && pszid);

	//check state
	hErr = TRP_GetDownloadInfo(eType, &pstInfo);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("can't get download info [%s:%d]\n", __FUNCTION__, __LINE__);
		TRLEAVE(ERR_FAIL);
	}

	if(pstInfo->state != eDOWNLOAD_STATE_STANDBY)
	{
		HxLOG_Error("state problem! check! type:[%d] state:[%d]\n", eType, pstInfo->state);
		TRLEAVE(ERR_FAIL);
	}

	pstInfo->filesize = filesize;
	pstInfo->state = eDOWNLOAD_STATE_DOWNLOADING;

	switch(eType)
	{
	case eDOWNLOAD_TYPE_FIRMWARE:
		//stop rf ota
		hErr = APK_SQCSCENARIOJAPAN_StopRFOTA(&ret);
		if(hErr == ERR_FAIL || ret == FALSE)
		{
			HxLOG_Error("can't command to stop RF OTA[%s:%d]\n", __FUNCTION__, __LINE__);
			TRLEAVE(ERR_FAIL);
		}

		//firmware일경우 file의 형태로 받지 않기에.. file명과 path를 output 값으로 넘길 필요가 없다.
		pstInfo->filepath[0] = '\0';
		pstInfo->filename[0] = '\0';
		break;

	case eDOWNLOAD_TYPE_X_JCOM_CFG:
		HxSTD_snprintf(pstInfo->filepath, TRPORT_DB_PATH_LEN, "%s", TRPORT_TMP_CONFIG_PATH);
		HxSTD_snprintf(pstInfo->filename, TRPORT_DB_FILE_LEN, "%s", "xjcomcfg.zip");
		break;

	case eDOWNLOAD_TYPE_X_JLABS_AM_CFG:
		HxSTD_snprintf(pstInfo->filepath, TRPORT_DB_PATH_LEN, "%s", TRPORT_TMP_CONFIG_PATH);
		HxSTD_snprintf(pstInfo->filename, TRPORT_DB_FILE_LEN, "%s", "xjlabAM.zip");
		break;

	case eDOWNLOAD_TYPE_X_JCOM_PrivateMessage:
		HxSTD_snprintf(pstInfo->filepath, TRPORT_DB_PATH_LEN, "%s", TRPORT_TMP_CONFIG_PATH);
		HxSTD_snprintf(pstInfo->filename, TRPORT_DB_FILE_LEN, "%s", "xjlabPM.zip");
		break;

	default:
		HxLOG_Error("not supported  type(%d) [%s:%d]\n", eType, __FUNCTION__, __LINE__);
		break;
	}

	TRP_DB_StoreDownloadInfo(eType, pstInfo);

	HxSTD_snprintf(szfullpath, TRPORT_DB_FULLPATH_LEN, "%s%s", pstInfo->filepath, pstInfo->filename);
	HxSTD_snprintf(szId, 4, "%02d", eType);

	*pszfullpath = HLIB_STD_StrDup(szfullpath);
	*pszid = HLIB_STD_StrDup(szId);

	TRLEAVE(ERR_OK);
}

//db -> structure
HERROR TRP_LoadDownloadInfosFromDB(void)
{
	HUINT32 i;

	for(i=eDOWNLOAD_TYPE_FIRMWARE; i<eDOWNLOAD_TYPE_MAX; i++)
	{
		TRP_DB_GetDownloadInfo(i, &stDownInfo[i]);
		HxLOG_Debug("state:[%d] filepath&filenmae:[%s%s] filesize:[%-16d]\n", stDownInfo[i].state, stDownInfo[i].filepath, stDownInfo[i].filename, stDownInfo[i].filesize, __FUNCTION__, __LINE__);
	}

	TRLEAVE(ERR_OK);
}

