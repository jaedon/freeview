/***************************************************************************
 *     Copyright (c) 2002, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mhttpfileserver.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mhttpfileserver.h $
 * 
 * 1   2/7/05 11:12p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   1/13/03 12:13p erickson
 * new classes to extend functionality of mhttpserver
 *
 ***************************************************************************/
#ifndef MHTTPFILESERVER_H__
#define MHTTPFILESERVER_H__

#include "mhttpserver.h"

class MHttpFileServer : public MHttpServer {
public:
	MHttpFileServer();

	/**
	* process an Http request and response.
	*
	* inputFd is a read file descriptor.
	* outputFd is a write file descriptor.
	**/
	int process(int inputFd, int outputFd);

	/**
	* Keep processing until stop() is called.
	**/
	int processAll(int inputFd, int outputFd);

	void stop() {_running = false;}

	MString getUrlBase() {return "http://directconnection";}

protected:
	bool _running;
};

#endif /* MHTTPFILESERVER_H__ */
