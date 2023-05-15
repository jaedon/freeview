/*************************************************************
 * @file		hxsocket.h
 * @date		2012/03/13
 * @author		someone
 * @brief		HLIB Socket API

 * http://www.humaxdigital.com
 *************************************************************/

#define ___HEADER________________________________________________________________________________________

#include <stdio.h>
#include <htype.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/queue.h>
#include <termio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include "vkernel.h"
#include "inc/vk_p_common_api.h"


#define ___DEFINE________________________________________________________________________________________

#define VK_SOCKET_DEBUG
#ifdef VK_SOCKET_DEBUG
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)				VK_DBG_Print("(+)%s\n", __FUNCTION__)
#define PrintExit(...)				VK_DBG_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintDebug(fmt, ...)    	while (0) ((int (*)(char *, ...)) 0)
#define PrintError(fmt, ...)		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...) 			while (0) ((int (*)(char *, ...)) 0)
#define PrintExit(...)				while (0) ((int (*)(char *, ...)) 0)
#endif /* VK_SOCKET_DEBUG */

#define vksocket_type_e    VK_SOCKET_TYPE_E
#define vksocket_flag_e    VK_SOCKET_FLAG_E

#define SOCKET_ERR(ret) {						\
		if(ret) {						\
			PrintError("ERROR %s,%d: %s\n", __FUNCTION__, __LINE__, strerror(errno)); \
			goto sock_err;				\
		}							\
	}

#define	SOCKET_TIMEOUT_FOREVER    0xFFFFFFFE
#define SOCKET_MAGIC_CODE         0x2255CEAC

#define ___TYPEDEF_______________________________________________________________________________________

typedef	enum {
	VK_SOCKET_RESULT_OK,
	VK_SOCKET_RESULT_DISCONNECTED,
	VK_SOCKET_RESULT_FAIL,
	VK_SOCKET_RESULT_TIMEOUT,
	VK_SOCKET_RESULT_MAX
} vksocket_result_e;

typedef struct {
	HUINT32 unique_id;
	HINT32 fd;
	vksocket_flag_e flags;
	vksocket_type_e type;
	vksocket_result_e result;
	union {
		HCHAR *udspath;
		HINT32 port;
	} addr;
	HCHAR szIpAddr[32];
	HINT32 pid;
	HINT32 tid;
} vksocket_t;

typedef	struct {
	HUINT32	magicCode;
	HUINT32	reqid;
	HUINT32	size;
	vksocket_t sender_sockinfo;
} vksocket_header_t;

typedef struct {
	vksocket_header_t header;
	HUINT8 *data;
	HUINT32 size;
} vksocket_data_t;

typedef struct {
	vksocket_t	socket;
} vksocket_client_t;

struct rxdata_t {
	vksocket_data_t rxdata;
	TAILQ_ENTRY(rxdata_t) entries;
};

struct client_t {
	vksocket_client_t client;
	TAILQ_HEAD(rxdata_list_t,rxdata_t) rxdatas;
	TAILQ_ENTRY(client_t) entries;
};

typedef struct {
	vksocket_t	socket;
	HUINT32 sem;
	TAILQ_HEAD(client_list_t,client_t) clients;
} vksocket_server_t;

#if defined(VK_SOCKET_DEBUG)
struct dbg_sock_hdl_t {
	HUINT32 sock_hdl;
	TAILQ_ENTRY(dbg_sock_hdl_t) entries;
};
TAILQ_HEAD(dbg_sock_list_t, dbg_sock_hdl_t) dbg_sock_queue;
#endif

typedef struct {
	vksocket_server_t *server;
	VK_SOCKET_DataListener_Cb pfn;
	void *usrdata;
} vksocket_intr_t;

#define ___STATIC_VARIABLES______________________________________________________________________________

static unsigned long sVkSocketIntrId;
#if defined(VK_SOCKET_DEBUG)
static unsigned long dbgsem;
#endif

#define ___PRIVATE_FUNCTION_PROTOTYPE____________________________________________________________________

#define	___PRIVATE_API___________________________________________________________________________________

static void	socket_common_close(vksocket_t *sock)
{
	SOCKET_ERR((!sock));

	if (sock->fd >= 0)
		close(sock->fd);

	if (sock->type == VK_SOCKET_TYPE_UDS && sock->addr.udspath)
		VK_free(sock->addr.udspath);

sock_err:
	return;
}

static vksocket_result_e socket_common_read(HINT32 fd,
	                                        void *pvData,
	                                        HUINT32 size)
{
	HUINT8 *pucData = (HUINT8*)pvData;
	HINT32 len;
	HINT32 readsize, remainsize;
	vksocket_result_e ret = VK_SOCKET_RESULT_FAIL;

	remainsize = (HINT32)size;
	readsize = 0;
	while (remainsize)
	{
		len = read(fd, pucData, remainsize);
		if (len < 0)
		{
			if (errno == EBADF || errno == EFAULT || errno == EINVAL || errno == EIO || errno == ENOSPC || errno == EPIPE)
			{
				PrintError("[%s,%d] Error! read bad block\n", __FUNCTION__, __LINE__);
				ret = VK_SOCKET_RESULT_DISCONNECTED;
				goto sock_err;
			}
			continue;
		}

		remainsize -= len;
		readsize += len;
		pucData += len;

		if (len == 0)
		{
			/* data is finished. */
			if (readsize == 0)
			{
				/* Socket is closed */
				ret = VK_SOCKET_RESULT_DISCONNECTED;
				goto sock_err;
			}

			SOCKET_ERR((remainsize));
		}
	}

	ret = VK_SOCKET_RESULT_OK;
sock_err:
	return ret;
}

static vksocket_result_e socket_common_write(HINT32 fd,
	                                         void *pvData,
	                                         HUINT32 size)
{
	HINT32	len;
	HINT32	remainsize;
	const HUINT8 *p =(const HUINT8 *)pvData;
	vksocket_result_e ret = VK_SOCKET_RESULT_FAIL;

	remainsize = (HINT32)size;
	do
	{
		len = write(fd, p, remainsize);
		if (len < 0)
		{
			if (errno == EBADF || errno == EFAULT || errno == EINVAL || errno == EIO || errno == ENOSPC || errno == EPIPE)
			{
				PrintError("[%s,%d] Error! write fail\n", __FUNCTION__, __LINE__);
				ret = VK_SOCKET_RESULT_DISCONNECTED;
				goto sock_err;
			}
			continue;
		}
		p += len;
		remainsize -= len;
	} while (remainsize > 0);

	ret = VK_SOCKET_RESULT_OK;
sock_err:
	return ret;
}

static void	socket_server_listen(vksocket_server_t *server,
                                 HUINT32 timeout,
                                 VK_SOCKET_DataListener_Cb cbPacketReceived,
                                 void *userdata)
{
	HINT32 monitorFD;
	fd_set read_fds;
	struct timeval tv;
	struct timeval *wt;
	vksocket_result_e ret;
	vksocket_header_t header;
	vksocket_intr_t intr;
	int rc;
	struct client_t *cq, *tcq;
	struct rxdata_t *rxdq, *trxdq;

	SOCKET_ERR((!server));

	FD_ZERO(&read_fds);
	FD_SET(server->socket.fd, &read_fds);

	monitorFD = server->socket.fd;
	VK_SEM_Get(server->sem);
	TAILQ_FOREACH(cq, &server->clients, entries)
	{
		FD_SET(cq->client.socket.fd, &read_fds);
		if (monitorFD < cq->client.socket.fd)
			monitorFD = cq->client.socket.fd;
	}
	VK_SEM_Release(server->sem);

	if (timeout == SOCKET_TIMEOUT_FOREVER)
	{
		wt = NULL;
	}
	else
	{
		tv.tv_sec  = timeout / 1000;
		tv.tv_usec = (timeout % 1000) * 1000;
		wt = &tv;
	}

	monitorFD = monitorFD + 1;
	do
	{
		rc  = select(monitorFD, &read_fds, (fd_set*)0, (fd_set*)0, wt);
	} while (rc == -1 && errno == EINTR);
	SOCKET_ERR((rc<0));

	if (rc == 0)
	{
		server->socket.result = VK_SOCKET_RESULT_TIMEOUT;
		SOCKET_ERR((1));
	}

	do
	{
		if (FD_ISSET(server->socket.fd, &read_fds))
		{
			struct sockaddr_in	addr;
			HINT32	fd;
			socklen_t addrsize = sizeof(struct sockaddr_in);

			fd = accept(server->socket.fd, (struct sockaddr*)&addr, &addrsize);
			if (fd < 0)
			{
				PrintError("ERROR %s,%d, %d, %s\n", __FUNCTION__, __LINE__, server->socket.fd, strerror(errno));
				continue;
			}

			cq = VK_malloc(sizeof(struct client_t));
			if (cq)
			{
				VK_memset(cq, 0, sizeof(struct client_t));
				cq->client.socket.unique_id = (HUINT32)&cq->client;
				cq->client.socket.fd = fd;
				VK_SEM_Get(server->sem);
				TAILQ_INIT(&cq->rxdatas);
				TAILQ_INSERT_TAIL(&server->clients, cq, entries);
				VK_SEM_Release(server->sem);
			}
			else
			{
				close(fd);
			}
		}

		VK_SEM_Get(server->sem);
		for (cq = TAILQ_FIRST(&server->clients); cq != NULL; cq = tcq)
		{
			tcq = TAILQ_NEXT(cq, entries);
			if (!FD_ISSET(cq->client.socket.fd, &read_fds))
			{
				continue;
			}

			ret = socket_common_read(cq->client.socket.fd, &header, sizeof(vksocket_header_t));
			if (ret == VK_SOCKET_RESULT_OK)
			{
				SOCKET_ERR((header.magicCode!=SOCKET_MAGIC_CODE));

				rxdq = VK_malloc(sizeof(struct rxdata_t));
				SOCKET_ERR((!rxdq));

				VK_memset(rxdq, 0, sizeof(struct rxdata_t));
				VK_memcpy(&rxdq->rxdata.header, &header, sizeof(vksocket_header_t));

				rxdq->rxdata.data = VK_malloc(header.size);
				if (rxdq->rxdata.data)
				{
					VK_memset(rxdq->rxdata.data, 0, sizeof(header.size));
					rxdq->rxdata.size = header.size;

					ret = socket_common_read(cq->client.socket.fd, rxdq->rxdata.data, rxdq->rxdata.size);
					if (ret != VK_SOCKET_RESULT_OK)
					{
						VK_free(rxdq->rxdata.data);
						rxdq->rxdata.data = NULL;
						rxdq->rxdata.size = 0;
						VK_free(rxdq);
					}
					else
					{
						TAILQ_INSERT_TAIL(&cq->rxdatas, rxdq, entries);
					}
				}
				else
				{
					VK_free(rxdq);
					ret = VK_SOCKET_RESULT_FAIL;
				}
			}

			switch (ret) {
			case VK_SOCKET_RESULT_DISCONNECTED:
				VK_SEM_Get(server->sem);
				for (rxdq = TAILQ_FIRST(&cq->rxdatas); rxdq != NULL; rxdq = trxdq)
				{
					trxdq = TAILQ_NEXT(rxdq, entries);
					TAILQ_REMOVE(&cq->rxdatas, rxdq, entries);
					if (rxdq->rxdata.data) VK_free(rxdq->rxdata.data);
					rxdq->rxdata.data = NULL;
					rxdq->rxdata.size = 0;
					VK_free(rxdq);
				}
				TAILQ_REMOVE(&server->clients, cq, entries);
				VK_SEM_Release(server->sem);
				socket_common_close(&cq->client.socket);
				VK_free(cq);
				break;
			case VK_SOCKET_RESULT_FAIL:
				PrintError("[%s,%d] Error! read fail\n", __FUNCTION__, __LINE__);
				/*go through*/
			default:
				cq->client.socket.result = ret;
				break;
			}
		}
		VK_SEM_Release(server->sem);

		tv.tv_sec  = 0;
		tv.tv_usec = 0;
	} while ((rc = select(monitorFD, &read_fds, (fd_set*)0, (fd_set*)0, &tv)) > 0);

	intr.server = server;
	intr.pfn = cbPacketReceived;
	intr.usrdata = userdata;
	VK_INTR_Event(sVkSocketIntrId, (void*)&intr);

sock_err:
	return;
}

static HINT32 socket_client_connect(vksocket_type_e type,
	                                const HCHAR *pszIPAddr,
	                                const void *target)
{
	HINT32 fd = -1;
	HINT32 err;
	struct sockaddr_un addr_un;
	struct sockaddr_in addr_in;

	switch (type) {
	case VK_SOCKET_TYPE_UDS:
		fd = socket(PF_FILE, SOCK_STREAM, 0);
		SOCKET_ERR((fd<0));

		VK_memset(&addr_un, 0, sizeof(struct sockaddr_un));
		addr_un.sun_family = AF_UNIX;
		VK_snprintf(addr_un.sun_path, sizeof(addr_un.sun_path), "%s", (char*)target);

		err = connect(fd, (struct sockaddr *)&addr_un, sizeof(struct sockaddr_un));
		SOCKET_ERR((err<0));
		return fd;
	case VK_SOCKET_TYPE_INET:
		fd = socket(AF_INET, SOCK_STREAM, 0);
		SOCKET_ERR((fd<0));

		VK_memset(&addr_in, 0, sizeof(struct sockaddr_in));
		addr_in.sin_family = AF_INET;
		addr_in.sin_port = htons(*((HINT32*)target));
		if (!pszIPAddr)
			inet_pton(AF_INET, "127.0.0.1", &addr_in.sin_addr);
		else
			inet_pton(AF_INET, pszIPAddr, &addr_in.sin_addr);

		err = connect(fd, (struct sockaddr*)&addr_in, sizeof(struct sockaddr_in));
		SOCKET_ERR((err<0));
		return fd;
	default:
		break;
	}

sock_err:
	if (fd>=0) close(fd);
	return -1;
}

static void socket_client_listen(vksocket_client_t *client,
	                             HUINT32 timeout,
	                             VK_SOCKET_DataListener_Cb cbPacketReceived,
	                             void *userdata)
{
	HINT32 fd;
	fd_set read_fds;
	struct timeval tv;
	struct timeval *wt;
	HINT32 rc;
	vksocket_header_t header;
	vksocket_result_e ret;
	vksocket_data_t rxdata;

	SOCKET_ERR((!client));

	VK_memset(&rxdata, 0, sizeof(vksocket_data_t));
	fd = client->socket.fd;

	FD_ZERO(&read_fds);
	FD_SET(fd,&read_fds);

	if (timeout == SOCKET_TIMEOUT_FOREVER)
	{
		wt = NULL;
	}
	else
	{
		tv.tv_sec  = timeout / 1000;
		tv.tv_usec = (timeout % 1000) * 1000;
		wt = &tv;
	}

	do {
		rc = select(fd + 1, &read_fds, (fd_set*)0, (fd_set*)0, wt);
	} while (rc == -1 && errno == EINTR);
	SOCKET_ERR((rc<0));

	if (rc == 0 || !FD_ISSET(fd, &read_fds))
	{
		client->socket.result = VK_SOCKET_RESULT_TIMEOUT;
		SOCKET_ERR((1));
	}

	ret = socket_common_read(fd, &header, sizeof(vksocket_header_t));
	if (ret == VK_SOCKET_RESULT_OK)
	{
		SOCKET_ERR((header.magicCode!=SOCKET_MAGIC_CODE));

		/*
		tHeader.sender_sockinfo includes the information of sender.
		Here, sender will be server.
		Implemet any code if you want to do something with the impormation of sender.
		*/
		VK_memcpy(&rxdata.header, &header, sizeof(vksocket_header_t));
		rxdata.data = VK_malloc(header.size);
		if (rxdata.data)
		{
			rxdata.size = header.size;
			ret = socket_common_read(fd, rxdata.data, rxdata.size);

			if (ret == VK_SOCKET_RESULT_OK && cbPacketReceived)
				cbPacketReceived(userdata,
				                 (HUINT32)client,
				                 rxdata.header.reqid,
				                 rxdata.data,
				                 rxdata.size);

			VK_free(rxdata.data);
			rxdata.data = NULL;
		}
	}
	client->socket.result = ret;

sock_err:
	return;
}

static void socket_intr_cb(void *pArg)
{
	vksocket_intr_t *intr = (vksocket_intr_t*)pArg;
	vksocket_server_t *server;
	struct client_t *cq;
	struct rxdata_t *rxdq, *trxdq;

	SOCKET_ERR((!intr));
	server = intr->server;

	VK_SEM_Get(server->sem);
	TAILQ_FOREACH(cq, &server->clients, entries)
	{
		for (rxdq = TAILQ_FIRST(&cq->rxdatas); rxdq != NULL; rxdq = trxdq)
		{
			trxdq = TAILQ_NEXT(rxdq, entries);
			if (intr->pfn && rxdq->rxdata.size)
			{
				intr->pfn(intr->usrdata,
			              (HUINT32)&cq->client,
			              rxdq->rxdata.header.reqid,
			              rxdq->rxdata.data,
			              rxdq->rxdata.size);

				VK_free(rxdq->rxdata.data);
				rxdq->rxdata.data = NULL;
				rxdq->rxdata.size = 0;
			}
			TAILQ_REMOVE(&cq->rxdatas, rxdq, entries);
			VK_free(rxdq);
		}
	}
	VK_SEM_Release(server->sem);

sock_err:
	return;
}

#define	___PUBLIC_API____________________________________________________________________________________

int VK_SOCKET_SERVER_Open(VK_SOCKET_TYPE_E eSocketType,
                          void *pServerPort,
                          HUINT32 *hSocket)
{
	HINT32	err;
	HINT32	fd = -1;
	vksocket_server_t *server = NULL;
	HINT32 reuse = 1;
	HCHAR sema_name[16];
#if defined(VK_SOCKET_DEBUG)
	struct dbg_sock_hdl_t *dbg_sock;
#endif

	switch(eSocketType) {
	case VK_SOCKET_TYPE_UDS:
	{
		HCHAR *serverName = (HCHAR*)pServerPort;
		struct sockaddr_un server_addr;

		if (access(serverName, F_OK) == 0)
		{
			err = unlink(serverName);
			SOCKET_ERR((err!=0));
		}

		fd = socket(AF_UNIX, SOCK_STREAM, 0);
		SOCKET_ERR((fd<0));

		VK_memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sun_family = AF_UNIX;
		VK_snprintf(server_addr.sun_path, sizeof(server_addr.sun_path), "%s", serverName);

		err = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*)&reuse, sizeof(reuse));
		SOCKET_ERR((err < 0));

		err = bind(fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
		SOCKET_ERR((err < 0));
		break;
	}
	case VK_SOCKET_TYPE_INET:
	{
		HINT32 serverPortNum = *((HINT32*)pServerPort);
		struct sockaddr_in server_addr;

		fd = socket(AF_INET, SOCK_STREAM, 0);
		SOCKET_ERR((fd<0));

		VK_memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(serverPortNum);
		server_addr.sin_addr.s_addr = INADDR_ANY;

		err = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*)&reuse, sizeof(reuse));
		SOCKET_ERR((err < 0));

		err = bind(fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
		SOCKET_ERR((err < 0));
		break;
	}
	default:
		SOCKET_ERR((1));
	}

	if (eSocketType == VK_SOCKET_TYPE_UDS)
	{
		err = chmod(pServerPort, 0777);
		SOCKET_ERR((err < 0));
	}

	err = listen(fd, 5);
	SOCKET_ERR((err < 0));

	server = VK_malloc(sizeof(vksocket_server_t));
	SOCKET_ERR((!server));

	VK_memset(server, 0, sizeof(vksocket_server_t));

	VK_snprintf(sema_name, sizeof(sema_name), "socksem%d", vk_p_common_GetPid());
	err = VK_SEM_Create((unsigned long*)&server->sem, sema_name, VK_SUSPENDTYPE_PRIORITY);
	SOCKET_ERR((err!=VK_OK));

	server->socket.unique_id = (HUINT32)server;
	server->socket.fd = fd;
	server->socket.flags = VK_SOCKET_FLAG_SERVER;
	server->socket.type = eSocketType;
	VK_snprintf(server->socket.szIpAddr, sizeof(server->socket.szIpAddr), "%s", "127.0.0.1");
	if(eSocketType == VK_SOCKET_TYPE_UDS)
		server->socket.addr.udspath = VK_strdup((const HCHAR*)pServerPort);
	else
		server->socket.addr.port = *((HINT32*)pServerPort);
	server->socket.pid = vk_p_common_GetPid();
	server->socket.tid = vk_p_common_GetTid();

	TAILQ_INIT(&server->clients);
#if defined(VK_SOCKET_DEBUG)
	dbg_sock = VK_malloc(sizeof(struct dbg_sock_hdl_t));
	if (dbg_sock)
	{
		VK_SEM_Get(dbgsem);
		dbg_sock->sock_hdl = (HUINT32)server;
		TAILQ_INSERT_TAIL(&dbg_sock_queue, dbg_sock, entries);
		VK_SEM_Release(dbgsem);
	}
#endif

	*hSocket = (HUINT32)server;
	return VK_OK;

sock_err:
	if (server) VK_free(server);
	if (fd >= 0) close(fd);
	return VK_ERROR;
}

int VK_SOCKET_CLIENT_Open(VK_SOCKET_TYPE_E eSocketType,
	                      const HCHAR *pszIPAddr,
	                      void *targetPort,
	                      HUINT32 nTry,
	                      HUINT32 *hSocket)
{
	HINT32 fd;
	HUINT32 failed = 0;
	vksocket_client_t *client;
#if defined(VK_SOCKET_DEBUG)
	struct dbg_sock_hdl_t *dbg_sock;
#endif

	do {
		fd = socket_client_connect(eSocketType, pszIPAddr, targetPort);
		if (fd != -1) break;
		if (nTry == 0) break;

		VK_TASK_Sleep(1000);
	} while (++failed < nTry);

	if (nTry > 0 && fd < 0)
		return 0;

	client = VK_malloc(sizeof(vksocket_client_t));
	SOCKET_ERR((!client));

	VK_memset(client, 0, sizeof(vksocket_client_t));

	client->socket.unique_id = (HUINT32)client;
	client->socket.fd = fd;
	client->socket.flags = VK_SOCKET_FLAG_CLIENT;
	client->socket.type = eSocketType;
	if (pszIPAddr)
		VK_snprintf(client->socket.szIpAddr, sizeof(client->socket.szIpAddr), "%s", pszIPAddr);
	else
		VK_snprintf(client->socket.szIpAddr, sizeof(client->socket.szIpAddr), "%s", "127.0.0.1");

	if(eSocketType == VK_SOCKET_TYPE_UDS)
		client->socket.addr.udspath = VK_strdup((const HCHAR*)targetPort);
	else
		client->socket.addr.port = *((HINT32*)targetPort);

	client->socket.pid = vk_p_common_GetPid();
	client->socket.tid = vk_p_common_GetTid();
#if defined(VK_SOCKET_DEBUG)
	dbg_sock = VK_malloc(sizeof(struct dbg_sock_hdl_t));
	if (dbg_sock)
	{
		VK_SEM_Get(dbgsem);
		dbg_sock->sock_hdl = (HUINT32)client;
		TAILQ_INSERT_TAIL(&dbg_sock_queue, dbg_sock, entries);
		VK_SEM_Release(dbgsem);
	}
#endif

	*hSocket = (HUINT32)client;
	return VK_OK;

sock_err:
	if (client) VK_free(client);
	if (fd >= 0) close(fd);
	return VK_ERROR;
}

int VK_SOCKET_Close(HUINT32 sock)
{
	int ret = VK_ERROR;
	vksocket_server_t *server;
	vksocket_client_t *client;
	vksocket_t *sockinfo;
	struct client_t *cq, *tcq;
	struct rxdata_t *rxdq, *trxdq;
#if defined(VK_SOCKET_DEBUG)
	struct dbg_sock_hdl_t *dq, *tdq;
#endif

	sockinfo = (vksocket_t*)sock;
	SOCKET_ERR((!sockinfo));
	SOCKET_ERR((sockinfo->unique_id!=sock));

	switch(sockinfo->flags) {
	case VK_SOCKET_FLAG_SERVER:
		server = (vksocket_server_t*)sock;
		socket_common_close(&server->socket);
		VK_SEM_Get(server->sem);
		for (cq = TAILQ_FIRST(&server->clients); cq != NULL; cq = tcq)
		{
			tcq = TAILQ_NEXT(cq, entries);
			for (rxdq = TAILQ_FIRST(&cq->rxdatas); rxdq != NULL; rxdq = trxdq)
			{
				trxdq = TAILQ_NEXT(rxdq, entries);
				TAILQ_REMOVE(&cq->rxdatas, rxdq, entries);
				if (rxdq->rxdata.data) VK_free(rxdq->rxdata.data);
				rxdq->rxdata.data = NULL;
				rxdq->rxdata.size = 0;
				VK_free(rxdq);
			}
			TAILQ_REMOVE(&server->clients, cq, entries);
			socket_common_close(&cq->client.socket);
			VK_free(cq);
		}
		VK_SEM_Release(server->sem);
		VK_SEM_Destroy(server->sem);
#if defined(VK_SOCKET_DEBUG)
		VK_SEM_Get(dbgsem);
		for (dq = TAILQ_FIRST(&dbg_sock_queue); dq != NULL; dq = tdq)
		{
			tdq = TAILQ_NEXT(dq, entries);
			if (dq->sock_hdl==(HUINT32)server)
			{
				TAILQ_REMOVE(&dbg_sock_queue, dq, entries);
				VK_free(dq);
			}
		}
		VK_SEM_Release(dbgsem);
#endif
		VK_free(server);
		break;
	case VK_SOCKET_FLAG_CLIENT:
		client = (vksocket_client_t*)sock;
		socket_common_close(&client->socket);
#if defined(VK_SOCKET_DEBUG)
		VK_SEM_Get(dbgsem);
		for (dq = TAILQ_FIRST(&dbg_sock_queue); dq != NULL; dq = tdq)
		{
			tdq = TAILQ_NEXT(dq, entries);
			if (dq->sock_hdl==(HUINT32)client)
			{
				TAILQ_REMOVE(&dbg_sock_queue, dq, entries);
				VK_free(dq);
			}
		}
		VK_SEM_Release(dbgsem);
#endif
		VK_free(client);
		break;
	default:
		SOCKET_ERR((1));
	}

	ret = VK_OK;
sock_err:
	return ret;
}

int VK_SOCKET_Listen(HUINT32 sock,
	                 HUINT32 timeout,
	                 VK_SOCKET_DataListener_Cb cbPacketReceived,
	                 void *userdata)
{
	vksocket_server_t *server;
	vksocket_client_t *client;
	vksocket_t *sockinfo;
	vksocket_result_e ret;
	HINT32 rc = VK_ERROR;

	sockinfo = (vksocket_t*)sock;
	SOCKET_ERR((!sockinfo));
	SOCKET_ERR((sockinfo->unique_id!=sock));

	switch(sockinfo->flags) {
	case VK_SOCKET_FLAG_SERVER:
		server = (vksocket_server_t*)sock;
		socket_server_listen(server, timeout, cbPacketReceived, userdata);
		ret = server->socket.result;
		break;
	case VK_SOCKET_FLAG_CLIENT:
		client = (vksocket_client_t*)sock;
		socket_client_listen(client, timeout, cbPacketReceived, userdata);
		ret = client->socket.result;
		break;
	default:
		ret = VK_SOCKET_RESULT_FAIL;
		break;
	}

	switch(ret) {
	case VK_SOCKET_RESULT_OK:
		break;
	case VK_SOCKET_RESULT_FAIL:
	case VK_SOCKET_RESULT_TIMEOUT:
	case VK_SOCKET_RESULT_DISCONNECTED:
	default:
		SOCKET_ERR((1));
	}

	rc = VK_OK;
sock_err:
	return rc;
}

int VK_SOCKET_Write(HUINT32 sock,
	                const void *pvData,
	                HUINT32 size,
	                HUINT32 reqid)
{
	vksocket_t *sockinfo;
	vksocket_result_e ret = VK_SOCKET_RESULT_FAIL;
	vksocket_header_t header;
	HINT32 rc = VK_ERROR;

	sockinfo = (vksocket_t*)sock;
	SOCKET_ERR((!sockinfo));
	SOCKET_ERR((sockinfo->unique_id!=sock));

	header.magicCode = SOCKET_MAGIC_CODE;
	header.size = size;
	header.reqid = reqid;
	VK_memcpy(&header.sender_sockinfo, sockinfo, sizeof(vksocket_t));

	ret = socket_common_write(sockinfo->fd, &header, sizeof(vksocket_header_t));
	switch(ret) {
	case VK_SOCKET_RESULT_OK:
		break;
	case VK_SOCKET_RESULT_FAIL:
	case VK_SOCKET_RESULT_TIMEOUT:
	case VK_SOCKET_RESULT_DISCONNECTED:
	default:
		SOCKET_ERR((1));
	}

	ret = socket_common_write(sockinfo->fd, (void*)pvData, size);
	switch(ret) {
	case VK_SOCKET_RESULT_OK:
		break;
	case VK_SOCKET_RESULT_FAIL:
	case VK_SOCKET_RESULT_TIMEOUT:
	case VK_SOCKET_RESULT_DISCONNECTED:
	default:
		SOCKET_ERR((1));
	}

	rc = VK_OK;
sock_err:
	sockinfo->result = ret;
	return rc;
}

int VK_SOCKET_Init(void)
{
	HINT32 rc = VK_ERROR;

	rc = VK_INTR_Register(socket_intr_cb, "vksockintr", sizeof(vksocket_intr_t), &sVkSocketIntrId, VK_INTR_TASK2);
	SOCKET_ERR((rc!=VK_OK));

#if defined(VK_SOCKET_DEBUG)
	TAILQ_INIT(&dbg_sock_queue);
	rc = VK_SEM_Create(&dbgsem, "vksockdbgsem", VK_SUSPENDTYPE_FIFO);
	SOCKET_ERR((rc!=VK_OK));
#endif

	rc = VK_OK;
sock_err:
	return rc;
}

void VK_SOCKET_PrintSockInfo(void)
{
#if defined(VK_SOCKET_DEBUG)
	vksocket_t *sockinfo;
	vksocket_server_t *server;
	vksocket_client_t *client;
	struct client_t *cq;
	struct dbg_sock_hdl_t *dq;
	int idx = 0;

	VK_SEM_Get(dbgsem);
	TAILQ_FOREACH(dq, &dbg_sock_queue, entries)
	{
		sockinfo = (vksocket_t*)dq->sock_hdl;
		switch(sockinfo->flags) {
		case VK_SOCKET_FLAG_SERVER:
			server = (vksocket_server_t*)dq->sock_hdl;
			break;
		case VK_SOCKET_FLAG_CLIENT:
			client = (vksocket_client_t*)dq->sock_hdl;
			break;
		default:
			continue;
		}
		PrintDebug("[Socket(%d,%d)]******************************************************************\n",
			        sockinfo->pid,
			        sockinfo->tid);
		PrintDebug("[Socket(%d,%d)] fd=%d, flags=%d, type=%d, udspath=%s, port=%d, szIpAddr=%s\n",
			       sockinfo->pid,
			       sockinfo->tid,
			       sockinfo->fd,
			       sockinfo->flags,
			       sockinfo->type,
			       (sockinfo->type==VK_SOCKET_TYPE_UDS)?sockinfo->addr.udspath:"",
			       (sockinfo->type!=VK_SOCKET_TYPE_UDS)?sockinfo->addr.port:0,
			       sockinfo->szIpAddr);

		switch(sockinfo->flags) {
		case VK_SOCKET_FLAG_SERVER:
			VK_SEM_Get(server->sem);
			TAILQ_FOREACH(cq, &server->clients, entries)
			{
				PrintDebug("[Socket(%d,%d)] Client%d fd=%d, flags=%d, type=%d\n",
					       sockinfo->pid,
					       sockinfo->tid,
					       idx+1,
					       cq->client.socket.fd,
					       cq->client.socket.flags,
					       cq->client.socket.type);
			}
			VK_SEM_Release(server->sem);
			break;
		case VK_SOCKET_FLAG_CLIENT:
			break;
		default:
			break;
		}
		PrintDebug("[Socket(%d,%d)]******************************************************************\n",
			        sockinfo->pid,
			        sockinfo->tid);
	}
	VK_SEM_Release(dbgsem);
	HAPPY(idx);
#endif
}
