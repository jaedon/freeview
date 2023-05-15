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
 * $brcm_Workfile: gl_alphablend.c $
 * $brcm_Revision: DirectFB_1_4_14_Port/1 $
 * $brcm_Date: 7/28/11 11:22a $
 *
 * Module Description: Alpha and Blend Func entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.5/systems/bcmnexus/gl/gl_alphablend.c $
 * 
 * DirectFB_1_4_14_Port/1   7/28/11 11:22a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 * 
 * DirectFB_1_4_5_Port/1   6/14/11 4:44p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 * 
 * DirectFB_1_4_Port/1   8/6/10 8:06p robertwm
 * SW7405-4724: DirectFB: Support OpenGL ES 1.0 applications.
 * 
 * Hydra_Software_Devel/4   9/23/08 6:16p nissen
 * PR 46638: Optimized rectangle blits.
 * 
 * Hydra_Software_Devel/3   9/8/08 12:29p nissen
 * PR 46638: Added support for screen aligned rectangle and drawing it
 * with the blitter.
 * 
 * Hydra_Software_Devel/2   7/28/04 12:47p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "gl_private.h"

/***************************************************************************/
BP3D_Compare GL_P_ConvertCompare_GLtoP3D( 
	GLenum func )
{
	switch( func )
	{
	case GL_NEVER:    return BP3D_Compare_Never;
	case GL_LESS:     return BP3D_Compare_Less;
	case GL_EQUAL:    return BP3D_Compare_Equal;
	case GL_LEQUAL:   return BP3D_Compare_LessEqual;
	case GL_GREATER:  return BP3D_Compare_Greater;
	case GL_NOTEQUAL: return BP3D_Compare_NotEqual;
	case GL_GEQUAL:   return BP3D_Compare_GreaterEqual;
	case GL_ALWAYS:   return BP3D_Compare_Always;
	default: GL_P_SET_ERROR(GL_INVALID_ENUM);
	}

	return (BP3D_Compare) -1;
}

/***************************************************************************/
BP3D_Blend GL_P_ConvertBlend_GLtoP3D( 
	GLenum factor )
{
	switch( factor )
	{
	case GL_ZERO:                return BP3D_Blend_Zero;
	case GL_ONE:                 return BP3D_Blend_One;
	case GL_SRC_ALPHA:           return BP3D_Blend_SrcAlpha;
	case GL_ONE_MINUS_SRC_ALPHA: return BP3D_Blend_InvSrcAlpha;
	case GL_SRC_COLOR:           return BP3D_Blend_SrcColor;
	case GL_ONE_MINUS_SRC_COLOR: return BP3D_Blend_InvSrcColor;
	case GL_DST_COLOR:           return BP3D_Blend_DstColor;
	case GL_ONE_MINUS_DST_COLOR: return BP3D_Blend_InvDstColor;
	case GL_DST_ALPHA:           return BP3D_Blend_DstAlpha;
	case GL_ONE_MINUS_DST_ALPHA: return BP3D_Blend_InvDstAlpha;
	case GL_SRC_ALPHA_SATURATE:  return BP3D_Blend_SrcAlphaSat;
	default: GL_P_SET_ERROR(GL_INVALID_ENUM);
	}

	return (BP3D_Blend) -1;
}

/***************************************************************************/
BGRC_Blend_Source GL_P_ConvertBlend_GLtoGRC( 
	GLenum factor )
{
	switch( factor )
	{
	case GL_ZERO:                return BGRC_Blend_Source_eZero;
	case GL_ONE:                 return BGRC_Blend_Source_eOne;
	case GL_SRC_ALPHA:           return BGRC_Blend_Source_eSourceAlpha;
	case GL_ONE_MINUS_SRC_ALPHA: return BGRC_Blend_Source_eInverseSourceAlpha;
	case GL_SRC_COLOR:           return BGRC_Blend_Source_eSourceColor;
	case GL_ONE_MINUS_SRC_COLOR: return BGRC_Blend_Source_eInverseSourceColor;
	case GL_DST_COLOR:           return BGRC_Blend_Source_eDestinationColor;
	case GL_ONE_MINUS_DST_COLOR: return BGRC_Blend_Source_eInverseDestinationColor;
	case GL_DST_ALPHA:           return BGRC_Blend_Source_eDestinationAlpha;
	case GL_ONE_MINUS_DST_ALPHA: return BGRC_Blend_Source_eInverseDestinationAlpha;
	}

	return (BGRC_Blend_Source) -1;
}

/***************************************************************************/
BGRC_Blend_Source GL_P_ConvertBlend_GLtoGRC_Alpha( 
	GLenum factor )
{
	switch( factor )
	{
	case GL_ZERO:                return BGRC_Blend_Source_eZero;
	case GL_ONE:                 return BGRC_Blend_Source_eOne;
	case GL_SRC_ALPHA:           return BGRC_Blend_Source_eSourceAlpha;
	case GL_ONE_MINUS_SRC_ALPHA: return BGRC_Blend_Source_eInverseSourceAlpha;
	case GL_SRC_COLOR:           return BGRC_Blend_Source_eSourceAlpha;
	case GL_ONE_MINUS_SRC_COLOR: return BGRC_Blend_Source_eInverseSourceAlpha;
	case GL_DST_COLOR:           return BGRC_Blend_Source_eDestinationAlpha;
	case GL_ONE_MINUS_DST_COLOR: return BGRC_Blend_Source_eInverseDestinationAlpha;
	case GL_DST_ALPHA:           return BGRC_Blend_Source_eDestinationAlpha;
	case GL_ONE_MINUS_DST_ALPHA: return BGRC_Blend_Source_eInverseDestinationAlpha;
	}

	return (BGRC_Blend_Source) -1;
}

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glAlphaFunc( 
	GLenum func, 
	GLclampf ref )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();

	if( pContext->bAlphaTest )
	{
		BP3D_Compare eCompare = GL_P_ConvertCompare_GLtoP3D( func );
		if( eCompare == (BP3D_Compare) -1 )
			return;

		BP3D_SetAlphaCompare( pContext->hP3d, eCompare );

		if( (func != GL_NEVER) && (func != GL_ALWAYS) )
			BP3D_SetAlphaReference( pContext->hP3d, GL_P_CONVERT_CHANNEL_FtoI(ref) );
	}
	else
	{
		BP3D_SetAlphaCompare( pContext->hP3d, BP3D_Compare_Always );
	}

	pContext->eAlphaFunc = func;
	pContext->fAlphaRef = ref;
}

/***************************************************************************/
GLAPI void APIENTRY glAlphaFuncx( 
	GLenum func, 
	GLclampx ref )
{
	glAlphaFunc( func, (GLclampf) ref );
}

/***************************************************************************/
GLAPI void APIENTRY glBlendFunc( 
	GLenum sfactor, 
	GLenum dfactor )
{
	p3dOpenGLContextData *pContext = GL_P_GetContext();

	if( pContext->bBlend )
	{
		BP3D_Blend eSrcBlend = GL_P_ConvertBlend_GLtoP3D( sfactor );
		BP3D_Blend eDstBlend = GL_P_ConvertBlend_GLtoP3D( dfactor );
		if( (eSrcBlend == (BP3D_Blend) -1) || (eDstBlend == (BP3D_Blend) -1) )
			return;

		BP3D_SetSrcBlend( pContext->hP3d, eSrcBlend );
		BP3D_SetDstBlend( pContext->hP3d, eDstBlend );

		pContext->eGRCSrcBlendColor = GL_P_ConvertBlend_GLtoGRC( sfactor );
		pContext->eGRCDstBlendColor = GL_P_ConvertBlend_GLtoGRC( dfactor );
		pContext->eGRCSrcBlendAlpha = GL_P_ConvertBlend_GLtoGRC_Alpha( sfactor );
		pContext->eGRCDstBlendAlpha = GL_P_ConvertBlend_GLtoGRC_Alpha( dfactor );
	}
	else
	{
		BP3D_SetSrcBlend( pContext->hP3d, BP3D_Blend_One );
		BP3D_SetDstBlend( pContext->hP3d, BP3D_Blend_Zero );

		pContext->eGRCSrcBlendColor = BGRC_Blend_Source_eOne;
		pContext->eGRCDstBlendColor = BGRC_Blend_Source_eZero;
		pContext->eGRCSrcBlendAlpha = BGRC_Blend_Source_eOne;
		pContext->eGRCDstBlendAlpha = BGRC_Blend_Source_eZero;
	}

	pContext->eSrcBlendFactor = sfactor;
	pContext->eDstBlendFactor = dfactor;
}

/* End of File */
