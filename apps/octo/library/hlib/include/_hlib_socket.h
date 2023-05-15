/*************************************************************
 * @file		hxsocket.h
 * @date		2012/03/13
 * @author		someone
 * @brief		HLIB Socket API

 * http://www.humaxdigital.com
 *************************************************************/

#ifndef	_HXSOCKET_H_
#define	_HXSOCKET_H_

#include "htype.h"

typedef	enum
{
	HxSOCKET_UDS,	///< UNIX Domain socket
	HxSOCKET_INET	///< Socket
} HxSocket_Type_e;

typedef	enum
{
	HxSOCKET_ADD_CLIENT,
	HxSOCKET_REMOVE_CLIENT,
	HxSOCKET_DISCONNECT_CLIENT
} HxSocket_Status_e;

/**
 * Socket data listener
 *
 * @param userdata	5th argument passed to HLIB_SOCKET_Listen
 * @param sock		socket handle (if server, client handle, else self)
 * @param requestId	4th argument passed to HLIB_SOCKET_Write
 * @param size		sizeof 'pvData' (3rd argument passed to HLIB_SOCKET_Write)
 * @param pvData	data (2nd argument passed to HLIB_SOCKET_Write)
 */
typedef void (* HxSocket_DataListener_t)(void *userdata, HINT32 sock, HUINT32 requestId, HINT32 size, void *pvData);

/**
 * Socket status listener
 *
 * @param userdata	5th argument passed to HLIB_SOCKET_Listen
 * @param status	see HxSocket_Status_e
 * @param sock		socket handle (if server, client handle, else self)
 */
typedef void (* HxSocket_StatusListener_t)(void *userdata, HxSocket_Status_e status, HINT32 sock);

/**
 * Open socket as client
 *
 * @param eSocketType	see HxSocket_Type_e
 * @param targetPort	if UDS, string("file path"), elif INET int(port)
 * @param nTry			if 0, auto-repair connection, else try to connect to server N time(s)
 * @return	instance of socket
 */
HINT32	_HLIB_SOCKET_OpenClientEx(HxSocket_Type_e eSocketType, const HCHAR *pszIpAddr, void *targetPort, HINT32 nTry, const HCHAR * file, HUINT32 line);

#define	HLIB_SOCKET_OpenClient(eSocketType, targetPort, nTry)\
	_HLIB_SOCKET_OpenClientEx(eSocketType, NULL, targetPort, nTry, __FILE__, __LINE__)

#define HLIB_SOCKET_OpenClientEx(eSocketType, pszIpAddr, targetPort, nTry) \
	_HLIB_SOCKET_OpenClientEx(eSocketType, pszIpAddr, targetPort, nTry, __FILE__, __LINE__)

/**
 * Open socket as server
 *
 * @param eSocketType	see HxSocket_Type_e
 * @param serverPort	if UDS, string("file path"), elif INET int(port)
 * @return	instance of socket
 */
HINT32	_HLIB_SOCKET_OpenServer(HxSocket_Type_e eSocketType, void *serverPort, const HCHAR * file, HUINT32 line);
#define HLIB_SOCKET_OpenServer(eSocketType, serverPort)\
	_HLIB_SOCKET_OpenServer(eSocketType, serverPort, __FILE__, __LINE__)

/**
 * Close socket
 *
 * @param sock	socket handle
 * @return	ERR_OK / ERR_FAIL
 */
HERROR	HLIB_SOCKET_Close(HINT32 sock);

/**
 * Listen socket from remote
 *
 * @param sock	socket handle
 * @param bForever	listen forever!!
 * @param cbPacketReceived	data listener
 * @param cbNotifyStatus	status listener
 * @param userdata
 * @return	< 0: error, else number of packet
 */
HINT32	_HLIB_SOCKET_Listen(
					  HINT32 sock
					, HBOOL bForever
					, HxSocket_DataListener_t cbPacketReceived
					, HxSocket_StatusListener_t cbNotifyStatus
					, void *userdata, const HCHAR * file, HUINT32 line
				);

#define HLIB_SOCKET_Listen( sock, bForever, cbPacketReceived, cbNotifyStatus, userdata)\
	_HLIB_SOCKET_Listen(sock, bForever, cbPacketReceived, cbNotifyStatus, userdata, __FILE__, __LINE__)


/**
 * Listen socket from remote with timeout
 *
 * @param sock	socket handle
 * @param timeout	waiting time
 * @param cbPacketReceived	data listener
 * @param cbNotifyStatus	status listener
 * @param userdata
 * @return	< 0: error, else number of packet
 */
HINT32	_HLIB_SOCKET_ListenTimeout(
					  HINT32 sock
					, HUINT32 timeout
					, HxSocket_DataListener_t cbPacketReceived
					, HxSocket_StatusListener_t cbNotifyStatus
					, void *userData, const HCHAR * file, HUINT32 line
				);

#define HLIB_SOCKET_ListenTimeout(sock, timeout, cbPacketReceived, cbNotifyStatus, userData)\
	_HLIB_SOCKET_ListenTimeout(sock, timeout, cbPacketReceived, cbNotifyStatus, userData, __FILE__, __LINE__)

/**
 * Write data to socket
 *
 * @param sock	socket handle
 * @param data	data to be written
 * @param size	size of 'data'
 * @param requestId	for managing purpose,
 * @return	ERR_OK / ERR_FAIL
 */
HERROR	HLIB_SOCKET_Write(HINT32 sock, const void *data, HUINT32 size, HUINT32 requestId);

/**
 * Get client socket handle(s)
 *
 * @param sock	socket handle(server)
 * @param clients	[OUT] client handles
 * @param n			[IN] size of array 'clients'
 * @return	number of clients
 */
HUINT32	HLIB_SOCKET_GetClients (HINT32 sock, HINT32 *clients, HUINT32 n);

/**
 * Get socket error number
 *
 * @return	socket error number
 */
HUINT32	HLIB_SOCKET_GetError (void);

/**
 * Set send buffer size
 *
 * @param sock	      [IN] socket handle
 * @param byte  [IN] send buffer size (byte)
 * @return	ERR_OK / ERR_FAIL

 */
HERROR	HLIB_SOCKET_SetSendBufferSize (HINT32 sock, HUINT32 ulSize);


/**
 * Set receive buffer size
 *
 * @param sock	      [IN] socket handle
 * @param byte  [IN] receive buffer size (byte)
 * @return	ERR_OK / ERR_FAIL

 */
HERROR	HLIB_SOCKET_SetReceiveBufferSize (HINT32 sock, HUINT32 ulSize);

/**
 * Set send timeout
 *
 * @param sock	      [IN] socket handle
 * @param microSec  [IN] send timeout micro sec
 * @return	ERR_OK / ERR_FAIL

 */
HERROR	HLIB_SOCKET_SetSendTimeout (HINT32 sock, HUINT32 microSec);

/**
 * Set receive timeout
 *
 * @param sock	      [IN] socket handle
 * @param microSec  [IN] receive timeout micro sec
 * @return	ERR_OK / ERR_FAIL

 */
HERROR	HLIB_SOCKET_SetReceiveTimeout (HINT32 sock, HUINT32 microSec);


#endif /* _HXSOCKET_H_ */

