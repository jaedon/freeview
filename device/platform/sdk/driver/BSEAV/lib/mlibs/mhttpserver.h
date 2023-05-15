/***************************************************************************
 *     Copyright (c) 2002, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mhttpserver.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:13p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mhttpserver.h $
 * 
 * 1   2/7/05 11:13p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/8   5/9/03 2:37p erickson
 * plugin's now support multithreaded access, therefore the MHttpServer
 * must be passed
 * in as a param to request
 *
 * Irvine_BSEAVSW_Devel/7   2/3/03 4:09p erickson
 * fixed error message for unhandled request.
 *
 * SanJose_Linux_Devel/6   1/13/03 10:54a erickson
 * Reworked HTTPServer and SoapServer to allow file-based io
 *
 ***************************************************************************/
#ifndef MHTTPSERVER_H
#define MHTTPSERVER_H

#include "mhttpengine.h"

class MHttpServer;

class MHttpServerPlugin {
public:
	/**
	* Return true if you accept the request and have processed it, otherwise return
	* false and it will be passed to someone else.
	*/
	virtual bool request(MHttpServer *server, const char *method, const char *request) = 0;
};

/**
* MHttpServer HOWTO
*
* 1. Add a handler for MHttpServerEventHandler
* 2. Call listen(port)
* 3. When you get an Accepted event, you've got a request.
* 4. Call getMethod(), getRequest() and getHeader() to process the request.
* 5. Call setResponseCode() and setHeader() to start the response.
* 6. When you've set all your response headers, start calling write().
* 7. When you're done writing, call close() and exit the handler.
*
* See webserver.cpp for a simple example.
*/
class MHttpServer : public MHttpEngine {
public:
	MHttpServer();
	~MHttpServer();

	/**
	* For maximum performance, turn off all messages.
	*/
	void setSilent(bool silent);
	bool silent() const {return _silent;}

	const char *getMethod() const {return _method;}
	const char *getRequest() const {return _request;}
	void setResponseCode(int code);

	void addPlugin(MHttpServerPlugin *plugin);
	bool removePlugin(MHttpServerPlugin *plugin);

	const char *lookupContentTypeByExtension(const char *extension);

	/**
	* Sets where does the default GET handler reads file from.
	* By default it is "."
	**/
	void setDocumentRoot(const char *docroot) {_docroot = docroot;}

	/**
	* Gets the URL base for this server.
	**/
	virtual MString getUrlBase() = 0;

protected:
	bool _silent;

	MString _request, _method;
	int _responseCode;
	int readRequestHeader();
	virtual int writeHeaders();

	MList<MHttpServerPlugin> _plugins;
	MString _docroot;

	virtual bool handleRequest();
};

#endif //MHTTPSERVER_H
