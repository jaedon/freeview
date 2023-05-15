/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_alphablend.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 8/14/10 2:50p $
 *
 * Module Description: Alpha and Blend Func entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_alphablend.c $
 * 
 * Hydra_Software_Devel/5   8/14/10 2:50p nissen
 * SW7400-2378: Fixed fixed-point alpha reference.
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

#include "egl.h"
#include "egl_private.h"

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
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( pContext->bAlphaTest )
	{
		BP3D_Compare eCompare = GL_P_ConvertCompare_GLtoP3D( func );
		if( eCompare == (BP3D_Compare) -1 )
			return;

		BP3D_SetAlphaCompare( pDisplay->hP3d, eCompare );

		if( (func != GL_NEVER) && (func != GL_ALWAYS) )
			BP3D_SetAlphaReference( pDisplay->hP3d, GL_P_CONVERT_CHANNEL_FtoI(ref) );
	}
	else
	{
		BP3D_SetAlphaCompare( pDisplay->hP3d, BP3D_Compare_Always );
	}

	pContext->eAlphaFunc = func;
	pContext->fAlphaRef = ref;
}

/***************************************************************************/
GLAPI void APIENTRY glAlphaFuncx( 
	GLenum func, 
	GLclampx ref )
{
	glAlphaFunc( func, (GLclampf) GL_P_CONVERT_XtoF(ref) );
}

/***************************************************************************/
GLAPI void APIENTRY glBlendFunc( 
	GLenum sfactor, 
	GLenum dfactor )
{
	EGL_P_Display *pDisplay = EGL_P_GetDisplay();
	EGL_P_Context *pContext = EGL_P_GetContext();

	if( pContext->bBlend )
	{
		BP3D_Blend eSrcBlend = GL_P_ConvertBlend_GLtoP3D( sfactor );
		BP3D_Blend eDstBlend = GL_P_ConvertBlend_GLtoP3D( dfactor );
		if( (eSrcBlend == (BP3D_Blend) -1) || (eDstBlend == (BP3D_Blend) -1) )
			return;

		BP3D_SetSrcBlend( pDisplay->hP3d, eSrcBlend );
		BP3D_SetDstBlend( pDisplay->hP3d, eDstBlend );

		pContext->eGRCSrcBlendColor = GL_P_ConvertBlend_GLtoGRC( sfactor );
		pContext->eGRCDstBlendColor = GL_P_ConvertBlend_GLtoGRC( dfactor );
		pContext->eGRCSrcBlendAlpha = GL_P_ConvertBlend_GLtoGRC_Alpha( sfactor );
		pContext->eGRCDstBlendAlpha = GL_P_ConvertBlend_GLtoGRC_Alpha( dfactor );
	}
	else
	{
		BP3D_SetSrcBlend( pDisplay->hP3d, BP3D_Blend_One );
		BP3D_SetDstBlend( pDisplay->hP3d, BP3D_Blend_Zero );

		pContext->eGRCSrcBlendColor = BGRC_Blend_Source_eOne;
		pContext->eGRCDstBlendColor = BGRC_Blend_Source_eZero;
		pContext->eGRCSrcBlendAlpha = BGRC_Blend_Source_eOne;
		pContext->eGRCDstBlendAlpha = BGRC_Blend_Source_eZero;
	}

	pContext->eSrcBlendFactor = sfactor;
	pContext->eDstBlendFactor = dfactor;
}

/* End of File */
