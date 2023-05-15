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
 * $brcm_Workfile: gl_vertexarray.c $
 * $brcm_Revision: DirectFB_1_4_14_Port/1 $
 * $brcm_Date: 7/28/11 11:22a $
 *
 * Module Description: Array entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.5/systems/bcmnexus/gl/gl_vertexarray.c $
 *
 * DirectFB_1_4_14_Port/1   7/28/11 11:22a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_5_Port/1   6/14/11 4:45p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 *
 * DirectFB_1_4_Port/1   8/6/10 8:09p robertwm
 * SW7405-4724: DirectFB: Support OpenGL ES 1.0 applications.
 *
 * Hydra_Software_Devel/8   9/8/08 1:16p nissen
 * PR 46638: Added support for screen aligned rectangle and drawing it
 * with the blitter.
 *
 * Hydra_Software_Devel/7   11/15/07 8:22p nissen
 * PR 35855: Added support for new native model.
 *
 * Hydra_Software_Devel/6   5/19/07 7:22a nissen
 * PR 30062: Added support for drawing screen aligned rectangles.
 *
 * Hydra_Software_Devel/5   9/20/06 1:25p nissen
 * PR 22858: Fixed fog.
 *
 * Hydra_Software_Devel/4   6/12/06 10:48a nissen
 * PR 21896: Added support for timing various operations.
 *
 * Hydra_Software_Devel/3   8/10/04 12:43p nissen
 * PR 9132: Fixed vertex strides. Added code to reject primitives if
 * viewport or scissor size is zero.
 *
 * Hydra_Software_Devel/2   7/28/04 2:30p nissen
 * PR 9132: Fixed comment header.
 *
 ***************************************************************************/

#include <direct/debug.h>

#include "gl_private.h"

D_DEBUG_DOMAIN( P3DGL_Draw, "P3DGL/Draw", "Broadcom P3D GL Draw" );

/***************************************************************************/
void GL_P_DrawVertices(
        p3dOpenGLContextData *pContext,
        GLenum mode,
        GLint iFirst,
        GLsizei iCount,
        GLenum eIndexType,
        const GLvoid *pvIndices )
{
        GL_P_Texture *pTexture = pContext->pTexture;
        GL_P_Matrix *pModelViewMatrix = &pContext->ModelViewMatrixStack.aMatrices[pContext->ModelViewMatrixStack.uiTop];
        GL_P_Matrix *pProjectionMatrix = &pContext->ProjectionMatrixStack.aMatrices[pContext->ProjectionMatrixStack.uiTop];

        D_DEBUG_AT( P3DGL_Draw, "%s( %p, %d, %d, %d, %d, %p )\n", __FUNCTION__, (void*)pContext, mode, iFirst, iCount, eIndexType, (void*)pvIndices );

        if( iCount > pContext->iVertexCount )
        {
                if( pContext->pVertices )
                        BKNI_Free( pContext->pVertices );

                pContext->pVertices = BKNI_Malloc( iCount * sizeof (BP3D_Vertex) );
                if( pContext->pVertices == NULL )
                {
                        pContext->iVertexCount = 0;
                        GL_P_SET_ERROR(GL_OUT_OF_MEMORY);
                        return;
                }

                pContext->iVertexCount = iCount;
        }

        if( GL_P_LoadTexture( pContext, pTexture ) == GL_FALSE )
        {
                GL_P_SET_ERROR(GL_OUT_OF_MEMORY);
                return;
        }

        if( pContext->ModelViewMatrixStack.bDirty || pContext->ProjectionMatrixStack.bDirty )
        {
                GL_P_SetCombinedMatrix( &pContext->CombinedMatrix, pProjectionMatrix, pModelViewMatrix );
                pContext->bNormalMatrixDirty = GL_TRUE;

                if( pContext->bFog )
                {
                        pContext->fTransFogStart = -pContext->fFogStart * pContext->CombinedMatrix.a[10];
                        pContext->fTransFogEnd = -pContext->fFogEnd * pContext->CombinedMatrix.a[10];
                }
        }

        if( pContext->bLighting )
        {
                if( pContext->bNormalMatrixDirty )
                {
                        GL_P_SetNormalMatrix( &pContext->NormalMatrix, pModelViewMatrix );
                        pContext->bNormalMatrixDirty = GL_FALSE;
                }

                GL_P_TransformEyeCoordArray( pContext, iFirst, iCount, eIndexType, pvIndices );
                GL_P_TransformNormalArray( pContext, iFirst, iCount, eIndexType, pvIndices );
                GL_P_CalculateLightValues( pContext, iCount );
        }
        else
        {
                GL_P_CopyColorArray( pContext, iFirst, iCount, eIndexType, pvIndices );
        }

        GL_P_TransformVertexArray( pContext, iFirst, iCount, eIndexType, pvIndices );

        if( pContext->bTexture2D && pTexture )
                GL_P_TransformTexCoordArray( pContext, iFirst, iCount, eIndexType, pvIndices );

        pContext->ModelViewMatrixStack.bDirty = GL_FALSE;
        pContext->ProjectionMatrixStack.bDirty = GL_FALSE;

        if( pContext->ulVertexRejectCode == 0 )
        {
                if( pContext->ulVertexOutCode == 0 )
                        GL_P_TransformVertices( pContext, iCount );

                switch( mode )
                {
                case GL_POINTS:
                        GL_P_ClipAndRenderPoints( pContext, iCount );
                        break;
                case GL_LINE_STRIP:
                        GL_P_ClipAndRenderLineStrip( pContext, iCount );
                        break;
                case GL_LINE_LOOP:
                        GL_P_ClipAndRenderLineLoop( pContext, iCount );
                        break;
                case GL_LINES:
                        GL_P_ClipAndRenderLines( pContext, iCount );
                        break;
                case GL_TRIANGLE_STRIP:
                        GL_P_ClipAndRenderTriangleStrip( pContext, iCount );
                        break;
                case GL_TRIANGLE_FAN:
                        GL_P_ClipAndRenderTriangleFan( pContext, iCount );
                        break;
                case GL_TRIANGLES:
                        GL_P_ClipAndRenderTriangles( pContext, iCount );
                        break;
                case GL_RECTANGLES:
                        GL_P_ClipAndRenderRectangles( pContext, iCount );
                        break;
                default:
                        GL_P_SET_ERROR(GL_INVALID_ENUM);
                        return;
                }
        }
}

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glVertexPointer(
        GLint size,
        GLenum type,
        GLsizei stride,
        const GLvoid *pointer )
{
        p3dOpenGLContextData *pContext = GL_P_GetContext();

        if( (size < 2) || (size > 4) || (stride < 0) )
        {
                GL_P_SET_ERROR(GL_INVALID_VALUE);
                return;
        }

        if( (type != GL_UNSIGNED_BYTE) && (type != GL_BYTE) &&
                (type != GL_UNSIGNED_SHORT) && (type != GL_SHORT) &&
                (type != GL_FIXED) && (type != GL_FLOAT) )
        {
                GL_P_SET_ERROR(GL_INVALID_ENUM);
                return;
        }

        pContext->VertexArray.iSize = size;
        pContext->VertexArray.eType = type;
        pContext->VertexArray.iStride = stride;
        pContext->VertexArray.pvMemory = pointer;
}

/***************************************************************************/
GLAPI void APIENTRY glNormalPointer(
        GLenum type,
        GLsizei stride,
        const GLvoid *pointer )
{
        p3dOpenGLContextData *pContext = GL_P_GetContext();

        if( stride < 0 )
        {
                GL_P_SET_ERROR(GL_INVALID_VALUE);
                return;
        }

        if( (type != GL_UNSIGNED_BYTE) && (type != GL_BYTE) &&
                (type != GL_UNSIGNED_SHORT) && (type != GL_SHORT) &&
                (type != GL_FIXED) && (type != GL_FLOAT) )
        {
                GL_P_SET_ERROR(GL_INVALID_ENUM);
                return;
        }

        pContext->NormalArray.iSize = 3;
        pContext->NormalArray.eType = type;
        pContext->NormalArray.iStride = stride;
        pContext->NormalArray.pvMemory = pointer;
}

/***************************************************************************/
GLAPI void APIENTRY glColorPointer(
        GLint size,
        GLenum type,
        GLsizei stride,
        const GLvoid *pointer )
{
        p3dOpenGLContextData *pContext = GL_P_GetContext();

        if( (size != 4) || (stride < 0) )
        {
                GL_P_SET_ERROR(GL_INVALID_VALUE);
                return;
        }

        if( (type != GL_UNSIGNED_BYTE) && (type != GL_BYTE) &&
                (type != GL_UNSIGNED_SHORT) && (type != GL_SHORT) &&
                (type != GL_FIXED) && (type != GL_FLOAT) )
        {
                GL_P_SET_ERROR(GL_INVALID_ENUM);
                return;
        }

        pContext->ColorArray.iSize = size;
        pContext->ColorArray.eType = type;
        pContext->ColorArray.iStride = stride;
        pContext->ColorArray.pvMemory = pointer;
}

/***************************************************************************/
GLAPI void APIENTRY glTexCoordPointer(
        GLint size,
        GLenum type,
        GLsizei stride,
        const GLvoid *pointer )
{
        p3dOpenGLContextData *pContext = GL_P_GetContext();

        if( (size < 2) || (size > 4) || (stride < 0) )
        {
                GL_P_SET_ERROR(GL_INVALID_VALUE);
                return;
        }

        if( (type != GL_UNSIGNED_BYTE) && (type != GL_BYTE) &&
                (type != GL_UNSIGNED_SHORT) && (type != GL_SHORT) &&
                (type != GL_FIXED) && (type != GL_FLOAT) )
        {
                GL_P_SET_ERROR(GL_INVALID_ENUM);
                return;
        }

        pContext->TexCoordArray.iSize = size;
        pContext->TexCoordArray.eType = type;
        pContext->TexCoordArray.iStride = stride;
        pContext->TexCoordArray.pvMemory = pointer;
}

/***************************************************************************/
GLAPI void APIENTRY glDrawArrays(
        GLenum mode,
        GLint first,
        GLsizei count )
{
        p3dOpenGLContextData *pContext = GL_P_GetContext();

        D_DEBUG_AT( P3DGL_Draw, "%s( %d, %d, %d )\n", __FUNCTION__, mode, first, count );

        if( count < 0 )
        {
                GL_P_SET_ERROR(GL_INVALID_VALUE);
                return;
        }

        if( count == 0 )
                return;

        if( pContext->bVertexArray == GL_FALSE ) {
             D_ONCE( "no vertex array enabled" );
                return;
        }

        if( pContext->VertexArray.pvMemory == NULL ) {
             D_ONCE( "no vertex array pointer" );
                return;
        }

        if( !(pContext->bMaskRed && pContext->bMaskGreen && pContext->bMaskBlue && pContext->bMaskAlpha) ) {
             D_ONCE( "no color masks" );
                return;
        }

        if( (pContext->Viewport.fWidth == 0.0) || (pContext->Viewport.fHeight == 0.0) ){
             D_ONCE( "zero viewport" );
                return;
        }

        if( pContext->bScissorTest && ((pContext->Scissor.fWidth == 0.0) || (pContext->Scissor.fHeight == 0.0)) ) {
             D_ONCE( "zero scissor" );
                return;
        }

        GL_P_DrawVertices( pContext, mode, first, count, 0, NULL );
}

/***************************************************************************/
GLAPI void APIENTRY glDrawElements(
        GLenum mode,
        GLsizei count,
        GLenum type,
        const GLvoid *indices )
{
        p3dOpenGLContextData *pContext = GL_P_GetContext();

        if( (count < 0) || (indices == NULL) )
        {
                GL_P_SET_ERROR(GL_INVALID_VALUE);
                return;
        }

        if( (type != GL_UNSIGNED_BYTE) && (type != GL_UNSIGNED_SHORT) )
        {
                GL_P_SET_ERROR(GL_INVALID_ENUM);
                return;
        }

        if( count == 0 )
                return;

        if( (pContext->bVertexArray == GL_FALSE) || (pContext->VertexArray.pvMemory == NULL) )
                return;

        if( !(pContext->bMaskRed && pContext->bMaskGreen && pContext->bMaskBlue && pContext->bMaskAlpha) )
                return;

        if( (pContext->Viewport.fWidth == 0.0) || (pContext->Viewport.fHeight == 0.0) )
                return;

        if( pContext->bScissorTest && ((pContext->Scissor.fWidth == 0.0) || (pContext->Scissor.fHeight == 0.0)) )
                return;

        GL_P_DrawVertices( pContext, mode, 0, count, type, indices );
}

/***************************************************************************/
GLAPI void APIENTRY glDrawAlignedRectangle(
        const GLfloat *v,
        const GLfloat *t,
        GLuint ulColor )
{
        p3dOpenGLContextData *pContext = GL_P_GetContext();
        GL_P_Texture *pTexture = pContext->pTexture;
        BP3D_Vertex vin0 = { 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0 };
        BP3D_Vertex vin1 = { 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0 };
        BP3D_Vertex vout0;
        BP3D_Vertex vout1;

        if( v == NULL )
                return;

        if( GL_P_LoadTexture( pContext, pTexture ) == GL_FALSE )
        {
                GL_P_SET_ERROR(GL_OUT_OF_MEMORY);
                return;
        }

        vin0.fX = v[0];
        vin0.fY = v[1];
        vin0.fS = t ? t[0] : 0;
        vin0.fT = t ? t[1] : 0;
        vin0.sX = (int16_t) vin0.fX;
        vin0.sY = (int16_t) vin0.fY;
        vin0.ulDiffuse = ulColor;

        vin1.fX = v[2];
        vin1.fY = v[3];
        vin1.fS = t ? t[2] : 0;
        vin1.fT = t ? t[3] : 0;
        vin1.sX = (int16_t) vin1.fX;
        vin1.sY = (int16_t) vin1.fY;
        vin1.ulDiffuse = ulColor;

        GL_P_TransformVertex( pContext, &vout0, &vin0 );
        GL_P_TransformVertex( pContext, &vout1, &vin1 );
        BP3D_DrawRectangle( pContext->hP3d, &vout0, &vout1 );
}

/* End of File */
