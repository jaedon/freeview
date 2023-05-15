/** **********************************************************************************************************
	File 		: napi_wifi.h
	author 		:
	comment		: WIFI
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

#ifndef __NAPI_WIFI_H__
#define __NAPI_WIFI_H__

#include "htype.h"
#include "napi_def.h"


/*-----------------------------------------------------------------------------------------------
 *	pre-definition
 *----------------------------------------------------------------------------------------------*/

typedef void (*NAPI_WifiCallback) (HINT32 event, void *data);

/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif


/**
 * Register callback function for wifi status.
 *
 * @param	void
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_Wifi_RegisterCallback(NAPI_WifiCallback callback);

/**
 * Function of Wifi Scan Start for wifi index.
 *
 * @param void
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_Wifi_ScanStart(HINT32 id);

/**
 * Function of Wifi Scan Stop for wifi index.
 *
 * @param void
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_Wifi_ScanStop(HINT32 id);

/**
 * Try wifi connection for index using ap information.
 *
 * @param info AP infomation
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_Wifi_Connect(HINT32 id, const NAPIWifiAPInfo_t *info);

/**
 * Try wifi WPS connection for index using ap information.
 *
 * @param info WPS Type, Pin Code
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_Wifi_ConnectWPS(HINT32 id, eNapiWifiWPS eWPSType, HCHAR *szPinCode);

/**
 * Try wifi disconnection for index
 *
 * @param void
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_Wifi_Disconnect(HINT32 id);

/**
 * Get Pincode By WPS
 *
 * @param void
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_Wifi_GetPincodeByWPS(HINT32 id, HCHAR *szPincode);

/**
 * Get a ap infomation for index. 
 *
 * @param info ap infomation
 * @return ERR_OK on success, ERR_FAIL on error.
 */
HERROR NAPI_Wifi_GetConnectedAPInfo(HINT32 id, NAPIWifiAPInfo_t *info);


/**
* This synchonous function is used to configure WoWLAN feature
* @param[in] id: index of wifi network interface
* @param[in] mode: chosen mode
* @return ERR_OK if this WoWLAN feature is configured succesfully, ERR_FAIL if vice versa
*/

HERROR NAPI_Wifi_SetWakeupOnWLAN(HINT32 id, eNapiWifiWoWlan mode);


#ifdef __cplusplus
}
#endif

#endif	/* __NAPI_WIFI_H__ */
