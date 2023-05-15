/**************************************************************
*	@file	homapi_dmr.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			DLNA / DMR
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
#include 	<homapi.h>

#define	________________Define_____________________________________
#define CHECK_DMR_MODULE_INIT	do { HxLOG_Warning("\n"); if(__dmr_IsInitialized() == FALSE) HOMAPI_DMR_Init(); } while (0)

#define ________________Typedef____________________________________
typedef struct
{
	HBOOL		bInit;

	HUINT32		accesscode;
	HUINT32		notifierId;

	HOMAPI_EventCallback	cb;
} dmr_context_t;

#define ________________Static_Variables___________________________
static  HUINT32     _ulModuleSemaphoreId = 0;

#define ________________Internal_Functions_________________________
static dmr_context_t *__ctx( void )
{
	static dmr_context_t s_manager;
	return &s_manager;
}

static HBOOL	__dmr_IsInitialized(void)
{
	dmr_context_t 	*mgr = __ctx();

	if(mgr == NULL)
	{
		HxLOG_Error("mgr == NULL\n");
		return FALSE;
	}

	return mgr->bInit;
}

static HERROR 	__decode_result_event( HUINT32	cmd, void *data, HUINT32 size)
{
	dmr_context_t 	*mgr = __ctx();
	HERROR			hErr	= ERR_FAIL;
	void			*pResultData = NULL;

	switch(cmd)
	{
		case eHOMAPI_DMR_Event_SetUri:
		case eHOMAPI_DMR_Event_Play:
		case eHOMAPI_DMR_Event_Stop:
		case eHOMAPI_DMR_Event_Pause:
		case eHOMAPI_DMR_Event_Seek:
		case eHOMAPI_DMR_Event_Volume:
		case eHOMAPI_DMR_Event_Mute:
		case eHOMAPI_DMR_Event_BringIn:
			{
				if(data != NULL)
					pResultData = HLIB_STD_MemDup(data, size);
			}
			break;
	}

	if ( mgr->cb )
			mgr->cb((HUINT32)cmd, (HUINT32)pResultData, (HUINT32)1);

	HOMAPI_MEMFREE(pResultData);

	return hErr;
}

static HERROR 	__query_result_event_handler( HUINT32 cmd, void *param, HUINT32 size, void *userdata )
{
	return  __decode_result_event( cmd, param, size);
}

static	HUINT32	__dmr_notifier(HUINT32 accesscode, HUINT32 cmd, void *param, HUINT32 size)
{
	dmr_context_t *mgr = __ctx();

	HOMAPI_SEM_GET;

	if ( NULL == param )
		HOMAPI_Query( eHOMAPI_Service_DMR, NULL, mgr, (HBOOL (*)(HUINT32, void*, HUINT32, void *))__query_result_event_handler);
	else
		__decode_result_event(cmd, param, size);

	if ( mgr->accesscode != accesscode )
	{
		HxLOG_Debug("accesscode in mgr (%d) not equal to cur accesscode (%d) \n", mgr->accesscode, accesscode);
		HOMAPI_SEM_RELEASE;
	}

	HOMAPI_SEM_RELEASE;

	return 0;
}

static	HERROR	__register_notifier( void )
{
	dmr_context_t *mgr = __ctx();
	HUINT32 	accesscode = 0;

	if(mgr == NULL)
	{
		HxLOG_Error("mgr == NULL\n");
		return ERR_FAIL;
	}

	mgr->bInit = FALSE;
	accesscode = HOMAPI_Query( eHOMAPI_Service_DMR, NULL, NULL,  NULL );

	if( accesscode )
	{
		mgr->accesscode = accesscode;
		mgr->notifierId = HOMAPI_ConnectNotifier( accesscode, __dmr_notifier );
		mgr->bInit = TRUE;

		return ERR_OK;
	}

	return ERR_FAIL;
}

static	void	__unregister_notifier( void )
{
	dmr_context_t *mgr = __ctx();

	if ( mgr )
		mgr->notifierId = HOMAPI_DisconnectNotifier( mgr->accesscode, mgr->notifierId );
}

#define ________________Public_Interfaces__________________________
void	HOMAPI_DMR_Init( void )
{
#if !defined(CONFIG_HOMMA_DMR)
	HxLOG_Error("### (HOMAPI_DMR_Init) Not Support DLNA DMR ###\n");
	return;
#else
	if(_ulModuleSemaphoreId == 0)
	{
		HxSEMT_Create(&_ulModuleSemaphoreId, "sadlnadmr", HxSEMT_FIFO);
	}

	if(__register_notifier() != ERR_OK)
	{
		HxLOG_Error("[HOMAPI_DMR_Init] __register_notifier() fail!\n");
	}
#endif
}

void	HOMAPI_DMR_DeInit( void )
{
#if !defined(CONFIG_HOMMA_DMR)
	HxLOG_Error("### (HOMAPI_DMR_DeInit) Not Support DLNA DMR ###\n");
	return;
#else
	__unregister_notifier( );

	if(_ulModuleSemaphoreId != 0)
	{
		HxSEMT_Destroy(_ulModuleSemaphoreId);
		_ulModuleSemaphoreId = 0;
	}
#endif
}

void	HOMAPI_DMR_RegisterNotifier( HOMAPI_EventCallback pfnObjectHandler )
{
#if !defined(CONFIG_HOMMA_DMR)
	HxLOG_Error("### (HOMAPI_DMR_RegisterNotifier) Not Support DLNA DMR ###\n");
	return;
#else
	dmr_context_t 	*mgr = __ctx();
	mgr->cb = pfnObjectHandler;
#endif
}

void	HOMAPI_DMR_UnRegisterNotifier( void )
{
#if !defined(CONFIG_HOMMA_DMR)
	HxLOG_Error("### (HOMAPI_DMR_UnRegisterNotifier) Not Support DLNA DMR ###\n");
	return;
#else
	dmr_context_t 	*mgr = __ctx();
	mgr->cb = NULL;
#endif
}

HERROR	HOMAPI_DMR_Start( HCHAR *pFriendlyName )
{
#if !defined(CONFIG_HOMMA_DMR)
	HxLOG_Error("### (HOMAPI_DMR_Start) Not Support DLNA DMR ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t 			data;
	HOMxDMR_Args_Start_t	arg;

	CHECK_DMR_MODULE_INIT;

	HxSTD_MemSet( &arg, 0x00, sizeof( HOMxDMR_Args_Start_t) );

	if( pFriendlyName && pFriendlyName[0] != '\0')
		HLIB_STD_StrUtf8NCpy( arg.szFriendlyName, pFriendlyName, sizeof( arg.szFriendlyName ) );

	data.cmd = eHOMAPI_DMR_Command_Start;
	data.data = (void *)&arg;
	data.size = sizeof(HOMxDMR_Args_Start_t);

	HOMAPI_Call( eHOMAPI_Service_DMR, &data);

	return ERR_OK;
#endif
}

HERROR	HOMAPI_DMR_Stop( void )
{
#if !defined(CONFIG_HOMMA_DMR)
	HxLOG_Error("### (HOMAPI_DMR_Stop) Not Support DLNA DMR ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;

	CHECK_DMR_MODULE_INIT;

	data.cmd = eHOMAPI_DMR_Command_Stop;
	data.data = NULL;
	data.size = 0;

	HOMAPI_Call( eHOMAPI_Service_DMR, &data);

	return ERR_OK;
#endif
}

HERROR	HOMAPI_DMR_SetFriendlyName( const HCHAR *pFriendlyName )
{
#if !defined(CONFIG_HOMMA_DMR)
	HxLOG_Error("### (HOMAPI_DMR_SetFriendlyName) Not Support DLNA DMR ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t			data;
	HOMxDMR_Args_Rename_t	arg;

	CHECK_DMR_MODULE_INIT;

	if( pFriendlyName && pFriendlyName[0] != '\0')
	{
		HxSTD_MemSet( &arg, 0x00, sizeof( HOMxDMR_Args_Rename_t) );
		HLIB_STD_StrUtf8NCpy( arg.szFriendlyName, pFriendlyName, sizeof( arg.szFriendlyName ) );

		data.cmd = eHOMAPI_DMR_Command_Rename;
		data.data = (void *)&arg;
		data.size = sizeof(HOMxDMR_Args_Rename_t);

		HOMAPI_Call( eHOMAPI_Service_DMR, &data);
		return ERR_OK;
	}

	HxLOG_Error("### (HOMAPI_DMR_SetFriendlyName) Error> DMR rename() failed ###\n");
	return ERR_FAIL;
#endif
}

HERROR	HOMAPI_DMR_SetBringInUri( const HCHAR *pUrl, const HCHAR *pPath )
{
#if !defined(CONFIG_HOMMA_DMR)
	HxLOG_Error("### (HOMAPI_DMR_SetBringInUri) Not Support DLNA DMR ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;
	HOMxDMR_Args_BringIn_t arg;
	HxLOG_Assert( pUrl );

	CHECK_DMR_MODULE_INIT;

	HxSTD_MemSet( &arg, 0x00, sizeof( HOMxDMR_Args_BringIn_t ) );
	HLIB_STD_StrUtf8NCpy( arg.szMediaURI, pUrl, sizeof( arg.szMediaURI ) );
	HLIB_STD_StrUtf8NCpy( arg.szMediaPath, pPath, sizeof( arg.szMediaPath ) );

	data.cmd = eHOMAPI_DMR_Command_BringIn;
	data.data = (void *)&arg;
	data.size = sizeof(HOMxDMR_Args_BringIn_t);

	HOMAPI_Call( eHOMAPI_Service_DMR, &data);

	return ERR_OK;
#endif
}

HERROR	HOMAPI_DMR_SetOperationMode( HINT32 nMode )
{
#if !defined(CONFIG_HOMMA_DMR)
	HxLOG_Error("### (HOMAPI_DMR_SetOperationMode) Not Support DLNA DMR ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;
	HOMxDMR_Args_OperationMode_t arg;

	CHECK_DMR_MODULE_INIT;

	arg.mode = nMode;

	data.cmd = eHOMAPI_DMR_Command_OperationMode;
	data.data = (void *)&arg;
	data.size = sizeof(HOMxDMR_Args_OperationMode_t);

	HOMAPI_Call( eHOMAPI_Service_DMR, &data);

	return ERR_OK;
#endif
}
