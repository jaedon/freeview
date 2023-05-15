/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: native_nexus.c $
 * $brcm_Revision: Hydra_Software_Devel/13 $
 * $brcm_Date: 5/11/12 2:07p $
 *
 * Module Description: Native entry point functions for Nexus.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/native_nexus.c $
 * 
 * Hydra_Software_Devel/13   5/11/12 2:07p nissen
 * SW7125-1283: Fixed P3D memory leak when closing.
 * 
 * Hydra_Software_Devel/12   1/17/12 4:01p nissen
 * SW7435-30: Added support for using planes instead of BSUR surfaces.
 * 
 * Hydra_Software_Devel/11   5/27/11 3:12p nissen
 * SW7346-149: Fixed call to GRClib.
 * 
 * Hydra_Software_Devel/10   8/27/10 8:49p nissen
 * SW7400-1936: Fixed compile warning and switched copy blit from Nexus to
 * GRClib.
 * 
 * Hydra_Software_Devel/9   4/23/10 4:51p nissen
 * SW7405-3183: Fixed problem with reopening 3D module.
 * 
 * Hydra_Software_Devel/8   7/8/09 10:39a jtna
 * PR55968: 3d graphics power management
 * 
 * Hydra_Software_Devel/7   1/27/09 3:45p mphillip
 * PR51468: Fix graphics2d deref to match the new global namespace
 * 
 * Hydra_Software_Devel/6   11/3/08 3:00p mphillip
 * PR47441: Shuffle nexus headers slightly for kernel-mode to build
 * correctly
 * 
 * Hydra_Software_Devel/5   10/23/08 2:17p nissen
 * PR 46638: Fixed warnings.
 * 
 * Hydra_Software_Devel/4   9/25/08 12:42p nissen
 * PR 46638: Added stub blit functions to build.
 * 
 * Hydra_Software_Devel/3   9/8/08 3:29p nissen
 * PR 46638: Added blit function to support 2D rectangle polygon blits.
 * 
 * Hydra_Software_Devel/2   8/28/08 12:32p nissen
 * PR 41229: Switch from GRC blits to Nexus.
 * 
 * Hydra_Software_Devel/1   7/15/08 3:02p nissen
 * PR 41229: Native files for Nexus.
 * 
 ***************************************************************************/

#include "bgrclib_packet.h"
#include "native_private.h"
#include "native_nexus.h"

#include "priv/nexus_core.h"
#include "nexus_graphics2d_impl.h"

/***************************************************************************/
BPXL_Format native_P_ConvertFormat_M2MCtoPXL( uint32_t format )
{
	switch( format )
	{
	case BM2MC_PACKET_PixelFormat_eA8_R8_G8_B8:     return BPXL_eA8_R8_G8_B8;
	case BM2MC_PACKET_PixelFormat_eR5_G6_B5:        return BPXL_eR5_G6_B5;
	case BM2MC_PACKET_PixelFormat_eA1_R5_G5_B5:     return BPXL_eA1_R5_G5_B5;
	case BM2MC_PACKET_PixelFormat_eA4_R4_G4_B4:     return BPXL_eA4_R4_G4_B4;
	case BM2MC_PACKET_PixelFormat_eY18_Cr8_Y08_Cb8: return BPXL_eY18_Cr8_Y08_Cb8;
	case BM2MC_PACKET_PixelFormat_eY08_Cb8_Y18_Cr8: return BPXL_eY08_Cb8_Y18_Cr8;
	case BM2MC_PACKET_PixelFormat_eL8:              return BPXL_eL8;
	case BM2MC_PACKET_PixelFormat_eA8:              return BPXL_eA8;
	case BM2MC_PACKET_PixelFormat_eL8_A8:           return BPXL_eL8_A8;
	default:                                        return BPXL_eZ16;
	}
}

/***************************************************************************/
static NEXUS_Graphics3DHandle gfx3d = 0;

/***************************************************************************/
/* PUBLIC */
/***************************************************************************/
void native_P_InitDisplay( 
	NativeDisplay display,
	BMEM_Handle *phMem,
	BP3D_Handle *phP3d )
{
	BSTD_UNUSED( display );

	if( *phP3d == 0 )
	{
		gfx3d = NEXUS_Graphics3D_Open(0, NULL);
		*phP3d = NEXUS_Graphics3d_GetP3d_priv();
	}
	*phMem = g_pCoreHandles->heap[0];
}

/***************************************************************************/
void native_P_TermDisplay( 
	NativeDisplay display )
{
	BSTD_UNUSED( display );
	NEXUS_Graphics3D_Close(gfx3d);
}

/***************************************************************************/
void native_P_InitWindow( 
	NativeWindow window,
	BSUR_Surface_Handle *phSurface,
	unsigned int *puiFormat,
	unsigned int *puiOffset,
	void **ppvMemory,
	int *piPitch,
	int *piWidth,
	int *piHeight )
{
	NEXUS_SurfaceHandle sur = (NEXUS_SurfaceHandle) window;

#if 0
	NEXUS_SurfaceCreateSettings settings;
	NEXUS_Module_Lock( g_NEXUS_graphics2DData.settings.surface );
	*phSurface = NEXUS_Surface_GetSurface_priv( sur );
	NEXUS_Module_Unlock( g_NEXUS_graphics2DData.settings.surface );

	NEXUS_Surface_GetCreateSettings( sur, &settings );
	*piWidth = settings.width;
	*piHeight = settings.height;
#else
	BM2MC_PACKET_Plane plane;
	NEXUS_Surface_InitPlane( sur, &plane );
	*phSurface = NULL;
	*puiFormat = native_P_ConvertFormat_M2MCtoPXL(plane.format);
	*puiOffset = plane.address;
	*piPitch = plane.pitch;
	*piWidth = plane.width;
	*piHeight = plane.height;

	if( ppvMemory )
	{
		NEXUS_SurfaceCreateSettings settings;
		NEXUS_Surface_GetCreateSettings( sur, &settings );
		*ppvMemory = settings.pMemory;
	}
#endif
}

/***************************************************************************/
void native_P_InitPixmap( 
	NativePixmap pixmap,
	BSUR_Surface_Handle *phSurface,
	unsigned int *puiFormat,
	unsigned int *puiOffset,
	void **ppvMemory,
	int *piPitch,
	int *piWidth,
	int *piHeight )
{
	NEXUS_SurfaceHandle sur = (NEXUS_SurfaceHandle) pixmap;
#if 0
	NEXUS_SurfaceCreateSettings settings;
	NEXUS_Module_Lock( g_NEXUS_graphics2DData.settings.surface );
	*phSurface = NEXUS_Surface_GetSurface_priv( sur );
	NEXUS_Module_Unlock( g_NEXUS_graphics2DData.settings.surface );

	NEXUS_Surface_GetCreateSettings( sur, &settings );
	*piWidth = settings.width;
	*piHeight = settings.height;
#else
	BM2MC_PACKET_Plane plane;
	NEXUS_Surface_InitPlane( sur, &plane );
	*phSurface = NULL;
	*puiFormat = native_P_ConvertFormat_M2MCtoPXL(plane.format);
	*puiOffset = plane.address;
	*piPitch = plane.pitch;
	*piWidth = plane.width;
	*piHeight = plane.height;

	if( ppvMemory )
	{
		NEXUS_SurfaceCreateSettings settings;
		NEXUS_Surface_GetCreateSettings( sur, &settings );
		*ppvMemory = settings.pMemory;
	}
#endif
}

/***************************************************************************/
void native_P_MakePlane( 
	BM2MC_PACKET_Plane *pPlane,
	BSUR_Surface_Handle hSurface
)
{
	BPXL_Format format;
	void *address;
	uint32_t pitch, width, height;
	BSUR_Surface_GetFormat( hSurface, &format );

	switch( format )
	{
	case BPXL_eA8_R8_G8_B8:     pPlane->format = BM2MC_PACKET_PixelFormat_eA8_R8_G8_B8; break;
	case BPXL_eR5_G6_B5:        pPlane->format = BM2MC_PACKET_PixelFormat_eR5_G6_B5; break;
	case BPXL_eA1_R5_G5_B5:     pPlane->format = BM2MC_PACKET_PixelFormat_eA1_R5_G5_B5; break;
	case BPXL_eA4_R4_G4_B4:     pPlane->format = BM2MC_PACKET_PixelFormat_eA4_R4_G4_B4; break;
	case BPXL_eY18_Cr8_Y08_Cb8: pPlane->format = BM2MC_PACKET_PixelFormat_eY18_Cr8_Y08_Cb8; break;
	case BPXL_eY08_Cb8_Y18_Cr8: pPlane->format = BM2MC_PACKET_PixelFormat_eY08_Cb8_Y18_Cr8; break;
	case BPXL_eL8:              pPlane->format = BM2MC_PACKET_PixelFormat_eL8; break;
	case BPXL_eA8:              pPlane->format = BM2MC_PACKET_PixelFormat_eA8; break;
	case BPXL_eL8_A8:           pPlane->format = BM2MC_PACKET_PixelFormat_eL8_A8; break;
	default:                    pPlane->format = BM2MC_PACKET_PixelFormat_eA8_R8_G8_B8;
	}

	BSUR_Surface_GetAddress( hSurface, &address, &pitch );
	BSUR_Surface_GetOffset( hSurface, &pPlane->address );
	BSUR_Surface_GetDimensions( hSurface, &width, &height );

	pPlane->pitch = (uint16_t) pitch;
	pPlane->width = (uint16_t) width;
	pPlane->height = (uint16_t) height;
}

/***************************************************************************/
void native_P_CopySurface( 
	NativeDisplay display,
	BSUR_Surface_Handle hDstSurface,
	BSUR_Surface_Handle hSrcSurface )
{
	NEXUS_Graphics2DHandle gfx = (NEXUS_Graphics2DHandle) display;

	if( gfx && hDstSurface && hSrcSurface )
	{
		BM2MC_PACKET_Plane srcSurface;
		BM2MC_PACKET_Plane outSurface;
		BRect dstRect = { 0, 0, 0, 0 };
		BRect srcRect = { 0, 0, 0, 0 };

		BGRClib_BlitParams blitParams;
		BGRClib_GetDefaultBlitParams( &blitParams );

		native_P_MakePlane( &srcSurface, hSrcSurface );
		native_P_MakePlane( &outSurface, hDstSurface );

		blitParams.srcSurface = &srcSurface;
		blitParams.outSurface = &outSurface;
		blitParams.srcRect = &srcRect;
		blitParams.outRect = &dstRect;
		blitParams.colorOp = BGRCLib_BlitColorOp_eCopySource;
		blitParams.alphaOp = BGRCLib_BlitAlphaOp_eCopySource;
		blitParams.constantColor = 0;
		blitParams.colorKeySelect = BGRC_Output_ColorKeySelection_eTakeSource;

		BGRClib_Blit( gfx->grclib, &blitParams, NULL, NULL, NULL, NULL );
	}
}

/***************************************************************************/
void native_P_BlitSurface( 
	EGL_P_Context *pContext,
	NativeDisplay display,
	BSUR_Surface_Handle hDstSurface,
	int iDstX, int iDstY, int iDstWidth, int iDstHeight,
	BSUR_Surface_Handle hSrcSurface,
	int iSrcX, int iSrcY, int iSrcWidth, int iSrcHeight,
	float fPhaseX, float fPhaseY, 
	float fScaleX, float fScaleY, 
	uint32_t ulColor,
	GLboolean bSame )
{
	BSTD_UNUSED( pContext );
	BSTD_UNUSED( display );
	BSTD_UNUSED( hDstSurface );
	BSTD_UNUSED( iDstX );
	BSTD_UNUSED( iDstY );
	BSTD_UNUSED( iDstWidth );
	BSTD_UNUSED( iDstHeight );
	BSTD_UNUSED( hSrcSurface );
	BSTD_UNUSED( iSrcX );
	BSTD_UNUSED( iSrcY );
	BSTD_UNUSED( iSrcWidth );
	BSTD_UNUSED( iSrcHeight );
	BSTD_UNUSED( fPhaseX );
	BSTD_UNUSED( fPhaseY );
	BSTD_UNUSED( fScaleX );
	BSTD_UNUSED( fScaleY );
	BSTD_UNUSED( ulColor );
	BSTD_UNUSED( bSame );
}

/***************************************************************************/
void native_P_BlitWait( 
	EGL_P_Context *pContext,
	NativeDisplay display,
	BSUR_Surface_Handle hSurface )
{
	BSTD_UNUSED( pContext );
	BSTD_UNUSED( display );
	BSTD_UNUSED( hSurface );
}

/* End of File */
