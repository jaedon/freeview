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
#if !defined(CONFIG_OS_RTOS)
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
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <stdlib.h>
/* chip manufacture's  header files */

/* humax header files */
#include "fdi_common.h"
#include "fdi_ethernet.h"

#include "htype.h"
#include "vkernel.h"
#include "di_err.h"
#include "di_network.h"
#if defined(CONFIG_PLATFORM_ALI)
#include "hui_socket.h"
#endif

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define DI_ETH_SET_IPADDR		0x01
#define DI_ETH_SET_NETMASK		0x02
#define DI_ETH_SET_BROADCAST	0x04
#define DI_ETH_SET_PROMISC		0x08
#define DI_ETH_SET_IFFUP		0x10
#define DI_ETH_SET_FLAGS		0x20

#define SET_MASK        (0x01)
#define CLR_MASK        (0x02)

#define RTF_UP          0x0001		/* route usable                 */
#define RTF_GATEWAY     0x0002	/* destination is a gateway     */

#ifndef	SIOCGLINKSTATE
#define	SIOCGLINKSTATE	(SIOCDEVPRIVATE + 0)	//	from if_net.h file.
#endif

#if defined(CONFIG_DI10)
#define DEVNAME "eth0"

#define IS_VALID_IPV4_ADDR(ipv4_addr) 	VK_memcmp(s_aucIpv4Null, ipv4_addr, FDI_IPV4_ADDR_LEN)
#define IS_VALID_IPV6_ADDR(ipv6_addr) 	VK_memcmp(s_aucIpv6Null, ipv6_addr, FDI_IPV4_ADDR_LEN)
#endif

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
#if defined(CONFIG_DI10)
static int s_sockfd;

const static HUINT8 s_aucMacNull[MAC_ADDR_LEN] = {0, };
const static HUINT8 s_aucIpv4Null[IPV4_ADDR_LEN] = {0, };
const static HUINT8 s_aucIpv6Null[IPV6_ADDR_LEN] = {0, };
#endif
/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_ETHERNET_GetDevCount(unsigned long *pulCount)
{
#if defined(CONFIG_PLATFORM_ALI) || defined(CONFIG_DI10)
	*pulCount = 1;

	return FDI_NO_ERR;
#else
	DI_ERR_CODE eDiErr;
	DI_NETWORK_GET_INFO_t stGetInfo;
	unsigned long ulEthCount, i;
	HUINT32	 ulCount;

	eDiErr = DI_NETWORK_GetDevCount(&ulCount);
	if (DI_ERR_OK != eDiErr)
	{
		return FDI_ERR;
	}

	ulEthCount = 0;
	for(i=0; i<ulCount; i++)
	{
		eDiErr = DI_NETWORK_GetDevInfo(i, &stGetInfo);
		if(eDiErr == DI_ERR_OK)
		{
			if((stGetInfo.eDevType != DI_NETWORK_DEV_WIFI) /*&& (stGetInfo.eDevType != DI_NETWORK_DEV_CABLEMODEM)*/)
			{
				ulEthCount++;
			}
		}
	}
	*pulCount = ulEthCount;

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
FDI_ERR_CODE FDI_ETHERNET_Open(unsigned long ulDevId)
{
#if defined(CONFIG_PLATFORM_ALI)
	(void)ulDevId;
	return FDI_NO_ERR;
#elif defined(CONFIG_DI10)
	struct ifreq ifreq;

	VK_MEM_Memset(&ifreq, 0, sizeof(struct ifreq));

	s_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(s_sockfd < 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_Open] : Socket open error!!\n"));
		return FDI_ERR;
	}

	VK_strncpy(ifreq.ifr_name, DEVNAME, IF_NAME_SIZE-1);
	if(ioctl(s_sockfd, SIOCGIFFLAGS, (char *)&ifreq) < 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_Open] : get device info error!!\n"));
		return FDI_ERR;
	}

	ifreq.ifr_flags |= (IFF_UP|IFF_RUNNING);
	ifreq.ifr_flags &= (~IFF_DYNAMIC);
	VK_strncpy(ifreq.ifr_name, DEVNAME, IF_NAME_SIZE-1);
	if(ioctl(s_sockfd, SIOCSIFFLAGS, (char *)&ifreq) < 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_Open] : set device info error!!\n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
#else
	DI_NETWORK_GET_INFO_t stGetInfo;
	DI_ERR_CODE eDiErr;

	VK_memset(&stGetInfo, 0, sizeof(DI_NETWORK_GET_INFO_t));

	eDiErr = DI_NETWORK_GetDevInfo(ulDevId, &stGetInfo);
	if(eDiErr != DI_ERR_OK)
	{
		return FDI_ERR;
	}

	if((stGetInfo.eDevType != DI_NETWORK_DEV_WIFI) && (stGetInfo.eDevType != DI_NETWORK_DEV_CABLEMODEM) && !stGetInfo.bIsUp)
	{
		eDiErr = DI_NETWORK_DevUp(ulDevId);
		if(eDiErr != DI_ERR_OK)
		{
			return FDI_ERR;
		}
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
FDI_ERR_CODE FDI_ETHERNET_SetInfo(unsigned long ulDevId, FDI_ETHERNET_INFO_t* pNetworkInfo)
{
#if defined(CONFIG_PLATFORM_ALI)
	HUI_IP_LOC_CFG_T cfg;
	int rc;

	VK_MEM_Memset(&cfg, 0, sizeof(HUI_IP_LOC_CFG_T));

	cfg.ip_addr = (pNetworkInfo->stIpv4Addr.aucIpAddr[0]) | \
		          (pNetworkInfo->stIpv4Addr.aucIpAddr[1] << 8) | \
		          (pNetworkInfo->stIpv4Addr.aucIpAddr[2] << 16) | \
		          (pNetworkInfo->stIpv4Addr.aucIpAddr[3] << 24);
	cfg.gateway = (pNetworkInfo->aucGwAddr[0]) | \
		          (pNetworkInfo->aucGwAddr[1] << 8) | \
		          (pNetworkInfo->aucGwAddr[2] << 16) | \
		          (pNetworkInfo->aucGwAddr[3] << 24);
	cfg.subnet_mask = (pNetworkInfo->stIpv4Addr.aucMaskAddr[0]) | \
		              (pNetworkInfo->stIpv4Addr.aucMaskAddr[1] << 8) | \
		              (pNetworkInfo->stIpv4Addr.aucMaskAddr[2] << 16) | \
		              (pNetworkInfo->stIpv4Addr.aucMaskAddr[3] << 24);

	rc = HUI_INET_Setipcfg(HUI_SOCK_DEVTYPE_USB2ETH, &cfg);
	if (rc < 0)
		return FDI_ERR;

	return FDI_NO_ERR;
#elif defined(CONFIG_DI10)
	struct ifreq ifreq;
	struct sockaddr_in *sin;
	unsigned char *addr;

	VK_strncpy(ifreq.ifr_name, DEVNAME, IF_NAME_SIZE-1);
	sin = (struct sockaddr_in *)&ifreq.ifr_addr;
	VK_MEM_Memset(sin, 0, sizeof(struct sockaddr));
	sin->sin_family = AF_INET;

	addr = pNetworkInfo->stIpv4Addr.aucIpAddr;
	if(IS_VALID_IPV4_ADDR(addr))
	{
		sin->sin_addr.s_addr = addr[3] << 24 | addr[2] << 16 | addr[1] << 8 | addr[0];
		if(ioctl(s_sockfd, SIOCSIFADDR, (char *)&ifreq) < 0)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_SetInfo] :  set ip address  error!!\n"));
			return FDI_ERR;
		}
	}

	addr = pNetworkInfo->stIpv4Addr.aucBcastAddr;
	if(IS_VALID_IPV4_ADDR(addr))
	{
		sin->sin_addr.s_addr = addr[3] << 24 | addr[2] << 16 | addr[1] << 8 | addr[0];
		if(ioctl(s_sockfd, SIOCSIFBRDADDR, (char *)&ifreq) < 0)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_SetInfo] :  set broadcast address  error!!\n"));
			return FDI_ERR;
		}
	}

	addr = pNetworkInfo->stIpv4Addr.aucMaskAddr;
	if(IS_VALID_IPV4_ADDR(addr))
	{
		sin->sin_addr.s_addr = addr[3] << 24 | addr[2] << 16 | addr[1] << 8 | addr[0];
		if(ioctl(s_sockfd, SIOCSIFNETMASK, (char *)&ifreq) < 0)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_SetInfo] :  set netmask error!!\n"));
			return FDI_ERR;
		}
	}

	return FDI_NO_ERR;
#else
	DI_NETWORK_SET_INFO_t stSetInfo;
	DI_ERR_CODE eDiErr;

	FVK_MEM_Set(&stSetInfo, 0, sizeof(DI_NETWORK_SET_INFO_t));

	FVK_MEM_Copy(&stSetInfo.stIpv4Addr, &pNetworkInfo->stIpv4Addr, sizeof(FDI_ETHERNET_IPV4_ADDR_t));
	eDiErr = DI_NETWORK_SetDevInfo(ulDevId, &stSetInfo);
	if(eDiErr != DI_ERR_OK)
	{
		return FDI_ERR;
	}
#if 0
	eDiErr = DI_NETWORK_SetDefaultRoute(ulDevId, DI_NETWORK_IP_VER_4, pNetworkInfo->aucGwAddr);
	if(eDiErr != DI_ERR_OK)
	{
		return FDI_ERR;
	}
#endif
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
FDI_ERR_CODE FDI_ETHERNET_GetInfo(unsigned long ulDevId, FDI_ETHERNET_INFO_t* pNetworkInfo)
{
#if defined(CONFIG_PLATFORM_ALI) || defined(CONFIG_DI10)
	(void)ulDevId;

	pNetworkInfo->eDevType = FDI_ETHERNET_DEV_LAN;
	VK_MEM_Memset(pNetworkInfo->stIpv4Addr.aucIpAddr, 0, FDI_IPV4_ADDR_LEN);
	return FDI_NO_ERR;
#else
	DI_NETWORK_GET_INFO_t stGetInfo;
	DI_ERR_CODE eDiErr;

	VK_memset(&stGetInfo, 0, sizeof(DI_NETWORK_GET_INFO_t));

	eDiErr = DI_NETWORK_GetDevInfo(ulDevId, &stGetInfo);
	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_GetInfo] : DI_NETWORK_GetDevInfo Error!!\n"));
		return FDI_ERR;
	}
	FVK_MEM_Copy(pNetworkInfo->szName, stGetInfo.szName, FDI_IF_NAME_SIZE);
	pNetworkInfo->eDevType = stGetInfo.eDevType;
	FVK_MEM_Copy(pNetworkInfo->aucMacAddr, stGetInfo.aucMacAddr, FDI_MAC_ADDR_LEN);
	pNetworkInfo->bIsUp = stGetInfo.bIsUp;
	pNetworkInfo->bIsConnected = stGetInfo.bIsConnected;
	pNetworkInfo->bIsDynamic = stGetInfo.bIsDynamic;
	FVK_MEM_Copy(&pNetworkInfo->stIpv4Addr, &stGetInfo.stIpv4Addr, sizeof(FDI_ETHERNET_IPV4_ADDR_t));

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
FDI_ERR_CODE FDI_ETHERNET_ARPTest(unsigned long ulDevId, char *szIpAddr, unsigned char* pucMacAddr, int waitSecond) /* from DI */
{
#if defined(CONFIG_PLATFORM_ALI)
	(void)ulDevId;
	(void)*szIpAddr;
	(void)pucMacAddr;
	(void)waitSecond;

	return FDI_NO_ERR;
#else
	int sock;
	struct ifreq ifr;
	struct sockaddr from, myaddr;
	unsigned int fromlen=0;
	unsigned char hostMAC[MAC_ADDR_LENGTH] = {0x00, };
	arp_packet_t packet;
	FDI_ETHERNET_INFO_t pNetworkInfo;
	FDI_ERR_CODE eFdiErr = FDI_ERR;

	int		 packetReceived = 0;
	unsigned long startTick, curTick;

	if( szIpAddr == NULL || pucMacAddr == NULL || waitSecond < 0 )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_ARPTest] : Invalid Parameters!!\n"));
		return FDI_ERR;
	}

	FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_ARPTest] : >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n"));
	FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_ARPTest] ulDevId(%d), szIpAddr(%s), pucMacAddr(%s) waitSecond(%d)\n", ulDevId, szIpAddr,pucMacAddr, waitSecond ));

	/* Check cable Disconnect */
	if( FDI_ETHERNET_IsLinked(ulDevId) == FALSE )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_ARPTest] : Ethernet is not connected!!\n"));
		return FDI_ERR;
	}

	eFdiErr = FDI_ETHERNET_GetInfo(ulDevId, &pNetworkInfo);
	if( eFdiErr != FDI_NO_ERR )
	{
		return FDI_ERR;
	}
	memcpy(hostMAC, pNetworkInfo.aucMacAddr, MAC_ADDR_LENGTH);

	sock = socket(AF_INET, SOCK_PACKET, htons(0x0003));
	if(sock == -1)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_ARPTest] : socket!!\n"));
#if !defined(CONFIG_OS_RTOS)
		perror("[FDI_ETHERNET_ARPTest] socket error!");
#endif
		return FDI_ERR;
	}

	memset((char *)&ifr, 0, sizeof(struct ifreq));
	sprintf(ifr.ifr_name, pNetworkInfo.szName);
	if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_ARPTest] : ioctl(SIOCGIFHWADDR)!!\n"));
#if !defined(CONFIG_OS_RTOS)
		perror("[FDI_ETHERNET_ARPTest] socket ioctl error!");
#endif
		close(sock);
		return FDI_ERR;
	}
	memcpy(packet.arphdr.sha, ifr.ifr_hwaddr.sa_data, ETH_LEN);

	memset(packet.ethhdr.dhost, 0xFF, ETH_LEN);
	memcpy(packet.ethhdr.shost, ifr.ifr_hwaddr.sa_data, ETH_LEN);

	packet.ethhdr.type = htons(ARP);
	packet.arphdr.hdr = htons(ETHERNET);
	packet.arphdr.proto = htons(0x0800);
	packet.arphdr.hlen = ETH_LEN;
	packet.arphdr.plen = 4;
	packet.arphdr.op = htons(ARP_REQUEST);

	memset((char *)&ifr, 0, sizeof(struct ifreq));
	sprintf(ifr.ifr_name, pNetworkInfo.szName);
	if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_ARPTest] : ioctl(SIOCGIFADDR)!!\n"));
#if !defined(CONFIG_OS_RTOS)
		perror("[FDI_ETHERNET_ARPTest] socket ioctl error!");
#endif
		close(sock);
		return FDI_ERR;
	}
	packet.arphdr.sip = ((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr.s_addr;
	memset(packet.arphdr.dha, 0, ETH_LEN);
	packet.arphdr.dip = inet_addr(szIpAddr);

	memset(&myaddr, 0, sizeof(myaddr));
	myaddr.sa_family = AF_INET;
	memcpy(myaddr.sa_data, pNetworkInfo.szName, sizeof(myaddr.sa_data));

	memset(&from, 0, sizeof(from));
	from.sa_family = AF_INET;
	memcpy(from.sa_data, pNetworkInfo.szName, sizeof(pNetworkInfo.szName));

	if(bind(sock, &myaddr, sizeof(myaddr)) == -1)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_ARPTest] : bind!!\n"));
#if !defined(CONFIG_OS_RTOS)
		perror("[FDI_ETHERNET_ARPTest] socket bind error!");
#endif
		close(sock);
		return FDI_ERR;
	}

	startTick = time(NULL);

	if(-1 == sendto(sock, &packet, sizeof(packet), 0, &from, sizeof(from)))
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_ARPTest] : sendto!!\n"));
#if !defined(CONFIG_OS_RTOS)
		perror("[FDI_ETHERNET_ARPTest] socket sendto error!");
#endif
		close(sock);
		return FDI_ERR;
	}

	if ((fcntl (sock, F_SETFL, O_NONBLOCK)) == -1)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_ARPTest] : fcntl!!\n"));
#if !defined(CONFIG_OS_RTOS)
		perror("[FDI_ETHERNET_ARPTest] socket fcntl error!");
#endif
		close(sock);
		return FDI_ERR;
	}

	while (1)
	{
		arp_packet_t *parp = NULL;
		unsigned char buffer[1500] = {0,};

		int i;

		parp =(arp_packet_t *) buffer;

		if(recvfrom(sock, buffer, sizeof(buffer), 0, &from, &fromlen) != -1)
		{
			if((parp->ethhdr).type == htons(ARP) && memcmp((*parp).ethhdr.dhost, hostMAC, MAC_ADDR_LENGTH) == 0)
			{
				if (packet.arphdr.dip != (*parp).arphdr.sip)  /* 딴곳에서 들어온 ARP Packet은 무시하도록 함. */
				{
					continue;
				}

				for(i = 0; i<ETH_LEN; i++)
				{
					pucMacAddr[i]= (*parp).arphdr.sha[i];
				}
				packetReceived = 1;
				break;
			}
		}

	  /* Packet 이 하나도 없으면 먹통 ?? */
		 curTick = time(NULL);
		 if ((curTick < startTick) || (curTick - startTick > (unsigned long) (waitSecond)))
					break;
		 sleep(1);

	}

	if (packetReceived == 0)
	{
		close(sock);
		return FDI_ERR;
	}

	close(sock);
	FDI_PRINT(0, ("[FDI_ETHERNET_ARPTest] : %02x-%02x-%02x-%02x-%02x-%02x\n",
					pucMacAddr[0], pucMacAddr[1], pucMacAddr[2], pucMacAddr[3], pucMacAddr[4], pucMacAddr[5]));

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
BOOL FDI_ETHERNET_IsLinked(unsigned long ulDevId)
{
#if defined(CONFIG_PLATFORM_ALI)
	return HUI_INET_GetLinkState(HUI_SOCK_DEVTYPE_USB2ETH);
#elif defined(CONFIG_DI10)
	struct ifreq ifreq;
	struct ethtool_value eth_data;
	BOOL flag = FALSE;

	VK_MEM_Memset(&ifreq, 0, sizeof(struct ifreq));

	VK_strncpy(ifreq.ifr_name, DEVNAME, IF_NAME_SIZE-1);
	eth_data.cmd = ETHTOOL_GLINK;
	ifreq.ifr_data = (caddr_t)&eth_data;
	if(ioctl(s_sockfd, SIOCETHTOOL, (char *)&ifreq) < 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_IsLinked] : get ethtool info failed\n"));
		return FALSE;
	}

	if(eth_data.data)
	{
		flag = TRUE;
	}
	else
	{
		flag = FALSE;
	}

	return flag;
#else
	DI_NETWORK_GET_INFO_t stGetInfo;
	DI_ERR_CODE eDiErr;

	VK_memset(&stGetInfo, 0, sizeof(DI_NETWORK_GET_INFO_t));

	eDiErr = DI_NETWORK_GetDevInfo(ulDevId, &stGetInfo);
	if(eDiErr != DI_ERR_OK)
	{
		return FALSE;
	}

	return stGetInfo.bIsConnected;
#endif
}

