/** **********************************************************************************************************
	File 		: napi_pppoe.h
	author 		: 
	comment		: Network Configuration
	date    	: 2013/11/11
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


#ifndef __NAPI_PPPOE_H__
#define __NAPI_PPPOE_H__

#include "htype.h"
#include "napi_def.h"
#if defined(WIN32)
# include <windows.h>
# include <winsock.h>
#else/* Not Windows */
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#endif

/*-----------------------------------------------------------------------------------------------
 *	pre-definition
 *----------------------------------------------------------------------------------------------*/

typedef void (*NAPI_PPPOECallback) (HINT32 event, void* data);

/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------------------------------
 * Network Configuration
 *----------------------------------------------------------------------------------------------*/

/**
 * Register callback function for network status. 
 *
 * @param	NAPI_PPPOECallback
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_PPPOE_RegisterCallback(NAPI_PPPOECallback callback);

/**
 *  PPPOE Connect
 *
 * @param	NAPIPPPOEInfo_t
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_PPPOE_Connect(const NAPIPPPOEInfo_t *info);

 
/**
 *  PPPOE Disconnect
 *
 * @param	void
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_PPPOE_Disconnect(HINT32 id);


/**
 * Set Info for PPPOE
 *
 * @param	NAPIPPPOEInfo_t
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_PPPOE_GetInfo(NAPIPPPOEInfo_t *info);

/**
 * Get Info for PPPOE
 *
 * @param	NAPIPPPOEInfo_t
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_PPPOE_SetInfo(const NAPIPPPOEInfo_t *info);

/**
 * Get OnOff for PPPOE
 *
 * @param  HINT32, HBOOL *
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_PPPOE_GetOnOff(HINT32 id, HBOOL *bOn);

/**
 * Set OnOff for PPPOE
 *
 * @param  HINT32, HBOOL
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_PPPOE_SetOnOff(HINT32 id, HBOOL bOn);



#ifdef __cplusplus
}
#endif

#endif	/* __NAPI_NETCONF_H__ */


