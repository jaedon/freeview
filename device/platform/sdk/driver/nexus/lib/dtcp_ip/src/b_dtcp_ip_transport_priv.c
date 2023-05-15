/********************************************************************************************
*     (c)2004-2011 Broadcom Corporation                                                     *
*                                                                                           *
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,   *
*  and may only be used, duplicated, modified or distributed pursuant to the terms and      *
*  conditions of a separate, written license agreement executed between you and Broadcom    *
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants*
*  no license (express or implied), right to use, or waiver of any kind with respect to the *
*  Software, and Broadcom expressly reserves all rights in and to the Software and all      *
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU       *
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY                    *
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.                                 *
*
*  Except as expressly set forth in the Authorized License,                                 *
*
*  1.     This program, including its structure, sequence and organization, constitutes     *
*  the valuable trade secrets of Broadcom, and you shall use all reasonable efforts to      *
*  protect the confidentiality thereof,and to use this information only in connection       *
*  with your use of Broadcom integrated circuit products.                                   *
*                                                                                           *
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"          *
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR                   *
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO            *
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES            *
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,            *
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION             *
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF              *
*  USE OR PERFORMANCE OF THE SOFTWARE.                                                      *
*                                                                                           *
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS         *
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR             *
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR               *
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF             *
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT              *
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE            *
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF              *
*  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: b_dtcp_ip_transport_priv.c $
 * $brcm_Revision: 6 $
 * $brcm_Date: 11/30/11 6:08p $
 * 
 * Module Description:
 *     DTCP-IP AKE connection handler.
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/lib/dtcp_ip/src/b_dtcp_ip_transport_priv.c $
 * 
 * 6   11/30/11 6:08p leisun
 * SWSECURITY-89: CKC fix from panasonic
 * 
 * 5   10/12/09 3:55p leisun
 * SW7405-3138: Fix for HTTP live streaming, connection handling
 * 
 * 4   9/11/09 11:42a leisun
 * SW7405-3013: coverity defect report fix
 * 
 * 3   7/13/09 11:19a leisun
 * PR 55693: Re-design interfaces, and other cleanups
 * 
 * 2   6/30/09 10:43p leisun
 * PR 55693: Added exchange key expiration timer, and other fixes
 * 
 * 1   6/5/09 4:07p leisun
 * PR 55693: checking in DTCP-IP lib
*********************************************************************************************/
/*! \file b_dtcp_ake_msg.c
 *  \brief implement DTCP-IP connection handler functions.
 */
#include <stdio.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "bdbg.h"
#include "b_dtcp_applib.h"
#include "b_dtcp_ip_ake.h"
#include "b_dtcp_status_codes.h"
#include "b_dtcp_transport.h"
#include "b_dtcp_ip_transport_priv.h"
#include "b_dtcp_stack.h"
#include "b_dtcp_ip_stack.h"

BDBG_MODULE(b_dtcp_ip);

#define DTCP_READER_TIMEOUT		1		/*!< This refers to "select()" timeout value */

struct __b_connection_thread_param
{
	B_DTCP_IP_Connection_T * Conn;
	B_DTCP_IP_TransportData_T pProtocol;
};
/*! \brief utility to get the event type.
 *
 * \param[in] Conn connection context pointer.
 * \retval one of the enum values defined in B_TransportEvent_T 
 */
B_TransportEvent_T B_DTCP_IP_GetEventType_Ex(B_DTCP_IP_Connection_T * Conn)
{
	B_TransportEvent_T Event;
	B_Mutex_Lock(Conn->hMutex);
	Event = Conn->EventType;
	B_Mutex_Unlock(Conn->hMutex);

	return Event;
}
/*! \brief utility to get data size available to read.
 *
 * \param[in] Conn connection context pointer.
 * \retval data size available for read.
 */
int B_DTCP_IP_GetDataSize_Ex(B_DTCP_IP_Connection_T * Conn)
{
	int size;
	B_Mutex_Lock(Conn->hMutex);
	size = Conn->DataSize;
	B_Mutex_Unlock(Conn->hMutex);
	return size;
}
/*! \brief a reader thread function.
 *  \param[in] UserData user data pointer, points to a connection context.
 *
 *  When the connection is initialized and established, both the server and 
 *  the client will spawn this reader thread, the main function of this thread 
 *  is to wait for data ready to be read, set the event to wake up other 
 *  thread who is waiting.
 */
static void B_DTCP_IP_ReaderThreadFunc(void * UserData)
{
	B_DTCP_IP_Connection_T * Conn = UserData;
	BERR_Code retValue = BERR_SUCCESS;
	BDBG_ASSERT(UserData);

	BDBG_MSG(("Starting reader thread...."));
	retValue =  B_SocketWaitForData(
			Conn->ConnectSocket,
			&Conn->EventType,
			Conn->hNetEvent,
			Conn->hBufCleared,
			Conn->hMutex,
			&Conn->DataSize,
			DTCP_READER_TIMEOUT
	);
}
/*! \brief helper function to spwan connection thread and call user supplied new connection function.
 *  \param[in] UserData user data pointer, points to a connection context.
 *  \retval none.
 */
static void B_DTCP_IP_ConnThreadFunc(void * UserData)
{
	B_DTCP_IP_Connection_T * Conn = (B_DTCP_IP_Connection_T *)UserData;
	B_DTCP_IP_TransportData_T * pProtocol = (B_DTCP_IP_TransportData_T *)(Conn->reserved);
	struct __dtcp_ip_stack_data sdata;
	int retValue = BERR_SUCCESS;
	
	BDBG_ASSERT(Conn);
	BDBG_ASSERT(pProtocol);
	BDBG_ENTER(B_DTCP_IP_ConnThreadFunc);
	
	BKNI_Memcpy(sdata.RemoteIp, Conn->RemoteAddr, MAX_IP_ADDR_SIZE);
	BKNI_Memcpy(sdata.LocalIp, Conn->LocalAddr, MAX_IP_ADDR_SIZE);
	sdata.LocalPort = Conn->Port;
	sdata.RemotePort = 0; /*TODO */
	sdata.ConnectSocket = Conn->ConnectSocket;
	BDBG_MSG(("Starting new connection thread...."));
	/*
	 * call the stack's new connection callback, pass in the socket id 
	 * we use this as AKE session id.
	 */
	retValue = Conn->OnNewConnection(pProtocol->pStack, (void*)&sdata);
	BDBG_MSG(("Callback terminated with : %d\n", retValue));

	/*
	 * If stack layer returned us a error code, and it's not a socket error,
	 * it means AKE failed so we close connection before terminating this thread.
	 */
	if( retValue != BERR_SUCCESS && retValue != BERR_SOCKET_ERROR)
	{
		BDBG_MSG(("Connection thread terminated with %d\n", retValue));
		B_Event_Set(Conn->hConnTerminated);
		retValue = B_DTCP_IP_CloseConnection( Conn, pProtocol );
		if(retValue != BERR_SUCCESS)
		{
			BDBG_WRN(("returned %d when trying to close connection\n"));
		}
		return ;
	}
	/*
	 * Success return, we could fall through here if client issued AKE cancel command.
	 */
	B_Event_Set(Conn->hConnTerminated);

	BDBG_LEAVE(B_DTCP_IP_ConnThreadFunc);
}
/*! \brief create and initialize a connection context.
 *  \param[in] RemoteAddr can be NULL if it's server context.
 *  \param[in] Port port number.
 *  \param[in] Mode connection mode, server/client.
 *  \param[in] ReaderTimeOut Timeout for reader thread.
 *  \param[in] ConnClosedcallback pointer to connection close callback.
 *  \param[in] OnNewConncallback pointer to connection close callback.
 *  \param[in] IsMaster a flag indicate if this is a master connection (server only).
 *  \retval pointer to created connection context or NULL if it failed.
 */
static B_DTCP_IP_Connection_T * B_DTCP_IP_CreateConnection(
		char * RemoteAddr,
		int Port,
		B_ConnectionMode_T Mode,
		int ReaderTimeOut,
		B_DTCP_TransportCallbackFunc_Ptr OnNewConnCallback,
		B_DTCP_TransportCallbackFunc_Ptr OnClosedConnCallback,
		bool IsMaster)
{
	B_DTCP_IP_Connection_T * Conn ;

	BDBG_ENTER(B_DTCP_IP_CreateConnection);

	Conn = (B_DTCP_IP_Connection_T *)BKNI_Malloc(sizeof(B_DTCP_IP_Connection_T));
	
	if (Conn == NULL) {
		BDBG_ERR(("failed to allocate memory for new connection context"));
		return NULL;
	}
	BKNI_Memset(Conn, 0, sizeof(B_DTCP_IP_Connection_T));
	if(IsMaster == false)
		BKNI_Memcpy(Conn->RemoteAddr, RemoteAddr, 32);
	Conn->Port = Port;
	Conn->Mode = Mode;
	Conn->IsMaster = IsMaster;
	Conn->ListenerSocket = INVALID_SOCKET;
	Conn->ConnectSocket = INVALID_SOCKET;
	/*Conn->ReaderTimeout = DTCP_READER_TIMEOUT; */
	(void)ReaderTimeOut;
	Conn->bufRead = true;
	Conn->OnNewConnection = OnNewConnCallback;
	Conn->OnClosedConnection = OnClosedConnCallback;
	
	if((Conn->hNetEvent = B_Event_Create(NULL)) == NULL)
	{
		BDBG_ERR(("Failed to create NetEvent"));
		goto err4;
	}
	if((Conn->hConnTerminated = B_Event_Create(NULL)) == NULL)
	{
		BDBG_ERR(("Failed to create NetEvent"));
		goto err3;
	}
	if((Conn->hBufCleared = B_Event_Create(NULL)) == NULL)
	{
		BDBG_ERR(("Failed to create NetEvent"));
		goto err2;
	}
	if((Conn->hMutex = B_Mutex_Create(NULL)) == NULL)
	{
		BDBG_ERR(("Failed to create mutex for new connection"));
		goto err1;
	}
	
	BDBG_LEAVE(B_DTCP_IP_CreateConnection);
	return Conn;

err1:	/*B_Event_Destroy(Conn->hBufCleared); */
err2:	B_Event_Destroy(Conn->hConnTerminated);
err3:	B_Event_Destroy(Conn->hNetEvent);
err4:	BKNI_Free(Conn);
	return NULL;
}
/*! \brief destroy a connection context, release resource.
 *  \param[in] Conn connection context pointer to be destroyed.
 *  
 *  For server connection, make sure connection thread is terminated before calling this!
 *  For client connection, make sure reader thread is terminited before calling this!
 */
static void B_DTCP_IP_DestroyConnection(B_DTCP_IP_Connection_T * Conn)
{
	if(Conn)
	{
		if(Conn->IsMaster && Conn->ListenerSocket != INVALID_SOCKET)
		{
			BDBG_WRN(("ListenerSocket is not closed when trying to destroy connection"));
		}else if (Conn->ConnectSocket != INVALID_SOCKET) {
			BDBG_WRN(("ConnectSocket is not closed when trying to destroy connection"));
		}
		B_Event_Destroy(Conn->hConnTerminated);
		B_Mutex_Destroy(Conn->hMutex);
		B_Event_Destroy(Conn->hNetEvent);
		B_Event_Destroy(Conn->hBufCleared);
		BKNI_Free(Conn);
	}
}
/*! \brief Wait for connection request, launch user supplied reader thread.
 *  \param[in] master a master connection context pointer.
 */
static BERR_Code B_WaitForConnection(B_DTCP_IP_Connection_T * master, B_DTCP_IP_TransportData_T * pProtocol)
{
	B_DTCP_IP_Connection_T * Conn;

	int ConnectSocket;
	char RemoteAddr[32];
	char ThreadName[64];
	
	BDBG_ENTER(B_WaitForConnection);
	BDBG_ASSERT(master);
	BDBG_ASSERT(pProtocol);

	/* the loop will exit when listener socket is closed. */
	while(true)
	{
		BDBG_MSG(("Waiting for connection...\n"));
		if(B_SocketListen(master->ListenerSocket) != BERR_SUCCESS)
			return BERR_SOCKET_ERROR;
		/* This will block untill remote initiate a connection request.*/
		if((ConnectSocket = B_SocketAccept(master->ListenerSocket, RemoteAddr, 32)) == INVALID_SOCKET)
		{
			BDBG_ERR(("Error when accepting connection\n"));
			return BERR_SOCKET_ERROR;
		}
		BDBG_MSG(("Accepted connection request from %s sfd=%d", RemoteAddr, ConnectSocket));
		Conn = B_DTCP_IP_CreateConnection(
				RemoteAddr,
				master->Port,
				B_ConnectionMode_eServer,
				DTCP_READER_TIMEOUT,
				master->OnNewConnection,			/* OnNewConnCallback */
				master->OnClosedConnection,
				false								/* IsMaster */
				);
		if(Conn == NULL) {
			BDBG_ERR(("Failed to create connection for request from %s\n", RemoteAddr));
			return BERR_UNKNOWN;
		}
		Conn->ConnectSocket = ConnectSocket;
		Conn->reserved = (void*)pProtocol;
		/*
		 * Start a reader thread, the thread function is a internal function, the main 
		 * purpose is to wait for data, and set the event to wake up other waiting thread.
		 * (AKE session thread).
		 */
		snprintf(ThreadName, 64, "Reader:%s", Conn->RemoteAddr);
		Conn->hReaderThread = B_Thread_Create(
				ThreadName,
				B_DTCP_IP_ReaderThreadFunc,
				(void*)Conn,
				NULL);
		if(Conn->hReaderThread == NULL)
		{
			BDBG_ERR(("Failed to create reader thread"));
			B_SocketClose(Conn->ConnectSocket);
			B_DTCP_IP_DestroyConnection(Conn);
			return BERR_OS_ERROR;
		}else {
			/* Wait for reader thread entering wait for data loop */
			B_Event_Wait(Conn->hNetEvent, 3000);
			B_Event_Reset(Conn->hNetEvent);
		}
		BLST_S_INSERT_HEAD(&(pProtocol->Connection_list), Conn, node);
		/*
		 * Start a new connection thread, the thread function should be implmented by 
		 * DTCP AKE component, the main function is to allocate an AKE session and 
		 * waiting for message from sink device.
		 */
		snprintf(ThreadName, 64, "Connection:%s", Conn->RemoteAddr);
		Conn->hConnectionThread = B_Thread_Create(
				ThreadName,
				B_DTCP_IP_ConnThreadFunc,
				(void*)Conn,
				NULL);
		if(Conn->hConnectionThread == NULL) {
			BDBG_ERR(("Failed to create connection thread"));
			B_SocketClose(Conn->ConnectSocket);
			B_Thread_Destroy(Conn->hReaderThread);
			Conn->ConnectSocket = INVALID_SOCKET;
			BLST_S_REMOVE(&(pProtocol->Connection_list), Conn, B_DTCP_IP_Connection, node);
			B_DTCP_IP_DestroyConnection(Conn);
			return BERR_OS_ERROR;
		}
	}
	
	BDBG_LEAVE(B_WaitForConnection);
	return BERR_SUCCESS;
}

/*! \brief Open a DTCP-IP connection, 
 *  For source device, this function will block , waiting for client connection.
 *  For sink devie, this function will return , the pProtocol->MasterConn points to the created connection context.
 * \param[in] Mode server or client mode.
 * \param[in] LocalAddr Local IP address, optional
 * \param[in] RemoteAddr Remote IP address.
 * \param[in] Port TCP port number.
 * \param[in] OnNewConnCallback callback function when there is new connection, only required for server mode.
 * \param[in] OnCloseConneCallback callback function when connection is closed by transport layer.
 * \param[in, out] pProtocol pointer to IP transport protocol specified data.
 * \param[out] ConnectionId, if it's a client connection, ConnectionId is the socket fd.
 */
BERR_Code B_DTCP_IP_OpenConnection(B_ConnectionMode_T Mode,
		char * LocalAddr,
		char * RemoteAddr,
		int Port,
		B_DTCP_TransportCallbackFunc_Ptr OnNewConnCallback,
		B_DTCP_TransportCallbackFunc_Ptr OnClosedConnCallback,
		B_DTCP_IP_TransportData_T * pProtocol,
		int * ConnectionId)
{
	int sfd = INVALID_SOCKET;
	BERR_Code retValue = BERR_SUCCESS;
	B_DTCP_IP_Connection_T * Conn = NULL;
	char ThreadName[32];

	(void)LocalAddr;

	BDBG_ENTER(B_DTCP_IP_OpenConnection);
	BDBG_ASSERT(pProtocol);

	if(Mode == B_ConnectionMode_eServer)
	{
		BDBG_ASSERT(OnNewConnCallback);
		BDBG_MSG(("Openning server connection..."));
		
		sfd = B_SocketOpen();
		if (sfd == INVALID_SOCKET) {
			BDBG_ERR(("Failed to open socket!"));
			return BERR_SOCKET_ERROR;
		}

		/* Create a global master connection context.*/
		Conn = B_DTCP_IP_CreateConnection(
				NULL,						/* Remote Address. */
				Port,						/* TCP port # */
				B_ConnectionMode_eServer,	/* Connection Mode. */
				DTCP_READER_TIMEOUT,		/* Reader timeout in seconds */
				OnNewConnCallback,			/* new connection callback.*/
				OnClosedConnCallback,		/* Close connection callbac */
				true);						/* Is master */
		if(Conn == NULL)
		{
			BDBG_ERR(("Failed to create master connection"));
			B_SocketClose(sfd);
			return BERR_UNKNOWN;
		}
		Conn->ListenerSocket = sfd;
		if(B_SocketBind(Conn->ListenerSocket, NULL, Conn->Port) != BERR_SUCCESS)
		{
			B_SocketClose(sfd);
			Conn->ListenerSocket = INVALID_SOCKET;
			B_DTCP_IP_DestroyConnection(Conn);
			return BERR_SOCKET_ERROR;
		}
		pProtocol->MasterConn = Conn;
		/*
		 * This will block, untill the listener socket is closed. 
		 */
		retValue = B_WaitForConnection(Conn, pProtocol);
		BDBG_MSG(("Exiting from server connection...\n"));

	}else if (Mode == B_ConnectionMode_eClient)
	{
		BDBG_MSG(("Openning client connection...\n"));
		if((sfd = B_SocketOpen()) == INVALID_SOCKET)
		{
			BDBG_ERR(("Failed to open socket!"));
			return BERR_SOCKET_ERROR;
		}
		Conn = B_DTCP_IP_CreateConnection(
				RemoteAddr,
				Port, 
				B_ConnectionMode_eClient,
				DTCP_READER_TIMEOUT,
				NULL,
				OnClosedConnCallback,
				false);
		if(Conn == NULL)
		{
			BDBG_ERR(("Failed to create master connection"));
			B_SocketClose(sfd);
			return BERR_UNKNOWN;
		}
		Conn->ConnectSocket = sfd;
		*ConnectionId = sfd;
		BDBG_MSG(("sfd=%d\n", sfd));
		if((retValue = B_SocketConnect(sfd,RemoteAddr, Port)) != BERR_SUCCESS)
		{
			BDBG_ERR(("Failed to connect to %s:%d\n", RemoteAddr, Port));
			B_SocketClose(sfd);
			Conn->ConnectSocket = INVALID_SOCKET;
			B_DTCP_IP_DestroyConnection(Conn);
			return retValue;
		}
		/*
		 * Start a reader thread, the thread function is a internal function, the main 
		 * purpose is to wait for data, and set the event to wake up other waiting thread.
		 * (AKE session thread).
		 */
		snprintf(ThreadName, 32, "Reader:%s", RemoteAddr);
		
		Conn->hReaderThread = B_Thread_Create(
				ThreadName,
				B_DTCP_IP_ReaderThreadFunc,
				(void*)Conn,
				NULL);
		if(Conn->hReaderThread == NULL)
		{
			BDBG_ERR(("Failed to create reader thread"));
			B_SocketClose(sfd);
			B_DTCP_IP_DestroyConnection(Conn);
			return BERR_OS_ERROR;
		}else {
			/* Wait for reader thread entering wait for data loop */
			B_Event_Wait(Conn->hNetEvent, 3000);
			B_Event_Reset(Conn->hNetEvent);
			BDBG_MSG(("Succesfully Opened client connection"));
			retValue = BERR_SUCCESS;
		}
		BLST_S_INSERT_HEAD(&(pProtocol->Connection_list), Conn, node);
	}

	BDBG_LEAVE(B_DTCP_IP_OpenConnection);
	return retValue;
}
/*! \brief close a opened connection. 
 *	\param[in] Conn connection context pointer.
 *	\param[in] pProtocol pointer to IP transport layer protocol data.
 *	
 *  If it's a master connection, e.g. a listenning session,  all exsiting connections will be closed.
 */
BERR_Code B_DTCP_IP_CloseConnection(B_DTCP_IP_Connection_T * Conn,
		B_DTCP_IP_TransportData_T * pProtocol)
{
	B_DTCP_IP_Connection_T * iter, * tmp;
	BERR_Code retValue = BERR_SUCCESS;
	
	BDBG_ASSERT(Conn);
	BDBG_ENTER(B_DTCP_IP_CloseConnection);
	if(Conn->Mode == B_ConnectionMode_eServer)
	{
		if(Conn->IsMaster == true)
		{
			if((retValue = B_SocketClose(Conn->ListenerSocket) != BERR_SUCCESS))
			{
				BDBG_WRN(("Unable to close listenning socket\n"));
			}
			Conn->ListenerSocket = INVALID_SOCKET;
			/* close all accepted connections*/
			iter = BLST_S_FIRST(&(pProtocol->Connection_list));
			while(iter != NULL){
				if(iter->Mode == B_ConnectionMode_eServer)
				{
					if((retValue = B_SocketClose(iter->ConnectSocket)) != BERR_SUCCESS)
					{
						BDBG_WRN(("Unable to close socket for %s : %d\n", iter->RemoteAddr, retValue));
					}
					iter->ConnectSocket = INVALID_SOCKET;
					/*
					 * After socket closed, reader thread should return with error first,
					 * and then Connection thread (new connection callback, provided by stack layer)
					 * should also return with error.
					 */
					B_Event_Wait(iter->hConnTerminated, 3000); /* wait for 3 seconds */
					B_Thread_Destroy(iter->hReaderThread); 
					B_Thread_Destroy(iter->hConnectionThread);
					
					tmp = iter;
					iter = BLST_S_NEXT(iter, node);
					BLST_S_REMOVE(&(pProtocol->Connection_list), tmp, B_DTCP_IP_Connection, node);
					B_DTCP_IP_DestroyConnection(tmp);
				}else {
					iter = BLST_S_NEXT(iter, node);
				}
			}
			/* Destroy master connection */
			B_DTCP_IP_DestroyConnection(pProtocol->MasterConn);

		}else {
			/* Close a single  accepted connection */
			if((retValue = B_SocketClose(Conn->ConnectSocket)) != BERR_SUCCESS)
			{
				BDBG_WRN(("Unable to close socket for %s : %d\n", Conn->RemoteAddr, retValue));
			}else {
				Conn->ConnectSocket = INVALID_SOCKET;
			}
			B_Event_Wait(Conn->hNetEvent, 3000); /* wait for 3 seconds */
			/*B_Thread_Destroy(Conn->hReaderThread); */
			BLST_S_REMOVE(&(pProtocol->Connection_list), Conn, B_DTCP_IP_Connection, node);
			B_DTCP_IP_DestroyConnection(Conn);

		}
	}else {
		if ((retValue = B_SocketClose(Conn->ConnectSocket)) != BERR_SUCCESS)
		{
			BDBG_WRN(("Unable to close socket for %s: %d ", Conn->RemoteAddr, retValue));
		}
		B_Thread_Destroy(Conn->hReaderThread);
		Conn->ConnectSocket = INVALID_SOCKET;
		/*
		 * Remove from connection list.
		 */
		BLST_S_REMOVE(&(pProtocol->Connection_list), Conn, B_DTCP_IP_Connection, node);
		B_DTCP_IP_DestroyConnection(Conn);
	}
	
	BDBG_LEAVE(B_DTCP_IP_CloseConnection);

	return retValue;
}
