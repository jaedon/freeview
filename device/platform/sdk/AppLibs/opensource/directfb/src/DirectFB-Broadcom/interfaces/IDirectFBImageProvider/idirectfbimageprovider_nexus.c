/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: idirectfbimageprovider_nexus.c $
 * $brcm_Revision: DirectFB_1_4_15_Port/15 $
 * $brcm_Date: 3/7/12 5:26p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/interfaces/IDirectFBImageProvider/idirectfbimageprovider_nexus.c $
 *
 * DirectFB_1_4_15_Port/15   3/7/12 5:26p christ
 * SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
 *
 * DirectFB_1_4_15_Port/14   3/6/12 12:00p mnelis
 * SW7425-2476: DirectFB: Improve Nexus SID image provider
 *
 * DirectFB_1_4_15_Port/13   2/24/12 11:48a mnelis
 * SW7425-2476: DirectFB: Improve Nexus SID image provider
 *
 * DirectFB_1_4_15_Port/12   2/24/12 11:44a mnelis
 * SW7425-2476: DirectFB: Improve Nexus SID image provider
 *
 * DirectFB_1_4_15_Port/11   1/25/12 11:35a mnelis
 * SW7425-2263: DirectFB: Segmented decoded images not displayed correctly
 *
 * DirectFB_1_4_15_Port/10   12/1/11 5:53p apandav
 * SW7425-1879: DirectFB: Enable Segmented Image decode support after
 *  checking correct magnum-SID firmware
 *
 * DirectFB_1_4_15_Port/9   11/30/11 7:49p scchang
 * SW7420-2158: DirectFB: Consider setting pthread mutex robust attribute
 *
 * DirectFB_1_4_15_Port/8   11/7/11 1:22p apandav
 * SW7425-1686: DirectFB: SID driver in non segmented decode mode doesn't
 *  scale small images correctly
 *
 * DirectFB_1_4_15_Port/7   11/4/11 12:30p robertwm
 * SW7425-1664: DirectFB: Generic IPC for hardware accelerated image
 *  providers.
 *
 * DirectFB_1_4_15_Port/6   10/20/11 11:31a christ
 * SW7425-1553: DirectFB: SID driver not working correctly on 7425 with
 *  Nexus Phase 3.5
 *
 * DirectFB_1_4_15_Port/5   10/6/11 3:45p christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.15 release.
 *
 * DirectFB_1_4_14_Port/4   9/16/11 5:27p apandav
 * SWDTV-8707: DirectFB: Update Nexus image provider in 1.4.14 branch
 *
 * DirectFB_1_4_14_Port/2   8/12/11 4:44p christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_5_Port/19   9/29/11 2:38p tross
 * SWBLURAY-27457: Change D_INFO to D_DEBUG_AT at customer's request.
 *
 * DirectFB_1_4_5_Port/18   9/27/11 5:32p tross
 * SWBLURAY-26466: Correct creation of SW ImageProvider as it was not
 * properly allocating interface and overwriting private data of
 * existing HW ImageProvider interface causing segfaults.
 *
 * DirectFB_1_4_5_Port/17   9/26/11 2:45p tross
 * SWBLURAY-26466: Replace call to dfb_gfx_stretch_to() with card state
 *  setup
 * and call to dfb_gfxcard_stretchblit so that premultiplication will
 * actually occur.
 *
 * DirectFB_1_4_5_Port/16   9/22/11 12:09p tross
 * SWBLURAY-27410: Revert to SW decode if JPEG image is progressive
 *  (multiscan).
 * The SW decode is many times faster than the SID decode.
 *
 * DirectFB_1_4_5_Port/16   9/22/11 12:09p tross
 * SWBLURAY-27410: Revert to SW decode if JPEG image is progressive
 *  (multiscan).
 * The SW decode is many times faster than the SID decode.
 *
 * DirectFB_1_4_5_Port/15   9/6/11 5:29p apandav
 * SWDTV-8549: DirectFB: Need to provide image transparency data in Nexus
 *  imageprovider
 *
 * DirectFB_1_4_5_Port/14   9/6/11 12:13p apandav
 * SWDTV-8549: DirectFB: Need to provide image transparency data in Nexus
 *  imageprovider
 *
 * DirectFB_1_4_5_Port/13   8/11/11 12:38p apandav
 * SWDTV-8228: DirectFB: Nexus Imageprovider updates
 *
 * DirectFB_1_4_5_Port/12   7/29/11 1:09p apandav
 * SW7425-1035: DirectFB: Phase 3.0 DirectFB 1.4.5 fails to build with
 *  phase 2.0 Nexus due to Imageprovider error
 *
 * DirectFB_1_4_14_Port/1   7/28/11 11:21a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_5_Port/11   7/21/11 6:15p apandav
 * SW7425-907: DirectFB: Reduce decode memory requirements for
 *  ImageProvider by striping
 *
 * DirectFB_1_4_5_Port/10   7/19/11 11:16a apandav
 * SW7425-907: DirectFB: Reduce decode memory requirements for
 *  ImageProvider by striping
 *
 * DirectFB_1_4_5_Port/9   7/18/11 4:28p apandav
 * SW7425-907: DirectFB: Reduce decode memory requirements for
 *  ImageProvider by striping
 *
 * DirectFB_1_4_5_Port/6   7/8/11 11:48a apandav
 * SWDTV-7820: DirectFB: DirectFB build fails due to Nexus image provider
 *  error
 *
 * DirectFB_1_4_5_Port/5   7/7/11 6:42p tross
 * SWBLURAY-26478: Correct colorspace default.
 *
 * DirectFB_1_4_5_Port/4   7/1/11 2:49p apandav
 * SWDTV-7760: DirectFB: Nexus image provider hangs in multi app mode
 *
 * DirectFB_1_4_5_Port/3   6/27/11 6:25p christ
 * SW7425-782: DirectFB: Latest DirectFB 1.4.5 tree fails to shutdown when
 *  running testman / testrun.
 *
 * DirectFB_1_4_5_Port/2   6/17/11 7:25p apandav
 * SWDTV-7609: DirectFB: Nexus image provder should take care of
 *  premultiply surfaces during stretch blit
 *
 * DirectFB_1_4_5_Port/1   6/14/11 5:48p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1/3   7/1/11 2:52p tross
 * SWBLURAY-26422: Merge in Bluray libinit support.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1/3   7/1/11 2:52p tross
 * SWBLURAY-26422: Merge in Bluray libinit support.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1/2   7/1/11 4:09p apandav
 * SWDTV-7760: DirectFB: Nexus image provider hangs in multi app mode
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/5   4/11/11 5:26p tross
 * Set colorspace of indexed formats to RGB.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/4   4/7/11 8:17p tross
 * Merge from latest.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1/1   6/10/11 5:08p christ
 * SW7422-138: DirecftFB: Add support for stereoscopic 3D graphics
 *  display.
 *
 * DirectFB_1_4_Port/11   4/21/11 7:54p apandav
 * SWDTV-6766: DirectFB: Allow h/w imageprovider to fallback to s/w if
 *  operation fails
 *
 * DirectFB_1_4_Port/10   3/30/11 6:20p christ
 * SW7425-254: DirectFB: Improve performance by replacing linux-fusion
 *  calls with pthreads
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/3   3/28/11 10:57a tross
 * Merge from DirectFB 1.4.1 Phase 3.01 label.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/2   3/11/11 3:38p tross
 * Forced probe to always fail for Bluray platform. This is temporary
 *  until proper integration of
 * image provider with Bluray design can take place.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1_BD_Integration/1   3/2/11 6:03p tross
 * Merge in Bluray support.
 *
 * DirectFB_1_4_Port/9   3/17/11 12:43p robertwm
 * SW7422-383: DirectFB: Need to serialise access to SID to prevent
 *  assertions.
 * SW7422-359: DirectFB: Add support for A8 Palette8 (DSPF_ALUT8) support
 *  into DirectFB.
 * SW7422-278: DirectFB: ImageProvider needs to be re-architected to use
 *  internal graphics APIs.
 *
 * DirectFB_1_4_Port/8   2/18/11 2:57p robertwm
 * SW7422-278: DirectFB: ImageProvider needs to be re-architected to use
 *  internal graphics APIs
 *
 * DirectFB_1_4_Port/7   7/26/10 5:32p jackf
 * SW35230-771: Added include for shim_nexus.h.
 *
 * DirectFB_1_4_Port/6   7/22/10 4:27p robertwm
 * SW7405-4649: DirectFB: Optimise surface locking and wait idle syncing.
 *
 * DirectFB_1_4_Port/5   7/13/10 12:06p jackf
 * SW35230-618: Changes needed to support JPEG (JFIF) format files
 *  embedded in Flash content.
 *
 * DirectFB_1_4_Port/5   7/13/10 12:04p jackf
 * SW35230-618: Changes needed to support JPEG (JFIF) format files
 *  embedded in Flash content.
 *
 * DirectFB_1_4_Port/4   5/20/10 12:49p robertwm
 * SW3556-1119: DirectFB: Nexus ImageProvider fails to build with
 *  DirectFB-1.4.1 Phase 2.0.
 *
 * DirectFB_1_4_Port/3   4/19/10 2:11p robertwm
 * SW3556-1098: DirectFB: Nexus ImageProvider fails to build with
 *  DirectFB-1.4.1 Phase 2.0 PreRel.
 * SW3556-1099: DirectFB: SID should not be enabled if there is
 *  insufficient driver memory to initialise it.
 *
 * DirectFB_1_4_Port/2   1/14/10 6:55p robertwm
 * SW3556-996: DirectFB: Implement ImageProvider for DirectFB-1.4.1 using
 *  SID.
 *
 * DirectFB_1_4_Port/1   7/8/09 3:19p mphillip
 * PR56658: Add DirectFB 1.4.1 broadcom_files
 *
 * 5   1/16/09 1:45p gmohile
 * PR 46490 : Free allocated surface after rendering
 *
 * 4   11/21/08 8:06p vsilyaev
 * PR 49524: Added graphics2d_sync routine that could be used by different
 *  users of Nexus_Graphics2D
 *
 * 3   10/24/08 12:42p gmohile
 * PR 46490 : Add hw picture decode support
 *
 * 2   10/24/08 11:05a gmohile
 * PR 46490 : Add hw picture decode support
 *
 ***************************************************************************/

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>

#include <directfb.h>

#include <display/idirectfbsurface.h>

#include <media/idirectfbimageprovider.h>

#include <core/coredefs.h>
#include <core/coretypes.h>

#include <core/core.h>
#include <core/layers.h>
#include <core/surface.h>
#include <core/system.h>
#include <core/palette.h>

#include <misc/conf.h>
#include <misc/util.h>
#include <misc/gfx_util.h>
#include <gfx/convert.h>
#include <gfx/util.h>

#include <direct/interface.h>
#include <direct/mem.h>
#include <direct/memcpy.h>

#include "bcmnexus.h"
#include "bcmnexus_utils.h"

D_DEBUG_DOMAIN( bcmnexusImageProvider,        "bcmNexus/ImageProvider",        "Broadcom NEXUS Image Provider module" );
D_DEBUG_DOMAIN( bcmnexusImageProviderPalette, "bcmNexus/ImageProviderPalette", "Broadcom NEXUS Image Provider Palette debug" );

static DFBResult
Probe( IDirectFBImageProvider_ProbeContext *ctx );

static DFBResult
Construct( IDirectFBImageProvider *thiz,
           ... );

#include <direct/interface_implementation.h>

DIRECT_INTERFACE_IMPLEMENTATION( IDirectFBImageProvider, NEXUS )

/*
 * private data struct of IDirectFBImageProvider_NEXUS
 */
typedef struct {
     IDirectFBImageProvider_data base;
     int                         ref;      /* reference counter */
     pthread_mutex_t             mutex;
     pthread_cond_t              cond;
     int                         width;
     int                         height;
     DFBSurfacePixelFormat       pixelformat;
     NEXUS_PictureDecoderHandle  pictureDecoder;
     NEXUS_PictureFormat         pictureFormat;
     NEXUS_PixelFormat           nexusPixelFormat;
     NEXUS_PictureDecoderPalette nexusPalette;
     bool                        multiscan;
     bool                        transparency;
     bool                        hasAlphaChannel;
     unsigned long               colorkey;
     unsigned int                dataptr;
     bool                        decoderStarted;
     bool                        rotated;

} IDirectFBImageProvider_NEXUS_data;

static DFBResult
IDirectFBImageProvider_NEXUS_SegmentedRenderTo( IDirectFBImageProvider *thiz,
                                       IDirectFBSurface       *destination,
                                       const DFBRectangle     *destination_rect );

static DFBResult
IDirectFBImageProvider_NEXUS_SetRenderCallback( IDirectFBImageProvider *thiz,
                                                DIRenderCallback        callback,
                                                void                   *context );

static DFBResult
IDirectFBImageProvider_NEXUS_WriteBack( IDirectFBImageProvider *thiz,
                                        IDirectFBSurface       *surface,
                                        const DFBRectangle     *src_rect,
                                        const char             *filename );
static DFBResult
IDirectFBImageProvider_NEXUS_GetSurfaceDescription( IDirectFBImageProvider *thiz,
                                                    DFBSurfaceDescription  *dsc );

static DFBResult
IDirectFBImageProvider_NEXUS_GetImageDescription( IDirectFBImageProvider *thiz,
                                                  DFBImageDescription    *dsc );

static DFBResult
IDirectFBImageProvider_NEXUS_SWFallback(IDirectFBImageProvider *thiz,
                                                  IDirectFBSurface       *destination,
                                                  const DFBRectangle     *dest_rect,
                                                  bool                    render_to_failed);

static IDirectFBImageProvider_ProbeContext image_ctx;

static void PictureDecoderStep(void *context, int unused)
{

    IDirectFBImageProvider_NEXUS_data *data = (IDirectFBImageProvider_NEXUS_data*)context;
    NEXUS_PictureDecoderStatus pictureStatus;
    pthread_mutex_t *pLock = &(data->mutex);
    pthread_cond_t  *pCond =  &(data->cond);
    size_t                            size;
    unsigned int                      nbytes = 0;
    void                              *decode_buffer;

    (void)unused; /* remove compiler warning */

    /*Callbacks can come after image_provider->Destruct() is called*/
    if (!data->pictureDecoder)
    {
        return;
    }

    NEXUS_PictureDecoder_GetStatus(data->pictureDecoder, &pictureStatus);

    D_DEBUG_AT(bcmnexusImageProvider, "%s() State: %d HeaderValid: %d\n", __FUNCTION__, pictureStatus.state, pictureStatus.headerValid);

    if (pictureStatus.state == NEXUS_PictureDecoderState_eMoreData)
    {
          if (NEXUS_PictureDecoder_GetBuffer(data->pictureDecoder, &decode_buffer, &size) != NEXUS_SUCCESS)
          {
              D_ERROR("bcmNexus/ImageProvider: NEXUS_PictureDecoder_GetBuffer() call failed!\n");
              goto err;
          }
          data->base.buffer->PeekData( data->base.buffer, size, data->dataptr, decode_buffer, &nbytes );

          D_DEBUG_AT(bcmnexusImageProvider, "%s() PeekData Sizewanted %d offset %d bytes got %d\n", __FUNCTION__,
            size, data->dataptr, nbytes);
          data->dataptr += nbytes;

          if (NEXUS_PictureDecoder_ReadComplete(data->pictureDecoder, 0, nbytes) != NEXUS_SUCCESS)
          {
              D_ERROR("bcmNexus/ImageProvider: NEXUS_PictureDecoder_ReadComplete() call failed!\n");
              goto err;
          }

          return;
    }

err:
        pthread_mutex_lock( pLock );
        pthread_cond_broadcast( pCond );
        pthread_mutex_unlock( pLock );

}

static void
IDirectFBImageProvider_NEXUS_Destruct( IDirectFBImageProvider *thiz )
{
     IDirectFBImageProvider_NEXUS_data *data     =
                              (IDirectFBImageProvider_NEXUS_data*)thiz->priv;

     D_DEBUG_AT(bcmnexusImageProvider, "%s()\n", __FUNCTION__);

     if (data->decoderStarted)
     {
        NEXUS_PictureDecoder_Stop(data->pictureDecoder);
        data->decoderStarted = false;
     }

     pthread_mutex_destroy( &data->mutex );
     pthread_cond_destroy( &data->cond );

}

static DFBResult
Probe( IDirectFBImageProvider_ProbeContext *ctx )
{
    DFBBCMNEXUS         *pBrcmDfb = (DFBBCMNEXUS *) dfb_system_data();
    BCMNEXUS_Options    *brcm_config = &pBrcmDfb->options;

    D_DEBUG_AT(bcmnexusImageProvider, "%s() sw_picture_decode=%d\n", __FUNCTION__, brcm_config->sw_picture_decode);

    if (brcm_config->sw_picture_decode == false) {
        if (ctx->filename)
            D_DEBUG_AT(bcmnexusImageProvider, "%s(): Filename=%s\n", __FUNCTION__, ctx->filename);

         /*store for software fallback*/
        memcpy(&image_ctx, ctx, sizeof(IDirectFBImageProvider_ProbeContext));

        /* Look of the Jpeg SOI marker */
        if (ctx->header[0] == 0xff && ctx->header[1] == 0xd8) {
            /* Look for JFIF or Exif strings, also could look at header[3:2] for APP0(0xFFE0),
            * APP1(0xFFE1) or even other APPx markers.
            */
            if (strncmp ((char*) ctx->header + 6, "JFIF", 4) == 0 ||
                strncmp ((char*) ctx->header + 6, "Exif", 4) == 0 ||
                strncmp ((char*) ctx->header + 6, "VVL", 3) == 0 ||
                strncmp ((char*) ctx->header + 6, "WANG", 4) == 0)
            {
                D_DEBUG_AT(bcmnexusImageProvider, "%s: JFIF/Exif format detected.\n", __FUNCTION__);
                return DFB_OK;
            }

            /* Else look for Quantization table marker or Define Huffman table marker,
            * useful for EXIF thumbnails that have no APPx markers.
            */
            if (ctx->header[2] == 0xff && (ctx->header[3] == 0xdb || ctx->header[3] == 0xc4))
            {
                D_DEBUG_AT(bcmnexusImageProvider, "%s: JFIF/Exif format detected.\n", __FUNCTION__);
                return DFB_OK;
            }

            if (ctx->filename && strchr (ctx->filename, '.' ) &&
                (strcasecmp ( strrchr (ctx->filename, '.' ), ".jpg" ) == 0 ||
                 strcasecmp ( strrchr (ctx->filename, '.' ), ".jpeg") == 0))
            {
                D_DEBUG_AT(bcmnexusImageProvider, "%s: JPEG format detected.\n", __FUNCTION__);
                return DFB_OK;
            }
        }

        if (ctx->header[1] == 'P' && ctx->header[2] == 'N' && ctx->header[3] == 'G') {
            unsigned char png_signature[8] = {137, 80, 78, 71, 13, 10, 26, 10};

            if (ctx->filename && strrchr (ctx->filename, '.' ) &&
                (strcasecmp ( strrchr (ctx->filename, '.' ), ".png") == 0))
            {
                D_DEBUG_AT(bcmnexusImageProvider, "%s: PNG format detected.\n", __FUNCTION__);
                return DFB_OK;
            }

            if(!(memcmp(&ctx->header[0], &png_signature[0], 8)))
                return DFB_OK;
        }

        if (strncmp ((char*) ctx->header, "GIF8", 4) == 0) {
            D_DEBUG_AT(bcmnexusImageProvider, "%s: GIF format detected.\n", __FUNCTION__);
            return DFB_OK;
        }
    }

    return DFB_UNSUPPORTED;
}

static DFBResult
Construct( IDirectFBImageProvider *thiz,
           ... )
{
     NEXUS_PictureDecoderStartSettings pictureSettings;
     NEXUS_PictureDecoderStatus        pictureStatus;
     DFBBCMNEXUS                      *pBrcmDfb = (DFBBCMNEXUS *) dfb_system_data();
     void                             *decode_buffer;
     size_t                            size;
     unsigned int                      nbytes = 0;
     unsigned char                     header[32];
     IDirectFBDataBuffer              *buffer;
     CoreDFB                          *core;
     va_list                           tag;
     pthread_mutexattr_t               mutexAttr;
     pthread_condattr_t                condAttr;
     struct timespec                   timeout;
#ifndef BCMNEXUS_IMAGE_TRANSPARENCY_DATA_PRESENT
     IDirectFBImageProvider           *sw_image_provider;
     DFBResult                         ret  = DFB_OK;
     DFBImageDescription               dsc;
     BCMNEXUS_Options                 *brcm_config = &pBrcmDfb->options;
#endif
     /* Get Picture Decoder global access lock */
     PTHREAD_ROBUST_MUTEX_LOCK( &GET_PICTURE_DECODER_LOCK(pBrcmDfb) );

     DIRECT_ALLOCATE_INTERFACE_DATA(thiz, IDirectFBImageProvider_NEXUS)

     va_start( tag, thiz );
     buffer = va_arg( tag, IDirectFBDataBuffer * );
     core = va_arg( tag, CoreDFB * );
     va_end( tag );

     data->base.ref    = 1;
     data->base.buffer = buffer;
     data->base.core   = core;

     data->pictureDecoder = GET_PICTURE_DECODER_HND(pBrcmDfb);
     data->transparency    = false;
     data->hasAlphaChannel = false;
     D_ASSERT(data->pictureDecoder != NULL);

     data->base.buffer->PeekData( data->base.buffer, 32, 0, header, &nbytes );
     if(header[1] == 'P' && header[2] == 'N' && header[3] == 'G'){
         data->pictureFormat = NEXUS_PictureFormat_ePng;
     }
     else if(header[0] == 0xff && header[1] == 0xd8){
         data->pictureFormat = NEXUS_PictureFormat_eJpeg;
     }
     else if (strncmp ((char *)header, "GIF8", 4) == 0) {
         data->pictureFormat = NEXUS_PictureFormat_eGif;
     }
     else {
        D_DEBUG_AT(bcmnexusImageProvider, "%s: Unsupported format.\n", __FUNCTION__);
        goto err_format;
     }
         /*If Nexus does provide tranparancy info, use software imageprovider to retrieve transparancy info.*/
#ifndef BCMNEXUS_IMAGE_TRANSPARENCY_DATA_PRESENT
     if (NEXUS_PictureFormat_ePng == data->pictureFormat)
     {
         D_DEBUG_AT(bcmnexusImageProvider, "%s: Loading sw imageprovider to retrieve transparency info\n", __FUNCTION__);

         brcm_config->sw_picture_decode = true;
         IDirectFBImageProvider_CreateFromBuffer(buffer, data->base.core, &sw_image_provider);

         ret = sw_image_provider->GetImageDescription(sw_image_provider, &dsc);
         if(ret)
         {
             D_ERROR("bcmNexus/ImageProvider: %s(): GetImageDescription() call failed!\n", __FUNCTION__);
             sw_image_provider->Release(sw_image_provider);
             pthread_mutex_unlock( &GET_PICTURE_DECODER_LOCK(pBrcmDfb) );
             return DFB_FAILURE;
         }

         if(dsc.caps & DICAPS_COLORKEY)
         {
             /*Store transparency info and continu with Nexus imageprovider*/
             data->colorkey = ((dsc.colorkey_r << 16 ) | (dsc.colorkey_g << 8) | dsc.colorkey_b ) & 0x00FFFFFF;
             data->transparency = true;
         }

         if(dsc.caps & DICAPS_ALPHACHANNEL)
         {
             data->hasAlphaChannel = true;
         }

         brcm_config->sw_picture_decode = false;
         D_DEBUG_AT(bcmnexusImageProvider, "%s: Image transparancy: %d RGB colorkey:%08lx hasAlphaChannel: %d \n", __FUNCTION__,
                         data->transparency, data->colorkey, (dsc.caps & DICAPS_ALPHACHANNEL)?1:0);

         data->base.buffer->SeekTo(data->base.buffer, 0);
         sw_image_provider->Release(sw_image_provider);

         D_DEBUG_AT(bcmnexusImageProvider, "%s: Restoring Nexus ImageProvider \n", __FUNCTION__);
     }

#endif

     pthread_mutexattr_init(&mutexAttr);
     pthread_mutexattr_setpshared(&mutexAttr, PTHREAD_PROCESS_SHARED);
     pthread_condattr_init(&condAttr);
     pthread_condattr_setpshared(&condAttr, PTHREAD_PROCESS_SHARED);

     pthread_mutex_init( &(data->mutex),&mutexAttr);
     pthread_cond_init( &(data->cond), &condAttr);

     data->base.buffer->AddRef( data->base.buffer );

     data->base.buffer->GetLength(data->base.buffer, &nbytes);

     NEXUS_PictureDecoder_GetDefaultStartSettings(&pictureSettings);
     pictureSettings.format = data->pictureFormat;
     pictureSettings.pictureParsed.callback = PictureDecoderStep;
     pictureSettings.pictureParsed.context = data;
     pictureSettings.segmentDecoded.callback = PictureDecoderStep;
     pictureSettings.segmentDecoded.context = data;
     pictureSettings.imageSize = nbytes;

     NEXUS_PictureDecoder_GetBuffer(data->pictureDecoder, &decode_buffer, &size);
     data->base.buffer->PeekData( data->base.buffer, size, 0, decode_buffer, &nbytes );
     NEXUS_PictureDecoder_ReadComplete(data->pictureDecoder, 0, nbytes);

     pthread_mutex_lock( &data->mutex);

     if (NEXUS_PictureDecoder_Start(data->pictureDecoder, &pictureSettings) != NEXUS_SUCCESS)
     {
          D_WARN("SID: Start() call failed!\n");
                  pthread_mutex_unlock( &data->mutex );
          goto err_start;
     }
     data->decoderStarted = true;

     /* Wait up to 1s for image header */
     clock_gettime(CLOCK_REALTIME, &timeout);
     timeout.tv_sec +=1;

     pthread_cond_timedwait(&data->cond,
                            &data->mutex,
                            &timeout);

     pthread_mutex_unlock( &data->mutex );

     NEXUS_PictureDecoder_GetStatus(data->pictureDecoder, &pictureStatus);
     if (pictureStatus.state == NEXUS_PictureDecoderState_eError) {
         D_WARN("SID: Decoding Failed\n");
         goto err_decode;
     } else if ( !pictureStatus.headerValid ||
                ( pictureStatus.header.surface.width  == 0 ) ||
                ( pictureStatus.header.surface.height == 0 ) ) {
         D_WARN("SID: Could not decode image header\n");
         goto err_decode;
     }

     data->width  = pictureStatus.header.surface.width;
     data->height = pictureStatus.header.surface.height;
     data->nexusPixelFormat = pictureStatus.header.format;
     data->pixelformat = bcmnexus_getDFBSurfacePixelFormat(pictureStatus.header.format);
     data->multiscan = pictureStatus.header.multiscan;
     data->rotated = pictureStatus.header.rotated;

#ifdef BCMNEXUS_IMAGE_TRANSPARENCY_DATA_PRESENT
     data->transparency = pictureStatus.header.hasTransparentData?true:false;
     if (data->transparency)
        data->colorkey = pictureStatus.header.transparentColorRgb;

     if(DFB_PIXELFORMAT_HAS_ALPHA(data->pixelformat))
        data->hasAlphaChannel = true;
     else
        data->hasAlphaChannel = false;

     D_DEBUG_AT(bcmnexusImageProvider, "%s: Image transparancy: %d RGB colorkey:%x hasAlphaChannel: %d \n", __FUNCTION__,
                pictureStatus.header.hasTransparentData, pictureStatus.header.transparentColorRgb, data->hasAlphaChannel);
#endif

     /* If indexed colour, then load palette */
     if (pictureStatus.header.palette)
     {
        D_DEBUG_AT(bcmnexusImageProvider, "%s: Palette detected.\n", __FUNCTION__);
        if (NEXUS_PictureDecoder_GetPalette(data->pictureDecoder, &data->nexusPalette) != NEXUS_SUCCESS)
        {
             D_WARN("SID: Failed to get Palette\n");
             goto err_decode;
        }
     }

     D_DEBUG_AT(bcmnexusImageProvider, "%s: Nexus Picture format=%d, Nexus pixel format=%d, pixelformat=%s, size=%dx%d, multiscan=%d, rotated=%d\n", __FUNCTION__,
                data->pictureFormat, data->nexusPixelFormat, dfb_pixelformat_name(data->pixelformat),
                data->width, data->height, pictureStatus.header.multiscan, pictureStatus.header.rotated);


     data->base.Destruct         = IDirectFBImageProvider_NEXUS_Destruct;
     thiz->RenderTo              = IDirectFBImageProvider_NEXUS_SegmentedRenderTo;
     thiz->SetRenderCallback     = IDirectFBImageProvider_NEXUS_SetRenderCallback;
     thiz->GetImageDescription   = IDirectFBImageProvider_NEXUS_GetImageDescription;
     thiz->GetSurfaceDescription = IDirectFBImageProvider_NEXUS_GetSurfaceDescription;
     thiz->WriteBack             = IDirectFBImageProvider_NEXUS_WriteBack;

     /* Release Picture Decoder global access lock */
     pthread_mutex_unlock( &GET_PICTURE_DECODER_LOCK(pBrcmDfb) );

     return DFB_OK;

err_decode:
     NEXUS_PictureDecoder_Stop(data->pictureDecoder);
     data->decoderStarted = false;
err_start:
     data->base.buffer->Release( data->base.buffer );
err_format:

     /* Release Picture Decoder global access lock */
     pthread_mutex_unlock( &GET_PICTURE_DECODER_LOCK(pBrcmDfb) );

     /*if error in Nexus image provider, fallback to software imageprovider*/
     if(DFB_OK == IDirectFBImageProvider_NEXUS_SWFallback(thiz, NULL, NULL, false))
     {
         return DFB_OK;
     }

     // If the fallback fails this seems to be broken and freeing it causes a sig 5.
     // DIRECT_DEALLOCATE_INTERFACE( thiz );

     return DFB_FAILURE;
}

static DFBResult
IDirectFBImageProvider_NEXUS_SegmentedRenderTo( IDirectFBImageProvider *thiz,
                                       IDirectFBSurface       *destination,
                                       const DFBRectangle     *dest_rect )
{
    DFBResult                         ret = DFB_OK;
    IDirectFBSurface_data            *dst_data;
    CoreSurface                      *src_surface;
    CoreSurface                      *dst_surface;
    CorePalette                      *palette = NULL;
    CoreSurfaceBufferLock             src_surface_lock;
    DFBRegion                         clip;
    DFBRectangle                      rect;
    NEXUS_PictureDecoderStartSettings pictureSettings;
    NEXUS_PictureDecoderStatus        pictureStatus;
    void                             *decode_buffer;
    size_t                            size;
    unsigned int                      nbytes = 0;
    int                               numEntries;
    int                               index;
    struct timespec                   timeout;
    DFBSurfaceColorSpace              colorspace;
    DFBBCMNEXUS                      *pBrcmDfb = (DFBBCMNEXUS *) dfb_system_data();
    unsigned int                      file_size;
    CoreSurfaceBufferLock             row;
    int                               block = 256;   /* segmented decode size needs to be a multiple of 16 for jpegs */
    CoreSurface                      *row_surface;
    int                               y;
    unsigned                          dst_y;
    int                               cur_row;
    DFBRectangle                      rect_dst, rect_src;
    CardState                         stretchState;
    DFBRectangle                      srcRect, dstRect;
    int loops = 0;

    /* Get Picture Decoder global access lock */
    pthread_mutex_lock( &GET_PICTURE_DECODER_LOCK(pBrcmDfb) );

    DIRECT_INTERFACE_GET_DATA(IDirectFBImageProvider_NEXUS)

    D_DEBUG_AT(bcmnexusImageProvider, "%s()\n", __FUNCTION__);


    D_DEBUG_AT(bcmnexusImageProvider, "%s() Block size is %d\n", __FUNCTION__, block);
    /*
     * If it's a interlaced JPEG then let SW do it as SID is much slower than libjpg.
     */
    if (data->pictureFormat == NEXUS_PictureFormat_eJpeg && data->multiscan)
    {
        ret = DFB_UNSUPPORTED;
        goto err_unsupported;
    }

    dst_data = (IDirectFBSurface_data*) destination->priv;
    if (!dst_data)
    {
          /* Release Picture Decoder global access lock */
          pthread_mutex_unlock( &GET_PICTURE_DECODER_LOCK(pBrcmDfb) );
          return DFB_DEAD;
    }

    dst_surface = dst_data->surface;
    if (!dst_surface)
    {
          /* Release Picture Decoder global access lock */
          pthread_mutex_unlock( &GET_PICTURE_DECODER_LOCK(pBrcmDfb) );
          return DFB_DESTROYED;
    }

    dfb_region_from_rectangle( &clip, &dst_data->area.current );

    if (dest_rect) {
          if (dest_rect->w < 1 || dest_rect->h < 1)
          {
               /* Release Picture Decoder global access lock */
               pthread_mutex_unlock( &GET_PICTURE_DECODER_LOCK(pBrcmDfb) );
               return DFB_INVARG;
          }
          rect = *dest_rect;
          rect.x += dst_data->area.wanted.x;
          rect.y += dst_data->area.wanted.y;
    }
    else {
          rect = dst_data->area.wanted;
    }

    /* actual loading and rendering */
    if (dfb_rectangle_region_intersects( &rect, &clip )) {
          if (DFB_PIXELFORMAT_IS_INDEXED(data->pixelformat))
          {
              numEntries = data->nexusPalette.numPaletteEntries;
              D_DEBUG_AT(bcmnexusImageProvider, "%s(): Num Palette Entries=%d\n", __FUNCTION__, numEntries);

              /* Sanity check! */
              D_ASSERT( numEntries <= D_ARRAY_SIZE(data->nexusPalette.palette));

              ret = dfb_palette_create( data->base.core, numEntries, &palette );
              if (ret)
              {
                   D_ERROR("bcmNexus/ImageProvider: Cannot create palette of %d entries!\n", numEntries);
                   ret = DFB_FAILURE;
                   goto err_palette;
              }

              /* Convert from ARGB8888 NEXUS pixel format to DFB format */
              for (index = 0; index < numEntries; index++)
              {
                   dfb_pixel_to_color(DSPF_ARGB, data->nexusPalette.palette[index], &palette->entries[index]);
                   ret = DFB_FAILURE;
                   D_DEBUG_AT(bcmnexusImageProviderPalette, "%s(): Palette [%d] = 0x%08x\n", __FUNCTION__, index, data->nexusPalette.palette[index]);
              }

              dfb_palette_update( palette, 0, numEntries - 1 );
          }

          D_DEBUG_AT(bcmnexusImageProvider, "%s(): creating surface %dx%d format=%s...\n", __FUNCTION__,
                     data->width, data->height, dfb_pixelformat_name(data->pixelformat));

          colorspace = DFB_PIXELFORMAT_IS_INDEXED(data->pixelformat) || DFB_COLOR_IS_RGB(data->pixelformat) ? DSCS_RGB :
                            !DFB_COLOR_IS_YUV(data->pixelformat) ? DSCS_UNKNOWN :
                                data->height >= 720 ? DSCS_BT709 :
                                    data->pictureFormat == NEXUS_PictureFormat_eJpeg ? DSCS_BT601_FULLRANGE :
                                        DSCS_BT601;

          data->base.buffer->GetLength(data->base.buffer, &file_size);
          D_DEBUG_AT(bcmnexusImageProvider, "NEXUS RenderTo():: Image Size:%u\n", file_size);

          NEXUS_PictureDecoder_GetDefaultStartSettings(&pictureSettings);
          pictureSettings.format = data->pictureFormat;
          pictureSettings.segmentDecoded.callback = PictureDecoderStep;
          pictureSettings.segmentDecoded.context = data;
          pictureSettings.pictureParsed.callback  = PictureDecoderStep;
          pictureSettings.pictureParsed.context   = data;
          pictureSettings.imageSize = file_size;

          data->dataptr = 0;

          /*Parse Image Header first*/
          if (NEXUS_PictureDecoder_GetBuffer(data->pictureDecoder, &decode_buffer, &size) != NEXUS_SUCCESS)
          {
              D_ERROR("bcmNexus/ImageProvider: NEXUS_PictureDecoder_GetBuffer() call failed!\n");
              ret = DFB_FAILURE;
              goto err_buffer;
          }

          data->base.buffer->PeekData( data->base.buffer, size, 0, decode_buffer, &nbytes );
          data->dataptr = nbytes;
          if (NEXUS_PictureDecoder_ReadComplete(data->pictureDecoder, 0, nbytes) != NEXUS_SUCCESS)
          {
              D_ERROR("bcmNexus/ImageProvider: NEXUS_PictureDecoder_ReadComplete() call failed!\n");
              ret = DFB_FAILURE;
              goto err_buffer;
          }

          if (!data->decoderStarted)
          {
              if (NEXUS_PictureDecoder_Start(data->pictureDecoder, &pictureSettings) != NEXUS_SUCCESS)
              {
                  D_ERROR("bcmNexus/ImageProvider: NEXUS_PictureDecoder_Start() call failed!\n");
                  ret = DFB_FAILURE;
                  pthread_mutex_unlock( &data->mutex );
                  goto err_buffer;
              }
              data->decoderStarted = true;

             /* Wait up to 1s for image header */
              clock_gettime(CLOCK_REALTIME, &timeout);
              timeout.tv_sec +=1;

              pthread_cond_timedwait(&data->cond,
                                     &data->mutex,
                                     &timeout);

              pthread_mutex_unlock( &data->mutex );
              data->dataptr = 0;

              /*Parse Image Header first*/
              if (NEXUS_PictureDecoder_GetBuffer(data->pictureDecoder, &decode_buffer, &size) != NEXUS_SUCCESS)
              {
                  D_ERROR("bcmNexus/ImageProvider: NEXUS_PictureDecoder_GetBuffer() call failed!\n");
                  ret = DFB_FAILURE;
                  goto err_buffer;
              }

              data->base.buffer->PeekData( data->base.buffer, size, 0, decode_buffer, &nbytes );
              data->dataptr = nbytes;
              if (NEXUS_PictureDecoder_ReadComplete(data->pictureDecoder, 0, nbytes) != NEXUS_SUCCESS)
              {
                  D_ERROR("bcmNexus/ImageProvider: NEXUS_PictureDecoder_ReadComplete() call failed!\n");
                  ret = DFB_FAILURE;
                  goto err_buffer;
              }
          }

          /* Segmented mode decode for Non-Multiscanbuffer JPEG images only.
           * Other supported formats use full size decode */
          if (data->pictureFormat == NEXUS_PictureFormat_eJpeg && !data->rotated)
          {
                 D_DEBUG_AT(bcmnexusImageProvider, "Segmented & streaming mode decode for Jpeg images \n");

                  /* Create a surface for the picture decoder to decode into (call it src surface) */
                 ret = dfb_surface_create_simple( data->base.core,
                                                data->width,
                                                block,
                                                data->pixelformat,
                                                colorspace,
                                                DSCAPS_VIDEOONLY,
                                                CSTF_SHARED, 0, DSHF_NONE, palette, &row_surface );

                 if (ret)
                 {
                     D_ERROR("bcmNexus/ImageProvider: Cannot create surface row 0 %dx%d format=%s!\n",
                             data->width, data->height, dfb_pixelformat_name(data->pixelformat));
                     ret = DFB_FAILURE;
                     goto err_segment;
                 }

                /* Actually instantiate the buffer */
                 ret = dfb_surface_lock_buffer( row_surface, CSBR_BACK, CSAID_CPU, CSAF_READ|CSAF_WRITE, &row );
                if (ret)
                {
                    D_ERROR( "bcmNexus/ImageProvider: Could NOT lock source surface! (res=%d)\n", ret);
                    ret = DFB_FAILURE;
                    goto err_surface_jpg;
                }

                 for(cur_row=0, dst_y=y=0; y<data->height;y+=block)  {
                     NEXUS_Rect rect_nexus;

                     rect_nexus.x = 0;
                     rect_nexus.width = data->width;
                     rect_nexus.y = y;
                     if(rect_nexus.y + block <= data->height) {
                         rect_nexus.height = block;
                     } else {
                         rect_nexus.height = data->height - rect_nexus.y;
                     }

                     pthread_mutex_lock( &data->mutex);

                     if (NEXUS_PictureDecoder_DecodeSegment(data->pictureDecoder, row.handle, &rect_nexus) != NEXUS_SUCCESS)
                     {
                         D_ERROR("bcmNexus/ImageProvider: NEXUS_PictureDecoder_DecodeSegment() call failed!\n");
                         ret = DFB_FAILURE;
                         pthread_mutex_unlock( &data->mutex );
                         goto err_surface_jpg_locked;
                     }

                      loops = 0;
                      do
                      {
                          if (loops > 1)
                                pthread_mutex_lock( &data->mutex);

                          clock_gettime(CLOCK_REALTIME, &timeout);
                          timeout.tv_sec +=3;

                          pthread_cond_timedwait(&data->cond,
                                                 &data->mutex,
                                                 &timeout);

                          pthread_mutex_unlock( &data->mutex );

                          NEXUS_PictureDecoder_GetStatus(data->pictureDecoder, &pictureStatus);
                          loops++;
                      } while (loops < 10 &&
                        ( pictureStatus.state == NEXUS_PictureDecoderState_eDecoding
                            || pictureStatus.state == NEXUS_PictureDecoderState_eMoreData ));

                     if(pictureStatus.state == NEXUS_PictureDecoderState_eError) {
                         D_ERROR( "%s:%d NEXUS RenderTo():: decoding failed  Decoder state %d\n",
                                   __FUNCTION__, __LINE__, pictureStatus.state );
                         ret = DFB_FAILURE;
                         goto err_surface_jpg_locked;
                     }

                     rect_src.x = 0;
                     rect_src.y = 0;
                     rect_src.w = pictureStatus.header.width;;// data->width;
                     if(NEXUS_PIXEL_FORMAT_IS_YCRCB(data->nexusPixelFormat)) {
                         rect_src.w += rect_src.w%2; /* YCrCb single pixel has width of 2 */
                     }
                     if((rect_nexus.y + block) <= data->height) {
                         rect_src.h = block;
                     } else {
                         rect_src.h = data->height - rect_nexus.y;
                     }

                     rect_dst.x  = rect.x;
                     rect_dst.w  = rect.w;
                     rect_dst.y  = rect.y + dst_y;
                     rect_dst.h  = ((rect_nexus.y+rect_src.h)*rect.h)/data->height - dst_y;

                     D_DEBUG_AT(bcmnexusImageProvider, "blit from x=%d y=%d w=%d h=%d \n" , rect_src.x, rect_src.y, rect_src.w, rect_src.h );
                     D_DEBUG_AT(bcmnexusImageProvider, "blit to   x=%d y=%d w=%d h=%d \n" , rect_dst.x, rect_dst.y, rect_dst.w, rect_dst.h );

                     dst_y += rect_dst.h;

                     dfb_state_init(&stretchState, NULL);
                     srcRect.x = srcRect.y = dstRect.x = dstRect.y = 0;
                     srcRect.w = row_surface->config.size.w;
                     srcRect.h = rect_src.h;
                     dstRect.w = dst_surface->config.size.w;
                     dstRect.h = dst_surface->config.size.h;

                     if (dfb_rectangle_intersect( &dstRect, &rect_dst )) {
                         stretchState.modified   |= SMF_CLIP | SMF_SOURCE | SMF_DESTINATION;
                         stretchState.clip.x2     = dst_surface->config.size.w - 1;
                         stretchState.clip.y2     = dst_surface->config.size.h - 1;
                         stretchState.source      = row_surface;
                         stretchState.destination = dst_surface;
                         stretchState.from        = CSBR_FRONT;
                         stretchState.to          = CSBR_BACK;
                         if(CSCONF_CAPS & dst_surface->config.flags)
                         {
                             if(DSCAPS_PREMULTIPLIED & dst_surface->config.caps)
                             {
                                 stretchState.modified |= SMF_BLITTING_FLAGS;
                                 stretchState.blittingflags = DSBLIT_SRC_PREMULTIPLY;
                             }
                         }

                         dfb_gfxcard_stretchblit( &srcRect, &dstRect, &stretchState );
                         dfb_state_stop_drawing( &stretchState );
                     }

                     dfb_gfxcard_sync();
                     cur_row  = (cur_row+1)%2;
                 }

err_surface_jpg_locked:
                 dfb_surface_unlock_buffer(row_surface, &row);
err_surface_jpg:
                 dfb_surface_unref(row_surface);
                 goto err_segment;
          }
          else {

                /* Create a surface for the picture decoder to decode into (call it src surface) */
                ret = dfb_surface_create_simple( data->base.core,
                                               data->width,
                                               data->height,
                                               data->pixelformat,
                                               colorspace,
                                               DSCAPS_VIDEOONLY,
                                               CSTF_SHARED, 0, DSHF_NONE, palette, &src_surface );
                if (ret)
                {
                    D_ERROR("bcmNexus/ImageProvider: Cannot create surface %dx%d format=%s!\n",
                            data->width, data->height, dfb_pixelformat_name(data->pixelformat));
                    ret = DFB_FAILURE;
                    goto err_buffer;
                }

                /* Actually instantiate the buffer */
                ret = dfb_surface_lock_buffer( src_surface, CSBR_BACK, CSAID_CPU, CSAF_READ|CSAF_WRITE, &src_surface_lock );
                if (ret)
                {
                    D_ERROR( "bcmNexus/ImageProvider: Could NOT lock source surface! (res=%d)\n", ret);
                    ret = DFB_FAILURE;
                    goto err_surface;
                }

                /* start decoding */
                pthread_mutex_lock( &data->mutex);

                if (NEXUS_PictureDecoder_DecodeSegment(data->pictureDecoder, src_surface_lock.handle, NULL) != NEXUS_SUCCESS)
                {
                    D_ERROR("bcmNexus/ImageProvider: NEXUS_PictureDecoder_DecodeSegment() call failed!\n");
                    dfb_surface_unlock_buffer( src_surface, &src_surface_lock );
                    ret = DFB_FAILURE;
                    pthread_mutex_unlock( &data->mutex );
                    goto err_surface;
                }

                loops = 0;
                do
                {
                  if (loops > 1)
                        pthread_mutex_lock( &data->mutex);

                  clock_gettime(CLOCK_REALTIME, &timeout);
                  timeout.tv_sec +=3;

                  pthread_cond_timedwait(&data->cond,
                                         &data->mutex,
                                         &timeout);

                  pthread_mutex_unlock( &data->mutex );

                  NEXUS_PictureDecoder_GetStatus(data->pictureDecoder, &pictureStatus);
                  loops++;
                } while (loops < 10 &&
                ( pictureStatus.state == NEXUS_PictureDecoderState_eDecoding
                    || pictureStatus.state == NEXUS_PictureDecoderState_eMoreData ));

               if(pictureStatus.state == NEXUS_PictureDecoderState_eError) {
                   D_ERROR( "%s:%d NEXUS RenderTo():: decoding failed  Decoder state %d\n",
                            __FUNCTION__, __LINE__, pictureStatus.state );
                  dfb_surface_unlock_buffer( src_surface, &src_surface_lock );
                  ret = DFB_FAILURE;
                  goto err_surface;
               }

               dfb_surface_unlock_buffer( src_surface, &src_surface_lock );

               rect_src.x = 0;
               rect_src.y = 0;
               rect_src.w = data->width;
               if(NEXUS_PIXEL_FORMAT_IS_YCRCB(data->nexusPixelFormat)) {
                   rect_src.w += rect_src.w%2; /* YCrCb single pixel has width of 2 */
               }
               rect_src.h = data->height;

               rect_dst.x = rect.x;
               rect_dst.y = rect.y;
               rect_dst.w = rect.w;
               rect_dst.h = rect.h;

               dfb_state_init(&stretchState, NULL);
               srcRect.x = srcRect.y = dstRect.x = dstRect.y = 0;
               srcRect.w = src_surface->config.size.w;
               srcRect.h = src_surface->config.size.h;
               dstRect.w = dst_surface->config.size.w;
               dstRect.h = dst_surface->config.size.h;

               if (dfb_rectangle_intersect( &dstRect, &rect_dst )) {
                   stretchState.modified   |= SMF_CLIP | SMF_SOURCE | SMF_DESTINATION;
                   stretchState.clip.x2     = dst_surface->config.size.w - 1;
                   stretchState.clip.y2     = dst_surface->config.size.h - 1;
                   stretchState.source      = src_surface;
                   stretchState.destination = dst_surface;
                   stretchState.from        = CSBR_FRONT;
                   stretchState.to          = CSBR_BACK;
                   if(CSCONF_CAPS & dst_surface->config.flags)
                   {
                       if(DSCAPS_PREMULTIPLIED & dst_surface->config.caps)
                       {
                           stretchState.modified |= SMF_BLITTING_FLAGS;
                           stretchState.blittingflags = DSBLIT_SRC_PREMULTIPLY;
                       }
                   }

                   dfb_gfxcard_stretchblit( &srcRect, &dstRect, &stretchState );
                   dfb_state_stop_drawing( &stretchState );
               }

               dfb_gfxcard_sync();
          }

          if (data->base.render_callback) {
               DIRenderCallbackResult r;

                rect.x = 0;
                rect.y = 0;
                rect.w = pictureStatus.header.width;
                rect.h = pictureStatus.header.height;

                r = data->base.render_callback( &rect, data->base.render_callback_context );

                if (r != DIRCR_OK)
                    ret = DFB_INTERRUPTED;
          }
err_surface:
    dfb_surface_unref(src_surface);
err_segment:
err_buffer:
     if (palette) {
          dfb_palette_unref( palette );
     }
  }

err_palette:
err_unsupported:
     if ( data->decoderStarted )
     {
         NEXUS_PictureDecoder_Stop(data->pictureDecoder);
         data->decoderStarted = false;
     }

     /* Release Picture Decoder global access lock */
     pthread_mutex_unlock( &GET_PICTURE_DECODER_LOCK(pBrcmDfb) );

     /*if error in Nexus image provider, fallback to software imageprovider*/
     if(DFB_OK != ret)
     {

         if(DFB_OK == IDirectFBImageProvider_NEXUS_SWFallback(thiz, destination, dest_rect, true))
         {
             ret = DFB_OK;
         }
     }

     D_DEBUG_AT(bcmnexusImageProvider, "%s() Exit [%d]\n", __FUNCTION__, ret);
     return ret;
}

static DFBResult
IDirectFBImageProvider_NEXUS_WriteBack( IDirectFBImageProvider *thiz,
                                        IDirectFBSurface       *surface,
                                        const DFBRectangle     *src_rect,
                                        const char             *filename )
{
     DFBResult                   ret = DFB_OK;
     int                         fd;
     ssize_t                     cnt;
     IDirectFBSurface_data      *sur_data;
     CoreSurface                *core_surface;
     CoreSurfaceBufferLock       lock;
     NEXUS_SurfaceHandle         handle;
     NEXUS_SurfaceMemory         mem;
     NEXUS_SurfaceCreateSettings createSettings;

     BSTD_UNUSED(src_rect);

     DIRECT_INTERFACE_GET_DATA (IDirectFBImageProvider_NEXUS)

     if (filename) {
          sur_data = (IDirectFBSurface_data*) surface->priv;
          if (!sur_data)
              return DFB_DEAD;

          core_surface = sur_data->surface;
          if (!core_surface)
              return DFB_DESTROYED;

          /* First check to see if the file already exists.  If it does
             then just overwrite it. */
          fd = open( filename, O_RDWR);
          if (fd < 0) {
              fd = open( filename,  O_EXCL | O_WRONLY | O_CREAT, 0644 );

              if (fd < 0) {
                   D_PERROR("%s: could not open %s for writing!\n", __FUNCTION__, filename);
                   return DFB_IO;
              }
          }

          ret = dfb_surface_lock_buffer( core_surface, CSBR_BACK, CSAID_CPU, CSAF_READ, &lock );
          if (ret)
          {
               D_WARN("Could not lock surface buffer %p!\n", (void *)core_surface);
               close(fd);
               return ret;
          }

          handle = lock.handle;
          NEXUS_Surface_GetMemory(handle, &mem);
          NEXUS_Surface_GetCreateSettings(handle, &createSettings);
          cnt = write(fd, mem.buffer, mem.pitch*createSettings.height);
          if (cnt < 0) {
              D_WARN("Could not write to %s (err=%s)!\n", filename, strerror(errno));
              ret = DFB_IO;
          } else {
              D_DEBUG_AT(bcmnexusImageProvider, "Wrote %d bytes of surface %dx%d (Nexus pixelformat=%d) to \"%s\".\n", cnt, createSettings.width,
                    createSettings.height, createSettings.pixelFormat, filename);
          }

          dfb_surface_unlock_buffer( core_surface, &lock );
          close(fd);
     }
     return ret;
}

static DFBResult
IDirectFBImageProvider_NEXUS_SetRenderCallback( IDirectFBImageProvider *thiz,
                                                DIRenderCallback        callback,
                                                void                   *context )
{
     DIRECT_INTERFACE_GET_DATA (IDirectFBImageProvider_NEXUS)

     D_DEBUG_AT(bcmnexusImageProvider, "%s()\n", __FUNCTION__);

     data->base.render_callback         = callback;
     data->base.render_callback_context = context;

     return DFB_OK;
}

static DFBResult
IDirectFBImageProvider_NEXUS_GetSurfaceDescription( IDirectFBImageProvider *thiz,
                                                    DFBSurfaceDescription  *dsc )
{
     DIRECT_INTERFACE_GET_DATA(IDirectFBImageProvider_NEXUS)

     dsc->flags       = DSDESC_WIDTH |  DSDESC_HEIGHT | DSDESC_PIXELFORMAT | DSDESC_CAPS;
     dsc->caps        = DSCAPS_VIDEOONLY;
     dsc->height      = data->height;
     dsc->width       = data->width;
     dsc->pixelformat = dfb_primary_layer_pixelformat();

     D_DEBUG_AT(bcmnexusImageProvider, "%s() %dx%d format=%s\n", __FUNCTION__, dsc->width, dsc->height, dfb_pixelformat_name(dsc->pixelformat));

     return DFB_OK;
}

static DFBResult
IDirectFBImageProvider_NEXUS_GetImageDescription( IDirectFBImageProvider *thiz,
                                                  DFBImageDescription    *dsc )
{
     DIRECT_INTERFACE_GET_DATA(IDirectFBImageProvider_NEXUS)

     D_DEBUG_AT(bcmnexusImageProvider, "%s()\n", __FUNCTION__);

     if (!dsc)
          return DFB_INVARG;

     dsc->caps = DICAPS_NONE;

#ifdef BCMNEXUS_IMAGE_TRANSPARENCY_DATA_PRESENT

    if (data->hasAlphaChannel)
          dsc->caps |= DICAPS_ALPHACHANNEL;

    if (data->transparency) {
              dsc->caps = DICAPS_COLORKEY;

              dsc->colorkey_r = (data->colorkey & 0xff0000) >> 16;
              dsc->colorkey_g = (data->colorkey & 0x00ff00) >>  8;
              dsc->colorkey_b = (data->colorkey & 0x0000ff);
    }
#endif

     return DFB_OK;
}

static DFBResult
IDirectFBImageProvider_NEXUS_SWFallback(IDirectFBImageProvider *thiz,
                                                      IDirectFBSurface       *destination,
                                                      const DFBRectangle     *dest_rect,
                                                      bool                    render_to_failed)
{
    DFBResult             ret = DFB_OK;
    DirectInterfaceFuncs  *funcs = NULL;

    IDirectFBDataBuffer   *image_buffer = NULL;
    CoreDFB                *directfb_core = NULL;

    DFBBCMNEXUS         *pBrcmDfb = (DFBBCMNEXUS *) dfb_system_data();
    BCMNEXUS_Options    *brcm_config = &pBrcmDfb->options;

    DIRECT_INTERFACE_GET_DATA (IDirectFBImageProvider_NEXUS)

    D_DEBUG_AT(bcmnexusImageProvider, "%s() Using SW fallback\n", __FUNCTION__);

    image_buffer  = data->base.buffer;
    directfb_core = data->base.core;

    if (thiz->priv) {
         /*release lock, mutex before releasing NEXUS imageprovider private data*/
         /*Need to fix condition when software imageprovider Construct() call fails, and app calls
         IDirectFBImageProvider_NEXUS_Release(), we already have released Nexus private data !*/
         pthread_mutex_destroy( &data->mutex );
         pthread_cond_destroy( &data->cond );
         D_FREE( thiz->priv );
         thiz->priv = NULL;
    }

     brcm_config->sw_picture_decode = true;
     ret = DirectGetInterface( &funcs, "IDirectFBImageProvider", NULL, DirectProbeInterface, &image_ctx );
     if (ret)
     {
        D_ERROR("bcmNexus/ImageProvider: DirectGetInterface() call failed!\n");
        goto err_intface;
     }

     /* Construct the interface. */
     ret = funcs->Construct( thiz, image_buffer, directfb_core );
     if(ret)
     {
        D_ERROR("bcmNexus/ImageProvider: Construct() call failed!\n");
        goto err_intface;
     }

     /*if NEXUS RenderTo failed, re-call software RenderTo() */
     if(render_to_failed)
     {
         ret = thiz->RenderTo(thiz, destination, dest_rect);
         if(ret)
         {
            D_ERROR("bcmNexus/ImageProvider: SW fallback RenderTo() call failed!\n");
         }
     }

     /*Next image decode with hardware SID*/
     brcm_config->sw_picture_decode = false;
err_intface:
    return ret;

}
