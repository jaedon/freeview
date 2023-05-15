/*************************************************************************************************************
	File            : pl_common.h
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

#define ________________Typedef____________________________________

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

typedef struct
{
	SOCKET            	local_socket;
	struct sockaddr_in 	local_address;

	struct socket 		queue[20];
	HINT32				current_index;
	HINT32				last_index;
	HULONG				lock;
	HBOOL				flag_stop;
	HINT32				task_count;

	PL_DIAL_RestService_CB callback;
} rest_service_context_t;

/* HTTP PARSER */
typedef void (*name_value_handler)(const HCHAR * name, const HCHAR * value, void * userData);
static HERROR __parse_header_fields(HCHAR * str, name_value_handler handler, void * userData);
static void __remove_header_fields(header_fields_t * fields);
static HCHAR * __get_header_field(header_fields_t * fields, const HCHAR * name);
static void __make_name_value_list(const HCHAR * name, const HCHAR * value, void * userData);

#define ________________Static_Variables___________________________
static rest_service_context_t	s_pst_context;


#define ________________Internal_Functions_________________________
static rest_service_context_t *__context(void)
{
	return &s_pst_context;
}

static void __lock(void)
{
	rest_service_context_t *ctx = __context();
	VK_SEM_Get(ctx->lock);
}

static void __unlock(void)
{
	rest_service_context_t *ctx = __context();
	VK_SEM_Release(ctx->lock);
}

static HUINT32	__check_http_header_size(const HCHAR *buf, HUINT32 buflen)
{
	const HCHAR *s, *e;
	HINT32 len = 0;

	for (s = buf, e = s + buflen - 1; len <= 0 && s < e; s++)
	{
		if (s[0] == '\n' && s[1] == '\n')
		{
			len = (HINT32) (s - buf) + 2;
		}
		else if (s[0] == '\n' && &s[1] < e && s[1] == '\r' && s[2] == '\n')
		{
			len = (HINT32) (s - buf) + 3;
		}
	}

	return len;
}

static HERROR	parse_http_request(HCHAR * buf, RestService_RequestInfo_t * pInfo)
{
	HCHAR data[4096] = {0, };
	HCHAR * token, * pPtr;

	while (*buf != '\0' && isspace(* (unsigned char *) buf))
	{
		buf++;
	}

	HxSTD_snprintf(data, sizeof(data), "%s", buf);

	token = HxSTD_strtok_r(data, " ", &pPtr);
	if (token != NULL)
	{
		HxLOG_Error("parse_http_request::Request Method: %s\n", token);
		if (!strcmp(token, "GET")) {
			pInfo->method = DIAL_Request_GET;
		} else if (!strcmp(token, "POST")) {
			pInfo->method = DIAL_Request_POST;
	 	} else if (!strcmp(token, "DELETE")) {
			pInfo->method = DIAL_Request_DELETE;
		} else {
			return ERR_FAIL;
		}
	}

	token = HxSTD_strtok_r(NULL, " ", &pPtr);
	if (token != NULL)
	{
		pInfo->uri = HLIB_STD_StrDup(token);
	}

	__parse_header_fields(buf, __make_name_value_list, (void*)&(pInfo->fields));

	HxSTD_snprintf(data, sizeof(data), "%s", buf);
	pPtr = HxSTD_StrStr(data, "Content-Length: ");
	if (pPtr != NULL) {
		pInfo->content_size = strtoll(pPtr + HxSTD_StrLen("Content-Length: "), NULL, 10);
	} else {
		pInfo->content_size = -1;
	}

	return ERR_OK;
}

static HINT32 __recv_header(SOCKET sock, HCHAR * buf, HUINT32 size) {
	HUINT32 len = 0;
	HUINT32 p = 0;
	HCHAR ch;
	while ((len = recv(sock, &ch, 1, 0)) > 0) {
		buf[p++] = ch;
		if (p >= size) {
			return -1;
		}

		if (p >= 4 && buf[p-4] == '\r' && buf[p-3] == '\n' && buf[p-2] == '\r' && buf[p-1] == '\n') {
			break;
		}
	}
	return (len < 0 ? -1 : p);
}

static HUINT32 __recv_content(SOCKET sock, HCHAR * buf, HUINT32 size) {
	HUINT32 len = 0;
	HUINT32 p = 0;
	while ((p < size) && (len = recv(sock, buf + p, size - p, 0)) > 0) {
		p += len;
	}
	return p;
}

static HUINT32 __write(SOCKET sock, const HCHAR *buf, HUINT32 len)
{
	HINT32	sent, n, k;

	sent = 0;
	while (sent < len) {
		k = len - sent > INT_MAX ? INT_MAX : (int) (len - sent);

		n = send(sock, buf + sent, (size_t)k, 0);

		if (n < 0) 	break;

		sent += n;
	}

	return sent;
}

static HERROR	__set_server(int port)
{
	rest_service_context_t *ctx = __context();
	socklen_t sock_len = sizeof(struct sockaddr_in);

	HxSTD_MemSet(&ctx->local_address, 0, sock_len);
	ctx->local_address.sin_family = AF_INET;
	ctx->local_address.sin_port = htons((uint16_t) port);
	ctx->local_address.sin_addr.s_addr = htonl(INADDR_ANY);

	ctx->local_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(ctx->local_socket < 0) {
		return ERR_FAIL;
	} else
	{
		HINT32	reuseaddr = 1;
		struct timeval tv;

		tv.tv_sec = 0;
		tv.tv_usec = 500 * 1000;

		(void)setsockopt(ctx->local_socket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));
		(void)setsockopt(ctx->local_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
		(void)bind(ctx->local_socket, (const struct sockaddr *) &ctx->local_address, sock_len);
		(void)listen(ctx->local_socket, 20);
	}

	return ERR_OK;
}

static void __handle_connection(RestService_Connection_t * conn)
{
	socklen_t sock_len = sizeof(struct sockaddr_in);

	rest_service_context_t * ctx = __context();
	RestService_RequestInfo_t * req_info = &conn->request_info;

	struct timeval tv = {3, 0}; // 3 seconds timeout

	HxSTD_MemSet(conn->buf, 0, DIAL_REST_SERVER_BUF_SIZE);

	HxLOG_Debug( " ===== NEW CONNECTION =====\n");

	HxSTD_MemSet(req_info, 0,  sizeof(RestService_RequestInfo_t));

	conn->content_size = -1;
	conn->header_size = 0;
	conn->data_len = 0;

	if(getsockname(conn->client.sock, (struct sockaddr *) &conn->request_info.local_addr, &sock_len) < 0) {
		HxLOG_Error("[%s:%d] cann't check local ip -- ignore \n", "DIAL", __LINE__);
	}

	if (setsockopt(conn->client.sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) != 0) {
		HxLOG_Error("[%s:%d] setsockopt() failed - SO_RCVTIMEO \n", "DIAL", __LINE__);
		goto quit;
	}

	conn->data_len = __recv_header(conn->client.sock, conn->buf, DIAL_REST_SERVER_BUF_SIZE);

	if(conn->data_len > 0) {
		conn->header_size =	__check_http_header_size(conn->buf, conn->data_len);
	}

	if (conn->header_size == 0 && conn->data_len == DIAL_REST_SERVER_BUF_SIZE) {
		PL_DIAL_RestService_SendHttpError(conn, 413, "Request Too Large", "");
		goto quit;
	}

	if (conn->header_size <= 0) {
		HxLOG_Error("http header size invalid. (header size: %d)\n", conn->header_size);
		goto quit;
	}

	if (parse_http_request(conn->buf, req_info) != ERR_OK) {
		PL_DIAL_RestService_SendHttpError(conn, 400, "Bad Request", "Cannot parse HTTP request: [%.*s]", conn->data_len, conn->buf);
		goto quit;
	}

	HxLOG_Debug("Header : %s\n", conn->buf);

	if(req_info->method == DIAL_Request_POST) {

		HCHAR * content = conn->buf + conn->header_size;
		HUINT32 contentSize = (HUINT32)req_info->content_size;

		if (contentSize >= (DIAL_REST_SERVER_BUF_SIZE - conn->header_size)) {
			PL_DIAL_RestService_SendHttpError(conn, 413, "Request Too Large", "");
			goto quit;
		}

		if (contentSize > 0) {

			HUINT32 ret = 0;

			if ((ret = __recv_content(conn->client.sock, content, contentSize)) != contentSize) {
				HxLOG_Error("__recv_content() error / expected : %d but %d received...\n", contentSize, ret);
				goto quit;
			}

			HxLOG_Debug("Content : %s\n", content);
		}

		req_info->body = content;
	}

	if(ctx->callback) {
		ctx->callback(Dial_New_Request, conn, &conn->request_info);
	} else {
		PL_DIAL_RestService_SendHttpError(conn, 404, "Not Found", "%s", "File not found");
	}

 quit:

	if (req_info->uri) {
		HLIB_STD_MemFree(req_info->uri);
	}

	__remove_header_fields(&(req_info->fields));
}

static struct socket * __pop_client_socket(void) {

	rest_service_context_t * ctx = __context();
	struct socket * client = NULL;

	__lock();

	if (ctx->last_index != ctx->current_index) {
		HxLOG_Error("__pop_client_socket()::connection handling idx: %d\n", ctx->last_index);
		client = &ctx->queue[ctx->last_index];
		ctx->last_index = ((ctx->last_index + 1) % ARRAY_SIZE(ctx->queue));
	}

	__unlock();

	return client;
}

static void * __connection_task(void *arg)
{
	rest_service_context_t * ctx = __context();
	RestService_Connection_t * conn = NULL;
//	socklen_t addr_len;

	HxLOG_Error("[%s:%d] ++ DIAL REST Server Task\n", __func__, __LINE__);

	conn = HLIB_STD_MemCalloc(sizeof(RestService_Connection_t));
	if (conn == NULL)
	{
		HxLOG_Error("[%s:%d] Error> Out of memory!\n", "DIAL", __LINE__);
		return NULL;
	}

	__lock();
	ctx->task_count++;
	__unlock();

	while (!ctx->flag_stop) {

		struct socket * client = __pop_client_socket();

		if (client) {

			HxSTD_MemSet(conn, 0, sizeof(RestService_Connection_t));
			HxSTD_MemCopy(&conn->client, client, sizeof(struct socket));
			HxSTD_MemCopy(&conn->request_info.remote_addr, &conn->client.remote_addr, sizeof(struct sockaddr_in));

//			addr_len = sizeof(conn->request_info.local_addr);

			if(conn->client.sock >= 0) {
				__handle_connection(conn);
				(void) close(conn->client.sock);
			}
		}
		else {
			HLIB_STD_TaskSleep(10);
		}
	}

	if (conn) {
		HLIB_STD_MemFree(conn);
	}

	HxLOG_Error("[%s:%d] -- DIAL REST Server Task\n", __func__, __LINE__);

	__lock();
	ctx->task_count--;
	__unlock();
	return NULL;
}

static void * __server_task(void *arg)
{
	struct socket accepted;
	rest_service_context_t *ctx = __context();

	socklen_t sock_len = sizeof(accepted.local_addr);
	HxSTD_MemCopy(&accepted.local_addr, &s_pst_context.local_address, sock_len);

	HxLOG_Error("[%s:%d] ++\n", __func__, __LINE__);

	while (!ctx->flag_stop)
	{
		HxSTD_MemSet(&accepted.remote_addr, 0, sock_len);

		accepted.sock = accept(ctx->local_socket, (struct sockaddr *) &accepted.remote_addr, &sock_len);
		if (accepted.sock >= 0) {
			__lock();

			ctx->queue[ctx->current_index] = accepted;
			ctx->current_index = ((ctx->current_index + 1) % ARRAY_SIZE(ctx->queue));

			__unlock();
		}
	}

	(void) close(ctx->local_socket);

	HxLOG_Error("[%s:%d] --\n", __func__, __LINE__);
	return NULL;
}

/**
* This function is used to create a new thread
* @param[in] pFunc: function pointer to the function which be run in thread
* @param[in] pParam: pointer to parameter struct which is passed to above function
* @param[in] bIsJoinable: TRUE if desirable thread is joinable, FALSE if vice versa
* @param[in]lSize: stack size in PTHREAD_STACK_MIN unit, 0 for using default stack size
* @param[out] pThread: pthread_t pointer to the created thread, pass NULL if do not want to receive the output
* @return ERR_OK if this function is called succesfully, ERR_FAIL if vice versa
*/
HERROR __create_thread(void *(*pFunc) (void *), void *pParam, HBOOL bIsJoinable, HINT32 lSize, pthread_t *pThread)
{
	HERROR	eRet = ERR_OK;
	pthread_attr_t stThreadAttr;
	pthread_t stThread;
	int ret = 0;

	ret = pthread_attr_init(&stThreadAttr);
	if (ret != 0)
	{
		HxLOG_Critical("pthread_attr_init errno = %d!!!\n", ret);
		eRet = ERR_FAIL;
	}
	else
	{
		if(lSize)
		{
			ret = pthread_attr_setstacksize (&stThreadAttr, PTHREAD_STACK_MIN * lSize);
			if(ret != 0)
			{
				HxLOG_Critical("pthread_attr_setstacksize errno = %d!!!\n", ret);
	 		}
		}

		if(!bIsJoinable)
		{
			ret = pthread_attr_setdetachstate(&stThreadAttr, PTHREAD_CREATE_DETACHED);
			if(ret != 0)
			{
				HxLOG_Critical("pthread_attr_setdetachstate errno = %d!!!\n", ret);
			}
		}

		ret = pthread_create(&stThread, &stThreadAttr, pFunc, pParam);
		if(ret != 0)
		{
			HxLOG_Critical("pthread_create errno = %d!!!\n", ret);
			eRet = ERR_FAIL;
		}
		else if(pThread)
		{
			*pThread = stThread;
		}

		ret = pthread_attr_destroy(&stThreadAttr);
		if(ret != 0)
		{
			HxLOG_Critical("pthread_attr_destroy = %d!!!\n", ret);
		}
	}
	return eRet;
}

#define ________________Public_Interfaces__________________________
HERROR PL_DIAL_RestService_Start(PL_DIAL_RestService_CB callback, HUINT32 port)
{
	rest_service_context_t * ctx = __context();
	HUINT32 i = 0;

	HxSTD_MemSet(ctx, 0,  sizeof(rest_service_context_t));
	ctx->callback = callback;

	if (__set_server(port) != ERR_OK)
	{
		HxLOG_Error("Cann't start dial rest server. \n");
		return ERR_FAIL;
	}

	if(VK_SEM_Create(&ctx->lock, "dial_server_sema", VK_SUSPENDTYPE_PRIORITY) != ERR_OK)
	{
		HxLOG_Error("Cann't start dial rest server. (sema error) \n");
		return ERR_FAIL;
	}

	__create_thread(&__server_task, (void *)ctx, FALSE, 2, NULL);
	for (i = 0; i < 5; i++)	{
		__create_thread(&__connection_task, (void *)ctx, FALSE, 2, NULL);
	}

	return ERR_OK;
}

HERROR PL_DIAL_RestService_Stop(void)
{

	rest_service_context_t *ctx = __context();
	HUINT32 wait_cnt = 0;

	__lock();
	ctx->flag_stop = TRUE;
	close(ctx->local_socket);
	__unlock();

	HxLOG_Error("Wait for tasks finished...\n");
	while (ctx->task_count > 0) {
		wait_cnt++;
		if (wait_cnt >= 1000) { /* approximately 10 seconds timeout */
			HxLOG_Error("Waiting timeout: too much time waited...\n");
			break;
		}
		HLIB_STD_TaskSleep(10);
	}

	return ERR_OK;
}

HERROR PL_DIAL_RestService_SendMsssage(RestService_Connection_t *conn, const HCHAR * msg)
{
	HCHAR		buf[4096] = {0,};
	HUINT32		len =  snprintf(buf, sizeof(buf), "%s", msg);

	HxLOG_Debug("[%s:%d] Send Message \n%s\n", __func__, __LINE__, msg);
	__write(conn->client.sock, (const HCHAR *) buf, len);

	return ERR_OK;
}

HERROR	PL_DIAL_RestService_SendHttpError(RestService_Connection_t *conn, HUINT32 status, const HCHAR *reason, const HCHAR *fmt, ...)
{
	HCHAR		buf[4096] = {0, };
	HUINT32		len;

	buf[0] = '\0';
	len = 0;

	snprintf(buf, sizeof(buf), "HTTP/1.1 %d %s\r\nContent-Type: text/plain\r\nContent-Length: %d\r\nConnection: close\r\n\r\n", status, reason, len);
	PL_DIAL_RestService_SendMsssage(conn, buf);

	if (status > 199 && status != 204 && status != 304)
	{
		HxSTD_MemSet(buf, 0, sizeof(buf));
		len = snprintf(buf, sizeof(buf), "Error %d: %s\n", status, reason);
		len += snprintf(buf+len, sizeof(buf)-len, fmt);
		PL_DIAL_RestService_SendMsssage(conn,  buf);
	}

	return ERR_OK;
}

HINT32	PL_DIAL_RestService_GetServicePort(void)
{
	struct sockaddr addr;

	HxSTD_MemSet(&addr, 0, sizeof(struct sockaddr));
	HxSTD_MemCopy(&addr, &s_pst_context.local_address, sizeof(s_pst_context.local_address));

    return ntohs(((struct sockaddr_in *)&addr)->sin_port);
}

HCHAR * PL_DIAL_GetHeaderField(header_fields_t * fields, const HCHAR * name) {
	return __get_header_field(fields, name);
}

/*
 * HTTP PARSER
 */

static void __append_header_field(header_fields_t * fields, const HCHAR * name, const HCHAR * value) {
	header_field_t * new_node = NULL;
	header_field_t * last = fields->first;
	while (last && last->next) {
		last = last->next;
	}

	new_node = HLIB_STD_MemCalloc(sizeof(header_field_t));
	HxSTD_snprintf(new_node->name, sizeof(new_node->name), "%s", name);
	HxSTD_snprintf(new_node->value, sizeof(new_node->value), "%s", value);

	if (last == NULL) {
		fields->first = new_node;
	} else {
		last->next = new_node;
	}
}

static HCHAR * __get_header_field(header_fields_t * fields, const HCHAR * name) {
	header_field_t * header = fields->first;
	while (header) {
		if (!HLIB_STD_StrCaseCmp(header->name, name)) {
			return header->value;
		}
		header = header->next;
	}
	return NULL;
}

static void __remove_header_fields(header_fields_t * fields) {
	header_field_t * header = fields->first;
	while (header) {
		header_field_t * node = header;
		header = header->next;
		HOM_MEMFREE(node);
	}
}

static HCHAR * __first_not_space(HCHAR * str) {
	while (*str) {
		if (!isspace(*str)) {
			return str;
		}
		str++;
	}
	return NULL;
}

static HCHAR * __find(HCHAR * str, char m) {
	while (*str) {
		if (*str == m) {
			return str;
		}
		str++;
	}
	return NULL;
}

static HCHAR * __rtrim(HCHAR * str, size_t size) {
	size_t i = size - 1;
	for (; i >= 0; i--) {
		if (!isspace(str[i])) {
			break;
		}
		str[i] = '\0';
	}
	return str;
}

static HERROR __parse_header_field(HCHAR * line, HCHAR * nameBuf, size_t nameLen, HCHAR * valueBuf, size_t valueLen) {
	HCHAR * ns = NULL;
	HCHAR * ne = NULL;

	ns = __first_not_space(line);
	if (!ns) {
		return ERR_FAIL;
	}
	ne = __find(ns, ':');
	if (!ne) {
		return ERR_FAIL;
	}
	*ne = '\0';

	HxSTD_snprintf(nameBuf, nameLen, "%s", __rtrim(ns, ne - ns));
	HxSTD_snprintf(valueBuf, valueLen, "%s", __first_not_space(ne + 1));

	return ERR_OK;
}

static HCHAR *  __read_header_line(HCHAR * str, HCHAR * buf, size_t size) {
	HCHAR * e = __find(str, '\r');
	if (!e) {
		HxSTD_snprintf(buf, size, "%s", str);
		return NULL;
	} else {
		size_t len = e - str;
		HxSTD_snprintf(buf, (len + 1 > size ? size : len + 1), "%s", str);
		return e + 2;
	}
}

static HERROR __parse_header_fields(HCHAR * str, name_value_handler handler, void * userData) {
	HCHAR line[1024] = {0,};
	HCHAR * lp = str;
	while ((lp =  __read_header_line(lp, line, sizeof(line)))) {
		HCHAR name[100] = {0,};
		HCHAR value[128] = {0,};
		__parse_header_field(line, name, sizeof(name), value, sizeof(value));
		if (handler) {
			handler(name, value, userData);
		}
	}
	return ERR_OK;
}

static void __make_name_value_list(const HCHAR * name, const HCHAR * value, void * userData) {
	header_fields_t * fields = (header_fields_t*)userData;
	__append_header_field(fields, name, value);
}
