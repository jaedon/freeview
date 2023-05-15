/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: pxl_gen.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 12/20/11 3:43p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/pxl_gen.c $
 * 
 * Hydra_Software_Devel/5   12/20/11 3:43p yuxiaz
 * SW7425-967: Fixed compile warning.
 * 
 * Hydra_Software_Devel/4   12/16/11 4:09p yuxiaz
 * SW7425-967: Added support to use bitmap image for static buffer test.
 * 
 * Hydra_Software_Devel/3   11/15/10 7:45p hongtaoz
 * SW7422-13, SW7425-32: added circle-diagonal-cross-box pattern to static
 * buffer; this will help with BVN bringup test;
 * 
 * Hydra_Software_Devel/2   10/11/10 1:11p jessem
 * SW7420-173: Added support for VFD as source feature.
 * 
 * Hydra_Software_Devel/SW7420-173/3   10/4/10 3:36p jessem
 * SW7420-173: Added HD colors to TEST_singlecolor_YCbCr_pixel().
 *
 * Hydra_Software_Devel/SW7420-173/2   10/1/10 4:15p jessem
 * SW7420-173: Added TEST_singlecolor_YCbCr_pixel function.
 *
 * Hydra_Software_Devel/SW7420-173/1   5/25/10 2:55p jessem
 * SW7420-173: Added support for using VFD as  a  source.
 *
 * Hydra_Software_Devel/1   9/17/07 4:37p rpan
 * PR34857: Initial revision. Ported from xvd_vdc_test (09/17/2007).
 * 
 *
 ***************************************************************************/

#include <math.h>           /* for ceil */
#include <stdlib.h>         /* for abs */
#include <stdio.h>          /* printf */

/* base modules */
#include "bstd.h"           /* standard types */
#include "berr.h"           /* error code */
#include "bkni.h"           /* kernel interface */
#include "bdbg.h"           /* debug interface */
#include "bpxl.h"

BDBG_MODULE(static_buffer);


/*
 *  int colorbar_YCbCr_pixel ()
 *  Generate colorbar YCbCr pixel
 */
void TEST_colorbar_YCbCr_pixel (
	uint32_t  xi,
	uint32_t  x_size,
	bool      bHD,
	uint32_t  uiFrameIndex,
	uint8_t  *y,
	uint8_t  *cb,
	uint8_t  *cr
	)
{
	uint32_t region;

	BDBG_ENTER(colorbar_YCbCr_pixel);
	if(x_size == 0)
	{
		BDBG_ERR(("The ColorBar x_size is 0!"));
		return;
	}

	/* each frame will be left-shifted with the index number of regions */
	region = ( (xi<<3) / x_size + uiFrameIndex ) % 8;

	switch (region)
	{
	case 7:	/* black */
		*y = 16; *cb = 128; *cr = 128;
		break;
	case 6:	/* blue */
		if( bHD )
		{ *y = 28; *cb = 212; *cr = 120; }
		else
		{ *y = 35; *cb = 212; *cr = 114; }
		break;
	case 5:	/* red */
		if( bHD )
		{ *y = 51; *cb = 109; *cr = 212; }
		else
		{ *y = 65; *cb = 100; *cr = 212; }
		break;
	case 4:	/* magenta */
		if( bHD )
		{ *y = 63; *cb = 193; *cr = 204; }
		else
		{ *y = 84; *cb = 184; *cr = 198; }
		break;
	case 3:	/* green */
		if( bHD )
		{ *y = 133; *cb = 63; *cr = 52; }
		else
		{ *y = 112; *cb = 72; *cr = 58; }
		break;
	case 2:	/* cyan */
		if( bHD )
		{ *y = 145; *cb = 147; *cr = 44; }
		else
		{ *y = 131; *cb = 156; *cr = 44; }
		break;
	case 1:	/* yellow */
		if( bHD )
		{ *y = 168; *cb = 44; *cr = 136; }
		else
		{ *y = 162; *cb = 44; *cr = 142; }
		break;
	case 0:	/* white */
		*y = 180; *cb = 128; *cr = 128;
		break;
	default:/* impossible to be here */
		BDBG_ERR(("The ColorBar Region %d is invalid!", region));
	}
	BDBG_LEAVE(colorbar_YCbCr_pixel);
}


/*
 *  int colorbar_YCbCr_pixel ()
 *  Generate colorbar YCbCr pixel
 */
void TEST_singlecolor_YCbCr_pixel
(	uint32_t  color,
	bool      bHD,
	uint8_t  *y,
	uint8_t  *cb,
	uint8_t  *cr )
{
	BDBG_ENTER(single_colorbar_YCbCr_pixel);

	switch (color)
	{
	case 7:	/* black */
		*y = 16; *cb = 128; *cr = 128;
		break;
	case 6:	/* blue */
		if( bHD )
		{ *y = 28; *cb = 212; *cr = 120; }
		else
		{ *y = 35; *cb = 212; *cr = 114; }
		break;
	case 5:	/* red */
		if( bHD )
		{ *y = 51; *cb = 109; *cr = 212; }
		else
		{ *y = 65; *cb = 100; *cr = 212; }
		break;
	case 4:	/* magenta */
		if( bHD )
		{ *y = 63; *cb = 193; *cr = 204; }
		else
		{ *y = 84; *cb = 184; *cr = 198; }
		break;
	case 3:	/* green */
		if( bHD )
		{ *y = 133; *cb = 63; *cr = 52; }
		else
		{ *y = 112; *cb = 72; *cr = 58; }
		break;
	case 2:	/* cyan */
		if( bHD )
		{ *y = 145; *cb = 147; *cr = 44; }
		else
		{ *y = 131; *cb = 156; *cr = 44; }
		break;
	case 1:	/* yellow */
		if( bHD )
		{ *y = 168; *cb = 44; *cr = 136; }
		else
		{ *y = 162; *cb = 44; *cr = 142; }
		break;
	case 0:	/* white */
		*y = 180; *cb = 128; *cr = 128;
		break;
	default:/* impossible to be here */
		BDBG_ERR(("The color %d is invalid!", color));
	}
	BDBG_LEAVE(colorbar_YCbCr_pixel);
}


/*
 *  int checkboard_YCbCr_pixel ()
 *  Generate check board YCbCr pixel
 */
void TEST_checkboard_YCbCr_pixel (
	uint32_t  xi, 
	uint32_t  x_size, 
	uint32_t  yi,
	uint32_t  y_size,
	uint8_t  *y,
	uint8_t  *cb,
	uint8_t  *cr
	)
{
	uint32_t x_region = (xi * 30) / x_size;
	uint32_t y_region = (yi * 30) / y_size;

	if((x_region + y_region) % 2)
	{/* black */
		*y = 16 ; *cb = 128; *cr = 128;
	} else
	{/* white */
		*y = 180; *cb = 128; *cr = 128;
	}
}

/*
 *  int crosshatch_YCbCr_pixel ()
 *  Generate grid/dot YCbCr pixel
 */
void TEST_crosshatch_YCbCr_pixel (
	uint32_t  xi, 
	uint32_t  yi,
	uint8_t  *y,
	uint8_t  *cb,
	uint8_t  *cr
	)
{

	if((xi % 48 > 1) && (yi % 32 > 1) && 
		!((((xi / 24) & 1) && (xi % 24 <= 1)) && (((yi/16) & 1) && (yi%16 <= 1))) )
	{/* black */
		*y = 16 ; *cb = 128; *cr = 128;
	} else
	{/* white */
		*y = 180; *cb = 128; *cr = 128;
	}
}

/*
 *  int lumaramp_YCbCr_pixel ()
 *  Generate luma ramp in H or V direction YCbCr pixel
 */
extern uint8_t  g_minY, g_maxY;

void TEST_lumaramp_YCbCr_pixel( 
	uint32_t  xi, 
	uint32_t  x_size, 
	uint32_t  yi,
	uint32_t  y_size,
	uint8_t  *y,
	uint8_t  *cb,
	uint8_t  *cr,
	bool      bHorz )
{
	uint32_t value;

	/* horizontal ramp */
	if(bHorz)
	{
		value = (uint16_t)((g_maxY - g_minY) * xi / (x_size - 1) + g_minY);
 
		if(value >= g_maxY)
		{
			value = g_maxY;
		}
		else if(value == g_minY)
		{ 
			value = g_minY; 
		}
 
        *y = (uint8_t) value;
		*cb = 0x80; *cr = 0x80;
	}
	else
	{
		value = (uint16_t)((g_maxY - g_minY) * yi / (y_size - 1) + g_minY);
 
		if(value >= g_maxY)
		{
			value = g_maxY;
		}

        *y = (uint8_t) value;
		*cb = 0x80; *cr = 0x80;
	}
}


/*
 *  int cdc_YCbCr_pixel ()
 *  Generate circle-diagonal-cross YCbCr pixel
 */
void TEST_cdc_YCbCr_pixel (
	int32_t  xi, 
	int32_t  yi,
	uint32_t  w,
	uint32_t  h,
	uint8_t  *y,
	uint8_t  *cb,
	uint8_t  *cr
	)
{
	int32_t value = (1000 * (abs(xi - w/2) * abs(xi - w/2)) / (w*w/4) + 
			1000 * (abs(yi - h/2) * abs(yi - h/2)) / (h*h/4));
    
	/* circle */
	if( value <= 1000 && value >= 995)
	{	/* red */
		*y = 51; *cb = 109; *cr = 212;
	} 
    else
	{
		/* diagonal */
		if(abs(abs(h*(xi - w/2)) - abs(w * (yi - h/2))) <= 1000)
		{
			/* green */
			*y = 112; *cb = 72; *cr = 58;
		}
		else
		{
			/* cross */
			if((unsigned)xi == w/2 || (unsigned)yi == h/2)
			{
				/* blue */
				*y = 28; *cb = 212; *cr = 120;
			}
			else
			{
				/* black */
				*y = 16; *cb = 128; *cr = 128;
			}
		}
	}

	/* box in middle and perimeter */
	if(((abs(xi - w/2) == w/4) && abs(yi - h/2) <= h/4) || 
	   ((abs(yi - h/2) == h/4) && abs(xi - w/2) <= w/4) || 
	   (abs(xi - w/2) == w/2)   || (abs(yi - h/2) == h/2))
	{
		/* white */
		*y = 180; *cb = 128; *cr = 128;
	}
}


/*
 *  Generate YCbCr pixel from 24-bit bitmap file
 */
void TEST_Bitmap_YCbCr_pixel (
	FILE    *fp,
	int32_t  xi, 
	int32_t  yi,
	uint32_t  w,
	uint32_t  h,
	uint8_t  *y,
	uint8_t  *cb,
	uint8_t  *cr
	)
{
	uint8_t    buf[4096];
	uint8_t   *pucSrc;
	uint32_t   ulRgb, ulYCrCb;

	BSTD_UNUSED(yi);
	BSTD_UNUSED(h);

	fread( buf, w * 3, 1, fp );
	pucSrc = (uint8_t *) buf + xi * 3;

	ulRgb = BPXL_MAKE_PIXEL(BPXL_eA8_R8_G8_B8,
		0x00, pucSrc[2], pucSrc[1], pucSrc[0]);
	BPXL_ConvertPixel_RGBtoYCbCr(BPXL_eA8_Y8_Cb8_Cr8, BPXL_eA8_R8_G8_B8,
		ulRgb, &ulYCrCb);

	*y  = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, ulYCrCb, 2);
	*cr = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, ulYCrCb, 0);
	*cb = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, ulYCrCb, 1);

}

/* end of file */
