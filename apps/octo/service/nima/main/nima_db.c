/** **********************************************************************************************************
	File 		: napi_cache.c
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
#include <hlib.h>
#include <hapi.h>
#include <arpa/inet.h>
#include <resolv.h>

#include "nima_db.h"
#include "nima_protocol.h"
#include "nima_util.h"

#include "ncapi_pppoe.h"

/*-----------------------------------------------------------------------------------------------
 *	pre-definitions
 *----------------------------------------------------------------------------------------------*/
#define SAVE_INT_TO_DB(pszKey, nValue) \
	do{ \
		if(ERR_OK != HAPI_SetRegistryInt(pszKey, nValue)){ \
			HxLOG_Debug("ERROR Write DB (%s)\n", pszKey); \
			return ERR_FAIL; \
		}else{ \
			HxLOG_Debug("SAVE %s : %d \n", pszKey, nValue);	\
		} \
	}while(0)


#define SAVE_STR_TO_DB(pszKey, szValue)	\
	do{ \
		if(ERR_OK != HAPI_SetRegistryStr(pszKey, szValue)){ \
			HxLOG_Debug("ERROR Write DB (%s)\n", pszKey); \
			return ERR_FAIL; \
		}else{ \
			HxLOG_Debug("SAVE %s : %s \n", pszKey, szValue); \
		} \
	}while(0)

#define SAVE_ENUM_TO_DB(pszKey, pszType, nEnum)	\
	do{ \
		if(ERR_OK != HAPI_SetRegistryEnum(pszKey, pszType, nEnum)){	\
			HxLOG_Debug("ERROR Write DB (%s)\n", pszKey); \
			return ERR_FAIL; \
		}else{ \
			HxLOG_Debug("SAVE %s : %d \n", pszKey, nEnum); \
		} \
	}while(0)

#define LOAD_INT_FROM_DB(pszKey, pnValue) \
	do{ \
		if(ERR_OK != HAPI_GetRegistryInt(pszKey, pnValue)){ \
			HxLOG_Debug("ERROR DB Read (%s) \n", pszKey); \
			return ERR_FAIL; \
		}else{ \
			HxLOG_Debug("LOAD %s : %d \n", pszKey, *pnValue); \
		} \
	}while(0)


#define LOAD_STR_FROM_DB(pszKey, pnValue, nMax)	\
	do{ \
		if(ERR_OK != HAPI_GetRegistryStr(pszKey, pnValue, nMax)){ \
			HxLOG_Debug("ERROR DB Read (%s) \n", pszKey); \
			return ERR_FAIL; \
		}else{ \
			HxLOG_Debug("LOAD %s : %s \n", pszKey, pnValue); \
		} \
	}while(0)

#define LOAD_ENUM_FROM_DB(pszKey, pnType, pnEnum) \
	do{ \
		if(ERR_OK != HAPI_GetRegistryEnum(pszKey, pnType, pnEnum)){ \
			HxLOG_Debug("ERROR DB Read (%s) \n", pszKey); \
			return ERR_FAIL; \
		}else{ \
			HxLOG_Debug("LOAD %s : %d \n", pszKey, *pnEnum); \
		} \
	}while(0)

#define ZeroMemory(x) \
		HxSTD_MemSet(x, 0x00, sizeof(x));


/*-----------------------------------------------------------------------------------------------
 *	internal
 *----------------------------------------------------------------------------------------------*/

 static HERROR __nima_db_load_ipv4info(HINT32 dev_id, NIMAProtocol_IpInfo_t *ip_info)
{
	HERROR hRet = ERR_OK;
	FuncEnter;
 	{	//1 #### for ipv4
	 	HCHAR szkey[32] = {0,};
 		HCHAR sztemp[256] = {0,};
		struct in_addr temp;

		ZeroMemory(sztemp);
		ZeroMemory(szkey);
		HxSTD_PrintToStrN(szkey, sizeof(szkey), "net_%d_ip", dev_id);
		LOAD_STR_FROM_DB(szkey, sztemp, sizeof(sztemp));
		inet_aton( sztemp, &temp);
		HxSTD_MemCopy(ip_info->ipv4.ipAddr, &temp, sizeof(struct in_addr));

		ZeroMemory(sztemp);
		ZeroMemory(szkey);
		HxSTD_PrintToStrN(szkey, sizeof(szkey), "net_%d_netmask", dev_id);
		LOAD_STR_FROM_DB(szkey, sztemp, sizeof(sztemp));
		inet_aton(sztemp, &temp);
		HxSTD_MemCopy(ip_info->ipv4.netmask, &temp, sizeof(struct in_addr));


		ZeroMemory(sztemp);
		ZeroMemory(szkey);
		HxSTD_PrintToStrN(szkey, sizeof(szkey), "net_%d_gateway", dev_id);
		LOAD_STR_FROM_DB(szkey, sztemp, sizeof(sztemp));
		inet_aton(sztemp, &temp);
		HxSTD_MemCopy(ip_info->ipv4.gateway, &temp, sizeof(struct in_addr));

		ZeroMemory(szkey);
		HxSTD_PrintToStrN(szkey, sizeof(szkey), "net_%d_dns%d", dev_id, eNIMA_Dns_Primary);
		ZeroMemory(sztemp);
		LOAD_STR_FROM_DB(szkey, sztemp, sizeof(sztemp));
		inet_aton(sztemp, &temp);
		HxSTD_MemCopy(ip_info->ipv4.dns1, &temp, sizeof(struct in_addr));


		ZeroMemory(szkey);
		HxSTD_PrintToStrN(szkey, sizeof(szkey), "net_%d_dns%d", dev_id, eNIMA_Dns_Secondary);
		ZeroMemory(sztemp);
		LOAD_STR_FROM_DB(szkey, sztemp, sizeof(sztemp));
		inet_aton(sztemp, &temp);
		HxSTD_MemCopy(ip_info->ipv4.dns2, &temp, sizeof(struct in_addr));
	}

	HxLOG_Debug("#################### LOAD DB IP ########################\n");
	HxLOG_Debug("net_%d_ip : %d.%d.%d.%d \n", dev_id, ip_info->ipv4.ipAddr[0], ip_info->ipv4.ipAddr[1],
												ip_info->ipv4.ipAddr[2], ip_info->ipv4.ipAddr[3]);
	HxLOG_Debug("net_%d_netmask : %d.%d.%d.%d \n", dev_id, ip_info->ipv4.netmask[0], ip_info->ipv4.netmask[1],
														ip_info->ipv4.netmask[2], ip_info->ipv4.netmask[3]);
	HxLOG_Debug("net_%d_gateway : %d.%d.%d.%d \n", dev_id, ip_info->ipv4.gateway[0], ip_info->ipv4.gateway[1],
														ip_info->ipv4.gateway[2], ip_info->ipv4.gateway[3]);
	HxLOG_Debug("net_%d_dns0 :%d.%d.%d.%d \n", dev_id, ip_info->ipv4.dns1[0], ip_info->ipv4.dns1[1],
													ip_info->ipv4.dns1[2], ip_info->ipv4.dns1[3]);
	HxLOG_Debug("net_%d_dns1 :%d.%d.%d.%d \n", dev_id, ip_info->ipv4.dns2[0], ip_info->ipv4.dns2[1],
													ip_info->ipv4.dns2[2], ip_info->ipv4.dns2[3]);
	HxLOG_Debug("#########################################################\n\n");

	FuncLeave;
	return hRet;
}
static HERROR __nima_db_load_ipv6info(HINT32 dev_id, NIMAProtocol_IpInfo_t *ip_info)
{
	HERROR hRet = ERR_OK;

	return hRet;
}

static HERROR __nima_db_save_ipv6info(HINT32 dev_id, NIMAProtocol_Netconf_t *ipinfo)
{
	HERROR hRet = ERR_OK;

	return hRet;
}
static HERROR __nima_db_save_ipv4info(HINT32 dev_id, NIMAProtocol_Netconf_t *ipinfo)
{
	HERROR hRet = ERR_OK;
	HCHAR szkey[32]={0,};
	HCHAR sztemp[32] = {0,};

	ZeroMemory(sztemp);
	HxSTD_PrintToStrN(sztemp, sizeof(sztemp), "%d.%d.%d.%d", ipinfo->ip_info.ipv4.ipAddr[0], ipinfo->ip_info.ipv4.ipAddr[1],
										ipinfo->ip_info.ipv4.ipAddr[2], ipinfo->ip_info.ipv4.ipAddr[3]);
	ZeroMemory(szkey);
	HxSTD_PrintToStrN(szkey, sizeof(szkey), "net_%d_ip", dev_id);
	SAVE_STR_TO_DB(szkey, sztemp);

	ZeroMemory(sztemp);
	HxSTD_PrintToStrN(sztemp, sizeof(sztemp), "%d.%d.%d.%d", ipinfo->ip_info.ipv4.netmask[0], ipinfo->ip_info.ipv4.netmask[1],
										ipinfo->ip_info.ipv4.netmask[2], ipinfo->ip_info.ipv4.netmask[3]);
	ZeroMemory(szkey);
	HxSTD_PrintToStrN(szkey, sizeof(szkey), "net_%d_netmask", dev_id);
	SAVE_STR_TO_DB(szkey, sztemp);

	ZeroMemory(sztemp);
	HxSTD_PrintToStrN(sztemp, sizeof(sztemp), "%d.%d.%d.%d", ipinfo->ip_info.ipv4.gateway[0], ipinfo->ip_info.ipv4.gateway[1],
										ipinfo->ip_info.ipv4.gateway[2], ipinfo->ip_info.ipv4.gateway[3]);
	ZeroMemory(szkey);
	HxSTD_PrintToStrN(szkey, sizeof(szkey), "net_%d_gateway", dev_id);
	SAVE_STR_TO_DB(szkey, sztemp);

	ZeroMemory(sztemp);
	HxSTD_PrintToStrN(sztemp, sizeof(sztemp), "%d.%d.%d.%d", ipinfo->ip_info.ipv4.dns1[0], ipinfo->ip_info.ipv4.dns1[1],
										ipinfo->ip_info.ipv4.dns1[2], ipinfo->ip_info.ipv4.dns1[3]);
	ZeroMemory(szkey);
	HxSTD_PrintToStrN(szkey, sizeof(szkey), "net_%d_dns%d", dev_id, eNIMA_Dns_Primary);
	SAVE_STR_TO_DB(szkey, sztemp);

	ZeroMemory(sztemp);
	HxSTD_PrintToStrN(sztemp, sizeof(sztemp), "%d.%d.%d.%d", ipinfo->ip_info.ipv4.dns2[0], ipinfo->ip_info.ipv4.dns2[1],
										ipinfo->ip_info.ipv4.dns2[2], ipinfo->ip_info.ipv4.dns2[3]);
	ZeroMemory(szkey);
	HxSTD_PrintToStrN(szkey, sizeof(szkey), "net_%d_dns%d", dev_id, eNIMA_Dns_Secondary);
	SAVE_STR_TO_DB(szkey, sztemp);

	HxLOG_Debug("################## SAVE DB IP ###################### \n");

	HxLOG_Debug("net_%d_ip      : %d.%d.%d.%d \n", dev_id,
					ipinfo->ip_info.ipv4.ipAddr[0], ipinfo->ip_info.ipv4.ipAddr[1],
					ipinfo->ip_info.ipv4.ipAddr[2], ipinfo->ip_info.ipv4.ipAddr[3]);
	HxLOG_Debug("net_%d_netmask : %d.%d.%d.%d \n", dev_id,
					ipinfo->ip_info.ipv4.netmask[0], ipinfo->ip_info.ipv4.netmask[1],
					ipinfo->ip_info.ipv4.netmask[2], ipinfo->ip_info.ipv4.netmask[3]);
	HxLOG_Debug("net_%d_gateway : %d.%d.%d.%d \n", dev_id,
					ipinfo->ip_info.ipv4.gateway[0], ipinfo->ip_info.ipv4.gateway[1],
					ipinfo->ip_info.ipv4.gateway[2], ipinfo->ip_info.ipv4.gateway[3]);
	HxLOG_Debug("net_%d_dns0    : %d.%d.%d.%d \n", dev_id,
					ipinfo->ip_info.ipv4.dns1[0], ipinfo->ip_info.ipv4.dns1[1],
					ipinfo->ip_info.ipv4.dns1[2], ipinfo->ip_info.ipv4.dns1[3]);
	HxLOG_Debug("net_%d_dns1    : %d.%d.%d.%d \n", dev_id,
					ipinfo->ip_info.ipv4.dns2[0], ipinfo->ip_info.ipv4.dns2[1],
					ipinfo->ip_info.ipv4.dns2[2], ipinfo->ip_info.ipv4.dns2[3]);
	HxLOG_Debug("#########################################################\n\n");

	return hRet;
}

static HERROR __nima_db_save_netconf_connect_info(HINT32 dev_id, NIMAProtocol_Netconf_t *info)
{
	HCHAR szkey[32] = {0,};

	FuncEnter;

	ZeroMemory(szkey);
	HxSTD_snprintf(szkey, 32, "net_%d_dhcp", dev_id);
	SAVE_INT_TO_DB(szkey, info->dhcp);

	ZeroMemory(szkey);
	HxSTD_snprintf(szkey, 32, "net_%d_dns_auto", dev_id);
	SAVE_INT_TO_DB(szkey, info->dns_auto);

	ZeroMemory(szkey);
	HxSTD_snprintf(szkey, 32, "net_%d_auto_ip", dev_id);
	SAVE_INT_TO_DB(szkey, info->auto_ip);

	ZeroMemory(szkey);
	HxSTD_snprintf(szkey, 32, "net_%d_ip_ver", dev_id);
	SAVE_INT_TO_DB(szkey, info->ip_ver);

	HxLOG_Debug("################## SAVE DB NETCONF ###################### \n");
	HxLOG_Debug("net_%d_dhcp : %d \n", dev_id, info->dhcp);
	HxLOG_Debug("net_%d_dns_auto : %d \n", dev_id, info->dns_auto);
	HxLOG_Debug("net_%d_auto_ip : %d \n", dev_id, info->auto_ip);
	HxLOG_Debug("net_%d_ip_ver : %d \n", dev_id, info->ip_ver);
	HxLOG_Debug("######################################################### \n\n");

	FuncLeave;

	return ERR_OK;
}

static HERROR __nima_db_save_wifi_connected_info(HINT32 dev_id, NIMAProtocol_APInfo_t *info)
{
	HCHAR szkey[32] = {0,};
	HCHAR sztemp[32] = {0,};

	if( !info )
	{
		HxLOG_Debug("ap info has NULL \n");
		return ERR_FAIL;
	}

	ZeroMemory(szkey);
	HxSTD_snprintf(szkey, 20, "ap_%d_essid", dev_id);
	SAVE_STR_TO_DB(szkey, info->essid);

	ZeroMemory(szkey);
	HxSTD_snprintf(szkey, 20, "ap_%d_apkey", dev_id);
	SAVE_STR_TO_DB(szkey, info->apkey);

	ZeroMemory(szkey);
	ZeroMemory(sztemp);
	HxSTD_snprintf(szkey, 20, "ap_%d_macaddr", dev_id);
	HxSTD_PrintToStrN(sztemp, sizeof(sztemp), "%02x-%02x-%02x-%02x-%02x-%02x",
												info->macaddr[0], info->macaddr[1],
												info->macaddr[2], info->macaddr[3],
												info->macaddr[4], info->macaddr[5]);
	SAVE_STR_TO_DB(szkey, sztemp);

	ZeroMemory(szkey);
	HxSTD_snprintf(szkey, 20, "ap_%d_wave_strength", dev_id);
	SAVE_INT_TO_DB(szkey, info->wave_strength);

	ZeroMemory(szkey);
	HxSTD_snprintf(szkey, 20, "ap_%d_connect_speed", dev_id);
	SAVE_INT_TO_DB(szkey, info->connect_speed);

	ZeroMemory(szkey);
	HxSTD_snprintf(szkey, 20, "ap_%d_op_mode", dev_id);
	SAVE_INT_TO_DB(szkey, info->op_mode);

	ZeroMemory(szkey);
	HxSTD_snprintf(szkey, 20, "ap_%d_secure_type", dev_id);
	SAVE_INT_TO_DB(szkey, info->secure_type);

	ZeroMemory(szkey);
	HxSTD_snprintf(szkey, 20, "ap_%d_authen_type", dev_id);
	SAVE_INT_TO_DB(szkey, info->authen_type);

	ZeroMemory(szkey);
	HxSTD_snprintf(szkey, 20, "ap_%d_encrypt_type", dev_id);
	SAVE_INT_TO_DB(szkey, info->encrypt_type);

	return ERR_OK;
}

static HERROR __nima_db_save_bt_info(NIMAProtocol_bt_info_t *info)
{
	HCHAR szkey[32] = {0,};

	ZeroMemory(szkey);
	HxSTD_snprintf(szkey, sizeof(szkey), "bt_enable");
	SAVE_INT_TO_DB(szkey, info->enable);

	return ERR_OK;
}

static HERROR __nima_db_save_pppoe_onoff(HINT32 dev_id, HBOOL bOn)
{
	HCHAR szkey[32] = {0,};

	ZeroMemory(szkey);
	HxSTD_snprintf(szkey, sizeof(szkey), "pppoe_bOn");
	SAVE_INT_TO_DB(szkey, bOn);

	HxLOG_Debug("################## SAVE DB PPPOE ################ \n");
	HxLOG_Debug("[%d]pppoe_bOn : %d \n", dev_id, bOn);
	HxLOG_Debug("################################################# \n\n");

	return ERR_OK;
}

static HERROR __nima_db_save_pppoe_connected_info(HINT32 dev_id, NIMAProtocol_PPPOE_info_t *info)
{
	HCHAR szkey[32] = {0,};

	if( !info )
		return ERR_FAIL;

	ZeroMemory(szkey);
	HxSTD_snprintf(szkey, sizeof(szkey), "pppoe_dev_id");
	SAVE_INT_TO_DB(szkey, info->dev_id);

	ZeroMemory(szkey);
	HxSTD_snprintf(szkey, sizeof(szkey), "pppoe_szUserID");
	SAVE_STR_TO_DB(szkey, info->szUserID);

	ZeroMemory(szkey);
	HxSTD_snprintf(szkey, sizeof(szkey), "pppoe_szUserPW");
	SAVE_STR_TO_DB(szkey, info->szUserPW);

	HxLOG_Debug("################## SAVE DB PPPOE ################ \n");
	HxLOG_Debug("[%d]pppoe_dev_id : %d \n", dev_id, info->dev_id);
	HxLOG_Debug("[%d]pppoe_szUserID : %s \n", dev_id, info->szUserID);
	HxLOG_Debug("[%d]pppoe_szUserPW : %s \n", dev_id, info->szUserPW);
	HxLOG_Debug("################################################# \n\n");

	return ERR_OK;
}


static HERROR __nima_db_load_netconf_connected_info(HINT32 dev_id, NIMAProtocol_Netconf_t *info)
{
	HCHAR szkey[32] = {0,};
	FuncEnter;

	if( !info )
	{
		return ERR_FAIL;
	}
	else // Set initial values (This is necessary in case there is no NIMA info in DB, for example, after reseting factory
	{
		info->dhcp = TRUE;
		info->dns_auto = TRUE;
		info->auto_ip = TRUE;
		info->ip_ver = eNIMA_NETCONF_IPV4;
	}

	ZeroMemory(szkey);
	HxSTD_PrintToStrN(szkey, sizeof(szkey), "net_%d_dhcp", dev_id);
	LOAD_INT_FROM_DB(szkey, &info->dhcp);

	ZeroMemory(szkey);
	HxSTD_PrintToStrN(szkey, sizeof(szkey), "net_%d_dns_auto", dev_id);
	LOAD_INT_FROM_DB(szkey, &info->dns_auto);

	ZeroMemory(szkey);
	HxSTD_PrintToStrN(szkey, sizeof(szkey), "net_%d_auto_ip", dev_id);
	LOAD_INT_FROM_DB(szkey, &info->auto_ip);

	ZeroMemory(szkey);
	HxSTD_PrintToStrN(szkey, sizeof(szkey), "net_%d_ip_ver", dev_id);
	LOAD_INT_FROM_DB(szkey, &info->ip_ver);

	HxLOG_Debug("################## LOAD DB ###################### \n");
	HxLOG_Debug("net_%d_dhcp : %d \n", dev_id, info->dhcp);
	HxLOG_Debug("net_%d_dns_auto : %d \n", dev_id, info->dns_auto);
	HxLOG_Debug("net_%d_auto_ip : %d \n", dev_id, info->auto_ip);
	HxLOG_Debug("net_%d_ip_ver : %d \n", dev_id, info->ip_ver);
	HxLOG_Debug("################################################# \n\n");

	FuncLeave;
	return ERR_OK;
}

static HERROR __nima_db_load_wifi_connected_info(HINT32 dev_id, NIMAProtocol_APInfo_t *info)
{
	HCHAR szkey[32] = {0,};
	HCHAR szname[256] = {0,};
	HCHAR sztemp[256] = {0,};

	FuncEnter;
	if( !info )
		return ERR_FAIL;

	ZeroMemory(szkey);
	ZeroMemory(szname);
	HxSTD_PrintToStrN(szkey, sizeof(szkey), "ap_%d_essid", dev_id);
	LOAD_STR_FROM_DB(szkey, szname, sizeof(szname));
	HxSTD_MemCopy(info->essid, szname, NIMA_PROTO_AP_ESSID);

	ZeroMemory(szkey);
	ZeroMemory(szname);
	HxSTD_PrintToStrN(szkey, sizeof(szkey), "ap_%d_apkey", dev_id);
	LOAD_STR_FROM_DB(szkey, szname, sizeof(szname));
	HxSTD_MemCopy(info->apkey, szname, NIMA_PROTO_AP_KEY);

	ZeroMemory(szkey);
	ZeroMemory(sztemp);
	HxSTD_PrintToStrN(szkey, sizeof(szkey), "ap_%d_macaddr", dev_id);
	LOAD_STR_FROM_DB(szkey, sztemp, sizeof(sztemp));
	nima_util_ConverMacAddressStringIntoByte(sztemp, info->macaddr);

	ZeroMemory(szkey);
	HxSTD_PrintToStrN(szkey, sizeof(szkey), "ap_%d_wave_strength", dev_id);
	LOAD_INT_FROM_DB(szkey, &info->wave_strength);

	ZeroMemory(szkey);
	HxSTD_PrintToStrN(szkey, sizeof(szkey), "ap_%d_connect_speed", dev_id);
	LOAD_INT_FROM_DB(szkey, &info->connect_speed);

	ZeroMemory(szkey);
	HxSTD_PrintToStrN(szkey, sizeof(szkey), "ap_%d_op_mode", dev_id);
	LOAD_INT_FROM_DB(szkey, &info->op_mode);

	ZeroMemory(szkey);
	HxSTD_PrintToStrN(szkey, sizeof(szkey), "ap_%d_secure_type", dev_id);
	LOAD_INT_FROM_DB(szkey, &info->secure_type);

	ZeroMemory(szkey);
	HxSTD_PrintToStrN(szkey, sizeof(szkey), "ap_%d_authen_type", dev_id);
	LOAD_INT_FROM_DB(szkey, &info->authen_type);

	ZeroMemory(szkey);
	HxSTD_PrintToStrN(szkey, sizeof(szkey), "ap_%d_encrypt_type", dev_id);
	LOAD_INT_FROM_DB(szkey, &info->encrypt_type);

	FuncLeave;
	return ERR_OK;
}

static HERROR  __nima_db_load_bt_info(NIMAProtocol_bt_info_t *bt_info)
{
	if( !bt_info ) return ERR_FAIL;

    LOAD_INT_FROM_DB("bt_enable", (HINT32*)&bt_info->enable);

    return ERR_OK;
}

static HERROR __nima_db_load_pppoe_onoff(HINT32 dev_id, HBOOL *bOn)
{
	HCHAR szkey[32] = {0,};
	HINT32 on = 0;

	if( !bOn ) return ERR_FAIL;

	ZeroMemory(szkey);
	HxSTD_PrintToStrN(szkey, sizeof(szkey), "pppoe_bOn");
	LOAD_INT_FROM_DB(szkey, &on);

	*bOn = (HBOOL) on;

	HxLOG_Debug("################## LOAD DB PPPOE ###################### \n");
	HxLOG_Debug("[%d]pppoe_bOn : %d \n", dev_id, on);
	HxLOG_Debug("################################################# \n\n");

	return ERR_OK;
}

static HERROR __nima_db_load_pppoe_connected_info(HINT32 dev_id, NIMAProtocol_PPPOE_info_t *info)
{
	HCHAR szkey[32] = {0,};
	HCHAR szname[256] = {0,};

	if( !info )
		return ERR_FAIL;

	ZeroMemory(szkey);
	HxSTD_PrintToStrN(szkey, sizeof(szkey), "pppoe_dev_id");
	LOAD_INT_FROM_DB(szkey, &info->dev_id);

	ZeroMemory(szkey);
	ZeroMemory(szname);
	HxSTD_PrintToStrN(szkey, sizeof(szkey), "pppoe_szUserID");
	LOAD_STR_FROM_DB(szkey, szname, sizeof(szname));
	HxSTD_MemCopy(info->szUserID, szname, NIMA_PROTO_PPPOE_ID_LEN_MAX - 1);

	ZeroMemory(szkey);
	ZeroMemory(szname);
	HxSTD_PrintToStrN(szkey, sizeof(szkey), "pppoe_szUserPW");
	LOAD_STR_FROM_DB(szkey, szname, sizeof(szname));
	HxSTD_MemCopy(info->szUserPW, szname, NIMA_PROTO_PPPOE_PW_LEN_MAX - 1);

	HxLOG_Debug("################## LOAD DB PPPOE ###################### \n");
	HxLOG_Debug("[%d]pppoe_dev_id : %d \n", dev_id, info->dev_id);
	HxLOG_Debug("[%d]pppoe_szUserID : %s \n", dev_id, info->szUserID);
	HxLOG_Debug("[%d]pppoe_szUserPW : %s \n", dev_id, info->szUserPW);
	HxLOG_Debug("################################################# \n\n");

	return ERR_OK;
}


/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/
HERROR nima_db_init(void)
{
	return ERR_OK;
}

HERROR nima_db_save(void *data, eNimaDBType db_type)
{
	HERROR hRet = ERR_OK;

	FuncEnter;
	HxLOG_Debug("db_type to save : %d \n", db_type);

	switch(db_type)
	{
		case NIMA_DB_NETCONF_CONNECTED_INFO:
			{
				NIMAProtocol_Netconf_t *netconf_info = (NIMAProtocol_Netconf_t *)data;
				if( !netconf_info )
					return ERR_FAIL;

				__nima_db_save_netconf_connect_info(netconf_info->id, netconf_info);

				switch(netconf_info->ip_ver)
				{
					case eNIMA_NETCONF_IPV4:
						__nima_db_save_ipv4info(netconf_info->id, netconf_info);
						break;
					case eNIMA_NETCONF_IPV6:
						__nima_db_save_ipv6info(netconf_info->id, netconf_info);
						break;
				}
			}
			break;
		case NIMA_DB_WIFI_CONNECTED_INFO:
			{
				NIMAProtocol_Wifi_t *wifi_info = (NIMAProtocol_Wifi_t *)data;
				if( !wifi_info )
					return ERR_FAIL;
				return __nima_db_save_wifi_connected_info(wifi_info->dev_id, &wifi_info->ap[0]);
			}
			break;
		case NIMA_DB_PPPOE_SET_ONOFF:
			{
				NIMAProtocol_PPPOE_info_t *pppoe_info = (NIMAProtocol_PPPOE_info_t *)data;
				if( !pppoe_info )
					return ERR_FAIL;

				return __nima_db_save_pppoe_onoff(pppoe_info->dev_id, pppoe_info->bOn);
			}
			break;
		case NIMA_DB_PPPOE_CONNECTED_INFO:
			{
				NIMAProtocol_PPPOE_info_t *pppoe_info = (NIMAProtocol_PPPOE_info_t *)data;
				if( !pppoe_info )
					return ERR_FAIL;

				return __nima_db_save_pppoe_connected_info(pppoe_info->dev_id, pppoe_info);
			}
			break;
		case NIMA_DB_BT_INFO:
			{
				NIMAProtocol_bt_info_t *bt_info = (NIMAProtocol_bt_info_t *)data;
				return __nima_db_save_bt_info(bt_info);
			}
			break;
		default:
			break;
	}
	FuncLeave;
	return hRet;
}

HERROR nima_db_load(NIMAProtocol_devinfo_t *devinfo)
{
	HERROR hRet = ERR_OK;
	HINT32 i = 0;
	FuncEnter;

	if( NULL == devinfo ) return ERR_FAIL;

	for(i=0; i<devinfo->dev_count; i++)
	{
		NIMAProtocol_Netconf_t *netconf_info = &devinfo->netconf[i];
		hRet |= __nima_db_load_netconf_connected_info(netconf_info->id, netconf_info);

		switch(netconf_info->ip_ver)
		{
			case eNIMA_NETCONF_IPV4:
				hRet |= __nima_db_load_ipv4info(netconf_info->id,  &netconf_info->ip_info);
				break;
			case eNIMA_NETCONF_IPV6:
				hRet |= __nima_db_load_ipv6info(netconf_info->id,  &netconf_info->ip_info);
				break;
		}

		switch(netconf_info->type)
		{
			case eNIMA_NETCONF_LAN:
				break;
			case eNIMA_NETCONF_WIFI:
				hRet |= __nima_db_load_wifi_connected_info(netconf_info->id, &devinfo->apinfo);
				break;
			case eNIMA_NETCONF_PPPOE:
				hRet |= __nima_db_load_pppoe_onoff(netconf_info->id, &devinfo->pppoeinfo.bOn);
				hRet |= __nima_db_load_pppoe_connected_info(netconf_info->id, &devinfo->pppoeinfo);
				break;
			case eNIMA_NETCONF_CM:
			case eNIMA_NETCONF_RF4CE:
			case eNIMA_NETCONF_WAN:
			case eNIMA_NETCONF_MAX:
			default:
				break;

		}
	}

	hRet |= __nima_db_load_bt_info(&devinfo->btinfo);


#if 0 //defined(CONFIG_MW_INET_PPPOE) // sgchoi
	// Due to devinfo->netconf[i].type is all 0, must be call __nima_db_load_pppoe_connected_info once
	static HBOOL s_bPPPoELoadedDB = FALSE;

	if(s_bPPPoELoadedDB == FALSE)
	{
		NCAPI_PPPoEInfo_t *pppoe_info = NULL;
		HUINT32 data_size = 0;

		data_size = sizeof(NCAPI_PPPoEInfo_t);
		pppoe_info = HLIB_STD_MemAlloc(data_size);
		if ( !pppoe_info ) return ERR_FAIL;
		HxSTD_MemSet(pppoe_info, 0x0, data_size);

		__nima_db_load_pppoe_connected_info(eNIMA_NETCONF_PPPOE, &devinfo->pppoeinfo);

		pppoe_info->id = devinfo->pppoeinfo.dev_id;
		pppoe_info->enable = devinfo->pppoeinfo.bOn;
		HxSTD_MemCopy(pppoe_info->connect_id, devinfo->pppoeinfo.szUserID, NIMA_PROTO_PPPOE_ID_LEN_MAX - 1);
		HxSTD_MemCopy(pppoe_info->connect_pw, devinfo->pppoeinfo.szUserPW, NIMA_PROTO_PPPOE_PW_LEN_MAX - 1);

		NCAPI_PPPoE_SetInfo(pppoe_info->id, pppoe_info);
		s_bPPPoELoadedDB = TRUE;

		if(pppoe_info)
		{
			HLIB_STD_MemFree(pppoe_info);
			pppoe_info = NULL;
		}
	}
#endif
	FuncLeave;
	return hRet;
}

