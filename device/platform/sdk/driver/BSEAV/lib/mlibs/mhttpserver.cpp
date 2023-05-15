/***************************************************************************
 *     Copyright (c) 2002-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mhttpserver.cpp $
 * $brcm_Revision: 3 $
 * $brcm_Date: 2/26/08 4:32p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mhttpserver.cpp $
 * 
 * 3   2/26/08 4:32p jrubio
 * PR39363: fixed warnings
 * 
 * 2   8/4/05 4:43p erickson
 * PR15139: fixed -pedantic warnings
 * 
 * 1   2/7/05 11:12p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   7/10/03 11:32a erickson
 * parse out http://host:port if present in request
 * 
 * Irvine_BSEAVSW_Devel/13   5/9/03 2:37p erickson
 * plugin's now support multithreaded access, therefore the MHttpServer
 * must be passed
 * in as a param to request
 * 
 * Irvine_BSEAVSW_Devel/12   2/3/03 4:09p erickson
 * fixed error message for unhandled request.
 * 
 * SanJose_Linux_Devel/11   1/13/03 10:54a erickson
 * Reworked HTTPServer and SoapServer to allow file-based io
 *
 ***************************************************************************/
#include "mhttpserver.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

MHttpServer::MHttpServer() {
	_silent = false;
	_docroot = ".";
}

MHttpServer::~MHttpServer() {
}

void MHttpServer::setSilent(bool silent) {
	_silent = silent;
}

void MHttpServer::setResponseCode(int code) {
	_responseCode = code;
}

int MHttpServer::readRequestHeader() {
	char buf[MAXHEADERLINE];

	// read lines until we get what is probably the header
	// this is needed for keep-alive
	char *line;
	while ((line = readline(buf, MAXHEADERLINE))) {
		if (strstr(line, " HTTP/1"))
			break;
	}
	if (!line) return -1;

	char *find = strchr(line, ' ');
	if (!find) return -1;
	_method.strncpy(line,find-line);
	line = find+1;
	find = strchr(line, ' ');
	if (!find) return -1;
	_request.strncpy(line,find-line);

	// Valid requests can also include http://host:port.
	// I'm not sure what's correct, but I'm going to parse that off if detected.
	if (strstr(_request, "http://") == (const char *)_request) {
		const char *s = _request;
		s += 7;
		s = strchr(s, '/');
		if (s)
			_request = s;
		else
			_request = "/";
	}

	return readHeaders();
}

int MHttpServer::writeHeaders() {
	char buf[MAXHEADERLINE];
	//TODO: translate _responseCode to message, don't hardcode OK
	int len = snprintf(buf, MAXHEADERLINE, "HTTP/1.0 %d OK\r\n", _responseCode);
	if (_write(buf, len) == -1)
		return -1;
	else
		return MHttpEngine::writeHeaders();
}

bool MHttpServer::handleRequest() {
	if (strcmp(getMethod(),"GET"))
		return false;
	else {
		MString req = _docroot + "/" + getRequest();
		if (req && req[req.length()-1] == '/')
			req += "index.html";

		// set default content type
		int dot = req.findRev('.');
		if (dot != -1) {
			const char *ct = lookupContentTypeByExtension(req.mid(dot+1));
			if (ct)
				setHeader("Content-Type", ct);
		}

		if (writeFile(req) == -1)
			write("Unknown document");
		return true;
	}
}

struct contenttype {char *extension; char *type; };
static contenttype contenttypes[] = {
	{(char *)"html", (char *)"text/html"},
	{(char *)"html", (char *)"text/xml"},
	{(char *)"mp3", (char *)"audio/mpeg"},
	{0,0}};

const char *MHttpServer::lookupContentTypeByExtension(const char *extension) {
	for (int i=0;contenttypes[i].extension;i++)
		if (!strcmp(contenttypes[i].extension, extension))
			return contenttypes[i].type;
	return NULL;
}

void MHttpServer::addPlugin(MHttpServerPlugin *plugin) {
	_plugins.add(plugin);
}

bool MHttpServer::removePlugin(MHttpServerPlugin *plugin) {
	return _plugins.remove(plugin) ? true:false;
}
