/***************************************************************************
 *     Copyright (c) 2002, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mhttpfileserver.cpp $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mhttpfileserver.cpp $
 * 
 * 1   2/7/05 11:12p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/3   5/9/03 2:37p erickson
 * request now requires MHttpServer param
 * 
 * Irvine_BSEAVSW_Devel/2   2/3/03 4:09p erickson
 * fixed error message for unhandled request.
 * 
 * Irvine_BSEAVSW_Devel/1   1/13/03 12:13p erickson
 * new classes to extend functionality of mhttpserver
 *
 ***************************************************************************/
#include "mhttpfileserver.h"

MHttpFileServer::MHttpFileServer() {
	_running = true;
	_useSendRecv = false;
}

int MHttpFileServer::processAll(int inputFd, int outputFd) {
	while (_running) {
		process(inputFd, outputFd);
	}
	return 0;
}

int MHttpFileServer::process(int inputFd, int outputFd) {
	_inputFd = inputFd;
	_outputFd = outputFd;
	clearHeaders();
	_responseCode = 200;
	_wroteHeaders = false;

	if (readRequestHeader() == -1) {
		// this is a "normal" message for keepalive
		if (!_silent)
			printf("Unable to read HTTP headers.\n");
		return -1;
	}
	// process incoming header here
	clearHeaders();

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

	flush();
	return 0;
}
