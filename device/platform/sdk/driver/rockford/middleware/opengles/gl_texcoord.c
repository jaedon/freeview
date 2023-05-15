/***************************************************************************
 *     Copyright (c) 2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_texcoord.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 7/28/04 2:11p $
 *
 * Module Description: Texture Coordinate entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_texcoord.c $
 * 
 * Hydra_Software_Devel/2   7/28/04 2:11p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glMultiTexCoord4f( 
	GLenum target, 
	GLfloat s, 
	GLfloat t, 
	GLfloat r, 
	GLfloat q )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	BSTD_UNUSED( target );
	BSTD_UNUSED( r );
	BSTD_UNUSED( q );

	pContext->TexCoord.fS = s;
	pContext->TexCoord.fT = t;
}

/***************************************************************************/
GLAPI void APIENTRY glMultiTexCoord4x( 
	GLenum target, 
	GLfixed s, 
	GLfixed t, 
	GLfixed r, 
	GLfixed q )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	BSTD_UNUSED( target );
	BSTD_UNUSED( r );
	BSTD_UNUSED( q );

	pContext->TexCoord.fS = GL_P_CONVERT_XtoF(s);;
	pContext->TexCoord.fT = GL_P_CONVERT_XtoF(t);;
}

/* End of File */
