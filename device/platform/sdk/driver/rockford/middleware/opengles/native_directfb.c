/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: native_directfb.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 5/11/12 1:36p $
 *
 * Module Description: Native entry point functions for DirectFB.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/native_directfb.c $
 * 
 * Hydra_Software_Devel/6   5/11/12 1:36p nissen
 * SW7125-1283: Fixed P3D memory leak when closing.
 * 
 * Hydra_Software_Devel/5   1/17/12 3:51p nissen
 * SW7435-30: Added support for using planes instead of BSUR surfaces.
 * 
 * Hydra_Software_Devel/4   4/23/10 4:47p nissen
 * SW7405-3183: Fixed problem with reopening 3D module.
 * 
 * Hydra_Software_Devel/3   10/21/09 8:24p nissen
 * SW7405-3183: Switched to DirectFB method of getting surface size.
 * 
 * Hydra_Software_Devel/2   10/14/09 5:51p nissen
 * SW7405-3183: Fixed opening graphics.
 * 
 * Hydra_Software_Devel/1   10/9/09 4:22p nissen
 * SW7405-3183: DirectFB version.
 * 
 ***************************************************************************/

#include "native_private.h"
#include "native_nexus.h"

#include "priv/nexus_core.h"
#include "nexus_graphics2d_impl.h"

#define inline

#include <display/idirectfbsurface.h>
#include <interface.h>

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
	IDirectFBSurface *thiz = (IDirectFBSurface *) window;
	NEXUS_SurfaceHandle sur;
	void *pMemory;
	int iPitch;

	DIRECT_INTERFACE_GET_DATA(IDirectFBSurface);

	thiz->Lock( thiz, DSLF_READ, &pMemory, &iPitch );
	sur = (NEXUS_SurfaceHandle) data->lock.handle;
	thiz->Unlock( thiz );
#if 0
	thiz->GetSize( thiz, piWidth, piHeight );

	NEXUS_Module_Lock( g_NEXUS_graphics2DData.settings.surface );
	*phSurface = NEXUS_Surface_GetSurface_priv( sur );
	NEXUS_Module_Unlock( g_NEXUS_graphics2DData.settings.surface );
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
	IDirectFBSurface *thiz = (IDirectFBSurface *) pixmap;
	NEXUS_SurfaceHandle sur;
	void *pMemory;
	int iPitch;

	DIRECT_INTERFACE_GET_DATA(IDirectFBSurface);

	thiz->Lock( thiz, DSLF_READ, &pMemory, &iPitch );
	sur = (NEXUS_SurfaceHandle) data->lock.handle;
	thiz->Unlock( thiz );
#if 0
	thiz->GetSize( thiz, piWidth, piHeight );

	NEXUS_Module_Lock( g_NEXUS_graphics2DData.settings.surface );
	*phSurface = NEXUS_Surface_GetSurface_priv( sur );
	NEXUS_Module_Unlock( g_NEXUS_graphics2DData.settings.surface );
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
void native_P_CopySurface( 
	NativeDisplay display,
	BSUR_Surface_Handle hDstSurface,
	BSUR_Surface_Handle hSrcSurface )
{
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
