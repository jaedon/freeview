/***************************************************************************
 *     Copyright (c) 2004-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwin_image_jpeg.c $
 * $brcm_Revision: 8 $
 * $brcm_Date: 2/26/08 10:01a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwin/src/bwin_image_jpeg.c $
 * 
 * 8   2/26/08 10:01a jrubio
 * PR39363: cleaning up warnings
 * 
 * 6   7/26/06 2:15p tokushig
 * PR20685: added DRM support to brutus
 * 
 * SanDiego_DRM_ASF/1   7/20/06 6:04p tokushig
 * fix build warning
 * 
 * 5   5/31/06 4:19p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/11   5/18/06 12:22p tokushig
 * memset converted to BKNI_Memset
 * 
 * SanDiego_Brutus_Skin/10   5/16/06 10:17a tokushig
 * Merge from main
 * 
 * SanDiego_Brutus_Skin/9   5/11/06 11:06a tokushig
 * fix problem with jpeg decoding in 16bit color using offscreen surface
 * 
 * SanDiego_Brutus_Skin/8   5/3/06 12:33p tokushig
 * added support for hardware acceleration of offscreen buffers in bwin
 * 
 * SanDiego_Brutus_Skin/7   4/27/06 11:03a tokushig
 * fix build warnings and improve ability to determine if given file is a
 * jpeg (in support of the exif thumb)
 * $brcm_Log: /BSEAV/lib/bwin/src/bwin_image_jpeg.c $
 * 
 * 8   2/26/08 10:01a jrubio
 * PR39363: cleaning up warnings
 * 
 * 6   7/26/06 2:15p tokushig
 * PR20685: added DRM support to brutus
 * 
 * SanDiego_DRM_ASF/1   7/20/06 6:04p tokushig
 * fix build warning
 * 
 * 5   5/31/06 4:19p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/11   5/18/06 12:22p tokushig
 * memset converted to BKNI_Memset
 * 
 * SanDiego_Brutus_Skin/10   5/16/06 10:17a tokushig
 * Merge from main
 * 
 * 3   4/17/06 5:09p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/6   4/14/06 4:20p tokushig
 * cleaned up compile warnings
 * 
 * SanDiego_Brutus_Skin/5   4/4/06 5:02p tokushig
 * do hardware assisted scaling/blitting only if 'copy rect' drawops has
 * been overridden.  otherwise use legacy software scaling.  also added
 * additional comments.
 * 
 * SanDiego_Brutus_Skin/4   4/4/06 11:20a tokushig
 * modify jpeg decompression scheme to be able to read jpeg from memory or
 * directly from a file
 * 
 * SanDiego_Brutus_Skin/3   3/28/06 3:14p tokushig
 * add handling for decoding images directly from a file.
 * 
 * SanDiego_Brutus_Skin/2   3/9/06 11:14a tokushig
 * simplify jpeg decompression abort technique
 * 
 * SanDiego_Brutus_Skin/1   3/7/06 2:31p tokushig
 * added DCT rough scaling to jpeg decodes.  DCT scaling by 1/2, 1/4, or
 * 1/8 speeds the decoding of large images.
 * 
 * 2   5/4/05 9:12p sri
 * PR 14773 : Adding support for  Exif which stands for   EXchangeable
 * Image format for Digital Still Camera File format
 * 
 * 1   2/7/05 9:05p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/4   1/10/05 4:09p erickson
 * PR13386: rework for image-rendering changs
 * 
 * Irvine_BSEAVSW_Devel/3   1/6/05 4:11p erickson
 * PR13639: moved shared rendering code into bwin_image and fixed some
 * clipping problems
 * 
 * Irvine_BSEAVSW_Devel/2   1/5/05 3:03p erickson
 * PR13639: first rel of jpeg support
 *
 * Irvine_BSEAVSW_Devel/1   12/22/04 5:32p erickson
 * PR13639: some initial jpeg code, in process
 *
 ****************************************************************/
#include "bwin.h"
#include "bwin_priv.h"
#include "bwin_image.h"
#include "bwin_image_priv.h"
#include "jpeglib.h"
#include "bwin_image_debug.h"
BDBG_MODULE(bwin_image_jpeg);

struct bwin_image_data {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPARRAY linebuffer;      /* Output row buffer */
};

static void b_init_source(j_decompress_ptr cinfo)
{
    BSTD_UNUSED(cinfo);
    /* nothing */
}
static boolean b_fill_input_buffer(j_decompress_ptr cinfo)
{
    BSTD_UNUSED(cinfo);
    /* already filled */
    return true;
}
static void b_skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
    cinfo->src->bytes_in_buffer -= num_bytes;
    cinfo->src->next_input_byte += num_bytes;
}
static void b_term_source(j_decompress_ptr cinfo)
{
    BSTD_UNUSED(cinfo);
    /* nothing */
}

static void b_jpeg_mem_src(j_decompress_ptr cinfo, const unsigned char * buffer, int length)
{
    if (cinfo->src == NULL) {   /* first time for this JPEG object? */
        cinfo->src = (struct jpeg_source_mgr *)
        (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                    sizeof(struct jpeg_source_mgr));
        /* TODO: is this automatically dealloced? */
    }

    cinfo->src->init_source = b_init_source;
    cinfo->src->fill_input_buffer = b_fill_input_buffer;
    cinfo->src->skip_input_data = b_skip_input_data;
    cinfo->src->resync_to_restart = jpeg_resync_to_restart; /* use default*/
    cinfo->src->term_source = b_term_source;
    cinfo->src->bytes_in_buffer = length;
    cinfo->src->next_input_byte = buffer;
}

static void b_jpeg_start(bwin_image_t image)
{
    struct bwin_image_data *jpeg = image->data;
    jpeg->cinfo.err = jpeg_std_error(&jpeg->jerr);
    jpeg_create_decompress(&jpeg->cinfo);

    if (image->fpImage)
    {
        /* image resides in file */
        rewind(image->fpImage);
        jpeg_stdio_src(&jpeg->cinfo, image->fpImage);
    }
    else
    {
        /* image resides in memory buffer */
        b_jpeg_mem_src(&jpeg->cinfo, image->buffer, image->size);
    }

    jpeg_read_header(&jpeg->cinfo, TRUE);
}

static void b_jpeg_stop(bwin_image_t image)
{
    struct bwin_image_data *jpeg = image->data;
    jpeg_destroy_decompress(&jpeg->cinfo);
}

static bwin_result bwin_image_jpeg_init(bwin_image_t image)
{
    struct bwin_image_data *jpeg;

    jpeg = (struct bwin_image_data *)BKNI_Malloc(sizeof(struct bwin_image_data));
    if (!jpeg)
        return bwin_result_allocation_error;
    BKNI_Memset(jpeg, 0, sizeof(struct bwin_image_data));
    image->data = (void*)jpeg;

    b_jpeg_start(image);
    image->settings.width = jpeg->cinfo.image_width;
    image->settings.height = jpeg->cinfo.image_height;
    b_jpeg_stop(image);
    return 0;
}

static void bwin_image_jpeg_finalize(bwin_image_t image)
{
    BKNI_Free(image->data);
    image->data = NULL;
}

static int get_jpeg_src_line(bwin_image_t image, int srcrow)
{
    struct bwin_image_data *jpeg = image->data;
    while (srcrow > image->srcrow) {
        jpeg_read_scanlines(&jpeg->cinfo, jpeg->linebuffer, 1);
        image->srcrow++;
        image->srcrow_buffer = jpeg->linebuffer[0];
    }
    return 0;
}

static void bwin_image_jpeg_render(bwin_framebuffer_t fb, bwin_image_t image,
    const bwin_rect *destrect,const bwin_rect *srcrect,const bwin_rect *clipdestrect)
{
    struct bwin_image_data *jpeg = image->data;
    bwin_image_transform_settings transform_settings;
    int scale;

    bwin_rect rectScaled = *srcrect;
    bwin_framebuffer_settings   fbSettingsMain;

    BDBG_MSG(("jpeg image render begin"));
    bwin_image_transform_settings_init(&transform_settings,
        bwin_pixel_format_r8_g8_b8, fb->settings.pixel_format);

    b_jpeg_start(image);

    BDBG_MSG(("src size %dx%d", image->settings.width, image->settings.height));
    BDBG_MSG(("destrect %dx%d", destrect->width, destrect->height));
    /* calc amount of dct downscaling (use simple integer based rounding) */
    scale = max(((image->settings.width * 10 / destrect->width + 5) / 10), 
                ((image->settings.height * 10 / destrect->height + 5) / 10));

    jpeg->cinfo.scale_num = 1;

    if (scale >= 8)
        jpeg->cinfo.scale_denom = 8;
    else if (scale >= 4)
        jpeg->cinfo.scale_denom = 4;
    else if (scale >= 2)
        jpeg->cinfo.scale_denom = 2;

    BDBG_MSG(("JPEG DCT scale 1/%d", jpeg->cinfo.scale_denom));

    jpeg_start_decompress(&jpeg->cinfo);
    BDBG_MSG(("jpeg_render %dx%d", jpeg->cinfo.output_width, jpeg->cinfo.output_height));
    jpeg->linebuffer = (*jpeg->cinfo.mem->alloc_sarray)
        ((j_common_ptr) &jpeg->cinfo, JPOOL_IMAGE, 
        jpeg->cinfo.output_width * jpeg->cinfo.output_components, 1);
    BDBG_MSG(("linebuffer size:%d", jpeg->cinfo.output_width * jpeg->cinfo.output_components));

    rectScaled.width  = jpeg->cinfo.output_width;
    rectScaled.height = jpeg->cinfo.output_height;

    bwin_get_framebuffer_settings(fb, &fbSettingsMain);

    if (fbSettingsMain.drawops.copy_rect != bwin_p_copy_rect)
    {
        /* 
         * the copy rect function has been overridden (probably to include
         * hardware acceleration) so we'll take advantage of that for
         * jpeg images.  note that we are assuming that this copy
         * rect function can handle scaling.
         * 
         * so we will use an offscreen buffer to decompress our image
         * into, and then rely of the overridden copy rect function to 
         * to do scaling and blitting to the screen.
         *
         * we draw into the offscreen buffer here since we only want this to
         * apply for jpeg images.  jpeg images are unique in that they allow
         * for dct scaling during decompression...which reduces the size of
         * the offscreen buffer we have to allocate.  since this is known
         * only during decompression, we must allocate the buffer here.
         *
         * 1. create offscreen surface (handle and ptr to memory)
         *
         * 2. use jpeg_read_scanlines() to decompress jpeg line by line and
         *    save to offscreen surface
         *
         * 3. use bwin_copy_rect() to copy/scale/colorcorrect to display
         *
         * 4. destroy offscreen surface
         *
         */

        bwin_framebuffer_t          fbTmp = NULL;
        bwin_framebuffer_settings   fbSettingsTmp;
        int        srcBytesPerPixel = 0;
        int        dstBytesPerPixel = 0;
        uint32_t   srcwidth         = 0;
        uint32_t   destwidth        = 0;
        uint8_t  * pSrc             = NULL;
        uint8_t  * pDest            = NULL;

        BDBG_MSG(("drawing jpeg image with HW scaling"));
        bwin_framebuffer_settings_init(&fbSettingsTmp);
        fbSettingsTmp.data         = fbSettingsMain.data;
        fbSettingsTmp.drawops      = fbSettingsMain.drawops;
        fbSettingsTmp.width        = jpeg->cinfo.output_width;
        fbSettingsTmp.height       = jpeg->cinfo.output_height;
        fbSettingsTmp.pixel_format = bwin_pixel_format_a8_r8_g8_b8;
        fbSettingsTmp.pitch        = bwin_get_bpp(fbSettingsTmp.pixel_format)/8 * 
                                                  fbSettingsTmp.width;
        /* null buffer setting will make bwin_open_framebuffer() 
         * allocate/deallocate buffer memory for us.  
         * it will attempt to use graphics memory if possible in anticipation
         * of bwin_copy_rect() hardware acceleration.
         */
        fbSettingsTmp.buffer       = NULL;

        BDBG_MSG(("temp fb size:%d", fbSettingsTmp.height * fbSettingsTmp.pitch));
        fbTmp = bwin_open_framebuffer(fb->win_engine, &fbSettingsTmp);
        bwin_get_framebuffer_settings(fbTmp, &fbSettingsTmp);

        srcBytesPerPixel = jpeg->cinfo.output_components;
        dstBytesPerPixel = bwin_get_bpp(fbSettingsTmp.pixel_format)/8;
        srcwidth         = jpeg->cinfo.output_width * srcBytesPerPixel;
        destwidth        = fbSettingsTmp.pitch;
        pSrc             = (uint8_t *)jpeg->linebuffer[0];
        pDest            = (uint8_t *)fbSettingsTmp.buffer;

        while (jpeg->cinfo.output_scanline < jpeg->cinfo.output_height) 
        {
            uint32_t srcIdx = 0;
            uint32_t dstIdx = 0;

            typedef struct {
                unsigned char r;
                unsigned char g;
                unsigned char b;
            } jpeg_rgb;
#if BSTD_CPU_ENDIAN==BSTD_ENDIAN_LITTLE
            typedef struct {
                unsigned char b;
                unsigned char g;
                unsigned char r;
                unsigned char a;
            } jpeg_argb;
#else
            typedef struct {
                unsigned char a;
                unsigned char r;
                unsigned char g;
                unsigned char b;
            } jpeg_argb;
#endif
			jpeg_rgb  * pSrcColorFmt;
			jpeg_argb * pDstColorFmt;

            jpeg_read_scanlines(&jpeg->cinfo, jpeg->linebuffer, 1);

            /* copy decoded line to our temp bwin framebuffer while
             * adjusting to a compatible color format and endianness */
            while (srcIdx < srcwidth)
            {
                pSrcColorFmt  = (jpeg_rgb *)&(pSrc[srcIdx]);
                pDstColorFmt  = (jpeg_argb *)&(pDest[dstIdx]);

                pDstColorFmt->r = pSrcColorFmt->r;
                pDstColorFmt->g = pSrcColorFmt->g;
                pDstColorFmt->b = pSrcColorFmt->b;
                pDstColorFmt->a = 0xFF;

                srcIdx += srcBytesPerPixel;
                dstIdx += dstBytesPerPixel;
            }
            
            pDest += destwidth;
        }

        bwin_copy_rect(fb->settings.window, destrect, 
                       fbTmp->settings.window, &rectScaled, 
                       clipdestrect);

        bwin_close_framebuffer(fbTmp);
    }
    else
    {
        BDBG_MSG(("drawing jpeg image with SW scaling"));
        bwin_image_p_render_lines(fb, image, destrect, &rectScaled, clipdestrect,
            &transform_settings, get_jpeg_src_line);
    }

    jpeg_destroy_decompress(&jpeg->cinfo);

    /* no need to read the unread lines, call jpeg_finish_decompress() */ 
    /* or jpeg_abort_decompress() because b_jpeg_stop() calls          */ 
    /* jpeg_destroy_decompress() which does all the clean up for us -  */ 
    /* see jpeglib.h for more details                                  */
    b_jpeg_stop(image);
    BDBG_MSG(("jpeg image render complete"));
}

int bwin_image_p_is_jpeg(bwin_image_t image)
{
    int rc = -1;

    if (image->size < 11)
        return -1;

    /* TODO: what's the real method? */
    /* adding support for EXchangeable Imga format for Digital Still Camera */

    if (image->fpImage)
    {
        /* decompressor will read image directly from file 
           so use file handle to determine if it is a jpeg */
        char buf[11];
        fpos_t posOrig;
        BKNI_Memset(buf, 0, sizeof(buf));
        fgetpos(image->fpImage, &posOrig);
        rewind(image->fpImage);
        fread(buf, sizeof(*buf), sizeof(buf), image->fpImage);
        rc = (BKNI_Memcmp(&buf[6], "JFIF", 5) && BKNI_Memcmp(&buf[6], "Exif", 4));
        fsetpos(image->fpImage, &posOrig);
    }
    else
    if (image->buffer)
    {
        /* decompressor will read image from memory
           so use buffer to determine if it is a jpeg */
        unsigned char soi[] = { 0xff, 0xd8 };
        rc = (BKNI_Memcmp(image->buffer, soi, 2) &&
              BKNI_Memcmp(&image->buffer[6], "JFIF", 5) && BKNI_Memcmp(&image->buffer[6], "Exif", 4));
    }

    if (!rc) {
        image->init = bwin_image_jpeg_init;
        image->render = bwin_image_jpeg_render;
        image->finalize = bwin_image_jpeg_finalize;
    }

    return rc;
}
