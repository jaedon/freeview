#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/ether.h>
#include <stddef.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "linden_trace.h"

#include "drv_err.h"
#include "drv_micom.h"
#include "vkernel.h"
#include "di_ethernet.h"
#include "di_uart.h"

#define DEBUG_MODULE        TRACE_MODULE_DI_FAN	//temp

//#define CONFIG_CONFIG_FILE
#if 0
#define DI_ETHERNET_DEBUG

#ifdef DI_ETHERNET_DEBUG
#define PrintDebug		VK_printf
#define PrintError		VK_printf
#else
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		VK_printf
#endif

#ifdef DI_ETHERNET_DEBUG
#define PrintEnter		VK_printf("(+)%s\n", __FUNCTION__)
#define PrintExit		VK_printf("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter		while (0) ((int (*)(char *, ...)) 0)
#define PrintExit		while (0) ((int (*)(char *, ...)) 0)
#endif
#endif

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

#define UNUSED(x) (void)(x)

#if defined(CONFIG_ETHERNET)
#if defined(CONFIG_OS_UCOS)
#define SIZE_NET_PATH	0x100

static HBOOL			s_bEthUp = FALSE;

#else
static char* P_ETH_Convert_Address(unsigned char * address, char *buf)
{
	VK_memset(buf,0,16);

	// sizeof(buf) 의 값이 4로 나온다. ip address의 용도이므로 16으로 고정한다.
	VK_snprintf(buf, 16, "%d.%d.%d.%d",
		address[0] & 0xff,
		address[1] & 0xff,
		address[2] & 0xff,
		address[3] & 0xff);

	return buf;
}

static DRV_Error P_ETH_Call_System(char *command)
{
	int status;
	pid_t pid;

	if (command == NULL)
		return DRV_ERR;

	pid = fork();

	if (pid == 0)
	{
		char *argv[4];
		argv[0] = "sh";
		argv[1] = "-c";
		argv[2] = command;
		argv[3] = 0;
		execve("/bin/sh", argv, environ);

		PrintError("failed to exec \n\r");
		exit(0);
	}
	else if (pid < 0)
	{
		PrintError("ERROR :: fork\n\r");
		return DRV_ERR;
	}
	else
	{
			PrintDebug("wait\n\r");
			if (waitpid (pid, &status, 0) != pid)
			{
				PrintError("ERROR :: waitpid\n\r");

				return DRV_ERR;
			}
	}

	PrintDebug("[ %s ] status = %d\n\r",__FUNCTION__,status);

	return DRV_OK;
}

#if defined(CONFIG_CONFIG_FILE)

/*
ifconfig eth0 10.0.13.53 broadcast 10.0.13.255 netmask 255.255.255.0 up
route add default gw 10.0.13.1 eth0
echo "nameserver 10.0.0.5" >> /etc/resolv.conf
*/
static DRV_Error P_ETH_SaveManualSetting(char *command)
{
	char buf[128];

	FILE *fp = fopen("/var/lib/humaxtv/network/network.conf", "a+");

	if (fp == NULL)
	{
		return DRV_ERR;
	}

	VK_snprintf(buf, sizeof(buf) - 1, "%s\n\r",
		command);

	fputs(command, fp);

	fclose(fp);

	return DRV_OK;
}

static DRV_Error P_ETH_SaveDhcpSetting(void)
{
	DRV_Error returnValue = DRV_ERR;

	char buf[128];

	FILE *fp = fopen("/var/lib/humaxtv/network/network.conf", "r");

	if (fp == NULL)
	{
		return DRV_ERR;
	}

	fclose(fp);

	VK_snprintf(buf, sizeof(buf) - 1, "%s %s",
		"rm","/var/lib/humaxtv/network/network.conf");

	returnValue = P_ETH_Call_System (buf);

	if(returnValue != DRV_OK)
	{
		return DRV_ERR;
	}

	return DRV_OK;
}
#endif

/* 10.0.13.1  ==  0A000D01*/
// gateway address가 존재 할 경우에만 DI_NO_ERR return함.
DRV_Error DRV_ETH_GetAddr_Gateway(unsigned char* aucGateAddr)
{
	char devname[64];
	unsigned long int d, gatewayAddr, m;
	unsigned int flgs, ref, use, metric, mtu, win, ir;

	DRV_Error	returnValue = DRV_ERR;

	FILE *fp = fopen("/proc/net/route", "r");

	if (fp == NULL)
	{
		return DRV_ERR;
	}

	if (fscanf(fp, "%*[^\n]\n") < 0)
	{
		fclose(fp);
		return returnValue;
	}

	while (1) {
		int readNumber;

		readNumber = fscanf(fp, "%63s%lx%lx%u%u%u%u%lx%u%u%u\n",
				   devname, &d, &gatewayAddr, &flgs, &ref, &use, &metric, &m,
				   &mtu, &win, &ir);

		if (readNumber != 11)
		{
			if ((readNumber < 0) && feof(fp))  /* EOF with no (nonspace) chars read. */
			{
				break;
			}
		}

		if (!(flgs & RTF_UP)) /* Skip interfaces that are down. */
		{
			continue;
		}

		PrintDebug("[ %s ] Get Gateway Addr = %lx\n\r",__FUNCTION__,gatewayAddr);

		if ((flgs & RTF_GATEWAY))
		{
			returnValue = DRV_OK;

			aucGateAddr[0]= ( (0xff) & (gatewayAddr >> 6*4 ));
			aucGateAddr[1]= ( (0xff) & (gatewayAddr >> 4*4 ));
			aucGateAddr[2]= ( (0xff) & (gatewayAddr >> 2*4 ));
			aucGateAddr[3]= ( (0xff) & (gatewayAddr ));

			break;
		}
	}

	fclose(fp);

	return returnValue;
}

DRV_Error DRV_ETH_SetAddr_Gateway(unsigned char* aucGateAddr)
{
	DRV_Error	returnValue = DRV_ERR;
	char buf[128];
	char gateway_addr[16];
	unsigned char readCurrentGateAddr[4];

	VK_memset(buf,0,128);
	VK_memset(gateway_addr,0,16);

	while(1)
	{
		VK_memset(gateway_addr,0,4);

		if(( DRV_ETH_GetAddr_Gateway(readCurrentGateAddr)) == DRV_OK)
		{
			PrintDebug("[ %s ] Gateway\t%s\n",__FUNCTION__,readCurrentGateAddr);

			P_ETH_Convert_Address(readCurrentGateAddr, gateway_addr);

			VK_snprintf(buf, sizeof(buf) - 1,"%s %s %s"
				,"route del default gw"
				,gateway_addr
				,"eth0");

			PrintDebug("[ %s ] %s\n\r",__FUNCTION__,buf);

			returnValue = P_ETH_Call_System (buf);

			if(returnValue != DRV_OK)
			{
				return DRV_ERR;
			}

			VK_memset(buf,0,128);
			VK_memset(gateway_addr,0,16);

		}
		else
		{
			break;
		}
	}

	P_ETH_Convert_Address(aucGateAddr, gateway_addr);

	PrintDebug("Gateway\t%s\n",gateway_addr);

	VK_snprintf(buf, sizeof(buf) - 1, "%s %s %s"
		,"route add default gw "
		,gateway_addr
		,"eth0");

	PrintDebug("[ %s ] %s\n\r",__FUNCTION__, buf);

	returnValue = P_ETH_Call_System (buf);

	if(returnValue != DRV_OK)
	{
		return DRV_ERR;
	}
#if defined(CONFIG_CONFIG_FILE)
	returnValue = P_ETH_SaveManualSetting(buf);

	if(returnValue != DRV_OK)
	{
		return DRV_ERR;
	}
#endif

	return DRV_OK;
}

DRV_Error DRV_ETH_GetAddr_DNS(unsigned char* aucDNSAddr)
{
	int readNumber = 0;

	char dnsTempString[11];

	int a,b,c,d;

// 경로 변경 : /tmp/resolv.conf --> /etc/resolv.conf
	FILE *fp = fopen("/etc/resolv.conf", "r");

	if (fp == NULL)
	{
		PrintDebug("[ %s ] fopen error\n",__FUNCTION__);
		return DRV_ERR;
	}

	readNumber = fscanf(fp, "%10s %d.%d.%d.%d",
			   dnsTempString,
			   &a,
			   &b,
			   &c,
			   &d);

	aucDNSAddr[0]=a;
	aucDNSAddr[1]=b;
	aucDNSAddr[2]=c;
	aucDNSAddr[3]=d;


	PrintDebug("[%s] Get DNS :: %10s %d.%d.%d.%d\n",
			__FUNCTION__,
			   dnsTempString,
			   aucDNSAddr[0],
			   aucDNSAddr[1],
			   aucDNSAddr[2],
			   aucDNSAddr[3]);

	if( readNumber != 5 )
	{
		fclose(fp);

		return DRV_ERR;
	}

	fclose(fp);

	return DRV_OK;

}

DRV_Error DRV_ETH_SetAddr_DNS(unsigned char* aucDNSAddr)
{
#if defined(CONFIG_CONFIG_FILE)
	DRV_Error returnValue = DRV_ERR;
#endif
	char buf[128];
	char dns_addr[16];

// 경로 변경 : /tmp/resolv.conf --> /etc/resolv.conf
	FILE *fp = fopen("/etc/resolv.conf", "w+");

	if (fp == NULL)
	{
		return DRV_ERR;
	}

	VK_memset(buf,0,128);
	VK_memset(dns_addr,0,16);

	PrintDebug("[ %s ] %s\n\r",__FUNCTION__," set DNS");
	P_ETH_Convert_Address(aucDNSAddr, dns_addr);

	VK_snprintf(buf, sizeof(buf) - 1, "%s %s\n"
		,"nameserver"
		,dns_addr);

	fputs(buf, fp);

	fclose(fp);
#if defined(CONFIG_CONFIG_FILE)
//	echo "nameserver 10.0.0.5" >> /etc/resolv.conf
	VK_memset(buf,0,128);

// 경로 변경 : /tmp/resolv.conf --> /etc/resolv.conf
	VK_snprintf(buf, sizeof(buf) - 1, "echo \"%s %s\" >> /etc/resolv.conf"
		,"nameserver"
		,dns_addr);

	returnValue = P_ETH_SaveManualSetting(buf);

	if(returnValue != DRV_OK)
	{
		return DRV_ERR;
	}
#endif
	return DRV_OK;
}

DRV_Error DRV_ETH_Network_SetInfo(DI_ETH_NETWORK_INFO* pNetworkInfo)
{
	DRV_Error returnValue = DRV_ERR;
	char buf[128];
	char ip_addr[16];
	char net_addr[16];
	char brd_addr[16];

	VK_memset(buf,0,128);
	VK_memset(ip_addr,0,16);
	VK_memset(net_addr,0,16);
	VK_memset(brd_addr,0,16);

	P_ETH_Convert_Address(pNetworkInfo->ip_addr, ip_addr);
	P_ETH_Convert_Address(pNetworkInfo->net_addr, net_addr);
	P_ETH_Convert_Address(pNetworkInfo->brd_addr, brd_addr);

	VK_snprintf(buf, sizeof(buf) - 1, "%s %s %s %s %s %s %s"
		,"ifconfig eth0"
		,ip_addr
		,"netmask"
		,net_addr
		,"broadcast"
		,brd_addr
		,"up");

	PrintDebug("[ %s ] %s\n\r",__FUNCTION__,buf);

	returnValue = P_ETH_Call_System (buf);

	if(returnValue != DRV_OK)
	{
		return DRV_ERR;
	}
#if defined(CONFIG_CONFIG_FILE)
	returnValue = P_ETH_SaveManualSetting(buf);

	if(returnValue != DRV_OK)
	{
		return DRV_ERR;
	}

#endif
	if (pNetworkInfo->gate_addr[0] != 0x00)
	{
		returnValue = DRV_ETH_SetAddr_Gateway(pNetworkInfo->gate_addr);

		if(returnValue != DRV_OK)
		{
			return DRV_ERR;
		}
	}

	if (pNetworkInfo->dns_addr[0] != 0x00)
	{
		returnValue = DRV_ETH_SetAddr_DNS(pNetworkInfo->dns_addr);

		if(returnValue != DRV_OK)
		{
			return DRV_ERR;
		}
	}

	return DRV_OK;
}


//ifconfig | grep -A 1 eth1 | grep inet | awk '{print $2}' | sed -e 's@addr:@@'
//201.74.194.216
DRV_Error DRV_ETH_Network_GetInfo(DI_ETH_NETWORK_INFO* pNetworkInfo)
{
	struct ifreq *ifr;
	struct sockaddr_in *sin;
	struct sockaddr *sa;

	struct ifconf ifcfg;
	int fd;
	int n, rc;
	int numreqs = 30;
	unsigned char *pMacAddr;
	char	buf[16];

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	if (fd < 0)
	{
		PrintError("Socket Error \n\r");
		return DRV_ERR;
	}

	VK_memset(&ifcfg, 0, sizeof(ifcfg));
	ifcfg.ifc_buf = NULL;
	ifcfg.ifc_len = sizeof(struct ifreq) * numreqs;
	ifcfg.ifc_buf = VK_malloc((unsigned int)ifcfg.ifc_len);

//20100312 :: for문 삭제. condition을 보고 break가 되어야 되지만 일단은 한번만 값을 읽는 것으로 수정.
//	for(;;)
//	{
		ifcfg.ifc_len = sizeof(struct ifreq) * numreqs;
		ifcfg.ifc_buf = realloc(ifcfg.ifc_buf,(unsigned int) ifcfg.ifc_len);

		if (ioctl(fd, SIOCGIFCONF, (char *)&ifcfg) < 0)
		{
			PrintError("SIOCGIFCONF ");
			close(fd);
			return DRV_ERR;
		}

		PrintDebug("%d / %d = %d\n", ifcfg.ifc_len, sizeof(struct ifreq), ifcfg.ifc_len/sizeof(struct ifreq));
//		break;
//	}

	ifr = ifcfg.ifc_req;

	//20080311 :: TestByPooh ::
	//Must  have always two connection ^^ or ...... fix....
	//one is loop back device, and another is eth0
	if((ifcfg.ifc_len/sizeof(struct ifreq)) < 1 )
	{
		close(fd);
		return DRV_ERR;
	}
	VK_memset(pNetworkInfo, 0, sizeof(DI_ETH_NETWORK_INFO));

	for (n = 0; n < ifcfg.ifc_len; n+= sizeof(struct ifreq))
	{
		PrintDebug("\nNetwork Info\n");
		PrintDebug("[%s]\n", ifr->ifr_name);
		sin = (struct sockaddr_in *)&ifr->ifr_addr;
		PrintDebug("IP\t%s\n", inet_ntoa(sin->sin_addr) );

		if ((sin->sin_addr.s_addr) == htonl(INADDR_LOOPBACK) )
		{
			PrintDebug("\tLoop Back\n");

			// loop back device인 경우 ifr++를 해주지 않아 다시 loop back device를 읽는 문제 수정
			ifr++;
			continue;
		}
		else
		{
			rc = ioctl(fd, SIOCGIFHWADDR, (char *)ifr);
			if(rc < 0){
				close(fd);
				return DRV_ERR;
			}
			sa = &ifr->ifr_hwaddr;

			pMacAddr = (unsigned char *)sa->sa_data;
			VK_memcpy(pNetworkInfo->mac_addr, pMacAddr, MAC_ADDR_LENGTH);

			PrintDebug("MAC\t%02x:%02x:%02x:%02x:%02x:%02x\n",
					pMacAddr[0], pMacAddr[1], pMacAddr[2],
					pMacAddr[3], pMacAddr[4], pMacAddr[5]);
		}

		rc = ioctl(fd,  SIOCGIFBRDADDR, (char *)ifr);
		if(rc < 0){
			close(fd);
			return DRV_ERR;
		}
		sin = (struct sockaddr_in *)&ifr->ifr_broadaddr;
		VK_memcpy(pNetworkInfo->brd_addr, (unsigned char *)&sin->sin_addr, ETH_ADDR_LENGTH);
		PrintDebug("BROD\t%s\n", inet_ntoa(sin->sin_addr));

		rc = ioctl(fd, SIOCGIFNETMASK, (char *)ifr);
		if(rc < 0){
			close(fd);
			return DRV_ERR;
		}

		sin = (struct sockaddr_in *)&ifr->ifr_addr;
		VK_memcpy(pNetworkInfo->net_addr, (unsigned char *)&sin->sin_addr, ETH_ADDR_LENGTH);
		PrintDebug("MASK\t%s\n", inet_ntoa(sin->sin_addr));

		rc = ioctl(fd, SIOCGIFMTU, (char *)ifr);
		if(rc < 0){
			close(fd);
			return DRV_ERR;
		}
		pNetworkInfo->mtu = ifr->ifr_mtu;
		PrintDebug("MTU\t%d\n", ifr->ifr_mtu);

		rc = ioctl(fd,  SIOCGIFADDR, (char *)ifr);
		if(rc < 0){
			close(fd);
			return DRV_ERR;
		}
		sin = (struct sockaddr_in *)&ifr->ifr_addr;
		VK_memcpy(pNetworkInfo->ip_addr, (unsigned char *)&sin->sin_addr, ETH_ADDR_LENGTH);
		P_ETH_Convert_Address(pNetworkInfo->ip_addr, buf);
		PrintDebug("IP\t%s\n", buf);
		PrintDebug("IP\t%s\n", inet_ntoa(sin->sin_addr));
		PrintDebug("IP\t%x\n", sin->sin_addr);

		ifr++;
	}

	close(fd);

	DRV_ETH_GetAddr_Gateway(pNetworkInfo->gate_addr);
	DRV_ETH_GetAddr_DNS(pNetworkInfo->dns_addr);

	return DRV_OK;
}

DRV_Error DRV_ETH_Network_SetDhcp(void)
{
	DRV_Error returnValue = DRV_ERR;

	PrintDebug("[ %s ] udhcpd\n",__FUNCTION__);

	returnValue = P_ETH_Call_System ("udhcpc");
	if(returnValue != DRV_OK)
	{
		return DRV_ERR;
	}
#if defined(CONFIG_CONFIG_FILE)
	returnValue = P_ETH_SaveDhcpSetting();

	if(returnValue != DRV_OK)
	{
		return DRV_ERR;
	}
#endif
	return DRV_OK;
}

DI_ERR_CODE DI_ETH_Network_GetInfo(int ethnum, DI_ETH_NETWORK_INFO* pNetworkInfo)
{
	UNUSED(ethnum);

	return (DI_ERR_CODE)DRV_ETH_Network_GetInfo((DI_ETH_NETWORK_INFO *)pNetworkInfo);
}

DI_ERR_CODE	DI_ETH_LinkState(int ethnum, int *pnLinkState)
{
	int		sck, sz;
	int		up, run;
#if 0
	int		linkState;
#endif
	struct	ifreq	ifr;
	UNUSED(ethnum);

	*pnLinkState = 0;

	VK_memset(&ifr, 0, sizeof(struct ifreq));
	sck = socket(AF_INET, SOCK_DGRAM, 0);
	if (sck == -1)
	{
		PrintError("Exception : open generic socket!!!\n");
		return DI_ERR_ERROR;
	}

	sz = VK_strlen("eth0");
	VK_memcpy(ifr.ifr_name, "eth0", sz);
	ifr.ifr_name[sz] = 0;
	if (ioctl(sck, SIOCGIFFLAGS, &ifr) == -1)
	{
		PrintError("Exception : Getting interface flags for (%s)\n", ifr.ifr_name);
		close(sck);
		return DI_ERR_ERROR;
	}

	up = (ifr.ifr_flags & IFF_UP) ? 1 : 0;
	run = (ifr.ifr_flags & IFF_RUNNING) ? 1 : 0;

#if 0
	ifr.ifr_data = (char *	)&linkState;
	if (ioctl(sck, SIOCGLINKSTATE, &ifr) == -1)
	{
		PrintError("Gettting LINK STATE Information for %s\n", ifr.ifr_name);
		return DI_ERR_ERROR;
	}

	PrintDebug("Link Status : [%d] - [%d] - [%d]\n", linkState, up, run);

	close(sck);

	if (linkState && up && run)
		*pnLinkState = 1;
#else
	PrintDebug("Link Status : [%d] - [%d]\n", up, run);

	close(sck);

	if (up && run)
		*pnLinkState = 1;
#endif

	return DI_ERR_OK;
}


/****************************************************************************
Function	: DI_ETH_arp
Parameters	:
Output		:
Return		:
	retValue
	0 -> OK,
	1 -> Not Send when some errors happen, source mac, null, lib err etc.
	2 -> Send, but not received.
	4 -> Cable Disconnected
Description	:
****************************************************************************/
DI_ERR_CODE DI_ETH_arp(int ethnum, char *strIP, int intIp, unsigned char* aucMAC, int waitSecond, int *retValue)
{
	int sock;
	struct ifreq ifr;
	struct sockaddr from, myaddr;
	unsigned int fromlen=0;
	unsigned char hostMAC[MAC_ADDR_LENGTH] = {0x00, };
	//unsigned char targetMAC[MAC_ADDR_LENGTH] = {0xFF, };
	arp_packet_t packet;
	DI_ETH_NETWORK_INFO pNetworkInfo;
	DI_ERR_CODE errorCode = DI_ERR_ERROR;
	DRV_Error returnValue = DRV_ERR;

	int		 cableConnected = 0;
	int		 packetReceived = 0;
	unsigned long startTick, curTick;

	unsigned char *buffer = NULL;
	UNUSED(ethnum);

	if (aucMAC == NULL || retValue == NULL || waitSecond < 0)
			 return DI_ERR_ERROR;

	/* Check cable Disconnect, return 4 */
	errorCode = DI_ETH_LinkState(0, &cableConnected);
	if (errorCode != DI_ERR_OK || cableConnected != 1)
	{
		DI_UART_Print("Cable Disconnected \n");
		*retValue = 4;
		return DI_ERR_ERROR;
	}

	*retValue = 1;
	if (strIP == NULL && intIp == 0)
	{
	 return DI_ERR_ERROR;
	}

	returnValue = DRV_ETH_Network_GetInfo(&pNetworkInfo);
	if (returnValue != DRV_OK)
	{
		 DI_UART_Print("Mac Address\n");
		 return DI_ERR_ERROR;
	}
	VK_memcpy(hostMAC, pNetworkInfo.mac_addr, MAC_ADDR_LENGTH);

	sock = socket(AF_INET, SOCK_PACKET, htons(0x0003));
	if(sock < 0)
	{
		 DI_UART_Print("socket");
		 return DI_ERR_ERROR;
	}

	VK_memset((char *)&ifr, 0, sizeof(struct ifreq));
	VK_strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);
	if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0)
	{
		 DI_UART_Print("SIOCGIFHWADDR");
		 close(sock);
		 return DI_ERR_ERROR;
	}
	VK_memcpy(packet.arphdr.sha, ifr.ifr_hwaddr.sa_data, ETH_LEN);

	VK_memset(packet.ethhdr.dhost, 0xFF, ETH_LEN);
	VK_memcpy(packet.ethhdr.shost, ifr.ifr_hwaddr.sa_data, ETH_LEN);

	packet.ethhdr.type = htons(ARP);
	packet.arphdr.hdr = htons(ETHERNET);
	packet.arphdr.proto = htons(0x0800);
	packet.arphdr.hlen = ETH_LEN;
	packet.arphdr.plen = 4;
	packet.arphdr.op = htons(ARP_REQUEST);

	VK_memset((char *)&ifr, 0, sizeof(struct ifreq));
	VK_strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);
	if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)
	{
		 DI_UART_Print("SIOCGIFADDR");
		 close(sock);
		 return DI_ERR_ERROR;
	}
	packet.arphdr.sip = ((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr.s_addr;
	VK_memset(packet.arphdr.dha, 0, ETH_LEN);
	packet.arphdr.dip = intIp;
	if (strIP != NULL)
			 packet.arphdr.dip = inet_addr(strIP);

	VK_memset(&myaddr, 0, sizeof(myaddr));
	myaddr.sa_family = AF_INET;
	VK_memcpy(myaddr.sa_data, "eth0", sizeof("eth0"));

	VK_memset(&from, 0, sizeof(from));
	from.sa_family = AF_INET;
	VK_memcpy(from.sa_data, "eth0", sizeof("eth0"));

	if(bind(sock, &myaddr, sizeof(myaddr)) == -1)
	{
		 DI_UART_Print("bind");
		 close(sock);
		 return DI_ERR_ERROR;
	}

	startTick = VK_TIMER_GetSystemTick();

	if(-1 == sendto(sock, &packet, sizeof(packet), 0, &from, sizeof(from)))
	{
		 DI_UART_Print("sendto");
		 close(sock);
		 return DI_ERR_ERROR;
	}

	if ((fcntl (sock, F_SETFL, O_NONBLOCK)) == -1)
	{
		close(sock);
		return DI_ERR_ERROR;
	}

	buffer = (unsigned char *) DD_MEM_Alloc(1500);
	if(buffer == NULL)
	{
		PrintError("%s(%d) : Error. can't alloc\n", __FUNCTION__, __LINE__);
		close(sock);
		return DI_ERR_ERROR;
	}

	while (1)
	{
		arp_packet_t *parp = NULL;

		int i;

		VK_memset(buffer, 0, 1500);

		parp =(arp_packet_t *) buffer;

		if(recvfrom(sock, buffer, 1500, 0, &from, &fromlen) != -1)
		{
			if((parp->ethhdr).type == htons(ARP) && VK_memcmp((*parp).ethhdr.dhost, hostMAC, MAC_ADDR_LENGTH) == 0)
			{
				if (packet.arphdr.dip != (*parp).arphdr.sip)  /* 딴곳에서 들어온 ARP Packet은 무시하도록 함. */
				{
					continue;
				}

				for(i = 0; i<ETH_LEN; i++)
				{
					aucMAC[i]= (*parp).arphdr.sha[i];
				}
				packetReceived = 1;
				break;
			}
		}


	  /* Packet 이 하나도 없으면 먹통 ?? */
		 curTick = VK_TIMER_GetSystemTick();
		 if ((curTick < startTick) || (curTick - startTick > (unsigned long) (waitSecond * 1000)))
					break;
		 sleep(1);

	}

	DD_MEM_Free(buffer);

	if (packetReceived == 0)
	{
		*retValue = 2;
		close(sock);
		return DI_ERR_ERROR;
	}

	*retValue = 0;
	close(sock);
	return DI_ERR_OK;
}

#endif

DRV_Error DRV_ETH_Network_Up(HINT32 ethnum, DI_ETH_NETWORK_INFO* pNetworkInfo)
{
	DRV_Error returnValue = DRV_OK;

#if defined(CONFIG_OS_UCOS)
	HINT32	vResult = 0;
	HUINT8	szEnv[SIZE_NET_PATH];

	if (pNetworkInfo == NULL)
	{
		PrintError("pNetworkInfo is NULL\n");
		return DRV_ERR;
	}

	if (s_bEthUp == TRUE)
	{
		PrintError("Ethernet device is already up\n");
		return DRV_OK;
	}

	VK_MEM_Memset(szEnv,0,SIZE_NET_PATH);
	/* Get IP using DHCP */
	if (pNetworkInfo->isDhcp == TRUE)
	{
		VK_snprintf(szEnv, SIZE_NET_PATH, "ifconfig eth0 -auto");
	}
	/* Set IP from NVRAM */
	else
	{
		VK_snprintf(szEnv, SIZE_NET_PATH, "ifconfig eth0 -addr=%d.%d.%d.%d -mask=%d.%d.%d.%d -gw=%d.%d.%d.%d -dns=%d.%d.%d.%d"
			,pNetworkInfo->ip_addr[0]		,pNetworkInfo->ip_addr[1]		,pNetworkInfo->ip_addr[2]		,pNetworkInfo->ip_addr[3]
			,pNetworkInfo->net_addr[0]		,pNetworkInfo->net_addr[1]		,pNetworkInfo->net_addr[2]		,pNetworkInfo->net_addr[3]
			,pNetworkInfo->gate_addr[0] 	,pNetworkInfo->gate_addr[1] 	,pNetworkInfo->gate_addr[2] 	,pNetworkInfo->gate_addr[3]
			,pNetworkInfo->dns_addr[0]		,pNetworkInfo->dns_addr[1]		,pNetworkInfo->dns_addr[2]		,pNetworkInfo->dns_addr[3]
			);
	}

	/* Get Ready of Ethernet Device */
	vResult = VK_SYSTEM_Command(szEnv);
	if (vResult < 0)
	{
		PrintError("VK_SYSTEM_Command Error, rc = 0x%X\n", vResult);
		return vResult;
	}

	/* Turn the flag on */
	s_bEthUp = TRUE;
	PrintDebug("[DRV_ETH_Network_Up] %s\n", szEnv);
#else
	if( pNetworkInfo->isDhcp == TRUE )
	{
		returnValue = DRV_ETH_Network_SetDhcp();
	}
	else
	{
		returnValue = DRV_ETH_Network_SetInfo((DI_ETH_NETWORK_INFO *)pNetworkInfo);
	}
#endif
	UNUSED(ethnum);

	return returnValue;
}

DRV_Error DRV_ETH_Network_Down(HINT32 ethnum)
{
	DRV_Error returnValue = DRV_OK;

#if defined(CONFIG_OS_UCOS)
	HINT32	vResult = 0;
	HUINT8	szEnv[SIZE_NET_PATH];

	if (s_bEthUp == FALSE)
	{
		return DRV_ERR;
	}

	VK_MEM_Memset(szEnv,0,SIZE_NET_PATH);
	VK_snprintf(szEnv, SIZE_NET_PATH, "ifconfig eth0 -off");

	vResult = VK_SYSTEM_Command(szEnv);
	if (vResult < 0)
	{
		PrintError("VK_SYSTEM_Command Error, rc = 0x%X\n", vResult);
		return vResult;
	}

	s_bEthUp = FALSE;
	PrintDebug("[DRV_ETH_Network_Down] %s\n", szEnv);
#else

	PrintDebug("[ %s ] %s\n\r",__FUNCTION__,"ifdown eth0");

	returnValue = P_ETH_Call_System ("ifdown eth0");

	if(returnValue != DRV_OK)
	{
		return DRV_ERR;
	}
#endif
	UNUSED(ethnum);

	return returnValue;
}

DI_ERR_CODE DI_ETH_Network_Up(int ethnum, DI_ETH_NETWORK_INFO* pNetworkInfo)
{
	return (DI_ERR_CODE)DRV_ETH_Network_Up(ethnum, (DI_ETH_NETWORK_INFO *)pNetworkInfo);
}

DI_ERR_CODE DI_ETH_Network_Down(int ethnum)
{
	return (DI_ERR_CODE)DRV_ETH_Network_Down(ethnum);

}

#endif

#if defined(CONFIG_ETH_EXT_PHY)
/*******************************************************************************
* function : DI_ETH_SetWakeOnLAN
* description : External PHY support (WOL)
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_ETH_SetWakeOnLAN (HBOOL bOn_Off)
{
	DRV_Error	nDrvErr = DRV_OK;

	nDrvErr = DRV_MICOM_SetWakeOnLAN(bOn_Off);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DI_ETH_SetWakeOnLAN() \n", nDrvErr);
		return	DI_ERR_ERROR;
	}

	PrintDebug("[%s] DI_ETH_SetWakeOnLAN is %s \r\n", __func__, bOn_Off?"ON":"OFF");

	return DI_ERR_OK;
}
#endif

