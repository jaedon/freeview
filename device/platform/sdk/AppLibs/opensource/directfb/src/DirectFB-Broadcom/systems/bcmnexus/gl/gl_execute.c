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
 * $brcm_Workfile: gl_execute.c $
 * $brcm_Revision: DirectFB_1_4_14_Port/1 $
 * $brcm_Date: 7/28/11 11:22a $
 *
 * Module Description: Execute entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.5/systems/bcmnexus/gl/gl_execute.c $
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
 * Hydra_Software_Devel/8   9/8/08 9:02p nissen
 * PR 46638: Added support for waiting for blits.
 * 
 * Hydra_Software_Devel/7   11/15/07 8:08p nissen
 * PR 35855: Added support for new native model.
 * 
 * Hydra_Software_Devel/6   9/10/07 11:53a nissen
 * PR 30062: Fixed flush and finish functions.
 * 
 * Hydra_Software_Devel/5   5/7/07 1:03p nissen
 * PR 30062: Chaned glFlush to initiate render without flushing it.
 * 
 * Hydra_Software_Devel/4   9/17/04 3:10p nissen
 * PR 9132: Changed parameters for calling initiate render function.
 * 
 * Hydra_Software_Devel/3   9/7/04 3:43p nissen
 * PR 9132: Added buffer swapping support.
 * 
 * Hydra_Software_Devel/2   7/28/04 1:14p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include <direct/messages.h>

#include "gl_private.h"


pthread_once_t __P3DGL_context_once;
pthread_key_t  __P3DGL_context_tls;


static inline void GL_P_Render( 
     p3dOpenGLContextData *pContext,
	GLboolean bFlush )
{
	if( bFlush )
		BP3D_IssueRenderAndWait( pContext->hP3d );
	else
		BP3D_IssueRender( pContext->hP3d, 0, 0, 0 );
}

void GL_P_InitiateRender( 
	p3dOpenGLContextData *pContext,
	GLboolean bFlush )
{
//	if( pContext->pDrawSurface == NULL )
//		return;

	GL_P_Render( pContext, bFlush );
	pContext->bfClearMask = 0;
}


/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glFinish( void )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();

//        D_UNIMPLEMENTED();

//	BP3D_SetDstColorSurface( pContext->hP3d, pContext->pDrawSurface->hRenderSurface );
//	BP3D_SetSrcColorSurface( pContext->hP3d, pContext->pDrawSurface->hRenderSurface );

	GL_P_InitiateRender( pContext, GL_TRUE );
//	native_P_BlitWait( pContext, pContext->nativeDisplay, pContext->pDrawSurface->hRenderSurface );
}

/***************************************************************************/
GLAPI void APIENTRY glFlush( void )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();

//        D_UNIMPLEMENTED();

//	BP3D_SetDstColorSurface( pContext->hP3d, pContext->pDrawSurface->hRenderSurface );
//	BP3D_SetSrcColorSurface( pContext->hP3d, pContext->pDrawSurface->hRenderSurface );

	GL_P_InitiateRender( pContext, GL_FALSE );
//	native_P_BlitWait( pContext, pContext->nativeDisplay, pContext->pDrawSurface->hRenderSurface );
}

/* End of File */
