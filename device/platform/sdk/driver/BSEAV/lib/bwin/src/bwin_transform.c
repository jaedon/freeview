/***************************************************************************
 *     Copyright (c) 2004-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwin_transform.c $
 * $brcm_Revision: 6 $
 * $brcm_Date: 12/5/11 3:29p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwin/src/bwin_transform.c $
 * 
 * 6   12/5/11 3:29p erickson
 * SW7405-5560: move set for blend to top
 * 
 * 5   12/5/11 12:22p erickson
 * SW7405-5560: extend bwin to support no-blend image loading
 * 
 * 4   4/21/11 11:02a jtna
 * SW7425-365: fix build warnings for 2.6.37 kernel
 * 
 * 3   8/4/05 4:42p erickson
 * PR15139: fixed -pedantic warnings
 * 
 * 2   2/15/05 5:23p erickson
 * PR13387: added palette8 support
 * 
 * 1   2/7/05 9:06p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/9   2/7/05 12:07p erickson
 * PR13639: added b8_g8_r8 for bmp support
 * 
 * Irvine_BSEAVSW_Devel/8   1/12/05 12:22p erickson
 * PR13639: fixed debug msg
 * 
 * Irvine_BSEAVSW_Devel/7   1/10/05 5:07p erickson
 * PR13386: added commen
 * 
 * Irvine_BSEAVSW_Devel/6   1/10/05 4:09p erickson
 * PR13386: reworked transform code using byte-per-color-channel
 * assumption which makes code more compact and efficient
 * 
 * Irvine_BSEAVSW_Devel/5   1/6/05 4:11p erickson
 * PR13639: moved shared rendering code into bwin_image and fixed some
 * clipping problems
 * 
 * Irvine_BSEAVSW_Devel/4   1/5/05 3:01p erickson
 * PR13639: added RGB888 support for jpeg, and refactored
 *
 * Irvine_BSEAVSW_Devel/3   9/9/04 1:53p erickson
 * PR12608: convert dbg from private impl to magnum impl
 *
 * Irvine_BSEAVSW_Devel/2   6/24/04 12:24p bandrews
 * PR10889: Added code to not swap pixel bytes for big endian platforms
 *
 * Irvine_BSEAVSW_Devel/1   2/23/04 11:35a bandrews
 * PR9685: Refactored software blt and scale.
 *
 ****************************************************************/

#include "bwin.h"
#include "bwin_image.h"
#include "bwin_image_priv.h"
#include "bwin_transform.h"

#include "bwin_image_debug.h"
BDBG_MODULE(bwin_image_transform);

/**
There are three basic modes.
1) No alpha in the src or dest. In this case, just transform the src and assign
    it into the dest.
2) Alpha in the dest only. In this case, transform the src and write to the
    dest while preserving the dest's alpha.
3) Alpha in the src. There may or may not be alpha in the dest, but in this case,
    the per-pixel operation is expensive already so just process the dest_alphamask
    anyway. In this case, transform the src, transform the dest, blend them 
    together using the src's alpha, then writing the value to dest while preserving 
    the dest's alpha.

We're making the following assumptions about src formats
1) 8 bits per color channel (R, G, B)
2) optional 8 bits of alpha

Therefore here are the elemental steps:
1) decompose dest pixel to bytes
2) blend each color channel for dest & src using src alpha
3) compose the new dest pixel from src bytes
4) add the alpha from the original dest pixel
5) assign to dest

Mode 1 uses steps 3 & 5.
Mode 2 uses steps 3-5.
Mode 3 uses steps 1-5.
**/

void bwin_image_p_render_line(bwin_framebuffer_t fb,
    const bwin_rect *destrect,
    const bwin_rect *clipdestrect,
    int destrow, /* row in fb */
    bwin_image_t image,
    const bwin_rect *srcrect,
    const bwin_image_transform_settings *ts)
{
    uint8_t *src_lineptr = (uint8_t *)image->srcrow_buffer;
    uint8_t *dest_ptr;
    unsigned dest_x;
    int bytes_per_dest_pixel = ts->dest_bpp/8;
    int bytes_per_src_pixel = ts->src_bpp/8;
    bool blend = image->render_mode != bwin_image_render_mode_single_load;

    dest_ptr = (unsigned char *)fb->buffer + (fb->settings.pitch * destrow) + 
        clipdestrect->x * bytes_per_dest_pixel;

    /* This is the inner loop for scaling images. It should be as tight as
    possible, so this needs some work. */
    for (dest_x = 0; dest_x < clipdestrect->width; dest_x++) {
        unsigned src_x = srcrect->x + (dest_x + (clipdestrect->x - destrect->x)) * \
            srcrect->width / destrect->width;
        uint8_t *src_ptr = src_lineptr + (src_x * bytes_per_src_pixel);
        uint8_t alpha = 0xFF, red = 0, green = 0, blue = 0;

        if (ts->src_alpha && blend) {
            switch (ts->dest_format) {
            case bwin_pixel_format_r5_g6_b5:
                DECOMPOSE_RGB565(*(uint16_t*)dest_ptr, red, green, blue);
                break;
            case bwin_pixel_format_a8_r8_g8_b8:
                DECOMPOSE_ARGB8888(*(uint32_t*)dest_ptr,
                    alpha, red, green, blue);
                break;
            case bwin_pixel_format_a1_r5_g5_b5:
                DECOMPOSE_ARGB1555(*(uint16_t*)dest_ptr, alpha, red, green, blue);
                break;
            default: break;
            }
            
            /* blend with src */
            BLEND(red,src_ptr[ts->red_index],src_ptr[ts->alpha_index]);
            BLEND(green,src_ptr[ts->green_index],src_ptr[ts->alpha_index]);
            BLEND(blue,src_ptr[ts->blue_index],src_ptr[ts->alpha_index]);
        }
        else {
            /* assign src */
            if (ts->src_alpha) {
                alpha = src_ptr[ts->alpha_index];
            }
            red = src_ptr[ts->red_index];
            green = src_ptr[ts->green_index];
            blue = src_ptr[ts->blue_index];
        }
        
#if 0
        printf("src: %x,%x,%x (%x) ==> ", src_ptr[ts->red_index],
            src_ptr[ts->green_index], src_ptr[ts->blue_index], src_ptr[ts->alpha_index]);
        printf("dest: %x,%x,%x,%x\n", alpha, red, green, blue);
#endif
        
        switch (ts->dest_format) {
        case bwin_pixel_format_r5_g6_b5:
            *(uint16_t*)dest_ptr = COMPOSE_RGB565(red, green, blue);
            break;
        case bwin_pixel_format_a8_r8_g8_b8:
            *(uint32_t*)dest_ptr = COMPOSE_ARGB8888(alpha, red, green, blue);
            break;
        case bwin_pixel_format_a1_r5_g5_b5:
            *(uint16_t*)dest_ptr = COMPOSE_ARGB1555(alpha, red, green, blue);
            break;
        default: break;
        }
        
        dest_ptr += bytes_per_dest_pixel;
    }

#if 0
    /* temp */
    dest_ptr = fb->buffer + (fb->settings.pitch * destrow) + 
        clipdestrect->x * bytes_per_dest_pixel;
    int x;
    for (x=0;x<destrect->width*bytes_per_dest_pixel;x++)
        printf("%02x ",((uint8_t*)dest_ptr)[x]);
    printf("\n");
#endif
}

void bwin_image_transform_settings_init(
    bwin_image_transform_settings* ts,
    bwin_pixel_format src_format,
    bwin_pixel_format dest_format)
{
    switch ((unsigned)src_format) {
    case bwin_pixel_format_r8_g8_b8:
        /* TODO: verify this on big endian systems for all image
        formats. is the src format in host endianness? */
        ts->red_index = 0;
        ts->green_index = 1;
        ts->blue_index = 2;
        ts->src_alpha = false;
        ts->src_bpp = 24;
        break;
    case bwin_pixel_format_b8_g8_r8:
        /* TODO: verify this on big endian systems for all image
        formats. is the src format in host endianness? */
        ts->red_index = 2;
        ts->green_index = 1;
        ts->blue_index = 0;
        ts->src_alpha = false;
        ts->src_bpp = 24;
        break;
    case bwin_pixel_format_a8_r8_g8_b8:
        ts->alpha_index = 0;
        ts->red_index = 1;
        ts->green_index = 2;
        ts->blue_index = 3;
        ts->src_alpha = true;
        ts->src_bpp = 32;
        break;
    default:
        /* internal unsupported format. something is wrong. */
        BDBG_ASSERT(false);
    }
    
    ts->dest_format = dest_format;
    
    switch (dest_format) {
    case bwin_pixel_format_r5_g6_b5:
    case bwin_pixel_format_a1_r5_g5_b5:
        ts->dest_bpp = 16;
        break;
    case bwin_pixel_format_a8_r8_g8_b8:
        ts->dest_bpp = 32;
        break;
    case bwin_pixel_format_palette8:
        /* should never have come here */
        return;
    }
}
