/*************************************************************
 * @file		hxsocket.h
 * @date		2012/03/13
 * @author		someone
 * @brief		HLIB Socket API

 * http://www.humaxdigital.com
 *************************************************************/

/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */
#include 	<htype.h>

#if defined(OS_LINUX)
#include	<errno.h>
#include	<sys/ipc.h>
#include	<sys/select.h>
#include	<arpa/inet.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<sys/un.h>
#include	<sys/socket.h>
#include 	<termio.h>
#include 	<time.h>
#include 	<sys/time.h>
#include 	<sys/times.h>
#include 	<sys/stat.h>
#endif

#if defined(OS_LINUX)
#define SUPPORT_UDS_SYSTEM
#define SUPPORT_LINUX_TIMER

#define SOCKET_CLOSE	close

#endif

#if defined(OS_WIN32)
#if defined(CONFIG_SINGLE_PROCESS)
#define SUPPORT_MSG_SYSTEM
#else
#define SUPPORT_UDS_SYSTEM
#endif
#define SUPPORT_VK_TIMER
#endif

#if defined(OS_WIN32) && defined(SUPPORT_UDS_SYSTEM)
#include <winsock2.h>
#include <windows.h>

#define SOCKET_CLOSE closesocket

#endif

#include	"hlib.h"
#include	"_hlib_socket.h"


#ifdef	CONFIG_DEBUG
//#define	CONFIG_DEBUG_COMM
#endif

#define	SOCKET_MALLOC	HLIB_MEM_Malloc
#define	SOCKET_CALLOC	HLIB_MEM_Calloc
#define	SOCKET_STRDUP	HLIB_MEM_StrDup_CB
#define	SOCKET_FREE		HLIB_MEM_Free
#define	SOCKET_FREE_CB	HLIB_MEM_Free_CB
#define SOCKET_MEMDUP	HLIB_MEM_MemDup

#define ___DEFINE________________________________________________________________________________________

#define	SOCKET_FOREVER				0xFFFFFFFE
#define	SOCKET_BACKLOG_NUM			5
#define	SOCKET_MAX_CLIENT			64
#define	SOCKET_MAGIC_CODE			0x2255CEAC
#define	SOCKET_DATA_SIZE			256
#define	SOCKET_BUFFER_THRESHOLD		102400	// 100k
#define SOCKET_DEFAULT_TIMEOUT		(2500000)	/* 2.5 sec */

#define	SOCKET_IS_SERVER(soc)	(((HxSocket_t *)soc)->flags & eHxSOCKET_SERVER)
#define	SOCKET_IS_CLIENT(soc)	(((HxSocket_t *)soc)->flags & eHxSOCKET_CLIENT)
#define	SOCKET_ERROR(soc)		(((HxSocket_t *)soc)->flags & eHxSOCKET_ERROR)

#define ___TYPEDEF_______________________________________________________________________________________


typedef	enum
{
	SOCKET_RESULT_OK,
	SOCKET_RESULT_DISCONNECTED,
	SOCKET_RESULT_FAIL
} HxSocket_Result_t;


typedef enum
{
	eHxSOCKET_FLAG_NULL	= 0,
	eHxSOCKET_SERVER	= 0x00000001,
	eHxSOCKET_CLIENT	= 0x00000002,
	eHxSOCKET_RECONNECT = 0x00000010,
	eHxSOCKET_UNDEAD	= 0x10000000,
	eHxSOCKET_ZOMBIE	= 0x20000000,
	eHxSOCKET_ERROR		= 0x80000000,
	eHxSOCKET_FLAG_ALL	= 0xFFFFFFFF
} HxSocket_Flags_e;

typedef	struct
{
	HUINT32	magicCode;
	HUINT32	requestId;
	HUINT32	size;
#if defined(SUPPORT_LINUX_TIMER)
	struct timespec refTime;
#elif defined(SUPPORT_VK_TIMER)
	HUINT32	refTime;
#endif
} HxSocket_Header_t;


typedef struct
{
	HxSocket_Header_t	header;
	HINT32				client;
	HUINT32				allocated;
	HUINT32				size;
	HUINT8				data[4];
} HxSocket_Data_t;

typedef struct
{
	HINT32				fd;
	HxSocket_Flags_e	flags;
	HxSocket_Type_e		type;
#if defined(SUPPORT_UDS_SYSTEM)
	union {
		HCHAR 	*udspath;
		HINT32	port;
	} addr;
	HxQueue_t			*freed;
	HxPriorityQueue_t	*fifoQ;
	HCHAR				szIpAddr[32];
#elif defined(SUPPORT_MSG_SYSTEM)
	HINT32				parent_fd;
	HxList_t			*pSocketList;
	HULONG				ulSemId;
	HCHAR				szHashKey[128];
#endif
#ifdef	CONFIG_DEBUG
	HCHAR				description[128];
	HCHAR				* debugText;
	HUINT32 			debugLine;
#endif
} HxSocket_t;

typedef struct hxsocket_server
{
	HxSocket_t	socket;

	HxVector_t	*clients;	// connected client(s)
	HxQueue_t	*disconnected;
} HxSocket_Server_t;

typedef struct hxsocket_client
{
	HxSocket_t	socket;
	HxQueue_t	*bufferQ;
	HUINT32		bufferSize;
} HxSocket_Client_t;


#define ___STATIC_VARIABLES______________________________________________________________________________

#define ___PRIVATE_FUNCTION_PROTOTYPE____________________________________________________________________

#if	defined(CONFIG_DEBUG_COMM)
#include	<_hlib_hash.h>
#include	<_hlib_dir.h>

#define	DBG_HXSOCKET_ITEMPADDING	4
#define	DBG_HXSOCKET_ITEMSIZE		1020
#define	DBG_HXSOCKET_QUEUESIZE		1024			//	1MB buffer for each connections
#define	DBG_HXSOCKET_PATH			"/logsck/"
typedef struct
{
	FILE	*file;

	HUINT8	aucData[(DBG_HXSOCKET_ITEMSIZE + DBG_HXSOCKET_ITEMPADDING) * 2];
	HUINT32	nItemIndex;
} DBG_HXSOCKETInfo_t;

static HxHASH_t	*s_pstDbgSocketTable = NULL;

static void	dbg_socket_register(HCHAR *name, HINT32 handle, HBOOL isServer)
{
	HCHAR	szFileName[256], *p;
	DBG_HXSOCKETInfo_t	*pstSocketInfo;

	if (access(DBG_HXSOCKET_PATH, F_OK) == -1)
	{
		if (HLIB_DIR_Make(DBG_HXSOCKET_PATH) != ERR_OK)
		{
			return;
		}
		chmod(DBG_HXSOCKET_PATH, 0777);
	}

	if (s_pstDbgSocketTable == NULL)
		s_pstDbgSocketTable = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL, SOCKET_FREE);

	pstSocketInfo = (DBG_HXSOCKETInfo_t*)HLIB_HASH_Lookup(s_pstDbgSocketTable, (void*)handle);
	if (pstSocketInfo)
	{
		HxLOG_Warning("Socket DBG is already registered!! [%s]\n", name);
		return;
	}

	if (access(DBG_HXSOCKET_PATH, F_OK) == -1)
	{
		HLIB_DIR_Make(DBG_HXSOCKET_PATH);
		chmod(DBG_HXSOCKET_PATH, 0777);
	}

	p = HxSTD_StrRChar(name, '/');
	if (p == NULL)
		p = name;

	HxSTD_snprintf(szFileName, 256, "%s%s.%d", DBG_HXSOCKET_PATH, p, getpid());
	pstSocketInfo = (DBG_HXSOCKETInfo_t*)SOCKET_MALLOC(sizeof(DBG_HXSOCKETInfo_t));
	pstSocketInfo->file = fopen(szFileName, "wb");
	pstSocketInfo->nItemIndex = 0;
	HxSTD_MemSet(pstSocketInfo->aucData, 0, DBG_HXSOCKET_ITEMSIZE * 2);

	HLIB_HASH_Insert(s_pstDbgSocketTable, (void*)handle, (void*)pstSocketInfo);
}

static void	dbg_socket_write(HINT32 fd, const HUINT8 *pucData, HINT32 size, HBOOL bSend)
{
	HCHAR				szInfo[256];
	DBG_HXSOCKETInfo_t	*pstSocketInfo;

	pstSocketInfo = (DBG_HXSOCKETInfo_t*)HLIB_HASH_Lookup(s_pstDbgSocketTable, (void*)fd);
	if (pstSocketInfo == NULL)
		return;

	HxSTD_MemSet(pstSocketInfo->aucData, 0, (DBG_HXSOCKET_ITEMSIZE + DBG_HXSOCKET_ITEMPADDING));
	HxSTD_MemSet(pstSocketInfo->aucData + (DBG_HXSOCKET_ITEMSIZE + DBG_HXSOCKET_ITEMPADDING), 0xFF, (DBG_HXSOCKET_ITEMSIZE + DBG_HXSOCKET_ITEMPADDING));
	HxSTD_MemCopy(pstSocketInfo->aucData, pucData, (size > DBG_HXSOCKET_ITEMSIZE) ? DBG_HXSOCKET_ITEMSIZE : size);
	if (pstSocketInfo->nItemIndex >= DBG_HXSOCKET_QUEUESIZE)
	{
		fseek(pstSocketInfo->file, 0, SEEK_SET);
		pstSocketInfo->nItemIndex = 0;
	} else
	{
		fseek(pstSocketInfo->file, -(DBG_HXSOCKET_ITEMSIZE + DBG_HXSOCKET_ITEMPADDING + 128), SEEK_CUR);
	}

	HxSTD_MemSet(szInfo, 0, 128);
	HxSTD_snprintf(szInfo, 128, "%s (%d) bytes : tick (%d)ms", (bSend ? "send" : "recv"), size, HLIB_STD_GetSystemTick());
	fwrite(szInfo, 1, 128, pstSocketInfo->file);
	fwrite(pstSocketInfo->aucData, 1, (DBG_HXSOCKET_ITEMSIZE + DBG_HXSOCKET_ITEMPADDING) * 2, pstSocketInfo->file);
	fflush(pstSocketInfo->file);

	pstSocketInfo->nItemIndex++;
}
#endif

#if defined(SUPPORT_UDS_SYSTEM)

static HERROR		socket_client_Close(HxSocket_t *info);
static HERROR		socket_server_Close(HxSocket_t *info);

#if defined(SUPPORT_LINUX_TIMER)
static HINT32	socket_CompareReftime (const struct timespec *lhs, const struct timespec *rhs)
{
	if (lhs->tv_sec > rhs->tv_sec)
		return +1;
	if (lhs->tv_sec < rhs->tv_sec)
		return -1;
	if (lhs->tv_nsec > rhs->tv_nsec)
		return +1;
	if (lhs->tv_nsec < rhs->tv_nsec)
		return -1;
	return 0;
}

static HINT32	socket_CompareData (const HxSocket_Data_t *lhs, const HxSocket_Data_t *rhs)
{
	return socket_CompareReftime(&(lhs->header.refTime), &(rhs->header.refTime));
}

#elif defined(SUPPORT_VK_TIMER)

static HINT32	socket_CompareData (const HxSocket_Data_t *lhs, const HxSocket_Data_t *rhs)
{
	return lhs->header.refTime < rhs->header.refTime;
}
#endif

static HxSocket_Data_t *socket_AllocData (HxSocket_t *info, HUINT32 size)
{
	HxSocket_Data_t	*data;
	HUINT32			alloc;

	if (size <= SOCKET_DATA_SIZE)
	{
		data = (HxSocket_Data_t *)HLIB_QUEUE_Pop(info->freed);
		if (data)
		{
			data->size = size;
			return data;
		}
		alloc = SOCKET_DATA_SIZE;
	} else alloc = size;

#if defined(CONFIG_DEBUG)
	data = (HxSocket_Data_t *)DS_MEM_Malloc(sizeof(HxSocket_Data_t) + alloc, info, info->debugText, info->debugLine);
#else
	data = (HxSocket_Data_t *)SOCKET_MALLOC(sizeof(HxSocket_Data_t) + alloc);
#endif
	if (data)
	{
		data->size = size;
		data->allocated = alloc;
	}
	return data;
}

static void			socket_FreeDataCB (HxSocket_t *info, HxSocket_Data_t *data)
{
	if (data)
	{
		if (data->allocated <= SOCKET_DATA_SIZE)
		{
			HLIB_QUEUE_Push(info->freed, data);
		}
		else
		{
			DS_MEM_Free(data,info,info->debugText,info->debugLine);
		}
	}
}

static void			socket_FreeData (HxSocket_t *info, HxSocket_Data_t *data)
{
	if (data)
	{
		if (data->allocated <= SOCKET_DATA_SIZE)
		{
			HLIB_QUEUE_Push(info->freed, data);
		}
		else
		{
#ifdef	CONFIG_DEBUG
			DS_MEM_Free(data,info,info->debugText,info->debugLine);
#else
			SOCKET_FREE(data);
#endif
		}
	}
}

static const HCHAR *	socket_ToString (HxSocket_t *sock)
{
	HCHAR	*desc;

#ifdef	CONFIG_DEBUG
	desc = sock->description;
#else
	static HCHAR	s_description[128];
	desc = s_description;
	desc[0] = '\0';
#endif

	if (HxSTD_StrEmpty(desc))
	{
		if (sock->type == HxSOCKET_UDS)
		{
			HxSTD_PrintToStrN(desc, 128
						, "UDS %s{fd(%d:0x%08X);addr(%s)}"
						, SOCKET_IS_CLIENT(sock) ? "CLIENT" : "SERVER"
						, sock->fd
						, sock->flags
						, sock->addr.udspath
					);
		}
		else
		{
			HxSTD_PrintToStrN(desc, 128
						, "INET %s{fd(%d:0x%08X);port(%d)}"
						, SOCKET_IS_CLIENT(sock) ? "CLIENT" : "SERVER"
						, sock->fd
						, sock->flags
						, sock->addr.port
					);
		}
	}
	return desc;
}

static HxSocket_t *	socket_Open (HINT32 fd, HUINT32 size, HxSocket_Flags_e flags, HxSocket_Type_e type, const HCHAR *pszIpAddr, void *address, const HCHAR * file, HUINT32 line)
{
	HxSocket_t	*sock;

#if defined(CONFIG_DEBUG)
	sock = (HxSocket_t *)DS_MEM_Calloc(size,NULL, file, line);
#else
	sock = (HxSocket_t *)SOCKET_CALLOC(size);
#endif

	if (sock)
	{
		sock->fd    = fd;
		sock->flags = flags;
		sock->type  = type;

		if (HxSTD_StrEmpty(pszIpAddr) == TRUE)
			HLIB_STD_StrNCpySafe(sock->szIpAddr, "127.0.0.1", 32);
		else
			HLIB_STD_StrNCpySafe(sock->szIpAddr, pszIpAddr, 32);
#if defined(CONFIG_DEBUG)
		sock->debugText = (HCHAR *)file;
		sock->debugLine = line;
#endif

		if (type == HxSOCKET_UDS)
		{
#if defined(CONFIG_DEBUG)
			sock->addr.udspath = DS_MEM_StrDup((const HCHAR *)address,(void *)sock,file,line);
#else
			sock->addr.udspath = SOCKET_STRDUP(address);
#endif
		}
		else //if (type == HxSOCKET_INET)
			sock->addr.port = (HINT32)address;

		sock->freed = HLIB_QUEUE_New(SOCKET_FREE_CB);
		sock->fifoQ = HLIB_PRIORITYQUEUE_New((HxCompareFunc_t)socket_CompareData, (HxFreeFunc_t)socket_FreeDataCB);
	}
	return sock;
}

static void		socket_Close (HxSocket_t *sock)
{
	if (sock)
	{
		if (sock->fd >= 0)
			close(sock->fd);

		if (sock->type == HxSOCKET_UDS && sock->addr.udspath)
		{
#if defined(CONFIG_DEBUG)
			DS_MEM_Free(sock->addr.udspath,sock,sock->debugText,sock->debugLine);
#else
			SOCKET_FREE(sock->addr.udspath);
#endif
		}
		if (sock->freed)
			HLIB_QUEUE_Delete(sock->freed);
		if (sock->fifoQ)
			HLIB_PRIORITYQUEUE_Delete(sock->fifoQ);
#if defined(CONFIG_DEBUG)
		DS_MEM_Free(sock,sock,sock->debugText,sock->debugLine);
#else
		SOCKET_FREE(sock);
#endif
	}
}

#if defined(OS_LINUX)

static HxSocket_Result_t	socket_ReadFromSocket(HINT32 fd, void *pvData, HUINT32 size)
{
	HUINT8		*pucData = (HUINT8*)pvData;
	HINT32		ret;
	HINT32		nReadSize, nRemainedSize;

	nRemainedSize = (HINT32)size;
	nReadSize = 0;
	while (nRemainedSize)
	{
		ret = read(fd, pucData, nRemainedSize);
		if (ret < 0)
		{
			if (errno == EBADF || errno == EFAULT || errno == EINVAL || errno == EIO || errno == ENOSPC || errno == EPIPE)
			{

				HxLOG_Critical("Read Error : read bad block!! - [%s] - (%s)\n", strerror(errno), HLIB_STD_GetCurProcessName());
				return SOCKET_RESULT_DISCONNECTED;
			}

			HxLOG_Warning("Socket Read Retry : read (%d / %d) - (%s)\n", nReadSize, size, HLIB_STD_GetCurProcessName());
			HLIB_LOG_Dump(pvData, size, 0, 0);
			continue;
		}

		nRemainedSize -= ret;
		nReadSize     += ret;
		pucData       += ret;

		if (ret == 0)
		{
			//	data is finished.

			if (nReadSize == 0)
			{
				//	Socket is closed
				return SOCKET_RESULT_DISCONNECTED;
			}

			if (nRemainedSize)
			{
				HxLOG_Critical("Socket Data received error...\n");
				HxLOG_Assert(!nRemainedSize);
			}
		}
	}
	return SOCKET_RESULT_OK;
}


static HERROR	socket_WriteToSocket(HINT32 fd, const void *pvData, HUINT32 size)
{
	HINT32	written;
	HINT32	remain;
	const HUINT8 *p =(const HUINT8 *)pvData;

//	HxLOG_Debug("WriteData [%d] (%d) [%X]\n", fd, size, (int)pvData);
	remain = (HINT32)size;
	do
	{
		written = write(fd, p, remain);
		if (written < 0)
		{
			if (errno == EBADF || errno == EFAULT || errno == EINVAL || errno == EIO || errno == ENOSPC || errno == EPIPE || errno == EAGAIN)
			{
				HxLOG_Print("Fail to Write Data... [%s]\n", strerror(errno));
				return ERR_FAIL;
			}

			HxLOG_Print("write retry.... remained(%d) [%x : %x]\n", remain, (int)p, (int)pvData);
#ifdef CONFIG_DEBUG
			HLIB_LOG_Dump((HUINT8 *)pvData, remain, 0, 0);
#endif
			continue;
		}
		p += written;
		remain -= written;
	} while (remain > 0);

	return ERR_OK;
}
#endif

#if defined(OS_WIN32)
static HxSocket_Result_t	socket_ReadFromSocket(HINT32 fd, void *pvData, HUINT32 size)
{
	HINT32 ret = 0;

	ret = recv(fd, pvData, size, 0);
	if (ret < 0)
	{
		return SOCKET_RESULT_DISCONNECTED;
	}

	return SOCKET_RESULT_OK;
}

static HERROR	socket_WriteToSocket(HINT32 fd, const void *pvData, HUINT32 size)
{
	HINT32 ret = 0;

	ret = send(fd, pvData, size, 0);
	if (ret < 0)
	{
		return SOCKET_RESULT_DISCONNECTED;
	}

	return SOCKET_RESULT_OK;
}
#endif

static HERROR		socket_Write(HxSocket_t *info, const void *pvData, HUINT32 size, HUINT32 requestId)
{
	HERROR	err;
	HINT32	fd;
	HxSocket_Header_t	header;

	fd = info->fd;
	if (fd < 0)	// not connected
		return ERR_FAIL;

	header.magicCode = SOCKET_MAGIC_CODE;
	header.size      = size;
	header.requestId = requestId;

	#if defined(SUPPORT_LINUX_TIMER)
	if (clock_gettime(CLOCK_MONOTONIC, &header.refTime) != 0)
	{
		header.refTime.tv_sec  = 0;
		header.refTime.tv_nsec = 0;
	}
	#endif
	#if defined(SUPPORT_VK_TIMER)
	header.refTime	 = VK_TIMER_GetSystemTick();
	#endif

	err = socket_WriteToSocket(fd, &header, sizeof(HxSocket_Header_t));
	if (err == ERR_OK)
	{
		err = socket_WriteToSocket(fd, pvData, size);
#ifdef	CONFIG_DEBUG_COMM
		dbg_socket_write(fd, pvData, size, TRUE);
#endif
	}

	if (err != ERR_OK)
	{
		HxLOG_Error("%s():%d SOCKET WRITE ERROR!!!!!!\n", __FUNCTION__, __LINE__);
	}
	return err;
}

static HINT32	socket_client_ListenOnce(
							  HxSocket_t *info
							, HUINT32 timeout
							, HxSocket_DataListener_t cbPacketReceived
							, HxSocket_StatusListener_t cbNotifyStatus
							, void *userData
						)
{
	HINT32	fd;
	#if defined(SUPPORT_LINUX_TIMER)
	fd_set	read_fds;
	struct timeval	timev;
	struct timeval	*waitTime;
	#endif
	int		ret = 0;
	HxSocket_Header_t	header;
	HxSocket_Result_t	result;

	HxLOG_Assert(info);
	fd = info->fd;

	#if defined(SUPPORT_LINUX_TIMER)
	FD_ZERO(&read_fds);
	FD_SET(fd, &read_fds);

	if (timeout == SOCKET_FOREVER)
	{
		waitTime = NULL;
	}
	else
	{
		timev.tv_sec  = timeout / 1000;
		timev.tv_usec = (timeout % 1000) * 1000;
		waitTime = &timev;
	}

	// user privilege로 동작시 select시 EINTR error가 발생해서, 통신 실패가 발생해서, EINTR발생하면, 재시도하는 코드 추가
	do {
		ret = select(fd + 1, &read_fds, (fd_set*)0, (fd_set*)0, waitTime);
	} while (ret == -1 && errno == EINTR);
	if (ret < 0)
	{
		HxLOG_Critical("Selection error!!!\n");
		return -1;
	}
	if (ret == 0)
		return 0;

	if (!FD_ISSET(fd, &read_fds))
		return 0;
	#endif

	info->flags |= eHxSOCKET_UNDEAD;

	result = socket_ReadFromSocket(fd, &header, sizeof(HxSocket_Header_t));
	if (result == SOCKET_RESULT_OK)
	{
		HxSocket_Data_t	*rxdata;

		if (header.magicCode != SOCKET_MAGIC_CODE)
		{
			HxLOG_Critical("%s() magiccode does NOT matched!!! (%X != %X)\n", __FUNCTION__, header.magicCode, SOCKET_MAGIC_CODE);
			info->flags &= ~eHxSOCKET_UNDEAD;
			return -1;
		}

		rxdata = socket_AllocData(info, header.size);
		if (rxdata)
		{
			result = socket_ReadFromSocket(fd, rxdata->data, header.size);
#ifdef	CONFIG_DEBUG_COMM
			dbg_socket_write(fd, rxdata->data, header.size, FALSE);
#endif
			HxLOG_Debug("client packet received : \n -- size: %u\n", header.size);
			if ((result == SOCKET_RESULT_OK) && (cbPacketReceived))
				cbPacketReceived(userData, (HINT32)info, header.requestId, header.size, (void*)rxdata->data);
			socket_FreeData(info, rxdata);
		}
	}

	switch (result)
	{
	case SOCKET_RESULT_DISCONNECTED:
		HxLOG_Critical("client is disconnected!!! - [%s]\n", HLIB_STD_GetCurProcessName());
		if (cbNotifyStatus)
			cbNotifyStatus(userData, HxSOCKET_DISCONNECT_CLIENT, (HINT32)info);
		info->fd = -1;
		SOCKET_CLOSE(fd);
		info->flags |= eHxSOCKET_ERROR;
		break;

	case SOCKET_RESULT_FAIL:
		HxLOG_Critical("Abnormal Error is occurred...\n");
		HxLOG_Assert(0);
		break;

	default:
		break;
	}
	info->flags &= ~eHxSOCKET_UNDEAD;
	if (info->flags & eHxSOCKET_ZOMBIE)
	{
		socket_client_Close(info);
		return -1;
	}
	return (HINT32)ret;
}

#if defined(OS_LINUX)
static HINT32	socket_server_ListenOnce(
							HxSocket_t *info
							, HUINT32 timeout
							, HxSocket_DataListener_t cbPacketReceived
							, HxSocket_StatusListener_t cbNotifyStatus
							, void *userData
							, const HCHAR * file, HUINT32 line
						)
{
	HINT32				i, n, monitorFD;
	fd_set				read_fds;
	struct timeval		timev;
	struct timeval		*waitTime;
	HxSocket_Result_t	result;
	HxSocket_Server_t	*server;
	HxSocket_t			*client;
	HxSocket_Data_t		*rxdata;
	HxSocket_Header_t	header;
	HINT32				count;
	HINT32				ret;
	int					selected;

	HxLOG_Assert(info);

	server = (HxSocket_Server_t *)info;

	FD_ZERO(&read_fds);
	FD_SET(info->fd, &read_fds);

	//	Set flag for connected clients
	monitorFD = info->fd;
	HxVECTOR_EACH(server->clients, HxSocket_t *, client, {
		FD_SET(client->fd, &read_fds);
		if (monitorFD < client->fd)
			monitorFD = client->fd;
	});

	if (timeout == SOCKET_FOREVER)
	{
		waitTime = NULL;
	}
	else
	{
		timev.tv_sec  = timeout / 1000;
		timev.tv_usec = (timeout % 1000) * 1000;
		waitTime = &timev;
	}

	monitorFD = monitorFD + 1;
	// user privilege로 동작시 select시 EINTR error가 발생해서, 통신 실패가 발생해서, EINTR발생하면, 재시도하는 코드 추가
	do {
		selected  = select(monitorFD, &read_fds, (fd_set*)0, (fd_set*)0, waitTime);
	} while (selected == -1 && errno == EINTR);
	if (selected < 0)
	{
		HxLOG_Critical("Fail to Select Function...\n");
		return -1;
	}
	if (selected == 0)
		return 0;

	ret = 0;
	do
	{
		ret += (HINT32)selected;

		if (FD_ISSET(info->fd, &read_fds))
		{
			struct sockaddr_in	addr;
			HINT32	fd;
			socklen_t addrsize = sizeof(struct sockaddr_in);

			fd = accept(info->fd, (struct sockaddr*)&addr, &addrsize);
			if (fd == -1)
			{
				HxLOG_Critical("Fail to Accept Client...\n");
				return -1;
			}

			HxLOG_Debug("request client to connect server module...\n");
			client = socket_Open(fd, sizeof(HxSocket_Client_t)
						, eHxSOCKET_CLIENT
						, info->type
						, info->szIpAddr
						, (void *)info->addr.udspath, file, line
					);
			if (client)
			{
				HLIB_VECTOR_Add(server->clients, client);
			}
			if (cbNotifyStatus)
				cbNotifyStatus(userData, HxSOCKET_ADD_CLIENT, (HINT32)client);
		}

		count = 0;
		n = HLIB_VECTOR_Length(server->clients);
		for (i = 0 ; i < n ; i++)
		{
			client = (HxSocket_t *)HLIB_VECTOR_ItemAt(server->clients, i);
			if (!FD_ISSET(client->fd, &read_fds))
				continue;

			result = socket_ReadFromSocket(client->fd, &header, sizeof(HxSocket_Header_t));
			if (result == SOCKET_RESULT_OK)
			{
				if (header.magicCode != SOCKET_MAGIC_CODE)
				{
					HxLOG_Critical("%s() magiccode does NOT matched!!! (%X != %X)\n", __FUNCTION__, header.magicCode, SOCKET_MAGIC_CODE);
					continue;
				}
				rxdata = socket_AllocData(info, header.size);
				if (rxdata == NULL)
					continue;

				result = socket_ReadFromSocket(client->fd, rxdata->data, header.size);
#ifdef	CONFIG_DEBUG_COMM
				dbg_socket_write(client->fd, rxdata->data, header.size, FALSE);
#endif

				if (result == SOCKET_RESULT_OK)
				{
					memcpy(&rxdata->header, &header, sizeof(HxSocket_Header_t));
					rxdata->client = (HINT32)client;
					HLIB_PRIORITYQUEUE_Push(info->fifoQ, (void *)rxdata);
					HxLOG_Debug("Packet Received... requestId:0x%X, refTime:%d\n", header.requestId, header.refTime.tv_sec);
				}
				else
				{
					socket_FreeData(info, rxdata);
				}
			}
			switch (result)
			{
			case SOCKET_RESULT_DISCONNECTED:
				if (cbNotifyStatus)
					cbNotifyStatus(userData, HxSOCKET_REMOVE_CLIENT, (HINT32)client);
				HLIB_VECTOR_Detach(server->clients, i);
				HLIB_QUEUE_Push(server->disconnected, (void *)client);
				client->flags |= eHxSOCKET_ERROR;
				FD_CLR(client->fd, &read_fds);
				n--; i--;
				break;

			case SOCKET_RESULT_FAIL:
				HxLOG_Critical("Abnormal Error is occurred...\n");
				HxLOG_Assert(0);
				break;

			default:
				break;
			}
		}
		if (selected == 1)
			break;

		timev.tv_sec  = 0;
		timev.tv_usec = 0;
	} while ((selected = select(monitorFD, &read_fds, (fd_set*)0, (fd_set*)0, &timev)) > 0);

	while ((rxdata = (HxSocket_Data_t *)HLIB_PRIORITYQUEUE_Pop(info->fifoQ)) != NULL)
	{
		if (cbPacketReceived)
			cbPacketReceived(
				userData, rxdata->client, rxdata->header.requestId, rxdata->header.size, rxdata->data
			);
		socket_FreeData(info, rxdata);
	}
	return ret;
}



static HINT32	socket_client_Connect (HxSocket_Type_e type, const HCHAR *pszIPAddr, const void *target)
{
	HINT32 fd;
	struct sockaddr_un	addr_un;
	struct sockaddr_in	addr_in;

	fd = -1;
	switch (type)
	{
	case HxSOCKET_UDS:
		fd = socket(PF_FILE, SOCK_STREAM, 0);
		if (fd == -1)
		{
			HxLOG_Error("Fail to make socket [%s]\n", target);
			break;
		}

		memset(&addr_un, 0, sizeof(struct sockaddr_un));
		addr_un.sun_family = AF_UNIX;
		HLIB_STD_StrNCpySafe(addr_un.sun_path, target, 108);

		if (connect(fd, (struct sockaddr *)&addr_un, sizeof(struct sockaddr_un)) < 0)
			break;
		return fd;

	case HxSOCKET_INET:
		fd = socket(PF_INET, SOCK_STREAM, 0);
		if (fd == -1)
		{
			HxLOG_Error("Fail to Create socket Port [%d]\n", (int)target);
			break;
		}

		memset(&addr_in, 0, sizeof(struct sockaddr_in));
		addr_in.sin_family		= AF_INET;
		addr_in.sin_port		= htons((HINT32)target);
		if (HxSTD_StrEmpty(pszIPAddr) == TRUE)
			inet_pton(AF_INET, "127.0.0.1", &addr_in.sin_addr);
		else
			inet_pton(AF_INET, pszIPAddr, &addr_in.sin_addr);
//		addr_in.sin_addr.s_addr	= INADDR_ANY;

		if (connect(fd, (struct sockaddr*)&addr_in, sizeof(struct sockaddr_in)) < 0)
			break;
		return fd;

	default:
		break;
	}
	if (fd != -1)
		close(fd);
	return -1;
}
#endif

#if defined(OS_WIN32)
typedef struct
{
	 HxSocket_t *info;
	HUINT32 timeout;
	HxSocket_DataListener_t cbPacketReceived;
	HxSocket_StatusListener_t cbNotifyStatus;
	void *userData;
	const HCHAR * file;
	HUINT32 line;
} SockServer_Task_t;

typedef struct
{
	HCHAR *pServerName;
	HINT32	nPort;
} SockServerNameTable_t;

const static SockServerNameTable_t s_pSockServerNameTable[] =
{
	{"/tmp/.comma_signal.tmp",			18910},
	{"/tmp/.dama_signal.tmp", 			18911},
	{"/tmp/.hama_signal.tmp", 			18912},
	{"/tmp/.hamalauncher_signal.tmp", 	18913},
	{"/tmp/.homma_signal.tmp", 			18914},
	{"/tmp/.imma_signal.tmp", 			18915},
	{"/tmp/.inoma_signal.tmp", 			18916},
	{"/tmp/.nima_signal.tmp", 			18917},
	{"/tmp/.obama_signal.tmp", 			18918},
	{"/tmp/.pama_signal.tmp", 			18919},
	{"/tmp/.rema_signal.tmp", 			18920},
	{"/tmp/.sama_signal.tmp", 			18921},
	{"/tmp/.secma_signal.tmp", 			18922},
	{"/tmp/.thma_signal.tmp", 			18923},
	{"/tmp/.tima_signal.tmp", 			18924},
	{"/tmp/.umma_signal.tmp", 			18925},
};

static HINT32	socket_GetPortByServerName(HCHAR *pServerName)
{
	HINT32	i = 0;
	HINT32	port = 19000;

	if(NULL == pServerName)
	{
		HxLOG_Assert(0);
		return 19999;
	}

	for(i=0; i < sizeof(s_pSockServerNameTable)/sizeof(SockServerNameTable_t); i++)
	{
		if(0 == HxSTD_StrCmp(s_pSockServerNameTable[i].pServerName, pServerName))
		{
			return s_pSockServerNameTable[i].nPort;
		}
	}

	for(i=0; 0 != pServerName[i]; i++)
	{
		port += pServerName[i];
	}

	HxLOG_Error("=============================================================================\n");
	HxLOG_Error("Unknown Server name : %s, temp port number : %d\n", pServerName, port);
	HxLOG_Error("=============================================================================\n");

	return port;
}

static HINT32	socket_server_ListenOnce_Server(
										void *argv)

{
	SOCKADDR_IN	addr;
	SOCKET	fd;
	HINT32 addrsize = sizeof(SOCKADDR_IN);
	HxSocket_t		*client;

	SockServer_Task_t *pTaskInfo = argv;

	HxSocket_t *info = pTaskInfo->info;
//	HUINT32 timeout = pTaskInfo->timeout;
//	HxSocket_DataListener_t cbPacketReceived = pTaskInfo->cbPacketReceived;
	HxSocket_StatusListener_t cbNotifyStatus = pTaskInfo->cbNotifyStatus;
	void *userData = pTaskInfo->userData;
	const HCHAR * file = pTaskInfo->file;
	HUINT32 line = pTaskInfo->line;

	HxSocket_Server_t *server = (HxSocket_Server_t *)info;

	while(1)
	{
		fd = accept(info->fd, (struct sockaddr*)&addr, &addrsize);
		if (fd == INVALID_SOCKET)
		{
			HxLOG_Critical("Fail to Accept Client...\n");
			return -1;
		}

		HxLOG_Debug("request client to connect server module...\n");
		client = socket_Open(fd, sizeof(HxSocket_Client_t)
					, eHxSOCKET_CLIENT
					, info->type
					, info->szIpAddr
					, (void *)info->addr.udspath, file, line
				);
		if (client)
		{
			HLIB_VECTOR_Add(server->clients, client);
		}
		if (cbNotifyStatus)
			cbNotifyStatus(userData, HxSOCKET_ADD_CLIENT, (HINT32)client);
	}

	SOCKET_FREE(pTaskInfo);
}


static HINT32	socket_server_ListenOnce(
							HxSocket_t *info
							, HUINT32 timeout
							, HxSocket_DataListener_t cbPacketReceived
							, HxSocket_StatusListener_t cbNotifyStatus
							, void *userData
							, const HCHAR * file, HUINT32 line
						)
{
	HINT32				i, n, monitorFD;
	HxSocket_Result_t	result;
	HxSocket_Server_t	*server;
	HxSocket_t			*client;
	HxSocket_Data_t		*rxdata;
	HxSocket_Header_t	header;
	HINT32				count;
	HINT32				ret;

	HxLOG_Assert(info);

	server = (HxSocket_Server_t *)info;

	//	Set flag for connected clients
	monitorFD = info->fd;

	ret = 0;

	{
		count = 0;
		n = HLIB_VECTOR_Length(server->clients);
		for (i = 0 ; i < n ; i++)
		{
			client = (HxSocket_t *)HLIB_VECTOR_ItemAt(server->clients, i);

			result = socket_ReadFromSocket(client->fd, &header, sizeof(HxSocket_Header_t));
			if (result == SOCKET_RESULT_OK)
			{
				if (header.magicCode != SOCKET_MAGIC_CODE)
				{
					HxLOG_Critical("%s() magiccode does NOT matched!!! (%X != %X)\n", __FUNCTION__, header.magicCode, SOCKET_MAGIC_CODE);
					continue;
				}
				rxdata = socket_AllocData(info, header.size);
				if (rxdata == NULL)
					continue;

				result = socket_ReadFromSocket(client->fd, rxdata->data, header.size);
#ifdef	CONFIG_DEBUG_COMM
				dbg_socket_write(client->fd, rxdata->data, header.size, FALSE);
#endif

				if (result == SOCKET_RESULT_OK)
				{
					memcpy(&rxdata->header, &header, sizeof(HxSocket_Header_t));
					rxdata->client = (HINT32)client;
					HLIB_PRIORITYQUEUE_Push(info->fifoQ, (void *)rxdata);
				}
				else
				{
					socket_FreeData(info, rxdata);
				}
			}
			switch (result)
			{
			case SOCKET_RESULT_FAIL:
			case SOCKET_RESULT_DISCONNECTED:
				if (cbNotifyStatus)
					cbNotifyStatus(userData, HxSOCKET_REMOVE_CLIENT, (HINT32)client);
				HLIB_VECTOR_Detach(server->clients, i);
				HLIB_QUEUE_Push(server->disconnected, (void *)client);
				client->flags |= eHxSOCKET_ERROR;
				n--; i--;
				break;

			default:
				break;
			}
		}
	}

	while ((rxdata = (HxSocket_Data_t *)HLIB_PRIORITYQUEUE_Pop(info->fifoQ)) != NULL)
	{
		if (cbPacketReceived)
			cbPacketReceived(
				userData, rxdata->client, rxdata->header.requestId, rxdata->header.size, rxdata->data
			);
		socket_FreeData(info, rxdata);
	}
	return ret;
}



static HINT32	socket_client_Connect (HxSocket_Type_e type, const HCHAR *pszIPAddr, const void *target)
{
	HINT32 fd = -1;
	SOCKADDR_IN	addr_in;
	WSADATA wsaData;

	fd = -1;
	switch (type)
	{
	case HxSOCKET_UDS:
		{
			HINT32 port = 0;

			port= socket_GetPortByServerName(target);

			if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
				HxLOG_Print ("WSAStartup() error");

			fd = socket(PF_INET, SOCK_STREAM, 0);
			if (fd == -1)
			{
				HxLOG_Error("Fail to Create socket Port [%d]\n", (int)port);
				break;
			}

			memset(&addr_in, 0, sizeof(SOCKADDR_IN));
			addr_in.sin_family		= AF_INET;
			addr_in.sin_port		= htons((HINT32)port);
			addr_in.sin_addr.s_addr = inet_addr("127.0.0.1");
	//		addr_in.sin_addr.s_addr	= INADDR_ANY;

			if (connect(fd, (struct sockaddr*)&addr_in, sizeof(SOCKADDR_IN)) < 0)
				break;
			return fd;
		}

	case HxSOCKET_INET:
		if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
			HxLOG_Print ("WSAStartup() error");

		fd = socket(PF_INET, SOCK_STREAM, 0);
		if (fd == -1)
		{
			HxLOG_Error("Fail to Create socket Port [%d]\n", (int)target);
			break;
		}

		memset(&addr_in, 0, sizeof(SOCKADDR_IN));
		addr_in.sin_family		= AF_INET;
		addr_in.sin_port		= htons((HINT32)target);
		if (HxSTD_StrEmpty(pszIPAddr) == TRUE)
			addr_in.sin_addr.s_addr = inet_addr("127.0.0.1");
		else
			addr_in.sin_addr.s_addr = inet_addr(pszIPAddr);
//		addr_in.sin_addr.s_addr	= INADDR_ANY;

		if (connect(fd, (struct sockaddr*)&addr_in, sizeof(SOCKADDR_IN)) < 0)
			break;
		return fd;

	default:
		break;
	}
	if (fd != -1)
		SOCKET_CLOSE(fd);
	return -1;
}
#endif

static HINT32		socket_client_RepairConnection (HxSocket_t *sock)
{
	sock->fd = socket_client_Connect(sock->type, sock->szIpAddr, (void *)sock->addr.udspath);
	if (sock->fd >= 0)
	{
		HxSocket_Data_t   *data;
		HxSocket_Client_t *client = (HxSocket_Client_t *)sock;

		sock->flags &= ~eHxSOCKET_RECONNECT;

		HxLOG_Warning("%s(%s) connnection repaired. and flush all data(%d bytes) to destination!\n"
					, __FUNCTION__
					, socket_ToString(sock)
					, client->bufferSize
				);

		if (client->bufferQ)
		{
			while ((data = (HxSocket_Data_t *)HLIB_QUEUE_Pop(client->bufferQ)) != NULL)
			{
				(void)socket_Write(sock, data->data, data->size, data->header.requestId);
#if defined(CONFIG_DEBUG)
				DS_MEM_Free(data,sock,sock->debugText,sock->debugLine);
#else
				SOCKET_FREE(data);
#endif
			}
		}
		client->bufferSize = 0;
	}
	return sock->fd;
}

static HERROR	socket_client_Buffering (HxSocket_t *sock, const void *data, HUINT32 size, HUINT32 requestId)
{
	HxSocket_Data_t   *buf;
	HxSocket_Client_t *client;

	buf = socket_AllocData(sock, size);
	if (buf)
	{
		client = (HxSocket_Client_t *)sock;
		if (client->bufferQ == NULL)
		{
			client->bufferQ = HLIB_QUEUE_New(SOCKET_FREE_CB);
			if (client->bufferQ == NULL)
				return ERR_FAIL;
		}

		buf->header.requestId = requestId;
		memcpy(buf->data, data, size);
		HLIB_QUEUE_Push(client->bufferQ, buf);
		client->bufferSize += size;

		HxLOG_Critical("%s(%s) socket doesn't connected yet... buffering... (%u/%u bytes)\n"
					, __FUNCTION__, socket_ToString(sock), size, client->bufferSize);

		if (client->bufferSize > SOCKET_BUFFER_THRESHOLD)
		{
			HxLOG_Critical("%s(%s) socket buffer overflow (threshold:%u)\n"
						, __FUNCTION__, socket_ToString(sock), SOCKET_BUFFER_THRESHOLD);
			sock->flags |= eHxSOCKET_ERROR;
			return ERR_FAIL;
		}
		return ERR_OK;
	}
	return ERR_FAIL;
}


static HxSocket_Client_t*	socket_client_Open(HxSocket_Type_e eSocketType, const HCHAR *pszIPAddr, void *targetPort, HINT32 nTry, const HCHAR * file, HUINT32 line)
{
	HINT32	fd;
	HINT32	failed;
	HxSocket_t	*info;

	failed = 0;
	do
	{
		fd = socket_client_Connect(eSocketType, pszIPAddr, targetPort);
		if (fd != -1)
			break;

		if (eSocketType == HxSOCKET_UDS)
			HxLOG_Critical("%s(UDS, %s) %d Fail to Connect with (\'%s\')\n", __FUNCTION__, targetPort, failed, strerror(errno));
		else
			HxLOG_Critical("%s(INET, %d) %d Fail to Connect with (\'%s\')\n", __FUNCTION__, (int)targetPort, failed, strerror(errno));

		if (nTry == 0)
			break;

		HLIB_STD_TaskSleep(1000);
	} while (++failed < nTry);

	if (nTry > 0 && fd < 0)
		return NULL;

	info = socket_Open(fd, sizeof(HxSocket_Client_t), eHxSOCKET_CLIENT, eSocketType, pszIPAddr, targetPort, file, line);
	if (nTry == 0 && fd < 0)
	{
		info->flags |= eHxSOCKET_RECONNECT;
	}

#ifdef	CONFIG_DEBUG_COMM
	if (eSocketType == HxSOCKET_UDS)
		dbg_socket_register((HCHAR*)targetPort, fd, FALSE);
#endif

	HxLOG_Info("%s(%s) opened!\n", __FUNCTION__, socket_ToString(info));
	return (HxSocket_Client_t *)info;
}


static HERROR		socket_client_Close(HxSocket_t *info)
{
	HxSocket_Client_t *client = (HxSocket_Client_t *)info;

	if (info->flags & eHxSOCKET_UNDEAD)
	{
		info->flags |= eHxSOCKET_ZOMBIE;
		return ERR_OK;
	}

	if (client->bufferQ)
	{
		HLIB_QUEUE_Delete(client->bufferQ);
	}
	socket_Close(info);
	return ERR_OK;
}

static HINT32		socket_client_Listen(
							  HxSocket_t *info
							, HUINT32 timeout
							, HxSocket_DataListener_t cbPacketReceived
							, HxSocket_StatusListener_t cbNotifyStatus
							, void *userData
						)
{
	HINT32	ret = 0;

	do
	{
		if (SOCKET_ERROR(info))
			return -1;

		if (info->flags & eHxSOCKET_RECONNECT)
		{
			if (socket_client_RepairConnection(info) >= 0)
				continue;
			if (timeout == SOCKET_FOREVER)
				HLIB_STD_TaskSleep(10);
		}
		else
		{
			ret = socket_client_ListenOnce(info, timeout, cbPacketReceived, cbNotifyStatus, userData);
		}
	} while (ret >= 0 && timeout == SOCKET_FOREVER);

	return ret;
}

#if defined(OS_LINUX)

static HxSocket_Server_t*	socket_server_Open(HxSocket_Type_e eSocketType, void *serverPort, const HCHAR * file, HUINT32 line)
{
	HINT32	err;
	HINT32	fd = -1;
	HxSocket_t        *sock;
	HxSocket_Server_t *server;

	if (eSocketType == HxSOCKET_UDS)
	{
		HCHAR	*serverName = (HCHAR*)serverPort;
		struct	sockaddr_un		server_addr;

		if (access(serverName, F_OK) == 0)
		{
			if (unlink(serverName))
			{
				HxLOG_Error("Fail to Create socket named [%s]\n", serverName);
				return NULL;
			}
		}

		fd = socket(AF_UNIX, SOCK_STREAM, 0);
		if (fd == -1)
		{
			HxLOG_Error("Fail to Create socket named [%s]\n", serverName);
			return NULL;
		}
		HxSTD_MemSet(&server_addr, 0, sizeof(server_addr));
		server_addr.sun_family = AF_UNIX;
		HxSTD_StrNCpy(server_addr.sun_path, serverName, (sizeof(server_addr.sun_path) - 1));

		err = bind(fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	}
	else if (eSocketType == HxSOCKET_INET)
	{
		HINT32	serverPortNum = (HINT32)serverPort;
		struct	sockaddr_in		server_addr;

		fd = socket(PF_INET, SOCK_STREAM, 0);
		if (fd == -1)
		{
			HxLOG_Error("Fail to Create socket Port [%d]\n", serverPortNum);
			return NULL;
		}
		HxSTD_MemSet(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family		= AF_INET;
		server_addr.sin_port		= htons(serverPortNum);
		server_addr.sin_addr.s_addr	= INADDR_ANY;

		err = bind(fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	}
	else
	{
		HxLOG_Critical("Not Supported Protocol!!!\n");
		return NULL;
	}

	if (-1 == err)
	{
		HxLOG_Error("Fail to bind socket errno (%d) - (%s)\n", errno, strerror(errno));
		close(fd);
		return NULL;
	}

	if (eSocketType == HxSOCKET_UDS)
	{
		err = chmod(serverPort, 0777);
	    if (err < 0)
		{
			HxLOG_Error("error in chmod (0x%x)\n", err);
			close(fd);
			return NULL;
		}
	}

	listen(fd, SOCKET_BACKLOG_NUM);

	sock = socket_Open(fd, sizeof(HxSocket_Server_t), eHxSOCKET_SERVER, eSocketType, NULL, serverPort, file, line);

#ifdef	CONFIG_DEBUG_COMM
	if (eSocketType == HxSOCKET_UDS)
		dbg_socket_register((HCHAR*)serverPort, fd, TRUE);
#endif
	server = (HxSocket_Server_t *)sock;
	server->clients = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))socket_client_Close, NULL);
	server->disconnected = HLIB_QUEUE_New((HxQueue_RemoveCallback_t)socket_client_Close);
	return server;
}
#endif

#if defined(OS_WIN32)
static HxSocket_Server_t*	socket_server_Open(HxSocket_Type_e eSocketType, void *serverPort, const HCHAR * file, HUINT32 line)
{
	HINT32	err;
	HINT32	fd = -1;
	HxSocket_t        *sock;
	HxSocket_Server_t *server;
	WSADATA wsaData;

	if (eSocketType == HxSOCKET_UDS)
	{
		HINT32	port = 0;
		struct	sockaddr_in 	server_addr;

		port= socket_GetPortByServerName(serverPort);

		if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
			HxLOG_Print( "WSAStartup() error!" );

		fd = socket(PF_INET, SOCK_STREAM, 0);
		if (fd == -1)
		{
			HxLOG_Error("Fail to Create socket Port [%d]\n", port);
			return NULL;
		}
		HxSTD_MemSet(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family		= AF_INET;
		server_addr.sin_port		= htons(port);
		server_addr.sin_addr.s_addr	= INADDR_ANY;

		err = bind(fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	}
	else if (eSocketType == HxSOCKET_INET)
	{
		HINT32	serverPortNum = (HINT32)serverPort;
		struct	sockaddr_in		server_addr;

		if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
			HxLOG_Print( "WSAStartup() error!" );

		fd = socket(PF_INET, SOCK_STREAM, 0);
		if (fd == -1)
		{
			HxLOG_Error("Fail to Create socket Port [%d]\n", serverPortNum);
			return NULL;
		}
		HxSTD_MemSet(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family		= AF_INET;
		server_addr.sin_port		= htons(serverPortNum);
		server_addr.sin_addr.s_addr	= INADDR_ANY;

		err = bind(fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	}
	else
	{
		HxLOG_Critical("Not Supported Protocol!!!\n");
		return NULL;
	}

	if (-1 == err)
	{
		HxLOG_Error("Fail to bind socket errno (%d) - (%s)\n", errno, strerror(errno));
		SOCKET_CLOSE(fd);
		return NULL;
	}

	listen(fd, SOCKET_BACKLOG_NUM);

	sock = socket_Open(fd, sizeof(HxSocket_Server_t), eHxSOCKET_SERVER, eSocketType, NULL, serverPort, file, line);

#ifdef	CONFIG_DEBUG_COMM
	if (eSocketType == HxSOCKET_UDS)
		dbg_socket_register((HCHAR*)serverPort, fd, TRUE);
#endif
	server = (HxSocket_Server_t *)sock;
	server->clients = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))socket_client_Close, NULL);
	server->disconnected = HLIB_QUEUE_New((HxQueue_RemoveCallback_t)socket_client_Close);
	return server;
}
#endif

static HERROR		socket_server_Close(HxSocket_t *info)
{
	HxSocket_Data_t   *data;
	HxSocket_Server_t *server;

	server = (HxSocket_Server_t *)info;

	while ((data = (HxSocket_Data_t *)HLIB_PRIORITYQUEUE_Pop(info->fifoQ)) != NULL)
		socket_FreeData(info, data);

	HLIB_VECTOR_Delete(server->clients);
	HLIB_QUEUE_Delete(server->disconnected);
	socket_Close(info);
	return ERR_OK;
}

static HINT32		socket_server_Listen(
							  HxSocket_t *info
							, HUINT32 timeout
							, HxSocket_DataListener_t cbPacketReceived
							, HxSocket_StatusListener_t cbNotifyStatus
							, void *userData, const HCHAR * file, HUINT32 line
	)
{
	HINT32	ret;
#if defined(OS_WIN32)
	HUINT32 ulTaskId = 0;

	SockServer_Task_t *pTask = SOCKET_MALLOC(sizeof(SockServer_Task_t));

	pTask->info = info;
	pTask->timeout = timeout;
	pTask->cbPacketReceived = cbPacketReceived;
	pTask->cbNotifyStatus = cbNotifyStatus;
	pTask->userData = userData;
	pTask->file = file;
	pTask->line = line;

	VK_TASK_Create(socket_server_ListenOnce_Server, 40, SIZE_256K,
						"server", pTask, &ulTaskId, 0);
	VK_TASK_Start (ulTaskId);
#endif
	do
	{
		if (SOCKET_ERROR(info))
			return -1;

		ret = socket_server_ListenOnce(info, timeout, cbPacketReceived, cbNotifyStatus, userData, file, line);
		if (timeout == SOCKET_FOREVER)
			HLIB_STD_TaskSleep(10);

	} while(timeout == SOCKET_FOREVER);

	return ret;
}


#elif defined(SUPPORT_MSG_SYSTEM)
#include	<_hlib_hash.h>

typedef struct tagHxMsgSocketContent_t
{
	HxHASH_t	*pHash;

} HxMsgSocketContent_t;

typedef struct tagHxMsgSocketData_t
{
	HxSocket_Flags_e	eFlags;
	HxSocket_Status_e 	eStatus;
	HBOOL	bIsData;
	void 	*buf;
	HINT32 	size;
	HUINT32	reqId;
} HxMsgSocketData_t;


STATIC HULONG					s_ulMsgSockSemdId = 0;
STATIC HxMsgSocketContent_t		s_stSockContent;

STATIC HxMsgSocketContent_t *msgsocket_content_Get(void)
{
	if(0 == s_ulMsgSockSemdId)
	{
		HINT32 nRet = 0;

		nRet = HxSEMT_Create(&s_ulMsgSockSemdId, "HxMsgScokSem", HxSEMT_FIFO);
		if(nRet)
		{
			return NULL;
		}

		if(NULL == s_stSockContent.pHash)
			s_stSockContent.pHash = HLIB_HASH_New(HLIB_HASH_StrHash, HLIB_HASH_StrEqual);
	}

	return &s_stSockContent;
}



STATIC void		msgsocket_hash_MakeKey(HCHAR *ret_Key, HxSocket_Type_e eSocketType, const HCHAR *pszIPAddr, void *targetPort)
{
	HCHAR 		*addr = NULL;

	addr = pszIPAddr;
	if (HxSTD_StrEmpty(addr) == TRUE)
	{
		addr = "127.0.0.1";
	}

	switch(eSocketType)
	{
	case HxSOCKET_UDS:
		HxSTD_snprintf(ret_Key, 128, "UDS//%s:%s", addr, (HCHAR *)targetPort);
		break;

	case HxSOCKET_INET:
		HxSTD_snprintf(ret_Key, 128, "INET//%s:%d", addr, (HINT32)targetPort);
		break;

	default:
		break;
	}
}

STATIC void 	msgsocket_SendMessage(HINT32 fd, HxList_t *root, HxSocket_Flags_e eFlags, HxSocket_Status_e eStatus, void *buf, HINT32 size, HINT32 reqId)
{
	HxList_t			*pList = NULL;
	HxSocket_t			*childInfo = NULL;
	HxMsgSocketData_t 	stData;

	HxSTD_memset(&stData, 0, sizeof(HxMsgSocketData_t));

	stData.eFlags	= eFlags;
	stData.eStatus	= eStatus;
	stData.buf		= buf;
	stData.size 	= size;
	stData.reqId	= reqId;
	stData.bIsData	= (size == 0) ? FALSE : TRUE;

	pList = root;
	while(pList)
	{
		childInfo = (HxSocket_t *)HLIB_LIST_Data(pList);
		if(NULL != childInfo && fd != childInfo->fd)
		{
			if(stData.buf)
			{
				stData.buf = SOCKET_MEMDUP(stData.buf, stData.size);
			}
			HxSEMT_Get(childInfo->ulSemId);
			HLIB_MSG_Send(childInfo->fd, &stData, sizeof(HxMsgSocketData_t));
			HxSEMT_Release(childInfo->ulSemId);
		}
		pList = HLIB_LIST_Next(pList);
	}
}

STATIC HINT32 msgsocket_ReceiveMessage(HINT32 fd, HxMsgSocketData_t *pData, HINT32 timeout)
{
	return HLIB_MSG_ReceiveTimeout(fd, pData, sizeof(HxMsgSocketData_t), timeout);
}


STATIC HxMsgSocketContent_t	*msgsocket_Open(HxSocket_Type_e eSocketType, const HCHAR *pszIPAddr, void *targetPort, HINT32 nTry, const HCHAR * file, HUINT32 line)
{
	HxSocket_t	*info = NULL;
	HCHAR		key[128] = {0, };
	HCHAR		szMsgName[128] = {0, };
	HxMsgSocketContent_t  *pContent = msgsocket_content_Get();

	msgsocket_hash_MakeKey(key, eSocketType, pszIPAddr, targetPort);
	if (HxSTD_StrEmpty(key) == TRUE)
	{
		return NULL;
	}

	HxSEMT_Get(s_ulMsgSockSemdId);
	if(pContent->pHash)
	{
		info = HLIB_HASH_Lookup(pContent->pHash, key);
	}
	HxSEMT_Release(s_ulMsgSockSemdId);

	if(NULL != info)
	{
		return SOCKET_MEMDUP(info, sizeof(HxSocket_t));
	}

	info = SOCKET_CALLOC(sizeof(HxSocket_t));
	if(NULL == info)
	{
		return NULL;
	}

#if defined(CONFIG_DEBUG)
	info->debugText = (HCHAR *)file;
	info->debugLine = line;
#endif
	HxSTD_StrNCpy(info->szHashKey, key, 128);

	HxSEMT_Create (&info->ulSemId, (key+5), HxSEMT_FIFO);

	HxSTD_snprintf(szMsgName, 128, "msg_s_%x%x", info, info->ulSemId);
	HLIB_MSG_Create (32, sizeof(HxMsgSocketData_t), szMsgName, &info->fd, eHLIB_SUSPENDTYPE_FIFO);

	info->pSocketList = HLIB_LIST_Append(info->pSocketList, info);
	info->flags = eHxSOCKET_SERVER;
	info->parent_fd = info->fd;

	HxSEMT_Get(s_ulMsgSockSemdId);
	if(pContent->pHash)
	{
		HLIB_HASH_Insert (pContent->pHash, info->szHashKey, info);
	}
	/*
	 * hash에 들어간 socket info는 삭제하지 않는다.
	 */

	HxSEMT_Release(s_ulMsgSockSemdId);

	return SOCKET_MEMDUP(info, sizeof(HxSocket_t));
}


STATIC HINT32	msgsocket_client_Open(HxSocket_Type_e eSocketType, const HCHAR *pszIPAddr, void *targetPort, HINT32 nTry, const HCHAR * file, HUINT32 line)
{
	HxSocket_t	*info = NULL;
	HINT32		nRet = 0;
	HCHAR		szMsgName[128] = {0, };
	HULONG		msgQueue = 0;

	info = msgsocket_Open( eSocketType, pszIPAddr, targetPort, nTry, file, line);
	if(NULL == info)
	{
		return 0;
	}

	HxSEMT_Get(info->ulSemId);
	info->flags = eHxSOCKET_CLIENT;
	HxSTD_snprintf(szMsgName, 128, "msg_c_%x%x", info, info->ulSemId);
	nRet = HLIB_MSG_Create (32, sizeof(HxMsgSocketData_t), szMsgName, &info->fd, eHLIB_SUSPENDTYPE_FIFO);
	if(0 == nRet)
	{
		info->pSocketList = HLIB_LIST_Append(info->pSocketList, info);
	}
	HxSEMT_Release(info->ulSemId);

	if(nRet)
	{
		SOCKET_FREE(info);
		info = 0;
	}

	msgsocket_SendMessage(info->fd, info->pSocketList, eHxSOCKET_CLIENT, HxSOCKET_ADD_CLIENT, NULL, 0, 0);

	return (HINT32)info;
}

STATIC HINT32	msgsocket_server_Open(HxSocket_Type_e eSocketType, void *serverPort, const HCHAR * file, HUINT32 line)
{
	HxSocket_t	*info = NULL;

	info = msgsocket_Open(eSocketType, NULL, serverPort, 0, file, line);
	if(NULL == info)
	{
		return 0;
	}

	info->flags = eHxSOCKET_SERVER;

	return (HINT32)info;
}

STATIC HERROR	msgsocket_Close(HINT32 sock)
{
	HxSocket_t	*info = (HxSocket_t *)sock;

	if(NULL == info)
	{
		return ERR_OK;
	}

	switch(info->flags)
	{
	case eHxSOCKET_SERVER:
		SOCKET_FREE(info);
		break;

	case eHxSOCKET_CLIENT:
		msgsocket_SendMessage(info->fd, info->pSocketList, eHxSOCKET_CLIENT, HxSOCKET_REMOVE_CLIENT, NULL, 0, 0);
		HLIB_MSG_Destroy(info->fd);

		HxSEMT_Get(info->ulSemId);
		HLIB_LIST_Remove(info->pSocketList, info);
		HxSEMT_Release(info->ulSemId);

		SOCKET_FREE(info);
		break;
	}

	return ERR_OK;
}

STATIC HINT32	msgsocket_ListenTimeout(HINT32 sock, HUINT32 timeout, HxSocket_DataListener_t cbPacketReceived, HxSocket_StatusListener_t cbNotifyStatus , void *userData )
{
	HINT32				ret;
	HxMsgSocketData_t 	stData;
	HxSocket_t			*info = (HxSocket_t *)sock;

	HxSTD_memset(&stData, 0, sizeof(HxMsgSocketData_t));

	do
	{
		ret = msgsocket_ReceiveMessage(info->fd, &stData, timeout);
		if(0 == ret)
		{
			if(FALSE == stData.bIsData)
			{
				cbNotifyStatus(userData, stData.eStatus, sock);
			}
			else
			{
				cbPacketReceived(userData, sock, stData.reqId, stData.size, stData.buf);
				if(stData.buf)
				{
					SOCKET_FREE(stData.buf);
				}
			}
		}
	} while (timeout == SOCKET_FOREVER);

	return ret;
}

STATIC HERROR	msgsocket_Write(HINT32 sock, const void *pvData, HUINT32 size, HUINT32 requestId)
{
	HxSocket_t			*info = (HxSocket_t *)sock;

	msgsocket_SendMessage(info->fd, info->pSocketList, info->flags, 0, pvData, size, requestId);

	return ERR_OK;
}

STATIC HUINT32	msgsocket_GetClients(HINT32 sock)
{
	HxSocket_t			*info = (HxSocket_t *)sock;

	return (HLIB_LIST_Length(info->pSocketList) - 1);
}

#endif


#define	__PUBLIC_API________________________________________________________________________________

/**
 * @see comments on hxsocket.h
 */
HINT32	_HLIB_SOCKET_OpenClientEx(HxSocket_Type_e eSocketType, const HCHAR *pszIpAddr, void *targetPort, HINT32 nTry, const HCHAR * file, HUINT32 line)
{
	HxLOG_Trace();

#if	defined(SUPPORT_UDS_SYSTEM)
	return (HINT32)socket_client_Open(eSocketType, pszIpAddr, targetPort, nTry, file, line);
#elif defined(SUPPORT_MSG_SYSTEM)
	return (HINT32)msgsocket_client_Open(eSocketType, pszIpAddr, targetPort, nTry, file, line);;
#else
	return 0;
#endif
}

/**
 * @see comments on hxsocket.h
 */
HINT32	_HLIB_SOCKET_OpenServer(HxSocket_Type_e eSocketType, void *serverPort, const HCHAR * file, HUINT32 line)
{
	HxLOG_Trace();

#if	defined(SUPPORT_UDS_SYSTEM)
	return (HINT32)socket_server_Open(eSocketType, serverPort, file, line);
#elif defined(SUPPORT_MSG_SYSTEM)
	return (HINT32)msgsocket_server_Open(eSocketType, serverPort, file, line);
#else
	return 0;
#endif
}

/**
 * @see comments on hxsocket.h
 */
HERROR		HLIB_SOCKET_Close(HINT32	sock)
{
	HxLOG_Trace();
	HxLOG_Assert(sock);

#if	defined(SUPPORT_UDS_SYSTEM)
	if (SOCKET_IS_CLIENT(sock))
		return socket_client_Close((HxSocket_t *)sock);
	else if (SOCKET_IS_SERVER(sock))
		return socket_server_Close((HxSocket_t *)sock);
	else
		return ERR_FAIL;
#elif defined(SUPPORT_MSG_SYSTEM)
	return msgsocket_Close(sock);
#else
	return ERR_OK;
#endif
}

/**
 * @see comments on hxsocket.h
 */
HINT32	_HLIB_SOCKET_Listen(
					  HINT32 sock
					, HBOOL bForever
					, HxSocket_DataListener_t cbPacketReceived
					, HxSocket_StatusListener_t cbNotifyStatus
					, void *userData, const HCHAR * file, HUINT32 line
				)
{
#if	defined(SUPPORT_UDS_SYSTEM)
	HxSocket_t *info = (HxSocket_t *)sock;

	if (SOCKET_ERROR(sock))
	{
		HxLOG_Error("%s(%s) error flag on!!!\n", __FUNCTION__, socket_ToString(info));
		return -1;
	}

	if (SOCKET_IS_CLIENT(sock))
		return socket_client_Listen(info, bForever ? SOCKET_FOREVER : 0, cbPacketReceived, cbNotifyStatus, userData);
	else if (SOCKET_IS_SERVER(sock))
		return socket_server_Listen(info, bForever ? SOCKET_FOREVER : 0, cbPacketReceived, cbNotifyStatus, userData, file, line);
	else
		return -1;
#elif defined(SUPPORT_MSG_SYSTEM)
	return msgsocket_ListenTimeout(sock, SOCKET_FOREVER, cbPacketReceived, cbNotifyStatus, userData);
#else
	return 0;
#endif
}

HINT32	_HLIB_SOCKET_ListenTimeout(
					  HINT32 sock
					, HUINT32 timeout
					, HxSocket_DataListener_t cbPacketReceived
					, HxSocket_StatusListener_t cbNotifyStatus
					, void *userData, const HCHAR * file, HUINT32 line
				)
{
#if	defined(SUPPORT_UDS_SYSTEM)
	HxSocket_t *info = (HxSocket_t *)sock;

	if (SOCKET_ERROR(sock))
	{
		HxLOG_Error("%s(%s) error flag on!!!\n", __FUNCTION__, socket_ToString(info));
		return -1;
	}

	if (SOCKET_IS_CLIENT(sock))
		return socket_client_Listen(info, timeout, cbPacketReceived, cbNotifyStatus, userData);
	else if (SOCKET_IS_SERVER(sock))
		return socket_server_Listen(info, timeout, cbPacketReceived, cbNotifyStatus, userData, file, line);
	else
		return -1;
#elif defined(SUPPORT_MSG_SYSTEM)
	return msgsocket_ListenTimeout(sock, timeout, cbPacketReceived, cbNotifyStatus, userData);
#else
	return 0;
#endif
}


/**
 * @see comments on hxsocket.h
 */
HERROR	HLIB_SOCKET_Write(HINT32 sock, const void *pvData, HUINT32 size, HUINT32 requestId)
{
#if	defined(SUPPORT_UDS_SYSTEM)
	HxSocket_t *info = (HxSocket_t *)sock;

	if (SOCKET_ERROR(sock))
	{
		HxLOG_Error("%s(%s) error flag on!!!\n", __FUNCTION__, socket_ToString(info));
		return ERR_FAIL;
	}

	if (SOCKET_IS_CLIENT(info) && (info->flags & eHxSOCKET_RECONNECT))
	{
		//client sock의 경우 socket_client_Listen 에서 repair. 그냥 buffering만 해둔다.
//		if (socket_client_RepairConnection(info) < 0)
		{
			return socket_client_Buffering(info, pvData, size, requestId);
		}
	}
	return socket_Write(info, pvData, size, requestId);
#elif defined(SUPPORT_MSG_SYSTEM)
	return msgsocket_Write(sock, pvData, size, requestId);
#else
	return ERR_OK;
#endif
}


HUINT32	HLIB_SOCKET_GetClients (HINT32 sock, HINT32 *clients, HUINT32 n)
{
#if	defined(SUPPORT_UDS_SYSTEM)
	HxSocket_t *info = (HxSocket_t *)sock;
	HxSocket_Server_t *server;

	if (SOCKET_ERROR(sock))
	{
		HxLOG_Error("%s(%s) error flag on!!!\n", __FUNCTION__, socket_ToString(info));
		return 0;
	}

	if (!SOCKET_IS_SERVER(info))
	{
		HxLOG_Error("%s(%s) server only!\n", __FUNCTION__, socket_ToString(info));
		return 0;
	}

	server = (HxSocket_Server_t *)info;
	if (clients)
	{
		HINT32		client;
		HUINT32 	c = 0;

		HxVECTOR_EACH(server->clients, HINT32, client, {
			clients[c++] = client;
			if (c == n)
				break;
		});
		return c;
	}
	return (HUINT32)HLIB_VECTOR_Length(server->clients);
#elif defined(SUPPORT_MSG_SYSTEM)
	return msgsocket_GetClients(sock);
#else
	return 0;
#endif
}

HUINT32	HLIB_SOCKET_GetError (void)
{
	return errno;
}

HERROR	HLIB_SOCKET_SetSendTimeout (HINT32 sock, HUINT32 microSec)
{
#ifdef	OS_LINUX
	HxSocket_t *info = (HxSocket_t *)sock;
	struct timeval tv_timeo;

	if (microSec > 0)
	{
		tv_timeo.tv_sec = microSec/(1000*1000);
		tv_timeo.tv_usec = microSec%(1000*1000);
	}
	else
	{
		tv_timeo.tv_sec = SOCKET_DEFAULT_TIMEOUT/(1000*1000);
		tv_timeo.tv_usec = SOCKET_DEFAULT_TIMEOUT%(1000*1000);
	}

	if (setsockopt( info->fd, SOL_SOCKET, SO_SNDTIMEO, &tv_timeo, sizeof( tv_timeo ) ) == -1 )
	{
		HxLOG_Error("set send timeout failed. \n");
		return ERR_FAIL;
	}

	return ERR_OK;

#else
	return ERR_OK;
#endif
}

HERROR	HLIB_SOCKET_SetReceiveTimeout (HINT32 sock, HUINT32 microSec)
{
#ifdef	OS_LINUX
	HxSocket_t *info = (HxSocket_t *)sock;
	struct timeval tv_timeo;

	if (microSec > 0)
	{
		tv_timeo.tv_sec = microSec/(1000*1000);
		tv_timeo.tv_usec = microSec%(1000*1000);
	}
	else
	{
		tv_timeo.tv_sec = SOCKET_DEFAULT_TIMEOUT/(1000*1000);
		tv_timeo.tv_usec = SOCKET_DEFAULT_TIMEOUT%(1000*1000);
	}

	if (setsockopt( info->fd, SOL_SOCKET, SO_RCVTIMEO, &tv_timeo, sizeof( tv_timeo ) ) == -1 )
	{
		HxLOG_Error("set send timeout failed. \n");
		return ERR_FAIL;
	}

	return ERR_OK;

#else
	return ERR_OK;
#endif
}


