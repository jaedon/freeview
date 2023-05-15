#ifndef _PL_DIAL_RESTSERVICE_
#define _PL_DIAL_RESTSERVICE_ 

#ifdef __cplusplus
extern "C" {
#endif

#include <netinet/in.h>
#include <netinet/ip.h>

#include <common.h>
#include <pl_common.h>

#define DIAL_REST_SERVER_BUF_SIZE (4096 * 2)

typedef int SOCKET;

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

typedef struct _header_field_t {
	HCHAR name[100];
	HCHAR value[128];
	struct _header_field_t * next;
} header_field_t;

typedef struct _header_fields_t {
	header_field_t * first;
} header_fields_t;

typedef struct 
{
	RestService_RequestMethod_e method;

	HCHAR 	*uri;
	HCHAR	*body;

	header_fields_t fields;

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

HERROR 	PL_DIAL_RestService_Start(PL_DIAL_RestService_CB cb, HUINT32 port);
HERROR	PL_DIAL_RestService_Stop(void);
HERROR	PL_DIAL_RestService_SendMsssage(RestService_Connection_t *comm, const HCHAR *buf);
HERROR	PL_DIAL_RestService_SendHttpError(RestService_Connection_t *conn, HUINT32 status, const HCHAR *reason, const HCHAR *fmt, ...);
HINT32 	PL_DIAL_RestService_GetServicePort(void);
HCHAR * PL_DIAL_GetHeaderField(header_fields_t * fields, const HCHAR * name);
	
#ifdef __cplusplus
}
#endif // __cplusplus

#endif
