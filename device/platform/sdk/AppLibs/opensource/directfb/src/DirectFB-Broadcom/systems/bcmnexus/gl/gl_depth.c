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
 * $brcm_Workfile: gl_depth.c $
 * $brcm_Revision: DirectFB_1_4_14_Port/1 $
 * $brcm_Date: 7/28/11 11:22a $
 *
 * Module Description: Depth entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.5/systems/bcmnexus/gl/gl_depth.c $
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
 * Hydra_Software_Devel/2   7/28/04 1:02p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "gl_private.h"

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glDepthFunc( 
	GLenum func )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();

	BP3D_Compare eCompare = GL_P_ConvertCompare_GLtoP3D( func );
	if( eCompare == (BP3D_Compare) -1 )
		return;

	BP3D_SetDepthCompare( pContext->hP3d, pContext->bDepthTest ? eCompare : BP3D_Compare_Always );
	pContext->eDepthFunc = func;
}

/***************************************************************************/
GLAPI void APIENTRY glDepthMask( 
	GLboolean flag )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();

	BP3D_SetDepthMask( pContext->hP3d, pContext->bDepthTest ? (bool) (!flag) : true );
	pContext->bDepthMask = flag;
}

/***************************************************************************/
GLAPI void APIENTRY glDepthRangef( 
	GLclampf zNear, 
	GLclampf zFar )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();

	pContext->fDepthNear = GL_P_MAX(zNear, -1.0);
	pContext->fDepthFar = GL_P_MIN(zFar, 1.0);
	pContext->fHalfDepth = (pContext->fDepthFar - pContext->fDepthNear) / 2.0;
	pContext->fInvDepthRange = 1.0 / (pContext->fDepthFar - pContext->fDepthNear);
}

/***************************************************************************/
GLAPI void APIENTRY glDepthRangex( 
	GLclampx zNear, 
	GLclampx zFar )
{
	glDepthRangef( GL_P_CONVERT_XtoF(zNear), GL_P_CONVERT_XtoF(zFar) );
}

/* End of File */
