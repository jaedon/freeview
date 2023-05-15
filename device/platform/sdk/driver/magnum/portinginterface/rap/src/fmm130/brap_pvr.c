/***************************************************************************
*     Copyright (c) 2005-2008, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_pvr.c $
* $brcm_Revision: Hydra_Software_Devel/12 $
* $brcm_Date: 2/5/08 6:55p $
*
* Module Description: This module contains APIs related to PVR trick modes
*                     of Raptor audio decoder.
*
* Revision History:
* $brcm_Log: /magnum/portinginterface/rap/base/brap_pvr.c $
* 
* Hydra_Software_Devel/12   2/5/08 6:55p srajapur
* PR38789 : [7400,7118,7403,7401] Audio lost on continous switching
* between compressed<->PCM over SPDIF/HDMI along with watchdog
* interrupts(via BBS) for all audio form
* 
* PR 38851 : [7400,7118,7403,7401] Audio lost on continous channel change
* along with watchdog interrupts(via BBS)
* -----------------------------------------------------------------------
* -----
* When Dsp Hardware reset is done during watchdog recovery that time the
* watchdog is triggered.Because of this the firmware is not ack the stop
* and start commands.Added a flag to check the Dsp hardware reset.
* 
* Hydra_Software_Devel/11   2/9/07 10:12a rjain
* PR 26640: if num of frames > 256, force it to 256. Ignore the rest.
* 
* Hydra_Software_Devel/10   1/17/07 3:38p rjain
* PR 26640: For frame advance, if numFrames > 255, pass it to FW in
* chunks of 255.
* 
* Hydra_Software_Devel/9   12/20/06 4:34p rjain
* PR 26640: adding debug messages
* 
* Hydra_Software_Devel/8   11/2/06 2:55p gautamk
* PR24912:  7412 Using REFSW_PREREL_97398_7411E0_20061012 labeled Raptor,
* Pause of DSS stream is not honored immediate
* 
* Hydra_Software_Devel/7   4/11/06 5:43p rjain
* PR 20752: fixing file inclusion warning
* 
* Hydra_Software_Devel/6   10/18/05 6:19p nitinb
* PR 17399: PVR: wrap-around with 'slow' trickmode result in audio noise
* at wrap-around point
* 
* Hydra_Software_Devel/5   8/17/05 1:07p nitinb
* PR 16694: calling BRAP_PVR_FrameAdvance when decode is not started
* results in div by zero error
* 
* Hydra_Software_Devel/4   8/4/05 7:19p nitinb
* PR 16354: FMM need not be touched for PVR pause/resume and frame
* advance features. Removing FMM related code.
* 
* Hydra_Software_Devel/3   7/21/05 12:58p rjain
* PR 16354: FMM related changes for Pause/Resume
* 
* Hydra_Software_Devel/2   7/19/05 6:56p nitinb
* PR 16354: Develope PVR features for RAP PI
* 
* Hydra_Software_Devel/1   7/19/05 4:09p nitinb
* PR16354: Develope PVR features for RAP PI
***************************************************************************/

#include "brap.h"
#include "brap_priv.h"

BDBG_MODULE(rap_pvr);		/* Register software module with debug interface */


/*******************************************************
 * Local defines
 *******************************************************/
 /*******************************************************
 * Local Type definitions
 *******************************************************/
/*******************************************************
 * Static Variables & Functions
 *******************************************************/


/***************************************************************************
Summary: 
	Pause/resume audio

Description: 
	This function pauses/resumes audio. When unpaused (resumed), audio
	starts from the point it paused. This function works only for playback.

Returns:
	BERR_SUCCESS - If pause/resume operation is successful.

See Also: 
	BRAP_PVR_GetPauseStatus
****************************************************************************/
BERR_Code BRAP_PVR_EnablePause(
	BRAP_ChannelHandle		hRapCh, /* [In] The RAP Channel handle */
	bool					bOnOff	/* [In] TRUE = Pause video
											FALSE = Resume video */
	)
{
	BERR_Code err;

	BDBG_ENTER(BRAP_PVR_EnablePause);
	/* Assert the function arguments*/
	BDBG_ASSERT(hRapCh);

    BDBG_MSG(("BRAP_PVR_EnablePause: bOnOff=%d", bOnOff));

	/* If audio is already in required state, return success */
	if ((hRapCh->sTrickModeState.bAudioPaused==bOnOff)
		&& (!(BRAP_P_GetInternalCallFlag(hRapCh))))
		return BERR_SUCCESS;

	/*Check is Hardware is reset */
	if(hRapCh->hRap->bResetHwDone==false)
	{
		/* Pause the DSP */
		err = BRAP_DSPCHN_P_EnablePause(hRapCh->sModuleHandles.hDspCh, bOnOff);
		if (err!=BERR_SUCCESS)
			return BERR_TRACE(err);
	}

	if (bOnOff==true) 
	{
		/* Change "audio paused" state to "true" */
		hRapCh->sTrickModeState.bAudioPaused = true;

	}
	else if (bOnOff==false) 
	{
		/* Clear frame residual time on resume */
		hRapCh->sTrickModeState.uiFrameAdvResidualTime = 0;
		/* Change "audio paused" state to "false" */
		hRapCh->sTrickModeState.bAudioPaused = false;
	}			
	

	BDBG_LEAVE(BRAP_PVR_EnablePause);
	return BERR_SUCCESS;
}

/***************************************************************************
Summary: 
	Get Pause/resume audio status

Description: 
	This function gets the current pause/resume status of the audio. When 
	audio is paused it returns TRUE and when audio is playing it returns 
	FALSE.

Returns:
	BERR_SUCCESS - If pause/resume information is retrieved successfully.

See Also: 
	BRAP_PVR_EnablePause
****************************************************************************/
BERR_Code BRAP_PVR_GetPauseStatus(
	BRAP_ChannelHandle		hRapCh,		/* [In] The RAP Channel handle */
	bool					*pbPauseStatus /* [Out]
										   1 = Audio paused
										   0 = Audio playing
										   */
	)
{
	BDBG_ENTER(BRAP_PVR_GetPauseStatus);
	/* Assert the function arguments*/
	BDBG_ASSERT(hRapCh);
	BDBG_ASSERT(pbPauseStatus);

	/* Return the current pause state for this channel */
	*pbPauseStatus = hRapCh->sTrickModeState.bAudioPaused;


    BDBG_MSG(("BRAP_PVR_GetPauseStatus: bPauseStatus=%d", *pbPauseStatus));

	BDBG_LEAVE(BRAP_PVR_GetPauseStatus);
	return BERR_SUCCESS;
}

/***************************************************************************
Summary: 
	Frame advance trick mode

Description: 
	This function programs audio decoder to perform frame advance trick 
	mode. During frame advance trick mode,audio is muted at the output port.
	Parameter uiFrameAdvTime tells audio decoder how many frames to advance
	in terms of time (in millisecond). 
	This function works only for playback.

Returns:
	BERR_SUCCESS - If frame advance mode is programmed successfully.

See Also:
****************************************************************************/
BERR_Code BRAP_PVR_FrameAdvance (
	BRAP_ChannelHandle		hRapCh,			/* [In] The RAP Channel handle */
	unsigned int			uiFrameAdvTime	/* [In] Frame advance time in msec */
	)
{
	BERR_Code err;
	unsigned int uiNumFrames, uiOneFrameTime;
	bool bDecoderPaused;

	BDBG_ENTER(BRAP_PVR_FrameAdvance);
	/* Assert the function arguments*/
	BDBG_ASSERT(hRapCh);

	/* Return error if audio is not paused */
	if (!hRapCh->sTrickModeState.bAudioPaused) {
		BDBG_ERR(("For Frame Advance audio should be in paused state"));
		return BERR_TRACE(BRAP_ERR_BAD_DEVICE_STATE);
	}

	/* Check if decoder got into "paused" state, i.e. check whether decoder got atleast
	 * one frame of data for calculation of TSM upper threshold. TSM upper threshold
	 * is required for calculating number of frames to be frame advanced. If decoder
	 * is not in "paused" state, then just return. */

	bDecoderPaused = BRAP_DSPCHN_P_GetDecoderPauseState(hRapCh->sModuleHandles.hDspCh);
	if (!bDecoderPaused)
		return BERR_SUCCESS;

	/* Get one audio frame time required for number of frames calculations */
	err = BRAP_DSPCHN_P_GetOneAudioFrameTime(hRapCh->sModuleHandles.hDspCh, &uiOneFrameTime);
	if (err!=BERR_SUCCESS)
		return BERR_TRACE(err);

	/* Translate frame advance time into number of frames to be advanced */
	uiNumFrames = (hRapCh->sTrickModeState.uiFrameAdvResidualTime + uiFrameAdvTime)/uiOneFrameTime;
	hRapCh->sTrickModeState.uiFrameAdvResidualTime = 
		(hRapCh->sTrickModeState.uiFrameAdvResidualTime + uiFrameAdvTime) % uiOneFrameTime;
    BDBG_MSG(("uiFrameAdvTime (from app)=%d, uiOneFrameTime (from register)=%d, uiNumFrames=%d", 
            uiFrameAdvTime, uiOneFrameTime, uiNumFrames));

    /* If number of frames > 256, force it to 256. Ignore the remaining */
	if (uiNumFrames > 255)
	{
        uiNumFrames = 255;
        BDBG_MSG(("uiNumFrames=%d", uiNumFrames));
    }
	/*Check is Hardware is reset */
	if(hRapCh->hRap->bResetHwDone==false)
	{
		err = BRAP_DSPCHN_P_FrameAdvance(hRapCh->sModuleHandles.hDspCh, uiNumFrames);
		if (err!=BERR_SUCCESS)
			return BERR_TRACE(err);
	}
	
	BDBG_LEAVE(BRAP_PVR_FrameAdvance);
	return BERR_SUCCESS;
}

/* End of File */

