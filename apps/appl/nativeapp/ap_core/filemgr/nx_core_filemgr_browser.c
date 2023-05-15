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
 * @file	  		nx_core_filemgr_browser.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/
#include <nx_core_filemgr_browser.h>
#include <nx_core_message.h>


/**************************************************************************************************/
#define ___________________Private_variable________________________________________________
/**************************************************************************************************/
static	NxFileBrowserContext_t	*s_pstBrowserContext 	= NULL;
/**************************************************************************************************/
#define ___________________Private_typedef________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define ___________________Private_prototype________________________________________________
/**************************************************************************************************/
static NxFileBrowserContext_t *nx_filemgr_browser_GetContext(void);
static void 	nx_filemgr_browser_ContextItemListDelete(void *item);
static void		nx_filemgr_browser_BrowserHandleItemDelete(void *item);
static HBOOL 	nx_filemgr_browser_FindHandleData(void *data, void *userdata);
static HBOOL 	nx_filemgr_browser_FindHandleDataByTicket(void *data, void *userdata);
static HBOOL	nx_filemgr_browser_IsBrowserHandle(NxFileBrowserHandle_t *handle);
static NxFileBrowserHandle_t	*nx_filemgr_browser_FindBrowserHandle(HxVector_t *list, NxFileBrowserHandle_t *data);
static void 	nx_filemgr_browser_Dama_callback(int requestId, void *pvUserData, HxList_t *pstResultList, void *pvCbInfo);

#if defined(CONFIG_DEBUG)
static void nx_filemgr_browser_Command_Init(void);
#endif

/**************************************************************************************************/
#define ___________________Private_functions________________________________________________
/**************************************************************************************************/
static 	NxFileBrowserContext_t *nx_filemgr_browser_GetContext(void)
{
	if (s_pstBrowserContext == NULL)
	{
		s_pstBrowserContext = (NxFileBrowserContext_t *)NX_APP_Calloc(sizeof(NxFileBrowserContext_t));
		s_pstBrowserContext->hBrowserList = HLIB_VECTOR_NewEx(NULL, 0,
												(void *(*)(void *))NULL,
												(void *(*)(void *))nx_filemgr_browser_ContextItemListDelete,
												NULL);
	}
	return s_pstBrowserContext;
}

static void 	nx_filemgr_browser_ContextItemListDelete(void *item)
{

	NxFileBrowserHandle_t *hItemHandle = (NxFileBrowserHandle_t*)item;
	if (hItemHandle != NULL)
	{
		if (hItemHandle->itemList)
			HLIB_VECTOR_Delete(hItemHandle->itemList);
		NX_APP_Free(hItemHandle);
	}

}

static void		nx_filemgr_browser_BrowserHandleItemDelete(void *item)
{
	DxFileInfo_t* pstFsInfo = (DxFileInfo_t*)item;
	HxLOG_Debug("Handle item delete. eInterface(%d), eDataType(%d), szFileName(%s).\n",
					pstFsInfo->eInterface, pstFsInfo->eDataType, pstFsInfo->szFileName);
	NX_APP_Free(item);
}

static HBOOL 	nx_filemgr_browser_FindHandleData(void *data, void *userdata)
{
	NxFileBrowserHandle_t *pListData = (NxFileBrowserHandle_t *)data;
	NxFileBrowserHandle_t *pUserData = (NxFileBrowserHandle_t *)userdata;

	if (pListData == pUserData)
		return TRUE;
	else
		return FALSE;
}

static HBOOL 	nx_filemgr_browser_FindHandleDataByTicket(void *data, void *userdata)
{
	NxFileBrowserHandle_t *pListData = (NxFileBrowserHandle_t *)data;
	Nxfile_Ticket_t ticket = (Nxfile_Ticket_t)userdata;

	if (pListData->ticket == ticket)
		return TRUE;
	else
		return FALSE;
}

static NxFileBrowserHandle_t	*nx_filemgr_browser_FindBrowserHandle(HxVector_t *list, NxFileBrowserHandle_t *data)
{
	NxFileBrowserHandle_t *pFoundData = NULL;

	pFoundData = HLIB_VECTOR_Find(list, nx_filemgr_browser_FindHandleData, (void*)data);
	return pFoundData;
}


static NxFileBrowserHandle_t	*nx_filemgr_browser_FindBrowserHandleByTicket(HxVector_t *list, HINT32 ticket)
{
	NxFileBrowserHandle_t *pFoundData = NULL;

	pFoundData = HLIB_VECTOR_Find(list, nx_filemgr_browser_FindHandleDataByTicket, (void*)ticket);

	return pFoundData;
}

static 	HBOOL	nx_filemgr_browser_IsBrowserHandle(NxFileBrowserHandle_t *handle)
{
	NxFileBrowserContext_t * pstContext = nx_filemgr_browser_GetContext();

	NxFileBrowserHandle_t *pFoundHandle = NULL;

	if (pstContext == NULL)
		return FALSE;

	pFoundHandle = nx_filemgr_browser_FindBrowserHandle(pstContext->hBrowserList, handle);

	if (pFoundHandle == NULL)
	{
		HxLOG_Error("No Exist handle(0x%x). Must to Create FileBrowser Handle.\n", handle);
		return FALSE;
	}
	else
		return TRUE;
}

#define _______DAMA_CALLBACK_FOR_FILE_BROWSING_________
static void nx_filemgr_browser_Dama_callback(int requestId, void *pvUserData, HxList_t *pstResultList, void *pvCbInfo)
{
	HxList_t *pstOrgResultList = pstResultList;
	HxList_t *pFound = NULL;
	DxFileInfo_t* pstFsInfo = NULL;
	NxFileBrowserHandle_t	*pstFoundHandle = NULL;
	NxFileBrowserContext_t * pstContext = nx_filemgr_browser_GetContext();
	HINT32	lDataSize = 0;

	HINT32	nContentCount = HLIB_LIST_Length(pstResultList);

	pstFoundHandle = nx_filemgr_browser_FindBrowserHandleByTicket(pstContext->hBrowserList, requestId);

	if (pstFoundHandle == NULL)
	{
		HxLOG_Error("Not Found NxFileBrowserHandle. ticket(%d)\n", requestId);
		return ;
	}

	if (nContentCount > 0)
	{
		pFound = pstOrgResultList;

		if (pstFoundHandle->itemList && (HLIB_VECTOR_Length(pstFoundHandle->itemList) > 0))
		{
			HxLOG_Debug("remove all BrowserHandle Item list! ticket(%d), listLen(%d)\n",
				pstFoundHandle->ticket, HLIB_VECTOR_Length(pstFoundHandle->itemList));
			HLIB_VECTOR_Delete(pstFoundHandle->itemList);
			pstFoundHandle->itemList = NULL;
		}
		pstFoundHandle->itemList = HLIB_VECTOR_NewEx(NULL, 0,
												(void *(*)(void *))NULL,
												(void *(*)(void *))nx_filemgr_browser_BrowserHandleItemDelete,
												NULL);

		while(pFound)
		{
			pstFsInfo = (DxFileInfo_t*)HLIB_LIST_Data(pFound);

			if (pstFsInfo->eDataType == eDxFILETYPE_AVSTREAM)
			{
				lDataSize = sizeof(HxAV_SteamInfo_t);
			}

			HLIB_VECTOR_Add(pstFoundHandle->itemList, HLIB_STD_MemDup(pstFsInfo, sizeof(DxFileInfo_t) + lDataSize));

			HxLOG_Debug("AddItem. eInterface(%d), eDataType(%d), szFileName(%s)\n",
							pstFsInfo->eInterface, pstFsInfo->eDataType, pstFsInfo->szFileName);

			pFound = HLIB_LIST_Next(pFound);
		}
	}

	APK_FILEMGR_FreeResultList(pstOrgResultList);

	ONDK_GWM_PostMessage(NULL, MSG_CORE_MEDIA_BROWSE_DONE, 0, 0, pstFoundHandle->ticket, nContentCount);
}


/**************************************************************************************************/
#define ___________________Public_functions________________________________________________
/**************************************************************************************************/
HERROR	NX_FILEMGR_BROWSER_Init(void)
{
#if defined(CONFIG_DEBUG)
	nx_filemgr_browser_Command_Init();
#endif

	return	ERR_OK;
}

NxFileBrowserHandle_t *	NX_FILEMGR_BROWSER_CreateHandle(void)
{
	NxFileBrowserContext_t * pstContext = nx_filemgr_browser_GetContext();

	NxFileBrowserHandle_t *newHandle = (NxFileBrowserHandle_t*)NX_APP_Calloc(sizeof(NxFileBrowserHandle_t));

	HLIB_VECTOR_Add(pstContext->hBrowserList, newHandle);

	return newHandle;
}

HERROR	NX_FILEMGR_BROWSER_DestroyHandle(NxFileBrowserHandle_t *handle)
{
	HINT32	nFoundIdx = -1;
	NxFileBrowserContext_t * pstContext = nx_filemgr_browser_GetContext();

	nFoundIdx = HLIB_VECTOR_IndexOf(pstContext->hBrowserList, handle);

	if(nFoundIdx != -1)
	{
		return HLIB_VECTOR_Remove(pstContext->hBrowserList, nFoundIdx);
	}
	else
	{
		HxLOG_Error("Not found NxFileBrowserHandle_t (0x%x)\n", handle);
		return ERR_FAIL;
	}
}

HERROR	NX_FILEMGR_BROWSER_SetPath(NxFileBrowserHandle_t *handle, HCHAR *szPath)
{
	NxFileBrowserHandle_t *pFoundHandle = handle;

	if (nx_filemgr_browser_IsBrowserHandle(pFoundHandle) != TRUE)
	{
		HxLOG_Error("No Exist handle(0x%x). Must to Create FileBrowser Handle.\n", pFoundHandle);
		return ERR_FAIL;
	}

	HxSTD_snprintf(pFoundHandle->szCurPath, DLIB_FILEINFO_NAME_LEN, "%s", szPath);

	return ERR_OK;
}

HCHAR	*NX_FILEMGR_BROWSER_GetCurPath(NxFileBrowserHandle_t *handle)
{
	NxFileBrowserHandle_t *pFoundHandle = handle;

	if (nx_filemgr_browser_IsBrowserHandle(pFoundHandle) != TRUE)
	{
		HxLOG_Error("No Exist handle(0x%x). Must to Create FileBrowser Handle.\n", pFoundHandle);
		return NULL;
	}

	return pFoundHandle->szCurPath;
}

// 상위 폴더로 이동.
HERROR	NX_FILEMGR_BROWSER_MoveToUp(NxFileBrowserHandle_t *handle)
{
	NxFileBrowserHandle_t *pFoundHandle = handle;
	HCHAR *szLastPointer = NULL;

	if (nx_filemgr_browser_IsBrowserHandle(pFoundHandle) != TRUE)
	{
		HxLOG_Error("No Exist handle(0x%x). Must to Create FileBrowser Handle.\n", pFoundHandle);
		return ERR_FAIL;
	}

	szLastPointer = (HCHAR*)strrchr(pFoundHandle->szCurPath, '/');

	if(szLastPointer)
	{
		*szLastPointer = '\0';
		HxLOG_Debug("MoveToUp Path(%s)\n", pFoundHandle->szCurPath);
		return ERR_OK;
	}
	else
		return ERR_FAIL;
}

// add sub folder path
HERROR	NX_FILEMGR_BROWSER_AddFolderPath(NxFileBrowserHandle_t *handle, HCHAR *pszFolderName)
{
	NxFileBrowserHandle_t *pFoundHandle = handle;

	if (nx_filemgr_browser_IsBrowserHandle(pFoundHandle) != TRUE)
	{
		HxLOG_Error("No Exist handle(0x%x). Must to Create FileBrowser Handle.\n", pFoundHandle);
		return ERR_FAIL;
	}

	HxSTD_snprintf(pFoundHandle->szCurPath, DLIB_FILEINFO_NAME_LEN, "%s/%s", pFoundHandle->szCurPath, pszFolderName);

	return ERR_OK;
}

HERROR	NX_FILEMGR_BROWSER_BrowseByOneDepth(NxFileBrowserHandle_t *handle, HCHAR *pszFilter)
{
	NxFileBrowserHandle_t *pFoundHandle = handle;

	HUINT32	ulBrowsingMode;

	if (nx_filemgr_browser_IsBrowserHandle(pFoundHandle) != TRUE)
	{
		HxLOG_Error("No Exist handle(0x%x). Must to Create FileBrowser Handle.\n", pFoundHandle);
		return ERR_FAIL;
	}

	ulBrowsingMode = eAxFILEMGR_MODE_ONEDEPTHVIEW | AxFILEMGR_BROWSE_DIR | AxFILEMGR_BROWSE_FILE | AxFILEMGR_BROWSE_MASK_SUBTYPE;

	if (ERR_OK == APK_FILEMGR_BrowserFiles(pFoundHandle->szCurPath, pszFilter, NULL, ulBrowsingMode, nx_filemgr_browser_Dama_callback, &pFoundHandle->ticket))
	{
		HxLOG_Error("Successed APK_FILEMGR_BrowserFiles(), Received Ticket (%d)\n", pFoundHandle->ticket);
	}
	else
	{
		HxLOG_Error("Can't Execute FileMgr Browser Files!\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	NX_FILEMGR_BROWSER_BrowseByFolderView(NxFileBrowserHandle_t *handle, HCHAR *pszFilter)
{
	NxFileBrowserHandle_t *pFoundHandle = handle;

	HUINT32	ulBrowsingMode;

	if (nx_filemgr_browser_IsBrowserHandle(pFoundHandle) != TRUE)
	{
		HxLOG_Error("No Exist handle(0x%x). Must to Create FileBrowser Handle.\n", pFoundHandle);
		return ERR_FAIL;
	}

	ulBrowsingMode = eAxFILEMGR_MODE_FOLDERVIEW | AxFILEMGR_BROWSE_DIR | AxFILEMGR_BROWSE_FILE;

	if (ERR_OK == APK_FILEMGR_BrowserFiles(pFoundHandle->szCurPath, pszFilter, NULL, ulBrowsingMode, nx_filemgr_browser_Dama_callback, &pFoundHandle->ticket))
	{
		HxLOG_Error("Successed APK_FILEMGR_BrowserFiles(), Received Ticket (%d)\n", pFoundHandle->ticket);
	}
	else
	{
		HxLOG_Error("Can't Execute FileMgr Browser Files!\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	NX_FILEMGR_BROWSER_GetCurListAll(NxFileBrowserHandle_t *handle)
{
	return	ERR_OK;
}



#define ______________RELATED_FILEMGR_BROWSER_COMMAND____________________
/***************************************************************************
Related Search Command
***************************************************************************/
#if defined(CONFIG_DEBUG)

static HINT32 cmdBrowserTest(void *szArgs)
{
	NxFileBrowserHandle_t *testHandle = NULL;

	if (testHandle == NULL)
		testHandle = NX_FILEMGR_BROWSER_CreateHandle();


	//NX_FILEMGR_BROWSER_BrowseByOneDepth(testHandle);

	NX_FILEMGR_BROWSER_SetPath(testHandle, "/media/drive1/Video/Movie Test Files");

	NX_FILEMGR_BROWSER_BrowseByFolderView(testHandle, NXFILE_FILTER_MOVIE);

	NX_FILEMGR_BROWSER_MoveToUp(testHandle);

	NX_FILEMGR_BROWSER_DestroyHandle(testHandle);

	return HxCMD_OK;
}

static void nx_filemgr_browser_Command_Init(void)
{
#define cmdHandle		"nxbrowsercmd"

	HLIB_CMD_RegisterWord(cmdHandle, cmdBrowserTest,
						 (HINT8 *)"testBrowser",
						 (HINT8 *)"testBrowser.",
						 (HINT8 *)"testBrowser");


}

#endif		// #if defined(CONFIG_DEBUG)

