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
 * $brcm_Workfile: gl_material.c $
 * $brcm_Revision: DirectFB_1_4_14_Port/1 $
 * $brcm_Date: 7/28/11 11:22a $
 *
 * Module Description: Material entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.5/systems/bcmnexus/gl/gl_material.c $
 * 
 * DirectFB_1_4_14_Port/1   7/28/11 11:22a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 * 
 * DirectFB_1_4_5_Port/1   6/14/11 4:45p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 * 
 * DirectFB_1_4_Port/1   8/6/10 8:08p robertwm
 * SW7405-4724: DirectFB: Support OpenGL ES 1.0 applications.
 * 
 * Hydra_Software_Devel/2   7/28/04 1:41p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "gl_private.h"

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glMaterialf( 
	GLenum face, 
	GLenum pname, 
	GLfloat param )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();

	if( (face != GL_FRONT_AND_BACK) || (pname != GL_SHININESS) )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	if( (param < 0.0) || (param > 128.0) )
	{
		GL_P_SET_ERROR(GL_INVALID_VALUE);
		return;
	}

	pContext->Material.fSpecularExponent = param;
}

/***************************************************************************/
GLAPI void APIENTRY glMaterialfv( 
	GLenum face, 
	GLenum pname, 
	const GLfloat *params )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();

	if( face != GL_FRONT_AND_BACK )
	{
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}

	switch( pname )
	{
	case GL_SHININESS:
		glMaterialf( face, pname, params[0] );
		break;
	case GL_AMBIENT:
		GL_P_SET_LIGHT_COLOR( &pContext->Material.AmbientColor, params[0], params[1], params[2], params[3] );
		break;
	case GL_DIFFUSE:
		GL_P_SET_LIGHT_COLOR( &pContext->Material.DiffuseColor, params[0], params[1], params[2], params[3] );
		break;
	case GL_SPECULAR:
		GL_P_SET_LIGHT_COLOR( &pContext->Material.SpecularColor, params[0], params[1], params[2], params[3] );
		break;
	case GL_EMISSION:
		GL_P_SET_LIGHT_COLOR( &pContext->Material.EmissionColor, params[0], params[1], params[2], params[3] );
		break;
	case GL_AMBIENT_AND_DIFFUSE:
		GL_P_SET_LIGHT_COLOR( &pContext->Material.AmbientColor, params[0], params[1], params[2], params[3] );
		GL_P_SET_LIGHT_COLOR( &pContext->Material.DiffuseColor, params[0], params[1], params[2], params[3] );
		break;
	default:
		GL_P_SET_ERROR(GL_INVALID_ENUM);
		return;
	}
}

/***************************************************************************/
GLAPI void APIENTRY glMaterialx( 
	GLenum face, 
	GLenum pname, 
	GLfixed param )
{
	glMaterialf( face, pname, GL_P_CONVERT_XtoF(param) );
}

/***************************************************************************/
GLAPI void APIENTRY glMaterialxv( 
	GLenum face, 
	GLenum pname, 
	const GLfixed *params )
{
	if( pname == GL_SHININESS )
	{
		glMaterialf( face, pname, GL_P_CONVERT_XtoF(params[0]) );
	}
	else
	{
		GLfloat afParams[4];
		afParams[0] = GL_P_CONVERT_XtoF(params[0]);
		afParams[1] = GL_P_CONVERT_XtoF(params[1]);
		afParams[2] = GL_P_CONVERT_XtoF(params[2]);
		afParams[3] = GL_P_CONVERT_XtoF(params[3]);
		glMaterialfv( face, pname, afParams );
	}
}

/* End of File */
