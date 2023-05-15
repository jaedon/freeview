/*******************************************************************
    File Description
********************************************************************/
/**
    @file     apk_dlna_rc.c
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
* MS949 encoding ???¨Ïö©?òÎäî source file ?ÖÎãà??
* MS949 encoding?í‰Ωø?®„Åó??source file?ß„Åô??* Quelldatei, die MS949-Codierung verwendet.
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
#include    <homapi_rc.h>

#define ________________Static_Variables___________________________
static HxList_t	*s_pst_rc_cblist;

#define ________________Internal_Functions_________________________
STATIC void	apk_dlnarc_freeEventCb(HINT32 nArgc, void *apArgV[])
{
	HCHAR	*pszData = NULL;
	HCHAR	*pszName = NULL;

	HxLOG_Assert(nArgc == 4);

	pszData = (HCHAR*)apArgV[2];
	pszName = (HCHAR*)apArgV[3];

	if (pszData)
		APK_Free(pszData);

	if (pszName)
		APK_Free(pszName);
}

static HERROR ___event_cb(HUINT32 p1, HUINT32 p2, HUINT32 p3)
{
	APKE_RC_Property_e 	eType = eDLNA_RC_CmdEnd;
	HOMxRC_Event_e		cmd = p1;

	HCHAR					*pszData = NULL;
	HCHAR					*pName = NULL;

	switch ( cmd )
	{
		case eHOMAPI_RC_Event_Started:
			{
				eType = eDLNA_RC_Started;
			}
			break;

		case eHOMAPI_RC_Event_Stopped:
			{
				eType = eDLNA_RC_Stopped;
			}
			break;

		case eHOMAPI_RC_Event_Paired:
			{
				HOMxRC_Events_t *evt = (HOMxRC_Events_t *)p2;

				HxLOG_Error("[%s:%d] eHOMAPI_RC_Event_Paired : %s, client : %s \n", __func__, __LINE__, evt->szData, evt->szName);
				pszData = APK_StrDup(evt->szData);
				pName = APK_StrDup(evt->szName);
				eType = eDLNA_RC_Paired;
			}
			break;

		case eHOMAPI_RC_Event_Unpaired:
			{
				HOMxRC_Events_t *evt = (HOMxRC_Events_t *)p2;

				HxLOG_Error("[%s:%d] eHOMAPI_RC_Event_Unpaired : %s\n", __func__, __LINE__, evt->szData);
				pszData = APK_StrDup(evt->szData);
				eType = eDLNA_RC_Unpaired;
			}
			break;

		case eHOMAPI_RC_Event_Message:
			{
				HOMxRC_Events_t *evt = (HOMxRC_Events_t *)p2;

				HxLOG_Error("[%s:%d] eHOMAPI_RC_Event_Message : %s \n", __func__, __LINE__, evt->szData);
				pszData = APK_StrDup(evt->szData);
				eType = eDLNA_RC_Message;
			}
			break;

		case eHOMAPI_RC_Event_LaunchApp:
			{
				HOMxRC_Events_t *evt = (HOMxRC_Events_t *)p2;

				HxLOG_Error("[%s:%d] eHOMAPI_RC_Event_AppLaunch :: %s \n", __func__, __LINE__, evt->szData);
				pszData = APK_StrDup(evt->szData);
				eType = eDLNA_RC_ReqAppLaunch;
			}
			break;

		case eHOMAPI_RC_Event_SetFriendlyName:
			{
				HOMxRC_Events_t *evt = (HOMxRC_Events_t *)p2;

				HxLOG_Error("[%s:%d] eHOMAPI_RC_Event_SetFriendlyName : %s\n", __func__, __LINE__, evt->szName);
				pName = APK_StrDup(evt->szName);
				eType = eDLNA_RC_SetFriendlyName;
			}
			break;

		default:
			HxLOG_Warning("[%s:%d] can't handle : rc Property (%d) \n", __HxFILE__, __HxLINE__, eType);
			return ERR_FAIL;
	}

	if ( s_pst_rc_cblist )
	{
		HxList_t 		*item =  s_pst_rc_cblist;
		APKS_DLNA_CbHandler	pfnObjectHandler = NULL;

		while ( item )
		{
			pfnObjectHandler = (APKS_DLNA_CbHandler)HLIB_LIST_Data(item);
			if ( pfnObjectHandler )
			{
				APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

				stApkEvtArgs.pfnFreeFunc = apk_dlnarc_freeEventCb;
				stApkEvtArgs.ulArgCount = 4;
				stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
				stApkEvtArgs.apArgV[1] = (void *)eType;
				stApkEvtArgs.apArgV[2] = (void *)(pszData!=NULL ? APK_StrDup(pszData):NULL);
				stApkEvtArgs.apArgV[3] = (void *)(pName !=NULL ? APK_StrDup(pName):NULL);

				APK_EVENT_Emit(&stApkEvtArgs);
			}

			item = item->next;
		}
	}

	if(pszData) 	HLIB_STD_MemFree(pszData);
	if(pName) 	HLIB_STD_MemFree(pName);

	return ERR_OK;
}

#define ________________Public_Interfaces__________________________
void	APK_RC_Init( void )
{
	HOMAPI_RC_Init();
	HOMAPI_RC_RegisterNotifier(___event_cb);
}

void	APK_RC_DeInit( void )
{
	HOMAPI_RC_UnRegisterNotifier();
	HOMAPI_RC_DeInit();
}

void	APK_RC_RegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler )
{
	HxList_t		*list = HLIB_LIST_Find( s_pst_rc_cblist, pfnObjectHandler );

	if ( list )
		HxLOG_Error("pfnObjectHandler(%X) is already registered!!!\n", pfnObjectHandler);
	else
		s_pst_rc_cblist = HLIB_LIST_Append( s_pst_rc_cblist, (void *)pfnObjectHandler );
}

void	APK_RC_UnRegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler )
{
	HxList_t		*list =  HLIB_LIST_Find( s_pst_rc_cblist, pfnObjectHandler );

	if ( NULL == list )
		HxLOG_Error("pfnObjectHandler(%X) does not registered yet..\n", pfnObjectHandler);
	else
		s_pst_rc_cblist = HLIB_LIST_Remove( s_pst_rc_cblist, pfnObjectHandler );
}

HERROR	APK_RC_Start( HCHAR *pFriendlyName )
{
	return HOMAPI_RC_Start(pFriendlyName);
}

HERROR	APK_RC_Stop( void )
{
	return HOMAPI_RC_Stop();
}

HERROR	APK_RC_SetFriendlyName( HCHAR *pFriendlyName )
{
	return HOMAPI_RC_SetFriendlyName(pFriendlyName);
}

HERROR	APK_RC_UnpairClient( void )
{
	return HOMAPI_RC_UnpairClient();
}

