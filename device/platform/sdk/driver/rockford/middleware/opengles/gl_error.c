/***************************************************************************
 *     Copyright (c) 2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_error.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 7/28/04 1:10p $
 *
 * Module Description: Error entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_error.c $
 * 
 * Hydra_Software_Devel/2   7/28/04 1:10p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI GLenum APIENTRY glGetError( void )
{
	EGL_P_Context *pContext = EGL_P_GetContext();
	GLenum eError = pContext->eError;

	pContext->eError = GL_NO_ERROR;
	return eError;
}

/* End of File */
