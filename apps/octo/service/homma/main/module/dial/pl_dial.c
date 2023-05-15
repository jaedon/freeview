/*************************************************************************************************************
	File            : pl_dial.h
	author          : IPM tipmsw1@humaxdigital.com
	comment         :
	date            :
	attention       :

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Humax) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/

#define	________________Header_Files_______________________________
#include "vkernel.h"

#include "pl_rest_svc.h"
#include "pl_dial.h"

#include "mxdial_server.h"

#include <netdb.h>
#include <netinet/in.h>

#define ________________Defines____________________________________

#define COOKIE_MAX 4096

#define APP_NAME_MAX 64
#define APP_URL_MAX 2048
#define ADDITIONAL_DATA_MAX 2048
#define	DIAL_DATA_MAX	4

#define SSDP_PORT (56790)
#define DIAL_PORT (56789)

#define APPS_URI 		"/apps/"
#define RUN_URI 		"/run"
#define DIAL_DATA_URI 	"/dialData"
#define HUMAX_APP_PREFIX "com.humaxtvportal"

#define APP_NAME_YOUTUBE "YouTube"
#define APP_NAME_NETFLIX "Netflix"
#define CORS_NETFLIX_SUFFIX ".netflix.com"

typedef struct
{
	HCHAR 	name[32];
	HCHAR	value[64];
} dial_data_t;

typedef struct
{
	HCHAR 			name[APP_NAME_MAX];
	PL_DIAL_Status 	state;

	HCHAR			szAdditionalData[ADDITIONAL_DATA_MAX];

	HCHAR			url[APP_URL_MAX];	// app startup url
#ifdef SUPPORT_DIAL_EXTENTION
	HBOOL			isHumaxApp;
#endif
} dial_app_t;

typedef struct
{
	HBOOL running;
	HCHAR 		ip_addr[20];
	HCHAR		mac_addr[20];
	HxList_t 	*pAppList;
	HULONG		lock;
	MXDIAL_SERVER_HANDLE handle;
	HCHAR friendlyName[1024];
} dial_server_t;

#define ________________Static_Variables___________________________
static dial_server_t s_server;
static struct DIALAppCallbacks * s_callbacks = NULL;
static HCHAR s_cookie[COOKIE_MAX] = {0,};

#define ________________Internal_Functions_________________________
static HCHAR * __url_encode(const HCHAR * in);
static int __check_origin(const HCHAR * origin, ...);
static int __check_netflix_cors(header_fields_t * fields);

dial_server_t * __server(void)
{
	return &s_server;
}

static void __lock(void)
{
	dial_server_t *context = __server();
	VK_SEM_Get(context->lock);
}

static void __unlock(void)
{
	dial_server_t *context = __server();
	VK_SEM_Release(context->lock);
}

static dial_app_t *__find_app(const HCHAR *name)
{
	dial_server_t * context = __server();
	HxList_t * list = HLIB_LIST_First(context->pAppList);
	dial_app_t * app = NULL;

	if (!context->running) {
		HxLOG_Error("[%s:%d] DIAL stopped\n", __func__, __LINE__);
		return NULL;
	}

	while(list) {
		dial_app_t * item = (dial_app_t *)HLIB_LIST_Data(list);
		if (item && !strcmp(item->name, name)) {
			app = item;
			break;
		}
		list = list->next;
	}

	return app;
}

HERROR  __sync_wait(HUINT32 ulWaitTick, const HCHAR * appname, PL_DIAL_Status changeFor)
{
	HUINT32     starttick;

	starttick = HLIB_STD_GetSystemTick();

	while ((HLIB_STD_GetSystemTick() - starttick) < ulWaitTick)
	{
		HBOOL done = FALSE;
		HBOOL failed = FALSE;
		dial_app_t * app = NULL;

		__lock();
		if ((app = __find_app(appname))) {
			if (app->state == changeFor) {
				done = TRUE;
			}
		} else {
			failed = TRUE;
		}
		__unlock();

		if (failed) {
			return ERR_FAIL;
		}

		if (done) {
			return ERR_OK;
		}

		HLIB_STD_TaskSleep(10);
	}

	return ERR_FAIL;

}

#ifdef SUPPORT_DIAL_EXTENTION

/**
 * left trim
 */
static char * __trim(char * s, int len) {
	int i = 0;
	while (*s != 0 && (len == -1 || ++i < len)) {
		if (*s != ' ' && *s != '\t' && *s != '\r' && *s != '\n') {
			break;
		}
		s++;
	}
	return s;
}

/**
 * Query app info to portal
 */
static int __query_portal_app(const HCHAR * mac, const HCHAR * package, HCHAR * out, int max) {

	HCHAR packet[4096 + COOKIE_MAX] = {0,};
	HCHAR portal[2048] = {0,};
	HCHAR host[1024] = {0,};
	HCHAR address[1024] = {0, };
	HCHAR port[10] = {0,};
	HCHAR * p;
	int result = -1;
	HCHAR cookie_part[COOKIE_MAX + 10] = {0,};

	HOMMA_GetPortalURL(portal, sizeof(portal));
	p = HxSTD_StrStr(portal, "http://");
	HxSTD_StrNCpy(host, (p ? p + 7 : portal), sizeof(host));
	p = HxSTD_StrStr(host, "/");
	if (p) {
		*p = 0;
	}
	p = HxSTD_StrStr(host, ":");
	HxSTD_StrNCpy(address, host, (p ? p - host : sizeof(address)));
	HxSTD_StrNCpy(port, (p ? p + 1 : "80"), sizeof(port));

	if (s_cookie[0] == 0) {
		cookie_part[0] = 0;
	} else {
		HxSTD_snprintf(cookie_part, sizeof(cookie_part), "Cookie: %s\r\n", s_cookie);
	}

	HxSTD_snprintf(packet, sizeof(packet),
				   "GET /api/DialLookup?deviceID=%s&package=%s HTTP/1.1\r\n"
				   "User-Agent: HMS/Dial\r\n"
				   "Host: %s\r\n"
				   "Accept: */*\r\n"
				   "%s"
				   "\r\n",
				   mac, package, host, cookie_part);

	HxLOG_Debug("{packet:%s}\n", packet);

	/**
	 * http request & response handling
	 */

	{

		//struct sockaddr_in targetAddr;
		SOCKET sock;
		char c;
		int len;
		char respHeader[4096] = {0,};
		int contentLength = -1;
		int respHeaderPosition = -1;
		char respContent[4096] = {0,};
		int respContentPosition = -1;
		unsigned char headerPart = 1;
		char line[1024] = {0,};
		int lineCount = 0;
		struct timeval timeout;
		//int errCode = 0;

		struct addrinfo hints, * res;


		/**
		 * dns lookup
		 */

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		if (getaddrinfo(address, port, &hints, &res) < 0) {
			HxLOG_Error("getaddrinfo() error\n");
			return -1;
		}

		sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sock < 0) {
			HxLOG_Error("socket() error\n");
			freeaddrinfo(res);
			return -1;
		}

		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0){
			HxLOG_Error("setsockopt() error\n");
			freeaddrinfo(res);
			return -1;
		}

		if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
			HxLOG_Error("connect() error\n");
			close(sock);
			freeaddrinfo(res);
			return -1;
		}

		/**
		 * send request packet
		 */
		if ((len = write(sock, packet, HLIB_STD_StrNLen(packet, sizeof(packet)))) < 0) {
			HxLOG_Error("write() error\n");
			freeaddrinfo(res);
			close(sock);
			return -1;
		}

		/**
		 * receive response packet and parse http header
		 */
		while ((len = read(sock, &c, 1)) > 0) {

			if (headerPart) {

				if (respHeaderPosition >= (int)(sizeof(respHeader) - 1)) {
					// header overflow
					break;
				}

				respHeader[++respHeaderPosition] = c;

				if (HxSTD_StrStr(respHeader, "\r\n\r\n")) {

					/**
					 * parse header
					 */

					headerPart = 0;

					/**
					 * extract status code and content-length
					 */

					lineCount = 0;
					p = __trim(respHeader, -1);
					while (p) {
						char * e = HxSTD_StrStr(p, "\r\n");
						p = __trim(p, e - p);

						if (!e) {
							break;
						}

						if (p == e) {
							break;
						}

						HxSTD_StrNCpy(line, p, e - p);
						line[e - p] = 0;
						lineCount++;

						if (lineCount == 1) {

							/*
							 * parse first line
							 */

							// HTTP protocol (e.g. HTTP/1.1)
							char * vs = line;
							char * ve = HxSTD_StrStr(vs, " ");

							// status code (e.g. 200)
							vs = __trim(ve, -1);
							ve = HxSTD_StrStr(vs, " ");
							result = atoi(vs);

							// reason (e.g. OK)
							vs = __trim(ve, -1);
							ve = HxSTD_StrStr(vs, "\r\n");

						} else {

							/*
							 * parse header fields
							 */

							char * CONTENT_LENGTH_STR = "CONTENT-LENGTH";

							// name
							char * vs = line;
							char * ve = HxSTD_StrStr(vs, ":");
							char name[512] = {0,};

							HxSTD_StrNCpy(name, vs, ve - vs);
							HLIB_STD_StrUpper(name);

							if (!HxSTD_StrNCmp(name, CONTENT_LENGTH_STR, sizeof(name))) {

								// value (content-length)
								vs = __trim(ve + 1, -1);
								contentLength = atoi(vs);
							}
						}

						p = e + 2;
					}

					/**
					 * if content-length <== 0, stop receiving
					 */
					if (contentLength <= 0) {
						break;
					}

					/**
					 * check if content length too big
					 */
					if (contentLength >= sizeof(respContent)) {
						result = -1;
						break;
					}
				}
			} else {

				/**
				 * fill content data
				 */

				respContent[++respContentPosition] = c;

				if (respContentPosition >= contentLength - 1) {
					HxSTD_snprintf(out, max, "%s", respContent);
					break;
				}
			}
		}

		if (len < 0) {
			result = -1;
		}

		close(sock);
		freeaddrinfo(res);
	}

	return result;
}

#endif	// SUPPORT_DIAL_EXTENTION

static HBOOL __query_and_register(const HCHAR * appname)
{

#ifdef SUPPORT_DIAL_EXTENTION

	dial_server_t 	*context	= __server();
	dial_app_t		*app		= NULL;

	/**
	 * If the app name starts with HUMAX APP PREFIX, query to the portal
	 */
	if (!strncmp(appname, HUMAX_APP_PREFIX, strlen(HUMAX_APP_PREFIX))) {

		HCHAR app_url[4096] = {0,};
		// int ret = __query_portal_app(
		//     "08:eb:74:b0:b0:eb",
		//     "com.humaxtvportal.apps.humax.quickflix",
		//     app_url,
		//     sizeof(app_url)); // for TESTING
		int ret = __query_portal_app(context->mac_addr, appname, app_url, sizeof(app_url));

		HxLOG_Error("__query_portal_app() result: %d\n", ret);

		if (ret == 200) {
			// register the app
			PL_DIAL_register_app(appname);
			app = __find_app(appname);
			if (app) {
				HxSTD_snprintf(app->url, sizeof(app->url), "%s", app_url);
				HxLOG_Debug("app url : %s\n", app->url);
				return ERR_OK;
			}
			return ERR_FAIL;
		} else {
			PL_DIAL_unregister_app(appname);
			app = NULL;
			return ERR_FAIL;
		}
	}
#endif	// SUPPORT_DIAL_EXTENTION

	return ERR_OK;
}

static void __send_app_start_response_ok(RestService_Connection_t *conn, const HCHAR * appname, RestService_RequestInfo_t * request_info) {
	HCHAR address[INET6_ADDRSTRLEN] = {0, };
	HCHAR response[4096] = {0, };
	const struct sockaddr_in *addr = (struct sockaddr_in *)&request_info->local_addr;
	inet_ntop(AF_INET, &addr->sin_addr, address, sizeof(address));
	HxSTD_snprintf(response, sizeof(response),
				   "HTTP/1.1 201 Created\r\n"
				   "Content-Type: text/plain\r\n"
				   "Content-Length: 0\r\n"
				   "Location: http://%s:%d/apps/%s/run\r\n"
				   "\r\n", address, PL_DIAL_get_port(), appname);

	PL_DIAL_RestService_SendMsssage(conn, response);
}

static void __send_app_stop_response_ok(RestService_Connection_t *conn, const HCHAR * appname, RestService_RequestInfo_t * request_info) {
	
	const HCHAR * response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/plain\r\n"
		"Content-Length: 0\r\n"
		"\r\n";

	PL_DIAL_RestService_SendMsssage(conn, response);
}

static void __app_start(RestService_Connection_t *conn, RestService_RequestInfo_t *request_info, const HCHAR *appname)
{
	HCHAR body[DIAL_MAX_PAYLOAD+2] = {0,};
	dial_app_t *app = NULL;
	HINT32 nread = 0;
	PL_DIAL_Status _state;
	HCHAR _url[APP_URL_MAX] = {0,};

	if(conn == NULL || request_info == NULL || appname == NULL)
	{
		return;
	}

	__lock();
	__query_and_register(appname);
	app = __find_app(appname);
	if (app) {
		_state = app->state;
		HxSTD_snprintf(_url, sizeof(_url), "%s", app->url);
	}
	__unlock();

	if (app == NULL)
	{
		PL_DIAL_RestService_SendHttpError(conn, 404, "Not Found", "Not Found");
	}
	else
	{
		if (request_info->body == NULL || HxSTD_StrLen(request_info->body) > DIAL_MAX_PAYLOAD)
		{
			PL_DIAL_RestService_SendHttpError(conn, 413, "413 Request Entity Too Large",
											  "413 Request Entity Too Large");
		}
		else
		{
			if (!HxSTD_StrCmp(appname, APP_NAME_NETFLIX)) {
				if (__check_netflix_cors(&(request_info->fields)) != 0) {
					PL_DIAL_RestService_SendHttpError(conn, 403, "Forbidden", "Forbidden");
					return;
				}

				if (_state == DIAL_Status_Running) {
					__send_app_start_response_ok(conn, appname, request_info);
					return;
				}
			}

			nread = HxSTD_StrLen(request_info->body);
			if (nread > sizeof(body) - 2) {
				nread = sizeof(body) - 2;
			}

			HxSTD_StrNCpy(body, request_info->body, nread);
			HxLOG_Debug("DIAL Launch - body: \"%s\"\n", body);
			s_callbacks->start_cb(appname, _url, body, nread);

			if (__sync_wait(60000, appname, DIAL_Status_Running) == ERR_OK) {
				HxLOG_Error("Launch succeeded (%s)\n", appname);
				__send_app_start_response_ok(conn, appname, request_info);
			} else {
				HxLOG_Error("Launch failed (%s)\n", appname);
				PL_DIAL_RestService_SendHttpError(conn, 503, "Service Unavailable", "Service Unavailable");
			}
		}
	}
}

static void __app_status(RestService_Connection_t *conn, RestService_RequestInfo_t *request_info, const HCHAR *appname)
{
	dial_server_t 	*context	= __server();
	dial_app_t 		*app = NULL;
	dial_app_t _app_clone;

	__lock();
	__query_and_register(appname);
	app = __find_app(appname);
	if (app) {
		memcpy(&_app_clone, app, sizeof(dial_app_t));
	} else {
		memset(&_app_clone, 0, sizeof(dial_app_t));
	}
	__unlock();
	
	if (!app)
	{
		PL_DIAL_RestService_SendHttpError(conn, 404, "Not Found", "Not Found");
	}
	else
	{
		HCHAR response[4096] = {0, };
		HCHAR payload[4096] = {0,};
		HCHAR wolMac[256] = {0,};
		HBOOL canStop = (!HxSTD_StrCmp(appname, APP_NAME_NETFLIX) ? TRUE : FALSE);
		HBOOL needWolMac = (!HxSTD_StrCmp(appname, APP_NAME_YOUTUBE) ? TRUE : FALSE);

		HxLOG_Error("[%s:%d] Application(%s) Status Request - %s !! \n", __func__, __LINE__,
					appname, _app_clone.state ? "running" : "stopped");

		HxSTD_snprintf(wolMac, sizeof(wolMac), "wolMac=\"%s\" ", context->mac_addr);

		HxSTD_snprintf(payload, sizeof(payload), "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
					   "<service xmlns=\"urn:dial-multiscreen-org:schemas:dial\" dialVer=\"1.7\">\r\n"
					   "  <name>%s</name>\r\n"
					   "  <options allowStop=\"%s\" %s/>\r\n"
					   "  <state>%s</state>\r\n"
					   "%s"
					   "%s"
					   "</service>\r\n",
					   _app_clone.name,
					   (canStop ? "true":"false"),
					   (needWolMac ? wolMac : ""),
					   (_app_clone.state ? "running" : "stopped"),
					   (_app_clone.szAdditionalData[0] != 0 ? _app_clone.szAdditionalData : ""),
					   (_app_clone.state ==  DIAL_Status_Stop ? "" : "  <link rel=\"run\" href=\"run\"/>\r\n"));

		HxSTD_snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\n"
					   "Content-Type: application/xml\r\n"
					   "Content-Length: %d\r\n"
					   "\r\n"
					   "%s",
					   HxSTD_StrLen(payload), payload);

		PL_DIAL_RestService_SendMsssage(conn, response);

		HxLOG_Debug("[%s:%d] %s \n", __func__, __LINE__, appname, response);
	}

}

static void __app_stop(RestService_Connection_t *conn, RestService_RequestInfo_t *request_info, const HCHAR *appname)
{
	dial_app_t *app = NULL;

	__lock();
	app = __find_app(appname);
	__unlock();

	if( app == NULL )
	{
		PL_DIAL_RestService_SendHttpError(conn, 404, "Not Found", "Not Found");
	}
	else
	{
		// If the app name is not Netflix
		if (HxSTD_StrCmp(appname, APP_NAME_NETFLIX)) {
			PL_DIAL_RestService_SendHttpError(conn, 503, "Service Unavailable", "Service Unavailable");
			return;
		}

		if (!HxSTD_StrCmp(appname, APP_NAME_NETFLIX)) {
			if (__check_netflix_cors(&(request_info->fields)) != 0) {
				PL_DIAL_RestService_SendHttpError(conn, 403, "Forbidden", "Forbidden");
				return;
			}
		}

		if (app->state == DIAL_Status_Stop) {
			__send_app_stop_response_ok(conn, appname, request_info);
			return;
		}

		s_callbacks->stop_cb(appname);
		if (__sync_wait(30000, appname, DIAL_Status_Stop) == ERR_OK) {
			HxLOG_Error("Stop succeeded (%s)\n", appname);
			__send_app_stop_response_ok(conn, appname, request_info);
		} else {
			HxLOG_Error("Stop failed (%s)\n", appname);
			PL_DIAL_RestService_SendHttpError(conn, 503, "Service Unavailable", "Service Unavailable");
		}
	}

}
#define ADDITIONAL_DATA_STRART_STR		"  <additionalData>\r\n"
#define ADDITIONAL_DATA_END_STR			"  </additionalData>\r\n"

/**
 * handle additional data
 */
static void __app_info(RestService_Connection_t *conn, const HCHAR *appname, const HCHAR *info)
{
	HBOOL exists = FALSE;
	dial_app_t *app = NULL;

	__lock();
	exists = __find_app(appname) != NULL;
	__unlock();

	HxLOG_Print("[%s:%d] App(%s) Info(%s) \n", __func__, __LINE__, appname, info);

	if( exists )
	{
		HUINT32 i = 0;
		HCHAR 	*buf = (HCHAR *)info;
		HCHAR 	*token = NULL, *pPtr = NULL, szData[128] = {0,};
		dial_data_t data[10];
		HUINT32 data_cnt = 0;
		HCHAR _additionaldata[ADDITIONAL_DATA_MAX] = {0,};

		HxSTD_memset(data, 0, sizeof(dial_data_t) * 10);

		token = HxSTD_strtok_r( buf, "&=", &pPtr);

		if(token != NULL) {
			HxSTD_snprintf(data[data_cnt].name, sizeof(data[data_cnt].name), "%s", token);
		} else {
			HxLOG_Error("[%s:%d] additional data is null. \n", __func__, __LINE__);
			return;
		}

		while( token != NULL )
		{
			token = HxSTD_strtok_r( NULL, "&=", &pPtr);
			if( token != NULL)
			{
				if(data[data_cnt].name[0] != '\0') {
					HxSTD_snprintf(data[data_cnt].value, sizeof(data[data_cnt].value), "%s", token);
					data_cnt += 1;
				} else {
					HxSTD_snprintf(data[data_cnt].name, sizeof(data[data_cnt].name), "%s", token);
				}
			}
		}

		HxSTD_memset(_additionaldata, 0, sizeof(app->szAdditionalData));
		HxSTD_StrNCpy(_additionaldata, ADDITIONAL_DATA_STRART_STR, HxSTD_StrLen(ADDITIONAL_DATA_STRART_STR));
		for(i = 0; i < data_cnt; i++)
		{
			HxSTD_snprintf(szData, sizeof(szData), "    <%s>%s</%s>\r\n", data[i].name, data[i].value, data[i].name);
			HxSTD_StrNCat(_additionaldata, szData, HxSTD_StrLen(szData));
			HxSTD_memset(szData, 0x00, sizeof(szData));
		}
		HxSTD_StrNCat(_additionaldata, ADDITIONAL_DATA_END_STR, HxSTD_StrLen(ADDITIONAL_DATA_END_STR));

		__lock();
		app = __find_app(appname);
		if (app) {
			HxSTD_snprintf(app->szAdditionalData, ADDITIONAL_DATA_MAX, "%s", _additionaldata);
		}
		__unlock();

		{
			// 200 OK
			const HCHAR * response = 
				"HTTP/1.1 200 OK\r\n"
				"Content-Length: 0\r\n"
				"Access-Control-Allow-Origin: *\r\n"
				"\r\n";

			PL_DIAL_RestService_SendMsssage(conn, response);
		}
	}
	else
	{
		HxLOG_Error("[%s:%d] cann't find app(%s). \n", __func__, __LINE__, appname);
		PL_DIAL_RestService_SendHttpError(conn, 404, "Not Found", "Not Found");
	}
}

static void *__request_cb(RestService_Event_e evt, RestService_Connection_t *conn, RestService_RequestInfo_t *request_info)
{

	HxLOG_Error("[%s:%d]request_info->uri: %s\n", __func__, __LINE__, request_info->uri);

	switch(evt)
	{
		case  Dial_New_Request:
			{
				if(request_info->uri == NULL)
					return NULL;

				/*
				 * additionalData request will be came via POST or GET
				 * e.g. POST or GET (ideal)
				 * http://localhost:8080/dialData?app=YouTube
				 * screenId=screen123&authToken=token123
				*/
				if (!strncmp(request_info->uri, DIAL_DATA_URI,  sizeof(DIAL_DATA_URI) -1))
				{
					HUINT32 i = 0;
					//HUINT32 size = 0;
					HCHAR appname[256] = {0,};
					HCHAR * buf = NULL;
					const HCHAR * prefix = DIAL_DATA_URI "?app=";

					if (!strncmp(request_info->uri, prefix, strlen(prefix))) {
						HCHAR * p = request_info->uri + strlen(prefix);
						for (i = 0; *p != '&' && *p != 0 && i < sizeof(appname); i++, p++) {
							appname[i] = *p;
						}

						if (request_info->method == DIAL_Request_POST) {
							buf = request_info->body;
						} else if (request_info->method == DIAL_Request_GET) {
							buf = p+1;
						} else {
							PL_DIAL_RestService_SendHttpError(conn, 501, "Not Implemented", "Not Implemented");
							return NULL;
						}

						HxLOG_Debug("[%s:%d] appname:%s/buf:%s\n", __func__, __LINE__, appname, buf);

						__app_info(conn, appname, buf);
					} else {
						PL_DIAL_RestService_SendHttpError(conn, 400, "Bad Request", "Bad Request");
						return NULL;
					}
				}

				if (!strncmp(request_info->uri + strlen(request_info->uri)-4, RUN_URI, strlen(RUN_URI)))
				{
					HCHAR appname[256] = {0,};
					strncpy (appname, request_info->uri + strlen(APPS_URI),
							 ((strlen(request_info->uri)-4) - (sizeof(APPS_URI)-1)));

					if (appname[0] != '\0' && request_info->method == DIAL_Request_DELETE) {
						__app_stop(conn, request_info, appname);
					} else {
						PL_DIAL_RestService_SendHttpError(conn, 501, "Not Implemented", "Not Implemented");
					}
				}
				else if (!strncmp(request_info->uri, APPS_URI, sizeof(APPS_URI) - 1))
				{
					const HCHAR *appname;
					appname = request_info->uri + sizeof(APPS_URI) - 1;

					switch(request_info->method)
					{
						case DIAL_Request_POST:
							__app_start(conn, request_info, appname);
							break;
						case DIAL_Request_GET:
							__app_status(conn, request_info, appname);
							break;
						default:
							PL_DIAL_RestService_SendHttpError(conn, 501, "Not Implemented", "Not Implemented");
							break;
					}
				}
				else
				{
					PL_DIAL_RestService_SendHttpError(conn, 404, "Not Found", "Not Found");
				}

				return "done";
			}
			return NULL;

		case Dial_Http_Error:
			break;
	}

	return NULL;
}

#define ________________Public_Interfaces__________________________
#define	PL_DIAL_DEFAULT_UDN_PREFIX			"1E96FF1A-6771-2F44-D0C5-"
HERROR	PL_DIAL_start(HCHAR *pszFriendlyName)
{
	HCHAR 			szAppUrl[256] = {0, };
	HCHAR			szUdn[48] = {0, };
	HCHAR			szFriendlyName[128] = {0, };
	dial_server_t 	*context	= __server();
	PL_COMMON_NetworkInfo_t	net_info;
	MXDIAL_SERVER_INFO_t server_info;
	PL_COMMON_ProductInfo_t	product_info;
	HxLOG_Error("[%s:%d] start(+)\n", __func__, __LINE__);

	HOM_MEMZERO(context, sizeof(dial_server_t));
	HOM_MEMZERO(&server_info, sizeof(MXDIAL_SERVER_INFO_t));

	PL_COMMON_GetProductInfo(&product_info);
	PL_COMMON_GetDefaultNetwork(&net_info);

	HxSTD_snprintf(szAppUrl, sizeof(szAppUrl), "http://%s:%d/apps/", net_info.szIp, DIAL_PORT);

	server_info.manufacturer = product_info.szManufacturer;
	server_info.manufacturerURL = product_info.szManufacturerUrl;
	server_info.corporation = product_info.szManufacturer;

	server_info.modelName = product_info.szModelName;
	server_info.modelNumber = product_info.szModelNumber;
	server_info.modelDescription = product_info.szModelDescription;
	server_info.modelURL = product_info.szModelUrl;
	server_info.serialNumber = product_info.szSerialNumber;

	if(pszFriendlyName == NULL)
		HxSTD_snprintf(szFriendlyName, sizeof(szFriendlyName), "%s %s %s", server_info.modelName, "DIAL SERVER", net_info.szIp);
	else
		HxSTD_snprintf(szFriendlyName, sizeof(szFriendlyName), "%s", pszFriendlyName);

	HxSTD_snprintf(szUdn, sizeof(szUdn), "%s%s", PL_DIAL_DEFAULT_UDN_PREFIX, net_info.szMacAddr);
	HxSTD_snprintf(context->ip_addr, sizeof(context->ip_addr), "%s", net_info.szIp);
	HxSTD_snprintf(context->mac_addr, sizeof(context->mac_addr), "%c%c:%c%c:%c%c:%c%c:%c%c:%c%c",
			net_info.szMacAddr[0], net_info.szMacAddr[1], net_info.szMacAddr[2], net_info.szMacAddr[3],
			net_info.szMacAddr[4], net_info.szMacAddr[5], net_info.szMacAddr[6], net_info.szMacAddr[7],
			net_info.szMacAddr[8], net_info.szMacAddr[9], net_info.szMacAddr[10], net_info.szMacAddr[11]);

	server_info.friendlyName = szFriendlyName;
	HxSTD_snprintf(context->friendlyName, sizeof(context->friendlyName), "%s", szFriendlyName);
	server_info.UDN = szUdn;

	HxLOG_Debug("DIAL FN: %s\n", context->friendlyName);
	HxLOG_Debug("DIAL IP: %s\n", context->ip_addr);

	server_info.applicationURL = szAppUrl;
	server_info.upnp_port = SSDP_PORT;

	context->handle = mxDialServer_createServer(&server_info);
	if(context->handle == NULL)
	{
		HxLOG_Error("[%s:%d] Error> Mxdial handle is null !!! \n", __func__, __LINE__);
		return ERR_FAIL;
	}

	PL_DIAL_RestService_Start(&__request_cb, DIAL_PORT);

	mxDialServer_startServer(context->handle);

	(void)VK_SEM_Create(&context->lock, "dial_server_sema", VK_SUSPENDTYPE_FIFO);

	context->running = TRUE;
	HxLOG_Error("[%s:%d] start(-)\n", __func__, __LINE__);

	return ERR_OK;
}

void PL_DIAL_stop(void)
{
	dial_server_t *context = __server();
	HxLOG_Error("[%s:%d] stop(+)\n", __func__, __LINE__);

	context->running = FALSE;

	if (context->pAppList)
	{
		HLIB_LIST_RemoveAllFunc(context->pAppList, HLIB_STD_MemFree_CB);
		context->pAppList = NULL;
	}

	if (context->handle != NULL)
	{
		mxDialServer_destroyServer(context->handle);
		context->handle = NULL;
	}

	PL_DIAL_RestService_Stop();

	(void)VK_SEM_Destroy(context->lock);
	context->lock = 0;
	HxLOG_Error("[%s:%d] stop(-)\n", __func__, __LINE__);
}

HERROR 	PL_DIAL_SetApplicationURL(void)
{
	dial_server_t 	*context = __server();

	HxLOG_Debug("[%s:%d] ++\n", __func__, __LINE__);

	if (context->handle == NULL)
	{
		HxLOG_Error("[%s:%d] error -- no handler\n", __func__, __LINE__);
		return ERR_FAIL;
	}
	else
	{
		PL_COMMON_ProductInfo_t product_info;
		HCHAR szFriendlyName[128] = {0, };
		HCHAR szAppUrl[256] = {0, };
		PL_COMMON_NetworkInfo_t	net_info;

		PL_COMMON_GetDefaultNetwork(&net_info);

		HxSTD_snprintf(szAppUrl, sizeof(szAppUrl), "http://%s:%d/apps/", net_info.szIp, DIAL_PORT);
		HxLOG_Debug("[%s:%d] Set application url to \"%s\"\n", __func__, __LINE__, szAppUrl);

		mxDialServer_setApplicationURL(context->handle, szAppUrl);

		// change friendlyName with the new IP address
		PL_COMMON_GetProductInfo(&product_info);

		HxSTD_snprintf(szFriendlyName, sizeof(szFriendlyName), "%s %s %s", product_info.szModelName, "DIAL SERVER", net_info.szIp);

		PL_DIAL_SetFriendlyName(szFriendlyName);
	}

	HxLOG_Debug("[%s:%d] OK --\n", __func__, __LINE__);

	return ERR_OK;
}


HERROR	PL_DIAL_SetFriendlyName( const HCHAR *pszFriendlyName)
{

	dial_server_t 	*context = __server();

	HxLOG_Debug("[%s:%d] ++\n", __func__, __LINE__);

	if (context->handle == NULL)
	{
		HxLOG_Error("[%s:%d] error -- no handler\n", __func__, __LINE__);
		return ERR_FAIL;
	}

	if(HxSTD_StrEmpty(pszFriendlyName))
	{
		HxLOG_Error("[%s:%d] Error> FriendlyName is empty. \n", __func__, __LINE__);
		return ERR_FAIL;
	}

	mxDialServer_setFriendlyName(context->handle, pszFriendlyName);
	HxSTD_snprintf(context->friendlyName, sizeof(context->friendlyName), "%s", pszFriendlyName);
	HxLOG_Debug("[%s:%d] OK -- set to \"%s\"\n", __func__, __LINE__	, pszFriendlyName);

	return ERR_OK;
}

HUINT32	PL_DIAL_get_port(void)
{
	return PL_DIAL_RestService_GetServicePort();
}

HERROR	PL_DIAL_registerAppHandler(struct DIALAppCallbacks * cbs) {
	s_callbacks = cbs;
	return ERR_OK;
}

HERROR	PL_DIAL_register_app(const HCHAR *appname)
{
	dial_server_t 	*context = __server();
	dial_app_t 		*ptr, *app;
	HERROR err = ERR_FAIL;

	if (!context->running) {
		HxLOG_Error("[%s:%d] DIAL stopped\n", __func__, __LINE__);
		return err;
	}

	__lock();
	ptr = __find_app(appname);
	if (ptr == NULL)
	{
		app = HLIB_STD_MemCalloc(sizeof(dial_app_t)); // initialized with '0'
		HxSTD_snprintf(app->name, sizeof(app->name), "%s", appname);
		app->state = DIAL_Status_Stop;
		context->pAppList = HLIB_LIST_Append(context->pAppList, (void *)app);

		err = ERR_OK;
	}
	__unlock();

	return err;
}

HERROR	PL_DIAL_unregister_app(const HCHAR *appname)
{
	dial_server_t 	*context = __server();
	dial_app_t 		*app;
	HERROR err = ERR_FAIL;

	if (!context->running) {
		HxLOG_Error("[%s:%d] DIAL stopped\n", __func__, __LINE__);
		return err;
	}

	__lock();
	app = __find_app(appname);
	if (app != NULL)
	{
		context->pAppList = HLIB_LIST_Remove(context->pAppList, app);
		HOM_MEMFREE(app);
		err = ERR_OK;
	}
	__unlock();

	return err;
}

HERROR	PL_DIAL_update_state_app(const HCHAR *appname, PL_DIAL_Status status)
{
	dial_app_t 		*app;
	HERROR err = ERR_FAIL;

	HxLOG_Error("App state changed: %s -> %s\n",
			appname, (status == DIAL_Status_Running ? "running" : "stopped"));

	__lock();
	app = __find_app(appname);
	if (app != NULL) {
		app->state = status;
		err = ERR_OK;
	}
	__unlock();

	return err;
}

HERROR	PL_DIAL_setCookie(const char * cookie) {
	HxSTD_snprintf(s_cookie, sizeof(s_cookie), "%s", cookie);
	HxLOG_Debug("cookie arrived: {%s}\n", cookie);
	HxLOG_Debug("copied cookies: {%s}\n", s_cookie);
	return ERR_OK;
}

HERROR PL_DIAL_GetFriendlyName(HCHAR * pFriendlyName, HUINT32 size) {
	dial_server_t * ctx = __server();
	HxSTD_snprintf(pFriendlyName, size, "%s", ctx->friendlyName);
	return ERR_OK;
}

HERROR PL_DIAL_GetAdditionalDataUrl(HCHAR * pAdditionalDataUrl, HUINT32 size, HCHAR * appName) {
	HCHAR url[2048] = {0,};
	HCHAR * encoded = NULL;
	HxSTD_snprintf(url, sizeof(url), "http://localhost:%d%s?app=%s", DIAL_PORT, DIAL_DATA_URI, appName);
	encoded = __url_encode(url);
	HxSTD_snprintf(pAdditionalDataUrl, size, "%s", encoded);
	HOM_MEMFREE(encoded);

	return ERR_OK;
}


static HCHAR * __url_encode(const HCHAR * in) {

	HCHAR * encbuf = NULL;
	const HCHAR * p = NULL;
	HCHAR * e = NULL;
	int len = 0;

	if (!in) {
		return NULL;
	}

	len = HxSTD_StrLen(in) * 3;

	encbuf = (HCHAR*)HLIB_STD_MemCalloc(len + 1);
	if (!encbuf) {
		return NULL;
	}

	HxSTD_MemSet(encbuf, 0, len + 1);

	p = in;
	e = encbuf;
	while (*p) {
		const HCHAR c = *(p++);

		if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
			*e++ = c;
		} else {
			HCHAR hex[3] = {0,};
			HxSTD_snprintf(hex, sizeof(hex), "%02X", c);
			*e++ = '%';
			*e++ = hex[0];
			*e++ = hex[1];
		}
	}

	return encbuf;
}

/*
 * check origin ends with an item of provided suffix items
 * return -1 for invalid, 0 for ignore, 1, for success
 */
static int __check_origin(const HCHAR * origin, ...) {

	const HCHAR * const CHECK_PROTOS[] = {"http:", "https:", "file:"};
	const int PROTO_COUNT = 3;
	va_list vl;
	HCHAR * suffix = NULL;
	int len_origin, i;
	int need_check = 0;

	if (origin == NULL || !origin[0]) {
		HxLOG_Debug("origin emtpy: ignore\n");
		return 0;
	}

	len_origin = strlen(origin);
	HxLOG_Debug("origin: %s\n", origin);

	for (i = 0; i < PROTO_COUNT; i++) {
		if (!strncmp(origin, CHECK_PROTOS[i], strlen(CHECK_PROTOS[i]) -1)) {
			need_check = 1;
			break;
		}
	}

	if (!need_check) {
		HxLOG_Debug("not matched to any proto: ignore\n");
		return 0;
	}

	va_start(vl, origin);

	while ((suffix = va_arg(vl, HCHAR *)) != NULL) {

		int len_suffix = strlen(suffix);

		HxLOG_Debug("suffix: %s(%d)\n", suffix, len_suffix);

		if (len_suffix > len_origin) {
			continue;
		}

		if (!strncmp(origin + (len_origin - len_suffix), suffix, len_suffix)) {
			HxLOG_Debug("matched/[%s/%s]\n", origin, suffix);
			va_end(vl);
			return 1;
		}
	}

	va_end(vl);

	return -1;
}

/*
 * check netflix cors
 */
static int __check_netflix_cors(header_fields_t * fields) {

	HCHAR * host;
	HCHAR * origin;

	if (!fields) {
		HxLOG_Error("header fields null\n");
		return -1;
	}

	host = PL_DIAL_GetHeaderField(fields, "Host");
	origin = PL_DIAL_GetHeaderField(fields, "Origin");

	HxLOG_Debug("check_netflix_cors/Host:%s/Origin:%s\n", host, origin);

	if (host == NULL || HxSTD_StrLen(host) == 0 || __check_origin(origin, CORS_NETFLIX_SUFFIX, NULL) < 0) {
		return -1;
	}

	return 0;
}

