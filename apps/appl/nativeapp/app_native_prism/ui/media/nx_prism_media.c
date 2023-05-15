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
 * @file	  		nx_prism_media.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_common.h>

#include <nx_prism_app.h>
#include <nx_prism_ui.h>



/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_MEDIA_MAX_SHOW_SERVER_COUNT				9
#define	NX_MEDIA_SERVER_RECENT_COUNT				1
#define	NX_MEDIA_PHOTO_MAX_SHOW_ROW_COUNT			3
#define	NX_MEDIA_PHOTO_MAX_SHOW_COL_COUNT			5
#define	NX_MEDIA_MAX_VIDEO_SHOW_ITEM_COUNT			7
#define	NX_MEDIA_MAX_PHOTO_SHOW_ITEM_COUNT			(NX_MEDIA_PHOTO_MAX_SHOW_ROW_COUNT * NX_MEDIA_PHOTO_MAX_SHOW_COL_COUNT)



// timeout
#define NX_MEDIA_TIMEOUT_ID							0xf000
#define NX_MEDIA_TIMEOUT							(3*60*NX_COMMON_TIME_SECOND)	/* 3 minutes */
#define NX_MEDIA_TIME_THUMB_UPDATE					(5*NX_COMMON_TIME_SECOND)		/* 5 Second */

// Path
#define	NX_MEDIA_PATH_X								220
#define	NX_MEDIA_PATH_Y								41
#define	NX_MEDIA_PATH_W								820
#define	NX_MEDIA_PATH_H								53

// server
#define	NX_MEDIA_SERVER_LIST_X						60
#define	NX_MEDIA_SERVER_LIST_Y						103

#define	NX_MEDIA_SERVER_ITEM_W						310
#define	NX_MEDIA_SERVER_ITEM_H						58

#define	NX_MEDIA_SERVER_SUBJECT_MARGIN				18
#define	NX_MEDIA_SERVER_ITEM_MARGIN					8

#define	NX_MEDIA_SERVER_ITEM_TXT_W					268
#define	NX_MEDIA_SERVER_ITEM_TXT_X_TAB				55
#define	NX_MEDIA_SERVER_ITEM_TXT_Y_TAB				(37 - 24)

#define	NX_MEDIA_SERVER_ITEM_ICON_X_TAB				10
#define	NX_MEDIA_SERVER_ITEM_ICON_Y_TAB				11
#define	NX_MEDIA_SERVER_ITEM_ICON_W					36
#define	NX_MEDIA_SERVER_ITEM_ICON_H					36

// contents
#define	NX_MEDIA_CONTENTS_BG_X						396
#define	NX_MEDIA_CONTENTS_BG_Y						103
#define	NX_MEDIA_CONTENTS_BG_W						810
#define	NX_MEDIA_CONTENTS_BG_H						617
#define	NX_MEDIA_CONTENTS_BG_MARGIN_W				10
#define	NX_MEDIA_CONTENTS_BG_MARGIN_H				7

#define	NX_MEDIA_CONTENTS_NOITEM_IMG_X				715
#define	NX_MEDIA_CONTENTS_NOITEM_IMG_Y				207
#define	NX_MEDIA_CONTENTS_NOITEM_IMG_W				136
#define	NX_MEDIA_CONTENTS_NOITEM_IMG_H				123

#define	NX_MEDIA_CONTENTS_NOITEM_MAINTXT_X			445
#define	NX_MEDIA_CONTENTS_NOITEM_MAINTXT_Y			361
#define	NX_MEDIA_CONTENTS_NOITEM_MAINTXT_W			712
#define	NX_MEDIA_CONTENTS_NOITEM_MAINTXT_H			(NX_PRISM_FONT_SIZE_28 + NX_PRISM_FONT_VSPACE)

#define	NX_MEDIA_CONTENTS_NOITEM_SUBTXT_X			445
#define	NX_MEDIA_CONTENTS_NOITEM_SUBTXT_Y			424
#define	NX_MEDIA_CONTENTS_NOITEM_SUBTXT_W			712
#define	NX_MEDIA_CONTENTS_NOITEM_SUBTXT_H			(NX_PRISM_FONT_SIZE_24 + NX_PRISM_FONT_VSPACE) * 7

#define	NX_MEDIA_CONTENTS_ITEM_CURSOR_W				790
#define	NX_MEDIA_CONTENTS_ITEM_CURSOR_H				81

#define	NX_MEDIA_CONTENTS_ITEM_MARGIN_H				4

#define	NX_MEDIA_CONTENTS_ITEM_NAME_X_TAB			144
#define	NX_MEDIA_CONTENTS_ITEM_NAME_Y_TAB			17
#define	NX_MEDIA_CONTENTS_ITEM_NAME_W				628

#define	NX_MEDIA_CONTENTS_ITEM_INFO_X_TAB			144
#define	NX_MEDIA_CONTENTS_ITEM_INFO_Y_TAB			49

#define	NX_MEDIA_CONTENTS_ITEM_FOLDER_X_TAB			34
#define	NX_MEDIA_CONTENTS_ITEM_FOLDER_Y_TAB			15
#define	NX_MEDIA_CONTENTS_ITEM_FOLDER_W				67
#define	NX_MEDIA_CONTENTS_ITEM_FOLDER_H				52

#define	NX_MEDIA_CONTENTS_ITEM_THUMB_X_TAB			5
#define	NX_MEDIA_CONTENTS_ITEM_THUMB_Y_TAB			5
#define	NX_MEDIA_CONTENTS_ITEM_THUMB_W				123
#define	NX_MEDIA_CONTENTS_ITEM_THUMB_H				69

#define	NX_MEDIA_CONTENTS_ITEM_CHECH_X_TAB			6
#define	NX_MEDIA_CONTENTS_ITEM_CHECH_Y_TAB			21
#define	NX_MEDIA_CONTENTS_ITEM_CHECH_W				38
#define	NX_MEDIA_CONTENTS_ITEM_CHECH_H				38
#define	NX_MEDIA_CONTENTS_ITEM_CHECK_FILE_GAP		11
#define	NX_MEDIA_CONTENTS_ITEM_CHECK_FOLDER_GAP		39

// photo contents
#define	NX_MEDIA_PHOTO_CONTENTS_MAX_COL				5
#define	NX_MEDIA_PHOTO_CONTENTS_MAX_ROW				3

#define	NX_MEDIA_PHOTO_CONTENTS_BG_MARGIN_W			18
#define	NX_MEDIA_PHOTO_CONTENTS_BG_MARGIN_H			13

#define	NX_MEDIA_PHOTO_CONTENTS_ITEM_CURSOR_W		134
#define	NX_MEDIA_PHOTO_CONTENTS_ITEM_CURSOR_H		134
#define	NX_MEDIA_PHOTO_CONTENTS_ITEM_CURSOR_BORDER	5

#define	NX_MEDIA_PHOTO_CONTENTS_ITEM_MARGIN_W		26
#define	NX_MEDIA_PHOTO_CONTENTS_ITEM_MARGIN_H		45

#define	NX_MEDIA_PHOTO_CONTENTS_ITEM_NAME_X_TAB		0
#define	NX_MEDIA_PHOTO_CONTENTS_ITEM_NAME_Y_TAB		145

#define	NX_MEDIA_PHOTO_CONTENTS_ITEM_FOLDER_X_TAB	34
#define	NX_MEDIA_PHOTO_CONTENTS_ITEM_FOLDER_Y_TAB	41
#define	NX_MEDIA_PHOTO_CONTENTS_ITEM_FOLDER_W		67
#define	NX_MEDIA_PHOTO_CONTENTS_ITEM_FOLDER_H		52

#define	NX_MEDIA_PHOTO_CONTENTS_ITEM_THUMB_X_TAB	2
#define	NX_MEDIA_PHOTO_CONTENTS_ITEM_THUMB_Y_TAB	2
#define	NX_MEDIA_PHOTO_CONTENTS_ITEM_THUMB_W		130
#define	NX_MEDIA_PHOTO_CONTENTS_ITEM_THUMB_H		130

#define	NX_MEDIA_PHOTO_CONTENTS_ITEM_CHECK_X_TAB	5
#define	NX_MEDIA_PHOTO_CONTENTS_ITEM_CHECK_Y_TAB	5
#define	NX_MEDIA_PHOTO_CONTENTS_ITEM_CHECK_W		38
#define	NX_MEDIA_PHOTO_CONTENTS_ITEM_CHECK_H		38
#define	NX_MEDIA_PHOTO_CONTENTS_ITEM_CHECK_FOLDER_Y	52

// Option
#define	NX_MEDIA_OPT_X								820
#define	NX_MEDIA_OPT_Y								674
#define	NX_MEDIA_OPT_W								400
#define	NX_MEDIA_OPT_H								30

#define	NX_PRISM_MEDIA_OPTION_LIST_Y2				694
#define	NX_PRISM_MEDIA_OPTION_SELECTLIST_MAX		5

#define	NX_PRISM_MEDIA_OPTION_DIM_X					560
#define	NX_PRISM_MEDIA_OPTION_DIM_Y					0
#define	NX_PRISM_MEDIA_OPTION_DIM_W					660
#define	NX_PRISM_MEDIA_OPTION_DIM_H					656

#define	NX_PRISM_MEDIA_OPERATING_BG_X				1010
#define	NX_PRISM_MEDIA_OPERATING_BG_Y				0
#define	NX_PRISM_MEDIA_OPERATING_BG_W				270
#define	NX_PRISM_MEDIA_OPERATING_BG_H				96

#define	NX_PRISM_MEDIA_OPERATING_IMG_X				(NX_PRISM_MEDIA_OPERATING_BG_X + 20)
#define	NX_PRISM_MEDIA_OPERATING_IMG_Y				(NX_PRISM_MEDIA_OPERATING_BG_Y + 40)
#define	NX_PRISM_MEDIA_OPERATING_IMG_W				46
#define	NX_PRISM_MEDIA_OPERATING_IMG_H				46

#define	NX_PRISM_MEDIA_OPERATING_TXT_X				(NX_PRISM_MEDIA_OPERATING_BG_X + 86)
#define	NX_PRISM_MEDIA_OPERATING_TXT_Y				(NX_PRISM_MEDIA_OPERATING_BG_Y + 71 - NX_PRISM_FONT_SIZE_22)
#define	NX_PRISM_MEDIA_OPERATING_TXT_W				170
#define	NX_PRISM_MEDIA_OPERATING_TXT_H				96

// Scroll bar
#define	NX_PRISM_MEDIA_SCROLL_MG					(NX_MEDIA_CONTENTS_BG_W - NX_MEDIA_CONTENTS_ITEM_CURSOR_W) / 2 + 8
#define	NX_PRISM_MEDIA_SCROLL_X						1
#define	NX_PRISM_MEDIA_SCROLL_W						1
#define	NX_PRISM_MEDIA_SCROLL_H						590
#define	NX_PRISM_MEDIA_SCROLLBALL_W					3
#define	NX_PRISM_MEDIA_SCROLLBALL_MIN_H				5

// bottom dim
#define	NX_PRISM_MEDIA_BOTTOM_DIM_X					0
#define	NX_PRISM_MEDIA_BOTTOM_DIM_Y					630
#define	NX_PRISM_MEDIA_BOTTOM_DIM_W					1280
#define	NX_PRISM_MEDIA_BOTTOM_DIM_H					90

#define	NX_PRISM_MEDIA_SELECT_HELP_X				60
#define	NX_PRISM_MEDIA_SELECT_HELP_Y				121
#define	NX_PRISM_MEDIA_SELECT_HELP_W				310
#define	NX_PRISM_MEDIA_SELECT_HELP_H				(NX_PRISM_FONT_SIZE_24 + NX_PRISM_FONT_VSPACE) * 3

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef enum
{
	// Progress
	eNxMedia_Dialog_Progress_Connecting,
	eNxMedia_Dialog_Progress_Deleting,
	eNxMedia_Dialog_Progress_Copying,
	eNxMedia_Dialog_Progress_Moving,

	// Select
	eNxMedia_Dialog_Select_WatchFromBegin,
	eNxMedia_Dialog_Select_DeleteOneFile,
	eNxMedia_Dialog_Select_DeleteMultiFile,
	eNxMedia_Dialog_Select_DeleteAllFiles,
	eNxMedia_Dialog_Select_CopyTarget,
	eNxMedia_Dialog_Select_MoveTarget,

	// Confirm
	eNxMedia_Dialog_Confirm_CannotCopyFormatInvalid,
	eNxMedia_Dialog_Confirm_CannotCopyLackSpace,
	eNxMedia_Dialog_Confirm_CannotCopyFileSize,
	eNxMedia_Dialog_Confirm_DamageOccur,

	// Responsive
	eNxMedia_Dialog_Response_InvalidVideo,
	eNxMedia_Dialog_Response_InvalidAudio,
	eNxMedia_Dialog_Response_NoDeviceCopy,
	eNxMedia_Dialog_Response_NoDeviceMove,
	eNxMedia_Dialog_Response_SelectAtLeastOne,

	eNxMedia_Dialog_None
} Nx_MediaDialogType_e;

typedef enum {
	eNxMedia_ServerType_Recent,
	eNxMedia_ServerType_Storage,
	eNxMedia_ServerType_Dlna
} Nx_MediaServerType_e;

typedef struct {
	HUINT8			szPath[DLIB_FILEINFO_NAME_LEN];
	HBOOL			bGetThumb;
} Nx_MediaThumbRequest_t;

typedef struct {
	HUINT8		szTitle[MAX_ELEMENT_TEXT_LENGTH];
	HUINT8		szThumbnail[MAX_ELEMENT_TEXT_LENGTH];
	HUINT8		szDate[MAX_ELEMENT_TEXT_LENGTH];
	HBOOL		bDirectory;
	HBOOL		bHasSubFile;
	HUINT32		ulDuration;
} Nx_MediaContentData_t;

typedef struct tagMediaContext {
	/* Storage */
	HUINT32 		ulStorageListNum;
	HUINT32 		*pStorageDevIdList;

	/* DLNA DMS */
#if defined(CONFIG_MW_MM_DLNA)
	HxVector_t		*pDmsList;
	HxVector_t		*pDlnaContentList;
	HUINT8			szParentCID[MAX_ELEMENT_TEXT_LENGTH];
#endif
	HUINT32			ulDmsListNum;

	/* Common Server */
	HUINT32					ulCurrentServerId;
	HUINT8					ucCurrentServerPath[MAX_ELEMENT_TEXT_LENGTH];
	Nx_MediaServerType_e	eCurrentServerType;
	HUINT8					szCurPath[MAX_ELEMENT_TEXT_LENGTH];

	/* Browsed Contents */
	HINT32					lShowItemCnt;
	NxFileBrowserHandle_t	*browseHandle;
	Nx_MediaThumbRequest_t	*thumbRequest;
	HUINT32					ulCurrentContentLength;
	HBOOL					bUpperFolder;
	HxVector_t				*playList;  // Contents List for Play
	HBOOL					bSelectMode;    // Contents Select Mode
	HBOOL					*pSelectedList; // Contents Selected List

	// Option Item
	HUINT8			**szItemTextList;
	HUINT8			**szImageNItemTextList;
	HUINT8			**szImageCItemTextList;
	NX_Util_OPTIONList_t		stOptList;

	// Else
	HUINT8			ucTitle[MAX_ELEMENT_TEXT_LENGTH];
	HUINT8			szDate[NX_TEXT_SIZE_32];
	HUINT8			szTime[NX_TEXT_SIZE_32];
	HUINT32			ulCurrentDlg;
	HUINT32			ulAniIconIdx;
	HUINT32			ulContentsType;

	/* Grid view data */
	HINT32			lCount;
	HINT32			lPhotoThumbReqIdx;

} Nx_MediaContext_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_MediaContext_t		s_stMediaContent;
static NXUI_DialogInstance_t	s_stDialogProgressInstance;
static NXUI_DialogInstance_t	s_stDialogSelectiveInstance;

// option
static Nx_PrismListDialog_Content_t		s_stMediaOptContents;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static inline Nx_MediaContext_t	*nx_media_GetContents(void);
static ONDK_Result_t	nx_media_SelectiveMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static void				nx_media_Ui_SelectiveMsgCreate(Nx_MediaDialogType_e eDialogType);
static ONDK_Result_t	nx_media_SelectiveMsgDestroy(void);
static ONDK_Result_t	nx_media_ProcessingMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static void				nx_media_Ui_ProcessingMsgCreate(Nx_MediaDialogType_e eDialogType);
static void				nx_media_ProcessingMsgDestroy(void);
static ONDK_Result_t	nx_media_ResponseMsgProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);
static void				nx_media_Ui_ResponseMsgCreate(HUINT8* pszString);
static void				nx_media_ResponseMsgDestroy(void);
static ONDK_Result_t	nx_media_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_media_DestroyListPopup(void);

static ONDK_Result_t	nx_media_FreeDeviceIdList (HUINT32 *pDevIdList);
static ONDK_Result_t	nx_media_FreeSelectedList (HBOOL *pSelectedList);
static ONDK_Result_t	nx_media_FreeOptionTextList (Nx_MediaContext_t *pstContents);

static ONDK_Result_t	nx_media_LoadStorageDevIdList(Nx_MediaContext_t *pstContents);
static ONDK_Result_t	nx_media_DlnaDmsStartScan(void);
static ONDK_Result_t	nx_media_DlnaDmsStopScan(Nx_MediaContext_t *pstContents);
static void	*			nx_media_CloneContentObject(const void *pvSelf);
static ONDK_Result_t	nx_media_MakePlayList(Nx_MediaContext_t *pstContents, HINT32 lCurIndex);
static HBOOL			nx_media_CheckStorageEjectableWithIdx(Nx_MediaContext_t *pstContents, HINT32 lIndex);
static ONDK_Result_t	nx_media_GetDurationStr(HCHAR* pszDuration, HUINT32	ulDuration);
static HINT32			nx_media_GetThumbImgObjId(HINT32 lIndex);
static HCHAR* 			nx_media_GetStorageLabelFromObjInfo(NX_PhysicalStorageData_t *physicalStorageData);
static HCHAR* 			nx_media_GetStorageIconIdFromObjInfo(NX_PhysicalStorageData_t *physicalStorageData, HBOOL bFocus);
static ONDK_Result_t 	nx_media_GetContentData(Nx_MediaContext_t *pstContents, Nx_MediaContentData_t *pstContentData, Nx_MediaServerType_e	eServerType, HINT32	lContentIdx);

static void				nx_media_DrawBackground(void);
static void				nx_media_UpdateCurrentTime(Nx_MediaContext_t *pstContents);
static ONDK_Result_t	nx_media_UpdateThumbnail(Nx_MediaContext_t *pstContents);
static ONDK_Result_t	nx_media_HideThumbnail(Nx_MediaContext_t *pstContents);
static ONDK_Result_t	nx_media_ShowCurrentPath(Nx_MediaContext_t *pstContents);
static ONDK_Result_t	nx_media_ShowOperatingDlg(Nx_MediaContext_t *pstContents, HBOOL bShow);
static ONDK_Result_t	nx_media_ShowNoItemInfo(Nx_MediaContext_t *pstContents, HBOOL bShow, Nx_MediaServerType_e eServerType);
static ONDK_Result_t	nx_media_ShowOptList(Nx_MediaContext_t *pstContents, HBOOL bShow, HBOOL bReCreate);
static ONDK_Result_t	nx_media_MakeSelectModeDim(HBOOL bShow);
static ONDK_Result_t	nx_media_OptListInitData(Nx_MediaContext_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_media_DrawBottomDim(Nx_MediaContext_t *pstContents);
static ONDK_Result_t	nx_media_DrawBottomDimFunc(ONDKSurfaceHandle screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t	nx_media_DrawOptListPopup(Nx_MediaContext_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_media_DrawContentList(Nx_MediaContext_t *pstContents);
static ONDK_Result_t	nx_media_DrawVideoContentListFunc(ONDKSurfaceHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t	nx_media_DrawPhotoContentListFunc(HINT32 objId, ONDKSurfaceHandle screen, GRID_DrawItem_t *drawItem, HINT32 nColIndex);
static ONDK_Result_t	nx_media_DrawServerListFunc(ONDKSurfaceHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t	nx_media_DrawDefaultUI(Nx_MediaContext_t *pstContents);

static ONDK_Result_t	nx_media_ServerListSelect(Nx_MediaContext_t *pstContents, HINT32 lIndex);
static ONDK_Result_t	nx_media_ContentListSelect(Nx_MediaContext_t *pstContents, HINT32 lIndex);
static ONDK_Result_t	nx_media_SetCurrentContentSelected(Nx_MediaContext_t *pstContents);
static ONDK_Result_t	nx_media_RemoveContentGroup(Nx_MediaContext_t *pstContents);
static ONDK_Result_t	nx_media_MoveContentGroup(Nx_MediaContext_t	*pstContents, HCHAR* pszTargetPath);
static ONDK_Result_t	nx_media_CopyContentGroup(Nx_MediaContext_t	*pstContents, HCHAR* pszTargetPath);
static ONDK_Result_t	nx_media_RemoveCurrentFocusedServer(Nx_MediaContext_t *pstContents);
static ONDK_Result_t	nx_media_RemoveFocusedServerAllContent(Nx_MediaContext_t *pstContents);

static ONDK_Result_t	nx_media_MsgGwmCreate(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_media_MsgGwmDestroy(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_media_MsgGwmHapiMsg(Nx_MediaContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_media_MsgGwmTimer(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_media_MsgGwmKeyDown(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_media_MsgGwmListSelected(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_media_MsgGwmListChanged(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_media_MsgGwmObjFocusChanged(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_media_MsgCoreMediaBrowseDone(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_media_MsgCoreDeviceChange(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_media_MsgCoreOperationDone(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 lMessage);
static	ONDK_Result_t	nx_media_MsgCoreDlnaDmsChange(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 lMessage);
static ONDK_Result_t	nx_media_MsgAppPlayerStop(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_media_MsgAppDlgClicked(Nx_MediaContext_t *pstContents, HINT32 lClickedBtn);
static ONDK_Result_t	nx_media_MsgAppTargetStorageSelected(Nx_MediaContext_t *pstContents, HINT32 lSelectedDevId);
static ONDK_Result_t	nx_media_MsgAppOptListSelected(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_media_MsgAppKeyboardResult(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_MediaContext_t	*nx_media_GetContents(void)
{
	return	&s_stMediaContent;
}

static ONDK_Result_t	nx_media_SelectiveMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_DIALOGUE_Message_Proc(&s_stDialogSelectiveInstance, message, hAction, p1, p2, p3);
}

static void		nx_media_Ui_SelectiveMsgCreate(Nx_MediaDialogType_e eDialogType)
{
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;
	HUINT8			*szMsgStr = NULL;

	pstDialog = &s_stDialogSelectiveInstance;
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	switch (eDialogType)
	{
		case eNxMedia_Dialog_Select_WatchFromBegin:
			szMsgStr = (HCHAR*)ONDK_GetString(RES_MESG_4563_ID);
			ulDlgAttr = (NX_DLG_YES_NO|NX_DLG_TIMEOUT_30SEC|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
			break;

		case eNxMedia_Dialog_Select_DeleteOneFile:
			szMsgStr = (HCHAR*)ONDK_GetString(RES_MESG_3599_ID);
			ulDlgAttr = (NX_DLG_YES_NO|NX_DLG_TIMEOUT_30SEC|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
			break;

		case eNxMedia_Dialog_Select_DeleteMultiFile:
			szMsgStr = (HCHAR*)ONDK_GetString(RES_MESG_3696_ID);
			ulDlgAttr = (NX_DLG_YES_NO|NX_DLG_TIMEOUT_30SEC|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
			break;

		case eNxMedia_Dialog_Select_DeleteAllFiles:
			szMsgStr = (HCHAR*)ONDK_GetString(RES_MESG_3909_ID);
			ulDlgAttr = (NX_DLG_YES_NO|NX_DLG_TIMEOUT_30SEC|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
			break;

		case eNxMedia_Dialog_Confirm_CannotCopyFormatInvalid:
			szMsgStr = (HCHAR*)ONDK_GetString(RES_MESG_3448_ID);
			ulDlgAttr = (NX_DLG_OK|NX_DLG_TIMEOUT_30SEC|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
			break;

		case eNxMedia_Dialog_Confirm_CannotCopyLackSpace:
			szMsgStr = (HCHAR*)ONDK_GetString(RES_MESG_900_ID);
			ulDlgAttr = (NX_DLG_OK|NX_DLG_TIMEOUT_30SEC|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
			break;

		case eNxMedia_Dialog_Confirm_CannotCopyFileSize:
			szMsgStr = (HCHAR*)ONDK_GetString(RES_MESG_1650_ID);
			ulDlgAttr = (NX_DLG_OK|NX_DLG_TIMEOUT_30SEC|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
			break;

		case eNxMedia_Dialog_Confirm_DamageOccur:
			szMsgStr = (HCHAR*)ONDK_GetString(RES_MESG_818_ID);
			ulDlgAttr = (NX_DLG_OK|NX_DLG_TIMEOUT_30SEC|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
			break;

		default:
			NX_APP_Error("Error!!! Unknow Dialogue Type:[%d]\n", eDialogType);
			return;
	}

	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
				ulDlgAttr, NULL, szMsgStr);

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "nx_media_SelectiveMsgProc",
						APP_DIALOGBOX_PRIORITY,	nx_media_SelectiveMsgProc,
						(int)NULL, 0, 0, (int)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis ()));
}

static ONDK_Result_t	nx_media_SelectiveMsgDestroy(void)
{
	return	ONDK_GWM_APP_Destroy(nx_media_SelectiveMsgProc);
}

static ONDK_Result_t	nx_media_ProcessingMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_DIALOGUE_ProcessingMessage_Proc(&s_stDialogProgressInstance, message, hAction, p1, p2, p3);
}

static void		nx_media_Ui_ProcessingMsgCreate(Nx_MediaDialogType_e eDialogType)
{
	HINT8			*pszMsg = NULL;
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = &s_stDialogProgressInstance;
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	switch (eDialogType)
	{
		case eNxMedia_Dialog_Progress_Connecting:
			pszMsg = (HINT8*)ONDK_GetString(RES_CONNECTING_ID);
			break;
		default:
			NX_APP_Error("Error!!! Unknow Dialogue Type:[%d]\n", eDialogType);
			return;
	}
	ulDlgAttr = (NX_DLG_TYPE_NO_STANDBY|NX_DLG_TYPE_NO_EXIT|NX_DLG_TIMEOUT_NOTIMEOUT|NX_DLG_NO_BUTTON|NX_DLG_TYPE_PM_ANIMAION);

	pstDialog->ulDlgWhoseId = eDialogType;
	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
								ulDlgAttr, (HUINT8 *)NULL, (HUINT8 *)pszMsg);

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_media_ProcessingMsgProc", APP_DIALOGBOX_PRIORITY,
							nx_media_ProcessingMsgProc, 0, 0, 0, 0);
}

static void		nx_media_ProcessingMsgDestroy(void)
{
	ONDK_GWM_APP_Destroy(nx_media_ProcessingMsgProc);
}

static ONDK_Result_t	nx_media_ResponseMsgProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
{
	switch (lMessage)
	{
		case MSG_GWM_KEYDOWN:
			NX_PRISM_UTIL_KeyConv_UiCommon (&lParam1);
			switch (lParam1)
			{
				case KEY_RED:
				case KEY_GREEN:
				case KEY_YELLOW:
				case KEY_BLUE:
				case KEY_ARROWUP :
				case KEY_ARROWDOWN:
				case KEY_TVRADIO:
				case KEY_EXT_AV:
				case KEY_COMBO:
				case KEY_TEXT:
				case KEY_TTX_HOLD:
				case KEY_TTX_SUBPAGE:
				case KEY_AUDIO:
				case KEY_SUBTITLE:
				case KEY_WIDE:
				case KEY_3D:
				case KEY_INFO:
				case KEY_BACK:
				case KEY_EXT_AV_PLUS:
				case KEY_EXT_AV_MINUS:
					ONDK_GWM_APP_Destroy(0);
					return	ONDK_MESSAGE_BREAK;
				default:
					break;
			}
			break;

		case MSG_APP_SYSTEM_SVC_CHANGED :
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		default :
			break;
	}

	return	NX_PRISM_DIALOGUE_ResponseMessage_Proc(lMessage, hHandle, lParam1, lParam2, lParam3);
}

static void		nx_media_Ui_ResponseMsgCreate(HUINT8* pszString)
{
	if (pszString == NULL)
	{
		NX_APP_Error("Error!!! Pls check pszString was NULL!! \n");
		return;
	}
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_media_ResponseMsgProc", APP_DEFAULT_PRIORITY,
						nx_media_ResponseMsgProc, 0, (HINT32)pszString, 0, 0);
}

static void		nx_media_ResponseMsgDestroy(void)
{
	ONDK_GWM_APP_Destroy(nx_media_ResponseMsgProc);
}

static ONDK_Result_t	nx_media_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stMediaOptContents, message, hAction, p1, p2, p3);
}

static ONDK_Result_t	nx_media_DestroyListPopup(void)
{
	return ONDK_GWM_APP_Destroy(nx_media_DisplayListPopup);
}

static ONDK_Result_t	nx_media_FreeDeviceIdList (HUINT32 *pDevIdList)
{
	if(pDevIdList != NULL)
	{
		NX_APP_Free (pDevIdList);
		pDevIdList = NULL;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_FreeSelectedList (HBOOL *pSelectedList)
{
	if(pSelectedList != NULL)
	{
		NX_APP_Free (pSelectedList);
		pSelectedList = NULL;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_FreeOptionTextList (Nx_MediaContext_t *pstContents)
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

#define ___________GET_DATA___________

static ONDK_Result_t	nx_media_LoadStorageDevIdList(Nx_MediaContext_t *pstContents)
{
	HERROR 		hError = ERR_FAIL;

	//Get Storage DevId List
	nx_media_FreeDeviceIdList(pstContents->pStorageDevIdList);
	hError = NX_FILEMGR_STORAGE_GetPhysicalStorageDevIdList(&pstContents->pStorageDevIdList, &pstContents->ulStorageListNum);
	if (hError != ERR_OK || pstContents->pStorageDevIdList == NULL)
	{
		NX_APP_Error("physicalStorageDevIdList is NULL!\n");
		return ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_DlnaDmsStartScan(void)
{
	HERROR 		hError = ERR_FAIL;

#if defined(CONFIG_MW_MM_DLNA)
	hError = NX_DLNA_DMP_Start();
#endif
	if (hError != ERR_OK)
	{
		NX_APP_Error("NX_DLNA_DMP_Start Fail !!\n");
		return ONDK_RET_FAIL;
	}
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_DlnaDmsStopScan(Nx_MediaContext_t *pstContents)
{
	HERROR		hError = ERR_FAIL;

#if defined(CONFIG_MW_MM_DLNA)
	hError = NX_DLNA_DMP_Stop();

	if (pstContents->pDlnaContentList)
	{
		NX_DLNA_RemoveAllBrowsedItem();
		pstContents->pDlnaContentList = NULL;
	}
#endif
	if (hError != ERR_OK)
	{
		NX_APP_Error("NX_DLNA_DMP_Start Fail !!\n");
		return ONDK_RET_FAIL;
	}
	return ONDK_RET_OK;
}

static void	*	nx_media_CloneContentObject(const void *pvSelf)
{
	DxFileInfo_t    *pstClone;

	pstClone = (DxFileInfo_t *)HLIB_STD_MemCalloc(sizeof(DxFileInfo_t));
	if (pstClone)
	{
		HxSTD_memcpy(pstClone, pvSelf, sizeof(DxFileInfo_t));
	}
	return pstClone;
}

static ONDK_Result_t	nx_media_MakePlayList(Nx_MediaContext_t *pstContents, HINT32 lCurIndex)
{
	HINT32				lItemCount = 0, lIndex = 0;
	DxFileInfo_t 		*pstFsInfo = NULL;

	if (pstContents->browseHandle == NULL)
	{
		NX_APP_Error("pstContents->browseHandle is NULL!\n");
		return ONDK_RET_FAIL;
	}

	// Init playlist
	if (pstContents->playList)
	{
		HLIB_VECTOR_Delete(pstContents->playList);
		pstContents->playList = NULL;
	}

	pstContents->playList = HLIB_VECTOR_CloneEx(pstContents->browseHandle->itemList, (void *(*)(void *))nx_media_CloneContentObject);
	lItemCount = HLIB_VECTOR_Length(pstContents->playList);

	// reordering item from current Index
	for(lIndex = lCurIndex; lIndex < lItemCount; lIndex++)
	{
		pstFsInfo = HLIB_VECTOR_Detach(pstContents->playList, lIndex);
		HLIB_VECTOR_Insert(pstContents->playList, lIndex - lCurIndex, pstFsInfo);
	}

	// remove directory item
	lItemCount = HLIB_VECTOR_Length(pstContents->playList);

	for(lIndex = 0; lIndex < lItemCount; lIndex++)
	{
		pstFsInfo = HLIB_VECTOR_ItemAt(pstContents->playList, lIndex);
		if (pstFsInfo->eDataType == eDxFILETYPE_DIRECTORY)
		{
			HLIB_VECTOR_Remove(pstContents->playList, lIndex);
			lItemCount = HLIB_VECTOR_Length(pstContents->playList);
			lIndex = 0;
		}
	}

	return	ONDK_RET_OK;
}

static HBOOL	nx_media_CheckStorageEjectableWithIdx(Nx_MediaContext_t *pstContents, HINT32 lIndex)
{
	HERROR			hError;
	HINT32			lStorageCount = pstContents->ulStorageListNum;
	NX_PhysicalStorageData_t	*physicalStorageData = NULL;

	if (lIndex == 0) //recent
	{
		return FALSE;
	}
	else if ((lIndex > 0) && (lIndex < lStorageCount + NX_MEDIA_SERVER_RECENT_COUNT)) //storage
	{
		hError = NX_FILEMGR_STORAGE_GetPhysicalStorageInfoWithDevId(pstContents->pStorageDevIdList[lIndex - NX_MEDIA_SERVER_RECENT_COUNT], &physicalStorageData);
		if (hError != ERR_OK || physicalStorageData == NULL)
		{
			NX_APP_Error("Error!!! GetPhysicalStorageInfoWithDevId FAIL !!\n");
			return FALSE;
		}

		if (physicalStorageData->eSort == eStorageSort_externalHDD || physicalStorageData->eSort == eStorageSort_UsbMemory || physicalStorageData->eSort == eStorageSort_SdMemory)
		{
			return TRUE;
		}
	}
	else //dlna
	{
		return FALSE;
	}

	return FALSE;
}

static ONDK_Result_t	nx_media_GetDurationStr(HCHAR* pszDuration, HUINT32	ulDuration)
{
	HINT32	lHour = 0, lMin = 0, lSec = 0;

	lHour = ulDuration / (60 * 60);
	ulDuration = ulDuration - (lHour * (60 * 60));

	lMin = ulDuration / 60;
	ulDuration = ulDuration - (lMin * 60);

	lSec = ulDuration;

	HxSTD_sprintf((char*)pszDuration, "%02d:%02d:%02d", lHour, lMin, lSec);

	return ONDK_RET_OK;
}

static HINT32	nx_media_GetThumbImgObjId(HINT32 lIndex)
{
	switch (lIndex)
	{
		case 0:
			return NX_PRISM_UI_MEDIA_THUMB_ITEM1_ID;

		case 1:
			return NX_PRISM_UI_MEDIA_THUMB_ITEM2_ID;

		case 2:
			return NX_PRISM_UI_MEDIA_THUMB_ITEM3_ID;

		case 3:
			return NX_PRISM_UI_MEDIA_THUMB_ITEM4_ID;

		case 4:
			return NX_PRISM_UI_MEDIA_THUMB_ITEM5_ID;

		case 5:
			return NX_PRISM_UI_MEDIA_THUMB_ITEM6_ID;

		case 6:
			return NX_PRISM_UI_MEDIA_THUMB_ITEM7_ID;

		default:
			return 0;
	}
}

static HCHAR* 	nx_media_GetStorageLabelFromObjInfo(NX_PhysicalStorageData_t *physicalStorageData)
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

static HCHAR* 	nx_media_GetStorageIconIdFromObjInfo(NX_PhysicalStorageData_t *physicalStorageData, HBOOL bFocus)
{
	if (physicalStorageData == NULL)
	{
		NX_APP_Error("ERROR!!! physicalStorageData is NULL !!\n");
		return NULL;
	}

	switch(physicalStorageData->eSort)
	{
		case eStorageSort_InternalPVRHDD:
			if (bFocus == TRUE)
			{
				return RES_214_00_STORAGE_SET_IN_C_PNG;
			}
			else
			{
				return RES_214_00_STORAGE_SET_IN_N_PNG;
			}

		case eStorageSort_externalSubPVRHDD:
			if (bFocus == TRUE)
			{
				return RES_214_00_STORAGE_SET_ESATA_C_PNG;
			}
			else
			{
				return RES_214_00_STORAGE_SET_ESATA_N_PNG;
			}

		case eStorageSort_externalPVRHDD:
		case eStorageSort_externalHDD:
		case eStorageSort_UsbMemory:
			if (bFocus == TRUE)
			{
				return RES_214_00_STORAGE_SET_USB_C_PNG;
			}
			else
			{
				return RES_214_00_STORAGE_SET_USB_N_PNG;
			}

		case eStorageSort_SdMemory:
			if (bFocus == TRUE)
			{
				return RES_214_00_STORAGE_SET_SD_C_PNG;
			}
			else
			{
				return RES_214_00_STORAGE_SET_SD_N_PNG;
			}

		default:
			return NULL;
	}
}

static ONDK_Result_t 	nx_media_GetContentData(Nx_MediaContext_t *pstContents, Nx_MediaContentData_t *pstContentData, Nx_MediaServerType_e	eServerType, HINT32	lContentIdx)
{
	DxFileInfo_t 		*pstFsInfo = NULL;
	NxFileInfo_t 		*pstNxFsInfo = NULL;
#if defined(CONFIG_MW_MM_DLNA)
	NX_DLNA_CONTENT_t	*pstContentInfo = NULL;
#endif
	HCHAR	*thumbnail = NULL;

	if (eServerType == eNxMedia_ServerType_Storage)
	{
		pstFsInfo = HLIB_VECTOR_ItemAt(pstContents->browseHandle->itemList, lContentIdx);
		if (pstFsInfo == NULL)
		{
			NX_APP_Error("ERROR!!! Get Item Info Fail !!\n");
			return ONDK_RET_FAIL;
		}

		pstNxFsInfo = NX_FILEMGR_FILE_MakeNxFileInfo(pstFsInfo, NXFILE_FILTER_MOVIE);
		if (pstNxFsInfo == NULL)
		{
			NX_APP_Error("ERROR!!! Get Item Info Fail !!\n");
			return ONDK_RET_FAIL;
		}

		HxSTD_sprintf((char*)pstContentData->szTitle, "%s", (char*)pstNxFsInfo->szFileName);
		pstContentData->bDirectory = (pstFsInfo->eDataType == eDxFILETYPE_DIRECTORY) ? TRUE : FALSE;
		pstContentData->bHasSubFile = (pstNxFsInfo->files.dir.bHasSubFile == TRUE) ? TRUE : FALSE;
		pstContentData->ulDuration = pstNxFsInfo->files.av.nDuration;
		HxSTD_sprintf((char*)pstContentData->szDate, "%s", (char*)pstNxFsInfo->szDate);
		if (pstContentData->bDirectory != TRUE)
		{
			thumbnail = NX_FILEMGR_FILE_GetThumbnail(pstNxFsInfo->szFullPath, 5, NULL, TRUE);
			HxSTD_sprintf((char*)pstContentData->szThumbnail, "%s", (char*)thumbnail);
		}

		if (thumbnail)
		{
			ONDK_Free(thumbnail);
			thumbnail = NULL;
		}
		if (pstNxFsInfo)
		{
			ONDK_Free(pstNxFsInfo);
			pstNxFsInfo = NULL;
		}
	}
	else if (eServerType == eNxMedia_ServerType_Dlna)
	{
#if defined(CONFIG_MW_MM_DLNA)
		pstContentInfo = HLIB_VECTOR_ItemAt(pstContents->pDlnaContentList, lContentIdx);
		HxSTD_sprintf((char*)pstContentData->szTitle, "%s", (char*)pstContentInfo->szTitle);
		pstContentData->bDirectory = TRUE;
		pstContentData->bHasSubFile = TRUE;
#endif
	}

	return ONDK_RET_OK;
}

#define ___________DRAW_FUNCTION___________

static void		nx_media_DrawBackground(void)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	/* background start */
	stRect = ONDK_Rect(NX_PRISM_UI_BACKGROUND_BOX_X, NX_PRISM_UI_BACKGROUND_BOX_Y, NX_PRISM_UI_BACKGROUND_BOX_WIDTH, NX_PRISM_UI_BACKGROUND_BOX_HEIGHT);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_BACKGROUND_BOX_ID, &stRect, COL(C_G_Bang_80));
}

static void		nx_media_UpdateCurrentTime(Nx_MediaContext_t *pstContents)
{
	HERROR			hError = ERR_FAIL;
	HCHAR 			*pszDayOfWeek;
	HUINT32 		ulUnixtime = 0, ulWeekIndex = 0;
	HxDATETIME_t 	stDatetime;

	hError = NX_SYSTEM_GetTime(&ulUnixtime);
	if (hError == ERR_OK)
	{
		HLIB_DATETIME_ConvertUnixTimeToDateTime(ulUnixtime, &stDatetime);
		ulWeekIndex = HLIB_DATETIME_GetWeekDayIndex(stDatetime.stDate);
		pszDayOfWeek = (HCHAR*)ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(ulWeekIndex));

		HxSTD_sprintf((char*)pstContents->szDate, "%02d.%02d.%04d", stDatetime.stDate.ucDay, stDatetime.stDate.ucMonth,
										stDatetime.stDate.usYear);

		HxSTD_sprintf((char*)pstContents->szTime, "%s %02d:%02d", pszDayOfWeek, stDatetime.stTime.ucHour,
												stDatetime.stTime.ucMinute);
	}
	else
	{
		pstContents->szDate[0] = '\0';
		pstContents->szTime[0] = '\0';
	}
}

static ONDK_Result_t	nx_media_UpdateThumbnail(Nx_MediaContext_t *pstContents)
{
	HINT32		lIndex = 0;

	for (lIndex = 0; lIndex < pstContents->lShowItemCnt; lIndex++)
	{
		if (HxSTD_StrLen(pstContents->thumbRequest[lIndex].szPath) > 0)
		{
			if (HLIB_FILE_Exist((HUINT8*)pstContents->thumbRequest[lIndex].szPath) == TRUE &&
					pstContents->thumbRequest[lIndex].bGetThumb == FALSE)
			{
				pstContents->thumbRequest[lIndex].bGetThumb = TRUE;
				ONDK_GWM_APP_InvalidateObject(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID);
				break;
			}
		}
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_HideThumbnail(Nx_MediaContext_t *pstContents)
{
	HINT32					lIndex = 0, lObjId = 0;
	ONDK_GWM_GuiObject_t	thumbObj = NULL;

	for (lIndex = 0; lIndex < pstContents->lShowItemCnt; lIndex++)
	{
		lObjId = nx_media_GetThumbImgObjId(lIndex);
		thumbObj = ONDK_GWM_Obj_GetObject(lObjId);
		if (thumbObj)
		{
			ONDK_GWM_Obj_SetHide(lObjId, TRUE);
		}
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_ShowCurrentPath(Nx_MediaContext_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };
	HCHAR			*szCurPath = NULL;
	HINT32			lIdx = 0, lServerPathLen = 0, lCurPathLen = 0;

	if (pstContents->eCurrentServerType == eNxMedia_ServerType_Storage)
	{
		if (pstContents->browseHandle == NULL)
		{
			NX_APP_Info("Error!!! pstContents->browseHandle is NULL !!\n");
			return ONDK_RET_FAIL;
		}

		lCurPathLen = HxSTD_StrLen(pstContents->browseHandle->szCurPath);
		lServerPathLen = HxSTD_StrLen(pstContents->ucCurrentServerPath);

		if (lCurPathLen <= lServerPathLen)
		{
			pstContents->szCurPath[0] = '\0';
		}
		else
		{
			szCurPath = pstContents->browseHandle->szCurPath;
			for (lIdx = 0; lIdx < lServerPathLen; lIdx++)
			{
				szCurPath++;
			}
			HxSTD_sprintf((char*)pstContents->szCurPath, "%s", (char*)szCurPath);
		}
	}
	else if (pstContents->eCurrentServerType == eNxMedia_ServerType_Dlna)
	{
		HxSTD_sprintf((char*)pstContents->szCurPath, "%s", (char*)"");
	}

	stRect = ONDK_Rect(NX_MEDIA_PATH_X, NX_MEDIA_PATH_Y, NX_MEDIA_PATH_W, NX_MEDIA_PATH_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_MEDIA_PATH_ID, &stRect, pstContents->szCurPath);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_MEDIA_PATH_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_MEDIA_PATH_ID, NX_PRISM_FONT_SIZE_28);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_MEDIA_PATH_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_MEDIA_PATH_ID, COL(C_T_Gong01), COL(C_T_Gong01));

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_media_ShowOperatingDlg(Nx_MediaContext_t *pstContents, HBOOL bShow)
{
	ONDK_Rect_t				stRect;
	ONDK_GWM_GuiObject_t 	bgObj, imgObj, txtObj;
	HCHAR 					*pszText = NULL;
	NxFileOperation_e		operStatus = eFileOperation_Idle;

	bgObj = ONDK_GWM_APP_GetObject(NX_PRISM_UI_MEDIA_OPERATING_BG_ID);
	imgObj = ONDK_GWM_APP_GetObject(NX_PRISM_UI_MEDIA_OPERATING_IMG_ID);
	txtObj = ONDK_GWM_APP_GetObject(NX_PRISM_UI_MEDIA_OPERATING_TXT_ID);

	if (bShow == TRUE)
	{
		operStatus = NX_FILEMGR_FILE_GetFileOperationStatus();
		switch(operStatus)
		{
			case eFileOperation_Deleting:
				pstContents->ulCurrentDlg = eNxMedia_Dialog_Progress_Deleting;
				pszText = (HCHAR*)ONDK_GetString(RES_DELETING_01_ID);
				break;
			case eFileOperation_Copying:
				pstContents->ulCurrentDlg = eNxMedia_Dialog_Progress_Copying;
				pszText = (HCHAR*)ONDK_GetString(RES_COPYING_ID);
				break;
			case eFileOperation_Moving:
				pstContents->ulCurrentDlg = eNxMedia_Dialog_Progress_Moving;
				pszText = (HCHAR*)ONDK_GetString(RES_MOVING_ID);
				break;
			case eFileOperation_Stopping:
				if (pstContents->ulCurrentDlg == eNxMedia_Dialog_Progress_Copying)
				{
					pszText = (HCHAR*)ONDK_GetString(RES_COPYING_ID);
				}
				else if (pstContents->ulCurrentDlg == eNxMedia_Dialog_Progress_Moving)
				{
					pszText = (HCHAR*)ONDK_GetString(RES_MOVING_ID);
				}
				else
				{
					NX_APP_Info("file operation status is invalid!\n");
					return ONDK_RET_FAIL;
				}
				break;
			default:
				NX_APP_Info("file operation status is idle!\n");
				return ONDK_RET_OK;
		}
	}
	else
	{
		pstContents->ulCurrentDlg = eNxMedia_Dialog_None;
	}

	if (bgObj && imgObj && txtObj)
	{
		if (bShow == TRUE)
		{
			ONDK_GWM_Obj_SetHideByObj(bgObj, FALSE);
			ONDK_GWM_Obj_SetHideByObj(imgObj, FALSE);
			ONDK_GWM_COM_Text_SetText(NX_PRISM_UI_MEDIA_OPERATING_TXT_ID, pszText);
			ONDK_GWM_Obj_SetHideByObj(txtObj, FALSE);
		}
		else
		{
			ONDK_GWM_Obj_SetHideByObj(bgObj, TRUE);
			ONDK_GWM_Obj_SetHideByObj(imgObj, TRUE);
			ONDK_GWM_Obj_SetHideByObj(txtObj, TRUE);
		}
	}
	else
	{
		if (bShow == TRUE)
		{
			// Draw - Bg
			stRect = ONDK_Rect(NX_PRISM_MEDIA_OPERATING_BG_X, NX_PRISM_MEDIA_OPERATING_BG_Y, NX_PRISM_MEDIA_OPERATING_BG_W, NX_PRISM_MEDIA_OPERATING_BG_H);
			ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_MEDIA_OPERATING_BG_ID, &stRect, COL(C_T_Bang_100));

			// Draw - Img
			stRect = ONDK_Rect(NX_PRISM_MEDIA_OPERATING_IMG_X, NX_PRISM_MEDIA_OPERATING_IMG_Y, NX_PRISM_MEDIA_OPERATING_IMG_W, NX_PRISM_MEDIA_OPERATING_IMG_H);
			ONDK_GWM_COM_Image_Create(NX_PRISM_UI_MEDIA_OPERATING_IMG_ID, &stRect);
			ONDK_GWM_COM_Image_SetImage(NX_PRISM_UI_MEDIA_OPERATING_IMG_ID, (HCHAR*)RES_206_00_LOADING_01_PNG);

			// Draw - Txt
			stRect = ONDK_Rect(NX_PRISM_MEDIA_OPERATING_TXT_X, NX_PRISM_MEDIA_OPERATING_TXT_Y, NX_PRISM_MEDIA_OPERATING_TXT_W, NX_PRISM_MEDIA_OPERATING_TXT_H);
			ONDK_GWM_COM_Text_Create(NX_PRISM_UI_MEDIA_OPERATING_TXT_ID, &stRect, pszText);
			ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_MEDIA_OPERATING_TXT_ID, eFont_SystemNormal);
			ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_MEDIA_OPERATING_TXT_ID, NX_PRISM_FONT_SIZE_22);
			ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_MEDIA_OPERATING_TXT_ID, (TEXT_ALIGN_LEFT));
			ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_MEDIA_OPERATING_TXT_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));
		}
	}

	if (bShow == TRUE)
	{
		pstContents->ulAniIconIdx = 0;
		ONDK_GWM_SetTimer(NX_PRISM_UI_DLG_PROCESS_ANIMATION_TIMER_ID, (HUINT32)NX_PRISM_UI_DLG_PROCESS_ANIMATION_TIMER_PERIOD);
	}
	else
	{
		ONDK_GWM_KillTimer(NX_PRISM_UI_DLG_PROCESS_ANIMATION_TIMER_ID);
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_ShowNoItemInfo(Nx_MediaContext_t *pstContents, HBOOL bShow, Nx_MediaServerType_e eServerType)
{
	ONDK_Rect_t				stRect;
	ONDK_GWM_GuiObject_t 	imgObj, mainTxtObj, subTxtObj;
	HCHAR					*pszMainStr = NULL, *pszSubStr = NULL;

	imgObj = ONDK_GWM_APP_GetObject(NX_PRISM_UI_MEDIA_CONTENT_NOITEM_IMG_ID);
	mainTxtObj = ONDK_GWM_APP_GetObject(NX_PRISM_UI_MEDIA_CONTENT_NOITEM_MAINTXT_ID);
	subTxtObj = ONDK_GWM_APP_GetObject(NX_PRISM_UI_MEDIA_CONTENT_NOITEM_SUBTXT_ID);

	//Get String
	if (eServerType == eNxMedia_ServerType_Recent)
	{
		pszMainStr = (HCHAR*)ONDK_GetString(RES_MESG_4365_ID);
		pszSubStr = (HCHAR*)ONDK_GetString(RES_MESG_4366_ID);
	}
	else if (eServerType == eNxMedia_ServerType_Storage)
	{
		pszMainStr = (HCHAR*)ONDK_GetString(RES_MESG_3338_ID);
		pszSubStr = (HCHAR*)ONDK_GetString(RES_MESG_3441_ID);
	}

	// Show & Hide
	if (imgObj && mainTxtObj && subTxtObj)
	{
		if (bShow == TRUE)
		{
			ONDK_GWM_COM_Text_SetText(NX_PRISM_UI_MEDIA_CONTENT_NOITEM_MAINTXT_ID, pszMainStr);
			ONDK_GWM_COM_Text_SetText(NX_PRISM_UI_MEDIA_CONTENT_NOITEM_SUBTXT_ID, pszSubStr);
			ONDK_GWM_Obj_SetHideByObj(imgObj, FALSE);
			ONDK_GWM_Obj_SetHideByObj(mainTxtObj, FALSE);
			ONDK_GWM_Obj_SetHideByObj(subTxtObj, FALSE);
		}
		else
		{
			ONDK_GWM_Obj_SetHideByObj(imgObj, TRUE);
			ONDK_GWM_Obj_SetHideByObj(mainTxtObj, TRUE);
			ONDK_GWM_Obj_SetHideByObj(subTxtObj, TRUE);
		}
	}
	else
	{
		if (bShow == TRUE)
		{
			// Draw - No Item Img
			stRect = ONDK_Rect(NX_MEDIA_CONTENTS_NOITEM_IMG_X, NX_MEDIA_CONTENTS_NOITEM_IMG_Y, NX_MEDIA_CONTENTS_NOITEM_IMG_W, NX_MEDIA_CONTENTS_NOITEM_IMG_H);
			ONDK_GWM_COM_Image_Create(NX_PRISM_UI_MEDIA_CONTENT_NOITEM_IMG_ID, &stRect);
			ONDK_GWM_COM_Image_SetImage(NX_PRISM_UI_MEDIA_CONTENT_NOITEM_IMG_ID, (HCHAR*)RES_420_05_ME_V_NO_PNG);

			// Draw - No Item Main Txt
			stRect = ONDK_Rect(NX_MEDIA_CONTENTS_NOITEM_MAINTXT_X, NX_MEDIA_CONTENTS_NOITEM_MAINTXT_Y, NX_MEDIA_CONTENTS_NOITEM_MAINTXT_W, NX_MEDIA_CONTENTS_NOITEM_MAINTXT_H);
			ONDK_GWM_COM_Text_Create(NX_PRISM_UI_MEDIA_CONTENT_NOITEM_MAINTXT_ID, &stRect, pszMainStr);
			ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_MEDIA_CONTENT_NOITEM_MAINTXT_ID, eFont_SystemBold);
			ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_MEDIA_CONTENT_NOITEM_MAINTXT_ID, NX_PRISM_FONT_SIZE_28);
			ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_MEDIA_CONTENT_NOITEM_MAINTXT_ID, TEXT_ALIGN_CENTER);
			ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_MEDIA_CONTENT_NOITEM_MAINTXT_ID, COL(C_T_Gong03), COL(C_T_Bang_100));

			// Draw - No Item Sub Txt
			stRect = ONDK_Rect(NX_MEDIA_CONTENTS_NOITEM_SUBTXT_X, NX_MEDIA_CONTENTS_NOITEM_SUBTXT_Y, NX_MEDIA_CONTENTS_NOITEM_SUBTXT_W, NX_MEDIA_CONTENTS_NOITEM_SUBTXT_H);
			ONDK_GWM_COM_Text_Create(NX_PRISM_UI_MEDIA_CONTENT_NOITEM_SUBTXT_ID, &stRect, pszSubStr);
			ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_MEDIA_CONTENT_NOITEM_SUBTXT_ID, eFont_SystemNormal);
			ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_MEDIA_CONTENT_NOITEM_SUBTXT_ID, NX_PRISM_FONT_SIZE_24);
			ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_MEDIA_CONTENT_NOITEM_SUBTXT_ID, (TEXT_ALIGN_CENTER | TEXT_MULTILINED));
			ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_MEDIA_CONTENT_NOITEM_SUBTXT_ID, COL(C_T_Gong05), COL(C_T_Bang_100));
		}
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_ShowOptList(Nx_MediaContext_t *pstContents, HBOOL bShow, HBOOL bReCreate)
{
	ONDK_Rect_t				stRect;
	ONDK_GWM_GuiObject_t 	optObj;

	optObj = ONDK_GWM_APP_GetObject(NX_PRISM_UI_MEDIA_OPTION_ID);

	if (optObj)
	{
		if (bReCreate == TRUE)
		{
			ONDK_GWM_Obj_Destroy(NX_PRISM_UI_MEDIA_OPTION_ID);
			stRect = ONDK_Rect(NX_MEDIA_OPT_X, NX_MEDIA_OPT_Y, NX_MEDIA_OPT_W, NX_MEDIA_OPT_H);
			ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_MEDIA_OPTION_ID, &stRect, 0);
			ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_MEDIA_OPTION_ID, NX_PRISM_COMP_DrawEdit_HelpR);
		}
		if (bShow == TRUE)
		{
			ONDK_GWM_Obj_SetHide(NX_PRISM_UI_MEDIA_OPTION_ID, FALSE);
		}
		else
		{
			ONDK_GWM_Obj_SetHide(NX_PRISM_UI_MEDIA_OPTION_ID, TRUE);
		}
	}
	else
	{
		if (bShow == TRUE)
		{
			stRect = ONDK_Rect(NX_MEDIA_OPT_X, NX_MEDIA_OPT_Y, NX_MEDIA_OPT_W, NX_MEDIA_OPT_H);
			ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_MEDIA_OPTION_ID, &stRect, 0);
			ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_MEDIA_OPTION_ID, NX_PRISM_COMP_DrawEdit_HelpR);
		}
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_MakeSelectModeDim(HBOOL bShow)
{
	ONDK_Rect_t				stRect = { 0, 0, 0, 0 };
	ONDK_GWM_GuiObject_t 	dimObj, helpStrObj;

	dimObj = ONDK_GWM_APP_GetObject(NX_PRISM_UI_MEDIA_SELECTMODE_DIM_ID);
	helpStrObj = ONDK_GWM_APP_GetObject(NX_PRISM_UI_MEDIA_SELECTMODE_HELPSTR_ID);

	if (dimObj == NULL)
	{
		stRect = ONDK_Rect(NX_PRISM_UI_BACKGROUND_BOX_X, NX_PRISM_UI_BACKGROUND_BOX_Y, NX_PRISM_UI_BACKGROUND_BOX_WIDTH, NX_PRISM_UI_BACKGROUND_BOX_HEIGHT);
		ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_MEDIA_SELECTMODE_DIM_ID, &stRect, COL(C_G_Bang_90));
	}
	if (helpStrObj == NULL)
	{
		stRect = ONDK_Rect(NX_PRISM_MEDIA_SELECT_HELP_X, NX_PRISM_MEDIA_SELECT_HELP_Y, NX_PRISM_MEDIA_SELECT_HELP_W, NX_PRISM_MEDIA_SELECT_HELP_H);
		ONDK_GWM_COM_Text_Create(NX_PRISM_UI_MEDIA_SELECTMODE_HELPSTR_ID, &stRect, (HCHAR*)"Select items and then press the PLUS button.");
		ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_MEDIA_SELECTMODE_HELPSTR_ID, eFont_SystemBold);
		ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_MEDIA_SELECTMODE_HELPSTR_ID, NX_PRISM_FONT_SIZE_24);
		ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_MEDIA_SELECTMODE_HELPSTR_ID, (TEXT_ALIGN_LEFT| TEXT_MULTILINED));
		ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_MEDIA_SELECTMODE_HELPSTR_ID, COL(C_T_Yoyo01), COL(C_T_Bang_100));
	}

	if (bShow == TRUE)
	{
		ONDK_GWM_Obj_SetHide(NX_PRISM_UI_MEDIA_SELECTMODE_DIM_ID, FALSE);
		ONDK_GWM_Obj_SetHide(NX_PRISM_UI_MEDIA_SELECTMODE_HELPSTR_ID, FALSE);
	}
	else
	{
		ONDK_GWM_Obj_SetHide(NX_PRISM_UI_MEDIA_SELECTMODE_DIM_ID, TRUE);
		ONDK_GWM_Obj_SetHide(NX_PRISM_UI_MEDIA_SELECTMODE_HELPSTR_ID, TRUE);
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_OptListInitData(Nx_MediaContext_t *pstContents, HINT32 lObjId)
{
	NxFileOperation_e	operStatus = eFileOperation_Idle;
	HBOOL				bEjectableServer = FALSE;

	nx_media_FreeOptionTextList(pstContents);
	HxSTD_MemSet(&pstContents->stOptList, 0x00, sizeof(NX_Util_OPTIONList_t));

	operStatus = NX_FILEMGR_FILE_GetFileOperationStatus();

	if (lObjId == NX_PRISM_UI_MEDIA_CONTENT_LIST_ID)
	{
		if (pstContents->eCurrentServerType == eNxMedia_ServerType_Dlna)
		{
			return ONDK_RET_OK;
		}

		switch(operStatus)
		{
			case eFileOperation_Idle:
				if (pstContents->bSelectMode == TRUE)
				{
					NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Delete);
					NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Copy);
					NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Move);
				}
				else
				{
					NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Delete);
					NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Copy);
					NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Move);
					NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Select);
					NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_SelectAll);
				}
				break;
			case eFileOperation_Deleting:
				if (pstContents->bSelectMode == TRUE)
				{
					NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Delete);
				}
				else
				{
					NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Delete);
					NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Select);
					NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_SelectAll);
				}
				break;
			case eFileOperation_Copying:
				NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_StopCopying);
				break;
			case eFileOperation_Moving:
				NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_StopMoving);
				break;
			case eFileOperation_Stopping:
				break;
			default:
				break;
		}
	}
	else if (lObjId == NX_PRISM_UI_MEDIA_SERVER_LIST_ID)
	{
		bEjectableServer = nx_media_CheckStorageEjectableWithIdx(pstContents, ONDK_GWM_List_GetCurrentIndex(NX_PRISM_UI_MEDIA_SERVER_LIST_ID));
		if (bEjectableServer == TRUE)
		{
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Eject);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_Rename);
			NX_PRISM_UTIL_OptionAddItemList(&pstContents->stOptList, eNxUtil_OptItem_DeleteAll);
		}
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_DrawBottomDim(Nx_MediaContext_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return GWM_RET_SOURCE_NULL;
	}

	// Draw - Img
	stRect = ONDK_Rect(NX_PRISM_MEDIA_BOTTOM_DIM_X, NX_PRISM_MEDIA_BOTTOM_DIM_Y, NX_PRISM_MEDIA_BOTTOM_DIM_W, NX_PRISM_MEDIA_BOTTOM_DIM_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_MEDIA_BOTTOM_DIM_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_MEDIA_BOTTOM_DIM_ID, nx_media_DrawBottomDimFunc);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_DrawBottomDimFunc(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };

	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;

	ONDK_GUICOM_HBox(screen, stRect, RES_105_00_BG_BOTTOM_90_PNG, RES_105_00_BG_BOTTOM_90_PNG, RES_105_00_BG_BOTTOM_90_PNG);

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_DrawOptListPopup(Nx_MediaContext_t *pstContents, HINT32 lObjId)
{
	HINT32		y = 0, h = 0;
	HUINT32 	ulTextListNum = 0, ulIndex = 0;
	HUINT8		**pptextList = NULL, **ppImageListOfItem = NULL;
	ONDK_Rect_t stRect;

	nx_media_OptListInitData(pstContents, lObjId);

	if (pstContents->stOptList.ulOptItemNum == 0)
	{
		NX_APP_Info("Error!!! Please Check added Option item !!\n");
		return ONDK_RET_FAIL;
	}

	ulTextListNum = pstContents->stOptList.ulOptItemNum;
	ulIndex = 0;
	if (pstContents->szItemTextList == NULL)
	{
		pstContents->szItemTextList = NX_PRISM_UTIL_OptionMakeItemTextList(eNxUtil_OptType_TEXT, &pstContents->stOptList);
		pptextList = pstContents->szItemTextList;
		if (pptextList == NULL)
		{
			NX_APP_Info("Error!!! Please Check added Option item !!\n");
			return ONDK_RET_FAIL;
		}
	}
	else
	{
		pptextList = pstContents->szItemTextList;
	}

	// Set Text List
	h = (NX_PRISM_UI_OPT_POPUP_CELL_HEIGHT * ulTextListNum) + (NX_PRISM_UI_OPT_POPUP_CELL_MG * (ulTextListNum-1)) + NX_PRISM_UI_OPT_POPUP_POINTER_H;
	y = NX_PRISM_MEDIA_OPTION_LIST_Y2 - h - NX_PRISM_UI_OPT_POPUP_POINTER_H;
	stRect = ONDK_Rect(NX_PRISM_UI_OPT_POPUP_LIST_R_X, y, NX_PRISM_UI_OPT_POPUP_LIST_WIDTH, h);
	NX_PRISM_COMP_ListDlg_SetListPopup(&s_stMediaOptContents, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
							&stRect, (NX_PRISM_UI_OPT_POPUP_CELL_HEIGHT + NX_PRISM_UI_OPT_POPUP_CELL_MG),
							eListDlg_IMAGE_OptionRApp, ulTextListNum, ulTextListNum, ulIndex, (HUINT32)pptextList);

	NX_PRISM_COMP_ListDlg_SetFontSize(&s_stMediaOptContents, NX_PRISM_FONT_SIZE_22);

	// Draw BackGround
	stRect = ONDK_Rect(NX_PRISM_MEDIA_OPTION_DIM_X, NX_PRISM_MEDIA_OPTION_DIM_Y, NX_PRISM_MEDIA_OPTION_DIM_W, NX_PRISM_MEDIA_OPTION_DIM_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_MEDIA_OPTION_DIM_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_MEDIA_OPTION_DIM_ID, NX_PRISM_COMP_DrawWindowOptionRight);

	// Set Normal Image List
	if (pstContents->szImageNItemTextList == NULL)
	{
		pstContents->szImageNItemTextList = NX_PRISM_UTIL_OptionMakeItemTextList(eNxUtil_OptType_IMG_N, &pstContents->stOptList);
		ppImageListOfItem = pstContents->szImageNItemTextList;
		if (ppImageListOfItem == NULL)
		{
			NX_APP_Info("Error!!! Please Check added Option Normal Image item !!\n");
			return ONDK_RET_FAIL;
		}
	}
	else
	{
		ppImageListOfItem = pstContents->szImageNItemTextList;
	}

	NX_PRISM_COMP_ListDlg_SetImageList(&s_stMediaOptContents, (HUINT32)ppImageListOfItem);

	// Set Cursor Image List
	if (pstContents->szImageCItemTextList == NULL)
	{
		pstContents->szImageCItemTextList = NX_PRISM_UTIL_OptionMakeItemTextList(eNxUtil_OptType_IMG_C, &pstContents->stOptList);
		ppImageListOfItem = pstContents->szImageCItemTextList;
		if (ppImageListOfItem == NULL)
		{
			NX_APP_Info("Error!!! Please Check added Option Cursor Image item !!\n");
			return ONDK_RET_FAIL;
		}
	}
	else
	{
		ppImageListOfItem = pstContents->szImageCItemTextList;
	}

	NX_PRISM_COMP_ListDlg_SetImageExtList(&s_stMediaOptContents, (HUINT32)ppImageListOfItem);

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_media_DisplayListPopup", APP_DEFAULT_POPUP_PRIORITY, nx_media_DisplayListPopup, 0, 0, 0, 0);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_DrawContentList (Nx_MediaContext_t *pstContents)
{
	ONDK_Rect_t			stRect;
	HINT32				lItemCount = 0;
	LIST_OBJECT			listObj = NULL;

	if (pstContents->bUpperFolder == TRUE)
	{
		lItemCount = pstContents->ulCurrentContentLength + 1;
	}
	else
	{
		lItemCount = pstContents->ulCurrentContentLength;
	}

	// Draw - Content List
	listObj = (LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID);

	if (listObj)
	{
		ONDK_GWM_Obj_SetHide(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID, FALSE);
		ONDK_GWM_List_ChangeItems(listObj, lItemCount, 0);
		ONDK_GWM_APP_InvalidateObject(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID);
	}
	else
	{
		if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypeVideo)
		{
			stRect = ONDK_Rect(NX_MEDIA_CONTENTS_BG_X + NX_MEDIA_CONTENTS_BG_MARGIN_W, NX_MEDIA_CONTENTS_BG_Y + NX_MEDIA_CONTENTS_BG_MARGIN_H,
			NX_MEDIA_CONTENTS_ITEM_CURSOR_W + NX_PRISM_MEDIA_SCROLL_MG + NX_PRISM_MEDIA_SCROLLBALL_W, NX_MEDIA_CONTENTS_BG_H);
			ONDK_GWM_List_Create(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID, &stRect, "NxBrowseList", (NX_MEDIA_CONTENTS_ITEM_CURSOR_H + NX_MEDIA_CONTENTS_ITEM_MARGIN_H),
								lItemCount, NX_MEDIA_MAX_VIDEO_SHOW_ITEM_COUNT, 0);
			ONDK_GWM_List_SetFontHandle(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID, eFont_SystemBold, NX_PRISM_FONT_SIZE_24);
			ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID, 0);
			ONDK_GWM_List_SetSelectedIndex(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID, 0);
			ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID, (GWM_ObjDraw_t)nx_media_DrawVideoContentListFunc);
		}
		else if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypePhoto)
		{
			stRect = ONDK_Rect(NX_MEDIA_CONTENTS_BG_X + NX_MEDIA_CONTENTS_BG_MARGIN_W, NX_MEDIA_CONTENTS_BG_Y + NX_MEDIA_CONTENTS_BG_MARGIN_H,
			NX_MEDIA_CONTENTS_ITEM_CURSOR_W + NX_PRISM_MEDIA_SCROLL_MG + NX_PRISM_MEDIA_SCROLLBALL_W, NX_MEDIA_CONTENTS_BG_H);
			pstContents->lCount = lItemCount;
			ONDK_GWM_COM_GridList_Create(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID, &stRect, "NxBrowseList", (NX_MEDIA_PHOTO_CONTENTS_ITEM_CURSOR_W + NX_MEDIA_PHOTO_CONTENTS_ITEM_MARGIN_W),
				(NX_MEDIA_PHOTO_CONTENTS_ITEM_CURSOR_H + NX_MEDIA_PHOTO_CONTENTS_ITEM_MARGIN_H), lItemCount, NX_MEDIA_PHOTO_CONTENTS_MAX_COL, NX_MEDIA_PHOTO_CONTENTS_MAX_ROW, 0);

			ONDK_GWM_COM_GridList_SetItemDrawMethod(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID, (GRID_ItemDraw_t)nx_media_DrawPhotoContentListFunc);



		}
		else if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypeMusic)
		{
			//TODO: Draw Music
		}
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_DrawVideoContentListFunc(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0;
	HINT32		i, lCellHeight = 0, lScroll_w_off = 0, lScroll_height = 0, lNumOfPage = NX_MEDIA_MAX_VIDEO_SHOW_ITEM_COUNT;
	HINT32		lTotal = 0, lStart = 0, lCurrent = 0, lSelect = 0, lCursor_y_off = 0, lImgMargin = 0;
	HINT32		ulStatus = 0, lItem = 0, lItemCount = 0, lContentIdx = 0, lThumbReqIdx = 0, lImgObjId = 0;
	HCHAR		infoStr[MAX_ELEMENT_TEXT_LENGTH];
	HCHAR		durationStr[MAX_ELEMENT_TEXT_LENGTH];
	HBOOL		bFocus = FALSE, bObjFocus = FALSE, bScroll = FALSE;

	ONDK_Rect_t 	stImgRect;
	ONDK_Color_t	title_fontColor = 0x00000000, value_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDK_FontId_t	eFontId = eFont_SystemNormal;
	ONDKFontHandle	*hFontHandle = NULL;

	LIST_OBJECT 			obj = (LIST_OBJECT)object;
	Nx_MediaContext_t		*pstContents = nx_media_GetContents();
	Nx_MediaContentData_t 	*pstContentData = NULL;

	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	if (pstContents->eCurrentServerType == eNxMedia_ServerType_Storage)
	{
		if (pstContents->browseHandle == NULL || pstContents->browseHandle->itemList == NULL)
		{
			NX_APP_Error("ERROR!!! browseHandle is NULL !!\n");
			return	ONDK_RET_INITFAIL;
		}
	}
	else if (pstContents->eCurrentServerType == eNxMedia_ServerType_Dlna)
	{
#if defined(CONFIG_MW_MM_DLNA)
		if (pstContents->pDlnaContentList == NULL)
		{
			NX_APP_Error("ERROR!!! pDlnaContentList is NULL !!\n");
			return	ONDK_RET_INITFAIL;
		}
#endif
	}

	/* Get Item Count */
	if (pstContents->bUpperFolder == TRUE)
	{
		lItemCount = pstContents->ulCurrentContentLength + 1;
	}
	else
	{
		lItemCount = pstContents->ulCurrentContentLength;
	}

	ONDK_GWM_List_GetProperty(obj, &lTotal, &lStart, &lCurrent, &lSelect, &lNumOfPage);
	NX_APP_Info("lTotal[%d], stat[%d], lCurrent[%d], lSelect[%d], lNumOfPage[%d]\n", lTotal, lStart, lCurrent, lSelect, lNumOfPage);
	if ((lTotal == 0) || (lItemCount != lTotal))
	{
		NX_APP_Error("ERROR!!! (lTotal == 0) !!\n");
		return	ONDK_RET_FAIL;
	}

	/* Get Scroll value */
	if (NX_MEDIA_MAX_VIDEO_SHOW_ITEM_COUNT < lTotal)
	{
		bScroll = TRUE;
	}
	lScroll_w_off = NX_PRISM_MEDIA_SCROLL_MG + NX_PRISM_MEDIA_SCROLLBALL_W;
	lScroll_height = NX_PRISM_MEDIA_SCROLL_H;
	w = w - lScroll_w_off;

	/* Get Draw Data */
	lCursorMg = NX_MEDIA_SERVER_ITEM_MARGIN;
	ulStatus = ONDK_GWM_List_GetStatus(obj);
	lCellHeight = ONDK_GWM_List_GetCellHeight(ONDK_GWM_Obj_GetObjectID(object));
	if (object == ONDK_GWM_Obj_GetFocusedObject())
	{
		bObjFocus = TRUE;
	}
	else
	{
		bObjFocus = FALSE;
	}

	/* Init thumbnail */
	HxSTD_MemSet(pstContents->thumbRequest, 0, sizeof(Nx_MediaThumbRequest_t) * NX_MEDIA_MAX_VIDEO_SHOW_ITEM_COUNT);
	nx_media_HideThumbnail(pstContents);

	pstContentData = (Nx_MediaContentData_t *)NX_APP_Malloc(sizeof(Nx_MediaContentData_t));

	/* Draw Item List */
	for (i = 0; i < lTotal; i++)
	{
		lItem = lStart + i;

		if (lItem >= lTotal || i >= NX_MEDIA_MAX_VIDEO_SHOW_ITEM_COUNT)
		{
			NX_APP_Info("Index gets to max !!\n");
			break;
		}

		HxSTD_MemSet(pstContentData, 0, sizeof(Nx_MediaContentData_t));

		/* Get Content Data */
		if (pstContents->bUpperFolder == TRUE && lItem == 0) // Get Go Upper Title
		{
			HxSTD_sprintf((char*)pstContentData->szTitle, "%s", (char*)"Go back to the previous");
		}
		else
		{
			lContentIdx = (pstContents->bUpperFolder == TRUE) ? lItem - 1 : lItem;
			nx_media_GetContentData(pstContents, pstContentData, pstContents->eCurrentServerType, lContentIdx);
		}

		/* Draw Cursor */
		x = x1;
		y = y1 + lCursor_y_off;
		if (bUiTop > 0)
		{
			if (lItem == lCurrent && bObjFocus == TRUE)
			{
				/* get focused */
				bFocus = TRUE;
				title_fontColor = COL (C_T_Bang_100);
				value_fontColor = COL (C_T_Bang_100);
				cursorColor = COL(C_G_Base01_100);
				eFontId = eFont_SystemBold;
			}
			else
			{
				/* Normal Item cursor */
				bFocus = FALSE;
				title_fontColor = COL (C_T_Whistle_100);
				value_fontColor = COL (C_T_Gong05);
				cursorColor = COL(C_G_Whistle_15);
				eFontId = eFont_SystemNormal;
			}
		}
		else
		{
			/* history */
			if (lItem == lCurrent && bObjFocus == TRUE)
			{
				bFocus = TRUE;
				title_fontColor = COL (C_T_Whistle_100);
				value_fontColor = COL (C_T_Whistle_100);
				cursorColor = COL(C_G_Base01_20);
				eFontId = eFont_SystemBold;
			}
			else
			{
				bFocus = FALSE;
				title_fontColor = COL (C_T_Whistle_100);
				value_fontColor = COL (C_T_Whistle_100);
				cursorColor = COL(C_G_Whistle_15);
				eFontId = eFont_SystemNormal;
			}
		}
		ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight - lCursorMg, cursorColor);

		/* Get Image Margin on SelectMode */
		if (pstContents->bSelectMode == TRUE)
		{
			if (pstContentData->bDirectory == TRUE)
			{
				lImgMargin = NX_MEDIA_CONTENTS_ITEM_CHECH_X_TAB + NX_MEDIA_CONTENTS_ITEM_CHECH_W + NX_MEDIA_CONTENTS_ITEM_CHECK_FOLDER_GAP - NX_MEDIA_CONTENTS_ITEM_FOLDER_X_TAB;
			}
			else
			{
				lImgMargin = NX_MEDIA_CONTENTS_ITEM_CHECH_X_TAB + NX_MEDIA_CONTENTS_ITEM_CHECH_W + NX_MEDIA_CONTENTS_ITEM_CHECK_FILE_GAP - NX_MEDIA_CONTENTS_ITEM_THUMB_X_TAB;
			}
		}
		else
		{
			lImgMargin = 0;
		}

		/* Draw Title */
		hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, NX_PRISM_FONT_SIZE_24);
		if (hFontHandle)
		{
			x = x1 + NX_MEDIA_CONTENTS_ITEM_NAME_X_TAB + lImgMargin;
			y = y1 + lCursor_y_off + NX_MEDIA_CONTENTS_ITEM_NAME_Y_TAB;
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)pstContentData->szTitle, x, y, NX_MEDIA_CONTENTS_ITEM_NAME_W, title_fontColor, ONDKSTF_LEFT);
			ONDK_FONT_Release(hFontHandle);
		}

		/* Draw Item Info */
		if (pstContents->bUpperFolder == TRUE && lItem == 0) // Draw Go Upper Img
		{
			stImgRect = ONDK_Rect(x1 + NX_MEDIA_CONTENTS_ITEM_FOLDER_X_TAB + lImgMargin, y1 + lCursor_y_off + NX_MEDIA_CONTENTS_ITEM_FOLDER_Y_TAB,
										NX_MEDIA_CONTENTS_ITEM_FOLDER_W, NX_MEDIA_CONTENTS_ITEM_FOLDER_H);
			ONDK_GUICOM_IMG_Blit(screen, stImgRect, RES_420_05_ME_FOLDER_GOBACK_PNG);
		}
		else
		{
			if (pstContentData->bDirectory == TRUE) // Draw Folder Img
			{
				stImgRect = ONDK_Rect(x1 + NX_MEDIA_CONTENTS_ITEM_FOLDER_X_TAB + lImgMargin, y1 + lCursor_y_off + NX_MEDIA_CONTENTS_ITEM_FOLDER_Y_TAB,
										NX_MEDIA_CONTENTS_ITEM_FOLDER_W, NX_MEDIA_CONTENTS_ITEM_FOLDER_H);
				if (pstContentData->bHasSubFile == TRUE)
				{
					ONDK_GUICOM_IMG_Blit(screen, stImgRect, RES_420_05_ME_FOLDER_PLUS_PNG);
				}
				else
				{
					ONDK_GUICOM_IMG_Blit(screen, stImgRect, RES_420_05_ME_FOLDER_PNG);
				}
			}
			else
			{
				/* Draw File Duration & Date */
				hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, NX_PRISM_FONT_SIZE_18);
				if (hFontHandle)
				{
					x = x1 + NX_MEDIA_CONTENTS_ITEM_INFO_X_TAB + lImgMargin;
					y = y1 + lCursor_y_off + NX_MEDIA_CONTENTS_ITEM_INFO_Y_TAB;

					if (pstContents->eCurrentServerType == eNxMedia_ServerType_Storage)
					{
						nx_media_GetDurationStr((HCHAR*)durationStr, pstContentData->ulDuration);
						HxSTD_sprintf((char*)infoStr, "%s / %s", (char*)durationStr, (char*)pstContentData->szDate);
					}
					else if (pstContents->eCurrentServerType == eNxMedia_ServerType_Dlna)
					{
						HxSTD_sprintf((char*)infoStr, "%s / %s", (char*)"0", (char*)"0");
					}
					ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)infoStr, x, y, NX_MEDIA_CONTENTS_ITEM_NAME_W, value_fontColor, ONDKSTF_LEFT);
					ONDK_FONT_Release(hFontHandle);
				}

				/* Draw Thumbnail */
				stImgRect = ONDK_Rect(x1 + NX_MEDIA_CONTENTS_ITEM_THUMB_X_TAB + lImgMargin, y1 + lCursor_y_off + NX_MEDIA_CONTENTS_ITEM_THUMB_Y_TAB,
										NX_MEDIA_CONTENTS_ITEM_THUMB_W, NX_MEDIA_CONTENTS_ITEM_THUMB_H);
				if (pstContentData->szThumbnail)
				{
					HxSTD_sprintf((char*)pstContents->thumbRequest[lThumbReqIdx].szPath, "%s", (char*)pstContentData->szThumbnail);
					if (HLIB_FILE_Exist((HUINT8*)pstContentData->szThumbnail) == TRUE)
					{
						pstContents->thumbRequest[lThumbReqIdx].bGetThumb = TRUE;
						lImgObjId = nx_media_GetThumbImgObjId(i);
						if (lImgObjId != 0)
						{
							if (ONDK_GWM_Obj_GetObject(lImgObjId) == NULL)
							{
								ONDK_GWM_COM_Image_Create(lImgObjId, &stImgRect);
							}
							else
							{
								ONDK_GWM_COM_Image_ChangeRect(lImgObjId, &stImgRect);
							}
							ONDK_GWM_COM_Image_SetImage(lImgObjId, (HCHAR*)pstContentData->szThumbnail);
							ONDK_GWM_Obj_SetHide(lImgObjId, FALSE);
						}
					}
					else
					{
						pstContents->thumbRequest[lThumbReqIdx].bGetThumb = FALSE;
						ONDK_GUICOM_IMG_Blit(screen, stImgRect, RES_212_05_THUMB_ME_VIDEO_PNG);
					}
					lThumbReqIdx++;
				}
				else
				{
					ONDK_GUICOM_IMG_Blit(screen, stImgRect, RES_212_05_THUMB_ME_VIDEO_PNG);
				}
			}
			if (pstContents->bSelectMode == TRUE && pstContents->pSelectedList != NULL)
			{
				stImgRect = ONDK_Rect(x1 + NX_MEDIA_CONTENTS_ITEM_CHECH_X_TAB, y1 + lCursor_y_off + NX_MEDIA_CONTENTS_ITEM_CHECH_Y_TAB,
										NX_MEDIA_CONTENTS_ITEM_CHECH_W, NX_MEDIA_CONTENTS_ITEM_CHECH_H);
				if (pstContents->pSelectedList[lContentIdx] == TRUE)
				{
					ONDK_GUICOM_IMG_Blit(screen, stImgRect, RES_112_00_CHECK03_ON_PNG);
				}
				else
				{
					ONDK_GUICOM_IMG_Blit(screen, stImgRect, RES_112_00_CHECK03_OFF_PNG);
				}
			}
		}
		lCursor_y_off += lCellHeight;
	}

	NX_APP_Free(pstContentData);

	// Draw Scroll
	if (bScroll == TRUE)
	{
		HINT32	lScrollBall_step = 0, lHeight = 0;
		HINT32	lAllPage = 0, lScrollBall_height = 0, lScroll_y_offset = 0;

		lAllPage = (lTotal / lNumOfPage);
		lAllPage += (lTotal % lNumOfPage) ? 1 : 0;

		lScrollBall_height = lScroll_height / lAllPage;
		if (lScrollBall_height < NX_PRISM_MEDIA_SCROLLBALL_MIN_H)
		{
			lScrollBall_height = NX_PRISM_MEDIA_SCROLLBALL_MIN_H;
		}

		lHeight = (lScroll_height - lScrollBall_height);
		/* calculate vertical position */
		for (i = 1; lTotal > lHeight; i++)
		{
			lTotal /= i;
		} //while (total > (460 - lScrollBall_height));

		lScrollBall_step = lHeight / (lTotal - 1);
		lScroll_y_offset = (lCurrent / i) * lScrollBall_step;
		lScrollBall_height += (lHeight - ((lTotal - 1) * lScrollBall_step));

		x = x1 + w + NX_PRISM_MEDIA_SCROLL_MG;
		y = y1;

		// Draw Scroll bar
		ONDK_DRAW_FillRectElement(screen, x + NX_PRISM_MEDIA_SCROLL_X, y, NX_PRISM_MEDIA_SCROLL_W, NX_PRISM_MEDIA_SCROLL_H, COL(C_G_Whistle_20));

		// Draw Scroll Bar Ball
		stImgRect = ONDK_Rect(x, y + lScroll_y_offset, NX_PRISM_MEDIA_SCROLLBALL_W, lScrollBall_height);
		ONDK_GUICOM_VBox(screen, stImgRect, RES_111_00_SCROLL_BAR_1U_PNG, RES_111_00_SCROLL_BAR_2M_PNG, RES_111_00_SCROLL_BAR_3D_PNG);
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_DrawPhotoContentListFunc(HINT32 objId, ONDKSurfaceHandle screen, GRID_DrawItem_t *drawItem, HINT32 nColIndex)
{
	Nx_MediaContext_t	*pstContents = nx_media_GetContents();
	ONDK_Result_t		eResult = ONDK_RET_FAIL;
	HINT32				x = 0, y = 0, w = 0, lCursorMgW = 0, lCursorMgH = 0;
	HINT32				i = 0, lCellWidth = 0, lCellHeight = 0, lScroll_w_off = 0, lScroll_height;
	HINT32				lTotal = 0, lCurrentIndex = 0, lCurrent = 0;
	HINT32				ulStatus = 0, lItem = 0, lContentIdx = 0;
	HBOOL				bFocus = FALSE;
	HBOOL				bObjFocus = FALSE;
	HBOOL				bScroll = FALSE;


	ONDK_Rect_t 		stDstRect;
	ONDK_Color_t		title_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDK_FontId_t		eFontId = eFont_SystemBold;
	ONDKFontHandle		hFontHandle;

	Nx_MediaContentData_t 	*pstContentData = NULL;

	lCurrentIndex = ONDK_GWM_COM_GridList_GetCurrentIndexByObjID(objId);
	lItem = drawItem->item_index;

	if (pstContents->eCurrentServerType == eNxMedia_ServerType_Storage)
	{
		if (pstContents->browseHandle == NULL || pstContents->browseHandle->itemList == NULL)
		{
			NX_APP_Error("ERROR!!! browseHandle is NULL !!\n");
			return	ONDK_RET_INITFAIL;
		}
	}
	else if (pstContents->eCurrentServerType == eNxMedia_ServerType_Dlna)
	{
#if defined(CONFIG_MW_MM_DLNA)
		if (pstContents->pDlnaContentList == NULL)
		{
			NX_APP_Error("ERROR!!! pDlnaContentList is NULL !!\n");
			return	ONDK_RET_INITFAIL;
		}
#endif
	}

//	if (pstContents->bUpperFolder == TRUE)
//	{
//		if (pstContents->ulCurrentContentLength == 0)
//		{
//			lTotal = 1;
//		}
//		else
//		{
//			lTotal = HLIB_VECTOR_Length(pstContents->browseHandle->itemList) + 1;
//		}
//	}
//	else
	{
		lTotal = HLIB_VECTOR_Length(pstContents->browseHandle->itemList);
	}

	if ((NX_MEDIA_PHOTO_MAX_SHOW_COL_COUNT * NX_MEDIA_PHOTO_MAX_SHOW_ROW_COUNT) < lTotal)
	{
		bScroll = TRUE;
	}
	lScroll_w_off = NX_PRISM_MEDIA_SCROLL_MG + NX_PRISM_MEDIA_SCROLLBALL_W;
	lScroll_height = NX_PRISM_MEDIA_SCROLL_H;
	w = w - lScroll_w_off;

	// Draw Data
	lCursorMgW = NX_MEDIA_PHOTO_CONTENTS_ITEM_MARGIN_W;
	lCursorMgH = NX_MEDIA_PHOTO_CONTENTS_ITEM_MARGIN_H;
	lCellWidth = NX_MEDIA_PHOTO_CONTENTS_ITEM_CURSOR_W + lCursorMgW;
	lCellHeight = NX_MEDIA_PHOTO_CONTENTS_ITEM_CURSOR_H + lCursorMgH;
	ulStatus = ONDK_GWM_Obj_GetObjectStatus(ONDK_GWM_Obj_GetObjectByID(objId));

	if (lCurrentIndex == lItem)
	{
		bObjFocus = TRUE;
	}
	else
	{
		bObjFocus = FALSE;
	}

	// Init thumbnail
	if (lItem == 0)
	{
		pstContents->lPhotoThumbReqIdx = 0;
		HxSTD_MemSet(pstContents->thumbRequest, 0, sizeof(Nx_MediaThumbRequest_t) * (NX_MEDIA_PHOTO_MAX_SHOW_COL_COUNT * NX_MEDIA_PHOTO_MAX_SHOW_ROW_COUNT));
		nx_media_HideThumbnail(pstContents);
	}

	pstContentData = (Nx_MediaContentData_t *)NX_APP_Malloc(sizeof(Nx_MediaContentData_t));

	// Draw Item
	// bUpperFolder가 true이면 folder그리고 원래 그리려던 아이템도 그려야함.

	HxSTD_MemSet(pstContentData, 0, sizeof(Nx_MediaContentData_t));

//	if (pstContents->bUpperFolder == TRUE && lItem == 0) // Get Go Upper Title
//	{
//		title = "Go back to the previous";
//	}
//	else
	{
//		lContentIdx = (pstContents->bUpperFolder == TRUE) ? lItem - 1 : lItem;
		lContentIdx = lItem;
		nx_media_GetContentData(pstContents, pstContentData, pstContents->eCurrentServerType, lContentIdx);
	}

	// Draw Cursor
	if (ulStatus == GUIOBJ_HILIGHTED)
	{
		if (lCurrentIndex == lItem)
		{
			/* get focused */
			bFocus = TRUE;
			title_fontColor = COL (C_T_Base01);
			cursorColor = COL(C_G_Base01_100);
		}
		else
		{
			/* Normal Item cursor */
			bFocus = FALSE;
			title_fontColor = COL (C_T_Gong05);
			cursorColor = COL(C_G_Base01_100);
		}
	}
	else
	{
		/* history */
		if (lCurrentIndex == lItem)
		{
			bFocus = TRUE;
			title_fontColor = COL (C_T_Base02);
			cursorColor = COL(C_G_Base02_100);
		}
		else
		{
			bFocus = FALSE;
			title_fontColor = COL (C_T_Gong05);
			cursorColor = COL(C_G_Base02_100);
		}
	}

	if (bFocus == TRUE)
	{
		ONDK_DRAW_StrokeRect(screen, drawItem->itemRect.x, drawItem->itemRect.y, drawItem->itemRect.x + (drawItem->itemRect.w - lCursorMgW),
			drawItem->itemRect.y + (lCellHeight - lCursorMgH), NX_MEDIA_PHOTO_CONTENTS_ITEM_CURSOR_BORDER, cursorColor);
	}

	// Calculate Gab with Select Img
	if (pstContents->bSelectMode == TRUE)
	{
	}

	// Draw Title
	hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, NX_PRISM_FONT_SIZE_20);
	if (hFontHandle)
	{
		x = drawItem->itemRect.x + NX_MEDIA_PHOTO_CONTENTS_ITEM_NAME_X_TAB;
		y = drawItem->itemRect.y + NX_MEDIA_PHOTO_CONTENTS_ITEM_NAME_Y_TAB;
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)pstContentData->szTitle, x, y, NX_MEDIA_PHOTO_CONTENTS_ITEM_CURSOR_W, title_fontColor, ONDKSTF_CENTER);
		ONDK_FONT_Release(hFontHandle);
	}

	// Draw Thumbnail
	if (pstContentData->bDirectory == TRUE) // Draw Folder Img
	{
		stDstRect = ONDK_Rect(drawItem->itemRect.x + NX_MEDIA_PHOTO_CONTENTS_ITEM_FOLDER_X_TAB, drawItem->itemRect.y + NX_MEDIA_PHOTO_CONTENTS_ITEM_FOLDER_Y_TAB,
								NX_MEDIA_PHOTO_CONTENTS_ITEM_FOLDER_W, NX_MEDIA_PHOTO_CONTENTS_ITEM_FOLDER_H);
		if (pstContentData->bHasSubFile == TRUE)
		{
			ONDK_GUICOM_IMG_Blit(screen, stDstRect, RES_420_05_ME_FOLDER_PLUS_PNG);
		}
		else
		{
			ONDK_GUICOM_IMG_Blit(screen, stDstRect, RES_420_05_ME_FOLDER_PNG);
		}
	}
	else
	{
		if (pstContentData->szThumbnail)
		{
			HxSTD_sprintf((char*)pstContents->thumbRequest[pstContents->lPhotoThumbReqIdx].szPath, "%s", (char*)pstContentData->szThumbnail);
			if (HLIB_FILE_Exist((HUINT8*)pstContentData->szThumbnail) == TRUE)
			{
				ONDK_Rect_t stSrcRect;
				ONDK_Size_t size;
				ONDKImageHandle hThum = NULL;

				pstContents->thumbRequest[pstContents->lPhotoThumbReqIdx].bGetThumb = TRUE;

				hThum = ONDK_IMG_CreateSelf((HUINT8*)pstContents->thumbRequest[pstContents->lPhotoThumbReqIdx].szPath);

				size = ONDK_IMG_GetSize(hThum);
				stSrcRect.x = 0;
				stSrcRect.y = 0;
				stSrcRect.w = size.w;
				stSrcRect.h = size.h;

				stDstRect = ONDK_Rect(drawItem->itemRect.x + NX_MEDIA_PHOTO_CONTENTS_ITEM_THUMB_X_TAB, drawItem->itemRect.y + NX_MEDIA_PHOTO_CONTENTS_ITEM_THUMB_Y_TAB,
									NX_MEDIA_PHOTO_CONTENTS_ITEM_THUMB_W, NX_MEDIA_PHOTO_CONTENTS_ITEM_THUMB_H);
				ONDK_SURFACE_StretchBlit(hThum, screen, stSrcRect, stDstRect);
			}
			else
			{
				pstContents->thumbRequest[pstContents->lPhotoThumbReqIdx].bGetThumb = FALSE;
				ONDK_GUICOM_IMG_Blit(screen, stDstRect, RES_212_05_THUMB_ME_PHOTO_PNG);
			}
			pstContents->lPhotoThumbReqIdx++;
		}
		else
		{
			ONDK_GUICOM_IMG_Blit(screen, stDstRect, RES_212_05_THUMB_ME_PHOTO_PNG);
		}
	}

	NX_APP_Free(pstContentData);

	// Draw Scroll
//	if (bScroll == TRUE)
//	{
//		HINT32	lScrollBall_step = 0, lHeight = 0;
//		HINT32	lAllPage = 0, lScrollBall_height = 0, lScroll_y_offset = 0;
//
//		lAllPage = (lTotal / (NX_MEDIA_PHOTO_MAX_SHOW_COL_COUNT * NX_MEDIA_PHOTO_MAX_SHOW_ROW_COUNT));
//		lAllPage += (lTotal % (NX_MEDIA_PHOTO_MAX_SHOW_COL_COUNT * NX_MEDIA_PHOTO_MAX_SHOW_ROW_COUNT)) ? 1 : 0;
//
//		lScrollBall_height = lScroll_height / lAllPage;
//		if (lScrollBall_height < NX_PRISM_MEDIA_SCROLLBALL_MIN_H)
//		{
//			lScrollBall_height = NX_PRISM_MEDIA_SCROLLBALL_MIN_H;
//		}
//
//		lHeight = (lScroll_height - lScrollBall_height);
//		/* calculate vertical position */
//		for (i = 1; lTotal > lHeight; i++)
//		{
//			lTotal /= i;
//		} //while (total > (460 - lScrollBall_height));
//
//		lScrollBall_step = lHeight / (lTotal - 1);
//		lScroll_y_offset = (lCurrent / i) * lScrollBall_step;
//		lScrollBall_height += (lHeight - ((lTotal - 1) * lScrollBall_step));
//
//		x = NX_MEDIA_CONTENTS_BG_X + NX_MEDIA_CONTENTS_BG_MARGIN_W + NX_MEDIA_CONTENTS_ITEM_CURSOR_W + NX_PRISM_MEDIA_SCROLL_MG;
//		y = NX_MEDIA_CONTENTS_BG_Y + NX_MEDIA_CONTENTS_BG_MARGIN_H;
//
//		// Draw Scroll bar
//		ONDK_DRAW_FillRectElement(screen, x + NX_PRISM_MEDIA_SCROLL_X, y, NX_PRISM_MEDIA_SCROLL_W, NX_PRISM_MEDIA_SCROLL_H, COL(C_G_Whistle_20));
//
//		// Draw Scroll Bar Ball
//		stImgRect = ONDK_Rect(x, y + lScroll_y_offset, NX_PRISM_MEDIA_SCROLLBALL_W, lScrollBall_height);
//		ONDK_GUICOM_VBox(screen, stImgRect, RES_111_00_SCROLL_BAR_1U_PNG, RES_111_00_SCROLL_BAR_2M_PNG, RES_111_00_SCROLL_BAR_3D_PNG);
//	}


	return ONDK_RET_OK;
}

ONDK_Result_t	nx_media_DrawServerListFunc(ONDKSurfaceHandle *screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 0;
	HINT32		i, lCellHeight = 0, lNumOfPage = NX_MEDIA_MAX_SHOW_SERVER_COUNT;
	HINT32		lTotal = 0, lStart = 0, lCurrent = 0, lSelect = 0, lCursor_y_off = 0;

	HINT32		ulStatus = 0, ulFontSize = 0, lItem = 0, lStorageCount = 0;
	HCHAR		*title = NULL;
	HCHAR		*imgId = NULL;
	HBOOL		bFocus = FALSE, bObjFocus = FALSE, bLastInSameType = FALSE, bCurrentBrowsed = FALSE;
	HERROR		hError = ERR_FAIL;

	ONDK_Rect_t 	stImgRect;
	ONDK_Color_t	title_fontColor = 0x00000000, value_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDK_FontId_t	eFontId = eFont_SystemNormal;
	ONDKFontHandle	*hFontHandle = NULL;

	LIST_OBJECT 				obj = (LIST_OBJECT)object;
	Nx_MediaContext_t			*pstContents = nx_media_GetContents();
	NX_PhysicalStorageData_t 	*physicalStorageData = NULL;
#if defined(CONFIG_MW_MM_DLNA)
	NX_DLNA_DMS_Data_t			*pstDlnaDmsData = NULL;
#endif
	Nx_MediaServerType_e		eMediaServerType;

	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	ONDK_GWM_List_GetProperty(obj, &lTotal, &lStart, &lCurrent, &lSelect, &lNumOfPage);
	NX_APP_Info("lTotal[%d], stat[%d], lCurrent[%d], lSelect[%d], lNumOfPage[%d]\n", lTotal, lStart, lCurrent, lSelect, lNumOfPage);
	lStorageCount = pstContents->ulStorageListNum;

	if ((lTotal == 0) || (lTotal != (lStorageCount + pstContents->ulDmsListNum + NX_MEDIA_SERVER_RECENT_COUNT)))
	{
		NX_APP_Error("ERROR!!! lTotal count error !!\n");
		return	ONDK_RET_FAIL;
	}

	ulFontSize = NX_PRISM_FONT_SIZE_24;
	lCursorMg = NX_MEDIA_SERVER_ITEM_MARGIN;
	ulStatus = ONDK_GWM_List_GetStatus(obj);
	bObjFocus = (object == ONDK_GWM_Obj_GetFocusedObject()) ? TRUE : FALSE;

	/* active, front most */
	for (i = 0; i < lTotal; i++)
	{
		lItem = lStart + i;

		if (lItem >= lTotal)
		{
			NX_APP_Error("ERROR!!! up to max schedule count !!\n");
			break;
		}

		if (lItem == 0) //recent
		{
			eMediaServerType = eNxMedia_ServerType_Recent;
			title = (HCHAR*)ONDK_GetString(RES_RECENT_01_ID);
			bLastInSameType = TRUE;
		}
		else if ((lItem > 0) && (lItem < lStorageCount + NX_MEDIA_SERVER_RECENT_COUNT)) //storage
		{
			eMediaServerType = eNxMedia_ServerType_Storage;
			hError = NX_FILEMGR_STORAGE_GetPhysicalStorageInfoWithDevId(pstContents->pStorageDevIdList[lItem - NX_MEDIA_SERVER_RECENT_COUNT], &physicalStorageData);
			if (hError != ERR_OK || physicalStorageData == NULL)
			{
				NX_APP_Error("ERROR!!! Get Storage Info Fail !!\n");
				break;
			}
			title = nx_media_GetStorageLabelFromObjInfo(physicalStorageData);
			bLastInSameType = (lItem == lStorageCount) ? TRUE : FALSE;
		}
		else
		{
			eMediaServerType = eNxMedia_ServerType_Dlna;
#if defined(CONFIG_MW_MM_DLNA)
			pstDlnaDmsData = HLIB_VECTOR_ItemAt(pstContents->pDmsList, lItem - (lStorageCount + NX_MEDIA_SERVER_RECENT_COUNT));
			title = pstDlnaDmsData->szTitle;
#endif
			bLastInSameType = FALSE;
		}

		lCellHeight = (bLastInSameType == TRUE) ? (NX_MEDIA_SERVER_ITEM_H + NX_MEDIA_SERVER_SUBJECT_MARGIN) : (NX_MEDIA_SERVER_ITEM_H + NX_MEDIA_SERVER_ITEM_MARGIN);

		if (title == NULL)
		{
			NX_APP_Error("Assert!!! Please Check!! \n");
			break;
		}

		x = x1;
		y = y1 + lCursor_y_off;
		if (bUiTop > 0)
		{
			if (lItem == lCurrent && bObjFocus == TRUE)
			{
				/* get focused */
				bFocus = TRUE;
				title_fontColor = COL (C_T_Bang_100);
				value_fontColor = COL (C_T_Bang_100);
				cursorColor = COL(C_G_Base01_100);
				eFontId = eFont_SystemBold;
			}
			else
			{
				/* Normal Item cursor */
				switch (eMediaServerType)
				{
					case eNxMedia_ServerType_Recent:
						bCurrentBrowsed = FALSE;
						break;
					case eNxMedia_ServerType_Storage:
						if (physicalStorageData != NULL && (HxSTD_StrCmp((char*)pstContents->ucCurrentServerPath, (char*)physicalStorageData->pszMountpath) == 0)) //If Current Browsed Server
						{
							bCurrentBrowsed = TRUE;
						}
						else
						{
							bCurrentBrowsed = FALSE;
						}
						break;
					case eNxMedia_ServerType_Dlna:
#if defined(CONFIG_MW_MM_DLNA)
						if (pstDlnaDmsData != NULL && (HxSTD_StrCmp((char*)pstContents->ucCurrentServerPath, (char*)pstDlnaDmsData->szUDN) == 0))
						{
							bCurrentBrowsed = TRUE;
						}
						else
						{
							bCurrentBrowsed = FALSE;
						}
#endif
						break;
					default:
						bCurrentBrowsed = FALSE;
						break;
				}

				if (bCurrentBrowsed == TRUE)
				{
					bFocus = TRUE;
					title_fontColor = COL (C_T_Whistle_100);
					value_fontColor = COL (C_T_Whistle_100);
					cursorColor = COL(C_G_Base01_20);
					eFontId = eFont_SystemBold;
				}
				else
				{
					bFocus = FALSE;
					title_fontColor = COL (C_T_Whistle_100);
					value_fontColor = COL (C_T_Whistle_100);
					cursorColor = COL(C_G_Whistle_15);
					eFontId = eFont_SystemNormal;
				}
			}
		}
		else
		{
			/* history */
			if (lItem == lCurrent && bObjFocus == TRUE)
			{
				bFocus = TRUE;
				title_fontColor = COL (C_T_Whistle_100);
				value_fontColor = COL (C_T_Whistle_100);
				cursorColor = COL(C_G_Base01_20);
				eFontId = eFont_SystemBold;
			}
			else
			{
				bFocus = FALSE;
				title_fontColor = COL (C_T_Whistle_100);
				value_fontColor = COL (C_T_Whistle_100);
				cursorColor = COL(C_G_Whistle_15);
				eFontId = eFont_SystemNormal;
			}
		}
		ONDK_DRAW_FillRectElement(screen, x, y, w, NX_MEDIA_SERVER_ITEM_H, cursorColor);

		hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
		if (hFontHandle)
		{
			// SubItem - Title
			x = x1 + NX_MEDIA_SERVER_ITEM_TXT_X_TAB;
			y = y1 + lCursor_y_off + NX_MEDIA_SERVER_ITEM_TXT_Y_TAB;
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)title, x, y, NX_MEDIA_SERVER_ITEM_TXT_W, title_fontColor, ONDKSTF_LEFT);
			ONDK_FONT_Release(hFontHandle);
		}

		switch(eMediaServerType)
		{
			case eNxMedia_ServerType_Recent:
				imgId = (bFocus == TRUE) ? RES_421_05_RECENT_C_PNG : RES_421_05_RECENT_N_PNG;
				break;
			case eNxMedia_ServerType_Storage:
				imgId = nx_media_GetStorageIconIdFromObjInfo(physicalStorageData, bFocus);
				break;
			case eNxMedia_ServerType_Dlna:
				if (bUiTop > 0)
				{
					imgId = (bFocus == TRUE) ? RES_214_05_STORAGE_DLNA_C_PNG : RES_214_05_STORAGE_DLNA_N_PNG;
				}
				else
				{
					imgId = (bFocus == TRUE) ? RES_214_05_STORAGE_DLNA_H_PNG : RES_214_05_STORAGE_DLNA_N_PNG;
				}
				break;
		}

		if (imgId != NULL)
		{
			stImgRect = ONDK_Rect(x1 + NX_MEDIA_SERVER_ITEM_ICON_X_TAB, y1 + lCursor_y_off + NX_MEDIA_SERVER_ITEM_ICON_Y_TAB,
										NX_MEDIA_SERVER_ITEM_ICON_W, NX_MEDIA_SERVER_ITEM_ICON_H);
			ONDK_GUICOM_IMG_Blit(screen, stImgRect, imgId);
		}

		lCursor_y_off += lCellHeight;
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_DrawDefaultUI(Nx_MediaContext_t *pstContents)
{
	ONDK_Rect_t	stRect;
	HINT32		serverItemCount = 0;

	// Draw - Background - Black_80
	nx_media_DrawBackground();

	// Draw - Bottom Dim
	nx_media_DrawBottomDim(pstContents);

	// Draw - Title
	switch(pstContents->ulContentsType)
	{
		case eNxUtil_OptItem_MediaTypeVideo:
			NX_PRISM_COMP_Media_DrawTitle(ONDK_GetString(RES_VIDEO_01_ID));
			break;
		case eNxUtil_OptItem_MediaTypePhoto:
			NX_PRISM_COMP_Media_DrawTitle(ONDK_GetString(RES_PHOTO_01_ID));
			break;
		case eNxUtil_OptItem_MediaTypeMusic:
			NX_PRISM_COMP_Media_DrawTitle(ONDK_GetString(RES_MUSIC_01_ID));
			break;
	}
	NX_PRISM_COMP_Media_DrawTitleLine();
	nx_media_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);

	// Draw - Contents BG
	stRect = ONDK_Rect(NX_MEDIA_CONTENTS_BG_X, NX_MEDIA_CONTENTS_BG_Y, NX_MEDIA_CONTENTS_BG_W, NX_MEDIA_CONTENTS_BG_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_MEDIA_CONTENT_BG_ID, &stRect, COL(C_G_Whistle_05));

	// Draw - No Item Info
	nx_media_ShowNoItemInfo(pstContents, TRUE, eNxMedia_ServerType_Recent);

	// Draw - Operating Dlg
	//nx_media_ShowOperatingDlg(pstContents, TRUE);

	// Draw - Server List
	serverItemCount = pstContents->ulStorageListNum + pstContents->ulDmsListNum + 1;
	stRect = ONDK_Rect(NX_MEDIA_SERVER_LIST_X, NX_MEDIA_SERVER_LIST_Y,
							NX_MEDIA_SERVER_ITEM_W, (NX_MEDIA_SERVER_ITEM_H * NX_MEDIA_MAX_SHOW_SERVER_COUNT + NX_MEDIA_SERVER_SUBJECT_MARGIN * 2 + NX_MEDIA_SERVER_ITEM_MARGIN * 6));
	ONDK_GWM_List_Create(NX_PRISM_UI_MEDIA_SERVER_LIST_ID, &stRect, "NxServerList", (NX_MEDIA_SERVER_ITEM_H + NX_MEDIA_SERVER_ITEM_MARGIN),
							serverItemCount, NX_MEDIA_MAX_SHOW_SERVER_COUNT, 0);
	ONDK_GWM_List_SetFontHandle(NX_PRISM_UI_MEDIA_SERVER_LIST_ID, eFont_SystemBold, NX_PRISM_FONT_SIZE_24);
	ONDK_GWM_List_SetCurrentIndex(NX_PRISM_UI_MEDIA_SERVER_LIST_ID, 0);
	ONDK_GWM_List_SetSelectedIndex(NX_PRISM_UI_MEDIA_SERVER_LIST_ID, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_MEDIA_SERVER_LIST_ID, (GWM_ObjDraw_t)nx_media_DrawServerListFunc);

	ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_MEDIA_SERVER_LIST_ID);

	return	ONDK_RET_OK;
}

#define ___________SELECTION_HANDLING___________

static ONDK_Result_t	nx_media_ServerListSelect(Nx_MediaContext_t *pstContents, HINT32 lIndex)
{
	HERROR			hError;
	NX_PhysicalStorageData_t	*physicalStorageData = NULL;
	HCHAR			*pszFilter = NULL;

	/* Set Filter */
	if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypeVideo)
	{
		pszFilter = NXFILE_FILTER_MOVIE;
	}
	else if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypePhoto)
	{
		pszFilter = NXFILE_FILTER_PHOTO;
	}
	else if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypeMusic)
	{
		//TODO: FILTER_MUSIC
	}

	if (pstContents->browseHandle)
	{
		NX_FILEMGR_BROWSER_DestroyHandle(pstContents->browseHandle);
		pstContents->browseHandle = NULL;
	}
#if defined(CONFIG_MW_MM_DLNA)
	if (pstContents->pDlnaContentList)
	{
		NX_DLNA_RemoveAllBrowsedItem();
		pstContents->pDlnaContentList = NULL;
	}
#endif

	/* Select Handling */
	if (lIndex == 0) //recent
	{
		pstContents->eCurrentServerType = eNxMedia_ServerType_Recent;
		return	ONDK_RET_OK;
	}
	else if ((lIndex > 0) && (lIndex < pstContents->ulStorageListNum + NX_MEDIA_SERVER_RECENT_COUNT)) //storage
	{
		hError = NX_FILEMGR_STORAGE_GetPhysicalStorageInfoWithDevId(pstContents->pStorageDevIdList[lIndex - NX_MEDIA_SERVER_RECENT_COUNT], &physicalStorageData);
		if (hError != ERR_OK || physicalStorageData == NULL)
		{
			NX_APP_Error("Error!!! GetPhysicalStorageInfoWithDevId FAIL !!\n");
			return ONDK_RET_INITFAIL;
		}
		pstContents->eCurrentServerType = eNxMedia_ServerType_Storage;
		pstContents->ulCurrentServerId = pstContents->pStorageDevIdList[lIndex - NX_MEDIA_SERVER_RECENT_COUNT];

		// Show Progress Dialog
		nx_media_Ui_ProcessingMsgCreate(eNxMedia_Dialog_Progress_Connecting);
		pstContents->ulCurrentDlg = eNxMedia_Dialog_Progress_Connecting;

		pstContents->bUpperFolder = FALSE;
		HxSTD_sprintf((char*)pstContents->ucCurrentServerPath, "%s", (char*)physicalStorageData->pszMountpath);
		pstContents->browseHandle = NX_FILEMGR_BROWSER_CreateHandle();
		NX_FILEMGR_BROWSER_SetPath(pstContents->browseHandle, physicalStorageData->pszMountpath);
		NX_FILEMGR_BROWSER_BrowseByFolderView(pstContents->browseHandle, pszFilter);
	}
	else if (lIndex < pstContents->ulStorageListNum + pstContents->ulDmsListNum + NX_MEDIA_SERVER_RECENT_COUNT) //dlna
	{
#if defined(CONFIG_MW_MM_DLNA)
		NX_DLNA_DMS_Data_t			*pstDlnaDmsData = NULL;

		pstContents->eCurrentServerType = eNxMedia_ServerType_Dlna;

		// Show Progress Dialog
		nx_media_Ui_ProcessingMsgCreate(eNxMedia_Dialog_Progress_Connecting);
		pstContents->ulCurrentDlg = eNxMedia_Dialog_Progress_Connecting;

		pstContents->bUpperFolder = FALSE;
		pstDlnaDmsData = HLIB_VECTOR_ItemAt(pstContents->pDmsList, lIndex - (pstContents->ulStorageListNum + NX_MEDIA_SERVER_RECENT_COUNT));
		HxSTD_sprintf((char*)pstContents->ucCurrentServerPath, "%s", (char*)pstDlnaDmsData->szUDN);
		NX_DLNA_BrwosingChildItem(pstDlnaDmsData->szUDN, (HCHAR*)"0");
#endif
		return	ONDK_RET_OK;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_ContentListSelect(Nx_MediaContext_t *pstContents, HINT32 lIndex)
{
	DxFileInfo_t		*pstFsInfo = NULL;
	HINT32				lContentIdx = 0;
	HUINT8				ucFilePath[MAX_ELEMENT_TEXT_LENGTH];
	HCHAR				*pszFilter = NULL;
#if defined(CONFIG_MW_MM_DLNA)
	NX_DLNA_CONTENT_t	*pstContentInfo = NULL;
#endif

	/* Set Filter */
	if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypeVideo)
	{
		pszFilter = NXFILE_FILTER_MOVIE;
	}
	else if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypePhoto)
	{
		//TODO: Photo Filter
	}
	else if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypeMusic)
	{
		//TODO: Music Filter
	}

	/* Selection Handling */
	if (pstContents->eCurrentServerType == eNxMedia_ServerType_Storage)
	{
		if (pstContents->browseHandle)
		{
			if (pstContents->bUpperFolder == TRUE && lIndex == 0) // if Go Upper
			{
				if (pstContents->bSelectMode == TRUE && pstContents->pSelectedList != NULL) // if Select Mode
				{
					return	ONDK_RET_OK;
				}
				else
				{
					nx_media_Ui_ProcessingMsgCreate(eNxMedia_Dialog_Progress_Connecting);
					pstContents->ulCurrentDlg = eNxMedia_Dialog_Progress_Connecting;

					// Browse
					NX_FILEMGR_BROWSER_MoveToUp(pstContents->browseHandle);
					NX_FILEMGR_BROWSER_BrowseByFolderView(pstContents->browseHandle, pszFilter);
				}
			}
			else
			{
				lContentIdx = (pstContents->bUpperFolder == TRUE) ? lIndex - 1 : lIndex;
				pstFsInfo = HLIB_VECTOR_ItemAt(pstContents->browseHandle->itemList, lContentIdx);

				if (pstFsInfo)
				{
					if (pstContents->bSelectMode == TRUE && pstContents->pSelectedList != NULL) // if Select Mode
					{
						if (pstContents->pSelectedList[lContentIdx] == TRUE)
						{
							pstContents->pSelectedList[lContentIdx] = FALSE;
						}
						else
						{
							pstContents->pSelectedList[lContentIdx] = TRUE;
						}
						ONDK_GWM_APP_InvalidateObject(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID);
					}
					else
					{
						if (pstFsInfo->eDataType == eDxFILETYPE_DIRECTORY)
						{
							nx_media_Ui_ProcessingMsgCreate(eNxMedia_Dialog_Progress_Connecting);
							pstContents->ulCurrentDlg = eNxMedia_Dialog_Progress_Connecting;

							HxSTD_sprintf((char*)ucFilePath, "%s", (char*)pstFsInfo->szFileName);
							NX_FILEMGR_BROWSER_SetPath(pstContents->browseHandle, (HCHAR*)ucFilePath);
							NX_FILEMGR_BROWSER_BrowseByFolderView(pstContents->browseHandle, pszFilter);
						}
						else // Contents Play
						{
							if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypeVideo)
							{
								ONDK_GWM_APP_HideUI();
								nx_media_MakePlayList(pstContents, lContentIdx);
								NX_PRISM_PLAYMGR_PlayStart(ePlayerType_MEDIAPLAY, pstContents->playList, 0, FALSE);
							}
							else if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypePhoto)
							{
								//TODO: Photo Play
							}
							else if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypeMusic)
							{
								//TODO: Music Play
							}
						}
					}
				}
			}
		}
	}
	else if (pstContents->eCurrentServerType == eNxMedia_ServerType_Dlna)
	{
#if defined(CONFIG_MW_MM_DLNA)
		if (pstContents->pDlnaContentList)
		{
			if (pstContents->bUpperFolder == TRUE && lIndex == 0) // if Go Upper
			{
				nx_media_Ui_ProcessingMsgCreate(eNxMedia_Dialog_Progress_Connecting);
				pstContents->ulCurrentDlg = eNxMedia_Dialog_Progress_Connecting;
				NX_DLNA_BrwosingChildItem(pstContents->ucCurrentServerPath, (HCHAR*)pstContents->szParentCID);
			}
			else
			{
				lContentIdx = (pstContents->bUpperFolder == TRUE) ? lIndex - 1 : lIndex;
				pstContentInfo = HLIB_VECTOR_ItemAt(pstContents->pDlnaContentList, lContentIdx);

				if (pstContentInfo)
				{
					if (HxSTD_StrCmp((char*)pstContentInfo->szObjectType, (char*)"folder") == 0)
					{
						nx_media_Ui_ProcessingMsgCreate(eNxMedia_Dialog_Progress_Connecting);
						pstContents->ulCurrentDlg = eNxMedia_Dialog_Progress_Connecting;
						HxSTD_sprintf((char*)pstContents->szParentCID, "%s", pstContentInfo->szParentId);
						NX_DLNA_BrwosingChildItem(pstContents->ucCurrentServerPath, (HCHAR*)pstContentInfo->szCID);
					}
					else // Contents Play
					{
						if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypeVideo)
						{
							//TODO: Play
							//ONDK_GWM_APP_HideUI();
							//nx_media_MakePlayList(pstContents, lContentIdx);
							//NX_PRISM_PLAYMGR_PlayStart(ePlayerType_MEDIAPLAY, pstContents->playList, 0, FALSE);
						}
						else if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypePhoto)
						{
							//TODO: Photo Play
						}
						else if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypeMusic)
						{
							//TODO: Music Play
						}
					}
				}
			}
		}
#endif
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_SetCurrentContentSelected(Nx_MediaContext_t *pstContents)
{
	HINT32			lIndex = 0, lListLength = 0;
	HINT32			lTotal = 0, lStart = 0, lCurrent = 0, lSelect = 0, lNumOfPage = 0, lContentIdx = 0;
	LIST_OBJECT 	listObj = NULL;

	listObj = (LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID);
	ONDK_GWM_List_GetProperty(listObj, &lTotal, &lStart, &lCurrent, &lSelect, &lNumOfPage);
	lContentIdx = (pstContents->bUpperFolder == TRUE) ? lCurrent - 1 : lCurrent;

	lListLength = HLIB_VECTOR_Length(pstContents->browseHandle->itemList);
	for (lIndex = 0; lIndex < lListLength; lIndex++)
	{
		if (lIndex == lContentIdx)
		{
			pstContents->pSelectedList[lIndex] = TRUE;
		}
		else
		{
			pstContents->pSelectedList[lIndex] = FALSE;
		}
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_RemoveContentGroup(Nx_MediaContext_t *pstContents)
{
	DxFileInfo_t	*pstFsInfo = NULL;
	HINT32			lIndex = 0, lListLength = 0;
	HERROR			hErr = ERR_FAIL;
	HCHAR			*pszFilter = NULL;

	if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypeVideo)
	{
		pszFilter = NXFILE_FILTER_MOVIE;
	}
	else if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypePhoto)
	{
		//TODO: Photo Filter
	}
	else if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypeMusic)
	{
		//TODO: Music Filter
	}

	hErr = NX_FILEMGR_FILE_CreateFileOperation(pszFilter);
	if (hErr == ERR_OK)
	{
		lListLength = HLIB_VECTOR_Length(pstContents->browseHandle->itemList);
		for (lIndex = 0; lIndex < lListLength; lIndex++)
		{
			if (pstContents->pSelectedList[lIndex] == TRUE)
			{
				pstFsInfo = HLIB_VECTOR_ItemAt(pstContents->browseHandle->itemList, lIndex);
				if (pstFsInfo)
				{
					NX_FILEMGR_FILE_AddPath((HCHAR*)pstFsInfo->szFileName);
				}
			}
		}
		NX_FILEMGR_FILE_RemoveFileGroup();
	}
	else
	{
		return ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_MoveContentGroup(Nx_MediaContext_t	*pstContents, HCHAR* pszTargetPath)
{
	DxFileInfo_t	*pstFsInfo = NULL;
	HINT32			lIndex = 0, lListLength = 0;
	HERROR			hErr = ERR_FAIL;
	HCHAR			*pszFilter = NULL;

	if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypeVideo)
	{
		pszFilter = NXFILE_FILTER_MOVIE;
	}
	else if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypePhoto)
	{
		//TODO: Photo Filter
	}
	else if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypeMusic)
	{
		//TODO: Music Filter
	}

	hErr = NX_FILEMGR_FILE_CreateFileOperation(pszFilter);
	if (hErr == ERR_OK)
	{
		lListLength = HLIB_VECTOR_Length(pstContents->browseHandle->itemList);
		for (lIndex = 0; lIndex < lListLength; lIndex++)
		{
			if (pstContents->pSelectedList[lIndex] == TRUE)
			{
				pstFsInfo = HLIB_VECTOR_ItemAt(pstContents->browseHandle->itemList, lIndex);
				if (pstFsInfo)
				{
					NX_FILEMGR_FILE_AddPath((HCHAR*)pstFsInfo->szFileName);
				}
			}
		}
		NX_FILEMGR_FILE_MoveFileGroup(pszTargetPath);
	}
	else
	{
		return ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_CopyContentGroup(Nx_MediaContext_t	*pstContents, HCHAR* pszTargetPath)
{
	DxFileInfo_t	*pstFsInfo = NULL;
	HINT32			lIndex = 0, lListLength = 0;
	HERROR			hErr = ERR_FAIL;
	HCHAR			*pszFilter = NULL;

	if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypeVideo)
	{
		pszFilter = NXFILE_FILTER_MOVIE;
	}
	else if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypePhoto)
	{
		//TODO: Photo Filter
	}
	else if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypeMusic)
	{
		//TODO: Music Filter
	}

	hErr = NX_FILEMGR_FILE_CreateFileOperation(pszFilter);
	if (hErr == ERR_OK)
	{
		lListLength = HLIB_VECTOR_Length(pstContents->browseHandle->itemList);
		for (lIndex = 0; lIndex < lListLength; lIndex++)
		{
			if (pstContents->pSelectedList[lIndex] == TRUE)
			{
				pstFsInfo = HLIB_VECTOR_ItemAt(pstContents->browseHandle->itemList, lIndex);
				if (pstFsInfo)
				{
					NX_FILEMGR_FILE_AddPath((HCHAR*)pstFsInfo->szFileName);
				}
			}
		}
		NX_FILEMGR_FILE_CopyFileGroup(pszTargetPath);
	}
	else
	{
		return ONDK_RET_FAIL;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_RemoveCurrentFocusedServer(Nx_MediaContext_t *pstContents)
{
	HINT32		lIndex = 0;
	HINT32		lStorageCount = pstContents->ulStorageListNum;

	lIndex = ONDK_GWM_List_GetCurrentIndex(NX_PRISM_UI_MEDIA_SERVER_LIST_ID);
	if (lIndex == 0) //recent
	{
		return ONDK_RET_FAIL;
	}
	else if ((lIndex > 0) && (lIndex < lStorageCount + NX_MEDIA_SERVER_RECENT_COUNT)) //storage
	{
		NX_FILEMGR_STORAGE_SafeRemove(pstContents->pStorageDevIdList[lIndex - NX_MEDIA_SERVER_RECENT_COUNT]);
	}
	else //dlna
	{
		return ONDK_RET_FAIL;
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_RemoveFocusedServerAllContent(Nx_MediaContext_t *pstContents)
{
	HERROR		hError;
	HINT32		lIndex = 0;
	HINT32		lStorageCount = pstContents->ulStorageListNum;
	HCHAR		szPath[DLIB_FILEINFO_NAME_LEN];
	NX_PhysicalStorageData_t	*physicalStorageData = NULL;

	lIndex = ONDK_GWM_List_GetCurrentIndex(NX_PRISM_UI_MEDIA_SERVER_LIST_ID);
	if (lIndex == 0) //recent
	{
		return ONDK_RET_FAIL;
	}
	else if ((lIndex > 0) && (lIndex < lStorageCount + NX_MEDIA_SERVER_RECENT_COUNT)) //storage
	{
		hError = NX_FILEMGR_STORAGE_GetPhysicalStorageInfoWithDevId(pstContents->pStorageDevIdList[lIndex - NX_MEDIA_SERVER_RECENT_COUNT], &physicalStorageData);
		if (hError != ERR_OK || physicalStorageData == NULL)
		{
			NX_APP_Error("Error!!! GetPhysicalStorageInfoWithDevId FAIL !!\n");
			return ONDK_RET_FAIL;
		}

		hError = NX_FILEMGR_FILE_CreateFileOperation(NXFILE_FILTER_MOVIE); //TODO : it should be All Filter type
		if (hError == ERR_OK)
		{
			HxSTD_sprintf((char*)szPath, "%s/*", (char*)physicalStorageData->pszMountpath);
			NX_FILEMGR_FILE_AddPath((HCHAR*)szPath);
			NX_FILEMGR_FILE_RemoveFileGroup();
		}
		else
		{
			return ONDK_RET_FAIL;
		}
	}
	else //dlna
	{
		return ONDK_RET_FAIL;
	}

	return ONDK_RET_OK;
}

#define ___________MESSAGE_PROCEDURE___________

static ONDK_Result_t	nx_media_MsgGwmCreate(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	if (pstContents == NULL)
	{
		NX_APP_Error("pstContents is NULL!\n");
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_FAIL;
	}

	pstContents->ulContentsType = (HUINT32)p1;

	switch(p1)
	{
		case eNxUtil_OptItem_MediaTypeVideo:
			pszPanelStr = NX_PRISM_SPEC_GetPanelString(eNX_PANEL_STR_Video);
			pstContents->lShowItemCnt = NX_MEDIA_MAX_VIDEO_SHOW_ITEM_COUNT;
			break;
		case eNxUtil_OptItem_MediaTypePhoto:
			pszPanelStr = NX_PRISM_SPEC_GetPanelString(eNX_PANEL_STR_Photo);
			pstContents->lShowItemCnt = NX_MEDIA_MAX_PHOTO_SHOW_ITEM_COUNT;
			break;
		case eNxUtil_OptItem_MediaTypeMusic:
			pszPanelStr = NX_PRISM_SPEC_GetPanelString(eNX_PANEL_STR_Music);
			pstContents->lShowItemCnt = NX_MEDIA_MAX_VIDEO_SHOW_ITEM_COUNT;
			break;
	}
	pstContents->thumbRequest = NX_APP_Malloc(sizeof(Nx_MediaThumbRequest_t) * pstContents->lShowItemCnt);


	nx_media_LoadStorageDevIdList(pstContents);
	pstContents->ulDmsListNum = 0;
	nx_media_DlnaDmsStartScan();
	nx_media_DrawDefaultUI(pstContents);

	ONDK_GWM_SetTimer(NX_MEDIA_TIMEOUT_ID, NX_MEDIA_TIMEOUT);
	ONDK_GWM_SetTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID, NX_SETTING_TIME_UPDATE_TIME);
	ONDK_GWM_SetTimer(NX_PRISM_UI_MEDIA_THUMB_TIMER_ID, NX_MEDIA_TIME_THUMB_UPDATE);

	if (pszPanelStr == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_SPEC_GetPanelString() - eNX_PANEL_STR_Video !!!\n");
	}
	NX_PANEL_SetString((HCHAR*)pszPanelStr, eDxPANEL_ALIGN_CENTER, FALSE);

	pstContents->ulCurrentDlg = eNxMedia_Dialog_None;

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_media_MsgGwmDestroy(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_PANEL_ClearString();
	nx_media_DlnaDmsStopScan(pstContents);

	ONDK_GWM_KillTimer(NX_MEDIA_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_MEDIA_THUMB_TIMER_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_DLG_PROCESS_ANIMATION_TIMER_ID);

	if (pstContents->browseHandle)
	{
		NX_FILEMGR_BROWSER_DestroyHandle(pstContents->browseHandle);
		pstContents->browseHandle = NULL;
	}
	nx_media_FreeDeviceIdList(pstContents->pStorageDevIdList);
	nx_media_FreeSelectedList(pstContents->pSelectedList);
	nx_media_FreeOptionTextList(pstContents);
	if (pstContents->playList)
	{
		HLIB_VECTOR_Delete(pstContents->playList);
		pstContents->playList = NULL;
	}

	if (pstContents->thumbRequest)
	{
		NX_APP_Free(pstContents->thumbRequest);
		pstContents->thumbRequest = NULL;
	}

	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MediaContext_t));

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_media_MsgGwmHapiMsg(Nx_MediaContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}

static ONDK_Result_t	nx_media_MsgGwmTimer(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HCHAR		*pszResource = NULL;

	if (p1 == NX_PRISM_UI_MEDIA_THUMB_TIMER_ID)
	{
		if (pstContents->ulCurrentContentLength > 0)
		{
			nx_media_UpdateThumbnail(pstContents);
		}
	}
	else if (p1 == NX_PRISM_UI_DLG_PROCESS_ANIMATION_TIMER_ID)
	{
		pstContents->ulAniIconIdx++;
		if (pstContents->ulAniIconIdx > 3)
		{
			pstContents->ulAniIconIdx = 0;
		}
		switch (pstContents->ulAniIconIdx)
		{
			case 0:		pszResource = RES_206_00_LOADING_01_PNG;	break;
			case 1:		pszResource = RES_206_00_LOADING_02_PNG;	break;
			case 2:		pszResource = RES_206_00_LOADING_03_PNG;	break;
			case 3:		pszResource = RES_206_00_LOADING_04_PNG;	break;
			default:	pszResource = NULL;							break;
		}
		ONDK_GWM_COM_Image_SetImage(NX_PRISM_UI_MEDIA_OPERATING_IMG_ID, (HCHAR*)pszResource);
		ONDK_GWM_APP_InvalidateObject(NX_PRISM_UI_MEDIA_OPERATING_IMG_ID);
	}
	else if (p1 == NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID)
	{
		nx_media_UpdateCurrentTime(pstContents);
		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_MsgGwmKeyDown(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32					lKeyId = p1, lIndex = 0;
	HBOOL					bSelect = FALSE;
	ONDK_GWM_GuiObject_t	object = NULL;
	HCHAR					*pszFilter = NULL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	GWM_RET_SOURCE_NULL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
			object = ONDK_GWM_Obj_GetFocusedObject();
			switch (ONDK_GWM_Obj_GetObjectID(object))
			{
				case NX_PRISM_UI_MEDIA_CONTENT_LIST_ID:
					if (pstContents->bSelectMode == TRUE)
					{
						pstContents->bSelectMode = FALSE;
						nx_media_MakeSelectModeDim(FALSE);
						ONDK_GWM_APP_InvalidateObject(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID);
					}
					else
					{
						if (pstContents->bUpperFolder == TRUE)
						{
							nx_media_Ui_ProcessingMsgCreate(eNxMedia_Dialog_Progress_Connecting);
							pstContents->ulCurrentDlg = eNxMedia_Dialog_Progress_Connecting;

							/* Set Filter */
							if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypeVideo)
							{
								pszFilter = NXFILE_FILTER_MOVIE;
							}
							else if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypePhoto)
							{
								pszFilter = NXFILE_FILTER_PHOTO;
								//TODO: Photo Filter
							}
							else if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypeMusic)
							{
								//TODO: Music Filter
							}

							// Browse
							NX_FILEMGR_BROWSER_MoveToUp(pstContents->browseHandle);
							NX_FILEMGR_BROWSER_BrowseByFolderView(pstContents->browseHandle, pszFilter);
						}
						else
						{
							ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_MEDIA_SERVER_LIST_ID);
							nx_media_ShowOptList(pstContents, FALSE, FALSE);
						}
					}
					break;

				default:
					ONDK_GWM_APP_Destroy(0);
					break;
			}

			return	ONDK_MESSAGE_BREAK;

		case KEY_EXIT:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;
		case KEY_ARROWLEFT:
		case KEY_ARROWRIGHT:
		case KEY_OK:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;

		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
		case KEY_CH_PLUS:
		case KEY_CH_MINUS:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		case KEY_OPT:
			if (pstContents->bSelectMode == TRUE)
			{
				if (pstContents->pSelectedList != NULL)
				{
					for (lIndex = 0; lIndex < pstContents->ulCurrentContentLength; lIndex++)
					{
						if (pstContents->pSelectedList[lIndex] == TRUE)
						{
							bSelect = TRUE;
						}
					}
				}

				if (bSelect == FALSE) // Notting Selected in SelectMode
				{
					nx_media_Ui_ResponseMsgCreate((HCHAR*)ONDK_GetString(RES_MESG_4562_ID));
				}
				else
				{
					object = ONDK_GWM_Obj_GetFocusedObject();
					nx_media_DrawOptListPopup(pstContents, ONDK_GWM_Obj_GetObjectID(object));
				}
			}
			else
			{
				object = ONDK_GWM_Obj_GetFocusedObject();
				nx_media_DrawOptListPopup(pstContents, ONDK_GWM_Obj_GetObjectID(object));
			}
			return	ONDK_MESSAGE_BREAK;

		default:
			NX_APP_Print("nx_media_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_MsgGwmListSelected(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lListId = p1;
	HINT32			lListIdx = p2;

	if (lListId == NX_PRISM_UI_MEDIA_SERVER_LIST_ID)
	{
		nx_media_ServerListSelect(pstContents, lListIdx);
	}
	else if (lListId == NX_PRISM_UI_MEDIA_CONTENT_LIST_ID)
	{
		nx_media_ContentListSelect(pstContents, lListIdx);
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2,p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_MsgGwmListChanged(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lListId = p1;
	HINT32			lListIdx = p2;
	HBOOL			bEjectableServer = FALSE;

	if (lListId == NX_PRISM_UI_MEDIA_SERVER_LIST_ID)
	{
		bEjectableServer = nx_media_CheckStorageEjectableWithIdx(pstContents, lListIdx);
		if (bEjectableServer == TRUE)
		{
			nx_media_ShowOptList(pstContents, TRUE, FALSE);
		}
		else
		{
			nx_media_ShowOptList(pstContents, FALSE, FALSE);
		}
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2,p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_MsgGwmObjFocusChanged(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32	lObjId = p1;
	HINT32	lListLength = 0;
	HBOOL	bEjectableServer = FALSE;

	if (lObjId == NX_PRISM_UI_MEDIA_CONTENT_LIST_ID)
	{
		if (ONDK_GWM_Obj_GetHide(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID) == TRUE)
		{
			ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_MEDIA_SERVER_LIST_ID);
			return ONDK_RET_OK;
		}

		if (pstContents->eCurrentServerType == eNxMedia_ServerType_Storage)
		{
			if (pstContents->browseHandle)
			{
				if (pstContents->browseHandle->itemList)
				{
					lListLength = HLIB_VECTOR_Length(pstContents->browseHandle->itemList);
					if (lListLength > 0)
					{
						nx_media_ShowOptList(pstContents, TRUE, FALSE);
						return	ONDK_RET_OK;
					}
				}
			}
		}
	}
	else if (lObjId == NX_PRISM_UI_MEDIA_SERVER_LIST_ID)
	{
		if (pstContents->bSelectMode == TRUE)
		{
			ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID);
			return ONDK_RET_OK;
		}

		bEjectableServer = nx_media_CheckStorageEjectableWithIdx(pstContents, ONDK_GWM_List_GetCurrentIndex(NX_PRISM_UI_MEDIA_SERVER_LIST_ID));
		if (bEjectableServer == TRUE)
		{
			nx_media_ShowOptList(pstContents, TRUE, FALSE);
			return	ONDK_RET_OK;
		}
	}
	nx_media_ShowOptList(pstContents, FALSE, FALSE);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_MsgCoreMediaBrowseDone(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	Nxfile_Ticket_t		ticket = p2;
	HINT32				lListLength = p3;
	HBOOL				bServerPath = FALSE;

	nx_media_HideThumbnail(pstContents);

	if (pstContents->ulCurrentDlg == eNxMedia_Dialog_Progress_Connecting)
	{
		nx_media_ProcessingMsgDestroy();
		pstContents->ulCurrentDlg = eNxMedia_Dialog_None;
	}

	if (pstContents->browseHandle == NULL)
	{
		ONDK_GWM_Obj_SetHide(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID, TRUE);
		nx_media_ShowNoItemInfo(pstContents, TRUE, eNxMedia_ServerType_Storage);
		ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_MEDIA_SERVER_LIST_ID);
		NX_APP_Error("Error!!! pstContents->browseHandle is NULL !!\n");
		return ONDK_RET_INITFAIL;
	}

	if (HxSTD_StrCmp((char*)pstContents->ucCurrentServerPath, (char*)pstContents->browseHandle->szCurPath) == 0)
	{
		bServerPath = TRUE;
	}
	else
	{
		bServerPath = FALSE;
	}

	if (pstContents->browseHandle->ticket == ticket)
	{
		if (pstContents->browseHandle->itemList)
		{
			if (bServerPath == TRUE)
			{
				pstContents->bUpperFolder = FALSE;
				if (lListLength > 0)
				{
					nx_media_ShowNoItemInfo(pstContents, FALSE, eNxMedia_ServerType_Storage);
				}
				else
				{
					ONDK_GWM_Obj_SetHide(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID, TRUE);
					nx_media_ShowNoItemInfo(pstContents, TRUE, eNxMedia_ServerType_Storage);
					ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_MEDIA_SERVER_LIST_ID);
					return ONDK_RET_OK;
				}
			}
			else
			{
				pstContents->bUpperFolder = TRUE;
			}

			nx_media_ShowCurrentPath(pstContents);
			if (lListLength > 0)
			{
				nx_media_FreeSelectedList(pstContents->pSelectedList);
				pstContents->pSelectedList = (HBOOL *)NX_APP_Malloc (sizeof(HBOOL) * lListLength);
				if (pstContents->pSelectedList == NULL)
				{
					NX_APP_Error("Error!!! pstContents->pSelectedList is NULL !!\n");
					return	ONDK_RET_INITFAIL;
				}
				HxSTD_MemSet(pstContents->pSelectedList, 0, sizeof(HBOOL) * lListLength);
			}
			pstContents->bSelectMode = FALSE;
			pstContents->ulCurrentContentLength = lListLength;
			nx_media_MakeSelectModeDim(FALSE);
			nx_media_DrawContentList(pstContents);
			return ONDK_RET_OK;
		}
	}
	ONDK_GWM_Obj_SetHide(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID, TRUE);
	nx_media_ShowNoItemInfo(pstContents, TRUE, eNxMedia_ServerType_Storage);
	ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_MEDIA_SERVER_LIST_ID);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_media_MsgCoreDeviceChange(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32			ulListIdx = 0;
	HINT32			lServerListCount = 0;

	if (p2 == eNxStorageState_Detach)
	{
		for (ulListIdx = 0; ulListIdx < pstContents->ulStorageListNum; ulListIdx++)
		{
			if (pstContents->pStorageDevIdList[ulListIdx] == (HUINT32)p1)
			{
				nx_media_LoadStorageDevIdList(pstContents);
				lServerListCount = pstContents->ulStorageListNum + pstContents->ulDmsListNum + 1;
				ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MEDIA_SERVER_LIST_ID), lServerListCount, 0);
				ONDK_GWM_APP_InvalidateObject(NX_PRISM_UI_MEDIA_SERVER_LIST_ID);

				// If Current Browsed Contents Server Removed
				if ((HUINT32)p1 == pstContents->ulCurrentServerId)
				{
					pstContents->bSelectMode = FALSE;
					pstContents->ulCurrentContentLength = 0;
					pstContents->szCurPath[0] = '\0';
					pstContents->ucCurrentServerPath[0] = '\0';
					pstContents->bUpperFolder = FALSE;
					ONDK_GWM_Obj_SetHide(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID, TRUE);
					nx_media_ShowOptList(pstContents, FALSE, FALSE);
					nx_media_MakeSelectModeDim(FALSE);
					nx_media_HideThumbnail(pstContents);
					nx_media_ShowNoItemInfo(pstContents, TRUE, eNxMedia_ServerType_Storage);
					ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_MEDIA_SERVER_LIST_ID);
				}
				break;
			}
		}
	}
	else if (p2 == eNxStorageState_Attach)
	{
		nx_media_LoadStorageDevIdList(pstContents);
		lServerListCount = pstContents->ulStorageListNum + pstContents->ulDmsListNum + 1;
		ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MEDIA_SERVER_LIST_ID), lServerListCount, 0);
		ONDK_GWM_APP_InvalidateObject(NX_PRISM_UI_MEDIA_SERVER_LIST_ID);
	}
	else if (p3 == eNxStorageState_Update)
	{
		ONDK_GWM_APP_InvalidateObject(NX_PRISM_UI_MEDIA_SERVER_LIST_ID);
	}

	NX_COMMON_NOT_USED_PARAM(p3);

	return ONDK_RET_OK;
}

static	ONDK_Result_t	nx_media_MsgCoreOperationDone(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 lMessage)
{
	HCHAR			*pszFilter = NULL;

	if (pstContents->ulCurrentDlg == eNxMedia_Dialog_Progress_Copying ||
		pstContents->ulCurrentDlg == eNxMedia_Dialog_Progress_Deleting ||
		pstContents->ulCurrentDlg == eNxMedia_Dialog_Progress_Moving)
	{
		nx_media_ShowOperatingDlg(pstContents, FALSE);

		// re browse
		nx_media_Ui_ProcessingMsgCreate(eNxMedia_Dialog_Progress_Connecting);
		pstContents->ulCurrentDlg = eNxMedia_Dialog_Progress_Connecting;

		if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypeVideo)
		{
			pszFilter = NXFILE_FILTER_MOVIE;
		}
		else if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypePhoto)
		{
			//TODO: Photo Filter
		}
		else if (pstContents->ulContentsType == (HUINT32)eNxUtil_OptItem_MediaTypeMusic)
		{
			//TODO: Music Filter
		}
		NX_FILEMGR_BROWSER_BrowseByFolderView(pstContents->browseHandle, pszFilter);
	}

	return ONDK_RET_OK;
}

static	ONDK_Result_t	nx_media_MsgCoreDlnaDmsChange(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 lMessage)
{
#if defined(CONFIG_MW_MM_DLNA)
	HINT32	lServerListCount = 0;

	if (eNxDlna_Dms_Add == (NX_DLNA_DMS_CHANGE_e)p1)
	{
		pstContents->pDmsList = NX_DLNA_GetDmsList();
		pstContents->ulDmsListNum = HLIB_VECTOR_Length(pstContents->pDmsList);
		lServerListCount = pstContents->ulStorageListNum + pstContents->ulDmsListNum + 1;
		ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MEDIA_SERVER_LIST_ID), lServerListCount, 0);
		ONDK_GWM_APP_InvalidateObject(NX_PRISM_UI_MEDIA_SERVER_LIST_ID);
	}
	else if (eNxDlna_Dms_Remove == (NX_DLNA_DMS_CHANGE_e)p1)
	{
		//TODO : Have to Check Current Server
		pstContents->pDmsList = NX_DLNA_GetDmsList();
		pstContents->ulDmsListNum = HLIB_VECTOR_Length(pstContents->pDmsList);
		lServerListCount = pstContents->ulStorageListNum + pstContents->ulDmsListNum + 1;
		ONDK_GWM_List_ChangeItems((LIST_OBJECT)ONDK_GWM_APP_GetObject(NX_PRISM_UI_MEDIA_SERVER_LIST_ID), lServerListCount, 0);
		ONDK_GWM_APP_InvalidateObject(NX_PRISM_UI_MEDIA_SERVER_LIST_ID);
	}
#endif

	return ONDK_RET_OK;
}

static	ONDK_Result_t	nx_media_MsgCoreDlnaContentsBrowseDone(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if defined(CONFIG_MW_MM_DLNA)
	HINT32				lListLength = p3;

	pstContents->pDlnaContentList = (HxVector_t*)p2;

	nx_media_HideThumbnail(pstContents);

	if (pstContents->ulCurrentDlg == eNxMedia_Dialog_Progress_Connecting)
	{
		nx_media_ProcessingMsgDestroy();
		pstContents->ulCurrentDlg = eNxMedia_Dialog_None;
	}

	if (pstContents->pDlnaContentList == NULL)
	{
		ONDK_GWM_Obj_SetHide(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID, TRUE);
		nx_media_ShowNoItemInfo(pstContents, TRUE, eNxMedia_ServerType_Dlna);
		ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_MEDIA_SERVER_LIST_ID);
		NX_APP_Error("Error!!! pstContents->pDlnaContentList is NULL !!\n");
		return ONDK_RET_INITFAIL;
	}

	if (pstContents->pDlnaContentList)
	{
		if (lListLength > 0)
		{
			nx_media_ShowNoItemInfo(pstContents, FALSE, eNxMedia_ServerType_Dlna);
		}
		else
		{
			ONDK_GWM_Obj_SetHide(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID, TRUE);
			nx_media_ShowNoItemInfo(pstContents, TRUE, eNxMedia_ServerType_Dlna);
			ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_MEDIA_SERVER_LIST_ID);
			return ONDK_RET_OK;
		}

		if (HxSTD_StrCmp((char*)NX_DLNA_GetCurrentCID(), (char*)"0") == 0)
		{
			pstContents->bUpperFolder = FALSE;
		}
		else
		{
			pstContents->bUpperFolder = TRUE;
		}
		nx_media_ShowCurrentPath(pstContents);
		pstContents->ulCurrentContentLength = lListLength;
		nx_media_DrawContentList(pstContents);
		return ONDK_RET_OK;
	}
	ONDK_GWM_Obj_SetHide(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID, TRUE);
	nx_media_ShowNoItemInfo(pstContents, TRUE, eNxMedia_ServerType_Dlna);
	ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_MEDIA_SERVER_LIST_ID);
#endif

	return ONDK_RET_OK;
}

static	ONDK_Result_t	nx_media_MsgAppPlayerStop(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_APP_ShowUI();

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return ONDK_RET_OK;
}

static	ONDK_Result_t	nx_media_MsgAppDlgClicked(Nx_MediaContext_t *pstContents, HINT32 lClickedBtn)
{
	nx_media_SelectiveMsgDestroy();

	switch(lClickedBtn)
	{
		case eNX_DLG_CLICKED_BTN_LEFT: // YES
			if (pstContents->ulCurrentDlg == eNxMedia_Dialog_Select_DeleteOneFile || pstContents->ulCurrentDlg == eNxMedia_Dialog_Select_DeleteMultiFile)
			{
				nx_media_RemoveContentGroup(pstContents);
				nx_media_ShowOperatingDlg(pstContents, TRUE);
			}
			else if (pstContents->ulCurrentDlg == eNxMedia_Dialog_Select_DeleteAllFiles)
			{
				nx_media_RemoveFocusedServerAllContent(pstContents);
				nx_media_ShowOperatingDlg(pstContents, TRUE);
			}
			else
			{
				pstContents->ulCurrentDlg = eNxMedia_Dialog_None;
				break;
			}
			break;
		default:
			pstContents->ulCurrentDlg = eNxMedia_Dialog_None;
			break;
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return ONDK_MESSAGE_BREAK;
}

static	ONDK_Result_t	nx_media_MsgAppTargetStorageSelected(Nx_MediaContext_t *pstContents, HINT32 lSelectedDevId)
{
	HERROR		hError = ERR_FAIL;
	NX_PhysicalStorageData_t	*physicalStorageInfo = NULL;

	hError = NX_FILEMGR_STORAGE_GetPhysicalStorageInfoWithDevId((HUINT32)lSelectedDevId, &physicalStorageInfo);
	if (hError != ERR_OK || physicalStorageInfo == NULL)
	{
		NX_APP_Error("Error!!! GetPhysicalStorageInfoWithDevId FAIL !!\n");
		return ONDK_RET_INITFAIL;
	}

	if (pstContents->ulCurrentDlg == eNxMedia_Dialog_Select_CopyTarget)
	{
		nx_media_CopyContentGroup(pstContents, physicalStorageInfo->pszMountpath);
		nx_media_ShowOperatingDlg(pstContents, TRUE);
	}
	else if (pstContents->ulCurrentDlg == eNxMedia_Dialog_Select_MoveTarget)
	{
		nx_media_MoveContentGroup(pstContents, physicalStorageInfo->pszMountpath);
		nx_media_ShowOperatingDlg(pstContents, TRUE);
	}

	return ONDK_MESSAGE_BREAK;
}

static	ONDK_Result_t	nx_media_MsgAppOptListSelected(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lItemIdx = p1;
	HINT32			lIndex = 0, lListLength = 0, lSelectedCount = 0;
	eNxUiUtil_OptItem_t		eOptItem = pstContents->stOptList.aeOptItemList[lItemIdx];

	if (eOptItem != eNxUtil_OptItem_Rename && eOptItem != eNxUtil_OptItem_Eject && eOptItem != eNxUtil_OptItem_DeleteAll)
	{
		if (pstContents->browseHandle == NULL || pstContents->browseHandle->itemList == NULL || pstContents->pSelectedList == NULL)
		{
			NX_APP_Error("Error!!! pstContents Data is NULL !!\n");
			return ONDK_RET_INITFAIL;
		}
	}

	switch (eOptItem)
	{
		case eNxUtil_OptItem_Delete:
			lListLength = HLIB_VECTOR_Length(pstContents->browseHandle->itemList);
			for (lIndex = 0; lIndex < lListLength; lIndex++)
			{
				if (pstContents->pSelectedList[lIndex] == TRUE)
				{
					lSelectedCount++;
				}
			}

			if (lSelectedCount == 1)
			{
				nx_media_Ui_SelectiveMsgCreate(eNxMedia_Dialog_Select_DeleteOneFile);
				pstContents->ulCurrentDlg = eNxMedia_Dialog_Select_DeleteOneFile;
			}
			else if (lSelectedCount > 1)
			{
				nx_media_Ui_SelectiveMsgCreate(eNxMedia_Dialog_Select_DeleteMultiFile);
				pstContents->ulCurrentDlg = eNxMedia_Dialog_Select_DeleteMultiFile;
			}
			else
			{
				if (pstContents->bSelectMode == TRUE)
				{
					NX_APP_Error("Error!!! lSelectedCount must at least be one !!\n");
					return ONDK_RET_FAIL;
				}
				else
				{
					nx_media_SetCurrentContentSelected(pstContents);
					nx_media_Ui_SelectiveMsgCreate(eNxMedia_Dialog_Select_DeleteOneFile);
					pstContents->ulCurrentDlg = eNxMedia_Dialog_Select_DeleteOneFile;
				}
			}
			break;

		case eNxUtil_OptItem_Copy:
			lListLength = HLIB_VECTOR_Length(pstContents->browseHandle->itemList);
			for (lIndex = 0; lIndex < lListLength; lIndex++)
			{
				if (pstContents->pSelectedList[lIndex] == TRUE)
				{
					lSelectedCount++;
				}
			}

			if (lSelectedCount > 0)
			{
				if (pstContents->ulStorageListNum > 1)
				{
					pstContents->ulCurrentDlg = eNxMedia_Dialog_Select_CopyTarget;
					ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MEDIA_TypeSelect_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MEDIA), (ONDK_GWM_Callback_t)NX_PRISM_MEDIA_TypeSelect_Proc,
										 0, 0, (HINT32)pstContents->ulCurrentServerId, (HINT32)TRUE);
				}
				else
				{
					nx_media_Ui_ResponseMsgCreate((HCHAR*)ONDK_GetString(RES_MESG_3436_ID));
				}
			}
			else
			{
				if (pstContents->bSelectMode == TRUE)
				{
					NX_APP_Error("Error!!! lSelectedCount must at least be one !!\n");
					return ONDK_RET_FAIL;
				}
				else
				{
					if (pstContents->ulStorageListNum > 1)
					{
						nx_media_SetCurrentContentSelected(pstContents);
						pstContents->ulCurrentDlg = eNxMedia_Dialog_Select_CopyTarget;
						ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MEDIA_TypeSelect_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MEDIA), (ONDK_GWM_Callback_t)NX_PRISM_MEDIA_TypeSelect_Proc,
										 0, 0, (HINT32)pstContents->ulCurrentServerId, (HINT32)TRUE);
					}
					else
					{
						nx_media_Ui_ResponseMsgCreate((HCHAR*)ONDK_GetString(RES_MESG_3436_ID));
					}
				}
			}
			break;

		case eNxUtil_OptItem_Move:
			lListLength = HLIB_VECTOR_Length(pstContents->browseHandle->itemList);
			for (lIndex = 0; lIndex < lListLength; lIndex++)
			{
				if (pstContents->pSelectedList[lIndex] == TRUE)
				{
					lSelectedCount++;
				}
			}

			if (lSelectedCount > 0)
			{
				if (pstContents->ulStorageListNum > 1)
				{
					pstContents->ulCurrentDlg = eNxMedia_Dialog_Select_MoveTarget;
					ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MEDIA_TypeSelect_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MEDIA), (ONDK_GWM_Callback_t)NX_PRISM_MEDIA_TypeSelect_Proc,
										 0, 0, (HINT32)pstContents->ulCurrentServerId, (HINT32)TRUE);
				}
				else
				{
					nx_media_Ui_ResponseMsgCreate((HCHAR*)ONDK_GetString(RES_MESG_3437_ID));
				}
			}
			else
			{
				if (pstContents->bSelectMode == TRUE)
				{
					NX_APP_Error("Error!!! lSelectedCount must at least be one !!\n");
					return ONDK_RET_FAIL;
				}
				else
				{
					if (pstContents->ulStorageListNum > 1)
					{
						nx_media_SetCurrentContentSelected(pstContents);
						pstContents->ulCurrentDlg = eNxMedia_Dialog_Select_MoveTarget;
						ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MEDIA_TypeSelect_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MEDIA), (ONDK_GWM_Callback_t)NX_PRISM_MEDIA_TypeSelect_Proc,
										 0, 0, (HINT32)pstContents->ulCurrentServerId, (HINT32)TRUE);
					}
					else
					{
						nx_media_Ui_ResponseMsgCreate((HCHAR*)ONDK_GetString(RES_MESG_3437_ID));
					}
				}
			}
			break;

		case eNxUtil_OptItem_Select:
			pstContents->bSelectMode = TRUE;
			nx_media_SetCurrentContentSelected(pstContents);
			nx_media_MakeSelectModeDim(TRUE);
			nx_media_ShowOptList(pstContents, TRUE, TRUE);
			ONDK_GWM_APP_InvalidateObject(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID);
			break;

		case eNxUtil_OptItem_SelectAll:
			pstContents->bSelectMode = TRUE;
			lListLength = HLIB_VECTOR_Length(pstContents->browseHandle->itemList);
			for (lIndex = 0; lIndex < lListLength; lIndex++)
			{
				pstContents->pSelectedList[lIndex] = TRUE;
			}
			nx_media_MakeSelectModeDim(TRUE);
			nx_media_ShowOptList(pstContents, TRUE, TRUE);
			ONDK_GWM_APP_InvalidateObject(NX_PRISM_UI_MEDIA_CONTENT_LIST_ID);
			break;

		case eNxUtil_OptItem_StopCopying:
		case eNxUtil_OptItem_StopMoving:
			NX_FILEMGR_FILE_CancelOperation();
			nx_media_ShowOperatingDlg(pstContents, FALSE);
			break;

		case eNxUtil_OptItem_Rename:
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_KeyBoard_Proc", APP_DEFAULT_PRIORITY, NX_PRISM_KeyBoard_Proc, HANDLE_NULL, 0, 0 /*KEYBOARD_INPUT_NORMAL*/, 0);
			break;

		case eNxUtil_OptItem_Eject:
			nx_media_RemoveCurrentFocusedServer(pstContents);
			break;

		case eNxUtil_OptItem_DeleteAll:
			nx_media_Ui_SelectiveMsgCreate(eNxMedia_Dialog_Select_DeleteAllFiles);
			pstContents->ulCurrentDlg = eNxMedia_Dialog_Select_DeleteAllFiles;
			break;

		default:
			NX_APP_Error("Not Handle this Option !!\n");
			break;
	}

	return	ONDK_RET_OK;
}

static	ONDK_Result_t	nx_media_MsgAppKeyboardResult(Nx_MediaContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HCHAR   	*pName = NULL;
	HINT32		lIndex = 0;
	HINT32		lStorageCount = pstContents->ulStorageListNum;

	pName = NX_PRISM_KeyBoard_GetResultString();

	lIndex = ONDK_GWM_List_GetCurrentIndex(NX_PRISM_UI_MEDIA_SERVER_LIST_ID);
	if ((lIndex > 0) && (lIndex < lStorageCount + NX_MEDIA_SERVER_RECENT_COUNT)) //storage
	{
		NX_FILEMGR_STORAGE_SetStorageName(pstContents->pStorageDevIdList[lIndex - NX_MEDIA_SERVER_RECENT_COUNT], pName);
	}

	ONDK_GWM_SendMessage(NULL, MSG_APP_KEYBOARD_DESTROY_KEYBOARD, (Handle_t)NULL, 0, 0, 0);
	NX_APP_Free(pName);

	return ONDK_RET_OK;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MEDIA_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_MediaContext_t	*pstContents = NULL;

	pstContents = nx_media_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_media_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_media_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_LIST_SELECTED:
			NX_APP_Message("MSG_GWM_LIST_SELECTED\n");
			eResult = nx_media_MsgGwmListSelected(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_LIST_CHANGED:
			NX_APP_Message("MSG_GWM_LIST_CHANGED\n");
			eResult = nx_media_MsgGwmListChanged(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			eResult = nx_media_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_media_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			eResult = nx_media_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_OBJ_FOCUS_CHANGED:
			eResult = nx_media_MsgGwmObjFocusChanged(pstContents, p1, p2, p3);
			break;

		case MSG_CORE_MEDIA_BROWSE_DONE:
			eResult = nx_media_MsgCoreMediaBrowseDone(pstContents, p1, p2, p3);
			break;

		case MSG_CORE_STORAGE_DEVICE_CHANGE:
			eResult = nx_media_MsgCoreDeviceChange(pstContents, p1, p2, p3);
			break;

		case MSG_CORE_MEDIA_OPERATION_DONE:
		case MSG_CORE_MEDIA_OPERATION_ERROR:
			eResult = nx_media_MsgCoreOperationDone(pstContents, p1, p2, lMessage);
			break;

#if defined(CONFIG_MW_MM_DLNA)
		case MSG_CORE_DLNA_DMS_CHANGE:
			eResult = nx_media_MsgCoreDlnaDmsChange(pstContents, p1, p2, p3);
			break;

		case MSG_CORE_DLNA_CONTENTS_BRWOSE_DONE:
			eResult = nx_media_MsgCoreDlnaContentsBrowseDone(pstContents, p1, p2, p3);
			break;
#endif

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED!\n");
			ONDK_GWM_Obj_Destroy(NX_PRISM_UI_MEDIA_OPTION_DIM_ID);
			eResult = nx_media_MsgAppOptListSelected(pstContents, p1, p2, p3);
			break;

		case MSG_APP_KEYBOARD_RESULT:
			NX_APP_Message("MSG_APP_KEYBOARD_RESULT!\n");
			eResult = nx_media_MsgAppKeyboardResult(pstContents, p1, p2, p3);
			break;

		case MSG_APP_POPUP_LIST_DESTROYED:
			NX_APP_Message("MSG_APP_POPUP_LIST_DESTROYED!\n");
			ONDK_GWM_Obj_Destroy(NX_PRISM_UI_MEDIA_OPTION_DIM_ID);
			break;

		case MSG_APP_PLAYER_DO_STOP:
			eResult = nx_media_MsgAppPlayerStop(pstContents, p1, p2, p3);
			break;

		case MSG_APP_DLG_CLICKED:
		case MSG_APP_DLG_TIMEOUT:
			NX_APP_Message("MSG_APP_DLG_CLICKED || MSG_APP_DLG_TIMEOUT!\n");
			eResult = nx_media_MsgAppDlgClicked(pstContents, p1);
			break;

		case MSG_APP_MEDIA_TARGET_STORAGE_SELECTED:
			NX_APP_Message("MSG_APP_MEDIA_TARGET_STORAGE_SELECTED!\n");
			eResult = nx_media_MsgAppTargetStorageSelected(pstContents, p1);
			break;

		case MSG_APP_HOMEMENU_UI_LAUNCH:
			NX_APP_Message("MSG_APP_HOMEMENU_UI_LAUNCH\n");
			if (p1 != eNX_UCT_Meida)
			{
				ONDK_GWM_APP_Destroy(0);
			}
			break;

		default:
			break;
	}

	if (eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

