/***************************************************************************
 *     Copyright (c) 2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_stencil.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 7/28/04 2:08p $
 *
 * Module Description: Stencil entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_stencil.c $
 * 
 * Hydra_Software_Devel/2   7/28/04 2:08p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glStencilFunc( 
	GLenum func, 
	GLint ref, 
	GLuint mask )
{
	BSTD_UNUSED( func );
	BSTD_UNUSED( ref );
	BSTD_UNUSED( mask );
}

/***************************************************************************/
GLAPI void APIENTRY glStencilMask( 
	GLuint mask )
{
	BSTD_UNUSED( mask );
}

/***************************************************************************/
GLAPI void APIENTRY glStencilOp( 
	GLenum fail, 
	GLenum zfail, 
	GLenum zpass )
{
	BSTD_UNUSED( fail );
	BSTD_UNUSED( zfail );
	BSTD_UNUSED( zpass );
}

/* End of File */
