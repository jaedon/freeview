/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_int_priv.c $
* $brcm_Revision: Hydra_Software_Devel/146 $
* $brcm_Date: 5/24/12 8:22p $
*
* Module Description:
*   Module name: INT
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_int_priv.c $
* 
* Hydra_Software_Devel/146   5/24/12 8:22p sgadara
* SW7435-174: [7550] Support Destination sample rate info for 7550.
* Fixing build issue as well
* 
* Hydra_Software_Devel/145   5/17/12 3:27p sgadara
* SW7435-174: [7125] Extending Destination Sample Rate info return
* support to Ringbuffer destinations
* 
* Hydra_Software_Devel/144   2/17/12 7:26p sgadara
* SW7420-2194: [7420] Adding new feature, Mixing of two independent
* decode outputs. Also refactoring SRC programming
* 
* Hydra_Software_Devel/143   6/7/11 1:37p gautamk
* SW7420-1885:[7420] Changing SRC CFG from Lint to Bypass on the fly
* causes SRC to go in invalid state. So Instead of changing to Bypass,
* keep it in Lint and program NUm and Den accordingly.
* 
* Hydra_Software_Devel/142   5/24/11 5:02p purush
* SW7420-1895: [7420] Added missing code in watchdog recovery case in
* function BRAP_P_InterruptUnInstall and BRAP_P_InterruptInstall for
* RAP_MULTISTREAM_DECODER_SUPPORT.
* 
* Hydra_Software_Devel/141   4/20/11 3:43p sgadara
* SW7420-1756: [7420] Implementing code review comments --> Change the
* Pointer validity check function to a macro and name it aptly
* 
* Hydra_Software_Devel/140   4/1/11 6:00p gautamk
* SW7420-1748: [7420]If Raptor is stopped do not call
* BRAP_P_FMMBF_SourceBuf_isr.Also Installing callback on
* BRAP_P_FMMBF_SourceBuf_isr for only PCM PB.
* 
* Hydra_Software_Devel/139   3/18/11 9:42p sgadara
* SW7420-767: [7420] Merge MS11 development branch to mainline
* 
* Hydra_Software_Devel/138   3/7/11 6:06p pothiraj
* SW7550-648: [7550] Secure Code Module feature
* 
* Hydra_Software_Devel/137   2/7/11 2:45p gautamk
* SW7550-637: [7550] pAssing HDMI sampling rate enum correctly.
* 
* Hydra_Software_Devel/136   2/7/11 1:59p gautamk
* SW7550-631: [7550] Sample rate value and mode value in
* Getstreaminformation() is taken from payload sent by FW through
* interrupt.
* 
* Hydra_Software_Devel/135   2/1/11 5:04p gautamk
* SW7550-637: [7550] Adding PI support for multiple sampling frequency
* support.
* 
* Hydra_Software_Devel/134   10/28/10 6:03p sgadara
* SW7405-4066: [7405,3548] Updating SRC type in data structures before
* SRC Coeff Alloc call.
* 
* Hydra_Software_Devel/133   10/28/10 5:23p sgadara
* SW3556-1207: [3548,3556] Implement isr version of Equalizer
* coefficients programming
* 
* Hydra_Software_Devel/132   10/26/10 7:06p sgadara
* SW3556-1207: [3548,3556] Implement isr version of Equalizer
* coefficients programming
* 
* Hydra_Software_Devel/131   10/13/10 5:48p sgadara
* SW7405-4066: [7405,3548] Implementing code review comments --> Updating
* SRC type in data structures for 7405 as well, and do it after the
* register programming is done.
* 
* Hydra_Software_Devel/130   9/6/10 5:17p gautamk
* SW7550-459: [7550] Merging GFX support in mainline
* 
* Hydra_Software_Devel/129   8/31/10 2:42p gautamk
* SW7420-994: [7420] Fixing Mute status for 5.1 PCM.
* 
* Hydra_Software_Devel/128   8/4/10 7:22p gautamk
* SW7405-4713: [7405] Whenever there is a change Output clock rate, SRC
* of running PB and cap should be updated.
* 
* Hydra_Software_Devel/127   7/29/10 1:32p gautamk
* SW7125-540: [7125] 7125 have 9 SrCCh
* 
* Hydra_Software_Devel/126   7/23/10 12:13p gautamk
* SW7400-2848: [7405] Fixing compilation
* 
* Hydra_Software_Devel/125   7/22/10 5:58p gautamk
* SW7400-2848: [7405] Adding support for on the fly input sample rate
* programming for PB channel.
* 
* Hydra_Software_Devel/RAP_GFX_SUPPORT_PH6P0/3   8/13/10 2:55p gautamk
* SW7550-519: [7550] Interrupt install/uninstall handled for GFX case
* 
* Hydra_Software_Devel/RAP_GFX_SUPPORT_PH6P0/2   8/9/10 5:39p gautamk
* SW7550-459: [7550] Meging GFX support to Ph6p0 of 7550
* 
* Hydra_Software_Devel/RAP_GFX_SUPPORT_PH6P0/1   8/9/10 5:16p gautamk
* SW7550-459: [7550] Meging GFX support to Ph6p0 of 7550
* 
* Hydra_Software_Devel/RAP_GFX_SUPPORT/4   7/23/10 7:29p gautamk
* SW7550-459: [7550] Adding Support for GFx in RAP
* 
* Hydra_Software_Devel/RAP_GFX_SUPPORT/3   7/12/10 12:27p gautamk
* SW7550-459: [7550] Adding Support for GFx in RAP
* 
* Hydra_Software_Devel/RAP_GFX_SUPPORT/2   7/7/10 7:20p gautamk
* SW7550-459: [7550] Adding Support for GFx in RAP
* 
* Hydra_Software_Devel/RAP_GFX_SUPPORT/1   6/16/10 3:20p gautamk
* SW7550-459: [7550] Adding Support for GFx in RAP
* 
* Hydra_Software_Devel/123   5/19/10 3:29p gautamk
* SW7405-3063: [7405] checking the validity of ptrs instead of checking
* NULL
* 
* Hydra_Software_Devel/122   5/17/10 5:30p gautamk
* SW7405-4327: [7405] Muting the decode channel, only if there is any PCM
* PB/CAP channel in the association.
* 
* Hydra_Software_Devel/121   5/6/10 5:53p gautamk
* SW7405-4058: [7405] Adding BRAP_Interrupt_eUnlicensedAlgo interrupt
* support
* 
* Hydra_Software_Devel/120   4/27/10 6:16p gautamk
* SW7405-4253:[7405] Trigger DestinationSample rate change interrupt when
* sample rate change interrupt is triggered for all Destination attached
* to that hRapch.
* 
* Hydra_Software_Devel/119   4/23/10 4:51p gautamk
* SW7405-3063:[7405] Fixing a bug for Fixed output sample rate.
* 
* Hydra_Software_Devel/118   4/22/10 5:12p sgadara
* SW7405-4170:[7405,3548,3556] Making chages to internally map AACHE and
* AC3 Encode to Dolby Pulse and DD Transcode in PI for MS10 license
* 
* Hydra_Software_Devel/117   4/15/10 6:00p sgadara
* SW3548-2852: [3548,3556] Improving the error handling mechanism.
* 
* Hydra_Software_Devel/116   4/12/10 4:17p gautamk
* SW7405-3063: [7405] Fixing the logic to clear the Status bit
* 
* Hydra_Software_Devel/115   3/10/10 5:06p speter
* SW3548-2819:[3548] Adding Dolby Pulse Algorithm
* 
* Hydra_Software_Devel/114   2/17/10 11:07a sgadara
* SW7325-591: [7335] Adding feature to keep output sample rate to 48khz,
* irrespective of stream sample rate. Porting to mainline
* --> Fixing build issue for 48kz capture of 3548/56 platform
* 
* Hydra_Software_Devel/113   2/12/10 5:11p gautamk
* SW7325-591: [7335] Adding feature to keep output sample rate to 48khz,
* irrespective of stream sample rate. Porting to mainline
* 
* Hydra_Software_Devel/112   2/8/10 4:19p sgadara
* SW3548-2739-2686: [3548,3556] Modifying the Equalizer implementation
* design to support PEQ, and GEQ+Tone Control.
* --> After Mixer 2 SRCs will be connected back to back if Eq is added
* 
* Hydra_Software_Devel/111   1/29/10 3:46p sgadara
* SW3556-1023: [3548,3556] Updating the Input and Output Sampling rate
* info in SRC handle
* 
* Hydra_Software_Devel/110   1/7/10 10:43p sgadara
* SW3556-905: [3548,3556] Adding "Capture of Mixed Audio" feature
* --> Handling different combinations/usage cases of mixing
* 
* Hydra_Software_Devel/109   12/23/09 9:32p sgadara
* SW3556-904: [3548,3556] Adding feature of "Fixed Ringbuffer Capture at
* 48khz sample rate"
* 
* Hydra_Software_Devel/108   12/23/09 3:48p sgadara
* SW3556-904: [3548,3556] Adding feature of "Fixed Ringbuffer Capture at
* 48khz sample rate"
* --> Adding function to return SRC Coefficient Index
* 
* Hydra_Software_Devel/107   12/17/09 9:16p sgadara
* SW3556-904: [3548,3556] Adding feature of "Fixed Ringbuffer Capture at
* 48khz sample rate"
* 
* Hydra_Software_Devel/106   11/30/09 12:39p gautamk
* SW7405-3063: [7405] Populating TSm structure.
* 
* Hydra_Software_Devel/105   11/10/09 2:35p speter
* SW7405-2959: [3548] Adding DTS-HD support
* 
* Hydra_Software_Devel/104   11/6/09 4:39p gautamk
* SW7405-3063: [7405] Correcting debug message
* 
* Hydra_Software_Devel/103   10/27/09 12:42p gautamk
* SW7405-3110: [7405]Adding Destination sample rate change interrupt.
* This will need to be installed for HDMI. Whenver sample rate changes,
* this interrupt gets trigerred. So app need to install callback
* function on it to program HDMI rx sample rate. Checkin at mainline
* 
* Hydra_Software_Devel/102   10/21/09 5:02p gautamk
* SW7335-609:[7405] Setting hTask-bStopped appropriately in Isr Ctxt when
* Event is getting set. This is done to fix race condition. Porting to
* mainline
* 
* Hydra_Software_Devel/101   10/2/09 2:34a srajapur
* SW7550-52: [7550] Merging 7550 branch code to main line
* 
* Hydra_Software_Devel/100   9/24/09 4:02p gautamk
* SW7405-3063: [7405] Adding CDBITB UNderflow/Overflow interrupt
* 
* Hydra_Software_Devel/99   9/22/09 5:48p gautamk
* SW7405-3058: [7405] Adding support for DRA decoder.
* 
* Hydra_Software_Devel/98   9/3/09 7:30p sgadara
* SW7405-2928: [7405] Adding Multichannel capture support.
* --> To pass the callback the channel pair for which the fullmark
* interrupt has been triggered and also removing the interrupt clear as
* it is done in relevant APIs of brap_cap.c
* 
* Hydra_Software_Devel/97   8/27/09 2:43p gautamk
* SW7405-2928: [7405] Adding support Multichannel capture.
* 
* Hydra_Software_Devel/96   8/25/09 1:06p gautamk
* SW7405-2928: [7405] Adding support Multichannel capture.
* 
* Hydra_Software_Devel/95   8/17/09 5:13p gautamk
* PR57743:[7405] using Pvt functions when calling from internal RAP PI
* 
* Hydra_Software_Devel/94   6/30/09 9:01p sgadara
* PR 56011: [3548,3556] Implementing the code review comments.
* --> For Linear Interpolation SRC type, while scaling down the sample
* rate values make sure there is no loss of information
* For e.g., 48000 can be scaled to 480 but 22050 to 2205.
* 
* Hydra_Software_Devel/93   6/30/09 1:23p gautamk
* PR56270: [7405] Programming SRC for linear interpolation correctly.
* 
* Hydra_Software_Devel/92   6/12/09 3:40p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/91   6/10/09 3:07p gautamk
* PR55135: [7405] fixing compilation
* 
* Hydra_Software_Devel/90   6/10/09 3:04p gautamk
* PR55135: [7405] fixing compilation for 3548
* 
* Hydra_Software_Devel/89   6/9/09 12:03p gautamk
* PR55135: [7405] If sample rate changes on the fly, SRC of PB and CAP
* should also get configured accordingly
* 
* Hydra_Software_Devel/88   5/27/09 3:40p gautamk
* PR53283: [7405] Fixing compilation for 3548
* 
* Hydra_Software_Devel/87   5/22/09 4:42p gautamk
* PR55135: [7405] Moving SRC programming of PB/CAP from Samplerate_isr to
* RampEnable_isr
* 
* Hydra_Software_Devel/86   5/19/09 12:14p gautamk
* PR52383: [7405] Adding mode change interrupt support for DTS Brodcast.
* 
* Hydra_Software_Devel/85   5/19/09 11:53a gautamk
* PR52383: [7405] fixing coverity issue.
* 
* Hydra_Software_Devel/84   5/14/09 4:22p gautamk
* PR50455: [7405]Adding APIs BRAP_GetMixerRampStepSize()
* BRAP_SetMixerRampStepSize()
* BRAP_GetSrcRampStepSize()
* BRAP_SetSrcRampStepSize()
* 
* Hydra_Software_Devel/83   5/14/09 1:41p gautamk
* PR53560: [7405] No-ack is required if Mask/Unmaskinterrupt function is
* called from interrupt ctxt.
* 
* Hydra_Software_Devel/82   5/13/09 2:51p gautamk
* PR53560: [7405] fixing Stream info avaialable interrupt support.
* 
* Hydra_Software_Devel/81   5/12/09 3:55p gautamk
* PR53560: [7405] Adding Stream info avaialable interrupt support.
* 
* Hydra_Software_Devel/80   5/12/09 3:37p gautamk
* PR54608:[7405] Adding API BRAP_GetTotalErrorCount(), which returns sum
* of following errors
*        PTS_Error + Crc_Error + Dec_unlock +TSM_Fail
* 
* Hydra_Software_Devel/79   5/12/09 1:37p gautamk
* PR53560: [7405] Adding Stream info avaialable interrupt.
* 
* Hydra_Software_Devel/78   4/20/09 3:56p gautamk
* PR52383: [7405] Creating seperate event handles for each task.
* 
* Hydra_Software_Devel/77   3/23/09 2:57p gautamk
* PR53419: [7405] non-isr function hsould not be called from isr
* functions.
* 
* Hydra_Software_Devel/76   3/17/09 2:39p gautamk
* PR52383: [7405] correcting programming of SRC for PB.
* 
* Hydra_Software_Devel/75   3/17/09 11:53a gautamk
* PR52383: [7405] correcting programming of SRC for PB.
* 
* Hydra_Software_Devel/74   3/16/09 6:09p sgadara
* PR 52754: [3548,3556] Adding the code for proper programming of SRC for
* playback and capture channels during mixing with decode channel.
* 
* Hydra_Software_Devel/73   3/16/09 4:14p gautamk
* PR52383: [7405] When programming SRC of PB channel do not check if
* already in Lint mode.
* 
* Hydra_Software_Devel/72   2/25/09 12:15p gautamk
* PR52383: [7405] Correcting the call sequence of Enable/Disabel Callback
* of Int module as expected by this module
* 
* Hydra_Software_Devel/71   2/25/09 12:12p gautamk
* PR52383: [7325] Fixed bug with index overflow of aray
* 
* Hydra_Software_Devel/68   2/5/09 12:48p gautamk
* PR48850: [7405] correcting debug messages
* 
* Hydra_Software_Devel/67   2/2/09 3:08p gautamk
* PR48850: [7405]Changing BDBG_ERR to  BDBG_MSG
* 
* Hydra_Software_Devel/66   1/27/09 3:30p speter
* PR 50495: [3548,3556] Merging the customer branch to main line
* 
* Hydra_Software_Devel/65   1/23/09 11:36a gautamk
* PR50455: [7405] Programming SRC_CTRL0_RAMP_SCALE at Ramp enable time.
* 
* Hydra_Software_Devel/64   1/22/09 5:28p gautamk
* PR50455: [7405] Setting VOLUME STEP RAMP to 0x20 when DSP FW triggers
* EnableRamp interrupt.
* 
* Hydra_Software_Devel/63   12/19/08 6:01p gautamk
* PR48850: [7405] Fixing workaround of corruption happening in
* sAudProcessingStage[i][j].hAudioProcessing
* 
* Hydra_Software_Devel/62   12/18/08 2:17p gautamk
* PR48850: [7405] Making  modification in mode_change_isr to populate the
* union element correctly.
* 
* Hydra_Software_Devel/61   12/15/08 4:08p gautamk
* PR50333: [7405] printing  warning messages when AAC-DTS xcode is
* atttempted on non 48Khz stream
* 
* Hydra_Software_Devel/RAP_354856_PH_4_1/1   1/19/09 3:27p sgadara
* PR 50859: [3548,3556] Adding the code to trigger watchdog whenever
* there is a timeout error.
* Also adding the code to program the SRC registers properly for linear
* and bypass src types for playback and capture channels.
* 
* Hydra_Software_Devel/60   11/25/08 7:22p sgadara
* PR 44771: [3548,3556] Adding the code to write to status clear
* registers for FMM destination based interrupts.
* 
* Hydra_Software_Devel/59   11/19/08 5:05p gautamk
* PR49167: [7405] OutSr for Playback channel isavailable only when
* channel is started. Fixing this bug .
* 
* Hydra_Software_Devel/58   10/29/08 3:25a speter
* PR 44771: [3548,3556] Adding interrupt for Ring Buffer Overflow and
* cleaning up base3 code to remove 3563 and 7440 specific code
* 
* Hydra_Software_Devel/57   10/28/08 11:14p speter
* PR 48086: [3548,3556] Fixing the Playback case when the sampling rate
* of main decoder channel has not yet arrived till the playback channel
* is started.
* 
* Hydra_Software_Devel/56   10/21/08 6:22p gautamk
* PR42213:[7405] Allocating only 1 SRC for Playback/Capture. It can
* handle Upsample by 4x and Downsample by 2x.
* 
* Hydra_Software_Devel/55   10/20/08 2:42p gautamk
* PR43454: [7405] If the task in the stop state OR task id is invalid,
* Handle this case appropriately.
* 
* Hydra_Software_Devel/54   10/20/08 2:25p gautamk
* PR47961: [7405] Fixing warning and compilation for 7405 platform due to
* changes of this PR.
* 
* Hydra_Software_Devel/53   10/18/08 3:29a speter
* PR 47360: [3548,3556]: Changing the design of Spdif Rx. SPDIF Rx is now
* a device level interrupt and handling of digital input format change
* is moved from RAP PI to higher layer.
* 
* Hydra_Software_Devel/52   10/17/08 6:02a speter
* PR 47360: [3548,3556]: Changing the design of Spdif Rx. SPDIF Rx is now
* a device level interrupt and handling of digital input format change
* is moved from RAP PI to higher layer.
* 
* Hydra_Software_Devel/51   10/17/08 2:30a speter
* PR 47360: [3548,3556]: Changing the design of Spdif Rx. SPDIF Rx is now
* a device level interrupt and handling of digital input format change
* is moved from RAP PI to higher layer.
* 
* Hydra_Software_Devel/50   10/15/08 9:59p speter
* PR42213: [3548,3556] Fixing compilation break for 3548 family
* 
* Hydra_Software_Devel/49   10/13/08 5:45p gautamk
* PR42213: [7405] Programming SRC for the PB when O/P sample rate is
* different from input, During decode's sample rate interrupt.
* 
* Hydra_Software_Devel/48   10/10/08 12:11p speter
* PR 45444: [7405] Fixing compilation break
* 
* Hydra_Software_Devel/47   10/10/08 10:06a speter
* PR 47764: [3548,3556] Merging the release branch 3.0 to main line of
* development
* 
* Hydra_Software_Devel/46   9/30/08 9:01p gautamk
* PR43454: [7405] Correcting BRAP_P_Watchdog_isr function.
* 
* Hydra_Software_Devel/RAP_354856_PH_3_0/1   10/9/08 11:14a speter
* PR 45444: [3548,3556] Adding the equalizer code for Sampling rate
* 
* Hydra_Software_Devel/45   9/19/08 8:52p sgadara
* PR 44770: [3548,3556] Implementing the Destination based Interrupts for
* 'capture to ringbuffer' feature.
* 
* Hydra_Software_Devel/44   9/11/08 3:01p gautamk
* PR43454: [7405] written seperate function for various  interrupt Isrs.
* 
* Hydra_Software_Devel/43   7/28/08 9:46a speter
* PR 45171: [3548,3556] Merging from Phase 2.0 release to main line of
* development (Hydra_Software_Devel)
* 
* Hydra_Software_Devel/RAP_354856_PH_2_0/2   7/24/08 10:22a speter
* PR 45032: [3548,3556] Add DSP support to capture channel (I2S In and
* ADC In)
* 
* Hydra_Software_Devel/RAP_354856_PH_2_0/1   7/23/08 1:38p speter
* PR 44775: [3548,3556] Fixing the auto switch interrupts by turning them
* off. Also making warnings as messages for RF Audio
* 
* Hydra_Software_Devel/42   7/17/08 4:23p sgadara
* PR 44895: [3548] To bring up 'capture to ringbuffer' feature on 3548.
* 
* Hydra_Software_Devel/41   7/9/08 10:19a speter
* PR 44660:[3548,3556] Merging the Phase 1.5 release branch code to main
* line of development
* 
* Hydra_Software_Devel/40   7/3/08 11:29p gautamk
* PR42605: [7405] fixing coverity issues for 7405
* 
* Hydra_Software_Devel/39   6/11/08 7:31a speter
* PR38950: [3548,3556] Changing generic use of BTSC as RF Audio
* 
* Hydra_Software_Devel/38   6/10/08 8:56p gautamk
* PR43472: [7405] Adding interrupt support for TSM fail interrupt.
* 
* Hydra_Software_Devel/37   6/5/08 2:15p gautamk
* PR41726: [7405] fixing segmentation fault
* 
* Hydra_Software_Devel/36   6/4/08 11:08p gautamk
* PR43314:[7405] Segmentataion fault occurs on trigerring watchdog(via
* BBS) with playback in pause state
* PR43312:[7405]
* 
* Hydra_Software_Devel/35   5/28/08 9:20p speter
* PR 39442: [3548,3556] Enabling interrupts for BTSC module
* 
* Hydra_Software_Devel/34   5/21/08 7:20p gdata
* PR41726: [7405] Implementing review comments for Interrputs
* 
* Hydra_Software_Devel/33   5/20/08 7:10p gautamk
* PR42807: [7335] Fixing the error condition incase of watchdog recovery
* 
* PR42837:[7325]
* 
* Hydra_Software_Devel/32   5/13/08 6:28p gautamk
* PR42504:[7405] Not sending task command if the stop command for task
* already sent.
* 
* Hydra_Software_Devel/31   5/9/08 4:17p gautamk
* PR42302:[7405] If watchdogISR is trigerred in between when PI waiting a
* response/ack from FW DSP, Then PI shoul exit gracefully. And then
* ProcessWatchdogRecovery() function should be called
* 
* PR42504:[7405]
* 
* Hydra_Software_Devel/30   5/9/08 1:29p speter
* PR 38950: [3548,3556] Adding SpdifRx support
* 
* Hydra_Software_Devel/29   5/5/08 11:40a speter
* PR 38950: [3548] Making 3548_FAMILY macro and using it for both 3548
* and 3556 cases.
* 
* Hydra_Software_Devel/28   4/30/08 7:41p gautamk
* PR42335: [7405] downgrade non-error BDBG_ERR's to either WRN or MSG
* 
* Hydra_Software_Devel/27   4/28/08 4:29p speter
* PR 38950: [3548] Fixing compilation for 3548/3556.
* 
* Hydra_Software_Devel/26   4/22/08 2:00p gautamk
* PR41994: [7405] Using BCHP_VER_ Correctly...
* 
* Hydra_Software_Devel/25   4/16/08 6:03p gautamk
* PR40920: [7405] Adding PI support for ASTM on new arch code (base3).
* 
* Hydra_Software_Devel/24   4/16/08 12:07p gautamk
* PR41726: [7405] Coverity fixes.
* 
* Hydra_Software_Devel/23   4/14/08 11:38a sushmit
* PR 34648: [7405] Reducing Stack Usage
* 
* Hydra_Software_Devel/22   4/8/08 2:22p gautamk
* PR34648: [7405] Merging wdog code to mainline
* 
* Hydra_Software_Devel/RAP_NEWFWARCH_WDOG/3   4/3/08 2:24p gautamk
* PR34648:[7405] Adding code for wdog
* 
* Hydra_Software_Devel/RAP_NEWFWARCH_WDOG/2   4/1/08 2:42p gautamk
* PR34648: [7405] Adding changes in WDOg branch
* 
* Hydra_Software_Devel/RAP_NEWFWARCH_WDOG/1   3/28/08 4:02p gautamk
* PR34648: [7405] adding code for watchdog recovery in a branch
* 
* Hydra_Software_Devel/21   3/18/08 12:26p sushmit
* PR 34648: [7405] Reading all async messages on one interrupt
* 
* Hydra_Software_Devel/20   3/13/08 11:28a sushmit
* PR 34648: [7405] Fixing Broken PCM Playback
* 
* Hydra_Software_Devel/19   3/12/08 6:21p sushmit
* PR 34648: [7405] Fixing passing of sampling rate info to APP
* 
* Hydra_Software_Devel/18   3/3/08 5:26p sushmit
* PR34648: [7405] Updating SR code & BRAP_FlushChannel
* 
* Hydra_Software_Devel/17   3/3/08 2:20p gdata
* PR40134: [7405] Changes in remove application callback for DSP
* interrupts
* 
* Hydra_Software_Devel/16   3/3/08 1:57p sgadara
* PR 40138: [7405] To get Autotrap up on New Architecture
* 
* Hydra_Software_Devel/15   3/3/08 11:10a gdata
* PR40134: [7405] Making the changes to support the install interrupt for
* open channel
* 
* Hydra_Software_Devel/14   2/29/08 7:21p gdata
* PR40134: [7405] Correcting the event wait from different locations
* 
* Hydra_Software_Devel/13   2/28/08 2:02p sushmit
* PR 40006: [7405] Workaround for expiring timer even before channel
* start completion.
* 
* Hydra_Software_Devel/12   2/27/08 11:19p sushmit
* PR 40006: [7405] Fixing Clear Mask for FMM Interrupts
* 
* Hydra_Software_Devel/11   2/26/08 6:38p gautamk
* PR34648: [7405] Fixing bugs and programming RDBDsp Offset to CIT.
* 
* Hydra_Software_Devel/10   2/25/08 1:55p sgadara
* PR 39904: [7405] Enabling 'BRAP_P_CRC_isr' function prototype for 7405
* family
* 
* Hydra_Software_Devel/8   2/15/08 5:26p gdata
* PR34648: [7405] Adding the changes to support the interrupt install
* before channel start
* 
* Hydra_Software_Devel/7   2/15/08 3:47p gdata
* PR34648: [7405] Enabling the asynchronous interrupt at dspchn start
* time
* 
* Hydra_Software_Devel/6   2/6/08 12:05p gautamk
* PR34648: [7405] fising bug in BRAP_P_DSP2HostAsyn_isr(), And removing
* duplicate function BRAP_P_GetDspChannelHandle
* 
* Hydra_Software_Devel/5   2/1/08 9:13p gdata
* PR34648: [7405] Sending the decode interrupt at start time
* 
* Hydra_Software_Devel/4   1/28/08 4:13p gdata
* PR34648:: [7405] Removing segmentation fault in get task handle.
* 
* Hydra_Software_Devel/3   1/28/08 2:20p gdata
* PR34648: [7405] Correting the logic of calling wait event for DSP
* interrupt
* Adding comments for different APIs
* 
* Hydra_Software_Devel/2   1/24/08 12:04a sushmit
* PR34648: [7405] Merging in updated code from Base2
* 
* Hydra_Software_Devel/1   1/22/08 4:47p sushmit
* PR34648: [7405] Merging in updated code from Base2
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/10   1/10/08 6:58p gautamk
* PR34648: [7405] Files for Mapping Processing network to CIT input
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/9   1/10/08 3:24p gdata
* PR 34648:[7440] Adding command for asynchronous interrupt
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/8   1/10/08 3:19p sushmit
* PR 34648: [7405] Removing hDsp from hDspChn
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/7   1/10/08 12:09p sushmit
* PR34648: [7405] Updating Async Event ID handling
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/6   1/9/08 10:36p sushmit
* PR 34648: [7405] Updating FW-PI Interface after syncup meeting.
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/5   1/7/08 4:20p gdata
* PR34648:  [7405]Adding the code for interrupt changes
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/4   1/7/08 3:46p sushmit
* PR34648: [7405] Adding Event Creation
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/3   1/4/08 11:35a sushmit
* PR34648: [7405] Updating more code.
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/2   1/2/08 4:15p gdata
* PR34648: [7405] Adding changes for Interrupt
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/1   12/31/07 10:26p sushmit
* PR34648: [7405] Adding Interrupt Change.
* 
* Hydra_Software_Devel/37   8/20/07 9:27p gautamk
* PR34154:[7440] Fixing Watchdog for PCM playback.
* 
* Hydra_Software_Devel/36   7/16/07 9:47a gdata
* PR22490 : Removing the compilation warning
* 
* Hydra_Software_Devel/35   7/12/07 6:48p gdata
* PR22490 : Merging the CRC code on main branch
* 
* Hydra_Software_Devel/34   7/9/07 3:21p gautamk
* PR32789: [7440] Add Phase2B Feature - PCM Playback in Interrupt Mode
* 
* Hydra_Software_Devel/33   7/3/07 7:03p bmishra
* PR 23951: Merged Autotrap related changes
* 
* Hydra_Software_Devel/32   6/14/07 7:25p gautamk
* PR30069: Changes for new algorithm Mpeg MC
* 
* Hydra_Software_Devel/31   6/14/07 4:48p gautamk
* PR30069: Adding Algorithm DDP 7.1
* 
* Hydra_Software_Devel/30   5/21/07 9:28p kagrawal
* PR 30069: Added a check to avert an assert failure with non-decode
* channel while masking an interrupt
* 
* Hydra_Software_Devel/29   5/11/07 8:58p sushmit
* PR 27646: Getting 7405 PI compiled
* 
* Hydra_Software_Devel/28   5/3/07 4:20p speter
* PR 30400: [3563] Merged the changes done as part of Phase 1.6 Release
* to the main line. Merge happened from branch RAP_3563_PH_1_6 to
* Hydra_Software_Devel
* 
* Hydra_Software_Devel/RAP_3563_PH_1_6/2   4/27/07 6:38p speter
* PR 28715: SPDIF and HDMI interrupts needs to be handled separately
* based on the input type and the state machine has to give the states
* also based on the input type apart from the interrupt status
* 
* Hydra_Software_Devel/RAP_3563_PH_1_6/1   4/27/07 2:25p speter
* PR 28715: [3563] Implementation of SPDIF Rx, Switching between PCM and
* Compressed input and vice versa (Not applicable to HDMI as it supports
* only PCM as of now)
* 
* Hydra_Software_Devel/27   4/16/07 9:47a kagrawal
* PR 28561: Reduced channel handle size by changing sPath[] to *pPath[]
* 
* Hydra_Software_Devel/26   3/27/07 11:59a speter
* PR 28718: Spdif Rx code organisation related changes
* 
* Hydra_Software_Devel/25   3/14/07 12:32p kagrawal
* PR 28561: [7440] Picking up the latest RDB registers for interrupt
* 
* Hydra_Software_Devel/24   3/12/07 3:34p speter
* PR 27999: Fixing interrupt code to handle PCM and compressed SPDIF
* using Good FMT interrupt and HDMI using MAI Change Interrupt
* 
* Hydra_Software_Devel/23   3/10/07 5:02p speter
* PR 27999: Spdif Rx Compressed input coding
* 
* Hydra_Software_Devel/22   2/22/07 12:18p bhanus
* PR 28040 : Modified code for 7440 B0 porting
* 
* Hydra_Software_Devel/21   2/9/07 6:21p bhanus
* PR 25607 : Fixed Compilation warnings
* 
* Hydra_Software_Devel/20   2/8/07 1:51p speter
* PR 25346: [3563] HDMI In feature - Adding MAI Format change interrupt
* handling
* 
* Hydra_Software_Devel/19   2/7/07 4:46p speter
* PR 25346: [3563] Removed warnings for 0.5 release
* 
* Hydra_Software_Devel/18   1/30/07 1:42p nitinb
* PR 25346: Debugged BTSC code and fixed few issues
* 
* Hydra_Software_Devel/17   1/22/07 7:29p sushmit
* PR 26864: SPDIF RX bringup. Fix issues in the handler. Also fix issues
* with interrupt handling for format/signal change
* 
* Hydra_Software_Devel/16   12/29/06 1:09a nitinb
* PR 26766: Added BTSC interrupt handling
* 
* Hydra_Software_Devel/15   12/15/06 4:51p speter
* PR 25607: Correcting warnings [7440/3563]
* 
* Hydra_Software_Devel/14   12/15/06 12:01p speter
* PR 25607: [7440] Finishing Todos and rearranging code for better
* clarity
* 
* Hydra_Software_Devel/13   12/13/06 4:23p speter
* PR 25346: [3563] SPDIF/HDMI In Feature
* 
* Hydra_Software_Devel/12   12/4/06 1:52p kagrawal
* PR 25607: Bug fixes for stereo PCM over HDMI output port
* 
* Hydra_Software_Devel/11   11/27/06 12:12p sushmit
* PR 25607: Added code for CAP, XCD & ChSubTypeNone
* 
* Hydra_Software_Devel/10   11/22/06 2:21p kagrawal
* PR 26079: Removed compilation warnings due to C++ type comments
* 
* Hydra_Software_Devel/9   11/9/06 11:39a bhanus
* PR 24717 : Added Support for PCM Playback.
* - Added a check to avoid assertion failure on DSP Id.
* 
* Hydra_Software_Devel/8   10/26/06 3:16p kagrawal
* PR 24717: Added support for StartPtsReached and StopPtsReached
* interrupts
* 
* Hydra_Software_Devel/7   10/11/06 2:28p bselva
* PR 24037: Modified for compilation of 3563 code.
* 
* Hydra_Software_Devel/6   10/6/06 7:13p kagrawal
* PR 24717: Merged 7440 devel branch to Hydra_Software_Devel
* 
* Hydra_Software_Devel/5   10/2/06 5:47p kagrawal
* PR 20654: Removed DSPCHN internal module name from the exposed API's
* 
* Hydra_Software_Devel/4   10/2/06 4:37p kagrawal
* PR 20654: Removed redundant error message
* 
* Hydra_Software_Devel/3   9/27/06 6:07p kagrawal
* PR 20654: Merged latest changes from the devel branch
* 
* Hydra_Software_Devel/2   9/18/06 1:14p kagrawal
* PR 20654: Modified for 0.1 bring up
* 
* Hydra_Software_Devel/1   9/5/06 11:06a speter
* PR 20654: Adding a new file for interrupt
* 
*
***************************************************************************/

#include "brap.h"   /* API function and data structures declarations */
#include "brap_priv.h"  /* Internal module includes */
#ifdef RAP_SCM_SUPPORT
#include "brap_scm.h"
#endif

BDBG_MODULE(rap_int);   /* Register software module with debug interface */

/* Static Variables & Function prototypes */
/* Static BINT array defining the ESR_Sxx IDs for the different DSP Contextx */
static const  BINT_Id ui32DSPSynInterruptId[] =
{
        BCHP_INT_ID_ESR_SO0
};
static const  BINT_Id ui32DSPAsynInterruptId[] =
{
        BCHP_INT_ID_ESR_SO1
};
static const  BINT_Id ui32DSPAckInterruptId[] =
{
        BCHP_INT_ID_ESR_SO2
};
        
/***************************************************************************
Summary:
    This private routine, returns the current Dsp ID associated with the channel handle.
****************************************************************************/
unsigned int BRAP_P_GetDspId (
    const BRAP_ChannelHandle hRapCh
    );

/***************************************************************************
Summary:
    This private routine, on receipt of the Watchdog interrupt calls the 
    application callback.
****************************************************************************/
static void 
BRAP_P_Watchdog_isr(
    void *pParm1, 
    int iParm2 
    );

/***************************************************************************
Summary:
    This private routine, on receipt of the FMM Source interrupt calls the 
    application callback.
****************************************************************************/
static void 
BRAP_P_FMMBF_SourceBuf_isr (
    void * pParm1, /* [in] Raptor channel handle */
    int    iParm2  /* [in] Not used */        
);

#if (BRAP_7550_FAMILY !=1)
/***************************************************************************
Summary:
    This private routine, on receipt of the FMM Destination interrupt calls the 
    application callback.
****************************************************************************/
static void 
BRAP_P_FMMBF_DestinationBuf_isr (
    void * pParm1, /* [in] Raptor destination handle */
    int    iParm2  /* [in] Not used */        
); 
#endif
#if (BRAP_3548_FAMILY == 1)
/***************************************************************************
Summary:
    This private routine, on receipt of the RF Audio interrupt calls the 
    application callback.
****************************************************************************/
static void
BRAP_P_RfAudio_isr (    
    void *pParm1, 
    int iParm2 
    );
   
/***************************************************************************
Summary:
    This private routine, on receipt of the SPDIF Rx interrupt calls the 
    application callback.
****************************************************************************/
static void 
BRAP_P_SPDIF_RX_isr (
    void * pParm1, /* [in] Raptor channel handle */
    int    iParm2  /* [in] Not used */        
);
#endif

#ifdef CRC_ENABLE
/***************************************************************************
Summary:
    This private routine, on receipt of the CRC interrupt calls the application callback.
****************************************************************************/
static void 
BRAP_P_CRC_isr(
void *pParm1, 
int iParm2
);
#endif



/***************************************************************************
Description:
	This API will unmask the bit for the corrosponding task id. So that FW can 
	raise the interrupt for this task.
Returns:
	BERR_SUCCESS - If successful
***************************************************************************/
static BERR_Code   BRAP_P_UnmaskTask_isr(
        BRAP_FWIF_P_FwTaskHandle hTask
);

/***************************************************************************
Description:
	This API will mask the bit for the corrosponding task id. So that FW can 
	not raise the interrupt for this task any more.
Returns:
	BERR_SUCCESS - If successful
***************************************************************************/
static BERR_Code   BRAP_P_MaskTask_isr(
        BRAP_FWIF_P_FwTaskHandle hTask
);

/***************************************************************************
Description:
	This API will be called when any asynchronous interrupt will be raised by 
	FW for any task.
Returns:
    void
See Also:    
    BRAP_P_DSP2HostSyn_isr, BRAP_P_DSP2HostAsyn_isr, BRAP_P_InterruptInstall
***************************************************************************/
static void BRAP_P_DSP2HostAsyn_isr(
        void    *pParm1,
        int     iParm2
);
/***************************************************************************
Description:
	This API will be called when any asynchronous interrupt will be raised by 
	FW for any task.
Returns:
    void
See Also:    
    BRAP_P_DSP2HostSyn_isr, BRAP_P_DSP2HostAsyn_isr, BRAP_P_InterruptInstall
***************************************************************************/
static void BRAP_P_DSP2HostSyn_isr(
        void    *pParm1,
        int     iParm2
);

/***************************************************************************
Description:
	This API will be called when any acknowledgement come from the FW. This 
	will be used to receive the ping command from FW when any DSP will be 
	started.
Returns:
    void
See Also:    
    BRAP_P_DSP2HostSyn_isr, BRAP_P_DSP2HostAsyn_isr, BRAP_P_AckInstall
***************************************************************************/
static void BRAP_P_DSP2HostAck_isr(
        void    *pParm1,
        int     iParm2
);

static void BRAP_P_DecoderLock_isr(void *pParam1,void *pParam2);
static void BRAP_P_DecoderLock_isr(void *pParam1,void *pParam2);
static void BRAP_P_SampleRateChange_isr(void *pParam1, void *pParam2,void *pParam3);
static void BRAP_P_BitRateChange_isr(void *pParam1, void *pParam2);
static void BRAP_P_ModeChange_isr(void *pParam1, void *pParam2);
static void BRAP_P_CrcError_isr(void *pParam1, void *pParam2);
static void BRAP_P_PtsError_isr(void *pParam1, void *pParam2);
static void BRAP_P_FirstPtsReady_isr(void *pParam1, void *pParam2);
static void BRAP_P_StartPtsReached_isr(void *pParam1, void *pParam2);
static void BRAP_P_StopPtsReached_isr(void *pParam1, void *pParam2);
static void BRAP_P_AstmPass_isr(void *pParam1, void *pParam2);
static void BRAP_P_TsmFail_isr(void *pParam1, void *pParam2);
static void BRAP_P_RampEnable_isr(void *pParam1, void *pParam2);
static void BRAP_P_StreamInfoAvailable_isr(void *pParam1, void *pParam2);
static void BRAP_P_CdbOverflow_isr(void *pParam1, void *pParam2);
static void BRAP_P_CdbUnderflow_isr(void *pParam1, void *pParam2);
static void BRAP_P_IsDtsTranscodePresent_isr(BRAP_DSPCHN_Handle  hDspCh ,unsigned int uiTaskId,bool *bPresent);
#ifdef 	RAP_GFX_SUPPORT
static void BRAP_P_GfxOperationCompleted_isr(void *pParam1, unsigned int    iParm2 );
#endif
#ifdef RAP_SCM_SUPPORT  
static void BRAP_P_ScmResponse_isr(void *pParam1, unsigned int    iParm2);
#endif




/***************************************************************************
Description:
	This API masks all the interrupts handled in this module and clears them.
Returns:
	BERR_Code 
Note:
   Typically during intialisation all interrupts are cleared and  masked
***************************************************************************/
BERR_Code BRAP_P_ClearInterrupts (
    BREG_Handle hRegister   /* [in] The register handle */
)
{
	BERR_Code ret = BERR_SUCCESS;

	BDBG_ENTER (BRAP_P_ClearInterrupts);
	BDBG_ASSERT (hRegister);            

	/* Mask all DSP interrupts */
	BRAP_Write32(hRegister, BCHP_AUD_DSP_INTH0_R5F_MASK_SET, 
	         ~(BCHP_MASK (AUD_DSP_INTH0_R5F_MASK_SET, reserved0)));
	BRAP_Write32(hRegister, BCHP_AUD_DSP_INTH0_PCI_MASK_SET, 
	         ~(BCHP_MASK (AUD_DSP_INTH0_PCI_MASK_SET, reserved0)));    
	BRAP_Write32(hRegister, BCHP_AUD_DSP_ESR_BE0_MASK_SET, 
	         ~(BCHP_MASK (AUD_DSP_ESR_BE0_MASK_SET, reserved0)));    
	BRAP_Write32(hRegister, BCHP_AUD_DSP_ESR_PI0_MASK_SET, 
	         ~(BCHP_MASK (AUD_DSP_ESR_PI0_MASK_SET, reserved0)));   

	BRAP_Write32(hRegister, BCHP_AUD_DSP_ESR_SO00_MASK_SET, 0xffffffff );
	BRAP_Write32(hRegister, BCHP_AUD_DSP_ESR_SO10_MASK_SET, 0xffffffff );
	BRAP_Write32(hRegister, BCHP_AUD_DSP_ESR_SO20_MASK_SET, 0xffffffff );
	BRAP_Write32(hRegister, BCHP_AUD_DSP_ESR_SO30_MASK_SET, 0xffffffff );
	BRAP_Write32(hRegister, BCHP_AUD_DSP_ESR_SO40_MASK_SET, 0xffffffff );
	BRAP_Write32(hRegister, BCHP_AUD_DSP_ESR_SO50_MASK_SET, 0xffffffff );


	/* Mask all FMM interrupts. */
	BDBG_MSG(("Masking all FMM interrupts"));
	BRAP_Write32 (hRegister, BCHP_AIO_INTH_R5F_MASK_SET, 
	             ~(BCHP_MASK (AIO_INTH_R5F_MASK_SET, reserved0)));   

	BRAP_Write32 (hRegister, BCHP_AIO_INTH_PCI_MASK_SET, 
	             ~(BCHP_MASK (AIO_INTH_PCI_MASK_SET, reserved0)));   


	BRAP_Write32 (hRegister, BCHP_AUD_FMM_BF_ESR0_H_MASK_SET, 
	             ~(BCHP_MASK (AUD_FMM_BF_ESR0_H_MASK_SET, reserved0)));
    
#if (BRAP_3548_FAMILY == 1)
	BRAP_Write32 (hRegister, BCHP_AUD_FMM_BF_ESR1_H_MASK_SET, 
	             ~(BCHP_MASK (AUD_FMM_BF_ESR1_H_MASK_SET, reserved0)));

	BRAP_Write32 (hRegister, BCHP_AUD_FMM_BF_ESR2_H_MASK_SET, 
                ~(BCHP_MASK (AUD_FMM_BF_ESR2_H_MASK_SET, reserved0))); 

#else
	BRAP_Write32 (hRegister, BCHP_AUD_FMM_BF_ESR1_H_MASK_SET, 0xffffffff);
	BRAP_Write32 (hRegister, BCHP_AUD_FMM_BF_ESR2_H_MASK_SET, 0xffffffff);
#endif


    BRAP_Write32 (hRegister, BCHP_AUD_FMM_DP_ESR00_MASK_SET, 0xffffffff );
    BRAP_Write32 (hRegister, BCHP_AUD_FMM_DP_ESR20_MASK_SET, 0xffffffff );

	BRAP_Write32 (hRegister, BCHP_AUD_FMM_IOP_ESR_MASK_SET, 
	             ~(BCHP_MASK (AUD_FMM_OP_ESR_MASK_SET, reserved0)));

	BRAP_Write32 (hRegister, BCHP_AUD_FMM_MS_ESR_MASK_SET, 
	             ~(BCHP_MASK (AUD_FMM_MS_ESR_MASK_SET, reserved0)));   

	BRAP_Write32 (hRegister, BCHP_AUD_FMM_OP_ESR_MASK_SET, 
	             ~(BCHP_MASK (AUD_FMM_OP_ESR_MASK_SET, reserved0)));   

#if (BRAP_3548_FAMILY == 1)
    BRAP_Write32 (hRegister, BCHP_SPDIF_RCVR_ESR_MASK_SET, 0xffffffff); 
    BRAP_Write32 (hRegister, BCHP_BTSC_ESR_MASK_SET, 0xffffffff); 
#endif

	/* Mask all Misc interrupts */
	BRAP_Write32 (hRegister, BCHP_AIO_INTD0_MASK_SET, 
	             ~(BCHP_MASK (AIO_INTD0_MASK_SET, reserved0)));   


	/* Clear all pending DSP interrupts */
	BRAP_Write32(hRegister, BCHP_AUD_DSP_INTH0_R5F_CLEAR, 
	             ~(BCHP_MASK (AUD_DSP_INTH0_R5F_CLEAR, reserved0)));   

	BRAP_Write32(hRegister, BCHP_AUD_DSP_ESR_BE0_INT_CLEAR, 
	             ~(BCHP_MASK (AUD_DSP_ESR_BE0_INT_CLEAR, reserved0)));   

	BRAP_Write32(hRegister, BCHP_AUD_DSP_ESR_PI0_INT_CLEAR, 
	                           ~(BCHP_MASK (AUD_DSP_ESR_PI0_INT_CLEAR, reserved0)));   

	BRAP_Write32(hRegister, BCHP_AUD_DSP_ESR_SO00_INT_CLEAR, 0xffffffff);
	BRAP_Write32(hRegister, BCHP_AUD_DSP_ESR_SO10_INT_CLEAR, 0xffffffff);
	BRAP_Write32(hRegister, BCHP_AUD_DSP_ESR_SO20_INT_CLEAR, 0xffffffff);
	BRAP_Write32(hRegister, BCHP_AUD_DSP_ESR_SO30_INT_CLEAR, 0xffffffff);
	BRAP_Write32(hRegister, BCHP_AUD_DSP_ESR_SO40_INT_CLEAR, 0xffffffff);
	BRAP_Write32(hRegister, BCHP_AUD_DSP_ESR_SO50_INT_CLEAR, 0xffffffff);

	/* Clear any pending FMM interrupts */
	BDBG_MSG(("Clearing all FMM interrupts"));
	BRAP_Write32 (hRegister, BCHP_AIO_INTH_R5F_CLEAR, 
	                            ~(BCHP_MASK (AIO_INTH_R5F_CLEAR, reserved0)));   
	BRAP_Write32 (hRegister, BCHP_AIO_INTH_PCI_CLEAR, 
	                            ~(BCHP_MASK (AIO_INTH_PCI_CLEAR, reserved0)));   

	BRAP_Write32 (hRegister, BCHP_AUD_FMM_BF_ESR0_H_STATUS_CLEAR, 
	             ~(BCHP_MASK (AUD_FMM_BF_ESR0_H_STATUS_CLEAR, reserved0)));

#if (BRAP_3548_FAMILY == 1)
	BRAP_Write32 (hRegister, BCHP_AUD_FMM_BF_ESR1_H_STATUS_CLEAR, 
	             ~(BCHP_MASK (AUD_FMM_BF_ESR1_H_STATUS_CLEAR, reserved0)));

	BRAP_Write32 (hRegister, BCHP_AUD_FMM_BF_ESR2_H_STATUS_CLEAR, 
	             ~(BCHP_MASK (AUD_FMM_BF_ESR2_H_STATUS_CLEAR, reserved0))); 
#else
	BRAP_Write32 (hRegister, BCHP_AUD_FMM_BF_ESR1_H_STATUS_CLEAR,0xffffffff ); 
	BRAP_Write32 (hRegister, BCHP_AUD_FMM_BF_ESR2_H_STATUS_CLEAR,0xffffffff ); 
#endif

	BRAP_Write32 (hRegister, BCHP_AUD_FMM_DP_ESR00_STATUS_CLEAR, 0xffffffff );
	BRAP_Write32 (hRegister, BCHP_AUD_FMM_DP_ESR20_STATUS_CLEAR, 0xffffffff );


	BRAP_Write32 (hRegister, BCHP_AUD_FMM_IOP_ESR_STATUS_CLEAR, 
	             ~(BCHP_MASK (AUD_FMM_OP_ESR_STATUS_CLEAR, reserved0)));

	BRAP_Write32 (hRegister, BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR, 
	                            ~(BCHP_MASK (AUD_FMM_MS_ESR_STATUS_CLEAR, reserved0)));   
	BRAP_Write32 (hRegister, BCHP_AUD_FMM_OP_ESR_STATUS_CLEAR, 
	                            ~(BCHP_MASK (AUD_FMM_OP_ESR_STATUS_CLEAR, reserved0)));   
	    
#if (BRAP_3548_FAMILY == 1)
    BRAP_Write32 (hRegister, BCHP_SPDIF_RCVR_ESR_STATUS_CLEAR, 0xffffffff);     
    BRAP_Write32 (hRegister, BCHP_BTSC_ESR_INT_CLEAR, 0xffffffff);     
#endif
	    
	/* Clear all miscellaneous interrupts */   
	BRAP_Write32 (hRegister, BCHP_AIO_INTD0_INT_CLEAR, 
	                            ~(BCHP_MASK (AIO_INTD0_INT_CLEAR, reserved0)));  

	BDBG_LEAVE (BRAP_P_ClearInterrupts);
	return ret;
}

/***************************************************************************
Description:
	This API Installs the top level interrupt handlers for DSP ping command.
Returns:
	BERR_Code 
See Also:
	BRAP_P_AckUnInstall
***************************************************************************/
BERR_Code BRAP_P_AckInstall(
    BRAP_DSP_Handle         hDsp
)
{
    BERR_Code       ret = BERR_SUCCESS;
    unsigned int    uiDspId;
    uint32_t        ui32RegVal = 0;
    bool            bWdgRecovery = false;


    BDBG_ENTER(BRAP_P_AckInstall);
    BDBG_ASSERT(hDsp);

    bWdgRecovery = BRAP_P_GetWatchdogRecoveryFlag(hDsp->hRap);

    uiDspId = hDsp->uiDspIndex;
    
    if(false == bWdgRecovery)
    {    
        ret = BINT_CreateCallback(
            &hDsp->hDspAckCallback,
    		hDsp->hInt,
    		ui32DSPAckInterruptId[uiDspId],
    		BRAP_P_DSP2HostAck_isr,
    		(void*)hDsp,
    		0 
    		);
    	if(ret != BERR_SUCCESS )
    	{
    		BDBG_ERR(("Create Callback hDspAckCallback failed for DSP = %d BCHP_INT_ID_ESR_SO2",
                        uiDspId));
    		ret = BERR_TRACE(ret);
    	}
        if(hDsp->hDspAckCallback)
        {
        	ret = BINT_EnableCallback(hDsp->hDspAckCallback);
        	if(ret != BERR_SUCCESS )
        	{
        		BDBG_ERR(("Enable Callback hDspAckCallback failed for DSP = %d BCHP_INT_ID_ESR_SO2",
                        uiDspId));
        		ret = BERR_TRACE(ret);
        	}
        }
    }
    else
    {
        if(hDsp->hDspAckCallback)
        {
            ret = BINT_DisableCallback(hDsp->hDspAckCallback);
            if(ret != BERR_SUCCESS )
            {
                BDBG_MSG(("Disable Callback hDspAckCallback failed for DSP = %d BCHP_INT_ID_ESR_SO2",
                uiDspId));
                ret = BERR_TRACE(ret);
            }
        }
        if(hDsp->hDspAckCallback)
        {            
            ret = BINT_EnableCallback(hDsp->hDspAckCallback);
            if(ret != BERR_SUCCESS )
            {
                BDBG_MSG(("Enable Callback hDspAckCallback failed for DSP = %d BCHP_INT_ID_ESR_SO2",
                uiDspId));
                ret = BERR_TRACE(ret);
            }
        }

        ui32RegVal =0;
        ui32RegVal |=BCHP_MASK(AUD_DSP_INTH0_R5F_MASK_CLEAR, ESR_SO2);    
        BRAP_Write32_isr(hDsp->hRegister, BCHP_AUD_DSP_INTH0_R5F_MASK_CLEAR, ui32RegVal);    
    }
    /*Setting the mask for Ping bit */
    ui32RegVal |= 0x1 ;
    BRAP_Write32_isr(hDsp->hRegister, BCHP_AUD_DSP_ESR_SO20_MASK_CLEAR, ui32RegVal);    


    BDBG_LEAVE(BRAP_P_AckInstall);        
    return ret;
}

/***************************************************************************
Description:
	This API uninstalls the top level interrupt handlers for DSP ping command.
Returns:
	BERR_Code 
See Also:
	BRAP_P_AckUnInstall
***************************************************************************/
BERR_Code BRAP_P_AckUnInstall(
    BRAP_DSP_Handle         hDsp
)
{
    BERR_Code       ret = BERR_SUCCESS;

    BDBG_ENTER(BRAP_P_AckUnInstall);
    BDBG_ASSERT(hDsp);
    if(hDsp->hDspAckCallback)
    {
        ret = BINT_DisableCallback(hDsp->hDspAckCallback);
    	if (ret!=BERR_SUCCESS)
    	{
    		ret = BERR_TRACE(ret);
        }
    	ret = BINT_DestroyCallback(hDsp->hDspAckCallback);
    	if (ret!=BERR_SUCCESS)
    	{
    		ret = BERR_TRACE(ret);
        }
    }

    BDBG_LEAVE(BRAP_P_AckUnInstall);        
    return ret;
}

/***************************************************************************
Description:
	This API installs the decoder interrupts.
Returns:
	BERR_Code 
See Also:
	BRAP_P_InterruptInstall
***************************************************************************/
BERR_Code BRAP_P_DecoderInterrupt(
    BRAP_FWIF_P_FwTaskHandle    hTask		/* [in] Task handle */
)
{
	BERR_Code       ret = BERR_SUCCESS;
    BRAP_FWIF_P_Command     sFwCommand;
    BRAP_FWIF_P_Response sRsp;
    BRAP_P_MsgType      eMsgType;
     BRAP_Handle hRap;    

    BDBG_ENTER(BRAP_P_DecoderInterrupt);
    BDBG_ASSERT(hTask);

    BKNI_Memset((void *)&sRsp,0,sizeof(BRAP_FWIF_P_Response));
    
    if(hTask->bChSpecificTask== true)
    {
        hRap = hTask->uHandle.hRapCh->hRap;
    }
    else
    {
        hRap = hTask->uHandle.hAssociation->hRap;
    }

    /*Prepare the command for FW*/
    sFwCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_EVENT_NOTIFICATION_COMMAND_ID;
    sFwCommand.sCommandHeader.ui32CommandSizeInBytes = sizeof(BRAP_FWIF_P_Command);
    sFwCommand.sCommandHeader.eResponseType = BRAP_FWIF_P_ResponseType_eAckRequired;
    sFwCommand.sCommandHeader.ui32TaskID = hTask->uiTaskId;
    sFwCommand.sCommandHeader.ui32CommandCounter = hRap->uiCommandCounter++;;
    hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] |= BRAP_FWIF_P_EventIdMask_eSampleRateChange;    
    hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] |= BRAP_FWIF_P_EventIdMask_eFirstPTS_Received;    
    hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] |= BRAP_FWIF_P_EventIdMask_ePTS_error;    
    hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] |= BRAP_FWIF_P_EventIdMask_eTsmFail;        
    hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] |= BRAP_FWIF_P_EventIdMask_eTSM_Lock;    
    hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] |= BRAP_FWIF_P_EventIdMask_eFrameSyncLock;    
    hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] |= BRAP_FWIF_P_EventIdMask_eFrameSyncLockLost;    
    hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] |= BRAP_FWIF_P_EventIdMask_eStartOnPTS;    
    hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] |= BRAP_FWIF_P_EventIdMask_eStopOnPTS;        
    hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] |= BRAP_FWIF_P_EventIdMask_eAstmTsmPass;        
    hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] |= BRAP_FWIF_P_EventIdMask_eStreamInfoAvail;           
    hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] |= BRAP_FWIF_P_EventIdMask_eUnlicensedAlgo;           
    hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] |= BRAP_FWIF_P_EventIdMask_eRampEnable;        

    
    
    sFwCommand.uCommand.sEnableDisableEvent.ui32EnableEvent =     
        hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)];

    hTask->uiLastEventType = sFwCommand.sCommandHeader.ui32CommandID;
    BRAP_P_EventReset(hTask->hDsp,BRAP_GET_TASK_INDEX(hTask->uiTaskId));
    /* Write in Message queue */
    ret = BRAP_FWIF_P_SendCommand(hTask->hDsp->hCmdQueue, &sFwCommand,hRap,hTask);
    if(ret != BERR_SUCCESS)
    {
        if((hRap->bWatchdogTriggered == false)
        &&(hTask->bStopped == false))
        {
            BDBG_ERR(("BRAP_P_DecoderInterrupt: Send command failed"));
            return BERR_TRACE(ret);
        }
        else
            ret = BERR_SUCCESS;  
    }
    ret = BRAP_P_EventWait(hTask->hDsp, BRAP_DSPCHN_P_EVENT_TIMEOUT_IN_MS,BRAP_GET_TASK_INDEX(hTask->uiTaskId ));    
    if(BERR_TIMEOUT == ret)
    {
        if((hRap->bWatchdogTriggered == false))
        {
            /* Please note that, If the code reaches at this point then there is a potential Bug in Fw 
            code which needs to be debugged. However Watchdog is being triggered to recover the system*/            
            BDBG_WRN(("BRAP_P_DecoderInterrupt: Event failed! Triggering Watchdog"));        
#if 0                
            BDBG_ASSERT(0);                
#endif
            BRAP_Write32(hTask->hDsp->hRegister, BCHP_AUD_DSP_INTH0_R5F_SET+ hTask->hDsp->ui32Offset,0x1);
            hRap->bWatchdogTriggered  = true;
#if 0                
            err = BERR_TRACE(err);
            goto error;
#endif                 
            ret = BERR_SUCCESS;              
        }
        else
            ret = BERR_SUCCESS;              
    }

    eMsgType = BRAP_P_MsgType_eSyn;
    if((hRap->bWatchdogTriggered == false)
    &&(hTask->bStopped == false))    
    {
        ret = BRAP_FWIF_P_GetMsg(hTask->hSyncMsgQueue, (void *)&sRsp,eMsgType);
    }
    if(BERR_SUCCESS != ret)
    {
        if((hRap->bWatchdogTriggered == false)
            &&(hTask->bStopped == false))
        {
            BDBG_ERR(("BRAP_P_DecoderInterrupt: Unable to read ACK!"));
            return BERR_TRACE(ret);
        }
        else
            ret = BERR_SUCCESS;    
    }
    if((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
        (sRsp.sCommonAckResponseHeader.ui32ResponseID != 0x307)||
        (sRsp.sCommonAckResponseHeader.ui32TaskID != hTask->uiTaskId))
    {
        if((hRap->bWatchdogTriggered == false)
            &&(hTask->bStopped == false))
        {
            BDBG_ERR(("BRAP_P_DecoderInterrupt: START_TASK ACK not received successfully!"));
            return BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
        }
        else
            ret = BERR_SUCCESS;    
    }
    
    BDBG_LEAVE(BRAP_P_DecodeInterrupt);
    return ret;    
}


/***************************************************************************
Description:
	This API installs the top level interrupt handlers for all the interrups.
Returns:
	BERR_Code 
See Also:
	BRAP_P_InterruptUnInstall
***************************************************************************/
BERR_Code BRAP_P_InterruptInstall (
	BRAP_ChannelHandle 		hRapCh		/* [in] Raptor Channel handle */
)
{
	BERR_Code       ret = BERR_SUCCESS;
	bool            bWdgRecovery = false;
    unsigned int    uiCounter;
	BDBG_ENTER (BRAP_P_InterruptInstall);    
	BDBG_ASSERT (hRapCh);            

	bWdgRecovery = BRAP_P_GetWatchdogRecoveryFlag(hRapCh->hRap);

#ifndef EMULATION /* Do not handle interrupts in emulation */
	if(false == bWdgRecovery)
	{
		/* Install the interrupt callback for the DSP to HOST Audio interrupts */
		if( (hRapCh->eChannelType == BRAP_ChannelType_eDecode)
#if (BRAP_3548_FAMILY == 1)
            || (BRAP_ChannelType_ePcmCapture== hRapCh->eChannelType)        
#endif         
#ifdef RAP_MULTISTREAM_DECODER_SUPPORT
            ||(hRapCh->eChannelType == BRAP_ChannelType_ePcmPlayback)
#endif
#ifdef RAP_GFX_SUPPORT            
            ||(hRapCh->eChannelType == BRAP_ChannelType_eGfx)
#endif            
#ifdef RAP_SCM_SUPPORT  
			||(hRapCh->eChannelType == BRAP_ChannelType_eScm)
#endif			
          )
          
		{
		    /* We will create two callbacks for Asynchronous Interrupt & for 
		    Synchtonous interrupt on both DAP0 as well as on DS1 */
            for(uiCounter = 0; uiCounter < BRAP_RM_P_MAX_DSPS; ++uiCounter)
            {
                ret = BINT_CreateCallback(
                    &hRapCh->hDSPAsynCallback[uiCounter],
    				hRapCh->hInt,
    				ui32DSPAsynInterruptId[uiCounter],
    				BRAP_P_DSP2HostAsyn_isr,
    				(void*)hRapCh->hRap,
    				uiCounter
    				);
    			if(ret != BERR_SUCCESS )
    			{
    				BDBG_ERR(("Create Callback failed for DSP = %d INT_ID_ESR_SO1",uiCounter));
    				ret = BERR_TRACE(ret);
                    goto end;
    			}
    			ret = BINT_EnableCallback(hRapCh->hDSPAsynCallback[uiCounter]);
    			if(ret != BERR_SUCCESS )
    			{
    				BDBG_ERR(("Enable Callback failed for DSP = %d INT_ID_ESR_SO1",uiCounter));
    				ret = BERR_TRACE(ret);
                    goto end;
    			}

                ret = BINT_CreateCallback(
                    &hRapCh->hDSPSynCallback[uiCounter],
    				hRapCh->hInt,
    				ui32DSPSynInterruptId[uiCounter],
    				BRAP_P_DSP2HostSyn_isr,
    				(void*)hRapCh->hRap,
    				uiCounter 
    				);
    			if(ret != BERR_SUCCESS )
    			{
    				BDBG_ERR(("Create Callback failed for DSP = %d BCHP_INT_ID_ESR_SO0",uiCounter));
    				ret = BERR_TRACE(ret);
                    goto end;
    			}
    			ret = BINT_EnableCallback(hRapCh->hDSPSynCallback[uiCounter]);
    			if(ret != BERR_SUCCESS )
    			{
    				BDBG_ERR(("Enable Callback failed for DSP = %d BCHP_INT_ID_ESR_SO0",uiCounter));
    				ret = BERR_TRACE(ret);
                    goto end;
    			}
            }

		}

            if((hRapCh->eChannelType == BRAP_ChannelType_ePcmPlayback))
            {
    		/* Install the interrupt callback for the FMM to HOST Audio interrupts */
    		ret = BINT_CreateCallback(
    			&hRapCh->hFMMCallback,
    			hRapCh->hInt,
    			BCHP_INT_ID_FMM_BF2,
    			BRAP_P_FMMBF_SourceBuf_isr,
    			(void*)hRapCh,
    			0 /* Not used */
    			);

    		if ( ret != BERR_SUCCESS )
    		{
    			BDBG_ERR(("Create Callback failed for FMM INT_ID[%d]", BCHP_INT_ID_FMM_BF2));
    			ret = BERR_TRACE(ret);
                goto end;
    		}

    		ret = BINT_EnableCallback(hRapCh->hFMMCallback);		
    		if ( ret != BERR_SUCCESS )
    		{
    			BDBG_ERR(("Enable Callback failed for FMM INT_ID[%d]", BCHP_INT_ID_FMM_BF2));
    			ret = BERR_TRACE(ret);
                goto end;

    		}
            }
#ifdef CRC_ENABLE
		ret = BINT_CreateCallback(
			&hRapCh->hFMMOpCallback,
			hRapCh->hInt,
			BCHP_INT_ID_FMM_OP,
			BRAP_P_CRC_isr,
			(void*)hRapCh,
			0 /* Not used */
			);
		if ( ret != BERR_SUCCESS )
		{
			BDBG_ERR(("Create Callback failed for FMM INT_ID[%d]", BCHP_INT_ID_FMM_OP));
			ret = BERR_TRACE(ret);
            goto end;
		}

		ret = BINT_EnableCallback(hRapCh->hFMMOpCallback);		
		if ( ret != BERR_SUCCESS )
		{
			BDBG_ERR(("Enable Callback failed for FMM INT_ID[%d]", BCHP_INT_ID_FMM_OP));
			ret = BERR_TRACE(ret);
            goto end;

		}
#endif
	}
	else
	{   

        /* Now enable the callback */
        if ( (hRapCh->eChannelType == BRAP_ChannelType_eDecode)
#ifdef 	RAP_GFX_SUPPORT            
            ||(hRapCh->eChannelType == BRAP_ChannelType_eGfx)
#endif            
#ifdef RAP_SCM_SUPPORT  
			||(hRapCh->eChannelType == BRAP_ChannelType_eScm)
#endif		
#ifdef RAP_MULTISTREAM_DECODER_SUPPORT
					||(hRapCh->eChannelType == BRAP_ChannelType_ePcmPlayback)
#endif
#if (BRAP_3548_FAMILY == 1)
             || (BRAP_ChannelType_ePcmCapture== hRapCh->eChannelType)        
#endif         
           )            
        {
            for(uiCounter = 0; uiCounter < BRAP_RM_P_MAX_DSPS; ++uiCounter)
            {
                if (hRapCh->hDSPSynCallback[uiCounter])
                {        
                    ret = BINT_EnableCallback(hRapCh->hDSPSynCallback[uiCounter]);
                    if(ret != BERR_SUCCESS )
                    {
                        BDBG_ERR(("Enable Callback failed for BCHP_INT_ID_ESR_SO0"));
                        ret = BERR_TRACE(ret);
                        goto end;
                    }
                }

                if (hRapCh->hDSPAsynCallback[uiCounter])
                {        
                    ret = BINT_EnableCallback(hRapCh->hDSPAsynCallback[uiCounter]);
                    if(ret != BERR_SUCCESS )
                    {
                        BDBG_ERR(("Enable Callback failed for  INT_ID_ESR_SO1"));
                        ret = BERR_TRACE(ret);
                        goto end;
                    }
                }
            }
        }
        if((hRapCh->eChannelType == BRAP_ChannelType_ePcmPlayback))
        {
            if (hRapCh->hFMMCallback)
            {        
                ret = BINT_EnableCallback(hRapCh->hFMMCallback);		
                if ( ret != BERR_SUCCESS )
                {
                    BDBG_ERR(("Enable Callback failed for FMM INT_ID[%d]", BCHP_INT_ID_FMM_BF2));
                    ret = BERR_TRACE(ret);
                    goto end;
                }
            }
        }
        
#ifdef CRC_ENABLE		
        if (hRapCh->hFMMOpCallback)
        {        
            ret = BINT_EnableCallback(hRapCh->hFMMOpCallback);		
            if ( ret != BERR_SUCCESS )
            {
                BDBG_ERR(("Enable Callback failed for FMM INT_ID[%d]", BCHP_INT_ID_FMM_OP));
                ret = BERR_TRACE(ret);
                goto end;
            }
        }
#endif

        
   }
#endif /* ifndef EMULATION */

end:
	BDBG_LEAVE(BRAP_P_InterruptInstall);
	return ret;    
}

/***************************************************************************
Description:
	This API uninstalls the top level interrupt handlers for all the interrups.
Returns:
	BERR_Code 
See Also:
	BRAP_P_InterruptUnInstall
***************************************************************************/
BERR_Code BRAP_P_InterruptUnInstall (
	BRAP_ChannelHandle 		hRapCh		/* [in] Raptor Channel handle */
)
{
	BERR_Code ret = BERR_SUCCESS;
    uint32_t  uiCounter = 0;
	BREG_Handle hRegister;
        bool bWdgRecovery=false;

	BDBG_ASSERT (hRapCh);            

	BDBG_ENTER (BRAP_P_InterruptUnInstall);    

	bWdgRecovery = BRAP_P_GetWatchdogRecoveryFlag(hRapCh->hRap);

#ifndef EMULATION /* Do not handle interrupts in emulation */
	hRegister = hRapCh->hRegister;

    if(bWdgRecovery == false)
    {
        if( (hRapCh->eChannelType == BRAP_ChannelType_eDecode)
#if (BRAP_3548_FAMILY == 1)
            || (BRAP_ChannelType_ePcmCapture== hRapCh->eChannelType)        
#endif         
#ifdef RAP_MULTISTREAM_DECODER_SUPPORT
            ||(hRapCh->eChannelType == BRAP_ChannelType_ePcmPlayback)
#endif
#ifdef RAP_GFX_SUPPORT            
          ||(hRapCh->eChannelType == BRAP_ChannelType_eGfx)
#endif  
#ifdef RAP_SCM_SUPPORT
		  ||(hRapCh->eChannelType == BRAP_ChannelType_eScm)
#endif		  
          )
    {
        for(uiCounter = 0; uiCounter < BRAP_RM_P_MAX_DSPS; ++uiCounter)
        {
            if(hRapCh->hDSPAsynCallback[uiCounter] != NULL)
            {
                ret = BINT_DisableCallback(hRapCh->hDSPAsynCallback[uiCounter]);
            	if (ret!=BERR_SUCCESS)
            	{
            		ret = BERR_TRACE(ret);
                    goto end;
                }
            	ret = BINT_DestroyCallback(hRapCh->hDSPAsynCallback[uiCounter]);
            	if (ret!=BERR_SUCCESS)
            	{
            		ret = BERR_TRACE(ret);
                    goto end;
                }
            }
            if(hRapCh->hDSPSynCallback[uiCounter]  != NULL)
            {
                ret = BINT_DisableCallback(hRapCh->hDSPSynCallback[uiCounter]);
            	if (ret!=BERR_SUCCESS)
            	{
            		ret = BERR_TRACE(ret);
                    goto end;
                }
            	ret = BINT_DestroyCallback(hRapCh->hDSPSynCallback[uiCounter]);
            	if (ret!=BERR_SUCCESS)
            	{
            		ret = BERR_TRACE(ret);
                    goto end;
                }
            }
        }
    }

    if((hRapCh->eChannelType == BRAP_ChannelType_ePcmPlayback))
    {    
        /* UnInstall the interrupt callback for FMM to Host interrupts */    
        if(hRapCh->hFMMCallback)
        {
            ret = BINT_DisableCallback(hRapCh->hFMMCallback);
            if (ret!=BERR_SUCCESS)
            {
                ret = BERR_TRACE(ret);
                goto end;
            }

            ret = BINT_DestroyCallback(hRapCh->hFMMCallback);
            if (ret!=BERR_SUCCESS)
            {
                ret = BERR_TRACE(ret);
                goto end;
            }
        }
    }
#ifdef CRC_ENABLE	
	if(hRapCh->hFMMOpCallback)
	{
		ret = BINT_DisableCallback(hRapCh->hFMMOpCallback);
		if (ret!=BERR_SUCCESS)
		{
	    		ret = BERR_TRACE(ret);
       	     goto end;
	        }

		ret = BINT_DestroyCallback(hRapCh->hFMMOpCallback);
		if (ret!=BERR_SUCCESS)
		{
    			ret = BERR_TRACE(ret);
	            goto end;
       	 }
	}
#endif
    }
    else
    {
        if( (hRapCh->eChannelType == BRAP_ChannelType_eDecode)
#ifdef 	RAP_GFX_SUPPORT            
            ||(hRapCh->eChannelType == BRAP_ChannelType_eGfx)
#endif            
#ifdef RAP_SCM_SUPPORT
			||(hRapCh->eChannelType == BRAP_ChannelType_eScm)
#endif			
#ifdef RAP_MULTISTREAM_DECODER_SUPPORT
            ||(hRapCh->eChannelType == BRAP_ChannelType_ePcmPlayback)
#endif
#if (BRAP_3548_FAMILY == 1)
            || (BRAP_ChannelType_ePcmCapture== hRapCh->eChannelType)        
#endif         
          )
        {
            for(uiCounter = 0; uiCounter < BRAP_RM_P_MAX_DSPS; ++uiCounter)
            {
                if(hRapCh->hDSPAsynCallback[uiCounter] != NULL)
                {
                    ret = BINT_DisableCallback(hRapCh->hDSPAsynCallback[uiCounter]);
                	if (ret!=BERR_SUCCESS)
                	{
                		ret = BERR_TRACE(ret);
                        goto end;
                    }
                }
                if(hRapCh->hDSPSynCallback[uiCounter]  != NULL)
                {
                    ret = BINT_DisableCallback(hRapCh->hDSPSynCallback[uiCounter]);
                	if (ret!=BERR_SUCCESS)
                	{
                		ret = BERR_TRACE(ret);
                        goto end;
                    }
                }
            }
        }
        if((hRapCh->eChannelType == BRAP_ChannelType_ePcmPlayback))
        {        
        	/* UnInstall the interrupt callback for FMM to Host interrupts */    
        	if(hRapCh->hFMMCallback)
        	{
        		ret = BINT_DisableCallback(hRapCh->hFMMCallback);
        		if (ret!=BERR_SUCCESS)
        		{
            		ret = BERR_TRACE(ret);
                    goto end;
                }
        	}
        }
#ifdef CRC_ENABLE	
    	if(hRapCh->hFMMOpCallback)
    	{
    		ret = BINT_DisableCallback(hRapCh->hFMMOpCallback);
    		if (ret!=BERR_SUCCESS)
    		{
    	    		ret = BERR_TRACE(ret);
           	     goto end;
    	        }
    	}
#endif
    }

        

	BDBG_MSG(("Callbacks destroyed."));
#endif /* ifndef EMULATION */

end:
	BDBG_LEAVE(BRAP_P_InterruptUnInstall);
	return ret;    
}

/***************************************************************************
Description:
	This API Installs callbacks for device level interrupts.
Returns:
	BERR_Code 
See Also:
	BRAP_P_DeviceLevelInterruptUnInstall
***************************************************************************/
BERR_Code BRAP_P_DeviceLevelInterruptInstall (
	BRAP_Handle		hRap
)
{
	BERR_Code ret = BERR_SUCCESS;
    bool            bWdgRecovery = false;
    
	BDBG_ENTER (BRAP_P_DeviceLevelInterruptInstall);
	BDBG_ASSERT(hRap);

    bWdgRecovery = BRAP_P_GetWatchdogRecoveryFlag(hRap);

    if(false == bWdgRecovery)
    {
    	ret = BINT_CreateCallback(
    				&hRap->hCallback,
    				hRap->hInt,
    				BCHP_INT_ID_WDOG_TIMER_ATTN,
    				BRAP_P_Watchdog_isr,
    				(void*)hRap,
    				0 /* Not used */
    				);

    	if (ret!=BERR_SUCCESS)
        {
            ret = BERR_TRACE(ret);
            goto end;
    	}

#if (BRAP_3548_FAMILY == 1)
        ret = BINT_CreateCallback(
        			&hRap->hRfAudioCallback,
        			hRap->hInt,
        			BCHP_INT_ID_BTSC,
        			BRAP_P_RfAudio_isr,
        			(void*)hRap,
        			0 /* Not used */
        			);

    	if (ret!=BERR_SUCCESS)
        {
            BDBG_ERR(("Create Callback failed for  RFAUDIO INT_ID[%d]", BCHP_INT_ID_BTSC));
            ret = BERR_TRACE(ret);
            goto end;
    	}

        ret = BINT_EnableCallback( hRap->hRfAudioCallback );
    	if (ret!=BERR_SUCCESS)
        {
            BDBG_ERR(("Enable Callback failed for  RFAUDIO INT_ID[%d]", BCHP_INT_ID_BTSC));
            ret = BERR_TRACE(ret);
            goto end;
    	}
        BDBG_MSG(("Installed RF Audio Callback"));    

        /* Install the interrupt callback for the SPDIF RX to HOST interrupts */
        ret = BINT_CreateCallback(
            &hRap->hSpdifRxCallback,
            hRap->hInt,
            BCHP_INT_ID_SPDIFRX,
            BRAP_P_SPDIF_RX_isr,
            (void*)hRap,
            0 /* Not used*/
            );

    	if ( ret != BERR_SUCCESS )
    	{
    		BDBG_ERR(("Create Callback failed for SPDIFRX INT_ID[%d]", BCHP_INT_ID_SPDIFRX));
    		ret = BERR_TRACE(ret);
            goto end;

    	}
        
    	ret = BINT_EnableCallback(hRap->hSpdifRxCallback);		
    	if ( ret != BERR_SUCCESS )
    	{
    		BDBG_ERR(("Enable Callback failed for SPDIFRX INT_ID[%d]", BCHP_INT_ID_SPDIFRX));
    		ret = BERR_TRACE(ret);
            goto end;

    	}
        BDBG_MSG(("Installed SPDIF Callback"));    
#endif    
    }    
    else
    {
#if (BRAP_3548_FAMILY == 1)
        if (hRap->hRfAudioCallback)
        {
            ret = BINT_DisableCallback( hRap->hRfAudioCallback );
            if (ret!=BERR_SUCCESS)
            {
                BDBG_ERR(("Disable Callback failed for  RFAUDIO INT_ID[%d]", BCHP_INT_ID_BTSC));
                ret = BERR_TRACE(ret);
                goto end;
            }
            ret = BINT_EnableCallback( hRap->hRfAudioCallback );
            if (ret!=BERR_SUCCESS)
            {
                BDBG_ERR(("Enable Callback failed for  RFAUDIO INT_ID[%d]", BCHP_INT_ID_BTSC));
                ret = BERR_TRACE(ret);
                goto end;
            }        
        }

        if (hRap->hSpdifRxCallback)
        {
            ret = BINT_DisableCallback(hRap->hSpdifRxCallback);		
            if ( ret != BERR_SUCCESS )
            {
                BDBG_ERR(("Disable Callback failed for  SPDIFRX INT_ID[%d]", BCHP_INT_ID_SPDIFRX));
                ret = BERR_TRACE(ret);
                goto end;
            }
        	ret = BINT_EnableCallback(hRap->hSpdifRxCallback);		
        	if ( ret != BERR_SUCCESS )
        	{
        		BDBG_ERR(("Enable Callback failed for SPDIFRX INT_ID[%d]", BCHP_INT_ID_SPDIFRX));
        		ret = BERR_TRACE(ret);
                        goto end;

        	}            
        }
#endif        
    }
end:     
    BDBG_LEAVE (BRAP_P_DeviceLevelInterruptInstall);
	return ret;
}

/***************************************************************************
Description:
	This API uninstalls any previously installed callbacks for device level interrupts.
Returns:
	BERR_Code 
See Also:
	BRAP_P_DeviceLevelInterruptInstall
***************************************************************************/
BERR_Code BRAP_P_DeviceLevelInterruptUnInstall (
	BRAP_Handle		hRap
)
{
	BERR_Code ret = BERR_SUCCESS;
	BDBG_ENTER (BRAP_P_DeviceLevelInterruptUnInstall);
	BDBG_ASSERT(hRap);

	if(hRap->hCallback)
	{
		ret = BINT_DisableCallback(hRap->hCallback);
		if (ret!=BERR_SUCCESS)
		{
			ret = BERR_TRACE(ret);
            goto end;
		}
		
		ret = BINT_DestroyCallback(hRap->hCallback);
		if (ret!=BERR_SUCCESS)
		{
			ret = BERR_TRACE(ret);
            goto end;
		}		
        BDBG_MSG(("Callback destroyed."));
	}

#if (BRAP_3548_FAMILY == 1)
	if(hRap->hRfAudioCallback)
	{
    	ret = BINT_DisableCallback(hRap->hRfAudioCallback);
    	if (ret!=BERR_SUCCESS)
    	{
    		ret = BERR_TRACE(ret);
            goto end;
    	}
    	
    	ret = BINT_DestroyCallback(hRap->hRfAudioCallback);
    	if (ret!=BERR_SUCCESS)
    	{
    		ret = BERR_TRACE(ret);
            goto end;
    	}	
    }

	if(hRap->hSpdifRxCallback)
	{
		ret = BINT_DisableCallback(hRap->hSpdifRxCallback);
		if (ret!=BERR_SUCCESS)
		{
    		ret = BERR_TRACE(ret);
            goto end;
        }

		ret = BINT_DestroyCallback(hRap->hSpdifRxCallback);
		if (ret!=BERR_SUCCESS)
		{
    		ret = BERR_TRACE(ret);
            goto end;
        }
	}    
   
    BDBG_MSG(("Callback destroyed."));
#endif    

end:    
    BDBG_LEAVE (BRAP_P_DeviceLevelInterruptUnInstall);
	return ret;
}

#if (BRAP_7550_FAMILY != 1)

/***************************************************************************
Description:
	This API Installs callbacks for destination level interrupts.
Returns:
	BERR_Code 
See Also:
	BRAP_P_DestinationLevelInterruptUnInstall
***************************************************************************/
BERR_Code BRAP_P_DestinationLevelInterruptInstall(
	BRAP_DestinationHandle        hDstHandle
)
{
	BERR_Code       ret = BERR_SUCCESS;
	bool            bWdgRecovery = false;
    uint32_t        ui32RegVal = 0;

    BDBG_ENTER (BRAP_P_DestinationLevelInterruptInstall);    
    BDBG_ASSERT (hDstHandle);            

	bWdgRecovery = BRAP_P_GetWatchdogRecoveryFlag(hDstHandle->hAssociation->hRap); 

#ifndef EMULATION /* Handle interrupts only in non emulation case */
	if(false == bWdgRecovery)
	{      

		/* Install the interrupt callback for the FMM to HOST Audio interrupts */
		ret = BINT_CreateCallback(
			&hDstHandle->hFmmBf1Callback,
			hDstHandle->hAssociation->hRap->hInt,
			BCHP_INT_ID_FMM_BF1,
			BRAP_P_FMMBF_DestinationBuf_isr,
			(void*)hDstHandle,
			0 /* Not used */
			);

		if ( ret != BERR_SUCCESS )
		{
			BDBG_ERR(("Create Callback failed for FMM INT_ID[%d]", BCHP_INT_ID_FMM_BF1));
			ret = BERR_TRACE(ret);
            goto end;
		}

		ret = BINT_EnableCallback(hDstHandle->hFmmBf1Callback);		
		if ( ret != BERR_SUCCESS )
		{
			BDBG_ERR(("Enable Callback failed for FMM INT_ID[%d]", BCHP_INT_ID_FMM_BF1));
			ret = BERR_TRACE(ret);
            goto end;
		}
        
		/* Install the interrupt callback for the FMM to HOST Audio interrupts */
		ret = BINT_CreateCallback(
			&hDstHandle->hFmmBf2Callback,
			hDstHandle->hAssociation->hRap->hInt,
			BCHP_INT_ID_FMM_BF2,
			BRAP_P_FMMBF_DestinationBuf_isr,
			(void*)hDstHandle,
			0 /* Not used */
			);

		if ( ret != BERR_SUCCESS )
		{
			BDBG_ERR(("Create Callback failed for FMM INT_ID[%d]", BCHP_INT_ID_FMM_BF2));
			ret = BERR_TRACE(ret);
            goto end;
		}

		ret = BINT_EnableCallback(hDstHandle->hFmmBf2Callback);		
		if ( ret != BERR_SUCCESS )
		{
			BDBG_ERR(("Enable Callback failed for FMM INT_ID[%d]", BCHP_INT_ID_FMM_BF2));
			ret = BERR_TRACE(ret);
            goto end;
		}
	}
	else
	{   
	    /* Watchdog recovery mode */
    	/* First disable and then enable the callback */

        if (hDstHandle->hFmmBf1Callback)
        {
            ret = BINT_DisableCallback(hDstHandle->hFmmBf1Callback);		
            if ( ret != BERR_SUCCESS )
            {
                BDBG_ERR(("Disable Callback failed for  FMM INT_ID[%d]", BCHP_INT_ID_FMM_BF1));
                ret = BERR_TRACE(ret);
                goto end;
            }
        }

        /* Now enable the callback */
        if (hDstHandle->hFmmBf1Callback)
        {        
            ret = BINT_EnableCallback(hDstHandle->hFmmBf1Callback);		
            if ( ret != BERR_SUCCESS )
            {
                BDBG_ERR(("Enable Callback failed for FMM INT_ID[%d]", BCHP_INT_ID_FMM_BF1));
                ret = BERR_TRACE(ret);
                goto end;
            }
        }        

        if (hDstHandle->hFmmBf2Callback)
        {
            ret = BINT_DisableCallback(hDstHandle->hFmmBf2Callback);		
            if ( ret != BERR_SUCCESS )
            {
                BDBG_ERR(("Disable Callback failed for  FMM INT_ID[%d]", BCHP_INT_ID_FMM_BF2));
                ret = BERR_TRACE(ret);
                goto end;
            }
        }

        /* Now enable the callback */
        if (hDstHandle->hFmmBf2Callback)
        {        
            ret = BINT_EnableCallback(hDstHandle->hFmmBf2Callback);		
            if ( ret != BERR_SUCCESS )
            {
                BDBG_ERR(("Enable Callback failed for FMM INT_ID[%d]", BCHP_INT_ID_FMM_BF2));
                ret = BERR_TRACE(ret);
                goto end;
            }
        }
        
        ui32RegVal =0;
        ui32RegVal |=BCHP_MASK(AIO_INTH_R5F_MASK_CLEAR, FMM_BF2);
        BRAP_Write32_isr(hDstHandle->hAssociation->hRap->hRegister, BCHP_AIO_INTH_R5F_MASK_CLEAR, ui32RegVal);    
	}  
#endif /* ifndef EMULATION */

end:
	BDBG_LEAVE(BRAP_P_DestinationLevelInterruptInstall);
	return ret;    
}

/***************************************************************************
Description:
	This API uninstalls any previously installed callbacks for destination level interrupts.
Returns:
	BERR_Code 
See Also:
	BRAP_P_DestinationLevelInterruptInstall
***************************************************************************/
BERR_Code BRAP_P_DestinationLevelInterruptUnInstall(
	BRAP_DestinationHandle        hDstHandle
)
{
	BERR_Code ret = BERR_SUCCESS;

	BDBG_ENTER (BRAP_P_DestinationLevelInterruptUnInstall);    
	BDBG_ASSERT (hDstHandle);            
    
#ifndef EMULATION /* Do not handle interrupts in emulation */    
    
	/* UnInstall the interrupt callback for FMM to Host interrupts */    
	if(hDstHandle->hFmmBf1Callback)
	{
		ret = BINT_DisableCallback(hDstHandle->hFmmBf1Callback);
		if (ret!=BERR_SUCCESS)
		{
    		ret = BERR_TRACE(ret);
            goto end;
        }

		ret = BINT_DestroyCallback(hDstHandle->hFmmBf1Callback);
		if (ret!=BERR_SUCCESS)
		{
    		ret = BERR_TRACE(ret);
            goto end;
        }
	}

    if(hDstHandle->hFmmBf2Callback)
	{
		ret = BINT_DisableCallback(hDstHandle->hFmmBf2Callback);
		if (ret!=BERR_SUCCESS)
		{
    		ret = BERR_TRACE(ret);
            goto end;
        }

		ret = BINT_DestroyCallback(hDstHandle->hFmmBf2Callback);
		if (ret!=BERR_SUCCESS)
		{
    		ret = BERR_TRACE(ret);
            goto end;
        }
	}    
	BDBG_MSG(("Callbacks destroyed."));
#endif /* ifndef EMULATION */

end:
	BDBG_LEAVE(BRAP_P_DestinationLevelInterruptUnInstall);
	return ret;    
}

#endif
/***************************************************************************
Description:
	This API will be a helper function and will search the TASK in which FW
	is doing decoding.
Returns:
    void
***************************************************************************/
 void BRAP_P_GetTask(
    BRAP_ChannelHandle hRapCh,     
    BRAP_FWIF_P_FwTaskHandle *phTask
)
{    
    int     i=0;    
    BDBG_ENTER(BRAP_P_GetTask);    
    BDBG_ASSERT(hRapCh);
    for(i=0; i<BRAP_P_MAX_PATHS_IN_A_CHAN; ++i)    
    {           
        if(hRapCh->pPath[i] == NULL)
            continue;
        if((hRapCh->pPath[i]->eUsgPath == BRAP_P_UsgPath_eDecodePcm)        
            ||(hRapCh->pPath[i]->eUsgPath == BRAP_P_UsgPath_eDecodeCompress)
#ifdef RAP_GFX_SUPPORT            
            ||(hRapCh->pPath[i]->eUsgPath == BRAP_P_UsgPath_eGfx)
#endif
#ifdef RAP_SCM_SUPPORT
            ||(hRapCh->pPath[i]->eUsgPath == BRAP_P_UsgPath_eScm)
#endif            
            )
        {           
            if(hRapCh->pPath[i]->hDspCh != NULL)
            {
                *phTask = hRapCh->pPath[i]->hDspCh->sFwTaskInfo[0].hFwTask;          
                break;        
            }
        }    
    }    
    BDBG_LEAVE(BRAP_P_GetTask);    
    return;
}

/***************************************************************************
Description:
	This API will unmask the bit for the corrosponding task id. So that FW can 
	raise the interrupt for this task.
Returns:
	BERR_SUCCESS - If successful
***************************************************************************/
BERR_Code   BRAP_P_UnmaskTask(
    BRAP_FWIF_P_FwTaskHandle hTask 
)
{
    BERR_Code ret = BERR_SUCCESS;
    
    BDBG_ENTER(BRAP_P_UnmaskTask);

	BKNI_EnterCriticalSection();
	ret = BRAP_P_UnmaskTask_isr(hTask);
	BKNI_LeaveCriticalSection();

    BDBG_LEAVE(BRAP_P_UnmaskTask);
    return ret;
}

/***************************************************************************
Description:
	This API will unmask the bit for the corrosponding task id. So that FW can 
	raise the interrupt for this task.
Returns:
	BERR_SUCCESS - If successful
***************************************************************************/
static BERR_Code   BRAP_P_UnmaskTask_isr(
    BRAP_FWIF_P_FwTaskHandle hTask 
)
{
    BRAP_DSP_Handle hDsp=NULL;
    uint32_t        ui32RegVal=0, ui32Offset=0;
    BERR_Code       ret = BERR_SUCCESS;

    BDBG_ENTER(BRAP_P_UnmaskTask_isr);
    BDBG_ASSERT(hTask);
    hDsp = hTask->hDsp;
    ui32RegVal |= (0x1 << hTask->uiTaskId);
    
    /*Setting the mask for Synchronous Interrupt*/
    BRAP_Write32_isr(hDsp->hRegister, BCHP_AUD_DSP_ESR_SO00_INT_CLEAR+ 
                    ui32Offset, ui32RegVal);    
    BRAP_Write32_isr(hDsp->hRegister, BCHP_AUD_DSP_ESR_SO00_MASK_CLEAR+ 
                    ui32Offset, ui32RegVal);    

    /*Setting the mask for ASynchronous Interrupt*/
    BRAP_Write32_isr(hDsp->hRegister, BCHP_AUD_DSP_ESR_SO10_INT_CLEAR+ 
                    ui32Offset, ui32RegVal);    
    BRAP_Write32_isr(hDsp->hRegister, BCHP_AUD_DSP_ESR_SO10_MASK_CLEAR+ 
                    ui32Offset, ui32RegVal);    

    BDBG_LEAVE(BRAP_P_UnmaskTask_isr);
    
    return ret;
}

/***************************************************************************
Description:
	This API will mask the bit for the corrosponding task id. So that FW can 
	not raise the interrupt for this task any more.
Returns:
	BERR_SUCCESS - If successful
***************************************************************************/
BERR_Code   BRAP_P_MaskTask
(
    BRAP_FWIF_P_FwTaskHandle hTask 
)
{
    BERR_Code ret = BERR_SUCCESS;
    BDBG_ENTER(BRAP_P_MaskTask);

	BKNI_EnterCriticalSection();
	ret = BRAP_P_MaskTask_isr(hTask);
	BKNI_LeaveCriticalSection();

    BDBG_LEAVE(BRAP_P_MaskTask);
    return ret;
}

/***************************************************************************
Description:
	This API will mask the bit for the corrosponding task id. So that FW can 
	not raise the interrupt for this task any more.
Returns:
	BERR_SUCCESS - If successful
***************************************************************************/
static BERR_Code   BRAP_P_MaskTask_isr(
    BRAP_FWIF_P_FwTaskHandle hTask 
)
{
    BRAP_DSP_Handle hDsp=NULL;
    uint32_t        ui32RegVal=0, ui32Offset=0;
    BERR_Code       ret = BERR_SUCCESS;

    BDBG_ENTER(BRAP_P_MaskTask_isr);
    BDBG_ASSERT(hTask);
    hDsp = hTask->hDsp;
    ui32RegVal |= (0x1 << hTask->uiTaskId);

    /*Setting the mask for synchronous Interrupt*/
    BRAP_Write32_isr(hDsp->hRegister, BCHP_AUD_DSP_ESR_SO00_MASK_SET + 
                    ui32Offset, ui32RegVal);    

    /*Setting the mask for ASynchronous Interrupt*/
    BRAP_Write32_isr(hDsp->hRegister, BCHP_AUD_DSP_ESR_SO10_MASK_SET +
                    ui32Offset, ui32RegVal);    

    BDBG_LEAVE(BRAP_P_MaskTask_isr);
    
    return ret;
}
     
/*****************************************************************************
Summary:
    Used to unmask a particular interrupt.

Description:
    This PI should be used by the application to unmask an interrupt. This can 
    be done only for the interrupts that are handled by Raptor PI and listed in 
    BRAP_Interrupt. For all other interrupts, application has to use the 
    standard INT interface.

Returns:
    BERR_SUCCESS on success
    Error code on failure
******************************************************************************/
BERR_Code BRAP_P_UnmaskInterrupt (
	BRAP_ChannelHandle hRapCh,	/* [in] The RAP channel handle */
    BRAP_Interrupt  eInterrupt  /* [in] The interrupt that needs to be deactivated */
)
{
	BERR_Code ret = BERR_SUCCESS;
    BRAP_FWIF_P_FwTaskHandle hTask = NULL;
    bool bWdogRecovery = false;
    BRAP_FWIF_P_Response sRsp;    
	BDBG_ENTER(BRAP_P_UnmaskInterrupt);
    BDBG_ASSERT(hRapCh);
    bWdogRecovery =  BRAP_P_GetWatchdogRecoveryFlag(hRapCh->hRap);

    BKNI_Memset((void *)&sRsp,0,sizeof(BRAP_FWIF_P_Response));

    if((BRAP_Interrupt_eDspFirstInterrupt <= (signed int)eInterrupt)&&
       (BRAP_Interrupt_eDspLastInterrupt >= eInterrupt)&&
       (hRapCh->eState != BRAP_P_State_eStarted))
    {
        switch(eInterrupt)
        {
            case (BRAP_Interrupt_eBitRateChange):
                hRapCh->ui32AsynIntMask |= BRAP_FWIF_P_EventIdMask_eBitRateChange;
                break;
            case (BRAP_Interrupt_eCrcError):
                hRapCh->ui32AsynIntMask |= BRAP_FWIF_P_EventIdMask_eCrcError;
                break;
            case (BRAP_Interrupt_eModeChange):
                hRapCh->ui32AsynIntMask |= BRAP_FWIF_P_EventIdMask_eAudioModeChange;
                break;
            case (BRAP_Interrupt_eCdbItbOverflow):
                hRapCh->ui32AsynIntMask |= BRAP_FWIF_P_EventIdMask_eCdbItbOverflow;
                    break;
            case (BRAP_Interrupt_eCdbItbUnderflow):
                hRapCh->ui32AsynIntMask |= BRAP_FWIF_P_EventIdMask_eCdbItbUnderflow;
                    break;
            case (BRAP_Interrupt_eStreamInfoAvailable):
                hRapCh->ui32AsynIntMask |= BRAP_FWIF_P_EventIdMask_eStreamInfoAvail;                
                    break;
            case (BRAP_Interrupt_eUnlicensedAlgo):
                hRapCh->ui32AsynIntMask |= BRAP_FWIF_P_EventIdMask_eUnlicensedAlgo;                
                    break;                    
                default :
                    break;
            }
            return ret;
        }

    if((BRAP_Interrupt_eDspFirstInterrupt <= (signed int)eInterrupt)&&
       (BRAP_Interrupt_eDspLastInterrupt >= eInterrupt))
    {    
        BRAP_P_GetTask(hRapCh,&hTask);    
        if(hTask == NULL)
        {   
            BDBG_ERR(("BRAP_P_UnmaskInterrupt: Channel = %0x is not having decode path", 
                        hRapCh));
            BDBG_ASSERT(hTask);
            return BERR_TRACE(BRAP_ERR_INVALID_TASK);
        }
        
        hTask->uiLastEventType = BRAP_FWIF_P_EVENT_NOTIFICATION_COMMAND_ID;        
        BRAP_P_EventReset(hTask->hDsp,BRAP_GET_TASK_INDEX(hTask->uiTaskId));    
    }        
        BKNI_EnterCriticalSection();
        ret = BRAP_P_UnmaskInterrupt_isr(hRapCh,eInterrupt,BRAP_FWIF_P_ResponseType_eAckRequired);
        BKNI_LeaveCriticalSection();
        if(ret != BERR_SUCCESS)
        {
    		return BERR_TRACE(ret);        
        }

    if((BRAP_Interrupt_eDspFirstInterrupt <= (signed int)eInterrupt)&&
       (BRAP_Interrupt_eDspLastInterrupt >= eInterrupt))
    {
        ret = BRAP_P_EventWait(hTask->hDsp, BRAP_DSPCHN_P_EVENT_TIMEOUT_IN_MS,BRAP_GET_TASK_INDEX(hTask->uiTaskId));
        if(BERR_TIMEOUT == ret)
        {
            if((hRapCh->hRap->bWatchdogTriggered == false))
            {
                /* Please note that, If the code reaches at this point then there is a potential Bug in Fw 
                code which needs to be debugged. However Watchdog is being triggered to recover the system*/            
                BDBG_WRN(("BRAP_P_UnmaskInterrupt: Event failed! Triggering Watchdog"));
#if 0                
                BDBG_ASSERT(0);                
#endif
                BRAP_Write32(hTask->hDsp->hRegister, BCHP_AUD_DSP_INTH0_R5F_SET+ hTask->hDsp->ui32Offset,0x1);
                hRapCh->hRap->bWatchdogTriggered  = true;
#if 0                
                err = BERR_TRACE(err);
                goto error;
#endif                 
                ret = BERR_SUCCESS;              
            }
            else
                ret = BERR_SUCCESS;              
        }
        
        if((hRapCh->hRap->bWatchdogTriggered == false)
        &&(hTask->bStopped == false))
        {
            ret = BRAP_FWIF_P_GetMsg(hTask->hSyncMsgQueue, (void *)&sRsp, BRAP_P_MsgType_eSyn);
        }
        if(BERR_SUCCESS != ret)
        {
            if((hRapCh->hRap->bWatchdogTriggered == false)
                &&(hTask->bStopped == false))
            {
                BDBG_ERR(("BRAP_P_UnmaskInterrupt: Unable to read ACK!"));
                return BERR_TRACE(ret);
            }
                else
                    ret = BERR_SUCCESS;
        }

        if((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
            (sRsp.sCommonAckResponseHeader.ui32ResponseID != BRAP_FWIF_P_EVENT_NOTIFICATION_COMMAND_ID)||
            (sRsp.sCommonAckResponseHeader.ui32TaskID != hTask->uiTaskId))
        {
            if((hRapCh->hRap->bWatchdogTriggered == false)
                &&(hTask->bStopped == false))
            {
                BDBG_ERR(("sRsp.sCommonAckResponseHeader.eStatus =%d",sRsp.sCommonAckResponseHeader.eStatus));
                BDBG_ERR(("BRAP_P_UnmaskInterrupt: Event notification not received successfully!"));
                return BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
            }
            else
                ret = BERR_SUCCESS;        
        }
    }
    
	BDBG_LEAVE(BRAP_P_UnmaskInterrupt);
	return ret;
}


/*****************************************************************************
Summary:
    Used to unmask a particular interrupt - isr version.

Description:
    ISR version routine for BRAP_UnmaskInterrupt().
    
Returns:
    BERR_SUCCESS on success
    Error code on failure
******************************************************************************/

BERR_Code BRAP_P_UnmaskInterrupt_isr (
	BRAP_ChannelHandle hRapCh,	/* [in] The RAP channel handle */
    BRAP_Interrupt  eInterrupt,  /* [in] The interrupt that needs to be deactivated */
    BRAP_FWIF_P_ResponseType    eRsp    
)
{
    /*We will do the following steps
    1. Decided for which task the interrupt has been 
    2. Unmask the corrosponding in the SO00 
    3. Send the message in the quere with the corrosponding message.
    This PI can be called on both synchronous as well as for asynchronous interrupts.

    How to decided which interrupt has been installed on which task???
    */

	BERR_Code       ret = BERR_SUCCESS;
	uint32_t        ui32RegValFMM = 0, ui32RegValFMMCrc=0;
	unsigned int    uichannelpair = 0, uiPth = 0;
    BRAP_Handle hRap;

	BDBG_ENTER (BRAP_P_UnmaskInterrupt_isr);

	/* Check input parameters */
	BDBG_ASSERT (hRapCh);     
    hRap = hRapCh->hRap;

	if((eInterrupt == BRAP_Interrupt_eFmmRbufFreeByte) && 
		(hRapCh->eChannelType != BRAP_ChannelType_ePcmPlayback))
	{
        BDBG_ERR(("BRAP_P_MaskInterrupt_isr: eFmmRbufFreeByte should be masked "       
            "\n\tonly for a PCM playback channel"));
		return BERR_TRACE(BERR_NOT_SUPPORTED);
   	}

#ifdef CRC_ENABLE	
		ui32RegValFMMCrc=BRAP_Read32_isr(hRapCh->hRegister,BCHP_AUD_FMM_OP_ESR_MASK_CLEAR); 
#else
		BSTD_UNUSED(ui32RegValFMMCrc);
#endif

	ui32RegValFMM = BRAP_Read32_isr(hRapCh->hRegister, BCHP_AUD_FMM_BF_ESR2_H_MASK_CLEAR);    

	switch (eInterrupt)
	{
		case BRAP_Interrupt_eFmmRbufFreeByte:
            for(uiPth = 0; uiPth < BRAP_P_MAX_PATHS_IN_A_CHAN; uiPth++)
            {
                if (NULL == hRapCh->pPath[uiPth])
                {
                    continue;
                }
                for (uichannelpair = 0; uichannelpair < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; uichannelpair++)
                {
                    if (hRapCh->pPath[uiPth]->sRsrcGrnt.uiSrcChId[uichannelpair] !=  (unsigned int)BRAP_RM_P_INVALID_INDEX)
                    {
                        switch(hRapCh->pPath[uiPth]->sRsrcGrnt.uiSrcChId[uichannelpair])
                        {
                            case 0:
                            	ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_CLEAR, SOURCE_RINGBUF_0_EXCEED_FREEMARK, 1);
                            	break;
                            case 1:
                            	ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_CLEAR, SOURCE_RINGBUF_1_EXCEED_FREEMARK, 1);
                            	break;
                            case 2:
                            	ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_CLEAR, SOURCE_RINGBUF_2_EXCEED_FREEMARK, 1);
                            	break;
                            case 3:
                            	ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_CLEAR, SOURCE_RINGBUF_3_EXCEED_FREEMARK, 1);
                            	break;
                            case 4:
                            	ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_CLEAR, SOURCE_RINGBUF_4_EXCEED_FREEMARK, 1);
                            	break;
                            case 5:
                            	ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_CLEAR, SOURCE_RINGBUF_5_EXCEED_FREEMARK, 1);
                            	break;
                            case 6:
                            	ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_CLEAR, SOURCE_RINGBUF_6_EXCEED_FREEMARK, 1);
                            	break;
                            case 7:
                            	ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_CLEAR, SOURCE_RINGBUF_7_EXCEED_FREEMARK, 1);
                            	break;
#if ((BRAP_7420_FAMILY == 1) || (BRAP_7550_FAMILY == 1))
                            case 8:
                            	ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_CLEAR, SOURCE_RINGBUF_8_EXCEED_FREEMARK, 1);
                            	break;
#endif                                
#if ((BRAP_7420_FAMILY == 1))
                            case 9:
                            	ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_CLEAR, SOURCE_RINGBUF_9_EXCEED_FREEMARK, 1);
                            	break;
#endif
                            default: 
                            	BDBG_ERR(("BRAP_P_UnmaskInterrupt_isr: BRAP_Interrupt_eFmmRbufFreeByte"
                                          "Invalid srcch index %d",
                                          hRapCh->pPath[uiPth]->sRsrcGrnt.uiSrcChId[uichannelpair]));
                            	return BERR_TRACE(BERR_NOT_SUPPORTED);
                             	break;                                
                        }
                    }
                }
            }
        break;
		case BRAP_Interrupt_eBitRateChange:
        {
            BRAP_FWIF_P_Command     sFwCommand;
            BRAP_FWIF_P_FwTaskHandle    hTask = NULL;

            BRAP_P_GetTask(hRapCh, &hTask);
            if(hTask == NULL)
            {   
                BDBG_ERR(("BRAP_P_UnmaskInterrupt_isr: BRAP_Interrupt_eBitRateChange"
                            "Channel = %0x is not having decode path", hRapCh));
                BDBG_ASSERT(hTask);
                return BERR_TRACE(BRAP_ERR_INVALID_TASK);
            }
            /*  Prepare message structure for FW to write in message queue */
            sFwCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_EVENT_NOTIFICATION_COMMAND_ID;
            sFwCommand.sCommandHeader.ui32CommandSizeInBytes = sizeof(BRAP_FWIF_P_Command);
            sFwCommand.sCommandHeader.eResponseType =eRsp;
            sFwCommand.sCommandHeader.ui32TaskID = hTask->uiTaskId;
            sFwCommand.sCommandHeader.ui32CommandCounter = hRap->uiCommandCounter++;;
            sFwCommand.uCommand.sEnableDisableEvent.ui32EnableEvent = 
                hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] | BRAP_FWIF_P_EventIdMask_eBitRateChange;

            /* Write in Message queue */
            ret = BRAP_FWIF_P_SendCommand_isr(hTask->hDsp->hCmdQueue, &sFwCommand,hRap,hTask);
            if(ret != BERR_SUCCESS)
            {        
                if((hRap->bWatchdogTriggered == false)
                &&(hTask->bStopped == false))
                {
                    BDBG_ERR(("BRAP_P_UnmaskInterrupt_isr: Send command failed"));
                    return BERR_TRACE(ret);  
                }
                else
                    ret = BERR_SUCCESS;   
            }
            hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] |= BRAP_FWIF_P_EventIdMask_eBitRateChange;
            break;
        }
		case BRAP_Interrupt_eModeChange:
        {
            BRAP_FWIF_P_Command     sFwCommand;
            BRAP_FWIF_P_FwTaskHandle    hTask = NULL;
            
            BRAP_P_GetTask(hRapCh, &hTask);
            if(hTask == NULL)
            {   
                BDBG_ERR(("BRAP_P_UnmaskInterrupt_isr: BRAP_Interrupt_eModeChange"
                            "Channel = %0x is not having decode path", hRapCh));
                BDBG_ASSERT(hTask);
                return BERR_TRACE(BRAP_ERR_INVALID_TASK);
            }
            /*  Prepare message structure for FW to write in message queue */
            sFwCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_EVENT_NOTIFICATION_COMMAND_ID;
            sFwCommand.sCommandHeader.ui32CommandSizeInBytes = sizeof(BRAP_FWIF_P_Command);
            sFwCommand.sCommandHeader.eResponseType = eRsp;
            sFwCommand.sCommandHeader.ui32TaskID = hTask->uiTaskId;
            sFwCommand.sCommandHeader.ui32CommandCounter = hRap->uiCommandCounter++;;
            sFwCommand.uCommand.sEnableDisableEvent.ui32EnableEvent = 
                hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] | BRAP_FWIF_P_EventIdMask_eAudioModeChange;

            /* Write in Message queue */
            ret = BRAP_FWIF_P_SendCommand_isr(hTask->hDsp->hCmdQueue, &sFwCommand,hRap,hTask);
            if(ret != BERR_SUCCESS)
            {
                if((hRap->bWatchdogTriggered == false)
                &&(hTask->bStopped == false))
                {
                    BDBG_ERR(("BRAP_P_UnmaskInterrupt_isr: Send command failed"));
                    return BERR_TRACE(ret);  
                }
                else
                    ret = BERR_SUCCESS;                
            }
            hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] |= BRAP_FWIF_P_EventIdMask_eAudioModeChange;
            break;
        }
		case BRAP_Interrupt_eCrcError:
        {
            BRAP_FWIF_P_Command     sFwCommand;
            BRAP_FWIF_P_FwTaskHandle    hTask = NULL;
            
            BRAP_P_GetTask(hRapCh, &hTask);
            if(hTask == NULL)
            {   
                BDBG_ERR(("BRAP_P_UnmaskInterrupt_isr: BRAP_Interrupt_eCrcError"
                            "Channel = %0x is not having decode path", hRapCh));
                BDBG_ASSERT(hTask);
                return BERR_TRACE(BRAP_ERR_INVALID_TASK);
            }
            /*  Prepare message structure for FW to write in message queue */
            sFwCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_EVENT_NOTIFICATION_COMMAND_ID;
            sFwCommand.sCommandHeader.ui32CommandSizeInBytes = sizeof(BRAP_FWIF_P_Command);
            sFwCommand.sCommandHeader.eResponseType = eRsp;
            sFwCommand.sCommandHeader.ui32TaskID = hTask->uiTaskId;
            sFwCommand.sCommandHeader.ui32CommandCounter = hRap->uiCommandCounter++;;
            sFwCommand.uCommand.sEnableDisableEvent.ui32EnableEvent = 
                hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] | BRAP_FWIF_P_EventIdMask_eCrcError;

            /* Write in Message queue */
            ret = BRAP_FWIF_P_SendCommand_isr(hTask->hDsp->hCmdQueue, &sFwCommand,hRap,hTask);
            if(ret != BERR_SUCCESS)
            {
                if((hRap->bWatchdogTriggered == false)
                &&(hTask->bStopped == false))
                {
                    BDBG_ERR(("BRAP_P_UnmaskInterrupt_isr: Send command failed"));
                    return BERR_TRACE(ret);  
                }
                else
                    ret = BERR_SUCCESS;               
            }
            hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] |= BRAP_FWIF_P_EventIdMask_eCrcError;
            break;
        }
#ifdef CRC_ENABLE
		case BRAP_Interrupt_eFmmCrc:
		{
			switch(hRapCh->sAppIntCbInfo[BRAP_Interrupt_eFmmCrc].iParm2)
			{
				case 0:
					ui32RegValFMMCrc |= BCHP_FIELD_DATA (AUD_FMM_OP_ESR_MASK_CLEAR, CRC0_INT, 1);
				break;
				
				case 1:
					ui32RegValFMMCrc |= BCHP_FIELD_DATA (AUD_FMM_OP_ESR_MASK_CLEAR, CRC1_INT, 1);					
				break;
				
				case 2:
					ui32RegValFMMCrc |= BCHP_FIELD_DATA (AUD_FMM_OP_ESR_MASK_CLEAR, CRC2_INT, 1);					
				break;
				
				case 3:
					ui32RegValFMMCrc |= BCHP_FIELD_DATA (AUD_FMM_OP_ESR_MASK_CLEAR, CRC3_INT, 1);					
				break;
				
				case 4:
					ui32RegValFMMCrc |= BCHP_FIELD_DATA (AUD_FMM_OP_ESR_MASK_CLEAR, CRC4_INT, 1);					
				break;
				
				case 5:
					ui32RegValFMMCrc |= BCHP_FIELD_DATA (AUD_FMM_OP_ESR_MASK_CLEAR, CRC5_INT, 1);					
				break;
				
				case 6:
					ui32RegValFMMCrc |= BCHP_FIELD_DATA (AUD_FMM_OP_ESR_MASK_CLEAR, CRC6_INT, 1);					
				break;
				
				case 7:
					ui32RegValFMMCrc |= BCHP_FIELD_DATA (AUD_FMM_OP_ESR_MASK_CLEAR, CRC7_INT, 1);					
				break;

			}
		}		
		break;
#endif	
	case BRAP_Interrupt_eStreamInfoAvailable:
        {
            BRAP_FWIF_P_Command     sFwCommand;
            BRAP_FWIF_P_FwTaskHandle    hTask = NULL;
            
            BRAP_P_GetTask(hRapCh, &hTask);
            if(hTask == NULL)
            {   
                BDBG_ERR(("BRAP_P_UnmaskInterrupt_isr: BRAP_FWIF_P_EventIdMask_eStreamInfoAvail"
                            "Channel = %0x is not having decode path", hRapCh));
                BDBG_ASSERT(hTask);
                return BERR_TRACE(BRAP_ERR_INVALID_TASK);
            }
            /*  Prepare message structure for FW to write in message queue */
            sFwCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_EVENT_NOTIFICATION_COMMAND_ID;
            sFwCommand.sCommandHeader.ui32CommandSizeInBytes = sizeof(BRAP_FWIF_P_Command);
            sFwCommand.sCommandHeader.eResponseType = eRsp;
            sFwCommand.sCommandHeader.ui32TaskID = hTask->uiTaskId;
            sFwCommand.sCommandHeader.ui32CommandCounter = hRap->uiCommandCounter++;;
            sFwCommand.uCommand.sEnableDisableEvent.ui32EnableEvent = 
                hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] | BRAP_FWIF_P_EventIdMask_eStreamInfoAvail;

            /* Write in Message queue */
            ret = BRAP_FWIF_P_SendCommand_isr(hTask->hDsp->hCmdQueue, &sFwCommand,hRap,hTask);
            if(ret != BERR_SUCCESS)
            {
                if((hRap->bWatchdogTriggered == false)
                &&(hTask->bStopped == false))
                {
                    BDBG_ERR(("BRAP_P_UnmaskInterrupt_isr: Send command failed"));
                    return BERR_TRACE(ret);  
                }
                else
                    ret = BERR_SUCCESS;                
            }
            hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] |= BRAP_FWIF_P_EventIdMask_eStreamInfoAvail;
        }
            break;
    	case BRAP_Interrupt_eCdbItbOverflow:
        {
            BRAP_FWIF_P_Command     sFwCommand;
            BRAP_FWIF_P_FwTaskHandle    hTask = NULL;
            
            BRAP_P_GetTask(hRapCh, &hTask);
            if(hTask == NULL)
            {   
                BDBG_ERR(("BRAP_P_UnmaskInterrupt_isr: BRAP_Interrupt_eCdbItbOverflow"
                            "Channel = %0x is not having decode path", hRapCh));
                BDBG_ASSERT(hTask);
                return BERR_TRACE(BRAP_ERR_INVALID_TASK);
        }
            /*  Prepare message structure for FW to write in message queue */
            sFwCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_EVENT_NOTIFICATION_COMMAND_ID;
            sFwCommand.sCommandHeader.ui32CommandSizeInBytes = sizeof(BRAP_FWIF_P_Command);
            sFwCommand.sCommandHeader.eResponseType = eRsp;
            sFwCommand.sCommandHeader.ui32TaskID = hTask->uiTaskId;
            sFwCommand.sCommandHeader.ui32CommandCounter = hRap->uiCommandCounter++;;
            sFwCommand.uCommand.sEnableDisableEvent.ui32EnableEvent = 
                hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] | BRAP_FWIF_P_EventIdMask_eCdbItbOverflow;

            /* Write in Message queue */
            ret = BRAP_FWIF_P_SendCommand_isr(hTask->hDsp->hCmdQueue, &sFwCommand,hRap,hTask);
            if(ret != BERR_SUCCESS)
            {
                if((hRap->bWatchdogTriggered == false)
                &&(hTask->bStopped == false))
                {
                    BDBG_ERR(("BRAP_P_UnmaskInterrupt_isr: Send command failed"));
                    return BERR_TRACE(ret);  
                }
                else
                    ret = BERR_SUCCESS;                
            }
            hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] |= BRAP_FWIF_P_EventIdMask_eCdbItbOverflow;
        }        
        break;        
    	case BRAP_Interrupt_eCdbItbUnderflow:
        {
            BRAP_FWIF_P_Command     sFwCommand;
            BRAP_FWIF_P_FwTaskHandle    hTask = NULL;
            
            BRAP_P_GetTask(hRapCh, &hTask);
            if(hTask == NULL)
            {   
                BDBG_ERR(("BRAP_P_UnmaskInterrupt_isr: BRAP_Interrupt_eCdbItbUnderflow"
                            "Channel = %0x is not having decode path", hRapCh));
                BDBG_ASSERT(hTask);
                return BERR_TRACE(BRAP_ERR_INVALID_TASK);
            }
            /*  Prepare message structure for FW to write in message queue */
            sFwCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_EVENT_NOTIFICATION_COMMAND_ID;
            sFwCommand.sCommandHeader.ui32CommandSizeInBytes = sizeof(BRAP_FWIF_P_Command);
            sFwCommand.sCommandHeader.eResponseType = eRsp;
            sFwCommand.sCommandHeader.ui32TaskID = hTask->uiTaskId;
            sFwCommand.sCommandHeader.ui32CommandCounter = hRap->uiCommandCounter++;;
            sFwCommand.uCommand.sEnableDisableEvent.ui32EnableEvent = 
                hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] | BRAP_FWIF_P_EventIdMask_eCdbItbUnderflow;

            /* Write in Message queue */
            ret = BRAP_FWIF_P_SendCommand_isr(hTask->hDsp->hCmdQueue, &sFwCommand,hRap,hTask);
            if(ret != BERR_SUCCESS)
            {
                if((hRap->bWatchdogTriggered == false)
                &&(hTask->bStopped == false))
                {
                    BDBG_ERR(("BRAP_P_UnmaskInterrupt_isr: Send command failed"));
                    return BERR_TRACE(ret);  
        }
                else
                    ret = BERR_SUCCESS;                
            }
            hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] |= BRAP_FWIF_P_EventIdMask_eCdbItbUnderflow;
        }        
        break;   
    	case BRAP_Interrupt_eUnlicensedAlgo:
        {
            BRAP_FWIF_P_Command     sFwCommand;
            BRAP_FWIF_P_FwTaskHandle    hTask = NULL;
            
            BRAP_P_GetTask(hRapCh, &hTask);
            if(hTask == NULL)
            {   
                BDBG_ERR(("BRAP_P_UnmaskInterrupt_isr: BRAP_FWIF_P_EventIdMask_eUnlicensedAlgo"
                            "Channel = %0x is not having decode path", hRapCh));
                BDBG_ASSERT(hTask);
                return BERR_TRACE(BRAP_ERR_INVALID_TASK);
            }
            /*  Prepare message structure for FW to write in message queue */
            sFwCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_EVENT_NOTIFICATION_COMMAND_ID;
            sFwCommand.sCommandHeader.ui32CommandSizeInBytes = sizeof(BRAP_FWIF_P_Command);
            sFwCommand.sCommandHeader.eResponseType = eRsp;
            sFwCommand.sCommandHeader.ui32TaskID = hTask->uiTaskId;
            sFwCommand.sCommandHeader.ui32CommandCounter = hRap->uiCommandCounter++;;
            sFwCommand.uCommand.sEnableDisableEvent.ui32EnableEvent = 
                hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] | BRAP_FWIF_P_EventIdMask_eUnlicensedAlgo;

            /* Write in Message queue */
            ret = BRAP_FWIF_P_SendCommand_isr(hTask->hDsp->hCmdQueue, &sFwCommand,hRap,hTask);
            if(ret != BERR_SUCCESS)
            {
                if((hRap->bWatchdogTriggered == false)
                &&(hTask->bStopped == false))
                {
                    BDBG_ERR(("BRAP_P_UnmaskInterrupt_isr: Send command failed"));
                    return BERR_TRACE(ret);  
        }
                else
                    ret = BERR_SUCCESS;                
            }
            hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] |= BRAP_FWIF_P_EventIdMask_eUnlicensedAlgo;
        }        
        break;         
        default:
            /*Do nothing */
            break;
    }
                    
	BDBG_MSG(("BCHP_AUD_FMM_BF_ESR2_H_MASK_CLEAR = %x", ui32RegValFMM));
	BRAP_Write32_isr(hRapCh->hRegister, BCHP_AUD_FMM_BF_ESR2_H_MASK_CLEAR, ui32RegValFMM);

#ifdef CRC_ENABLE
	BRAP_Write32_isr(hRapCh->hRegister, BCHP_AUD_FMM_OP_ESR_MASK_CLEAR, ui32RegValFMMCrc);
#endif

    BDBG_LEAVE (BRAP_P_UnmaskInterrupt_isr);
    return ret;
}

/*****************************************************************************
Summary:
    Used to unmask a particular interrupt.

Description:
    This PI should be used by the application to unmask an interrupt. This can 
    be done only for the interrupts that are handled by Raptor PI at Destination 
    level and listed in BRAP_DestinationInterrupt. For all other interrupts, 
    application has to use the standard INT interface.

Returns:
    BERR_SUCCESS on success
    Error code on failure
******************************************************************************/
BERR_Code BRAP_P_UnmaskDestinationInterrupt (
	BRAP_DestinationHandle        hDstHandle,	   /* [in] The RAP destination handle */
    BRAP_DestinationInterrupt     eInterrupt  /* [in] The interrupt that needs to be deactivated */
)
{
	BERR_Code ret = BERR_SUCCESS;

	BDBG_ENTER(BRAP_P_UnmaskDestinationInterrupt);
    BDBG_ASSERT(hDstHandle);
          
    BKNI_EnterCriticalSection();
    ret = BRAP_P_UnmaskDestinationInterrupt_isr(hDstHandle,eInterrupt);
    BKNI_LeaveCriticalSection();
    if(ret != BERR_SUCCESS)
    {
    	return BERR_TRACE(ret);        
    }   
    
	BDBG_LEAVE(BRAP_P_UnmaskDestinationInterrupt);
	return ret;
}

/*****************************************************************************
Summary:
    Used to unmask a particular interrupt - isr version.

Description:
    ISR version routine for BRAP_P_UnmaskDestinationInterrupt().
    
Returns:
    BERR_SUCCESS on success
    Error code on failure
******************************************************************************/

BERR_Code BRAP_P_UnmaskDestinationInterrupt_isr (
	BRAP_DestinationHandle        hDstHandle,	   /* [in] The RAP destination handle */
    BRAP_DestinationInterrupt     eInterrupt  /* [in] The interrupt that needs to be deactivated */
)
{
	BERR_Code       ret = BERR_SUCCESS;

#if (BRAP_7550_FAMILY !=1)
	uint32_t        ui32RegValFMM = 0;
	unsigned int    uichannelpair = 0;
#endif    

	BDBG_ENTER (BRAP_P_UnmaskDestinationInterrupt_isr);

	/* Check input parameters */
	BDBG_ASSERT (hDstHandle);     

    switch (eInterrupt)
    {
#if (BRAP_7550_FAMILY !=1)    
        case BRAP_DestinationInterrupt_eFmmRbufFullMark:
            ui32RegValFMM = BRAP_Read32_isr(hDstHandle->hAssociation->hRap->hRegister, BCHP_AUD_FMM_BF_ESR2_H_MASK_CLEAR);
            for (uichannelpair = 0; uichannelpair < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; uichannelpair++)
        	{
        		if (hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiDstChId[uichannelpair] != (unsigned int)BRAP_RM_P_INVALID_INDEX)
        		{
        		    switch(hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiDstChId[uichannelpair])
        		    {
        		       	case 0:
        			        ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_CLEAR, DEST_RINGBUF_0_EXCEED_FULLMARK, 1);
        		            break;

        				case 1:
        					ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_CLEAR, DEST_RINGBUF_1_EXCEED_FULLMARK, 1);
        		            break;

        				case 2:
        					ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_CLEAR, DEST_RINGBUF_2_EXCEED_FULLMARK, 1);
        		            break;							

        				case 3:
        					ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_CLEAR, DEST_RINGBUF_3_EXCEED_FULLMARK, 1);
        		            break;							

        				default:
        					break;
        			}
        		}
            } 
            BDBG_MSG(("BCHP_AUD_FMM_BF_ESR2_H_MASK_CLEAR = %x", ui32RegValFMM));
	        BRAP_Write32_isr(hDstHandle->hAssociation->hRap->hRegister, BCHP_AUD_FMM_BF_ESR2_H_MASK_CLEAR, ui32RegValFMM);            
            break;
            
        case BRAP_DestinationInterrupt_eFmmRbufOverflow:
            ui32RegValFMM = BRAP_Read32_isr(hDstHandle->hAssociation->hRap->hRegister, BCHP_AUD_FMM_BF_ESR1_H_MASK_CLEAR);
            for (uichannelpair = 0; uichannelpair < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; uichannelpair++)
        	{
        		if (hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiDstChId[uichannelpair] != (unsigned int)BRAP_RM_P_INVALID_INDEX)
        		{
        		    switch(hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiDstChId[uichannelpair])
        		    {
        		       	case 0:
        			        ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR1_H_MASK_CLEAR, DEST_RINGBUF_0_OVERFLOW, 1);
        		            break;

        				case 1:
        					ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR1_H_MASK_CLEAR, DEST_RINGBUF_1_OVERFLOW, 1);
        		            break;

        				case 2:
        					ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR1_H_MASK_CLEAR, DEST_RINGBUF_2_OVERFLOW, 1);
        		            break;							

        				case 3:
        					ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR1_H_MASK_CLEAR, DEST_RINGBUF_3_OVERFLOW, 1);
        		            break;							

        				default:
        					break;
        			}
        		}
            }            
            BDBG_MSG(("BCHP_AUD_FMM_BF_ESR1_H_MASK_CLEAR = %x", ui32RegValFMM));
	        BRAP_Write32_isr(hDstHandle->hAssociation->hRap->hRegister, BCHP_AUD_FMM_BF_ESR1_H_MASK_CLEAR, ui32RegValFMM);            
            break;
#endif
        case BRAP_DestinationInterrupt_eSampleRateChange:
            hDstHandle->bSampleRateChangeCallbackEnabled = true;
            break;
        default:
            break;
    }	

    BDBG_LEAVE (BRAP_P_UnmaskDestinationInterrupt_isr);
    return ret;
}

/*****************************************************************************
Summary:
    Used to mask a particular interrupt.

Description:
    This PI should be used by the application to mask an interrupt. This can 
    be done only for the interrupts that are handled by Raptor PI and listed in 
    BRAP_Interrupt. For all other interrupts, application has to use the 
    standard INT interface.

Returns:
    BERR_SUCCESS on success
    Error code on failure
******************************************************************************/
BERR_Code BRAP_P_MaskInterrupt (
    BRAP_ChannelHandle hRapCh,	/* [in] The RAP channel handle */
    BRAP_Interrupt  eInterrupt  /* [in] The interrupt that needs to be deactivated */
)
{
    BERR_Code ret = BERR_SUCCESS;
    BRAP_FWIF_P_FwTaskHandle hTask = NULL;
    BRAP_FWIF_P_Response sRsp;
    
    BDBG_ENTER(BRAP_P_MaskInterrupt);

    BKNI_Memset((void *)&sRsp,0,sizeof(BRAP_FWIF_P_Response));

    if((hRapCh->eChannelType == BRAP_ChannelType_eDecode)&&
       (BRAP_Interrupt_eDspFirstInterrupt <= (signed int)eInterrupt)&&
       (BRAP_Interrupt_eDspLastInterrupt >= eInterrupt)&&
       (hRapCh->eState != BRAP_P_State_eStarted))
    {
        switch(eInterrupt)
        {
            case (BRAP_Interrupt_eBitRateChange):
                hRapCh->ui32AsynIntMask &= ~(BRAP_FWIF_P_EventIdMask_eBitRateChange);
                break;
            case (BRAP_Interrupt_eCrcError):
                hRapCh->ui32AsynIntMask &= ~(BRAP_FWIF_P_EventIdMask_eCrcError);
                break;
            case (BRAP_Interrupt_eModeChange):
                hRapCh->ui32AsynIntMask &= ~(BRAP_FWIF_P_EventIdMask_eAudioModeChange);
                break;
            case (BRAP_Interrupt_eCdbItbOverflow):
                hRapCh->ui32AsynIntMask &= ~(BRAP_FWIF_P_EventIdMask_eCdbItbOverflow);
                break;       
            case (BRAP_Interrupt_eCdbItbUnderflow):
                hRapCh->ui32AsynIntMask &= ~(BRAP_FWIF_P_EventIdMask_eCdbItbUnderflow);
                break;                    
            case (BRAP_Interrupt_eStreamInfoAvailable):
                hRapCh->ui32AsynIntMask &= ~(BRAP_FWIF_P_EventIdMask_eStreamInfoAvail);
                break;    
            case (BRAP_Interrupt_eUnlicensedAlgo):
                hRapCh->ui32AsynIntMask &= ~(BRAP_FWIF_P_EventIdMask_eUnlicensedAlgo);
                break;                   
            default :                        
                break;
        }    
        return ret;        
    }
    if((BRAP_Interrupt_eDspFirstInterrupt <= (signed int)eInterrupt)&&
       (BRAP_Interrupt_eDspLastInterrupt >= eInterrupt))
    {    
        BRAP_P_GetTask(hRapCh,&hTask);    
        if(hTask == NULL)
        {   
            BDBG_ERR(("BRAP_P_UnmaskInterrupt: Channel = %0x is not having decode path", 
                        hRapCh));
            BDBG_ASSERT(hTask);
            return BERR_TRACE(BRAP_ERR_INVALID_TASK);
        }
        hTask->uiLastEventType = BRAP_FWIF_P_EVENT_NOTIFICATION_COMMAND_ID;
        BRAP_P_EventReset(hTask->hDsp,BRAP_GET_TASK_INDEX(hTask->uiTaskId));    
    }        
    BKNI_EnterCriticalSection();
    ret = BRAP_P_MaskInterrupt_isr(hRapCh, eInterrupt,BRAP_FWIF_P_ResponseType_eAckRequired);
    BKNI_LeaveCriticalSection();
    if(ret != BERR_SUCCESS)
    {
		return BERR_TRACE(ret);        
    }
        
    if((BRAP_Interrupt_eDspFirstInterrupt <= (signed int)eInterrupt)&&
       (BRAP_Interrupt_eDspLastInterrupt >= eInterrupt))
    {
        ret = BRAP_P_EventWait(hTask->hDsp, BRAP_DSPCHN_P_EVENT_TIMEOUT_IN_MS,BRAP_GET_TASK_INDEX(hTask->uiTaskId));
        if(BERR_TIMEOUT == ret)
        {
            if((hRapCh->hRap->bWatchdogTriggered == false))
            {
                /* Please note that, If the code reaches at this point then there is a potential Bug in Fw 
                code which needs to be debugged. However Watchdog is being triggered to recover the system*/            
                BDBG_WRN(("BRAP_P_MaskInterrupt: Event failed! Triggering Watchdog"));
#if 0                
                BDBG_ASSERT(0);                
#endif
                BRAP_Write32(hTask->hDsp->hRegister, BCHP_AUD_DSP_INTH0_R5F_SET+ hTask->hDsp->ui32Offset,0x1);
                hRapCh->hRap->bWatchdogTriggered  = true;
#if 0                
                err = BERR_TRACE(err);
                goto error;
#endif                 
                ret = BERR_SUCCESS;              
            }
            else
                ret = BERR_SUCCESS;              
        }
        
        if((hRapCh->hRap->bWatchdogTriggered == false)
        &&(hTask->bStopped == false))
        {
            ret = BRAP_FWIF_P_GetMsg(hTask->hSyncMsgQueue, (void *)&sRsp, BRAP_P_MsgType_eSyn);
        }
        if(BERR_SUCCESS != ret)
        {
            if((hRapCh->hRap->bWatchdogTriggered == false)
                &&(hTask->bStopped == false))
            {
                BDBG_ERR(("BRAP_P_MaskInterrupt: Unable to read ACK!"));
                return BERR_TRACE(ret);
            }
                else
                    ret = BERR_SUCCESS;
        }

        if((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
            (sRsp.sCommonAckResponseHeader.ui32ResponseID != BRAP_FWIF_P_EVENT_NOTIFICATION_COMMAND_ID)||
            (sRsp.sCommonAckResponseHeader.ui32TaskID != hTask->uiTaskId))
        {
            if((hRapCh->hRap->bWatchdogTriggered == false)
                &&(hTask->bStopped == false))
            {
                BDBG_ERR(("sRsp.sCommonAckResponseHeader.eStatus =%d",sRsp.sCommonAckResponseHeader.eStatus));
                BDBG_ERR(("BRAP_P_MaskInterrupt: Event notification not received successfully!"));
                return BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
            }
            else
                ret = BERR_SUCCESS;        
        }
   
    }
    BDBG_LEAVE(BRAP_P_MaskInterrupt);
	return ret;
}


/*****************************************************************************
Summary:
    Used to unmask a particular interrupt - isr version.

Description:
    ISR version routine for BRAP_P_UnmaskInterrupt().
    
Returns:
    BERR_SUCCESS on success
    Error code on failure
******************************************************************************/
BERR_Code BRAP_P_MaskInterrupt_isr (
	BRAP_ChannelHandle hRapCh,	/* [in] The RAP channel handle */
    BRAP_Interrupt  eInterrupt,  /* [in] The interrupt that needs to be deactivated */
    BRAP_FWIF_P_ResponseType    eRsp
)
{
	BERR_Code       ret = BERR_SUCCESS;
	uint32_t        ui32RegValFMM = 0, ui32RegValFMMCrc=0;
	unsigned int    uichannelpair = 0, uiPth = 0;

     BRAP_Handle hRap;
	BDBG_ENTER (BRAP_P_MaskInterrupt_isr);

	/* Check input parameters */
	BDBG_ASSERT (hRapCh);     
    
        hRap = hRapCh->hRap;
	if((eInterrupt == BRAP_Interrupt_eFmmRbufFreeByte) && 
		(hRapCh->eChannelType != BRAP_ChannelType_ePcmPlayback))
	{
        BDBG_ERR(("BRAP_P_MaskInterrupt_isr: eFmmRbufFreeByte should be masked "       
            "\n\tonly for a PCM playback channel"));
		return BERR_TRACE(BERR_NOT_SUPPORTED);
   	}

#ifdef CRC_ENABLE
		ui32RegValFMMCrc=BRAP_Read32_isr(hRapCh->hRegister,
										BCHP_AUD_FMM_OP_ESR_MASK_SET);
#else
		BSTD_UNUSED(ui32RegValFMMCrc);
#endif		

	switch (eInterrupt)
	{
		case BRAP_Interrupt_eFmmRbufFreeByte:
            for(uiPth = 0; uiPth < BRAP_P_MAX_PATHS_IN_A_CHAN; uiPth++)
            {
			    if (NULL == hRapCh->pPath[uiPth])
		        {
		            continue;
		        }
			    for (uichannelpair = 0; uichannelpair < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; uichannelpair++)
    			{
    			    if (hRapCh->pPath[uiPth]->sRsrcGrnt.uiSrcChId[uichannelpair] !=  (unsigned int)BRAP_RM_P_INVALID_INDEX)
    				{
    				    switch(hRapCh->pPath[uiPth]->sRsrcGrnt.uiSrcChId[uichannelpair])
    					{
    						case 0:
    							ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_SET, SOURCE_RINGBUF_0_EXCEED_FREEMARK, 1);
    							break;
    						case 1:
    							ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_SET, SOURCE_RINGBUF_1_EXCEED_FREEMARK, 1);
    							break;
    						case 2:
    							ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_SET, SOURCE_RINGBUF_2_EXCEED_FREEMARK, 1);
    							break;
    						case 3:
    							ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_SET, SOURCE_RINGBUF_3_EXCEED_FREEMARK, 1);
    							break;
    						case 4:
    							ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_SET, SOURCE_RINGBUF_4_EXCEED_FREEMARK, 1);
    							break;
    						case 5:
    							ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_SET, SOURCE_RINGBUF_5_EXCEED_FREEMARK, 1);
    							break;
    						case 6:
    							ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_SET, SOURCE_RINGBUF_6_EXCEED_FREEMARK, 1);
    							break;
    						case 7:
    							ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_SET, SOURCE_RINGBUF_7_EXCEED_FREEMARK, 1);
    							break;
#if ((BRAP_7420_FAMILY == 1) || (BRAP_7550_FAMILY == 1))
    						case 8:
    							ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_SET, SOURCE_RINGBUF_8_EXCEED_FREEMARK, 1);
    							break;
#endif                                
#if (BRAP_7420_FAMILY == 1)                                                                
    						case 9:
    							ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_SET, SOURCE_RINGBUF_9_EXCEED_FREEMARK, 1);
    							break;                                
#endif                                
    						default: 
    							BDBG_ERR(("BRAP_P_UnmaskInterrupt_isr: Invalid srcch index %d",
    										hRapCh->pPath[uiPth]->sRsrcGrnt.uiSrcChId[uichannelpair]));
    							return BERR_TRACE(BERR_NOT_SUPPORTED);
    						 	break;
    					}
    				}
    			}
			}
        break;
		case BRAP_Interrupt_eBitRateChange:
        {
            BRAP_FWIF_P_Command     sFwCommand;
            BRAP_FWIF_P_FwTaskHandle    hTask = NULL;
            
            BRAP_P_GetTask(hRapCh, &hTask);
            if(hTask == NULL)
            {   
                BDBG_ERR(("BRAP_P_MaskInterrupt_isr: BRAP_Interrupt_eBitRateChange"
                            "Channel = %0x is not having decode path", hRapCh));
                BDBG_ASSERT(hTask);
                return BERR_TRACE(BRAP_ERR_INVALID_TASK);
            }
            /*  Prepare message structure for FW to write in message queue */
            sFwCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_EVENT_NOTIFICATION_COMMAND_ID;
            sFwCommand.sCommandHeader.ui32CommandSizeInBytes = sizeof(BRAP_FWIF_P_Command);
            sFwCommand.sCommandHeader.eResponseType = eRsp;
            sFwCommand.sCommandHeader.ui32TaskID = hTask->uiTaskId;
            sFwCommand.sCommandHeader.ui32CommandCounter = hRap->uiCommandCounter++;;
            sFwCommand.uCommand.sEnableDisableEvent.ui32EnableEvent = 
                hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] & (~(BRAP_FWIF_P_EventIdMask_eBitRateChange));

            /* Write in Message queue */
            ret = BRAP_FWIF_P_SendCommand_isr(hTask->hDsp->hCmdQueue, &sFwCommand,hRap,hTask);
            if(ret != BERR_SUCCESS)
            {
                if((hRap->bWatchdogTriggered == false)
                &&(hTask->bStopped == false))
                {
                    BDBG_ERR(("BRAP_P_maskInterrupt_isr: Send command failed"));
                    return BERR_TRACE(ret);  
                }
                else
                    ret = BERR_SUCCESS;                
            }
            hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] &= (~(BRAP_FWIF_P_EventIdMask_eBitRateChange));
            break;
        }
		case BRAP_Interrupt_eModeChange:
        {
            BRAP_FWIF_P_Command     sFwCommand;
            BRAP_FWIF_P_FwTaskHandle    hTask = NULL;
            
            BRAP_P_GetTask(hRapCh, &hTask);            
            if(hTask == NULL)
            {   
                BDBG_ERR(("BRAP_P_MaskInterrupt_isr: BRAP_Interrupt_eModeChange"
                            "Channel = %0x is not having decode path", hRapCh));
                BDBG_ASSERT(hTask);
                return BERR_TRACE(BRAP_ERR_INVALID_TASK);
            }
            /*  Prepare message structure for FW to write in message queue */
            sFwCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_EVENT_NOTIFICATION_COMMAND_ID;
            sFwCommand.sCommandHeader.ui32CommandSizeInBytes = sizeof(BRAP_FWIF_P_Command);
            sFwCommand.sCommandHeader.eResponseType = eRsp;
            sFwCommand.sCommandHeader.ui32TaskID = hTask->uiTaskId;
            sFwCommand.sCommandHeader.ui32CommandCounter = hRap->uiCommandCounter++;;
            sFwCommand.uCommand.sEnableDisableEvent.ui32EnableEvent = 
                hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] & (~(BRAP_FWIF_P_EventIdMask_eAudioModeChange));

            /* Write in Message queue */
            ret = BRAP_FWIF_P_SendCommand_isr(hTask->hDsp->hCmdQueue, &sFwCommand,hRap,hTask);
            if(ret != BERR_SUCCESS)
            {
                if((hRap->bWatchdogTriggered == false)
                &&(hTask->bStopped == false))
                {
                    BDBG_ERR(("BRAP_P_maskInterrupt_isr: Send command failed"));
                    return BERR_TRACE(ret);  
                }
                else
                    ret = BERR_SUCCESS;            
            }
            hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] &= (~(BRAP_FWIF_P_EventIdMask_eAudioModeChange));
            break;
        }
		case BRAP_Interrupt_eCrcError:
        {
            BRAP_FWIF_P_Command     sFwCommand;
            BRAP_FWIF_P_FwTaskHandle    hTask = NULL;
                        
            BRAP_P_GetTask(hRapCh, &hTask);            
            if(hTask == NULL)
            {   
                BDBG_ERR(("BRAP_P_MaskInterrupt_isr: BRAP_Interrupt_eCrcError"
                            "Channel = %0x is not having decode path", hRapCh));
                BDBG_ASSERT(hTask);
                return BERR_TRACE(BRAP_ERR_INVALID_TASK);
            }
            /*  Prepare message structure for FW to write in message queue */
            sFwCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_EVENT_NOTIFICATION_COMMAND_ID;
            sFwCommand.sCommandHeader.ui32CommandSizeInBytes = sizeof(BRAP_FWIF_P_Command);
            sFwCommand.sCommandHeader.eResponseType = eRsp;
            sFwCommand.sCommandHeader.ui32TaskID = hTask->uiTaskId;
            sFwCommand.sCommandHeader.ui32CommandCounter = hRap->uiCommandCounter++;
            sFwCommand.uCommand.sEnableDisableEvent.ui32EnableEvent = 
                hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] & (~(BRAP_FWIF_P_EventIdMask_eCrcError));

            /* Write in Message queue */
            ret = BRAP_FWIF_P_SendCommand_isr(hTask->hDsp->hCmdQueue, &sFwCommand,hRap,hTask);
            if(ret != BERR_SUCCESS)
            {
                if((hRap->bWatchdogTriggered == false)
                &&(hTask->bStopped == false))
                {
                    BDBG_ERR(("BRAP_P_maskInterrupt_isr: Send command failed"));
                    return BERR_TRACE(ret);  
                }
                else
                    ret = BERR_SUCCESS;             
            }
            hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] &= (~(BRAP_FWIF_P_EventIdMask_eCrcError));
            break;
        }

#ifdef CRC_ENABLE
		case BRAP_Interrupt_eFmmCrc:
		{
			switch(hRapCh->sAppIntCbInfo[BRAP_Interrupt_eFmmCrc].iParm2)
			{
				case 0:
					ui32RegValFMMCrc |= BCHP_FIELD_DATA (AUD_FMM_OP_ESR_MASK_SET, CRC0_INT, 1);
				break;
				
				case 1:
					ui32RegValFMMCrc |= BCHP_FIELD_DATA (AUD_FMM_OP_ESR_MASK_SET, CRC1_INT, 1);					
				break;
				
				case 2:
					ui32RegValFMMCrc |= BCHP_FIELD_DATA (AUD_FMM_OP_ESR_MASK_SET, CRC2_INT, 1);					
				break;
				
				case 3:
					ui32RegValFMMCrc |= BCHP_FIELD_DATA (AUD_FMM_OP_ESR_MASK_SET, CRC3_INT, 1);					
				break;
				
				case 4:
					ui32RegValFMMCrc |= BCHP_FIELD_DATA (AUD_FMM_OP_ESR_MASK_SET, CRC4_INT, 1);					
				break;
				
				case 5:
					ui32RegValFMMCrc |= BCHP_FIELD_DATA (AUD_FMM_OP_ESR_MASK_SET, CRC5_INT, 1);					
				break;
				
				case 6:
					ui32RegValFMMCrc |= BCHP_FIELD_DATA (AUD_FMM_OP_ESR_MASK_SET, CRC6_INT, 1);					
				break;
				
				case 7:
					ui32RegValFMMCrc |= BCHP_FIELD_DATA (AUD_FMM_OP_ESR_MASK_SET, CRC7_INT, 1);					
				break;

			}
		}		
		break;
#endif	
        case BRAP_Interrupt_eStreamInfoAvailable:
        {
            BRAP_FWIF_P_Command     sFwCommand;
            BRAP_FWIF_P_FwTaskHandle    hTask = NULL;
            
            BRAP_P_GetTask(hRapCh, &hTask);            
            if(hTask == NULL)
            {   
                BDBG_ERR(("BRAP_P_MaskInterrupt_isr: BRAP_FWIF_P_EventIdMask_eStreamInfoAvail"
                            "Channel = %0x is not having decode path", hRapCh));
                BDBG_ASSERT(hTask);
                return BERR_TRACE(BRAP_ERR_INVALID_TASK);
            }
            /*  Prepare message structure for FW to write in message queue */
            sFwCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_EVENT_NOTIFICATION_COMMAND_ID;
            sFwCommand.sCommandHeader.ui32CommandSizeInBytes = sizeof(BRAP_FWIF_P_Command);
            sFwCommand.sCommandHeader.eResponseType = eRsp;
            sFwCommand.sCommandHeader.ui32TaskID = hTask->uiTaskId;
            sFwCommand.sCommandHeader.ui32CommandCounter = hRap->uiCommandCounter++;
            sFwCommand.uCommand.sEnableDisableEvent.ui32EnableEvent = 
                hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] & (~(BRAP_FWIF_P_EventIdMask_eStreamInfoAvail));

            /* Write in Message queue */
            ret = BRAP_FWIF_P_SendCommand_isr(hTask->hDsp->hCmdQueue, &sFwCommand,hRap,hTask);
            if(ret != BERR_SUCCESS)
            {
                if((hRap->bWatchdogTriggered == false)
                &&(hTask->bStopped == false))
                {
                    BDBG_ERR(("BRAP_P_maskInterrupt_isr: Send command failed"));
                    return BERR_TRACE(ret);  
                }
                else
                    ret = BERR_SUCCESS;            
            }
            hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] &= (~(BRAP_FWIF_P_EventIdMask_eStreamInfoAvail));
            break;
        }
        case BRAP_Interrupt_eCdbItbOverflow:
        {
            BRAP_FWIF_P_Command     sFwCommand;
            BRAP_FWIF_P_FwTaskHandle    hTask = NULL;
            
            BRAP_P_GetTask(hRapCh, &hTask);            
            if(hTask == NULL)
            {   
                BDBG_ERR(("BRAP_P_MaskInterrupt_isr: BRAP_FWIF_P_EventIdMask_eCdbItbOverflow"
                            "Channel = %0x is not having decode path", hRapCh));
                BDBG_ASSERT(hTask);
                return BERR_TRACE(BRAP_ERR_INVALID_TASK);
            }
            /*  Prepare message structure for FW to write in message queue */
            sFwCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_EVENT_NOTIFICATION_COMMAND_ID;
            sFwCommand.sCommandHeader.ui32CommandSizeInBytes = sizeof(BRAP_FWIF_P_Command);
            sFwCommand.sCommandHeader.eResponseType = eRsp;
            sFwCommand.sCommandHeader.ui32TaskID = hTask->uiTaskId;
            sFwCommand.sCommandHeader.ui32CommandCounter = hRap->uiCommandCounter++;
            sFwCommand.uCommand.sEnableDisableEvent.ui32EnableEvent = 
                hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] & (~(BRAP_FWIF_P_EventIdMask_eCdbItbOverflow));

            /* Write in Message queue */
            ret = BRAP_FWIF_P_SendCommand_isr(hTask->hDsp->hCmdQueue, &sFwCommand,hRap,hTask);
            if(ret != BERR_SUCCESS)
            {
                if((hRap->bWatchdogTriggered == false)
                &&(hTask->bStopped == false))
                {
                    BDBG_ERR(("BRAP_P_maskInterrupt_isr: Send command failed"));
                    return BERR_TRACE(ret);  
                }
                else
                    ret = BERR_SUCCESS;            
            }
            hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] &= (~(BRAP_FWIF_P_EventIdMask_eCdbItbOverflow));
            break;
        }
        case BRAP_Interrupt_eCdbItbUnderflow:
        {
            BRAP_FWIF_P_Command     sFwCommand;
            BRAP_FWIF_P_FwTaskHandle    hTask = NULL;
            
            BRAP_P_GetTask(hRapCh, &hTask);            
            if(hTask == NULL)
            {   
                BDBG_ERR(("BRAP_P_MaskInterrupt_isr: BRAP_FWIF_P_EventIdMask_eCdbItbUnderflow"
                            "Channel = %0x is not having decode path", hRapCh));
                BDBG_ASSERT(hTask);
                return BERR_TRACE(BRAP_ERR_INVALID_TASK);
            }
            /*  Prepare message structure for FW to write in message queue */
            sFwCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_EVENT_NOTIFICATION_COMMAND_ID;
            sFwCommand.sCommandHeader.ui32CommandSizeInBytes = sizeof(BRAP_FWIF_P_Command);
            sFwCommand.sCommandHeader.eResponseType = eRsp;
            sFwCommand.sCommandHeader.ui32TaskID = hTask->uiTaskId;
            sFwCommand.sCommandHeader.ui32CommandCounter = hRap->uiCommandCounter++;
            sFwCommand.uCommand.sEnableDisableEvent.ui32EnableEvent = 
                hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] & (~(BRAP_FWIF_P_EventIdMask_eCdbItbUnderflow));

            /* Write in Message queue */
            ret = BRAP_FWIF_P_SendCommand_isr(hTask->hDsp->hCmdQueue, &sFwCommand,hRap,hTask);
            if(ret != BERR_SUCCESS)
            {
                if((hRap->bWatchdogTriggered == false)
                &&(hTask->bStopped == false))
                {
                    BDBG_ERR(("BRAP_P_maskInterrupt_isr: Send command failed"));
                    return BERR_TRACE(ret);  
                }
                else
                    ret = BERR_SUCCESS;            
            }
            hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] &= (~(BRAP_FWIF_P_EventIdMask_eCdbItbUnderflow));
            break;
        }        
        case BRAP_Interrupt_eUnlicensedAlgo:
        {
            BRAP_FWIF_P_Command     sFwCommand;
            BRAP_FWIF_P_FwTaskHandle    hTask = NULL;
            
            BRAP_P_GetTask(hRapCh, &hTask);            
            if(hTask == NULL)
            {   
                BDBG_ERR(("BRAP_P_MaskInterrupt_isr: BRAP_FWIF_P_EventIdMask_eUnlicensedAlgo"
                            "Channel = %0x is not having decode path", hRapCh));
                BDBG_ASSERT(hTask);
                return BERR_TRACE(BRAP_ERR_INVALID_TASK);
            }
            /*  Prepare message structure for FW to write in message queue */
            sFwCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_EVENT_NOTIFICATION_COMMAND_ID;
            sFwCommand.sCommandHeader.ui32CommandSizeInBytes = sizeof(BRAP_FWIF_P_Command);
            sFwCommand.sCommandHeader.eResponseType = eRsp;
            sFwCommand.sCommandHeader.ui32TaskID = hTask->uiTaskId;
            sFwCommand.sCommandHeader.ui32CommandCounter = hRap->uiCommandCounter++;
            sFwCommand.uCommand.sEnableDisableEvent.ui32EnableEvent = 
                hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] & (~(BRAP_FWIF_P_EventIdMask_eUnlicensedAlgo));

            /* Write in Message queue */
            ret = BRAP_FWIF_P_SendCommand_isr(hTask->hDsp->hCmdQueue, &sFwCommand,hRap,hTask);
            if(ret != BERR_SUCCESS)
            {
                if((hRap->bWatchdogTriggered == false)
                &&(hTask->bStopped == false))
                {
                    BDBG_ERR(("BRAP_P_maskInterrupt_isr: Send command failed"));
                    return BERR_TRACE(ret);  
                }
                else
                    ret = BERR_SUCCESS;            
            }
            hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] &= (~(BRAP_FWIF_P_EventIdMask_eUnlicensedAlgo));
            break;
        }        
        default:
            /*Do nothing */
            break;
    }/* switch */

	BDBG_MSG(("BCHP_AUD_FMM_BF_ESR2_H_MASK_SET = %x", ui32RegValFMM));
	BRAP_Write32_isr(hRapCh->hRegister, BCHP_AUD_FMM_BF_ESR2_H_MASK_SET, ui32RegValFMM);

#ifdef CRC_ENABLE
	BRAP_Write32_isr(hRapCh->hRegister, BCHP_AUD_FMM_OP_ESR_MASK_SET, ui32RegValFMMCrc);
#endif

    BDBG_LEAVE (BRAP_P_MaskInterrupt_isr);
    return ret;
}

/*****************************************************************************
Summary:
    Used to mask a particular interrupt.

Description:
    This PI should be used by the application to mask an interrupt. This can 
    be done only for the interrupts that are handled by Raptor PI and listed in 
    BRAP_DestinationInterrupt. For all other interrupts, application has to use the 
    standard INT interface.

Returns:
    BERR_SUCCESS on success
    Error code on failure
******************************************************************************/
BERR_Code BRAP_P_MaskDestinationInterrupt (
	BRAP_DestinationHandle        hDstHandle,	   /* [in] The RAP destination handle */
    BRAP_DestinationInterrupt     eInterrupt  /* [in] The interrupt that needs to be deactivated */
)
{
    BERR_Code ret = BERR_SUCCESS;
    
    BDBG_ENTER(BRAP_P_MaskDestinationInterrupt);
        
    BKNI_EnterCriticalSection();
    ret = BRAP_P_MaskDestinationInterrupt_isr(hDstHandle, eInterrupt);
    BKNI_LeaveCriticalSection();
    if(ret != BERR_SUCCESS)
    {
		return BERR_TRACE(ret);        
    }

    BDBG_LEAVE(BRAP_P_MaskDestinationInterrupt);
	return ret;
}


/*****************************************************************************
Summary:
    Used to mask a particular interrupt - isr version.

Description:
    ISR version routine for BRAP_P_MaskDestinationInterrupt().
    
Returns:
    BERR_SUCCESS on success
    Error code on failure
******************************************************************************/
BERR_Code BRAP_P_MaskDestinationInterrupt_isr (
	BRAP_DestinationHandle        hDstHandle,	   /* [in] The RAP destination handle */
    BRAP_DestinationInterrupt     eInterrupt  /* [in] The interrupt that needs to be deactivated */
)
{
	BERR_Code       ret = BERR_SUCCESS;

#if (BRAP_7550_FAMILY !=1)       
	uint32_t        ui32RegValFMM = 0;
	unsigned int    uichannelpair = 0;
#endif    

	BDBG_ENTER (BRAP_P_MaskDestinationInterrupt_isr);

	/* Check input parameters */
	BDBG_ASSERT (hDstHandle); 

    switch (eInterrupt)
    {
#if (BRAP_7550_FAMILY !=1)    
        case BRAP_DestinationInterrupt_eFmmRbufFullMark:
            ui32RegValFMM = BRAP_Read32_isr(hDstHandle->hAssociation->hRap->hRegister, BCHP_AUD_FMM_BF_ESR2_H_MASK_SET);
            for (uichannelpair = 0; uichannelpair < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; uichannelpair++)
        	{
        		if (hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiDstChId[uichannelpair] != (unsigned int)BRAP_RM_P_INVALID_INDEX)
        		{
        		    switch(hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiDstChId[uichannelpair])
        		    {
        		       	case 0:
        			        ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_SET, DEST_RINGBUF_0_EXCEED_FULLMARK, 1);
        		            break;

        				case 1:
        					ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_SET, DEST_RINGBUF_1_EXCEED_FULLMARK, 1);
        		            break;

        				case 2:
        					ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_SET, DEST_RINGBUF_2_EXCEED_FULLMARK, 1);
        		            break;							

        				case 3:
        					ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR2_H_MASK_SET, DEST_RINGBUF_3_EXCEED_FULLMARK, 1);
        		            break;							

        				default:
        					break;
        			}
        		}
            }  
        	BDBG_MSG(("BCHP_AUD_FMM_BF_ESR2_H_MASK_SET = %x", ui32RegValFMM));
        	BRAP_Write32_isr(hDstHandle->hAssociation->hRap->hRegister, BCHP_AUD_FMM_BF_ESR2_H_MASK_SET, ui32RegValFMM);
            break;
            
        case BRAP_DestinationInterrupt_eFmmRbufOverflow:
            ui32RegValFMM = BRAP_Read32_isr(hDstHandle->hAssociation->hRap->hRegister, BCHP_AUD_FMM_BF_ESR1_H_MASK_SET);
            for (uichannelpair = 0; uichannelpair < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; uichannelpair++)
        	{
        		if (hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiDstChId[uichannelpair] != (unsigned int)BRAP_RM_P_INVALID_INDEX)
        		{
        		    switch(hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiDstChId[uichannelpair])
        		    {
        		       	case 0:
        			        ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR1_H_MASK_SET, DEST_RINGBUF_0_OVERFLOW, 1);
        		            break;

        				case 1:
        					ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR1_H_MASK_SET, DEST_RINGBUF_1_OVERFLOW, 1);
        		            break;

        				case 2:
        					ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR1_H_MASK_SET, DEST_RINGBUF_2_OVERFLOW, 1);
        		            break;							

        				case 3:
        					ui32RegValFMM |= BCHP_FIELD_DATA (AUD_FMM_BF_ESR1_H_MASK_SET, DEST_RINGBUF_3_OVERFLOW, 1);
        		            break;							

        				default:
        					break;
        			}
        		}
            }  
        	BDBG_MSG(("BCHP_AUD_FMM_BF_ESR1_H_MASK_SET = %x", ui32RegValFMM));
        	BRAP_Write32_isr(hDstHandle->hAssociation->hRap->hRegister, BCHP_AUD_FMM_BF_ESR1_H_MASK_SET, ui32RegValFMM);            
            break;
#endif            
        case BRAP_DestinationInterrupt_eSampleRateChange:
            hDstHandle->bSampleRateChangeCallbackEnabled = false;
            break;
        default:
            break;
    }


    BDBG_LEAVE (BRAP_P_MaskDestinationInterrupt_isr);
    return ret;
}

static void BRAP_P_DecoderLock_isr(void *pParam1,void *pParam2)
{
    BRAP_ChannelHandle hRapCh = (BRAP_ChannelHandle)pParam1;

    BDBG_ASSERT(hRapCh);
    BSTD_UNUSED(pParam2);
    BDBG_MSG(("BRAP_P_DSP2HostAsyn_isr: Decoder lock interrupt occured for RAP channel %d",
    hRapCh->uiChannelNo));

    hRapCh->bDecLocked = true;

    /* Call the application Decode lock callback function */
    if(hRapCh->sAppIntCbInfo[BRAP_Interrupt_eDecLock].pfAppCb) 
    {
        hRapCh->sAppIntCbInfo[BRAP_Interrupt_eDecLock].pfAppCb (
                                            hRapCh->sAppIntCbInfo[BRAP_Interrupt_eDecLock].pParm1,
                                            hRapCh->sAppIntCbInfo[BRAP_Interrupt_eDecLock].iParm2,
                                            NULL
                                            );
    }
    return;
}  

static void BRAP_P_DecoderUnlock_isr(void *pParam1,void *pParam2)
{
    BRAP_ChannelHandle hRapCh = (BRAP_ChannelHandle)pParam1;

    BDBG_ASSERT(hRapCh);
    BSTD_UNUSED(pParam2);
    BDBG_MSG(("BRAP_P_DSP2HostAsyn_isr: Decoder unlock interrupt occured for RAP channel %d",hRapCh->uiChannelNo));

    hRapCh->bDecLocked = false;

/*Increment Error Count*/
    hRapCh->uiTotalErrorCount++;

    /* Call the application Decoder unlock callback function */
    if(hRapCh->sAppIntCbInfo[BRAP_Interrupt_eDecUnlock].pfAppCb) 
    {
        hRapCh->sAppIntCbInfo[BRAP_Interrupt_eDecUnlock].pfAppCb (
                        hRapCh->sAppIntCbInfo[BRAP_Interrupt_eDecUnlock].pParm1,
                        hRapCh->sAppIntCbInfo[BRAP_Interrupt_eDecUnlock].iParm2,
                        NULL);
    }
    return;
}    

#if 0

/* BRAP_P_MuteChannelOutputOnSr_isr: Mutes the output ports associated with
 * a decode channel at output port level on sample rate change. Returns the
 * previous mute status of output ports in psOpPortPrevMuteStatus.
 */
BERR_Code 
BRAP_P_MuteAndSetSRChannelOutputOnSr_isr
(
	BRAP_ChannelHandle hRapCh,		/* [in] The RAP decode channel handle */ 
	BRAP_P_OpPortPrevMuteStatus *psOpPortPrevMuteStatus, /* [out] Returns previous mute states of output ports */
       BAVC_AudioSamplingRate	eSamplingRate  /* Sampling freq value */	
)
{

    BRAP_OutputPort eOpType = BRAP_OutputPort_eMax;
    unsigned int i=0,j=0,k=0,uiAssocId;
    BRAP_Handle hRap;
    BERR_Code   ret=BERR_SUCCESS;
    bool bPbCapChPresent=false;

    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(psOpPortPrevMuteStatus);
    BSTD_UNUSED(eSamplingRate);

    BKNI_Memset(psOpPortPrevMuteStatus, 0x0, sizeof(BRAP_P_OpPortPrevMuteStatus));


    hRap =hRapCh->hRap;
/*Mute the outputports associated with this hRapCh*/
    for(i =0 ; i< BRAP_MAX_ASSOCIATED_GROUPS ; i++)
    {
        if(hRapCh->uiAssociationId[i] != BRAP_INVALID_VALUE)
        {
            uiAssocId = hRapCh->uiAssociationId[i];
            bPbCapChPresent =false;
            for(j=0; j <BRAP_MAX_PB_CHAN_IN_ASSOCIATED_GRP;j++ )
            {
                if(hRap->sAssociatedCh[uiAssocId].hPBCh[j] != NULL)
                {
                    bPbCapChPresent = true;
                    break;
                }
            }
            if(bPbCapChPresent == false)
            {
                for(j=0; j <BRAP_MAX_CAP_CHAN_IN_ASSOCIATED_GRP;j++ )
                {
                    if(hRap->sAssociatedCh[uiAssocId].hCapCh[j] != NULL)
                    {
                        bPbCapChPresent = true;
                        break;
                    }
                }
            }
            if(bPbCapChPresent == true)
            {
                for(j =0 ; j < BRAP_P_MAX_DST_PER_RAPCH; j++ )
                {
                    if(hRap->sAssociatedCh[uiAssocId].sDstDetails[j].sExtDstDetails.eAudioDst == BRAP_AudioDst_eOutputPort)
                    {
                        for(k =0 ; k < BRAP_OutputChannelPair_eMax; k++ )
                        {
                            eOpType = hRap->sAssociatedCh[uiAssocId].sDstDetails[j].sExtDstDetails.uDstDetails.sOpDetails.eOutput[k];
                            if(eOpType != BRAP_OutputPort_eMax)
                            {
                                ret = BRAP_OP_P_GetMute(hRap,eOpType,&psOpPortPrevMuteStatus->bDecMuteStatus[i][j][k]);
                                if (ret != BERR_SUCCESS)
                                {
                                    ret = BERR_TRACE(ret);                            
                                    goto end;
                                }
                                if(psOpPortPrevMuteStatus->bDecMuteStatus[i][j][k] == false)
                                {
                                    BRAP_OP_P_SetMute_isr(hRap,eOpType,true);
                                    if (ret != BERR_SUCCESS)
                                    {
                                        ret = BERR_TRACE(ret);                            
                                        goto end;                                    
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        /*Mute Rbuf*/
                    }
                }
            }
        }
    }
end:
    return ret;
}

/* BRAP_P_UnMuteChannelOutputOnSr_isr: Unmutes the output ports associated with
 * a decode channel at output port level on sample rate change. Unmutes only those
 * output ports for which previous mute status was unmute. Previous mute states of
 * output ports are passed in parameter psOpPortPrevMuteStatus.
 */
BERR_Code 
BRAP_P_UnMuteChannelOutputOnSr_isr
(
	BRAP_ChannelHandle hRapCh, 		/* [in] The RAP decode channel handle */ 
	BRAP_P_OpPortPrevMuteStatus *psOpPortPrevMuteStatus /* [in] Previous mute states of output ports */
)
{
    BRAP_OutputPort eOpType = BRAP_OutputPort_eMax;
    unsigned int i=0,j=0,k=0,uiAssocId;
    BRAP_Handle hRap;
    BERR_Code   ret=BERR_SUCCESS;

    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(psOpPortPrevMuteStatus);



    hRap =hRapCh->hRap;
/*Mute the outputports associated with this hRapCh*/
    for(i =0; i< BRAP_MAX_ASSOCIATED_GROUPS ; i++)
    {
        if(hRapCh->uiAssociationId[i] != BRAP_INVALID_VALUE)
        {
            uiAssocId = hRapCh->uiAssociationId[i];
            for(j =0 ; j < BRAP_P_MAX_DST_PER_RAPCH; j++ )
            {
                if(hRap->sAssociatedCh[uiAssocId].sDstDetails[j].sExtDstDetails.eAudioDst == BRAP_AudioDst_eOutputPort)
                {
                    for(k =0 ; k < BRAP_OutputChannelPair_eMax; k++ )
                    {
                        eOpType = hRap->sAssociatedCh[uiAssocId].sDstDetails[j].sExtDstDetails.uDstDetails.sOpDetails.eOutput[k];
                        if(eOpType != BRAP_OutputPort_eMax)
                        {
                                BRAP_OP_P_SetMute_isr(hRapCh->hRap,eOpType,psOpPortPrevMuteStatus->bDecMuteStatus[i][j][k]);
                                if (ret != BERR_SUCCESS)
                                {
                                    ret = BERR_TRACE(ret);                            
                                    goto end;                                    
                                }
                        }
                    }
                }
                else
                {
                    /*Mute Rbuf*/
                }
            }
        }
    }
end:
    return ret;
}
#endif
#if (BRAP_7550_FAMILY != 1)

BERR_Code
BRAP_P_ConfigureAssociatedPathsSrc_isr(
    BRAP_ChannelHandle  hRapCh,
    unsigned int        uiAssocId,
    unsigned int        ui32OutSamplingRate
)
{
    BERR_Code   ret=BERR_SUCCESS;
    unsigned int    uiNum = 0,uiDen = 0, ui32RegVal =0,uiDenScale = 0;
    unsigned int    uiDivider = 1, uiPathId = BRAP_INVALID_VALUE;    
    unsigned int    l =0, m =0 , n=0, p =0;
    BRAP_SRC_P_Handle   hSrc;
#if (BRAP_3548_FAMILY == 1)    
    BAVC_AudioSamplingRate  eInputSamplingRate;     
#endif
    BDBG_ASSERT(hRapCh);    

    /* Store the output sample rate of channel */
    BRAP_P_ConvertSrToEnum(ui32OutSamplingRate,&(hRapCh->eSamplingRate));                
    BDBG_MSG(("The Output Sampling rate of channel = %d", ui32OutSamplingRate));
    BDBG_MSG(("The Input Sampling rate of channel = %d", hRapCh->uiInputSamplingRate));                    

    if(hRapCh->eState == BRAP_P_State_eStarted)
    {
        /* Calculate Den/Num/DenScale of the SRC*/
        uiDen = ui32OutSamplingRate;
        uiNum = hRapCh->uiInputSamplingRate;
        if((uiDen  % 100 == 0)&&(uiNum % 100 == 0))
        {
            uiDivider =100;
        }
        else if((uiDen  % 10 == 0)&&(uiNum % 10 == 0))
        {
            uiDivider =10;
        }
        else
        {
            uiDivider =1;
        }                         
        uiDen = uiDen/uiDivider;
        uiNum= uiNum/uiDivider;
#if (BRAP_3548_FAMILY != 1)        
        uiDenScale = (0x400000)/uiDen;        /*2^22/Den*/
#else
        uiDenScale = (0x1000000)/uiDen;        /*2^24/Den*/
#endif
        BDBG_MSG(("uiNum = %d, uiDen =%d, uiScale =%d",uiNum,uiDen,uiDenScale));
    }
    
    /*For PCM Capture channel zeroth path is a capture path and SRC need not be reconfigured*/
    uiPathId = (hRapCh->eChannelType == BRAP_ChannelType_ePcmCapture) ? 1:0;

    for( ; uiPathId < BRAP_P_MAX_PATHS_IN_A_CHAN ; uiPathId++)
    {   
        if((!(BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPathId]))) ||
           (BRAP_P_UsgPath_eDecodePcmPostMixing == hRapCh->pPath[uiPathId]->eUsgPath))
            break;    
        
        if(uiAssocId != hRapCh->pPath[uiPathId]->uiAssocId)
            continue;
        
        for(l =0; l <BRAP_RM_P_MAX_MIXING_LEVELS;l++ )
        {
            for(m =0; m <BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;m++ )                        
            {
                for(n =0; n <BRAP_RM_P_MAX_PARALLEL_PATHS;n++ )                                  
                {
                    for(p =0; p <BRAP_RM_P_MAX_SRC_IN_CASCADE;p++ )                                             
                    {
                        hSrc = hRapCh->pPath[uiPathId]->sSrc[l][m][n].hSrc[p];                      
                        if(BRAP_P_IsPointerValid((void *)hSrc))
                        {                                                  
                            if(hRapCh->eState == BRAP_P_State_eStarted)
                            {
                                BDBG_MSG(("Programming Src Index %d",hSrc->uiIndex));
#if (BRAP_3548_FAMILY == 1)
                                BRAP_P_ConvertSrToEnum(hRapCh->uiInputSamplingRate,&eInputSamplingRate);
                                if (ui32OutSamplingRate == hRapCh->uiInputSamplingRate)
                                {
                                    /* If earlier SRC type is LinInt return the Coeff Addr as it now becomes Bypass */    
                                    if((hSrc->sParams.eSrcType == BRAP_SRC_P_Type_eLinInt) &&
                                       (hSrc->uiCoeffAddr != BRAP_INVALID_VALUE))
                                    {                                                   
                                        ret = BRAP_SRC_P_DeallocateCoeffAddr(hSrc);
                                        if(ret != BERR_SUCCESS)
                                        {
                                            BDBG_ERR(("BRAP_P_ConfigureAssociatedPathsSrc_isr: BRAP_SRC_P_DeallocateCoeffAddr returned error"));
                                            return ret;
                                        }
                                    }
                                    
                                    /* Program SRC type */                                                
                                    ui32RegVal = BRAP_Read32 (hSrc->hRegister,
                                            hSrc->uiBlkOffset + BCHP_AUD_FMM_SRC_CTRL0_SRC_CFGi_ARRAY_BASE +
                                            hSrc->uiSrcOffset);

                                    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_SRC_CTRL0_SRC_CFGi,SRC_TYPE));
                                    ui32RegVal |= (BCHP_FIELD_DATA(AUD_FMM_SRC_CTRL0_SRC_CFGi,SRC_TYPE,
                                                    BRAP_SRC_P_Type_eBypass));                                                     

                                    BRAP_Write32(hSrc->hRegister,
                                        hSrc->uiBlkOffset + BCHP_AUD_FMM_SRC_CTRL0_SRC_CFGi_ARRAY_BASE +
                                        hSrc->uiSrcOffset, ui32RegVal);

                                    hSrc->sParams.eSrcType = BRAP_SRC_P_Type_eBypass;
                                    hSrc->sParams.eInputSR = eInputSamplingRate;
                                    hSrc->sParams.eOutputSR = hRapCh->eSamplingRate;
                                    BDBG_MSG (("The SRC Type is Bypass"));                                                
                                }
                                else
                                {
                                    hSrc->sParams.eSrcType = BRAP_SRC_P_Type_eLinInt;
                                    hSrc->sParams.eInputSR = eInputSamplingRate;
                                    hSrc->sParams.eOutputSR = hRapCh->eSamplingRate;
                                    
                                    /* Program SRC Coefficients */
                                    if(hSrc->uiCoeffAddr == BRAP_INVALID_VALUE)
                                    {
                                        ret = BRAP_SRC_P_AllocateCoeffAddr(hSrc);

                                        if(BERR_SUCCESS != ret)
                                        {
                                            BDBG_ERR(("BRAP_P_ConfigureAssociatedPathsSrc_isr: BRAP_SRC_P_AllocateCoeffAddr returned error"));
                                            return ret;
                                        }
                                    }

                                    BRAP_Write32(hSrc->hRegister,
                                                    hSrc->uiBlkOffset +
                                                    BCHP_AUD_FMM_SRC_CTRL0_COEFFi_ARRAY_BASE + (hSrc->uiCoeffAddr*4),
                                                    uiNum);
                                    
                                    BRAP_Write32(hSrc->hRegister,
                                                    hSrc->uiBlkOffset +
                                                    BCHP_AUD_FMM_SRC_CTRL0_COEFFi_ARRAY_BASE + ((hSrc->uiCoeffAddr+1)*4),
                                                    uiDen);
                                    
                                    BRAP_Write32(hSrc->hRegister,
                                                    hSrc->uiBlkOffset +
                                                    BCHP_AUD_FMM_SRC_CTRL0_COEFFi_ARRAY_BASE + ((hSrc->uiCoeffAddr+2)*4),
                                                    uiDenScale);                                                

                                    /* Program adress where coefficents are written */
                                    ui32RegVal = BRAP_Read32 (hSrc->hRegister,
                                        hSrc->uiBlkOffset +(BCHP_AUD_FMM_SRC_CTRL0_CF_SELi_ARRAY_BASE +
                                        (4 * hSrc->uiIndex)));

                                    /* First 24 bits of Coeff moemory is hidden and is used by HW */    
                                    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_SRC_CTRL0_CF_SELi,CF_BASE_ADDR0));
                                    ui32RegVal |= (BCHP_FIELD_DATA(AUD_FMM_SRC_CTRL0_CF_SELi,CF_BASE_ADDR0,(hSrc->uiCoeffAddr+24)));
                                
                                    BRAP_Write32(hSrc->hRegister,
                                        hSrc->uiBlkOffset +(BCHP_AUD_FMM_SRC_CTRL0_CF_SELi_ARRAY_BASE +
                                        (4 * hSrc->uiIndex)), ui32RegVal);

                                    /* Program SRC type */
                                    ui32RegVal = BRAP_Read32 (hSrc->hRegister,
                                        hSrc->uiBlkOffset + BCHP_AUD_FMM_SRC_CTRL0_SRC_CFGi_ARRAY_BASE +
                                        hSrc->uiSrcOffset);

                                    ui32RegVal &= ~(BCHP_MASK(AUD_FMM_SRC_CTRL0_SRC_CFGi,SRC_TYPE));
                                    ui32RegVal |= (BCHP_FIELD_DATA(AUD_FMM_SRC_CTRL0_SRC_CFGi,SRC_TYPE,
                                                    BRAP_SRC_P_Type_eLinInt));

                                    BRAP_Write32(hSrc->hRegister,
                                        hSrc->uiBlkOffset + BCHP_AUD_FMM_SRC_CTRL0_SRC_CFGi_ARRAY_BASE +
                                        hSrc->uiSrcOffset, ui32RegVal);

                                    BDBG_MSG (("The SRC Type is Linear Interpolation"));                                                
                                }
#else
                                ui32RegVal = BRAP_Read32(hSrc->hRegister,
                                                            hSrc->uiBlkOffset+BCHP_AUD_FMM_SRC_CTRL0_SRC_CFG0+hSrc->uiSrcOffset);

/* Dynamic change of SRC type from LINT to BYPASS is causing artifacts, so program coefficients for BYPASS */
#if 0
                                if(uiNum != uiDen)
#endif                                                    
                                {
                                    BRAP_Write32(hSrc->hRegister,
                                                    hSrc->uiBlkOffset+BCHP_AUD_FMM_SRC_CTRL0_LI_NUM0+hSrc->uiSrcOffset,
                                                    uiNum);       
                                    BRAP_Write32(hSrc->hRegister,
                                                    hSrc->uiBlkOffset+BCHP_AUD_FMM_SRC_CTRL0_LI_DEN0+hSrc->uiSrcOffset,
                                                    uiDen);  
                                    BRAP_Write32(hSrc->hRegister,
                                                    hSrc->uiBlkOffset+BCHP_AUD_FMM_SRC_CTRL0_LI_DEN_SCALE0+hSrc->uiSrcOffset,
                                                    uiDenScale);   
                                    
                                    ui32RegVal &= ~((BCHP_MASK(AUD_FMM_SRC_CTRL0_SRC_CFG0,TYPE)));
                                    ui32RegVal |= (BCHP_FIELD_DATA(AUD_FMM_SRC_CTRL0_SRC_CFG0,TYPE,
                                                    BRAP_SRC_P_Type_eLinInt));                                           
                                    BRAP_Write32(hSrc->hRegister,
                                                    hSrc->uiBlkOffset+BCHP_AUD_FMM_SRC_CTRL0_SRC_CFG0+hSrc->uiSrcOffset,
                                                    ui32RegVal);
                                    
                                    hSrc->sParams.eSrcType = BRAP_SRC_P_Type_eLinInt;                                                    
                                }
#if 0
                                else
                                {   
                                    ui32RegVal &= ~((BCHP_MASK(AUD_FMM_SRC_CTRL0_SRC_CFG0,TYPE)));

                                    ui32RegVal |= (BCHP_FIELD_DATA(AUD_FMM_SRC_CTRL0_SRC_CFG0,TYPE,0));                                      
                                    
                                    BRAP_Write32(hSrc->hRegister,
                                                    hSrc->uiBlkOffset+BCHP_AUD_FMM_SRC_CTRL0_SRC_CFG0+hSrc->uiSrcOffset,
                                                    ui32RegVal);   
                                    
                                    BRAP_Write32(hSrc->hRegister,
                                                    hSrc->uiBlkOffset+BCHP_AUD_FMM_SRC_CTRL0_LI_NUM0+hSrc->uiSrcOffset,
                                                    0);       
                                    
                                    BRAP_Write32(hSrc->hRegister,
                                                    hSrc->uiBlkOffset+BCHP_AUD_FMM_SRC_CTRL0_LI_DEN0+hSrc->uiSrcOffset,
                                                    0);  
                                    
                                    BRAP_Write32(hSrc->hRegister,
                                                    hSrc->uiBlkOffset+BCHP_AUD_FMM_SRC_CTRL0_LI_DEN_SCALE0+hSrc->uiSrcOffset,
                                                    0); 

                                    hSrc->sParams.eSrcType = BRAP_SRC_P_Type_eBypass;                                                    
                                }
#endif
#endif
                            }
                            else
                            {
                                    hSrc->sParams.eOutputSR = hRapCh->eSamplingRate;
                            }
                        }
                    }
                }
            }
        }
    }
    return ret;
}

BERR_Code
BRAP_P_ConfigureAssociatedChannelsSrc_isr(
                BRAP_ChannelHandle hRapCh,
                unsigned int        ui32OutSamplingRate)
{
    BRAP_Handle hRap;
    BERR_Code   ret=BERR_SUCCESS;
    unsigned int i=0,j=0,uiAssocId;
    BDBG_ASSERT(hRapCh);

    hRap =hRapCh->hRap;
    
    BDBG_MSG(("BRAP_P_ConfigureAssociatedChannelsSrc_isr: hRapCh %p Channel Type %d", 
                hRapCh,hRapCh->eChannelType));
    
    for(i =0; i< BRAP_MAX_ASSOCIATED_GROUPS ; i++)
    {
        if(BRAP_INVALID_VALUE == hRapCh->uiAssociationId[i])
            continue;
        
        uiAssocId = hRapCh->uiAssociationId[i];
        if(BRAP_ChannelType_eDecode == hRapCh->eChannelType)
        {
            if((true == hRapCh->bEnableFixedSampleRateOutput) ||
               (BRAP_P_SECONDARY_CHANNEL_SRC(hRapCh, uiAssocId)))
            {
                ret = BRAP_P_ConfigureAssociatedPathsSrc_isr(hRapCh, uiAssocId, ui32OutSamplingRate);
                if(BERR_SUCCESS != ret)
                {
                    BDBG_ERR(("BRAP_P_ConfigureAssociatedChannelsSrc_isr: BRAP_P_ConfigureAssociatedPathsSrc_isr returned error"));
                    return ret;
                }
            }
            else if(BRAP_P_PRIMARY_CHANNEL(hRapCh, uiAssocId))
            {
                BRAP_ChannelHandle  hSecRapCh;      

                for(j=0; j<BRAP_MAX_SEC_CHANNEL_FOR_MS_DECODER; j++)
                {
                    hSecRapCh = hRapCh->hRap->sAssociatedCh[uiAssocId].hMultiStreamDecoder->\
                                sExtMultiStreamDecoderDetails.hSecondaryChannel[j];
                    if((NULL != hSecRapCh) &&
                       (BRAP_P_SECONDARY_CHANNEL_SRC(hSecRapCh, uiAssocId)))
                    {
                        ret = BRAP_P_ConfigureAssociatedPathsSrc_isr(hSecRapCh, uiAssocId, ui32OutSamplingRate);
                        if(BERR_SUCCESS != ret)
                        {
                            BDBG_ERR(("BRAP_P_ConfigureAssociatedChannelsSrc_isr: BRAP_P_ConfigureAssociatedPathsSrc_isr returned error"));
                            return ret;
                        }
                    }                        
                }
            }
        }
        for(j =0 ; j < BRAP_MAX_PB_CHAN_IN_ASSOCIATED_GRP; j++)
        {          
            if(BRAP_P_IsPointerValid((void *)hRap->sAssociatedCh[uiAssocId].hPBCh[j]))
            {
                ret = BRAP_P_ConfigureAssociatedPathsSrc_isr(hRap->sAssociatedCh[uiAssocId].hPBCh[j], uiAssocId, ui32OutSamplingRate);
                if(BERR_SUCCESS != ret)
                {
                    BDBG_ERR(("BRAP_P_ConfigureAssociatedChannelsSrc_isr: BRAP_P_ConfigureAssociatedPathsSrc_isr returned error"));
                    return ret;
                }                            
            }
        }
        for(j =0 ; j < BRAP_MAX_CAP_CHAN_IN_ASSOCIATED_GRP; j++)
        {          
            if(BRAP_P_IsPointerValid((void *)hRap->sAssociatedCh[uiAssocId].hCapCh[j]))
            {
                ret = BRAP_P_ConfigureAssociatedPathsSrc_isr(hRap->sAssociatedCh[uiAssocId].hCapCh[j], uiAssocId, ui32OutSamplingRate);
                if(BERR_SUCCESS != ret)
                {
                    BDBG_ERR(("BRAP_P_ConfigureAssociatedChannelsSrc_isr: BRAP_P_ConfigureAssociatedPathsSrc_isr returned error"));
                    return ret;
                }
            }
        }
    }
    
    return ret;
}

#if (BRAP_3548_FAMILY == 1)
#if (BRAP_48KHZ_RINGBUFFER_DESTINATION == 1)
BERR_Code
BRAP_P_ProgramSRCOfRingBufferDstn_isr(
                BRAP_ChannelHandle hRapCh,
                unsigned int        ui32SamplingRate)
{
    BRAP_Handle     hRap;
    BERR_Code       ret=BERR_SUCCESS;
    unsigned int    i=0,j=0,k=0,uiAssocId = BRAP_INVALID_VALUE;    
    unsigned int    l =0, m =0 , n=0, p =0, uiDstIndex = 0;
    unsigned int    uiNum = 0,uiDen = 0, ui32RegVal =0,uiDenScale = 0,uiCaptureDataSR = 0;
    BAVC_AudioSamplingRate  eCaptureDataSR = BAVC_AudioSamplingRate_eUnknown;    
    unsigned int    uiDivider = 1;    
    bool            bSRCProgrmd = false;
    BRAP_SRC_P_Handle   hSrc;
    BAVC_AudioSamplingRate  eInputSamplingRate;     
    

    BDBG_ENTER(BRAP_P_ProgramSRCOfRingBufferDstn_isr);
    BDBG_ASSERT(hRapCh);
    hRap =hRapCh->hRap;

    if(hRapCh->bIndepDelayEnabled == false)
    {
        BDBG_ERR(("Fixed 48khz capture is not supported if Independent Delay is disabled." 
            "Enable Independent delay flag if this feature is required"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);    
    }

    BDBG_MSG(("Sampling rate of Decode content = %d", ui32SamplingRate));
    uiCaptureDataSR = 48000;  /* Samplerate at which Ringbuffer Capture is required */
    BRAP_P_ConvertSrToEnum(uiCaptureDataSR, &(eCaptureDataSR));

    for(i =0; i < BRAP_MAX_ASSOCIATED_GROUPS; i++)
    {
        if(hRapCh->uiAssociationId[i] != BRAP_INVALID_VALUE)
        {
            uiAssocId = hRapCh->uiAssociationId[i];

            for(j = 0 ; j < BRAP_MAX_PRI_DEC_CHAN_IN_ASSOCIATED_GRP; j++)
            {          
                if(BRAP_P_IsPointerValid((void *)hRap->sAssociatedCh[uiAssocId].hPriDecCh[j]))
                {
                    for(k = 0 ; k < BRAP_P_MAX_PATHS_IN_A_CHAN ; k++)
                    {
                        if(!(BRAP_P_IsPointerValid((void *)hRap->sAssociatedCh[uiAssocId].hPriDecCh[j]->pPath[k])))
                            break;
                        for(uiDstIndex = 0; uiDstIndex < BRAP_P_MAX_DST_PER_RAPCH; uiDstIndex++)
                        {
                            if((BRAP_P_IsPointerValid((void *)hRap->sAssociatedCh[uiAssocId].hPriDecCh[j]->pPath[k]->pDstDetails[uiDstIndex])) &&
                               (hRap->sAssociatedCh[uiAssocId].hPriDecCh[j]->pPath[k]->pDstDetails[uiDstIndex]->eAudioDst == BRAP_AudioDst_eRingBuffer))
                            {
                                for(l =0; l <BRAP_RM_P_MAX_MIXING_LEVELS;l++ )
                                {
                                    for(m =0; m <BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;m++ )                        
                                    {
                                        for(n =0; n <BRAP_RM_P_MAX_PARALLEL_PATHS;n++ )                                  
                                        {
                                            for(p =0; p <BRAP_RM_P_MAX_SRC_IN_CASCADE;p++ )                                             
                                            {
                                                hSrc = hRap->sAssociatedCh[uiAssocId].hPriDecCh[j]->pPath[k]->sSrc[l][m][n].hSrc[p];                      
                                                if(BRAP_P_IsPointerValid((void *)hSrc))
                                                {
                                                    BDBG_MSG(("Programming Src Index %d for Primary Decode channel",hSrc->uiIndex));
                                                    if (hRap->sAssociatedCh[uiAssocId].hPriDecCh[j]->eSamplingRate == eCaptureDataSR)
                                                    {
                                                        /* If earlier SRC type is LinInt return the Coeff Addr as it now becomes Bypass */
                                                        if((hSrc->sParams.eSrcType == BRAP_SRC_P_Type_eLinInt) &&
                                                           (hSrc->uiCoeffAddr != BRAP_INVALID_VALUE))
                                                        {
                                                            ret = BRAP_SRC_P_DeallocateCoeffAddr(hSrc);
                                                            if(ret != BERR_SUCCESS)
                                                            {
                                                                BDBG_ERR(("BRAP_P_ProgramSRCOfRingBufferDstn_isr: BRAP_SRC_P_DeallocateCoeffAddr returned error"));
                                                                return ret;
                                                            }
                                                        }
                                                        
                                                        /* Program SRC type */                                                
                                                        ui32RegVal = BRAP_Read32 (hSrc->hRegister,
                                                                hSrc->uiBlkOffset + BCHP_AUD_FMM_SRC_CTRL0_SRC_CFGi_ARRAY_BASE +
                                                                hSrc->uiSrcOffset);

                                                        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_SRC_CTRL0_SRC_CFGi,SRC_TYPE));
                                                        ui32RegVal |= (BCHP_FIELD_DATA(AUD_FMM_SRC_CTRL0_SRC_CFGi,SRC_TYPE,
                                                                        BRAP_SRC_P_Type_eBypass));                                                     

                                                        BRAP_Write32(hSrc->hRegister,
                                                            hSrc->uiBlkOffset + BCHP_AUD_FMM_SRC_CTRL0_SRC_CFGi_ARRAY_BASE +
                                                            hSrc->uiSrcOffset, ui32RegVal);

                                                        
                                                        hSrc->sParams.eSrcType = BRAP_SRC_P_Type_eBypass;
                                                        hSrc->sParams.eInputSR = hRap->sAssociatedCh[uiAssocId].hPriDecCh[j]->eSamplingRate;
                                                        hSrc->sParams.eOutputSR = eCaptureDataSR;
                                                        BDBG_MSG (("The SRC Type is made as Bypass"));
                                                        bSRCProgrmd = true;
                                                        break;
                                                    }
                                                    else
                                                    {
                                                        hSrc->sParams.eSrcType = BRAP_SRC_P_Type_eLinInt;
                                                        hSrc->sParams.eInputSR = hRap->sAssociatedCh[uiAssocId].hPriDecCh[j]->eSamplingRate;
                                                        hSrc->sParams.eOutputSR = eCaptureDataSR;

                                                        /* Calculate Den/Num/DenScale of the SRC */
                                                        uiDen = uiCaptureDataSR;
                                                        uiNum = ui32SamplingRate;

                                                        if((uiDen  % 100 == 0)&&(uiNum % 100 == 0))
                                                        {
                                                            uiDivider =100;
                                                        }
                                                        else if((uiDen  % 10 == 0)&&(uiNum % 10 == 0))
                                                        {
                                                            uiDivider =10;
                                                        }
                                                        else
                                                        {
                                                            uiDivider =1;
                                                        }
                                                        uiDen = uiDen/uiDivider;
                                                        uiNum= uiNum/uiDivider;
                                                        uiDenScale = (1 << 24)/uiDen;        /*2^24/Den*/
                                                        BDBG_MSG(("uiNum = %d, uiDen =%d, uiScale =%d",uiNum,uiDen,uiDenScale));
                                                        
                                                        /* Program SRC Coefficients */ 
                                                        if(hSrc->uiCoeffAddr == BRAP_INVALID_VALUE)
                                                        {
                                                            ret = BRAP_SRC_P_AllocateCoeffAddr(hSrc);

                                                            if(BERR_SUCCESS != ret)
                                                            {
                                                                BDBG_ERR(("BRAP_P_ProgramSRCOfRingBufferDstn_isr: "
                                                                            "BRAP_SRC_P_AllocateCoeffAddr returned error"));
                                                                return ret;
                                                            }
                                                        }

                                                        BRAP_Write32(hSrc->hRegister,
                                                                        hSrc->uiBlkOffset +
                                                                        BCHP_AUD_FMM_SRC_CTRL0_COEFFi_ARRAY_BASE + (hSrc->uiCoeffAddr*4),
                                                                        uiNum);
                                                        
                                                        BRAP_Write32(hSrc->hRegister,
                                                                        hSrc->uiBlkOffset +
                                                                        BCHP_AUD_FMM_SRC_CTRL0_COEFFi_ARRAY_BASE + ((hSrc->uiCoeffAddr+1)*4),
                                                                        uiDen);
                                                        
                                                        BRAP_Write32(hSrc->hRegister,
                                                                        hSrc->uiBlkOffset +
                                                                        BCHP_AUD_FMM_SRC_CTRL0_COEFFi_ARRAY_BASE + ((hSrc->uiCoeffAddr+2)*4),
                                                                        uiDenScale);                                                

                                                        /* Program adress where coefficents are written */
                                                        ui32RegVal = BRAP_Read32 (hSrc->hRegister,
                                                            hSrc->uiBlkOffset +(BCHP_AUD_FMM_SRC_CTRL0_CF_SELi_ARRAY_BASE +
                                                            (4 * hSrc->uiIndex)));

                                                        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_SRC_CTRL0_CF_SELi,CF_BASE_ADDR0));
                                                        ui32RegVal |= (BCHP_FIELD_DATA(AUD_FMM_SRC_CTRL0_CF_SELi,CF_BASE_ADDR0,(hSrc->uiCoeffAddr+24)));
                                                    
                                                        BRAP_Write32(hSrc->hRegister,
                                                            hSrc->uiBlkOffset +(BCHP_AUD_FMM_SRC_CTRL0_CF_SELi_ARRAY_BASE +
                                                            (4 * hSrc->uiIndex)), ui32RegVal);

                                                        /* Program SRC type */
                                                        ui32RegVal = BRAP_Read32 (hSrc->hRegister,
                                                            hSrc->uiBlkOffset + BCHP_AUD_FMM_SRC_CTRL0_SRC_CFGi_ARRAY_BASE +
                                                            hSrc->uiSrcOffset);

                                                        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_SRC_CTRL0_SRC_CFGi,SRC_TYPE));
                                                        ui32RegVal |= (BCHP_FIELD_DATA(AUD_FMM_SRC_CTRL0_SRC_CFGi,SRC_TYPE,
                                                                        BRAP_SRC_P_Type_eLinInt));

                                                        BRAP_Write32(hSrc->hRegister,
                                                            hSrc->uiBlkOffset + BCHP_AUD_FMM_SRC_CTRL0_SRC_CFGi_ARRAY_BASE +
                                                            hSrc->uiSrcOffset, ui32RegVal);

                                                        BDBG_MSG (("The SRC Type is made as Linear Interpolation"));
                                                        bSRCProgrmd = true;
                                                        break;                                                        
                                                    }
                                                }
                                            }
                                            if(bSRCProgrmd == true)
                                                break;
                                        }
                                        if(bSRCProgrmd == true)
                                            break;
                                    }
                                    if(bSRCProgrmd == true)
                                        break;                                    
                                }
                            }
                            if(bSRCProgrmd == true)
                                break;                            
                        }
                        if(bSRCProgrmd == true)
                            break;                        
                    }
                }
                if(bSRCProgrmd == true)
                    break;                
            }

            /* Break from entire for loop */
            if(bSRCProgrmd == true)
                break;

            for(j = 0 ; j < BRAP_MAX_SEC_DEC_CHAN_IN_ASSOCIATED_GRP; j++)
            {          
                if(BRAP_P_IsPointerValid((void *)hRap->sAssociatedCh[uiAssocId].hSecDecCh[j]))
                {
                    for(k = 0 ; k < BRAP_P_MAX_PATHS_IN_A_CHAN ; k++)
                    {
                        if(!(BRAP_P_IsPointerValid((void *)hRap->sAssociatedCh[uiAssocId].hSecDecCh[j]->pPath[k])))
                            break;
                        for(uiDstIndex = 0; uiDstIndex < BRAP_P_MAX_DST_PER_RAPCH; uiDstIndex++)
                        {
                            if((BRAP_P_IsPointerValid((void *)hRap->sAssociatedCh[uiAssocId].hSecDecCh[j]->pPath[k]->pDstDetails[uiDstIndex])) &&
                               (hRap->sAssociatedCh[uiAssocId].hSecDecCh[j]->pPath[k]->pDstDetails[uiDstIndex]->eAudioDst == BRAP_AudioDst_eRingBuffer))
                            {
                                for(l =0; l <BRAP_RM_P_MAX_MIXING_LEVELS;l++ )
                                {
                                    for(m =0; m <BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;m++ )                        
                                    {
                                        for(n =0; n <BRAP_RM_P_MAX_PARALLEL_PATHS;n++ )                                  
                                        {
                                            for(p =0; p <BRAP_RM_P_MAX_SRC_IN_CASCADE;p++ )                                             
                                            {
                                                hSrc = hRap->sAssociatedCh[uiAssocId].hSecDecCh[j]->pPath[k]->sSrc[l][m][n].hSrc[p];                      
                                                if(BRAP_P_IsPointerValid((void *)hSrc))
                                                {
                                                    BDBG_MSG(("Programming Src Index %d for Secondary Decode channel",hSrc->uiIndex));
                                                    if (hRap->sAssociatedCh[uiAssocId].hSecDecCh[j]->eSamplingRate == eCaptureDataSR)
                                                    {
                                                        /* If earlier SRC type is LinInt return the Coeff Addr as it now becomes Bypass */
                                                        if((hSrc->sParams.eSrcType == BRAP_SRC_P_Type_eLinInt) &&
                                                           (hSrc->uiCoeffAddr != BRAP_INVALID_VALUE))
                                                        {
                                                            ret = BRAP_SRC_P_DeallocateCoeffAddr(hSrc);
                                                            if(ret != BERR_SUCCESS)
                                                            {
                                                                BDBG_ERR(("BRAP_P_ProgramSRCOfRingBufferDstn_isr: BRAP_SRC_P_DeallocateCoeffAddr returned error"));
                                                                return ret;
                                                            }
                                                        }
                                                        
                                                        /* Program SRC type */                                                
                                                        ui32RegVal = BRAP_Read32 (hSrc->hRegister,
                                                                hSrc->uiBlkOffset + BCHP_AUD_FMM_SRC_CTRL0_SRC_CFGi_ARRAY_BASE +
                                                                hSrc->uiSrcOffset);

                                                        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_SRC_CTRL0_SRC_CFGi,SRC_TYPE));
                                                        ui32RegVal |= (BCHP_FIELD_DATA(AUD_FMM_SRC_CTRL0_SRC_CFGi,SRC_TYPE,
                                                                        BRAP_SRC_P_Type_eBypass));                                                     

                                                        BRAP_Write32(hSrc->hRegister,
                                                            hSrc->uiBlkOffset + BCHP_AUD_FMM_SRC_CTRL0_SRC_CFGi_ARRAY_BASE +
                                                            hSrc->uiSrcOffset, ui32RegVal);

                                                        hSrc->sParams.eSrcType = BRAP_SRC_P_Type_eBypass;
                                                        hSrc->sParams.eInputSR = hRap->sAssociatedCh[uiAssocId].hSecDecCh[j]->eSamplingRate;
                                                        hSrc->sParams.eOutputSR = eCaptureDataSR;
                                                        BDBG_MSG (("The SRC Type is made as Bypass"));
                                                        bSRCProgrmd = true;
                                                        break;
                                                    }
                                                    else
                                                    {
                                                        hSrc->sParams.eSrcType = BRAP_SRC_P_Type_eLinInt;
                                                        hSrc->sParams.eInputSR = hRap->sAssociatedCh[uiAssocId].hSecDecCh[j]->eSamplingRate;
                                                        hSrc->sParams.eOutputSR = eCaptureDataSR;
                                                        
                                                        /* Calculate Den/Num/DenScale of the SRC*/
                                                        uiDen = uiCaptureDataSR;
                                                        uiNum = ui32SamplingRate;

                                                        if((uiDen  % 100 == 0)&&(uiNum % 100 == 0))
                                                        {
                                                            uiDivider =100;
                                                        }
                                                        else if((uiDen  % 10 == 0)&&(uiNum % 10 == 0))
                                                        {
                                                            uiDivider =10;
                                                        }
                                                        else
                                                        {
                                                            uiDivider =1;
                                                        }
                                                        uiDen = uiDen/uiDivider;
                                                        uiNum= uiNum/uiDivider;
                                                        uiDenScale = (1 << 24)/uiDen;        /*2^24/Den*/
                                                        BDBG_MSG(("uiNum = %d, uiDen =%d, uiScale =%d",uiNum,uiDen,uiDenScale));
                                                        
                                                        /* Program SRC Coefficients */                                                
                                                        if(hSrc->uiCoeffAddr == BRAP_INVALID_VALUE)
                                                        {
                                                            ret = BRAP_SRC_P_AllocateCoeffAddr(hSrc);

                                                            if(BERR_SUCCESS != ret)
                                                            {
                                                                BDBG_ERR(("BRAP_P_ProgramSRCOfRingBufferDstn_isr: "
                                                                            "BRAP_SRC_P_AllocateCoeffAddr returned error"));
                                                                return ret;
                                                            }
                                                        }

                                                        BRAP_Write32(hSrc->hRegister,
                                                                        hSrc->uiBlkOffset +
                                                                        BCHP_AUD_FMM_SRC_CTRL0_COEFFi_ARRAY_BASE + (hSrc->uiCoeffAddr*4),
                                                                        uiNum);
                                                        
                                                        BRAP_Write32(hSrc->hRegister,
                                                                        hSrc->uiBlkOffset +
                                                                        BCHP_AUD_FMM_SRC_CTRL0_COEFFi_ARRAY_BASE + ((hSrc->uiCoeffAddr+1)*4),
                                                                        uiDen);
                                                        
                                                        BRAP_Write32(hSrc->hRegister,
                                                                        hSrc->uiBlkOffset +
                                                                        BCHP_AUD_FMM_SRC_CTRL0_COEFFi_ARRAY_BASE + ((hSrc->uiCoeffAddr+2)*4),
                                                                        uiDenScale);                                                

                                                        /* Program adress where coefficents are written */
                                                        ui32RegVal = BRAP_Read32 (hSrc->hRegister,
                                                            hSrc->uiBlkOffset +(BCHP_AUD_FMM_SRC_CTRL0_CF_SELi_ARRAY_BASE +
                                                            (4 * hSrc->uiIndex)));

                                                        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_SRC_CTRL0_CF_SELi,CF_BASE_ADDR0));
                                                        ui32RegVal |= (BCHP_FIELD_DATA(AUD_FMM_SRC_CTRL0_CF_SELi,CF_BASE_ADDR0,(hSrc->uiCoeffAddr+24)));
                                                    
                                                        BRAP_Write32(hSrc->hRegister,
                                                            hSrc->uiBlkOffset +(BCHP_AUD_FMM_SRC_CTRL0_CF_SELi_ARRAY_BASE +
                                                            (4 * hSrc->uiIndex)), ui32RegVal);

                                                        /* Program SRC type */
                                                        ui32RegVal = BRAP_Read32 (hSrc->hRegister,
                                                            hSrc->uiBlkOffset + BCHP_AUD_FMM_SRC_CTRL0_SRC_CFGi_ARRAY_BASE +
                                                            hSrc->uiSrcOffset);

                                                        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_SRC_CTRL0_SRC_CFGi,SRC_TYPE));
                                                        ui32RegVal |= (BCHP_FIELD_DATA(AUD_FMM_SRC_CTRL0_SRC_CFGi,SRC_TYPE,
                                                                        BRAP_SRC_P_Type_eLinInt));

                                                        BRAP_Write32(hSrc->hRegister,
                                                            hSrc->uiBlkOffset + BCHP_AUD_FMM_SRC_CTRL0_SRC_CFGi_ARRAY_BASE +
                                                            hSrc->uiSrcOffset, ui32RegVal);

                                                        BDBG_MSG (("The SRC Type is made as Linear Interpolation"));
                                                        bSRCProgrmd = true;
                                                        break;                                                        
                                                    }
                                                }
                                            }
                                            if(bSRCProgrmd == true)
                                                break;
                                        }
                                        if(bSRCProgrmd == true)
                                            break;
                                    }
                                    if(bSRCProgrmd == true)
                                        break;                                    
                                }
                            }
                            if(bSRCProgrmd == true)
                                break;                            
                        }
                        if(bSRCProgrmd == true)
                            break;                        
                    }
                }
                if(bSRCProgrmd == true)
                    break;                
            }

            /* Break from entire for loop */
            if(bSRCProgrmd == true)
                break;

            for(j =0 ; j < BRAP_MAX_PB_CHAN_IN_ASSOCIATED_GRP; j++)
            {          
                if(BRAP_P_IsPointerValid((void *)hRap->sAssociatedCh[uiAssocId].hPBCh[j]))
                {          
                    /* Store the output sample rate of PB channel */
                    BRAP_P_ConvertSrToEnum(ui32SamplingRate,&(hRap->sAssociatedCh[uiAssocId].hPBCh[j]->eSamplingRate));                
                    BDBG_MSG (("The Sampling rate of Decode channel = %d", ui32SamplingRate));
                    BDBG_MSG (("The Input Sampling rate of Playback channel = %d", hRap->sAssociatedCh[uiAssocId].hPBCh[j]->uiInputSamplingRate));

                    for(k = 0 ; k < BRAP_P_MAX_PATHS_IN_A_CHAN ; k++)
                    {
                        if(!(BRAP_P_IsPointerValid((void *)hRap->sAssociatedCh[uiAssocId].hPBCh[j]->pPath[k])))
                            break;
                        for(uiDstIndex=0; uiDstIndex<BRAP_P_MAX_DST_PER_RAPCH; uiDstIndex++)
                        {
                            if((BRAP_P_IsPointerValid((void *)hRap->sAssociatedCh[uiAssocId].hPBCh[j]->pPath[k]->pDstDetails[uiDstIndex])) &&
                               (hRap->sAssociatedCh[uiAssocId].hPBCh[j]->pPath[k]->pDstDetails[uiDstIndex]->eAudioDst == BRAP_AudioDst_eRingBuffer))
                            {
                                for(l =0; l <BRAP_RM_P_MAX_MIXING_LEVELS;l++ )
                                {
                                    for(m =0; m <BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;m++ )                        
                                    {
                                        for(n =0; n <BRAP_RM_P_MAX_PARALLEL_PATHS;n++ )                                  
                                        {
                                            for(p =0; p <BRAP_RM_P_MAX_SRC_IN_CASCADE;p++ )                                             
                                            {
                                                hSrc = hRap->sAssociatedCh[uiAssocId].hPBCh[j]->pPath[k]->sSrc[l][m][n].hSrc[p];                      
                                                if(BRAP_P_IsPointerValid((void *)hSrc))
                                                {
                                                    BDBG_MSG(("Programming Src Index %d for Playback channel",hSrc->uiIndex));
                                                    BRAP_P_ConvertSrToEnum(hRap->sAssociatedCh[uiAssocId].hPBCh[j]->uiInputSamplingRate,&eInputSamplingRate);                                                    
                                                    if(eInputSamplingRate == eCaptureDataSR)
                                                    {
                                                        /* If earlier SRC type is LinInt return the Coeff Addr as it now becomes Bypass */
                                                        if((hSrc->sParams.eSrcType == BRAP_SRC_P_Type_eLinInt) &&
                                                           (hSrc->uiCoeffAddr != BRAP_INVALID_VALUE))
                                                        {
                                                            ret = BRAP_SRC_P_DeallocateCoeffAddr(hSrc);
                                                            if(ret != BERR_SUCCESS)
                                                            {
                                                                BDBG_ERR(("BRAP_P_ProgramSRCOfRingBufferDstn_isr: BRAP_SRC_P_DeallocateCoeffAddr returned error"));
                                                                return ret;
                                                            }
                                                        }
                                                        
                                                        /* Program SRC type */                                                
                                                        ui32RegVal = BRAP_Read32 (hSrc->hRegister,
                                                                hSrc->uiBlkOffset + BCHP_AUD_FMM_SRC_CTRL0_SRC_CFGi_ARRAY_BASE +
                                                                hSrc->uiSrcOffset);

                                                        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_SRC_CTRL0_SRC_CFGi,SRC_TYPE));
                                                        ui32RegVal |= (BCHP_FIELD_DATA(AUD_FMM_SRC_CTRL0_SRC_CFGi,SRC_TYPE,
                                                                        BRAP_SRC_P_Type_eBypass));                                                     

                                                        BRAP_Write32(hSrc->hRegister,
                                                            hSrc->uiBlkOffset + BCHP_AUD_FMM_SRC_CTRL0_SRC_CFGi_ARRAY_BASE +
                                                            hSrc->uiSrcOffset, ui32RegVal);

                                                        hSrc->sParams.eSrcType = BRAP_SRC_P_Type_eBypass;
                                                        hSrc->sParams.eInputSR = eInputSamplingRate;
                                                        hSrc->sParams.eOutputSR = eCaptureDataSR;
                                                        BDBG_MSG (("The SRC Type is made as Bypass"));
                                                        bSRCProgrmd = true;
                                                        break;                                                        
                                                    }
                                                    else
                                                    {
                                                        hSrc->sParams.eSrcType = BRAP_SRC_P_Type_eLinInt;
                                                        hSrc->sParams.eInputSR = eInputSamplingRate;
                                                        hSrc->sParams.eOutputSR = eCaptureDataSR;
                                                        
                                                        /* Calculate Den/Num/DenScale of the SRC*/
                                                        uiDen = uiCaptureDataSR;
                                                        uiNum = hRap->sAssociatedCh[uiAssocId].hPBCh[j]->uiInputSamplingRate;

                                                        if((uiDen  % 100 == 0)&&(uiNum % 100 == 0))
                                                        {
                                                            uiDivider =100;
                                                        }
                                                        else if((uiDen  % 10 == 0)&&(uiNum % 10 == 0))
                                                        {
                                                            uiDivider =10;
                                                        }
                                                        else
                                                        {
                                                            uiDivider =1;
                                                        }
                                                        uiDen = uiDen/uiDivider;
                                                        uiNum= uiNum/uiDivider;
                                                        uiDenScale = (1 << 24)/uiDen;        /*2^24/Den*/
                                                        BDBG_MSG(("uiNum = %d, uiDen =%d, uiScale =%d",uiNum,uiDen,uiDenScale));
                                                        
                                                        /* Program SRC Coefficients */                                                
                                                        if(hSrc->uiCoeffAddr == BRAP_INVALID_VALUE)
                                                        {
                                                            ret = BRAP_SRC_P_AllocateCoeffAddr(hSrc);

                                                            if(BERR_SUCCESS != ret)
                                                            {
                                                                BDBG_ERR(("BRAP_P_ProgramSRCOfRingBufferDstn_isr: "
                                                                            "BRAP_SRC_P_AllocateCoeffAddr returned error"));
                                                                return ret;
                                                            }
                                                        }

                                                        BRAP_Write32(hSrc->hRegister,
                                                                        hSrc->uiBlkOffset +
                                                                        BCHP_AUD_FMM_SRC_CTRL0_COEFFi_ARRAY_BASE + (hSrc->uiCoeffAddr*4),
                                                                        uiNum);
                                                        
                                                        BRAP_Write32(hSrc->hRegister,
                                                                        hSrc->uiBlkOffset +
                                                                        BCHP_AUD_FMM_SRC_CTRL0_COEFFi_ARRAY_BASE + ((hSrc->uiCoeffAddr+1)*4),
                                                                        uiDen);
                                                        
                                                        BRAP_Write32(hSrc->hRegister,
                                                                        hSrc->uiBlkOffset +
                                                                        BCHP_AUD_FMM_SRC_CTRL0_COEFFi_ARRAY_BASE + ((hSrc->uiCoeffAddr+2)*4),
                                                                        uiDenScale);                                                

                                                        /* Program adress where coefficents are written */
                                                        ui32RegVal = BRAP_Read32 (hSrc->hRegister,
                                                            hSrc->uiBlkOffset +(BCHP_AUD_FMM_SRC_CTRL0_CF_SELi_ARRAY_BASE +
                                                            (4 * hSrc->uiIndex)));

                                                        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_SRC_CTRL0_CF_SELi,CF_BASE_ADDR0));
                                                        ui32RegVal |= (BCHP_FIELD_DATA(AUD_FMM_SRC_CTRL0_CF_SELi,CF_BASE_ADDR0,(hSrc->uiCoeffAddr+24)));
                                                    
                                                        BRAP_Write32(hSrc->hRegister,
                                                            hSrc->uiBlkOffset +(BCHP_AUD_FMM_SRC_CTRL0_CF_SELi_ARRAY_BASE +
                                                            (4 * hSrc->uiIndex)), ui32RegVal);

                                                        /* Program SRC type */
                                                        ui32RegVal = BRAP_Read32 (hSrc->hRegister,
                                                            hSrc->uiBlkOffset + BCHP_AUD_FMM_SRC_CTRL0_SRC_CFGi_ARRAY_BASE +
                                                            hSrc->uiSrcOffset);

                                                        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_SRC_CTRL0_SRC_CFGi,SRC_TYPE));
                                                        ui32RegVal |= (BCHP_FIELD_DATA(AUD_FMM_SRC_CTRL0_SRC_CFGi,SRC_TYPE,
                                                                        BRAP_SRC_P_Type_eLinInt));

                                                        BRAP_Write32(hSrc->hRegister,
                                                            hSrc->uiBlkOffset + BCHP_AUD_FMM_SRC_CTRL0_SRC_CFGi_ARRAY_BASE +
                                                            hSrc->uiSrcOffset, ui32RegVal);

                                                        BDBG_MSG (("The SRC Type is made as Linear Interpolation"));
                                                        bSRCProgrmd = true;
                                                        break;
                                                    }
                                                }
                                            }
                                            if(bSRCProgrmd == true)
                                                break;
                                        }
                                        if(bSRCProgrmd == true)
                                            break;
                                    }
                                    if(bSRCProgrmd == true)
                                        break;
                                }
                            }
                            if(bSRCProgrmd == true)
                                break;
                        }
                        if(bSRCProgrmd == true)
                            break;
                    }
                }
                if(bSRCProgrmd == true)
                    break;
            }            
            /* Break from entire for loop */
            if(bSRCProgrmd == true)
                break;              

            for(j = 0 ; j < BRAP_MAX_CAP_CHAN_IN_ASSOCIATED_GRP; j++)
            {          
                if(BRAP_P_IsPointerValid((void *)hRap->sAssociatedCh[uiAssocId].hCapCh[j]))
                {
                    /* Store the output sample rate of PB channel */
                    BRAP_P_ConvertSrToEnum(ui32SamplingRate,&(hRap->sAssociatedCh[uiAssocId].hCapCh[j]->eSamplingRate));                
                    BDBG_MSG (("The Sampling rate of Decode channel = %d", ui32SamplingRate));
                    BDBG_MSG (("The Input Sampling rate of Capture channel = %d", hRap->sAssociatedCh[uiAssocId].hCapCh[j]->uiInputSamplingRate));

                    for(k = 0 ; k < BRAP_P_MAX_PATHS_IN_A_CHAN ; k++)
                    {
                        if(!(BRAP_P_IsPointerValid((void *)hRap->sAssociatedCh[uiAssocId].hCapCh[j]->pPath[k])))
                            break;
                        for(uiDstIndex = 0; uiDstIndex < BRAP_P_MAX_DST_PER_RAPCH; uiDstIndex++)
                        {
                            if((BRAP_P_IsPointerValid((void *)hRap->sAssociatedCh[uiAssocId].hCapCh[j]->pPath[k]->pDstDetails[uiDstIndex])) &&
                               (hRap->sAssociatedCh[uiAssocId].hCapCh[j]->pPath[k]->pDstDetails[uiDstIndex]->eAudioDst == BRAP_AudioDst_eRingBuffer))
                            {
                                for(l =0; l <BRAP_RM_P_MAX_MIXING_LEVELS;l++ )
                                {
                                    for(m =0; m <BRAP_RM_P_MAX_OP_CHANNEL_PAIRS;m++ )                        
                                    {
                                        for(n =0; n <BRAP_RM_P_MAX_PARALLEL_PATHS;n++ )                                  
                                        {
                                            for(p =0; p <BRAP_RM_P_MAX_SRC_IN_CASCADE;p++ )                                             
                                            {
                                                hSrc = hRap->sAssociatedCh[uiAssocId].hCapCh[j]->pPath[k]->sSrc[l][m][n].hSrc[p];                      
                                                if(BRAP_P_IsPointerValid((void *)hSrc))
                                                {
                                                    BDBG_MSG(("Programming Src Index for Capture channel= %d",hSrc->uiIndex));
                                                    BRAP_P_ConvertSrToEnum(hRap->sAssociatedCh[uiAssocId].hCapCh[j]->uiInputSamplingRate,&eInputSamplingRate);                                                                                                        
                                                    if (eInputSamplingRate == eCaptureDataSR)
                                                    {
                                                        /* If earlier SRC type is LinInt return the Coeff Addr as it now becomes Bypass */
                                                        if((hSrc->sParams.eSrcType == BRAP_SRC_P_Type_eLinInt) &&
                                                           (hSrc->uiCoeffAddr != BRAP_INVALID_VALUE))
                                                        {
                                                            ret = BRAP_SRC_P_DeallocateCoeffAddr(hSrc);
                                                            if(ret != BERR_SUCCESS)
                                                            {
                                                                BDBG_ERR(("BRAP_P_ProgramSRCOfRingBufferDstn_isr: BRAP_SRC_P_DeallocateCoeffAddr returned error"));
                                                                return ret;
                                                            }
                                                        }
                                                        
                                                        /* Program SRC type */                                                
                                                        ui32RegVal = BRAP_Read32 (hSrc->hRegister,
                                                                hSrc->uiBlkOffset + BCHP_AUD_FMM_SRC_CTRL0_SRC_CFGi_ARRAY_BASE +
                                                                hSrc->uiSrcOffset);

                                                        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_SRC_CTRL0_SRC_CFGi,SRC_TYPE));
                                                        ui32RegVal |= (BCHP_FIELD_DATA(AUD_FMM_SRC_CTRL0_SRC_CFGi,SRC_TYPE,
                                                                        BRAP_SRC_P_Type_eBypass));                                                     

                                                        BRAP_Write32(hSrc->hRegister,
                                                            hSrc->uiBlkOffset + BCHP_AUD_FMM_SRC_CTRL0_SRC_CFGi_ARRAY_BASE +
                                                            hSrc->uiSrcOffset, ui32RegVal);

                                                        hSrc->sParams.eSrcType = BRAP_SRC_P_Type_eBypass;
                                                        hSrc->sParams.eInputSR = eInputSamplingRate;
                                                        hSrc->sParams.eOutputSR = eCaptureDataSR;
                                                        BDBG_MSG (("The SRC Type is made as Bypass"));
                                                        bSRCProgrmd = true;
                                                        break;
                                                    }
                                                    else
                                                    {
                                                        hSrc->sParams.eSrcType = BRAP_SRC_P_Type_eLinInt;
                                                        hSrc->sParams.eInputSR = eInputSamplingRate;
                                                        hSrc->sParams.eOutputSR = eCaptureDataSR;
                                                        
                                                        /* Calculate Den/Num/DenScale of the SRC*/
                                                        uiDen = uiCaptureDataSR;
                                                        uiNum = hRap->sAssociatedCh[uiAssocId].hCapCh[j]->uiInputSamplingRate;
                                                        if((uiDen  % 100 == 0)&&(uiNum % 100 == 0))
                                                        {
                                                            uiDivider =100;
                                                        }
                                                        else if((uiDen  % 10 == 0)&&(uiNum % 10 == 0))
                                                        {
                                                            uiDivider =10;
                                                        }
                                                        else
                                                        {
                                                            uiDivider =1;
                                                        }
                                                        uiDen = uiDen/uiDivider;
                                                        uiNum= uiNum/uiDivider;
                                                        uiDenScale = (1 << 24)/uiDen;        /*2^24/Den*/
                                                        BDBG_MSG(("uiNum = %d, uiDen =%d, uiScale =%d",uiNum,uiDen,uiDenScale));
                                                        
                                                        /* Program SRC Coefficients */                                                
                                                        if(hSrc->uiCoeffAddr == BRAP_INVALID_VALUE)
                                                        {
                                                            ret = BRAP_SRC_P_AllocateCoeffAddr(hSrc);

                                                            if(BERR_SUCCESS != ret)
                                                            {
                                                                BDBG_ERR(("BRAP_P_ProgramSRCOfRingBufferDstn_isr: "
                                                                            "BRAP_SRC_P_AllocateCoeffAddr returned error"));
                                                                return ret;
                                                            }
                                                        }

                                                        BRAP_Write32(hSrc->hRegister,
                                                                        hSrc->uiBlkOffset +
                                                                        BCHP_AUD_FMM_SRC_CTRL0_COEFFi_ARRAY_BASE + (hSrc->uiCoeffAddr*4),
                                                                        uiNum);
                                                        
                                                        BRAP_Write32(hSrc->hRegister,
                                                                        hSrc->uiBlkOffset +
                                                                        BCHP_AUD_FMM_SRC_CTRL0_COEFFi_ARRAY_BASE + ((hSrc->uiCoeffAddr+1)*4),
                                                                        uiDen);
                                                        
                                                        BRAP_Write32(hSrc->hRegister,
                                                                        hSrc->uiBlkOffset +
                                                                        BCHP_AUD_FMM_SRC_CTRL0_COEFFi_ARRAY_BASE + ((hSrc->uiCoeffAddr+2)*4),
                                                                        uiDenScale);                                                

                                                        /* Program adress where coefficents are written */
                                                        ui32RegVal = BRAP_Read32 (hSrc->hRegister,
                                                            hSrc->uiBlkOffset +(BCHP_AUD_FMM_SRC_CTRL0_CF_SELi_ARRAY_BASE +
                                                            (4 * hSrc->uiIndex)));

                                                        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_SRC_CTRL0_CF_SELi,CF_BASE_ADDR0));
                                                        ui32RegVal |= (BCHP_FIELD_DATA(AUD_FMM_SRC_CTRL0_CF_SELi,CF_BASE_ADDR0,(hSrc->uiCoeffAddr+24)));
                                                    
                                                        BRAP_Write32(hSrc->hRegister,
                                                            hSrc->uiBlkOffset +(BCHP_AUD_FMM_SRC_CTRL0_CF_SELi_ARRAY_BASE +
                                                            (4 * hSrc->uiIndex)), ui32RegVal);

                                                        /* Program SRC type */
                                                        ui32RegVal = BRAP_Read32 (hSrc->hRegister,
                                                            hSrc->uiBlkOffset + BCHP_AUD_FMM_SRC_CTRL0_SRC_CFGi_ARRAY_BASE +
                                                            hSrc->uiSrcOffset);

                                                        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_SRC_CTRL0_SRC_CFGi,SRC_TYPE));
                                                        ui32RegVal |= (BCHP_FIELD_DATA(AUD_FMM_SRC_CTRL0_SRC_CFGi,SRC_TYPE,
                                                                        BRAP_SRC_P_Type_eLinInt));

                                                        BRAP_Write32(hSrc->hRegister,
                                                            hSrc->uiBlkOffset + BCHP_AUD_FMM_SRC_CTRL0_SRC_CFGi_ARRAY_BASE +
                                                            hSrc->uiSrcOffset, ui32RegVal);

                                                        BDBG_MSG (("The SRC Type is made as Linear Interpolation"));
                                                        bSRCProgrmd = true;
                                                        break;                                                        
                                                    }
                                                }
                                            }
                                            if(bSRCProgrmd == true)
                                                break;
                                        }
                                        if(bSRCProgrmd == true)
                                            break;
                                    }
                                    if(bSRCProgrmd == true)
                                        break;                                    
                                }
                            }
                            if(bSRCProgrmd == true)
                                break;                            
                        }
                        if(bSRCProgrmd == true)
                            break;                        
                    }
                }
                if(bSRCProgrmd == true)
                    break;                
            }
            /* Break from entire for loop */
            if(bSRCProgrmd == true)
                break;                                      
        }
    }

    BDBG_LEAVE(BRAP_P_ProgramSRCOfRingBufferDstn_isr);    
    return ret;
}
#endif /* #if (BRAP_48KHZ_RINGBUFFER_DESTINATION == 1) */
#endif /* #if (BRAP_3548_FAMILY == 1) */
#endif /* #if (BRAP_7550_FAMILY != 1) */

static void BRAP_P_IsDtsTranscodePresent_isr(BRAP_DSPCHN_Handle  hDspCh ,unsigned int uiTaskId,bool *bPresent)
{
    unsigned int i=0,j=0,k=0;
    bool bFound =false;

    BDBG_ENTER(BRAP_P_IsDtsTranscodePresent_isr);
    BDBG_ASSERT(hDspCh);
    BDBG_ASSERT(bPresent);    

    for(k =0; k< BRAP_FWIF_P_MAX_FW_TASK_PER_DSPCHN ; k++)
    {
        if(hDspCh->sFwTaskInfo[k].hFwTask->uiTaskId == uiTaskId)
        {
            bFound =true;
            break;
        }
    }
    if(bFound == true)
    {
    for(i = 0; i < BRAP_P_MAX_DST_PER_RAPCH; i++)
    {
        for(j = 0; j < BRAP_MAX_STAGE_PER_BRANCH_SUPPORTED; j++)
        {    
                if((hDspCh->sFwTaskInfo[k].sProcessingNw.sAudProcessingStage[i][j].bDecoderStage == true)
                    || (hDspCh->sFwTaskInfo[k].sProcessingNw.sAudProcessingStage[i][j].bCloneStage== true))
                    continue;
                if((hDspCh->sFwTaskInfo[k].sProcessingNw.sAudProcessingStage[i][j].hAudioProcessing == NULL))
                break;
            
                    if((hDspCh->sFwTaskInfo[k].sProcessingNw.sAudProcessingStage[i][j].hAudioProcessing->sProcessingStageSettings.eAudioProcessing == BRAP_ProcessingType_eEncodeDts))
                {
                    *bPresent = true;
                      break;
                }
        }    
        if(*bPresent == true)
            break;
        if(j == 0)
            break;
        }
    }
    else
    {
        *bPresent = false;
    }
    BDBG_LEAVE(BRAP_P_IsDtsTranscodePresent_isr);    
}

static void BRAP_P_SampleRateChange_isr(void *pParam1, void *pParam2,void *pParam3)
{
    BRAP_ChannelHandle hRapCh = (BRAP_ChannelHandle)pParam1;
    BRAP_DSPCHN_SampleRateChangeInfo	sSampleRateChangeInfo;
    BRAP_DSPCHN_Handle  hDspCh = NULL;
    BRAP_DSPCHN_P_FwSampleinfo  *psFwSampleInfo = (BRAP_DSPCHN_P_FwSampleinfo  *)pParam2;    
    BRAP_FWIF_P_FwTaskHandle   hTask = (BRAP_FWIF_P_FwTaskHandle)pParam3;
    bool    bPresent =false;
    bool    bSRCRequired=false;
#if 0    
    BRAP_P_OpPortPrevMuteStatus sOpPortPrevMuteStatus;   
#endif
    unsigned int i=0,j=0,k=0;

#if BRAP_P_EQUALIZER
    /* Variables to update the sampling rate of Equalizer */        
    BRAP_P_DstDetails       sPvtDstDetails;
    unsigned int            uiAssocId = 0;
    BERR_Code               ret = BERR_SUCCESS;
#endif                        

#if (BRAP_48KHZ_RINGBUFFER_DESTINATION == 1)
    unsigned int    uiDst = 0;
#endif

    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(psFwSampleInfo);
    BDBG_ASSERT(hTask);    

    hDspCh = BRAP_DSPCHN_P_GetDspChHandle_isr(&hRapCh, true);
    if(NULL == hDspCh)
    {
        BDBG_ERR(("BRAP_P_DSP2HostAsyn_isr: hDspCh can't be NULL at this point"));
        BDBG_ASSERT(hDspCh);
        return;
    }

    if((hRapCh->eSamplingRate != BAVC_AudioSamplingRate_eUnknown)
        &&(hRapCh->bGateOpened == true))
    {
        bSRCRequired = true;
    }
    if(hRapCh->bEnableFixedSampleRateOutput == true)    
    {
        hRapCh->uiInputSamplingRate = psFwSampleInfo->sBaseSamplingRate.ui32SamplingRate;
        hRapCh->eSamplingRate = BAVC_AudioSamplingRate_e48k;
    }
    else
    {
    BRAP_P_ConvertSrToEnum(psFwSampleInfo->sBaseSamplingRate.ui32SamplingRate,&(hRapCh->eSamplingRate));
    }

/*Trigger DestinationSample rate change interrupt for all Destination attached to this hRapch*/
    for(i= 0; i<BRAP_P_MAX_DST_PER_RAPCH;i++)
    {
        if(hRapCh->pDstDetails[i] != NULL)
        {
            for(j=0; j<BRAP_MAX_ASSOCIATED_GROUPS; j++)
            {
                if(hRapCh->uiAssociationId[j] == BRAP_INVALID_VALUE)
                    continue;
                for(k=0; k<BRAP_P_MAX_DST_PER_RAPCH; k++)
                {
                    if(hRapCh->pDstDetails[i] == &(hRapCh->hRap->sAssociatedCh[hRapCh->uiAssociationId[j]].sDstDetails[k].sExtDstDetails))
                    {                                
                        if(hRapCh->hRap->sAssociatedCh[hRapCh->uiAssociationId[j]].sDstDetails[k].bSampleRateChangeCallbackEnabled == true)
                        {
                            BRAP_P_DestinationSampleRateChange_isr(
                                (void *)(&(hRapCh->hRap->sAssociatedCh[hRapCh->uiAssociationId[j]].sDstDetails[k]))
                                ,(unsigned int)hRapCh->eSamplingRate);
                        }
                        break;
                    }
                }
            }
        }
    }

    BDBG_MSG(("BRAP_P_DSP2HostAsyn_isr: Sample rate change interrupt occured for RAP channel %d",
    hRapCh->uiChannelNo));
    BDBG_MSG(("BRAP_P_DSP2HostAsyn_isr: New hDspCh->eSR = %d", 
                        hRapCh->eSamplingRate));

    /*If this is not the first Sample rate interrupt then Program the SRC accordingly */
    if(bSRCRequired == true)
    {
#if 0    
        /*Mute and Program new Sampling rate to all the O/P connected to association of the RapCh*/
        BRAP_P_MuteAndSetSRChannelOutputOnSr_isr(hRapCh,&sOpPortPrevMuteStatus,hRapCh->eSamplingRate);
#endif
#if (BRAP_7550_FAMILY != 1)                

#if (BRAP_48KHZ_RINGBUFFER_DESTINATION != 1)
        /*Program SRC used by PB/CAP channels in the association of the RapCh */
        if(hRapCh->eChannelType == BRAP_ChannelType_eDecode)
        {
            if(hRapCh->bEnableFixedSampleRateOutput == true)    
            {    
                BRAP_P_ConfigureAssociatedChannelsSrc_isr(hRapCh,48000);
            }
            else
            {    
                BRAP_P_ConfigureAssociatedChannelsSrc_isr(hRapCh,psFwSampleInfo->sBaseSamplingRate.ui32SamplingRate);
            }  
        }
#else        
        for(j = 0; j < BRAP_MAX_ASSOCIATED_GROUPS; j++)
        {
            if(BRAP_INVALID_VALUE == hRapCh->uiAssociationId[j])
            {
                continue;
            }
            else
            {
                uiAssocId = hRapCh->uiAssociationId[j];
            }

            for(uiDst = 0; uiDst < BRAP_P_MAX_DST_PER_RAPCH; uiDst++)
            {
                if(BRAP_AudioDst_eRingBuffer == 
                    hRapCh->hRap->sAssociatedCh[uiAssocId].sDstDetails[uiDst].sExtDstDetails.eAudioDst)
                {
                    /* SRC type of all the channels of this assoc should be such that its output is at 48khz */
                    BRAP_P_ProgramSRCOfRingBufferDstn_isr(hRapCh,psFwSampleInfo->sBaseSamplingRate.ui32SamplingRate);
                    break;
                }            
                else if((BRAP_AudioDst_eOutputPort == 
                         hRapCh->hRap->sAssociatedCh[uiAssocId].sDstDetails[uiDst].sExtDstDetails.eAudioDst) &&
                        ((hRapCh->hRap->sAssociatedCh[uiAssocId].hPBCh[0] != NULL) ||
                         (hRapCh->hRap->sAssociatedCh[uiAssocId].hCapCh[0] != NULL))
                       ) /* To make sure only SRC of Pb/Cap Channel feeding output port is handled:
                            Mixing on output port */
                {
                    /* SRC type of Pb/Cap Channel of this assoc should be such that 
                       its output is at decode channel's sample rate */
                    BRAP_P_ConfigureAssociatedChannelsSrc_isr(hRapCh,psFwSampleInfo->sBaseSamplingRate.ui32SamplingRate);
                    break;                    
                }
            }
        }
#endif

#endif
    }

    if((BRAP_ChannelType_eDecode == hRapCh->eChannelType)&&
    (BRAP_ChannelSubType_eNone == hRapCh->eChannelSubType))
    {
        BDBG_MSG(("SamplingRate > %x", psFwSampleInfo->sBaseSamplingRate.ui32SamplingRate));
        if(psFwSampleInfo->sBaseSamplingRate.ui32SamplingRate >= 96000)
        {
            /* This case might not occur for STB/DTV, but if it occurs, code for
            doing the following needs to be added */
            BDBG_MSG(("NEED TO INCREASE PRIORITY OF SRC, SRCCH & DSTCH as needed!!"));
        }
    }
#if BRAP_P_EQUALIZER
    if(bSRCRequired == true)
    {
        for(i=0; i < BRAP_P_MAX_DST_PER_RAPCH; i++)
        {
            if(NULL == hRapCh->pDstDetails[i])
                continue;
            
            BDBG_MSG (("For SRC-EQ in rap_int :: hRapCh->pDstDetails[i]=%x",hRapCh->pDstDetails[i]));
            
            /* hEqualizer is present in Private structure. Get it */
            ret = BRAP_P_GetPvtDstDetails_isr(hRapCh,hRapCh->pDstDetails[i],&sPvtDstDetails,&uiAssocId);
            if(BERR_SUCCESS!=ret)
            {
                continue;
            }

            if(sPvtDstDetails.hEqualizer != NULL)
            {
                /* Equalizer found in the channel. 
                Update its sampling frequency  and call setEqualizerSettings */
                sPvtDstDetails.hEqualizer->ui32SamplingRate = psFwSampleInfo->sBaseSamplingRate.ui32SamplingRate;
                
                BDBG_MSG (("SamplingRate Changed as %u for Equalizer %x",
                sPvtDstDetails.hEqualizer->ui32SamplingRate,
                sPvtDstDetails.hEqualizer));

                ret = BRAP_P_ApplyCoefficients_isr(sPvtDstDetails.hEqualizer);

                if(BERR_SUCCESS!=ret)
                {
                    BDBG_MSG (("BRAP_P_ApplyCoefficients_isr returned error"));
                    continue;
                }                
            }
        }   
    }
#endif    
    if(bSRCRequired == true)
    {
#if 0    
        /*Unmute new Sampling rate to all the O/P connected to association of the RapCh*/
        BRAP_P_UnMuteChannelOutputOnSr_isr(hRapCh,&sOpPortPrevMuteStatus);
#endif
    }

    /*Prepare the structure for application */
    sSampleRateChangeInfo.eType =     hDspCh->sDspAudioParams.sExtAudioParams.eType;
    BRAP_P_ConvertSrToEnum(psFwSampleInfo->sHDMISamplingRate.ui32SamplingRate,&(sSampleRateChangeInfo.eSamplingRate));

    if((hDspCh->sDspAudioParams.sExtAudioParams.eType == BRAP_DSPCHN_AudioType_eAac) /* AAC and AACADTS has same ID*/
        ||(hDspCh->sDspAudioParams.sExtAudioParams.eType == BRAP_DSPCHN_AudioType_eAacLoas)
        ||(hDspCh->sDspAudioParams.sExtAudioParams.eType == BRAP_DSPCHN_AudioType_eAacSbr)/*AACSBR and AACSBRLOAS has same Id*/
        ||(hDspCh->sDspAudioParams.sExtAudioParams.eType == BRAP_DSPCHN_AudioType_eAacSbrAdts))
    {
        BRAP_P_IsDtsTranscodePresent_isr(hDspCh,BRAP_GET_TASK_INDEX(hTask->uiTaskId),&bPresent);
        if((true == bPresent)
            &&(hRapCh->eSamplingRate != BAVC_AudioSamplingRate_e48k))
        {
            BDBG_WRN(("WARNING!!!! DTS Transcode is only Possible from AAC/AACHE 48Khz streams"));
        }
    }
    else
    {
        BRAP_P_IsDtsTranscodePresent_isr(hDspCh,BRAP_GET_TASK_INDEX(hTask->uiTaskId),&bPresent);
        if((true == bPresent))
        {
            BDBG_WRN(("WARNING!!!! DTS Transcode is only Possible from AAC/AACHE 48Khz streams"));
        }
    }

    /* If sample rate change, call the app callback function for sample rate change */
    if(hRapCh->sAppIntCbInfo[BRAP_Interrupt_eSampleRateChange].pfAppCb) 
    {
        hRapCh->sAppIntCbInfo[BRAP_Interrupt_eSampleRateChange].pfAppCb(
                                    hRapCh->sAppIntCbInfo[BRAP_Interrupt_eSampleRateChange].pParm1,
                                    hRapCh->sAppIntCbInfo[BRAP_Interrupt_eSampleRateChange].iParm2,
                                    &sSampleRateChangeInfo);
    }


   
    return;
}

static void BRAP_P_BitRateChange_isr(void *pParam1, void *pParam2)
{

    BRAP_DSPCHN_BitRateChangeInfo   sBitRateInfo;
    BRAP_ChannelHandle hRapCh = (BRAP_ChannelHandle)pParam1;
    BRAP_DSPCHN_Handle  hDspCh = NULL;    
    BRAP_DSPCHN_P_FwBitRateChangeInfo	*psFwBitRateChange = (BRAP_DSPCHN_P_FwBitRateChangeInfo	*)pParam2;    

    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(psFwBitRateChange);

    hDspCh = BRAP_DSPCHN_P_GetDspChHandle_isr(&hRapCh, true);     
    if(NULL == hDspCh)
    {
        BDBG_ERR(("BRAP_P_DSP2HostAsyn_isr: hDspCh can't be NULL at this point"));
        BDBG_ASSERT(hDspCh);
        return;
    }    

    BDBG_MSG(("BRAP_P_DSP2HostAsyn_isr: Bit rate change interrupt occured for RAP channel %d",
    hRapCh->uiChannelNo));

    /*Prepare the structure for application */
    sBitRateInfo.eType = hDspCh->sDspAudioParams.sExtAudioParams.eType;
    sBitRateInfo.ui32BitRate = psFwBitRateChange->ui32BitRate;
    sBitRateInfo.ui32BitRateIndex = psFwBitRateChange->ui32BitRateIndex;

    /* Call the application Bir rate chnage callback function */
    if(hRapCh->sAppIntCbInfo[BRAP_Interrupt_eBitRateChange].pfAppCb) 
    {
        hRapCh->sAppIntCbInfo[BRAP_Interrupt_eBitRateChange].pfAppCb (
                                    hRapCh->sAppIntCbInfo[BRAP_Interrupt_eBitRateChange].pParm1,
                                    hRapCh->sAppIntCbInfo[BRAP_Interrupt_eBitRateChange].iParm2,
                                    &sBitRateInfo);
    }                
}

static void BRAP_P_ModeChange_isr(void *pParam1, void *pParam2)
{
    BRAP_DSPCHN_ModeChangeInfo  sModeInfo;
    BRAP_ChannelHandle hRapCh = (BRAP_ChannelHandle)pParam1;
    BRAP_DSPCHN_Handle  hDspCh = NULL;    
    BRAP_DSPCHN_P_FwModeChangeInfo	*psFwModeChange = (BRAP_DSPCHN_P_FwModeChangeInfo	*)pParam2;    

    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(psFwModeChange);
    
    hDspCh = BRAP_DSPCHN_P_GetDspChHandle_isr(&hRapCh, true);     
    if(NULL == hDspCh)
    {
        BDBG_ERR(("BRAP_P_DSP2HostAsyn_isr: hDspCh can't be NULL at this point"));
        BDBG_ASSERT(hDspCh);
        return;
    }    

    BDBG_MSG(("BRAP_P_DSP2HostAsyn_isr: Mode change interrupt occured for RAP channel %d",
                        hRapCh->uiChannelNo));

    /*Prepare the structure for application */
    sModeInfo.eType =     hDspCh->sDspAudioParams.sExtAudioParams.eType;
    switch(sModeInfo.eType)
    {
        case BRAP_DSPCHN_AudioType_eMpeg:
            sModeInfo.uModInfo.eChmod =     psFwModeChange->ui32ModeValue;     
            break;
        case BRAP_DSPCHN_AudioType_eAc3:
        case BRAP_DSPCHN_AudioType_eAc3Plus:
        case BRAP_DSPCHN_AudioType_eAc3Lossless:            
            sModeInfo.uModInfo.eAcmod =     psFwModeChange->ui32ModeValue;     
            break;            
        case BRAP_DSPCHN_AudioType_eAac:            
        case BRAP_DSPCHN_AudioType_eAacLoas:            
        case BRAP_DSPCHN_AudioType_eAacSbr:            
        case BRAP_DSPCHN_AudioType_eAacSbrAdts: 
            sModeInfo.uModInfo.eAacAcmod =     psFwModeChange->ui32ModeValue;     
            break;            
        case BRAP_DSPCHN_AudioType_eWmaStd:            
            sModeInfo.uModInfo.eWmaStdAcmod =     psFwModeChange->ui32ModeValue;     
            break;                
        case BRAP_DSPCHN_AudioType_eWmaPro:    
            sModeInfo.uModInfo.eWmaProAcmod =     psFwModeChange->ui32ModeValue;     
            break;    
        case BRAP_DSPCHN_AudioType_eDtsBroadcast:    
            sModeInfo.uModInfo.eDtsBroadcastAcmod =     psFwModeChange->ui32ModeValue;     
            break;
        case BRAP_DSPCHN_AudioType_eDtshd:    
            sModeInfo.uModInfo.eDtsHdAcmod =     psFwModeChange->ui32ModeValue;     
            break;            
        case BRAP_DSPCHN_AudioType_eDra:    
            sModeInfo.uModInfo.eDraAcmod =     psFwModeChange->ui32ModeValue;     
            break;             
        default:
            BDBG_ERR(("Mode Change not supported for Audio type %d",sModeInfo.eType));            
    }

    /* Call the application streaminfo callback function */
    if(hRapCh->sAppIntCbInfo[BRAP_Interrupt_eModeChange].pfAppCb) 
    {
        hRapCh->sAppIntCbInfo[BRAP_Interrupt_eModeChange].pfAppCb (
            hRapCh->sAppIntCbInfo[BRAP_Interrupt_eModeChange].pParm1,
            hRapCh->sAppIntCbInfo[BRAP_Interrupt_eModeChange].iParm2,
            &sModeInfo
            );
    }
    hRapCh->uiModeValue = psFwModeChange->ui32ModeValue;
    return;
}

static void BRAP_P_CrcError_isr(void *pParam1, void *pParam2)
{
    BRAP_ChannelHandle hRapCh = (BRAP_ChannelHandle)pParam1;

    BDBG_ASSERT(hRapCh);
    BSTD_UNUSED(pParam2);
    
    BDBG_MSG(("BRAP_P_DSP2HostAsyn_isr: CRC error interrupt occured for RAP channel %d",
                            hRapCh->uiChannelNo));

    /*Increment Error Count*/
    hRapCh->uiTotalErrorCount++;


    /* Call the application Crc Error callback function */
    if(hRapCh->sAppIntCbInfo[BRAP_Interrupt_eCrcError].pfAppCb) 
    {
        hRapCh->sAppIntCbInfo[BRAP_Interrupt_eCrcError].pfAppCb (
            hRapCh->sAppIntCbInfo[BRAP_Interrupt_eCrcError].pParm1,
            hRapCh->sAppIntCbInfo[BRAP_Interrupt_eCrcError].iParm2,
            NULL
            );
    }
    return;
}
static void BRAP_P_FirstPtsReady_isr(void *pParam1, void *pParam2)
{
    BRAP_DSPCHN_PtsInfo	sPtsInfo;
    BRAP_ChannelHandle hRapCh = (BRAP_ChannelHandle)pParam1;   
    BRAP_DSPCHN_PtsInfo	*psPtsInfo = (BRAP_DSPCHN_PtsInfo	*)pParam2;    

    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(psPtsInfo);    

    BDBG_MSG(("BRAP_P_DSP2HostAsyn_isr: First PTS Ready interrupt occured for RAP channel %d",
                            hRapCh->uiChannelNo));

    sPtsInfo = *psPtsInfo;


    /* Call the application first pts ready callback function */
    if(hRapCh->sAppIntCbInfo[BRAP_Interrupt_eFirstPtsReady].pfAppCb) 
    {
        hRapCh->sAppIntCbInfo[BRAP_Interrupt_eFirstPtsReady].pfAppCb (
                    hRapCh->sAppIntCbInfo[BRAP_Interrupt_eFirstPtsReady].pParm1,
                    hRapCh->sAppIntCbInfo[BRAP_Interrupt_eFirstPtsReady].iParm2,
                    &sPtsInfo
                    );
    }
    return;
}

static void BRAP_P_PtsError_isr(void *pParam1, void *pParam2)
{
    BRAP_DSPCHN_PtsInfo	sPtsInfo;
    BRAP_ChannelHandle hRapCh = (BRAP_ChannelHandle)pParam1;   
    BRAP_DSPCHN_PtsInfo	*psPtsInfo = (BRAP_DSPCHN_PtsInfo	*)pParam2;    

    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(psPtsInfo);    

    BDBG_MSG(("BRAP_P_DSP2HostAsyn_isr: PTS error interrupt occured (i.e. ITB entry with the PTS value is in error) for RAP channel %d", 
                            hRapCh->uiChannelNo));

    sPtsInfo = *psPtsInfo;


/*Increment Error Count*/
    hRapCh->uiTotalErrorCount++;

    /* Call the application Pts Error ready callback function */
    if(hRapCh->sAppIntCbInfo[BRAP_Interrupt_ePtsError].pfAppCb) 
    {
        hRapCh->sAppIntCbInfo[BRAP_Interrupt_ePtsError].pfAppCb (
                    hRapCh->sAppIntCbInfo[BRAP_Interrupt_ePtsError].pParm1,
                    hRapCh->sAppIntCbInfo[BRAP_Interrupt_ePtsError].iParm2,
                    &sPtsInfo
                    );
    }
    return;
}

static void BRAP_P_StartPtsReached_isr(void *pParam1, void *pParam2)
{
    BRAP_ChannelHandle hRapCh = (BRAP_ChannelHandle)pParam1;   

    BDBG_ASSERT(hRapCh);
    BSTD_UNUSED(pParam2);
    
    BDBG_MSG(("BRAP_P_DSP2HostAsyn_isr: Start PTS reached interrupt occured for RAP channel %d",
    		    hRapCh->uiChannelNo));

    /* Call the application Start PTS callback function */
    if(hRapCh->sAppIntCbInfo[BRAP_Interrupt_eStartPtsReached].pfAppCb) {
    	hRapCh->sAppIntCbInfo[BRAP_Interrupt_eStartPtsReached].pfAppCb (
    		hRapCh->sAppIntCbInfo[BRAP_Interrupt_eStartPtsReached].pParm1,
    		hRapCh->sAppIntCbInfo[BRAP_Interrupt_eStartPtsReached].iParm2,
    		NULL);
    }        	
}

static void BRAP_P_StopPtsReached_isr(void *pParam1, void *pParam2)
{
    BRAP_ChannelHandle hRapCh = (BRAP_ChannelHandle)pParam1;   

    BDBG_ASSERT(hRapCh);
    BSTD_UNUSED(pParam2);
    
    BDBG_MSG(("BRAP_P_DSP2HostAsyn_isr: Stop PTS reached interrupt occured for RAP channel %d",
    		    hRapCh->uiChannelNo));

    /* Call the application Stop PTS callback function */
    if(hRapCh->sAppIntCbInfo[BRAP_Interrupt_eStopPtsReached].pfAppCb) {
    	hRapCh->sAppIntCbInfo[BRAP_Interrupt_eStopPtsReached].pfAppCb (
    		hRapCh->sAppIntCbInfo[BRAP_Interrupt_eStopPtsReached].pParm1,
    		hRapCh->sAppIntCbInfo[BRAP_Interrupt_eStopPtsReached].iParm2,
    		NULL);
    }        	
}

static void BRAP_P_AstmPass_isr(void *pParam1, void *pParam2)
{
    BRAP_ChannelHandle hRapCh = (BRAP_ChannelHandle)pParam1;   

    BDBG_ASSERT(hRapCh);
    BSTD_UNUSED(pParam2);
    
    BDBG_MSG(("BRAP_P_DSP2HostAsyn_isr: ASTM TSM Pass reached interrupt occured for RAP channel %d",
    		    hRapCh->uiChannelNo));

    /* Call the application Astm Tsm pass callback function */
    if(hRapCh->sAppIntCbInfo[BRAP_Interrupt_eAstmTsmPass].pfAppCb) {
    	hRapCh->sAppIntCbInfo[BRAP_Interrupt_eAstmTsmPass].pfAppCb (
    		hRapCh->sAppIntCbInfo[BRAP_Interrupt_eAstmTsmPass].pParm1,
    		hRapCh->sAppIntCbInfo[BRAP_Interrupt_eAstmTsmPass].iParm2,
    		NULL);
    }        	
}

static void BRAP_P_TsmFail_isr(void *pParam1, void *pParam2)
{
    BRAP_DSPCHN_PtsInfo	sPtsInfo;
    BRAP_ChannelHandle hRapCh = (BRAP_ChannelHandle)pParam1;   
    BRAP_DSPCHN_PtsInfo	*psPtsInfo = (BRAP_DSPCHN_PtsInfo	*)pParam2;    

    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(psPtsInfo);    

    BDBG_MSG(("BRAP_P_DSP2HostAsyn_isr: TSM FAIL Event interrupt occured for RAP channel %d", 
                            hRapCh->uiChannelNo));

    /*Prepare the structure for application */
    sPtsInfo = *psPtsInfo;

/*Increment Error Count*/
    hRapCh->uiTotalErrorCount++;

    /* Call the application Tsm fail ready callback function */
    if(hRapCh->sAppIntCbInfo[BRAP_Interrupt_eTsmFail].pfAppCb) 
    {
        hRapCh->sAppIntCbInfo[BRAP_Interrupt_eTsmFail].pfAppCb (
                    hRapCh->sAppIntCbInfo[BRAP_Interrupt_eTsmFail].pParm1,
                    hRapCh->sAppIntCbInfo[BRAP_Interrupt_eTsmFail].iParm2,
                    &sPtsInfo
                    );
    }
    return;
}
#if (BRAP_7405_FAMILY == 1)
void BRAP_P_RampEnableTimer_isr(
        void * pParm1, /* [in] Raptor channel handle */
        int    iParm2  /* [in] RBUF index */
)
{
	BRAP_ChannelHandle hRapCh;
#if (BRAP_7550_FAMILY != 1)   
    unsigned int    uiBlkId=0,uiBlkOffset=0;
#endif
        unsigned int    ui32RegVal=0;
	BDBG_ENTER (BRAP_P_RampEnableTimer_isr);
	BDBG_ASSERT (pParm1);
	BSTD_UNUSED ( iParm2 );
	hRapCh = (BRAP_ChannelHandle) pParm1;
    
        ui32RegVal = BRAP_Read32 (hRapCh->hRegister,BCHP_AUD_FMM_DP_CTRL0_VOLUME_RAMP_STEP);  

        ui32RegVal &= ~(BCHP_MASK (    
                        AUD_FMM_DP_CTRL0_VOLUME_RAMP_STEP, 
                        VOLUME_RAMP_STEP));    

        ui32RegVal |= (BCHP_FIELD_DATA (    
                            AUD_FMM_DP_CTRL0_VOLUME_RAMP_STEP, 
                            VOLUME_RAMP_STEP, 
                        (hRapCh->hRap->uiMixerRampStepSize)));

    BDBG_MSG(("Setting VOLUME_RAMP_STEP to 0x%x",ui32RegVal));
    BRAP_Write32 (hRapCh->hRegister, BCHP_AUD_FMM_DP_CTRL0_VOLUME_RAMP_STEP, ui32RegVal);    
    
#if (BRAP_7550_FAMILY != 1)   
    for (uiBlkId = 0; uiBlkId < BRAP_RM_P_MAX_SRC_BLCK; uiBlkId++)
    {
        uiBlkOffset = 0;
#if ( BRAP_RM_P_MAX_SRC_BLCK > 1 )    
        uiBlkOffset = (BCHP_AUD_FMM_SRC_CTRL1_STRM_ENA - BCHP_AUD_FMM_SRC_CTRL0_STRM_ENA) * uiBlkId;
#endif
        BRAP_Write32(hRapCh->hRegister, BCHP_AUD_FMM_SRC_CTRL0_RAMP_STEP + uiBlkOffset, (hRapCh->hRap->uiSrcRampStepSize));  
    }
#endif

    if(hRapCh->hTimer != NULL)
    {
        BTMR_StopTimer_isr( hRapCh->hTimer );
    }



    return;

}
#endif
static void BRAP_P_RampEnable_isr(void *pParam1, void *pParam2)
{

    BRAP_ChannelHandle hRapCh = (BRAP_ChannelHandle)pParam1;   
    BRAP_DSPCHN_RampEnableInfo	*psRampEnableInfo = (BRAP_DSPCHN_RampEnableInfo	*)pParam2;    
#if 0    
    BRAP_P_OpPortPrevMuteStatus sOpPortPrevMuteStatus;   
#endif
    unsigned int ui32SamplingRate;

#if BRAP_P_EQUALIZER
    /* Variables to update the sampling rate of Equalizer */        
    int i=0;
    BRAP_P_DstDetails       sPvtDstDetails;
    unsigned int            uiAssocId = 0;
    BERR_Code               ret = BERR_SUCCESS;
#endif    

#if (BRAP_48KHZ_RINGBUFFER_DESTINATION == 1)
    unsigned int            j = 0, uiDst = 0;
#endif

    BDBG_ENTER(BRAP_P_RampEnable_isr);

    BDBG_ASSERT(hRapCh);
    BDBG_ASSERT(psRampEnableInfo);    

    BDBG_MSG(("BRAP_P_DSP2HostAsyn_isr: RAMp Enable Event interrupt occured for RAP channel %d, psRampEnableInfo->ui32TimeDelay =%d", 
                            hRapCh->uiChannelNo,psRampEnableInfo->ui32TimeDelay));

#if (BRAP_7405_FAMILY == 1)
    if(hRapCh->hTimer != NULL)
    {
        BTMR_StartTimer_isr(hRapCh->hTimer, psRampEnableInfo->ui32TimeDelay*1000);
    }
#endif
    hRapCh->bGateOpened = true;

     if(hRapCh->bEnableFixedSampleRateOutput == true)    
     {
         /*BRAP_P_ConvertSR(hRapCh->eSamplingRate,&(hRapCh->eInputSamplingRate));*/
         hRapCh->eSamplingRate = BAVC_AudioSamplingRate_e48k;
         ui32SamplingRate = 48000;    
     }
     else
     {
     BRAP_P_ConvertSR(hRapCh->eSamplingRate,&ui32SamplingRate);
     }
#if 0     
    /*Mute and Program new Sampling rate to all the O/P connected to association of the RapCh*/
    BRAP_P_MuteAndSetSRChannelOutputOnSr_isr(hRapCh,&sOpPortPrevMuteStatus,hRapCh->eSamplingRate);
#endif    
#if (BRAP_7550_FAMILY != 1)

#if (BRAP_48KHZ_RINGBUFFER_DESTINATION != 1)
    /*Program SRC used by PB/CAP channels in the association of the RapCh */
    if(hRapCh->eChannelType == BRAP_ChannelType_eDecode)
    {
        BRAP_P_ConfigureAssociatedChannelsSrc_isr(hRapCh,ui32SamplingRate);
    }
#else        
    for(j = 0; j < BRAP_MAX_ASSOCIATED_GROUPS; j++)
    {
        if(BRAP_INVALID_VALUE == hRapCh->uiAssociationId[j])
        {
            continue;
        }
        else
        {
            uiAssocId = hRapCh->uiAssociationId[j];
        }

        for(uiDst = 0; uiDst < BRAP_P_MAX_DST_PER_RAPCH; uiDst++)
        {
            if(BRAP_AudioDst_eRingBuffer == 
                hRapCh->hRap->sAssociatedCh[uiAssocId].sDstDetails[uiDst].sExtDstDetails.eAudioDst)
            {
                /* SRC type of all channels of this assoc should be such that the outputs are at 48khz */
                BRAP_P_ProgramSRCOfRingBufferDstn_isr(hRapCh,ui32SamplingRate);
                break;
            }            
            else if((BRAP_AudioDst_eOutputPort == 
                     hRapCh->hRap->sAssociatedCh[uiAssocId].sDstDetails[uiDst].sExtDstDetails.eAudioDst) &&
                    ((hRapCh->hRap->sAssociatedCh[uiAssocId].hPBCh[0] != NULL) ||
                     (hRapCh->hRap->sAssociatedCh[uiAssocId].hCapCh[0] != NULL))
                   ) /* To make sure only SRC of Pb/Cap Channel feeding output port is handled */
            {
                /* SRC type of Pb/Cap Channel of this assoc should be such that 
                   its output is at decode channel's sample rate */
                BRAP_P_ConfigureAssociatedChannelsSrc_isr(hRapCh,ui32SamplingRate);
                break;
            }
        }
    }
#endif

#endif

#if BRAP_P_EQUALIZER
        for(i=0; i < BRAP_P_MAX_DST_PER_RAPCH; i++)
        {
            if(NULL == hRapCh->pDstDetails[i])
                continue;
            
            BDBG_MSG (("For SRC-EQ in rap_int :: hRapCh->pDstDetails[i]=%x",hRapCh->pDstDetails[i]));
            
            /* hEqualizer is present in Private structure. Get it */
            ret = BRAP_P_GetPvtDstDetails_isr(hRapCh,hRapCh->pDstDetails[i],&sPvtDstDetails,&uiAssocId);
            if(BERR_SUCCESS!=ret)
            {
                continue;
            }

            if(sPvtDstDetails.hEqualizer != NULL)
            {
                /* Equalizer found in the channel. 
                Update its sampling frequency  and call setEqualizerSettings */
                sPvtDstDetails.hEqualizer->ui32SamplingRate = ui32SamplingRate;
                
                BDBG_MSG (("SamplingRate Changed as %u for Equalizer %x",
                sPvtDstDetails.hEqualizer->ui32SamplingRate,
                sPvtDstDetails.hEqualizer));

                ret = BRAP_P_ApplyCoefficients_isr(sPvtDstDetails.hEqualizer);

                if(BERR_SUCCESS!=ret)
                {
                    BDBG_MSG (("BRAP_P_ApplyCoefficients_isr returned error"));
                    continue;
                }                
            }
        }   
#endif    
    
#if 0
/*Unmute new Sampling rate to all the O/P connected to association of the RapCh*/
    BRAP_P_UnMuteChannelOutputOnSr_isr(hRapCh,&sOpPortPrevMuteStatus);
#endif
    /*Prepare the structure for application */
	
    BDBG_LEAVE(BRAP_P_RampEnable_isr);

}

static void BRAP_P_StreamInfoAvailable_isr(void *pParam1, void *pParam2)
{
    BRAP_ChannelHandle hRapCh = (BRAP_ChannelHandle)pParam1;   

    BDBG_ASSERT(hRapCh);
    BSTD_UNUSED(pParam2);

    BRAP_P_MaskInterrupt_isr(hRapCh,BRAP_Interrupt_eStreamInfoAvailable,BRAP_FWIF_P_ResponseType_eNone);

    
    BDBG_MSG(("BRAP_P_DSP2HostAsyn_isr: Stream Info Available interrupt occured for RAP channel %d",
    		    hRapCh->uiChannelNo));


    /* Call the application Stream Info Available  callback function */
    if(hRapCh->sAppIntCbInfo[BRAP_Interrupt_eStreamInfoAvailable].pfAppCb) {
    	hRapCh->sAppIntCbInfo[BRAP_Interrupt_eStreamInfoAvailable].pfAppCb (
    		hRapCh->sAppIntCbInfo[BRAP_Interrupt_eStreamInfoAvailable].pParm1,
    		hRapCh->sAppIntCbInfo[BRAP_Interrupt_eStreamInfoAvailable].iParm2,
    		NULL);
    }        	
}

static void BRAP_P_UnlicensedAlgo_isr(void *pParam1, void *pParam2)
{
    BRAP_ChannelHandle hRapCh = (BRAP_ChannelHandle)pParam1;   
    BRAP_FWIF_P_UnsupportedAlgoInfo 	*psUnlicensedAlgoInfo;   

    BDBG_ASSERT(hRapCh);
    BSTD_UNUSED(psUnlicensedAlgoInfo);

    psUnlicensedAlgoInfo = (BRAP_FWIF_P_UnsupportedAlgoInfo	*)pParam2;   
    
    BDBG_ERR(("BRAP_P_DSP2HostAsyn_isr: Unlicensed Algo (%s) interrupt occured for RAP channel %d"
                   ,BRAP_P_PrintDecEncPpTypeName(psUnlicensedAlgoInfo->ui32AudioAlgorithm),
    		    hRapCh->uiChannelNo));


    /* Call the application Stream Info Available  callback function */
    if(hRapCh->sAppIntCbInfo[BRAP_Interrupt_eUnlicensedAlgo].pfAppCb) {
    	hRapCh->sAppIntCbInfo[BRAP_Interrupt_eUnlicensedAlgo].pfAppCb (
    		hRapCh->sAppIntCbInfo[BRAP_Interrupt_eUnlicensedAlgo].pParm1,
    		hRapCh->sAppIntCbInfo[BRAP_Interrupt_eUnlicensedAlgo].iParm2,
    		NULL);
    }        	
}



static void BRAP_P_CdbOverflow_isr(void *pParam1, void *pParam2)
{
    BRAP_ChannelHandle hRapCh = (BRAP_ChannelHandle)pParam1;   

    BDBG_ASSERT(hRapCh);
    BSTD_UNUSED(pParam2);
    
    BDBG_MSG(("BRAP_P_DSP2HostAsyn_isr: CDB Overflow interrupt occured for RAP channel %d",
    		    hRapCh->uiChannelNo));


    /* Call the application Stream Info Available  callback function */
    if(hRapCh->sAppIntCbInfo[BRAP_Interrupt_eCdbItbOverflow].pfAppCb) {
    	hRapCh->sAppIntCbInfo[BRAP_Interrupt_eCdbItbOverflow].pfAppCb (
    		hRapCh->sAppIntCbInfo[BRAP_Interrupt_eCdbItbOverflow].pParm1,
    		hRapCh->sAppIntCbInfo[BRAP_Interrupt_eCdbItbOverflow].iParm2,
    		NULL);
    }        	
}


static void BRAP_P_CdbUnderflow_isr(void *pParam1, void *pParam2)
{
    BRAP_ChannelHandle hRapCh = (BRAP_ChannelHandle)pParam1;   

    BDBG_ASSERT(hRapCh);
    BSTD_UNUSED(pParam2);
    
    BDBG_MSG(("BRAP_P_DSP2HostAsyn_isr: CDB underflow interrupt occured for RAP channel %d",
    		    hRapCh->uiChannelNo));


    /* Call the application Stream Info Available  callback function */
    if(hRapCh->sAppIntCbInfo[BRAP_Interrupt_eCdbItbUnderflow].pfAppCb) {
    	hRapCh->sAppIntCbInfo[BRAP_Interrupt_eCdbItbUnderflow].pfAppCb (
    		hRapCh->sAppIntCbInfo[BRAP_Interrupt_eCdbItbUnderflow].pParm1,
    		hRapCh->sAppIntCbInfo[BRAP_Interrupt_eCdbItbUnderflow].iParm2,
    		NULL);
    }        	
}

/***************************************************************************
Description:
	This API will be called when any asynchronous interrupt will be raised by 
	FW for any task.
Returns:
    void
See Also:    
    BRAP_P_DSP2HostSyn_isr, BRAP_P_DSP2HostAsyn_isr, BRAP_P_InterruptInstall
***************************************************************************/
static void BRAP_P_DSP2HostAsyn_isr(
        void    *pParm1,
        int     iParm2
)
{
    /*
        Using param1 we get the channel handle. 
        than on tat particular channel handle we will read the SI00 register to 
        decided for which task the interrupt has been raised.
        We will call the isr function which are attached with the particulr interrupt.        
        example 
    */
    BRAP_Handle  hRap;
    uint32_t    ui32DspId = 0, ui32TaskId = 0, ui32IntStatus = 0;
    uint32_t    ui32MaskStatus = 0, ui32Counter = 0, ui32Offset = 0;
    BRAP_FWIF_P_FwTaskHandle   hTask;

    BRAP_FWIF_P_AsynEventMsg  *pEventMsg = NULL;

    
    BDBG_ENTER(BRAP_P_DSP2HostAsyn_isr);
    ui32DspId = iParm2;
    hRap = (BRAP_Handle) pParm1;
    BDBG_ASSERT(hRap);
	ui32Offset = 0;    
    ui32MaskStatus = BRAP_Read32_isr(hRap->hRegister, 
                        BCHP_AUD_DSP_ESR_SO10_MASK_STATUS+ui32Offset);
	BDBG_MSG (("BCHP_AUD_DSP_ESR_SO10_MASK_STATUS =0x%x", ui32MaskStatus));

    ui32IntStatus = BRAP_Read32_isr(hRap->hRegister, 
                        BCHP_AUD_DSP_ESR_SO10_INT_STATUS+ui32Offset);
	BDBG_MSG (("BCHP_AUD_DSP_ESR_SO10_INT_STATUS =0x%x", ui32IntStatus));

	BRAP_Write32_isr (hRap->hRegister, BCHP_AUD_DSP_ESR_SO10_INT_CLEAR+
                        ui32Offset,ui32IntStatus);

    for(ui32Counter = 0; ui32Counter < BRAP_RM_P_MAX_FW_TASK_PER_DSP; ++ui32Counter )
    {
        hTask = hRap->hFwTask[ui32DspId][ui32Counter];
        if(hTask == NULL)
        {
            BDBG_MSG(("BRAP_P_DSP2HostAsyn_isr: Task ID %d is NULL for DSP = %d ",ui32Counter,
                        ui32DspId));
            continue;
        }
        ui32TaskId = hTask->uiTaskId;     
        pEventMsg = hTask->pAsyncMsgMemory;

        if((hTask->uiTaskId == BRAP_FWIF_P_INVALID_TSK_ID))
        {
            continue;
        }

        if(((ui32IntStatus >> ui32TaskId)& 0x1) == 1)
        {
            BRAP_ChannelHandle  hRapCh = NULL;
            BRAP_DSPCHN_Handle  hDspCh = NULL;
            BRAP_AssociatedChannelHandle    hAssociatedChan = NULL;
            void                *pHandle = NULL;
            unsigned int uiNumMsgs = 0, i = 0;
            
            if(hTask->bChSpecificTask == true)
            {
            hRapCh = hTask->uHandle.hRapCh;                    
                pHandle = &hRapCh;
                
                hDspCh = BRAP_DSPCHN_P_GetDspChHandle_isr(pHandle, true);     
            if(NULL == hDspCh)
            {
                BDBG_ERR(("BRAP_P_DSP2HostAsyn_isr: hDspCh can't be NULL at this point"));
                BDBG_ASSERT(hDspCh);
                return;
            }
            }
            else
            {
                hAssociatedChan = hTask->uHandle.hAssociation;
                pHandle = &hAssociatedChan;
                    
                hDspCh = BRAP_DSPCHN_P_GetDspChHandle_isr(pHandle, false);     
                if(NULL == hDspCh)
                {
                    BDBG_ERR(("BRAP_P_DSP2HostAsyn_isr: hDspCh can't be NULL at this point"));
                    BDBG_ASSERT(hDspCh);
                    return;
                }            
            }

            /* Read the message in sEventMsg */
            BRAP_FWIF_P_GetAsyncMsg_isr(hTask->hAsyncMsgQueue,pEventMsg, &uiNumMsgs);
            
            /*TODO: Handle interrupts for Assocaiation specifc task */
            if(hTask->bChSpecificTask == true)
            {            
            /* Now check which response came from FW and work according to it*/
            for(i = 0; i < uiNumMsgs; i++)
            {
                if(pEventMsg[i].sMsgHeader.ui32EventID == BRAP_FWIF_P_FRAME_SYNC_LOCK_LOST_EVENT_ID)
                {
                    BRAP_P_DecoderUnlock_isr((void *)hRapCh,NULL);
                }
                else if(pEventMsg[i].sMsgHeader.ui32EventID == BRAP_FWIF_P_FRAME_SYNC_LOCK_EVENT_ID)
                {
                    BRAP_P_DecoderLock_isr((void *)hRapCh,NULL);
                }
                else if(pEventMsg[i].sMsgHeader.ui32EventID == BRAP_FWIF_P_SAMPLING_RATE_CHANGE_EVENT_ID)
                {
                    BRAP_P_SampleRateChange_isr((void *)hRapCh,&(pEventMsg[i].uFWMessageInfo.sFwSampleInfo),(void *)hTask);
                }
                else if (pEventMsg[i].sMsgHeader.ui32EventID == BRAP_FWIF_P_BIT_RATE_CHANGE_EVENT_ID)
                {
                    BRAP_P_BitRateChange_isr((void *)hRapCh,&(pEventMsg[i].uFWMessageInfo.sFwBitRateChange));
                }
                else if (pEventMsg[i].sMsgHeader.ui32EventID == BRAP_FWIF_P_AUDIO_MODE_CHANGE_EVENT_ID)
                {
                    BRAP_P_ModeChange_isr((void *)hRapCh,&(pEventMsg[i].uFWMessageInfo.sFwModeChange));            	
                }
                else if (pEventMsg[i].sMsgHeader.ui32EventID == BRAP_FWIF_P_CRC_ERROR_EVENT_ID)
                {
                    BRAP_P_CrcError_isr((void *)hRapCh,NULL);                        	
                }
                else if (pEventMsg[i].sMsgHeader.ui32EventID == BRAP_FWIF_P_FIRST_PTS_RECEIVED_FROM_ITB_EVENT_ID)
                {
                    BRAP_P_FirstPtsReady_isr((void *)hRapCh,&(pEventMsg[i].uFWMessageInfo.sPtsInfo));             	
                }
                else if (pEventMsg[i].sMsgHeader.ui32EventID == BRAP_FWIF_P_PTS_ERR_EVENT_ID)
                {
                    BRAP_P_PtsError_isr((void *)hRapCh,&(pEventMsg[i].uFWMessageInfo.sPtsInfo));             	
                }
                else if (pEventMsg[i].sMsgHeader.ui32EventID  == BRAP_FWIF_P_START_PTS_EVENT_ID)
                {
                    BRAP_P_StartPtsReached_isr((void *)hRapCh,NULL);
                }   	
                else if (pEventMsg[i].sMsgHeader.ui32EventID  == BRAP_FWIF_P_STOP_PTS_EVENT_ID)
                {
                    BRAP_P_StopPtsReached_isr((void *)hRapCh,NULL);
                }  
                else if (pEventMsg[i].sMsgHeader.ui32EventID  == BRAP_FWIF_P_ASTMTSM_PASS_EVENT_ID)
                {
                    BRAP_P_AstmPass_isr((void *)hRapCh,NULL);
                } 
                else if (pEventMsg[i].sMsgHeader.ui32EventID  == BRAP_FWIF_P_TSM_FAIL_EVENT_ID)
                {
                    BRAP_P_TsmFail_isr((void *)hRapCh,&(pEventMsg[i].uFWMessageInfo.sPtsInfo));  ;
                }       
                else if (pEventMsg[i].sMsgHeader.ui32EventID  == BRAP_FWIF_P_RAMP_ENABLE_EVENT_ID)
                {
                    BRAP_P_RampEnable_isr((void *)hRapCh,&(pEventMsg[i].uFWMessageInfo.sRampEnableInfo));  ;
                }          
                else if (pEventMsg[i].sMsgHeader.ui32EventID  == BRAP_FWIF_P_STREAM_INFO_AVAIL_EVENT_ID)
                {
                    BRAP_P_StreamInfoAvailable_isr((void *)hRapCh,NULL);
                }          
                else if (pEventMsg[i].sMsgHeader.ui32EventID  == BRAP_FWIF_P_CDB_ITB_OVERFLOW_EVENT_ID)
                {
                    BRAP_P_CdbOverflow_isr((void *)hRapCh,NULL);
                }
                else if (pEventMsg[i].sMsgHeader.ui32EventID  == BRAP_FWIF_P_CDB_ITB_UNDERFLOW_EVENT_ID)
                {
                    BRAP_P_CdbUnderflow_isr((void *)hRapCh,NULL);
                }                
                else if (pEventMsg[i].sMsgHeader.ui32EventID  == BRAP_FWIF_P_UNLICENSED_ALGO_EVENT_ID)
                {
                    BRAP_P_UnlicensedAlgo_isr((void *)hRapCh,&(pEventMsg[i].uFWMessageInfo.sUnsupportedAlgoInfo));
                }                   
#ifdef 	RAP_GFX_SUPPORT
                else if ((pEventMsg[i].sMsgHeader.ui32EventID  == BRAP_FWGFX_GFX_CMD_COMPLETION_EVENT_ID))
                {
                    BRAP_P_GfxOperationCompleted_isr((void *)hRapCh,(unsigned int)pEventMsg[i].uFWMessageInfo.sGfxCommandCompletionInfo.ui32CommandCntr);
                }      
#endif            
#ifdef RAP_SCM_SUPPORT
				else if ((pEventMsg[i].sMsgHeader.ui32EventID  == BRAP_BSP_SCM_RESPONSE_EVENT_ID))
				{
					BRAP_P_ScmResponse_isr((void *)hRapCh,(unsigned int)pEventMsg[i].uFWMessageInfo.sBspScmResponseInfo.eBspScmCodeAuthStatus);
				}	   
#endif
				

            }
            }
        } /* if(((ui32StatusInfo >> ui32TaskId)& 0x1) == 1) */
    } /*    for(ui32DspId =0; ui32DspId < BRAP_RM_P_MAX_DSPS; ++ui32DspId) */
    BDBG_LEAVE(BRAP_P_DSP2HostAsyn_isr);
    return;
}

/***************************************************************************
Description:
	This API will be called when any synchronous interrupt will be raised by 
	FW for any task.
Returns:
    void
See Also:    
    BRAP_P_DSP2HostSyn_isr, BRAP_P_DSP2HostAsyn_isr, BRAP_P_InterruptInstall
***************************************************************************/
static void BRAP_P_DSP2HostSyn_isr(
        void    *pParm1,
        int     iParm2
)
{
    /*
        Using param1 we get the channel handle. Than on that particular channel 
        handle we will read the SI00 register to decided for which task the 
        interrupt has been raised. We will call the isr function which are 
        attached with the particulr interrupt.        
        example 
        BRAP_P_DecoderAck_isr(pParm1, uiTaskId);
    */

    BRAP_Handle  hRap;
    uint32_t    ui32DspId = 0, ui32TaskId = 0, ui32Offset = 0,ui32IntStatus = 0;
    uint32_t    ui32MaskStatus = 0, ui32StatusInfo = 0, ui32Counter = 0; 
    BRAP_FWIF_P_FwTaskHandle   hTask;
    bool        bEvent = false;
    BDBG_ENTER(BRAP_P_DSP2HostSyn_isr);
    BDBG_ASSERT(pParm1);
    hRap = (BRAP_Handle) pParm1;
    ui32DspId = iParm2;
	ui32Offset = 0;

    ui32MaskStatus = BRAP_Read32_isr(hRap->hRegister, 
                            BCHP_AUD_DSP_ESR_SO00_MASK_STATUS+ ui32Offset);
	BDBG_MSG(("BRAP_P_DSP2HostSyn_isr: BCHP_AUD_DSP_ESR_SO00X_MASK_STATUS =0x%x", 
                ui32MaskStatus));

	ui32IntStatus = BRAP_Read32_isr(hRap->hRegister, 
                            BCHP_AUD_DSP_ESR_SO00_INT_STATUS + ui32Offset );
	BDBG_MSG(("BRAP_P_DSP2HostSyn_isr: BCHP_AUD_DSP_ESR_SO00X_INT_STATUS =0x%x", 
                ui32IntStatus));

    ui32IntStatus &=  ~ui32MaskStatus;
    ui32StatusInfo = ui32IntStatus;    
	BRAP_Write32_isr (hRap->hRegister, BCHP_AUD_DSP_ESR_SO00_INT_CLEAR + 
                            ui32Offset, ui32IntStatus);
    for(ui32Counter  = 0; ui32Counter < BRAP_RM_P_MAX_FW_TASK_PER_DSP; ++ui32Counter )
    {
        hTask = hRap->hFwTask[ui32DspId][ui32Counter];
        if(hTask == NULL)
        {
            BDBG_MSG(("BRAP_P_DSP2HostSyn_isr: Task is NULL for DSP = %d ",
                        ui32DspId));
            continue;
        }
        ui32TaskId = hTask->uiTaskId;                        

        if((hTask->uiTaskId == BRAP_FWIF_P_INVALID_TSK_ID))
        {
            continue;
        }
        
        if(((ui32StatusInfo >> ui32TaskId)& 0x1) == 1)
        {
            BDBG_MSG(("Setting Event for TaskId =%d",ui32TaskId));
            BRAP_P_EventSet(hTask->hDsp,(ui32TaskId -BRAP_FWIF_P_TASK_ID_START_OFFSET));
            switch(hTask->uiLastEventType)
            {
                case    BRAP_FWIF_P_START_TASK_COMMAND_ID:
                    hTask->bStopped=false;
                    break;
                case    BRAP_FWIF_P_STOP_TASK_COMMAND_ID:
                    hTask->bStopped=true;
                    break;
                default:
                    break;
            }            
            BDBG_MSG(("After Setting Event for TaskId =%d",ui32TaskId)); 
            bEvent = true;
        }
    } /* for(ui32Counter  = 0; ui32Counter < BRAP_RM_P_MAX_FW_TASK_PER_DSP; ++ui32Counter ) */

    /*This need to be checked why we are getting two calls for this callback
    even when only interrupt register is set once */
    BDBG_LEAVE(BRAP_P_DSP2HostSyn_isr);
    return;
}

/***************************************************************************
Description:
	This API will be called when any acknowledgement come from the FW. This 
	will be used to receive the ping command from FW when any DSP will be 
	started.
Returns:
    void
See Also:    
    BRAP_P_DSP2HostSyn_isr, BRAP_P_DSP2HostAsyn_isr, BRAP_P_AckInstall
***************************************************************************/
static void BRAP_P_DSP2HostAck_isr(
        void    *pParm1,
        int     iParm2
)
{

    BRAP_DSP_Handle hDsp;
	uint32_t    ui32IntStatus = 0, ui32MaskStatus = 0, ui32StatusInfo = 0;
    uint32_t    ui32Offset = 0;
    BDBG_ENTER(BRAP_P_DSP2HostAck_isr);
	BSTD_UNUSED(iParm2);	
    BDBG_ASSERT(pParm1);
    hDsp = (BRAP_DSP_Handle) pParm1;

    ui32Offset = 0;
    ui32MaskStatus = BRAP_Read32_isr(hDsp->hRegister, 
                        BCHP_AUD_DSP_ESR_SO20_MASK_STATUS+ui32Offset);
	BDBG_MSG (("BRAP_P_DSP2HostAsyn_isr: BCHP_AUD_DSP_ESR_SO20X_MASK_STATUS =0x%x", 
                ui32MaskStatus));
    
	ui32IntStatus = BRAP_Read32_isr(hDsp->hRegister, 
                        BCHP_AUD_DSP_ESR_SO20_INT_STATUS +ui32Offset);
	BDBG_MSG (("BRAP_P_DSP2HostAsyn_isr: BCHP_AUD_DSP_ESR_SO20X_INT_STATUS =0x%x", 
                ui32IntStatus));
    ui32StatusInfo = ui32IntStatus;
	ui32IntStatus &=  ~ui32MaskStatus;
	BRAP_Write32_isr (hDsp->hRegister, BCHP_AUD_DSP_ESR_SO20_INT_CLEAR+
                        ui32Offset, ui32IntStatus);

    /* We are using first bit to ping the DSP */
    if((ui32StatusInfo & 0x1) == 1)
    {
        BRAP_P_EventSet(hDsp,(0));
    }
    
    BDBG_LEAVE(BRAP_P_DSP2HostAck_isr);
    return;
}

/***************************************************************************
Description:
	This API will send the command to FW to disbale all the asynchronous events 
	for any task. This api will be called in the start of the task so that we 
	can enable event as per required later.
Returns:
	BERR_SUCCESS - If successful
***************************************************************************/
BERR_Code   BRAP_P_DisableAllEvent(BRAP_FWIF_P_FwTaskHandle hTask)
{   
    BRAP_FWIF_P_Command     sFwCommand;
    BERR_Code               ret=BERR_SUCCESS; 
    BRAP_FWIF_P_Response sRsp;        

        BRAP_Handle hRap;    
        BDBG_ENTER(BRAP_P_DisableAllEvent);
        BDBG_ASSERT(hTask);

    BKNI_Memset((void *)&sRsp,0,sizeof(BRAP_FWIF_P_Response));
    
        if(hTask->bChSpecificTask== true)
        {
            hRap = hTask->uHandle.hRapCh->hRap;
        }
        else
        {
            hRap = hTask->uHandle.hAssociation->hRap;
        }
        
        /* Prepare message structure for FW to write in message queue */
        sFwCommand.sCommandHeader.ui32CommandID = BRAP_FWIF_P_EVENT_NOTIFICATION_COMMAND_ID;
        sFwCommand.sCommandHeader.ui32CommandSizeInBytes = sizeof(BRAP_FWIF_P_Command);
        sFwCommand.sCommandHeader.eResponseType = BRAP_FWIF_P_ResponseType_eAckRequired;
        sFwCommand.sCommandHeader.ui32TaskID = hTask->uiTaskId;
        sFwCommand.sCommandHeader.ui32CommandCounter = hRap->uiCommandCounter++;;
        sFwCommand.uCommand.sEnableDisableEvent.ui32EnableEvent = BRAP_FWIF_P_EventIdMask_eAll;

        hTask->uiLastEventType = sFwCommand.sCommandHeader.ui32CommandID;        
        BRAP_P_EventReset(hTask->hDsp,BRAP_GET_TASK_INDEX(hTask->uiTaskId));
        ret = BRAP_FWIF_P_SendCommand(hTask->hDsp->hCmdQueue, &sFwCommand,hRap,hTask);
        if(BERR_SUCCESS != ret)
        {
            if((hRap->bWatchdogTriggered == false)
            &&(hTask->bStopped == false))
            {
                BDBG_ERR(("BRAP_P_DisableAllEvent: Event notification Command failed!"));
                return BERR_TRACE(ret);
            }
            else
                ret = BERR_SUCCESS;          
        }
        ret = BRAP_P_EventWait(hTask->hDsp,BRAP_DSPCHN_P_EVENT_TIMEOUT_IN_MS,BRAP_GET_TASK_INDEX(hTask->uiTaskId));
        if(BERR_TIMEOUT == ret)
        {
            if((hRap->bWatchdogTriggered == false))
            {
                /* Please note that, If the code reaches at this point then there is a potential Bug in Fw 
                code which needs to be debugged. However Watchdog is being triggered to recover the system*/            
                BDBG_WRN(("BRAP_P_DisableAllEvent: Event failed! Triggering Watchdog"));
#if 0                
                BDBG_ASSERT(0);                
#endif
                BRAP_Write32(hTask->hDsp->hRegister, BCHP_AUD_DSP_INTH0_R5F_SET+ hTask->hDsp->ui32Offset,0x1);
                hRap->bWatchdogTriggered  = true;
#if 0                
                err = BERR_TRACE(err);
                goto error;
#endif                 
                ret = BERR_SUCCESS;              
            }
            else
                ret = BERR_SUCCESS;              
        }
        
        if((hRap->bWatchdogTriggered == false)
        &&(hTask->bStopped == false))
        {
            ret = BRAP_FWIF_P_GetMsg(hTask->hSyncMsgQueue, (void *)&sRsp, BRAP_P_MsgType_eSyn);
        }
        if(BERR_SUCCESS != ret)
        {
            if((hRap->bWatchdogTriggered == false)
                &&(hTask->bStopped == false))
            {
                BDBG_ERR(("BRAP_P_DisableAllEvent: Unable to read ACK!"));
                return BERR_TRACE(ret);
            }
                else
                    ret = BERR_SUCCESS;
        }

        if((sRsp.sCommonAckResponseHeader.eStatus != BERR_SUCCESS)||
            (sRsp.sCommonAckResponseHeader.ui32ResponseID != BRAP_FWIF_P_EVENT_NOTIFICATION_COMMAND_ID)||
            (sRsp.sCommonAckResponseHeader.ui32TaskID != hTask->uiTaskId))
        {
            if((hRap->bWatchdogTriggered == false)
                &&(hTask->bStopped == false))
            {
                BDBG_ERR(("sRsp.sCommonAckResponseHeader.eStatus =%d",sRsp.sCommonAckResponseHeader.eStatus));
                BDBG_ERR(("BRAP_P_DisableAllEvent: Event notification not received successfully!"));
                return BERR_TRACE(sRsp.sCommonAckResponseHeader.eStatus);
            }
            else
                ret = BERR_SUCCESS;        
        }
        
        hTask->hDsp->ui32EventIdMatrix[BRAP_GET_TASK_INDEX(hTask->uiTaskId)] = BRAP_FWIF_P_EventIdMask_eNone;

        BDBG_LEAVE(BRAP_P_DisableAllEvent);
        return ret;
}

/***************************************************************************
Description:
	This API handles the FMM to Host interrupts. This is the top level ISR handler
	for all the FMM to Host interrupts and dispatches to the corresponding sub handlers.
Returns:
	void 
Note:
	Based on the respective bits other handlers are called
***************************************************************************/
void BRAP_P_FMMBF_SourceBuf_isr (
        void * pParm1, /* [in] Raptor channel handle */
        int    iParm2  /* [in] Not used */        
)
{
	BRAP_ChannelHandle  hRapCh;
	uint32_t            ui32IntStatus=0,ui32IntStatusTemp = 0;
	uint32_t            ui32MaskStatus=0;
	bool                bWMFlag = false;
	unsigned int        uichannelpair = 0;
    unsigned int        uiPth = 0;
	
	BDBG_ENTER (BRAP_P_FMMBF_SourceBuf_isr);
	BDBG_ASSERT (pParm1);
	BSTD_UNUSED(iParm2);

	hRapCh = (BRAP_ChannelHandle) pParm1;

	BDBG_MSG(("BRAP_P_FMMBF_SourceBuf_isr\n"));
 
	/* Check all ESR registers and call corresponding ISR for all supported
	interrupts */

	/* For buffer block related interrupts, check AUD_FMM_BF_ESR2_H_STATUS */
	ui32IntStatus = BRAP_Read32_isr (hRapCh->hRegister, BCHP_AUD_FMM_BF_ESR2_H_STATUS);
	BDBG_MSG (("BCHP_AUD_FMM_BF_ESR2_H_STATUS = 0x%x ChanType(%d)", ui32IntStatus, hRapCh->eChannelType));

	/* Clear the interrupts in BCHP_AUD_FMM_BF_ESR2_H_CLEAR
        This is edge triggered. so we need to clear first.*/
	ui32MaskStatus = BRAP_Read32_isr (hRapCh->hRegister, BCHP_AUD_FMM_BF_ESR2_H_MASK);
	BDBG_MSG (("BCHP_AUD_FMM_BF_ESR2_H_MASK = 0x%x", ui32MaskStatus));

        if(hRapCh->bStopinvoked == true)
        {
            return;
        }


	for(uiPth = 0; uiPth < BRAP_P_MAX_PATHS_IN_A_CHAN; uiPth++)
	{
    	if (NULL == hRapCh->pPath[uiPth])
	    {
	        continue;
	    }
	    for (uichannelpair = 0; uichannelpair < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; uichannelpair++)
    	{
    		bWMFlag = false;
    		if (hRapCh->pPath[uiPth]->sRsrcGrnt.uiSrcChId[uichannelpair] !=  (unsigned int) BRAP_RM_P_INVALID_INDEX)
    		{
    			switch(hRapCh->pPath[uiPth]->sRsrcGrnt.uiSrcChId[uichannelpair])
    			{
    				case 0:
    					bWMFlag = (bool) BCHP_GET_FIELD_DATA (ui32IntStatus, 
    						AUD_FMM_BF_ESR2_H_STATUS, SOURCE_RINGBUF_0_EXCEED_FREEMARK);
    					break;   
    				case 1:
    					bWMFlag = (bool) BCHP_GET_FIELD_DATA (ui32IntStatus, 
    						AUD_FMM_BF_ESR2_H_STATUS, SOURCE_RINGBUF_1_EXCEED_FREEMARK);
    					break;
    				case 2:
    					bWMFlag = (bool) BCHP_GET_FIELD_DATA (ui32IntStatus, 
    						AUD_FMM_BF_ESR2_H_STATUS, SOURCE_RINGBUF_2_EXCEED_FREEMARK);
    					break;
    				case 3:
    					bWMFlag = (bool) BCHP_GET_FIELD_DATA (ui32IntStatus, 
    						AUD_FMM_BF_ESR2_H_STATUS, SOURCE_RINGBUF_3_EXCEED_FREEMARK);
    					break;
    				case 4:
    					bWMFlag = (bool) BCHP_GET_FIELD_DATA (ui32IntStatus, 
    						AUD_FMM_BF_ESR2_H_STATUS, SOURCE_RINGBUF_4_EXCEED_FREEMARK);
    					break;   
    				case 5:
    					bWMFlag = (bool) BCHP_GET_FIELD_DATA (ui32IntStatus, 
    						AUD_FMM_BF_ESR2_H_STATUS, SOURCE_RINGBUF_5_EXCEED_FREEMARK);
    					break;   
    				case 6:
    					bWMFlag = (bool) BCHP_GET_FIELD_DATA (ui32IntStatus, 
    						AUD_FMM_BF_ESR2_H_STATUS, SOURCE_RINGBUF_6_EXCEED_FREEMARK);
    					break;   
    				case 7:
    					bWMFlag = (bool) BCHP_GET_FIELD_DATA (ui32IntStatus, 
    						AUD_FMM_BF_ESR2_H_STATUS, SOURCE_RINGBUF_7_EXCEED_FREEMARK);
    					break;   
#if ((BRAP_7420_FAMILY == 1) || (BRAP_7550_FAMILY == 1))
    				case 8:
    					bWMFlag = (bool) BCHP_GET_FIELD_DATA (ui32IntStatus, 
    						AUD_FMM_BF_ESR2_H_STATUS, SOURCE_RINGBUF_8_EXCEED_FREEMARK);
    					break;   
#endif
#if ((BRAP_7420_FAMILY == 1))
    				case 9:
    					bWMFlag = (bool) BCHP_GET_FIELD_DATA (ui32IntStatus, 
    						AUD_FMM_BF_ESR2_H_STATUS, SOURCE_RINGBUF_9_EXCEED_FREEMARK);
    					break;   
#endif				
    				default:
    					break;
    			}

    		}

    		if ((bWMFlag==true))
    		{
    			BRAP_P_FmmRbufFreeWaterMark_isr (pParm1, (int) hRapCh->pPath[uiPth]->sRsrcGrnt.uiSrcChId[uichannelpair]); 
				ui32IntStatusTemp |= (1 << hRapCh->pPath[uiPth]->sRsrcGrnt.uiSrcChId[uichannelpair]);

    		}

    	}
	}
	BDBG_MSG (("BCHP_AUD_FMM_BF_ESR2_H_STATUS = 0x%x ChanType(%d)", ui32IntStatus, hRapCh->eChannelType));

	BRAP_Write32_isr ( hRapCh->hRegister, BCHP_AUD_FMM_BF_ESR2_H_STATUS_CLEAR, ui32IntStatusTemp);


	BDBG_LEAVE (BRAP_P_FMMBF_SourceBuf_isr);
	return;
}

#if (BRAP_7550_FAMILY !=1)
/***************************************************************************
Description:
	This API handles the Destination level FMM to Host interrupts. This is the 
	top level ISR handler for all the Destination level FMM to Host interrupts 
	and dispatches to the corresponding sub handlers.
Returns:
	void 
Note:
	Based on the respective bits other handlers are called
***************************************************************************/
void BRAP_P_FMMBF_DestinationBuf_isr (
        void * pParm1, /* [in] Raptor channel handle */
        int    iParm2  /* [in] Not used */        
)
{
        BRAP_DestinationHandle        hDstHandle;
        uint32_t            ui32IntStatus=0,ui32IntStatusTemp=0,ui32MaskStatus=0;
        bool                bFullWMflag = false, bOverflowFlag = false;
        unsigned int        uichannelpair = 0;        
    
	
	BDBG_ENTER (BRAP_P_FMMBF_DestinationBuf_isr);
	BDBG_ASSERT (pParm1);
	BSTD_UNUSED(iParm2);

	hDstHandle = (BRAP_DestinationHandle) pParm1;

	BDBG_MSG(("BRAP_P_FMMBF_DestinationBuf_isr\n"));

    /* ------------------------- AUD_FMM_BF_ESR1_H ---------------------------*/
    
  	/* For buffer block related interrupts, check AUD_FMM_BF_ESR1_H_STATUS */
	ui32IntStatus = BRAP_Read32_isr (hDstHandle->hAssociation->hRap->hRegister, BCHP_AUD_FMM_BF_ESR1_H_STATUS);
	BDBG_MSG (("BCHP_AUD_FMM_BF_ESR1_H_STATUS = 0x%x Destination Handle(%d)", ui32IntStatus, hDstHandle));

	/* Clear the interrupts in BCHP_AUD_FMM_BF_ESR2_H_CLEAR. This is edge triggered. so we need to clear first.*/
	ui32MaskStatus = BRAP_Read32_isr (hDstHandle->hAssociation->hRap->hRegister, BCHP_AUD_FMM_BF_ESR1_H_MASK);
	BDBG_MSG (("BCHP_AUD_FMM_BF_ESR1_H_MASK = 0x%x", ui32MaskStatus));

        ui32IntStatus &=~ui32MaskStatus;

        ui32IntStatus &= 0xF00; /* Keep Destination Fifo Information*/

        ui32IntStatusTemp =0;
        for (uichannelpair = 0; uichannelpair < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; uichannelpair++)
    	{
		bOverflowFlag = false;
		if (hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiDstChId[uichannelpair] != (unsigned int)BRAP_RM_P_INVALID_INDEX)
		{
			switch(hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiDstChId[uichannelpair])
			{
				case 0:
					bOverflowFlag = (bool) BCHP_GET_FIELD_DATA (ui32IntStatus, 
							  AUD_FMM_BF_ESR1_H_STATUS, DEST_RINGBUF_0_OVERFLOW);
					break;   

				case 1:
					bOverflowFlag = (bool) BCHP_GET_FIELD_DATA (ui32IntStatus, 
							  AUD_FMM_BF_ESR1_H_STATUS, DEST_RINGBUF_1_OVERFLOW);
					break;   

				case 2:
					bOverflowFlag = (bool) BCHP_GET_FIELD_DATA (ui32IntStatus, 
							  AUD_FMM_BF_ESR1_H_STATUS, DEST_RINGBUF_2_OVERFLOW);
					break;   

				case 3:
					bOverflowFlag = (bool) BCHP_GET_FIELD_DATA (ui32IntStatus, 
							  AUD_FMM_BF_ESR1_H_STATUS, DEST_RINGBUF_3_OVERFLOW);
					break;   
				default:
					break;
            }
        }
       
            if ((true == bOverflowFlag) && (hDstHandle->ui32FmmIntMask & BCHP_AIO_INTH_R5F_SET_FMM_BF1_MASK))
            {
                BRAP_P_FmmDestinationRbufFlowControl_isr(pParm1, hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiDstChId[uichannelpair]); 
                ui32IntStatusTemp |= 1 << (8 + (hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiDstChId[uichannelpair]));
                                            /*since for Dstn Rbuf it is bits 8-11 and 0-7bits are for source Rbuf*/
            }        
        }        
#if 0
    /* This interrupt clearing is done in BRAP_FlushRingBuffer_isr() as to avoid 
       the repeated triggering of interrupts for the same condition */
	if(bOverflowFlag == true)
	{
		BRAP_Write32_isr ( hDstHandle->hAssociation->hRap->hRegister, BCHP_AUD_FMM_BF_ESR1_H_STATUS_CLEAR, ui32IntStatusTemp);
    }    
#endif
	BDBG_MSG (("BCHP_AUD_FMM_BF_ESR1_H_STATUS = 0x%x Destination Handle(%d)", ui32IntStatus, hDstHandle));
 

    /* ------------------------- AUD_FMM_BF_ESR2_H ---------------------------*/
    
	/* For buffer block related interrupts, check AUD_FMM_BF_ESR2_H_STATUS */
	ui32IntStatus = BRAP_Read32_isr (hDstHandle->hAssociation->hRap->hRegister, BCHP_AUD_FMM_BF_ESR2_H_STATUS);
	BDBG_MSG (("BCHP_AUD_FMM_BF_ESR2_H_STATUS = 0x%x Destination Handle(%d)", ui32IntStatus, hDstHandle));

	/* Clear the interrupts in BCHP_AUD_FMM_BF_ESR2_H_CLEAR. This is edge triggered. so we need to clear first.*/
	ui32MaskStatus = BRAP_Read32_isr (hDstHandle->hAssociation->hRap->hRegister, BCHP_AUD_FMM_BF_ESR2_H_MASK);
	BDBG_MSG (("BCHP_AUD_FMM_BF_ESR2_H_MASK = 0x%x", ui32MaskStatus));

        ui32IntStatus &=~ui32MaskStatus;

        ui32IntStatus &= 0xF00; /* Keep Destination Fifo Information*/
   
    ui32IntStatusTemp =0;
    for (uichannelpair = 0; uichannelpair < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; uichannelpair++)
	{
		bFullWMflag = false;
		if (hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiDstChId[uichannelpair] != (unsigned int)BRAP_RM_P_INVALID_INDEX)
		{
			switch(hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiDstChId[uichannelpair])
			{
				case 0:
					bFullWMflag = (bool) BCHP_GET_FIELD_DATA (ui32IntStatus, 
							  AUD_FMM_BF_ESR2_H_STATUS, DEST_RINGBUF_0_EXCEED_FULLMARK);
					break;   

				case 1:
					bFullWMflag = (bool) BCHP_GET_FIELD_DATA (ui32IntStatus, 
							  AUD_FMM_BF_ESR2_H_STATUS, DEST_RINGBUF_1_EXCEED_FULLMARK);
					break;   

				case 2:
					bFullWMflag = (bool) BCHP_GET_FIELD_DATA (ui32IntStatus, 
							  AUD_FMM_BF_ESR2_H_STATUS, DEST_RINGBUF_2_EXCEED_FULLMARK);
					break;   

				case 3:
					bFullWMflag = (bool) BCHP_GET_FIELD_DATA (ui32IntStatus, 
							  AUD_FMM_BF_ESR2_H_STATUS, DEST_RINGBUF_3_EXCEED_FULLMARK);
					break;   
				default:
					break;
            }
        }
        if ((bFullWMflag==true) && (hDstHandle->ui32FmmIntMask & BCHP_AIO_INTH_R5F_SET_FMM_BF2_MASK))
        {
           	BRAP_P_FmmDestinationRbufFullMark_isr(pParm1, uichannelpair);
            ui32IntStatusTemp |= 1 << (8 + (hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiDstChId[uichannelpair]));
                                        /*since for Dstn Rbuf it is bits 8-11 and 0-7bits are for source Rbuf*/
        }        
    }

#if 0
    /* This interrupt clearing is done in BRAP_UpdateRingBufUsg_isr() as to avoid 
       the repeated triggering of interrupts for the same condition */
	/*BRAP_Write32_isr ( hDstHandle->hAssociation->hRap->hRegister, BCHP_AUD_FMM_BF_ESR2_H_STATUS_CLEAR, ui32IntStatusTemp);    */
#endif

  	BDBG_MSG (("BCHP_AUD_FMM_BF_ESR2_H_STATUS = 0x%x Destination Handle(%d)", ui32IntStatus, hDstHandle));
    
	BDBG_LEAVE (BRAP_P_FMMBF_DestinationBuf_isr);
	return;        
}
#endif

#if (BRAP_3548_FAMILY == 1)
/***************************************************************************
Description:
	This API handles the SpdifRx to Host interrupts. As of now there are no separate
	despatchers and this single function handles all the interrupts.
Returns:
	void 
***************************************************************************/
void BRAP_P_SPDIF_RX_isr (
        void * pParm1, /* [in] Raptor channel handle */
        int    iParm2  /* [in] Not used */        
)
{
	BRAP_Handle                     hRap = NULL;
	uint32_t                        ui32IntStatus=0;
	uint32_t                        ui32MaskStatus=0;
    BRAP_P_SPDIFRX_DetectChange_InputParams  sDetectChangeInputParams;
    BRAP_P_SPDIFRX_DetectChange_OutputParams sDetectChangeOutputParams;
	
	BDBG_ENTER (BRAP_P_SPDIF_RX_isr);
	BDBG_ASSERT (pParm1);
	BSTD_UNUSED(iParm2);

    hRap = (BRAP_Handle) pParm1;

	/* Check the SPDIF ESR register and handle the interrupt */
	ui32IntStatus = BRAP_Read32_isr (hRap->hRegister, BCHP_SPDIF_RCVR_ESR_STATUS);
	BDBG_MSG (("BCHP_SPDIF_RCVR_ESR_STATUS = 0x%x", ui32IntStatus));

    /* Check the Mask */
	ui32MaskStatus = BRAP_Read32_isr (hRap->hRegister, BCHP_SPDIF_RCVR_ESR_MASK);
	BDBG_MSG (("BCHP_SPDIF_RCVR_ESR_MASK = 0x%x", ui32MaskStatus));

    ui32IntStatus &= ~ui32MaskStatus;

	/* Clear the interrupts in BCHP_SPDIF_RCVR_ESR_CLEAR.
	* This is edge triggered. so we need to clear first.*/
	BRAP_Write32_isr ( hRap->hRegister, BCHP_SPDIF_RCVR_ESR_STATUS_CLEAR, ui32IntStatus);    

    if (ui32IntStatus)
    {
        if ( (BRAP_CapInputPort_eSpdif == hRap->eCapInputPort) ||
             (BRAP_CapInputPort_eHdmi == hRap->eCapInputPort)
           )
        {
            /* Input Change Detection Logic --- Start ----  */
            sDetectChangeInputParams.eCapInputPort = hRap->eCapInputPort;
            sDetectChangeInputParams.eCurrentState = hRap->eSpdifRxState;
            sDetectChangeInputParams.ui32CurrentCompState = hRap->ui32SpdifRxCompState;
            sDetectChangeInputParams.ui32CurrentEsrStatus = ui32IntStatus;
            
            BRAP_SPDIFRX_P_DetectInputChange(
                hRap, 
                &sDetectChangeInputParams,
                &sDetectChangeOutputParams
                );
            
            hRap->eSpdifRxState = sDetectChangeOutputParams.eNewState;
            hRap->ui32SpdifRxCompState = sDetectChangeOutputParams.ui32NewCompState;
            /* Input Change Detection Logic --- End ----  */ 
        
        	if (hRap->sAppIntCbInfo[BRAP_DeviceLevelInterrupt_eSpdifRx].pfAppCb)
        	{
        		hRap->sAppIntCbInfo[BRAP_DeviceLevelInterrupt_eSpdifRx].pfAppCb (
        			hRap->sAppIntCbInfo[BRAP_DeviceLevelInterrupt_eSpdifRx].pParm1,
        			hRap->sAppIntCbInfo[BRAP_DeviceLevelInterrupt_eSpdifRx].iParm2,
        			NULL
        		);
        	}
        }
    }
	BDBG_LEAVE (BRAP_P_SPDIF_RX_isr);
	return;    
}
#endif

/***************************************************************************
Description:
	This API handles the FMM RBUF Free Water Mark interrupt. It is used in the 
	context of a playback channel for letting the application know that the RBUF 
	is free and that the application can fill it.
Returns:
	void 
See Also:
	BRAP_P_FmmDstRbufFullWaterMark_isr
***************************************************************************/
void BRAP_P_FmmRbufFreeWaterMark_isr (
        void * pParm1, /* [in] Raptor channel handle */
        int    iParm2  /* [in] RBUF index */
)
{

	BRAP_ChannelHandle hRapCh;

	BDBG_ENTER (BRAP_P_FmmRbufFreeWaterMark_isr);
	BDBG_ASSERT (pParm1);
	BSTD_UNUSED ( iParm2 );
	hRapCh = (BRAP_ChannelHandle) pParm1;

#if (BRAP_P_WATERMARK_WORKAROUND == 0)
#if (BRAP_P_EDGE_TRIG_INTRPT==0)
	if(hRapCh->eChannelType == BRAP_ChannelType_ePcmPlayback)
		BRAP_P_MaskInterrupt_isr(hRapCh, BRAP_Interrupt_eFmmRbufFreeByte,BRAP_FWIF_P_ResponseType_eNone);
#endif
#endif

	/* If PCM Playback channel, then call the callback handler only if it is in 
	    running state. */
	/* Note: Here we should be checking "channel state". But since "channel state" 
	    is not maintained separately, We look at the SrcCh state. 
	    Also, PCM playback always uses 0th channel pair. */
	if(hRapCh->eChannelType == BRAP_ChannelType_ePcmPlayback &&
		hRapCh->eState == BRAP_P_State_eStarted)
	{
    	if( hRapCh->sAppIntCbInfo[BRAP_Interrupt_eFmmRbufFreeByte].pfAppCb)
		{
			hRapCh->sAppIntCbInfo[BRAP_Interrupt_eFmmRbufFreeByte].pfAppCb (
				hRapCh->sAppIntCbInfo[BRAP_Interrupt_eFmmRbufFreeByte].pParm1,
				hRapCh->sAppIntCbInfo[BRAP_Interrupt_eFmmRbufFreeByte].iParm2,
				NULL /* Not used */
    			);
		}
	}
#if (BRAP_P_WATERMARK_WORKAROUND == 1)
    else if(hRapCh->eChannelType == BRAP_ChannelType_ePcmPlayback)
    {
        /* Temporary workaround if time expires even before channel has started */
        BTMR_StartTimer_isr(hRapCh->hTimer, 1000);
    }
#endif
    
	BDBG_LEAVE (BRAP_P_FmmRbufFreeWaterMark_isr);
 	return;
}

/***************************************************************************
Description:
	This API handles the FMM Destination RBUF Full Water Mark interrupt. This is
	used in the context of a Capture to ringbuffer to let the user know that the 
	Destination ring buffer is full to the water mark limit and they can read from it.
Returns:
	void 
See Also:
	
***************************************************************************/
void BRAP_P_FmmDestinationRbufFullMark_isr (
        void * pParm1, /* [in] Raptor destination handle */
        int    iParm2  
)
{

    BRAP_DestinationHandle        hDstHandle;
    BRAP_DestinationRbufIsrStatus   sDestRbufIsrStatus;
    

    BDBG_ENTER (BRAP_P_FmmDestinationRbufFullMark_isr);    
    BDBG_ASSERT (pParm1);

    hDstHandle = (BRAP_DestinationHandle) pParm1;
    sDestRbufIsrStatus.eChannelPair = (unsigned int)iParm2;
    

#if (BRAP_P_WATERMARK_WORKAROUND == 0)
#if (BRAP_P_EDGE_TRIG_INTRPT==0)
    BRAP_P_MaskDestinationInterrupt_isr(hDstHandle, BRAP_DestinationInterrupt_eFmmRbufFullMark);
#endif
#endif

	/*  Earlier when capture api interrupt was channel based, the condition "if(BRAP_P_State_eStarted == hRapCh->eState" condition used to get checked here.
	Do we now need to check ouput start or anything related, before calling App installed callback here? */
	
	if( hDstHandle->sAppIntCbInfo[BRAP_DestinationInterrupt_eFmmRbufFullMark].pfAppCb)
	{
		hDstHandle->sAppIntCbInfo[BRAP_DestinationInterrupt_eFmmRbufFullMark].pfAppCb (
			hDstHandle->sAppIntCbInfo[BRAP_DestinationInterrupt_eFmmRbufFullMark].pParm1,
			hDstHandle->sAppIntCbInfo[BRAP_DestinationInterrupt_eFmmRbufFullMark].iParm2,
			&sDestRbufIsrStatus
		);
	}
#if (BRAP_P_WATERMARK_WORKAROUND == 1)
    else
    {
        /* Temporary workaround for internal capture if time expires even before channel has started */
        BTMR_StartTimer_isr(hRapCh->hTimer, 1000);
    }
#endif

	BDBG_LEAVE (BRAP_P_FmmDestinationRbufFullMark_isr);
	return;
}

/***************************************************************************
Description:
	This API handles the FMM Destination RBUF Overflow interrupt. This is
	used in the context of a Capture to ringbuffer to let the user know that the 
	Destination ring buffer is full and has overflowed
Returns:
	void 
See Also:
	
***************************************************************************/
void BRAP_P_FmmDestinationRbufFlowControl_isr (
        void * pParm1, /* [in] Raptor destination handle */
        int    iParm2  
)
{

    BRAP_DestinationHandle        hDstHandle;
    BRAP_DestinationRbufIsrStatus   sDestRbufIsrStatus;

    BDBG_ENTER (BRAP_P_FmmDestinationRbufFlowControl_isr);    
    BDBG_ASSERT (pParm1);

    hDstHandle = (BRAP_DestinationHandle) pParm1;
    sDestRbufIsrStatus.eChannelPair = (unsigned int)iParm2;

#if (BRAP_P_WATERMARK_WORKAROUND == 0)
#if (BRAP_P_EDGE_TRIG_INTRPT==0)
    BRAP_P_MaskDestinationInterrupt_isr(hDstHandle, BRAP_DestinationInterrupt_eFmmRbufOverflow);
#endif
#endif

	/*  Earlier when capture api interrupt was channel based, the condition "if(BRAP_P_State_eStarted == hRapCh->eState" condition used to get checked here.
	Do we now need to check ouput start or anything related, before calling App installed callback here? */
	
	if( hDstHandle->sAppIntCbInfo[BRAP_DestinationInterrupt_eFmmRbufOverflow].pfAppCb)
	{
		hDstHandle->sAppIntCbInfo[BRAP_DestinationInterrupt_eFmmRbufOverflow].pfAppCb (
			hDstHandle->sAppIntCbInfo[BRAP_DestinationInterrupt_eFmmRbufOverflow].pParm1,
			hDstHandle->sAppIntCbInfo[BRAP_DestinationInterrupt_eFmmRbufOverflow].iParm2,
			&sDestRbufIsrStatus
		);
	}
#if (BRAP_P_WATERMARK_WORKAROUND == 1)
    else
    {
        /* Temporary workaround for internal capture if time expires even before channel has started */
        BTMR_StartTimer_isr(hRapCh->hTimer, 1000);
    }
#endif

	BDBG_LEAVE (BRAP_P_FmmDestinationRbufFlowControl_isr);
	return;
}


/***************************************************************************
Description:
    This Isr function is called when sample rate associated with any Destination is programmed.
    
Returns:
	void 
***************************************************************************/

void BRAP_P_DestinationSampleRateChange_isr (
        void * pParm1, /* [in] Raptor destination handle */
        unsigned int    iParm2  
)
{

    BRAP_DestinationHandle        hDstHandle;
    BRAP_DSPCHN_DestinationSampleRateChangeInfo   sDestSampleRate;

    BDBG_ENTER (BRAP_P_DestinationSampleRateChange_isr);    
    BDBG_ASSERT (pParm1);

    hDstHandle = (BRAP_DestinationHandle) pParm1;
    sDestSampleRate.eSamplingRate   = (BAVC_AudioSamplingRate)iParm2;

    BDBG_MSG(("==> hDstHandle= %x, sDestSampleRate.eSamplingRate =%d",hDstHandle,sDestSampleRate.eSamplingRate));


	/*  Earlier when capture api interrupt was channel based, the condition "if(BRAP_P_State_eStarted == hRapCh->eState" condition used to get checked here.
	Do we now need to check ouput start or anything related, before calling App installed callback here? */
	
	if( hDstHandle->sAppIntCbInfo[BRAP_DestinationInterrupt_eSampleRateChange].pfAppCb)
	{
		hDstHandle->sAppIntCbInfo[BRAP_DestinationInterrupt_eSampleRateChange].pfAppCb (
			hDstHandle->sAppIntCbInfo[BRAP_DestinationInterrupt_eSampleRateChange].pParm1,
			hDstHandle->sAppIntCbInfo[BRAP_DestinationInterrupt_eSampleRateChange].iParm2,
			&sDestSampleRate
		);
	}
	BDBG_LEAVE (BRAP_P_DestinationSampleRateChange_isr);
	return;
}



/***************************************************************************
Description:
	The APIs below are the low level interrupt hanlders which are dispatched 
	from the top level handlers
Returns:
	All of them return void
Note:
	This comment is common for all the APIs below
***************************************************************************/

static void BRAP_P_Watchdog_isr(void *pParm1,  int iParm2 )
{

#ifdef RAP_SCM_SUPPORT

	BRAP_Handle hRap = (BRAP_Handle) pParm1;
	BSTD_UNUSED (iParm2);

	/* Call the application callback */
	if(hRap->sAppIntCbInfo[BRAP_DeviceLevelInterrupt_eWatchdog].pfAppCb) 
	{
		hRap->sAppIntCbInfo[BRAP_DeviceLevelInterrupt_eWatchdog].pfAppCb (
			hRap->sAppIntCbInfo[BRAP_DeviceLevelInterrupt_eWatchdog].pParm1,
			hRap->sAppIntCbInfo[BRAP_DeviceLevelInterrupt_eWatchdog].iParm2,
			NULL
		);
	}

#else
	unsigned int uiDspId = 0;
	BRAP_Handle hRap = (BRAP_Handle) pParm1;
	BSTD_UNUSED (iParm2); 

        hRap->bWatchdogTriggered = true;        
	/* Reset the DSP */
	for (uiDspId = 0; uiDspId < BRAP_RM_P_MAX_DSPS; uiDspId++)
	{
		BRAP_DSP_P_WatchDogResetHardware ( hRap->hDsp[uiDspId] );
    }

	/* Reset the FMM block */
	BRAP_FMM_P_ResetHardware ( hRap->hFmm[0] );
	/* hrap->hfmm[0]->hsrcch[id] != 0 get group id from this hanlde and check id+1, id+2, id+3 */

	BDBG_WRN(("Raptor Watchdog Interrupt occured for RAP handle 0x%08x", hRap));

	/* Call the application callback */
	if(hRap->sAppIntCbInfo[BRAP_DeviceLevelInterrupt_eWatchdog].pfAppCb) 
	{
		hRap->sAppIntCbInfo[BRAP_DeviceLevelInterrupt_eWatchdog].pfAppCb (
			hRap->sAppIntCbInfo[BRAP_DeviceLevelInterrupt_eWatchdog].pParm1,
			hRap->sAppIntCbInfo[BRAP_DeviceLevelInterrupt_eWatchdog].iParm2,
			NULL
		);
	}
#endif	
}

#if (BRAP_3548_FAMILY == 1)
static void BRAP_P_RfAudio_isr(void * pParm1, int iParm2)
{
	BRAP_Handle hRap = (BRAP_Handle) pParm1;
    BRAP_CAPPORT_P_Handle hCapPort;
    uint32_t ui32IntStatus, ui32MaskStatus;
    
	BSTD_UNUSED (iParm2); 
	BDBG_MSG(("RF Audio Interrupt occured for RAP handle 0x%08x", hRap));

    /* Read RF Audio interrupt status and mask registers */
    ui32IntStatus = BRAP_Read32_isr( hRap->hRegister, BCHP_BTSC_ESR_INT_STATUS );
    ui32MaskStatus = BRAP_Read32_isr( hRap->hRegister, BCHP_BTSC_ESR_MASK_STATUS );

    /* Clear only un-masked interrupts. Status of masked interrupts might be
     * helpful in debugging. */
    ui32IntStatus &= ~ui32MaskStatus;
    BRAP_Write32_isr(hRap->hRegister, BCHP_BTSC_ESR_INT_CLEAR, ui32IntStatus);
    
    hCapPort = hRap->hFmm[0]->hCapPort[BRAP_CapInputPort_eRfAudio];
    
	if (BCHP_GET_FIELD_DATA(ui32IntStatus, BTSC_ESR_INT_STATUS, IFIFO_E_V_INTR) == 1)
    {
        BDBG_MSG(("RF Audio input FIFO empty interrupt"));
    }
	if (BCHP_GET_FIELD_DATA(ui32IntStatus, BTSC_ESR_INT_STATUS, IFIFO_F_V_INTR) == 1)
    {
        BDBG_MSG(("RF Audio input FIFO full interrupt"));
    }
	if (BCHP_GET_FIELD_DATA(ui32IntStatus, BTSC_ESR_INT_STATUS, OFIFO_E_V_INTR) == 1)
    {
        BDBG_MSG(("RF Audio output FIFO empty interrupt"));
    }
	if (BCHP_GET_FIELD_DATA(ui32IntStatus, BTSC_ESR_INT_STATUS, OFIFO_F_V_INTR) == 1)
    {
        BDBG_MSG(("RF Audio output FIFO full interrupt"));
    }
	if (BCHP_GET_FIELD_DATA(ui32IntStatus, BTSC_ESR_INT_STATUS, PROC_ERR_INTR) == 1)
    {
        BDBG_MSG(("RF Audio processor error interrupt"));
    }
	if (BCHP_GET_FIELD_DATA(ui32IntStatus, BTSC_ESR_INT_STATUS, AMCARRIERON_MONO_INTR) == 1)
    {
        BDBG_MSG(("RF Audio Japan broadcast to MONO interrupt"));
    }
	if (BCHP_GET_FIELD_DATA(ui32IntStatus, BTSC_ESR_INT_STATUS, AUTOSW_STEXMONO_INTR) == 1)
    {
        BDBG_MSG(("RF Audio stereo/mono autoswitch interrupt"));
    }
	if (BCHP_GET_FIELD_DATA(ui32IntStatus, BTSC_ESR_INT_STATUS, AUTOSW_SAPXMUTE_INTR) == 1)
    {
        BDBG_MSG(("RF Audio SAP/Mute autoswitch interrupt"));
        BRAP_RFAUDIO_P_SapXMute_isr( hCapPort );
    }
	if (BCHP_GET_FIELD_DATA(ui32IntStatus, BTSC_ESR_INT_STATUS, RAM_INIT_INTR) == 1)
    {
        BDBG_MSG(("RF Audio RAM init done interrupt"));
    }
 	if (BCHP_GET_FIELD_DATA(ui32IntStatus, BTSC_ESR_INT_STATUS, OMP_INTR) == 1)
    {
        BDBG_MSG(("BTSC OMP interrupt"));
    }

	/* Call the application callback */
	if(hRap->sAppIntCbInfo[BRAP_DeviceLevelInterrupt_eRfAudio].pfAppCb) 
	{
		hRap->sAppIntCbInfo[BRAP_DeviceLevelInterrupt_eRfAudio].pfAppCb (
			hRap->sAppIntCbInfo[BRAP_DeviceLevelInterrupt_eRfAudio].pParm1,
			hRap->sAppIntCbInfo[BRAP_DeviceLevelInterrupt_eRfAudio].iParm2,
			NULL
		);
	}
}
#endif


#ifdef CRC_ENABLE
static void BRAP_P_CRC_isr(void *pParm1, int iParam2)
{
	BRAP_ChannelHandle	hRapCh = (BRAP_ChannelHandle)pParm1;
	uint32_t				Crc_cfg = hRapCh->sAppIntCbInfo[BRAP_Interrupt_eFmmCrc].iParm2;
	uint32_t				Crc_value;
	BDBG_ENTER(BRAP_P_CRC_isr);
	BDBG_ASSERT(hRapCh);
	BSTD_UNUSED(iParam2);	
	BDBG_MSG(("In crc_isr %x",Crc_cfg));
	Crc_value=BRAP_Read32 (hRapCh->hRegister, 
                   ( BCHP_AUD_FMM_OP_CTRL_CRC_STATUSi_ARRAY_BASE 
                   + Crc_cfg*4 )); 
	if (hRapCh->sAppIntCbInfo[BRAP_Interrupt_eFmmCrc].pfAppCb)
	{
		hRapCh->sAppIntCbInfo[BRAP_Interrupt_eFmmCrc].pfAppCb (
		hRapCh->sAppIntCbInfo[BRAP_Interrupt_eFmmCrc].pParm1,
		hRapCh->sAppIntCbInfo[BRAP_Interrupt_eFmmCrc].iParm2,
		&Crc_value);
	}
	BDBG_LEAVE(BRAP_P_CRC_isr);
}
#endif

#if (BRAP_3548_FAMILY == 1)
void BRAP_P_EnableRfAudioInterrupts (
    BRAP_CAPPORT_P_Handle       phCapPort     /* [out] Pointer to Capture Port handle */
)
{
    uint32_t ui32RegVal;

    /* Clear all pending interrupts */
    BRAP_Write32( phCapPort->hRegister, BCHP_BTSC_ESR_INT_CLEAR, 0xFFFFFFFF );

    /* Enable required RF Audio interrupts */
    ui32RegVal = 0;
    ui32RegVal |= BCHP_FIELD_DATA( BTSC_ESR_MASK_CLEAR, IFIFO_E_INTR_MASK, 1)
        |BCHP_FIELD_DATA( BTSC_ESR_MASK_CLEAR, IFIFO_F_INTR_MASK, 1)
        |BCHP_FIELD_DATA( BTSC_ESR_MASK_CLEAR, OFIFO_E_INTR_MASK, 1)
        |BCHP_FIELD_DATA( BTSC_ESR_MASK_CLEAR, OFIFO_F_INTR_MASK, 1)
        |BCHP_FIELD_DATA( BTSC_ESR_MASK_CLEAR, PROC_ERR_INTR_MASK, 1)
        |BCHP_FIELD_DATA( BTSC_ESR_MASK_CLEAR, AMSIDEBANDS_MONO_INTR_MASK, 0)
        |BCHP_FIELD_DATA( BTSC_ESR_MASK_CLEAR, AUTOSW_STEREO_MONO_INTR_MASK, 0)
        |BCHP_FIELD_DATA( BTSC_ESR_MASK_CLEAR, AUTOSW_SAPXMUTE_INTR_MASK, 0)
        |BCHP_FIELD_DATA( BTSC_ESR_MASK_CLEAR, RAM_INIT_INTR_MASK, 1)
        |BCHP_FIELD_DATA( BTSC_ESR_MASK_CLEAR, OMP_INTR_MASK, 1);

    BRAP_Write32( phCapPort->hRegister, BCHP_BTSC_ESR_MASK_CLEAR, ui32RegVal );
    return;
}


void BRAP_P_DisableRfAudioInterrupts (
    BRAP_CAPPORT_P_Handle       phCapPort     /* [out] Pointer to Capture Port handle */
)
{

    BRAP_Write32( phCapPort->hRegister, BCHP_BTSC_ESR_MASK_SET, 0xffffffff );
    return;
}
#endif

#ifdef 	RAP_GFX_SUPPORT
static void BRAP_P_GfxOperationCompleted_isr(void *pParam1, unsigned int    iParm2 )
{
    BRAP_ChannelHandle hRapCh;
    unsigned int uiCommandCounter;
    BDBG_ENTER (BRAP_P_GfxOperationCompleted_isr);    
    BDBG_ASSERT (pParam1);
    uiCommandCounter = (unsigned int)iParm2;
    BDBG_MSG(("---------> BRAP_P_GfxOperationCompleted_isr, Command Id = %dcompleted ",uiCommandCounter));

    hRapCh = (BRAP_ChannelHandle)pParam1;

    if( hRapCh->sAppIntCbInfo[BRAP_Interrupt_eGfxOperationComplete].pfAppCb)
    {
        hRapCh->sAppIntCbInfo[BRAP_Interrupt_eGfxOperationComplete].pfAppCb (
            hRapCh->sAppIntCbInfo[BRAP_Interrupt_eGfxOperationComplete].pParm1,
            hRapCh->sAppIntCbInfo[BRAP_Interrupt_eGfxOperationComplete].iParm2,
            NULL
        );
    }
    BDBG_LEAVE (BRAP_P_GfxOperationCompleted_isr);
    return;
}
#endif

#ifdef RAP_SCM_SUPPORT 
static void BRAP_P_ScmResponse_isr(void *pParam1, unsigned int    iParm2 )
{
    BRAP_ChannelHandle hRapCh;
	BRAP_FWIF_P_BspScmCodeAuthInfo   sScmCodeAuthInfo;
	BRAP_FWIF_P_BspScmCodeAuthInfo   *pScmCodeAuthInfo;

    BDBG_ENTER (BRAP_P_ScmResponse_isr);    
    BDBG_ASSERT (pParam1);

	pScmCodeAuthInfo = (BRAP_FWIF_P_BspScmCodeAuthInfo*)iParm2;
	sScmCodeAuthInfo.eBspScmCodeAuthStatus = pScmCodeAuthInfo->eBspScmCodeAuthStatus;
	
    BDBG_MSG(("BRAP_P_ScmResponse_isr : Response = %d", sScmCodeAuthInfo.eBspScmCodeAuthStatus ));

    hRapCh = (BRAP_ChannelHandle)pParam1;

    if( hRapCh->sAppIntCbInfo[BRAP_Interrupt_eScmResponse].pfAppCb)
    {
        hRapCh->sAppIntCbInfo[BRAP_Interrupt_eScmResponse].pfAppCb (
            hRapCh->sAppIntCbInfo[BRAP_Interrupt_eScmResponse].pParm1,
            hRapCh->sAppIntCbInfo[BRAP_Interrupt_eScmResponse].iParm2,
            &sScmCodeAuthInfo
        );
    }
    BDBG_LEAVE (BRAP_P_ScmResponse_isr);
    return;
}
#endif




/* End of File */
    
