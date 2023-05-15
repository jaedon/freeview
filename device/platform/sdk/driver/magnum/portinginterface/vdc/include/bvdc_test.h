/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_test.h $
 * $brcm_Revision: Hydra_Software_Devel/27 $
 * $brcm_Date: 4/4/12 2:17p $
 *
 * Module Description:
 *	Header file for Test functions.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_test.h $
 * 
 * Hydra_Software_Devel/27   4/4/12 2:17p jessem
 * SW7425-2827: Added BVDC_Test_Source_GetGfdScratchRegisters
 * 
 * Hydra_Software_Devel/26   1/27/12 3:53p pntruong
 * SW7231-587: Allow Original PTS to be passed-through VDC and come out of
 * GetBuffer() to Nexus to Raaga encode.
 *
 * Hydra_Software_Devel/25   10/27/11 4:29p syang
 * SW7425-1429: move per vsync call back from window to display
 *
 * Hydra_Software_Devel/24   10/21/11 4:52p syang
 * SW7425-1429: refined some name sof the data in
 * BVDC_Test_Window_CapturedImage
 *
 * Hydra_Software_Devel/23   10/21/11 11:16a syang
 * SW7425-1429: added GetBuff_isr and ReturnBuff_isr and per vsync window
 * call back
 *
 * Hydra_Software_Devel/22   4/12/07 5:37p tdo
 * PR28837: Provide API to customize LAB LUT
 *
 * Hydra_Software_Devel/21   3/26/07 3:29p tdo
 * PR 28401: Provide API to customize CAB LUT and enable/disable CAB
 *
 * Hydra_Software_Devel/20   12/18/06 11:32p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   11/20/06 3:20p tdo
 * PR 23174:  Add VDC function to overwrite HD_DVI color matrix to output
 * fix color
 *
 * Hydra_Software_Devel/19   8/30/06 11:25p tdo
 * PR 17994, PR 23383:  Add support for DNR demo mode
 *
 * Hydra_Software_Devel/18   8/30/06 6:02p tdo
 * PR 23212: Remove API for color clip and contrast stretch from test.
 * They are now in bvdc.h
 *
 * Hydra_Software_Devel/17   8/21/06 4:02p yuxiaz
 * PR20875: Remove unused test API BVDC_Test_Source_SetFeederImageFormat.
 * Use BVDC_Window_SetPixelFormat instead.
 *
 * Hydra_Software_Devel/16   5/19/06 2:29p hongtaoz
 * PR17514: added bypass filter enum for DNR setting; disable extreme
 * filter for 7401a0 due to PR16299; fixed range clamping for DNR
 * settings; added DNR demo mode setting in test api;
 *
 * Hydra_Software_Devel/15   5/19/06 1:49p pntruong
 * PR21560: Global symbols without BVDC prefix in the BVDC module.  Reduce
 * unnecessary global exported symbols.  Tighten const params.
 *
 * Hydra_Software_Devel/14   2/23/06 2:32p tdo
 * PR 16061: Add filter for max, mid, and min.  Change some variable names
 * to be more meaningful.  Change the max point to 94 and 95 to reduce
 * the affect of some white letters suddenly appear on screen.  Increase
 * precision of some incremental variables
 *
 * Hydra_Software_Devel/13   1/12/06 9:58a tdo
 * PR 16061: Implementing dynamic contrast stretch in the PEP
 *
 * Hydra_Software_Devel/12   10/13/05 11:52a hongtaoz
 * PR17532: added a test API to turn on OSD for deinterlacer;
 *
 * Hydra_Software_Devel/11   5/20/05 9:45a tdo
 * PR 15183: Add sharpness control to the PEP and remove the old TAB
 * setting
 *
 * Hydra_Software_Devel/10   7/28/04 11:55a tdo
 * PR11971: Add CAB and LAB support
 *
 * Hydra_Software_Devel/9   7/26/04 6:03p tdo
 * PR 11971: add TAB and color clip support
 *
 * Hydra_Software_Devel/8   4/30/04 2:28p hongtaoz
 * PR8761: fixed C++ compiling errors;
 *
 * Hydra_Software_Devel/7   2/19/04 9:06a yuxiaz
 * PR 9393: Added more comment.
 *
 * Hydra_Software_Devel/6   2/18/04 4:21p yuxiaz
 * PR 9393: Add test API to support packed image format for Mpeg feeder.
 *
 * Hydra_Software_Devel/5   1/16/04 2:15p maivu
 * PR 9375: Added BVDC_Test_Display_GetInterruptName
 *
 * Hydra_Software_Devel/4   10/30/03 4:36p maivu
 * Added comments.
 *
 * Hydra_Software_Devel/3   10/30/03 3:16p maivu
 * Changed the interface for BVDC_Test_Display_EnableCrc and
 * BVDC_Test_Display_ResetCrc
 *
 * Hydra_Software_Devel/2   10/10/03 3:38p maivu
 * Removed private header file
 *
 * Hydra_Software_Devel/1   10/10/03 12:54p maivu
 * Initial revision
 *
 ***************************************************************************/
#ifndef BVDC_TEST_H__
#define BVDC_TEST_H__

#include "bvdc.h"

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
Summary:
	A structure representing the captured field/frame that is provided to the
	user application.

Description:
	pvBufAddr       - captured image derived from a field or frame
	pvBufAddrR      - captured right image derived from a field or frame
	ulWidth         - width of the captued image
	ulHeight        - height of the captued image
	ulPitch         - pitch of the captued image
	eFormat         - pixel format in the captued image
	eOutOrientation - display 3D orientation of the left and right surfaces
	eCapturePolarity - the polarity of the captured image.
	ulOrigPTS       - Original PTS of the picture.  Note this PTS value could be
	    either original coded PTS or interolated PTS by DM.  See also
	    BAVC_MFD_Picture.ulOrigPTS.

See Also:
	BVDC_Test_Window_GetBuffer_isr
****************************************************************************/
typedef struct
{
	void                              *pvBufAddr;
	void                              *pvBufAddrR;
	uint32_t                           ulWidth;
	uint32_t                           ulHeight;
	uint32_t                           ulPitch;
	BPXL_Format                        ePxlFmt;
	BFMT_Orientation                   eDispOrientation;
	unsigned long                      ulCaptureNumVbiLines;
	BAVC_Polarity                      eCapturePolarity;
	uint32_t                           ulOrigPTS;

} BVDC_Test_Window_CapturedImage;

/***************************************************************************
Summary:
	Get the Display interrupt name for the new state

Description:
	This function returns the interrupt name associated with the Display
	RUL when execution is completed. Applications can use the interrupt
	name to register a callback with BINT.

Input:
	hDisplay       - Display handle created earlier with BVDC_Display_Create.
	eFieldPolarity - Field polarity for that interrupt (top or bottom)

Output:
	pInterruptName - The L2 interrupt name that can be use with BINT
	to create the callback.

Returns:
	BERR_INVALID_PARAMETER - hDisplay is not a valid display handle.
	BERR_SUCCESS - Function succeed

See Also:
	BINT_CreateCallback.
****************************************************************************/
BERR_Code BVDC_Test_Display_GetInterruptName
(	BVDC_Display_Handle              hDisplay,
	const BAVC_Polarity              eFieldId,
	BINT_Id                         *pInterruptName );

/***************************************************************************
Summary:
	This function show/hide OSD of MAD for a window if that window enables
	deinterlace.

Description:
	Disable or enable OSD and set the OSD position.

Input:
	hWindow    - A valid window handle created ealier.
	bEnable    - boolean to enable/disable OSD.
	ulHpos     - On screen display horizontal position.
	             Note: ulHPos must be an even number due to YUV422 format.
	ulVpos     - On screen display vertical position.

Output:

Returns:
	BERR_SUCCESS - Successfully set the flag to shows/hides the OSD for the window.

See Also:
**************************************************************************/
BERR_Code BVDC_Test_Window_SetMadOsd
	( BVDC_Window_Handle               hWindow,
	  bool                             bEnable,
	  uint32_t                         ulHpos,
	  uint32_t                         ulVpos);

/***************************************************************************
Summary:
	This function gets OSD setting of MAD block if the window enables deinterlace.

Description:
	Get the OSD setting of window's MAD block.

Input:
	hWindow    - A valid window handle created ealier.
	bEnable    - boolean to enable/disable OSD.
	pulHpos    - Pointer to the On screen display horizontal position.
	pulVpos    - Pointer to the On screen display vertical position.

Output:

Returns:
	BERR_SUCCESS - Successfully Get the MAD's OSD status of the window.

See Also:
**************************************************************************/
BERR_Code BVDC_Test_Window_GetMadOsd
	( BVDC_Window_Handle               hWindow,
	  bool                            *pbEnable,
	  uint32_t                        *pulHpos,
	  uint32_t                        *pulVpos);

/***************************************************************************
Summary:
	This function sets source fix color output

Description:
	Overwrite the source color matrix to outputing a fix color

Input:
	hWindow    - A valid window handle created ealier.
	eFieldId   - Field polarity to have this fix color.
	bEnable    - boolean to enable/disable fix color.
	ucRed      - The RED component of the fix color.
	ucGreen    - The GREEN component of the fix color.
	ucBlue     - The BLUE component of the fix color.

Output:

Returns:
	BERR_SUCCESS - Successfully set the fix color for the source.

See Also:
	BVDC_Test_Source_GetFixColor
**************************************************************************/
BERR_Code BVDC_Test_Source_SetFixColor
	( BVDC_Source_Handle               hSource,
	  BAVC_Polarity                    eFieldId,
	  bool                             bEnable,
	  uint32_t                         ulRed,
	  uint32_t                         ulGreen,
	  uint32_t                         ulBlue );

/***************************************************************************
Summary:
	This function gets source fix color output

Description:
	Gets the source fix color for a field polarity if it is enable

Input:
	hWindow    - A valid window handle created ealier.
	eFieldId   - Field polarity to have this fix color.

Output:
	pbEnable   - boolean to enable/disable fix color.
	pucRed     - The RED component of the fix color.
	pucGreen   - The GREEN component of the fix color.
	pucBlue    - The BLUE component of the fix color.

Returns:
	BERR_SUCCESS - Successfully get the fix color for the source.

See Also:
	BVDC_Test_Source_SetFixColor
*************************************************************************/
BERR_Code BVDC_Test_Source_GetFixColor
	( BVDC_Source_Handle               hSource,
	  BAVC_Polarity                    eFieldId,
	  bool                            *pbEnable,
	  uint32_t                        *pulRed,
	  uint32_t                        *pulGreen,
	  uint32_t                        *pulBlue );

/***************************************************************************
Summary:
	Obtains the last captured buffer from the associated video window in
	_isr context

Description:
	This is the _isr version of BVDC_Window_GetBuffer. Refer to it for
	description first.

	An example application of this _isr version is for upper level software
	to get the current VDC display buffer and to pass to Raaga encoder,
	inside the callback function called from VDC at every display vsync. The
	callback function is installed by upper level software to VDC display.

	When field inversion happens, the captured buffer's polarity will not
	match the display polarity passed to the the callback function.

	Upper level software should call BVDC_Test_Window_GetBuffer_isr inside
	or after the display's per vsync callback. It should only be called ONCE.
	If more than once are called, the returned buffers are likely out of
	time order.

Input:
	hWindow - the window handle created earlier with BVDC_Window_Create.

Output:
	pCapturedImage - the last captured buffer associated with hWindow.

Returns:
	BVDC_ERR_CAPTURED_BUFFER_NOT_FOUND - No valid capture buffer to return
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Function succeed

See Also:
	BVDC_Test_Window_ReturnBuffer_isr, BVDC_Test_Window_CapturedImage,
	BVDC_Window_SetUserCaptureBufferCount, BVDC_Display_InstallCallback
	BVDC_Open, BVDC_Window_SetPixelFormat,
**************************************************************************/
BERR_Code BVDC_Test_Window_GetBuffer_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_Test_Window_CapturedImage  *pCapturedImage);

/***************************************************************************
Summary:
	Returns a captured buffer to the associated window in _isr context.

Description:
	This is _isr verion of BVDC_Window_ReturnBuffer. Refer to it for
	description first.

Input:
	hWindow - the window handle created earlier with BVDC_Window_Create.
	pCapturedImage - the captured buffer associated with hWindow.

Output:
	None

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Function succeeds

Note:
	An outstanding capture buffer must be returned and freed if the
	associated window is to be shut down or reconfigured.

See Also:
	BVDC_Test_Window_GetBuffer_isr, BVDC_Test_Window_CapturedImage,
	BVDC_Window_SetUserCaptureBufferCount, BVDC_Display_InstallCallback
	BVDC_Open, BVDC_Window_SetPixelFormat,
**************************************************************************/
BERR_Code BVDC_Test_Window_ReturnBuffer_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_Test_Window_CapturedImage  *pCapturedImage );

/***************************************************************************
Summary:
	Returns RDC scratch registers used by a given GFD.

Description:
	This function returns the RDC scratch registers where the
	surface addresses are stored. This function is typically used in
	applications that generate RULs that need to be ran independent
	of the BRCM driver, eg. CFE or any bootloader, that has a splash
	display feature.

Input:
	hSource - the source handle created. This is associated with the GFD used
	to feed in the surface to the BVN

Output:
	pulScratchReg1, pulScratchReg2 - the returned scratch registers.


Returns:
	BERR_SUCCESS - Function succeeds


See Also:
	BVDC_Source_Create, BVDC_Window_SetPixelFormat,
**************************************************************************/
BERR_Code BVDC_Test_Source_GetGfdScratchRegisters
	( BVDC_Source_Handle               hSource,
	  uint32_t                        *pulScratchReg1,
	  uint32_t                        *pulScratchReg2 );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_TEST_H__ */
/* End of file. */
