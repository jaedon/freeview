/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gl_private.h $
 * $brcm_Revision: Hydra_Software_Devel/15 $
 * $brcm_Date: 8/16/10 11:11a $
 *
 * Module Description: GL Private Header File.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/gl_private.h $
 * 
 * Hydra_Software_Devel/15   8/16/10 11:11a nissen
 * SW7400-2378: Changed version define.
 * 
 * Hydra_Software_Devel/14   7/29/10 8:50p nissen
 * SW7405-4657: Added support for color channel conversions when using
 * color arrays with lighting.
 * 
 * Hydra_Software_Devel/13   7/4/09 5:19p nissen
 * PR 56587: Removed function definition for LogicOps.
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

#include "math.h"
#include "bstd_defs.h"
#include "berr.h"
#include "bkni.h"
#include "bpxl.h"
#include "bsur.h"
#include "bgrc.h"
#include "bp3d.h"

/***************************************************************************/
#define GL_P_VENDOR                 "Broadcom"
#define GL_P_RENDERER               "PX3D"
#define GL_P_VERSION                "OpenGL ES-CM 1.0"
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
	GLboolean bVideo;
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
#define GL_P_SET_ERROR( err ) \
{ \
	EGL_P_Context *pContext = EGL_P_GetContext(); \
	if( pContext->eError == GL_NO_ERROR ) \
		pContext->eError = (err); \
}

/***************************************************************************/
#define GL_P_CONVERT_CHANNEL_XtoI( vv ) \
	((uint8_t) ((GL_P_MIN(GL_P_MAX((vv), 0), (1 << GL_P_FIXED_FRAC_BITS)) * 255) >> GL_P_FIXED_FRAC_BITS))

/***************************************************************************/
#define GL_P_CONVERT_CHANNEL_FtoI( vv ) \
	((uint8_t) (GL_P_MIN(GL_P_MAX((vv), 0.0), 1.0) * 255.0))

/***************************************************************************/
#define GL_P_CONVERT_CHANNEL_FtoF( vv ) (vv)

/***************************************************************************/
#define GL_P_CONVERT_CHANNEL_XtoF( vv ) ((GLfloat) (vv) / (1 << GL_P_FIXED_FRAC_BITS))

/***************************************************************************/
#define GL_P_CONVERT_CHANNEL_ItoF( vv ) ((GLfloat) (vv) / 255.0)

/***************************************************************************/
#define GL_P_CONVERT_COLOR_XtoI( aa, rr, gg, bb ) ( \
	(((uint32_t) GL_P_CONVERT_CHANNEL_XtoI(aa)) << 24) | \
	(((uint32_t) GL_P_CONVERT_CHANNEL_XtoI(rr)) << 16) | \
	(((uint32_t) GL_P_CONVERT_CHANNEL_XtoI(gg)) << 8) | \
	(((uint32_t) GL_P_CONVERT_CHANNEL_XtoI(bb))))

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
#ifdef __cplusplus
}
#endif

#endif /* #ifndef GL_PRIVATE_H__ */

/* end of file */
