/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_pvr.h $
 * $brcm_Revision: Hydra_Software_Devel/19 $
 * $brcm_Date: 7/6/12 12:04p $
 *
 * Module Description:
 *   See Module Overview below
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/include/bxvd_pvr.h $
 * 
 * Hydra_Software_Devel/19   7/6/12 12:04p btosi
 * SW7425-3358: added support for FNRT
 * 
 * Hydra_Software_Devel/18   7/2/12 9:56a btosi
 * SW7425-3358: first cut at the FNRT changes
 * 
 * Hydra_Software_Devel/17   2/1/12 1:58p btosi
 * SW7425-2270: add support for IgnoreNRTUnderflow
 * 
 * Hydra_Software_Devel/16   8/3/10 2:03p btosi
 * SW7400-2870: added BXVD_[Set/Get]PlaybackRate(_isr)
 * 
 * Hydra_Software_Devel/15   7/29/09 2:45p pblanco
 * PR57177: There is no need for a GetFrameAdvanceMode API. Removing it.
 * 
 * Hydra_Software_Devel/14   7/29/09 1:45p pblanco
 * PR57177: Added BXVD_PVR_GetFrameAdvance and
 * BXVD_PVR_GetFrameAdvance_isr function prototypes.
 * 
 * Hydra_Software_Devel/13   6/15/09 2:45p btosi
 * PR55816: reconcile DQT logic with "Too Early" code
 * 
 * Hydra_Software_Devel/12   3/24/09 4:52p nilesh
 * PR52905: Added ISR versions of most get/set functions.  Cleaned up
 * formatting.  Removed critical sections for some get functions.
 * 
 * Hydra_Software_Devel/11   10/22/07 10:39a btosi
 * PR30310: added BXVD_PVR_GetGopTrickMode()
 * 
 * Hydra_Software_Devel/10   8/30/07 4:41p rayshap
 * PR33893: Add support for BXVD_PVR_Set/GetAutoValidateStcOnPause
 * 
 * Hydra_Software_Devel/9   6/13/07 8:08a btosi
 * PR30310: added BXVD_PVR_SetGopTrickMode() to mainline branch
 * 
 * Hydra_Software_Devel/8   1/23/07 2:33p pblanco
 * PR27168: More review changes to documentation.
 * 
 * Hydra_Software_Devel/7   1/17/07 4:26p pblanco
 * PR27168: Incorporated changes suggested in code review on 16-Jan-2007
 * 
 * Hydra_Software_Devel/6   3/28/06 11:48a pblanco
 * PR20274: Removed old trick mode API prototypes.
 * 
 * Hydra_Software_Devel/5   3/24/06 1:35p pblanco
 * PR20274: Interim checkin prior to FW implementation.
 * 
 * Hydra_Software_Devel/4   1/5/06 2:44p pblanco
 * PR16052: Removed deprecated function prototypes.
 * 
 * Hydra_Software_Devel/3   1/3/06 2:42p pblanco
 * PR16052: Added new enum, changed some API prototypes to new forms and
 * removed unused API prototypes.
 * 
 * Hydra_Software_Devel/2   12/14/05 1:50p pblanco
 * PR16052: Changed STC enum to on and off modes only.
 * 
 * Hydra_Software_Devel/1   12/13/05 11:33a pblanco
 * PR16052: Initial check in.
 * 
 ****************************************************************************/
#ifndef BXVD_PVR_H__
#define BXVD_PVR_H__

#ifdef __cplusplus
extern "C" {
#endif

#if 0
}
#endif


/***************************************************************************
Summary: 
	Frame advance mode enum

Description: 
	Enum to specify whether frame advance is frame by frame or field by field

See Also:
	BXVD_PVR_FrameAdvance
****************************************************************************/
typedef enum BXVD_PVR_FrameAdvanceMode
{
   BXVD_PVR_FrameAdvanceMode_eFrame_by_Frame, /* Frame by frame advance. Only 1st 
                                                 field of each frame is displayed */
   BXVD_PVR_FrameAdvanceMode_eField_by_Field  /* Field by field advance. Only 
                                                 one field is advanced */
} BXVD_PVR_FrameAdvanceMode;

/***************************************************************************
Summary: 
	BTP Mode Type

Description: 
	BTP modes can be either implemented as BTP type (Transport) or 
	as BUD type (User Data) trick modes.

Note:
	Currently only BXVD_PVR_BTPMode_eOff is used. This is the value that 
	BXVD_OpenChannel and BXVD_StopDecode sets the channel's state to.

 This enum is deprecated and may be removed in a future revision of PVR. Do
 not use this in new code.

See Also:
	BXVD_PVR_EnableBTPMode
****************************************************************************/
typedef enum BXVD_PVR_BTPMode
{
   BXVD_PVR_BTPMode_eOff, /* Trick modes are off */
   BXVD_PVR_BTPMode_eBTP, /* Trick mode info travels in the
                             Adaptation field of Transport Packets */
   BXVD_PVR_BTPMode_eBUD, /* Trick mode info travels in the
                             User Data with a special marker */
   BXVD_PVR_BTPMode_MaxMode 
} BXVD_PVR_BTPMode;

/***************************************************************************
Summary: 
	Pause/resume video

Description: 
	This function pauses/resumes video. When unpaused (resumed), video 
	starts from the point it paused. This function works only for playback.

Returns:
	BERR_SUCCESS - If pause/resume operation is successful.

See Also: 
	BXVD_PVR_GetPauseStatus
****************************************************************************/
BERR_Code BXVD_PVR_EnablePause
(
   BXVD_ChannelHandle      hXvdCh,        /* [In] XVD channel handle */
   bool                    bEnablePause   /* [In] true=pause, false=resume */
   );

BERR_Code BXVD_PVR_EnablePause_isr
(
   BXVD_ChannelHandle      hXvdCh,        /* [In] XVD channel handle */
   bool                    bEnablePause   /* [In] true=pause, false=resume */
   );

/***************************************************************************
Summary: 
	Gets pause status of video
	
Description: 
	Returns true if paused, false otherwise
****************************************************************************/
BERR_Code BXVD_PVR_GetPauseStatus
(
   BXVD_ChannelHandle      hXvdCh,        /* [In] XVD channel handle */
   bool                   *pbPauseStatus /* [Out] video pause status */
   );

BERR_Code BXVD_PVR_GetPauseStatus_isr
(
   BXVD_ChannelHandle      hXvdCh,        /* [In] XVD channel handle */
   bool                   *pbPauseStatus /* [Out] video pause status */
   );

/***************************************************************************
SW7400-2870:
DEPRECATED: use BXVD_SetPlaybackRate(isr) instead

Summary: 
	Set Slow motion rate

Description: 
	This function programs video decoder to perform slow motion trick mode 
	with slow motion rate that is passed to this function. If rate > 1, 
	then slow motion with that rate. If rate = 1, then normal decode. 
	This is a decoder based trick mode. This function works only for 
	playback.

Returns:
	BERR_SUCCESS - If slow motion rate is programmed successfully.

See Also: 
	BXVD_PVR_GetSlowMotionRate
****************************************************************************/
BERR_Code BXVD_PVR_SetSlowMotionRate
(
   BXVD_ChannelHandle      hXvdCh,      /* [In] XVD channel handle */
   unsigned long           ulRate       /* [In] slow motion rate */
   );

BERR_Code BXVD_PVR_SetSlowMotionRate_isr
(
   BXVD_ChannelHandle      hXvdCh,      /* [In] XVD channel handle */
   unsigned long           ulRate       /* [In] slow motion rate */
   );

/***************************************************************************
SW7400-2870
DEPRECATED: use BXVD_GetPlaybackRate(isr) instead
Summary: 
	Get slow motion rate programmed

Description: This function returns the slow motion rate that is currently
             programmed in the video decoder.

Returns:
	BERR_SUCCESS - If opened XVD is successful.

See Also: 
	BXVD_PVR_SetSlowMotionRate
****************************************************************************/
BERR_Code BXVD_PVR_GetSlowMotionRate 
(
   BXVD_ChannelHandle       hXvdCh,   /* [In] XVD channel handle */
   unsigned long           *pSMRate   /* [Out] slow motion rate */
   );

BERR_Code BXVD_PVR_GetSlowMotionRate_isr
(
   BXVD_ChannelHandle       hXvdCh,   /* [In] XVD channel handle */
   unsigned long           *pSMRate   /* [Out] slow motion rate */
   );

/***************************************************************************
Summary: 
	Set frame advance trick mode

Description: 
	This function programs video decoder to perform frame advance trick 
	mode. Parameter eFrameAdvMode that is passed to this function tells 
	whether frame advance is achieved by advancing frames or fields. 
	This is a decoder based trick mode. This function works only for 
	playback.

Returns:
	BERR_SUCCESS - If frame advance mode is programmed successfully.

See Also:
****************************************************************************/
BERR_Code BXVD_PVR_FrameAdvance 
(
   BXVD_ChannelHandle          hXvdCh,         /* [In] XVD channel handle */
   BXVD_PVR_FrameAdvanceMode   eFrameAdvMode  /* [In] Frame advance mode */
   );

BERR_Code BXVD_PVR_FrameAdvance_isr
(
   BXVD_ChannelHandle          hXvdCh,         /* [In] XVD channel handle */
   BXVD_PVR_FrameAdvanceMode   eFrameAdvMode  /* [In] Frame advance mode */
   );

/***************************************************************************
Summary: 
	Enable/disable field reversal

Description: 
	This function enables/disables field reversal. When enabled fields are
	displayed in the reverse order (bottom/top), this function has only effect
	if source has interlaced format. Usually field reversal is enabled in 
	rewind type of trickmodes.

Returns:
	BERR_SUCCESS - If field reversal is enabled/disabled successfully.

See Also: 
	BXVD_PVR_GetReverseFieldStatus 
****************************************************************************/
BERR_Code BXVD_PVR_EnableReverseFields 
(
   BXVD_ChannelHandle   hXvdCh,   /* [In] XVD channel handle */
   bool                 bEnable	  /* [In] flag to activate field reversal */
   );

BERR_Code BXVD_PVR_EnableReverseFields_isr
(
   BXVD_ChannelHandle   hXvdCh,  /* [In] XVD channel handle */
   bool                 bEnable  /* [In] flag to activate field reversal */
   );

/***************************************************************************
Summary: 
	Get current status of field reversal

Description: 
	This function returns current status of field reversal

Returns:
	BERR_SUCCESS - If status was returned successfully.

See Also: 
	BXVD_PVR_GetReverseFieldStatus 
****************************************************************************/
BERR_Code BXVD_PVR_GetReverseFieldStatus 
(
   BXVD_ChannelHandle   hXvdCh,   /* [In] XVD channel handle */
   bool                *pbEnable  /* [Out] Status of field reversal */
   );

BERR_Code BXVD_PVR_GetReverseFieldStatus_isr
(
   BXVD_ChannelHandle   hXvdCh,  /* [In] XVD channel handle */
   bool                *pbEnable /* [Out] Status of field reversal */
   );

/***************************************************************************
Summary: 
  Enable or disable host sparse mode.

Description: 
  This function enables host sparse mode when bSparseMode is true or disables
  it when bSparseMode is false. For AVC this function is used to tell the 
	 decoder it is in a host trick mode... it will have no effect on a non-H264
  stream. Calling this function on valid AVC stream will result in the decode
  stopping then restarting.

Returns:
	BERR_SUCCESS - If status was returned successfully.

See Also: 
  BXVD_PVR_GetHostSparseMode
****************************************************************************/
BERR_Code BXVD_PVR_SetHostSparseMode
(
   BXVD_ChannelHandle hXvdCh, /* [In] XVD channel handle */
   bool bSparseMode           /* [In] Sparse mode enable flag */
   );

/***************************************************************************
Summary: 
  Return the current host sparse mode setting.

Description: 
  This function returns a boolean value indicating the current setting of 
  the host sparse mode flag (true/false).

Returns:
	BERR_SUCCESS - If status was returned successfully.

See Also: 
  BXVD_PVR_SetHostSparseMode
****************************************************************************/
BERR_Code BXVD_PVR_GetHostSparseMode
(
   BXVD_ChannelHandle hXvdCh, /* [In] XVD channel handle */
   bool *pbSparseMode         /* [Out] Current sparse mode status */
   );

/***************************************************************************
Summary: 
  Enable or disable GOP reverse trick mode.

Description: 
  This function sets the flag which enables/disables the DQT mode.  
  In this mode, the DM displays the pictures within a GOP in reverse order.
  It takes effect when "BXVD_StartDecode()" is called.

Returns:
	BERR_SUCCESS - If status was returned successfully.

See Also: 
    BXVD_PVR_GetGopTrickMode
****************************************************************************/
BERR_Code BXVD_PVR_SetGopTrickMode
(
   BXVD_ChannelHandle hXvdCh, /* [In] XVD channel handle */
   bool bEnableTrickMode         /* [In] enable/disable DQT trickmode */
   );

BERR_Code BXVD_PVR_SetGopTrickMode_isr
(
   BXVD_ChannelHandle hXvdCh, /* [In] XVD channel handle */
   bool bEnableTrickMode         /* [In] enable/disable DQT trickmode */
   );

/***************************************************************************
Summary: 
  Return the current setting of the DQT flag.

Description: 
  This function returns a boolean value indicating the current setting of 
  the DQT (Display Queue Trick) mode flag.


Returns:
	BERR_SUCCESS - If status was returned successfully.

See Also: 
    BXVD_PVR_SetGopTrickMode
****************************************************************************/
BERR_Code BXVD_PVR_GetGopTrickMode
(
   BXVD_ChannelHandle hXvdCh, /* [In] XVD channel handle */
   bool * pbEnableTrickMode         /* [Out] enable/disable DQT trickmode */
   );

BERR_Code BXVD_PVR_GetGopTrickMode_isr
(
   BXVD_ChannelHandle hXvdCh, /* [In] XVD channel handle */
   bool * pbEnableTrickMode         /* [Out] enable/disable DQT trickmode */
   );


/***************************************************************************
Summary: 
  Enable or disable automatic validation of the STC when transitioning from
  pause to play (AutoValidateStcOnPause).

Description: 
  By default, the display manager will try to automatically determine if the 
  STC is valid when transitioning from pause to play by waiting for "TSM pass" in 
  processing stamp management.  AutoValidateStcOnPause will cause the display manager 
  to pause indefinately waiting for TSM to pass, and may cause video to stop.

  Disabling this feature will cause the display manager to process TSM normally during
  the transition, but may also result in some pictures being discarded if the STC is
  not properly set or set in time.

  Making this call will have no effect when done during trick mode transition from pause to 
  play.  The call must be made before playback is "unpaused".

Returns:
	BERR_SUCCESS - If status was returned successfully.

See Also: 
  BXVD_PVR_GetAutoValidateStcOnPause
****************************************************************************/
BERR_Code BXVD_PVR_SetAutoValidateStcOnPause
(
   BXVD_ChannelHandle hXvdCh,     /* [In] XVD channel handle */
   bool bAutoValidateStcOnPause   /* [In] Enable/disable flag */
   );

BERR_Code BXVD_PVR_SetAutoValidateStcOnPause_isr
(
   BXVD_ChannelHandle hXvdCh,     /* [In] XVD channel handle */
   bool bAutoValidateStcOnPause   /* [In] Enable/disable flag */
   );

/***************************************************************************
Summary: 
  Return the current host AutoValidateStcOnPause setting.

Description: 
  This function returns a boolean value indicating the current setting of 
  the AutoValidateStcOnPause (true/false).

Returns:
	BERR_SUCCESS - If status was returned successfully.

See Also: 
  BXVD_PVR_SetAutoValidateStcOnPause
****************************************************************************/
BERR_Code BXVD_PVR_GetAutoValidateStcOnPause
(
   BXVD_ChannelHandle hXvdCh,     /* [In] XVD channel handle */
   bool *pbAutoValidateStcOnPause /* [Out] Current sparse mode status */
   );

BERR_Code BXVD_PVR_GetAutoValidateStcOnPause_isr
(
   BXVD_ChannelHandle hXvdCh,     /* [In] XVD channel handle */
   bool *pbAutoValidateStcOnPause /* [Out] Current sparse mode status */
   );

/***************************************************************************

  SW7425-2270:

  The application will call SetIgnoreNRTUnderflow when it determines that an 
  NRT underflow is actually a gap in the content (e.g. slideshow or end of 
  stream) and the repeated picture should actually be encoded.

  When SetIgnoreNRTUnderflow=true, the "decoder underflow" scenario should 
  be ignored until either: 
  - the underflow condition ends 
  - the app explicitly sets SetIgnoreNRTUnderflow=false 
 
  Note: only the "decoder underflow" condition is ignored. All other NRT cenarios 
  (e.g. "Other Transcode Stalled", "FIC Stall", etc) are still in effect. 
 
****************************************************************************/

BERR_Code BXVD_PVR_SetIgnoreNRTUnderflow(
   BXVD_ChannelHandle hXvdCh,
   bool bIgnoreNRTUnderflow
   );

BERR_Code BXVD_PVR_SetIgnoreNRTUnderflow_isr(
   BXVD_ChannelHandle hXvdCh,
   bool bIgnoreNRTUnderflow
   );

BERR_Code BXVD_PVR_GetIgnoreNRTUnderflow(
   BXVD_ChannelHandle hXvdCh,
   bool * pbIgnoreNRTUnderflow
   );

BERR_Code BXVD_PVR_GetIgnoreNRTUnderflow_isr(
   BXVD_ChannelHandle hXvdCh,
   bool * pbIgnoreNRTUnderflow
   );


/***************************************************************************
   SW7425-3358: support for FNRT.
****************************************************************************/

#define BXVD_PVR_FNRTSettings  BXDM_PictureProvider_FNRTSettings 

BERR_Code BXVD_PVR_SetFNRTSettings(
   BXVD_ChannelHandle hXvdCh,
   const BXVD_PVR_FNRTSettings * pstFNRTSettings
   );

BERR_Code BXVD_PVR_SetFNRTSettings_isr(
   BXVD_ChannelHandle hXvdCh,
   const BXVD_PVR_FNRTSettings * pstFNRTSettings
   );

BERR_Code BXVD_PVR_GetFNRTSettings(
   BXVD_ChannelHandle hXvdCh,
   BXVD_PVR_FNRTSettings * pstFNRTSettings
   );

BERR_Code BXVD_PVR_GetFNRTSettings_isr(
   BXVD_ChannelHandle hXvdCh,
   BXVD_PVR_FNRTSettings * pstFNRTSettings
   );

BERR_Code BXVD_PVR_GetDefaultFNRTSettings(
   BXVD_ChannelHandle hXvdCh,
   BXVD_PVR_FNRTSettings * pstFNRTSettings
   );

BERR_Code BXVD_PVR_GetDefaultFNRTSettings_isr(
   BXVD_ChannelHandle hXvdCh,
   BXVD_PVR_FNRTSettings * pstFNRTSettings
   );


/*******************/
/* Deprecated APIs */
/*******************/

/***************************************************************************
Summary: 
  [DEPRECATED] Enable or disable BUD trick mode.
Description: 
  This function enables BUD trick mode when bBudMode is true or disables
  it when bBudMode is false.

Returns:
	BERR_SUCCESS - If status was returned successfully.

See Also: 
      BXVD_PVR_GetBUDMode
****************************************************************************/
BERR_Code BXVD_PVR_SetBUDMode
(
   BXVD_ChannelHandle hXvdCh, /* [In] XVD channel handle */
   bool bBudMode              /* [In] BUD mode enable flag */
   );

/***************************************************************************
Summary:
  [DEPRECATED] Get the current setting of the BUD trick mode flag.

Description: 
  This function returns a boolean value indicating the current setting of
  BUD trick mode enable flag (true/false).

Returns:
	BERR_SUCCESS - If status was returned successfully.

See Also: 
  BXVD_PVR_SetBUDMode
****************************************************************************/
BERR_Code BXVD_PVR_GetBUDMode
(
   BXVD_ChannelHandle hXvdCh, /* [In] XVD channel handle */
   bool *pbBudMode            /* [Out] Current BUD mode status */
   );

#ifdef __cplusplus
}
#endif

#endif /* BXVD_PVR_H__ */
/* End of file. */
