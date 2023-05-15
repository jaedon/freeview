/**************************************************************
*	@file	homapi_dial.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			DLNA / DIAL
**************************************************************/

/*
* ⓒ 2011-2012 Humax Co., Ltd.
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
#include    <homapi.h>

#define	________________Define_____________________________________
#define CHECK_DIAL_MODULE_INIT	do { HxLOG_Warning("\n"); if(__DIAL_IsInitialized() == FALSE) HOMAPI_DIAL_Init(); } while (0)

#define ________________Typedef____________________________________
typedef struct
{
	HBOOL		bInit;

	HUINT32		accesscode;
	HUINT32		notifierId;

	HOMAPI_EventCallback cb;

	HBOOL		bRun;
	HCHAR		szFriendlyName[ 128 ];
} DIAL_context_t;

typedef struct _string_container_t {
	HCHAR * str;
} string_container_t;

#define ________________Static_Variables___________________________
static  HUINT32     _ulModuleSemaphoreId = 0;

#define ________________Internal_Prototypes_________________________
static HERROR __query(HINT8 cmd, HINT8 * data, HINT32 size, void * result);

#define ________________Internal_Functions_________________________
static DIAL_context_t *__ctx( void )
{
	static DIAL_context_t s_manager;
	return &s_manager;
}

static HBOOL	__DIAL_IsInitialized(void)
{
	DIAL_context_t 	*mgr = __ctx();

	if(mgr == NULL)
	{
		HxLOG_Error("mgr == NULL\n");
		return FALSE;
	}

	return mgr->bInit;
}

static HERROR 	__decode_result_event( HUINT32	cmd, void *data, HUINT32 size)
{
	DIAL_context_t *mgr = __ctx();
	HERROR		hErr	= ERR_FAIL;
	void		*pResultData = NULL;
	HOMxDIAL_Event_AppLaunch_t evt; // TODO: Change struct name to tell it is used for launch and stop

	switch ( cmd)
	{
		case eHOMAPI_DIAL_Event_AppLaunch:
			{
				HxSTD_MemCopy(&evt, data, sizeof(HOMxDIAL_Event_AppLaunch_t));
				HxLOG_Error("[%s:%d] APP LAUNCH : %s / %s\n", __func__, __LINE__, evt.szApplicationName, evt.szUrl);
				pResultData = &evt;
			}
			break;

		case eHOMAPI_DIAL_Event_AppStop:
			{
				HxSTD_MemCopy(&evt, data, sizeof(HOMxDIAL_Event_AppLaunch_t));
				HxLOG_Debug("[%s:%d] APP STOP : %s\n", __func__, __LINE__, evt.szApplicationName);
				pResultData = &evt;
			}
			break;

		case eHOMAPI_DIAL_Event_AppStatus:
			{
			}
			break;

		case eHOMAPI_DIAL_Event_Started:
			{
			}
			break;

		case eHOMAPI_DIAL_Event_Stopped:
			{
			}
			break;

		default:
			HxLOG_Warning("[%s:%d] can't handle : dial Property (%d) \n", __HxFILE__, __HxLINE__, cmd);
			return ERR_OK;
	}

	// mgr->cb would be registered from "$appkit/dlna/apk_dlna_dial.c"
	if (mgr->cb) {
		hErr = mgr->cb((HUINT32)cmd,
					   (HUINT32)pResultData,  /* DANGEROUS TYPE CONVERTING (32-bit fixed) */
					   (HUINT32)1);
	} else {
		hErr = ERR_OK;
	}

	return hErr == ERR_OK ? TRUE : FALSE;
}

static HBOOL 	__query_result_event_handler( HUINT32 cmd, void *param, HUINT32 size, void *userdata )
{
	switch (cmd) {
	case eHOMAPI_DIAL_Command_IsRunning:
		{
			string_container_t * container = (string_container_t*)userdata;
			container->str = HLIB_STD_MemDup(param, size);
		}
		break;
	case eHOMAPI_DIAL_Command_GetFriendlyName:
		{
			string_container_t * container = (string_container_t*)userdata;
			container->str = HLIB_STD_MemDup(param, size);
		}
		break;
	case eHOMAPI_DIAL_Command_GetAdditionalDataUrl:
		{
			string_container_t * container = (string_container_t*)userdata;
			container->str = HLIB_STD_MemDup(param, size);
		}
		break;
	default:
		return __decode_result_event( cmd, param, size);
	}

	return TRUE;
}

static	HUINT32	__DIAL_notifier(HUINT32 accesscode, HUINT32 cmd, void *param, HUINT32 size)
{
	DIAL_context_t *mgr = __ctx();

	HOMAPI_SEM_GET;

	if ( NULL == param )
	{
		HOMAPI_Query(eHOMAPI_Service_Dial, NULL, mgr, __query_result_event_handler);
	}
	else
	{
		__decode_result_event(cmd, param, size);
	}
	if ( mgr->accesscode != accesscode )
	{
		HxLOG_Error("accesscode in mgr (%d) not equal to cur accesscode (%d) \n", mgr->accesscode, accesscode);
		HOMAPI_SEM_RELEASE;
	}

	HOMAPI_SEM_RELEASE;

	return 0;
}

static	HERROR	__register_notifier( void )
{
	DIAL_context_t *mgr = __ctx();
	HUINT32 	accesscode = 0;

	if(mgr == NULL)
	{
		HxLOG_Error("mgr == NULL\n");
		return ERR_FAIL;
	}

	mgr->bInit = FALSE;
	accesscode = HOMAPI_Query( eHOMAPI_Service_Dial, NULL, NULL,  NULL );

	if( accesscode )
	{
		mgr->accesscode = accesscode;
		mgr->notifierId = HOMAPI_ConnectNotifier( accesscode, __DIAL_notifier );
		mgr->bInit = TRUE;

		return ERR_OK;
	}

	HxLOG_Error("[__register_notifier] Fail!!\n");
	return ERR_FAIL;
}

static	void	__unregister_notifier( void )
{
	DIAL_context_t *mgr = __ctx();

	if ( mgr )
		mgr->notifierId = HOMAPI_DisconnectNotifier( mgr->accesscode, mgr->notifierId );
}

static void __init_HOMxCommData_t(HOMxCommData_t * container, HINT8 cmd, HINT8 * data, HINT32 size) {
	memset(container, 0, sizeof(HOMxCommData_t));

	container->cmd = cmd;
	container->data = data;
	container->size = size;
}

static HERROR __query(HINT8 cmd, HINT8 * data, HINT32 size, void * result) {

	HUINT32 accessCode = 0;
	HOMxCommData_t container;
	HERROR err = ERR_FAIL;

	__init_HOMxCommData_t(&container, cmd, data, size);

	HOMAPI_SEM_GET;

	accessCode = HOMAPI_Query(eHOMAPI_Service_Dial,
							  (void *)&container,
							  result,
							  __query_result_event_handler);

	if(!accessCode)
	{
		HxLOG_Error("### (__query) Error ###\n");
		goto quit;
	}

	err = ERR_OK;

 quit:
	HOMAPI_SEM_RELEASE;
	return err;
}

#define ________________Public_Interfaces__________________________

void	HOMAPI_DIAL_Init( void )
{
#if !defined(CONFIG_HOMMA_DIAL)
	HxLOG_Error("### (HOMAPI_DIAL_Init) Not Support DIAL ###\n");
	return;
#else
	if(_ulModuleSemaphoreId == 0)
	{
		HxSEMT_Create(&_ulModuleSemaphoreId, "sadlna_dial", HxSEMT_FIFO);
	}

	if(__register_notifier() != ERR_OK)
	{
		HxLOG_Error("[HOMAPI_DIAL_Init] __register_notifier() fail!\n");
	}
#endif
}

void	HOMAPI_DIAL_DeInit( void )
{
#if !defined(CONFIG_HOMMA_DIAL)
	HxLOG_Error("### (HOMAPI_DIAL_DeInit) Not Support DIAL ###\n");
	return;
#else
	__unregister_notifier();

	if(_ulModuleSemaphoreId != 0)
	{
		HxSEMT_Destroy(_ulModuleSemaphoreId);
		_ulModuleSemaphoreId = 0;
	}
#endif
}

void	HOMAPI_DIAL_RegisterNotifier( HOMAPI_EventCallback pfnObjectHandler )
{
#if !defined(CONFIG_HOMMA_DIAL)
	HxLOG_Error("### (HOMAPI_DIAL_DeInit) Not Support DIAL ###\n");
	return;
#else
	DIAL_context_t 	*mgr = __ctx();
	HxLOG_Error("** pfnObjectHandler : %p\n", pfnObjectHandler);
	mgr->cb = pfnObjectHandler;
#endif
}

void	HOMAPI_DIAL_UnRegisterNotifier( void )
{
#if !defined(CONFIG_HOMMA_DIAL)
	HxLOG_Error("### (HOMAPI_DIAL_DeInit) Not Support DIAL ###\n");
	return;
#else
	DIAL_context_t 	*mgr = __ctx();
	mgr->cb = NULL;
#endif
}

HERROR	HOMAPI_DIAL_Start( HCHAR *pFriendlyName )
{
#if !defined(CONFIG_HOMMA_DIAL)
	HxLOG_Error("### (HOMAPI_DIAL_DeInit) Not Support DIAL ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;
	HOMxDIAL_Args_Start_t arg;

	CHECK_DIAL_MODULE_INIT;

	HxSTD_MemSet(&arg, 0, sizeof(HOMxDIAL_Args_Start_t));

	HxSTD_snprintf(arg.szFriendlyName, sizeof(arg.szFriendlyName), "%s", pFriendlyName);

	data.cmd = eHOMAPI_DIAL_Command_Start;
	data.data = (void *)&arg;
	data.size = sizeof(HOMxDIAL_Args_Start_t);

	HOMAPI_Call( eHOMAPI_Service_Dial, &data);

	return ERR_OK;
#endif
}

HERROR	HOMAPI_DIAL_Stop( void )
{
#if !defined(CONFIG_HOMMA_DIAL)
	HxLOG_Error("### (HOMAPI_DIAL_DeInit) Not Support DIAL ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;

	CHECK_DIAL_MODULE_INIT;

	data.cmd = eHOMAPI_DIAL_Command_Stop;
	data.data = NULL;
	data.size = 0;

	HOMAPI_Call( eHOMAPI_Service_Dial, &data);

	return ERR_OK;
#endif
}

HERROR	HOMAPI_DIAL_SetFriendlyName(const HCHAR *pFriendlyName )
{
#if !defined(CONFIG_HOMMA_DIAL)
	HxLOG_Error("### (HOMAPI_DIAL_SetFriendlyName) Not Support DIAL ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;
	HOMxDIAL_Args_Rename_t arg;

	CHECK_DIAL_MODULE_INIT;

	HxSTD_MemSet(&arg, 0, sizeof(HOMxDIAL_Args_Rename_t));

	HxSTD_snprintf(arg.szFriendlyName, sizeof(arg.szFriendlyName), "%s", pFriendlyName);

	data.cmd = eHOMAPI_DIAL_Command_Rename;
	data.data = (void *)&arg;
	data.size = sizeof(HOMxDIAL_Args_Rename_t);

	HOMAPI_Call( eHOMAPI_Service_Dial, &data);

	return ERR_OK;
#endif
}

HERROR	HOMAPI_DIAL_AppRegist(const HCHAR * app)
{
#if !defined(CONFIG_HOMMA_DIAL)
	HxLOG_Error("### (HOMAPI_DIAL_AppRegist) Not Support DIAL ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;
	HOMxDIAL_Args_AppRegist_t arg;

	CHECK_DIAL_MODULE_INIT;

	HxSTD_MemSet(&arg, 0, sizeof(HOMxDIAL_Args_AppRegist_t));

	HxSTD_snprintf(arg.szApplicationName, sizeof(arg.szApplicationName), "%s", app);

	data.cmd = eHOMAPI_DIAL_Command_AppRegist;
	data.data = (void *)&arg;
	data.size = sizeof(HOMxDIAL_Args_AppRegist_t);

	HOMAPI_Call( eHOMAPI_Service_Dial, &data);

	return ERR_OK;
#endif
}

HERROR	HOMAPI_DIAL_AppUnregist(const HCHAR * app)
{
#if !defined(CONFIG_HOMMA_DIAL)
	HxLOG_Error("### (HOMAPI_DIAL_AppUnregist) Not Support DIAL ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;
	HOMxDIAL_Args_AppUnregist_t arg;

	CHECK_DIAL_MODULE_INIT;

	HxSTD_MemSet(&arg, 0, sizeof(HOMxDIAL_Args_AppUnregist_t));

	HxSTD_snprintf(arg.szApplicationName, sizeof(arg.szApplicationName), "%s", app);

	data.cmd = eHOMAPI_DIAL_Command_AppUnregist;
	data.data = (void *)&arg;
	data.size = sizeof(HOMxDIAL_Args_AppUnregist_t);

	HOMAPI_Call( eHOMAPI_Service_Dial, &data);

	return ERR_OK;
#endif
}

HERROR	HOMAPI_DIAL_SetAppStatus(const HCHAR * app, HINT32 status)
{
#if !defined(CONFIG_HOMMA_DIAL)
	HxLOG_Error("### (HOMAPI_DIAL_SetAppStatus) Not Support DIAL ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;
	HOMxDIAL_Args_AppStatus_t arg;

	CHECK_DIAL_MODULE_INIT;

	HxSTD_MemSet(&arg, 0, sizeof(HOMxDIAL_Args_AppStatus_t));

	HxSTD_snprintf(arg.szApplicationName, sizeof(arg.szApplicationName), "%s", app);
	arg.nStatus = status;

	data.cmd = eHOMAPI_DIAL_Command_AppStatusUpdate;
	data.data = (void *)&arg;
	data.size = sizeof(HOMxDIAL_Args_AppStatus_t);

	HOMAPI_Call( eHOMAPI_Service_Dial, &data);

	return ERR_OK;
#endif
}

HERROR	HOMAPI_DIAL_SetCookie(const HCHAR * cookie )
{
#if !defined(CONFIG_HOMMA_DIAL)
	HxLOG_Error("### (HOMAPI_DIAL_SetCookie) Not Support DIAL ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;
	HOMxDIAL_Args_Cookie_t arg;

	CHECK_DIAL_MODULE_INIT;

	HxSTD_MemSet(&arg, 0, sizeof(HOMxDIAL_Args_Cookie_t));

	HxSTD_snprintf(arg.cookie, sizeof(arg.cookie), "%s", cookie);

	data.cmd = eHOMAPI_DIAL_Command_SetCookie;
	data.data = (void *)&arg;
	data.size = sizeof(HOMxDIAL_Args_Cookie_t);

	HOMAPI_Call( eHOMAPI_Service_Dial, &data);

	return ERR_OK;
#endif
}

HERROR	HOMAPI_DIAL_GetDialStatus(HBOOL * pDialActivated)
{
#if !defined(CONFIG_HOMMA_DIAL)
	HxLOG_Error("### (HOMAPI_DIAL_GetDialStatus) Not Support DIAL ###\n");
	return ERR_FAIL;
#else
	string_container_t container = {0,};
	HERROR err = ERR_FAIL;

	CHECK_DIAL_MODULE_INIT;

	err = __query(eHOMAPI_DIAL_Command_IsRunning, NULL, 0, &container);
	if (err != ERR_OK) {
		return err;
	}

	if (container.str) {
		if (!strcmp(container.str, "YES")) {
			*pDialActivated = TRUE;
		} else {
			*pDialActivated = FALSE;
		}
		HOMAPI_MEMFREE(container.str);
	}

	return err;
#endif
}

HERROR	HOMAPI_DIAL_GetFriendlyName(HCHAR * pFriendlyName, HUINT32 size)
{
#if !defined(CONFIG_HOMMA_DIAL)
	HxLOG_Error("### (HOMAPI_DIAL_GetFriendlyName) Not Support DIAL ###\n");
	return ERR_FAIL;
#else
	string_container_t container = {0,};
	HERROR err = ERR_FAIL;

	CHECK_DIAL_MODULE_INIT;

	err = __query(eHOMAPI_DIAL_Command_GetFriendlyName, NULL, 0, &container);
	if (err != ERR_OK)
	{
		return err;
	}

	if (container.str)
	{
		snprintf(pFriendlyName, size, "%s", container.str);
		HOMAPI_MEMFREE(container.str);
	}

	return err;
#endif
}

HERROR	HOMAPI_DIAL_GetAdditionalDataUrl(HCHAR * pAdditionalDataUrl, HUINT32 size, HCHAR * pAppName)
{
#if !defined(CONFIG_HOMMA_DIAL)
	HxLOG_Error("### (HOMAPI_DIAL_GetAdditionalDataUrl) Not Support DIAL ###\n");
	return ERR_FAIL;
#else
	string_container_t container = {0,};
	HERROR err = ERR_FAIL;

	CHECK_DIAL_MODULE_INIT;

	err = __query(eHOMAPI_DIAL_Command_GetAdditionalDataUrl,
						 pAppName, HLIB_STD_StrNLen(pAppName, 128) + 1, &container);

	if (err != ERR_OK)
	{
		return err;
	}

	if (container.str)
	{
		snprintf(pAdditionalDataUrl, size, "%s", container.str);
		HOMAPI_MEMFREE(container.str);
	}

	return err;
#endif
}
