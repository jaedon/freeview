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
#include "napi_cache.h"
#include "../napi_debug.h"

#include <hlib.h>

/*-----------------------------------------------------------------------------------------------
 *	pre-definitions
 *----------------------------------------------------------------------------------------------*/
typedef struct _napicache_table_t {
	HxList_t			*dev_info;
	HxList_t			*netconfs;
	HxList_t			*wifi_aps;
	HxList_t			*cm_info;
	HxList_t			*dhcp_server;
	HxList_t			*bt_info;
	HxList_t			*pppoe_info;
} napicache_table_t;

/*-----------------------------------------------------------------------------------------------
 *	internal
 *----------------------------------------------------------------------------------------------*/
static napicache_t *s_cache_defobj = NULL;

static inline napicache_table_t *__table(napicache_t *cache)
{ return (napicache_table_t *)cache; }

static void __cache_constructor(void) CONSTRUCTOR;
static void __cache_destructor(void) DESTRUCTOR;

static void __cache_free_list(void *list)
{
	HINT32 i = 0;
	HINT32 len = HLIB_LIST_Length(list);
	for ( i=0; i<len; ++i )
	{
		void *item = HLIB_LIST_GetListItem(list, i);
		void *data = HLIB_LIST_Data(item);
		HLIB_STD_MemFree(data);
	}
	HLIB_LIST_RemoveAll(list);
	list = NULL;
}

void __cache_constructor(void)
{
	napicache_table_t *table = NULL;

	s_cache_defobj = HLIB_STD_MemAlloc(sizeof(napicache_table_t));

	if(s_cache_defobj != NULL)
	{
		HxSTD_MemSet(s_cache_defobj, 0, sizeof(napicache_table_t));
		table = __table(s_cache_defobj);
		table->cm_info = NULL;
		table->dev_info = NULL;
		table->netconfs = NULL;
		table->wifi_aps = NULL;
		table->cm_info = NULL;
		table->dhcp_server = NULL;
		table->pppoe_info = NULL;
	}
}

void __cache_destructor(void)
{
	napicache_table_t *table = NULL;

	table = __table(s_cache_defobj);
	if ( !table ) return;
	if ( table->netconfs )
		__cache_free_list(table->netconfs);
	if ( table->wifi_aps )
		__cache_free_list(table->wifi_aps);
	if ( table->dev_info)
		__cache_free_list(table->dev_info);
	if ( table->cm_info )
		__cache_free_list(table->cm_info);
	if ( table->dhcp_server)
		__cache_free_list(table->dhcp_server);
	if ( table->pppoe_info)
		__cache_free_list(table->pppoe_info);

	if ( s_cache_defobj )
		HLIB_STD_MemFree(s_cache_defobj);
}

/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/
napicache_t *napi_cache_get_instance(void)
{
	return s_cache_defobj;
}

NAPIDevInfo_t *napi_cache_devinfo_get(napicache_t *cache)
{
	HUINT32 list_len = 0;
	NAPIDevInfo_t *dev_info = NULL;
	HxList_t *list = NULL;
	napicache_table_t *table = NULL;

	FuncEnter;

	table = __table(cache);
	if ( table == NULL ) return NULL;

	list = HLIB_LIST_First(table->dev_info);
	list_len = HLIB_LIST_Length(list);
	if(list_len)
	{
		list = HLIB_LIST_GetListItem(list, 0);
		dev_info = (NAPIDevInfo_t *)HLIB_LIST_Data(list);
		if ( dev_info == NULL ) return NULL;
		HxLOG_Debug("  (#)get cache \n");
		return dev_info;
	}

	HxLOG_Debug("  (#)no cache \n");
	FuncLeave;

	return NULL;
}

NAPINetConfInfo_t *napi_cache_netconf_get(napicache_t *cache, HINT32 id)
{
	HUINT32 list_len, i = 0;
	NAPINetConfInfo_t *netconf = NULL;
	HxList_t *list = NULL;
	HxList_t *current_list = NULL;
	napicache_table_t *table = NULL;

	FuncEnter;

	table = __table(cache);
	if ( table == NULL ) return NULL;

	list = HLIB_LIST_First(table->netconfs);
	list_len = HLIB_LIST_Length(list);
	if(list_len)
	{
		for( i=0; i<list_len; i++ )
		{
			current_list = HLIB_LIST_GetListItem(list, i);
			netconf = (NAPINetConfInfo_t *)HLIB_LIST_Data(current_list);
			if ( netconf == NULL) return NULL;
			if( netconf->id == id )
			{
				//debug
				napi_debug_log_netconf(id, netconf);
				return netconf;
			}
		}
	}

	HxLOG_Debug("  (#)no cache \n");
	FuncLeave;

	return NULL;
}

NAPIWifiInfo_t *napi_cache_wifi_get(napicache_t *cache, HINT32 id)
{
	HUINT32 list_len = 0, i = 0;
	NAPIWifiInfo_t *ap_list = NULL;
	HxList_t *list = NULL;
	HxList_t *current_list = NULL;
	napicache_table_t *table = NULL;

	FuncEnter;

	table = __table(cache);
	if ( table == NULL ) return NULL;

	list = HLIB_LIST_First(table->wifi_aps);
	list_len = HLIB_LIST_Length(list);
	if(list_len)
	{
		for( i=0; i<list_len; i++ )
		{
			current_list = HLIB_LIST_GetListItem(list, i);
			ap_list = (NAPIWifiInfo_t *)HLIB_LIST_Data(current_list);
			if ( ap_list == NULL ) return NULL;
			if( ap_list->dev_id == id )
			{
				HxLOG_Debug("  (#)get cache \n");
				return ap_list;
			}
		}
	}

	HxLOG_Debug("  (#)no cache \n");

	FuncLeave;

	return NULL;
}

NAPIDHCPServerInfo_t *napi_cache_dhcpserver_get(napicache_t *cache, HINT32 index)
{
	HUINT32 list_len = 0;
	NAPIDHCPServerInfo_t *dhcpserver_info = NULL;
	HxList_t *list = NULL;
	napicache_table_t *table = NULL;

	FuncEnter;

	table = __table(cache);
	if ( table == NULL ) return NULL;

	list = HLIB_LIST_First(table->dhcp_server);
	list_len = HLIB_LIST_Length(list);
	if(list_len)
	{
		list = HLIB_LIST_GetListItem(list, 0);
		dhcpserver_info = (NAPIDHCPServerInfo_t *)HLIB_LIST_Data(list);
		if ( dhcpserver_info == NULL ) return NULL;
		HxLOG_Debug("  (#)get cache \n");
		return dhcpserver_info;
	}

	HxLOG_Debug("  (#)no cache \n");

	FuncLeave;

	return NULL;
}

NAPIBTInfo_t *napi_cache_bt_get(napicache_t *cache)
{
	NAPIBTInfo_t *bt_info = NULL;
	HxList_t *list = NULL;
	napicache_table_t *table = NULL;
	FuncEnter;

	table = __table(cache);
	if ( table == NULL ) return NULL;

	list = HLIB_LIST_GetListItem(HLIB_LIST_First(table->bt_info), 0);
	bt_info = (NAPIBTInfo_t *)HLIB_LIST_Data(list);
	if ( bt_info == NULL )
	{
		HxLOG_Debug("  (#)have no cache \n");
		return NULL;
	}
	HxLOG_Debug("  (#)get cache \n");

	FuncLeave;
	return bt_info;
}

NAPIPPPOEInfo_t *napi_cache_pppoe_get(napicache_t *cache)
{
	NAPIPPPOEInfo_t *pppoe_info = NULL;
	HxList_t *list = NULL;
	napicache_table_t *table = NULL;
	FuncEnter;

	table = __table(cache);
	if ( table == NULL ) return NULL;

	list = HLIB_LIST_GetListItem(HLIB_LIST_First(table->pppoe_info), 0);
	pppoe_info = (NAPIPPPOEInfo_t *)HLIB_LIST_Data(list);
	if ( pppoe_info == NULL )
	{
		HxLOG_Debug("  (#)have no cache \n");
		return NULL;
	}
	HxLOG_Debug("  (#)dev_id : %d, bOn : %d, userID : %s, userPW : %s\n",
			pppoe_info->dev_id, pppoe_info->bOn, pppoe_info->szUserID, pppoe_info->szUserPW);
	HxLOG_Debug("  (#)get cache \n");

	FuncLeave;
	return pppoe_info;
}


HINT32 napi_cache_netconf_get_count(napicache_t *cache)
{
	napicache_table_t *table = NULL;
	HxList_t *list = NULL;
	HINT32 data_count = 0;
	FuncEnter;

	table = __table(cache);
	if ( !table ) return -1;

	list = HLIB_LIST_First(table->netconfs);
	data_count = HLIB_LIST_Length(list);

	FuncLeave;
	return data_count;
}

HINT32 napi_cache_ap_get_count(napicache_t *cache)
{
	napicache_table_t *table = NULL;
	HxList_t *list = NULL;
	HINT32 data_count = 0;

	FuncEnter;

	table = __table(cache);
	if ( !table ) return -1;

	list = HLIB_LIST_First(table->wifi_aps);
	data_count = HLIB_LIST_Length(list);

	FuncLeave;
	return data_count;
}

HINT32 napi_cache_dhcpserver_get_count(napicache_t *cache)
{
	napicache_table_t *table = NULL;
	HxList_t *list = NULL;
	HINT32 data_count = 0;

	FuncEnter;

	table = __table(cache);
	if ( !table ) return -1;

	list = HLIB_LIST_First(table->dhcp_server);
	data_count = HLIB_LIST_Length(list);

	FuncLeave;
	return data_count;
}

HINT32 napi_cache_bt_get_count(napicache_t *cache)
{
	napicache_table_t *table = NULL;
	HxList_t *list = NULL;
	HINT32 data_count = 0;

	FuncEnter;

	table = __table(cache);
	if ( !table ) return -1;

	list = HLIB_LIST_First(table->bt_info);
	data_count = HLIB_LIST_Length(list);

	FuncLeave;
	return data_count;
}

HERROR napi_cache_netconf_update(napicache_t *cache, const NAPINetConfInfo_t *info)
{
	HUINT32 list_len, i = 0;
	NAPINetConfInfo_t *netconf = NULL;
	HxList_t *current_list = NULL;
	napicache_table_t *table = NULL;
	void *tmpbuf = NULL;

	FuncEnter;

	table = __table(cache);
	if ( !table ) return ERR_FAIL;
	if ( !info ) return ERR_FAIL;

	table->netconfs = HLIB_LIST_First(table->netconfs);
	list_len = HLIB_LIST_Length(table->netconfs);
	HxLOG_Debug("  (#)netconf list len : %d \n", list_len);
	if(list_len)
	{
		for( i=0; i<list_len; i++ )
		{
			current_list = HLIB_LIST_GetListItem(table->netconfs, i);
			netconf = (NAPINetConfInfo_t *)HLIB_LIST_Data(current_list);
			if ( !netconf ) return ERR_FAIL;
			if( info->id == netconf->id )
			{
				HxSTD_MemSet(netconf, 0x0, sizeof(NAPINetConfInfo_t));
				tmpbuf = HLIB_STD_MemDup(info, sizeof(NAPINetConfInfo_t));
				HxSTD_MemCopy(netconf, tmpbuf, sizeof(NAPINetConfInfo_t));
				table->netconfs = HLIB_LIST_First(table->netconfs);
				HxLOG_Debug("  (#)netconf update !! list_len : %d \n",
					HLIB_LIST_Length(table->netconfs));

				if(tmpbuf != NULL)
				{
					HLIB_STD_MemFree(tmpbuf);
					tmpbuf = NULL;
				}

				FuncLeave;
				return ERR_OK;
			}
		}
	}

	table->netconfs = HLIB_LIST_Append(table->netconfs, HLIB_STD_MemDup(info, sizeof(NAPINetConfInfo_t)));
	table->netconfs = HLIB_LIST_First(table->netconfs);
	HxLOG_Debug("  (#)netconf append !!: list_len : %d \n",
		HLIB_LIST_Length(table->netconfs));

	FuncLeave;
	return ERR_OK;
}

HERROR napi_cache_wifi_update(napicache_t *cache, const NAPIWifiInfo_t *info)
{
	HUINT32 list_len, i, dLen = 0;
	NAPIWifiInfo_t *ap_list = NULL;
	HxList_t *current_list = NULL;
	napicache_table_t *table = NULL;
	void *tmpbuf = NULL;

	FuncEnter;

	table = __table(cache);
	if ( !table ) return ERR_FAIL;
	if ( !info ) return ERR_FAIL;

	dLen = sizeof(NAPIWifiInfo_t) + sizeof(NAPIWifiAPInfo_t);
	table->wifi_aps = HLIB_LIST_First(table->wifi_aps);
	list_len = HLIB_LIST_Length(table->wifi_aps);
	HxLOG_Debug("  (#)wifi_aps list len : %d \n", list_len);
	if(list_len)
	{
		for( i=0; i<list_len; i++ )
		{
			current_list = HLIB_LIST_GetListItem(HLIB_LIST_First(table->wifi_aps), i);
			ap_list = (NAPIWifiInfo_t *)HLIB_LIST_Data(current_list);
			if ( !ap_list ) return ERR_FAIL;
			if( info->dev_id == ap_list->dev_id )
			{
				HxSTD_MemSet(ap_list, 0x0, dLen);
				tmpbuf = HLIB_STD_MemDup(info, dLen);
				HxSTD_MemCopy(ap_list, tmpbuf, dLen);
				HxLOG_Debug("  (#)wifi_aps update !! list_len : %d \n",
					HLIB_LIST_Length(HLIB_LIST_First(table->wifi_aps)));

				if(tmpbuf != NULL)
				{
					HLIB_STD_MemFree(tmpbuf);
					tmpbuf = NULL;
				}

				FuncLeave;
				return ERR_OK;
			}
		}
	}

	table->wifi_aps = HLIB_LIST_Append(table->wifi_aps, HLIB_STD_MemDup(info, dLen));
	HxLOG_Debug("  (#)ap append !!: list_len : %d \n",
		HLIB_LIST_Length(HLIB_LIST_First(table->wifi_aps)));
	FuncLeave;

	return ERR_OK;
}

HERROR napi_cache_devinfo_update(napicache_t *cache, const NAPIDevInfo_t *info)
{
	HUINT32 list_len = 0;
	NAPIDevInfo_t *dev_info = NULL;
	HxList_t *list = NULL;
	napicache_table_t *table = NULL;

	FuncEnter;

	table = __table(cache);
	if ( !table ) return ERR_FAIL;
	if ( !info ) return ERR_FAIL;

	list = HLIB_LIST_First(table->dev_info);
	list_len = HLIB_LIST_Length(list);
	if(list_len)
	{
		list = HLIB_LIST_GetListItem(list, 0);
		dev_info = (NAPIDevInfo_t *)HLIB_LIST_Data(list);
		HxSTD_MemCopy(dev_info, info, sizeof(NAPIDevInfo_t));
		HxLOG_Debug("  (#)dhcpserver update !! list len : %d \n",
			HLIB_LIST_Length(table->dev_info));
		return ERR_OK;
	}

	dev_info = HLIB_STD_MemDup(info, sizeof(NAPIDevInfo_t));
	table->dev_info = HLIB_LIST_Append(table->dev_info, dev_info);
	HxLOG_Debug("  (#)dev append !!: list_len : %d \n",
		HLIB_LIST_Length(table->dev_info));

	FuncLeave;
	return ERR_OK;
}

HERROR napi_cache_dhcpserver_update(napicache_t *cache, const NAPIDHCPServerInfo_t *info)
{
	HUINT32 list_len = 0;
	NAPIDHCPServerInfo_t *dhcpserver_info = NULL;
	HxList_t *current_list = NULL;
	napicache_table_t *table = NULL;
	void *tmpbuf = NULL;

	FuncEnter;

	table = __table(cache);
	if ( !table ) return ERR_FAIL;
	if ( !info ) return ERR_FAIL;

	table->dhcp_server = HLIB_LIST_First(table->dhcp_server);
	list_len = HLIB_LIST_Length(table->dhcp_server);
	HxLOG_Debug("  (#)dhcp_server list len : %d \n", list_len);
	if(list_len)
	{
		current_list = HLIB_LIST_GetListItem(table->dhcp_server, 0);
		dhcpserver_info = (NAPIDHCPServerInfo_t *)HLIB_LIST_Data(current_list);
		if ( !dhcpserver_info ) return ERR_FAIL;

		HxSTD_MemSet(dhcpserver_info, 0x0, sizeof(NAPIDHCPServerInfo_t));
		tmpbuf = HLIB_STD_MemDup(info, sizeof(NAPIDHCPServerInfo_t));
		HxSTD_MemCopy(dhcpserver_info, tmpbuf, sizeof(NAPIDHCPServerInfo_t));
		table->dhcp_server = HLIB_LIST_First(table->dhcp_server);
		HxLOG_Debug("  (#)dhcp_server update !! list_len : %d \n",
			HLIB_LIST_Length(table->dhcp_server));

		if(tmpbuf != NULL)
		{
			HLIB_STD_MemFree(tmpbuf);
			tmpbuf = NULL;
		}

		FuncLeave;
		return ERR_OK;
	}

	table->dhcp_server = HLIB_LIST_Append(table->dhcp_server, HLIB_STD_MemDup(info, sizeof(NAPIDHCPServerInfo_t)));
	table->dhcp_server = HLIB_LIST_First(table->dhcp_server);
	HxLOG_Debug("  (#)dhcp_server append !!: list_len : %d \n",
		HLIB_LIST_Length(table->dhcp_server));

	FuncLeave;
	return ERR_OK;
}

HERROR napi_cache_bt_update(napicache_t *cache, const NAPIBTInfo_t *info)
{
	HUINT32 list_len = 0;
	NAPIBTInfo_t *bt_info = NULL;
	HxList_t *current_list = NULL;
	napicache_table_t *table = NULL;
	void *tmpbuf = NULL;

	FuncEnter;

	table = __table(cache);
	if ( !table ) return ERR_FAIL;
	if ( !info ) return ERR_FAIL;

	table->bt_info = HLIB_LIST_First(table->bt_info);
	list_len = HLIB_LIST_Length(table->bt_info);
	HxLOG_Debug("  (#)bluetooth list len : %d \n", list_len);
	if(list_len)
	{
		current_list = HLIB_LIST_GetListItem(table->bt_info, 0);
		bt_info = (NAPIBTInfo_t *)HLIB_LIST_Data(current_list);
		if ( !bt_info ) return ERR_FAIL;

		HxSTD_MemSet(bt_info, 0x0, sizeof(NAPIBTInfo_t));
		tmpbuf = HLIB_STD_MemDup(info, sizeof(NAPIBTInfo_t));
		HxSTD_MemCopy(bt_info, tmpbuf, sizeof(NAPIBTInfo_t));
		table->bt_info = HLIB_LIST_First(table->bt_info);
		HxLOG_Debug("  (#)bluetooth update !! list_len : %d \n",
			HLIB_LIST_Length(table->bt_info));

		if(tmpbuf != NULL)
		{
			HLIB_STD_MemFree(tmpbuf);
			tmpbuf = NULL;
		}

		FuncLeave;
		return ERR_OK;
	}

	table->bt_info = HLIB_LIST_Append(table->bt_info, HLIB_STD_MemDup(info, sizeof(NAPIBTInfo_t)));
	table->bt_info = HLIB_LIST_First(table->bt_info);
	HxLOG_Debug("  (#)bluetooth append !!: list_len : %d \n",
		HLIB_LIST_Length(table->bt_info));

	FuncLeave;
	return ERR_OK;
}

HERROR napi_cache_pppoe_update_onoff(napicache_t *cache, const NAPIPPPOEInfo_t *info)
{
	HUINT32 list_len = 0;
	NAPIPPPOEInfo_t *pppoe_info = NULL;
	HxList_t *current_list = NULL;
	napicache_table_t *table = NULL;

	FuncEnter;
	if(!cache) return ERR_FAIL;

	table = __table(cache);
	if ( !table ) return ERR_FAIL;
	if ( !info ) return ERR_FAIL;
	HxLOG_Debug("  (#) info dev_id : %d, bOn : %d, userID : %s, userPW : %s\n",	info->dev_id, info->bOn, info->szUserID, info->szUserPW);

	table->pppoe_info = HLIB_LIST_First(table->pppoe_info);
	list_len = HLIB_LIST_Length(table->pppoe_info);
	HxLOG_Debug("  (#) pppoe list len : %d \n", list_len);
	if(list_len)
	{
		current_list = HLIB_LIST_GetListItem(table->pppoe_info, 0);
		pppoe_info = (NAPIPPPOEInfo_t *)HLIB_LIST_Data(current_list);
		if ( !pppoe_info ) return ERR_FAIL;

		pppoe_info->bOn = info->bOn;

		table->pppoe_info = HLIB_LIST_First(table->pppoe_info);

		HxLOG_Debug("  (#)pppoe update !! list_len : %d \n", HLIB_LIST_Length(table->pppoe_info));
		HxLOG_Debug("  (#)pppoe update !! pppoe_info dev_id : %d, bOn : %d, userID : %s, userPW : %s\n",
			pppoe_info->dev_id, pppoe_info->bOn, pppoe_info->szUserID, pppoe_info->szUserPW);

		FuncLeave;
		return ERR_OK;
	}

	table->pppoe_info = HLIB_LIST_Append(table->pppoe_info, HLIB_STD_MemDup(info, sizeof(NAPIPPPOEInfo_t)));
	table->pppoe_info = HLIB_LIST_First(table->pppoe_info);
	HxLOG_Debug("  (#)pppoe append !!: list_len : %d \n", HLIB_LIST_Length(table->pppoe_info));

	FuncLeave;
	return ERR_OK;
}

HERROR napi_cache_pppoe_update_info(napicache_t *cache, const NAPIPPPOEInfo_t *info)
{
	HUINT32 list_len = 0;
	NAPIPPPOEInfo_t *pppoe_info = NULL;
	HxList_t *current_list = NULL;
	napicache_table_t *table = NULL;
	void			  *tmpbuf = NULL;
	HBOOL tmp_on = FALSE;

	FuncEnter;
	if(!cache) return ERR_FAIL;

	table = __table(cache);
	if ( !table ) return ERR_FAIL;
	if ( !info ) return ERR_FAIL;
	HxLOG_Debug("  (#) info dev_id : %d, bOn : %d, userID : %s, userPW : %s\n",
			info->dev_id, info->bOn, info->szUserID, info->szUserPW);

	table->pppoe_info = HLIB_LIST_First(table->pppoe_info);
	list_len = HLIB_LIST_Length(table->pppoe_info);
	HxLOG_Debug("  (#) pppoe list len : %d \n", list_len);
	if(list_len)
	{
		current_list = HLIB_LIST_GetListItem(table->pppoe_info, 0);
		pppoe_info = (NAPIPPPOEInfo_t *)HLIB_LIST_Data(current_list);
		if ( !pppoe_info ) return ERR_FAIL;

		tmp_on = pppoe_info->bOn;
		HxSTD_MemSet(pppoe_info, 0x0, sizeof(NAPIPPPOEInfo_t));

		tmpbuf = HLIB_STD_MemDup(info, sizeof(NAPIPPPOEInfo_t));
		HxSTD_MemCopy(pppoe_info, tmpbuf, sizeof(NAPIPPPOEInfo_t));
		pppoe_info->bOn = tmp_on;

		table->pppoe_info = HLIB_LIST_First(table->pppoe_info);

		if(tmpbuf)
		{
			HLIB_STD_MemFree(tmpbuf);
		}

		HxLOG_Debug("  (#)pppoe update !! list_len : %d \n", HLIB_LIST_Length(table->pppoe_info));
		HxLOG_Debug("  (#)pppoe update !! pppoe_info dev_id : %d, bOn : %d, userID : %s, userPW : %s\n",
			pppoe_info->dev_id, pppoe_info->bOn, pppoe_info->szUserID, pppoe_info->szUserPW);

		FuncLeave;
		return ERR_OK;
	}

	table->pppoe_info = HLIB_LIST_Append(table->pppoe_info, HLIB_STD_MemDup(info, sizeof(NAPIPPPOEInfo_t)));
	table->pppoe_info = HLIB_LIST_First(table->pppoe_info);
	HxLOG_Debug("  (#)pppoe append !!: list_len : %d \n",
		HLIB_LIST_Length(table->pppoe_info));

	FuncLeave;
	return ERR_OK;
}


