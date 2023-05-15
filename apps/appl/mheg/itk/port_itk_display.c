/**
	@file     port_itk_display.c
	@brief    ITK Host Display 모듈.

	Description: ITK Host Display Interface로서 5개의 하위 모듈을 갖는다.
	- Pipeline
	- The Frame Buffer
	- Image
	- Audio Clips
	- Audio/Video Stream
	Module: mw/mheg/itk

	Copyright (c) 2008 HUMAX Co., Ltd.
	All rights reserved.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <vkernel.h>
#include <hlib.h>
#include <hapi.h>
#include <apk.h>

#include "port_itk_main.h"
#include "port_itk_display.h"
#include "port_itk_image.h"
#include "port_itk_audioclip.h"
#include "port_itk_pipeline.h"
#include "port_itk_avstream.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	ENTER_CRITICAL_KEY		HxSEMT_Get(s_stDisplayInfo.ulKeyMaskSemId)
#define	LEAVE_CRITICAL_KEY		HxSEMT_Release(s_stDisplayInfo.ulKeyMaskSemId)
#define	ENTER_CRITICAL_IS		HxSEMT_Get(s_stDisplayInfo.ulImageStillSemId)
#define	LEAVE_CRITICAL_IS		HxSEMT_Release(s_stDisplayInfo.ulImageStillSemId)


#define MAXINT (int)0x7fffffff
#define MININT MAXINT-(int)0xffffffff

#define CHECK_MHEG_AR_CONTROL(x)			((x == ITK_SHAPE_16_9) || (x == ITK_SHAPE_4_3)) ? TRUE : FALSE
#define CHECK_MHEG_VIDEO_SCALED(x,y,w,h)	((x == 0) && (y == 0) && (w == ITK_DISPLAY_WINDOW_W) && (h == ITK_DISPLAY_WINDOW_H)) ? FALSE : TRUE

/* defines capability of display */
/* 1 << 0  : support pipe line
   1 << 1  : can presend HD Video
   1 << 2  : can display HD graphics via pipe line
   1 << 3  : support alpha-numeric keyboarod
   1 << 4  : has programmable backgroud colour
   1 << 5  : has secondard still image plane */
#define ITK_DISPLAY_CAPS			0x00

/*
* Coordinates System
*/
#define MHEG_DISPLAY_OFFSET_X	-1440
#define MHEG_DISPLAY_OFFSET_Y	-1152

#define MHEG_DISPLAY_SCALE_W	1440
#define MHEG_DISPLAY_SCALE_H	1152

/*
* S&T Comment : this will always be called with values 720x576
*/
#define ITK_DISPLAY_WINDOW_W	720
#define ITK_DISPLAY_WINDOW_H	576

/*
* Display Visibility
*/

#define DECOMPOSE_ARGB4444(pixel,a,r,g,b) do {\
		a = ((pixel) & 0xF000) >> 8; \
		r = ((pixel) & 0x0F00) >> 4; \
		g = ((pixel) & 0x00F0); \
		b = ((pixel) & 0x000F) << 4; \
	} while(0)

#define COMPOSE_ARGB4444(a,r,g,b) (\
		((unsigned short)(a) & 0xF0) << 8 | \
		((unsigned short)(r) & 0xF0) << 4 | \
		((unsigned short)(g) & 0xF0) | \
		((unsigned short)(b) & 0xF0) >> 4 \
	)

#define DECOMPOSE_ARGB8888(pixel,a,r,g,b) do {\
		a = ((pixel) & 0xFF000000) >> 24; \
		r = ((pixel) & 0x00FF0000) >> 16; \
		g = ((pixel) & 0x0000FF00) >> 8; \
		b = ((pixel) & 0x000000FF); \
	} while(0)

#define COMPOSE_ARGB8888(a,r,g,b) (\
		((itk_uint32_t)(a) & 0xFF) << 24 | \
		((itk_uint32_t)(r) & 0xFF) << 16 | \
		((itk_uint32_t)(g) & 0xFF) << 8 | \
		((itk_uint32_t)(b) & 0xFF) \
	)

#define COLOR_BLEND(DEST,SRC,ALPHA) \
	(DEST) = (((SRC)-(DEST))*(ALPHA)/255) + (DEST)

#define ALPHA_BLEND(DEST,SRC) \
	(DEST) = ((SRC)+(DEST))-(((SRC)*(DEST))/255)

#define MINnum(x,y) \
	((x)<(y)?(x):(y))

#define MAXnum(x,y) \
	((x)>(y)?(x):(y))

/* macro for a safe call to DirectFB functions */
#ifdef CONFIG_DEBUG
#define DFBCHECK(x...) \
{                                                                \
	DFBResult err = x;                                           \
	if (err != DFB_OK) {                                         \
		HxLOG_Error("ERROR : [DirectFB] %s <%d>:\n\t", __FILE__, __LINE__ ); \
		fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ ); 		\
		DirectFBErrorFatal( #x, err );                           \
	}                                                            \
}
#else
#define DFBCHECK(x)  \
{					\
	   DFBResult err  = x;													  \
	   if (err != DFB_OK) { 									   \
			HxLOG_Error("dfb error."); \
			DirectFBErrorFatal( #x, err );						   \
	   }				\
}
#endif

/*******************************************************************/
/********************      struct         *************************/
/*******************************************************************/
typedef struct
{
	DxRect_t	stInRect;
	HUINT32		ulInRefWidth;
	HUINT32		ulInRefHeight;

	DxRect_t	stOutRect;
	HUINT32		ulOutRefWidth;
	HUINT32		ulOutRefHeight;
} MHEG_PIGRect_t;

typedef struct
{
	MHEG_PIGRect_t			stPigRect;
	DxDfcSelect_e		mhegDfc_4_3;
	DxDfcSelect_e		mhegDfc_16_9;
	itk_display_shape_t	mhegAR;
	HBOOL				bQuarterScreen;
	HBOOL				bPIGFlag;
} MHEG_VIDEOInfo_t;

/*
* Display Scaling & Positioning
*/
typedef struct tagItkDisplayScaling
{
	int decode_x;
	int decode_y;
	int decode_w;
	int decode_h;
	int view_x;
	int view_y;
	int view_w;
	int view_h;
} MHEG_DisplayScaling_t;

typedef struct
{
	HUINT32		ulKeyMaskSemId;
	HUINT32		ulImageStillSemId;

	HINT32		nChangedVolume;
	HxVector_t	*pstKeyMask;
	HxVector_t	*pstKeyMaskBI;
	itk_int32_t	*pnLastKeyMask;
	HINT32		nLastKeyMaskSize;

	HBOOL							bIFrameStarted[MHEG_DISPLAY_PLANE_STILL_MAX];
	MHEG_DFBInfo_t					dfbInfo;
	MHEG_VIDEOInfo_t				videoContext;
	MHEG_DisplayScaling_t			scaling[MHEG_DISPLAY_PLANE_VIDEO_ALL];

	redkey_profile_t				itkProfile;
	itk_display_t					object;
	itk_display_key_callback_t		pfnKeyCallback;
	void*							pfnKeyCallbackContext;
	itk_display_event_callback_t	pfnDisplayCallback;
	void*							pfnDisplayCallbackContext;
	itk_colour_t					stBgColor;
} MHEG_DisplayInfo_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static MHEG_DisplayInfo_t	s_stDisplayInfo;

/*******************************************************************/
/********************      extern functions     ********************/
/*******************************************************************/
extern HBOOL	AP_Mheg_IsExistSubtitle(void);


/*******************************************************************/
/********************      local function       ********************/
/*******************************************************************/
#define ___STATIC_APIS__________________________________________________________

static void		mheg_itk_display_initializeVideoContext(void)
{
	HxSTD_memset(&s_stDisplayInfo.videoContext, 0, sizeof(MHEG_VIDEOInfo_t));

	s_stDisplayInfo.videoContext.mhegDfc_4_3		= eDxDFC_SELECT_UNKNOWN;
	s_stDisplayInfo.videoContext.mhegDfc_16_9		= eDxDFC_SELECT_UNKNOWN;
	s_stDisplayInfo.videoContext.mhegAR = ITK_SHAPE_UND;
	s_stDisplayInfo.videoContext.bQuarterScreen = FALSE;
	s_stDisplayInfo.videoContext.bPIGFlag		= FALSE;
}

static void		mheg_itk_display_initializeInfo(void)
{
	MHEG_DisplayInfo_t	*pstInfo = &s_stDisplayInfo;

	HxSTD_MemSet(pstInfo, 0, sizeof(MHEG_DisplayInfo_t));

	pstInfo->pstKeyMask = HLIB_VECTOR_New(NULL, NULL);
	pstInfo->pstKeyMaskBI = HLIB_VECTOR_New(NULL, NULL);

	pstInfo->stBgColor.red = 0;
	pstInfo->stBgColor.green = 0;
	pstInfo->stBgColor.blue = 0;
	pstInfo->stBgColor.trans = 255;

	mheg_itk_display_initializeVideoContext();
}

/**
 * Convert itk key to DirectFB key
 *
 * @author swkim (2011-08-10)
 *
 * @param pKeysMaskDst
 * @param pKeysMaskDstByBI
 * @param pKeysMaskSrc
 * @param nKeysCount
 *
 * @return HBOOL
 */
static void mheg_itk_displayConvertITK2DFB(HxVector_t *pstKeyMask, HxVector_t *pstKeyMaskBI, const itk_int32_t* pKeysMaskSrc, size_t nKeysCount)
{
	size_t	i;
	HINT32	nDfbKey;

	for (i = 0; i < nKeysCount; i++)
	{
		//HxLOG_Debug("[%d]key value (%ld)\n", i, pKeysMaskSrc[i]);

		switch(pKeysMaskSrc[i])
		{	// TODO: front key
			case ITK_KEY_0:			nDfbKey = DIKS_0;		break;
			case ITK_KEY_1:			nDfbKey = DIKS_1;		break;
			case ITK_KEY_2:			nDfbKey = DIKS_2;		break;
			case ITK_KEY_3:			nDfbKey = DIKS_3;		break;
			case ITK_KEY_4:			nDfbKey = DIKS_4;		break;
			case ITK_KEY_5:			nDfbKey = DIKS_5;		break;
			case ITK_KEY_6:			nDfbKey = DIKS_6;		break;
			case ITK_KEY_7:			nDfbKey = DIKS_7;		break;
			case ITK_KEY_8:			nDfbKey = DIKS_8;		break;
			case ITK_KEY_9:			nDfbKey = DIKS_9;		break;

			case ITK_KEY_RED:			nDfbKey = DIKS_RED;		break;
			case ITK_KEY_BLUE:		nDfbKey = DIKS_BLUE;	break;
			case ITK_KEY_YELLOW:		nDfbKey = DIKS_YELLOW;	break;
			case ITK_KEY_GREEN:		nDfbKey = DIKS_GREEN;	break;

			case ITK_KEY_SELECT:		nDfbKey = DIKS_RETURN;	break; // 시스템에 따라 DIKS_OK 가 될 수 있음
			case ITK_KEY_CANCEL:	nDfbKey = DIKS_EXIT;	break;
			case ITK_KEY_TEXT:		nDfbKey = DIKS_TEXT;	break;

			/* PVR Key */
			case ITK_KEY_INFO:			nDfbKey = DIKS_INFO; 			break;
			case ITK_KEY_STOP:			nDfbKey = DIKS_STOP;			break;
			case ITK_KEY_PLAY:			nDfbKey = DIKS_PLAY; 			break;
			case ITK_KEY_PAUSE:			nDfbKey = DIKS_PAUSE; 			break;
			case ITK_KEY_SKIP_BACK:		nDfbKey = DIKS_PREVIOUS; 		break;
			case ITK_KEY_SKIP_FORWARD:	nDfbKey = DIKS_NEXT; 			break;
			case ITK_KEY_FAST_FORWARD:	nDfbKey = DIKS_FASTFORWARD; 	break;
			case ITK_KEY_REWIND:			nDfbKey = DIKS_REWIND; 		break;

			case ITK_KEY_UP:				nDfbKey = DIKS_CURSOR_UP; 	break;
			case ITK_KEY_DOWN:			nDfbKey = DIKS_CURSOR_DOWN; 	break;
			case ITK_KEY_LEFT:				nDfbKey = DIKS_CURSOR_LEFT; 	break;
			case ITK_KEY_RIGHT:			nDfbKey = DIKS_CURSOR_RIGHT; 	break;

			case ITK_KEY_GUIDE:			nDfbKey = DIKS_EPG; 			break;

			/* CANVAS Key */
			case ITK_KEY_HELP:			nDfbKey = DIKS_HELP; 			break;

			default:
				nDfbKey = DIKS_NULL;
				break;
		}

		if (nDfbKey != DIKS_NULL)
		{
			HLIB_VECTOR_Add(pstKeyMask, (void*)nDfbKey);
			if (nDfbKey != DIKS_GREEN) // RCT
				HLIB_VECTOR_Add(pstKeyMaskBI, (void*)nDfbKey);
		}
	}
}

/**
 * flip window
 *
 * @author swkim (2013-06-21)
 *
 * @param[in] pSurface : frame buffer
 * @param[in] pRegion : flip region
 * @param[in] flipFlags : flip flags
 *
 * @return HBOOL
 */
static void mheg_itk_displayFlipWindow(IDirectFBSurface* pSurface, DFBRegion* pRegion, DFBSurfaceFlipFlags flipFlags)
{
	HINT32 nWndId = eAWINDOW_NOT_HANDLED;

	if (pSurface == s_stDisplayInfo.dfbInfo.dfbSurface)
	{
		nWndId = eAWINDOW_MHEGPLAYER;
	}
	else if (pSurface == s_stDisplayInfo.dfbInfo.dfbStillSurface)
	{
		nWndId = eAWINDOW_MHEGPLAYER_IMG_STILL;
	}

	DFBCHECK(pSurface->Flip(pSurface, pRegion, flipFlags));

	UNUSED(nWndId);
}

/**
 *Check for the presence of key input
 *
 * @author swkim (2013-06-25)
 */
static HBOOL mheg_itk_displayIsAvailableKey(HINT32 key_symbol)
{
	ENTER_CRITICAL_KEY;

	if (HLIB_VECTOR_IndexOf(s_stDisplayInfo.pstKeyMask, (const void*)key_symbol) >= 0)
	{
		LEAVE_CRITICAL_KEY;

		return TRUE;
	}

	if(key_symbol==DIKS_BACK)
	{
		LEAVE_CRITICAL_KEY;
		return TRUE;
	}

	LEAVE_CRITICAL_KEY;

	return FALSE;
}

/**
 * MHEG Key process task
 *
 * @author yjlim (2012-03-07)
 */
static void mheg_itk_displayKeyProcessTask(void *args)
{
	HInputEvent_t	hEvent;

	HInputRetType_e	eRetType = eHINPUTRET_NONE;

	while (1)
	{
		if (HAPI_GetInputEvent(eAWINDOW_MHEGPLAYER, &hEvent, -1) == TRUE)
		{
			HxLOG_Debug("Key pressed [%d - %d]\n", hEvent.type, hEvent.key_symbol);
			switch  (hEvent.type)
			{
				case DIET_KEYPRESS:
					PORT_ITK_PostMsgEx(eMHEG_KEY_DOWN, 0, 0, 0, 0, 0, (void *)&hEvent, sizeof(HInputEvent_t));
					break;

				case DIET_KEYRELEASE:
					eRetType = eHINPUTRET_PASS;
					if (mheg_itk_displayIsAvailableKey(hEvent.key_symbol) == TRUE)
					{
						eRetType = eHINPUTRET_CONSUMED;
					}
					HAPI_ReturnKeyEvent(eAWINDOW_MHEGPLAYER, eRetType, &hEvent);

					break;

				default:
					HAPI_ReturnKeyEvent(eAWINDOW_MHEGPLAYER, eHINPUTRET_PASS, &hEvent);
					break;
			}
		}
	}
}

static HERROR mheg_itk_displayFillSurface(IDirectFBSurface *pSurface, HUINT8 red, HUINT8 green, HUINT8 blue, HUINT8 trans)
{
	DFBCHECK(pSurface->SetColor(pSurface, red, green, blue, trans));
	DFBCHECK(pSurface->SetDrawingFlags(pSurface, DSDRAW_NOFX));
	DFBCHECK(pSurface->SetPorterDuff(pSurface, DSPD_SRC));
	DFBCHECK(pSurface->FillRectangle(pSurface, 0, 0, ITK_DISPLAY_OSD_HD_W, ITK_DISPLAY_OSD_HD_H));

	mheg_itk_displayFlipWindow(pSurface, NULL, DSFLIP_BLIT);
	return ERR_OK;
}

/**
 * Returns a bitfield of display capabilities
 *
 * @author swkim (2011-08-09)
 *
 * @param thiz[in] The display to change
 *
 * @return itk_display_caps_t A bitfield built from the values of itk_display_caps_t
 */
static itk_display_caps_t mheg_itk_port_displayGetCaps(struct itk_display_t* thiz)
{
	int	nCaps = 0;

	PARAM_UNUSED(thiz);

#if defined(CONFIG_3RD_MHEG_UKFREESAT_PROFILE)
	nCaps = (ITK_DISPLAY_CAPS_PIPELINE
			| ITK_DISPLAY_CAPS_HD_VIDEO
			| ITK_DISPLAY_CAPS_HD_GRAPHICS
			| ITK_DISPLAY_CAPS_BACKGROUND
			| ITK_DISPLAY_CAPS_STILL_PLANE
			| ITK_DISPLAY_CAPS_SIMUL_SUBS
			| ITK_DISPLAY_CAPS_IP_STREAMING
			| ITK_DISPLAY_CAPS_NLP_KEYS
			);
#elif defined (CONFIG_3RD_MHEG_UK_PROFILE_HD)
	nCaps = (ITK_DISPLAY_CAPS_HD_VIDEO
			| ITK_DISPLAY_CAPS_BACKGROUND
			| ITK_DISPLAY_CAPS_SIMUL_SUBS
			| ITK_DISPLAY_CAPS_PIPELINE
			| ITK_DISPLAY_CAPS_HD_GRAPHICS
			| ITK_DISPLAY_CAPS_NLP_KEYS
			| ITK_DISPLAY_CAPS_IP_STREAMING
			| ITK_DISPLAY_CAPS_IPS_BUFFER_RESTRICTION
			| ITK_DISPLAY_CAPS_IPS_BUFFERING
			| ITK_DISPLAY_CAPS_IPS_ENCRYPTION
			| ITK_DISPLAY_CAPS_IPS_HD
			);
#elif defined (CONFIG_3RD_MHEG_AUSTRALIA_PROFILE)
	nCaps = (ITK_DISPLAY_CAPS_HD_VIDEO
			| ITK_DISPLAY_CAPS_SIMUL_SUBS
			| ITK_DISPLAY_CAPS_PIPELINE
			| ITK_DISPLAY_CAPS_HD_GRAPHICS
			| ITK_DISPLAY_CAPS_IP_STREAMING
			| ITK_DISPLAY_CAPS_NLP_KEYS
			| ITK_DISPLAY_CAPS_BACKGROUND
			);
#elif defined (CONFIG_3RD_MHEG_CIPLUS_PROFILE1)
	nCaps = (ITK_DISPLAY_CAPS_PIPELINE
			| ITK_DISPLAY_CAPS_HD_VIDEO
			| ITK_DISPLAY_CAPS_HD_GRAPHICS
			| ITK_DISPLAY_CAPS_SIMUL_SUBS
			| ITK_DISPLAY_CAPS_NLP_KEYS
		);
#else
	nCaps = ITK_DISPLAY_CAPS;	// <--  check point
#endif

	if (nCaps & ITK_DISPLAY_CAPS_STILL_PLANE)
	{
		/*
			Video Plane에 I-Frame을 사용하므로 Second Still은 BG1을 사용한다.
			만약 I-Frame을 OSD plane으로 사용한다면, Second Still은 BG2를 사용해야 한다.
		*/
	}

	HxLOG_Debug("nCaps (%p)\n", nCaps);

	return (itk_display_caps_t)nCaps;
}

/**
 * Configures the co-ordinate system for the display
 *
 * The display is configured for a specified co-ordinate system. All dimensions of
 * video, still image and frame buffer co-ordinates are based on this logical size.
 *
 * Note that this need not be the same as the physical dimensions used in the decoder.
 *
 * @param[in] thiz The display to change
 * @param[in] logicalSize The pixel co-ordinate size to use for all API references to this display

 * S&T Comment : this will always be called with values 720x576
 */
static void mheg_itk_port_displaySetLogicalSize(struct itk_display_t *thiz, const itk_rect_t *logical_size)
{
	if (thiz == NULL)
	{
		HxLOG_Critical("Input parameter is invalid \n");
		return;
	}

	if (logical_size == NULL)
	{
		HxLOG_Critical("Logical_size is NULL\n");
		return;
	}

	HxLOG_Debug("Logical : (%d, %d, %d, %d)\n", logical_size->x, logical_size->y, logical_size->w, logical_size->h);
}

/**
 * Adds a callback function to the display.
 *
 * @note There is at most one callback required for a display
 *
 * @param[in] thiz The display
 * @param[in] callback A pointer to the callback function
 * @param[in] context Context data to provide to the callback when it is invoked
 *
 * @return ITKE_OK if the callback is registered
 * @return ITKE_NO_RESOURCE if there is already a callback registered
 */
static itk_errcode_t mheg_itk_port_displayAddEventCallback(struct itk_display_t *thiz, itk_display_event_callback_t callback, void *callback_context)
{
	if (thiz == NULL)
	{
		HxLOG_Critical(" input parameter is invalid NUL failed to register!!!\n");
		return ITKE_FAIL;
	}

	if (callback == NULL)
	{
		HxLOG_Critical(" calback is invalid NULL  failed to register!!!\n");
		return ITKE_FAIL;
	}

	s_stDisplayInfo.pfnDisplayCallback = callback;
	s_stDisplayInfo.pfnDisplayCallbackContext = callback_context;

	HxLOG_Debug("%s registered!!! \n", __FUNCTION__);

	return ITKE_OK;
}

/**
 * Removes a callback function from the display.
 *
 * @param[in] thiz The display
 */
static void mheg_itk_port_displayRemoveEventCallback(struct itk_display_t *thiz)
{
	if (thiz == NULL)
	{
		HxLOG_Critical(" input parameter is invalid NULL\n");
		return;
	}

	s_stDisplayInfo.pfnDisplayCallback = NULL;
	s_stDisplayInfo.pfnDisplayCallbackContext = NULL;

	HxLOG_Debug("%s unregistered!\n", __FUNCTION__);
}

/**
 *
 *
 * @author swkim (2011-08-10)
 *
 * @param thiz
 * @param callback
 * @param context
 *
 * @return itk_errcode_t
 */
static itk_errcode_t mheg_itk_port_displayAddKeyCallback(struct itk_display_t *thiz, itk_display_key_callback_t callback, void *context)
{
	PARAM_UNUSED(thiz);

	if(s_stDisplayInfo.pfnKeyCallback)
	{
		HxLOG_Error(" Already been defined keyCallback !!!! \n");
		return ITKE_NO_RESOURCE;
	}

	HxLOG_Debug(" register key callback -> context(%p) \n", context);
	s_stDisplayInfo.pfnKeyCallback = callback;
	s_stDisplayInfo.pfnKeyCallbackContext = context;

	return ITKE_OK;
}

/**
  * Removes a key event callback function from the display.
  *
  * @param[in] thiz The display
 */
static void mheg_itk_port_displayRemoveKeyCallback(struct itk_display_t *thiz)
{
	PARAM_UNUSED(thiz);

	s_stDisplayInfo.pfnKeyCallback = NULL;
	s_stDisplayInfo.pfnKeyCallbackContext = NULL;
	HxLOG_Debug(" Free registered key callback ! \n");
}

/**
 * refer to itk_display.h - void (*setKeyMask)(struct itk_display_t thiz, const itk_int32_t *keys, size_t keysCount);
 *
 * @author swkim (2011-08-09)
 *
 * @param thiz
 * @param keys
 * @param keysCount
 */
static void mheg_itk_port_displaySetKeyMask(struct itk_display_t *thiz, const itk_int32_t *keys, size_t keysCount)
{
	HINT32	dfbKey;
	MHEG_DisplayInfo_t	*pstInfo = &s_stDisplayInfo;

	ENTER_CRITICAL_KEY;

	if (keysCount == 0)
	{
		HAPI_RegisterAcceptableKey(eAWINDOW_MHEGPLAYER, -1, FALSE);
		HAPI_RegisterAcceptableKey(eAWINDOW_MHEGPLAYER, DIKS_RED, TRUE);

		HLIB_VECTOR_RemoveAll(pstInfo->pstKeyMask);
		HLIB_VECTOR_RemoveAll(pstInfo->pstKeyMaskBI);

		if (pstInfo->pnLastKeyMask)
		{
			HLIB_STD_MemFree(pstInfo->pnLastKeyMask);
			pstInfo->pnLastKeyMask = NULL;
		}

		pstInfo->nLastKeyMaskSize = 0;

		LEAVE_CRITICAL_KEY;

		return;
	}

	if ((pstInfo->nLastKeyMaskSize == keysCount) && (HxSTD_MemCmp(pstInfo->pnLastKeyMask, keys, sizeof(itk_int32_t) * keysCount) == 0))
	{
		HxLOG_Debug("Same key mask occurred...\n");
		LEAVE_CRITICAL_KEY;

		return;
	}

	HLIB_VECTOR_RemoveAll(pstInfo->pstKeyMask);
	HLIB_VECTOR_RemoveAll(pstInfo->pstKeyMaskBI);

	mheg_itk_displayConvertITK2DFB(pstInfo->pstKeyMask, pstInfo->pstKeyMaskBI, keys, keysCount);
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGPLAYER, -1, FALSE);

	if (pstInfo->pnLastKeyMask)
		HLIB_STD_MemFree(pstInfo->pnLastKeyMask);
	pstInfo->nLastKeyMaskSize = keysCount;
	pstInfo->pnLastKeyMask = (itk_int32_t*)HLIB_STD_MemDup(keys, sizeof(itk_int32_t) * keysCount);

	HxVECTOR_EACH(pstInfo->pstKeyMask, HINT32, dfbKey, {
		HAPI_RegisterAcceptableKey(eAWINDOW_MHEGPLAYER, dfbKey, TRUE);
	});

	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGPLAYER, DIKS_BACK, TRUE);

	LEAVE_CRITICAL_KEY;
}

/**
 * Checks if the specified set of key event values are supported and avialable
 * for use by the MHEG application.  If all of the key event value are available
 * then the function shll return ITK_VALID otherwise it shall return ITK_NOT_VALID.
 *
 * @param[in] thiz The display
 * @param[in] keys a list of key values to pass
 * @param[in] keysCount the number of values in keys
 */
static itk_validation_t mheg_itk_port_displayCheckKeyMask(struct itk_display_t *thiz, const itk_int32_t *keys, size_t keysCount)
{
	int i;
	itk_validation_t eValidation = ITK_VALID;
	redkey_profile_t eItkProfile = s_stDisplayInfo.itkProfile;

	HxLOG_Debug(C_CODE_F_WHITE_B_RED" keys = %p, keyCount=%d"C_CODE_RESET"\n", keys, keysCount);

	if (keysCount > 0)
	{
		for (i = 0; i < keysCount; i++)
		{
			HxLOG_Debug("[%d] keys [%d]\n", i, keys[i]);

			switch (keys[i])
			{
				case ITK_KEY_UP:
				case ITK_KEY_DOWN:
				case ITK_KEY_LEFT:
				case ITK_KEY_RIGHT:
				case ITK_KEY_0:
				case ITK_KEY_1:
				case ITK_KEY_2:
				case ITK_KEY_3:
				case ITK_KEY_4:
				case ITK_KEY_5:
				case ITK_KEY_6:
				case ITK_KEY_7:
				case ITK_KEY_8:
				case ITK_KEY_9:
				case ITK_KEY_SELECT:
				case ITK_KEY_CANCEL:
				case ITK_KEY_RED:
				case ITK_KEY_GREEN:
				case ITK_KEY_YELLOW:
				case ITK_KEY_BLUE:
				case ITK_KEY_TEXT:
				case ITK_KEY_STOP:
				case ITK_KEY_PLAY:
				case ITK_KEY_PAUSE:
				case ITK_KEY_SKIP_FORWARD:
				case ITK_KEY_SKIP_BACK:
				case ITK_KEY_FAST_FORWARD:
				case ITK_KEY_REWIND:
				case ITK_KEY_PLAY_PAUSE:
					break;


				case ITK_KEY_INFO:
					if (eItkProfile == REDKEY_PROFILE_CIPLUS)
					{
						eValidation = ITK_NOT_VALID;
					}
					break;

				default:
					HxLOG_Error(" Invalid key : [%d] keys(%ld)\n", i, keys[i]);
					eValidation = ITK_NOT_VALID;
			}

			if (eValidation == ITK_NOT_VALID)
				break;
		}
	}

	return eValidation;
}


/**
  * Enables and disables an extended keyboard
  *
  * The extended keyboard is used where it is implemented to input alpha-numeric
  * data. Keys are received through the key callback function. When the keyboard
  * is enabled the parameter keyFilter points to a set of key values that are
  * expected (which may be used to limit the keys displayed for input).
  *
  * When the keyboard is disabled the standard remote control key events are enabled.
  *
  * @param[in] thiz The display
  * @param[in] control ITK_ENABLE to enable the extended keyboard
  * @param[in] keyFilter A pointer to a set of key values that should be recognised and sent, NULL
  *            if control is ITK_DISABLE
  *
  * @return ITKE_FAIL if the display does not support an extended keyboard
  * @return ITKE_OK if the keyboard is enabled or disabled correctly
 */
itk_errcode_t mheg_itk_port_displaySetKeyboard(struct itk_display_t *thiz, itk_control_t control, const itk_uint8_t *keyFilter)
{
	PARAM_UNUSED(thiz);
	PARAM_UNUSED(control);
	PARAM_UNUSED(keyFilter);

	HxLOG_Trace();
	return ITKE_FAIL;	/* <== check point #49 */
}

/**
* Gets the (emulated) aspect ratio of the display.
*
* In most cases this will be the physical shape of the display.
* Some displays (e.g. a computer screen) may be different from the
* standard 16:9 and 4:3 shapes. In this case the shape is likely to be the closest
* available.
*
* @note The value itk_display_shape_t::ITK_SHAPE_UND is not allowed as a
* return value
*
* @param[in] thiz The display
*
* @return The display's emulated aspect ratio
*/
itk_display_shape_t mheg_itk_port_displayGetShape(struct itk_display_t *thiz)
{
	HERROR		hErr;
	DxAspectRatio_e	scrRatio = eDxASPECTRATIO_UNKNOWN;

	if (thiz == NULL)
	{
		HxLOG_Error("[%s:%d] input parameter is invalid\n", __FUNCTION__, __LINE__);

		return ITK_SHAPE_4_3;		/* 디폴트로 4:3 설정함. */
	}

	hErr = APK_OUTPUT_VIDEO_GetTvAspectRatio(&scrRatio);

	if (hErr == ERR_OK &&  scrRatio == eDxASPECTRATIO_16X9)
	{
		HxLOG_Info("Screen Ratio is 16:9\n");

		return ITK_SHAPE_16_9;
	}
	else if (hErr == ERR_OK &&  scrRatio == eDxASPECTRATIO_4X3)
	{
		HxLOG_Info("Screen Ratio is 4:3\n");
		return ITK_SHAPE_4_3;
	}

	HxLOG_Info("Screen Ratio is 4:3\n");

	return ITK_SHAPE_4_3;		/* 디폴트로 4:3 설정함. */
}

/**
 * Update video window like position
 *
 * @author swkim (2011-08-10)
 *
 * @param pstVideoWindowParam [in] : rate
 *
 * @return HERROR
 */
static HERROR mheg_itk_port_displayUpdateVideoWindow(MHEG_PIGRect_t *pstVideoWindowParam)
{
	HUINT32				ulViewId;
	HERROR				hErr = ERR_FAIL;
	DxDfcSelect_e		dfc;
	MHEG_PIGRect_t		VidWinSetting;
	DxAspectRatio_e	videoAR = eDxASPECTRATIO_UNKNOWN;
	DxAspectRatio_e	eOsdAR;

	PORT_ITK_GetMastViewId(&ulViewId);

	/*********************************************************************************************/
	/*	Update DFC	*/
	/*********************************************************************************************/
	/*
		Video plane에 I-frame재생중이면 DFC는 무조건 None이다
	*/
	if (port_itk_display_GetStillPlaneStatus(MHEG_DISPLAY_PLANE_STILL2) == TRUE)
	{
		dfc = eDxDFC_SELECT_FULLSCREEN;
	}
	else
	{
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
		hErr = APK_MEDIA_PLAY_GetStreamAspectRatio(ulViewId, &videoAR);
#endif

		HxLOG_Info(" hErr(%d) videoAR(%d)[1->4:3][2->16:9], aspectRatio(%d)[0->16:9][1->4:3], dfc4_3(%d) dfc16_9(%d)\n",
				hErr, videoAR, s_stDisplayInfo.videoContext.mhegAR ,s_stDisplayInfo.videoContext.mhegDfc_4_3, s_stDisplayInfo.videoContext.mhegDfc_16_9);

		if(hErr == ERR_OK)
		{
			if(videoAR == eDxASPECTRATIO_16X9)
			{
				dfc = s_stDisplayInfo.videoContext.mhegDfc_16_9;
			}
			else if(videoAR == eDxASPECTRATIO_4X3)
			{
				dfc = s_stDisplayInfo.videoContext.mhegDfc_4_3;
			}
			else
			{
				dfc = eDxDFC_SELECT_UNKNOWN;
			}
		}
		else
		{
			dfc = eDxDFC_SELECT_UNKNOWN;
		}
	}

	/*********************************************************************************************/
	/*		Update Rectangle for Video position	 */
	/*********************************************************************************************/
	/* Video window param이 NULL이면 기존 RECT기준으로 수행한다 */
	if (pstVideoWindowParam == NULL)
	{
		HxLOG_Debug(" same video rect!\n");

		VidWinSetting = s_stDisplayInfo.videoContext.stPigRect;
	}
	else
	{
		HxSTD_memcpy(&VidWinSetting, pstVideoWindowParam, sizeof(MHEG_PIGRect_t));
		s_stDisplayInfo.videoContext.stPigRect = VidWinSetting;

		if( CHECK_MHEG_VIDEO_SCALED(s_stDisplayInfo.videoContext.stPigRect.stOutRect.nX,
										s_stDisplayInfo.videoContext.stPigRect.stOutRect.nY,
										s_stDisplayInfo.videoContext.stPigRect.stOutRect.nW,
										s_stDisplayInfo.videoContext.stPigRect.stOutRect.nH) )
		{
			s_stDisplayInfo.videoContext.bPIGFlag = TRUE;

			if( (s_stDisplayInfo.videoContext.stPigRect.ulOutRefWidth != 0)
				&& (s_stDisplayInfo.videoContext.stPigRect.ulOutRefHeight != 0)
				&& (s_stDisplayInfo.videoContext.stPigRect.stOutRect.nW == s_stDisplayInfo.videoContext.stPigRect.ulOutRefWidth >> 1)
				&& (s_stDisplayInfo.videoContext.stPigRect.stOutRect.nH == s_stDisplayInfo.videoContext.stPigRect.ulOutRefHeight >> 1))
			{
				s_stDisplayInfo.videoContext.bQuarterScreen = TRUE;
			}
			else
			{
				s_stDisplayInfo.videoContext.bQuarterScreen = FALSE;
			}
		}
		else
		{
			s_stDisplayInfo.videoContext.bPIGFlag = FALSE;
		}
	}

	HxLOG_Info(" [#1] SET In[%d, %d, %d, %d] Out[%d, %d, %d, %d]\n",
						VidWinSetting.stInRect.nX, VidWinSetting.stInRect.nY, VidWinSetting.stInRect.nW, VidWinSetting.stInRect.nH,
						VidWinSetting.stOutRect.nX, VidWinSetting.stOutRect.nY, VidWinSetting.stOutRect.nW, VidWinSetting.stOutRect.nH);

	/*********************************************************************************************/
	/*	Update Aspect Ratio	*/
	/*********************************************************************************************/
	if(s_stDisplayInfo.videoContext.mhegAR == ITK_SHAPE_16_9)
	{
		eOsdAR = eDxASPECTRATIO_16X9;
	}
	else if(s_stDisplayInfo.videoContext.mhegAR == ITK_SHAPE_4_3)
	{
		eOsdAR = eDxASPECTRATIO_4X3;
	}
	else
	{
		eOsdAR = eDxASPECTRATIO_UNKNOWN;
	}

	HxLOG_Info(" [#2] aspectRatio(%d) dfc(%d) bQuater(%d)\n",
						s_stDisplayInfo.videoContext.mhegAR,
						dfc,
						s_stDisplayInfo.videoContext.bQuarterScreen);
	HxLOG_Info(" Reserved In[%d, %d, %d, %d]/[%d, %d] Out[%d, %d, %d, %d]/[%d, %d]\n",
								s_stDisplayInfo.videoContext.stPigRect.stInRect.nX,
								s_stDisplayInfo.videoContext.stPigRect.stInRect.nY,
								s_stDisplayInfo.videoContext.stPigRect.stInRect.nW,
								s_stDisplayInfo.videoContext.stPigRect.stInRect.nH,
								s_stDisplayInfo.videoContext.stPigRect.ulInRefWidth,
								s_stDisplayInfo.videoContext.stPigRect.ulInRefHeight,
								s_stDisplayInfo.videoContext.stPigRect.stOutRect.nX,
								s_stDisplayInfo.videoContext.stPigRect.stOutRect.nY,
								s_stDisplayInfo.videoContext.stPigRect.stOutRect.nW,
								s_stDisplayInfo.videoContext.stPigRect.stOutRect.nH,
								s_stDisplayInfo.videoContext.stPigRect.ulOutRefWidth,
								s_stDisplayInfo.videoContext.stPigRect.ulOutRefHeight);

	if( (eOsdAR != eDxASPECTRATIO_UNKNOWN) && (dfc == eDxDFC_SELECT_UNKNOWN) )
	{ /* Presantation shape을 걸었는데, DFC가 Unknown인 경우는 강제로 full screen으로 한다.
		ref. ITK user guide Appendix D */
		HxLOG_Info(" Change DFC from [Unknown] to [Full] while presentation shape exists.\n");
		dfc = eDxDFC_SELECT_FULLSCREEN;
	}

	hErr = APK_MEDIA_PLAY_SetPigRect(ulViewId, 3/*eOsdZOrder_MHEG*/,
									 VidWinSetting.stInRect.nX, VidWinSetting.stInRect.nY, VidWinSetting.stInRect.nW, VidWinSetting.stInRect.nH,
									 VidWinSetting.ulInRefWidth, VidWinSetting.ulInRefHeight,
									 VidWinSetting.stOutRect.nX, VidWinSetting.stOutRect.nY, VidWinSetting.stOutRect.nW, VidWinSetting.stOutRect.nH,
									 VidWinSetting.ulOutRefWidth, VidWinSetting.ulOutRefHeight,
									 eOsdAR, dfc, eDxASPECTRATIO_UNKNOWN);

	// 3.12.7.2 Decode Offset Performance 위한 WA 코드임.
	// Video -> I-Frame 전환 늦는 문제
	VK_TASK_Sleep(40);

	return hErr;
}

/**
* Sets the output aspect ratio to be used by the display.
*
* Most of the time the value of aspect ratio is itk_display_shape_t::ITK_SHAPE_UND,
* or undefined. In this case the output aspect ratio signalling is
* controlled by the platform's rules for processing and presenting
* the input video stream. However, in certain cases the client may have
* to force a particular aspect ratio by calling this method. In this
* case the output is forced to 16:9 or 4:3, with any induced distortions
* this creates. Note that following the standard decoder model as
* described in @ref decodermodel, the ultimate output is controlled
* by the Display Format Conversion - this aspect ratio is only an
* input to that conversion.
*
* The default value for output aspect ratio is "undefined".
*
* @param[in] thiz The display
* @param[in] mode The widescreen mode to use
*/
void mheg_itk_port_displaySetPresentationShape(struct itk_display_t *thiz, itk_display_shape_t shape)
{
	HERROR				hErr = ERR_OK;

	HxLOG_Info(C_CODE_F_WHITE_B_RED" shape : %d [0->16:9][1->4:3][2->UND][3->UNSET]"C_CODE_RESET"\n", shape);

	if (thiz == NULL)
	{
		HxLOG_Error("[%s:%d] input parameter is invalid NULL\n", __FUNCTION__, __LINE__);
		return;
	}

	if (PORT_ITK_GetMhegRunningStatus() == FALSE)
	{
		HxLOG_Error("[%s:%d] ERROR : Redkey is not running! \n", __FUNCTION__, __LINE__);
		return;
	}

	s_stDisplayInfo.videoContext.mhegAR = shape;

	hErr = mheg_itk_port_displayUpdateVideoWindow(NULL);
#ifdef DEBUG
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("ERROR : while calling [%s]\n", __FUNCTION__);
	}
#else
	UNUSED(hErr);
#endif
}

/**
 * Sets the display format conversion mode to be used for video
 * presentation.
 *
 * @see itk_display_dfc_t
 *
 * @note The DFC mode is not used for presentation of images on any plane.
 *
 * @param[in] thiz The media
 * @param[in] dfcFor4_3 The DFC mode to use for presenting 4:3 video source
 * @param[in] dfcFor16_9 The DFC mode to use for presenting 16:9 video source
 */
void mheg_itk_port_displaySetDFBMode(struct itk_display_t *thiz, itk_display_dfc_t dfc4_3, itk_display_dfc_t dfc16_9)
{
	HERROR				hErr = ERR_OK;

	HxLOG_Info(C_CODE_F_WHITE_B_RED" dfc4_3 : %d, dfc16_9 : %d"C_CODE_RESET"\n",dfc4_3, dfc16_9);

	if (thiz == NULL)
	{
		HxLOG_Error("ERROR : input parameter is invalid NULL\n");
		return;
	}

	if (PORT_ITK_GetMhegRunningStatus() == FALSE)
	{
		HxLOG_Error("ERROR : Redkey is not running! \n");
		return;
	}

	HxLOG_Info("quarter (%d)\n", s_stDisplayInfo.videoContext.bQuarterScreen);

	switch (dfc4_3)
	{
		case ITK_DFC_UND:			/**< Follow the platform's rules */
			s_stDisplayInfo.videoContext.mhegDfc_4_3 = eDxDFC_SELECT_UNKNOWN;
			HxLOG_Info(" [4:3] ITK_DFC_UND\n");
			break;

		case ITK_DFC_CCO:			/* Centre Cut Out 16:9 on 4:3 */
			s_stDisplayInfo.videoContext.mhegDfc_4_3 = eDxDFC_SELECT_SIDECUT;
			HxLOG_Info(" [4:3] ITK_DFC_CCO\n");
			break;

		case ITK_DFC_LETTERBOX:		/* Letterbox 16:9 on 4:3 */
			s_stDisplayInfo.videoContext.mhegDfc_4_3 = eDxDFC_SELECT_LETTERBOX;
			HxLOG_Info(" [4:3] ITK_DFC_LETTERBOX\n");
			break;

		case ITK_DFC_PILLARBOX:		/* Pillarbox 4:3 on 16:9 (where this is not supported emulate ITK_DFC_NONE), Note that the Pillarbox DFC only applies if the video is in quarter-screen format.*/
			if ((s_stDisplayInfo.videoContext.bQuarterScreen == TRUE) && CHECK_MHEG_AR_CONTROL(s_stDisplayInfo.videoContext.mhegAR))
			{
				s_stDisplayInfo.videoContext.mhegDfc_4_3 = eDxDFC_SELECT_PILLARBOX;
			}
			else
			{
				s_stDisplayInfo.videoContext.mhegDfc_4_3 = eDxDFC_SELECT_FULLSCREEN;
			}
			HxLOG_Info(" [4:3] ITK_DFC_PILLARBOX - (%s)\n", (s_stDisplayInfo.videoContext.mhegDfc_4_3 == eDxDFC_SELECT_PILLARBOX) ? "Pillar" : "Full");
			break;

		case ITK_DFC_UNSET:
			HxLOG_Info(" [4:3] ITK_DFC_UNSET\n");
			s_stDisplayInfo.videoContext.mhegDfc_4_3 = eDxDFC_SELECT_UNKNOWN;
			return;

		case ITK_DFC_NONE:			/**< No conversion (may cause aspect distortion) */
			s_stDisplayInfo.videoContext.mhegDfc_4_3 = eDxDFC_SELECT_FULLSCREEN;
			HxLOG_Info(" [4:3] ITK_DFC_NONE\n");
			break;

		default:
			HxLOG_Assert(0);
			return;
	}

	switch (dfc16_9)
	{
		case ITK_DFC_UND:			/**< Follow the platform's rules */
			s_stDisplayInfo.videoContext.mhegDfc_16_9 = eDxDFC_SELECT_UNKNOWN;
			HxLOG_Info(" [16:9] ITK_DFC_UND\n");
			break;

		case ITK_DFC_CCO:			/* Centre Cut Out 16:9 on 4:3 */
			s_stDisplayInfo.videoContext.mhegDfc_16_9 = eDxDFC_SELECT_SIDECUT;
			HxLOG_Info(" [16:9] ITK_DFC_CCO\n");
			break;

		case ITK_DFC_LETTERBOX:		/* Letterbox 16:9 on 4:3 */
			s_stDisplayInfo.videoContext.mhegDfc_16_9 = eDxDFC_SELECT_LETTERBOX;
			HxLOG_Info(" [16:9] ITK_DFC_LETTERBOX\n");
			break;

		case ITK_DFC_PILLARBOX:		/* Pillarbox 4:3 on 16:9 (where this is not supported emulate ITK_DFC_NONE), Note that the Pillarbox DFC only applies if the video is in quarter-screen format.*/
			if ((s_stDisplayInfo.videoContext.bQuarterScreen == TRUE) && CHECK_MHEG_AR_CONTROL(s_stDisplayInfo.videoContext.mhegAR))
			{
				s_stDisplayInfo.videoContext.mhegDfc_16_9 = eDxDFC_SELECT_PILLARBOX;
			}
			else
			{
				s_stDisplayInfo.videoContext.mhegDfc_16_9 = eDxDFC_SELECT_FULLSCREEN;
			}
			HxLOG_Info(" [16:9] ITK_DFC_PILLARBOX - (%s)\n", (s_stDisplayInfo.videoContext.mhegDfc_16_9 == eDxDFC_SELECT_PILLARBOX) ? "Pillar" : "Full");
			break;

		case ITK_DFC_UNSET:
			HxLOG_Info(" [16:9] ITK_DFC_UNSET\n");
			s_stDisplayInfo.videoContext.mhegDfc_16_9 = eDxDFC_SELECT_UNKNOWN;
			return;

		case ITK_DFC_NONE:			/**< No conversion (may cause aspect distortion) */
			s_stDisplayInfo.videoContext.mhegDfc_16_9 = eDxDFC_SELECT_FULLSCREEN;
			HxLOG_Info(" [16:9] ITK_DFC_NONE\n");
			break;

		default:
			HxLOG_Assert(0);
			return;
	}

	hErr = mheg_itk_port_displayUpdateVideoWindow(NULL);
#ifdef DEBUG
	if (hErr != ERR_OK)
	{
		HxLOG_Error("ERROR : while calling local_itk_Display_SetDfcWindow \n");
	}
#else
	UNUSED(hErr);
#endif

}

/**
 * Sets the display's background colour. The transparency of the colour
 * is ignored. The background colour is presented wherever it is not masked
 * out by the planes above it.
 *
 * @param[in] thiz The display
 * @param[in] colour The colour
 */
void mheg_itk_port_displaySetBackgroundColor(struct itk_display_t *thiz, itk_colour_t const *colour)
{
	// check point.
	if (thiz == NULL)
	{
		HxLOG_Error(" input parameter is invalid NULL\n");
		return;
	}

	if (colour == NULL)
	{
		HxLOG_Error(" colour is invalid NULL\n");
		return;
	}

	HxLOG_Info(" R,G,B,A(%d, %d, %d, %d) \n", colour->red, colour->green, colour->blue, colour->trans);

	if (APK_OUTPUT_VIDEO_SetBackGroundColor(colour->red, colour->green, colour->blue) != ERR_OK)
	{
		HxLOG_Error("Error : can't set backgroup color\n");
		return;
	}
	else
	{
		s_stDisplayInfo.stBgColor.red	= colour->red;
		s_stDisplayInfo.stBgColor.green	= colour->green;
		s_stDisplayInfo.stBgColor.blue	= colour->blue;
		s_stDisplayInfo.stBgColor.trans	= colour->trans;
	}
}

/**
* Enables or disables subtitle decoding for a video stream
*
* Note that setting this control to ITK_ENABLE only causes subtitles to be
* presented if:
* - the viewer has enabled presentation
* - the service has subtitles
* - the host allows simultaneous MHEG and subtitles
*
* @see subtitle
*
* @param[in] thiz The media
* @param[in] control The subtitle control to operate
*/
void mheg_itk_port_displaySetSubtitle(struct itk_display_t *thiz, itk_control_t control)
{
	HUINT32	ulViewId;

	if (thiz == NULL)
	{
		HxLOG_Error(" input parameter is invalid NULL\n");
		return;
	}

	PORT_ITK_GetMastViewId(&ulViewId);
	PORT_ITK_PostMsg(eMHEG_DISPLAY_SUBTITLE_CONTROL, ulViewId, control, 0, 0, 0);

	HxLOG_Debug(" subtitle control : %s\n", (control == ITK_ENABLE) ? "ENABLE" : "DISABLE");
}

/**
 *
 *
 * @author swkim (2011-08-10)
 *
 * @param plane
 * @param decode_origin_x
 * @param decode_origin_y
 * @param decode_origin_width
 * @param decode_origin_height
 * @param view_origin_x
 * @param view_origin_y
 * @param view_origin_width
 * @param view_origin_height
 */
static void	mheg_itk_port_displaySetVideoScalingInfo(HUINT8 plane,
												int decode_origin_x,
												int decode_origin_y,
												int decode_origin_width,
												int decode_origin_height,
												int view_origin_x,
												int view_origin_y,
												int view_origin_width,
												int view_origin_height)
{
	if (plane >= MHEG_DISPLAY_PLANE_VIDEO_ALL)
	{
		return;
	}

	s_stDisplayInfo.scaling[plane].decode_x = decode_origin_x;
	s_stDisplayInfo.scaling[plane].decode_y = decode_origin_y;
	s_stDisplayInfo.scaling[plane].decode_w = decode_origin_width;
	s_stDisplayInfo.scaling[plane].decode_h = decode_origin_height;
	s_stDisplayInfo.scaling[plane].view_x = view_origin_x;
	s_stDisplayInfo.scaling[plane].view_y = view_origin_y;
	s_stDisplayInfo.scaling[plane].view_w = view_origin_width;
	s_stDisplayInfo.scaling[plane].view_h = view_origin_height;
}


static HERROR mheg_itk_port_displaySetVideoRectOnImgStillPlane(DxRect_t *videoRect, DxRect_t *sourceRect, int offset_x, int offset_y)
{
	DxRect_t				realRect = {0, 0, 0, 0};

	IDirectFBSurface*		pImgStillSurface;
	IDirectFBSurface*		pImgStillBackupSurface;
	DFBRectangle			offsetRect;

	ENTER_CRITICAL_IS;

	pImgStillSurface = s_stDisplayInfo.dfbInfo.dfbStillSurface;
	pImgStillBackupSurface = s_stDisplayInfo.dfbInfo.dfbStillBackupSurface;

	if(pImgStillSurface && pImgStillBackupSurface)
	{
		DFBCHECK(pImgStillSurface->SetColor(pImgStillSurface, s_stDisplayInfo.stBgColor.red, s_stDisplayInfo.stBgColor.green, s_stDisplayInfo.stBgColor.blue, 0xFF));
		DFBCHECK(pImgStillSurface->SetDrawingFlags(pImgStillSurface, DSDRAW_NOFX));
		DFBCHECK(pImgStillSurface->SetPorterDuff(pImgStillSurface, DSPD_SRC));
		DFBCHECK(pImgStillSurface->FillRectangle(pImgStillSurface, 0, 0, ITK_DISPLAY_OSD_HD_W, ITK_DISPLAY_OSD_HD_H));

		// Image Still Plane의 Crop / Positioning
		if(sourceRect)
		{
			offsetRect.x = sourceRect->nX * ITK_DISPLAY_OSD_HD_W / ITK_DISPLAY_WINDOW_W;
			offsetRect.y = sourceRect->nY * ITK_DISPLAY_OSD_HD_H / ITK_DISPLAY_WINDOW_H;
			offsetRect.w = sourceRect->nW * ITK_DISPLAY_OSD_HD_W / ITK_DISPLAY_WINDOW_W;
			offsetRect.h = sourceRect->nH * ITK_DISPLAY_OSD_HD_H / ITK_DISPLAY_WINDOW_H;
		}
		else
		{
			offsetRect.x = 0;
			offsetRect.y = 0;
			offsetRect.w = ITK_DISPLAY_OSD_HD_W;
			offsetRect.h = ITK_DISPLAY_OSD_HD_H;
		}

		DFBCHECK(pImgStillSurface->Blit(pImgStillSurface, pImgStillBackupSurface, &offsetRect,
										offset_x * ITK_DISPLAY_OSD_HD_W / ITK_DISPLAY_WINDOW_W,
										offset_y * ITK_DISPLAY_OSD_HD_H / ITK_DISPLAY_WINDOW_H));

		HxLOG_Debug("mheg_itk_port_displaySetVideoRectOnImgStillPlane(), realRect (%d, %d, %d, %d)\n", realRect.nX, realRect.nY, realRect.nW, realRect.nH);

		// Image Still Plane에 그려질 Video area rect 설정
		realRect.nX = videoRect->nX * ITK_DISPLAY_OSD_HD_W / ITK_DISPLAY_WINDOW_W;
		realRect.nY = videoRect->nY * ITK_DISPLAY_OSD_HD_H / ITK_DISPLAY_WINDOW_H;
		realRect.nW = videoRect->nW * ITK_DISPLAY_OSD_HD_W / ITK_DISPLAY_WINDOW_W;
		realRect.nH = videoRect->nH * ITK_DISPLAY_OSD_HD_H / ITK_DISPLAY_WINDOW_H;

		if(realRect.nW <= 0 || realRect.nH <= 0)
		{
			HxLOG_Debug("mheg_itk_port_displaySetVideoRectOnImgStillPlane(), invalid rect -> do not draw the video rect!!\n");
		}
		else
		{
			// Image Still Plane에 video 영역을 위한 black transparent 구멍을 냄
			HxLOG_Debug("mheg_itk_port_displaySetVideoRectOnImgStillPlane(), scaled --> draw the video rect!!\n");
			DFBCHECK(pImgStillSurface->SetColor(pImgStillSurface, 0, 0, 0, 0));	// transparent
			DFBCHECK(pImgStillSurface->SetDrawingFlags(pImgStillSurface, DSDRAW_NOFX));
			DFBCHECK(pImgStillSurface->SetPorterDuff(pImgStillSurface, DSPD_SRC));
			DFBCHECK(pImgStillSurface->FillRectangle(pImgStillSurface, realRect.nX, realRect.nY, realRect.nW, realRect.nH));
		}

		// Image Still Plane Flip
		/* mheg player에서 Flip 하도록 함 - DFB Slave로 동작하게 하기 위함 */
		mheg_itk_displayFlipWindow(pImgStillSurface, NULL, DSFLIP_BLIT);
	}
	else
	{
		LEAVE_CRITICAL_IS;

		return ERR_FAIL;
	}

	LEAVE_CRITICAL_IS;

	return ERR_OK;
}

/**
 * Hide video rect on background plane
 *
 * @author yjlim (2012-08-25)
 */
static void mheg_itk_port_displayHideFullVideoRectOnImgStillPlane(void)
{
	DxRect_t	videoRect = {0, 0, 0, 0};

	mheg_itk_port_displaySetVideoRectOnImgStillPlane(&videoRect, NULL, 0, 0);
}

/**
 *
 *
 * @author swkim (2011-08-10)
 *
 * @param plane
 * @param decode_origin_x
 * @param decode_origin_y
 * @param decode_origin_width
 * @param decode_origin_height
 * @param view_origin_x
 * @param view_origin_y
 * @param view_origin_width
 * @param view_origin_height
 */
static void	mheg_itk_port_displayGetScalingInfo(HUINT8 plane,
												int *decode_origin_x,
												int *decode_origin_y,
												int *decode_origin_width,
												int *decode_origin_height,
												int *view_origin_x,
												int *view_origin_y,
												int *view_origin_width,
												int *view_origin_height)
{
	if (plane >= MHEG_DISPLAY_PLANE_VIDEO_ALL)
	{
		return;
	}

	if (decode_origin_x)		*decode_origin_x = s_stDisplayInfo.scaling[plane].decode_x;
	if (decode_origin_y) 		*decode_origin_y = s_stDisplayInfo.scaling[plane].decode_y;
	if (decode_origin_width)	*decode_origin_width = s_stDisplayInfo.scaling[plane].decode_w;
	if (decode_origin_height)	*decode_origin_height = s_stDisplayInfo.scaling[plane].decode_h;
	if (view_origin_x)			*view_origin_x = s_stDisplayInfo.scaling[plane].view_x;
	if (view_origin_y)			*view_origin_y = s_stDisplayInfo.scaling[plane].view_y;
	if (view_origin_width)		*view_origin_width = s_stDisplayInfo.scaling[plane].view_w;
	if (view_origin_height)		*view_origin_height = s_stDisplayInfo.scaling[plane].view_h;
}



/**
 *
 *
 * @author swkim (2011-08-10)
 *
 * @param plane
 * @param decode_origin_x
 * @param decode_origin_y
 * @param decode_origin_width
 * @param decode_origin_height
 * @param view_origin_x
 * @param view_origin_y
 * @param view_origin_width
 * @param view_origin_height
 */
static void	mheg_itk_port_displaySetBasicVideoPosition(HUINT8 plane,
											int decode_origin_x,
											int decode_origin_y,
											int decode_origin_width,
											int decode_origin_height,
											int view_origin_x,
											int view_origin_y,
											int view_origin_width,
											int view_origin_height)
{
	int view_x, view_y, view_width, view_height;
	int source_x, source_y, source_width, source_height;
	int window_x, window_y, window_width, window_height;
	int ref_video_width=0;
	int ref_video_height=0;
	HERROR hError = ERR_FAIL;
	MHEG_PIGRect_t	stVidWinSetting;

	HxLOG_Info(" plane(%s) decode(%d, %d, %d, %d) view(%d, %d, %d, %d) \n",
			(plane == MHEG_DISPLAY_PLANE_STILL1) ? "Image" : (plane == MHEG_DISPLAY_PLANE_STILL2) ? "IFrame" : "VIDEO",
			decode_origin_x, decode_origin_y, decode_origin_width, decode_origin_height,
			view_origin_x, view_origin_y, view_origin_width, view_origin_height);

	if ( decode_origin_x < MHEG_DISPLAY_OFFSET_X || view_origin_x < MHEG_DISPLAY_OFFSET_X
		|| decode_origin_y < MHEG_DISPLAY_OFFSET_Y || view_origin_y < MHEG_DISPLAY_OFFSET_Y
		|| decode_origin_width > MHEG_DISPLAY_SCALE_W || view_origin_width > MHEG_DISPLAY_SCALE_W
		|| decode_origin_height > MHEG_DISPLAY_SCALE_H || view_origin_height > MHEG_DISPLAY_SCALE_H)
	{
		HxLOG_Error("ERROR : Display Offset \n");
		return;
	}

	ref_video_width = ITK_DISPLAY_WINDOW_W;
	ref_video_height = ITK_DISPLAY_WINDOW_H;

	HxLOG_Info("\t ref_video_width(%d) height(%d)\n", ref_video_width, ref_video_height);

//////////////////////////////////////////////////////////////////////
// active 영역 내로 cliiping 처리.
//////////////////////////////////////////////////////////////////////
// View Window의 Logical Size는 720x576으로 고정.
	if (view_origin_x < 0)
	{
		view_x = 0;
		view_width = view_origin_width + view_origin_x;
		if (view_width > ITK_DISPLAY_WINDOW_W)
			view_width = ITK_DISPLAY_WINDOW_W;
	}
	else
	{
		view_x = view_origin_x;
		if ((view_origin_x + view_origin_width) > ITK_DISPLAY_WINDOW_W)
			view_width = ITK_DISPLAY_WINDOW_W - view_origin_x;
		else
			view_width = view_origin_width;
	}

	if (view_origin_y < 0)
	{
		view_y = 0;
		view_height = view_origin_height + view_origin_y;
		if (view_height > ITK_DISPLAY_WINDOW_H)
			view_height = ITK_DISPLAY_WINDOW_H;
	}
	else
	{
		view_y = view_origin_y;
		if ((view_origin_y + view_origin_height) > ITK_DISPLAY_WINDOW_H)
			view_height = ITK_DISPLAY_WINDOW_H - view_origin_y;
		else
			view_height = view_origin_height;
	}

	HxLOG_Info("\t view rect (%d %d %d %d)\n", view_x, view_y, view_width, view_height);

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// decode rect 는 화면상의 out rect 이고 view rect는 out rect 중의 show 영역을 나타내는 rect 임.
// 그러므로 scaling은 decode rect 를 기준으로만 일어나야 한다.
// view 처리를 위해서는 in rect 처리가 필요한데, in rect 계산은 decode rect를 ref w/h 로 해야한다.
// 다만, in rect 를 설정하면 decode rect에 full 로 차도록 scaling 이 되므로 out rect도 in rect에 맞추어야 한다.
//////////////////////////////////////////////////////////////////////
// Original Video의 Cutting Part  & View Window의 Size 조정
	if (decode_origin_x <= view_x)
	{ /* decode_x 보다 view_x 가 안쪽이면 */
		source_x = view_x - decode_origin_x; /*decode_x 와 view_x 차이 만큼 in rect x 시키고... */
		window_x = view_x; /* out rect x는 scaling을 막기 위해 view_x 로 치환 */

		if(decode_origin_x + decode_origin_width > view_origin_x + view_origin_width)
		{ /* decode end가 view end 바깥이면 view end 까지만 right in rect 시킴. */
			source_width = view_width;
		}
		else
		{ /* decode end가 view end 안쪽이면 decode end 까지 사용. 즉, right in rect 안함. */
			source_width = decode_origin_width - source_x;
		}
	}
	else
	{ /* decode x 보다 view x 가 바깥이면 */
		source_x = 0; /* inrect x clipping 할 필요 없다. */
		window_x = decode_origin_x; /* out rect x 는 그대로 사용 */

		if(decode_origin_x + decode_origin_width > view_origin_x + view_origin_width)
		{ /* decode end가 view end 바깥이면 view end 까지만 right in rect 시킴. */
			source_width = view_x + view_width - decode_origin_x;
		}
		else
		{ /* decode end가 view end 안쪽이면 decode end 까지 사용. 즉, right in rect 안함. */
			source_width = decode_origin_width;
		}
	}

	if (decode_origin_y <= view_y)
	{ /* decode_y 보다 view_y 가 안쪽이면 */
		source_y = view_y - decode_origin_y; /*decode_y 와 view_y 차이 만큼 in rect y 시키고... */
		window_y = view_y; /* out rect y는 scaling을 막기 위해 view_y 로 치환 */

		if(decode_origin_y + decode_origin_height > view_origin_y + view_origin_height)
		{ /* decode end가 view end 바깥이면 view end 까지만 bottom in rect 시킴. */
			source_height = view_height;
		}
		else
		{ /* decode end가 view end 안쪽이면 decode end 까지 사용. 즉, bottom in rect 안함. */
			source_height = decode_origin_height - source_y;
		}
	}
	else
	{ /* decode x 보다 view x 가 바깥이면 */
		source_y = 0; /* inrect x clipping 할 필요 없다. */
		window_y = decode_origin_y; /* out rect y 는 그대로 사용 */

		if(decode_origin_y + decode_origin_height > view_origin_y + view_origin_height)
		{ /* decode end가 view end 바깥이면 view end 까지만 bottom in rect 시킴. */
			source_height = view_y + view_height - decode_origin_y;
		}
		else
		{ /* decode end가 view end 안쪽이면 decode end 까지 사용. 즉, bottom in rect 안함. */
			source_height = decode_origin_height;
		}
	}

	if (source_width <= 0 || source_height <= 0)
		return;

	/* in rect scaling을 막기 위해 out rect w, h는 in rect에 일치 시킨다. */
	window_width = source_width;
	window_height = source_height;

	HxLOG_Info("\t source(%d %d %d %d) window(%d %d %d %d)\n",
			source_x, source_y, source_width, source_height,
			window_x, window_y, window_width, window_height);
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
	HxLOG_Info("\t >----------------------------------\n");
	HxLOG_Info("\t source(%d %d %d %d) window(%d %d %d %d)\n",
			source_x, source_y, source_width, source_height,
			window_x, window_y, window_width, window_height);
	HxLOG_Info("\t <----------------------------------\n");

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

	if (plane == MHEG_DISPLAY_PLANE_VIDEO || plane == MHEG_DISPLAY_PLANE_STILL2)
	{
		HBOOL bStill1 = port_itk_display_GetStillPlaneStatus(MHEG_DISPLAY_PLANE_STILL1);
		HBOOL bStill2 = port_itk_display_GetStillPlaneStatus(MHEG_DISPLAY_PLANE_STILL2);

		HxLOG_Info("\t\t IMAGE STILL(%d) VIDEO STILL(%d)\n", bStill1, bStill2);

		/* Video Plane에 Still IFrame이 display중일때 아래 video rect는 무시 */
		if ((bStill1 == TRUE) && (bStill2 == FALSE))
		{
			DxRect_t videoRect = {window_x, window_y, window_width, window_height};
			mheg_itk_port_displaySetVideoRectOnImgStillPlane(&videoRect, NULL, 0, 0);
		}

		/* In rect는 decode window 기준 */
		stVidWinSetting.stInRect.nX = source_x;
		stVidWinSetting.stInRect.nY = source_y;
		stVidWinSetting.stInRect.nW = source_width;
		stVidWinSetting.stInRect.nH = source_height;
		stVidWinSetting.ulInRefWidth = decode_origin_width;
		stVidWinSetting.ulInRefHeight = decode_origin_height;

		/* Out rect는 전체 영역 기준 */
		stVidWinSetting.stOutRect.nX = window_x;
		stVidWinSetting.stOutRect.nY = window_y;
		stVidWinSetting.stOutRect.nW = window_width;
		stVidWinSetting.stOutRect.nH = window_height;
		stVidWinSetting.ulOutRefWidth = ref_video_width;
		stVidWinSetting.ulOutRefHeight = ref_video_height;

		hError = mheg_itk_port_displayUpdateVideoWindow(&stVidWinSetting);
	}
	else // MHEG_DISPLAY_PLANE_STILL1
	{
		// TODO: 추가 still 이 필요시 사용
		DxRect_t sourceRect = {source_x, source_y, source_width, source_height};
		DxRect_t videoRect = {0, 0, 0, 0};
		DxRect_t videoSourceRect = {0, 0, 0, 0};
		DxRect_t videoWindowRect = {0, 0, 0, 0};

		mheg_itk_port_displayGetScalingInfo(MHEG_DISPLAY_PLANE_VIDEO,
										&videoSourceRect.nX, &videoSourceRect.nY, &videoSourceRect.nW, &videoSourceRect.nH,
										&videoWindowRect.nX, &videoWindowRect.nY, &videoWindowRect.nW, &videoWindowRect.nH);

		HxSTD_MemCopy(&videoRect, &videoWindowRect, sizeof(DxRect_t));

		mheg_itk_port_displaySetVideoRectOnImgStillPlane(&videoRect , &sourceRect, window_x, window_y);
	}

#ifdef DEBUG
	if (hError != ERR_OK)
	{
		if ( plane == MHEG_DISPLAY_PLANE_STILL1 )
			HxLOG_Error("ERROR : while calling MHEG_DISPLAY_PLANE_STILL1 \n");
		else if ( plane == MHEG_DISPLAY_PLANE_STILL2 )
			HxLOG_Error("ERROR : while calling MHEG_DISPLAY_PLANE_STILL2 \n");
		else
			HxLOG_Error("ERROR : while calling local_itk_Display_UpdateVideoWindow \n");
	}
#else
	UNUSED(hError);
#endif
}
/**
 *
 *
 * @author swkim (2011-08-10)
 *
 * @param plane
 * @param decode_origin_x
 * @param decode_origin_y
 * @param decode_origin_width
 * @param decode_origin_height
 * @param view_origin_x
 * @param view_origin_y
 * @param view_origin_width
 * @param view_origin_height
 */
static void	mheg_itk_port_displayChangeVideoPosition(HUINT8 plane,
											int decode_origin_x,
											int decode_origin_y,
											int decode_origin_width,
											int decode_origin_height,
											int view_origin_x,
											int view_origin_y,
											int view_origin_width,
											int view_origin_height)

{
	int decode_x, decode_y, decode_width, decode_height;
	int view_x, view_y, view_width, view_height;

	mheg_itk_port_displayGetScalingInfo(plane,
		&decode_x, &decode_y, &decode_width, &decode_height,
		&view_x, &view_y, &view_width, &view_height);

#if 0	/* 같은 값이라도 일단 Video SET 진행. (LOCK ON/OFF를 위해) */
	if (decode_x != decode_origin_x
		|| decode_y != decode_origin_y
		|| decode_width != decode_origin_width
		|| decode_height != decode_origin_height
		|| view_x != view_origin_x
		|| view_y != view_origin_y
		|| view_width != view_origin_width
		|| view_height != view_origin_height)
#endif
	{
		mheg_itk_port_displaySetBasicVideoPosition(plane,
			decode_origin_x, decode_origin_y, decode_origin_width, decode_origin_height,
			view_origin_x, view_origin_y, view_origin_width, view_origin_height);
	}
}

/**
* Sets a planes's output rectangles
*
* Two rectangles are specified in the call. Decode is the rectangle where the video
* or image being presented should be scaled and positioned. View is the area of
* the display that should be masked for presentation. Anywhere outside the view rectangle
* must be transparent so that the still and background planes behind the video plane show
* through. The view rectangle is filled with black (r = g = b = 0) anywhere that the
* decode rectangle does not show.
*
* @param[in] thiz   The media
* @param[in] plane  Which plane to change the display window of.  Can be set to either the
*                   still picture or video plane.
* @param[in] decode A pointer to the decode area rectangle
* @param[in] view   A pointer to the viewable area rectangle
*/
void mheg_itk_port_displaySetDisplayWindow(struct itk_display_t *thiz, itk_display_plane_t plane, const itk_rect_t *decode, const itk_rect_t *view)
{
	HxLOG_Info("plane (%s), decode [%ld, %ld, %ld, %ld], view [%ld, %ld, %ld, %ld] \n",
						((plane==ITK_DISPLAY_PLANE_VIDEO)?"VIDEO":"Image"),
						decode->x, decode->y, decode->w, decode->h, view->x, view->y, view->w, view->h);

	if (thiz == NULL)
	{
		HxLOG_Error("ERROR : Input parameter is invalid NULL \n");
		return;
	}

	if (PORT_ITK_GetMhegRunningStatus() == FALSE)
	{
		HxLOG_Error("ERROR : Redkey is not running! \n");
		return;
	}

	if (decode->w <= 0 || decode->h <= 0 || view->w <= 0 || view->h <= 0)
	{
		if (plane == ITK_DISPLAY_PLANE_VIDEO)
		{
			if (port_itk_display_GetStillPlaneStatus(MHEG_DISPLAY_PLANE_STILL2) == TRUE)
			{
				HxLOG_Info("mheg_itk_port_displaySetVideoScalingInfo : Plane(MHEG_DISPLAY_PLANE_STILL2) (L:%d)\n", __LINE__);
				mheg_itk_port_displaySetVideoScalingInfo(MHEG_DISPLAY_PLANE_STILL2,
							decode->x, decode->y, decode->w, decode->h, view->x, view->y, view->w, view->h);
			}
			else
			{
				HxLOG_Info("mheg_itk_port_displaySetVideoScalingInfo : Plane(MHEG_DISPLAY_PLANE_VIDEO) (L:%d)\n", __LINE__);
				mheg_itk_port_displaySetVideoScalingInfo(MHEG_DISPLAY_PLANE_VIDEO,
							decode->x, decode->y, decode->w, decode->h, view->x, view->y, view->w, view->h);

				/* invalid rect일 경우 Image plane에 IFrame재생중일때는 video plane을 Image plane뒤로 모두 숨긴다 */
				if (port_itk_display_GetStillPlaneStatus(MHEG_DISPLAY_PLANE_STILL1) == TRUE)
				{
					mheg_itk_port_displayHideFullVideoRectOnImgStillPlane();
				}
			}
		}
		else
		{
			HxLOG_Info("mheg_itk_port_displaySetVideoScalingInfo : plane(Image STILL) (L:%d)\n", __LINE__);
			mheg_itk_port_displaySetVideoScalingInfo(MHEG_DISPLAY_PLANE_STILL1,
						0, 0, ITK_DISPLAY_WINDOW_W, ITK_DISPLAY_WINDOW_H,
						0, 0, ITK_DISPLAY_WINDOW_W, ITK_DISPLAY_WINDOW_H);
		}
	}
	else
	{
		if (plane == ITK_DISPLAY_PLANE_VIDEO)
		{
			if (port_itk_display_GetStillPlaneStatus(MHEG_DISPLAY_PLANE_STILL2) == TRUE)
			{
				HxLOG_Info("mheg_itk_port_displayChangeVideoPosition : plane(MHEG_DISPLAY_PLANE_STILL2) (L:%d)\n", __LINE__);
				mheg_itk_port_displayChangeVideoPosition(MHEG_DISPLAY_PLANE_STILL2,
							decode->x, decode->y, decode->w, decode->h, view->x, view->y, view->w, view->h);
			}
			else
			{
				HxLOG_Info("mheg_itk_port_displayChangeVideoPosition : plane(MHEG_DISPLAY_PLANE_VIDEO) (L:%d)\n", __LINE__);
				mheg_itk_port_displayChangeVideoPosition(MHEG_DISPLAY_PLANE_VIDEO,
							decode->x, decode->y, decode->w, decode->h, view->x, view->y, view->w, view->h);
			}

			HxLOG_Info("mheg_itk_port_displaySetVideoScalingInfo : plane(MHEG_DISPLAY_PLANE_STILL2) (L:%d)\n", __LINE__);
			mheg_itk_port_displaySetVideoScalingInfo(MHEG_DISPLAY_PLANE_STILL2,
						decode->x, decode->y, decode->w, decode->h, view->x, view->y, view->w, view->h);

			HxLOG_Info("mheg_itk_port_displaySetVideoScalingInfo : plane(VIDEO) (L:%d)\n", __LINE__);
			mheg_itk_port_displaySetVideoScalingInfo(MHEG_DISPLAY_PLANE_VIDEO,
						decode->x, decode->y, decode->w, decode->h, view->x, view->y, view->w, view->h);
		}
		else
		{
			mheg_itk_port_displayChangeVideoPosition(MHEG_DISPLAY_PLANE_STILL1,
						decode->x, decode->y, decode->w, decode->h, view->x, view->y, view->w, view->h);

			HxLOG_Info("mheg_itk_port_displaySetVideoScalingInfo : plane(Image STILL) (L:%d)\n", __LINE__);
			mheg_itk_port_displaySetVideoScalingInfo(MHEG_DISPLAY_PLANE_STILL1,
						decode->x, decode->y, decode->w, decode->h, view->x, view->y, view->w, view->h);
		}
	}
}

/**
 * void (*getDisplayPoint)(struct itk_display_t *thiz, const
 *  						 itk_point_t
 *  						  videoPoint,itk_point_t
 *  						  outputPoint);

 *
 * @author swkim (2011-08-09)
 *
 * @param thiz
 * @param video_point
 * @param output_point
 */
void mheg_itk_port_displayGetDisplayPoint(struct itk_display_t *thiz, const itk_point_t *video_point, itk_point_t *output_point)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	output_x = 0, output_y = 0;
	HUINT32	ulViewId;

	if (thiz == NULL)
	{
		HxLOG_Error(" input parameter is invalid NULL\r\n");
		return;
	}

	if (video_point == NULL || output_point == NULL)
	{
		HxLOG_Error(" video_point or output_point is NULL\r\n");
		return;
	}

	if (video_point->x < 0 || video_point->x >= ITK_DISPLAY_WINDOW_W || video_point->y < 0 || video_point->y >= ITK_DISPLAY_WINDOW_H)
	{
		HxLOG_Error(" video_point is invalid (%ld, %ld)\r\n", video_point->x, video_point->y);
		return;
	}

	PORT_ITK_GetMastViewId(&ulViewId);
	hErr = APK_MEDIA_PLAY_GetOsdDisplayPoint(ulViewId, ITK_DISPLAY_WINDOW_W, ITK_DISPLAY_WINDOW_H, video_point->x, video_point->y, &output_x, &output_y);
	if(hErr == ERR_OK)
	{
		HxLOG_Info("\t\tPORT_ITK_Display_GetDisplayPoint(), (%d, %d) -> (%d, %d)\n", video_point->x, video_point->y, output_x, output_y);
		output_point->x = output_x;
		output_point->y = output_y;
	}
}

/**
 * Sets the player output volume.
 *
 * The level has the following values:
 * - 0 means leave the volume unchanged
 * - -MAXINT means mute
 * - >0 means louder than current
 * - <0 means quiet than current (may be implemented as mute)
 *
 * @param[in] thiz The media
 * @param[in] level The volume to set
 */
void mheg_itk_port_displaySetVolume(struct itk_display_t *thiz, itk_int32_t level)
{
	HINT32	current_volume = 0, next_volume = 0, prev_volume = 0, nVolRate, nVolMax;
	HERROR	ulErr = ERR_OK;

	if (thiz == NULL)
	{
		HxLOG_Error(" input parameter is invalid NULL\n");
		return;
	}

	if (APK_OUTPUT_AUDIO_GetVolume(&current_volume) != ERR_OK)
	{
		HxLOG_Error("Fail to get current volume\n");
		return;
	}

	if (APK_OUTPUT_AUDIO_GetVolumeMax(&nVolMax) != ERR_OK)
	{
		HxLOG_Error("Fail to get max volume!\n");
		return;
	}


	// settop 의 경우 volume이 0 ~ 20 range 이며 1 step이 2dB임.
	// level 값이 -8dB 이면 -4 step을 하면 됨 (DD 담당자에게 확인이 필요함)
	nVolRate = (level / 2) * (nVolMax / 20); // UI 에 포시되는 Max volume 이 20 일 경우

	prev_volume = current_volume;

	HxLOG_Info("level = %ld, current_volume = %d, s_stDisplayInfo.nChangedVolume= %d, VolRate (%d) volumeRange(%d)\n", level, current_volume, s_stDisplayInfo.nChangedVolume, nVolRate, nVolMax);

	current_volume -= s_stDisplayInfo.nChangedVolume;

	if (level == 0)
	{
		next_volume = current_volume;
	}
	else if (level == -MAXINT)
	{
		next_volume = 0;
	}
	else if (level > 0 || level < 0)
	{
		next_volume = current_volume + nVolRate;
	}

	if (next_volume < 0)
	{
		next_volume = 0;
	}
	else if (next_volume > nVolMax)
	{
		next_volume = nVolMax;
	}

	if (prev_volume != next_volume)
	{
		ulErr = APK_OUTPUT_AUDIO_SetVolume(next_volume);

		if(ulErr != ERR_OK)
		{
			HxLOG_Error(" change volume Fail..\n");
			return;
		}
	}

	s_stDisplayInfo.nChangedVolume = next_volume - current_volume;

	HxLOG_Info("level = %ld, volume chaged to = %d\n", level, next_volume);
}

/**
 * Gets a frame buffer from the display.
 *
 * @param[in] thiz The display
 *
 * @return A handle to the frame buffer on success, NULL otherwise.
 */
itk_dfb_t *mheg_itk_port_displayGetFrameBuffer(struct itk_display_t *thiz)
{
	return NULL;
}

/**
 * Gets the presentation state of subtitles.  If the user has selected to
 * to display subtitles and the subtitle data is being received then the
 * subtitles are considered to be "present".
 *
 * @param[in] thiz The display
 *
 * @return ITK_PRESENT If subtitles are being displayed.
 * @return ITK_NOT_PRESENT If subtitles are not being displayed.
 *
 */
itk_present_t mheg_itk_port_displayGetSubtitlePresentationState(struct itk_display_t *thiz)
{
	HxLOG_Debug("thiz (%p)\n", thiz);

	if (thiz == NULL)
	{
		HxLOG_Error("input parameter is invalid NULL\n");
		return ITK_NOT_PRESENT;
	}

	/* MHEG ICS 012 Subtitle Control from HDR2000T */
	return  AP_Mheg_IsExistSubtitle();
}

/**
 * refer to itk_display.h - itk_avstream_t *(*newNativeAvstream)
     *
 * @author
     *
 * @param thiz
 * @param reference
 * @param name
 * @param description
 * @param resume
 * @param guidance
 * @param callback
 * @param callbackContext
     *
 * @return itk_avstream_t*
     */
itk_avstream_t * mheg_itk_port_displayNativeAVCNewStream(struct itk_display_t const *thiz,
														const char *reference,
														const char *name,
														const char *description,
														itk_int8_t resume,
														itk_int8_t guidance,
														itk_avstream_callback_t callback,
														void *callbackContext)
{
	HxLOG_Trace();

	UNUSED(thiz);
	UNUSED(reference);
	UNUSED(name);
	UNUSED(description);
	UNUSED(resume);
	UNUSED(guidance);
	UNUSED(callback);
	UNUSED(callbackContext);
	return NULL;
}

/**
 * refer to itk_display.h - itk_present_t *(*requestMPEGDecoder)
     *
 * @author
     *
 * @param thiz
 * @param request

 * @return itk_present_t
     */
itk_present_t  mheg_itk_port_displayRequestMPEGDecoder(struct itk_display_t *thiz, itk_control_t request)
{
	itk_present_t res;

	HxLOG_Trace();

	if (thiz == NULL)
	{
		HxLOG_Error("input parameter is invalid NULL\n");
		return ITK_NOT_PRESENT;
	}

	if (request == ITK_ENABLE)
	{
		// TODO : 다른 application이 실행되어 있는지 확인
		res = ITK_PRESENT;
	}
	else // ITK_DISABLE
	{
		res = ITK_NOT_PRESENT;
	}

	return res;
}

/**
 * refer to itk_display.h - itk_present_t *(*requestMPEGDecoder)
     *
 * @author
     *
 * @param thiz
 * @param request

     */
void mheg_itk_port_displayReleaseGraphicsPlane(struct itk_display_t *thiz, itk_control_t request)
{
	HxLOG_Trace();

	if (thiz == NULL)
	{
		HxLOG_Error("input parameter is invalid NULL\n");
		return;
	}

	if (request == ITK_ENABLE)
	{
		// subtitle 과 같이 사용하는 single plane 인 경우에만 사용하면 함
	}
	else // ITK_DISABLE
	{

	}
}

#define	___PUBLIC_INTERNAL_FUNCTION____
/**
 * Get status whether still-plane has been started or not.
 *
 * @author swkim (2011-08-09)
 *
 * @param plane
 *
 * @return HBOOL
 */
HBOOL port_itk_display_GetStillPlaneStatus(HUINT8 plane)
{
	if (plane >= MHEG_DISPLAY_PLANE_STILL_MAX)
		return FALSE;

	return s_stDisplayInfo.bIFrameStarted[plane];
}

/**
 * Set still-plane status
 *
 * @author swkim (2011-08-10)
 *
 * @param plane [in]
 * @param bStarted [in]
 */
void port_itk_display_SetStillPlaneStatus(HUINT8 plane, HBOOL bStarted)
{
	if (plane >= MHEG_DISPLAY_PLANE_STILL_MAX)
		return;

	s_stDisplayInfo.bIFrameStarted[plane] = bStarted;
}
/**
 * release DirectFB resources (surface, window, display layer, interface)
 *
 * @author swkim (2011-08-09)
 *
 * @return HINT32 : result status (success or fail)
 */
HERROR port_itk_display_ReleaseStill(void)
{
	IDirectFBWindow*	pSubWindow;		// Still
	IDirectFBSurface*	pSubSurface;	// Still

	pSubWindow = s_stDisplayInfo.dfbInfo.dfbStillWindow;
	pSubSurface = s_stDisplayInfo.dfbInfo.dfbStillSurface;

	HxLOG_Info("Release: s_stDisplayInfo.dfbInfo .pImgStillWindow(%p), .pSurface(%p)\n", pSubWindow, pSubSurface);

	if (pSubSurface)	DFBCHECK(pSubSurface->Release(pSubSurface)); 	pSubSurface = NULL;
	if (pSubWindow)		DFBCHECK(pSubWindow->Release(pSubWindow)); 		pSubWindow = NULL;

	return ERR_OK;
}

MHEG_DFBInfo_t*	port_itk_display_getDfbInfo(void)
{
	return &s_stDisplayInfo.dfbInfo;
}

void port_itk_display_flipWindow(IDirectFBSurface *pSurface, DFBRegion *pRegion, DFBSurfaceFlipFlags flipFlags)
{
	mheg_itk_displayFlipWindow(pSurface, pRegion, flipFlags);
}

#define	___PUBLIC_FUNCTION____
HERROR PORT_ITK_DISPLAY_CreateObject(redkey_profile_t eItkProfile)
{
	MHEG_DisplayInfo_t	*pstInfo = &s_stDisplayInfo;

	HxLOG_Debug("Display Object Profile : [%d]\n", eItkProfile);

	mheg_itk_display_initializeInfo();

	pstInfo->itkProfile = eItkProfile;

	pstInfo->object.getCaps				= mheg_itk_port_displayGetCaps;
	pstInfo->object.setLogicalSize		= mheg_itk_port_displaySetLogicalSize;
	pstInfo->object.addEventCallback	= mheg_itk_port_displayAddEventCallback;
	pstInfo->object.removeEventCallback	= mheg_itk_port_displayRemoveEventCallback;
	pstInfo->object.addKeyCallback		= mheg_itk_port_displayAddKeyCallback;
	pstInfo->object.setKeyMask			= mheg_itk_port_displaySetKeyMask;
	pstInfo->object.checkKeyMask		= mheg_itk_port_displayCheckKeyMask;
	pstInfo->object.removeKeyCallback	= mheg_itk_port_displayRemoveKeyCallback;
	pstInfo->object.setKeyboard			= mheg_itk_port_displaySetKeyboard;
	pstInfo->object.getShape			= mheg_itk_port_displayGetShape;
	pstInfo->object.setPresentationShape= mheg_itk_port_displaySetPresentationShape;
	pstInfo->object.setDFCMode			= mheg_itk_port_displaySetDFBMode;
	pstInfo->object.setBackgroundColour	= mheg_itk_port_displaySetBackgroundColor;
	pstInfo->object.setSubtitle			= mheg_itk_port_displaySetSubtitle;
	pstInfo->object.setDisplayWindow	= mheg_itk_port_displaySetDisplayWindow;
	pstInfo->object.getDisplayPoint		= mheg_itk_port_displayGetDisplayPoint;
	pstInfo->object.setVolume			= mheg_itk_port_displaySetVolume;
	pstInfo->object.getFrameBuffer		= mheg_itk_port_displayGetFrameBuffer;
	pstInfo->object.getSubtitlePresentationState	= mheg_itk_port_displayGetSubtitlePresentationState;
	pstInfo->object.newNativeAvstream	= mheg_itk_port_displayNativeAVCNewStream;
	pstInfo->object.requestMPEGDecoder	= mheg_itk_port_displayRequestMPEGDecoder;		// for CI+ Profile
	pstInfo->object.releaseGraphicsPlane= mheg_itk_port_displayReleaseGraphicsPlane;	// for CI+ Profile

	pstInfo->object.newImage			= port_itk_image_NewImage;
	pstInfo->object.newAudioclip		= port_itk_audioclip_NewAudioClip;
	pstInfo->object.newAvstream			= port_itk_avs_NewAvStream;
	pstInfo->object.getPipeline			= port_itk_pipeline_Get;

	// according to each profile, set NULL to usused function
	if ( (eItkProfile != REDKEY_PROFILE_UK_FSAT)
		&& (eItkProfile != REDKEY_PROFILE_FREESATG2))
	{
		pstInfo->object.newNativeAvstream = NULL;
	}

	if ((eItkProfile != REDKEY_PROFILE_SOUTH_AFRICA)
		&& (eItkProfile != REDKEY_PROFILE_CIPLUS))
	{
		pstInfo->object.checkKeyMask = NULL;
	}

	if (eItkProfile == REDKEY_PROFILE_CIPLUS)
	{
		pstInfo->object.setKeyboard = NULL;
		pstInfo->object.setBackgroundColour = NULL;
	}
	else
	{
		pstInfo->object.getSubtitlePresentationState	= NULL;
		pstInfo->object.requestMPEGDecoder 			= NULL;
		pstInfo->object.releaseGraphicsPlane			= NULL ;
	}

	port_itk_pipeline_init();
	port_itk_avs_init();

	ITK_registerManager(ITK_DISPLAY, &pstInfo->object);

	HxSEMT_Create(&pstInfo->ulKeyMaskSemId,    "sHostKM",  HxSEMT_PRIORITY);
	HxSEMT_Create(&pstInfo->ulImageStillSemId, "sHostIS", HxSEMT_PRIORITY);

	return ERR_OK;
}

void	PORT_ITK_DISPLAY_InitVideoScalingInfo(void)
{
	//HxLOG_Trace();

	mheg_itk_port_displaySetVideoScalingInfo(MHEG_DISPLAY_PLANE_STILL2,
		0, 0, ITK_DISPLAY_WINDOW_W, ITK_DISPLAY_WINDOW_H,
		0, 0, ITK_DISPLAY_WINDOW_W, ITK_DISPLAY_WINDOW_H);

	mheg_itk_port_displaySetVideoScalingInfo(MHEG_DISPLAY_PLANE_STILL1,
		0, 0, ITK_DISPLAY_WINDOW_W, ITK_DISPLAY_WINDOW_H,
		0, 0, ITK_DISPLAY_WINDOW_W, ITK_DISPLAY_WINDOW_H);

	mheg_itk_port_displaySetVideoScalingInfo(MHEG_DISPLAY_PLANE_VIDEO,
		0, 0, ITK_DISPLAY_WINDOW_W, ITK_DISPLAY_WINDOW_H,
		0, 0, ITK_DISPLAY_WINDOW_W, ITK_DISPLAY_WINDOW_H);
}

HINT32 PORT_ITK_DISPLAY_Initialize(void)
{
	IDirectFB				*pDfb = NULL;
	IDirectFBWindow			*pWindow = NULL;
	IDirectFBSurface		*pSurface = NULL;
	IDirectFBDisplayLayer	*pLayer = NULL;
	DFBWindowID				nDfbWndId;
	DFBWindowID				nDfbStillWndId;
	IDirectFBWindow			*pImgStillWindow = NULL;
	IDirectFBSurface		*pImgStillSurface = NULL;
	DFBWindowDescription	desc;

	HUINT32					ulTaskId;

	DFBCHECK(DirectFBInit(NULL, NULL));
	DFBCHECK(DirectFBCreate(&pDfb));

	DFBCHECK(pDfb->GetDisplayLayer(pDfb, DLID_PRIMARY, &pLayer));

	//	Main MHEG Window.
	DFBCHECK(pLayer->SetCooperativeLevel(pLayer, DLSCL_ADMINISTRATIVE));
	HxSTD_MemSet(&desc, 0, sizeof(DFBWindowDescription));
	desc.flags = (DFBWindowDescriptionFlags)(DWDESC_POSX |
									DWDESC_POSY |
									DWDESC_WIDTH |
									DWDESC_HEIGHT |
									DWDESC_CAPS |
									DWDESC_OPTIONS |
									DWDESC_PIXELFORMAT |
									DWDESC_SURFACE_CAPS
									);
	desc.posx = 0;
	desc.posy = 0;
	desc.width = ITK_DISPLAY_PHY_WIN_W;
	desc.height = ITK_DISPLAY_PHY_WIN_H;
	desc.caps = (DFBWindowCapabilities)DWCAPS_ALPHACHANNEL;
	desc.options = (DFBWindowOptions)(DWOP_ALPHACHANNEL | DWOP_SCALE);
	desc.pixelformat = (DFBSurfacePixelFormat)DSPF_ARGB;
	desc.surface_caps = (DFBSurfaceCapabilities)DSCAPS_PREMULTIPLIED;

	DFBCHECK(pLayer->CreateWindow(pLayer, &desc, &pWindow));
	DFBCHECK(pWindow->SetApplicationID(pWindow, eAWINDOW_MHEGPLAYER));
	DFBCHECK(pWindow->GetID(pWindow, &nDfbWndId));

	DFBCHECK(pWindow->GetSurface(pWindow, &pSurface));
	DFBCHECK(pSurface->Clear(pSurface, 0, 0, 0, 0));
	DFBCHECK(pWindow->SetOpacity(pWindow, 0xFF));

	HAPI_AddVirtualWindow(eAWINDOW_MHEGPLAYER);
	HAPI_RegisterAcceptableKey(eAWINDOW_MHEGPLAYER, DIKS_RED, TRUE); // TODO : 한개라도 등록하지 않으면 HLIB이 오작동하여 등록함

	if(HAPI_SetWindowStyle(eAWINDOW_MHEGPLAYER, eHWS_WindowInputEnable, FALSE) != ERR_OK)
		HxLOG_Error("[HAPI_SetWindowStyle] Err");

	if ((s_stDisplayInfo.itkProfile == REDKEY_PROFILE_UK_FSAT) || (s_stDisplayInfo.itkProfile == REDKEY_PROFILE_FREESATG2))
	{
		// Image Still Plane Window 생성
		HxSTD_MemSet(&desc, 0, sizeof(DFBWindowDescription));
		desc.flags = (DFBWindowDescriptionFlags)(DWDESC_POSX |
										DWDESC_POSY |
										DWDESC_WIDTH |
										DWDESC_HEIGHT |
										DWDESC_CAPS |
										DWDESC_OPTIONS |
										DWDESC_PIXELFORMAT
										);
		desc.posx = 0;
		desc.posy = 0;
		desc.width = ITK_DISPLAY_PHY_WIN_W;
		desc.height = ITK_DISPLAY_PHY_WIN_H;
		desc.caps = (DFBWindowCapabilities)(DWCAPS_ALPHACHANNEL|DWCAPS_NOFOCUS);
		desc.options = (DFBWindowOptions)(DWOP_ALPHACHANNEL|DWOP_GHOST);
		desc.pixelformat = (DFBSurfacePixelFormat)DSPF_ARGB;
		DFBCHECK(pLayer->CreateWindow(pLayer, &desc, &pImgStillWindow));
		DFBCHECK(pImgStillWindow->GetID(pImgStillWindow, &nDfbStillWndId));
		DFBCHECK(pImgStillWindow->SetApplicationID(pImgStillWindow, eAWINDOW_MHEGPLAYER_IMG_STILL));

		HAPI_SetWindowStyle(eAWINDOW_MHEGPLAYER_IMG_STILL, 0, TRUE);

		DFBCHECK(pImgStillWindow->GetSurface(pImgStillWindow, &pImgStillSurface));
		DFBCHECK(pImgStillSurface->Clear(pImgStillSurface, 0, 0, 0, 0));
		DFBCHECK(pImgStillWindow->SetOpacity(pImgStillWindow, 0xFF));
	}

	/* set mheg display info */
	s_stDisplayInfo.dfbInfo.dfb = pDfb;
	s_stDisplayInfo.dfbInfo.dfbLayer = pLayer;
	s_stDisplayInfo.dfbInfo.dfbWindow = pWindow;
	s_stDisplayInfo.dfbInfo.dfbSurface = pSurface;

	s_stDisplayInfo.dfbInfo.dfbStillWindow = pImgStillWindow;
	s_stDisplayInfo.dfbInfo.dfbStillSurface = pImgStillSurface;

	/* Create Key Process Task */
	VK_TASK_Create(mheg_itk_displayKeyProcessTask, 40, 0x4000, "mhegKeyProcessTask", (void*)NULL, (HULONG*)&ulTaskId, 0);
	VK_TASK_Start(ulTaskId);

	HxLOG_Info("initialise DirectFB Interface, dfbWndId : %d, pDfb : %p, pSurface : %p, pWindow : %p\n", nDfbWndId, pDfb, pWindow, pSurface);

	return ERR_OK;
}

HERROR	PORT_ITK_DISPLAY_SendKey(HUINT32 ulKey, itk_validation_t *peValidation)
{
	if (s_stDisplayInfo.pfnKeyCallback)
		*peValidation = s_stDisplayInfo.pfnKeyCallback(s_stDisplayInfo.pfnKeyCallbackContext, &s_stDisplayInfo.object, ulKey);
	else
	{
		HxLOG_Error("Key callback has not been registered\n");
		*peValidation = ITK_NOT_VALID;
	}

	return ERR_OK;
}

void	PORT_ITK_DISPLAY_ShowImage(HUINT8 plane, HUINT8* pBuffer, HUINT32 ulBufferSize, itk_image_format_t format)
{
	DxVideoCodec_e	eVideoCodec;
	HERROR			hErr = ERR_FAIL;
	HUINT32			ulViewId;

	PORT_ITK_GetMastViewId(&ulViewId);

	if (format == ITK_IMAGE_H264)
		eVideoCodec = eDxVIDEO_CODEC_H264;
	else
		eVideoCodec = eDxVIDEO_CODEC_MPEG2;

	if (plane == MHEG_DISPLAY_PLANE_STILL1)
	{
		IDirectFBSurface* pImgStillSurface;

		hErr = APK_MEDIA_PLAY_CaptureStillPicture(ulViewId, eVideoCodec, ITK_DISPLAY_OSD_HD_W, ITK_DISPLAY_OSD_HD_H, eDxPixelFormat_ARGB_8888, ulBufferSize, pBuffer);
		if(hErr == ERR_OK)
		{
			ENTER_CRITICAL_IS;

			pImgStillSurface = s_stDisplayInfo.dfbInfo.dfbStillSurface;
			if(pImgStillSurface)
			{
				mheg_itk_displayFillSurface(pImgStillSurface, s_stDisplayInfo.stBgColor.red, s_stDisplayInfo.stBgColor.green, s_stDisplayInfo.stBgColor.blue, 0xFF);
			}

			LEAVE_CRITICAL_IS;
		}
		else
		{
			HxLOG_Error("APK_MEDIA_PLAY_CaptureStillPicture() returned ERROR!!!\n");
		}
	}
	else if (plane == MHEG_DISPLAY_PLANE_STILL2)
	{
		/* Async */
		hErr = APK_MEDIA_PLAY_StartStillPicture(ulViewId, eVideoCodec, ulBufferSize, pBuffer);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("APK_MEDIA_PLAY_StartStillPicture() returned ERROR!!!\n");
		}
	}
}

void	PORT_ITK_DISPLAY_HideImage(HUINT8 plane)
{
	HUINT32			ulViewId;

	PORT_ITK_GetMastViewId(&ulViewId);

	if (plane == MHEG_DISPLAY_PLANE_STILL1)
	{
		IDirectFBSurface*		pImgStillSurface;
		IDirectFBSurface*		pImgStillBackupSurface; 	// Image Still backup surface

		ENTER_CRITICAL_IS;

		pImgStillSurface = s_stDisplayInfo.dfbInfo.dfbStillSurface;
		pImgStillBackupSurface = s_stDisplayInfo.dfbInfo.dfbStillBackupSurface;		// Image Still backup surface

		if(pImgStillSurface)
		{
			mheg_itk_displayFillSurface(pImgStillSurface, 0, 0, 0, 0);
		}

		if(pImgStillBackupSurface)
		{
			DFBCHECK(pImgStillBackupSurface->Release(pImgStillBackupSurface));
			s_stDisplayInfo.dfbInfo.dfbStillBackupSurface = NULL;
		}

		LEAVE_CRITICAL_IS;
	}
	else if (plane == MHEG_DISPLAY_PLANE_STILL2)
	{
		// I-Frame과 Video를 동시에 띄우려고 할 경우 행업되는 현상을 막기 위해 Video를 띄우기 전에 I-Frame stop 처리
		/* Async */
		APK_MEDIA_PLAY_StopStillPicture(ulViewId);
	}

	port_itk_display_SetStillPlaneStatus(plane, FALSE);
}

void PORT_ITK_DISPLAY_UpdateVideoStatus(void)
{
	/*
		mheg이 AR or DFC를 고정후 video pid만 변경시 해당 video의 AR을 반영해야 하는데, MW로 부터 Video관련 event를 받아서 다시 세팅한다.
		ex) 6.6 Aspect Ratio Scene 3/4 에서 4:3 <-> 16:9로 전환시 4:3은 pillar, 16:9는 None으로 처리해야함
			AR, DFC세팅 -> SetDisplayWindow -> Video전환시 매번 적용해야함(AR, DFC, SetDisplayWindow는 video play이전에 내려준다)
	*/
	mheg_itk_port_displayUpdateVideoWindow(NULL);
}

void	PORT_ITK_DISPLAY_Restore(itk_service_tune_event_t eTuneEvent)
{
	HERROR					hErr;

	/* Video관련 정보 default로 설정 */
	mheg_itk_display_initializeVideoContext();
	hErr = mheg_itk_port_displayUpdateVideoWindow(NULL);
#ifdef DEBUG
	if (hErr != ERR_OK)
	{
		HxLOG_Error("ERROR : while calling local_itk_Display_UpdateVideoWindow \n");
	}
#endif

//	SVC_OUT_SetBackgroundColor(0, 0, 0);

	if (s_stDisplayInfo.nChangedVolume)
	{
		HINT32	current_volume = 0;

		if (APK_OUTPUT_AUDIO_GetVolume(&current_volume) != ERR_OK)
		{
			HxLOG_Error("Fail to get current volume\n");
			return;
		}

		current_volume -= s_stDisplayInfo.nChangedVolume;

		if(APK_OUTPUT_AUDIO_SetVolume(current_volume) != ERR_OK)
		{
			HxLOG_Error("Fail to set current volume\n");
			return;
		}

		s_stDisplayInfo.nChangedVolume = 0;
	}

	UNUSED(hErr);
}


