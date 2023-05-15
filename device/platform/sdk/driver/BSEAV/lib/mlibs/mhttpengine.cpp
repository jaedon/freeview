/***************************************************************************
 *     Copyright (c) 2002, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mhttpengine.cpp $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mhttpengine.cpp $
 * 
 * 1   2/7/05 11:12p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/20   1/31/03 4:35p erickson
 * added #ifndef NO_SOCKETS option
 * 
 * SanJose_Linux_Devel/19   1/13/03 10:54a erickson
 * Reworked HTTPServer and SoapServer to allow file-based io
 *
 ***************************************************************************/
#include "mhttpengine.h"
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#if defined(__vxworks)
	#include <sockLib.h>	// struct timeval
	#include <selectLib.h>	// select()
#endif
#if defined(__unix__) || defined(__vxworks)
	#include <unistd.h>
	#include <sys/socket.h>	/* socket/bind/etc. */
#endif
#if defined(_WIN32)
	#include <io.h>		/* open/close */
#endif

MHttpEngine::MHttpEngine() : _headers(false) {
#ifdef _WINSOCKAPI_
    WSAStartup(MAKEWORD(1,1), &WinSockData);
#endif
	_printHttp = false;
	_currentFd = _outputFd = _inputFd = -1;
	_useSendRecv = true;
	_currentFile = NULL;
}

MHttpEngine::~MHttpEngine() {
#ifdef _WINSOCKAPI_
	WSACleanup();
#endif
}

void MHttpEngine::close() {
#ifndef NO_SOCKETS
	if (_currentFd >= 0)
		shutdown(_currentFd, 2);	/* SHUT_RDWR */
#endif

	if (_currentFile) {
		fclose(_currentFile);
		_currentFd = -1;
		_currentFile = NULL;
	}
	else if (_currentFd >= 0) {
#if defined(_WIN32)
		closesocket(_currentFd);
#else
		::close(_currentFd);
#endif
		_currentFd = -1;
	}
	// don't close _inputFd and _outputFd.
	// MHttpFileServer doesn't allow this to be called anyway.
}

bool MHttpEngine::isConnected()
{
	if (_currentFile)
		return true;
	if (!_useSendRecv && _inputFd >= 0 || _outputFd >= 0)
		return true;
		
	// _currentFd gets more processing as a socket
	if (_currentFd == -1)
		return false;

	fd_set	socket_set;
	FD_ZERO(&socket_set);
	FD_SET(_currentFd,&socket_set);

	struct	timeval tv;
	tv.tv_sec=0;
	tv.tv_usec=0;

	if (select(_currentFd+1,&socket_set,NULL,NULL,&tv)<0)
		return false ;

	return true;
}

const char *MHttpEngine::getHeader(const char *name) {
	return _headers.get(name);
}

int MHttpEngine::readHeaders() {
	char buf[MAXHEADERLINE];
	_headers.clear();
	char *line;
	// now read the headers
	while ((line = readline(buf, MAXHEADERLINE))) {

		//printf("read line <%s>\n", line);
		// we've finished reading the header
		if (!*line)
			break;
		char *colon = strchr(line, ':');
		if (colon) {
			char *value = strskip(colon+1, " \t");
			_headers.add(line,colon-line,value);
		}
	}
	if (!line) return -1;
	return 0;
}

void MHttpEngine::setHeader(const char *name, const char *value) {
	_headers.add(name, value);
}

int MHttpEngine::write(const char *buf, int len) {
	if (!_wroteHeaders)
		if (writeHeaders() == -1) {
			printf("Couldn't write headers, errno %d\n", SOCKET_ERRNO);
			return -1;
		}
	if (buf)
		return _write(buf, len);
	return(-1);
}

int MHttpEngine::writeline(const char *buf) {
	write(buf);
	return(write("\r\n"));
}

int MHttpEngine::writeHeaders() {
	_wroteHeaders = true;
	for (_headers.first();_headers.current();_headers.next()) {
		if (_write(_headers.name().s()) == -1 ||
			_write(": ") == -1 ||
			_write(_headers.value().s()) == -1 ||
			_write("\r\n") == -1)
		{
			return -1;
		}
	}
	if (_write("\r\n") == -1)
		return -1;
	//flush();
	return 0;
}

int MHttpEngine::_write(const char *buf, int len) {
	if (len == -1)
		len = strlen(buf);
	if (_printHttp)
		printf("TX: %.*s\n", len, buf);
	if (!_currentFile) {
#ifndef NO_SOCKETS
		if (_useSendRecv)
			return ::send(_currentFd, (char*)buf, len, 0 /* flags */);
		else
#endif
			return ::write(_outputFd, (char*)buf, len);
	}
	else
		return fwrite(buf, 1, len, _currentFile);
}

int MHttpEngine::read(char *buf, int len) {
	int n;
	if (!_currentFile) {
#ifndef NO_SOCKETS
		if (_useSendRecv)
			n = ::recv(_currentFd, buf, len, 0 /* flags */);
		else
#endif
			n = ::read(_inputFd, buf, len);
	}
	else
		n = fread(buf, 1, len, _currentFile);
	if (_printHttp)
		printf("RX: %.*s\n", n, buf);
	return n;
}

/****************************************************************************/
/* Truncates white-space chars off end of 'str'								*/
/****************************************************************************/
static void truncsp(char *str)
{
	size_t c;

	if(str==NULL)
		return;
	c=strlen(str);
	while(c && (unsigned char)str[c-1]<=' ') c--;
	str[c]=0;
}


char *MHttpEngine::readline(char *buf, int len)
{
	char *s;

	if (!_currentFile) {
		char	ch;
		int		rd=0;
		fd_set	socket_set;

		while (rd<len-1) {
			FD_ZERO(&socket_set);
			
#ifndef NO_SOCKETS
			if (_useSendRecv) {
				FD_SET(_currentFd,&socket_set);
				if (::select(_currentFd+1,&socket_set,NULL,NULL,NULL) < 1)
					return NULL; /* invalid fd */
				if (::recv(_currentFd, &ch, 1, 0 /* flags */) < 1)
					return NULL; /* disconnected */
			}
			else 
#endif
			{
				FD_SET(_inputFd,&socket_set);
				if (::select(_inputFd+1,&socket_set,NULL,NULL,NULL) < 1)
					return NULL; /* invalid fd */
				if (::read(_inputFd, &ch, 1) < 1)
					return NULL; /* disconnected */
			}
			if(ch=='\n')
				break;
			buf[rd++]=ch;
		}
		buf[rd]=0;

		s = buf;
	} else
		s = fgets(buf, len, _currentFile);

	truncsp(s);
	if (_printHttp && s)
		printf("RX: %s\n",s);
	return s;
}

int MHttpEngine::readBody(char *buf, int len) {
	const char *clstr = getHeader("content-length");
	int cl = clstr?atoi(clstr):len;
	if (cl < len)
		len = cl;
	return read(buf, len);
}

int MHttpEngine::writeFile(const char *filename) {
	FILE *f = fopen(filename, "r");
	if (f) {
		char buf[4096];
		while (!feof(f)) {
			int r = fread(buf, 1, 4096, f);
			if (!r)
				break;
			if (write(buf, r) == -1)
				break;
		}
		fclose(f);
		return 0;
	}
	return -1;
}

int MHttpEngine::readFile(const char *filename) {
	FILE *f = fopen(filename, "w+");
	if (f) {
		char buf[4096];
		int n;
		do {
			n = read(buf, sizeof(buf));
			//printf("readFile %d\n", n);
			if (n)
				fwrite(buf, 1, n, f);
		} while (n > 0);
		fclose(f);
		return 0;
	}
	return -1;
}

void MHttpEngine::printHeaders() {
	for (_headers.first();_headers.current();_headers.next()) {
		printf("%s: %s\n", _headers.name().s(), _headers.value().s());
	}
}

void MHttpEngine::flush() {
	if (_currentFile)
		fflush(_currentFile);
}

