#ifndef _PL_RC_RESTSERVICE_
#define _PL_RC_RESTSERVICE_

#ifdef __cplusplus
extern "C" {
#endif

#include <netinet/in.h>
#include <netinet/ip.h>

#include <common.h>
#include <pl_common.h>

#include "pl_rc.h"
#include "mongoose.h"

#define RC_REST_SERVER_BUF_SIZE 4096
typedef int SOCKET;

#if 0
typedef enum
{
	Dial_New_Request,
	Dial_Http_Error,
} RestService_Event_e;

typedef enum
{
	DIAL_Request_POST,
	DIAL_Request_GET,
	DIAL_Request_DELETE,
} RestService_RequestMethod_e;

struct socket
{
	SOCKET sock;
	struct sockaddr_in local_addr;
	struct sockaddr_in remote_addr;
};

typedef struct
{
	RestService_RequestMethod_e method;

	HCHAR 	*uri;
	HCHAR	*body;

	HINT32	content_size;

	struct sockaddr_in local_addr;
	struct sockaddr_in remote_addr;

} RestService_RequestInfo_t;

typedef struct
{
	struct socket client;
	RestService_RequestInfo_t request_info;

	HCHAR	buf[DIAL_REST_SERVER_BUF_SIZE];
	HUINT32	content_size;
	HUINT32	header_size;
	HUINT32	data_len;
} RestService_Connection_t;

typedef void *(*PL_DIAL_RestService_CB)(RestService_Event_e evt, RestService_Connection_t *conn, RestService_RequestInfo_t *request_info);

HERROR	PL_DIAL_RestService_SendMsssage(RestService_Connection_t *comm, HCHAR *buf);

HERROR	PL_DIAL_RestService_SendHttpError(RestService_Connection_t *conn, HUINT32 status, HCHAR *reason, HCHAR *fmt, ...);

HINT32 	PL_DIAL_RestService_GetServicePort(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif

HERROR 	PL_RC_RestService_Start(HCHAR *pszFriendlyName, struct RCClientCallbacks *client_cb);

HERROR	PL_RC_RestService_Stop(void);

HERROR	PL_RC_RestService_UnpairClient(void);

HERROR 	PL_RC_RestService_SetPowerStatus(HBOOL bOperating);

HERROR	PL_RC_RestService_SetFriendlyName( const HCHAR *pszFriendlyName);

#endif // _PL_RC_RESTSERVICE_