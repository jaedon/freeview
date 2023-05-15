/** **********************************************************************************************************
	File 		: napi_cm.h
	author 		: 
	comment		: Cable Model
	date    	: 2013/05/23
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


#ifndef __NAPI_CM_H__
#define __NAPI_CM_H__

#include "ncapi.h"

/*-----------------------------------------------------------------------------------------------
 *	pre-definition
 *----------------------------------------------------------------------------------------------*/
typedef enum
{
	eNCAPI_CM_EVENT_ONLINE 	= 0,	
	eNCAPI_CM_EVENT_OFFLINE	=1
} eNcapi_CM_LinkEvent;

typedef enum {
	eNCAPI_CM_NONE 					= 0,
	eNCAPI_CM_OTHER,
	eNCAPI_CM_NOT_READY,
	eNCAPI_CM_NOT_SYNC,
	eNCAPI_CM_PHY_SYNC_DONE,
	eNCAPI_CM_PARAM_ACQUIRED,
	eNCAPI_CM_RANGING_COMPLETE,
	eNCAPI_CM_IP_COMPLETE,
	eNCAPI_CM_TOD_CERTIFY,
	eNCAPI_CM_SECURITY_CERTIFY,
	eNCAPI_CM_PARAM_TRAANSE_COMPLETE,
	eNCAPI_CM_REGIST_COMPLETE,
	eNCAPI_CM_OPERATION,
	eNCAPI_CM_ACCESS_DENIED,
} eNcapiCmStatus;

typedef enum {
	eNCAPI_CM_QAM_64 = 	0,
	eNCAPI_CM_QAM_256,
	eNCAPI_CM_QAM_OTHER,
	eNCAPI_CM_QAM_UNKNOWN,
} eNcapiCmQam;

typedef struct {
	eNcapiCmQam	qam_type;
	HINT32 		freq;
	HINT32 		ber;
	HINT32 		snr;
	HINT32 		signal_level;
} NCAPI_CMDownStream_t;

typedef struct {
	HINT32	 	freq;
	HINT32	 	channel_id;
	HINT32		bandwidth;
	HINT32		signal_level;
} NCAPI_CMUpStream_t;

typedef struct {
	eNcapiCmStatus state;
	NCAPI_CMDownStream_t down_stream;
	NCAPI_CMUpStream_t up_stream;
} NCAPI_CM_Info_t;

typedef void (*NCAPI_CmCallback) (eNcapi_CM_LinkEvent event, HINT32 count,
										NCAPI_CM_Info_t *data);

/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

HERROR NCAPI_CM_Init(void);
HERROR NCAPI_CM_Deinit(void);
HERROR NCAPI_CM_RegisterCallback(NCAPI_CmCallback callback);
HERROR NCAPI_CM_UnregisterCallback(NCAPI_CmCallback callback);


/**
 * Gets a Downstream information of the cable modem.
 *
 * @param info Downstream information of cable modem.
 */

HERROR NCAPI_CM_GetDownStreamInfo(NCAPI_CMDownStream_t *info);

/**
 * Sets a Downstream information of the cable modem.
 *
 * @param info Downstream information of cable modem.
 */
HERROR NCAPI_CM_SetDownStreamInfo(const NCAPI_CMDownStream_t *info);

/**
 * Gets a Upstream information of the cable modem.
 *
 * @param info Upstream information of cable modem.
 */
HERROR NCAPI_CM_GetUpStreamInfo(NCAPI_CMUpStream_t *info);

/**
 * Get the status of cable modem.
 *
 * @return status status of the cable modem.
 */
eNcapiCmStatus NCAPI_CM_GetStatus(void);

#ifdef __cplusplus
}
#endif

#endif	/* __NAPI_CM_H__ */
