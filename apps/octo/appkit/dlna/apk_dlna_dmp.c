/*******************************************************************
    File Description
********************************************************************/
/**
    @file     apk_dlan_dmp.c
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
* MS949 encoding?í‰Ωø?®„Åó??source file?ß„Åô?
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
#include 	<homapi.h>
#include 	<homapi_dmp.h>

typedef struct
{
	HBOOL    bStarted;
	HUINT32  semaphore;
	HxList_t *pHList;
	HINT32   uDmsNum;
	APKS_DLNA_DMP_ServerDevice_t *pDmsSendList;
} apkDlnaDmp_Ctx_t;

#define ________________Static_Variables___________________________
static HxList_t	*s_pst_dmp_cblist;
static apkDlnaDmp_Ctx_t	s_dlnaDmpCtx  = { 0, };
static apkDlnaDmp_Ctx_t *apk_dlnadmp_getContext(void);
static void	apk_dlnadmp_releaseContext(apkDlnaDmp_Ctx_t *ctx);

#define ________________Internal_Functions_________________________

static apkDlnaDmp_Ctx_t *	apk_dlnadmp_getContext (void)
{
	if (s_dlnaDmpCtx.semaphore == 0)
	{
		s_dlnaDmpCtx.bStarted      = FALSE;
		s_dlnaDmpCtx.pHList      = NULL;
		s_dlnaDmpCtx.uDmsNum       = 0;
		s_dlnaDmpCtx.pDmsSendList  = NULL;
		HxSEMT_Create(&s_dlnaDmpCtx.semaphore, "ApkDlnaDmp", HxSEMT_FIFO);
		HxSEMT_Get(s_dlnaDmpCtx.semaphore);
	}
	else
	{
		HxSEMT_Get(s_dlnaDmpCtx.semaphore);
	}

	return &s_dlnaDmpCtx;
}

static void	apk_dlnadmp_releaseContext (apkDlnaDmp_Ctx_t *ctx)
{
	HxSEMT_Release(ctx->semaphore);
}

static HERROR	__convert_device_info(HOMxDMP_DmsInfo_t *p1, APKS_DLNA_DMP_ServerDevice_t *p2)
{
	HxSTD_snprintf(p2->szTitle, sizeof(p2->szTitle), "%s", p1->szFriendlyName);
	HxSTD_snprintf(p2->szUDN, sizeof(p2->szUDN), "%s", p1->szUDN);
	HxSTD_snprintf(p2->szIP, sizeof(p2->szIP), "%s", p1->szIp);

	return ERR_OK;
}

static void 	__dms_info_dump(void *data)
{
	APKS_DLNA_DMP_ServerDevice_t *p = (APKS_DLNA_DMP_ServerDevice_t *)data;
	HxLOG_Debug("[%s:%d] FriendlyName : %s / UDN :%s / IP : %s \n", \
			__FUNCTION__, __LINE__, p->szTitle, p->szUDN, p->szIP);
}

static HERROR	__convert_cds_item(void *data, HUINT32 size,  HINT32 *nCount, APKS_DLNA_DMP_ContentItem_t **ppContent )
{
	HINT32 i = 0, listCount = size;
	HOMxComm_CdsInfo_t *list = (HOMxComm_CdsInfo_t *)data;

	*nCount	= listCount;
	*ppContent = (APKS_DLNA_DMP_ContentItem_t*)APK_Calloc( sizeof(APKS_DLNA_DMP_ContentItem_t) * listCount );

	for ( i = 0; i < listCount; i++ )
	{
		HLIB_STD_StrNCpySafe((*ppContent)[i].szTitle, list[i].Title,  APKD_DLNA_DMP_MAX_TITLE -1);
		HLIB_STD_StrNCpySafe((*ppContent)[i].szCID, list[i].ID,  APKD_DLNA_DMP_MAX_CID -1);

		switch(list[i].eItemType)
		{
			case HOMxComm_MediaType_Container:
				{
					HLIB_STD_StrNCpySafe((*ppContent)[i].szObjectType, "folder",  APKD_DLNA_DMP_MAX_OBJECTTYPE -1);
				}
				break;
			case HOMxComm_MediaType_Video:
				{
					HLIB_STD_StrNCpySafe((*ppContent)[i].szObjectType, "video",  APKD_DLNA_DMP_MAX_OBJECTTYPE -1);
				}
				break;
			case HOMxComm_MediaType_Music:
				{
					HLIB_STD_StrNCpySafe((*ppContent)[i].szObjectType, "music",  APKD_DLNA_DMP_MAX_OBJECTTYPE -1);
				}
				break;
			case HOMxComm_MediaType_Photo:
				{
					HLIB_STD_StrNCpySafe((*ppContent)[i].szObjectType, "photo",  APKD_DLNA_DMP_MAX_OBJECTTYPE -1);
				}
				break;

			default :
				break;
		}

		HLIB_STD_StrNCpySafe((*ppContent)[i].szUDN, list[i].szUDN,  APKD_DLNA_DMP_MAX_UDN -1);
		HLIB_STD_StrNCpySafe((*ppContent)[i].szParentId, list[i].IDofUpper, APKD_DLNA_DMP_MAX_UNKNOWN -1);

		if(list[i].eItemType != HOMxComm_MediaType_Photo)
		{
			HxSTD_snprintf( (*ppContent)[i].szURI, sizeof( (*ppContent)[i].szURI), "hdlna://%s_Prot=%s_Size=%ld_Dura=%d_Rate=%d",
				list[i].URI, list[i].Protocol, (long int)list[i].FileSize, list[i].Duration, list[i].Bitrate);
		}
		else
			HxSTD_snprintf( (*ppContent)[i].szURI, sizeof( (*ppContent)[i].szURI), "%s", list[i].URI);

		HLIB_STD_StrNCpySafe((*ppContent)[i].szThumnail, list[i].ThumbnailURI, APKD_DLNA_DMP_MAX_URI -1);
		HLIB_STD_StrNCpySafe((*ppContent)[i].szResolution, list[i].resolution, APKD_DLNA_DMP_MAX_RESOLUTION -1);

		HxSTD_snprintf((*ppContent)[i].szDuration, sizeof((*ppContent)[i].szDuration), "%d", list[i].Duration);

		(*ppContent)[i].bByteSeekAble = list[i].ByteSeekable;
		(*ppContent)[i].bTimeSeekAble = list[i].TimeSeekable;

		(*ppContent)[i].ulTotalMatches = list[i].totalMatches;
		(*ppContent)[i].ulChildCount = list[i].ChildCount;
		(*ppContent)[i].ulFileSize = list[i].FileSize;
		(*ppContent)[i].ulBitRate = list[i].Bitrate;

		(*ppContent)[i].ulAudioChannels = list[i].NrAudioChannels;
		(*ppContent)[i].ulSampleRateFreq = list[i].SampleFrequency;

		HLIB_STD_StrNCpySafe((*ppContent)[i].szAlbumTitle, list[i].AlbumTitle, APKD_DLNA_DMP_MAX_TITLE -1);
		HLIB_STD_StrNCpySafe((*ppContent)[i].szGenre, list[i].Genre, APKD_DLNA_DMP_MAX_GENRE -1);
		HLIB_STD_StrNCpySafe((*ppContent)[i].szArtist, list[i].Artist, APKD_DLNA_DMP_MAX_TITLE -1);
	}

	return ERR_OK;
}

STATIC void	apk_dlnadmp_freeEventCb(HINT32 nArgc, void *apArgV[])
{
	void	*pvData = NULL;
	void	*pvDataResult = NULL;
	HOMxDMP_Event_t eEvt = 0;

	HxLOG_Assert(nArgc == 4);

	eEvt = (HOMxDMP_Event_t)apArgV[1];
	pvData = apArgV[2];
	pvDataResult = apArgV[3];

	if (pvData)
		APK_Free(pvData);

	switch ( eEvt )
	{
		case eHOMAPI_DMP_Event_SearchResult :
		case eHOMAPI_DMP_Event_SearchAllResult :
			if ( pvDataResult )
				APK_Free(pvDataResult);
			break;
		default :
			 break;
	}
}

static HBOOL    __compare_dmsudn(void *pSrc, void *pDest)
{
	APKS_DLNA_DMP_ServerDevice_t *pstSrc  = (APKS_DLNA_DMP_ServerDevice_t*)pSrc;
	APKS_DLNA_DMP_ServerDevice_t *pstDest = (APKS_DLNA_DMP_ServerDevice_t*)pDest;

	if (pSrc == NULL || pDest == NULL)
		return FALSE;

	if (HxSTD_StrNCmp(pstSrc->szUDN, pstDest->szUDN, UDN_LEN) == 0)
		return TRUE;

	return FALSE;
}

static HERROR ___event_cb(HUINT32 p1, HUINT32 p2, HUINT32 p3)
{
	apkDlnaDmp_Ctx_t	*ctx;
	void			*pResultData = NULL;
	void			*pResultInfo = NULL;
	HINT32 			nCount = 1;
	HOMxDMP_Event_t cmd = p1;
	APKE_DLNA_DMP_Property_e eType = eDLNA_DMP_UnknownType;

	HxLOG_Trace();

	ctx = apk_dlnadmp_getContext();

	switch(cmd)
	{
		case eHOMAPI_DMP_Event_DmsAdd:
		case eHOMAPI_DMP_Event_DmsRemove:
			{
				HOMxDMP_DmsInfo_t *p = (HOMxDMP_DmsInfo_t *)p2;
				APKS_DLNA_DMP_ServerDevice_t *ret = (APKS_DLNA_DMP_ServerDevice_t *)APK_Calloc(sizeof(APKS_DLNA_DMP_ServerDevice_t));
				if(ret)
				{
					HxSTD_MemSet(ret, 0x0, sizeof(APKS_DLNA_DMP_ServerDevice_t));
					__convert_device_info((HOMxDMP_DmsInfo_t *)p2, ret);
					//__dms_info_dump(ret);
					eType = (cmd == eHOMAPI_DMP_Event_DmsAdd)?eDLNA_DMP_ServerAdd:eDLNA_DMP_ServerRemove;
					pResultData = ret;

					HxLOG_Print("[%s:%d] DMS %s - %s \n", __FUNCTION__, __LINE__, (cmd == eHOMAPI_DMP_Event_DmsAdd)?"ADD":"REMOVE", p->szFriendlyName);

					if (1)
					{
						APKS_DLNA_DMP_ServerDevice_t *pServiceDevice = NULL;
						pServiceDevice = (APKS_DLNA_DMP_ServerDevice_t *)APK_MemDup(ret, sizeof(APKS_DLNA_DMP_ServerDevice_t));
						if (cmd == eHOMAPI_DMP_Event_DmsAdd) /* DMS Added */
						{
							ctx->pHList = HLIB_LIST_Append(ctx->pHList, (void *)pServiceDevice);
							ctx->uDmsNum++;
						}
						else if (cmd == eHOMAPI_DMP_Event_DmsRemove) /* DMS Remove */
						{
							HxList_t *pRemoveEntry = HLIB_LIST_FindEx(ctx->pHList, (void*)pServiceDevice, __compare_dmsudn);
							if (pRemoveEntry)
							{
								void *ptr = HLIB_LIST_Data(pRemoveEntry);
								ctx->pHList = HLIB_LIST_Remove(ctx->pHList, (void*)HLIB_LIST_Data(pRemoveEntry));
								ctx->uDmsNum--;
								APK_Free(ptr);
								APK_Free(pServiceDevice); // pServiceDevice is no more needed
								ptr = NULL;
							}
						}
					}
				}
			}
			break;

		case eHOMAPI_DMP_Event_CdsListAdd:
			{
				__convert_cds_item((void *) p2, p3, &nCount, (APKS_DLNA_DMP_ContentItem_t **)&pResultData );
				eType = eDLNA_DMP_ContentAdd;
			}
			break;

		case eHOMAPI_DMP_Event_CdsListEnd:
			{
				eType = eDLNA_DMP_ContentEnd;
				nCount = 0;
			}
			break;

		case eHOMAPI_DMP_Event_DmsList:
			{
				//dlnadmp_DecodeJsonToServerList( JDMPObject, &nCount, (APKS_DLNA_DMP_ServerDevice_t **)&pResultData );
			}
			break;

		case eHOMAPI_DMP_Event_DmsRecRst:
			{
#if 0
				HOMxDMP_RecordDestination_t *dst = data;
				HINT32 count = size/sizeof(HOMxDMP_RecordDestination_t);

				typedef struct
				{
					HCHAR	id[128];
					HCHAR	name[128];
				} HOMxDMP_RecordDestination_t;
#endif
				eType = eDLNA_DMP_GetRecordDestination;
				nCount = 0;
			}
			break;

		case eHOMAPI_DMP_Event_DmsDiskInfo:
			{
				HOMxDMP_DmsDiskInfo_t *info = (HOMxDMP_DmsDiskInfo_t *)p2;
				APKS_DLNA_DMP_Storage_t *ret = APK_Calloc(sizeof(APKS_DLNA_DMP_Storage_t));

				if( ret )
				{
					ret->Total = info->total;
					ret->Available = info->available;

					HxLOG_Print("[%s:%d] Storage size (%lld / %lld ) \n", __FUNCTION__, __LINE__, ret->Available, ret->Total);
					eType = eDLNA_DMP_GetDmsDiskInfo;
					pResultData = ret;
					nCount = 1;
				}
			}
			break;


		case eHOMAPI_DMP_Event_CdsRemoved:
			{
				HOMxDMP_CdsRemoveResult_t *result = (HOMxDMP_CdsRemoveResult_t *)p2;
				APKS_DLNA_DMP_RemoveItemResult_t *ret = APK_Calloc(sizeof(APKS_DLNA_DMP_RemoveItemResult_t));
				if( ret )
				{
					ret->nRetVal = result->nRetVal;
					HxSTD_snprintf(ret->szUDN, sizeof(ret->szUDN), "%s", result->szUDN);
					HxSTD_snprintf(ret->szCID, sizeof(ret->szCID), "%s", result->szCID);
					eType = eDLNA_DMP_RemoveCmd;
					pResultData = ret;
					nCount = 1;
				}
			}
			break;

		case eHOMAPI_DMP_Event_SearchResult:
		case eHOMAPI_DMP_Event_SearchAllResult:
			{
				APKS_DLNA_DMP_ContentResult_t *pSearchResult = NULL;
				HOMxDMP_ContentResult_t *pResult = (HOMxDMP_ContentResult_t *)p3;
				HINT32 itemCnt = pResult->nReturnCnt;

				pSearchResult = APK_Calloc(sizeof(APKS_DLNA_DMP_ContentResult_t));

				if (cmd == eHOMAPI_DMP_Event_SearchResult)
					eType = eDLNA_DMP_SearchResult;
				else
					eType = eDLNA_DMP_SearchAllResult;

				pSearchResult->nReqId			= pResult->nReqId;
				pSearchResult->nTotalMatch		= pResult->nTotalMatch;
				pSearchResult->nReturnCnt		= pResult->nReturnCnt;
				pSearchResult->nErrCode			= pResult->nErrCode;
				HLIB_STD_StrNCpySafe( pSearchResult->szUDN, pResult->szUDN, sizeof(pSearchResult->szUDN) );

				__convert_cds_item((void *) p2, itemCnt, &nCount, (APKS_DLNA_DMP_ContentItem_t **)&pResultData );

				pResultInfo = (void *)pSearchResult;
			}
			break;
	}

	if ( s_pst_dmp_cblist )
	{
		APKS_DLNA_CbHandler pfnObjectHandler;
		HxList_t 			*item = s_pst_dmp_cblist;
		APK_EVENT_EMIT_ARGS	stApkEvtArgs;

		while ( item )
		{
			pfnObjectHandler = (APKS_DLNA_CbHandler)HLIB_LIST_Data(item);
			if ( pfnObjectHandler )
			{
				HxSTD_MemSet(&stApkEvtArgs, 0, sizeof(APK_EVENT_EMIT_ARGS));
				switch (eType)
				{
					case eDLNA_DMP_ServerAdd:
					case eDLNA_DMP_ServerRemove:
						stApkEvtArgs.pfnFreeFunc = apk_dlnadmp_freeEventCb;
						stApkEvtArgs.ulArgCount = 4;
						stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
						stApkEvtArgs.apArgV[1] = (void *)eType;
						stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(pResultData, sizeof(APKS_DLNA_DMP_ServerDevice_t));
						stApkEvtArgs.apArgV[3] = (void *)nCount;
						APK_EVENT_Emit(&stApkEvtArgs);
						break;

					case eDLNA_DMP_ContentAdd:
						stApkEvtArgs.pfnFreeFunc = apk_dlnadmp_freeEventCb;
						stApkEvtArgs.ulArgCount = 4;
						stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
						stApkEvtArgs.apArgV[1] = (void *)eType;
						stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(pResultData, sizeof(APKS_DLNA_DMP_ContentItem_t)*nCount);
						stApkEvtArgs.apArgV[3] = (void *)nCount;
						APK_EVENT_Emit(&stApkEvtArgs);
						break;

					case eDLNA_DMP_ContentEnd:
					case eDLNA_DMP_GetRecordDestination:
						stApkEvtArgs.pfnFreeFunc = NULL;
						stApkEvtArgs.ulArgCount = 4;
						stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
						stApkEvtArgs.apArgV[1] = (void *)eType;
						stApkEvtArgs.apArgV[2] = (void *)NULL;
						stApkEvtArgs.apArgV[3] = (void *)0;
						APK_EVENT_Emit(&stApkEvtArgs);
						break;

					case eDLNA_DMP_GetDmsDiskInfo:
						stApkEvtArgs.pfnFreeFunc = apk_dlnadmp_freeEventCb;
						stApkEvtArgs.ulArgCount = 4;
						stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
						stApkEvtArgs.apArgV[1] = (void *)eType;
						stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(pResultData, sizeof(APKS_DLNA_DMP_Storage_t));
						stApkEvtArgs.apArgV[3] = (void *)nCount;
						APK_EVENT_Emit(&stApkEvtArgs);
						break;

					case eDLNA_DMP_RemoveCmd:
						stApkEvtArgs.pfnFreeFunc = apk_dlnadmp_freeEventCb;
						stApkEvtArgs.ulArgCount = 4;
						stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
						stApkEvtArgs.apArgV[1] = (void *)eType;
						stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(pResultData, sizeof(APKS_DLNA_DMP_RemoveItemResult_t));
						stApkEvtArgs.apArgV[3] = (void *)nCount;
						APK_EVENT_Emit(&stApkEvtArgs);
						break;

					case eDLNA_DMP_SearchResult:
						stApkEvtArgs.pfnFreeFunc = apk_dlnadmp_freeEventCb;
						stApkEvtArgs.ulArgCount = 4;
						stApkEvtArgs.apArgV[0] = (void *)pfnObjectHandler;
						stApkEvtArgs.apArgV[1] = (void *)eType;
						stApkEvtArgs.apArgV[2] = (void *)APK_MemDup(pResultData, sizeof(APKS_DLNA_DMP_ContentItem_t)*nCount);
						stApkEvtArgs.apArgV[3] = (void *)APK_MemDup(pResultInfo, sizeof(APKS_DLNA_DMP_ContentResult_t));
						APK_EVENT_Emit(&stApkEvtArgs);
						break;

					default:
						HxLOG_Error("[%s:%d] not defined type -- [%d]\n", __FUNCTION__, __LINE__, eType);
						break;
				}
			}
			item = item->next;
		}
	}

	if(pResultData)
		APK_Free(pResultData);

	if(pResultInfo)
		APK_Free(pResultInfo);

	apk_dlnadmp_releaseContext(ctx);
	return ERR_OK;
}

#define ________________Public_Interfaces__________________________
void	APK_DLNADMP_Init( void )
{
	HOMAPI_DMP_RegisterNotifier(___event_cb);
}

void	APK_DLNADMP_DeInit( void )
{
	HOMAPI_DMP_DeInit();
	HOMAPI_DMP_UnRegisterNotifier();
}

void	APK_DLNADMP_RegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler )
{
	HxList_t		*list = HLIB_LIST_Find(s_pst_dmp_cblist, pfnObjectHandler );

	if ( list )
		HxLOG_Error("pfnObjectHandler(%X) is already registered!!!\n", pfnObjectHandler);
	else
		s_pst_dmp_cblist = HLIB_LIST_Append( s_pst_dmp_cblist, (void *)pfnObjectHandler );
}

void	APK_DLNADMP_UnRegisterNotifier( APKS_DLNA_CbHandler pfnObjectHandler )
{
	HxList_t		*list = HLIB_LIST_Find( s_pst_dmp_cblist, pfnObjectHandler );

	if ( NULL == list )
		HxLOG_Error("pfnObjectHandler(%X) does not registered yet..\n", pfnObjectHandler);
	else
		s_pst_dmp_cblist = HLIB_LIST_Remove( s_pst_dmp_cblist, pfnObjectHandler );
}

HERROR	APK_DLNADMP_Start( void )
{
	HERROR hError = ERR_OK;
	apkDlnaDmp_Ctx_t *ctx;
	HxLOG_Trace();

	ctx = apk_dlnadmp_getContext();

	if (ctx->pHList)
	{
		HLIB_LIST_RemoveAllFunc(ctx->pHList, HLIB_STD_MemFree_CB);
		ctx->pHList = NULL;
	}
	ctx->uDmsNum = 0;

	if (ctx->pDmsSendList)
	{
		APK_Free(ctx->pDmsSendList);
		ctx->pDmsSendList = NULL;
	}

	hError = HOMAPI_DMP_Start();
	if (hError == ERR_OK)
	{
		ctx->bStarted = TRUE;
	}

	apk_dlnadmp_releaseContext(ctx);
	return hError;
}

HERROR	APK_DLNADMP_Stop( void )
{
	HERROR hError = ERR_OK;
	apkDlnaDmp_Ctx_t *ctx;
	HxLOG_Trace();

	ctx = apk_dlnadmp_getContext();

	hError = HOMAPI_DMP_Stop();

	if (ctx->pHList)
	{
		HLIB_LIST_RemoveAllFunc(ctx->pHList, HLIB_STD_MemFree_CB);
		ctx->pHList = NULL;
	}
	ctx->uDmsNum = 0;

	if (ctx->pDmsSendList)
	{
		APK_Free(ctx->pDmsSendList);
		ctx->pDmsSendList = NULL;
	}
	ctx->bStarted = FALSE;

	apk_dlnadmp_releaseContext(ctx);
	return hError;
}

HERROR	APK_DLNADMP_RefreshDms( const HCHAR *pUdn )
{
	return HOMAPI_DMP_RefreshDms(pUdn);
}

static HERROR	apk_dlnadmp_getDmsListFromHomma( HUINT32 *pnCount, APKS_DLNA_DMP_ServerDevice_t **pResultData )
{
	HUINT32 		 nCount = 0 ;
	HOMxDMP_DmsInfo_t *pInfoList = NULL;
	APKS_DLNA_DMP_ServerDevice_t *pCdsList = NULL;
	HERROR hError;

	*pnCount = 0;

	HxLOG_Debug("HOMAPI_DMP_getListOfDMS()++\n");
	hError = HOMAPI_DMP_getListOfDMS(&nCount, &pInfoList);
	if (hError != ERR_OK)
	{
		HxLOG_Error("Error> failed to get DMS list\n");
		*pnCount = 0;
		return ERR_FAIL;
	}
	HxLOG_Debug("HOMAPI_DMP_getListOfDMS()-- count:%d \n", nCount);

	if ( nCount )
	{
		HINT32 i = 0;
		pCdsList = (APKS_DLNA_DMP_ServerDevice_t *)APK_Calloc( sizeof(APKS_DLNA_DMP_ServerDevice_t) * nCount );
		for ( i=0; i < nCount; i++ )
		{
			__convert_device_info( &pInfoList[i], &pCdsList[i] );
		}
	}
	*pResultData = (APKS_DLNA_DMP_ServerDevice_t *)pCdsList;
	*pnCount = nCount;

	if ( pInfoList )
	{
		APK_Free(pInfoList);
	}
	return ERR_OK;
}

static HERROR	apk_dlnadmp_getDmsList( HUINT32 *pnCount, APKS_DLNA_DMP_ServerDevice_t **pResultData )
{
	apkDlnaDmp_Ctx_t *ctx;
	HxList_t *pHList = NULL;
	HUINT32 nDmsCount = 0;
	HINT32 i;

	HxLOG_Trace();
	*pnCount = 0;

	ctx = apk_dlnadmp_getContext();
	nDmsCount = HLIB_LIST_Length(HLIB_LIST_First(ctx->pHList));

	if (ctx->pDmsSendList)
	{
		APK_Free(ctx->pDmsSendList);
		ctx->pDmsSendList = NULL;
	}

	if (nDmsCount > 0)
	{
		ctx->pDmsSendList = ( APKS_DLNA_DMP_ServerDevice_t* )APK_Calloc(sizeof(APKS_DLNA_DMP_ServerDevice_t)*nDmsCount);
		HxSTD_MemSet(ctx->pDmsSendList, 0x0, sizeof(APKS_DLNA_DMP_ServerDevice_t)*nDmsCount);
		for (i = 0; i<nDmsCount; i++)
		{
			pHList = HLIB_LIST_GetListItem(HLIB_LIST_First(ctx->pHList), i);
			if (pHList)
			{
				HxSTD_MemCopy(&ctx->pDmsSendList[i], HLIB_LIST_Data(pHList), sizeof(APKS_DLNA_DMP_ServerDevice_t));
				__dms_info_dump((void *)&ctx->pDmsSendList[i]);
			}
		}
	}

	*pResultData = ctx->pDmsSendList;
	*pnCount = nDmsCount;
	apk_dlnadmp_releaseContext(ctx);
	return ERR_OK;
}

HERROR	APK_DLNADMP_getListOfDMS( HUINT32 *pnCount, APKS_DLNA_DMP_ServerDevice_t **pResultData )
{
	return apk_dlnadmp_getDmsList(pnCount, pResultData);
}

HERROR	APK_DLNADMP_BrowsingChildItem( const HCHAR *udn, const HCHAR *cid, const HCHAR *sort, HINT32 startIdx, HINT32 nRequest)
{
	return HOMAPI_DMP_BrowsingChildItem(udn, cid, sort, startIdx, nRequest);
}

HERROR  APK_DLNADMP_SearchingItem( const HCHAR *udn, const HCHAR *cid, const HCHAR *category, const HCHAR *keyword,
                     const HCHAR *sort, HINT32 startIdx, HINT32 nRequest, HINT32 reqId )
{
	HxLOG_Debug("HOMAPI_DMP_SearchingItem() - [reqId:%d, udn:%s, cid:%s]\n", reqId, udn, cid);
	return HOMAPI_DMP_SearchingItem(udn, cid, category, keyword, sort, startIdx, nRequest, reqId);
}

HERROR  APK_DLNADMP_SearchingAll( const HCHAR *cid, const HCHAR *category, const HCHAR *keyword,
                     const HCHAR *sort, HINT32 reqId )
{
	HxLOG_Debug("APK_DLNADMP_SearchingAll() - [reqId:%d, cid:%s]\n", reqId, cid);
	return HOMAPI_DMP_SearchAll(cid, category, keyword, sort, reqId);
}

/*-----------------------------------------------------------------------------
 *  for japan spec(jlabs)
 *-----------------------------------------------------------------------------*/

HERROR	APK_DLNADMP_GetRecordDestinations(const HCHAR *udn)
{
	return HOMAPI_DMP_GetRecordDestinations(udn);
}

HERROR	APK_DLNADMP_GetRecordDiskInfo( const HCHAR *udn, const HCHAR *id)
{
	return HOMAPI_DMP_GetRecordDiskInfo(udn, id);
}

HERROR	APK_DLNADMP_RemoveItem( const HCHAR *udn, const HCHAR *id, char **retval)
{
	return HOMAPI_DMP_RemoveItem(udn, id, retval);
}

HERROR	APK_DLNADMP_GetIsIPDubbing(HBOOL *val)
{
	return ERR_OK;
}
