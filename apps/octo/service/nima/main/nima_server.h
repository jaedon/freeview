/** **********************************************************************************************************
	File 		: nima_server.h
	author 		: STB Component tstbcomp@humaxdigital.com
	comment		: 
	date    	: 2013/05/15
	attention 	: 
	
	(c) 2011-2012 Humax Co., Ltd. 
	This program is produced by Humax Co., Ltd. ("Humax") and 
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
	non-assignable, non-transferable and non-exclusive license to use this Software. 
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
	you agree to the responsibility to take all reasonable efforts to protect the any information 
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
	reverse engineer or extract the source code of this Software unless you have Humax's written permission 
	to do so.
	If you have no authorized license, discontinue using this Software immediately. 
	
	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, 
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
*********************************************************************************************************** */


#ifndef __NIMA_SERVER_H__
#define __NIMA_SERVER_H__

#include "htype.h"
#include "nima_session.h"

/*-----------------------------------------------------------------------------------------------
 *	pre-definition
 *----------------------------------------------------------------------------------------------*/
#define MIMA_SERV_MAGIC_NO		0xFEFEFEFE
#define MIMA_SERV_MAX_PATH		(128)

typedef void nimaserver_t;

/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Create the server instance
 *
 * @param[in] magic_code the magic number used for the certificate between client and server.
 *
 * @return the handle of server instance on success, and NULL on error
 */
nimaserver_t *nima_server_create(HUINT32 magic_code);

/**
 * Destroy the server instance. All resources related to server module are freed.
 *
 * @param[in] server the server instance
 *
 * @return NONE
 */
void nima_server_destroy(nimaserver_t *server);

/**
 * Start the server with the unix domain socket path. 
 * The server creates one thread to listen the request from clients.
 *
 * @param[in] server the server instance
 * @param[in] addr the path for unix domain socket. (eg. "/tmp/nima_server")
 *
 * @return ERR_OK on success. if error, return ERR_FAIL.
 */
HERROR nima_server_start(nimaserver_t *server, const HCHAR *addr);

/**
 * Start the server with IP and port number of TCP/IP protocol.
 * The server creates one thread to listen the request from clients.
 *
 * @param[in] server the server instance
 * @param[in] ipaddr the IP address
 * @param[in] port the port number
 *
 * @return ERR_OK on success. if error, return ERR_FAIL.
 */
HERROR nima_server_start_tcp(nimaserver_t *server, HUINT32 ipaddr, HUINT16 port);

/**
 * Stop the server.
 * The create thread will be killed, but all resources are still remained.
 *
 * @param[in] server the server instance
 *
 * @return ERR_OK on success. if error, return ERR_FAIL.
 */
HERROR nima_server_stop(nimaserver_t *server);

void nima_set_enable_auto_connect(HBOOL enable);

/**
 * Create a session.
 * The server will create a session after accepting the request from client.
 *
 * @param[in] server the server instance
 *
 * @return the handle of session instance on success, and NULL on error
 */
nimasession_t *nima_server_create_session(nimaserver_t *server);

/**
 * Destroy a session.
 * The server will destroy a session after terminating a session gracefully.
 *
 * @param[in] server the server instance
 * @param[in] session the created session
 *
 * @return NONE
 */
 
void nima_server_init_ncapi(void);
void nima_server_destroy_session(nimaserver_t *server, nimasession_t *session);


/**
 * Find the session using the socket descorptor. the server will return the session instance if
 * it has the matched socket descriptor in own session list.
 *
 * @param[in] server the server instance
 * @param[in] sock the socket descriptor of client
 *
 * @return the handle of session instance if it is in the session list of server.
 */
nimasession_t *nima_server_find_session(nimaserver_t *server, HINT32 sock);

#ifdef __cplusplus
}

class NimaServer {
public:
	NimaServer() { create(MIMA_SERV_MAGIC_NO); }
	NimaServer(HUINT32 magic_code) { create(magic_code); }
	virtual ~NimaServer() { destroy(); }

	bool start(const HCHAR *addr)
	{ return nima_server_start(m_server, addr) == ERR_OK; }
	bool start(HUINT32 ipaddr, HUINT16 port)
	{ return nima_server_start_tcp(m_server, ipaddr, port) == ERR_OK; }
	bool stop()
	{ return nima_server_stop(m_server) == ERR_OK; }

	NimaSession *createSession()
	{
		nimasession_t *session = nima_server_create_session(m_server);
		if ( !session ) return NULL;
		return new NimaSession(session);
	}
	void destroySession(NimaSession *session);
	{
		nima_server_destroy_session(m_server, session->getHandle());
		delete session;
	}
	/* uncomportable...
	 * This object shoude be deleted by user. it needs to improve something. */
	NimaSession *findSession(HINT32 sock)
	{
		nimasession_t *session = nima_server_find_session(m_server, sock);
		if ( !session ) return NULL;
		return new NimaSession(session);
	}

private:
	bool create(HUINT32 magic_code)
	{ return (m_server = nima_server_create(magic_code)) ? true : false; }
	void destroy()
	{ nima_server_destroy(m_server); }


private:
	nimaserver_t	*m_server;
};

#endif

#endif
