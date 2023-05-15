/***************************************************************************
 *     Copyright (c) 2004-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwin_image.c $
 * $brcm_Revision: 20 $
 * $brcm_Date: 12/5/11 12:22p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwin/src/bwin_image.c $
 * 
 * 20   12/5/11 12:22p erickson
 * SW7405-5560: extend bwin to support no-blend image loading
 * 
 * 19   9/15/11 8:47a dlwin
 * SW7335-1354:  Fix for Coverity: 33636 and Coverity: 33637
 * 
 * 18   6/30/11 4:56p erickson
 * SW7405-5356: change memcpy to strncpy to avoid segfault
 * 
 * 17   5/22/07 2:52p mward
 * PR31384:  Free pEXIF always, even if pEXIF->size == 0.  Memory leak
 * otherwise.
 * 
 * 15   12/11/06 11:39a jgarrett
 * PR 20139: Fixing compiler warnings
 * 
 * 13   11/3/06 9:16a erickson
 * PR25522: if bwin_image_load fails midway, just call bwin_image_close.
 * previous code was doing double fclose's.
 * 
 * 12   9/14/06 3:26p tokushig
 * PR21847: handle case where attemping to render image into an invalid
 * window
 * 
 * SanDiego_Brutus_Skin/10   9/14/06 3:20p tokushig
 * handle case where attempting to render an image to an invalid window
 * 
 * SanDiego_Brutus_Skin/9   9/13/06 5:47p tokushig
 * merge from main
 * 
 * 11   8/25/06 1:26p tokushig
 * PR22347: in non-skinned brutus, images w/o embedded thumbnails will
 * display full image as thumb.  if using a skin, small images w/o
 * embedded thumbnails will display full image as thumb but large images
 * w/o embedded thumbnails will display a placeholder icon instead
 * (images are currently considered large if they have more than 640 *
 * 480 = 307200pixels.)  thumbnail please wait popup was replaced by
 * sequential drawing of thumbs which provides better feedback to users.
 * implemented image caching on thumbs (default: last 30) and for full
 * screen image view (default: last 3).
 * 
 * SanDiego_Brutus_Skin/8   8/24/06 5:49p tokushig
 * fixed return value for bwin_image_render_thumb()
 * 
 * SanDiego_Brutus_Skin/7   8/23/06 5:05p tokushig
 * added return code to bwin_image_render_thumb()
 * 
 * 10   8/7/06 12:11p rjlewis
 * PR23292: print the file name when displaying an error.
 * 
 * 8   5/31/06 4:19p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/6   5/16/06 10:17a tokushig
 * Merge from main
 * 
 * SanDiego_Brutus_Skin/5   5/4/06 11:50a tokushig
 * added functions to draw image thumbnails using EXIF data if available
 * 
 * SanDiego_Brutus_Skin/4   4/27/06 10:56a tokushig
 * fixed bwin_image_load_memory() to make local copy of given image memory
 * 
 * 6   4/17/06 5:08p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/3   4/4/06 11:11a tokushig
 * add handling for loading images from file without saving compressed
 * image in memory (only save file pointer)
 * 
 * SanDiego_Brutus_Skin/2   3/28/06 2:46p tokushig
 * loading jpeg images no longer requires that the entire compressed image
 * be loaded into memory.  the jpeg decoder will access the file
 * directly.  this will allow brutus to decode large jpeg images in
 * limited memory space.
 * 
 * SanDiego_Brutus_Skin/1   3/7/06 2:26p tokushig
 * added handling for proportional maximizing, and proportional
 * downscaling only for images.
 * 
 * 5   8/24/05 6:16p arbisman
 * PR16678: Add support for WinCE
 * 
 * 4   8/4/05 4:42p erickson
 * PR15139: fixed -pedantic warnings
 * 
 * 3   2/17/05 4:22p erickson
 * PR14180: improve #if's
 * 
 * 2   2/15/05 5:23p erickson
 * PR13387: added palette8 support
 * 
 * 1   2/7/05 9:04p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/20   1/14/05 12:18p biyongc
 * PR2807: BSE PR - update for VxWorks build
 * 
 * Irvine_BSEAVSW_Devel/19   1/10/05 4:09p erickson
 * PR13386: rework for image-rendering changs
 * 
 * Irvine_BSEAVSW_Devel/18   1/6/05 4:11p erickson
 * PR13639: moved shared rendering code into bwin_image and fixed some
 * clipping problems
 * 
 * Irvine_BSEAVSW_Devel/17   1/6/05 10:38a erickson
 * PR13761: fix i386 compiler errors
 * 
 * Irvine_BSEAVSW_Devel/16   1/5/05 4:29p erickson
 * PR13639: added bmp support
 * 
 * Irvine_BSEAVSW_Devel/15   1/5/05 3:00p erickson
 * PR13639: moved clipping and conversion code into generic render
 * function to simplify lower layers
 * 
 * Irvine_BSEAVSW_Devel/14   12/22/04 5:05p erickson
 * PR13639: reworked image handling internals in preparation for adding
 * jpeg
 * 
 * Irvine_BSEAVSW_Devel/13   12/13/04 11:31a erickson
 * PR13495: simplified some code
 * 
 * Irvine_BSEAVSW_Devel/12   9/9/04 1:52p erickson
 * PR12608: convert dbg from private impl to magnum impl
 * 
 * Irvine_BSEAVSW_Devel/11   2/25/04 11:36a bandrews
 * PR9882: Checked the return codes while trying to read from a file. PNG
 * ignores these, but we don't have to.
 * 
 * Irvine_BSEAVSW_Devel/10   2/25/04 9:11a erickson
 * PR9882: check if image->reset fails (the file might not be present) and
 * exit cleanly with an error code
 * 
 * Irvine_BSEAVSW_Devel/9   2/17/04 7:15p bandrews
 * PR9723: Added 1555 support to bwin_image api
 * 
 * Irvine_BSEAVSW_Devel/8   2/3/04 7:46p bandrews
 * PR9107: Added render mode to render function.
 * 
 * Irvine_BSEAVSW_Devel/7   2/3/04 10:40a bandrews
 * PR9107: Fixed comment.
 * 
 * Irvine_BSEAVSW_Devel/6   2/3/04 10:36a bandrews
 * PR9107: Removed mode for now.  Adding back later.
 * 
 * Irvine_BSEAVSW_Devel/5   2/3/04 10:32a bandrews
 * PR9107: Fixed image api to bwin_image_*.
 * 
 * Irvine_BSEAVSW_Devel/4   1/30/04 4:42p bandrews
 * PR9107: BWIN Image API
 *
 * Irvine_BSEAVSW_Devel/2   1/19/04 9:02a erickson
 * PR9107: getting ready for BRUTUS_PR9107 branch
 *
 * Irvine_BSEAVSW_Devel/1   1/14/04 11:36p erickson
 * PR9107: freetype support added to iolaus and still doing initial
 * development
 *
 ****************************************************************/

#include <stdio.h>
#include <string.h>

#include "bwin.h"
#include "bwin_image.h"
#include "bwin_image_priv.h"
#include "bwin_image_debug.h"
#include "bwin_transform.h"
#ifdef EXIF_SUPPORT
#include "exif-data.h"
#endif
BDBG_MODULE(bwin_image);

void bwin_image_settings_init(bwin_image_settings *settings)
{
    memset(settings, 0, sizeof(*settings));
}

#ifdef PNG_SUPPORT
extern int bwin_image_p_is_png(bwin_image_t image);
#endif
#ifdef JPEG_SUPPORT
extern int bwin_image_p_is_jpeg(bwin_image_t image);
#endif
extern int bwin_image_p_is_bmp(bwin_image_t image);

static int bwin_image_p_discover_type(bwin_image_t image)
{
    /* if extension type id failed */
#ifdef PNG_SUPPORT
    if (!bwin_image_p_is_png(image)) {
        BDBG_MSG(("File '%s': image is png", image->filename));
        goto reset;
    }
#endif
#ifdef JPEG_SUPPORT
    if (!bwin_image_p_is_jpeg(image)) {
        BDBG_MSG(("File '%s': image is jpeg", image->filename));
        goto reset;
    }
#endif
    if (!bwin_image_p_is_bmp(image)) {
        BDBG_MSG(("File '%s': image is bmp", image->filename));
        goto reset;
    }

    /* We can always try using the file extension, but I'm not sure if
    it's worth it at this point. */
    BDBG_ERR(("File '%s': unable to determine image type", image->filename));
    return -1;

reset:
    /* we've read some data, but now we need to back up for a normal read */
    image->pos = 0;
    return 0;
}

static bwin_image_t bwin_image_p_init(bwin_engine_t engine, bwin_image_t image)
{
    image->pos = 0;
    image->srcrow = -1; /* srcrow_buffer is invalid */

    /* determine the type */
    if (bwin_image_p_discover_type(image))
        goto error;

    /* initialize the image data structure */
    if (image->init(image))
        goto error;

    image->settings.thumb_width  = image->settings.width;
    image->settings.thumb_height = image->settings.height;

#ifndef EXIF_SUPPORT
    (void)engine; /* touch it so we don't get warning */
#else
    {
        ExifData * pEXIF = NULL;

        /* look up true thumbnail width/height
         * we will unload the thumb after getting the width/height to save memory.
         * note that the thumbnail generation will be loaded on demand (when 
         * rendering).  this will save memory (thumbnails are technically not
         * limited to any particular size) and since thumbnails are 
         * typically very small, will not significantly hamper rendering 
         * performance.
         */
        if (image->fpImage)
            pEXIF = exif_data_new_from_file(image->filename);
        else if ((image->buffer) && (image->size > 0))
            pEXIF = exif_data_new_from_data(image->buffer, image->size);

        if ((pEXIF) && (pEXIF->size > 0))
        {
            bwin_image_t tmpImage = NULL;
            bwin_image_settings tmpSettings;

            tmpImage = bwin_image_load_memory(engine, (void *)pEXIF->data, (size_t)pEXIF->size);
            bwin_image_get_settings(tmpImage, &tmpSettings);

            image->settings.thumb_width  = tmpSettings.width;
            image->settings.thumb_height = tmpSettings.height;

            bwin_image_close(tmpImage);
        }
        if (pEXIF)
        {
            exif_data_free(pEXIF);
        }
    }
#endif

    return image;

error:
    bwin_image_close(image);
    return NULL;
}

#if defined(__unix__)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#elif defined(__vxworks)
#include  <sys/stat.h>
#elif defined(_WIN32_WCE)
#include <windows.h>
#include <Winbase.h>
#endif

static int b_filesize(const char *filename)
{
#ifndef _WIN32_WCE
    struct stat st;
    if (stat((char*)filename, &st))
        return -1;
    return st.st_size;
#else
    WIN32_FILE_ATTRIBUTE_DATA   fileData;
    TCHAR*     wchName;
    BOOL    bRslt;
    int str_size = strlen(filename);
    
    wchName = (TCHAR*)malloc(sizeof(TCHAR) *(str_size + 1));
    mbstowcs(wchName, filename, str_size+1);
    bRslt = GetFileAttributesEx(wchName, GetFileExInfoStandard, &fileData);
    free(wchName);
    if( bRslt)
        return (int)fileData.nFileSizeLow;
    else
        return -1;
#endif /* _WIN32_WCE */
}

/* This opens the file but doesn't read it in (yet) */
bwin_image_t bwin_image_file_io(bwin_engine_t win, const char *filename)
{
    bwin_image_t image;
    FILE *file;

    int size = b_filesize(filename);

    BSTD_UNUSED(win); /* TODO: Use win, store in struct? */

    if (size == -1) {
        BDBG_ERR(("Unable to get filesize for file '%s'", filename));
        return NULL;
    }
    BDBG_MSG(("getting %d bytes from file '%s'", size, filename));

    image = (bwin_image_t)BKNI_Malloc(sizeof(*image));
    if (!image)
        return NULL;
    memset(image, 0, sizeof(*image));

    image->size = size;

    file = fopen(filename, "rb");
    if (!file)
        goto error;
    image->fpImage = file;
    /* SW7335-1354, Coverity: 33636 */
    strncpy(image->filename, filename, sizeof(image->filename)-1);

    return bwin_image_p_init(win, image);

error:
    bwin_image_close(image);
    return NULL;
}

/* This opens the file, reads it into a buffer, and closes the file */
bwin_image_t bwin_image_load(bwin_engine_t win, const char *filename)
{
    bwin_image_t image;
    FILE *file;
    
    int size = b_filesize(filename);
    
    BSTD_UNUSED(win); /* TODO: Use win, store in struct? */
    
    if (size == -1) {
        BDBG_ERR(("Unable to read filesize of file '%s'", filename));
        return NULL;
    }
    BDBG_MSG(("reading %d bytes from file '%s'", size, filename));
    
    image = (bwin_image_t)BKNI_Malloc(sizeof(*image));
    if (!image)
        return NULL;
    memset(image, 0, sizeof(*image));
    
    image->size = size;

    image->buffer = BKNI_Malloc(size);
    if (!image->buffer)
        goto error;
    image->allocated = true;
    
    file = fopen(filename, "rb");
    if (!file)
        goto error;
    image->fpImage = file;
    /* SW7335-1354, Coverity: 33637 */
    strncpy(image->filename, filename, sizeof(image->filename)-1);
    
    /* read the entire file into memory */
    if ((size_t)fread((void*)image->buffer, 1, size, file) != (size_t)size)
        goto error;
    fclose(file);
    image->fpImage = NULL;
    
    return bwin_image_p_init(win, image);
    
error:
    bwin_image_close(image);
    return NULL;
}

bwin_image_t bwin_image_load_memory(bwin_engine_t win, const void *mem, size_t length)
{
    bwin_image_t image;
    BSTD_UNUSED(win); /* TODO: Use win, store in struct? */

    image = (bwin_image_t)BKNI_Malloc(sizeof(*image));
    if (!image)
        return NULL;
    memset(image, 0, sizeof(*image));

    image->buffer = BKNI_Malloc(length);
    if (!image->buffer) {
        BKNI_Free(image);
        return NULL;
    }

    BKNI_Memcpy((void *)image->buffer, mem, length);
    image->allocated = true;
    image->size = length;

    return bwin_image_p_init(win, image);
}

void bwin_image_get_settings(bwin_image_t image, bwin_image_settings *settings)
{
    *settings = image->settings;
}

void bwin_image_close(bwin_image_t image)
{
    if (!image) return;
    
    BDBG_MSG(("closing image %p (buffer=%p, size=%d, file='%s')",
              image, image->buffer, image->size, image->fpImage?image->filename:"none"));

    if (image->finalize)
        image->finalize(image);
    if (image->allocated)
        BKNI_Free((void*)image->buffer);
    if (image->fpImage)
    {
        fclose(image->fpImage);
        image->filename[0]='\0';
    }

    BKNI_Free(image);
}

int bwin_image_render_thumb(bwin_t window, bwin_image_t image,
                             bwin_image_render_mode mode,
                             const bwin_rect *destrect, 
                             const bwin_rect *cliprect)
{
    int bRet = 0;

    if (cliprect && cliprect->width == 0 && cliprect->height == 0)
    {
        /* nothing to draw so don't bother rendering */
        BDBG_WRN(("bwin_image_render_thumb: cancelling render request because cliprect area is 0"));
        return bRet;
    }

    if (!window)
    {
        BDBG_WRN(("bwin_image_render_thumb: cancelling render request because given window is invalid"));
        return bRet;
    }

    if (!image)
    {
        BDBG_WRN(("bwin_image_render_thumb: cancelling render request because given image is invalid"));
        return bRet;
    }

#ifdef EXIF_SUPPORT
    {
        /* extract thumb if possible */
        ExifData * pEXIF = NULL;

        if (image->fpImage)
            pEXIF = exif_data_new_from_file(image->filename);
        else if ((image->buffer) && (image->size > 0))
            pEXIF = exif_data_new_from_data(image->buffer, image->size);

        if ((pEXIF) && (pEXIF->size > 0))
        {
            bwin_image_t tmpImage = NULL;
            tmpImage = bwin_image_load_memory(window->fb->win_engine, pEXIF->data, pEXIF->size);
            bwin_image_render(window, tmpImage, mode, destrect, NULL, cliprect);
            bwin_image_close(tmpImage);
            exif_data_free(pEXIF);
            bRet = 1;
        }
    }
#else
    /* exif not supported so just render full sized image */
    bwin_image_render(window, image, mode, destrect, NULL, cliprect);
    bRet = 1;
#endif

    return bRet;
}

void bwin_image_render(bwin_t window, bwin_image_t image,
    bwin_image_render_mode mode,
    const bwin_rect *destrect, const bwin_rect *srcrect,
    const bwin_rect *cliprect)
{
    /* default to whole src -> whole window */
    bwin_rect actual_srcrect = {0,0,0,0};
    bwin_rect actual_destrect;
    bwin_rect gcliprect;

    if (!window)
    {
        BDBG_WRN(("bwin_image_render: cancelling render request because given window is invalid"));
        return;
    }

    if (!image)
    {
        BDBG_WRN(("bwin_image_render: cancelling render request because given image is invalid"));
        return;
    }

    if (cliprect && cliprect->width == 0 && cliprect->height == 0)
    {
        /* nothing to draw so don't bother rendering */
        BDBG_WRN(("bwin_image_render: cancelling render request because cliprect area is 0"));
        return;
    }

    actual_destrect = window->settings.rect;
    
    actual_srcrect.width = image->settings.width;
    actual_srcrect.height = image->settings.height;
    
    if (window->fb->settings.pixel_format == bwin_pixel_format_palette8) {
        BDBG_ERR(("Cannot render image to palettized surface"));
        return;
    }

    /* src coordiates are always relative to the image only */
    if (srcrect)
        bwin_intersect_rect(&actual_srcrect, &actual_srcrect, srcrect);

    /* dest coordinates are relative to the window and must be clipped and converted */
    if (destrect)
        bwin_intersect_rect(&actual_destrect, &actual_destrect, destrect);

    bwin_p_convert_to_global(window, actual_destrect.x, actual_destrect.y,
        &actual_destrect.x, &actual_destrect.y);

    /* compute global clipping rect */
    if (!cliprect)
        cliprect = &window->settings.rect;
    gcliprect = *cliprect;
    bwin_p_convert_to_global(window, cliprect->x, cliprect->y, &gcliprect.x, &gcliprect.y);
    /* clip to the framebuffer */
    bwin_intersect_rect(&gcliprect, &gcliprect, &window->fb->rect);

    image->render_mode = mode;
    switch (mode) {
    case bwin_image_render_mode_single:
    case bwin_image_render_mode_single_load:
        /* intersect the dest and cliprect now to get the correct final region */
        bwin_intersect_rect(&gcliprect, &gcliprect, &actual_destrect);
        /* then set the dest size to be the same as the src size */
        actual_destrect.width = actual_srcrect.width;
        actual_destrect.height = actual_srcrect.height;
        /* fall through */
    case bwin_image_render_mode_stretch:
    case bwin_image_render_mode_maximize:
    case bwin_image_render_mode_maximize_down:
        {
        bwin_rect clipdestrect;
        bwin_intersect_rect(&clipdestrect, &gcliprect, &actual_destrect);
        BDBG_MSG(("image->render %d,%d,%d,%d (%d,%d,%d,%d)",
            actual_destrect.x, actual_destrect.y, actual_destrect.width, actual_destrect.height,
            clipdestrect.x, clipdestrect.y, clipdestrect.width, clipdestrect.height));
        image->render(window->fb, image, &actual_destrect, &actual_srcrect, &clipdestrect);
        }
        break;
    case bwin_image_render_mode_tile:
        {
        unsigned x,y;
        for (x=0; x<actual_destrect.width; x+=actual_srcrect.width)
            for (y=0; y<actual_destrect.height; y+=actual_srcrect.height) {
                bwin_rect tiled_destrect;
                bwin_rect clipdestrect;
                
                tiled_destrect.x = actual_destrect.x+x;
                tiled_destrect.y = actual_destrect.y+y;
                tiled_destrect.width = actual_srcrect.width;
                tiled_destrect.height = actual_srcrect.height;
                
                bwin_intersect_rect(&clipdestrect, &gcliprect, &tiled_destrect);
                image->render(window->fb, image, &tiled_destrect, &actual_srcrect, &clipdestrect);
            }
        }
        break;
    }
}

/**
This function is called from the image-specific render function 
in order to render all the lines. It requires a callback to the image-specific
module in order to get each source line.
**/
void bwin_image_p_render_lines(
    bwin_framebuffer_t fb,
    bwin_image_t image,
    const bwin_rect *destrect,
    const bwin_rect *srcrect,
    const bwin_rect *clipdestrect,
    const bwin_image_transform_settings *transform_settings, 
    bwin_image_get_src_line_func get_src_line)
{
    unsigned destrow;
    
    /* invalidate the buffer before call the get_src_line callback */
    image->srcrow = -1;

    /* step through each line of the final destination */   
    for (destrow=0;destrow<clipdestrect->height;destrow++) {
        /* compute src row for this destrow */
        int srcrow = srcrect->y + (destrow + (clipdestrect->y - destrect->y)) * srcrect->height / destrect->height;
/*        BDBG_MSG(("row %d ==> %d", srcrow, destrow)); */

        /* request the line from the image-specific module */
        if (get_src_line(image, srcrow))
            break;
            
#if 0
        BDBG_MSG(("line %x %x %x %x", 
            (uint8_t*)image->srcrow_buffer[0],
            (uint8_t*)image->srcrow_buffer[1],
            (uint8_t*)image->srcrow_buffer[2],
            (uint8_t*)image->srcrow_buffer[3]));
#endif
            
        /* now render the line */
        bwin_image_p_render_line(fb, destrect, clipdestrect,
            destrow+clipdestrect->y, image, srcrect, transform_settings);
    }
}
