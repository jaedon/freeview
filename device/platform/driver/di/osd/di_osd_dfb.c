/*
 * $Header:   $
 */
/*********************************************************************
 * $Workfile:   di_osd.c  $
 * $Modtime:   Jul 16 2007 01:07:04  $
 *
 * Author:
 * Description:
 *
 *                                 Copyright (c) 2008 HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

/*********************************************************************
 * Header Files
 *********************************************************************/
#include "linden_trace.h"

#include "htype.h"
#include "di_err.h"
#include "di_uart.h"
#include "di_video.h"
#include "drv_video.h"
#include "di_osd.h"
#include "drv_osd.h"
#include "vkernel.h"
#include "taskdef.h"

#if defined (CONFIG_DIRECTFB)
#include <directfb.h>
#include "platform_init.h"
#endif

#include "nexus_platform.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_display.h"

#if defined (CONFIG_DIRECTFB)
#define A(a)							((a >> 24) & 0xFF)
#define R(r)							((r >> 16) & 0xFF)
#define G(g)							((g >>  8) & 0xFF)
#define B(b)							((b >>  0) & 0xFF)
#endif

#define DUAL_OUTPUT
#define UNUSED(x) (void)(x)
#define AssertNEXUS(arg1, arg2) 	if(arg1 != NEXUS_SUCCESS) {PrintError("ERROR code : 0x%x\n",arg1); return arg2;}

HBOOL	g_BlockPrint[DI_OSD_SURFACE_MAX_NUM];

#define DEBUG_MODULE				TRACE_MODULE_DI_OSD
#define PrintStatus					DI_UART_Print
static unsigned long 			s_ulOsdSemId;

#define	DiLog_Assert(expr)	\
	if (!(expr)) {\
		PrintDebug("%s:%d:%s: Assertion failed!!\n", __FILE__, __LINE__, __FUNCTION__);\
		abort();\
	}

typedef enum tagDI_BufferType_e
{
	eDI_BufferType_None		= 0x00,

	eDI_BufferType_Surface,
	eDI_BufferType_FrameBuffer,

	eDI_BufferType_Max
} DI_BufferType_e;

/* DirectFB interfaces needed by df_andi */
typedef struct tagDFBInterface_t
{
	IDirectFB			   	*dfb;
	IDirectFBSurface 	   	*primary;
	IDirectFBScreen		 	*primary_screen;
	IDirectFBEventBuffer  	*keybuffer;

#ifdef DUAL_OUTPUT
	IDirectFBSurface 	   	*secondary;
	IDirectFBDisplayLayer	*secondary_layer;
#endif
	IDirectFBImageProvider  *provider;
	DFBScreenEncoderConfig	encoderCfg;
} HDFBInterface_t;

typedef struct tagDFB_Context_t
{
	IDirectFBSurface 	   	*surface;
	void					*buffer;
	HINT32					pitch;

	DI_BufferType_e		type;
	HBOOL					bIsLock;
	HBOOL					bIsShow;

	HINT32					w, h;
} DFB_Context_t;

typedef struct tagDFBMgr_t
{
	HDFBInterface_t	dif;

	HINT32 xres;
	HINT32 yres;


	DFB_Context_t	*pConList;
} DFBMgr_t;

NEXUS_DisplayHandle 	HD_DispHandle;
NEXUS_DisplayHandle 	SD_DispHandle;

STATIC DFBMgr_t s_sDFBMgr;

DI_OSD_RECT				HD_ClipRect;
STATIC DFB_Context_t	*di_layer[DI_OSD_SURFACE_MAX_NUM];

STATIC DFBMgr_t *_di_osd_GetDFBMgr(void)
{
	return &s_sDFBMgr;
}

DI_ERR_CODE	DI_OSD_Lock(DI_OSD_HANDLE hHandle)
{
	DFB_Context_t			*pContext = (DFB_Context_t *)hHandle;

	if(pContext->bIsLock == FALSE)
	{
		pContext->surface->Lock(pContext->surface,  DSLF_READ | DSLF_WRITE, &pContext->buffer, &pContext->pitch);
		pContext->bIsLock = TRUE;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE	DI_OSD_Unlock(DI_OSD_HANDLE hHandle)
{
	DFB_Context_t			*pContext = (DFB_Context_t *)hHandle;

	if(pContext->bIsLock == TRUE)
	{
		pContext->surface->Unlock(pContext->surface);
		pContext->bIsLock = FALSE;
	}

	return DI_ERR_OK;
}


STATIC DI_ERR_CODE _di_osd_InitDirectFB( int argc, char *argv[] )
{
	DFBResult 				err;
	DFBSurfaceDescription 	dsc;
	DFBMgr_t 				*pDFBMgr = _di_osd_GetDFBMgr();

	/* HDFBInterface_t */
	HDFBInterface_t 		*pInterface = &pDFBMgr->dif;

#ifdef DUAL_OUTPUT
	DFBDisplayLayerConfig	layerCfg;
#endif

	err = DirectFBInit( &argc, &argv );
	if (err) {
		DirectFBError( "DirectFBInit", err );
		return DI_ERR_ERROR;
	}


	/* create the super interface */
	err = DirectFBCreate( &pInterface->dfb );
	if (err) {
		DirectFBError( "DirectFBCreate", err );
		return DI_ERR_ERROR;
	}

	err = pInterface->dfb->GetScreen( pInterface->dfb, DSCID_PRIMARY, &pInterface->primary_screen );
	if (err)
	{
		PrintError( "Failed to get primary screen\n", err );
	}

	/* create an input buffer for key events */
	err = pInterface->dfb->CreateInputEventBuffer( pInterface->dfb, DICAPS_KEYS,	DFB_FALSE, &pInterface->keybuffer );
	if (err) {
		DirectFBError( "CreateInputEventBuffer", err );
		return DI_ERR_ERROR;
	}

	/* set our cooperative level to DFSCL_FULLSCREEN for exclusive access to the primary layer */
	err = pInterface->dfb->SetCooperativeLevel( pInterface->dfb, DFSCL_FULLSCREEN );
	if (err)
	{
		PrintError( "Failed to get exclusive access\n", err );
	}

	/* get the primary surface, i.e. the surface of the primary layer we have exclusive access to */
	dsc.flags = DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT;
	dsc.caps = DSCAPS_PRIMARY | DSCAPS_DOUBLE;
	dsc.width = 1280;
	dsc.height= 720;
	dsc.pixelformat = DSPF_ARGB;

	err = pInterface->dfb->CreateSurface( pInterface->dfb, &dsc, &pInterface->primary );
	if (err)
	{
		dsc.caps = DSCAPS_PRIMARY | DSCAPS_DOUBLE;

		err = pInterface->dfb->CreateSurface( pInterface->dfb, &dsc, &pInterface->primary );
		if (err) {
			DirectFBError( "CreateSurface", err );
			return DI_ERR_ERROR;
		}
	}

#ifdef DUAL_OUTPUT
	err = pInterface->dfb->GetDisplayLayer(pInterface->dfb, DLID_PRIMARY+1, &pInterface->secondary_layer);
	err = pInterface->secondary_layer->SetCooperativeLevel(pInterface->secondary_layer, DLSCL_ADMINISTRATIVE);
	layerCfg.flags = DLCONF_SOURCE;
	layerCfg.source = DLSID_SURFACE;
	err = pInterface->secondary_layer->SetConfiguration(pInterface->secondary_layer, &layerCfg);
	err = pInterface->secondary_layer->GetSurface(pInterface->secondary_layer, &pInterface->secondary);
#if defined(CONFIG_TEMP_CDIG2000C)
#else
	pInterface->secondary->Flip( pInterface->secondary, NULL, DSFLIP_NONE );
#endif
#endif

	err = pInterface->primary->GetSize( pInterface->primary, &pDFBMgr->xres, &pDFBMgr->yres );

#if 1	// av control....
	pInterface->encoderCfg.flags			= (DFBScreenEncoderConfigFlags)(DSECONF_TV_STANDARD | DSECONF_SCANMODE | DSECONF_FREQUENCY | DSECONF_CONNECTORS | DSECONF_RESOLUTION );
	pInterface->encoderCfg.tv_standard 	= DSETV_DIGITAL;
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1
	pInterface->encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_COMPONENT | DSOC_HDMI);
#else
	pInterface->encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_HDMI);
#endif
	pInterface->encoderCfg.frequency 		= DSEF_50HZ;
	pInterface->encoderCfg.scanmode 		= DSESM_INTERLACED;
	pInterface->encoderCfg.resolution 	= DSOR_1920_1080;
#endif

	(pInterface->primary->Flip( pInterface->primary, NULL, DSFLIP_NONE ));

	err = pInterface->primary_screen->SetEncoderConfiguration(pInterface->primary_screen, 0, &pInterface->encoderCfg);

	pInterface->primary->SetDrawingFlags( pInterface->primary, DSDRAW_BLEND );
//	pInterface->primary->Clear( pInterface->primary, 0xff, 0xff, 0xff, 0xff );

	return DI_ERR_OK;
}

DI_ERR_CODE	DI_OSD_Init( int argc, char *argv[] )
{
	DI_ERR_CODE	dErr = DI_ERR_OK;
	DRV_Error				nRet = DRV_OK;

	dErr = _di_osd_InitDirectFB(argc, argv);
	if(dErr != DI_ERR_OK)
	{
		return DI_ERR_ERROR;
	}

	nRet = DRV_OSD_GetDisplayHandle(0, &HD_DispHandle, &SD_DispHandle);
	if (nRet != DRV_OK)
	{
		PrintError("DRV_OSD_GetDisplayHandle error!!\n");
		return DRV_ERR;
	}

	if( DRV_OSD_SetDisplayFormatToHD() != DRV_OK)
	{
		PrintError("DRV_OSD_SetDisplayFormat error!!\n");
		return DRV_ERR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE	DI_OSD_GetCapability(DI_OSD_CAPABILITY *pOsdCapability)
{
	PrintEnter();

	UNUSED(pOsdCapability);

	PrintExit();
	return DI_ERR_OK;
}



DI_ERR_CODE	DI_OSD_GetSurfaceInfo(DI_OSD_HANDLE hOsdHandle, DI_OSD_SURFACE_INFO *pInfo)
{
	PrintEnter();

	UNUSED(hOsdHandle);
	UNUSED(pInfo);

	PrintExit();
	return DI_ERR_OK;
}



DI_ERR_CODE	DI_OSD_GetFrameBufferInfo(DI_OSD_HANDLE hOsdHandle, DI_OSD_SURFACE_INFO *pInfo)
{
	PrintEnter();

	UNUSED(hOsdHandle);
	UNUSED(pInfo);

	PrintExit();
	return DI_ERR_OK;
}

DI_ERR_CODE	DI_OSD_CreateSurface(DI_OSD_LAYER nOsdLayer, DI_OSD_RECT rect, DI_OSD_PIXEL_FORMAT eColorFormat, DI_OSD_HANDLE *pOsdHandle)
{
	DFB_Context_t			*pContext = NULL;

	PrintEnter();

	DI_OSD_CreateFrameBuffer(rect.width, rect.height, eColorFormat, pOsdHandle);
	pContext = (DFB_Context_t *)*pOsdHandle;
	pContext->type	= eDI_BufferType_Surface;
	di_layer[nOsdLayer] = pContext;

	PrintExit();
	return DI_ERR_OK;
}

DI_ERR_CODE	DI_OSD_CreateFrameBuffer( unsigned long width, unsigned long height, DI_OSD_PIXEL_FORMAT eColorFormat, DI_OSD_HANDLE *pOsdHandle)
{
	DFBResult 				err;
	DFBSurfaceDescription	dsc;
	DFBMgr_t				*pDFBMgr = _di_osd_GetDFBMgr();
	DFB_Context_t			*pContext = NULL;

	/* HDFBInterface_t */
	HDFBInterface_t			*pInterface = &pDFBMgr->dif;

	PrintEnter();

	VK_memset(&dsc, 0, sizeof(DFBSurfaceDescription));

	pContext = VK_MEM_Alloc(sizeof(DFB_Context_t));
	DiLog_Assert(pContext);
	VK_memset(pContext, 0, sizeof(DFB_Context_t));

	dsc.width		= width;
	dsc.height		= height;
	dsc.flags		= DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT;

	switch(eColorFormat) {
	case DI_OSD_PIXEL_FORMAT_ARGB_8888:
		dsc.pixelformat = DSPF_ARGB;
		break;
	case DI_OSD_PIXEL_FORMAT_ARGB_4444:
		dsc.pixelformat = DSPF_ARGB4444;
		break;
	case DI_OSD_PIXEL_FORMAT_RGBA_4444:
		dsc.pixelformat = DSPF_RGBA4444;
		break;
	case DI_OSD_PIXEL_FORMAT_ARGB_1555:
		dsc.pixelformat = DSPF_ARGB1555;
		break;
	case DI_OSD_PIXEL_FORMAT_RGB_565:
		dsc.pixelformat = DSPF_RGB16;
		break;
	case DI_OSD_PIXEL_FORMAT_A_8:
		dsc.pixelformat = DSPF_A8;
		break;
	case DI_OSD_PIXEL_FORMAT_CLUT_8:
		dsc.pixelformat = DSPF_LUT8;
		break;
	case DI_OSD_PIXEL_FORMAT_YCBCR_444:
	case DI_OSD_PIXEL_FORMAT_YCBCR_422:
	case DI_OSD_PIXEL_FORMAT_RGBA_8888:
	default:
		PrintError("[%s] Not Supported(%d)!! default : ARGB8888\n", __FUNCTION__, eColorFormat);
		dsc.pixelformat = DSPF_ARGB;
		break;
	}

	err = pInterface->dfb->CreateSurface( pInterface->dfb, &dsc, &pContext->surface);
//	pContext->surface->Clear(pContext->surface, 0x0, 0x0, 0x0, 0x0);

	DI_OSD_Lock((DI_OSD_HANDLE)pContext);

	pContext->type		= eDI_BufferType_FrameBuffer;
	pContext->w			= width;
	pContext->h			= height;

	pContext->surface->SetDrawingFlags ( pContext->surface, DSDRAW_NOFX);
	pContext->surface->SetBlittingFlags( pContext->surface, DSBLIT_NOFX);

	*pOsdHandle = (DI_OSD_HANDLE)pContext;

	PrintExit();
	return DI_ERR_OK;
}



DI_ERR_CODE	DI_OSD_DeleteSurface(DI_OSD_HANDLE hOsdHandle)
{
	DFB_Context_t			*pContext= (DFB_Context_t *)hOsdHandle;

	PrintEnter();

	DiLog_Assert(pContext);

	if(pContext->bIsLock)
	{
		pContext->surface->Unlock(pContext->surface);
	}

	pContext->surface->Release(pContext->surface);

	VK_MEM_Free(pContext);

	PrintExit();
	return DI_ERR_OK;
}



DI_ERR_CODE	DI_OSD_DeleteFrameBuffer(DI_OSD_HANDLE hOsdHandle)
{
	DFB_Context_t			*pContext= (DFB_Context_t *)hOsdHandle;

	PrintEnter();

	DiLog_Assert(pContext);

	if(pContext->bIsLock)
	{
		pContext->surface->Unlock(pContext->surface);
	}

	pContext->surface->Release(pContext->surface);

	VK_MEM_Free(pContext);

	PrintExit();
	return DI_ERR_OK;
}



DI_ERR_CODE	DI_OSD_Show(DI_OSD_HANDLE hOsdHandle)
{
	DFB_Context_t			*pContext = (DFB_Context_t *)hOsdHandle;

	PrintEnter();

	PrintDebug("hOsdHandle = %x\n", hOsdHandle);

	DiLog_Assert(pContext);

	pContext->bIsShow = TRUE;

	PrintExit();
	return DI_ERR_OK;
}



DI_ERR_CODE	DI_OSD_Hide(DI_OSD_HANDLE hOsdHandle)
{
	DFB_Context_t			*pContext = (DFB_Context_t *)hOsdHandle;

	PrintEnter();

	PrintDebug("hOsdHandle = %x\n", hOsdHandle);

	DiLog_Assert(pContext);

	pContext->bIsShow = FALSE;

	PrintExit();
	return DI_ERR_OK;

}


/*
triple buffer : DSFLIP_ONSYNC
double buffer : DSFLIP_WAITFORSYNC
single buffer : DSFLIP_NONE
*/

DI_ERR_CODE	DI_OSD_Flush(DI_OSD_HANDLE hOsdHandle, DI_OSD_RECT *pRect)
{

	DFBMgr_t 				*pDFBMgr = _di_osd_GetDFBMgr();

	/* HDFBInterface_t */
	HDFBInterface_t 		*pInterface = &pDFBMgr->dif;
	DFB_Context_t			*pContext = NULL;
	int i;
	DI_OSD_RECT dstRect;

	PrintEnter();

	UNUSED(pRect);
	UNUSED(hOsdHandle);

	dstRect.x = dstRect.y = 0;
	dstRect.width = pDFBMgr->xres;
	dstRect.width = pDFBMgr->yres;

	DI_OSD_Unlock((DI_OSD_HANDLE)pInterface);

//	pInterface->primary->Clear(pInterface->primary, 0x0, 0x0, 0x0, 0x0);
//    pInterface->primary->SetDrawingFlags ( pInterface->primary, DSDRAW_BLEND);
//	pInterface->primary->SetBlittingFlags( pInterface->primary, DSBLIT_NOFX );

#if 0
	pInterface->primary->SetBlittingFlags( pInterface->primary, DSBLIT_BLEND_ALPHACHANNEL);

	pInterface->primary->SetSrcBlendFunction(pInterface->primary, DSBF_INVDESTALPHA);
	pInterface->primary->SetDstBlendFunction(pInterface->primary, DSBF_DESTALPHA);
#endif
	pInterface->primary->SetDrawingFlags(pInterface->primary, DSDRAW_NOFX);
	pInterface->primary->SetBlittingFlags( pInterface->primary, DSBLIT_BLEND_ALPHACHANNEL);

	/* first, we have to clear previous osd buffer for using alpha blending */
	pInterface->primary->Clear( pInterface->primary, 0, 0, 0, 0);

	for(i=DI_OSD_SURFACE_MAX_NUM-1; i>=0; i--)
	{
		pContext = di_layer[i];

		if(pContext)
		{
			if(pContext->bIsShow == TRUE)
			{
				PrintDebug("Blit [%d]\n", i);
				pInterface->primary->Blit( pInterface->primary, pContext->surface, NULL, 0, 0);
			}
		}
	}

	pInterface->primary->Flip( pInterface->primary, NULL, DSFLIP_WAITFORSYNC/*DSFLIP_NONE | DSFLIP_BLIT*/);//DSFLIP_ONSYNC | DSFLIP_NONE );

	DI_OSD_Lock((DI_OSD_HANDLE)pInterface);

	PrintExit();
	return DI_ERR_OK;
}



DI_ERR_CODE	DI_OSD_GetFrameBuffer(DI_OSD_HANDLE hOsdHandle, void **pFrameBuffer)
{
	DFB_Context_t	*pContext = (DFB_Context_t *)hOsdHandle;

	PrintEnter();

	if(pContext->bIsLock)
	{
		*pFrameBuffer = pContext->buffer;
		return DI_ERR_OK;
	}

	*pFrameBuffer = NULL;

	PrintExit();
	return DI_ERR_ERROR;
}


DI_ERR_CODE	DI_OSD_CreatePalette(DI_OSD_COLOR_FORMAT nColorFmt, DI_OSD_PALETTE **pPaletteHandle)
{
	PrintEnter();

	UNUSED(nColorFmt);
	UNUSED(pPaletteHandle);

	PrintExit();
	return DI_ERR_OK;
}

DI_ERR_CODE	DI_OSD_DeletePalette(DI_OSD_PALETTE *pPaletteHandle)
{
	PrintEnter();

	UNUSED(pPaletteHandle);

	PrintExit();
	return DI_ERR_OK;
}



DI_ERR_CODE	DI_OSD_SetPalette(DI_OSD_HANDLE hOsdHandle, DI_OSD_PALETTE *pPaletteHandle)
{
	PrintEnter();

	UNUSED(hOsdHandle);
	UNUSED(pPaletteHandle);

	PrintExit();
	return DI_ERR_OK;
}



DI_ERR_CODE	DI_OSD_ChangeColor(DI_OSD_HANDLE hOsdHandle, unsigned char index, unsigned long color)
{
	PrintEnter();

	UNUSED(hOsdHandle);
	UNUSED(index);
	UNUSED(color);

	PrintExit();
	return DI_ERR_OK;
}

DI_ERR_CODE	DI_OSD_CopyOSDBlock(DI_OSD_HANDLE hSrcHandle, DI_OSD_RECT srcRect, DI_OSD_HANDLE hDstHandle, DI_OSD_RECT dstRect)
{
	DFB_Context_t			*iSrc = (DFB_Context_t *)hSrcHandle;
	DFB_Context_t			*iDst = (DFB_Context_t *)hDstHandle;

	PrintEnter();

	DI_OSD_Unlock((DI_OSD_HANDLE)iSrc);
	DI_OSD_Unlock((DI_OSD_HANDLE)iDst);

    iDst->surface->SetDrawingFlags ( iDst->surface, DSDRAW_NOFX);
	iDst->surface->SetBlittingFlags( iDst->surface, DSBLIT_NOFX );
	iDst->surface->StretchBlit(iDst->surface, iSrc->surface, (DFBRectangle *)&srcRect, (DFBRectangle *)&dstRect);
	iDst->surface->Flip( iDst->surface, NULL, DSFLIP_NONE | DSFLIP_BLIT);

	DI_OSD_Lock((DI_OSD_HANDLE)iSrc);
	DI_OSD_Lock((DI_OSD_HANDLE)iDst);

	PrintExit();
	return DI_ERR_OK;
}



DI_ERR_CODE	DI_OSD_BlendOSDBlock(DI_OSD_HANDLE hSrcHandle, DI_OSD_RECT srcRect, DI_OSD_HANDLE hDstHandle, DI_OSD_RECT dstRect, DI_OSD_RASTER_OP rop)
{
	PrintEnter();
	return DI_OSD_FilterOSDBlock(hSrcHandle, srcRect, hDstHandle, dstRect, rop, 0, 0);
}



DI_ERR_CODE	DI_OSD_BlendBlock(DI_OSD_HANDLE hSrcHandle, DI_OSD_RECT srcRect, DI_OSD_HANDLE hDstHandle, DI_OSD_RECT dstRect, DI_OSD_RASTER_OP rop)
{
	PrintEnter();
	return DI_OSD_FilterOSDBlock(hSrcHandle, srcRect, hDstHandle, dstRect, rop, 0, 0);
}


DI_ERR_CODE	DI_OSD_FillOSDBlock(DI_OSD_HANDLE hOsdHandle, DI_OSD_RECT rect, unsigned long color)
{
	DFB_Context_t			*iDst = (DFB_Context_t *)hOsdHandle;

	PrintEnter();

	PrintDebug("hOsdHandle = %x\n", hOsdHandle);
	PrintDebug("[%x] rect(%d, %d, %d, %d)\n", color, rect.x, rect.y, rect.width, rect.height);

	DI_OSD_Unlock((DI_OSD_HANDLE)iDst);

//	iDst->surface->SetDrawingFlags ( iDst->surface, DSDRAW_NOFX);
//	iDst->surface->SetBlittingFlags( iDst->surface, DSBLIT_NOFX);
	iDst->surface->SetColor( iDst->surface, R(color), G(color), B(color), A(color) );
	iDst->surface->FillRectangle( iDst->surface, rect.x, rect.y, rect.width, rect.height);
	iDst->surface->Flip( iDst->surface, NULL, DSFLIP_NONE );

	DI_OSD_Lock((DI_OSD_HANDLE)iDst);

	PrintExit();
	return DI_ERR_OK;
}



DI_ERR_CODE	DI_OSD_BlendFillOSDBlock(DI_OSD_HANDLE hOsdHandle, DI_OSD_RECT rect, unsigned long color)
{
	PrintEnter();

	UNUSED(hOsdHandle);
	UNUSED(rect);
	UNUSED(color);

	PrintExit();
	return DI_ERR_OK;
}

DI_ERR_CODE	DI_OSD_SetAlphaOSDBlock(DI_OSD_HANDLE hSrcHandle, DI_OSD_RECT srcRect,
									DI_OSD_HANDLE hDstHandle, DI_OSD_RECT dstRect,
									DI_OSD_RASTER_OP rop, HUINT8 ucAlpha)
{
	PrintEnter();
	return DI_OSD_FilterOSDBlock(hSrcHandle, srcRect, hDstHandle, dstRect, rop, ucAlpha << 24, 0);
}



DI_ERR_CODE	DI_OSD_FilterOSDBlock(DI_OSD_HANDLE hSrcHandle, DI_OSD_RECT srcRect,
								  DI_OSD_HANDLE hDstHandle, DI_OSD_RECT dstRect,
								  DI_OSD_RASTER_OP rop, 	unsigned int ulConstantColor, int nFilter)
{
	DFB_Context_t			*iSrc = (DFB_Context_t *)hSrcHandle;
	DFB_Context_t			*iDst = (DFB_Context_t *)hDstHandle;

	UNUSED(nFilter);

	PrintEnter();

	PrintDebug("hSrcHandle = %x, hDstHandle = %x, rop = %x\n", hSrcHandle, hDstHandle, rop);
	PrintDebug("[%x] srcRect(%d, %d, %d, %d), dstRect(%d, %d, %d, %d)\n", rop, srcRect.x, srcRect.y, srcRect.width, srcRect.height,
		dstRect.x, dstRect.y, dstRect.width, dstRect.height);

	switch(rop)
	{
		case DI_OSD_RASTER_OP_SRCOVER:
			iDst->surface->SetDrawingFlags(iDst->surface, DSDRAW_NOFX);
			iDst->surface->SetBlittingFlags(iDst->surface, DSBLIT_BLEND_ALPHACHANNEL /*| DSBLIT_COLORIZE | DSBLIT_DST_PREMULTIPLY*/);
//			iDst->surface->SetSrcBlendFunction(iDst->surface, DSBF_ZERO);
			iDst->surface->SetDstBlendFunction(iDst->surface, DSBF_ONE);
//			iDst->surface->SetPorterDuff(iDst->surface, DSPD_DST_OVER);
			break;

		case DI_OSD_RASTER_OP_SRC:
			iDst->surface->SetDrawingFlags(iDst->surface, DSDRAW_NOFX);
			iDst->surface->SetBlittingFlags(iDst->surface, DSBLIT_NOFX);
			break;

		case DI_OSD_RASTER_OP_ALPHA_COMBINE_SRC:
			iDst->surface->SetColor(iDst->surface, A(ulConstantColor), A(ulConstantColor), A(ulConstantColor), A(ulConstantColor));
			iDst->surface->SetDrawingFlags(iDst->surface, DSDRAW_NOFX);
			iDst->surface->SetBlittingFlags(iDst->surface, DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR);
			break;

		default:
			PrintError("Unkown rop (%d)\n", rop);
			return DI_ERR_ERROR;
	}

	DI_OSD_Unlock((DI_OSD_HANDLE)iSrc);
	DI_OSD_Unlock((DI_OSD_HANDLE)iDst);

	iDst->surface->StretchBlit(iDst->surface, iSrc->surface, (DFBRectangle *)&srcRect, (DFBRectangle *)&dstRect);
	iDst->surface->Flip( iDst->surface, NULL, DSFLIP_NONE | DSFLIP_BLIT);

	DI_OSD_Lock((DI_OSD_HANDLE)iSrc);
	DI_OSD_Lock((DI_OSD_HANDLE)iDst);

	PrintExit();
	return DI_ERR_OK;
}



DI_ERR_CODE	DI_OSD_SetOutWindow(DI_OSD_LAYER nOsdLayer, DI_OSD_RECT *pRect)
{
	PrintEnter();

	UNUSED(nOsdLayer);
	UNUSED(pRect);

	PrintExit();
	return DI_ERR_OK;
}

DRV_Error DRV_OSD_GetDisplayHandle(HUINT32 ulDeviceId, NEXUS_DisplayHandle *dispHD, NEXUS_DisplayHandle *dispSD)
{
	DRV_Error err = DRV_OK;

#ifndef CONFIG_DIRECTFB
	err = DRV_VIDEO_GetDisplayHandle(ulDeviceId, dispHD, dispSD);
#else
	UNUSED(ulDeviceId);
	*dispHD = (NEXUS_DisplayHandle)DFB_Platform_P_GetDisplayHandle(0);
	*dispSD = (NEXUS_DisplayHandle)DFB_Platform_P_GetDisplayHandle(1);
#endif
	return err;
}

DRV_Error	DRV_OSD_Init( void )
{
	DI_ERR_CODE	dErr = DI_ERR_OK;

	dErr = VK_SEM_Create(&s_ulOsdSemId, "di_osd", DEFAULT_SUSPEND_TYPE);
	if( dErr != VK_OK )
	{
		PrintError("Error(0x%x) in VK_SEM_Create()\n\r", dErr);
		return DRV_ERR;
	}

	dErr = DI_OSD_Init(0, NULL);
	if(dErr != DI_ERR_OK)
	{
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_OSD_GetOsdHandle(DI_OSD_LAYER nOsdLayer, DI_OSD_HANDLE *pOsdHandle)
{
	PrintEnter();

	if (di_layer[nOsdLayer] && (di_layer[nOsdLayer]->surface))
	{
		*pOsdHandle = (DI_OSD_HANDLE)di_layer[nOsdLayer];
	}
	else
	{
		PrintError("Layer(%d) Created Surface is not existed.\n", nOsdLayer);
		*pOsdHandle = 0;
		return DRV_ERR;
	}

	PrintExit();
	return DRV_OK;
}


DRV_Error DRV_OSD_GetFB(int nFB_Num, DI_OSD_HANDLE *pOsdHandle)
{
	PrintEnter();

	UNUSED(nFB_Num);
	UNUSED(pOsdHandle);

	PrintExit();
	return DRV_OK;
}


DRV_Error DRV_OSD_Refresh(DI_OSD_LAYER nOsdLayer)
{
	DI_ERR_CODE			nRet = DI_ERR_OK;

	PrintEnter();

	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
	{
		nRet = DI_OSD_Flush((DI_OSD_HANDLE)di_layer[nOsdLayer], NULL);
	}
	else
	{
		PrintError("VK_SEM_Get() Error~!\n");
		return DRV_ERR;
	}

	VK_SEM_Release(s_ulOsdSemId);

	PrintExit();
	return DRV_OK;
}


DRV_Error DRV_OSD_SetFilterCoeffs(int lFilterSelect)
{
	PrintEnter();

	UNUSED(lFilterSelect);

	PrintExit();
	return DRV_OK;
}


void DRV_OSD_PrintInfo(void)
{
	/* OSD Plane Info */
	PrintStatus("\n\tOSD Infomation\n");

	return;
}


DRV_Error DRV_OSD_CopyImage(NEXUS_SurfaceHandle hSrcNexusSurfHandle, NEXUS_SurfaceHandle hDstNexusSurfHandle)
{
	PrintEnter();

	UNUSED(hSrcNexusSurfHandle);
	UNUSED(hDstNexusSurfHandle);

	PrintExit();
	return DRV_OK;
}

DRV_Error DRV_OSD_BlendVisibleSurface(NEXUS_SurfaceHandle hDstNexusSurfHandle)
{
	PrintEnter();

	UNUSED(hDstNexusSurfHandle);

	PrintExit();
	return DRV_OK;
}

DRV_Error DRV_OSD_AcquireSemaphore(void)
{
	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
		return DRV_OK;
	else
		return DRV_ERR;
}

DRV_Error DRV_OSD_ReleaseSemaphore(void)
{
	if(VK_SEM_Release(s_ulOsdSemId) == VK_OK)
		return DRV_OK;
	else
		return DRV_ERR;
}



/*********************************************************************
 *
 * Function 	DRV_OSD_SetDisplayFormat
 * Description	This function set display format to osd (graphic) when display format change.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DRV_Error DRV_OSD_SetDisplayFormatToHD(void)
{
#if 0
	NEXUS_Error				rc = NEXUS_SUCCESS;
    NEXUS_GraphicsSettings 	graphicsSettings;
#endif
	NEXUS_DisplaySettings 	displaySettings;
	DRV_Error				nRet = DRV_OK;
	DFBScreenEncoderConfig	encoderCfg;

	DFBResult				err;
	DFBMgr_t				*pDFBMgr = _di_osd_GetDFBMgr();

	/* HDFBInterface_t */
	HDFBInterface_t 		*pInterface = &pDFBMgr->dif;
	nRet = DRV_OSD_GetDisplayHandle(0, &HD_DispHandle, &SD_DispHandle);
	if (nRet != DRV_OK)
	{
		PrintError("DRV_OSD_GetDisplayHandle error!!\n");
		return DRV_ERR;
	}

	if(HD_DispHandle != NULL)
	{
		// HD Display
		NEXUS_Display_GetSettings(HD_DispHandle, &displaySettings);
		switch ( displaySettings.format )
		{
			case NEXUS_VideoFormat_e1080p24hz:
			case NEXUS_VideoFormat_e1080i50hz:
			case NEXUS_VideoFormat_e1080i:
			case NEXUS_VideoFormat_e1080p50hz:
			case NEXUS_VideoFormat_e1080p:
			case NEXUS_VideoFormat_e1080p30hz:
				HD_ClipRect.width = 1280;	// 1280
				HD_ClipRect.height = 720;
				break;
			case NEXUS_VideoFormat_e720p50hz:
			case NEXUS_VideoFormat_e720p:
				HD_ClipRect.width = 1280;	// 1280
				HD_ClipRect.height = 720;
				break;
			case NEXUS_VideoFormat_ePal:
			case NEXUS_VideoFormat_ePalI:
			case NEXUS_VideoFormat_e576p:
				HD_ClipRect.width = 720;	// 640
				HD_ClipRect.height = 576;
				break;
			case NEXUS_VideoFormat_eNtsc:
			case NEXUS_VideoFormat_eNtsc443:
			case NEXUS_VideoFormat_eNtscJapan:
			case NEXUS_VideoFormat_e480p:
				HD_ClipRect.width = 640;	// 640
				HD_ClipRect.height = 480;
				break;
	#if defined(CONFIG_HDOUT_2160P)
			case NEXUS_VideoFormat_e3840x2160p24hz:
			case NEXUS_VideoFormat_e3840x2160p25hz:
			case NEXUS_VideoFormat_e3840x2160p30hz:
			case NEXUS_VideoFormat_e3840x2160p50hz:
			case NEXUS_VideoFormat_e3840x2160p60hz:
				HD_ClipRect.width = 3840; // 3840
				HD_ClipRect.width = 2160;
				break;
	#endif

			default:
				PrintError("Not supported HD disp format(0x%x)\n", displaySettings.format);
				return DRV_ERR_INVALID_PARAMETER;
		}

		PrintDebug("HD Disp format(0x%x), ClipRect(w, h)=(%d,%d)\n",
				displaySettings.format, HD_ClipRect.width, HD_ClipRect.height);
#if 0
	/* graphicsSettings.position will default to the display size */

	/*
	display format 변경시 마다 HD_Surface 를 다시 만들지 말고,
	HD Surface 는 1080 size 를 감안하여, (1280, 1080) 로 고정하여 생성하고,
	clip 을 조정하여 format 변경에 대처하자.
	*/
	/* NEXUS_Rect clip;
		The area within the surface that should be shown, relative to 0,0.
        If you want to show the entire surface, cliprect should be 0,0,position.width,position.height.
        If you specify a cliprect which is smaller, only that portition will be shown
        and it will be scaled to fit the rectangle of size position.width and position.height.
        At this time only horizontal upscaling is supported. Vertical scaling and horizontal downscaling are not supported.
        The default value of clip.width is less than position.width for HD resolutions. Typically, HD graphics use horizontal upscaling to converse memory bandwidth.
	*/

	// HD
	NEXUS_Display_GetGraphicsSettings(HD_DispHandle, &graphicsSettings);

	graphicsSettings.clip.width = HD_ClipRect.width;
	graphicsSettings.clip.height = HD_ClipRect.height;

	graphicsSettings.enabled = true;

#if 1
    rc = NEXUS_Display_SetGraphicsSettings(HD_DispHandle, &graphicsSettings);
	AssertNEXUS(rc, DRV_ERR);
#if 0
	rc = NEXUS_Display_SetGraphicsFramebuffer(s_OsdList.HD_DispHandle, s_OsdList.HD_OsdInstance[MAIN_BUF].surface);
	AssertNEXUS(rc, DRV_ERR);
#endif
#endif
#endif
		err = pInterface->primary_screen->GetEncoderConfiguration(pInterface->primary_screen, 0, &encoderCfg);
		if (err) {
			DirectFBError( "DirectFBInit", err );
			return DRV_ERR;
		}

		encoderCfg.flags   = (DFBScreenEncoderConfigFlags)(DSECONF_TV_STANDARD | DSECONF_CONNECTORS |
					DSECONF_SCANMODE | DSECONF_FREQUENCY | DSECONF_RESOLUTION );
		encoderCfg.tv_standard = DSETV_DIGITAL;
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1
		encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_HDMI | DSOC_COMPONENT);
#else
		encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_HDMI);
#endif
		switch ( displaySettings.format )
		{
#if defined(CONFIG_HDOUT_2160P)
			case NEXUS_VideoFormat_e3840x2160p24hz:
				encoderCfg.frequency = DSEF_24HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_3840_2160;
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1
				encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_HDMI);
#endif
				break;
			case NEXUS_VideoFormat_e3840x2160p25hz:
				encoderCfg.frequency = DSEF_25HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_3840_2160;
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1
				encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_HDMI);
#endif
				break;
			case NEXUS_VideoFormat_e3840x2160p30hz:
				encoderCfg.frequency = DSEF_30HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_3840_2160;
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1
				encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_HDMI);
#endif
				break;
			case NEXUS_VideoFormat_e3840x2160p50hz:
				encoderCfg.frequency = DSEF_50HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_3840_2160;
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1
				encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_HDMI);
#endif
				break;
			case NEXUS_VideoFormat_e3840x2160p60hz:
				encoderCfg.frequency = DSEF_60HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_3840_2160;
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1
				encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_HDMI);
#endif
				break;
#endif
			case NEXUS_VideoFormat_e1080p24hz:
				encoderCfg.frequency = DSEF_24HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_1920_1080;
				break;
			case NEXUS_VideoFormat_e1080p30hz:
				encoderCfg.frequency = DSEF_30HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_1920_1080;
				break;
			case NEXUS_VideoFormat_e1080i50hz:
				encoderCfg.frequency = DSEF_50HZ;
				encoderCfg.scanmode = DSESM_INTERLACED;
				encoderCfg.resolution = DSOR_1920_1080;
				break;
			case NEXUS_VideoFormat_e1080i:
				encoderCfg.frequency = DSEF_60HZ;
				encoderCfg.scanmode = DSESM_INTERLACED;
				encoderCfg.resolution = DSOR_1920_1080;
				break;
			case NEXUS_VideoFormat_e1080p50hz:
				encoderCfg.frequency = DSEF_50HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_1920_1080;
				break;
			case NEXUS_VideoFormat_e1080p:
				encoderCfg.frequency = DSEF_60HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_1920_1080;
				break;
			case NEXUS_VideoFormat_e720p50hz:
				encoderCfg.frequency = DSEF_50HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_1280_720;
				break;
			case NEXUS_VideoFormat_e720p:
				encoderCfg.frequency = DSEF_60HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_1280_720;
				break;
			case NEXUS_VideoFormat_ePal:
			case NEXUS_VideoFormat_ePalI:
				encoderCfg.frequency = DSEF_50HZ;
				encoderCfg.scanmode = DSESM_INTERLACED;
				encoderCfg.resolution = DSOR_720_576;
				encoderCfg.flags   = (DFBScreenEncoderConfigFlags)(DSECONF_TV_STANDARD | DSECONF_CONNECTORS);
				encoderCfg.tv_standard = DSETV_PAL;
				break;
			case NEXUS_VideoFormat_e576p:
				encoderCfg.frequency = DSEF_50HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_720_576;
				break;
			case NEXUS_VideoFormat_eNtsc:
			case NEXUS_VideoFormat_eNtsc443:
			case NEXUS_VideoFormat_eNtscJapan:
				encoderCfg.frequency = DSEF_60HZ;
				encoderCfg.scanmode = DSESM_INTERLACED;
				encoderCfg.resolution = DSOR_640_480;
				encoderCfg.flags   = (DFBScreenEncoderConfigFlags)(DSECONF_TV_STANDARD | DSECONF_CONNECTORS);
				encoderCfg.tv_standard = DSETV_NTSC;
				break;
			case NEXUS_VideoFormat_e480p:
				encoderCfg.frequency = DSEF_60HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_640_480;
				break;
			default:
				PrintError("Not supported HD disp format(0x%x)\n", displaySettings.format);
				return DRV_ERR_INVALID_PARAMETER;
		}

		err = pInterface->primary_screen->SetEncoderConfiguration(pInterface->primary_screen, 0, &encoderCfg);
		if (err) {
			DirectFBError( "DirectFBInit", err );
			return DRV_ERR;
		}

		PrintDebug("encoderCfg %x, %x, %x, %x, %x, %x\n", encoderCfg.flags, encoderCfg.tv_standard, encoderCfg.out_connectors,
				encoderCfg.frequency, encoderCfg.scanmode, encoderCfg.resolution);

	}
	return DRV_OK;
}

DRV_Error DRV_OSD_SetDisplayFormatToHDForDFB(NEXUS_VideoFormat format)
{
	DFBScreenEncoderConfig	encoderCfg;

	DFBResult				err;
	DFBMgr_t				*pDFBMgr = _di_osd_GetDFBMgr();

	/* HDFBInterface_t */
	HDFBInterface_t 		*pInterface = &pDFBMgr->dif;

	err = pInterface->primary_screen->GetEncoderConfiguration(pInterface->primary_screen, 0, &encoderCfg);
	if (err) {
		DirectFBError( "DirectFBInit", err );
		return DRV_ERR;
	}

	encoderCfg.flags   = (DFBScreenEncoderConfigFlags)(DSECONF_TV_STANDARD | DSECONF_CONNECTORS |
				DSECONF_SCANMODE | DSECONF_FREQUENCY | DSECONF_RESOLUTION );
	encoderCfg.tv_standard = DSETV_DIGITAL;
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1
	encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_HDMI | DSOC_COMPONENT);
#else
	encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_HDMI);
#endif
	switch ( format )
	{
#if defined(CONFIG_HDOUT_2160P)
		case NEXUS_VideoFormat_e3840x2160p24hz:
			encoderCfg.frequency = DSEF_24HZ;
			encoderCfg.scanmode = DSESM_PROGRESSIVE;
			encoderCfg.resolution = DSOR_3840_2160;
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1
			encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_HDMI);
#endif
			break;
		case NEXUS_VideoFormat_e3840x2160p25hz:
			encoderCfg.frequency = DSEF_25HZ;
			encoderCfg.scanmode = DSESM_PROGRESSIVE;
			encoderCfg.resolution = DSOR_3840_2160;
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1
			encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_HDMI);
#endif
			break;
		case NEXUS_VideoFormat_e3840x2160p30hz:
			encoderCfg.frequency = DSEF_30HZ;
			encoderCfg.scanmode = DSESM_PROGRESSIVE;
			encoderCfg.resolution = DSOR_3840_2160;
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1
			encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_HDMI);
#endif
			break;
		case NEXUS_VideoFormat_e3840x2160p50hz:
			encoderCfg.frequency = DSEF_50HZ;
			encoderCfg.scanmode = DSESM_PROGRESSIVE;
			encoderCfg.resolution = DSOR_3840_2160;
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1
			encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_HDMI);
#endif
			break;
		case NEXUS_VideoFormat_e3840x2160p60hz:
			encoderCfg.frequency = DSEF_60HZ;
			encoderCfg.scanmode = DSESM_PROGRESSIVE;
			encoderCfg.resolution = DSOR_3840_2160;
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1
			encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_HDMI);
#endif
			break;
#endif
#if defined(CONFIG_HDOUT_1080P_24HZ)
		case NEXUS_VideoFormat_e1080p24hz:
			encoderCfg.frequency = DSEF_24HZ;
			encoderCfg.scanmode = DSESM_PROGRESSIVE;
			encoderCfg.resolution = DSOR_1920_1080;
			break;
#endif
		case NEXUS_VideoFormat_e1080p30hz:
			encoderCfg.frequency = DSEF_30HZ;
			encoderCfg.scanmode = DSESM_PROGRESSIVE;
			encoderCfg.resolution = DSOR_1920_1080;
			break;
		case NEXUS_VideoFormat_e1080i50hz:
			encoderCfg.frequency = DSEF_50HZ;
			encoderCfg.scanmode = DSESM_INTERLACED;
			encoderCfg.resolution = DSOR_1920_1080;
			break;
		case NEXUS_VideoFormat_e1080i:
			encoderCfg.frequency = DSEF_60HZ;
			encoderCfg.scanmode = DSESM_INTERLACED;
			encoderCfg.resolution = DSOR_1920_1080;
			break;
		case NEXUS_VideoFormat_e1080p50hz:
			encoderCfg.frequency = DSEF_50HZ;
			encoderCfg.scanmode = DSESM_PROGRESSIVE;
			encoderCfg.resolution = DSOR_1920_1080;
			break;
		case NEXUS_VideoFormat_e1080p:
			encoderCfg.frequency = DSEF_60HZ;
			encoderCfg.scanmode = DSESM_PROGRESSIVE;
			encoderCfg.resolution = DSOR_1920_1080;
			break;
		case NEXUS_VideoFormat_e720p50hz:
			encoderCfg.frequency = DSEF_50HZ;
			encoderCfg.scanmode = DSESM_PROGRESSIVE;
			encoderCfg.resolution = DSOR_1280_720;
			break;
		case NEXUS_VideoFormat_e720p:
			encoderCfg.frequency = DSEF_60HZ;
			encoderCfg.scanmode = DSESM_PROGRESSIVE;
			encoderCfg.resolution = DSOR_1280_720;
			break;
		case NEXUS_VideoFormat_ePal:
		case NEXUS_VideoFormat_ePalI:
			encoderCfg.frequency = DSEF_50HZ;
			encoderCfg.scanmode = DSESM_INTERLACED;
			encoderCfg.resolution = DSOR_720_576;
			encoderCfg.flags   = (DFBScreenEncoderConfigFlags)(DSECONF_TV_STANDARD | DSECONF_CONNECTORS | DSECONF_ASPECT_RATIO);
			encoderCfg.tv_standard = DSETV_PAL;
			encoderCfg.aspect_ratio = DFB_ASPECT_RATIO_e16x9;
			break;
		case NEXUS_VideoFormat_e576p:
			encoderCfg.frequency = DSEF_50HZ;
			encoderCfg.scanmode = DSESM_PROGRESSIVE;
			encoderCfg.resolution = DSOR_720_576;
			break;
		case NEXUS_VideoFormat_eNtsc:
		case NEXUS_VideoFormat_eNtsc443:
		case NEXUS_VideoFormat_eNtscJapan:
			encoderCfg.frequency = DSEF_60HZ;
			encoderCfg.scanmode = DSESM_INTERLACED;
			encoderCfg.resolution = DSOR_720_480;
			encoderCfg.flags   = (DFBScreenEncoderConfigFlags)(DSECONF_TV_STANDARD | DSECONF_CONNECTORS);
			encoderCfg.tv_standard = DSETV_NTSC;
			break;
		case NEXUS_VideoFormat_e480p:
			encoderCfg.frequency = DSEF_60HZ;
			encoderCfg.scanmode = DSESM_PROGRESSIVE;
			encoderCfg.resolution = DSOR_720_480;
			break;
		default:
						VK_printf("#################[%s]_%d \n",__func__,__LINE__);
			PrintError("Not supported HD disp format(0x%x)\n", format);
			return DRV_ERR_INVALID_PARAMETER;
	}
	VK_printf("#################[%s]_%d \n",__func__,__LINE__);

	err = pInterface->primary_screen->SetEncoderConfiguration(pInterface->primary_screen, 0, &encoderCfg);
	if (err) {
		DirectFBError( "DirectFBInit", err );
		return DRV_ERR;
	}
	VK_printf("#################[%s]_%d \n",__func__,__LINE__);

	PrintDebug("encoderCfg %x, %x, %x, %x, %x, %x\n", encoderCfg.flags, encoderCfg.tv_standard, encoderCfg.out_connectors,
			encoderCfg.frequency, encoderCfg.scanmode, encoderCfg.resolution);

	return DRV_OK;
}

/*********************************************************************
 *
 * Function 	DRV_OSD_ResetDisplayFormat
 * Description	This function reset osd (graphic) clip size when display format change.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DRV_Error DRV_OSD_ResetDisplayFormatToHD(void)
{
#if 0
	NEXUS_Error				rc = NEXUS_SUCCESS;
    NEXUS_GraphicsSettings 	graphicsSettings;

	/* graphicsSettings.position will default to the display size */

	/*
	display format 변경에 대응하기위해 (display  format 변경시 clip 이 display size 보다 크면 error),
	display format 변경 전에 graphic engine disable 하자.
	display format 변경 후에 변경된 display format 으로 clip 재 조정하자. (call by DRV_OSD_SetDisplayFormat()
	*/
	/* NEXUS_Rect clip;
		The area within the surface that should be shown, relative to 0,0.
        If you want to show the entire surface, cliprect should be 0,0,position.width,position.height.
        If you specify a cliprect which is smaller, only that portition will be shown
        and it will be scaled to fit the rectangle of size position.width and position.height.
        At this time only horizontal upscaling is supported. Vertical scaling and horizontal downscaling are not supported.
        The default value of clip.width is less than position.width for HD resolutions. Typically, HD graphics use horizontal upscaling to converse memory bandwidth.
	*/

	// For HD Display
	NEXUS_Display_GetGraphicsSettings(HD_DispHandle, &graphicsSettings);

	graphicsSettings.enabled = false;

    rc = NEXUS_Display_SetGraphicsSettings(HD_DispHandle, &graphicsSettings);
	AssertNEXUS(rc, DRV_ERR);
#endif
	return DRV_OK;
}



DRV_Error DRV_OSD_GetGraphics2DHandle(NEXUS_Graphics2DHandle* pGraphics2DHandle)
{
	PrintEnter();

	UNUSED(pGraphics2DHandle);

	PrintExit();
	return DRV_OK;
}

void DRV_OSD_Sync(void)
{

}

