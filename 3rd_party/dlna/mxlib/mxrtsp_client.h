
// Author : Park, Doyu.

#pragma once


#include "core/ILibParsers.h"
#include "core/ILibWebServer.h"
#include "core/ILibWebClient.h"
#include "core/ILibAsyncSocket.h"
#include "core/ILibAsyncUDPSocket.h"

//#include "mxrtp.h"

#define MAX_MXRTSP_SESSION_ID			32

typedef struct _MXRTSP_SESSION {

	char id[MAX_MXRTSP_SESSION_ID];
//	struct _MXRTP_SESSION *rtp_session;
	void *rtp_session;

	struct _MXRTSP_SESSION *next;
	struct _MXRTSP_SESSION *prev;

} MXRTSP_SESSION;





typedef enum _MXRTSP_SERVER_RETURN_CODE {
	MXRTSP_SERVER_RETURN_CODE_ERROR = -1,	
	MXRTSP_SERVER_RETURN_CODE_OK = 0,	
} MXRTSP_SERVER_RETURN_CODE;

typedef void* MXRTSP_SERVER_HANDLE;

MXRTSP_SERVER_HANDLE mxrtsp_server_create(int rtsp_port ,int rtsp_max_connection);

MXRTSP_SERVER_RETURN_CODE mxrtsp_server_destroy();
int mxrtsp_method(struct ILibWebServer_Session *session, struct packetheader* header, char *bodyBuffer, int offset, int bodyBufferLength);


