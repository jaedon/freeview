/**************************************************************
*	@file		ondk.c
*	Minimized Graphic Library for DirectFB
*
*	http://www.humaxdigital.com
*
*	@data			2012/09/17
*	@author			humax
*	@brief			Minimized Graphic Library for DirectFB
**************************************************************/


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
* ⓒ 2011-2012 Humax Co., Ltd.
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


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include 	<ondk.h>

#include 	"ondk_dfb.h"

#define DUMY_PROC_ID		0x00000991
#define IMAGE_PATH_LENGTH	512


#define USE_ONDK_DFB_IMAGE_POOL
/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/

typedef struct tagImageProcItem
{
	HUINT8				szPath[IMAGE_PATH_LENGTH];
	HUINT32 			nRef;
	ONDK_Size_t			stImageSize;
	IDirectFBSurface 	*pSurface;
	HxList_t			*pUsingList;
} ImageProcItem_t;

typedef struct tagImagesMgr
{
	ONDK_DFB_IMG_Settings_t stSettings;
	HxHASH_t 				*pImgPoolHash; /* ImageProcItem_t */
} ImageMgr_t;

typedef struct tagImageDBGArg
{
	void 	*pProcId;
	HINT32	totalSize;
} ImageDBGArg_t;

STATIC ImageMgr_t *s_pImageMgr = NULL;

STATIC HULONG		s_ulImgPoolSem = 0;


/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/

STATIC ImageProcItem_t *local_imgpool_ProcItem_New(const HUINT8* path)
{
	ImageProcItem_t *pItem = NULL;

	pItem = ONDK_Calloc(sizeof(ImageProcItem_t));
	ONDK_Assert(pItem);
	if (NULL == pItem)
	{
		return NULL;
	}

	HLIB_STD_StrNCpySafe((HCHAR*)pItem->szPath, (HCHAR*)path, IMAGE_PATH_LENGTH);

	return	pItem;
}


STATIC HBOOL	local_imgpool_ProcItem_Delete(ImageProcItem_t *pItem)
{
	void			*pProcId = NULL;
	HBOOL			bWillDelete = FALSE;
	ImageMgr_t 	*pImgMgr = s_pImageMgr;

	if(pItem->nRef <= 1)
	{
		pItem->nRef = 0;
		bWillDelete = TRUE;
	}

	if(FALSE == bWillDelete)
	{
		if(NULL != pImgMgr->stSettings.cbGetProcId)
		{
			pProcId = pImgMgr->stSettings.cbGetProcId();
			if(HLIB_LIST_Find (pItem->pUsingList, pProcId))
			{
				pItem->pUsingList = HLIB_LIST_Remove(pItem->pUsingList, pProcId);
				pItem->nRef--;
			}
		}
		else
		{
			pItem->nRef--;
		}
	}

	if(TRUE == bWillDelete)
	{
		IDirectFBSurface	*dfbSurface = NULL;

		dfbSurface = (IDirectFBSurface*)pItem->pSurface;
		HLIB_HASH_Remove(pImgMgr->pImgPoolHash, pItem->szPath);
		pItem->pUsingList = HLIB_LIST_RemoveAll(pItem->pUsingList);
		ONDK_Free(pItem);

		ONDK_SURFACE_Destroy(dfbSurface);
	}

	return bWillDelete;
}


STATIC ONDKImageHandle local_imgpool_Get(const HUINT8* path)
{
	void			*pProcId = NULL;
	ImageMgr_t 	*pImgMgr = s_pImageMgr;
	ImageProcItem_t *pItem = NULL;

	pItem = (ImageProcItem_t *)HLIB_HASH_Lookup(pImgMgr->pImgPoolHash, path);
	if(NULL == pItem)
	{
		return NULL;
	}

	if(NULL == pImgMgr->stSettings.cbGetProcId)
	{
		pItem->nRef++;
		return pItem->pSurface;
	}

	pProcId = pImgMgr->stSettings.cbGetProcId();
	if(NULL == HLIB_LIST_Find (pItem->pUsingList, pProcId))
	{
		pItem->nRef++;
		pItem->pUsingList = HLIB_LIST_Append(pItem->pUsingList, pProcId);
	}

	return pItem->pSurface;
}


STATIC void local_imgpool_New(const HUINT8* path, ONDKImageHandle handle, HINT32 w, HINT32 h)
{
	void			*pProcId = NULL;
	ImageProcItem_t *pItem = NULL;
	ImageMgr_t 	*pImgMgr = s_pImageMgr;

	pItem = local_imgpool_ProcItem_New(path);
	ONDK_Assert(pItem);

	pItem->pSurface 	= handle;
	pItem->nRef			= 1;
	pItem->stImageSize	= ONDK_Size(w, h);

	if(NULL != pImgMgr->stSettings.cbGetProcId)
	{
		pProcId = pImgMgr->stSettings.cbGetProcId();
		pItem->pUsingList	= HLIB_LIST_Append(pItem->pUsingList, pProcId);
	}

	HLIB_HASH_Insert(pImgMgr->pImgPoolHash, pItem->szPath, pItem);
}


STATIC HBOOL	local_imgpool_callbackHash_FindByImageHandle(void* key, void* value, void* user_data)
{
	ONDKImageHandle handle = (ONDKImageHandle)user_data;
	ImageProcItem_t *pItem = (ImageProcItem_t *)value;

	if(pItem->pSurface == handle)
	{
		return TRUE;
	}

	NOT_USED_PARAM(key);

	return FALSE;
}


STATIC HBOOL	local_imgpool_callbackHash_FindByUsingProcId(void* key, void* value, void* user_data)
{
	ImageProcItem_t *pItem = (ImageProcItem_t *)value;
	void 			*pProcId = (ImageProcItem_t *)user_data;

	if(HLIB_LIST_Find(pItem->pUsingList, pProcId))
	{
		return TRUE;
	}

	NOT_USED_PARAM(key);

	return FALSE;
}

STATIC HBOOL local_imgpool_Delete(ONDKImageHandle handle)
{
	HBOOL			bWillDelete = FALSE;
	ImageProcItem_t *pItem = NULL;
	ImageMgr_t 	*pImgMgr = s_pImageMgr;

	pItem = (ImageProcItem_t *)HLIB_HASH_Find(pImgMgr->pImgPoolHash, local_imgpool_callbackHash_FindByImageHandle, (void *)handle);
	if(NULL == pItem)
	{
		/*
		 * proc이 destroy 될 때 먼저 삭제되었다.
		 */
		return FALSE;
	}

	bWillDelete = local_imgpool_ProcItem_Delete(pItem);

	return bWillDelete;
}

STATIC void local_imgpool_DeleteByProcId(void *pProcId)
{
	ImageProcItem_t *pItem = NULL;
	ImageMgr_t 	*pImgMgr = s_pImageMgr;

	if(NULL == pProcId)
	{
		ONDK_Assert(0);
		return;
	}

	while(TRUE)
	{
		pItem = (ImageProcItem_t *)HLIB_HASH_Find(pImgMgr->pImgPoolHash, local_imgpool_callbackHash_FindByUsingProcId, (void *)pProcId);
		if(NULL == pItem)
		{
			break;
		}

		(void)local_imgpool_ProcItem_Delete(pItem);
	}
}

STATIC void	local_imgpool_callbackHash_ForeachDump(void*  key , void*  value , void*  user_data)
{
	ImageProcItem_t *pItem = (ImageProcItem_t *)value;
	ImageDBGArg_t 	*pArgs = (void *)user_data;
	void			*pProcId = pArgs->pProcId;
	HxList_t		*pList = NULL;

	if(NULL != pProcId && NULL == HLIB_LIST_Find(pItem->pUsingList, pProcId))
	{
		return;
	}

	if(NULL != pProcId)
	{
		HLIB_CMD_Printf("__img__ ref(%d) (w:%-3d h:%-3d) path(%s)\r\n", pItem->nRef, pItem->stImageSize.w, pItem->stImageSize.h, pItem->szPath);
		pArgs->totalSize += (pItem->stImageSize.w * pItem->stImageSize.h * 4);
	}
	else
	{
		pList = pItem->pUsingList;
		while(pList)
		{
			HLIB_CMD_Printf("__img__ ProcID(0x%x) ref(%d) (w:%-3d h:%-3d) path(%s)\r\n", HLIB_LIST_Data(pList),
							pItem->nRef, pItem->stImageSize.w, pItem->stImageSize.h, pItem->szPath);
			pList = HLIB_LIST_Next(pList);
			pArgs->totalSize += (pItem->stImageSize.w * pItem->stImageSize.h * 4);
		}
	}

	NOT_USED_PARAM(key);
}

STATIC void local_imgpool_Dump_UsingItem(void *pProcId)
{
	ImageMgr_t 		*pImgMgr = s_pImageMgr;
	ImageDBGArg_t	stArg;
	/*
	 * procId == NULL => all dump
	 */
	HxSTD_MemSet(&stArg, 0, sizeof(ImageDBGArg_t));
	stArg.pProcId 	= pProcId;
	stArg.totalSize	= 0;

	HLIB_HASH_Foreach(pImgMgr->pImgPoolHash, local_imgpool_callbackHash_ForeachDump, (void *)&stArg);
	if(NULL == pProcId)
	{
		HLIB_CMD_Printf("__img__ Total Using Image Size(%d,%03d)\r\n", (stArg.totalSize) / 1000, (stArg.totalSize) % 1000);
	}
	else
	{
		HLIB_CMD_Printf("__img__ UI Image Size(%d,%03d)\r\n", (stArg.totalSize) / 1000, (stArg.totalSize) % 1000);
	}
}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/
ONDKImageHandle ONDK_IMG_CreateSelf(const HUINT8* path)
{
	DFBResult err;
	IDirectFBImageProvider	*provider;
	IDirectFBSurface		*dfbSurface;
	IDirectFB				*pDirectFB = NULL;
	DFBSurfaceDescription	surfaceDes;
	ONDKImageHandle			imgHandle = NULL;


	pDirectFB = ONDK_DFB_GetIDirectFB();
	ONDK_Assert(pDirectFB);

	VK_SEM_Get(s_ulImgPoolSem);
	imgHandle = local_imgpool_Get(path);
	VK_SEM_Release(s_ulImgPoolSem);

	if(imgHandle != NULL)
	{
		return imgHandle;
	}

	DFBCHECK(pDirectFB->CreateImageProvider (pDirectFB, (HCHAR*)path, &provider));
	if(err != DFB_OK)
	{
		ONDK_Error("CreateImageProvider Failed.. path(%s)\n", path);
		return NULL;
	}

	HxSTD_MemSet(&surfaceDes, 0, sizeof(DFBSurfaceDescription));
	DFBCHECK(provider->GetSurfaceDescription(provider, &surfaceDes));

	dfbSurface=  (IDirectFBSurface*)ONDK_SURFACE_CreateWithPixelFormatIndex(surfaceDes.width, surfaceDes.height, surfaceDes.pixelformat);

	DFBCHECK (provider->RenderTo (provider, dfbSurface, NULL));
	provider->Release (provider);

	VK_SEM_Get(s_ulImgPoolSem);
	local_imgpool_New(path, (ONDKImageHandle)dfbSurface, surfaceDes.width, surfaceDes.height);
	VK_SEM_Release(s_ulImgPoolSem);

	return (ONDKImageHandle)dfbSurface;
}


ONDKImageHandle ONDK_IMG_Create(HINT32 width, HINT32 height, HINT8 *pixelformat,const HUINT8* path)
{
	DFBResult err;
	IDirectFBImageProvider  *provider;
	IDirectFBSurface		 *dfbSurface;
	IDirectFB			*pDirectFB = NULL;
	ONDKImageHandle			imgHandle = NULL;

	pDirectFB = ONDK_DFB_GetIDirectFB();
	ONDK_Assert(pDirectFB);

	VK_SEM_Get(s_ulImgPoolSem);
	imgHandle = local_imgpool_Get(path);
	VK_SEM_Release(s_ulImgPoolSem);

	if(imgHandle != NULL)
	{
		return imgHandle;
	}

	DFBCHECK(pDirectFB->CreateImageProvider (pDirectFB, (HCHAR*)path, &provider));
	if(err != DFB_OK)
	{
		HxLOG_Critical("Volume Control CreateImageProvider Failed..\n");
		return NULL;
	}

	dfbSurface = (IDirectFBSurface*)ONDK_SURFACE_Create(width,height,pixelformat);

	DFBCHECK (provider->RenderTo (provider, dfbSurface, NULL));
	DFBCHECK (provider->Release (provider));

	VK_SEM_Get(s_ulImgPoolSem);
	local_imgpool_New(path, dfbSurface, width,height);
	VK_SEM_Release(s_ulImgPoolSem);

	return (ONDKImageHandle)dfbSurface;
}

void	 ONDK_IMG_Destroy(ONDKImageHandle surface)
{
#if 0
	/*
		Destroy는 proc이 종료되는 시점에, 자동으로 된다.
	*/
	IDirectFBSurface	*dfbSurface = (IDirectFBSurface*)surface;

	VK_SEM_Get(s_ulImgPoolSem);
	local_imgpool_Delete(surface);
	VK_SEM_Release(s_ulImgPoolSem);
#endif
	(void)surface;
}

void	 ONDK_IMG_DestroyByProcId(HUINT32 procId)
{
	VK_SEM_Get(s_ulImgPoolSem);
	local_imgpool_DeleteByProcId((void *)procId);
	VK_SEM_Release(s_ulImgPoolSem);
}


ONDK_Size_t ONDK_IMG_GetSize( ONDKImageHandle surface)
{
	DFBResult err;
	IDirectFBSurface	*dfbSurface = (IDirectFBSurface*)surface;
	ONDK_Size_t			size;

	DFBCHECK(dfbSurface->GetSize(dfbSurface, &size.w, &size.h));

	return size;
}

void ONDK_IMG_BlitAlpha( ONDKImageHandle srcSurface, ONDKSurfaceHandle destsurface, HINT32 x, HINT32 y, HUINT8 alpha)
{
	DFBResult err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)destsurface;
	IDirectFBSurface	 *dfbSrcSurface = (IDirectFBSurface*)srcSurface;

	DFBRectangle		srcDfbRect = {0, 0, 0, 0};

	DFBCHECK(dfbSrcSurface->GetSize(dfbSrcSurface, &srcDfbRect.w, &srcDfbRect.h));

	ONDK_SURFACE_Blitflag_SetAlphaBlend(destsurface, alpha);
 	DFBCHECK(dfbSurface->Blit(dfbSurface, dfbSrcSurface, &srcDfbRect, x, y));
}

void ONDK_IMG_Blit( ONDKImageHandle srcSurface, ONDKSurfaceHandle destsurface, HINT32 x, HINT32 y)
{
	DFBResult err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)destsurface;
	IDirectFBSurface	 *dfbSrcSurface = (IDirectFBSurface*)srcSurface;

	DFBRectangle		srcDfbRect = {0, 0, 0, 0};

	DFBCHECK(dfbSrcSurface->GetSize(dfbSrcSurface, &srcDfbRect.w, &srcDfbRect.h));

	ONDK_SURFACE_Blitflag_SetBlend(destsurface);
 	DFBCHECK(dfbSurface->Blit(dfbSurface, dfbSrcSurface, &srcDfbRect, x, y));
}


void ONDK_IMG_StretchBlit( ONDKImageHandle srcSurface, ONDKSurfaceHandle destsurface, ONDK_Rect_t dstRect)
{
	DFBResult err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)destsurface;
	IDirectFBSurface	 *dfbSrcSurface = (IDirectFBSurface*)srcSurface;

	DFBRectangle		 dstDfbRect = {dstRect.x, dstRect.y, dstRect.w, dstRect.h};
	DFBRectangle		 srcDfbRect = {0, 0, 0, 0};

	DFBCHECK(dfbSrcSurface->GetSize(dfbSrcSurface, &srcDfbRect.w, &srcDfbRect.h));

	ONDK_SURFACE_Blitflag_SetBlend(destsurface);
 	DFBCHECK(dfbSurface->StretchBlit(dfbSurface, dfbSrcSurface, &srcDfbRect, &dstDfbRect));
}

void ONDK_IMG_StretchBlitAlpha( ONDKImageHandle srcSurface, ONDKSurfaceHandle destsurface, ONDK_Rect_t dstRect, HUINT8 alpha)
{
	DFBResult err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)destsurface;
	IDirectFBSurface	 *dfbSrcSurface = (IDirectFBSurface*)srcSurface;

	DFBRectangle		 dstDfbRect = {dstRect.x, dstRect.y, dstRect.w, dstRect.h};
	DFBRectangle		 srcDfbRect = {0, 0, 0, 0};

	DFBCHECK(dfbSrcSurface->GetSize(dfbSrcSurface, &srcDfbRect.w, &srcDfbRect.h));

	ONDK_SURFACE_Blitflag_SetAlphaBlend(destsurface, alpha);
 	DFBCHECK(dfbSurface->StretchBlit(dfbSurface, dfbSrcSurface, &srcDfbRect, &dstDfbRect));
}

void ONDK_IMG_StretchBlitPartRectAlpha( ONDKImageHandle srcSurface, ONDKSurfaceHandle destsurface, ONDK_Rect_t srcRect, ONDK_Rect_t dstRect, HUINT8 alpha)
{
	DFBResult err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)destsurface;
	IDirectFBSurface	 *dfbSrcSurface = (IDirectFBSurface*)srcSurface;
	HINT32				w = 0, h = 0;
	DFBRectangle		 dstDfbRect = {dstRect.x, dstRect.y, dstRect.w, dstRect.h};
	DFBRectangle		 srcDfbRect = {srcRect.x, srcRect.y, 0, 0};

	DFBCHECK(dfbSrcSurface->GetSize(dfbSrcSurface, &w, &h));

	srcDfbRect.w = MIN((w-srcRect.x), (srcRect.w));
	srcDfbRect.h = MIN((h-srcRect.y), (srcRect.h));

	ONDK_SURFACE_Blitflag_SetAlphaBlend(destsurface, alpha);
 	DFBCHECK(dfbSurface->StretchBlit(dfbSurface, dfbSrcSurface, &srcDfbRect, &dstDfbRect));
}

void ONDK_IMG_StretchBlitPartRect( ONDKImageHandle srcSurface, ONDKSurfaceHandle destsurface, ONDK_Rect_t srcRect, ONDK_Rect_t dstRect)
{
	DFBResult err;
	IDirectFBSurface	 *dfbSurface = (IDirectFBSurface*)destsurface;
	IDirectFBSurface	 *dfbSrcSurface = (IDirectFBSurface*)srcSurface;
	HINT32				w = 0, h = 0;
	DFBRectangle		 dstDfbRect = {dstRect.x, dstRect.y, dstRect.w, dstRect.h};
	DFBRectangle		 srcDfbRect = {srcRect.x, srcRect.y, 0, 0};

	DFBCHECK(dfbSrcSurface->GetSize(dfbSrcSurface, &w, &h));

	srcDfbRect.w = MIN((w-srcRect.x), (srcRect.w));
	srcDfbRect.h = MIN((h-srcRect.y), (srcRect.h));

	ONDK_SURFACE_Blitflag_SetBlend(destsurface);
 	DFBCHECK(dfbSurface->StretchBlit(dfbSurface, dfbSrcSurface, &srcDfbRect, &dstDfbRect));
}

ONDK_Size_t ONDK_IMG_NonCreateOnlyGetSize(const HUINT8* path)
{
	DFBResult err;
	IDirectFBImageProvider	*provider;
	IDirectFB				*pDirectFB = NULL;
	DFBSurfaceDescription	surfaceDes;
	ONDK_Size_t				size = {0, 0};


	pDirectFB = ONDK_DFB_GetIDirectFB();
	ONDK_Assert(pDirectFB);

	DFBCHECK(pDirectFB->CreateImageProvider (pDirectFB, (HCHAR*)path, &provider));
	if(err != DFB_OK)
	{
		HxLOG_Critical("Volume Control CreateImageProvider Failed..\n");
		return size;
	}

	HxSTD_MemSet(&surfaceDes, 0, sizeof(DFBSurfaceDescription));
	DFBCHECK(provider->GetSurfaceDescription(provider, &surfaceDes));
	size = ONDK_Size(surfaceDes.width, surfaceDes.height);
	provider->Release (provider);

	return size;
}


ONDK_Result_t  ONDK_IMG_Init(HINT32 argc, HCHAR **argv)
{
	HERROR hErr = ERR_OK;

	s_pImageMgr =  ONDK_Calloc(sizeof(ImageMgr_t));
	ONDK_Assert (s_pImageMgr);

	s_pImageMgr->pImgPoolHash = HLIB_HASH_New(HLIB_HASH_StrHash, HLIB_HASH_StrEqual);
	ONDK_Assert (s_pImageMgr->pImgPoolHash);

	hErr = VK_SEM_Create(&s_ulImgPoolSem, "ONDK_IMGPOOL", VK_SUSPENDTYPE_FIFO);
	if(hErr != ERR_OK)
	{
		return ONDK_RET_INITFAIL;
	}

	NOT_USED_PARAM(argv);
	NOT_USED_PARAM(argc);

	return ONDK_RET_OK;
}

ONDK_Result_t  ONDK_IMG_Settings(ONDK_DFB_IMG_Settings_t *pSettings)
{
	ImageMgr_t 		*pImgMgr = s_pImageMgr;

	if( NULL == pImgMgr )
	{
		ONDK_Error("[%s:%d] non-init error!!\n", __FUNCTION__, __LINE__);
		return ONDK_RET_INITFAIL;
	}

	HxSTD_MemCopy(&pImgMgr->stSettings, pSettings, sizeof(ONDKSTR_Settings_t));

	return ONDK_RET_OK;
}


void DBG_ONDK_IMG_Dump(void *procId)
{
	VK_SEM_Get(s_ulImgPoolSem);
	local_imgpool_Dump_UsingItem(procId);
	VK_SEM_Release(s_ulImgPoolSem);
}

void DBG_ONDK_IMG_Info(void *procId)
{
	VK_SEM_Get(s_ulImgPoolSem);

	HLIB_CMD_Printf(" + Image Info \r\n");
	//HLIB_CMD_Printf("\t - Total Size = %d\r\n");

	VK_SEM_Release(s_ulImgPoolSem);

	NOT_USED_PARAM(procId);
}


