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
 * $brcm_Workfile: gl_clear.c $
 * $brcm_Revision: DirectFB_1_4_14_Port/1 $
 * $brcm_Date: 7/28/11 11:22a $
 *
 * Module Description: Clear entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.5/systems/bcmnexus/gl/gl_clear.c $
 * 
 * DirectFB_1_4_14_Port/1   7/28/11 11:22a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 * 
 * DirectFB_1_4_5_Port/1   6/14/11 4:44p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 * 
 * DirectFB_1_4_Port/1   8/6/10 8:07p robertwm
 * SW7405-4724: DirectFB: Support OpenGL ES 1.0 applications.
 * 
 * Hydra_Software_Devel/7   9/23/08 6:23p nissen
 * PR 46638: Optimized rectangle blits.
 * 
 * Hydra_Software_Devel/6   9/8/08 12:35p nissen
 * PR 46638: Added support for screen aligned rectangle and drawing it
 * with the blitter.
 * 
 * Hydra_Software_Devel/5   9/17/04 3:23p nissen
 * PR 9132: Fixed logic op rendering.
 * 
 * Hydra_Software_Devel/4   9/7/04 4:08p nissen
 * PR 9132: Added support for logic op rendering.
 * 
 * Hydra_Software_Devel/3   8/10/04 3:03p nissen
 * PR 9132: Added support for clearing depth buffer to specified value.
 * 
 * Hydra_Software_Devel/2   7/28/04 12:51p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include <direct/messages.h>

#include "gl_private.h"

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glClear( 
	GLbitfield mask )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();

	if( mask & (~(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)) )
	{
		GL_P_SET_ERROR(GL_INVALID_VALUE);
		return;
	}

	if( pContext->bRectangleBlit2D )
	{
	     D_UNIMPLEMENTED();
//		native_P_BlitSurface( pContext, pContext->nativeDisplay, pContext->pDrawSurface->hRenderSurface, 
//			pContext->iViewportOffsetX, pContext->iViewportOffsetY, pContext->iViewportWidth, 
//			pContext->iViewportHeight, 0, 0, 0, 0, 0, 0, 0, 1, 1, pContext->ulClearColor, GL_FALSE );
	}
	else
	{
		if( mask & GL_COLOR_BUFFER_BIT )
			BP3D_ClearColor( pContext->hP3d, pContext->ulClearColor );
	
		if( mask & GL_DEPTH_BUFFER_BIT )
			BP3D_ClearDepth( pContext->hP3d, pContext->fClearDepth );
	}

	pContext->bfClearMask = mask;
}

/***************************************************************************/
GLAPI void APIENTRY glClearColor( 
	GLclampf red, 
	GLclampf green, 
	GLclampf blue, 
	GLclampf alpha )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();
	pContext->ulClearColor = GL_P_CONVERT_COLOR_FtoI(alpha, red, green, blue);
}

/***************************************************************************/
GLAPI void APIENTRY glClearColorx( 
	GLclampx red, 
	GLclampx green, 
	GLclampx blue, 
	GLclampx alpha )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();
	pContext->ulClearColor = GL_P_CONVERT_COLOR_XtoI(alpha, red, green, blue);
}

/***************************************************************************/
GLAPI void APIENTRY glClearDepthf( 
	GLclampf depth )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();
	pContext->fClearDepth = depth;
}

/***************************************************************************/
GLAPI void APIENTRY glClearDepthx( 
	GLclampx depth )
{
	glClearDepthf( GL_P_CONVERT_XtoF(depth) );
}

/***************************************************************************/
GLAPI void APIENTRY glClearStencil( 
	GLint s )
{
	BSTD_UNUSED( s );
}

/* End of File */
