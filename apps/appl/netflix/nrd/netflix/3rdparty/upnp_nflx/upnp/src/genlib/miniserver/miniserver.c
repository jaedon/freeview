/**************************************************************************
 *
 * Copyright (c) 2000-2003 Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * - Neither name of Intel Corporation nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **************************************************************************/

#include "upnp_nflx_config.h"

/*!
 * \file
 *
 * \brief Implements the functionality and utility functions
 * used by the Miniserver module.
 *
 * The miniserver is a central point for processing all network requests.
 * It is made of:
 *   - The SSDP sockets for discovery.
 *   - The HTTP listeners for description / control / eventing.
 *
 */

#include "miniserver.h"

#include "ithread.h"
#include "ssdplib.h"
#include "statcodes.h"
#include "ThreadPool.h"
#include "unixutil.h" /* for socklen_t, EAFNOSUPPORT */
#include "upnpapi.h"
#include "upnputil.h"
#include "upnpdebug.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define STOP_SOCKET_PORT 40003

/*! . */

struct mserv_request_t {
    /*! Connection handle. */
    SOCKET connfd;
    /*! . */
    struct sockaddr_storage foreign_sockaddr;
};

/*! . */
typedef enum {
    /*! . */
    MSERV_IDLE,
    /*! . */
    MSERV_RUNNING,
    /*! . */
    MSERV_STOPPING
} MiniServerState;

/*! . */
uint16_t miniStopSockPort;

/*!
 * module vars
 */
static MiniServerState gMServState = MSERV_IDLE;

static UPNP_INLINE void fdset_if_valid(SOCKET sock, fd_set *set)
{
    if (sock != INVALID_SOCKET) {
        FD_SET(sock, set);
    }
}

static void ssdp_read(SOCKET rsock, fd_set *set)
{
    if (rsock != INVALID_SOCKET && FD_ISSET(rsock, set)) {
        readFromSSDPSocket(rsock);
    }
}

static int receive_from_stopSock(SOCKET ssock, fd_set *set)
{
    ssize_t byteReceived;
    socklen_t clientLen;
    struct sockaddr_storage clientAddr;
    char requestBuf[256];
    char buf_ntop[64];

    if (FD_ISSET(ssock, set)) {
        clientLen = sizeof(clientAddr);
        memset((char *)&clientAddr, 0, sizeof(clientAddr));
        byteReceived = recvfrom(ssock, requestBuf,
            25, 0, (struct sockaddr *)&clientAddr, &clientLen);
        if (byteReceived > 0) {
            requestBuf[byteReceived] = '\0';
            inet_ntop(AF_INET,
                &((struct sockaddr_in*)&clientAddr)->sin_addr,
                buf_ntop, sizeof(buf_ntop));
            UpnpPrintf( UPNP_INFO, MSERV, __FILE__, __LINE__,
                "Received response: %s From host %s \n",
                requestBuf, buf_ntop );
            UpnpPrintf( UPNP_PACKET, MSERV, __FILE__, __LINE__,
                "Received multicast packet: \n %s\n",
                requestBuf);
            if (NULL != strstr(requestBuf, "ShutDown")) {
                return 1;
            }
        }
    }

    return 0;
}

/*!
 * \brief Run the miniserver.
 *
 * The MiniServer accepts a new request and schedules a thread to handle the
 * new request. Checks for socket state and invokes appropriate read and
 * shutdown actions for the Miniserver and SSDP sockets.
 */
static void RunMiniServer(
    /*! [in] Socket Array. */
    MiniServerSockArray *miniSock)
{
    char errorBuffer[ERROR_BUFFER_LEN];
    fd_set expSet;
    fd_set rdSet;
    SOCKET maxMiniSock;
    int ret = 0;
    SOCKET stopSock = 0;

    maxMiniSock = 0;
    if (miniSock->miniServerStopSock != INVALID_SOCKET)
        maxMiniSock = max(maxMiniSock, miniSock->miniServerStopSock);
    if (miniSock->ssdpSock4 != INVALID_SOCKET)
        maxMiniSock = max(maxMiniSock, miniSock->ssdpSock4);
    if (miniSock->ssdpSock6 != INVALID_SOCKET)
        maxMiniSock = max(maxMiniSock, miniSock->ssdpSock6);
    if (miniSock->ssdpSock6UlaGua != INVALID_SOCKET)
        maxMiniSock = max(maxMiniSock, miniSock->ssdpSock6UlaGua);
#ifdef INCLUDE_CLIENT_APIS
    if (miniSock->ssdpReqSock4 != INVALID_SOCKET)
        maxMiniSock = max(maxMiniSock, miniSock->ssdpReqSock4);
    if (miniSock->ssdpReqSock6 != INVALID_SOCKET)
        maxMiniSock = max(maxMiniSock, miniSock->ssdpReqSock6);
#endif /* INCLUDE_CLIENT_APIS */
    ++maxMiniSock;

    gMServState = MSERV_RUNNING;
    while (!stopSock) {
        FD_ZERO(&rdSet);
        FD_ZERO(&expSet);
        /* FD_SET()'s */
        FD_SET(miniSock->miniServerStopSock, &expSet);
        FD_SET(miniSock->miniServerStopSock, &rdSet);
        fdset_if_valid(miniSock->ssdpSock4, &rdSet);
        fdset_if_valid(miniSock->ssdpSock6, &rdSet);
        fdset_if_valid(miniSock->ssdpSock6UlaGua, &rdSet);
#ifdef INCLUDE_CLIENT_APIS
        fdset_if_valid(miniSock->ssdpReqSock4, &rdSet);
        fdset_if_valid(miniSock->ssdpReqSock6, &rdSet);
#endif /* INCLUDE_CLIENT_APIS */
        /* select() */
        ret = select((int) maxMiniSock, &rdSet, NULL, &expSet, NULL);
        if (ret == SOCKET_ERROR && errno == EINTR) {
            continue;
        }
        if (ret == SOCKET_ERROR) {
            (void)strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
            UpnpPrintf(UPNP_CRITICAL, SSDP, __FILE__, __LINE__,
                "Error in select(): %s\n", errorBuffer);
            continue;
        } else {
#ifdef INCLUDE_CLIENT_APIS
            ssdp_read(miniSock->ssdpReqSock4, &rdSet);
            ssdp_read(miniSock->ssdpReqSock6, &rdSet);
#endif /* INCLUDE_CLIENT_APIS */
            ssdp_read(miniSock->ssdpSock4, &rdSet);
            ssdp_read(miniSock->ssdpSock6, &rdSet);
            ssdp_read(miniSock->ssdpSock6UlaGua, &rdSet);
            stopSock = receive_from_stopSock(
                miniSock->miniServerStopSock, &rdSet);
        }
    }
    /* Close all sockets. */
    sock_close(miniSock->miniServerStopSock);
    sock_close(miniSock->ssdpSock4);
    sock_close(miniSock->ssdpSock6);
    sock_close(miniSock->ssdpSock6UlaGua);
#ifdef INCLUDE_CLIENT_APIS
    sock_close(miniSock->ssdpReqSock4);
    sock_close(miniSock->ssdpReqSock6);
#endif /* INCLUDE_CLIENT_APIS */
    /* Free minisock. */
    free(miniSock);
    gMServState = MSERV_IDLE;

    return;
}

/*!
 * \brief Returns port to which socket, sockfd, is bound.
 *
 * \return -1 on error; check errno. 0 if successfull.
 */
static int get_port(
    /*! [in] Socket descriptor. */
    SOCKET sockfd,
    /*! [out] The port value if successful, otherwise, untouched. */
    uint16_t *port)
{
    struct sockaddr_storage sockinfo;
    socklen_t len;
    int code;

    len = sizeof(sockinfo);
    code = getsockname(sockfd, (struct sockaddr *)&sockinfo, &len);
    if (code == -1) {
        return -1;
    }
    if (sockinfo.ss_family == AF_INET) {
        *port = ntohs(((struct sockaddr_in*)&sockinfo)->sin_port);
    } else if(sockinfo.ss_family == AF_INET6) {
        *port = ntohs(((struct sockaddr_in6*)&sockinfo)->sin6_port);
    }
    UpnpPrintf(UPNP_INFO, MSERV, __FILE__, __LINE__,
        "sockfd = %d, .... port = %u\n", sockfd, *port);

    return 0;
}

/*!
 * \brief Creates the miniserver STOP socket. This socket is created and
 *  listened on to know when it is time to stop the Miniserver.
 *
 * \return
 * \li \c UPNP_E_OUTOF_SOCKET: Failed to create a socket.
 * \li \c UPNP_E_SOCKET_BIND: Bind() failed.
 * \li \c UPNP_E_INTERNAL_ERROR: Port returned by the socket layer is < 0.
 * \li \c UPNP_E_SUCCESS: Success.
 */
static int get_miniserver_stopsock(
    /*! [in] Miniserver Socket Array. */
    MiniServerSockArray *out)
{
    char errorBuffer[ERROR_BUFFER_LEN];
    struct sockaddr_in stop_sockaddr;
    SOCKET miniServerStopSock = 0;
    int ret = 0;

    miniServerStopSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (miniServerStopSock == INVALID_SOCKET) {
        (void)strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
        UpnpPrintf(UPNP_CRITICAL, MSERV, __FILE__, __LINE__,
            "Error in socket(): %s\n", errorBuffer);
        return UPNP_E_OUTOF_SOCKET;
    }
    /* Bind to local socket. */
    memset(&stop_sockaddr, 0, sizeof (stop_sockaddr));
    stop_sockaddr.sin_family = AF_INET;
    stop_sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    stop_sockaddr.sin_port = htons(STOP_SOCKET_PORT);
    ret = bind(miniServerStopSock, (struct sockaddr *)&stop_sockaddr,
        sizeof(stop_sockaddr));
    if (ret == SOCKET_ERROR) {
        UpnpPrintf(UPNP_CRITICAL,
        MSERV, __FILE__, __LINE__,
            "Error in binding localhost!!!\n");
        sock_close(miniServerStopSock);
        return UPNP_E_SOCKET_BIND;
    }
    ret = get_port(miniServerStopSock, &miniStopSockPort);
    if (ret < 0) {
        sock_close(miniServerStopSock);
        return UPNP_E_INTERNAL_ERROR;
    }
    out->miniServerStopSock = miniServerStopSock;
    out->stopPort = miniStopSockPort;

    return UPNP_E_SUCCESS;
}

static UPNP_INLINE void InitMiniServerSockArray(MiniServerSockArray *miniSocket)
{
    miniSocket->miniServerStopSock = INVALID_SOCKET;
    miniSocket->ssdpSock4 = INVALID_SOCKET;
    miniSocket->ssdpSock6 = INVALID_SOCKET;
    miniSocket->ssdpSock6UlaGua = INVALID_SOCKET;
    miniSocket->stopPort = 0;
    miniSocket->miniServerPort4 = 0;
    miniSocket->miniServerPort6 = 0;
#ifdef INCLUDE_CLIENT_APIS
    miniSocket->ssdpReqSock4 = INVALID_SOCKET;
    miniSocket->ssdpReqSock6 = INVALID_SOCKET;
#endif /* INCLUDE_CLIENT_APIS */
}

int StartMiniServer(unsigned short advtPort, char advtTTL)
{
    int ret_code;
    int count;
    int max_count = 10000;
    MiniServerSockArray *miniSocket;
    ThreadPoolJob job;

    if (gMServState != MSERV_IDLE) {
        /* miniserver running. */
        return UPNP_E_INTERNAL_ERROR;
    }
    miniSocket = (MiniServerSockArray *)malloc(
        sizeof (MiniServerSockArray));
    if (!miniSocket) {
        return UPNP_E_OUTOF_MEMORY;
    }
    InitMiniServerSockArray(miniSocket);
    /* Stop socket (To end miniserver processing). */
    ret_code = get_miniserver_stopsock(miniSocket);
    if (ret_code != UPNP_E_SUCCESS) {
        free(miniSocket);
        return ret_code;
    }

    gSSDP_PORT = advtPort ? advtPort : SSDP_DEFAULT_PORT;
    gSSDP_TTL = advtTTL ? advtTTL : SSDP_DEFAULT_TTL;

    /* SSDP socket for discovery/advertising. */
    ret_code = get_ssdp_sockets(miniSocket);
    if (ret_code != UPNP_E_SUCCESS) {
        sock_close(miniSocket->miniServerStopSock);
        free(miniSocket);
        return ret_code;
    }
    TPJobInit(&job, (start_routine)RunMiniServer, (void *)miniSocket);
    TPJobSetPriority(&job, MED_PRIORITY);
    TPJobSetFreeFunction(&job, (free_routine)free);
    ret_code = ThreadPoolAddPersistent(&gMiniServerThreadPool, &job, NULL);
    if (ret_code < 0) {
        sock_close(miniSocket->miniServerStopSock);
        sock_close(miniSocket->ssdpSock4);
        sock_close(miniSocket->ssdpSock6);
        sock_close(miniSocket->ssdpSock6UlaGua);
#ifdef INCLUDE_CLIENT_APIS
        sock_close(miniSocket->ssdpReqSock4);
        sock_close(miniSocket->ssdpReqSock6);
#endif /* INCLUDE_CLIENT_APIS */
        return UPNP_E_OUTOF_MEMORY;
    }
    /* Wait for miniserver to start. */
    count = 0;
    while (gMServState != MSERV_RUNNING && count < max_count) {
        /* 0.05s */
        usleep(50 * 1000);
        count++;
    }
    if (count >= max_count) {
        /* Took it too long to start that thread. */
        sock_close(miniSocket->miniServerStopSock);
        sock_close(miniSocket->ssdpSock4);
        sock_close(miniSocket->ssdpSock6);
        sock_close(miniSocket->ssdpSock6UlaGua);
#ifdef INCLUDE_CLIENT_APIS
        sock_close(miniSocket->ssdpReqSock4);
        sock_close(miniSocket->ssdpReqSock6);
#endif /* INCLUDE_CLIENT_APIS */
        return UPNP_E_INTERNAL_ERROR;
    }

    return UPNP_E_SUCCESS;
}

int StopMiniServer()
{
    char errorBuffer[ERROR_BUFFER_LEN];
    socklen_t socklen = sizeof (struct sockaddr_in);
    SOCKET sock;
    struct sockaddr_in ssdpAddr;
    char buf[256] = "ShutDown";
    size_t bufLen = strlen(buf);

    if(gMServState == MSERV_RUNNING) {
        gMServState = MSERV_STOPPING;
    } else {
        return 0;
    }
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        (void)strerror_r(errno, errorBuffer, ERROR_BUFFER_LEN);
        UpnpPrintf(UPNP_INFO, SSDP, __FILE__, __LINE__,
            "SSDP_SERVER: StopSSDPServer: Error in socket() %s\n",
            errorBuffer);
        return 0;
    }
    while(gMServState != MSERV_IDLE) {
        ssdpAddr.sin_family = AF_INET;
        ssdpAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        ssdpAddr.sin_port = htons(miniStopSockPort);
        sendto(sock, buf, bufLen, 0,
            (struct sockaddr *)&ssdpAddr, socklen);
        usleep(1000);
        if (gMServState == MSERV_IDLE) {
            break;
        }
        usleep(1000);
    }
    sock_close(sock);

    return 0;
}
