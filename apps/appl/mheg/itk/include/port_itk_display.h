/**
	@file     port_itk_display.h
	@brief    ITK Host Display 모듈 헤더파일.

	Description: Porinting Layer 함수 선언을 포함 함.
	Module: mw/mheg/itk

	Copyright (c) 2008 HUMAX Co., Ltd.
	All rights reserved.
*/
#ifndef _PORT_ITK_DISPLAY_H_
#define _PORT_ITK_DISPLAY_H_

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <directfb.h>
#include "itk_display.h"
#include "itk_service.h"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*
* Display Event
*/
typedef enum
{
	eITK_DISPLAY_EVENT_STOP,
	eITK_DISPLAY_EVENT_PLAY,
	eITK_DISPLAY_EVENT_PAUSE
} ITK_DIsplay_State_t;


/** @brief Display frame buffer handle type */
typedef struct ITK_FramebufferHandle_t
{
	itk_dfb_t			vtbl;

	IDirectFBSurface	*pSurface;
	itk_rect_t			stClip;

	HUINT32			ulPitch;
	HUINT8			ucVisible;
	HBOOL			bIsLock;
	itk_pixel_format_t PixelFormat;
} ITK_FramebufferHandle_t;

/** @brief Display graphics pipeline handle type */
typedef struct ITK_PipelineHandle_t
{
	itk_graphics_pipeline_t	vtbl;

	IDirectFBSurface	*pSurface;
	HUINT8				ucVisible;
	itk_rect_t			stClip;
	itk_pixel_format_t	PixelFormat;
	HBOOL				bDrawStarted;
} ITK_PipelineHandle_t;


typedef struct ITK_MaskHandle_t
{
	itk_mask_t			mask;
	IDirectFBSurface	*pMaskSurface;
} ITK_MaskHandle_t;

typedef struct ITK_BitmapHandle_t
{
	itk_bitmap_t		bitmap;
	IDirectFBSurface	*pBitmapSurface;
} ITK_BitmapHandle_t;

/** @brief Display Frame buffer Bitmap */
typedef struct ITK_bitmap_funcs_t
{
    void *(*alloc)(struct ITK_bitmap_funcs_t *thiz, size_t size);
    void *(*zalloc)(struct ITK_bitmap_funcs_t *thiz, size_t size);
    void (*free)(struct ITK_bitmap_funcs_t *thiz, void *alloc);
    itk_bitmap_t *(*createBitmap)(struct ITK_bitmap_funcs_t *thiz, itk_uint32_t w, itk_uint32_t h, itk_pixel_format_t format);
    void (*freeBitmap)(struct ITK_bitmap_funcs_t *thiz, itk_bitmap_t *b);
} ITK_bitmap_funcs_t;

/*
* for SD resolutions of 720x576
*/
#define ITK_DISPLAY_OSD_SD_W	720
#define ITK_DISPLAY_OSD_SD_H	576

/*
* for HD resolutions of 1280x720
*/
#define ITK_DISPLAY_OSD_HD_W	1280
#define ITK_DISPLAY_OSD_HD_H	720

/*
* HUMAX Comment : physical window size 1280x720
*/
#if defined(CONFIG_3RD_MHEG_UK_PROFILE1)
#define ITK_DISPLAY_PHY_WIN_W	ITK_DISPLAY_OSD_SD_W
#define ITK_DISPLAY_PHY_WIN_H	ITK_DISPLAY_OSD_SD_H
#else
#define ITK_DISPLAY_PHY_WIN_W	ITK_DISPLAY_OSD_HD_W
#define ITK_DISPLAY_PHY_WIN_H	ITK_DISPLAY_OSD_HD_H
#endif

typedef struct
{
	IDirectFB				*dfb;
	IDirectFBWindow			*dfbWindow;
	IDirectFBWindow			*dfbStillWindow;
	IDirectFBSurface		*dfbSurface;
	IDirectFBSurface		*dfbStillSurface;
	IDirectFBSurface		*dfbStillBackupSurface;
	IDirectFBDisplayLayer	*dfbLayer;
} MHEG_DFBInfo_t;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern HBOOL	port_itk_display_GetStillPlaneStatus(HUINT8 plane);
extern void		port_itk_display_SetStillPlaneStatus(HUINT8 plane, HBOOL bStarted);
extern HERROR	port_itk_display_ReleaseStill(void);
extern MHEG_DFBInfo_t*	port_itk_display_getDfbInfo(void);
extern void		port_itk_display_flipWindow(IDirectFBSurface *pSurface, DFBRegion *pRegion, DFBSurfaceFlipFlags flipFlags);

extern HERROR	PORT_ITK_DISPLAY_CreateObject(redkey_profile_t eItkProfile);
extern void		PORT_ITK_DISPLAY_InitVideoScalingInfo(void);
extern HINT32	PORT_ITK_DISPLAY_Initialize(void);
extern HERROR	PORT_ITK_DISPLAY_SendKey(HUINT32 ulKey, itk_validation_t *peValidation);
extern void		PORT_ITK_DISPLAY_ShowImage(HUINT8 plane, HUINT8* pBuffer, HUINT32 ulBufferSize, itk_image_format_t format);
extern void		PORT_ITK_DISPLAY_HideImage(HUINT8 plane);
extern void 	PORT_ITK_DISPLAY_UpdateVideoStatus(void);
extern void		PORT_ITK_DISPLAY_Restore(itk_service_tune_event_t eTuneEvent);
#endif	/* _PORT_ITK_DISPLAY_H_ */

