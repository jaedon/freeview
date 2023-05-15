/***************************************************************************
 *     (c)2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: mhttpclient.cpp $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/7/12 10:05a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mhttpclient.cpp $
 * 
 * 2   9/7/12 10:05a erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "mhttpclient.h"
#include "murl.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#if defined(__unix__)
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>	//inet_pton
	#include <netdb.h>		// gethostbyname
	#include <unistd.h>
	extern int h_errno;
#endif

#if defined(__vxworks)
	#include <sockLib.h>	// socket
	#include <hostLib.h>	// hostGetByName
#endif

int MHttpClient::connect(const char *host, int port) {
#ifndef NO_SOCKETS
	struct sockaddr_in addr;

	_currentFd = socket(AF_INET, SOCK_STREAM, 0);
	if(_currentFd<0) {
		perror("cannot open socket ");
		return -1;
	}

	addr.sin_family = AF_INET;

#if defined(__vxworks)
    addr.sin_addr.s_addr = hostGetByName((char*)host);
	if(addr.sin_addr.s_addr==ERROR) {
		perror("resolving hostname");
		return -1;
	}
#else
	struct hostent *h = gethostbyname(host);
	if (!h) {
#if defined(_WIN32)
		printf("gethostbyname error errno=%d\n",SOCKET_ERRNO);
#else
		printf("gethostbyname error h_errno=%d\n", h_errno);
#endif
		return -1;
	}
	//char *addrstr = h->h_addr_list[0];
/*
	int rc = inet_pton(AF_INET,host,&addr.sin_addr);
	if (rc < 0) {
		printf("inet_pton error %d\n", errno);
		return -1;
	}
	else if (rc == 0) {
		printf("Invalid address %s\n", addrstr);
		return -1;
	}
*/
	addr.sin_addr.s_addr = *(long *)h->h_addr_list[0];
#endif
	addr.sin_port = htons(port);
	if (::connect(_currentFd, (struct sockaddr *) &addr, sizeof(addr))<0) {
		printf("Cannot connect to %s:%d, errno %d\n", host, port, SOCKET_ERRNO);
		close();	// This resets errno :-(
		return -1;
	}
#if !defined(_WIN32)
	_currentFile = fdopen(_currentFd, "r+");
	if (!_currentFile) {
		close();
		return -1;
	}
#endif
#endif
	return 0;
}

int MHttpClient::get(const char *urlstr) {
	if (sendRequest("GET", urlstr))
		return -1;
	if (writeHeaders())
		return -1;
	return readResponseHeader();
}

int MHttpClient::sendRequest(const char *method, const char *urlstr) {
	char buf[MAXHEADERLINE];
	MUrl url(urlstr);

	bool keepalive = false;
	if (_currentFd == -1) {
		if (!url.server() || connect(url.server(), url.port()) == -1)
			return -1;
	}
	else
		keepalive = true;

	const char *query = url.query();
	if (!query || !*query)
		query = "/";
	int len = snprintf(buf, MAXHEADERLINE, "%s %s HTTP/1.0\r\n", method, query);
	if (_write(buf, len) == -1)
		return -1;
	_headers.clear();
	if (keepalive)
		setHeader("Connection", "Keep-Alive");
	_wroteHeaders = false;
	_readHeaders = false;
	return 0;
}

int MHttpClient::readResponseHeader() {
	if (_readHeaders) {
		// this is a danger for get()
		printf("Cannot read headers twice.\n");
		return -1;
	}
	_readHeaders = true;

	char buf[MAXHEADERLINE];
	// read the top line
	char *line = readline(buf, MAXHEADERLINE);
	if (!line) return -1;
	char *find = strchr(line, ' ');
	if (!find) return -1;
	line = find+1;
	find = strchr(line, ' ');
	if (!find) return -1;
	_responseCode = MString(line,find-line).toInt();

	return readHeaders();
}

