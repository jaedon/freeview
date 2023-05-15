/***************************************************************************
 *     Copyright (c) 2002-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: intercept.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 10/16/07 2:45p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/intercept.c $
 * 
 * Hydra_Software_Devel/5   10/16/07 2:45p rpan
 * PR34857: First version after merging the latest xvd_vdc_test and
 * mvd_vdc_test as of Oct. 16, 2007, 12pm.
 * 
 * 
 * Hydra_Software_Devel/1   9/27/07 4:12p rpan
 * PR34857: Initial revision. Copied over from mvd_vdc_test.
 *
 ***************************************************************************/

#include "stdio.h"

#include "bstd.h"           /* standard types */
#include "berr.h"           /* error code */
#include "bkni.h"           /* kernel interface */
#include "bavc.h"
#include "interactive.h"
#include "test_util.h"
#include "mpg_decoder.h"

BDBG_MODULE(vdc_test);

#if (BCHP_CHIP == 3563) 

/* allocate uiFrames of 1920 X 1088 HD frame buffer for interception of MVD
   frame buffer data. This is necessary since MVD organizes frame buffer in stacked form.
   to reduce the overhead of callback handling of the frame interception, we just
   allocate full 4-frame buffer here and mem copy the whole mvd frame store into our buffer.*/
BERR_Code AllocateInterceptFrameBuffer(
	uint32_t       uiFrames,      /* number of frame stores */
	bool           b422,
	uint32_t      *pulLumaBufSize,
	void         **ppvLumaStartAddress,
	uint32_t      *pulChromaBufSize,
	void         **ppvChromaStartAddress
	)
{
	uint32_t  ulLumaBufSize;
	void     *pvLumaStartAddress;
	uint32_t  ulChromaBufSize;
	void     *pvChromaStartAddress;

	/* to calculate the favorable number of macroblocks(fnmbx) per row */
	uint32_t nmbx = ( 1920 * uiFrames + 15 ) >> 4;
	uint32_t n = (nmbx / 8 + 1 ) / 2 ;
	uint32_t fnmbx = (( n << 1 ) + 1) << 3;

	uint32_t uiRowStride = fnmbx << 8;/* uiRowStride contains all frames' row and waste MB's */
	uint32_t totalRows = (1088 + 15) / 16;

	/* reserve enough buffer space for MPEG frame buffer interception */
	ulLumaBufSize = uiRowStride * totalRows + 2* sizeof(BAVC_MVD_Field);/* interleaved frames buffer */
	ulChromaBufSize = uiRowStride * ( (totalRows+1) >> (!b422) ) + 256;/* interleaved frames buffer */

	BDBG_MSG(("Allocating intercept luma buffer size = %d", ulLumaBufSize));
	BDBG_MSG(("Allocating intercept chroma buffer size = %d", ulChromaBufSize));
	/* allocate intercept frame buffer to monitor frame forwarded by mvd to vdc. */
	if( (pvLumaStartAddress =
			BKNI_Malloc( ulLumaBufSize )  /* MB aligned */
		 ) == NULL )
	{
		BDBG_ERR(("ERROR: BKNI_Malloc out of system memory!"));
		return 1;
	}
	if( (pvChromaStartAddress =
			BKNI_Malloc( ulChromaBufSize )/* MB aligned */
		) == NULL )
	{
		BDBG_ERR(("ERROR: BKNI_Malloc out of system memory!"));
		return 1;
	}

	/* store returned values */
	*pulLumaBufSize = ulLumaBufSize;
	*ppvLumaStartAddress = pvLumaStartAddress;
	*pulChromaBufSize = ulChromaBufSize;
	*ppvChromaStartAddress = pvChromaStartAddress;

	return BERR_SUCCESS;
}

/* 4:2:0 image files
 * -----------------------------
 * luma.img:                         | chroma.img:
 *   y(0,0)   y(0,1)   ... y(0,w-1)  |   Cb(0,0)Cr(0,0)Cb(0,2)Cr(0,2) ...
 *   y(1,0)   y(1,1)   ... y(1,w-1)  |
 *   y(2,0)   y(2,1)   ... y(2,w-1)  |   Cb(2,0)Cr(2,0)Cb(2,2)Cr(2,2) ...
 *   y(3,0)   y(3,1)   ... y(3,w-1)  |
 *   ...                             |   ...
 *   y(h-2,0) y(h-2,1) ... y(h-2,w-1)|   Cb(h-2,0)Cr(h-2,0)...
 *   y(h-1,0) y(h-1,1) ... y(h-1,w-1)|
 *
 * YUV4:2:2 image file
 * -------------------------
 * yuv422.img:
 *   width
 *   height
 *   // line = 0
 *   y(0,0)u(0,0)
 *   y(0,1)v(0,0)
 *   y(0,2)u(0,2)
 *   y(0,3)v(0,2)
 *   .
 *   .
 *   .
 *   // line = 1
 *   y(1,0)u(1,0)
 *   y(1,1)v(1,0)
 *   .
 *   .
 *   .
 *   y(h-1,w-2)u(h-1,w-2)
 *   y(h-1,w-1)v(h-1,w-2)
 *
 * Note:
 *   1. h = image height; w = image width;
 *   2. both h and w must be even number.
 *   3. YUV422 image is merged from luma.img and chroma.img; 420 format chroma.img
 *      will derive 422 format chroma by vertical interpolation method.
 *   4. The yuv422 image file can be converted to RGB888 ppm format by yuv2ppm tool.
 */
void DumpInterceptFrameBuffer(
	void         *pvLumaStartAddress,
	void         *pvChromaStartAddress,
	bool          b422 )
{
	BAVC_MVD_Field *pFieldData = (BAVC_MVD_Field*)pvLumaStartAddress;
	uint32_t        uiAvcFieldSize = sizeof(BAVC_MVD_Field);
	uint32_t        row, mb, line, col, ii, jj, offsetLuma=0, offsetChroma=0;
	uint32_t        row_plus1, line_plus1, row_m1, line_m1, offsetChroma_plus1;
	FILE           *fpLuma = fopen("luma.img", "w");
	FILE           *fpChroma = fopen("chroma.img", "w");
	FILE           *fpYuv = fopen("yuv422.img", "w");

	BDBG_MSG(("Start dumping luma and chroma image of the intercepted frame..."));
	BDBG_MSG(("\tFrame Width  = %d", pFieldData->ulSourceHorizontalSize));
	BDBG_MSG(("\tFrame Height = %d", pFieldData->ulSourceVerticalSize));
	BDBG_ASSERT((pFieldData->ulSourceHorizontalSize % 2) == 0);
	BDBG_ASSERT((pFieldData->ulSourceVerticalSize % 2) == 0);

	/* write YUV422 image's header */
	fprintf(fpYuv, "%x\n", pFieldData->ulSourceHorizontalSize);
	fprintf(fpYuv, "%x\n", pFieldData->ulSourceVerticalSize);

	for( ii = 0; ii < pFieldData->ulSourceVerticalSize; ii++ )
	{/* for each line of the frame */
		fprintf(fpYuv, "// line = %d\n", ii);

		row = ii / 16;/* row of MB's within a frame */
		line = ii % 16;/* line within a MB */
		row_plus1 = (ii + 1) / 16;/* row of MB of next line */
		line_plus1 = (ii + 1) % 16;/* next line within a MB */
		row_m1 = (ii - 1) / 16;
		line_m1 = (ii - 1) % 16;

		for( jj = 0; jj < pFieldData->ulSourceHorizontalSize; jj++ )
		{/* for each pixel of the line */
			mb = jj / 16;/* MB index within a row */
			col = jj % 16;/* colomn within a MB */

			/* find the luma data offset */
			offsetLuma = uiAvcFieldSize +             /* exclude the AVC field header */
				row * pFieldData->ulRowStride +   /* big row offset */
				(mb << 8) +                       /* MB offset */
				(line << 4) +                     /* line offset */
				col;                              /* colomn offset */

			if( col == 0 )
			{
				fprintf(fpLuma, " ");
			}
			fprintf(fpLuma, "%02x", *(uint8_t*)((uint32_t)pvLumaStartAddress + Y_OFFSET(offsetLuma)));/* Y */

			/* 422 */
			if(b422)
			{
				if( !(col & 1) ) 
				{/* if even colomn */
					if( !(col & 3) )/* luma colomn 0, 4, 8, 12, will be the same chroma colomn */
					{/* first half of the chroma pair */
						offsetChroma = (row) * pFieldData->ulRowStride + /* row offset */
								(mb << 8) +                           /* MB offset */
								(line << 4) +                         /* line offset */
								col;                                  /* colomn offset */
					}else/* luma colomn 2, 6, 10, 14 will be chroma colomn 1, 5, 9, 13 */
					{/* second half of the chroma pair */
						offsetChroma = (row) * pFieldData->ulRowStride + /* row offset */
								(mb << 8) +                           /* MB offset */
								(line << 4) +                         /* line offset */
								col - 1;                              /* colomn offset */
					}

					if( col == 0 )
					{
						fprintf(fpChroma, " ");
					}

					fprintf(fpChroma, "%02x%02x",
						*(uint8_t*)((uint32_t)pvChromaStartAddress + CB_OFFSET(offsetChroma)), /* U/Cb */
						*(uint8_t*)((uint32_t)pvChromaStartAddress + CR_OFFSET(offsetChroma)));/* V/Cr */

					fprintf(fpYuv, "%2x%02x\n",
						*(uint8_t*)((uint32_t)pvLumaStartAddress + Y_OFFSET(offsetLuma)),      /* y0 */
						*(uint8_t*)((uint32_t)pvChromaStartAddress + CB_OFFSET(offsetChroma)));/* u0 */
					fprintf(fpYuv, "%2x%02x\n",
						*(uint8_t*)((uint32_t)pvLumaStartAddress + Y_OFFSET(offsetLuma + 1)),  /* y1 */
						*(uint8_t*)((uint32_t)pvChromaStartAddress + CR_OFFSET(offsetChroma)));/* v0 */
				}/* end of chroma if even line, even colomn */
			}
			else
			{
				/* MPEG 4:2:0 format chroma */
				if( (line % 2 == 0) && (col % 2 == 0) )
				{/* if even line, even colomn */
					if( !(col & 3) )/* luma colomn 0, 4, 8, 12, will be the same chroma colomn */
					{/* first half of the chroma pair */
						offsetChroma = (row >> 1) * pFieldData->ulRowStride + /* row offset */
								(mb << 8) +                           /* MB offset */
								(line << 3) +                         /* line offset */
								col;                                  /* colomn offset */
					}else/* luma colomn 2, 6, 10, 14 will be chroma colomn 1, 5, 9, 13 */
					{/* second half of the chroma pair */
						offsetChroma = (row >> 1) * pFieldData->ulRowStride + /* row offset */
								(mb << 8) +                           /* MB offset */
								(line << 3) +                         /* line offset */
								col - 1;                              /* colomn offset */
					}
					if(row & 1)
					{/* even luma row: top half 16X8 chroma MB;
						odd luma row: bottom half 16X8 chroma MB */
						offsetChroma += 128;
					}

					if( col == 0 )
					{
						fprintf(fpChroma, " ");
					}

					fprintf(fpChroma, "%02x%02x",
						*(uint8_t*)((uint32_t)pvChromaStartAddress + CB_OFFSET(offsetChroma)), /* U/Cb */
						*(uint8_t*)((uint32_t)pvChromaStartAddress + CR_OFFSET(offsetChroma)));/* V/Cr */

					fprintf(fpYuv, "%2x%02x\n",
						*(uint8_t*)((uint32_t)pvLumaStartAddress + Y_OFFSET(offsetLuma)),      /* y0 */
						*(uint8_t*)((uint32_t)pvChromaStartAddress + CB_OFFSET(offsetChroma)));/* u0 */
					fprintf(fpYuv, "%2x%02x\n",
						*(uint8_t*)((uint32_t)pvLumaStartAddress + Y_OFFSET(offsetLuma + 1)),  /* y1 */
						*(uint8_t*)((uint32_t)pvChromaStartAddress + CR_OFFSET(offsetChroma)));/* v0 */
				}/* end of chroma if even line, even colomn */
				else if( (line % 2 == 1) && (col % 2 == 0) )
				{/* if odd line and even colomn */
					/* recalculate the chroma address for the odd line */
					offsetChroma = (row_m1 >> 1) * pFieldData->ulRowStride + /* row offset */
							(mb << 8) +                                      /* MB offset */
							(line_m1 << 3) +                                 /* line offset */
							col - ((col & 3)? 1:0);                          /* colomn offset */
					
					if(ii == pFieldData->ulSourceVerticalSize - 1)
					{/* last odd line is approximated to the last even line */
						fprintf(fpYuv, "%2x%02x\n",
							*(uint8_t*)((uint32_t)pvLumaStartAddress + Y_OFFSET(offsetLuma)),      /* y0 */
							*(uint8_t*)((uint32_t)pvChromaStartAddress + CB_OFFSET(offsetChroma)));/* u0 */
						fprintf(fpYuv, "%2x%02x\n",
							*(uint8_t*)((uint32_t)pvLumaStartAddress + Y_OFFSET(offsetLuma + 1)),  /* y1 */
							*(uint8_t*)((uint32_t)pvChromaStartAddress + CR_OFFSET(offsetChroma)));/* v0 */
					}/* last odd line is approximated to the last even line */
					else
					{/* calculate the next line's chroma offset */
						if( !(col & 3) )/* luma colomn 0, 4, 8, 12, will be the same chroma colomn */
						{/* first half of the chroma pair */
							offsetChroma_plus1 = (row_plus1 >> 1) * pFieldData->ulRowStride + /* row offset */
									(mb << 8) +                           /* MB offset */
									(line_plus1 << 3) +                   /* line offset */
									col;                                  /* colomn offset */
						}else/* luma colomn 2, 6, 10, 14 will be chroma colomn 1, 5, 9, 13 */
						{/* second half of the chroma pair */
							offsetChroma_plus1 = (row_plus1 >> 1) * pFieldData->ulRowStride + /* row offset */
									(mb << 8) +                           /* MB offset */
									(line_plus1 << 3) +                   /* line offset */
									col - 1;                              /* colomn offset */
						}
						if(row_plus1 & 1)
						{/* even luma row: top half 16X8 chroma MB;
							odd luma row: bottom half 16X8 chroma MB */
							offsetChroma_plus1 += 128;
						}

						/* chroma interpolation */
						fprintf(fpYuv, "%2x%02x\n",
							*(uint8_t*)((uint32_t)pvLumaStartAddress + Y_OFFSET(offsetLuma)),                 /* y0 */
							(*(uint8_t*)((uint32_t)pvChromaStartAddress + CB_OFFSET(offsetChroma)) +
							 *(uint8_t*)((uint32_t)pvChromaStartAddress + CB_OFFSET(offsetChroma_plus1))) /2);/* (u0_minus1 + u0_plus1)/2 */
						fprintf(fpYuv, "%2x%02x\n",
							*(uint8_t*)((uint32_t)pvLumaStartAddress + Y_OFFSET(offsetLuma + 1)),             /* y1 */
							(*(uint8_t*)((uint32_t)pvChromaStartAddress + CR_OFFSET(offsetChroma)) +
							 *(uint8_t*)((uint32_t)pvChromaStartAddress + CR_OFFSET(offsetChroma_plus1))) / 2);/* v0 */
					}/* end of if not the last line */
				}/* end of chroma if odd line, even colomn */
			}
		}/* end of width for loop */
		fprintf(fpLuma, "\n");
		fprintf(fpChroma, "\n");
	}/* end of height for loop */

	BDBG_MSG(("End dumping."));
	fclose(fpLuma);
	fclose(fpChroma);
	fclose(fpYuv);

	/* free the interception frame buffer */
	BKNI_Free(pvLumaStartAddress);
	BKNI_Free(pvChromaStartAddress);
}
#else
BERR_Code AllocateInterceptFrameBuffer(
	uint32_t       uiFrames,      /* number of frame stores */
	bool           b422,
	uint32_t      *pulLumaBufSize,
	void         **ppvLumaStartAddress,
	uint32_t      *pulChromaBufSize,
	void         **ppvChromaStartAddress
	)
{
	BSTD_UNUSED(uiFrames);      
	BSTD_UNUSED(b422);
	BSTD_UNUSED(pulLumaBufSize);
	BSTD_UNUSED(ppvLumaStartAddress);
	BSTD_UNUSED(pulChromaBufSize);
	BSTD_UNUSED(ppvChromaStartAddress);

	printf("\n\n ***** Frame buffer interception is not yet implemented in vdc_test for %d ****** \n\n", BCHP_CHIP);

	/* TODO: Add support for chips other than 3563 */
	return -1;
}

void DumpInterceptFrameBuffer(
	void         *pvLumaStartAddress,
	void         *pvChromaStartAddress,
	bool          b422 )
{
	BSTD_UNUSED(pvLumaStartAddress);
	BSTD_UNUSED(pvChromaStartAddress);
	BSTD_UNUSED(b422); 

	printf("\n\n ***** Frame buffer interception is not yet implemented in vdc_test for %d ****** \n\n", BCHP_CHIP);

	/* TODO: Add support for chips other than 3563 */
}

#endif 
/* end of file */
