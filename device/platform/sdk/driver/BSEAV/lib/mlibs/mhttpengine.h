/***************************************************************************
 *     Copyright (c) 2002, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mhttpengine.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mhttpengine.h $
 * 
 * 1   2/7/05 11:12p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/11   5/8/03 3:03p erickson
 * check to make sure headers aren't read twice
 * 
 * Irvine_BSEAVSW_Devel/10   5/2/03 3:59p erickson
 * added read comments
 * 
 * SanJose_Linux_Devel/9   1/13/03 12:11p erickson
 * added inputfd and outputfd
 *
 ***************************************************************************/
#ifndef MHTTPENGINE_H
#define MHTTPENGINE_H

#if defined(_WIN32)
	#include <winsock.h>	/* WSADATA */
	static  wsa_error;
	#define SOCKET_ERRNO	((wsa_error=WSAGetLastError())>0 ? wsa_error-WSABASEERR : wsa_error)
#else
	#define SOCKET_ERRNO	errno
#endif

#include "mstringhash.h"

#define MAXHEADERLINE 1024

class MHttpEngine {
public:
	MHttpEngine();
	~MHttpEngine();

	void close();
	void flush();
	void setPrintHttp(bool printHttp) {_printHttp = printHttp;}

	/**
	* After a ReadData event, call these to get the request
	*/
	const char *getHeader(const char *name);
	
	/**
	* Read data from HTTP. Returns < 0 on error, otherwise length read.
	**/
	int read(char *buf, int len);

	/**
	* Read a \n terminated line from HTTP. Returns pointer to string
	* on success, NULL on failure.
	**/
	char *readline(char *buf, int len);

	/**
	* Create a new file and read from HTTP and write to the file.
	* Returns 0 on success, -1 on failure.
	**/
	int readFile(const char *filename);

	/**
	* Checks for a Content-length, then reads the entire
	* body into the buffer. Returns the amount read.
	*/
	int readBody(char *buf, int len);

	void clearHeaders() {_headers.clear();}
	void setHeader(const char *name, const char *value);
	void setHeader(const char *name, int value) {
		setHeader(name, MString(value));
	}
	/**
	* Call write() only after you have set all headers.
	* Call write(NULL) when you're done writing.
	*/
	int write(const char *buf, int len = -1);
	int writeline(const char *buf);
	int writeFile(const char *filename);

	void printHeaders();

	bool isConnected();

protected:
#ifdef _WINSOCKAPI_
	WSADATA WinSockData;
#endif

	/**
	* There are 3 types of IO supported:
	* 1. send/recv for sockets
	* 2. read/write for files or sockets
	* 3. fread/fwrite for stdio
	*
	* Logic:
	*   If _currentFd is not NULL, it is used with fwrite() and fread().
	*   else if _useSendRecv is true, _currentFd is used with send() and recv()
	*   else _inputFd and _outputFd are used with write() and read().
	**/
	bool _useSendRecv; // if true, use _currentFd, else _inputFd/_outputFd;
	int _currentFd;
	int _inputFd, _outputFd;
	FILE *_currentFile; // use this if !NULL, else _currentFd

	bool _printHttp;
	MStringHash _headers;
	bool _wroteHeaders, _readHeaders;

	virtual int writeHeaders();
	int readHeaders();
	int _write(const char *buf, int len = -1);
};

#endif //MHTTPENGINE_H
