/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: splash_bmp.c $
 * $brcm_Revision: 14 $
 * $brcm_Date: 9/28/12 4:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/common/splash_bmp.c $
 * 
 * 14   9/28/12 4:18p mward
 * SW7435-114:  Compiler warning for signed/unsigned compare.
 * 
 * 13   9/19/12 1:11p jessem
 * SW7231-969: Enclosed render_image_4bpp() with B_4BPP_BMP_IMAGE.
 *
 * 12   9/18/12 2:58p jessem
 * SW7231-969: Changed 4BPP_BMP_IMAGE to B_4BPP_BMP_IMAGE.
 *
 * 11   9/18/12 2:00p randyjew
 * SW7231-969: Add 4bpp image support
 *
 * 10   12/8/11 6:46p syang
 * SW7572-623: add support to BPXL_eA8_Y8_Cb8_Cr8 dst pxl format
 *
 * 9   2/24/11 5:09p gmohile
 * SW7408-239 : Add 7408 splash support
 *
 * 8   10/14/10 5:19p jkim
 * SWCFE-386: remove compiler warning.
 *
 * 7   8/26/10 4:41p rpereira
 * SWCFE-368: Splash screen updates
 *
 * 7   8/26/10 4:38p rpereira
 * SWCFE-368: Splash screen updates
 *
 * 6   8/12/10 6:36p mward
 * SW7125-569:  Fixed c++ style comments.
 *
 * 5   5/19/10 1:47p jkim
 * SWCFE-312: modify not to use definitions from bpxl.h
 *
 * 4   5/14/09 5:44p shyam
 * PR52592 : Add support for ARGB8888 surfaces
 *
 * 3   6/28/07 5:06p shyam
 * PR 30741 : syncup with cfe version
 *
 * 2   6/27/07 6:24p shyam
 * PR 30741 : enhancements to bmp support
 *
 ***************************************************************************/

#include "splash_magnum.h"

#include "splash_bmp.h"
#if 0
#include "bpxl.h"

#include <stdio.h>
#endif
/* #include "lib_printf.h" */

BDBG_MODULE(splash_bmp);

#define B_GET_U16(x) (x[1]<<8  | x[0]<<0)
#define B_GET_U32(x) (x[3]<<24 | x[2]<<16 | x[1]<<8 | x[0]<<0)

uint8_t *buffer;
BMP_HEADER_INFO bmp ;
int SrcRow ;
uint8_t *frmbuf;
uint32_t pitch ;
#define  B_4BPP_BMP_IMAGE 0

#if B_4BPP_BMP_IMAGE
#define BMP_BYTES_PER_PIXEL 2
#else
#define BMP_BYTES_PER_PIXEL 3
#endif
uint32_t surBytesPerPixel ;

uint32_t surPitch ;
uint32_t surWidth ;
uint32_t surHeight ;
BPXL_Format surPxl ;

int splash_bmp_getinfo(uint8_t *imgAddress, BMP_HEADER_INFO *bmpinfo)
{
	uint8_t * ptr = imgAddress? imgAddress : buffer ;

	if(ptr[0]!='B' || ptr[1]!='M')
	{
		BDBG_MSG(("Invalid file format byte0 = %c byte1 = %c", ptr[0], ptr[1]));
		return INVALID_FILE ;
	}

	bmpinfo->header.type      = B_GET_U16(ptr); ptr += 2;
	bmpinfo->header.size      = B_GET_U32(ptr); ptr += 4;
	bmpinfo->header.reserved1 = B_GET_U16(ptr); ptr += 2;
	bmpinfo->header.reserved2 = B_GET_U16(ptr); ptr += 2;
	bmpinfo->header.offset    = B_GET_U32(ptr); ptr += 4;

	bmpinfo->info.size        = B_GET_U32(ptr); ptr += 4;
	bmpinfo->info.width       = B_GET_U32(ptr); ptr += 4;
	bmpinfo->info.height      = B_GET_U32(ptr); ptr += 4;
	bmpinfo->info.planes      = B_GET_U16(ptr); ptr += 2;
	bmpinfo->info.bits        = B_GET_U16(ptr); ptr += 2;
	bmpinfo->info.compression = B_GET_U32(ptr); ptr += 4;
	bmpinfo->info.imagesize   = B_GET_U32(ptr); ptr += 4;
	bmpinfo->info.xresolution = B_GET_U32(ptr); ptr += 4;
	bmpinfo->info.yresolution = B_GET_U32(ptr); ptr += 4;
	bmpinfo->info.ncolours    = B_GET_U32(ptr); ptr += 4;
	bmpinfo->info.importantcolours = B_GET_U32(ptr); ptr += 4;

	return 0;
}

#define COMPOSE_ARGB8888(a,r,g,b) (\
	((unsigned long)(a) & 0xFF) << 24 | \
	((unsigned long)(r) & 0xFF) << 16 | \
	((unsigned long)(g) & 0xFF) << 8 | \
	((unsigned long)(b) & 0xFF) \
	)

#define COMPOSE_RGB565(r,g,b) (\
	((unsigned short)(r) & 0xF8) << 8 | \
	((unsigned short)(g) & 0xFC) << 3 | \
	((unsigned short)(b) & 0xF8) >> 3 \
	)

#define COMPOSE_AYCbCr8888(a,y,cb,cr) (\
	((unsigned long)(a) & 0xFF) << 24 | \
	((unsigned long)(y) & 0xFF) << 16 | \
	((unsigned long)(cb) & 0xFF) << 8 | \
	((unsigned long)(cr) & 0xFF) \
	)

static void render_image(int x, int y)
{
	uint8_t *dest_ptr, *src_ptr;
	uint8_t *srcrow_buffer ;
	int r, g, b ;
	uint32_t ulY, ulCr, ulCb, row, src_x, dest_x ;
    for (row=0;row<bmp.info.height;row++)
	{
		/* the bmp buffer starts with the bottom row. */
		srcrow_buffer = buffer + bmp.header.offset +
			(pitch * (bmp.info.height - row - 1));
		dest_ptr = frmbuf + (surPitch * (row+y)) + x * surBytesPerPixel;
		for (src_x = 0; src_x < bmp.info.width; src_x++)
		{
#if B_4BPP_BMP_IMAGE
			src_ptr = srcrow_buffer + (src_x / BMP_BYTES_PER_PIXEL);
#else
			src_ptr = srcrow_buffer + (src_x * BMP_BYTES_PER_PIXEL);
#endif
			dest_x = x + src_x ;
			b = src_ptr[0] ; /* Blue is 0 in BMP files */
			g = src_ptr[1] ; /* Greeb is 1 in BMP files */
			r = src_ptr[2] ; /* Red is 2 in BMP files */
			if (surPxl == BPXL_eR5_G6_B5)
			{
				*(uint16_t *)dest_ptr = COMPOSE_RGB565( r, g, b) ;
			}
			else if (surPxl == BPXL_eA8_R8_G8_B8)
			{
				*(uint32_t *)dest_ptr = COMPOSE_ARGB8888( 0xFF, r, g, b) ;
			}
			else if (surPxl == BPXL_eA8_Y8_Cb8_Cr8)
			{

				/* Convert RGB components to YCbCr */
				/* Y  = R *  0.257 + G *  0.504 + B *  0.098 + 16  */
				/* Cb = R * -0.148 + G * -0.291 + B *  0.439 + 128 */
				/* Cr = R *  0.439 + G * -0.368 + B * -0.071 + 128 */
				ulY  = r *  0x41CA + g *  0x8106 + b *  0x1916 + 0x100000;
				ulCb = r * -0x25E3 + g * -0x4A7F + b *  0x7062 + 0x800000;
				ulCr = r *  0x7062 + g * -0x5E35 + b * -0x122D + 0x800000;
				ulY  = ulY >> 16;
				ulCb = ulCb >> 16;
				ulCr = ulCr >> 16;
				*(uint32_t *)dest_ptr = COMPOSE_AYCbCr8888( 0xFF, ulY, ulCb, ulCr) ;
			}
			dest_ptr += surBytesPerPixel;
		}
	}
}

#if B_4BPP_BMP_IMAGE
static void render_image_4bpp(int x, int y)
{
	int row, dest_x, src_x ;
	uint8_t *dest_ptr, *src_ptr;
	uint8_t *srcrow_buffer ;
	uint8_t *palette;
	int r, g, b ;
	int nn;
	uint32_t ulY, ulCr, ulCb;

#if 0
	int t;
	for(t=0; t<16; t++)
		{
		BDBG_ERR(("offset %d ",sizeof (BMP_HEADER_INFO)));
			BDBG_ERR(("%d, b 0x%x, g 0x%x, r 0x%x, reserve 0x%x ",t,palette[t*4], palette[t*4+1], palette[t*4+2], palette[t*4+3]));
		}
#endif
	for (row=0;row<bmp.info.height;row++)
	{
		/* the bmp buffer starts with the bottom row. */
		srcrow_buffer = buffer + bmp.header.offset + (pitch * (bmp.info.height - row - 1));
		palette = buffer + sizeof (BMP_HEADER_INFO) -2; /* TODO: Set proper palette address */
		BDBG_MSG(("scrow_buffer %p",srcrow_buffer));

		dest_ptr = frmbuf + (surPitch * (row+y)) + x * surBytesPerPixel;
		for (src_x = 0; src_x < bmp.info.width/2; src_x++)
		{
			src_ptr = srcrow_buffer + src_x;
			BDBG_MSG(("row %d src_x %d scr_ptr %p",row, src_x, src_ptr));
			dest_x = x + src_x ;

			for (nn = 0; nn < 2; nn++)
			{
				int shift = (1-nn)*4;
				int index = (src_ptr[0] >> shift) & 0xF;

#if 0
				BDBG_ERR(("src_ptr[0] 0x%x, index %d,shift %d",src_ptr[0],index, shift));
#endif
				b = palette[index*4];
				g = palette[index*4+1];
				r = palette[index*4+2];

				#if 0
				BDBG_ERR(("b %d, g %d,r %d",b,g,r));
				#endif
				if (surPxl == BPXL_eR5_G6_B5)
				{
					*(uint16_t *)dest_ptr = COMPOSE_RGB565( r, g, b) ;
				}
				else if (surPxl == BPXL_eA8_R8_G8_B8)
				{
					*(uint32_t *)dest_ptr = COMPOSE_ARGB8888( 0xFF, r, g, b) ;
				}
				else if (surPxl == BPXL_eA8_Y8_Cb8_Cr8)
				{
					/* Convert RGB components to YCbCr */
					/* Y  = R *  0.257 + G *  0.504 + B *  0.098 + 16  */
					/* Cb = R * -0.148 + G * -0.291 + B *  0.439 + 128 */
					/* Cr = R *  0.439 + G * -0.368 + B * -0.071 + 128 */
					ulY  = r *  0x41CA + g *  0x8106 + b *  0x1916 + 0x100000;
					ulCb = r * -0x25E3 + g * -0x4A7F + b *  0x7062 + 0x800000;
					ulCr = r *  0x7062 + g * -0x5E35 + b * -0x122D + 0x800000;
					ulY  = ulY >> 16;
					ulCb = ulCb >> 16;
					ulCr = ulCr >> 16;
					*(uint32_t *)dest_ptr = COMPOSE_AYCbCr8888( 0xFF, ulY, ulCb, ulCr) ;
				}
				dest_ptr += surBytesPerPixel;
			}
		}
	}
}
#endif

int splash_render_bmp(int x,
	int y,
	uint8_t *imgAddress,
	void* splashAddress)
{
	if(imgAddress) buffer = imgAddress ;

	if(splash_bmp_getinfo(imgAddress, &bmp) == INVALID_FILE)
		return INVALID_FILE ;

	BDBG_MSG(("image %x, size=%d, offset=%d\n",
		bmp.header.type, bmp.header.size, bmp.header.offset));
	BDBG_MSG(("info %d, %dx%d compression %d\n",
		bmp.info.size, bmp.info.width, bmp.info.height, bmp.info.compression));

	if (bmp.info.ncolours) {
		BDBG_ERR(("Palettized bmp's not supported. (ncolors=%d)\n", bmp.info.ncolours));
		return 1;
	}

	if (bmp.info.compression) {
		BDBG_ERR(("compression not supported. (compression=%d)\n", bmp.info.ncolours));
		return 1;
	}
#if B_4BPP_BMP_IMAGE
	pitch = bmp.info.width / 2 ;
#else
	pitch = bmp.info.width * 3;
#endif
	if (pitch % 4)
		pitch += 4 - (pitch % 4);

	frmbuf = splashAddress ;
#if B_4BPP_BMP_IMAGE
	render_image_4bpp(x, y) ;
#else
	render_image(x, y) ;
#endif
	return 0 ;

}

int splash_setparams(BPXL_Format pxl,
	uint32_t splashPitch,
	uint32_t splashWidth,
	uint32_t splashHeight
	)
{
	surPitch = 	splashPitch;
	surWidth = splashWidth ;
	surHeight = splashHeight ;
	surPxl = pxl ;
	switch(pxl)
	{
		case BPXL_eA8_R8_G8_B8 :
	  	case BPXL_eA8_Y8_Cb8_Cr8:
			surBytesPerPixel = 4 ;
			break ;
		case BPXL_eR5_G6_B5 :
			surBytesPerPixel = 2;
			break ;
		default :
			/* printf("Error Unsupported pixel format %08x\n", pxl) ; */
			surBytesPerPixel = 10;
	} ;
#if 0
	printf("splashPitch = %d, splashWidth = %d, splashHeight=%d Pixel Format = %08x \n"
		" bytesperpixel = %d\n",
		surPitch, surWidth, surHeight, pxl, surBytesPerPixel) ;
#endif
	return 0;
}

int splash_fillbuffer( void* splashAddress, int r, int g, int b)
{
	uint32_t i, j;
	uint8_t *pCurr ;
	uint8_t *pLine = splashAddress ;
	/* fill surface */
	for(i=0; i<surHeight; i++)
	{
		pCurr = pLine;
		for(j=0; j<surWidth; j++)
		{
			if(surBytesPerPixel == 2)
				*(uint16_t *)pCurr = COMPOSE_RGB565( r, g, b) ;
			else if(surBytesPerPixel == 4)
				*(uint32_t *)pCurr = COMPOSE_ARGB8888( 0xFF, r, g, b) ;
			pCurr += surBytesPerPixel ;
		}
		pLine += surPitch ;
	}
	return 0;
}

