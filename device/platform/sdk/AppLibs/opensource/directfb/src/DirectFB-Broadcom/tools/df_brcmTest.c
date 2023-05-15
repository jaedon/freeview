/******************************************************************************
 *    (c)2009-2012 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: df_brcmTest.c $
 * $brcm_Revision: DirectFB_1_4_15_Port/4 $
 * $brcm_Date: 12/20/11 4:15p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/tools/df_brcmTest.c $
 *
 * DirectFB_1_4_15_Port/4   12/20/11 4:15p bfu
 * SW7425-1414: DirectFB: replace DFB_FALSE (0) by EXIT_FAILURE to have
 *  proper exit code
 *
 * DirectFB_1_4_15_Port/3   12/16/11 5:04p bfu
 * SW7425-1414: DirectFB: return false if the test failed
 *
 * DirectFB_1_4_15_Port/2   12/7/11 11:40a bfu
 * SW7425-1414: DirectFB: automate df_brcmTest by comparing the two
 *  surfaces
 *
 * DirectFB_1_4_14_Port/1   7/28/11 11:24a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_5_Port/1   6/14/11 5:09p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 *
 * DirectFB_1_4_Port/8   4/15/11 3:11p raywu
 * SW7420-1667: 2D GraphicsHW vs SW verification: enable to go to specific
 *  test case
 *
 * DirectFB_1_4_Port/6   8/17/10 3:59p robertwm
 * SW3556-1183: DirectFB: Cannot run df_brcmTest on 3556 platform.
 *
 * DirectFB_1_4_Port/5   8/17/10 12:06a robertwm
 * SW7405-4763: DirectFB: df_brcmTest fails to update Test Description
 *  when in "force-window" mode.
 *
 * DirectFB_1_4_Port/4   8/13/10 4:08p robertwm
 * SW7405-4752: DirectFB: Add "Dual Output" capabilities to the "tools"
 *  unit test applications.
 *
 * DirectFB_1_4_Port/3   7/23/10 12:05p robertwm
 * SW7405-4479: Segmentation fault happened in the example df_brcmTest.
 *
 * DirectFB_1_4_Port/2   4/13/10 3:05p robertwm
 * SW7405-4199: DirectFB: Define FONT in Makefiles rather than in
 *  individual applications.
 *
 * DirectFB_1_4_Port/1   12/4/09 1:10p robertwm
 * SW7405-3547: DirectFB: Create test harness for exercising all drawing
 *  and blitting combinations.
 *
 *****************************************************************************/
#include <directfb.h>

#include <sys/time.h> /* for gettimeofday() */
#include <stdio.h>    /* for fprintf()      */
#include <stdlib.h>   /* for rand()         */
#include <unistd.h>   /* for sleep()        */
#include <string.h>   /* for strcmp()       */

#include "biglogo_A8.h"
#include "pngtest3.h"

#ifdef BUILD_AUTOMATION
#include <math.h>
#include "gfx/convert.h"
#endif

/* Definitions */
#define VERSION "0.1"
#define BRCM_MSG "Broadcom DirectFB Test Harness - Version "

/* Uncomment the line below to enable mirroring of primary graphics on secondary display */
/*#define DUAL_OUTPUT*/

/* Specify Primary surface Pixel Format */
#define PRIMARY_PIXELFORMAT DSPF_ARGB
//#define PRIMARY_PIXELFORMAT DSPF_LUT8

/* Either use image file or internal header for image */
#define TEST_IMAGE_FILE "pngtest2.png"
#define LOGO_IMAGE_FILE "biglogo_LUT8.gif"

#define USE_IMAGE_LUT8_FILE
#define LOGO_IMAGE_LUT8_FILE "beta_LUT8.png"

/* Define colors */
#define BG_COLOR_R 0xFF
#define BG_COLOR_G 0x80
#define BG_COLOR_B 0x80
#define BG_COLOR_A 0xFF

#define BOR_COLOR_R 0xFF
#define BOR_COLOR_G 0xFF
#define BOR_COLOR_B 0x00
#define BOR_COLOR_A 0xFF

#define BLT_COLOR_R 0x20
#define BLT_COLOR_G 0x40
#define BLT_COLOR_B 0x60
#define BLT_COLOR_A 0xFF

#define BLT_COLOR_A_HALF 0x80
#define BLT_COLOR_A_QUAR 0x40


/* macro for a safe call to DirectFB functions */
#define DFBCHECK(x)                                                     \
               err = x;                                                    \
               if (err != DFB_OK) {                                        \
                    fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ ); \
                    DirectFBErrorFatal( #x, err );                                                           \
                                }

#ifdef BUILD_AUTOMATION
static int failure = 0;                                              /*number of failures for automation test*/
static int skipped_tests[10] = {33, 39, 92, 98, 147, 154, 202, 208, 251, 296};   /*skip XOR and Stretch tests*/
#endif

/* Main DirectFB Interface */
static IDirectFB *dfb;

static IDirectFBSurface *background;

/* Co-ords for the windows */
static int MAIN_W, MAIN_H;
static int SUB_W = 500, SUB_H = 500;
static int SUB1_X = 100;
static int SUB1_Y = 150;
static int SUB2_X = 650;
static int SUB2_Y = 150;

static int INFO_W = (650 + 500 - 100); /* SUB2_X + SUB_W - SUB1_X */
static int INFO_H = 60;
static int INFO_X = 100; /* SUB1_X */
static int INFO_Y = 40;

static DFBSurfaceBlittingFlags globalBlittingFlags = DSBLIT_NOFX;
static DFBSurfaceDrawingFlags  globalDrawingFlags  = DSDRAW_NOFX;

#ifdef DUAL_OUTPUT
static IDirectFBDisplayLayer *secondary_layer;
#endif
static IDirectFBSurface *primary_hardware;
static IDirectFBSurface *primary_software;
static IDirectFBSurface *primary_info;

static IDirectFBSurface *primary;
static IDirectFBSurface *logo_surface;
#ifndef USE_IMAGE_LUT8_FILE
static IDirectFBSurface *biglogo_surface;
#endif

static IDirectFBSurface *test_surface;
static IDirectFBSurface *pngtest2_surface;
static IDirectFBFont *font;
static IDirectFBEventBuffer *key_events;

static DFBSurfaceDescription dsc;
static int fill_blending(long i, char* title, int blitFlags, int drawFlags, int porterduffFlags, int alpha, void* pLogo, void* pTest);
static int fill_porter_duff(long i, char* title, int blitFlags, int drawFlags, int porterduffFlags, int alpha, void* pLogo, void* pTest);
static int blit_normal(long i, char* title, int blitFlags, int drawFlags, int porterduffFlags, int alpha, void* pLogo, void* pTest);
static int blit_porter_duff(long i, char* title, int blitFlags, int drawFlags, int porterduffFlags, int alpha, void* pLogo, void* pTest);
static int blit_stretch(long i, char* title, int blitFlags, int drawFlags, int porterduffFlags, int alpha, void* pLogo, void* pTest);
static int draw_text(long i, char* title, int blitFlags, int drawFlags, int porterduffFlags, int alpha, void* pFontHeight, void* pSurfDesc);

typedef struct {
     char        desc[128];
     int           blitFlags;
     int           drawFlags;
     int           porterduffFlags;
     int           alpha;
     int (* func) ( long, char*, int, int, int, int, void *, void *);
} Test_t;

static Test_t draw_tests[] ={
    /*** Test with DSDRAW_NOFX flags set ***/
    { "Drawing Text with various levels of Alpha blending",
        DSBLIT_BLEND_ALPHACHANNEL, DSDRAW_NOFX,
        DSPD_NONE,
        0,
        draw_text},

    /*** Test with DSDRAW_BLEND set ***/
    { "Drawing Text with blending and various levels of Alpha blending",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_NONE,
        0,
        draw_text},
};

static Test_t fill_tests[] = {
    /* Test Blending with Drawing Flags = DSDRAW_BLEND */
    { "Fill Blending",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND, DSPD_NONE,
        0,
        fill_blending},

    /* Test Porter-Duff Clear */
    { "Blended Fill with Porter-Duff Clear (DSPD_CLEAR)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_CLEAR,
        0,
        fill_porter_duff},

    /* Test Porter-Duff Source */
    { "Blended Fill with Porter-Duff Source (DSPD_SRC)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_SRC,
        0,
        fill_porter_duff},

    /* Test Porter-Duff Source Over */
    { "Blended Fill with Porter-Duff Source Over blend (DSPD_SRC_OVER)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        0,
        fill_porter_duff},

    /* Test Porter-Duff Destination Over */
    { "Blended Fill with Porter-Duff Destination Over blend (DSPD_DST_OVER)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        0,
        fill_porter_duff},

    /* Test Porter-Duff Source In */
    { "Blended Fill with Porter-Duff Source In (DSPD_SRC_IN)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_SRC_IN,
        0,
        fill_porter_duff},

    /* Test Porter-Duff Destination In */
    { "Blended Fill with Porter-Duff Destination In (DSPD_DST_IN)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_DST_IN,
        0,
        fill_porter_duff},

    /* Test Porter-Duff Source Out */
    { "Blended Fill with Porter-Duff Source Out (DSPD_SRC_OUT)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_SRC_OUT,
        0,
        fill_porter_duff},

    /* Test Porter-Duff Destination Out */
    { "Blended Fill with Porter-Duff Destination Out (DSPD_DST_OUT)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_DST_OUT,
        0,
        fill_porter_duff},

    /* Test Porter-Duff Source ATOP */
    { "Blended Fill with Porter-Duff Source ATOP (DSPD_SRC_ATOP)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_SRC_ATOP,
        0,
        fill_porter_duff},

    /* Test Porter-Duff Destination ATOP */
    { "Blended Fill with Porter-Duff Destination ATOP (DSPD_DST_ATOP)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_DST_ATOP,
        0,
        fill_porter_duff},

    /* Test Porter-Duff Add */
    { "Blended Fill with Porter-Duff Add (DSPD_ADD)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_ADD,
        0,
        fill_porter_duff},

    /* Test Porter-Duff Xor */
    { "Blended Fill with Porter-Duff XOR (DSPD_XOR)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_XOR,
        0,
        fill_porter_duff},

    /* Test Porter-Duff Clear */
    { "Premultiplied Blended Fill with Porter-Duff Clear (DSPD_CLEAR)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND|DSDRAW_SRC_PREMULTIPLY,
        DSPD_CLEAR,
        0,
        fill_porter_duff},

    /* Test Porter-Duff Source */
    { "Premultiplied Blended Fill with Porter-Duff Source (DSPD_SRC)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND|DSDRAW_SRC_PREMULTIPLY,
        DSPD_SRC,
        0,
        fill_porter_duff},

    /* Test Porter-Duff Source Over */
    { "Premultiplied Blended Fill with Porter-Duff Source Over blend (DSPD_SRC_OVER)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND|DSDRAW_SRC_PREMULTIPLY,
        DSPD_SRC_OVER,
        0,
        fill_porter_duff},

    /* Test Porter-Duff Destination Over */
    { "Premultiplied Blended Fill with Porter-Duff Destination Over blend (DSPD_DST_OVER)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND|DSDRAW_SRC_PREMULTIPLY,
        DSPD_DST_OVER,
        0,
        fill_porter_duff},

     /* Test Porter-Duff Source In */
    { "Premultiplied Blended Fill with Porter-Duff Source In (DSPD_SRC_IN)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND|DSDRAW_SRC_PREMULTIPLY,
        DSPD_SRC_IN,
        0,
        fill_porter_duff},

    /* Test Porter-Duff Destination In */
    { "Premultiplied Blended Fill with Porter-Duff Destination In (DSPD_DST_IN)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND|DSDRAW_SRC_PREMULTIPLY,
        DSPD_DST_IN,
        0,
        fill_porter_duff},

    /* Test Porter-Duff Source Out */
    { "Premultiplied Blended Fill with Porter-Duff Source Out (DSPD_SRC_OUT)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND|DSDRAW_SRC_PREMULTIPLY,
        DSPD_SRC_OUT,
        0,
        fill_porter_duff},

    /* Test Porter-Duff Destination Out */
    { "Premultiplied Blended Fill with Porter-Duff Destination Out (DSPD_DST_OUT)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND|DSDRAW_SRC_PREMULTIPLY,
        DSPD_DST_OUT,
        0,
        fill_porter_duff},

    /* Test Porter-Duff Source ATOP */
    { "Premultiplied Blended Fill with Porter-Duff Source ATOP (DSPD_SRC_ATOP)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND|DSDRAW_SRC_PREMULTIPLY,
        DSPD_SRC_ATOP,
        0,
        fill_porter_duff},

    /* Test Porter-Duff Destination ATOP */
    { "Premultiplied Blended Fill with Porter-Duff Destination ATOP (DSPD_DST_ATOP)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND|DSDRAW_SRC_PREMULTIPLY,
        DSPD_DST_ATOP,
        0,
        fill_porter_duff},

    /* Test Porter-Duff Add */
    { "Premultiplied Blended Fill with Porter-Duff Add (DSPD_ADD)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND|DSDRAW_SRC_PREMULTIPLY,
        DSPD_ADD,
        0,
        fill_porter_duff},

    /* Test Porter-Duff Xor */
    { "Premultiplied Blended Fill with Porter-Duff XOR (DSPD_XOR)",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND|DSDRAW_SRC_PREMULTIPLY,
        DSPD_XOR,
        0,
        fill_porter_duff},
};

static Test_t blit_tests[] = {
    /* Test Normal Blit with no flags */
    { "Blit with no flags set",
        DSBLIT_NOFX, DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

     /* Test Flip Horizontally */
    { "Blit with Horizontal Flip",
        DSBLIT_FLIP_HORIZONTAL,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A,
        blit_normal},

     /* Test Flip Vertically */
    { "Blit with Vertical Flip",
        DSBLIT_FLIP_VERTICAL,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A,
        blit_normal},

    /* Test Rotate 180 */
    { "Blit with 180 Degrees Rotation",
        DSBLIT_ROTATE180,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A,
        blit_normal},

    /* Test Flip Horizontally with Alpha Channel Blending */
    { "Blit with Horizontal Flip and Alpha Channel Blending",
        DSBLIT_FLIP_HORIZONTAL | DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A,
        blit_normal},

    /* Test Flip Vertically with Alpha Channel Blending */
    { "Blit with Vertical Flip and Alpha Channel Blending",
        DSBLIT_FLIP_VERTICAL | DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A,
        blit_normal},

    /* Test Rotate 180 with Alpha Channel Blending */
    { "Blit with 180 Degrees Rotation and Alpha Channel Blending",
        DSBLIT_ROTATE180 | DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A,
        blit_normal},

    /* Test XOR */
    { "Blit with XOR",
        DSBLIT_XOR,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A,
        blit_normal},

    /* Test XOR with Alpha Channel Blending */
    { "Blit with XOR and Alpha Channel Blending",
        DSBLIT_XOR | DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test XOR with Color Alpha Blending */
    { "Blit with XOR and Color Alpha Blending",
        DSBLIT_XOR | DSBLIT_BLEND_COLORALPHA,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test XOR with Alpha Channel and Color Alpha Blending */
    { "Blit with XOR and Alpha Channel + Color Alpha Blending",
        DSBLIT_XOR | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test XOR with Premultiply and Alpha Channel and Color Alpha Blending */
    { "Blit with XOR and Premultiply + Alpha Channel + Color Alpha Blending",
        DSBLIT_XOR | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test XOR with Premultiply and Alpha Channel and Color Alpha Blending and PremultColor */
    { "Blit with XOR + Premultiply + Alpha Channel + Color Alpha + PREMULTCOLOR Blending",
        DSBLIT_XOR | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test XOR with Premultiply and Alpha Channel and Color Alpha Blending and PremultColor and Colorize */
    { "Blit with XOR + Premultiply + Alpha Channel + Color Alpha + PREMULTCOLOR + Colorize Blending",
        DSBLIT_XOR | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test Alpha Channel Blending */
    { "Blit with Alpha Channel Blending",
        DSBLIT_BLEND_ALPHACHANNEL | 0/*DSBLIT_SRC_COLORKEY*/,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A,
        blit_normal},

    /* Test 100% Color Alpha Blending */
    { "Blit with 100% Color Alpha",
        DSBLIT_BLEND_COLORALPHA | 0/*DSBLIT_SRC_COLORKEY*/,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A,
        blit_normal},

    /* Test 50% Color Alpha Blending */
    { "Blit with 50% Color Alpha",
        DSBLIT_BLEND_COLORALPHA | 0/*DSBLIT_SRC_COLORKEY*/,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test 25% Color Alpha Blending */
    { "Blit with 25% Color Alpha",
        DSBLIT_BLEND_COLORALPHA | 0/*DSBLIT_SRC_COLORKEY*/,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_QUAR,
        blit_normal},

    /* Test 100% Color Alpha + Alpha Channel Blending */
    { "Blit with 100% Color Alpha + Alpha Blend",
        DSBLIT_BLEND_COLORALPHA|DSBLIT_BLEND_ALPHACHANNEL | 0/*DSBLIT_SRC_COLORKEY*/,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A,
        blit_normal},

    /* Test 50% Color Alpha + Alpha Channel Blending */
    { "Blit with 50% Color Alpha + Alpha Blend",
        DSBLIT_BLEND_COLORALPHA|DSBLIT_BLEND_ALPHACHANNEL | 0/*DSBLIT_SRC_COLORKEY*/,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test 25% Color Alpha + Alpha Channel Blending */
    { "Blit with 25% Color Alpha + Alpha Blend",
        DSBLIT_BLEND_COLORALPHA|DSBLIT_BLEND_ALPHACHANNEL | 0/*DSBLIT_SRC_COLORKEY*/,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_QUAR,
        blit_normal},

    /* Test PREMULTIPLY */
    { "Blit with Premultiply Source",
        DSBLIT_SRC_PREMULTIPLY|0/*DSBLIT_SRC_COLORKEY*/,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test 100% Color Alpha Premultipled Source Blending */
    { "Blit with 100% Color Alpha Premultiplied Source",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY |0/*DSBLIT_SRC_COLORKEY*/,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A,
        blit_normal},

    /* Test 100% Color Alpha Premultipled Source Blending */
    { "Blit with 50% Color Alpha Premultiplied Source",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY |0/*DSBLIT_SRC_COLORKEY*/,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test 100% Color Alpha Premultipled Source Blending */
    { "Blit with 25% Color Alpha Premultiplied Source",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY |0/*DSBLIT_SRC_COLORKEY*/,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_QUAR,
        blit_normal},

    /* Test Normal Blit with no flags */
    { "Blit with no flags set",
        DSBLIT_NOFX,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

     /* Test Alpha Channel + PREMULTIPLY Blending */
    { "Blit with Premultiply Source Alpha Blending",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_SRC_PREMULTIPLY|0/*DSBLIT_SRC_COLORKEY*/,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY Blending */
    { "Blit with Premultiply Source Alpha + 100% Alpha Color blending",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A,
        blit_normal},

    /* Test 50% Alpha Color + Alpha Channel + PREMULTIPLY Blending */
    { "Blit with Premultiply Source Alpha + 50% Alpha Color blending",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test 25% Alpha Color + Alpha Channel + PREMULTIPLY Blending */
    { "Blit with Premultiply Source Alpha + 25% Alpha Color blending",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_QUAR,
        blit_normal},

    /* Test 100% PREMULTCOLOR */
    { "Blit with 100% PREMULTCOLOR on the Source",
        DSBLIT_SRC_PREMULTCOLOR|0/*DSBLIT_SRC_COLORKEY*/,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A,
        blit_normal},

    /* Test 50% PREMULTCOLOR */
    { "Blit with 50% PREMULTCOLOR on the Source",
        DSBLIT_SRC_PREMULTCOLOR|0/*DSBLIT_SRC_COLORKEY*/,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test 25% PREMULTCOLOR */
    { "Blit with 25% PREMULTCOLOR on the Source",
        DSBLIT_SRC_PREMULTCOLOR|0/*DSBLIT_SRC_COLORKEY*/,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_QUAR,
        blit_normal},

    /* Test 100% Alpha Color + Alpha Channel + 100% PREMULTCOLOR Blending */
    { "Blit with Source Alpha + 100% Alpha Color + 100% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A,
        blit_normal},

    /* Test 50% Alpha Color + Alpha Channel + 50% PREMULTCOLOR Blending */
    { "Blit with Source Alpha + 50% Alpha Color + 50% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test 25% Alpha Color + Alpha Channel + 25% PREMULTCOLOR Blending */
    { "Blit with Source Alpha + 25% Alpha Color + 25% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_QUAR,
        blit_normal},

    /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY + 100% PREMULTCOLOR Blending */
    { "Blit with Premultiply Source Alpha + 100% Alpha Color + 100% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A,
        blit_normal},

    /* Test 50% Alpha Color + Alpha Channel + PREMULTIPLY + 50% PREMULTCOLOR Blending */
    { "Blit with Premultiply Source Alpha + 50% Alpha Color + 50% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test 25% Alpha Color + Alpha Channel + PREMULTIPLY + 25% PREMULTCOLOR Blending */
    { "Blit with Premultiply Source Alpha + 25% Alpha Color + 25% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_QUAR,
        blit_normal},

    /* Test Alpha Channel Blending with Colorize */
    { "Blit with Source Alpha Blending with Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test 100% Color Alpha Blending with Colorize */
    { "Blit with 100% Color Alpha Blending with Colorize",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A,
        blit_normal},

    /* Test 100% Color Alpha Blending with Colorize */
    { "Blit with 50% Color Alpha Blending with Colorize",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test 100% Color Alpha Blending with Colorize */
    { "Blit with 25% Color Alpha Blending with Colorize",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_QUAR,
        blit_normal},

    /* Test 100% Color Alpha + Alpha Channel Blending + Colorize */
    { "Blit with 100% Color Alpha + Source Alpha Blending + Colorize",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A,
        blit_normal},

    /* Test 50% Color Alpha + Alpha Channel Blending + Colorize */
    { "Blit with 50% Color Alpha + Source Alpha Blending + Colorize",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test 25% Color Alpha + Alpha Channel Blending + Colorize */
    { "Blit with 25% Color Alpha + Source Alpha Blending + Colorize",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_QUAR,
        blit_normal},

    /* Test PREMULTIPLY with Colorize */
    { "Blit with Premultiply Source with Colorize",
        DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test Alpha Channel + PREMULTIPLY Blending + Colorize */
    { "Blit with Premultiply Source Alpha Blending with Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY Blending + Colorize */
    { "Blit with Premultiply Source Alpha + 100% Alpha Color blending + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A,
        blit_normal},

    /* Test 50% Alpha Color + Alpha Channel + PREMULTIPLY Blending + Colorize */
    { "Blit with Premultiply Source Alpha + 50% Alpha Color blending + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test 25% Alpha Color + Alpha Channel + PREMULTIPLY Blending + Colorize */
    { "Blit with Premultiply Source Alpha + 25% Alpha Color blending + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_QUAR,
        blit_normal},

    /* Test 100% PREMULTCOLOR with Colorize */
    { "Blit with 100% PREMULTCOLOR on the Source with Colorize",
        DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A,
        blit_normal},

    /* Test 50% PREMULTCOLOR with Colorize */
    { "Blit with 50% PREMULTCOLOR on the Source with Colorize",
        DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test 25% PREMULTCOLOR with Colorize */
    { "Blit with 25% PREMULTCOLOR on the Source with Colorize",
        DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_QUAR,
        blit_normal},

    /* Test 100% Alpha Color + Alpha Channel + 100% PREMULTCOLOR Blending + Colorize */
    { "Blit with Source Alpha + 100% Alpha Color + 100% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A,
        blit_normal},

    /* Test 100% Alpha Color + Alpha Channel + 100% PREMULTCOLOR Blending + Colorize */
    { "Blit with Source Alpha + 50% Alpha Color + 50% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test 100% Alpha Color + Alpha Channel + 100% PREMULTCOLOR Blending + Colorize */
    { "Blit with Source Alpha + 25% Alpha Color + 25% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_QUAR,
        blit_normal},

    /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY + 100% PREMULTCOLOR Blending + Colorize */
    { "Blit with Premultiply Source Alpha + 100% Alpha Color + 100% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY |DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A,
        blit_normal},

    /* Test 50% Alpha Color + Alpha Channel + PREMULTIPLY + 50% PREMULTCOLOR Blending + Colorize */
    { "Blit with Premultiply Source Alpha + 50% Alpha Color + 50% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY |DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_HALF,
        blit_normal},

    /* Test 25% Alpha Color + Alpha Channel + PREMULTIPLY + 25% PREMULTCOLOR Blending + Colorize */
    { "Blit with Premultiply Source Alpha + 25% Alpha Color + 25% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY |DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_NONE,
        BLT_COLOR_A_QUAR,
        blit_normal},

    /*** Test with Porter-Duff Source */
    /* Test Flip Horizontally */
    { "Porter-Duff Source Blit with Horizontal Flip",
        DSBLIT_FLIP_HORIZONTAL,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A,
        blit_porter_duff},

     /* Test Flip Vertically */
    { "Porter-Duff Source Blit with Vertical Flip",
        DSBLIT_FLIP_VERTICAL,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A,
        blit_porter_duff},

     /* Test Rotate 180 */
    { "Porter-Duff Source Blit with 180 Degrees Rotation",
        DSBLIT_ROTATE180,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test Flip Horizontally with Alpha Channel Blending */
    { "Porter-Duff Source Blit with Horizontal Flip and Alpha Channel Blending",
        DSBLIT_FLIP_HORIZONTAL | DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A,
        blit_porter_duff},

     /* Test Flip Vertically with Alpha Channel Blending */
    { "Porter-Duff Source Blit with Vertical Flip and Alpha Channel Blending",
        DSBLIT_FLIP_VERTICAL | DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_SRC, BLT_COLOR_A,
        blit_porter_duff},

     /* Test Rotate 180 with Alpha Channel Blending */
    { "Porter-Duff Source Blit with 180 Degrees Rotation and Alpha Channel Blending",
        DSBLIT_ROTATE180 | DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test XOR */
    { "Porter-Duff Source Blit with XOR",
        DSBLIT_XOR,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A,
        blit_porter_duff},

     /* Test XOR with Alpha Channel Blending */
    { "Porter-Duff Source Blit with XOR + Alpha Channel Blending",
        DSBLIT_XOR | DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test XOR with Color Alpha Blending */
    { "Porter-Duff Source Blit with XOR + Color Alpha Blending",
        DSBLIT_XOR | DSBLIT_BLEND_COLORALPHA,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test XOR with Alpha Channel and Color Alpha Blending */
    { "Porter-Duff Source Blit with XOR + Alpha Channel + Color Alpha Blending",
        DSBLIT_XOR | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test XOR with Premultiply and Alpha Channel and Color Alpha Blending */
    { "Porter-Duff Source Blit with XOR + Premultiply + Alpha Channel + Color Alpha Blending",
        DSBLIT_XOR | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test XOR with Premultiply and Alpha Channel and Color Alpha Blending and PremultColor */
    { "Porter-Duff Source Blit with XOR + Premultiply + Alpha Channel + Color Alpha + PREMULTCOLOR Blending",
        DSBLIT_XOR | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test XOR with Premultiply and Alpha Channel and Color Alpha Blending and PremultColor and Colorize */
    { "Porter-Duff Source Blit with XOR + Premultiply + Alpha Channel + Color Alpha + PREMULTCOLOR + Colorize Blending",
        DSBLIT_XOR | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test Alpha Channel Blending */
    { "Porter-Duff Source Blit with Alpha Channel Blending",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A,
        blit_porter_duff},

     /* Test 100% Color Alpha Blending */
    { "Porter-Duff Source Blit with 100% Color Alpha Blending",
        DSBLIT_BLEND_COLORALPHA,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A,
        blit_porter_duff},

     /* Test 50% Color Alpha Blending */
    { "Porter-Duff Source Blit with 50% Color Alpha Blending",
        DSBLIT_BLEND_COLORALPHA,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test 25% Color Alpha Blending */
    { "Porter-Duff Source Blit with 25% Color Alpha Blending",
        DSBLIT_BLEND_COLORALPHA,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test 100% Color Alpha + Alpha Channel Blending */
    { "Porter-Duff Source Blit with 100% Color Alpha + Alpha Blend",
        DSBLIT_BLEND_COLORALPHA|DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% Color Alpha + Alpha Channel Blending */
    { "Porter-Duff Source Blit with 50% Color Alpha + Alpha Blend",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% Color Alpha + Alpha Channel Blending */
    { "Porter-Duff Source Blit with 25% Color Alpha + Alpha Blend",
        DSBLIT_BLEND_COLORALPHA|DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test PREMULTIPLY */
    { "Porter-Duff Source Blit with Premultiply Source",
        DSBLIT_SRC_PREMULTIPLY,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test Alpha Channel + PREMULTIPLY Blending */
    { "Porter-Duff Source Blit with Premultiply Source Alpha Blending",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_SRC_PREMULTIPLY,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY Blending */
    { "Porter-Duff Source Blit with Premultiply Source Alpha + 100% Alpha Color blending",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A,
        blit_porter_duff},

     /* Test 50% Alpha Color + Alpha Channel + PREMULTIPLY Blending */
    { "Porter-Duff Source Blit with Premultiply Source Alpha + 50% Alpha Color blending",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test 25% Alpha Color + Alpha Channel + PREMULTIPLY Blending */
    { "Porter-Duff Source Blit with Premultiply Source Alpha + 25% Alpha Color blending",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test 100% PREMULTCOLOR */
    { "Porter-Duff Source Blit with 100% PREMULTCOLOR on the Source",
        DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% PREMULTCOLOR */
    { "Porter-Duff Source Blit with 50% PREMULTCOLOR on the Source",
        DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% PREMULTCOLOR */
    { "Porter-Duff Source Blit with 25% PREMULTCOLOR on the Source",
        DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},


    /* Test 100% Alpha Color + Alpha Channel + 100% PREMULTCOLOR Blending */
    { "Porter-Duff Source Blit with Source Alpha + 100% Alpha Color + 100% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% Alpha Color + Alpha Channel + 50% PREMULTCOLOR Blending */
    { "Porter-Duff Source Blit with Source Alpha + 50% Alpha Color + 50% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% Alpha Color + Alpha Channel + 25% PREMULTCOLOR Blending */
    { "Porter-Duff Source Blit with Source Alpha + 25% Alpha Color + 25% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY + 100% PREMULTCOLOR Blending */
    { "Porter-Duff Source Blit with Premultiply Source Alpha + 100% Alpha Color + 100% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA |DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% Alpha Color + Alpha Channel + PREMULTIPLY + 50% PREMULTCOLOR Blending */
    { "Porter-Duff Source Blit with Premultiply Source Alpha + 50% Alpha Color + 50% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% Alpha Color + Alpha Channel + PREMULTIPLY + 25% PREMULTCOLOR Blending */
    { "Porter-Duff Source Blit with Premultiply Source Alpha + 25% Alpha Color + 25% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test Alpha Channel Blending with Colorize */
    { "Porter-Duff Source Blit with Source Alpha Blending with Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 100% Color Alpha Blending with Colorize */
    { "Porter-Duff Source Blit with 100% Color Alpha Blending with Colorize",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% Color Alpha Blending with Colorize */
    { "Porter-Duff Source Blit with 50% Color Alpha Blending with Colorize",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% Color Alpha Blending with Colorize */
    { "Porter-Duff Source Blit with 25% Color Alpha Blending with Colorize",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test 100% Color Alpha + Alpha Channel Blending + Colorize */
    { "Porter-Duff Source Blit with 100% Color Alpha + Source Alpha Blending + Colorize",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% Color Alpha + Alpha Channel Blending + Colorize */
    { "Porter-Duff Source Blit with 50% Color Alpha + Source Alpha Blending + Colorize",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% Color Alpha + Alpha Channel Blending + Colorize */
    { "Porter-Duff Source Blit with 25% Color Alpha + Source Alpha Blending + Colorize",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test PREMULTIPLY with Colorize */
    { "Porter-Duff Source Blit with Premultiply Source with Colorize",
        DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test Alpha Channel + PREMULTIPLY Blending + Colorize */
    { "Porter-Duff Source Blit with Premultiply Source Alpha Blending with Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY Blending + Colorize */
    { "Porter-Duff Source Blit with Premultiply Source Alpha + 100% Alpha Color blending + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% Alpha Color + Alpha Channel + PREMULTIPLY Blending + Colorize */
    { "Porter-Duff Source Blit with Premultiply Source Alpha + 50% Alpha Color blending + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% Alpha Color + Alpha Channel + PREMULTIPLY Blending + Colorize */
    { "Porter-Duff Source Blit with Premultiply Source Alpha + 25% Alpha Color blending + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test 100% PREMULTCOLOR with Colorize */
    { "Porter-Duff Source Blit with 100% PREMULTCOLOR on the Source with Colorize",
        DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A,
        blit_porter_duff},


    /* Test 50% PREMULTCOLOR with Colorize */
    { "Porter-Duff Source Blit with 50% PREMULTCOLOR on the Source with Colorize",
        DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% PREMULTCOLOR with Colorize */
    { "Porter-Duff Source Blit with 25% PREMULTCOLOR on the Source with Colorize",
        DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test 100% Alpha Color + Alpha Channel + 100% PREMULTCOLOR Blending + Colorize */
    { "Porter-Duff Source Blit with Source Alpha + 100% Alpha Color + 100% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% Alpha Color + Alpha Channel + 50% PREMULTCOLOR Blending + Colorize */
    { "Porter-Duff Source Blit with Source Alpha + 50% Alpha Color + 50% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% Alpha Color + Alpha Channel + 25% PREMULTCOLOR Blending + Colorize */
    { "Porter-Duff Source Blit with Source Alpha + 25% Alpha Color + 25% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY + 100% PREMULTCOLOR Blending + Colorize */
    { "Porter-Duff Source Blit with Premultiply Source Alpha + 100% Alpha Color + 100% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% Alpha Color + Alpha Channel + PREMULTIPLY + 50% PREMULTCOLOR Blending + Colorize */
    { "Porter-Duff Source Blit with Premultiply Source Alpha + 50% Alpha Color + 50% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% Alpha Color + Alpha Channel + PREMULTIPLY + 25% PREMULTCOLOR Blending + Colorize */
    { "Porter-Duff Source Blit with Premultiply Source Alpha + 25% Alpha Color + 25% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /*** Test with Porter-Duff Source Over */
    /* Test Flip Horizontally */
    { "Porter-Duff Source Over Blit with Horizontal Flip",
        DSBLIT_FLIP_HORIZONTAL,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test Flip Vertically */
    { "Porter-Duff Source Over Blit with Vertical Flip",
        DSBLIT_FLIP_VERTICAL,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test Rotate 180 */
    { "Porter-Duff Source Over Blit with 180 Degrees Rotation",
        DSBLIT_ROTATE180,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test Flip Horizontally with Alpha Channel Blending */
    { "Porter-Duff Source Over Blit with Horizontal Flip and Alpha Channel Blending",
        DSBLIT_FLIP_HORIZONTAL | DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test Flip Vertically with Alpha Channel Blending */
    { "Porter-Duff Source Over Blit with Vertical Flip and Alpha Channel Blending",
        DSBLIT_FLIP_VERTICAL | DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test Rotate 180 with Alpha Channel Blending */
    { "Porter-Duff Source Over Blit with 180 Degrees Rotation and Alpha Channel Blending",
        DSBLIT_ROTATE180 | DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

     /* Test XOR */
    { "Porter-Duff Source Over Blit with XOR",
        DSBLIT_XOR,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

     /* Test XOR with Alpha Channel Blending */
    { "Porter-Duff Source Over Blit with XOR and Alpha Channel Blending",
        DSBLIT_XOR | DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test XOR with Color Alpha Blending */
    { "Porter-Duff Source Over Blit with XOR and Color Alpha Blending",
        DSBLIT_XOR | DSBLIT_BLEND_COLORALPHA,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test XOR with Alpha Channel and Color Alpha Blending */
    { "Porter-Duff Source Over Blit with XOR and Alpha Channel + Color Alpha Blending",
        DSBLIT_XOR | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test XOR with Premultiply and Alpha Channel and Color Alpha Blending */
    { "Porter-Duff Source Over Blit with XOR and Premultiply + Alpha Channel + Color Alpha Blending",
        DSBLIT_XOR | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test XOR with Premultiply and Alpha Channel and Color Alpha Blending and PremultColor */
    { "Porter-Duff Source Over Blit with XOR + Premultiply + Alpha Channel + Color Alpha + PREMULTCOLOR Blending",
        DSBLIT_XOR | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test XOR with Premultiply and Alpha Channel and Color Alpha Blending and PremultColor and Colorize */
    { "Porter-Duff Source Over Blit with XOR + Premultiply + Alpha Channel + Color Alpha + PREMULTCOLOR + Colorize Blending",
        DSBLIT_XOR | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test Alpha Channel Blending */
    { "Porter-Duff Source Over Blit with Alpha Channel Blending",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 100% Color Alpha Blending */
    { "Porter-Duff Source Over Blit with 100% Color Alpha Blending",
        DSBLIT_BLEND_COLORALPHA,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% Color Alpha Blending */
    { "Porter-Duff Source Over Blit with 50% Color Alpha Blending",
        DSBLIT_BLEND_COLORALPHA,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% Color Alpha Blending */
    { "Porter-Duff Source Over Blit with 25% Color Alpha Blending",
        DSBLIT_BLEND_COLORALPHA,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test 100% Color Alpha + Alpha Channel Blending */
    { "Porter-Duff Source Over Blit with 100% Color Alpha + Alpha Blend",
        DSBLIT_BLEND_COLORALPHA|DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% Color Alpha + Alpha Channel Blending */
    { "Porter-Duff Source Over Blit with 50% Color Alpha + Alpha Blend",
        DSBLIT_BLEND_COLORALPHA|DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% Color Alpha + Alpha Channel Blending */
    { "Porter-Duff Source Over Blit with 25% Color Alpha + Alpha Blend",
        DSBLIT_BLEND_COLORALPHA|DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test PREMULTIPLY */
    { "Porter-Duff Source Over Blit with Premultiply Source",
        DSBLIT_SRC_PREMULTIPLY,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test Alpha Channel + PREMULTIPLY Blending */
    { "Porter-Duff Source Over Blit with Premultiply Source Alpha Blending",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_SRC_PREMULTIPLY,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY Blending */
    { "Porter-Duff Source Over Blit with Premultiply Source Alpha + 100% Alpha Color blending",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

     /* Test 50% Alpha Color + Alpha Channel + PREMULTIPLY Blending */
    { "Porter-Duff Source Over Blit with Premultiply Source Alpha + 50% Alpha Color blending",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test 25% Alpha Color + Alpha Channel + PREMULTIPLY Blending */
    { "Porter-Duff Source Over Blit with Premultiply Source Alpha + 25% Alpha Color blending",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test 100% PREMULTCOLOR */
    { "Porter-Duff Source Over Blit with 100% PREMULTCOLOR on the Source",
        DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% PREMULTCOLOR */
    { "Porter-Duff Source Over Blit with 50% PREMULTCOLOR on the Source",
        DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% PREMULTCOLOR */
    { "Porter-Duff Source Over Blit with 25% PREMULTCOLOR on the Source",
        DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test 100% Alpha Color + Alpha Channel + 100% PREMULTCOLOR Blending */
    { "Porter-Duff Source Over Blit with Source Alpha + 100% Alpha Color + 100% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% Alpha Color + Alpha Channel + 50% PREMULTCOLOR Blending */
    { "Porter-Duff Source Over Blit with Source Alpha + 50% Alpha Color + 50% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% Alpha Color + Alpha Channel + 25% PREMULTCOLOR Blending */
    { "Porter-Duff Source Over Blit with Source Alpha + 25% Alpha Color + 25% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY + 100% PREMULTCOLOR Blending */
    { "Porter-Duff Source Over Blit with Premultiply Source Alpha + 100% Alpha Color + 100% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% Alpha Color + Alpha Channel + PREMULTIPLY + 50% PREMULTCOLOR Blending */
    { "Porter-Duff Source Over Blit with Premultiply Source Alpha + 50% Alpha Color + 50% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% Alpha Color + Alpha Channel + PREMULTIPLY + 25% PREMULTCOLOR Blending */
    { "Porter-Duff Source Over Blit with Premultiply Source Alpha + 25% Alpha Color + 25% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test Alpha Channel Blending with Colorize */
    { "Porter-Duff Source Over Blit with Source Alpha Blending with Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 100% Color Alpha Blending with Colorize */
    { "Porter-Duff Source Over Blit with 100% Color Alpha Blending with Colorize",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% Color Alpha Blending with Colorize */
    { "Porter-Duff Source Over Blit with 50% Color Alpha Blending with Colorize",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% Color Alpha Blending with Colorize */
    { "Porter-Duff Source Over Blit with 25% Color Alpha Blending with Colorize",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test 100% Color Alpha + Alpha Channel Blending + Colorize */
    { "Porter-Duff Source Over Blit with 100% Color Alpha + Source Alpha Blending + Colorize",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 100% Color Alpha + Alpha Channel Blending + Colorize */
   { "Porter-Duff Source Over Blit with 50% Color Alpha + Source Alpha Blending + Colorize",
       DSBLIT_BLEND_COLORALPHA | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_COLORIZE,
       DSDRAW_BLEND,
       DSPD_SRC_OVER,
       BLT_COLOR_A_HALF,
       blit_porter_duff},

   /* Test 100% Color Alpha + Alpha Channel Blending + Colorize */
   { "Porter-Duff Source Over Blit with 25% Color Alpha + Source Alpha Blending + Colorize",
       DSBLIT_BLEND_COLORALPHA | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_COLORIZE,
       DSDRAW_BLEND,
       DSPD_SRC_OVER,
       BLT_COLOR_A_QUAR,
       blit_porter_duff},

    /* Test PREMULTIPLY with Colorize */
    { "Porter-Duff Source Over Blit with Premultiply Source with Colorize",
        DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test Alpha Channel + PREMULTIPLY Blending + Colorize */
    { "Porter-Duff Source Over Blit with Premultiply Source Alpha Blending with Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY Blending + Colorize */
    { "Porter-Duff Source Over Blit with Premultiply Source Alpha + 100% Alpha Color blending + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% Alpha Color + Alpha Channel + PREMULTIPLY Blending + Colorize */
    { "Porter-Duff Source Over Blit with Premultiply Source Alpha + 50% Alpha Color blending + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% Alpha Color + Alpha Channel + PREMULTIPLY Blending + Colorize */
    { "Porter-Duff Source Over Blit with Premultiply Source Alpha + 25% Alpha Color blending + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test 100% PREMULTCOLOR with Colorize */
    { "Porter-Duff Source Over Blit with 100% PREMULTCOLOR on the Source with Colorize",
        DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% PREMULTCOLOR with Colorize */
    { "Porter-Duff Source Over Blit with 50% PREMULTCOLOR on the Source with Colorize",
        DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% PREMULTCOLOR with Colorize */
    { "Porter-Duff Source Over Blit with 25% PREMULTCOLOR on the Source with Colorize",
        DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test 100% Alpha Color + Alpha Channel + 100% PREMULTCOLOR Blending + Colorize */
    { "Porter-Duff Source Over Blit with Source Alpha + 100% Alpha Color + 100% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% Alpha Color + Alpha Channel + 50% PREMULTCOLOR Blending + Colorize */
    { "Porter-Duff Source Over Blit with Source Alpha + 50% Alpha Color + 50% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 250% Alpha Color + Alpha Channel + 25% PREMULTCOLOR Blending + Colorize */
    { "Porter-Duff Source Over Blit with Source Alpha + 25% Alpha Color + 25% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY + 100% PREMULTCOLOR Blending + Colorize */
    { "Porter-Duff Source Over Blit with Premultiply Source Alpha + 100% Alpha Color + 100% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% Alpha Color + Alpha Channel + PREMULTIPLY + 50% PREMULTCOLOR Blending + Colorize */
    { "Porter-Duff Source Over Blit with Premultiply Source Alpha + 50% Alpha Color + 50% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% Alpha Color + Alpha Channel + PREMULTIPLY + 25% PREMULTCOLOR Blending + Colorize */
    { "Porter-Duff Source Over Blit with Premultiply Source Alpha + 25% Alpha Color + 25% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /*** Test with Porter-Duff Destination Over */
     /* Test Flip Horizontally */
    { "Porter-Duff Dest. Over Blit with Horizontal Flip",
        DSBLIT_FLIP_HORIZONTAL,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

     /* Test Flip Vertically */
    { "Porter-Duff Dest. Over Blit with Vertical Flip",
        DSBLIT_FLIP_VERTICAL,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

     /* Test Rotate 180 */
    { "Porter-Duff Dest. Over Blit with 180 Degrees Rotation",
        DSBLIT_ROTATE180,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

     /* Test Flip Horizontally with Alpha Channel Blending */
    { "Porter-Duff Dest. Over Blit with Horizontal Flip and Alpha Channel Blending",
        DSBLIT_FLIP_HORIZONTAL | DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

     /* Test Flip Vertically with Alpha Channel Blending */
    { "Porter-Duff Dest. Over Blit with Vertical Flip and Alpha Channel Blending",
        DSBLIT_FLIP_VERTICAL | DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

     /* Test Rotate 180 with Alpha Channel Blending */
    { "Porter-Duff Dest. Over Blit with 180 Degrees Rotation and Alpha Channel Blending",
        DSBLIT_ROTATE180 | DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

     /* Test XOR */
    { "Porter-Duff Dest. Over Blit with XOR",
        DSBLIT_XOR,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

     /* Test XOR with Alpha Channel Blending */
    { "Porter-Duff Dest. Over Blit with XOR and Alpha Channel Blending",
        DSBLIT_XOR | DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test XOR with Color Alpha Blending */
    { "Porter-Duff Dest. Over Blit with XOR and Color Alpha Blending",
        DSBLIT_XOR | DSBLIT_BLEND_COLORALPHA,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test XOR with Alpha Channel and Color Alpha Blending */
    { "Porter-Duff Dest. Over Blit with XOR and Alpha Channel + Color Alpha Blending",
        DSBLIT_XOR | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test XOR with Premultiply and Alpha Channel and Color Alpha Blending */
    { "Porter-Duff Dest. Over Blit with XOR and Premultiply + Alpha Channel + Color Alpha Blending",
        DSBLIT_XOR | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test XOR with Premultiply and Alpha Channel and Color Alpha Blending and PremultColor */
    { "Porter-Duff Dest. Over Blit with XOR + Premultiply + Alpha Channel + Color Alpha + PREMULTCOLOR Blending",
        DSBLIT_XOR | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test XOR with Premultiply and Alpha Channel and Color Alpha Blending and PremultColor and Colorize */
    { "Porter-Duff Dest. Over Blit with XOR + Premultiply + Alpha Channel + Color Alpha + PREMULTCOLOR + Colorize Blending",
        DSBLIT_XOR | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test Alpha Channel Blending */
    { "Porter-Duff Dest. Over Blit with Alpha Channel Blending",
        DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

     /* Test 100% Color Alpha Blending */
    { "Porter-Duff Dest. Over Blit with 100% Color Alpha Blending",
        DSBLIT_BLEND_COLORALPHA,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

     /* Test 50% Color Alpha Blending */
    { "Porter-Duff Dest. Over Blit with 50% Color Alpha Blending",
        DSBLIT_BLEND_COLORALPHA,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test 25% Color Alpha Blending */
    { "Porter-Duff Dest. Over Blit with 25% Color Alpha Blending",
        DSBLIT_BLEND_COLORALPHA,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test 100% Color Alpha + Alpha Channel Blending */
    { "Porter-Duff Dest. Over Blit with 100% Color Alpha + Alpha Blend",
        DSBLIT_BLEND_COLORALPHA|DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% Color Alpha + Alpha Channel Blending */
    { "Porter-Duff Dest. Over Blit with 50% Color Alpha + Alpha Blend",
        DSBLIT_BLEND_COLORALPHA|DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% Color Alpha + Alpha Channel Blending */
    { "Porter-Duff Dest. Over Blit with 25% Color Alpha + Alpha Blend",
        DSBLIT_BLEND_COLORALPHA|DSBLIT_BLEND_ALPHACHANNEL,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test PREMULTIPLY */
    { "Porter-Duff Dest. Over Blit with Premultiply Source",
        DSBLIT_SRC_PREMULTIPLY,
        DSDRAW_BLEND,
        DSPD_SRC_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test Alpha Channel + PREMULTIPLY Blending */
    { "Porter-Duff Dest. Over Blit with Premultiply Source Alpha Blending",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_SRC_PREMULTIPLY,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY Blending */
    { "Porter-Duff Dest. Over Blit with Premultiply Source Alpha + 100% Alpha Color blending",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

     /* Test 50% Alpha Color + Alpha Channel + PREMULTIPLY Blending */
    { "Porter-Duff Dest. Over Blit with Premultiply Source Alpha + 50% Alpha Color blending",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

     /* Test 25% Alpha Color + Alpha Channel + PREMULTIPLY Blending */
    { "Porter-Duff Dest. Over Blit with Premultiply Source Alpha + 25% Alpha Color blending",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test 100% PREMULTCOLOR */
    { "Porter-Duff Dest. Over Blit with 100% PREMULTCOLOR on the Source",
        DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% PREMULTCOLOR */
    { "Porter-Duff Dest. Over Blit with 50% PREMULTCOLOR on the Source",
        DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% PREMULTCOLOR */
    { "Porter-Duff Dest. Over Blit with 25% PREMULTCOLOR on the Source",
        DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test 100% Alpha Color + Alpha Channel + 100% PREMULTCOLOR Blending */
    { "Porter-Duff Dest. Over Blit with Source Alpha + 100% Alpha Color + 100% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 100% Alpha Color + Alpha Channel + 100% PREMULTCOLOR Blending */
    { "Porter-Duff Dest. Over Blit with Source Alpha + 50% Alpha Color + 50% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 100% Alpha Color + Alpha Channel + 100% PREMULTCOLOR Blending */
    { "Porter-Duff Dest. Over Blit with Source Alpha + 25% Alpha Color + 25% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY + 100% PREMULTCOLOR Blending */
    { "Porter-Duff Dest. Over Blit with Premultiply Source Alpha + 100% Alpha Color + 100% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY + 100% PREMULTCOLOR Blending */
    { "Porter-Duff Dest. Over Blit with Premultiply Source Alpha + 50% Alpha Color + 50% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY + 100% PREMULTCOLOR Blending */
    { "Porter-Duff Dest. Over Blit with Premultiply Source Alpha + 25% Alpha Color + 25% PREMULTCOLOR",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test Alpha Channel Blending with Colorize */
    { "Porter-Duff Dest. Over Blit with Source Alpha Blending with Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 100% Color Alpha Blending with Colorize */
    { "Porter-Duff Dest. Over Blit with 100% Color Alpha Blending with Colorize",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% Color Alpha Blending with Colorize */
    { "Porter-Duff Dest. Over Blit with 50% Color Alpha Blending with Colorize",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% Color Alpha Blending with Colorize */
    { "Porter-Duff Dest. Over Blit with 25% Color Alpha Blending with Colorize",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test 100% Color Alpha + Alpha Channel Blending + Colorize */
    { "Porter-Duff Dest. Over Blit with 100% Color Alpha + Source Alpha Blending + Colorize",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% Color Alpha + Alpha Channel Blending + Colorize */
    { "Porter-Duff Dest. Over Blit with 50% Color Alpha + Source Alpha Blending + Colorize",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% Color Alpha + Alpha Channel Blending + Colorize */
    { "Porter-Duff Dest. Over Blit with 25% Color Alpha + Source Alpha Blending + Colorize",
        DSBLIT_BLEND_COLORALPHA | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test PREMULTIPLY with Colorize */
    { "Porter-Duff Dest. Over Blit with Premultiply Source with Colorize",
        DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test Alpha Channel + PREMULTIPLY Blending + Colorize */
    { "Porter-Duff Dest. Over Blit with Premultiply Source Alpha Blending with Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY Blending + Colorize */
    { "Porter-Duff Dest. Over Blit with Premultiply Source Alpha + 100% Alpha Color blending + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% Alpha Color + Alpha Channel + PREMULTIPLY Blending + Colorize */
    { "Porter-Duff Dest. Over Blit with Premultiply Source Alpha + 50% Alpha Color blending + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% Alpha Color + Alpha Channel + PREMULTIPLY Blending + Colorize */
    { "Porter-Duff Dest. Over Blit with Premultiply Source Alpha + 25% Alpha Color blending + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test 100% PREMULTCOLOR with Colorize */
    { "Porter-Duff Dest. Over Blit with 100% PREMULTCOLOR on the Source with Colorize",
        DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% PREMULTCOLOR with Colorize */
    { "Porter-Duff Dest. Over Blit with 50% PREMULTCOLOR on the Source with Colorize",
        DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% PREMULTCOLOR with Colorize */
    { "Porter-Duff Dest. Over Blit with 25% PREMULTCOLOR on the Source with Colorize",
        DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test 100% Alpha Color + Alpha Channel + 100% PREMULTCOLOR Blending + Colorize */
    { "Porter-Duff Dest. Over Blit with Source Alpha + 100% Alpha Color + 100% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% Alpha Color + Alpha Channel + 50% PREMULTCOLOR Blending + Colorize */
    { "Porter-Duff Dest. Over Blit with Source Alpha + 50% Alpha Color + 50% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% Alpha Color + Alpha Channel + 25% PREMULTCOLOR Blending + Colorize */
    { "Porter-Duff Dest. Over Blit with Source Alpha + 25% Alpha Color + 25% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},

    /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY + 100% PREMULTCOLOR Blending + Colorize */
    { "Porter-Duff Dest. Over Blit with Premultiply Source Alpha + 100% Alpha Color + 100% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A,
        blit_porter_duff},

    /* Test 50% Alpha Color + Alpha Channel + PREMULTIPLY + 50% PREMULTCOLOR Blending + Colorize */
    { "Porter-Duff Dest. Over Blit with Premultiply Source Alpha + 50% Alpha Color + 50% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_HALF,
        blit_porter_duff},

    /* Test 25% Alpha Color + Alpha Channel + PREMULTIPLY + 25% PREMULTCOLOR Blending + Colorize */
    { "Porter-Duff Dest. Over Blit with Premultiply Source Alpha + 25% Alpha Color + 25% PREMULTCOLOR + Colorize",
        DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
        DSDRAW_BLEND,
        DSPD_DST_OVER,
        BLT_COLOR_A_QUAR,
        blit_porter_duff},
};


static Test_t stretch_tests[] = {
    /*** Test using Stretch Blit ***/

    /* Test Flip Horizontally */
   { "Stretch Blit with Horizontal Flip",
      DSBLIT_FLIP_HORIZONTAL|0/*DSBLIT_SRC_COLORKEY*/,
      DSDRAW_BLEND,
      DSPD_NONE,
      BLT_COLOR_A,
      blit_stretch},

    /* Test Flip Vertically */
   { "Stretch Blit with Vertical Flip",
      DSBLIT_FLIP_VERTICAL|0/*DSBLIT_SRC_COLORKEY*/,
      DSDRAW_BLEND,
      DSPD_NONE,
      BLT_COLOR_A,
      blit_stretch},

    /* Test Rotate 180 */
   { "Stretch Blit with 180 Degrees Rotation",
      DSBLIT_ROTATE180|0/*DSBLIT_SRC_COLORKEY*/,
      DSDRAW_BLEND,
      DSPD_NONE,
      BLT_COLOR_A,
      blit_stretch},

    /* Test XOR */
   { "Stretch Blit with XOR",
      DSBLIT_XOR|0/*DSBLIT_SRC_COLORKEY*/,
      DSDRAW_BLEND,
      DSPD_NONE,
      BLT_COLOR_A,
      blit_stretch},

    /* Test Alpha Channel Blending */
   { "Stretch Blit with Alpha Channel Blending",
      DSBLIT_BLEND_ALPHACHANNEL|0/*DSBLIT_SRC_COLORKEY*/,
      DSDRAW_BLEND,
      DSPD_NONE,
      BLT_COLOR_A,
      blit_stretch},

    /* Test 100% Color Alpha Blending */
   { "Stretch 100% Color Alpha Blending",
      DSBLIT_BLEND_COLORALPHA|0/*DSBLIT_SRC_COLORKEY*/,
      DSDRAW_BLEND,
      DSPD_NONE,
      BLT_COLOR_A,
      blit_stretch},

    /* Test 50% Color Alpha Blending */
   { "Stretch 50% Color Alpha Blending",
       DSBLIT_BLEND_COLORALPHA|0/*DSBLIT_SRC_COLORKEY*/,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_HALF,
       blit_stretch},

    /* Test 25% Color Alpha Blending */
   { "Stretch 25% Color Alpha Blending",
       DSBLIT_BLEND_COLORALPHA|0/*DSBLIT_SRC_COLORKEY*/,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_QUAR,
       blit_stretch},

    /* Test 100% Color Alpha + Alpha Channel Blending */
   { "Stretch 100% Color Alpha + Alpha Blend",
       DSBLIT_BLEND_COLORALPHA|DSBLIT_BLEND_ALPHACHANNEL|0/*DSBLIT_SRC_COLORKEY*/,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A,
       blit_stretch},

    /* Test 50% Color Alpha + Alpha Channel Blending */
   { "Stretch 50% Color Alpha + Alpha Blend",
       DSBLIT_BLEND_COLORALPHA|DSBLIT_BLEND_ALPHACHANNEL|0/*DSBLIT_SRC_COLORKEY*/,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_HALF,
       blit_stretch},

    /* Test 25% Color Alpha + Alpha Channel Blending */
   { "Stretch 25% Color Alpha + Alpha Blend",
       DSBLIT_BLEND_COLORALPHA|DSBLIT_BLEND_ALPHACHANNEL|0/*DSBLIT_SRC_COLORKEY*/,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_QUAR,
       blit_stretch},

    /* Test PREMULTIPLY */
   { "Stretch Premultiply Source",
       DSBLIT_SRC_PREMULTIPLY|0/*DSBLIT_SRC_COLORKEY*/,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_HALF,
       blit_stretch},

    /* Test Alpha Channel + PREMULTIPLY Blending */
   { "Stretch Premultiply Source Alpha Blending",
       DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_SRC_PREMULTIPLY|0/*DSBLIT_SRC_COLORKEY*/,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_HALF,
       blit_stretch},

    /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY Blending */
   { "Stretch Premultiply Source Alpha + 100% Alpha Color blending",
       DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY|0/*DSBLIT_SRC_COLORKEY*/,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A,
       blit_stretch},

    /* Test 50% Alpha Color + Alpha Channel + PREMULTIPLY Blending */
   { "Stretch Premultiply Source Alpha + 50% Alpha Color blending",
       DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY|0/*DSBLIT_SRC_COLORKEY*/,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_HALF,
       blit_stretch},

    /* Test 25% Alpha Color + Alpha Channel + PREMULTIPLY Blending */
   { "Stretch Premultiply Source Alpha + 25% Alpha Color blending",
       DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY|0/*DSBLIT_SRC_COLORKEY*/,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_QUAR,
       blit_stretch},

    /* Test 100% PREMULTCOLOR */
   { "Stretch 100% PREMULTCOLOR on the Source",
       DSBLIT_SRC_PREMULTCOLOR,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A,
       blit_stretch},

    /* Test 50% PREMULTCOLOR */
   { "Stretch 50% PREMULTCOLOR on the Source",
       DSBLIT_SRC_PREMULTCOLOR,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_HALF,
       blit_stretch},

    /* Test 25% PREMULTCOLOR */
   { "Stretch 25% PREMULTCOLOR on the Source",
       DSBLIT_SRC_PREMULTCOLOR,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_QUAR,
       blit_stretch},

    /* Test 100% Alpha Color + Alpha Channel + 100% PREMULTCOLOR Blending */
   { "Stretch Source Alpha + 100% Alpha Color + 100% PREMULTCOLOR",
       DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A,
       blit_stretch},

    /* Test 50% Alpha Color + Alpha Channel + 50% PREMULTCOLOR Blending */
   { "Stretch Source Alpha + 50% Alpha Color + 50% PREMULTCOLOR",
       DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_HALF,
       blit_stretch},

    /* Test 25% Alpha Color + Alpha Channel + 25% PREMULTCOLOR Blending */
   { "Stretch Source Alpha + 25% Alpha Color + 25% PREMULTCOLOR",
       DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_QUAR,
       blit_stretch},

    /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY + 100% PREMULTCOLOR Blending */
   { "Stretch Premultiply Source Alpha + 100% Alpha Color + 100% PREMULTCOLOR",
       DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A,
       blit_stretch},

    /* Test 50% Alpha Color + Alpha Channel + PREMULTIPLY + 50% PREMULTCOLOR Blending */
   { "Stretch Premultiply Source Alpha + 50% Alpha Color + 50% PREMULTCOLOR",
       DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_HALF,
       blit_stretch},

    /* Test 25% Alpha Color + Alpha Channel + PREMULTIPLY + 25% PREMULTCOLOR Blending */
   { "Stretch Premultiply Source Alpha + 25% Alpha Color + 25% PREMULTCOLOR",
       DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_QUAR,
       blit_stretch},

    /* Test Alpha Channel Blending with Colorize */
   { "Stretch Source Alpha Blending with Colorize",
       DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_COLORIZE,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_HALF,
       blit_stretch},

    /* Test 100% Color Alpha Blending with Colorize */
   { "Stretch 100% Color Alpha Blending with Colorize",
       DSBLIT_BLEND_COLORALPHA | DSBLIT_COLORIZE,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A,
       blit_stretch},

    /* Test 50% Color Alpha Blending with Colorize */
   { "Stretch 50% Color Alpha Blending with Colorize",
       DSBLIT_BLEND_COLORALPHA | DSBLIT_COLORIZE,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_HALF,
       blit_stretch},

    /* Test 25% Color Alpha Blending with Colorize */
   { "Stretch 25% Color Alpha Blending with Colorize",
       DSBLIT_BLEND_COLORALPHA | DSBLIT_COLORIZE,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_QUAR,
       blit_stretch},

    /* Test 100% Color Alpha + Alpha Channel Blending + Colorize */
   { "Stretch 100% Color Alpha + Source Alpha Blending + Colorize",
       DSBLIT_BLEND_COLORALPHA | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_COLORIZE,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A,
       blit_stretch},

    /* Test 50% Color Alpha + Alpha Channel Blending + Colorize */
   { "Stretch 50% Color Alpha + Source Alpha Blending + Colorize",
       DSBLIT_BLEND_COLORALPHA | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_COLORIZE,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_HALF,
       blit_stretch},

    /* Test 25% Color Alpha + Alpha Channel Blending + Colorize */
   { "Stretch 25% Color Alpha + Source Alpha Blending + Colorize",
       DSBLIT_BLEND_COLORALPHA | DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_COLORIZE,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_QUAR,
       blit_stretch},

    /* Test PREMULTIPLY with Colorize */
   { "Stretch Premultiply Source with Colorize",
       DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_HALF,
       blit_stretch},

    /* Test Alpha Channel + PREMULTIPLY Blending + Colorize */
   { "Stretch Premultiply Source Alpha Blending with Colorize",
       DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_HALF,
       blit_stretch},

    /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY Blending + Colorize */
   { "Stretch Premultiply Source Alpha + 100% Alpha Color blending + Colorize",
       DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A,
       blit_stretch},

    /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY Blending + Colorize */
   { "Stretch Premultiply Source Alpha + 50% Alpha Color blending + Colorize",
       DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_HALF,
       blit_stretch},

    /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY Blending + Colorize */
   { "Stretch Premultiply Source Alpha + 25% Alpha Color blending + Colorize",
       DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTIPLY | DSBLIT_COLORIZE,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_QUAR,
       blit_stretch},

    /* Test 100% PREMULTCOLOR with Colorize */
   { "Stretch 100% PREMULTCOLOR on the Source with Colorize",
       DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A,
       blit_stretch},

    /* Test 50% PREMULTCOLOR with Colorize */
   { "Stretch 50% PREMULTCOLOR on the Source with Colorize",
       DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_HALF,
       blit_stretch},

    /* Test 25% PREMULTCOLOR with Colorize */
   { "Stretch 25% PREMULTCOLOR on the Source with Colorize",
       DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_QUAR,
       blit_stretch},

    /* Test 100% Alpha Color + Alpha Channel + 100% PREMULTCOLOR Blending + Colorize */
   { "Stretch Source Alpha + 100% Alpha Color + 100% PREMULTCOLOR + Colorize",
       DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A,
       blit_stretch},

    /* Test 50% Alpha Color + Alpha Channel + 50% PREMULTCOLOR Blending + Colorize */
   { "Stretch Source Alpha + 50% Alpha Color + 50% PREMULTCOLOR + Colorize",
       DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_HALF,
       blit_stretch},

    /* Test 25% Alpha Color + Alpha Channel + 25% PREMULTCOLOR Blending + Colorize */
   { "Stretch Source Alpha + 25% Alpha Color + 25% PREMULTCOLOR + Colorize",
       DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_QUAR,
       blit_stretch},

    /* Test 100% Alpha Color + Alpha Channel + PREMULTIPLY + 100% PREMULTCOLOR Blending + Colorize */
   { "Stretch Premultiply Source Alpha + 100% Alpha Color + 100% PREMULTCOLOR + Colorize",
       DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA |  DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A,
       blit_stretch},

    /* Test 50% Alpha Color + Alpha Channel + PREMULTIPLY + 50% PREMULTCOLOR Blending + Colorize */
   { "Stretch Premultiply Source Alpha + 50% Alpha Color + 50% PREMULTCOLOR + Colorize",
       DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA |  DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_HALF,
       blit_stretch},

    /* Test 25% Alpha Color + Alpha Channel + PREMULTIPLY + 25% PREMULTCOLOR Blending + Colorize */
   { "Stretch Premultiply Source Alpha + 25% Alpha Color + 25% PREMULTCOLOR + Colorize",
       DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA |  DSBLIT_SRC_PREMULTIPLY | DSBLIT_SRC_PREMULTCOLOR | DSBLIT_COLORIZE,
       DSDRAW_BLEND,
       DSPD_NONE,
       BLT_COLOR_A_QUAR,
       blit_stretch},
};


int num_blit_tests = sizeof( blit_tests ) / sizeof (Test_t);
int num_fill_tests = sizeof( fill_tests ) / sizeof (Test_t);
int num_stretch_tests = sizeof( stretch_tests ) / sizeof (Test_t);
int num_draw_tests = sizeof( draw_tests ) / sizeof (Test_t);

static void print_usage(void)
{
     printf ("Broadcom DirectFB GFX Driver Tests \n\n");
     printf ("Usage: df_brcmTest [options]\n\n");
     printf ("Options:\n\n");
     printf ("  --i <test#>               Only run specific test case (test#)\n");
     printf ("  --start <test#>         Start test from case test#\n");
     printf ("  --showall                  Print out all test cases\n");
     printf ("  --help                      Print usage information.\n");
     printf ("  --dfb-help                Output DirectFB usage information.\n\n");
     printf ("\n");
}

static void print_testcases(void)
{
    int test_number = 1;
    int index = 0;

    for (index = 0;index < num_fill_tests ;index ++, test_number ++)
        printf("Test %d: %s\n", test_number,  fill_tests[index].desc);

    for (index = 0;index < num_blit_tests ;index ++, test_number ++)
        printf("Test %d: %s\n", test_number,  blit_tests[index].desc);

    for (index = 0;index < num_stretch_tests ;index ++, test_number ++)
        printf("Test %d: %s\n", test_number,  stretch_tests[index].desc);

    for (index = 0;index < num_draw_tests;index ++, test_number ++)
        printf("Test %d: %s\n", test_number,  draw_tests[index].desc);

}


static void shutdown_(void)
{
    font->Release( font );
    test_surface->Release( test_surface );
    logo_surface->Release( logo_surface );
    if (dsc.pixelformat == DSPF_LUT8)
    {
        pngtest2_surface->Release( pngtest2_surface );
#ifndef USE_IMAGE_LUT8_FILE
        biglogo_surface->Release( biglogo_surface );
#endif
    }
    background->Release( background );
    primary_info->Release( primary_info );
    primary_hardware->Release( primary_hardware );
    primary_software->Release( primary_software );

#ifdef DUAL_OUTPUT
    secondary_layer->Release( secondary_layer );
#endif
    key_events->Release( key_events );
    primary->Release( primary );
    dfb->Release( dfb );
}

static inline int get_user_input(void)
{
    DFBInputEvent ev;
    int ret = 0;

#ifdef BUILD_AUTOMATION
    return 1;
#endif

    while (1)
    {
        if (key_events->GetEvent( key_events, DFB_EVENT(&ev) ) == DFB_OK)
        {
            if (ev.type == DIET_KEYPRESS) {
                if  (ev.key_symbol == DIKS_RETURN ||
                     ev.key_symbol == DIKS_OK ||
                     ev.key_symbol == DIKS_SELECT ||
                     ev.key_symbol == DIKS_CURSOR_RIGHT ||
                        ev.key_symbol == DIKS_CURSOR_DOWN ||
                        ev.key_symbol == DIKS_PAGE_DOWN)
                        {
                            ret = 1;
                         break;
                        }
                if  (ev.key_symbol == DIKS_CURSOR_LEFT ||
                        ev.key_symbol == DIKS_CURSOR_UP ||
                        ev.key_symbol == DIKS_PAGE_UP)
                        {
                            ret = -1;
                         break;
                        }
                if  (ev.key_symbol == DIKS_ESCAPE ||
                      ev.key_symbol == DIKS_SMALL_Q ||
                      ev.key_symbol == DIKS_CAPITAL_Q ||
                      ev.key_symbol == DIKS_BACK ||
                      ev.key_symbol == DIKS_STOP ||
                      ev.key_symbol == DIKS_EXIT)
                {
                    shutdown_();
                    exit( 42 );
                    break;
                }
            }
        }
        key_events->Reset( key_events );
        key_events->WaitForEvent( key_events );
    }
    return ret;
}

static IDirectFBSurface *create_context(IDirectFBSurface *surface,
                                        int x, int y, int width, int height)
{
   DFBResult err;
   IDirectFBSurface *context;
   DFBRectangle rect = {x, y, width, height};

   DFBCHECK(surface->GetSubSurface(surface, &rect, &context));

   return context;
}

#ifdef BUILD_AUTOMATION
static DFBBoolean compare_surfaces(IDirectFBSurface *surface1, IDirectFBSurface *surface2)
{
     DFBResult err;
     int   w1, h1, w2, h2;
     void *ptr1, *ptr2;
     int   pitch1,  pitch2, x, y;
     u8   *p8_1, *p8_2;
     u16  *p16_1, *p16_2;
     u32  *p32_1, *p32_2;
     DFBColor c1, c2;
     int   diff_a, diff_r, diff_g, diff_b, deviate_rgb;
     int   tolerance_alpha = 5*255/100;     /*5% tolerance for both alpha & rgb*/
     int   tolerance_rgb = 5*255/100;
     DFBBoolean ret = DFB_TRUE;

     DFBSurfacePixelFormat f1;
     DFBSurfacePixelFormat f2;

     DFBCHECK(surface1->GetPixelFormat(surface1, &f1));
     DFBCHECK(surface2->GetPixelFormat(surface2, &f2));

     DFBCHECK(surface1->GetSize(surface1, &w1, &h1));
     DFBCHECK(surface2->GetSize(surface2, &w2, &h2));

     if( ( w1 != w2 ) || ( h1 != h2 )  || ( f1 != f1 ))
         return DFB_FALSE;

     DFBCHECK(surface1->Lock(surface1, DSLF_READ, &ptr1, &pitch1 ));
     DFBCHECK(surface2->Lock(surface2, DSLF_READ, &ptr2, &pitch2 ));

     if ( f1 == DSPF_LUT8 ){ /*only 8bits Palette is used in this test*/
         for(y=0; y < h1; y++){
             for (x=0; x < w1; x++){
                 p8_1 =  (u8*) ptr1 + x + y*pitch1;
                 p8_2 =  (u8*) ptr2 + x + y*pitch1;

                 if(*p8_1 - *p8_2){
                     ret =  DFB_FALSE;
                     break;
                 }
             }
         }
     }else{  /*DSPF_ARGB*/
         for(y=0; y < h1; y++){
             for (x=0; x < w1; x++){
                 switch(DFB_BYTES_PER_PIXEL(f1)){
                     case 1:{
                          p8_1 = (u8*) ptr1 + x + y*pitch1;
                          p8_2 = (u8*) ptr2 + x + y*pitch2;
                          dfb_pixel_to_color( f1, *p8_1, &c1 );
                          dfb_pixel_to_color( f2, *p8_2, &c2 );
                          break;
                     }
                     case 2:{
                          p16_1 = (u16*) ((u8*)ptr1 + x*2 + y*pitch1);
                          p16_2 = (u16*) ((u8*)ptr2 + x*2 + y*pitch2);
                          dfb_pixel_to_color( f1, *p16_1, &c1 );
                          dfb_pixel_to_color( f2, *p16_2, &c2 );
                          break;
                     }
                     case 4:{
                          p32_1 = (u32*) ((u8*)ptr1 + x*4 + y*pitch1);
                          p32_2 = (u32*) ((u8*)ptr2 + x*4 + y*pitch2);
                          dfb_pixel_to_color( f1, *p32_1, &c1 );
                          dfb_pixel_to_color( f2, *p32_2, &c2 );
                          break;
                     }
                     default:
                          printf("@@@ unknow pixel format\n");
                 }

                 diff_a = abs( (int) c1.a - c2.a );
                 diff_r = abs( (int) c1.r - c2.r );
                 diff_g = abs( (int) c1.g - c2.g );
                 diff_b = abs( (int) c1.b - c2.b );
                 deviate_rgb   = sqrt( diff_r * diff_r + diff_g * diff_g + diff_b * diff_b );

                 if(diff_a > tolerance_alpha || deviate_rgb > tolerance_rgb){
                     ret =  DFB_FALSE;
                     break;
                 }
             }
         }
     }

     DFBCHECK(surface1->Unlock(surface1));
     DFBCHECK(surface2->Unlock(surface2));
     return ret;
}
#endif

static void flip_context(IDirectFBSurface *info_surface, IDirectFBSurface *hardware_surface, IDirectFBSurface *software_surface)
{
    DFBResult err;

    DFBCHECK(hardware_surface->Flip(info_surface,     NULL, DSFLIP_BLIT));
    DFBCHECK(hardware_surface->Flip(hardware_surface, NULL, DSFLIP_BLIT));
    DFBCHECK(software_surface->Flip(software_surface, NULL, DSFLIP_BLIT));
#ifdef BUILD_AUTOMATION
    if (!compare_surfaces(hardware_surface, software_surface)){
         failure++;
         printf("@@@ The two surfaces do not match.\n");
    }
#endif
}

static void display_information(int i, char *title)
{
    DFBResult err;
    char buf[81];
    char string[128];

    if (i)
        sprintf(string, "Test %d: %s", i, title);
    else
        sprintf(string, "%s",title);

    DFBCHECK(primary_info->Clear(primary_info, BG_COLOR_R, BG_COLOR_G, BG_COLOR_B, BG_COLOR_A));
    DFBCHECK(primary_info->SetDrawingFlags(primary_info, DSDRAW_BLEND));
    DFBCHECK(primary_info->SetColor(primary_info, 0xff, 0xff, 0xff, 0xff));
    sprintf(buf, BRCM_MSG VERSION);
    DFBCHECK(primary_info->DrawString(primary_info, buf, -1, 0, 0, DSTF_TOPLEFT));
    DFBCHECK(primary_info->DrawString(primary_info, string, -1, 0, 30, DSTF_TOPLEFT));
    printf("%s\n", string);
}

static void prepare_test_surface(IDirectFBSurface *surface, bool isHwSurface)
{
    DFBResult err;
    DFBRectangle rect;

    rect.x = 0;
    rect.y = 0;
    rect.w = SUB_W;
    rect.h = SUB_H;

    DFBCHECK(surface->SetPorterDuff(surface, DSPD_NONE));
    DFBCHECK(surface->Clear( surface, BG_COLOR_R, BG_COLOR_G, BG_COLOR_B, BG_COLOR_A));
    DFBCHECK(surface->SetBlittingFlags( surface, DSBLIT_BLEND_ALPHACHANNEL ));
    DFBCHECK(surface->DisableAcceleration(surface, DFXL_NONE));
    DFBCHECK(surface->StretchBlit(surface, background, NULL, &rect));
    if (!isHwSurface) {
        DFBCHECK(surface->DisableAcceleration(surface, DFXL_ALL));
    }
}

static void porter_duff_fill(IDirectFBSurface *surface, DFBSurfacePorterDuffRule rule, bool isHwSurface)
{
    DFBResult err;

    prepare_test_surface(surface, isHwSurface);

    /* Overlapping boxes with blending enabled */
    DFBCHECK(surface->SetPorterDuff(surface, rule));
    DFBCHECK(surface->SetColor(surface, 0xff, 0x00, 0x00, 0x77));
    DFBCHECK(surface->FillRectangle(surface, 50, 150, 150, 150));
    DFBCHECK(surface->SetColor(surface, 0x00, 0xff, 0x00, 0x77));
    DFBCHECK(surface->FillRectangle(surface, 150, 200, 150, 150));
    DFBCHECK(surface->SetColor(surface, 0x00, 0x00, 0xff, 0x77));
    DFBCHECK(surface->FillRectangle(surface, 100, 250, 150, 150));

    DFBCHECK(surface->Flip(surface, NULL, DSFLIP_BLIT));
}

static void setup_blit(IDirectFBSurface *surface, DFBSurfaceBlittingFlags flags, int alpha, bool isHwSurface)
{
    DFBResult err;

    prepare_test_surface( surface, isHwSurface );
    DFBCHECK(surface->SetColor( surface, BLT_COLOR_R, BLT_COLOR_G, BLT_COLOR_B, alpha ));
    DFBCHECK(surface->SetBlittingFlags( surface, flags | globalBlittingFlags ));
}

static void setup_fill(IDirectFBSurface *surface, DFBSurfaceDrawingFlags flags, int red, int green, int blue, int alpha, bool isHwSurface)
{
    DFBResult err;

    prepare_test_surface( surface, isHwSurface );
    DFBCHECK(surface->SetColor( surface, red, green, blue, alpha ));
    DFBCHECK(surface->SetDrawingFlags( surface, flags | globalDrawingFlags ));
}

static void porter_duff_fills(IDirectFBSurface *hardware_surface, IDirectFBSurface *software_surface, DFBSurfacePorterDuffRule rule)
{
    porter_duff_fill(hardware_surface, rule, true);
    porter_duff_fill(software_surface, rule, false);
}

static void setup_blits(IDirectFBSurface *hardware_surface, IDirectFBSurface *software_surface, DFBSurfaceBlittingFlags flags, int alpha)
{
    setup_blit(hardware_surface, flags, alpha, true);
    setup_blit(software_surface, flags, alpha, false);
}

static void setup_porter_duff_blits(IDirectFBSurface *hardware_surface, IDirectFBSurface *software_surface, DFBSurfacePorterDuffRule rule,
                                    DFBSurfaceBlittingFlags flags, int alpha)
{
    DFBResult err;

    setup_blit(hardware_surface, flags, alpha, true);
    DFBCHECK(hardware_surface->SetPorterDuff(hardware_surface, rule));
    setup_blit(software_surface, flags, alpha, false);
    DFBCHECK(software_surface->SetPorterDuff(software_surface, rule));
}

static void setup_fills(IDirectFBSurface *hardware_surface, IDirectFBSurface *software_surface, DFBSurfaceDrawingFlags flags,
                       int red, int green, int blue, int alpha)
{
    setup_fill(hardware_surface, flags, red, green, blue, alpha, true);
    setup_fill(software_surface, flags, red, green, blue, alpha, false);
}

static int fill_blending(long i, char* title, int blitFlags, int drawFlags, int porterduffFlags, int alpha, void* pLogo, void* pTest)
{
    DFBResult err;
//    DFBRectangle *pLogoRect = (DFBRectangle *)pLogo;
//    DFBRectangle *pTestRect = (DFBRectangle *)pTest;
    (void)porterduffFlags;
    (void)alpha;
    (void)pLogo;
    (void)pTest;


    /* Test Blending with Drawing Flags = DSDRAW_BLEND */
    display_information(i, title);

    /* Test Blending with Drawing Flags = DSDRAW_BLEND */
    primary_hardware->SetBlittingFlags( primary_hardware, blitFlags );
    primary_software->SetBlittingFlags( primary_software, blitFlags );

    primary_hardware->SetDrawingFlags (primary_hardware, drawFlags);
    primary_software->SetDrawingFlags (primary_software, drawFlags);

    setup_fills(primary_hardware, primary_software, drawFlags, 0xFF, 0, 0, 77);

    primary_hardware->SetColor( primary_hardware, 0xFF, 0, 0, 77 );
    primary_hardware->FillRectangle( primary_hardware, 50, 150, 150, 150 );

    DFBCHECK(primary->Flip(primary, NULL, DSFLIP_WAITFORSYNC));

    primary_software->SetColor( primary_software, 0xFF, 0, 0, 77 );
    primary_software->FillRectangle( primary_software, 50, 150, 150, 150 );

    flip_context( primary_info, primary_hardware, primary_software );

    return get_user_input();
}

static int  fill_porter_duff(long i, char* title, int blitFlags, int drawFlags, int porterduffFlags, int alpha, void* pLogo, void* pTest)
{
//    DFBRectangle * pLogoRect = (DFBRectangle *)pLogo;
//    DFBRectangle *pTestRect = (DFBRectangle *)pTest;
    (void)pLogo;
    (void)pTest;
    (void)alpha;

    /* Test Porter-Duff*/
    display_information(i, title);

    primary_hardware->SetBlittingFlags( primary_hardware, blitFlags );
    primary_software->SetBlittingFlags( primary_software, blitFlags );

    primary_hardware->SetDrawingFlags (primary_hardware, drawFlags);
    primary_software->SetDrawingFlags (primary_software, drawFlags);

    porter_duff_fills(primary_hardware, primary_software, (DFBSurfacePorterDuffRule)porterduffFlags);
    flip_context( primary_info, primary_hardware, primary_software );

    primary_hardware->SetPorterDuff(primary_hardware, DSPD_NONE);
    primary_software->SetPorterDuff(primary_software, DSPD_NONE);

    return get_user_input();
}

static int blit_normal(long i, char* title, int blitFlags, int drawFlags, int porterduffFlags, int alpha, void* pLogo, void* pTest)
{
    DFBRectangle * pLogoRect = (DFBRectangle *)pLogo;
    DFBRectangle *pTestRect = (DFBRectangle *)pTest;
    (void)drawFlags;
    (void)porterduffFlags;

    /* Test Normal Blit*/
    display_information(i, title);

    setup_blits(primary_hardware, primary_software, blitFlags, alpha);
    primary_hardware->Blit( primary_hardware, logo_surface, NULL,  pLogoRect->x, pLogoRect->y );
    primary_hardware->Blit( primary_hardware, test_surface, NULL, pTestRect->x, pTestRect->y );
    primary_software->Blit( primary_software, logo_surface, NULL,  pLogoRect->x, pLogoRect->y );
    primary_software->Blit( primary_software, test_surface, NULL, pTestRect->x, pTestRect->y );
    flip_context( primary_info, primary_hardware, primary_software );

    return get_user_input();
}

static int blit_porter_duff(long i, char* title, int blitFlags, int drawFlags, int porterduffFlags, int alpha, void* pLogo, void* pTest)
{
    DFBRectangle * pLogoRect = (DFBRectangle *)pLogo;
    DFBRectangle *pTestRect = (DFBRectangle *)pTest;
    (void)drawFlags;

    /* Test Normal Blit*/
    display_information(i, title);

    setup_porter_duff_blits(primary_hardware, primary_software, porterduffFlags, blitFlags, alpha);
    primary_hardware->Blit( primary_hardware, logo_surface, NULL,  pLogoRect->x, pLogoRect->y );
    primary_hardware->Blit( primary_hardware, test_surface, NULL, pTestRect->x, pTestRect->y );
    primary_software->Blit( primary_software, logo_surface, NULL,  pLogoRect->x, pLogoRect->y );
    primary_software->Blit( primary_software, test_surface, NULL, pTestRect->x, pTestRect->y );
    flip_context( primary_info, primary_hardware, primary_software );

    primary_hardware->SetPorterDuff(primary_hardware, DSPD_NONE);
    primary_software->SetPorterDuff(primary_software, DSPD_NONE);

    return get_user_input();
}

static int blit_stretch(long i, char* title, int blitFlags, int drawFlags, int porterduffFlags, int alpha, void* pLogo, void* pTest)
{
    DFBRectangle * pLogoRect = (DFBRectangle *)pLogo;
    DFBRectangle *pTestRect = (DFBRectangle *)pTest;
    (void)drawFlags;
    (void)porterduffFlags;

    /* Test Normal Blit*/
    display_information(i, title);

    setup_blits(primary_hardware, primary_software, blitFlags, alpha);
//    fprintf(stderr, "### Stretch Blit x,y,w,h = %d,%d,%d,%d\n", pTestRect->x, pTestRect->y, pTestRect->w, pTestRect->h);

    primary_hardware->StretchBlit( primary_hardware, logo_surface, NULL,  pLogoRect);
    primary_hardware->StretchBlit( primary_hardware, test_surface, NULL, pTestRect);
    primary_software->StretchBlit( primary_software, logo_surface, NULL,  pLogoRect);
    primary_software->StretchBlit( primary_software, test_surface, NULL, pTestRect);
    flip_context( primary_info, primary_hardware, primary_software );
    return get_user_input();
}

static int draw_text(long i, char* title, int blitFlags, int drawFlags, int porterduffFlags, int alpha, void* pFontHeight, void* pSurfDesc)
{
    DFBSurfaceDescription* pDFBSurfDesc = (DFBSurfaceDescription*)(pSurfDesc);
    int fontheight = *(int *)(pFontHeight);
    char buf[80];

    (void)porterduffFlags;
    (void)alpha;

    display_information(i, title);

    snprintf( buf, sizeof(buf), "B" );
    primary_hardware->SetBlittingFlags( primary_hardware, blitFlags );
    primary_software->SetBlittingFlags( primary_software, blitFlags );

    /* Row 1 */
    setup_fills(primary_hardware, primary_software, drawFlags, 0, 0, 60, BLT_COLOR_A);

    primary_hardware->FillRectangle( primary_hardware, 20, 20, 440, fontheight+5 );

    primary_hardware->SetColor( primary_hardware, 180, 200, 255, 0xFF );
    primary_hardware->DrawString( primary_hardware, buf, -1, 100, 20, DSTF_LEFT | DSTF_TOP );

    primary_hardware->SetColor( primary_hardware, 180, 200, 255, 0x80 );
    primary_hardware->DrawString( primary_hardware, buf, -1, 115, 20, DSTF_LEFT | DSTF_TOP );

    primary_hardware->SetColor( primary_hardware, 255, 0, 0, 0xFF );
    primary_hardware->DrawString( primary_hardware, buf, -1, 130, 20, DSTF_LEFT | DSTF_TOP );

    primary_hardware->SetColor( primary_hardware, 255, 0, 0, 0x80 );
    primary_hardware->DrawString( primary_hardware, buf, -1, 145, 20, DSTF_LEFT | DSTF_TOP );

    primary_software->FillRectangle( primary_software, 20, 20, 440, fontheight+5 );

    primary_software->SetColor( primary_software, 180, 200, 255, 0xFF );
    primary_software->DrawString( primary_software, buf, -1, 100, 20, DSTF_LEFT | DSTF_TOP );

    primary_software->SetColor( primary_software, 180, 200, 255, 0x80 );
    primary_software->DrawString( primary_software, buf, -1, 115, 20, DSTF_LEFT | DSTF_TOP );

    primary_software->SetColor( primary_software, 255, 0, 0, 0xFF );
    primary_software->DrawString( primary_software, buf, -1, 130, 20, DSTF_LEFT | DSTF_TOP );

    primary_software->SetColor( primary_software, 255, 0, 0, 0x80 );
    primary_software->DrawString( primary_software, buf, -1, 145, 20, DSTF_LEFT | DSTF_TOP );

    /* Row 2 */
    primary_hardware->SetColor( primary_hardware, 0, 0, 60, 0x80 );
    primary_hardware->FillRectangle( primary_hardware, 20, 40 + fontheight+5, 440, fontheight+5 );

    primary_hardware->SetColor( primary_hardware, 180, 200, 255, 0x80 );
    primary_hardware->DrawString( primary_hardware, buf, -1, 100, (40 + fontheight+5), DSTF_LEFT | DSTF_TOP );

    primary_hardware->SetColor( primary_hardware, 180, 200, 255, 0x40 );
    primary_hardware->DrawString( primary_hardware, buf, -1, 115, (40 + fontheight+5), DSTF_LEFT | DSTF_TOP );

    primary_hardware->SetColor( primary_hardware, 255, 0, 0, 0x80 );
    primary_hardware->DrawString( primary_hardware, buf, -1, 130, (40 + fontheight+5), DSTF_LEFT | DSTF_TOP );

    primary_hardware->SetColor( primary_hardware, 255, 0, 0, 0x40 );
    primary_hardware->DrawString( primary_hardware, buf, -1, 145, (40 + fontheight+5), DSTF_LEFT | DSTF_TOP );

    primary_software->SetColor( primary_software, 0, 0, 60, 0x80 );
    primary_software->FillRectangle( primary_software, 20, 40 + fontheight+5, 440, fontheight+5 );

    primary_software->SetColor( primary_software, 180, 200, 255, 0x80 );
    primary_software->DrawString( primary_software, buf, -1, 100, (40 + fontheight+5), DSTF_LEFT | DSTF_TOP );

    primary_software->SetColor( primary_software, 180, 200, 255, 0x40 );
    primary_software->DrawString( primary_software, buf, -1, 115, (40 + fontheight+5), DSTF_LEFT | DSTF_TOP );

    primary_software->SetColor( primary_software, 255, 0, 0, 0x80 );
    primary_software->DrawString( primary_software, buf, -1, 130, (40 + fontheight+5), DSTF_LEFT | DSTF_TOP );

    primary_software->SetColor( primary_software, 255, 0, 0, 0x40 );
    primary_software->DrawString( primary_software, buf, -1, 145, (40 + fontheight+5), DSTF_LEFT | DSTF_TOP );

    /* Row 3 */
    primary_hardware->SetColor( primary_hardware, 0, 0, 60, 0x40 );
    primary_hardware->FillRectangle( primary_hardware, 20, 60 + 2*(fontheight+5), 440, fontheight+5 );

    primary_hardware->SetColor( primary_hardware, 180, 200, 255, 0x40 );
    primary_hardware->DrawString( primary_hardware, buf, -1, 100, 60 + 2*(fontheight+5), DSTF_LEFT | DSTF_TOP );

    primary_hardware->SetColor( primary_hardware, 180, 200, 255, 0x0 );
    primary_hardware->DrawString( primary_hardware, buf, -1, 115, 60 + 2*(fontheight+5), DSTF_LEFT | DSTF_TOP );

    primary_hardware->SetColor( primary_hardware, 255, 0, 0, 0x40 );
    primary_hardware->DrawString( primary_hardware, buf, -1, 130, 60 + 2*(fontheight+5), DSTF_LEFT | DSTF_TOP );

    primary_hardware->SetColor( primary_hardware, 255, 0, 0, 0x0 );
    primary_hardware->DrawString( primary_hardware, buf, -1, 145, 60 + 2*(fontheight+5), DSTF_LEFT | DSTF_TOP );

    primary_software->SetColor( primary_software, 0, 0, 60, 0x40 );
    primary_software->FillRectangle( primary_software, 20, 60 + 2*(fontheight+5), 440, fontheight+5 );

    primary_software->SetColor( primary_software, 180, 200, 255, 0x40 );
    primary_software->DrawString( primary_software, buf, -1, 100, 60 + 2*(fontheight+5), DSTF_LEFT | DSTF_TOP );

    primary_software->SetColor( primary_software, 180, 200, 255, 0x0 );
    primary_software->DrawString( primary_software, buf, -1, 115, 60 + 2*(fontheight+5), DSTF_LEFT | DSTF_TOP );

    primary_software->SetColor( primary_software, 255, 0, 0, 0x40 );
    primary_software->DrawString( primary_software, buf, -1, 130, 60 + 2*(fontheight+5), DSTF_LEFT | DSTF_TOP );

    primary_software->SetColor( primary_software, 255, 0, 0, 0x0 );
    primary_software->DrawString( primary_software, buf, -1, 145, 60 + 2*(fontheight+5), DSTF_LEFT | DSTF_TOP );

    test_surface->SetDrawingFlags (test_surface, drawFlags);
    test_surface->SetColor(test_surface, 0, 0xff, 0, 0xff);
    test_surface->FillRectangle(test_surface, 0, 0, pDFBSurfDesc->width, pDFBSurfDesc->height);

    primary_hardware->SetColor(primary_hardware, 50, 100, 200, 0x80);
    primary_hardware->FillRectangle(primary_hardware, 200, 200, 100, 100);
    primary_hardware->SetBlittingFlags(primary_hardware, blitFlags);
    primary_hardware->Blit( primary_hardware, test_surface, NULL, 250, 250);

    primary_software->SetColor(primary_software, 50, 100, 200, 0x80);
    primary_software->FillRectangle(primary_software, 200, 200, 100, 100);
    primary_software->SetBlittingFlags(primary_software, blitFlags);
    primary_software->Blit( primary_software, test_surface, NULL, 250, 250);

    flip_context( primary_info, primary_hardware, primary_software );
    return get_user_input();
}



int main( int argc, char *argv[] )
{
    DFBResult err;
    IDirectFBImageProvider *provider;
    DFBSurfacePixelFormat pixelformat;
    DFBSurfaceDescription dsc_bg, dsc_logo, dsc_test_surface;
    int fontheight;
    int test_number = 0;

    DFBRectangle logo_rect, test_rect;
    DFBRectangle sLogo_rect, sTest_rect;

    int start_test = 1;
    int run_once = 0;

    int index = 0;

    int max_fill_ind = num_fill_tests;
    int max_blit_ind = max_fill_ind + num_blit_tests;
    int max_stretch_ind = max_blit_ind + num_stretch_tests;
    int max_draw_ind = max_stretch_ind + num_draw_tests;
    int max_index = max_draw_ind;

    /* Input interfaces: event buffer */
    int n;

    if (getenv("blittingflags"))
    {
        globalBlittingFlags = strtoul(getenv("blittingflags"), NULL, 0);
        fprintf(stderr, "%s: Blitting Flags = 0x%08x\n", argv[0], globalBlittingFlags);
    }

    if (getenv("drawingflags"))
    {
        globalDrawingFlags = strtoul(getenv("drawingflags"), NULL, 0);
        fprintf(stderr, "%s: Drawing Flags = 0x%08x\n", argv[0], globalDrawingFlags);
    }

     /* parse command line */
    for (n = 1; n < argc; n++)
    {
        if (strncmp (argv[n], "--", 2) == 0)
        {
            if (strcmp (argv[n] + 2, "help") == 0)
             {
                print_usage();
                return EXIT_SUCCESS;
            }
            else if (strcmp (argv[n] + 2, "showall") == 0)
            {
                print_testcases();
                return EXIT_SUCCESS;
            }
            else if ((strcmp (argv[n] + 2, "i") == 0)  &&
                        (++n < argc) &&
                        (sscanf (argv[n], "%d", &start_test) == 1))
            {
                if (start_test > max_index || start_test < 1)
                {
                    print_testcases();
                    return EXIT_SUCCESS;
                }
                  run_once = 1;
                continue;
            }
            else if ((strcmp (argv[n] + 2, "start") == 0)  &&
                        (++n < argc) &&
                        (sscanf (argv[n], "%d", &start_test) == 1))
            {
                if (start_test > max_index || start_test < 1)
                {
                    print_testcases();
                    return EXIT_SUCCESS;
                }
                continue;
            }
            else
            {
                continue;
            }
        }
        print_usage();
        return EXIT_FAILURE;
    }


    DFBCHECK(DirectFBInit( &argc, &argv ));

    DirectFBSetOption ("bg-none", NULL);

    /* create the super interface */
    DFBCHECK(DirectFBCreate( &dfb ));

    /* Set the cooperative level to DFSCL_FULLSCREEN for exclusive access to the primary layer. */
    err = dfb->SetCooperativeLevel( dfb, DFSCL_FULLSCREEN );
    if (err)
        DirectFBError( "Failed to get exclusive access", err );

    /* Get the primary surface, i.e. the surface of the primary layer. */
    dsc.flags = DSDESC_CAPS | DSDESC_PIXELFORMAT;
    dsc.caps = DSCAPS_PRIMARY;
    dsc.pixelformat = PRIMARY_PIXELFORMAT;

    /* Create the Primary Surface */
    DFBCHECK(dfb->CreateSurface( dfb, &dsc, &primary ));
    primary->GetPixelFormat( primary, &pixelformat );

    primary->GetSize( primary, &MAIN_W, &MAIN_H );

#ifdef DUAL_OUTPUT
    DFBCHECK(dfb->GetDisplayLayer(dfb, DLID_PRIMARY+1, &secondary_layer));
    DFBCHECK(secondary_layer->SetCooperativeLevel(secondary_layer, DLSCL_EXCLUSIVE));
    layerCfg.flags = DLCONF_SOURCE;
    layerCfg.source = DLSID_SURFACE;
    DFBCHECK(secondary_layer->SetConfiguration(secondary_layer, &layerCfg));
#endif

    /* create an input buffer for key events */
    DFBCHECK(dfb->CreateInputEventBuffer( dfb, DICAPS_KEYS,
                                       DFB_FALSE, &key_events ));


    /* Create Image Provider for rendering to test surface */
    if (dsc.pixelformat != DSPF_LUT8)
    {
        DFBCHECK(dfb->CreateImageProvider( dfb, DATADIR "/" TEST_IMAGE_FILE,
                                                                    &provider ));
        DFBCHECK(provider->GetSurfaceDescription( provider, &dsc_test_surface ));

        /* Create the test surface */
        dsc_test_surface.flags = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT;

        DFBCHECK(dfb->CreateSurface( dfb, &dsc_test_surface, &test_surface ));

        DFBCHECK(provider->RenderTo( provider, test_surface, NULL ));
        provider->Release(provider);

        /* Create Image Provider for rendering to logo surface */
        DFBCHECK(dfb->CreateImageProvider( dfb, DATADIR "/" LOGO_IMAGE_FILE,
                                                                    &provider ));
        DFBCHECK(provider->GetSurfaceDescription( provider, &dsc_logo ));
        dsc_logo.width  = (MAIN_H / 16) * dsc_logo.width / dsc_logo.height;
        dsc_logo.height = MAIN_H / 16;
        dsc_logo.flags = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT;

        DFBCHECK(dfb->CreateSurface( dfb, &dsc_logo, &logo_surface ));
        DFBCHECK(provider->RenderTo( provider, logo_surface, NULL ));
        provider->Release(provider);
    }
    else
    {
        IDirectFBPalette *palette;

        DFBCHECK(dfb->CreateSurface( dfb, &pngtest3_png_desc, &pngtest2_surface ));
        DFBCHECK(pngtest2_surface->GetPalette( pngtest2_surface, &palette ));

        memset(&dsc_test_surface, 0, sizeof(dsc_test_surface));
        dsc_test_surface.flags = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT;
        dsc_test_surface.width = pngtest3_png_desc.width;
        dsc_test_surface.height = pngtest3_png_desc.height;
        dsc_test_surface.pixelformat = pngtest3_png_desc.pixelformat;
        DFBCHECK(dfb->CreateSurface( dfb, &dsc_test_surface, &test_surface ));
        DFBCHECK(test_surface->SetPalette( test_surface, palette ));
        DFBCHECK(primary->SetPalette( primary, palette ));
        test_surface->Blit( test_surface, pngtest2_surface, NULL, 0, 0 );

#ifdef USE_IMAGE_LUT8_FILE
        DFBCHECK(dfb->CreateImageProvider( dfb, DATADIR "/" LOGO_IMAGE_LUT8_FILE,
                                                                    &provider ));
        DFBCHECK(provider->GetSurfaceDescription( provider, &dsc_logo ));

        /* Create the test surface */
        dsc_logo.flags = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT;
        printf("%s: pixel format=0x%08x\n", __FUNCTION__, dsc_logo.pixelformat);
        DFBCHECK(dfb->CreateSurface( dfb, &dsc_logo, &logo_surface ));

        DFBCHECK(provider->RenderTo( provider, logo_surface, NULL ));
        provider->Release(provider);
#else
        /* Create logo surface */
        DFBCHECK(dfb->CreateSurface( dfb, &biglogo_desc, &biglogo_surface ));
        memset(&dsc_logo, 0, sizeof(dsc_logo));
        dsc_logo.flags = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT;
        dsc_logo.width = biglogo_desc.width;
        dsc_logo.height = biglogo_desc.height;
        dsc_logo.pixelformat = biglogo_desc.pixelformat;
        DFBCHECK(dfb->CreateSurface( dfb, &dsc_logo, &logo_surface ));
        logo_surface->Blit( logo_surface, biglogo_surface, NULL, 0, 0 );
#endif
        palette->Release( palette );
    }

    /* Setup co-ords of where the logo and the test image will be placed within each sub-surface */
    logo_rect.x = (SUB_W - dsc_logo.width) / 4;
    logo_rect.y = SUB_H / 8;
    logo_rect.w = dsc_logo.width;
    logo_rect.h = dsc_logo.height;
    test_rect.x = logo_rect.x;
    test_rect.y = 20 + logo_rect.y;
    test_rect.w = dsc_test_surface.width;
    test_rect.h = dsc_test_surface.height;

    sLogo_rect.x = (SUB_W - biglogo_desc.width) / 8;
    sLogo_rect.y = SUB_H / 8;
    sLogo_rect.w = (biglogo_desc.width * 3) >> 1;
    sLogo_rect.h = (biglogo_desc.height * 3) >> 1;

    sTest_rect.x = sLogo_rect.x;
    sTest_rect.y = 20 + sLogo_rect.y;
    sTest_rect.w = (dsc_test_surface.width  * 3) >> 1;
    sTest_rect.h = (dsc_test_surface.height * 3) >> 1;

    /* load font */
    {
        DFBFontDescription desc;

        desc.flags = DFDESC_HEIGHT;
        desc.height = 24;

        DFBCHECK(dfb->CreateFont( dfb, FONT, &desc, &font ));
        DFBCHECK(font->GetHeight( font, &fontheight ));
        DFBCHECK(primary->SetFont( primary, font ));
    }

    /* Clear Primary Layer */
    DFBCHECK(primary->Clear( primary, BG_COLOR_R, BG_COLOR_G, BG_COLOR_B, BG_COLOR_A));
    DFBCHECK(primary->SetDrawingFlags (primary, DSDRAW_BLEND));
    DFBCHECK(primary->SetBlittingFlags( primary, DSBLIT_BLEND_ALPHACHANNEL ));
    DFBCHECK(primary->SetColor(primary, 0xff, 0xff, 0xff, 0xff));
    DFBCHECK(primary->DrawString(primary, "Hardware", -1,
                                                         SUB1_X, SUB1_Y+SUB_H, DSTF_TOPLEFT));

    DFBCHECK(primary->SetColor(primary, 0xff, 0xff, 0xff, 0xff));
    DFBCHECK(primary->DrawString(primary, "Software", -1,
                                                        SUB2_X, SUB2_Y+SUB_H, DSTF_TOPLEFT));

    DFBCHECK(primary->Flip(primary, NULL, DSFLIP_WAITFORSYNC));

     /* Create Top Test Number Surface */
    primary_info = create_context(primary, INFO_X, INFO_Y, INFO_W, INFO_H);
    DFBCHECK(primary_info->Clear( primary_info, BG_COLOR_R, BG_COLOR_G, BG_COLOR_B, BG_COLOR_A));
    DFBCHECK(primary_info->SetDrawingFlags (primary_info, DSDRAW_BLEND));
    DFBCHECK(primary_info->SetBlittingFlags( primary_info, DSBLIT_BLEND_ALPHACHANNEL ));
    DFBCHECK(primary_info->SetFont( primary_info, font ));
    DFBCHECK(primary->SetColor(primary, BOR_COLOR_R, BOR_COLOR_G, BOR_COLOR_B, BOR_COLOR_A));
    DFBCHECK(primary->DrawRectangle(primary, INFO_X-2, INFO_Y-2, INFO_W+4, INFO_H+4));

     /* Create Left Hardware Surface */
    primary_hardware = create_context(primary, SUB1_X, SUB1_Y, SUB_W, SUB_H);
    DFBCHECK(primary_hardware->Clear( primary_hardware, BG_COLOR_R, BG_COLOR_G, BG_COLOR_B, BG_COLOR_A));
    DFBCHECK(primary_hardware->SetDrawingFlags (primary_hardware, DSDRAW_BLEND));
    DFBCHECK(primary_hardware->SetBlittingFlags( primary_hardware, DSBLIT_BLEND_ALPHACHANNEL ));
    DFBCHECK(primary_hardware->DisableAcceleration(primary_hardware, DFXL_NONE));
    DFBCHECK(primary_hardware->SetFont( primary_hardware, font ));
    DFBCHECK(primary->SetColor(primary, BOR_COLOR_R, BOR_COLOR_G, BOR_COLOR_B, BOR_COLOR_A));
    DFBCHECK(primary->DrawRectangle(primary, SUB1_X-2, SUB1_Y-2, SUB_W+4, SUB_H+4));

     /* Create Right Software Surface */
    primary_software = create_context(primary, SUB2_X, SUB2_Y, SUB_W, SUB_H);
    DFBCHECK(primary_software->Clear( primary_software, BG_COLOR_R, BG_COLOR_G, BG_COLOR_B, BG_COLOR_A));
    DFBCHECK(primary_software->SetDrawingFlags (primary_software, DSDRAW_BLEND));
    DFBCHECK(primary_software->SetBlittingFlags( primary_software, DSBLIT_BLEND_ALPHACHANNEL ));
    DFBCHECK(primary_software->DisableAcceleration(primary_software, DFXL_ALL));
    DFBCHECK(primary_software->SetFont( primary_software, font ));
    DFBCHECK(primary->SetColor(primary, BOR_COLOR_R, BOR_COLOR_G, BOR_COLOR_B, BOR_COLOR_A));
    DFBCHECK(primary->DrawRectangle(primary, SUB2_X-2, SUB2_Y-2, SUB_W+4, SUB_H+4));

    /* Create Image Provider for rendering to background surface */
    DFBCHECK(dfb->CreateImageProvider( dfb, DATADIR"/fish.png",
                                                                  &provider ));
    DFBCHECK(provider->GetSurfaceDescription( provider, &dsc_bg ));

    dsc_bg.flags = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT;

    /* Create background surface */
    DFBCHECK(dfb->CreateSurface( dfb, &dsc_bg, &background ));
    DFBCHECK(provider->RenderTo( provider, background, NULL ));
    provider->Release(provider);

    /* Setup Source Color Keying */
    primary_hardware->SetSrcColorKey(primary_hardware, 0x00, 0x00, 0x00);
    primary_software->SetSrcColorKey(primary_software, 0x00, 0x00, 0x00);

    /* Setup Destination Color Keying */
    primary_hardware->SetDstColorKey(primary_hardware, 0xff, 0xff, 0xff);
    primary_software->SetDstColorKey(primary_software, 0xff, 0xff, 0xff);

    /* Setup Source Color Keying on logo and test surfaces */
    logo_surface->SetSrcColorKey(logo_surface, 0x00, 0x00, 0x00);
    test_surface->SetSrcColorKey(test_surface, 19, 9, 4);

    test_number = start_test;

    while (test_number > 0 && test_number <= max_index)
    {

#ifdef BUILD_AUTOMATION
        if ( (test_number >= skipped_tests[0] && test_number <= skipped_tests[1]) ||\
             (test_number >= skipped_tests[2] && test_number <= skipped_tests[3]) ||\
             (test_number >= skipped_tests[4] && test_number <= skipped_tests[5]) ||\
             (test_number >= skipped_tests[6] && test_number <= skipped_tests[9]) ||\
             (test_number >= skipped_tests[8] && test_number <= skipped_tests[9]) ){
             test_number++;
             continue;                  /*skip XOR and Stretch tests for automation*/
        }
#endif

        if (test_number <= max_fill_ind)
        {
            index = test_number -1;
            test_number += fill_tests[index].func(test_number,  fill_tests[index].desc,
                                                           fill_tests[index].blitFlags, fill_tests[index].drawFlags,
                                                           fill_tests[index].porterduffFlags, fill_tests[index].alpha, &logo_rect, &test_rect);

        }
        else if (test_number <= max_blit_ind)
        {
            index = test_number - max_fill_ind -1;
            test_number += blit_tests[index].func(test_number,  blit_tests[index].desc,
                                                         blit_tests[index].blitFlags, blit_tests[index].drawFlags,
                                                         blit_tests[index].porterduffFlags, blit_tests[index].alpha, &logo_rect, &test_rect);
        }
        else if (test_number <= max_stretch_ind)
        {
            index = test_number - max_blit_ind -1;
            test_number += stretch_tests[index].func(test_number,  stretch_tests[index].desc,
                                                            stretch_tests[index].blitFlags, stretch_tests[index].drawFlags,
                                                            stretch_tests[index].porterduffFlags, stretch_tests[index].alpha, &sLogo_rect, &sTest_rect);
        }
        else
        {
            index = test_number - max_stretch_ind -1;
            test_number += draw_tests[index].func(test_number,  draw_tests[index].desc,
                                                          draw_tests[index].blitFlags, draw_tests[index].drawFlags,
                                                          draw_tests[index].porterduffFlags, draw_tests[index].alpha, &fontheight, &dsc_test_surface);
        }
        if (run_once)
            break;
    }

    display_information(0, "End of All Tests - Press <ENTER> to end."); //display_information(primary_info, buf);
    flip_context( primary_info, primary_hardware, primary_software );
    get_user_input();

#ifdef BUILD_AUTOMATION
    if (failure){
        printf("@BAT_FAILURE@ df_brcmTest failed with %d failures.\n",failure);
        printf("@@@ Test Completed.\n");
        shutdown_();
        return EXIT_FAILURE;
    }else{
        printf("@BAT_SUCCESS@ df_brcmTest passed.\n");
    }
    printf("@@@ Test Completed.\n");
#endif

    shutdown_();
    return 0;
}
