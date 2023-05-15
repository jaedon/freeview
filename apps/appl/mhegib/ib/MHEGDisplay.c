/*
 * MHEGDisplay.c
 */

#include <stdio.h>
#if !defined(WIN32)
#include <stdbool.h>
#include <stdint.h>
#endif
#include <errno.h>
#include <png.h>

#include "MHEGEngine.h"
#include "MHEGDisplay.h"
#include "readpng.h"
#include "utils.h"

/* internal utils */

#define TRANSPARENT_COLOR			0

#if defined(WIN32) || defined(CONFIG_PROD_ENDIAN_LITTLE)
static unsigned int local_ByteSwap_32(unsigned int x)
{
	x = ((x<<8)&0xFF00FF00) | ((x>>8)&0x00FF00FF);
	return (x>>16) | (x<<16);
}
#endif

/*
 * convert MHEGColour to internal format
 */

static void
display_colour(int *out, MHEGColour *in)
{
	MHEGIB_OSD_GetColor(out, in->r, in->g, in->b, in->t);
}

int
MHEGDisplay_init(MHEGDisplay *d)
{
	/* size of the Window */
	/* resolution defined in UK MHEG Profile */
	d->xres = MHEG_XRES;
	d->yres = MHEG_YRES;

	/*
	 * create an XRender Picture for the Window contents
	 * we composite the video frame and the MHEG overlay onto this, then copy it onto the Window
	 */
//	d->contents = MHEGIB_CreatePixmap(d->win, d->xres, d->yres, d->depth);
//	d->contents_pic = MHEGIB_RenderCreatePicture(d->contents);

	return 0;
}

int
MHEGDisplay_fini(MHEGDisplay *d)
{
	return 0;
}

/*
 * gets the given area of the Window refreshed
 * coords should be in the range 0-MHEG_XRES, 0-MHEG_YRES
 */

void
MHEGDisplay_refresh(MHEGDisplay *d, XYPosition *pos, OriginalBoxSize *box)
{
	int x, y;
	unsigned int w, h;

	verbose_osd("%s", __FUNCTION__);

	/* scale if fullscreen */
	x = MHEGDisplay_scaleX(d, pos->x_position);
	y = MHEGDisplay_scaleY(d, pos->y_position);
	w = MHEGDisplay_scaleX(d, box->x_length);
	h = MHEGDisplay_scaleY(d, box->y_length);

	/* copy the Window contents onto the screen */
	MHEGIB_UpdateScreen(0, 0, MHEG_XRES, MHEG_YRES);

	return;
}

void
MHEGDisplay_clearScreen(MHEGDisplay *d)
{
	MHEGIB_DrawRectFill(0, 0, MHEG_XRES, MHEG_YRES, MHEG_COLOR_TRANS);
	MHEGIB_UpdateScreen(0, 0, MHEG_XRES, MHEG_YRES);

	return;
}

/*
 * all these drawing routines draw onto next_overlay_pic
 * all coords should be in the range 0-MHEG_XRES, 0-MHEG_YRES
 * the drawing routines themselves will scale the coords to full screen if needed
 * you have to call MHEGDisplay_useOverlay() when you have finished drawing
 * this copies next_overlay onto used_overlay
 * used_overlay_pic is composited onto any video and put on the screen by MHEGDisplay_refresh()
 */

/*
 * set the clip rectangle for all subsequent drawing on the overlay
 * coords should be in the range 0-MHEG_XRES, 0-MHEG_YRES
 */

void
MHEGDisplay_setClipRectangle(MHEGDisplay *d, XYPosition *pos, OriginalBoxSize *box)
{
	// hmkim : commented out.
}

/*
 * remove the clip rectangle from the overlay
 */

void
MHEGDisplay_unsetClipRectangle(MHEGDisplay *d)
{
	// hmkim : commented out.
}

/*
 * coords should be in the range 0-MHEG_XRES, 0-MHEG_YRES
 * width is the line width in pixels
 * style should be LineStyle_solid/dashed/dotted
 */

void
MHEGDisplay_drawHoriLine(MHEGDisplay *d, XYPosition *pos, unsigned int len, int width, int style, MHEGColour *col)
{
	int rcol;
	int x, y;
	unsigned int w, h;

	/* if it is transparent or line width is <=0, just bail out */
	if(col->t == MHEGCOLOUR_TRANSPARENT || width <= 0)
		return;

	/* convert to internal colour format */
	display_colour(&rcol, col);

	/* scale if fullscreen */
	x = MHEGDisplay_scaleX(d, pos->x_position);
	y = MHEGDisplay_scaleY(d, pos->y_position);
	w = MHEGDisplay_scaleX(d, len);
	/* aspect ratio */
	h = MHEGDisplay_scaleY(d, width);

	if(style != LineStyle_solid)
		verbose_todo("%s; LineStyle %d", __FUNCTION__ , style);

	/* draw a rectangle */
	MHEGIB_DrawRectFill(x, y, w, h, rcol);

	return;
}

/*
 * coords should be in the range 0-MHEG_XRES, 0-MHEG_YRES
 * width is the line width in pixels
 * style should be LineStyle_solid/dashed/dotted
 */

void
MHEGDisplay_drawVertLine(MHEGDisplay *d, XYPosition *pos, unsigned int len, int width, int style, MHEGColour *col)
{
	int rcol;
	int x, y;
	unsigned int w, h;

	/* if it is transparent or line width is <=0, just bail out */
	if(col->t == MHEGCOLOUR_TRANSPARENT || width <= 0)
		return;

	/* convert to internal colour format */
	display_colour(&rcol, col);

	/* scale if fullscreen */
	x = MHEGDisplay_scaleX(d, pos->x_position);
	y = MHEGDisplay_scaleY(d, pos->y_position);
	h = MHEGDisplay_scaleY(d, len);
	/* aspect ratio */
	w = MHEGDisplay_scaleX(d, width);

	if(style != LineStyle_solid)
		verbose_todo("%s; LineStyle %d", __FUNCTION__ , style);

	/* draw a rectangle */
	MHEGIB_DrawRectFill(x, y, w, h, rcol);

	return;
}

/*
 * coords should be in the range 0-MHEG_XRES, 0-MHEG_YRES
 */

void
MHEGDisplay_fillRectangle(MHEGDisplay *d, XYPosition *pos, OriginalBoxSize *box, MHEGColour *col)
{
	int rcol;
	int x, y;
	unsigned int w, h;

	/* if it is transparent, just bail out */
	if(col->t == MHEGCOLOUR_TRANSPARENT)
	{
		return;
	}

	/* convert to internal colour format */
	display_colour(&rcol, col);

	/* scale if fullscreen */
	x = MHEGDisplay_scaleX(d, pos->x_position);
	y = MHEGDisplay_scaleY(d, pos->y_position);
	w = MHEGDisplay_scaleX(d, box->x_length);
	h = MHEGDisplay_scaleY(d, box->y_length);

	MHEGIB_DrawRectFill(x, y, w, h, rcol);

	return;
}

/*
 * explicitly make a transparent rectangle in the MHEG overlay
 * MHEGDisplay_fillRectangle() uses PictOpOver => it can't create a transparent box in the output
 * this uses PictOpSrc
 */

void
MHEGDisplay_fillTransparentRectangle(MHEGDisplay *d, XYPosition *pos, OriginalBoxSize *box)
{
	int rcol = TRANSPARENT_COLOR;
	int x, y;
	unsigned int w, h;

	/* scale if fullscreen */
	x = MHEGDisplay_scaleX(d, pos->x_position);
	y = MHEGDisplay_scaleY(d, pos->y_position);
	w = MHEGDisplay_scaleX(d, box->x_length);
	h = MHEGDisplay_scaleY(d, box->y_length);

	MHEGIB_DrawRectFill(x, y, w, h, rcol);

	return;
}

/*
 * coords should be in the range 0-MHEG_XRES, 0-MHEG_YRES
 */

void
MHEGDisplay_drawBitmap(MHEGDisplay *d, XYPosition *src, OriginalBoxSize *box, MHEGBitmap *bitmap, XYPosition *dst, bool tiling)
{
	int src_x, src_y;
	int dst_x, dst_y;
	unsigned int w, h;

	verbose_osd("%s", __FUNCTION__);

	/* in case we don't have any content yet, UK MHEG Profile says make it transparent */
	if(bitmap == NULL)
		return;

	/*
	 * scale up if fullscreen
	 * the bitmap itself is scaled when it is created in MHEGDisplay_newBitmap()
	 */
 	verbose_osd("[MHEGDisplay_drawBitmap] ORG src [%d, %d], dst[%d, %d], box[%d, %d]\n", src->x_position, src->y_position, dst->x_position, dst->y_position, box->x_length, box->y_length);

	src_x = MHEGDisplay_scaleX(d, src->x_position);
	src_y = MHEGDisplay_scaleY(d, src->y_position);
	w = MHEGDisplay_scaleX(d, box->x_length);
	h = MHEGDisplay_scaleY(d, box->y_length);
	dst_x = MHEGDisplay_scaleX(d, dst->x_position);
	dst_y = MHEGDisplay_scaleY(d, dst->y_position);

	/* UK MHEG Profile (D-BOOK 5.0) :
	   * Scaling : Scaling (the ScaleBitmap action) shall be supported for bitmap objects with MPEG I-frame content.
	               Scaling shall not be supported for Bitmap objects using PNG bitmaps.
	   * Tiling : Support for tiling is only required for PNG bitmaps.
	   * Transparency : Transparency can be encoded within the PNG bitmaps.
	                    Support for object level transparency (i.e. the Transparency internal attribute of the Bitmap class) is not required for any bitmap type.
	   * BitmapDecodeOffset : only required for MPEG I-frame not PNG bitmaps.
	*/

	if (bitmap->pMpegImage != NULL) /* for MHEG_ContentHook_Bitmap_MPEG (I-frame) */
	{
		MHEGIB_PlayIFrame(src_x+dst_x, src_y+dst_y, w, h, bitmap->pMpegImage, bitmap->ulMpegImageSize);
	}
	else /* for MHEG_ContentHook_Bitmap_PNG */
	{
		MHEGIB_DrawBitmap(src_x+dst_x, src_y+dst_y, w, h, bitmap->pImage, tiling);
	}

	return;
}

/*
 * coords should be in the range 0-MHEG_XRES, 0-MHEG_YRES
 */

void
MHEGDisplay_drawCanvas(MHEGDisplay *d, XYPosition *src, OriginalBoxSize *box, MHEGCanvas *canvas, XYPosition *dst)
{
#if 0 // TODO : 삭제 또는 수정 필요.

	int src_x, src_y;
	int dst_x, dst_y;
	unsigned int w, h;

	/*
	 * scale up if fullscreen
	 * the canvas image itself is scaled when it is created
	 */
	src_x = MHEGDisplay_scaleX(d, src->x_position);
	src_y = MHEGDisplay_scaleY(d, src->y_position);
	w = MHEGDisplay_scaleX(d, box->x_length);
	h = MHEGDisplay_scaleY(d, box->y_length);
	dst_x = MHEGDisplay_scaleX(d, dst->x_position);
	dst_y = MHEGDisplay_scaleY(d, dst->y_position);

	XRenderComposite(d->dpy, PictOpOver, canvas->contents_pic, None, d->next_overlay_pic, src_x, src_y, src_x, src_y, dst_x, dst_y, w, h);

#endif

	verbose_osd("%s", __FUNCTION__);

	return;
}

/*
 * coords should be in the range 0-MHEG_XRES, 0-MHEG_YRES
 * text can include multibyte (UTF8) chars as described in the UK MHEG Profile
 * text can also include tab characters (0x09)
 * if tabs is false, tab characters are just treated as spaces
 * text should *not* include ESC sequences to change colour or \r for new lines
 */

void
MHEGDisplay_drawTextElement(MHEGDisplay *d, XYPosition *pos, MHEGFont *font, MHEGTextElement *text, bool tabs, unsigned int hj, unsigned int vj)
{
	int x, y, rcol;

	verbose_osd("%s", __FUNCTION__);

	/* is there any text */
	if(text->size == 0)
		return;

	/* convert to internal colour format */
	display_colour(&rcol, &text->col);

	/* scale the x origin if fullscreen */
	x = MHEGDisplay_scaleX(d, pos->x_position + text->x);

	/* y coord does not change */
	y = MHEGDisplay_scaleY(d, pos->y_position + text->y);
	y = ( y > MHEG_FONT_ADJUST_Y ) ? (y - MHEG_FONT_ADJUST_Y) : 0 ;

	/* draw string */
	if (font->size < 1)
	{
		verbose_osd("%s Too small font size(%d)", __FUNCTION__, font->size);
		return;
	}

	MHEGIB_DrawString(x, y, rcol, font->size, tabs, MHEG_TAB_WIDTH, text->data, text->size, hj, vj);

	return;
}

/*
 * copy the contents of next_overlay onto used_overlay
 * ie all drawing done since the last call to this will appear on the screen at the next refresh()
 */

void
MHEGDisplay_useOverlay(MHEGDisplay *d)
{
#if 0 // hmkim : commented out (현재로선 필요치 않음)
	/* avoid any XRender clip mask */
	XCopyArea(d->dpy, d->next_overlay, d->used_overlay, d->overlay_gc, 0, 0, d->xres, d->yres, 0, 0);
#endif

	return;
}

/*
 * convert the given PNG data to an internal format
 * returns NULL on error
 */

MHEGBitmap *
MHEGDisplay_newPNGBitmap(MHEGDisplay *d, OctetString *png)
{
	MHEGBitmap *b;
	png_uint_32 width, height;
	unsigned char *rgba;
	unsigned int i;

	/* nothing to do */
	if(png == NULL || png->size == 0)
		return NULL;

	/* convert the PNG into a standard format we can use as an XImage */
	if((rgba = readpng_get_image(png->data, png->size, &width, &height)) == NULL)
	{
		error("Unable to decode PNG file");
		return NULL;
	}

	/*
	 * we now have an array of 32-bit RGBA pixels in network byte order
	 * ie if pix is a char *: pix[0] = R, pix[1] = G, pix[2] = B, pix[3] = A
	 * we need to convert it to ffmpeg's PIX_FMT_RGBA32 format
	 * ffmpeg always stores PIX_FMT_RGBA32 as
	 *  (A << 24) | (R << 16) | (G << 8) | B
	 * no matter what byte order our CPU uses. ie,
	 * it is stored as BGRA on little endian CPU architectures and ARGB on big endian CPUs
	 */
	for(i=0; i<width*height; i++)
	{
		uint8_t a, r, g, b;
		uint32_t pix;
		/*
		 * if the pixel is transparent, set the RGB components to 0
		 * otherwise, if we scale up the bitmap in fullscreen mode,
		 * we may end up with a border around the image
		 * this happens, for example, with the BBC's "Press Red" image
		 * it has a transparent box around it, but the RGB values are not 0 in the transparent area
		 * when we scale it up we get a pink border around it
		 */
		a = rgba[(i * 4) + 3];
		if(a == 0)
		{
			pix = 0;
		}
		else
		{
			r = rgba[(i * 4) + 0];
			g = rgba[(i * 4) + 1];
			b = rgba[(i * 4) + 2];

#if defined(WIN32) || defined(CONFIG_PROD_ENDIAN_LITTLE)
			pix = (b << 24) | (g << 16) | (r << 8) | a;
#else
			pix = (a << 24) | (r << 16) | (g << 8) | b;
#endif
		}
		*((uint32_t *) &rgba[i * 4]) = pix;
	}

	/* convert the PIX_FMT_RGBA32 data to a MHEGBitmap */
	//b = MHEGBitmap_fromRGBA(d, rgba, width, height);
	{
		unsigned int npixs;

		b = safe_malloc(sizeof(MHEGBitmap));
		VK_memset(b, 0x00, sizeof(MHEGBitmap));

		npixs = width * height;
#if defined(WIN32) || defined(CONFIG_PROD_ENDIAN_LITTLE)
		width = local_ByteSwap_32(width);
		height = local_ByteSwap_32(height);
#endif
		b->pImage = safe_malloc((4 * npixs) + (sizeof(HUINT32) * 2));
		VK_memset(b->pImage, 0x00, ((4 * npixs) + (sizeof(HUINT32) * 2)));
		VK_memcpy(&b->pImage[0], &width, sizeof(HUINT32));
		VK_memcpy(&b->pImage[4], &height, sizeof(HUINT32));
		VK_memcpy(&b->pImage[8], rgba, 4 * npixs);
	}

	/* clean up */
	readpng_free_image(rgba);

	return b;
}

/*
 * convert the given MPEG I-frame data to an internal format
 * returns NULL on error
 */

MHEGBitmap *
MHEGDisplay_newMPEGBitmap(MHEGDisplay *d, OctetString *mpeg)
{
	MHEGBitmap *b;

#if 1

	/* nothing to do */
	if(mpeg == NULL || mpeg->size == 0)
		return NULL;

	/* just alloc and copy */
	b = safe_mallocz(sizeof(MHEGBitmap));
	b->pMpegImage = safe_malloc(mpeg->size);
	VK_memcpy(b->pMpegImage, mpeg->data, mpeg->size);
	b->ulMpegImageSize = mpeg->size;

#else

	AVCodecContext *codec_ctx;
	AVCodec *codec;
	AVFrame *yuv_frame;
	AVFrame *rgb_frame;
	unsigned char *padded;
	unsigned char *data;
	int used;
	int got_picture;
	unsigned int width;
	unsigned int height;
	int nbytes;
	unsigned char *rgba = NULL;

	/* nothing to do */
	if(mpeg == NULL || mpeg->size == 0)
		return NULL;

	/* use ffmpeg to convert the data into a standard format we can use as an XImage */
	if((codec_ctx = avcodec_alloc_context()) == NULL)
	{
		fatal("Out of memory");
		return NULL;
	}

	if((codec = avcodec_find_decoder(CODEC_ID_MPEG2VIDEO)) == NULL)
	{
		fatal("Unable to initialise MPEG decoder");
		return NULL;
	}

	if(avcodec_open(codec_ctx, codec) < 0)
	{
		fatal("Unable to open video codec");
		return NULL;
	}

	if(((yuv_frame = avcodec_alloc_frame()) == NULL) || ((rgb_frame = avcodec_alloc_frame()) == NULL))
	{
		fatal("Out of memory");
		return NULL;
	}

	/* ffmpeg may read passed the end of the buffer, so pad it out */
	padded = safe_malloc(mpeg->size + FF_INPUT_BUFFER_PADDING_SIZE);
	VK_memcpy(padded, mpeg->data, mpeg->size);
	VK_memset(padded + mpeg->size, 0, FF_INPUT_BUFFER_PADDING_SIZE);

	/* decode the YUV frame */
	data = padded;
	size = mpeg->size;
	do
	{
		used = avcodec_decode_video(codec_ctx, yuv_frame, &got_picture, data, size);
		data += used;
		size -= used;
	}
	while(!got_picture && size > 0);
	/* need to call it one final time with size=0, to actually get the frame */
	if(!got_picture)
		(void) avcodec_decode_video(codec_ctx, yuv_frame, &got_picture, data, size);

	if(!got_picture)
	{
		error("Unable to decode MPEG image");
		b = NULL;
	}
	else
	{
		/* convert to RGBA */
		struct SwsContext *sws_ctx;
		width = codec_ctx->width;
		height = codec_ctx->height;
		if((nbytes = avpicture_get_size(PIX_FMT_RGBA32, width, height)) < 0)
		{
			fatal("Invalid MPEG image");
			b = NULL;
			goto EXIT;
		}
		rgba = safe_malloc(nbytes);
		avpicture_fill((AVPicture *) rgb_frame, rgba, PIX_FMT_RGBA32, width, height);
		//img_convert((AVPicture *) rgb_frame, PIX_FMT_RGBA32, (AVPicture*) yuv_frame, codec_ctx->pix_fmt, width, height);
		if((sws_ctx = sws_getContext(width, height, codec_ctx->pix_fmt,
					     width, height, PIX_FMT_RGBA32,
					     SWS_FAST_BILINEAR, NULL, NULL, NULL)) == NULL)
		{
			fatal("Out of memory");
			b = NULL;
			goto EXIT;
		}
		sws_scale(sws_ctx, yuv_frame->data, yuv_frame->linesize, 0, height, rgb_frame->data, rgb_frame->linesize);
		sws_freeContext(sws_ctx);
		/* convert the PIX_FMT_RGBA32 data to a MHEGBitmap */
		//b = MHEGBitmap_fromRGBA(d, rgba, width, height);
		{
			unsigned int npixs;

			b = safe_malloc(sizeof(MHEGBitmap));
			VK_memset(b, 0x00, sizeof(MHEGBitmap));

			npixs = width * height;
			b->pImage = safe_malloc((4 * npixs) + (sizeof(HUINT32) * 2));
			VK_memset(b->pImage, 0x00, ((4 * npixs) + (sizeof(HUINT32) * 2)));
			VK_memcpy(&b->pImage[0], &width, sizeof(HUINT32));
			VK_memcpy(&b->pImage[4], &height, sizeof(HUINT32));
			VK_memcpy(&b->pImage[8], rgba, 4 * npixs);
		}
	}

EXIT :

	/* clean up */
	safe_free(padded);
	safe_free(rgba);
	av_free(yuv_frame);
	av_free(rgb_frame);
	avcodec_close(codec_ctx);

#endif

	return b;
}

void
MHEGDisplay_freeBitmap(MHEGDisplay *d, MHEGBitmap *b)
{
	if(b != NULL)
	{
		safe_free(b->pImage);

		if (b->pMpegImage != NULL)
		{
			MHEGIB_StopIFrame();
			safe_free(b->pMpegImage);
		}

		safe_free(b);
	}

	return;
}

/*
 * construct a MHEGBitmap from an array of ffmpeg's PIX_FMT_RGBA32 pixels
 * ffmpeg always stores PIX_FMT_RGBA32 as
 *  (A << 24) | (R << 16) | (G << 8) | B
 * no matter what byte order our CPU uses. ie,
 * it is stored as BGRA on little endian CPU architectures and ARGB on big endian CPUs
 */
#if 0
static MHEGBitmap *
MHEGBitmap_fromRGBA(MHEGDisplay *d, unsigned char *rgba, unsigned int width, unsigned int height)
{
	MHEGBitmap *bitmap;
	unsigned char *xdata;
	uint32_t rgba_pix;
	uint32_t xpix;
	uint8_t r, g, b, a;
	unsigned int i, npixs;
	XImage *ximg;
	XRenderPictFormat *pic_format;
	enum PixelFormat av_format;
	GC gc;

	bitmap = safe_mallocz(sizeof(MHEGBitmap));

	/* find a matching XRender pixel format */
	pic_format = XRenderFindStandardFormat(d->dpy, PictStandardARGB32);
	av_format = find_av_pix_fmt(32,
				    pic_format->direct.redMask << pic_format->direct.red,
				    pic_format->direct.greenMask << pic_format->direct.green,
				    pic_format->direct.blueMask << pic_format->direct.blue);

	/* copy the RGBA values into a block we can use as XImage data */
	npixs = width * height;
	/* 4 bytes per pixel */
	xdata = safe_malloc(npixs * 4);
	/* are the pixel layouts exactly the same */
	if(av_format == PIX_FMT_RGBA32)
	{
		VK_memcpy(xdata, rgba, npixs * 4);
	}
	else
	{
		/* swap the RGBA components as needed */
		for(i=0; i<npixs; i++)
		{
			rgba_pix = *((uint32_t *) &rgba[i * 4]);
			a = (rgba_pix >> 24) & 0xff;
			r = (rgba_pix >> 16) & 0xff;
			g = (rgba_pix >> 8) & 0xff;
			b = rgba_pix & 0xff;
			/* is it transparent */
			if(a == 0)
			{
				xpix = 0;
			}
			else
			{
				xpix = a << pic_format->direct.alpha;
				xpix |= r << pic_format->direct.red;
				xpix |= g << pic_format->direct.green;
				xpix |= b << pic_format->direct.blue;
			}
			*((uint32_t *) &xdata[i * 4]) = xpix;
		}
	}

	/* get X to draw the XImage onto a Pixmap */
	if((ximg = XCreateImage(d->dpy, NULL, 32, ZPixmap, 0, (char *) xdata, width, height, 32, 0)) == NULL)
		fatal("XCreateImage failed");
	/* passed NULL Visual to XCreateImage, so set the rgb masks now */
	ximg->red_mask = pic_format->direct.redMask;
	ximg->green_mask = pic_format->direct.greenMask;
	ximg->blue_mask = pic_format->direct.blueMask;
	/* create the Pixmap */
	bitmap->image = XCreatePixmap(d->dpy, d->win, width, height, 32);
	gc = XCreateGC(d->dpy, bitmap->image, 0, NULL);
	XPutImage(d->dpy, bitmap->image, gc, ximg, 0, 0, 0, 0, width, height);
	XFreeGC(d->dpy, gc);

	/* associate a Picture with it */
	bitmap->image_pic = XRenderCreatePicture(d->dpy, bitmap->image, pic_format, 0, NULL);

	/* if we are using fullscreen mode, scale the image */
	if(d->fullscreen)
	{
		/* set up the matrix to scale it */
		XTransform xform;
		verbose_todo("%s; MHEGBitmap_fromRGBA; take aspect ratio into account", __FUNCTION__); /* TODO: use swscale here to permenantly scale up the image */
		/* X */
		xform.matrix[0][0] = (MHEG_XRES << 16) / d->xres;
		xform.matrix[0][1] = 0;
		xform.matrix[0][2] = 0;
		/* Y */
		xform.matrix[1][0] = 0;
		xform.matrix[1][1] = (MHEG_YRES << 16) / d->yres;
		xform.matrix[1][2] = 0;
		/* Z */
		xform.matrix[2][0] = 0;
		xform.matrix[2][1] = 0;
		xform.matrix[2][2] = 1 << 16;
		/* scale it */
		XRenderSetPictureTransform(d->dpy, bitmap->image_pic, &xform);
		/* set a filter to smooth the edges */
		XRenderSetPictureFilter(d->dpy, bitmap->image_pic, FilterBilinear, 0, 0);
	}

	/* we alloc'ed the XImage data, make sure XDestroyImage doesn't try to free it */
	safe_free(xdata);
	ximg->data = NULL;
	XDestroyImage(ximg);

	return bitmap;
}
#endif

/*
 * returns true if the two boxes intersect
 * sets out_pos and out_box to the intersection
 */

bool
intersects(XYPosition *p1, OriginalBoxSize *b1, XYPosition *p2, OriginalBoxSize *b2, XYPosition *out_pos, OriginalBoxSize *out_box)
{
	int x1 = p1->x_position;
	int y1 = p1->y_position;
	int w1 = b1->x_length;
	int h1 = b1->y_length;
	int x2 = p2->x_position;
	int y2 = p2->y_position;
	int w2 = b2->x_length;
	int h2 = b2->y_length;
	bool hmatch;
	bool vmatch;

	/* intersection */
	out_pos->x_position = MAX(x1, x2);
	out_pos->y_position = MAX(y1, y2);
	out_box->x_length = MIN(x1 + w1, x2 + w2) - out_pos->x_position;
	out_box->y_length = MIN(y1 + h1, y2 + h2) - out_pos->y_position;

	/* does it intersect */
	hmatch = (x1 < (x2 + w2)) && ((x1 + w1) > x2);
	vmatch = (y1 < (y2 + h2)) && ((y1 + h1) > y2);

//	VK_Print("intersects: return %s\n", hmatch && vmatch ? "true" : "false");

	return hmatch && vmatch;
}

