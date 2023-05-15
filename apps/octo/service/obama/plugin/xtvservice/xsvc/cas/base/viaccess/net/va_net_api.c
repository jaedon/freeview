/*
 * (c) 2011-2012 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include "va_def.h"
#include "va_setup.h"

#include "va_net.h"
#include "va_net_api.h"

#include "vkernel.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>

#include <errno.h>		/* errno */

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

HULONG g_ulVaNetSemId;

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/********************************************************************
*	External variables & function prototypes
********************************************************************/

/********************************************************************
*	Static variables
********************************************************************/
static UINT32 sVaNetQid;
static UINT32 sVaNetTid;
static UINT32 sVaNetRecvQid;
static UINT32 sVaNetRecvTid;
static char ip[64];

static VA_NET_Slot *sVaNetUnirSlot;
static VA_NET_Slot *sVaNetMultirSlot;


/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/
static void VA_NET_Destroy();
static void VA_NET_Task ( void * args );
static void VA_NET_RECV_Destroy();
static void VA_NET_RECV_Task ( void * args );

static HINT8 *acRecvBuff[kVA_SETUP_MAX_UNICAST_SOCKETS];
static HINT8 *acMultiRecvBuff[kVA_SETUP_MAX_MULTICAST_SOCKETS];
static HINT8 *acMultiStoreBuff[kVA_SETUP_MAX_MULTICAST_SOCKETS];

static int count = 0, sem_count=0;
BYTE* pSendData;
static INT sError = kVA_ILLEGAL_HANDLE;

static tVA_NET_Callback pfSTBUnicastCallback=NULL;

static void local_NET_CloseConnection(int sockfd)
{
	if (sockfd > 0)
	{
		close(sockfd);
	}
}

static HBOOL local_NET_IsNumber(unsigned char ch)
{
	// 상수 범위 ->  - 48, 문자 범위 (A~F) -> - 55
	// '0' ~ '9'
	if ('0' <= ch && ch <= '9')
	{
		return TRUE;
	}

	return FALSE;
}

static HBOOL local_NET_CheckIP(const char *pIPAddress)
{
	int len, i, count=0;

	if (pIPAddress == NULL)
	{
		return FALSE;
	}

	len = strlen(pIPAddress);

	if (len > 15)
	{
		return FALSE;
	}

	for (i = 0; i < len; i++)
	{
		if (local_NET_IsNumber(pIPAddress[i] == FALSE) && (pIPAddress[i] != '.'))
		{
			return FALSE;
		}

		if (pIPAddress[i] == '.')
		{
			count++;
		}
	}

	if (count == 3)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static void local_VA_NET_empty(VA_NET_Slot *sVaNetSlot)
{
	sVaNetSlot->dwVaNetHandle = VA_INVALID_HANDLE;
	sVaNetSlot->dwStbNetHandle = VA_INVALID_HANDLE;
	memset(sVaNetSlot->pIPAddress, 0, VA_NET_IP_LENGTH);
	sVaNetSlot->uiPort = 0;
	sVaNetSlot->bActive = FALSE;
	sVaNetSlot->bReceive = FALSE;
	sVaNetSlot->bLock = FALSE;
	sVaNetSlot->datacount = 0;
}

// Receive
#ifdef __VIACCESS50__
INT VA_NET_Receive_Proc(int index, UINT32 uiLength , UINT32 uiTimeout, UINT32 eStatus)
#else
INT VA_NET_Receive_Proc(int index, UINT32 uiLength , UINT32 uiTimeout)
#endif
{
	UINT32 aulMsg[4];
	HINT32 nRecv_len = 0;
	HINT32 nTotalLen = 0;

	struct timeval	time_out;
	fd_set		fds1;
	HINT32 status = 0;

	HxLOG_Print("[Enter] index : %d langth : %ld timeout : %ld\n", index, uiLength, uiTimeout);

	if(sVaNetUnirSlot[index].dwStbNetHandle <= 0)
	{
		HxLOG_Error("\n Socket Error, in RecvMsgFormServer" "\n");
		sVaNetUnirSlot[index].dwVaNetHandle = 0;
		goto end_func;
	}

	memset(acRecvBuff[index], 0, kVA_SETUP_UNICAST_MINIMUM_BUFFER_SIZE);

	while(1)
	{
		FD_ZERO(&fds1);
		FD_SET(sVaNetUnirSlot[index].dwStbNetHandle, &fds1);

		time_out.tv_sec = uiTimeout/1000;
		time_out.tv_usec = uiTimeout%1000;

		status = select(sVaNetUnirSlot[index].dwStbNetHandle+1, &fds1, NULL, NULL, &time_out);
		sVaNetUnirSlot[index].bLock = TRUE;

		if(status > 0 && FD_ISSET(sVaNetUnirSlot[index].dwStbNetHandle, &fds1))
		{
			nRecv_len = read(sVaNetUnirSlot[index].dwStbNetHandle, &acRecvBuff[index][nTotalLen], uiLength);
			HxLOG_Print("\n recv len: %d (%ld)", nRecv_len, uiLength);
			if(nRecv_len <= 0)
			{
				/* Recev error widthout network error */
				HxLOG_Error("\nERROR!! in read \n");
				aulMsg[0] = VA_NET_MSG_RECEIVE_FAIL;
				aulMsg[1] = sVaNetUnirSlot[index].dwVaNetHandle;
				VA_PostMessage(sVaNetRecvQid, aulMsg);
				sVaNetUnirSlot[index].bReceive = FALSE;
				return kVA_ILLEGAL_HANDLE;
			}
			else
			{
				nTotalLen += nRecv_len;
				if (nTotalLen>=uiLength)
				{
					HxLOG_Print("\n whole receive\n");
					goto end_func;
				}
			}
		}
		/* Timeout */
		else if(status == 0)
		{
			/* Timeout without network error. */
			HxLOG_Error("\n ERROR!! Timeout" "\n");
			//nTotalLen = 0;
			goto end_func;
		}
		else
		{
			/* Success */
			break;
		}
	}
	end_func:

	aulMsg[0] = VA_NET_MSG_RECEIVE_DONE;
	aulMsg[1] = index;
	aulMsg[2] = nTotalLen;
#ifdef __VIACCESS50__
	aulMsg[5] = eStatus;
#endif
	VA_PostMessage(sVaNetRecvQid, aulMsg);
	sVaNetUnirSlot[index].bReceive = FALSE;

	HxLOG_Print("[Exit]\n");

	return kVA_OK;
}

// Send
#ifdef __VIACCESS50__
INT VA_NET_Send_Proc(int index, UINT32 uiLength, UINT32 uiTimeout , UINT32 eStatus)
#else
INT VA_NET_Send_Proc(int index, UINT32 uiLength , UINT32 uiTimeout)
#endif
{
	UINT32 aulMsg[6];
	HINT32 nRecv_len = 0;
	HINT32 nTotalLen = 0;

	struct timeval	time_out;
	fd_set		fds1;
	HINT32 status = 0;

	HxLOG_Print("[Enter] index : %d length : %ld timeout : %ld\n", index, uiLength, uiTimeout);

	if(sVaNetUnirSlot[index].dwStbNetHandle <= 0)
	{
		HxLOG_Error("\n Socket Error, in SendMsgFormClient" "\n");
		sVaNetUnirSlot[index].dwVaNetHandle = 0;
		goto end_func;
	}

	while(1)
	{
		FD_ZERO(&fds1);
		FD_SET(sVaNetUnirSlot[index].dwStbNetHandle, &fds1);

		time_out.tv_sec = uiTimeout/1000;
		time_out.tv_usec = uiTimeout%1000;

		status = select(sVaNetUnirSlot[index].dwStbNetHandle+1, NULL, &fds1, NULL, &time_out);
		sVaNetUnirSlot[index].bLock = TRUE;

		if(status > 0 && FD_ISSET(sVaNetUnirSlot[index].dwStbNetHandle, &fds1))
		{
			nRecv_len = write(sVaNetUnirSlot[index].dwStbNetHandle, pSendData, uiLength);
			HxLOG_Print("\n recv len: %d (%ld)", nRecv_len, uiLength);
			if(nRecv_len <= 0)
			{
				/* Recev error widthout network error */
				HxLOG_Error("\nERROR!! in write \n");
				return kVA_ILLEGAL_HANDLE;
			}
			else
			{
				nTotalLen += nRecv_len;
				if (nTotalLen>=uiLength)
				{
					HxLOG_Print("\n whole send\n");
					goto end_func;
				}
			}
		}
		/* Timeout */
		else if(status == 0)
		{
			/* Timeout without network error. */
			HxLOG_Error("\n ERROR!! Timeout" "\n");
			//nTotalLen = 0;
			goto end_func;
		}
		else
		{
			/* Success */
			break;
		}
	}
	end_func:

	aulMsg[0] = VA_NET_MSG_SEND_DONE;
	aulMsg[1] = sVaNetUnirSlot[index].dwVaNetHandle;
#ifdef __VIACCESS50__
	aulMsg[5] = eStatus;
#endif
	VA_PostMessage(sVaNetQid, aulMsg);
	sVaNetUnirSlot[index].bReceive = FALSE;

	HxLOG_Print("[Exit]\n");

	return kVA_OK;
}

INT VA_NET_Multi_Proc(int index, UINT32 uiLength)
{
	UINT32 aulMsg[4];
	HINT32 nRecv_len = 0;
	HINT32 nTotalLen[kVA_SETUP_MAX_MULTICAST_SOCKETS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	struct sockaddr_in client_addr;
	int              client_addr_size;

	HINT32 status = 0;
	struct ip_mreq join_addr;
	//int    flags;
	int    i, max_fd=0;

	struct timeval	time_out;
	fd_set		fds1;

	HxLOG_Print("[Enter] index : %d length : %ld\n", index, uiLength);

	if(sVaNetMultirSlot[index].dwStbNetHandle <= 0)
	{
		HxLOG_Error("\n Socket Error, in RecvMsgFormServer" "\n");
		sVaNetMultirSlot[index].dwVaNetHandle = 0;
		goto end_func;
	}

	while(1)
	{
		client_addr_size = sizeof( client_addr);
		memset(&client_addr, 0, client_addr_size);

		FD_ZERO(&fds1);
		for (i=0; i<kVA_SETUP_MAX_MULTICAST_SOCKETS; i++)
		{
			if (sVaNetMultirSlot[i].dwStbNetHandle != VA_INVALID_HANDLE)
			{
				FD_SET(sVaNetMultirSlot[i].dwStbNetHandle, &fds1);
				if (max_fd < sVaNetMultirSlot[i].dwStbNetHandle)
					max_fd = sVaNetMultirSlot[i].dwStbNetHandle;
			}
		}

		time_out.tv_sec = 1, time_out.tv_usec = 0;
		status = select(max_fd + 1, (fd_set *)(&fds1), (fd_set *)0, (fd_set *)0, &time_out);

		if (count == 0)
		{
			HxLOG_Print("[VA_NET_Multi_Proc] exit while\n");
			return kVA_OK;
		}

		if(status <= 0)
		{
			HxLOG_Print("Wait packet . . . whole empty\n");
			continue;
		}

		for (i=0; i<kVA_SETUP_MAX_MULTICAST_SOCKETS; i++)
		{
			if (sVaNetMultirSlot[i].dwStbNetHandle == VA_INVALID_HANDLE)
				continue;

			if(status > 0)
			{
				if(FD_ISSET(sVaNetMultirSlot[i].dwStbNetHandle, &fds1))
				{
					status = 1;
				}
				else
				{
					status = 0;
				}
			}

			if(status <= 0)
			{
				HxLOG_Print("Wait packet . . .i : %d empty\n", i);
				continue;
			}

			nRecv_len = recvfrom(sVaNetMultirSlot[i].dwStbNetHandle, (void *)(&acMultiRecvBuff[i][nTotalLen[i]]), (size_t)uiLength, 0, (struct sockaddr *)(&client_addr), (socklen_t *)(&client_addr_size));

			if(nRecv_len > 0)
			{
				HxLOG_Print("[VA_NET_Multi_Proc] (%d) %d\n", (unsigned int)nTotalLen[i], nRecv_len);
				nTotalLen[i] += nRecv_len;
			}
			else
				HxLOG_Error("[VA_NET_Multi_Proc] Can not recv index=%d, recv=%d/%d !!!\n", index, nRecv_len, sizeof(acMultiRecvBuff[i]));

			if (((uiLength < kVA_SETUP_MULTICAST_MINIMUM_BUFFER_SIZE)) || (nTotalLen[i]==kVA_SETUP_MULTICAST_DATAGRAM_MAX_SIZE))
			{
				HxLOG_Print("[VA_NET_Multi_Proc] (%d) uiLength : %d\n", (unsigned int)nTotalLen[i], (int)uiLength);

				memcpy((void *)(acMultiStoreBuff[i]), (void *)(acMultiRecvBuff[i]), nTotalLen[i]);
				memset(acMultiRecvBuff[i], 0, kVA_SETUP_MULTICAST_MINIMUM_BUFFER_SIZE);
				sVaNetMultirSlot[i].bLock = TRUE;
				acMultiStoreBuff[i][nTotalLen[i]] = '\0';
				aulMsg[0] = VA_NET_MSG_MULTI_NOTIFY;
				aulMsg[1] = i;
				aulMsg[2] = nTotalLen[i];
				VA_PostMessage(sVaNetQid, aulMsg);
				nTotalLen[i] = 0;
			}
		}
	}

	HxLOG_Print("[VA_NET_Multi_Proc] (%d)\n", (unsigned int)nTotalLen);

	end_func:

	join_addr.imr_multiaddr.s_addr = inet_addr(ip);
	join_addr.imr_interface.s_addr = htonl(INADDR_ANY);

	status = setsockopt(sVaNetMultirSlot[index].dwStbNetHandle,IPPROTO_IP,IP_DROP_MEMBERSHIP,(void*)&join_addr,sizeof(join_addr));

	sVaNetMultirSlot[index].bReceive = FALSE;

	HxLOG_Print("[Exit]\n");

	return kVA_OK;
}

/*
	initialize variables and start task
*/
void VA_NET_Init()
{
	int nErr;
	int i;

	HxLOG_Print("[Enter]\n");

	//Semaphore Create
	nErr = VA_SEM_Create(VA_NET_SEM_NAME, &g_ulVaNetSemId);
	if (nErr)
	{
		HxLOG_Error("failed to create semaphore (%d)\n", nErr);
		VA_ASSERT(nErr == VK_OK);
	}

	// Slot Create
	sVaNetUnirSlot = (VA_NET_Slot *)VA_MEM_Alloc(sizeof(VA_NET_Slot) * kVA_SETUP_MAX_UNICAST_SOCKETS);
	VA_ASSERT(sVaNetUnirSlot != NULL);

	for (i = 0; i < kVA_SETUP_MAX_UNICAST_SOCKETS; i++)
	{
		sVaNetUnirSlot[i].pIPAddress = (char *)VA_MEM_Alloc(sizeof(char) * VA_NET_IP_LENGTH);
		local_VA_NET_empty(&sVaNetUnirSlot[i]);
	}

	sVaNetMultirSlot = (VA_NET_Slot *)VA_MEM_Alloc(sizeof(VA_NET_Slot) * kVA_SETUP_MAX_MULTICAST_SOCKETS);
	VA_ASSERT(sVaNetMultirSlot != NULL);

	for (i = 0; i < kVA_SETUP_MAX_MULTICAST_SOCKETS; i++)
	{
		sVaNetMultirSlot[i].pIPAddress = (char *)VA_MEM_Alloc(sizeof(char) * VA_NET_IP_LENGTH);
		local_VA_NET_empty(&sVaNetMultirSlot[i]);
	}

	// Queue Create
	nErr = VA_CreateMessageQueue(VA_NET_QUEUE_NAME, &sVaNetQid);
	if (nErr)
	{
		HxLOG_Error("failed to create message queue (%d)\n", nErr);
		VA_ASSERT(nErr == VK_OK);
	}

	nErr = VK_TASK_Create(VA_NET_Task, VA_NET_TASK_PRIORITY, VA_NET_TASK_STACK_SIZE, VA_NET_TASK_NAME, NULL, &sVaNetTid, 0);
	if (nErr)
	{
		HxLOG_Error("failed to create task (%d)\n", nErr);
		VA_ASSERT(nErr == VK_OK);
	}

	nErr = VK_TASK_Start(sVaNetTid);
	if (nErr)
	{
		HxLOG_Error("failed to start task (%d)\n", nErr);
		VA_ASSERT(nErr == VK_OK);
	}

	nErr = VA_CreateMessageQueue(VA_NET_RECV_QUEUE_NAME, &sVaNetRecvQid);
	if (nErr)
	{
		HxLOG_Error("failed to create message queue (%d)\n", nErr);
		VA_ASSERT(nErr == VK_OK);
	}

	nErr = VK_TASK_Create(VA_NET_RECV_Task, VA_NET_TASK_PRIORITY, VA_NET_TASK_STACK_SIZE, VA_NET_RECV_TASK_NAME, NULL, &sVaNetRecvTid, 0);
	if (nErr)
	{
		HxLOG_Error("failed to create task (%d)\n", nErr);
		VA_ASSERT(nErr == VK_OK);
	}

	nErr = VK_TASK_Start(sVaNetRecvTid);
	if (nErr)
	{
		HxLOG_Error("failed to start task (%d)\n", nErr);
		VA_ASSERT(nErr == VK_OK);
	}

	for (i=0; i<kVA_SETUP_MAX_UNICAST_SOCKETS; i++)
	{
		acRecvBuff[i] = NULL;
	}
	for (i=0; i<kVA_SETUP_MAX_MULTICAST_SOCKETS; i++)
	{
		acMultiRecvBuff[i] = acMultiStoreBuff[i] = NULL;
	}

	HxLOG_Print("[Exit]\n");
}

static void VA_NET_Destroy()
{
	int i;

	HxLOG_Print("[Enter]\n");

	for (i = 0; i < kVA_SETUP_MAX_UNICAST_SOCKETS; i++)
	{
		local_VA_NET_empty(&sVaNetUnirSlot[i]);
		VA_MEM_Free(sVaNetUnirSlot[i].pIPAddress);
	}
	VA_MEM_Free(sVaNetUnirSlot);

	for (i = 0; i < kVA_SETUP_MAX_MULTICAST_SOCKETS; i++)
	{
		local_VA_NET_empty(&sVaNetMultirSlot[i]);
		VA_MEM_Free(sVaNetMultirSlot[i].pIPAddress);
	}
	VA_MEM_Free(sVaNetMultirSlot);

	// destory message queue
	VA_MSG_Destroy(sVaNetQid);

	HxLOG_Print("[Exit]\n");
}

static void VA_NET_RECV_Destroy()
{
	VA_MSG_Destroy(sVaNetRecvQid);
}

static void VA_NET_RECV_Task ( void * args )
{
	UINT32 aulMsg[4];
	UINT32 ulMsgType;

	HBOOL bWaitMsg = TRUE;
#ifdef __VIACCESS50__
	DWORD dwVaNetHandle;
	tVA_NET_Status	eStatus ;
#endif

	HxLOG_Print("[Enter]\n");



	while (bWaitMsg)
	{
		if (VA_GetMessage(sVaNetRecvQid, (void *)&aulMsg) == VK_OK)
		{
			ulMsgType = aulMsg[0];

			HxLOG_Print("[VA_NET_RECV_Task] MSG Received msg_type(%d)\n", (unsigned int)ulMsgType);

			switch (ulMsgType)
			{
				case VA_NET_MSG_RECEIVE_FAIL:
					HxLOG_Print("[VA_NET_RECV_Task] VA_NET_MSG_RECEIVE_FAIL(%d)\n", (unsigned int)aulMsg[1]);
#if !defined (__VIACCESS50__)
					sError = VA_NET_UnicastReceiveFailed(aulMsg[1]);
#endif
					break;
				case VA_NET_MSG_RECEIVE_PROC:
					HxLOG_Print("[VA_NET_RECV_Task] VA_NET_MSG_RECEIVE_PROC(%d)\n", (unsigned int)aulMsg[1]);
#ifdef __VIACCESS50__
					sError = VA_NET_Receive_Proc(aulMsg[1], aulMsg[2], aulMsg[3], aulMsg[5]);
#else
					sError = VA_NET_Receive_Proc(aulMsg[1], aulMsg[2], aulMsg[3]);
#endif
					break;
				case VA_NET_MSG_RECEIVE_DONE:
					HxLOG_Print("[VA_NET_RECV_Task] VA_NET_MSG_RECEIVE_DONE(%d)\n", (unsigned int)aulMsg[1]);
#ifdef __VIACCESS50__
					pfSTBUnicastCallback(dwVaNetHandle,aulMsg[5],0,NULL);
#else
					sError = VA_NET_UnicastReceiveDone(sVaNetUnirSlot[aulMsg[1]].dwVaNetHandle, aulMsg[2], aulMsg[2] ? acRecvBuff[aulMsg[1]] : NULL);
#endif
					break;
				case VA_NET_MSG_MULTI_NOTIFY_PROC:
					HxLOG_Print("[VA_NET_MULTI_NOTIFY_Task] VA_NET_MSG_MULTI_NOTIFY_PROC(%d)\n", (unsigned int)aulMsg[1]);
					sError = VA_NET_Multi_Proc(aulMsg[1], aulMsg[2]);
					break;
			}
		}
	}

	// this task is going to be terminated
	VA_NET_RECV_Destroy();

	HxLOG_Print("[Exit]\n");
}

static void VA_NET_Task ( void * args )
{
	UINT32 aulMsg[5];
	UINT32 ulMsgType;
	HBOOL bWaitMsg = TRUE;

#ifdef __VIACCESS50__
	DWORD dwVaNetHandle;
	tVA_NET_Status	eStatus ;
#endif

	HxLOG_Print("[Enter]\n");
	while (bWaitMsg)
	{
		if (VA_GetMessage(sVaNetQid, (void *)&aulMsg) == VK_OK)
		{
			ulMsgType = aulMsg[0];

			HxLOG_Print("[VA_NET_Task] MSG Received msg_type(%d)\n", (unsigned int)ulMsgType);
#ifdef __VIACCESS50__
			dwVaNetHandle=aulMsg[1];
			eStatus=aulMsg[5];
#endif
			switch (ulMsgType)
			{
				case VA_NET_MSG_OPEN_FAIL:
					HxLOG_Print("[VA_NET_Task] VA_NET_MSG_OPEN_FAIL(%d)\n", (unsigned int)aulMsg[1]);
#ifdef __VIACCESS50__
					pfSTBUnicastCallback(dwVaNetHandle,eStatus,0,NULL);
#else
					sError = VA_NET_UnicastOpenFailed(aulMsg[1]);
#endif
					break;
				case VA_NET_MSG_OPEN_DONE:
					HxLOG_Print("[VA_NET_Task] VA_NET_MSG_OPEN_DONE(%d)\n", (unsigned int)aulMsg[1]);
#ifdef __VIACCESS50__
					pfSTBUnicastCallback(dwVaNetHandle,eStatus,0,NULL);
#else
					sError = VA_NET_UnicastOpenDone(aulMsg[1]);
#endif
					sVaNetUnirSlot[aulMsg[2]].bActive = TRUE;
					break;
				case VA_NET_MSG_SEND_PROC:
					HxLOG_Print("[VA_NET_Task] VA_NET_MSG_SEND_PROC(%d)\n", (unsigned int)aulMsg[1]);
#ifdef __VIACCESS50__
					sError = VA_NET_Send_Proc(aulMsg[1], aulMsg[2], aulMsg[3], aulMsg[5]);
#else
					sError = VA_NET_Send_Proc(aulMsg[1], aulMsg[2], aulMsg[2]);
#endif

					break;
				case VA_NET_MSG_SEND_DONE:
					HxLOG_Print("[VA_NET_Task] VA_NET_MSG_SEND_DONE(%d)\n", (unsigned int)aulMsg[1]);
#ifdef __VIACCESS50__
					pfSTBUnicastCallback(dwVaNetHandle,eVA_SEND_DONE,0,NULL);
#else
					sError = VA_NET_UnicastSendDone(aulMsg[1]);
#endif
					break;
				case VA_NET_MSG_MULTI_OPEN_FAIL:
					HxLOG_Print("[VA_NET_Task] VA_NET_MSG_MULTI_OPEN_FAIL(%d)\n", (unsigned int)aulMsg[1]);
#ifdef __VIACCESS50__
					pfSTBUnicastCallback(dwVaNetHandle,eStatus,0,NULL);
#else
					sError = VA_NET_MulticastOpenFailed(aulMsg[1]);
#endif
					break;

				case VA_NET_MSG_MULTI_OPEN_DONE:
					HxLOG_Print("[VA_NET_Task] VA_NET_MSG_MULTI_OPEN_DONE(%d)\n", (unsigned int)aulMsg[3]);
#ifdef __VIACCESS50__
					pfSTBUnicastCallback(dwVaNetHandle,eStatus,0,NULL);
#else
					sError = VA_NET_MulticastOpenDone(aulMsg[3]);

					sVaNetMultirSlot[aulMsg[1]].bActive = TRUE;

					HxLOG_Print("[VA_NET_Task] count : %d\n", count);
					if (sError == kVA_OK && count == 1)
					{
						HxLOG_Print("[VA_NET_Task] VA_NET_MSG_MULTI_NOTIFY_PROC(%d)\n", (unsigned int)aulMsg[1]);
						aulMsg[0] = VA_NET_MSG_MULTI_NOTIFY_PROC;
						VA_PostMessage(sVaNetRecvQid, aulMsg);
					}
#endif
					break;

				case VA_NET_MSG_MULTI_NOTIFY:
					HxLOG_Print("[VA_NET_MULTI_NOTIFY_Task] VA_NET_MSG_MULTI_NOTIFY(%d)\n", (unsigned int)aulMsg[1]);
					VA_ASSERT(g_ulVaNetSemId);
					VA_SEM_Get(g_ulVaNetSemId);
					sem_count++;
#if !defined(__VIACCESS50__)
					sError = VA_NET_MulticastNotify(sVaNetMultirSlot[aulMsg[1]].dwVaNetHandle, aulMsg[2], aulMsg[2] ? acMultiStoreBuff[aulMsg[1]] : NULL);
#endif
					break;
			}
		}
	}
	// this task is going to be terminated
	VA_NET_Destroy();

	HxLOG_Print("[Exit]\n");
}

DWORD VA_NET_UnicastOpen( DWORD dwVaNetHandle,
		                      const char *pIPAddress, UINT16 uiPort, UINT32 uiTimeout,
        		              tVA_NET_Callback pfVaUnicastCallback)
{
	DWORD		dwStbNetHandle = VA_INVALID_HANDLE;
	HINT32		i;
	HUINT16		usFirstIPDigit;
	HUINT8		szIP[64];
	HUINT8		szFirstIPDigit[4];
	HUINT32		aulMsg[6];
	HINT32		address_len = 0;
	HINT32		ret;
	HINT32		sockfd;
	tVA_NET_Status	eStatus = eVA_NOT_CONNECTED;
	struct timeval 	time_out;
	struct sockaddr_in ic_gateway;


	HxLOG_Print("[Enter] VaHandle : %ld ipAddress : %s Port : %d Timeout : %ld\n", dwVaNetHandle, pIPAddress, uiPort, uiTimeout);
/*
	함수 포인터 연결한다.
	전역으로 사용한다.
*/
	pfSTBUnicastCallback = &pfVaUnicastCallback;

	// NET_UNICASE_OPEN_ERR 2, 3, 4
	if ((pIPAddress == NULL) || (uiPort == 0) || (dwVaNetHandle == kVA_ILLEGAL_HANDLE))
	{
		return kVA_ILLEGAL_HANDLE;
	}

	HxSTD_memset(szIP, 0, 64);

	if (local_NET_CheckIP(pIPAddress))
	{
		// NET_UNICASE_OPEN_ERR 1
		HxSTD_memset(szFirstIPDigit, 0, 4);
		strncpy(szFirstIPDigit, pIPAddress, 3);

		for (i = 0; i < 3; i++)
		{
			if (szFirstIPDigit[i] == '.')
			{
				szFirstIPDigit[i] = '\0';
				break;
			}
		}

		usFirstIPDigit = atoi(szFirstIPDigit);
		if ((usFirstIPDigit >= 224) && (usFirstIPDigit <= 239))
		{
			return kVA_ILLEGAL_HANDLE;
		}

		HxSTD_memcpy(szIP, pIPAddress, strlen(pIPAddress));
	}
	else
	{
		//Driver Test 편의를 위해
		//memset(szIP, 0, strlen(szIP));
		//memcpy((void *)(szIP), (void *)("10.0.14.180"), strlen("10.0.14.180"));
		struct hostent 	*he;
		struct in_addr 	**addr_list;

		HxLOG_Print ("\n IP : %s \n", pIPAddress);

		if ((he = gethostbyname(pIPAddress)) == NULL)
		{
			HxLOG_Error("gethostbyname error...\n");
			return kVA_ERROR;
		}

		addr_list = (struct in_addr **)he->h_addr_list;

		if (addr_list[0] !=NULL)
		{
			strncpy(szIP, inet_ntoa(*addr_list[0]), sizeof(szIP) - 1);
			HxLOG_Print ("pIPAddress : %s\n", szIP);
		}

		if (!local_NET_CheckIP(szIP))
		{
			return kVA_ERROR;
		}

		HxLOG_Print ("\n IP : %s \n", szIP);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		HxLOG_Error("socket error...\n");
		return sockfd;

	}

	memset(&ic_gateway, 0, sizeof(ic_gateway));
	ic_gateway.sin_family = AF_INET;
	ic_gateway.sin_addr.s_addr = inet_addr(szIP);
	ic_gateway.sin_port = htons(uiPort);
	address_len = sizeof(ic_gateway);

	time_out.tv_sec = uiTimeout/1000;
	time_out.tv_usec = uiTimeout%1000;

	ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &time_out, sizeof(time_out));
	HxLOG_Print ("SO_RCVTIMEO returns %d: %m\n", ret);
	ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &time_out, sizeof(time_out));
	HxLOG_Print ("SO_SNDTIMEO returns %d: %m\n", ret);

	HxLOG_Print ("call connect\n");
	ret = connect(sockfd,(struct sockaddr *)&ic_gateway, address_len);
	HxLOG_Print ("connect return %d, %m\n", ret);

	// NET_UNICAST_OPEN 2, 3 : timeout 처리 필요
	if ((uiTimeout == 0) || (ret < 0))
	{
		aulMsg[0] = VA_NET_MSG_OPEN_FAIL;
		aulMsg[1] = dwVaNetHandle;
		//aulMsg[2] = i;
		aulMsg[5] = eVA_OPEN_FAILED;
		VA_PostMessage(sVaNetQid, aulMsg);
		return sockfd;
	}

	for (i = 0; i < kVA_SETUP_MAX_UNICAST_SOCKETS; i++)
	{
		if (sVaNetUnirSlot[i].dwVaNetHandle == VA_INVALID_HANDLE)
		{
			sVaNetUnirSlot[i].dwVaNetHandle = dwVaNetHandle;
			sVaNetUnirSlot[i].dwStbNetHandle = sockfd;
			HxSTD_memcpy((void *)(sVaNetUnirSlot[i].pIPAddress), (void *)(szIP), strlen(szIP));
			sVaNetUnirSlot[i].uiPort = uiPort;
			break;
		}
	}

	// OpenDone call
	if ((i < kVA_SETUP_MAX_UNICAST_SOCKETS)  && ((dwStbNetHandle == 0) || (ret == 0)))
	{
		aulMsg[0] = VA_NET_MSG_OPEN_DONE;
		aulMsg[1] = dwVaNetHandle;
		aulMsg[2] = i;
#ifdef __VIACCESS50__
		aulMsg[5] = eVA_OPEN_DONE;
#endif
		VA_PostMessage(sVaNetQid, aulMsg);
		if (acRecvBuff[i] == NULL)
			acRecvBuff[i] = (HINT8 *)VA_MEM_Alloc(kVA_SETUP_UNICAST_MINIMUM_BUFFER_SIZE);
	}

;
	HxLOG_Print ("[Exit]\n");

	return sockfd;
}

INT VA_NET_UnicastReceive( DWORD dwStbNetHandle,
						  UINT32 uiLength ,
						  UINT32 uiTimeout )
{
	int i;
	UINT32 aulMsg[6];
	char buf[kVA_SETUP_UNICAST_MINIMUM_BUFFER_SIZE];

	HxLOG_Print ("[Enter] StbHandle : %ld Length : %ld Timeout : %ld\n", dwStbNetHandle, uiLength, uiTimeout);

	// NET_UNICAST_RECEIVE_ERR 1, 3, 4
	if ((dwStbNetHandle == kVA_ILLEGAL_HANDLE) || (uiLength == 0) || (uiLength > kVA_SETUP_UNICAST_MINIMUM_BUFFER_SIZE))
		return kVA_INVALID_PARAMETER;

	// NET_UNICAST_RECEIVE_ERR 2
	for ( i = 0; i < kVA_SETUP_MAX_UNICAST_SOCKETS; i++ )
	{
		HxLOG_Print ("%ld %ld %ld %d\n", sVaNetUnirSlot[i].dwStbNetHandle, sVaNetUnirSlot[i].dwVaNetHandle, dwStbNetHandle, sVaNetUnirSlot[i].bActive);
		if ((sVaNetUnirSlot[i].dwStbNetHandle == dwStbNetHandle) && (sVaNetUnirSlot[i].bActive == TRUE))
		{
			if (sVaNetUnirSlot[i].bReceive == FALSE)
			{
				sVaNetUnirSlot[i].bReceive = TRUE;
				break;
			}
			else
			{
				return kVA_RESOURCE_BUSY;
			}
		}
	}
	if (i == kVA_SETUP_MAX_UNICAST_SOCKETS)
		return kVA_INVALID_PARAMETER;

	// NET UNICAST_RECEIVE_ERR 5
	if(recv(dwStbNetHandle, buf, sizeof(buf), MSG_PEEK | MSG_DONTWAIT) == 0)
	{
//		HxLOG_Print ("%d [%s]\n", errno, strerror(errno));
		return kVA_CONNECTION_LOST;
	}

	// NET UNICAST_RECEIVE_ERR 6
	// return kVA_INVALID_PARAMETER;

	aulMsg[0] = VA_NET_MSG_RECEIVE_PROC;
	aulMsg[1] = i;
	aulMsg[2] = uiLength;
	aulMsg[3] = uiTimeout;
	aulMsg[4] = uiLength;
#ifdef __VIACCESS50__
	aulMsg[5] = eVA_RECEIVE_DONE;
#endif
	VA_PostMessage(sVaNetRecvQid, aulMsg);

	HxLOG_Print ("[Exit]\n");

	return kVA_OK;
}

INT VA_NET_UnicastReceiveRelease( DWORD dwStbNetHandle )
{
	int i;

	HxLOG_Print ("[Enter] StbHandle : %ld\n", dwStbNetHandle);

	// NET_UNICAST_RECEIVE_ERR 1
	if (dwStbNetHandle == kVA_ILLEGAL_HANDLE)
		return kVA_INVALID_PARAMETER;
#if !defined(__VIACCESS50__)
	//NET_UNICAST_RECEIVE_RELEASE_ERR 2
	if (sError == kVA_RELEASE_RESOURCES)
		return kVA_RESOURCE_NOT_LOCKED;
#endif

	//NET_UNICAST_RECEIVE_RELEASE_ERR 3
	for ( i = 0; i < kVA_SETUP_MAX_UNICAST_SOCKETS; i++ )
	{
		HxLOG_Print ("%ld %ld %ld %d\n", sVaNetUnirSlot[i].dwStbNetHandle, sVaNetUnirSlot[i].dwVaNetHandle, dwStbNetHandle, sVaNetUnirSlot[i].bActive);
		if ((sVaNetUnirSlot[i].dwStbNetHandle == dwStbNetHandle) && (sVaNetUnirSlot[i].bActive == TRUE))
		{
			if (sVaNetUnirSlot[i].bLock == TRUE)
				sVaNetUnirSlot[i].bLock = FALSE;
			else
				return kVA_RESOURCE_NOT_LOCKED;
			break;
		}
	}

	HxLOG_Print ("[Exit]\n");

	return kVA_OK;
}

INT VA_NET_UnicastSend ( DWORD dwStbNetHandle,
                        UINT32 uiDataLength ,
						BYTE* pData,
						UINT32 uiTimeout )
{
	int i;
	UINT32 aulMsg[4];
	char buf[kVA_SETUP_UNICAST_MINIMUM_BUFFER_SIZE];

	HxLOG_Print ("[Enter] StbHandle : %ld Length : %ld Timeout : %ld\n", dwStbNetHandle, uiDataLength, uiTimeout);

	//NET_UNICAST_SEND_ERR 1, 3, 4
	if ((dwStbNetHandle == kVA_ILLEGAL_HANDLE) || (uiDataLength == 0) || (pData == NULL))
		return kVA_INVALID_PARAMETER;

	// NET_UNICAST_SEND_ERR 2
	for ( i = 0; i < kVA_SETUP_MAX_UNICAST_SOCKETS; i++ )
	{
		HxLOG_Print ("%ld %ld %ld %d\n", sVaNetUnirSlot[i].dwStbNetHandle, sVaNetUnirSlot[i].dwVaNetHandle, dwStbNetHandle, sVaNetUnirSlot[i].bActive);
		if ((sVaNetUnirSlot[i].dwStbNetHandle == dwStbNetHandle) && (sVaNetUnirSlot[i].bActive == TRUE))
			break;
	}
	if (i == kVA_SETUP_MAX_UNICAST_SOCKETS)
		return kVA_INVALID_PARAMETER;

	if(recv(dwStbNetHandle, buf, sizeof(buf), MSG_PEEK | MSG_DONTWAIT) == 0)
	{
//		HxLOG_Print ("%d [%s]\n", errno, strerror(errno));
		return kVA_CONNECTION_LOST;
	}

	pSendData = pData;

	aulMsg[0] = VA_NET_MSG_SEND_PROC;
	aulMsg[1] = i;
	aulMsg[2] = uiDataLength;
	aulMsg[3] = uiTimeout;
	VA_PostMessage(sVaNetQid, aulMsg);

	HxLOG_Print ("[Exit]\n");

	return kVA_OK;
}

INT VA_NET_UnicastClose( DWORD dwStbNetHandle )
{
	int i;
	HxLOG_Print ("[Enter] StbHandle : %ld\n", dwStbNetHandle);

	//NET_UNICAST_CLOSE_ERR 1
	if (dwStbNetHandle == kVA_ILLEGAL_HANDLE)
		return kVA_INVALID_PARAMETER;

	// NET_UNICAST_CLOSE_ERR 2
	for ( i = 0; i < kVA_SETUP_MAX_UNICAST_SOCKETS; i++ )
	{
		HxLOG_Print ("%ld %ld %ld %d\n", sVaNetUnirSlot[i].dwStbNetHandle, sVaNetUnirSlot[i].dwVaNetHandle, dwStbNetHandle, sVaNetUnirSlot[i].bActive);
		if ((sVaNetUnirSlot[i].dwStbNetHandle == dwStbNetHandle) && (sVaNetUnirSlot[i].bActive == TRUE))
		{
			(void)local_NET_CloseConnection(sVaNetUnirSlot[i].dwStbNetHandle);
			local_VA_NET_empty(&sVaNetUnirSlot[i]);
			if (acRecvBuff[i] != NULL)
				VA_MEM_Free(acRecvBuff[i]);
			acRecvBuff[i] = NULL;
			break;
		}
	}
	if (i == kVA_SETUP_MAX_UNICAST_SOCKETS)
		return kVA_INVALID_PARAMETER;

	HxLOG_Print ("[Exit]\n");

	return kVA_OK;
}

DWORD VA_NET_MulticastOpen( DWORD dwVaNetHandle,
                           	   const char *pIPAddress, UINT16 uiPort, UINT32 uiBufferSize,
                           	   tVA_NET_Callback  pfVaMulticastCallback )
{
	int i, IP;
	char IP1[4];
	UINT32 aulMsg[5];
	int sockfd, state;
	struct sockaddr_in ic_gateway;
	HINT32 address_len = 0;
	struct ip_mreq join_addr;
	int option = 1;

	HxLOG_Print ("[Enter]\n");

	// NET_MULTICASE_OPEN_ERR 1, 3, 4
	if ((pIPAddress == NULL) || (uiPort == 0) || (dwVaNetHandle == kVA_ILLEGAL_HANDLE))
		return kVA_ILLEGAL_HANDLE;

	// NET_MULTICASE_OPEN_ERR 2
	if (local_NET_CheckIP(pIPAddress))
	{
		// NET_UNICASE_OPEN_ERR 1
		memset(IP1, 0, 4);
		strncpy(IP1, pIPAddress, 3);
		IP = atoi(IP1);
		if ((IP<224) || (IP>239))
			return kVA_ILLEGAL_HANDLE;
	}

	HxLOG_Print ("%ld %s %d %ld\n", dwVaNetHandle, pIPAddress, uiPort, uiBufferSize);

	sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sockfd < 0)
	{
		HxLOG_Print ("socket error...\n");
		return kVA_ERROR;
	}

	for ( i = 0; i < kVA_SETUP_MAX_MULTICAST_SOCKETS; i++ )
	{
		if (sVaNetMultirSlot[i].uiPort == uiPort)
		{
			aulMsg[0] = VA_NET_MSG_MULTI_OPEN_FAIL;
			aulMsg[1] = dwVaNetHandle;//dwStbNetHandle;
			VA_PostMessage(sVaNetQid, aulMsg);
			goto end_func;
		}
	}

	memset(&ic_gateway, 0, sizeof(ic_gateway));
	ic_gateway.sin_family = AF_INET;
	ic_gateway.sin_addr.s_addr = htonl(INADDR_ANY);
	ic_gateway.sin_port = htons(uiPort);
	address_len = sizeof(ic_gateway);

	state = setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(void*)&option,sizeof(option));

	if (bind(sockfd, (struct sockaddr *)&ic_gateway, address_len) == -1)
	{
		HxLOG_Error( "\n bind Error"  "\n");
		goto end_func;
	}

	join_addr.imr_multiaddr.s_addr = inet_addr(pIPAddress);
	join_addr.imr_interface.s_addr = htonl(INADDR_ANY);

	state = setsockopt(sockfd,IPPROTO_IP,IP_ADD_MEMBERSHIP,(void*)&join_addr,sizeof(join_addr));

	for ( i = 0; i < kVA_SETUP_MAX_MULTICAST_SOCKETS; i++ )
	{
		if (sVaNetMultirSlot[i].dwVaNetHandle == VA_INVALID_HANDLE)
		{
			sVaNetMultirSlot[i].dwVaNetHandle = dwVaNetHandle;
			sVaNetMultirSlot[i].dwStbNetHandle = sockfd;
			memcpy((void *)(sVaNetMultirSlot[i].pIPAddress), (void *)(pIPAddress), strlen(pIPAddress));
			sVaNetMultirSlot[i].uiPort = uiPort;
			break;
		}
	}

	strncpy(ip, pIPAddress, strlen(pIPAddress));
	count++;
	// MultiOpenDone call
	if (i < kVA_SETUP_MAX_MULTICAST_SOCKETS)
	{
		aulMsg[0] = VA_NET_MSG_MULTI_OPEN_DONE;
		aulMsg[1] = i;
		aulMsg[2] = uiBufferSize;
		aulMsg[3] = dwVaNetHandle;
#ifdef __VIACCESS50__
		aulMsg[5] = eVA_OPEN_DONE;
#endif
		VA_PostMessage(sVaNetQid, aulMsg);
		if (acMultiRecvBuff[i] == NULL)
			acMultiRecvBuff[i] = (HINT8 *)VA_MEM_Alloc(kVA_SETUP_MULTICAST_MINIMUM_BUFFER_SIZE);
		if (acMultiStoreBuff[i] == NULL)
			acMultiStoreBuff[i] = (HINT8 *)VA_MEM_Alloc(kVA_SETUP_MULTICAST_MINIMUM_BUFFER_SIZE);
		memset(acMultiRecvBuff[i], 0, kVA_SETUP_MULTICAST_MINIMUM_BUFFER_SIZE);
		memset(acMultiStoreBuff[i], 0, kVA_SETUP_MULTICAST_MINIMUM_BUFFER_SIZE);
	}

end_func:

	HxLOG_Print ("[Exit]\n");

	return sockfd;
}

INT VA_NET_MulticastNotifyRelease( DWORD dwStbNetHandle )
{
	int i;

	HxLOG_Print ("[Enter] StbHandle : %ld\n", dwStbNetHandle);

	// NET_MULTICASE_NOTIFY_RELEASE_ERR 1
	if (dwStbNetHandle == kVA_ILLEGAL_HANDLE)
		return kVA_INVALID_PARAMETER;

	// NET_MULTICASE_NOTIFY_RELEASE_ERR 2, 3
	for ( i = 0; i < kVA_SETUP_MAX_MULTICAST_SOCKETS; i++ )
	{
		HxLOG_Print ("%ld %ld %ld %d\n", sVaNetMultirSlot[i].dwStbNetHandle, sVaNetMultirSlot[i].dwVaNetHandle, dwStbNetHandle, sVaNetMultirSlot[i].bActive);
		if ((sVaNetMultirSlot[i].dwStbNetHandle == dwStbNetHandle) && (sVaNetMultirSlot[i].bActive == TRUE))
		{
			HxLOG_Print ("%d\n", sVaNetMultirSlot[i].bLock );
			if (sVaNetMultirSlot[i].bLock == TRUE)
			{
				sVaNetMultirSlot[i].datacount--;
				if (sVaNetMultirSlot[i].datacount == 0)
					sVaNetMultirSlot[i].bLock = FALSE;
			}
			else
				return kVA_RESOURCE_NOT_LOCKED;
			break;
		}
	}
	if (i == kVA_SETUP_MAX_MULTICAST_SOCKETS)
		return kVA_INVALID_PARAMETER;
	HxLOG_Print("[VA_NET_MulticastNotifyRelease] Locked\n");

	sem_count--;
	VA_SEM_Release(g_ulVaNetSemId);

	HxLOG_Print ("[Exit]\n");

	return kVA_OK;
}


INT VA_NET_MulticastClose( DWORD dwStbNetHandle )
{
	int i;

	HINT32 status = 0;
	struct ip_mreq join_addr;
	DWORD Handle;

	HxLOG_Print ("[Enter] StbHandle : %ld\n", dwStbNetHandle);

	// NET_MULTICASE_CLOSE_ERR 1
	if (dwStbNetHandle == kVA_ILLEGAL_HANDLE)
		return kVA_INVALID_PARAMETER;

	// NET_MULTICASE_CLOSE_ERR 2
	for ( i = 0; i < kVA_SETUP_MAX_MULTICAST_SOCKETS; i++ )
	{
		HxLOG_Print("%ld %ld %ld %d\n", sVaNetMultirSlot[i].dwStbNetHandle, sVaNetMultirSlot[i].dwVaNetHandle, dwStbNetHandle, sVaNetMultirSlot[i].bActive);
		if ((sVaNetMultirSlot[i].dwStbNetHandle == dwStbNetHandle) && (sVaNetMultirSlot[i].bActive == TRUE))
		{
			Handle = sVaNetMultirSlot[i].dwStbNetHandle;
			local_VA_NET_empty(&sVaNetMultirSlot[i]);

			join_addr.imr_multiaddr.s_addr = inet_addr(ip);
			join_addr.imr_interface.s_addr = htonl(INADDR_ANY);

			status = setsockopt(Handle,IPPROTO_IP,IP_DROP_MEMBERSHIP,(void*)&join_addr,sizeof(join_addr));
			(void)local_NET_CloseConnection(sVaNetMultirSlot[i].dwStbNetHandle);
			if (acMultiRecvBuff[i] != NULL)
				VA_MEM_Free(acMultiRecvBuff[i]);
			acMultiRecvBuff[i] = NULL;
			if (acMultiStoreBuff[i] != NULL)
				VA_MEM_Free(acMultiStoreBuff[i]);
			acMultiStoreBuff[i] = NULL;
			count--;
			break;
		}
	}
	if (i == kVA_SETUP_MAX_MULTICAST_SOCKETS)
		return kVA_INVALID_PARAMETER;

	for (i=0; i<kVA_SETUP_MAX_MULTICAST_SOCKETS; i++)
	{
		if (sVaNetMultirSlot[i].dwStbNetHandle != VA_INVALID_HANDLE)
			break;
	}
	if ((i == kVA_SETUP_MAX_MULTICAST_SOCKETS) && sem_count)
	{
		sem_count=0;
		VA_SEM_Release(g_ulVaNetSemId);
	}

	if (count == 0)
		sleep(1);

	HxLOG_Print("[Exit]\n");

	return kVA_OK;
}

