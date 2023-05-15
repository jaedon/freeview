/***************************************************************************
 *     Copyright (c) 2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_normal.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 7/28/04 1:49p $
 *
 * Module Description: Normal entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_normal.c $
 * 
 * Hydra_Software_Devel/2   7/28/04 1:49p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glNormal3f(
	GLfloat nx, 
	GLfloat ny, 
	GLfloat nz )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	pContext->Normal.fX = nx;
	pContext->Normal.fY = ny;
	pContext->Normal.fZ = nz;
}

/***************************************************************************/
GLAPI void APIENTRY glNormal3x( 
	GLfixed nx, 
	GLfixed ny, 
	GLfixed nz )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	pContext->Normal.fX = GL_P_CONVERT_XtoF(nx);
	pContext->Normal.fY = GL_P_CONVERT_XtoF(ny);
	pContext->Normal.fZ = GL_P_CONVERT_XtoF(nz);
}

/* End of File */
