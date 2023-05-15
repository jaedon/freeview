/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mpg_decoder.h $
 * $brcm_Revision: Hydra_Software_Devel/13 $
 * $brcm_Date: 12/16/11 4:09p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/mpg_decoder.h $
 * 
 * Hydra_Software_Devel/13   12/16/11 4:09p yuxiaz
 * SW7425-967: Added support to use bitmap image for static buffer test.
 * 
 * Hydra_Software_Devel/12   4/15/11 1:54p tdo
 * SW7422-417: Add MVC/SVC support to vdc_test
 *
 * Hydra_Software_Devel/11   11/15/10 7:45p hongtaoz
 * SW7422-13, SW7425-32: added circle-diagonal-cross-box pattern to static
 * buffer; this will help with BVN bringup test;
 *
 * Hydra_Software_Devel/10   11/5/10 2:44p yuxiaz
 * SW7422-33, SW7422-77: Added 3D support in static buffer test.
 *
 * Hydra_Software_Devel/9   11/3/10 11:25a yuxiaz
 * SW7422-33: Added format selection for static buffer.
 *
 * Hydra_Software_Devel/8   10/11/10 1:11p jessem
 * SW7420-173: Added support for VFD as source feature.
 *
 * Hydra_Software_Devel/SW7420-173/2   10/4/10 3:36p jessem
 * SW7420-173: Added HD colors to TEST_singlecolor_YCbCr_pixel().
 *
 * Hydra_Software_Devel/SW7420-173/1   10/1/10 4:15p jessem
 * SW7420-173: Added TEST_singlecolor_YCbCr_pixel function.
 *
 * Hydra_Software_Devel/7   9/10/09 4:33p yuxiaz
 * SW3548-2455: Fixed the wrong color.
 *
 * Hydra_Software_Devel/6   9/10/09 1:32p yuxiaz
 * SW3548-2455: Colorbar should be rotating when allocating more than 1
 * frame.
 *
 * Hydra_Software_Devel/5   2/13/09 4:19p tdo
 * PR48866: Bringup gfxmfd in vdc_test for MFD on MEMC1
 *
 * Hydra_Software_Devel/4   6/10/08 2:03p syang
 * PR 39209:  use gfxmfd to test lBOX with 1080p input
 *
 * Hydra_Software_Devel/3   10/16/07 2:46p rpan
 * PR34857: First version after merging the latest xvd_vdc_test and
 * mvd_vdc_test as of Oct. 16, 2007, 12pm.
 *
 *
 * Hydra_Software_Devel/1   9/17/07 4:36p rpan
 * PR34857: Initial revision. Ported from xvd_vdc_test.
 *
 *
 ***************************************************************************/

#ifndef _MPG_DECODER_H__
#define _MPG_DECODER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bstd.h"           /* standard types */
#include "bavc.h"
#include "config.h"

/* NOTE: mpeg feeder is in big-endian if it's mpeg format data!!!
 *	- the luma buffer is organized as Y0Y1Y2Y3...
 *	- the chroma buffer is organized as Cb0Cb2Cr0Cr2...for MVD
 *  - the chroma buffer is organized as Cr0Cb0Cr2Cb2...for XVD
 */
#if BSTD_CPU_ENDIAN == BSTD_ENDIAN_LITTLE
	/* reverse byte order */
	/* Used for XVD */
	#define BTST_XVD_GET_LUMA_OFFSET(offset)    ((offset) & 0xFFFFFFFC) + (3 - ((offset) & 0x3))
	#define BTST_XVD_GET_CHROMA_OFFSET(offset)  ((offset) & 0xFFFFFFFD) + (2 - ((offset) & 0x2))
	/* Used for MVD */
	#define Y_OFFSET(offset)           ((offset) & 0xFFFFFFFC) + (3 - ((offset) & 0x3))
	#define CB_OFFSET(offset)           Y_OFFSET(offset)
	#define CR_OFFSET(offset)          (CB_OFFSET(offset) - 2)
#else
	/* Used for XVD */
	#define BTST_XVD_GET_LUMA_OFFSET(offset)     (offset)
	#define BTST_XVD_GET_CHROMA_OFFSET(offset)    (offset)
	/* Used for MVD */
	#define Y_OFFSET(offset)           (offset)
	#define CB_OFFSET(offset)          (offset)
	#define CR_OFFSET(offset)          ((offset) + 2)
#endif


#define IS_XVD(mpegCoreId) (mpegCoreId < MAX_XVD_CORES)
#define IS_MVD(mpegCoreId) (mpegCoreId >= MAX_XVD_CORES)

/* ***********************************************************
 * Static buffer
 */
BERR_Code InitializeMpegStaticSrc(
	BTST_P_Context	   *pContext,
	BVDC_Source_Handle  hMfdSource,
	BFMT_VideoFmt       eMfdFormat,
	uint32_t            uiFrames,
	bool                bEnablePanScan,
	uint32_t            uiPattern);

void TerminateMpegStaticSrc(
	BTST_P_Context		*pContext);

/* ***********************************************************
 * MPEG decoding
 */
void BTST_StartDecode
	(BTST_P_Context          *pContext,
     uint16_t                 ui16VidPid,
	 uint16_t                 ui16PcrPid,
	 uint16_t                 ui16EnhPid,
	 BAVC_StreamType          eStreamType,
	 BAVC_VideoCompressionStd eVideoCmprStd,
	 int					  iXptChNum);

void BTST_StopDecode
	(BTST_P_Context          *pContext);


void BTST_OpenDecoderChannel
	( BTST_P_Context          *pContext );

void BTST_CloseDecoderChannel
	( BTST_P_Context          *pContext );


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
	);

/* ***********************************************************
 * Terminate use gfx surface as mfd src
 */
void TerminateGfx2Mfd(
	int          iMfdIndex );



/***********************************************************************
 * pixel generator utility functions
 */

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
	);

/*
 *  int singlecolor_YCbCr_pixel ()
 *  Generate single color
 */
void TEST_singlecolor_YCbCr_pixel
(	uint32_t  color,
	bool      bHD,
	uint8_t  *y,
	uint8_t  *cb,
	uint8_t  *cr );

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
	);
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
	);

/*
 *  int lumaramp_YCbCr_pixel ()
 *  Generate luma ramp in H or V direction YCbCr pixel
 */
void TEST_lumaramp_YCbCr_pixel(
	uint32_t  xi,
	uint32_t  x_size,
	uint32_t  yi,
	uint32_t  y_size,
	uint8_t  *y,
	uint8_t  *cb,
	uint8_t  *cr,
	bool      bHorz );

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
	);

void TEST_Bitmap_YCbCr_pixel (
	FILE    *fp,
	int32_t  xi, 
	int32_t  yi,
	uint32_t  w,
	uint32_t  h,
	uint8_t  *y,
	uint8_t  *cb,
	uint8_t  *cr
	);

#ifdef __cplusplus
}
#endif

#endif /* _MPG_DECODER_H__ */

/* end of file */
