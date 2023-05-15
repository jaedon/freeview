/***************************************************************************
 *     Copyright (c) 2004-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_color.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 7/6/09 4:43p $
 *
 * Module Description: Color entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_color.c $
 * 
 * Hydra_Software_Devel/3   7/6/09 4:43p nissen
 * PR 56587: Added limited support for glColorMask.
 * 
 * Hydra_Software_Devel/2   7/28/04 12:59p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
void GL_P_SetColorMaskState_Pass0(
	EGL_P_Context *pContext )
{
	/* setup state to write zero to dest channels that are masked */
	pContext->bStoredTexCoordArray = pContext->bTexCoordArray;
	pContext->bStoredNormalArray = pContext->bNormalArray;
	pContext->bStoredColorArray = pContext->bColorArray;
	pContext->bStoredAlphaTest = pContext->bAlphaTest;
	pContext->bStoredBlend = pContext->bBlend;
	pContext->bStoredLogicOp = pContext->bLogicOp;
	pContext->bStoredDither = pContext->bDither;
	pContext->bStoredFog = pContext->bFog;
	pContext->bStoredLighting = pContext->bLighting;
	pContext->bStoredTexture2D = pContext->bTexture2D;
	pContext->bStoredDepthMask = pContext->bDepthMask;
	pContext->eStoredSrcBlendFactor = pContext->eSrcBlendFactor;
	pContext->eStoredDstBlendFactor = pContext->eDstBlendFactor;
	pContext->eStoredTexEnvMode = pContext->eTexEnvMode;
	pContext->StoredColor = pContext->Color;

	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );

	glDisable( GL_ALPHA_TEST );
	glDisable( GL_COLOR_LOGIC_OP );
	glDisable( GL_DITHER );
	glDisable( GL_FOG );
	glDisable( GL_LIGHTING );
	glDisable( GL_TEXTURE_2D );

	glDepthMask( GL_FALSE );

	glEnable( GL_BLEND );
	glBlendFunc( GL_DST_COLOR, GL_ZERO );

	glColor4f( pContext->bMaskRed ? 0 : 1, pContext->bMaskGreen ? 0 : 1, pContext->bMaskBlue ? 0 : 1, pContext->bMaskAlpha ? 0 : 1 );
}

/***************************************************************************/
void GL_P_SetColorMaskState_Pass1(
	EGL_P_Context *pContext )
{
	/* setup state to write source into zero'ed dest channels */
	if( pContext->bStoredTexCoordArray )
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	if( pContext->bStoredNormalArray )
		glEnableClientState( GL_NORMAL_ARRAY );

	if( pContext->bStoredAlphaTest )
		glEnable( GL_ALPHA_TEST );
	if( pContext->bStoredLogicOp )
		glEnable( GL_COLOR_LOGIC_OP );
	if( pContext->bStoredDither )
		glEnable( GL_DITHER );
	if( pContext->bStoredFog )
		glEnable( GL_FOG );
	if( pContext->bStoredLighting )
		glEnable( GL_LIGHTING );
	if( pContext->bStoredTexture2D )
		glEnable( GL_TEXTURE_2D );

	if( pContext->bStoredDepthMask )
		glDepthMask( GL_TRUE );

	glBlendFunc( GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	if( pContext->eStoredTexEnvMode == GL_MODULATE )
	{
		glColor4f( pContext->StoredColor.fR * pContext->bMaskRed, pContext->StoredColor.fG * pContext->bMaskGreen, 
			pContext->StoredColor.fB * pContext->bMaskBlue, pContext->StoredColor.fA * pContext->bMaskAlpha );
	}
	else
	{
		glColor4f( pContext->bMaskRed, pContext->bMaskGreen, pContext->bMaskBlue, pContext->bMaskAlpha );
	}
}

/***************************************************************************/
void GL_P_SetColorMaskState_Done(
	EGL_P_Context *pContext )
{
	/* restore original state */
	if( pContext->bStoredColorArray )
		glEnableClientState( GL_COLOR_ARRAY );

	glBlendFunc( pContext->eStoredSrcBlendFactor, pContext->eStoredDstBlendFactor );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, pContext->eStoredTexEnvMode );
	glColor4f( pContext->StoredColor.fR, pContext->StoredColor.fG, pContext->StoredColor.fB , pContext->StoredColor.fA );
}

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glColor4f( 
	GLfloat red, 
	GLfloat green, 
	GLfloat blue, 
	GLfloat alpha )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	pContext->Color.fR = red;
	pContext->Color.fG = green;
	pContext->Color.fB = blue;
	pContext->Color.fA = alpha;
}

/***************************************************************************/
GLAPI void APIENTRY glColor4x( 
	GLfixed red, 
	GLfixed green, 
	GLfixed blue, 
	GLfixed alpha )
{
	EGL_P_Context *pContext = EGL_P_GetContext();

	pContext->Color.fR = GL_P_CONVERT_XtoF(red);
	pContext->Color.fG = GL_P_CONVERT_XtoF(green);
	pContext->Color.fB = GL_P_CONVERT_XtoF(blue);
	pContext->Color.fA = GL_P_CONVERT_XtoF(alpha);
}

/***************************************************************************/
GLAPI void APIENTRY glColorMask( 
	GLboolean red, 
	GLboolean green, 
	GLboolean blue, 
	GLboolean alpha )
{
	EGL_P_Context *pContext = EGL_P_GetContext();
	
	pContext->bMaskRed = red;
	pContext->bMaskGreen = green;
	pContext->bMaskBlue = blue;
	pContext->bMaskAlpha = alpha;
}

/* End of File */
