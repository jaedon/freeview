/***************************************************************************
 *     (c)2012 Broadcom Corporation
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
 * $brcm_Workfile: directfb.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 9/6/12 6:08p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/nano_dfb/directfb.h $
 * 
 * Hydra_Software_Devel/2   9/6/12 6:08p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#ifndef DIRECTFB_SHIM_H__
#define DIRECTFB_SHIM_H__

/* this is a reverse-engineered shim of DFB API used by dfbgrid.c. */

#include <sys/time.h>
#include <stdbool.h>

typedef struct DFBSurfaceDescription {
    unsigned width, height;
    int caps;
    int flags;
    int pixelformat;
} DFBSurfaceDescription;

#define DSCAPS_VIDEOONLY 0
#define DSDESC_WIDTH 1
#define DSDESC_HEIGHT 2
#define DSDESC_PIXELFORMAT 4
#define DSDESC_CAPS 8
#define DSCAPS_PRIMARY 1
#define DSCAPS_DOUBLE 2
#define DSCAPS_DEPTH 4
#define DSPF_A8 0
#define DSPF_ARGB 1
#define DFB_FALSE 0
#define DFSCL_FULLSCREEN 0
#define DSLF_READ 1
#define DSLF_WRITE 2
#define DSBLIT_BLEND_ALPHACHANNEL 1
#define DSBLIT_BLEND_COLORALPHA 2
#define DSBLIT_COLORIZE  4
#define DSPD_NONE 0
#define DSPD_DST_IN 1
#define DSRO_NONE 0
#define DSFLIP_BLIT 0
#define DSFLIP_ONSYNC 0
#define DICAPS_ALL 0xFFFFFFFF
#define DIET_KEYPRESS 0
#define DIET_KEYRELEASE 1
#define DIKS_EXIT 0

typedef int DFBResult;
#define DFB_OK 0
         
typedef struct DFBRectangle {
    int x, y, w, h;
} DFBRectangle;

typedef struct DFBPoint {
    int x, y;
} DFBPoint;

typedef struct DFBInputEvent {
    int type;
    int key_symbol;
} DFBInputEvent;

#define DFB_EVENT(X) (X)
typedef struct IDirectFBEventBuffer IDirectFBEventBuffer;
struct IDirectFBEventBuffer {
    int (*GetEvent)(IDirectFBEventBuffer *ev, DFBInputEvent *event);    
    void (*Release)(IDirectFBEventBuffer *ev);
};

typedef struct IDirectFBSurface IDirectFBSurface;
typedef unsigned DFBSurfaceBlittingFlags;

struct IDirectFBSurface{
    void (*GetSize)(const IDirectFBSurface *surface, unsigned *width, unsigned *height);
    int  (*Lock)(IDirectFBSurface *surface, int flags, void **buffer, unsigned *pitch);
    void (*Unlock)(IDirectFBSurface *surface);
    void (*Clear)(IDirectFBSurface *surface, unsigned r, unsigned g, unsigned b, unsigned a);
    void (*SetColor)(IDirectFBSurface *surface, unsigned r, unsigned g, unsigned b, unsigned a);
    void (*SetBlittingFlags)(IDirectFBSurface *surface, DFBSurfaceBlittingFlags flags);
    void (*SetPorterDuff)(IDirectFBSurface *surface, int flags);
    void (*SetRenderOptions)(IDirectFBSurface *surface, int flags);
    void (*FillRectangles)(IDirectFBSurface *surface, const DFBRectangle *destRect, unsigned number);
    void (*Blit)(IDirectFBSurface *source, const IDirectFBSurface *source_surface, const DFBRectangle *sourceRect, int x, int y );
    void (*StretchBlit)(IDirectFBSurface *source, const IDirectFBSurface *source_surface, const DFBRectangle *sourceRect, const DFBRectangle *destRect);
    void (*BatchBlit)(IDirectFBSurface *surface, const IDirectFBSurface *source_surface, const DFBRectangle *sourceRect, const DFBPoint *destPoint, unsigned number);
    void (*Flip)(IDirectFBSurface *surface, void *unknown, int flags);
    void (*Release)(IDirectFBSurface *surface);
    
    struct IDirectFBSurfaceState *state;
};

typedef struct IDirectFBImageProvider IDirectFBImageProvider;
struct IDirectFBImageProvider {
    void (*GetSurfaceDescription)(IDirectFBImageProvider *, DFBSurfaceDescription *);
    int (*RenderTo)(IDirectFBImageProvider *, IDirectFBSurface *, void *unknown);
    void (*Release)(IDirectFBImageProvider *);
    
    struct IDirectFBImageProviderState *state;
};

typedef struct IDirectFB IDirectFB;
struct IDirectFB {
    int (*CreateSurface)(IDirectFB *dfb, const DFBSurfaceDescription *, IDirectFBSurface **surface);
    int (*CreateImageProvider)(IDirectFB *dfb, const char *name, IDirectFBImageProvider **provider);
    int (*SetCooperativeLevel)(IDirectFB *dfb, int flags);
    int (*CreateInputEventBuffer)(IDirectFB *dfb, int flags, int unknown, IDirectFBEventBuffer **events);
    void (*Release)(IDirectFB *dfb);
    
    struct IDirectFBState *state;
};
int DirectFBCreate(IDirectFB **dfb);

int DirectFBInit(int *argc, char ***argv);

#endif /* DIRECTFB_SHIM_H__ */
