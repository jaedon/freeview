/*******************************************************************
    File Description
********************************************************************/
/**
    @file     apk_dlna_satip.c
    @brief

    Description:                                    \n
    Module: APPKITv2            \n

    Copyright (c) 2013 HUMAX Co., Ltd.              \n
    All rights reserved.                            \n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/


#define	________________Header_Files_______________________________
#include	"_apk_int.h"
#include	"apk_dlna.h"
#include	"apk_event.h"
#include	<homapi.h>
#include	<homapi_satip.h>

#ifdef CONFIG_NOVA_SATIP_SERVER
#include "nova_api.h"
#endif

#if defined(CONFIG_OP_JAPAN)
#define SATIP_DEFAULT_FRIENDLYNAME	"WA-7000"
#else
#define SATIP_DEFAULT_FRIENDLYNAME	"HMS-1000SPH2"
#endif

#define ________________Typedef____________________________________
static HxList_t	*s_pst_satip_cblist;

#define ________________Internal_Functions_________________________
STATIC void	apk_satip_freeEventCb(HINT32 nArgc, void *apArgV[])
{
	void	*pvData;

	HxLOG_Assert(nArgc == 4);

	pvData = apArgV[2];

	if (pvData)
		APK_Free(pvData);
}

static HERROR ___event_cb(HUINT32 p1, HUINT32 p2, HUINT32 p3)
{
	HERROR			hErr = ERR_FAIL;
	void			*pResultData = NULL;
	HOMxSATIP_Event_e	 cmd = p1;

	APKE_SATIP_Property_e eType = eSATIP_UnknownType;

	switch(cmd)
	{
		case eHOMAPI_SATIP_Event_Started:
		case eHOMAPI_SATIP_Event_Stopped:
		case eHOMAPI_SATIP_Event_Status:
			break;

		default :
			HxLOG_Error( "[ APK_SATIP ] Error> Unsupported cmd(%d)!\n", cmd);
			break;
	}

	if ( s_pst_satip_cblist )
	{
		HxList_t *item;
		APKS_DLNA_CbHandler pfnObjectHandler;
		//APK_EVENT_EMIT_ARGS	stApkEvtArgs;

		item = s_pst_satip_cblist;
		while ( item )
		{
			pfnObjectHandler = (APKS_DLNA_CbHandler)HLIB_LIST_Data(item);
			if ( pfnObjectHandler )
			{
				//HxSTD_MemSet(&stApkEvtArgs, 0, sizeof(APK_EVENT_EMIT_ARGS));
				switch (eType)
				{
					case eHOMAPI_SATIP_Event_Started:
					case eHOMAPI_SATIP_Event_Stopped:
					case eHOMAPI_SATIP_Event_Status:
					case eDLNA_DMS_GetDmsInfo:
						/*
						stApkEvtArgs.pfnFreeFunc = apk_dlnasatip_freeEventCb;
						stApkEvtArgs.ulArgCount = 4;
						stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
						stApkEvtArgs.apArgV[1] = (void *)eType;
						stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(pResultData, sizeof(APKS_DLNA_DMS_DmsInfo_t));
						stApkEvtArgs.apArgV[3] = (void *)1;
						APK_EVENT_Emit(&stApkEvtArgs);
						*/
						break;


					default:
						HxLOG_Error("[%s:%d] not defined type [%d]\n", __FUNCTION__, __LINE__, eType);
						break;
				}
			}

			item = item->next;
		}
	}

	if ( pResultData )
		APK_Free( pResultData );

	return hErr;
}

#define ________________Public_Interfaces__________________________
void	APK_SATIP_Init( void )
{
	HOMAPI_SATIP_RegisterNotifier(___event_cb);
}

void	APK_SATIP_DeInit( void )
{
	HOMAPI_SATIP_UnRegisterNotifier();
	HOMAPI_SATIP_DeInit();
}

void	APK_SATIP_RegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler )
{
	HxList_t	*list = HLIB_LIST_Find( s_pst_satip_cblist, pfnObjectHandler );

	if ( list )
		HxLOG_Error("pfnObjectHandler(%X) is already registered!!!\n", pfnObjectHandler);
	else
		s_pst_satip_cblist = HLIB_LIST_Append( s_pst_satip_cblist, (void *)pfnObjectHandler );
}

void	APK_SATIP_UnRegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler )
{
	HxList_t	*list = HLIB_LIST_Find( s_pst_satip_cblist, pfnObjectHandler );

	if ( NULL == list )
		HxLOG_Error("pfnObjectHandler(%X) does not registered yet..\n", pfnObjectHandler);
	else
		s_pst_satip_cblist = HLIB_LIST_Remove( s_pst_satip_cblist, pfnObjectHandler );
}

HERROR	APK_SATIP_Start( HCHAR *pFriendlyName)
{
	HERROR hErr = ERR_OK;

#ifdef CONFIG_HOMMA_SATIP_SERVER
	hErr = HOMAPI_SATIP_Start(pFriendlyName);
#endif

#ifdef CONFIG_NOVA_SATIP_SERVER
	hErr = NOVAAPI_SATIP_Start(pFriendlyName);
#endif

	return hErr;
}

HERROR	APK_SATIP_SetFriendlyName( const HCHAR *pFriendlyName)
{
	HERROR hErr = ERR_OK;

#ifdef CONFIG_HOMMA_SATIP_SERVER
	hErr = HOMAPI_SATIP_SetFriendlyName(pFriendlyName);
#endif

#ifdef CONFIG_NOVA_SATIP_SERVER
	hErr = NOVAAPI_SATIP_SetFriendlyName(pFriendlyName);
#endif

	return hErr;
}

HERROR	APK_SATIP_Stop( void )
{
	HERROR hErr = ERR_OK;

#ifdef CONFIG_HOMMA_SATIP_SERVER
	hErr = HOMAPI_SATIP_Stop();
#endif

#ifdef CONFIG_NOVA_SATIP_SERVER
	hErr = NOVAAPI_SATIP_Stop();
#endif

	return hErr;
}
