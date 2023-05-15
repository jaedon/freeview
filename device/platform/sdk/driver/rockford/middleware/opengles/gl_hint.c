/***************************************************************************
 *     Copyright (c) 2004-2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_hint.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 10/17/05 9:01p $
 *
 * Module Description: Hint entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_hint.c $
 * 
 * Hydra_Software_Devel/3   10/17/05 9:01p nissen
 * PR 17656: Added support for generating mipmap lods.
 * 
 * Hydra_Software_Devel/2   7/28/04 1:30p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glHint( 
	GLenum target, 
	GLenum mode )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();

	if( target == GL_GENERATE_MIPMAP_HINT )
		pDisplay->bGenerateMipmaps = (mode == GL_NICEST) ? GL_TRUE : GL_FALSE;
}

/* End of File */
