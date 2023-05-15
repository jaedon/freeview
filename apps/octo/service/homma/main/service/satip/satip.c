/**************************************************************
*	@file	satip.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			SATIP
**************************************************************/

/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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
#include "pl_common.h"
#include "pl_satip.h"
#include "satip.h"

#define ________________Typedef____________________________________
typedef enum
{
	eSATIP_STATUS_STOPED,
	eSATIP_STATUS_READY,
	eSATIP_STATUS_START,
	eSATIP_STATUS_UNKNOWN,
} SATIP_Status_e;

typedef struct
{
	CO_NET_Iface_t			tDefIface;
	SATIP_Status_e			eStatus;

	SATIP_EVT_HANDLE_Func	pfCb;
} SATIP_Context_t;

#define ________________Static_Variables___________________________
STATIC	SATIP_Context_t		s_tSatipContext;

#define ________________Internal_Functions_________________________ 
STATIC SATIP_Context_t * __satip_getContext(void)
{
	return &s_tSatipContext;
}

STATIC HERROR __satip_start(const HCHAR *pszFriendlyName)
{
	SATIP_Context_t				*pSatipCtx = __satip_getContext();
	PL_COMMON_ProductInfo_t		product_info;
	CO_NET_Iface_t				*pIface = NULL;
	HCHAR						szFriendlyName[127] = {0,};
	HERROR 						ret = ERR_FAIL;

	PL_COMMON_GetProductInfo(&product_info);

	if ( eSATIP_STATUS_START == pSatipCtx->eStatus )
	{
		HxLOG_Error( "[ SAT>IP ] Already Started SAT>IP !!!!  \n" );
		return ERR_FAIL;
	}

	pSatipCtx->eStatus = eSATIP_STATUS_READY;

	pIface = &pSatipCtx->tDefIface;

	if(HxSTD_StrEmpty(pszFriendlyName))
		HxSTD_snprintf( szFriendlyName, sizeof(szFriendlyName), "%s SAT>IP", product_info.szModelName);
	else
		HxSTD_snprintf( szFriendlyName, sizeof(szFriendlyName), "%s", pszFriendlyName);


	ret = PL_SATIP_Start(szFriendlyName);

	if( ret != ERR_OK )
	{
		HxLOG_Error( "[ SAT>IP ] fail to start SAT>IP !!!!  \n" );
		return ERR_FAIL;
	}
	
	pSatipCtx->eStatus = eSATIP_STATUS_START;

	return ERR_OK;
}

STATIC inline HERROR __satip_stop( void )
{
	SATIP_Context_t *pSatipCtx =  __satip_getContext();

	if ( eSATIP_STATUS_STOPED == pSatipCtx->eStatus )
	{
		HxLOG_Error( "[ SAT>IP ] Already Stopped SAT>IP !!!!\n" );
		return ERR_FAIL;
	}

	PL_SATIP_Stop();

	pSatipCtx->eStatus = eSATIP_STATUS_STOPED;

	return ERR_OK;
}

#define ________________Public_Interfaces__________________________
HERROR	SATIP_OnSystemEvent( HOMMA_BUS_Event_e eEvent, void *eDataType, void *pvData  )
{
	SATIP_Context_t			*pSatipCtx = __satip_getContext();
	CO_NET_Iface_t			*pIfaceInfo = NULL;

	switch ( eEvent )
	{
		case eBUS_EVENT_SYS_NET_IF_ONLINE :
		case eBUS_EVENT_SYS_NET_IF_SWITCHING :	
//			pIfaceInfo = (CO_NET_Iface_t *)pvData;	
//			if ( pIfaceInfo && (eSATIP_STATUS_START != pSatipCtx->eStatus) )
			if ( eSATIP_STATUS_START != pSatipCtx->eStatus )
			{
//				HxLOG_Print( "[ SAT>IP ] SAT>IP Start() by ONLINE Event : %s\n", pIfaceInfo->szName );
//				HxSTD_MemCopy( &pSatipCtx->tDefIface, pIfaceInfo, sizeof(CO_NET_Iface_t) );
				HxLOG_Print( "[ SAT>IP ] SAT>IP Start() by ONLINE Event \n");
				//__satip_start(NULL);
			}
			else {
				PL_SATIP_ChangeBootID();
			}

			break;

		case eBUS_EVENT_SYS_NET_IF_OFFLINE :
			break;

		case eBUS_EVENT_SYS_POWER_REBOOT:
		case eBUS_EVENT_SYS_POWER_SHUTDOWN:
		//case eBUS_EVENT_SYS_POWER_SHUTDOWN:
		case eBUS_EVENT_SYS_MS_FORMAT_TRY:
			{
				HxLOG_Error( "[ SAT>IP ] System Event(%d) - SAT>IP Stop() \n", eEvent);
				__satip_stop();
			}
			break;
		default :
			return ERR_OK;
	}

	return ERR_OK;
}

void 	SATIP_SetEvtListener( SATIP_EVT_HANDLE_Func cbNotify )
{
	SATIP_Context_t *pSatipCtx = __satip_getContext();

	pSatipCtx->pfCb = cbNotify;
}

HERROR	SATIP_SetNetworkInfo( const CO_NET_Info_t *pstNetInfo )
{
	SATIP_Context_t		*pSatipCtx = __satip_getContext();
	HINT32				defIdx = 0;

	HOM_ASSERT(pstNetInfo);

	defIdx = pstNetInfo->nDefIdx;

	if ( pstNetInfo->pstIfaces )
		HxSTD_MemCopy( &pSatipCtx->tDefIface, &pstNetInfo->pstIfaces[defIdx], sizeof(CO_NET_Iface_t) );

	return ERR_OK;
}

HERROR	SATIP_Start( const HCHAR *pszFriendlyName)
{
	PL_COMMON_NetworkInfo_t	net_info;
	PL_COMMON_GetDefaultNetwork(&net_info);

	if(net_info.ulIpAddr == 0) {
		HxLOG_Error("[%s:%d] Error> network information request is faild. \n", __func__, __LINE__);
		return ERR_FAIL;
	}

	return  __satip_start(pszFriendlyName);	
}

HERROR	SATIP_Stop( void )
{
	return  __satip_stop();
}

HBOOL	SATIP_IsStart(void)
{
	SATIP_Context_t *pSatipCtx = __satip_getContext();

	if ( eSATIP_STATUS_START != pSatipCtx->eStatus )
		return FALSE;

	return TRUE;
}

HERROR	SATIP_SetFriendlyName(HCHAR *pszFriendlyName)
{
	return PL_SATIP_SetFriendlyName(pszFriendlyName);
}

#define _____END_OF_FILE_________

