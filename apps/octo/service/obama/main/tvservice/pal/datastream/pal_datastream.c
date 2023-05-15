/*******************************************************************
	File Description
********************************************************************/
/**
	@file	mwc_datastream.c
	@brief	mwc_datastream.c

	Description:
	Module: MW / \n

	Copyright (c) 2011 HUMAX Co., Ltd.								\n
	All rights reserved.											\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <pal_datastream.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



//#define	SERVER_DATASTREAMER_PORT			"/tmp/.datastreamer.dat"

#define	palDs_DATATYPE_LEN						128
/*******************************************************************************/
/***************************  Typedefs           *******************************/
/*******************************************************************************/
typedef enum
{
	eDataStreamer_Open,
	eDataStreamer_Close,
	eDataStreamer_Broadcast
} palDs_Cmd_e;

typedef struct
{
	HUINT32	ulSize;
	HUINT8	*pucData;
} palDs_Data_t;

typedef struct
{
	palDs_Cmd_e		eCommand;
	HCHAR			szDataType[palDs_DATATYPE_LEN];
	union
	{
		HUINT32			ulClientId;
		palDs_Data_t	stDsData;
	} data;
} palDs_Msg_t;

typedef struct
{
	HUINT32		ulTaskId;
	HUINT32		ulSemId;
	HUINT32		ulMsgQueueId;

	HINT32		serverInfo;

	HxHASH_t		*dataTable;
} palDs_Info_t;

typedef	struct
{
	HCHAR		szDataType[palDs_DATATYPE_LEN];
	HxList_t	*clientList;
} palDs_DataInfo_t;

/*******************************************************************************/
/***************************  Static Variables   ******************************/
/*******************************************************************************/
STATIC palDs_Info_t		*s_pstDataStreamer = NULL;

/*******************************************************************************/
/***************************  Functions          *******************************/
/*******************************************************************************/

/*******************************************************************************/
/***************************  Local Functions    *******************************/
/*******************************************************************************/
HERROR	pal_dstream_Open(HUINT32 ulClientId, HCHAR *pszDataType)
{
	palDs_Msg_t		dsMsg;

	HxLOG_Debug("[%s] - clientId [%d] dataType [%s]\n", __FUNCTION__, ulClientId, pszDataType);

	HxSTD_MemSet(&dsMsg, 0, sizeof(palDs_Msg_t));
	HxSTD_StrNCpy(dsMsg.szDataType, pszDataType, palDs_DATATYPE_LEN - 1);

	dsMsg.eCommand = eDataStreamer_Open;
	dsMsg.data.ulClientId = ulClientId;

	VK_MSG_Send(s_pstDataStreamer->ulMsgQueueId, (void*)&dsMsg, sizeof(palDs_Msg_t));

	return ERR_OK;
}

HERROR	pal_dstream_Close(HUINT32 ulClientId, HCHAR *pszDataType)
{
	palDs_Msg_t		dsMsg;

	HxLOG_Debug("[%s] - clientId [%d] dataType [%s]\n", __FUNCTION__, ulClientId, pszDataType);

	HxSTD_MemSet(&dsMsg, 0, sizeof(palDs_Msg_t));
	HxSTD_StrNCpy(dsMsg.szDataType, pszDataType, palDs_DATATYPE_LEN - 1);

	dsMsg.eCommand = eDataStreamer_Close;
	dsMsg.data.ulClientId = ulClientId;

	VK_MSG_Send(s_pstDataStreamer->ulMsgQueueId, (void*)&dsMsg, sizeof(palDs_Msg_t));

	return ERR_OK;
}

STATIC void	pal_dstream_TraverseTable(void *key, void *value, void *user_data)
{
	palDs_DataInfo_t	*data = (palDs_DataInfo_t*)value;

	while (HLIB_LIST_Find(data->clientList, user_data))
	{
		data->clientList = HLIB_LIST_Remove(data->clientList, user_data);
	}
}

STATIC void	pal_dstream_NotifyStatus(void *userData, HxSocket_Status_e eStatus, HINT32 clientHandle)
{
	palDs_Info_t	*info = (palDs_Info_t*)userData;

	switch (eStatus)
	{
		case HxSOCKET_ADD_CLIENT:
			//	Nothing to do.
			HxLOG_Debug("Client is added [%d]\n", clientHandle);
			break;
		case HxSOCKET_REMOVE_CLIENT:
		case HxSOCKET_DISCONNECT_CLIENT:
			{
				//	remove client socket info.
				HLIB_HASH_Foreach(info->dataTable, pal_dstream_TraverseTable, (void*)clientHandle);
			}
			break;
		default:
			break;
	}
}

STATIC void	pal_dstream_ReceivePacket(void *userData, HINT32 socketHandle, HUINT32 requestId, HINT32 size, void *pvData)
{
	HCHAR		*op_code, *data_type;
	HxJSON_t	root, streamer;

	root = HLIB_JSON_Decode((HCHAR*)pvData);
	streamer = HxJSON_Object_Get(root, "datastreamer");

	op_code = (HCHAR*)HLIB_JSON_Object_GetString(streamer, "op_code");
	data_type = (HCHAR*)HLIB_JSON_Object_GetString(streamer, "datatype");

	if (HLIB_STD_StrCaseCmp(op_code, "connect") == 0)
	{
		pal_dstream_Open(socketHandle, data_type);
	} else
	if (HLIB_STD_StrCaseCmp(op_code, "disconnect") == 0)
	{
		pal_dstream_Close(socketHandle, data_type);
	} else
	{
		HxLOG_Critical("Unexpected data is coming... [%s]\n", pvData);
	}

	HLIB_JSON_Delete(root);
}

STATIC void	pal_dstream_Task(void *vp)
{
	palDs_Info_t	*info = (palDs_Info_t*)vp;
	palDs_Msg_t		dsMsg;
	HUINT32		ulTimeOut = 100;

	while (1)
	{
		if (VK_MSG_ReceiveTimeout(info->ulMsgQueueId, &dsMsg, sizeof(palDs_Msg_t), ulTimeOut) == VK_OK)
		{
			switch (dsMsg.eCommand)
			{
				case eDataStreamer_Open:
					{
						palDs_DataInfo_t	*dataInfo;

						dataInfo = (palDs_DataInfo_t*)HLIB_HASH_Lookup(info->dataTable, dsMsg.szDataType);
						if (dataInfo == NULL)
						{
							dataInfo = (palDs_DataInfo_t*)HLIB_STD_MemCalloc(sizeof(palDs_DataInfo_t));
							HLIB_HASH_Insert(info->dataTable, dsMsg.szDataType, (void*)dataInfo);
						}

						if (HLIB_LIST_Find(dataInfo->clientList, (const void*)dsMsg.data.ulClientId) == NULL)
							dataInfo->clientList = HLIB_LIST_Append(dataInfo->clientList, (void*)dsMsg.data.ulClientId);
					}
					break;
				case eDataStreamer_Close:
					{
						palDs_DataInfo_t	*dataInfo;

						dataInfo = (palDs_DataInfo_t*)HLIB_HASH_Lookup(info->dataTable, dsMsg.szDataType);
						if (dataInfo)
						{
							dataInfo->clientList = HLIB_LIST_Remove(dataInfo->clientList, (void*)dsMsg.data.ulClientId);
							if (dataInfo->clientList == NULL)
							{
								HLIB_HASH_Remove(info->dataTable, dsMsg.szDataType);
								HLIB_STD_MemFree(dataInfo);
							}
						}
					}
					break;
				case eDataStreamer_Broadcast:
					{
						palDs_DataInfo_t	*dataInfo;

						dataInfo = (palDs_DataInfo_t*)HLIB_HASH_Lookup(info->dataTable, dsMsg.szDataType);
						if (dataInfo)
						{
							HxList_t	*list = dataInfo->clientList;

							while (list)
							{

								HxLOG_Debug("WriteData Client : [%d]\n", HLIB_LIST_Data(list));
								HLIB_SOCKET_Write((HINT32)HLIB_LIST_Data(list), (void*)dsMsg.data.stDsData.pucData, dsMsg.data.stDsData.ulSize, 0);

								list = list->next;
							}
						}
						HLIB_STD_MemFree(dsMsg.data.stDsData.pucData);
					}
					break;
				default:
					break;
			}
			continue;
		}

		HLIB_SOCKET_Listen(info->serverInfo, FALSE, pal_dstream_ReceivePacket, pal_dstream_NotifyStatus, info);
	}
}

STATIC HERROR	pal_dstream_Init(void)
{
	s_pstDataStreamer = (palDs_Info_t*)HLIB_STD_MemCalloc(sizeof(palDs_Info_t));

	s_pstDataStreamer->dataTable = HLIB_HASH_New(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual);
	s_pstDataStreamer->serverInfo = HLIB_EXTRA_OpenServer("datastream");//HLIB_SOCKET_OpenServer(HxSOCKET_UDS, (void*)SERVER_DATASTREAMER_PORT);

	HxSEMT_Create((HUINT32*)&s_pstDataStreamer->ulSemId, (HCHAR*)"ds_sem", 0);
	if (VK_MSG_Create(MW_DATASTREAMER_MSGQ_SIZE, sizeof(palDs_Msg_t), "ds_msg", (unsigned long *)&s_pstDataStreamer->ulMsgQueueId, VK_SUSPENDTYPE_FIFO) != VK_OK)
	{
		HxLOG_Critical("VK_MSG_Create() failed..!! \n");
	}

	VK_TASK_Create(pal_dstream_Task, VK_TASK_PRIORITY_MW_REF, SIZE_64K, "datastreamer", (void*)s_pstDataStreamer, (unsigned long*)&s_pstDataStreamer->ulTaskId, 0);
	VK_TASK_Start(s_pstDataStreamer->ulTaskId);

	return ERR_OK;
}

/*******************************************************************************/
/****************************** Global Functions *******************************/
/*******************************************************************************/
#define	CHECK_IF_INITIALIZED(a, b)		do {if (a == NULL) b();} while(0)
HERROR	PAL_DSTREAM_Init(void)
{
	CHECK_IF_INITIALIZED(s_pstDataStreamer, pal_dstream_Init);

	return ERR_OK;
}

HERROR	PAL_DSTREAM_Broadcast(HCHAR *pszDataType, HUINT32 ulSize, void *pData)
{
	HUINT8		*p;
	HINT32		newLen;
	palDs_Msg_t		dsMsg;

	HxLOG_Debug("[%s] - dataType[%s] size[%d]\n", __FUNCTION__, pszDataType, ulSize);

	HxSTD_MemSet(&dsMsg, 0, sizeof(palDs_Msg_t));
	HxSTD_StrNCpy(dsMsg.szDataType, pszDataType, palDs_DATATYPE_LEN - 1);

	dsMsg.eCommand = eDataStreamer_Broadcast;

	newLen = ulSize + (HxSTD_StrLen(pszDataType) + 1) + 4;		//	datatype len(4) + dataType + data
	dsMsg.data.stDsData.ulSize = newLen;
	dsMsg.data.stDsData.pucData = HLIB_STD_MemAlloc(newLen);
	p = dsMsg.data.stDsData.pucData;

	HxMACRO_Put32Bit(p, HxSTD_StrLen(pszDataType) + 1);			p += 4;
	HxSTD_StrNCpy((HCHAR*)p, pszDataType, HxSTD_StrLen(pszDataType));	p += HxSTD_StrLen(pszDataType);
	*p = 0;														p ++;
	HxSTD_MemCopy(p, pData, ulSize);

	if (VK_MSG_SendTimeout(s_pstDataStreamer->ulMsgQueueId, (void*)&dsMsg, sizeof(palDs_Msg_t), 10) != VK_OK)
	{
		HxLOG_Warning("send timeout \n");
		HLIB_STD_MemFree(dsMsg.data.stDsData.pucData);

		return ERR_FAIL;
	}

	return ERR_OK;
}

/*********************** End of File ******************************/
