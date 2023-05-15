/***************************************************************************
 *     (c)2004-2012 Broadcom Corporation
 *  
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *   
 *  Except as expressly set forth in the Authorized License,
 *   
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *   
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *  
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 *  ANY LIMITED REMEDY.
 * 
 * $brcm_Workfile: gl_logicop.c $
 * $brcm_Revision: DirectFB_1_4_14_Port/1 $
 * $brcm_Date: 7/28/11 11:22a $
 *
 * Module Description: Logical operation entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.5/systems/bcmnexus/gl/gl_logicop.c $
 * 
 * DirectFB_1_4_14_Port/1   7/28/11 11:22a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 * 
 * DirectFB_1_4_5_Port/1   6/14/11 4:45p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 * 
 * DirectFB_1_4_Port/1   8/6/10 8:07p robertwm
 * SW7405-4724: DirectFB: Support OpenGL ES 1.0 applications.
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

#include "gl_private.h"

/***************************************************************************/
GLubyte GL_P_ConvertLogicOp_GLtoP3D( 
	GLenum opcode )
{
	switch( opcode )
	{
	case GL_CLEAR:         return 0x00;     /* 0 */
	case GL_NOR:           return 0x11;     /* !(s | d) */
	case GL_AND_INVERTED:  return 0x22;     /* !s & d */
	case GL_COPY_INVERTED: return 0x33;     /* !s */
	case GL_AND_REVERSE:   return 0x44;     /* s & !d */
	case GL_INVERT:        return 0x55;     /* !d */
	case GL_XOR:           return 0x66;     /* s ^ d */
	case GL_NAND:          return 0x77;     /* !(s & d) */
	case GL_AND:           return 0x88;     /* s & d */
	case GL_EQUIV:         return 0x99;     /* !(s ^ d) */
	case GL_NOOP:          return 0xAA;     /* d */
	case GL_OR_INVERTED:   return 0xBB;     /* !s | d */
	case GL_COPY:          return 0xCC;     /* s */
	case GL_OR_REVERSE:    return 0xDD;     /* s | !d */
	case GL_OR:            return 0xEE;     /* s | d */
	case GL_SET:           return 0xFF;     /* 1 */
	default: GL_P_SET_ERROR(GL_INVALID_ENUM);
	}

	return (GLubyte) -1;
}

/***************************************************************************/
void GL_P_SetLogicOp( 
	p3dOpenGLContextData *pContext,
	GLboolean bLogicOp,
	GLenum eLogicOp )
{
	GLboolean bCurrLogicOp = pContext->bLogicOp && (pContext->eLogicOp != GL_COPY);
	GLboolean bNextLogicOp = bLogicOp && (eLogicOp != GL_COPY);

	if( (bCurrLogicOp != bNextLogicOp) || (bCurrLogicOp && bNextLogicOp && (eLogicOp != pContext->eLogicOp)) )
		GL_P_InitiateRender( pContext, GL_TRUE );
}

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glLogicOp( 
	GLenum opcode )
{
/*	EGL_P_Display *pDisplay = EGL_P_GetDisplay();*/
	p3dOpenGLContextData *pContext = GL_P_GetContext();

	GLubyte ucRop3 = GL_P_ConvertLogicOp_GLtoP3D( opcode );
	if( ucRop3 == (GLubyte) -1 )
		return;
	
/*	GL_P_SetLogicOp( pContext, pContext->bLogicOp, opcode );*/

	pContext->eLogicOp = opcode;
	pContext->ucRop3 = ucRop3;
}

/* End of File */
