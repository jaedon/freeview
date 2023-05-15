/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: static_buffer.c $
 * $brcm_Revision: Hydra_Software_Devel/52 $
 * $brcm_Date: 10/2/12 6:36p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/static_buffer.c $
 * 
 * Hydra_Software_Devel/52   10/2/12 6:36p vanessah
 * SW7425-2501: cleanup static buffer chip dependency
 * 
 * Hydra_Software_Devel/51   9/25/12 5:36p vanessah
 * SW7425-2501: vdc_test clean up
 * 
 * Hydra_Software_Devel/50   8/16/12 5:47p vanessah
 * SW7425-2501: clean up unnecessary include file
 * 
 * Hydra_Software_Devel/49   7/9/12 5:20p vanessah
 * SW7425-2501: removal 3548/3556 support
 * 
 * Hydra_Software_Devel/48   6/5/12 11:09a pntruong
 * SW7435-9: Default aspect ratio to be the same as bfmt format's default.
 *
 * Hydra_Software_Devel/47   2/17/12 11:27a vanessah
 * SW7425-2327: add Bar data handling test
 *
 * Hydra_Software_Devel/46   1/19/12 3:27p tdo
 * SW7435-9: Add static buffer support for 7435
 *
 * Hydra_Software_Devel/45   12/16/11 4:09p yuxiaz
 * SW7425-967: Added support to use bitmap image for static buffer test.
 *
 * Hydra_Software_Devel/44   12/9/11 10:30a yuxiaz
 * SW7425-967: Added static buffer test for MRE.
 *
 * Hydra_Software_Devel/43   11/30/11 11:28a pntruong
 * SW7429-16: Corrected the stripewidth for 7429.
 *
 * Hydra_Software_Devel/42   11/29/11 11:43a vanessah
 * SW7425-1783: add unittest for stg picture id call back
 *
 * Hydra_Software_Devel/41   8/1/11 2:19p yuxiaz
 * SW7325-863: Fixed build issue with vdc_test.
 *
 * Hydra_Software_Devel/40   1/26/11 3:17p yuxiaz
 * SW7400-2998, SW7425-32: Only reduce cdb/itb buffer size for mosaic
 * mode.
 *
 * Hydra_Software_Devel/39   12/14/10 12:34p darnstein
 * SW7231-2: handle 7346 chipset in same way as 7422.
 *
 * Hydra_Software_Devel/38   12/8/10 4:31p yuxiaz
 * SW7422-77: Added half resolution 3d format in static buffer test.
 *
 * Hydra_Software_Devel/37   11/15/10 7:45p hongtaoz
 * SW7422-13, SW7425-32: added circle-diagonal-cross-box pattern to static
 * buffer; this will help with BVN bringup test;
 *
 * Hydra_Software_Devel/36   11/15/10 5:06p hongtaoz
 * SW7422-33, SW7422-77: newer chip static buffer can be directly loaded
 * by CPU instead of DMA; assign the static buffer heap to MFD instance
 * according to the decoder instance heap assignment; release static
 * buffer including 3D buffers properly at close;
 *
 * Hydra_Software_Devel/35   11/11/10 7:53p hongtaoz
 * SW7425-32: fixed interrupt polarity of static buffer;
 *
 * Hydra_Software_Devel/34   11/11/10 7:44p hongtaoz
 * SW7425-32: added mosaic mode test for static buffer; added progressive
 * static buffer test;
 *
 * Hydra_Software_Devel/33   11/5/10 2:45p yuxiaz
 * SW7422-33, SW7422-77: Added 3D support in static buffer test.
 *
 * Hydra_Software_Devel/32   11/3/10 3:43p hongtaoz
 * SW7425-32: added static buffer support for 7425;
 *
 * Hydra_Software_Devel/31   11/3/10 11:26a yuxiaz
 * SW7422-33: Added format selection for static buffer.
 *
 * Hydra_Software_Devel/30   10/15/10 3:25p hongtaoz
 * SW7425-1, SW7425-10: added MEMC1 support to emulation framwork;
 *
 * Hydra_Software_Devel/29   10/8/10 4:49p hongtaoz
 * SW7425-32: terminate frame interrupt callback properly for static
 * buffer test;
 *
 * Hydra_Software_Devel/28   12/15/09 5:49p yuxiaz
 * SW7408-13: Get vdc_test compile for 7408. Clean up compile flags.
 *
 * Hydra_Software_Devel/27   12/14/09 1:39p rpan
 * SW7468-22: Fixed DDR stripe width setting for 7468.
 *
 * Hydra_Software_Devel/26   11/23/09 4:00p rpan
 * SW7468-22: Initial work for 7468.
 *
 * Hydra_Software_Devel/25   10/14/09 4:49p tdo
 * SW7420-376: Bringup appframework for 7410
 *
 * Hydra_Software_Devel/24   9/10/09 4:34p yuxiaz
 * SW3548-2455: Fixed the wrong color.
 *
 * Hydra_Software_Devel/23   9/10/09 1:33p yuxiaz
 * SW3548-2455: Colorbar should be rotating when allocating more than 1
 * frame.
 *
 * Hydra_Software_Devel/22   9/8/09 4:10p yuxiaz
 * SW3548-2455: Fixed pixel swap in static buffer test
 *
 * Hydra_Software_Devel/21   8/31/09 5:57p albertl
 * SW7125-13: Added initial 7125 support.
 *
 * Hydra_Software_Devel/20   8/13/09 3:51p tdo
 * PR55225: Initial VDC/Appframework support for 7342
 *
 * Hydra_Software_Devel/19   7/31/09 6:49p syang
 * PR 55812:  add 7550 support
 *
 * Hydra_Software_Devel/18   2/4/09 1:41p tdo
 * PR51627: add vdc_test support for 7336
 *
 * Hydra_Software_Devel/17   12/17/08 8:49p rpan
 * PR50391: Static buffer bringup for 7420.
 *
 * Hydra_Software_Devel/16   6/11/08 10:13a tdo
 * PR41488: Cleaning warnings
 *
 * Hydra_Software_Devel/15   6/4/08 5:25p yuxiaz
 * PR39807: 3548 static buffer test bring up.
 *
 * Hydra_Software_Devel/14   6/4/08 11:42a yuxiaz
 * PR39807: 3548 bring up.
 *
 * Hydra_Software_Devel/13   3/10/08 4:07p yuxiaz
 * PR39807: Static buffer bring up on 3548.
 *
 * Hydra_Software_Devel/12   12/14/07 11:16a tdo
 * PR36898: Bringup 7335
 *
 * Hydra_Software_Devel/11   11/10/07 2:51p yuxiaz
 * PR34540: 7325 bringup.
 *
 * Hydra_Software_Devel/10   10/16/07 2:46p rpan
 * PR34857: First version after merging the latest xvd_vdc_test and
 * mvd_vdc_test as of Oct. 16, 2007, 12pm.
 *
 *
 * Hydra_Software_Devel/1   9/17/07 4:37p rpan
 * PR34857: Initial revision. Ported from xvd_vdc_test.
 *
 *
 ***************************************************************************/

#include <math.h>           /* for ceil */
#include <stdio.h>          /* printf */

/* base modules */
#include "bstd.h"           /* standard types */
#include "berr.h"           /* error code */
#include "bkni.h"           /* kernel interface */

#if (BTST_P_SUPPORT_DMA)
#include "bdma.h"
#endif

#include "framework.h"

/* test include */
#include "config.h"
#include "test_util.h"
#include "commands.h"
#include "mpg_decoder.h"
#include "bchp_sun_top_ctrl.h"

BDBG_MODULE(static_buffer);


#define TEST_FRAME_RATE                          (60)

#define STRIPE_WIDTH_128                         (128)
#define STRIPE_WIDTH_64                          (64)

/*
 * Test move state: for window movement test
 */
typedef enum TEST_EMoveState
{
	eMOVE_RIGHT = 0,
	eMOVE_LEFT,
	eMOVE_UP,
	eMOVE_DOWN
} TEST_EMoveState;

typedef enum TEST_SurfacePattern
{
	TEST_SurfacePattern_eFixedColor = 0,
	TEST_SurfacePattern_eColorBar,
	TEST_SurfacePattern_eCheckBoard,
	TEST_SurfacePattern_eCrossHatch,
	TEST_SurfacePattern_eHLumaRamp,
	TEST_SurfacePattern_eVLumaRamp,
	TEST_SurfacePattern_eHorzColorBar,
	TEST_SurfacePattern_eCDC,
	TEST_SurfacePattern_eBitmapImage
} TEST_SurfacePattern;

typedef struct _TEST_MpegSrc_FrameBuffer
{
	/* Address on main memory */
	uint8_t            *pucLumaAddr;
	uint8_t            *pucChromaAddr;
	uint32_t            ulLumaBufSize;
	uint32_t            ulChromaBufSize;

	uint32_t           uiLumaRowStride;
	uint32_t           uiChromaRowStride;
	uint32_t           ulFrames;/* number of frames */
	uint32_t           ulLumaFrameOffset;
	uint32_t           ulChromaFrameOffset;
	bool               bDumpImage;

	/* Address on DMA memory is needed */
	uint8_t            *pucDmaMemLumaAddr;
	uint8_t            *pucDmaMemChromaAddr;
} TEST_MpegSrc_FrameBuffer;

typedef struct _TEST_MpegSrc_Field
{
	BAVC_MFD_Picture          stMFD_Field;
	TEST_MpegSrc_FrameBuffer  stFrameBuffer;
	uint32_t                  uiFrameCount;/* which frame in the frame buffers */
	uint32_t                  uiFrameIndex;/* the number of frames fed */
	BVDC_Source_Handle        hSource;
	uint32_t                  ulMuteColor; /* ARGB8888 format */
	bool                      bEnablePanScan;
	bool                      bEnableIntercept;
	int                       iMfdIndex;
	uint32_t                  ulStripeWidth;
	BMEM_Handle               hHeap;
	bool					  bXvd;
    bool                      bInterlaced;
    BAVC_Polarity             ePolarity;

	char                      chBitmapImageName[256];
	char                      chBitmapImageName_R[256];

} TEST_MpegSrc_Field;

/* static buffer structures */
static TEST_MpegSrc_Field     s_stTEST_MpegSrc_Field[BTST_P_MAX_MFD];
static BINT_CallbackHandle    s_hMpegSrcCallback[BTST_P_MAX_MFD][3];

/* global variables */
extern  bool            g_bInterceptFrame;
void           *g_pvLumaStartAddress, *g_pvChromaStartAddress;
uint32_t        g_uiLumaBufSize, g_uiChromaBufSize;
bool            g_bPausePanScan = false;
extern  bool            g_bCapture;
extern  uint32_t        g_ulAdjQp;
extern  BFramework_Info g_frmInfo;
extern  bool            g_bEnablePicListForMosaicMode;
extern  bool            g_bMosaicWorst;
extern  BTST_P_Context *g_pContext;

/* reference:  Brian Schoner's Doc DRAM MAP 1.0 and bxvd_platform_revk0.c */
static int32_t Test_P_GetBankHeight(void)
{
	BERR_Code rc = BERR_SUCCESS;
	uint32_t ulMemPartSize, ulDramWidth;
	int32_t uiBankHeight;
	
	rc = BCHP_GetFeature(g_pContext->hChp, BCHP_Feature_eMemCtrl0DDRDeviceTechCount, &ulMemPartSize);
	if (rc != BERR_SUCCESS)
	{
		BDBG_MSG(("BCHP_GetFeature; feature not supported: eMemCtrl0DDR3DeviceTechCount"));
		/* 7400/7403 only */
		uiBankHeight = 0;
		return (uiBankHeight);
	}
	
	rc = BCHP_GetFeature(g_pContext->hChp, BCHP_Feature_eMemCtrl0DramWidthCount, &ulDramWidth);
	if (rc != BERR_SUCCESS)
	{
		BDBG_MSG(("BCHP_GetFeature; feature not supported: eMemCtrl0DramWidthCount"));
		uiBankHeight = 0;
		return (uiBankHeight);
	}
	
	
	/* Set bank height base on bus width and memory part size */
	switch(ulDramWidth)
	{
#ifdef BCHP_SUN_TOP_CTRL_STRAP_VALUE_0_strap_ddr_configuration
		/* BCHP_CHIP==7405/7325/7335/7336*/
		/* Dram bus width 32 */
		case 32:
			uiBankHeight = 2; /* 4 Mblks */
			break;
		case 16
			uiBankHeight = 1; /* 4 Mblks */
			break;
#else
		/* Dram bus width 32 */
		case 32:
	
			switch(ulMemPartSize)
			{
				case 256: /* 256 Mbits Device Tech*/
					uiBankHeight = 1; /* 2 Mblks */
					break;
		
				case 512:	/* 512 Mbits Device Tech*/
				case 1024: /* 1024 Mbits Device Tech*/
				case 2048: /* 2048 Mbits Device Tech*/
				case 4096: /* 4096 Mbits Device Tech*/
				case 8192: /* 8192 Mbits Device Tech*/
					uiBankHeight = 1; /* 2 Mblks */
					break;
		
				default:
					uiBankHeight = 1; /* 2 Mblks */
					BDBG_ERR(("Unknown Value in DDR0 Device Config Register"));
					break;
			}
	
			break;
		/* Dram bus width 32 */
		case 16:
	
			switch(ulMemPartSize)
			{
				case 256: /* 256 Mbits Device Tech*/
					uiBankHeight = 0; /* 1 Mblks */
					break;
		
				 case 512:	/* 512 Mbits Device Tech*/
				 case 1024: /* 1024 Mbits Device Tech*/
				 case 2048: /* 2048 Mbits Device Tech*/
				 case 4096: /* 4096 Mbits Device Tech*/
				 case 8192: /* 8192 Mbits Device Tech*/
					uiBankHeight = 1; /* 2 Mblks */
					break;
		
				default:
					uiBankHeight = 0; /* 1 Mblks */
					BDBG_ERR(("Unknown Value in DDR0 Device Config Register"));
					break;
			}
	
		break;
#endif
		default:
			uiBankHeight  = ~0;
			BDBG_ERR(("Unknown Value in DDR0 Device Config Register"));
			break;
	}
	return (uiBankHeight);
}
/*
 * the utility function to fill the main surface with predefined pattern
 * Return: 0 - succeed; 1 - fail
 */
uint32_t TEST_MFD_CreateFrameBuffers(
	BTST_MDEC_Id			  eMDecId, 		   /* which Mpeg decoder to simulate */
	TEST_MpegSrc_FrameBuffer  *pstFrameBuffer, /* pointer to frame buffer struct */
	TEST_SurfacePattern       ePattern,       /* frame buffer data pattern */
	BMEM_Handle               hMem,
	uint32_t                  ulStripeWidth,
	unsigned long             ulWidth,        /* picture width */
	unsigned long             ulHeight,       /* picture height */
	bool                      bHD,            /* HD? required for colorbar pattern */
	unsigned long             ulFrames,       /* 3 or 4 frames buffer */
	uint32_t                 *pulLuminanceNMBY,
	uint32_t                 *pulChrominanceNMBY,
	bool                      bChroma422,     /* is 422 image */
	bool                      bLeftBuffer )
{
	unsigned long xi, yi;
	uint32_t offset, ulStripeNum;
	uint8_t  Y, Cb, Cr;
	unsigned long ulNumOfStripes, ulTotalByteWidth, ulTotalLumaHeight, ulTotalChromaHeight;
	unsigned long ulBankHeightLuma=0, ulBankRemainder, ulBankAddLuma=0, ulBankHeightChroma=0, ulBankAddChroma=0;
	int32_t      uiBankHeight;
	unsigned long ulLumaBufSize, ulChromaBufSize;
	uint8_t *pvLumaStartAddress, *pvChromaStartAddress;
	uint32_t mbRow, mbCol, line, col;
	uint32_t uiFrameIndex;          /* frame index */
	FILE    *fp = NULL;

	BDBG_ENTER(TEST_MFD_CreateFrameBuffers);

	/* initialize */
	pstFrameBuffer->pucLumaAddr = NULL;
	pstFrameBuffer->pucChromaAddr = NULL;
	*pulLuminanceNMBY = 0;
	*pulChrominanceNMBY = 0;

	/* A fixed color will not store values in the buffer. Instead it will
	   use the constant color from the feeder */
	if( ePattern == TEST_SurfacePattern_eFixedColor )
	{
		/* exit */
		return 0;
	}

	if(ePattern == TEST_SurfacePattern_eBitmapImage)
	{
		int16_t   iOffBits;
		int       iMfdIndex = g_pContext->iSelectedVidSrc;

		if(bLeftBuffer)
		{
			fp = fopen(s_stTEST_MpegSrc_Field[iMfdIndex].chBitmapImageName, "rb");
			if(fp == NULL)
			{
				printf("Can not open file %s\n", s_stTEST_MpegSrc_Field[iMfdIndex].chBitmapImageName);
				return BERR_TRACE(BERR_INVALID_PARAMETER);
			}
		}
		else
		{
			fp = fopen(s_stTEST_MpegSrc_Field[iMfdIndex].chBitmapImageName_R, "rb");
			if(fp == NULL)
			{
				printf("Can not open file %s\n", s_stTEST_MpegSrc_Field[iMfdIndex].chBitmapImageName_R);
				return BERR_TRACE(BERR_INVALID_PARAMETER);
			}
		}

		fseek( fp, 10, SEEK_SET );
		fread( &iOffBits, 4, 1, fp );
		fseek( fp, iOffBits, SEEK_SET );
	}

	if (IS_XVD(eMDecId))
	{
		uint32_t ulLumaOffset, ulChromaOffset;

		/* Determine number of vertical stripes for both luma and chroma
		   buffers. ulStripeWidth is the width of the stripe in bytes */
		ulNumOfStripes = (ulWidth + (ulStripeWidth-1)) / ulStripeWidth;
		ulTotalByteWidth = ulStripeWidth * ulNumOfStripes;

		uiBankHeight = Test_P_GetBankHeight();
		ulBankHeightLuma   = (ulHeight + 15)/16;
		ulBankHeightChroma = ((ulHeight >> (1 - bChroma422)) + 15) / 16;

		switch(uiBankHeight)
		{
			/* bank height is 4MBlk tall,16N + 8 */
			case (2):
				ulBankRemainder = ulBankHeightLuma & 15;
				ulBankAddLuma   = (8 - ulBankRemainder) & 15;
				ulBankRemainder = ulBankHeightChroma & 15;
				ulBankAddChroma = (8 - ulBankRemainder) & 15;
				break;
			/* bank height is 2MBlk tall,8N + 4 */
			case (1):
					
					ulBankRemainder = ulBankHeightLuma & 7;
					ulBankAddLuma	= (4 - ulBankRemainder) & 7;
					ulBankRemainder = ulBankHeightChroma & 7;
					ulBankAddChroma = (4 - ulBankRemainder) & 7;
			case (0):
			default:
				/* bank height is 1MBlk tall, 4N + 2 */
				ulBankRemainder = ulBankHeightLuma & 3;
				ulBankAddLuma   = (2 - ulBankRemainder) & 3;
				ulBankRemainder = ulBankHeightChroma & 3;
				ulBankAddChroma = (2 - ulBankRemainder) & 3;
				break;
		}

		ulTotalLumaHeight = (ulBankHeightLuma + ulBankAddLuma) * 16;
		*pulLuminanceNMBY = (ulBankHeightLuma + ulBankAddLuma);
		ulTotalChromaHeight = (ulBankHeightChroma + ulBankAddChroma) * 16;
		*pulChrominanceNMBY = (ulBankHeightChroma + ulBankAddChroma);

		/* Calculate size of luma and chroma buffers in bytes */
		ulLumaBufSize   = ulTotalByteWidth*ulTotalLumaHeight;
		ulChromaBufSize = ulTotalByteWidth*ulTotalChromaHeight;

		/* Set offset */
		ulLumaOffset   = ulLumaBufSize;
		ulChromaOffset = ulChromaBufSize;

		/* Calculate total buffer size */
		ulLumaBufSize   = ulLumaBufSize*ulFrames;
		ulChromaBufSize = ulChromaBufSize*ulFrames;

		/* Allocate luma and chroma buffers */
		pvLumaStartAddress =
			(uint8_t *)BMEM_AllocAligned( hMem, ulLumaBufSize, 8, 0 );
		if (!pvLumaStartAddress)
		{
			BDBG_ERR(("ERROR: BMEM_AllocAligned out of heap memory!"));
			return 1;
		}
		pvChromaStartAddress =
			(uint8_t *)BMEM_AllocAligned( hMem, ulChromaBufSize, 8, 0 );
		if (!pvChromaStartAddress)
		{
			BDBG_ERR(("ERROR: BMEM_AllocAligned out of heap memory!"));
			return 1;
		}

		/* Traverse through each column in the frame */
		for( yi = 0; yi < ulHeight; yi++ )
		{
			/* Traverse through each row in the frame */
			for ( xi = 0; xi < ulWidth; xi++)
			{
				/* traverse through each frame */
				for (uiFrameIndex = 0; uiFrameIndex < ulFrames; uiFrameIndex++ )
				{
					/* which pattern to use? */
					switch( ePattern )
					{
					case TEST_SurfacePattern_eColorBar:
						/* the colorbar pattern will left-shift a region for each frame */
						TEST_colorbar_YCbCr_pixel( xi, ulWidth, bHD, uiFrameIndex, &Y, &Cb, &Cr );
						break;
					case TEST_SurfacePattern_eHorzColorBar:
						TEST_colorbar_YCbCr_pixel( yi, ulHeight, bHD, uiFrameIndex, &Y, &Cb, &Cr );
						break;
					case TEST_SurfacePattern_eCheckBoard:
						TEST_checkboard_YCbCr_pixel( xi, ulWidth, yi, ulHeight, &Y, &Cb, &Cr );
						break;
					case TEST_SurfacePattern_eCrossHatch:
						TEST_crosshatch_YCbCr_pixel( xi, yi, &Y, &Cb, &Cr );
						break;
					case TEST_SurfacePattern_eHLumaRamp:
						TEST_lumaramp_YCbCr_pixel( xi, ulWidth, yi, ulHeight, &Y, &Cb, &Cr, true );
						break;
					case TEST_SurfacePattern_eVLumaRamp:
						TEST_lumaramp_YCbCr_pixel( xi, ulWidth, yi, ulHeight, &Y, &Cb, &Cr, false );
						break;
					case TEST_SurfacePattern_eCDC:
						TEST_cdc_YCbCr_pixel( xi, yi, ulWidth, ulHeight, &Y, &Cb, &Cr );
						break;
					case TEST_SurfacePattern_eBitmapImage:
						TEST_Bitmap_YCbCr_pixel(fp, xi, yi, ulWidth, ulHeight, &Y, &Cb, &Cr);
						break;
					default:
						BDBG_ERR(("TODO: frame buffer filling Error handling."));
						return 1;
					}/* end of switch pattern */

					/* determine which stripe contains this pixel */
					ulStripeNum = xi / ulStripeWidth;

					/* calculate the pixel's luma byte offset */
					offset = uiFrameIndex * ulLumaOffset +
						(xi % ulStripeWidth) +
						(yi * ulStripeWidth) +
						(ulStripeNum * ulStripeWidth * ulTotalLumaHeight);

					/* store luma byte */
					BDBG_ASSERT( offset < ulLumaBufSize);
					/* NOTE: mpeg feeder is in big-endian if it's mpeg format data!!! */
					*(pvLumaStartAddress + BTST_XVD_GET_LUMA_OFFSET(offset)) = Y;

					/* store chroma? */
					if ((xi%2 == 0) && (bChroma422 || (yi%2 == 0)))
					{
						/* calculate pixel's Cr byte offset */
						offset = uiFrameIndex * ulChromaOffset +
							(xi % ulStripeWidth) +
							(bChroma422 ? yi : yi/2) * ulStripeWidth +
							(ulStripeNum * ulStripeWidth * ulTotalChromaHeight);

						/* store chroma bytes */
						BDBG_ASSERT( offset < ulChromaBufSize);

						/* NOTE: mpeg feeder is in big-endian if it's mpeg format data!!! */
						*(pvChromaStartAddress + BTST_XVD_GET_CHROMA_OFFSET(offset))     = Cr;
						*(pvChromaStartAddress + BTST_XVD_GET_CHROMA_OFFSET(offset)+1) = Cb;
					}
				}
			}
		}

		/* store finished buffers */
		pstFrameBuffer->pucLumaAddr   = pvLumaStartAddress;
		pstFrameBuffer->pucChromaAddr = pvChromaStartAddress;
		pstFrameBuffer->ulLumaBufSize = ulLumaBufSize;
		pstFrameBuffer->ulChromaBufSize = ulChromaBufSize;

		pstFrameBuffer->ulFrames            = ulFrames;
		pstFrameBuffer->ulLumaFrameOffset   = ulLumaOffset;
		pstFrameBuffer->ulChromaFrameOffset = ulChromaOffset;
	}
	else
	{
		/* to calculate the favorable number of macroblocks(fnmbx) per row */
		unsigned long nmbx = ( ulWidth * ulFrames + 15 ) / 16;
		unsigned long n = (nmbx / 8 + 1 ) / 2 ;
		unsigned long fnmbx = (( n << 1 ) + 1) << 3;

		unsigned long bytesPerBigRow = fnmbx << 8;/* big row contains all frames' row and waste MB's */
		unsigned long bytesPerRow = (( ulWidth + 15 ) >> 4) << 8;/* a row contains one frame's row */
		unsigned long totalRows = (ulHeight + 15) / 16;
		unsigned long uiLumaBufSize = bytesPerBigRow * totalRows;/* interleaved frames buffer */
		/* TODO: double check with MVD firmware that the chroma buffer is organized this way!
		 * if odd number of rows => reserve one more row for chroma */
		unsigned long uiChromaBufSize = bytesPerBigRow * ( (totalRows+1) >> (!bChroma422) );/* interleaved frames buffer */

		BDBG_ENTER(TEST_MFD_CreateFrameBuffer);

		/* allocate luma/chroma frame buffers */
		if( (pvLumaStartAddress =
				(uint8_t *)BMEM_AllocAligned( hMem, uiLumaBufSize, 8, 0 )  /* MB aligned */
			 ) == NULL )
		{
			BDBG_ERR(("ERROR: BMEM_AllocAligned out of heap memory!"));
			return 1;
		}
		if( (pvChromaStartAddress =
				(uint8_t *)BMEM_AllocAligned( hMem, uiChromaBufSize, 8, 0 )/* MB aligned */
			) == NULL )
		{
			BDBG_ERR(("ERROR: BMEM_AllocAligned out of heap memory!"));
			return 1;
		}

		/* fill in each line of the frame */
		for( xi = 0; xi < ulHeight; xi++ )
		{
			/* calculate macroblock line and row within macroblock */
			mbRow = xi / 16;
			line  = xi % 16;

			/* for each pixel of the line */
			for( yi = 0; yi < ulWidth; yi++ )
			{
				/* calculate macroblock and column within the macroblock */
				mbCol = yi / 16;
				col = yi % 16;

				/* traverse through each frame */
				for (uiFrameIndex = 0; uiFrameIndex < ulFrames; uiFrameIndex++ )
				{
					/* which pattern to use? */
					switch( ePattern )
					{
					case TEST_SurfacePattern_eColorBar:
						/* the colorbar pattern will left-shift a region for each frame */
						TEST_colorbar_YCbCr_pixel( yi, ulWidth, bHD, uiFrameIndex, &Y, &Cb, &Cr );
						break;
					case TEST_SurfacePattern_eCheckBoard:
						TEST_checkboard_YCbCr_pixel( yi, ulWidth, xi, ulHeight, &Y, &Cb, &Cr );
						break;
					case TEST_SurfacePattern_eCrossHatch:
						TEST_crosshatch_YCbCr_pixel( yi, xi, &Y, &Cb, &Cr );
						break;
					case TEST_SurfacePattern_eHLumaRamp:
						TEST_lumaramp_YCbCr_pixel( yi, ulWidth, xi, ulHeight, &Y, &Cb, &Cr, true );
						break;
					case TEST_SurfacePattern_eVLumaRamp:
						TEST_lumaramp_YCbCr_pixel( yi, ulWidth, xi, ulHeight, &Y, &Cb, &Cr, false );
						break;
					default:
						BDBG_ERR(("TODO: frame buffer filling Error handling."));
					}/* end of switch pattern */

					/* calculate the pixel's luma offset within the interleaved frame buffer */
					offset = uiFrameIndex * bytesPerRow + /* frame offset within a big row */
						mbRow * bytesPerBigRow +		 /* big row offset */
						(mbCol << 8) +					  /* MB offset */
						(line << 4) +					   /* line offset */
						col;							  /* column offset */

					/* store luma value */
					BDBG_ASSERT( offset < uiLumaBufSize );

					/* NOTE: mpeg feeder is in big-endian if it's mpeg format data!!! */
					*(pvLumaStartAddress + Y_OFFSET(offset))= Y;

					/* MPEG 4:2:2 format chroma */
					if(bChroma422)
					{
						/* every line and even col has co-located cbcr pairs */
						if( !(col & 1) )
						{
							if( !(col & 3) )/* luma colomn 0, 4, 8, 12, will be the same chroma colomn */
							{/* first half of the chroma pair */
								offset = uiFrameIndex * bytesPerRow +	  /* frame offset */
										(mbRow) * bytesPerBigRow +	 /* row offset */
										(mbCol << 8) +					  /* MB offset */
										(line  << 4) +					  /* line offset */
										col;							  /* colomn offset */
							}else/* luma colomn 2, 6, 10, 14 will be chroma colomn 1, 5, 9, 13 */
							{/* second half of the chroma pair */
								offset = uiFrameIndex * bytesPerRow +	  /* frame offset */
										(mbRow) * bytesPerBigRow +	 /* row offset */
										(mbCol << 8) +					  /* MB offset */
										(line  << 4) +					  /* line offset */
										col - 1;						  /* colomn offset */
							}
							/* assert the buffer range */
							BDBG_ASSERT( offset < uiChromaBufSize-2 );

							/* NOTE: mpeg feeder is in big-endian if it's mpeg format data!!! */
							*(pvChromaStartAddress + CB_OFFSET(offset)) = Cb; /* chroma buffer */
							*(pvChromaStartAddress + CR_OFFSET(offset)) = Cr;
						}/* end of chroma if */
					}
					/* MPEG 4:2:0 format chroma */
					else
					{
						/* even line and even col has co-located cbcr pairs */
						if( !(line & 1) && !(col & 1) )
						{
							if( !(col & 3) )/* luma colomn 0, 4, 8, 12, will be the same chroma colomn */
							{/* first half of the chroma pair */
								offset = uiFrameIndex * bytesPerRow +	  /* frame offset */
										(mbRow >> 1) * bytesPerBigRow +  /* row offset */
										(mbCol << 8) +					  /* MB offset */
										(line << 3) +					   /* line offset */
										col;							  /* colomn offset */
							}else/* luma colomn 2, 6, 10, 14 will be chroma colomn 1, 5, 9, 13 */
							{/* second half of the chroma pair */
								offset = uiFrameIndex * bytesPerRow +	  /* frame offset */
										(mbRow >> 1) * bytesPerBigRow +  /* row offset */
										(mbCol << 8) +					  /* MB offset */
										(line << 3) +					   /* line offset */
										col - 1;						  /* colomn offset */
							}
							if(mbRow & 1)
							{/* even luma row: top half 16X8 chroma MB;
								odd luma row: bottom half 16X8 chroma MB */
								offset += 128;
							}
							/* assert the buffer range */
							BDBG_ASSERT( offset < uiChromaBufSize-2 );

							/* NOTE: mpeg feeder is in big-endian if it's mpeg format data!!! */
							*(pvChromaStartAddress + CB_OFFSET(offset)) = Cb; /* chroma buffer */
							*(pvChromaStartAddress + CR_OFFSET(offset)) = Cr;
						}/* end of chroma if */
					}
				}/* end of frame index for loop */
			}/* end of width for loop */
		}/* end of height for loop */

		pstFrameBuffer->pucLumaAddr 	  = pvLumaStartAddress;
		pstFrameBuffer->pucChromaAddr	  = pvChromaStartAddress;
		pstFrameBuffer->uiLumaRowStride   = bytesPerBigRow;
		pstFrameBuffer->uiChromaRowStride = bytesPerBigRow;
		pstFrameBuffer->ulFrames            = ulFrames;
		pstFrameBuffer->ulLumaFrameOffset   = bytesPerRow;
		pstFrameBuffer->ulChromaFrameOffset = bytesPerRow;
	}

	if((ePattern == TEST_SurfacePattern_eBitmapImage) && fp)
	{
		fclose(fp);
	}


	BDBG_LEAVE(TEST_MFD_CreateFrameBuffers);
	return 0;
}

#if (BTST_P_SUPPORT_DMA)
static BERR_Code TEST_Mpeg_DmaFrameBuffers(
	BCHP_Handle      hChp,
	BREG_Handle      hReg,
	BMEM_Handle      hMem,
	BINT_Handle      hInt,
	BMEM_Handle      hSrcMem,
	unsigned char   *pBufSrc,
	BMEM_Handle      hDstMem,
	unsigned char   *pBufDst,
	uint32_t         ulNumBytes )
{
	BERR_Code eResult = BERR_SUCCESS;
	BDMA_Handle  hDma;
	BDMA_Mem_Handle  hMemDma;
	BDMA_Mem_Tran_Handle  hTran;
	uint32_t  ulSrcOffset, ulDstOffset;
	BDMA_TranStatus  eTranStatus;

	/* open dma module */
	eResult = BDMA_Open(&hDma, hChp, hReg, hMem, hInt);
	BDBG_ASSERT( BERR_SUCCESS == eResult );

	/* create mem dma sub-module */
	eResult = BDMA_Mem_Create( hDma, &hMemDma );
	BDBG_ASSERT( eResult == BERR_SUCCESS );

	/* create a Tran */
	eResult = BDMA_Mem_Tran_Create( hMemDma, 1, &hTran );
	BDBG_ASSERT( BERR_SUCCESS == eResult );

	/* set swap, ok to change per block */
	eResult = BDMA_Mem_SetByteSwapMode( hMemDma, BDMA_Endian_eLittle,
		BDMA_SwapMode_eNoSwap );
	BDBG_ASSERT( BERR_SUCCESS == eResult );

	/* convert software addr to bus addr */
	BMEM_ConvertAddressToOffset( hSrcMem, pBufSrc, &ulSrcOffset );
	BMEM_ConvertAddressToOffset( hDstMem, pBufDst, &ulDstOffset );

	eResult  = BDMA_Mem_SetCrypt( hMemDma, BDMA_CryptMode_eNoCrypt, 0 /*ulKeySlot*/ );
	BDBG_ASSERT( BERR_SUCCESS == eResult );

	/* set block src/dst addr and size */
	eResult |= BDMA_Mem_Tran_SetDmaBlockInfo( hTran, 0,
		ulDstOffset, ulSrcOffset, ulNumBytes, false);
	BDBG_ASSERT( BERR_SUCCESS == eResult );

	/* start the dma transfer */
	eResult = BDMA_Mem_Tran_Start(hTran);
	BDBG_ASSERT( BERR_SUCCESS == eResult );

	/* wait for it to finish */
	do {
		BKNI_Sleep(100);
		BDMA_Mem_Tran_GetStatus(hTran, &eTranStatus);
	} while ( BDMA_TranStatus_eInProgress == eTranStatus );

	if ( BDMA_TranStatus_eSucceeded == eTranStatus )
	{
		BDBG_MSG(("      buf dma done"));
	}
	else
	{
		BDBG_ERR(("      buf dma failed. eTranStatus is %d", eTranStatus));
	}

	/* clean up */
	eResult  = BDMA_Mem_Tran_Destroy( hTran );
	eResult |= BDMA_Mem_Destroy( hMemDma );
	eResult |= BDMA_Close( hDma );
	BDBG_ASSERT( BERR_SUCCESS == eResult );

	return eResult;
}
#endif

/*======== Callback: XVD Source ==========
 * This Callback will supply the XVD source frame buffer to the XVD
 * in behalf of XVD PI for VDC unit test such that the VDC unit test
 * is de-linked from the external modules.
 */
void TEST_MpegSrc_Callback(void *parm1, int iField)
{
	TEST_MpegSrc_Field      *pstFieldData = (TEST_MpegSrc_Field *)parm1;
	static bool             bValidMove = false, bFirst = true;
	static TEST_EMoveState  eMoveState = eMOVE_RIGHT;
	static int16_t          iXOffsetMax;
	static int16_t          iYOffsetMax;
	static int16_t          iLeft, iTop;/* in 1/16th pixel unit */
	BAVC_XVD_Picture        *pFieldData = &(pstFieldData->stMFD_Field);
	BAVC_XVD_Picture        stPic;
	BAVC_XVD_Picture       *pMvdField = &stPic;
	static uint32_t         uiCnt = 0;
	uint32_t  uiAvcFieldSize = sizeof(BAVC_MFD_Picture);
	uint32_t                bytesPerRow = (( pstFieldData->stMFD_Field.ulSourceHorizontalSize + 15 ) >> 4) << 8;/* a row contains one frame's row */
	uint32_t                totalRows = (pstFieldData->stMFD_Field.ulSourceVerticalSize + 15) >> 4;
	uint32_t  uiLumaBufSize = pstFieldData->stFrameBuffer.uiLumaRowStride * (totalRows - 1) + bytesPerRow;
	uint32_t  uiChromaBufSize = pstFieldData->stFrameBuffer.uiChromaRowStride * ((totalRows + 1) / 2 - 1) + bytesPerRow;

#ifdef BTST_MFD_SIZE
	extern uint32_t g_ulSizeX;
	extern uint32_t g_ulSizeY;
	pstFieldData->stAVC_MVD_Field.ulSourceHorizontalSize = g_ulSizeX;
	pstFieldData->stAVC_MVD_Field.ulSourceVerticalSize	 = g_ulSizeY;
	pstFieldData->stAVC_MVD_Field.ulDisplayHorizontalSize= g_ulSizeX;
	pstFieldData->stAVC_MVD_Field.ulDisplayVerticalSize  = g_ulSizeY;
#endif

	extern BAVC_BarDataType eBarDataType ;
	extern uint32_t         ulTopLeftBarValue;
    extern uint32_t         ulBotRightBarValue;
	extern uint32_t         ulAfd;

	pFieldData->eBarDataType       = eBarDataType;
	pFieldData->ulTopLeftBarValue  = ulTopLeftBarValue;
	pFieldData->ulBotRightBarValue = ulBotRightBarValue;
	pFieldData->bValidAfd = true;
	pFieldData->ulAfd     = ulAfd;
	if(pstFieldData->bEnablePanScan)
	{
		if(bFirst) {
			iXOffsetMax = (pstFieldData->stMFD_Field.ulSourceHorizontalSize -
					pstFieldData->stMFD_Field.ulDisplayHorizontalSize) << 3;
			iYOffsetMax = (pstFieldData->stMFD_Field.ulSourceVerticalSize -
					pstFieldData->stMFD_Field.ulDisplayVerticalSize) << 3;
			iLeft = -iXOffsetMax;
			iTop = -iYOffsetMax;
			bFirst = false;
			BDBG_MSG(("source moves right from (0x%x, 0x%x)\n", iLeft, iTop));
		}
	}

	if (pstFieldData->bEnableIntercept)
	{
		/* if requested => intercept a frame to be dumped later on when exiting */
		if(g_bInterceptFrame)
		{
			BDBG_MSG(("Source Width  = %d\n", pFieldData->ulSourceHorizontalSize));
			BDBG_MSG(("Source Height = %d\n", pFieldData->ulSourceVerticalSize));
			BDBG_MSG(("size of AVC field data structure = %d\n", uiAvcFieldSize));
			BDBG_MSG(("Calculated luma buffer size = %d\n", uiLumaBufSize));
			BDBG_MSG(("Calculated chroma buffer size = %d\n", uiChromaBufSize));
			BDBG_ASSERT(((uiLumaBufSize + uiAvcFieldSize) < g_uiLumaBufSize));
			BDBG_ASSERT((uiChromaBufSize < g_uiChromaBufSize));
			if(++uiCnt == 1)
			{/* only capture the 20th frame from now on */
				BKNI_Memcpy(g_pvLumaStartAddress, pFieldData, uiAvcFieldSize);
				BKNI_Memcpy(
					(void*)((uint32_t)g_pvLumaStartAddress + uiAvcFieldSize),
					pFieldData->pLuminanceFrameBufferAddress,
					uiLumaBufSize);
				BKNI_Memcpy(
					g_pvChromaStartAddress,
					pFieldData->pChrominanceFrameBufferAddress,
					uiChromaBufSize);
			}
		}
	}

	if( !pstFieldData->stMFD_Field.bMute )
	{
		/* for unit test, I'll supply the mpeg field data */

		if(pstFieldData->hHeap == g_frmInfo.hMem)
		{
			pstFieldData->stMFD_Field.pLuminanceFrameBufferAddress = (void *)
				((uint32_t)pstFieldData->stFrameBuffer.pucLumaAddr +
				pstFieldData->stFrameBuffer.ulLumaFrameOffset * pstFieldData->uiFrameCount);
			pstFieldData->stMFD_Field.pChrominanceFrameBufferAddress = (void *)
				((uint32_t)pstFieldData->stFrameBuffer.pucChromaAddr +
				pstFieldData->stFrameBuffer.ulChromaFrameOffset * pstFieldData->uiFrameCount);
		}
		else
		{
			pstFieldData->stMFD_Field.hHeap = pstFieldData->hHeap;
#if (BTST_P_SUPPORT_DMA) 
			/* these chips CPU has only access to MEMC0; has to load the static buffer by DMA */
			pstFieldData->stMFD_Field.pLuminanceFrameBufferAddress = (void *)
				((uint32_t)pstFieldData->stFrameBuffer.pucDmaMemLumaAddr +
				pstFieldData->stFrameBuffer.ulLumaFrameOffset * pstFieldData->uiFrameCount);
			pstFieldData->stMFD_Field.pChrominanceFrameBufferAddress = (void *)
				((uint32_t)pstFieldData->stFrameBuffer.pucDmaMemChromaAddr +
				pstFieldData->stFrameBuffer.ulChromaFrameOffset * pstFieldData->uiFrameCount);
#else
			pstFieldData->stMFD_Field.pLuminanceFrameBufferAddress = (void *)
				((uint32_t)pstFieldData->stFrameBuffer.pucLumaAddr +
				pstFieldData->stFrameBuffer.ulLumaFrameOffset * pstFieldData->uiFrameCount);
			pstFieldData->stMFD_Field.pChrominanceFrameBufferAddress = (void *)
				((uint32_t)pstFieldData->stFrameBuffer.pucChromaAddr +
				pstFieldData->stFrameBuffer.ulChromaFrameOffset * pstFieldData->uiFrameCount);
#endif
		}

		if (!pstFieldData->bXvd)
			pstFieldData->stMFD_Field.ulRowStride = pstFieldData->stFrameBuffer.uiChromaRowStride;

#ifdef IKOS_EMULATION
		/* looping the 3 or 4 frames */
		pstFieldData->uiFrameCount = (pstFieldData->uiFrameIndex++) % pstFieldData->stFrameBuffer.ulFrames;
#else
		/* shuffle the frames: repeat 30 times for each frame then loop around the frame buffer. */
		pstFieldData->uiFrameCount = ((pstFieldData->uiFrameIndex++)/TEST_FRAME_RATE) %
			pstFieldData->stFrameBuffer.ulFrames;
		pstFieldData->stMFD_Field.ulDecodePictureId = pstFieldData->uiFrameCount;
#endif

		if(pstFieldData->bEnablePanScan)
		{
			/* decide the new window position */
			/* check if next move will exceed valid coordinates */
			bValidMove = false;
			while(!bValidMove)
			{
				bValidMove = true;

				switch(eMoveState)
				{
				case eMOVE_RIGHT:
					if(((iField == BAVC_Polarity_eTopField) || (iField == BAVC_Polarity_eFrame))
						&& !g_bPausePanScan )
						iLeft += 8;/* half pixel movement */
					if (iLeft > iXOffsetMax)
					{
						eMoveState = eMOVE_DOWN;
						bValidMove = false;
						BDBG_WRN(("source moves down from (0x%x, 0x%x)\n", iLeft, iTop));
					}
					break;

				case eMOVE_DOWN:
					if(((iField == BAVC_Polarity_eTopField) || (iField == BAVC_Polarity_eFrame))
						&& !g_bPausePanScan)
					{
						iTop += 8;/* half pixel movement */
						/*g_bPausePanScan = true;*/
					}
					if ((iTop) > iYOffsetMax)
					{
						eMoveState = eMOVE_LEFT;
						bValidMove = false;
						BDBG_WRN(("source moves left from (0x%x, 0x%x)\n", iLeft, iTop));
					}
					break;

				case eMOVE_LEFT:
					if(((iField == BAVC_Polarity_eTopField) || (iField == BAVC_Polarity_eFrame))
						&& !g_bPausePanScan)
						iLeft -= 8;/* half pixel movement */
					if ((iLeft) < (-iXOffsetMax))
					{
						eMoveState = eMOVE_UP;
						bValidMove = false;
						BDBG_WRN(("source moves up from (0x%x, 0x%x)\n", iLeft, iTop));
					}
					break;

				case eMOVE_UP:
					if(((iField == BAVC_Polarity_eTopField) || (iField == BAVC_Polarity_eFrame))
						&& !g_bPausePanScan)
					{
						iTop -= 8;/* half pixel movement */
					}
					if ((iTop) < (-iYOffsetMax))
					{
						eMoveState = eMOVE_RIGHT;
						bValidMove = false;
						BDBG_WRN(("source moves right from (0x%x, 0x%x)\n", iLeft, iTop));
					}
					break;

				default:

					break;
				}
			}/* end of bValidMove while loop */

			/* pan & scan offset circling: ->(-1, 1) -> (1, 1) -> (1, -1) -> (-1, -1)-> */
			pstFieldData->stMFD_Field.i32_HorizontalPanScan = iLeft;
			pstFieldData->stMFD_Field.i32_VerticalPanScan = iTop;
		}
		else
		{
			pstFieldData->stMFD_Field.i32_HorizontalPanScan = 0;
			pstFieldData->stMFD_Field.i32_VerticalPanScan = 0;
		}
		/*TODO: to config other mpeg info */
	}

	/* adapt the source field polarity to the registered polarity */
	switch (iField)
	{
		case BAVC_Polarity_eTopField:
			pstFieldData->stMFD_Field.eSourcePolarity = pstFieldData->bInterlaced
				? BAVC_Polarity_eTopField : BAVC_Polarity_eFrame;
			pstFieldData->stMFD_Field.eInterruptPolarity = BAVC_Polarity_eTopField;
			break;

		case BAVC_Polarity_eBotField:
			pstFieldData->stMFD_Field.eSourcePolarity = pstFieldData->bInterlaced
				? BAVC_Polarity_eBotField : BAVC_Polarity_eFrame;
			pstFieldData->stMFD_Field.eInterruptPolarity = BAVC_Polarity_eBotField;
			break;

		case BAVC_Polarity_eFrame:
			if(pstFieldData->bInterlaced)
			{
				if(pstFieldData->ePolarity == BAVC_Polarity_eTopField)
				{
					pstFieldData->stMFD_Field.eSourcePolarity = BAVC_Polarity_eBotField;
					pstFieldData->ePolarity = BAVC_Polarity_eBotField;
				}
				else
				{
					pstFieldData->stMFD_Field.eSourcePolarity = BAVC_Polarity_eTopField;
					pstFieldData->ePolarity = BAVC_Polarity_eTopField;
				}
			}
			else
			{
				pstFieldData->stMFD_Field.eSourcePolarity = BAVC_Polarity_eFrame;
			}
			pstFieldData->stMFD_Field.eInterruptPolarity = BAVC_Polarity_eFrame;
			break;
		default:
			BDBG_ERR(("Invalid iField %d!", iField));
			BDBG_ASSERT(0);
	}

	pstFieldData->stMFD_Field.ulAdjQp = g_ulAdjQp;
	pstFieldData->stMFD_Field.bCaptureCrc = g_bCapture;

	/* copy to local structure so following code won't change the original test field data */
	BKNI_Memcpy_isr((void*)&stPic, (void*)pFieldData, sizeof(BAVC_XVD_Picture));

	/* MosaicMode test: always feed 16 sub-pictures; */
	if(g_bEnablePicListForMosaicMode)
	{
		int i;
		uint32_t ulStripeNum;
		int iPicIdx = 0;

		uint32_t ulWidth  = pMvdField->ulSourceHorizontalSize / 4;
		uint32_t ulHeight = pMvdField->ulSourceVerticalSize   / 4;
		int iScambleIdx[ ] = {
			10,  8, 14,  7,
			 1,  6,  0, 13,
			11,  5,  4, 15,
			 3,  2, 12,  9 };

		/* (0) replace picture size */
		pMvdField->ulSourceHorizontalSize  = ulWidth;
		pMvdField->ulSourceVerticalSize    = ulHeight;
		pMvdField->ulDisplayHorizontalSize = ulWidth;
		pMvdField->ulDisplayVerticalSize   = ulHeight;

		for(i = 0; i < 16; i++)
		{
			uint32_t ulRow = iScambleIdx[i] / 4, ulCol = iScambleIdx[i] % 4;
			uint32_t ulLumFrameBufAddr   = (uint32_t)pMvdField->pLuminanceFrameBufferAddress;
			uint32_t ulChromFrameBufAddr = (uint32_t)pMvdField->pChrominanceFrameBufferAddress;

			/* skip closed channel(s) */
			if(!g_pContext->abChanOpened[pstFieldData->iMfdIndex][i]) continue;

			BDBG_MSG(("channel %d opened: wxh = %d x %d mute?%d", i,
               pMvdField->ulSourceHorizontalSize, pMvdField->ulSourceVerticalSize, pMvdField->bMute));

			/* (1) copy the field structure to the picture list */
			BKNI_Memcpy_isr((void*)&g_pContext->astMosaicPictures[pstFieldData->iMfdIndex][iPicIdx], pMvdField, sizeof(BAVC_XVD_Picture));

			/* (2) re-calculate sub-pictures start address */
			/* a stripe is 64 pixels wide */
			ulStripeNum        = ulCol * ulWidth / pstFieldData->ulStripeWidth;

			g_pContext->astMosaicPictures[pstFieldData->iMfdIndex][iPicIdx].pLuminanceFrameBufferAddress = (void*)
				(ulLumFrameBufAddr +
				 ulStripeNum * pMvdField->ulLuminanceNMBY * 16 * pstFieldData->ulStripeWidth +
			     ulRow * ulHeight * pstFieldData->ulStripeWidth +
			     ((ulCol * ulWidth) % pstFieldData->ulStripeWidth));

			g_pContext->astMosaicPictures[pstFieldData->iMfdIndex][iPicIdx].pChrominanceFrameBufferAddress = (void*)
				(ulChromFrameBufAddr +
				/* stripe offset */
				 ulStripeNum * pMvdField->ulChrominanceNMBY * 16 * pstFieldData->ulStripeWidth +

				/* vertical offset within a stripe (take care of 422 vs 420 chroma type) */
		         ((ulRow * ulHeight * pstFieldData->ulStripeWidth /2) << (pMvdField->eYCbCrType - BAVC_YCbCrType_e4_2_0)) +

				/* horizontal offset within a stripe */
		         ((ulCol * ulWidth) % pstFieldData->ulStripeWidth));

			g_pContext->astMosaicPictures[pstFieldData->iMfdIndex][iPicIdx].ulChannelId = i;

			/* (3) update picture list pointer */
			if(iPicIdx > 0)
			{
				g_pContext->astMosaicPictures[pstFieldData->iMfdIndex][iPicIdx - 1].pNext =
					(void*)&g_pContext->astMosaicPictures[pstFieldData->iMfdIndex][iPicIdx];
			}
			/* (4) if dynamic picture change */
			if(g_bMosaicWorst)
			{

				if((i % 2) == 1)
				{
					g_pContext->astMosaicPictures[pstFieldData->iMfdIndex][iPicIdx].ulSourceHorizontalSize = ulWidth * 2 / 3;
					g_pContext->astMosaicPictures[pstFieldData->iMfdIndex][iPicIdx].ulSourceVerticalSize   = ulHeight * 2 / 3;
				}
			}

			/* increment pic index in the link-list */
			iPicIdx++;
		}

		/* (4) replace the callback picture with the first sub-picture; */
		if(iPicIdx)
		{
			pMvdField = &g_pContext->astMosaicPictures[pstFieldData->iMfdIndex][0];
			g_pContext->astMosaicPictures[pstFieldData->iMfdIndex][iPicIdx-1].pNext = NULL;
		}
	}

	/* call BVDC_Source_isr */
	BVDC_Source_MpegDataReady_isr( (void*)pstFieldData->hSource,
		0,
		pMvdField );

}

#if (BTST_P_SUPPORT_DMA)
/*
 * the utility function to create DMA buffers
 * Return: 0 - succeed; 1 - fail
 *
 */
uint32_t TEST_Mpeg_CreateDmaFrameBuffers(
	TEST_MpegSrc_FrameBuffer  *pstFrameBuffer,
	BMEM_Handle               hDmaMem,
	BCHP_Handle               hChp,
	BREG_Handle               hReg,
	BMEM_Handle               hMem,
	BINT_Handle               hInt )
{
	/* Allocate luma and chroma buffers on MEMC_1 */
	pstFrameBuffer->pucDmaMemLumaAddr =
		(uint8_t *)BMEM_AllocAligned(hDmaMem, pstFrameBuffer->ulLumaBufSize, 8, 0 );
	if (!pstFrameBuffer->pucDmaMemLumaAddr )
	{
		BDBG_ERR(("ERROR: BMEM_AllocAligned out of heap memory 1!"));
		return 1;
	}

	pstFrameBuffer->pucDmaMemChromaAddr =
		(uint8_t *)BMEM_AllocAligned(hDmaMem, pstFrameBuffer->ulChromaBufSize, 8, 0 );
	if (!pstFrameBuffer->pucDmaMemChromaAddr)
	{
		BDBG_ERR(("ERROR: BMEM_AllocAligned out of heap memory 1!"));
		return 1;
	}

	/* DMA to hDmaMem */
	TEST_Mpeg_DmaFrameBuffers(hChp, hReg, hMem, hInt,
		hMem, pstFrameBuffer->pucChromaAddr,
		hDmaMem, pstFrameBuffer->pucDmaMemChromaAddr,
		pstFrameBuffer->ulChromaBufSize);

	TEST_Mpeg_DmaFrameBuffers(hChp, hReg, hMem, hInt,
		hMem, pstFrameBuffer->pucLumaAddr,
		hDmaMem, pstFrameBuffer->pucDmaMemLumaAddr,
		pstFrameBuffer->ulLumaBufSize);

	return 0;
}
#endif

/*
 * Initialize MPEG static src data routine
 */
BERR_Code InitializeMpegStaticSrc(
	BTST_P_Context	   *pContext,
	BVDC_Source_Handle  hMfdSource,
	BFMT_VideoFmt       eMfdFormat,
	uint32_t            uiFrames,
	bool                bEnablePanScan,
	uint32_t            uiPattern)
{
	BERR_Code       err = BERR_SUCCESS;
	BINT_Id         InterruptName;
	uint32_t        ulWidth, ulHeight;
	uint32_t        ulLuminanceNMBY, ulChrominanceNMBY;
	int				iMfdIndex = pContext->iSelectedVidSrc;
	uint32_t ulDataWidth;
	const BFMT_VideoInfo  *pMfdFmtInfo;
	bool             bIsHd, bLeftBuffer = true;

	pMfdFmtInfo = BFMT_GetVideoFormatInfoPtr(eMfdFormat);
	ulWidth     = pMfdFmtInfo->ulDigitalWidth;
	ulHeight    = pMfdFmtInfo->ulDigitalHeight;

	if(uiPattern == TEST_SurfacePattern_eBitmapImage)
	{
		/* Check image */
		FILE *fp;

		printf("Enter file name: ");
		BTST_P_Scanf(pContext, "%s", s_stTEST_MpegSrc_Field[iMfdIndex].chBitmapImageName);

		fp = fopen(s_stTEST_MpegSrc_Field[iMfdIndex].chBitmapImageName, "rb");
		if(fp == NULL)
		{
			printf("Can not open file %s\n", s_stTEST_MpegSrc_Field[iMfdIndex].chBitmapImageName);
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}

		fseek( fp, 18, SEEK_SET );
		fread( &ulWidth, 4, 1, fp );
		fseek( fp, 22, SEEK_SET );
		fread( &ulHeight, 4, 1, fp );
		fseek( fp, 10, SEEK_SET );

		if(pContext->eSrcOrientation != BFMT_Orientation_e2D)
		{
			FILE        *fp_R;
			uint32_t     ulWidth_R, ulHeight_R;

			printf("Enter file name for right buffer: ");
			BTST_P_Scanf(pContext, "%s", s_stTEST_MpegSrc_Field[iMfdIndex].chBitmapImageName_R);

			fp_R = fopen(s_stTEST_MpegSrc_Field[iMfdIndex].chBitmapImageName_R, "rb");
			if(fp_R == NULL)
			{
				printf("Can not open file %s\n", s_stTEST_MpegSrc_Field[iMfdIndex].chBitmapImageName_R);
				return BERR_TRACE(BERR_INVALID_PARAMETER);
			}

			fseek( fp_R, 18, SEEK_SET );
			fread( &ulWidth_R, 4, 1, fp_R );
			fseek( fp_R, 22, SEEK_SET );
			fread( &ulHeight_R, 4, 1, fp_R );
			fseek( fp_R, 10, SEEK_SET );

			if((ulWidth != ulWidth_R) || (ulHeight != ulHeight_R))
			{
				printf("Left buffer (W=%d, H=%d) and right buffer (W=%d, H=%d) size is different\n",
					ulWidth, ulHeight, ulWidth_R, ulHeight_R);
			}
			fclose(fp_R);
		}

		printf("Bitmap ulWidth: %d, ulHeight: %d\n", ulWidth, ulHeight);

		fclose(fp);
	}


	s_stTEST_MpegSrc_Field[iMfdIndex].bInterlaced = pMfdFmtInfo->bInterlaced;
	s_stTEST_MpegSrc_Field[iMfdIndex].ePolarity = pMfdFmtInfo->bInterlaced
		? BAVC_Polarity_eTopField : BAVC_Polarity_eFrame;

	/* currently supported formats:
	 *  BFMT_VideoFmt_eNTSC, BFMT_VideoFmt_e480p, BFMT_VideoFmt_e720p,
	 *  BFMT_VideoFmt_e1080i, BFMT_VideoFmt_e1080p
	 */
	if((eMfdFormat == BFMT_VideoFmt_eNTSC) ||
	   (eMfdFormat == BFMT_VideoFmt_e480p))
		bIsHd = false;
	else
		bIsHd = true;


	/* Stripe width */
	err = BCHP_GetFeature(pContext->hChp, BCHP_Feature_eMemCtrl0DramWidthCount, &ulDataWidth);
	if(err == BERR_SUCCESS)
	{
	    if(ulDataWidth == 16)
			s_stTEST_MpegSrc_Field[iMfdIndex].ulStripeWidth = STRIPE_WIDTH_64;
	    else
			s_stTEST_MpegSrc_Field[iMfdIndex].ulStripeWidth = STRIPE_WIDTH_128;
	}
	else
		s_stTEST_MpegSrc_Field[iMfdIndex].ulStripeWidth = STRIPE_WIDTH_64;

	/* intialize AVC structure for this feeder */
	s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.eInterruptPolarity = BAVC_Polarity_eTopField;

	s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.bMute = (uiPattern == 0);
	s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.pLuminanceFrameBufferAddress   = NULL;
	s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.pChrominanceFrameBufferAddress = NULL;
	s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.eChrominanceInterpolationMode = BAVC_InterpolationMode_eFrame;
	s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.eMpegType  = BAVC_MpegType_eMpeg2;
	s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.eYCbCrType = BAVC_YCbCrType_e4_2_0 + pContext->b422;	/* REDO!! */
	s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.eAspectRatio   = pMfdFmtInfo->eAspectRatio;
	s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.eFrameRateCode = BAVC_FrameRateCode_e29_97;
	s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.ulSourceHorizontalSize  = ulWidth;
	s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.ulSourceVerticalSize    = ulHeight;
	if(pMfdFmtInfo->bInterlaced)
		s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.eSourcePolarity = BAVC_Polarity_eTopField;
	else
		s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.eSourcePolarity = BAVC_Polarity_eFrame;
	s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.eMatrixCoefficients     =
		bIsHd ? BAVC_MatrixCoefficients_eItu_R_BT_709 : BAVC_MatrixCoefficients_eSmpte_170M;
	s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.eTransferCharacteristics =
		bIsHd ? BAVC_TransferCharacteristics_eItu_R_BT_709 : BAVC_TransferCharacteristics_eSmpte_170M;
	s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.eColorPrimaries =
		bIsHd ? BAVC_ColorPrimaries_eItu_R_BT_709 : BAVC_ColorPrimaries_eSmpte_170M;

	/* TEST_MpegSrc_FrameBuffer */
	s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.pucLumaAddr       = NULL;
	s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.pucChromaAddr     = NULL;
	s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.uiLumaRowStride   = 0;
	s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.uiChromaRowStride = 0;
	s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.ulFrames          = uiFrames;
	s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.ulLumaFrameOffset = 0;
	s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.ulChromaFrameOffset = 0;
	s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.bDumpImage        = false;

	s_stTEST_MpegSrc_Field[iMfdIndex].uiFrameCount = 0;
	s_stTEST_MpegSrc_Field[iMfdIndex].uiFrameIndex = 0;
	s_stTEST_MpegSrc_Field[iMfdIndex].hSource      = NULL;
	s_stTEST_MpegSrc_Field[iMfdIndex].ulMuteColor  = 0x108080;

	/* set source handle for frame buffer */
	s_stTEST_MpegSrc_Field[iMfdIndex].hSource = hMfdSource;
	s_stTEST_MpegSrc_Field[iMfdIndex].iMfdIndex = iMfdIndex;

	s_stTEST_MpegSrc_Field[iMfdIndex].bEnableIntercept = pContext->bIntercept;
	s_stTEST_MpegSrc_Field[iMfdIndex].bEnablePanScan = bEnablePanScan;
	/* default: display size is same as display format (no pan scan) */
	if (bEnablePanScan)
	{
		s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.ulDisplayHorizontalSize = ulWidth/2;
		s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.ulDisplayVerticalSize   = ulHeight/2;
	}
	else
	{
		s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.ulDisplayHorizontalSize = ulWidth;
		s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.ulDisplayVerticalSize   = ulHeight;
	}

	/* source buffer size */
	s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.ulSourceHorizontalSize = ulWidth;
	s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.ulSourceVerticalSize = ulHeight;

	s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.ulLumaRangeRemapping = 8;
	s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.ulChromaRangeRemapping = 8;

	if( s_stTEST_MpegSrc_Field[iMfdIndex].ulStripeWidth == STRIPE_WIDTH_128)
	{
		s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.eStripeWidth = BAVC_StripeWidth_e128Byte;
	}
	else
	{
		s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.eStripeWidth = BAVC_StripeWidth_e64Byte;
	}

	s_stTEST_MpegSrc_Field[iMfdIndex].bXvd = IS_XVD(pContext->eSelectedMDec);
	s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.eOrientation = pContext->eSrcOrientation;

    /* static buffer heap follows the decoder heap */
	s_stTEST_MpegSrc_Field[iMfdIndex].hHeap = pContext->astMDecInfo[pContext->eSelectedMDec].hHeap;

	if(pContext->eSrcOrientation != BFMT_Orientation_e2D)
	{
		BAVC_XVD_Picture   *pEnhanced;

		pEnhanced = (BAVC_XVD_Picture *)BKNI_Malloc(sizeof(BAVC_XVD_Picture));
		if(!pEnhanced)
		{
			BDBG_ERR(("TEST_XVD_CreateFrameBuffers failed for pEnhanced!"));
			return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		}

		*pEnhanced = s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field;
		pEnhanced->pEnhanced = NULL;
		bLeftBuffer = false;
		if((pContext->eSrcOrientation == BFMT_Orientation_e3D_Left) ||
		   (pContext->eSrcOrientation == BFMT_Orientation_eLeftRight_Enhanced) ||
		   (pContext->eSrcOrientation == BFMT_Orientation_e3D_Right))
		{
			if(pContext->eSrcOrientation == BFMT_Orientation_e3D_Left)
			{
				pEnhanced->eOrientation = BFMT_Orientation_e3D_Right;
			}
			else if(pContext->eSrcOrientation == BFMT_Orientation_e3D_Right)
			{
				pEnhanced->eOrientation = BFMT_Orientation_e3D_Left;
				bLeftBuffer = true;
			}
			else if(pContext->eSrcOrientation == BFMT_Orientation_eLeftRight_Enhanced)
			{
				pEnhanced->eOrientation = BFMT_Orientation_eLeftRight_Enhanced;
			}

			/* Create horizontal color bar for right buffer */
#if (BTST_P_SUPPORT_DMA) 
			if( TEST_MFD_CreateFrameBuffers(
				pContext->eSelectedMDec,
				&s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer,
				(TEST_SurfacePattern)uiPattern,
				pContext->astMemInfo[0].hMem,
				s_stTEST_MpegSrc_Field[iMfdIndex].ulStripeWidth,
				ulWidth, ulHeight, bIsHd,
				uiFrames, &ulLuminanceNMBY, &ulChrominanceNMBY, pContext->b422,
				bLeftBuffer) != 0 )
			{
				BDBG_ERR(("TEST_XVD_CreateFrameBuffers failed!"));
			}

			if(s_stTEST_MpegSrc_Field[iMfdIndex].hHeap != pContext->astMemInfo[0].hMem)
			{
				if( TEST_Mpeg_CreateDmaFrameBuffers(
					&s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer,
					s_stTEST_MpegSrc_Field[iMfdIndex].hHeap,
					g_frmInfo.hChp, g_frmInfo.hReg, g_frmInfo.hMem, g_frmInfo.hInt) != 0 )
				{
					BDBG_ERR(("TEST_XVD_CreateDmaFrameBuffers failed!"));
				}
				pEnhanced->pChrominanceFrameBufferAddress = s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.pucDmaMemChromaAddr;
				pEnhanced->pLuminanceFrameBufferAddress = s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.pucDmaMemLumaAddr;

				/* release the intermediate buffers right away once consumed */
				BMEM_Free(pContext->astMemInfo[0].hMem, s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.pucChromaAddr);
				BMEM_Free(pContext->astMemInfo[0].hMem, s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.pucLumaAddr);
			}
			else
#else /* 3D chips should support CPU access to all MEMCs */
			if( TEST_MFD_CreateFrameBuffers(
				pContext->eSelectedMDec,
				&s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer,
				(TEST_SurfacePattern)uiPattern,
				s_stTEST_MpegSrc_Field[iMfdIndex].hHeap,
				s_stTEST_MpegSrc_Field[iMfdIndex].ulStripeWidth,
				ulWidth, ulHeight, bIsHd,
				uiFrames, &ulLuminanceNMBY, &ulChrominanceNMBY, pContext->b422,
				bLeftBuffer) != 0 )
			{
				BDBG_ERR(("TEST_XVD_CreateFrameBuffers failed!"));
			}
#endif
			{
				pEnhanced->pChrominanceFrameBufferAddress = s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.pucChromaAddr;
				pEnhanced->pLuminanceFrameBufferAddress = s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.pucLumaAddr;
			}
			s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.pEnhanced = pEnhanced;

			if((pContext->eSrcOrientation == BFMT_Orientation_eLeftRight_Enhanced) &&
			   (pEnhanced->eOrientation == BFMT_Orientation_eLeftRight_Enhanced))
			{
				s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.eOrientation = BFMT_Orientation_e3D_LeftRight;
			}
		}
		else
		{
			s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.pEnhanced = NULL;

		}

		bLeftBuffer = !bLeftBuffer;
	}

	/* create all the frame buffers */
#if (BTST_P_SUPPORT_DMA)
	if( TEST_MFD_CreateFrameBuffers(
		pContext->eSelectedMDec,
		&s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer,
		(TEST_SurfacePattern)uiPattern,
		pContext->astMemInfo[0].hMem,
		s_stTEST_MpegSrc_Field[iMfdIndex].ulStripeWidth,
		ulWidth, ulHeight, bIsHd,
		uiFrames, &ulLuminanceNMBY, &ulChrominanceNMBY, pContext->b422,
		bLeftBuffer) != 0 )
	{
		BDBG_ERR(("TEST_XVD_CreateFrameBuffers failed!"));
	}

	/* Check if need to create DMA buffers */

	/* Usage:
	 * Need to use DMA buffers for chips has separate XVD frame
	 * buffer memory (7400, 7405 non-UMA mode) and CPU only can access MEMC_0
	 *
	 * CPU can only access MEMC_0. Static buffer images are
	 * created in MEMC_0.
	 * By default, MFD_0 and MFD_1 are blocked out on MEMC_0.
	 *
	 * 7400:
	 *    MEMC_1: decoder 0 + MFD_0.
	 *    MEMC_2: decoder 1 + MFD_1.
	 * 7400 A0: MFD_0 and MFD_1 has access to all MEMC_x. Can
	 *          adjust RTS for MFD_0 and MFD_1 to make static
	 *          buffer working.
	 * 7400 B0: MFD_0 and MFD_1 only access to MEMC_0 when
	 *          IMAGE_FORMAT is PACKED. Display read access
	 *          (IMAGE_FORMAT is AVC_MPEG) to MEMC_0 is removed.
	 *          Need to DMA the image from MEMC_0 to MEMC_1/MEMC_2
	 *          to make static buffer working.
	 *
	 * 7405
	 *    UMA:
	 *          MEMC_0: decoder 0 + MFD_0 + MFD_1
	 *          Do not need DMA buffers
	 *    non-UMA:
	 *          MEMC_1: decoder 0 + MFD_0 + MFD_1
	 *          Need to DMA the image from MEMC_0 to MEMC_1
	 *
	 */
	if(s_stTEST_MpegSrc_Field[iMfdIndex].hHeap != pContext->astMemInfo[0].hMem)
	{
		if( TEST_Mpeg_CreateDmaFrameBuffers(
			&s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer,
			s_stTEST_MpegSrc_Field[iMfdIndex].hHeap,
			g_frmInfo.hChp, g_frmInfo.hReg, g_frmInfo.hMem, g_frmInfo.hInt) != 0 )
		{
			BDBG_ERR(("TEST_XVD_CreateDmaFrameBuffers failed!"));
		}
		/* release the intermediate buffers right away once consumed */
		BMEM_Free(pContext->astMemInfo[0].hMem, s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.pucChromaAddr);
		BMEM_Free(pContext->astMemInfo[0].hMem, s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.pucLumaAddr);
	}
	else
#else
	if( TEST_MFD_CreateFrameBuffers(
		pContext->eSelectedMDec,
		&s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer,
		(TEST_SurfacePattern)uiPattern,
		s_stTEST_MpegSrc_Field[iMfdIndex].hHeap,
		s_stTEST_MpegSrc_Field[iMfdIndex].ulStripeWidth,
		ulWidth, ulHeight, bIsHd,
		uiFrames, &ulLuminanceNMBY, &ulChrominanceNMBY, pContext->b422,
		bLeftBuffer) != 0 )
	{
		BDBG_ERR(("TEST_XVD_CreateFrameBuffers failed!"));
	}
#endif
	{
		s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.pucDmaMemChromaAddr = NULL;
		s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.pucDmaMemLumaAddr   = NULL;
	}

	/* Luma and Chroma NMBY */
	s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.ulLuminanceNMBY = ulLuminanceNMBY;
	s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.ulChrominanceNMBY = ulChrominanceNMBY;

	/*===============================================================
	   register and enable a callback function for the mpeg src to feed data.
	   Query the interrupt name for the mpeg source and use it to
	   register the callback.
	   NOTE:
		- For interlaced picture, BVDC_g_GetInterruptName gets the interrupt
		  corresponding to the RDC Done slot interrupt id for the next field, not
		  the current field.
	*/
	/* top field */
	TestError( BVDC_Source_GetInterruptName(
		hMfdSource,
		BAVC_Polarity_eTopField, /* top field */
		&InterruptName),
		"ERROR: BVDC_Source_GetInterruptName()" );

	TestError( BINT_CreateCallback(
		&s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eTopField],
		pContext->hInt,
		InterruptName,           /* use the queried int name */
		TEST_MpegSrc_Callback,      /* the registered callback */
		&s_stTEST_MpegSrc_Field[iMfdIndex],
		BAVC_Polarity_eTopField),   /* top field */
		"ERROR: BINT_CreateCallback()" );

	TestError( BINT_EnableCallback( s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eTopField] ),
		"ERROR: BINT_EnableCallback" );

	/* bottom field */
	TestError( BVDC_Source_GetInterruptName(
		hMfdSource,
		BAVC_Polarity_eBotField,    /* bottom field */
		&InterruptName),
		"ERROR: BVDC_Source_GetInterruptName()" );

	TestError( BINT_CreateCallback(
		&s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eBotField],
		pContext->hInt,
		InterruptName,           /* use the queried int name */
		TEST_MpegSrc_Callback,      /* the registered callback */
		&s_stTEST_MpegSrc_Field[iMfdIndex],
		BAVC_Polarity_eBotField),   /* bottom field */
		"ERROR: BINT_CreateCallback()" );

	TestError( BINT_EnableCallback( s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eBotField] ),
		"ERROR: BINT_EnableCallback" );

	/* progressive source */
	TestError( BVDC_Source_GetInterruptName(
		hMfdSource,
		BAVC_Polarity_eFrame,  /* progressive frame */
		&InterruptName),
		"ERROR: BVDC_Source_GetInterruptName()" );

	TestError( BINT_CreateCallback(
		&s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eFrame],
		pContext->hInt,
		InterruptName,           /* use the queried int name */
		TEST_MpegSrc_Callback,      /* the registered callback */
		&s_stTEST_MpegSrc_Field[iMfdIndex],
		BAVC_Polarity_eFrame),   /* progressive frame */
		"ERROR: BINT_CreateCallback()" );

	TestError( BINT_EnableCallback( s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eFrame] ),
		"ERROR: BINT_EnableCallback" );

Done:
	return err;
}

/*
 * Terminate Mpeg src data routine
 */
void TerminateMpegStaticSrc(
	BTST_P_Context	*pContext)
{
	BMEM_Handle hMem;
	int			iMfdIndex = pContext->iSelectedVidSrc;

	hMem = s_stTEST_MpegSrc_Field[iMfdIndex].hHeap;
#if (BTST_P_SUPPORT_DMA)
	/* if DMA buffer exists, release it */
	if(s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.pucDmaMemLumaAddr)
	{
		BMEM_Free(
			hMem,
			s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.pucDmaMemLumaAddr);
	}

	if(s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.pucDmaMemChromaAddr)
	{
		BMEM_Free(
			hMem,
			s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.pucDmaMemChromaAddr);
	}
#else
	/* free static frame buffer */
	BMEM_Free(hMem,
		s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.pucLumaAddr);
	BMEM_Free(hMem,
		s_stTEST_MpegSrc_Field[iMfdIndex].stFrameBuffer.pucChromaAddr);
#endif


	/* if 3D enhanced buffer exists, release it */
	if(s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.pEnhanced)
	{
		BAVC_XVD_Picture   *pEnhanced = s_stTEST_MpegSrc_Field[iMfdIndex].stMFD_Field.pEnhanced;

		BMEM_Free(
			hMem,
			pEnhanced->pLuminanceFrameBufferAddress);
		BMEM_Free(
			hMem,
			pEnhanced->pChrominanceFrameBufferAddress);
		BKNI_Free(pEnhanced);
	}

	/* shutdown top field or frame callbacks? */
	if(s_hMpegSrcCallback[iMfdIndex][0])
	{
		/* top field or frame */
		BINT_DisableCallback(s_hMpegSrcCallback[iMfdIndex][0]);
		BINT_DestroyCallback(s_hMpegSrcCallback[iMfdIndex][0]);
		s_hMpegSrcCallback[iMfdIndex][0] = NULL;
	}

	/* shutdown bottom field callback? */
	if(s_hMpegSrcCallback[iMfdIndex][1])
	{
		/* bottom field */
		BINT_DisableCallback(s_hMpegSrcCallback[iMfdIndex][1]);
		BINT_DestroyCallback(s_hMpegSrcCallback[iMfdIndex][1]);
		s_hMpegSrcCallback[iMfdIndex][1] = NULL;
	}

	/* shutdown frame callback? */
	if(s_hMpegSrcCallback[iMfdIndex][2])
	{
		/* bottom field */
		BINT_DisableCallback(s_hMpegSrcCallback[iMfdIndex][2]);
		BINT_DestroyCallback(s_hMpegSrcCallback[iMfdIndex][2]);
		s_hMpegSrcCallback[iMfdIndex][2] = NULL;
	}
}


/* end of file */
