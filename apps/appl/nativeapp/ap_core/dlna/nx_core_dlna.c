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

/******************************************************************************/
/**
 * @file	  		nx_core_dlna.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_core_dlna.h>
#include <nx_core_message.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct
{
	HxVector_t	*dmsList;
	HxVector_t	*contentList;
	HCHAR		currentCid[APKD_DLNA_DMP_MAX_CID];
} NX_DlnaContext_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static NX_DlnaContext_t g_DlnaContext;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static	NX_DlnaContext_t *nx_dlna_GetContext(void);
static	NX_DLNA_DMS_Data_t *nx_dlna_CreateDmsData(APKS_DLNA_DMP_ServerDevice_t *pstDmsInfo);
static	NX_DLNA_DMS_Data_t *nx_dlna_FindDmsWithUDN(HCHAR* pszUDN);
static	HERROR	nx_dlna_AddDmsInfoData(NX_DLNA_DMS_Data_t *pstDmsData);
static	HERROR	nx_dlna_RemoveDmsInfoData(APKS_DLNA_DMP_ServerDevice_t *pstDmsInfo);
static	HBOOL	nx_dlna_FindDmsInfoCB(void *data, void *userdata);
static	void	nx_dlna_dmp_callback(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static	NX_DlnaContext_t *nx_dlna_GetContext(void)
{
	return &g_DlnaContext;
}

static	NX_DLNA_DMS_Data_t *nx_dlna_CreateDmsData(APKS_DLNA_DMP_ServerDevice_t *pstDmsInfo)
{
	NX_DLNA_DMS_Data_t *pstData = (NX_DLNA_DMS_Data_t*)NX_APP_Calloc(sizeof(NX_DLNA_DMS_Data_t));

	pstData->szTitle 	= HLIB_STD_StrDup((const HCHAR*)pstDmsInfo->szTitle);
	pstData->szUDN		= HLIB_STD_StrDup((const HCHAR*)pstDmsInfo->szUDN);

	HxLOG_Print("[%s:%04d] szTitle 	: [%s]\n", __FUNCTION__, __LINE__, pstData->szTitle);
	HxLOG_Print("[%s:%04d] szUDN  : [%s]\n", __FUNCTION__, __LINE__, pstData->szUDN);

	return pstData;
}

static	NX_DLNA_DMS_Data_t	*nx_dlna_FindDmsWithUDN(HCHAR* pszUDN)
{
	NX_DLNA_DMS_Data_t 	*pstData = NULL;
	NX_DlnaContext_t	*pstContext = nx_dlna_GetContext();
	HUINT32				ulListLength = 0, ulIdx = 0;

	if (pstContext == NULL || pstContext->dmsList == NULL)
	{
		HxLOG_Debug("pstContext Error!\n");
		return NULL;
	}

	ulListLength = HLIB_VECTOR_Length(pstContext->dmsList);
	for (ulIdx = 0; ulIdx < ulListLength; ulIdx++)
	{
		pstData = HLIB_VECTOR_ItemAt(pstContext->dmsList, ulIdx);
		if (pstData == NULL)
		{
			continue;
		}
		if (HxSTD_StrCmp(pstData->szUDN, pszUDN) == 0)
		{
			return pstData;
		}
	}

	return NULL;
}

static	HERROR	nx_dlna_AddDmsInfoData(NX_DLNA_DMS_Data_t *pstDmsData)
{
	NX_DlnaContext_t *pstContext = nx_dlna_GetContext();

	if(pstDmsData == NULL)
	{
		HxLOG_Debug("pstDmsData is NULL!\n");
		return ERR_FAIL;
	}

	HLIB_VECTOR_Add(pstContext->dmsList, pstDmsData);
	return ERR_OK;
}

static	HERROR	nx_dlna_RemoveDmsInfoData(APKS_DLNA_DMP_ServerDevice_t *pstDmsInfo)
{
	HINT32 nRemovedCount = 0;
	NX_DlnaContext_t *pstContext = nx_dlna_GetContext();

	nRemovedCount = HLIB_VECTOR_RemoveIf(pstContext->dmsList, nx_dlna_FindDmsInfoCB, (void*)(pstDmsInfo->szUDN));

	if (nRemovedCount > 0)
		return ERR_OK;

	return ERR_FAIL;
}

static	HBOOL	nx_dlna_FindDmsInfoCB(void *data, void *userdata)
{
	NX_DLNA_DMS_Data_t 		*pstDmsData	= (NX_DLNA_DMS_Data_t *)data;
	HCHAR 					*pszUDN 	= (HCHAR*)userdata;

	if (HxSTD_StrCmp(pstDmsData->szUDN, pszUDN) == 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static	void	nx_dlna_dmp_callback(HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	NX_DlnaContext_t	*pstContext = nx_dlna_GetContext();
	APKS_DLNA_DMP_ServerDevice_t *serverItems = NULL;
	APKS_DLNA_DMP_ContentItem_t	 *contentItems = NULL;
	HUINT32 ulEvent = ulParam1;
	HUINT32 count = ulParam3;
	HINT32	lIndex = 0;

	NX_DLNA_DMS_Data_t	*pstDlnaDmsData = NULL;
	NX_DLNA_CONTENT_t	*pstContentData = NULL;

	switch (ulEvent)
	{
		case eDLNA_DMP_ServerAdd :
		case eDLNA_DMP_ServerList :
			serverItems = (APKS_DLNA_DMP_ServerDevice_t *)ulParam2;

			for (lIndex = 0; lIndex < count; lIndex++)
			{
				if (nx_dlna_FindDmsWithUDN((HCHAR*)serverItems[lIndex].szUDN) == NULL)
				{
					pstDlnaDmsData = nx_dlna_CreateDmsData((APKS_DLNA_DMP_ServerDevice_t*)(&serverItems[lIndex]));
					nx_dlna_AddDmsInfoData(pstDlnaDmsData);
				}
			}
			ONDK_GWM_PostMessage(NULL, MSG_CORE_DLNA_DMS_CHANGE, 0, eNxDlna_Dms_Add, 0, 0);
			break;

		case eDLNA_DMP_ServerRemove:
			serverItems = (APKS_DLNA_DMP_ServerDevice_t *)ulParam2;

			for (lIndex = 0; lIndex < count; lIndex++)
			{
				nx_dlna_RemoveDmsInfoData((APKS_DLNA_DMP_ServerDevice_t*)(&serverItems[lIndex]));
			}
			ONDK_GWM_PostMessage(NULL, MSG_CORE_DLNA_DMS_CHANGE, 0, eNxDlna_Dms_Remove, 0, 0);
			break;

		case eDLNA_DMP_ContentAdd :
			contentItems = (APKS_DLNA_DMP_ContentItem_t *)ulParam2;

			if(count > 0)
			{
				if(pstContext->currentCid == NULL || HxSTD_StrEmpty(contentItems[0].szParentId))
				{
					HxLOG_Error("pstContext->currentCid or szParentId is null \n");
					return;
				}
				else
				{
					if(HxSTD_StrCmp(contentItems[0].szParentId, pstContext->currentCid) != 0)
					{
						HxLOG_Error("this is not contencts list of current cid... ! \n");
						return;
					}
				}
			}

			if (0 < count)
			{
				NX_DLNA_RemoveAllBrowsedItem();
				pstContext->contentList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))HLIB_STD_MemFree_CB, NULL);
			}

			for (lIndex = 0; lIndex < count; lIndex++)
			{
				pstContentData = (NX_DLNA_CONTENT_t *)HLIB_STD_MemAlloc(sizeof(NX_DLNA_CONTENT_t));
				HxSTD_MemSet(pstContentData, 0, sizeof(NX_DLNA_CONTENT_t));

				HxSTD_StrNCpy(pstContentData->szTitle, contentItems[lIndex].szTitle, APKD_DLNA_DMP_MAX_TITLE-1);

				HxSTD_StrNCpy(pstContentData->szCID, contentItems[lIndex].szCID, APKD_DLNA_DMP_MAX_CID-1);
				HxSTD_StrNCpy(pstContentData->szClass, contentItems[lIndex].szClass, APKD_DLNA_DMP_MAX_CLASS-1);
				HxSTD_StrNCpy(pstContentData->szObjectType, contentItems[lIndex].szObjectType, APKD_DLNA_DMP_MAX_OBJECTTYPE-1);
				HxSTD_StrNCpy(pstContentData->szImportURI, contentItems[lIndex].szURI, APKD_DLNA_DMP_MAX_URI-1);
				HxSTD_StrNCpy(pstContentData->szDuration, contentItems[lIndex].szDuration, APKD_DLNA_DMP_MAX_DURATION-1);
				HxSTD_StrNCpy(pstContentData->szThumbnailURI, contentItems[lIndex].szThumnail, APKD_DLNA_DMP_MAX_URI-1);
				HxSTD_StrNCpy(pstContentData->szGenre, contentItems[lIndex].szGenre, APKD_DLNA_DMP_MAX_GENRE-1);
				HxSTD_StrNCpy(pstContentData->szResolution, contentItems[lIndex].szResolution, APKD_DLNA_DMP_MAX_RESOLUTION-1);
				HxSTD_StrNCpy(pstContentData->szUDN, contentItems[lIndex].szUDN, APKD_DLNA_DMP_MAX_UDN-1);
				HxSTD_StrNCpy(pstContentData->szParentId, contentItems[lIndex].szParentId, APKD_DLNA_DMP_MAX_TITLE-1);

				/* Seek ???? */
				pstContentData->bByteSeekAble = contentItems[lIndex].bByteSeekAble;
				pstContentData->bTimeSeekAble = contentItems[lIndex].bTimeSeekAble;

				HxLOG_Debug( "[ NX_DLNA ] szCID = %s\n", pstContentData->szCID);
				HxLOG_Debug( "[ NX_DLNA ] szCID = %s\n", pstContentData->szTitle);
				HxLOG_Debug( "[ NX_DLNA ] szClass = %s\n", pstContentData->szClass);
				HxLOG_Debug( "[ NX_DLNA ] szObjectType = %s\n", pstContentData->szObjectType);
				HxLOG_Debug( "[ NX_DLNA ] szImportURI = %s\n", pstContentData->szImportURI);
				HxLOG_Debug( "[ NX_DLNA ] szDuration = %s\n", pstContentData->szDuration);
				HxLOG_Debug( "[ NX_DLNA ] szThumbnailURI = %s\n", pstContentData->szThumbnailURI);
				HxLOG_Debug( "[ NX_DLNA ] szGenre = %s\n", pstContentData->szGenre);
				HxLOG_Debug( "[ NX_DLNA ] szResolution = %s\n", pstContentData->szResolution);
				HxLOG_Debug( "[ NX_DLNA ] szUDN = %s\n", pstContentData->szUDN);
				HxLOG_Debug( "[ NX_DLNA ] bByteSeekAble = %ds\n", pstContentData->bByteSeekAble);
				HxLOG_Debug( "[ NX_DLNA ] bTimeSeekAble = %ds\n", pstContentData->bTimeSeekAble);

				pstContentData->ulChildCount = contentItems[lIndex].ulChildCount;

				HLIB_VECTOR_Add(pstContext->contentList, (void*)pstContentData);

				ONDK_GWM_PostMessage(NULL, MSG_CORE_DLNA_CONTENTS_BRWOSE_DONE, 0, 0, (HINT32)pstContext->contentList, (HINT32)count);
			}
			break;
		case eDLNA_DMP_ContentEnd :
		default :
			break;
	}
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
HERROR	NX_DLNA_Init(void)
{
	NX_DlnaContext_t	*pstContext = nx_dlna_GetContext();

	NX_DLNA_DMP_Stop();

	HxSTD_MemSet(pstContext, 0, sizeof(NX_DlnaContext_t));
	pstContext->dmsList = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))HLIB_STD_MemFree_CB, NULL);

	return ERR_OK;
}

HxVector_t*	NX_DLNA_GetDmsList(void)
{
	NX_DlnaContext_t	*pstContext = nx_dlna_GetContext();

	return pstContext->dmsList;
}

HERROR	NX_DLNA_DMP_Start(void)
{
	NX_PORT_DLNA_DMP_RegisterNotifier(nx_dlna_dmp_callback);

	return NX_PORT_DLNA_DMP_Start();
}

HERROR	NX_DLNA_DMP_Stop(void)
{
	NX_PORT_DLNA_DMP_UnRegisterNotifier(nx_dlna_dmp_callback);

	return NX_PORT_DLNA_DMP_Stop();
}

HERROR	NX_DLNA_DMP_RefreshDms(const HCHAR *pUdn)
{
	return NX_PORT_DLNA_DMP_RefreshDms(pUdn);
}

HERROR	NX_DLNA_BrwosingChildItem(HCHAR *pszUDN, HCHAR *pszCid)
{
	HERROR hErr;
	HCHAR szUDN[APKD_DLNA_DMP_MAX_UNKNOWN] = {0, };
	NX_DlnaContext_t	*pstContext = nx_dlna_GetContext();

	HxSTD_MemSet(szUDN, '\0', sizeof(szUDN));
	HxSTD_MemSet(pstContext->currentCid, '\0', sizeof(pstContext->currentCid));

	HxSTD_sprintf((char*)szUDN, "%s", pszUDN);
	HxSTD_sprintf((char*)pstContext->currentCid, "%s", pszCid);

	hErr = APK_DLNADMP_BrowsingChildItem((HCHAR*)szUDN, (HCHAR*)pstContext->currentCid, (HCHAR*)"ASC_TITLE", 0, 16);

	return ERR_OK;
}

HERROR	NX_DLNA_RemoveAllBrowsedItem(void)
{
	NX_DlnaContext_t	*pstContext = nx_dlna_GetContext();

	if (pstContext->contentList && (HLIB_VECTOR_Length(pstContext->contentList) > 0))
	{
		HxLOG_Debug("remove all dlna contentList !\n");
		HLIB_VECTOR_Delete(pstContext->contentList);
		pstContext->contentList = NULL;
	}

	return ERR_OK;
}

HCHAR*	NX_DLNA_GetCurrentCID(void)
{
	NX_DlnaContext_t	*pstContext = nx_dlna_GetContext();

	return pstContext->currentCid;
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

