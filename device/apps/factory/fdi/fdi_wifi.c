/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termio.h>
#include <termios.h>
#include <poll.h>

/* chip manufacture's  header files */

/* humax header files */
#include "fdi_common.h"
#include "fdi_wifi.h"
#include "fdi_ethernet.h"

#include "htype.h"
#include "di_err.h"
#if defined(CONFIG_DI10)
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>
#include <linux/if.h>
#include <linux/route.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <errno.h>
#include "vkernel.h"
#include "di_usb_wireless.h"
#else
#include "di_wifi.h"
#include "di_network.h"
#endif

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#if defined(CONFIG_DI10)
#define MAX_MODULE_CMD 30
#define DEVNAME "wlan0"
#define IF_NAME_SIZE 16
#define IS_VALID_IPV4_ADDR(ipv4_addr) 	VK_memcmp(s_aucIpv4Null, ipv4_addr, FDI_IPV4_ADDR_LEN)
#define IS_VALID_IPV6_ADDR(ipv6_addr) 	VK_memcmp(s_aucIpv6Null, ipv6_addr, FDI_IPV4_ADDR_LEN)
#if defined(CONFIG_BCM43236) || defined(CONFIG_BCM43236_MFG)
#if defined(CONFIG_WIFI_AARDVARK)
#define FIRST_MODULE_NAME "bcm_dbus.ko"
#define SECOND_MODULE_NAME "wl.ko"
#else
#define MODULE_NAME "dhd.ko"
#endif
#endif
#endif

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
pfnFDI_WIFI_NotifyFunc g_pfnWIFINofifyFunc[FDI_WIFI_MAX_NUM][FDI_WIFI_NOTIFY_EVT_MAX];
static unsigned long ulWiFiDev[FDI_WIFI_MAX_NUM];
#if defined(CONFIG_DI10)
static int s_sockfd;

const static HUINT8 s_aucMacNull[FDI_MAC_ADDR_LEN] = {0, };
const static HUINT8 s_aucIpv4Null[FDI_IPV4_ADDR_LEN] = {0, };
const static HUINT8 s_aucIpv6Null[FDI_IPV6_ADDR_LEN] = {0, };
#endif

#if defined(CONFIG_DI10)
DI_ERR_CODE INTRN_FDI_WIFI_NotifyFunc(DI_USB_Wireless_Event_t event, void *info)
{
	FDI_WIFI_EVENT_e eEvent = FDI_WIFI_NOTIFY_EVT_MAX;
	usb_wireless_info_t *hpInfo = (usb_wireless_info_t *)info;
	FDI_WIFI_CALLBACK_DATA_t stCallbackData;
#if defined(CONFIG_WIFI_AARDVARK)
	unsigned char first_module_cmd[MAX_MODULE_CMD];
	unsigned char second_module_cmd[MAX_MODULE_CMD];
#else
	unsigned char module_cmd[MAX_MODULE_CMD];
#endif

#if defined(CONFIG_WIFI_AARDVARK)
	memset(first_module_cmd, 0, MAX_MODULE_CMD);
	memset(second_module_cmd, 0, MAX_MODULE_CMD);
#else
	memset(module_cmd, 0, MAX_MODULE_CMD);
#endif

	switch(event)
	{
		case DI_USB_WIRELESS_Insert:
			eEvent = FDI_WIFI_NOTIFY_EVT_INSERTED;
#if defined(CONFIG_WIFI_AARDVARK)
			snprintf(first_module_cmd, MAX_MODULE_CMD, "modprobe %s", FIRST_MODULE_NAME);
			snprintf(second_module_cmd, MAX_MODULE_CMD, "modprobe %s", SECOND_MODULE_NAME);
#else
			snprintf(module_cmd, MAX_MODULE_CMD, "modprobe %s", MODULE_NAME);
#endif
			break;
		case DI_USB_WIRELESS_Extract:
			eEvent = FDI_WIFI_NOTIFY_EVT_EXTRACTED;
#if defined(CONFIG_WIFI_AARDVARK)
			snprintf(first_module_cmd, MAX_MODULE_CMD, "rmmod %s", FIRST_MODULE_NAME);
			snprintf(second_module_cmd, MAX_MODULE_CMD, "rmmod %s", SECOND_MODULE_NAME);
#else
			snprintf(module_cmd, MAX_MODULE_CMD, "rmmod %s", MODULE_NAME);
#endif
			break;
		default:
			break;
	}

	if(hpInfo->devCount)
	{
		ulWiFiDev[0] = 1;
	}

	if(eEvent != FDI_WIFI_NOTIFY_EVT_MAX)
	{
		if(g_pfnWIFINofifyFunc[0][eEvent])
		{
			stCallbackData.eEvent = eEvent;
			stCallbackData.ulDevId = 0;
			(*g_pfnWIFINofifyFunc[0][eEvent])(&stCallbackData);
#if defined(CONFIG_WIFI_AARDVARK)
			VK_SYSTEM_Command(first_module_cmd);
			VK_SYSTEM_Command(second_module_cmd);
#else
			VK_SYSTEM_Command(module_cmd);
#endif
		}
	}
	else
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[INTRN_FDI_WIFI_NotifyFunc] Detect unsupported event %d\n",event));
	}

	return DI_ERR_OK;
}
#else
void INTRN_FDI_WIFI_NotifyFunc(void *data)
{
	DI_WIFI_CALLBACK_DATA_t *pstCallbackData;
	unsigned int ulDevId = 0;
	FDI_WIFI_EVENT_e eEvent = FDI_WIFI_NOTIFY_EVT_MAX;
	FDI_WIFI_CALLBACK_DATA_t stCallbackData;
	int i = 0;
	pstCallbackData = (DI_WIFI_CALLBACK_DATA_t *)data;
	ulDevId = pstCallbackData->ulDevId;

	switch(pstCallbackData->eEvent)
	{
		case DI_WIFI_NOTIFY_EVT_INSERTED:
			eEvent = FDI_WIFI_NOTIFY_EVT_INSERTED;
			break;
		case DI_WIFI_NOTIFY_EVT_EXTRACTED:
			eEvent = FDI_WIFI_NOTIFY_EVT_EXTRACTED;
			break;
		case DI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS:
			eEvent = FDI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS;
			break;
		case DI_WIFI_NOTIFY_EVT_CONNECT_FAIL:
			eEvent = FDI_WIFI_NOTIFY_EVT_CONNECT_FAIL;
			break;
		default:
			break;
	}

	for(i = 0 ; i < FDI_WIFI_MAX_NUM ; i++)
	{
		if(ulWiFiDev[i] == ulDevId)
		{
			break;
		}
	}

	if (i == FDI_WIFI_MAX_NUM)
	{
		return;
	}

	if(eEvent != FDI_WIFI_NOTIFY_EVT_MAX)
	{
		if(g_pfnWIFINofifyFunc[i][eEvent])
		{
			stCallbackData.eEvent = eEvent;
			stCallbackData.ulDevId = i;
			(*g_pfnWIFINofifyFunc[i][eEvent])(&stCallbackData);
		}
	}
	return DI_ERR_OK;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_WIFI_Open(void)
{
#if defined(CONFIG_DI10)
	DI_ERR_CODE eDiErr = DI_ERR_OK;

	s_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(s_sockfd < 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_Open] : Socket open error!!\n"));
		return FDI_ERR;
	}

	eDiErr = DI_USB_WIRELESS_Init();
	if(eDiErr != DI_ERR_OK)
		return FDI_ERR;

	return FDI_NO_ERR;
#else
	return FDI_NO_ERR;
#endif
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_WIFI_GetCapability(unsigned long *pulNum)
{
#if defined(CONFIG_DI10)
	*pulNum = 1;

	return FDI_NO_ERR;
#else
	unsigned long ulLoop = 0;
	int nIndex = 0;
	unsigned long ulCount = 0;
	DI_NETWORK_GET_INFO_t stGetInfo;
	DI_ERR_CODE	eDiErr = DI_ERR_OK;

	memset(ulWiFiDev,0xFF,sizeof(ulWiFiDev));

	DI_NETWORK_GetDevCount(&ulCount);
	for(ulLoop = 0; ulLoop < ulCount; ulLoop++)
	{
		eDiErr = DI_NETWORK_GetDevInfo(ulLoop,&stGetInfo);
		if(eDiErr != DI_ERR_OK)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_GetCapability] DI_NETWORK_GetDevInfo[%d] err %d\n",ulLoop,eDiErr));
			return FDI_ERR;
		}

		FDI_PRINT(0, ("[FDI_WIFI_GetCapability] nIndex [%d] ulLoop [%d] Type %d \n",nIndex, ulLoop,stGetInfo.eDevType));
		if(stGetInfo.eDevType == DI_NETWORK_DEV_WIFI)
		{
			if(nIndex < FDI_WIFI_MAX_NUM)
			{
				ulWiFiDev[nIndex] = ulLoop;
			}
			else
			{
				FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_GetCapability]  MAX WIFI NOT Support\n"));
			}
			nIndex++;
		}
	}

	*pulNum = nIndex;
	return FDI_NO_ERR;
#endif
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if defined(CONFIG_DI10)
FDI_ERR_CODE FDI_WIFI_RegisterCallback(unsigned long ulIndex, FDI_WIFI_EVENT_e eEvent, pfnFDI_WIFI_NotifyFunc pfnCallback)
{
	unsigned long ulDevId;
	DI_ERR_CODE eDiErr = DI_ERR_OK;

	if(ulIndex >= FDI_WIFI_MAX_NUM)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_RegisterCallback] Input Param ulDevId Err %d\n",ulIndex));
		return FDI_ERR;
	}

	if(ulWiFiDev[ulIndex]==0xFF)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_RegisterCallback] DevId Err %d\n",ulWiFiDev[ulIndex]));
		return FDI_ERR;

	}

	g_pfnWIFINofifyFunc[0][eEvent] = pfnCallback;

	DI_USB_WIRELESS_RegisterCallback(INTRN_FDI_WIFI_NotifyFunc);

	return FDI_NO_ERR;
}
#else
FDI_ERR_CODE FDI_WIFI_RegisterCallback(unsigned long ulIndex, FDI_WIFI_EVENT_e eEvent, pfnFDI_WIFI_NotifyFunc pfnCallback)
{
	DI_WIFI_NOTIFY_EVT_e eDiEvent;
	unsigned long ulDevId;
	DI_ERR_CODE	eDiErr = DI_ERR_OK;

	if(ulIndex >= FDI_WIFI_MAX_NUM)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_RegisterCallback] Input Param ulDevId Err %d\n",ulIndex));
		return FDI_ERR;
	}

	if(ulWiFiDev[ulIndex]==0xFF)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_RegisterCallback] DevId Err %d\n",ulWiFiDev[ulIndex]));
		return FDI_ERR;

	}
	ulDevId = ulWiFiDev[ulIndex];
	switch(eEvent)
	{
		case FDI_WIFI_NOTIFY_EVT_INSERTED:
			eDiEvent = DI_WIFI_NOTIFY_EVT_INSERTED;
			break;
		case FDI_WIFI_NOTIFY_EVT_EXTRACTED:
			eDiEvent = DI_WIFI_NOTIFY_EVT_EXTRACTED;
			break;
		case FDI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS:
			eDiEvent = DI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS;
			break;
		case FDI_WIFI_NOTIFY_EVT_CONNECT_FAIL:
			eDiEvent = DI_WIFI_NOTIFY_EVT_CONNECT_FAIL;
			break;
		default:
			return FDI_ERR;
	}
	g_pfnWIFINofifyFunc[ulIndex][eEvent] = pfnCallback;

	eDiErr = DI_WIFI_RegisterEventCallback(ulDevId,eDiEvent,INTRN_FDI_WIFI_NotifyFunc);
	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_RegisterCallback] DI_WIFI_RegisterEventCallback err %d\n",eDiErr));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE 	FDI_WIFI_Connect(unsigned long ulIndex, unsigned char *pucApName, unsigned long ulWaitSeconds)
{
#if defined(CONFIG_DI10)
#if defined(CONFIG_BCM43236)
	unsigned char ucIdx, ucStrLen=0;
	char acTmpSsid[66];
	char acWlCmdBuf[384];
	FDI_WIFI_CALLBACK_DATA_t stCallbackData;

	if(ulIndex >= FDI_WIFI_MAX_NUM)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_Connect] Input Param ulDevId Err %d\n",ulIndex));
		return FDI_ERR;
	}

	if(ulWiFiDev[ulIndex]==0xFF)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_Connect] DevId Err %d\n",ulWiFiDev[ulIndex]));
		return FDI_ERR;
	}

	if(pucApName == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_Connect] pucApName NULL\n"));
		return FDI_ERR;
	}

	VK_SYSTEM_Command("wl down");
#if defined(CONFIG_WIFI_AARDVARK)&&defined(CONFIG_DI10)
#else
	VK_SYSTEM_Command("wl mimo_bw cap 1"); /* not supported */
#endif
	VK_SYSTEM_Command("wl obss_coex 1");
	VK_SYSTEM_Command("wl wme 1");
	VK_SYSTEM_Command("wl up");
	VK_SYSTEM_Command("wl frameburst 1");
	VK_SYSTEM_Command("wl auth 0");
	VK_SYSTEM_Command("wl infra 1");
	VK_SYSTEM_Command("wl wsec 0");
	VK_SYSTEM_Command("wl wpa_auth 0");
	VK_SYSTEM_Command("wl sup_wpa 1");

	ucStrLen = 0;
	VK_MEM_Memset(acTmpSsid, 0x00, sizeof(acTmpSsid));
	ucStrLen = VK_strlen(pucApName);
	printf("[%s]_%d SSID Length = %d \n",__func__,__LINE__,ucStrLen);
	for ( ucIdx = 0 ; ucIdx < ucStrLen ; ucIdx++ )
	{
		printf("pucApName[%d] = [%c]\n",ucIdx, pucApName[ucIdx]);
		acTmpSsid[2*ucIdx]	= 0x5C;		// Back Slash
		acTmpSsid[2*ucIdx+1]	= pucApName[ucIdx];
	}

	VK_MEM_Memset(acWlCmdBuf, 0x00, sizeof(acWlCmdBuf));
#if defined(CONFIG_DI10)
	snprintf(acWlCmdBuf, sizeof(acWlCmdBuf), "wl escanresults --ssid=%s", acTmpSsid);
#else
	snprintf(acWlCmdBuf, sizeof(acWlCmdBuf), "wl join %s", acTmpSsid);
#endif
	VK_SYSTEM_Command(acWlCmdBuf);

	if(g_pfnWIFINofifyFunc[0][FDI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS])
	{
		stCallbackData.eEvent = FDI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS;
		stCallbackData.ulDevId = 0;
		(*g_pfnWIFINofifyFunc[0][FDI_WIFI_NOTIFY_EVT_CONNECT_SUCCESS])(&stCallbackData);
	}

	return FDI_NO_ERR;
#endif
#else
	unsigned long ulDevId;
	DI_WIFI_INFO_t ApInfo;
	DI_ERR_CODE	eDiErr = DI_ERR_OK;

	if(ulIndex >= FDI_WIFI_MAX_NUM)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_Connect] Input Param ulDevId Err %d\n",ulIndex));
		return FDI_ERR;
	}

	if(ulWiFiDev[ulIndex]==0xFF)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_Connect] DevId Err %d\n",ulWiFiDev[ulIndex]));
		return FDI_ERR;
	}

	if(pucApName == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_Connect] pucApName NULL\n"));
		return FDI_ERR;
	}
	ulDevId = ulWiFiDev[ulIndex];

	memset(&ApInfo,0,sizeof(DI_WIFI_INFO_t));
	sprintf(ApInfo.aucESSID,"%s",pucApName);

	ApInfo.eAuthenType	= DI_WIFI_AUTHEN_OPEN;
	ApInfo.eEncryptType = DI_WIFI_ENCRYPT_NONE;
	ApInfo.eSecureType	= DI_WIFI_SECURE_OPEN;

	eDiErr = DI_WIFI_Connect(ulDevId,DI_WIFI_WPS_NONE,&ApInfo,ulWaitSeconds);
	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_Connect] DI_WIFI_RegisterEventCallback err %d\n",eDiErr));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
#endif
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE 	FDI_WIFI_DisConnect(unsigned long ulIndex)
{
#if defined(CONFIG_DI10)
	FDI_WIFI_CALLBACK_DATA_t stCallbackData;

	if(ulIndex >= FDI_WIFI_MAX_NUM)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_Connect] Input Param ulDevId Err %d\n",ulIndex));
		return FDI_ERR;
	}

	if(ulWiFiDev[ulIndex]==0xFF)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_Connect] DevId Err %d\n",ulWiFiDev[ulIndex]));
		return FDI_ERR;
	}

	VK_SYSTEM_Command("wl down");
	VK_SYSTEM_Command("wl up");

	if(g_pfnWIFINofifyFunc[0][FDI_WIFI_NOTIFY_EVT_CONNECT_FAIL])
	{
		stCallbackData.eEvent = FDI_WIFI_NOTIFY_EVT_CONNECT_FAIL;
		stCallbackData.ulDevId = 0;
		(*g_pfnWIFINofifyFunc[0][FDI_WIFI_NOTIFY_EVT_CONNECT_FAIL])(&stCallbackData);
	}
	return FDI_NO_ERR;
#else
	unsigned long ulDevId;

	if(ulIndex >= FDI_WIFI_MAX_NUM)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_Connect] Input Param ulDevId Err %d\n",ulIndex));
		return FDI_ERR;
	}

	if(ulWiFiDev[ulIndex]==0xFF)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_Connect] DevId Err %d\n",ulWiFiDev[ulIndex]));
		return FDI_ERR;
	}

	ulDevId = ulWiFiDev[ulIndex];

	DI_WIFI_Disconnect(ulDevId);
	return FDI_NO_ERR;
#endif
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_WIFI_SetAddr(unsigned long ulIndex, FDI_WIFI_NETINFO* pNetworkInfo)
{
#if defined(CONFIG_DI10)
	struct ifreq ifreq;
	struct sockaddr_in *sin;
	unsigned char *addr;

	VK_strncpy(ifreq.ifr_name, DEVNAME, IF_NAME_SIZE-1);
	sin = (struct sockaddr_in *)&ifreq.ifr_addr;
	VK_MEM_Memset(sin, 0, sizeof(struct sockaddr));
	sin->sin_family = AF_INET;

	addr = pNetworkInfo->ip_addr;
	if(IS_VALID_IPV4_ADDR(addr))
	{
		sin->sin_addr.s_addr = addr[3] << 24 | addr[2] << 16 | addr[1] << 8 | addr[0];
		if(ioctl(s_sockfd, SIOCSIFADDR, (char *)&ifreq) < 0)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_SetInfo] :  set ip address  error!!\n"));
			return FDI_ERR;
		}
	}

	addr = pNetworkInfo->net_addr;
	if(IS_VALID_IPV4_ADDR(addr))
	{
		sin->sin_addr.s_addr = addr[3] << 24 | addr[2] << 16 | addr[1] << 8 | addr[0];
		if(ioctl(s_sockfd, SIOCSIFNETMASK, (char *)&ifreq) < 0)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_SetInfo] :  set netmask error!!\n"));
			return FDI_ERR;
		}
	}

#else
	DI_ERR_CODE	eDiErr = DI_ERR_OK;
	unsigned long ulDevId;

	DI_NETWORK_SET_INFO_t stSetInfo;

	if(ulIndex >= FDI_WIFI_MAX_NUM)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_Connect] Input Param ulDevId Err %d\n",ulIndex));
		return FDI_ERR;
	}

	if(ulWiFiDev[ulIndex]==0xFF)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_Connect] DevId Err %d\n",ulWiFiDev[ulIndex]));
		return FDI_ERR;
	}
	ulDevId = ulWiFiDev[ulIndex];

	memset(&stSetInfo,0,sizeof(DI_NETWORK_SET_INFO_t));
	memcpy(stSetInfo.stIpv4Addr.aucIpAddr,pNetworkInfo->ip_addr,4);
	memcpy(stSetInfo.stIpv4Addr.aucMaskAddr,pNetworkInfo->net_addr,4);

	eDiErr = DI_NETWORK_SetDevInfo(ulDevId, &stSetInfo);
	if(eDiErr != FDI_NO_ERR)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_SetInfo] DI_NETWORK_SetDevInfo err %d\n",eDiErr));
		return FDI_ERR;
	}
#endif
	return FDI_NO_ERR;
}
FDI_ERR_CODE FDI_WIFI_GetMacAddr(unsigned long ulIndex, unsigned char *pucMacAddr)
{
#if defined(CONFIG_DI10)
	struct ifreq ifreq;

	VK_MEM_Memset(&ifreq, 0, sizeof(struct ifreq));

	/* get mac addr */
	VK_strncpy(ifreq.ifr_name, DEVNAME, IF_NAME_SIZE-1);
	VK_MEM_Memset(pucMacAddr, 0, FDI_MAC_ADDR_LEN);
	if(ioctl(s_sockfd, SIOCGIFHWADDR, &ifreq) < 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_GetMacAddr] get mac info err"));
		return FDI_ERR;
	}
	VK_memcpy(pucMacAddr, &ifreq.ifr_hwaddr.sa_data, sizeof(struct ether_addr));

	return FDI_NO_ERR;
#else
	DI_ERR_CODE	eDiErr = DI_ERR_OK;
	unsigned long ulDevId;
	DI_NETWORK_GET_INFO_t stGetInfo;

	if(ulIndex >= FDI_WIFI_MAX_NUM)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_Connect] Input Param ulDevId Err %d\n",ulIndex));
		return FDI_ERR;
	}

	if(ulWiFiDev[ulIndex]==0xFF)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_Connect] DevId Err %d\n",ulWiFiDev[ulIndex]));
		return FDI_ERR;
	}
	ulDevId = ulWiFiDev[ulIndex];

	memset(&stGetInfo, 0, sizeof(DI_NETWORK_GET_INFO_t));

	eDiErr = DI_NETWORK_GetDevInfo(ulDevId, &stGetInfo);
	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_GetInfo] : DI_NETWORK_GetDevInfo Error!!\n"));
		return FDI_ERR;
	}
	FVK_MEM_Copy(pucMacAddr, stGetInfo.aucMacAddr, FDI_MAC_ADDR_LEN);

	return FDI_NO_ERR;
#endif
}


FDI_ERR_CODE FDI_WIFI_GetApInfo(unsigned long ulIndex, unsigned char *pucSsid)
{
#if defined(CONFIG_DI10)
	return FDI_NO_ERR;
#else
	DI_ERR_CODE	eDiErr = DI_ERR_OK;
	unsigned long ulDevId;
	DI_WIFI_INFO_t stApInfo;

	if(ulIndex >= FDI_WIFI_MAX_NUM)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_GetApInfo] Input Param ulDevId Err %d\n",ulIndex));
		return FDI_ERR;
	}

	if(ulWiFiDev[ulIndex]==0xFF)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_GetApInfo] DevId Err %d\n",ulWiFiDev[ulIndex]));
		return FDI_ERR;
	}
	ulDevId = ulWiFiDev[ulIndex];

	memset(&stApInfo, 0, sizeof(DI_WIFI_INFO_t));

	eDiErr = DI_WIFI_GetAPInfo(ulDevId, &stApInfo);
	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_GetApInfo] : DI_WIFI_GetAPInfo Error!!\n"));
		return FDI_ERR;
	}

	memcpy(pucSsid, &stApInfo.aucESSID[0], sizeof(stApInfo.aucESSID));

	return FDI_NO_ERR;
#endif
}

FDI_ERR_CODE FDI_WIFI_GetRSSI(unsigned long ulIndex, int *pnRSSI)
{
#if defined(CONFIG_DI10)
	return FDI_NO_ERR;
#else
	DI_ERR_CODE eDiErr = DI_ERR_OK;
	unsigned long ulDevId;
	DI_WIFI_RSSI_INFO_t RssiInfo;

	*pnRSSI = 0;

	if(ulIndex >= FDI_WIFI_MAX_NUM)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_Connect] Input Param ulDevId Err %d\n",ulIndex));
		return FDI_ERR;
	}

	if(ulWiFiDev[ulIndex]==0xFF)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_Connect] DevId Err %d\n",ulWiFiDev[ulIndex]));
		return FDI_ERR;
	}
	ulDevId = ulWiFiDev[ulIndex];

	eDiErr = DI_WIFI_GetRSSI(ulDevId, &RssiInfo);
	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_GetInfo] : DI_NETWORK_GetDevInfo Error!!\n"));
		return FDI_ERR;
	}

	*pnRSSI = RssiInfo.uiRssi;

	return FDI_NO_ERR;
#endif
}

FDI_ERR_CODE FDI_WIFI_GetEachAntRssi(unsigned long ulIndex, FDI_WIFI_RSSI_INFO_t *pRssiInfo)
{
#if defined(CONFIG_DI10)
#if defined(CONFIG_BCM43236)
	unsigned int i = 0;
	unsigned long ulDevId;
	FILE *fp;
	char input[129];
	char searchStr[32];
	char	*pLine;

	if((ulIndex >= FDI_WIFI_MAX_NUM)||(pRssiInfo == NULL))
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_GetEachAntRssi] Input Param ulDevId Err %d\n",ulIndex));
		return FDI_ERR;
	}

	if(ulWiFiDev[ulIndex]==0xFF)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_GetEachAntRssi] DevId Err %d\n", ulWiFiDev[ulIndex]));
		return FDI_ERR;
	}
	ulDevId = ulWiFiDev[ulIndex];

	VK_MEM_Memset(pRssiInfo, 0x0, sizeof(FDI_WIFI_RSSI_INFO_t));

	/* Get  RSSI */
	VK_SYSTEM_Command("wl rssi>/tmp/rssi.txt");
	fp = fopen("/tmp/rssi.txt","r");
	if(fp == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("P_WIFI_RunWlCmdForRssi] Err fopen %s\n",strerror( errno )));
		return FDI_ERR;
	}

	VK_MEM_Memset(input, 0x00, 129);

	while(fgets(input,128,fp)!= NULL)
	{
		/* Remove '\n' */
		if((pLine = strchr(input,'\n')) !=NULL)
		{
			*pLine='\0';
		}
		sscanf(input,"%d",&pRssiInfo->iRssi);
		FDI_PRINT(FDI_PRT_ERROR, ("[P_WIFI_RunWlCmdForRssi] Rssi = %d \n", pRssiInfo->iRssi));
	}
	fclose(fp);

	/* Get RSSI of each antena & Antena count */
	VK_SYSTEM_Command("wl phy_rssi_ant > /tmp/each_ant_rssi.txt");
	fp = fopen("/tmp/each_ant_rssi.txt","r");
	if(fp == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("P_WIFI_RunWlCmdForEachAntRssi] Err fopen %s\n",strerror( errno )));
		return FDI_ERR;
	}

	VK_MEM_Memset(input, 0x00, 129);

	pLine = fgets(input, 128, fp);
	if(pLine != NULL)
	{
		/* Remove '\n' */
		pLine = NULL;
		if((pLine = strchr(input,'\n')) != NULL)
		{
			*pLine='\0';
		}

		for(i=0; i<FDI_WIFI_ANT_MAX; i++)
		{
			pLine = NULL;
			VK_MEM_Memset(searchStr, 0x00, sizeof(searchStr));
			snprintf(searchStr, sizeof(searchStr), "rssi[%d] ", i);

			if((pLine = strstr(input, searchStr)) != NULL)
			{
				sscanf(pLine+strlen(searchStr), "%d", &(pRssiInfo->iAntRssi[i]));
				FDI_PRINT(FDI_PRT_ERROR, ("[P_WIFI_RunWlCmdForRssi] Antena[%d] rssi = %d \n",i, pRssiInfo->iAntRssi[i]));
			}
			else
			{
				pRssiInfo->uiAntCount = i;
				FDI_PRINT(FDI_PRT_ERROR, ("[P_WIFI_RunWlCmdForRssi] Antena Count = %d \n",pRssiInfo->uiAntCount));
				break;
			}
		}
	}

	fclose(fp);

	return FDI_NO_ERR;
#endif
#else
	DI_ERR_CODE eDiErr = DI_ERR_OK;
	unsigned int i = 0,tmp = 0;
	unsigned long ulDevId;
	DI_WIFI_RSSI_INFO_t RssiInfo;

	if((ulIndex >= FDI_WIFI_MAX_NUM)||(pRssiInfo == NULL))
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_GetEachAntRssi] Input Param ulDevId Err %d\n",ulIndex));
		return FDI_ERR;
	}

	if(ulWiFiDev[ulIndex]==0xFF)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_GetEachAntRssi] DevId Err %d\n", ulWiFiDev[ulIndex]));
		return FDI_ERR;
	}
	ulDevId = ulWiFiDev[ulIndex];

	eDiErr = DI_WIFI_GetRSSI(ulDevId, &RssiInfo);
	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[DI_WIFI_GetRSSI] : DI_NETWORK_GetDevInfo Error!!\n"));
		return FDI_ERR;
	}

	for(i = 0; i < RssiInfo.iAntCount; i++)
	{
		if(RssiInfo.uiAntRssi[i] < 0)
		{
			tmp = (-1*RssiInfo.uiAntRssi[i]);
			RssiInfo.uiAntRssi[i] = tmp;
		}
	}

    //copy DI -> FDI
	pRssiInfo->uiAntCount = RssiInfo.iAntCount;
	pRssiInfo->iRssi = RssiInfo.uiRssi;
	for(i = 0; i < FDI_WIFI_ANT_MAX; i++)
		pRssiInfo->iAntRssi[i] = RssiInfo.uiAntRssi[i];

	return FDI_NO_ERR;
#endif
}

FDI_ERR_CODE FDI_WIFI_SetDevStatus(unsigned long ulIndex,HBOOL bUp)
{
#if defined(CONFIG_DI10)
	return FDI_NO_ERR;
#else
	DI_ERR_CODE eDiErr;
	unsigned long ulDevId;

	if(ulIndex >= FDI_WIFI_MAX_NUM)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_Connect] Input Param ulDevId Err %d\n",ulIndex));
		return FDI_ERR;
	}

	if(ulWiFiDev[ulIndex]==0xFF)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_WIFI_Connect] DevId Err %d\n",ulWiFiDev[ulIndex]));
		return FDI_ERR;
	}
	ulDevId = ulWiFiDev[ulIndex];

	if(bUp == TRUE)
	{
		eDiErr = DI_NETWORK_DevUp(ulDevId);
	}
	else
	{
		eDiErr = DI_NETWORK_DevDown(ulDevId);
	}

	if(eDiErr != DI_ERR_OK)
	{
		return FDI_ERR;
	}

	return FDI_NO_ERR;
#endif
}


