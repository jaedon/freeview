/*******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
*******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#if defined(CONFIG_PLATFORM_ALI)
#include "hui_socket.h"
#endif

#if !defined(CONFIG_OS_RTOS)
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

/* chip manufacture's  header files */

/* humax header files */

/* model specific configuration header */
#include "fdi_common.h"
#include "fdi_ethernet.h"

#include "fvk.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#if (TMD_ETHERNET_TEST_LOCAL_PC==YES)
	//PC에서 테스트 
#define FDI_MES_SERVER_IP					"192.168.0.11"
#else
#define FDI_MES_SERVER_IP					"10.0.10.92"
#endif
#define FDI_MES_SERVER_PORT					8282

#define FDI_MES_CONNECTION_TIMEOUT			5*1000

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
int g_nMesClientHandle;


/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_MES_Open(void)
{
#if !defined(CONFIG_PLATFORM_ALI)	
	int nClientLen, nMode;
	struct sockaddr_in tClientAddr;
	unsigned long ulMesConnectionWaitTime = 0;

	FDI_PRINT(FDI_PRT_DBGINFO, ("[FDI_MES_Open] : Connect Server:%s, Port:%d\n", FDI_MES_SERVER_IP, FDI_MES_SERVER_PORT));
#if (MES_DOWNLOAD_MODE_ENABLE==YES)
	/* Check ethernet connection : timeout 5 second */
	while(!FDI_ETHERNET_IsLinked(FDI_ETHERNET_FACTORY_DEV))
	{
		FVK_TASK_Sleep(10);
		ulMesConnectionWaitTime += 10;
		if(FDI_MES_CONNECTION_TIMEOUT < ulMesConnectionWaitTime)
		{
			return FDI_ERR;
		}
	}
#endif	
	g_nMesClientHandle = socket(AF_INET, SOCK_STREAM, 0);
	tClientAddr.sin_family = AF_INET;
	tClientAddr.sin_addr.s_addr = inet_addr(FDI_MES_SERVER_IP);
	tClientAddr.sin_port = htons(FDI_MES_SERVER_PORT);

	nClientLen = sizeof(tClientAddr);

	if (connect(g_nMesClientHandle, (struct sockaddr *)&tClientAddr, nClientLen) < 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MES_Open] : connect error!!\n"));
		goto ERROR;
	}

	/* set non block mode */
	nMode = fcntl(g_nMesClientHandle, F_GETFL, 0);
	if( nMode < 0 )
	{
		FDI_PRINT(FDI_PRT_WARNING, ("[FDI_TMD_Open] : fcntl error!!\n"));
		goto ERROR;
	}
	nMode |= O_NONBLOCK;
	nMode = fcntl(g_nMesClientHandle, F_SETFL, nMode);
	if( nMode < 0 )
	{
		FDI_PRINT(FDI_PRT_WARNING, ("[FDI_TMD_Open] : fcntl error!!\n"));
		goto ERROR;
	}
	return FDI_NO_ERR;

ERROR:
	close(g_nMesClientHandle);
	return FDI_ERR;
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

	//FDI_PRINT(FDI_PRT_DBGINFO, ("[%s,%d] Connecting......\n", __FUNCTION__, __LINE__));

	g_nMesClientHandle = HUI_SOCKET_Socket(HUI_AF_INET, HUI_SOCK_STREAM, 0);
	if (g_nMesClientHandle < 0)
	{
		sock_err = SOCK_ERR_OPEN;
		goto done;
	}
	
	VK_MEM_Memset(&tClientAddr, 0, sizeof(HUI_SOCKETADDR_IN_T));
	tClientAddr.sin_family = HUI_AF_INET;
	tClientAddr.sin_addr.s_addr = HUI_INET_Addr(FDI_MES_SERVER_IP);
	tClientAddr.sin_port = HUI_INET_Htons(FDI_MES_SERVER_PORT);
	nClientLen = sizeof(HUI_SOCKETADDR_IN_T);	

	n = HUI_SOCKET_Fcntl(g_nMesClientHandle, HUI_F_SETFL, 1);
	if (n < 0)
	{
		sock_err = SOCK_ERR_FCNTL;
		goto done;
	}
	
	n = HUI_SOCKET_Connect(g_nMesClientHandle, (HUI_SOCKETADDR_IN_T*)&tClientAddr, nClientLen);
	if (n < 0)
	{
		sock_err = SOCK_ERR_CONNECT;
		goto done;
	}
	
	len = sizeof(error);
	n = HUI_SOCKET_Getsockopt(g_nMesClientHandle, HUI_SOL_SOCKET, HUI_SO_ERROR, &error, &len);
	if (n < 0 || error)
	{
		sock_err = SOCK_ERR_GETOPT;
		goto done;
	}

done:
	if (sock_err)
	{		
		FDI_PRINT(FDI_PRT_DBGINFO, ("[%s,%d] Connecting...... Fail, err=%d\n", __FUNCTION__, __LINE__, sock_err));
		HUI_SOCKET_Close(g_nMesClientHandle);
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
		                        FDI_MES_SERVER_IP, FDI_MES_SERVER_PORT));
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
FDI_ERR_CODE FDI_MES_SendData(unsigned char *pucData, unsigned long ulSize)
{
#if !defined(CONFIG_PLATFORM_ALI)	
	int i;
	unsigned char ucData;

	for( i = 0; i < ulSize; i++ )
	{
		ucData = pucData[i];
		if( write(g_nMesClientHandle, &ucData, 1) == -1 )
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_ETHERNET_TxData] : write error!!\n"));
			return FDI_ERR;
		}
	}
#else
	int written = 0;
	int rc;

	written = HUI_SOCKET_Send(g_nMesClientHandle, pucData, ulSize, 0);
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
FDI_ERR_CODE FDI_MES_ReceiveData(unsigned char *pucData, unsigned long ulSize, unsigned long ulTimeoutMs)
{
#if !defined(CONFIG_PLATFORM_ALI)	
	unsigned long ulReadSize, ulTime;

	ulReadSize = ulTime= 0;
	while( ulReadSize != ulSize )
	{
		if( read(g_nMesClientHandle, &pucData[ulReadSize], 1) <= 0 )
		{
			FVK_TASK_Sleep(10);
			ulTime += 10;
		}
		else
		{
			ulReadSize++;
		}
#if 1   /* timeout */	
		if( ulTime >= ulTimeoutMs )
		{
			return FDI_ERR;
		}
#endif 		
	}
#else
	unsigned long ulReadSize;
	int nResult;

	ulReadSize = 0;
	while( ulReadSize != ulSize )
	{
		nResult = HUI_SOCKET_Recv(g_nMesClientHandle, &pucData[ulReadSize], 1, 0, ulTimeoutMs);
		if (nResult <= 0)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[%s,%d] HUI_SOCKET_Recv Error(%d) \n", __FUNCTION__, __LINE__, nResult));
			return FDI_ERR; 
		}
		ulReadSize++;
	}
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
FDI_ERR_CODE FDI_MES_Close(void)
{
#if !defined(CONFIG_PLATFORM_ALI)	
	close(g_nMesClientHandle);
#else
	HUI_SOCKET_Close(g_nMesClientHandle);
#endif

	return FDI_NO_ERR;
}

