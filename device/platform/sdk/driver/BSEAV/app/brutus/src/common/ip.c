/***************************************************************************
 *     Copyright (c) 2007-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: ip.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 4/22/11 4:47p $
 *
 * Module Description: Helpful functions for IP. Only include this for IP 
 * enabled builds.
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/common/ip.c $
 * 
 * 4   4/22/11 4:47p ssood
 * SW7425-365: new tool chain required changing gethostbyname to
 * getaddrinfo
 * 
 * 3   9/24/07 3:09p rjlewis
 * PR34613: vxworks doesn't like competing defs (lseek) even if they are
 * the same.
 * 
 * 2   9/19/07 12:34p jrubio
 * PR34613: add new recv function
 * 
 * 1   9/17/07 12:41p jrubio
 * PR34613: Helper for ip funcs
 * 
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#ifdef __vxworks
#include <sockLib.h>
#include <hostLib.h>
#else
#include <unistd.h>
#endif

#include "bsettop.h"
#include "bstd.h"
#include "bkni.h"



#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>

BDBG_MODULE(ip_utils);

int tcpConnect(const char * server, int port) 
{
    int sd = -1;
    struct addrinfo hints;
    struct addrinfo *addrInfo = NULL;
    char portString[16];
    struct sockaddr_in localAddr;

    BDBG_MSG(("TCP - Connection to %s:%d ...\n",server,port));

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    /* we dont know the family */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    memset(portString, 0, sizeof(portString));  /* getaddrinfo() requires port # in the string form */
    snprintf(portString, sizeof(portString)-1, "%d", port);
    if (getaddrinfo(server, portString, &hints, &addrInfo) != 0) {
        BDBG_ERR(("%s: ERROR: getaddrinfo failed for server:port: %s:%d, errno %d", __FUNCTION__, server, port, errno));
        perror("getaddrinfo");
        return -1;
    }

    sd = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol);
    if (sd < 0) {
        BDBG_ERR(("%s: socket() error, errno %d", __FUNCTION__, errno));
        perror("socket");
        goto error;
    }

    localAddr.sin_family = addrInfo->ai_family;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(0);

    if (bind(sd, (struct sockaddr *)&localAddr, sizeof(localAddr))) {
        BDBG_ERR(("%s: bind() error, errno %d", __FUNCTION__, errno));
        perror("bind");
        goto error;
    }

    /* Connect to server */
    if (connect(sd, addrInfo->ai_addr, addrInfo->ai_addrlen)) {
        BDBG_ERR(("%s: TCP connect Error to %s:%d failed, errno %d\n", __FUNCTION__, server, port, errno));
        goto error;
    }

    BDBG_MSG(("TCP: Connection Success\n"));
    if (addrInfo)
        freeaddrinfo(addrInfo);
    return sd;

error:
    if (sd > 0)
        close(sd);
    if (addrInfo)
        freeaddrinfo(addrInfo);
    return -EINVAL;
}

int tcprecv(int fd, void *buf, size_t n, int flags)
{
    return recv(fd,buf,n,flags);
}

void tcpClose(int sd)
{
    close(sd);
}
