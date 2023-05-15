/*************************************************************************************************************
	File            : pl_dmp_jp.c
	author          : IPM tipmsw1@humaxdigital.com
	comment         :
	date            :
	attention       :

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

#define	________________Header_Files_______________________________

#include <hlib.h>

#include "vkernel.h"
#include "mxdlna.h"
#include "mxres.h"
#include "mxfile.h"
#include "mxdlna_mscp.h"
#include "mxmedia_define.h"
#include <sys/time.h>

#include "common.h"
#include "pl_common.h"
#include "pl_dmp.h"

#define ________________Private_Mecro_and_Defines__________________
#define	__MODULE__	"PL:DMP:JP"

#define PL_DMP_ASSERT(x)	\
    if ( x == NULL )	\
    {\
		 HxLOG_Error("[ %s ] : the param is NULL.\n", __MODULE__);\
		 return ERR_FAIL;\
    }\

#define	PL_DMP_SNPRINTF(x, v)			HxSTD_snprintf(x, sizeof(x), "%s", v)

#define PL_DMP_CHECK_OBJ(x)	 \
	{\
		object_t *v = NULL;\
		PL_DMP_ASSERT(x); \
		v = __find_obj((object_t*)x); \
		if(v == NULL)\

		{\
			HxLOG_Error("[ %s ] : the param is NULL.\n", __MODULE__);\
		 	return ERR_FAIL;\
		}\
	}\

#define ________________Internal_Typedef_and_Structure_____________
typedef enum
{
    object_Create = 0,
    object_Ready,

    object_Req_DmsInfo,
    object_Res_DmsInfo,

    object_Req_PostUrl,
    object_Res_PostUrl,

    object_Start,
    object_Process,
    object_Finished,
} object_status_e;

typedef enum
{
	Object_Recording,
	Object_Dubbing,
} object_type_e;

typedef struct
{
    object_type_e		type;
    HCHAR				*udn;

    object_status_e		status;

    HCHAR				*destination;

    HCHAR				*protocol;
    HCHAR				*url;
    HCHAR				*object_id;

    HCHAR				*profile;
	HCHAR				*container_id;

    HCHAR				*pPath;
    HINT32				content_id;

    Handle_t			hRingBuf;

    //MXDLNA_MSCP_XSRS_ARGS_CREATEOBJECT  MxObj;
} object_t;

typedef struct
{
	/*  created object list  */
	HxList_t            		*obj_list;

	/*  waiting object list for dms response  */
	//HxList_t            		*obj_list;
	/*  todo - add timer for timeout  */

	PL_DMP_JP_ResultCallback	cb;

	HULONG  task;
	HULONG  msg;
	HULONG  lock;

} jp_context_t;

typedef enum
{
	DmsInfo = 0,
	Add_Object,
	Add_ObjectRetry,
	Del_Object,

	Send_StartPacket,
	Send_UploadStart,
	Send_UploadStop,
	Send_EndPacket,
	Send_CleanUp,
} Upload_Cmd;

typedef struct
{
    Upload_Cmd   cmd;
    object_t 	*obj;
} jp_upload_cmd_t;

#define ________________Private_Static_Variables___________________
static jp_context_t	stContext;

#define ________________Internal_APIs_Prototype____________________
static object_t * __obj_by_cds(MXDLNA_CDSOBJECT *pObject, HCHAR *pUdn);

#define ________________Internal_Functions_________________________
jp_context_t *__ctx(void)
{
	return &stContext;
}

static int __convert_time_string_to_unix(HCHAR *TimeString, UNIXTIME *pOut)
{
    HxDATETIME_t in;
    UNIXTIME out;
    HERROR  ret;

    char *tmp = NULL;
    tmp = HxMEM_REALLOC(tmp,   2);

    HxSTD_MemCopy(tmp, TimeString+5, 2);    in.stDate.ucMonth   = atoi(tmp);
    HxSTD_MemCopy(tmp, TimeString+8, 2);    in.stDate.ucDay     = atoi(tmp);

    HxSTD_MemCopy(tmp, TimeString+11, 2);   in.stTime.ucHour    = atoi(tmp);
    HxSTD_MemCopy(tmp, TimeString+14, 2);   in.stTime.ucMinute  = atoi(tmp);
    HxSTD_MemCopy(tmp, TimeString+17, 2);   in.stTime.ucSecond  = atoi(tmp);

    tmp = HxMEM_REALLOC(tmp,  4);
    HxSTD_MemCopy(tmp, TimeString, 4);      in.stDate.usYear    = atoi(tmp);
    HLIB_STD_MemFree(tmp);

    ret = HLIB_DATETIME_ConvertDateTimeToUnixTime(&in, &out);
    HxSTD_MemCopy(pOut, &out, sizeof(UNIXTIME));
    //HLIB_DATETIME_PrintDateTime(*pOut);
    return ret;
}

static void __debug_mxdlna_data(HINT32 action, HCHAR *udn, void *data)
{
	switch(action)
	{
#if 0
		case MXDLNA_MSCP_X_CREATERECORDSCHEDULE :
			{
				MXDLNA_MSCP_XSRS_RESPONSE_XCREATERECORDSCHEDULE * pCreateObjResp = (MXDLNA_MSCP_XSRS_RESPONSE_XCREATERECORDSCHEDULE*)data;
				HxLOG_Print( "RecordScheduleID(%s) ID(%s) MediaRemainAlertID(%s) UpdateID(%s)\n\n",
						pCreateObjResp->RecordScheduleID,pCreateObjResp->ID,pCreateObjResp->MediaRemainAlertID,pCreateObjResp->UpdateID );
			}
			break;

        case MXDLNA_MSCP_X_UPDATERECORDSCHEDULE :
			{
				MXDLNA_MSCP_XSRS_RESPONSE_XUPDATERECORDSCHEDULE * pUpdateResp = (MXDLNA_MSCP_XSRS_RESPONSE_XUPDATERECORDSCHEDULE*)data;
				HxLOG_Print( " ID(%s) MediaRemainAlertID(%s) RecordDestinationID(%s)\n",
						pUpdateResp->ID, pUpdateResp->MediaRemainAlertID, pUpdateResp->RecordDestinationID);
			}
			break;

		case MXDLNA_MSCP_X_HDLNKGETRECORDDESTINATIONINFO :
			{
				MXDLNA_RECORDDEST_INFO * pRecordDest = (MXDLNA_RECORDDEST_INFO *)data;
				if(pRecordDest && pRecordDest->totalCapacity_bytes)
					HxLOG_Print( "[%s:%d] [Name]:%s [allowedTypes]:%s [Recordable]:%d [Percent]:%d [Size]: Avail-%.3fGB/ Total-%.3fGB\n\n", __MODULE__, __LINE__,
							pRecordDest->destName ,
							mxDlnaXSrs_GetRecordDestAllowedTypeAsString(pRecordDest->allowedTypes),
							pRecordDest->recordable,
							100-(HINT32)((pRecordDest->availableCapacity_bytes*100)/pRecordDest->totalCapacity_bytes),
							(float)pRecordDest->availableCapacity_bytes/(1024*1024*1024),
							(float)pRecordDest->totalCapacity_bytes/(1024*1024*1024)
							);
			}
			break;
#endif



	}

}

static void __notify( eRecordEvent event, HCHAR *udn,  HUINT32 p1, HUINT32 p2, HUINT32 p3)
{
	jp_context_t *ctx = __ctx();

	if(ctx->cb)
		ctx->cb(event, udn, p1, p2, p3);
}

static void __device_response_cb( HCHAR *pUdn, HINT32 action, HINT32 error, void *data)
{
    switch(action)
    {
#if 0
		/*  XSRS  */
        case MXDLNA_MSCP_X_CREATERECORDSCHEDULE :
            {
                MXDLNA_MSCP_XSRS_RESPONSE_XCREATERECORDSCHEDULE * pCreateObjResp = (MXDLNA_MSCP_XSRS_RESPONSE_XCREATERECORDSCHEDULE*)data;
                HxLOG_Print( "[ %s:%d ] DeviceResponseSink(%d) - X_CREATERECORDSCHEDULE \n", __MODULE__, __LINE__, error);

                if(error)
					__notify((HINT32) ePAL_DMP_ServiceEvent_Xsrs_CreateRecordSchedule, pUdn, 0, error, 0);
                else
                {
                    if(pCreateObjResp)
                    {
						HOM_MEMFREE(s_DmpXsrs_Context.pLastResponseSchedule.RecordScheduleID);
                        s_DmpXsrs_Context.pLastResponseSchedule.RecordScheduleID = HLIB_STD_StrDup(pCreateObjResp->RecordScheduleID);

                        __notify(eRecord_ScheduleCreate, pUdn, (HINT32)s_DmpXsrs_Context.pLastResponseSchedule.RecordScheduleID, 0, 0);
                    }
                }
            }
			break;

        case MXDLNA_MSCP_X_UPDATERECORDSCHEDULE :
            {
                MXDLNA_MSCP_XSRS_RESPONSE_XUPDATERECORDSCHEDULE * pUpdateResp = (MXDLNA_MSCP_XSRS_RESPONSE_XUPDATERECORDSCHEDULE*)data;
                HxLOG_Print( "[ %s:%d ] DeviceResponseSink(%d) - X_UPDATERECORDSCHEDULE", __MODULE__, __LINE__, error);

                if(error)
                    __notify((HINT32)ePAL_DMP_ServiceEvent_Xsrs_UpdateRecordSchedule, pUdn, 0, error, 0);
                else
                {
                    if(pUpdateResp)
                    {
						HOM_MEMFREE(s_DmpXsrs_Context.pLastResponseSchedule.ID);
                        s_DmpXsrs_Context.pLastResponseSchedule.ID = HLIB_STD_StrDup(pUpdateResp->ID);

                        __notify(eRecord_ScheduleUpdate, pUdn,  (HINT32)s_DmpXsrs_Context.pLastResponseSchedule.ID, 0, 0);
					}
				}
			}
			break;

		case MXDLNA_MSCP_X_DELETERECORDSCHEDULE :
			{
				HxLOG_Print( "[ %s:%d ] DeviceResponseSink(%d) - X_DELETERECORDSCHEDULE\n", __MODULE__, __LINE__, error);
				__notify(eRecord_ScheduleDelete, pUdn, 0, error, 0);
			}
			break;

			/* HDRL  */
		case MXDLNA_MSCP_X_HDLNKGETRECORDDESTINATIONS :
			{
				HINT32 index = 0;
				MXDLNA_RECORDDEST_INFO *pRecordDest = (MXDLNA_RECORDDEST_INFO *)data;
				PAL_DMP_HDRL_RecordDest_t *pRecDst = HLIB_STD_MemCalloc(sizeof(PAL_DMP_HDRL_RecordDest_t) * 5);

				HxLOG_Critical( "[ %s:%d ] DeviceResponseSink(%d) - X_HDLNKGETRECORDDESTINATIONS\n", __MODULE__, __LINE__, error);

				if(error != ERR_OK)
					__notify(eRecord_Destinations, pUdn, 0, 0, 0);
				else
				{
					while(pRecordDest != NULL)
					{
						HxSTD_snprintf( pRecDst[index].szName, 64 , "%s", pRecordDest->destName);
						HxSTD_snprintf( pRecDst[index].szId,  256 , "%s", pRecordDest->ID);
						index++;

						HxLOG_Print( "[%s:%d]   [%d][Name]:%s [ID]:%s\n" ,__MODULE__, __LINE__, \
								index, pRecordDest->destName , pRecordDest->ID );

						pRecordDest = pRecordDest->pNext;
					}

					__notifys(eRecord_Destinations, pUdn, (HINT32) pRecDst, index, 0);
				}
			}
			break;

		case MXDLNA_MSCP_X_HDLNKGETRECORDDESTINATIONINFO :
			{
				MXDLNA_RECORDDEST_INFO * pRecordDest = (MXDLNA_RECORDDEST_INFO *)data;
				PAL_DMP_HDRL_Storage_t *pDestInfo = NULL;

				HxLOG_Print( "[ %s:%d ] DeviceResponseSink(%d) - X_HDLNKGETRECORDDESTINATIONINFO - \n", __MODULE__, __LINE__, error);

				if(error != ERR_OK)
					__notify(eRecord_StorageInfo, pUdn, (HINT32) 0, 0, 0);
				else
				{
					pDestInfo = HLIB_STD_MemCalloc(sizeof(PAL_DMP_HDRL_Storage_t));
					if(pRecordDest != NULL)
					{
						pDestInfo->availableCapacity_bytes = (float)pRecordDest->availableCapacity_bytes;
						pDestInfo->totalCapacity_bytes = (float)pRecordDest->totalCapacity_bytes;
					}
					__notify(eRecord_StorageInfo, pUdn, (HINT32) pDestInfo, 1, 0);
				}
			}
			break;
#endif

		case MXDLNA_MSCP_CREATEOBJECT_SINK  :
			{
				MXDLNA_USER_CREATEOBJECT * pUserObj = (MXDLNA_USER_CREATEOBJECT *)data;
				object_t				 *pObj = NULL;
				local_upload_PrintCurrentTime();
				HxLOG_Critical( "[ %s:%d ] DeviceResponseSink(%d) - CREATEOBJECT_SINK \n", __MODULE__, __LINE__, error );

				if(pUserObj && pUserObj->ObjectData)
				{
					MXDLNA_CDSOBJECT * pObject = (MXDLNA_CDSOBJECT *)pUserObj->ObjectData;

					if((pObject->res != NULL)&& (pObject->res->ImportUri != NULL))
					{
						pObj = __obj_by_cds(pObject, pUdn);
						if(pObj == NULL)
						{
							HxLOG_Error( "[ %s:%d ] NOT found (%s) in Wait Objec.  \n", __MODULE__, __LINE__, pObject->Title);
							break;
						}

						if(error != ERR_OK)
						{
							HxLOG_Error( "[ %s:%d ] fail to get post url (error : %d).  \n", __MODULE__, __LINE__, error);
							__notify(eRecord_CreateObject,  pUdn, (HINT32)pObj, error, 0);
						}


						HxLOG_Print( "[ PAL_DMP ] ID=(%s), ParentID=(%s), Title(%s/%d)\n", pObject->ID,  pObject->ParentID, pObject->Title, pObject->TitleLength);
						HxLOG_Print( "[ PAL_DMP ] UploadUrl=(%s), ProtocolInfo(%s)\n" , pObject->res->ImportUri, pObject->res->ProtocolInfo);
						//HxLOG_Print( "[ PAL_DMP ] ReservationID =(%s)\n" , pObject->reservationID);

						if(pObj->url|| pObj->protocol)
							break;

						pObj->object_id = HLIB_STD_StrDup(pObject->ID);
						pObj->url= HLIB_STD_StrDup(pObject->res->ImportUri);
						pObj->protocol = HLIB_STD_StrDup(pObject->res->ProtocolInfo);
						pObj->status = object_Res_PostUrl;

						HxLOG_Print( "[ %s:%d ] ok ! get post url (%s).  \n", __MODULE__, __LINE__, pObject->res->ImportUri);
						__notify(eRecord_CreateObject,  pUdn, (HINT32)pObj, ERR_OK, 0 );

					}
				}
			}
			break;

#if 0
		case MXDLNA_MSCP_X_HDLNKGETRECORDCONTAINERID :
			{
				HCHAR *containerID = NULL;
				HxLOG_Print( "[ %s:%d ] DeviceResponseSink(%d) - X_HDLNKGETRECORDCONTAINERID - %s(%p) \n", \
					   	__MODULE__, __LINE__, error, containerID, data);

				if(data != NULL)
				{
					containerID = HLIB_STD_StrDup((HCHAR*)data);
					__insertContainerId(pUdn, containerID);
				}
			}
			break;

		case MXDLNA_MSCP_X_GETDLNAUPLOADPROFILES :
			{
				HCHAR *supportedUploadProfiles = HLIB_STD_StrDup((HCHAR*)data);
				HxLOG_Print( "[ %s:%d ] DeviceResponseSink(%d) - X_GETDLNAUPLOADPROFILES - \n", __MODULE__, __LINE__, error);
				__profile_matching(pUdn, supportedUploadProfiles);
			}
			break;
#endif

		case MXDLNA_MSCP_DESTROYOBJECT_SINK :
			HxLOG_Print( "[ %s:%d ] DeviceResponseSink(%d) - DESTROYOBJECT_SINK\n", __MODULE__, __LINE__, error );
			__notify(eRecord_DestroyObject,  pUdn, 0, error, 0);
			break;

		case MXDLNA_MSCP_IMPORTRESOURCE_SINK:
			HxLOG_Print( "[ %s:%d ] DeviceResponseSink(%d) - IMPORTRESOURCE_SINK\n", __MODULE__, __LINE__, error );
			break;

		case MXDLNA_MSCP_EXPORTRESOURCE_SINK:
			HxLOG_Print( "[ %s:%d ] DeviceResponseSink(%d) - EXPORTRESOURCE_SINK\n", __MODULE__, __LINE__, error );
			break;

		case MXDLNA_MSCP_TRANSFERPROGRESS_SINK:
			HxLOG_Print( "[ %s:%d ] DeviceResponseSink(%d) - TRANSFERPROGRESS_SINK\n", __MODULE__, __LINE__, error );
			break;

		case MXDLNA_MSCP_MOVEOBJECT_SINK:
			HxLOG_Print( "[ %s:%d ] DeviceResponseSink(%d) - MOVEOBJECT_SINK\n", __MODULE__, __LINE__, error );
			break;

		case MXDLNA_MSCP_UPDATEOBJECT_SINK:
			HxLOG_Print( "[ %s:%d ] DeviceResponseSink(%d) - UPDATEOBJECT_SINK\n", __MODULE__, __LINE__, error );
			break;

		case MXDLNA_MSCP_STOP_TRANSFER_RESOURCE_SINK:
			HxLOG_Print( "[ %s:%d ] DeviceResponseSink(%d) - STOP_TRANSFER_RESOURCE_SINK\n", __MODULE__, __LINE__, error );
			break;

		default:
			break;
	}
}

static HERROR	__create_iobject(object_t *pInArgs)
{
	HINT32 nRet = 0;
	MSCP_ACTION_HANDLE 	mscpa_handle = (MSCP_ACTION_HANDLE)PL_DMP_GetMSCPAHandle();
#if 0
	MXDLNA_MSCP_XSRS_ARGS_CREATEOBJECT CreateObjArgs;

	HxSTD_MemSet((void*)&CreateObjArgs,0,sizeof(MXDLNA_MSCP_XSRS_ARGS_CREATEOBJECT));
	HxSTD_MemCopy(&CreateObjArgs, &pInArgs->MxObj, sizeof(MXDLNA_MSCP_XSRS_ARGS_CREATEOBJECT));

	if(mscpa_handle)
	{
		nRet = mxDlnaMscpXSrs_createObject(mscpa_handle, pInArgs->pUdn, &CreateObjArgs);
		if(nRet == MSCP_HDRLACTION_OK)
		{
			pInArgs->status = Upload_Object_Req_PostUrl;
			return ERR_OK;
		}
		else
		{
			return ERR_FAIL;
		}
	}

	LEAVE_FUNC;
#endif

	return ERR_FAIL;
}

static HERROR	__get_profile(object_t *pObj)
{
	HINT32	 nRet = 0;
	HCHAR 	*pInFilterProfiles = NULL; /* 추후 Codec 정보 추가시 필터 추가 ! */
	MSCP_ACTION_HANDLE mscpa_handle = (MSCP_ACTION_HANDLE)PAL_DMP_GetMSCPAHandle();

	pObj->status = object_Req_DmsInfo;

	if(mscpa_handle)
	{
#if 0
		if(mxDlnaMscp_HDRL_X_GetDLNAUploadProfiles(mscpa_handle,pObj->pUdn,pInFilterProfiles))
			return ERR_FAIL;
#endif
	}

	return ERR_OK;
}

static HERROR __get_container_id(object_t *pObj)
{
#if 0
	HINT32 nRet = 0;
	MXDLNA_MSCP_ARGS_HDRLGETRECORDCONTAINER InArgs;
	MSCP_ACTION_HANDLE mscpa_handle = (MSCP_ACTION_HANDLE)PAL_DMP_GetMSCPAHandle();

	HxSTD_MemSet((void*)&InArgs,0,sizeof(MXDLNA_MSCP_ARGS_HDRLGETRECORDCONTAINER));

	InArgs.date = pObj->MxObj.Date;
	InArgs.title = pObj->MxObj.Title;
	InArgs.aribObjectType = pObj->MxObj.Arib_ObjectType;
	InArgs.protocolInfo = "*:*:application/x-dtcp1;CONTENTFORMAT=\"video/vnd.dlna.mpeg-tts\":DLNA.ORG_PN=DTCP_MPEG_TS_SD_JP_MPEG1_L2_T;DLNA.ORG_CI=0";
	InArgs.upnpClass = "object.item.videoItem";

	HxLOG_Print("[DmpHdrl_GetRecordContainerID] %s %s\n", pObj->pUdn, pObj->pDestinationId);

	pObj->status = Upload_Object_Req_DmsInfo;

	if( mscpa_handle && pObj->pDestinationId)
	{
		HxLOG_Print( "[%s:%d] will be sent request packet for container id.  \n", __MODULE__, __LINE__);
		nRet = mxDlnaMscp_HDRL_X_HDLnkGetRecordContainerID(mscpa_handle, pObj->pUdn, pObj->pDestinationId, &InArgs);
		if(nRet == MSCP_HDRLACTION_OK)
		{
			HxLOG_Print( "[%s:%d] Sent request packet for container id.  \n", __MODULE__, __LINE__);
			return ERR_OK;
		}
	}
#endif
	return ERR_FAIL;
}

static inline void __send_cmd(object_t *obj, HUINT32 cmd)
{
	jp_upload_cmd_t		msg;
	jp_context_t		*ctx = __ctx();

	HxSTD_MemSet(&msg, 0x00, sizeof(jp_upload_cmd_t));

	msg.cmd	= cmd;
	msg.obj	= obj;

	VK_MSG_Send(ctx->msg, &msg, sizeof(jp_upload_cmd_t));
}


#define ________________Public_Functions___________________________
static void	__cmd_task(void *pMsg)
{
	jp_context_t		*ctx = __ctx();
	jp_upload_cmd_t		msg = { 0, };
	object_t 			*obj = NULL;

	while (1)
	{
		if(VK_MSG_ReceiveTimeout(stContext.msg, &msg, sizeof(jp_upload_cmd_t), 300))
			continue;

		//__cmd_lock();

		HxLOG_Print("[%s:%d] received message (cmd : %d) .\n", __MODULE__, __LINE__, msg.cmd);

		obj = msg.obj;
		if(obj == NULL) continue;

		switch(msg.cmd)
		{
			case Add_Object:
			case Add_ObjectRetry:
				{
					switch(obj->status)
					{
						case object_Ready:
							obj->status != object_Req_DmsInfo;
#if 0
							HxLOG_Print("[%s:%d] Upload Object(%s) is created. will be request more info(container/profile) before getting posturl.\n", \
									__MODULE__, __LINE__, pObj->MxObj.Title);
#endif

						case object_Req_DmsInfo:
#if 0
							if(pObj->MxObj.ContainerID == NULL)
								__get_container_id((DmpUpload_Object_t *)msg.handle);

							if(pObj->bIsProfileMatch == FALSE)
								__get_profile((DmpUpload_Object_t *)msg.handle);
#endif

							break;

						case object_Res_DmsInfo:
#if 0
							HxLOG_Print("[%s:%d] container id is received. object(%s) will be request for post url to dms.\n", __MODULE__, __LINE__, pObj->MxObj.Title);
							if(__create_iobject((DmpUpload_Object_t *)msg.handle) != ERR_OK)
							{
								HxLOG_Print("[%s:%d] CrreateObject Failed.\n", __MODULE__, __LINE__);
							}
#endif
							break;

						case object_Req_PostUrl:
							break;

						case object_Res_PostUrl:
							//local_upload_prepare_upload((DmpUpload_Object_t *)msg.handle);
							break;

						default:
							//HxLOG_Error("[%s:%d] obj(%s) Unknown status(%d) .\n", __MODULE__, __LINE__, pObj->MxObj.Title, obj->status);
						   	break;
					}
				}
				break;

			case Del_Object:
				{
					HxLOG_Print("[%s:%d] Object Del Object Cmd !!!!! \n", __MODULE__, __LINE__);
#if 0
					if(pObj->is_working == TRUE)
					{
						HxLOG_Print("[%s:%d] object is uploading. will be stoped. \n", __MODULE__, __LINE__);
						__stop_object((DmpUpload_Object_t *)msg.handle);
					}
					else
					{
						HxLOG_Print("[%s:%d] object is wait object. will be destroy. \n", __MODULE__, __LINE__);
						__del_object((DmpUpload_Object_t *)msg.handle);
					}
#endif
				}
				break;

			case Send_StartPacket:
	//			local_upload_prepare_upload((DmpUpload_Object_t *)msg.handle);
				break;

			case Send_UploadStart:
				{
					HxLOG_Print("[%s:%d] Object Send File Cmd !!!!! \n", __MODULE__, __LINE__);

#if 0
					if( pObj->status != Upload_object_Start || pObj->buffer == NULL )
					{
						HxLOG_Print(" [%s:%d] Object is not ready status. \n", __MODULE__, __LINE__);
					}

					if( __is_sender_running()== FALSE)
						__start_worker();

					__add_job(pObj);

					if((s_Upload_cbRequestResult != NULL) && (pObj->eType == PAL_DMP_UPLOAD_RECORDING))
					{
						s_Upload_cbRequestResult(ePAL_DMP_ServiceEvent_Upload_Started, pObj->pUdn, (HINT32)pObj, 0, 0);
					}

					HxLOG_Print("[%s:%d] Object Send to worker thread!!!!! \n", __MODULE__, __LINE__);
#endif
				}
				break;
			case Send_UploadStop:
				/* TODO */
				break;

			case Send_EndPacket:
				//local_upload_terminate((DmpUpload_Object_t *)msg.handle);
				break;

			case Send_CleanUp:
				//local_upload_cleanup((DmpUpload_Object_t *)msg.handle, ERR_FAIL);
				break;


			default:
				break;
		}

		//__cmd_unlock();

		VK_TASK_Sleep(100);
	}
}

HERROR	PL_DMP_JP_Init(HBOOL upload)
{
	PL_DMP_MakeUserDeviceCaps("X_JLABSCAP", "urn:schemas-jlabs-or-jp:device-1-0", "jlabs");
	PL_DMP_MakeUserDeviceCaps("X_JLABSCAP", "urn:schemas-jlabs-or.jp:device-1-0", "jlabs"); /* Sharp DMS */
	PL_DMP_RegisterUserDeviceEventResponseSink(__device_response_cb);

	mxDlnaMscp_setUserCommandAtBrowseSOAPHeader("X-PARENTALLOCK:");
	HxSTD_MemSet(&stContext, 0x00, sizeof(jp_context_t));

	if(upload)
	{
		if(VK_MSG_Create(128, sizeof(jp_upload_cmd_t), "jp_dmp_msg",  &stContext.msg, VK_SUSPENDTYPE_FIFO))
		{
			HxLOG_Error( "[ %s:%d ] fail create dmp message queue for japan.\n", __MODULE__, __LINE__);
			return ERR_FAIL;
		}

		if(VK_TASK_Create(__cmd_task, 40, SIZE_64K, "jp_dmp_task", NULL, &stContext.task, 0))
		{
			HxLOG_Error( "[ %s:%d ] cann't create upload command tesk. \n", __MODULE__, __LINE__);
			return ERR_FAIL;
		}

		if(VK_SEM_Create(&stContext.lock, "jp_dmp_sema", VK_SUSPENDTYPE_PRIORITY))
		{
			HxLOG_Error( "[ %s:%d ] cann't create upload command semaphone.\n", __MODULE__, __LINE__);
			return ERR_FAIL;
		}
	}

	return ERR_OK;
}

HBOOL   PL_DMP_JP_IsSupportedHDRL(HCHAR * pInUDN)
{
    HINT32 bSupportedHDRL = 0;
    MXDLNA_MSCP_OBJECTHANDLE mscp_handle = (MXDLNA_MSCP_OBJECTHANDLE) PL_DMP_GetMSCPHandle();

    if(mscp_handle)
        bSupportedHDRL = mxDlnaMscp_isHDRLMediaServer(mscp_handle, pInUDN);

	return bSupportedHDRL? TRUE:FALSE;
}

HERROR  PL_DMP_JP_GetRecordDestinations(HCHAR * pInUDN)
{
    HINT32 nRet = 0;
    MSCP_ACTION_HANDLE mscpa_handle = (MSCP_ACTION_HANDLE)PL_DMP_GetMSCPAHandle();

    if(mscpa_handle)
        nRet = mxDlnaMscp_HDRL_X_HDLnkGetRecordDestinations(mscpa_handle,pInUDN);

	return nRet? ERR_FAIL:ERR_OK;
}

HERROR  PL_DMP_JP_GetRecordDestinationInfo(HCHAR * pInUDN, HCHAR * pInRecordDestID)
{
    HINT32 nRet = 0;
    MSCP_ACTION_HANDLE mscpa_handle = (MSCP_ACTION_HANDLE)PL_DMP_GetMSCPAHandle();

    if(mscpa_handle && pInRecordDestID)
        nRet = mxDlnaMscp_HDRL_X_HDLnkGetRecordDestinationInfo(mscpa_handle,pInUDN,pInRecordDestID);

	return nRet? ERR_FAIL:ERR_OK;
}

static object_t *__new_obj(void)
{
	object_t	*p = HLIB_STD_MemCalloc(sizeof(object_t));

	if(p)  	return p;

	HxLOG_Debug("[%s:%d] not enough memcpy. \n", __FUNCTION__, __LINE__);

	return NULL;
}

static object_t *__del_obj(void)
{

	return NULL;
}
static object_t * __find_obj(object_t *obj)
{
	jp_context_t		*ctx = __ctx();
	object_t 			*p = NULL;
	HxList_t			*list = NULL;

	list = HLIB_LIST_Find(HLIB_LIST_First(ctx->obj_list), obj);
	p = HLIB_LIST_Data(list);

	if(p)  	return p;

	HxLOG_Debug("[%s:%d] object is not existed. \n", __FUNCTION__, __LINE__);

	return NULL;
}

static HUINT32	__obj_count(void)
{
	jp_context_t	*ctx = __ctx();
	return HLIB_LIST_Length(HLIB_LIST_First(ctx->obj_list));
}

static object_t * __obj_by_cds_rsvid(HCHAR *rsv_id, HCHAR *pUdn)
{

	return NULL;
}

static object_t * __obj_by_cds_title(MXDLNA_CDSOBJECT *pObject, HCHAR *pUdn)
{
	jp_context_t	*ctx = __ctx();
	HUINT32 		i, cnt = __obj_count();

	object_t		*obj = NULL;
	HxList_t		*list = NULL;

	if(pObject == NULL || pUdn == NULL)
		return NULL;

	if(cnt == 0) return NULL;

	for(i=0; i<cnt; i++)
	{
		list = HLIB_LIST_GetListItem(HLIB_LIST_First(ctx->obj_list), i);
		obj = HLIB_LIST_Data(list);

		if(obj != NULL )
		{
#if 0
			if(!strncmp(obj->MxObj.Title, pObject->Title, pObject->TitleLength) && !HxSTD_StrCmp(obj->pUdn, pUdn) )
			{
				//HxLOG_Print( "[ %s:%d ] found (%s) in Wait Objec.  \n", __MODULE__, __LINE__, pObject->Title);

				if(obj->pUrl || obj->pProtocolInfo)
				{
#if 0
					HxLOG_Error( "[ %s:%d ] SAME EVENT !!!! fail to get post url.  \n", __MODULE__, __LINE__);
					HxLOG_Error( "[ %s:%d ] URI : %s  \n", __MODULE__, __LINE__, obj->pUrl);
					HxLOG_Error( "[ %s:%d ] ProtocolInfo : %s  \n", __MODULE__, __LINE__, obj->pProtocolInfo);
#endif
					continue;
				}
				else
					return obj;
			}
#endif
		}
	}

	return NULL;
}



HERROR PL_DMP_JP_CreateObject(HINT32 *pHandle)
{
	object_t	*p = __new_obj();

	if(p)
	{
		p->status = object_Create;
		*pHandle = (HINT32) p;
		return ERR_OK;
	}

	return ERR_FAIL;
}

#if 0
HERROR PL_DMP_JP_SetObjectInfo(PAL_DMP_XSRS_RecordObject *pInArgs, void *pHandle )
{
	DmpUpload_Object_t	*pObj = NULL;
	HxDATETIME_t 		startDateTime;
	HCHAR				*pProtocol_codec = NULL;
	HCHAR 				szTime[40];

	ENTER_FUNC;

	PAL_DMP_ASSERT(pInArgs);
	PAL_DMP_ASSERT(pHandle);

	if(pInArgs == NULL || pInArgs->Udn == NULL || pInArgs->DestinationId == NULL)
	{
		HxLOG_Error("[%s:%d] Create Object Param ERROR !!!! \n", __MODULE__, __LINE__);
	}

	if(pInArgs->bUseDubbing)
	{
#if 0
		/* 같은 파일은 동시에 업로드가 불가 - 시나리오 협의 후 결정 */
		if(__object_by_path(pInArgs->Path) != NULL)
		{
			HxLOG_Error("[%s:%d] tiel is same in work list. \n", __MODULE__, __LINE__);
			return ERR_FAIL;
		}
#endif

		/* 같은 DMS 에 같은 Title 로 전달할 수 없음. DMS Respone 를 식별할수 없음 */
		if(__object_by_title(pInArgs->Title, pInArgs->Udn) != NULL)
		{
			HxLOG_Error("[%s:%d] title is same in work list. \n", __MODULE__, __LINE__);
			return ERR_FAIL;
		}
	}

	pObj = __object_by_handle(pHandle);
	if(pObj == NULL)
	{
		HxLOG_Error( "[ %s:%d ] this object(%p) is not existed  \n", __MODULE__, __LINE__, pObj );
		return ERR_FAIL;
	}

	if(pInArgs != NULL)
	{
		pObj->pUdn = HLIB_STD_StrDup(pInArgs->Udn);
		pObj->pDestinationId = HLIB_STD_StrDup(pInArgs->DestinationId);

		if(pInArgs->bUseDubbing == TRUE)
		{
			pObj->hRingBuf = 0;
			pObj->pPath = HLIB_STD_StrDup(pInArgs->Path);
			pObj->eType = PAL_DMP_UPLOAD_DUBBING;

			pObj->content_id = pInArgs->ulContentsId;
			HxLOG_Print("[%s:%d] PAL_DMP_UPLOAD_DUBBING\n", __MODULE__, __LINE__);
		}
		else
		{
			pObj->hRingBuf = 0;
			pObj->pPath = NULL;
			pObj->eType = PAL_DMP_UPLOAD_RECORDING;
			pObj->content_id = -1;
			HxLOG_Print("[%s:%d] PAL_DMP_UPLOAD_RECORDING\n", __MODULE__, __LINE__);
		}

		/* Panasonic 장비와의 호환성을 위해 변경함. */
		pObj->MxObj.ContainerID = HLIB_STD_StrDup("DLNA.ORG_AnyContainer");

		pObj->MxObj.Restricted = pInArgs->Restricted;
		pObj->MxObj.Title = HLIB_STD_StrDup(pInArgs->Title);
		pObj->MxObj.Genre = HLIB_STD_StrDup(pInArgs->Genre);
		pObj->MxObj.ChannelName = HLIB_STD_StrDup(pInArgs->ChannelName);
		pObj->MxObj.ChannelNr = pInArgs->ChannelNr;
		if(!HxSTD_StrEmpty(pInArgs->ReservationID))
		{
			pObj->MxObj.XSrs_ReservationID = HLIB_STD_StrDup(pInArgs->ReservationID);
			HxLOG_Print("[%s:%d] ReservationID = %s \n", __MODULE__, __LINE__, pInArgs->ReservationID);
		}
		else
			pObj->MxObj.XSrs_ReservationID = NULL;
		pObj->MxObj.mediaClass = MXDLNA_MSCP_XSRS_MEDIATYPE_VIDEOITEM;

		switch(pInArgs->eCodec)
		{
			case PAL_DMP_HDRL_MPEG2_TS: pProtocol_codec = PROFILE_MPEG2_TS_JP; break;
			case PAL_DMP_HDRL_H264: pProtocol_codec = PROFILE_H264; break;
		}

		if(pInArgs->ProtocolInfo != NULL)
			pObj->MxObj.ProtocolInfo = pInArgs->ProtocolInfo;
		else
		{
			pObj->MxObj.ProtocolInfo = HLIB_STD_MemAlloc(512);
			HxSTD_MemSet(pObj->MxObj.ProtocolInfo, 0x00, 512);

			/* Panasonic 장비와의 호환성을 위해 변경함 Panasonic 장비는 http-get:*:으로 전성시 인식하지 못함 */
#if 0
			HxSTD_snprintf(pObj->MxObj.ProtocolInfo, 512, \
				"http-get:*:application/x-dtcp1;DTCP1HOST=192.168.11.2;DTCP1PORT=9998;CONTENTFORMAT=\"video/vnd.dlna.mpeg-tts\":DLNA.ORG_PN=%s;DLNA.ORG_CI=0", pProtocol_codec );
#else
			HxSTD_snprintf(pObj->MxObj.ProtocolInfo, 512, "*:*:application/x-dtcp1;DTCP1HOST=%s;DTCP1PORT=9998;CONTENTFORMAT=\"video/vnd.dlna.mpeg-tts\":DLNA.ORG_PN=%s;DLNA.ORG_CI=0", pInArgs->Ip, pProtocol_codec);
#endif
		}

		if(!HxSTD_StrEmpty(pInArgs->Rating))
			pObj->MxObj.Rating = HLIB_STD_StrDup(pInArgs->Rating);

		pObj->MxObj.MoreInfo.bUseDubbing = pInArgs->bUseDubbing;
		pObj->MxObj.MoreInfo.duration = pInArgs->ullDuration;
		pObj->MxObj.MoreInfo.clearTextSize = 20480000 ;

		HxSTD_MemSet(&startDateTime, 0x00, sizeof(HxDATETIME_t));
		HxSTD_MemSet(&szTime, 0x00, 40);

		HLIB_DATETIME_ConvertUnixTimeToDateTime (pInArgs->Date, &startDateTime);

		HxSTD_snprintf( szTime, sizeof( szTime),\
				"%.4d-%.2d-%.2dT%.2d:%.2d:%.2dZ",
				startDateTime.stDate.usYear, startDateTime.stDate.ucMonth, startDateTime.stDate.ucDay,
				startDateTime.stTime.ucHour, startDateTime.stTime.ucMinute, startDateTime.stTime.ucSecond);
		pObj->MxObj.Date = HLIB_STD_StrDup(szTime);

		switch(pInArgs->eObjectType)
		{
			case PAL_DMP_XSRS_ARIB_TB: pObj->MxObj.Arib_ObjectType = HLIB_STD_StrDup("ARIB_TB") ; break;
			case PAL_DMP_XSRS_ARIB_BS: pObj->MxObj.Arib_ObjectType = HLIB_STD_StrDup("ARIB_BS") ; break;
			case PAL_DMP_XSRS_ARIB_CS: pObj->MxObj.Arib_ObjectType = HLIB_STD_StrDup("ARIB_CS") ; break;
			case PAL_DMP_XSRS_CATV_CB: pObj->MxObj.Arib_ObjectType = HLIB_STD_StrDup("CATV_CB") ; break;
			case PAL_DMP_XSRS_CATV_CS: pObj->MxObj.Arib_ObjectType = HLIB_STD_StrDup("CATV_CS") ; break;
			case PAL_DMP_XSRS_CATV_JC: pObj->MxObj.Arib_ObjectType = HLIB_STD_StrDup("CATV_JC") ; break;
		}

		switch(pInArgs->eCodec)
		{
			case PAL_DMP_HDRL_MPEG2_TS: pObj->pProfile = HLIB_STD_StrDup(PROFILE_MPEG2_TS_JP); break;
			case PAL_DMP_HDRL_H264: 	pObj->pProfile = HLIB_STD_StrDup(PROFILE_H264); break;
		}
	}

	pObj->status = object_Ready;

	__send_cmd((HUINT32)pObj, Add_Object);

	HxLOG_Print("[%s:%d] Object(%p) is created. \n", __MODULE__, __LINE__, pObj);

	LEAVE_FUNC;

	return ERR_OK;
}
#endif

HERROR PL_DMP_UPLOAD_CreateObjectRetry(void *handle)
{
	object_t *p = NULL;
	PL_DMP_CHECK_OBJ(handle);

	p = __find_obj((object_t *)handle);

	if(p->status == object_Req_PostUrl)
		p->status = object_Res_DmsInfo;
	else
	{
#if 0
		HOM_MEMFREE(p->MxObj.ContainerID)
		p->MxObj.ContainerID = NULL;
		p->bIsProfileMatch = FALSE;
#endif
		p->status = object_Ready;
	}

	__send_cmd(p, Add_ObjectRetry);

	return ERR_OK;
}

HERROR PL_DMP_UPLOAD_DestroyObject(void *handle)
{
	object_t *p = NULL;
	PL_DMP_CHECK_OBJ(handle);

	p = __find_obj((object_t *)handle);
	HxLOG_Print("[%s:%d] Cann't find upload object. this is already deleted object or not created object  \n", __MODULE__, __LINE__);

	/* 전송 완료 후, EndPacket 사이에 Destroy 가 되면?? 어떻게 처리 하나.. !!
	 * 지울 수 없다. 이미 보낸 파일은 삭제 가능한 Dms 인 경우 별처 처리 해야함. */

	__send_cmd(p, Del_Object);

	return ERR_OK;
}

/* 다른 방안이 필요 */
HERROR PL_DMP_UPLOAD_CancelByHdd(HCHAR *pszMountPath)
{
	jp_context_t 	*ctx = __ctx();
	HINT32			cnt = __obj_count();
	HINT32			index = 0;

	object_t		*obj = NULL;
	HxList_t		*list = NULL;

	HxLOG_Print("[%s:%d] HDD is detachad. Uploading uploading will be canceled \n", __MODULE__, __LINE__);

	if(cnt == 0)
		return ERR_FAIL;

	if(cnt)
	{
		for(index = 0; index<cnt; index++)
		{
			list = HLIB_LIST_GetListItem(HLIB_LIST_First(ctx->obj_list), index);
			obj = HLIB_LIST_Data(list);
#if 0
			if(pObj->eType == PAL_DMP_UPLOAD_DUBBING && HxSTD_StrStr(pObj->pPath, pszMountPath) != NULL)
				__send_cmd((HUINT32)pObj, Cmd_Del_Object);
#endif
		}
	}

	return ERR_OK;
}

HERROR PL_DMP_UPLOAD_SendStartPacket(void *handle)
{
	object_t *p = NULL;
	PL_DMP_CHECK_OBJ(handle);

	p = __find_obj((object_t *)handle);
	__send_cmd(p, Send_StartPacket);

	return ERR_OK;
}

HERROR PL_DMP_UPLOAD_Start(void *handle)
{
	object_t *p = NULL;
	PL_DMP_CHECK_OBJ(handle);

	p = __find_obj((object_t *)handle);
	__send_cmd(p, Send_UploadStart);

	return ERR_OK;
}


HERROR PL_DMP_UPLOAD_Stop(void *handle)
{
	object_t *p = NULL;
	PL_DMP_CHECK_OBJ(handle);

	p = __find_obj((object_t *)handle);
	__send_cmd(p, Send_UploadStop);

	return ERR_OK;
}

HERROR PL_DMP_UPLOAD_SetSource(void *handle, Handle_t hSrc)
{
	object_t *p = NULL;
	PL_DMP_CHECK_OBJ(handle);

	//__cmd_lock();

	p = __find_obj((object_t *)handle);
   	p->hRingBuf = hSrc;

	return ERR_OK;

	//__cmd_unlock();
}

HERROR	PL_DMP_JP_RemoveCDS(HCHAR *udn, HCHAR *obj)
{
	MSCP_ACTION_HANDLE mscpa_handle = (MSCP_ACTION_HANDLE)PAL_DMP_GetMSCPAHandle();

	if(mscpa_handle == NULL)
		return ERR_FAIL;
	else
	{
		mxDlnaMscp_setUDN(mscpa_handle, udn);
		mxDlnaMscp_setCMID(mscpa_handle, 0);
	}

	mxDlnaMscp_destroyObject(mscpa_handle, obj);

	return ERR_OK;
}

#if 0
HERROR	PL_DMP_HDRL_GetRecordStatus(DMP_HDRL_Status_t *pStatus)
{
	DmpUpload_Object_t 	*pObj;
	HINT32				i;

	PAL_DMP_ASSERT(pStatus);

	pStatus->bIsUploading = FALSE;
	pStatus->bIsRecording = FALSE;
	pStatus->bIsDubbing = FALSE;

	__sender_lock();
	pStatus->nUploadingObjectCount = s_WorkingObjects.nWorkCount;
	for(i=0; i<pStatus->nUploadingObjectCount; i++)
	{
		pObj = s_WorkingObjects.WorkingObject[i];
		pStatus->bIsUploading = TRUE;

		if(pObj->eType == PAL_DMP_UPLOAD_DUBBING )
			pStatus->bIsDubbing = TRUE;
		else
			pStatus->bIsRecording = TRUE;
	}
	__sender_unlock();

	return ERR_OK;
}
#endif

/* end od file */
