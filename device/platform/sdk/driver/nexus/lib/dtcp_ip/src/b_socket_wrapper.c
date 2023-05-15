/********************************************************************************************
*     (c)2004-2012 Broadcom Corporation                                                     *
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
 * $brcm_Workfile: b_socket_wrapper.c $
 * $brcm_Revision: 7 $
 * $brcm_Date: 4/17/12 4:48p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/lib/dtcp_ip/src/b_socket_wrapper.c $
 * 
 * 7   4/17/12 4:48p leisun
 * SW7425-992: Link warning: gethostbyname is obsolescent
 * 
 * 6   9/14/09 10:55a leisun
 * SW7405-3013: Coverity defects report fix
 * 
 * 5   9/11/09 11:42a leisun
 * SW7405-3013: coverity defect report fix
 * 
 * 4   7/13/09 11:19a leisun
 * PR 55693: Re-design interfaces, and other cleanups
 * 
 * 3   6/30/09 10:43p leisun
 * PR 55693: Added exchange key expiration timer, and other fixes
 * 
 * 2   6/25/09 9:57a ssood
 * PR53773: Added socket option to reuse port if socket is in TCP TIMEWAIT
 *  state
 * 
 * 1   6/5/09 4:07p leisun
 * PR 55693: checking in DTCP-IP lib
*
* $Source:$
* $Revision:$
* $Date:$
* $Author:$
*
* Description:
*    Socket wrapper functions.
*
* Revision History:
* $Revision:$ $Date:$ $Author:$ Initial creation.
* 
*********************************************************************************************/
/*! \file b_socket_wrapper.c
 *  \brief implement TCP socket wrapper functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#include "bstd.h"
#include "bdbg.h"
#include "b_os_lib.h"
#include "b_dtcp_applib.h"
#include "b_dtcp_ake.h" 
#include "b_dtcp_ip_constants.h"
#include "b_dtcp_status_codes.h"
#include "b_dtcp_transport.h"
#include "b_dtcp_ip_transport_priv.h"

BDBG_MODULE(b_dtcp_ip);

static BERR_Code B_VerifySocketParams(int sfd);

/*! \brief Open a TCP socket.
 *
 *  Assuming AF_INET, SOCK_STREAM and IPPROTO_TCP, the socket obtained is for AKE session only. 
 *  \ret socket file descriptor.
 */
int B_SocketOpen(void)
{
	int sfd;
	int ttl = DTCP_TTL;

	sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sfd > 0)
	{
		if(setsockopt(sfd, IPPROTO_IP, IP_TTL, (char *)&ttl, sizeof(ttl)) < 0 )
		{
			BDBG_ERR(("%s: Unable to set socket TTL to %d\n", __FUNCTION__, DTCP_TTL));
		}
	}
	return sfd;
}
/*! \brief Close a TCP socket
 *  \param[in] sfd socket file descriptor.
 */
int B_SocketClose(int sfd)
{
	if(sfd > 0)
		return (close(sfd));

	return -EINVAL;
}
/*! \brief listen on TCP socket.
 *  \param[in] sfd socket file descriptor.
 */
BERR_Code B_SocketListen(int sfd)
{
	if(listen(sfd, 0) == 0)
		return BERR_SUCCESS;
	else
		return BERR_SOCKET_ERROR;
}
/*! \brief Accept connection from remote host (server only).
 *  \param[in] sfd, socket handle.
 *  \param[in,out] RemoteAddr connected remote host address.
 *  \return connected socket handle.
 */
int B_SocketAccept(int sfd, char * RemoteAddr, int size)
{
	int afd;
	struct sockaddr_in c_addr;
	size_t addr_size = sizeof(struct sockaddr);

	afd = accept(sfd, (struct sockaddr *)&c_addr, &addr_size);

	if(afd > 0)
	{
		struct in_addr addr;
		addr.s_addr = c_addr.sin_addr.s_addr;
		strncpy(RemoteAddr, inet_ntoa(addr), size);
	}
	return afd;
}
/*! \brief initiate connection to remote host (client only)
 *  \param[in] sfd socket handle.
 *  \param[in] RemoteAddr remote host's IP address.
 *  \param[in[ port port number.
 */
BERR_Code B_SocketConnect(int sfd, char * RemoteAddr, int port)
{
	struct sockaddr_in addr;

	BDBG_ENTER(B_SocketConnect);
	memset(&addr, 0, sizeof(addr));
	if(inet_aton(RemoteAddr, &addr.sin_addr) == 0)
		return BERR_INVALID_PARAMETER;

	addr.sin_family = AF_INET;
	addr.sin_port = htons((short)port);

	if(connect(sfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		BDBG_ERR(("%s: connect to %s:%d faild\n", __FUNCTION__, RemoteAddr, port));
		return BERR_SOCKET_ERROR;
	}
	BDBG_LEAVE(B_SocketConnect);
	return BERR_SUCCESS;
}
/*! \brief bind a socket to TCP port and ip address(Server only)
 *  \param[in] sfd socket handle.
 *  \param[in] LocalAddr local ip address, if it's NULL, then bind to INADDR_ANY
 *  \param[in] port local port number
 */
BERR_Code B_SocketBind(int sfd, char * LocalAddr, int port)
{
	struct sockaddr_in addr;
    int reuse_flag = 1;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	if (port == 0) return BERR_INVALID_PARAMETER;
	
	if (LocalAddr == NULL)
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	else if (inet_aton(LocalAddr, &addr.sin_addr))
		return BERR_INVALID_PARAMETER;
	
	addr.sin_port = htons((short)port);

    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse_flag, sizeof(reuse_flag) ) < 0 ) {
        printf("REUSE Socket Error\n");
        return -EINVAL;
    }

	if(bind(sfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		BDBG_ERR(("%s: Failed to bind to socket on %s:%d\n", 
					__FUNCTION__, LocalAddr, port));
		return BERR_SOCKET_ERROR;
	}
	return BERR_SUCCESS;
}
/*! \brief wait for data ready.
 * 
 *  This function only set the vent when data is ready, to wake up other waiting thread.
 *  \param[in] sfd socket handle.
 *  \param[in] Terminate a flag to indicate the terminate the loop.
 *  \param[in,out] type of the occured event.
 *  \param[in] hEvent handle to a event to set when data is ready.
 *  \param[in] hMutex handle to a mutex to lock/unlock when modifing shared data, in this case the "size" parameter.
 *  \param[out] the size of the data that is ready to read. 
 *  \param[in] Timeout timeout value in seconds.
 */
BERR_Code B_SocketWaitForData(
		int sfd, 
		B_TransportEvent_T * Event,
		B_EventHandle hEvent,
		B_EventHandle hBufCleared,
		B_MutexHandle hMutex,
		int * size,
		int Timeout)
{
	int ret, lsize;
	fd_set rfds, wfds;
	struct timeval tv;
	BERR_Code retValue = BERR_SUCCESS;

	/* We are not checking the writable event */
	(void)wfds;

	FD_ZERO(&rfds);

	tv.tv_sec = Timeout;
	tv.tv_usec = 0;

	BDBG_ENTER(B_SocketWaitForData);
	/* Signal upper layer that we are ready to receive data. */
	*Event = B_Transport_eReady;
	B_Event_Set(hEvent);
	while(true)
	{
		FD_SET(sfd, &rfds);
		if((ret = select(sfd + 1, &rfds, NULL, NULL, &tv)) < 0)
		{
			BDBG_ERR(("%s: Select returned %d  exsiting..\n", __FUNCTION__, ret));
			B_Mutex_Lock(hMutex);
			*Event = B_Transport_eError;
			B_Mutex_Unlock(hMutex);
			B_Event_Set(hEvent);
			retValue = BERR_SOCKET_ERROR;
			break;
		}else if (ret == 0)
		{
			/* Timed out. */
			tv.tv_sec = Timeout;
			tv.tv_usec = 0;
			continue;
		}else if (ret == EINTR)
		{
			BDBG_MSG(("%s is interrupted, continue...\n", __FUNCTION__));
			continue;
		}else if(FD_ISSET(sfd, &rfds))
		{
			BDBG_MSG(("Select returned %d \n", ret)); 
			if ((ret = ioctl(sfd, FIONREAD, &lsize)) < 0) {
				BDBG_ERR(("IOCTL returned %d\n", ret));
				B_Mutex_Lock(hMutex);
				*Event = B_Transport_eError;
				B_Mutex_Unlock(hMutex);
				B_Event_Set(hEvent);
				retValue= BERR_SOCKET_ERROR;
				break;
			}else {
				B_Mutex_Lock(hMutex);
				*size = lsize;
				if(lsize == 0) {
					*Event = B_Transport_eClosed;
					B_Mutex_Unlock(hMutex);
					B_Event_Set(hEvent);
					break;
				}else {
					*Event = B_Transport_eDataReady;
					B_Event_Reset(hBufCleared);
				}
				B_Mutex_Unlock(hMutex);
				BDBG_MSG(("Setting DataReady event size=%d\n", lsize)); 
				B_Event_Set(hEvent);
			}
			/* Wait for upper layer to read data out of buffer.*/
			if( lsize > 0)
			{
				B_Event_Wait(hBufCleared, Timeout);
			}
		}else {
			BDBG_ERR(("Unknown select event: %d\n", ret));
		}
	}
	BDBG_LEAVE(B_SocketWaitForData);
	return retValue;
}
#if 0
BERR_Code B_SocketWaitForData( int maxfd, B_DTCP_IP_TransportData_T * pProtocol, int Timeout)
{
	int ret;
	fd_set rfds, wfds, efds;
	struct timeval tv;
	B_DTCP_IP_Connection_T * Conn;
	BERR_Code retValue = BERR_SUCCESS;

	/* We are not checking the writable event */
	(void)wfds;

	FD_ZERO(&rfds);

	tv.tv_sec = Timeout;
	tv.tv_usec = 0;

	BDBG_ENTER(B_SocketWaitForData);
	/* Signal upper layer that we are ready to receive data. */
	Conn = BLST_S_FIRST(&(pProtocol->Connection_list));
	if(Conn) {
		Conn->EventType = B_Transport_eReady;
		B_Event_Set(Conn->hNetEvent);
	}
	while(pProtocol->TerminateReader == false)
	{
		Conn = BLST_S_FIRST(&(pProtocol->Connection_list));
		while(Conn != NULL ) {
			FD_SET(Conn->ConnectSocket, &rfds);
			FD_SET(Conn->ConnectSocket, &efds);
			Conn = BLST_S_NEXT(Conn, node);
		}
		if((ret = select(maxfd + 1, &rfds, NULL, &efds, &tv)) < 0)
		{
			Conn = BLST_S_FIRST(&(pProtocol->Connection_list));
			while(Conn != NULL) {
				if(FD_ISSET(Conn->ConnectSocket, &efds)) {
					BDBG_ERR(("%s: Select returned %d  For socket: %d \n", __FUNCTION__, ret, Conn->ConnectSocket));
					B_Mutex_Lock(Conn->hMutex);
					Conn->EventType = B_Transport_eError;
					B_Mutex_Unlock(Conn->hMutex);
					B_Event_Set(Conn->hNetEvent);
				}
				Conn = BLST_S_NEXT(Conn, node);
			}
		}else if (ret == 0)
		{
			/* Timed out. */
			tv.tv_sec = Timeout;
			tv.tv_usec = 0;
			continue;
		}else if (ret == EINTR)
		{
			BDBG_MSG(("%s is interrupted, continue...\n", __FUNCTION__));
			continue;
		}else 
		{
			Conn = BLST_S_FIRST(&(pProtocol->Connection_list));
			while(Conn != NULL) {
				if(FD_ISSET(Conn->ConnectSocket, &rfds)) {
					B_Mutex_Lock(Conn->hMutex);
					/*BDBG_MSG(("%s: Select returned %d  For socket: %d \n", __FUNCTION__, ret, Conn->ConnectSocket)); */
					if ((ret = ioctl(Conn->ConnectSocket, FIONREAD, &Conn->DataSize)) < 0) {
						BDBG_ERR(("IOCTL returned %d\n", ret));
						Conn->EventType = B_Transport_eError;
					}else if(Conn->DataSize == 0){
						Conn->EventType = B_Transport_eClosed;
					}else {
						Conn->EventType = B_Transport_eDataReady;
					}
					if(Conn->bufRead == true && Conn->DataSize > 0) {	
						BDBG_MSG(("Setting DataReady event For sock %d:  size=%d\n",Conn->ConnectSocket, Conn->DataSize )); 
						B_Event_Set(Conn->hNetEvent);
						Conn->bufRead = false;
					}
					B_Mutex_Unlock(Conn->hMutex);
				}
				Conn = BLST_S_NEXT(Conn, node);
			}
		}
		FD_ZERO(&rfds);
		FD_ZERO(&efds);
	}
	BDBG_LEAVE(B_SocketWaitForData);
	return retValue;
}
#endif
/*! \brief read data from socket.
 *  \param[in] sfd socket file descriptor.
 *  \param[in,out] buffer data buffer.
 *  \param[in] length buffer length.
 */
int B_SocketRecvData( int sfd, unsigned char * buffer, int length)
{
	int ret;
	BDBG_ASSERT(buffer);

	BDBG_ENTER(B_SocketRecvData);
	if (B_VerifySocketParams(sfd) == BERR_SUCCESS)
	{
		ret = recv(sfd, buffer, length, 0); 
	} else {
		ret = BERR_SOCKET_ERROR;
		BDBG_ERR(("Incoming TTL incorrect"));
	}
	BDBG_LEAVE(B_SocketRecvData);
	return ret;
}

/*! \brief wrapper function to send data out through connected TCP socket.
 *  
 *  \param[in] sfd socket file desriptor.
 *  \param[in] buffer pointer to data buffer.
 *  \param[in] length length of the data buffer.
 *  \ret length of the data sent, or error code if error occured.
 */
int B_SocketSendData(int sfd, unsigned char * buffer, int length)
{
	int retValue = 0;

	BDBG_ASSERT(buffer);

	retValue = send(sfd, buffer, length, 0);

	if( retValue < 0)
	{
		BDBG_ERR(("%d sending message \n", errno));
	}
	return retValue;
}

/*! \brief verify socket parameter, mainly the incoming TTL value.
 * 
 * \param[in] sfd socket descriptor.
 * \ret BERR_SUCCESS if parameter is ok, or BERR_INVALID_PARAMETER is verification failed.
 */
static BERR_Code B_VerifySocketParams(int sfd)
{
	int option = 0;
	int size = sizeof(option);
	int retValue = BERR_SUCCESS;
	
	BDBG_ENTER(B_VerifySocketParams);
	if (getsockopt(sfd, IPPROTO_IP, IP_TTL, (char*)&option, (socklen_t *)&size) < 0)
	{
		BDBG_ERR(("%s getsockopt failed\n", __FUNCTION__));
		retValue = BERR_UNKNOWN;
	}else if (option == DTCP_TTL) {
		retValue = BERR_SUCCESS;
	}else {
		retValue = BERR_INVALID_PARAMETER;
	}

	BDBG_LEAVE(B_VerifySocketParams);
	return retValue;
}
