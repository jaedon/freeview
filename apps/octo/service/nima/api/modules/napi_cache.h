/** **********************************************************************************************************
	File 		: napi_cache.h
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

#ifndef __NAPI_CACHE_H__
#define __NAPI_CACHE_H__

#include "htype.h"
#include "napi_def.h"

typedef void napicache_t;


#ifdef __cplusplus
extern "C" {
#endif

napicache_t *napi_cache_get_instance(void);

NAPIDevInfo_t *napi_cache_devinfo_get(napicache_t *cache);
NAPINetConfInfo_t *napi_cache_netconf_get(napicache_t *cache, HINT32 index);
NAPIWifiInfo_t *napi_cache_wifi_get(napicache_t *cache, HINT32 index);
//NAPICMInfo_t *napi_cache_cm_get(napicache_t *cache, HINT32 index);
NAPIDHCPServerInfo_t *napi_cache_dhcpserver_get(napicache_t *cache, HINT32 index);
NAPIBTInfo_t *napi_cache_bt_get(napicache_t *cache);
NAPIPPPOEInfo_t *napi_cache_pppoe_get(napicache_t *cache);



HINT32 napi_cache_netconf_get_count(napicache_t *cache);
HINT32 napi_cache_ap_get_count(napicache_t *cache);
//HINT32 napi_cache_cm_get_count(napicache_t *cache);
HINT32 napi_cache_dhcpserver_get_count(napicache_t *cache);
HINT32 napi_cache_bt_get_count(napicache_t *cache);


HERROR napi_cache_devinfo_update(napicache_t *cache, const NAPIDevInfo_t *info);
HERROR napi_cache_netconf_update(napicache_t *cache, const NAPINetConfInfo_t *info);
HERROR napi_cache_wifi_update(napicache_t *cache, const NAPIWifiInfo_t *info);
//HERROR napi_cache_cm_update(napicache_t *cache, const NAPICMInfo_t *info);
HERROR napi_cache_dhcpserver_update(napicache_t *cache, const NAPIDHCPServerInfo_t *info);
HERROR napi_cache_bt_update(napicache_t *cache, const NAPIBTInfo_t *info);
HERROR napi_cache_pppoe_update_onoff(napicache_t *cache, const NAPIPPPOEInfo_t *info);
HERROR napi_cache_pppoe_update_info(napicache_t *cache, const NAPIPPPOEInfo_t *info);


#ifdef __cplusplus
}
#endif

#endif	/* __NAPI_CACHE_H__ */
