/***************************************************************************
 *     Copyright (c) 2002, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mhttpsocketserver.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:13p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mhttpsocketserver.h $
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
 * Irvine_BSEAVSW_Devel/2   5/9/03 2:37p erickson
 * added MHttpSocketListener to support multithreaded http server
 *
 * Irvine_BSEAVSW_Devel/1   1/13/03 12:14p erickson
 * new classes to extend functionality of mhttpserver
 *
 ***************************************************************************/
#ifndef MHTTPSOCKETSERVER_H__
#define MHTTPSOCKETSERVER_H__

#include "mhttpserver.h"

struct MHttpSocketServerPrivateData;

/**
* MHttpSocketServer is used for serving HTTP requests over sockets.
* 
* This object is single-threaded and not reentrant.
* Use MHttpSocketListener for multi-threaded a HTTP server.
* However, the listen() method can block or be non-blocking for idle-time processing.
**/
class MHttpSocketServer : public MHttpServer {
public:
	MHttpSocketServer(int socketFd = -1, int port = 0);
	virtual ~MHttpSocketServer();

	/**
	* When a connection is accepted, the Accepted event is fired.
	* All processing must happen before you return from the event,
	* because the connection will be dropped.
	* The header "Content-Type: text/html" and response code 200 are preset.
	*/
	int listen(int port, bool doAccept = true);

	/**
	* Call stopListening during an Accept event, and the current
	* listen() or accept() method will exit.
	*/
	void stopListening() {_listening = false;}

	/**
	* If you listen with doAccept == false, then you can accept
	* connections by calling accept. The msecTimeout specifies how
	* many milliseconds can expire with no connection before returning.
	* If msecTimeout == -1, it will block forever.
	**/
	int accept(int msecTimeout = -1);

	/**
	* port() returns the port that the server is listening on.
	*/
	int port() const {return _port;}

	/**
	* serverAddress() returns a string ip address in the form of "#.#.#.#"
	* for the server's current connection. Note that for a multihomed host,
	* this might change from connection to connection.
	*/
	const char *serverAddress();

	/**
	* clientAddress() returns a string ip address in the form of "#.#.#.#"
	* for the client currently connected to the server.
	*/
	const char *clientAddress();

	/**
	* Used to disable keep-alive connection support
	*/
	void setKeepAlive(bool keep_alive);

	/**
	* http://serverAddress:port
	**/
	MString getUrlBase();

protected:
	MHttpSocketServerPrivateData *_private;
	bool _listening;
	bool _keep_alive;
	int _port;
	int _socketFd;
};


struct MHttpSocketListenerPrivateData;

/**
* MHttpSocketListener is an optional pthread-based listener. You can use
* MHttpSocketServer's listener, but it must always be single threaded.
*
* MHttpSocketListener creates a new MHttpSocketServer for each socket connection accepted.
* You can add plugins to the listener and they are passed on to each
* server. The plugins are responsible for handling synchronization.
**/
class MHttpSocketListener {
public:
	MHttpSocketListener();

	/**
	* When a connection is accepted, the Accepted event is fired.
	* All processing must happen before you return from the event,
	* because the connection will be dropped.
	* The header "Content-Type: text/html" and response code 200 are preset.
	*/
	int listen(int port);

	/**
	* Call stopListening during an Accept event, and the current
	* listen() or accept() method will exit.
	*/
	void stopListening() {_listening = false;}

	void addPlugin(MHttpServerPlugin *plugin);

protected:
	bool _listening;
	int _socketFd, _port;
	MList<MHttpServerPlugin> _plugins;
	MHttpSocketListenerPrivateData *_private;

	static void *acceptHttp(void *data);
};

#endif /* MHTTPSOCKETSERVER_H__ */
