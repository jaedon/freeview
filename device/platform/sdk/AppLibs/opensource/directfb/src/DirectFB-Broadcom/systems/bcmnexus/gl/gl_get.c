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
 * $brcm_Workfile: gl_get.c $
 * $brcm_Revision: DirectFB_1_4_14_Port/1 $
 * $brcm_Date: 7/28/11 11:22a $
 *
 * Module Description: Get entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.5/systems/bcmnexus/gl/gl_get.c $
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
 * Hydra_Software_Devel/5   3/9/05 2:58p nissen
 * PR 12818: Fixed support for triple buffer flipping.
 * 
 * Hydra_Software_Devel/4   9/7/04 3:47p nissen
 * PR 9132: Added buffer swapping support.
 * 
 * Hydra_Software_Devel/3   8/10/04 2:31p nissen
 * PR 9132: Increased number of compressed texture formats returned in get
 * function.
 * 
 * Hydra_Software_Devel/2   7/28/04 1:26p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "gl_private.h"

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glGetIntegerv(
	GLenum pname, 
	GLint *params )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();

	switch( pname )
	{
	case GL_ALPHA_BITS:
		params[0] = pContext->bits.alpha;
                D_UNIMPLEMENTED();
		break;
	case GL_RED_BITS:
		params[0] = pContext->bits.red;
                D_UNIMPLEMENTED();
		break;
	case GL_GREEN_BITS:
		params[0] = pContext->bits.green;
                D_UNIMPLEMENTED();
		break;
	case GL_BLUE_BITS:
		params[0] = pContext->bits.blue;
                D_UNIMPLEMENTED();
		break;
	case GL_DEPTH_BITS:
		params[0] = pContext->bits.depth;
                D_UNIMPLEMENTED();
		break;
	case GL_STENCIL_BITS:
		params[0] = 0;
		break;
	case GL_ALIASED_POINT_SIZE_RANGE:
	case GL_ALIASED_LINE_WIDTH_RANGE:
	case GL_SMOOTH_POINT_SIZE_RANGE:
	case GL_SMOOTH_LINE_WIDTH_RANGE:
		params[0] = 1;
		params[1] = 1;
		break;
	case GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES:
		params[0] = pContext->bits.alpha ? GL_RGBA : GL_RGB;
                D_UNIMPLEMENTED();
		break;
	case GL_IMPLEMENTATION_COLOR_READ_TYPE_OES:
             D_UNIMPLEMENTED();
		switch( pContext->bits.green )
		{
		case 4: params[0] = GL_UNSIGNED_SHORT_4_4_4_4; break;
		case 5: params[0] = GL_UNSIGNED_SHORT_5_5_5_1; break;
		case 6: params[0] = GL_UNSIGNED_SHORT_5_6_5; break;
		case 8: params[0] = GL_UNSIGNED_BYTE; break;
		}
		break;
	case GL_MAX_ELEMENTS_INDICES:
		params[0] = GL_P_VERTEX_MAX;
		break;
	case GL_MAX_ELEMENTS_VERTICES:
		params[0] = GL_P_VERTEX_MAX;
		break;
	case GL_MAX_LIGHTS:
		params[0] = GL_P_LIGHT_MAX;
		break;
	case GL_MAX_MODELVIEW_STACK_DEPTH:
		params[0] = GL_P_MATRIX_STACK_DEPTH;
		break;
	case GL_MAX_PROJECTION_STACK_DEPTH:
		params[0] = GL_P_MATRIX_STACK_DEPTH;
		break;
	case GL_MAX_TEXTURE_STACK_DEPTH:
		params[0] = GL_P_MATRIX_STACK_DEPTH;
		break;
	case GL_MAX_TEXTURE_SIZE:
		params[0] = GL_P_TEXTURE_SIZE_MAX;
		break;
	case GL_MAX_TEXTURE_UNITS:
		params[0] = GL_P_TEXTURE_UNIT_MAX;
		break;
	case GL_MAX_VIEWPORT_DIMS:
                params[0] = pContext->size.w;
                params[1] = pContext->size.h;
                D_UNIMPLEMENTED();
		break;
	case GL_COMPRESSED_TEXTURE_FORMATS:
		params[0] = 0;
		break;
	case GL_NUM_COMPRESSED_TEXTURE_FORMATS:
		params[0] = 1;
		break;
	case GL_SUBPIXEL_BITS:
		params[0] = 0;
		break;
	default: 
		GL_P_SET_ERROR(GL_INVALID_ENUM);
	}
}

/***************************************************************************/
GLAPI const GLubyte * APIENTRY glGetString( 
	GLenum name )
{
	switch( name )
	{
	case GL_VENDOR:     return (GLubyte *) GL_P_VENDOR;
	case GL_RENDERER:   return (GLubyte *) GL_P_RENDERER;
	case GL_VERSION:    return (GLubyte *) GL_P_VERSION;
	case GL_EXTENSIONS: return (GLubyte *) GL_P_EXTENSIONS;
	default: 
		GL_P_SET_ERROR(GL_INVALID_ENUM);
	}

	return NULL;
}

/* End of File */
