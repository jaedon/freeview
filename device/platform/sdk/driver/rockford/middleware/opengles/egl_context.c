/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: egl_context.c $
 * $brcm_Revision: Hydra_Software_Devel/21 $
 * $brcm_Date: 4/18/12 1:06p $
 *
 * Module Description: Context entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/egl_context.c $
 * 
 * Hydra_Software_Devel/21   4/18/12 1:06p nissen
 * SW7125-1283: Fixed depth surface support using nexus planes.
 * 
 * Hydra_Software_Devel/20   4/18/12 12:29p nissen
 * SW7125-1283: Fixed typo causing build error.
 * 
 * Hydra_Software_Devel/19   4/17/12 5:36p nissen
 * SW7125-1283: Fixed problem with setting depth buffer.
 * 
 * Hydra_Software_Devel/18   1/17/12 3:24p nissen
 * SW7435-30: Added support for using planes instead of BSUR surfaces.
 * 
 * Hydra_Software_Devel/17   8/14/10 2:56p nissen
 * SW7400-2378: Fixed problem with destroying P3D context.
 * 
 * Hydra_Software_Devel/16   8/12/09 1:43p nissen
 * PR 56587: Using ReadSurface as a background source.
 * 
 * Hydra_Software_Devel/15   2/27/09 3:00p nissen
 * PR 42507: Added support for dual display.
 * 
 * Hydra_Software_Devel/14   10/22/08 9:48p nissen
 * PR 41194: Enabled dither by default.
 * 
 * Hydra_Software_Devel/13   9/8/08 12:18p nissen
 * PR 46638: Added support for screen aligned rectangle and drawing it
 * with the blitter.
 * 
 * Hydra_Software_Devel/12   6/16/08 7:32p nissen
 * PR 42507: Added support for using P3D contexts.
 * 
 * Hydra_Software_Devel/11   11/15/07 7:55p nissen
 * PR 35855: Added support for new native model.
 * 
 * Hydra_Software_Devel/10   4/27/07 3:09p nissen
 * PR 30062: Removed LogicOp render.
 * 
 * Hydra_Software_Devel/9   6/12/06 10:38a nissen
 * PR 21896: Added support for timing various operations.
 * 
 * Hydra_Software_Devel/8   6/7/06 10:49p nissen
 * PR 20908: Added support for triple buffering to Settop API verision.
 * 
 * Hydra_Software_Devel/7   11/11/05 3:36p nissen
 * PR 17423: Added support for using Settop API.
 * 
 * Hydra_Software_Devel/6   3/9/05 3:07p nissen
 * PR 12818: Fixed support for triple buffer flipping.
 * 
 * Hydra_Software_Devel/5   9/17/04 4:07p nissen
 * PR 9132: Fixed logic op rendering.
 * 
 * Hydra_Software_Devel/4   9/7/04 3:22p nissen
 * PR 9132: Added buffer swapping and logic op rendering support.
 * 
 * Hydra_Software_Devel/3   8/10/04 3:37p nissen
 * PR 9132: Added code to check if there is a context before deleting
 * default texture.
 * 
 * Hydra_Software_Devel/2   7/28/04 12:24p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
void EGL_P_Render( 
	EGL_P_Display *pDisplay, 
	EGL_P_Context *pContext, 
	EGLBoolean bFlush )
{
	BSTD_UNUSED( pContext );

	if( bFlush )
		BP3D_IssueRenderAndWait( pDisplay->hP3d );
	else
		BP3D_IssueRender( pDisplay->hP3d, 0, 0, 0 );
}

/***************************************************************************/
void EGL_P_InitiateRender( 
	EGL_P_Display *pDisplay, 
	EGL_P_Context *pContext,
	EGLBoolean bFlush )
{
	if( pDisplay->pDrawSurface == NULL )
		return;

	EGL_P_Render( pDisplay, pContext, bFlush );
	pContext->bfClearMask = 0;
}

/***************************************************************************/
void EGL_P_SetDefaultState( 
	EGL_P_Display *pDisplay, 
	EGL_P_Context *pContext, 
	EGL_P_Surface *pDrawSurface,
	EGL_P_Surface *pReadSurface )
{
	GLfloat afZeroColor[4] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat afFullColor[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat afFullAlpha[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat afAmbColor[4]  = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat afDifColor[4]  = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat afPosition[4]  = { 0.0, 0.0, 1.0, 0.0 };
	GLfloat afDirection[3] = { 0.0, 0.0,-1.0 };
	GLuint uiTextureID;
	GLint ii;

	pContext->ModelViewMatrixStack.uiTop = GL_P_MATRIX_STACK_DEPTH - 1;
	pContext->ProjectionMatrixStack.uiTop = GL_P_MATRIX_STACK_DEPTH - 1;
	pContext->TextureMatrixStack.uiTop = GL_P_MATRIX_STACK_DEPTH - 1;

	glActiveTexture( GL_TEXTURE0 );
	glAlphaFunc( GL_ALWAYS, 0 );
	glBlendFunc( GL_ONE, GL_ZERO );
	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glClearDepthf( 1.0 );
	glClearStencil( 0 );
	glColor4f( 1.0, 1.0, 1.0, 1.0 );
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glCullFace( GL_BACK );
	glDepthFunc( GL_LESS );
	glDepthMask( GL_TRUE );
	glDepthRangef( 0.0, 1.0 );
	glFogf( GL_FOG_MODE, GL_EXP );
	glFogf( GL_FOG_DENSITY, 1.0 );
	glFogf( GL_FOG_START, 0.0 );
	glFogf( GL_FOG_END, 1.0 );
	glFogfv( GL_FOG_COLOR, afZeroColor );
	glFrontFace( GL_CCW );
	glLightModelf( GL_LIGHT_MODEL_TWO_SIDE, 0 );
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, afAmbColor );
	glLineWidth( 1.0 );
	glLogicOp( GL_COPY );
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0.0 );
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, afAmbColor );
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, afDifColor );
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, afFullAlpha );
	glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, afFullAlpha );
	glMultiTexCoord4f( GL_TEXTURE0, 0.0, 0.0, 0.0, 1.0 );
	glNormal3f( 0.0, 0.0, 1.0 );
	glPointSize( 1.0 );
	glPolygonOffset( 0.0, 0.0 );
	glSampleCoverage( 1.0, GL_FALSE );
	glShadeModel( GL_SMOOTH );
	glStencilFunc( GL_ALWAYS, 0, 0xFFFFFFFF );
	glStencilMask( 0xFFFFFFFF );
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
	glViewport( 0, 0, pDrawSurface->iWidth, pDrawSurface->iHeight );

	glGenTextures( 1, &uiTextureID );
	glBindTexture( GL_TEXTURE_2D, uiTextureID );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, afZeroColor );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	glMatrixMode( GL_TEXTURE );
	glLoadIdentity();
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	for( ii = 0; ii < GL_P_LIGHT_MAX; ++ii )
	{
		glDisable( GL_LIGHT0 + ii );
		glLightf( GL_LIGHT0 + ii, GL_SPOT_EXPONENT, 0.0 );
		glLightf( GL_LIGHT0 + ii, GL_SPOT_CUTOFF, 180.0 );
		glLightf( GL_LIGHT0 + ii, GL_CONSTANT_ATTENUATION, 1.0 );
		glLightf( GL_LIGHT0 + ii, GL_LINEAR_ATTENUATION, 0.0 );
		glLightf( GL_LIGHT0 + ii, GL_QUADRATIC_ATTENUATION, 0.0 );
		glLightfv( GL_LIGHT0 + ii, GL_AMBIENT, afFullAlpha );
		glLightfv( GL_LIGHT0 + ii, GL_DIFFUSE, ii ? afZeroColor : afFullColor );
		glLightfv( GL_LIGHT0 + ii, GL_SPECULAR, ii ? afZeroColor : afFullColor );
		glLightfv( GL_LIGHT0 + ii, GL_POSITION, afPosition );
		glLightfv( GL_LIGHT0 + ii, GL_SPOT_DIRECTION, afDirection );
	}

	glDisableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );

	glDisable( GL_ALPHA_TEST );
	glDisable( GL_BLEND );
	glDisable( GL_COLOR_LOGIC_OP );
	glDisable( GL_COLOR_MATERIAL );
	glDisable( GL_CULL_FACE );
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_FOG );
	glDisable( GL_LIGHTING );
	glDisable( GL_LINE_SMOOTH );
	glDisable( GL_MULTISAMPLE );
	glDisable( GL_NORMALIZE );
	glDisable( GL_POINT_SMOOTH );
	glDisable( GL_POLYGON_OFFSET_FILL );
	glDisable( GL_RESCALE_NORMAL );
	glDisable( GL_SAMPLE_ALPHA_TO_COVERAGE );
	glDisable( GL_SAMPLE_ALPHA_TO_ONE );
	glDisable( GL_SAMPLE_COVERAGE );
	glDisable( GL_SCISSOR_TEST );
	glDisable( GL_STENCIL_TEST );
	glDisable( GL_TEXTURE_2D );

	glEnable( GL_DITHER );

#if 0
	BP3D_SetDstColorSurface( pDisplay->hP3d, pDrawSurface->hRenderSurface );
	BP3D_SetSrcColorSurface( pDisplay->hP3d, pReadSurface->hRenderSurface );
	BP3D_SetDstDepthSurface( pDisplay->hP3d, pDrawSurface->hDepthSurface );
	BP3D_SetSrcDepthSurface( pDisplay->hP3d, pDrawSurface->hDepthSurface );
#else
	BP3D_SetDstColorPlane( pDisplay->hP3d, pDrawSurface->uiFormat, pDrawSurface->uiOffset, pDrawSurface->iPitch );
	BP3D_SetSrcColorPlane( pDisplay->hP3d, pReadSurface->uiFormat, pReadSurface->uiOffset, pReadSurface->iPitch );
	BP3D_SetDstDepthPlane( pDisplay->hP3d, BPXL_eZ16, pDrawSurface->uiDepthOffset, pDrawSurface->iDepthPitch );
	BP3D_SetSrcDepthPlane( pDisplay->hP3d, BPXL_eZ16, pDrawSurface->uiDepthOffset, pDrawSurface->iDepthPitch );
#endif
}

/***************************************************************************/
void EGL_P_SetContextState( 
	EGL_P_Display *pDisplay, 
	EGL_P_Context *pContext, 
	EGL_P_Surface *pDrawSurface,
	EGL_P_Surface *pReadSurface )
{
#if 0
	BP3D_SetDstColorSurface( pDisplay->hP3d, pDrawSurface->hRenderSurface );
	BP3D_SetSrcColorSurface( pDisplay->hP3d, pReadSurface->hRenderSurface );
	BP3D_SetDstDepthSurface( pDisplay->hP3d, pDrawSurface->hDepthSurface );
	BP3D_SetSrcDepthSurface( pDisplay->hP3d, pDrawSurface->hDepthSurface );
#else
	BP3D_SetDstColorPlane( pDisplay->hP3d, pDrawSurface->uiFormat, pDrawSurface->uiOffset, pDrawSurface->iPitch );
	BP3D_SetSrcColorPlane( pDisplay->hP3d, pReadSurface->uiFormat, pReadSurface->uiOffset, pReadSurface->iPitch );
	BP3D_SetDstDepthPlane( pDisplay->hP3d, BPXL_eZ16, pDrawSurface->uiDepthOffset, pDrawSurface->iDepthPitch );
	BP3D_SetSrcDepthPlane( pDisplay->hP3d, BPXL_eZ16, pDrawSurface->uiDepthOffset, pDrawSurface->iDepthPitch );
#endif

	GL_P_SetEnable( pDisplay, pContext, GL_ALPHA_TEST, pContext->bAlphaTest );
	GL_P_SetEnable( pDisplay, pContext, GL_BLEND, pContext->bBlend );
	GL_P_SetEnable( pDisplay, pContext, GL_COLOR_MATERIAL, pContext->bColorMaterial );
	GL_P_SetEnable( pDisplay, pContext, GL_CULL_FACE, pContext->bCullFace );
	GL_P_SetEnable( pDisplay, pContext, GL_DEPTH_TEST, pContext->bDepthTest );
	GL_P_SetEnable( pDisplay, pContext, GL_DITHER, pContext->bDither );
	GL_P_SetEnable( pDisplay, pContext, GL_FOG, pContext->bFog );
	GL_P_SetEnable( pDisplay, pContext, GL_LIGHTING, pContext->bLighting );
	GL_P_SetEnable( pDisplay, pContext, GL_NORMALIZE, pContext->bNormalize );
	GL_P_SetEnable( pDisplay, pContext, GL_RESCALE_NORMAL, pContext->bRescaleNormal );
	GL_P_SetEnable( pDisplay, pContext, GL_SCISSOR_TEST, pContext->bScissorTest );
	GL_P_SetEnable( pDisplay, pContext, GL_TEXTURE_2D, pContext->bTexture2D );
}

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI EGLContext APIENTRY eglCreateContext( 
	EGLDisplay dpy, 
	EGLConfig config, 
	EGLContext share_list, 
	const EGLint *attrib_list )
{
	BERR_Code err;
	EGL_P_Context *pContext;

	EGL_P_Display *pDisplay = EGL_P_ValidateDisplay( dpy );
	if( pDisplay == NULL )
		return (EGLContext) EGL_NO_CONTEXT;

	BSTD_UNUSED( share_list );
	BSTD_UNUSED( attrib_list );

	pContext = BKNI_Malloc( sizeof (EGL_P_Context) );
	if( pContext == NULL )
	{
		EGL_P_SET_ERROR(EGL_BAD_ALLOC);
		return (EGLContext) EGL_NO_CONTEXT;
	}

	BKNI_Memset( pContext, 0, sizeof (EGL_P_Context) );

	err = BP3D_Context_Create( pDisplay->hP3d, &pContext->hContext );
	if( err != BERR_SUCCESS )
	{
		BKNI_Free( pContext );
		EGL_P_SET_ERROR(EGL_BAD_ALLOC);
		return (EGLContext) EGL_NO_CONTEXT;
	}

	pContext->config = config;
	pContext->uiFreeTexCount = GL_P_TEXTURE_MAX;
	pContext->ModelViewMatrixStack.uiTop = GL_P_MATRIX_STACK_DEPTH - 1;
	pContext->ProjectionMatrixStack.uiTop = GL_P_MATRIX_STACK_DEPTH - 1;
	pContext->TextureMatrixStack.uiTop = GL_P_MATRIX_STACK_DEPTH - 1;

	return (EGLContext) pContext;
}

/***************************************************************************/
GLAPI EGLBoolean APIENTRY eglDestroyContext( 
	EGLDisplay dpy, 
	EGLContext ctx )
{
	EGL_P_Context *pContext = (EGL_P_Context *) ctx;
	GLuint uiTextureID = 0;

	EGL_P_Display *pDisplay = EGL_P_ValidateDisplay( dpy );
	if( pDisplay == NULL )
		return EGL_FALSE;

	if( pContext->pVertices )
		BKNI_Free( pContext->pVertices );

	if( EGL_P_GetContext() )
		glDeleteTextures( 1, &uiTextureID );

	if( pContext->hContext )
		BP3D_Context_Destroy( pDisplay->hP3d, pContext->hContext );

	BKNI_Free( (EGL_P_Context *) ctx );

	return EGL_TRUE;
}

/***************************************************************************/
GLAPI EGLBoolean APIENTRY eglMakeCurrent( 
	EGLDisplay dpy, 
	EGLSurface draw, 
	EGLSurface read, 
	EGLContext ctx )
{
	EGL_P_Surface *pDrawSurface = (EGL_P_Surface *) draw;
	EGL_P_Surface *pReadSurface = (EGL_P_Surface *) read;
	EGL_P_Context *pContext = (EGL_P_Context *) ctx;

	EGL_P_Display *pDisplay = EGL_P_ValidateDisplay( dpy );
	if( pDisplay == NULL )
		return EGL_FALSE;

	if( (draw == EGL_NO_SURFACE) || (read == EGL_NO_SURFACE) || (ctx == EGL_NO_CONTEXT) )
	{
		if( (draw != EGL_NO_SURFACE) || (read != EGL_NO_SURFACE) || (ctx != EGL_NO_CONTEXT) )
		{
			EGL_P_SET_ERROR(EGL_BAD_MATCH);
			return EGL_FALSE;
		}
	}
	else
	{
		if( (pDrawSurface->config != pReadSurface->config) || 
			(pDrawSurface->config != pContext->config) )
		{
			EGL_P_SET_ERROR(EGL_BAD_MATCH);
			return EGL_FALSE;
		}
	}

#if 1
	pDisplay->pDrawSurface = pDrawSurface;
	pDisplay->pReadSurface = pReadSurface;
	pDisplay->pContext = pContext;

	EGL_P_SetDisplay( dpy );
	BP3D_Context_SetCurrent( pDisplay->hP3d, pContext ? pContext->hContext : 0 );

	if( pContext )
	{
		if( !pContext->bInitialized )
		{
			EGL_P_SetDefaultState( pDisplay, pContext, pDrawSurface, pReadSurface );
			pContext->bInitialized = GL_TRUE;
		}
	}
#else
	if( (pDisplay->pDrawSurface != pDrawSurface) ||
		(pDisplay->pReadSurface != pReadSurface) ||
		(pDisplay->pContext != pContext) )
	{
		if( pDisplay->pContext )
			EGL_P_InitiateRender( pDisplay, pContext, GL_TRUE );

		pDisplay->pDrawSurface = pDrawSurface;
		pDisplay->pReadSurface = pReadSurface;
		pDisplay->pContext = pContext;

		if( pContext )
		{
			if( pContext->bInitialized )
				EGL_P_SetContextState( pDisplay, pContext, pDrawSurface, pReadSurface );
			else
				EGL_P_SetDefaultState( pDisplay, pContext, pDrawSurface, pReadSurface );

			pContext->bInitialized = GL_TRUE;
		}
	}
#endif

	return EGL_TRUE;
}

/***************************************************************************/
GLAPI EGLBoolean APIENTRY eglQueryContext( 
	EGLDisplay dpy, 
	EGLContext ctx, 
	EGLint attribute, 
	EGLint *value )
{
	EGL_P_Context *pContext;

	EGL_P_Display *pDisplay = EGL_P_ValidateDisplay( dpy );
	if( pDisplay == NULL )
		return EGL_FALSE;

	pContext = (EGL_P_Context *) ctx;
	if( pContext == NULL )
	{
		EGL_P_SET_ERROR(EGL_BAD_CONTEXT);
		return EGL_FALSE;
	}

	if( attribute == EGL_CONFIG_ID )
		*value = (EGLint) pContext->config;

	return EGL_TRUE;
}

/***************************************************************************/
GLAPI void APIENTRY eglConfigureContext(
	EGLDisplay dpy, 
	EGLContext ctx, 
	EGLBoolean bRectangleBlit2D )
{
	EGL_P_Context *pContext;

	EGL_P_Display *pDisplay = EGL_P_ValidateDisplay( dpy );
	if( pDisplay == NULL )
		return;

	pContext = (EGL_P_Context *) ctx;
	if( pContext == NULL )
	{
		EGL_P_SET_ERROR(EGL_BAD_CONTEXT);
		return;
	}

	pContext->bRectangleBlit2D = bRectangleBlit2D;
}

/* End of File */
