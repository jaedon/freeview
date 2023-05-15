/*************************************************************************************************************
	File 		: nico_debug.h
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2014/01/24
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Humax) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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
*************************************************************************************************************/

#include "hlib.h"
#include "di_err.h"
#include "di_cm.h"
#include "di_wifi.h"
#include "di_network.h"
#include "di_bt.h"
#include "di_pppoe.h"
#include "di_network.h"
#include "nico_msg.h"

//#define	NICO_DEBUG //for debugging in release mode (Not NFS mode)
#ifdef	NICO_DEBUG
	#ifdef HxLOG_Error
		#undef	HxLOG_Error
		#define HxLOG_Error printf
	#endif

	#ifdef HxLOG_Warning
		#undef	HxLOG_Warning
		#define HxLOG_Warning printf
	#endif

	#ifdef HxLOG_Debug
		#undef	HxLOG_Debug
		#define HxLOG_Debug printf
	#endif
#endif

void nico_debug_print_netinfo(NICO_NetConf_Info_t *netinfo);
void nico_debug_print_ipv6info(NICO_NetConf_IpInfo_t *ipinfo);
void nico_debug_print_ipv4info(NICO_NetConf_IpInfo_t *ipinfo);
void nico_debug_print_apinfo(NICO_Wifi_APInfo_t *apinfo);
void nico_debug_print_bt_info(NICO_BT_DevInfo_t *info, HINT32 count);
void nico_debug_print_pppoe_info(NICO_PPPoE_Info_t *info);
void nico_debug_print_dinetinfo(HINT32 id, DI_NETWORK_GET_INFO_t *di_netinfo);
void nico_debug_print_eventtype(pevent_msg event);
void nico_debug_print_netinfo(NICO_NetConf_Info_t *netinfo);


