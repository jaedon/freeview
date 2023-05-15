/*************************************************************************************************************
	File 		: woons.c
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	:
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (징째Humax징짹) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED 징째AS IS징짹 AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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
#include "homma_bus.h"
#include "pl_common.h"
#include "pl_woons.h"
#include "pl_dms.h"
#include "woons.h"

#define SAFE_DELETE(p)			{if(p){ HLIB_STD_MemFree(p); (p)=NULL;}}

typedef struct
{
	HBOOL						isStarted;
	PL_WOONS_DmsInfo_t			stLocalDmsInfo;
	WOONS_EVENT_CALLBACK		cbListener;
}woonserver_context_t;

#define ________________static_Variables___________________________

static woonserver_context_t 				s_woonserver_context = {
											FALSE,
											{NULL, NULL, NULL, ""},
											NULL};

#define ________________Internal_APIs_Prototype____________________


#define ________________Internal_Functions_________________________
static WoonServer_ModelInfo_t*	__make_model_info(const char *szFriendlyName)
{
	WoonServer_ModelInfo_t *pstCopyModeInfo = HLIB_STD_MemCalloc(sizeof(WoonServer_ModelInfo_t));
	PL_DMS_MXDLNA_HANDLE_t *pDmsHandle = HLIB_STD_MemCalloc(sizeof(PL_DMS_MXDLNA_HANDLE_t));

	if(PL_DMS_Get_MxDlna_Res(pDmsHandle) == ERR_OK)
	{
		pstCopyModeInfo->hDms 	= pDmsHandle->hDms;
		pstCopyModeInfo->hDmsDb = pDmsHandle->hDmsDB;
		pstCopyModeInfo->hFs	= pDmsHandle->hFSS;
	}
	else
	{
		HxLOG_Debug("[%s:%d] Error> Cant get dlna handle!\n", __FUNCTION__, __LINE__ );
		return NULL;
	}

	if(NULL == pstCopyModeInfo->hDms|| NULL == pstCopyModeInfo->hDmsDb || NULL == pstCopyModeInfo->hFs)
	{
		HxLOG_Debug("[%s:%d] Error> DMS is NOT started .. !! \n", __FUNCTION__, __LINE__);
		SAFE_DELETE(pstCopyModeInfo);
		return NULL;
	}

	HxSTD_MemSet(pstCopyModeInfo->szServerName, 0x00, sizeof(char)*eWOONS_MAX_SRVNAME_LEN);

	if(HxSTD_StrEmpty(szFriendlyName))
	{
		PL_COMMON_ProductInfo_t		product_info;

		HxSTD_MemSet( &product_info, 0x00, sizeof( PL_COMMON_ProductInfo_t ) );
		PL_COMMON_GetProductInfo(&product_info);
		HxSTD_StrNCpy(pstCopyModeInfo->szServerName, product_info.szModelName, HxSTD_StrLen(product_info.szModelName));
	}
	else
		HxSTD_StrNCpy(pstCopyModeInfo->szServerName, szFriendlyName, HxSTD_StrLen(szFriendlyName));

	return pstCopyModeInfo;
}

static void __init_context(WoonServer_ModelInfo_t *pstModeInfo)
{
	HINT16 nSrvNameLength=0;

	s_woonserver_context.stLocalDmsInfo.hDms 	= pstModeInfo->hDms;
	s_woonserver_context.stLocalDmsInfo.hDmsDB 	= pstModeInfo->hDmsDb;
	s_woonserver_context.stLocalDmsInfo.hFs		= pstModeInfo->hFs;

	nSrvNameLength = HxSTD_StrLen(pstModeInfo->szServerName);
	if(nSrvNameLength > eWOONS_MAX_SRVNAME_LEN)
		nSrvNameLength = eWOONS_MAX_SRVNAME_LEN-1;

	HxSTD_MemSet(s_woonserver_context.stLocalDmsInfo.szServerName, 0x00, eWOONS_MAX_SRVNAME_LEN);
	HxSTD_StrNCpy(s_woonserver_context.stLocalDmsInfo.szServerName, pstModeInfo->szServerName, nSrvNameLength);
}

static woonserver_context_t* __context(void)
{
	return &s_woonserver_context;
}

static void __notify(WOONS_Event_e  eEvt, HINT32 p1, HINT32 p2, HINT32 p3)
{
	woonserver_context_t *ctx=	__context();

	if(ctx->cbListener != NULL)
		ctx->cbListener(eEvt, p1, p2, p3);
}

static void	__woons_evt_cb(PL_WOONS_EVENT_e type, PL_WOONS_ERROR_CODE_e error, void *data)
{
	switch(type)
	{
		case PL_MXRAS_EVENT_WOONSTART_FAILED:
			HxLOG_Debug("### PL_MXRAS_EVENT_WOONSTART_FAILED ### \n");
			/* 서버 스타트 정상 유무(Debug) 전달 */
			break;

		case PL_MXRAS_EVENT_PAIRING_INIT:
			HxLOG_Print("### PL_MXRAS_EVENT_PAIRING_INIT(%d-error:%d) - (%s) ### \n", type, error, (HCHAR *)data);
			{
				HBOOL bOpen = FALSE;
				if (error == PL_MXRAS_EVENT_OK) {
					bOpen = FALSE;
				}
				else {
					bOpen = TRUE;
				}
				__notify(WOONSERVER_NOTIFY_PINCODE, bOpen, (HINT32)data, 0);
			}
			break;

		case PL_MXRAS_EVENT_NOTIFICATION:
			HxLOG_Debug("### PL_MXRAS_EVENT_NOTIFICATION ### \n");
			__notify(WOONSERVER_NOTIFY_PINCODE, TRUE, (HINT32)data, 0);
			break;

		case PL_MXRAS_EVENT_NETWORK_CS_PUBLIC_IP:
		case PL_MXRAS_EVENT_NETWORK_REVERSE_CONNECTION:
			HxLOG_Debug("### PL_MXRAS_EVENT_NETWORK_REVERSE_CONNECTION ### \n");
			__notify(WOONSERVER_TESTCONNECTION_RESULT, TRUE, (HINT32)error, 0);
			break;

		case PL_MXRAS_EVENT_STREAMMING_SEND:
			HxLOG_Debug("### PL_MXRAS_EVENT_STREAMMING_SEND ### \n");
			/*  스트리밍 중이라면 Panel 에 표시 하기 위해 이벤트 전달  */
			break;

		case PL_MXRAS_EVENT_WOONSERVER_START:
			HxLOG_Debug("### PL_MXRAS_EVENT_WOONSERVER_START ### \n");
			__notify(WOONSERVER_STARTED, FALSE, (HINT32)0, 0);
			break;

		case PL_MXRAS_EVENT_WOONSERVER_STOP:
			HxLOG_Debug("### PL_MXRAS_EVENT_WOONSERVER_STOP ### \n");
			__notify(WOONSERVER_STOPPED, FALSE, (HINT32)0, 0);
			break;

		case PL_MXRAS_EVENT_WOONSERVER_RENAME:
			HxLOG_Debug("### PL_MXRAS_EVENT_WOONSERVER_RENAME ### \n");
			__notify(WOONSERVER_RENAMED, 0, (HINT32)0, 0);
			break;

		default:
			break;
	}
}

static void	__woons_collector_cb(PL_WOONS_NOTIFY_e notification_code)
{
	/*  not used  */
}

#define ________________Public_Interfaces__________________________

#if 0
HERROR WOONS_Init(void)
{
	PL_WOONS_Init();
	return ERR_OK;
}

HERROR WOONS_Deinit(void)
{
	woonserver_context_t *pCtx = __context();

	if(TRUE == pCtx->isStarted)
	{
		if(ERR_OK == PL_WOONS_Stop())
			pCtx->isStarted = FALSE;
	}
	else
	{
		HxLOG_Debug("[%s] Already Stopped....\n", __FUNCTION__);
	}

	PL_WOONS_Deinit();

	return ERR_OK;
}
#endif

HBOOL	WOONS_IsStarted(void)
{
	return PL_WOONS_IsStarted();
}

void	WOONS_RegisterEventCallback(WOONS_EVENT_CALLBACK apEventListener)
{
	woonserver_context_t *pCtx = __context();
	pCtx->cbListener = apEventListener;
}

HERROR 	WOONS_Start(const char *szFriendlyName, const char *szMac)
{
	woonserver_context_t *pCtx = NULL;
	WoonServer_ModelInfo_t *pstCopyModeInfo = NULL;

	pCtx =	__context();

	if(PL_WOONS_IsStarted())
	{
		HxLOG_Error("Error> Already started\n");
		__notify(WOONSERVER_STARTED, TRUE, (HINT32)0, 0);
		return ERR_FAIL;
	}

	pstCopyModeInfo = __make_model_info(szFriendlyName);
	if(pstCopyModeInfo == NULL)
	{
		HxLOG_Error("Error> There was problem to init WOONS Mode\n");
		return ERR_FAIL;
	}

	__init_context(pstCopyModeInfo);

	PL_WOONS_AddEventCallback(__woons_evt_cb);
	PL_WOONS_AddCollectorCallback(__woons_collector_cb);

	HxSTD_snprintf(pCtx->stLocalDmsInfo.szPairingId, sizeof(pCtx->stLocalDmsInfo.szPairingId), "%s", szMac +1);
	PL_WOONS_Start(&pCtx->stLocalDmsInfo);

	pCtx->isStarted = TRUE;
	return ERR_OK;
}

HERROR 	WOONS_Stop(void)
{
	woonserver_context_t *pCtx = NULL;
	pCtx =	__context();

	if(pCtx->isStarted != TRUE)
	{
		HxLOG_Error("Error> Already stopped\n");
		__notify(WOONSERVER_STOPPED, TRUE, (HINT32)0, 0);
		return ERR_OK;
	}

	return PL_WOONS_Stop();
}

HERROR 	WOONS_Rename(HCHAR *pszServerName)
{
	return PL_WOONS_RenameServer(pszServerName);
}

HERROR WOONS_TestConnection(void)
{
	PL_WOONS_TestConnection();
	return ERR_OK;
}

HERROR WOON_GetServerInfo(WOON_SERVER_INFO_t *pInfo)
{
	PL_WOONS_INFO_t *p;

	p = PL_WOONS_GetServerInfo();
	HxSTD_MemSet(pInfo, 0x00, sizeof(WOON_SERVER_INFO_t));
	memcpy(pInfo, p, sizeof(WOON_SERVER_INFO_t));
	return ERR_OK;
}

/*  For Debug Command Tool  */
HCHAR*	 WOONS_GetLocalIp(void)
{
	return PL_WOONS_GetLocalIp();
}

HCHAR*	WOONS_GetExternalIp(void)
{
	return PL_WOONS_GetExternalIp();
}

HCHAR*	WOONS_GetPublicIp(void)
{
	return PL_WOONS_GetPublicIp();
}

HINT16	WOONS_GetExternalPort(void)
{
	return PL_WOONS_GetExternalPort();
}

HINT16 	WOONS_GetPublicPort(void)
{
	return PL_WOONS_GetPublicPort();
}

HCHAR* 	WOONS_GetWoonId(void)
{
	return PL_WOONS_GetWoonId();
}

HERROR	WOONS_AddWoonId(HCHAR *pszWid)
{
	return PL_WOONS_AddWoonID(pszWid, NULL);
}

HERROR	WOONS_DelWoonId(HCHAR *pszWid)
{
	return PL_WOONS_DelWoonID(pszWid);
}

void 	WOONS_GetDeviceList(void)
{
	PL_WOONS_GetDeviceList();
}

void	WOONS_ShowNetworkInfo(void)
{
	PL_WOONS_ShowNetworkInfo();
}

void 	WOONS_ShowWidList(void)
{
	PL_WOONS_GetWoonIDList();
}
/* end of file */

