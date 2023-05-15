/***************************************************************************
 *     Copyright (c) 2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: egl_error.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 7/28/04 12:28p $
 *
 * Module Description: Error entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/egl_error.c $
 * 
 * Hydra_Software_Devel/2   7/28/04 12:28p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
GLAPI EGLint APIENTRY eglGetError( void )
{
	EGLint iError;

	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	if( pDisplay == NULL )
	{
		EGL_P_SET_ERROR(EGL_BAD_DISPLAY);
		return 0;
	}

	iError = pDisplay->iError;
	pDisplay->iError = EGL_SUCCESS;

	return iError;
}

/* End of File */
