/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: egl_private.h $
 * $brcm_Revision: Hydra_Software_Devel/27 $
 * $brcm_Date: 4/18/12 1:11p $
 *
 * Module Description: EGL Private Header File.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/egl_private.h $
 * 
 * Hydra_Software_Devel/27   4/18/12 1:11p nissen
 * SW7125-1283: Fixed depth surface support using nexus planes.
 * 
 * Hydra_Software_Devel/26   1/17/12 3:29p nissen
 * SW7435-30: Added support for using planes instead of BSUR surfaces.
 * 
 * Hydra_Software_Devel/25   8/16/10 11:03a nissen
 * SW7400-2378: Changed version define.
 * 
 * Hydra_Software_Devel/24   8/6/10 1:33p nissen
 * SW7400-2378: Added gl.h include.
 * 
 * Hydra_Software_Devel/23   7/29/10 6:20p nissen
 * SW7405-4657: Added support for using color array with lighting enabled.
 * 
 * Hydra_Software_Devel/22   3/12/10 2:45p nissen
 * SW7405-2856: Added ucRop3 in context structure.
 * 
 * Hydra_Software_Devel/21   8/12/09 1:48p nissen
 * PR 56587: Using ReadSurface as a background source.
 * 
 * Hydra_Software_Devel/20   7/6/09 4:37p nissen
 * PR 56587: Added limited support for glColorMask.
 * 
 * Hydra_Software_Devel/19   7/4/09 5:14p nissen
 * PR 56587: Removed LogicOp related stuff.
 * 
 * Hydra_Software_Devel/18   2/27/09 3:11p nissen
 * PR 42507: Added support for dual display.
 * 
 * Hydra_Software_Devel/17   9/23/08 6:10p nissen
 * PR 46638: Optimized rectangle blits.
 * 
 * Hydra_Software_Devel/16   9/8/08 8:57p nissen
 * PR 46638: Added support for waiting for blits.
 * 
 * Hydra_Software_Devel/15   9/8/08 12:23p nissen
 * PR 46638: Added support for screen aligned rectangle and drawing it
 * with the blitter.
 * 
 * Hydra_Software_Devel/14   6/16/08 7:36p nissen
 * PR 42507: Added support for using P3D contexts.
 * 
 * Hydra_Software_Devel/13   11/15/07 8:02p nissen
 * PR 35855: Added support for new native model.
 * 
 * Hydra_Software_Devel/12   11/10/06 12:37p nissen
 * PR 20908: Added support for offscreen back buffer (quadruple
 * buffering).
 * 
 * Hydra_Software_Devel/11   9/20/06 1:39p nissen
 * PR 22858: Fixed fog.
 * 
 * Hydra_Software_Devel/10   6/12/06 10:45a nissen
 * PR 21896: Added support for timing various operations.
 * 
 * Hydra_Software_Devel/9   11/11/05 3:42p nissen
 * PR 17423: Added support for using Settop API.
 * 
 * Hydra_Software_Devel/8   10/26/05 2:33p nissen
 * PR 17656: Fixed page flipping problem.
 * 
 * Hydra_Software_Devel/7   10/17/05 8:40p nissen
 * PR 17656: Added generate mipmap and texture related fields.
 * 
 * Hydra_Software_Devel/6   3/9/05 3:04p nissen
 * PR 12818: Fixed support for triple buffer flipping.
 * 
 * Hydra_Software_Devel/5   9/17/04 4:03p nissen
 * PR 9132: Fixed logic op rendering.
 * 
 * Hydra_Software_Devel/4   9/7/04 3:30p nissen
 * PR 9132: Added buffer swapping and logic op rendering support.
 * 
 * Hydra_Software_Devel/3   8/10/04 3:27p nissen
 * PR 9132: Added function definition.
 * 
 * Hydra_Software_Devel/2   7/28/04 12:36p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#ifndef EGL_PRIVATE_H__
#define EGL_PRIVATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "gl.h"
#include "gl_private.h"

/***************************************************************************/
#define EGL_P_MAJOR_VERSION         1
#define EGL_P_MINOR_VERSION         0
#define EGL_P_VENDOR_STRING         "Broadcom"
#define EGL_P_VERSION_STRING        "OpenGL ES-CM 1.0"
#define EGL_P_EXTENSIONS_STRING     "None"

/***************************************************************************/
#define EGL_P_RENDER_SIZE_MAX       720
#define EGL_P_DISPLAY_FREQUENCY     60

/***************************************************************************/
#define EGL_P_CONFIG_ID(config)    ((((EGLint) (config)) >> 24) & 0xFF)
#define EGL_P_DEPTH_SIZE(config)   ((((EGLint) (config)) >> 16) & 0xFF)
#define EGL_P_ALPHA_SIZE(config)   ((((EGLint) (config)) >> 12) & 0xF)
#define EGL_P_RED_SIZE(config)     ((((EGLint) (config)) >>  8) & 0xF)
#define EGL_P_GREEN_SIZE(config)   ((((EGLint) (config)) >>  4) & 0xF)
#define EGL_P_BLUE_SIZE(config)    ((((EGLint) (config)) >>  0) & 0xF)
#define EGL_P_BUFFER_SIZE(config)  (EGL_P_ALPHA_SIZE(config) + EGL_P_RED_SIZE(config) + EGL_P_GREEN_SIZE(config) + EGL_P_BLUE_SIZE(config))

/***************************************************************************/
#define EGL_P_SET_ERROR( err ) \
{ \
	EGL_P_Display *pDisplay = EGL_P_GetDisplay(); \
	if( pDisplay->iError == EGL_SUCCESS ) \
		pDisplay->iError = (err); \
}

/***************************************************************************/
typedef struct
{
	NativeWindow nativeWindow;
	NativePixmap nativePixmap;

	BSUR_Surface_Handle hPrevRenderSurface;
	BSUR_Surface_Handle hRenderSurface;
	BSUR_Surface_Handle hDepthSurface;
	BPXL_Format eFormat;

	EGLConfig config;
	GLuint uiFormat;
	GLuint uiOffset;
	void *pvMemory;
	EGLint iPitch;
	EGLint iWidth;
	EGLint iHeight;
	EGLint iLargestBuffer;
	EGLBoolean bColorOwned;

	GLuint uiDepthOffset;
	void *pvDepthMemory;
	EGLint iDepthPitch;
}
EGL_P_Surface;

/***************************************************************************/
typedef struct
{
	EGLBoolean bRectangleBlit2D;
	EGLBoolean bBlitBusy;
	EGLBoolean bInitialized;
	EGLConfig config;
	GLenum eError;

	GL_P_Texture *apTextures[GL_P_TEXTURE_MAX];
	GLuint uiFreeTexIndex;
	GLuint uiFreeTexCount;

	GL_P_Texture *pTexture;
	GLboolean bTexture2D;
	GLenum eTexEnvMode;
	GLuint ulTextureColor;
	int32_t aiTextureMatrix[20];

	GL_P_Texture *pVideoTexture;

	GLboolean bAlphaTest;
	GLenum eAlphaFunc;
	GLclampf fAlphaRef;

	GLboolean bBlend;
	GLenum eSrcBlendFactor;
	GLenum eDstBlendFactor;
	BGRC_Blend_Source eGRCSrcBlendColor;
	BGRC_Blend_Source eGRCDstBlendColor;
	BGRC_Blend_Source eGRCSrcBlendAlpha;
	BGRC_Blend_Source eGRCDstBlendAlpha;

	GLboolean bColorMaterial;
	GL_P_Material Material;
	GL_P_Color AmbientColor;

	GLboolean bCullFace;
	GLenum eCullMode;
	GLenum eFrontFace;

	GLboolean bDither;

	GLboolean bDepthTest;
	GLenum eDepthFunc;
	GLboolean bDepthMask;
	GLclampf fDepthNear;
	GLclampf fDepthFar;
	GLfloat fInvDepthRange;
	GLfloat fHalfDepth;
	GLfloat fDepthFactor;
	GLfloat fDepthUnits;
	GLfloat fDepthOffset;

	GLboolean bFog;
	GLenum eFogMode;
	GLclampf fFogDensity;
	GLclampf fFogStart;
	GLclampf fFogEnd;
	GLclampf fTransFogStart;
	GLclampf fTransFogEnd;
	GLuint ulFogColor;

	GLboolean bLogicOp;
	GLenum eLogicOp;
	uint8_t ucRop3;

	GLboolean bMaskRed;
	GLboolean bMaskGreen;
	GLboolean bMaskBlue;
	GLboolean bMaskAlpha;

	GLboolean bLighting;
	GLboolean bLightModelTwoSide;
	GLenum eShadeModel;
	GLboolean bLight[GL_P_LIGHT_MAX];
	GL_P_Light aLights[GL_P_LIGHT_MAX];

	GLbitfield bfClearMask;
	GLfloat fClearDepth;
	GLuint ulClearColor;

	GLboolean bNormalize;
	GLboolean bRescaleNormal;
	GLboolean bPolygonOffsetFill;

	GL_P_Color Color;
	GL_P_Coord3f Normal;
	GL_P_TexCoord TexCoord;

	GLboolean bVertexArray;
	GL_P_Array VertexArray;

	GLboolean bNormalArray;
	GL_P_Array NormalArray;

	GLboolean bColorArray;
	GL_P_Array ColorArray;

	GLboolean bTexCoordArray;
	GL_P_Array TexCoordArray;

	GLuint ulVertexOutCode;
	GLuint ulVertexRejectCode;

	GLenum eMatrixMode;
	GL_P_Matrix *pMatrix;
	GL_P_Matrix CombinedMatrix;
	GL_P_Matrix NormalMatrix;
	GL_P_MatrixStack *pMatrixStack;
	GL_P_MatrixStack ModelViewMatrixStack;
	GL_P_MatrixStack ProjectionMatrixStack;
	GL_P_MatrixStack TextureMatrixStack;
	GLboolean bNormalMatrixDirty;

	GLboolean bScissorTest;
	GLboolean bScissorTestChanged;
	GL_P_Rect Scissor;
	GL_P_Viewport Viewport;
	GLint iViewportX;
	GLint iViewportY;
	GLint iViewportWidth;
	GLint iViewportHeight;
	GLint iViewportOffsetX;
	GLint iViewportOffsetY;

	BP3D_Vertex *pVertices;
	GLsizei iVertexCount;

	BP3D_Context hContext;

	GLboolean bStoredTexCoordArray;
	GLboolean bStoredNormalArray;
	GLboolean bStoredColorArray;
	GLboolean bStoredAlphaTest;
	GLboolean bStoredBlend;
	GLboolean bStoredLogicOp;
	GLboolean bStoredDither;
	GLboolean bStoredFog;
	GLboolean bStoredLighting;
	GLboolean bStoredTexture2D;
	GLboolean bStoredDepthMask;
	GLenum eStoredSrcBlendFactor;
	GLenum eStoredDstBlendFactor;
	GLenum eStoredTexEnvMode;
	GL_P_Color StoredColor;
}
EGL_P_Context;

/***************************************************************************/
typedef struct
{
	NativeDisplay nativeDisplay;

	BMEM_Handle hMem;
	BP3D_Handle hP3d;

	EGLint iError;
	EGL_P_Context *pContext;
	EGL_P_Surface *pDrawSurface;
	EGL_P_Surface *pReadSurface;
	GLboolean bGenerateMipmaps;
}
EGL_P_Display;

/***************************************************************************/
EGL_P_Display *EGL_P_ValidateDisplay( EGLDisplay dpy );
void EGL_P_SetDisplay( EGLDisplay dpy );
EGL_P_Display *EGL_P_GetDisplay( void );
EGL_P_Context *EGL_P_GetContext( void );
EGLConfig EGL_P_GetConfig( EGLint iID );
BPXL_Format EGL_P_ConvertFormat_EGLtoP3D( EGLConfig config );
void EGL_P_Render( EGL_P_Display *pDisplay, EGL_P_Context *pContext, EGLBoolean bFlush );
void EGL_P_InitiateRender( EGL_P_Display *pDisplay, EGL_P_Context *pContext, EGLBoolean bFlush );
void EGL_P_SetDefaultState( EGL_P_Display *pDisplay, EGL_P_Context *pContext, EGL_P_Surface *pDrawSurface, EGL_P_Surface *pReadSurface );
void EGL_P_SetContextState( EGL_P_Display *pDisplay, EGL_P_Context *pContext, EGL_P_Surface *pDrawSurface, EGL_P_Surface *pReadSurface );

void GL_P_SetEnable( EGL_P_Display *pDisplay, EGL_P_Context *pContext, GLenum cap, GLint bEnable );
void GL_P_SetClientStateEnable( EGL_P_Display *pDisplay, EGL_P_Context *pContext, GLenum array, GLint bEnable );

void GL_P_SetColorMaskState_Pass0( EGL_P_Context *pContext );
void GL_P_SetColorMaskState_Pass1( EGL_P_Context *pContext );
void GL_P_SetColorMaskState_Done( EGL_P_Context *pContext );

void GL_P_TransformVertexArray( EGL_P_Context *pContext, GLint iFirst, GLsizei iCount, GLenum eIndexType, const GLvoid *pvIndices );
void GL_P_TransformEyeCoordArray( EGL_P_Context *pContext, GLint iFirst, GLsizei iCount, GLenum eIndexType, const GLvoid *pvIndices );
void GL_P_TransformNormalArray( EGL_P_Context *pContext, GLint iFirst, GLsizei iCount, GLenum eIndexType, const GLvoid *pvIndices );
void GL_P_TransformTexCoordArray( EGL_P_Context *pContext, GLint iFirst, GLsizei iCount, GLenum eIndexType, const GLvoid *pvIndices );
void GL_P_CopyColorArray( EGL_P_Context *pContext, GLint iFirst, GLsizei iCount, GLenum eIndexType, const GLvoid *pvIndices );
void GL_P_TransformColorArray( EGL_P_Context *pContext, GLint iFirst, GLsizei iCount, GLenum eIndexType, const GLvoid *pvIndices );
void GL_P_TransformVertex( EGL_P_Context *pContext, BP3D_Vertex *pVOut, BP3D_Vertex *pVIn );
void GL_P_TransformVertices( EGL_P_Context *pContext, GLsizei iCount );
void GL_P_SetVertexFogIndex( EGL_P_Context *pContext, BP3D_Vertex *pVertexIn, BP3D_Vertex *pVertexOut );

void GL_P_RenderPoint( EGL_P_Display *pDisplay, EGL_P_Context *pContext, BP3D_Vertex *pV0 );
void GL_P_ClipAndRenderPoints( EGL_P_Display *pDisplay, EGL_P_Context *pContext, GLsizei iVertexCount );
void GL_P_RenderLine( EGL_P_Display *pDisplay, EGL_P_Context *pContext, BP3D_Vertex *pV0, BP3D_Vertex *pV1 );
void GL_P_ClipAndRenderLine( EGL_P_Display *pDisplay, EGL_P_Context *pContext, BP3D_Vertex *pV0, BP3D_Vertex *pV1 );
void GL_P_ClipAndRenderLineStrip( EGL_P_Display *pDisplay, EGL_P_Context *pContext, GLsizei iVertexCount );
void GL_P_ClipAndRenderLineLoop( EGL_P_Display *pDisplay, EGL_P_Context *pContext, GLsizei iVertexCount );
void GL_P_ClipAndRenderLines( EGL_P_Display *pDisplay, EGL_P_Context *pContext, GLsizei iVertexCount );
void GL_P_RenderTriangle( EGL_P_Display *pDisplay, EGL_P_Context *pContext, BP3D_Vertex *pV0, BP3D_Vertex *pV1, BP3D_Vertex *pV2 );
void GL_P_ClipAndRenderTriangle( EGL_P_Display *pDisplay, EGL_P_Context *pContext, BP3D_Vertex *pV0, BP3D_Vertex *pV1, BP3D_Vertex *pV2 );
void GL_P_ClipAndRenderTriangleStrip( EGL_P_Display *pDisplay, EGL_P_Context *pContext, GLsizei iVertexCount );
void GL_P_ClipAndRenderTriangleFan( EGL_P_Display *pDisplay, EGL_P_Context *pContext, GLsizei iVertexCount );
void GL_P_ClipAndRenderTriangles( EGL_P_Display *pDisplay, EGL_P_Context *pContext, GLsizei iVertexCount );
void GL_P_RenderRectangle( EGL_P_Display *pDisplay, EGL_P_Context *pContext, BP3D_Vertex *pV0, BP3D_Vertex *pV1 );
void GL_P_ClipAndRenderRectangle( EGL_P_Display *pDisplay, EGL_P_Context *pContext, BP3D_Vertex *pV0, BP3D_Vertex *pV1 );
void GL_P_ClipAndRenderRectangles( EGL_P_Display *pDisplay, EGL_P_Context *pContext, GLsizei iVertexCount );

GLboolean GL_P_LoadTexture( EGL_P_Display *pDisplay, EGL_P_Context *pContext, GL_P_Texture *pTexture );
void GL_P_CopySceneToTexture( EGL_P_Display *pDisplay, EGL_P_Context *pContext, GL_P_MipmapLevel *pLevel, GLint internalformat, GLint iTexX, GLint iTexY, GLint iSceneX, GLint iSceneY, GLsizei iWidth, GLsizei iHeight );
GLenum GL_P_GetSceneTextureType( EGL_P_Display *pDisplay, GLint internalformat );
void GL_P_SetViewport( EGL_P_Display *pDisplay, EGL_P_Context *pContext );
void GL_P_CalculateLightValues( EGL_P_Context *pContext, GLsizei iCount );
void GL_P_DrawVertices( EGL_P_Display *pDisplay, EGL_P_Context *pContext, GLenum mode, GLint iFirst, GLsizei iCount, GLenum eIndexType, const GLvoid *pvIndices );

/***************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* #ifndef EGL_PRIVATE_H__ */

/* end of file */
