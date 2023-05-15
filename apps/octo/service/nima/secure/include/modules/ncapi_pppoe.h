/** **********************************************************************************************************
	File 		: ncapi_pppoe.h
	author 		: IP/OTT
	comment		: PPPoE
	date    	: 2013/11/12
	attention 	: 
	
	(c) 2011-2013 Humax Co., Ltd. 
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


#ifndef __NCAPI_PPPOE_H__
#define __NCAPI_PPPOE_H__

#include "htype.h"

/*-----------------------------------------------------------------------------------------------
 *	pre-definition
 *----------------------------------------------------------------------------------------------*/
#define NCAPI_PPPOE_ID_LEN		(64)
#define NCAPI_PPPOE_PW_LEN		(128)


typedef enum {
	eNCAPI_PPPOE_EVENT_CONNECT_SUCCESS,
	eNCAPI_PPPOE_EVENT_CONNECT_FAIL,
	eNCAPI_PPPOE_EVENT_DISCONNECT_SUCCESS,
	eNCAPI_PPPOE_EVENT_DISCONNECT_FAIL,
	eNCAPI_PPPOE_EVENT_SET_INFO,
	eNCAPI_PPPOE_EVENT_SET_ONOFF,
	eNCAPI_PPPOE_EVENT_UNKNOWN,
} eNcapiPPPoEEvent;

typedef struct
{
	HINT32 id;
	HBOOL  enable;
	HUINT8 connect_id[NCAPI_PPPOE_ID_LEN];
	HUINT8 connect_pw[NCAPI_PPPOE_PW_LEN];
} __attribute__((packed)) NCAPI_PPPoEInfo_t;


typedef void (*NCAPI_PPPoECallback) (eNcapiPPPoEEvent event, NCAPI_PPPoEInfo_t *data);

/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Sample Subject
 *
 * @param[in] server the server instance
 * @param[in] ipaddr the IP address
 * @param[in] port the port number
 *
 * @return ERR_OK on success. if error, return ERR_FAIL.
 */

/**
 * initialize wifi module for ncapi
 *
 * @param[in] void 
 *
 * @return ERR_OK on success. if error, return ERR_FAIL.
 */
extern HERROR NCAPI_PPPoE_Init(void);
extern HERROR NCAPI_PPPoE_Deinit(void);
extern HERROR NCAPI_PPPoE_RegisterCallback(NCAPI_PPPoECallback callback);
extern HERROR NCAPI_PPPoE_UnregisterCallback(NCAPI_PPPoECallback callback);
extern HERROR NCAPI_PPPoE_Connect(HINT32 id, const NCAPI_PPPoEInfo_t *info);
extern HERROR NCAPI_PPPoE_Disconnect(HINT32 id);
extern HERROR NCAPI_PPPoE_GetInfo(HINT32 id, NCAPI_PPPoEInfo_t *info);
extern HERROR NCAPI_PPPoE_SetInfo(HINT32 id, const NCAPI_PPPoEInfo_t *info);
extern HERROR NCAPI_PPPoE_GetOnOff(HINT32 id, NCAPI_PPPoEInfo_t *info);
extern HERROR NCAPI_PPPoE_SetOnOff(HINT32 id, HBOOL enable);
 
#ifdef __cplusplus
 }
#endif
 
#endif	/* __NCAPI_PPPOE_H__ */

