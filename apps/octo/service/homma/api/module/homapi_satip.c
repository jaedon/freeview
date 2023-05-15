/**************************************************************
*	@file	homapi_satip.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			DLNA / SATIP
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
#define CHECK_SATIP_MODULE_INIT	do { if(__IsInitialized() == FALSE) HOMAPI_SATIP_Init(); } while (0)

#define ________________Typedef____________________________________
typedef struct
{
	HBOOL		bInit;

	HUINT32		accesscode;
	HUINT32		notifierId;

	HOMAPI_EventCallback cb;
} satip_context_t;

#define ________________Static_Variables___________________________
static  HUINT32     _ulModuleSemaphoreId = 0;

#define ________________Internal_Functions_________________________
static satip_context_t * __ctx( void )
{
	static satip_context_t s_manager;
	return &s_manager;
}

static HBOOL __IsInitialized(void)
{
	satip_context_t 	*mgr = __ctx();

	if(mgr == NULL)
	{
		HxLOG_Error("mgr == NULL\n");
		return FALSE;
	}

	return mgr->bInit;
}

static HERROR 	__DecodeResultEvent ( HUINT32	cmd, void *data, HUINT32 size)
{
	satip_context_t 	*mgr = __ctx();
	HERROR			hErr = ERR_FAIL;
	void			*pResultData = NULL;

	switch(cmd)
	{
		case eHOMAPI_SATIP_Event_Started:
		case eHOMAPI_SATIP_Event_Stopped:
		case eHOMAPI_SATIP_Event_Status:
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
static HERROR 	__QueryResultEventHandler( HUINT32 cmd, void *param, HUINT32 size, void *userdata )
{
	return  __DecodeResultEvent( cmd, param, size);
}

static	HUINT32	__Notifier(HUINT32 accesscode, HUINT32 cmd, void *param, HUINT32 size)
{
	satip_context_t *mgr = __ctx();

	HOMAPI_SEM_GET;

	if ( NULL == param )
		HOMAPI_Query( eHOMAPI_Service_SatIpServer, NULL, mgr, (HBOOL (*)(HUINT32, void *, HUINT32, void *))__QueryResultEventHandler );
	else
		__DecodeResultEvent(cmd, param, size);

	if ( mgr->accesscode != accesscode )
	{
		HxLOG_Debug("accesscode in mgr (%d) not equal to cur accesscode (%d) \n", mgr->accesscode, accesscode);
		HOMAPI_SEM_RELEASE;
	}

	HOMAPI_SEM_RELEASE;

	return 0;
}

static	HERROR	__RegisterNotifier( void )
{
	satip_context_t *mgr  =  __ctx();
	HUINT32 	accesscode = 0;

	if(mgr == NULL)
	{
		HxLOG_Error("mgr == NULL\n");
		return ERR_FAIL;
	}

	mgr->bInit = FALSE;
	accesscode = HOMAPI_Query( eHOMAPI_Service_SatIpServer, NULL, NULL,  NULL );

	if( accesscode )
	{
		mgr->accesscode = accesscode;
		mgr->notifierId = HOMAPI_ConnectNotifier( accesscode, __Notifier );
		mgr->bInit = TRUE;

		return ERR_OK;
	}

	return ERR_FAIL;
}

static	void	__UnRegisterNotifier( void )
{
	satip_context_t *mgr  =  __ctx();

	if ( mgr )
		mgr->notifierId = HOMAPI_DisconnectNotifier( mgr->accesscode, mgr->notifierId );
}

#define ________________Public_Interfaces__________________________
void	HOMAPI_SATIP_Init( void )
{
#if !defined(CONFIG_HOMMA_SATIP)
	HxLOG_Error("### (HOMAPI_SATIP_Init) Not Support SATIP Server###\n");
	return;
#else
	if(_ulModuleSemaphoreId == 0)
	{
		HxSEMT_Create(&_ulModuleSemaphoreId, "sadlnasatip", HxSEMT_FIFO);
	}

	if(__RegisterNotifier() != ERR_OK)
	{
		HxLOG_Error("[HOMAPI_SATIP_Init] __RegisterNotifier() fail!\n");
	}
#endif
}

void	HOMAPI_SATIP_DeInit( void )
{
#if !defined(CONFIG_HOMMA_SATIP)
	HxLOG_Error("### (HOMAPI_SATIP_DeInit) Not Support SATIP Server###\n");
	return;
#else
	__UnRegisterNotifier();

	if(_ulModuleSemaphoreId != 0)
	{
		HxSEMT_Destroy(_ulModuleSemaphoreId);
		_ulModuleSemaphoreId = 0;
	}
#endif
}

void	HOMAPI_SATIP_RegisterNotifier( HOMAPI_EventCallback pfnObjectHandler )
{
#if !defined(CONFIG_HOMMA_SATIP)
	HxLOG_Error("### (HOMAPI_SATIP_RegisterNotifier) Not Support SATIP Server###\n");
	return;
#else
	satip_context_t 	*mgr  = __ctx();
	mgr->cb= pfnObjectHandler;
#endif
}

void	HOMAPI_SATIP_UnRegisterNotifier( void )
{
#if !defined(CONFIG_HOMMA_SATIP)
	HxLOG_Error("### (HOMAPI_SATIP_UnRegisterNotifier) Not Support SATIP Server###\n");
	return;
#else
	satip_context_t 	*mgr = __ctx();
	mgr->cb= NULL;
#endif
}

HERROR	HOMAPI_SATIP_Start( HCHAR *pFriendlyName)
{
#if !defined(CONFIG_HOMMA_SATIP)
	HxLOG_Error("### (HOMAPI_SATIP_Start) Not Support SATIP Server###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t 			data;
	HOMxSATIP_Args_Start_t	arg;

	CHECK_SATIP_MODULE_INIT;

	HxSTD_MemSet(&arg, 0, sizeof(HOMxSATIP_Args_Start_t));

	HxSTD_snprintf( arg.szFriendlyName, sizeof( arg.szFriendlyName ), "%s", pFriendlyName );

	data.cmd = eHOMAPI_SATIP_Command_Start;
	data.data = (void *)&arg;
	data.size = sizeof(HOMxSATIP_Args_Start_t);

	HOMAPI_Call( eHOMAPI_Service_SatIpServer, &data);

	return ERR_OK;
#endif
}

HERROR	HOMAPI_SATIP_Stop( void )
{
#if !defined(CONFIG_HOMMA_SATIP)
	HxLOG_Error("### (HOMAPI_SATIP_Stop) Not Support SATIP Server###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;

	CHECK_SATIP_MODULE_INIT;

	data.cmd = eHOMAPI_SATIP_Command_Stop;
	data.data = NULL;
	data.size = 0;

	HOMAPI_Call( eHOMAPI_Service_SatIpServer, &data);

	return ERR_OK;
#endif
}

HERROR	HOMAPI_SATIP_SetFriendlyName(const  HCHAR *pFriendlyName)
{
#if !defined(CONFIG_HOMMA_SATIP)
	HxLOG_Error("### (HOMAPI_SATIP_SetFriendlyName) Not Support SATIP Server###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t 			data;
	HOMxSATIP_Args_Rename_t	arg;
	satip_context_t			*hContext = NULL;

	CHECK_SATIP_MODULE_INIT;

	hContext = __ctx();
	if( hContext == NULL ) 	return ERR_FAIL;

	HxSTD_MemSet(&arg, 0, sizeof(HOMxSATIP_Args_Rename_t));

	HxSTD_snprintf( arg.szFriendlyName, sizeof( arg.szFriendlyName ), "%s", pFriendlyName );

	data.cmd = eHOMAPI_SATIP_Command_Rename;
	data.data = (void *)&arg;
	data.size = sizeof(HOMxSATIP_Args_Rename_t);

	HOMAPI_Call( eHOMAPI_Service_SatIpServer, &data);

	return ERR_OK;
#endif
}


