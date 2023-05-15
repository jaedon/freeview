/***************************************************************************
 *     Copyright (c) 2002, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mhttpsocketserver.cpp $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:13p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mhttpsocketserver.cpp $
 * 
 * 1   2/7/05 11:13p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   11/6/03 5:11p erickson
 * PR8570: I'm not done, but I need to work on another bug.
 * 
 * Irvine_BSEAVSW_Devel/PR8570/1   11/6/03 11:40a erickson
 * I'm not done, but I need to work on another bug.
 * 
 * Irvine_BSEAVSW_Devel/4   5/9/03 2:37p erickson
 * added MHttpSocketListener to support multithreaded http server
 *
 * Irvine_BSEAVSW_Devel/3   2/3/03 4:09p erickson
 * fixed error message for unhandled request.
 *
 * Irvine_BSEAVSW_Devel/2   1/22/03 10:13a erickson
 * fixed bug building urlbase()
 *
 * Irvine_BSEAVSW_Devel/1   1/13/03 12:13p erickson
 * new classes to extend functionality of mhttpserver
 *
 ***************************************************************************/
#include "mhttpsocketserver.h"
#include <pthread.h>

#if defined(__unix__) || defined(__vxworks)
	#include <unistd.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h> //inet_ntop
#endif
#include <signal.h>

#if defined(__vxworks)
	#include <sockLib.h>	// socket
	#include <hostLib.h>	// hostGetByName
	#include <inetLib.h>	// inet_ntoa_b
	#define inet_ntop(af, src, dst, len)	inet_ntoa_b(*((struct in_addr*)src),dst)
    #include <selectLib.h>
#elif defined(_WIN32)
	#define inet_ntop(af, src, dst, len)	sprintf(dst,"%.*s",len,inet_ntoa(*((struct in_addr*)src)))
#endif

#if !defined(__unix)
	#define socklen_t int
#endif

/**
* PrivateData allows socket data structures to be stored at
* a class level without having lots of socket includes in
* mhttpserver.h.
*/
#define MAXADDRNAME 100
struct MHttpSocketServerPrivateData {
	struct sockaddr_in clientAddr;
	char clientAddress[MAXADDRNAME];
	char serverAddress[MAXADDRNAME];
};

MHttpSocketServer::MHttpSocketServer(int socketFd, int port) {
	_socketFd = socketFd;
	_private = new MHttpSocketServerPrivateData;
	_port = port;
	_keep_alive = true;
	_private->serverAddress[0] = 0;
	_private->clientAddress[0] = 0;
#if defined(__unix__) || defined(__vxworks)
	signal(SIGPIPE, SIG_IGN);
#endif
}

MHttpSocketServer::~MHttpSocketServer() {
	delete _private;
}

int MHttpSocketServer::listen(int port, bool doAccept) {
	struct sockaddr_in servAddr;

	_port = port;
	_socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if(_socketFd<0) {
		perror("cannot open socket ");
		return -1;
	}
	int one = 1;
	if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one))) {
		perror("setsockopt");
		return -1;
	}
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(port);
	if (bind(_socketFd, (struct sockaddr *) &servAddr, sizeof(servAddr))<0) {
		perror("cannot bind port ");
		return -1;
	}
	::listen(_socketFd,5);

	_listening = true;
	while (_listening && doAccept) {
		if (accept())
			return -1;
	}
	return 0;
}

int MHttpSocketServer::accept(int msecTimeout) {
	if (!_silent)
		printf("Accepting connections on port %d, timeout %d\n", _port, msecTimeout);

	_private->serverAddress[0] = 0;
	_private->clientAddress[0] = 0;

	// if we have a timeout, use select
	if (msecTimeout >= 0) {
		fd_set readfds;
		struct timeval tv;

		tv.tv_sec = msecTimeout / 1000;
		tv.tv_usec = (msecTimeout % 1000) * 1000;
		FD_ZERO(&readfds);
		FD_SET(_socketFd, &readfds);
		switch (select(_socketFd+1, &readfds, NULL, NULL, &tv)) {
		case -1:
			// error
			return -1;
		case 0:
			// nothing to accept
			return 0;
		// default: fall through and process the connection
		}
	}

	socklen_t l = sizeof(_private->clientAddr);
	_currentFd = ::accept(_socketFd, (struct sockaddr *) &_private->clientAddr, &l);
	if(_currentFd < 0) {
		perror("cannot accept connection ");
		return -1;
	}

	if (!_silent)
		printf("...got connection from client %s, server %s\n",
			clientAddress(), serverAddress());

	bool keepalive = false;
	do {
		clearHeaders();
		_responseCode = 200;
		_wroteHeaders = false;

		if (readRequestHeader() == -1) {
			// this is a "normal" message for keepalive
			if (!_silent)
				printf("Unable to read HTTP headers.\n");
			break;
		}
		const char *connection;
		keepalive = ((connection = getHeader("Connection")) &&
			!strcasecmp(connection, "Keep-Alive"));
		clearHeaders();
		if(keepalive) {
			if(!_silent)
				printf("Keep-alive received\n");
			setHeader("Connection", "Keep-Alive");
		}

		MHttpServerPlugin *plugin = NULL;
		for (plugin = _plugins.first(); plugin; plugin = _plugins.next()) {
			if (plugin->request(this, getMethod(), getRequest()))
				break;
		}
		if (!plugin) {
			// no one handled it
			if (!handleRequest())
				printf("Unable to handle request: %s %s\n", getMethod(), getRequest());
		}

		// if content-length wasn't specified, we can't
		// keep going. the client will hang.
		if (keepalive && !getHeader("Content-length"))
			keepalive = false;
		flush();
	} while (keepalive && _keep_alive);
	close();
	return 0;
}

const char *MHttpSocketServer::serverAddress() {
	if (!_private->serverAddress[0]) {
		struct sockaddr_in temp;
		socklen_t tempsize = sizeof(temp);
		getsockname(_currentFd, (struct sockaddr *)&temp, &tempsize);
		inet_ntop(AF_INET, (void*)&temp.sin_addr, _private->serverAddress, MAXADDRNAME);
	}
	return _private->serverAddress;
}

const char *MHttpSocketServer::clientAddress() {
	if (!_private->clientAddress[0]) {
		inet_ntop(AF_INET, (void*)&_private->clientAddr.sin_addr, _private->clientAddress, MAXADDRNAME);
	}
	return _private->clientAddress;
}

void MHttpSocketServer::setKeepAlive(bool keep_alive) {
	_keep_alive = keep_alive;
}

MString MHttpSocketServer::getUrlBase() {
	char buf[128];
	snprintf(buf, 128, "http://%s:%d", serverAddress(), port());
	return buf;
}

////////////////////////////////////////////////////////

class MHttpSocketListenerPrivateData {
public:
	pthread_mutex_t mutex;
};

void *MHttpSocketListener::acceptHttp(void *data)
{
	MHttpSocketListener *listener = (MHttpSocketListener *)data;
	
	MHttpSocketServer server(listener->_socketFd, listener->_port);
	pthread_mutex_lock(&listener->_private->mutex);
	for (MHttpServerPlugin *plugin = listener->_plugins.first(); plugin; plugin = listener->_plugins.next()) {
		server.addPlugin(plugin);
	}
	pthread_mutex_unlock(&listener->_private->mutex);
	server.accept();
	return NULL;
}

MHttpSocketListener::MHttpSocketListener()
{
	_listening = true;
	_private = new MHttpSocketListenerPrivateData;
	pthread_mutex_init(&_private->mutex, NULL);
}

int MHttpSocketListener::listen(int port)
{
	struct sockaddr_in servAddr;

	_port = port;
	_socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if(_socketFd<0) {
		perror("cannot open socket ");
		return -1;
	}
	int one = 1;
	if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one))) {
		perror("setsockopt");
		return -1;
	}
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(port);
	if (bind(_socketFd, (struct sockaddr *) &servAddr, sizeof(servAddr))<0) {
		perror("cannot bind port ");
		return -1;
	}

	while (1) {
		fd_set readfds;
//		struct timeval tv;
		printf("listening...\n");
		::listen(_socketFd,5);

//		tv.tv_sec = msecTimeout / 1000;
//		tv.tv_usec = (msecTimeout % 1000) * 1000;
		FD_ZERO(&readfds);
		FD_SET(_socketFd, &readfds);
		switch (select(_socketFd+1, &readfds, NULL, NULL, NULL/*&tv*/)) {
		case -1:
			// error
			return -1;
		case 0:
			// nothing to accept
			return 0;
		// default: fall through and process the connection
		}

		pthread_t thread;
		pthread_create(&thread, NULL, acceptHttp, (void*)this);
		pthread_detach(thread);
	}
	return 0;
}


void MHttpSocketListener::addPlugin(MHttpServerPlugin *plugin) 
{
	pthread_mutex_lock(&_private->mutex);
	_plugins.add(plugin);
	pthread_mutex_unlock(&_private->mutex);
}
