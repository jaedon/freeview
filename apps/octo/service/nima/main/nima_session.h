/** **********************************************************************************************************
	File 		: nima_session.h
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


#ifndef __NIMA_SESSION_H__
#define __NIMA_SESSION_H__

#include <pthread.h>

#include "htype.h"
#include "nima_protocol.h"
/*-----------------------------------------------------------------------------------------------
 *	pre-definition
 *----------------------------------------------------------------------------------------------*/
enum enumNimaSession {
	NIMA_SESSION_USELOCK		= 0x01 
};

typedef struct _nimasession_t {
	HINT32		sock;
	HBOOL		is_init;
	HBOOL		is_active;
	HULONG		msgq;
	pthread_t	thread_id;
} nimasession_t;

/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif


HINT32 nima_session_find_connected_apindex(HINT32 device_id, HINT32 ap_count, NIMAProtocol_APInfo_t *aplist);
NIMAProtocol_devinfo_t* nima_session_update_db(nimapacket_t *packet, NIMAProtocol_devinfo_t *devinfo);
HBOOL nima_session_do_scenario(nimapacket_t *packet, NIMAProtocol_devinfo_t *devinfo);

HERROR nima_session_shutdown(void);

/**
 * Start the session operator.
 * One session has one thread and one message queue. It waits for the message from client.
 *
 * @param[in] session the session instance
 * @param[in] sock the socket descriptor connected between server and client.
 * @param[in] flags the flags for session
 * 	NIMA_SESSION_USELOCK: all functions in session are operated with lock mechanism.
 *
 * @return ERR_OK on success. if error, return ERR_FAIL.
 */
HERROR nima_session_start(nimasession_t *session, HINT32 sock, HINT32 flags);

/*
 * Stop the session operator.
 * The internal thread of session will be killed, but all resources are still remained.
 * If you want to destroy the session, please refer to nima_server_destroy_session.
 *
 * @param[in] session the session instance
 *
 * @return ERR_OK on success. if error, return ERR_FAIL.
 */
HERROR nima_session_stop(nimasession_t *session);

/**
 * Push the data into the message queue of session.
 *
 * @param[in] session the session instance
 * @param[in] data the data from client
 * @param[in] len the data length
 *
 * @return ERR_OK on success. if error, return ERR_FAIL.
 */
HERROR nima_session_push(nimasession_t *session, const HCHAR *data, HINT32 len);

/*
 * Check if the session is avaliable or not.
 *
 * @param[in] session the session instance
 *
 * @return TRUE if the session is availble.
 */
HBOOL nima_session_is_active(nimasession_t *session);

/*
 * Return the socket descriptor of session.
 *
 * @param[in] session the session instance
 *
 * @return the socket descritptor. It will return -1 if the session is not available.
 */
HINT32 nima_session_get_sock(nimasession_t *session);

HERROR nima_session_boot_action(void);

#ifdef __cplusplus
}

class NimaSession {
public:
	NimaSession(nimasession_t *session) { m_session = session; }
	virtual ~NimaSession() {}
	bool start(HINT32 sock, HINT32 flags)
	{ return nima_session_start(m_session, sock, flags) == ERR_OK; }
	bool stop()
	{ return nima_session_stop(m_session) == ERR_OK; }

	bool push(const HCHAR *data, HINT32 len)
	{ return nima_session_push(m_session, data, len) == ERR_OK; }
	bool isActive()
	{ return nima_session_is_active(m_session) == TRUE; }

	HINT32 getSocket()
	{ return nima_session_get_sock(m_session); }
	nimasession_t *getHandle()
	{ return m_session; }

private:
	nimasession_t	*m_session;
};

#endif

#endif
