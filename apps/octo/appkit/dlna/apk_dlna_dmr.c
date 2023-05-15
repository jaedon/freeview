/*******************************************************************
	File Description
********************************************************************/
/**
	@file     apk_dlna_dmr.c
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
#include 	<dlib.h>
#include 	<homapi.h>
#include 	<homapi_dmr.h>
#include 	<apk_meta_record.h>


#if defined(CONFIG_OP_JAPAN)
#define DMR_DEFAULT_FRIENDLYNAME	"WA-7000"
#else
#define DMR_DEFAULT_FRIENDLYNAME	"HMS-1000SPH2"
#endif

#define ________________Static_Variables___________________________
static HxList_t	*s_pst_dmr_cblist;

#define ________________Internal_Functions_________________________
STATIC void	apk_dlnadmr_freeEventCb(HINT32 nArgc, void *apArgV[])
{
	void	*pvData;

	HxLOG_Assert(nArgc == 4);

	pvData = apArgV[2];
	if (pvData)
		APK_Free(pvData);
}

static HERROR ___event_cb(HUINT32 p1, HUINT32 p2, HUINT32 p3)
{
	APKE_DLNA_DMR_Property_e eType = eDLNA_DMR_UnknownType;
	HERROR		hErr	= ERR_FAIL;
	void	*pResultData = NULL;
	HOMxDMR_Event_e cmd = p1;

	switch(cmd)
	{
		case eHOMAPI_DMR_Event_SetUri:
			{
				APKS_DLNA_DMR_ReqSetUriInfo_t	*pContentInfo = NULL;
				HxLOG_Debug("[%s:%d] eHOMAPI_DMR_Event_SetUri \n", __func__, __LINE__);
				pContentInfo = ( APKS_DLNA_DMR_ReqSetUriInfo_t* )APK_Calloc( sizeof( APKS_DLNA_DMR_ReqSetUriInfo_t ) );
				if(pContentInfo != NULL)
				{
					HOMxComm_CdsInfo_t *p = (HOMxComm_CdsInfo_t *)p2;

					HxSTD_MemSet(pContentInfo, 0x00, sizeof(APKS_DLNA_DMR_ReqSetUriInfo_t));
					if(p != NULL)
					{
						pContentInfo->eMediaType 	= p->eItemType;
						pContentInfo->nDuration 	= p->Duration;
						pContentInfo->bCopyControl 	= p->dtcpenable;
						/* pContentInfo->nPlaySpeed */
						HxSTD_snprintf(pContentInfo->szObjectID, sizeof(pContentInfo->szObjectID), "%s", p->ID);
						HxSTD_snprintf(pContentInfo->szMediaUri, sizeof(pContentInfo->szMediaUri), "%s", p->URI);
						HxSTD_snprintf(pContentInfo->szTitle, sizeof(pContentInfo->szTitle), "%s", p->Title);
						HxSTD_snprintf(pContentInfo->szThumbnailUri, sizeof(pContentInfo->szThumbnailUri), "%s", p->ThumbnailURI);
		//				HxSTD_snprintf(pContentInfo->szDate, sizeof(pContentInfo->szDate), "%s", p->szDate);
						HxSTD_snprintf(pContentInfo->szResolution, sizeof(pContentInfo->szResolution), "%s", p->resolution);
						HxSTD_snprintf(pContentInfo->szGenre, sizeof(pContentInfo->szGenre), "%s", p->Genre);
						HxSTD_snprintf(pContentInfo->szAlbum, sizeof(pContentInfo->szAlbum), "%s", p->AlbumTitle);
						HxSTD_snprintf(pContentInfo->szArtist, sizeof(pContentInfo->szArtist), "%s", p->Artist);

						if(!HxSTD_StrNCmp(p->URI, "http://", HxSTD_StrLen("http://")))
						{
							HxLOG_Debug ( "[%s:%s:%d] \n", __FILE__, __FUNCTION__, __LINE__ );
							HxSTD_snprintf( pContentInfo->szMediaUri, sizeof(pContentInfo->szMediaUri), "hdlna://%s_Prot=%s_Size=%ld_Dura=%d_Rate=%d",
									p->URI, p->Protocol, (long int)p->FileSize, p->Duration, p->Bitrate);
						}

						pResultData = (void *)pContentInfo;
					}
					else
					{
						HxLOG_Error("[%s:%d] Error> Not received content data. \n", __HxFILE__, __HxLINE__);
						pResultData = (void *)pContentInfo;
					}
				}
				else
				{
					HxLOG_Error("[%s:%d] Error> fail memory alloc.  \n", __HxFILE__, __HxLINE__);
				}

				eType = eDLNA_DMR_RequestSetUri;
			}
			break;

		case eHOMAPI_DMR_Event_Play:
			{
				if (0x0 != p2) {
					HOMxDMR_Event_Play_t 		*evt = (HOMxDMR_Event_Play_t *)p2;
					APKS_DLNA_DMR_ReqPlayInfo_t *tmp = APK_Calloc(sizeof( APKS_DLNA_DMR_ReqPlayInfo_t));
					if(tmp)
					{
						tmp->nPlaySpeed = evt->nPlaySpeed;
						pResultData = (void *)tmp;
					}

					eType = eDLNA_DMR_RequestPlay;
				}
			}
			break;

		case eHOMAPI_DMR_Event_Stop:
			{
				eType = eDLNA_DMR_RequestStop;
			}
			break;

		case eHOMAPI_DMR_Event_Pause:
			{
				eType = eDLNA_DMR_RequestPause;
			}
			break;

		case eHOMAPI_DMR_Event_Seek:
			{
				if (0x0 != p2)	{
					HOMxDMR_Event_Seek_t *evt = (HOMxDMR_Event_Seek_t *)p2;
					APKS_DLNA_DMR_ReqSeekInfo_t *tmp = APK_Calloc(sizeof(APKS_DLNA_DMR_ReqSeekInfo_t));
					if(tmp)
					{
						tmp->nPosition = evt->nPosition;
						pResultData = (void *)tmp;
						eType = eDLNA_DMR_RequestSeek;
					}
				}
			}
			break;

		case eHOMAPI_DMR_Event_Volume:
			{
				if (0x0 != p2) {
					HOMxDMR_Event_Volume_t *evt = (HOMxDMR_Event_Volume_t *)p2;
					APKS_DLNA_DMR_ReqSetVolumeInfo_t *tmp = APK_Calloc(sizeof( APKS_DLNA_DMR_ReqSetVolumeInfo_t));
					if(tmp)
					{
						tmp->nVolume = evt->nVolume;
						pResultData = (void *)tmp;
						eType = eDLNA_DMR_RequestSetVolume;
					}
				}
			}
			break;

		case eHOMAPI_DMR_Event_Mute:
			{
				if(p2 != 0x0)		// tmp->bMute = evt->bMute; crash
				{
					HOMxDMR_Event_Mute_t *evt = (HOMxDMR_Event_Mute_t *)p2;
					APKS_DLNA_DMR_ReqSetMuteInfo_t* tmp = APK_Calloc(sizeof(APKS_DLNA_DMR_ReqSetMuteInfo_t));
					if(tmp)
					{
						tmp->bMute = evt->bMute;
						pResultData = (void *)tmp;
						eType = eDLNA_DMR_RequestSetMute;
					}
				}
			}
			break;

		case eHOMAPI_DMR_Event_BringIn:
			{
				eType = eDLNA_DMR_RequestBringIn;
			}
			break;

		default :
			HxLOG_Error("Error> Unsupported event(%d)!\n", cmd);
			return ERR_FAIL;
	}

	if ( s_pst_dmr_cblist )
	{
		HxList_t 		*item =  s_pst_dmr_cblist;
		APKS_DLNA_CbHandler pfnObjectHandler = NULL;
		APK_EVENT_EMIT_ARGS	stApkEvtArgs;

		while ( item )
		{
			pfnObjectHandler = (APKS_DLNA_CbHandler)HLIB_LIST_Data(item);
			if ( pfnObjectHandler )
			{
				HxSTD_MemSet(&stApkEvtArgs, 0, sizeof(APK_EVENT_EMIT_ARGS));
				switch (eType)
				{
					case eDLNA_DMR_RequestSetUri:
						stApkEvtArgs.pfnFreeFunc = apk_dlnadmr_freeEventCb;
						stApkEvtArgs.ulArgCount = 4;
						stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
						stApkEvtArgs.apArgV[1] = (void *)eType;
						stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(pResultData, sizeof(APKS_DLNA_DMR_ReqSetUriInfo_t));
						stApkEvtArgs.apArgV[3] = (void *)1;
						APK_EVENT_Emit(&stApkEvtArgs);
						break;

					case eDLNA_DMR_RequestPlay:
						stApkEvtArgs.pfnFreeFunc = apk_dlnadmr_freeEventCb;
						stApkEvtArgs.ulArgCount = 4;
						stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
						stApkEvtArgs.apArgV[1] = (void *)eType;
						stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(pResultData, sizeof(APKS_DLNA_DMR_ReqPlayInfo_t));
						stApkEvtArgs.apArgV[3] = (void *)1;
						APK_EVENT_Emit(&stApkEvtArgs);
						break;

					case eDLNA_DMR_RequestStop:
					case eDLNA_DMR_RequestPause:
						stApkEvtArgs.pfnFreeFunc = NULL;
						stApkEvtArgs.ulArgCount = 4;
						stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
						stApkEvtArgs.apArgV[1] = (void *)eType;
						stApkEvtArgs.apArgV[2] = (void *)NULL;
						stApkEvtArgs.apArgV[3] = (void *)0;
						APK_EVENT_Emit(&stApkEvtArgs);
						break;

					case eDLNA_DMR_RequestSeek:
						stApkEvtArgs.pfnFreeFunc = apk_dlnadmr_freeEventCb;
						stApkEvtArgs.ulArgCount = 4;
						stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
						stApkEvtArgs.apArgV[1] = (void *)eType;
						stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(pResultData, sizeof(APKS_DLNA_DMR_ReqSeekInfo_t));
						stApkEvtArgs.apArgV[3] = (void *)1;
						APK_EVENT_Emit(&stApkEvtArgs);
						break;

					case eDLNA_DMR_RequestSetVolume:
						stApkEvtArgs.pfnFreeFunc = apk_dlnadmr_freeEventCb;
						stApkEvtArgs.ulArgCount = 4;
						stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
						stApkEvtArgs.apArgV[1] = (void *)eType;
						stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(pResultData, sizeof(APKS_DLNA_DMR_ReqSetVolumeInfo_t));
						stApkEvtArgs.apArgV[3] = (void *)1;
						APK_EVENT_Emit(&stApkEvtArgs);
						break;

					case eDLNA_DMR_RequestSetMute:
						stApkEvtArgs.pfnFreeFunc = apk_dlnadmr_freeEventCb;
						stApkEvtArgs.ulArgCount = 4;
						stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
						stApkEvtArgs.apArgV[1] = (void *)eType;
						stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(pResultData, sizeof(APKS_DLNA_DMR_ReqSetMuteInfo_t));
						stApkEvtArgs.apArgV[3] = (void *)1;
						APK_EVENT_Emit(&stApkEvtArgs);
						break;

					case eDLNA_DMR_RequestBringIn: /* Request bringIn SetMediaChange(uri) to webapp */
						stApkEvtArgs.pfnFreeFunc = NULL;
						stApkEvtArgs.ulArgCount = 4;
						stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
						stApkEvtArgs.apArgV[1] = (void *)eType;
						stApkEvtArgs.apArgV[2] = (void *)NULL;
						stApkEvtArgs.apArgV[3] = (void *)0;
						APK_EVENT_Emit(&stApkEvtArgs);
						break;

					default:
						HxLOG_Error("Error> Unsupported event[%d]\n", eType);
						break;
				}
			}

			item = item->next;
		}
	}

	if(pResultData)
		APK_Free(pResultData);

	return hErr;
}

#define ________________Public_Interfaces__________________________
void	APK_DLNADMR_Init( void )
{
	HOMAPI_DMR_RegisterNotifier(___event_cb);
}

void	APK_DLNADMR_DeInit( void )
{
	HOMAPI_DMR_UnRegisterNotifier();
	HOMAPI_DMR_DeInit();
}

void	APK_DLNADMR_RegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler )
{
	HxList_t 		*list = HLIB_LIST_Find( s_pst_dmr_cblist, pfnObjectHandler );

	if ( list )
		HxLOG_Error("pfnObjectHandler(%X) is already registered!!!\n", pfnObjectHandler);
	else
		s_pst_dmr_cblist = HLIB_LIST_Append( s_pst_dmr_cblist, (void *)pfnObjectHandler );
}

void	APK_DLNADMR_UnRegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler )
{
	HxList_t		*list = HLIB_LIST_Find( s_pst_dmr_cblist, pfnObjectHandler );

	if ( NULL == list )
		HxLOG_Error("pfnObjectHandler(%X) does not registered yet..\n", pfnObjectHandler);
	else
		s_pst_dmr_cblist = HLIB_LIST_Remove( s_pst_dmr_cblist, pfnObjectHandler );
}

HERROR	APK_DLNADMR_Start( HCHAR *pFriendlyName )
{
	return HOMAPI_DMR_Start(pFriendlyName);
}

HERROR	APK_DLNADMR_Stop( void )
{
	return HOMAPI_DMR_Stop();
}

HERROR	APK_DLNADMR_GetDefaultFriendlyName( HCHAR* pFriendlyName, HINT32 nLen )
{
	HxSTD_snprintf( pFriendlyName, nLen, "%s", DMR_DEFAULT_FRIENDLYNAME );
	/* Dont save to DB; This will be decided by BPL */
	return ERR_OK;
}

HERROR	APK_DLNADMR_SetFriendlyName( const HCHAR *pFriendlyName )
{
	return HOMAPI_DMR_SetFriendlyName(pFriendlyName);
}

HERROR	APK_DLNADMR_SetBringInUri(const HCHAR *pUrl)
{
	HCHAR szFullPath[DxRECLIST_URL];
	DxRecListData_t *pRecData = NULL;

	if((pUrl == NULL) || (pUrl[0] == 0))
	{
		HxLOG_Error("[%s:%d] Error> uri is null \n", __FUNCTION__, __LINE__ );
		return ERR_FAIL;
	}

	HxSTD_MemSet(szFullPath, 0x00, DxRECLIST_URL);
	if( HLIB_STD_StrNCaseCmp( pUrl, "hpvr://", HxSTD_StrLen( "hpvr://" ) ) == 0 )
	{
		pRecData = APK_META_RECORD_GetByUrl(pUrl);
		if (pRecData)
		{
			DLIB_Print(DxNAMEOF(DxRecListData_t), NULL, pRecData);
			HxSTD_PrintToStrN(szFullPath, DxRECLIST_URL, "%s%s%s", pRecData->stMeta.szPath, pRecData->stMeta.szFileName, DxEXT_META_FILE);
		}
		else
		{
			HxLOG_Error("[%s:%d] Error> Cant get record_meta(%s)!\n", __FUNCTION__, __LINE__, pUrl);
		}

		if( pRecData != NULL)
		{
			APK_META_RECORD_Release(pRecData);
			pRecData = NULL;
		}
	}

	return HOMAPI_DMR_SetBringInUri(pUrl, szFullPath);
}

HERROR	APK_DLNADMR_SetOperationMode(HINT32 nMode)
{
	return HOMAPI_DMR_SetOperationMode(nMode);
}
