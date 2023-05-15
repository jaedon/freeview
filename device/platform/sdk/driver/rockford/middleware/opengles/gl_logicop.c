/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_logicop.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 3/12/10 2:41p $
 *
 * Module Description: Logical operation entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_logicop.c $
 * 
 * Hydra_Software_Devel/8   3/12/10 2:41p nissen
 * SW7405-2856: Added ucRop3 in context structure.
 * 
 * Hydra_Software_Devel/7   7/4/09 5:17p nissen
 * PR 56587: Changed LogicOp to using blending for OpenGL conformance.
 * 
 * Hydra_Software_Devel/6   10/23/08 2:16p nissen
 * PR 46638: Fixed warning.
 * 
 * Hydra_Software_Devel/5   9/8/08 12:53p nissen
 * PR 46638: Added support for screen aligned rectangle and drawing it
 * with the blitter.
 * 
 * Hydra_Software_Devel/4   9/17/04 3:15p nissen
 * PR 9132: Fixed logic op rendering.
 * 
 * Hydra_Software_Devel/3   9/7/04 4:12p nissen
 * PR 9132: Added support for logic op rendering.
 * 
 * Hydra_Software_Devel/2   7/28/04 12:15p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glLogicOp( 
	GLenum opcode )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

	switch( opcode )
	{
	case GL_CLEAR:         /* 0 */
		BP3D_SetSrcBlend( pDisplay->hP3d, BP3D_Blend_Zero );
		BP3D_SetDstBlend( pDisplay->hP3d, BP3D_Blend_Zero );
		pContext->ucRop3 = 0x00;
		break;
	case GL_XOR:           /* s ^ d */
		BP3D_SetSrcBlend( pDisplay->hP3d, BP3D_Blend_InvDstColor );
		BP3D_SetDstBlend( pDisplay->hP3d, BP3D_Blend_InvSrcColor );
		pContext->ucRop3 = 0x66;
		break;
	case GL_AND:           /* s & d */
		BP3D_SetSrcBlend( pDisplay->hP3d, BP3D_Blend_DstColor );
		BP3D_SetDstBlend( pDisplay->hP3d, BP3D_Blend_SrcColor );
		pContext->ucRop3 = 0x88;
		break;
	case GL_NOOP:          /* d */
		BP3D_SetSrcBlend( pDisplay->hP3d, BP3D_Blend_Zero );
		BP3D_SetDstBlend( pDisplay->hP3d, BP3D_Blend_One );
		pContext->ucRop3 = 0xAA;
		break;
	case GL_COPY:          /* s */
		BP3D_SetSrcBlend( pDisplay->hP3d, BP3D_Blend_One );
		BP3D_SetDstBlend( pDisplay->hP3d, BP3D_Blend_Zero );
		pContext->ucRop3 = 0xCC;
		break;
	case GL_OR:            /* s | d */
		BP3D_SetSrcBlend( pDisplay->hP3d, BP3D_Blend_One );
		BP3D_SetDstBlend( pDisplay->hP3d, BP3D_Blend_One );
		pContext->ucRop3 = 0xEE;
		break;
	case GL_NOR:           /* !(s | d) */
		pContext->ucRop3 = 0x11;
		break;
	case GL_AND_INVERTED:  /* !s & d */
		pContext->ucRop3 = 0x22;
		break;
	case GL_COPY_INVERTED: /* !s */
		pContext->ucRop3 = 0x33;
		break;
	case GL_AND_REVERSE:   /* s & !d */
		pContext->ucRop3 = 0x44;
		break;
	case GL_INVERT:        /* !d */
		pContext->ucRop3 = 0x55;
		break;
	case GL_NAND:          /* !(s & d) */
		pContext->ucRop3 = 0x77;
		break;
	case GL_EQUIV:         /* !(s ^ d) */
		pContext->ucRop3 = 0x99;
		break;
	case GL_OR_INVERTED:   /* !s | d */
		pContext->ucRop3 = 0xDD;
		break;
	case GL_OR_REVERSE:    /* s | !d */
		pContext->ucRop3 = 0xBB;
		break;
	case GL_SET:           /* 1 */
		pContext->ucRop3 = 0xFF;
		break;
	default: 
		GL_P_SET_ERROR(GL_INVALID_ENUM);
	}

	pContext->eLogicOp = opcode;
}

/* End of File */
