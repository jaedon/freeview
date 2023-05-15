/*************************************************************************************************************
	File 		: nico_operator_bluetooth.c
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2013/07/15
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

	THE SOFTWARE IS PROVIDED ¢®¡ÆAS IS¢®¡¾ AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "nico_operator_bluetooth.h"

#include "di_bt.h"
#include "di_network.h"
#include "drv_err.h"
#include "drv_hotplug.h"

/*******************************************************************/
/*******************      template         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct 
{

	HINT32 device_count;
	NICO_NetDevice_Info_t **deviceinfo;
} st_bluetooth_context;

typedef st_bluetooth_context* pst_bluetooth_context;

typedef struct 
{
	st_bluetooth_context obj_ctx;
	void  (*listener)(void *dimsg);	
} st_bluetooth_object;
typedef st_bluetooth_object* pst_bluetooth_object;
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC st_bluetooth_object g_bluetooth_object;

/*******************************************************************/
/********************      Public 			 ******************/
/*******************************************************************/
pst_bluetooth_object bluetooth_object = &g_bluetooth_object;

/*******************************************************************/
/********************      Private Functions     ********************/
/*******************************************************************/
#if defined(CONFIG_PROD_DEVICE_BLUETOOTH)
typedef void (*NICO_BT_EVT_CALLBACK)(HINT32 etBtEventType, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);

STATIC pst_bluetooth_context __nico_bluetooth_getcontext(void)
{
	return &bluetooth_object->obj_ctx;
}

STATIC void	__nico_bluetooth_di_listener(void *param)
{
	DI_BT_CALLBACK_DATA_t *pData = (DI_BT_CALLBACK_DATA_t *)param;
	pst_bluetooth_context ctx = __nico_bluetooth_getcontext();
	(void*)(pData);
	(void*)(ctx);
	ENTRY;

	if(bluetooth_object->listener)
	{
		bluetooth_object->listener(param);
	}
	EXIT;
}

STATIC void __nico_bluetooth_send_extendevent(HINT32 id, HUINT32 event_type)
{
	DI_BT_CALLBACK_DATA_t evt = {.ulDevId = id, .eEvent = event_type};
	
	ENTRY;
	__nico_bluetooth_di_listener(&evt);
	EXIT;
}

STATIC HERROR __nico_bluetooth_set_enable(HBOOL enable)
{
	HERROR hRet = ERR_OK;
	ENTRY;
	hRet = DI_BT_Enable(enable);
	HxLOG_Debug("set enable : %d \n", enable);

	if ( ERR_OK != hRet )
	{
		HxLOG_Error("ERROR : DI_BT_Enable() Err(%d).\n", hRet);
	}
	EXIT;

	return hRet;
}

STATIC HERROR __nico_bluetooth_start_scan(eNicoBTProfile profile)
{
	HERROR hRet = ERR_OK;
	ENTRY;
	hRet = DI_BT_Scan(profile);
	HxLOG_Debug("start scan : profile : %d \n", profile);

	if ( ERR_OK != hRet )
	{
		HxLOG_Error("ERROR : DI_BT_Scan() Err(%d).\n", hRet);
	}
	EXIT;

	return hRet;
}

STATIC HERROR __nico_bluetooth_set_pincode(HUINT8 *bdaddr, HUINT8 *pincode)
{
	HERROR hRet = ERR_OK;
	ENTRY;

	if(!bdaddr)
		return ERR_FAIL;

	hRet = DI_BT_SendPinCode2BdAddr(bdaddr, pincode);
	HxLOG_Debug("[%s:%d] set pincode : bdaddr : %02X:%02X:%02X:%02X:%02X:%02X, pincode : %s \n",
		__FUNCTION__, __LINE__, bdaddr[0], bdaddr[1], bdaddr[2], bdaddr[3], bdaddr[4], bdaddr[5], pincode);

	if ( ERR_OK != hRet )
	{
		HxLOG_Error("ERROR : DI_BT_SendPinCode2BdAddr() Err(%d).\n", hRet);
	}
	EXIT;

	return hRet;
}


STATIC HERROR __nico_bluetooth_connect(HUINT8 *bdaddr)
{
	HERROR hRet = ERR_OK;
	ENTRY;

	if(!bdaddr)
		return ERR_FAIL;

	hRet = DI_BT_Connect2BdAddr(bdaddr);
	HxLOG_Debug("BT connect : btaddr : %02X:%02X:%02X:%02X:%02X:%02X \n",
		bdaddr[0], bdaddr[1], bdaddr[2], bdaddr[3], bdaddr[4], bdaddr[5]);

	if ( ERR_OK != hRet )
	{
		HxLOG_Error("ERROR : DI_BT_Connect2BdAddr() Err(%d).\n", hRet);
	}
	EXIT;

	return hRet;
}

STATIC HERROR __nico_bluetooth_disconnect(HUINT8 *bdaddr)
{
	HERROR hRet = ERR_OK;
	ENTRY;

	if(!bdaddr)
		return ERR_FAIL;

	hRet = DI_BT_Disconnect2BdAddr(bdaddr);
	HxLOG_Debug("BT connect : btaddr : %02X:%02X:%02X:%02X:%02X:%02X \n",
		bdaddr[0], bdaddr[1], bdaddr[2], bdaddr[3], bdaddr[4], bdaddr[5]);

	if ( ERR_OK != hRet )
	{
		HxLOG_Error("ERROR : DI_BT_Disconnect2BdAddr() Err(%d).\n", hRet);
	}
	EXIT;

	return hRet;
}

STATIC HERROR __nico_bluetooth_remove(HUINT8 *bdaddr)
{
	HERROR hRet = ERR_OK;
	ENTRY;

	if(!bdaddr)
		return ERR_FAIL;

	hRet = DI_BT_Remove2BdAddr(bdaddr);
	HxLOG_Debug("BT connect : btaddr : %02X:%02X:%02X:%02X:%02X:%02X \n",
		bdaddr[0], bdaddr[1], bdaddr[2], bdaddr[3], bdaddr[4], bdaddr[5]);

	if ( ERR_OK != hRet )
	{
		HxLOG_Error("ERROR : __nico_blueooth_remove() Err(%d).\n", hRet);
	}
	EXIT;

	return hRet;
}

STATIC HERROR __nico_bluetooth_remove_db(void)
{
	HERROR hRet = ERR_OK;
	ENTRY;

	hRet = DI_BT_DelXmlFiles();

	if ( ERR_OK != hRet )
	{
		HxLOG_Error("ERROR : __nico_blueooth_remove() Err(%d).\n", hRet);
	}
	EXIT;

	return hRet;
}

STATIC HERROR __nico_bluetooth_open_profile(HUINT8 *bdaddr, eNicoBTProfile profile)
{
	HERROR hRet = ERR_OK;
	ENTRY;

	if(!bdaddr)
		return ERR_FAIL;

	hRet = DI_BT_OpenProfile2BdAddr(bdaddr, profile);
	HxLOG_Debug("[%s:%d] BT connect : btaddr : %02X:%02X:%02X:%02X:%02X:%02X : profile : %d\n",
		__FUNCTION__, __LINE__, bdaddr[0], bdaddr[1], bdaddr[2], bdaddr[3], bdaddr[4], bdaddr[5], profile);

	if ( ERR_OK != hRet )
	{
		HxLOG_Error("ERROR : __nico_blueooth_open_profile() Err(%d).\n", hRet);
	}
	EXIT;

	return hRet;
}

STATIC HERROR __nico_bluetooth_close_profile(HUINT8 *bdaddr, eNicoBTProfile profile)
{
	HERROR hRet = ERR_OK;
	ENTRY;

	if(!bdaddr)
		return ERR_FAIL;

	hRet = DI_BT_CloseProfile2BdAddr(bdaddr, profile);
	HxLOG_Debug("[%s:%d] BT connect : btaddr : %02X:%02X:%02X:%02X:%02X:%02X : profile : %d\n",
		__FUNCTION__, __LINE__, bdaddr[0], bdaddr[1], bdaddr[2], bdaddr[3], bdaddr[4], bdaddr[5], profile);

	if ( ERR_OK != hRet )
	{
		HxLOG_Error("ERROR : DI_BT_CloseProfile2BdAddr() Err(%d).\n", hRet);
	}
	EXIT;

	return hRet;
}

STATIC HERROR __nico_bluetooth_get_hostinfo(NICO_BT_DevInfo_t *info)
{
	HERROR hRet = ERR_OK;
	DI_BT_DEV_INFO_t dev_info;
	HxSTD_MemSet(&dev_info, 0x00, sizeof(DI_BT_DEV_INFO_t));

	ENTRY;

	if(!info)
		return ERR_FAIL;

	hRet = DI_BT_GetHostInfo(&dev_info);
	if ( ERR_OK != hRet )
	{
		HxLOG_Error("ERROR : DI_BT_GetHostInfo() Err(%d).\n", hRet);
	}

	HxSTD_MemCopy(info->name, dev_info.ucName, NICO_BT_NAME_MAX);
	HxSTD_MemCopy(info->bdaddr, dev_info.ucBdAddr, NICO_BT_BDADDR_LEN);
	HxSTD_MemCopy(info->dev_class, dev_info.ucClass, NICO_BT_CLASS_LEN);
	info->status = dev_info.etDevStatus;
	info->available_profile = dev_info.etAvailableProfile;

	EXIT;

	return hRet;
}

STATIC HERROR __nico_bluetooth_get_device_list(NICO_BT_Info_t *info)
{
	HERROR hRet = ERR_OK;
	HUINT32 i = 0;
	DI_BT_DEV_LIST_t dev_list;

	ENTRY;

	if(!info)
		return ERR_FAIL;

	hRet = DI_BT_GetDeviceList(&dev_list);
	if ( ERR_OK != hRet )
	{
		HxLOG_Error("ERROR : DI_BT_GetDeviceList() Err(%d).\n", hRet);
	}

	info->count = dev_list.ulNumOfData;
	for( i=0; i<info->count; i++ )
	{
		HxSTD_MemCopy(info->dev_list[i].name, dev_list.stBtDevList[i].ucName, NICO_BT_NAME_MAX);
		HxSTD_MemCopy(info->dev_list[i].bdaddr, dev_list.stBtDevList[i].ucBdAddr, NICO_BT_BDADDR_LEN);
		HxSTD_MemCopy(info->dev_list[i].dev_class, dev_list.stBtDevList[i].ucClass, NICO_BT_CLASS_LEN);
		info->dev_list[i].status = dev_list.stBtDevList[i].etDevStatus;
		info->dev_list[i].available_profile = dev_list.stBtDevList[i].etAvailableProfile;
	}

	EXIT;

	return hRet;
}

psyncreturn_msg __nico_bluetooth_operate_sync(command_msg *params)
{
	psyncreturn_msg retmsg = HLIB_STD_MemCalloc(sizeof(syncreturn_msg));
	Cmd_Bluetooth_t *pcommand =  (Cmd_Bluetooth_t*)&params->cmd_protocol.cmd_bluetooth;

	if(NULL == retmsg)
	{
		HxLOG_Error("Out of Memory +++ \n");
	}
	else
	{
		HxSTD_MemCopy(retmsg, params, sizeof(syncreturn_msg));
		retmsg->type.evt_type = params->type.cmd_type;
		HxLOG_Debug("Wow Received sync_call!!!! [%x] \n", retmsg->type.evt_type);
		switch(params->type.cmd_type)
		{

			case CMD_GET_BT_HOST_INFO:
				{
					Cmd_BT_HostInfo_t *cmdinfo = &pcommand->cmd.cmd_get_hostinfo;
					__nico_bluetooth_get_hostinfo(&cmdinfo->dev_info);
					if(retmsg)
					{
						HxSTD_MemCopy(&retmsg->cmd_protocol.cmd_bluetooth.cmd.cmd_get_hostinfo.dev_info,
										&cmdinfo->dev_info, sizeof(NICO_BT_DevInfo_t));
					}
				}
				break;
			case CMD_GET_BT_DEVICE_LIST:
				{
					Cmd_BT_DevList_t *cmdinfo = &pcommand->cmd.cmd_get_devlist;
					__nico_bluetooth_get_device_list(&cmdinfo->info);
					if(retmsg)
					{
						HxSTD_MemCopy(&retmsg->cmd_protocol.cmd_bluetooth.cmd.cmd_get_devlist.info,
										&cmdinfo->info, sizeof(NICO_BT_Info_t));
					}
				}
				break;
			case CMD_UNKNOWN:
			default:
				break;
		}
	}
	return retmsg;
}

void __nico_bluetooth_operate_async(command_msg *params)
{
	pst_bluetooth_context ctx = __nico_bluetooth_getcontext();
	Cmd_Bluetooth_t *pcommand =  (Cmd_Bluetooth_t*)&params->cmd_protocol.cmd_bluetooth;
	//psyncreturn_msg retmsg = HLIB_STD_MemCalloc(sizeof(syncreturn_msg));

	ENTRY;
	/*
	if(TRUE == params->sync_call)
	{
		retmsg = __nico_bluetooth_operate_sync(params);
	}
	else
	{
	*/
		//HxSTD_MemCopy(retmsg, params, sizeof(syncreturn_msg));
		//retmsg->type.evt_type = params->type.cmd_type;
		switch(params->type.cmd_type)
		{
			case CMD_SET_BT_ENABLE:
				{
					Cmd_BT_Enable_t *cmdinfo = &pcommand->cmd.cmd_set_enable;
					__nico_bluetooth_set_enable(cmdinfo->enable);
				}
				break;
			case CMD_SET_BT_START_SCAN:
				{
					Cmd_BT_StartScan_t *cmdinfo = &pcommand->cmd.cmd_set_startscan;
					__nico_bluetooth_start_scan(cmdinfo->profile);
				}
				break;
			case CMD_SET_BT_PINCODE:
				{
					Cmd_BT_SendPincode_t *cmdinfo = &pcommand->cmd.cmd_set_pincode;
					__nico_bluetooth_set_pincode(cmdinfo->bdaddr, cmdinfo->pincode);
				}
				break;
			case CMD_SET_BT_CONNECT:
				{
					Cmd_BT_Connect_t *cmdinfo = &pcommand->cmd.cmd_set_connect;
					__nico_bluetooth_connect(cmdinfo->bdaddr);
				}
				break;
			case CMD_SET_BT_DISCONNECT:
				{
					Cmd_BT_Disconnect_t *cmdinfo = &pcommand->cmd.cmd_set_disconnect;
					__nico_bluetooth_disconnect(cmdinfo->bdaddr);
				}
				break;
			case CMD_SET_BT_REMOVE:
				{
					Cmd_BT_Remove_t *cmdinfo = &pcommand->cmd.cmd_set_remove;
					if( ERR_OK == __nico_bluetooth_remove(cmdinfo->bdaddr) )
					{
						__nico_bluetooth_send_extendevent(0, EVENT_NET_BT_REMOVED);
					}
				}
				break;
			case CMD_SET_BT_OPEN_PROFILE:
				{
					Cmd_BT_OpenProfile_t *cmdinfo = &pcommand->cmd.cmd_set_openprofile;
					__nico_bluetooth_open_profile(cmdinfo->bdaddr, cmdinfo->profile);
				}
				break;
			case CMD_SET_BT_CLOSE_PROFILE:
				{
					Cmd_BT_CloseProfile_t *cmdinfo = &pcommand->cmd.cmd_set_closeprofile;
					__nico_bluetooth_close_profile(cmdinfo->bdaddr, cmdinfo->profile);
				}
				break;
			case CMD_SET_BT_INFO_DB:
				{
					Cmd_BT_SetDB_t *cmdinfo = &pcommand->cmd.cmd_set_db;
					ctx->deviceinfo[ctx->device_count]->btinfo.enable = cmdinfo->info.enable;
				}
				break;
			case CMD_SET_BT_REMOVE_DB:
				{
					__nico_bluetooth_remove_db();
				}
				break;
			case CMD_UNKNOWN:
			default:
				break;
			}
		//}
}

/*******************************************************************/
/********************      Public Functions       ******************/
/*******************************************************************/
HERROR nico_bluetooth_init( HINT32 dev_count, NICO_NetDevice_Info_t **devices )
{
	HERROR hRet = ERR_OK;
	pst_bluetooth_context ctx = __nico_bluetooth_getcontext();
	DI_ERR_CODE diErr;
	ENTRY;

	HxLOG_Print("[%s,%d]\n", __FUNCTION__, __LINE__);

	diErr =  DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_INSERTED, __nico_bluetooth_di_listener);
	diErr |=  DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_EXTRACTED, __nico_bluetooth_di_listener);
	diErr |=  DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_ENABLE, __nico_bluetooth_di_listener);
	diErr |=  DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_DISABLE, __nico_bluetooth_di_listener);
	diErr |=  DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_SCAN_SUCCESS, __nico_bluetooth_di_listener);
	diErr |=  DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_SCAN_FAIL, __nico_bluetooth_di_listener);
	diErr |=  DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_AUTHEN_SUCCESS, __nico_bluetooth_di_listener);
	diErr |=  DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_AUTHEN_FAIL, __nico_bluetooth_di_listener);
	diErr |=  DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_CONNECT_SUCCESS, __nico_bluetooth_di_listener);
	diErr |=  DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_CONNECT_FAIL, __nico_bluetooth_di_listener);
	diErr |=  DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_PASSKEY, __nico_bluetooth_di_listener);
	if (diErr != DI_ERR_OK)
	{
		hRet = ERR_FAIL;
	}

	ctx->device_count = dev_count;
	ctx->deviceinfo = devices;
	EXIT;
	return hRet;
}

psyncreturn_msg nico_bluetooth_operate( command_msg *params )
{
	psyncreturn_msg retmsg = NULL;
	ENTRY;
	if(TRUE == params->sync_call)
	{

		retmsg = __nico_bluetooth_operate_sync(params);
	}
	else
	{
		__nico_bluetooth_operate_async(params);
	}
	EXIT;
	return retmsg;
}

HERROR nico_bluetooth_deinit( void )
{
	HERROR hRet = ERR_OK;
//	pst_bluetooth_context ctx = __nico_bluetooth_getcontext();

	if( bluetooth_object )
		bluetooth_object->listener = NULL;
	ENTRY;

	EXIT;
	return hRet;
}

HERROR nico_bluetooth_register_callback( void *callback )
{	
	ENTRY;

	if( bluetooth_object )
		bluetooth_object->listener = callback;
	
	EXIT;
	return ERR_OK;
}

#else
HERROR nico_bluetooth_init( HINT32 dev_count, NICO_NetDevice_Info_t **devices )
{
	//(void)__nico_bluetooth_send_extendevent;
	return ERR_FAIL;
}
psyncreturn_msg nico_bluetooth_operate( command_msg *params ){return NULL;}
HERROR nico_bluetooth_deinit( void ){return ERR_FAIL;}
HERROR nico_bluetooth_register_callback( void *callback ){return ERR_FAIL;}
#endif

