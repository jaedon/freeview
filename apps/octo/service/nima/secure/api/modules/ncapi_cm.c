/*************************************************************************************************************
	File 		: ncapi_cm.c
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2013/05/27
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

#include "ncapi_cm.h"

/*******************************************************************/
/********************      typedef		        ********************/
/*******************************************************************/
typedef struct
{
	HUINT32	semaphore;
	HxList_t *callback_list;
} st_ncapi_cm_object;
typedef st_ncapi_cm_object* pst_ncapi_cm_object;

st_ncapi_cm_object g_ncapi_cm_object =
{
	.callback_list = NULL,
};

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	ENTER_CRITICAL		HxSEMT_Get(g_ncapi_cm_object.semaphore)
#define	LEAVE_CRITICAL		HxSEMT_Release(g_ncapi_cm_object.semaphore)


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Private Functions     ********************/
/*******************************************************************/
void __ncapi_cm_update_info(NICO_CM_Info_t *src, NCAPI_CM_Info_t *dst)
{
	if(src && dst)
	{
		dst->state = src->state;
		dst->down_stream.ber = src->down_stream.ber;
		dst->down_stream.freq = src->down_stream.freq;
		dst->down_stream.qam_type = src->down_stream.qam_type;
		dst->down_stream.signal_level = src->down_stream.signal_level;
		dst->down_stream.snr = src->down_stream.snr;

		dst->up_stream.bandwidth = src->up_stream.bandwidth;
		dst->up_stream.channel_id = src->up_stream.channel_id;
		dst->up_stream.freq = src->up_stream.freq;
		dst->up_stream.signal_level = src->up_stream.signal_level;
	}
}

void __ncapi_cm_client_listener (pevent_msg event)
{
	eNico_event_type	nico_evt= event->type.evt_type;
	eNcapi_CM_LinkEvent ncapi_evt = eNCAPI_CM_EVENT_OFFLINE;
	NCAPI_CM_Info_t cmInfo = {0,};

	ENTRY;
	if(EVT_CM != (nico_evt & 0xffff0000))
	{
	}
	else
	{
		HxLOG_Debug("Got CM Event (%08x) \n", nico_evt);
		switch(nico_evt)
		{
			case EVENT_NET_CM_ONLINE:
			case EVENT_NET_CM_OFFLINE:
				__ncapi_cm_update_info(&event->evt_protocol.evt_cm.cminfo, &cmInfo);
				break;
			default:
				EXIT;
				return;
		}
		{
			HINT32 i = 0, cb_count = HLIB_LIST_Length(g_ncapi_cm_object.callback_list);
			for(i = 0; i < cb_count ; i++)
			{
				HxList_t *pFound = HLIB_LIST_GetListItem(g_ncapi_cm_object.callback_list, i);
				if(pFound)
				{
					NCAPI_CmCallback callback = (NCAPI_CmCallback)HLIB_LIST_Data(pFound);
					callback(ncapi_evt, 1, &cmInfo);
				}
			}
		}
	}
	EXIT;
}

/*******************************************************************/
/********************      Public Functions       ******************/
/*******************************************************************/
HERROR NCAPI_CM_Init(void)
{
	HERROR hRet = ERR_OK;
	ENTRY;
	hRet = HxSEMT_Create(&g_ncapi_cm_object.semaphore, "ncapi_cm_sem", 0);
	NCAPI_Client_RegisterCallback(__ncapi_cm_client_listener);
	EXIT;
	return hRet;

}
HERROR NCAPI_CM_Deinit(void)
{
	HERROR hRet = ERR_OK;
	ENTRY;
	hRet = HxSEMT_Destroy(g_ncapi_cm_object.semaphore);
	NCAPI_Client_UnregisterCallback(__ncapi_cm_client_listener);
	HxSTD_MemSet(&g_ncapi_cm_object, 0x00, sizeof(st_ncapi_cm_object));
	EXIT;
	return hRet;
}
HERROR NCAPI_CM_RegisterCallback(NCAPI_CmCallback callback)
{
	HERROR hRet = ERR_OK;
	HxList_t	*list = NULL;
	ENTRY;

	ENTER_CRITICAL;
	list = HLIB_LIST_Find(g_ncapi_cm_object.callback_list, callback);
	if (list)
	{
		HxLOG_Error("[%s] pfnObjectHandler(%X) is already registered!!!\n", __FUNCTION__, callback);
		hRet = ERR_FAIL;
	}
	else
	{
		g_ncapi_cm_object.callback_list = HLIB_LIST_Append(g_ncapi_cm_object.callback_list, (void*)callback);

	}
	LEAVE_CRITICAL;
	EXIT;
	return hRet;
}

HERROR NCAPI_CM_UnregisterCallback(NCAPI_CmCallback callback)
{
	HERROR	hRet = ERR_OK;
	HxList_t	*list = NULL;
	ENTRY;

	ENTER_CRITICAL;
	list = HLIB_LIST_Find(g_ncapi_cm_object.callback_list, callback);
	if (list == NULL)
	{
		HxLOG_Error("pfnObjectHandler(%X) does not registered yet..\n", callback);
		hRet = ERR_FAIL;
	}
	else
	{
		g_ncapi_cm_object.callback_list = HLIB_LIST_Remove(g_ncapi_cm_object.callback_list, callback);
	}
	LEAVE_CRITICAL;
	EXIT;
	return hRet;

}


HERROR NCAPI_CM_GetDownStreamInfo(NCAPI_CMDownStream_t *info)
{
	HERROR hRet = ERR_FAIL;
	//1 #### Not Implement


	return hRet;
}

HERROR NCAPI_CM_SetDownStreamInfo(const NCAPI_CMDownStream_t *info)
{
	HERROR hRet = ERR_OK;
	command_msg *msg = NULL;

	ENTRY;
	msg = HLIB_STD_MemAlloc(sizeof(*msg));
	if(NULL != msg)
	{
		HxSTD_MemSet(msg, 0x00, sizeof(*msg));
		msg->sync_call = FALSE;
		msg->type.cmd_type = CMD_SET_CM_DOWNSTREAM_INFO;
		hRet = NCAPI_Client_Send(msg);
		HxSTD_MemCopy(&msg->cmd_protocol.cmd_cm.cmd.cmd_set_downstreaminfo.downstreamInfo,
				info, sizeof(NCAPI_CMDownStream_t));
		HLIB_STD_MemFree(msg);
	}
	EXIT;
	
	return hRet;
}

HERROR NCAPI_CM_GetUpStreamInfo(NCAPI_CMUpStream_t *info)
{
	HERROR hRet = ERR_FAIL;
	//1 #### Not Implement

	return hRet;
}

eNcapiCmStatus NCAPI_CM_GetStatus()
{
	HERROR hRet = ERR_FAIL;
	//1 #### Not Implement


	return hRet;
}

