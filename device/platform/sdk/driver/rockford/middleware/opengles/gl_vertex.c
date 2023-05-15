/***************************************************************************
 *     Copyright (c) 2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_vertex.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 7/28/04 2:26p $
 *
 * Module Description: Vertex entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_vertex.c $
 * 
 * Hydra_Software_Devel/2   7/28/04 2:26p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glPointSize( 
	GLfloat size )
{
	BSTD_UNUSED( size );
}

/***************************************************************************/
GLAPI void APIENTRY glPointSizex( 
	GLfixed size )
{
	BSTD_UNUSED( size );
}

/***************************************************************************/
GLAPI void APIENTRY glLineWidth( 
	GLfloat width )
{
	BSTD_UNUSED( width );
}

/***************************************************************************/
GLAPI void APIENTRY glLineWidthx( 
	GLfixed width )
{
	BSTD_UNUSED( width );
}

/* End of File */
