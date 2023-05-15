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
 * $brcm_Workfile: gl_pixel.c $
 * $brcm_Revision: DirectFB_1_4_14_Port/1 $
 * $brcm_Date: 7/28/11 11:22a $
 *
 * Module Description: Pixel entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.5/systems/bcmnexus/gl/gl_pixel.c $
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
 * Hydra_Software_Devel/7   11/15/07 8:12p nissen
 * PR 35855: Added support for new native model.
 *
 * Hydra_Software_Devel/6   11/11/05 3:49p nissen
 * PR 17423: Added support for using Settop API.
 *
 * Hydra_Software_Devel/5   3/9/05 3:17p nissen
 * PR 12813: Added support for more render and texture formats.
 *
 * Hydra_Software_Devel/4   9/17/04 3:04p nissen
 * PR 9132: Changed parameters for calling initiate render function.
 *
 * Hydra_Software_Devel/3   9/7/04 3:51p nissen
 * PR 9132: Added buffer swapping support.
 *
 * Hydra_Software_Devel/2   7/28/04 1:53p nissen
 * PR 9132: Fixed comment header.
 *
 ***************************************************************************/

#include <direct/messages.h>

#include "gl_private.h"

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI void APIENTRY glPixelStorei(
        GLenum pname,
        GLint param )
{
        BSTD_UNUSED( pname );
        BSTD_UNUSED( param );
}

/***************************************************************************/
GLAPI void APIENTRY glReadPixels(
        GLint x,
        GLint y,
        GLsizei width,
        GLsizei height,
        GLenum format,
        GLenum type,
        GLvoid *pixels )
{

#if 1

     BSTD_UNUSED(x);
     BSTD_UNUSED(y);
     BSTD_UNUSED(width);
     BSTD_UNUSED(height);
     BSTD_UNUSED(format);
     BSTD_UNUSED(type);
     BSTD_UNUSED(pixels);

     D_UNIMPLEMENTED();
#else
        EGL_P_Display *pDisplay = EGL_P_GetDisplay();
        p3dOpenGLContextData *pContext = GL_P_GetContext();
        EGL_P_Surface *pSurface = pContext->pDrawSurface;
        BSUR_Surface_Handle hSurface = pSurface->hRenderSurface;

        if( (format != GL_RGBA) && (format != GL_RGB) && (format != GL_LUMINANCE) &&
                (format != GL_LUMINANCE_ALPHA) && (format != GL_ALPHA) )
        {
                GL_P_SET_ERROR(GL_INVALID_ENUM);
                return;
        }

        if( (type != GL_UNSIGNED_BYTE) && (type != GL_UNSIGNED_SHORT_5_6_5) &&
                (type != GL_UNSIGNED_SHORT_5_5_5_1) && (type != GL_UNSIGNED_SHORT_4_4_4_4) )
        {
                GL_P_SET_ERROR(GL_INVALID_ENUM);
                return;
        }

        if( (x < 0) || (y < 0) || (width < 0) || (height < 0) || (pixels == NULL) )
        {
                GL_P_SET_ERROR(GL_INVALID_VALUE);
                return;
        }

        if( (width == 0) || (height == 0) )
                return;

        if( pContext->pDrawSurface && hSurface )
        {
                BPXL_Format eFormat;
                void *pvMemory;
                GLuint ulPitch;
                GLuint ulWidth;
                GLuint ulHeight;
                GLuint *pulDst;
                GLuint *pulSrc;
                GLushort *pusDst;
                GLushort *pusSrc;
                GLint ii, jj;

                EGL_P_InitiateRender( pContext, GL_TRUE );

                BSUR_Surface_GetFormat( hSurface, &eFormat );
                BSUR_Surface_GetAddress( hSurface, &pvMemory, &ulPitch );
                BSUR_Surface_GetDimensions( hSurface, &ulWidth, &ulHeight );

                pulDst = (GLuint *) pixels;
                pusDst = (GLushort *) pixels;
                pulSrc = (GLuint *) ((GLubyte *) pvMemory + (ulHeight - y - 1) * ulPitch) + x;
                pusSrc = (GLushort *) ((GLubyte *) pvMemory + (ulHeight - y - 1) * ulPitch) + x;

                if( type == GL_UNSIGNED_BYTE )
                {
                        switch( eFormat )
                        {
                        case BPXL_eA8_R8_G8_B8:
                                for( jj = 0; jj < height; ++jj )
                                {
                                        for( ii = 0; ii < width; ++ii )
                                                pulDst[ii] = (pulSrc[ii] & 0xFF00FF00) | ((pulSrc[ii] >> 16) & 0xFF) | ((pulSrc[ii] & 0xFF) << 16);
                                        pulDst += width;
                                        pulSrc -= ulPitch / 4;
                                }
                                break;

                        case BPXL_eR5_G6_B5:
                                for( jj = 0; jj < height; ++jj )
                                {
                                        for( ii = 0; ii < width; ++ii )
                                                pulDst[ii] = 0xFF000000 |
                                                        ((((GLuint) pusSrc[ii]) & 0xF800) >>  8) | ((((GLuint) pusSrc[ii]) & 0xE000) >> 13) |
                                                        ((((GLuint) pusSrc[ii]) & 0x07E0) <<  5) | ((((GLuint) pusSrc[ii]) & 0x0600) >>  1) |
                                                        ((((GLuint) pusSrc[ii]) & 0x001F) << 19) | ((((GLuint) pusSrc[ii]) & 0x001C) << 14);
                                        pulDst += width;
                                        pusSrc -= ulPitch / 2;
                                }
                                break;

                        case BPXL_eA4_R4_G4_B4:
                                for( jj = 0; jj < height; ++jj )
                                {
                                        for( ii = 0; ii < width; ++ii )
                                                pulDst[ii] =
                                                        ((((GLuint) pusSrc[ii]) & 0xF000) << 16) | ((((GLuint) pusSrc[ii]) & 0xF000) << 12) |
                                                        ((((GLuint) pusSrc[ii]) & 0x0F00) >>  4) | ((((GLuint) pusSrc[ii]) & 0x0F00) >>  8) |
                                                        ((((GLuint) pusSrc[ii]) & 0x00F0) <<  8) | ((((GLuint) pusSrc[ii]) & 0x00F0) <<  4) |
                                                        ((((GLuint) pusSrc[ii]) & 0x000F) << 20) | ((((GLuint) pusSrc[ii]) & 0x000F) << 16);
                                        pulDst += width;
                                        pusSrc -= ulPitch / 2;
                                }
                                break;

                        case BPXL_eA1_R5_G5_B5:
                                for( jj = 0; jj < height; ++jj )
                                {
                                        for( ii = 0; ii < width; ++ii )
                                                pulDst[ii] = ((((GLuint) pusSrc[ii]) & 0x8000) ? 0xFF000000 : 0) |
                                                        ((((GLuint) pusSrc[ii]) & 0x7C00) >>  7) | ((((GLuint) pusSrc[ii]) & 0x7000) >> 12) |
                                                        ((((GLuint) pusSrc[ii]) & 0x03E0) <<  6) | ((((GLuint) pusSrc[ii]) & 0x0380) <<  1) |
                                                        ((((GLuint) pusSrc[ii]) & 0x001F) << 19) | ((((GLuint) pusSrc[ii]) & 0x001C) << 14);
                                        pulDst += width;
                                        pusSrc -= ulPitch / 2;
                                }
                                break;

                        default:
                                break;
                        }
                }
                else
                {
                        for( jj = 0; jj < height; ++jj )
                        {
                                for( ii = 0; ii < width; ++ii )
                                        pusDst[ii] = pusSrc[ii];
                                pusDst += width;
                                pusSrc -= ulPitch / 2;
                        }
                }
        }
#endif
}

/* End of File */
