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
 * $brcm_Workfile: gl_fog.c $
 * $brcm_Revision: DirectFB_1_4_14_Port/1 $
 * $brcm_Date: 7/28/11 11:22a $
 *
 * Module Description: Fog entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.5/systems/bcmnexus/gl/gl_fog.c $
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
 * Hydra_Software_Devel/5   4/27/07 3:17p nissen
 * PR 30062: Changed exponential fog to pass conformance.
 * 
 * Hydra_Software_Devel/4   9/20/06 1:32p nissen
 * PR 22858: Fixed fog.
 * 
 * Hydra_Software_Devel/3   8/10/04 2:57p nissen
 * PR 9132: Fixed fog by switching from table to vertex.
 * 
 * Hydra_Software_Devel/2   7/28/04 1:22p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "gl_private.h"

/***************************************************************************/
void GL_P_SetVertexFogIndex(
	p3dOpenGLContextData *pContext, 
	BP3D_Vertex *pVertexIn,
	BP3D_Vertex *pVertexOut )
{
	GLfloat fFogExp;
	GLfloat fFogIndex;
	GLfloat fFogClamp = 0.0;
	GLfloat fZ;

	switch( pContext->eFogMode )
	{
	case GL_LINEAR:
		fZ = (pVertexIn->fZ < 0.0) ? -pVertexIn->fZ : pVertexIn->fZ;
		fFogIndex = (pContext->fTransFogEnd - fZ) / (pContext->fTransFogEnd - pContext->fTransFogStart);
		fFogClamp = GL_P_MIN(GL_P_MAX(fFogIndex, 0.0), 1.0) * 255.0;
		break;
	case GL_EXP:
/*		fFogExp   = pContext->fFogDensity * pVertexIn->fZ * 8;
*/		fFogExp   = pContext->fFogDensity * pVertexIn->fZ;
		fFogIndex = exp( -fFogExp );
		fFogClamp = GL_P_MIN(GL_P_MAX(fFogIndex, 0.0), 1.0) * 255.0;
		break;
	case GL_EXP2:
/*		fFogExp   = pContext->fFogDensity * pVertexIn->fZ * 8;
*/		fFogExp   = pContext->fFogDensity * pVertexIn->fZ;
		fFogIndex = exp( -fFogExp * fFogExp );
		fFogClamp = GL_P_MIN(GL_P_MAX(fFogIndex, 0.0), 1.0) * 255.0;
		break;
	default:
		break;
	}
	
	pVertexOut->ulSpecular |= ((uint32_t) fFogClamp) << 24;
}

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glFogf( 
	GLenum pname, 
	GLfloat param )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();

	switch( pname )
	{
	case GL_FOG_MODE:
		if( ((GLenum) param != GL_LINEAR) && ((GLenum) param != GL_EXP) && ((GLenum) param != GL_EXP2) )
		{
			GL_P_SET_ERROR(GL_INVALID_VALUE);
			return;
		}
		pContext->eFogMode = (GLenum) param;
		BP3D_SetFog( pContext->hP3d, pContext->bFog ? BP3D_Fog_Vertex : BP3D_Fog_None );
		break;

	case GL_FOG_DENSITY:
		if( param < 0.0 )
		{
			GL_P_SET_ERROR(GL_INVALID_VALUE);
			return;
		}
		pContext->fFogDensity = param;
		break;

	case GL_FOG_START:
		pContext->fFogStart = param;
		break;

	case GL_FOG_END:
		pContext->fFogEnd = param;
		break;

	default: 
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}
}

/***************************************************************************/
GLAPI void APIENTRY glFogfv( 
	GLenum pname, 
	const GLfloat *params )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();

	if( pname == GL_FOG_COLOR )
	{
		pContext->ulFogColor = GL_P_CONVERT_COLOR_FtoI(params[3], params[0], params[1], params[2]);
		BP3D_SetFogColor( pContext->hP3d, pContext->ulFogColor );
	}
	else
	{
		glFogf( pname, *params );
	}
}

/***************************************************************************/
GLAPI void APIENTRY glFogx( 
	GLenum pname, 
	GLfixed param )
{
	glFogf( pname, (pname == GL_FOG_MODE) ? (GLfloat) param : GL_P_CONVERT_XtoF(param) );
}

/***************************************************************************/
GLAPI void APIENTRY glFogxv( 
	GLenum pname, 
	const GLfixed *params )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();

	if( pname == GL_FOG_COLOR )
	{
		pContext->ulFogColor = GL_P_CONVERT_COLOR_XtoI(params[3], params[0], params[1], params[2]);
		BP3D_SetFogColor( pContext->hP3d, pContext->ulFogColor );
	}
	else
	{
		glFogx( pname, *params );
	}
}

/* End of File */
