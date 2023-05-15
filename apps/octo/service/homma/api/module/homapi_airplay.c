/**************************************************************
*	@file	homapi_airplay.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			AIRPLAY
**************************************************************/

/*
* 2011-2012 Humax Co., Ltd.
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
#define CHECK_AIRPLAY_MODULE_INIT	do { HxLOG_Warning("\n"); if(airplay_IsInitialized() == FALSE) HOMAPI_AIRPLAY_Init(); } while (0)

#define ________________Typedef____________________________________
typedef struct
{
	HBOOL		bInit;

	HUINT32		accesscode;
	HUINT32		notifierId;

	HOMAPI_EventCallback cb;
} airplay_context_t;

#define ________________Static_Variables___________________________
static  HUINT32     _ulModuleSemaphoreId = 0;

#define ________________Internal_Functions_________________________
static airplay_context_t *__ctx( void )
{
	static airplay_context_t s_manager;
	return &s_manager;
}

static HBOOL	airplay_IsInitialized(void)
{
	airplay_context_t 	*mgr = __ctx();

	if(mgr == NULL)
	{
		HxLOG_Error("mgr == NULL\n");
		return FALSE;
	}

	return mgr->bInit;
}

static HERROR 	airplay_DecodeResultEvent ( HUINT32	cmd, void *data, HUINT32 size)
{
	airplay_context_t 	*mgr = __ctx();
	HERROR			hErr = ERR_FAIL;
	void			*pResultData = NULL;

	switch(cmd)
	{
		case eHOMAPI_AIRPLAY_OP_PLAY:
			pResultData = HLIB_STD_MemDup(data, size);
			HxLOG_Debug("Receive Play requeat at homapi\n");
			break;
		case eHOMAPI_AIRPLAY_OP_STOP:
			pResultData = HLIB_STD_MemDup(data, size);
			HxLOG_Debug("Receive Stop requeat at homapi\n");
			break;
		case eHOMAPI_AIRPLAY_OP_SHOWPHOTO:
			pResultData = HLIB_STD_MemDup(data, size);
			HxLOG_Debug("Receive Show Photo requeat at homapi\n");
			break;
		case eHOMAPI_AIRPLAY_OP_SETPOSITION:
			pResultData = HLIB_STD_MemDup(data, size);
			HxLOG_Debug("Receive Setposition requeat at homapi\n");
			break;
		case eHOMAPI_AIRPLAY_OP_SETRATE:
			pResultData = HLIB_STD_MemDup(data, size);
			HxLOG_Debug("Receive Setrate requeat at homapi\n");
			break;
		case eHOMAPI_AIRPLAY_OP_GET_PLAYBACK_INFO:
			pResultData = HLIB_STD_MemDup(data, size);
			HxLOG_Debug("Receive playback info request at homapi\n");
			break;
		case eHOMAPI_AIRTUNES_OP_PLAY:
			pResultData = HLIB_STD_MemDup(data, size);
			HxLOG_Debug("Receive Airtunes Play request at homapi\n");
			break;
		case eHOMAPI_AIRTUNES_OP_FLUSH:
			pResultData = HLIB_STD_MemDup(data, size);
			HxLOG_Debug("Receive Receive Airtunes Flush request at homapi\n");
			break;
		case eHOMAPI_AIRTUNES_OP_STOP:
			pResultData = HLIB_STD_MemDup(data, size);
			HxLOG_Debug("Receive Receive Airtunes Stop request at homapi\n");
			break;
		case eHOMAPI_AIRTUNES_OP_SET_METADATA:
			pResultData = HLIB_STD_MemDup(data, size);
			HxLOG_Debug("Receive Receive Airtunes Set Metadata request at homapi\n");
			break;
		case eHOMAPI_AIRTUNES_OP_SET_ALBUMART:
			pResultData = HLIB_STD_MemDup(data, size);
			HxLOG_Debug("Receive Receive Airtunes Set Albumart request at homapi\n");
			break;
		default:
			break;
	}

	if (mgr->cb)
		 mgr->cb( (HUINT32)cmd, (HUINT32)pResultData, 0 );

	HOMAPI_MEMFREE(pResultData);

	return hErr;
}

static HBOOL airplay_QueryResultEventHandler( HUINT32 cmd, void *param, HUINT32 size, void *userdata )
{
	return  airplay_DecodeResultEvent( cmd, param, size);
}

static	HUINT32	airplay_Notifier(HUINT32 accesscode, HUINT32 cmd, void *param, HUINT32 size)
{
	airplay_context_t *mgr = __ctx();

	HOMAPI_SEM_GET;
	if ( NULL == param )
		HOMAPI_Query( eHOMAPI_Service_AIRPLAY, NULL, mgr, (HBOOL (*)(HUINT32, void *, HUINT32, void *))airplay_QueryResultEventHandler );
	else
		airplay_DecodeResultEvent(cmd, param, size);

	if ( mgr->accesscode != accesscode )
		HxLOG_Debug("accesscode in mgr (%d) not equal to cur accesscode (%d) \n", mgr->accesscode, accesscode);

	HOMAPI_SEM_RELEASE;

	return 0;
}

static	HERROR	airplay_RegisterNotifier( void )
{
	airplay_context_t *mgr = __ctx();
	HUINT32 	accesscode = 0;

	if(mgr == NULL)
	{
		HxLOG_Error("mgr == NULL\n");
		return ERR_FAIL;
	}

	mgr->bInit = FALSE;
	accesscode = HOMAPI_Query( eHOMAPI_Service_AIRPLAY, NULL, NULL,  NULL );

	if( accesscode )
	{
		mgr->accesscode = accesscode;
		mgr->notifierId = HOMAPI_ConnectNotifier( accesscode, airplay_Notifier );
		mgr->bInit = TRUE;

		return ERR_OK;
	}

	return ERR_FAIL;
}

static	void	airplay_UnRegisterNotifier( void )
{
	airplay_context_t *mgr = __ctx();

	if ( mgr )
		mgr->notifierId = HOMAPI_DisconnectNotifier( mgr->accesscode, mgr->notifierId );
}

#define ________________Public_Interfaces__________________________
void	HOMAPI_AIRPLAY_Init( void )
{
#if !defined(CONFIG_HOMMA_AIRPLAY)
	HxLOG_Error("### (HOMAPI_AIRPLAY_Init) Not Support AIRPLAY ###\n");
	return;
#else
	if(_ulModuleSemaphoreId == 0)
	{
		HxSEMT_Create(&_ulModuleSemaphoreId, "hom_airplay", HxSEMT_FIFO);
	}

	if(airplay_RegisterNotifier() != ERR_OK)
	{
		HxLOG_Error("[HOMAPI_AIRPLAY_Init] airplay_RegisterNotifier() fail!\n");
	}
#endif
}

void	HOMAPI_AIRPLAY_DeInit( void )
{
#if !defined(CONFIG_HOMMA_AIRPLAY)
	HxLOG_Error("### (HOMAPI_AIRPLAY_DeInit) Not Support AIRPLAY ###\n");
	return;
#else
	airplay_UnRegisterNotifier();

	if(_ulModuleSemaphoreId != 0)
	{
		HxSEMT_Destroy(_ulModuleSemaphoreId);
		_ulModuleSemaphoreId = 0;
	}
#endif
}

void	HOMAPI_AIRPLAY_RegisterNotifier(HOMAPI_EventCallback pfnObjectHandler)
{
#if !defined(CONFIG_HOMMA_AIRPLAY)
	HxLOG_Error("### (HOMAPI_AIRPLAY_RegisterNotifier) Not Support AIRPLAY ###\n");
	return;
#else
	airplay_context_t *mgr = __ctx();
	mgr->cb = pfnObjectHandler;
#endif
}

void	HOMAPI_AIRPLAY_UnRegisterNotifier( void )
{
#if !defined(CONFIG_HOMMA_AIRPLAY)
	HxLOG_Error("### (HOMAPI_AIRPLAY_UnRegisterNotifier) Not Support AIRPLAY ###\n");
	return;
#else
	airplay_context_t * mgr = __ctx();
	mgr->cb = NULL;
#endif
}

HERROR	HOMAPI_AIRPLAY_Start( void )
{
#if !defined(CONFIG_HOMMA_AIRPLAY)
	HxLOG_Error("### (HOMAPI_AIRPLAY_Start) Not Support AIRPLAY ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;

	CHECK_AIRPLAY_MODULE_INIT;

	data.cmd = eHOMAPI_AIRPLAY_Command_Start;
	data.data = NULL;
	data.size = 0;

	HOMAPI_Call( eHOMAPI_Service_AIRPLAY, &data);
	return ERR_OK;
#endif
}

HERROR	HOMAPI_AIRPLAY_Stop( void )
{
#if !defined(CONFIG_HOMMA_AIRPLAY)
	HxLOG_Error("### (HOMAPI_AIRPLAY_Stop) Not Support AIRPLAY ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;

	CHECK_AIRPLAY_MODULE_INIT;

	data.cmd = eHOMAPI_AIRPLAY_Command_Stop;
	data.data = NULL;
	data.size = 0;

	HOMAPI_SEM_GET;

	HOMAPI_Call( eHOMAPI_Service_AIRPLAY, &data);

	HOMAPI_SEM_RELEASE;

	return ERR_OK;
#endif
}

HERROR	HOMAPI_AIRPLAY_SetPlaybackInfo(HOMxAIRPLAY_PLAYBACK_INFO_t tPlaybackInfo)
{
#if !defined(CONFIG_HOMMA_AIRPLAY)
	HxLOG_Error("### (HOMAPI_AIRPLAY_SetPlaybackInfo) Not Support AIRPLAY ###\n");
	return ERR_FAIL;
#else
	HOMxCommData_t data;

	CHECK_AIRPLAY_MODULE_INIT;

	data.cmd = eHOMAPI_AIRPLAY_Command_SetPlaybackInfo;
	data.data = (HINT8 *)&tPlaybackInfo;
	data.size = sizeof(HOMxAIRPLAY_PLAYBACK_INFO_t);

	HOMAPI_SEM_GET;

	HOMAPI_Call( eHOMAPI_Service_AIRPLAY, &data);

	HOMAPI_SEM_RELEASE;

	return ERR_OK;
#endif
}

