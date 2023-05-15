/*************************************************************************************************************
	File 		: nico_operator_common.h
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2013/06/05
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

#ifndef __NICO_OPERATOR_COMMON_H_
#define __NICO_OPERATOR_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "nico.h"
#include "di_err.h"
#include "di_cm.h"
#include "di_wifi.h"
#include "di_network.h"
#include "di_bt.h"
#include "di_pppoe.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define		MAKE_IP_ADDRESS(a, b, c, d)		\
	(((HUINT32)(a))|((HUINT32)(b<<8))|((HUINT32)(c<<16))|((HUINT32)(d<<24)))


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct {
	HULONG	ulIPaddr;
	HULONG	ulNetMask;
} IPaddr_t;

/*******************************************************************/
/********************      Public                 ******************/
/*******************************************************************/
void nico_op_common_update_devinfo(HINT32 id, NICO_NetDevice_Info_t *deviceinfo);

eNicoWifiSecurity nico_op_common_ap_securetype(DI_WIFI_SECURE_e sec_type);
eNicoWifiAuth nico_op_common_ap_authentype(DI_WIFI_AUTHEN_e auth_type);
eNicoWifiEncrypt nico_op_common_ap_encrypttype(DI_WIFI_ENCRYPT_e enc_type);
DI_WIFI_WPS_e	nico_op_common_get_wpstype(eNicoWifiWps wps_type);
#if defined(CONFIG_MW_INET_WLAN)
HBOOL	nico_op_common_wifi_scan(HUINT32 id);
#endif

HERROR nico_op_common_interface_updown(HINT32 id, HBOOL is_up);
HERROR nico_op_common_restart_dhcp_connecting(HINT32 id, eNicoNetConfIpVersion ip_ver);
HERROR nico_op_common_start_dhcp_client(HINT32 id);
HERROR nico_op_common_stop_dhcp_connecting(HINT32 id, eNicoNetConfIpVersion ip_ver);
HERROR nico_op_common_stop_menual_connecting(HINT32 id, eNicoNetConfIpVersion ip_ver);
HERROR nico_op_common_set_config(HINT32 id, NICO_NetDevice_Info_t *deviceinfo, NICO_NetConf_Info_t* info);
HERROR nico_op_common_set_routetable(pevent_msg event,  HINT32 dev_count, NICO_NetDevice_Info_t **deviceinfo, nico_dievtmsg_t *di_event);
HERROR nico_op_common_set_autoip(HINT32 dev_id, nico_dievtmsg_t *di_event, NICO_NetDevice_Info_t *deviceinfo);
HERROR nico_op_common_get_bluetooth_host(HINT32 dev_id, nico_dievtmsg_t *di_event, event_msg *event);


HBOOL nico_op_common_wifi_is_connected( HUINT32 id);
void nico_op_common_get_connected_apinfo(HINT32 id, NICO_Wifi_APInfo_t *nico_apinfo);
HERROR nico_op_common_get_bluetooth_devicelist(NICO_BT_Info_t *bt_info);

#ifdef __cplusplug
}
#endif

#endif

