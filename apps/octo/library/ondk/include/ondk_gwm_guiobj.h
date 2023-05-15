/**************************************************************
 *	@file		mlib.h
 *	Minimized Graphic Library for DirectFB
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/03/07
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

#ifndef	__GUIOBJ_H__
#define	__GUIOBJ_H__

#include <ondk.h>

/* ======================================================================== */
/* Include. 																						*/
/* ======================================================================== */

/* ======================================================================== */
/* Exported Macros. 																				*/
/* ======================================================================== */

// 32비트중, MSB 4비트는 기본 속성을 지정해야 하므로 사용하지 말 것.
/* Object Focus flags - 비트 단위로 검사 */
#define	GUIOBJ_NORMAL				0x00000000			/* doesn't have a focus */
#define	GUIOBJ_HILIGHTED			0x10000000			/* same to the focused */
#define	GUIOBJ_PUSHED				0x20000000			/* button feedback */
#define	GUIOBJ_DISABLED				0x40000000

#define	SELECTOR_RIGHT_NOMORE		0x0400
#define	SELECTOR_LEFT_NOMORE		0x0800
#define	LIST_NOENABLE_ITEM			0x0200
#define	LIST_UP_NOMORE				0x0400
#define	LIST_DOWN_NOMORE			0x0800
#define	LIST_RIGHT_NOMORE			0x1000
#define	LIST_LEFT_NOMORE			0x2000
#define	SCROLLER_RIGHT_MORE			0x0400
#define	SCROLLER_LEFT_MORE			0x0800
#define	SCROLLER_UP_MORE			0x1000
#define	SCROLLER_DOWN_MORE			0x2000

#define	TEXT_ALIGN_LEFT				0x0100
#define	TEXT_ALIGN_RIGHT			0x0200
#define	TEXT_ALIGN_CENTER			0x0400
#define	TEXT_ALIGN_H_MASK			0x0F00

#define	TEXT_ALIGN_TOP				0x0001
#define	TEXT_ALIGN_BOTTOM			0x0002
#define	TEXT_ALIGN_MIDDLE			0x0004
#define	TEXT_MULTILINED				0x0008
#define	TEXT_WORDWRAP				0x0010
#define	TEXT_ALIGN_V_MASK			0x00F0

//RECT
#define	RECT_SHAPE_NORMAL		    0x0000
#define	RECT_SHAPE_STROKE		    0x0001


#define	TAB_RIGHT_NOMORE			0x0400
#define	TAB_LEFT_NOMORE				0x0800


/** @brief definitions for Formatted text list (FMT) object */
#define FMT_ITEM_WIDTH				80
#define FMT_ITEM_HEIGHT				26
#define FMT_HEIGHT					(FMT_ITEM_HEIGHT * 5)
#define FMT_WIDTH					200

#define MAX_FMT_ITEM				500			// the maxuimum number of list items which can handle

#define FMT_BOUND_THICK				0			// list 테두리가 필요하면, 이 값을 0뵤다 큰값으로 지정하시요.
#define FMT_SCROLL_W				26


/** @brief attribute flags of FMT object */
#define FMT_SCROLL_ON				0x0010		// default look scroll

#define FMT_DEFAULT_LIST			0x0100		// default list item look and feel
#define FMT_OWN_DRAWITEM			0x0200		// item drawer
#define FMT_NOTIFY_ACTION			0x1000		// when moving item selection

/** @brief custom tag definition of FMT object */
#define FMT_CUSTOMTAG_ON					'<'

#define FMT_CUSTOMTAG_IMAGE					'I'
#define FMT_CUSTOMTAG_IMAGE_ID				'P'

#define FMT_CUSTOMTAG_DATA_START			'['
#define FMT_CUSTOMTAG_DATA_END				']'

#define FMT_CUSTOMTAG_IMAGE_INDICATOR		"[!IMG]"
#define FMT_CUSTOMTAG_IMAGE_INDICATOR_LEN	6

/* ======================================================================== */
/* Exported types. 																				*/
/* ======================================================================== */

//// Object 로 전달될 메소드 들..
/* 원래 이름이 Method Type으로 MT를 가지고 있었으나 proc으로 통일 하였다. */
/* MT_XXX를 PM_XXX로 바꾸려 하였으나 이름이 적절치 않아 메세지만 그대로 둠. */
typedef	enum
{
	MT_CREATE							= 1,
	MT_DESTROY,
	MT_DRAW,
	MT_RELOCATE,
	MT_GET_FOCUS,
	MT_LOST_FOCUS,
	MT_KEYDOWN,
	MT_KEYUP,
	MT_CHANGE_TEXT,
	MT_SET_VALUE,
	MT_CHANGED_VALUE,
	MT_VISIBLE,
	MT_CHANGED_ALPHA,


	MT_END
} GWM_Obj_MethodType_t;

//// Object 의 종류..
typedef	enum
{
	GUIOBJ_APPLICATION	= 0,	// 항상 필요한 Object
	GUIOBJ_UNKNOWN,
	GUIOBJ_RECTANGLE,
	GUIOBJ_BITMAP,
	GUIOBJ_SCROLLER,
	GUIOBJ_TEXT,
	GUIOBJ_SLIDETEXT,
	GUIOBJ_MULTITEXT,
	GUIOBJ_EXTTEXT,
	GUIOBJ_STATUS,
	GUIOBJ_FRAME,			/* Focus 를 갖지 못하는 Object 의 경우에는 Frame 의 앞에 둘 것. */
	GUIOBJ_BUTTON,			/* Focus 를 갖는 Object 의 경우에는 Frame 의 뒤에 둘 것. */
	GUIOBJ_SELECTOR,
	GUIOBJ_LIST,

	GUIOBJ_GRID,
	GUIOBJ_INPUT,
	GUIOBJ_FMT,

	GUIOBJ_TREE,
	GUIOBJ_TAB,
	GUIOBJ_GAUGEBAR,
	GUIOBJ_INPUTSELECTOR,
	GUIOBJ_PICTURE,
	GUIOBJ_STYLEDTEXT,
	GUIOBJ_KEY,
	GUIOBJ_MULTILINETEXT,

	GUIOBJ_VPROGRESS,
	GUIOBJ_HPROGRESS,
	GUIOBJ_CHANNELLIST,
	GUIOBJ_TVGUIDE,
	GUIOBJ_KEYBOARD,
	GUIOBJ_SIMPLEKEYBOARD,
	GUIOBJ_NATIVESIMPLEKEYBOARD,
	GUIOBJ_IMAGE,
	GUIOBJ_MARKUP,
	GUIOBJ_MULTI,
	// TODO: add focusable items for spin, check, slider, tab controls...


	GUIOBJ_MAX
} GWM_ObjectType_t;

typedef HUINT32*	GWM_PFont_t;

typedef	struct	tagFrameObject *		FRAME_OBJECT;
typedef	struct	tagButtonObject *		BUTTON_OBJECT;
typedef	struct	tagRectObject *		    RECT_OBJECT;
typedef	struct	tagTextObject *		    TEXT_OBJECT;
typedef	struct	tagSlideTextObject *	SLIDETEXT_OBJECT;
typedef	struct	tagListObject *		    LIST_OBJECT;
typedef	struct	tagNewListObject *		NEW_LIST_OBJECT;
typedef	struct	tagHorListObject *		HOR_LIST_OBJECT;
typedef	struct	tagSelectorObject *	    SELECTOR_OBJECT;
typedef struct	tagStatusObject *		STATUS_OBJECT;
typedef	struct	tagScrollerObject *		SCROLLER_OBJECT;
typedef	struct	tagInputObject *		INPUT_OBJECT;
typedef	 struct	tagFMTObject *		    FMT_OBJECT;
typedef	struct	tagVProgressObject *	VPROGRESS_OBJECT;
typedef	struct	tagHProgressObject *	HPROGRESS_OBJECT;
typedef struct	tagChannelListObject *	CHANNEL_LIST_OBJECT;
typedef struct	tagTvGuideObject *		TV_GUIDE_OBJECT;
typedef struct  tagGridListObject * 	GRID_LIST_OBJECT;
typedef	struct	tagLinkedSurfaceObject *	LINKED_SURFACE_OBJECT;
typedef	struct	tagKeyboardObject  *	KEYBOARD_OBJECT;
typedef	struct	tagSimpleKeyboardObject  *	SIMPLEKEYBOARD_OBJECT;
typedef struct 	tagNativeSimpleKeyboardObject * NATIVE_SIMPLEKEYBOARD_OBJECT;

typedef struct	tagMultiObject *		MULTI_OBJECT;


typedef	struct	tagImgObject *			IMAGE_OBJECT;



#if 0
typedef	struct	tagBitmapObject *		BITMAP_OBJECT;
#ifdef CONFIG_UI_ALVIN
typedef	struct	tagMListObject *		MLIST_OBJECT;
#endif

#endif

#if 0 //비사용 오브젝트
typedef struct	tagTreeObject *				PTREE_OBJECT;
typedef struct 	tagTreeObjectItemDrawInfo*	PTREEOBJ_ITEM_DRAWINFO;
typedef HERROR	(*FP_CompFunc)(const void*, const void*);
typedef GWM_Result_t	(*FP_GetNextItemFunc)(HTREEITEM hItem, HTREEITEM *phFindItem);

typedef struct	tagGridObject *				PGRID_OBJECT;
typedef struct 	tagGridObjectItemDrawInfo*		PGRIDOBJ_ITEM_DRAWINFO;



typedef struct	tagTabObject *		TAB_OBJECT;
typedef struct	tagGaugebarObject*	PGAUGEBAR_OBJECT;
typedef	struct	tagInputSelectorObject *	INPUTSELECTOR_OBJECT;
typedef struct	tagPictureObject	*	PICTURE_OBJECT;
typedef struct	tagStyledTextObject *	STYLEDTEXT_OBJECT;
#endif

#if 0

typedef struct
{
	HINT32			locId;
	ONDK_FontId_t	fondId;
	ONDK_Color_t	color;
	HUINT32			size;
	HUINT32			withRatio;
	HINT32			space;
} PictureCaption_t;
#endif
// Item Array Value in the list structure :
/* Customized GWM Item Name Finding procedure */
/* arg will usually be used to access object's own data member */
typedef ONDK_Result_t		(*GWM_GetItem_t)(HUINT32 ulItemArray, HINT32 nIndex, HCHAR **pszOutputString);
typedef HUINT32             	GWM_LIST_ITEMS;
typedef struct tagTextProperty
{
	HUINT32			ulFontSize;
	HUINT32			ulFontAlign;
	ONDK_Color_t	cFontColor;
} Text_Property_t;

typedef enum
{
	eFMT_Item_Disabled				= 0,
	eFMT_Item_Enabled
} FMT_ItemStatus_t;


typedef struct tagFmtDrawItem
{
	ONDK_Rect_t         itemRect;
	FMT_ItemStatus_t	status;
	int					list_index;				// index in the list
	int					item_index;				// index in all list items
	Text_Property_t		stTextProperty;
	char					*str;
} FMT_DrawItem_t;

typedef struct tagFmtItem
{
	FMT_ItemStatus_t		status;
	Text_Property_t			stTextProperty;
	int					size;
	char					*itemStr;
} FMT_Item_t;

typedef ONDK_Result_t    (*FMT_Notify_Func)(HINT32 ID, HINT32 itemIndex);
typedef ONDK_Result_t    (*FMT_DrawItem_Func)(HINT32 ID, ONDKSurfaceHandle screen, FMT_DrawItem_t *drawItem);
typedef ONDK_Result_t    (*FMT_DrawScroll_Func)(ONDKSurfaceHandle pixmap, ONDK_Region_t *area, ONDK_Rect_t *rect, FMT_OBJECT obj);

#define GWM_INPUT_TIME_LENGTH	5
#define GWM_INPUT_DATE_LENGTH	5
#define GWM_INPUT_IP_LENGTH		16

typedef enum {
	eGUI_INPUTOBJ_MODE_Normal = 0,
	eGUI_INPUTOBJ_MODE_SpecialCh,
	eGUI_INPUTOBJ_MODE_Time,
	eGUI_INPUTOBJ_MODE_Date,
	eGUI_INPUTOBJ_MODE_Fill,		// Neo 처럼, 한글자씩 체우는 방식
	eGUI_INPUTOBJ_MODE_Pin,
	eGUI_INPUTOBJ_MODE_NoneFormatted,		// Neo 처럼, 한글자씩 체우는 방식, left가 Back Space로 동작
	eGUI_INPUTOBJ_MODE_IP,			// IP 입력.
	eGUI_INPUTOBJ_MODE_FREESAT_SMS,	// For Freesat SMS
	eGUI_INPUTOBJ_MODE_FORMAT,
	eGUI_INPUTOBJ_MODE_MAX
} InputObj_StringType_t;

typedef enum {
	eGUI_INPUTSELECTOROBJ_MODE_Normal = 0,
	eGUI_INPUTSELECTOROBJ_MODE_SPECIAL,
	eGUI_INPUTSELECTOROBJ_MODE_IndexedInput,
	eGUI_INPUTSELECTOROBJ_MODE_MAX
} InputSelectorObj_StringType_t;


typedef ONDK_Result_t (* GWM_StyledText_DrawScroll_t)(ONDKWindowHandle *pixmap, HINT32 x, HINT32 y, HINT32 h, HINT32 curr, HINT32 max, HINT32 view);

/* Customized Grid List Item drawing procedure */
typedef struct tagGridDrawItem
{
	ONDK_Rect_t         itemRect;
	int					item_index;				// index in all list items
} GRID_DrawItem_t;

typedef ONDK_Result_t	(*GRID_ItemDraw_t)(HINT32 ID, ONDKSurfaceHandle pixmap, GRID_DrawItem_t	*drawItem, HINT32 nColIndex);

/* ======================================================================== */
/* Exported Constants. 																				*/
/* ======================================================================== */

/* ======================================================================== */
/* Exported Variables. 																				*/
/* ======================================================================== */

/* ======================================================================== */
/* Exported Functions. 																				*/
/* ======================================================================== */

/* Frame */
extern ONDK_Result_t		ONDK_GWM_COM_Frame_Create(int id, ONDK_Rect_t *rect, HCHAR *title);
extern ONDK_Result_t		ONDK_GWM_COM_Frame_ChangeTitle(int id, char *new_title);
extern HCHAR *				ONDK_GWM_COM_Frame_GetTitleByObject(FRAME_OBJECT frame);
extern HCHAR *				ONDK_GWM_COM_Frame_GetTitle(HINT32 nId, HINT32 nParam);
extern HINT32				ONDK_GWM_COM_Frame_GetParam2ByObject(FRAME_OBJECT object);
extern HINT32				ONDK_GWM_COM_Frame_GetParam1ByObject(FRAME_OBJECT object);
extern ONDK_Result_t		ONDK_GWM_COM_Frame_SetParam2ByObject(FRAME_OBJECT object, HINT32 nParam);
extern ONDK_Result_t		ONDK_GWM_COM_Frame_SetParam1ByObject(FRAME_OBJECT object, HINT32 nParam);
extern ONDK_Result_t		ONDK_GWM_COM_Frame_SetParam2(HINT32 nId, HINT32 nParam);
extern ONDK_Result_t		ONDK_GWM_COM_Frame_SetParam1(HINT32 nId, HINT32 nParam);
extern ONDK_Result_t		ONDK_GWM_COM_Frame_SetColor(HINT32 nId, ONDK_Color_t textColor, ONDK_Color_t frameColor);


/* Button */
extern ONDK_Result_t		ONDK_GWM_COM_Button_Create(HINT32 id, ONDK_Rect_t *rect, HCHAR *text);
extern ONDK_Result_t	ONDK_GWM_COM_Button_ChangeImage(HINT32 id, HCHAR *normal, HCHAR *focused,  HCHAR *disabled, HCHAR *pushed);
extern ONDK_Result_t		ONDK_GWM_COM_Button_GetImageByObj(BUTTON_OBJECT obj, HCHAR **normal, HCHAR **focused, HCHAR **disabled, HCHAR **pushed);
extern ONDK_Result_t		ONDK_GWM_COM_Button_GetImageHandleByObj(BUTTON_OBJECT obj,ONDKImageHandle *normal,ONDKImageHandle *focused, ONDKImageHandle *disabled, ONDKImageHandle *pushed);
extern ONDK_Result_t		ONDK_GWM_COM_Button_GetImage(HINT32 id, HCHAR **normal, HCHAR **focused, HCHAR **disabled, HCHAR **pushed);
extern ONDK_Result_t		ONDK_GWM_COM_Button_SetStatus(HINT32 id, HINT32 push_status);
extern ONDK_Result_t		ONDK_GWM_COM_Button_SetFontColor(HINT32 id, ONDK_Color_t fontColor, ONDK_Color_t fontfocus);
extern ONDK_Result_t		ONDK_GWM_COM_Button_SetButtonColor(HINT32 id, ONDK_Color_t over, ONDK_Color_t normal, ONDK_Color_t click, ONDK_Color_t disable);
extern ONDK_Result_t		ONDK_GWM_COM_Button_SetFont(HINT32 id,ONDK_FontId_t fontId);

extern HCHAR *				ONDK_GWM_COM_Button_GetTitle(HINT32 id);
extern HCHAR *				ONDK_GWM_COM_Button_GetTitleByObj(BUTTON_OBJECT btn);
extern HUINT32				ONDK_GWM_COM_Button_GetStatusByObj(BUTTON_OBJECT btn);
extern ONDK_Result_t		ONDK_GWM_COM_Button_ChangeTitle (HINT32 id, HCHAR *szNewText);
extern ONDK_Result_t		ONDK_GWM_COM_Button_ChangeTitleWithoutUpdate (HINT32 id, HCHAR *szNewText);
extern ONDK_Result_t		ONDK_GWM_COM_Button_SetTitleSize(HINT32 id, HUINT16 size);
extern ONDK_Result_t		ONDK_GWM_COM_Button_SetAlign(HINT32 id, ONDKSurfaceTextFlags align);
extern ONDK_Result_t		ONDK_GWM_COM_Button_SetExtraAlign(HINT32 id, ONDKSurfaceTextFlags align);
extern ONDK_Result_t		ONDK_GWM_COM_Button_GetTitleSize(HINT32 id, HUINT16 *size);
extern ONDK_Result_t		ONDK_GWM_COM_Button_SetExtraText(HINT32 id, HUINT8 *text);
extern HCHAR *				ONDK_GWM_COM_Button_GetExtraTextByObj(BUTTON_OBJECT btn);
extern ONDK_Result_t		ONDK_GWM_COM_Button_SetSimpleButton(HINT32 id, HBOOL simple);
extern ONDK_Result_t		ONDK_GWM_COM_Button_SetButtonDimmedAlpha(HINT32 id,HINT32 level);
extern ONDK_Result_t		ONDK_GWM_COM_Button_GetTitleSizeByObj(BUTTON_OBJECT	obj, HUINT16 *size);

/* vprogress */
extern HINT32				ONDK_GWM_COM_VProgress_GetPos(HINT32 id);
extern void					ONDK_GWM_COM_VProgress_SetMin(HINT32 id, HINT32 minPos);
extern HINT32				ONDK_GWM_COM_VProgress_GetMin(HINT32 id);
extern void					ONDK_GWM_COM_VProgress_SetMax(HINT32 id, HINT32 maxPos);
extern HINT32				ONDK_GWM_COM_VProgress_GetMax(HINT32 id);
extern void					ONDK_GWM_COM_VProgress_SetUserdata(HINT32 id, void *userdata);
extern void					*ONDK_GWM_COM_VProgress_GetUserdata(HINT32 id);
extern ONDK_Result_t		ONDK_GWM_COM_VProgress_Create(HINT32 id, ONDK_Rect_t *rect, HINT32 minPos, HINT32 maxPos, HINT32 curPos);

extern void 				ONDK_GWM_COM_VProgress_SetPos(HINT32 id, HINT32 curPos);
extern HINT32				ONDK_GWM_COM_VProgress_GetVspace(HINT32 id);
extern void 				ONDK_GWM_COM_VProgress_SetVspace(HINT32 id, HINT32 vspace);
extern HCHAR				*ONDK_GWM_COM_VProgress_GetBlankImg(HINT32 id);
extern void 				ONDK_GWM_COM_VProgress_SetBlankImg(HINT32 id, HCHAR	*blankImg);
extern HCHAR				*ONDK_GWM_COM_VProgress_GetFillImg(HINT32 id);
extern void 				ONDK_GWM_COM_VProgress_SetFillImg(HINT32 id, HCHAR *fillImg);
extern HCHAR				*ONDK_GWM_COM_VProgress_GetBackgroundImg(HINT32 id);
extern void 				ONDK_GWM_COM_VProgress_SetBackgroundImg(HINT32 id, HCHAR *backgroundImg);

/*
Channel list
*/
extern ONDK_Result_t		ONDK_GWM_COM_ChannelList_Create(HINT32 id, HINT32 posX, HINT32 posY);
#if defined(CONFIG_PROD_LANG_ARABIC)
extern void					ONDK_GWM_COM_ChannelList_SetUserdata(HINT32 id, void *userdata, HINT32 lUserDataCount, HINT32 lLanguage);
#else
extern void 				ONDK_GWM_COM_ChannelList_SetUserdata(HINT32 id, void *userdata, HINT32 lUserDataCount);
#endif
extern HCHAR				*ONDK_GWM_COM_ChannelList_GetBackgroundImg(HINT32 id);
extern void	 				ONDK_GWM_COM_ChannelList_SetBackgroundImg(HINT32 id, HCHAR *backgroundImg);
extern void 				ONDK_GWM_COM_ChannelList_SetCasImg(HINT32 id, HCHAR	*casImg);
extern void					ONDK_GWM_COM_ChannelList_SetLockImg(HINT32 id, HCHAR *szLockImg);
extern void 				ONDK_GWM_COM_ChannelList_SetProgrammeTime(HINT32 id, HUINT32 startTime, HUINT32 duration);

/*
TvGuide
*/
extern ONDK_Result_t		ONDK_GWM_COM_TvGuide_Create(HINT32 id, HINT32 posX, HINT32 posY);
extern void 				ONDK_GWM_COM_TvGuide_SetBackgroundImg(HINT32 id, HCHAR *backgroundImg);
extern void 				ONDK_GWM_COM_TvGuide_SetPF(HINT32 id, HBOOL bPf);
extern void 				ONDK_GWM_COM_TvGuide_SetCurrentTime(HINT32 id, HUINT32 currentTime);
extern void					ONDK_GWM_COM_TvGuide_SetStartUnitTime(HINT32 id, HUINT32 unitTime);
extern void 				ONDK_GWM_COM_TvGuide_SetGrid1LImg(HINT32 id, HCHAR *img);
extern void 				ONDK_GWM_COM_TvGuide_SetGrid2CImg(HINT32 id, HCHAR *img);
extern void 				ONDK_GWM_COM_TvGuide_SetGrid3RImg(HINT32 id, HCHAR *img);
extern void					ONDK_GWM_COM_TvGuide_SetGridDimLImg(HINT32 id, HCHAR *img);
extern void					ONDK_GWM_COM_TvGuide_SetGridDimRImg(HINT32 id, HCHAR *img);

extern void 				ONDK_GWM_COM_TvGuide_SetEpgData(HINT32 id, void	*userdata);
extern void					ONDK_GWM_COM_TvGuide_SetEpgStringData(HINT32 id, HCHAR *pszNoProgram, HCHAR *pszOnAir, HCHAR *pszNext);
extern void					ONDK_GWM_COM_TvGuide_SetEpgCurrentChannel(HINT32 id, HUINT32 ulSvcUid);

/*
Grid List
*/
extern ONDK_Result_t		ONDK_GWM_COM_GridList_Create(HINT32 id, ONDK_Rect_t *rect, HCHAR *title, HINT32 cellWidth, HINT32 cellHeight, HINT32 itemCount, HINT32 maxColCount, HINT32 maxRowCount, HINT32 startIndex);
extern ONDK_Result_t		ONDK_GWM_COM_GridList_SetCurrentIndex(HINT32 id, HINT32 nIndex);
extern ONDK_Result_t		ONDK_GWM_COM_GridList_ResetItemNum(HINT32 id, HINT32 nItemNum);
extern ONDK_Result_t		ONDK_GWM_COM_GridList_SetItemDrawMethod(HINT32 id, GRID_ItemDraw_t itemDrawFunc);
extern HINT32				ONDK_GWM_COM_GridList_GetCurrentIndexByObjID(HINT32 id);
extern HBOOL				ONDK_GWM_COM_GridList_IsFirstColumn(HINT32 id);
extern ONDK_Result_t		ONDK_GWM_COM_GridList_ChangeProperty(HINT32 id, HINT32 cellWidth, HINT32 cellHeight, HINT32 itemCount, HINT32 maxColCount, HINT32 maxRowCount, HINT32 startIndex);

#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
typedef	ONDK_Result_t		(*pfnGridListAnimationCtl)(void);
extern ONDK_Result_t		ONDK_GWM_COM_GridList_SetAnimationCtl(HINT32 id, pfnGridListAnimationCtl fnSetAnimationCtl);
extern HINT32				ONDK_GWM_COM_GridList_UpdateAnimation(HINT32 id);
#endif


#if defined(CONFIG_APUI_NATIVE_APP)
//extern ONDK_Result_t		ONDK_GWM_COM_Keyboard_Create(HINT32 id, ONDK_Rect_t *rect, HUINT8 *str);
extern ONDK_Result_t		ONDK_GWM_COM_Keyboard_Create(HINT32 id, ONDK_Rect_t *rect, HINT32 curFocus, HINT32 isEnglishArea, HCHAR *str);

extern HINT32 				ONDK_GWM_COM_Keyboard_GetCurFocus_Row(HINT32 id);
extern HINT32 				ONDK_GWM_COM_Keyboard_GetCurFocus_Col(HINT32 id);
extern void 				ONDK_GWM_COM_Keyboard_SetCurFocus_Row(HINT32 id, HINT32 cur);
extern void 				ONDK_GWM_COM_Keyboard_SetCurFocus_Col(HINT32 id, HINT32 cur);
extern void 				ONDK_GWM_COM_Keyboard_SetCharValue(HINT32 id, HINT32 pos, HCHAR *str);
extern void 				ONDK_GWM_COM_Keyboard_SetNumValue(HINT32 id, HINT32 pos, HCHAR *str);
extern void 				ONDK_GWM_COM_Keyboard_SetInputBoxSize(HINT32 id, HINT32 size);
extern void					ONDK_GWM_COM_Keyboard_SetInputBoxRect(HINT32 id, ONDK_Rect_t *stRect);
extern void					ONDK_GWM_COM_Keyboard_SetBtnKeyRect(HINT32 id, ONDK_Rect_t *stRect);
extern void					ONDK_GWM_COM_Keyboard_SetBtnActRect(HINT32 id, ONDK_Rect_t *stRect);
extern void					ONDK_GWM_COM_Keyboard_SetBtnChangeColor(HINT32 id, ONDK_Color_t ulBNColor, ONDK_Color_t ulTNColor, ONDK_Color_t ulBCColor, ONDK_Color_t ulTCColor);
extern void 				ONDK_GWM_COM_Keyboard_SetLanguage(HINT32 id, HCHAR *str);
extern void					ONDK_GWM_COM_Keyboard_SetEnterBtnStr(HINT32 id, HCHAR *str);
extern void 				ONDK_GWM_COM_Keyboard_SetUppercaseAndLowercaseLetters(HINT32 id, HBOOL bUpperCase);
extern void					ONDK_GWM_COM_Keyboard_SetImageBack(HINT32 id, HCHAR *imgPathC, HCHAR *imgPathN);
extern void					ONDK_GWM_COM_Keyboard_SetImageClear(HINT32 id, HCHAR *imgPathC, HCHAR *imgPathN);
extern void					ONDK_GWM_COM_Keyboard_SetImageSpace(HINT32 id, HCHAR *imgPathC, HCHAR *imgPathN);

extern ONDK_Result_t		ONDK_GWM_COM_SimpleKeyboard_Create(HINT32 id, ONDK_Rect_t *rect, HINT32 curFocus, HCHAR *str);
extern HINT32 				ONDK_GWM_COM_SimpleKeyboard_GetCurFocus(HINT32 id);
extern void 				ONDK_GWM_COM_SimpleKeyboard_SetCurFocus(HINT32 id, HINT32 cur);
extern void 				ONDK_GWM_COM_SimpleKeyboard_SetCharValue(HINT32 id, HINT32 pos, HCHAR *str);
extern void 				ONDK_GWM_COM_SimpleKeyboard_SetCurLanguage(HINT32 id, HCHAR *str);


/* TEST KEYBOARD 작업중*/
/* // hkjeong 가 nativepp keyboard 를 재작업하던중.. 아직 완료가 안되어 , 다 막아버림.
   // 추후 작업이 진행된다면.. .........................
extern ONDK_Result_t		ONDK_GWM_COM_Keyboard_SetKeyCapColor(HINT32 id, ONDK_Color_t over, ONDK_Color_t normal, ONDK_Color_t click, ONDK_Color_t disable);
extern ONDK_Result_t		ONDK_GWM_COM_Keyboard_SetFontColor(HINT32 id, ONDK_Color_t fontColor, ONDK_Color_t fontfocus);
extern ONDK_Result_t		ONDK_GWM_COM_Keyboard_SetStatus(HINT32 id, HINT32 push_status);
extern ONDK_Result_t		ONDK_GWM_COM_Keyboard_ChangeKeyCapStr (HINT32 id, HUINT8 *szNewStr);
extern ONDK_Result_t		ONDK_GWM_COM_Keyboard_SetKeycapStrSize(HINT32 id, HUINT32 size);


extern ONDK_Result_t		ONDK_GWM_COM_Keyboard_SetAlign(HINT32 id, ONDKSurfaceTextFlags align);
extern ONDK_Result_t		ONDK_GWM_COM_Keyboard_SetDisableKeyCap(HINT32 id, HBOOL flag);
extern ONDK_Result_t		ONDK_GWM_COM_Keyboard_SetBtnImage(HINT32 id, HCHAR *normal, HCHAR *focused,  HCHAR *disabled, HCHAR *pushed);
//extern HCHAR *			ONDK_GWM_COM_Keyboard_GetKeycapStr(HINT32 id);
*/
#else
/* Virtual Keyboard */

extern ONDK_Result_t		ONDK_GWM_Keyboard_Create(HINT32 id, ONDK_Rect_t *rect, HINT32 curFocus, HINT32 isEnglishArea, HCHAR *str);
extern HINT32 				ONDK_GWM_Keyboard_GetCurFocus_Row(HINT32 id);
extern HINT32 				ONDK_GWM_Keyboard_GetCurFocus_Col(HINT32 id);
extern void 				ONDK_GWM_Keyboard_SetCurFocus_Row(HINT32 id, HINT32 cur);
extern void 				ONDK_GWM_Keyboard_SetCurFocus_Col(HINT32 id, HINT32 cur);

extern void 				ONDK_GWM_Keyboard_SetCharValue(HINT32 id, HINT32 pos, HCHAR *str);
extern void					ONDK_GWM_Keyboard_SetNumValue(HINT32 id, HINT32 pos, HCHAR *str);
extern void					ONDK_GWM_Keyboard_SetInputBoxSize(HINT32 id, HINT32 size);
extern void 				ONDK_GWM_Keyboard_SetLanguage(HINT32 id, HCHAR *str);
extern void 				ONDK_GWM_Keyboard_SetUppercaseAndLowercaseLetters(HINT32 id, HBOOL bUpperCase);

extern ONDK_Result_t		ONDK_GWM_SimpleKeyboard_Create(HINT32 id, ONDK_Rect_t *rect, HINT32 curFocus, HCHAR *str);
extern HINT32 				ONDK_GWM_SimpleKeyboard_GetCurFocus(HINT32 id);
extern void 				ONDK_GWM_SimpleKeyboard_SetCurFocus(HINT32 id, HINT32 cur);
extern void 				ONDK_GWM_SimpleKeyboard_SetCharValue(HINT32 id, HINT32 pos, HCHAR *str);
extern void 				ONDK_GWM_SimpleKeyboard_SetCurLanguage(HINT32 id, HCHAR *str);
#endif


/* Rect */
extern ONDK_Result_t		ONDK_GWM_COM_Rect_Create(HINT32 id, ONDK_Rect_t *rect, HUINT32 color);
extern ONDK_Result_t		ONDK_GWM_COM_Rect_ChangeColor(HINT32 id, HUINT32 color);
extern ONDK_Color_t			ONDK_GWM_COM_Rect_GetColor(RECT_OBJECT obj);
extern ONDK_Result_t		ONDK_GWM_COM_Rect_ChangeSize(HINT32 id,HINT32 lx,HINT32 ly,HINT32 lw,HINT32 lh);
extern ONDK_Result_t		ONDK_GWM_COM_Rect_SetStroke(HINT32 id, HINT32 r, HUINT32 strokeColor);


#if 0
/* Bitmap */
extern ONDK_Result_t		GWM_Bitmap_Create(int id, ONDK_Rect_t *rect, unsigned char *bitmap);
extern ONDK_Result_t		GWM_Bitmap_ChangeBmp(int id, unsigned char *bitmap);
extern char *				GWM_Bitmap_GetBmpPtrByObject(BITMAP_OBJECT obj);
#endif
/* Selector */
extern ONDK_Result_t		ONDK_GWM_Selector_Create(HINT32 id, ONDK_Rect_t *rect, HCHAR *title,
													HINT32 item_count, char **item_array);
extern void					ONDK_GWM_Selector_GetProperty(SELECTOR_OBJECT object,
													HINT32 *status, HINT32 *total, HINT32 *curr, HCHAR **title);
extern ONDK_Result_t		ONDK_GWM_Selector_GetTotalItem(HINT32 id, HINT32 *number);
extern int					ONDK_GWM_Selector_GetCurrentIndex(HINT32 id);
extern ONDK_Result_t		ONDK_GWM_Selector_SetCurrentIndex(HINT32 id, HINT32 nIndex);
extern HINT32				ONDK_GWM_Selector_IsFocused(SELECTOR_OBJECT selector);
extern char *				ONDK_GWM_Selector_GetSelectedItemTextPtr(SELECTOR_OBJECT selector);
extern char *				ONDK_GWM_Selector_GetCurItemTextPtr(SELECTOR_OBJECT selector);
extern int					ONDK_GWM_Selector_GetSelectedIndex(SELECTOR_OBJECT selector);
extern ONDK_Result_t 		ONDK_GWM_Selector_ChangeProperty(HINT32 id, HCHAR *title, HINT32 item_count, HCHAR **item_array);
extern char *				ONDK_GWM_Selector_GetTitlePtr(SELECTOR_OBJECT selector);
extern ONDK_Result_t		ONDK_GWM_Selector_SetItemDimension (HINT32 id, HINT32 xoffset, HINT32 width);
extern ONDK_Result_t		ONDK_GWM_Selector_GetItemDimension (HINT32 id, HINT32 *xoffset, HINT32 *width);
extern ONDK_Result_t		ONDK_GWM_Selector_SetColor(HINT32 id, ONDK_Color_t over, ONDK_Color_t normal, ONDK_Color_t click,
														ONDK_Color_t disable, ONDK_Color_t shadow, ONDK_Color_t hilight);
#if 0
/* Input Selector */
extern ONDK_Result_t		GWM_InputSelector_Create(int id, ONDK_Rect_t *rect, char *title, int digit,
														int item_count, char **item_array);
extern void					GWM_InputSelector_GetProperty(INPUTSELECTOR_OBJECT object,
														int *status, int *total, int *curr, int *digit, int *pos, char **title);
extern ONDK_Result_t 		GWM_InputSelector_SetProperty(HINT32 id, char *title, int digit, int item_count, char **item_array);
extern ONDK_Result_t		GWM_InputSelector_GetTotalItem(int id, int *number);
extern int					GWM_InputSelector_GetCurrentIndex(int id);
extern ONDK_Result_t		GWM_InputSelector_SetCurrentIndex(int id, int nIndex);
extern HINT32				GWM_InputSelector_IsFocused(INPUTSELECTOR_OBJECT selector);
extern char *				GWM_InputSelector_GetSelectedItemTextPtr(INPUTSELECTOR_OBJECT selector);
extern char *				GWM_InputSelector_GetCurItemTextPtr(INPUTSELECTOR_OBJECT selector);
extern int					GWM_InputSelector_GetSelectedIndex(INPUTSELECTOR_OBJECT selector);
extern char *				GWM_InputSelector_GetTitlePtr(INPUTSELECTOR_OBJECT selector);
extern ONDK_Result_t		GWM_InputSelector_SetTitlePtr(int id, char *title);

extern void					GWM_InputSelector_GetPosition(INPUTSELECTOR_OBJECT object, int *digit, int *pos);
extern ONDK_Result_t		GWM_InputSelector_CheckInput(int id);
extern ONDK_Result_t		GWM_InputSelector_SetSelectorType(int ID, int type);
extern int					GWM_InputSelector_GetSelectorType(INPUTSELECTOR_OBJECT inobj);
extern ONDK_Result_t		GWM_InputSelector_SetInputIndex (HINT32 id, HINT32 nIndex);
extern ONDK_Result_t		GWM_InputSelector_GetInputIndex (HINT32 id, HINT32 *pnIndex);
extern ONDK_Result_t		GWM_InputSelector_SetInputValue (HINT32 id, const HUINT8 *szValue);
extern ONDK_Result_t		GWM_InputSelector_SetItemDimension (HINT32 id, HINT32 xoffset, HINT32 width);
extern ONDK_Result_t		GWM_InputSelector_GetItemDimension (HINT32 id, HINT32 *xoffset, HINT32 *width);
extern ONDK_Result_t		ONDK_Result_t	GWM_InputSelector_SetColor(HINT32 id,
		ONDK_Color_t focus_light, ONDK_Color_t focus_face, ONDK_Color_t focus_shadow, ONDK_Color_t hilight_text,
		ONDK_Color_t disable_light, ONDK_Color_t disable_face, ONDK_Color_t disable_shadow,
		ONDK_Color_t disable_text, ONDK_Color_t inactive_shadow, ONDK_Color_t inactive_hilight);

#endif

/* Text */
extern ONDK_Result_t		ONDK_GWM_COM_Text_Create(HINT32 id, ONDK_Rect_t *rect, HCHAR *text);
extern void					ONDK_GWM_COM_Text_GetProperty(TEXT_OBJECT object, ONDK_Color_t *font_color, ONDK_Color_t *bg_color, HUINT32 *align, HUINT32 *size);
extern HUINT8 *				ONDK_GWM_COM_Text_GetPtrByObject(TEXT_OBJECT obj);
extern ONDK_Result_t		ONDK_GWM_COM_Text_ChangeColor(int id, ONDK_Color_t color, ONDK_Color_t background);
extern HINT32				ONDK_GWM_COM_Text_GetAlignByObject(TEXT_OBJECT obj);
extern ONDK_Result_t		ONDK_GWM_COM_Text_SetAlign(int id, int align);
extern ONDK_Color_t			ONDK_GWM_COM_Text_GetForeColorByObject(TEXT_OBJECT obj);
extern ONDK_Color_t			ONDK_GWM_COM_Text_GetBackColorByObject(TEXT_OBJECT obj);
extern HINT32				ONDK_GWM_COM_Text_GetSizeByObject(TEXT_OBJECT obj);
extern ONDK_Result_t		ONDK_GWM_COM_Text_SetSize(HUINT32 id, HINT32 size);
extern ONDK_Result_t		ONDK_GWM_COM_Text_SetText(HINT32 id, HCHAR *str);
extern ONDK_Result_t		ONDK_GWM_COM_Text_SetFont (HINT32 id, ONDK_FontId_t fontId);
extern ONDK_FontId_t		ONDK_GWM_COM_Text_GetFont (HINT32 id);
extern HINT32				ONDK_GWM_COM_Text_GetStringWidth(ONDK_FontId_t eFontId, HINT32 lFontsize, HCHAR *str);
extern ONDK_Result_t		ONDK_GWM_COM_Text_ChangeLocation(HINT32 id, const ONDK_Rect_t *rect);

/* SlideText */
extern ONDK_Result_t		ONDK_GWM_COM_SlideText_Create(HINT32 id, ONDK_Rect_t *rect, HCHAR *text);
extern HUINT8 *				ONDK_GWM_COM_SlideText_GetPtrByObject(SLIDETEXT_OBJECT obj);
extern ONDK_Result_t		ONDK_GWM_COM_SlideText_ChangeColor(int id, ONDK_Color_t color);
extern HINT32				ONDK_GWM_COM_SlideText_GetAlignByObject(SLIDETEXT_OBJECT obj);
extern ONDK_Result_t		ONDK_GWM_COM_SlideText_SetAlign(int id, int align);
extern ONDK_Color_t			ONDK_GWM_COM_SlideText_GetForeColorByObject(SLIDETEXT_OBJECT obj);
extern HINT32				ONDK_GWM_COM_SlideText_GetSizeByObject(SLIDETEXT_OBJECT obj);
extern ONDK_Result_t		ONDK_GWM_COM_SlideText_SetSize(HUINT32 id, HINT32 size);
extern ONDK_Result_t		ONDK_GWM_COM_SlideText_SetText(HINT32 id, HCHAR *str);
extern ONDK_Result_t		ONDK_GWM_COM_SlideText_SetFont (HINT32 id, ONDK_FontId_t fontId);
extern ONDK_FontId_t		ONDK_GWM_COM_SlideText_GetFont (HINT32 id);
extern ONDK_Result_t		ONDK_GWM_COM_SlideText_Update (HINT32 id, HINT32 speed);
extern ONDK_Result_t		ONDK_GWM_COM_SlideText_BGImage (HINT32 id, HCHAR *backgroundImg);
extern ONDK_Result_t		ONDK_GWM_COM_SlideText_BGColor (HINT32 id, ONDK_Color_t color);


/* Scroll */
extern ONDK_Result_t		ONDK_GWM_Scroll_CreateVertical(HINT32 id, ONDK_Rect_t *rect, HINT32 range, HINT32 start, HINT32 size);
extern ONDK_Result_t		ONDK_GWM_Scroll_CreateHorizontal(HINT32 id, ONDK_Rect_t *rect, HINT32 range, HINT32 start, HINT32 size);
extern void					ONDK_GWM_Scroll_GetProperty(SCROLLER_OBJECT object,
												HINT32 *type, HINT32 *range, HINT32 *start, HINT32 *size);
extern ONDK_Result_t		ONDK_GWM_Scroll_SetValue(HINT32 id, HINT32 range, HINT32 start, HINT32 size);
/* List */
//extern GWM_Result_t			GWM_List_Create(int id, OSD_Rect_t *rect, char *title, int cell_height,
//												int item_count, GWM_LIST_ITEMS item_array);
extern ONDK_Result_t		ONDK_GWM_List_Create(HINT32 id, ONDK_Rect_t *rect, HCHAR *title, HINT32 cell_height,
									HINT32 item_count, HINT32 page_count, GWM_LIST_ITEMS item_array);

extern ONDK_Result_t		ONDK_GWM_Horizontal_List_Create(HINT32 id, ONDK_Rect_t *rect, HCHAR *title, HINT32 cell_width,
							        int item_count, int page_count, GWM_LIST_ITEMS item_array);

extern void					ONDK_GWM_List_GetProperty(LIST_OBJECT object, HINT32 *out_total, HINT32 *out_start, HINT32 *out_curr, HINT32 *out_select, HINT32 *out_pagecount);
extern void					ONDK_GWM_List_ChangeGetItemProc (LIST_OBJECT object, GWM_GetItem_t pfGetItemProc);
extern void					ONDK_GWM_List_ChangeItems(LIST_OBJECT object, HINT32 total, GWM_LIST_ITEMS item_array);
extern void					ONDK_GWM_List_ChangeNumOfPage (LIST_OBJECT object, HINT32 page_count);
extern ONDK_Result_t		ONDK_GWM_List_GetTotalItem(HINT32 id, HINT32 *number);
extern void					ONDK_GWM_List_GetItemProperty(LIST_OBJECT list, HINT32 *item_height);			// TBD
extern void					ONDK_GWM_List_ChangeItemHeight(LIST_OBJECT list, HINT32 item_height);			// TBD
extern HINT32				ONDK_GWM_List_GetCurrentIndex(HINT32 id);
extern HINT32				ONDK_GWM_List_GetSelectedIndex(HINT32 id);
extern HINT32 				ONDK_GWM_List_GetStartIndex(HINT32 id);
extern HINT32				ONDK_GWM_List_GetCellHeight (HINT32 id);
extern HINT32				ONDK_GWM_List_GetCellWidth (HINT32 id);
extern HINT32				ONDK_GWM_List_SetCellWidth (HINT32 id, HINT32 item_width);
extern HINT32				ONDK_GWM_List_GetNumOfPage (HINT32 id);
extern ONDK_Result_t		ONDK_GWM_List_SetCurrentIndex(HINT32 id, HINT32 nIndex);
extern ONDK_Result_t		ONDK_GWM_List_SetSelectedIndex(HINT32 id, HINT32 nIndex);
extern ONDK_Result_t		ONDK_GWM_List_SetStartIndex(HINT32 id, HINT32 nIndex);
extern ONDK_Result_t		ONDK_GWM_List_SetQuickDraw(HINT32 id, ONDK_Rect_t *pFirstRect, ONDK_Rect_t *pScrollRect, HINT32 interval);
extern ONDK_Result_t		ONDK_GWM_List_SetTitle(HINT32 id, HCHAR *title);
extern HCHAR*				ONDK_GWM_List_GetTitlePtr(LIST_OBJECT list);
extern GWM_LIST_ITEMS		ONDK_GWM_List_GetArrayPtr(LIST_OBJECT list);
extern ONDK_Result_t		ONDK_GWM_List_SetItemDisable(HINT32 id, HCHAR *bDisabled);
extern ONDK_Result_t		ONDK_GWM_List_SetCircularMove(HINT32 id, unsigned char bCircularList);
extern ONDK_Result_t		ONDK_GWM_List_SetInvalidateOffset(HINT32 id,HUINT32 offset_y);
extern ONDK_Result_t		ONDK_GWM_List_GetNextEnabledItem (HINT32 nListObjId, HINT32 step, HINT32 *pnNextItem);
extern HUINT32				ONDK_GWM_List_GetStatus(LIST_OBJECT list);
extern unsigned char*		ONDK_GWM_List_GetItemDisable(LIST_OBJECT list);
extern void					ONDK_GWM_List_FreeList (HINT32 nObjId);
extern void					ONDK_GWM_List_PageUpPage(HINT32 nObjId);
extern void					ONDK_GWM_List_PageDownPage(HINT32 nObjId);
extern void					ONDK_GWM_List_ScrollUp(HINT32 nObjId);
extern void	                ONDK_GWM_List_ScrollDown(HINT32 nObjId);

#if 0
extern void					ONDK_GWM_HorList_GetProperty(HOR_LIST_OBJECT object, HINT32 *out_total, HINT32 *out_start, HINT32 *out_curr, HINT32 *out_select);
extern GWM_LIST_ITEMS		ONDK_GWM_HorList_GetArrayPtr(HOR_LIST_OBJECT list);

extern ONDK_Result_t		ONDK_GWM_HorList_GetTotalItem(HINT32 id, HINT32 *number);
extern ONDK_Result_t		ONDK_GWM_HorList_SetCircularMove(HINT32 id, unsigned char bCircularList);
extern ONDK_Result_t		ONDK_GWM_HorList_SetPageMove(HINT32 id, unsigned char bPageScroll);
extern ONDK_Result_t		ONDK_GWM_HorList_SetCurrentIndex(HINT32 id, HINT32 nIndex);
extern int					ONDK_GWM_HorList_GetCurrentIndex(HINT32 id);
extern int					ONDK_GWM_HorList_GetStartIndex(HINT32 id);
extern void					ONDK_GWM_HorList_ChangeItems(HOR_LIST_OBJECT object, HINT32 total, GWM_LIST_ITEMS item_array);
extern int					ONDK_GWM_HorList_GetNumOfPage (HINT32 id);
extern ONDK_Result_t		ONDK_GWM_HorList_SetColorById (HINT32 Id, ONDK_Color_t focus_face, ONDK_Color_t disabled_shadow, ONDK_Color_t hilighted_text,
										ONDK_Color_t inactive_light, ONDK_Color_t inactive_darkshadow, ONDK_Color_t inactive_face);
#endif

extern ONDK_Result_t		ONDK_GWM_List_SetFontHandle(HINT32 Id, ONDK_FontId_t fontId, HINT32 fontSize);
extern ONDK_Result_t		ONDK_GWM_List_SetColorById (HINT32 Id, ONDK_Color_t textColor, ONDK_Color_t focusTextColor, ONDK_Color_t disableTextColor, ONDK_Color_t cursorColor);
extern ONDK_Result_t		ONDK_GWM_List_GetFontHandleObj(LIST_OBJECT object, ONDKFontHandle *handle);
extern ONDK_Result_t		ONDK_GWM_List_SetOffset(HINT32	id, HINT32	textOffset_X, HINT32	textOffset_Y, HINT32 CursorOffset);
extern ONDK_Result_t		ONDK_GWM_List_SetCursorEnable(HINT32 Id, HBOOL enable);
extern HINT32				ONDK_GWM_List_GetLineInterval(HINT32 id);
extern ONDK_Result_t		ONDK_GWM_List_SetLineInterval(HINT32 id, HINT32 lLineInterval);

/* MULTI */
extern ONDK_Result_t	ONDK_GWM_COM_Multi_Create(HINT32 id, ONDK_Rect_t *rect);
extern ONDK_Result_t	ONDK_GWM_COM_Multi_SetContents(HUINT32 id, HUINT8 *normalPath, HUINT8 *focusPath, HUINT8 *leftStr, HUINT8 *rightStr, HUINT32 offset);
extern ONDK_Result_t	ONDK_GWM_COM_Multi_ChangeFocusImage(HINT32 id, HCHAR *imagePath);
extern ONDK_Result_t	ONDK_GWM_COM_Multi_ChangeNormalImage(HINT32 id, HCHAR *imagePath);
extern ONDK_Result_t	ONDK_GWM_COM_Multi_GetImage(HINT32 id, HCHAR **normal);
extern ONDK_Result_t	ONDK_GWM_COM_Multi_GetImageByObj(MULTI_OBJECT obj, HCHAR **normal);
extern ONDK_Result_t	ONDK_GWM_COM_Multi_SetFocusColor(HINT32 id, ONDK_Color_t over, ONDK_Color_t normal, ONDK_Color_t click, ONDK_Color_t disable);
extern ONDK_Result_t	ONDK_GWM_COM_Multi_SetFontColor(HINT32 id, ONDK_Color_t fontColor, ONDK_Color_t fontfocus);
extern ONDK_Result_t	ONDK_GWM_COM_Multi_SetStatus(HINT32 id, HINT32 push_status);
extern ONDK_Result_t	ONDK_GWM_COM_Multi_GetStr(HINT32 id, HUINT8 **leftStr, HUINT8 **rightStr);
extern ONDK_Result_t	ONDK_GWM_COM_Multi_ChangeLeftStr (HINT32 id, HCHAR *szNewText);
extern ONDK_Result_t	ONDK_GWM_COM_Multi_ChangeRightStr (HINT32 id, HCHAR *szNewText);
extern ONDK_Result_t	ONDK_GWM_COM_Multi_ChangeCenterStr (HINT32 id, HCHAR *szNewText);
extern HUINT32			ONDK_GWM_COM_Multi_GetStatusByObj(MULTI_OBJECT obj);
extern ONDK_Result_t	ONDK_GWM_COM_Multi_SetFocusFlag(HUINT32 id, HBOOL flag);
extern ONDK_Result_t	ONDK_GWM_COM_Multi_SetFontHandle(HINT32 Id, ONDK_FontId_t fontId, HINT32 fontSize);
extern ONDK_Result_t	ONDK_GWM_COM_Multi_SetAlign(HINT32 id, ONDKSurfaceTextFlags align);
extern ONDK_Result_t	ONDK_GWM_COM_Multi_ChangeImagePosition (HINT32 id, 	HINT32	x,	HINT32	y);
extern ONDK_Result_t	ONDK_GWM_COM_Multi_ChangeStrPosition (HINT32 id, 	HINT32	x,	HINT32	y);

#if 0
#ifdef CONFIG_UI_ALVIN
extern ONDK_Result_t GWM_MList_Create(int id, ONDK_Rect_t *rect, int item_count,
		int num_rows, int num_cols, int current_index);
extern int GWM_MList_GetCurrentIndex(int id);
extern ONDK_Result_t GWM_MList_SetCurrentIndex(int id, int val);
extern int GWM_MList_GetFirstVisibleIndex(int id);
extern int GWM_MList_GetLastVisibleIndex(int id);
extern int GWM_MList_GetItemCount(int id);
extern int GWM_MList_GetRowCount(int id);
extern int GWM_MList_GetColumnCount(int id);
extern ONDK_Result_t GWM_MList_SetListData(int id, int item_count, int num_rows, int num_cols, int current_index);
extern ONDK_Result_t GWM_MList_SetWrapAroundMode(int id, HBOOL up, HBOOL down, HBOOL left, HBOOL right);
extern void GWM_MList_FreeList(int id);
#endif
#endif


/* Input */
extern ONDK_Result_t			ONDK_GWM_COM_Input_Create(HINT32 id, ONDK_Rect_t *rect, HCHAR *title, HINT32 digit, HCHAR *text);
extern void						ONDK_GWM_COM_Input_GetPropertyByObject(INPUT_OBJECT object, HINT32 *status, HINT32 *digit, HINT32 *pos, HCHAR **title, HCHAR **text);
extern HCHAR *				    ONDK_GWM_COM_Input_GetTextPtrFromByObject(INPUT_OBJECT inobj);
extern HCHAR *				    ONDK_GWM_COM_Input_GetTitlePtrFromByObject(INPUT_OBJECT inobj);
extern HCHAR *				    ONDK_GWM_COM_Input_GetText(HINT32 ID);
extern ONDK_Result_t			ONDK_GWM_COM_Input_SetTitle(HINT32 ID, HCHAR *title);
extern ONDK_Result_t			ONDK_GWM_COM_Input_SetText(HINT32 ID, HINT32 length, HCHAR *string);
extern ONDK_Result_t			ONDK_GWM_COM_Input_SetPosition(HINT32 ID, HINT32 pos);
extern ONDK_Result_t			ONDK_GWM_COM_Input_SetPasswordMode(HINT32 ID, HINT32 mode);
extern HINT32					ONDK_GWM_COM_Input_IsPasswordModeByObject(INPUT_OBJECT inobj);
extern HINT32					ONDK_GWM_COM_Input_GetDigitByObject(INPUT_OBJECT inobj);
extern HINT32					ONDK_GWM_COM_Input_IsFocusedByObject(INPUT_OBJECT inobj);
extern HINT32					ONDK_GWM_COM_Input_GetCurrentPositionByObject(INPUT_OBJECT inobj);
extern ONDK_Result_t			ONDK_GWM_COM_Input_SetStringMode(HINT32 ID, HINT32 mode);
extern ONDK_Result_t			ONDK_GWM_COM_Input_SetStringModeWithFormat(HINT32 ID, HINT32 mode, HCHAR *format);
extern HINT32					ONDK_GWM_COM_Input_IsPasswordModeByObject(INPUT_OBJECT inobj);
extern HUINT8				    *ONDK_GWM_COM_Input_GetFixedPointByObject(INPUT_OBJECT inobj);
extern HCHAR					*ONDK_GWM_COM_Input_GetFormatByObject(INPUT_OBJECT inobj);
extern HBOOL 				    ONDK_GWM_COM_Input_IsFixedPoint(HUINT8 *aucFixedPoint, HINT32 pos);
extern ONDK_Result_t			ONDK_GWM_COM_Input_SetItemDimension (HINT32 id, HINT32 xoffset, HINT32 width);
extern ONDK_Result_t			ONDK_GWM_COM_Input_GetItemDimension (HINT32 id, HINT32 *xoffset, HINT32 *width);
extern HBOOL				    ONDK_GWM_COM_Input_IsInputtingNowByObject (INPUT_OBJECT inobj);
extern HBOOL				    ONDK_GWM_COM_Input_SetInputtingByObject (INPUT_OBJECT inobj, HBOOL bInputting);

/* Status */
extern ONDK_Result_t			ONDK_GWM_COM_Status_Create(HINT32 id, ONDK_Rect_t *rect, HUINT32 max, HUINT32 value);
extern ONDK_Result_t			ONDK_GWM_COM_Status_SetValue(HINT32 id, HUINT32 new_value);
extern ONDK_Result_t			ONDK_GWM_COM_Status_GetValue(HINT32 id, HUINT32 *value);
extern ONDK_Result_t			ONDK_GWM_COM_Status_GetProperty(STATUS_OBJECT obj, HUINT32 *max, HUINT32 *value, ONDK_Color_t *ulBackColor, ONDK_Color_t *ulForeColor);
extern ONDK_Result_t			ONDK_GWM_COM_Status_SetText (HINT32 id, HUINT8 *text);
extern HUINT8 *				    ONDK_GWM_COM_Status_GetText (HINT32 id);
extern ONDK_Result_t			ONDK_GWM_COM_Status_SetColor(HINT32 id, ONDK_Color_t ulBackColor, ONDK_Color_t ulForeColor);

/* formatted text list */
extern ONDK_Result_t			ONDK_GWM_FMT_Create(HINT32 ID, ONDK_Rect_t *rect, HINT32 attrib, HINT32 pageNum);
extern ONDK_Result_t			ONDK_GWM_FMT_ChangeItemDrawMethod(HINT32 ID, FMT_DrawItem_Func method);
extern ONDK_Result_t			ONDK_GWM_FMT_ChangeNotifyMethod(HINT32 ID, FMT_Notify_Func method);
extern ONDK_Result_t			ONDK_GWM_FMT_ChangeScrollDrawMethod(HINT32 ID, FMT_DrawScroll_Func method);
extern void						ONDK_GWM_FMT_GetProperty(FMT_OBJECT obj, HUINT32 *attrib, HUINT32 *startIndex, HUINT32 *pageNum, HUINT32 *itemNum);
extern ONDK_Result_t			ONDK_GWM_FMT_MakeAddItem(HINT32 ID, HCHAR *item_str, ONDK_Rect_t *pRect, Text_Property_t *pstTextProperty);
extern ONDK_Result_t			ONDK_GWM_FMT_AddItem(HINT32 ID, HCHAR *item_str, Text_Property_t *pstTextProperty);
extern ONDK_Result_t			ONDK_GWM_FMT_SelectItem(HINT32 ID, HINT32 selIndex);
extern ONDK_Result_t 			ONDK_GWM_FMT_GetItem(HINT32 ID, HINT32 item_index, HCHAR **outString);
extern ONDK_Result_t			ONDK_GWM_FMT_Delete(HINT32 ID);
extern ONDK_Result_t			ONDK_GWM_FMT_SetStartIndex(HINT32 ID, HINT32 startIndex);
extern ONDK_Result_t			ONDK_GWM_FMT_SetPageNum(HINT32 ID, HINT32 pageNum);
extern ONDK_Result_t			ONDK_GWM_FMT_ProcessKey(HINT32 ID, HINT32 key);
extern void 				    ONDK_GWM_FMT_ScrollUp(HINT32 objID);
extern void 				    ONDK_GWM_FMT_ScrollDown(HINT32 objID);
extern void 				    ONDK_GWM_FMT_ScrollUpPage(HINT32 objID);
extern void 				    ONDK_GWM_FMT_ScrollDownPage(HINT32 objID);
extern void 					ONDK_GWM_FMT_ScrollUpPageStrict(HINT32 objID);
extern void 					ONDK_GWM_FMT_ScrollDownPageStrict(HINT32 objID);
extern ONDK_Result_t			ONDK_GWM_FMT_AddnItem(HINT32 ID, HUINT8 *pItemStr, HUINT32 ulLength, Text_Property_t *pstTextProperty);
extern ONDK_Result_t			ONDK_GWM_FMT_MakeCustomTagString(HUINT8 ucMainGenreIndex, HUINT8 *pszRetStr,  HUINT8 *pucLen);
extern ONDK_Result_t			ONDK_GWM_FMT_SetColor(HINT32 ID, ONDK_Color_t inactive_face, ONDK_Color_t hilighted_text,
										ONDK_Color_t frame_light, ONDK_Color_t frame_face, ONDK_Color_t focus_face, ONDK_Color_t disabled_shadow);
#if 0
/* Grid Object */
typedef enum etag_CellAddDirection
{
	eCellAddDirection_None	= 0,


	eCellAddDirection_Horizontal,
	eCellAddDirection_Vertically,


	eCellAddDirection_Max
} CellAddDirection_t;

extern ONDK_Result_t			GWM_GRID_Create(HINT32 ulId, OBJ_CREATE_t *pstObjCreateParam);
extern ONDK_Result_t			GWM_GRID_FreeObjectById(HINT32 ulId);
extern ONDK_Result_t			GWM_GRID_CleanObject(HINT32 ulId);
extern ONDK_Result_t			GWM_GRID_GetNextItem(Handle_t hItem, Handle_t *phNextItem);
extern ONDK_Result_t			GWM_GRID_GetPrevItem(Handle_t hItem, Handle_t *phPrevItem);
extern ONDK_Result_t			GWM_GRID_GetEventItem(Handle_t hSvcItem, Handle_t *phEventItem);
extern ONDK_Result_t			GWM_GRID_GetItemData(Handle_t hItem, void **ppData);
extern ONDK_Result_t			GWM_GRID_GetServiceItemByIndex(HUINT32 ulId, HUINT32 ulSvcIndex, Handle_t *phSvcItem);
extern ONDK_Result_t			GWM_GRID_UpdateFocus(HUINT32 ulId);
extern ONDK_Result_t			GWM_GRID_SelectItem(HINT32 ulId, Handle_t hItem);
extern ONDK_Result_t			GWM_GRID_GetSelectedItem(HINT32 ulId, Handle_t *phSelectedItem);
extern ONDK_Result_t			GWM_GRID_GetItemParamInfo(HUINT32 ulParam, HBOOL *pbSelected, HBOOL *pbFront, HUINT8 *pucDepth, HUINT8 *pucIndex);
extern ONDK_Result_t			GWM_GRID_InsertItem(HINT32 lId, Handle_t hParent, Handle_t hPrevLink, Handle_t hNextLink, void *pParam, Handle_t *phNewItem);
extern ONDK_Result_t			GWM_GRID_DeleteItem(HINT32 lId, Handle_t hDelItem);
extern ONDK_Result_t			GWM_GRID_DeleteEventItems(HINT32 ulId, Handle_t hSvclItem);
extern ONDK_Result_t			GWM_GRID_SetAddDirection(HINT32 lId, CellAddDirection_t eCellAddDi);
extern ONDK_Result_t			GWM_GRID_GetAddDirection(HINT32 lId, CellAddDirection_t *peCellAddDi);
#endif

#if 0
/* Tab */
ONDK_Result_t				GWM_Tab_Create(int id, ONDK_Rect_t *rect, int item_count, int total_width, char **title_array);
void						GWM_Tab_GetProperty(TAB_OBJECT object, int *out_total, int *out_curr, int *out_focus);
#endif
/* Checkbox */

/* Group / Tab */
#if 0
/* Tree */
	// create & destroy
extern ONDK_Result_t		GWM_TREE_Create(HINT32 ulId, OBJ_CREATE_t *pstObjCreateParam);
extern ONDK_Result_t		GWM_TREE_FreeObjectById(HINT32 ulId);

	// insert & remove
extern ONDK_Result_t		GWM_TREE_InsertItem(HINT32 ulId, HTREEITEM hParent, HTREEITEM hPrevLink, HTREEITEM hNextLink, TREEITEM_PARAM_t *pParam, HTREEITEM *phNewItem);
extern ONDK_Result_t		GWM_TREE_RemoveItem(HINT32 ulId, HTREEITEM hItem);

	// get
extern ONDK_Result_t		GWM_TREE_GetTreeObject(HINT32 ulId,	 PTREE_OBJECT *pTreeObject);
extern ONDK_Result_t		GWM_TREE_GetTreeMgr(HINT32 ulId, HTREEMGR *phTreeMgr);
extern ONDK_Result_t		GWM_TREE_GetTreeTotalItemCount(HINT32 ulId, HUINT32 *pulCnt);


extern ONDK_Result_t		GWM_TREE_GetDrawItemInfo(PTREEOBJ_ITEM_DRAWINFO	pItem, HUINT32 *pulIndexAtTotal, HUINT32 *pulIndexAtPage,
											 HUINT32 *pulStatus, HUINT32 *pulDepth, HUINT32 *pulNumOfChild, void **ppLocalData);
extern ONDK_Result_t		GWM_TREE_GetScrollInfo(HINT32 ulId, HUINT16 *pusTotalNumber, HUINT16 *pusSelectedIndexAtTotal);
extern ONDK_Result_t		GWM_TREE_SetLastItemOnLastIdx(HINT32 ulId, HBOOL bAllLayer, HUINT32 ulSelectIndex);
extern ONDK_Result_t		GWM_TREE_GetItemData(HTREEITEM hItem, void **ppData);
extern ONDK_Result_t		GWM_TREE_FindNextItem_InAllItem(HTREEITEM hItem, HTREEITEM *phFindItem);
extern ONDK_Result_t		GWM_TREE_FindItem_ByData(HINT32 ulId, HTREEITEM *phFindItem, void *pTargetData, FP_CompFunc compFunc);
extern ONDK_Result_t		GWM_TREE_FindItem_ByData_InAllItem(HINT32 ulId, HTREEITEM *phFindItem, void *pTargetData, FP_CompFunc compFunc);
extern ONDK_Result_t		GWM_TREE_GetCondMatchItemCount_ByData(HINT32 ulId, HUINT32 *pulCount, void *pTargetData, FP_CompFunc compFunc);
extern ONDK_Result_t		GWM_TREE_GetCondMatchItemCount_ByData_InAllItem(HINT32 ulId, HUINT32 *pulCount, void *pTargetData, FP_CompFunc compFunc);


extern ONDK_Result_t		GWM_TREE_GetFirstChildItem(HINT32 ulId, HTREEITEM *phFirstChildItem);
extern ONDK_Result_t		GWM_TREE_GetDisplayStartItem(HINT32 ulId, HTREEITEM *phItem);
extern ONDK_Result_t		GWM_TREE_SetDisplayStartItem(HINT32 ulId, HTREEITEM hItem);
extern ONDK_Result_t		GWM_TREE_GetRootItem(HINT32 ulId, HTREEITEM *phRootItem);
extern ONDK_Result_t		GWM_TREE_GetChildNum(HTREEITEM hItem, HUINT32 *pulNumOfChild);
extern ONDK_Result_t		GWM_TREE_GetChildItem(HTREEITEM hItem, HTREEITEM *phChildItem);
extern ONDK_Result_t		GWM_TREE_FindNextItem(HTREEITEM hItem, HTREEITEM *phFindItem);
extern ONDK_Result_t		GWM_TREE_FindPrevItem(HTREEITEM hItem, HTREEITEM *phFindItem);

extern ONDK_Result_t		GWM_TREE_FindNextItem_InSameLevel(HTREEITEM hItem, HTREEITEM *phFindItem);
extern ONDK_Result_t		GWM_TREE_FindPrevItem_InSameLevel(HTREEITEM hItem, HTREEITEM *phFindItem);

extern ONDK_Result_t		GWM_TREE_GetIndexOfItem(HINT32 ulId, HTREEITEM hTargetItem, HUINT32 *pulIndex);
extern ONDK_Result_t		GWM_TREE_GetSelectedItem(HINT32 ulId, HTREEITEM *phSelectedItem);

	// functions
extern ONDK_Result_t		GWM_TREE_SelectItem(HINT32 ulId, HTREEITEM hItem);
extern ONDK_Result_t		GWM_TREE_SelectNextItem(HINT32 ulId,  HTREEITEM *phSelectedItem);
extern ONDK_Result_t		GWM_TREE_SelectPrevItem(HINT32 ulId,  HTREEITEM *phSelectedItem);
extern ONDK_Result_t		GWM_TREE_Collapse(HINT32 ulId, HTREEITEM hItem, HBOOL bSubCollapse);
extern ONDK_Result_t		GWM_TREE_Expand(HINT32 ulId, HTREEITEM hItem, HBOOL bSubExpand);
extern ONDK_Result_t		GWM_TREE_IsExpandItem(HINT32 ulId, HTREEITEM hItem, HBOOL *pbExpand);

extern ONDK_Result_t		GWM_TREE_SetObjDisable(HINT32 ulId, HBOOL bDisable);
extern ONDK_Result_t		GWM_TREE_SetOnlyOneItemExpandFlag(HINT32 ulId, HBOOL bDisable);
#endif



#if 0
/* gauge bar */
extern ONDK_Result_t GWM_Gaugebar_Create(HINT32 iId,
											ONDK_Rect_t *psRect,
											HINT8 cFocusType,
											HINT8 *pcTitle,
											HINT32 iMin,
											HINT32 iMax,
											HINT32 iValue,
											HLONG ulItemAddr);
extern HUINT32				GWM_Gaugebar_GetStatus(PGAUGEBAR_OBJECT pGbObj);
extern HINT8* 				GWM_Gaugebar_GetTitlePtr(PGAUGEBAR_OBJECT pGbObj);
extern void 				GWM_Gaugebar_GetProperty(PGAUGEBAR_OBJECT pGbObj, HINT8 *pcFocusType, HINT32 *piMin, HINT32 *piMax, HINT32 *piValue);
extern HLONG 				GWM_Gaugebar_GetItemsAddr(PGAUGEBAR_OBJECT pGbObj);
extern ONDK_Result_t 		GWM_Gaugebar_SetValue(HINT32 iId, HINT32 iValue);
extern ONDK_Result_t			GWM_Gaugebar_ChangeProperty(HINT32 iId,
																	HINT8 cFocusType,
																	HINT8 *pcTitle,
																	HINT32 iMin,
																	HINT32 iMax,
																	HINT32 iValue,
																	HLONG ulItemAddr);


extern ONDK_Result_t	GWM_Picture_SetDefaultDraw (/*GWM_ObjDraw_t*/void * defaultDraw);
extern ONDK_Result_t	GWM_Picture_Create(int id, ONDK_Rect_t *rect, HINT32 nImgEnum, OSD_BmpBlendMode_t mode, PictureCaption_t *pCaption);
extern ONDK_Result_t	GWM_Picture_SetImage (PICTURE_OBJECT label, HINT32 nImage);
extern ONDK_Result_t	GWM_Picture_GetImage (PICTURE_OBJECT label, HINT32 *pnImage);
extern ONDK_Result_t	GWM_Picture_SetCaption (PICTURE_OBJECT label, PictureCaption_t *pCaption);
extern ONDK_Result_t	GWM_Picture_GetCaption (PICTURE_OBJECT label, PictureCaption_t *pCaption);

extern ONDK_Result_t	GWM_StyledText_Create(HINT32 id, ONDK_Rect_t *rect, HINT32 baseline);
extern ONDK_Result_t	GWM_StyledText_SetStyle (HINT32 id, const HUINT8 *szType, ...);
extern ONDK_Result_t	GWM_StyledText_SetText (HINT32 id, const HUINT8 *text);
extern ONDK_Result_t	GWM_StyledText_ShowText (HINT32 id, HBOOL bShow);
extern ONDK_Result_t	GWM_StyledText_Clear (HINT32 id);
extern ONDK_Result_t GWM_StyledText_SetScrollDraw (HINT32 id, GWM_StyledText_DrawScroll_t pfnDraw, HINT32 width);
#endif
#endif	//__GUIOBJ_H__
/* End of File. ---------------------------------------------------------- */


