/**************************************************************************************/
/**
 * @file opldlna.cpp
 *
 * OplDLNA
 *
 * @author  Mun-ho Park(mhpark3@humaxdigital.com)
 * @date    2011/12/08
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/
#include "bpldb.h"
#include "opldmr.h"
#include "oapi.h"
#include "apk.h"
#include <hlib.h>

/* Old feature부분은 hommma내에서 기존과 다르게 구현된 부분으로 관련하여 기능이 통합되면 지워야함. */
#define SUPPORT_OLD_FEATURE 0

OPL_DMR_EventCallback		eventListener	 = NULL;

#define ________________Typedef____________________________________
typedef struct
{
	Opl_DLNA_DmrContent_t		*ptContentInfo;
} OPL_DMR_Context_t;

#define ________________Static_Variables___________________________
static OPL_DMR_Context_t s_DMR_Context = {0,};

#define ____________INTERNEL_OPL_DMR_______________
/* DMR을 위한 Context를 초기화. */
static OPL_DMR_Context_t *__get_context( void )
{
	return &s_DMR_Context;
}

static void __debug_contentinfo( Opl_DLNA_DmrContent_t *ptContentInfo )
{
	HxLOG_Debug("szTitle			: %s \n", ptContentInfo->szTitle);
	HxLOG_Debug("szImportURI		: %s \n", ptContentInfo->szImportURI);
	HxLOG_Debug("szUDN				: %s \n", ptContentInfo->szUDN);
	HxLOG_Debug("szCID		  		: %s \n", ptContentInfo->szCID);
	HxLOG_Debug("szParentId 	  	: %s \n", ptContentInfo->szParentId);
	HxLOG_Debug("szClass		  	: %s \n", ptContentInfo->szClass);
	HxLOG_Debug("szObjectType 		: %s \n", ptContentInfo->szObjectType);
	HxLOG_Debug("szThumbnailURI		: %s \n", ptContentInfo->szThumbnailURI);
	HxLOG_Debug("szDate			  	: %s \n", ptContentInfo->szDate);
	HxLOG_Debug("szDuration 	  	: %s \n", ptContentInfo->szDuration);
	HxLOG_Debug("szResolution		: %s \n", ptContentInfo->szResolution);
	HxLOG_Debug("szDigitalCopyContro: %s \n", ptContentInfo->szDigitalCopyControl);
	HxLOG_Debug("szGenre 		  	: %s \n", ptContentInfo->szGenre);
	HxLOG_Debug("bByteSeekAble 		: %d \n", ptContentInfo->bByteSeekAble);
	HxLOG_Debug("bTimeSeekAble 		: %d \n", ptContentInfo->bTimeSeekAble);
	HxLOG_Debug("ulChildCount 		: %d \n", ptContentInfo->ulChildCount);
	HxLOG_Debug("bRestricted 		: %d \n", ptContentInfo->bRestricted);

	return;
}

#define ____________PUBLIC_OPL_DMR_______________
BPLStatus	OPL_DMR_Start( void )
{
	HCHAR szFriendlyName[128];

	OPL_DMR_Context_t 	*pDmr = __get_context();

	HxSTD_MemSet(szFriendlyName, 0, 128);
	APK_DB_GetStr( USERDB_DLNA_DMR_FRIENDLYNAME, szFriendlyName, 128);

	APK_DLNADMR_Start(szFriendlyName);
	APK_DB_SetInt(USERDB_DLNA_DMR_ACTIVATION, (HUINT32)TRUE );

	return BPL_STATUS_OK;
}

BPLStatus	OPL_DMR_Stop( void )
{
	OPL_DMR_Context_t 	*pDmr = __get_context();

	APK_DLNADMR_Stop();
	APK_DB_SetInt(USERDB_DLNA_DMR_ACTIVATION, (HUINT32)FALSE );

	if(pDmr->ptContentInfo != NULL)
	{
		HLIB_STD_MemFree(pDmr->ptContentInfo);
		pDmr->ptContentInfo = NULL;
	}

	return BPL_STATUS_OK;
}

BPLStatus  	OPL_DMR_ApkListener(HUINT32 p1, HUINT32 p2, HUINT32 p3)
{
	OPL_DMR_Context_t 	*pDmr = __get_context();

	HUINT32 event = eDLNA_DMR_DoNothing, result = eDLNA_DMR_DoNothing;
	HxLOG_Debug("[ OPL_DMR_ApkListener ] received event [%d] \n", p1);

	switch ( p1 )
	{
		case eDLNA_DMR_RequestSetUri :
			//event = eOPL_DMR_REQ_SETURI;
			event = eOPL_DMR_REQ_PLAY;
			{
				APKS_DLNA_DMR_ReqSetUriInfo_t *temp = (APKS_DLNA_DMR_ReqSetUriInfo_t*)p2;
				if(temp != NULL)
				{
					if(pDmr->ptContentInfo == NULL)
						pDmr->ptContentInfo = (Opl_DLNA_DmrContent_t *)HLIB_STD_MemCalloc(sizeof(Opl_DLNA_DmrContent_t));
					else
						HxSTD_MemSet(pDmr->ptContentInfo, 0x00, sizeof(Opl_DLNA_DmrContent_t));

					HxSTD_PrintToStrN(pDmr->ptContentInfo->szCID, 			sizeof(pDmr->ptContentInfo->szCID),  			"%s",	temp->szObjectID);
					HxSTD_PrintToStrN(pDmr->ptContentInfo->szTitle, 		sizeof(pDmr->ptContentInfo->szTitle), 			"%s", 	temp->szTitle);
					//HxSTD_PrintToStrN(playInfo->szUDN, 			sizeof(playInfo->szUDN), 			"%s", 	temp->szUDN);
					//HxSTD_PrintToStrN(playInfo->szClass, 			sizeof(playInfo->szClass),  		"%s",	temp->szClass);
					switch(temp->eMediaType)
					{
						case eDLNA_DMR_MEDIATYPE_Container:
							HxSTD_PrintToStrN(pDmr->ptContentInfo->szObjectType, sizeof(pDmr->ptContentInfo->szObjectType), "%s",  "folder");
							break;
						case eDLNA_DMR_MEDIATYPE_Video:
							HxSTD_PrintToStrN(pDmr->ptContentInfo->szObjectType, sizeof(pDmr->ptContentInfo->szObjectType), "%s",  "video");
							break;
						case eDLNA_DMR_MEDIATYPE_Music:
							HxSTD_PrintToStrN(pDmr->ptContentInfo->szObjectType, sizeof(pDmr->ptContentInfo->szObjectType), "%s",  "music");
							break;
						case eDLNA_DMR_MEDIATYPE_Photo:
							HxSTD_PrintToStrN(pDmr->ptContentInfo->szObjectType, sizeof(pDmr->ptContentInfo->szObjectType), "%s",  "photo");
							break;
						default:
							HxSTD_PrintToStrN(pDmr->ptContentInfo->szObjectType, sizeof(pDmr->ptContentInfo->szObjectType), "%s",  "unknown");
							break;
					}

#if SUPPORT_OLD_FEATURE
					if(HxSTD_StrStr(temp->szMediaUri, "http://") != NULL)
						HxSTD_PrintToStrN(pDmr->ptContentInfo->szImportURI, 	sizeof(pDmr->ptContentInfo->szImportURI), 		"hdlna://%s", 	temp->szMediaUri);
					else
						HxSTD_PrintToStrN(pDmr->ptContentInfo->szImportURI, 	sizeof(pDmr->ptContentInfo->szImportURI), 		"%s", 	temp->szMediaUri);
#else
					HxSTD_PrintToStrN(pDmr->ptContentInfo->szImportURI, 			sizeof(pDmr->ptContentInfo->szImportURI),		"%s",	temp->szMediaUri);
#endif

					HxSTD_PrintToStrN(pDmr->ptContentInfo->szThumbnailURI, 			sizeof(pDmr->ptContentInfo->szThumbnailURI), 	"%s", 	temp->szThumbnailUri);
					HxSTD_PrintToStrN(pDmr->ptContentInfo->szDate, 					sizeof(pDmr->ptContentInfo->szDate), 			"%s", 	temp->szDate);
					HxSTD_PrintToStrN(pDmr->ptContentInfo->szDuration,				sizeof(pDmr->ptContentInfo->szDuration),		"%d", temp->nDuration);
					HxSTD_PrintToStrN(pDmr->ptContentInfo->szResolution, 			sizeof(pDmr->ptContentInfo->szResolution), 	"%s", 	temp->szResolution);
					HxSTD_PrintToStrN(pDmr->ptContentInfo->szDigitalCopyControl, 	sizeof(pDmr->ptContentInfo->szDigitalCopyControl), 	"%d", temp->bCopyControl);
					HxSTD_PrintToStrN(pDmr->ptContentInfo->szGenre, 				sizeof(pDmr->ptContentInfo->szGenre), 			"%s", 	temp->szGenre);

					HxSTD_PrintToStrN(pDmr->ptContentInfo->stMetaData.szAlbumTitle,	sizeof(pDmr->ptContentInfo->stMetaData.szAlbumTitle), 	"%s", temp->szAlbum);
					HxSTD_PrintToStrN(pDmr->ptContentInfo->stMetaData.szArtist, 	sizeof(pDmr->ptContentInfo->stMetaData.szArtist), 		"%s", temp->szArtist);

					result 	= temp->nPlaySpeed;

					__debug_contentinfo(pDmr->ptContentInfo);
				}
				else
				{
					HxLOG_Error("Error> Content info is not exist. \n");
 				}
			}
			break;

#if 0
		case eDLNA_DMR_RequestPlay :
			event = eOPL_DMR_REQ_PLAY;
			{
				APKS_DLNA_DMR_ReqPlayInfo_t *temp = (APKS_DLNA_DMR_ReqPlayInfo_t*)p2;
				result 		= temp->nPlaySpeed;
			}
			break;
#endif

		case eDLNA_DMR_RequestStop:
			event = eOPL_DMR_REQ_STOP;
			break;

		case eDLNA_DMR_RequestPause:
			event = eOPL_DMR_REQ_PAUSE;
			break;

		case eDLNA_DMR_RequestSeek:
			event = eOPL_DMR_REQ_SEEK;
			result = (HUINT32)((APKS_DLNA_DMR_ReqSeekInfo_t*)p2)->nPosition;
			break;

		case eDLNA_DMR_RequestSetVolume:
			event = eOPL_DMR_REQ_SETVOLUME;
			result = ((APKS_DLNA_DMR_ReqSetVolumeInfo_t*)p2)->nVolume;
			break;

		case eDLNA_DMR_RequestSetMute:
			event = eOPL_DMR_REQ_SETMUTE;
			result = ((APKS_DLNA_DMR_ReqSetMuteInfo_t*)p2)->bMute;
			break;

		case eDLNA_DMR_RequestBringIn:
			event = eOPL_DMR_REQ_BRINGIN;
			break;

		case eDLNA_DMR_UnknownType :

		default:
			HxLOG_Error("Error> Unsupported event(%d)!\n", p1);
			return BPL_STATUS_OK;
	}

	if(NULL != eventListener) {
		eventListener(event, result, (Opl_DLNA_DmrContent_t*)HLIB_STD_MemDup(pDmr->ptContentInfo, sizeof(Opl_DLNA_DmrContent_t)));
	}

	return BPL_STATUS_OK;
}

BPLStatus	OPL_DMR_RegisterListener( OPL_DMR_EventCallback listener )
{
	eventListener = listener ;
	APK_DLNADMR_RegisterNotifier((APKS_DLNA_CbHandler) OPL_DMR_ApkListener );
	return BPL_STATUS_OK;
}

BPLStatus	OPL_DMR_UnRegisterListener(void)
{
	APK_DLNADMR_UnRegisterNotifier((APKS_DLNA_CbHandler) OPL_DMR_ApkListener);
	return BPL_STATUS_OK;
}

BPLStatus	OPL_DMR_GetActionStatus(HBOOL *status )
{
	APK_DB_GetInt(USERDB_DLNA_DMR_ACTIVATION, (HINT32*)status);
	return BPL_STATUS_OK;
}

BPLStatus 	OPL_DMR_IsActivated(HBOOL *bEnable)
{
	APK_DB_GetInt(USERDB_DLNA_DMR_ACTIVATION, (HINT32*)bEnable);
	return BPL_STATUS_OK;
}

BPLStatus OPL_DMR_GetDmrFriendlyName(HCHAR *friendlyName, int nLen)
{
	APK_DB_GetStr( USERDB_DLNA_DMR_FRIENDLYNAME, friendlyName, nLen);
	return BPL_STATUS_OK;
}

BPLStatus OPL_DMR_SetDmrFriendlyName(const HCHAR *friendlyName)
{
	(void)APK_DLNADMR_SetFriendlyName(friendlyName);
	APK_DB_SetStr( USERDB_DLNA_DMR_FRIENDLYNAME, friendlyName ); /* Store UDB regardless of setFriendlyName() failure */
	return BPL_STATUS_OK;
}

BPLStatus OPL_DMR_SetBringInUri(const HCHAR *pUrl)
{
	APK_DLNADMR_SetBringInUri( pUrl );
	return BPL_STATUS_OK;
}

BPLStatus OPL_DMR_SetOperationMode(const HINT32	nMode)
{
	APK_DLNADMR_SetOperationMode(nMode);
	return BPL_STATUS_OK;
}

