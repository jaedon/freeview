#if defined(CONFIG_WIFI) || defined(CONFIG_ETHERNET) || defined(CONFIG_BLUETOOTH) || defined(CONFIG_PPPOE)
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <ctype.h>	//for "isxdigit"
#include <stdlib.h>	//for "strtol"
#include "vkernel.h"
#include "htype.h"
#include "taskdef.h"
#include "cmdtool.h"
#include "di_err.h"
#include "drv_err.h"
#include "drv_network.h"
#if defined(CONFIG_WIFI)
#include "di_wifi.h"
#endif
#include "di_network.h"
#if defined(CONFIG_BLUETOOTH)
#include "di_bt.h"
#endif
#if defined(CONFIG_PPPOE)
#include "di_pppoe.h"
#endif


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define TEST_EXT_DEBUG
#define TEST_EXT_ENTER_LEAVE_DEBUG

#ifdef TEST_EXT_DEBUG
#define PrintDebug			VK_printf
#define PrintError			VK_printf
#else
#define PrintDebug			while (0) ((int (*)(char *, ...)) 0)
#define PrintError			VK_printf
#endif

#ifdef TEST_EXT_ENTER_LEAVE_DEBUG
#define PrintEnter				VK_printf("(+)%s\n", __FUNCTION__)
#define PrintExit				VK_printf("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter				while (0) ((int (*)(char *, ...)) 0)
#define PrintExit				while (0) ((int (*)(char *, ...)) 0)
#endif

#define AssertDI(arg1, arg2) if(arg1 != DI_ERR_OK) {VK_printf(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}

#define AssertDRV(arg1, arg2) if(arg1 != DRV_OK) {VK_printf(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}

#define UNUSED(x) (void)(x)

#define HWTEST_CMD		szCmd
#define HWTEST_PARAM	szParam
#define HWTEST_PARAM1	szParam1
#define HWTEST_PARAM2	szParam2
#define HWTEST_PARAM3	szParam3
#define HWTEST_PARAM4	szParam4
#define HWTEST_PARAM5	szParam5

#define GET_ARGS		int iResult=CMD_ERR; \
						char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM3=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM4=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM5=CMD_NextArg((char **)&szArgs);

#define CMD_IS(x)		(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)		(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)	(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)	(HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define PARAM3_IS(x)	(HWTEST_PARAM3!=NULL && VK_strcmp(HWTEST_PARAM3, x)==0)
#define PARAM4_IS(x)	(HWTEST_PARAM4!=NULL && VK_strcmp(HWTEST_PARAM4, x)==0)
#define PARAM5_IS(x)	(HWTEST_PARAM5!=NULL && VK_strcmp(HWTEST_PARAM5, x)==0)

#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)

#if defined(CONFIG_ETHERNET)
void CMD_HwTest_Parser_EthernetEventCallback(void *data)
{
	DI_NETWORK_CALLBACK_DATA_t *pstCallbackData;

	pstCallbackData = (DI_NETWORK_CALLBACK_DATA_t *)data;

	switch(pstCallbackData->eEvent)
	{
		case DI_NETWORK_EVENT_CABLE_CONNECT:
			CMD_Printf("== DEV[%d] : DI_NETWORK_EVENT_CABLE_CONNECT ==\n", pstCallbackData->ulDevId);
			break;
		case DI_NETWORK_EVENT_CABLE_DISCONNECT:
			CMD_Printf("== DEV[%d] : DI_NETWORK_EVENT_CABLE_DISCONNECT ==\n", pstCallbackData->ulDevId);
			break;
		case DI_NETWORK_EVENT_DHCP_SUCCESS:
			CMD_Printf("== DEV[%d] :  DI_NETWORK_EVENT_DHCP_SUCCESS ==\n", pstCallbackData->ulDevId);
			break;
		case DI_NETWORK_EVENT_DHCP_FAIL:
			CMD_Printf("== DEV[%d] :  DI_NETWORK_EVENT_DHCP_FAIL ==\n", pstCallbackData->ulDevId);
			break;
		case DI_NETWORK_EVENT_DHCP6_SUCCESS:
			CMD_Printf("== DEV[%d] :  DI_NETWORK_EVENT_DHCP6_SUCCESS ==\n", pstCallbackData->ulDevId);
			break;
		case DI_NETWORK_EVENT_DHCP6_FAIL:
			CMD_Printf("== DEV[%d] :  DI_NETWORK_EVENT_DHCP6_FAIL ==\n", pstCallbackData->ulDevId);
			break;
		case DI_NETWORK_EVENT_AUTOIP_SUCCESS:
			CMD_Printf("== DEV[%d] :  DI_NETWORK_EVENT_AUTOIP_SUCCESS ==\n", pstCallbackData->ulDevId);
			break;
		case DI_NETWORK_EVENT_AUTOIP_FAIL:
			CMD_Printf("== DEV[%d] :  DI_NETWORK_EVENT_AUTOIP_FAIL ==\n", pstCallbackData->ulDevId);
			break;
		default:
			CMD_Printf("== DEV[%d] :  eEvent[%d] ==\n", pstCallbackData->ulDevId, pstCallbackData->eEvent);
			break;
	}
}

int CMD_HwTest_Parser_Network(void *szArgs)
{
	GET_ARGS;

	UNUSED(szParam4);
	UNUSED(szParam5);

	if CMD_IS("info")
	{
		HUINT32 unCount, unCount2, i;
		DI_NETWORK_GET_INFO_t stGetInfo;
		HUINT8 dns[16] = {0, };
		HUINT8 defaultRoute[4] = {0, };
		HBOOL bIsUserSet;

		DI_NETWORK_GetDevCount(&unCount);
		for(i=0; i<unCount; i++)
		{
			VK_memset(&stGetInfo, 0, sizeof(DI_NETWORK_GET_INFO_t));
			VK_memset(defaultRoute, 0, 4);
			DI_NETWORK_GetDevInfo(i, &stGetInfo);
			CMD_Printf("============================================\n");
			CMD_Printf("eth[%d] name      : %s\n", i, stGetInfo.szName);
			CMD_Printf("eth[%d] dev type  : %d\n", i, stGetInfo.eDevType);
			CMD_Printf("eth[%d] mac addr  : %02X.%02X.%02X.%02X.%02X.%02X\n", i, stGetInfo.aucMacAddr[0], stGetInfo.aucMacAddr[1], stGetInfo.aucMacAddr[2], stGetInfo.aucMacAddr[3], stGetInfo.aucMacAddr[4], stGetInfo.aucMacAddr[5]);
			CMD_Printf("eth[%d] up        : %d\n", i, stGetInfo.bIsUp);
			if(stGetInfo.bIsUp)
			{
				CMD_Printf("eth[%d] connected : %d\n", i, stGetInfo.bIsConnected);
				CMD_Printf("eth[%d] dynamic   : %d\n", i, stGetInfo.bIsDynamic);
				CMD_Printf("eth[%d] ip addr   : %u.%u.%u.%u\n", i, stGetInfo.stIpv4Addr.aucIpAddr[0], stGetInfo.stIpv4Addr.aucIpAddr[1], stGetInfo.stIpv4Addr.aucIpAddr[2], stGetInfo.stIpv4Addr.aucIpAddr[3]);
				CMD_Printf("eth[%d] brd addr  : %u.%u.%u.%u\n", i, stGetInfo.stIpv4Addr.aucBcastAddr[0], stGetInfo.stIpv4Addr.aucBcastAddr[1], stGetInfo.stIpv4Addr.aucBcastAddr[2], stGetInfo.stIpv4Addr.aucBcastAddr[3]);
				CMD_Printf("eth[%d] net addr  : %u.%u.%u.%u\n", i, stGetInfo.stIpv4Addr.aucMaskAddr[0], stGetInfo.stIpv4Addr.aucMaskAddr[1], stGetInfo.stIpv4Addr.aucMaskAddr[2], stGetInfo.stIpv4Addr.aucMaskAddr[3]);
#if defined(CONFIG_IPV6)
				CMD_Printf("eth[%d] ip addr6  : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x/%d\n", i,
								 stGetInfo.stIpv6Addr.aucIpAddr[0], stGetInfo.stIpv6Addr.aucIpAddr[1],stGetInfo.stIpv6Addr.aucIpAddr[2],stGetInfo.stIpv6Addr.aucIpAddr[3],
								 stGetInfo.stIpv6Addr.aucIpAddr[4], stGetInfo.stIpv6Addr.aucIpAddr[5],stGetInfo.stIpv6Addr.aucIpAddr[6],stGetInfo.stIpv6Addr.aucIpAddr[7],
								 stGetInfo.stIpv6Addr.aucIpAddr[8], stGetInfo.stIpv6Addr.aucIpAddr[9],stGetInfo.stIpv6Addr.aucIpAddr[10],stGetInfo.stIpv6Addr.aucIpAddr[11],
								 stGetInfo.stIpv6Addr.aucIpAddr[12], stGetInfo.stIpv6Addr.aucIpAddr[13],stGetInfo.stIpv6Addr.aucIpAddr[14],stGetInfo.stIpv6Addr.aucIpAddr[15], stGetInfo.stIpv6Addr.ulPrefix);
#endif
			}
			DI_NETWORK_GetDefaultRoute(i, DI_NETWORK_IP_VER_4, defaultRoute);
			CMD_Printf("eth[%d] gate addr : %u.%u.%u.%u\n", i, defaultRoute[0], defaultRoute[1], defaultRoute[2], defaultRoute[3]);
			CMD_Printf("============================================\n\n");
		}

		CMD_Printf("============================================\n");
		DI_NETWORK_GetDnsMode(DI_NETWORK_IP_VER_4, &bIsUserSet);
		CMD_Printf("DNS v4 User Mode : %d\n", bIsUserSet);
#if defined(CONFIG_IPV6)
		DI_NETWORK_GetDnsMode(DI_NETWORK_IP_VER_6, &bIsUserSet);
		CMD_Printf("DNS v6 User Mode : %d\n", bIsUserSet);
#endif
		unCount = unCount2 = 0;
		DI_NETWORK_GetDnsCount(&unCount, &unCount2);
		CMD_Printf("DNS\n");
		for(i=0; i<unCount; i++)
		{
			DI_NETWORK_GetDnsInfo(DI_NETWORK_IP_VER_4, i, dns);
			CMD_Printf("%u.%u.%u.%u\n", dns[0], dns[1], dns[2], dns[3]);
		}
#if defined(CONFIG_IPV6)
		for(i=0; i<unCount2; i++)
		{
			DI_NETWORK_GetDnsInfo(DI_NETWORK_IP_VER_6, i, dns);
			CMD_Printf("%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X\n",
							dns[0], dns[1], dns[2], dns[3],
							dns[4], dns[5], dns[6], dns[7],
							dns[8], dns[9], dns[10], dns[11],
							dns[12], dns[13], dns[14], dns[15]);
		}
#endif
		CMD_Printf("============================================\n\n");
		iResult = CMD_OK;
	}
	else if CMD_IS("up")
	{
		int devId;

		READABLE_IN_DEC(HWTEST_PARAM, devId);

		DI_NETWORK_DevUp(devId);

		iResult = CMD_OK;
	}
	else if CMD_IS("down")
	{
		int devId;

		READABLE_IN_DEC(HWTEST_PARAM, devId);

		DI_NETWORK_DevDown(devId);

		iResult = CMD_OK;
	}
	else if CMD_IS("setmac")
	{
		int devId;
		int i;

		HUINT32 aulMacInput[6];
		HUINT8 aucMacAddr[6];

		READABLE_IN_DEC(HWTEST_PARAM, devId);

		if (HWTEST_PARAM1!=NULL)
		{
			VK_sscanf(HWTEST_PARAM1, "%X:%X:%X:%X:%X:%X", &aulMacInput[0], &aulMacInput[1], &aulMacInput[2], &aulMacInput[3], &aulMacInput[4], &aulMacInput[5]);
			for(i=0; i<6; i++)
				aucMacAddr[i] = (HUINT8)aulMacInput[i];
			DRV_NETWORK_SetMacAddr(devId, aucMacAddr);
		}

		iResult = CMD_OK;
	}
	else if CMD_IS("psetmac")
	{
		int devId;
		int i;

		HUINT32 aulMacInput[6];
		HUINT8 aucMacAddr[6];

		READABLE_IN_DEC(HWTEST_PARAM, devId);

		if (HWTEST_PARAM1!=NULL)
		{
			VK_sscanf(HWTEST_PARAM1, "%X:%X:%X:%X:%X:%X", &aulMacInput[0], &aulMacInput[1], &aulMacInput[2], &aulMacInput[3], &aulMacInput[4], &aulMacInput[5]);
			for(i=0; i<6; i++)
				aucMacAddr[i] = (HUINT8)aulMacInput[i];
			DRV_NETWORK_WriteMacAddrToNvram(devId, aucMacAddr);
			DRV_NETWORK_SetMacAddr(devId, aucMacAddr);
		}

		iResult = CMD_OK;
	}
	else if CMD_IS("setaddr")
	{
		int devId;
		DI_NETWORK_GET_INFO_t stGetInfo;
		DI_NETWORK_SET_INFO_t stSetInfo;

		READABLE_IN_DEC(HWTEST_PARAM, devId);

		VK_memset(&stGetInfo, 0, sizeof(DI_NETWORK_GET_INFO_t));
		VK_memset(&stSetInfo, 0, sizeof(DI_NETWORK_SET_INFO_t));

		DI_NETWORK_GetDevInfo(devId, &stGetInfo);
		VK_memcpy(&stSetInfo.stIpv4Addr, &stGetInfo.stIpv4Addr, sizeof(DI_NETWORK_IPV4_ADDR_t));

		if (HWTEST_PARAM1!=NULL)
		{
			unsigned int iIndex;
			int iStartPos;
			int iIpIndex=0;
			unsigned int iVal;
			char caData[10];

			for (iIndex=0, iStartPos=0 ; iIndex<VK_strlen(HWTEST_PARAM1) ; iIndex++)
			{
				if (HWTEST_PARAM1[iIndex]=='.' || iIndex==VK_strlen(HWTEST_PARAM1)-1)
				{
					VK_memset(caData, 0, 10);
					if (iIndex==VK_strlen(HWTEST_PARAM1)-1)
						VK_memcpy(caData, &HWTEST_PARAM1[iStartPos], iIndex-iStartPos+1);
					else
						VK_memcpy(caData, &HWTEST_PARAM1[iStartPos], iIndex-iStartPos);

					VK_sscanf(caData, "%u", &iVal);
					stSetInfo.stIpv4Addr.aucIpAddr[iIpIndex++]=iVal;
					iStartPos=iIndex+1;
				}
			}
		}

		if (HWTEST_PARAM2!=NULL)
		{
			unsigned int iIndex;
			int iStartPos;
			int iIpIndex=0;
			unsigned int iVal;
			char caData[10];

			for (iIndex=0, iStartPos=0 ; iIndex<VK_strlen(HWTEST_PARAM2) ; iIndex++)
			{
				if (HWTEST_PARAM2[iIndex]=='.' || iIndex==VK_strlen(HWTEST_PARAM2)-1)
				{
					VK_memset(caData, 0, 10);
					if (iIndex==VK_strlen(HWTEST_PARAM2)-1)
						VK_memcpy(caData, &HWTEST_PARAM2[iStartPos], iIndex-iStartPos+1);
					else
						VK_memcpy(caData, &HWTEST_PARAM2[iStartPos], iIndex-iStartPos);

					VK_sscanf(caData, "%u", &iVal);
					stSetInfo.stIpv4Addr.aucBcastAddr[iIpIndex++]=iVal;
					iStartPos=iIndex+1;
				}
			}
		}

		if (HWTEST_PARAM3!=NULL)
		{
			unsigned int iIndex;
			int iStartPos;
			int iIpIndex=0;
			unsigned int iVal;
			char caData[10];

			for (iIndex=0, iStartPos=0 ; iIndex<VK_strlen(HWTEST_PARAM3) ; iIndex++)
			{
				if (HWTEST_PARAM3[iIndex]=='.' || iIndex==VK_strlen(HWTEST_PARAM3)-1)
				{
					VK_memset(caData, 0, 10);
					if (iIndex==VK_strlen(HWTEST_PARAM3)-1)
						VK_memcpy(caData, &HWTEST_PARAM3[iStartPos], iIndex-iStartPos+1);
					else
						VK_memcpy(caData, &HWTEST_PARAM3[iStartPos], iIndex-iStartPos);

					VK_sscanf(caData, "%u", &iVal);
					stSetInfo.stIpv4Addr.aucMaskAddr[iIpIndex++]=iVal;
					iStartPos=iIndex+1;
				}
			}
		}
		DI_NETWORK_SetDevInfo(devId, &stSetInfo);
		iResult = CMD_OK;
	}
	else if CMD_IS("setroute")
	{
		int devId;
		HUINT8 aucGateway[4];

		READABLE_IN_DEC(HWTEST_PARAM, devId);

		if (HWTEST_PARAM1!=NULL)
		{
			unsigned int iIndex;
			int iStartPos;
			int iIpIndex=0;
			unsigned int iVal;
			char caData[10];

			for (iIndex=0, iStartPos=0 ; iIndex<VK_strlen(HWTEST_PARAM1) ; iIndex++)
			{
				if (HWTEST_PARAM1[iIndex]=='.' || iIndex==VK_strlen(HWTEST_PARAM1)-1)
				{
					VK_memset(caData, 0, 10);
					if (iIndex==VK_strlen(HWTEST_PARAM1)-1)
						VK_memcpy(caData, &HWTEST_PARAM1[iStartPos], iIndex-iStartPos+1);
					else
						VK_memcpy(caData, &HWTEST_PARAM1[iStartPos], iIndex-iStartPos);

					VK_sscanf(caData, "%u", &iVal);
					aucGateway[iIpIndex++]=iVal;
					iStartPos=iIndex+1;
				}
			}
		}
		DI_NETWORK_SetDefaultRoute(devId, DI_NETWORK_IP_VER_4, aucGateway);

		iResult = CMD_OK;
	}
	else if CMD_IS("setdns")
	{
		HUINT8 dns[8] = {0, };
		HUINT32 count = 0;

		if (HWTEST_PARAM!=NULL)
		{
			unsigned int iIndex;
			int iStartPos;
			int iIpIndex=0;
			unsigned int iVal;
			char caData[10];

			for (iIndex=0, iStartPos=0 ; iIndex<VK_strlen(HWTEST_PARAM) ; iIndex++)
			{
				if (HWTEST_PARAM[iIndex]=='.' || iIndex==VK_strlen(HWTEST_PARAM)-1)
				{
					VK_memset(caData, 0, 10);
					if (iIndex==VK_strlen(HWTEST_PARAM)-1)
						VK_memcpy(caData, &HWTEST_PARAM[iStartPos], iIndex-iStartPos+1);
					else
						VK_memcpy(caData, &HWTEST_PARAM[iStartPos], iIndex-iStartPos);

					VK_sscanf(caData, "%u", &iVal);
					dns[iIpIndex++]=iVal;
					iStartPos=iIndex+1;
				}
			}
			count++;
			DI_UART_Print("dns : %u.%u.%u.%u\n", dns[0], dns[1], dns[2], dns[3]);
		}

		if (HWTEST_PARAM1!=NULL)
		{
			unsigned int iIndex;
			int iStartPos;
			int iIpIndex=0;
			unsigned int iVal;
			char caData[10];

			for (iIndex=0, iStartPos=0 ; iIndex<VK_strlen(HWTEST_PARAM1) ; iIndex++)
			{
				if (HWTEST_PARAM1[iIndex]=='.' || iIndex==VK_strlen(HWTEST_PARAM1)-1)
				{
					VK_memset(caData, 0, 10);
					if (iIndex==VK_strlen(HWTEST_PARAM1)-1)
						VK_memcpy(caData, &HWTEST_PARAM1[iStartPos], iIndex-iStartPos+1);
					else
						VK_memcpy(caData, &HWTEST_PARAM1[iStartPos], iIndex-iStartPos);

					VK_sscanf(caData, "%u", &iVal);
					dns[4+iIpIndex++]=iVal;
					iStartPos=iIndex+1;
				}
			}
			count++;
			DI_UART_Print("dns : %u.%u.%u.%u\n", dns[4], dns[5], dns[6], dns[7]);
		}

		DI_NETWORK_SetDnsInfo(count, dns, 0, NULL);

		iResult = CMD_OK;
	}
	else if CMD_IS("setdnsmode")
	{
		if PARAM_IS("user")
		{
			DI_NETWORK_SetDnsMode(DI_NETWORK_IP_VER_4, TRUE);
		}
		else
		{
			DI_NETWORK_SetDnsMode(DI_NETWORK_IP_VER_4, FALSE);
		}

		iResult = CMD_OK;
	}
	else if CMD_IS("dhcpc")
	{
		int devId;
		char *szVendorClass = "dslforum.org";

		READABLE_IN_DEC(HWTEST_PARAM, devId);

		if PARAM1_IS("start")
		{
			DI_NETWORK_DHCPC_PARAMS_t stDhcpParams;

			stDhcpParams.ulWaitSeconds = 30;
			stDhcpParams.bUseVendorClass = TRUE;
			VK_strncpy(stDhcpParams.aucVendorClass, szVendorClass, VK_strlen(szVendorClass)+1);

			DI_NETWORK_RegisterEventCallback(devId, DI_NETWORK_EVENT_DHCP_SUCCESS, CMD_HwTest_Parser_EthernetEventCallback);
			DI_NETWORK_RegisterEventCallback(devId, DI_NETWORK_EVENT_DHCP_FAIL, CMD_HwTest_Parser_EthernetEventCallback);
			DI_NETWORK_StartDhcpClient(devId, DI_NETWORK_IP_VER_4, &stDhcpParams);
		}
		else if PARAM1_IS("stop")
		{
			DI_NETWORK_StopDhcpClient(devId, DI_NETWORK_IP_VER_4);
		}

		iResult = CMD_OK;
	}
	else if CMD_IS("dhcps")
	{
		int devId;
		DI_NETWORK_DHCPS_PARAMS_t stParams = {
			{192, 168, 0, 20}, 	// start
			{192, 168, 0, 254},  	// end
			{{192, 168, 10, 2}, 	// dns1
			{192, 168, 10, 10}, 	// dns2
			{0, 0, 0, 0}}, 		// dns3
			{255, 255, 255, 0},	// subnet
			{192, 168, 10, 2}, 	// router
			"local", 		// domain
			864000, 		// lease
			{0x00, }, // ipv6 start
			{0x00, },  // ipv6 end
			{0x00, },  // ipv6 dns
			{0x00, },  // ipv6 subnet
			0 // ipv6 prefix
		};
		READABLE_IN_DEC(HWTEST_PARAM, devId);

		if PARAM1_IS("start")
		{
			DI_NETWORK_StartDhcpServer(devId, DI_NETWORK_IP_VER_4, &stParams);
		}
		else if PARAM1_IS("stop")
		{
			DI_NETWORK_StopDhcpServer(devId, DI_NETWORK_IP_VER_4);
		}

		iResult = CMD_OK;
	}
#if defined(CONFIG_IPV6)
	else if CMD_IS("setaddr6")
	{
		int devId;
		DI_NETWORK_GET_INFO_t stGetInfo;
		DI_NETWORK_SET_INFO_t stSetInfo;
		HUINT8 ipaddr6[16] = {0x3f, 0xfe, 0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x02};

		READABLE_IN_DEC(HWTEST_PARAM, devId);

		VK_memset(&stGetInfo, 0, sizeof(DI_NETWORK_GET_INFO_t));
		VK_memset(&stSetInfo, 0, sizeof(DI_NETWORK_SET_INFO_t));

		DI_NETWORK_GetDevInfo(devId, &stGetInfo);
		VK_memcpy(&stSetInfo.stIpv4Addr, &stGetInfo.stIpv4Addr, sizeof(DI_NETWORK_IPV4_ADDR_t));
		stSetInfo.eIpv6AddrOp = DI_NETWORK_IPV6_ADDR_ADD;
		VK_memcpy(stSetInfo.stIpv6Addr.aucIpAddr, ipaddr6, 16);
		stSetInfo.stIpv6Addr.ulPrefix = 64;
		DI_NETWORK_SetDevInfo(devId, &stSetInfo);

		iResult = CMD_OK;
	}
	else if CMD_IS("dhcp6c_stateful")
	{
		int devId;

		READABLE_IN_DEC(HWTEST_PARAM, devId);

		if PARAM1_IS("start")
		{
			DI_NETWORK_DHCPC_PARAMS_t stDhcpParams;

			stDhcpParams.ulWaitSeconds = 30;
			stDhcpParams.eMode = DI_NETWORK_IPV6_DHCP_STATEFUL;
			DI_NETWORK_RegisterEventCallback(devId, DI_NETWORK_EVENT_DHCP6_SUCCESS, CMD_HwTest_Parser_EthernetEventCallback);
			DI_NETWORK_RegisterEventCallback(devId, DI_NETWORK_EVENT_DHCP6_FAIL, CMD_HwTest_Parser_EthernetEventCallback);
			DI_NETWORK_StartDhcpClient(devId, DI_NETWORK_IP_VER_6, &stDhcpParams);
		}
		else if PARAM1_IS("stop")
		{
			DI_NETWORK_StopDhcpClient(devId, DI_NETWORK_IP_VER_6);
		}

		iResult = CMD_OK;
	}
	else if CMD_IS("dhcp6c_stateless")
	{
		int devId;

		READABLE_IN_DEC(HWTEST_PARAM, devId);

		if PARAM1_IS("start")
		{
			DI_NETWORK_DHCPC_PARAMS_t stDhcpParams;

			stDhcpParams.ulWaitSeconds = 30;
			stDhcpParams.eMode = DI_NETWORK_IPV6_DHCP_STATELESS;
			DI_NETWORK_RegisterEventCallback(devId, DI_NETWORK_EVENT_DHCP6_SUCCESS, CMD_HwTest_Parser_EthernetEventCallback);
			DI_NETWORK_RegisterEventCallback(devId, DI_NETWORK_EVENT_DHCP6_FAIL, CMD_HwTest_Parser_EthernetEventCallback);
			DI_NETWORK_StartDhcpClient(devId, DI_NETWORK_IP_VER_6, &stDhcpParams);
		}
		else if PARAM1_IS("stop")
		{
			DI_NETWORK_StopDhcpClient(devId, DI_NETWORK_IP_VER_6);
		}

		iResult = CMD_OK;
	}
	else if CMD_IS("dhcp6s")
	{
		int devId;
		DI_NETWORK_DHCPS_PARAMS_t stParams = {
			{0, 0, 0, 0}, 	// start
			{0, 0, 0, 0},  	// end
			{{0, 0, 0, 0}, 	// dns1
			{0, 0, 0, 0}, 	// dns2
			{0, 0, 0, 0}}, 	// dns3
			{0, 0, 0, 0},	// subnet
			{0, 0, 0, 0}, 	// router
			"test.example.com", 		// domain
			2592000, 			// lease
			{0x3f, 0xfe, 0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10}, // ipv6 start
			{0x3f, 0xfe, 0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00}, // ipv6 end
			{0x3f, 0xfe, 0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x20, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x3f, 0x3e}, // ipv6 dns
			{0x3f, 0xfe, 0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // ipv6 subnet
			64 // ipv6 prefix
		};
		READABLE_IN_DEC(HWTEST_PARAM, devId);

		if PARAM1_IS("start")
		{
			DI_NETWORK_StartDhcpServer(devId, DI_NETWORK_IP_VER_6, &stParams);
		}
		else if PARAM1_IS("stop")
		{
			DI_NETWORK_StopDhcpServer(devId, DI_NETWORK_IP_VER_6);
		}

		iResult = CMD_OK;
	}
#endif
	else if CMD_IS("regcb")
	{
		int devId;

		READABLE_IN_DEC(HWTEST_PARAM, devId);

		DI_NETWORK_RegisterEventCallback(devId, DI_NETWORK_EVENT_CABLE_CONNECT, CMD_HwTest_Parser_EthernetEventCallback);
		DI_NETWORK_RegisterEventCallback(devId, DI_NETWORK_EVENT_CABLE_DISCONNECT, CMD_HwTest_Parser_EthernetEventCallback);
		DI_NETWORK_RegisterEventCallback(devId, DI_NETWORK_EVENT_DHCP_SUCCESS, CMD_HwTest_Parser_EthernetEventCallback);
		DI_NETWORK_RegisterEventCallback(devId, DI_NETWORK_EVENT_DHCP_SUCCESS, CMD_HwTest_Parser_EthernetEventCallback);
		DI_NETWORK_RegisterEventCallback(devId, DI_NETWORK_EVENT_AUTOIP_SUCCESS, CMD_HwTest_Parser_EthernetEventCallback);
		DI_NETWORK_RegisterEventCallback(devId, DI_NETWORK_EVENT_AUTOIP_FAIL, CMD_HwTest_Parser_EthernetEventCallback);
		CMD_Printf("Register cable connect/disconnect event callback - dev[%d]\n", devId);

		iResult = CMD_OK;
	}
	else if CMD_IS("setportforward")
	{
		int devId;

		READABLE_IN_DEC(HWTEST_PARAM, devId);

		DI_NETWORK_SetPortForward(devId, DI_NETWORK_IP_VER_4);

		iResult = CMD_OK;
	}
	else if CMD_IS("clearportforward")
	{
		int devId;

		READABLE_IN_DEC(HWTEST_PARAM, devId);

		DI_NETWORK_ClearPortForward(devId, DI_NETWORK_IP_VER_4);

		iResult = CMD_OK;
	}
	else if CMD_IS("autoip")
	{
		int devId;

		READABLE_IN_DEC(HWTEST_PARAM, devId);

		DI_NETWORK_StartAutoIp(devId);

		iResult = CMD_OK;
	}
	else if CMD_IS("dhcpopt")
	{
		int devId;
		DI_NETWORK_DHCP_OPTIONS_t stOptions;

		READABLE_IN_DEC(HWTEST_PARAM, devId);

		DI_NETWORK_GetDhcpOptions(devId, DI_NETWORK_IP_VER_4, &stOptions);
		CMD_Printf("aucAcsUrl = %s\n", stOptions.aucAcsUrl);
		CMD_Printf("aucProvisioningCode = %s\n", stOptions.aucProvisioningCode);
		CMD_Printf("ulCWMPRetryMinWait = %d\n", stOptions.ulCWMPRetryMinWait);
		CMD_Printf("ulCWMPRetryIntervalMulti = %d\n", stOptions.ulCWMPRetryIntervalMulti);

		iResult = CMD_OK;
	}
#if defined(CONFIG_WAKE_ON_LAN)
	else if CMD_IS("wol")
	{
		if PARAM_IS("on")
		{
			CMD_Printf("Command :  net wol on-> Eth0 Wake on Lan ON\n");

			iResult = DI_NETWORK_SetWakeOnLAN(0, TRUE);
			AssertDI(iResult, CMD_ERR);
		}
		else if PARAM_IS("off")
		{
			CMD_Printf("Command : net wol off-> Eth0 Wake on Lan OFF\n");

			iResult = DI_NETWORK_SetWakeOnLAN(0, FALSE);
			AssertDI(iResult, CMD_ERR);
		}
		else
		{
			CMD_Printf("Wrong Command~! right command : net wol on/off \r\n");
			return CMD_ERR;
		}
	}
#endif
	else if CMD_IS("amount")
	{
		int devId;
		DI_NETWORK_DATA_AMOUNT_INFO_t stGetStatus;

		READABLE_IN_DEC(HWTEST_PARAM, devId);

		DI_NETWORK_GetDevDataAmountInfo(devId, &stGetStatus);

		CMD_Printf("[%d] Bytes(RX : %u, TX : %u) Packes(RX : %u, TX :%u) \n"
						,devId,stGetStatus.ulBytesReceive,stGetStatus.ulBytesSend,stGetStatus.ulPacketsReceive,stGetStatus.ulPacketsSend);

		iResult = CMD_OK;
	}
	else
	{
		CMD_Printf("Command : net : unknown parameter\n");
		CMD_Printf("  ex) net info\n");

		return 0;
	}
	return iResult;
}
#endif

#if defined(CONFIG_WIFI)
static HINT8 * P_WIFI_LogSecureInfo( DI_WIFI_SECURE_e eSecure )
{
	HINT8 *retStr = (HINT8 *)"UNKNOWN";
	switch ( (HUINT32)eSecure )
	{
		case DI_WIFI_SECURE_OPEN :
			retStr = (HINT8 *)"OPEN";
			break;
		case DI_WIFI_SECURE_WEP :
			retStr = (HINT8 *)"WEP";
			break;
		case DI_WIFI_SECURE_WPA :
			retStr = (HINT8 *)"WPA";
			break;
		case DI_WIFI_SECURE_WPA2 :
			retStr = (HINT8 *)"WPA2";
			break;
		case DI_WIFI_SECURE_WPA_WPA2:
			retStr = (HINT8 *)"WPA_WPA2";
			break;
		case DI_WIFI_SECURE_UNKNOWN:
			retStr = (HINT8 *)"UNKNOWN";
			break;
	}
	return retStr;
}

static HINT8 * P_WIFI_LogAuthenInfo( DI_WIFI_AUTHEN_e eAuthen )
{
	HINT8 *retStr = (HINT8 *)"UNKNOWN";
	switch ( (HUINT32)eAuthen )
	{
		case DI_WIFI_AUTHEN_OPEN :
			retStr = (HINT8 *)"NONE";
			break;
		case DI_WIFI_AUTHEN_PSK :
			retStr = (HINT8 *)"PSK";
			break;
		case DI_WIFI_AUTHEN_802_1X :
			retStr = (HINT8 *)"802.1x";
			break;
		case DI_WIFI_AUTHEN_UNKNOWN :
			retStr = (HINT8 *)"UNKNOWN";
			break;
	}
	return retStr;
}

static HINT8 * P_WIFI_LogEncryptInfo( DI_WIFI_ENCRYPT_e eEncrypt )
{
	HINT8 *retStr = (HINT8 *)"UNKNOWN";
	switch ( (HUINT32)eEncrypt ) {
		case DI_WIFI_ENCRYPT_NONE :
			retStr = (HINT8 *)"NONE";
			break;
		case DI_WIFI_ENCRYPT_WEP_64 :
			retStr = (HINT8 *)"WEP64";
			break;
		case DI_WIFI_ENCRYPT_TKIP :
			retStr = (HINT8 *)"TKIP";
			break;
		case DI_WIFI_ENCRYPT_AES :
			retStr = (HINT8 *)"AES";
			break;
		case DI_WIFI_ENCRYPT_TKIP_AES :
			retStr = (HINT8 *)"TKIP_AES";
			break;
		case DI_WIFI_ENCRYPT_UNKNOWN:
			retStr = (HINT8 *)"UNKNOWN";
			break;
	}
	return retStr;
}

void CMD_HwTest_Parser_WiFiEventCallback(void *data)
{
	DI_WIFI_CALLBACK_DATA_t *pstCallbackData;
	unsigned int i = 0;
	DI_WIFI_INFO_t	*pApInfo;

	pstCallbackData = (DI_WIFI_CALLBACK_DATA_t *)data;
	pApInfo = pstCallbackData->pvEventData;

	switch(pstCallbackData->eEvent)
	{
		case DI_WIFI_NOTIFY_EVT_INSERTED:
			CMD_Printf("== DEV[%d] : DI_WIFI_NOTIFY_EVT_INSERTED ==\n", pstCallbackData->ulDevId);
			break;
		case DI_WIFI_NOTIFY_EVT_SCAN_SUCCESS:
			CMD_Printf("== DEV[%d] : DI_WIFI_NOTIFY_EVT_SCAN_SUCCESS ==\n", pstCallbackData->ulDevId);
			for ( i = 0; i < pstCallbackData->ulNumOfData ; i++ )
			{
				CMD_Printf("  --------------- AP %2d Info -------------\n", i);
				CMD_Printf("  * ESSID	 : %s\n", pApInfo[i].aucESSID);
				CMD_Printf("  * MAC	 : 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x\n", pApInfo[i].aucMAC[0],pApInfo[i].aucMAC[1],pApInfo[i].aucMAC[2]
					,pApInfo[i].aucMAC[3],pApInfo[i].aucMAC[4],pApInfo[i].aucMAC[5]);
				CMD_Printf("  * Channel  : %d\n", pApInfo[i].ulChannel);
				CMD_Printf("  * freq  : %d\n", pApInfo[i].ulFreq);
				CMD_Printf("  * WPS 	: %d\n", pApInfo[i].bWPS);
				CMD_Printf("  * opMode	 : %d\n", pApInfo[i].eOpMode);
				CMD_Printf("  * SECURE  : %s\n", P_WIFI_LogSecureInfo( pApInfo[i].eSecureType) );
				CMD_Printf("  * AUTHEN  : %s %d\n", P_WIFI_LogAuthenInfo( pApInfo[i].eAuthenType),pApInfo[i].eAuthenType );
				CMD_Printf("  * ENCRYPT : %s\n", P_WIFI_LogEncryptInfo( pApInfo[i].eEncryptType) );
				CMD_Printf("  * waveStrength	 : %d\n", pApInfo[i].nWaveStrength);
				CMD_Printf("  * BitRate	 : %d\n", pApInfo[i].nBitRate);
				CMD_Printf("  \n");
			}
			break;
		case DI_WIFI_NOTIFY_EVT_SCAN_FAIL:
			CMD_Printf("== DEV[%d] :  DI_WIFI_NOTIFY_EVT_SCAN_FAIL ==\n", pstCallbackData->ulDevId);
			break;
		case DI_WIFI_NOTIFY_EVT_AUTHEN_SUCCESS:
			CMD_Printf("== DEV[%d] :  DI_WIFI_NOTIFY_EVT_AUTHEN_SUCCESS ==\n", pstCallbackData->ulDevId);
			break;
		case DI_WIFI_NOTIFY_EVT_AUTHEN_FAIL:
			CMD_Printf("== DEV[%d] :  DI_WIFI_NOTIFY_EVT_AUTHEN_FAIL ==\n", pstCallbackData->ulDevId);
			break;
		case DI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS:
			CMD_Printf("== DEV[%d] :  DI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS ==\n", pstCallbackData->ulDevId);
			break;
		case DI_WIFI_NOTIFY_EVT_CONNECT_FAIL:
			CMD_Printf("== DEV[%d] :  DI_WIFI_NOTIFY_EVT_CONNECT_FAIL ==\n", pstCallbackData->ulDevId);
			break;
		default:
			CMD_Printf("== DEV[%d] :  eEvent[%d] ==\n", pstCallbackData->ulDevId, pstCallbackData->eEvent);
			break;
	}
}


int CMD_HwTest_Parser_WiFi(void *szArgs)
{
	GET_ARGS;
	static HBOOL s_bWiFiInit = FALSE;
	static HUINT32 	ulWiFiDev[2] = {0xFF,0xFF};
	HUINT32 ulDevId;
	HUINT32 ulTimeout;
	DI_ERR_CODE err = 0;
	HUINT32 ulLoop = 0;
	int nIndex = 0;
	extern DRV_NETWORK_DEV_INFO_t s_stDevList[];
#if defined(CONFIG_SUPPORT_SECDMA) && defined(CONFIG_WIFI_SCRIPT_TEST)
	HCHAR acCmdBuf[64] = {0,};
#endif

	if(s_bWiFiInit == FALSE)
	{
		for(ulLoop = 0; ulLoop < DRV_CFG_GetNetMaxDevice(); ulLoop++)
		{
			if(s_stDevList[ulLoop].eDevType == DI_NETWORK_DEV_WIFI)
			{
				if(nIndex < 2)
				{
					ulWiFiDev[nIndex] = s_stDevList[ulLoop].ulDevId;
#if !(defined(CONFIG_SUPPORT_SECDMA) && defined(CONFIG_WIFI_SCRIPT_TEST))
					DI_WIFI_RegisterEventCallback(ulWiFiDev[nIndex],DI_WIFI_NOTIFY_EVT_INSERTED,CMD_HwTest_Parser_WiFiEventCallback);
					DI_WIFI_RegisterEventCallback(ulWiFiDev[nIndex],DI_WIFI_NOTIFY_EVT_EXTRACTED,CMD_HwTest_Parser_WiFiEventCallback);
					DI_WIFI_RegisterEventCallback(ulWiFiDev[nIndex],DI_WIFI_NOTIFY_EVT_SCAN_SUCCESS,CMD_HwTest_Parser_WiFiEventCallback);
					DI_WIFI_RegisterEventCallback(ulWiFiDev[nIndex],DI_WIFI_NOTIFY_EVT_SCAN_FAIL,CMD_HwTest_Parser_WiFiEventCallback);
					DI_WIFI_RegisterEventCallback(ulWiFiDev[nIndex],DI_WIFI_NOTIFY_EVT_AUTHEN_SUCCESS,CMD_HwTest_Parser_WiFiEventCallback);
					DI_WIFI_RegisterEventCallback(ulWiFiDev[nIndex],DI_WIFI_NOTIFY_EVT_AUTHEN_FAIL,CMD_HwTest_Parser_WiFiEventCallback);
					DI_WIFI_RegisterEventCallback(ulWiFiDev[nIndex],DI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS,CMD_HwTest_Parser_WiFiEventCallback);
					DI_WIFI_RegisterEventCallback(ulWiFiDev[nIndex],DI_WIFI_NOTIFY_EVT_CONNECT_FAIL,CMD_HwTest_Parser_WiFiEventCallback);
#endif
				}
				else
				{
					CMD_Printf("### MAX WIFI NOT Support \n");
				}
				nIndex++;
			}
		}
		s_bWiFiInit = TRUE;
	}

	if (CMD_IS("scan"))
	{
		if ((HWTEST_PARAM == NULL) ||			/* Device ID */
			(!READABLE_IN_DEC(HWTEST_PARAM, ulDevId)) ||
			(ulDevId > 1))
		{
			iResult = CMD_ERR;
			goto TEST_WIFI_SCAN_EXIT;
		}

		err = DI_WIFI_Scan(ulWiFiDev[ulDevId]);
		if(err != 0)
		{
			CMD_Printf("ERR] DI_WIFI_Scan \n\r");
		}
		iResult = CMD_OK;
	}
	else if (CMD_IS("open"))
	{
		DI_WIFI_INFO_t ApInfo;

		if ((HWTEST_PARAM == NULL) ||			/* Device ID */
			(HWTEST_PARAM1 == NULL) ||			/* SSID */
			(HWTEST_PARAM2 == NULL) ||			/* Time-out */
			(!READABLE_IN_DEC(HWTEST_PARAM, ulDevId)) ||
			(ulDevId > 1) ||
			(!READABLE_IN_DEC(HWTEST_PARAM2, ulTimeout)) ||
			(ulTimeout < 10))
		{
			iResult = CMD_ERR;
			goto TEST_WIFI_OPEN_EXIT;
		}

		CMD_Printf("Security       : OPEN\n");
		CMD_Printf("Authentication : OPEN\n");
		CMD_Printf("Encryption     : NONE\n");

		VK_MEM_Memset(&ApInfo,0x00,sizeof(DI_WIFI_INFO_t));
		VK_snprintf(ApInfo.aucESSID,sizeof(ApInfo.aucESSID),"%s",HWTEST_PARAM1);

		ApInfo.eAuthenType	= DI_WIFI_AUTHEN_OPEN;
		ApInfo.eEncryptType = DI_WIFI_ENCRYPT_NONE;
		ApInfo.eSecureType	= DI_WIFI_SECURE_OPEN;

		DI_WIFI_Connect(ulWiFiDev[ulDevId],DI_WIFI_WPS_NONE,&ApInfo,ulTimeout);
		iResult = CMD_OK;
	}
	else if (CMD_IS("wep64a") || CMD_IS("wep64h") || CMD_IS("wep128a") || CMD_IS("wep128h"))
	{
		HINT32 nKeyLen = 0;
		DI_WIFI_INFO_t ApInfo;

#if defined(CONFIG_QV940)
		iResult = CMD_ERR;
		CMD_Printf("Not support!!\n\r");
		goto TEST_WIFI_WEP_EXIT;
#endif

		if ((HWTEST_PARAM == NULL) ||			/* Device ID */
			(HWTEST_PARAM1 == NULL) ||			/* SSID */
			(HWTEST_PARAM2 == NULL) ||			/* WEP Key */
			(HWTEST_PARAM3 == NULL) ||			/* Time-out */
			(!READABLE_IN_DEC(HWTEST_PARAM, ulDevId)) ||
			(ulDevId > 1) ||
			(!READABLE_IN_DEC(HWTEST_PARAM3, ulTimeout)) ||
			(ulTimeout < 10))
		{
			iResult = CMD_ERR;
			goto TEST_WIFI_WEP_EXIT;
		}

		nKeyLen = VK_strlen(HWTEST_PARAM2);
		if ((CMD_IS("wep64a") && (nKeyLen != 5)) ||
			(CMD_IS("wep64h") && (nKeyLen != 10)) ||
			(CMD_IS("wep128a") && (nKeyLen != 13)) ||
			(CMD_IS("wep128h") && (nKeyLen != 26)))
		{
			CMD_Printf("WEP key length (%d) is not valid!\n",nKeyLen);
			iResult = CMD_ERR;
			goto TEST_WIFI_WEP_EXIT;
		}

		VK_MEM_Memset(&ApInfo,0x00,sizeof(DI_WIFI_INFO_t));
		VK_snprintf(ApInfo.aucESSID,sizeof(ApInfo.aucESSID),"%s",HWTEST_PARAM1);
		VK_snprintf(ApInfo.aucKey,sizeof(ApInfo.aucKey),"%s",HWTEST_PARAM2);

		CMD_Printf("Security       : WEP\n");
		ApInfo.eSecureType	= DI_WIFI_SECURE_WEP;

		CMD_Printf("Authentication : OPEN\n");
		ApInfo.eAuthenType	= DI_WIFI_AUTHEN_OPEN;

		if (CMD_IS("wep64a"))
		{
			CMD_Printf("Encryption     : WEP64_ASCII\n");
			ApInfo.eEncryptType = DI_WIFI_ENCRYPT_WEP_64;
		}
		else if (CMD_IS("wep64h"))
		{
			CMD_Printf("Encryption     : WEP64_HEX\n");
			ApInfo.eEncryptType = DI_WIFI_ENCRYPT_WEP_64HEX;
		}
		else if (CMD_IS("wep128a"))
		{
			CMD_Printf("Encryption     : WEP128_ASCII\n");
			ApInfo.eEncryptType = DI_WIFI_ENCRYPT_WEP_128;
		}
		else
		{
			CMD_Printf("Encryption     : WEP128_HEX\n");
			ApInfo.eEncryptType = DI_WIFI_ENCRYPT_WEP_128HEX;
		}

		DI_WIFI_Connect(ulWiFiDev[ulDevId],DI_WIFI_WPS_NONE,&ApInfo,ulTimeout);
		iResult = CMD_OK;
	}
	else if (CMD_IS("wpa") || CMD_IS("wpa2")|| CMD_IS("wpawpa2"))
	{
		HINT32 nKeyLen = 0;
		DI_WIFI_INFO_t ApInfo;

		if ((HWTEST_PARAM == NULL) ||			/* Device ID */
			(HWTEST_PARAM1 == NULL) ||			/* SSID */
			(HWTEST_PARAM2 == NULL) ||			/* Key */
			(HWTEST_PARAM3 == NULL) ||			/* Key Type */
			(HWTEST_PARAM4 == NULL) ||			/* Time-out */
			(!READABLE_IN_DEC(HWTEST_PARAM, ulDevId)) ||
			(ulDevId > 1) ||
			(!READABLE_IN_DEC(HWTEST_PARAM4, ulTimeout)) ||
			(ulTimeout < 10))
		{
			iResult = CMD_ERR;
			goto TEST_WIFI_WPA_EXIT;
		}

		nKeyLen = VK_strlen(HWTEST_PARAM2);
		if (nKeyLen < 8)
		{
			CMD_Printf("PSK Key length (%d) has to be greter than 8!\n",nKeyLen);
			iResult = CMD_ERR;
			goto TEST_WIFI_WPA_EXIT;
		}

		VK_MEM_Memset(&ApInfo,0x00,sizeof(DI_WIFI_INFO_t));
		VK_snprintf(ApInfo.aucESSID,sizeof(ApInfo.aucESSID),"%s",HWTEST_PARAM1);
		VK_snprintf(ApInfo.aucKey,sizeof(ApInfo.aucKey),"%s",HWTEST_PARAM2);

		if(CMD_IS("wpa"))
		{
			CMD_Printf("Security       : WPA\n");
			ApInfo.eSecureType	= DI_WIFI_SECURE_WPA;
		}
		else if(CMD_IS("wpa2"))
		{
			CMD_Printf("Security       : WPA2\n");
			ApInfo.eSecureType	= DI_WIFI_SECURE_WPA2;
		}
		else
		{
			CMD_Printf("Security       : WPA_WPA2\n");
			ApInfo.eSecureType	= DI_WIFI_SECURE_WPA_WPA2;
		}

		CMD_Printf("Authentication : PSK\n");
		ApInfo.eAuthenType	= DI_WIFI_AUTHEN_PSK;

		if(PARAM3_IS("tkip"))
		{
			CMD_Printf("Encryption     : TKIP\n");
			ApInfo.eEncryptType = DI_WIFI_ENCRYPT_TKIP;
		}
		else if(PARAM3_IS("aes"))
		{
			CMD_Printf("Encryption     : AES\n");
			ApInfo.eEncryptType = DI_WIFI_ENCRYPT_AES;
		}
		else
		{
			CMD_Printf("Encryption     : TKIP_AES\n");
			ApInfo.eEncryptType = DI_WIFI_ENCRYPT_TKIP_AES;
		}

		DI_WIFI_Connect(ulWiFiDev[ulDevId],DI_WIFI_WPS_NONE,&ApInfo,ulTimeout);
		iResult = CMD_OK;
	}
	else if (CMD_IS("wpspin") || CMD_IS("wpspbc"))
	{
		HINT32 nKeyLen = 0, ucIdx = 0;
		DI_WIFI_INFO_t ApInfo;

		if ((HWTEST_PARAM == NULL) ||			/* Device ID */
			(!READABLE_IN_DEC(HWTEST_PARAM, ulDevId)) ||
			(ulDevId > 1))
		{
			iResult = CMD_ERR;
			goto TEST_WIFI_WPS_EXIT;
		}

		if (CMD_IS("wpspin"))
		{
			HUINT8	ucPinCode;

			if ((HWTEST_PARAM1 == NULL) ||			/* PIN Code */
				(HWTEST_PARAM2 == NULL))			/* Time-out */
			{
				iResult = CMD_ERR;
				goto TEST_WIFI_WPS_EXIT;
			}

			nKeyLen = VK_strlen(HWTEST_PARAM1);
			if (nKeyLen != 8)
			{
				CMD_Printf("PIN Code length (%d) has to be equal to 8!\n",nKeyLen);
				iResult = CMD_ERR;
				goto TEST_WIFI_WPS_EXIT;
			}

			if ((!READABLE_IN_DEC(HWTEST_PARAM2, ulTimeout)) ||
				(ulTimeout < 120))
			{
				CMD_Printf("Timeout for WPS (%d) has to be greater than 120s!\n",ulTimeout);
				iResult = CMD_ERR;
				goto TEST_WIFI_WPS_EXIT;
			}

			VK_MEM_Memset(&ApInfo,0x00,sizeof(DI_WIFI_INFO_t));

			for (ucIdx=0 ; ucIdx<8 ; ucIdx++)
			{
				VK_MEM_Memcpy(&ucPinCode,HWTEST_PARAM1+ucIdx,sizeof(HUINT8));
				ApInfo.aucKey[ucIdx] = VK_atoi(&ucPinCode);
			}

			CMD_Printf("PIN Code = [%d][%d][%d][%d][%d][%d][%d][%d]\n",
				ApInfo.aucKey[0], ApInfo.aucKey[1], ApInfo.aucKey[2], ApInfo.aucKey[3],
				ApInfo.aucKey[4], ApInfo.aucKey[5], ApInfo.aucKey[6], ApInfo.aucKey[7]);

			DI_WIFI_Connect(ulWiFiDev[ulDevId],DI_WIFI_WPS_PIN,&ApInfo,ulTimeout);
			iResult = CMD_OK;
		}
		else
		{
			if ((HWTEST_PARAM1 == NULL) ||			/* Time-out */
				(!READABLE_IN_DEC(HWTEST_PARAM1, ulTimeout)) ||
				(ulTimeout < 120))
			{
				CMD_Printf("Timeout for WPS (%d) has to be greater than 120s!\n",ulTimeout);
				iResult = CMD_ERR;
				goto TEST_WIFI_WPS_EXIT;
			}

			DI_WIFI_Connect(ulWiFiDev[ulDevId],DI_WIFI_WPS_PBC,NULL,ulTimeout);
			iResult = CMD_OK;
		}
	}
	else if (CMD_IS("disconnect"))
	{
		if ((HWTEST_PARAM == NULL) ||			/* Device ID */
			(!READABLE_IN_DEC(HWTEST_PARAM, ulDevId)) ||
			(ulDevId > 1))
		{
			iResult = CMD_ERR;
			goto TEST_WIFI_EXIT;
		}

		DI_WIFI_Disconnect(ulWiFiDev[ulDevId]);
		iResult = CMD_OK;
	}
	else if (CMD_IS("apinfo"))
	{
		DI_WIFI_INFO_t stApInfo;

		if ((HWTEST_PARAM == NULL) ||			/* Device ID */
			(!READABLE_IN_DEC(HWTEST_PARAM, ulDevId)) ||
			(ulDevId > 1))
		{
			iResult = CMD_ERR;
			goto TEST_WIFI_EXIT;
		}

		VK_MEM_Memset(&stApInfo,0xFF,sizeof(DI_WIFI_INFO_t));

		DI_WIFI_GetAPInfo(ulWiFiDev[ulDevId],&stApInfo);

		CMD_Printf("  --------------- AP Info -------------\n");
		CMD_Printf("  * ESSID	 : %s\n", 	stApInfo.aucESSID);
		CMD_Printf("  * MAC  : 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x\n", stApInfo.aucMAC[0],stApInfo.aucMAC[1],stApInfo.aucMAC[2],
			stApInfo.aucMAC[3],stApInfo.aucMAC[4],stApInfo.aucMAC[5]);
		CMD_Printf("  * Channel	 : %d\n", stApInfo.ulChannel);
		CMD_Printf("  * Freq	 : %d\n", stApInfo.ulFreq);
		CMD_Printf("  * WPS	 	: %d\n", stApInfo.bWPS);
		CMD_Printf("  * opMode	 : %d\n", 	stApInfo.eOpMode);
		CMD_Printf("  * SECURE	: %s\n", P_WIFI_LogSecureInfo( stApInfo.eSecureType) );
		CMD_Printf("  * AUTHEN	: %s %d\n", P_WIFI_LogAuthenInfo( stApInfo.eAuthenType),stApInfo.eAuthenType );
		CMD_Printf("  * ENCRYPT : %s\n", P_WIFI_LogEncryptInfo( stApInfo.eEncryptType) );
		CMD_Printf("  * waveStrength	 : %d\n", stApInfo.nWaveStrength);
		CMD_Printf("  * BitRate  : %d\n", stApInfo.nBitRate);
		CMD_Printf("  * KEY  : %s\n", stApInfo.aucKey);
		CMD_Printf("  \n");

		iResult = CMD_OK;
	}
	else if (CMD_IS("bitrate"))
	{
		HINT32 nBitRate;

		if ((HWTEST_PARAM == NULL) ||			/* Device ID */
			(!READABLE_IN_DEC(HWTEST_PARAM, ulDevId)) ||
			(ulDevId > 1))
		{
			iResult = CMD_ERR;
			goto TEST_WIFI_EXIT;
		}

		DI_WIFI_GetBitRate(ulWiFiDev[ulDevId],&nBitRate);
		CMD_Printf(" BitRate %d bps\n",nBitRate);
		iResult = CMD_OK;
	}
	else if (CMD_IS("strength"))
	{
		HINT32 nStrength;

		if ((HWTEST_PARAM == NULL) ||			/* Device ID */
			(!READABLE_IN_DEC(HWTEST_PARAM, ulDevId)) ||
			(ulDevId > 1))
		{
			iResult = CMD_ERR;
			goto TEST_WIFI_EXIT;
		}

		DI_WIFI_GetStrength(ulWiFiDev[ulDevId],&nStrength);
		CMD_Printf(" strength %d \n",nStrength);
		iResult = CMD_OK;
	}
	else if (CMD_IS("status"))
	{
		HBOOL bConnect;

		if ((HWTEST_PARAM == NULL) ||			/* Device ID */
			(!READABLE_IN_DEC(HWTEST_PARAM, ulDevId)) ||
			(ulDevId > 1))
		{
			iResult = CMD_ERR;
			goto TEST_WIFI_EXIT;
		}

		DRV_WIFI_GetDongleStatus(ulWiFiDev[ulDevId], &bConnect);
		CMD_Printf(" bConnect %d \n",bConnect);
		iResult = CMD_OK;
	}
	else if (CMD_IS("rssi"))
	{
		int i=0;
		HBOOL bConnect;
		DI_WIFI_RSSI_INFO_t nRSSI;

		if ((HWTEST_PARAM == NULL) ||			/* Device ID */
			(!READABLE_IN_DEC(HWTEST_PARAM, ulDevId)) ||
			(ulDevId > 1))
		{
			iResult = CMD_ERR;
			goto TEST_WIFI_EXIT;
		}

		DI_WIFI_GetRSSI(ulWiFiDev[ulDevId], &nRSSI);
		CMD_Printf("Rssi = %d \n", nRSSI.uiRssi);
		CMD_Printf("Antena count = %d \n", nRSSI.iAntCount);
		for(i=0; i<nRSSI.iAntCount; i++)
		{
			CMD_Printf("Antena[%d] Rssi = %d \n", i, nRSSI.uiAntRssi[i]);
		}
		iResult = CMD_OK;
	}
	else if (CMD_IS("reset"))
	{
		if ((HWTEST_PARAM == NULL) ||			/* Device ID */
			(!READABLE_IN_DEC(HWTEST_PARAM, ulDevId)) ||
			(ulDevId > 1))
		{
			iResult = CMD_ERR;
			goto TEST_WIFI_EXIT;
		}

		DI_WIFI_ResetModule(ulWiFiDev[ulDevId]);
		iResult = CMD_OK;
	}
	else if (CMD_IS("wowl"))
	{
		DI_WIFI_WOWL_SETTINGS_t stWowl;
		
		if ((HWTEST_PARAM == NULL) ||			/* Device ID */
			(!READABLE_IN_DEC(HWTEST_PARAM, ulDevId)) ||
			(ulDevId > 1))
		{
			iResult = CMD_ERR;
			goto TEST_WIFI_EXIT;
		}

		VK_MEM_Memset(&stWowl, 0x0, sizeof(DI_WIFI_WOWL_SETTINGS_t));

		stWowl.eWowlMask = DI_WIFI_WOWL_EVENT_MAGIC_PATTERN | DI_WIFI_WOWL_EVENT_DISASSOC_DEAUTH | DI_WIFI_WOWL_EVENT_BCN_LOSS;
		stWowl.ulBcnLossSec = 10;
		
		DI_WIFI_SetWakeOnWLAN(ulWiFiDev[ulDevId], &stWowl);

		iResult = CMD_OK;
	}
#if defined (CONFIG_QV940)
	else if (CMD_IS("setmac"))
	{
		char str_mac[32];

		if (HWTEST_PARAM == NULL || 			/* Device ID */
			HWTEST_PARAM1 == NULL) 			/* MAC */
		{
			iResult = CMD_ERR;
			goto TEST_WIFI_EXIT;
		}	

		VK_memset(str_mac, 0x00, strlen(str_mac));
		snprintf(str_mac,sizeof(str_mac),"%s",HWTEST_PARAM1);

		DI_WIFI_SetMAC(str_mac);
		iResult = CMD_OK;		
	}
	else if (CMD_IS("getmac"))
	{
		char str_mac[32];

		if (HWTEST_PARAM == NULL) 			/* Device ID */
		{
			iResult = CMD_ERR;
			goto TEST_WIFI_EXIT;
		}	

		VK_memset(str_mac, 0x00, strlen(str_mac));

		DI_WIFI_GetMAC(str_mac);
		CMD_Printf("WLAN MAC Address = %s\n", str_mac);
		iResult = CMD_OK;		
	}
	else if (CMD_IS("on"))
	{
		if ((HWTEST_PARAM == NULL) ||			/* Device ID */
			(!READABLE_IN_DEC(HWTEST_PARAM, ulDevId)) ||
			(ulDevId > 1))
		{
			iResult = CMD_ERR;
			goto TEST_WIFI_EXIT;
		}

		DI_WIFI_On(ulWiFiDev[ulDevId]);
		CMD_Printf("WiFi On!!!\n");
		iResult = CMD_OK;		
	}
	else if (CMD_IS("off"))
	{
		if ((HWTEST_PARAM == NULL) ||			/* Device ID */
			(!READABLE_IN_DEC(HWTEST_PARAM, ulDevId)) ||
			(ulDevId > 1))
		{
			iResult = CMD_ERR;
			goto TEST_WIFI_EXIT;
		}

		DI_WIFI_Off(ulWiFiDev[ulDevId]);
		CMD_Printf("WiFi Off!!!\n");
		iResult = CMD_OK;		
	}
	else if (CMD_IS("version"))
	{
		char str_version[32];

		if ((HWTEST_PARAM == NULL) ||			/* Device ID */
			(!READABLE_IN_DEC(HWTEST_PARAM, ulDevId)) ||
			(ulDevId > 1))
		{
			iResult = CMD_ERR;
			goto TEST_WIFI_EXIT;
		}

		VK_memset(str_version, 0x00, strlen(str_version));

		DI_WIFI_GetVersion(ulWiFiDev[ulDevId], str_version);
		CMD_Printf("WLAN Version = %s\n", str_version);
		iResult = CMD_OK;		
	}
#endif
#if defined(CONFIG_SUPPORT_SECDMA) && defined(CONFIG_WIFI_SCRIPT_TEST)
	else if (CMD_IS("init"))
	{
		if ((HWTEST_PARAM == NULL) ||			/* Device ID */
			(HWTEST_PARAM1 == NULL) ||			/* Driver type (mfg or prd) */
			(!READABLE_IN_DEC(HWTEST_PARAM, ulDevId)) ||
			(ulDevId > 1) ||
			((VK_strncmp(HWTEST_PARAM1, "mfg", 3)!=0) && (VK_strncmp(HWTEST_PARAM1, "prd", 3)!=0)))
		{
			iResult = CMD_ERR;
			goto TEST_WIFI_EXIT;
		}

		VK_snprintf(acCmdBuf, 64, "echo \"%s\" > /tmp/bcm_wifi_test_driver_type.txt", HWTEST_PARAM1);
		VK_SYSTEM_Command(acCmdBuf);
		
		DRV_WIFI_Init();
		
		iResult = CMD_OK;
	}
#endif
	else if (CMD_IS("wpspincode"))
	{
		HCHAR	acWpsPinCode[10];
		
		if (HWTEST_PARAM == NULL) 				/* Device ID */
		{
			iResult = CMD_ERR;
			goto TEST_WIFI_EXIT;
		}

		VK_MEM_Memset(acWpsPinCode, 0x00, sizeof(acWpsPinCode));
		
		DI_WIFI_GetWpsPinCode(ulWiFiDev[ulDevId], acWpsPinCode);

		CMD_Printf("WLAN WPS PIN Code = %s \n", acWpsPinCode);

		iResult = CMD_OK;
	}
	else
	{
		CMD_Printf("wifi : unknown command!\n");
		return CMD_ERR;
	}

	return iResult;

TEST_WIFI_SCAN_EXIT:
	CMD_Printf("Invalid Command Options !! \n\r");
	CMD_Printf("usage : wifi scan [0/1] \n\r");
	return iResult;

TEST_WIFI_OPEN_EXIT:
	CMD_Printf("Invalid Command Options !! \n\r");
	CMD_Printf("usage : wifi open [0/1] [SSID] [Timeout]\n\r");
	return iResult;

TEST_WIFI_WEP_EXIT:
	CMD_Printf("Invalid Command Options !! \n\r");
	CMD_Printf("usage : wifi [wep64a/wep64h/wep128a/wep128h] [0/1] [SSID] [WEP Key] [Timeout]\n\r");
	return iResult;

TEST_WIFI_WPA_EXIT:
	CMD_Printf("Invalid Command Options !! \n\r");
	CMD_Printf("usage : wifi [wpa/wpa2/wpawpa2] [0/1] [SSID] [PSK Key] [tkip/aes/tkipaes] [Timeout]\n\r");
	return iResult;

TEST_WIFI_WPS_EXIT:
	CMD_Printf("Invalid Command Options !! \n\r");
	CMD_Printf("usage : wifi [wpspin/wpspbc] [0/1] [PIN Code(if wpspin)] [Timeout]\n\r");
	return iResult;

TEST_WIFI_EXIT:
	CMD_Printf("Invalid Command Options !! \n\r");
	return iResult;

}
#endif


#if defined(CONFIG_BLUETOOTH)
static int P_Bluetooth_IsValidBdAddr(const HINT8 *pszStrBdAddr)
{
	if (!pszStrBdAddr)
		return -1;

	if (VK_strlen(pszStrBdAddr) != 17)
		return -1;

	while (*pszStrBdAddr) {
		if (!isxdigit(*pszStrBdAddr++))
			return -1;
		if (!isxdigit(*pszStrBdAddr++))
			return -1;
		if (*pszStrBdAddr == 0)
			break;
		if (*pszStrBdAddr++ != ':')
			return -1;
	}
	return 0;
}

static int P_Bluetooth_ConvertHex2Str(HUINT8 *pucHexBdAddr, HINT8 *pszStrBdAddr)
{
	return VK_sprintf(pszStrBdAddr, "%02X:%02X:%02X:%02X:%02X:%02X",
			pucHexBdAddr[0], pucHexBdAddr[1],pucHexBdAddr[2],pucHexBdAddr[3],pucHexBdAddr[4],pucHexBdAddr[5]);
}



static int P_Bluetooth_ConvertStr2Hex(HINT8 *pszStrBdAddr, HUINT8 *pucHexBdAddr)
{
	int i;

	if (P_Bluetooth_IsValidBdAddr(pszStrBdAddr) < 0) {
		VK_MEM_Memset(pucHexBdAddr, 0x0,BT_NB_BDADDR_LEN);
		return -1;
	}

	for (i = 0; i < BT_NB_BDADDR_LEN; i++, pszStrBdAddr += 3)
	{
		pucHexBdAddr[i] = strtol(pszStrBdAddr, NULL, 16);
	}
	return 0;
}

static void P_Bluetooth_PrintDeviceList(DI_BT_DEV_LIST_t *pstDevList)
{
	HUINT32 i;
	HINT8 szBdAddr[20];
	if ( pstDevList -> ulNumOfData <= 0)
	{
		DI_UART_Print("Remote Bluetooth Device List is Empty[%d]\n", pstDevList->ulNumOfData);
	}
	else
	{
		DI_UART_Print("Remote Bluetooth Device List[%d]\n", pstDevList->ulNumOfData);

		for (i = 0 ; i < pstDevList->ulNumOfData; i++)
		{
			DI_UART_Print("Index [%d] o Name : %s\n", i, pstDevList->stBtDevList[i].ucName);
			P_Bluetooth_ConvertHex2Str(pstDevList->stBtDevList[i].ucBdAddr, szBdAddr);
			DI_UART_Print("\t  o BD Address : %s\n", szBdAddr);
			DI_UART_Print("\t  o Class Of Device(Service/Major/Minor) : %02X/%02X/%02X\n",
					pstDevList->stBtDevList[i].ucClass[0],
					pstDevList->stBtDevList[i].ucClass[1],
					pstDevList->stBtDevList[i].ucClass[2]);
			DI_UART_Print("\t  o Status of Bluetooth Device : %s%s%s\n",
					(pstDevList->stBtDevList[i].etDevStatus == DI_BT_DEV_NOT_PAIRED) ? "Not Paired":"",
					(pstDevList->stBtDevList[i].etDevStatus == DI_BT_DEV_CONNECTED) ? "Connected" :"",
					(pstDevList->stBtDevList[i].etDevStatus == DI_BT_DEV_DISCONNECTED) ? "Not Connected" : "");
			if ( pstDevList->stBtDevList[i].etDevStatus == DI_BT_DEV_NOT_PAIRED)
			{
#if defined(CONFIG_BLUETOOTH_BLE)
				DI_UART_Print("\t  o Available Service Profiles : %s%s%s%s\n",
#else
				DI_UART_Print("\t  o Available Service Profiles : %s%s%s\n",
#endif
				(pstDevList->stBtDevList[i].etAvailableProfile & DI_BT_PROFILE_A2DP) ? "A2DP " : "",
				(pstDevList->stBtDevList[i].etAvailableProfile & DI_BT_PROFILE_AVRCP) ? "AVRCP " : "",
#if defined(CONFIG_BLUETOOTH_BLE)
				(pstDevList->stBtDevList[i].etAvailableProfile & DI_BT_PROFILE_HID) ? "HID" : "",
				(pstDevList->stBtDevList[i].etAvailableProfile & DI_BT_PROFILE_BLE) ? "BLE" : "");
#else
				(pstDevList->stBtDevList[i].etAvailableProfile & DI_BT_PROFILE_HID) ? "HID" : "");
#endif

			}
			else
			{
#if defined(CONFIG_BLUETOOTH_BLE)
				DI_UART_Print("\t  o Trusted Service Profiles : %s%s%s%s\n\t   - STB used to open this profiles using %s\n",
#else
				DI_UART_Print("\t  o Trusted Service Profiles : %s%s%s\n\t   - STB used to open this profiles using %s\n",
#endif
				(pstDevList->stBtDevList[i].etAvailableProfile & DI_BT_PROFILE_A2DP) ? "A2DP " : "",
				(pstDevList->stBtDevList[i].etAvailableProfile & DI_BT_PROFILE_AVRCP) ? "AVRCP " : "",
				(pstDevList->stBtDevList[i].etAvailableProfile & DI_BT_PROFILE_HID) ? "HID" : "",
#if defined(CONFIG_BLUETOOTH_BLE)
				(pstDevList->stBtDevList[i].etAvailableProfile & DI_BT_PROFILE_BLE) ? "BLE" : "",
#endif
				pstDevList->stBtDevList[i].ucName);
			}
		}

	}
}

void CMD_HwTest_Parser_BluetoothEventCallback(void *data)
{
	DI_BT_CALLBACK_DATA_t *pstCallbackData;

	pstCallbackData = (DI_BT_CALLBACK_DATA_t *)data;
	HUINT32 i;
	HUINT8 ucPinCode[BT_NB_PINCODE_LEN];
	DI_ERR_CODE err = 0;

	DI_BT_DEV_LIST_t stDIDevList;
	HUINT32 ulDIDevId;
	HINT8 szBdAddr[20];

	VK_memset(&ucPinCode,0x0,BT_NB_PINCODE_LEN);

	VK_memset(&stDIDevList,0x0,sizeof(DI_BT_DEV_LIST_t));


	switch(pstCallbackData->eEvent)
	{
		case DI_BT_NOTIFY_EVT_INSERTED:
			CMD_Printf("==== DI_BT_NOTIFY_EVT_INSERTED ====\n");
			break;
		case DI_BT_NOTIFY_EVT_EXTRACTED:
			CMD_Printf("==== DI_BT_NOTIFY_EVT_EXTRACTED ====\n");
			break;
		case DI_BT_NOTIFY_EVT_ENABLE:
			CMD_Printf("==== DI_BT_NOTIFY_EVT_ENABLE ====\n");
			break;
		case DI_BT_NOTIFY_EVT_DISABLE:
			CMD_Printf("==== DI_BT_NOTIFY_EVT_DISABLE ====\n");
			break;
		case DI_BT_NOTIFY_EVT_SCAN_SUCCESS:
			CMD_Printf("==== DI_BT_NOTIFY_EVT_SCAN_SUCCESS ====\n");
			err = DI_BT_GetDeviceList(&stDIDevList);
			if(err != 0)
			{
				CMD_Printf("[ERR] DI_BT_GetDeviceList \n\r");
			}
			P_Bluetooth_PrintDeviceList(&stDIDevList);
			break;
		case DI_BT_NOTIFY_EVT_SCAN_FAIL:
			CMD_Printf("==== DI_BT_NOTIFY_EVT_SCAN_FAIL ==\n");
			break;
		case DI_BT_NOTIFY_EVT_AUTHEN_SUCCESS:
			CMD_Printf("==== DI_BT_NOTIFY_EVT_AUTHEN_SUCCESS ====\n");
			err = DI_BT_GetDeviceList(&stDIDevList);
			if(err != 0)
			{
				CMD_Printf("[ERR] DI_BT_GetDeviceList \n\r");
			}
			P_Bluetooth_PrintDeviceList(&stDIDevList);
			ulDIDevId = pstCallbackData->ulDevId;
			CMD_Printf("AUTHEN_SUCCESS Index [%d] \n",ulDIDevId);
			P_Bluetooth_ConvertHex2Str(pstCallbackData->pucBdAddr, szBdAddr);
			CMD_Printf("AUTHEN_SUCCESS BD Address [%s]\n", szBdAddr);
			break;
		case DI_BT_NOTIFY_EVT_AUTHEN_FAIL:
			CMD_Printf("==== DI_BT_NOTIFY_EVT_AUTHEN_FAIL ====\n");
			P_Bluetooth_PrintDeviceList((DI_BT_DEV_LIST_t *) pstCallbackData->pvEventData);
			CMD_Printf(" PINCODE REQUEST Index [%d] \n",pstCallbackData->ulDevId);
			P_Bluetooth_ConvertHex2Str(pstCallbackData->pucBdAddr, szBdAddr);
			CMD_Printf(" PINCODE REQUEST BD Address [%s]\n", szBdAddr);
			CMD_Printf("Input Pin Code : ");
			for( i=0 ; i < BT_NB_PINCODE_LEN; i++)
			{
				DI_UART_ReadByte(&ucPinCode[i]);
				if(ucPinCode[i] == '\n')
				{
					break;
				}
			}
			err = DI_BT_SendPinCode2BdAddr(pstCallbackData->pucBdAddr, ucPinCode);
			if(err != 0)
			{
				CMD_Printf("[ERR] DI_BT_SendPinCode2BdAddr \n\r");
			}
/*			err = DI_BT_SendPinCode(pstCallbackData->ulDevId, ucPinCode);
			if(err != 0)
			{
				CMD_Printf("[ERR] DI_BT_SendPinCode \n\r");
			}*/
			break;
		case DI_BT_NOTIFY_EVT_CONNECT_SUCCESS:
			CMD_Printf("==== DI_BT_NOTIFY_EVT_CONNECT_SUCCESS ====\n");
			P_Bluetooth_PrintDeviceList((DI_BT_DEV_LIST_t *) pstCallbackData->pvEventData);
			CMD_Printf(" CONNECTION_SUCCESS Index [%d] \n",pstCallbackData->ulDevId );
			P_Bluetooth_ConvertHex2Str(pstCallbackData->pucBdAddr, szBdAddr);
			CMD_Printf(" CONNECTION_SUCCESS BD Address [%s] \n", szBdAddr);
/*
			err = DI_BT_OpenProfile2BdAddr(pstCallbackData->pucBdAddr,
			  ((DI_BT_DEV_LIST_t *)pstCallbackData->pvEventData)->stBtDevList[pstCallbackData->ulDevId].etAvailableProfile);
			if(err != 0)
			{
				CMD_Printf("[ERR] DI_BT_OpenProfile \n\r");
			}*/
/*			err = DI_BT_OpenProfile(pstCallbackData->ulDevId, ((DI_BT_DEV_LIST_t *)pstCallbackData->pvEventData)->stBtDevList[pstCallbackData->ulDevId].etAvailableProfile);
			if(err != 0)
			{
				CMD_Printf("[ERR] DI_BT_OpenProfile \n\r");
			}*/
			break;
		case DI_BT_NOTIFY_EVT_CONNECT_FAIL:
			CMD_Printf("==== DI_BT_NOTIFY_EVT_CONNECT_FAIL ====\n");
			P_Bluetooth_PrintDeviceList((DI_BT_DEV_LIST_t *) pstCallbackData->pvEventData);
			CMD_Printf("CONNECTION_FAIL Index [%d]\n",pstCallbackData->ulDevId);
			P_Bluetooth_ConvertHex2Str(pstCallbackData->pucBdAddr, szBdAddr);
			CMD_Printf("CONNECT_FAIL BD Address [%s] \n", szBdAddr);
			break;
		case DI_BT_NOTIFY_EVT_REMOTE_CMD:
			CMD_Printf("==== DI_BT_NOTIFY_EVT_REMOTE_CMD ====\n");
			CMD_Printf("REMOTE CMD[%03d] %s \n",((DI_BT_REMOTE_CMD_INFO_t *)pstCallbackData->pvEventData)->ulKey,
			(((DI_BT_REMOTE_CMD_INFO_t *)pstCallbackData->pvEventData)->bPressed ? "Pressed":"Released"));
			break;
		case DI_BT_NOTIFY_EVT_PASSKEY:
			CMD_Printf("==== DI_BT_NOTIFY_EVT_PASSKEY ====\n");
			CMD_Printf("ulDevId[%d] \n", pstCallbackData->ulDevId);
			CMD_Printf("ulNumOfData[%d] \n", pstCallbackData->ulNumOfData);
			P_Bluetooth_ConvertHex2Str(pstCallbackData->pucBdAddr, szBdAddr);
			CMD_Printf("pucBdAddr[%s] \n", szBdAddr);
			CMD_Printf("PASSKEY[%d] \n", *((int *)(pstCallbackData->pvEventData)));
			CMD_Printf("\t=> You must enter this value on peer device's keyboard\n");
			break;
		default:
			CMD_Printf("eEvent[%d] ==\n", pstCallbackData->eEvent);
			break;
	}
}

int CMD_HwTest_Parser_Bluetooth(void *szArgs)
{
	GET_ARGS;
	DI_ERR_CODE err = 0;
	HUINT32 ulDevId;
	int nLevel;
	DI_BT_DEV_INFO_t stHostInfo;
	DI_BT_DEV_LIST_t stDIDevList;
	DI_BT_VERSION_t stBTVersion;
	HUINT32 ulSupportedServiceMask = DI_BT_PROFILE_ALL;	/* Supportable prfiles */
	HUINT8 aucBdAddr[BT_NB_BDADDR_LEN]={0x00,0x00,0x00,0x00,0x00,0x00};
	HINT8 szBdAddr[20];

	VK_MEM_Memset(&stHostInfo, 0x0, sizeof(DI_BT_DEV_INFO_t));
	VK_MEM_Memset(&stDIDevList, 0x0, sizeof(DI_BT_DEV_LIST_t));
	VK_MEM_Memset(&stBTVersion, 0x0, sizeof(DI_BT_VERSION_t));

	DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_INSERTED,CMD_HwTest_Parser_BluetoothEventCallback);
	DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_EXTRACTED,CMD_HwTest_Parser_BluetoothEventCallback);
	DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_ENABLE,CMD_HwTest_Parser_BluetoothEventCallback);
	DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_DISABLE, CMD_HwTest_Parser_BluetoothEventCallback);
	DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_SCAN_SUCCESS,CMD_HwTest_Parser_BluetoothEventCallback);
	DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_SCAN_FAIL,CMD_HwTest_Parser_BluetoothEventCallback);
	DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_AUTHEN_SUCCESS,CMD_HwTest_Parser_BluetoothEventCallback);
	DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_AUTHEN_FAIL,CMD_HwTest_Parser_BluetoothEventCallback);
	DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_CONNECT_SUCCESS,CMD_HwTest_Parser_BluetoothEventCallback);
	DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_CONNECT_FAIL,CMD_HwTest_Parser_BluetoothEventCallback);
	DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_REMOTE_CMD,CMD_HwTest_Parser_BluetoothEventCallback);
	DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_PASSKEY,CMD_HwTest_Parser_BluetoothEventCallback);

	if (CMD_IS("init"))
	{
		err = DI_BT_Init();
		if(err != 0)
		{
			CMD_Printf("[ERR] DI_BT_Init \n\r");
		}
		iResult = CMD_OK;
	}
	else if (CMD_IS("on"))
	{
		err = DI_BT_Enable(TRUE);
		if(err != 0)
		{
			CMD_Printf("[ERR] DI_BT_Enable \n\r");
		}
		iResult = CMD_OK;
	}
	else if (CMD_IS("off"))
	{
		err = DI_BT_Enable(FALSE);
		if(err != 0)
		{
			CMD_Printf("[ERR] DI_BT_Enable \n\r");
		}
		iResult = CMD_OK;
	}
	else if (CMD_IS("dev"))
	{
		err = DI_BT_GetHostInfo(&stHostInfo);
		if(err != 0)
		{
			CMD_Printf("[ERR] DI_BT_GetDeviceinfo \n\r");
		}
		iResult = CMD_OK;
		DI_UART_Print("Local Bluetooth Device Information\n");
		DI_UART_Print("\t- Name %s \n", stHostInfo.ucName);
		P_Bluetooth_ConvertHex2Str(stHostInfo.ucBdAddr, szBdAddr);
		DI_UART_Print("\t- Bd Addr %s\n", szBdAddr);
		DI_UART_Print("\t- Class Of Device(Service:Major Device:Minor Device) %02x:%02x:%02x \n",
			stHostInfo.ucClass[0], stHostInfo.ucClass[1], stHostInfo.ucClass[2]);
	}
	else if (CMD_IS("scan"))
	{
		err = DI_BT_Scan(ulSupportedServiceMask);
		if(err != 0)
		{
			CMD_Printf("[ERR] DI_BT_Scan \n\r");
		}
		iResult = CMD_OK;
	}
	else if (CMD_IS("info"))
	{
		err = DI_BT_GetDeviceList(&stDIDevList);
		if(err != 0)
		{
			CMD_Printf("[ERR] DI_BT_GetDeviceList \n\r");
		}
		iResult = CMD_OK;
		P_Bluetooth_PrintDeviceList(&stDIDevList);
	}
	else if (CMD_IS("cc"))
	{
		if(READABLE_IN_DEC(HWTEST_PARAM, ulDevId))
		{
			err = DI_BT_Connect(ulDevId);
			if(err != 0)
			{
				CMD_Printf("[ERR] DI_BT_Connect \n\r");
			}
			iResult = CMD_OK;
		}
		else if(HWTEST_PARAM !=NULL)
		{
			err = P_Bluetooth_ConvertStr2Hex(HWTEST_PARAM, aucBdAddr);
			if(err !=0)
			{
				iResult = CMD_ERR;
				CMD_Printf("[ERR] Invalid Device BD_Address\n\r");
				goto TEST_BT_EXIT;
			}
			else
			{
				err = DI_BT_Connect2BdAddr(aucBdAddr);
				if(err != 0)
				{
					CMD_Printf("[ERR] DI_BT_Connect2BdAddr \n\r");
				}
				iResult = CMD_OK;
			}
		}
		else
		{
			iResult = CMD_ERR;
			CMD_Printf("Invalid Dev ID 0x%x\n\r",ulDevId);
			goto TEST_BT_EXIT;
		}
	}
	else if (CMD_IS("dc"))
	{
		if(READABLE_IN_DEC(HWTEST_PARAM, ulDevId))
		{
			err = DI_BT_Disconnect(ulDevId);
			if(err != 0)
			{
				CMD_Printf("[ERR] DI_BT_Disconnect \n\r");
			}
			iResult = CMD_OK;
		}
		else if(HWTEST_PARAM !=NULL)
		{
			err = P_Bluetooth_ConvertStr2Hex(HWTEST_PARAM, aucBdAddr);
			if(err !=0)
			{
				iResult = CMD_ERR;
				CMD_Printf("[ERR] Invalid Device BD_Address\n\r");
				goto TEST_BT_EXIT;
			}
			else
			{
				err = DI_BT_Disconnect2BdAddr(aucBdAddr);
				if(err != 0)
				{
					CMD_Printf("[ERR] DI_BT_Disconnect2BdAddr \n\r");
				}
				iResult = CMD_OK;
			}
		}
		else
		{
			iResult = CMD_ERR;
			CMD_Printf("Invalid Dev ID 0x%x\n\r",ulDevId);
			goto TEST_BT_EXIT;
		}
	}
	else if (CMD_IS("rm"))
	{
		if(READABLE_IN_DEC(HWTEST_PARAM, ulDevId))
		{
			err = DI_BT_Remove(ulDevId);
			if(err != 0)
			{
				CMD_Printf("[ERR] DI_BT_Remove \n\r");
			}
			iResult = CMD_OK;
		}
		else if(HWTEST_PARAM !=NULL)
		{
			err = P_Bluetooth_ConvertStr2Hex(HWTEST_PARAM, aucBdAddr);
			if(err !=0)
			{
				iResult = CMD_ERR;
				CMD_Printf("[ERR] Invalid Device BD_Address\n\r");
				goto TEST_BT_EXIT;
			}
			else
			{
				err = DI_BT_Remove2BdAddr(aucBdAddr);
				if(err != 0)
				{
					CMD_Printf("[ERR] DI_BT_Remove2BdAddr \n\r");
				}
				iResult = CMD_OK;
			}
		}
		else
		{
			iResult = CMD_ERR;
			CMD_Printf("Invalid Dev ID 0x%x\n\r",ulDevId);
			goto TEST_BT_EXIT;
		}
	}
	else if (CMD_IS("setmac"))
	{
		if(HWTEST_PARAM !=NULL)
		{
			err = P_Bluetooth_ConvertStr2Hex(HWTEST_PARAM, aucBdAddr);
			if(err !=0)
			{
				iResult = CMD_ERR;
				CMD_Printf("[ERR] Invalid Device BD_Address\n\r");
				goto TEST_BT_EXIT;
			}
			else
			{
				err = DI_BT_SetBdAddrToNvram(aucBdAddr);
				if(err != 0)
				{
					CMD_Printf("[ERR] DI_BT_SetBdAddrToNvram \n\r");
				}
				iResult = CMD_OK;
			}
		}
		else
		{
			iResult = CMD_ERR;
			CMD_Printf("Invalid Bd Address\n\r");
			goto TEST_BT_EXIT;
		}
	}
	else if (CMD_IS("getmac"))
	{
		err = DI_BT_GetBdAddrFromNvram(aucBdAddr);
		if(err != 0)
		{
			CMD_Printf("[ERR] DI_BT_GetBdAddrFromNvram \n\r");
		}
		else
		{
			P_Bluetooth_ConvertHex2Str(aucBdAddr, szBdAddr);
			CMD_Printf("BD Address From Nvram [%s] \n", szBdAddr);
		}
		iResult = CMD_OK;
	}
	else if (CMD_IS("ver"))
	{
		err = DI_BT_ReadVersion(&stBTVersion);
		if(err != 0)
		{
			CMD_Printf("[ERR] DI_BT_ReadVersion \n\r");
		}
		iResult = CMD_OK;
	}
	else if (CMD_IS("tr"))
	{
		if(READABLE_IN_DEC(HWTEST_PARAM, nLevel))
		{
			err = DI_BT_SetTraceLevel(nLevel);
			if(err != 0)
			{
				CMD_Printf("[ERR] DI_BT_SetTraceLevel \n\r");
			}
			iResult = CMD_OK;
		}
		else
		{
			iResult = CMD_ERR;
			CMD_Printf("Invalid Level %d\n\r", nLevel);
			goto TEST_BT_EXIT;
		}
	}
	else if (CMD_IS("delxml"))
	{
		err = DI_BT_DelXmlFiles();
		if(err != 0)
		{
			CMD_Printf("[ERR] DI_BT_DelXmlFiles \n\r");
		}
		iResult = CMD_OK;
	}
	else if (CMD_IS("tm"))
	{
		if PARAM_IS("on")
		{
			CMD_Printf("Enable Test Mode\n");
			iResult = DI_BT_SetTestMode(TRUE);
			AssertDI(iResult, CMD_ERR);
		}
		else if PARAM_IS("off")
		{
			CMD_Printf("Disable Test Mode\n");

			iResult = DI_BT_SetTestMode(FALSE);
			AssertDI(iResult, CMD_ERR);
		}
		else
		{
			iResult = CMD_ERR;
			CMD_Printf("Invalid Parameter( usage: bt tm on/off )\n\r");
			goto TEST_BT_EXIT;
		}
	}
#if defined(CONFIG_BLUETOOTH_SIG)
	else if (CMD_IS("disc"))
	{
		if PARAM_IS("on")
		{
			CMD_Printf("Enable Discoverable Mode\n");
			iResult = DI_BT_SetDiscoverableMode(TRUE);
			AssertDI(iResult, CMD_ERR);
		}
		else if PARAM_IS("off")
		{
			CMD_Printf("Disable Discoverable Mode\n");

			iResult = DI_BT_SetDiscoverableMode(FALSE);
			AssertDI(iResult, CMD_ERR);
		}
		else
		{
			iResult = CMD_ERR;
			CMD_Printf("Invalid Parameter( usage: bt disc on/off )\n\r");
			goto TEST_BT_EXIT;
		}
	}
	else if (CMD_IS("senddata"))
	{
		if(READABLE_IN_DEC(HWTEST_PARAM, ulDevId))
		{
			err = DI_BT_HID_SendData(ulDevId);
			if(err != 0)
			{
				CMD_Printf("[ERR] DI_BT_HID_SendData \n\r");
			}
			iResult = CMD_OK;
		}
		else
		{
			iResult = CMD_ERR;
			CMD_Printf("Invalid Dev ID 0x%x\n\r",ulDevId);
			goto TEST_BT_EXIT;
		}
	}
#endif
	else
	{
		CMD_Printf("Command : bt\n");
		CMD_Printf("  ex) bt help/init/on/off/dev/ver/scan/info/cc/dc/rm/setmac/getmac/tr\n");
		CMD_Printf("bt init : Bluetooth Module Init(SEMA,MSG,TASK Create & load .ko) \n");
		CMD_Printf("bt on : Bluetooth Enable\n");
		CMD_Printf("bt off : Bluetooth Disable\n");
		CMD_Printf("bt dev : Host Bluetooth information\n");
		CMD_Printf("bt ver : Print the version of bsa_server\n");
		CMD_Printf("bt scan : Scan Remote Devices\n");
		CMD_Printf("bt info : Print Remote Device List\n");
		CMD_Printf("bt cc # or XX:XX:XX:XX:XX:XX : Connect a Remote Device # or BD Address on Remote Device List\n");
		CMD_Printf("bt dc # or XX:XX:XX:XX:XX:XX : Disconnect a Remote Device # or BD Addess on Remote Device List\n");
		CMD_Printf("bt rm # or XX:XX:XX:XX:XX:XX : Remove a Remote Device # or BD Address on Remote Device List except for not paired devices\n");
		CMD_Printf("bt setmac XX:XX:XX:XX:XX:XX : <CAUTION> Set Host Bluetooth BD Address to NVRAM\n");
		CMD_Printf("bt getmac : Get Host Bluetooth BD Address from NVRAM\n");
		CMD_Printf("bt tr # : Set the trace level of bsa_server (0(NONE)~5(DEBUG))\n");
		CMD_Printf("bt delxml : Delete /var/lib/humaxtv_user/bt_*.xml\n");
		CMD_Printf("bt tm on/off : Enable/Disable Test Mode\n");
		return 0;
	}
TEST_BT_EXIT:

	return iResult;
}

#endif

#if defined(CONFIG_PPPOE)
void CMD_HwTest_Parser_PPPoEEventCallback(void *data)
{
	DI_PPPOE_CALLBACK_DATA_t *pstCallbackData;

	pstCallbackData = (DI_PPPOE_CALLBACK_DATA_t *)data;

	switch(pstCallbackData->eEvent)
	{
		case DI_PPPOE_EVENT_CONNECT_SUCCESS:
			CMD_Printf("== DEV[%d] : DI_PPPOE_EVENT_CONNECT_SUCCESS ==\n", pstCallbackData->ulDevId);
			break;
		case DI_PPPOE_EVENT_CONNECT_FAIL:
			CMD_Printf("== DEV[%d] : DI_PPPOE_EVENT_CONNECT_FAIL ==\n", pstCallbackData->ulDevId);
			break;
		case DI_PPPOE_EVENT_DISCONNECT_SUCCESS:
			CMD_Printf("== DEV[%d] : DI_PPPOE_EVENT_DISCONNECT_SUCCESS ==\n", pstCallbackData->ulDevId);
			break;
		case DI_PPPOE_EVENT_DISCONNECT_FAIL:
			CMD_Printf("== DEV[%d] : DI_PPPOE_EVENT_DISCONNECT_FAIL ==\n", pstCallbackData->ulDevId);
			break;
		default:
			CMD_Printf("== DEV[%d] :  eEvent[%d] ==\n", pstCallbackData->ulDevId, pstCallbackData->eEvent);
			break;
	}
}

int CMD_HwTest_Parser_PPPoE(void *szArgs)
{
	GET_ARGS;
	HUINT32 ulDevId = 2; /* future use */
	DI_ERR_CODE err = 0;

	UNUSED(szParam2);
	UNUSED(szParam3);
	UNUSED(szParam4);
	UNUSED(szParam5);

	iResult = CMD_OK;

	if (CMD_IS("on"))
	{
		DI_PPPOE_INFO_t stPPPoEInfo;

		if( (HWTEST_PARAM == NULL) ||		/* ID */
			(HWTEST_PARAM1 == NULL) )		/* PW */
		{
			iResult = CMD_ERR;
			CMD_Printf("Invalid Parameter\n\r");
			goto TEST_PPPOE_EXIT;
		}

		VK_MEM_Memset(&stPPPoEInfo, 0x00, sizeof(DI_PPPOE_INFO_t));
		stPPPoEInfo.ulWaitSeconds = 30;
		VK_snprintf(stPPPoEInfo.aucId, sizeof(stPPPoEInfo.aucId), "%s", HWTEST_PARAM);
		VK_snprintf(stPPPoEInfo.aucPassword, sizeof(stPPPoEInfo.aucPassword), "%s", HWTEST_PARAM1);

		DI_PPPOE_RegisterEventCallback(ulDevId, DI_PPPOE_EVENT_CONNECT_SUCCESS, CMD_HwTest_Parser_PPPoEEventCallback);
		DI_PPPOE_RegisterEventCallback(ulDevId, DI_PPPOE_EVENT_CONNECT_FAIL, CMD_HwTest_Parser_PPPoEEventCallback);
		err = DI_PPPOE_Connect(ulDevId, &stPPPoEInfo);
		if(err != 0)
		{
			iResult = CMD_ERR;
			CMD_Printf("[ERR] DI_PPPOE_Connect \n\r");
			goto TEST_PPPOE_EXIT;
		}
	}
	else if(CMD_IS("off"))
	{
		DI_PPPOE_RegisterEventCallback(ulDevId, DI_PPPOE_EVENT_DISCONNECT_SUCCESS, CMD_HwTest_Parser_PPPoEEventCallback);
		DI_PPPOE_RegisterEventCallback(ulDevId, DI_PPPOE_EVENT_DISCONNECT_FAIL, CMD_HwTest_Parser_PPPoEEventCallback);
		err = DI_PPPOE_Disconnect(ulDevId);
		if(err != 0)
		{
			iResult = CMD_ERR;
			CMD_Printf("[ERR] DI_PPPOE_Disconnect \n\r");
			goto TEST_PPPOE_EXIT;
		}
	}
	else if(CMD_IS("set"))
	{
		DI_PPPOE_INFO_t stPPPoEInfo;

		if( (HWTEST_PARAM == NULL) ||		/* ID */
			(HWTEST_PARAM1 == NULL) )		/* PW */
		{
			iResult = CMD_ERR;
			CMD_Printf("Invalid Parameter\n\r");
			goto TEST_PPPOE_EXIT;
		}

		VK_MEM_Memset(&stPPPoEInfo, 0x00, sizeof(DI_PPPOE_INFO_t));
		stPPPoEInfo.ulWaitSeconds = 30;
		VK_snprintf(stPPPoEInfo.aucId, sizeof(stPPPoEInfo.aucId), "%s", HWTEST_PARAM);
		VK_snprintf(stPPPoEInfo.aucPassword, sizeof(stPPPoEInfo.aucPassword), "%s", HWTEST_PARAM1);

		err = DI_PPPOE_SetInfo(ulDevId, &stPPPoEInfo);
		if(err != 0)
		{
			iResult = CMD_ERR;
			CMD_Printf("[ERR] DI_NETWORK_SetPPPoEInfo \n\r");
			goto TEST_PPPOE_EXIT;
		}
	}
	else if(CMD_IS("get"))
	{
		DI_PPPOE_INFO_t stPPPoEInfo;

		VK_MEM_Memset(&stPPPoEInfo, 0x00, sizeof(DI_PPPOE_INFO_t));
		err = DI_PPPOE_GetInfo(ulDevId,&stPPPoEInfo);
		if(err != 0)
		{
			iResult = CMD_ERR;
			CMD_Printf("[ERR] DI_NETWORK_GetPPPoEInfo \n\r");
			goto TEST_PPPOE_EXIT;
		}

		CMD_Printf("=== PPPoE Information ===\n");
		CMD_Printf("\t- ID: %s\n", stPPPoEInfo.aucId);
		CMD_Printf("\t- PW: %s\n", stPPPoEInfo.aucPassword);
		CMD_Printf("\t- Waiting Time: %d\n", stPPPoEInfo.ulWaitSeconds);
		CMD_Printf("=========================\n");
	}
	else
	{
		CMD_Printf("Command : pppoe\n");
		CMD_Printf("  ex) pppoe on|off|set|get\n");
		CMD_Printf("pppoe on [ID] [PW]: Start PPPoE Connection\n");
		CMD_Printf("pppoe off : Stop PPPoE Connection\n");
		CMD_Printf("pppoe set [ID] [PW]: Setting PPPoE Information (ID/PW)\n");
		CMD_Printf("pppoe get : Print PPPoE Information (ID/PW)\n");
	}

TEST_PPPOE_EXIT:
	return iResult;
}

#endif

#endif
