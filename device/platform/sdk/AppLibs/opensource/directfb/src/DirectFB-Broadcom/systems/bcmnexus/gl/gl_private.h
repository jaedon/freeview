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
 * $brcm_Workfile: gl_private.h $
 * $brcm_Revision: DirectFB_1_4_14_Port/1 $
 * $brcm_Date: 7/28/11 11:22a $
 *
 * Module Description: GL Private Header File.
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.5/systems/bcmnexus/gl/gl_private.h $
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
 * Hydra_Software_Devel/12   9/8/08 12:58p nissen
 * PR 46638: Added support for screen aligned rectangle and drawing it
 * with the blitter.
 * 
 * Hydra_Software_Devel/11   8/19/08 9:53p nissen
 * PR 41194: Added support for swizzling textues on 3548/3556.
 * 
 * Hydra_Software_Devel/10   11/15/07 8:15p nissen
 * PR 35855: Added support for new native model.
 * 
 * Hydra_Software_Devel/9   12/8/06 2:47p nissen
 * PR 20908: Added support for copying only the dirty portion of a texture
 * to device memory.
 * 
 * Hydra_Software_Devel/8   12/7/06 11:53a nissen
 * PR 20908: Added support for using cached memory for internal textures
 * for faster copies.
 * 
 * Hydra_Software_Devel/7   6/5/06 12:19p nissen
 * PR 20908: Added support for texture swizzling.
 * 
 * Hydra_Software_Devel/6   11/11/05 4:04p nissen
 * PR 17423: Added video texture support.
 * 
 * Hydra_Software_Devel/5   10/17/05 9:14p nissen
 * PR 17656: Added support for generating mipmap lods and converting fixed
 * point colors. Moved texture parameter fields.
 * 
 * Hydra_Software_Devel/4   9/7/04 4:20p nissen
 * PR 9132: Added support for logic op rendering.
 * 
 * Hydra_Software_Devel/3   8/10/04 2:19p nissen
 * PR 9132: Added function definition.
 * 
 * Hydra_Software_Devel/2   7/28/04 2:00p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#ifndef GL_PRIVATE_H__
#define GL_PRIVATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <direct/thread.h>

#include <core/surface.h>
#include <core/surface_buffer.h>

#include "GLES/gl.h"

#include "math.h"
#include "bstd_defs.h"
#include "berr.h"
#include "bkni.h"
#include "bpxl.h"
#include "bsur.h"
#include "bgrc.h"
#include "bp3d.h"

#ifdef __GNUC__
#define UNUSED_ __attribute__ ((unused))
#else
#define UNUSED_
#endif 

/***************************************************************************/
#define GL_P_VENDOR                 "Broadcom"
#define GL_P_RENDERER               "PX3D"
#define GL_P_VERSION                "1.0"
#define GL_P_EXTENSIONS             ""
#define GL_P_RENDER_SIZE_MAX        2048
#define GL_P_TEXTURE_MAX            1024
#define GL_P_TEXTURE_SIZE_MAX       1024
#define GL_P_TEXTURE_LEVEL_MAX      11
#define GL_P_TEXTURE_UNIT_MAX       1
#define GL_P_VERTEX_MAX             65536
#define GL_P_MATRIX_STACK_DEPTH     16
#define GL_P_LIGHT_MAX              8
#define GL_P_FIXED_FRAC_BITS        16
#define GL_P_PI                     3.14159265

/***************************************************************************/

#define GL_TEXTURE_2D_SURFACE_DIRECTFB  0xfdfb

/***************************************************************************/
typedef struct
{
	GLint iMinX;
	GLint iMinY;
	GLint iMaxX;
	GLint iMaxY;
}
GL_P_Bounds;

/***************************************************************************/
typedef struct
{
	GLfloat fX;
	GLfloat fY;
	GLfloat fWidth;
	GLfloat fHeight;
}
GL_P_Rect;

/***************************************************************************/
typedef struct
{
	GLfloat fX;
	GLfloat fY;
	GLfloat fWidth;
	GLfloat fHeight;
	GLfloat fGBEpsilon;
	GLfloat fGBInvEpsilon;
	GLfloat fGBWidth;
	GLfloat fGBHeight;
} 
GL_P_Viewport;

/***************************************************************************/
typedef struct
{
	GLfloat a[16];
}
GL_P_Matrix;

/***************************************************************************/
typedef struct
{
	GL_P_Matrix aMatrices[GL_P_MATRIX_STACK_DEPTH];
	GLuint uiTop;
	GLboolean bDirty;
}
GL_P_MatrixStack;

/***************************************************************************/
typedef struct
{
	GLvoid *pvMemory;
	BSUR_Surface_Handle hSurface;
	GLsizei iOffset;
	GLsizei iWidth;
	GLsizei iHeight;
	GLsizei iSize;
}
GL_P_MipmapLevel;

/***************************************************************************/
typedef struct
{
     CoreSurface *pSurface;

	GL_P_MipmapLevel aLevels[GL_P_TEXTURE_LEVEL_MAX];
	BPXL_Format eFormat;
	BSUR_Surface_Handle hSurface;
	GLvoid *pvMemory;
	GLvoid *pvCached;
	GLenum eInternalFormat;
	GLsizei iSize;
	GLsizei iWidth;
	GLsizei iHeight;
	GLint iLevelCount;
	GLenum eTextureMinFilter;
	GLenum eTextureMagFilter;
	GLenum eTextureWrapS;
	GLenum eTextureWrapT;
	GLboolean bMipmap;
	GLboolean bGenerateMipmaps;
	GLboolean bSwizzle;
	GLboolean bDirty;
	GLboolean bAllDirty;
	GL_P_Bounds DirtyBounds;
}
GL_P_Texture;

/***************************************************************************/
typedef struct
{
	const GLvoid *pvMemory;
	GLenum eType;
	GLint iSize;
	GLsizei iStride;
}
GL_P_Array;

/***************************************************************************/
typedef struct
{
	GLfloat fR;
	GLfloat fG;
	GLfloat fB;
	GLfloat fA;
}
GL_P_Color;

/***************************************************************************/
typedef struct
{
	GLfloat fX;
	GLfloat fY;
	GLfloat fZ;
}
GL_P_Coord3f;

/***************************************************************************/
typedef struct
{
	GLfloat fX;
	GLfloat fY;
	GLfloat fZ;
	GLfloat fW;
}
GL_P_Coord4f;

/***************************************************************************/
typedef struct
{
	GLfloat fS;
	GLfloat fT;
}
GL_P_TexCoord;

/***************************************************************************/
typedef struct
{
	GL_P_Color AmbientColor;
	GL_P_Color DiffuseColor;
	GL_P_Color SpecularColor;
	GL_P_Color EmissionColor;
	GLfloat fSpecularExponent;
}
GL_P_Material;

/***************************************************************************/
typedef struct
{
	GLboolean bEnable;
	GL_P_Color AmbientColor;
	GL_P_Color DiffuseColor;
	GL_P_Color SpecularColor;
	GL_P_Coord4f Position;
	GL_P_Coord3f SpotDirection;
	GLfloat fSpotExponent;
	GLfloat fSpotCutoffAngle;
	GLfloat fSpotCutoffAngleCos;
	GLfloat fConstantAttenuation;
	GLfloat fLinearAttenuation;
	GLfloat fQuadraticAttenuation;
}
GL_P_Light;

/***************************************************************************/
#define GL_P_MIN( v0, v1 )    (((v0) < (v1)) ? (v0) : (v1))
#define GL_P_MAX( v0, v1 )    (((v0) > (v1)) ? (v0) : (v1))

/***************************************************************************/

extern pthread_once_t __P3DGL_context_once;
extern pthread_key_t  __P3DGL_context_tls;

static inline void
context_destructor( UNUSED_ void *ctx )
{
}

static inline void
context_once_handler( void )
{
     pthread_key_create( &__P3DGL_context_tls, context_destructor );
}

static inline void *
GL_P_GetContext( void )
{
     pthread_once( &__P3DGL_context_once, context_once_handler );

     return pthread_getspecific( __P3DGL_context_tls );
}

static inline void
GL_P_SetContext( void *ctx )
{
     pthread_once( &__P3DGL_context_once, context_once_handler );

     pthread_setspecific( __P3DGL_context_tls, ctx );
}

/***************************************************************************/
#define GL_P_SET_ERROR( err ) \
{ \
	p3dOpenGLContextData *pContext = GL_P_GetContext(); \
	if( pContext->eError == GL_NO_ERROR ) \
		pContext->eError = (err); \
}

/***************************************************************************/
#define GL_P_CONVERT_CHANNEL_XtoI( vv ) \
	((uint8_t) ((GL_P_MIN(GL_P_MAX((vv), 0), (1 << GL_P_FIXED_FRAC_BITS)) * 255) >> GL_P_FIXED_FRAC_BITS))

/***************************************************************************/
#define GL_P_CONVERT_COLOR_XtoI( aa, rr, gg, bb ) ( \
	(((uint32_t) GL_P_CONVERT_CHANNEL_XtoI(aa)) << 24) | \
	(((uint32_t) GL_P_CONVERT_CHANNEL_XtoI(rr)) << 16) | \
	(((uint32_t) GL_P_CONVERT_CHANNEL_XtoI(gg)) << 8) | \
	(((uint32_t) GL_P_CONVERT_CHANNEL_XtoI(bb))))

/***************************************************************************/
#define GL_P_CONVERT_CHANNEL_FtoI( vv ) \
	((uint8_t) (GL_P_MIN(GL_P_MAX((vv), 0.0), 1.0) * 255.0))

/***************************************************************************/
#define GL_P_CONVERT_COLOR_FtoI( aa, rr, gg, bb ) ( \
	(((uint32_t) GL_P_CONVERT_CHANNEL_FtoI(aa)) << 24) | \
	(((uint32_t) GL_P_CONVERT_CHANNEL_FtoI(rr)) << 16) | \
	(((uint32_t) GL_P_CONVERT_CHANNEL_FtoI(gg)) << 8) | \
	(((uint32_t) GL_P_CONVERT_CHANNEL_FtoI(bb))))

/***************************************************************************/
#define GL_P_CONVERT_COLOR_ItoI( aa, rr, gg, bb ) ( \
	(((uint32_t) (aa)) << 24) | (((uint32_t) (rr)) << 16) | \
	(((uint32_t) (gg)) <<  8) | (((uint32_t) (bb))))

/***************************************************************************/
#define GL_P_CONVERT_XtoF( vv ) \
	(((GLfloat) (vv)) / (1 << GL_P_FIXED_FRAC_BITS))

/***************************************************************************/
#define GL_P_CONVERT_FtoF( vv ) (vv)

/***************************************************************************/
#define GL_P_CONVERT_ItoX( vv ) ((vv) << GL_P_FIXED_FRAC_BITS)

/***************************************************************************/
#define GL_P_CONVERT_ItoF( vv ) (vv)

/***************************************************************************/
#define GL_P_CONVERT_ItoI( vv ) (vv)

/***************************************************************************/
#define GL_P_SET_LIGHT_COLOR( cc, rr, gg, bb, aa ) \
{ \
	(cc)->fR = (rr); \
	(cc)->fG = (gg); \
	(cc)->fB = (bb); \
	(cc)->fA = (aa); \
}

/***************************************************************************/
#define GL_P_CPY_LIGHT_COLOR( c0, c1 ) \
{ \
	(c0)->fR = (c1)->fR; \
	(c0)->fG = (c1)->fG; \
	(c0)->fB = (c1)->fB; \
	(c0)->fA = (c1)->fA; \
}

/***************************************************************************/
#define GL_P_ADD_LIGHT_COLOR( c0, c1, c2 ) \
{ \
	(c0)->fR = (c1)->fR + (c2)->fR; \
	(c0)->fG = (c1)->fG + (c2)->fG; \
	(c0)->fB = (c1)->fB + (c2)->fB; \
	(c0)->fA = (c1)->fA + (c2)->fA; \
}

/***************************************************************************/
#define GL_P_MUL_LIGHT_COLOR( c0, c1, c2 ) \
{ \
	(c0)->fR = (c1)->fR * (c2)->fR; \
	(c0)->fG = (c1)->fG * (c2)->fG; \
	(c0)->fB = (c1)->fB * (c2)->fB; \
	(c0)->fA = (c1)->fA * (c2)->fA; \
}

/***************************************************************************/
#define GL_P_MUL_LIGHT_CONSTANT( c0, c1, vv ) \
{ \
	(c0)->fR = (c1)->fR * (vv); \
	(c0)->fG = (c1)->fG * (vv); \
	(c0)->fB = (c1)->fB * (vv); \
	(c0)->fA = (c1)->fA * (vv); \
}

/***************************************************************************/
#define GL_P_OUTCODE_POS_X    0x01000000
#define GL_P_OUTCODE_NEG_X    0x02000000
#define GL_P_OUTCODE_POS_Y    0x04000000
#define GL_P_OUTCODE_NEG_Y    0x08000000
#define GL_P_OUTCODE_POS_Z    0x10000000
#define GL_P_OUTCODE_NEG_Z    0x20000000

/***************************************************************************/
#define GL_P_GENERATE_OUTCODE( pv, eps ) \
{ \
	GLfloat fPosW = ((pv)->fW < 0.0f) ? -(pv)->fW : (pv)->fW; \
	GLfloat fNegW = -fPosW; \
	(pv)->ulSpecular &= 0x00FFFFFF; \
	if( (pv)->fZ < fNegW ) \
		(pv)->ulSpecular |= GL_P_OUTCODE_NEG_Z; \
	if( (pv)->fZ > fPosW ) \
		(pv)->ulSpecular |= GL_P_OUTCODE_POS_Z; \
	fPosW *= (eps); \
	fNegW = -fPosW; \
	if( (pv)->fX < fNegW ) \
		(pv)->ulSpecular |= GL_P_OUTCODE_NEG_X; \
	if( (pv)->fX > fPosW ) \
		(pv)->ulSpecular |= GL_P_OUTCODE_POS_X; \
	if( (pv)->fY < fNegW ) \
		(pv)->ulSpecular |= GL_P_OUTCODE_NEG_Y; \
	if( (pv)->fY > fPosW ) \
		(pv)->ulSpecular |= GL_P_OUTCODE_POS_Y; \
}

/***************************************************************************/
#define GL_P_GET_OUTCODE( pv )   ((pv)->ulSpecular & 0xFF000000)

/***************************************************************************/
BP3D_Compare GL_P_ConvertCompare_GLtoP3D( GLenum func );
BP3D_Blend GL_P_ConvertBlend_GLtoP3D( GLenum factor );
BGRC_Blend_Source GL_P_ConvertBlend_GLtoGRC( GLenum factor );
BGRC_Blend_Source GL_P_ConvertBlend_GLtoGRC_Alpha( GLenum factor );
BPXL_Format GL_P_ConvertFormat_GLtoP3D( GLenum format, GLenum type );
GLubyte GL_P_ConvertLogicOp_GLtoP3D( GLenum opcode );
void GL_P_ConvertPixels_GLtoP3D( GLvoid *pvDst, GLvoid *pvSrc, GLenum format, GLenum type, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLsizei pitch );
GLboolean GL_P_SwizzleTexture( GLvoid *pvDst, GLvoid *pvSrc, GLint iWidth, GLint iHeight, GLint iBpp );
void GL_P_SetTextureDirty( GL_P_Texture *pTexture, GLint iX, GLint iY, GLint iWidth, GLint iHeight );

void GL_P_AddVector( GL_P_Coord3f *pVOut, GL_P_Coord3f *pVIn0, GL_P_Coord3f *pVIn1 );
GLfloat GL_P_NormalizeVector( GL_P_Coord3f *pV );
GLfloat GL_P_LightVector( GL_P_Coord3f *pVOut, GL_P_Coord3f *pVIn0, GL_P_Coord4f *pVIn1 );
GLfloat GL_P_LightDotProduct( GL_P_Coord3f *pVIn0, GL_P_Coord3f *pVIn1 );

void GL_P_MultiplyMatrix( GL_P_Matrix *pMatOut, GL_P_Matrix *pMatIn, GLfloat *pfInA );
void GL_P_SetCombinedMatrix( GL_P_Matrix *pCombinedMat, GL_P_Matrix *pProjectionMat, GL_P_Matrix *pModelViewMat );
void GL_P_SetNormalMatrix( GL_P_Matrix *pNormalMat, GL_P_Matrix *pModelViewMat );

/***************************************************************************/

typedef struct __P3D_OpenGLContextData p3dOpenGLContextData;

struct __P3D_OpenGLContextData {
     int            magic;

     CoreDFB               *core;

     CoreSurfaceBufferLock dst;
     CoreSurfaceBufferLock src;
     CoreSurfaceBufferLock depth;

     DFBDimension          size;
     DFBSurfacePixelFormat format;
     struct {
          int              alpha;
          int              red;
          int              green;
          int              blue;
          int              depth;
     } bits;

//     EGLDisplay     display;
     BMEM_Handle hMem;
     BP3D_Handle hP3d;

     GLint iError;
//     EGL_P_Surface *pDrawSurface;
//     EGL_P_Surface *pReadSurface;
     GLboolean bGenerateMipmaps;



//     EGLContext     context;
     GLboolean bRectangleBlit2D;
     GLboolean bBlitBusy;
     GLboolean bInitialized;
//     EGLConfig config;
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
     GLubyte ucRop3;
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
};

void GL_P_SetLogicOp( p3dOpenGLContextData *pContext, GLboolean bLogicOp, GLenum eLogicOp );
void GL_P_SetEnable( p3dOpenGLContextData *pContext, GLenum cap, GLint bEnable );
void GL_P_SetClientStateEnable( p3dOpenGLContextData *pContext, GLenum array, GLint bEnable );

void GL_P_TransformVertexArray( p3dOpenGLContextData *pContext, GLint iFirst, GLsizei iCount, GLenum eIndexType, const GLvoid *pvIndices );
void GL_P_TransformEyeCoordArray( p3dOpenGLContextData *pContext, GLint iFirst, GLsizei iCount, GLenum eIndexType, const GLvoid *pvIndices );
void GL_P_TransformNormalArray( p3dOpenGLContextData *pContext, GLint iFirst, GLsizei iCount, GLenum eIndexType, const GLvoid *pvIndices );
void GL_P_TransformTexCoordArray( p3dOpenGLContextData *pContext, GLint iFirst, GLsizei iCount, GLenum eIndexType, const GLvoid *pvIndices );
void GL_P_CopyColorArray( p3dOpenGLContextData *pContext, GLint iFirst, GLsizei iCount, GLenum eIndexType, const GLvoid *pvIndices );
void GL_P_TransformVertex( p3dOpenGLContextData *pContext, BP3D_Vertex *pVOut, BP3D_Vertex *pVIn );
void GL_P_TransformVertices( p3dOpenGLContextData *pContext, GLsizei iCount );
void GL_P_SetVertexFogIndex( p3dOpenGLContextData *pContext, BP3D_Vertex *pVertexIn, BP3D_Vertex *pVertexOut );

void GL_P_RenderPoint( p3dOpenGLContextData *pContext, BP3D_Vertex *pV0 );
void GL_P_ClipAndRenderPoints( p3dOpenGLContextData *pContext, GLsizei iVertexCount );
void GL_P_RenderLine( p3dOpenGLContextData *pContext, BP3D_Vertex *pV0, BP3D_Vertex *pV1 );
void GL_P_ClipAndRenderLine( p3dOpenGLContextData *pContext, BP3D_Vertex *pV0, BP3D_Vertex *pV1 );
void GL_P_ClipAndRenderLineStrip( p3dOpenGLContextData *pContext, GLsizei iVertexCount );
void GL_P_ClipAndRenderLineLoop( p3dOpenGLContextData *pContext, GLsizei iVertexCount );
void GL_P_ClipAndRenderLines( p3dOpenGLContextData *pContext, GLsizei iVertexCount );
void GL_P_RenderTriangle( p3dOpenGLContextData *pContext, BP3D_Vertex *pV0, BP3D_Vertex *pV1, BP3D_Vertex *pV2 );
void GL_P_ClipAndRenderTriangle( p3dOpenGLContextData *pContext, BP3D_Vertex *pV0, BP3D_Vertex *pV1, BP3D_Vertex *pV2 );
void GL_P_ClipAndRenderTriangleStrip( p3dOpenGLContextData *pContext, GLsizei iVertexCount );
void GL_P_ClipAndRenderTriangleFan( p3dOpenGLContextData *pContext, GLsizei iVertexCount );
void GL_P_ClipAndRenderTriangles( p3dOpenGLContextData *pContext, GLsizei iVertexCount );
void GL_P_RenderRectangle( p3dOpenGLContextData *pContext, BP3D_Vertex *pV0, BP3D_Vertex *pV1 );
void GL_P_ClipAndRenderRectangle( p3dOpenGLContextData *pContext, BP3D_Vertex *pV0, BP3D_Vertex *pV1 );
void GL_P_ClipAndRenderRectangles( p3dOpenGLContextData *pContext, GLsizei iVertexCount );

GLboolean GL_P_LoadTexture( p3dOpenGLContextData *pContext, GL_P_Texture *pTexture );
void GL_P_CopySceneToTexture( p3dOpenGLContextData *pContext, GL_P_MipmapLevel *pLevel, GLint internalformat, GLint iTexX, GLint iTexY, GLint iSceneX, GLint iSceneY, GLsizei iWidth, GLsizei iHeight );
GLenum GL_P_GetSceneTextureType( p3dOpenGLContextData *pContext, GLint internalformat );
void GL_P_SetViewport( p3dOpenGLContextData *pContext );
void GL_P_CalculateLightValues( p3dOpenGLContextData *pContext, GLsizei iCount );
void GL_P_DrawVertices( p3dOpenGLContextData *pContext, GLenum mode, GLint iFirst, GLsizei iCount, GLenum eIndexType, const GLvoid *pvIndices );


void GL_P_InitiateRender( 
	struct __P3D_OpenGLContextData *pContext,
	GLboolean bFlush );


#ifdef __cplusplus
}
#endif

#endif /* #ifndef GL_PRIVATE_H__ */

/* end of file */
