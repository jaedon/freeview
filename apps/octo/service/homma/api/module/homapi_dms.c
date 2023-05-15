/**************************************************************
*	@file	homapi_dms.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			DLNA / DMS
**************************************************************/

/*
* ??2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/


#define	________________Header_Files_______________________________
#include	<homapi.h>

#define	________________Define_____________________________________
#define CHECK_DMS_MODULE_INIT	do { HxLOG_Warning("\n"); if(dlnadms_IsInitialized() == FALSE) HOMAPI_DMS_Init(); } while (0)


#define ________________Typedef____________________________________
typedef struct
{
	HBOOL		bInit;

	HUINT32		accesscode;
	HUINT32		notifierId;

	HOMAPI_EventCallback cb;
} dms_context_t;

#define ________________Static_Variables___________________________
static  HUINT32     _ulModuleSemaphoreId = 0;

#define ________________Internal_Functions_________________________
static dms_context_t * __ctx( void )
{
	static dms_context_t s_manager;
	return &s_manager;
}

typedef struct
{
	HINT32						nCount;
	HOMxDMS_LiveStreamInfo_t 	*pList;
} dms_livestreaminfo_list_t;

static HBOOL	dlnadms_IsInitialized(void)
{
	dms_context_t 	*mgr = __ctx();

	if(mgr == NULL)
	{
		HxLOG_Error("mgr == NULL\n");
		return FALSE;
	}

	return mgr->bInit;
}

static HBOOL    __dms_querydata_decoder (HUINT32 cmd, void *data, HUINT32 size, void *pData)
{
	switch(cmd)
	{
		case eHOMAPI_DMS_Command_GetLiveStreamInfo:
			{
				dms_livestreaminfo_list_t *pResultData = (dms_livestreaminfo_list_t *)pData;
				HINT32 nCount = 0;
				HCHAR *pRecv = data;

				if(pResultData != NULL)
				{
					pResultData->pList = NULL;

					if(pRecv != NULL)
					{
						HxSTD_MemCopy( &nCount, pRecv, sizeof(HINT32));
						HxLOG_Debug("### Get Live Stream info (%d) ### \n", nCount);

						if ( nCount > 0)
						{
							pResultData->pList = HLIB_STD_MemCalloc(sizeof(HOMxDMS_LiveStreamInfo_t) * nCount);
							HxSTD_MemCopy( pResultData->pList, pRecv+sizeof(HINT32),
												sizeof(HOMxDMS_LiveStreamInfo_t) * nCount );
						}
					}

					pResultData->nCount = nCount;
				}
			}
			break;

		default:
			HxLOG_Error("Error> unsupported command(%d)\n", cmd);
			break;
	}

	return TRUE;
}

static HERROR 	dlnadms_DecodeResultEvent ( HUINT32	cmd, void *data, HUINT32 size)
{
	dms_context_t 	*mgr = __ctx();
	HERROR			hErr = ERR_FAIL;
	void			*pResultData = NULL;

	switch(cmd)
	{
		case eHOMAPI_DMS_Event_DmsInfo:
		case eHOMAPI_DMS_Event_DmsRunningState:
			{
				if(data != NULL && size != 0)
					pResultData = HLIB_STD_MemDup(data, size);
			}
			break;
	}

	if (mgr->cb)
		mgr->cb( (HUINT32)cmd, (HUINT32)pResultData, (HUINT32)1);

	if ( pResultData )
		HLIB_STD_MemFree( pResultData );

	return hErr;
}
static HERROR 	dlnadms_QueryResultEventHandler( HUINT32 cmd, void *param, HUINT32 size, void *userdata )
{
	return  dlnadms_DecodeResultEvent( cmd, param, size);
}

static	HUINT32	dlnadms_Notifier(HUINT32 accesscode, HUINT32 cmd, void *param, HUINT32 size)
{
	dms_context_t *mgr = __ctx();

	HOMAPI_SEM_GET;

	if ( NULL == param )
		HOMAPI_Query( eHOMAPI_Service_DMS, NULL, mgr, (HBOOL (*)(HUINT32, void *, HUINT32, void *))dlnadms_QueryResultEventHandler );
	else
		dlnadms_DecodeResultEvent(cmd, param, size);

	if ( mgr->accesscode != accesscode )
	{
		HxLOG_Debug("accesscode in mgr (%d) not equal to cur accesscode (%d) \n", mgr->accesscode, accesscode);
		HOMAPI_SEM_RELEASE;
	}

	HOMAPI_SEM_RELEASE;

	return 0;
}

static	HERROR dlnadms_RegisterNotifier( void )
{
	dms_context_t *mgr  =  __ctx();
	HUINT32 	accesscode = 0;

	if(mgr == NULL)
	{
		HxLOG_Error("mgr == NULL\n");
		return ERR_FAIL;
	}

	mgr->bInit = FALSE;
	accesscode = HOMAPI_Query( eHOMAPI_Service_DMS, NULL, NULL,  NULL );

	if( accesscode )
	{
		mgr->accesscode = accesscode;
		mgr->notifierId = HOMAPI_ConnectNotifier( accesscode, dlnadms_Notifier );
		mgr->bInit = TRUE;

		return ERR_OK;
	}

	return ERR_FAIL;
}

static	void	dlnadms_UnRegisterNotifier( void )
{
	dms_context_t *mgr  =  __ctx();

	if ( mgr )
		mgr->notifierId = HOMAPI_DisconnectNotifier( mgr->accesscode, mgr->notifierId );
}

#define ________________Public_Interfaces__________________________
void	HOMAPI_DMS_Init( void )
{
#if !defined(CONFIG_HOMMA_DMS)
	HxLOG_Error("### (HOMAPI_DMS_Init) Not Support DLNA DMS ###\n");
	return;
#else
	if(_ulModuleSemaphoreId == 0)
	{
		HxSEMT_Create(&_ulModuleSemaphoreId, "sadlnadms", HxSEMT_FIFO);
	}

	if(dlnadms_RegisterNotifier() != ERR_OK)
	{
		HxLOG_Error("[HOMAPI_DMS_Init] dlnadms_RegisterNotifier() fail!\n");
	}
#endif
}

void	HOMAPI_DMS_DeInit( void )
{
#if !defined(CONFIG_HOMMA_DMS)
	HxLOG_Error("### (HOMAPI_DMS_DeInit) Not Support DLNA DMS ###\n");
	return;
#else
	dlnadms_UnRegisterNotifier();

	if(_ulModuleSemaphoreId != 0)
	{
		HxSEMT_Destroy(_ulModuleSemaphoreId);
		_ulModuleSemaphoreId = 0;
	}
#endif
}

void	HOMAPI_DMS_RegisterNotifier( HOMAPI_EventCallback pfnObjectHandler )
{
#if !defined(CONFIG_HOMMA_DMS)
	HxLOG_Error("### (HOMAPI_DMS_RegisterNotifier) Not Support DLNA DMS ###\n");
	return;
#else
	dms_context_t 	*mgr  = __ctx();
	mgr->cb= pfnObjectHandler;
#endif
}

void	HOMAPI_DMS_UnRegisterNotifier( void )
{
#if !defined(CONFIG_HOMMA_DMS)
	HxLOG_Error("### (HOMAPI_DMS_UnRegisterNotifier) Not Support DLNA DMS ###\n");
	return;
#else
	dms_context_t 	*mgr = __ctx();
	mgr->cb= NULL;
#endif
}

HERROR	HOMAPI_DMS_Start( HCHAR *pFriendlyName, HBOOL bBootTimeStart )
{
#if !defined(CONFIG_HOMMA_DMS)
	HxLOG_Error("### (HOMAPI_DMS_Start) Not Support DLNA DMS ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t 			data;
	HOMxDMS_Args_Start_t	arg;

	CHECK_DMS_MODULE_INIT;

	HxSTD_MemSet(&arg, 0, sizeof(HOMxDMS_Args_Start_t));

	HxSTD_snprintf( arg.szFriendlyName, sizeof( arg.szFriendlyName ), "%s", pFriendlyName );
	arg.bBootTimeStart = bBootTimeStart;

	data.cmd = eHOMAPI_DMS_Command_Start;
	data.data = (void *)&arg;
	data.size = sizeof(HOMxDMS_Args_Start_t);

	HOMAPI_Call( eHOMAPI_Service_DMS, &data);

	return ERR_OK;
#endif
}

HERROR	HOMAPI_DMS_Stop( void )
{
#if !defined(CONFIG_HOMMA_DMS)
	HxLOG_Error("### (HOMAPI_DMS_Stop) Not Support DLNA DMS ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;

	CHECK_DMS_MODULE_INIT;

	data.cmd = eHOMAPI_DMS_Command_Stop;
	data.data = NULL;
	data.size = 0;

	HOMAPI_Call( eHOMAPI_Service_DMS, &data);

	return ERR_OK;
#endif
}

HERROR	HOMAPI_DMS_SetFriendlyName( const HCHAR *pFriendlyName )
{
#if !defined(CONFIG_HOMMA_DMS)
	HxLOG_Error("### (HOMAPI_DMS_SetFriendlyName) Not Support DLNA DMS ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;
	HOMxDMS_Args_Rename_t arg;

	CHECK_DMS_MODULE_INIT;

	if( pFriendlyName && pFriendlyName[0] != '\0')
	{
		HxSTD_MemSet(&arg, 0, sizeof(HOMxDMS_Args_Rename_t));
		HLIB_STD_StrUtf8NCpy( arg.szFriendlyName, pFriendlyName, sizeof(arg.szFriendlyName));

		data.cmd = eHOMAPI_DMS_Command_Rename;
		data.data = (void*)&arg;
		data.size = sizeof(HOMxDMS_Args_Rename_t);

		HOMAPI_Call( eHOMAPI_Service_DMS, &data);
		return ERR_OK;
	}

	HxLOG_Error("### (HOMAPI_DMS_SetFriendlyName) Error> DMS rename() failed ###\n");
	return ERR_FAIL;
#endif
}

HERROR	HOMAPI_DMS_GetLiveStreamInfo(HINT32 nSvcUid, HUINT32 *pCount, HOMxDMS_LiveStreamInfo_t **pResultData)
{
#if !defined(CONFIG_HOMMA_DMS)
	HxLOG_Error("### (HOMAPI_DMS_GetLiveStreamInfo) Not Support DLNA DMS ###\n");
	return ERR_FAIL;
#else
	HUINT32     accessCode = 0;
	dms_livestreaminfo_list_t tResult;
	HOMxCommData_t data;
	HOMxDMS_Args_LiveStreamInfo_t arg;

	CHECK_DMS_MODULE_INIT;

	if(pCount != NULL) {
		*pCount = 0;
	}

	if(pResultData != NULL) {
		*pResultData = NULL;
	}

	HxSTD_MemSet(&tResult, 0, sizeof(dms_livestreaminfo_list_t));
	HxSTD_MemSet(&arg, 0, sizeof(HOMxDMS_Args_LiveStreamInfo_t));
	HxSTD_MemSet(&data, 0, sizeof(HOMxCommData_t));

	arg.nSvcUid = nSvcUid;
	data.cmd = eHOMAPI_DMS_Command_GetLiveStreamInfo;
	data.data = (void*)&arg;
	data.size = sizeof(HOMxDMS_Args_LiveStreamInfo_t);

	HOMAPI_SEM_GET;
	accessCode = HOMAPI_Query( eHOMAPI_Service_DMS, (void *)&data , &tResult, __dms_querydata_decoder );

	if(!accessCode)
	{
		HxLOG_Error("### (getLiveStreamInfo) Error> Cant get DMS Live Stream Info ###\n");
		HOMAPI_SEM_RELEASE;
		return ERR_FAIL;
	}

	if(pResultData != NULL) {
		*pResultData = (HOMxDMS_LiveStreamInfo_t *)tResult.pList;
	}

	if(pCount != NULL) {
		*pCount = tResult.nCount;
	}
	HOMAPI_SEM_RELEASE;

	return ERR_OK;
#endif
}

HERROR HOMAPI_DMS_FreeLiveStreamInfo(HUINT32 nCount, HOMxDMS_LiveStreamInfo_t *pResultData)
{
#if !defined(CONFIG_HOMMA_DMS)
	HxLOG_Error("### (HOMAPI_DMS_FreeLiveStreamInfo) Not Support DLNA DMS ###\n");
	return ERR_FAIL;
#else
	if ( pResultData )
		HLIB_STD_MemFree( pResultData );

	return ERR_OK;
#endif
}
