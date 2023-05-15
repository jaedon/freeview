/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_execute.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 1/17/12 3:39p $
 *
 * Module Description: Execute entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_execute.c $
 * 
 * Hydra_Software_Devel/10   1/17/12 3:39p nissen
 * SW7435-30: Added support for using planes instead of BSUR surfaces.
 * 
 * Hydra_Software_Devel/9   8/12/09 1:53p nissen
 * PR 56587: Using ReadSurface as a background source.
 * 
 * Hydra_Software_Devel/8   9/8/08 9:02p nissen
 * PR 46638: Added support for waiting for blits.
 * 
 * Hydra_Software_Devel/7   11/15/07 8:08p nissen
 * PR 35855: Added support for new native model.
 * 
 * Hydra_Software_Devel/6   9/10/07 11:53a nissen
 * PR 30062: Fixed flush and finish functions.
 * 
 * Hydra_Software_Devel/5   5/7/07 1:03p nissen
 * PR 30062: Chaned glFlush to initiate render without flushing it.
 * 
 * Hydra_Software_Devel/4   9/17/04 3:10p nissen
 * PR 9132: Changed parameters for calling initiate render function.
 * 
 * Hydra_Software_Devel/3   9/7/04 3:43p nissen
 * PR 9132: Added buffer swapping support.
 * 
 * Hydra_Software_Devel/2   7/28/04 1:14p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "native_private.h"

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glFinish( void )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

#if 0
	BP3D_SetDstColorSurface( pDisplay->hP3d, pDrawSurface->hRenderSurface );
	BP3D_SetSrcColorSurface( pDisplay->hP3d, pReadSurface->hRenderSurface );
#else
	BP3D_SetDstColorPlane( pDisplay->hP3d, pDisplay->pDrawSurface->uiFormat, pDisplay->pDrawSurface->uiOffset, pDisplay->pDrawSurface->iPitch );
	BP3D_SetSrcColorPlane( pDisplay->hP3d, pDisplay->pReadSurface->uiFormat, pDisplay->pReadSurface->uiOffset, pDisplay->pReadSurface->iPitch );
#endif

	EGL_P_InitiateRender( pDisplay, pContext, GL_TRUE );
	native_P_BlitWait( pContext, pDisplay->nativeDisplay, pDisplay->pDrawSurface->hRenderSurface );
}

/***************************************************************************/
GLAPI void APIENTRY glFlush( void )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

#if 0
	BP3D_SetDstColorSurface( pDisplay->hP3d, pDrawSurface->hRenderSurface );
	BP3D_SetSrcColorSurface( pDisplay->hP3d, pReadSurface->hRenderSurface );
#else
	BP3D_SetDstColorPlane( pDisplay->hP3d, pDisplay->pDrawSurface->uiFormat, pDisplay->pDrawSurface->uiOffset, pDisplay->pDrawSurface->iPitch );
	BP3D_SetSrcColorPlane( pDisplay->hP3d, pDisplay->pReadSurface->uiFormat, pDisplay->pReadSurface->uiOffset, pDisplay->pReadSurface->iPitch );
#endif

	EGL_P_InitiateRender( pDisplay, pContext, GL_FALSE );
	native_P_BlitWait( pContext, pDisplay->nativeDisplay, pDisplay->pDrawSurface->hRenderSurface );
}

/* End of File */
