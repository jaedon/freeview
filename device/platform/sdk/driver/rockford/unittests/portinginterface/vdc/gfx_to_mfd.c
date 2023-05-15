/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: gfx_to_mfd.c $
 * $brcm_Revision: Hydra_Software_Devel/16 $
 * $brcm_Date: 9/25/12 5:36p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/gfx_to_mfd.c $
 * 
 * Hydra_Software_Devel/16   9/25/12 5:36p vanessah
 * SW7425-2501: vdc_test clean up
 * 
 * Hydra_Software_Devel/15   2/14/12 11:32a yuxiaz
 * SW7425-2181: Clear out BAVC_MFD_Picture structure for gfxmfd.
 * 
 * Hydra_Software_Devel/14   2/9/12 4:50p yuxiaz
 * SW7425-2181: Added test case in vdc_test
 * 
 * Hydra_Software_Devel/13   2/9/12 10:07a yuxiaz
 * SW7425-2181: Fixed gfxmfd in vdc_test.
 * 
 * Hydra_Software_Devel/12   8/3/11 12:40p tdo
 * SW7425-1043: vdc_test: gfxmfd does not work and give pink screen.
 *
 * Hydra_Software_Devel/11   4/20/11 3:38p tdo
 * SW7425-365: Remove compiler warnings with new kernel build
 *
 * Hydra_Software_Devel/10   11/5/10 2:44p yuxiaz
 * SW7422-33, SW7422-77: Added 3D support in static buffer test.
 *
 * Hydra_Software_Devel/9   12/15/09 5:48p yuxiaz
 * SW7408-13: Get vdc_test compile for 7408. Clean up compile flags.
 *
 * Hydra_Software_Devel/8   8/3/09 10:23a syang
 * PR 55812: add 7550 support
 *
 * Hydra_Software_Devel/7   4/6/09 2:57p yuxiaz
 * PR46252: Need to expose 444 format in static buffers case.
 *
 * Hydra_Software_Devel/6   2/13/09 4:19p tdo
 * PR48866: Bringup gfxmfd in vdc_test for MFD on MEMC1
 *
 * Hydra_Software_Devel/5   10/1/08 4:55p yuxiaz
 * PR47303: Add BSUR API to Support Gfx Compression Usage
 *
 * Hydra_Software_Devel/4   6/10/08 2:03p syang
 * PR 39209:  use gfxmfd to test lBOX with 1080p input
 *
 * Hydra_Software_Devel/3   10/9/07 5:19p rpan
 * PR34857: Merged static buffer code for mvd and xvd_vdc_test.
 *
 * Hydra_Software_Devel/2   9/21/07 1:57p rpan
 * PR34857: Re-organized vdc_test source code.
 *
 * Hydra_Software_Devel/1   9/17/07 4:39p rpan
 * PR34857: Initial revision. Ported from xvd_vdc_test (09/17/2007).
 *
 *
 ***************************************************************************/

#include <math.h>           /* for ceil */

/* base modules */
#include "bstd.h"           /* standard types */
#include "berr.h"           /* error code */
#include "bkni.h"           /* kernel interface */

/* test include */
#include "config.h"
#include "test_util.h"
#include "commands.h"
#include "mpg_decoder.h"

#if (BTST_P_SUPPORT_DMA)
#include "bdma.h"
#endif

BDBG_MODULE(static_buffer);

typedef enum TEST_SurfacePattern
{
	TEST_SurfacePattern_eFixedColor = 0,
	TEST_SurfacePattern_eColorBar,
	TEST_SurfacePattern_eCheckBoard,
	TEST_SurfacePattern_eCrossHatch,
	TEST_SurfacePattern_eHLumaRamp,
	TEST_SurfacePattern_eVLumaRamp
} TEST_SurfacePattern;


typedef struct TEST_MFD_FrameBuffer
{
	BAVC_Polarity             ePolarity;
	BSUR_Surface_Handle       hSurface;
} TEST_MFD_FrameBuffer;

typedef struct TEST_MFD_SrcFields
{
	BAVC_MFD_Picture          AvcMfdPic;     /* avc struct sent to BVDC */
	TEST_MFD_FrameBuffer      aFrmBuf[4*2];  /* top and bot, 4 matches the max# in interactive.c */
	uint32_t                  uiNumFrames;   /* number of frames */
	uint32_t                  uiCurFrm;      /* current frame index */
	bool                      bFldSur;       /* sur is fld or frame */
	BVDC_Source_Handle        hSource;       /* src handle for this mfd */
	TEST_PanVecGen_Handle     hPanVecGen;    /* pan vector generator handle */
	uint32_t                  ulGfx2Mfd;     /* Use Gfx2Mfd setting */
} TEST_MFD_SrcFields;

/* static buffer structures */
static TEST_MFD_SrcFields     s_TEST_MfdSrcFields[BTST_P_MAX_MFD];
static BINT_CallbackHandle    s_hMpegSrcCallback[BTST_P_MAX_MFD][3];

/* global variables */
BPXL_Format                   g_ePxlFmt;

#if (BTST_P_SUPPORT_DMA)
static BERR_Code TEST_Gfx2Mfd_DmaFrameBuffers(
	BCHP_Handle      hChp,
	BREG_Handle      hReg,
	BMEM_Handle      hMem,
	BINT_Handle      hInt,
	BMEM_Handle      hSrcMem,
	BSUR_Surface_Handle hSrcSur,
	BMEM_Handle      hDstMem,
	BSUR_Surface_Handle hDstSur )
{
	BERR_Code eResult = BERR_SUCCESS;
	BDMA_Handle  hDma;
	BDMA_Mem_Handle  hMemDma;
	BDMA_Mem_Tran_Handle  hTran;
	uint32_t  ulSrcOffset, ulDstOffset;
	BDMA_TranStatus  eTranStatus;
	unsigned char   *pBufSrc;
	unsigned char   *pBufDst;
	uint32_t         ulSurPitch;
	uint32_t         ulNumBytes;
	uint32_t         ulWidth, ulHeight;

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

	BSUR_Surface_GetAddress( hSrcSur, (void *) (&pBufSrc), &ulSurPitch );
	BSUR_Surface_GetAddress( hDstSur, (void *) (&pBufDst), &ulSurPitch );
	BSUR_Surface_GetDimensions(hSrcSur, &ulWidth, &ulHeight);
	ulNumBytes = ulWidth * ulSurPitch;

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

/*
 * the utility function to create and fill surface with predefined pattern
 * Return: 0 - succeed; 1 - fail
 */
static uint32_t TEST_Gfx2Mfd_MakeSurfaces(
	BTST_P_Context           *pContext,
	TEST_MFD_SrcFields       *pTstMpegSrcFld,
	BMEM_Handle               hMem,
	TEST_SurfacePattern       ePattern,     /* frame buffer data pattern */
	uint32_t                  uiWidth,      /* picture width */
	uint32_t                  uiHeight,     /* picture height */
	uint32_t                  uiWhiteWidthPcnt,    /* white box width percent */
	uint32_t                  uiWhiteHeightPcnt,   /* white box height percent */
	bool                      bFldSur)      /* sur is fld or frame */
{
	uint32_t ii, jj;
	uint8_t  Y, Cb, Cr;
	uint32_t uiFrmId;          /* frame index */
	uint32_t ulSurPitch;
	uint8_t *pvSurStartAddr;
	uint32_t  ulSurHeight, ulNumFrms;
	uint32_t  uiWhiteTop, uiWhiteBot;
	uint32_t  uiWhiteLft, uiWhiteRgt;
	BERR_Code    eResult;
#if (BTST_P_SUPPORT_DMA)
	BSUR_Surface_Handle hTmpSurface[4*2];
#endif

	BDBG_ENTER(TEST_Gfx2Mfd_MakeSurfaces);

	/* Note: field surface only has half of frame height */
	ulSurHeight = (bFldSur)? (uiHeight / 2) : uiHeight;
	ulNumFrms   = pTstMpegSrcFld->uiNumFrames;

	/* create surfaces */
	for (uiFrmId = 0; uiFrmId < ulNumFrms; uiFrmId++ )
	{
		eResult = BSUR_Surface_Create(hMem, uiWidth, ulSurHeight, 0, NULL, g_ePxlFmt, NULL, 0,
				NULL, &pTstMpegSrcFld->aFrmBuf[uiFrmId].hSurface);
#if (BTST_P_SUPPORT_DMA)
		if(hMem != pContext->astMemInfo[0].hMem)
		{
			eResult = BSUR_Surface_Create(pContext->astMemInfo[0].hMem, uiWidth, ulSurHeight, 0, NULL, g_ePxlFmt, NULL, 0,
					NULL, &hTmpSurface[uiFrmId]);
		}
#endif
		if (eResult != BERR_SUCCESS)
		{
			BDBG_ERR(("ERROR: BMEM_AllocAligned out of heap memory!"));
			return 1;
		}

		if (bFldSur)
			pTstMpegSrcFld->aFrmBuf[uiFrmId].ePolarity = (uiFrmId & 0x1)?
				BAVC_Polarity_eBotField: BAVC_Polarity_eTopField;
		else
			pTstMpegSrcFld->aFrmBuf[uiFrmId].ePolarity = BAVC_Polarity_eFrame;
	}

	/* fill in each line of the frame */
	uiWhiteTop = (uiHeight - uiWhiteHeightPcnt * uiHeight / 100) / 2;
	uiWhiteBot = uiHeight - uiWhiteTop;
	uiWhiteLft = (uiWidth - uiWhiteWidthPcnt * uiWidth / 100) / 2;
	uiWhiteRgt = uiWidth - uiWhiteLft;

	for( ii = 0; ii < uiHeight; ii++ )
	{
		/* for each pixel of the line */
		for( jj = 0; jj < uiWidth; jj++ )
		{
			if (ePattern != TEST_SurfacePattern_eColorBar)
			{
				/* which pattern to use? */
				switch( ePattern )
				{
				case TEST_SurfacePattern_eCheckBoard:
					TEST_checkboard_YCbCr_pixel( jj, uiWidth, ii, uiHeight, &Y, &Cb, &Cr );
					break;
				case TEST_SurfacePattern_eCrossHatch:
					TEST_crosshatch_YCbCr_pixel( jj, ii, &Y, &Cb, &Cr );
					break;
				case TEST_SurfacePattern_eHLumaRamp:
					TEST_lumaramp_YCbCr_pixel( jj, uiWidth, ii, uiHeight, &Y, &Cb, &Cr, true );
					break;
				case TEST_SurfacePattern_eVLumaRamp:
					TEST_lumaramp_YCbCr_pixel( jj, uiWidth, ii, uiHeight, &Y, &Cb, &Cr, false );
					break;
				default:
					BDBG_ERR(("TODO: frame buffer filling Error handling."));
				}/* end of switch pattern */
			}

			if ((ii < uiWhiteTop) || (ii > uiWhiteBot) ||
				(jj < uiWhiteLft) || (jj > uiWhiteRgt))
			{
				Y = 16;
				Cb = 128;
				Cr = 128;
			}

			/* traverse through each frame */
			for (uiFrmId = 0; uiFrmId < ulNumFrms; uiFrmId++ )
			{
				/* Note: top fld surface only has even lines (including line 0), and
				 * botttom field surfaces only has odd lines */
				if ((!bFldSur) ||
					((ii & 0x1) == (uiFrmId && 0x1)))
				{
					/* color bar depends on FrmId, it left-shift a region for each frame */
					if (ePattern == TEST_SurfacePattern_eColorBar)
					{
						TEST_colorbar_YCbCr_pixel( jj, uiWidth, false, 0,
												   /*((bFldSur)? (uiFrmId/2): uiFrmId),*/ &Y, &Cb, &Cr );
					}
					if ((ii < uiWhiteTop) || (ii > uiWhiteBot) ||
						(jj < uiWhiteLft) || (jj > uiWhiteRgt))
					{
						Y = 16;
						Cb = 128;
						Cr = 128;
					}

#if (BTST_P_SUPPORT_DMA)
					if(hMem != pContext->astMemInfo[0].hMem)
					{
						BSUR_Surface_GetAddress( hTmpSurface[uiFrmId],
												 (void *) (&pvSurStartAddr), &ulSurPitch );
					}
					else
#endif
					{
						BSUR_Surface_GetAddress( pTstMpegSrcFld->aFrmBuf[uiFrmId].hSurface,
												 (void *) (&pvSurStartAddr), &ulSurPitch );
					}
					pvSurStartAddr += ((bFldSur)? ii/2: ii)  * ulSurPitch;

					if(BPXL_IS_YCbCr444_10BIT_FORMAT(g_ePxlFmt))
					{
						((uint32_t *) pvSurStartAddr) [jj] =
								(Cr << 22) | (Y << 12) | (Cb << 2);
					}
					else
					{
						if( jj & 1 )
						{
							pvSurStartAddr[(jj-1) * 2 + BPXL_COMPONENT_POS(g_ePxlFmt, 2) / 8] = Y;
						}
						else
						{
							((uint32_t *) pvSurStartAddr) [jj / 2] =
								BPXL_MAKE_PIXEL(g_ePxlFmt, Y, Y, Cb, Cr);
						}
					}

				}
			}/* end of frame index for loop */
		}/* end of width for loop */
	}/* end of height for loop */

#if (BTST_P_SUPPORT_DMA)
	/* DMA from Memc0 to Memc1 */
	if(hMem != pContext->astMemInfo[0].hMem)
	{
		for (uiFrmId = 0; uiFrmId < ulNumFrms; uiFrmId++ )
		{
			TEST_Gfx2Mfd_DmaFrameBuffers(g_frmInfo.hChp, g_frmInfo.hReg,
				g_frmInfo.hMem, g_frmInfo.hInt,
				pContext->astMemInfo[0].hMem, hTmpSurface[uiFrmId],
				hMem, pTstMpegSrcFld->aFrmBuf[uiFrmId].hSurface);
		}
	}
#else
	BSTD_UNUSED(pContext);
#endif /* #if (BCHP_CHIP != 3563) && (BCHP_CHIP != 7550) */

	BDBG_LEAVE(TEST_Gfx2Mfd_MakeSurfaces);
	return 0;
}

/* ======================= Callback: Mpeg Source ======================
 * This Callback is called at start of current display field/frame,
 * it should provide a graphics surface to BVDC in order to display
 * in next field/frame, by passing the BAVC_MFD_Picture struct point
 * to BVDC.
 *
 * eInterruptPolarity, eSourcePolarity, i32_HorizontalPanScan,
 * i32_VerticalPanScan and hSurface changes per vsync and need to
 * be filled in callback
 */
static void TEST_Gfx2Mfd_Callback(void *parm1, int iField)
{
	TEST_MFD_SrcFields     *pstFieldData = (TEST_MFD_SrcFields *)parm1;
	BAVC_MFD_Picture       *pAvcPic = &(pstFieldData->AvcMfdPic);
	int16_t iPanTop, iPanLeft;
	uint32_t uiCurFrm = pstFieldData->uiCurFrm;
	uint32_t  ulSurfWidth, ulSurfHeight, ulSurfPitch;
	BSUR_Surface_Handle   hSurface;
	uint8_t   *pulSurfAddr;

	/* loop around the frame buffer. */
	pstFieldData->uiCurFrm = (++uiCurFrm) % pstFieldData->uiNumFrames;

	/* avoid field inversion */
	if ((pstFieldData->bFldSur) && (BAVC_Polarity_eFrame != iField) &&
		((BAVC_Polarity) iField != pstFieldData->aFrmBuf[pstFieldData->uiCurFrm].ePolarity))
	{
		uiCurFrm = pstFieldData->uiCurFrm;
		pstFieldData->uiCurFrm = (++uiCurFrm) % pstFieldData->uiNumFrames;
	}

	hSurface = pstFieldData->aFrmBuf[pstFieldData->uiCurFrm].hSurface;
	BSUR_Surface_GetDimensions(hSurface, &ulSurfWidth, &ulSurfHeight);
	BSUR_Surface_GetAddress(hSurface, (void *)(&pulSurfAddr), &ulSurfPitch);

	pAvcPic->ulSourceHorizontalSize  = ulSurfWidth;
	pAvcPic->ulSourceVerticalSize    = ulSurfHeight;
	pAvcPic->ulDisplayHorizontalSize = ulSurfWidth;
	pAvcPic->ulDisplayVerticalSize   = ulSurfHeight;
	pAvcPic->ulRowStride = ulSurfPitch;

	if(pstFieldData->ulGfx2Mfd == 1)
	{
		pAvcPic->hSurface = hSurface;
		pAvcPic->ePxlFmt  = BPXL_INVALID;
	}
	else if(pstFieldData->ulGfx2Mfd == 2)
	{
		BPXL_Format eSurfFormat;
		
		BSUR_Surface_GetFormat(pstFieldData->aFrmBuf[pstFieldData->uiCurFrm].hSurface,
			&eSurfFormat);
		pAvcPic->hSurface = NULL;
		pAvcPic->ePxlFmt = eSurfFormat;
		pAvcPic->pLuminanceFrameBufferAddress = pulSurfAddr;
		pAvcPic->pChrominanceFrameBufferAddress = NULL;
	}
	
	if(pstFieldData->hPanVecGen)
	{
		/* pan & scan offset circling:
		 * ->(-1, 1) -> (1, 1) -> (1, -1) -> (-1, -1)-> */
		TEST_PanVecGen_GetPanVector(pstFieldData->hPanVecGen, iField,
									&iPanTop, &iPanLeft);
		pAvcPic->i32_HorizontalPanScan = iPanLeft;
		pAvcPic->i32_VerticalPanScan   = iPanTop;
	}
	else
	{
		pAvcPic->i32_HorizontalPanScan = 0;
		pAvcPic->i32_VerticalPanScan   = 0;
	}

	/* OK to display interlaced frames with progressive display, also OK to
	 * display full frame on interlaced display */
	switch (iField)
	{
	case BAVC_Polarity_eTopField:
		pAvcPic->eSourcePolarity = pstFieldData->aFrmBuf[pstFieldData->uiCurFrm].ePolarity;
		pAvcPic->eInterruptPolarity = BAVC_Polarity_eTopField;
		break;

	case BAVC_Polarity_eBotField:
		pAvcPic->eSourcePolarity = pstFieldData->aFrmBuf[pstFieldData->uiCurFrm].ePolarity;
		pAvcPic->eInterruptPolarity = BAVC_Polarity_eBotField;
		break;

	case BAVC_Polarity_eFrame:
		pAvcPic->eSourcePolarity = pstFieldData->aFrmBuf[pstFieldData->uiCurFrm].ePolarity;
		pAvcPic->eInterruptPolarity = BAVC_Polarity_eFrame;
		break;
	}

	/* call BVDC_Source_MpegDataReady_isr: get the surface built into the RUL which will
	 * be executed after current field/frame display is done */
	BVDC_Source_MpegDataReady_isr( (void*)pstFieldData->hSource,
								   0,
								   pAvcPic );
}

/* ***********************************************************
 * Initialize to use gfx surface as mfd src
 */
BERR_Code InitializeGfx2Mfd(
	BTST_P_Context     *pContext,
	BMEM_Handle         hMem,       /* device memory handle */
	BINT_Handle         hInt,       /* L2 interrupt handle */
	BVDC_Source_Handle  hSource,    /* source handle for the feeder */
	int                 iMfdIndex,  /* feeder Id */
	BFMT_VideoFmt       eSrcFormat,
	uint32_t            ulWhiteWidthPcnt,    /* white box width percent */
	uint32_t            ulWhiteHeightPcnt,   /* white box height percent */
	bool                bFldSur,    /* sur buf is a fld or frame */
	uint32_t            uiNumFrames,/* number of buffers to cyc through */
	uint32_t            uiPattern,  /* graphics pattern */
	bool                bEnPanScan
	)
{
	BERR_Code       err = BERR_SUCCESS;
	BINT_Id         InterruptName;
	uint32_t        ulWidth, ulHeight;
	const BFMT_VideoInfo  *pSrcFmtInfo;
	bool             bIsHd;

	pSrcFmtInfo = BFMT_GetVideoFormatInfoPtr(eSrcFormat);
	ulWidth     = pSrcFmtInfo->ulDigitalWidth;
	ulHeight    = pSrcFmtInfo->ulDigitalHeight;

	/* currently supported formats:
	 *  BFMT_VideoFmt_eNTSC, BFMT_VideoFmt_e480p, BFMT_VideoFmt_e720p,
	 *  BFMT_VideoFmt_e1080i, BFMT_VideoFmt_e1080p
	 */
	if((eSrcFormat == BFMT_VideoFmt_eNTSC) ||
	   (eSrcFormat == BFMT_VideoFmt_e480p))
		bIsHd = false;
	else
		bIsHd = true;

	/* intialize BAVC_MFD_Picture for this feeder:
	 * Fields of BAVC_MFD_Picture need to be filled for gfx surface
	 *    BAVC_Polarity            eInterruptPolarity;
	 *    bool                     bMute;
	 *    BMEM_Handle              hHeap;
	 *    BAVC_Polarity            eSourcePolarity;
	 *    int32_t                  i32_HorizontalPanScan;
	 *    int32_t                  i32_VerticalPanScan;
	 *    uint32_t                 ulDisplayHorizontalSize;
	 *    uint32_t                 ulDisplayVerticalSize;
	 *    BAVC_MatrixCoefficients  eMatrixCoefficients;
	 *    BFMT_AspectRatio         eAspectRatio;
	 *    BAVC_FrameRateCode       eFrameRateCode;
	 *    BSUR_Surface_Handle      hSurface;
	 * eInterruptPolarity, eSourcePolarity, i32_HorizontalPanScan,
	 * i32_VerticalPanScan and hSurface change per vsync, and will be filled
	 * in the vsync callback, the other do not change per vsync and will be
	 * initialized here */
	BKNI_Memset((void*)&s_TEST_MfdSrcFields[iMfdIndex], 0x0, sizeof(TEST_MFD_SrcFields));
	s_TEST_MfdSrcFields[iMfdIndex].AvcMfdPic.bMute = false;
	s_TEST_MfdSrcFields[iMfdIndex].AvcMfdPic.eOrientation = BFMT_Orientation_e2D;
	s_TEST_MfdSrcFields[iMfdIndex].AvcMfdPic.eMpegType = BAVC_MpegType_eMpeg2;
	s_TEST_MfdSrcFields[iMfdIndex].AvcMfdPic.hHeap = hMem;
	s_TEST_MfdSrcFields[iMfdIndex].AvcMfdPic.ulChannelId = 0;
	s_TEST_MfdSrcFields[iMfdIndex].AvcMfdPic.eAspectRatio   = BFMT_AspectRatio_e4_3;
	s_TEST_MfdSrcFields[iMfdIndex].AvcMfdPic.eFrameRateCode = BAVC_FrameRateCode_e29_97;
	s_TEST_MfdSrcFields[iMfdIndex].AvcMfdPic.eMatrixCoefficients     =
		bIsHd ? BAVC_MatrixCoefficients_eItu_R_BT_709 : BAVC_MatrixCoefficients_eSmpte_170M;
	if (bEnPanScan)
	{
		s_TEST_MfdSrcFields[iMfdIndex].AvcMfdPic.ulDisplayHorizontalSize = ulWidth/2;
		s_TEST_MfdSrcFields[iMfdIndex].AvcMfdPic.ulDisplayVerticalSize   = ulHeight/2;
	}
	else
	{
		s_TEST_MfdSrcFields[iMfdIndex].AvcMfdPic.ulDisplayHorizontalSize = ulWidth;
		s_TEST_MfdSrcFields[iMfdIndex].AvcMfdPic.ulDisplayVerticalSize   = ulHeight;
	}

	/* initialize internal info */
	s_TEST_MfdSrcFields[iMfdIndex].hSource     = hSource;
	s_TEST_MfdSrcFields[iMfdIndex].ulGfx2Mfd   = pContext->ulGfx2Mfd[pContext->iSelectedVidSrc];
	s_TEST_MfdSrcFields[iMfdIndex].bFldSur     = bFldSur;
	s_TEST_MfdSrcFields[iMfdIndex].uiNumFrames = (bFldSur)? 2 * uiNumFrames: uiNumFrames;
	s_TEST_MfdSrcFields[iMfdIndex].uiCurFrm    = 0;

	/* create frame buffers (gfx surfaces) */
	if( TEST_Gfx2Mfd_MakeSurfaces(pContext,
								  &s_TEST_MfdSrcFields[iMfdIndex], hMem,
								  (TEST_SurfacePattern) uiPattern,
								  ulWidth, ulHeight, ulWhiteWidthPcnt, ulWhiteHeightPcnt,
								  bFldSur)
		!= 0 )
	{
		BDBG_ERR(("TEST_Gfx2Mfd_MakeSurfaces failed!"));
	}

	/* create pan scan vector generator */
	if ( bEnPanScan )
	{
		if ( TEST_PanVecGen_Create(&(s_TEST_MfdSrcFields[iMfdIndex].hPanVecGen), ulWidth, ulHeight,
								   s_TEST_MfdSrcFields[iMfdIndex].AvcMfdPic.ulDisplayHorizontalSize,
								   s_TEST_MfdSrcFields[iMfdIndex].AvcMfdPic.ulDisplayVerticalSize)
			 != 0 )
		{
			BDBG_ERR(("TEST_Gfx2Mfd_MakeSurfaces failed!"));
		}
	}
	else
	{
		s_TEST_MfdSrcFields[iMfdIndex].hPanVecGen = NULL;
	}

	/* --------------------------------------------------------------------
	 * register and enable a callback function for the mpeg src to feed data.
	 * Query the interrupt name for the mpeg source and use it to
	 * register the callback.
	 * NOTE:
	 * - For interlaced picture, BVDC_Source_GetInterruptName gets the interrupt
	 * corresponding to the RDC Done slot interrupt id for the next field, not
	 * the current field.
	 */
	/* for interlaced display: top field */
	TestError( BVDC_Source_GetInterruptName(
		hSource,
		BAVC_Polarity_eTopField,     /* top field */
		&InterruptName),
		"ERROR: BVDC_Source_GetInterruptName()" );

	TestError( BINT_CreateCallback(
		&s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eTopField],
		hInt,
		InterruptName,              /* use the queried int name */
		TEST_Gfx2Mfd_Callback,      /* the registered callback */
		&s_TEST_MfdSrcFields[iMfdIndex],
		BAVC_Polarity_eTopField),   /* top field */
		"ERROR: BINT_CreateCallback()" );

	TestError( BINT_EnableCallback( s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eTopField] ),
		"ERROR: BINT_EnableCallback" );

	/* for interlaced display: bottom field */
	TestError( BVDC_Source_GetInterruptName(
		hSource,
		BAVC_Polarity_eBotField,    /* bottom field */
		&InterruptName),
		"ERROR: BVDC_Source_GetInterruptName()" );

	TestError( BINT_CreateCallback(
		&s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eBotField],
		hInt,
		InterruptName,              /* use the queried int name */
		TEST_Gfx2Mfd_Callback,      /* the registered callback */
		&s_TEST_MfdSrcFields[iMfdIndex],
		BAVC_Polarity_eBotField),   /* bottom field */
		"ERROR: BINT_CreateCallback()" );

	TestError( BINT_EnableCallback( s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eBotField] ),
		"ERROR: BINT_EnableCallback" );

	/* for progressive display */
	TestError( BVDC_Source_GetInterruptName(
		hSource,
		BAVC_Polarity_eFrame,       /* progressive frame */
		&InterruptName),
		"ERROR: BVDC_Source_GetInterruptName()" );

	TestError( BINT_CreateCallback(
		&s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eFrame],
		hInt,
		InterruptName,              /* use the queried int name */
		TEST_Gfx2Mfd_Callback,      /* the registered callback */
		&s_TEST_MfdSrcFields[iMfdIndex],
		BAVC_Polarity_eFrame),      /* progressive frame */
		"ERROR: BINT_CreateCallback()" );

	TestError( BINT_EnableCallback( s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eFrame] ),
		"ERROR: BINT_EnableCallback" );

Done:
	return err;
}

/* ***********************************************************
 * Terminate use gfx surface as mfd src
 */
void TerminateGfx2Mfd(
	int          iMfdIndex )
{
	uint32_t uiFrmId;

	/* shutdown top field callback */
	if(s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eBotField])
	{
		/* top field or frame */
		BINT_DisableCallback(s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eBotField]);
		BINT_DestroyCallback(s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eBotField]);
		s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eBotField] = NULL;
	}

	/* shutdown bottom field callback */
	if(s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eBotField])
	{
		/* top field or frame */
		BINT_DisableCallback(s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eBotField]);
		BINT_DestroyCallback(s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eBotField]);
		s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eBotField] = NULL;
	}

	/* shutdown frame callback */
	if(s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eFrame])
	{
		/* top field or frame */
		BINT_DisableCallback(s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eFrame]);
		BINT_DestroyCallback(s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eFrame]);
		s_hMpegSrcCallback[iMfdIndex][BAVC_Polarity_eFrame] = NULL;
	}

	/* destroy surfaces */
	for (uiFrmId = 0; uiFrmId < s_TEST_MfdSrcFields[iMfdIndex].uiNumFrames; uiFrmId++ )
	{
		BSUR_Surface_Destroy(s_TEST_MfdSrcFields[iMfdIndex].aFrmBuf[uiFrmId].hSurface);
	}
}

/* end of file */
