/****************************************************************************
*
* File Name	: osd.h
*
* Description	:
*
*
 * Revision History	:
*
* Date			Modification				Name
* -----------	-------------------------------------	------------------
* 2008/11/05	File Created.				JK Baek
* 2009/03/12	File reCreated.				wjmoh
*
****************************************************************************/

#ifndef	__OSD_H__
#define	__OSD_H__

#include <octo_common.h>

/*////////////// Graphics Device Definition.*/
#define	TEXT_ALIGN_LEFT			0x0100
#define	TEXT_ALIGN_RIGHT		0x0200
#define	TEXT_ALIGN_CENTER		0x0400
#define	TEXT_ALIGN_H_MASK		0x0F00

#define	TEXT_ALIGN_TOP			0x1000
#define	TEXT_ALIGN_BOTTOM		0x2000
#define	TEXT_ALIGN_MIDDLE		0x4000
#define	TEXT_MULTILINED			0x8000
#define	TEXT_ALIGN_V_MASK		0xF000

#define	OSD_720X576_WIDTH		720
#define OSD_720X576_HEIGHT		576
#define	OSD_1280X720_WIDTH		1280
#define	OSD_1280X720_HEIGHT		720
#define	OSD_1920X1080_WIDTH		1920
#define	OSD_1920X1080_HEIGHT	1080

#define MAX_8BITS_PALETTE	256


typedef	enum tagOSDResult {
	OSDERR_NO_ERROR	= ERR_OK,
	OSDERR_ERROR,
	OSDERR_INITIAL_FAILED,
	OSDERR_ALLOC_INST_FAILED,
	OSDERR_ALLOC_EXCEED_INST_MAX,
	OSDERR_FREE_INST_FAILED,

	OSDERR_SETSIZE_FAIL,
	OSDERR_ALLOC_FAIL,

	// argument 관련 error들
	OSDERR_SOURCE_NULL,
	OSDERR_TARGET_NULL,
	OSDERR_INPUTPARAM_INVALID,
	OSDERR_PAL_SETTING_FAIL,

	OSDERR_NEW_ERROR_CODE,
	// ...

	/* area */
	OSDERR_AREA_IS_INVALID,
	OSDERR_AREA_X1_IS_INVALID,
	OSDERR_AREA_Y1_IS_INVALID,
	OSDERR_AREA_X2_IS_INVALID,
	OSDERR_AREA_Y2_IS_INVALID,

	OSDERR_AREA_X_IS_OUTOFSCREEN_START,
	OSDERR_AREA_Y_IS_OUTOFSCREEN_START,
	OSDERR_AREA_X_IS_OUTOFSCREEN_END,
	OSDERR_AREA_Y_IS_OUTOFSCREEN_END,
	/* rect */
	OSDERR_RCCT_IS_INVALID,
	OSDERR_RECT_X_IS_INVALID,
	OSDERR_RECT_Y_IS_INVALID,
	OSDERR_RECT_W_IS_INVALID,
	OSDERR_RECT_H_IS_INVALID,

	OSDERR_SCREEN_NOT_EXIST,

	/* image */
	OSDERR_IMAGE_WRONG_TYPE,


	// Last Item in the GWM Error Code
	OSDERR_LAST
} OSD_Result_t;


typedef	HUINT32			OSD_Color_t;		/* True color OSD */
typedef	HUINT32	 		OSD_PixelFormat_t;
typedef HUINT32			OSD_Screen_Handle_t;
typedef HUINT32			OSD_Screen_Buffer_t;
typedef HUINT32 		OSD_Image_Handle_t;


/** @brief application screen type */
typedef enum
{
	eOSDScreen_DrawableBuf = 0,
	eOSDScreen_MainOsd,			/**< main OSD screen */
	eOSDScreen_SWMergeTarget,				/**< blending screen */
	eOSDScreen_Aux,				/**< aux OSD screen, depend on its output requirement */
	eOSDScreen_AuxBG,				/**< background (JPEG or still) screen */
	eOSDScreen_Data,					/**< multi-purpose OSD screen such as a filter */
	eOSDScreen_DataBG,
	eOSDScreen_DataBG2,
	eOSDScreen_MAX
} OSD_ScreenType_t;

typedef enum
{
	eImage_JPEG			= 0,
	eImage_PNG,
	eImage_BMP,
	eImage_TIFF,
	eImage_PCX,
	eImage_Unknown
} OSD_ImageType_t;

typedef struct tagGfxPoint {
	HINT32 x;
	HINT32 y;
} OSD_Point_t;

typedef struct tagGfxSize {
	HUINT32 width;
	HUINT32 height;
} OSD_Size_t;

typedef	struct	tagGfxRect {
	HINT32		x, y, w, h;		// 넓이와 높이로 객체화.
} OSD_Rect_t;

typedef	struct	tagGfxArea {
	HINT32		x1, y1, x2, y2;	// 시작점과 끝점으로 영역을 저장.
} OSD_Area_t;

typedef struct tagGfxAreaList
{
	OSD_Area_t area;
	struct tagGfxAreaList* next;
} OSD_Area_List_t;

typedef	struct	tagGfxScreen
{
	OSD_Screen_Handle_t	handle;		/* Normally It would be handle or pointer to graphics device structure. */
	HUINT32				ulIndex;
	OSD_ScreenType_t	type;
	HUINT32		bpp;		/* TrueColor or IndexedColor */

	OSD_Color_t		*clut;		/* CLUT if bpp = 8 */
	void			*func_table;		/* 구조체 내부의 멤버들을 숨기기 위해 void 로 선언하였음 */
	HINT32				clip_sx, clip_sy, clip_ex, clip_ey;
	OSD_Rect_t		ScreenRect;
} OSD_Screen_t;

/* OSD_Screen_t를 상속받았다. */
typedef	struct	tagGfxBuffer
{
	/* tagGfxScreen */
	HUINT32		handle;
	OSD_ScreenType_t	type;
	HUINT32		bpp;

	OSD_Color_t		*clut;
	void			*func_table;		/* 구조체 내부의 멤버들을 숨기기 위해 void 로 선언하였음 */
	HINT32				clip_sx, clip_sy, clip_ex, clip_ey;
	OSD_Rect_t		BufferRect;

	/* additional */
	HUINT8	*buffer;
	OSD_Size_t		BufSize;
	HUINT32	width;
	HUINT32	height;
	HUINT32	pitch;
} OSD_DrawableBuf_t;

typedef struct tagGfxImage
{
	OSD_Screen_Handle_t	handle;			/* Normally It would be handle or pointer to image surface structure. */
	OSD_ImageType_t		type;
	HUINT32		bpp;

	OSD_Rect_t		ImageRect;
	OSD_Size_t		ImageSize;
	HUINT32			width;			/* width that should be scaled */
	HUINT32			height;			/* height that should be scaled */
	HUINT32			transparency;
	HUINT32			reserved;		/* WIN32 usage (CxImage index) */
} OSD_Image_t;

typedef struct tagOSDImageResourceInfo
{
	OSD_ImageType_t type;
	HUINT8 ucFileBytePadding;		/* Image 파일의 Align 을 맞추기 위한 padding */
	HUINT32 ulImageSize;
	HUINT16 usWidth;
	HUINT16 usHeight;
	HUINT16 usBytesPerLine;
	HUINT8 ucAlpha;
	HUINT8* pucImageData;
} OSD_Image_Resource_Info_t;

/* system fonts ID enumeration */
typedef enum {
	eFont_SystemNormal		= 0,
	eFont_SystemFixed,
	eFont_SystemBold,

	eFont_IdMax
} OSD_FontId_t;

typedef enum tagGfxFont_DrawType {
	eOSD_Font_DrawType_Static,
	eOSD_Font_DrawType_AutoAbbr,
	eOSD_Font_DrawType_AutoMultiLine
} OSD_Font_DrawType_t;

typedef HINT32		OSD_Font_Baseline_t;
typedef HINT32		OSD_Font_linespace_t;

typedef enum {
	eFontAlign_Left,
	eFontAlign_Center,
	eFontAlign_Right
} OSD_Font_Align_t;

/*
typedef struct tagGfxFontDrawMask {
	HINT32		bpp;
	HBOOL		bIsAbsoluteCoord;
	HINT32		x, y;
	HINT32		w, h;
	HUINT32		*pMaskBuffer;
} OSD_Font_DrawMask_t;
*/

typedef OSD_DrawableBuf_t	OSD_Font_DrawMask_t;
typedef OSD_DrawableBuf_t	*OSD_Font_PDrawMask_t;

typedef HUINT32	Font_handle_t;

#include <isosvc.h>

typedef struct tagGfxStringData {
	eHxCP_CodePage_e eEncoding;
	HINT32			strLength;
	CH_POINTER		strData;
} OSD_StringInfo_t;

typedef struct tagGfxFontBasicInfo {
	OSD_FontId_t		fontId;
	HINT32				fontSize;
	HINT32				fontWidthRatio;
} OSD_CheckFontInfo_t;

typedef struct tagGfxDrawFontInfo {
	OSD_FontId_t		fontId;
	HINT32				fontSize;
	HINT32				fontWidthRatio;

	OSD_Color_t			fontColor;
	union {
		OSD_Rect_t	fontBaseRect;
		struct
		{
			OSD_Point_t			startPoint;
			OSD_Size_t			rectSize;
		} rect;
	} drawRect;

	OSD_Font_Align_t	strAlign;
	OSD_Font_Baseline_t	strBaseline;
	OSD_Font_linespace_t	strLinespace;
	OSD_Font_DrawType_t	eDrawType;
	OSD_Font_DrawMask_t	*pDrawMask;
} OSD_DrawFontInfo_t;

/* stock image item definition */
typedef struct tagGfxStockImage
{
	OSD_Screen_Buffer_t		hBuffer;			/* raw bitmap buffer*/
	HUINT32			width, height;
	HUINT8			filename[128];
	HUINT32			ulStockImgId;
	HUINT32			opt;				/* attribute */
} OSD_StockImage_t;


#define		DEFAULT_FONT_WIDTHRATIO		100

/* font definition */
typedef	struct tagGfxFont
{
	/* basic attribute */
	OSD_FontId_t		id;				/* font family & style identifier */
	HINT32			nInstanceIdx;
	HBOOL			bIsUsing;

	HINT32		size;		/**< point */
	HINT32		width;		/**< point */
	HINT32		height;		/**< ascender + dscender */
	HINT32		widthRatio;

	HINT32		ascender;
	HINT32		descender;

	HINT32		refCount;

	/* font data : 3rd party's own data block */
	Font_handle_t	context;
} OSD_Font_t;
//typedef	HUINT32*	OSD_PFont_t;


/** @brief members of composition plane */
/********************************************************/
	/*	plane stacking order (refer to pal_plane)			*/
/*------------------------------------------------------*/
/*	TOP													*/
/*	1. GWM OSD				: HD(1, 2 for double buffer)*/
	/*	2. data					: SD 						*/
	/*	3. auxiliary			: SD 						*/
	/*	4. composition			: HD						*/
/*	VIDEO 												*/
/********************************************************/
#if 0
/* 이 enum은 사용하지 않습니다. OSD_ScreenType_t로 대체합니다. */
typedef enum
{
	eOSD_SCREEN			= 0,				/**< application OSD */
	eDATA_SCREEN,							/**< data broadcating's or subtitle/TTX */
	eAUX_SCREEN,							/**< data broadcating's or subtitle/TTX */
	eSWMerge_SCREEN,							/**< auxillary screen, usually for image browser */
	eCOMP_SCREEN,							/**< plane composition screen */
	eBG1_SCREEN,							/**< Background 1 */
	eBG2_SCREEN,							/**< Background 2 */
	NUMBER_OF_SCREEN
} OSD_ScrType_t;
#endif

/** @brief bitmap blending mode */
typedef enum
{
	eBmpBlend_COPY				= 0,			/* copy the source maintaing its alpha */
	eBmpBlend_Ignore_SrcAlpha					/* don't care the source's alpha, so reflect the background */
} OSD_BmpBlendMode_t;




typedef enum
{
	eBlit_COPY					= 0,
	eBlit_BLEND,
	eBlit_BLEND_WITHOUT_PREMULTIPLIEDALPHA			/* SRC OVER rule will be applied without pre-multiplied alpha on Source color */
} OSD_BlitMode_t;


//////////////////////////////////////////////////////
// 기본 OSD 표시영역을 정의한다.
//////////////////////////////////////////////////////

#define OSD_SD_SCREEN_WIDTH 		720
#define OSD_SD_SCREEN_HEIGHT		576


#endif	/*__OSD_H__ */


