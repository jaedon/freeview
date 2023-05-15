/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwin_image_bmp.c $
 * $brcm_Revision: 9 $
 * $brcm_Date: 6/14/10 4:30p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwin/src/bwin_image_bmp.c $
 * 
 * 9   6/14/10 4:30p mananp
 * SW7335-726:CLONE -CLONE -Segmentation fault occurs while select Picture
 * folder i
 * n Main Menu to display some bmp pictures
 * 
 * 8   12/19/08 2:52p erickson
 * PR47980: don't support anything but 24 bpp, also protect against buffer
 * overrun
 *
 * 5   6/30/06 2:51p erickson
 * PR21941: fix warning
 *
 * 4   6/2/06 4:58p tokushig
 * PR21686: fixed endian conversion macros which are used to read the bmp
 * header information (always little endian)
 *
 * 2   8/4/05 4:42p erickson
 * PR15139: fixed -pedantic warnings
 *
 * 1   2/7/05 9:04p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/9   2/7/05 12:06p erickson
 * PR13639: fix endianness of bmp's
 *
 * Irvine_BSEAVSW_Devel/8   2/4/05 2:19p erickson
 * PR13639: fixed bmp pitch and bottom-to-top buffer handling
 *
 * Irvine_BSEAVSW_Devel/7   1/10/05 4:09p erickson
 * PR13386: rework for image-rendering changs
 *
 * Irvine_BSEAVSW_Devel/6   1/6/05 4:11p erickson
 * PR13639: moved shared rendering code into bwin_image and fixed some
 * clipping problems
 *
 * Irvine_BSEAVSW_Devel/5   1/6/05 10:38a erickson
 * PR13761: fix i386 compiler errors
 *
 * Irvine_BSEAVSW_Devel/4   1/5/05 4:29p erickson
 * PR13639: added bmp support
 *
 * Irvine_BSEAVSW_Devel/3   9/9/04 1:53p erickson
 * PR12608: convert dbg from private impl to magnum impl
 *
 * Irvine_BSEAVSW_Devel/2   2/3/04 7:47p bandrews
 * PR9107: Fixed unused warnings.  Added render mode.
 *
 * Irvine_BSEAVSW_Devel/1   1/30/04 4:42p bandrews
 * PR9107: BWIN Image API
 *
 ****************************************************************/

#include "bwin.h"
#include "bwin_image.h"
#include "bwin_image_priv.h"

/* set the debug to a higher level for more debug output */
#include "bwin_image_debug.h"
BDBG_MODULE(bwin_image_bmp);

typedef struct {
    unsigned short int type;                 /* Magic identifier            */
    unsigned int size;                       /* File size in bytes          */
    unsigned short int reserved1, reserved2;
    unsigned int offset;                     /* Offset to image data, bytes */
} BMP_HEADER;

typedef struct {
    unsigned int size;               /* Header size in bytes      */
    int width,height;                /* Width and height of image */
    unsigned short int planes;       /* Number of colour planes   */
    unsigned short int bits;         /* Bits per pixel            */
    unsigned int compression;        /* Compression type          */
    unsigned int imagesize;          /* Image size in bytes       */
    int xresolution,yresolution;     /* Pixels per meter          */
    unsigned int ncolours;           /* Number of colours         */
    unsigned int importantcolours;   /* Important colours         */
} BMP_INFOHEADER;

struct bwin_image_data {
    BMP_HEADER header;
    BMP_INFOHEADER info;
};

/* Access the data by bytes so we don't do un-aligned accesses */
#define B_GET_U16(x) (x[1]<<8  | x[0]<<0)
#define B_GET_U32(x) (x[3]<<24 | x[2]<<16 | x[1]<<8 | x[0]<<0)

static bwin_result bwin_image_bmp_init(bwin_image_t image)
{
    struct bwin_image_data *bmp;
    const unsigned char *ptr;

    bmp = (struct bwin_image_data *)BKNI_Malloc(sizeof(struct bwin_image_data));
    if (!bmp)
        return bwin_result_allocation_error;
    BKNI_Memset(bmp, 0, sizeof(struct bwin_image_data));

    /* BMP header format is little endian, access the data appropriate for the platform. */
    ptr = image->buffer;
    bmp->header.type      = B_GET_U16(ptr); ptr += 2;
    bmp->header.size      = B_GET_U32(ptr); ptr += 4;
    bmp->header.reserved1 = B_GET_U16(ptr); ptr += 2;
    bmp->header.reserved2 = B_GET_U16(ptr); ptr += 2;
    bmp->header.offset    = B_GET_U32(ptr); ptr += 4;

    bmp->info.size        = B_GET_U32(ptr); ptr += 4;
    bmp->info.width       = B_GET_U32(ptr); ptr += 4;
    bmp->info.height      = B_GET_U32(ptr); ptr += 4;
    bmp->info.planes      = B_GET_U16(ptr); ptr += 2;
    bmp->info.bits        = B_GET_U16(ptr); ptr += 2;
    bmp->info.compression = B_GET_U32(ptr); ptr += 4;
    bmp->info.imagesize   = B_GET_U32(ptr); ptr += 4;
    bmp->info.xresolution = B_GET_U32(ptr); ptr += 4;
    bmp->info.yresolution = B_GET_U32(ptr); ptr += 4;
    bmp->info.ncolours    = B_GET_U32(ptr); ptr += 4;
    bmp->info.importantcolours = B_GET_U32(ptr); ptr += 4;

    image->data = bmp;

    BDBG_MSG(("image %x, size=%d, offset=%d",
        image->data->header.type, image->data->header.size, image->data->header.offset));
    BDBG_MSG(("info %d, %dx%d",
        image->data->info.size, image->data->info.width, image->data->info.height));
#if 0
    BDBG_MSG(("planes=%d, bits=%d, compression=%d, imagesize=%d, xres=%d, yres=%d, #colors=%d, important colors=%d",
              bmp->info.planes, bmp->info.bits, bmp->info.compression, bmp->info.imagesize,
              bmp->info.xresolution, bmp->info.yresolution, bmp->info.ncolours, bmp->info.importantcolours));
#endif

    /* Here's where all of our limitations come in. */
    if (image->data->info.compression) {
        BDBG_ERR(("file '%s': bmp decompression %d not supported.", image->filename, image->data->info.compression));
        goto error;
    }
    if (image->data->info.ncolours) {
        BDBG_ERR(("file '%s': palettized bmp's not supported. (ncolors=%d)", image->filename, image->data->info.ncolours));
        goto error;
    }
    if (image->data->info.bits != 24) {
        BDBG_ERR(("file '%s': %d bpp not supported.", image->filename, image->data->info.bits));
        goto error;
    }

    image->settings.width = image->data->info.width;
    image->settings.height = image->data->info.height;
    image->pitch = image->data->info.width * 3;
    if (image->pitch % 4)
        image->pitch += 4 - (image->pitch % 4);
    return 0;

  error:
    BKNI_Free(image->data);
    image->data = 0;
    return -1;
}

static void bwin_image_bmp_finalize(bwin_image_t image)
{
    if(image->data!=NULL) {
    BKNI_Free(image->data);
    image->data = NULL;
	}
}

static int get_bmp_src_line(bwin_image_t image, int srcrow)
{
    if (image->srcrow != srcrow) {
        image->srcrow = srcrow;
        /* the bmp buffer starts with the bottom row. */
        image->srcrow_buffer = image->buffer + image->data->header.offset +
            (image->pitch * (image->settings.height - image->srcrow - 1));

        /* ensure here that we aren't going to overrun because of bad metadata */
        if (image->srcrow_buffer + image->pitch - image->buffer > image->size) {
            image->srcrow_buffer = image->buffer + image->data->header.offset;
        }
    }
    return 0;
}

static void bwin_image_bmp_render(
    bwin_framebuffer_t fb,
    bwin_image_t image,
    const bwin_rect *destrect,
    const bwin_rect *srcrect,
    const bwin_rect *clipdestrect /* guaranteed to be inside destrect */
    )
{
    bwin_image_transform_settings transform_settings;
    BKNI_Memset(&transform_settings, 0, sizeof(bwin_image_transform_settings));

    bwin_image_transform_settings_init(&transform_settings,
        bwin_pixel_format_b8_g8_r8, fb->settings.pixel_format);

    bwin_image_p_render_lines(fb, image, destrect, srcrect, clipdestrect,
        &transform_settings, get_bmp_src_line);
}

int bwin_image_p_is_bmp(bwin_image_t image)
{
    int rc;
    if ((unsigned)image->size < sizeof(BMP_HEADER) + sizeof(BMP_INFOHEADER))
        return -1;

    rc = BKNI_Memcmp(image->buffer, "BM", 2);
    if (!rc) {
        image->init = bwin_image_bmp_init;
        image->render = bwin_image_bmp_render;
        image->finalize = bwin_image_bmp_finalize;
    }
    return rc;
}
