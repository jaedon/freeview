/***************************************************************************
 *     Copyright (c) 2004-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: jpeg_slideshow.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 6/19/07 11:19a $
 *
 * Module Description:
 *    Reference JPEG Decode into YUYV or RGB Color spaces with slideshow
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/jpeg_slideshow.c $
 * 
 * 5   6/19/07 11:19a mward
 * PR32274: Fix use of nanosleep for -auto.
 * 
 * 4   12/8/06 11:59a mward
 * PR23262: Run on 7118, Picture may still be scaled and positioned wrong.
 * 
 * 3   7/27/06 12:44p rjlewis
 * PR23019: needs this definition in VxWorks.
 * 
 * 2   5/1/06 4:15p jgarrett
 * PR 20236: Updating to call bsurface_create_settings_init().
 * 
 * 1   3/21/06 5:06p jgarrett
 * PR 20255: Adding JPEG slideshow demo
 *
 *************************************************************************/
#include <stdio.h>
#include <assert.h>
#include <sys/times.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "jpeglib.h"
#include "bsettop.h"

#define USE_STDIO	/* Use STDIO or MMAP? */
#define AUTO_TIMEOUT 5 /* seconds */

#ifndef MAP_FAILED
#define MAP_FAILED      ((void *) -1)
#endif

/* Currently supporting little-endian only */
typedef union
{
	struct
	{
		unsigned char b;
		unsigned char g;
		unsigned char r;
		unsigned char a;
	} argb;
	struct
	{
		unsigned char cr;
		unsigned char y1;
		unsigned char cb;
		unsigned char y0;
	} ycbcr;
} graphics_pixel;

typedef struct
{
	unsigned char grayscale;
} grayscale_pixel;

typedef struct
{
	unsigned char y;
	signed char cb;
	signed char cr;
} ycbcr_pixel;

typedef struct
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
} rgb_pixel;

void DecompressGrayscale(j_decompress_ptr cinfo, graphics_pixel *pGraphicsMemory, int pitch, JSAMPARRAY pMemory);
void DecompressYCbCr(j_decompress_ptr cinfo, graphics_pixel *pGraphicsMemory, int pitch, JSAMPARRAY pMemory);
void DecompressRGB(j_decompress_ptr cinfo, graphics_pixel *pGraphicsMemory, int pitch, JSAMPARRAY pMemory);
bsurface_t DecompressJpeg(const char *pszFilename, unsigned int maxWidth, unsigned int maxHeight, bgraphics_t graphics);

#ifndef USE_STDIO
static void b_init_source(j_decompress_ptr cinfo)
{
	/* nothing */
}

static boolean b_fill_input_buffer(j_decompress_ptr cinfo)
{
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
	/* nothing */
}

static void b_jpeg_mem_src(j_decompress_ptr cinfo, void *buffer, int length)
{
	if (cinfo->src == NULL) 
	{	/* first time for this JPEG object? */
		cinfo->src = (struct jpeg_source_mgr *)
		(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
					sizeof(struct jpeg_source_mgr));
	}

	cinfo->src->init_source = b_init_source;
	cinfo->src->fill_input_buffer = b_fill_input_buffer;
	cinfo->src->skip_input_data = b_skip_input_data;
	cinfo->src->resync_to_restart = jpeg_resync_to_restart; /* use default*/
	cinfo->src->term_source = b_term_source;
	cinfo->src->bytes_in_buffer = length;
	cinfo->src->next_input_byte = buffer;
}
#endif /* #ifndef USE_STDIO */

int main(int argc, char **argv)
{
	bgraphics_t graphics;
	bsurface_t framebuffer;
	bsurface_t newSurface=NULL;
	bsurface_t oldSurface=NULL;
	bdisplay_t display;
	bgraphics_settings       graphicsSettings;
	bsurface_create_settings surfaceCreateSettings;
	bdisplay_settings        displaySettings;
	bvideo_format_settings   formatSettings;
	const bsettop_rect destRect = {0, 0, 960, 1080};
	int i, total, first;
	bresult rc;
	struct timeval timeStart, timeEnd;

	/* Init settop API and graphics */
	rc=bsettop_init(BSETTOP_VERSION);
	assert(rc==b_ok);

	display=bdisplay_open(B_ID(0));
	assert(NULL!=display);
	graphics=bgraphics_open(B_ID(0),display);

	bsurface_create_settings_init(&surfaceCreateSettings, graphics);
	surfaceCreateSettings.width = destRect.width;
	surfaceCreateSettings.height = destRect.height;
	surfaceCreateSettings.pixel_format = bgraphics_pixel_format_y08_cb8_y18_cr8;
	framebuffer = bgraphics_create_framebuffer(graphics, &surfaceCreateSettings, true);
	assert(NULL!=framebuffer);

    /* Configure both displays and graphics scalers for the correct output mode */
    /* HD Display */
    bdisplay_get(display, &displaySettings);
#if (BCHP_CHIP==7118) || 1
	displaySettings.format = bvideo_format_ntsc;
#else
    displaySettings.format = bvideo_format_1080i;
#endif
    displaySettings.component = boutput_component_open(B_ID(0));
    displaySettings.dvi = NULL;
    displaySettings.rf = NULL;
    displaySettings.svideo = NULL;
    displaySettings.composite = NULL;
    bdisplay_set(display, &displaySettings);
    bdisplay_get_video_format_settings(&displaySettings, &formatSettings);
    /* HD Graphics */
    bgraphics_get(graphics, &graphicsSettings);
    graphicsSettings.destination_height = formatSettings.height;
    graphicsSettings.destination_width = formatSettings.width;
    graphicsSettings.source_height = surfaceCreateSettings.height;
    graphicsSettings.source_width = surfaceCreateSettings.width;
    graphicsSettings.enabled = true;
    graphicsSettings.async_blits = true;
	graphicsSettings.chromakey_enabled = false;
	graphicsSettings.antiflutter_filter = false;	/* Looks better */
    bgraphics_set(graphics, &graphicsSettings);

	/* Create two pseudo-framebufffers for blending */
	newSurface = bsurface_create(graphics, &surfaceCreateSettings);
	oldSurface = bsurface_create(graphics, &surfaceCreateSettings);

	if ( strcmp(argv[1], "-auto") )
		first=1;
	else
		first=2;	/* Skip first arg, it's the -auto flag */

	for ( i = first, total=0; i < argc; total++ )
	{
		/* Decompress Surface */
		bsurface_settings surfaceSettings;
		bsurface_t surface;

		gettimeofday(&timeStart, NULL);

		surface = DecompressJpeg(argv[i], surfaceCreateSettings.width, surfaceCreateSettings.height, graphics);

		if ( NULL != surface )
		{
			bsettop_rect copyrect;
			bsurface_get(surface, &surfaceSettings);
			if ( (((surfaceSettings.cliprect.width)*540)/surfaceSettings.cliprect.height) > 960 )
			{
				/* Image is too wide, letterbox */
				copyrect.x = 0;
				copyrect.width = destRect.width;
				copyrect.height = 2*((surfaceSettings.cliprect.height * 960)/surfaceSettings.cliprect.width);
				copyrect.y = ((destRect.height - copyrect.height)/2)&~1;
			}
			else
			{
				/* Image is too tall, pillarbox */
				copyrect.y = 0;
				copyrect.height = destRect.height;
				copyrect.width = ((surfaceSettings.cliprect.width * 540) / surfaceSettings.cliprect.height)&~1;
				copyrect.x = ((destRect.width - copyrect.width)/2)&~1;				
			}
			/* Fill with black for letterbox */
			bsurface_fill(newSurface, &destRect, 0xff000000);
			bsurface_copy(newSurface, &copyrect, surface, &surfaceSettings.cliprect);
			bsurface_sync(newSurface);
			bsurface_destroy(surface);

			if ( 0==total )
			{
				/* Full image copy */
				bsurface_copy(framebuffer, &destRect, newSurface, &destRect);
				bgraphics_flip(graphics);
			}
			else
			{
				#define TRANS_FIELDS 45 /* 1/2 second */
				int j, trans;
				trans=total%4;
				if ( first == 1 )
				{
					/* Not auto mode... */
					printf("Press enter to continue\n", argv[i]);
					(void)getchar();
				}
				switch ( trans )
				{
				case 0:
					/* Dissolve */
					for ( j=1; j<=TRANS_FIELDS; j++ )
					{
						unsigned long alpha = ((255*j)/TRANS_FIELDS);
						bsurface_blit(framebuffer, &destRect, BSURFACE_BLEND_WITH_PIXEL1|BSURFACE_SET_DEST_ALPHA_WITH_SRC1_ALPHA,
							newSurface, &destRect, oldSurface, &destRect, alpha, alpha);
						bgraphics_flip(graphics);
					}
					break;
				case 1:
					/* Wipe from right */
					for ( j=1; j<=TRANS_FIELDS; j++ )
					{
						bsettop_rect rect=destRect;
						rect.width = ((destRect.width*(TRANS_FIELDS-j))/TRANS_FIELDS)&~1;
						if (rect.width > 0 )
							bsurface_copy(framebuffer, &rect, oldSurface, &rect);
						rect.x = rect.width;
						rect.width = destRect.width - rect.x;
						if ( rect.width > 0 )
							bsurface_copy(framebuffer, &rect, newSurface, &rect);
						bgraphics_flip(graphics);
					}
					break;
				case 3:
					/* Split shutter horizontal reveal effect */
					for ( j=1; j<=TRANS_FIELDS; j++ )
					{
						int k;
						/* We're going to work in 24 steps, (1080/24)=45) */
						bsettop_rect rect=destRect;
						bsettop_rect srcrect=destRect;
						bsurface_copy(framebuffer, &destRect, newSurface, &destRect);
						for ( k=0; k<24; k+=2 )
						{
							/* Even pass, move to right side */
							bsettop_rect rect=destRect;
							bsettop_rect srcrect=destRect;
							srcrect.width = rect.width = ((destRect.width*(TRANS_FIELDS-j))/TRANS_FIELDS)&~1;
							rect.x = destRect.width - rect.width;
							srcrect.height = rect.height = destRect.height/24;
							srcrect.y = rect.y = (destRect.height*k)/24;
							if ( srcrect.width > 0 )
								bsurface_copy(framebuffer, &rect, oldSurface, &srcrect);
						}
						for ( k=1; k<24; k+=2 )
						{
							/* Odd pass, move to left side */
							bsettop_rect rect=destRect;
							bsettop_rect srcrect=destRect;
							srcrect.width = rect.width = ((destRect.width*(TRANS_FIELDS-j))/TRANS_FIELDS)&~1;
							srcrect.x = destRect.width - rect.width;
							srcrect.height = rect.height = destRect.height/24;
							srcrect.y = rect.y = (destRect.height*k)/24;
							if ( srcrect.width > 0 )
								bsurface_copy(framebuffer, &rect, oldSurface, &srcrect);
						}
						bgraphics_flip(graphics);
					}
					break;
				case 2:
					/* Box wipe */
					bsurface_copy(framebuffer, &destRect, oldSurface, &destRect);
					for ( j=1; j<=TRANS_FIELDS; j++ )
					{
						bsettop_rect rect;
						rect.width = ((destRect.width*j)/TRANS_FIELDS)&~1;
						rect.x = ((destRect.width - rect.width)/2)&~1;
						rect.height = ((destRect.height*j)/TRANS_FIELDS)&~1;
						rect.y = ((destRect.height - rect.height)/2)&~1;
						bsurface_copy(framebuffer, &rect, newSurface, &rect);
						bgraphics_flip(graphics);
					}
					break;
				default:
					break;
				}
			}
		}

		/* Loop after last image */
		if ( ++i >= argc )
			i=first;

		/* Rotate two surfaces */
		bsurface_copy(oldSurface, &destRect, newSurface, &destRect);

		/* Wait... */
		if ( first > 1 )
		{
			struct timespec sleepTime;
			gettimeofday(&timeEnd, NULL);

			sleepTime.tv_sec = 4 - (timeEnd.tv_sec - timeStart.tv_sec);
			sleepTime.tv_nsec = 1000000000 - (1000 *(timeEnd.tv_usec - timeStart.tv_usec));
			if ( sleepTime.tv_nsec > 1000000000 )
			{
				sleepTime.tv_sec++;
				sleepTime.tv_nsec -= 1000000000;
			}
			nanosleep(&sleepTime, NULL);
		}
	}

	return 0;
}

void DecompressGrayscale(j_decompress_ptr cinfo, graphics_pixel *pGraphicsMemory, int pitch, JSAMPARRAY pMemory)
{	
	JDIMENSION lines_read, total_lines_read;
	int pixel_offset, rounded_width;
	unsigned char *pBaseGraphicsMemory = (unsigned char *)pGraphicsMemory;

	rounded_width = cinfo->output_width & ~0x1L;
	total_lines_read = 0;
	while ( total_lines_read < cinfo->output_height )
	{
		grayscale_pixel *pGrayscalePixels = (grayscale_pixel *)pMemory[0];

		lines_read = jpeg_read_scanlines(cinfo, pMemory, cinfo->rec_outbuf_height);
		total_lines_read += lines_read;

		while ( lines_read-- > 0 )
		{
			pGraphicsMemory = (graphics_pixel *)pBaseGraphicsMemory;
			/* Now we have cinfo.output_width * lines_read of data to copy to the bitmap */
			for ( pixel_offset=0; pixel_offset < rounded_width; pixel_offset+=2 )
			{
				/* Grayscale -> YCbCr -- Y = grayscale, CbCr = 0x80 (unity) */
				pGraphicsMemory->ycbcr.y0 = pGrayscalePixels[pixel_offset].grayscale;
				pGraphicsMemory->ycbcr.y1 = pGrayscalePixels[pixel_offset+1].grayscale;
				pGraphicsMemory->ycbcr.cr = pGraphicsMemory->ycbcr.cb = 0x80;
				pGraphicsMemory++;
			}
			/* Skip to next scanline */
			pGrayscalePixels += cinfo->output_width;
			pBaseGraphicsMemory += pitch;
		}
	}
}

void DecompressYCbCr(j_decompress_ptr cinfo, graphics_pixel *pGraphicsMemory, int pitch, JSAMPARRAY pMemory)
{
	JDIMENSION lines_read, total_lines_read;
	int pixel_offset, rounded_width;
	JDIMENSION output_width;
	JDIMENSION output_height;
	int rec_outbuf_height;
	ycbcr_pixel *pBase = (ycbcr_pixel *)pMemory[0];
	unsigned char *pBaseGraphicsMemory = (unsigned char *)pGraphicsMemory;

	output_height = cinfo->output_height;
	output_width = cinfo->output_width;
	rec_outbuf_height = cinfo->rec_outbuf_height;

	rounded_width = output_width & ~0x1L;
	total_lines_read = 0;
	while ( total_lines_read < output_height )
	{
		ycbcr_pixel *pYCbCrPixels = pBase;

		lines_read = jpeg_read_scanlines(cinfo, pMemory, rec_outbuf_height);
		total_lines_read += lines_read;

		while ( lines_read-- > 0 )
		{
			pGraphicsMemory = (graphics_pixel *)pBaseGraphicsMemory;
			/* Now we have cinfo.output_width * lines_read of data to copy to the bitmap */
			for ( pixel_offset=0; pixel_offset < rounded_width; pixel_offset+=2 )
			{
				int chroma;
				pGraphicsMemory->ycbcr.y0 = pYCbCrPixels[pixel_offset].y;
				pGraphicsMemory->ycbcr.y1 = pYCbCrPixels[pixel_offset+1].y;
				/* Could average for slightly better quality but most are 4:2:2 -> 4:4:4 anyway */
				chroma = pYCbCrPixels[pixel_offset].cr;
				pGraphicsMemory->ycbcr.cr = chroma;
				chroma = pYCbCrPixels[pixel_offset].cb;
				pGraphicsMemory->ycbcr.cb = chroma;
				pGraphicsMemory++;
			}
			/* Skip to next scanline */
			pYCbCrPixels += output_width;
			pBaseGraphicsMemory += pitch;
		}
	}
}

void DecompressRGB(j_decompress_ptr cinfo, graphics_pixel *pGraphicsMemory, int pitch, JSAMPARRAY pMemory)
{
	JDIMENSION lines_read, total_lines_read;
	JDIMENSION pixel_offset;
	unsigned char *pBaseGraphicsMemory = (unsigned char *)pGraphicsMemory;

	total_lines_read = 0;
	while ( total_lines_read < cinfo->output_height )
	{
		rgb_pixel *pRGBPixels = (rgb_pixel *)pMemory[0];

		lines_read = jpeg_read_scanlines(cinfo, pMemory, cinfo->rec_outbuf_height);
		total_lines_read += lines_read;

		while ( lines_read-- > 0 )
		{
			pGraphicsMemory = (graphics_pixel *)pBaseGraphicsMemory;
			/* Now we have cinfo.output_width * lines_read of data to copy to the bitmap */
			for ( pixel_offset=0; pixel_offset < cinfo->output_width; pixel_offset++ )
			{
				/* RGB -> ARGB */
				pGraphicsMemory->argb.a = 0xff;
				pGraphicsMemory->argb.r = pRGBPixels[pixel_offset].r;
				pGraphicsMemory->argb.g = pRGBPixels[pixel_offset].g;
				pGraphicsMemory->argb.b = pRGBPixels[pixel_offset].b;
				pGraphicsMemory++;
			}
			/* Skip to next scanline */
			pRGBPixels += cinfo->output_width;
			pBaseGraphicsMemory += pitch;
		}
	}
}

bsurface_t DecompressJpeg(const char *pszFilename, unsigned int maxWidth, unsigned int maxHeight, bgraphics_t graphics)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	graphics_pixel *pGraphicsMemory;
	JSAMPROW pMemory;
	JSAMPROW pSampArray[8];
	int i;
	struct timeval timeStart, timeEnd;
	bsurface_create_settings surfaceCreateSettings;
	bsurface_memory surfaceMemory;
	bsurface_t returnSurface=NULL;
	FILE *pFile=NULL;
	int fd=-1, rc;
	void *pBuffer=MAP_FAILED;
	struct stat st;
	unsigned int maxScale=8;

	/* Initialize surface settings */
	bsurface_create_settings_init(&surfaceCreateSettings, graphics);

	/* Initialize decoder */
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	/* Open source file */
	#ifdef USE_STDIO
		/* STDIO -- Standard, but slower */
	pFile = fopen(pszFilename, "rb");
	if ( NULL == pFile )
	{
		fprintf(stderr, "Cannot open file '%s'\n", pszFilename);
		goto err;
	}
	jpeg_stdio_src(&cinfo, pFile);

	#else

		/* MMAP -- Linux specific, but faster */
	fd = open(pszFilename, O_RDONLY);
	if ( fd < 0 )
	{
		fprintf(stderr, "Cannot open file '%s'\n", pszFilename);
		goto err;
	}
	rc = fstat(fd, &st);
	if ( rc < 0 )
	{
		close(fd);
		return NULL;
	}
	pBuffer=mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if ( pMemory == MAP_FAILED )
	{
		goto err;
	}

	b_jpeg_mem_src(&cinfo, pBuffer, st.st_size);
	#endif

	jpeg_read_header(&cinfo, TRUE);

	/* Optimize for speed */
	cinfo.dct_method = JDCT_IFAST;
	cinfo.do_fancy_upsampling = FALSE;
	cinfo.do_block_smoothing = FALSE;

	/* Check for supported color spaces */
	switch ( cinfo.jpeg_color_space )
	{
	case JCS_YCbCr:
		cinfo.out_color_space = JCS_YCbCr;
		surfaceCreateSettings.pixel_format = bgraphics_pixel_format_y08_cb8_y18_cr8;				
		{
			char *pszFormat;
			if ( cinfo.comp_info[0].h_samp_factor > cinfo.comp_info[1].h_samp_factor )
			{
				if ( cinfo.comp_info[0].v_samp_factor > cinfo.comp_info[1].v_samp_factor )
				{
					pszFormat = "4:2:0 [CANNOT DOWNSCALE]";
					maxScale=1;
				}
				else
					pszFormat = "4:2:2";
			}
			else
				pszFormat = "4:4:4";

			printf("%s: File color space is YCbCr (%s)\n", pszFilename, pszFormat);
		}
		break;
	case JCS_GRAYSCALE:
		cinfo.out_color_space = JCS_GRAYSCALE;
		surfaceCreateSettings.pixel_format = bgraphics_pixel_format_y08_cb8_y18_cr8;
		printf("%s: File color space is Grayscale\n", pszFilename);
		break;
	case JCS_RGB:
		cinfo.out_color_space = JCS_RGB;
		surfaceCreateSettings.pixel_format = bgraphics_pixel_format_a8_r8_g8_b8;
		printf("%s: File color space is RGB\n", pszFilename);
		break;
	default:
		fprintf(stderr, "%s: Unsupported color space in file.  Currently supporting YCbCr, Grayscale, and RGB only.\n", pszFilename);
		goto err;
	}

	/* Determine output size */
	jpeg_calc_output_dimensions(&cinfo);
	printf("%s: Original image resolution is %dx%d\n", pszFilename, cinfo.image_width, cinfo.image_height);
	/* Downscale until less than output resolution. */
	while ( cinfo.scale_denom < maxScale && 
			(cinfo.output_width > maxWidth || cinfo.output_height > maxHeight) )
	{
		cinfo.scale_denom<<=1;
		jpeg_calc_output_dimensions(&cinfo);
	}
	printf("%s: Target image resolution is %dx%d (scaling=1/%d)\n", pszFilename, cinfo.output_width, cinfo.output_height, cinfo.scale_denom);

	/* Now, we have the official output size.  Create a surface to match */
	surfaceCreateSettings.height = cinfo.output_height;
	surfaceCreateSettings.width = (cinfo.output_width+1)&~1;

	/* Calculate graphics memory requirements */
	returnSurface = bsurface_create(graphics, &surfaceCreateSettings);
	if ( NULL == returnSurface )
	{
		fprintf(stderr, "%s: Unable to create graphics surface (%dx%d)\n", pszFilename, cinfo.output_width, cinfo.output_height);
		goto err;
	}
	else
	{
		(void)bsurface_get_memory(returnSurface, &surfaceMemory);
		pGraphicsMemory = (graphics_pixel *)surfaceMemory.buffer;
	}

	/* Fire up decoder & start timing */
	gettimeofday(&timeStart, NULL);
	jpeg_start_decompress(&cinfo);

	/* Intermediate buffer for decompression */
	pMemory = malloc(cinfo.output_width * cinfo.output_components * cinfo.rec_outbuf_height);
	if ( NULL == pMemory )
	{
		fprintf(stderr, "%s: Unable to allocate line buffer memory (rec_outbuf_height=%d)\n", pszFilename, cinfo.rec_outbuf_height);
		goto err;
	}

	/* Setup sample array */
	assert(cinfo.rec_outbuf_height <= 8);
	for ( i = 0; i < cinfo.rec_outbuf_height; i++ )
		pSampArray[i] = pMemory + (cinfo.output_width * i);

	switch ( cinfo.out_color_space )
	{
	case JCS_YCbCr:
		DecompressYCbCr(&cinfo, pGraphicsMemory, surfaceMemory.pitch, pSampArray);
		break;
	case JCS_GRAYSCALE:
		DecompressGrayscale(&cinfo, pGraphicsMemory, surfaceMemory.pitch, pSampArray);
		break;
	case JCS_RGB:
		DecompressRGB(&cinfo, pGraphicsMemory, surfaceMemory.pitch, pSampArray);
		break;
	default:
		fprintf(stderr, "%s: Unsupported color space in file.  Currently supporting YCbCr, Grayscale, and RGB only.\n", pszFilename);
		goto err;
	}

	jpeg_finish_decompress(&cinfo);

	gettimeofday(&timeEnd, NULL);

	timeEnd.tv_usec -= timeStart.tv_usec;
	if ( timeEnd.tv_usec < 0 )
	{
		timeEnd.tv_usec += 1000000;
		timeEnd.tv_sec--;
	}
	timeEnd.tv_sec -= timeStart.tv_sec;
	printf("%s: Elapsed time to decode was %d.%02.2d seconds\n", pszFilename, timeEnd.tv_sec, timeEnd.tv_usec/10000);	

	jpeg_destroy_decompress(&cinfo);
	if ( pFile )
		fclose(pFile);
	if ( fd >= 0 )
		close(fd);
	if ( pBuffer != MAP_FAILED )
		munmap(pBuffer, st.st_size);

	return returnSurface;

err:
	
	jpeg_destroy_decompress(&cinfo);
	if ( pFile )
		fclose(pFile);
	if ( fd >= 0 )
		close(fd);
	if ( pBuffer != MAP_FAILED )
		munmap(pBuffer, st.st_size);
	if ( returnSurface )
		bsurface_destroy(returnSurface);

	return NULL;
}

