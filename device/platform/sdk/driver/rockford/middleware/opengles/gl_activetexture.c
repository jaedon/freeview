/***************************************************************************
 *     Copyright (c) 2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_activetexture.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 7/28/04 12:44p $
 *
 * Module Description: Active Texture entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_activetexture.c $
 * 
 * Hydra_Software_Devel/2   7/28/04 12:44p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glActiveTexture( 
	GLenum texture )
{
	BSTD_UNUSED( texture );
}

/***************************************************************************/
GLAPI void APIENTRY glClientActiveTexture(
	GLenum texture )
{
	BSTD_UNUSED( texture );
}

/* End of File */
