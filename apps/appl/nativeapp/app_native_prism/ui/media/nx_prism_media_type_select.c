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
 * @file	  		nx_prism_media_type_select.c
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

#include <nx_common.h>
#include <nx_core.h>
#include <nx_port.h>

#include <nx_prism_app.h>
#include <nx_prism_menu_contents.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>
#include <nx_prism_menu_items.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

#define	NX_PRISM_UI_MEDIA_TYPE_X					290
#define	NX_PRISM_UI_MEDIA_TYPE_Y					226
#define	NX_PRISM_UI_MEDIA_TYPE_W					700
#define	NX_PRISM_UI_MEDIA_TYPE_H					269

#define	NX_PRISM_UI_MEDIA_TYPE_LIST_ITEM_W			120

#define	NX_PRISM_UI_MEDIA_TYPE_LIST_X				(NX_PRISM_UI_MEDIA_TYPE_X + 140)
#define	NX_PRISM_UI_MEDIA_TYPE_LIST_Y				(NX_PRISM_UI_MEDIA_TYPE_Y + 84)
#define	NX_PRISM_UI_MEDIA_TYPE_LIST_W				700
#define	NX_PRISM_UI_MEDIA_TYPE_LIST_H				185

#define	NX_PRISM_UI_MEDIA_TYPE_SELECTLIST_MAX				3


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct
{
	// Option Item
	HUINT8			**szItemTextList;
	HUINT8			**szImageNItemTextList;
	HUINT8			**szImageCItemTextList;
	NX_Util_OPTIONList_t	stOptList;
	HBOOL			bSelectStorage;
	HUINT32	    	*pStorageDevIdList;
	HUINT32			*pOptListDevIdList;
	HUINT32			ulCurrentDevId;
	HUINT32			ulStorageListNum;
	HCHAR			*pszTitle;
} Nx_MediaTypeSelectContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_MediaTypeSelectContent_t		s_stMediaTypeSelectContent;
static Nx_PrismListDialog_Content_t		s_stPopupListMediaTypeContent;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_mediaType_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_mediaType_DestroyListPopup(void);

static void				nx_mediaType_CreateMediaTypeListPopup(Nx_MediaTypeSelectContent_t *pstContents);
static ONDK_Result_t	nx_mediaType_MediaTypeInitData(Nx_MediaTypeSelectContent_t *pstContents);
static void				nx_mediaType_MediaTypeInitDataDefault(Nx_MediaTypeSelectContent_t *pstContents);

static HUINT8			**nx_mediaType_StorageOptMakeItemTextList(Nx_MediaTypeSelectContent_t *pstContents, NX_Util_OPTIONList_t *pstOptItemList);
static HUINT32			nx_mediaType_GetStorageItemId(NX_PhysicalStorageData_t *physicalStorageData);
static HCHAR* 			nx_mediaType_GetStorageLabelFromObjInfo(NX_PhysicalStorageData_t *physicalStorageData);
static ONDK_Result_t	nx_mediaType_FreeDeviceIdList(HUINT32 *pDevIdList);
static ONDK_Result_t	nx_mediaType_FreeOptInfoList(Nx_MediaTypeSelectContent_t *pstContents);

static inline Nx_MediaTypeSelectContent_t* nx_mediaType_GetContents(void);

static ONDK_Result_t	nx_mediaType_ActionMediaType(Nx_MediaTypeSelectContent_t *pstContents);
static ONDK_Result_t	nx_mediaType_ChangedSubItemValue(Nx_MediaTypeSelectContent_t *pstContents, HUINT32 ulCurIdx);

static ONDK_Result_t	nx_mediaType_InitSettingsItems(Nx_MediaTypeSelectContent_t *pstContents);

static ONDK_Result_t	nx_mediaType_DrawDefaultUI(Nx_MediaTypeSelectContent_t *pstContents);

static ONDK_Result_t	nx_mediaType_MsgGwmCreate(Nx_MediaTypeSelectContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_mediaType_MsgGwmDestroy(Nx_MediaTypeSelectContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_mediaType_MsgGwmHapiMsg(Nx_MediaTypeSelectContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_mediaType_MsgGwmKeyDown(Nx_MediaTypeSelectContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_mediaType_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stPopupListMediaTypeContent, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_mediaType_DestroyListPopup(void)
{
	return ONDK_GWM_APP_Destroy(nx_mediaType_DisplayListPopup);
}


/******************************************************************************/
#define __Satellite_Option_Functions___________________________________________
/******************************************************************************/

static void		nx_mediaType_CreateMediaTypeListPopup(Nx_MediaTypeSelectContent_t *pstContents)
{
	HINT32			lListX = 0, lListW = 0, lItemW = 0;
	HUINT32			ulTextListNum = 0, ulIndex = 0, ulMaxListNum = 0, ulAttribute = 0;
	HUINT8			**pptextList = NULL, **ppImageListOfItem = NULL;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };


	if (pstContents->stOptList.ulOptItemNum == 0)
	{
		NX_APP_Error("Error!!! Please Check added Option item !!\n");
		return;
	}

	ulTextListNum = pstContents->stOptList.ulOptItemNum;
	if (pstContents->szItemTextList == NULL)
	{
		if (pstContents->bSelectStorage == TRUE)
		{
			pstContents->szItemTextList = nx_mediaType_StorageOptMakeItemTextList(pstContents, &pstContents->stOptList);
		}
		else
		{
			pstContents->szItemTextList = NX_PRISM_UTIL_OptionMakeItemTextList(eNxUtil_OptType_TEXT, &pstContents->stOptList);
		}
		pptextList = pstContents->szItemTextList;
		if (pptextList == NULL)
		{
			NX_APP_Info("Error!!! Please Check added Option item !!\n");
			return;
		}
	}
	else
	{
		pptextList = pstContents->szItemTextList;
	}

	ulIndex = 0;

	// Set Text List
	ulMaxListNum = (ulTextListNum > NX_PRISM_UI_MEDIA_TYPE_SELECTLIST_MAX) ? NX_PRISM_UI_MEDIA_TYPE_SELECTLIST_MAX : ulTextListNum;
	ulAttribute = (eListDlg_OSD_USER | eListDlg_OSD_BG | eListDlg_Title | eListDlg_MultiSelectHApp);
	lListW = (ulMaxListNum * NX_PRISM_UI_MEDIA_TYPE_LIST_ITEM_W) + (NX_PRISM_UI_DLG_MULTISEL_H_LIST_ITEM_TAB * (ulMaxListNum-1));
	if (ulTextListNum >= NX_PRISM_UI_MEDIA_TYPE_SELECTLIST_MAX)
	{
		lListX = NX_PRISM_UI_MEDIA_TYPE_LIST_X;
	}
	else
	{
		lItemW = (NX_PRISM_UI_MEDIA_TYPE_LIST_ITEM_W + (NX_PRISM_UI_DLG_MULTISEL_H_LIST_ITEM_TAB/2));
		lListX = NX_PRISM_UI_MEDIA_TYPE_LIST_Y + ((NX_PRISM_UI_MEDIA_TYPE_SELECTLIST_MAX - ulTextListNum)*lItemW);
	}

	stRect = ONDK_Rect(lListX, NX_PRISM_UI_MEDIA_TYPE_LIST_Y,
						lListW, NX_PRISM_UI_MEDIA_TYPE_LIST_H);
	NX_PRISM_COMP_ListDlg_SetListPopup(&s_stPopupListMediaTypeContent, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
							&stRect, NX_PRISM_UI_MEDIA_TYPE_LIST_H, ulAttribute, ulTextListNum, ulMaxListNum, ulIndex, (HUINT32)pptextList);
	lItemW = (NX_PRISM_UI_MEDIA_TYPE_LIST_ITEM_W + (NX_PRISM_UI_DLG_MULTISEL_H_LIST_ITEM_TAB/2));
	NX_PRISM_COMP_ListDlg_SetLineWithSpace(&s_stPopupListMediaTypeContent, lItemW);
	NX_PRISM_COMP_ListDlg_SetFontSize(&s_stPopupListMediaTypeContent, NX_PRISM_FONT_SIZE_22);
	NX_PRISM_COMP_ListDlg_SetTitle(&s_stPopupListMediaTypeContent, (HUINT8*)pstContents->pszTitle);

	if (ulAttribute & eListDlg_OSD_USER)
	{
		stRect = ONDK_Rect(NX_PRISM_UI_MEDIA_TYPE_X, NX_PRISM_UI_MEDIA_TYPE_Y,
							NX_PRISM_UI_MEDIA_TYPE_W, NX_PRISM_UI_MEDIA_TYPE_H);
		NX_PRISM_COMP_ListDlg_SetExternalArea(&s_stPopupListMediaTypeContent, &stRect);
	}

	// Set Normal Image List
	if (pstContents->szImageNItemTextList == NULL)
	{
		pstContents->szImageNItemTextList = NX_PRISM_UTIL_OptionMakeItemTextList(eNxUtil_OptType_IMG_N, &pstContents->stOptList);
		ppImageListOfItem = pstContents->szImageNItemTextList;
		if (ppImageListOfItem == NULL)
		{
			NX_APP_Info("Error!!! Please Check added Option Normal Image item !!\n");
			return;
		}
	}
	else
	{
		ppImageListOfItem = pstContents->szImageNItemTextList;
	}

	NX_PRISM_COMP_ListDlg_SetImageList(&s_stPopupListMediaTypeContent, (HUINT32)ppImageListOfItem);

	// Set Cursor Image List
	if (pstContents->szImageCItemTextList == NULL)
	{
		pstContents->szImageCItemTextList = NX_PRISM_UTIL_OptionMakeItemTextList(eNxUtil_OptType_IMG_C, &pstContents->stOptList);
		ppImageListOfItem = pstContents->szImageCItemTextList;
		if (ppImageListOfItem == NULL)
		{
			NX_APP_Info("Error!!! Please Check added Option Cursor Image item !!\n");
			return;
		}
	}
	else
	{
		ppImageListOfItem = pstContents->szImageCItemTextList;
	}

	NX_PRISM_COMP_ListDlg_SetImageExtList(&s_stPopupListMediaTypeContent, (HUINT32)ppImageListOfItem);

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_mediaType_DisplayListPopup", APP_DEFAULT_PRIORITY, nx_mediaType_DisplayListPopup, 0, 0, 0, 0);
}


static ONDK_Result_t	nx_mediaType_MediaTypeInitData(Nx_MediaTypeSelectContent_t *pstContents)
{
	nx_mediaType_FreeOptInfoList(pstContents);

	HxSTD_MemSet(&pstContents->stOptList, 0x00, sizeof(NX_Util_OPTIONList_t));

	nx_mediaType_MediaTypeInitDataDefault(pstContents);

	return ONDK_RET_OK;
}

static void		nx_mediaType_MediaTypeInitDataDefault(Nx_MediaTypeSelectContent_t *pstContents)
{
	HUINT32						ulIndex = 0, ulLength = 0, ulItemId = 0, ulOptListIndex = 0;
	HERROR						hError = ERR_FAIL;
	NX_PhysicalStorageData_t	*physicalStorageData = NULL;

	if (pstContents->bSelectStorage == TRUE)
	{
		ulLength = pstContents->ulStorageListNum;

		// Init Option List Device Id List
		nx_mediaType_FreeDeviceIdList(pstContents->pOptListDevIdList);
		pstContents->pOptListDevIdList = (HUINT32*)NX_APP_Malloc(sizeof(HUINT32) * ulLength);
		if (pstContents->pOptListDevIdList == NULL)
		{
			NX_APP_Error("Error!!! NX_APP_Malloc() Fail!!\n");
			return;
		}
		HxSTD_MemSet(pstContents->pOptListDevIdList, 0x00, sizeof(HUINT32) * ulLength);

		// Get Device Item
		for (ulIndex = 0; ulIndex < ulLength; ulIndex++)
		{
			if (pstContents->ulCurrentDevId == pstContents->pStorageDevIdList[ulIndex])
			{
				continue;
			}
			hError = NX_FILEMGR_STORAGE_GetPhysicalStorageInfoWithDevId(pstContents->pStorageDevIdList[ulIndex], &physicalStorageData);
			if (hError != ERR_OK || physicalStorageData == NULL)
			{
				NX_APP_Error("ERROR!!! Get Storage Info Fail !!\n");
				continue;
			}
			ulItemId = nx_mediaType_GetStorageItemId(physicalStorageData);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, ulItemId);
			pstContents->pOptListDevIdList[ulOptListIndex] = pstContents->pStorageDevIdList[ulIndex];
			ulOptListIndex++;
		}
	}
	else
	{
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_MediaTypeVideo);
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_MediaTypePhoto);
		NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_MediaTypeMusic);
	}
}

static HUINT8	**nx_mediaType_StorageOptMakeItemTextList(Nx_MediaTypeSelectContent_t *pstContents, NX_Util_OPTIONList_t *pstOptItemList)
{
	HUINT32 		ulIndex = 0, ulArraySize = 0;
	HUINT32 		ulTextLen = 0;
	HUINT8			*pucSrcText = NULL, *pucDstText = NULL;
	HUINT8			**ppucTextArray = NULL;
	HERROR			hErr = ERR_FAIL;
	NX_PhysicalStorageData_t	*physicalStorageData = NULL;


	ulArraySize = pstOptItemList->ulOptItemNum;
	if (ulArraySize == 0)
	{
		NX_APP_Error(" Error!!! ulItemList list ulItemNum is Zero!!\n");
		return	NULL;
	}

	ppucTextArray = (HUINT8 **)NX_APP_Malloc(sizeof(HUINT8 *) * ulArraySize);
	if (ppucTextArray == NULL)
	{
		NX_APP_Error("Error!!! NX_APP_Malloc() Fail!!\n");
		return	NULL;
	}

	HxSTD_memset(ppucTextArray, 0, sizeof(HUINT8 *) * ulArraySize);
	for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
	{
		hErr = NX_FILEMGR_STORAGE_GetPhysicalStorageInfoWithDevId(pstContents->pOptListDevIdList[ulIndex], &physicalStorageData);
		if (hErr != ERR_OK || physicalStorageData == NULL)
		{
			NX_APP_Error("ERROR!!! Get Storage Info Fail !!\n");
			continue;
		}

		pucSrcText = (HUINT8*)nx_mediaType_GetStorageLabelFromObjInfo(physicalStorageData);
		if (pucSrcText == NULL)
		{
			for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
			{
				if (ppucTextArray[ulIndex] != NULL)
				{
					NX_APP_Free (ppucTextArray[ulIndex]);
				}
			}

			NX_APP_Free (ppucTextArray);
			return	NULL;
		}

		ulTextLen  = HxSTD_StrLen ((char*)pucSrcText);

		// Destination Text
		pucDstText = (HUINT8 *)NX_APP_Malloc (ulTextLen + 1);
		if (pucDstText == NULL)
		{
			for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
			{
				if (ppucTextArray[ulIndex] != NULL)
				{
					NX_APP_Free (ppucTextArray[ulIndex]);
				}
			}

			NX_APP_Free (ppucTextArray);
			return	NULL;
		}

		HxSTD_memset(pucDstText, 0x0, ulTextLen + 1);
		pucDstText[0] = '\0';

		HxSTD_StrCpy ((char*)pucDstText, (char*)pucSrcText);
		ppucTextArray[ulIndex] = pucDstText;
	}

	return	ppucTextArray;
}

static HUINT32	nx_mediaType_GetStorageItemId(NX_PhysicalStorageData_t *physicalStorageData)
{
	if (physicalStorageData == NULL)
	{
		return eNxUtil_OptItem_None;
	}

	switch(physicalStorageData->eSort)
	{
		case eStorageSort_InternalPVRHDD:
			return eNxUtil_OptItem_InternalHDD;

		case eStorageSort_externalSubPVRHDD:
			return eNxUtil_OptItem_DetachableHDD;

		case eStorageSort_externalPVRHDD:
		case eStorageSort_externalHDD:
		case eStorageSort_UsbMemory:
			return eNxUtil_OptItem_USB;

		case eStorageSort_SdMemory:
			return eNxUtil_OptItem_SdCard;

		default:
			return eNxUtil_OptItem_None;
	}
}

static HCHAR* 	nx_mediaType_GetStorageLabelFromObjInfo(NX_PhysicalStorageData_t *physicalStorageData)
{
	if (physicalStorageData == NULL)
	{
		NX_APP_Error("ERROR!!! physicalStorageData is NULL !!\n");
		return NULL;
	}

	switch(physicalStorageData->eSort)
	{
		case eStorageSort_InternalPVRHDD:
			return (HCHAR*)ONDK_GetString(RES_INTERNAL_HDD_ID);

		case eStorageSort_externalSubPVRHDD:
			return (HCHAR*)ONDK_GetString(RES_DETACHABLE_HDD_ID);

		case eStorageSort_externalPVRHDD:
		case eStorageSort_externalHDD:
		case eStorageSort_UsbMemory:
			if (physicalStorageData->pszLabel == NULL)
			{
				return (HCHAR*)ONDK_GetString(RES_UNKNOWN_01_ID);
			}
			else
			{
				return physicalStorageData->pszLabel;
			}

		case eStorageSort_SdMemory:
			if (physicalStorageData->pszLabel == NULL)
			{
				return (HCHAR*)ONDK_GetString(RES_SD_CARD_01_ID);
			}
			else
			{
				return physicalStorageData->pszLabel;
			}

		default:
			return NULL;
	}
}

static ONDK_Result_t	nx_mediaType_FreeDeviceIdList(HUINT32 *pDevIdList)
{
	if(pDevIdList != NULL)
	{
		NX_APP_Free (pDevIdList);
		pDevIdList = NULL;
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_mediaType_FreeOptInfoList(Nx_MediaTypeSelectContent_t *pstContents)
{
	if (pstContents->szItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szItemTextList);
		pstContents->szItemTextList = NULL;
	}
	if (pstContents->szImageNItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szImageNItemTextList);
		pstContents->szImageNItemTextList = NULL;
	}
	if (pstContents->szImageCItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szImageCItemTextList);
		pstContents->szImageCItemTextList = NULL;
	}

	return ONDK_RET_OK;
}

/******************************************************************************/
#define __Media_Option_Functions___________________________________________
/******************************************************************************/

static inline Nx_MediaTypeSelectContent_t* nx_mediaType_GetContents(void)
{
	return	&s_stMediaTypeSelectContent;
}

static ONDK_Result_t	nx_mediaType_ActionMediaType(Nx_MediaTypeSelectContent_t *pstContents)
{
	// Init Data
	nx_mediaType_MediaTypeInitData(pstContents);

	// Show List
	nx_mediaType_CreateMediaTypeListPopup(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_mediaType_ChangedSubItemValue(Nx_MediaTypeSelectContent_t *pstContents, HUINT32 ulCurIdx)
{
	eNxUiUtil_OptType_t		eCurMediaTypeItem = eNxUtil_OptItem_None;
	HINT32					lActionMenu = eNX_UCT_Meida;
	HUINT32					ulProperty = 0;

	nx_mediaType_DestroyListPopup();
	if (ulCurIdx >= pstContents->stOptList.ulOptItemNum)
	{
		NX_APP_Error("Error!!! Unknow Index - ulCurIdx(%d)!! \n", ulCurIdx);
		return ONDK_RET_FAIL;
	}

	if (pstContents->bSelectStorage == TRUE)
	{
		ONDK_GWM_PostMessage(NULL, MSG_APP_MEDIA_TARGET_STORAGE_SELECTED, 0, pstContents->pOptListDevIdList[ulCurIdx], 0, 0);
		ONDK_GWM_APP_Destroy(0);
	}
	else
	{
		eCurMediaTypeItem = (eNxUiUtil_OptType_t)pstContents->stOptList.aeOptItemList[ulCurIdx];
		ONDK_GWM_APP_Destroy(0);
		if (eCurMediaTypeItem == (HUINT32)eNxUtil_OptItem_MediaTypeVideo)
		{
			ONDK_GWM_PostMessage(NULL, MSG_APP_HOMEMENU_UI_LAUNCH, 0, lActionMenu, 0, 0);
			ulProperty = (APP_DEFAULT_PRIORITY | UISTATE_MEDIA);
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MEDIA_Proc", ulProperty, NX_PRISM_MEDIA_Proc, HANDLE_NULL, eNxUtil_OptItem_MediaTypeVideo, 0, 0);
		}
		else if (eCurMediaTypeItem == (HUINT32)eNxUtil_OptItem_MediaTypePhoto)
		{
			ONDK_GWM_PostMessage(NULL, MSG_APP_HOMEMENU_UI_LAUNCH, 0, lActionMenu, 0, 0);
			ulProperty = (APP_DEFAULT_PRIORITY | UISTATE_MEDIA);
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MEDIA_Proc", ulProperty, NX_PRISM_MEDIA_Proc, HANDLE_NULL, eNxUtil_OptItem_MediaTypePhoto, 0, 0);
		}
		else if (eCurMediaTypeItem == (HUINT32)eNxUtil_OptItem_MediaTypeMusic)
		{
			//ONDK_GWM_PostMessage(NULL, MSG_APP_HOMEMENU_UI_LAUNCH, 0, lActionMenu, 0, 0);
			//ulProperty = (APP_DEFAULT_PRIORITY | UISTATE_MEDIA);
			//ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MEDIA_Proc", ulProperty, NX_PRISM_MEDIA_Proc, HANDLE_NULL, eNxUtil_OptItem_MediaTypeMusic, 0, 0);
		}
		else
		{
			NX_APP_Error("Error!!! Unknow Item - Item(%d)!! \n", eCurMediaTypeItem);
			return ONDK_RET_FAIL;
		}
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(ulCurIdx);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_mediaType_InitSettingsItems(Nx_MediaTypeSelectContent_t *pstContents)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !! \n");
		return ONDK_RET_INITFAIL;
	}

	if (pstContents->szItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szItemTextList);
		pstContents->szItemTextList = NULL;
	}
	if (pstContents->szImageNItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szImageNItemTextList);
		pstContents->szImageNItemTextList = NULL;
	}
	if (pstContents->szImageCItemTextList)
	{
		NX_PRISM_UTIL_OptionDeleteItemTextList(&pstContents->stOptList, pstContents->szImageCItemTextList);
		pstContents->szImageCItemTextList = NULL;
	}

	HxSTD_MemSet(&pstContents->stOptList, 0x00, sizeof(NX_Util_OPTIONList_t));

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_mediaType_DrawDefaultUI(Nx_MediaTypeSelectContent_t *pstContents)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);

	nx_mediaType_ActionMediaType(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_mediaType_MsgGwmCreate(Nx_MediaTypeSelectContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR	hErr = ERR_FAIL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_UI_OPT_POPUP_R_X, NX_PRISM_UI_OPT_POPUP_R_Y, NX_PRISM_UI_OPT_POPUP_WIDTH, NX_PRISM_UI_OPT_POPUP_HEIGHT);

	if (p1 == 0)
	{
		pstContents->pszTitle = (HCHAR*)"Select the item as below.";
	}
	else
	{
		pstContents->pszTitle = (HCHAR*)p1;
	}

	pstContents->bSelectStorage = (HBOOL)p3;

	// Init Menu Item
	nx_mediaType_InitSettingsItems(pstContents);

	if (pstContents->bSelectStorage == TRUE)
	{
		// Get Device List
		pstContents->ulCurrentDevId = (HUINT32)p2;
		hErr = NX_FILEMGR_STORAGE_GetPhysicalStorageDevIdList(&pstContents->pStorageDevIdList, &pstContents->ulStorageListNum);
		if (hErr != ERR_OK || pstContents->pStorageDevIdList == NULL)
		{
			NX_APP_Error("physicalStorageDevIdList is NULL!\n");
			ONDK_GWM_APP_Destroy(0);
			return ONDK_RET_FAIL;
		}
	}

	// Draw Menu Item
	nx_mediaType_DrawDefaultUI(pstContents);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_mediaType_MsgGwmDestroy(Nx_MediaTypeSelectContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	ONDK_GWM_PostMessage(NULL, MSG_APP_MEDIA_TYPE_SELECT_DLG_DESTROY, 0, 0, 0, 0);

	nx_mediaType_DestroyListPopup();

	nx_mediaType_FreeOptInfoList(pstContents);
	HxSTD_MemSet(&pstContents->stOptList, 0x00, sizeof(NX_Util_OPTIONList_t));

	if (pstContents->bSelectStorage == TRUE)
	{
		nx_mediaType_FreeDeviceIdList(pstContents->pStorageDevIdList);
		nx_mediaType_FreeDeviceIdList(pstContents->pOptListDevIdList);
	}

	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MediaTypeSelectContent_t));

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_mediaType_MsgGwmHapiMsg(Nx_MediaTypeSelectContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_mediaType_MsgGwmKeyDown(Nx_MediaTypeSelectContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_EXIT:
		case KEY_GUIDE:
		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		case KEY_OPT:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		case KEY_OK:
		case KEY_ARROWRIGHT:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;

		default:
			NX_APP_Print("nx_mediaType_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

ONDK_Result_t	NX_PRISM_MEDIA_TypeSelect_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MediaTypeSelectContent_t	*pstContents = NULL;

	pstContents = nx_mediaType_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_mediaType_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_mediaType_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
		case MSG_GWM_SCROLLRIGHT:
			NX_APP_Message("MSG_GWM_CLICKED or MSG_GWM_SCROLLRIGHT!\n");
			break;

		case MSG_GWM_SCROLLUP:
			NX_APP_Message("MSG_GWM_SCROLLUP!\n");
			break;

		case MSG_GWM_SCROLLDOWN:
			NX_APP_Message("MSG_GWM_SCROLLDOWN!\n");
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_mediaType_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			eResult = nx_mediaType_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_POPUP_LIST_DESTROYED & MSG_APP_SETTING_VALUE_CHANGED\n");
			if (lMessage == MSG_APP_SETTING_VALUE_CHANGED)
			{
				NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED\n");
				nx_mediaType_DestroyListPopup();
				eResult = nx_mediaType_ChangedSubItemValue(pstContents, p1);
			}
			break;

		case MSG_APP_POPUP_LIST_DESTROYED:
			NX_APP_Message("MSG_APP_POPUP_LIST_DESTROYED!\n");
			ONDK_GWM_APP_Destroy(0);
			break;

		case MSG_APP_HOMEMENU_UI_LAUNCH:
			NX_APP_Message("MSG_APP_HOMEMENU_UI_LAUNCH\n");
			break;

		default:
			break;
	}

	if(eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return	ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

