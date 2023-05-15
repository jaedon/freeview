/*************************************************************************************************************
	File 		: homapi_woonserver.c
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2012/11/12
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and
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
#include	<hlib.h>
#include	<homapi.h>

#define	________________Define_____________________________________
#define CHECK_WOON_MODULE_INIT	do { HxLOG_Warning("\n"); if(woonserver_IsInitialized() == FALSE) HOMAPI_WOONSERVER_Init(); } while (0)

#define ________________Typedef____________________________________
typedef struct
{
	HBOOL		bInit;
	HUINT32		accesscode;
	HUINT32		notifierId;

	HOMAPI_EventCallback cb;
} _WOONSERVER_MGR_t;

#define ________________Static_Variables___________________________
static	HUINT32		_ulModuleSemaphoreId = 0;

#define ________________Internal_Functions_________________________
static _WOONSERVER_MGR_t * woonserver_GetManager( void )
{
	static _WOONSERVER_MGR_t s_manager;
	return &s_manager;
}

static HBOOL woonserver_IsInitialized(void)
{
	_WOONSERVER_MGR_t 	*mgr = woonserver_GetManager();

	if(mgr == NULL)
	{
		HxLOG_Error("mgr == NULL\n");
		return FALSE;
	}

	return mgr->bInit;
}

static HERROR 	woonserver_DecodeResultEvent ( HUINT32	cmd, void *data, HUINT32 size)
{
	_WOONSERVER_MGR_t *mgr = woonserver_GetManager();
	HERROR			hErr	= ERR_FAIL;
	void			*pResultData = NULL;

	switch ( cmd )
	{
		case eHOMAPI_WoonServer_Event_Start:
		case eHOMAPI_WoonServer_Event_Stop:
		case eHOMAPI_WoonServer_Event_Renamed:
		case eHOMAPI_WoonServer_Event_TestConnection:
		case eHOMAPI_WoonServer_Event_PinCode:
			{
				if(data != NULL && size != 0)
					pResultData = HLIB_STD_MemDup(data, size);
			}
			break;

		default: return ERR_OK;
	}

	if ( mgr->cb)
	{
		mgr->cb( (HUINT32)cmd, (HUINT32)pResultData, (HUINT32)1);
	}

	if ( pResultData )
		HLIB_STD_MemFree( pResultData );

	return hErr;
}

static HERROR 	woonserver_QueryResultEventHandler ( HUINT32 cmd, void *param, HUINT32 size, void *userdata )
{
	HERROR		hErr	= ERR_FAIL;
	_WOONSERVER_MGR_t *mgr = woonserver_GetManager();

	if ( NULL == mgr )	return ERR_FAIL;

	hErr = woonserver_DecodeResultEvent( cmd, param, size );

	return hErr;
}

static	HUINT32	woonserver_Notifier(HUINT32 accesscode, HUINT32 cmd, void *param, HUINT32 size)
{
	_WOONSERVER_MGR_t *mgr = woonserver_GetManager();

	HOMAPI_SEM_GET;

	if ( NULL == param )
		HOMAPI_Query( eHOMAPI_Service_WoonServer, NULL, mgr, (HBOOL (*)(HUINT32, void*, HUINT32, void *))woonserver_QueryResultEventHandler );
	else
		woonserver_DecodeResultEvent(cmd, param, size);

	if ( mgr->accesscode != accesscode )
	{
		HxLOG_Debug("accesscode in mgr (%d) not equal to cur accesscode (%d) \n", mgr->accesscode, accesscode);
		HOMAPI_SEM_RELEASE;
	}

	HOMAPI_SEM_RELEASE;

	return 0;
}
static	HERROR woonserver_RegisterNotifier( void )
{
	_WOONSERVER_MGR_t *mgr = woonserver_GetManager();
	HUINT32 	accesscode = 0;

	if(mgr == NULL)
	{
		HxLOG_Error("mgr == NULL\n");
		return ERR_FAIL;
	}

	mgr->bInit = FALSE;
	accesscode = HOMAPI_Query( eHOMAPI_Service_WoonServer, NULL, NULL,  NULL );

	if( accesscode )
	{
		mgr->accesscode = accesscode;
		mgr->notifierId = HOMAPI_ConnectNotifier( accesscode, woonserver_Notifier );
		mgr->bInit = TRUE;

		return ERR_OK;
	}

	return ERR_FAIL;
}

static	void	woonserver_UnRegisterNotifier( void )
{
	_WOONSERVER_MGR_t *mgr = woonserver_GetManager();

	if ( mgr )
		mgr->notifierId = HOMAPI_DisconnectNotifier( mgr->accesscode, mgr->notifierId );
}

#define ________________Public_Interfaces__________________________
void	HOMAPI_WOONSERVER_Init( void )
{
#if !defined(CONFIG_HOMMA_WOON)
	HxLOG_Error("### (HOMAPI_WOONSERVER_Init) Not Support WOON ###\n");
	return;
#else
	if(_ulModuleSemaphoreId == 0)
	{
		HxSEMT_Create(&_ulModuleSemaphoreId, "sawoonserver", HxSEMT_FIFO);
	}

	if(woonserver_RegisterNotifier() != ERR_OK)
	{
		HxLOG_Error("[HOMAPI_WOONSERVER_Init] woonserver_RegisterNotifier() fail!\n");
	}
#endif
}

void	HOMAPI_WOONSERVER_DeInit( void )
{
#if !defined(CONFIG_HOMMA_WOON)
	HxLOG_Error("### (HOMAPI_WOONSERVER_DeInit) Not Support WOON ###\n");
	return;
#else
	woonserver_UnRegisterNotifier( );

	if(_ulModuleSemaphoreId != 0)
	{
		HxSEMT_Destroy(_ulModuleSemaphoreId);
		_ulModuleSemaphoreId = 0;
	}
#endif
}

void	HOMAPI_WOONSERVER_RegisterNotifier( HOMAPI_EventCallback pfnObjectHandler )
{
#if !defined(CONFIG_HOMMA_WOON)
	HxLOG_Error("### (HOMAPI_WOONSERVER_RegisterNotifier) Not Support WOON ###\n");
	return;
#else
	_WOONSERVER_MGR_t * mgr = woonserver_GetManager();
	mgr->cb = pfnObjectHandler;
#endif
}

void	HOMAPI_WOONSERVER_UnRegisterNotifier( void )
{
#if !defined(CONFIG_HOMMA_WOON)
	HxLOG_Error("### (HOMAPI_WOONSERVER_UnRegisterNotifier) Not Support WOON ###\n");
	return;
#else
	_WOONSERVER_MGR_t * mgr = woonserver_GetManager();
	mgr->cb = NULL;
#endif
}

HERROR	HOMAPI_WOONSERVER_Start( HCHAR *pFriendlyName )
{
#if !defined(CONFIG_HOMMA_WOON)
	HxLOG_Error("### (HOMAPI_WOONSERVER_Start) Not Support WOON ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t 				data;
	HOMxWoonServer_Args_Start_t arg;

	CHECK_WOON_MODULE_INIT;

	HxSTD_MemSet(&arg, 0, sizeof(HOMxWoonServer_Args_Start_t));
	HLIB_STD_StrUtf8NCpy( arg.szFriendlyName, pFriendlyName, sizeof( arg.szFriendlyName ) );

	data.data = (void *)&arg;
	data.cmd = eHOMAPI_WoonServer_Command_Start;
	data.size = sizeof(HOMxWoonServer_Args_Start_t);

	HOMAPI_Call( eHOMAPI_Service_WoonServer, &data);

	return ERR_OK;
#endif
}

HERROR	HOMAPI_WOONSERVER_Stop( void )
{
#if !defined(CONFIG_HOMMA_WOON)
	HxLOG_Error("### (HOMAPI_WOONSERVER_Stop) Not Support WOON ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;

	CHECK_WOON_MODULE_INIT;

	data.cmd  = eHOMAPI_WoonServer_Command_Stop;
	data.data = NULL;
	data.size = 0;

	HOMAPI_Call( eHOMAPI_Service_WoonServer, &data);

	return ERR_OK;
#endif
}


HERROR	HOMAPI_WOONSERVER_SetFriendlyName( const HCHAR *pFriendlyName )
{
#if !defined(CONFIG_HOMMA_WOON)
	HxLOG_Error("### (HOMAPI_WOONSERVER_SetFriendlyName) Not Support WOON ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;
	HOMxWoonServer_Args_Rename_t arg;

	CHECK_WOON_MODULE_INIT;

	HxSTD_MemSet(&arg, 0, sizeof(HOMxWoonServer_Args_Rename_t));
	HxSTD_snprintf( arg.szFriendlyName, sizeof( arg.szFriendlyName ), "%s", pFriendlyName );

	data.data = (void *)&arg;
	data.cmd = eHOMAPI_WoonServer_Command_Rename;
	data.size = sizeof(HOMxWoonServer_Args_Rename_t);

	HOMAPI_SEM_GET;

	HOMAPI_Call( eHOMAPI_Service_WoonServer, &data);

	HOMAPI_SEM_RELEASE;

	return ERR_OK;
#endif
}

HERROR	HOMAPI_WOONSERVER_TestConnection(void )
{
#if !defined(CONFIG_HOMMA_WOON)
	HxLOG_Error("### (HOMAPI_WOONSERVER_TestConnection) Not Support WOON ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;

	CHECK_WOON_MODULE_INIT;

	data.cmd  = eHOMAPI_WoonServer_Command_TestConnection;
	data.data = NULL;
	data.size = 0;

	HOMAPI_SEM_GET;

	HOMAPI_Call( eHOMAPI_Service_WoonServer, &data);

	HOMAPI_SEM_RELEASE;

	return ERR_OK;
#endif
}
