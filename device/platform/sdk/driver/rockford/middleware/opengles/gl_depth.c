/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_depth.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 8/14/10 2:23p $
 *
 * Module Description: Depth entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_depth.c $
 * 
 * Hydra_Software_Devel/3   8/14/10 2:23p nissen
 * SW7400-2378: Fixed problems with bad input.
 * 
 * Hydra_Software_Devel/2   7/28/04 1:02p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glDepthFunc( 
	GLenum func )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

	BP3D_Compare eCompare = GL_P_ConvertCompare_GLtoP3D( func );
	if( eCompare == (BP3D_Compare) -1 )
		return;

	BP3D_SetDepthCompare( pDisplay->hP3d, pContext->bDepthTest ? eCompare : BP3D_Compare_Always );
	pContext->eDepthFunc = func;
}

/***************************************************************************/
GLAPI void APIENTRY glDepthMask( 
	GLboolean flag )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

	BP3D_SetDepthMask( pDisplay->hP3d, pContext->bDepthTest ? (bool) (!flag) : true );
	pContext->bDepthMask = flag;
}

/***************************************************************************/
GLAPI void APIENTRY glDepthRangef( 
	GLclampf zNear, 
	GLclampf zFar )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( zNear == zFar )
	{
		GL_P_SET_ERROR(GL_INVALID_VALUE);
		return;
	}

	pContext->fDepthNear = GL_P_MAX(zNear, -1.0);
	pContext->fDepthFar = GL_P_MIN(zFar, 1.0);
	pContext->fHalfDepth = (pContext->fDepthFar - pContext->fDepthNear) / 2.0;
	pContext->fInvDepthRange = 1.0 / (pContext->fDepthFar - pContext->fDepthNear);
}

/***************************************************************************/
GLAPI void APIENTRY glDepthRangex( 
	GLclampx zNear, 
	GLclampx zFar )
{
	glDepthRangef( GL_P_CONVERT_XtoF(zNear), GL_P_CONVERT_XtoF(zFar) );
}

/* End of File */
