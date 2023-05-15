/** **********************************************************************************************************
	File 		: napi_bluetooth.c
	author 		:
	comment		: BlueTooth
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
#include "napi_bluetooth.h"
#include "napi_protocol.h"
#include "napi_cache.h"
#include "napi_agent_common.h"
#include "napi_parser.h"
#include "../napi_debug.h"

/*-----------------------------------------------------------------------------------------------
 *	pre-definition
 *----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
 *	internal functions
 *----------------------------------------------------------------------------------------------*/
STATIC void __napi_bt_log_bt(NAPIBTDevInfo_t *info, HINT32 count)
{
	HINT32 i = 0;

	for( i=0; i<count; i++ )
	{
		if(info)
		{
			HxLOG_Debug("  (#)++ Debug BT Info +++++++++++++++++++++++++++++++ \n");
			HxLOG_Debug("  (#)dev_id					 : %d \n", info[i].dev_id);
			HxLOG_Debug("  (#)name						 : %s \n", info[i].name);
			HxLOG_Debug("  (#)mac_addr					 : %s \n", info[i].bdaddr);
			HxLOG_Debug("  (#)device class				 : %x:%x:%x \n", info[i].dev_class[0], info[i].dev_class[1], info[i].dev_class[2]);
			HxLOG_Debug("  (#)status 					 : %d \n", info[i].status);
			HxLOG_Debug("  (#)avaliable profiles 		 : %d \n", info[i].support_profile);
			HxLOG_Debug("  (#)----------------------------------------------- \n");
		}
	}
}

/*-----------------------------------------------------------------------------------------------
 *	interfaces
 *----------------------------------------------------------------------------------------------*/

 HERROR  NAPI_BT_RegisterCallback(NAPI_BTCallback callback)
 {
	 HERROR  eRet = ERR_OK;
	 FuncEnter;
	 napi_agent_register_callback(napi_agent_get_instance(), callback, eAGENT_EVENT_TYPE_BT);

	 FuncLeave;
	 return eRet;
 }

 HERROR  NAPI_BT_SetEnable(HBOOL enable)
 {
	HERROR	eRet = ERR_OK;
	NAPIProtocol_cmd_bt_t *bt_cmd = NULL;
	napipacket_t *packet = NULL;
	HINT32 dLen = 0, packet_len = 0;

	FuncEnter;

	dLen = sizeof(NAPIProtocol_cmd_bt_t);
	bt_cmd = HLIB_STD_MemAlloc(dLen);
	if( NULL != bt_cmd )
	{
		HxSTD_MemSet(bt_cmd, 0x00, dLen);
		bt_cmd->enable = enable;

		packet_len = sizeof(napipacket_t) + dLen;
		packet = HLIB_STD_MemAlloc(packet_len);
		if( NULL != packet )
		{
			HxSTD_MemSet(packet, 0x0, packet_len);
			packet->type = NAPI_PROTOTYPE_REQUEST;
			packet->code = NAPI_PROTO_BT_ENABLE;
			packet->dlen = dLen;
			HxSTD_MemCopy(packet->data, bt_cmd, packet->dlen);

			eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, packet_len);
		}
	}

	if( NULL != bt_cmd ) HLIB_STD_MemFree(bt_cmd);
	if( NULL != packet ) HLIB_STD_MemFree(packet);

	FuncLeave;
	return eRet;
 }

 HERROR  NAPI_BT_GetEnable(HBOOL *enable)
 {
	HERROR	eRet = ERR_OK;
	NAPIBTInfo_t *bt_info = NULL;
	napicache_t *cache = NULL;

	FuncEnter;

	cache = napi_cache_get_instance();
	if ( cache == NULL ) return 0;

	bt_info = (NAPIBTInfo_t *)napi_cache_bt_get(cache);
	if( !bt_info )
	{
		HxLOG_Debug("	(#)have no bt info !!! \n");
		return ERR_OK;
	}

	if( !enable )
	{
		HxLOG_Debug("	(#)pointer null \n");
		return ERR_FAIL;
	}
	*enable = bt_info->enable;

	HxLOG_Debug("	(#)get enable : %d \n", *enable);

	FuncLeave;
	return eRet;
 }

 HERROR  NAPI_BT_Scan(eNapiBTProfile profile)
 {
 	HERROR	eRet = ERR_OK;
	NAPIProtocol_cmd_bt_t *bt_cmd = NULL;
	napipacket_t *packet = NULL;
	HINT32 dLen = 0, packet_len = 0;
	NAPIBTInfo_t *bt_info = NULL;

	FuncEnter;

	bt_info = napi_cache_bt_get(napi_cache_get_instance());
	if( !bt_info->enable )
		return ERR_FAIL;

	dLen = sizeof(NAPIProtocol_cmd_bt_t);
	bt_cmd = HLIB_STD_MemAlloc(dLen);
	if( NULL != bt_cmd )
	{
		HxSTD_MemSet(bt_cmd, 0x00, dLen);
		bt_cmd->profile = profile;

		packet_len = sizeof(napipacket_t) + dLen;
		packet = HLIB_STD_MemAlloc(packet_len);
		if( NULL != packet )
		{	
			HxSTD_MemSet(packet, 0x0, packet_len);
			packet->type = NAPI_PROTOTYPE_REQUEST;
			packet->code = NAPI_PROTO_BT_SCAN;
			packet->dlen = dLen;
			HxSTD_MemCopy(packet->data, bt_cmd, packet->dlen);

			eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, packet_len);
		}
	}

	if( NULL != bt_cmd ) HLIB_STD_MemFree(bt_cmd);
	if( NULL != packet ) HLIB_STD_MemFree(packet);

	FuncLeave;
	return eRet;
 }

 HERROR  NAPI_BT_Connect(const HCHAR *bdaddr)
 {
 	HERROR	eRet = ERR_OK;
	NAPIProtocol_cmd_bt_t *bt_cmd = NULL;
	napipacket_t *packet = NULL;
	HINT32 dLen = 0, packet_len = 0;
	NAPIBTInfo_t *bt_info = NULL;

	FuncEnter;

	bt_info = napi_cache_bt_get(napi_cache_get_instance());
	if( !bt_info->enable )
		return ERR_FAIL;

	if( !bdaddr )
		return ERR_FAIL;

	dLen = sizeof(NAPIProtocol_cmd_bt_t);
	bt_cmd = HLIB_STD_MemAlloc(dLen);
	if( NULL != bt_cmd )
	{
		HxSTD_MemSet(bt_cmd, 0x00, dLen);
		napi_parser_macAtoN(bdaddr, bt_cmd->bdaddr);

		packet_len = sizeof(napipacket_t) + dLen;
		packet = HLIB_STD_MemAlloc(packet_len);
		if( NULL != packet )
		{
			HxSTD_MemSet(packet, 0x0, packet_len);
			packet->type = NAPI_PROTOTYPE_REQUEST;
			packet->code = NAPI_PROTO_BT_CONNECT;
			packet->dlen = dLen;
			HxSTD_MemCopy(packet->data, bt_cmd, packet->dlen);

			eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, packet_len);
		}
	}

	if( NULL != bt_cmd ) HLIB_STD_MemFree(bt_cmd);
	if( NULL != packet ) HLIB_STD_MemFree(packet);

	FuncLeave;
	return eRet;
 }

 HERROR  NAPI_BT_Disconnect(const HCHAR *bdaddr)
 {
	 HERROR  eRet = ERR_OK;
	 NAPIProtocol_cmd_bt_t *bt_cmd = NULL;
	 napipacket_t *packet = NULL;
	 HINT32 dLen = 0, packet_len = 0;
	 NAPIBTInfo_t *bt_info = NULL;

	 FuncEnter;

	bt_info = napi_cache_bt_get(napi_cache_get_instance());
	if( !bt_info->enable )
		return ERR_FAIL;

	 if( !bdaddr )
		 return ERR_FAIL;

	 dLen = sizeof(NAPIProtocol_cmd_bt_t);
	 bt_cmd = HLIB_STD_MemAlloc(dLen);
	 if( NULL != bt_cmd )
	 {
		 HxSTD_MemSet(bt_cmd, 0x00, dLen);
		 napi_parser_macAtoN(bdaddr, bt_cmd->bdaddr);

		 packet_len = sizeof(napipacket_t) + dLen;
		 packet = HLIB_STD_MemAlloc(packet_len);
		 if( NULL != packet ) 
		 {
			 HxSTD_MemSet(packet, 0x0, packet_len);
			 packet->type = NAPI_PROTOTYPE_REQUEST;
			 packet->code = NAPI_PROTO_BT_DISCONNECT;
			 packet->dlen = dLen;
			 HxSTD_MemCopy(packet->data, bt_cmd, packet->dlen);

			 eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, packet_len);
		 }
	 }

	 if( NULL != bt_cmd ) HLIB_STD_MemFree(bt_cmd);
	 if( NULL != packet ) HLIB_STD_MemFree(packet);

	 FuncLeave;
	 return eRet;
 }

 HERROR  NAPI_BT_Remove(const HCHAR *bdaddr)
 {
	 HERROR  eRet = ERR_OK;
	 NAPIProtocol_cmd_bt_t *bt_cmd = NULL;
	 napipacket_t *packet = NULL;
	 HINT32 dLen = 0, packet_len = 0;
	 NAPIBTInfo_t *bt_info = NULL;

	 FuncEnter;

	bt_info = napi_cache_bt_get(napi_cache_get_instance());
	if( !bt_info->enable )
		return ERR_FAIL;

	 if( !bdaddr )
		 return ERR_FAIL;

	 dLen = sizeof(NAPIProtocol_cmd_bt_t);
	 bt_cmd = HLIB_STD_MemAlloc(dLen);
	 if( NULL != bt_cmd )
	 {
		 HxSTD_MemSet(bt_cmd, 0x00, dLen);
		 napi_parser_macAtoN(bdaddr, bt_cmd->bdaddr);

		 packet_len = sizeof(napipacket_t) + dLen;
		 packet = HLIB_STD_MemAlloc(packet_len);
		 if( NULL != packet )
		 {
			 HxSTD_MemSet(packet, 0x0, packet_len);
			 packet->type = NAPI_PROTOTYPE_REQUEST;
			 packet->code = NAPI_PROTO_BT_REMOVE;
			 packet->dlen = dLen;
			 HxSTD_MemCopy(packet->data, bt_cmd, packet->dlen);

			 eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, packet_len);
		 }
	 }

	 if( NULL != bt_cmd ) HLIB_STD_MemFree(bt_cmd);
	 if( NULL != packet ) HLIB_STD_MemFree(packet);

	 FuncLeave;
	 return eRet;
 }

 HERROR  NAPI_BT_SendPinCode(const HCHAR *bdaddr, HCHAR *pincode)
 {
	 HERROR  eRet = ERR_OK;
	 NAPIProtocol_cmd_bt_t *bt_cmd = NULL;
	 napipacket_t *packet = NULL;
	 HINT32 dLen = 0, packet_len = 0;
	 NAPIBTInfo_t *bt_info = NULL;

	 FuncEnter;

	bt_info = napi_cache_bt_get(napi_cache_get_instance());
	if( !bt_info->enable )
		return ERR_FAIL;

	 if( !bdaddr || !pincode)
		 return ERR_FAIL;

	 dLen = sizeof(NAPIProtocol_cmd_bt_t);
	 bt_cmd = HLIB_STD_MemAlloc(dLen);
	 if( NULL != bt_cmd )
	 {
		 HxSTD_MemSet(bt_cmd, 0x00, dLen);
		 napi_parser_macAtoN(bdaddr, bt_cmd->bdaddr);
		 HxSTD_MemCopy(bt_cmd->pincode, pincode, NAPI_PROTO_BT_PINCODE_LEN);

		 packet_len = sizeof(napipacket_t) + dLen;
		 packet = HLIB_STD_MemAlloc(packet_len);
		 if( NULL != packet )
		 {
			 HxSTD_MemSet(packet, 0x0, packet_len);
			 packet->type = NAPI_PROTOTYPE_REQUEST;
			 packet->code = NAPI_PROTO_BT_SEND_PINCODE;
			 packet->dlen = dLen;
			 HxSTD_MemCopy(packet->data, bt_cmd, packet->dlen);

			 eRet = napi_agent_write_data(napi_agent_get_instance(), (HCHAR *)packet, packet_len);
		 }
	 } 

	 if( NULL != bt_cmd ) HLIB_STD_MemFree(bt_cmd);
	 if( NULL != packet ) HLIB_STD_MemFree(packet);

	 FuncLeave;
	 return eRet;
 }

 HUINT32 NAPI_BT_GetDeviceNum(void)
 {
	 NAPIBTInfo_t *devinfo = NULL;
	 HINT32 dev_count = 0;
	 napicache_t *cache = NULL;

	 FuncEnter;

	 cache = napi_cache_get_instance();
	 if ( cache == NULL ) return 0;

	 devinfo = (NAPIBTInfo_t *)napi_cache_bt_get(cache);
	 if( !devinfo ) return 0;

	 dev_count = devinfo->dev_count;

	 HxLOG_Debug("	(#)bluetooth device count : %d \n", dev_count);

	 FuncLeave;
	 return dev_count;
 }

 HERROR  NAPI_BT_GetDeviceInfo(HUINT32 index, NAPIBTDevInfo_t *info)
 {
	 NAPIBTInfo_t *devinfo = NULL;
	 napicache_t *cache = NULL;

	 FuncEnter;

	 cache = napi_cache_get_instance();
	 if ( cache == NULL ) return 0;

	 devinfo = (NAPIBTInfo_t *)napi_cache_bt_get(cache);
	 if( !devinfo )
	 {
		 HxLOG_Debug("	(#)have no devinfo \n");
		 return ERR_OK;
	 }
	 if(!info)
	 {
		 HxLOG_Debug("	(#)pointer null \n");
		 return ERR_FAIL;
	 }
	 HxSTD_MemCopy(info, &devinfo->dev_info[index], sizeof(NAPIBTDevInfo_t));

	 HxLOG_Debug("	(#)get device[%d] info \n", index);

	 FuncLeave;
	 return ERR_OK;
 }

 HERROR  NAPI_BT_GetHostMacAddrInfo(HCHAR *bdaddr)
 {
	 NAPIBTInfo_t *devinfo = NULL;
	 napicache_t *cache = NULL;

	 FuncEnter;

	 cache = napi_cache_get_instance();
	 if ( cache == NULL ) return 0;

	 devinfo = (NAPIBTInfo_t *)napi_cache_bt_get(cache);
	 if( !devinfo )
	 {
		 HxLOG_Debug("	(#)have no devinfo \n");
		 return ERR_OK;
	 }
	 if(!bdaddr)
	 {
		 HxLOG_Debug("	(#)pointer null \n");
		 return ERR_FAIL;
	 }
	 HxSTD_MemCopy(bdaddr, devinfo->host_info.bdaddr, NAPI_BT_BDADDR_LEN);

	 HxLOG_Debug("	(#)get host mac : %s \n", bdaddr);

	 FuncLeave;
	 return ERR_OK;
 }

 HUINT32 NAPI_BT_GetHostMacAddrInfoSize(void)
 {
 	return NAPI_BT_BDADDR_LEN;
 }

 HINT32 NAPI_BT_GetDevInfoList(HUINT32 dev_count, NAPIBTDevInfo_t *info)
 {
	NAPIBTInfo_t *bt_info = NULL;
	napicache_t *cache = NULL;
	HINT32 count = 0;

	FuncEnter;

	count = dev_count;
	cache = napi_cache_get_instance();
	if ( cache == NULL ) return 0;

	bt_info = (NAPIBTInfo_t *)napi_cache_bt_get(cache);
	if( !bt_info )
	{
		HxLOG_Debug("	(#)have no devinfo \n");
		return ERR_OK;
	}

	if(!info)
	{
		HxLOG_Debug("	(#)pointer null \n");
		return ERR_FAIL;
	}

	if( dev_count < bt_info->dev_count )
		count = dev_count;

	HxSTD_MemCopy(info, bt_info->dev_info, count*sizeof(NAPIBTDevInfo_t));
	__napi_bt_log_bt(info, count);

	HxLOG_Debug("	(#)get device list : dev count : %d\n", count);

	FuncLeave;
	return count;
 }


