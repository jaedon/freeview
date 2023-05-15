/***************************************************************************
 *     Copyright (c) 2002, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: webserver.cpp $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:22p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/webserver.cpp $
 * 
 * 1   2/7/05 11:22p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   11/6/03 5:11p erickson
 * PR8570: I'm not done, but I need to work on another bug.
 * 
 * Irvine_BSEAVSW_Devel/PR8570/1   11/6/03 11:40a erickson
 * I'm not done, but I need to work on another bug.
 * 
 * SanJose_Linux_Devel/3   1/13/03 10:54a erickson
 * Reworked HTTPServer and SoapServer to allow file-based io
 *
 ***************************************************************************/

//#define USE_SOCKETS

#ifdef USE_SOCKETS
#include "mhttpsocketserver.h"
#else
#include "mhttpfileserver.h"
#include <errno.h>
#include <fcntl.h>
#endif

int main(int argc, char **argv) {
#ifdef USE_SOCKETS
	MHttpSocketServer server;
	int port = 80;
	if (argc >= 2)
		port = atoi(argv[1]);
	return server.listen(port);
#else
	MHttpFileServer server;
	const char *infile = argv[1];
	const char *outfile = argv[2];
	int inputFd, outputFd;

	if (infile[0] == '-')
		inputFd = 0;
	else {
		inputFd = open(infile, O_RDWR, 0664);
		if (inputFd < 0) {
			printf("Unable to open %s for input: %d\n", infile, errno);
			exit(0);
		}
	}
	if (outfile[0] == '-')
		outputFd = 0;
	else {
		outputFd = open(outfile, O_RDWR, 0664);
		if (outputFd < 0) {
			printf("Unable to open %s for output: %d\n", outfile, errno);
			exit(0);
		}
	}

	//server.setPrintHttp(true);
	return server.processAll(inputFd, outputFd);
#endif
}
