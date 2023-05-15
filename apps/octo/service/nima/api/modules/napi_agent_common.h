/** **********************************************************************************************************
	File 		: napi_agent.h
	author 		:
	comment		:
	date    	: 2013/05/27
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

#ifndef __NAPI_AGENT_COMMON_H__
#define __NAPI_AGENT_COMMON_H__

#include "htype.h"
#include "napi_protocol.h"
#include <hlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NAPI_SAFE_DELETE(x) \
			do{ \
				if(x){	\
					HLIB_STD_MemFree(x); \
					x = NULL; \
				} \
			}while(0)

#define NAPI_AGENT_TIMEOUT 10000
#define NAPI_SYNC_CALL_TIMEOUT 5000
#define NAPI_SERV_MAX_PATH 128

typedef void napiagent_t;
typedef void (* napi_agent_callback) (HINT32 event, void *data);


typedef enum {
	eAGENT_EVENT_TYPE_NETCONF			= 0,
	eAGENT_EVENT_TYPE_WIFI,
	eAGENT_EVENT_TYPE_BT,
	eAGENT_EVENT_TYPE_PPPOE,
	eAGENT_EVENT_TYPE_MAX
} eAgentDeviceType;

typedef enum {
	NAPI_SYSTEM_STATUS_Undefined,
	NAPI_SYSTEM_STATUS_Operation,
	NAPI_SYSTEM_STATUS_Reboot,
	NAPI_SYSTEM_STATUS_Shutdown,
	NAPI_SYSTEM_STATUS_RebootDone,
	NAPI_SYSTEM_STATUS_ShutdownDone,
	NAPI_SYSTEM_STATUS_Unknow
} eNApiSystemStatus;


typedef struct _napiagent_t {
	HINT32 magic_code;
	HINT32 socket;
	HINT32 handle;
	HUINT32 sema;
	HULONG tid;
	HBOOL bInit;
	HBOOL bDBLoad;
	HCHAR uds_path[NAPI_SERV_MAX_PATH];
	napi_agent_callback callbacks[eAGENT_EVENT_TYPE_MAX];
	eNApiSystemStatus system_status;
} napiagent_info_t;

typedef struct {
	HBOOL bDone;
	HINT32 lCode;
	HCHAR * pvResult;
} napi_sync_call_info_t;

extern napiagent_t *s_agent_obj;
extern  napiagent_info_t *__agent_info(napiagent_t *agent);

#if 0 /* Dont use __attribute__((constructor)), __attribute__((destructor)) by napi_deinit() crash : USE_NAPI_CONSTRUCTOR */
extern void __agent_constructor(void) CONSTRUCTOR;
extern void __agent_destructor(void) DESTRUCTOR;
#else
void napi_agent_create_instance(void);
void napi_agent_destroy_instance(void);
#endif

/**
 * Start a agent client.
 *
 * @param agent pointer agent inforamtion.
 * @param addr uds path for connectting server.
 * @return ERR_OK on success, ERR_FAIL on error.
 */
napiagent_t *napi_agent_start(const HCHAR *addr);

/**
 * Stop a agent client.
 *
 * @param void.
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR napi_agent_stop(napiagent_t *agent);

/**
 * Send data to server.
 *
 * @param data data for sending to server
 * @param len data length.
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR napi_agent_write_data(napiagent_t *agent, const HCHAR *data, HINT32 len);

napiagent_t *napi_agent_get_instance(void);

HERROR napi_agent_notify_callback(napiagent_t *agent, eAgentDeviceType type,
										HINT32 event, void* data);

HERROR napi_agent_register_callback(napiagent_t *agent,
		napi_agent_callback callback, eAgentDeviceType type);

// fast boot
HERROR napi_agent_init_wait(void);

/**
* This function is used to to get result of a sync call
* Maximum time to wait for the reply from NIMA is 5 seconds
* pvResult memory area MUST be freed after calling this function
* @param[in] packet: packet which is sent from NAPI to NIMA
* @param[out] pvResult: the pointer which point to the memory area which allocated for storing result
* @return ERR_OK if napi can receive result from NIMA, ERR_FAIL if vice versa
*/

HERROR napi_agent_get_sync_call_result(napipacket_t *packet, void ** pvResult);


/**
* This function is used to to get result of a sync call
* pvResult memory area MUST be freed after calling this function
* @param[in] packet: packet which is sent from NAPI to NIMA
* @param[in] lTimeout: time to wait for the reply from NIMA
* @param[in] packet: packet which is sent from NAPI to NIMA
* @param[out] pvResult: the pointer which point to the memory area which allocated for storing result
* @return ERR_OK if napi can receive result from NIMA, ERR_FAIL if vice versa
*/

HERROR napi_agent_get_sync_call_result_timeout(napipacket_t *packet, HINT32 lTimeout, void ** pvResult);


/* TODO Rename */
void napi_client(void *arg);
extern void napi_agent_parser(napiagent_info_t *agent, napipacket_t *packet);

#ifdef __cplusplus
}
#endif

#endif	/* __NAPI_AGENT_COMMON_H__ */

