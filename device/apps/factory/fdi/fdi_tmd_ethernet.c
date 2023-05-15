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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#if defined(CONFIG_PLATFORM_ALI)
#include "hui_socket.h"
#include <string.h>
#endif
#if !defined(CONFIG_OS_RTOS)
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/sockios.h>
#include <poll.h>
#include <memory.h>
#endif

/* chip manufacture's  header files */

/* humax header files */
#include "fvk.h"
#include "fdi_common.h"
#include "fdi_tmd.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#if (TMD_ETHERNET_TEST_LOCAL_PC==YES)
//PC를 TMD Server로 테스트
#define TMD_SERVER_IP		"192.168.0.11"

#else
//Real TMD Server
#define TMD_SERVER_IP 		"10.0.10.92"
#endif
#define TMD_SERVER_PORT 	8281

char sTmdServerIP[16] = TMD_SERVER_IP;

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */
extern BOOL g_bEthLinked;

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
int g_nTmdClientHandle;

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static void INTRN_FDI_TMD_DelArpTable()
/* delete a arp entry with server ip*/
{
#if !defined(CONFIG_PLATFORM_ALI)
	int	result;
	struct arpreq arpreq;
	struct sockaddr_in *dest;
	int fd;

	result =0;
	fd = socket(PF_PACKET, SOCK_DGRAM, 0);
	if ( fd < 0 )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] : ARP Socket error!!\n", __FUNCTION__));
		return ;
	}
	memset(&arpreq, 0, sizeof(struct arpreq));
	sprintf(arpreq.arp_dev, "eth0");
	dest = (struct sockaddr_in *)&arpreq.arp_pa;
	dest->sin_family = AF_INET;
	inet_pton(AF_INET, sTmdServerIP, &dest->sin_addr);
	result = ioctl(fd, SIOCDARP, &arpreq);
	if ( result < 0 )
	{
		//hee FDI_PRINT(FDI_PRT_ERROR, ("[%s] : ARP SIOCDARP with error (%d) %d!!\n", __FUNCTION__, result));
	}
	close(fd);
#else
	HUI_ARP_Delete();
#endif

	return;
}

FDI_ERR_CODE FDI_TMD_Init()
{
	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_TMD_Open()
{
#if !defined(CONFIG_PLATFORM_ALI)
	int nMode;
	struct sockaddr_in tClientAddr;
	socklen_t		nClientLen;
	int 			n, error = 0;
	socklen_t		len;
	fd_set			rset, wset;
	struct timeval	tval;

	FDI_PRINT(FDI_PRT_DBGINFO, ("[%s] : Connect Server:%s, Port:%d\n", __FUNCTION__, TMD_SERVER_IP, TMD_SERVER_PORT));

	g_nTmdClientHandle = socket(AF_INET, SOCK_STREAM, 0);
	tClientAddr.sin_family = AF_INET;
	tClientAddr.sin_addr.s_addr = inet_addr(TMD_SERVER_IP);
	tClientAddr.sin_port = htons(TMD_SERVER_PORT);

	// initialize padding aray.
	memset(&tClientAddr.sin_zero, 0, sizeof(tClientAddr.sin_zero));

	nClientLen = sizeof(tClientAddr);

	nMode = fcntl(g_nTmdClientHandle, F_GETFL, 0);
	if (-1 == nMode)
	{
		error = errno;
		if (error)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[%s] : File Control error %d!!\n", __FUNCTION__, error));
		}
		goto done;
	}
	if (fcntl(g_nTmdClientHandle, F_SETFL, nMode | O_NONBLOCK) == -1)
	{
		error = errno;
		if (error)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[%s] : File Control error %d!!\n", __FUNCTION__, error));
		}
		goto done;
	}

	if ( (n = connect(g_nTmdClientHandle, (struct sockaddr *) &tClientAddr, nClientLen)) < 0)
	{
		if (errno != EINPROGRESS)
		{
			error = errno;
			if (error)
			{
				FDI_PRINT(FDI_PRT_ERROR, ("[%s] : connect error %d!!\n", __FUNCTION__, error));
			}
			goto done;
		}
	}

	/* Do whatever we want while the connect is taking place. */
	if (n == 0)
	{
		FDI_PRINT(FDI_PRT_DBGINFO, ("[%s] : connect completed immediately. \n", __FUNCTION__));
		goto done;	/* connect completed immediately */
	}

	FD_ZERO(&rset);
	FD_SET(g_nTmdClientHandle, &rset);
	wset = rset;
	tval.tv_sec = 2; //shlee5
	tval.tv_usec = 0;

	if ( (n = select(g_nTmdClientHandle+1, &rset, &wset, NULL, &tval)) == 0)
	{
		/* timeout */
		error = ETIMEDOUT;
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] : select error %d!!\n", __FUNCTION__, error));
		goto done;
	}

	if (FD_ISSET(g_nTmdClientHandle, &rset) || FD_ISSET(g_nTmdClientHandle, &wset))
	{
		len = sizeof(error);
		if (getsockopt(g_nTmdClientHandle, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
		{
			/* Solaris pending error */
			if (error)
			{
				FDI_PRINT(FDI_PRT_ERROR, ("[%s] : getsockopt error %d!!\n", __FUNCTION__, error));
			}
			goto done;
		}
	}
	else
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] : g_nTmdClientHandle not set !\n", __FUNCTION__));
		error = errno;
		if (error)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[%s] : FD_ISSET error %d!!\n", __FUNCTION__, error));
		}
		goto done;
	}

done:
	if (error)
	{
		INTRN_FDI_TMD_DelArpTable();
		close(g_nTmdClientHandle);
		return FDI_ERR;
	}
	return FDI_NO_ERR;
#else
enum {
	SOCK_ERR_NONE,
	SOCK_ERR_OPEN,
	SOCK_ERR_CONNECT,
	SOCK_ERR_GETOPT,
	SOCK_ERR_FCNTL,
	SOCK_ERR_SELECT
};
	int nMode;
	int n, error = -1, sock_err = SOCK_ERR_NONE;
	HUI_SOCKETADDR_IN_T tClientAddr;
	HUI_SOCKLEN_T nClientLen = sizeof(HUI_SOCKETADDR_IN_T);
	HUI_SOCKLEN_T len;
	HUI_FD_SET_T rset;
	HUI_TIMEVAL_T tval;
	HUI_IP_LOC_CFG_T cfg;
	FDI_ERR_CODE ret;

	//FDI_PRINT(FDI_PRT_DBGINFO, ("[%s,%d] Connecting......", __FUNCTION__, __LINE__));

	g_nTmdClientHandle = HUI_SOCKET_Socket(HUI_AF_INET, HUI_SOCK_STREAM, 0);
	if (g_nTmdClientHandle < 0)
	{
		sock_err = SOCK_ERR_OPEN;
		goto done;
	}

	VK_MEM_Memset(&tClientAddr, 0, sizeof(HUI_SOCKETADDR_IN_T));
	tClientAddr.sin_family = HUI_AF_INET;
	tClientAddr.sin_addr.s_addr = HUI_INET_Addr(TMD_SERVER_IP);
	tClientAddr.sin_port = HUI_INET_Htons(TMD_SERVER_PORT);
	nClientLen = sizeof(HUI_SOCKETADDR_IN_T);

	n = HUI_SOCKET_Fcntl(g_nTmdClientHandle, HUI_F_SETFL, 1);
	if (n < 0)
	{
		sock_err = SOCK_ERR_FCNTL;
		goto done;
	}

	if (HUI_INET_GetLinkState(HUI_SOCK_DEVTYPE_USB2ETH) == FALSE)
		tClientAddr.sin_family = HUI_AF_UNSPEC;

	n = HUI_SOCKET_Connect(g_nTmdClientHandle, (HUI_SOCKETADDR_IN_T*)&tClientAddr, nClientLen);
	if (n < 0)
	{
		sock_err = SOCK_ERR_CONNECT;
		goto done;
	}

	len = sizeof(error);
	n = HUI_SOCKET_Getsockopt(g_nTmdClientHandle, HUI_SOL_SOCKET, HUI_SO_ERROR, &error, &len);
	if (n < 0 || error)
	{
		sock_err = SOCK_ERR_GETOPT;
		goto done;
	}

done:
	if (sock_err)
	{
		FDI_PRINT(FDI_PRT_DBGINFO, ("[%s,%d] Connecting...... Fail, err=%d\n", __FUNCTION__, __LINE__, sock_err));
		INTRN_FDI_TMD_DelArpTable();
		HUI_SOCKET_Close(g_nTmdClientHandle);
		ret = FDI_ERR;
	}
	else
	{
		FDI_PRINT(FDI_PRT_DBGINFO, ("[%s,%d] Connecting...... Success\n", __FUNCTION__, __LINE__));
		ret = FDI_NO_ERR;
	}
#if 0
	HUI_INET_Getipcfg(HUI_SOCK_DEVTYPE_USB2ETH, &cfg);
	FDI_PRINT(FDI_PRT_DBGINFO, ("[%s,%d] ClientIP(%u.%u.%u.%u), ServerIP(%s), Port(%d)\n",
		                        __FUNCTION__, __LINE__,
		                        (cfg.ip_addr)&0xFF,
		                        (cfg.ip_addr>>8)&0xFF,
		                        (cfg.ip_addr>>16)&0xFF,
		                        (cfg.ip_addr>>24)&0xFF,
		                        TMD_SERVER_IP, TMD_SERVER_PORT));
#endif
	return ret;
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
FDI_ERR_CODE FDI_TMD_Close()
{
#if !defined(CONFIG_PLATFORM_ALI)
	close(g_nTmdClientHandle);
	INTRN_FDI_TMD_DelArpTable();

	FDI_PRINT(FDI_PRT_ERROR, ("[FDI_TMD_Close] : Disconnected TMD Server!!\n"));
#else
	HUI_SOCKET_Close(g_nTmdClientHandle);
#endif

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_TMD_SendData(unsigned char *pucData, unsigned long ulSize)
{
#if !defined(CONFIG_PLATFORM_ALI)
	/* ethenet port가 뽑힌 경우 socket close */
	if( !g_bEthLinked )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_TMD_SendData] %d \n", g_bEthLinked));
		return FDI_ERR;
	}

	if( write(g_nTmdClientHandle, pucData, ulSize) < 0 )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_TMD_ReceiveData] : write error \n"));
		return FDI_ERR;
	}
#else
	int written = 0;
	int rc;

	if (HUI_INET_GetLinkState(HUI_SOCK_DEVTYPE_USB2ETH) == FALSE)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_TMD_SendData] Link is disconnected\n"));
		return FDI_ERR;
	}

	written = HUI_SOCKET_Send(g_nTmdClientHandle, pucData, ulSize, 0);
	FDI_PRINT(FDI_PRT_ERROR, ("[%s,%d] HUI_SOCKET_Send written(%d)\n", __FUNCTION__, __LINE__, written));
#endif

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_TMD_ReceiveData(unsigned char *pucData, unsigned long ulSize, unsigned long ulTimeoutMs)
{
#if !defined(CONFIG_PLATFORM_ALI)
	unsigned long ulReadSize, ulTime;
	int nResult;
	struct pollfd fds;

	ulReadSize = ulTime= 0;
	while( ulReadSize != ulSize )
	{

		if( !g_bEthLinked )
		{
			return FDI_ERR;
		}

		fds.fd = g_nTmdClientHandle;
		fds.events = POLLIN;
		fds.revents = 0;

		if ( (nResult = poll(&fds, 1, 1000)) < 0 )
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_TMD_ReceiveData] poll err %d \n", nResult));
			return FDI_ERR;
		}
		else if ( nResult == 0 )
		{
			/*FDI_PRINT(FDI_PRT_ERROR, ("[FDI_TMD_ReceiveData] poll Timeout \n"));*/
			ulTime += 1000;

			if( ulTime >= ulTimeoutMs )
			{
				return FDI_ERR;
			}
			continue;
		}

		if ( !(fds.revents & POLLIN) )
		{
			continue;
		}

		if( read(g_nTmdClientHandle, &pucData[ulReadSize], 1) <= 0 )
		{
			return FDI_ERR;
		}
		else
		{
			ulReadSize++;
		}
	}
#else
	unsigned long ulReadSize;
	int nResult;
	HUI_SOCKLEN_T len;
	int error;

	ulReadSize = 0;
	while( ulReadSize != ulSize )
	{
		nResult = HUI_SOCKET_Recv(g_nTmdClientHandle, &pucData[ulReadSize], 1, 0, ulTimeoutMs);
		if (nResult <= 0)
		{
			len = sizeof(error);
			HUI_SOCKET_Getsockopt(g_nTmdClientHandle, HUI_SOL_SOCKET, HUI_SO_ERROR, &error, &len);
			FDI_PRINT(FDI_PRT_ERROR, ("[%s,%d] HUI_SOCKET_Recv Error(%d) \n", __FUNCTION__, __LINE__, error));
			return FDI_ERR;
		}
		ulReadSize++;
	}
#endif

	return FDI_NO_ERR;
}


