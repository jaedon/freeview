/**************************************************************
 *	@file		apk_network_concheck.c
 *	appkit for network
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2013-07-15
 *	@author			humax
 *	@brief			appkit for network
 **************************************************************/

#include 	<netdb.h>
#include	<resolv.h>
#include 	<arpa/inet.h>
#include 	<netinet/in.h>
#include 	<fcntl.h>

//#include	<if_ether.h>

#include	<vkernel.h>
#include	<hlib.h>
#include 	<apk.h>
#include 	<oapi.h>
#include 	<napi.h>

#if defined(CONFIG_SUPPORT_NIMA)

#define FuncEnter HxLOG_Debug("(+)[%s:%d] Enter!!\n",__FUNCTION__,__LINE__);
#define FuncLeave HxLOG_Debug("(-)[%s:%d] Leave -- \n",__FUNCTION__,__LINE__);

#define HARDWARE_SIZE 6
#define PROTO_SIZE 4
#define ARP_REQUEST 0x0001 //if_arp.h
#define ARP_REPLAY 0x0002
#define ARP 0x0806 // ETH_P_ARP
#define PROTO_TYPE  0x0800
#define ETHERNET 0x0001

typedef struct{
	HINT32 nDevIdx;
	HINT32 nReqId;
	HINT32 nCmd;
	HINT32 nTimeout;
	HCHAR szAddr[256];
	APKS_NETWORK_CbHandler pfnNotifier;
} NETCHK_MSG_t;

#pragma pack(1)
typedef struct
{
	unsigned char dhost[HARDWARE_SIZE]; // Destination MAC : 6Byte
	unsigned char shost[HARDWARE_SIZE]; // Source MAC : 6Byte
	unsigned short type;	// Ether Type : 2Byte
} ETHERHDR_t;

typedef struct
{
	unsigned short hdr;
	unsigned short proto;
	unsigned char hlen;
	unsigned char plen;
	unsigned short op;
	unsigned char sha[HARDWARE_SIZE];
	unsigned int sip;
	unsigned char dha[HARDWARE_SIZE];
	unsigned int dip;
} ARPHDR_t;

typedef struct arp_sendPacket
{
	ETHERHDR_t	ethhdr;
	ARPHDR_t	arphdr;
} ARP_PACKET_t;
#pragma pack()

void __apk_parser_macNtoA(const HUINT8 *pMacAddress, HUINT8 *pszMacAddrStr)
{
	HINT32		i = 0;
	HUINT8		mac_temp[8] = {0, };

	for(i = 0; i < 6; i++)
	{
		HxSTD_MemSet(mac_temp, 0, 8);
		HxSTD_snprintf(mac_temp, sizeof(mac_temp), "%02x", pMacAddress[i]);
		HxSTD_StrNCat(pszMacAddrStr, mac_temp, 2);
		if(i < 5)
			HxSTD_StrNCat(pszMacAddrStr, ":", 1);
	}
}

void __apk_parser_macAtoN(const HUINT8 *pszMacAddrStr, HUINT8 *pMacAddress)
{
	HUINT32	octet[6] = { 0 };

	sscanf ( pszMacAddrStr, "%02x:%02x:%02x:%02x:%02x:%02x", &octet[0], &octet[1], &octet[2], &octet[3], &octet[4], &octet[5] );

	pMacAddress[0] = octet[0];
	pMacAddress[1] = octet[1];
	pMacAddress[2] = octet[2];
	pMacAddress[3] = octet[3];
	pMacAddress[4] = octet[4];
	pMacAddress[5] = octet[5];
}

static void	__apk_network_resolve_task(void *arg)
{
	APKE_NET_STATUS_e eEvent = eNETWORK_CONCHK_RESOLV_FAIL;
	struct hostent *host_entry = NULL;
	NETCHK_MSG_t *pstMsg = (NETCHK_MSG_t *)arg;
	HCHAR szIpAddr[40] = { 0 }; // IPv4 - 16byte, IPv6 = 40byte
	HCHAR *pUrl = NULL;

	FuncEnter

	if (pstMsg == NULL)
	{
		HxLOG_Error("pstMsg is NULL");
		return;
	}

	res_init();

	if ( (pUrl = HxSTD_StrStr( pstMsg->szAddr, "http://")) )
		pUrl = pUrl + HxSTD_StrLen("http://");
	else if ( (pUrl = HxSTD_StrStr( pstMsg->szAddr, "https://")) )
		pUrl = pUrl + HxSTD_StrLen("https://");
	else
		pUrl = pstMsg->szAddr;

	HxLOG_Print("[APK_NET_CONCHK] Resolv domain : %s\n", pUrl);

	host_entry = gethostbyname( pUrl );
	if ( NULL != host_entry )
	{
		int i = 0;
		for ( i=0; NULL != host_entry->h_addr_list[i]; ++i)
		{
			HxSTD_StrNCpy( szIpAddr, inet_ntoa(*(struct in_addr*)host_entry->h_addr_list[i]), sizeof(szIpAddr)-1 );
			HxLOG_Print("[APK_NET_CONCHK] index[%d] ip : %s\n", i, szIpAddr );
		}

		if( 0 < i )
		{
			HxLOG_Print("[APK_NET_CONCHK] DNS ACCESS. SUCCESS!!!!  ip : %s\n", szIpAddr);
			eEvent = eNETWORK_CONCHK_RESOLV_SUCCESS;
		}
		else
		{
			HxLOG_Print("[APK_NET_CONCHK] DNS ACCESS. FAIL\n");
		}
	}

	if (pstMsg->pfnNotifier)
	{
		pstMsg->pfnNotifier(eEvent, pstMsg->nDevIdx, (HINT32)szIpAddr, pstMsg->nReqId );
	}

	if (pstMsg != NULL)
	{
		HLIB_STD_MemFree(pstMsg);
	}

	VK_TASK_Sleep(100);
	FuncLeave
}

static void	__apk_network_arp_task(void *arg)
{
	APKE_NET_STATUS_e eEvent = eNETWORK_CONCHK_ARP_FAIL;
	NETCHK_MSG_t *pstMsg = (NETCHK_MSG_t *)arg;
	int sock;
	HINT32 nIndex = 0;
	struct sockaddr stSockAddr;
	unsigned int stSockAddrlen=0;
	ARP_PACKET_t sendPacket;
	NAPINetConfInfo_t network_info;

	HINT32 startTick, curTick;
	HINT32 nTimeout = 0;
	HCHAR *pszTargetIp = NULL;
	HULONG ulNetdevIp = 0;
	HULONG ulTargetIp = 0;
	unsigned char aucTargetMac[HARDWARE_SIZE] = { 0 };
	unsigned char aucDevMac[HARDWARE_SIZE] = { 0 };
	HCHAR szTargetMac[24] = { 0 };

	ARP_PACKET_t *pReceivePacket = NULL;
	HUINT8 buffer[1024] = { 0 };

	FuncEnter

	if (pstMsg == NULL)
	{
		HxLOG_Error("pstMsg is NULL");
		return;
	}

	nIndex = pstMsg->nDevIdx;
	pszTargetIp = pstMsg->szAddr;
	nTimeout = pstMsg->nTimeout;
	NAPI_NetConf_GetInfo(nIndex, &network_info);

	if (pszTargetIp == NULL)
	{
		HxLOG_Error("Target IP is NULL");
		return ;
	}
	HxLOG_Print ("[APK_NETCHK] ARP target IP : %s\n", pszTargetIp );
	ulTargetIp = inet_addr(pszTargetIp);

	__apk_parser_macAtoN( network_info.ip_info.ipv4.macaddr, aucDevMac );
	APK_NETWORK_CONF_AddrA2N(network_info.ip_info.ipv4.ipAddr, &ulNetdevIp);

	sock = socket(AF_INET, SOCK_PACKET, htons(0x0003));
	if(sock < 0)
	{
		HxLOG_Error("socket() Error :  %d", sock);
		return ;
	}

	/* Ethernet Frame */
	sendPacket.ethhdr.type = htons(ARP);
	HxSTD_MemSet(sendPacket.ethhdr.dhost, 0xFF, HARDWARE_SIZE);			// FF:FF:FF:FF:FF:FF For Mac Broadcasting
	HxSTD_MemCopy(sendPacket.ethhdr.shost, aucDevMac, HARDWARE_SIZE);	// Sender MAC Address

	/* Set ARP Requset */
	sendPacket.arphdr.hdr	= htons(ETHERNET); 		// Ethernet : 0x0001
	sendPacket.arphdr.proto = htons(PROTO_TYPE); 	// IP : 0x0800
	sendPacket.arphdr.hlen	= HARDWARE_SIZE; 		// hardware size : 6
	sendPacket.arphdr.plen	= PROTO_SIZE;			// protocol size : 4
	sendPacket.arphdr.op	= htons(ARP_REQUEST); 	// Opcode : 0x0001

	HxSTD_MemCopy(sendPacket.arphdr.sha, aucDevMac, HARDWARE_SIZE);	// Sender MAC Address
	sendPacket.arphdr.sip = ulNetdevIp;								// Sender IP Address
	HxSTD_MemSet(sendPacket.arphdr.dha, 0x00, HARDWARE_SIZE);		// Target MAC Address
	sendPacket.arphdr.dip = ulTargetIp;								// Target IP Address

	HxSTD_MemSet(&stSockAddr, 0, sizeof(stSockAddr));
	stSockAddr.sa_family = AF_INET;
	HxSTD_MemCopy(stSockAddr.sa_data, network_info.name, sizeof(stSockAddr.sa_data)-1);

	if(bind(sock, &stSockAddr, sizeof(stSockAddr)) == -1)
	{
		HxLOG_Error("bind() Error\n");
		close(sock);
		return ;
	}

	startTick = HLIB_STD_GetSystemTick();
	if(-1 == sendto(sock, &sendPacket, sizeof(sendPacket), 0, &stSockAddr, sizeof(stSockAddr)))
	{
		HxLOG_Error("sendto() Error\n");
		close(sock);
		return ;
	}

	if ((fcntl (sock, F_SETFL, O_NONBLOCK)) == -1)
	{
		HxLOG_Error("fcntl() Error\n");
		close(sock);
		return ;
	}

	pReceivePacket =(ARP_PACKET_t *)buffer;
	while (1)
	{
		HxSTD_MemSet(buffer, 0, sizeof(buffer));
		if(recvfrom(sock, buffer, sizeof(buffer), 0, &stSockAddr, &stSockAddrlen) != -1)
		{
#ifdef CONFIG_DEBUG
			HCHAR targetMac[32] = { 0 };
			HCHAR senderMac[32] = { 0 };
			__apk_parser_macNtoA( (*pReceivePacket).ethhdr.dhost, targetMac );
			__apk_parser_macNtoA( (*pReceivePacket).ethhdr.shost, senderMac );
			HxLOG_Print ("[APK_NETCHK] recvfrom : ethhdr(0x%0x), dhost(%s), shost(%s)\n",
									htons(pReceivePacket->ethhdr.type), targetMac, senderMac);
#endif
			if((pReceivePacket->ethhdr).type == htons(ARP)
					&& 0 == memcmp((*pReceivePacket).ethhdr.dhost, aucDevMac, HARDWARE_SIZE) )
			{
				if (ulTargetIp != (*pReceivePacket).arphdr.sip)  /* 다른 Dev에서 들어온 ARP Packet은 무시 */
				{
					continue;
				}

				HxSTD_MemCopy( aucTargetMac, pReceivePacket->arphdr.sha, sizeof(aucTargetMac) );
				eEvent = eNETWORK_CONCHK_ARP_SUCCESS;
				break;
			}
		}

		curTick = HLIB_STD_GetSystemTick();
		if ((curTick < startTick) || (curTick - startTick > (unsigned long) (nTimeout)))
			break;

		VK_TASK_Sleep(10);
	}

	close(sock);

	__apk_parser_macNtoA( aucTargetMac, szTargetMac );
	if (pstMsg->pfnNotifier)
	{
		HxLOG_Print ("[APK_NETCHK] Targret IP(%s) - Address Resolution : MAC %s\n", pszTargetIp, szTargetMac );
		pstMsg->pfnNotifier(eEvent, pstMsg->nDevIdx, (HINT32)szTargetMac, pstMsg->nReqId );
	}

	if (pstMsg != NULL)
	{
		HLIB_STD_MemFree(pstMsg);
	}

	VK_TASK_Sleep(100);
	FuncLeave;
}


HINT32		APK_NETWORK_CONCHK_Gateway( HINT32 nIndex, APKS_NETWORK_CbHandler pfnCallback, HINT32 nTimeout )
{
	NAPINetConfInfo_t network_info;
	FuncEnter
	NAPI_NetConf_GetInfo(nIndex, &network_info);

	FuncLeave
	return APK_NETWORK_CONCHK_Arp( nIndex, network_info.ip_info.ipv4.gateway, pfnCallback, nTimeout );
}

HINT32		APK_NETWORK_CONCHK_Dns( HINT32 nIndex, APKS_NETWORK_CbHandler pfnCallback, HINT32 nTimeout )
{
#ifdef CONFIG_APPPORTAL_SERVER_URL
	HCHAR *pPortalUrl = CONFIG_APPPORTAL_SERVER_URL;
#else
	HCHAR *pPortalUrl = NULL;
#endif
	return APK_NETWORK_CONCHK_ResolvDomain( nIndex, pPortalUrl, pfnCallback, nTimeout );
}

static 	HINT32 s_reqId = 0;
HINT32		APK_NETWORK_CONCHK_Arp( HINT32 nIndex, HCHAR *pszIpAddr, APKS_NETWORK_CbHandler pfnCallback, HINT32 nTimeout )
{
	HUINT32	ulTaskId = 0;
	HINT32 nReqId = 0;
	NETCHK_MSG_t *pstMsg = NULL;
	FuncEnter
	if ( NULL == pszIpAddr )
		return -1;

	pstMsg = (NETCHK_MSG_t *)HLIB_STD_MemAlloc(sizeof(NETCHK_MSG_t));
	HxSTD_MemSet( pstMsg, 0x00, sizeof(NETCHK_MSG_t));

	nReqId = ++s_reqId;
	pstMsg->nDevIdx = nIndex;
	pstMsg->nReqId = nReqId;
	pstMsg->pfnNotifier = pfnCallback;
	pstMsg->nTimeout = nTimeout;
	HxSTD_StrNCpy( pstMsg->szAddr, pszIpAddr, sizeof(pstMsg->szAddr)-1);

	VK_TASK_Create( __apk_network_arp_task, 40, SIZE_16K, "arpaCheckTask", (void*)pstMsg, (unsigned long*)&ulTaskId, 0);
	VK_TASK_Start(ulTaskId);
	FuncLeave

	return nReqId;
}

HINT32		APK_NETWORK_CONCHK_ResolvDomain( HINT32 nIndex, HCHAR *pszDomain, APKS_NETWORK_CbHandler pfnCallback, HINT32 nTimeout )
{
	HUINT32	ulTaskId = 0;
	HINT32 nReqId = 0;
	NETCHK_MSG_t *pstMsg = NULL;
	FuncEnter

	if ( NULL == pszDomain )
		return -1;


	pstMsg = (NETCHK_MSG_t *)HLIB_STD_MemAlloc(sizeof(NETCHK_MSG_t));
	HxSTD_MemSet( pstMsg, 0x00, sizeof(NETCHK_MSG_t));

	nReqId = ++s_reqId;
	pstMsg->nDevIdx = nIndex;
	pstMsg->nReqId = nReqId;
	pstMsg->pfnNotifier = pfnCallback;
	pstMsg->nTimeout = nTimeout;
	HxSTD_StrNCpy( pstMsg->szAddr, pszDomain, sizeof(pstMsg->szAddr)-1);

	VK_TASK_Create( __apk_network_resolve_task, 40, SIZE_16K, "domainResolvTask", (void*)pstMsg, (unsigned long*)&ulTaskId, 0);
	VK_TASK_Start(ulTaskId);
	FuncLeave

	return nReqId;
}

#else



HINT32		APK_NETWORK_CONCHK_Gateway( HINT32 nIndex, APKS_NETWORK_CbHandler pfnCallback, HINT32 nTimeout )
{
	return 0;
}

HINT32		APK_NETWORK_CONCHK_Dns( HINT32 nIndex, APKS_NETWORK_CbHandler pfnCallback, HINT32 nTimeout )
{
	return 0;
}

HINT32		APK_NETWORK_CONCHK_Arp( HINT32 nIndex, HCHAR *pszIpAddr, APKS_NETWORK_CbHandler pfnCallback, HINT32 nTimeout )
{
	return 0;
}

HINT32		APK_NETWORK_CONCHK_ResolvDomain( HINT32 nIndex, HCHAR *pszDomain, APKS_NETWORK_CbHandler pfnCallback, HINT32 nTimeout )
{
	return 0;
}


#endif
