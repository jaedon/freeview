/** **********************************************************************************************************
	File 		: nima_conv.h
	author 		: 
	comment		: 
	date    	: 2013/12/19
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

#ifndef __NIMA_CONV_H__
#define __NIMA_CONV_H__

#include "htype.h"
#include "ncapi.h"
#include "nima_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

eNimaEvent nima_conv_event_netconf(eNcapiNetConfEvent event);
eNimaEvent nima_conv_event_wifi(eNcapiWifiEvent event);
eNimaEvent nima_conv_event_pppoe(eNcapiPPPoEEvent event);
eNimaEvent nima_conv_event_bt(eNcapiBTEvent event);
eNimaBTStatus nima_conv_bt_status(eNcapiBTStatus status);

HERROR nima_conv_ncapi_apinfo(NIMAProtocol_APInfo_t *src, NCAPI_WifiAPInfo_t *dst);
HERROR nima_conv_ncapi_pppoe(NIMAProtocol_PPPOE_info_t *src, NCAPI_PPPoEInfo_t *dst);

HERROR nima_conv_netconf(NCAPI_NetConfInfo_t *src, NIMAProtocol_Netconf_t *dst);
HERROR nima_conv_apinfo(NCAPI_WifiAPInfo_t *src, NIMAProtocol_APInfo_t *dst);
HERROR nima_conv_pppoe(NCAPI_PPPoEInfo_t *src, NIMAProtocol_PPPOE_info_t *dst);
HERROR nima_conv_btinfo(NCAPI_BTInfo_t *src, NIMAProtocol_bt_info_t *dst);
HERROR nima_conv_bt_devinfo(NCAPI_BTDevInfo_t *src, NIMAProtocol_bt_dev_t *dst);



#ifdef __cplusplus
}
#endif

#endif	/* __NIMA_CACHE_H__ */

