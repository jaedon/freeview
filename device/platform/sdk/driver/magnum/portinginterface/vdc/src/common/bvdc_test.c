/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_test.c $
 * $brcm_Revision: Hydra_Software_Devel/60 $
 * $brcm_Date: 5/9/12 10:54a $
 *
 * Module Description:
 *	Header file for Test functions.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/core/bvdc_test.c $
 * 
 * Hydra_Software_Devel/60   5/9/12 10:54a syang
 * SW7425-2093: refactor gfxfeeder code for deep cleanup and potential
 * error fix with GetSurface; use shared gfxsurface to manage gfx surface
 * 
 * Hydra_Software_Devel/59   4/4/12 2:17p jessem
 * SW7425-2827: Added BVDC_Test_Source_GetGfdScratchRegisters
 * 
 * Hydra_Software_Devel/58   2/16/12 1:16p pntruong
 * SW7231-587: Correctly propagate the original pts value to get buffer.
 *
 * Hydra_Software_Devel/57   1/27/12 3:53p pntruong
 * SW7231-587: Allow Original PTS to be passed-through VDC and come out of
 * GetBuffer() to Nexus to Raaga encode.
 *
 * Hydra_Software_Devel/56   1/17/12 5:09p tdo
 * SW7231-479: Fix GetBuffer_isr function to work with Psf case
 *
 * Hydra_Software_Devel/55   12/6/11 4:52p syang
 * SW7425-1429: quiet error print when no buf available
 *
 * Hydra_Software_Devel/54   11/30/11 6:16p syang
 * SW7425-1429: filter out fixed color buf
 *
 * Hydra_Software_Devel/53   11/9/11 6:45p syang
 * SW7425-1429: fix NULL pHeapNode issue with GetBuffer_isr /
 * ReturnBuffer_isr  during vnet reconfigure
 *
 * Hydra_Software_Devel/52   10/27/11 4:29p syang
 * SW7425-1429: move per vsync call back from window to display
 *
 * Hydra_Software_Devel/51   10/24/11 4:27p syang
 * SW7425-1429: check in after debug
 *
 * Hydra_Software_Devel/50   10/21/11 4:53p syang
 * SW7425-1429: refined some name sof the data in
 * BVDC_Test_Window_CapturedImage
 *
 * Hydra_Software_Devel/49   10/21/11 11:16a syang
 * SW7425-1429: added GetBuff_isr and ReturnBuff_isr and per vsync window
 * call back
 *
 * Hydra_Software_Devel/48   4/18/11 8:50a vanessah
 * SW7425-374: turn on osd for Mcdi debug
 *
 * Hydra_Software_Devel/47   11/11/10 7:31p albertl
 * SW7125-364: Fixed BVDC_P_CbIsDirty and added assert to check bitfields
 * in dirty bits fit within union integer representation.  Fixed naming
 * of dirty bits.
 *
 * Hydra_Software_Devel/46   6/18/10 4:24p rpan
 * SW7400-2763: New BVN resource management.
 *
 * Hydra_Software_Devel/45   5/7/10 7:20p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings
 *
 * Hydra_Software_Devel/44   4/19/10 10:22p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/43   4/7/10 11:35a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/42   4/5/10 4:14p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/41   5/31/07 5:39p tdo
 * PR27216: clean up warnings on VxWorks build
 *
 * Hydra_Software_Devel/40   4/12/07 5:37p tdo
 * PR28837: Provide API to customize LAB LUT
 *
 * Hydra_Software_Devel/39   3/28/07 11:15p pntruong
 * PR28395: Provide API to customize VDEC video quality.  Cleanup dirty
 * meanings.
 *
 * Hydra_Software_Devel/38   3/26/07 3:28p tdo
 * PR 28401: Provide API to customize CAB LUT and enable/disable CAB
 *
 * Hydra_Software_Devel/37   2/12/07 1:51p pntruong
 * PR27821:  Remove unused lagacy and work-around code (7038) from
 * mainline.
 *
 * Hydra_Software_Devel/36   12/18/06 11:31p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   11/20/06 3:20p tdo
 * PR 23174:  Add VDC function to overwrite HD_DVI color matrix to output
 * fix color
 *
 * Hydra_Software_Devel/35   8/30/06 11:25p tdo
 * PR 17994, PR 23383:  Add support for DNR demo mode
 *
 * Hydra_Software_Devel/34   8/30/06 6:02p tdo
 * PR 23212: Remove API for color clip and contrast stretch from test.
 * They are now in bvdc.h
 *
 * Hydra_Software_Devel/33   8/21/06 3:24p yuxiaz
 * PR20875: Remove unused test API BVDC_Test_Source_SetFeederImageFormat.
 * Use BVDC_Test_Source_SetFeederImageFormat instead.
 *
 * Hydra_Software_Devel/32   5/23/06 5:41p hongtaoz
 * PR17514: support correct usage mode of DNR; now DNR is available for
 * any video window and shared between mpeg sources; only one mpeg source
 * can acquire the DNR at a time;
 *
 * Hydra_Software_Devel/31   5/19/06 2:29p hongtaoz
 * PR17514: added bypass filter enum for DNR setting; disable extreme
 * filter for 7401a0 due to PR16299; fixed range clamping for DNR
 * settings; added DNR demo mode setting in test api;
 *
 * Hydra_Software_Devel/30   5/19/06 1:49p pntruong
 * PR21560: Global symbols without BVDC prefix in the BVDC module.  Reduce
 * unnecessary global exported symbols.  Tighten const params.
 *
 * Hydra_Software_Devel/28   3/7/06 4:18p syang
 * PR 19670: added 7438 support
 *
 * Hydra_Software_Devel/27   2/23/06 2:32p tdo
 * PR 16061: Add filter for max, mid, and min.  Change some variable names
 * to be more meaningful.  Change the max point to 94 and 95 to reduce
 * the affect of some white letters suddenly appear on screen.  Increase
 * precision of some incremental variables
 *
 * Hydra_Software_Devel/24   1/31/06 5:20p pntruong
 * PR17778: Take in changes for new hddvi.
 *
 * Hydra_Software_Devel/20   1/12/06 9:58a tdo
 * PR 16061: Implementing dynamic contrast stretch in the PEP
 *
 * Hydra_Software_Devel/19   10/13/05 11:52a hongtaoz
 * PR17532: added a test API to turn on OSD for deinterlacer;
 *
 * Hydra_Software_Devel/18   6/28/05 11:04a jasonh
 * PR 16012: Adding support for 7401
 *
 * Hydra_Software_Devel/17   5/20/05 9:44a tdo
 * PR 15183: Add sharpness control to the PEP and remove the old TAB
 * setting
 *
 * Hydra_Software_Devel/16   3/17/05 6:41p pntruong
 * PR14494: Add preliminary software support to 3560 A0.
 *
 * Hydra_Software_Devel/15   12/8/04 6:29p hongtaoz
 * PR9518: each display only has two associated rdc slots;
 *
 * Hydra_Software_Devel/14   7/30/04 12:47p tdo
 * PR11971: Use host write to load CAB and LAB table
 *
 * Hydra_Software_Devel/13   7/28/04 11:55a tdo
 * PR11971: Add CAB and LAB support
 *
 ***************************************************************************/
#include "bvdc_test.h"
#include "bvdc_display_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_feeder_priv.h"
#include "bvdc_compositor_priv.h"
#include "bvdc_pep_priv.h"
#include "bvdc_mad_priv.h"
#include "bvdc_mcvp_priv.h"
#include "bvdc_mcdi_priv.h"


BDBG_MODULE(BVDC_TEST);

/*************************************************************************
 *	BVDC_Test_Display_GetInterruptName
 *************************************************************************/
BERR_Code BVDC_Test_Display_GetInterruptName
	( BVDC_Display_Handle             hDisplay,
	  const BAVC_Polarity             eFieldId,
	  BINT_Id                        *pInterruptName )
{
	BDBG_ENTER(BVDC_Test_Display_GetInterruptName);

	if (!hDisplay)
	{
		BDBG_ERR(("Invalid parameter"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	BDBG_OBJECT_ASSERT(hDisplay->hCompositor, BVDC_CMP);

	if(pInterruptName)
	{
		*pInterruptName = BRDC_Slot_GetIntId(
			hDisplay->hCompositor->ahSlot[eFieldId == BAVC_Polarity_eBotField]);
	}

	BDBG_LEAVE(BVDC_Test_Display_GetInterruptName);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Test_Window_SetMadOsd
 *************************************************************************/
BERR_Code BVDC_Test_Window_SetMadOsd
	( BVDC_Window_Handle               hWindow,
	  bool                             bEnable,
	  uint32_t                         ulHpos,
	  uint32_t                         ulVpos)
{
	BVDC_P_Mad_Handle  hMad32 = NULL;
	BVDC_P_Mcdi_Handle  hMcdi  = NULL;
	BDBG_ENTER(BVDC_Test_Window_SetMadOsd);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	hMad32 = hWindow->stCurResource.hMad32;

	hMcdi = hWindow->stCurResource.hMcvp->hMcdi;

	if(!(hMad32 || hMcdi))
	{
		BDBG_ERR(("Window %d doesn't support deinterlacing Mad %8x Mcdi%8x", hWindow->eId, hMad32, hMcdi));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(hMad32)
	{
		/* set new value */
		hMad32->bEnableOsd = bEnable;
		/* Note: ulHPos must be an even number due to YUV422 format */
		hMad32->ulOsdHpos  = ulHpos & (~1);
		hMad32->ulOsdVpos  = ulVpos;
	}

	if(hMcdi)
	{
		/* set new value */
		hMcdi->bEnableOsd = bEnable;
		/* Note: ulHPos must be an even number due to YUV422 format */
		hMcdi->ulOsdHpos  = ulHpos & (~1);
		hMcdi->ulOsdVpos  = ulVpos;

	}

	BDBG_LEAVE(BVDC_Test_Window_SetMadOsd);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Test_Window_GetMadOsd
 *************************************************************************/
BERR_Code BVDC_Test_Window_GetMadOsd
	( BVDC_Window_Handle               hWindow,
	  bool                            *pbEnable,
	  uint32_t                        *pulHpos,
	  uint32_t                        *pulVpos)
{
	BVDC_P_Mad_Handle hMad32;
	BVDC_P_Mcdi_Handle hMcdi = NULL;
	BDBG_ENTER(BVDC_Test_Window_GetMadOsd);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	hMad32 = hWindow->stCurResource.hMad32;
	hMcdi = hWindow->stCurResource.hMcvp->hMcdi;
	if((!(hMad32 || hMcdi)) ||
		(!(pbEnable && pulHpos && pulVpos)))
	{
		BDBG_ERR(("Mad %8x Mcdi %8x", hMad32, hMcdi));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	/* Use BREG_Write32 to load table first */

	if(hMad32)
	{
		*pbEnable = hMad32->bEnableOsd;
		*pulHpos  = hMad32->ulOsdHpos;
		*pulVpos  = hMad32->ulOsdVpos;
	}

	if(hMcdi)
	{
		*pbEnable = hMcdi->bEnableOsd;
		*pulHpos  = hMcdi->ulOsdHpos;
		*pulVpos  = hMcdi->ulOsdVpos;
	}

	BDBG_LEAVE(BVDC_Test_Window_GetMadOsd);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Test_Source_SetFixColor
 *************************************************************************/
BERR_Code BVDC_Test_Source_SetFixColor
	( BVDC_Source_Handle               hSource,
	  BAVC_Polarity                    eFieldId,
	  bool                             bEnable,
	  uint32_t                         ulRed,
	  uint32_t                         ulGreen,
	  uint32_t                         ulBlue )
{
	BVDC_P_Source_Info *pNewInfo;
	uint32_t ulColorARGB;
	unsigned int ulFixColorYCrCb;

	BDBG_ENTER(BVDC_Test_Source_SetFixColor);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);
	pNewInfo = &hSource->stNewInfo;

	ulColorARGB = BPXL_MAKE_PIXEL(BPXL_eA8_R8_G8_B8, 0x00,
		ulRed, ulGreen, ulBlue);
	BPXL_ConvertPixel_RGBtoYCbCr(BPXL_eA8_Y8_Cb8_Cr8, BPXL_eA8_R8_G8_B8,
		ulColorARGB, &ulFixColorYCrCb);

	BDBG_ERR(("ulFixColorYCrCb[%d] = 0x%x", eFieldId, ulFixColorYCrCb));

	if((hSource->stCurInfo.bFixColorEnable != bEnable) ||
	   (hSource->stCurInfo.aulFixColorYCrCb[eFieldId] != ulFixColorYCrCb) ||
	   (hSource->stNewInfo.bErrorLastSetting))
	{
		BVDC_P_Source_DirtyBits *pNewDirty = &pNewInfo->stDirty;

		pNewInfo->bFixColorEnable = bEnable;
		pNewInfo->aulFixColorYCrCb[eFieldId] = ulFixColorYCrCb;

		/* Dirty bit set */
		pNewDirty->stBits.bColorspace = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Test_Source_SetFixColor);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Test_Source_GetFixColor
 *************************************************************************/
BERR_Code BVDC_Test_Source_GetFixColor
	( BVDC_Source_Handle               hSource,
	  BAVC_Polarity                    eFieldId,
	  bool                            *pbEnable,
	  uint32_t                        *pulRed,
	  uint32_t                        *pulGreen,
	  uint32_t                        *pulBlue )
{
	unsigned int ulColorARGB;

	BDBG_ENTER(BVDC_Test_Source_GetFixColor);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	BPXL_ConvertPixel_YCbCrtoRGB(BPXL_eA8_R8_G8_B8, BPXL_eA8_Y8_Cb8_Cr8,
		hSource->stCurInfo.aulFixColorYCrCb[eFieldId], 0, 0xFF, &ulColorARGB);

	BDBG_ERR(("aulFixColorYCrCb[%d] = 0x%x", eFieldId, hSource->stCurInfo.aulFixColorYCrCb[eFieldId]));

	if(pbEnable)
	{
		*pbEnable = hSource->stCurInfo.bFixColorEnable;
	}

	if(pulRed)
	{
		*pulRed   = (uint32_t)BPXL_GET_COMPONENT(BPXL_eA8_R8_G8_B8, ulColorARGB, 2);
	}

	if(pulGreen)
	{
		*pulGreen = (uint32_t)BPXL_GET_COMPONENT(BPXL_eA8_R8_G8_B8, ulColorARGB, 1);
	}

	if(pulBlue)
	{
		*pulBlue  = (uint32_t)BPXL_GET_COMPONENT(BPXL_eA8_R8_G8_B8, ulColorARGB, 0);
	}

	BDBG_LEAVE(BVDC_Test_Source_GetFixColor);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code  BVDC_Test_P_Buffer_ExtractBuffer_isr
	( BVDC_P_Buffer_Handle            hBuffer,
	  BVDC_P_PictureNode            **ppPicture )
{
	BVDC_P_PictureNode *pPrevReaderNode = NULL, *pTempNode = NULL, *pNextTempNode = NULL;

	/* user will call GetBuffer_isr in the callback function called at the end of reader_isr
	 * at this time, the PrevActiveAndNotUsedByUserNode of hBuffer->pCurReaderBuf is used
	 * by VFD right now, and we give it to user. This leads to correct lypsync. This buffer
	 * will then not be used by VDC PI software until it it returned.  Note: it is OK to have
	 * both user and VFD to read the same buffer. Note: if vnet reconfigure happens, we would
	 * have hBuffer->ulActiveBufCnt as 0, and pPrevReaderNode->pHeapNode might be NULL!!!
	 */
	BVDC_P_Buffer_GetPrevActiveAndNotUsedByUserNode(pPrevReaderNode, hBuffer->pCurReaderBuf);
	if ((pPrevReaderNode != hBuffer->pCurWriterBuf) && (pPrevReaderNode->pHeapNode) &&
		(!pPrevReaderNode->stFlags.bMute) && (!pPrevReaderNode->stFlags.bMuteFixedColor) &&
		(!pPrevReaderNode->stFlags.bMuteMad))
	{
		/* Mark picture node as currently used by user */
		pPrevReaderNode->stFlags.bUsedByUser = true;
		pPrevReaderNode->stFlags.bActiveNode = false;

		*ppPicture =pPrevReaderNode;

		/* XXX hSurface should NOT be needed !!!*/
		/*pTempNode->hSurface = NULL;*/

		/* Decrement active buffer count. */
		hBuffer->ulActiveBufCnt--;
		BDBG_MSG(("Extract bufferId %d", pPrevReaderNode->ulBufferId));

		/* Toggle the picture node destination polarity pointed
		 * to by the next writer and the nodes after it but before the reader. This
		 * is necessary to keep the field prediction correct.
		 */
		BVDC_P_Buffer_GetPrevActiveAndNotUsedByUserNode(pTempNode, hBuffer->pCurReaderBuf);

		while (pTempNode != hBuffer->pCurWriterBuf)
		{
			pTempNode->eDstPolarity = BVDC_P_NEXT_POLARITY(pTempNode->eDstPolarity);
			BVDC_P_Buffer_GetPrevActiveAndNotUsedByUserNode(pNextTempNode, pTempNode);
			pTempNode = pNextTempNode;
		} ;

		return BERR_SUCCESS;
	}
	else
	{
		BDBG_MSG(("Extract bufferId %d, node=0x%x???",
				  pPrevReaderNode->ulBufferId, pPrevReaderNode->pHeapNode));
		return BVDC_ERR_NO_AVAIL_CAPTURE_BUFFER;
	}
}

/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_Test_P_Window_CapturePicture_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_P_Window_CapturedPicture   *pCapturedPic )
{
	BERR_Code eRet = BERR_SUCCESS;
	unsigned int          uiPitch;

	BDBG_ENTER(BVDC_Test_P_Window_CapturePicture_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_ASSERT(pCapturedPic);

	/* Clear content */
	BKNI_Memset((void*)pCapturedPic, 0x0, sizeof(BVDC_P_Window_CapturedPicture));

	if(hWindow->uiAvailCaptureBuffers)
	{
		/* Mark the buffer as used by user */
		if((eRet = BVDC_Test_P_Buffer_ExtractBuffer_isr(hWindow->hBuffer, &pCapturedPic->pPicture)))
		{
			pCapturedPic->pvBufAddr   = NULL;
			pCapturedPic->pvBufAddr_R = NULL;
			return eRet;
		}

		/* Decrement number of capture buffers used */
		hWindow->uiAvailCaptureBuffers--;

		/* Give the virtual address to convert a picture to a surface. */
		pCapturedPic->pvBufAddr = pCapturedPic->pPicture->pHeapNode->pvBufAddr;

#if (BVDC_P_SUPPORT_3D_VIDEO)
		if(pCapturedPic->pPicture->pHeapNode_R != NULL)
			pCapturedPic->pvBufAddr_R  = pCapturedPic->pPicture->pHeapNode_R->pvBufAddr;
		pCapturedPic->eDispOrientation = pCapturedPic->pPicture->eDispOrientation;
#endif

		/* Get polarity */
		pCapturedPic->ePolarity = (BVDC_P_VNET_USED_SCALER_AT_WRITER(pCapturedPic->pPicture->stVnetMode)
				? pCapturedPic->pPicture->eDstPolarity : pCapturedPic->pPicture->eSrcPolarity);

		/* Get Pixel Format */
		pCapturedPic->ePxlFmt = pCapturedPic->pPicture->ePixelFormat;

		/* Get Height */
		if(pCapturedPic->ePolarity != BAVC_Polarity_eFrame)
		{
			pCapturedPic->ulHeight = pCapturedPic->pPicture->pVfdIn->ulHeight/2;
		}
		else
		{
			pCapturedPic->ulHeight = pCapturedPic->pPicture->pVfdIn->ulHeight;
		}

		/* Get width */
		pCapturedPic->ulWidth = pCapturedPic->pPicture->pVfdIn->ulWidth;

		/* Get pitch. See ulPitch in BVDC_P_Capture_SetEnable_isr and ulStride in
		   BVDC_P_Feeder_SetPlaybackInfo_isr */
		BPXL_GetBytesPerNPixels(pCapturedPic->ePxlFmt,
			pCapturedPic->pPicture->pVfdIn->ulWidth, &uiPitch);
		pCapturedPic->ulPitch = BVDC_P_ALIGN_UP(uiPitch, BVDC_P_PITCH_ALIGN);

		/* Get Original PTS */
		pCapturedPic->ulOrigPTS = pCapturedPic->pPicture->ulOrigPTS;
	}
	else
	{
		pCapturedPic->pvBufAddr   = NULL;
		pCapturedPic->pvBufAddr_R = NULL;
		eRet = BVDC_ERR_NO_AVAIL_CAPTURE_BUFFER;
	}

	BDBG_LEAVE(BVDC_Test_P_Window_CapturePicture_isr);
	return eRet;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Test_Window_GetBuffer_isr
	( BVDC_Window_Handle              hWindow,
	  BVDC_Test_Window_CapturedImage *pCapturedImage )
{
	BERR_Code eRet = BERR_SUCCESS;
	BVDC_P_Window_CapturedPicture stCaptPic;

	BDBG_ENTER(BVDC_Test_Window_GetBuffer_isr);
	BDBG_ASSERT(pCapturedImage);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	eRet = BVDC_Test_P_Window_CapturePicture_isr(hWindow, &stCaptPic);
	if (eRet == BERR_SUCCESS)
	{
		pCapturedImage->pvBufAddr = stCaptPic.pvBufAddr;
		pCapturedImage->ulWidth = stCaptPic.ulWidth;
		pCapturedImage->ulHeight = stCaptPic.ulHeight;
		pCapturedImage->ulPitch = stCaptPic.ulPitch;
		pCapturedImage->ePxlFmt = stCaptPic.ePxlFmt;
		pCapturedImage->eCapturePolarity = stCaptPic.ePolarity;
		pCapturedImage->ulCaptureNumVbiLines = stCaptPic.ulNumVbiLines;
#if (BVDC_P_SUPPORT_3D_VIDEO)
		pCapturedImage->pvBufAddrR = stCaptPic.pvBufAddr_R;
		pCapturedImage->eDispOrientation = stCaptPic.eDispOrientation;
#else
		pCapturedImage->pvBufAddrR = NULL;
#endif
		pCapturedImage->ulOrigPTS = stCaptPic.ulOrigPTS;
	}
	else
	{
		pCapturedImage->pvBufAddr  = NULL;
		pCapturedImage->pvBufAddrR = NULL;
	}

	BDBG_LEAVE(BVDC_Test_Window_GetBuffer_isr);
	return eRet;
	/*return BERR_TRACE(eRet); turn off error print per app request*/
}


/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_Test_P_Window_ReleasePicture_isr
	( BVDC_Window_Handle               hWindow,
	  void                            *pvBufAddr )
{
	BVDC_P_PictureNode *pPicture;
	uint32_t cnt = 0;

	BDBG_ENTER(BVDC_Test_P_Window_ReleasePicture_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	pPicture = hWindow->hBuffer->pCurReaderBuf;
	/* during vnet reconfigure, we might have pPicture->pHeapNode as NULL */
	while ((!pPicture->pHeapNode || (pvBufAddr != pPicture->pHeapNode->pvBufAddr)) &&
		   (cnt < BVDC_P_MAX_USER_CAPTURE_BUFFER_COUNT))
	{
		BVDC_P_Buffer_GetNextUsedByUserNode(pPicture, pPicture);
		cnt++;
	}

	if(cnt >= BVDC_P_MAX_USER_CAPTURE_BUFFER_COUNT)
	{
		/* likely it is during vnet reconfigure, what we should do with this
		 * return? does vnet reconfigure free and reset flag for all active and
		 * usedByUser buffers? */
		BDBG_MSG(("return mistached buffer, 0x%x???", pvBufAddr));
		return BERR_TRACE(BVDC_ERR_CAPTURED_BUFFER_NOT_FOUND);
	}
	else
	{
		BDBG_MSG(("return bufferId %d, addr 0x%x; Is Used? %d",
				  pPicture->ulBufferId, pPicture->pHeapNode->pvBufAddr,
				  pPicture->stFlags.bUsedByUser));
		BVDC_P_Buffer_ReturnBuffer_isr(hWindow->hBuffer, pPicture);
		hWindow->uiAvailCaptureBuffers++;
	}

	BDBG_LEAVE(BVDC_Test_P_Window_ReleasePicture_isr);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Test_Window_ReturnBuffer_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_Test_Window_CapturedImage  *pCapturedImage )
{
	BERR_Code eRet = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Test_Window_ReturnBuffer_isr);
	BDBG_ASSERT(pCapturedImage);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if (pCapturedImage->pvBufAddr)
	{
		eRet = BVDC_Test_P_Window_ReleasePicture_isr(hWindow, pCapturedImage->pvBufAddr);
		if (eRet != BERR_SUCCESS)
		{
			return BERR_TRACE(eRet);
		}
	}

	/* syang: the following is copied from BVDC_Winow_ReturnBuffer,
	   but looks wrong to me
#if (BVDC_P_SUPPORT_3D_VIDEO)
	if (pCapturedImage->pvBufAddrR)
	{
		eRet = BVDC_Test_P_Window_ReleasePicture_isr(hWindow, pCapturedImage->pvBufAddrR);
		if (eRet != BERR_SUCCESS)
		{
			return BERR_TRACE(eRet);
		}
	}
#endif
	*/

	BDBG_LEAVE(BVDC_Test_Window_ReturnBuffer_isr);
	return BERR_SUCCESS;
}

BERR_Code BVDC_Test_Source_GetGfdScratchRegisters
	( BVDC_Source_Handle               hSource,
	  uint32_t                        *pulScratchReg1,
	  uint32_t                        *pulScratchReg2 )
{
	BVDC_P_GfxFeeder_Handle hGfxFeeder;

	BDBG_ASSERT(hSource);

	hGfxFeeder = hSource->hGfxFeeder;

	*pulScratchReg1 = hGfxFeeder->stGfxSurface.ulSurAddrReg[0];
	*pulScratchReg2 = hGfxFeeder->stGfxSurface.ulSurAddrReg[1];

	return BERR_SUCCESS;
}

/* End of File */
