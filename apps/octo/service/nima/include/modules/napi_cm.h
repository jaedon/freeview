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

#include "htype.h"
#include "napi_def.h"

/*-----------------------------------------------------------------------------------------------
 *	pre-definition
 *----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Gets a Downstream information of the cable modem.
 *
 * @param info Downstream information of cable modem.
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_CM_GetDownStreamInfo(NAPICMDownStream_t *info);

/**
 * Sets a Downstream information of the cable modem.
 *
 * @param info Downstream information of cable modem.
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_CM_SetDownStreamInfo(const NAPICMDownStream_t *info);

/**
 * Gets a Upstream information of the cable modem.
 *
 * @param info Upstream information of cable modem.
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_CM_GetUpStreamInfo(NAPICMUpStream_t *info);

/**
 * Get the status of cable modem.
 *
 * @param void.
 * @return status of cable modem.
 */
eNapiCmStatus NAPI_CM_GetStatus(void);

#ifdef __cplusplus
}
#endif

#endif	/* __NAPI_CM_H__ */
