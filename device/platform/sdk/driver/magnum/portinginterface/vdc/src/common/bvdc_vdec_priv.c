/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_vdec_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/759 $
 * $brcm_Date: 8/16/12 2:23p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_vdec_priv.c $
 * 
 * Hydra_Software_Devel/759   8/16/12 2:23p yuxiaz
 * SW7425-3619: Replace uncache memory access with cache
 * memory access.
 * 
 * Hydra_Software_Devel/758   8/13/12 3:09p yuxiaz
 * SW7425-3626, SW7425-3619: Replace uncache memory access with cache
 * memory access: use the new API names in RDC.
 * 
 * Hydra_Software_Devel/757   8/10/12 3:26p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/756   3/8/12 11:51a yuxiaz
 * SW7425-2546: Fixed warning msg.
 *
 * Hydra_Software_Devel/755   6/24/11 3:49p albertl
 * SW7420-1966: Cleaned up and reorganized colorspace enums and indicies
 * in display, vdec, and cmp.
 *
 * Hydra_Software_Devel/754   1/12/11 4:35p yuxiaz
 * SW7335-1133: Added more debug messages for VDC heap debugging.
 *
 * Hydra_Software_Devel/753   11/11/10 7:32p albertl
 * SW7125-364: Fixed BVDC_P_CbIsDirty and added assert to check bitfields
 * in dirty bits fit within union integer representation.  Fixed naming
 * of dirty bits.
 *
 * Hydra_Software_Devel/752   9/24/10 5:13p pntruong
 * CR3556-66: SECAM video problem at scart source.  Put into hw auto acgc
 * for secam format.
 *
 * Hydra_Software_Devel/751   9/21/10 10:15a yuxiaz
 * SW3556-1193: undo previous change.
 *
 * Hydra_Software_Devel/750   9/17/10 11:27a yuxiaz
 * SW3556-1193: Scart-RGB grayscale image looks purple.
 *
 * Hydra_Software_Devel/749   9/8/10 5:48p pntruong
 * SW3556-1191: Added work-around for rf signal with top-only or bot-only
 * to put sync into big mode, and uses hw acgc mode.
 *
 * Hydra_Software_Devel/748   9/8/10 5:40p pntruong
 * SW3556-1138: Added similar work-around in 3560/3563 where comb are
 * turned off if it's in legacy mode when detect fh/fv shift.
 *
 * Hydra_Software_Devel/747   7/9/10 11:36p pntruong
 * SW3556-1143: Additional updates from pq engr prior optimization work.
 *
 * Hydra_Software_Devel/746   7/9/10 5:47p pntruong
 * SW3556-1143: Video output level is changed  between 910mV and 990mV in
 * analog tuner sources with below conditions.  Additional parameters.
 *
 * Hydra_Software_Devel/745   7/9/10 5:25p pntruong
 * SW3556-1143: Video output level is changed  between 910mV and 990mV in
 * analog tuner sources with below conditions.
 *
 * Hydra_Software_Devel/744   7/1/10 12:08p pntruong
 * SW3556-1140: Fix garbage at bottom for PAL Fv test.
 *
 * Hydra_Software_Devel/743   6/29/10 1:42p pntruong
 * SW3556-1138:  Vertical frequency shift problem.
 *
 * Hydra_Software_Devel/742   6/23/10 3:08p pntruong
 * CR3556-62: Color Sync issue happens on Fv test for PAL on 3556.
 *
 * Hydra_Software_Devel/741   5/7/10 7:21p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings
 *
 * Hydra_Software_Devel/740   4/19/10 10:23p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/739   4/16/10 2:56p pntruong
 * SW3548-2889: Need to trigger the programming of the top control for raw
 * input.
 *
 * Hydra_Software_Devel/738   4/15/10 5:04p pntruong
 * SW3548-2889: Added option in vdec setting to allow user to select 11-
 * bit fullrange processing.
 *
 * Hydra_Software_Devel/737   4/7/10 11:36a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/736   4/6/10 10:48a pntruong
 * SW3556-1085: Allowed cvbs-rgb scart with the external ifd option.
 *
 * Hydra_Software_Devel/735   4/5/10 4:15p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/734   3/29/10 4:52p pntruong
 * SW3556-1085: Allowed cvbs-rgb scart with the external ifd option.
 *
 * Hydra_Software_Devel/733   2/21/10 3:10p pntruong
 * SW3556-961: Fine tuned mv settings for 625 formats.
 *
 * Hydra_Software_Devel/732   2/3/10 4:43p pntruong
 * SW3548-2751: Added feature to report vdec agc level.
 *
 * Hydra_Software_Devel/731   2/1/10 3:14p pntruong
 * SW3556-1026: Horizontal dot noise in AV/SCART.
 *
 * Hydra_Software_Devel/730   2/1/10 2:18p pntruong
 * SW3556-961: Updated mv settings from hw recommendation.  May need a
 * further fine turned from mv characterization.
 *
 * Hydra_Software_Devel/729   1/21/10 9:33a pntruong
 * SW3556-993: For noisy/unstable signal disabled mad output loop control
 * to avoid fluttering similar to vcr mode.
 *
 * Hydra_Software_Devel/728   1/19/10 3:37p pntruong
 * SW3556-962: More secam detection tuning.
 *
 * Hydra_Software_Devel/727   1/14/10 4:01p pntruong
 * SW3556-962: Final secam detection tuning.
 *
 * Hydra_Software_Devel/726   1/13/10 9:33p pntruong
 * SW3556-967: Corrected the 3dcomb delay reports in callback for pal
 * formats.
 *
 * Hydra_Software_Devel/725   1/12/10 9:31a pntruong
 * SW3556-962: Fine tuned more secam settings for format detection, and
 * back out using the acgc status.
 *
 * Hydra_Software_Devel/724   1/8/10 4:21p pntruong
 * SW3556-962: Add addition software (using VDEC_CD_0_SECAM_ACGC_STATUS)
 * to detect secam unlock.  Also additional fine tune on settings.
 *
 * Hydra_Software_Devel/723   1/7/10 9:34p pntruong
 * SW3556-978: Use hsync witdth to identify format for pc format.  Print
 * the real fd table from dram.
 *
 * Hydra_Software_Devel/722   12/22/09 3:47p pntruong
 * SW3556-909: Additional threshold fine tuned for detecting no signal.
 *
 * Hydra_Software_Devel/721   12/15/09 8:26p pntruong
 * SW3556-886: TTX operation is wrong in SECAM signal.  Dynamically turn
 * on/off secam filter in sop/eop. Also use vfd clipping for this case.
 *
 * Hydra_Software_Devel/720   12/8/09 6:55p pntruong
 * SW3556-916: Improved default bandfilter for secam.  Refactored the
 * compiling flag for fast format detection but default off.
 *
 * Hydra_Software_Devel/719   12/8/09 3:44p pntruong
 * SW3556-916:  Additional fine tuned SECAM settings based suggestion from
 * HW.
 *
 * Hydra_Software_Devel/718   12/1/09 4:51p pntruong
 * SW3556-880: Optimized the rf input format detection time.
 *
 * Hydra_Software_Devel/717   12/1/09 12:04p pntruong
 * SW3556-892:  It happens Horizental shaking in SECAM L.
 *
 * Hydra_Software_Devel/716   11/30/09 3:56p pntruong
 * SW3556-844: Causing side-effects with cvbs input.  Back out for further
 * review of bi-level signal support.
 *
 * Hydra_Software_Devel/715   11/17/09 10:01p pntruong
 * SW3556-866: Expose vdec frontend csc for adc calibration for scart
 * input.
 *
 * Hydra_Software_Devel/714   11/12/09 3:58p pntruong
 * SW3556-848: Re-enable fullrange (11-bit mode) support.
 *
 * Hydra_Software_Devel/713   11/6/09 12:00p rpan
 * SW3556-844: Fixed a compile warning.
 *
 * Hydra_Software_Devel/712   11/6/09 11:32a rpan
 * SW3556-844: Integrated bi-level Hsync changes from field.
 *
 * Hydra_Software_Devel/711   10/20/09 3:25p pntruong
 * SW3556-829: SECAM-L dot Noise - All channel happen.  Fine tuned
 * additional secam settings for rf input.
 *
 * Hydra_Software_Devel/710   10/5/09 10:28p pntruong
 * SW3556-804: Need to be able to get function status update without auto
 * format enabled.
 *
 * Hydra_Software_Devel/709   10/5/09 10:45a pntruong
 * SW3556-804: Fixed build error.
 *
 * Hydra_Software_Devel/708   10/5/09 10:43a pntruong
 * SW3556-804: Should be for all input.  Used the better internal status
 * which is more reliable.
 *
 * Hydra_Software_Devel/707   10/1/09 6:19p pntruong
 * SW3556-804: Added scart function select callback.
 *
 * Hydra_Software_Devel/706   9/29/09 10:05p pntruong
 * SW3556-740: Additional fine tuning of secam settings per recommendation
 * from hw.
 *
 * Hydra_Software_Devel/705   9/23/09 6:54p pntruong
 * SW3556-740: Fine tuned SECAM settings for auto detect SECAM/PAL on IF
 * input as suggested by hw desginers.
 *
 * Hydra_Software_Devel/704   8/18/09 10:17p pntruong
 * PR57834: [GDVR] The video image shakes at the Composite signal.
 *
 * Hydra_Software_Devel/703   8/11/09 4:47p pntruong
 * PR51338: Another round of fine tuning the settings for fullrange.
 *
 * Hydra_Software_Devel/702   8/10/09 1:15p pntruong
 * PR51338: Temporary disabled the 11-bit fullrange mode until compoent
 * vbi is fixed.
 *
 * Hydra_Software_Devel/701   7/24/09 6:04p syang
 * PR 56664: increase the delayStart of the low vert freq (<50) fmt from
 * cmp input to 18
 *
 * Hydra_Software_Devel/700   6/9/09 11:10a pntruong
 * PR55747: Power consumption is more than expected.  Powerdown all adc
 * including the sif one (adc 5), when vdec source is destroyed.
 *
 * Hydra_Software_Devel/699   6/3/09 11:43p pntruong
 * PR55696: Expose new Combing registers for PQ tuning.
 *
 * Hydra_Software_Devel/698   6/3/09 5:58p pntruong
 * PR55682: VCR input shows vertical jitter.
 *
 * Hydra_Software_Devel/697   6/2/09 11:06a pntruong
 * PR55607: Reset fd together with the other reset.
 *
 * Hydra_Software_Devel/696   6/1/09 1:47p pntruong
 * PR55612: Define AWC region for content with copy protection.
 *
 * Hydra_Software_Devel/695   5/28/09 5:16p pntruong
 * PR51338: Turn off awc for 11-bit since this is for 10-bit mode only.
 *
 * Hydra_Software_Devel/694   5/28/09 2:20p yuxiaz
 * PR55364: Disable dithering when any testfeature1 is enabled
 *
 * Hydra_Software_Devel/693   5/22/09 11:33a tdo
 * PR55364: Disable dithering when any test feature is enabled
 *
 * Hydra_Software_Devel/692   5/20/09 3:24p pntruong
 * PR55329: Need to toggle VDEC_CD_0_CD_CTRL.CD bits to have
 * VDEC_CD_0_VAR_DELAY commit the change.
 *
 * Hydra_Software_Devel/691   5/15/09 2:27p pntruong
 * PR51338: Updated for the rgb input signal.
 *
 * Hydra_Software_Devel/690   5/14/09 7:55p pntruong
 * PR51338: Fine tuned the values for mv detected.
 *
 * Hydra_Software_Devel/689   5/12/09 10:45p pntruong
 * PR51338: Added support for adc fullrange raw 11-bit.
 *
 * Hydra_Software_Devel/688   5/6/09 6:08p pntruong
 * PR54885, PR51338: Need to restore RAW_11B to dither data; until further
 * evaluations.
 *
 * Hydra_Software_Devel/687   4/29/09 4:17p pntruong
 * PR54583: Update Y/C delay settings for Svideo.
 *
 * Hydra_Software_Devel/686   4/29/09 3:00p pntruong
 * PR54582: Update AGC/AWC settings for CVBS/Component input.
 *
 * Hydra_Software_Devel/685   4/21/09 7:23p pntruong
 * PR54382: Every tuner change color is different on RF.
 *
 * Hydra_Software_Devel/684   4/17/09 4:32p yuxiaz
 * PR54135: When change window size from full screen, soucepending
 * callback won't fire.
 *
 * Hydra_Software_Devel/683   4/17/09 11:56a pntruong
 * PR54064: Refactored common code for defered callback til bvn shutdown.
 * Improved readability and extensibility of dirty bits.
 *
 * Hydra_Software_Devel/682   4/10/09 6:33p hongtaoz
 * PR54064: source pending callback must fire after window shutdown;
 *
 * Hydra_Software_Devel/681   3/31/09 10:26p pntruong
 * PR53597: VDEC failed to quickly lock the tuner output in AFT.
 *
 * Hydra_Software_Devel/680   3/31/09 10:29a pntruong
 * PR53137: SIF ADC require to be active even if no adc is in used.
 *
 * Hydra_Software_Devel/679   3/30/09 10:58p pntruong
 * PR53137: ADC conflict when doing PIP/PBP between PC/Component and
 * SIF(audio).  Fixed build errors.
 *
 * Hydra_Software_Devel/678   3/30/09 10:26p pntruong
 * PR53137: ADC conflict when doing PIP/PBP between PC/Component and
 * SIF(audio).
 *
 * Hydra_Software_Devel/677   3/23/09 5:28p pntruong
 * PR53528: Fine tuned DCR start register for mv and non-mv.
 *
 * Hydra_Software_Devel/676   3/18/09 9:17p pntruong
 * PR52420: Fine tuned color kill.  Put acgc/color mode into auto when
 * detected vcr head switch mode.  Separate out update of cross-chroma
 * and 3dcomb.
 *
 * Hydra_Software_Devel/675   3/12/09 11:47p pntruong
 * PR52952: Don't reset video detected when force source pending.  Cleared
 * out unused code.
 *
 * Hydra_Software_Devel/674   3/11/09 11:41a pntruong
 * PR52952: When force pending for VDEC sources from application, source
 * pending callback happens twice and video present status toggles.
 *
 * Hydra_Software_Devel/673   3/2/09 6:35p pntruong
 * PR39711: Used the colorstripe status in cd block instead of fe.  New hw
 * implementation routed signal to cd.
 *
 * Hydra_Software_Devel/672   2/26/09 6:11p yuxiaz
 * PR52287: 422-444 and 444-422 converters should use decimate/duplicate.
 *
 * Hydra_Software_Devel/671   2/26/09 3:09p pntruong
 * PR52282: Improved autodetection for in ifd input.
 *
 * Hydra_Software_Devel/PR52282/2   2/20/09 6:44p xkliu
 * PR52282: [customer] sometimes autodetection failed in analog RF.
 * 1. fix a mistake about pc/comp signals.
 * 2. improve the transitions between palnc and ntsc,
 * 3. tune treatholds about ulBlackWhiteCnt for cross-hatch channels.
 *
 * Hydra_Software_Devel/PR52282/1   2/19/09 7:16p xkliu
 * PR52282: [customer] sometimes autodetection failed in analog RF.
 * video transitions or wrong video formats after switching channels.
 *
 * Hydra_Software_Devel/670   2/24/09 2:29p pntruong
 * PR52420: Cross color on cvbs, no burst signal.  Need to account for
 * both ifd and cvbs input.
 *
 * Hydra_Software_Devel/669   2/23/09 9:36p pntruong
 * PR49561: [VDEC]: Vertical black bar on the white noise channel.
 *
 * Hydra_Software_Devel/668   2/23/09 8:43p pntruong
 * PR52420: Cross color on cvbs, no burst signal.
 *
 * Hydra_Software_Devel/667   2/23/09 1:28p pntruong
 * PR31337: B2: Need to verify fix for SECAM YC separation filter bypass
 * works.  Corrected the programming for VDEC_CD_0_SECAM_CTL.LUMA_REJECT.
 *
 * Hydra_Software_Devel/666   2/20/09 5:35p pntruong
 * PR49362: Fine tuned bilevel detection.
 *
 * Hydra_Software_Devel/665   2/20/09 1:54p hongtaoz
 * PR51336: 3548B2 HW fixed VDEC short field problem; no need to toggle
 * PAL_SEPARATION for noisy input;
 *
 * Hydra_Software_Devel/664   2/18/09 8:03p pntruong
 * PR52270: Fine tuned the awc for vdec->vec passthru block.
 *
 * Hydra_Software_Devel/663   2/18/09 7:37p pntruong
 * PR52270: Added support for svideo input, and added default values for
 * new blanklevel field.
 *
 * Hydra_Software_Devel/662   2/18/09 1:22p pntruong
 * PR51196: Fined tune kill color in RF to reach burst=7% target.
 *
 * Hydra_Software_Devel/pr51196-take2/1   2/18/09 12:15p xkliu
 * PR51196:[customer] tune kill color in RF to reach burst=7% target.
 * 1. tune threatholds.
 * 2. aviod pollution from unstable macrovision status in rf.
 *
 * Hydra_Software_Devel/661   2/13/09 11:19p pntruong
 * PR52041 : Fine tuned VDEC_FE_0_NB_CONTROL.LINE = 4 for IFD input PAL-
 * BGDK format.
 *
 * Hydra_Software_Devel/660   2/13/09 9:59p pntruong
 * PR51857: [VDEC/PC PLL] Need to optimize the settings for pll to improve
 * format detection.
 *
 * Hydra_Software_Devel/659   2/13/09 4:49p pntruong
 * PR52086: Need toggle Hthreshold for RGB input manual phase adjustment.
 *
 * Hydra_Software_Devel/658   2/12/09 1:35p pntruong
 * PR51196:kill color on RF+colorbar signals with low-level color burst.
 *
 * Hydra_Software_Devel/pr51196-take1/1   2/12/09 5:22p bcao
 * PR51196:kill color on RF+colorbar signals with low-level color burst
 *
 * Hydra_Software_Devel/657   2/10/09 5:40p pntruong
 * PR51904 : Adjust threshold, counter and condition to prevent the false
 * detection of bi-level component when switching between 59.94Hz and
 * 60Hz format of Master 3233/1025D.
 *
 * Hydra_Software_Devel/PR51904/1   2/10/09 2:31p chengli
 * PR51904 : adjust threshold, counter and condition to prevent the false
 * detection of bi-level component when switching between 59.94Hz and
 * 60Hz format of Master 3233/1025D.
 *
 * Hydra_Software_Devel/656   2/7/09 4:52p pntruong
 * PR51810: Used the same round off as the one in multibuffering.
 *
 * Hydra_Software_Devel/655   2/3/09 7:19a pntruong
 * PR50097: Improved format detection for 1080p@24hz.
 *
 * Hydra_Software_Devel/pr50097-take2/1   2/3/09 5:46p bcao
 * pr50097:invalid format on component input when using 1080p24+full white
 * pattern
 *
 * Hydra_Software_Devel/654   2/2/09 5:03p yuxiaz
 * PR45484: Updated Dither settings for VDEC_BE.
 *
 * Hydra_Software_Devel/653   1/23/09 6:22p pntruong
 * PR49561: Vertical black bar in ATV snow noise.
 *
 * Hydra_Software_Devel/652   1/21/09 11:33p pntruong
 * PR51336, PR51337, PR51338:  Enabled long/short field fix.  Enabled
 * secam yc separation filter bypass. Enabled full 11-bit full precision.
 *
 * Hydra_Software_Devel/651   1/20/09 7:59p pntruong
 * PR51306: PI changes needed for case when CVBS or RF with no color
 * burst.
 *
 * Hydra_Software_Devel/650   1/20/09 7:53p pntruong
 * PR51196 : Tuned color busrt threshold for RF PALM.
 *
 * Hydra_Software_Devel/649   1/19/09 4:09p pntruong
 * PR51239, PR51240, : Support SW auto phase for RGB input some high pixel
 * frequency format. Add the phase bin into the status.  Increase line
 * length delta (format detection) capability.
 *
 * Hydra_Software_Devel/648   1/16/09 3:54p pntruong
 * PR50097: Toggle SLICE_LPF_HPLL_LOCK_CTL to improved detection of full
 * white pattern.
 *
 * Hydra_Software_Devel/647   1/11/09 7:25p pntruong
 * PR45333, PR50971: Initialized vdec blank level.
 *
 * Hydra_Software_Devel/646   1/7/09 3:58p pntruong
 * PR50365: Need ADC EXT Bypass on all the time.
 *
 * Hydra_Software_Devel/645   1/7/09 11:47a pntruong
 * PR49556: Initialize vdc with adc/pll power off.
 *
 * Hydra_Software_Devel/644   1/5/09 8:23p pntruong
 * PR50722, PR50724: Improve the speed of RGB input auto phase adjustment.
 *
 * Hydra_Software_Devel/643   1/5/09 7:36p pntruong
 * PR50658: Upated ifd audio adc mux selection.
 *
 * Hydra_Software_Devel/642   1/5/09 7:27p pntruong
 * PR50718 : Don't entering into color burst checking when current format
 * is SECAM.
 *
 * Hydra_Software_Devel/641   1/5/09 7:09p pntruong
 * PR50435: Improved locking to weak signal.
 *
 * Hydra_Software_Devel/640   1/5/09 6:47p pntruong
 * PR49165: FD is not stable when meet some RF channels.
 *
 * Hydra_Software_Devel/639   12/18/08 8:46p pntruong
 * PR50502: Used correct display/source rate for computing the number of
 * buffers.
 *
 * Hydra_Software_Devel/638   12/15/08 5:15p pntruong
 * PR50307: Sometimes autodetection failed to detect PAL-Nc RF channels.
 *
 * Hydra_Software_Devel/PR50307/1   12/12/08 10:47p xkliu
 * PR50307: [customer] sometimes autodetection failed to detect PAL-Nc RF
 * channels.
 *
 * Hydra_Software_Devel/637   12/12/08 1:44p pntruong
 * PR50064 : Restored last secam settings, until further qualification of
 * the new values from hw.
 *
 * Hydra_Software_Devel/636   12/11/08 8:05p pntruong
 * PR50097: Iinvalid format on component input when using 1080p24+full
 * white pattern.
 *
 * Hydra_Software_Devel/635   12/10/08 1:44p pntruong
 * PR49849: Auto detection for no burst signal.
 *
 * Hydra_Software_Devel/634   12/8/08 7:42p pntruong
 * PR50064 : Fine tuned registers for PAL/SECAM.
 *
 * Hydra_Software_Devel/633   12/8/08 7:29p pntruong
 * PR49561: PR49849: 1) Rewrite workaround for vertical black bar on the
 * white noise channel. 2) Fine tuned the threshold for kill color and no
 * burst judgement.
 *
 * Hydra_Software_Devel/632   12/8/08 6:31p pntruong
 * PR48892: Addtional fine tuned of VDEC_CD_0.VAR_DELAY and band limitting
 * filter.
 *
 * Hydra_Software_Devel/PR48892/2   12/8/08 3:50p xkliu
 * PR48892:[customer]VDEC: update driver to tune PQ settings on 3549b0
 * platform
 * update default VDEC_CD_0.VAR_DELAY.
 *
 * Hydra_Software_Devel/PR48892/1   12/8/08 12:00p chengli
 * PR48892 : set the band limit filter paramters according to the Excel
 * file.
 *
 * Hydra_Software_Devel/631   12/3/08 10:58a pntruong
 * PR49896, PR49849, PR49561: 1) Select format specified from user when
 * the signal doesn't contain burst. 2)fix auto detection issue fo no
 * burst case. 3) fix typo for PR49561.
 *
 * Hydra_Software_Devel/630   12/1/08 6:28p pntruong
 * PR49556: Powerdown additional bits in adc/pll.
 *
 * Hydra_Software_Devel/629   12/1/08 4:43p pntruong
 * PR48892: Use fine tuned value for bandlimit filter setting.
 * PR49257: PR49304: 1) component input 1080p/23.97 and 1080p/24 locked
 * slower than other 1080p formats. 2) It takes long time before HPLL get
 * locked for Component inuput, 1080p@60Hz, full white pattern.
 * PR49192: Remove the workaround for PR40810.
 * PR49561: [VDEC]: Vertical black bar on the white noise channel
 * PR48639: [customer]1line on/off pattern is bluring on component 1080p
 * signals.  Change the bandlimit filter for 1080P component.
 *
 * Hydra_Software_Devel/PR48639/5   12/1/08 5:27p mlei
 * PR48892: use fine tuned value for bandlimit filter setting.
 *
 * Hydra_Software_Devel/PR48639/3   11/28/08 8:37p mlei
 * PR49192: remove the workaround for PR40810.
 *
 * Hydra_Software_Devel/628   11/25/08 6:28p albertl
 * PR49414: Fixed memory leak in BVDC_P_Vdec_Create when scratch registers
 * allocation fails.
 *
 * Hydra_Software_Devel/627   11/20/08 6:30p pntruong
 * PR46325: Change some register and toggle VDEC_FE_0_FIELD_SYNC.TYPE when
 * manual clock.
 *
 * Hydra_Software_Devel/626   11/19/08 11:20p pntruong
 * PR49257: Some more fine tuning for the mute time.
 *
 * Hydra_Software_Devel/PR47952/23   11/19/08 10:30p mlei
 * PR49257: some more fine tuning for the mute time.
 *
 * Hydra_Software_Devel/625   11/19/08 8:45p pntruong
 * PR43661, PR48734: Reset be when oversample is toggle.
 *
 * Hydra_Software_Devel/624   11/19/08 10:12a pntruong
 * PR49258: Unknown rts config id printed while switching cvbs input
 * timing.
 *
 * Hydra_Software_Devel/623   11/17/08 9:48p pntruong
 * PR48892: Allow customization of backend band limiting filter. Refactors
 * blanklevel.
 *
 * Hydra_Software_Devel/622   11/17/08 2:57p pntruong
 * PR49192: [PQ]cross luma problem when change menu on NTSC CVBS/RF
 * inputs.
 * PR47773: Breaking H sync in Component input when connecting RGB input
 * simultaneously.
 *
 * Hydra_Software_Devel/621   11/14/08 6:09p pntruong
 * PR47952: PR48969: VDEC: Support HD component signals with bi-level
 * hsync.
 *
 * Hydra_Software_Devel/PR47952/18   11/14/08 5:45p mlei
 * PR48969: VDEC: Support HD component signals with bi-level hsync
 *
 * Hydra_Software_Devel/620   11/10/08 1:17p pntruong
 * PR48804: No longer apply 108mhz restriction for sif.  Now run at
 * 216mhz.
 *
 * Hydra_Software_Devel/619   11/10/08 10:03a pntruong
 * PR48676: Fix scart detection issue.
 *
 * Hydra_Software_Devel/618   11/7/08 4:58p pntruong
 * PR46500: fix the side effect of the new setting of vcr for normal cvbs.
 *
 * Hydra_Software_Devel/PR47952/15   11/7/08 4:40p mlei
 * PR46500: fix the side effect of the new setting of vcr for normal cvbs.
 *
 * Hydra_Software_Devel/617   11/6/08 8:03p pntruong
 * PR48538: Don't need clear subcarrier when hpll unlocked.
 * PR46500, PR48679:1) Some fine tuning for PAL/SECAM VCR 2) fine tuing
 * kill color threshold.
 *
 * Hydra_Software_Devel/616   11/5/08 9:55p pntruong
 * PR47820: Fined tune DEC_CD_0_VAR_DELAY.
 *
 * Hydra_Software_Devel/615   11/5/08 9:27p pntruong
 * PR47820: Rolled back the delay values.  Needed additional tuning.
 *
 * Hydra_Software_Devel/614   11/5/08 6:23p pntruong
 * PR47820, PR48266, PR48639: Fine tunnig some reigsters for cumstomer
 * issue.1: color delay 2: bandlimit filter 3: HSYNC_CTL_DISABLE to
 * improve HPLL lock.
 *
 * Hydra_Software_Devel/613   11/4/08 12:14p pntruong
 * PR47827: change the HStart of SECAM to same with PAL-G.
 *
 * Hydra_Software_Devel/612   11/3/08 10:53a pntruong
 * PR47982: More fine tuning video ready threshold for different input
 * source.
 *
 * Hydra_Software_Devel/611   10/31/08 4:09p pntruong
 * PR47013: Used defaul rdb value vbi decimate.
 *
 * Hydra_Software_Devel/610   10/30/08 7:42p pntruong
 * PR46325: RA can not get locked after remapping in VGA input.
 *
 * Hydra_Software_Devel/609   10/29/08 3:25p pntruong
 * PR48192: Need to retain the settings of ATSC_AA_FILTER for the full
 * sample rate mode.
 *
 * Hydra_Software_Devel/608   10/28/08 9:30p pntruong
 * PR47895: Send out source pending callback when video is stable.
 *
 * Hydra_Software_Devel/607   10/25/08 1:29p pntruong
 * PR47747: Added ability to bypass adc common mode.
 *
 * Hydra_Software_Devel/606   10/24/08 10:46p pntruong
 * PR45827, PR48400: Fine tune settings.
 *
 * Hydra_Software_Devel/605   10/24/08 6:18p pntruong
 * PR48192: Enable full sample rate and enable 256 phase.
 *
 * Hydra_Software_Devel/PR47952/9   10/23/08 4:47p mlei
 * PR48192: enable full sample rate and 256 phase.
 *
 * Hydra_Software_Devel/604   10/22/08 10:38p pntruong
 * PR48248: Need change ADC5 clock setting to CLK_PC.
 *
 * Hydra_Software_Devel/603   10/22/08 10:10p pntruong
 * PR48192: Backed this pr; has side-effects on pc input and detection.
 * Need further investigation.
 *
 * Hydra_Software_Devel/602   10/22/08 8:36p pntruong
 * PR48192: Enable full sample rate and enable 256 phase.  216M sample
 * rate in FE
 * PR45566: fix hang up issue in PAL, PAL-Nc Legacy Save mode.
 * PR48196: B0: Fine tuning Y/C delay in 3549B0.
 * PR43517: Fine tune secam auto detection on 3549B0.
 *
 * Hydra_Software_Devel/PR47952/8   10/22/08 7:30p mlei
 * PR48192: Enable full sample rate and enable 256 phase.
 *
 * Hydra_Software_Devel/601   10/20/08 7:37p darnstein
 * PR41782: VBI start delay problem is gone in new VDEC.
 *
 * Hydra_Software_Devel/600   10/20/08 10:55a pntruong
 * PR46735: Clarify the dr/db firstline from hw.
 *
 * Hydra_Software_Devel/599   10/18/08 10:40p pntruong
 * PR43517, PR48044: fine tune secam auto detection on 3549B0.  Fine tune
 * secam video quality on B0.
 *
 * Hydra_Software_Devel/PR47952/4   10/18/08 5:56p mlei
 * PR43517: fine tune secam auto detection on 3549B0
 *
 * Hydra_Software_Devel/PR47952/3   10/18/08 4:19p mlei
 * PR48044: B0: fine tune secam video quality on B0
 *
 * Hydra_Software_Devel/598   10/17/08 1:49p pntruong
 * PR48034: remove the workaournd for PR43823: Fh test by ShibaSoku-color
 * flashing on B0.
 *
 * Hydra_Software_Devel/PR47952/2   10/17/08 12:16p mlei
 * PR48034: remove the workaournd for PR43823: Fh test by ShibaSoku-color
 * flashing on B0
 *
 * Hydra_Software_Devel/597   10/16/08 5:25p pntruong
 * PR47952: PR47953: fix FD patch selection.
 *
 * Hydra_Software_Devel/PR47952/1   10/16/08 4:08p mlei
 * PR47952: PR47953: fix FD patch selection
 *
 * Hydra_Software_Devel/596   10/8/08 12:18p pntruong
 * PR46735: B0: Need SECAM Dr/Db adjustment implemented.
 *
 * Hydra_Software_Devel/595   10/7/08 7:55p pntruong
 * PR47013: Added all the new vdec controls for b0 with default values
 * from rdb.
 *
 * Hydra_Software_Devel/594   9/26/08 1:33p pntruong
 * PR46515: Adapted to new power management.
 *
 * Hydra_Software_Devel/593   9/25/08 7:34p pntruong
 * PR46325: Fine tune the RA lock status, and improved locking with
 * remapped formats.
 *
 * Hydra_Software_Devel/592   9/25/08 7:12p pntruong
 * PR45214: Add support of customization for cvbs auto detection.
 *
 * Hydra_Software_Devel/591   9/23/08 10:47a pntruong
 * PR46191: Fine tuning burst amp delta threshold for secam detection from
 * reference board.
 *
 * Hydra_Software_Devel/590   9/22/08 3:58p pntruong
 * PR46796: Fine tuning HPLL setting for CVBS, Svideo and analog RF.
 *
 * Hydra_Software_Devel/589   9/18/08 11:09a pntruong
 * PR46600: Fixed the in on/off for sif adc.
 *
 * Hydra_Software_Devel/588   9/17/08 1:27p pntruong
 * PR46836: Only reset vdec_yc when needed to avoid unneeded flash.
 *
 * Hydra_Software_Devel/587   9/17/08 11:48a pntruong
 * PR47013: Initial vdec b0 build support.
 *
 * Hydra_Software_Devel/586   9/16/08 10:30a pntruong
 * PR43517: Enable 2d comb for secam auto detection.
 *
 * Hydra_Software_Devel/585   9/15/08 11:52p pntruong
 * PR43661, PR45872, PR45751: Fixed warnings.
 *
 * Hydra_Software_Devel/584   9/15/08 11:19p pntruong
 * PR43661, PR45872, PR45751: 1) use BURST_AMP_DELTA to improve PAL-60/
 * PAL-M auto detection. 2) use BURST_AMP_DELTA to improve secam auto
 * detection. 3) clear subcarrier for NTSC format as well to fix error
 * detection when switch from PAL-60 to NTSC. 4) disable 2D comb for
 * Secam format and use the YC seperation feature in CD secam module.
 *
 * Hydra_Software_Devel/583   9/15/08 10:39p pntruong
 * PR46514: Fixed typo.
 *
 * Hydra_Software_Devel/582   9/15/08 9:17p pntruong
 * PR46514: Upon destroyed and disabled slot disable adc if not used.
 *
 * Hydra_Software_Devel/581   9/10/08 5:14p pntruong
 * PR46514: Dynamic power management of VAFEs (ADCs).
 *
 * Hydra_Software_Devel/580   9/3/08 7:07p pntruong
 * PR43635: Allowed user specify noise and vcr status.
 *
 * Hydra_Software_Devel/579   9/3/08 11:01a pntruong
 * PR46422: Fined tune ntsc443 2dcomb settings.
 *
 * Hydra_Software_Devel/578   9/2/08 8:15p pntruong
 * PR45219: Fine tune 108Mhz pll settings.
 *
 * Hydra_Software_Devel/577   9/2/08 6:06p pntruong
 * PR46314, PR46316: Updated 3dcomb memory allocation, and added buffers
 * alloc debug messages.
 *
 * Hydra_Software_Devel/576   8/29/08 9:35a pntruong
 * PR46260: Fine tune pll settings for 79.5MHz and 85.5MHz.
 *
 * Hydra_Software_Devel/575   8/28/08 11:16a pntruong
 * PR45182: Additional cti tuning for ntsc and else cases.
 *
 * Hydra_Software_Devel/574   8/28/08 10:53a pntruong
 * PR43775: The color is not correct when toggle video format for Svideo
 * PAL-60 input.
 *
 * Hydra_Software_Devel/573   8/27/08 10:27a pntruong
 * PR46163: Video is divided into 3 small windows in SCART0.
 *
 * Hydra_Software_Devel/PR45751/2   8/27/08 8:14p mlei
 * PR46163: Video is divided into 3 small windows in SCART0
 *
 * Hydra_Software_Devel/572   8/22/08 11:03a rpan
 * PR45979: More cleanup for removing VBI passthru support.
 *
 * Hydra_Software_Devel/571   8/22/08 10:28a rpan
 * PR45979: Remove VBI passthru support.
 *
 * Hydra_Software_Devel/570   8/18/08 7:52p pntruong
 * PR45827:  Fixed K_STOP setting for pc input.
 *
 * Hydra_Software_Devel/569   8/18/08 6:32p pntruong
 * PR45827: [PQ] Need to update BLF/AA filter settings for oversampling
 * and 1080p in VDEC.
 *
 * Hydra_Software_Devel/568   8/13/08 10:22a pntruong
 * PR45566: Fixed triggering of get yc table.
 *
 * Hydra_Software_Devel/567   8/12/08 9:31p pntruong
 * PR45566: Added support for standard (legacy) mode of combing.
 *
 * Hydra_Software_Devel/566   8/11/08 8:57p pntruong
 * PR45333: Set correct default pedestal settings according programming
 * spreadsheet.  Default sync on master channel instead of all-channel
 * for non-pc input.
 *
 * Hydra_Software_Devel/565   8/11/08 5:08p pntruong
 * PR45576: Wrong signal present information for component.
 *
 * Hydra_Software_Devel/PR45214/4   8/11/08 5:37p xkliu
 * PR45576: Wrong signal present information for Component
 *
 * Hydra_Software_Devel/PR45214/3   8/6/08 5:42p xkliu
 * PR45401: Shibasoku test with color bar and cross hatch.
 *
 * Hydra_Software_Devel/PR45214/2   8/5/08 6:01p mlei
 * PR45214: allow user to customize the formats for auto detection of
 * analog rf input source
 *
 * Hydra_Software_Devel/558   8/4/08 1:40p pntruong
 * PR45360: Updated csc for pc input.
 *
 * Hydra_Software_Devel/557   7/31/08 11:59a pntruong
 * PR45182: Updated to reflect new CTI tuing values from scripts.
 *
 * Hydra_Software_Devel/556   7/30/08 7:22p pntruong
 * PR45280: Update Combing setting for artifact on test disc.
 *
 * Hydra_Software_Devel/555   7/30/08 10:13a pntruong
 * PR40213: Fine tuned the hstart of auto raster, and use macro to improve
 * code readability.
 *
 * Hydra_Software_Devel/554   7/29/08 10:41p pntruong
 * PR40213: Fixed typo.
 *
 * Hydra_Software_Devel/553   7/29/08 7:49p pntruong
 * PR40213: Used correct alignment macro.
 *
 * Hydra_Software_Devel/552   7/29/08 5:18p pntruong
 * PR45174, PR40213: Fine tuned auto raster settings.  Used FD for video
 * detect for pc input.
 *
 * Hydra_Software_Devel/551   7/28/08 6:26p pntruong
 * PR45182: More fine tuned of cti settings.
 *
 * Hydra_Software_Devel/550   7/23/08 4:44p pntruong
 * PR40213: Added the new reset of auto raster bit.
 *
 * Hydra_Software_Devel/549   7/22/08 11:07a pntruong
 * PR45024: Make sure the ulHBlank is right in GetInputStatus for VGA
 * input.
 *
 * Hydra_Software_Devel/548   7/18/08 11:00a pntruong
 * PR44222: [PC] Video can't be locked (video mute) when switch PC timing
 * from other timing to 800x600@56 or 800x600@60.  Introduce delay after
 * reset.
 *
 * Hydra_Software_Devel/547   7/16/08 7:15p pntruong
 * PR43660: Updated the hstart 640x480p to reflect standard spec.
 *
 * Hydra_Software_Devel/546   7/16/08 9:58a pntruong
 * PR44808: Remove DCR hand over for CVBS as well.
 *
 * Hydra_Software_Devel/545   7/15/08 10:39p pntruong
 * PR44832: Corrected internal ifd settings.
 *
 * Hydra_Software_Devel/544   7/15/08 3:45p pntruong
 * PR44802: Fixed typo.
 *
 * Hydra_Software_Devel/543   7/14/08 5:44p pntruong
 * PR44802: Optimize SECAM settings for VCR
 *
 * Hydra_Software_Devel/542   7/11/08 11:04a pntruong
 * PR44695: Fine tune band limit filter and deringing setting for VDEC
 * source.
 *
 * Hydra_Software_Devel/541   7/9/08 11:05a pntruong
 * PR44237, PR44389: 1) set MASK_EQ_SYNC to 1 to avoid VCR false detection
 * issue. 2) disable CHROMA_ALIASING_3D to keep consistent with 3563.
 *
 * Hydra_Software_Devel/540   7/8/08 3:33p pntruong
 * PR44484, PR44483: Fine tune settings for component 720p locking.
 *
 * Hydra_Software_Devel/539   7/7/08 4:44p pntruong
 * PR43660: Account for hstart shift in auto raster block, and use macro
 * to indicated the adjustments.
 *
 * Hydra_Software_Devel/538   6/27/08 10:19a pntruong
 * PR43660: Fixed build errors for 3563.
 *
 * Hydra_Software_Devel/537   6/27/08 9:46a pntruong
 * PR43660: VDC: Need to check VGA settings and correct for horizontal
 * shift. Fix the shift problem cause by RA setting.
 *
 * Hydra_Software_Devel/536   6/26/08 7:41p pntruong
 * PR44243: Don't ADC Common Mode in 3548/3563D0.
 *
 * Hydra_Software_Devel/535   6/26/08 4:07p pntruong
 * PR41529: [VDEC] PCIN: 1024x768i@43Hz can't be displayed correctly.
 *
 * Hydra_Software_Devel/534   6/26/08 11:21a pntruong
 * PR44203: [VDEC][PC]:Ensure the H positon is right when finding new
 * format, adjusting manua phase and adjusting manual clock. Change some
 * parameters for 93549/93556.
 *
 * Hydra_Software_Devel/533   6/25/08 3:07p pntruong
 * PR40922: Additional scart tunings.
 *
 * Hydra_Software_Devel/532   6/22/08 9:42a pntruong
 * PR40213: Fine tune settings for PC raster adjustment.
 *
 * Hydra_Software_Devel/531   6/20/08 2:06p pntruong
 * PR43821, PR43823: Fh test by ShibaSoku-color flashing.
 *
 * Hydra_Software_Devel/530   6/18/08 3:06p syang
 * PR35549: 1). src-pending when new win is created; 2). don't do
 * BufferHeap_AllocateBuffers as bReConfigSrc
 *
 * Hydra_Software_Devel/529   6/17/08 7:18p pntruong
 * PR40922: Some more tuning for SCART input.
 *
 * Hydra_Software_Devel/528   6/13/08 8:12p pntruong
 * PR34711: Updated adc/ifd select.   Internal Rf can show video now.
 *
 * Hydra_Software_Devel/527   6/12/08 10:14p pntruong
 * PR40604: PAL tuning.
 *
 * Hydra_Software_Devel/526   6/12/08 9:11p pntruong
 * PR40963: Correctly initial gamemode var to in sync with 3dcomb mode.
 *
 * Hydra_Software_Devel/525   6/12/08 12:31a pntruong
 * PR37241: Fine tune yc settings.
 *
 * Hydra_Software_Devel/524   6/11/08 9:17p pntruong
 * PR34711: Updated video input mapping for 93556.
 *
 * Hydra_Software_Devel/523   6/11/08 4:38p pntruong
 * PR34711: Fine tune format detect.
 *
 * Hydra_Software_Devel/522   6/11/08 10:27a tdo
 * PR43119: Use atomic register access
 *
 * Hydra_Software_Devel/521   6/10/08 11:01p pntruong
 * PR34711: Need to also set adc3 to use pll even it's not one of the
 * input pin.  Similar to prevous version where we need adc_6.
 *
 * Hydra_Software_Devel/520   6/10/08 7:37p hongtaoz
 * PR43455, PR43456: added ~6.5 usec delay between frame comb disable and
 * YC reset to avoid memory corruption; added sample count tolerance to
 * 3D comb buffer size;
 *
 * Hydra_Software_Devel/519   6/10/08 1:25p pntruong
 * PR34711: Bringing up pc input.
 *
 * Hydra_Software_Devel/518   6/9/08 7:33p pntruong
 * PR34711: Re-enabled 216mhz clock.
 *
 * Hydra_Software_Devel/517   6/9/08 4:56p hongtaoz
 * PR43455, PR43456: added sw workaround for 3548 3D comb; changed 3548 3D
 * comb buffer allocation math to reflect hw design;
 *
 * Hydra_Software_Devel/516   6/9/08 4:17p pntruong
 * PR34806, PR40963: Fine tuned secam and 3dcomb settings.
 *
 * Hydra_Software_Devel/515   6/7/08 4:49p pntruong
 * PR41315: Additional secam tunings.
 *
 * Hydra_Software_Devel/514   6/7/08 7:40a pntruong
 * PR40963: Fixed 3563 build errors.
 *
 * Hydra_Software_Devel/513   6/6/08 7:28p pntruong
 * PR40963: Intial 3DComb bringup.
 *
 * Hydra_Software_Devel/512   6/5/08 11:05p pntruong
 * PR43373: Fixed build errors.
 *
 * Hydra_Software_Devel/511   6/5/08 10:15p pntruong
 * PR43373: Fixed leak scratch registers allocations.
 *
 * Hydra_Software_Devel/510   6/5/08 8:42p pntruong
 * PR41315: Initial secam bringup.
 *
 * Hydra_Software_Devel/509   6/5/08 3:31p pntruong
 * PR34711: Disabled common mode.
 *
 * Hydra_Software_Devel/508   6/5/08 3:06p pntruong
 * PR34711: Initial 3548 vdec bringup and added more default for new bits.
 *
 * Hydra_Software_Devel/507   5/30/08 7:41p pntruong
 * PR39155: Need to implement de-ringing filters in BVN.  Also init some
 * new fields to default.
 *
 * Hydra_Software_Devel/506   5/29/08 4:41p pntruong
 * PR40449: [VDEC] support 1080i component signals with bi-level hsync.
 *
 * Hydra_Software_Devel/505   5/28/08 4:57p pntruong
 * PR36323, PR40810: Fine tune format detection for component, and fine
 * tunes comb settings for pal-m.
 *
 * Hydra_Software_Devel/504   5/20/08 2:05p pntruong
 * PR42424: Fine tuned dcr settings for component/pc input.
 *
 * Hydra_Software_Devel/503   5/16/08 3:06p pntruong
 * PR34711: Updated to include new inputs.
 *
 * Hydra_Software_Devel/502   5/13/08 10:14a pntruong
 * PR42424: Generalized the digital gain initialization.
 *
 * Hydra_Software_Devel/501   5/12/08 8:36p pntruong
 * PR42416: Isolated for 625 lines formats, and also include vbi start.
 *
 * Hydra_Software_Devel/500   5/12/08 2:56p darnstein
 * PR42416: Implement Grossman's work-around for the conflict between VDEC
 * frame comb filter and teletext decoder (TTD).
 *
 * Hydra_Software_Devel/499   5/9/08 10:05a pntruong
 * PR42424: Make sure to input change triggers the new fine tuned values.
 *
 * Hydra_Software_Devel/498   5/8/08 5:54p pntruong
 * PR35549: Added more preparations for source pending.
 *
 * Hydra_Software_Devel/497   5/7/08 11:28a pntruong
 * PR35549: Hook up the source callback event first so that app can start
 * using it while adding the internal pending.
 *
 * Hydra_Software_Devel/496   5/5/08 10:43a pntruong
 * PR42424: Fine tune dcr and digital gain.
 *
 * Hydra_Software_Devel/495   5/2/08 5:47p pntruong
 * PR34806: More scart input works.
 *
 * Hydra_Software_Devel/494   5/1/08 2:38p rpan
 * PR42109: Initial work for SECAM variants.
 *
 * Hydra_Software_Devel/493   5/1/08 10:38a pntruong
 * PR42070: Optimized PLL setting for pc input.
 *
 * Hydra_Software_Devel/492   4/29/08 7:48p pntruong
 * PR35549: Make room for source pending dirty bit.
 *
 * Hydra_Software_Devel/491   4/28/08 5:34p pntruong
 * PR39159: Implement pass-through path from ADC to DAC.
 *
 * Hydra_Software_Devel/490   4/25/08 7:10p darnstein
 * PR23201: Now change the VBI HSTART to 0xe6.
 *
 * Hydra_Software_Devel/489   4/25/08 6:38p pntruong
 * PR40963: [VDEC] Add 3D Comb Game Mode Support.
 *
 * Hydra_Software_Devel/488   4/23/08 4:04p pntruong
 * PR40213, PR41315, PR41529: Added initial support SECAM and pc auto
 * raster.
 *
 * Hydra_Software_Devel/487   4/21/08 11:15p pntruong
 * PR34711: Prepared to add new features into vdec.  Removed vdec1
 * support.
 *
 * Hydra_Software_Devel/486   4/21/08 5:37p pntruong
 * PR38861: Improved autophase settings.
 *
 * Hydra_Software_Devel/485   4/21/08 5:01p pntruong
 * PR41470, PR32746: Merged in fixes.
 *
 * ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bdbg.h"
#include "bvdc_priv.h"
#include "bvdc_vdec_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_window_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bvdc_656in_priv.h"
#include "bvdc_csc_priv.h"
#include "bchp_common.h"

#if (BVDC_P_SUPPORT_VDEC)

/* VDEC Registers definitions access */
#include "bchp_adc_1.h"
#include "bchp_adc_2.h"
#include "bchp_adc_3.h"
#include "bchp_adc_4.h"
#if (BVDC_P_MAX_PHYSICAL_ADC >= 5)
#include "bchp_adc_5.h"
#endif
#if (BVDC_P_MAX_PHYSICAL_ADC >= 6)
#include "bchp_adc_6.h"
#endif

#include "bchp_pll.h"
#include "bchp_fd_0.h"
#include "bchp_pc_in_0.h"
#include "bchp_vd_top_0.h"
#include "bchp_vdec_cd_0.h"
#include "bchp_vdec_fe_0.h"
#include "bchp_vdec_be_0.h"
#include "bchp_vdec_yc_0.h"
#include "bchp_sun_top_ctrl.h"

/* Passthru BVN */
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4) /* 3548Ax */
#include "bchp_vd_passthrough_0.h"
#endif


BDBG_MODULE(BVDC_VDEC);
BDBG_OBJECT_ID(BVDC_DEC);


/* Static internal function forward declaration. */
static void BVDC_P_Vdec_GetFmtCfg_isr
	( const BFMT_VideoInfo            *pFmtInfo,
	  const BVDC_Vdec_3DGameMode       e3DCombGameMode,
	  const BVDC_P_CdCfgEntry        **ppCdCfg,
	  const BVDC_P_YcCfgEntry        **ppYcCfg,
	  const BVDC_P_PcInCfgEntry      **ppPcInCfg );

/****************************************************************************/
/* Number of entries use this to loop, and/or allocate memory.              */
/****************************************************************************/
#define BVDC_P_PLL_COUNT \
	(sizeof(s_aPllClkTbl)/sizeof(BVDC_P_PllEntry))

#define BVDC_P_ATSC_FDT_COUNT \
	(sizeof(s_aAtscFdtFmtTbl)/sizeof(BVDC_P_FdtEntry))

#define BVDC_P_VESA_FDT_COUNT \
	(sizeof(s_aVesaFdtFmtTbl)/sizeof(BVDC_P_FdtEntry))

#define BVDC_P_ADC_COUNT \
	(sizeof(s_aVdec0AdcSelTbl)/sizeof(BVDC_P_AdcEntry))

#define BVDC_P_VGA_COUNT \
	(sizeof(s_aPcInCfgTbl)/sizeof(BVDC_P_PcInCfgEntry))

#define BVDC_P_CD_COUNT \
	(sizeof(s_aCdCfgTbl)/sizeof(BVDC_P_CdCfgEntry))

#define BVDC_P_YC_COUNT \
	(sizeof(s_aYcCfgTbl)/sizeof(BVDC_P_YcCfgEntry))


/****************************************************************************/
/* Making an entry for PLL Rate                                             */
/****************************************************************************/
#define BVDC_P_MAKE_PLL(freq, NDIV_MODE, M, N, P1, P2, FCW_OFFSET, BYEN_PHI,\
	VCO_RANGE, ICPX, I, NDVIMUX_SEL)                                        \
{                                                                           \
	BVDC_P_Pll_##freq,                                                      \
	(M), (N), (P1), (P2), (FCW_OFFSET),(BYEN_PHI),                          \
	(BCHP_PLL_DIV_CTL_VCO_RANGE_##VCO_RANGE),                               \
	(ICPX), (I), (NDVIMUX_SEL),                                             \
	&s_aPllNDivModeTbl[BVDC_P_##NDIV_MODE],                                 \
	(#freq)                                                                 \
}


/****************************************************************************/
/* Making an entry for PLL_CTL_0                                            */
/****************************************************************************/
#define BVDC_P_MAKE_NMODE(Nmode, PX0, PX1, RZ, CZ, CP, RN, CN, LF_ORDER)    \
{                                                                           \
	BVDC_P_##Nmode,                                                         \
	(PX0), (PX1), (RZ), (CZ), (CP), (RN), (CN), (LF_ORDER),                 \
	(#Nmode)                                                                \
}

/* Line per field work-around for 3563A1 Pll issue.!
 * Basically we need to tweak the LPF & LL for format detection to work
 * correctly.  We use these macro to preserved the original parameter as
 * much as we can.  The two param that needed to tweak are:
 *   (1) FD_0_MF_FORMAT_ENTRY_01.FT_LINES_PER_FIELD
 *   (2) FD_0_MF_FORMAT_ENTRY_01.FT_LINE_LENGTH
 *
 * Four lsb bits of this field specify fractional value. Those bits should be
 * set to: 4b'0000 for progressive formats and 1250/50, 4b'1000 for interlaced
 * formats except for 1250/50.
 *
 * Line length is double for PC format! */
#define BVDC_P_RASTER_PROG_MASK                    (1)
#define BVDC_P_LPF_SHIFT                           (4)
#define BVDC_P_LL_FACTOR                           (2)
#define BVDC_P_DELTA_LL_SCALE                      (1)

/* Tweak (1): For both ATSC & PC input cases!  */
#define BVDC_P_FT_LINES_PER_FIELD(lpf, raster_t) \
	(lpf << (BVDC_P_LPF_SHIFT) |  \
	((raster_t & BVDC_P_RASTER_PROG_MASK) ? 0 : 1) << (BVDC_P_LPF_SHIFT-1))

/* Tweak (2): l_length only use for PC input case! */
#define BVDC_P_FT_LINE_LENGTH(l_length, freq)    \
	((BVDC_P_Pll_##freq == BVDC_P_Pll_e216_000)  \
	? (l_length) : (l_length) * BVDC_P_LL_FACTOR)

/* Tweak (3): raster_t only use for PC input case! */
#define BVDC_P_FT_RASTER_TYPE(raster_t, freq)    \
	((BVDC_P_Pll_##freq == BVDC_P_Pll_e216_000)  \
	? (0) : (raster_t))

/* Tweak (4): hstart -6 only use for PC input case! */
#define BVDC_P_FT_HSTART(h_start, freq)    \
	((BVDC_P_Pll_##freq == BVDC_P_Pll_e216_000)  \
	? (h_start) : (h_start - 6))

#define BVDC_P_MAKE_FDT_ATSC(fmt, freq, l_length, lpf, hs_width,            \
	raster_t, hs_pol, vs_pol, delta_ll, delta_lpf, delta_hw, h_start)       \
	BVDC_P_MAKE_FDT(true, fmt, freq, l_length, lpf, hs_width, raster_t,     \
	hs_pol, vs_pol, delta_ll, delta_lpf, delta_hw, h_start)                 \

#define BVDC_P_MAKE_FDT_PC(fmt, freq, l_length, lpf, hs_width,              \
	raster_t, hs_pol, vs_pol, delta_ll, delta_lpf, delta_hw, h_start)       \
	BVDC_P_MAKE_FDT(false, fmt, freq, l_length, lpf, hs_width, raster_t,    \
	hs_pol, vs_pol, delta_ll, delta_lpf, delta_hw, h_start)                 \

/****************************************************************************/
/* Making an entry for FT (Format Table) for format identification block.   */
/****************************************************************************/
#define BVDC_P_MAKE_FDT(bAtsc, fmt, freq, l_length, lpf, hs_width, raster_t,     \
	hs_pol, vs_pol, delta_ll, delta_lpf, delta_hw, h_start)                      \
{                                                                                \
	(bAtsc),                                                                     \
	BFMT_VideoFmt_e##fmt,                                                        \
	{                                                                            \
		/* FD_FORMAT_ENTRY_0 */                                                  \
		BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_00, FT_HSYNC_POLARITY, hs_pol   ) | \
		BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_00, FT_VSYNC_POLARITY, vs_pol   ) | \
		BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_00, FT_DELTA_LL,                    \
			((delta_ll) >> BVDC_P_DELTA_LL_SCALE)                            ) | \
		BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_00, FT_HSYNC_WIDTH,                 \
			((hs_width) << 1)                                                ) | \
		BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_00, FT_DELTA_HW,       delta_hw ) | \
		BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_00, FT_RASTER_TYPE,                 \
			BVDC_P_FT_RASTER_TYPE(raster_t, freq)                            ),  \
		/* FD_FORMAT_ENTRY_1 */                                                  \
		BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_01, FT_LINES_PER_FIELD,             \
			BVDC_P_FT_LINES_PER_FIELD(lpf, raster_t)                         ) | \
		BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_01, FT_DELTA_LPF,      delta_lpf) | \
		BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_01, FT_LINE_LENGTH,                 \
			BVDC_P_FT_LINE_LENGTH(l_length, freq)                            )   \
	},                                                                           \
	(hs_pol),                                    /* FT_HSYNC_POLARITY; */        \
	(vs_pol),                                    /* FT_VSYNC_POLARITY; */        \
	(delta_ll),                                  /* FT_DELTA_LL; */              \
	(hs_width),                                  /* FT_HSYNC_WIDTH; */           \
	(delta_hw),                                  /* FT_DELTA_HW; */              \
	(BVDC_P_FT_RASTER_TYPE(raster_t, freq)),     /* FT_RASTER_TYPE; */           \
	(BVDC_P_FT_LINES_PER_FIELD(lpf, raster_t)),  /* FT_LINES_PER_FIELD; */       \
	(delta_lpf),                                 /* FT_DELTA_LPF; */             \
	(BVDC_P_FT_LINE_LENGTH(l_length, freq)),     /* FT_LINE_LENGTH; */           \
	&s_aPllClkTbl[BVDC_P_Pll_##freq],                                            \
	(BVDC_P_FT_HSTART(h_start, freq))            /* FT_HSTART; */                \
}

/* To trick (CTL_FD_CLAMP_SEL == CTL_FD_CLAMP_0_SEL) */
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4) /* 3548Ax */
#define BCHP_VD_TOP_0_VD_VDEC_0_CTL_FD_CLAMP_SEL_MASK  BCHP_VD_TOP_0_VD_VDEC_0_CTL_FD_CLAMP_0_SEL_MASK
#define BCHP_VD_TOP_0_VD_VDEC_0_CTL_FD_CLAMP_SEL_SHIFT BCHP_VD_TOP_0_VD_VDEC_0_CTL_FD_CLAMP_0_SEL_SHIFT
#define BCHP_VD_TOP_0_VD_VDEC_0_CTL_FD_CLAMP_SEL_VDEC  BCHP_VD_TOP_0_VD_VDEC_0_CTL_FD_CLAMP_0_SEL_VDEC
#define BCHP_VD_TOP_0_VD_VDEC_0_CTL_FD_CLAMP_SEL_FD    BCHP_VD_TOP_0_VD_VDEC_0_CTL_FD_CLAMP_0_SEL_FD
#endif

/* No calibration!  Nor inoff!  Beautify the code. */
#define BCHP_ADC_1_SELECT_ANALOG_INPUT_NONE      (0)
#define BCHP_ADC_1_INOFF_NONE_MASK               (0)
#define BCHP_ADC_1_INOFF_NONE_SHIFT              (0)

/* Dither */
#define BVDC_P_DITHER_VDEC_FE_LFSR_VALUE             (0xC18F9)
#define BVDC_P_DITHER_VDEC_FE_LFSR_CTRL_T0           (0x5)
#define BVDC_P_DITHER_VDEC_FE_LFSR_CTRL_T1           (0x7)
#define BVDC_P_DITHER_VDEC_FE_LFSR_CTRL_T2           (0x5)

#define BVDC_P_DITHER_VDEC_BE_LFSR_VALUE             (0xC18F9)
#define BVDC_P_DITHER_VDEC_BE_LFSR_CTRL_T0           (0x4)
#define BVDC_P_DITHER_VDEC_BE_LFSR_CTRL_T1           (0x1)
#define BVDC_P_DITHER_VDEC_BE_LFSR_CTRL_T2           (0x5)

/****************************************************************************/
/* Making an entry for ADC muxes                                            */
/****************************************************************************/
#define BVDC_P_MAKE_ADC(connector, adc, input_sel, calib, inoff)            \
{                                                                           \
	(connector),                                                            \
	((adc)-1),                                                              \
	(BCHP_ADC_##adc##_REG_START - BCHP_ADC_1_REG_START),                    \
	(BCHP_VD_TOP_0_VD_VDEC_0_CTL_CH0_SELECT_ADC_##adc),                     \
	(BCHP_ADC_##adc##_SELECT_ANALOG_INPUT_##input_sel),                     \
	(BCHP_ADC_1_SELECT_ANALOG_INPUT_##calib),                               \
	(BCHP_ADC_1_INOFF_##inoff##_MASK),                                      \
	(BCHP_ADC_1_INOFF_##inoff##_SHIFT)                                      \
}


/****************************************************************************/
/* Making an entry for PC_IN FMT configurations                             */
/****************************************************************************/
#define BVDC_P_MAKE_VGA(fmt, dcr_ctrl, blank_avg,                           \
	diff_threshold, bAdcAdj, bDcrFilterAdj, bDitherAdj,  bPllAdj)           \
{                                                                           \
	BFMT_VideoFmt_e##fmt,                                                   \
	(dcr_ctrl),                                                             \
	BCHP_PC_IN_0_PHASE_LOCK_CONTROL_BLANK_AVG_##blank_avg,                  \
	(diff_threshold),                                                       \
	(bAdcAdj),                                                              \
	(bDcrFilterAdj),                                                        \
	(bDitherAdj),                                                           \
	(bPllAdj)                                                               \
}

#define BVDC_P_MAKE_ADC_NULL(connector)                                     \
{                                                                           \
	(connector),                                                            \
	BVDC_P_ADC_INVALID,                                                     \
	BVDC_P_ADC_INVALID,                                                     \
	BVDC_P_ADC_INVALID,                                                     \
	BVDC_P_ADC_INVALID,                                                     \
	BVDC_P_ADC_INVALID,                                                     \
	BVDC_P_ADC_INVALID,                                                     \
	BVDC_P_ADC_INVALID                                                      \
}


#define BVDC_P_MAKE_CD(fmt, step, per_cnt, acgc_ref, acgc_start, acgc_end, active) \
{                                                                           \
	BFMT_VideoFmt_e##fmt,                                                   \
	(step),                               /* SUBCARRIER_FREQ.STEP */        \
	(per_cnt),                            /* SUBCARRIER_PERIOD_COUNTER */   \
	(acgc_ref),                           /* ACGC.REFERENCE */              \
	(acgc_start),                         /* ACGC1.FIELD_AVERAGE_START */   \
	(acgc_end),                           /* ACGC1.FIELD_AVERAGE_END */     \
	(active)                              /* CD_CTRL.ACTIVE_REGION_END*/    \
}


#define BVDC_P_MAKE_YC(fmt, coef, line_delay, delta_line, delay_mode,       \
	decimation_ctrl, game_mode, phase_shift, mem_burst_norm,                \
	mem_burst_frame1, mem_burst_frame2, sync_line, active_line,             \
	frame1_offset, frame2_offset)                                           \
{                                                                           \
	BFMT_VideoFmt_e##fmt,                                                   \
	BCHP_VDEC_YC_0_YC_CTRL_FILTER_COEF_SEL_CENTER_FREQ_##coef,              \
	(line_delay),           /* YC_CTRL.LINE_DELAY */                        \
	BCHP_VDEC_YC_0_YC_CTRL_SUBCARRIER_LINE_PHASE_SHIFT_##phase_shift,       \
	(mem_burst_norm),       /* YC_FRAME_STORE.MEMORY_BURST */               \
	(mem_burst_frame1),   /* YC_FRAME_STORE.MEMORY_BURST */                 \
	(mem_burst_frame2),   /* YC_FRAME_STORE.MEMORY_BURST */                 \
	(sync_line),            /* VDEC_YC_0_YC_LOOP_ADJUST, FRAME_SYNC_LINE */ \
	(frame1_offset),        /* VDEC_YC_0_YC_FRAME_OFFSET , FRAME1_OFFSET */ \
	(frame2_offset),        /* VDEC_YC_0_YC_FRAME_OFFSET , FRAME2_OFFSET */ \
	(active_line),          /* VDEC_YC_0_YC_DELTA, LAST_ACTIVE_LINE */      \
	(delta_line),           /* VDEC_YC_0_YC_DELTA, LINE */                  \
	(delay_mode),           /* VDEC_YC_0_YC_CTRL,  FRAME_COMB_DELAY_MODE  */\
	(decimation_ctrl),      /*VDEC_YC_0_YC_CTRL, DECIMATION_CTRL */         \
	BVDC_Vdec_3DGameMode_##game_mode                                        \
}

/* Static PLL_CTL_0 lookup table. */
static const BVDC_P_NModeEntry s_aPllNDivModeTbl[] =
{
	/*                  Nmode     PX0  PX1  RZ  CZ  CP  RN  CN  LF_ORDER*/
	BVDC_P_MAKE_NMODE(eInteger ,  14,  24,  1,  0,  0,  0,  0,  0),
	BVDC_P_MAKE_NMODE(eMash    ,   2,   5,  1,  1,  0,  2,  1,  1),
	BVDC_P_MAKE_NMODE(eMfb     ,   1,   3,  2,  1,  0,  2,  1,  1),
	BVDC_P_MAKE_NMODE(eMash_1_8,  10,  21,  1,  1,  0,  0,  0,  1),
	BVDC_P_MAKE_NMODE(eMfb_1_8 ,   3,   7,  1,  1,  0,  0,  1,  1),
	BVDC_P_MAKE_NMODE(eUnknown ,   2,   2,  2,  2,  0,  2,  1,  1)
};

/* Static PLL lookup table. */
static const BVDC_P_PllEntry s_aPllClkTbl[] =
{
	/*               Freq     Dmode      M   N  P1 P2    fcw  BYEN_PHI
	                                                     VcoRge ICPX
	                                                               I  NDivMux */
	BVDC_P_MAKE_PLL(e13_500 , eMfb,  9, 72 , 2, 1, 0       , 1, F1600, 3, 2, 0),  /* 13.5        */
	BVDC_P_MAKE_PLL(e25_175 , eMfb,  4, 59 , 2, 1, 11309086, 1, F1600, 3, 2, 0),  /* 25.17499999 */
	BVDC_P_MAKE_PLL(e27_000 , eMfb,  4, 96 , 3, 1, 0       , 1, F1600, 3, 2, 0),  /* 27          */
	BVDC_P_MAKE_PLL(e28_320 , eMfb,  4, 67 , 2, 1, 2162397 , 1, F1600, 3, 2, 0),  /* 28.32000001 */
	BVDC_P_MAKE_PLL(e28_324 , eMfb,  4, 67 , 2, 1, 2321470 , 1, F1600, 3, 2, 0),  /* 28.32400001 */
	BVDC_P_MAKE_PLL(e30_240 , eMfb,  4, 71 , 2, 1, 11408507, 1, F1600, 3, 2, 0),  /* 30.2400001 */
	BVDC_P_MAKE_PLL(e31_500 , eMfb,  3, 56 , 2, 1, 0       , 1, F800 , 3, 2, 0),  /* 31.5        */
	BVDC_P_MAKE_PLL(e35_500 , eMfb,  3, 63 , 2, 1, 1864135 , 1, F1600, 3, 2, 0),  /* 35.5        */
	BVDC_P_MAKE_PLL(e36_000 , eMfb,  3, 64 , 2, 1, 0       , 1, F1600, 3, 2, 0),  /* 36          */
	BVDC_P_MAKE_PLL(e38_250 , eMfb,  3, 68 , 2, 1, 0       , 1, F1600, 3, 2, 0),  /* 38.25       */
	BVDC_P_MAKE_PLL(e40_000 , eMfb,  3, 71 , 2, 1, 1864135 , 1, F1600, 3, 2, 0),  /* 40          */
	BVDC_P_MAKE_PLL(e44_900 , eMfb,  2, 79 , 3, 1, 13794600, 1, F800 , 3, 2, 0),  /* 44.90000001 */
	BVDC_P_MAKE_PLL(e49_500 , eMfb,  2, 58 , 2, 1, 11184811, 1, F800 , 3, 2, 0),  /* 49.50000002 */
	BVDC_P_MAKE_PLL(e50_000 , eMfb,  2, 59 , 2, 1, 4349649 , 1, F1600, 3, 2, 0),  /* 50.00000002 */
	BVDC_P_MAKE_PLL(e56_250 , eMfb,  2, 66 , 2, 1, 11184811, 1, F1600, 3, 2, 0),  /* 56.25000002 */
	BVDC_P_MAKE_PLL(e65_000 , eMfb,  1, 38 , 2, 1, 8699297 , 1, F800 , 3, 2, 0),  /* 64.99999998 */
	BVDC_P_MAKE_PLL(e68_250 , eMfb,  1, 60 , 3, 1, 11184811, 1, F800 , 3, 2, 0),  /* 68.25000002 */
	BVDC_P_MAKE_PLL(e71_640 , eMfb,  1, 42 , 2, 1, 7605671 , 1, F800 , 3, 2, 0),  /* 71.64       */
	BVDC_P_MAKE_PLL(e72_000 , eMfb,  1, 64 , 3, 1, 0       , 1, F800 , 3, 2, 0),  /* 72          */
	BVDC_P_MAKE_PLL(e72_014 , eMfb,  1, 64 , 3, 1, 208783  , 1, F800 , 3, 2, 0),  /* 72.01399999 */
	BVDC_P_MAKE_PLL(e73_250 , eMfb,  1, 65 , 3, 1, 1864135 , 1, F800 , 3, 2, 0),  /* 73.24999999 */
	BVDC_P_MAKE_PLL(e74_250 , eMfb,  1, 44 , 2, 1, 0       , 1, F800 , 3, 2, 0),  /* 74.25       */
	BVDC_P_MAKE_PLL(e74_481 , eMfb,  1, 66 , 3, 1, 3444922 , 1, F800 , 3, 2, 0),  /* 74.48100002 */
	BVDC_P_MAKE_PLL(e75_000 , eMfb,  1, 66 , 3, 1, 11184811, 1, F800 , 3, 2, 0),  /* 75.00000002 */
	BVDC_P_MAKE_PLL(e78_750 , eMfb,  1, 70 , 3, 1, 0       , 1, F800 , 3, 2, 0),  /* 78.75       */
	BVDC_P_MAKE_PLL(e79_500 , eMfb,  1, 47 , 2, 1,  1864135, 1, F800 , 3, 2, 0),  /* 79.50000002 */
	BVDC_P_MAKE_PLL(e83_462 , eMfb,  1, 74 , 3, 1, 3161573 , 1, F800 , 3, 2, 0),  /* 83.46199999 */
	BVDC_P_MAKE_PLL(e83_500 , eMfb,  1, 74 , 3, 1, 3728270 , 1, F800 , 3, 2, 0),  /* 83.49999999 */
	BVDC_P_MAKE_PLL(e84_750 , eMfb,  1, 50 , 2, 1, 3728270 , 1, F800 , 3, 2, 0),  /* 84.74999998 */
	BVDC_P_MAKE_PLL(e85_500 , eMfb,  1, 12 , 2, 4, 11184810, 1, F800 , 3, 2, 0),  /* 85.5        */
	BVDC_P_MAKE_PLL(e94_500 , eMfb,  1, 84 , 3, 1, 0       , 1, F800 , 3, 2, 0),  /* 94.5        */
	BVDC_P_MAKE_PLL(e101_000, eMfb,  1, 89 , 3, 1, 13048946, 1, F1600, 3, 2, 0),  /* 101         */
	BVDC_P_MAKE_PLL(e102_250, eMfb,  1, 60 , 2, 1, 9942054 , 1, F1600, 3, 2, 0),  /* 102.25      */
	BVDC_P_MAKE_PLL(e106_500, eMfb,  1, 63 , 2, 1, 1864135 , 1, F1600, 3, 2, 0),  /* 106.5       */
	BVDC_P_MAKE_PLL(e108_000, eMfb,  1, 64 , 2, 1, 0       , 1, F1600, 3, 2, 0),  /* 108         */
	BVDC_P_MAKE_PLL(e117_500, eMfb,  1, 69 , 2, 1, 10563432, 1, F1600, 3, 2, 0),  /* 117.5       */
	BVDC_P_MAKE_PLL(e119_000, eMfb,  1, 105, 3, 1, 13048946, 1, F1600, 3, 2, 0),  /* 119         */
	BVDC_P_MAKE_PLL(e121_750, eMfb,  1, 108, 3, 1, 3728270 , 1, F1600, 3, 2, 0),  /* 121.75      */
	BVDC_P_MAKE_PLL(e125_000, eMfb,  1, 74,  2, 1, 1242757 , 1, F1600, 3, 2, 0),  /* 125.00      */
	BVDC_P_MAKE_PLL(e135_000, eMfb,  1, 60 , 3, 1, 0       , 0, F1600, 3, 2, 0),  /* 135         */
	BVDC_P_MAKE_PLL(e136_750, eMfb,  1, 60 , 3, 1, 13048946, 0, F1600, 3, 2, 0),  /* 136.75      */
	BVDC_P_MAKE_PLL(e138_625, eMfb,  1, 41 , 2, 1, 1242757 , 0, F1600, 3, 2, 0),  /* 138.6250001 */
	BVDC_P_MAKE_PLL(e146_250, eMfb,  1, 43 , 2, 1, 5592405 , 0, F1600, 3, 2, 0),  /* 146.2499999 */
	BVDC_P_MAKE_PLL(e148_500, eMfb,  1, 66 , 3, 1, 0       , 0, F1600, 3, 2, 0),  /* 148.5       */
	BVDC_P_MAKE_PLL(e156_000, eMfb,  1, 69 , 3, 1, 5592405 , 0, F1600, 3, 2, 0),  /* 156         */
	BVDC_P_MAKE_PLL(e157_500, eMfb,  1, 70 , 3, 1, 0       , 0, F1600, 3, 2, 0),  /* 157.5       */
	BVDC_P_MAKE_PLL(e162_000, eMfb,  1, 48 , 2, 1, 0       , 0, F1600, 3, 2, 0),   /* 162        */
	BVDC_P_MAKE_PLL(e216_000, eInteger,  1, 32, 3, 3,     0, 1, F1600, 2, 0, 1),  /* 216         */
	BVDC_P_MAKE_PLL(eUnknown, eUnknown,  0,  0, 0, 0,        0, 0, F1600, 0, 0, 0)
};

/* Add complete list here for both ATSC and PC!  To support a new fo, 1rmat
 * one would find out all necessary parameters and input here., 1
 * Static format lookup table for format identification.  Note some , 1are
 * duplicated (from arch doc), to be removed after testing with chip.
 * INDEX: none, searchable with BFMT_VideoFmt.
 *
 * Note that:
 *   DVI_640x400p_85Hz
 *   DVI_720x400p_85Hz
 * Have very similar parameters, and one of the tie breaker is lpf!  But their
 * value is only differ by 1 (445 vs 446)!  Currently FD would match with the
 * first one in the table.  Hence the parameters is changed to 443 and 448
 * respectively. */
static const BVDC_P_FdtEntry s_aAtscFdtFmtTbl[] =
{
	/*              format                        freq      l     lpf   hs   ip h  v  ll  lpf hw  hstart*/
	/* ATSC formats */
	BVDC_P_MAKE_FDT_ATSC(NTSC,                    e216_000, 6864, 263,  504, 0, 0, 0, 20, 4, 10, 244),
	BVDC_P_MAKE_FDT_ATSC(PAL_G,                   e216_000, 6912, 313,  504, 0, 0, 0, 20, 4, 10, 267),
	BVDC_P_MAKE_FDT_ATSC(PAL_M,                   e216_000, 6864, 263,  504, 0, 0, 0, 20, 4, 10, 244),
	BVDC_P_MAKE_FDT_ATSC(PAL_NC,                  e216_000, 6912, 313,  504, 0, 0, 0, 20, 4, 10, 267),
	BVDC_P_MAKE_FDT_ATSC(NTSC_J,                  e216_000, 6864, 263,  504, 0, 0, 0, 20, 4, 10, 244),
	BVDC_P_MAKE_FDT_ATSC(NTSC_443,                e216_000, 6864, 263,  504, 0, 0, 0, 20, 4, 10, 244),
	BVDC_P_MAKE_FDT_ATSC(PAL_60,                  e216_000, 6864, 263,  504, 0, 0, 0, 20, 4, 10, 244),
	BVDC_P_MAKE_FDT_ATSC(SECAM_L,                 e216_000, 6864, 263,  504, 0, 0, 0, 20, 4, 10, 267),
	BVDC_P_MAKE_FDT_ATSC(480p,                    e216_000, 3432, 525,  252, 1, 0, 0, 20, 2, 10, 244),
	BVDC_P_MAKE_FDT_ATSC(576p_50Hz,               e216_000, 3456, 625,  252, 1, 0, 0, 20, 2, 10, 266),
	BVDC_P_MAKE_FDT_ATSC(720p,                    e216_000, 2400, 750,  58,  1, 0, 0, 20, 2, 10, 378),
	BVDC_P_MAKE_FDT_ATSC(720p_50Hz,               e216_000, 2880, 750,  58,  1, 0, 0, 20, 2, 10, 378),
	BVDC_P_MAKE_FDT_ATSC(1080i,                   e216_000, 3200, 563,  64,  0, 0, 0, 20, 2, 10, 280),
	BVDC_P_MAKE_FDT_ATSC(1080i_50Hz,              e216_000, 3840, 563,  64,  0, 0, 0, 20, 2, 10, 280),
	BVDC_P_MAKE_FDT_ATSC(1080p,                   e216_000, 1600, 1125, 32,  1, 0, 0, 20, 2, 10, 277),
	BVDC_P_MAKE_FDT_ATSC(1080p_24Hz,              e216_000, 4000, 1125, 64,  1, 0, 0, 20, 2, 10, 277),
	BVDC_P_MAKE_FDT_ATSC(1080p_25Hz,              e216_000, 3840, 1125, 32,  1, 0, 0, 20, 2, 10, 277),
	BVDC_P_MAKE_FDT_ATSC(1080p_30Hz,              e216_000, 3200, 1125, 32,  1, 0, 0, 20, 2, 10, 277),
	BVDC_P_MAKE_FDT_ATSC(1080p_50Hz,              e216_000, 1920, 1125, 32,  1, 0, 0, 20, 2, 10, 277),
	BVDC_P_MAKE_FDT_ATSC(1080p_50Hz,              e216_000, 1728, 1250, 48,  1, 0, 0, 20, 2, 10, 277),
	BVDC_P_MAKE_FDT_ATSC(1080i_50Hz,              e216_000, 3456, 625,  96,  0, 0, 0, 20, 2, 10, 280),
	BVDC_P_MAKE_FDT_ATSC(1080i_50Hz,              e216_000, 3480, 562,  64,  0, 0, 0, 20, 2, 10, 280),
};

static const BVDC_P_FdtEntry s_aVesaFdtFmtTbl[] =
{
	/* ATSC formats in pc input */
	BVDC_P_MAKE_FDT_ATSC(NTSC,                    e13_500 , 4095, 440,  504, 0, 0, 0, 20, 4, 10, 119),
	BVDC_P_MAKE_FDT_ATSC(PAL_G,                   e13_500 , 4095, 527,  504, 0, 0, 0, 20, 4, 10, 132),
	BVDC_P_MAKE_FDT_ATSC(576p_50Hz,               e27_000 , 3456, 625,  252, 1, 0, 0, 20, 2, 10, 132),
	BVDC_P_MAKE_FDT_ATSC(720p,                    e74_250 , 2400, 750,  58,  1, 0, 0, 20, 2, 10, 260),
	BVDC_P_MAKE_FDT_ATSC(720p_50Hz,               e74_250 , 2880, 750,  58,  1, 0, 0, 20, 2, 10, 260),
	BVDC_P_MAKE_FDT_ATSC(1080i,                   e74_250 , 3200, 562,  64,  0, 0, 0, 20, 2, 10, 192),
	BVDC_P_MAKE_FDT_ATSC(1080i_50Hz,              e74_250 , 3840, 562,  64,  0, 0, 0, 20, 2, 10, 192),
	BVDC_P_MAKE_FDT_ATSC(1080p,                   e148_500, 1600, 1125, 32,  1, 0, 0, 20, 2, 10, 192),
	BVDC_P_MAKE_FDT_ATSC(1080p_50Hz,              e148_500, 1920, 1125, 32,  1, 0, 0, 20, 2, 10, 192),

	/* PC formats Group 1 */
	BVDC_P_MAKE_FDT_PC(DVI_640x480p,            e25_175 , 3431, 525,  411, 1, 1, 1, 40, 2, 10, 144),
	BVDC_P_MAKE_FDT_PC(DVI_640x480p_66Hz,       e30_240 , 3086, 525,  228, 1, 1, 1, 20, 2, 10, 160),
	BVDC_P_MAKE_FDT_PC(DVI_640x480p_72Hz,       e31_500 , 2852, 520,  137, 1, 1, 1, 20, 2, 10, 168),
	BVDC_P_MAKE_FDT_PC(DVI_640x480p_75Hz,       e31_500 , 2880, 500,  219, 1, 1, 1, 20, 2, 10, 184),
	BVDC_P_MAKE_FDT_PC(DVI_640x480p_85Hz,       e36_000 , 2496, 509,  168, 1, 1, 1, 20, 2, 10, 136),
	BVDC_P_MAKE_FDT_PC(DVI_1024x768p,           e65_000 , 2233, 806,  225, 1, 1, 1, 20, 2, 10, 296),
	BVDC_P_MAKE_FDT_PC(DVI_1024x768p_66Hz,      e71_640 , 2002, 816,  267, 1, 1, 1, 20, 2, 10, 288),
	BVDC_P_MAKE_FDT_PC(DVI_1024x768p_70Hz,      e75_000 , 1912, 806,  195, 1, 1, 1, 20, 2, 10, 280),
	BVDC_P_MAKE_FDT_PC(DVI_1366x768p_60Hz,      e72_014 , 2292, 790,  96,  1, 1, 1, 20, 2, 10, 114),

	/* PC formats Group 2 */
	BVDC_P_MAKE_FDT_PC(DVI_640x400p_85Hz,       e31_500 , 2852, 443,  219, 1, 1, 0, 20, 2, 10, 160),
	BVDC_P_MAKE_FDT_PC(DVI_720x400p_70Hz,       e28_324 , 3432, 449,  412, 1, 1, 0, 20, 2, 10, 162),
	BVDC_P_MAKE_FDT_PC(DVI_720x400p_85Hz,       e35_500 , 2847, 448,  219, 1, 1, 0, 20, 2, 10, 180),
	BVDC_P_MAKE_FDT_PC(DVI_1280x768p,           e79_500 , 2259, 796,  173, 1, 1, 0, 20, 2, 10, 320),
	BVDC_P_MAKE_FDT_PC(DVI_1280x768p_75Hz,      e102_250, 1791, 804,  135, 1, 1, 0, 20, 2, 10, 336),
	BVDC_P_MAKE_FDT_PC(DVI_1280x768p_85Hz,      e117_500, 1574, 807,  124, 1, 1, 0, 20, 2, 10, 352),
	BVDC_P_MAKE_FDT_PC(DVI_1400x1050p_60Hz,     e121_750, 1653, 1088, 127, 1, 1, 0, 20, 2, 10, 376),
	BVDC_P_MAKE_FDT_PC(DVI_1400x1050p_75Hz,     e156_000, 1312, 1097, 99,  1, 1, 0, 20, 2, 10, 392),

	/* PC formats Group 3 */
	BVDC_P_MAKE_FDT_PC(DVI_800x600p_56Hz,       e36_000 , 3072, 625,  216, 1, 0, 0, 20, 2, 10, 200),
	BVDC_P_MAKE_FDT_PC(DVI_800x600p_59Hz_Red,   e38_250 , 2891, 624,  225, 1, 0, 0, 20, 2, 10, 192),
	BVDC_P_MAKE_FDT_PC(DVI_800x600p,            e40_000 , 2851, 628,  345, 1, 0, 0, 20, 2, 10, 216),
	BVDC_P_MAKE_FDT_PC(DVI_800x600p_72Hz,       e50_000 , 2246, 666,  259, 1, 0, 0, 20, 2, 10, 184),
	BVDC_P_MAKE_FDT_PC(DVI_800x600p_75Hz,       e49_500 , 2304, 625,  174, 1, 0, 0, 20, 2, 10, 240),
	BVDC_P_MAKE_FDT_PC(DVI_800x600p_85Hz,       e56_250 , 2012, 631,  122, 1, 0, 0, 20, 2, 10, 216),
	BVDC_P_MAKE_FDT_PC(DVI_1024x768p_75Hz,      e78_750 , 1800, 800,  131, 1, 0, 0, 20, 2, 10, 272),
	BVDC_P_MAKE_FDT_PC(DVI_1024x768p_85Hz,      e94_500 , 1572, 808,  109, 1, 0, 0, 20, 2, 10, 304),
	BVDC_P_MAKE_FDT_PC(DVI_1360x768p_60Hz,      e85_500 , 2264, 795,  141, 1, 0, 0, 20, 2, 10, 368),
	BVDC_P_MAKE_FDT_PC(DVI_1152x864p_75Hz,      e108_000, 1600, 900,  128, 1, 0, 0, 20, 2, 10, 384),
	BVDC_P_MAKE_FDT_PC(DVI_1280x960p_60Hz,      e108_000, 1800, 1000, 112, 1, 0, 0, 20, 2, 10, 424),
	BVDC_P_MAKE_FDT_PC(DVI_1280x960p_85Hz,      e148_500, 1256, 1011, 116, 1, 0, 0, 20, 2, 10, 384),
	BVDC_P_MAKE_FDT_PC(DVI_1280x1024p_60Hz,     e108_000, 1688, 1066, 112, 1, 0, 0, 20, 3, 10, 360),
	BVDC_P_MAKE_FDT_PC(DVI_1280x1024p_69Hz,     e125_000, 1451, 1063, 138, 1, 0, 0, 20, 2, 10, 368),
	BVDC_P_MAKE_FDT_PC(DVI_1280x1024p_75Hz,     e135_000, 1350, 1066, 115, 1, 0, 0, 20, 2, 10, 392),
	BVDC_P_MAKE_FDT_PC(DVI_1280x1024p_85Hz,     e157_500, 1184, 1072, 109, 1, 0, 0, 20, 2, 10, 384),
	BVDC_P_MAKE_FDT_PC(DVI_1600x1200p_60Hz,     e162_000, 1440, 1250, 102, 1, 0, 0, 20, 2, 10, 496),
	BVDC_P_MAKE_FDT_PC(DVI_1024x768i_87Hz,      e44_900 , 3040, 408,  423, 2, 0, 0, 20, 2, 10, 232),

	/* PC formats Group 4 */
	BVDC_P_MAKE_FDT_PC(DVI_640x350p_70Hz,       e25_175 , 3432, 449,  412, 1, 0, 1, 20, 2, 10, 144),
	BVDC_P_MAKE_FDT_PC(DVI_640x350p_85Hz,       e31_500 , 2852, 445,  219, 1, 0, 1, 20, 2, 10, 160),
	BVDC_P_MAKE_FDT_PC(DVI_1280x768p_Red,       e68_250 , 2278, 790,  50,  1, 0, 1, 20, 2, 10, 112),
	BVDC_P_MAKE_FDT_PC(DVI_1400x1050p_60Hz_Red, e101_000, 1669, 1078, 34,  1, 0, 1, 20, 2, 10, 112),
	BVDC_P_MAKE_FDT_PC(DVI_1920x1080p_60Hz_Red, e138_625, 1620, 1111, 32,  1, 0, 1, 20, 2, 10, 112),

	/*Add the PC same format with non-standard polarity here */
	/* Atsc in pc */
	BVDC_P_MAKE_FDT_ATSC(NTSC,                    e13_500 , 4095, 440,  504, 0, 0, 1, 20, 4, 10, 119),
	BVDC_P_MAKE_FDT_ATSC(NTSC,                    e13_500 , 4095, 440,  504, 0, 1, 0, 20, 4, 10, 119),
	BVDC_P_MAKE_FDT_ATSC(NTSC,                    e13_500 , 4095, 440,  504, 0, 1, 1, 20, 4, 10, 119),
	BVDC_P_MAKE_FDT_ATSC(PAL_G,                   e13_500 , 4095, 527,  504, 0, 0, 1, 20, 4, 10, 132),
	BVDC_P_MAKE_FDT_ATSC(PAL_G,                   e13_500 , 4095, 527,  504, 0, 1, 0, 20, 4, 10, 132),
	BVDC_P_MAKE_FDT_ATSC(PAL_G,                   e13_500 , 4095, 527,  504, 0, 1, 1, 20, 4, 10, 132),
	BVDC_P_MAKE_FDT_ATSC(576p_50Hz,               e27_000 , 3456, 625,  252, 1, 0, 1, 20, 2, 10, 132),
	BVDC_P_MAKE_FDT_ATSC(576p_50Hz,               e27_000 , 3456, 625,  252, 1, 1, 0, 20, 2, 10, 132),
	BVDC_P_MAKE_FDT_ATSC(576p_50Hz,               e27_000 , 3456, 625,  252, 1, 1, 1, 20, 2, 10, 132),
	BVDC_P_MAKE_FDT_ATSC(720p,                    e74_250 , 2400, 750,  58,  1, 0, 1, 20, 2, 10, 260),
	BVDC_P_MAKE_FDT_ATSC(720p,                    e74_250 , 2400, 750,  58,  1, 1, 0, 20, 2, 10, 260),
	BVDC_P_MAKE_FDT_ATSC(720p,                    e74_250 , 2400, 750,  58,  1, 1, 1, 20, 2, 10, 260),
	BVDC_P_MAKE_FDT_ATSC(720p_50Hz,               e74_250 , 2880, 750,  58,  1, 0, 1, 20, 2, 10, 260),
	BVDC_P_MAKE_FDT_ATSC(720p_50Hz,               e74_250 , 2880, 750,  58,  1, 1, 0, 20, 2, 10, 260),
	BVDC_P_MAKE_FDT_ATSC(720p_50Hz,               e74_250 , 2880, 750,  58,  1, 1, 1, 20, 2, 10, 260),
	BVDC_P_MAKE_FDT_ATSC(1080i,                   e74_250 , 3200, 562,  64,  0, 0, 1, 20, 2, 10, 192),
	BVDC_P_MAKE_FDT_ATSC(1080i,                   e74_250 , 3200, 562,  64,  0, 1, 0, 20, 2, 10, 192),
	BVDC_P_MAKE_FDT_ATSC(1080i,                   e74_250 , 3200, 562,  64,  0, 1, 1, 20, 2, 10, 192),
	BVDC_P_MAKE_FDT_ATSC(1080i_50Hz,              e74_250 , 3840, 562,  64,  0, 0, 1, 20, 2, 10, 192),
	BVDC_P_MAKE_FDT_ATSC(1080i_50Hz,              e74_250 , 3840, 562,  64,  0, 1, 0, 20, 2, 10, 192),
	BVDC_P_MAKE_FDT_ATSC(1080i_50Hz,              e74_250 , 3840, 562,  64,  0, 1, 1, 20, 2, 10, 192),
	BVDC_P_MAKE_FDT_ATSC(1080p,                   e148_500, 1600, 1125, 32,  1, 0, 1, 20, 2, 10, 192),
	BVDC_P_MAKE_FDT_ATSC(1080p,                   e148_500, 1600, 1125, 32,  1, 1, 0, 20, 2, 10, 192),
	BVDC_P_MAKE_FDT_ATSC(1080p,                   e148_500, 1600, 1125, 32,  1, 1, 1, 20, 2, 10, 192),
	BVDC_P_MAKE_FDT_ATSC(1080p_50Hz,              e148_500, 1920, 1125, 32,  1, 0, 1, 20, 2, 10, 192),
	BVDC_P_MAKE_FDT_ATSC(1080p_50Hz,              e148_500, 1920, 1125, 32,  1, 1, 0, 20, 2, 10, 192),
	BVDC_P_MAKE_FDT_ATSC(1080p_50Hz,              e148_500, 1920, 1125, 32,  1, 1, 1, 20, 2, 10, 192),

	/*Group 1's */
	BVDC_P_MAKE_FDT_PC(DVI_640x480p,            e25_175 , 3431, 525,  411, 1, 0, 0, 40, 2, 10, 152),
	BVDC_P_MAKE_FDT_PC(DVI_640x480p,            e25_175 , 3431, 525,  411, 1, 0, 1, 40, 2, 10, 152),
	BVDC_P_MAKE_FDT_PC(DVI_640x480p,            e25_175 , 3431, 525,  411, 1, 1, 0, 40, 2, 10, 152),
	BVDC_P_MAKE_FDT_PC(DVI_640x480p_66Hz,       e30_240 , 3086, 525,  228, 1, 0, 0, 20, 2, 10, 160),
	BVDC_P_MAKE_FDT_PC(DVI_640x480p_66Hz,       e30_240 , 3086, 525,  228, 1, 0, 1, 20, 2, 10, 160),
	BVDC_P_MAKE_FDT_PC(DVI_640x480p_66Hz,       e30_240 , 3086, 525,  228, 1, 1, 0, 20, 2, 10, 160),
	BVDC_P_MAKE_FDT_PC(DVI_640x480p_72Hz,       e31_500 , 2852, 520,  137, 1, 0, 0, 20, 2, 10, 168),
	BVDC_P_MAKE_FDT_PC(DVI_640x480p_72Hz,       e31_500 , 2852, 520,  137, 1, 0, 1, 20, 2, 10, 168),
	BVDC_P_MAKE_FDT_PC(DVI_640x480p_72Hz,       e31_500 , 2852, 520,  137, 1, 1, 0, 20, 2, 10, 168),
	BVDC_P_MAKE_FDT_PC(DVI_640x480p_75Hz,       e31_500 , 2880, 500,  219, 1, 0, 0, 20, 2, 10, 184),
	BVDC_P_MAKE_FDT_PC(DVI_640x480p_75Hz,       e31_500 , 2880, 500,  219, 1, 0, 1, 20, 2, 10, 184),
	BVDC_P_MAKE_FDT_PC(DVI_640x480p_75Hz,       e31_500 , 2880, 500,  219, 1, 1, 0, 20, 2, 10, 184),
	BVDC_P_MAKE_FDT_PC(DVI_640x480p_85Hz,       e36_000 , 2496, 509,  168, 1, 0, 0, 20, 2, 10, 136),
	BVDC_P_MAKE_FDT_PC(DVI_640x480p_85Hz,       e36_000 , 2496, 509,  168, 1, 0, 1, 20, 2, 10, 136),
	BVDC_P_MAKE_FDT_PC(DVI_640x480p_85Hz,       e36_000 , 2496, 509,  168, 1, 1, 0, 20, 2, 10, 136),
	BVDC_P_MAKE_FDT_PC(DVI_1024x768p,           e65_000 , 2233, 806,  225, 1, 0, 0, 20, 2, 10, 296),
	BVDC_P_MAKE_FDT_PC(DVI_1024x768p,           e65_000 , 2233, 806,  225, 1, 0, 1, 20, 2, 10, 296),
	BVDC_P_MAKE_FDT_PC(DVI_1024x768p,           e65_000 , 2233, 806,  225, 1, 1, 0, 20, 2, 10, 296),
	BVDC_P_MAKE_FDT_PC(DVI_1024x768p_66Hz,      e71_640 , 2002, 816,  267, 1, 0, 0, 20, 2, 10, 288),
	BVDC_P_MAKE_FDT_PC(DVI_1024x768p_66Hz,      e71_640 , 2002, 816,  267, 1, 0, 1, 20, 2, 10, 288),
	BVDC_P_MAKE_FDT_PC(DVI_1024x768p_66Hz,      e71_640 , 2002, 816,  267, 1, 1, 0, 20, 2, 10, 288),
	BVDC_P_MAKE_FDT_PC(DVI_1024x768p_70Hz,      e75_000 , 1912, 806,  195, 1, 0, 0, 20, 2, 10, 280),
	BVDC_P_MAKE_FDT_PC(DVI_1024x768p_70Hz,      e75_000 , 1912, 806,  195, 1, 0, 1, 20, 2, 10, 280),
	BVDC_P_MAKE_FDT_PC(DVI_1024x768p_70Hz,      e75_000 , 1912, 806,  195, 1, 1, 0, 20, 2, 10, 280),

	/*Group 2's */
	/*640x400p@85 only support starndard*/
	/*720x400p@70 only support starndard*/
	/*720x400p@85 only support starndard*/
	BVDC_P_MAKE_FDT_PC(DVI_1280x768p,           e79_500 , 2259, 796,  173, 1, 0, 0, 20, 2, 10, 320),
	BVDC_P_MAKE_FDT_PC(DVI_1280x768p,           e79_500 , 2259, 796,  173, 1, 0, 1, 20, 2, 10, 320),
	BVDC_P_MAKE_FDT_PC(DVI_1280x768p,           e79_500 , 2259, 796,  173, 1, 1, 1, 20, 2, 10, 320),
	BVDC_P_MAKE_FDT_PC(DVI_1280x768p_75Hz,      e102_250, 1791, 804,  135, 1, 0, 0, 20, 2, 10, 336),
	BVDC_P_MAKE_FDT_PC(DVI_1280x768p_75Hz,      e102_250, 1791, 804,  135, 1, 0, 1, 20, 2, 10, 336),
	BVDC_P_MAKE_FDT_PC(DVI_1280x768p_75Hz,      e102_250, 1791, 804,  135, 1, 1, 1, 20, 2, 10, 336),
	/*1280x768p@85 only support starndard*/
	BVDC_P_MAKE_FDT_PC(DVI_1400x1050p_60Hz,     e121_750, 1653, 1088, 127, 1, 0, 0, 20, 2, 10, 376),
	BVDC_P_MAKE_FDT_PC(DVI_1400x1050p_60Hz,     e121_750, 1653, 1088, 127, 1, 0, 1, 20, 2, 10, 376),
	BVDC_P_MAKE_FDT_PC(DVI_1400x1050p_60Hz,     e121_750, 1653, 1088, 127, 1, 1, 1, 20, 2, 10, 376),
	BVDC_P_MAKE_FDT_PC(DVI_1400x1050p_75Hz,     e156_000, 1312, 1097, 99,  1, 0, 0, 20, 2, 10, 392),
	BVDC_P_MAKE_FDT_PC(DVI_1400x1050p_75Hz,     e156_000, 1312, 1097, 99,  1, 0, 1, 20, 2, 10, 392),
	BVDC_P_MAKE_FDT_PC(DVI_1400x1050p_75Hz,     e156_000, 1312, 1097, 99,  1, 1, 1, 20, 2, 10, 392),

	/*Group 3's*/
	BVDC_P_MAKE_FDT_PC(DVI_800x600p_56Hz,       e36_000 , 3072, 625,  216, 1, 0, 1, 20, 2, 10, 200),
	BVDC_P_MAKE_FDT_PC(DVI_800x600p_56Hz,       e36_000 , 3072, 625,  216, 1, 1, 0, 20, 2, 10, 200),
	BVDC_P_MAKE_FDT_PC(DVI_800x600p_56Hz,       e36_000 , 3072, 625,  216, 1, 1, 1, 20, 2, 10, 200),
	BVDC_P_MAKE_FDT_PC(DVI_800x600p_59Hz_Red,   e38_250 , 2891, 624,  225, 1, 0, 1, 20, 2, 10, 192),
	BVDC_P_MAKE_FDT_PC(DVI_800x600p_59Hz_Red,   e38_250 , 2891, 624,  225, 1, 1, 0, 20, 2, 10, 192),
	BVDC_P_MAKE_FDT_PC(DVI_800x600p_59Hz_Red,   e38_250 , 2891, 624,  225, 1, 1, 1, 20, 2, 10, 192),
	BVDC_P_MAKE_FDT_PC(DVI_800x600p,            e40_000 , 2851, 628,  345, 1, 0, 1, 20, 2, 10, 210),
	BVDC_P_MAKE_FDT_PC(DVI_800x600p,            e40_000 , 2851, 628,  345, 1, 1, 0, 20, 2, 10, 210),
	BVDC_P_MAKE_FDT_PC(DVI_800x600p,            e40_000 , 2851, 628,  345, 1, 1, 1, 20, 2, 10, 210),
	BVDC_P_MAKE_FDT_PC(DVI_800x600p_72Hz,       e50_000 , 2246, 666,  259, 1, 0, 1, 20, 2, 10, 184),
	BVDC_P_MAKE_FDT_PC(DVI_800x600p_72Hz,       e50_000 , 2246, 666,  259, 1, 1, 0, 20, 2, 10, 184),
	BVDC_P_MAKE_FDT_PC(DVI_800x600p_72Hz,       e50_000 , 2246, 666,  259, 1, 1, 1, 20, 2, 10, 184),
	BVDC_P_MAKE_FDT_PC(DVI_800x600p_75Hz,       e49_500 , 2304, 625,  174, 1, 0, 1, 20, 2, 10, 240),
	BVDC_P_MAKE_FDT_PC(DVI_800x600p_75Hz,       e49_500 , 2304, 625,  174, 1, 1, 0, 20, 2, 10, 240),
	BVDC_P_MAKE_FDT_PC(DVI_800x600p_75Hz,       e49_500 , 2304, 625,  174, 1, 1, 1, 20, 2, 10, 240),
	BVDC_P_MAKE_FDT_PC(DVI_800x600p_85Hz,       e56_250 , 2012, 631,  122, 1, 0, 1, 20, 2, 10, 216),
	BVDC_P_MAKE_FDT_PC(DVI_800x600p_85Hz,       e56_250 , 2012, 631,  122, 1, 1, 0, 20, 2, 10, 216),
	BVDC_P_MAKE_FDT_PC(DVI_800x600p_85Hz,       e56_250 , 2012, 631,  122, 1, 1, 1, 20, 2, 10, 216),
	BVDC_P_MAKE_FDT_PC(DVI_1024x768p_75Hz,      e78_750 , 1800, 800,  131, 1, 0, 1, 20, 2, 10, 272),
	BVDC_P_MAKE_FDT_PC(DVI_1024x768p_75Hz,      e78_750 , 1800, 800,  131, 1, 1, 0, 20, 2, 10, 272),
	BVDC_P_MAKE_FDT_PC(DVI_1024x768p_75Hz,      e78_750 , 1800, 800,  131, 1, 1, 1, 20, 2, 10, 272),
	/*1024x768p@85 only support starndard*/
	BVDC_P_MAKE_FDT_PC(DVI_1360x768p_60Hz,      e85_500 , 2264, 795,  141, 1, 0, 1, 20, 2, 10, 368),
	BVDC_P_MAKE_FDT_PC(DVI_1360x768p_60Hz,      e85_500 , 2264, 795,  141, 1, 1, 0, 20, 2, 10, 368),
	BVDC_P_MAKE_FDT_PC(DVI_1360x768p_60Hz,      e85_500 , 2264, 795,  141, 1, 1, 1, 20, 2, 10, 368),
	BVDC_P_MAKE_FDT_PC(DVI_1152x864p_75Hz,      e108_000, 1600, 900,  128, 1, 0, 1, 20, 2, 10, 384),
	BVDC_P_MAKE_FDT_PC(DVI_1152x864p_75Hz,      e108_000, 1600, 900,  128, 1, 1, 0, 20, 2, 10, 384),
	BVDC_P_MAKE_FDT_PC(DVI_1152x864p_75Hz,      e108_000, 1600, 900,  128, 1, 1, 1, 20, 2, 10, 384),
	BVDC_P_MAKE_FDT_PC(DVI_1280x960p_60Hz,      e108_000, 1800, 1000, 112, 1, 0, 1, 20, 2, 10, 424),
	BVDC_P_MAKE_FDT_PC(DVI_1280x960p_60Hz,      e108_000, 1800, 1000, 112, 1, 1, 0, 20, 2, 10, 424),
	BVDC_P_MAKE_FDT_PC(DVI_1280x960p_60Hz,      e108_000, 1800, 1000, 112, 1, 1, 1, 20, 2, 10, 424),
	BVDC_P_MAKE_FDT_PC(DVI_1280x960p_85Hz,      e148_500, 1256, 1011, 116, 1, 0, 1, 20, 2, 10, 384),
	BVDC_P_MAKE_FDT_PC(DVI_1280x960p_85Hz,      e148_500, 1256, 1011, 116, 1, 1, 0, 20, 2, 10, 384),
	BVDC_P_MAKE_FDT_PC(DVI_1280x960p_85Hz,      e148_500, 1256, 1011, 116, 1, 1, 1, 20, 2, 10, 384),
	BVDC_P_MAKE_FDT_PC(DVI_1280x1024p_60Hz,     e108_000, 1688, 1066, 112, 1, 0, 1, 20, 3, 10, 360),
	BVDC_P_MAKE_FDT_PC(DVI_1280x1024p_60Hz,     e108_000, 1688, 1066, 112, 1, 1, 0, 20, 3, 10, 360),
	BVDC_P_MAKE_FDT_PC(DVI_1280x1024p_60Hz,     e108_000, 1688, 1066, 112, 1, 1, 1, 20, 3, 10, 360),
	BVDC_P_MAKE_FDT_PC(DVI_1280x1024p_69Hz,     e125_000, 1451, 1063, 138, 1, 0, 1, 20, 2, 10, 368),
	BVDC_P_MAKE_FDT_PC(DVI_1280x1024p_69Hz,     e125_000, 1451, 1063, 138, 1, 1, 0, 20, 2, 10, 368),
	BVDC_P_MAKE_FDT_PC(DVI_1280x1024p_69Hz,     e125_000, 1451, 1063, 138, 1, 1, 1, 20, 2, 10, 368),
	BVDC_P_MAKE_FDT_PC(DVI_1280x1024p_75Hz,     e135_000, 1350, 1066, 115, 1, 0, 1, 20, 2, 10, 392),
	BVDC_P_MAKE_FDT_PC(DVI_1280x1024p_75Hz,     e135_000, 1350, 1066, 115, 1, 1, 0, 20, 2, 10, 392),
	BVDC_P_MAKE_FDT_PC(DVI_1280x1024p_75Hz,     e135_000, 1350, 1066, 115, 1, 1, 1, 20, 2, 10, 392),
	BVDC_P_MAKE_FDT_PC(DVI_1280x1024p_85Hz,     e157_500, 1184, 1072, 109, 1, 0, 1, 20, 2, 10, 384),
	BVDC_P_MAKE_FDT_PC(DVI_1280x1024p_85Hz,     e157_500, 1184, 1072, 109, 1, 1, 0, 20, 2, 10, 384),
	BVDC_P_MAKE_FDT_PC(DVI_1280x1024p_85Hz,     e157_500, 1184, 1072, 109, 1, 1, 1, 20, 2, 10, 384),
	BVDC_P_MAKE_FDT_PC(DVI_1600x1200p_60Hz,     e162_000, 1440, 1250, 102, 1, 0, 1, 20, 2, 10, 496),
	BVDC_P_MAKE_FDT_PC(DVI_1600x1200p_60Hz,     e162_000, 1440, 1250, 102, 1, 1, 0, 20, 2, 10, 496),
	BVDC_P_MAKE_FDT_PC(DVI_1600x1200p_60Hz,     e162_000, 1440, 1250, 102, 1, 1, 1, 20, 2, 10, 496),
	BVDC_P_MAKE_FDT_PC(DVI_1024x768i_87Hz,      e44_900 , 3040, 408,  423, 2, 0, 1, 20, 2, 10, 232),
	BVDC_P_MAKE_FDT_PC(DVI_1024x768i_87Hz,      e44_900 , 3040, 408,  423, 2, 1, 0, 20, 2, 10, 232),
	BVDC_P_MAKE_FDT_PC(DVI_1024x768i_87Hz,      e44_900 , 3040, 408,  423, 2, 1, 1, 20, 2, 10, 232),

	/*Group 4's*/
	/*640x350p@70 only support starndard*/
	/*640x350p@85 only support starndard*/
	BVDC_P_MAKE_FDT_PC(DVI_1280x768p_Red,       e68_250 , 2278, 790,  50,  1, 0, 0, 20, 2, 10, 112),
	BVDC_P_MAKE_FDT_PC(DVI_1280x768p_Red,       e68_250 , 2278, 790,  50,  1, 1, 0, 20, 2, 10, 112),
	BVDC_P_MAKE_FDT_PC(DVI_1280x768p_Red,       e68_250 , 2278, 790,  50,  1, 1, 1, 20, 2, 10, 112),
	BVDC_P_MAKE_FDT_PC(DVI_1400x1050p_60Hz_Red, e101_000, 1669, 1078, 34,  1, 0, 0, 20, 2, 10, 112),
	BVDC_P_MAKE_FDT_PC(DVI_1400x1050p_60Hz_Red, e101_000, 1669, 1078, 34,  1, 1, 0, 20, 2, 10, 112),
	BVDC_P_MAKE_FDT_PC(DVI_1400x1050p_60Hz_Red, e101_000, 1669, 1078, 34,  1, 1, 1, 20, 2, 10, 112),
	BVDC_P_MAKE_FDT_PC(DVI_1920x1080p_60Hz_Red, e138_625, 1620, 1111, 32,  1, 0, 0, 20, 2, 10, 112),
	BVDC_P_MAKE_FDT_PC(DVI_1920x1080p_60Hz_Red, e138_625, 1620, 1111, 32,  1, 1, 0, 20, 2, 10, 112),
	BVDC_P_MAKE_FDT_PC(DVI_1920x1080p_60Hz_Red, e138_625, 1620, 1111, 32,  1, 1, 1, 20, 2, 10, 112)
};


/* Static lookup table to figure out BVDC_P_VdecId_eVdec0 get what adc based
 * on connector (BVDC_Adc_n) selected.  Which ADC and which Input selection
 * for a particular ADC.
 * INDEX: [idx(BVDC_Adc_x)] */
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
static const BVDC_P_AdcEntry s_aVdec0AdcSelTbl[] =
{
	/* VDEC_0: Use VAFE 1, 2, 3 */
	BVDC_P_MAKE_ADC(BVDC_Adc_0 , 2, SD_Y1,    NONE, COMPONENT1),
	BVDC_P_MAKE_ADC(BVDC_Adc_1 , 3, SD_PR1,   NONE, COMPONENT1),
	BVDC_P_MAKE_ADC(BVDC_Adc_2 , 4, SD_PB1,   NONE, COMPONENT1),
	BVDC_P_MAKE_ADC(BVDC_Adc_3 , 2, SD_Y2,    NONE, COMPONENT2),
	BVDC_P_MAKE_ADC(BVDC_Adc_4 , 3, SD_PR2,   NONE, COMPONENT2),
	BVDC_P_MAKE_ADC(BVDC_Adc_5 , 4, SD_PB2,   NONE, COMPONENT2),
	BVDC_P_MAKE_ADC(BVDC_Adc_6 , 2, SD_Y3,    NONE, COMPONENT3),
	BVDC_P_MAKE_ADC(BVDC_Adc_7 , 3, SD_PR3,   NONE, COMPONENT3),
	BVDC_P_MAKE_ADC(BVDC_Adc_8 , 4, SD_PB3,   NONE, COMPONENT3),
	BVDC_P_MAKE_ADC(BVDC_Adc_9 , 2, SD_L1,    NONE, SVIDEO1   ),
	BVDC_P_MAKE_ADC(BVDC_Adc_10, 3, SD_C1,    NONE, SVIDEO1   ),
	BVDC_P_MAKE_ADC(BVDC_Adc_11, 2, SD_L2,    NONE, SVIDEO2   ),
	BVDC_P_MAKE_ADC(BVDC_Adc_12, 3, SD_C2,    NONE, SVIDEO2   ),
	BVDC_P_MAKE_ADC(BVDC_Adc_13, 2, SD_L3,    NONE, SVIDEO3   ),
	BVDC_P_MAKE_ADC(BVDC_Adc_14, 3, SD_C3,    NONE, SVIDEO3   ),
	BVDC_P_MAKE_ADC(BVDC_Adc_15, 1, SD_CVBS1, NONE, CVBS1     ),
	BVDC_P_MAKE_ADC(BVDC_Adc_16, 1, SD_CVBS2, NONE, CVBS2     ),
	BVDC_P_MAKE_ADC(BVDC_Adc_17, 1, SD_CVBS3, NONE, CVBS3     ),
	BVDC_P_MAKE_ADC(BVDC_Adc_18, 1, SD_CVBS4, NONE, CVBS4     ),
	BVDC_P_MAKE_ADC(BVDC_Adc_19, 2, SD_R,     SD_R, RGB       ),
	BVDC_P_MAKE_ADC(BVDC_Adc_20, 3, SD_G,     SD_G, RGB       ),
	BVDC_P_MAKE_ADC(BVDC_Adc_21, 4, SD_B,     SD_B, RGB       ),
	BVDC_P_MAKE_ADC_NULL(BVDC_Ibafe_0                         )
};

static const BVDC_P_AdcEntry s_aVdec1AdcSelTbl[] =
{
	/* VDEC_0: Use VAFE 1, 2, 3 */
	BVDC_P_MAKE_ADC_NULL(BVDC_Ibafe_0                         )
};

#else /* 3563Cx, Dx */
static const BVDC_P_AdcEntry s_aVdec0AdcSelTbl[] =
{
	/* VDEC_0: Use VAFE 1, 2, 3 */
	BVDC_P_MAKE_ADC(BVDC_Adc_0 , 1, SD_Y1,    NONE, COMPONENT1),
	BVDC_P_MAKE_ADC(BVDC_Adc_1 , 2, SD_PR1,   NONE, COMPONENT1),
	BVDC_P_MAKE_ADC(BVDC_Adc_2 , 3, SD_PB1,   NONE, COMPONENT1),
	BVDC_P_MAKE_ADC(BVDC_Adc_3 , 1, SD_Y2,    NONE, COMPONENT2),
	BVDC_P_MAKE_ADC(BVDC_Adc_4 , 2, SD_PR2,   NONE, COMPONENT2),
	BVDC_P_MAKE_ADC(BVDC_Adc_5 , 3, SD_PB2,   NONE, COMPONENT2),
	BVDC_P_MAKE_ADC(BVDC_Adc_6 , 1, SD_L1,    NONE, SVIDEO1   ),
	BVDC_P_MAKE_ADC(BVDC_Adc_7 , 2, SD_C1,    NONE, SVIDEO1   ),
	BVDC_P_MAKE_ADC(BVDC_Adc_8 , 1, SD_L2,    NONE, SVIDEO2   ),
	BVDC_P_MAKE_ADC(BVDC_Adc_9 , 2, SD_C2,    NONE, SVIDEO2   ),
	BVDC_P_MAKE_ADC(BVDC_Adc_10, 1, SD_L3,    NONE, SVIDEO3   ),
	BVDC_P_MAKE_ADC(BVDC_Adc_11, 2, SD_C3,    NONE, SVIDEO3   ),
	BVDC_P_MAKE_ADC(BVDC_Adc_12, 3, SD_CVBS1, NONE, CVBS1     ),
	BVDC_P_MAKE_ADC(BVDC_Adc_13, 3, SD_CVBS2, NONE, CVBS2     ),
	BVDC_P_MAKE_ADC(BVDC_Adc_14, 3, SD_CVBS3, NONE, CVBS3     ),
	BVDC_P_MAKE_ADC(BVDC_Adc_15, 3, SD_CVBS4, NONE, CVBS4     ),
	BVDC_P_MAKE_ADC(BVDC_Adc_16, 3, SD_CVBS5, NONE, CVBS5     ),
	BVDC_P_MAKE_ADC(BVDC_Adc_17, 2, SD_R,     SD_R, RGB       ),
	BVDC_P_MAKE_ADC(BVDC_Adc_18, 3, SD_G,     SD_G, RGB       ),
	BVDC_P_MAKE_ADC(BVDC_Adc_19, 4, SD_B,     SD_B, RGB       ),
	BVDC_P_MAKE_ADC_NULL(BVDC_Adc_20                          ),
	BVDC_P_MAKE_ADC_NULL(BVDC_Adc_21                          ),
	BVDC_P_MAKE_ADC_NULL(BVDC_Ibafe_0                         )
};

static const BVDC_P_AdcEntry s_aVdec1AdcSelTbl[] =
{
	/* VDEC_1: Use VAFE 4, 5, 6 */
	BVDC_P_MAKE_ADC(BVDC_Adc_0 , 4, SD_Y1,    NONE, COMPONENT1),
	BVDC_P_MAKE_ADC(BVDC_Adc_1 , 5, SD_PR1,   NONE, COMPONENT1),
	BVDC_P_MAKE_ADC(BVDC_Adc_2 , 6, SD_PB1,   NONE, COMPONENT1),
	BVDC_P_MAKE_ADC(BVDC_Adc_3 , 4, SD_Y2,    NONE, COMPONENT2),
	BVDC_P_MAKE_ADC(BVDC_Adc_4 , 5, SD_PR2,   NONE, COMPONENT2),
	BVDC_P_MAKE_ADC(BVDC_Adc_5 , 6, SD_PB2,   NONE, COMPONENT2),
	BVDC_P_MAKE_ADC(BVDC_Adc_6 , 4, SD_L1,    NONE, SVIDEO1   ),
	BVDC_P_MAKE_ADC(BVDC_Adc_7 , 5, SD_C1,    NONE, SVIDEO1   ),
	BVDC_P_MAKE_ADC(BVDC_Adc_8 , 4, SD_L2,    NONE, SVIDEO2   ),
	BVDC_P_MAKE_ADC(BVDC_Adc_9 , 5, SD_C2,    NONE, SVIDEO2   ),
	BVDC_P_MAKE_ADC(BVDC_Adc_10, 4, SD_L3,    NONE, SVIDEO3   ),
	BVDC_P_MAKE_ADC(BVDC_Adc_11, 5, SD_C3,    NONE, SVIDEO3   ),
	BVDC_P_MAKE_ADC(BVDC_Adc_12, 6, SD_CVBS1, NONE, CVBS1     ),
	BVDC_P_MAKE_ADC(BVDC_Adc_13, 6, SD_CVBS2, NONE, CVBS2     ),
	BVDC_P_MAKE_ADC(BVDC_Adc_14, 6, SD_CVBS3, NONE, CVBS3     ),
	BVDC_P_MAKE_ADC(BVDC_Adc_15, 6, SD_CVBS4, NONE, CVBS4     ),
	BVDC_P_MAKE_ADC(BVDC_Adc_16, 6, SD_CVBS5, NONE, CVBS5     ),
	BVDC_P_MAKE_ADC(BVDC_Adc_17, 2, SD_R,     SD_R, RGB       ),
	BVDC_P_MAKE_ADC(BVDC_Adc_18, 3, SD_G,     SD_G, RGB       ),
	BVDC_P_MAKE_ADC(BVDC_Adc_19, 4, SD_B,     SD_B, RGB       ),
	BVDC_P_MAKE_ADC_NULL(BVDC_Adc_20                          ),
	BVDC_P_MAKE_ADC_NULL(BVDC_Adc_21                          ),
	BVDC_P_MAKE_ADC_NULL(BVDC_Ibafe_0                         )
};
#endif

/* Static lookup table to figure out configuruation of PC_IN and VDEC_FE for
 * PC Input settings.
 * INDEX: none search.
 * Disable dcr, dither, and pll for all formats. 20071012 Val's solution*/
static const BVDC_P_PcInCfgEntry s_aPcInCfgTbl[] =
{
	/*              fmt                       dcr avg     diff adc dcr
	                                                                 dit pll */
	BVDC_P_MAKE_VGA(NTSC             ,        61, AVG_64,  160, 0, 0, 0, 0),
	BVDC_P_MAKE_VGA(PAL_G            ,        64, AVG_64,  160, 0, 0, 0, 0),
	BVDC_P_MAKE_VGA(576p_50Hz        ,        66, AVG_64,  160, 0, 0, 0, 0),
	BVDC_P_MAKE_VGA(720p             ,       130, AVG_64,  160, 0, 0, 0, 0),
	BVDC_P_MAKE_VGA(720p_50Hz        ,       130, AVG_64,  160, 0, 0, 0, 0),
	BVDC_P_MAKE_VGA(1080i            ,        96, AVG_64,  160, 0, 0, 0, 0),
	BVDC_P_MAKE_VGA(1080i_50Hz       ,        96, AVG_64,  160, 0, 0, 0, 0),
	BVDC_P_MAKE_VGA(1080p            ,        96, AVG_64,  160, 0, 0, 0, 0),
	BVDC_P_MAKE_VGA(1080p_50Hz       ,        96, AVG_64,  160, 0, 0, 0, 0),

	BVDC_P_MAKE_VGA(DVI_640x350p_70Hz,        48, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_640x350p_85Hz,        80, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_640x400p_85Hz,        80, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_640x480p,             60, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_640x480p_66Hz,        80, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_640x480p_72Hz,        32, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_640x480p_75Hz,        48, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_640x480p_85Hz,        60, AVG_64,  160, 1, 0, 0, 0),

	BVDC_P_MAKE_VGA(DVI_720x400p_70Hz,        80, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_720x400p_85Hz,        80, AVG_64,  160, 1, 0, 0, 0),

	BVDC_P_MAKE_VGA(DVI_800x600p_56Hz,       100, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_800x600p_59Hz_Red,    96, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_800x600p,             10, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_800x600p_72Hz,        32, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_800x600p_75Hz,       100, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_800x600p_85Hz,        70, AVG_64,  160, 1, 0, 0, 0),

	BVDC_P_MAKE_VGA(DVI_1024x768i_87Hz,      116, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_1024x768p,           148, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_1024x768p_66Hz,      144, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_1024x768p_70Hz,      140, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_1024x768p_75Hz,      120, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_1024x768p_85Hz,      120, AVG_64,  160, 1, 0, 0, 0),

	BVDC_P_MAKE_VGA(DVI_1152x864p_75Hz,      192, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_1280x768p,           150, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_1280x768p_Red,        40, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_1280x768p_75Hz,      150, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_1280x768p_85Hz,      176, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_1280x960p_60Hz,      200, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_1280x960p_85Hz,      197, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_1280x1024p_60Hz,     172, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_1280x1024p_69Hz,     184, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_1280x1024p_75Hz,     196, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_1280x1024p_85Hz,     197, AVG_64,  160, 1, 0, 0, 0),

	BVDC_P_MAKE_VGA(DVI_1360x768p_60Hz,      172, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_1366x768p_60Hz,       57, AVG_64,  160, 1, 0, 0, 0),

	BVDC_P_MAKE_VGA(DVI_1400x1050p_60Hz_Red,  40, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_1400x1050p_60Hz,     188, AVG_64,  160, 1, 0, 0, 0),
	BVDC_P_MAKE_VGA(DVI_1400x1050p_75Hz,     196, AVG_64,  160, 1, 0, 0, 0),

	BVDC_P_MAKE_VGA(DVI_1600x1200p_60Hz,     248, AVG_64,  160, 1, 0, 0, 0),

	BVDC_P_MAKE_VGA(DVI_1920x1080p_60Hz_Red,  56, AVG_64,  160, 1, 0, 0, 0),

	/* Last */
	BVDC_P_MAKE_VGA(MaxCount, 0, AVG_128, 0,  0, 0, 0, 0)
};


/* Static lookup table to figure out configuruation of CD
 * INDEX: none search. */
static const BVDC_P_CdCfgEntry s_aCdCfgTbl[] =
{
	/*             fmt,      freq_step, per_cnt, ref,start,end, active*/
	BVDC_P_MAKE_CD(NTSC,     0x21F07BD6,     35,  112, 24, 230, 5),
	BVDC_P_MAKE_CD(NTSC_J,   0x21F07BD6,     35,  112, 24, 230, 5),
	BVDC_P_MAKE_CD(NTSC_443, 0x2A098ACB, 709375,  112, 24, 230, 5),
	BVDC_P_MAKE_CD(PAL_G,    0x2A098A53, 709375,   83, 20, 256, 5),
	BVDC_P_MAKE_CD(PAL_M,    0x21E6EF56,    303,   83, 24, 230, 5),
	BVDC_P_MAKE_CD(PAL_60,   0x2a098acb,    303,   83, 24, 230, 5),
	BVDC_P_MAKE_CD(PAL_NC,   0x21F6941E,  21227,   83, 20, 256, 5),
	BVDC_P_MAKE_CD(SECAM,    0x2A098A53, 709375, 4096, 20, 256, 5),
};


/* Static lookup table to figure out configuruation of YC
 * INDEX: none search. */
/*in 3548, 3dcomb work on oversampling image, the memory burst need to config differently*/
static const BVDC_P_YcCfgEntry s_aYcCfgTbl[] =
{
	/*             fmt,      coef,      delay, delta_line,delay_mode,decimation_ctrl, game_mode,
	                                                            phase, mem_burst,burst1, burst2, delta, 1offset, 2offset*/
	BVDC_P_MAKE_YC(NTSC,     3579545,   77, 0,0,0, eLegacyFull, M90, 2584, 0x13EA, 0x27FB, 8,  310, 0x51, 0x2C),
	BVDC_P_MAKE_YC(NTSC,     3579545,   77, 0,0,1, eLegacySave, M90, 2584, 0x9F5,  0x13FE, 8,  310, 0x80, 0x16),
	BVDC_P_MAKE_YC(NTSC,     3579545,   77, 2,1,0, eGameFull,   M90, 2584, 0x13FD, 0x27FB, 8,  310, 0xA9, 0x2C),
	BVDC_P_MAKE_YC(NTSC,     3579545,   77, 2,1,1, eGameSave,   M90, 2584, 0x9FF,  0x13FE, 8,  310, 0x54, 0x16),

	/* Only support eLegacyFull */
	BVDC_P_MAKE_YC(NTSC_J,   3579545,   77, 0,0,0, eLegacyFull, M90, 2584, 2559,   5118,   8,  310, 84,   22  ),
	BVDC_P_MAKE_YC(NTSC_J,   3579545,   77, 0,0,0, eLegacySave, M90, 2584, 2559,   5118,   8,  310, 84,   22  ),
	BVDC_P_MAKE_YC(NTSC_J,   3579545,   77, 0,0,0, eGameFull,   M90, 2584, 2559,   5118,   8,  310, 84,   22  ),
	BVDC_P_MAKE_YC(NTSC_J,   3579545,   77, 0,0,0, eGameSave,   M90, 2584, 2559,   5118,   8,  310, 84,   22  ),

	/* Only support eLegacyFull */
	BVDC_P_MAKE_YC(NTSC_443, 443361875, 77, 0,0,0, eLegacyFull, M90, 2584, 2559,   5118,   8,  310, 84,   22  ),
	BVDC_P_MAKE_YC(NTSC_443, 443361875, 77, 0,0,0, eLegacySave, M90, 2584, 2559,   5118,   8,  310, 84,   22  ),
	BVDC_P_MAKE_YC(NTSC_443, 443361875, 77, 0,0,0, eGameFull,   M90, 2584, 2559,   5118,   8,  310, 84,   22  ),
	BVDC_P_MAKE_YC(NTSC_443, 443361875, 77, 0,0,0, eGameSave,   M90, 2584, 2559,   5118,   8,  310, 84,   22  ),

	BVDC_P_MAKE_YC(PAL_G,    443361875, 65, 0,0,0, eLegacyFull, M90, 6191, 0x2FDC, 0x5FDF, 5,  288, 0x35, 0x24),
	BVDC_P_MAKE_YC(PAL_G,    443361875, 65, 0,0,1, eLegacySave, M90, 6191, 0x17EE, 0x2FF0, 5,  288, 0x72, 0x12),
	BVDC_P_MAKE_YC(PAL_G,    443361875, 65, 2,1,0, eGameFull,   M90, 6191, 0x2FEF, 0x5FDF, 5,  288, 0xa5, 0x24),
	BVDC_P_MAKE_YC(PAL_G,    443361875, 65, 2,1,1, eGameSave,   M90, 6191, 0x17F8, 0x2FF0, 5,  288, 0x52, 0x12),

	BVDC_P_MAKE_YC(SECAM_L,  443361875, 65, 0,0,0, eLegacyFull, M90, 6191, 0x2FDC, 0x5FDF, 5,  288, 0x35, 0x24),
	BVDC_P_MAKE_YC(SECAM_L,  443361875, 65, 0,0,1, eLegacySave, M90, 6191, 0x17EE, 0x2FF0, 5,  288, 0x72, 0x12),
	BVDC_P_MAKE_YC(SECAM_L,  443361875, 65, 2,1,0, eGameFull,   M90, 6191, 0x2FEF, 0x5FDF, 5,  288, 0xa5, 0x24),
	BVDC_P_MAKE_YC(SECAM_L,  443361875, 65, 2,1,1, eGameSave,   M90, 6191, 0x17F8, 0x2FF0, 5,  288, 0x52, 0x12),

	BVDC_P_MAKE_YC(PAL_M,    357561149, 77, 0,0,0, eLegacyFull, P90, 5168, 0x27E9, 0x4FF8, 30, 254, 0x25, 0x84),
	BVDC_P_MAKE_YC(PAL_M,    357561149, 77, 0,0,1, eLegacySave, P90, 5168, 0x13F5, 0x27FC, 30, 254, 0x12, 0x9a),
	BVDC_P_MAKE_YC(PAL_M,    357561149, 77, 2,1,0, eGameFull,   P90, 5168, 0x27FC, 0x4FF8, 30, 254, 0x7d, 0x84),
	BVDC_P_MAKE_YC(PAL_M,    357561149, 77, 2,1,1, eGameSave,   P90, 5168, 0x13FE, 0x27FC, 30, 254, 0x96, 0x9a),

	/* Only support eLegacyFull */
	BVDC_P_MAKE_YC(PAL_60,   443361875, 77, 0,0,0, eLegacyFull, M90, 5168, 2559,   5118,   30, 254, 84,   22  ),
	BVDC_P_MAKE_YC(PAL_60,   443361875, 77, 0,0,0, eLegacySave, M90, 5168, 2559,   5118,   30, 254, 84,   22  ),
	BVDC_P_MAKE_YC(PAL_60,   443361875, 77, 0,0,0, eGameFull,   M90, 5168, 2559,   5118,   30, 254, 84,   22  ),
	BVDC_P_MAKE_YC(PAL_60,   443361875, 77, 0,0,0, eGameSave,   M90, 5168, 2559,   5118,   30, 254, 84,   22  ),

	BVDC_P_MAKE_YC(PAL_NC,   358205625, 65, 0,0,0, eLegacyFull, P90, 6191, 0x2FDC, 0x5FDF, 5,  288, 0x35, 0x24),
	BVDC_P_MAKE_YC(PAL_NC,   358205625, 65, 0,0,1, eLegacySave, P90, 6191, 0x17EE, 0x2FF0, 5,  288, 0x72, 0x12),
	BVDC_P_MAKE_YC(PAL_NC,   358205625, 65, 2,1,0, eGameFull,   P90, 6191, 0x2FEF, 0x5FDF, 5,  288, 0xa5, 0x24),
	BVDC_P_MAKE_YC(PAL_NC,   358205625, 65, 2,1,1, eGameSave,   P90, 6191, 0x17F8, 0x2FF0, 5,  288, 0x52, 0x12)
};


/***************************************************************************
 * Allocate format identification format tables and intialized with given
 * formats  size intenal to vdec.  Consider PC vs ATSC table separately to
 * reduce search space.
 */
static void* BVDC_P_Vdec_FmtTblAlloc
	( BMEM_Heap_Handle                 hMem,
	  const BVDC_P_FdtEntry           *aFdtFmtTbl,
	  uint32_t                         ulFdtEntries )
{
	uint32_t i, *pulFdtMem, *pulFdtCachedMem;
	uint32_t aulFdParms[2];
	const BFMT_VideoInfo *pFmtInfo;

	/* Alloc device memory for format identification table. */
	pulFdtMem = BMEM_Heap_AllocAligned(hMem,
		BVDC_P_FDT_ENTRY_SIZE * ulFdtEntries * sizeof(uint32_t),
		                      /* dwords per entry multiply by count */
		BVDC_P_FDT_ALIGNMENT, /* bytes aligned */
		0);                   /* no boundary */

	if(!pulFdtMem)
	{
		return NULL;
	}

	/* Convert to cached address */
	if (BERR_SUCCESS != BMEM_ConvertAddressToCached(hMem, pulFdtMem,
		(void**)((void *)&pulFdtCachedMem)))
	{
		BDBG_ERR(( "Failed to convert address 0x%x to cached", pulFdtMem));
		BMEM_Free(hMem, pulFdtMem);
		return NULL;
	}

	/* Did we get good address */
	BDBG_MSG(("VDEC Format Table Addr: 0x%08x", (uint32_t)pulFdtMem));

	/* Fill in the table with data */
	for(i = 0; i < ulFdtEntries; i++)
	{
		/* Compare against some FMT's format parameter.  Reduce data
		 * entries errors. */
		aulFdParms[0] = aFdtFmtTbl[i].aulFdParms[0];
		aulFdParms[1] = aFdtFmtTbl[i].aulFdParms[1];

		/* Format information!  Don't need the last one! */
		pFmtInfo = BFMT_GetVideoFormatInfoPtr(aFdtFmtTbl[i].eFmt);

		/* SW3556-978 */
		if((BFMT_VideoFmt_eDVI_1024x768p != pFmtInfo->eVideoFmt) &&
		   (BFMT_VideoFmt_eDVI_1280x768p != pFmtInfo->eVideoFmt) &&
		   (BFMT_VideoFmt_eDVI_1360x768p_60Hz != pFmtInfo->eVideoFmt))
		{
			aulFdParms[0] &= ~(BCHP_FD_0_MF_FORMAT_ENTRY_00_FT_HSYNC_WIDTH_MASK);
		}

		/* Fill in device memory FD :: FORMAT_ENTRY 0-1 */
		pulFdtCachedMem[(BVDC_P_FDT_ENTRY_SIZE * i) + 0] = aulFdParms[0];
		pulFdtCachedMem[(BVDC_P_FDT_ENTRY_SIZE * i) + 1] = aulFdParms[1];

		/* Let's see how the table looks. */
		BDBG_MSG(("\t[%03d]%-40s: %3d %3d %3d %4d %3d %3d %5d %3d %5d", i, pFmtInfo->pchFormatStr,
			/* FD_FORMAT_ENTRY_0 */
			BCHP_GET_FIELD_DATA(aulFdParms[0], FD_0_MF_FORMAT_ENTRY_00, FT_HSYNC_POLARITY ),
			BCHP_GET_FIELD_DATA(aulFdParms[0], FD_0_MF_FORMAT_ENTRY_00, FT_VSYNC_POLARITY ),
			BCHP_GET_FIELD_DATA(aulFdParms[0], FD_0_MF_FORMAT_ENTRY_00, FT_DELTA_LL       ),
			BCHP_GET_FIELD_DATA(aulFdParms[0], FD_0_MF_FORMAT_ENTRY_00, FT_HSYNC_WIDTH    ),
			BCHP_GET_FIELD_DATA(aulFdParms[0], FD_0_MF_FORMAT_ENTRY_00, FT_DELTA_HW       ),
			BCHP_GET_FIELD_DATA(aulFdParms[0], FD_0_MF_FORMAT_ENTRY_00, FT_RASTER_TYPE    ),
			/* FD_FORMAT_ENTRY_1 */
			BCHP_GET_FIELD_DATA(aulFdParms[1], FD_0_MF_FORMAT_ENTRY_01, FT_LINES_PER_FIELD),
			BCHP_GET_FIELD_DATA(aulFdParms[1], FD_0_MF_FORMAT_ENTRY_01, FT_DELTA_LPF      ),
			BCHP_GET_FIELD_DATA(aulFdParms[1], FD_0_MF_FORMAT_ENTRY_01, FT_LINE_LENGTH    )));
	}

	BMEM_FlushCache(hMem, pulFdtCachedMem, 
		BVDC_P_FDT_ENTRY_SIZE * ulFdtEntries * sizeof(uint32_t));

	return (void*)pulFdtMem;
}


/***************************************************************************
 * Free format identification format tables (macro instead).
 *
 */
static void BVDC_P_Vdec_FmtTblFree
	( BMEM_Heap_Handle                 hMem,
	  void                            *pulFdtMem )
{
	BMEM_Heap_Free(hMem, pulFdtMem);
	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_Vdec_SampleInit
	( BVDC_P_RegSample                *pRegSample,
	  uint32_t                        *aulSample,
	  uint32_t                         ulInitValue,
	  uint32_t                         ulThreshold,
	  BVDC_P_SampleAlg                 eAlg )
{
	uint32_t i;

	/* Initialized sample structure! */
	pRegSample->ulValue    = ulInitValue;
	pRegSample->ulTheshold = ulThreshold;
	pRegSample->eSampleAlg = eAlg;
	pRegSample->bUpdated = false;

	for(i = 0; i < pRegSample->ulTheshold; i++)
	{
		aulSample[i] = 0;
	}

	return;
}


/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Vdec_InitAwc
	( BVDC_Vdec_AwcSettings           *pAwc,
	  const BVDC_P_Source_Info        *pUserInfo )
{
	uint32_t i;

	for(i = 0; i < BVDC_COLOR_CHANNELS; i++)
	{
		pAwc->abEnable[i] = pUserInfo->bFullRange ? false : true;
	}

	return;
}


/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Vdec_InitCti
	( BVDC_Vdec_CtiSettings           *pCti,
	  const BVDC_P_Source_Info        *pUserInfo )
{
	pCti->bEnable     = true;
	pCti->eIirIfDelay = BVDC_Vdec_IirIfDelay_eOne;
	pCti->eIirFilter1 = BVDC_Vdec_IirFilter_eBypass;

	if(pUserInfo->bIfdInput)
	{
		pCti->ulVChnGain  = 31;
		pCti->ulUChnGain  = 31;
		pCti->ulClampWin  = 13;
		pCti->eBandFilter = BVDC_Vdec_BandLimitFilter_eBw_060;
		pCti->eFilterTaps = BVDC_Vdec_FilterTaps_e31_Taps;

		pCti->eFirFilter1 = BVDC_Vdec_FirFilter_eBypass;
		pCti->eFirFilter2 = BVDC_Vdec_FirFilter_eFir_3;
	}
	else if(VIDEO_FORMAT_IS_PAL(pUserInfo->pFmtInfo->eVideoFmt))
	{
		pCti->ulVChnGain  = 15;
		pCti->ulUChnGain  = 15;
		pCti->ulClampWin  = 5;
		pCti->eBandFilter = BVDC_Vdec_BandLimitFilter_eBypass;
		pCti->eFilterTaps = BVDC_Vdec_FilterTaps_e17_Taps;

		pCti->eFirFilter1 = BVDC_Vdec_FirFilter_eBypass;
		pCti->eFirFilter2 = BVDC_Vdec_FirFilter_eBypass;
	}
	else if(VIDEO_FORMAT_IS_SECAM(pUserInfo->pFmtInfo->eVideoFmt))
	{
		pCti->ulVChnGain  = 31;
		pCti->ulUChnGain  = 31;
		pCti->ulClampWin  = 10;
		pCti->eBandFilter = BVDC_Vdec_BandLimitFilter_eBw_060;
		pCti->eFilterTaps = BVDC_Vdec_FilterTaps_e17_Taps;

		pCti->eFirFilter1 = BVDC_Vdec_FirFilter_eBypass;
		pCti->eFirFilter2 = BVDC_Vdec_FirFilter_eBypass;
	}
	else /* NTSC or Others */
	{
		pCti->ulVChnGain  = 4;
		pCti->ulUChnGain  = 4;
		pCti->ulClampWin  = 2;
		pCti->eBandFilter = BVDC_Vdec_BandLimitFilter_eBypass;
		pCti->eFilterTaps = BVDC_Vdec_FilterTaps_e11_Taps;

		pCti->eFirFilter1 = BVDC_Vdec_FirFilter_eBypass;
		pCti->eFirFilter2 = BVDC_Vdec_FirFilter_eBypass;
	}

	return;
}


/***************************************************************************
 * {private}
 *
 * Trigger by:
 *   (1) macrovision
 *   (2) input format
 *   (3) input type
 */
static void BVDC_P_Vdec_InitDcr
	( BVDC_Vdec_DcrSettings           *pDcr,
	  const BVDC_P_Source_Info        *pUserInfo,
	  BVDC_MacrovisionType             eMvType )
{
	uint32_t i;

	/* Format dependence */
	if(BVDC_MacrovisionType_eNoProtection == eMvType)
	{
		if((pUserInfo->bIfdInput) ||
		   (pUserInfo->bCvbsInput) ||
		   (pUserInfo->bSvidInput) ||
		   (pUserInfo->bCompInput && pUserInfo->pVdcFmt->bSd))
		{
			pDcr->ulStart = 7;
			pDcr->ulEnd = 125; /*-3*/
		}
		else if(pUserInfo->bCompInput && pUserInfo->pVdcFmt->bEd)
		{
			pDcr->ulStart = 7;
			pDcr->ulEnd = 1;
		}
		else
		{
			pDcr->ulStart = 6;
			pDcr->ulEnd = 127;
		}
	}
	else
	{
		if((pUserInfo->bIfdInput || pUserInfo->bCvbsInput || pUserInfo->bSvidInput || pUserInfo->bCompInput) &&
		   (VIDEO_FORMAT_IS_525_LINES(pUserInfo->pFmtInfo->eVideoFmt)))
		{
			pDcr->ulStart = 19;
			pDcr->ulEnd = 122; /*-6*/
		}
		else if((pUserInfo->bIfdInput || pUserInfo->bCvbsInput || pUserInfo->bSvidInput || pUserInfo->bCompInput) &&
		        (VIDEO_FORMAT_IS_625_LINES(pUserInfo->pFmtInfo->eVideoFmt)))
		{
			pDcr->ulStart = 24;
			pDcr->ulEnd = 122;  /*-6*/
		}
		else if(pUserInfo->bCompInput && pUserInfo->pVdcFmt->bEd)
		{
			pDcr->ulStart = 15;
			pDcr->ulEnd = 1;
		}
		else
		{
			pDcr->ulStart = 6;
			pDcr->ulEnd = 127;
		}
	}

	/* PR51338: support fullrange */
	/* PR42424: Fine tuned dcr settings for component/pc input.*/
	for(i = 0; i < BVDC_COLOR_CHANNELS; i++)
	{
		if(pUserInfo->bFullRange)
		{
			pDcr->aulRef[i] = (BVDC_MacrovisionType_eNoProtection == eMvType)
				? 391 : 316;
		}
		else
		{
			if((!pUserInfo->bCompInput) &&
			   (VIDEO_FORMAT_IS_SPECIAL_BLANKLEVEL(pUserInfo->pFmtInfo->eVideoFmt)))
			{
				pDcr->aulRef[i] = 240;
			}
			else
			{
				pDcr->aulRef[i] = 252;
			}
		}

		pDcr->aeAccumType[i]  = BVDC_Vdec_DcrGain_eX1;
		pDcr->aeDirectType[i] = BVDC_Vdec_DcrGain_eX1;
	}

	return;
}


/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Vdec_InitComb
	( BVDC_Vdec_CombSettings          *pComb )
{
	/* VDEC_YC_0_YC_3D_BLEND_x */
	pComb->aul3DBlendConstant[0] = 32;
	pComb->aul3DBlendConstant[1] = 24;
	pComb->aul3DBlendConstant[2] = 20;
	pComb->aul3DBlendConstant[3] = 18;
	pComb->aul3DBlendConstant[4] = 18;
	pComb->aul3DBlendConstant[5] = 20;
	pComb->aul3DBlendConstant[6] = 24;
	pComb->aul3DBlendConstant[7] = 32;

	/* VDEC_YC_0_YC_SHARPENING_CHROMA_BLEND_x */
	pComb->aul3DChromaSharpness[0] = 1;
	pComb->aul3DChromaSharpness[1] = 2;
	pComb->aul3DChromaSharpness[2] = 3;
	pComb->aul3DChromaSharpness[3] = 4;
	pComb->aul3DChromaSharpness[4] = 205;
	pComb->aul3DChromaSharpness[5] = 171;
	pComb->aul3DChromaSharpness[6] = 146;
	pComb->aul3DChromaSharpness[7] = 128;

	/* VDEC_YC_0_YC_2D_GAIN.SHARPENING_CHROMA */
	pComb->ul2DChromaSharpnessGain = 2;

	/* VDEC_YC_0_YC_3D_BASIC_FRAME_ERR */
	pComb->ulTbDiffThresold = 3;
	pComb->ulObLumaDiffGain = 3;
	pComb->ulIBLumaDiffGain = 4;
	pComb->ulReductionCoef  = 2;

	return;
}


/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Vdec_InitAnalogGain
	( BVDC_Vdec_AnalogGainSettings    *pAGain )
{
	uint32_t i;

	for(i = 0; i < BVDC_COLOR_CHANNELS; i++)
	{
		pAGain->aulGain[i] = 0;
	}

	return;
}


/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Vdec_InitDigitalGain
	( BVDC_Vdec_DigitalGainSettings   *pDGain,
	  const BVDC_P_Source_Info        *pUserInfo,
	  BVDC_MacrovisionType             eMvType )
{
	uint32_t i;

	pDGain->bMasterMode = pUserInfo->bPcInput ? false : true;

	for(i = 0; i < BVDC_COLOR_CHANNELS; i++)
	{
		if(pUserInfo->bFullRange)
		{
			pDGain->aulRef[i] = (BVDC_MacrovisionType_eNoProtection == eMvType)
				? 375 : 300;
		}
		else
		{
			/* PR35333: set DCR  reference blank level for different video format.*/
			if((!pUserInfo->bCompInput) &&
			   (VIDEO_FORMAT_IS_SPECIAL_BLANKLEVEL(pUserInfo->pFmtInfo->eVideoFmt)))
			{
				pDGain->aulRef[i] = (BVDC_MacrovisionType_eNoProtection == eMvType)
					? 224 : 168;
			}
			else
			{
				pDGain->aulRef[i] = (BVDC_MacrovisionType_eNoProtection == eMvType)
					? 236 : 189;
			}
		}

		pDGain->aulGain[i]      = 512;
		pDGain->aulTimeConst[i] = 2;
		pDGain->bManualGain[i]  = false;
	}

	return;
}


/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Vdec_InitDelay
	( BVDC_Vdec_DelaySettings         *pDelay,
	  const BVDC_P_Source_Info        *pUserInfo )
{

	/* Get Y delay based on input format */
	pDelay->ulYDelay  = (
		pUserInfo->bCompInput &&
		pUserInfo->pVdcFmt->bSd) ? 1 : 0;

	/* Cr */
	pDelay->ulCrDelay =
		(pUserInfo->bSvidInput) ? 1 : 0;

	/* Cb */
	pDelay->ulCbDelay =
		(pUserInfo->bSvidInput) ? 1 : 0;

	/* Delay */
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
	if((VIDEO_FORMAT_IS_NTSC(pUserInfo->pFmtInfo->eVideoFmt)) &&
	   (pUserInfo->bIfdInput || pUserInfo->bCvbsInput || pUserInfo->bSvidInput))
	{
		pDelay->ulIfCompLumaBypass = 27;
	}
	else
	{
		pDelay->ulIfCompLumaBypass = 26;
	}
#else
	pDelay->ulIfCompLumaBypass = 25;
#endif

	return;
}


/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Vdec_InitBackend
	( BVDC_Vdec_BackendSettings       *pBackend,
	  const BVDC_P_Source_Info        *pUserInfo )
{
	const BFMT_VideoInfo *pFmtInfo = pUserInfo->pFmtInfo;
	const BVDC_P_FormatInfo *pVdcFmt = pUserInfo->pVdcFmt;

	/* Initialize */
	pBackend->bCoring = true;

	/* Backend filter settings!  This require a lot of PQ turning!  Here
	 * are refsw optimal values. */

	if(pUserInfo->bPcInput)
	{
		pBackend->ulKStop     = 0x20;
		pBackend->ulKPass     = 0;
		pBackend->eBandFilter = BVDC_Vdec_BackEndBandLimitFilter_eBw_B60;
	}
	else if(pVdcFmt->bHd)
	{
		if((BFMT_VideoFmt_e1080p_24Hz == pFmtInfo->eVideoFmt)||
		   (BFMT_VideoFmt_e1080p_25Hz == pFmtInfo->eVideoFmt)||
		   (BFMT_VideoFmt_e1080p_30Hz == pFmtInfo->eVideoFmt)||
		   (BFMT_VideoFmt_e1080p_50Hz == pFmtInfo->eVideoFmt)||
		   (BFMT_VideoFmt_e1080p      == pFmtInfo->eVideoFmt))
		{
			pBackend->ulKStop     = 0x20;
			pBackend->ulKPass     = 16;
			pBackend->eBandFilter = BVDC_Vdec_BackEndBandLimitFilter_eBw_B38;
		}
		else
		{
			pBackend->ulKStop     = 0;
			pBackend->ulKPass     = 0;
			pBackend->eBandFilter = BVDC_Vdec_BackEndBandLimitFilter_eBw_B38;
		}
	}
	else /* Sd & Ed */
	{
		pBackend->ulKStop     = 0;
		pBackend->ulKPass     = 16;
		if(VIDEO_FORMAT_IS_480P(pFmtInfo->eVideoFmt))
		{
			pBackend->eBandFilter = BVDC_Vdec_BackEndBandLimitFilter_eBw_B65;
		}
		else
		{
			pBackend->eBandFilter = BVDC_Vdec_BackEndBandLimitFilter_eBw_B60;
		}
	}

	return;
}


/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Vdec_InitBlackOffset
	( uint32_t                        *pulFeBlankLevel,
	  uint32_t                        *pulBeBlankLevel,
	  uint32_t                        *pulBlackOffset,
	  uint32_t                        *pulBlackOffsetInCmp,
	  const BVDC_P_Source_Info        *pUserInfo,
	  BVDC_MacrovisionType             eMvType )
{
	/* KLUDGE:
	 * Removed pedetal for vdec's CVBS/IFD input source!
	 * K = 42 * 1.6909= 71  (1.6909 is multiplier of Luma in CSC of BE in VDEC)
	 * and then it is converted to S11.2 = 0x11C in hex.  Hence instead
	 * of do it in VDEC_FE_0_FE_CONFIG.BLACK_OFFSET we're doing it
	 * here: CMP_0_V0_COEFF_C03_C02.COEFF_C3.
	 * There is an issue from customer which displays blacker than blank
	 * (they have a pattern with 11 gray steps which 3 steps are below
	 * blank level).  But in VDEC we remove pedestal (42) no matter whether
	 * there is pedestal in the source.  So in this pattern we lose some
	 * steps in the output of the VDEC, and we can not display it by
	 * changing brightness in the back end of the chip(compositor). */
	if((((BFMT_VideoFmt_eNTSC == pUserInfo->pFmtInfo->eVideoFmt) && (!pUserInfo->bCompInput)) ||
	    (BFMT_VideoFmt_ePAL_M == pUserInfo->pFmtInfo->eVideoFmt) ||
	    (BFMT_VideoFmt_ePAL_N == pUserInfo->pFmtInfo->eVideoFmt) ||
	    (BFMT_VideoFmt_eNTSC_443 == pUserInfo->pFmtInfo->eVideoFmt)) &&
	   (!pUserInfo->bPcInput))
	{
#if (BVDC_P_REMOVE_PEDESTAL_IN_CMP)
		*pulBlackOffset      = 0;
		*pulBlackOffsetInCmp = 0x11c;
#else
		*pulBlackOffset      = 0x2a;
		*pulBlackOffsetInCmp = 0;
#endif
	}
	else
	{
		*pulBlackOffset      = 0;
		*pulBlackOffsetInCmp = 0;
	}

	/* PR51338: support fullrange */
	/* PR42424: Fine tuned dcr settings for component/pc input.*/
	if(pUserInfo->bFullRange)
	{
		*pulFeBlankLevel = (BVDC_MacrovisionType_eNoProtection == eMvType)
			? 391 : 316;
		*pulBeBlankLevel = 64;
	}
	else
	{
		if((!pUserInfo->bCompInput) &&
		   (VIDEO_FORMAT_IS_SPECIAL_BLANKLEVEL(pUserInfo->pFmtInfo->eVideoFmt)))
		{
			*pulFeBlankLevel = 240;
			*pulBeBlankLevel = 240;
		}
		else
		{
			*pulFeBlankLevel = 252;
			*pulBeBlankLevel = 252;
		}
	}

	return;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Vdec_InitMisc
	( BVDC_Vdec_Settings              *pVdecSetting,
	  const BVDC_P_Source_InfoPtr      pUserInfo )
{
	uint32_t ulFeBlankLevel;
	uint32_t ulBeBlankLevel;
	uint32_t ulBlackOffsetInCmp;

	/* Initial value */
	pVdecSetting->bOverSample       = false;
	pVdecSetting->bBypassAcgc       = false;
	pVdecSetting->bBypassCommonMode = false;
	pVdecSetting->eVcrMode          = BVDC_Vdec_VcrMode_eAuto;
	pVdecSetting->eNoiseMode        = BVDC_Vdec_NoiseMode_eAuto;
	pVdecSetting->e3DGameMode       = BVDC_Vdec_3DGameMode_eGameFull;
	pVdecSetting->eDefaultNoBurst625LinesFormat = BFMT_VideoFmt_ePAL_G;
	pVdecSetting->eDefaultNoBurst525LinesFormat = BFMT_VideoFmt_eNTSC;

	/* Initialize back offset according to format and input */
	BVDC_P_Vdec_InitBlackOffset(&ulFeBlankLevel, &ulBeBlankLevel,
		&pVdecSetting->ulBlackOffset, &ulBlackOffsetInCmp, pUserInfo,
		BVDC_MacrovisionType_eNoProtection);

	return;
}


/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Vdec_Create
	( BVDC_P_Vdec_Handle              *phVdec,
	  BVDC_P_VdecId                    eVdecId,
	  BREG_Handle                      hReg,
	  BVDC_Source_Handle               hSource )
{
	BVDC_P_VdecContext *pVdec;

	BDBG_ENTER(BVDC_P_Vdec_Create);
	BDBG_ASSERT(phVdec);

	/* BDBG_SetModuleLevel("BVDC_VDEC", BDBG_eMsg);  */

	/* Get Source context */
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	/* The handle will be NULL if create fails. */
	*phVdec = NULL;

	/* (1) Alloc the context. */
	pVdec = (BVDC_P_VdecContext*)
		(BKNI_Malloc(sizeof(BVDC_P_VdecContext)));
	if(!pVdec)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pVdec, 0x0, sizeof(BVDC_P_VdecContext));
	BDBG_OBJECT_SET(pVdec, BVDC_DEC);

	/* (2a) Alloc device memory for ATSC FD table! */
	pVdec->pulAtscFdtMem = BVDC_P_Vdec_FmtTblAlloc(hSource->hVdc->hMemory,
		s_aAtscFdtFmtTbl, BVDC_P_ATSC_FDT_COUNT);
	if(!pVdec->pulAtscFdtMem)
	{
		BDBG_OBJECT_DESTROY(pVdec, BVDC_DEC);
		BKNI_Free((void*)pVdec);
		return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
	}

	/* ATSC Offset: Convert to device offset for ease of use */
	BMEM_Heap_ConvertAddressToOffset(hSource->hVdc->hMemory,
		pVdec->pulAtscFdtMem, &pVdec->ulAtscFdtMemOffset);

	/* (2b) Alloc device memory for VESA FD table! */
	pVdec->pulVesaFdtMem = BVDC_P_Vdec_FmtTblAlloc(hSource->hVdc->hMemory,
		s_aVesaFdtFmtTbl, BVDC_P_VESA_FDT_COUNT);
	if(!pVdec->pulVesaFdtMem)
	{
		BDBG_OBJECT_DESTROY(pVdec, BVDC_DEC);
		BKNI_Free((void*)pVdec);
		return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
	}

	/* VESA Offset: Convert to device offset for ease of use */
	BMEM_Heap_ConvertAddressToOffset(hSource->hVdc->hMemory,
		pVdec->pulVesaFdtMem, &pVdec->ulVesaFdtMemOffset);

	/* Store the id & hRegister for activating the triggers. */
	pVdec->eId          = eVdecId;
	pVdec->hReg         = hReg;
	pVdec->hSource      = hSource;

	/* (4a) Alloc scratch registers.  Fatal error!  Not recovering! */
	pVdec->ulPhaseAddr  = BRDC_AllocScratchReg(hSource->hVdc->hRdc);
	pVdec->ulFeSizeAddr = BRDC_AllocScratchReg(hSource->hVdc->hRdc);
	if(!pVdec->ulPhaseAddr || !pVdec->ulFeSizeAddr)
	{
		BDBG_OBJECT_DESTROY(pVdec, BVDC_DEC);
		BKNI_Free((void*)pVdec);
		return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
	}

	/* Getting the offset of various VDEC sub-modules.  The offset is
	 * from the VDEC_x_0.  Where x is FE, BE, YC, CD, and etc. */
	if(BVDC_P_VdecId_eVdec1 == pVdec->eId)
	{
		BDBG_ASSERT(BVDC_P_SUPPORT_VDEC > 1);
#if (BVDC_P_SUPPORT_VDEC > 1)
		pVdec->ulFdOffset  = BCHP_FD_1_REG_START - BCHP_FD_0_REG_START;
		pVdec->ulFeOffset  = BCHP_VDEC_FE_1_REG_START - BCHP_VDEC_FE_0_REG_START;
		pVdec->ulBeOffset  = BCHP_VDEC_BE_1_REG_START - BCHP_VDEC_BE_0_REG_START;
		pVdec->ulCdOffset  = BCHP_VDEC_CD_1_REG_START - BCHP_VDEC_CD_0_REG_START;
		pVdec->ulYcOffset  = BCHP_VDEC_YC_1_REG_START - BCHP_VDEC_YC_0_REG_START;
		pVdec->ulRstOffset = BCHP_VD_TOP_0_VDEC1_RESET - BCHP_VD_TOP_0_VDEC0_RESET;
		pVdec->ulTopOffset = BCHP_VD_TOP_0_VD_VDEC_1_CTL - BCHP_VD_TOP_0_VD_VDEC_0_CTL;
#endif
	}

#if (BVDC_P_VDEC_SECAM_TTD_BYPASS_FILTER_IN_VBI)
	/* (5): Alloc slot to execute at specific EOP/SOP or at specific line. */
	BRDC_Slot_Create(hSource->hVdc->hRdc, &pVdec->hCapSlot[0]);
	BRDC_List_Create(hSource->hVdc->hRdc, 0x100, &pVdec->hCapList[0]);
	BRDC_Slot_Create(hSource->hVdc->hRdc, &pVdec->hCapSlot[1]);
	BRDC_List_Create(hSource->hVdc->hRdc, 0x100, &pVdec->hCapList[1]);
	{
		uint32_t ulAndMask, ulOrMask;
		uint32_t ulNumEntries, *pulStart, *pulCurrent;

		/* Turn ON */
		             BRDC_List_GetNumEntries_isr(pVdec->hCapList[0], &ulNumEntries);
		pulStart   = BRDC_List_GetStartAddress_isr(pVdec->hCapList[0]);
		pulCurrent = pulStart + ulNumEntries;
		ulAndMask =
			BCHP_MASK(VDEC_CD_0_SECAM_CTL, LUMA_LOW_PASS) |
			BCHP_MASK(VDEC_CD_0_SECAM_CTL, CHROMA_REJECT);
		ulOrMask  =
			BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_CTL, LUMA_LOW_PASS, ENABLE) |
			BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_CTL, CHROMA_REJECT, ENABLE);
		BVDC_P_RD_MOD_WR_RUL(pulCurrent, ~ulAndMask, ulOrMask,
		BCHP_VDEC_CD_0_SECAM_CTL + pVdec->ulCdOffset);

		ulAndMask =
			BCHP_MASK(VDEC_CD_0_SECAM_ADAPT, ALPHA_MODE  ) |
			BCHP_MASK(VDEC_CD_0_SECAM_ADAPT, MANUAL_ALPHA);
		ulOrMask  =
			BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_ADAPT, ALPHA_MODE, AUTO) |
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_ADAPT, MANUAL_ALPHA, 0   );
		BVDC_P_RD_MOD_WR_RUL(pulCurrent, ~ulAndMask, ulOrMask,
		BCHP_VDEC_CD_0_SECAM_ADAPT + pVdec->ulCdOffset);

		ulNumEntries = (uint32_t)(pulCurrent - pulStart);
		BRDC_List_SetNumEntries_isr(pVdec->hCapList[0], ulNumEntries);
		BRDC_Slot_SetList_isr(pVdec->hCapSlot[0], pVdec->hCapList[0]);
		BRDC_Slot_ExecuteOnTrigger_isr(pVdec->hCapSlot[0], BRDC_Trigger_eCap0Trig0, true);

		/* Turn OFF */
		             BRDC_List_GetNumEntries_isr(pVdec->hCapList[1], &ulNumEntries);
		pulStart   = BRDC_List_GetStartAddress_isr(pVdec->hCapList[1]);
		pulCurrent = pulStart + ulNumEntries;
		ulAndMask =
			BCHP_MASK(VDEC_CD_0_SECAM_CTL, LUMA_LOW_PASS) |
			BCHP_MASK(VDEC_CD_0_SECAM_CTL, CHROMA_REJECT);
		ulOrMask  =
			BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_CTL, LUMA_LOW_PASS, DISABLE) |
			BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_CTL, CHROMA_REJECT, DISABLE);
		BVDC_P_RD_MOD_WR_RUL(pulCurrent, ~ulAndMask, ulOrMask,
		BCHP_VDEC_CD_0_SECAM_CTL + pVdec->ulCdOffset);

		ulAndMask =
			BCHP_MASK(VDEC_CD_0_SECAM_ADAPT, ALPHA_MODE  ) |
			BCHP_MASK(VDEC_CD_0_SECAM_ADAPT, MANUAL_ALPHA);
		ulOrMask  =
			BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_ADAPT, ALPHA_MODE, MANUAL) |
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_ADAPT, MANUAL_ALPHA, 0   );
		BVDC_P_RD_MOD_WR_RUL(pulCurrent, ~ulAndMask, ulOrMask,
		BCHP_VDEC_CD_0_SECAM_ADAPT + pVdec->ulCdOffset);

		ulNumEntries = (uint32_t)(pulCurrent - pulStart);
		BRDC_List_SetNumEntries_isr(pVdec->hCapList[1], ulNumEntries);
		BRDC_Slot_SetList_isr(pVdec->hCapSlot[1], pVdec->hCapList[1]);
		BRDC_Slot_ExecuteOnTrigger_isr(pVdec->hCapSlot[1], BRDC_Trigger_eCap0Trig1, true);
	}
#endif

	/* (6) All done. now return the new fresh context to user. */
	*phVdec = (BVDC_P_Vdec_Handle)pVdec;

	BDBG_LEAVE(BVDC_P_Vdec_Create);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Vdec_Destroy
	( BVDC_P_Vdec_Handle               hVdec )
{
	BDBG_ENTER(BVDC_P_Vdec_Destroy);
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	BDBG_OBJECT_ASSERT(hVdec->hSource, BVDC_SRC);

#if (BVDC_P_VDEC_SECAM_TTD_BYPASS_FILTER_IN_VBI)
	BRDC_Slot_Destroy(hVdec->hCapSlot[0]);
	BRDC_List_Destroy(hVdec->hCapList[0]);
	BRDC_Slot_Destroy(hVdec->hCapSlot[1]);
	BRDC_List_Destroy(hVdec->hCapList[1]);
#endif

	/* [4] Free scratch registers. */
	BRDC_FreeScratchReg(hVdec->hSource->hVdc->hRdc, hVdec->ulPhaseAddr);
	BRDC_FreeScratchReg(hVdec->hSource->hVdc->hRdc, hVdec->ulFeSizeAddr);

	/* [2] Release device memory for FD table! */
	BVDC_P_Vdec_FmtTblFree(hVdec->hSource->hVdc->hMemory, hVdec->pulAtscFdtMem);
	BVDC_P_Vdec_FmtTblFree(hVdec->hSource->hVdc->hMemory, hVdec->pulVesaFdtMem);

	BDBG_OBJECT_DESTROY(hVdec, BVDC_DEC);
	/* [1] Release context in system memory */
	BKNI_Free((void*)hVdec);

	BDBG_LEAVE(BVDC_P_Vdec_Destroy);
	return;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Vdec_Init
	( BVDC_P_Vdec_Handle               hVdec )
{
	uint32_t  ulLfsrCtrlT0, ulLfsrCtrlT1, ulLfsrCtrlT2, ulLfsrValue;

	BDBG_ENTER(BVDC_P_Vdec_Init);
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);

	/* VDEC states */
	hVdec->b3DComb           = false;
	hVdec->bVcrHeadSwitch    = false;
	hVdec->bVideoDetected    = false;
	hVdec->bHPllLocked       = false;
	hVdec->b240pMode         = false;
	hVdec->bNoisy            = false;
	hVdec->bManualColorMode  = false;
	hVdec->bManualAcgc       = false;
	hVdec->bOverSample       = false;
	hVdec->ulVdecPhase       = 0;
	hVdec->ulVcrTheshold     = 0;
	hVdec->eMacrovisionType  = BVDC_MacrovisionType_eNoProtection;
	hVdec->bMvAgc            = false;
	hVdec->ulMvCs            = 0;

	hVdec->bCcrEnable        = true;
	hVdec->ulNoiseHistory    = BVDC_P_VDEC_NOISE_LEVEL_MAX;
	hVdec->ulAutoColorModeOffThreshold = BVDC_P_VDEC_ABOVE_33DB_AUTO_COLOR_MODE_OFF;
	hVdec->ulAutoColorModeOnThreshold  = BVDC_P_VDEC_ABOVE_33DB_AUTO_COLOR_MODE_ON;
	hVdec->ulManualColorModeColorOnAmp  = BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_ON_AMP;
	hVdec->ulManualColorModeColorOffAmp = BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_OFF_AMP;
	hVdec->ulNoBurstThreshold = BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH_UP_33DB;
	hVdec->ulBlankSelLines   = 0; /* blank select */

	/* Color Burst (Acgc) */
	hVdec->ulColorBurstGain  = BVDC_P_ACGC_GAIN_NOMINAL;
	hVdec->ulCrGain          = BVDC_P_ACGC_CR_DEF_GAIN;
	hVdec->ulCbGain          = BVDC_P_ACGC_CB_DEF_GAIN;
	hVdec->ulColorBurstAmpDelta = 0;
	hVdec->bColorBurstAmpUpdated = false;

	/*subcarrier lock status*/
	hVdec->ulSubCarrUnlockTimer = 0;
	hVdec->ulFormatChangeTimer  = 0;

	/*PR33011, PR33012*/
	hVdec->ulClearSubcarrierCnt = 0;
	hVdec->ulChromaUnlockCnt    = 0;
	hVdec->ulBlackWhiteCnt      = 0;

	/* CSC min/max */
	hVdec->ulCscMin             = 0;
	hVdec->ulCscMax             = 1023;

	/*PR31869*/
	hVdec->bUseCommonMode       = false;

	/* Timer for switch mode of averaging */
	hVdec->ulNoiseSwitchTimer   = BVDC_P_VDEC_NOISE_DELAY_TO_NORMAL;

	/* Delay start feeding to avoid garbarge. */
	hVdec->ulDelayStart         = BVDEC_P_BVB_OUT_SLOW_START_COUNT;

	/*Vsync count form switching source, format change or hpll status toggle*/
	hVdec->ulVsyncCnt           = 0;

	/* ADC change count from switching source or channel*/
	hVdec->ulAdcChangCnt = 0;

	/*FD detection, Delay FD switch clamp*/
	hVdec->ulClampSelFdCnt      = 0;
	hVdec->ulFdFmtCnt           = 0;
	hVdec->ulLastFmtId          = 0;

	/*Auto detection for RF signal*/
	hVdec->bDisableRfAutoDetection = false;
	hVdec->ulFmtId                 = 0;
	hVdec->bNewFmt                 = false;

	hVdec->bPcFieldSyncTypeToggle  = false;

	/*VDEC fine tuning flag*/

	/*PR40449: Workaround for special 1080i/720p component streams with bi-levle Hsync insteadof tri-level.*/
	hVdec->bVdecTuningDirtyBitBilevel = 0;
	hVdec->bHdBilevelDetected   = false;
	hVdec->bNoInputSignal       = true;
	hVdec->ulHdBilevelSwitchCnt = 0;
	hVdec->bFieldAvgErr         = false;
	hVdec->ulFieldAvgErrCnt     = 0;

	/*PR49561: [VDEC]: Vertical black bar on the white noise channel*/
	hVdec->bVdecTuningDirtyBitResetHStart  = false;
	hVdec->bResetHStart                    = false;
	hVdec->bVdecTuningDirtyBitEnableSliceLPFHpllCtrl = false;  /*dirty bit for hsync reset*/
	hVdec->bCompEnableSliceLPFHpllCtrl     = false;
	hVdec->bVdecTuningDirtyBitResetHsync   = false;
	hVdec->bResetHsyncPllAccum             = false;

	hVdec->bToggleHthreshold               = false;
	hVdec->ulCountToggleHthreshold         = 0;
	hVdec->bVdecTuningDirtyBitCcr          = false;

#if (BVDC_P_VDEC_PR32896_WORK_AROUND)
	/* Copy protection detection */
	{
		uint32_t id;
		hVdec->ulMvId            = 0;
		hVdec->ulMvDetectedCount = 0;
		for(id = 0; id < BVDC_P_VDEC_MV_STATUS_COUNT; id++)
		{
			hVdec->aulMvDetected[id] = 0;
		}
	}
#endif

	/* Calibration ADC & SIF ADC for external IFD. */
	{
		/* Hush compiler! */
		BVDC_P_AdcEntry stTmpEntry0 = BVDC_P_MAKE_ADC(BVDC_Adc_0, 1, SD_R, NONE, NONE);
		BVDC_P_AdcEntry stTmpEntry1 =
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		{
			(BVDC_Adc_5),                                   /* ulAdc;        */
			(5-1),                                          /* ulId;         */
			(BCHP_ADC_5_REG_START - BCHP_ADC_1_REG_START),  /* ulOffset;     */
			(0),                                            /* ulChnSel;     */
			(BCHP_ADC_5_SELECT_ANALOG_INPUT_SD_SIF1),       /* ulInputSel;   */
			(BCHP_ADC_1_SELECT_ANALOG_INPUT_NONE),          /* ulCalibSel;   */
			(BCHP_ADC_1_INOFF_SIF1_MASK),                   /* ulInOffMask;  */
			(BCHP_ADC_1_INOFF_SIF1_SHIFT)                   /* ulInOffShift; */
		};
#else
			BVDC_P_MAKE_ADC(BVDC_Adc_4, 4, SD_SIF, NONE, SIF1);
#endif

		/* SIF and Calibration ADC */
		hVdec->stCalibAdc = stTmpEntry0;
		hVdec->stSifAdc   = stTmpEntry1;
	}

	/*PR41315: [VDEC] Add support for SECAM format.*/
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
	hVdec->bSecamColorResetAccum   = 0;
	hVdec->ulSecamColorCsyncSat    = 1024;
	hVdec->ulSecamColorCsyncThresh = 2;
	hVdec->ulSecamColorChromaWidth = 64;
	hVdec->ulSecamAcgcLoopGain     = 2;
	hVdec->bClearSubCarAccum       = 0;
	hVdec->ulSecamColorLimit       = 9;
	hVdec->ulSecamAccEnd           = 1124;
	hVdec->ulSecamAccStart         = 100;
	hVdec->ulSecamActiveEndLine    = 311;
	hVdec->ulSecamActiveStartLine  = 23;
#endif

	/*init value for vdec workaround*/
	hVdec->bPalSeparation = true;
	hVdec->bFh3DComb = true;
	hVdec->bFv3DComb = true;
	hVdec->bClk3DComb = true;

	/* PR43823: Fh test by ShibaSoku-color flashing*/
	hVdec->ulYcCrtlLineDelay = 65;
	hVdec->bYcCrtlColorsyncInvert = false;

	/* CUSTOMIZED: Awc */
	BVDC_P_Vdec_InitAwc(&hVdec->hSource->stNewInfo.stAwc,
		&hVdec->hSource->stNewInfo);

	/* CUSTOMIZED: Cti */
	BVDC_P_Vdec_InitCti(&hVdec->hSource->stNewInfo.stCti,
		&hVdec->hSource->stNewInfo);

	/* CUSTOMIZED: Dcr */
	BVDC_P_Vdec_InitDcr(&hVdec->hSource->stNewInfo.stDcr,
		&hVdec->hSource->stNewInfo, hVdec->eMacrovisionType);

	/* CUSTOMIZED: Comb */
	BVDC_P_Vdec_InitComb(&hVdec->hSource->stNewInfo.stComb);

	/* CUSTOMIZED: Analog Gain */
	BVDC_P_Vdec_InitAnalogGain(&hVdec->hSource->stNewInfo.stAGain);

	/* CUSTOMIZED: Digital Gain */
	BVDC_P_Vdec_InitDigitalGain(&hVdec->hSource->stNewInfo.stDGain,
		&hVdec->hSource->stNewInfo, hVdec->eMacrovisionType);

	/* CUSTOMIZED: YCrCb delay */
	BVDC_P_Vdec_InitDelay(&hVdec->hSource->stNewInfo.stDelay,
		&hVdec->hSource->stNewInfo);

	/* CUSTOMIZED: Backend */
	BVDC_P_Vdec_InitBackend(&hVdec->hSource->stNewInfo.stBackend,
		&hVdec->hSource->stNewInfo);

	/* CUSTOMIZED: misc fields */
	BVDC_P_Vdec_InitMisc(&hVdec->hSource->stNewInfo.stVdecSettings,
		&hVdec->hSource->stNewInfo);

	/* YUV -> SDYCrCb */
	BVDC_P_Csc_GetVdecTable_isr(&hVdec->stCsc, BVDC_P_VdecColorSpace_eYuv,
		BVDC_P_VdecColorSpace_eNtscSdYCrCb, hVdec->hSource->stNewInfo.pFmtInfo, false);

	BVDC_P_Vdec_SampleInit(&hVdec->stNoiseLevel, hVdec->aulNoiseLevel,
		BVDC_P_VDEC_NOISE_LEVEL_MAX,
		BVDC_P_VDEC_NOISE_LEVEL_NORM_COUNT,
		BVDC_P_SampleAlg_eMean);

	BVDC_P_Vdec_SampleInit(&hVdec->stSampleCount, hVdec->aulSampleCount,
		0,
		BVDC_P_VDEC_SAMPLE_COUNT,
		BVDC_P_SampleAlg_eMedian);

	BVDC_P_Vdec_SampleInit(&hVdec->stLinePerField, hVdec->aulLinePerField,
		0,
		BVDC_P_VDEC_LINE_PER_FIELD_COUNT,
		BVDC_P_SampleAlg_eMean);

	BVDC_P_Vdec_SampleInit(&hVdec->stSamplePerLine, hVdec->aulSamplePerLine,
		0,
		BVDC_P_VDEC_SAMPLER_PER_LINE_COUNT,
		BVDC_P_SampleAlg_eMean);

	BVDC_P_Vdec_SampleInit(&hVdec->stColorBurstAmp, hVdec->aulColorBurstAmp,
		0,
		BVDC_P_VDEC_COLORBURST_AMP_FAST_COUNT,
		BVDC_P_SampleAlg_eMean);

	BVDC_P_Vdec_SampleInit(&hVdec->stSubCarLoopAcc, hVdec->aulSubCarLoopAcc,
		0,
		BVDC_P_VDEC_SUBCARRIER_LOOP_ACCU_COUNT,
		BVDC_P_SampleAlg_eMedian);

	BVDC_P_Vdec_SampleInit(&hVdec->stAutoAcgcGain, hVdec->aulAutoAcgcGain,
		0,
		BVDC_P_VDEC_AUTO_ACGC_GAIN_COUNT,
		BVDC_P_SampleAlg_eMedian);

	/* Dither */
	ulLfsrValue  = BVDC_P_DITHER_VDEC_FE_LFSR_VALUE;
	ulLfsrCtrlT0 = BVDC_P_DITHER_VDEC_FE_LFSR_CTRL_T0;
	ulLfsrCtrlT1 = BVDC_P_DITHER_VDEC_FE_LFSR_CTRL_T1;
	ulLfsrCtrlT2 = BVDC_P_DITHER_VDEC_FE_LFSR_CTRL_T2;
	BVDC_P_Dither_Init(&hVdec->stFEDither, ulLfsrCtrlT0,
		ulLfsrCtrlT1, ulLfsrCtrlT2, ulLfsrValue);

	ulLfsrValue  = BVDC_P_DITHER_VDEC_BE_LFSR_VALUE;
	ulLfsrCtrlT0 = BVDC_P_DITHER_VDEC_BE_LFSR_CTRL_T0;
	ulLfsrCtrlT1 = BVDC_P_DITHER_VDEC_BE_LFSR_CTRL_T1;
	ulLfsrCtrlT2 = BVDC_P_DITHER_VDEC_BE_LFSR_CTRL_T2;
	BVDC_P_Dither_Init(&hVdec->stBEDither, ulLfsrCtrlT0,
		ulLfsrCtrlT1, ulLfsrCtrlT2, ulLfsrValue);

	BDBG_LEAVE(BVDC_P_Vdec_Init);
	return;
}

/***************************************************************************
 * Return VDEC default settings.
 *
 */
void BVDC_P_Vdec_GetDefaultSettings
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_Vdec_Settings              *pVdecSetting )
{
	BDBG_ENTER(BVDC_P_Vdec_GetDefaultSettings);
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);

	/* Awc */
	if(pVdecSetting->pAwc)
	{
		BVDC_P_Vdec_InitAwc(pVdecSetting->pAwc, &hVdec->hSource->stCurInfo);
	}

	/* Cti */
	if(pVdecSetting->pCti)
	{
		BVDC_P_Vdec_InitCti(pVdecSetting->pCti, &hVdec->hSource->stCurInfo);
	}

	/* Dcr */
	if(pVdecSetting->pDcr)
	{
		BVDC_P_Vdec_InitDcr(pVdecSetting->pDcr,
			&hVdec->hSource->stCurInfo, hVdec->eMacrovisionType);
	}

	/* Comb */
	if(pVdecSetting->pComb)
	{
		BVDC_P_Vdec_InitComb(pVdecSetting->pComb);
	}

	/* Analog Gain */
	if(pVdecSetting->pAGain)
	{
		BVDC_P_Vdec_InitAnalogGain(pVdecSetting->pAGain);
	}

	/* Digital Gain */
	if(pVdecSetting->pDGain)
	{
		BVDC_P_Vdec_InitDigitalGain(pVdecSetting->pDGain,
			&hVdec->hSource->stCurInfo, hVdec->eMacrovisionType);
	}

	/* YCrCb delay */
	if(pVdecSetting->pDelay)
	{
		BVDC_P_Vdec_InitDelay(pVdecSetting->pDelay,
			&hVdec->hSource->stCurInfo);
	}

	/* Backend */
	if(pVdecSetting->pBackend)
	{
		BVDC_P_Vdec_InitBackend(pVdecSetting->pBackend,
			&hVdec->hSource->stCurInfo);
	}

	/* misc fields */
	BVDC_P_Vdec_InitMisc(pVdecSetting, &hVdec->hSource->stCurInfo);

	BDBG_LEAVE(BVDC_P_Vdec_GetDefaultSettings);

	return;
}


/***************************************************************************
 * Validates the combine of VDEC sources.  Second pass validate.
 * It will be call after all source validate.  It's then we can be sure
 * if ADC are be used more then once.  And can determine clk accordingly
 *
 * (1) Check for duplicate use of ADC
 * (2) Determine to use System Clock or PC Clock.
 * (3) Determine unused ADC.
 */
BERR_Code BVDC_P_Vdec_ValidateAdc
	( const BVDC_Source_Handle         ahSource[] )
{
	int i, j;
	bool bHasPcInput;
	uint32_t ulAdcTurnOn;
	const BVDC_P_AdcEntry *apUsedAdc[BVDC_P_MAX_PHYSICAL_ADC];

	BDBG_ENTER(BVDC_P_Vdec_ValidateAdc);

	/* Initialized */
	bHasPcInput = false;
	ulAdcTurnOn = 0;
	for(j = 0; j < BVDC_P_MAX_PHYSICAL_ADC; j++)
	{
		apUsedAdc[j] = NULL;
	}

	/* Second pass validation of ADC duplicate uses!  Don't need it for
	 * non vdec chip. */
	for(i = 0; i < BVDC_P_MAX_SOURCE_COUNT; i++)
	{
		if((BVDC_P_STATE_IS_ACTIVE(ahSource[i]) ||
		    BVDC_P_STATE_IS_CREATE(ahSource[i])) &&
		   (BVDC_P_SRC_IS_ANALOG(ahSource[i]->eId)))
		{
			const BVDC_P_AdcEntry *pAdc;
			const BVDC_P_Source_Info *pNewInfo;

			BDBG_OBJECT_ASSERT(ahSource[i], BVDC_SRC);
			pNewInfo = &ahSource[i]->stNewInfo;
			pAdc = NULL;

			/* Check if ADC is already used! */
			for(j = 0; j < BVDC_P_VDEC_CHANNELS; j++)
			{
				pAdc = pNewInfo->apAdc[j];
				if(pAdc)
				{
					if(apUsedAdc[pAdc->ulId])
					{
						BDBG_ERR(("Adc[%d] already used by Source[%d].",
							pAdc->ulId, ahSource[i]->eId));
						return BERR_TRACE(BERR_INVALID_PARAMETER);
					}
					else
					{
						BDBG_MSG(("Source[%d] uses Adc[%d].", ahSource[i]->eId, pAdc->ulId));
						apUsedAdc[pAdc->ulId] = pAdc;
					}

					/* (1) Turn on when at lease one vdec use the INPUT. */
					/* (2) Turn off when all vdecs don't use the INPUT */
					ulAdcTurnOn |= pAdc->ulInOffMask;
				}
			}

			/* Do we have PC input */
			if(pNewInfo->bPcInput)
			{
				bHasPcInput |= true;
			}
		}
	}

	/* Copy back to the Info */
	for(i = 0; i < BVDC_P_MAX_SOURCE_COUNT; i++)
	{
		if((BVDC_P_STATE_IS_ACTIVE(ahSource[i]) ||
		    BVDC_P_STATE_IS_CREATE(ahSource[i])) &&
		   (BVDC_P_SRC_IS_ANALOG(ahSource[i]->eId)))
		{
			BVDC_P_Source_Info *pNewInfo, *pCurInfo;

			BDBG_OBJECT_ASSERT(ahSource[i], BVDC_SRC);
			pNewInfo = &ahSource[i]->stNewInfo;
			pCurInfo = &ahSource[i]->stCurInfo;

			/* Copy In On/Off */
			pNewInfo->ulAdcTurnOn = ulAdcTurnOn;

			BDBG_MSG(("-----Adc Used-----"));

			for(j = 0; j < BVDC_P_MAX_PHYSICAL_ADC; j++)
			{
				pNewInfo->apUsedAdc[j] = apUsedAdc[j];
				BDBG_MSG(("Adc[%d] is %s", j, apUsedAdc[j] ? "Used" : "Unused"));

				if(pNewInfo->apUsedAdc[j] != pCurInfo->apUsedAdc[j])
				{
					pNewInfo->stDirty.stBits.bAdcChange = BVDC_P_DIRTY;
				}
			}

			/* (1) PC input exist needs to make sure if we have CVBS/SVIDEO it
			 *     needs to run at 108MHz.
			 * (2) PC input does not exist needs to make if we have CVBS/SVIDEO
			 *     it needs to run at 216Mhz. */
			if(bHasPcInput)
			{
				if((pNewInfo->bCvbsInput || pNewInfo->bSvidInput || pNewInfo->bIfdInput) &&
				   (pNewInfo->bUsePllClk))
				{
					BDBG_MSG(("Need to use 108Mhz for CVBS/SVIDEO."));
					pNewInfo->bUsePllClk = false;
					pNewInfo->stDirty.stBits.bAdcSetting = BVDC_P_DIRTY;
				}
			}
			else
			{
#if (BVDC_P_USE_PLL_AND_SYS_CLK)
				if((pNewInfo->bCvbsInput || pNewInfo->bSvidInput) &&
				   (!pNewInfo->bUsePllClk))
				{
					BDBG_MSG(("Need to use 216Mhz for CVBS/SVIDEO."));
					pNewInfo->bUsePllClk = true;
					pNewInfo->stDirty.stBits.bAdcSetting = BVDC_P_DIRTY;
				}
#endif
			}
		}
	}

	BDBG_LEAVE(BVDC_P_Vdec_ValidateAdc);
	return BERR_SUCCESS;
}


/***************************************************************************
 * Validate the settings in pNewInfo set by user!
 *
 */
BERR_Code BVDC_P_Vdec_ValidateChanges
	( BVDC_P_Vdec_Handle               hVdec )
{
	const BVDC_AdcInput *aeAdcInput;
	BVDC_P_Source_Info *pNewInfo;
	BVDC_P_Source_DirtyBits *pNewDirty;
	uint32_t ulAdcUsed, ulInputSel, i;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	BDBG_OBJECT_ASSERT(hVdec->hSource, BVDC_SRC);

	/* New Information from user, and derived information. */
	pNewInfo  = &hVdec->hSource->stNewInfo;
	pNewDirty = &pNewInfo->stDirty;

	/* Reset and derive these values. */
	ulAdcUsed  = 0;
	ulInputSel = 0;
	aeAdcInput = pNewInfo->aeAdcInput;

	/* one of the following only */
	pNewInfo->bPcInput   = false;
	pNewInfo->bCompInput = false;
	pNewInfo->bSvidInput = false;
	pNewInfo->bCvbsInput = false;
	pNewInfo->bIfdInput  = false;
	pNewInfo->bIntIfd    = false;
	pNewInfo->bScartInput = false;

	/* Can't be both type of colorspace */
	pNewInfo->bRgbInput   = false;
	pNewInfo->bYPrPbInput = false;

	/* Get configuration for PC Input if any. */
	if((hVdec->hSource->bInitial) ||
	   (pNewDirty->stBits.bInputFormat))
	{
		BVDC_P_Vdec_GetFmtCfg_isr(pNewInfo->pFmtInfo, pNewInfo->stVdecSettings.e3DGameMode,
			&pNewInfo->pCdCfg, &pNewInfo->pYcCfg, &pNewInfo->pPcInCfg);
	}

	/* Clear out the ADCs. */
	for(i = 0; i < BVDC_P_VDEC_CHANNELS; i++)
	{
		pNewInfo->apAdc[i] = NULL;
	}

	/* TODO: Optimize only on Adc Change */
	for(i = 0; i < BVDC_P_MAX_ADC; i++)
	{
		if(BVDC_AdcInput_eUnused != aeAdcInput[i])
		{
			ulAdcUsed++;
		}
	}

	/* Validate ADC */
	for(i = 0; i < BVDC_P_MAX_ADC; i++)
	{
		uint32_t ulChn = UINT32_C(-1);

		/* Which channel it below to? */
		if(BVDC_AdcInput_eIfDemod == aeAdcInput[i])
		{
			/* Check if this an internal IFD; it should have as */
			pNewInfo->bIntIfd    = (BVDC_Ibafe_0 == (1 << i)) ? true : false;
			pNewInfo->bIfdInput  = true;
			pNewInfo->eInputType = BVDC_P_VdecInput_eIfd;
			ulChn = 0;
		}
		else if(BVDC_AdcInput_eComposite == aeAdcInput[i])
		{
			pNewInfo->bCvbsInput = true;
			pNewInfo->eInputType = BVDC_P_VdecInput_eCvbs;
			ulChn = 0;
		}
		else if((BVDC_AdcInput_eScartComposite == aeAdcInput[i]) &&
		        (1 == ulAdcUsed))
		{
			pNewInfo->bCvbsInput = true;
			pNewInfo->eInputType = BVDC_P_VdecInput_eCvbs;
			ulChn = 0;
		}
		else if((BVDC_AdcInput_eLuma == aeAdcInput[i]) ||
		        (BVDC_AdcInput_eChroma == aeAdcInput[i]))
		{
			pNewInfo->bSvidInput = true;
			pNewInfo->eInputType = BVDC_P_VdecInput_eSvideo;
			ulChn = (BVDC_AdcInput_eLuma == aeAdcInput[i]) ? 0 : 1;
		}
		else if((BVDC_AdcInput_eScartLuma == aeAdcInput[i]) ||
		        (BVDC_AdcInput_eScartChroma == aeAdcInput[i]))
		{
			pNewInfo->bSvidInput = true;
			pNewInfo->eInputType = BVDC_P_VdecInput_eSvideo;
			ulChn = (BVDC_AdcInput_eScartLuma == aeAdcInput[i]) ? 0 : 1;
		}
		else if((BVDC_AdcInput_eY == aeAdcInput[i]) ||
		        (BVDC_AdcInput_ePr == aeAdcInput[i]) ||
		        (BVDC_AdcInput_ePb == aeAdcInput[i]))
		{
			pNewInfo->bCompInput  = true;
			pNewInfo->bYPrPbInput = true;
			pNewInfo->eInputType  = BVDC_P_VdecInput_eYPrPb;
			ulChn =
				 (BVDC_AdcInput_eY == aeAdcInput[i]) ? 0
				:(BVDC_AdcInput_ePr == aeAdcInput[i]) ? 1 : 2;
		}
		else if((BVDC_AdcInput_eRed == aeAdcInput[i]) ||
		        (BVDC_AdcInput_eBlue == aeAdcInput[i]) ||
		        (BVDC_AdcInput_eGreen == aeAdcInput[i]))
		{
			pNewInfo->bCompInput = true;
			pNewInfo->bRgbInput  = true;
			pNewInfo->eInputType = BVDC_P_VdecInput_eYPrPb;
			ulChn =
				 (BVDC_AdcInput_eGreen == aeAdcInput[i]) ? 0
				:(BVDC_AdcInput_eRed == aeAdcInput[i]) ? 1 : 2;
		}
		else if(((BVDC_AdcInput_eScartRed == aeAdcInput[i]) ||
		         (BVDC_AdcInput_eScartBlue == aeAdcInput[i]) ||
		         (BVDC_AdcInput_eScartGreen == aeAdcInput[i])) &&
		        (3 == ulAdcUsed))
		{
			pNewInfo->bCompInput = true;
			pNewInfo->bRgbInput  = true;
			pNewInfo->eInputType = BVDC_P_VdecInput_eYPrPb;
			ulChn =
				 (BVDC_AdcInput_eScartGreen == aeAdcInput[i]) ? 0
				:(BVDC_AdcInput_eScartRed == aeAdcInput[i]) ? 1 : 2;
		}
		else if((BVDC_AdcInput_ePcRed == aeAdcInput[i]) ||
		        (BVDC_AdcInput_ePcBlue == aeAdcInput[i]) ||
		        (BVDC_AdcInput_ePcGreen == aeAdcInput[i]))
		{
			pNewInfo->bPcInput   = true;
			pNewInfo->bRgbInput  = true;
			pNewInfo->eInputType = BVDC_P_VdecInput_ePcIn;
			ulChn =
				 (BVDC_AdcInput_ePcGreen == aeAdcInput[i]) ? 1
				:(BVDC_AdcInput_ePcRed == aeAdcInput[i]) ? 0 : 2;
		}
		else if(((BVDC_AdcInput_eScartRed == aeAdcInput[i]) ||
		         (BVDC_AdcInput_eScartBlue == aeAdcInput[i]) ||
		         (BVDC_AdcInput_eScartGreen == aeAdcInput[i]) ||
		         (BVDC_AdcInput_eScartComposite == aeAdcInput[i])) &&
		        (4 == ulAdcUsed))
		{
			pNewInfo->bScartInput = true;
			/* SW3556-1085: Added this code for "RF analog + RGB scart input" */
			if(BVDC_AdcInput_eScartComposite == aeAdcInput[i])
			{
				pNewInfo->bCvbsInput = true;
				pNewInfo->eInputType = BVDC_P_VdecInput_eCvbs;
			}
			ulChn  =
				 (BVDC_AdcInput_eScartRed == aeAdcInput[i]) ? 1
				:(BVDC_AdcInput_eScartBlue == aeAdcInput[i]) ? 2
				:(BVDC_AdcInput_eScartGreen == aeAdcInput[i]) ? 3 : 0;
		}
		else
		{
			/* Make sure we handle all adc type. */
			BDBG_ASSERT(BVDC_AdcInput_eUnused == aeAdcInput[i]);
		}

		/* Increament for each ADC used */
		if(BVDC_AdcInput_eUnused != aeAdcInput[i])
		{
			BDBG_ASSERT(ulChn < BVDC_P_VDEC_CHANNELS);
			/* Invalid if pointing to the same ADC or selection does not
			 * exist on particular VDEC. */
			if(pNewInfo->apAdc[ulChn])
			{
				BDBG_ERR(("Duplicated adc type."));
				return BERR_TRACE(BVDC_ERR_ADC_NOT_SUPPORTED);
			}

			/* Not support mode in hw. */
			pNewInfo->apAdc[ulChn] = (BAVC_SourceId_eVdec0 == hVdec->hSource->eId)
				? &s_aVdec0AdcSelTbl[i] : &s_aVdec1AdcSelTbl[i];

			/* If uses internal IFdemod!  No adc is needed */
			if(BVDC_Ibafe_0 == pNewInfo->apAdc[ulChn]->ulAdc)
			{
				pNewInfo->apAdc[ulChn] = NULL;
			}
			else if(BVDC_P_ADC_INVALID == pNewInfo->apAdc[ulChn]->ulOffset)
			{
				BDBG_ERR(("Invalid adc type apAdc[%d]->ulOffset = %d.", ulChn,
					pNewInfo->apAdc[ulChn]->ulOffset));
				return BERR_TRACE(BVDC_ERR_ADC_NOT_SUPPORTED);
			}

			BDBG_MSG(("ulChn = %d, aeAdcInput[%d] = %d", ulChn, i, aeAdcInput[i]));
		}
	}

	/* Keep track of how input type user selected. */
	ulInputSel += (pNewInfo->bPcInput)    ? 1 : 0;
	ulInputSel += (pNewInfo->bIfdInput)   ? 1 : 0;
	ulInputSel += (pNewInfo->bSvidInput)  ? 1 : 0;
	ulInputSel += (pNewInfo->bCvbsInput)  ? 1 : 0;
	ulInputSel += (pNewInfo->bCompInput)  ? 1 : 0;

	/* (0) Error if user choses more than one input type (CVBS+SVIDEO/etc) */
	if(ulInputSel > 1)
	{
		BDBG_ERR(("Mixed of CVBS/SVIDEO/IFD/COMP/PC adc type."));
		return BERR_TRACE(BVDC_ERR_ADC_NOT_SUPPORTED);
	}

	/* (1) Error if user choses more than one colorspace type (RGB/YPrPb) */
	if(pNewInfo->bYPrPbInput && pNewInfo->bRgbInput)
	{
		BDBG_ERR(("Mixed of RGB/YPrPb color space for component."));
		return BERR_TRACE(BVDC_ERR_INVALID_ADC_FOR_COMPONENT);
	}

	/* (2) Error if user choses more ulAdcUsed for a particular input. */
	if((pNewInfo->bScartInput) &&
	   (ulAdcUsed != 4))
	{
		BDBG_ERR(("SCART-1 needs exactly four (4 instead of %d) ADCs.", ulAdcUsed));
		return BERR_TRACE(BVDC_ERR_INVALID_ADC_FOR_SCART);
	}

	if(((pNewInfo->bPcInput) || (pNewInfo->bCompInput)) &&
	   (ulAdcUsed != 3))
	{
		BDBG_ERR(("PC/RGB/YPrPb need exactly three (3 instead of %d) ADCs.", ulAdcUsed));
		return BERR_TRACE(BVDC_ERR_INVALID_ADC_FOR_COMPONENT);
	}

	if((pNewInfo->bSvidInput) &&
	   (ulAdcUsed != 2))
	{
		BDBG_ERR(("SVIDEO needs exactly two (2 instead of %d) ADCs.", ulAdcUsed));
		return BERR_TRACE(BVDC_ERR_INVALID_ADC_FOR_SVIDEO);
	}

	if((pNewInfo->bCvbsInput && (!pNewInfo->bScartInput)) &&
	   (ulAdcUsed != 1))
	{
		BDBG_ERR(("CVBS needs exactly one (1 instead of %d) ADC.", ulAdcUsed));
		return BERR_TRACE(BVDC_ERR_INVALID_ADC_FOR_COMPOSITE);
	}

	if((pNewInfo->bIfdInput) &&
	   (ulAdcUsed != 1 && ((!pNewInfo->bScartInput) || (ulAdcUsed != 4))))
	{
		BDBG_ERR(("IF Demod needs exactly one (1 instead of %d) ADC.", ulAdcUsed));
		return BERR_TRACE(BVDC_ERR_INVALID_ADC_FOR_IFDEMOD);
	}

	/* (3) Mismatch format and ADC used? */
	if(((pNewInfo->bIfdInput) ||
	    (pNewInfo->bCvbsInput) ||
	    (pNewInfo->bSvidInput)) &&
	   (!pNewInfo->pVdcFmt->bSd && !pNewInfo->bAutoFmtDetection))
	{
		BDBG_ERR(("IFD/CVBS/SVIDEO is restricted to SD formats"));
		return BERR_TRACE(BVDC_ERR_ADC_NOT_SUPPORTED);
	}

	/* If it's a PC input!  We will need additional ADC to do calibration!
	 * In this case other VDEC is only left with 2 ADCs.  Need to use
	 * resource manager to manage adc uses between the vdec'es. */
	if(pNewInfo->bPcInput)
	{
		pNewInfo->apAdc[3] = &hVdec->stCalibAdc;
	}

	/* PR53137: ADC conflict when doing PIP/PBP between PC/Component and SIF(audio).
	 * If at lease one ADC used also assume that SIF is used
	 * 5th Adc is for SIF */
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
	/* if(ulAdcUsed), SIF is always enabled and can be require with out vdec */
	{
		pNewInfo->apAdc[4] = &hVdec->stSifAdc;
	}
#else
	if((pNewInfo->bIfdInput) && (!pNewInfo->bIntIfd))
	{
		pNewInfo->apAdc[1] = &hVdec->stSifAdc;
	}
#endif

	/* Use system clock or PLL for VDEC. */
#if (BVDC_P_USE_PLL_AND_SYS_CLK)
	pNewInfo->bUsePllClk = true;
#else
	pNewInfo->bUsePllClk = false;
#endif

	/* (*) Do we need to do auto format detection?  (Lagacy). */
	pNewInfo->bAutoFmtDetection = pNewInfo->bAutoDetect;

	/* Fdt count/table changes */
	if(!pNewInfo->bPcInput)
	{
		pNewInfo->aFdtFmtTbl = s_aAtscFdtFmtTbl;
		pNewInfo->ulFdtCount = BVDC_P_ATSC_FDT_COUNT;
		pNewInfo->ulFdtMemOffset = hVdec->ulAtscFdtMemOffset;
	}
	else
	{
		pNewInfo->aFdtFmtTbl = s_aVesaFdtFmtTbl;
		pNewInfo->ulFdtCount = BVDC_P_VESA_FDT_COUNT;
		pNewInfo->ulFdtMemOffset = hVdec->ulVesaFdtMemOffset;
	}

	/* Validate input format! */
	if((!pNewInfo->bAutoFmtDetection) || (hVdec->hSource->bInitial))
	{
		for(i = 0; i < pNewInfo->ulFdtCount; i++)
		{
			if(pNewInfo->aFdtFmtTbl[i].eFmt == pNewInfo->pFmtInfo->eVideoFmt)
			{
				pNewInfo->pFdEntry = &pNewInfo->aFdtFmtTbl[i];
				break;
			}
		}

		/* No matching format */
		if(pNewInfo->ulFdtCount == i)
		{
			BDBG_ERR(("Format not yet supported by VDEC: %s",
				pNewInfo->pFmtInfo->pchFormatStr));
			return BERR_TRACE(BVDC_ERR_VIDEO_FORMAT_NOT_SUPPORTED);
		}
	}
	else
	{
		/* If auto-format detection is enabled!  Needs to ignore the format
		 * from user set! */
		if(pNewDirty->stBits.bInputFormat)
		{
			pNewInfo->pFmtInfo = hVdec->hSource->stCurInfo.pFmtInfo;
			pNewInfo->pVdcFmt  = hVdec->hSource->stCurInfo.pVdcFmt;
			pNewDirty->stBits.bInputFormat = BVDC_P_CLEAN;
		}
	}

#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_6)
	pNewInfo->bFullRange = (
		(pNewInfo->bCompInput) &&
		(pNewInfo->stVdecSettings.bFullRange));
#else
	pNewInfo->bFullRange = false;
#endif

	/* Reset back to default if the no longer want customization.  */
	if(BVDC_P_IS_DIRTY(pNewDirty))
	{
		const BVDC_Vdec_Settings *pVdecSettings = &pNewInfo->stVdecSettings;

		if(pNewDirty->stBits.bMiscCtrl)
		{
			if(!pVdecSettings->pComb)
			{
				BVDC_P_Vdec_InitComb(&pNewInfo->stComb);
			}
			if(!pVdecSettings->pAGain)
			{
				BVDC_P_Vdec_InitAnalogGain(&pNewInfo->stAGain);
			}
		}

		if((pNewDirty->stBits.bMiscCtrl) ||
		   (pNewDirty->stBits.bAdcSetting) ||
		   (pNewDirty->stBits.bInputFormat))
		{
			if(!pVdecSettings->pAwc)
			{
				BVDC_P_Vdec_InitAwc(&pNewInfo->stAwc, pNewInfo);
			}
			if(!pVdecSettings->pDelay)
			{
				BVDC_P_Vdec_InitDelay(&pNewInfo->stDelay, pNewInfo);
			}
			if(!pVdecSettings->pCti)
			{
				BVDC_P_Vdec_InitCti(&pNewInfo->stCti, pNewInfo);
			}
			if(!pVdecSettings->pDcr)
			{
				BVDC_P_Vdec_InitDcr(&pNewInfo->stDcr, pNewInfo, hVdec->eMacrovisionType);
			}
			if(!pVdecSettings->pBackend)
			{
				BVDC_P_Vdec_InitBackend(&pNewInfo->stBackend, pNewInfo);
			}
			if(!pVdecSettings->pDGain)
			{
				BVDC_P_Vdec_InitDigitalGain(&pNewInfo->stDGain, pNewInfo, hVdec->eMacrovisionType);
			}
		}
	}

	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
static void BVDC_P_Vdec_SetDynamicsPowerMode_isr
	( BREG_Handle                      hReg,
	  bool                             bPowerModeOn,
	  bool                             abAdcEachOn[BVDC_P_MAX_PHYSICAL_ADC],
	  uint32_t                         ulAdcTurnOn )
{
	bool bPowerOn;
	uint32_t ulOffset, ulReg, i;

	/* PR31679, PR46514: Shut off unused adc for power saving as well. */
	for(i = 0; i < BVDC_P_MAX_PHYSICAL_ADC; i++)
	{
		ulOffset = i * (BCHP_ADC_2_REG_START - BCHP_ADC_1_REG_START);

		bPowerOn = (
			bPowerModeOn ||
			abAdcEachOn[i]);

		/* (1a) ADCs */
		ulReg  = BREG_Read32_isr(hReg, BCHP_ADC_1_ENABLES + ulOffset);
		ulReg &= ~(
			BCHP_MASK(ADC_1_ENABLES, POWER_DOWN));
		ulReg |=  (
			BCHP_FIELD_DATA(ADC_1_ENABLES, POWER_DOWN, !bPowerOn));
		BREG_Write32_isr(hReg, BCHP_ADC_1_ENABLES + ulOffset, ulReg);
	}

	/* PR31679, PR46514, PR49556: If all power off then shutoff adcs, and PLLs */
	bPowerOn = (
		bPowerModeOn ||
		ulAdcTurnOn);

		/* (1b) ADCs */
	ulReg  = BREG_Read32_isr(hReg, BCHP_ADC_1_CONTROL);
	ulReg &= ~(
		BCHP_MASK(ADC_1_CONTROL, PD_D2S));
	ulReg |=  (
		BCHP_FIELD_DATA(ADC_1_CONTROL, PD_D2S, !bPowerOn));
	BREG_Write32_isr(hReg, BCHP_ADC_1_CONTROL, ulReg);

		/* (2a) PLL */
	ulReg  = BREG_Read32_isr(hReg, BCHP_PLL_ENABLE);
		ulReg &= ~(
			BCHP_MASK(PLL_ENABLE, PLL_POWER_DOWN    ) |
			BCHP_MASK(PLL_ENABLE, REFCOMP_POWER_DOWN));
		ulReg |=  (
		BCHP_FIELD_DATA(PLL_ENABLE, PLL_POWER_DOWN,     !bPowerOn) |
		BCHP_FIELD_DATA(PLL_ENABLE, REFCOMP_POWER_DOWN, !bPowerOn));
	BREG_Write32_isr(hReg, BCHP_PLL_ENABLE, ulReg);

		/* (2b) PLL */
	ulReg  = BREG_Read32_isr(hReg, BCHP_PLL_PLL_CTL_1);
		ulReg &= ~(
			BCHP_MASK(PLL_PLL_CTL_1, I_PWRDN_CH1 ) |
			BCHP_MASK(PLL_PLL_CTL_1, I_NDIV_PWRDN) |
			BCHP_MASK(PLL_PLL_CTL_1, I_EN_CMLBUF1));
		ulReg |=  (
		BCHP_FIELD_DATA(PLL_PLL_CTL_1, I_PWRDN_CH1,  !bPowerOn) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_1, I_NDIV_PWRDN, !bPowerOn) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_1, I_EN_CMLBUF1,  bPowerOn));
	BREG_Write32_isr(hReg, BCHP_PLL_PLL_CTL_1, ulReg);

	return;
}


/***************************************************************************
 *
 */
void BVDC_P_Vdec_SetPowerMode_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  bool                             bPowerModeOn )
{
	uint32_t i;
	const BVDC_P_Source_Info *pCurInfo;
	bool abAdcEachOn[BVDC_P_MAX_PHYSICAL_ADC];

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	pCurInfo = &hVdec->hSource->stCurInfo;

	for(i = 0; i < BVDC_P_MAX_PHYSICAL_ADC; i++)
	{
#if (BVDC_P_SUPPORT_VDEC > 1)
		abAdcEachOn[i] = (pCurInfo->apUsedAdc[i]) ? true : false;
#else
		abAdcEachOn[i] = (false);
#endif
	}

	BVDC_P_Vdec_SetDynamicsPowerMode_isr(hVdec->hReg, bPowerModeOn, abAdcEachOn,
		pCurInfo->ulAdcTurnOn);

	return;
}


/***************************************************************************
 * Reset top level in VDEC and individual cores!
 *
 */
void BVDC_P_Vdec_Reset
	( BREG_Handle                      hReg )
{
	uint32_t i;
	bool abAdcEachOn[BVDC_P_MAX_PHYSICAL_ADC];

	/* SUNDRY's VDEC/PC Reset: write 1 and then write 0.   SUNDRY Top is
	 * a share register need to have exclusive access! */
	BREG_AtomicUpdate32_isr(hReg, BCHP_SUN_TOP_CTRL_SW_RESET, 0,
		BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_RESET, vdec_sw_reset,   1 )|
		BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_RESET, pc_vbi_sw_reset, 1 ));

	BREG_AtomicUpdate32_isr(hReg, BCHP_SUN_TOP_CTRL_SW_RESET,
		BCHP_MASK( SUN_TOP_CTRL_SW_RESET, vdec_sw_reset  ) |
		BCHP_MASK( SUN_TOP_CTRL_SW_RESET, pc_vbi_sw_reset), 0);

	/* Start with power off */
	for(i = 0; i < BVDC_P_MAX_PHYSICAL_ADC; i++)
	{
		abAdcEachOn[i] = false;
	}
	BVDC_P_Vdec_SetDynamicsPowerMode_isr(hReg, false, abAdcEachOn, 0);

	return;
}


/***************************************************************************
 *
 */
void BVDC_P_Vdec_GetPcInSyncStatus
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_Source_SyncStatus          *pSyncStatus )
{
	uint32_t i, ulReg;
	uint32_t ulPcInDpllStatusReg;
	uint32_t ulPcInPhaseStatusReg;
	uint32_t ulPcInRaActiveStatusReg;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	/* read registers */
	ulPcInDpllStatusReg  =
		BREG_Read32(hVdec->hReg, BCHP_PC_IN_0_SYNC_DPLL_STATUS);
	ulPcInPhaseStatusReg =
		BREG_Read32(hVdec->hReg, BCHP_PC_IN_0_PHASE_STATUS);

	/* PC_IN_0_DPLL STATUS */
	pSyncStatus->bClockLocked = BCHP_GET_FIELD_DATA(
		ulPcInDpllStatusReg, PC_IN_0_SYNC_DPLL_STATUS, DPLL_LOCKED);

	/* PC_IN_0_PHASE STATUS */
	pSyncStatus->abPhaseLocked[0] = BCHP_GET_FIELD_DATA(
		ulPcInPhaseStatusReg, PC_IN_0_PHASE_STATUS, CH0_LOCK);
	pSyncStatus->abPhaseLocked[1] = BCHP_GET_FIELD_DATA(
		ulPcInPhaseStatusReg, PC_IN_0_PHASE_STATUS, CH1_LOCK);
	pSyncStatus->abPhaseLocked[2] = BCHP_GET_FIELD_DATA(
		ulPcInPhaseStatusReg, PC_IN_0_PHASE_STATUS, CH2_LOCK);

	pSyncStatus->aulPhase[0] = BCHP_GET_FIELD_DATA(
		ulPcInPhaseStatusReg, PC_IN_0_PHASE_STATUS, CH0_PHASE);
	pSyncStatus->aulPhase[1] = BCHP_GET_FIELD_DATA(
		ulPcInPhaseStatusReg, PC_IN_0_PHASE_STATUS, CH1_PHASE);
	pSyncStatus->aulPhase[2] = BCHP_GET_FIELD_DATA(
		ulPcInPhaseStatusReg, PC_IN_0_PHASE_STATUS, CH2_PHASE);

#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
	ulPcInRaActiveStatusReg =
		BREG_Read32(hVdec->hReg, BCHP_PC_IN_0_RA_MEASURED_ACTIVE);

	pSyncStatus->bPositionLocked = hVdec->bRaLocked;
	pSyncStatus->ulHStart= BCHP_GET_FIELD_DATA(
		ulPcInRaActiveStatusReg, PC_IN_0_RA_MEASURED_ACTIVE, H_START);
	pSyncStatus->ulVStart= BCHP_GET_FIELD_DATA(
		ulPcInRaActiveStatusReg, PC_IN_0_RA_MEASURED_ACTIVE, V_START);
	if(pSyncStatus->ulHStart >= BVDC_P_PC_RA_HSTART_OFFSET)
	{
		pSyncStatus->ulHStart -= BVDC_P_PC_RA_HSTART_OFFSET;
	}

	/* Phase lock bin stats */
	for(i = 0; i < BVDC_PC_PHASE_LOCK_BIN; i++)
	{
		ulReg = BREG_Read32(hVdec->hReg, BCHP_PC_IN_0_PHASE_LOCK_BIN_M4_STATUS + sizeof(uint32_t) * i);
		pSyncStatus->aulPhaseLockBin[i] =
			BCHP_GET_FIELD_DATA(ulReg, PC_IN_0_PHASE_LOCK_BIN_M4_STATUS, VALUE);
	}

	/* Current calibration channel. */
	ulReg = BREG_Read32_isr(hVdec->hReg, BCHP_ADC_1_STATUS);
	pSyncStatus->eCalibrationChannel = (BVDC_Vdec_CalibrationChannel)
		BCHP_GET_FIELD_DATA(ulReg, ADC_1_STATUS, CALIBRATION_CHANNEL);

#else
	BSTD_UNUSED(ulPcInRaActiveStatusReg);
#endif

	return;
}


/***************************************************************************
 *
 */
void BVDC_P_Vdec_GetInputStatus
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_Source_InputStatus         *pInputStatus )
{
	const BFMT_VideoInfo *pFmtInfo;
	uint32_t ulPxlFreq, ulSystemClock;
	uint32_t ulLineLength, ulLpf;
	uint32_t ulScanWidth, ulScanHeight;
	uint32_t ulMfLpf;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	BDBG_OBJECT_ASSERT(hVdec->hSource, BVDC_SRC);
	pFmtInfo = hVdec->hSource->stCurInfo.pFmtInfo;

	/* Get FD status */
	pInputStatus->ulVertFreq  = hVdec->hSource->ulVertFreq;
	/* HPLL will not be locked if there is no vsync signal */
	pInputStatus->bVsyncDetected =
		(hVdec->bHPllLocked) &&
		(BCHP_GET_FIELD_DATA(hVdec->ulFeStatus0Reg, VDEC_FE_0_FE_STATUS0, VPLL_LOCKED));

	/* Get input status */
	pInputStatus->bInterlaced =
		BCHP_GET_FIELD_DATA(hVdec->ulFdLpfMetric0Reg, FD_0_LPF_METRIC_STATUS_0, RASTER_TYPE);
	pInputStatus->ulHPolarity =
		BCHP_GET_FIELD_DATA(hVdec->ulFdLpfMetric1Reg, FD_0_LPF_METRIC_STATUS_1, HSYNC_POLARITY);
	pInputStatus->ulVPolarity =
		BCHP_GET_FIELD_DATA(hVdec->ulFdLpfMetric1Reg, FD_0_LPF_METRIC_STATUS_1, VSYNC_POLARITY);

	/* HW status registers only give rasterization width and height.
	 * Use the active width and height from pFmtInfo */
	pInputStatus->ulAvWidth  = pFmtInfo->ulWidth;
	pInputStatus->ulAvHeight = pFmtInfo->ulHeight;

	/* LINE_LENGTH is in system clock sample.
	 *  ulAvWidth = (ulLineLength * (ulPxlFreq / BFMT_FREQ_FACTOR)) /
	 *     (BVDC_P_108_SYSCLK / 1000000) */
	ulLineLength   = BCHP_GET_FIELD_DATA(hVdec->ulFdLpfMetric0Reg, FD_0_LPF_METRIC_STATUS_0, LINE_LENGTH);
	ulPxlFreq      = pFmtInfo->ulPxlFreq / BFMT_FREQ_FACTOR;
	ulSystemClock  = (hVdec->hSource->stCurInfo.bPcInput ? BVDC_P_216_SYSCLK : BVDC_P_108_SYSCLK) / 1000000;
	ulScanWidth    = (ulLineLength * pFmtInfo->ulPxlFreq) / (ulSystemClock * BFMT_FREQ_FACTOR);

	/* LINES_PER_FIELD is 11.4 */
	ulLpf =
		BCHP_GET_FIELD_DATA(hVdec->ulFdLpfMetric1Reg, FD_0_LPF_METRIC_STATUS_1, LINES_PER_FIELD) / 16;
	ulScanHeight = ulLpf << pInputStatus->bInterlaced;

	pInputStatus->ulHBlank = ulScanWidth - pInputStatus->ulAvWidth;
	pInputStatus->ulVBlank = ulScanHeight - pInputStatus->ulAvHeight;

	/* Line per field" */
	ulMfLpf =
		BCHP_GET_FIELD_DATA(hVdec->ulFdMfMetric1Reg, FD_0_MF_METRIC_STATUS_1, LINES_PER_FIELD) / 16;

	/* Init, "Valid Format" */
	pInputStatus->bNoSignal = false;
	pInputStatus->pFmtInfo  = pFmtInfo;

	/* PC Input */
	if(hVdec->hSource->stCurInfo.bPcInput)
	{
		/* No input */
		if((!BCHP_GET_FIELD_DATA(hVdec->ulFdLpfMetric0Reg, FD_0_LPF_METRIC_STATUS_0, LINE_LENGTH_LOCK)) ||
		   (!BCHP_GET_FIELD_DATA(hVdec->ulFdLpfMetric0Reg, FD_0_LPF_METRIC_STATUS_0, HSYNC_WIDTH_LOCK)))
		{
			pInputStatus->pFmtInfo  = NULL;
			pInputStatus->bNoSignal = true;
		}
		/* Invalid format! */
		else if(!BCHP_GET_FIELD_DATA(hVdec->ulFdFormatReg, FD_0_ARBITER_FINAL_FORMAT_STATUS, FORMAT_INDEX))
		{
			pInputStatus->pFmtInfo = NULL;
		}
	}
	else if(hVdec->hSource->stCurInfo.bCompInput)
	{
		/* No input */
		if((!BCHP_GET_FIELD_DATA(hVdec->ulFdLpfMetric0Reg, FD_0_LPF_METRIC_STATUS_0, LINE_LENGTH_LOCK)) &&
		   (!BCHP_GET_FIELD_DATA(hVdec->ulFdMfMetric0Reg, FD_0_MF_METRIC_STATUS_0, LINE_LENGTH_LOCK)))
		{
			pInputStatus->pFmtInfo  = NULL;
			pInputStatus->bNoSignal = true;
		}
		/* Invalid format! */
		else if(!BCHP_GET_FIELD_DATA(hVdec->ulFdFormatReg, FD_0_ARBITER_FINAL_FORMAT_STATUS, FORMAT_INDEX))
		{
			pInputStatus->pFmtInfo = NULL;
		}
	}
	else /*NTSC/CVBS/Svideo*/
	{
		pInputStatus->bNoSignal = !(
			(hVdec->bHPllLocked) ||
			((hVdec->hSource->stCurInfo.bIfdInput) &&
			 (BVDC_P_EQ_DELTA(
			 	BCHP_GET_FIELD_DATA(hVdec->ulFdMfMetric0Reg, FD_0_MF_METRIC_STATUS_0, LINE_LENGTH),
			 	hVdec->hSource->stCurInfo.pFdEntry->ulLineLength,
			 	BVDC_P_VDEC_FD_LINE_LENGTH_THRESHOLD))));

		pInputStatus->pFmtInfo  = pFmtInfo;
	}

	/* Also return the original format */
	pInputStatus->pOriFmtInfo = pInputStatus->pFmtInfo;

	if((BCHP_GET_FIELD_DATA(hVdec->ulFdFormatReg, FD_0_ARBITER_FINAL_FORMAT_STATUS, FORMAT_INDEX) &&
	   (hVdec->hSource->stCurInfo.bPcInput)))
	{
		uint32_t ulFmtId = BCHP_GET_FIELD_DATA(hVdec->ulFdFormatReg,
			FD_0_ARBITER_FINAL_FORMAT_STATUS, FORMAT_INDEX);
		pInputStatus->pOriFmtInfo = BFMT_GetVideoFormatInfoPtr(
			hVdec->hSource->stCurInfo.aFdtFmtTbl[ulFmtId-1].eFmt);
	}

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_Vdec_GetFmtCfg_isr
	( const BFMT_VideoInfo            *pFmtInfo,
	  const BVDC_Vdec_3DGameMode       e3DCombGameMode,
	  const BVDC_P_CdCfgEntry        **ppCdCfg,
	  const BVDC_P_YcCfgEntry        **ppYcCfg,
	  const BVDC_P_PcInCfgEntry      **ppPcInCfg )

{
	uint32_t i;

	/* Default so we know won't basic one. */
	*ppCdCfg   = &s_aCdCfgTbl[0];
	*ppYcCfg   = &s_aYcCfgTbl[0];
	*ppPcInCfg = &s_aPcInCfgTbl[0];

	for(i = 0; i < BVDC_P_CD_COUNT; i++)
	{
		if(s_aCdCfgTbl[i].eFmt == pFmtInfo->eVideoFmt)
		{
			*ppCdCfg = &s_aCdCfgTbl[i];
			break;
		}
	}

	for(i = 0; i < BVDC_P_YC_COUNT; i++)
	{
		if((s_aYcCfgTbl[i].eFmt == pFmtInfo->eVideoFmt) &&
		   (s_aYcCfgTbl[i].e3DGameMode == e3DCombGameMode))
		{
			*ppYcCfg = &s_aYcCfgTbl[i];
			break;
		}
	}

	for(i = 0; i < BVDC_P_VGA_COUNT; i++)
	{
		if(s_aPcInCfgTbl[i].eFmt == pFmtInfo->eVideoFmt)
		{
			*ppPcInCfg = &s_aPcInCfgTbl[i];
			break;
		}
	}

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_Vdec_ResetBeRul_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	BSTD_UNUSED(pCurInfo);
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);

	/* BCHP_VDEC_BE_0_BE_RESET (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_BE_RESET + hVdec->ulBeOffset);
	*pList->pulCurrent++ = 1;

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_Vdec_ResetRul_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	if(pCurInfo->bUsePllClk)
	{
		/* BCHP_VD_TOP_0_PC_IN_RESET (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VD_TOP_0_PC_IN_RESET);
		*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
			BCHP_FIELD_DATA(VD_TOP_0_PC_IN_RESET, PC_AUTO_RASTER, 1 ) | /* nominal */
#endif
			BCHP_FIELD_DATA(VD_TOP_0_PC_IN_RESET, PLL,            0 ) | /* nominal */
			BCHP_FIELD_DATA(VD_TOP_0_PC_IN_RESET, PC_INPUT,       1 );  /* nominal */

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VD_TOP_0_PC_IN_RESET);
		*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
			BCHP_FIELD_DATA(VD_TOP_0_PC_IN_RESET, PC_AUTO_RASTER, 0 ) | /* nominal */
#endif
			BCHP_FIELD_DATA(VD_TOP_0_PC_IN_RESET, PLL,            0 ) | /* nominal */
			BCHP_FIELD_DATA(VD_TOP_0_PC_IN_RESET, PC_INPUT,       0 );  /* nominal */
	}

	if((pCurInfo->stDirty.stBits.bInputFormat) ||
	   (pCurInfo->stDirty.stBits.bAdcSetting))
	{
		/* VD_TOP_0_VDEC0_RESET (RW): 1 -> 0 */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VD_TOP_0_VDEC0_RESET + hVdec->ulRstOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VD_TOP_0_VDEC0_RESET, FD, 1 );

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VD_TOP_0_VDEC0_RESET + hVdec->ulRstOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VD_TOP_0_VDEC0_RESET, FD, 0 );

		BVDC_P_Vdec_ResetBeRul_isr(hVdec, pList, pCurInfo);
	}

	return;
}

/***************************************************************************
 *
 */
static void BVDC_P_Vdec_SelectSampleRate_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	/* Base on format */
	if(pCurInfo->pVdcFmt->bSd)
	{
		hVdec->ulAaFilter   = BCHP_VD_TOP_0_VD_VDEC_0_CTL_ATSC_AA_FILTER_SR_27;
		hVdec->ulSampleRate = BCHP_VD_TOP_0_VD_VDEC_0_CTL_ATSC_SAMPLE_RATE_SR_27;
	}
	else if(pCurInfo->pVdcFmt->bEd)
	{
		hVdec->ulAaFilter   = BCHP_VD_TOP_0_VD_VDEC_0_CTL_ATSC_AA_FILTER_SR_54;
		hVdec->ulSampleRate = BCHP_VD_TOP_0_VD_VDEC_0_CTL_ATSC_SAMPLE_RATE_SR_54;
	}
	else /* (pVdcFmt->bHd) */
	{
		if((pCurInfo->pVdcFmt->eVideoFmt == BFMT_VideoFmt_e1080p) ||
		   (pCurInfo->pVdcFmt->eVideoFmt == BFMT_VideoFmt_e1080p_50Hz))
		{
			hVdec->ulAaFilter   = BCHP_VD_TOP_0_VD_VDEC_0_CTL_ATSC_AA_FILTER_SR_108B;
			hVdec->ulSampleRate = BCHP_VD_TOP_0_VD_VDEC_0_CTL_ATSC_SAMPLE_RATE_SR_216;
		}
		else
		{
			hVdec->ulAaFilter   = BCHP_VD_TOP_0_VD_VDEC_0_CTL_ATSC_AA_FILTER_SR_108A;
			hVdec->ulSampleRate = BCHP_VD_TOP_0_VD_VDEC_0_CTL_ATSC_SAMPLE_RATE_SR_108;
		}
	}

	/* To support 216M in FE, except PC and IFD*/
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
	if(pCurInfo->bCvbsInput || pCurInfo->bSvidInput || pCurInfo->bCompInput)
	{
		hVdec->bFullSampleRate = true;
	}
	else
	{
		hVdec->bFullSampleRate = false;
	}
#endif

	return;
}

/***************************************************************************
 *
 * Channel selection: VD_TOP_0_VD_VDEC_0_CTL.CHx_SELECT
 *
 *                  CH0         CH1        CH2        CH3
 *   CVBS           cvbs        n/a        n/a        n/a
 *   SVIDEO         luma        chroma     n/a        n/a
 *   COMPONENT      Y           Pr         Pb         n/a
 *   PC             G           R          B          n/a
 *   SCART-1        cvbs        R          G          B
 *   SCART-2        luma        chroma     n/a        n/a
 *   SCART-3        cvbs        n/a        n/a        n/a
 *   HD-SCART       G           R          B          n/a
 */
static void BVDC_P_Vdec_BuildTopCtrlRul_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	/* VD_TOP_0_PC_IN_RESET (RW) */
	/* VD_TOP_0_VDEC0_RESET (RW) */
	uint32_t ulTpOrMask;
	uint32_t ulTpAndMask;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	BDBG_OBJECT_ASSERT(hVdec->hSource, BVDC_SRC);

	/* Build up AND/OR mask for RUL read/modify/write */
	ulTpAndMask =
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_MASK(VD_TOP_0_VD_TEST_PORT, ADC_TP_OUT_10_OR_11BITS) | /* nominal */
		BCHP_MASK(VD_TOP_0_VD_TEST_PORT, ADC_TP_OUT_DATA_HVSYNC ) | /* nominal */
		BCHP_MASK(VD_TOP_0_VD_TEST_PORT, ADC_TP_OUT_DATA_HIGH   ) | /* nominal */
		BCHP_MASK(VD_TOP_0_VD_TEST_PORT, ADC_TP_OUT_DATA_MID    ) | /* nominal */
		BCHP_MASK(VD_TOP_0_VD_TEST_PORT, ADC_TP_OUT_DATA_LOW    ) | /* nominal */
		BCHP_MASK(VD_TOP_0_VD_TEST_PORT, PCIN_TP_IN_SEL         ) | /* nominal */
		BCHP_MASK(VD_TOP_0_VD_TEST_PORT, ADC_MSB_INVERT         ) | /* nominal */
#elif (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_MASK(VD_TOP_0_VD_TEST_PORT, FD_MF_IN_SELECT  ) | /* nominal */
		BCHP_MASK(VD_TOP_0_VD_TEST_PORT, FD_LPF_IN_SELECT ) | /* nominal */
		BCHP_MASK(VD_TOP_0_VD_TEST_PORT, PCIN_TP_IN_SEL   ) | /* nominal */
		BCHP_MASK(VD_TOP_0_VD_TEST_PORT, ADC_MSB_INVERT   ) | /* nominal */
#elif (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_3)
		BCHP_MASK(VD_TOP_0_VD_TEST_PORT, IFD_CLK_SEL      ) | /* nominal */
		BCHP_MASK(VD_TOP_0_VD_TEST_PORT, PCIN_TP_IN_SEL   ) | /* nominal */
		BCHP_MASK(VD_TOP_0_VD_TEST_PORT, ADC_MSB_INVERT   ) | /* nominal */
#elif (BVDC_P_SUPPORT_VDEC_VER == BVDC_P_VDEC_VER_2)
		BCHP_MASK(VD_TOP_0_VD_TEST_PORT, ADC_MSB_LSB      ) | /* nominal */
		BCHP_MASK(VD_TOP_0_VD_TEST_PORT, ADC_MSB_INVERT   ) | /* nominal */
#endif
		BCHP_MASK(VD_TOP_0_VD_TEST_PORT, IFD_1_SELECT     ) | /* nominal */
		BCHP_MASK(VD_TOP_0_VD_TEST_PORT, IFD_0_SELECT     ) | /* nominal */
		BCHP_MASK(VD_TOP_0_VD_TEST_PORT, DEBUG_SELECT     ) | /* nominal */
		BCHP_MASK(VD_TOP_0_VD_TEST_PORT, ADC_BYPASS       ) | /* nominal */
		BCHP_MASK(VD_TOP_0_VD_TEST_PORT, ADC_TP_OUT_SELECT); /* nominal */

	ulTpOrMask =
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, ADC_TP_OUT_10_OR_11BITS, ELEVEN ) | /* nominal */
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, ADC_TP_OUT_DATA_HVSYNC,  ZERO   ) | /* nominal */
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, ADC_TP_OUT_DATA_HIGH,    ZERO   ) | /* nominal */
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, ADC_TP_OUT_DATA_MID,     ZERO   ) | /* nominal */
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, ADC_TP_OUT_DATA_LOW,     ZERO   ) | /* nominal */
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, PCIN_TP_IN_SEL,          NORMAL ) | /* nominal */
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, ADC_MSB_INVERT,          DISABLE) | /* nominal */
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, IFD_1_SELECT,            ADC_5  ) | /* nominal */
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, IFD_0_SELECT,            ADC_4  ) | /* nominal */
#elif (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, FD_MF_IN_SELECT,  CH0 ) | /* nominal */
		((pCurInfo->bScartInput)
		? BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, FD_LPF_IN_SELECT,  CH0)
		: BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, FD_LPF_IN_SELECT,  CH0)     ) |
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, PCIN_TP_IN_SEL,    NORMAL     ) | /* nominal */
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, ADC_MSB_INVERT,    DISABLE    ) | /* nominal */
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, IFD_1_SELECT,      ADC_2      ) | /* nominal */
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, IFD_0_SELECT,      ADC_4      ) | /* nominal */
#elif (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_3)
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, IFD_CLK_SEL,       ADC_CH2_CLK) | /* nominal */
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, PCIN_TP_IN_SEL,    NORMAL     ) | /* nominal */
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, ADC_MSB_INVERT,    DISABLE    ) | /* nominal */
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, IFD_1_SELECT,      ADC_5      ) | /* nominal */
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, IFD_0_SELECT,      ADC_2      ) | /* nominal */
#elif (BVDC_P_SUPPORT_VDEC_VER == BVDC_P_VDEC_VER_2)
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, ADC_MSB_LSB,       MSB        ) | /* nominal */
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, ADC_MSB_INVERT,    DISABLE    ) | /* nominal */
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, IFD_1_SELECT,      ADC_5      ) | /* nominal */
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, IFD_0_SELECT,      ADC_2      ) | /* nominal */
#endif
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, DEBUG_SELECT,      VDEC_PRI   ) | /* nominal */
		BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, ADC_BYPASS,        DISABLE    ) | /* nominal */
		BCHP_FIELD_DATA(VD_TOP_0_VD_TEST_PORT, ADC_TP_OUT_SELECT, 0          );  /* nominal */

	/* Which ever VDEC uses IFD input we'll set the IFD feedback to that vdec. */
	if(pCurInfo->bIfdInput)
	{
		ulTpAndMask |=
			BCHP_MASK(VD_TOP_0_VD_TEST_PORT, IFD_FEEDBACK_SELECT);

		ulTpOrMask  |= (BVDC_P_VdecId_eVdec0 == hVdec->eId)
			? BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, IFD_FEEDBACK_SELECT, VDEC_PRI)
			: BCHP_FIELD_ENUM(VD_TOP_0_VD_TEST_PORT, IFD_FEEDBACK_SELECT, VDEC_SEC);
	}

	/* VD_TOP_0_VD_TEST_PORT (RW) */
	BVDC_P_RD_MOD_WR_RUL(pList->pulCurrent, ~ulTpAndMask, ulTpOrMask,
		BCHP_VD_TOP_0_VD_TEST_PORT);

	/* VD_TOP_0_VD_VDEC_0_CTL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VD_TOP_0_VD_VDEC_0_CTL + hVdec->ulTopOffset);
	*pList->pulCurrent++ =
		((pCurInfo->bUsePllClk)
		? BCHP_FIELD_ENUM(VD_TOP_0_VD_VDEC_0_CTL, ADC_CLOCK_RATE, ADC_216)       /* pll at 216Mhz */
		: BCHP_FIELD_ENUM(VD_TOP_0_VD_VDEC_0_CTL, ADC_CLOCK_RATE, ADC_108)   ) | /* sys at 108Mhz */
		((BCHP_VD_TOP_0_VD_VDEC_0_CTL_ATSC_SAMPLE_RATE_SR_216 == hVdec->ulSampleRate)
		? BCHP_FIELD_ENUM(VD_TOP_0_VD_VDEC_0_CTL, AA_FILTER_BYPASS, ENABLE )
		: BCHP_FIELD_ENUM(VD_TOP_0_VD_VDEC_0_CTL, AA_FILTER_BYPASS, DISABLE) ) |
		BCHP_FIELD_ENUM(VD_TOP_0_VD_VDEC_0_CTL, BYPASS_REALIGN_FIFO, DISABLE ) | /* nominal */
		BCHP_FIELD_DATA(VD_TOP_0_VD_VDEC_0_CTL, FD_CLAMP_SEL,
			hVdec->ulClampSel                                             ) | /* base on adc/format */
		BCHP_FIELD_ENUM(VD_TOP_0_VD_VDEC_0_CTL, ATSC_SOURCE,      VAFE    ) | /* nominal */
		BCHP_FIELD_ENUM(VD_TOP_0_VD_VDEC_0_CTL, ATSC_FIFO,        ENABLE  ) | /* nominal */
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		((pCurInfo->bIntIfd)
		? BCHP_FIELD_ENUM(VD_TOP_0_VD_VDEC_0_CTL, INPUT_SEL,      IFD)  /* base on adc */
		:(pCurInfo->bPcInput)
		? BCHP_FIELD_ENUM(VD_TOP_0_VD_VDEC_0_CTL, INPUT_SEL,      PC_INPUT)  /* base on adc */
		: BCHP_FIELD_ENUM(VD_TOP_0_VD_VDEC_0_CTL, INPUT_SEL,      ATSC    ))|
		((pCurInfo->apAdc[3])
		? BCHP_FIELD_DATA(VD_TOP_0_VD_VDEC_0_CTL, CH3_SELECT, pCurInfo->apAdc[3]->ulChnSel) /* base on adc */
		: BCHP_FIELD_ENUM(VD_TOP_0_VD_VDEC_0_CTL, CH3_SELECT, ZERO                        )) |
#else
		((pCurInfo->bPcInput)
		? BCHP_FIELD_ENUM(VD_TOP_0_VD_VDEC_0_CTL, INPUT_SEL,      PC_INPUT)  /* base on adc */
		: BCHP_FIELD_ENUM(VD_TOP_0_VD_VDEC_0_CTL, INPUT_SEL,      ATSC    ))|
#endif
		((hVdec->bFullSampleRate)
		? BCHP_FIELD_ENUM(VD_TOP_0_VD_VDEC_0_CTL, ATSC_SAMPLE_RATE, SR_216)
		: BCHP_FIELD_DATA(VD_TOP_0_VD_VDEC_0_CTL, ATSC_SAMPLE_RATE, hVdec->ulSampleRate)  )| /* base on format */
		BCHP_FIELD_DATA(VD_TOP_0_VD_VDEC_0_CTL, ATSC_AA_FILTER, hVdec->ulAaFilter         )| /* base on format */
		((pCurInfo->apAdc[2])
		? BCHP_FIELD_DATA(VD_TOP_0_VD_VDEC_0_CTL, CH2_SELECT, pCurInfo->apAdc[2]->ulChnSel) /* base on adc */
		: BCHP_FIELD_ENUM(VD_TOP_0_VD_VDEC_0_CTL, CH2_SELECT, ZERO                        ))|
		((pCurInfo->apAdc[1])
		? BCHP_FIELD_DATA(VD_TOP_0_VD_VDEC_0_CTL, CH1_SELECT, pCurInfo->apAdc[1]->ulChnSel) /* base on adc */
		: BCHP_FIELD_ENUM(VD_TOP_0_VD_VDEC_0_CTL, CH1_SELECT, ZERO                        ))|
		((pCurInfo->apAdc[0])
		? BCHP_FIELD_DATA(VD_TOP_0_VD_VDEC_0_CTL, CH0_SELECT, pCurInfo->apAdc[0]->ulChnSel) /* base on adc */
		: BCHP_FIELD_ENUM(VD_TOP_0_VD_VDEC_0_CTL, CH0_SELECT, ZERO                        ));

#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
	/* VD_TOP_0_VD_VDEC_0_CTL_1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VD_TOP_0_VD_VDEC_0_CTL_1 + hVdec->ulTopOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VD_TOP_0_VD_VDEC_0_CTL_1, RAW_11B, pCurInfo->bFullRange ) |
		BCHP_FIELD_DATA(VD_TOP_0_VD_VDEC_0_CTL_1, DITHER_MULT_BY_2, 0           ) |
		((pCurInfo->apAdc[0])
		? BCHP_FIELD_DATA(VD_TOP_0_VD_VDEC_0_CTL_1, FD_MF_IN_SELECT,  (pCurInfo->apAdc[0]->ulChnSel - 1)) /* base on adc */
		: BCHP_FIELD_DATA(VD_TOP_0_VD_VDEC_0_CTL_1, FD_MF_IN_SELECT, 0)         ) |
		((pCurInfo->apAdc[0])
		? BCHP_FIELD_DATA(VD_TOP_0_VD_VDEC_0_CTL_1, FD_LPF_IN_SELECT, (pCurInfo->apAdc[0]->ulChnSel - 1)) /* base on adc */
		: BCHP_FIELD_DATA(VD_TOP_0_VD_VDEC_0_CTL_1, FD_LPF_IN_SELECT, 0)        );
#endif

#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_3)
	/* VD_TOP_0_ADC1_DITHER (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VD_TOP_0_ADC1_DITHER);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(VD_TOP_0_ADC1_DITHER, LOAD_LFSR,   ON    ) |
		BCHP_FIELD_DATA(VD_TOP_0_ADC1_DITHER, LFSR,        0xFFC ) |
		BCHP_FIELD_ENUM(VD_TOP_0_ADC1_DITHER, POLYNOM_T2,  B17   ) |
		BCHP_FIELD_ENUM(VD_TOP_0_ADC1_DITHER, POLYNOM_T1,  B8    ) |
		BCHP_FIELD_ENUM(VD_TOP_0_ADC1_DITHER, POLYNOM_T0,  B2    ) |
		((hVdec->hSource->bTestFeature1)
		? BCHP_FIELD_DATA(VD_TOP_0_ADC1_DITHER, ENABLE_BITS, 0x0 )
		: BCHP_FIELD_DATA(VD_TOP_0_ADC1_DITHER, ENABLE_BITS, 0x3 ));

	/* VD_TOP_0_ADC2_DITHER (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VD_TOP_0_ADC2_DITHER);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(VD_TOP_0_ADC2_DITHER, LOAD_LFSR,   ON     ) |
		BCHP_FIELD_DATA(VD_TOP_0_ADC2_DITHER, LFSR,        0xFFFE ) |
		BCHP_FIELD_ENUM(VD_TOP_0_ADC2_DITHER, POLYNOM_T2,  B16    ) |
		BCHP_FIELD_ENUM(VD_TOP_0_ADC2_DITHER, POLYNOM_T1,  B13    ) |
		BCHP_FIELD_ENUM(VD_TOP_0_ADC2_DITHER, POLYNOM_T0,  B2     ) |
		((hVdec->hSource->bTestFeature1)
		? BCHP_FIELD_DATA(VD_TOP_0_ADC2_DITHER, ENABLE_BITS, 0x0  )
		: BCHP_FIELD_DATA(VD_TOP_0_ADC2_DITHER, ENABLE_BITS, 0x3  ));

	/* VD_TOP_0_ADC3_DITHER (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VD_TOP_0_ADC3_DITHER);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(VD_TOP_0_ADC3_DITHER, LOAD_LFSR,   ON      ) |
		BCHP_FIELD_DATA(VD_TOP_0_ADC3_DITHER, LFSR,        0x3E0F8 ) |
		BCHP_FIELD_ENUM(VD_TOP_0_ADC3_DITHER, POLYNOM_T2,  B18     ) |
		BCHP_FIELD_ENUM(VD_TOP_0_ADC3_DITHER, POLYNOM_T1,  B14     ) |
		BCHP_FIELD_ENUM(VD_TOP_0_ADC3_DITHER, POLYNOM_T0,  B3      ) |
		((hVdec->hSource->bTestFeature1)
		? BCHP_FIELD_DATA(VD_TOP_0_ADC3_DITHER, ENABLE_BITS, 0x0   )
		: BCHP_FIELD_DATA(VD_TOP_0_ADC3_DITHER, ENABLE_BITS, 0x3   ));

	/* VD_TOP_0_ADC4_DITHER (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VD_TOP_0_ADC4_DITHER);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(VD_TOP_0_ADC4_DITHER, LOAD_LFSR,   ON      ) |
		BCHP_FIELD_DATA(VD_TOP_0_ADC4_DITHER, LFSR,        0x3F003 ) |
		BCHP_FIELD_ENUM(VD_TOP_0_ADC4_DITHER, POLYNOM_T2,  B18     ) |
		BCHP_FIELD_ENUM(VD_TOP_0_ADC4_DITHER, POLYNOM_T1,  B13     ) |
		BCHP_FIELD_ENUM(VD_TOP_0_ADC4_DITHER, POLYNOM_T0,  B8      ) |
		((hVdec->hSource->bTestFeature1)
		? BCHP_FIELD_DATA(VD_TOP_0_ADC4_DITHER, ENABLE_BITS, 0x0   )
		: BCHP_FIELD_DATA(VD_TOP_0_ADC4_DITHER, ENABLE_BITS, 0x3   ));
#endif

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_Vdec_BuildFdRul_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t ulFdCtrl;
	uint32_t ulOffset;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	ulOffset = hVdec->ulFdOffset;

	/* FD_0_REV_ID (RO) */
	/* FD_0_LPF_TIP (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_LPF_TIP + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(FD_0_LPF_TIP, WINDOW, 16 ) |
		BCHP_FIELD_DATA(FD_0_LPF_TIP, TIP,    24 );

	/* FD_0_MF_TIP (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_MF_TIP + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(FD_0_MF_TIP, WINDOW, 648 );

	/* FD_0_AGC_0 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_AGC_0 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(FD_0_AGC_0, BIN_CLEAR,         DISABLE ) |
		BCHP_FIELD_DATA(FD_0_AGC_0, TIP_BIN_GAIN,      1       ) |
		BCHP_FIELD_ENUM(FD_0_AGC_0, ACCUMULATOR_CLEAR, DISABLE ) |
		BCHP_FIELD_DATA(FD_0_AGC_0, WINDOW,            32      ) |
		BCHP_FIELD_DATA(FD_0_AGC_0, BLANK_THRESHOLD,   2000    ) |
		BCHP_FIELD_DATA(FD_0_AGC_0, ACCUMULATOR_GAIN,  4       ) |
		BCHP_FIELD_DATA(FD_0_AGC_0, BLANK_LEVEL,       7       );

	/* FD_0_AGC_1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_AGC_1 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(FD_0_AGC_1, BIN_LIMIT,   32 ) |
		BCHP_FIELD_DATA(FD_0_AGC_1, TIP,         24 ) |
		BCHP_FIELD_DATA(FD_0_AGC_1, NUM_OF_BINS, 12 ) |
		BCHP_FIELD_DATA(FD_0_AGC_1, BIN_WIDTH,   32 );

	/* FD_0_AGC_2 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_AGC_2 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(FD_0_AGC_2, BIN_SEL,     0    ) |
		BCHP_FIELD_DATA(FD_0_AGC_2, MIN_GAIN,    0    ) |
		BCHP_FIELD_DATA(FD_0_AGC_2, MAX_GAIN,    15   ) |
		BCHP_FIELD_ENUM(FD_0_AGC_2, TYPE,        AUTO ) |
		BCHP_FIELD_DATA(FD_0_AGC_2, MANUAL_GAIN, 22528 );

	/* FD_0_SD_AGC_0 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_SD_AGC_0 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(FD_0_SD_AGC_0, ACCUMULATOR_CLEAR, DISABLE ) |
		BCHP_FIELD_DATA(FD_0_SD_AGC_0, ACCUMULATOR_GAIN,  4       ) |
		BCHP_FIELD_DATA(FD_0_SD_AGC_0, BLANK_REF,         224     );

	/* FD_0_SD_AGC_1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_SD_AGC_1 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(FD_0_SD_AGC_1, TYPE,        AUTO ) |
		BCHP_FIELD_DATA(FD_0_SD_AGC_1, MANUAL_GAIN, 2048 );

	/* FD_0_SD_AGC_2 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_SD_AGC_2 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(FD_0_SD_AGC_2, WINDOW,   648 ) |
		BCHP_FIELD_DATA(FD_0_SD_AGC_2, MIN_GAIN, 0   ) |
		BCHP_FIELD_DATA(FD_0_SD_AGC_2, MAX_GAIN, 63  );

	/* FD_0_DCR_CTRL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_DCR_CTRL + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(FD_0_DCR_CTRL, LPF_GAIN, 7 );
#else
		BCHP_FIELD_DATA(FD_0_DCR_CTRL, GAIN, 7 );
#endif

	/* FD_0_MF_0 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_MF_0 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(FD_0_MF_0, ACCUMULATOR_CLEAR, DISABLE ) |
		BCHP_FIELD_DATA(FD_0_MF_0, MIN_GAIN,          1       ) |
		BCHP_FIELD_DATA(FD_0_MF_0, MAX_GAIN,          5       ) |
		BCHP_FIELD_DATA(FD_0_MF_0, ACCUMULATOR_GAIN,  4       ) |
		BCHP_FIELD_DATA(FD_0_MF_0, BLANK_REF,         224     );

	/* FD_0_MF_1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_MF_1 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(FD_0_MF_1, TIP,         16   ) |
		BCHP_FIELD_ENUM(FD_0_MF_1, TYPE,        AUTO ) |
		BCHP_FIELD_DATA(FD_0_MF_1, MANUAL_GAIN, 2048 );

	/* FD_0_SLICER (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_SLICER + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(FD_0_SLICER, LPF_SLICE, 100 ) |
		BCHP_FIELD_DATA(FD_0_SLICER, MF_SLICE,  100 );

	/* FD_0_SAMPLE_WINDOW (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_SAMPLE_WINDOW + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(FD_0_SAMPLE_WINDOW, BEAT_COUNT, 7600 ) |
		BCHP_FIELD_DATA(FD_0_SAMPLE_WINDOW, COUNT,      2048 );

	/* FD_0_LPF_METRIC_0 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_LPF_METRIC_0 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(FD_0_LPF_METRIC_0, V_AVERAGE_SAMPLE_SELECT, FOUR ) |
		BCHP_FIELD_DATA(FD_0_LPF_METRIC_0, V_POLARITY_THRESHOLD,    1    ) |
		BCHP_FIELD_DATA(FD_0_LPF_METRIC_0, V_METRIC_HIT_MAX,        7    ) |
		BCHP_FIELD_DATA(FD_0_LPF_METRIC_0, V_VALID_THRESHOLD,       4    ) |
		BCHP_FIELD_DATA(FD_0_LPF_METRIC_0, V_INVALID_THRESHOLD,     0    ) |
		((pCurInfo->bPcInput)
		? BCHP_FIELD_DATA(FD_0_LPF_METRIC_0, VWIDTH_TOLERANCE,      20   )
		: BCHP_FIELD_DATA(FD_0_LPF_METRIC_0, VWIDTH_TOLERANCE,      10   ));

	/* FD_0_LPF_METRIC_1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_LPF_METRIC_1 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(FD_0_LPF_METRIC_1, F_AVERAGE_SAMPLE_SELECT, FOUR ) |
		BCHP_FIELD_DATA(FD_0_LPF_METRIC_1, F_METRIC_HIT_MAX,        7    ) |
		BCHP_FIELD_DATA(FD_0_LPF_METRIC_1, F_VALID_THRESHOLD,       4    ) |
		BCHP_FIELD_DATA(FD_0_LPF_METRIC_1, F_INVALID_THRESHOLD,     0    ) |
		((pCurInfo->bPcInput)
		? BCHP_FIELD_DATA(FD_0_LPF_METRIC_1, FLENGTH_TOLERANCE,     8    )
		: BCHP_FIELD_DATA(FD_0_LPF_METRIC_1, FLENGTH_TOLERANCE,     2    ));

	/* FD_0_LPF_METRIC_2 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_LPF_METRIC_2 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(FD_0_LPF_METRIC_2, H_AVERAGE_SAMPLE_SELECT, SIXTEEN ) |
		BCHP_FIELD_DATA(FD_0_LPF_METRIC_2, H_POLARITY_THRESHOLD,    1       ) |
		BCHP_FIELD_DATA(FD_0_LPF_METRIC_2, H_METRIC_HIT_MAX,        127     ) |
		BCHP_FIELD_DATA(FD_0_LPF_METRIC_2, H_VALID_THRESHOLD,       16      ) |
		BCHP_FIELD_DATA(FD_0_LPF_METRIC_2, H_INVALID_THRESHOLD,     0       ) |
		BCHP_FIELD_DATA(FD_0_LPF_METRIC_2, HWIDTH_TOLERANCE,        10      );

	/* FD_0_LPF_METRIC_3 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_LPF_METRIC_3 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(FD_0_LPF_METRIC_3, L_AVERAGE_SAMPLE_SELECT, SIXTEEN ) |
		BCHP_FIELD_DATA(FD_0_LPF_METRIC_3, L_METRIC_HIT_MAX,        127     ) |
		BCHP_FIELD_DATA(FD_0_LPF_METRIC_3, L_VALID_THRESHOLD,       16      ) |
		BCHP_FIELD_DATA(FD_0_LPF_METRIC_3, L_INVALID_THRESHOLD,     0       ) |
		((pCurInfo->bPcInput)
		? BCHP_FIELD_DATA(FD_0_LPF_METRIC_3, LLENGTH_TOLERANCE,     25      )
		: BCHP_FIELD_DATA(FD_0_LPF_METRIC_3, LLENGTH_TOLERANCE,     12      ));

	/* FD_0_LPF_METRIC_4 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_LPF_METRIC_4 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(FD_0_LPF_METRIC_4, R_METRIC_HIT_MAX,    15 ) |
		BCHP_FIELD_DATA(FD_0_LPF_METRIC_4, R_VALID_THRESHOLD,   3  ) |
		BCHP_FIELD_DATA(FD_0_LPF_METRIC_4, R_INVALID_THRESHOLD, 2  ) |
		BCHP_FIELD_DATA(FD_0_LPF_METRIC_4, R_MAX,               12 ) |
		BCHP_FIELD_DATA(FD_0_LPF_METRIC_4, R_MIN,               4  );

	/* FD_0_MF_METRIC_0 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_MF_METRIC_0 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(FD_0_MF_METRIC_0, V_AVERAGE_SAMPLE_SELECT, FOUR ) |
		BCHP_FIELD_DATA(FD_0_MF_METRIC_0, V_POLARITY_THRESHOLD,    1    ) |
		BCHP_FIELD_DATA(FD_0_MF_METRIC_0, V_METRIC_HIT_MAX,        7    ) |
		BCHP_FIELD_DATA(FD_0_MF_METRIC_0, V_VALID_THRESHOLD,       4    ) |
		BCHP_FIELD_DATA(FD_0_MF_METRIC_0, V_INVALID_THRESHOLD,     0    ) |
		BCHP_FIELD_DATA(FD_0_MF_METRIC_0, VWIDTH_TOLERANCE,        20   );

	/* FD_0_MF_METRIC_1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_MF_METRIC_1 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(FD_0_MF_METRIC_1, F_AVERAGE_SAMPLE_SELECT, FOUR ) |
		BCHP_FIELD_DATA(FD_0_MF_METRIC_1, F_METRIC_HIT_MAX,        7    ) |
		BCHP_FIELD_DATA(FD_0_MF_METRIC_1, F_VALID_THRESHOLD,       4    ) |
		BCHP_FIELD_DATA(FD_0_MF_METRIC_1, F_INVALID_THRESHOLD,     0    ) |
		BCHP_FIELD_DATA(FD_0_MF_METRIC_1, FLENGTH_TOLERANCE,       8    );

	/* FD_0_MF_METRIC_2 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_MF_METRIC_2 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(FD_0_MF_METRIC_2, H_AVERAGE_SAMPLE_SELECT, SIXTEEN ) |
		BCHP_FIELD_DATA(FD_0_MF_METRIC_2, H_POLARITY_THRESHOLD,    1       ) |
		BCHP_FIELD_DATA(FD_0_MF_METRIC_2, H_METRIC_HIT_MAX,        127     ) |
		BCHP_FIELD_DATA(FD_0_MF_METRIC_2, H_VALID_THRESHOLD,       16      ) |
		BCHP_FIELD_DATA(FD_0_MF_METRIC_2, H_INVALID_THRESHOLD,     0       ) |
		BCHP_FIELD_DATA(FD_0_MF_METRIC_2, HWIDTH_TOLERANCE,        10      );

	/* FD_0_MF_METRIC_3 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_MF_METRIC_3 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(FD_0_MF_METRIC_3, L_AVERAGE_SAMPLE_SELECT, SIXTEEN ) |
		BCHP_FIELD_DATA(FD_0_MF_METRIC_3, L_METRIC_HIT_MAX,        127     ) |
		BCHP_FIELD_DATA(FD_0_MF_METRIC_3, L_VALID_THRESHOLD,       16      ) |
		BCHP_FIELD_DATA(FD_0_MF_METRIC_3, L_INVALID_THRESHOLD,     0       ) |
		BCHP_FIELD_DATA(FD_0_MF_METRIC_3, LLENGTH_TOLERANCE,       25      );

	/* FD_0_MF_METRIC_4 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_MF_METRIC_4 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(FD_0_MF_METRIC_4, R_METRIC_HIT_MAX,    15 ) |
		BCHP_FIELD_DATA(FD_0_MF_METRIC_4, R_VALID_THRESHOLD,   3  ) |
		BCHP_FIELD_DATA(FD_0_MF_METRIC_4, R_INVALID_THRESHOLD, 2  ) |
		BCHP_FIELD_DATA(FD_0_MF_METRIC_4, R_MAX,               12 ) |
		BCHP_FIELD_DATA(FD_0_MF_METRIC_4, R_MIN,               4  );

	/* FD_0_LPF_ATSC_VSYNC (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_LPF_ATSC_VSYNC + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(FD_0_LPF_ATSC_VSYNC, VCR_COUNT,      0 ) |
		BCHP_FIELD_DATA(FD_0_LPF_ATSC_VSYNC, NUM_SERRATIONS, 2 ) |
		BCHP_FIELD_DATA(FD_0_LPF_ATSC_VSYNC, NUM_LINES,      1 ) |
		BCHP_FIELD_DATA(FD_0_LPF_ATSC_VSYNC, NUM_HWIDTHS,    1 );

	/* FD_0_MF_ATSC_VSYNC (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_MF_ATSC_VSYNC + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(FD_0_MF_ATSC_VSYNC, VCR_COUNT,      6330 ) |
		BCHP_FIELD_DATA(FD_0_MF_ATSC_VSYNC, NUM_SERRATIONS, 0    ) |
		BCHP_FIELD_DATA(FD_0_MF_ATSC_VSYNC, NUM_LINES,      3    ) |
		BCHP_FIELD_DATA(FD_0_MF_ATSC_VSYNC, NUM_HWIDTHS,    3    );

	/* FD_0_FORMAT_TABLE_0 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_FORMAT_TABLE_0 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(FD_0_FORMAT_TABLE_0, START_ADDR, pCurInfo->ulFdtMemOffset );

	/* FD_0_FORMAT_TABLE_1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_FORMAT_TABLE_1 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(FD_0_FORMAT_TABLE_1, LENGTH, pCurInfo->ulFdtCount     );

	/* Build-in Entries!  Don't use built-in entries for component/pc input
	 * for format detection.  Disabling it by making it not possible to
	 * match */
	if(pCurInfo->bCompInput || pCurInfo->bPcInput)
	{
		/* Assume it's BCHP_FD_0_MF_FORMAT_ENTRY_00 to
		 *             BCHP_FD_0_MF_FORMAT_ENTRY_31 is 8 register apart. */
		*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS( 8 );
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_MF_FORMAT_ENTRY_00 + ulOffset);
		*pList->pulCurrent++ = 0xffffffff;     /* BCHP_FD_0_MF_FORMAT_ENTRY_00 */
		*pList->pulCurrent++ = 0xffffffff;     /* BCHP_FD_0_MF_FORMAT_ENTRY_01 */
		*pList->pulCurrent++ = 0xffffffff;     /* BCHP_FD_0_MF_FORMAT_ENTRY_10 */
		*pList->pulCurrent++ = 0xffffffff;     /* BCHP_FD_0_MF_FORMAT_ENTRY_11 */
		*pList->pulCurrent++ = 0xffffffff;     /* BCHP_FD_0_MF_FORMAT_ENTRY_20 */
		*pList->pulCurrent++ = 0xffffffff;     /* BCHP_FD_0_MF_FORMAT_ENTRY_21 */
		*pList->pulCurrent++ = 0xffffffff;     /* BCHP_FD_0_MF_FORMAT_ENTRY_30 */
		*pList->pulCurrent++ = 0xffffffff;     /* BCHP_FD_0_MF_FORMAT_ENTRY_31 */
	}
	else
	{
		/* FD_0_MF_FORMAT_ENTRY_00 (RW) : NTSC */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_MF_FORMAT_ENTRY_00 + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(FD_0_MF_FORMAT_ENTRY_00, FT_HSYNC_POLARITY, NEGATIVE  ) |
			BCHP_FIELD_ENUM(FD_0_MF_FORMAT_ENTRY_00, FT_VSYNC_POLARITY, NEGATIVE  ) |
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_00, FT_DELTA_LL,       31        ) |
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_00, FT_HSYNC_WIDTH,    0         ) |
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_00, FT_DELTA_HW,       10        ) |
			BCHP_FIELD_ENUM(FD_0_MF_FORMAT_ENTRY_00, FT_RASTER_TYPE,    DONT_CARE );

		/* FD_0_MF_FORMAT_ENTRY_01 (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_MF_FORMAT_ENTRY_01 + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_01, FT_LINES_PER_FIELD, 4096 ) |
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_01, FT_DELTA_LPF,       15   ) |
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_01, FT_LINE_LENGTH,     0    );

		/* FD_0_MF_FORMAT_ENTRY_10 (RW) : PAL */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_MF_FORMAT_ENTRY_10 + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(FD_0_MF_FORMAT_ENTRY_10, FT_HSYNC_POLARITY, NEGATIVE   ) |
			BCHP_FIELD_ENUM(FD_0_MF_FORMAT_ENTRY_10, FT_VSYNC_POLARITY, NEGATIVE   ) |
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_10, FT_DELTA_LL,       31         ) |
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_10, FT_HSYNC_WIDTH,    0          ) |
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_10, FT_DELTA_HW,       10         ) |
			BCHP_FIELD_ENUM(FD_0_MF_FORMAT_ENTRY_10, FT_RASTER_TYPE,    DONT_CARE  );

		/* FD_0_MF_FORMAT_ENTRY_11 (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_MF_FORMAT_ENTRY_11 + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_11, FT_LINES_PER_FIELD, 5008 ) |
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_11, FT_DELTA_LPF,       15   ) |
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_11, FT_LINE_LENGTH,     0    );

		/* FD_0_MF_FORMAT_ENTRY_20 (RW) : NOT_USED */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_MF_FORMAT_ENTRY_20 + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(FD_0_MF_FORMAT_ENTRY_20, FT_HSYNC_POLARITY, NEGATIVE  ) |
			BCHP_FIELD_ENUM(FD_0_MF_FORMAT_ENTRY_20, FT_VSYNC_POLARITY, NEGATIVE  ) |
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_20, FT_DELTA_LL,       31        ) |
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_20, FT_HSYNC_WIDTH,    0         ) |
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_20, FT_DELTA_HW,       10        ) |
			BCHP_FIELD_ENUM(FD_0_MF_FORMAT_ENTRY_20, FT_RASTER_TYPE,    DONT_CARE );

		/* FD_0_MF_FORMAT_ENTRY_21 (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_MF_FORMAT_ENTRY_21 + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_21, FT_LINES_PER_FIELD, 8400 ) |
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_21, FT_DELTA_LPF,       10   ) |
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_21, FT_LINE_LENGTH,     6864 );

		/* FD_0_MF_FORMAT_ENTRY_30 (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_MF_FORMAT_ENTRY_30 + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(FD_0_MF_FORMAT_ENTRY_30, FT_HSYNC_POLARITY, NEGATIVE  ) |
			BCHP_FIELD_ENUM(FD_0_MF_FORMAT_ENTRY_30, FT_VSYNC_POLARITY, NEGATIVE  ) |
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_30, FT_DELTA_LL,       31        ) |
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_30, FT_HSYNC_WIDTH,    0         ) |
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_30, FT_DELTA_HW,       10        ) |
			BCHP_FIELD_ENUM(FD_0_MF_FORMAT_ENTRY_30, FT_RASTER_TYPE,    DONT_CARE );

		/* FD_0_MF_FORMAT_ENTRY_31 (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_MF_FORMAT_ENTRY_31 + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_31, FT_LINES_PER_FIELD, 10000 ) |
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_31, FT_DELTA_LPF,       10    ) |
			BCHP_FIELD_DATA(FD_0_MF_FORMAT_ENTRY_31, FT_LINE_LENGTH,     6912  );
	}

	/* FD_0_LPF_TIP_STATUS_0 (RO) */
	/* FD_0_MF_TIP_STATUS_0 (RO) */
	/* FD_0_MF_STATUS (RO) */
	/* FD_0_LPF_AGC_STATUS_0 (RO) */
	/* FD_0_LPF_AGC_STATUS_1 (RO) */
	/* FD_0_MF_AGC_STATUS_0 (RO) */
	/* FD_0_LPF_VIDEO_FORMAT_STATUS_0 (RO) */
	/* FD_0_LPF_VIDEO_FORMAT_STATUS_1 (RO) */
	/* FD_0_MF_VIDEO_FORMAT_STATUS_0 (RO) */
	/* FD_0_MF_VIDEO_FORMAT_STATUS_1 (RO) */
	/* FD_0_LPF_METRIC_STATUS_0 (RO) */
	/* FD_0_LPF_METRIC_STATUS_1 (RO) */
	/* FD_0_MF_METRIC_STATUS_0 (RO) */
	/* FD_0_MF_METRIC_STATUS_1 (RO) */
	/* FD_0_LPF_FORMAT_INTERNAL_STATUS (RO) */
	/* FD_0_MF_FORMAT_INTERNAL_STATUS (RO) */
	/* FD_0_ARBITER_FORMAT_INTERNAL_STATUS (RO) */

	/* Need to be last */
	ulFdCtrl =
		BCHP_FIELD_DATA(FD_0_CTRL, MF_FMT_LLENGTH_TOLERANCE_SCALE,
			BVDC_P_DELTA_LL_SCALE                                             ) |
		BCHP_FIELD_DATA(FD_0_CTRL, LPF_FMT_LLENGTH_TOLERANCE_SCALE,
			BVDC_P_DELTA_LL_SCALE                                             ) |
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(FD_0_CTRL, LPF_INPUT_FILTER,            ENABLE        ) |
#else
		BCHP_FIELD_ENUM(FD_0_CTRL, INPUT_FILTER,                ENABLE        ) |
#endif
		BCHP_FIELD_ENUM(FD_0_CTRL, MF_USE_F_LENGTH_LOCK,        ENABLE        ) |
		BCHP_FIELD_ENUM(FD_0_CTRL, LPF_USE_F_LENGTH_LOCK,       ENABLE        ) |
		BCHP_FIELD_ENUM(FD_0_CTRL, LPF_LLENGTH_TOLERANCE_SCALE, EIGHT         ) |
		BCHP_FIELD_ENUM(FD_0_CTRL, LPF_VIDEO_FORMAT,            MATCH         ) |
		BCHP_FIELD_ENUM(FD_0_CTRL, MF_UPDATE_SEL,               SAMPLE_WINDOW ) |
		BCHP_FIELD_ENUM(FD_0_CTRL, LPF_UPDATE_SEL,              SAMPLE_WINDOW ) |
		BCHP_FIELD_ENUM(FD_0_CTRL, STATUS_LOCK,                 DISABLE       ) |
		BCHP_FIELD_ENUM(FD_0_CTRL, FD,                          ENABLE        );

	/* Different lock for PC vs ATSC mode! */
	if(pCurInfo->bPcInput)
	{
		ulFdCtrl |=
			BCHP_FIELD_ENUM(FD_0_CTRL, MF_USE_RASTER_LOCK,         ENABLE      ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, MF_USE_LINE_LENGTH_LOCK,    ENABLE      ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, MF_USE_HSYNC_WIDTH_LOCK,    ENABLE      ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, MF_USE_VSYNC_WIDTH_LOCK,    ENABLE      ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, LPF_USE_RASTER_LOCK,        ENABLE      ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, LPF_USE_LINE_LENGTH_LOCK,   ENABLE      ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, LPF_USE_HSYNC_WIDTH_LOCK,   ENABLE      ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, LPF_USE_VSYNC_WIDTH_LOCK,   ENABLE      ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, MF_LLENGTH_TOLERANCE_SCALE, EIGHT       ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, MF_RASTER_TYPE,             HSYNC_TIMING) |
			BCHP_FIELD_ENUM(FD_0_CTRL, LPF_RASTER_TYPE,            HSYNC_TIMING);
	}
	else if (pCurInfo->bCompInput)
	{
		ulFdCtrl |=
			BCHP_FIELD_ENUM(FD_0_CTRL, MF_USE_RASTER_LOCK,         ENABLE        ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, MF_USE_LINE_LENGTH_LOCK,    ENABLE        ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, MF_USE_HSYNC_WIDTH_LOCK,    ENABLE        ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, MF_USE_VSYNC_WIDTH_LOCK,    ENABLE        ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, LPF_USE_RASTER_LOCK,        DISABLE       ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, LPF_USE_LINE_LENGTH_LOCK,   DISABLE       ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, LPF_USE_HSYNC_WIDTH_LOCK,   DISABLE       ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, LPF_USE_VSYNC_WIDTH_LOCK,   DISABLE       ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, MF_LLENGTH_TOLERANCE_SCALE, SIXTEEN       ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, MF_RASTER_TYPE,             FLNG_OVER_LLNG) |
			BCHP_FIELD_ENUM(FD_0_CTRL, LPF_RASTER_TYPE,            FLNG_OVER_LLNG);
	}
	else
	{
		ulFdCtrl |=
			BCHP_FIELD_ENUM(FD_0_CTRL, MF_USE_RASTER_LOCK,         DISABLE       ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, MF_USE_LINE_LENGTH_LOCK,  DISABLE      ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, MF_USE_HSYNC_WIDTH_LOCK,    DISABLE       ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, MF_USE_VSYNC_WIDTH_LOCK,    DISABLE       ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, LPF_USE_RASTER_LOCK,        DISABLE       ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, LPF_USE_LINE_LENGTH_LOCK,   DISABLE       ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, LPF_USE_HSYNC_WIDTH_LOCK,   DISABLE       ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, LPF_USE_VSYNC_WIDTH_LOCK,   DISABLE       ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, MF_LLENGTH_TOLERANCE_SCALE, SIXTEEN       ) |
			BCHP_FIELD_ENUM(FD_0_CTRL, MF_RASTER_TYPE,             FLNG_OVER_LLNG) |
			BCHP_FIELD_ENUM(FD_0_CTRL, LPF_RASTER_TYPE,            FLNG_OVER_LLNG);
	}

	/* FD_0_CTRL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_FD_0_CTRL + ulOffset);
	*pList->pulCurrent++ = ulFdCtrl;

	return;
}


/***************************************************************************
 *
 */
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
static void BVDC_P_Vdec_BuildPcInRaRul_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t ulOffset = 0;
	uint32_t ulNoiseRgnStart, ulNoiseRgnSize;
	uint32_t ulNoiseRgn, ulSearchWidthH, ulSearchWidthV, ulSearchDenomiTb;
	uint32_t ulScanWidth, ulScanHeight;
	uint32_t ulLineLength, ulLpf;
	const BFMT_VideoInfo *pFmtInfo;
	uint32_t  ulSystemClock;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	pFmtInfo = pCurInfo->pFmtInfo;
	ulSystemClock  =  BVDC_P_216_SYSCLK / 1000000;
	ulLineLength   = BCHP_GET_FIELD_DATA(hVdec->ulFdLpfMetric0Reg, FD_0_LPF_METRIC_STATUS_0, LINE_LENGTH);
	ulScanWidth    = ((pFmtInfo->ulScanWidth) *
		(BVDC_P_PC_CLOCK_ADJ_FACTOR + pCurInfo->stPcInSyncSettings.lClockAdj) +
		 BVDC_P_PC_CLOCK_ADJ_FACTOR / 2) / BVDC_P_PC_CLOCK_ADJ_FACTOR;;

	ulLpf =
	BCHP_GET_FIELD_DATA(hVdec->ulFdLpfMetric1Reg, FD_0_LPF_METRIC_STATUS_1, LINES_PER_FIELD) / 16;
	ulScanHeight = ulLpf << pFmtInfo->bInterlaced;

	ulNoiseRgnSize  = 3;
	ulNoiseRgnStart = 4;

	if((pFmtInfo->eVideoFmt == BFMT_VideoFmt_eDVI_1024x768p)||
	   (pFmtInfo->eVideoFmt == BFMT_VideoFmt_eDVI_1280x768p)||
	   (pFmtInfo->eVideoFmt == BFMT_VideoFmt_eDVI_1360x768p_60Hz))
	{
		ulNoiseRgnSize  = 1;
		ulNoiseRgnStart = 4;
	}

	ulNoiseRgn = ulNoiseRgnStart + ulNoiseRgnSize;

	/* ulSearchWidthH should be even, ulSearchWidthH should be less than 80 */
	ulSearchWidthH = (ulScanWidth -pFmtInfo->ulWidth - 40) / 2;
	ulSearchWidthH = BVDC_P_ALIGN_DN(ulSearchWidthH, BVDC_P_EVEN_PIXEL);
	ulSearchWidthH = BVDC_P_MIN(ulSearchWidthH, 80);

	/* ulSearchWidthV should be even*/
	ulSearchWidthV = (ulScanHeight - (pFmtInfo->ulHeight >> pFmtInfo->bInterlaced) - ulNoiseRgn - 2)/2;
	ulSearchWidthV = BVDC_P_ALIGN_DN(ulSearchWidthV, BVDC_P_EVEN_PIXEL);

	if((pFmtInfo->eVideoFmt == BFMT_VideoFmt_eDVI_1024x768p)||
	   (pFmtInfo->eVideoFmt == BFMT_VideoFmt_eDVI_1280x768p)||
	   (pFmtInfo->eVideoFmt == BFMT_VideoFmt_eDVI_1360x768p_60Hz))
	{
		ulSearchWidthV = 10;
	}

	/* ulSearchDenomiTb */
	ulSearchDenomiTb = (BVDC_P_PC_RA_SEARCH_NOISE_NUMERATOR) /
		(pFmtInfo->ulWidth + ulSearchWidthH * 2 - 2);

	/* PC_IN_0_RA_CONTROL */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PC_IN_0_RA_CONTROL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(PC_IN_0_RA_CONTROL, HW_MD2_NOMINAL_MEASURED_SEL, USE_NOMINAL ) |
		BCHP_FIELD_ENUM(PC_IN_0_RA_CONTROL, HW_MD1_MAX_MEM_DISABLE, DISABLE          ) |
		BCHP_FIELD_ENUM(PC_IN_0_RA_CONTROL, INTERRUPT, ALLOW                         ) |
		BCHP_FIELD_ENUM(PC_IN_0_RA_CONTROL, LIMIT_SEARCH, DISABLE                    ) |
		BCHP_FIELD_ENUM(PC_IN_0_RA_CONTROL, PICTURE_MODE, DRIFT                      ) |
		((pCurInfo->stPcInSyncSettings.bManualPosition || !hVdec->bPcPositionLocked)
		? BCHP_FIELD_ENUM(PC_IN_0_RA_CONTROL, MODE, FREEZE)
		: BCHP_FIELD_ENUM(PC_IN_0_RA_CONTROL, MODE, HW    )                          );

	/* PC_IN_0_RA_DATA_CAPTURE_CTRL */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PC_IN_0_RA_DATA_CAPTURE_CTRL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(PC_IN_0_RA_DATA_CAPTURE_CTRL, RIGHT_REGION_STARTING_ADDRESS, START_R ) |
		BCHP_FIELD_ENUM(PC_IN_0_RA_DATA_CAPTURE_CTRL, TOP_REGION_STARTING_ADDRESS  , START_T ) |
		BCHP_FIELD_ENUM(PC_IN_0_RA_DATA_CAPTURE_CTRL, BOTTOM_REGION_STARTING_ADDRESS,START_B ) |
		BCHP_FIELD_DATA(PC_IN_0_RA_DATA_CAPTURE_CTRL, TIME_CONSTANT, 2                       ) |
		BCHP_FIELD_ENUM(PC_IN_0_RA_DATA_CAPTURE_CTRL, CHANNEL_MERGE, SUM                     ) |
		BCHP_FIELD_ENUM(PC_IN_0_RA_DATA_CAPTURE_CTRL, SUM_DIVIDER, THREE                     ) |
		BCHP_FIELD_ENUM(PC_IN_0_RA_DATA_CAPTURE_CTRL, ZERO_OFFSET, DISABLE                   );

	/* PC_IN_0_RA_NOISE_REGION */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PC_IN_0_RA_NOISE_REGION + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PC_IN_0_RA_NOISE_REGION , START, ulNoiseRgnStart) |
		BCHP_FIELD_DATA(PC_IN_0_RA_NOISE_REGION , SIZE , ulNoiseRgnSize ) ;

	/* PC_IN_0_RA_NOISE_SCALE_THRESHOLD */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PC_IN_0_RA_NOISE_SCALE_THRESHOLD + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PC_IN_0_RA_NOISE_SCALE_THRESHOLD  , SCALE_FACTOR, 15 ) |
		BCHP_FIELD_DATA(PC_IN_0_RA_NOISE_SCALE_THRESHOLD  , FLOOR, 63        );

	/* PC_IN_0_RA_V_ACTIVE_REGION */
	/* TODO: START need a different offset for different timings? */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PC_IN_0_RA_V_ACTIVE_REGION + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PC_IN_0_RA_V_ACTIVE_REGION, START, pFmtInfo->ulTopActive) |
		BCHP_FIELD_DATA(PC_IN_0_RA_V_ACTIVE_REGION, SIZE, pFmtInfo->ulHeight >> pCurInfo->pFmtInfo->bInterlaced);

	/* PC_IN_0_RA_H_ACTIVE_REGION */
	/* TODO: START need a different offset for different timings? */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PC_IN_0_RA_H_ACTIVE_REGION + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PC_IN_0_RA_H_ACTIVE_REGION , START , pCurInfo->pFdEntry->ulHStart + BVDC_P_PC_RA_HSTART_OFFSET) |
		BCHP_FIELD_DATA(PC_IN_0_RA_H_ACTIVE_REGION , SIZE , pFmtInfo->ulWidth      );

	/* PC_IN_0_RA_SEARCH_WIDTH */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PC_IN_0_RA_SEARCH_WIDTH + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PC_IN_0_RA_SEARCH_WIDTH, VERTICAL , ulSearchWidthV   ) |
		BCHP_FIELD_DATA(PC_IN_0_RA_SEARCH_WIDTH, HORIZONTAL , ulSearchWidthH ) ;

	/* PC_IN_0_RA_SEARCH_DENOMINATOR */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PC_IN_0_RA_SEARCH_DENOMINATOR + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PC_IN_0_RA_SEARCH_DENOMINATOR, TOP_BOTTOM, ulSearchDenomiTb  ) |
		BCHP_FIELD_DATA(PC_IN_0_RA_SEARCH_DENOMINATOR, LEFT_RIGHT,
			BVDC_P_PC_RA_SEARCH_NOISE_NUMERATOR / (pFmtInfo->ulHeight >> pFmtInfo->bInterlaced));

	/* PC_IN_0_RA_NOISE_ENERGY_DENOMINATOR */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PC_IN_0_RA_NOISE_ENERGY_DENOMINATOR + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PC_IN_0_RA_NOISE_ENERGY_DENOMINATOR , NOISE_DENOMINATOR,
			BVDC_P_PC_RA_SEARCH_NOISE_NUMERATOR  / pFmtInfo->ulWidth );

	/* PC_IN_0_RA_PICTURE_SIZE */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PC_IN_0_RA_PICTURE_SIZE + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PC_IN_0_RA_PICTURE_SIZE, H_TOTAL , ulScanWidth) |
		BCHP_FIELD_DATA(PC_IN_0_RA_PICTURE_SIZE, V_TOTAL , ulScanHeight);

	/* PC_IN_0_RA_AUTO_RASTER_MBIT_TM */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PC_IN_0_RA_AUTO_RASTER_MBIT_TM + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PC_IN_0_RA_AUTO_RASTER_MBIT_TM, AUTO_RAST_SRAM_TM , 0);

	/* PC_IN_0_RA_CHANNEL_DATA_BLANK_LEVEL */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PC_IN_0_RA_CHANNEL_DATA_BLANK_LEVEL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PC_IN_0_RA_CHANNEL_DATA_BLANK_LEVEL, CHANNEL_0_BLANK, 380 ) |
		BCHP_FIELD_DATA(PC_IN_0_RA_CHANNEL_DATA_BLANK_LEVEL, CHANNEL_1_BLANK, 380 ) |
		BCHP_FIELD_DATA(PC_IN_0_RA_CHANNEL_DATA_BLANK_LEVEL, CHANNEL_2_BLANK, 380 );

	return;
}

/***************************************************************************
 *
 */
static void BVDC_P_Vdec_BuildPassThruRul_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t ulOffset = 0;

	BSTD_UNUSED(hVdec);
	BSTD_UNUSED(pCurInfo);

	/* VD_PASSTHROUGH_0_PASSTHROUGH_REV_ID (RO) */
	/* VD_PASSTHROUGH_0_PASSTHROUGH_CTL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VD_PASSTHROUGH_0_PASSTHROUGH_CTL + ulOffset);
	*pList->pulCurrent++ =
		((pCurInfo->bUsePllClk)
		? BCHP_FIELD_ENUM(VD_PASSTHROUGH_0_PASSTHROUGH_CTL, SAMPLE_RATE, Rate_216Mhz)
		: BCHP_FIELD_ENUM(VD_PASSTHROUGH_0_PASSTHROUGH_CTL, SAMPLE_RATE, Rate_108Mhz)) |

#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_FIELD_DATA(VD_PASSTHROUGH_0_PASSTHROUGH_CTL, BLANK_LEVEL,  512          ) |
		((pCurInfo->bSvidInput)
		? BCHP_FIELD_ENUM(VD_PASSTHROUGH_0_PASSTHROUGH_CTL, INPUT_SVIDEO, SVIDEO)
		: BCHP_FIELD_ENUM(VD_PASSTHROUGH_0_PASSTHROUGH_CTL, INPUT_SVIDEO, CVBS  )    ) |
#endif
		BCHP_FIELD_ENUM(VD_PASSTHROUGH_0_PASSTHROUGH_CTL, AGC_MODE,    AUTO          ) |
		BCHP_FIELD_ENUM(VD_PASSTHROUGH_0_PASSTHROUGH_CTL, AWC,         ENABLE        ) |
		BCHP_FIELD_ENUM(VD_PASSTHROUGH_0_PASSTHROUGH_CTL, PASSTHROUGH, ENABLE        );

	/* VD_PASSTHROUGH_0_PASSTHROUGH_TIP (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VD_PASSTHROUGH_0_PASSTHROUGH_TIP + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VD_PASSTHROUGH_0_PASSTHROUGH_TIP, WINDOW,     32 ) |
		BCHP_FIELD_DATA(VD_PASSTHROUGH_0_PASSTHROUGH_TIP, TIP_OFFSET, 16 );

	/* VD_PASSTHROUGH_0_PASSTHROUGH_BLANK1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VD_PASSTHROUGH_0_PASSTHROUGH_BLANK1 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VD_PASSTHROUGH_0_PASSTHROUGH_BLANK1, WINDOW,          32   ) |
		BCHP_FIELD_DATA(VD_PASSTHROUGH_0_PASSTHROUGH_BLANK1, BLANK_THRESHOLD, 2000 );

	/* VD_PASSTHROUGH_0_PASSTHROUGH_BLANK2 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VD_PASSTHROUGH_0_PASSTHROUGH_BLANK2 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VD_PASSTHROUGH_0_PASSTHROUGH_BLANK2, BIN_SEL,      0       ) |
		BCHP_FIELD_ENUM(VD_PASSTHROUGH_0_PASSTHROUGH_BLANK2, BIN_CLEAR,    DISABLE ) |
		BCHP_FIELD_DATA(VD_PASSTHROUGH_0_PASSTHROUGH_BLANK2, TIP_BIN_GAIN, 1       ) |
		BCHP_FIELD_DATA(VD_PASSTHROUGH_0_PASSTHROUGH_BLANK2, BIN_LIMIT,    32      ) |
		BCHP_FIELD_DATA(VD_PASSTHROUGH_0_PASSTHROUGH_BLANK2, NUM_OF_BINS,  12      ) |
		BCHP_FIELD_DATA(VD_PASSTHROUGH_0_PASSTHROUGH_BLANK2, BIN_WIDTH,    32      );

	/* VD_PASSTHROUGH_0_PASSTHROUGH_AGC (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VD_PASSTHROUGH_0_PASSTHROUGH_AGC + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VD_PASSTHROUGH_0_PASSTHROUGH_AGC, MANUAL_GAIN,     1024 ) |
		BCHP_FIELD_DATA(VD_PASSTHROUGH_0_PASSTHROUGH_AGC, TIME_CONSTANT,   10   ) |
		BCHP_FIELD_DATA(VD_PASSTHROUGH_0_PASSTHROUGH_AGC, REFERENCE_LEVEL, 240  );

	/* VD_PASSTHROUGH_0_PASSTHROUGH_AWC_THRESHOLD (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VD_PASSTHROUGH_0_PASSTHROUGH_AWC_THRESHOLD + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VD_PASSTHROUGH_0_PASSTHROUGH_AWC_THRESHOLD, EXCESS, 900 ) |
		BCHP_FIELD_DATA(VD_PASSTHROUGH_0_PASSTHROUGH_AWC_THRESHOLD, NORMAL, 800 );

	/* VD_PASSTHROUGH_0_PASSTHROUGH_AWC_RATE (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VD_PASSTHROUGH_0_PASSTHROUGH_AWC_RATE + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VD_PASSTHROUGH_0_PASSTHROUGH_AWC_RATE, CLAMP,   24 ) |
		BCHP_FIELD_DATA(VD_PASSTHROUGH_0_PASSTHROUGH_AWC_RATE, RECOVER, 24 );

	/* VD_PASSTHROUGH_0_PASSTHROUGH_MAX_MIN_GAIN (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VD_PASSTHROUGH_0_PASSTHROUGH_MAX_MIN_GAIN + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VD_PASSTHROUGH_0_PASSTHROUGH_MAX_MIN_GAIN, MAX_GAIN, 2047 ) |
		BCHP_FIELD_DATA(VD_PASSTHROUGH_0_PASSTHROUGH_MAX_MIN_GAIN, MIN_GAIN, 1024 );

	/* VD_PASSTHROUGH_0_PASSTHROUGH_STATUS1 (RO) */
	/* VD_PASSTHROUGH_0_PASSTHROUGH_STATUS2 (RO) */
	/* VD_PASSTHROUGH_0_PASSTHROUGH_STATUS3 (RO) */

	return;
}
#endif


/***************************************************************************
 *
 */
static void BVDC_P_Vdec_BuildPcInSyncRul_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t ulLine;
	uint32_t ulOffset = 0;
	uint32_t ulFcwOffset ;
	uint32_t ulFixApll;
	uint32_t ulVsyncMode;
	uint32_t ulHysteresis;

	/* PLL selection. */
	const BVDC_P_PllEntry *pPll = pCurInfo->pFdEntry->pPll;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	ulFcwOffset = (pCurInfo->bPcInput && pCurInfo->stPcInSyncSettings.lClockAdj)
		? hVdec->ulPcManualClockNF : pPll->ulFcwOffset;

	/* BDBG_MSG(("hVdec->bPcInPhaseLocked = %d", hVdec->bPcInPhaseLocked)); */
	ulLine = ((pCurInfo->pFmtInfo->ulScanWidth) *
		(BVDC_P_PC_CLOCK_ADJ_FACTOR + pCurInfo->stPcInSyncSettings.lClockAdj) +
		 BVDC_P_PC_CLOCK_ADJ_FACTOR / 2) / BVDC_P_PC_CLOCK_ADJ_FACTOR;

	ulFixApll = (hVdec->hSource->bInitial || pCurInfo->bPcInput)
		? 0 /* disable. For first time bring up and pc input */
		: 1;/* enable. For non-pc input */
#if (!BVDC_P_VDEC_PR36256_SYNC_LOCK)
	hVdec->bPcPositionLocked = true;
#endif

#if (BVDC_P_VDEC_PR38861_EVERY_VSYNC)
	ulVsyncMode = (pCurInfo->stPcInSyncSettings.bOverrideManualType)
	? (uint32_t) pCurInfo->stPcInSyncSettings.bMultiVsync
	: hVdec->ulVsyncMode;
#else
	ulVsyncMode = (pCurInfo->stPcInSyncSettings.bOverrideManualType)
	? (uint32_t) pCurInfo->stPcInSyncSettings.bMultiVsync
	: 1;
#endif
#if (BVDC_P_VDEC_PR27640_HYSTERESIS)
	ulHysteresis = (pCurInfo->stPcInSyncSettings.bOverrideHysteresis)
	? pCurInfo->stPcInSyncSettings.ulHysteresis
	: hVdec->bPcInPhaseLocked
	? 5
	: 0;
#else
	ulHysteresis = (pCurInfo->stPcInSyncSettings.bOverrideHysteresis)
	? pCurInfo->stPcInSyncSettings.ulHysteresis
	: 5;
#endif

	BDBG_ASSERT(pCurInfo->bUsePllClk);

	/* PC_IN_0_REVISION_ID (RO) */
	/* PC_IN_0_HSYNC_MASK (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PC_IN_0_HSYNC_MASK + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_VDEC_PR36256_SYNC_LOCK)
		BCHP_FIELD_DATA(PC_IN_0_HSYNC_MASK, COEF_A,
			(hVdec->bPcPositionLocked && !hVdec->bToggleHthreshold) ? 2 : BVDC_P_VDEC_COEFAB_TOGGLE ) |
		BCHP_FIELD_DATA(PC_IN_0_HSYNC_MASK, COEF_B,
			(hVdec->bPcPositionLocked && !hVdec->bToggleHthreshold) ? 2 : BVDC_P_VDEC_COEFAB_TOGGLE );
#else
		BCHP_FIELD_DATA(PC_IN_0_HSYNC_MASK, COEF_A, 2 ) |
		BCHP_FIELD_DATA(PC_IN_0_HSYNC_MASK, COEF_B, 2 );
#endif
	/* PC_IN_0_SYNC_CONTROL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PC_IN_0_SYNC_CONTROL + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_3)
		((pCurInfo->bPcInput)
		? BCHP_FIELD_ENUM(PC_IN_0_SYNC_CONTROL, MODE, JITTERLESS      )
		: BCHP_FIELD_ENUM(PC_IN_0_SYNC_CONTROL, MODE, JITTER_CLK      )      ) |
#endif
		BCHP_FIELD_ENUM(PC_IN_0_SYNC_CONTROL, PC_SYNC_FIFO_ALIGNMENT, ENABLE ) |
		BCHP_FIELD_ENUM(PC_IN_0_SYNC_CONTROL, PC_VDEC_SEL_CH2, BLUE          ) |
		BCHP_FIELD_ENUM(PC_IN_0_SYNC_CONTROL, PC_VDEC_SEL_CH1, GREEN         ) |
		BCHP_FIELD_ENUM(PC_IN_0_SYNC_CONTROL, PC_VDEC_SEL_CH0, RED           ) |
		BCHP_FIELD_ENUM(PC_IN_0_SYNC_CONTROL, AFIFO_SEL,   AUTO     ) |
		BCHP_FIELD_DATA(PC_IN_0_SYNC_CONTROL, V_POLARITY,
			hVdec->ulPcVsPol                                        ) |
		BCHP_FIELD_DATA(PC_IN_0_SYNC_CONTROL, H_POLARITY,
			hVdec->ulPcHsPol                                        ) |
		BCHP_FIELD_DATA(PC_IN_0_SYNC_CONTROL, H_THRESHOLD,
			(hVdec->bPcPositionLocked && !hVdec->bToggleHthreshold)? 5 : 0    ) |
		BCHP_FIELD_ENUM(PC_IN_0_SYNC_CONTROL, ENABLE,      ON       );

	/* PC_IN_0_DPLL_FILTER_GAIN (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PC_IN_0_DPLL_FILTER_GAIN + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PC_IN_0_DPLL_FILTER_GAIN, FCW_OFFSET, ulFcwOffset) |
		BCHP_FIELD_DATA(PC_IN_0_DPLL_FILTER_GAIN, LPF_COEF,   3          ) |
		((pCurInfo->bPcInput)
		? BCHP_FIELD_DATA(PC_IN_0_DPLL_FILTER_GAIN, ACC_GAIN, 7      )
		: BCHP_FIELD_DATA(PC_IN_0_DPLL_FILTER_GAIN, ACC_GAIN, 0      )   ) |
		((pCurInfo->bPcInput)
		? BCHP_FIELD_DATA(PC_IN_0_DPLL_FILTER_GAIN, LPF_GAIN, 7      )
		: BCHP_FIELD_DATA(PC_IN_0_DPLL_FILTER_GAIN, LPF_GAIN, 0      )   );

	/* PC_IN_0_DPLL_CTRL (RW) for auto/manual clock adjustment */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PC_IN_0_DPLL_CTRL + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_3)
		BCHP_FIELD_DATA(PC_IN_0_DPLL_CTRL, STATUS_SYNC_BY_VSYNC,      1      ) |
#endif
		BCHP_FIELD_DATA(PC_IN_0_DPLL_CTRL, FIX_APLL, ulFixApll               ) |
		BCHP_FIELD_DATA(PC_IN_0_DPLL_CTRL, JITTER_CLOCK_PHASE_INCDEC, 0      ) |
		BCHP_FIELD_DATA(PC_IN_0_DPLL_CTRL, DPLL_UPDATE,               16     ) |
		BCHP_FIELD_ENUM(PC_IN_0_DPLL_CTRL, PLL_LOCK_THRESHOLD,        NOMINAL) |
		BCHP_FIELD_ENUM(PC_IN_0_DPLL_CTRL, PLL_SETTLING_TIMEOUT,      NOMINAL) |
		BCHP_FIELD_DATA(PC_IN_0_DPLL_CTRL, LINE_LENGTH,               ulLine ) |
		BCHP_FIELD_DATA(PC_IN_0_DPLL_CTRL, JITTER_CLOCK_PHASE,        0      ) |
		((pCurInfo->bPcInput)
		? BCHP_FIELD_ENUM(PC_IN_0_DPLL_CTRL, JITTER_CLOCK_OVERRIDE,   AUTO)
		: BCHP_FIELD_ENUM(PC_IN_0_DPLL_CTRL, JITTER_CLOCK_OVERRIDE,   MANUAL));

	/* PC_IN_0_PHASE_LOCK_CH_OVERRIDE (RW) for auto/manual phase adjustment */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PC_IN_0_PHASE_LOCK_CH_OVERRIDE + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_3)
		BCHP_FIELD_DATA(PC_IN_0_PHASE_LOCK_CH_OVERRIDE, MANUAL_TYPE, ulVsyncMode     ) |
		BCHP_FIELD_ENUM(PC_IN_0_PHASE_LOCK_CH_OVERRIDE, PHASE_LOCK_TYPE, MANUAL      ) |
#endif
		BCHP_FIELD_DATA(PC_IN_0_PHASE_LOCK_CH_OVERRIDE, CH2_PHASE,
			pCurInfo->stPcInSyncSettings.aulPhase[2]               ) |
		BCHP_FIELD_DATA(PC_IN_0_PHASE_LOCK_CH_OVERRIDE, CH1_PHASE,
			pCurInfo->stPcInSyncSettings.aulPhase[1]               ) |
		BCHP_FIELD_DATA(PC_IN_0_PHASE_LOCK_CH_OVERRIDE, CH0_PHASE,
			pCurInfo->stPcInSyncSettings.aulPhase[0]               ) |
		BCHP_FIELD_DATA(PC_IN_0_PHASE_LOCK_CH_OVERRIDE, OVERRIDE,
			pCurInfo->stPcInSyncSettings.bManualPhase              );

	/* PC_IN_0_PHASE_LOCK_CONTROL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PC_IN_0_PHASE_LOCK_CONTROL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PC_IN_0_PHASE_LOCK_CONTROL, BLANK_AVG,
			pCurInfo->pPcInCfg->ulBlankAvg                                           ) |
		BCHP_FIELD_DATA(PC_IN_0_PHASE_LOCK_CONTROL, CAL_HSYNC_OFFSET,
			pCurInfo->pPcInCfg->ulDcrCtrl                                            ) |
		BCHP_FIELD_ENUM(PC_IN_0_PHASE_LOCK_CONTROL, PHASE_LOCK_CNT_MAX, EIGHT        ) |
		BCHP_FIELD_ENUM(PC_IN_0_PHASE_LOCK_CONTROL, PHASE_LOCK_TH,      NOMINAL      ) |
		BCHP_FIELD_ENUM(PC_IN_0_PHASE_LOCK_CONTROL, DERIVATIVE_SEL,     DERIVATIVE_1 ) |
		BCHP_FIELD_DATA(PC_IN_0_PHASE_LOCK_CONTROL, PHASE_HYSTERESIS,   ulHysteresis ) |
		BCHP_FIELD_DATA(PC_IN_0_PHASE_LOCK_CONTROL, PHASE_OFFSET,       4            ) |
		BCHP_FIELD_DATA(PC_IN_0_PHASE_LOCK_CONTROL, DIFF_THRESHOLD,
			pCurInfo->pPcInCfg->ulDiffThreshold                                      );

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_Vdec_BuildPcInRul_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t ulOffset = 0;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	BDBG_ASSERT(pCurInfo->bUsePllClk);

	/* PC_IN_0_SYNC_CONTROL (RW) */
	/* move to BVDC_P_Vdec_BuildPcInSyncRul_isr()*/

	/* PC_IN_0_DPLL_CLAMP (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PC_IN_0_DPLL_CLAMP + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PC_IN_0_DPLL_CLAMP, DPLL_CLAMP,       0       ) |
		BCHP_FIELD_ENUM(PC_IN_0_DPLL_CLAMP, DPLL_ACCUM_CLAMP, NOMINAL );

	/* PC_IN_0_PHASE_LOCK_CONTROL1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PC_IN_0_PHASE_LOCK_CONTROL1 + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_3)
		BCHP_FIELD_ENUM(PC_IN_0_PHASE_LOCK_CONTROL1, HYSTERESIS_TYPE,   AUTO              ) |
#endif
		BCHP_FIELD_DATA(PC_IN_0_PHASE_LOCK_CONTROL1, COUNT_THRESH,      2                 ) |
		BCHP_FIELD_ENUM(PC_IN_0_PHASE_LOCK_CONTROL1, INDEX_RATIO,       FIFTEEN_SIXTEENTH ) |
		BCHP_FIELD_ENUM(PC_IN_0_PHASE_LOCK_CONTROL1, PHASE_LOCK_ENABLE, ENABLE            );

	/* PC_IN_0_DPLL_FILTER_GAIN (RW) */
	/* PC_IN_0_DPLL_CTRL (RW) */
	/* PC_IN_0_PHASE_LOCK_CH_OVERRIDE (RW) */
	BVDC_P_Vdec_BuildPcInSyncRul_isr(hVdec, pList, pCurInfo);

	/* PC_IN_0_RA_CONTROL */
	/* PC_IN_0_RA_DATA_CAPTURE_CTRL */
	/* PC_IN_0_RA_NOISE_REGION */
	/* PC_IN_0_RA_NOISE_SCALE_THRESHOLD */
	/* PC_IN_0_RA_V_ACTIVE_REGION */
	/* PC_IN_0_RA_H_ACTIVE_REGION */
	/* PC_IN_0_RA_SEARCH_WIDTH */
	/* PC_IN_0_RA_SEARCH_DENOMINATOR */
	/* PC_IN_0_RA_NOISE_ENERGY_DENOMINATOR */
	/* PC_IN_0_RA_PICTURE_SIZE */
	/* PC_IN_0_RA_AUTO_RASTER_MBIT_TM */
	/* PC_IN_0_RA_CHANNEL_DATA_BLANK_LEVEL */
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
	BVDC_P_Vdec_BuildPcInRaRul_isr(hVdec, pList, pCurInfo);
#endif

	/* PC_IN_0_SYNC_DPLL_STATUS (RW) */
	/* PC_IN_0_PHASE_LOCK_CH0_DERIVATIVE (RO) */
	/* PC_IN_0_PHASE_LOCK_CH1_DERIVATIVE (RO) */
	/* PC_IN_0_PHASE_LOCK_CH2_DERIVATIVE (RO) */
	/* PC_IN_0_PHASE_STATUS (RO) */
	/* PC_IN_0_PHASE_LOCK_BIN_M4_STATUS (RO) */
	/* PC_IN_0_PHASE_LOCK_BIN_M3_STATUS (RO) */
	/* PC_IN_0_PHASE_LOCK_BIN_M2_STATUS (RO) */
	/* PC_IN_0_PHASE_LOCK_BIN_M1_STATUS (RO) */
	/* PC_IN_0_PHASE_LOCK_BIN_P1_STATUS (RO) */
	/* PC_IN_0_PHASE_LOCK_BIN_P2_STATUS (RO) */
	/* PC_IN_0_PHASE_LOCK_BIN_P3_STATUS (RO) */
	/* PC_IN_0_PHASE_LOCK_BIN_P4_STATUS (RO) */
	/* PC_IN_0_PHASE_LOCK_BIN_P5_STATUS (RO) */
	/* PC_IN_0_PHASE_LOCK_BIN_P6_STATUS (RO) */
	/* PC_IN_0_PHASE_LOCK_BIN_P7_STATUS (RO) */
	/* PC_IN_0_PHASE_LOCK_BIN_P8_STATUS (RO) */
	/* PC_IN_0_PHASE_LOCK_BIN_P9_STATUS (RO) */
	/* PC_IN_0_PHASE_LOCK_BIN_P10_STATUS (RO) */
	/* PC_IN_0_PHASE_LOCK_BIN_P11_STATUS (RO) */

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_Vdec_BuildPllSyncRul_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t ulN, ulM;
	uint32_t ulOffset = 0;
	const BVDC_P_PllEntry *pPll = pCurInfo->pFdEntry->pPll;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	BDBG_ASSERT(pCurInfo->bUsePllClk);

	/*FD should work in 216M for ATSC input*/
	if(!pCurInfo->bPcInput)
	{
		pPll = &s_aPllClkTbl[BVDC_P_Pll_e216_000];
	}

	ulN = (pCurInfo->bPcInput && pCurInfo->stPcInSyncSettings.lClockAdj)
		? hVdec->ulPcManualClockNI : pPll->ulN;

	ulM = (pCurInfo->pPcInCfg->bPllAdj)
		? (pPll->ulM * (1 << pPll->ulByenPhi)) : pPll->ulM;

	/* PLL_DIV_CTL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PLL_DIV_CTL + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_3)
		BCHP_FIELD_ENUM(PLL_DIV_CTL, HCLK_REVERSE, RISING       ) |
		BCHP_FIELD_ENUM(PLL_DIV_CTL, HCLK_DIV2,    DIV_16       ) |
#endif
		BCHP_FIELD_DATA(PLL_DIV_CTL, VCO_RANGE, pPll->ulVcoRange) |
		BCHP_FIELD_DATA(PLL_DIV_CTL, NDIV_INT,  ulN             ) |
		BCHP_FIELD_DATA(PLL_DIV_CTL, MDIV,      ulM             ) |
		BCHP_FIELD_DATA(PLL_DIV_CTL, P2,        pPll->ulP2      ) |
		BCHP_FIELD_DATA(PLL_DIV_CTL, P1,        pPll->ulP1      );

	return;
}

/***************************************************************************
 *
 */
static void BVDC_P_Vdec_BuildPllRul_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t ulOffset = 0;
	uint32_t ulByenPhi;
	const BVDC_P_PllEntry *pPll = pCurInfo->pFdEntry->pPll;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	BDBG_ASSERT(pCurInfo->bUsePllClk);

	/*FD should work in 216M for ATSC input*/
	if(!pCurInfo->bPcInput)
	{
		pPll = &s_aPllClkTbl[BVDC_P_Pll_e216_000];
	}

	BDBG_MSG(("PC Pll = %s", pPll->pchFreq));
	BDBG_MSG(("PC N_DIV_MODE = %s", pPll->pNDivMode->pchNmode));

	ulByenPhi = (pCurInfo->pPcInCfg->bPllAdj)
		? (1) : pPll->ulByenPhi;

	/* PLL_RESET (RW) */
	/* Klude: Correct!  Need to hold DIGITAL & ANALOG for this piece of
	 * hardware to work! */
	/*reset pll analog to avoid unlock issue.*/
	/*PR40448:as two VDECs share PLL, reseting pll will affect another VDEC soruce*/
	if(pCurInfo->bPcInput)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PLL_RESET + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(PLL_RESET, DIGITAL, 1 ) |
			BCHP_FIELD_DATA(PLL_RESET, ANALOG,  0 );

		/* KLUDGE: Introduce wait states when reseting the PLL by writing
		 * to a dummy status regs that does nothing, but allow PLL reset
		 * to settle down.  */
		*pList->pulCurrent++ = BRDC_OP_REG_TO_REG( 10 ); /* reg copy */
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PLL_STATUS + ulOffset);
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PLL_STATUS + ulOffset);
	}

	/* PLL_RESET (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PLL_RESET + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PLL_RESET, DIGITAL, 1 ) |
		BCHP_FIELD_DATA(PLL_RESET, ANALOG,  1 );

	/* PLL_ENABLE (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PLL_ENABLE + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PLL_ENABLE, BYPASS_CLK,         0 ) |
		BCHP_FIELD_DATA(PLL_ENABLE, REFCOMP_POWER_DOWN, 0 ) |
		BCHP_FIELD_DATA(PLL_ENABLE, PLL_POWER_DOWN,     0 );

	/* PLL_DIV_CTL (RW) */
	BVDC_P_Vdec_BuildPllSyncRul_isr(hVdec, pList, pCurInfo);

	/* PLL_PLL_CTL_0 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PLL_PLL_CTL_0 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PLL_PLL_CTL_0, TESTA_SEL, 0 ) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_0, KVCO_XS,   pCurInfo->bPcInput ? 4 : 7    ) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_0, KVCO_XF,   0 ) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_0, LPF_BW,    0                     ) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_0, LF_ORDER,  pPll->pNDivMode->ulLfOrder    ) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_0, CN,        pPll->pNDivMode->ulCn         ) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_0, RN,        pPll->pNDivMode->ulRn         ) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_0, CP,        pPll->pNDivMode->ulCp         ) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_0, CZ,        pPll->pNDivMode->ulCz         ) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_0, RZ,        pPll->pNDivMode->ulRz         ) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_0, ICPX,
			(!pPll->ulVcoRange) ? pPll->pNDivMode->ulIcpx0 : pPll->pNDivMode->ulIcpx1   ) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_0, ICP_OFF,   0 );

	/* PLL_PLL_CTL_1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PLL_PLL_CTL_1 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PLL_PLL_CTL_1, I_DIVMUX_SEL,      pPll->ulDivMuxSel) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_1, I_EN_CMLBUF1,      0 ) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_1, I_DLY_CH1,         0 ) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_1, I_PWRDN_CH1,       0 ) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_1, BYEN_PHI,          ulByenPhi) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_1, BYPASS_SDMOD,     !pCurInfo->bPcInput) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_1, I_NDIV_MODE,       pPll->ulI) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_1, I_NDIV_PWRDN,      0 ) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_1, I_NDIV_DITHER_MFB, 0 ) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_1, I_TEST_SEL,        6 ) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_1, I_TEST_EN,         1 ) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_1, I_ENB_CLKOUT,      0 ) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_1, PTAP_ADJ,          0 ) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_1, CTAP_ADJ,          0 ) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_1, LOWCUR_EN,         0 ) |
		BCHP_FIELD_DATA(PLL_PLL_CTL_1, BIASIN_EN,         1 );

	/* PLL_STATUS (RO) */
	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_Vdec_BuildFeSyncRul_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t ulHFreq;
	uint32_t ulOffset;

	/* LUT informations about a given format. */
	const BFMT_VideoInfo *pFmtInfo = pCurInfo->pFmtInfo;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	ulOffset = hVdec->ulFeOffset;

	if(pCurInfo->bPcInput)
	{
		ulHFreq = ((hVdec->ulProgHFreq) *
			(BVDC_P_PC_CLOCK_ADJ_FACTOR + pCurInfo->stPcInSyncSettings.lClockAdj) +
			 BVDC_P_PC_CLOCK_ADJ_FACTOR / 2) / BVDC_P_PC_CLOCK_ADJ_FACTOR;
	}
	else
	{
		ulHFreq = hVdec->ulProgHFreq;
	}

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_SYNC + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC, AUTO_VSYNC_THRESHOLD, 0           ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC, VSYNC_MODE,           RAW         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC, HSYNC_MODE,           PLL         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC, VERTICAL_FREQ,
			pFmtInfo->ulScanHeight << !pFmtInfo->bInterlaced              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC, HORIZONTAL_FREQ,
			ulHFreq                                                       ) |
		((pCurInfo->pVdcFmt->bHd)
		? BCHP_FIELD_DATA(VDEC_FE_0_SYNC, FORMAT,             (!hVdec->bHdBilevelDetected))
		: BCHP_FIELD_ENUM(VDEC_FE_0_SYNC, FORMAT,             TWO_LEVEL  ));
	return;
}

/***************************************************************************
 *
 */

static void BVDC_P_Vdec_BuildFeFiledSyncRul_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	if(pCurInfo->bPcInput)/* toggle VDEC_FE_0_FIELD_SYNC.TYPE when manual clock */
	{
		uint32_t ulFieldSyncTypeOrMask = 0;
		uint32_t ulFieldSyncTypeAndMask = 0;

		ulFieldSyncTypeAndMask |=
			BCHP_MASK(VDEC_FE_0_FIELD_SYNC, TYPE);
		ulFieldSyncTypeOrMask  |=
			hVdec->bPcFieldSyncTypeToggle
			? BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TYPE, BY_MARKING)
			: BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TYPE, BY_POSITION);
		/* VDEC_FE_0_FIELD_SYNC (RW) */
		BVDC_P_RD_MOD_WR_RUL(pList->pulCurrent, ~ulFieldSyncTypeAndMask, ulFieldSyncTypeOrMask,
			BCHP_VDEC_FE_0_FIELD_SYNC + hVdec->ulFeOffset);
	}
	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_Vdec_BuildFeHVStartRul_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t ulHStart;
	uint32_t ulVStartF0, ulVStartF1;
	uint32_t ulVbiHStart;
	uint32_t ulOffset;

	/* LUT informations about a given format. */
	const BFMT_VideoInfo *pFmtInfo = pCurInfo->pFmtInfo;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	ulOffset = hVdec->ulFeOffset;

	/* PC input position adjustment */
	if(pCurInfo->bHVStartOverride)
	{
		ulHStart   = pCurInfo->ulHstart;
		ulVStartF0 = pCurInfo->ulVstart;
	}
	else /* restore the norminal values */
	{
		ulHStart   = pCurInfo->pFdEntry->ulHStart;
		ulVStartF0 = pFmtInfo->ulTopActive;

#if (BVDC_P_3DCOMB_TTD_CONFLICT)
		if(VIDEO_FORMAT_IS_625_LINES(pCurInfo->pFmtInfo->eVideoFmt))
		{
			ulVStartF0 -= 1;
		}
#endif
	}

	/*PR40449: Workaround for special 1080i/720p component streams with bi-level
	 * Hsync insteadof tri-level.*/
	if(hVdec->bHdBilevelDetected)
	{
		switch(pCurInfo->pFmtInfo->eVideoFmt)
		{
			case BFMT_VideoFmt_e1080i:
			case BFMT_VideoFmt_e1080i_50Hz:
				ulHStart = 0x152;
				break;
			case BFMT_VideoFmt_e1080p:
			case BFMT_VideoFmt_e1080p_50Hz:
				ulHStart = 0x152;
				break;
			case BFMT_VideoFmt_e720p:
			case BFMT_VideoFmt_e720p_50Hz:
				ulHStart = 0x1AD;
				break;
			default:
				break;
		}
	}

	/* PR23201: for teletext to work. */
	ulVbiHStart = ulHStart;
#if (BVDC_P_SUPPORT_VDEC_VER < BVDC_P_VDEC_VER_5)
	if(VIDEO_FORMAT_IS_PAL(pCurInfo->pFmtInfo->eVideoFmt))
	{
		ulVbiHStart = 0xE6;
	}
#endif

	/* PR41529: 1024x768i@43Hz can't be displayed correctly (interlaced) */
	ulVStartF1 = ulVStartF0;
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
	if((pCurInfo->bPcInput) && (pFmtInfo->bInterlaced))
	{
		ulVStartF1 += 2;
	}
#endif

	/* Override FE's with format specific settings */
	/* VDEC_FE_0_VSTART (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_VSTART + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_FE_0_VSTART, FIELD1, ulVStartF1 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_VSTART, FIELD0, ulVStartF0 );

	/* VDEC_FE_0_HSTART (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_HSTART + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART, AV,  ulHStart ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART, VBI, ulVbiHStart );

	return;
}

/***************************************************************************
 *
 */
static void BVDC_P_Vdec_BuildFeCscRul_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t ulOffset;

	BSTD_UNUSED(pCurInfo);
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	ulOffset = hVdec->ulFeOffset;

	/* VDEC_FE_0_FE_CSC_Y_MIN_MAX (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_FE_CSC_Y_MIN_MAX + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_Y_MIN_MAX, MAX, hVdec->ulCscMax ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_Y_MIN_MAX, MIN, hVdec->ulCscMin );

	/* VDEC_FE_0_FE_CSC_CR_MIN_MAX (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_FE_CSC_CR_MIN_MAX + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_CR_MIN_MAX, MAX, hVdec->ulCscMax ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_CR_MIN_MAX, MIN, hVdec->ulCscMin );

	/* VDEC_FE_0_FE_CSC_CB_MIN_MAX (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_FE_CSC_CB_MIN_MAX + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_CB_MIN_MAX, MAX, hVdec->ulCscMax ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_CB_MIN_MAX, MIN, hVdec->ulCscMin );

	/* VDEC_FE_0_FE_CSC_MATRIX_00_01 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_FE_CSC_MATRIX_00_01 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_MATRIX_00_01, COEFF_C0, hVdec->stFrontendCsc.usY0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_MATRIX_00_01, COEFF_C1, hVdec->stFrontendCsc.usY1 );

	/* VDEC_FE_0_FE_CSC_MATRIX_02_03 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_FE_CSC_MATRIX_02_03 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_MATRIX_02_03, COEFF_C2, hVdec->stFrontendCsc.usY2 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_MATRIX_02_03, COEFF_C3, hVdec->stFrontendCsc.usYOffset );

	/* VDEC_FE_0_FE_CSC_MATRIX_10_11 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_FE_CSC_MATRIX_10_11 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_MATRIX_10_11, COEFF_C0, hVdec->stFrontendCsc.usCr0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_MATRIX_10_11, COEFF_C1, hVdec->stFrontendCsc.usCr1 );

	/* VDEC_FE_0_FE_CSC_MATRIX_12_13 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_FE_CSC_MATRIX_12_13 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_MATRIX_12_13, COEFF_C2, hVdec->stFrontendCsc.usCr2 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_MATRIX_12_13, COEFF_C3, hVdec->stFrontendCsc.usCrOffset );

	/* VDEC_FE_0_FE_CSC_MATRIX_20_21 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_FE_CSC_MATRIX_20_21 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_MATRIX_20_21, COEFF_C0, hVdec->stFrontendCsc.usCb0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_MATRIX_20_21, COEFF_C1, hVdec->stFrontendCsc.usCb1 );

	/* VDEC_FE_0_FE_CSC_MATRIX_22_23 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_FE_CSC_MATRIX_22_23 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_MATRIX_22_23, COEFF_C2, hVdec->stFrontendCsc.usCb2 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_MATRIX_22_23, COEFF_C3, hVdec->stFrontendCsc.usCbOffset );

	return;
}

/***************************************************************************
 *
 */
static void BVDC_P_Vdec_BuildFeRul_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t i = 0, j=0;
	uint32_t ulAvBias = 0;
	uint32_t ulOrMask;
	uint32_t ulAndMask;
	uint32_t ulOffset;
	uint32_t ulMacroVisionStartLine, ulMacroVisionEndLine;
	uint32_t ulPseudoSyncStartLine, ulPseudoSyncEndLine, ulPseudoSyncThreshold;
	const BVDC_P_VdecFmtTbl_Entry *pFmtEntry;

	/* LUT informations about a given format. */
	const BFMT_VideoInfo *pFmtInfo = pCurInfo->pFmtInfo;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	ulOffset = hVdec->ulFeOffset;
	BDBG_OBJECT_ASSERT(hVdec->hSource, BVDC_SRC);

	/* Load FT table */
	BVDC_P_Vdec_GetFormatTable_isr(hVdec, pFmtInfo, pCurInfo->eInputType,
		&pFmtEntry);

	/* Full range 11-bit support */
	if(pCurInfo->bFullRange)
	{
		ulAvBias = (BVDC_MacrovisionType_eNoProtection == hVdec->eMacrovisionType)
			? 327 : 252;
	}

	/* Pseudosync settings */
	if(BFMT_PAL_HEIGHT == pCurInfo->pFmtInfo->ulHeight) /* 625 lines ED */
	{
		ulPseudoSyncThreshold = 21;
		ulPseudoSyncEndLine   = 14;
		ulPseudoSyncStartLine = 8;
	}
	else /* 525/ED lines and the rest */
	{
		ulPseudoSyncThreshold = 24;
		ulPseudoSyncEndLine   = 18;
		ulPseudoSyncStartLine = 10;
	}

	/* VDEC_FE_0_FE_CONFIG (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_FE_CONFIG + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_FE_CONFIG, STATUS_UPDATE,       VSYNC    ) |  /* nominal */
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CONFIG, BLANK_LEVEL,         hVdec->ulFeBlankLevel ) |  /* nominal */
		BCHP_FIELD_ENUM(VDEC_FE_0_FE_CONFIG, DCR_REFERENCE,       DCR_BLANK) |  /* nominal */
		BCHP_FIELD_ENUM(VDEC_FE_0_FE_CONFIG, AGC_PEDESTAL_REMOVE, ENABLE   ) |  /* nominal */
		((pCurInfo->bScartInput)
		? BCHP_FIELD_ENUM(VDEC_FE_0_FE_CONFIG, SCART_FORMAT,     SCART      )
		: BCHP_FIELD_ENUM(VDEC_FE_0_FE_CONFIG, SCART_FORMAT,     NON_SCART )) |
#else
		((pCurInfo->bIntIfd)
		? BCHP_FIELD_ENUM(VDEC_FE_0_FE_CONFIG, SOURCE,           IFD       ) /* int ifd = ifd, or adc */
		: BCHP_FIELD_ENUM(VDEC_FE_0_FE_CONFIG, SOURCE,           ADC       ))|
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CONFIG, DEBUG_SELECT,       1         ) |  /* nominal */
		((pCurInfo->stDGain.bMasterMode || !pCurInfo->bPcInput)
		? BCHP_FIELD_ENUM(VDEC_FE_0_FE_CONFIG, AGC_MODE,         MASTER )
		: BCHP_FIELD_ENUM(VDEC_FE_0_FE_CONFIG, AGC_MODE,         ALL    )) |
		(((pFmtInfo->bInterlaced) &&( !(pCurInfo->bPcInput)))/* always per_field for pc */
		? BCHP_FIELD_ENUM(VDEC_FE_0_FE_CONFIG, SAMPLE_MODE,      PER_2FIELDS)
		: BCHP_FIELD_ENUM(VDEC_FE_0_FE_CONFIG, SAMPLE_MODE,      PER_FIELD  )) |
		((pCurInfo->bIfdInput)
		? BCHP_FIELD_ENUM(VDEC_FE_0_FE_CONFIG, VIDEO_DETECT_CTL, ALWAYS_ON    ) /* how to lock */
		: BCHP_FIELD_ENUM(VDEC_FE_0_FE_CONFIG, VIDEO_DETECT_CTL, REQUIRE_HLOCK)) |
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CONFIG, BLACK_ON_CH1CH2,    0        ) |  /* nominal */
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CONFIG, BLACK_OFFSET,
			hVdec->ulBlackOffset                                          ) |  /* base on pedestal present */
		((pCurInfo->bPcInput || pCurInfo->bScartInput ||(pCurInfo->bFullRange && pCurInfo->bRgbInput))
		? BCHP_FIELD_DATA(VDEC_FE_0_FE_CONFIG, CH1CH2_ARE_SIGNED, 0       )
		: BCHP_FIELD_DATA(VDEC_FE_0_FE_CONFIG, CH1CH2_ARE_SIGNED, 1       )) |
		((pCurInfo->bPcInput)
		? BCHP_FIELD_ENUM(VDEC_FE_0_FE_CONFIG, SYNC_MODE,        ALL      )
		: BCHP_FIELD_ENUM(VDEC_FE_0_FE_CONFIG, SYNC_MODE,        MASTER   )) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FE_CONFIG, ENABLE,             ON        );   /* nominal */

#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
	/* VDEC_FE_0_FE_CONFIG1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_FE_CONFIG1 + ulOffset);
	*pList->pulCurrent++ =
		((hVdec->bFullSampleRate)
		? BCHP_FIELD_ENUM(VDEC_FE_0_FE_CONFIG1, SYNC_AREA_FULL_RATE, ENABLE )
		: BCHP_FIELD_ENUM(VDEC_FE_0_FE_CONFIG1, SYNC_AREA_FULL_RATE, DISABLE) ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CONFIG1, AV_BIAS, ulAvBias               ) |
		((hVdec->bFullSampleRate)
		? BCHP_FIELD_DATA(VDEC_FE_0_FE_CONFIG1, SAMPLE_RATE_FULL, (hVdec->ulSampleRate + 1) % 4)
		: BCHP_FIELD_ENUM(VDEC_FE_0_FE_CONFIG1, SAMPLE_RATE_FULL, DISABLE));
#endif

	/* VDEC_FE_0_VSTART (RW) (& HSTART) */
	BVDC_P_Vdec_BuildFeHVStartRul_isr(hVdec, pList, pCurInfo);

	/* NOTE: Take advantage of VDEC_FE_x_AWC_CHx_* is equi-distance apart,
	 * and the settings for Ch0, Ch1, and Ch2 are the same. */
	for(i = 0; i < BVDC_COLOR_CHANNELS; i++)
	{
		uint32_t ulAwcOffset = ulOffset;
		uint32_t ulDcrOffset = ulOffset;
		uint32_t ulDgnOffset = ulOffset;
		uint32_t ulDcrStart  = (hVdec->eMacrovisionType) ? 197 : 170;
		uint32_t ulHSyncCtrlDisable = (pCurInfo->bPcInput || pCurInfo->bCompInput) ? 1 : 0;

		if(BFMT_VideoFmt_e1080p_24Hz == pFmtInfo->eVideoFmt)
		{
			ulHSyncCtrlDisable = 0;
		}

		ulAwcOffset += i * (BCHP_VDEC_FE_0_AWC_CTRL_CH1 - BCHP_VDEC_FE_0_AWC_CTRL_CH0);
		ulDcrOffset += i * (BCHP_VDEC_FE_0_DCR_CONTROL_CH1 - BCHP_VDEC_FE_0_DCR_CONTROL_CH0);
		ulDgnOffset += i * (BCHP_VDEC_FE_0_DIGITAL_GAIN_CH1 - BCHP_VDEC_FE_0_DIGITAL_GAIN_CH0);

		/* PR29982: [AUDIO] correct DC_restore value for SIF input.  So that
		 * CH1 DCR still updates when video not present*/
		if((1 == i) && (pCurInfo->bIfdInput) && (!pCurInfo->bIntIfd))
		{
			ulHSyncCtrlDisable = 1;
		}

		/* VDEC_FE_0_AWC_CHx (CH1, CH2, CH0) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_AWC_THRESHOLD_CH0 + ulAwcOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VDEC_FE_0_AWC_THRESHOLD_CH0, EXCESS, 40960 ) |
			BCHP_FIELD_DATA(VDEC_FE_0_AWC_THRESHOLD_CH0, NORMAL, 512   );

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_AWC_LEVEL_CH0 + ulAwcOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VDEC_FE_0_AWC_LEVEL_CH0, EXCESS, 812 ) |
			BCHP_FIELD_DATA(VDEC_FE_0_AWC_LEVEL_CH0, NORMAL, 800 );

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_AWC_RATE_CH0 + ulAwcOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VDEC_FE_0_AWC_RATE_CH0, CLAMP,   9 ) |
			BCHP_FIELD_DATA(VDEC_FE_0_AWC_RATE_CH0, RECOVER, 9 );

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_AWC_CTRL_CH0 + ulAwcOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(VDEC_FE_0_AWC_CTRL_CH0, FILTER, LPF_0p6 ) |
			BCHP_FIELD_DATA(VDEC_FE_0_AWC_CTRL_CH0, ENABLE,
				pCurInfo->stAwc.abEnable[i] && !pCurInfo->bFullRange) |
			BCHP_FIELD_DATA(VDEC_FE_0_AWC_CTRL_CH0, END,
				(hVdec->eMacrovisionType) ? 122 : 127               ) |
			BCHP_FIELD_DATA(VDEC_FE_0_AWC_CTRL_CH0, START,
				hVdec->bVcrHeadSwitch ? 0x40 : pFmtInfo->ulTopActive);

		/* VDEC_FE_0_DCR_CONTROL_CHx (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_DCR_CONTROL_CH0 + ulDcrOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VDEC_FE_0_DCR_CONTROL_CH0, HSYNC_CTL_DISABLE,
				ulHSyncCtrlDisable                                                ) |
			BCHP_FIELD_DATA(VDEC_FE_0_DCR_CONTROL_CH0, ZERO_ERROR,        13      ) |
			BCHP_FIELD_DATA(VDEC_FE_0_DCR_CONTROL_CH0, ZERO_CHECK,        1       ) |
			BCHP_FIELD_DATA(VDEC_FE_0_DCR_CONTROL_CH0, ACCUMULATOR,
				pCurInfo->stDcr.aeAccumType[i]                                    ) |
			BCHP_FIELD_DATA(VDEC_FE_0_DCR_CONTROL_CH0, DIRECT,
				pCurInfo->stDcr.aeDirectType[i]                                   ) |
			BCHP_FIELD_DATA(VDEC_FE_0_DCR_CONTROL_CH0, REFERENCE_LEVEL,
				pCurInfo->stDcr.aulRef[i]                                         ) |
			BCHP_FIELD_DATA(VDEC_FE_0_DCR_CONTROL_CH0, START,
				pCurInfo->bPcInput ? pCurInfo->pPcInCfg->ulDcrCtrl : ulDcrStart   ) |
			BCHP_FIELD_ENUM(VDEC_FE_0_DCR_CONTROL_CH0, ENABLE,            ON      );

		/* VDEC_FE_0_DIGITAL_GAIN_CHx (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_DIGITAL_GAIN_CH0 + ulDgnOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VDEC_FE_0_DIGITAL_GAIN_CH0, MANUAL_GAIN,
				pCurInfo->stDGain.aulGain[i]                                    ) |
			BCHP_FIELD_DATA(VDEC_FE_0_DIGITAL_GAIN_CH0, REFERENCE_LEVEL,
				pCurInfo->stDGain.aulRef[i]                                     ) |
			BCHP_FIELD_DATA(VDEC_FE_0_DIGITAL_GAIN_CH0, TIME_CONSTANT,
				pCurInfo->stDGain.aulTimeConst[i]                               ) |
			((pCurInfo->bPcInput || pCurInfo->stDGain.bManualGain[i])
			? BCHP_FIELD_ENUM(VDEC_FE_0_DIGITAL_GAIN_CH0, MODE,          MANUAL)
			: BCHP_FIELD_ENUM(VDEC_FE_0_DIGITAL_GAIN_CH0, MODE,          AUTO  ));
	}

	/* TODO: Need to handle 4th channel (CH3) for SCART!  Currently just make
	 * the same as Ch0 */
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
	*pList->pulCurrent++ = BRDC_OP_REG_TO_REG( 1 );
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_AWC_CTRL_CH0 + hVdec->ulFeOffset);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_AWC_CTRL_CH3 + hVdec->ulFeOffset);

	*pList->pulCurrent++ = BRDC_OP_REG_TO_REG( 1 );
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_DCR_CONTROL_CH0 + hVdec->ulFeOffset);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_DCR_CONTROL_CH3 + hVdec->ulFeOffset);

	*pList->pulCurrent++ = BRDC_OP_REG_TO_REG( 1 );
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_DIGITAL_GAIN_CH0 + hVdec->ulFeOffset);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_DIGITAL_GAIN_CH3 + hVdec->ulFeOffset);
#endif

	/* VDEC_FE_0_ADC_ANALOG_GAIN (RW): Not connected to hw */
	/* VDEC_FE_0_EDSAFE_ANALOG_GAIN (RW): Not connected to hw */

	/* VDEC_FE_0_SYNC (RW) */
	BVDC_P_Vdec_BuildFeSyncRul_isr(hVdec, pList, pCurInfo);

	/* VDEC_FE_0_F0_BLANK_SEL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_F0_BLANK_SEL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_FE_0_F0_BLANK_SEL, USAGE, hVdec->ulBlankSelLines);

	/* VDEC_FE_0_F1_BLANK_SEL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_F1_BLANK_SEL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_FE_0_F1_BLANK_SEL, USAGE, hVdec->ulBlankSelLines);

	/* VDEC_FE_0_DCR_REGION (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_DCR_REGION + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_FE_0_DCR_REGION, DCR_START_WINDOW,        64     ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_DCR_REGION, DCR_START_WINDOW_ENABLE, ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_DCR_REGION, DISABLE_DCR_FRO, 0     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_DCR_REGION, DISABLE_MEDIAN,  0     ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_DCR_REGION, PWM_GAIN,        EIGHT ) |
		BCHP_FIELD_DATA(VDEC_FE_0_DCR_REGION, END,
			pCurInfo->stDcr.ulEnd                                    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_DCR_REGION, START,
			pCurInfo->stDcr.ulStart                                  );

	/*VDEC_FE_0_SLICE_MACRO_VISION (RW)*/
	if(BVDC_MacrovisionType_eNoProtection == hVdec->eMacrovisionType)
	{
		if((pCurInfo->bIfdInput) ||
		    (pCurInfo->bCvbsInput) ||
		    (pCurInfo->bSvidInput) ||
		    (pCurInfo->bCompInput && pCurInfo->pVdcFmt->bSd))
		{
			ulMacroVisionStartLine = 61; /*-3*/
			ulMacroVisionEndLine = 7;
		}
		else if(pCurInfo->bCompInput && pCurInfo->pVdcFmt->bEd)
		{
			ulMacroVisionStartLine = 1;
			ulMacroVisionEndLine = 7;
		}
		else
		{
			ulMacroVisionStartLine = 0;
			ulMacroVisionEndLine = 6;
		}
	}
	else
	{
		if((pCurInfo->bIfdInput || pCurInfo->bCvbsInput ||pCurInfo->bSvidInput ||pCurInfo->bCompInput) &&
		    (VIDEO_FORMAT_IS_525_LINES(pCurInfo->pFmtInfo->eVideoFmt)))
		{
			ulMacroVisionStartLine = 58; /*-6*/
			ulMacroVisionEndLine = 19;
		}
		else if((pCurInfo->bIfdInput || pCurInfo->bCvbsInput ||pCurInfo->bSvidInput ||pCurInfo->bCompInput) &&
		        (VIDEO_FORMAT_IS_625_LINES(pCurInfo->pFmtInfo->eVideoFmt)))
		{
			ulMacroVisionStartLine = 58; /*-6*/
			ulMacroVisionEndLine = 24;
		}
		else if(pCurInfo->bCompInput && pCurInfo->pVdcFmt->bEd)
		{
			ulMacroVisionStartLine = 1;
			ulMacroVisionEndLine = 15;
		}
		else
		{
			ulMacroVisionStartLine = 0;
			ulMacroVisionEndLine = 6;
		}
	}

	/* VDEC_FE_0_SLICE_MACRO_VISION */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_SLICE_MACRO_VISION + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_FE_0_SLICE_MACRO_VISION, F1_END_LINE,   ulMacroVisionEndLine) |
		BCHP_FIELD_DATA(VDEC_FE_0_SLICE_MACRO_VISION, F1_START_LINE, ulMacroVisionStartLine) |
		BCHP_FIELD_DATA(VDEC_FE_0_SLICE_MACRO_VISION, F0_END_LINE,   ulMacroVisionEndLine) |
		BCHP_FIELD_DATA(VDEC_FE_0_SLICE_MACRO_VISION, F0_START_LINE, ulMacroVisionStartLine);

	/* BCHP_VDEC_FE_0_VSYNC_PLL */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_VSYNC_PLL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_FE_0_VSYNC_PLL, HD_LOCK_STATUS_CTL,
			((!pCurInfo->bPcInput) &&
			 (BFMT_VideoFmt_e720p_50Hz == pFmtInfo->eVideoFmt ||
			  BFMT_VideoFmt_e720p == pFmtInfo->eVideoFmt ||
			  VIDEO_FORMAT_IS_ED(pFmtInfo->eVideoFmt))) ? 1 : 0   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_VSYNC_PLL, ONLY_NEG_ERR, 0      ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_VSYNC_PLL, LOCK_MODE,    QUICK  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_VSYNC_PLL, LOCK_THRESH,  3      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_VSYNC_PLL, LOCK_WINDOW,  15     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_VSYNC_PLL, ACC_AMP,      255    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_VSYNC_PLL, LPF_AMP,      47     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_VSYNC_PLL, FILTER,       30     );

	/* BCHP_VDEC_FE_0_SYNC_PSEUDO */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_SYNC_PSEUDO + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_PSEUDO, MATCH_FILTER_TH,   0) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_PSEUDO, MATCH_FILTER_MODE, 0) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_PSEUDO, THRESHHOLD, ulPseudoSyncThreshold) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_PSEUDO, END_LINE,   ulPseudoSyncEndLine  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_PSEUDO, START_LINE, ulPseudoSyncStartLine);

	/* NOTE: The bvdc_vdecfmt_priv.c has precedence!
	 * Add the entries in RUL.  Consider using blockwrite if don't have
	 * a lot of gap.  This is to easily add new formats or new vdec mode. */
	for(i=0; BVDC_P_ADDR_LAST != pFmtEntry[i].ulIndex; i++)
	{
		if(pFmtEntry[i].pFmtTbl)
		{
			for(j = 0; BVDC_P_ADDR_LAST !=pFmtEntry[i].pFmtTbl[j].ulAddr; j++)
			{
				*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
				*pList->pulCurrent++ = BRDC_REGISTER(pFmtEntry[i].pFmtTbl[j].ulAddr + ulOffset);
				*pList->pulCurrent++ = pFmtEntry[i].pFmtTbl[j].ulData;
			}
		}
	}

	BVDC_P_Vdec_BuildFeCscRul_isr(hVdec, pList, pCurInfo);

	/* BCHP_VDEC_FE_0_SCART_FB_CTL */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_SCART_FB_CTL + ulOffset);
	*pList->pulCurrent++ =
		((pCurInfo->bScartInput)
		? BCHP_FIELD_ENUM(VDEC_FE_0_SCART_FB_CTL, FB_PATH,          ENABLE  )
		: BCHP_FIELD_ENUM(VDEC_FE_0_SCART_FB_CTL, FB_PATH,          BYPASS )) |
		((pCurInfo->bScartInput)
		? BCHP_FIELD_ENUM(VDEC_FE_0_SCART_FB_CTL, SRC_CSC_PATH,     ENABLE  )
		: BCHP_FIELD_ENUM(VDEC_FE_0_SCART_FB_CTL, SRC_CSC_PATH,     BYPASS )) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SCART_FB_CTL, SRC,                ENABLE   ) |    /* nominal */
		BCHP_FIELD_DATA(VDEC_FE_0_SCART_FB_CTL, LPF_COEFF,          5        ) |    /* nominal */
		BCHP_FIELD_DATA(VDEC_FE_0_SCART_FB_CTL, POS_TH,             2        ) |    /* nominal */
		BCHP_FIELD_DATA(VDEC_FE_0_SCART_FB_CTL, LAST_AV,            1500 );         /* nominal */

	/* VDEC_FE_0_FE_CSC_DITHER_LFSR_INIT */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_FE_CSC_DITHER_LFSR_INIT + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_DITHER_LFSR_INIT, SEQ,   hVdec->stFEDither.ulLfsrSeq  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_DITHER_LFSR_INIT, VALUE, hVdec->stFEDither.ulLfsrValue);

	/* VDEC_FE_0_FE_CSC_DITHER_LFSR_CTRL */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_FE_CSC_DITHER_LFSR_CTRL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_DITHER_LFSR_CTRL, T0, hVdec->stFEDither.ulLfsrCtrlT0) |
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_DITHER_LFSR_CTRL, T1, hVdec->stFEDither.ulLfsrCtrlT1) |
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_DITHER_LFSR_CTRL, T2, hVdec->stFEDither.ulLfsrCtrlT2);

	/* VDEC_FE_0_FE_CSC_DITHER_CTRL */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_FE_CSC_DITHER_CTRL + ulOffset);
	*pList->pulCurrent++ =
		((hVdec->hSource->bTestFeature1)
		? BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_DITHER_CTRL, MODE,     0                           )
		: BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_DITHER_CTRL, MODE,     hVdec->stFEDither.ulMode    )) |
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_DITHER_CTRL, OFFSET_CH2, hVdec->stFEDither.ulCh2Offset) |
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_DITHER_CTRL, SCALE_CH2,  hVdec->stFEDither.ulCh2Scale ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_DITHER_CTRL, OFFSET_CH1, hVdec->stFEDither.ulCh1Offset) |
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_DITHER_CTRL, SCALE_CH1,  hVdec->stFEDither.ulCh1Scale ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_DITHER_CTRL, OFFSET_CH0, hVdec->stFEDither.ulCh0Offset) |
		BCHP_FIELD_DATA(VDEC_FE_0_FE_CSC_DITHER_CTRL, SCALE_CH0,  hVdec->stFEDither.ulCh0Scale );

	/* VDEC_FE_0_FE_BURST_WINDOW */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_FE_BURST_WINDOW + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_FE_0_FE_BURST_WINDOW, FIELD_END,       230       ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FE_BURST_WINDOW, FIELD_START,     24        ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FE_BURST_WINDOW, MODE,            FIELD      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FE_BURST_WINDOW, THRESHOLD_SCALE, 0         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FE_BURST_WINDOW, THRESHOLD_TYPE,  PERCENTAGE) |
		BCHP_FIELD_DATA(VDEC_FE_0_FE_BURST_WINDOW, THRESHOLD,       78        );

	if(hVdec->bHdBilevelDetected)
	{
		ulAndMask =
			BCHP_MASK(VDEC_FE_0_SYNC_DETECT, SLICE_DELAY);
		ulOrMask  =
			BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_DELAY, 7);

		/* VDEC_FE_0_SYNC_DETECT (RW) */
		BVDC_P_RD_MOD_WR_RUL(pList->pulCurrent, ~ulAndMask, ulOrMask,
			BCHP_VDEC_FE_0_SYNC_DETECT + ulOffset);
	}

	if(hVdec->bCompEnableSliceLPFHpllCtrl)
	{
		ulAndMask =
			BCHP_MASK(VDEC_FE_0_HPLL_MISC, SLICE_LPF_HPLL_LOCK_CTL);
		ulOrMask  =
			BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, SLICE_LPF_HPLL_LOCK_CTL, 0);

		/* VDEC_FE_0_HPLL_MISC (RW) */
		BVDC_P_RD_MOD_WR_RUL(pList->pulCurrent, ~ulAndMask, ulOrMask,
			BCHP_VDEC_FE_0_HPLL_MISC + ulOffset);
	}

	/* PR52041 : video shake for a customer internal crossh-hatch channel. */
	if((pCurInfo->bIfdInput) &&
	   (VIDEO_FORMAT_IS_SECAM(pCurInfo->pFmtInfo->eVideoFmt) ||
	    VIDEO_FORMAT_IS_PAL_BB1D1G(pCurInfo->pFmtInfo->eVideoFmt)))
	{
		ulAndMask =
			BCHP_MASK(VDEC_FE_0_NB_CONTROL, LINE);
		ulOrMask  =
			BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, LINE, 4);

		/* VDEC_FE_0_NB_CONTROL (RW) */
		BVDC_P_RD_MOD_WR_RUL(pList->pulCurrent, ~ulAndMask, ulOrMask,
			BCHP_VDEC_FE_0_NB_CONTROL + ulOffset);
	}

	/* PR54382: Every tuner change color is different on RF. */
	if(VIDEO_FORMAT_IS_NTSC(pCurInfo->pFmtInfo->eVideoFmt))
	{
		ulAndMask =
			BCHP_MASK(VDEC_FE_0_HSTART2, WINDOW_MODE);
		ulOrMask  =
			((hVdec->bHPllLocked)
			? BCHP_FIELD_ENUM(VDEC_FE_0_HSTART2, WINDOW_MODE, AUTO)
			: BCHP_FIELD_ENUM(VDEC_FE_0_HSTART2, WINDOW_MODE, MANUAL));

		/* VDEC_FE_0_NB_CONTROL (RW) */
		BVDC_P_RD_MOD_WR_RUL(pList->pulCurrent, ~ulAndMask, ulOrMask,
			BCHP_VDEC_FE_0_HSTART2 + ulOffset);
	}

	/* PR51338: support fullrange */
	if(pCurInfo->bFullRange)
	{
		ulAndMask =
			BCHP_MASK(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP           ) |
			BCHP_MASK(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC    ) |
			BCHP_MASK(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP);
		ulOrMask  =
			BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
			BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     0 ) |
			BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 0 );

		/* VDEC_FE_0_NB_CONTROL (RW) */
		BVDC_P_RD_MOD_WR_RUL(pList->pulCurrent, ~ulAndMask, ulOrMask,
			BCHP_VDEC_FE_0_AGC_REGION + ulOffset);
	}

	/* (0) SW3556-1191: forced into big mode. */
	if((pCurInfo->bIfdInput) &&
	   (hVdec->b240pMode)) /* rf progressive mode */
	{
		ulAndMask =
			BCHP_MASK(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE);
		ulOrMask  =
			BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, BIG);

		/* VDEC_FE_0_NB_CONTROL (RW) */
		BVDC_P_RD_MOD_WR_RUL(pList->pulCurrent, ~ulAndMask, ulOrMask,
			BCHP_VDEC_FE_0_HSYNC_PLL1 + ulOffset);
	}

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_Vdec_BuildBeCscRul_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint16_t usCbGain, usCrGain;
	uint32_t ulOffset;
	BVDC_P_CscCoeffs *pCsc;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	ulOffset = hVdec->ulBeOffset;
	pCsc = &hVdec->stCsc;

	if(pCurInfo->bUserCsc)
	{
		BVDC_P_Csc_FromMatrix_isr(pCsc,
			pCurInfo->pl32_Matrix, pCurInfo->ulUserShift);
	}
	else
	{
		BVDC_P_VdecColorSpace eInCS;
		BVDC_P_CmpColorSpace eOutCS = BVDC_P_CmpColorSpace_eNtscSdYCrCb;
		const BVDC_P_FormatInfo *pVdcFmt = pCurInfo->pVdcFmt;

		/* TODO: Fixed color space selection, and synchronized with CMP's CSC. */
		if((pCurInfo->bCompInput) || (pCurInfo->bPcInput))
		{
			if(pCurInfo->bRgbInput)
			{
				eInCS = (pVdcFmt->bHd)
					? BVDC_P_VdecColorSpace_eHdRgb : BVDC_P_VdecColorSpace_eSdRgb;
			}
			else
			{
				/* this is apparently not a bug, but a requested change to
				   always treat HDYPrPb input as SD */
				eInCS = (pVdcFmt->bHd)
					? BVDC_P_VdecColorSpace_eSdYPrPb : BVDC_P_VdecColorSpace_eSdYPrPb;
			}

			if(pCurInfo->bPcInput)
			{
				eOutCS = BVDC_P_VdecColorSpace_eHdYCrCb;
			}
		}
		else
		{
			eInCS = BVDC_P_VdecColorSpace_eYuv;
		}

		/* Extract the CSC for current configuration! */
		BVDC_P_Csc_GetVdecTable_isr(pCsc, eInCS, eOutCS, pCurInfo->pFmtInfo,
			pCurInfo->bFullRange);
	}

#if (BVDC_P_ACGC_CSC_GAIN)
	usCbGain = hVdec->bManualAcgc ? (uint16_t)hVdec->ulCbGain : pCsc->usCr2;
	usCrGain = hVdec->bManualAcgc ? (uint16_t)hVdec->ulCrGain : pCsc->usCb1;
#else
	usCbGain = pCsc->usCr2;
	usCrGain = pCsc->usCb1;
#endif

#if (BVDC_P_VDEC_PR29782_WORK_AROUND)
	{
		/* A workaround for this issue would be to add 15.75 (0xFC) for each
		 * negative coefficient (not including offset) in the CSC row. */
		uint32_t ulCnt;

		/* First Row */
		ulCnt = 0;
		ulCnt += BVDC_P_VDEC_COEFF_IS_NEGATIVE(pCsc->usY0) ? 1 : 0;
		ulCnt += BVDC_P_VDEC_COEFF_IS_NEGATIVE(pCsc->usY1) ? 1 : 0;
		ulCnt += BVDC_P_VDEC_COEFF_IS_NEGATIVE(pCsc->usY2) ? 1 : 0;
		pCsc->usYOffset += (ulCnt * BVDC_P_VDEC_COEFF_COMPENSATE);

		/* Second Row */
		ulCnt = 0;
		ulCnt += BVDC_P_VDEC_COEFF_IS_NEGATIVE(pCsc->usCb0) ? 1 : 0;
		ulCnt += BVDC_P_VDEC_COEFF_IS_NEGATIVE(pCsc->usCb1) ? 1 : 0;
		ulCnt += BVDC_P_VDEC_COEFF_IS_NEGATIVE(pCsc->usCb2) ? 1 : 0;
		pCsc->usCbOffset += (ulCnt * BVDC_P_VDEC_COEFF_COMPENSATE);

		/* Third  Row */
		ulCnt = 0;
		ulCnt += BVDC_P_VDEC_COEFF_IS_NEGATIVE(pCsc->usCr0) ? 1 : 0;
		ulCnt += BVDC_P_VDEC_COEFF_IS_NEGATIVE(pCsc->usCr1) ? 1 : 0;
		ulCnt += BVDC_P_VDEC_COEFF_IS_NEGATIVE(pCsc->usCr2) ? 1 : 0;
		pCsc->usCrOffset += (ulCnt * BVDC_P_VDEC_COEFF_COMPENSATE);
	}
#endif

	/* STREAKY_NOISE::DITHER adjust csc matrix. */
	if((pCurInfo->bPcInput) &&
	   (pCurInfo->pPcInCfg->bDitAdj))
	{
		pCsc->usYOffset  += 32;
		pCsc->usCbOffset += 32;
		pCsc->usCrOffset += 32;
	}

	/* VDEC_BE_0_VDEC_CSC_MATRIX_00_01 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_CSC_MATRIX_00_01 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_MATRIX_00_01, COEFF_C0, pCsc->usY0 ) |
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_MATRIX_00_01, COEFF_C1, pCsc->usY1 );

	/* VDEC_BE_0_VDEC_CSC_MATRIX_02_03 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_CSC_MATRIX_02_03 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_MATRIX_02_03, COEFF_C2, pCsc->usY2 ) |
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_MATRIX_02_03, COEFF_C3, pCsc->usYOffset);

	/* VDEC_BE_0_VDEC_CSC_MATRIX_10_11 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_CSC_MATRIX_10_11 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_MATRIX_10_11, COEFF_C0, pCsc->usCb0) |
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_MATRIX_10_11, COEFF_C1, usCrGain   );         /* change by manual acgc */

	/* VDEC_BE_0_VDEC_CSC_MATRIX_12_13 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_CSC_MATRIX_12_13 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_MATRIX_12_13, COEFF_C2, pCsc->usCb2     ) |
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_MATRIX_12_13, COEFF_C3, pCsc->usCbOffset);

	/* VDEC_BE_0_VDEC_CSC_MATRIX_20_21 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_CSC_MATRIX_20_21 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_MATRIX_20_21, COEFF_C0, pCsc->usCr0 ) |
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_MATRIX_20_21, COEFF_C1, pCsc->usCr1 );

	/* VDEC_BE_0_VDEC_CSC_MATRIX_22_23 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_CSC_MATRIX_22_23 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_MATRIX_22_23, COEFF_C2, usCbGain        ) |   /* change by manual acgc */
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_MATRIX_22_23, COEFF_C3, pCsc->usCrOffset);

	return;
}


/***************************************************************************
 * (1) Location of triggers/interrupt (bInputFormat)
 * (2) Auto triggers repeat (bInputFormat)
 * (3) Misc... (bAdcSetting, bInputFormat)
 *
 */
static void BVDC_P_Vdec_BuildBeRul_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t ulOffset;

	uint32_t ulVSize      = 0;
	uint32_t ulPicDelay   = 0;
	uint32_t ulAutoRepeat = 0;
	uint32_t ulTopActive  = 0;
	uint32_t ulAutoRepeatVsyncDelay = BVDC_P_AUTO_TRIGGER_VSYNC_DELAY;

	/* LUT informations about a given format. */
	const BFMT_VideoInfo *pFmtInfo = pCurInfo->pFmtInfo;
	const BVDC_P_FormatInfo *pVdcFmt = pCurInfo->pVdcFmt;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	ulOffset = hVdec->ulBeOffset;
	BDBG_OBJECT_ASSERT(hVdec->hSource, BVDC_SRC);

	ulTopActive = pFmtInfo->ulTopActive;
#if (BVDC_P_3DCOMB_TTD_CONFLICT)
	if(VIDEO_FORMAT_IS_625_LINES(pCurInfo->pFmtInfo->eVideoFmt))
	{
		ulTopActive -= 1;
	}
#endif

	/* Vertical size in line unit + vbi lines */
	ulVSize  = pFmtInfo->ulHeight >> pFmtInfo->bInterlaced;  /* field base cnt */

	/* New trigger mode.  How often to repeat the trigger in sysclk cycle unit.
	 * In the case vdec does not detect vsync. */
	ulAutoRepeat = (BVDC_P_216_SYSCLK / pFmtInfo->ulVertFreq) * BFMT_FREQ_FACTOR;
	ulPicDelay   = (ulAutoRepeat / pFmtInfo->ulScanHeight);  /* Single line delay */
	ulPicDelay  *= BVDC_P_TRIGGER_OFFSET(pFmtInfo->ulTopActive); /* X% of Vertical Blanking Area */

	/* Saved the SysClk per vsync to determined 1/1000 part difference. */
	hVdec->ulClkPerVsyncDelta = ulAutoRepeat * 1 / 10000;    /*   0.01% */

	/* VDEC_BE_0_BE_REV_ID (RO) */
	/* VDEC_BE_0_BE_RESET (WO) */

	/* VDEC_BE_0_BE_MODE (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_BE_MODE + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_6) /* additional bit for B2 */
		BCHP_FIELD_DATA(VDEC_BE_0_BE_MODE, SHORT_FIELD_FIX,  0           ) | /* 0 applies the fix */
#endif
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, VBI_DECIMATE, DISABLE         ) | /* TODO: Need revisit */
		((VIDEO_FORMAT_IS_PAL(pFmtInfo->eVideoFmt) || VIDEO_FORMAT_IS_SECAM(pFmtInfo->eVideoFmt))
		? BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, PAL_BLEND_SEL,  HALF   )
		: BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, PAL_BLEND_SEL,  DISABLE)    ) |
		((pCurInfo->bScartInput && (!hVdec->bOverSample))
#else
		((VIDEO_FORMAT_IS_PAL(pFmtInfo->eVideoFmt))
		? BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, PAL_BLEND_HALF, ENABLE )
		: BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, PAL_BLEND_HALF, DISABLE)    ) |
		((pCurInfo->bScartInput && (!hVdec->bOverSample))
#endif
		? BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, SCART_DECIMATE, ENABLE )
		: BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, SCART_DECIMATE, DISABLE)    ) |
		((pCurInfo->bScartInput)
		? BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, SCART_FB,       ENABLED )
		: BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, SCART_FB,       DISABLED)   ) |
		((pCurInfo->bPcInput)
		? BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, PC_ATSC,        PC  )
		: BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, PC_ATSC,        ATSC)       ) |
		((pCurInfo->bPcInput ||(pCurInfo->bFullRange && pCurInfo->bRgbInput))
		? BCHP_FIELD_DATA(VDEC_BE_0_BE_MODE, CH1CH2_ARE_SIGNED, 0)
		: BCHP_FIELD_DATA(VDEC_BE_0_BE_MODE, CH1CH2_ARE_SIGNED, 1)       ) |
		((pVdcFmt->bHd || hVdec->bOverSample || pCurInfo->bPcInput || pCurInfo->bScartInput)
		? BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, DECIMATE, D1_TO_1)
		: BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, DECIMATE, D2_TO_1)          ) |
#else
		BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, BANDLIMIT_FILTER,  NONE       ) |
		((pVdcFmt->bHd || hVdec->bOverSample || pCurInfo->bPcInput)
		? BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, DECIMATE_2_TO_1, DISABLE)
		: BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, DECIMATE_2_TO_1, ENABLE )   ) |
#endif
		BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, FORCE_ACCEPT,      NEVER      ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_BE_0_BE_MODE, CB_CHANNEL_SHIFT,
			pCurInfo->stDelay.ulCbDelay                                  ) |
		BCHP_FIELD_DATA(VDEC_BE_0_BE_MODE, CR_CHANNEL_SHIFT,
			pCurInfo->stDelay.ulCrDelay                                  ) |
		BCHP_FIELD_DATA(VDEC_BE_0_BE_MODE, Y_CHANNEL_SHIFT,
			pCurInfo->stDelay.ulYDelay                                   ) |
		BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, CR_CB_FIFO, ENABLE            ) | /* nominal */
		BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, VBI_PASSTHROUGH, DISABLE      ) |
		BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, VBI_FILTER, DISABLE           ) | /* nominal */
		BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, BVB_OUTPUT, ENABLE            ) | /* nominal */
		((hVdec->bPalSeparation && (VIDEO_FORMAT_IS_SECAM(pCurInfo->pFmtInfo->eVideoFmt)
		|| VIDEO_FORMAT_IS_PAL(pCurInfo->pFmtInfo->eVideoFmt)) && !pCurInfo->bCompInput && !pCurInfo->bPcInput)
		? BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, PAL_SEPARATION,  ENABLE )
		: BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, PAL_SEPARATION,  DISABLE)   ) |
		((pVdcFmt->bHd && !pCurInfo->bPcInput)
		? BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, SCALING_FRACTION, FR_16_11)
		: BCHP_FIELD_ENUM(VDEC_BE_0_BE_MODE, SCALING_FRACTION, FR_1_1  ) );

	/* VDEC_BE_0_DITHER (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_DITHER + ulOffset);
	*pList->pulCurrent++ =
		(pCurInfo->stBackend.bCoring)
		? BCHP_FIELD_ENUM(VDEC_BE_0_DITHER, CORING_ENABLE, ENABLED  )
		: BCHP_FIELD_ENUM(VDEC_BE_0_DITHER, CORING_ENABLE, DISABLED );

	/* VDEC_BE_0_VDEC_ACTIVE_CNT (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_ACTIVE_CNT + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_ACTIVE_CNT, V_COUNT,
			ulVSize                                                          ) |
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_ACTIVE_CNT, H_COUNT,
			pFmtInfo->ulWidth / ((hVdec->bOverSample || pCurInfo->bScartInput)
				? BVDC_P_VDEC_H_COUNT_FACTOR_2X : BVDC_P_VDEC_H_COUNT_FACTOR));

	/* VDEC_BE_0_VDEC_BE_TM (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_BE_TM + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_BE_TM, LINE_DELAY_1, 0 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_BE_TM, LINE_DELAY_0, 0 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_BE_TM, FIFO_B,       0 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_BE_TM, FIFO_A,       0 );  /* nominal */

#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
	/* VDEC_BE_0_VDEC_BE_BANDLIMIT (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_BE_BANDLIMIT + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_BE_BANDLIMIT, BLANK_LEVEL, hVdec->ulBeBlankLevel          ) |
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_BE_BANDLIMIT, K_STOP,      pCurInfo->stBackend.ulKStop    ) |
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_BE_BANDLIMIT, K_PASS,      pCurInfo->stBackend.ulKPass    ) |
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_BE_BANDLIMIT, FILTER,      pCurInfo->stBackend.eBandFilter);

	/* VDEC_BE_0_VDEC_BE_SCART (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_BE_SCART + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_BE_SCART, SHIFT,     1 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_BE_SCART, SLOPE,     6 );  /* nominal */

	/* VDEC_BE_0_VDEC_DERING (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_DERING + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_DERING, FILTER_TAPS, 23 ) |
		((pCurInfo->bPcInput)
		? BCHP_FIELD_DATA(VDEC_BE_0_VDEC_DERING, DERING_EN,  0 )
		: BCHP_FIELD_DATA(VDEC_BE_0_VDEC_DERING, DERING_EN,  1 ));

	/* VDEC_BE_0_BE_CSC_DITHER_LFSR_INIT (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_BE_CSC_DITHER_LFSR_INIT + ulOffset);
	*pList->pulCurrent++ =
	BCHP_FIELD_DATA(VDEC_BE_0_BE_CSC_DITHER_LFSR_INIT, SEQ,   hVdec->stBEDither.ulLfsrSeq  ) |
	BCHP_FIELD_DATA(VDEC_BE_0_BE_CSC_DITHER_LFSR_INIT, VALUE, hVdec->stBEDither.ulLfsrValue);

	/* VDEC_BE_0_BE_CSC_DITHER_LFSR_CTRL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_BE_CSC_DITHER_LFSR_CTRL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_BE_0_BE_CSC_DITHER_LFSR_CTRL, T0, hVdec->stBEDither.ulLfsrCtrlT0) |
		BCHP_FIELD_DATA(VDEC_BE_0_BE_CSC_DITHER_LFSR_CTRL, T1, hVdec->stBEDither.ulLfsrCtrlT1) |
		BCHP_FIELD_DATA(VDEC_BE_0_BE_CSC_DITHER_LFSR_CTRL, T2, hVdec->stBEDither.ulLfsrCtrlT2);

	/* VDEC_BE_0_BE_CSC_DITHER_CTRL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_BE_CSC_DITHER_CTRL + ulOffset);
		*pList->pulCurrent++ =
		((hVdec->hSource->bTestFeature1)
		? BCHP_FIELD_DATA(VDEC_BE_0_BE_CSC_DITHER_CTRL, MODE,     0                           )
		: BCHP_FIELD_DATA(VDEC_BE_0_BE_CSC_DITHER_CTRL, MODE,     hVdec->stBEDither.ulMode    )) |
		BCHP_FIELD_DATA(VDEC_BE_0_BE_CSC_DITHER_CTRL, OFFSET_CH2, hVdec->stBEDither.ulCh2Offset) |
		BCHP_FIELD_DATA(VDEC_BE_0_BE_CSC_DITHER_CTRL, SCALE_CH2,  hVdec->stBEDither.ulCh2Scale ) |
		BCHP_FIELD_DATA(VDEC_BE_0_BE_CSC_DITHER_CTRL, OFFSET_CH1, hVdec->stBEDither.ulCh1Offset) |
		BCHP_FIELD_DATA(VDEC_BE_0_BE_CSC_DITHER_CTRL, SCALE_CH1,  hVdec->stBEDither.ulCh1Scale ) |
		BCHP_FIELD_DATA(VDEC_BE_0_BE_CSC_DITHER_CTRL, OFFSET_CH0, hVdec->stBEDither.ulCh0Offset) |
		BCHP_FIELD_DATA(VDEC_BE_0_BE_CSC_DITHER_CTRL, SCALE_CH0,  hVdec->stBEDither.ulCh0Scale );
#endif

	/* VDEC_BE_0_VDEC_CSC_Y_MIN_MAX (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_CSC_Y_MIN_MAX + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_Y_MIN_MAX, MAX, hVdec->ulCscMax ) |
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_Y_MIN_MAX, MIN, hVdec->ulCscMin );

	/* VDEC_BE_0_VDEC_CSC_CR_MIN_MAX (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_CSC_CR_MIN_MAX + ulOffset);
	*pList->pulCurrent++ =
		( (hVdec->bResetHStart)&&(pCurInfo->bIfdInput)
		?BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_CR_MIN_MAX, MAX, 0x80200 )
		:BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_CR_MIN_MAX, MAX, hVdec->ulCscMax ))|
		( (hVdec->bResetHStart)&&(pCurInfo->bIfdInput)
		?BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_CR_MIN_MAX, MIN, 0x80200 )
		:BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_CR_MIN_MAX, MIN, hVdec->ulCscMin));

	/* VDEC_BE_0_VDEC_CSC_CB_MIN_MAX (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_CSC_CB_MIN_MAX + ulOffset);
	*pList->pulCurrent++ =
		( (hVdec->bResetHStart)&&(pCurInfo->bIfdInput)
		?BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_CB_MIN_MAX, MAX, 0x80200 )
		:BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_CB_MIN_MAX, MAX, hVdec->ulCscMax ))|
		( (hVdec->bResetHStart)&&(pCurInfo->bIfdInput)
		?BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_CB_MIN_MAX, MIN, 0x80200 )
		:BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_CB_MIN_MAX, MIN, hVdec->ulCscMin ));

	/* VDEC_BE_0_VDEC_CSC_VBI_MATRIX_00_01 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_CSC_VBI_MATRIX_00_01 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_VBI_MATRIX_00_01, COEFF_C1, 0 ) |
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_VBI_MATRIX_00_01, COEFF_C0, 0 );

	/* VDEC_BE_0_VDEC_CSC_VBI_MATRIX_02_03 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_CSC_VBI_MATRIX_02_03 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_VBI_MATRIX_02_03, COEFF_C3, 0 ) |
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_CSC_VBI_MATRIX_02_03, COEFF_C2, 0 );

	/* VDEC_BE_0_VDEC_INTERRUPT (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_INTERRUPT + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_INTERRUPT, FIELD_1, ulTopActive ) |
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_INTERRUPT, FIELD_0, ulTopActive );

	/* VDEC_BE_0_VDEC_TRIGGER (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_TRIGGER + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(VDEC_BE_0_VDEC_TRIGGER, SWAP_TRIGGERS,  DISABLED ) |
		BCHP_FIELD_ENUM(VDEC_BE_0_VDEC_TRIGGER, FIELD_1_ENABLE, ENABLED  ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_TRIGGER, FIELD_1, 0               ) | /* don't care */
		BCHP_FIELD_ENUM(VDEC_BE_0_VDEC_TRIGGER, FIELD_0_ENABLE, ENABLED  ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_TRIGGER, FIELD_0, 0               );  /* don't care */

	/* VDEC_BE_0_VDEC_VBI_VSTART (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_VBI_VSTART + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_VBI_VSTART, FIELD1, ulTopActive) |
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_VBI_VSTART, FIELD0, ulTopActive);

	/* VDEC_BE_0_VDEC_STATUS (RW), readonly */
	/* VDEC_BE_0_VDEC_HOST_LEGACY (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_HOST_LEGACY + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_HOST_LEGACY, USE_LEGACY_TRIGGER, 0 ); /* use new mode */

	if((pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_e1080p_24Hz) ||
	   (pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_e1080p_25Hz) ||
	   (pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_e1080p_30Hz))
	{
		ulAutoRepeatVsyncDelay = 2;
	}
	else
	{
		ulAutoRepeatVsyncDelay = BVDC_P_AUTO_TRIGGER_VSYNC_DELAY;
	}

	/* VDEC_BE_0_VDEC_AUTO_REPEAT (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_AUTO_REPEAT + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_AUTO_REPEAT, AUTO_REPEAT,
			ulAutoRepeat * ulAutoRepeatVsyncDelay);

	/* VDEC_BE_0_VDEC_PICTURE_DELAY (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_PICTURE_DELAY + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_PICTURE_DELAY, PICTURE_DELAY, ulPicDelay );

	/* VDEC_BE_0_VDEC_HOST_ENABLE (WO) */
	/* VDEC_BE_0_VDEC_HOST_CONTROL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_HOST_CONTROL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(VDEC_BE_0_VDEC_HOST_CONTROL, AUTO_REPEAT_ENABLE, ENABLE ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_HOST_CONTROL, HOST_DEBUG,         0      ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_HOST_CONTROL, HOST_DISABLE,       0      ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_BE_0_VDEC_HOST_CONTROL, CLEAR_ERRORS,       0      );  /* nominal */

	/* VDEC_BE_0_VDEC_COUNT_AT_FIRST (RO) */
	/* VDEC_BE_0_VDEC_BRIDGE_ERRORS_AUTO (RO) */
	/* VDEC_BE_0_VDEC_BRIDGE_ERRORS_RDB (RO) */

	return;
}


/***************************************************************************
 * (1) CVBS/IFD/SVIDEO only
 */
static void BVDC_P_Vdec_BuildAcgcRul_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	/* LUT informations about a given format. */
	const BVDC_P_CdCfgEntry *pCdCfg = pCurInfo->pCdCfg;
	const BFMT_VideoInfo *pFmtInfo = pCurInfo->pFmtInfo;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	/* VDEC_CD_0_COLOR (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_COLOR + hVdec->ulCdOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(VDEC_CD_0_COLOR, ACC_MIN_CLAMP,     ENABLE ) | /* nominal */
		BCHP_FIELD_ENUM(VDEC_CD_0_COLOR, ACC_MAX_CLAMP,     ENABLE ) | /* nominal */
		BCHP_FIELD_ENUM(VDEC_CD_0_COLOR, VIDEO_STATUS,      DISABLE                           ) | /* nominal */
		BCHP_FIELD_ENUM(VDEC_CD_0_COLOR, CHROMA_PLL_LOCK,   DISABLE                           ) | /* nominal */
		((hVdec->bManualAcgc)
		? BCHP_FIELD_ENUM(VDEC_CD_0_COLOR, MODE,            MANUAL)
		: BCHP_FIELD_ENUM(VDEC_CD_0_COLOR, MODE,            AUTO  ))|
		BCHP_FIELD_DATA(VDEC_CD_0_COLOR, MANUAL_COLOR_MODE, hVdec->bManualColorMode           ) |
		BCHP_FIELD_DATA(VDEC_CD_0_COLOR, AUTO_DETECT_OFF,   hVdec->ulAutoColorModeOffThreshold) |
		BCHP_FIELD_DATA(VDEC_CD_0_COLOR, AUTO_DETECT_ON,    hVdec->ulAutoColorModeOnThreshold);

	/* VDEC_CD_0_ACGC (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_ACGC + hVdec->ulCdOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(VDEC_CD_0_ACGC, PRECISION, HIGH            ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_CD_0_ACGC, REFERENCE, pCdCfg->ulAcgcRef) |
		BCHP_FIELD_DATA(VDEC_CD_0_ACGC, FILTER_GAIN,        26     ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_CD_0_ACGC, ACCUMULATOR_GAIN,   15     ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_CD_0_ACGC, ACCUMULATOR_ENABLE, 1      ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_CD_0_ACGC, THRESHOLD,
			(hVdec->bVcrHeadSwitch) ? 8 : 2                        ) |
		((hVdec->bManualAcgc)
		? BCHP_FIELD_ENUM(VDEC_CD_0_ACGC, TYPE,             MANUAL)
		: BCHP_FIELD_ENUM(VDEC_CD_0_ACGC, TYPE,             AUTO  ));  /* nominal */

	/* VDEC_CD_0_ACGC_1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_ACGC_1 + hVdec->ulCdOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_CD_0_ACGC_1, FIELD_AVERAGE_END,
			pCdCfg->ulAcgcFieldEnd)                      |
		BCHP_FIELD_DATA(VDEC_CD_0_ACGC_1, FIELD_AVERAGE_START,
			pCdCfg->ulAcgcFieldStart)                    |
		BCHP_FIELD_DATA(VDEC_CD_0_ACGC_1, MANUAL_GAIN,
			hVdec->ulColorBurstGain                                );

	/* VDEC_CD_0_SUBCARRIER_LOOP (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_SUBCARRIER_LOOP + hVdec->ulCdOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_CD_0_SUBCARRIER_LOOP, ACCUMULATOR_CLEAR,
			hVdec->bClearSubCarAccum                                             ) | /* nominal */
		BCHP_FIELD_ENUM(VDEC_CD_0_SUBCARRIER_LOOP, NOISE_ADJUST_SELECT, BW_525HZ ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_CD_0_SUBCARRIER_LOOP, WIDER_PULLIN_RANGE,  0        ) |
		BCHP_FIELD_DATA(VDEC_CD_0_SUBCARRIER_LOOP, FILTER_GAIN,         2        ) |
		BCHP_FIELD_DATA(VDEC_CD_0_SUBCARRIER_LOOP, ACCUMULATOR_GAIN,    3        ) |
		BCHP_FIELD_DATA(VDEC_CD_0_SUBCARRIER_LOOP, ACCUMULATOR_ENABLE,  1        ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_CD_0_SUBCARRIER_LOOP, THRESHOLD,
			hVdec->bVcrHeadSwitch ? 96 : 8                                       );

#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
	if(VIDEO_FORMAT_IS_SECAM(pFmtInfo->eVideoFmt))
	{
		/* VDEC_CD_0_SECAM_COLOR_KILL (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_SECAM_COLOR_KILL + hVdec->ulCdOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_COLOR_KILL, AUTO_DETECT_OFF, hVdec->ulAutoColorModeOffThreshold) |
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_COLOR_KILL, AUTO_DETECT_ON, hVdec->ulAutoColorModeOnThreshold ) |
			((hVdec->bManualAcgc)
			? BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_COLOR_KILL, COLOR_MODE, MANUAL)
			: BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_COLOR_KILL, COLOR_MODE, AUTO  )                       ) |
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_COLOR_KILL, MANUAL_COLOR_MODE, hVdec->bManualColorMode         );

		/* VDEC_CD_0_SECAM_COLOR (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_SECAM_COLOR + hVdec->ulCdOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_COLOR, RESET_ACCUM,        hVdec->bSecamColorResetAccum   ) |
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
			BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_COLOR, MODE,               MID_FIELD ) |
#else
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_COLOR, CSYNC_ACC_SAT,      hVdec->ulSecamColorCsyncSat    ) |
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_COLOR, CSYNC_THRESHOLD,    hVdec->ulSecamColorCsyncThresh ) |
#endif
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_COLOR, CHROMA_BURST_WIDTH, hVdec->ulSecamColorChromaWidth );

		/* VDEC_CD_0_SECAM_ACGC (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_SECAM_ACGC + hVdec->ulCdOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_ACGC, RESET_ACCUM, hVdec->bClearSubCarAccum   ) |
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_ACGC, REFERENCE,   pCdCfg->ulAcgcRef          ) |
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_ACGC, K_ACGC,      hVdec->ulSecamAcgcLoopGain ) |
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_ACGC, ACCUMULATOR_ENABLE, 1                   ) | /* nominal */
			((hVdec->bManualAcgc)
			? BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_ACGC, TYPE, MANUAL                          )
			: BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_ACGC, TYPE, AUTO                            ));

		/* VDEC_CD_0_SECAM_ACGC_1 (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_SECAM_ACGC_1 + hVdec->ulCdOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_ACGC_1, COLOR_LIMIT, hVdec->ulSecamColorLimit ) |
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_ACGC_1, MANUAL_GAIN, hVdec->ulColorBurstGain  );

		/* VDEC_CD_0_SECAM_ACGC_REGION (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_SECAM_ACGC_REGION + hVdec->ulCdOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_ACGC_REGION, ACC_END,   hVdec->ulSecamAccEnd   ) | /* nominal */
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_ACGC_REGION, ACC_START, hVdec->ulSecamAccStart );

		/* VDEC_CD_0_SECAM_ACTIVE SECAM (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_SECAM_ACTIVE + hVdec->ulCdOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_ACTIVE, END_LINE,   hVdec->ulSecamActiveEndLine   ) |
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_ACTIVE, START_LINE, hVdec->ulSecamActiveStartLine );
	}
#endif

	return;
}


/***************************************************************************
 * (1) CVBS/IFD/SVIDEO only
 */
static void BVDC_P_Vdec_BuildCdRul_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t ulOffset;
	uint32_t ulCdCtrl, ulCtiLumaBypass;

	/* LUT informations about a given format. */
	const BFMT_VideoInfo *pFmtInfo = pCurInfo->pFmtInfo;
	const BVDC_P_CdCfgEntry *pCdCfg = pCurInfo->pCdCfg;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	ulOffset = hVdec->ulCdOffset;

	/* VDEC_CD_0_CD_REV_ID (RO) */
	/* VDEC_CD_0_CD_RESET (WO) */

	/* +++KLUDGE+++: In order to update VDEC_CD_0_VAR_DELAY,
	 * VDEC_CD_0_CD_CTRL needs to be written 0
	 * first then write the real value. */
	/* VDEC_CD_0_CD_CTRL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_CD_CTRL + ulOffset);
	*pList->pulCurrent++ = 0;

	/* VDEC_CD_0_CD_CTRL (RW) */
	ulCdCtrl =
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_CD_0_CD_CTRL, STATUS_UPDATE,       VSYNC                       ) |
		BCHP_FIELD_DATA(VDEC_CD_0_CD_CTRL, ACTIVE_REGION_END,   pCdCfg->ulActiveRegionEnd   ) |
		((VIDEO_FORMAT_IS_SECAM(pFmtInfo->eVideoFmt))
		? BCHP_FIELD_ENUM(VDEC_CD_0_CD_CTRL, CHROMA_TYPE,       SECAM              )
		: BCHP_FIELD_ENUM(VDEC_CD_0_CD_CTRL, CHROMA_TYPE,       NTSC_PAL           )) |
#else
		BCHP_FIELD_ENUM(VDEC_CD_0_CD_CTRL, WINDOW_SW_SELECT,    C_WINDOW   ) | /* nominal */
		BCHP_FIELD_ENUM(VDEC_CD_0_CD_CTRL, WINDOW_MODE,         AUTO       ) | /* nominal */
#endif
		BCHP_FIELD_DATA(VDEC_CD_0_CD_CTRL, FIR2_IF_COMP_SELECT, pCurInfo->stCti.eFirFilter2 ) |
		BCHP_FIELD_DATA(VDEC_CD_0_CD_CTRL, IIR_IF_COMP_SELECT,  pCurInfo->stCti.eIirFilter1 ) |
		BCHP_FIELD_DATA(VDEC_CD_0_CD_CTRL, FIR1_IF_COMP_SELECT, pCurInfo->stCti.eFirFilter1 ) |
		BCHP_FIELD_DATA(VDEC_CD_0_CD_CTRL, IIR_IF_GRP_DELAY,    pCurInfo->stCti.eIirIfDelay ) |
		((!hVdec->bManualColorMode && (pCurInfo->bIfdInput || pCurInfo->bCvbsInput))
		?BCHP_FIELD_ENUM(VDEC_CD_0_CD_CTRL, USE_COMBED_Y,       DISABLE   )
		:BCHP_FIELD_ENUM(VDEC_CD_0_CD_CTRL, USE_COMBED_Y,       ENABLE    )                 ) | /* nominal */
		BCHP_FIELD_ENUM(VDEC_CD_0_CD_CTRL, COLORSYNC_INVERT,    DISABLE   )  | /* nominal */
		(((pCurInfo->bCvbsInput || pCurInfo->bSvidInput || pCurInfo->bIfdInput) && (!hVdec->bResetHStart))
		? BCHP_FIELD_ENUM(VDEC_CD_0_CD_CTRL, CD,                ENABLE )
		: BCHP_FIELD_ENUM(VDEC_CD_0_CD_CTRL, CD,                BYPASS )                    );

	/* VDEC_CD_0_MV_COLOR_STRIPE (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_MV_COLOR_STRIPE + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_CD_0_MV_COLOR_STRIPE, DETECT_END,       498 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_CD_0_MV_COLOR_STRIPE, DETECT_START,     8   ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_CD_0_MV_COLOR_STRIPE, DETECT_THRESHOLD,
			VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt) ? 20 : 15          ) |
		BCHP_FIELD_DATA(VDEC_CD_0_MV_COLOR_STRIPE, LINE_THRESHOLD,
			VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt) ? 50 : 12          );

	/* VDEC_CD_0_SUBCARRIER_FREQ (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_SUBCARRIER_FREQ + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_CD_0_SUBCARRIER_FREQ, STEP, pCdCfg->ulFreqStep);

	/* VDEC_CD_0_SUBCARRIER_LOOP_ACC (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_SUBCARRIER_LOOP_ACC + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_CD_0_SUBCARRIER_LOOP_ACC, MAX,
			hVdec->bVcrHeadSwitch ? 0x45e7 : 0x1ffff                          );

	/* VDEC_CD_0_LOOP_FILTER_ADJ (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_LOOP_FILTER_ADJ + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(VDEC_CD_0_LOOP_FILTER_ADJ, NOISE_ADJUST_SELECT,    BW_262HZ ) | /* nominal */
		BCHP_FIELD_ENUM(VDEC_CD_0_LOOP_FILTER_ADJ, ACGC_ACCUMULATOR_CLEAR, DISABLE  ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_CD_0_LOOP_FILTER_ADJ, END,                    16       ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_CD_0_LOOP_FILTER_ADJ, START,                  58       );  /* nominal */

	/* VDEC_CD_0_SUBCARRIER_PERIOD_COUNTER (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_SUBCARRIER_PERIOD_COUNTER + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_CD_0_SUBCARRIER_PERIOD_COUNTER, COUNTER,
			pCdCfg->ulPerCnt);

	/* VDEC_CD_0_CHROMA_TRANSIENT_IMPROVEMENT (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_CHROMA_TRANSIENT_IMPROVEMENT + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_CD_0_CHROMA_TRANSIENT_IMPROVEMENT, CTI,              pCurInfo->stCti.bEnable    ) |
		BCHP_FIELD_DATA(VDEC_CD_0_CHROMA_TRANSIENT_IMPROVEMENT, BANDLIMIT_FILTER, pCurInfo->stCti.eBandFilter) |
		BCHP_FIELD_DATA(VDEC_CD_0_CHROMA_TRANSIENT_IMPROVEMENT, V_CHANNEL_GAIN,   pCurInfo->stCti.ulVChnGain ) |
		BCHP_FIELD_DATA(VDEC_CD_0_CHROMA_TRANSIENT_IMPROVEMENT, U_CHANNEL_GAIN,   pCurInfo->stCti.ulUChnGain ) |
		BCHP_FIELD_DATA(VDEC_CD_0_CHROMA_TRANSIENT_IMPROVEMENT, FILTER_TAPS,      pCurInfo->stCti.eFilterTaps) |
		BCHP_FIELD_DATA(VDEC_CD_0_CHROMA_TRANSIENT_IMPROVEMENT, CLAMPING_WINDOW,  pCurInfo->stCti.ulClampWin );

	/* VDEC_CD_0_SUBCARRIER_PHASE_STATUS (RO) */
	/* VDEC_CD_0_COLOR_STRIPE_STATUS (RO) */
	/* VDEC_CD_0_CD_STATUS (RO) */
	/* VDEC_CD_0_CD_STATUS1 (RO) */
	/* VDEC_CD_0_CD_STATUS2 (RO) */
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
	if(VIDEO_FORMAT_IS_SECAM(pFmtInfo->eVideoFmt))
	{
		/* VDEC_CD_0_SECAM_CTL (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_SECAM_CTL + ulOffset);
		*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_CTL, SAMPLE_LINE,         20      ) |
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_CTL, LUMA_REJECT_ALPHA,   8       ) |
			BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_CTL, CHROMA_CLIP_ENHANCE, METHOD_0) |
			BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_CTL, CHROMA_CLIP,         DISABLE ) |
			BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_CTL, ONE_OVER_X_METHOD,   TAYLOR  ) |
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_6)
			((pCurInfo->bCvbsInput || pCurInfo->bIfdInput)
			? BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_CTL, CHROMA_REJECT,     ENABLE)
			: BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_CTL, CHROMA_REJECT,     DISABLE)) |
			((pCurInfo->bCvbsInput || pCurInfo->bIfdInput)
			? BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_CTL, LUMA_LOW_PASS,     ENABLE)
			: BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_CTL, LUMA_LOW_PASS,     DISABLE)) |
#else
			BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_CTL, CHROMA_REJECT,       ENABLE  ) |
			BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_CTL, LUMA_LOW_PASS,       ENABLE  ) |
#endif
#else
			((pCurInfo->bCvbsInput || pCurInfo->bIfdInput)
			? BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_CTL, CHROMA_REJECT,     ENABLE)
			: BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_CTL, CHROMA_REJECT,     DISABLE)) |
#endif
			BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_CTL, LUMA_REJECT,         ENABLE  ) |
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_CTL, HIGH_FREQ_LUMA_DELAY, 42     ) | /* nominal */
			((pCurInfo->bCvbsInput || pCurInfo->bIfdInput)
			? BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_CTL, SEPARATION,        ENABLE)
			: BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_CTL, SEPARATION,       DISABLE) ) |
			BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_CTL, COLORSYNC_INVERT,    ENABLE  ) |
			BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_CTL, COLORSYNC,           ENABLE  );

		/* VDEC_CD_0_SECAM_BLUE SECAM (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_SECAM_BLUE + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_BLUE, SCALE,            157     ) | /* nominal */
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_BLUE, FREQ,             4250000 ); /* nominal */

		/* VDEC_CD_0_SECAM_RED SECAM (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_SECAM_RED + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_RED, SCALE,                191      ) | /* nominal */
			BCHP_FIELD_DATA(VDEC_CD_0_SECAM_RED, FREQ,                 4406250  );  /* nominal */

		/* VDEC_CD_0_SECAM_CSYNC_STATUS (RO) */
		/* VDEC_CD_0_SECAM_ACGC_STATUS (RO) */
		/* VDEC_CD_0_SECAM_ACGC_STATUS1 (RO) */
		/* VDEC_CD_0_SECAM_ACGC_STATUS2  (RO) */
	}

#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
	/* VDEC_CD_0_SECAM_ADAPT (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_SECAM_ADAPT + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_ADAPT, ALPHA_MODE,   AUTO ) |
		BCHP_FIELD_DATA(VDEC_CD_0_SECAM_ADAPT, MANUAL_ALPHA, 0    ) |
		BCHP_FIELD_DATA(VDEC_CD_0_SECAM_ADAPT, GAIN,         12    ) |
		BCHP_FIELD_DATA(VDEC_CD_0_SECAM_ADAPT, CORE,         24   );

	/* VDEC_CD_0_SECAM_COLOR_DECAY (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_SECAM_COLOR_DECAY + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(VDEC_CD_0_SECAM_COLOR_DECAY, MODE,  ALWAYS_DECAY ) |
		BCHP_FIELD_DATA(VDEC_CD_0_SECAM_COLOR_DECAY, DECAY, 1950);

	/* VDEC_CD_0_SECAM_COLOR_THRESHOLD (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_SECAM_COLOR_THRESHOLD + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_CD_0_SECAM_COLOR_THRESHOLD, LOCK_SCALE,    3 ) |
		BCHP_FIELD_DATA(VDEC_CD_0_SECAM_COLOR_THRESHOLD, UNLOCK_SCALE,  3 ) |
		BCHP_FIELD_DATA(VDEC_CD_0_SECAM_COLOR_THRESHOLD, CSYNC_ACC_SAT,
			hVdec->bVcrHeadSwitch? 512 : 512                            ) |
		BCHP_FIELD_DATA(VDEC_CD_0_SECAM_COLOR_THRESHOLD, LOCK,
			hVdec->bVcrHeadSwitch? 150 : 150                            ) |
		BCHP_FIELD_DATA(VDEC_CD_0_SECAM_COLOR_THRESHOLD, UNLOCK,
			hVdec->bVcrHeadSwitch? 16 : 16                              );

	/* VDEC_CD_0_SECAM_CHROMA_CLIP (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_SECAM_CHROMA_CLIP + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_CD_0_SECAM_CHROMA_CLIP, DELTA_INDEX,      12  ) |
		BCHP_FIELD_DATA(VDEC_CD_0_SECAM_CHROMA_CLIP, DELTA_STEP,       90  ) |
		BCHP_FIELD_DATA(VDEC_CD_0_SECAM_CHROMA_CLIP, DELTA_DELTA_STEP, 10  ) |
		BCHP_FIELD_DATA(VDEC_CD_0_SECAM_CHROMA_CLIP, INIT_DELTA,       2 );

	/* VDEC_CD_0_SECAM_CHROMA_LEVEL506 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_SECAM_CHROMA_LEVEL506 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_CD_0_SECAM_CHROMA_LEVEL506, DELTA_506,          100  ) |
		BCHP_FIELD_DATA(VDEC_CD_0_SECAM_CHROMA_LEVEL506, LEVEL_506_NEGATIVE, 3196 ) |
		BCHP_FIELD_DATA(VDEC_CD_0_SECAM_CHROMA_LEVEL506, LEVEL_506_POSITIVE, 1111 );

	/* VDEC_CD_0_SECAM_CHROMA_LEVEL350 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_SECAM_CHROMA_LEVEL350 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_CD_0_SECAM_CHROMA_LEVEL350, DELTA_350,          20   ) |
		BCHP_FIELD_DATA(VDEC_CD_0_SECAM_CHROMA_LEVEL350, LEVEL_350_NEGATIVE, 3318 ) |
		BCHP_FIELD_DATA(VDEC_CD_0_SECAM_CHROMA_LEVEL350, LEVEL_350_POSITIVE, 640 );
#endif

	/* VDEC_CD_0_VAR_DELAY.CTI_LUMA_BYPASS (RW) */
	if((pCurInfo->bIfdInput) &&
	   (VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt)))
	{
		ulCtiLumaBypass = 68;
	}
	else if(VIDEO_FORMAT_IS_SECAM(pFmtInfo->eVideoFmt))
	{
		ulCtiLumaBypass = 64;
	}
	else
	{
		ulCtiLumaBypass = 69;
	}

	/* VDEC_CD_0_VAR_DELAY (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_VAR_DELAY + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		((VIDEO_FORMAT_IS_SECAM(pFmtInfo->eVideoFmt))
		? BCHP_FIELD_DATA(VDEC_CD_0_VAR_DELAY, CD_LUMA_BYPASS,        0 )
		: ((VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt) && (pCurInfo->bCvbsInput))
		? BCHP_FIELD_DATA(VDEC_CD_0_VAR_DELAY, CD_LUMA_BYPASS,        8 )
		: BCHP_FIELD_DATA(VDEC_CD_0_VAR_DELAY, CD_LUMA_BYPASS,        9 ))) |
		BCHP_FIELD_DATA(VDEC_CD_0_VAR_DELAY, CTI_LUMA_BYPASS,
			ulCtiLumaBypass                                               ) |
		BCHP_FIELD_DATA(VDEC_CD_0_VAR_DELAY, IF_COMP_LUMA_BYPASS,
			pCurInfo->stDelay.ulIfCompLumaBypass                          );
#else
		BCHP_FIELD_DATA(VDEC_CD_0_VAR_DELAY, CD_CTI_LUMA_BYPASS,      88  ) |
		BCHP_FIELD_DATA(VDEC_CD_0_VAR_DELAY, IF_COMP_LUMA_BYPASS,
			pCurInfo->stDelay.ulIfCompLumaBypass                          );
#endif /* BVDC_P_VDEC_VER_5 */
#endif /* BVDC_P_VDEC_VER_4 */

	/* ---KLUDGE---: In order to update this register it needs to be written 0
	 * first then write the real value. */
	/* VDEC_CD_0_CD_CTRL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_CD_CTRL + ulOffset);
	*pList->pulCurrent++ = ulCdCtrl;

	return;
}


/***************************************************************************
 * (1) CVBS/IFD only
 */
static void BVDC_P_Vdec_BuildYcRul_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t ulYcCtrl;
	uint32_t ulSyncLine;
	uint32_t ulOffset;

	/* 3D Comb frame information */
	uint32_t ulSamplePerFrame;  /* Clk sample per frame */

	/* LUT informations about a given format. */
	const BFMT_VideoInfo *pFmtInfo = pCurInfo->pFmtInfo;
	const BVDC_P_YcCfgEntry *pYcCfg = pCurInfo->pYcCfg;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	ulOffset = hVdec->ulYcOffset;

	/* Sample Per Frame!  HW should have this value already, it's adsurb to
	 * have sw provide this value, again.
	 *
	 * example NTSC: 27,000,000 / (60.00/1.001)* 2 =  900900;
	 * example PAL : 27,000,000 / (50.00/1.000)* 2 = 1080000;
	 * or
	 * H_Freq * V_Freq
	 *
	 * example NTSC: 1716 * 525 =  900900;
	 * example PAL : 1728 * 626 = 1080000;
	 */
	ulSamplePerFrame = hVdec->ulStdrHFreq * pFmtInfo->ulScanHeight;

	/*  */
	ulSyncLine = pYcCfg->ulSyncLine;
#if (BVDC_P_3DCOMB_TTD_CONFLICT)
	if(VIDEO_FORMAT_IS_625_LINES(pCurInfo->pFmtInfo->eVideoFmt))
	{
		ulSyncLine -= 1;
	}
#endif

	/*default setting for game is enable, but need to switch to stand mode when
	 * 3dcomb is disable. */
	ulYcCtrl =
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		((pYcCfg->ulDelayMode && hVdec->b3DComb)
		? BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, FRAME_COMB_DELAY_MODE,     GAME    )     /* Game mode 0 delay */
		: BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, FRAME_COMB_DELAY_MODE,     STANDARD) ) | /* normal mode */
		((pYcCfg->ulDecimation)
		? BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, DECIMATION_CTRL,           ENABLE )
		: BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, DECIMATION_CTRL,           DISABLE)  ) | /* normal mode */
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, COLORSYNC_CTRL,              ENABLE    ) | /* nominal */
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, COMB_2D_V_TIMING_EN,         ENABLE    ) |
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, CHROMA_ALIASING_3D,          DISABLE   ) | /*PR44389: keep consistent with 3563*/
		((pCurInfo->bSvidInput)
		? BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, COLORSYNC_INVERT,     ENABLE )
		: BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, COLORSYNC_INVERT,     DISABLE)       ) | /*different setting with 3563*/
#else
		BCHP_FIELD_DATA(VDEC_YC_0_YC_CTRL, NEG_PHASE,                   1         ) | /* PR23430: eliminate diagonal beat noise */
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, FRAME_ADJUST,                FIELD     ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_CTRL, PROGRESSIVE,                 0         ) |
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, COLORSYNC_INVERT,            ENABLE    ) | /* nominal */
#endif
		((hVdec->bOverSample)
		? BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, BANDLIMITING_FILTER,       DISABLE)      /* no need when oversmaple */
		: BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, BANDLIMITING_FILTER,       ENABLE )  ) | /* nominal */
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, CL_FILTER_SELECT,            NARROW    ) | /* nominal */
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, BG_CL_SELECT,                OFF       ) | /* nominal */
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, BG_CC_SELECT,                ON        ) | /* nominal */
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, BASIC_COMPLIMENTARY,         DISABLE   ) | /* nominal */
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, BG_CHROMA,                   DISABLE   ) |
		BCHP_FIELD_DATA(VDEC_YC_0_YC_CTRL, SUBCARRIER_LINE_PHASE_SHIFT, pYcCfg->ulPhaseShift) |
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, COMB_2D_H,                   ENABLE    ) | /* nominal */
		((hVdec->bFh3DComb)
		? BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, COMB_2D_V,                 ENABLE)
		: BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, COMB_2D_V,                 DISABLE)  ) |
#if BVDC_P_SECAM_AUTO_DETECION_SUPPORT
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, COMB_2D,                     ENABLE    ) |
#else
		((VIDEO_FORMAT_IS_SECAM(pFmtInfo->eVideoFmt))
		? BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, COMB_2D,                   DISABLE)
		: BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, COMB_2D,                   ENABLE)   ) |
#endif
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, FRAME_COMB,                  DISABLE   ) |
		((pCurInfo->bCvbsInput || pCurInfo->bIfdInput)
		? BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, YC,                        ENABLE)
		: BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, YC,                        BYPASS)   ) |
		(VIDEO_FORMAT_IS_525_LINES(pFmtInfo->eVideoFmt)
		? BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, TB_5_LINE,                 ENABLE )
		: BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, TB_5_LINE,                 DISABLE)  ) |
		BCHP_FIELD_DATA(VDEC_YC_0_YC_CTRL, LINE_DELAY, pYcCfg->ulLineDelay        ) |
		BCHP_FIELD_DATA(VDEC_YC_0_YC_CTRL, FILTER_COEF_SEL, pYcCfg->ulFilterCoef  );

	/* Need to disabled before updating the address */
	if(hVdec->b3DComb && pCurInfo->stDirty.stBits.b3DComb)
	{
		/* The 3D comb address allocated from applychanges. */
		uint32_t ul3DCombMemOffset = hVdec->apHeapNode[0]->ulDeviceOffset;
		uint32_t ul3DCombMemSize   = hVdec->apHeapNode[0]->pHeapInfo->ulBufSize *
			hVdec->ul3DCombBufCnt;

		/* VDEC_YC_0_YC_CTRL (RW) disable 3D comb first; it'll be enabled at end of this function! */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_CTRL + ulOffset);
		*pList->pulCurrent++ = ulYcCtrl;

#if (BVDC_P_RESET_YC_FOR_3DCOMB) /* PR43455 */
		/* minus 8 JWords in case 3D comb overwrites 1 burst beyond END address; */
		ul3DCombMemSize -= 256;

		/* Note: it may take 176 27MHz cycles (the last burst to complete)  for disable to take effect which
		             is equivalent to 176x4 = 704 RDC cycles in 108 MHz;
		             Note this is about 6.5 usec delay implemented by RDC register window copy!
		             Before 3D comb is really idle, we cannot reset YC!!! */
		*pList->pulCurrent++ = BRDC_OP_REG_TO_REG(704); /* reg copy 704 times */
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_CTRL + ulOffset);
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_CTRL + ulOffset);

		/* VDEC_YC_0_YC_RESET (RW) workaround: must reset when changing address!*/
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_RESET + ulOffset);
		*pList->pulCurrent++ = 0; /* write any value to reset; pulse instead of level reset */
#endif

		/* Make sure it meets hardware restrictions! */
		BDBG_ASSERT(BVDC_P_IS_ALIGN(ul3DCombMemSize,   32)); /* Jword aligned? */
		BDBG_ASSERT(BVDC_P_IS_ALIGN(ul3DCombMemOffset, 32)); /* Jword aligned? */

		/* VDEC_YC_0_YC_Memory_Start (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_MEMORY_START + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VDEC_YC_0_YC_MEMORY_START, ADDR, ul3DCombMemOffset );

		/* VDEC_YC_0_YC_Memory_End (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_MEMORY_END + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VDEC_YC_0_YC_MEMORY_END, ADDR, ul3DCombMemOffset + ul3DCombMemSize );
	}

	/* VDEC_YC_0_YC_REV_ID (RO) */
	/* VDEC_YC_0_YC_RESET (WO) */

	/* VDEC_YC_0_YC_HLE_BLEND (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_HLE_BLEND + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_HLE_BLEND, K3, 16 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_HLE_BLEND, K2, 24 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_HLE_BLEND, K1, 28 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_HLE_BLEND, K0, 32 );  /* nominal */

	/* VDEC_YC_0_YC_CROSS_LUMA_BLEND (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_CROSS_LUMA_BLEND + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_CROSS_LUMA_BLEND, K3, 2  ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_CROSS_LUMA_BLEND, K2, 4  ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_CROSS_LUMA_BLEND, K1, 8  ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_CROSS_LUMA_BLEND, K0, 16 );  /* nominal */

	/* VDEC_YC_0_YC_DIFF_TB_BLEND (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_DIFF_TB_BLEND + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_DIFF_TB_BLEND, K3, 4  ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_DIFF_TB_BLEND, K2, 8  ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_DIFF_TB_BLEND, K1, 16 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_DIFF_TB_BLEND, K0, 32 );  /* nominal */

	/* VDEC_YC_0_YC_H_BLEND_0_3 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_H_BLEND_0_3 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_H_BLEND_0_3, K3, 20 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_H_BLEND_0_3, K2, 24 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_H_BLEND_0_3, K1, 32 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_H_BLEND_0_3, K0, 64 );  /* nominal */

	/* VDEC_YC_0_YC_H_BLEND_4_7 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_H_BLEND_4_7 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_H_BLEND_4_7, K7, 64 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_H_BLEND_4_7, K6, 32 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_H_BLEND_4_7, K5, 24 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_H_BLEND_4_7, K4, 20 );  /* nominal */

	/* VDEC_YC_0_YC_V_BLEND_0_3 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_V_BLEND_0_3 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_V_BLEND_0_3, K3, 20 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_V_BLEND_0_3, K2, 24 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_V_BLEND_0_3, K1, 32 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_V_BLEND_0_3, K0, 64 );  /* nominal */

	/* VDEC_YC_0_YC_V_BLEND_4_7 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_V_BLEND_4_7 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_V_BLEND_4_7, K7, 64 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_V_BLEND_4_7, K6, 32 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_V_BLEND_4_7, K5, 24 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_V_BLEND_4_7, K4, 20 );  /* nominal */

	/* VDEC_YC_0_YC_SHARPENING_CHROMA_BLEND_0_2 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_SHARPENING_CHROMA_BLEND_0_2 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_SHARPENING_CHROMA_BLEND_0_2, K2, pCurInfo->stComb.aul3DChromaSharpness[2] ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_SHARPENING_CHROMA_BLEND_0_2, K1, pCurInfo->stComb.aul3DChromaSharpness[1] ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_SHARPENING_CHROMA_BLEND_0_2, K0, pCurInfo->stComb.aul3DChromaSharpness[0] );  /* nominal */

	/* VDEC_YC_0_YC_SHARPENING_CHROMA_BLEND_3_5 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_SHARPENING_CHROMA_BLEND_3_5 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_SHARPENING_CHROMA_BLEND_3_5, K5, pCurInfo->stComb.aul3DChromaSharpness[5] ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_SHARPENING_CHROMA_BLEND_3_5, K4, pCurInfo->stComb.aul3DChromaSharpness[4] ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_SHARPENING_CHROMA_BLEND_3_5, K3, pCurInfo->stComb.aul3DChromaSharpness[3] );  /* nominal */

	/* VDEC_YC_0_YC_SHARPENING_CHROMA_BLEND_6_7 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_SHARPENING_CHROMA_BLEND_6_7 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_SHARPENING_CHROMA_BLEND_6_7, K7, pCurInfo->stComb.aul3DChromaSharpness[7] ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_SHARPENING_CHROMA_BLEND_6_7, K6, pCurInfo->stComb.aul3DChromaSharpness[6] );  /* nominal */

	/* VDEC_YC_0_YC_SHARPENING_LUMA_BLEND_0_2 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_SHARPENING_LUMA_BLEND_0_2 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_SHARPENING_LUMA_BLEND_0_2, K2, 6 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_SHARPENING_LUMA_BLEND_0_2, K1, 4 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_SHARPENING_LUMA_BLEND_0_2, K0, 2 );  /* nominal */

	/* VDEC_YC_0_YC_SHARPENING_LUMA_BLEND_3_5 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_SHARPENING_LUMA_BLEND_3_5 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_SHARPENING_LUMA_BLEND_3_5, K5, 85  ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_SHARPENING_LUMA_BLEND_3_5, K4, 102 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_SHARPENING_LUMA_BLEND_3_5, K3, 8   );  /* nominal */

	/* VDEC_YC_0_YC_SHARPENING_LUMA_BLEND_6_7 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_SHARPENING_LUMA_BLEND_6_7 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_SHARPENING_LUMA_BLEND_6_7, K7, 64 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_SHARPENING_LUMA_BLEND_6_7, K6, 73 );  /* nominal */

	/* VDEC_YC_0_YC_2D_GAIN (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_2D_GAIN + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_2D_GAIN, SHARPENING_CHROMA,
			pCurInfo->stComb.ul2DChromaSharpnessGain               ) |
			((BFMT_VideoFmt_eNTSC_443 == pCurInfo->pFmtInfo->eVideoFmt)
			? BCHP_FIELD_DATA(VDEC_YC_0_YC_2D_GAIN, SHARPENING_NOTCH, 0)
			: BCHP_FIELD_DATA(VDEC_YC_0_YC_2D_GAIN, SHARPENING_NOTCH,
				hVdec->bCcrEnable ? 6 : 0                             )) |
			(VIDEO_FORMAT_IS_625_LINES(pCurInfo->pFmtInfo->eVideoFmt)
			? BCHP_FIELD_DATA(VDEC_YC_0_YC_2D_GAIN, V_COMPE_ERR,     3)
			: BCHP_FIELD_DATA(VDEC_YC_0_YC_2D_GAIN, V_COMPE_ERR,     0)) |
			(VIDEO_FORMAT_IS_625_LINES(pCurInfo->pFmtInfo->eVideoFmt)
			? BCHP_FIELD_DATA(VDEC_YC_0_YC_2D_GAIN, H_LUMA_ERR,      1)
			: BCHP_FIELD_DATA(VDEC_YC_0_YC_2D_GAIN, H_LUMA_ERR,      4)) |
			BCHP_FIELD_DATA(VDEC_YC_0_YC_2D_GAIN, BASIC_V_BLEND_ERR, 0 ) | /* nominal */
			BCHP_FIELD_DATA(VDEC_YC_0_YC_2D_GAIN, BASIC_H_ERR,       4 );  /* nominal */

	/* VDEC_YC_0_YC_2D_OFFSET (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_2D_OFFSET + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_2D_OFFSET, ENHANCED_H_BLEND, 10 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_2D_OFFSET, SHARPEN_ERR,      10 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_2D_OFFSET, H_LUMA_ERR,       10 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_2D_OFFSET, H_BLEND,          10 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_2D_OFFSET, V_BLEND,          10 );  /* nominal */

	/* VDEC_YC_0_YC_3D_BASIC_FRAME_ERR (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_3D_BASIC_FRAME_ERR + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_BASIC_FRAME_ERR, REDUCTION_COEF,
			pCurInfo->stComb.ulReductionCoef                                       ) |
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_BASIC_FRAME_ERR, IB_LUMA_DIFF_GAIN,
			pCurInfo->stComb.ulIBLumaDiffGain                                      ) |
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_BASIC_FRAME_ERR, OB_LUMA_DIFF_GAIN,
			pCurInfo->stComb.ulObLumaDiffGain                                      ) |
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_BASIC_FRAME_ERR, INCREASE_STEP,          2 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_BASIC_FRAME_ERR, INCREASE_BASE,          5 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_BASIC_FRAME_ERR, INCREASE_AMOUNT,        4 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_BASIC_FRAME_ERR, ROUGH_CHROMA_THRESHOLD, 9 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_BASIC_FRAME_ERR, TB_DIFF_THRESHOLD,
			pCurInfo->stComb.ulTbDiffThresold                                      );

	/* VDEC_YC_0_YC_3D_ADJ_ERR (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_3D_ADJ_ERR + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_ADJ_ERR, ROUGH_CHROMA_GAIN, 0 ) | /* nominal, when CHROMA_ALIASING_3D == DISABLE */
#endif
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_ADJ_ERR, GAIN,      3  ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_ADJ_ERR, THRESHOLD, 24 );  /* nominal */

	/* VDEC_YC_0_YC_3D_BLEND_0_3 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_3D_BLEND_0_3 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_BLEND_0_3, K3, pCurInfo->stComb.aul3DBlendConstant[3] ) |
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_BLEND_0_3, K2, pCurInfo->stComb.aul3DBlendConstant[2] ) |
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_BLEND_0_3, K1, pCurInfo->stComb.aul3DBlendConstant[1] ) |
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_BLEND_0_3, K0, pCurInfo->stComb.aul3DBlendConstant[0] );

	/* VDEC_YC_0_YC_3D_BLEND_4_7 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_3D_BLEND_4_7 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_BLEND_4_7, K7,  pCurInfo->stComb.aul3DBlendConstant[7] ) |
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_BLEND_4_7, K6,  pCurInfo->stComb.aul3DBlendConstant[6] ) |
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_BLEND_4_7, K5,  pCurInfo->stComb.aul3DBlendConstant[5] ) |
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_BLEND_4_7, K4,  pCurInfo->stComb.aul3DBlendConstant[4] );

	/* VDEC_YC_0_YC_3D_ADJUST_BLEND (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_3D_ADJUST_BLEND + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_ADJUST_BLEND, K3, 32 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_ADJUST_BLEND, K2, 36 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_ADJUST_BLEND, K1, 40 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_ADJUST_BLEND, K0, 48 );  /* nominal */

	/* VDEC_YC_0_YC_3D_OFFSET (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_3D_OFFSET + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_OFFSET, BLEND_COEF_ADJ,  10 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_3D_OFFSET, RAW_FRAME_BLEND, 10 );  /* nominal */

	/* VDEC_YC_0_YC_FRAME_STORE (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_FRAME_STORE + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_YC_0_YC_FRAME_STORE, FRAME1_MEMORY_BURST,     pYcCfg->ulMemBurstFrame1) |
		BCHP_FIELD_DATA(VDEC_YC_0_YC_FRAME_STORE, FRAME2_MEMORY_BURST,     pYcCfg->ulMemBurstFrame2);
#else
		BCHP_FIELD_DATA(VDEC_YC_0_YC_FRAME_STORE, MEMORY_BURST, pYcCfg->ulMemBurst);
#endif

#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
	/* VDEC_YC_0_YC_DELTA (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_DELTA + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_DELTA, CSYNC_ACCUMULATOR_MODE, ZERO       ) | /* nominal */
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_DELTA, VBI_END_SEL,            LINE_COUNT ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_DELTA, CSYNC_THRESHOLD,        10         ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_DELTA, CSYNC_ACC_SAT,          128        ) | /* nominal */
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_DELTA, CSYNC_ZEROCROSS,        POS        ) | /* nominal */
		((hVdec->bVcrHeadSwitch)
		? BCHP_FIELD_ENUM(VDEC_YC_0_YC_DELTA, CSYNC_MODE,           MID_FIELD)
		: BCHP_FIELD_ENUM(VDEC_YC_0_YC_DELTA, CSYNC_MODE,           FIELD)     ) |
#endif
		BCHP_FIELD_DATA(VDEC_YC_0_YC_DELTA, LAST_ACTIVE_LINE,
			pYcCfg->ulDeltaLastActiveLine                                  ) |
		BCHP_FIELD_DATA(VDEC_YC_0_YC_DELTA, LINE,
			hVdec->b3DComb ? pYcCfg->ulDeltaLine : 2                       );

	/* VDEC_YC_0_YC_FRAME_OFFSET (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_FRAME_OFFSET + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_FRAME_OFFSET, FRAME2_OFFSET,  pYcCfg->ulFrame2Offset   ) |
		BCHP_FIELD_DATA(VDEC_YC_0_YC_FRAME_OFFSET, FRAME1_OFFSET,  pYcCfg->ulFrame1Offset   );

	/* VDEC_YC_0_YC_COARSE_ALIGN_CTRL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_COARSE_ALIGN_CTRL + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_COARSE_ALIGN_CTRL, ENHANCED_TRACK_MODE,         ENABLE ) | /* nominal */
#endif
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_COARSE_ALIGN_CTRL, FRAME_COMB_TIMING_EN_MEMORY, ENABLE ) | /* nominal */
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_COARSE_ALIGN_CTRL, FRAME_COMB_TIMING_EN_COARSE, ENABLE ) | /* nominal */
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_COARSE_ALIGN_CTRL, FRAME_COMB_TIMING_EN_FINE,   ENABLE ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_COARSE_ALIGN_CTRL, GAME_MODE_FIXED_DLY,         4      ) | /* nominal */
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_COARSE_ALIGN_CTRL, VSYNC_MODE,                  ENABLE ) | /* nominal */
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_COARSE_ALIGN_CTRL, HSYNC_MODE,                  ENABLE );  /* nominal */

	/* VDEC_YC_0_YC_HSYNC_COARSE_ALIGN (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_HSYNC_COARSE_ALIGN + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_HSYNC_COARSE_ALIGN , DECISION,                  512    ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_HSYNC_COARSE_ALIGN , THRESHOLD,                 100    ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_HSYNC_COARSE_ALIGN , WINDOW,                    2      );  /* nominal */

	/* VDEC_YC_0_YC_VSYNC_COARSE_ALIGN (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_VSYNC_COARSE_ALIGN + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_VSYNC_COARSE_ALIGN, DECISION,                   512    ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_VSYNC_COARSE_ALIGN, THRESHOLD,                  2      ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_VSYNC_COARSE_ALIGN, WINDOW,                     8      );  /* nominal */

	/* VDEC_YC_0_YC_FINE_ALIGN (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_FINE_ALIGN + ulOffset);
	*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_FIELD_DATA(VDEC_YC_0_YC_FINE_ALIGN, VBI_END,             12     ) | /* nominal */
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_FINE_ALIGN, AMP_VALID_CHECK_EN,  ENABLE ) | /* nominal */
#endif
		BCHP_FIELD_DATA(VDEC_YC_0_YC_FINE_ALIGN, THRESHOLD,           30     ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_FINE_ALIGN, AMP_THRESHOLD,       500    );  /* nominal */

	/* VDEC_YC_0_YC_INPUT_GAIN1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_INPUT_GAIN1 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_INPUT_GAIN1, NOISE_LEVEL4_GAIN,                   51  ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_INPUT_GAIN1, NOISE_LEVEL3_GAIN,                   64  ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_INPUT_GAIN1, NOISE_LEVEL2_GAIN,                   77  ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_INPUT_GAIN1, NOISE_LEVEL1_GAIN,                   128 );  /* nominal */

	/* VDEC_YC_0_YC_INPUT_GAIN2 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_INPUT_GAIN2 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_INPUT_GAIN2, INPUT_GAIN,                      ENABLE  ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_INPUT_GAIN2, NOISE_LEVEL5_GAIN,                    51 );  /* nominal */

	/* VDEC_YC_0_YC_FINE_SYNC_LOOP_FILTER_GAINS (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_FINE_SYNC_LOOP_FILTER_GAINS + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_FINE_SYNC_LOOP_FILTER_GAINS, LINE_K_ACC_GAIN,
			hVdec->ulLineKAccGain                                                          ) |
		BCHP_FIELD_DATA(VDEC_YC_0_YC_FINE_SYNC_LOOP_FILTER_GAINS, LINE_K_IIR_GAIN,        5) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_FINE_SYNC_LOOP_FILTER_GAINS, LINE_K_IIR_3DB_FREQ,    2) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_FINE_SYNC_LOOP_FILTER_GAINS, FRAME_K_ACC_GAIN,       1) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_FINE_SYNC_LOOP_FILTER_GAINS, FRAME_K_IIR_GAIN,       5) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_FINE_SYNC_LOOP_FILTER_GAINS, FRAME_K_IIR_3DB_FREQ,   2); /* nominal */
#endif

	/* VDEC_YC_0_YC_FRAME_SIZE (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_FRAME_SIZE + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_FRAME_SIZE, SAMPLES, ulSamplePerFrame );

	/* VDEC_YC_0_YC_LOOP_ADJUST (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_LOOP_ADJUST + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(VDEC_YC_0_YC_LOOP_ADJUST, FRAME_FILTER_MODE, AUTO ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_LOOP_ADJUST, FRAME_SYNC_LINE, ulSyncLine ) |
		BCHP_FIELD_DATA(VDEC_YC_0_YC_LOOP_ADJUST, END,             10     ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_LOOP_ADJUST, START,           58     );  /* nominal */

	/* VDEC_YC_0_YC_CROSS_CHROMA (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_CROSS_CHROMA + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_CROSS_CHROMA, CHROMA_3D_ERR_OFFSET, 4  ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_CROSS_CHROMA, CLAMPING_GAIN,        0  ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_CROSS_CHROMA, BG_CHROMA_OFFSET,     0  ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_CROSS_CHROMA, BG_CHROMA_GAIN,       4   ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_CROSS_CHROMA, H_VAR_OFFSET,
			VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt) ? 15 : 0              ) |
		BCHP_FIELD_DATA(VDEC_YC_0_YC_CROSS_CHROMA, H_VAR_GAIN,
			VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt) ? 4 : 0               );

	/* VDEC_YC_0_YC_MBIST_CONTROL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_MBIST_CONTROL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_MBIST_CONTROL, LINE_DELAY_3_TMB, 0 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_MBIST_CONTROL, LINE_DELAY_3_TMA, 0 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_MBIST_CONTROL, LINE_DELAY_2_TMB, 0 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_MBIST_CONTROL, LINE_DELAY_2_TMA, 0 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_MBIST_CONTROL, LINE_DELAY_1_TMB, 0 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_MBIST_CONTROL, LINE_DELAY_1_TMA, 0 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_MBIST_CONTROL, LINE_DELAY_0_TMB, 0 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_MBIST_CONTROL, LINE_DELAY_0_TMA, 0 );  /* nominal */

	/* VDEC_YC_0_YC_MBIST_CONTROL_1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_MBIST_CONTROL_1 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VDEC_YC_0_YC_MBIST_CONTROL_1, BOT_MATCH_TMB,      0 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_MBIST_CONTROL_1, BOT_MATCH_TMA,      0 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_MBIST_CONTROL_1, TOP_FRAME_FIFO_TMB, 0 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_MBIST_CONTROL_1, TOP_FRAME_FIFO_TMA, 0 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_MBIST_CONTROL_1, BOT_FRAME_FIFO_TMB, 0 ) | /* nominal */
		BCHP_FIELD_DATA(VDEC_YC_0_YC_MBIST_CONTROL_1, BOT_FRAME_FIFO_TMA, 0 );  /* nominal */

	ulYcCtrl |=
		((hVdec->b3DComb)
		? BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, FRAME_COMB, ENABLE )
		: BCHP_FIELD_ENUM(VDEC_YC_0_YC_CTRL, FRAME_COMB, DISABLE));

	/* VDEC_YC_0_YC_CTRL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_YC_0_YC_CTRL + ulOffset);
	*pList->pulCurrent++ = ulYcCtrl;

	/* VDEC_YC_0_YC_Status (RO) */

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_Vdec_BuildAdcRul_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t ulChn;
	uint32_t ulAGain = 0;
	uint32_t ulOffset;
	uint32_t ulDcRestore;
	uint32_t ulAdcClamp;
	uint32_t ulAdcExtBypass;
	const BVDC_P_AdcEntry *pAdc;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	/* Y/Luma/Cvbs, Pr/Chroma, Pb */
	for(ulChn = 0; ulChn < BVDC_P_VDEC_CHANNELS; ulChn++)
	{
		/* It's possbile that configuration use less ADC or none! */
		if(!pCurInfo->apAdc[ulChn])
		{
			continue;
		}

		pAdc = pCurInfo->apAdc[ulChn];
		ulOffset = pAdc->ulOffset;
		ulAGain = (ulChn < BVDC_COLOR_CHANNELS) /* PR32506: Need same gain adjust for calib chn! */
			? pCurInfo->stAGain.aulGain[ulChn] : ulAGain;

		/* DC restore!  The same as the input to VDEC's channel. */
		if((!ulOffset) && (pCurInfo->bPcInput))
		{
			ulDcRestore = BCHP_ADC_1_SELECT_DC_ZERO;
			/* STREAKY_NOISE::ADC: ADC1 clamp and extbypass are comment settings
			 * for pcinput.*/
			ulAdcClamp     = 0;
			ulAdcExtBypass = 0;
		}
		else
		{
			ulDcRestore = ulChn + BCHP_ADC_1_SELECT_DC_PRI_CH0;
			ulAdcClamp     = 1;
			ulAdcExtBypass = 1;
		}

		/* User wish to bypass the internal common mode */
		if(pCurInfo->stVdecSettings.bBypassCommonMode)
		{
			ulAdcExtBypass = 0;
		}

		/* PR53137: ADC conflict when doing PIP/PBP between PC/Component and
		 * 5th ADC is use for SIF(audio).*/
		if(ulOffset == hVdec->stSifAdc.ulOffset)
		{
			ulAdcClamp     = 0;
			ulAdcExtBypass = 0;
		}

		/* ADC_1_ENABLES (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_ADC_1_ENABLES + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(ADC_1_ENABLES, INV_CLKOUT, 0) |
			BCHP_FIELD_DATA(ADC_1_ENABLES, TWO_OFF_B , 0) |
			BCHP_FIELD_DATA(ADC_1_ENABLES, EXT_BYPASS, ulAdcExtBypass) |
			BCHP_FIELD_DATA(ADC_1_ENABLES, CLAMP,      ulAdcClamp    ) |
			BCHP_FIELD_DATA(ADC_1_ENABLES, POWER_DOWN, 0);

		/* ADC_1_GAIN (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_ADC_1_GAIN + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(ADC_1_GAIN, PGA, ulAGain);

		/* ADC_1_CLOCK (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_ADC_1_CLOCK + ulOffset);
		*pList->pulCurrent++ =
			((pCurInfo->bPcInput || pCurInfo->bUsePllClk)
			? BCHP_FIELD_ENUM(ADC_1_CLOCK, SELECT, CLK_PC  )   /* pll clk */
			: BCHP_FIELD_ENUM(ADC_1_CLOCK, SELECT, CLK_108 )); /* sys clk */

		/* ADC_1_SELECT (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_ADC_1_SELECT + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(ADC_1_SELECT, DC,           ulDcRestore      ) |
			BCHP_FIELD_DATA(ADC_1_SELECT, ANALOG_INPUT, pAdc->ulInputSel );

		/* Only ADC_1 has calibration capability */
		if(!pAdc->ulOffset)
		{
			uint32_t ulRedChn   = 0;
			uint32_t ulGreenChn = 0;
			uint32_t ulBlueChn  = 0;

			/* Not used for ADC 1.  ADC_1_CALIBRATION.RED_CHANNEL is used to
			 * select the analog input select value in the non-cycling case. */
			if(pCurInfo->bPcInput)
			{
				ulRedChn   = pCurInfo->apAdc[0]->ulCalibSel; /* PC_R/Y/Luma/Cvbs/Ifd */
				ulGreenChn = pCurInfo->apAdc[1]->ulCalibSel; /* PC_G/Pr/Chroma */
				ulBlueChn  = pCurInfo->apAdc[2]->ulCalibSel; /* PC_B/Pb */
			}
			else
			{
				ulRedChn = pAdc->ulInputSel;
			}

			/* ADC_1_CALIBRATION (RW) */
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_ADC_1_CALIBRATION);
			*pList->pulCurrent++ =
				BCHP_FIELD_DATA(ADC_1_CALIBRATION, BLUE_CHANNEL,  ulBlueChn ) |
				BCHP_FIELD_DATA(ADC_1_CALIBRATION, GREEN_CHANNEL, ulGreenChn) |
				BCHP_FIELD_DATA(ADC_1_CALIBRATION, RED_CHANNEL,   ulRedChn  ) |
				((pCurInfo->bPcInput)
				? BCHP_FIELD_ENUM(ADC_1_CALIBRATION, CYCLE,       ENABLE )
				: BCHP_FIELD_ENUM(ADC_1_CALIBRATION, CYCLE,       DISABLE)); /* nominal */
			/* ADC_1_STATUS (RO) */
		}
	}

	/* PR31679: Reduce crosstalks to avoid overflow of data into others
	 * channels.  Need to update In off! */
	{
		uint32_t i;
		uint32_t ulAdcTurnOn;  /* In on/off input */

		/* PR31679, PR46514: Shut off unused adc for power saving as well. */
		for(i = 0; i < BVDC_P_MAX_PHYSICAL_ADC; i++)
		{
			if(!pCurInfo->apUsedAdc[i])
			{
				bool bPowerDown = true;
				ulOffset = i * (BCHP_ADC_2_REG_START - BCHP_ADC_1_REG_START);

				/* SW3556-804: SCART's function select needed ADC_4 (4th) to
				 * to be on in ordered for it to detect.  Just the way HW is
				 * hardwired */
				if(3 == i)
				{
					bPowerDown = false;
				}

				/* ADC_x_ENABLES (RW) */
				*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
				*pList->pulCurrent++ = BRDC_REGISTER(BCHP_ADC_1_ENABLES + ulOffset);
				*pList->pulCurrent++ = BCHP_FIELD_DATA(ADC_1_ENABLES, POWER_DOWN, bPowerDown);
			}
		}

		/* ADC_1_CONTROL (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_ADC_1_CONTROL);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(ADC_1_CONTROL, PD_D2S,
				pCurInfo->ulAdcTurnOn ? 0 : 1           ) | /* 1 for powersaving when all ADCs are off */
			BCHP_FIELD_DATA(ADC_1_CONTROL, CLKCNTRL, 23 ) | /* nominal */
			BCHP_FIELD_DATA(ADC_1_CONTROL, BGADJ,    4  ) | /* nominal */
			BCHP_FIELD_DATA(ADC_1_CONTROL, IBCNTRL,  0  );  /* nominal */

		/* KLUDGE: ADC/VAFE to be running using PLL / SYS clock.   Currently
		 * all ADC's clk connected to ADC_6 clk. */
		if((pCurInfo->bUsePllClk) && (!pCurInfo->bPcInput))
		{
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
			/* ADC_3_CLOCK (RW) */
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_ADC_3_CLOCK);
			*pList->pulCurrent++ = BCHP_FIELD_ENUM(ADC_3_CLOCK, SELECT, CLK_PC);
#endif
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
			/* ADC_5_CLOCK (RW) */
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_ADC_5_CLOCK);
			*pList->pulCurrent++ = BCHP_FIELD_ENUM(ADC_5_CLOCK, SELECT, CLK_PC);
#endif
		}

		/* (1) Turn on when at lease one vdec use the INPUT. */
		/* (2) Turn off when all vdecs don't use the INPUT */
		/* ADC_1_INOFF (RW) */
		ulAdcTurnOn = pCurInfo->ulAdcTurnOn;
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		if((pCurInfo->bCompInput) && VIDEO_FORMAT_IS_480P(pCurInfo->pFmtInfo->eVideoFmt))
		{
			ulAdcTurnOn |= BCHP_ADC_1_INOFF_RGB_MASK;
		}
#endif
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_ADC_1_INOFF);
		*pList->pulCurrent++ = (pCurInfo->stVdecSettings.bBypassCommonMode)
			? (0) : (~ulAdcTurnOn);
	}

	return;
}


/***************************************************************************
 * Need good justification why entries are added into here!  Do not just
 * place them here!
 *
 */
static void BVDC_P_Vdec_BuildVsynRul_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_P_ListInfo                 *pList,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	/* New trigger mode require RDC to re-enable feeding out after every
	 * field.  So this is a must whenever we need to feed out. */
	if(hVdec->bVideoDetected && (!hVdec->ulDelayStart))
	{
		/* VDEC_BE_0_VDEC_HOST_ENABLE (WO) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_BE_0_VDEC_HOST_ENABLE + hVdec->ulBeOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VDEC_BE_0_VDEC_HOST_ENABLE, HOST_ENABLE, 1 );
	}

	/* Save the phase into reserved RDC variable */
	if(pCurInfo->bVdecPhaseAdj)
	{
		*pList->pulCurrent++ = BRDC_OP_REG_TO_REG( 1 );
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_CD_0_SUBCARRIER_PHASE_STATUS + hVdec->ulCdOffset);
		*pList->pulCurrent++ = BRDC_REGISTER(hVdec->ulPhaseAddr);
	}

	*pList->pulCurrent++ = BRDC_OP_REG_TO_REG( 1 );
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_FE_FIELD_SIZE + hVdec->ulFeOffset);
	*pList->pulCurrent++ = BRDC_REGISTER(hVdec->ulFeSizeAddr);

	return;
}

/***************************************************************************
 *PR49561:[VDEC]: Vertical black bar on the white noise channel
 */
static void BVDC_P_Vdec_BuildResetHsync_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_P_ListInfo                 *pList )
{
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	if(hVdec->bResetHsyncPllAccum)
	{
	/* VDEC_FE_0_HSYNC_PLL_ACCUM (WO) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VDEC_FE_0_HSYNC_PLL_ACCUM + hVdec->ulFeOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_ACCUM, ERROR, 0);
	}
	return;
}

/***************************************************************************
 *
 */
static void BVDC_P_Vdec_SampleReg_isr
	( uint32_t                         ulCurVal,
	  BVDC_P_RegSample                *pRegSample,
	  uint32_t                        *aulSample )
{
	uint32_t i, j;
	uint32_t ulTemp = 0;
	uint32_t ulSwapCnt;

	if(pRegSample->ulIndex < pRegSample->ulTheshold)
	{
		aulSample[pRegSample->ulIndex++] = ulCurVal;
	}

	/* Hit theshold doing min/max/avg/etc. */
	if(pRegSample->ulIndex == pRegSample->ulTheshold)
	{
		/* Reset the index and raise flag that average just updated. */
		pRegSample->ulIndex  = 0;
		pRegSample->bUpdated = true;

		/* Sample meet theshold */
		switch(pRegSample->eSampleAlg)
		{
		case BVDC_P_SampleAlg_eMin:
			ulTemp = aulSample[0];
			for(i = 0; i < pRegSample->ulTheshold; i++)
			{
				if(ulTemp > aulSample[i])
				{
					ulTemp = aulSample[i];
				}
			}
			pRegSample->ulValue = ulTemp;
			break;

		case BVDC_P_SampleAlg_eMax:
			for(i = 0; i < pRegSample->ulTheshold; i++)
			{
				if(ulTemp < aulSample[i])
				{
					ulTemp = aulSample[i];
				}
			}
			pRegSample->ulValue = ulTemp;
			break;

		case BVDC_P_SampleAlg_eMedian:
			for(i = 0; i < (pRegSample->ulTheshold - 1); i++)
			{
				ulSwapCnt = 0;
				for(j = 0; j < (pRegSample->ulTheshold - i - 1); j++)
				{
					if(aulSample[j] > aulSample[j + 1])
					{
						ulTemp = aulSample[j];
						aulSample[j] = aulSample[j + 1];
						aulSample[j + 1] = ulTemp;
						ulSwapCnt++;
					}
				}
				if(!ulSwapCnt)
				{
					break;
				}
			}
			pRegSample->ulValue = aulSample[pRegSample->ulTheshold / 2];
			break;

		default:
		case BVDC_P_SampleAlg_eMean:
			for(i = 0; i < pRegSample->ulTheshold; i++)
			{
				ulTemp += aulSample[i];
			}
			pRegSample->ulValue = ulTemp / pRegSample->ulTheshold;
			break;
		}
	}
	else if(pRegSample->ulIndex > pRegSample->ulTheshold)
	{
		pRegSample->ulIndex = 0;
	}

	return;
}


/***************************************************************************
 * To be use as software format detection!  Or for information!
 *
 */
static void BVDC_P_Vdec_ReadFdStatus_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_P_FdtEntry                 *pFd )
{
	uint32_t ulReg;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);

	/* Update all the related register every Vsync.  Consider optimization. */
	ulReg = BREG_Read32_isr(hVdec->hReg, BCHP_FD_0_LPF_VIDEO_FORMAT_STATUS_0 + hVdec->ulFdOffset);
	pFd->ulHsPol =
		BCHP_GET_FIELD_DATA(ulReg, FD_0_LPF_VIDEO_FORMAT_STATUS_0, FT_HSYNC_POLARITY );
	pFd->ulVsPol =
		BCHP_GET_FIELD_DATA(ulReg, FD_0_LPF_VIDEO_FORMAT_STATUS_0, FT_VSYNC_POLARITY );
	pFd->ulDeltaLl =
		BCHP_GET_FIELD_DATA(ulReg, FD_0_LPF_VIDEO_FORMAT_STATUS_0, FT_DELTA_LL       );
	pFd->ulHsWidth =
		BCHP_GET_FIELD_DATA(ulReg, FD_0_LPF_VIDEO_FORMAT_STATUS_0, FT_HSYNC_WIDTH    );
	pFd->ulDeltaHw =
		BCHP_GET_FIELD_DATA(ulReg, FD_0_LPF_VIDEO_FORMAT_STATUS_0, FT_DELTA_HW       );
	pFd->ulRasterT =
		BCHP_GET_FIELD_DATA(ulReg, FD_0_LPF_VIDEO_FORMAT_STATUS_0, FT_RASTER_TYPE    );

	ulReg = BREG_Read32_isr(hVdec->hReg, BCHP_FD_0_LPF_VIDEO_FORMAT_STATUS_1 + hVdec->ulFdOffset);
	pFd->ulLpf =
		BCHP_GET_FIELD_DATA(ulReg, FD_0_LPF_VIDEO_FORMAT_STATUS_1, FT_LINES_PER_FIELD);
	pFd->ulDeltaLpf =
		BCHP_GET_FIELD_DATA(ulReg, FD_0_LPF_VIDEO_FORMAT_STATUS_1, FT_DELTA_LPF      );
	pFd->ulLineLength =
		BCHP_GET_FIELD_DATA(ulReg, FD_0_LPF_VIDEO_FORMAT_STATUS_1, FT_LINE_LENGTH    );

	/* Print out the what hw sees! */
	BDBG_MSG(("FT_HSYNC_POLARITY  = %d", pFd->ulHsPol     ));
	BDBG_MSG(("FT_VSYNC_POLARITY  = %d", pFd->ulVsPol     ));
	BDBG_MSG(("FT_RASTER_TYPE     = %d", pFd->ulRasterT   ));
	BDBG_MSG(("FT_HSYNC_WIDTH     = %d", pFd->ulHsWidth   ));
	BDBG_MSG(("FT_LINES_PER_FIELD = %d", pFd->ulLpf       ));
	BDBG_MSG(("FT_LINE_LENGTH     = %d", pFd->ulLineLength));

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_Vdec_ReadRegStatus_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t ulCurVal;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);

	/* Update all the related register every Vsync.  Consider optimization. */
	hVdec->ulFdFormatReg      = BREG_Read32_isr(hVdec->hReg, BCHP_FD_0_ARBITER_FINAL_FORMAT_STATUS + hVdec->ulFdOffset);
	hVdec->ulFdLpfMetric0Reg  = BREG_Read32_isr(hVdec->hReg, BCHP_FD_0_LPF_METRIC_STATUS_0 + hVdec->ulFdOffset);
	hVdec->ulFdLpfMetric1Reg  = BREG_Read32_isr(hVdec->hReg, BCHP_FD_0_LPF_METRIC_STATUS_1 + hVdec->ulFdOffset);
	hVdec->ulFdMfMetric0Reg   = BREG_Read32_isr(hVdec->hReg, BCHP_FD_0_MF_METRIC_STATUS_0 + hVdec->ulFdOffset);
	hVdec->ulFdMfMetric1Reg   = BREG_Read32_isr(hVdec->hReg, BCHP_FD_0_MF_METRIC_STATUS_1 + hVdec->ulFdOffset);
	hVdec->ulDpllStatusReg    = BREG_Read32_isr(hVdec->hReg, BCHP_PC_IN_0_SYNC_DPLL_STATUS);
	hVdec->ulFeStatusReg      = BREG_Read32_isr(hVdec->hReg, BCHP_VDEC_FE_0_FE_STATUS + hVdec->ulFeOffset);
	hVdec->ulFeStatus0Reg     = BREG_Read32_isr(hVdec->hReg, BCHP_VDEC_FE_0_FE_STATUS0 + hVdec->ulFeOffset);
	hVdec->ulFeSyncSliceReg   = BREG_Read32_isr(hVdec->hReg, BCHP_VDEC_FE_0_FE_SYNC_SLICE + hVdec->ulFeOffset);
	hVdec->ulFeSampleCntReg   = BREG_Read32_isr(hVdec->hReg, BCHP_VDEC_FE_0_FE_SAMPLE_COUNT + hVdec->ulFeOffset);
	hVdec->ulCdStatusReg      = BREG_Read32_isr(hVdec->hReg, BCHP_VDEC_CD_0_CD_STATUS + hVdec->ulCdOffset);
	hVdec->ulCdStatus2Reg     = BREG_Read32_isr(hVdec->hReg, BCHP_VDEC_CD_0_CD_STATUS2 + hVdec->ulCdOffset);
	hVdec->ulCdColorStripeReg = BREG_Read32_isr(hVdec->hReg, BCHP_VDEC_CD_0_COLOR_STRIPE_STATUS + hVdec->ulCdOffset);
	hVdec->ulCdSubPhaseReg    = BREG_Read32_isr(hVdec->hReg, hVdec->ulPhaseAddr);
	hVdec->ulFeFieldSizeReg   = BREG_Read32_isr(hVdec->hReg, hVdec->ulFeSizeAddr);
	hVdec->ulFeAgcLevelsReg   = BREG_Read32_isr(hVdec->hReg, BCHP_VDEC_FE_0_FE_AGC_LEVELS + hVdec->ulFeOffset);
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
	hVdec->ulSecamSyncStatusReg = BREG_Read32_isr(hVdec->hReg, BCHP_VDEC_CD_0_SECAM_CSYNC_STATUS  + hVdec->ulCdOffset);
#endif
	hVdec->bDpllLocked = BCHP_GET_FIELD_DATA(hVdec->ulDpllStatusReg, PC_IN_0_SYNC_DPLL_STATUS, DPLL_LOCKED);

#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
	hVdec->ulFdFuncSelect0Reg = BREG_Read32_isr(hVdec->hReg, BCHP_FD_0_S1S2_FORMAT_INTERNAL_STATUS_0 + hVdec->ulFdOffset);
	hVdec->ulFdFuncSelect1Reg = BREG_Read32_isr(hVdec->hReg, BCHP_FD_0_S1S2_FORMAT_INTERNAL_STATUS_1 + hVdec->ulFdOffset);
#endif

	/* update stSampleCount */
	ulCurVal = BCHP_GET_FIELD_DATA(hVdec->ulFeSampleCntReg,
		VDEC_FE_0_FE_SAMPLE_COUNT, COUNT);
	BVDC_P_Vdec_SampleReg_isr(ulCurVal, &hVdec->stSampleCount, hVdec->aulSampleCount);

	/* update stLinePerField */
	ulCurVal = BCHP_GET_FIELD_DATA(hVdec->ulFeFieldSizeReg,
		VDEC_FE_0_FE_FIELD_SIZE, VERTICAL);
	BVDC_P_Vdec_SampleReg_isr(ulCurVal, &hVdec->stLinePerField, hVdec->aulLinePerField);

	/* update stSamplePerLine */
	ulCurVal = BCHP_GET_FIELD_DATA(hVdec->ulFeFieldSizeReg, VDEC_FE_0_FE_FIELD_SIZE, HORIZONTAL);
	BVDC_P_Vdec_SampleReg_isr(ulCurVal, &hVdec->stSamplePerLine, hVdec->aulSamplePerLine);

	/* Averaging only needed for these inputs! */
	if(pCurInfo->bCvbsInput ||pCurInfo->bSvidInput || pCurInfo->bIfdInput)
	{
		if(hVdec->ulVsyncCnt > BVDC_P_VDEC_NOISE_DELAY_TO_SAMPLE)
		{
			/*update stNoiseLevel*/
			ulCurVal = BCHP_GET_FIELD_DATA(hVdec->ulFeStatusReg,
				VDEC_FE_0_FE_STATUS, NOISE_LEVEL);
			BVDC_P_Vdec_SampleReg_isr(ulCurVal, &hVdec->stNoiseLevel, hVdec->aulNoiseLevel);
		}
		/*update stColorBurstAmp*/
		ulCurVal = BCHP_GET_FIELD_DATA(hVdec->ulCdStatus2Reg,
			VDEC_CD_0_CD_STATUS2, FIELD_AVERAGED_COLOR_BURST_AMP);
		BVDC_P_Vdec_SampleReg_isr(ulCurVal, &hVdec->stColorBurstAmp, hVdec->aulColorBurstAmp);

		/* update ColorBurstAmpDelta */
		hVdec->ulColorBurstAmpDelta = BCHP_GET_FIELD_DATA(hVdec->ulCdStatus2Reg,
			VDEC_CD_0_CD_STATUS2, FIELD_AVERAGED_COLOR_BURST_AMP_DELTA);

		/*update stSubCarLoopAcc*/
		ulCurVal = BCHP_GET_FIELD_DATA(hVdec->ulCdStatusReg,
			VDEC_CD_0_CD_STATUS, SUBCARRIER_LOOP_ACCU);
		BVDC_P_Vdec_SampleReg_isr(ulCurVal, &hVdec->stSubCarLoopAcc, hVdec->aulSubCarLoopAcc);

		/*update stAutoAcgcGain*/
		ulCurVal = BCHP_GET_FIELD_DATA(hVdec->ulCdStatus2Reg,
				VDEC_CD_0_CD_STATUS2, FIELD_AVERAGED_ACGC_GAIN);
		BVDC_P_Vdec_SampleReg_isr(ulCurVal, &hVdec->stAutoAcgcGain, hVdec->aulAutoAcgcGain);

	}
	else
	{
		/*update stNoiseLevel*/
		hVdec->stNoiseLevel.ulValue = BVDC_P_VDEC_NOISE_LEVEL_MAX;
		hVdec->stNoiseLevel.ulIndex = 0;

		/*update stColorBurstAmp*/
		hVdec->stColorBurstAmp.ulIndex = 0;

		/*update stSubCarLoopAcc*/
		hVdec->stSubCarLoopAcc.ulIndex = 0;

		/*update stAutoAcgcGain*/
		hVdec->stAutoAcgcGain.ulIndex = 0;
	}

	/* SW3556-804: Function select updated */
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
	{
		BVDC_ScartFuncSelect eScartFuncSelect0, eScartFuncSelect1;

		eScartFuncSelect0 = BCHP_GET_FIELD_DATA(hVdec->ulFdFuncSelect0Reg,
			FD_0_S1S2_FORMAT_INTERNAL_STATUS_0, FORMAT_INDEX);
		eScartFuncSelect1 = BCHP_GET_FIELD_DATA(hVdec->ulFdFuncSelect1Reg,
			FD_0_S1S2_FORMAT_INTERNAL_STATUS_1, FORMAT_INDEX);

		/* Updated to notify callback */
		if((hVdec->eScartFuncSelect0 != eScartFuncSelect0) ||
		   (hVdec->eScartFuncSelect1 != eScartFuncSelect1))
		{
			hVdec->eScartFuncSelect0 = eScartFuncSelect0;
			hVdec->eScartFuncSelect1 = eScartFuncSelect1;
			hVdec->bScartFuncSelectUpdated = true;
		}
	}
#endif

	/* SW3548-2751  */
	{
		uint32_t aulFeAgcGain[BVDC_COLOR_CHANNELS];

		aulFeAgcGain[0] = BCHP_GET_FIELD_DATA(hVdec->ulFeAgcLevelsReg,
			VDEC_FE_0_FE_AGC_LEVELS, CH0_GAIN);
		aulFeAgcGain[1] = BCHP_GET_FIELD_DATA(hVdec->ulFeAgcLevelsReg,
			VDEC_FE_0_FE_AGC_LEVELS, CH1_GAIN);
		aulFeAgcGain[2] = BCHP_GET_FIELD_DATA(hVdec->ulFeAgcLevelsReg,
			VDEC_FE_0_FE_AGC_LEVELS, CH2_GAIN);

		/* Updated to notify callback, level +/- delta=10 */
		if(!BVDC_P_EQ_DELTA(hVdec->aulFeAgcGain[0], aulFeAgcGain[0], 1) ||
		   !BVDC_P_EQ_DELTA(hVdec->aulFeAgcGain[1], aulFeAgcGain[1], 1) ||
		   !BVDC_P_EQ_DELTA(hVdec->aulFeAgcGain[2], aulFeAgcGain[2], 1))
		{
			/* BDBG_MSG(("aulFeAgcGain[0]: %d -> %d", hVdec->aulFeAgcGain[0], aulFeAgcGain[0]));
			BDBG_MSG(("aulFeAgcGain[1]: %d -> %d", hVdec->aulFeAgcGain[1], aulFeAgcGain[1]));
			BDBG_MSG(("aulFeAgcGain[2]: %d -> %d", hVdec->aulFeAgcGain[2], aulFeAgcGain[2]));
			BDBG_MSG(("-------------------------")); */
			BKNI_Memcpy(hVdec->aulFeAgcGain, aulFeAgcGain, sizeof(hVdec->aulFeAgcGain));
			hVdec->bFeAgcLevelsUpdated = true;
		}
	}

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_Vdec_UpdatePcInPhase_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  bool                            *pbPcInPhaseLocked )
{
	uint32_t i;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	/* PC_IN_0_PHASE STATUS */
	hVdec->ulPcInPhaseStatusReg =
		BREG_Read32_isr(hVdec->hReg, BCHP_PC_IN_0_PHASE_STATUS);

	hVdec->abPcPhaseLocked[0] = BCHP_GET_FIELD_DATA(
		hVdec->ulPcInPhaseStatusReg, PC_IN_0_PHASE_STATUS, CH0_LOCK);
	hVdec->abPcPhaseLocked[1] = BCHP_GET_FIELD_DATA(
		hVdec->ulPcInPhaseStatusReg, PC_IN_0_PHASE_STATUS, CH1_LOCK);
	hVdec->abPcPhaseLocked[2] = BCHP_GET_FIELD_DATA(
		hVdec->ulPcInPhaseStatusReg, PC_IN_0_PHASE_STATUS, CH2_LOCK);

	/* Check if phase locked of chx change? */
	*pbPcInPhaseLocked = true;

	for(i = 0; i < BVDC_COLOR_CHANNELS; i++)
	{
		*pbPcInPhaseLocked &= hVdec->abPcPhaseLocked[i];
	}

	return;
}


/***************************************************************************
 *
 */
static  const BVDC_P_FdtEntry* BVDC_P_Vdec_FindFmtMap_isr
	( uint32_t                         ulFmtId,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t i;
	bool bFound = false;
	const BFMT_VideoInfo *pFmtInfoFr;
	const BFMT_VideoInfo *pFmtInfoTo;
	const BVDC_P_FdtEntry *pFdEntryFr;
	const BVDC_P_FdtEntry *pFdEntryTo;

	/* (1) Normal detected id with correct H/V polarity. */
	pFdEntryFr = pFdEntryTo = &pCurInfo->aFdtFmtTbl[ulFmtId-1];
	pFmtInfoFr = pFmtInfoTo = BFMT_GetVideoFormatInfoPtr(pFdEntryFr->eFmt);

	/* (2a) Find matching fmt entry */
	for(i = 0; i < pCurInfo->ulReMapFormats; i++)
	{
		if(pFdEntryFr->eFmt == pCurInfo->aReMapFormats[i].eFmtA)
		{
			pFmtInfoTo = BFMT_GetVideoFormatInfoPtr(
				pCurInfo->aReMapFormats[i].eFmtB);
			bFound = true;
			break;
		}
	}

	/* (2b) Find coresponding fdt entry */
	if(bFound)
	{
		for(i = 0; i < pCurInfo->ulFdtCount; i++)
		{
			pFdEntryTo = &pCurInfo->aFdtFmtTbl[i];
			if((pFmtInfoTo->eVideoFmt == pFdEntryTo->eFmt) &&
			   (pFdEntryFr->ulHsPol == pFdEntryTo->ulHsPol) &&
			   (pFdEntryFr->ulVsPol == pFdEntryTo->ulVsPol))
			{
				break;
			}
		}
		BDBG_MSG(("Remapping format: %s -> %s!", pFmtInfoFr->pchFormatStr,
			pFmtInfoTo->pchFormatStr));
	}

	BDBG_ASSERT(pFdEntryTo->eFmt == pFmtInfoTo->eVideoFmt);
	return pFdEntryTo;
}


/***************************************************************************
 *
 */
static void BVDC_P_Vdec_UpdatePcInPol_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  const BVDC_P_Source_Info        *pCurInfo,
	  uint32_t                        *pulPcHsPol,
	  uint32_t                        *pulPcVsPol )
{
	uint32_t ulFmtId;
	const BVDC_P_FdtEntry *pFdEntry;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	/* FORMAT DETECTION */
	ulFmtId = BCHP_GET_FIELD_DATA(
		hVdec->ulFdFormatReg, FD_0_ARBITER_FINAL_FORMAT_STATUS, FORMAT_INDEX);

	pFdEntry = (ulFmtId)
		? &pCurInfo->aFdtFmtTbl[ulFmtId-1] : pCurInfo->pFdEntry;

	*pulPcHsPol = pFdEntry->ulHsPol;
	*pulPcVsPol = pFdEntry->ulVsPol;

	return;
}

/***************************************************************************
 *
 */
static void BVDC_P_Vdec_UpdateVideoLock_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  const BVDC_P_Source_Info        *pCurInfo,
	  bool                            *pbVideoLock )
{
	uint32_t ulFieldAvgErr;
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	/* VIDEO & HPLL STATUS */
	*pbVideoLock = BCHP_GET_FIELD_DATA(
		hVdec->ulFeStatus0Reg, VDEC_FE_0_FE_STATUS0, VIDEO_DETECTED);

	ulFieldAvgErr = BCHP_GET_FIELD_DATA(
		hVdec->ulFeStatus0Reg, VDEC_FE_0_FE_STATUS0, FIELD_AVG_ERR);

	if(*pbVideoLock)
	{
		if((ulFieldAvgErr > 26) && (hVdec->ulFieldAvgErrCnt  < 300))
		{
			hVdec->ulFieldAvgErrCnt ++;
		}
	}
	else
	{
		hVdec->ulFieldAvgErrCnt = 0;
	}

	if (hVdec->ulFieldAvgErrCnt > 8 )
	{
		hVdec->bFieldAvgErr = true;
	}
	else
	{
		hVdec->bFieldAvgErr = false;
	}

	/* for pc input, we need more condition to judge the video locked. */
	if(pCurInfo->bPcInput)
	{
		if((!BCHP_GET_FIELD_DATA(hVdec->ulFdLpfMetric0Reg, FD_0_LPF_METRIC_STATUS_0, LINE_LENGTH_LOCK)) ||
		   (!BCHP_GET_FIELD_DATA(hVdec->ulFdLpfMetric0Reg, FD_0_LPF_METRIC_STATUS_0, HSYNC_WIDTH_LOCK)) ||
		   (!BCHP_GET_FIELD_DATA(hVdec->ulFdFormatReg, FD_0_ARBITER_FINAL_FORMAT_STATUS, FORMAT_INDEX)))
		{
			*pbVideoLock = 0;
		}
	}
	/* If video is not detected, increase ulVideoNotDetect until it reaches
	 * BVDC_P_VDEC_VIDEO_NOT_DETECT_THRESHOLD */
	if(!(*pbVideoLock))
	{
		if(hVdec->ulVideoNotDetect < BVDC_P_VDEC_VIDEO_NOT_DETECT_THRESHOLD)
		{
			hVdec->ulVideoNotDetect++;
		}
	}
	else
	{
		hVdec->ulVideoNotDetect = 0;
	}

	/* In VCR mode, don't change video detect status in case of
	 * ulVideoNotDetect less than BVDC_P_VDEC_VIDEO_NOT_DETECT_THRESHOLD */
	if((!(*pbVideoLock)) &&
	   (hVdec->bVcrHeadSwitch) &&
	   (hVdec->ulVideoNotDetect < BVDC_P_VDEC_VIDEO_NOT_DETECT_THRESHOLD))
	{
		*pbVideoLock = hVdec->bVideoDetected;
	}
	else if((!(*pbVideoLock)) &&
	   (!hVdec->bVcrHeadSwitch) &&
	   (hVdec->ulVideoNotDetect < BVDC_P_VDEC_VIDEO_NOT_DETECT_THRESHOLD_NORMAL))
	{
		*pbVideoLock = hVdec->bVideoDetected;
	}

	/* Video is probably garbage when sourc is unplugpged, and need vec
	 * to drive triggers. */
	*pbVideoLock &= (BVDC_P_TriggerCtrl_eSource == hVdec->hSource->eTrigCtrl)
		? true : false;

	return;
}

/***************************************************************************
 *
 */
static void BVDC_P_Vdec_UpdateHpllLock_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  BVDC_P_Source_Info              *pCurInfo,
	  bool                            *pbHPllLocked,
	  bool                            *pbResetHStart,
	  bool                            *pbCompEnableSliceLPFHpllCtrl ,
	  bool                            *pbResetHsyncPllAccum )
{
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	/* VIDEO & HPLL STATUS */
	*pbHPllLocked = BCHP_GET_FIELD_DATA(
		hVdec->ulFeStatus0Reg, VDEC_FE_0_FE_STATUS0, HPLL_LOCKED);

	/* for pc input, we need more condition to judge the pll locked. */
	if(pCurInfo->bPcInput)
	{
		if((!BCHP_GET_FIELD_DATA(hVdec->ulFdLpfMetric0Reg, FD_0_LPF_METRIC_STATUS_0, LINE_LENGTH_LOCK)) ||
		   (!BCHP_GET_FIELD_DATA(hVdec->ulFdLpfMetric0Reg, FD_0_LPF_METRIC_STATUS_0, HSYNC_WIDTH_LOCK)) ||
		   (!BCHP_GET_FIELD_DATA(hVdec->ulFdFormatReg, FD_0_ARBITER_FINAL_FORMAT_STATUS, FORMAT_INDEX)))
		{
			*pbHPllLocked = 0;
		}
	}

	if(!(*pbHPllLocked))
	{
		if(hVdec->ulHpllUnLock < BVDC_P_VDEC_HPLL_UNLOCK_MAX)
		{
			hVdec->ulHpllUnLock++;
		}
	}
	else
	{
		hVdec->ulHpllUnLock = 0;
	}

	/* In VCR mode, don't change HPLL lock status in case of ulHpllUnLock
	 * less than BVDC_P_VDEC_HPLL_TOGGLING_VCR_DELAY */
	if((!(*pbHPllLocked)) &&
	   (hVdec->bVcrHeadSwitch) &&
	   (hVdec->ulHpllUnLock < BVDC_P_VDEC_HPLL_TOGGLING_VCR_DELAY))
	{
		*pbHPllLocked = hVdec->bHPllLocked;
	}
	else if((!(*pbHPllLocked)) &&
	   (!hVdec->bVcrHeadSwitch) &&
	   (hVdec->ulHpllUnLock < BVDC_P_VDEC_HPLL_TOGGLING_NORMAL_DELAY))
	{
		*pbHPllLocked = hVdec->bHPllLocked;
	}

	/* PC Input */
	if(pCurInfo->bPcInput)
	{
		/* No input */
		if((!BCHP_GET_FIELD_DATA(hVdec->ulFdLpfMetric0Reg, FD_0_LPF_METRIC_STATUS_0, LINE_LENGTH_LOCK)) ||
		   (!BCHP_GET_FIELD_DATA(hVdec->ulFdLpfMetric0Reg, FD_0_LPF_METRIC_STATUS_0, HSYNC_WIDTH_LOCK)))
		{
			hVdec->bNoInputSignal = true;
		}
		else
		{
			hVdec->bNoInputSignal = false;
		}
	}
	else if(pCurInfo->bCompInput)
	{
		/* No input */
		if((!BCHP_GET_FIELD_DATA(hVdec->ulFdLpfMetric0Reg, FD_0_LPF_METRIC_STATUS_0, LINE_LENGTH_LOCK)) &&
		   (!BCHP_GET_FIELD_DATA(hVdec->ulFdMfMetric0Reg, FD_0_MF_METRIC_STATUS_0, LINE_LENGTH_LOCK)))
		{
			hVdec->bNoInputSignal = true;
		}
		else
		{
			hVdec->bNoInputSignal = false;
		}
	}
	else /*NTSC/CVBS/Svideo*/
	{
		hVdec->bNoInputSignal = !(hVdec->bHPllLocked);
	}

	/*PR49561: [VDEC]: Vertical black bar on the white noise channel*/
	{
		uint32_t ulCurVal;
		ulCurVal = BCHP_GET_FIELD_DATA(hVdec->ulFeSyncSliceReg,
			VDEC_FE_0_FE_SYNC_SLICE, TYPE);
		if((ulCurVal == 1) && (pCurInfo->bIfdInput))
		{
			*pbResetHStart = true;
		}
		else
		{
			*pbResetHStart = false;
		}
	}

	/* PR50097: */
	if((pCurInfo->bCompInput) && (!(*pbHPllLocked)) && (hVdec->ulVsyncCnt > 40))
	{
		*pbCompEnableSliceLPFHpllCtrl = true;
	}
	else
	{
		*pbCompEnableSliceLPFHpllCtrl = false;
	}
	if(pCurInfo->bIfdInput && (!hVdec->bHPllLocked))
	{
		if(hVdec->ulVsyncCnt == 5)
		{
			*pbResetHsyncPllAccum = true;
		}
		else if(hVdec->ulVsyncCnt % 300 == 0)
		{
			*pbResetHsyncPllAccum = true;
		}
		else
		{
			*pbResetHsyncPllAccum = false;
		}
	}
	else
	{
		*pbResetHsyncPllAccum = false;
	}

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_Vdec_UpdateVdecPhase_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  uint32_t                        *pulVdecPhase )
{
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	/* VDEC/VEC PHASE ADJUST */
	*pulVdecPhase = BCHP_GET_FIELD_DATA(
		hVdec->ulCdSubPhaseReg, VDEC_CD_0_SUBCARRIER_PHASE_STATUS, RAW);

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_Vdec_UpdateCopyProtection_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  const BVDC_P_Source_Info        *pCurInfo,
	  bool                            *pbAgc,
	  uint32_t                        *pulColorStripe,
	  BVDC_MacrovisionType            *peMvType )
{
	uint32_t ulFeStatus;
	uint32_t ulCdMvStatusReg;
	bool bMvDetected;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	ulFeStatus = hVdec->ulFeStatusReg;
	ulCdMvStatusReg = hVdec->ulCdColorStripeReg;
	bMvDetected = BVDC_P_COMPARE_FIELD_NAME(ulFeStatus, VDEC_FE_0_FE_STATUS, MV_AGC, DETECTED);

	/* No need for pc input, nor SECAM */
	if((pCurInfo->bPcInput) ||
	   (VIDEO_FORMAT_IS_SECAM(pCurInfo->pFmtInfo->eVideoFmt)))
	{
		*pbAgc          = false;
		*peMvType       = BVDC_MacrovisionType_eNoProtection;
		*pulColorStripe = 0;
		goto done;
	}

#if (BVDC_P_VDEC_PR32896_WORK_AROUND)
	/* There is a bug in HW that can't detect narrow CP AGC pulse */
	/* when the signal is high, and the CP detection is toggled   */
	/* This SW work around maintains the historesis data of the detected   */
	/* status and suppress the toggling in case it is a false detection    */

	/* Recalculate the detected count and swap out the status in the */
	/* historesis buffer */
	hVdec->ulMvDetectedCount = hVdec->ulMvDetectedCount + bMvDetected -
	                           hVdec->aulMvDetected[hVdec->ulMvId];
	hVdec->aulMvDetected[hVdec->ulMvId] = bMvDetected;
	hVdec->ulMvId++;
	if(hVdec->ulMvId >=  BVDC_P_VDEC_MV_STATUS_COUNT)
	{
		hVdec->ulMvId = 0;
	}

	if(!bMvDetected)
	{
		if(hVdec->ulMvDetectedCount >= BVDC_P_VDEC_MV_STATUS_MIN_GOOD)
		{
			bMvDetected = true;
		}
	}
	else
	{
		if(hVdec->ulMvDetectedCount < BVDC_P_VDEC_MV_STATUS_MAX_BAD)
		{
			bMvDetected = false;
		}
	}
#endif

	/* Detect Copy Protection Type */
	if(!hVdec->bVideoDetected || (!bMvDetected && BVDC_P_COMPARE_FIELD_NAME(ulCdMvStatusReg, VDEC_CD_0_COLOR_STRIPE_STATUS, STATUS, UN_DETECT)))
	{
		*peMvType = BVDC_MacrovisionType_eNoProtection;
		*pulColorStripe = 0;
	}
	else if(bMvDetected && BVDC_P_COMPARE_FIELD_NAME(ulCdMvStatusReg, VDEC_CD_0_COLOR_STRIPE_STATUS, STATUS, UN_DETECT))
	{
		*peMvType = BVDC_MacrovisionType_eAgcOnly;
		*pulColorStripe = 0;
	}
	else
	{
		if(BVDC_P_COMPARE_FIELD_NAME(ulCdMvStatusReg, VDEC_CD_0_COLOR_STRIPE_STATUS, TYPE, TYPE2))
		{
			*peMvType = BVDC_MacrovisionType_eAgc2Lines;
			*pulColorStripe = 2;
		}
		else
		{
			*peMvType = BVDC_MacrovisionType_eAgc4Lines;
			*pulColorStripe = 4;
		}
	}

	*pbAgc = (bMvDetected && hVdec->bVideoDetected) ? true : false;

done:
	return;
}


#if (BVDC_P_USE_WORK_AROUNDS)
/***************************************************************************
 *
 */
static void BVDC_P_Vdec_UpdateSamplePerLine_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  BVDC_P_Source_Info              *pCurInfo,
	  bool                            *pbFh3DComb )
{
	BSTD_UNUSED(pCurInfo);
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);

	/* caculate the average sample per line every vsync */
	if(hVdec->stSamplePerLine.bUpdated)
	{
		if((pCurInfo->bCvbsInput || pCurInfo->bSvidInput || pCurInfo->bIfdInput) &&
		   (!hVdec->bVcrHeadSwitch))
		{
			uint32_t ulRealHFreq;
			/* HPLL can't recover, and will report wrong FE_FIELD_SIZE
			 * use the Mf's paramter instead.  But on error detection case
			 * need to divided by %80 to accomodate the Mf's errors. */
			ulRealHFreq = hVdec->stSamplePerLine.ulValue;

#if (BVDC_P_VDEC_FH_MANUAL)
			/* If MF couldn't detect lock restore back to the standard value. */
			if(!hVdec->bHPllLocked)
			{
				ulRealHFreq = hVdec->ulStdrHFreq;
			}

			/* Update SYNC to REAL if not in range to maintain lock. */
			if(!BVDC_P_EQ_DELTA(ulRealHFreq, hVdec->ulProgHFreq, BVDC_P_VDEC_FH_SYNC_THRESHOLD))
			{
				BDBG_MSG(("ulStdrHFreq = %d", hVdec->ulStdrHFreq));
				BDBG_MSG(("ulProgHFreq = %d", hVdec->ulProgHFreq));
				BDBG_MSG(("ulRealHFreq = %d", ulRealHFreq));
				BDBG_MSG(("----------------"));
				if(hVdec->ulVsyncCnt > BVDC_P_VDEC_FH_DELAY_TO_SAMPLE)
				{
					hVdec->ulProgHFreq = ulRealHFreq;
					pCurInfo->stDirty.stBits.bFvFhShift = BVDC_P_DIRTY;
				}
			}
#endif
			/* Fh deviates from standard Fh turn off 3D Comb. */
			if(BVDC_P_EQ_DELTA(ulRealHFreq, hVdec->ulStdrHFreq, BVDC_P_VDEC_FH_STDR_THRESHOLD))
			{
				*pbFh3DComb = true;
			}
			else
			{
				*pbFh3DComb = false;
			}
		}

		/* Reset index for averaging. */
		hVdec->stSamplePerLine.bUpdated = false;
	}

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_Vdec_CalManualClock_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	int32_t lClockAdj;
	uint32_t ulNI1, ulNF1, ulN2, ulNI2, ulNF2, ulTemp0, ulTemp1, ulTemp2, ulTemp3;
	const BVDC_P_PllEntry *pPll = pCurInfo->pFdEntry->pPll;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	ulNI1     = pPll->ulN;
	ulNF1     = pPll->ulFcwOffset;
	lClockAdj = pCurInfo->stPcInSyncSettings.lClockAdj;

	if(lClockAdj == 0)
	{
		ulNI2 = ulNI1;
		ulNF2 = ulNF1;
	}
	else
	{
		ulTemp0 = (ulNI1 << BVDC_P_PC_CLOCK_FCW_FACTOR) + ulNF1;
		ulTemp1 = ulTemp0 / BVDC_P_PC_CLOCK_ADJ_FACTOR;
		ulTemp2 = ulTemp0 - BVDC_P_PC_CLOCK_ADJ_FACTOR * ulTemp1;
		ulTemp3 = (uint32_t)(BVDC_P_PC_CLOCK_ADJ_FACTOR + lClockAdj);
		ulN2    = ulTemp1 * ulTemp3 + ulTemp2 * ulTemp3 / BVDC_P_PC_CLOCK_ADJ_FACTOR;
		ulNI2   = (ulN2 >> BVDC_P_PC_CLOCK_FCW_FACTOR);
		ulNF2   = ulN2 - (ulNI2 << BVDC_P_PC_CLOCK_FCW_FACTOR);
	}

	hVdec->ulPcManualClockNI = ulNI2;
	hVdec->ulPcManualClockNF = ulNF2;
	return;
}


/***************************************************************************
 *
 */
 #if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
static void BVDC_P_Vdec_UpdateRaLock_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t ulPcInRaLockStatusReg;
	uint32_t ulPcInRaVOffsetReg;
	uint32_t ulPcInRaHOffsetReg;
	uint32_t ulLeft, ulRight, ulTop, ulBottom;
	bool     bRaLocked;

	BSTD_UNUSED(pCurInfo);
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);

	/* read registers */
	ulPcInRaLockStatusReg =
		BREG_Read32(hVdec->hReg, BCHP_PC_IN_0_RA_RASTER_LOCK_STATUS);
	ulPcInRaVOffsetReg =
		BREG_Read32(hVdec->hReg, BCHP_PC_IN_0_RA_V_OFFSET);
	ulPcInRaHOffsetReg =
		BREG_Read32(hVdec->hReg, BCHP_PC_IN_0_RA_H_OFFSET);

	ulLeft   = BCHP_GET_FIELD_DATA(ulPcInRaHOffsetReg, PC_IN_0_RA_H_OFFSET, LEFT);
	ulRight  = BCHP_GET_FIELD_DATA(ulPcInRaHOffsetReg, PC_IN_0_RA_H_OFFSET, RIGHT);
	ulTop    = BCHP_GET_FIELD_DATA(ulPcInRaVOffsetReg, PC_IN_0_RA_V_OFFSET, TOP);
	ulBottom = BCHP_GET_FIELD_DATA(ulPcInRaVOffsetReg, PC_IN_0_RA_V_OFFSET, BOTTOM);

	bRaLocked = BCHP_GET_FIELD_DATA(
		ulPcInRaLockStatusReg, PC_IN_0_RA_RASTER_LOCK_STATUS, AUTO_RASTER_LOCK);
	/* Some cases the result is locked but the status is unlocked. */
	if(!bRaLocked)
	{
		bRaLocked =
			(BVDC_P_EQ_DELTA((int32_t)ulLeft, (int32_t)ulRight, 1)) &&
			(BVDC_P_EQ_DELTA((int32_t)ulTop, (int32_t)ulBottom, 1)) &&
			(ulLeft || ulRight)&&
			(ulTop || ulBottom);
	}
	if(bRaLocked)
	{
		hVdec->ulRaLockTimer ++ ;
	}
	else
	{
		hVdec->ulRaLockTimer = 0 ;
	}
	if(hVdec->ulRaLockTimer >= 3)
	{
		hVdec->ulRaLockTimer = 3 ;
		hVdec->bRaLocked = true ;
	}
	else
	{
		hVdec->bRaLocked = false ;
	}

	return;
}
#endif

/***************************************************************************
 *
 */
static void BVDC_P_Vdec_UpdateLinePerField_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  const BVDC_P_Source_Info        *pCurInfo,
	  bool                            *pbFv3DComb )
{
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	/* caculate the average line per field every vsync */
	if(hVdec->stLinePerField.bUpdated)
	{
		uint32_t ulMaxLpf;
		const BFMT_VideoInfo *pFmtInfo = pCurInfo->pFmtInfo;

		if(hVdec->stNoiseLevel.ulValue > BVDC_P_VDEC_NOISE_LEVEL_ENABLE_LINE_ADJUST)
		{
			/* Note: stLinePerField.ulValue contains number of half lines. */
			hVdec->ulLinePerField = hVdec->stLinePerField.ulValue / 2;

			/* Max line for this format. */
			ulMaxLpf = pFmtInfo->ulHeight << pFmtInfo->bInterlaced;

			/* Substract the VBI area */
			if(hVdec->ulLinePerField > pFmtInfo->ulTopActive)
			{
				hVdec->ulLinePerField -= pFmtInfo->ulTopActive;
			}

			/* BE active count (Active Video) must be less than */
			if(hVdec->ulLinePerField > ulMaxLpf)
			{
				hVdec->ulLinePerField = ulMaxLpf;
			}
		}

		/* With sample per frame shifted outside BVDC_P_VDEC_FH_SAMPLE_THRESHOLD,
		 * need to turn off 3D Comb.  Note 3D comb is only available if go
		 * thru CVBS/IFD. */
		if((pCurInfo->bCvbsInput || pCurInfo->bIfdInput) &&
		   (hVdec->bHPllLocked))
		{
			*pbFv3DComb = BVDC_P_EQ_DELTA(hVdec->ulLinePerField,
				pFmtInfo->ulHeight >> pFmtInfo->bInterlaced, BVDC_P_VDEC_FV_LINE_THRESHOLD);
		}
		else
		{
			*pbFv3DComb = false;
		}

		/* Reset index for averaging. */
		hVdec->stLinePerField.bUpdated = false;
	}

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_Vdec_UpdateFramerate_isr
	( BVDC_P_Vdec_Handle                hVdec,
	  BVDC_P_SourceContext             *pSource,
	  const BVDC_P_Source_Info         *pCurInfo,
	  BAVC_FrameRateCode               *peFrameRate,
	  bool                             *pbClk3DComb )
{
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	if(hVdec->stSampleCount.bUpdated)
	{
		/* For PC or no hpll lock just the format, as detect
		 * will not be correct. */
		if(( pCurInfo->bPcInput) ||
		   (!hVdec->bHPllLocked))
		{
			pSource->ulVertFreq    = pCurInfo->pFmtInfo->ulVertFreq;
			pSource->ulClkPerVsync =
				BVDC_P_216_SYSCLK / (pSource->ulVertFreq / BFMT_FREQ_FACTOR);
		}
		else
		{
			pSource->ulClkPerVsync = hVdec->stSampleCount.ulValue;

			/* Prevent dividing by zero */
			pSource->ulClkPerVsync = BVDC_P_MAX(
				pSource->ulClkPerVsync, BFMT_FREQ_FACTOR);

			/* Convert to BVDC_P_216_system clock!
			 * VDEC: Reports bSd/bEd in 54MHz */
			if((pCurInfo->pVdcFmt->bSd) ||
			   (pCurInfo->pVdcFmt->bEd))
			{
				pSource->ulClkPerVsync *= 4;
			}
			else if((BFMT_VideoFmt_e720p == pCurInfo->pFmtInfo->eVideoFmt) ||
			        (BFMT_VideoFmt_e720p_50Hz == pCurInfo->pFmtInfo->eVideoFmt))
			{
				pSource->ulClkPerVsync *= 2;
			}
			else if((BFMT_VideoFmt_e1080p_24Hz == pCurInfo->pFmtInfo->eVideoFmt) ||
			        (BFMT_VideoFmt_e1080p_25Hz == pCurInfo->pFmtInfo->eVideoFmt) ||
			        (BFMT_VideoFmt_e1080p_30Hz == pCurInfo->pFmtInfo->eVideoFmt))
			{
				pSource->ulClkPerVsync *= 2;
			}

			/* Vertical frequency of this format!  Prevent dividing by zero when
			 *input is removed. */
			pSource->ulVertFreq =
				BVDC_P_216_SYSCLK / (pSource->ulClkPerVsync / BFMT_FREQ_FACTOR);

			/* Frequency is out of bound to be meaningful; use standard vertical
			 * frequency instead. */
			if(!BVDC_P_EQ_DELTA(pSource->ulVertFreq,
				pCurInfo->pFmtInfo->ulVertFreq, (BFMT_FREQ_FACTOR/2)-1))
			{
				BDBG_MSG(("Unstable sample count: %d", pSource->ulVertFreq));
				pSource->ulVertFreq = pCurInfo->pFmtInfo->ulVertFreq;
			}
		}

		/* Generic lookup table use by all source */
		if(pCurInfo->bCvbsInput || pCurInfo->bSvidInput || pCurInfo->bIfdInput)
		{
			*peFrameRate = VIDEO_FORMAT_IS_525_LINES(pCurInfo->pFmtInfo->eVideoFmt)
				? BAVC_FrameRateCode_e59_94 : BAVC_FrameRateCode_e50;
		}
		else
		{
			BVDC_P_Source_UpdateFrameRate_isr(pCurInfo->pFmtInfo, pSource->ulClkPerVsync,
				hVdec->ulClkPerVsyncDelta, peFrameRate);
		}

		/* With sample per frame shifted outside BVDC_P_VDEC_FH_SAMPLE_THRESHOLD,
		 * need to turn off 3D Comb.  Note 3D comb is only available if go
		 * thru CVBS/IFD.
		 * When HPLL is not locked or signal is noisy, this need to be bypassed
		 * because field sample count is very instable in that case. */
		if((pCurInfo->bCvbsInput || pCurInfo->bIfdInput) &&
		    (hVdec->bHPllLocked))
		{
			*pbClk3DComb = BVDC_P_EQ_DELTA(hVdec->stSampleCount.ulValue,
				hVdec->ulStdrHFreq * pCurInfo->pFmtInfo->ulScanHeight,
				BVDC_P_VDEC_FH_CLK_THRESHOLD);
		}
		else
		{
			*pbClk3DComb = false;
		}

		hVdec->stSampleCount.bUpdated = false;
		/* BDBG_MSG(("VDEC[%d] Vertical Refresh Rate = %dHz", hVdec->eId,
			pSource->ulVertFreq)); */
	}

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_Vdec_UpdateVcrHeadSwitch_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  const BVDC_P_Source_Info        *pCurInfo,
	  bool                            *pbVcrHeadSwitch )
{
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	/* VCR HEAD SWITCH */
	*pbVcrHeadSwitch = BCHP_GET_FIELD_DATA(
		hVdec->ulFeStatus0Reg, VDEC_FE_0_FE_STATUS0, HEAD_SWITCH_DETECTED);

	/* Trick mode is invalid when video is not detected,
	   delay detect vcr*/
	*pbVcrHeadSwitch &= (hVdec->bHPllLocked) && (hVdec->ulVsyncCnt > 10);

	/*Trick mode is invalid when FH shift too much*/
	if(!VIDEO_FORMAT_IS_SECAM(pCurInfo->pFmtInfo->eVideoFmt))
	{
		*pbVcrHeadSwitch &= BVDC_P_EQ_DELTA(hVdec->ulStdrHFreq,
			hVdec->stSamplePerLine.ulValue, BVDC_P_VDEC_FH_DISABLE_VCR);
	}

	if(pCurInfo->bCvbsInput || pCurInfo->bSvidInput || pCurInfo->bIfdInput)
	{
		/* Force bVcrHeadSwitch to true until we see out side of window. */
		if((*pbVcrHeadSwitch) &&
		   (hVdec->ulVcrTheshold < BVDC_P_VDEC_HEAD_SWITCH_MAX))
		{
			hVdec->ulVcrTheshold++;

			if(hVdec->ulVcrTheshold > BVDC_P_VDEC_HEAD_SWITCH_GOOD)
			{
				*pbVcrHeadSwitch = true;
				hVdec->ulVcrTheshold = BVDC_P_VDEC_HEAD_SWITCH_MAX;
			}
			else
			{
				*pbVcrHeadSwitch = hVdec->bVcrHeadSwitch;
			}
		}
		else if(!(*pbVcrHeadSwitch) && (hVdec->ulVcrTheshold))
		{
			hVdec->ulVcrTheshold--;

			if(hVdec->ulVcrTheshold < BVDC_P_VDEC_HEAD_SWITCH_MIN)
			{
				*pbVcrHeadSwitch = false;
				hVdec->ulVcrTheshold = 0;
			}
			else
			{
				*pbVcrHeadSwitch = hVdec->bVcrHeadSwitch;
			}
		}
		else
		{
			*pbVcrHeadSwitch = hVdec->bVcrHeadSwitch;
		}
	}
	else
	{
		*pbVcrHeadSwitch = false;
	}

	/* PR43635 : Support manual VCR and Noisy mode */
	if(BVDC_Vdec_VcrMode_eOff == pCurInfo->stVdecSettings.eVcrMode)
	{
		*pbVcrHeadSwitch = false;
		hVdec->ulVcrTheshold = 0;
	}
	else if(BVDC_Vdec_VcrMode_eOn == pCurInfo->stVdecSettings.eVcrMode)
	{
		*pbVcrHeadSwitch = true;
		hVdec->ulVcrTheshold = BVDC_P_VDEC_HEAD_SWITCH_MIN;
	}

	/*
	if(hVdec->ulVcrTheshold && hVdec->ulVcrTheshold != BVDC_P_VDEC_HEAD_SWITCH_MAX)
	{
		BDBG_MSG(("Vsync[%d], VcrTheshold[%d], HFreq[%d]",
			hVdec->ulVsyncCnt, hVdec->ulVcrTheshold, hVdec->ulProgHFreq));
	}
	*/
	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_Vdec_Update240pMode_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  const BVDC_P_Source_Info        *pCurInfo,
	  bool                            *pb240pMode )
{
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);

	/* 240P DETECT */
	if((pCurInfo->pFmtInfo->bInterlaced) &&
	   (BVDC_P_COMPARE_FIELD_NAME(hVdec->ulFeStatusReg, VDEC_FE_0_FE_STATUS, FIELD_MODE, F0_PROG) ||
	    BVDC_P_COMPARE_FIELD_NAME(hVdec->ulFeStatusReg, VDEC_FE_0_FE_STATUS, FIELD_MODE, F1_PROG)))
	{
		*pb240pMode = true;
	}
	else
	{
		*pb240pMode = false;
	}

	return;
}


/***************************************************************************
 * Cross Chroma Reduction (CCR)
 */
static void BVDC_P_Vdec_UpdateCcr_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  const BVDC_P_Source_Info        *pCurInfo,
	  bool                            *pbCcrEnable )
{
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	/* Decide for pbCcrEnable */
	if(hVdec->stSubCarLoopAcc.bUpdated)
	{
		/* Already averaged */
		uint32_t ulSubCarLoopAcc = hVdec->stSubCarLoopAcc.ulValue;

		/* Store the absolute value here to be else where */
		hVdec->ulSubCarrLoopAccAbs = hVdec->stSubCarLoopAcc.ulValue;
		hVdec->bSubCarrLoopAccAbsUpdated = true;

		/* NOTE: Subcarrier accumulator is a 18-bit signed number with the
		 * MSB as sign bit. So if bit 18 is 1, it's actually a negative
		 * number and we should get it's absolute value.
		 * TODO: Use generic macro. */
		if(hVdec->ulSubCarrLoopAccAbs & 0x20000)
		{
			hVdec->ulSubCarrLoopAccAbs = 0x3FFFF - hVdec->ulSubCarrLoopAccAbs;
		}

		/* if subcarrier can not be locked, we will clear subcarrer every 20 Vsync,
		 * so need delay 10 vsyncs to detect CCR status*/
		if((!hVdec->ulSubCarrUnlockTimer) ||
		    (hVdec->ulSubCarrUnlockTimer == BVDC_P_VDEC_ENABLE_CCR_DETECTION))
		{
			/* TODO: Use generic macro for these */
			if((ulSubCarLoopAcc < 260990) && (ulSubCarLoopAcc > 1528))
			{
				*pbCcrEnable = false;
			}
			else if((ulSubCarLoopAcc > 260780) || (ulSubCarLoopAcc < 1410))
			{
				*pbCcrEnable = true;
			}
		}
		else
		{
			*pbCcrEnable = hVdec->bCcrEnable;
		}

		/*PR39265.if non-burst signals, keep old CCR status*/
		if((hVdec->ulFormatChangeTimer >= BVDC_P_VDEC_FORMAT_CHANGE_DELAY_START_SUB) &&
		   (hVdec->ulBlackWhiteCnt == BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH) &&
		   (hVdec->ulVsyncCnt > 20))
		{
			*pbCcrEnable = hVdec->bCcrEnable;
		}

		/*only apply to NTSC format*/
		if(pCurInfo->pFmtInfo->eVideoFmt != BFMT_VideoFmt_eNTSC)
		{
			*pbCcrEnable = false;
		}

		/* Reset index for averaging. */
		hVdec->stSubCarLoopAcc.bUpdated = false;
	}

	return;
}

/***************************************************************************
 *
 */
static void BVDC_P_Vdec_GetColorBurstThreshold_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t ulNoiseLevel;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	ulNoiseLevel = hVdec->stNoiseLevel.ulValue;

	if(ulNoiseLevel >= BVDC_P_VDEC_NOISE_LEVEL_33DB)
	{
		if(hVdec->bVcrHeadSwitch)
		{
			hVdec->ulAutoColorModeOffThreshold  = BVDC_P_VDEC_VCR_ABOVE_33DB_AUTO_COLOR_MODE_OFF;
			hVdec->ulAutoColorModeOnThreshold   = BVDC_P_VDEC_VCR_ABOVE_33DB_AUTO_COLOR_MODE_ON;
		}
		else if(pCurInfo->bIfdInput)
		{
			hVdec->ulAutoColorModeOffThreshold  = BVDC_P_VDEC_RF_ABOVE_33DB_AUTO_COLOR_MODE_OFF;
			hVdec->ulAutoColorModeOnThreshold   = BVDC_P_VDEC_RF_ABOVE_33DB_AUTO_COLOR_MODE_ON;
		}
		else
		{
			hVdec->ulAutoColorModeOffThreshold  = BVDC_P_VDEC_ABOVE_33DB_AUTO_COLOR_MODE_OFF;
			hVdec->ulAutoColorModeOnThreshold   = BVDC_P_VDEC_ABOVE_33DB_AUTO_COLOR_MODE_ON;
		}

		hVdec->ulNoBurstThreshold = BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH_UP_33DB;
		if((pCurInfo->bIfdInput) && (pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_eNTSC))
		{
			hVdec->ulManualColorModeColorOnAmp  = BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_ON_AMP_RF_NTSC;
			hVdec->ulManualColorModeColorOffAmp = BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_OFF_AMP_RF_NTSC;
		}
		else if((pCurInfo->bIfdInput) && (pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_ePAL_M))
		{
			hVdec->ulManualColorModeColorOnAmp  = BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_ON_AMP_RF_PALM;
			hVdec->ulManualColorModeColorOffAmp = BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_OFF_AMP_RF_PALM;
		}
		else if((pCurInfo->bIfdInput) && (pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_ePAL_NC))
		{
			hVdec->ulManualColorModeColorOnAmp  = BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_ON_AMP_RF_PALNC;
			hVdec->ulManualColorModeColorOffAmp = BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_OFF_AMP_RF_PALNC;
		}
		else if((pCurInfo->bIfdInput) && (VIDEO_FORMAT_IS_PAL(pCurInfo->pFmtInfo->eVideoFmt)))
		{
			hVdec->ulManualColorModeColorOnAmp  = BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_ON_AMP_RF;
			hVdec->ulManualColorModeColorOffAmp = BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_OFF_AMP_RF;
		}
		else
		{
			hVdec->ulManualColorModeColorOnAmp  = BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_ON_AMP;
			hVdec->ulManualColorModeColorOffAmp = BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_OFF_AMP;
		}
	}
	else if(ulNoiseLevel >= BVDC_P_VDEC_NOISE_LEVEL_28DB)
	{
		if(hVdec->bVcrHeadSwitch)
		{
			hVdec->ulAutoColorModeOffThreshold  = BVDC_P_VDEC_VCR_28DB_33DB_AUTO_COLOR_MODE_OFF;
			hVdec->ulAutoColorModeOnThreshold   = BVDC_P_VDEC_VCR_28DB_33DB_AUTO_COLOR_MODE_ON;
		}
		else if(pCurInfo->bIfdInput)
		{
			hVdec->ulAutoColorModeOffThreshold  = BVDC_P_VDEC_RF_28DB_33DB_AUTO_COLOR_MODE_OFF;
			hVdec->ulAutoColorModeOnThreshold   = BVDC_P_VDEC_RF_28DB_33DB_AUTO_COLOR_MODE_ON;
		}
		else
		{
			hVdec->ulAutoColorModeOffThreshold  = BVDC_P_VDEC_28DB_33DB_AUTO_COLOR_MODE_OFF;
			hVdec->ulAutoColorModeOnThreshold   = BVDC_P_VDEC_28DB_33DB_AUTO_COLOR_MODE_ON;
		}

		hVdec->ulNoBurstThreshold = BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH_33_28DB;
		if((pCurInfo->bIfdInput) && (VIDEO_FORMAT_IS_PAL(pCurInfo->pFmtInfo->eVideoFmt)))
		{
			hVdec->ulManualColorModeColorOnAmp  = BVDC_P_VDEC_NOISE_LEVEL_28DB_33DB_COLOR_ON_AMP_RF;
			hVdec->ulManualColorModeColorOffAmp = BVDC_P_VDEC_NOISE_LEVEL_28DB_33DB_COLOR_OFF_AMP_RF;
		}
		else
		{
			hVdec->ulManualColorModeColorOnAmp  = BVDC_P_VDEC_NOISE_LEVEL_28DB_33DB_COLOR_ON_AMP;
			hVdec->ulManualColorModeColorOffAmp = BVDC_P_VDEC_NOISE_LEVEL_28DB_33DB_COLOR_OFF_AMP;
		}
	}
	else if(ulNoiseLevel >= BVDC_P_VDEC_NOISE_LEVEL_23DB)
	{
		if(hVdec->bVcrHeadSwitch)
		{
			hVdec->ulAutoColorModeOffThreshold  = BVDC_P_VDEC_VCR_23DB_28DB_AUTO_COLOR_MODE_OFF;
			hVdec->ulAutoColorModeOnThreshold   = BVDC_P_VDEC_VCR_23DB_28DB_AUTO_COLOR_MODE_ON;
		}
		else if(pCurInfo->bIfdInput)
		{
			hVdec->ulAutoColorModeOffThreshold  = BVDC_P_VDEC_RF_23DB_28DB_AUTO_COLOR_MODE_OFF;
			hVdec->ulAutoColorModeOnThreshold   = BVDC_P_VDEC_RF_23DB_28DB_AUTO_COLOR_MODE_ON;
		}
		else
		{
			hVdec->ulAutoColorModeOffThreshold  = BVDC_P_VDEC_28DB_33DB_AUTO_COLOR_MODE_OFF;
			hVdec->ulAutoColorModeOnThreshold   = BVDC_P_VDEC_28DB_33DB_AUTO_COLOR_MODE_ON;
		}

		hVdec->ulNoBurstThreshold = BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH_28_23DB;
		if((pCurInfo->bIfdInput) && (pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_ePAL_M))
		{
			hVdec->ulManualColorModeColorOnAmp  = BVDC_P_VDEC_NOISE_LEVEL_23DB_28DB_COLOR_ON_AMP_RF_PALM;
			hVdec->ulManualColorModeColorOffAmp = BVDC_P_VDEC_NOISE_LEVEL_23DB_28DB_COLOR_OFF_AMP_RF_PALM;
		}
		else if((pCurInfo->bIfdInput) && (VIDEO_FORMAT_IS_PAL(pCurInfo->pFmtInfo->eVideoFmt)))
		{
			hVdec->ulManualColorModeColorOnAmp  = BVDC_P_VDEC_NOISE_LEVEL_23DB_28DB_COLOR_ON_AMP_RF;
			hVdec->ulManualColorModeColorOffAmp = BVDC_P_VDEC_NOISE_LEVEL_23DB_28DB_COLOR_OFF_AMP_RF;
		}
		else
		{
			hVdec->ulManualColorModeColorOnAmp  = BVDC_P_VDEC_NOISE_LEVEL_23DB_28DB_COLOR_ON_AMP;
			hVdec->ulManualColorModeColorOffAmp = BVDC_P_VDEC_NOISE_LEVEL_23DB_28DB_COLOR_OFF_AMP;
		}
	}
	else if(ulNoiseLevel >= BVDC_P_VDEC_NOISE_LEVEL_18DB)
	{
		if(hVdec->bVcrHeadSwitch)
		{
			hVdec->ulAutoColorModeOffThreshold  = BVDC_P_VDEC_VCR_18DB_23DB_AUTO_COLOR_MODE_OFF;
			hVdec->ulAutoColorModeOnThreshold   = BVDC_P_VDEC_VCR_18DB_23DB_AUTO_COLOR_MODE_ON;
		}
		else if(pCurInfo->bIfdInput)
		{
			hVdec->ulAutoColorModeOffThreshold  = BVDC_P_VDEC_RF_18DB_23DB_AUTO_COLOR_MODE_OFF;
			hVdec->ulAutoColorModeOnThreshold   = BVDC_P_VDEC_RF_18DB_23DB_AUTO_COLOR_MODE_ON;
		}
		else
		{
		hVdec->ulAutoColorModeOffThreshold  = BVDC_P_VDEC_18DB_23DB_AUTO_COLOR_MODE_OFF;
		hVdec->ulAutoColorModeOnThreshold   = BVDC_P_VDEC_18DB_23DB_AUTO_COLOR_MODE_ON;
		}

		hVdec->ulNoBurstThreshold = BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH_23_18DB;
		if((pCurInfo->bIfdInput) && (pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_ePAL_M))
		{
			hVdec->ulManualColorModeColorOnAmp  = BVDC_P_VDEC_NOISE_LEVEL_18DB_23DB_COLOR_ON_AMP_RF_PALM;
			hVdec->ulManualColorModeColorOffAmp = BVDC_P_VDEC_NOISE_LEVEL_18DB_23DB_COLOR_OFF_AMP_RF_PALM;
			hVdec->ulNoBurstThreshold = BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH_23_18DB_RF_PALM;
		}
		else if((pCurInfo->bIfdInput) && (VIDEO_FORMAT_IS_PAL(pCurInfo->pFmtInfo->eVideoFmt)))
		{
			hVdec->ulManualColorModeColorOnAmp  = BVDC_P_VDEC_NOISE_LEVEL_18DB_23DB_COLOR_ON_AMP_RF;
			hVdec->ulManualColorModeColorOffAmp = BVDC_P_VDEC_NOISE_LEVEL_18DB_23DB_COLOR_OFF_AMP_RF;
		}
		else
		{
			hVdec->ulManualColorModeColorOnAmp  = BVDC_P_VDEC_NOISE_LEVEL_18DB_23DB_COLOR_ON_AMP;
			hVdec->ulManualColorModeColorOffAmp = BVDC_P_VDEC_NOISE_LEVEL_18DB_23DB_COLOR_OFF_AMP;
		}
	}
	else if(ulNoiseLevel >= BVDC_P_VDEC_NOISE_LEVEL_13DB)
	{
		if(hVdec->bVcrHeadSwitch)
		{
			hVdec->ulAutoColorModeOffThreshold  = BVDC_P_VDEC_VCR_13DB_18DB_AUTO_COLOR_MODE_OFF;
			hVdec->ulAutoColorModeOnThreshold   = BVDC_P_VDEC_VCR_13DB_18DB_AUTO_COLOR_MODE_ON;
		}
		else if(pCurInfo->bIfdInput)
		{
			hVdec->ulAutoColorModeOffThreshold  = BVDC_P_VDEC_RF_13DB_18DB_AUTO_COLOR_MODE_OFF;
			hVdec->ulAutoColorModeOnThreshold   = BVDC_P_VDEC_RF_13DB_18DB_AUTO_COLOR_MODE_ON;
		}
		else
		{
		hVdec->ulAutoColorModeOffThreshold  = BVDC_P_VDEC_13DB_18DB_AUTO_COLOR_MODE_OFF;
		hVdec->ulAutoColorModeOnThreshold   = BVDC_P_VDEC_13DB_18DB_AUTO_COLOR_MODE_ON;
		}

		hVdec->ulNoBurstThreshold = BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH_18_13DB;
		if((pCurInfo->bIfdInput) && (pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_ePAL_M))
		{
			hVdec->ulManualColorModeColorOnAmp  = BVDC_P_VDEC_NOISE_LEVEL_13DB_18DB_COLOR_ON_AMP_RF_PALM;
			hVdec->ulManualColorModeColorOffAmp = BVDC_P_VDEC_NOISE_LEVEL_13DB_18DB_COLOR_OFF_AMP_RF_PALM;
			hVdec->ulNoBurstThreshold = BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH_18_13DB_RF_PALM;
		}
		else if((pCurInfo->bIfdInput) && (VIDEO_FORMAT_IS_PAL(pCurInfo->pFmtInfo->eVideoFmt)))
		{
			hVdec->ulManualColorModeColorOnAmp  = BVDC_P_VDEC_NOISE_LEVEL_13DB_18DB_COLOR_ON_AMP_RF;
			hVdec->ulManualColorModeColorOffAmp = BVDC_P_VDEC_NOISE_LEVEL_13DB_18DB_COLOR_OFF_AMP_RF;
		}
		else
		{
			hVdec->ulManualColorModeColorOnAmp  = BVDC_P_VDEC_NOISE_LEVEL_13DB_18DB_COLOR_ON_AMP;
			hVdec->ulManualColorModeColorOffAmp = BVDC_P_VDEC_NOISE_LEVEL_13DB_18DB_COLOR_OFF_AMP;
		}
	}
	else
	{
		if(ulNoiseLevel >= BVDC_P_VDEC_NOISE_LEVEL_10DB)
		{
			if(hVdec->bVcrHeadSwitch)
			{
				hVdec->ulAutoColorModeOffThreshold  = BVDC_P_VDEC_VCR_10DB_13DB_AUTO_COLOR_MODE_OFF;
				hVdec->ulAutoColorModeOnThreshold   = BVDC_P_VDEC_VCR_10DB_13DB_AUTO_COLOR_MODE_ON;
			}
			else if(pCurInfo->bIfdInput)
			{
				hVdec->ulAutoColorModeOffThreshold  = BVDC_P_VDEC_RF_10DB_13DB_AUTO_COLOR_MODE_OFF;
				hVdec->ulAutoColorModeOnThreshold   = BVDC_P_VDEC_RF_10DB_13DB_AUTO_COLOR_MODE_ON;
			}
			else
			{
				hVdec->ulAutoColorModeOffThreshold  = BVDC_P_VDEC_10DB_13DB_AUTO_COLOR_MODE_OFF;
				hVdec->ulAutoColorModeOnThreshold   = BVDC_P_VDEC_10DB_13DB_AUTO_COLOR_MODE_ON;
			}
		}
		else
		{
			if(hVdec->bVcrHeadSwitch)
			{
				hVdec->ulAutoColorModeOffThreshold  = BVDC_P_VDEC_VCR_BELOW_10DB_AUTO_COLOR_MODE_OFF;
				hVdec->ulAutoColorModeOnThreshold   = BVDC_P_VDEC_VCR_BELOW_10DB_AUTO_COLOR_MODE_ON;
			}
			else if(pCurInfo->bIfdInput)
			{
				hVdec->ulAutoColorModeOffThreshold  = BVDC_P_VDEC_RF_BELOW_10DB_AUTO_COLOR_MODE_OFF;
				hVdec->ulAutoColorModeOnThreshold   = BVDC_P_VDEC_RF_BELOW_10DB_AUTO_COLOR_MODE_ON;
			}
			else
			{
				hVdec->ulAutoColorModeOffThreshold  = BVDC_P_VDEC_BELOW_10DB_AUTO_COLOR_MODE_OFF;
				hVdec->ulAutoColorModeOnThreshold   = BVDC_P_VDEC_BELOW_10DB_AUTO_COLOR_MODE_ON;
			}
		}

		hVdec->ulNoBurstThreshold = BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH_DOWN_13DB;
		if((pCurInfo->bIfdInput) && (pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_ePAL_M))
		{
			hVdec->ulManualColorModeColorOnAmp  = BVDC_P_VDEC_NOISE_LEVEL_DOWN_13DB_COLOR_ON_AMP_RF_PALM;
			hVdec->ulManualColorModeColorOffAmp = BVDC_P_VDEC_NOISE_LEVEL_DOWN_13DB_COLOR_OFF_AMP_RF_PALM;
			hVdec->ulNoBurstThreshold = BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH_DOWN_13DB_RF_PALM;
		}
		else if((pCurInfo->bIfdInput) && (VIDEO_FORMAT_IS_PAL(pCurInfo->pFmtInfo->eVideoFmt)))
		{
			hVdec->ulManualColorModeColorOnAmp  = BVDC_P_VDEC_NOISE_LEVEL_DOWN_13DB_COLOR_ON_AMP_RF;
			hVdec->ulManualColorModeColorOffAmp = BVDC_P_VDEC_NOISE_LEVEL_DOWN_13DB_COLOR_OFF_AMP_RF;
		}
		else
		{
			hVdec->ulManualColorModeColorOnAmp  = BVDC_P_VDEC_NOISE_LEVEL_DOWN_13DB_COLOR_ON_AMP;
			hVdec->ulManualColorModeColorOffAmp = BVDC_P_VDEC_NOISE_LEVEL_DOWN_13DB_COLOR_OFF_AMP;
		}
	}

	return;
}

/***************************************************************************
 * PR23484:
 *
*/
static void BVDC_P_Vdec_UpdateNoiseLevel_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  const BVDC_P_Source_Info        *pCurInfo,
	  bool                            *pbNoisy,
	  bool                            *pbPalSeparation )
{
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	if(hVdec->stNoiseLevel.bUpdated)
	{
		if((hVdec->bVideoDetected) &&
		   ((hVdec->eMacrovisionType == BVDC_MacrovisionType_eNoProtection) ||
		    (pCurInfo->bIfdInput)))
		{
			uint32_t ulNoiseLevel = hVdec->stNoiseLevel.ulValue;

			/* COLOR_MODE base on noise */
			if(!(BVDC_P_EQ_DELTA(ulNoiseLevel, hVdec->ulNoiseHistory, BVDC_P_VDEC_NOISE_LEVEL_TOGGLING_THRESHOLD)))
			{
				BVDC_P_Vdec_GetColorBurstThreshold_isr(hVdec, pCurInfo);
				hVdec->ulNoiseHistory = ulNoiseLevel;
			}

			if(hVdec->stColorBurstAmp.ulTheshold == BVDC_P_VDEC_COLORBURST_AMP_FAST_COUNT)
			{
				if(ulNoiseLevel < 0x50) /* 80 */
				{
					hVdec->stColorBurstAmp.ulTheshold = BVDC_P_VDEC_COLORBURST_AMP_SLOW_COUNT;
				}
				else if(ulNoiseLevel < 0x70) /* 112 */
				{
					hVdec->stColorBurstAmp.ulTheshold = BVDC_P_VDEC_COLORBURST_AMP_MDSL_COUNT;
				}
				else if(ulNoiseLevel < 0x9E) /* 158 */
				{
					hVdec->stColorBurstAmp.ulTheshold = BVDC_P_VDEC_COLORBURST_AMP_NORM_COUNT;
				}
			}
			else if(hVdec->stColorBurstAmp.ulTheshold == BVDC_P_VDEC_COLORBURST_AMP_NORM_COUNT)
			{
				if(ulNoiseLevel > 0xA1) /* 161 */
				{
					hVdec->stColorBurstAmp.ulTheshold = BVDC_P_VDEC_COLORBURST_AMP_FAST_COUNT;
				}
				else if(ulNoiseLevel < 0x70) /* 112 */
				{
					hVdec->stColorBurstAmp.ulTheshold = BVDC_P_VDEC_COLORBURST_AMP_MDSL_COUNT;
				}
				else if(ulNoiseLevel < 0x50) /* 80 */
				{
					hVdec->stColorBurstAmp.ulTheshold = BVDC_P_VDEC_COLORBURST_AMP_SLOW_COUNT;
				}
			}
			else if(hVdec->stColorBurstAmp.ulTheshold == BVDC_P_VDEC_COLORBURST_AMP_MDSL_COUNT)
			{
				if(ulNoiseLevel > 0xA1) /* 161 */
				{
					hVdec->stColorBurstAmp.ulTheshold = BVDC_P_VDEC_COLORBURST_AMP_FAST_COUNT;
				}
				else if(ulNoiseLevel > 0x7D) /* 125 */
				{
					hVdec->stColorBurstAmp.ulTheshold = BVDC_P_VDEC_COLORBURST_AMP_NORM_COUNT;
				}
				else if(ulNoiseLevel < 0x50) /* 80 */
				{
					hVdec->stColorBurstAmp.ulTheshold = BVDC_P_VDEC_COLORBURST_AMP_SLOW_COUNT;
				}
			}
			else
			{
				if(ulNoiseLevel > 0xA1) /* 161 */
				{
					hVdec->stColorBurstAmp.ulTheshold = BVDC_P_VDEC_COLORBURST_AMP_FAST_COUNT;
				}
				else if(ulNoiseLevel > 0x7D) /* 125 */
				{
					hVdec->stColorBurstAmp.ulTheshold = BVDC_P_VDEC_COLORBURST_AMP_NORM_COUNT;
				}
				else if(ulNoiseLevel > 0x55) /*85*/
				{
					hVdec->stColorBurstAmp.ulTheshold = BVDC_P_VDEC_COLORBURST_AMP_MDSL_COUNT;
				}
			}
		}

		if(hVdec->ulVsyncCnt < BVDC_P_VDEC_NOISE_LEVEL_DELAY_TOGGLE)
		{
			if(hVdec->stNoiseLevel.ulValue > (BVDC_P_VDEC_NOISE_LEVEL_NOISY_TURN_OFF + BVDC_P_VDEC_NOISE_LEVEL_NOISY_TURN_ON) / 2)
			{
				*pbNoisy = false;
			}
			else
			{
				*pbNoisy = true;
			}
		}
		else
		{
			/* Turn on noisy when noise level is less than turn on threshold. And
			 * turn off noisy when noise level is larger than turn off threshold. */
			if(hVdec->bNoisy)
			{
				if(hVdec->stNoiseLevel.ulValue > BVDC_P_VDEC_NOISE_LEVEL_NOISY_TURN_OFF)
				{
					/* Turn off noisy */
					*pbNoisy = false;
					BDBG_MSG(("[NOISY]:bNoisy:[%d]->[%d], noise level:[%d]",
						hVdec->bNoisy, *pbNoisy,
						hVdec->stNoiseLevel.ulValue));
				}
			}
			else
			{
				if(hVdec->stNoiseLevel.ulValue < BVDC_P_VDEC_NOISE_LEVEL_NOISY_TURN_ON)
				{
					/* Turn on noisy */
					*pbNoisy = true;
					BDBG_MSG(("[NOISY]:bNoisy:[%d]->[%d], noise level:[%d]",
						hVdec->bNoisy, *pbNoisy,
						hVdec->stNoiseLevel.ulValue));
				}
			}
		}

		if(pCurInfo->bAutoFmtDetection)
		{
			if(hVdec->ulVsyncCnt > hVdec->ulAutoDetectionThreshold)
			{
				/*Auto detection for RF signal*/
				if(hVdec->bDisableRfAutoDetection)
				{
					if(hVdec->stNoiseLevel.ulValue > BVDC_P_VDEC_NOISE_AUTO_DETECTION_TURN_ON)
					{
						hVdec->bDisableRfAutoDetection = false;
						BDBG_MSG(("Enable auto detection. Noise[%d]", hVdec->stNoiseLevel.ulValue));
					}
				}
				else
				{
					if(hVdec->stNoiseLevel.ulValue < BVDC_P_VDEC_NOISE_AUTO_DETECTION_TURN_OFF)
					{
						hVdec->bDisableRfAutoDetection = true;
						BDBG_MSG(("Disable auto detection. Noise[%d]", hVdec->stNoiseLevel.ulValue));
					}
				}
			}
		}
		/* Reset index for averaging. */
		hVdec->stNoiseLevel.bUpdated = false;
	}

#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_6)
	*pbPalSeparation = true;/* unused */
#else
	/*PR33797: Disable PAL Separation*/
	if((hVdec->bHPllLocked) &&
		(hVdec->ulVsyncCnt > 5) &&
		(VIDEO_FORMAT_IS_PAL(pCurInfo->pFmtInfo->eVideoFmt)) &&
		(!pCurInfo->bCompInput) &&
		(!pCurInfo->bPcInput))
	{
		if(hVdec->bPalSeparation)
		{
			if(hVdec->stNoiseLevel.ulValue < BVDC_P_VDEC_NOISE_PAL_SEPARATION_OFF)
			{
				*pbPalSeparation = false;
			}
		}
		else
		{
			if(hVdec->stNoiseLevel.ulValue > BVDC_P_VDEC_NOISE_PAL_SEPARATION_ON)
			{
				*pbPalSeparation = true;
			}
		}
	}
	else if(VIDEO_FORMAT_IS_SECAM(pCurInfo->pFmtInfo->eVideoFmt))
	{
		*pbPalSeparation = true;
	}
	else
	{
		*pbPalSeparation = false;
	}

	/* During the switch channel for bad signal, ANR is very easy to receive two
	 * successive short field which will hung up ANR. The workaround is during
	 * the switch channel, monitor the ANR status, if get short field, Disable
	 * PAL separation. */
	if((hVdec->bPalSeparation) &&
	   (hVdec->ulVsyncCnt < 50) &&
	   (hVdec->stNoiseLevel.ulValue < 80) &&
	   (hVdec->hSource->stAnrStatus.bEnaError))
	{
		*pbPalSeparation = false;
	}
#endif

	/* PR43635 : Support manual VCR and Noisy mode */
	if(BVDC_Vdec_NoiseMode_eOff == pCurInfo->stVdecSettings.eNoiseMode)
	{
		*pbNoisy = false;
	}
	else if(BVDC_Vdec_NoiseMode_eOn == pCurInfo->stVdecSettings.eNoiseMode)
	{
		*pbNoisy = true;
	}

	return;
}


/***************************************************************************
 * This function will update some VDEC register that need dynamic change to
 * fix related issues.
 * PR28380: Fix for Full White Field.
 * PR31869: snow noise blink between normal snow noise and black screen
 * PR31907: detects wrong field mode when input source is component 720p
 * from some kind of settop box
 */
static void BVDC_P_Vdec_UpdateDynamicRegisters_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	uint32_t ulReg;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	/* PR31907: Turn on by marking! */
	if(((BFMT_VideoFmt_e720p == pCurInfo->pFmtInfo->eVideoFmt) ||
	    (BFMT_VideoFmt_e720p_50Hz== pCurInfo->pFmtInfo->eVideoFmt)) && (pCurInfo->bCompInput))
	{
		bool bFieldModeErr = (
			(hVdec->bHPllLocked) &&
			(hVdec->ulVsyncCnt > BVDC_P_VDEC_HPLL_TOGGLING_COLOR_DELAY) &&
			(BVDC_P_COMPARE_FIELD_NAME(hVdec->ulFeStatusReg, VDEC_FE_0_FE_STATUS, FIELD_MODE, INTERLACED)));

		if((bFieldModeErr != hVdec->bFieldModeErr) || (bFieldModeErr))
		{
			hVdec->bFieldModeErr = bFieldModeErr;
			ulReg = BREG_Read32_isr(hVdec->hReg, BCHP_VDEC_FE_0_FIELD_SYNC + hVdec->ulFeOffset);
			ulReg &= ~(
				BCHP_MASK(VDEC_FE_0_FIELD_SYNC, TYPE));
			ulReg |=  (
				(bFieldModeErr)
				? BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TYPE, BY_MARKING)
				: BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TYPE, BY_POSITION));
			BREG_Write32_isr(hVdec->hReg, BCHP_VDEC_FE_0_FIELD_SYNC + hVdec->ulFeOffset, ulReg);
		}
	}

	return;
}

/***************************************************************************
 * PR23496 Kill Color
 */
static void BVDC_P_Vdec_UpdateColorPresent_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  bool                            *pbChromaLock,
	  bool                            *pbChromaPresent,
	  bool                            *pbManualColorMode,
	  bool                            *pbClearSubCarAccum,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	/*Chroma should be killed if the burst level of incoming signal is too low*/
	*pbChromaPresent =
		(hVdec->ulColorBurstGain < BVDC_P_ACGC_GAIN_MAX) ||
		(hVdec->stColorBurstAmp.ulValue > hVdec->ulManualColorModeColorOnAmp) ||
		((hVdec->stColorBurstAmp.ulValue > hVdec->ulManualColorModeColorOffAmp) &&
		 (hVdec->bChromaPresent));

	/* 1.No chroma if burst too low.!
	   2.No chroma if delta/amp too big.*/
	if((*pbChromaPresent) &&
	   (((hVdec->ulColorBurstGain > BVDC_P_ACGC_GAIN_NOMINAL) &&
	    (hVdec->stColorBurstAmp.ulValue < 1))||
	   ((!hVdec->bNoisy) &&
	    (hVdec->ulColorBurstGain >= BVDC_P_ACGC_GAIN_MAX) &&
	    (hVdec->stColorBurstAmp.ulValue < 20) &&
	    (hVdec->ulColorBurstAmpDelta >= hVdec->stColorBurstAmp.ulValue * 4))))
	{
		*pbChromaPresent = false;
	}

	/*In very noise condition, kill color
	  If Hpll unlocked several Vsyncs, kill color*/
	if(((!(*pbChromaPresent)) ||
	   (hVdec->ulHpllUnLock >= BVDC_P_VDEC_HPLL_TOGGLING_COLOR_DELAY) ||
	   (hVdec->stNoiseLevel.ulValue < BVDC_P_VDEC_NOISE_LEVEL_SET_MANUAL_COLOR_MODE_BW)) &&
	   (!hVdec->bVcrHeadSwitch))
	{
		*pbManualColorMode = false;
	}
	else
	{
		if(hVdec->stNoiseLevel.ulValue > BVDC_P_VDEC_NOISE_LEVEL_SET_MANUAL_COLOR_MODE_COLOR)
		{
			*pbManualColorMode = true;
		}
	}

	/* Chroma lock? */
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
	if(VIDEO_FORMAT_IS_SECAM(pCurInfo->pFmtInfo->eVideoFmt))
	{
		*pbChromaLock = true;
	}
	else
#endif
	{
		if(hVdec->bHPllLocked)
		{
			*pbChromaLock = BCHP_GET_FIELD_DATA(hVdec->ulCdStatusReg,
				VDEC_CD_0_CD_STATUS, SUBCARRIER_LOOP_LOCK);
		}
		else
		{
			*pbChromaLock = false;
		}
	}

	/* (1) In case of the subcaraccum value is too big AND
	 * color burst gain is larger than maximum.
	 * then subcarrier loop need be cleared.   May need to check for
	 * case where SubCar is pegged, and need to recover.
	 * (2) If subcarrier continue unlocked for 20 Vsync.
	 * (3) Everytimes HPLL/ADC/Format change, clear subcarrier for PAL format. */
	*pbClearSubCarAccum = ((
		(hVdec->ulSubCarrLoopAccAbs > BVDC_P_VDEC_CLEAR_SUBCARRIER_ACC_THRESHHOLD) &&
		(hVdec->ulColorBurstGain >= BVDC_P_ACGC_GAIN_MAX)) ? true : false) ||
		(hVdec->ulSubCarrUnlockTimer == BVDC_P_VDEC_CLEAR_SUBCARRIER_TIMER) ||
		(hVdec->ulVsyncCnt == 1);

	if(pCurInfo->bCvbsInput || pCurInfo->bSvidInput || pCurInfo->bIfdInput)
	{
		if(!hVdec->bHPllLocked)
		{
			*pbClearSubCarAccum = false;
		}
	}

#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
	if(VIDEO_FORMAT_IS_SECAM(pCurInfo->pFmtInfo->eVideoFmt))
	{
		*pbClearSubCarAccum =
			((hVdec->ulVsyncCnt == 1) &&
			 (VIDEO_FORMAT_IS_SECAM(pCurInfo->pFmtInfo->eVideoFmt)));
	}
#endif

	/*Every 20 vsyncs, check if need clear subcarrier*/
	if(((hVdec->ulSubCarrLoopAccAbs >= BVDC_P_VDEC_SUBCARRIER_LOOP_ACC_UNLOCK_NORMAL) && (!hVdec->bVcrHeadSwitch)) ||
	    ((hVdec->ulSubCarrLoopAccAbs >= BVDC_P_VDEC_SUBCARRIER_LOOP_ACC_UNLOCK_VCR) && (hVdec->bVcrHeadSwitch))	)
	{
		if(hVdec->ulSubCarrUnlockTimer < BVDC_P_VDEC_CLEAR_SUBCARRIER_TIMER)
		{
			hVdec->ulSubCarrUnlockTimer++;
		}
		else
		{
			hVdec->ulSubCarrUnlockTimer = 0;
		}
	}
	else
	{
		hVdec->ulSubCarrUnlockTimer = 0;
	}

#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
	if(VIDEO_FORMAT_IS_SECAM(pCurInfo->pFmtInfo->eVideoFmt))
	{
		if(BCHP_GET_FIELD_DATA(hVdec->ulSecamSyncStatusReg, VDEC_CD_0_SECAM_CSYNC_STATUS, COLOR_SYNC_LOCK))
		{
			hVdec->ulSecamSyncUnlockTimer = 0;
		}
		else
		{
			hVdec->ulSecamSyncUnlockTimer++;
		}

		if(hVdec->ulVsyncCnt == 1)
		{
			hVdec->ulSecamSyncUnlockTimer = 0;
		}
	}
#endif

	/* To be used in ANR's MAAF filter.
	 *
	 * field count   DR_DB_BF  DR_DB_TF                 first line
	 * bot(n)           0         0                 Db (first line = 0)
	 * top(n+1)         0         1                 Dr (first line = 1)
	 * bot(n+1)         1         1                 Dr (first line = 1)
	 * top(n+2)         1         0                 Db (first line = 0) */
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
	hVdec->bDrTf = (
		BCHP_GET_FIELD_DATA(hVdec->ulSecamSyncStatusReg, VDEC_CD_0_SECAM_CSYNC_STATUS, DR_DB_TF));
#endif

	/*PR33012: When input NTSC CVBS signal with copy protection, no color displayed */
	if(*pbClearSubCarAccum)
	{
		hVdec->ulClearSubcarrierCnt = 0;
	}
	else if(hVdec->ulClearSubcarrierCnt < BVDC_P_VDEC_CLEAR_SUBCARRIER_THRESH)
	{
		hVdec->ulClearSubcarrierCnt ++;
	}

	if(*pbChromaLock)
	{
		hVdec->ulChromaUnlockCnt = 0;
	}
	else if(hVdec->ulChromaUnlockCnt < BVDC_P_VDEC_CHROMA_UNLOCK_THRESH)
	{
		hVdec->ulChromaUnlockCnt ++;
	}

	if(hVdec->stAutoAcgcGain.bUpdated)
	{
		hVdec->stAutoAcgcGain.bUpdated = false;
	}

	/*PR33011:If CVBS signal doesn't contain burst signal, need S/W assume a defult format	*/
	if(pCurInfo->bCvbsInput || pCurInfo->bSvidInput || pCurInfo->bIfdInput)
	{
		/*PR36051:RF, check color burst on/off*/
		if(!hVdec->bManualAcgc)
		{
			/*when use auto acgc gain and when gain has reach to some value, think the signal has no burst*/
			if(hVdec->stAutoAcgcGain.ulValue < BVDC_P_VDEC_AUTO_MAX_ACGC_GAIN)
			{
				hVdec->ulBlackWhiteCnt = 0;
			}
			else if(hVdec->ulBlackWhiteCnt < BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH)
			{
				hVdec->ulBlackWhiteCnt++;
			}
		}
		else
		{
			if((hVdec->stColorBurstAmp.ulValue > hVdec->ulNoBurstThreshold) &&
			   (*pbManualColorMode))
			{
				hVdec->ulBlackWhiteCnt = 0;
			}
			else if(hVdec->ulBlackWhiteCnt < BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH)
			{
				hVdec->ulBlackWhiteCnt++;
			}
		}
	}

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_Vdec_UpdateColorBurst_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  uint32_t                        *pulCrGain,
	  uint32_t                        *pulCbGain,
	  uint32_t                        *pulColorBurstGain,
	  const BVDC_P_Source_Info        *pCurInfo )
{
	int32_t lSmlCompare;
	int32_t lFinCompare;
	int32_t lMslCompare;
	int32_t lMedCompare;
	int32_t lBigCompare;
	int32_t lLrgCompare;
	int32_t lBurstDiff;
	uint32_t ulColorBurstAmp;
	uint32_t ulCrGain;
	uint32_t ulCbGain;
#if (BVDC_P_ACGC_CSC_GAIN)
	uint32_t ulCscGain;
	uint32_t ulBurstRatio;
#endif

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	/* Read burst and average!
	 * Do we really need to average over a period of times, and
	 * different average interval?  No!  3563A0 ++ already does averaging
	 * internally, so we're only need to read it now.  Or 1 in the loop. */
	ulColorBurstAmp = hVdec->stColorBurstAmp.ulValue;

	/* Burst difference to see how much we need to adjust gain */
	lBurstDiff = (int32_t)(ulColorBurstAmp - hVdec->ulColorBurstRef);

	/* Small & Big compare range. */
	lSmlCompare = BVDC_P_ACGC_SML_CMP_THRESHOLD;
	lFinCompare = BVDC_P_ACGC_MSL_FIN_THRESHOLD;
	lMslCompare = BVDC_P_ACGC_MSL_CMP_THRESHOLD;
	lMedCompare = BVDC_P_ACGC_MED_CMP_THRESHOLD;
	lBigCompare = BVDC_P_ACGC_BIG_CMP_THRESHOLD;
	lLrgCompare = BVDC_P_ACGC_LRG_CMP_THRESHOLD;
	if(hVdec->stColorBurstAmp.bUpdated)
	{
		/* Large */
		if((lBurstDiff < (-lLrgCompare)) &&
		   (*pulColorBurstGain  < (BVDC_P_ACGC_GAIN_MAX - BVDC_P_ACGC_GAIN_DELTA)))
		{
			BVDC_P_ACGC_INCREASE_MANUAL_GAIN(
				*pulColorBurstGain, BVDC_P_ACGC_LRG_BURST_INC, BVDC_P_ACGC_GAIN_MAX);
		}
		else if((lBurstDiff > lLrgCompare) &&
		        (*pulColorBurstGain  > (BVDC_P_ACGC_GAIN_MIN + BVDC_P_ACGC_GAIN_DELTA)))
		{
			BVDC_P_ACGC_DECREASE_MANUAL_GAIN(
				*pulColorBurstGain, BVDC_P_ACGC_LRG_BURST_INC, BVDC_P_ACGC_GAIN_MIN);
		}
		/* Big */
		if((lBurstDiff < (-lBigCompare)) &&
		   (*pulColorBurstGain  < (BVDC_P_ACGC_GAIN_MAX - BVDC_P_ACGC_GAIN_DELTA)))
		{
			BVDC_P_ACGC_INCREASE_MANUAL_GAIN(
				*pulColorBurstGain, BVDC_P_ACGC_BIG_BURST_INC, BVDC_P_ACGC_GAIN_MAX);
		}
		else if((lBurstDiff > lBigCompare) &&
		        (*pulColorBurstGain  > (BVDC_P_ACGC_GAIN_MIN + BVDC_P_ACGC_GAIN_DELTA)))
		{
			BVDC_P_ACGC_DECREASE_MANUAL_GAIN(
				*pulColorBurstGain, BVDC_P_ACGC_BIG_BURST_INC, BVDC_P_ACGC_GAIN_MIN);
		}
		/* Medium */
		else if((lBurstDiff < (-lMedCompare)) &&
		        (*pulColorBurstGain  < BVDC_P_ACGC_GAIN_MAX))
		{
			BVDC_P_ACGC_INCREASE_MANUAL_GAIN(
				*pulColorBurstGain, BVDC_P_ACGC_MED_BURST_INC, BVDC_P_ACGC_GAIN_MAX);
		}
		else if((lBurstDiff > lMedCompare) &&
		         (*pulColorBurstGain  > (BVDC_P_ACGC_GAIN_MIN + BVDC_P_ACGC_MED_BURST_INC)))
		{
			BVDC_P_ACGC_DECREASE_MANUAL_GAIN(
				*pulColorBurstGain, BVDC_P_ACGC_MED_BURST_INC, BVDC_P_ACGC_GAIN_MIN);
		}
		/* Medium/Small */
		else if((lBurstDiff < (-lMslCompare)) &&
		        (*pulColorBurstGain  < BVDC_P_ACGC_GAIN_MAX))
		{
			BVDC_P_ACGC_INCREASE_MANUAL_GAIN(
				*pulColorBurstGain, BVDC_P_ACGC_MSL_BURST_INC, BVDC_P_ACGC_GAIN_MAX);
		}
		else if((lBurstDiff > lMslCompare) &&
		        (*pulColorBurstGain  > (BVDC_P_ACGC_GAIN_MIN + BVDC_P_ACGC_MSL_BURST_INC)))
		{
			BVDC_P_ACGC_DECREASE_MANUAL_GAIN(
				*pulColorBurstGain, BVDC_P_ACGC_MSL_BURST_INC, BVDC_P_ACGC_GAIN_MIN);
		}
		/* Fine*/
		else if((lBurstDiff < (-lFinCompare)) &&
		        (*pulColorBurstGain  < BVDC_P_ACGC_GAIN_MAX))
		{
			BVDC_P_ACGC_INCREASE_MANUAL_GAIN(
				*pulColorBurstGain, BVDC_P_ACGC_FIN_BURST_INC, BVDC_P_ACGC_GAIN_MAX);
		}
		else if((lBurstDiff > lFinCompare) &&
		        (*pulColorBurstGain  > (BVDC_P_ACGC_GAIN_MIN + BVDC_P_ACGC_FIN_BURST_INC)))
		{
			BVDC_P_ACGC_DECREASE_MANUAL_GAIN(
				*pulColorBurstGain, BVDC_P_ACGC_FIN_BURST_INC, BVDC_P_ACGC_GAIN_MIN);
		}
		else if((!pCurInfo->bIfdInput) && (hVdec->stColorBurstAmp.ulTheshold != BVDC_P_VDEC_COLORBURST_AMP_SLOW_COUNT))
		{
			/* Small: for non-noisy signal only */
			if((lBurstDiff < (-lSmlCompare)) &&
			   (*pulColorBurstGain  < BVDC_P_ACGC_GAIN_MAX))
			{
				BVDC_P_ACGC_INCREASE_MANUAL_GAIN(
					*pulColorBurstGain, BVDC_P_ACGC_SML_BURST_INC, BVDC_P_ACGC_GAIN_MAX);
			}
			else if((lBurstDiff > lSmlCompare) &&
			        (*pulColorBurstGain  > (BVDC_P_ACGC_GAIN_MIN + BVDC_P_ACGC_SML_BURST_INC)))
			{
				BVDC_P_ACGC_DECREASE_MANUAL_GAIN(
					*pulColorBurstGain, BVDC_P_ACGC_SML_BURST_INC, BVDC_P_ACGC_GAIN_MIN);
			}
		}

#if (BVDC_P_ACGC_CSC_GAIN)
		if((hVdec->stColorBurstAmp.ulValue) &&
		   (hVdec->stColorBurstAmp.ulValue <
		   (hVdec->ulColorBurstRef - BVDC_P_ACGC_REF_TOLERANCE)) &&
		   (*pulColorBurstGain >= BVDC_P_ACGC_GAIN_MAX) &&
		   (VIDEO_FORMAT_IS_NTSC(pCurInfo->pFmtInfo->eVideoFmt))) /* Only NTSC for now! */
		{
				/* Burst ratio equals to the reference burst level divided by current
				* burst level. We need float calculation. The precision is 11 bits.*/
				ulBurstRatio = 1;
				ulBurstRatio =
					(hVdec->ulColorBurstRef << BVDC_P_CSC_VDEC_CX_F_BITS) / hVdec->stColorBurstAmp.ulValue;
				ulCscGain = (ulBurstRatio < (BVDC_P_ACGC_CSC_GAIN_MAX << BVDC_P_CSC_VDEC_CX_F_BITS))?
					ulBurstRatio : (BVDC_P_ACGC_CSC_GAIN_MAX << BVDC_P_CSC_VDEC_CX_F_BITS);

				/* Round the Cr gain and truncate it. */
				ulCrGain = (ulCscGain * BVDC_P_ACGC_CR_DEF_GAIN) >> BVDC_P_CSC_VDEC_CX_F_BITS;
				ulCrGain = (ulCrGain < BVDC_P_ACGC_CR_DEF_GAIN) ?
					BVDC_P_ACGC_CR_DEF_GAIN : ulCrGain;

				/* Round the Cb gain and truncate it. */
				ulCbGain = (ulCscGain * BVDC_P_ACGC_CB_DEF_GAIN) >> BVDC_P_CSC_VDEC_CX_F_BITS;
				ulCbGain = (ulCbGain < BVDC_P_ACGC_CB_DEF_GAIN) ?
					BVDC_P_ACGC_CB_DEF_GAIN : ulCbGain;
		}
		else
		{
			ulCrGain = BVDC_P_ACGC_CR_DEF_GAIN;
			ulCbGain = BVDC_P_ACGC_CB_DEF_GAIN;
		}
#else
		ulCrGain = BVDC_P_ACGC_CR_DEF_GAIN;
		ulCbGain = BVDC_P_ACGC_CB_DEF_GAIN;
#endif

		*pulCrGain = ulCrGain;
		*pulCbGain = ulCbGain;
		hVdec->stColorBurstAmp.bUpdated = false;

		/* Color Burst Updated to notify callback so application can use
		 * this value to do its manual acgc. */
		hVdec->bColorBurstAmpUpdated = true;
	}
	return;
}

/***************************************************************************
 *Bilevel for component HD format
 */
static void BVDC_P_Vdec_UpdateBilevel_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  const BVDC_P_Source_Info        *pCurInfo,
	  bool                            *pbHdBilevelDetected )
{
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	/*PR40449: Workaround for special 1080i/720p component streams with bi-level
	 * Hsync instead of tri-level.*/
	if((pCurInfo->bCompInput) &&
	   (pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_e1080i ||
	    pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_e1080i_50Hz ||
	    pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_e1080p ||
	    pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_e1080p_50Hz ||
	    pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_e720p ||
	    pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_e720p_50Hz))
	{
		if(hVdec->bNoInputSignal)
		{
			*pbHdBilevelDetected    = false;
			hVdec->bFieldAvgErr     = false;
			hVdec->ulFieldAvgErrCnt = 0;
		}
		else if(hVdec->bFieldAvgErr)
		{
			*pbHdBilevelDetected    = true;
			hVdec->bFieldAvgErr     = false;
			hVdec->ulFieldAvgErrCnt = 0;
		}
		else if((hVdec->ulHdBilevelSwitchCnt > 40) && (hVdec->ulHpllUnLock > 35))
		{
			*pbHdBilevelDetected = !hVdec->bHdBilevelDetected;
			hVdec->ulHdBilevelSwitchCnt = 0;
		}
		else
		{
			/* nothing to do */
		}
	}
	else
	{
		*pbHdBilevelDetected = false;
	}
	return;
}
#endif /* #if(BVDC_P_USE_WORK_AROUNDS) */

/***************************************************************************
 *
 */
static const BFMT_VideoFmt s_aeDefaultFormats[] =
{
	/*60Hz*/
	BFMT_VideoFmt_eNTSC,                   /* 480i, NTSC-M for North America */
	BFMT_VideoFmt_ePAL_M,                  /* 525-lines (Brazil) */
#if BVDC_P_VDEC_HAS_NTSC443_NO_PAL60
	BFMT_VideoFmt_eNTSC_443,               /* NTSC-443 */
	BFMT_VideoFmt_ePAL_60,                 /* 60Hz PAL */
#else
	BFMT_VideoFmt_ePAL_60,                 /* 60Hz PAL */
	BFMT_VideoFmt_eNTSC_443,               /* NTSC-443 */
#endif
	/*50Hz*/
	BFMT_VideoFmt_ePAL_G,                  /* Europe */
	BFMT_VideoFmt_ePAL_NC,                 /* N combination (Argentina) */
	BFMT_VideoFmt_eSECAM_L                 /* France */
};


/***************************************************************************
 *
 */
static BFMT_VideoFmt BVDC_P_Vdec_ConvertToInternalFmt
	( BFMT_VideoFmt                    eCustomFmt )
{
	BFMT_VideoFmt eInternalFmt;
	eInternalFmt = eCustomFmt;
	if(eCustomFmt == BFMT_VideoFmt_eNTSC_J)
	{
		eInternalFmt = BFMT_VideoFmt_eNTSC;
	}
	else if(VIDEO_FORMAT_IS_PAL_443(eCustomFmt))
	{
		eInternalFmt = BFMT_VideoFmt_ePAL_G;
	}
	else if(VIDEO_FORMAT_IS_SECAM(eCustomFmt))
	{
		eInternalFmt = BFMT_VideoFmt_eSECAM_L;
	}
	return eInternalFmt;
}


/***************************************************************************
 *
 */
static bool BVDC_P_Vdec_LookUpFmt
	( const BFMT_VideoFmt             *peFmtTable,
	  uint32_t                         ulNumFmt,
	  BFMT_VideoFmt                    eFmt )
{
	uint32_t i = 0;
	if(ulNumFmt == 0)
	{
		return true;
	}
	else
	{
		for(i = 0; i < ulNumFmt; i++)
		{
			if(eFmt == BVDC_P_Vdec_ConvertToInternalFmt(peFmtTable[i]))
			{
				return true;
			}
		}
		return false;
	}
}

/***************************************************************************
 *
 */
static void BVDC_P_Vdec_UpdateAutoFormat_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  const BVDC_P_Source_Info        *pCurInfo,
	  const BFMT_VideoInfo           **ppNewFmtInfo,
	  const BVDC_P_FdtEntry          **ppFdEntry,
	  BVDC_P_Source_DirtyBits          *pCurDirty,
	  bool                            *pbNoInput )
{
	bool bNewFmt;
	uint32_t ulFmtId;
	uint32_t ulClampSel;
	BFMT_VideoFmt eNewVideoFmt = pCurInfo->pFmtInfo->eVideoFmt;
	BFMT_VideoFmt eCurVideoFmt = pCurInfo->pFmtInfo->eVideoFmt;
	uint32_t ulFmtIndex = 0;
	uint32_t i = 0;
	uint32_t ulNumFormats = 0;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	ulClampSel = hVdec->ulClampSel;

	/***1: read FD status*/
	bNewFmt = BCHP_GET_FIELD_DATA(
		hVdec->ulFdFormatReg, FD_0_ARBITER_FINAL_FORMAT_STATUS, NEW_FORMAT);
	/* Need to clear the format status */
	ulFmtId = BCHP_GET_FIELD_DATA(
		hVdec->ulFdFormatReg, FD_0_ARBITER_FINAL_FORMAT_STATUS, FORMAT_INDEX);

	*pbNoInput = BCHP_GET_FIELD_DATA(
		hVdec->ulFdFormatReg, FD_0_ARBITER_FINAL_FORMAT_STATUS, MISSING_INPUT);

	/***2: need to clear New_Format bit*/
	if(bNewFmt)
	{
		BVDC_P_Vdec_ReadFdStatus_isr(hVdec, &hVdec->stFdStatus);
		hVdec->bNewFmt = true;
		BREG_Write32_isr(hVdec->hReg, BCHP_FD_0_ARBITER_FINAL_FORMAT_STATUS +
			hVdec->ulFdOffset, hVdec->ulFdFormatReg);
	}

	/* 2.5: PR40133: protect the fd index for the SD input. When swich input from Digital
	 * to analog ifd, the FD index sometimes return wrong values such as 480p. Some
	 * protection should be done. */
	if((ulFmtId) &&
	   (pCurInfo->bCvbsInput || pCurInfo->bSvidInput || pCurInfo->bIfdInput))
	{
		if(!VIDEO_FORMAT_IS_SD((&pCurInfo->aFdtFmtTbl[ulFmtId-1])->eFmt))
		{
			BDBG_MSG(("For SD input, FD report error format [%d]", ulFmtId));
			ulFmtId = 0;
		}
	}

	/* 2.7: PR45872: For Cvbs, Svideo input signal, FD core can only
	 * distinguish 50Hz and 60Hz. To avoid confusion,
	 * treat all the format  that reported from FD as NTSC and PAL-G format.*/
	if((ulFmtId) &&
	   (pCurInfo->bCvbsInput || pCurInfo->bSvidInput || pCurInfo->bIfdInput))
	{
		if(VIDEO_FORMAT_IS_525_LINES((&pCurInfo->aFdtFmtTbl[ulFmtId-1])->eFmt))
		{
			ulFmtId = 1; /*NTSC*/
		}
		if(VIDEO_FORMAT_IS_625_LINES((&pCurInfo->aFdtFmtTbl[ulFmtId-1])->eFmt))
		{
			ulFmtId = 2;/*PAL-G*/
		}
	}

	/*2.8: PR49165: FD is not stable when meet some customer RF channels.*/
	if(pCurInfo->bIfdInput)
	{
		if(hVdec->bNewFmt)
		{
			if(ulFmtId != hVdec->ulLastFmtId )
			{   /*new index*/
				hVdec->ulLastFmtId = ulFmtId;
				hVdec->ulFdFmtCnt = 0;
			}
			else if(hVdec->ulFdFmtCnt < BVDC_P_VDEC_FD_FMTID_CNT)
			{   /*count*/
				hVdec->ulFdFmtCnt++;
			}
			else if(hVdec->ulFmtId == ulFmtId)
			{   /*ignore if same*/
				hVdec->bNewFmt = false;
				hVdec->ulFdFmtCnt = 0;
			}
		}
	}

	/***3: need to hand over DCR for component input */
	if(pCurInfo->bCompInput)
	{
		/***switch to FD, the follow three condition:
		 * a: new format is set and HPLL hasn't get stable or HPLL has lost lock for a while
		   b: switch count has reach max */
		if((hVdec->ulClampSelFdCnt == BVDC_P_VDEC_SWITCH_CLAMP_LEVEL) &&
		    ((hVdec->bNewFmt && (!(hVdec->bHPllLocked && (hVdec->ulVsyncCnt > 5))))||
		      ((hVdec->ulVsyncCnt > 40) && (!hVdec->bHPllLocked) &&
		        (hVdec->ulClampSel == BCHP_VD_TOP_0_VD_VDEC_0_CTL_FD_CLAMP_SEL_VDEC))||
		      ((hVdec->ulVsyncCnt > 40) && (hVdec->bHPllLocked) &&
		        (!BVDC_P_EQ_DELTA(hVdec->stLinePerField.ulValue, /* in half line unit */
		          pCurInfo->pFmtInfo->ulScanHeight*(pCurInfo->pFmtInfo->bInterlaced ? 1 : 2),
		          BVDC_P_VDEC_LINES_PER_FIELD_THRESH)))))
		{
			hVdec->bNewFmt = false;
			ulClampSel = BCHP_VD_TOP_0_VD_VDEC_0_CTL_FD_CLAMP_SEL_FD;
			hVdec->ulClampSelFdCnt = 0;
		}
		/***switch to VDEC
		* when get a valid format index, or HPLL has get stable */
		if(hVdec->ulClampSelFdCnt == BVDC_P_VDEC_SWITCH_CLAMP_LEVEL)
		{
			if(ulFmtId || (hVdec->bHPllLocked && (hVdec->ulVsyncCnt > 5)))
			{
				ulClampSel = BCHP_VD_TOP_0_VD_VDEC_0_CTL_FD_CLAMP_SEL_VDEC;
			}
		}
	}
	else if((pCurInfo->bPcInput) || (pCurInfo->bSvidInput) || (pCurInfo->bIfdInput) || (pCurInfo->bCvbsInput))
	{
		/* a: no need switch DCR for Pc input, Svideo, analog RF input, CVBS source
		 * b: no connection between internal IFD and FD core*/
		ulClampSel = BCHP_VD_TOP_0_VD_VDEC_0_CTL_FD_CLAMP_SEL_VDEC;
	}
	else
	{
		/*unsupported format*/
		return;
	}

	/***4: change the s/w format.  */

	/* a. when put DCR in FD and get new format ID with FD core, change S/W format.
	 * if DCR is in VDEC, ignore the new format.
	 * b. For component and PC input, FD can detect all the format.
	 * c. For the format of CVBS/SVideo/Rf input, the subcarrier maybe different,
	 *    need S/W involved */

	/* For cvbs, svdieo, analog rf input. S/W support customize format for auto detection,
	  * find current format in the customer format table.*/
	ulNumFormats = sizeof(s_aeDefaultFormats) / sizeof(BFMT_VideoFmt);
	while(eCurVideoFmt != s_aeDefaultFormats[ulFmtIndex++])
	{
		if(ulFmtIndex >= ulNumFormats)
		{
			ulFmtIndex = 0;
			break;
		}
	}

	if((ulFmtId) &&
	   ((pCurDirty->stBits.bAutoDetectFmt) ||
	    ((ulFmtId != hVdec->ulFmtId) &&
		    (hVdec->ulClampSel || pCurInfo->bPcInput || pCurInfo->bSvidInput || pCurInfo->bCvbsInput)) ||
		((pCurInfo->bIfdInput) && (hVdec->ulFmtId != ulFmtId) && (hVdec->ulFdFmtCnt == BVDC_P_VDEC_FD_FMTID_CNT) &&
		 (!BVDC_P_EQ_DELTA(hVdec->stLinePerField.ulValue,pCurInfo->pFmtInfo->ulScanHeight, BVDC_P_VDEC_LINES_PER_FIELD_THRESHOLD)))))
	{
		*ppFdEntry = (pCurInfo->bReMapFormats)
			? BVDC_P_Vdec_FindFmtMap_isr(ulFmtId, pCurInfo)
			: &pCurInfo->aFdtFmtTbl[ulFmtId-1];
		hVdec->ulFmtId = ulFmtId;

		if((VIDEO_FORMAT_IS_525_LINES(eCurVideoFmt) &&
		    VIDEO_FORMAT_IS_525_LINES((*ppFdEntry)->eFmt)) ||
		   (VIDEO_FORMAT_IS_625_LINES(eCurVideoFmt) &&
		    VIDEO_FORMAT_IS_625_LINES((*ppFdEntry)->eFmt)))
		{
			/* If currently running PAL_NC and PAL_M, will ignore */
			/* the new format detected by hardware since hardware */
			/* can only detect NTSC and PAL */
			BDBG_MSG(("skip HW detection - using SW detection"));
		}
		else
		{
			if(VIDEO_FORMAT_IS_525_LINES((*ppFdEntry)->eFmt))
			{
				eNewVideoFmt = pCurInfo->stVdecSettings.eDefaultNoBurst525LinesFormat;
			}
			else if(VIDEO_FORMAT_IS_625_LINES((*ppFdEntry)->eFmt))
			{
				eNewVideoFmt = pCurInfo->stVdecSettings.eDefaultNoBurst625LinesFormat;
			}
			else
			{
				eNewVideoFmt = (*ppFdEntry)->eFmt;
			}
		}
	}
	else if((hVdec->ulClampSelFdCnt == BVDC_P_VDEC_SWITCH_CLAMP_LEVEL ) &&
	        (VIDEO_FORMAT_IS_525_LINES(eCurVideoFmt) ||
	         VIDEO_FORMAT_IS_625_LINES(eCurVideoFmt)))
	{
		if(pCurInfo->bCvbsInput || pCurInfo->bSvidInput)
		{
			/* PR26033: SW Auto-detect among 50Hz format or 60Hz for
			  * CVBS, Svideo and analog rf input */
			hVdec->ulsubcarrierloopunlock = (hVdec->bVcrHeadSwitch)
				? BVDC_P_VDEC_SUBCARRIER_LOOP_ACC_UNLOCK_VCR
				: BVDC_P_VDEC_SUBCARRIER_LOOP_ACC_UNLOCK_NORMAL;

#if BVDC_P_SECAM_AUTO_DETECION_SUPPORT
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
			/*secam format detection*/
			if(VIDEO_FORMAT_IS_SECAM(eCurVideoFmt))
			{
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
				if(hVdec->ulSecamSyncUnlockTimer++ > BVDC_P_VDEC_SECAM_LOCK_THRESHOLD)
				{
					eNewVideoFmt = BFMT_VideoFmt_ePAL_G;
				}
#else
				if((hVdec->bSubCarrLoopAccAbsUpdated) &&
				   (hVdec->ulColorBurstAmpDelta < 0x30) &&
				   (hVdec->ulFormatChangeTimer >= 30))
				{
					eNewVideoFmt = BFMT_VideoFmt_ePAL_G;
				}
#endif
			}
			else
#endif
#endif
			/*other format except secam*/
			if((hVdec->bSubCarrLoopAccAbsUpdated) && (hVdec->bHPllLocked && (hVdec->ulVsyncCnt > 3)))
			{
				if((hVdec->ulClearSubcarrierCnt == BVDC_P_VDEC_CLEAR_SUBCARRIER_THRESH) && (hVdec->bChromaPresent))
				{
					if(
					   /* Use (abs value) subcarrier loop accumulator to judge the subcarrier
					    * frequency is in the range of current format. In VCR mode, for some bad VCR,
					    * if set threshold to 0x1000, it is very easy to toggle, even set correct format.*/
					   ((hVdec->ulSubCarrLoopAccAbs >= hVdec->ulsubcarrierloopunlock ) &&
					      (hVdec->ulFormatChangeTimer >= BVDC_P_VDEC_FORMAT_CHANGE_DELAY_START_SUB)) ||
					   /* Use burst amp delta to judge for some format*/
					   (((hVdec->ulColorBurstAmpDelta > BVDC_P_VDEC_COLORBURST_AMP_DELTA_THRESH) &&
					      (hVdec->ulFormatChangeTimer >= BVDC_P_VDEC_FORMAT_CHANGE_DELAY_START_AMP))&&
					      (eCurVideoFmt == BFMT_VideoFmt_ePAL_NC || eCurVideoFmt == BFMT_VideoFmt_ePAL_G ||
					       eCurVideoFmt == BFMT_VideoFmt_ePAL_M || eCurVideoFmt == BFMT_VideoFmt_ePAL_60 ||
					       eCurVideoFmt == BFMT_VideoFmt_eNTSC_443)) ||
					   /* use SUBCARRIER_LOOP_LOCK to judge for some case*/
					   ((hVdec->ulSubCarrLoopAccAbs < BVDC_P_VDEC_SUBCARRIER_LOOP_ACC_UNLOCK_NORMAL) &&
					     (hVdec->ulChromaUnlockCnt == BVDC_P_VDEC_CHROMA_UNLOCK_THRESH)))
					{
						for(i = 0; i <  ulNumFormats; i++)
						{
							if((VIDEO_FORMAT_IS_525_LINES(eCurVideoFmt) &&
							    VIDEO_FORMAT_IS_525_LINES(s_aeDefaultFormats[ulFmtIndex])) ||
							   (VIDEO_FORMAT_IS_625_LINES(eCurVideoFmt) &&
							    VIDEO_FORMAT_IS_625_LINES(s_aeDefaultFormats[ulFmtIndex])))
							{
								if(BVDC_P_Vdec_LookUpFmt(pCurInfo->aeFormats, pCurInfo->ulNumFormats, s_aeDefaultFormats[ulFmtIndex]))
								{
									BDBG_MSG(("PI maps %d in %d formats", s_aeDefaultFormats[ulFmtIndex], pCurInfo->ulNumFormats));
									eNewVideoFmt = s_aeDefaultFormats[ulFmtIndex];
									break;
								}
							}
							ulFmtIndex++;
							ulFmtIndex = ulFmtIndex % ulNumFormats;
						}
						if((hVdec->ulSubCarrLoopAccAbs < BVDC_P_VDEC_SUBCARRIER_LOOP_ACC_UNLOCK_NORMAL) &&
						   (hVdec->ulChromaUnlockCnt == BVDC_P_VDEC_CHROMA_UNLOCK_THRESH))
						{
							hVdec->ulChromaUnlockCnt = 0;
						}
					}
					/*to fix some very bad VCR tape*/
					else if((hVdec->ulFormatChangeTimer >= BVDC_P_VDEC_FORMAT_CHANGE_DELAY_START_SUB) &&
					        (hVdec->ulVsyncCnt > 80) &&
					        (hVdec->bChromaLock == false))
					{
						if(BVDC_P_Vdec_LookUpFmt(pCurInfo->aeFormats, pCurInfo->ulNumFormats, BFMT_VideoFmt_ePAL_M) &&
						    BVDC_P_Vdec_LookUpFmt(pCurInfo->aeFormats, pCurInfo->ulNumFormats, BFMT_VideoFmt_eNTSC))
						{
							if(BFMT_VideoFmt_ePAL_M == eCurVideoFmt)
							{
								eNewVideoFmt = BFMT_VideoFmt_eNTSC;
							}
							else if(BFMT_VideoFmt_eNTSC == eCurVideoFmt)
							{
								eNewVideoFmt = BFMT_VideoFmt_ePAL_M;
							}
						}
					}
				}

				/*PR33011: when no burst, s/w needs to assume one defult format */
				if((hVdec->ulFormatChangeTimer >= BVDC_P_VDEC_FORMAT_CHANGE_DELAY_START_SUB) &&
				   (hVdec->ulBlackWhiteCnt == BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH) &&
				   (hVdec->ulVsyncCnt > 20))
				{
					if(VIDEO_FORMAT_IS_525_LINES(eCurVideoFmt))
					{
						eNewVideoFmt = pCurInfo->stVdecSettings.eDefaultNoBurst525LinesFormat;
					}
					else if(VIDEO_FORMAT_IS_625_LINES(eCurVideoFmt))
					{
						eNewVideoFmt = pCurInfo->stVdecSettings.eDefaultNoBurst625LinesFormat;
					}
				}
				hVdec->bSubCarrLoopAccAbsUpdated = false;
			}
		}
		else if(pCurInfo->bIfdInput)
		{
			/* for tuner input, PAL-Nc/NTSC/PAL-M, they have the same channel
			 * bandwidth(6M), so need auto format detection.  No need support
			 * auto detection for PAL, because it's bandwidth is different with
			 * NTSC. */
#if BVDC_P_SECAM_AUTO_DETECION_SUPPORT
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
			/*secam format detection*/
			if(VIDEO_FORMAT_IS_SECAM(eCurVideoFmt))
			{
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
				if(hVdec->ulSecamSyncUnlockTimer++ > BVDC_P_VDEC_SECAM_LOCK_THRESHOLD)
				{
					eNewVideoFmt = BFMT_VideoFmt_ePAL_G;
				}
#else
				if((hVdec->bSubCarrLoopAccAbsUpdated) &&
				   (hVdec->ulColorBurstAmpDelta < 0x30) &&
				   (hVdec->ulFormatChangeTimer >= 30))
				{
					eNewVideoFmt = BFMT_VideoFmt_ePAL_G;
				}
#endif
			}
			else
#endif
#endif
			if((hVdec->ulClearSubcarrierCnt == BVDC_P_VDEC_CLEAR_SUBCARRIER_THRESH) &&
			    (hVdec->bHPllLocked && (hVdec->ulVsyncCnt > 3)) &&
			    (!hVdec->bDisableRfAutoDetection))
			{
				if(hVdec->bSubCarrLoopAccAbsUpdated &&
				   (hVdec->ulFormatChangeTimer >= BVDC_P_VDEC_FORMAT_CHANGE_DELAY_START_SUB) &&
				   ((hVdec->ulSubCarrLoopAccAbs >= BVDC_P_VDEC_SUBCARRIER_LOOP_ACC_UNLOCK_NORMAL) ||
				     ((hVdec->ulSubCarrLoopAccAbs > BVDC_P_VDEC_SUBCARRIER_LOOP_ACC_UNLOCK_FAST) &&
				       (hVdec->ulAdcChangCnt < BVDC_P_VDEC_AUTO_DETECTION_USE_FAST_THRESH_TIMER)) ||
				     (!BVDC_P_Vdec_LookUpFmt(pCurInfo->aeFormats, pCurInfo->ulNumFormats, eCurVideoFmt))))
				{
					for(i = 0; i <  ulNumFormats; i++)
					{
						if((VIDEO_FORMAT_IS_525_LINES(eCurVideoFmt) &&
						     VIDEO_FORMAT_IS_525_LINES(s_aeDefaultFormats[ulFmtIndex])) ||
						    (VIDEO_FORMAT_IS_625_LINES(eCurVideoFmt) &&
						     VIDEO_FORMAT_IS_625_LINES(s_aeDefaultFormats[ulFmtIndex])))
						{
							if(BVDC_P_Vdec_LookUpFmt(pCurInfo->aeFormats, pCurInfo->ulNumFormats, s_aeDefaultFormats[ulFmtIndex]))
							{
								eNewVideoFmt = s_aeDefaultFormats[ulFmtIndex];
								break;
							}
						}
						ulFmtIndex++;
						ulFmtIndex = ulFmtIndex % ulNumFormats;
					}
				}
				hVdec->bSubCarrLoopAccAbsUpdated = false;
			}

			/*PR33011: when no burst, s/w needs to assume one defult format */
			if((hVdec->ulBlackWhiteCnt >= BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH) &&
			   (hVdec->ulFormatChangeTimer >= BVDC_P_VDEC_FORMAT_CHANGE_DELAY_START_SUB) &&
			   (hVdec->ulVsyncCnt > 20) &&
			   (!VIDEO_FORMAT_IS_SECAM(eCurVideoFmt)))
			{
				if(VIDEO_FORMAT_IS_525_LINES(eCurVideoFmt))
				{
					eNewVideoFmt = pCurInfo->stVdecSettings.eDefaultNoBurst525LinesFormat;
				}
				else if(VIDEO_FORMAT_IS_625_LINES(eCurVideoFmt))
				{
					eNewVideoFmt = pCurInfo->stVdecSettings.eDefaultNoBurst625LinesFormat;
				}
			}
		}
	}

	/***5: special process for some PR. */

	/* PR37058: component 480i/576i, 480p/576p can be locked each other;
	 * PR37449: [YPbPr] Video flicker seriously and overlap when change timing
	 *    from 1080i59.94 to 1080P29.
	 * PR38260: Video mute(Can't lock) for 480i59.94 format for YPbPr source
	 *    when swithing format between 480P59.94 and 480i59.94 if using Chroma.
	 *    the upper three PRs is the side effect of the S/W workaround of the
	 *    following PR36323. we don't need this workaround if apply H/W setting:
	 *    enable FD_0_CTRL.MF_USE_LINE_LENGTH_LOCK.
	 * PR36323: FD report wrong format index if meet certain pattern
	 *    (Master1025D--Pattern24) and 1080P59.94/60 format */
	if((ulFmtId) &&
	    (ulFmtId <= BVDC_P_ATSC_FDT_COUNT) &&
	    (hVdec->bHPllLocked) &&
	    (hVdec->ulVsyncCnt > 20) &&
	    (pCurInfo->bCompInput) &&
	    ((&pCurInfo->aFdtFmtTbl[ulFmtId-1])->eFmt != eCurVideoFmt))
	{
			BDBG_MSG(("Two format[%d -> %d] can be locked each other ",
				eNewVideoFmt, (&pCurInfo->aFdtFmtTbl[ulFmtId-1])->eFmt));
			eNewVideoFmt = (&pCurInfo->aFdtFmtTbl[ulFmtId-1])->eFmt;
			*ppFdEntry  = &pCurInfo->aFdtFmtTbl[ulFmtId-1];
			hVdec->ulFmtId = ulFmtId;
		}

	/* PR40133: protect the format for the SD input. When switch input from
	 * component to Ifd, if souce handle is not closed, the format info of
	 * component will be kept in Ifd input. */
	if((!VIDEO_FORMAT_IS_SD(eNewVideoFmt)) &&
	   (pCurInfo->bCvbsInput || pCurInfo->bSvidInput || pCurInfo->bIfdInput))
	{
		BDBG_MSG(("For SD input, Wrong current format [%d]",eNewVideoFmt));
		eNewVideoFmt = BFMT_VideoFmt_eNTSC;
	}

	/* Get the new format pointer at last. */
	*ppNewFmtInfo = BFMT_GetVideoFormatInfoPtr(eNewVideoFmt);

	/***6: other ... */
	if(ulClampSel != hVdec->ulClampSel)
	{
		hVdec->ulClampSel = ulClampSel;
		hVdec->ulClampSelFdCnt = 0;
		pCurDirty->stBits.bAutoDetectFmt = BVDC_P_DIRTY;
	}

	if(hVdec->ulClampSelFdCnt < BVDC_P_VDEC_SWITCH_CLAMP_LEVEL)
	{
		hVdec->ulClampSelFdCnt++;
	}

	return;
}


/***************************************************************************
 *
 * Update Vdec status.
 */
void BVDC_P_Vdec_UpdateStatus_isr
	( BVDC_P_Vdec_Handle               hVdec )
{
	/* New status */
	bool bNoisy;
	bool b3DComb;
	bool b240pMode;
	bool bFh3DComb;
	bool bHPllLock;
	bool bCcrEnable;
	bool bVideoLock;
	bool bChromaLock;
	bool bVcrHeadSwitch;
	bool bChromaPresent;
	bool bPcInPhaseLocked;
	bool bManualColorMode;
	bool bClearSubCarAccum;
	bool bPcPositionLocked;
	bool bPalSeparation;
	uint32_t ulCrGain;
	uint32_t ulCbGain;
	uint32_t ulPcHsPol;
	uint32_t ulPcVsPol;
	uint32_t ulColorBurstGain;
	bool bHdBilevelDetected;
	bool bResetHsyncPllAccum;
	bool bResetHStart;
	uint32_t ulCurrentLineLength;
	uint32_t ulNewLineLength;
	bool bCompEnableSliceLPFHpllCtrl;
#if (BVDC_P_VDEC_PR38861_EVERY_VSYNC)
	uint32_t ulVsyncMode;
#endif

	bool bAgc;
	uint32_t ulCs;
	BVDC_MacrovisionType eMvType;
	BAVC_FrameRateCode eFrameRate;
	const BVDC_P_FdtEntry *pFdEntry;
	const BFMT_VideoInfo *pNewFmtInfo;

	/* Source & dirty bit context */
	BVDC_P_Source_Info *pCurInfo;
	BVDC_P_Source_DirtyBits *pCurDirty;

	/* To be updated by following function from hw status */
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	BDBG_OBJECT_ASSERT(hVdec->hSource, BVDC_SRC);
	pCurInfo  = &hVdec->hSource->stCurInfo;
	pCurDirty = &pCurInfo->stDirty;

	/* Default to current value */
	bNoisy            = hVdec->bNoisy;
	b3DComb           = hVdec->b3DComb;
	b240pMode         = hVdec->b240pMode;
	bFh3DComb         = hVdec->bFh3DComb;
	bCcrEnable        = hVdec->bCcrEnable;
	bHPllLock         = hVdec->bHPllLocked;
	bVideoLock        = hVdec->bVideoDetected;
	bChromaLock       = hVdec->bChromaLock;
	bVcrHeadSwitch    = hVdec->bVcrHeadSwitch;
	bChromaPresent    = hVdec->bChromaPresent;
	bPalSeparation    = hVdec->bPalSeparation;
	bPcInPhaseLocked  = hVdec->bPcInPhaseLocked;
	bManualColorMode  = hVdec->bManualColorMode;
	bClearSubCarAccum = hVdec->bClearSubCarAccum;
	eMvType           = hVdec->eMacrovisionType;
	bPcPositionLocked = hVdec->bPcPositionLocked;

	ulCrGain          = hVdec->ulCrGain;
	ulCbGain          = hVdec->ulCbGain;
	ulPcHsPol         = hVdec->ulPcHsPol;
	ulPcVsPol         = hVdec->ulPcVsPol;
	ulColorBurstGain  = hVdec->ulColorBurstGain;

	eFrameRate        = hVdec->hSource->eFrameRateCode;
	pFdEntry          = pCurInfo->pFdEntry;
	pNewFmtInfo       = pCurInfo->pFmtInfo;

	bHdBilevelDetected = hVdec->bHdBilevelDetected;
	bResetHStart      = hVdec->bResetHStart;
	bResetHsyncPllAccum = hVdec->bResetHsyncPllAccum;
	bCompEnableSliceLPFHpllCtrl = hVdec->bCompEnableSliceLPFHpllCtrl;

	/* Read HW status, and see if SW need to response.  These eventually
	 * will go into HW when possible to reduce CPU load.
	 * Note:  The order of these function are sensitive, certain things
	 * are needed to be updated before. */
	BVDC_P_Vdec_ReadRegStatus_isr(hVdec, pCurInfo);
	BVDC_P_Vdec_UpdateVdecPhase_isr(hVdec, &hVdec->ulVdecPhase);
	BVDC_P_Vdec_UpdateHpllLock_isr(hVdec, pCurInfo, &bHPllLock, &bResetHStart, &bCompEnableSliceLPFHpllCtrl,&bResetHsyncPllAccum);
	BVDC_P_Vdec_UpdateVideoLock_isr(hVdec, pCurInfo, &bVideoLock);
	BVDC_P_Vdec_UpdateCopyProtection_isr(hVdec, pCurInfo, &bAgc, &ulCs, &eMvType);
#if (BVDC_P_USE_WORK_AROUNDS)
	BVDC_P_Vdec_UpdateSamplePerLine_isr(hVdec, pCurInfo, &bFh3DComb);
	BVDC_P_Vdec_UpdateLinePerField_isr(hVdec, pCurInfo, &hVdec->bFv3DComb);
	BVDC_P_Vdec_UpdateFramerate_isr(hVdec, hVdec->hSource, pCurInfo, &eFrameRate, &hVdec->bClk3DComb);
	BVDC_P_Vdec_UpdateVcrHeadSwitch_isr(hVdec, pCurInfo, &bVcrHeadSwitch);
	BVDC_P_Vdec_Update240pMode_isr(hVdec, pCurInfo, &b240pMode);
	BVDC_P_Vdec_UpdateCcr_isr(hVdec, pCurInfo, &bCcrEnable);
	BVDC_P_Vdec_UpdateNoiseLevel_isr(hVdec, pCurInfo, &bNoisy, &bPalSeparation);
	BVDC_P_Vdec_UpdateDynamicRegisters_isr(hVdec, pCurInfo);
	BVDC_P_Vdec_UpdateBilevel_isr(hVdec, pCurInfo, &bHdBilevelDetected);
#endif

	/* Do pc input auto phase adjustment! */
	if(pCurInfo->bPcInput)
	{
		if(hVdec->ulCountToggleHthreshold > 0)
		{
			hVdec->ulCountToggleHthreshold --;
			if(hVdec->ulCountToggleHthreshold == 0)
			{
				pCurDirty->stBits.bManualSync = true;
				hVdec->bToggleHthreshold = false;
			}
		}
		if(!pCurInfo->stPcInSyncSettings.bManualPhase)
		{
			BVDC_P_Vdec_UpdatePcInPhase_isr(hVdec, &bPcInPhaseLocked);
		}
		else
		{
			bPcInPhaseLocked = true;
		}
		if(!pCurInfo->stPcInSyncSettings.bManualPosition)
		{
			BVDC_P_Vdec_UpdateRaLock_isr(hVdec, pCurInfo);
		}
	}
	else
	{
		bPcInPhaseLocked = false;
	}

	if((pCurInfo->bPcInput)&&
		(hVdec->ulVsyncCnt >= BVDC_P_PC_POSITION_LOCKED_TIME)&&
		hVdec->bDpllLocked)
	{
		bPcPositionLocked = true;
	}
	else
	{
		bPcPositionLocked = false;
	}

#if (BVDC_P_USE_WORK_AROUNDS)
	/* Do manual color burst (or Auto Color Gain Control)? */
	if(hVdec->bManualAcgc)
	{
		BVDC_P_Vdec_UpdateColorBurst_isr(hVdec, &ulCrGain, &ulCbGain, &ulColorBurstGain, pCurInfo);
	}

	/* Update for chroma present. */
	if(pCurInfo->bCvbsInput || pCurInfo->bSvidInput || pCurInfo->bIfdInput)
	{
		BVDC_P_Vdec_UpdateColorPresent_isr(hVdec, &bChromaLock, &bChromaPresent,
			&bManualColorMode, &bClearSubCarAccum, pCurInfo);
	}
#endif

	/* Do auto detection. */
	if(pCurInfo->bAutoFmtDetection)
	{
		BVDC_P_Vdec_UpdateAutoFormat_isr(hVdec, pCurInfo, &pNewFmtInfo,
			&pFdEntry, pCurDirty, &hVdec->bNoInput);
	}

	/*when input is pc inut and auto detection is disable*/
	/*pol will be updated in new format when auto detection is enable. */
	if((!pCurInfo->bAutoFmtDetection) && (pCurInfo->bPcInput))
	{
		BVDC_P_Vdec_UpdatePcInPol_isr(hVdec, pCurInfo, &ulPcHsPol, &ulPcVsPol);
	}

	/* New Format? */
	if((pCurInfo->pFmtInfo != pNewFmtInfo) || (pCurInfo->pFdEntry != pFdEntry))
	{
		pCurDirty->stBits.bInputFormat   = true;
	}

	/* If source pending mode, then don't alloc until resume */
	if((pCurInfo->eResumeMode) && (pCurDirty->stBits.bInputFormat))
	{
		hVdec->hSource->bPending = true;
	}

	/* Clear pending flag if user clears it */
	if(hVdec->hSource->bPending)
	{
		if((pCurDirty->stBits.bResume) ||
		   (BVDC_ResumeMode_eAuto == pCurInfo->eResumeMode))
		{
			hVdec->hSource->bPending = false;
		}
	}

	/* Update Miscellaneous base on above update status */
	/* 3D Comb must be off when vcr mode or in non-sd formats source. */
	b3DComb = (
#if BVDC_P_VDEC_WORK_AROUND_DISABLE_COMB
		(!bVcrHeadSwitch) &&  /* Vcr head switch mode */
		(!hVdec->bNoisy) && /* Noisy disable 3DComb*/
		(((hVdec->ulVsyncCnt < 60) && (hVdec->bHPllLocked)) ? true :
		(bFh3DComb && /* Unstable horizontal frequency */
		 hVdec->bFv3DComb && /* Unstable vertical frequency */
		 hVdec->bClk3DComb)) && /* Unstable horizontal frequency with fixed FV */
#endif
		(!hVdec->hSource->bPending) &&
		(pCurInfo->ulWindows) && /* Need at least one window connected. */
		(pCurInfo->bEnable3DComb) && /* User explicitly turns it off */
		/* Not supported formats */
		(BFMT_VideoFmt_eNTSC_443 != pCurInfo->pFmtInfo->eVideoFmt) &&
		(!VIDEO_FORMAT_IS_SECAM(pCurInfo->pFmtInfo->eVideoFmt)) &&
		/* Not applicable to certain input */
		(pCurInfo->bCvbsInput || pCurInfo->bIfdInput)); /* only needed for cvbs/ifd */

	/* SW3556-1138: If 3DComb is in lagacy mode it needed to be turn off for
	 * VCR head switch, and fh/fv/clk unstable  */
	if((BVDC_Vdec_3DGameMode_eLegacyFull == pCurInfo->stVdecSettings.e3DGameMode) ||
	   (BVDC_Vdec_3DGameMode_eLegacySave == pCurInfo->stVdecSettings.e3DGameMode))
	{
		if((!bFh3DComb) ||
		   ( bVcrHeadSwitch) ||
		   (!hVdec->bFv3DComb) ||
		   (!hVdec->bClk3DComb))
		{
			b3DComb = false;
		}
	}

	/* Update dirty bits if any changes!  For building new RUL. */
	if(hVdec->bVideoDetected != bVideoLock)
	{
		hVdec->bVideoDetected = bVideoLock;
		pCurDirty->stBits.bVideoDetected = BVDC_P_DIRTY;
	}
#if (BVDC_P_USE_WORK_AROUNDS)
	if(hVdec->eMacrovisionType != eMvType ||
	   hVdec->bMvAgc != bAgc ||
	   hVdec->ulMvCs != ulCs)
	{
		hVdec->eMacrovisionType = eMvType;
		hVdec->bMvAgc = bAgc;
		hVdec->ulMvCs = ulCs;
		pCurDirty->stBits.bMvDetected = BVDC_P_DIRTY;
	}
#endif
	if(hVdec->bVcrHeadSwitch != bVcrHeadSwitch)
	{
		hVdec->bVcrHeadSwitch = bVcrHeadSwitch;
		pCurDirty->stBits.bVcrHeadSwitch = BVDC_P_DIRTY;
	}
	if((hVdec->bNoisy != bNoisy) ||(bPalSeparation != hVdec->bPalSeparation))
	{
		/*PR33797: Disable PAL Separation*/
		hVdec->bPalSeparation = bPalSeparation;
		hVdec->bNoisy = bNoisy;
		pCurDirty->stBits.bNoisy = BVDC_P_DIRTY;
	}
	if(hVdec->bNoisy != bNoisy)
	{
		hVdec->bNoisy = bNoisy;
		pCurDirty->stBits.bNoisy = BVDC_P_DIRTY;
	}
	if((hVdec->b3DComb != b3DComb) ||
	   (hVdec->bFh3DComb != bFh3DComb))
	{
		hVdec->b3DComb     = b3DComb;
		hVdec->bFh3DComb   = bFh3DComb;
		pCurDirty->stBits.b3DComb = BVDC_P_DIRTY;
	}
	if(hVdec->bCcrEnable != bCcrEnable)
	{
		hVdec->bCcrEnable  = bCcrEnable;
		hVdec->bVdecTuningDirtyBitCcr = true;
		pCurDirty->stBits.bVdecTuning = BVDC_P_DIRTY;
	}
	if(hVdec->bHPllLocked != bHPllLock)
	{
		hVdec->bHPllLocked = bHPllLock;
		pCurDirty->stBits.bHPllLocked = BVDC_P_DIRTY;
	}
	if(hVdec->hSource->eFrameRateCode != eFrameRate)
	{
		hVdec->hSource->eFrameRateCode = eFrameRate;
		pCurDirty->stBits.bFrameRateCode = BVDC_P_DIRTY;
	}

#if ((BVDC_P_VDEC_PR34153_PHASE_OFFSET) || (BVDC_P_VDEC_PR27640_HYSTERESIS) || \
	 (BVDC_P_VDEC_PR38861_EVERY_VSYNC))
	if(hVdec->bPcInPhaseLocked != bPcInPhaseLocked)
	{
#if (BVDC_P_VDEC_PR34153_PHASE_OFFSET)
		if((bPcInPhaseLocked) &&
		   (pCurInfo->bPcInput) &&
		   (!pCurInfo->stPcInSyncSettings.bManualPhase))
		{
			uint32_t i;
			uint32_t ulCh1Phase = BCHP_GET_FIELD_DATA(hVdec->ulPcInPhaseStatusReg,
				PC_IN_0_PHASE_STATUS, CH1_PHASE);

			pCurInfo->stPcInSyncSettings.bManualPhase = true;
			pCurInfo->stPcInSyncSettings.aulPhase[2] = ulCh1Phase;
			pCurInfo->stPcInSyncSettings.aulPhase[1] = ulCh1Phase;
			pCurInfo->stPcInSyncSettings.aulPhase[0] = ulCh1Phase;

			if(pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_eDVI_800x600p_56Hz)
			{
				pCurInfo->stPcInSyncSettings.aulPhase[2] += 4;
				pCurInfo->stPcInSyncSettings.aulPhase[1] += 0;
				pCurInfo->stPcInSyncSettings.aulPhase[0] += 2;
			}
#if (BVDC_P_PC_800X600_75_AUTO_PHASE_FREEZE)
			else if(pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_eDVI_800x600p_75Hz)
			{
				pCurInfo->stPcInSyncSettings.aulPhase[2] = 6;
				pCurInfo->stPcInSyncSettings.aulPhase[1] = 1;
				pCurInfo->stPcInSyncSettings.aulPhase[0] = 6;
			}
#endif
			else if(pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_eDVI_1280x1024p_75Hz ||
				pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_eDVI_1360x768p_60Hz||
				pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_eDVI_1400x1050p_75Hz||
				pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_eDVI_1920x1080p_60Hz_Red)
			{
				pCurInfo->stPcInSyncSettings.aulPhase[2] += 2;
				pCurInfo->stPcInSyncSettings.aulPhase[1] += 0;
				pCurInfo->stPcInSyncSettings.aulPhase[0] += 1;
			}

			else
			{
				pCurInfo->stPcInSyncSettings.aulPhase[2] += 2;
				pCurInfo->stPcInSyncSettings.aulPhase[1] += 0;
				pCurInfo->stPcInSyncSettings.aulPhase[0] += 3;
			}

			for(i = 0; i < BVDC_COLOR_CHANNELS; i++)
			{
				if(pCurInfo->stPcInSyncSettings.aulPhase[i] >= 16)
				{
					pCurInfo->stPcInSyncSettings.aulPhase[i] -= 16;
				}
			}

			hVdec->hSource->stNewInfo.stPcInSyncSettings.bManualPhase = pCurInfo->stPcInSyncSettings.bManualPhase;
			hVdec->hSource->stNewInfo.stPcInSyncSettings.aulPhase[2]  = pCurInfo->stPcInSyncSettings.aulPhase[2];
			hVdec->hSource->stNewInfo.stPcInSyncSettings.aulPhase[1]  = pCurInfo->stPcInSyncSettings.aulPhase[1];
			hVdec->hSource->stNewInfo.stPcInSyncSettings.aulPhase[0]  = pCurInfo->stPcInSyncSettings.aulPhase[0];
		}
#endif
		hVdec->bPcInPhaseLocked = bPcInPhaseLocked;
		pCurDirty->stBits.bManualSync  = BVDC_P_DIRTY;
	}

#if (BVDC_P_VDEC_PR38861_EVERY_VSYNC)
	ulVsyncMode = (
		(pCurInfo->bPcInput) &&
		(hVdec->ulVsyncCnt >= 40) &&
		(!hVdec->bPcInPhaseLocked)) ? 0 : 1;

	if(hVdec->ulVsyncMode != ulVsyncMode)
	{
		hVdec->ulVsyncMode = ulVsyncMode;
		pCurDirty->stBits.bManualSync  = BVDC_P_DIRTY;
	}
#endif
#endif

	if(hVdec->bPcPositionLocked != bPcPositionLocked)
	{
		if(bPcPositionLocked)
		{
			(hVdec->ulPcPositionLockdelay)++;
			if(BVDC_P_VDEC_TOGGLE_TIME == hVdec->ulPcPositionLockdelay)
			{
				hVdec->bPcPositionLocked = bPcPositionLocked;
				hVdec->bPcFieldSyncTypeToggle = false;
				pCurDirty->stBits.bManualSync  = BVDC_P_DIRTY;
				hVdec->ulPcPositionLockdelay = 0;
			}
		}
		else
		{
			hVdec->bPcPositionLocked = bPcPositionLocked;
			pCurDirty->stBits.bManualSync  = BVDC_P_DIRTY;
			hVdec->ulPcPositionLockdelay = 0;
		}
	}
	if((hVdec->ulPcHsPol != ulPcHsPol) ||
	   (hVdec->ulPcVsPol != ulPcVsPol))
	{
		hVdec->ulPcHsPol = ulPcHsPol;
		hVdec->ulPcVsPol = ulPcVsPol;
		pCurDirty->stBits.bManualSync  = BVDC_P_DIRTY;
	}
	if((hVdec->bChromaLock != bChromaLock) ||
	   (hVdec->bChromaPresent != bChromaPresent) ||
	   (hVdec->bManualColorMode != bManualColorMode) ||
	   (hVdec->bClearSubCarAccum != bClearSubCarAccum) ||
	   (hVdec->ulColorBurstGain  != ulColorBurstGain))
	{
#if (BVDC_P_ACGC_DBG_MANUAL_ACGC)
		BDBG_MSG(("[ACGC_MANULA_GAIN]--------------------------------"));
		BDBG_MSG(("bChromaLock:[%d -> %d]%s",
			hVdec->bChromaLock, bChromaLock,
			(hVdec->bChromaLock != bChromaLock) ? "*":""));
		BDBG_MSG(("ulCrGain:[0x%x -> 0x%x]%s",
			hVdec->ulCrGain, ulCrGain,
			(hVdec->ulCrGain != ulCrGain) ? "*":""));
		BDBG_MSG(("ulCbGain:[0x%x -> 0x%x]%s",
			hVdec->ulCbGain, ulCbGain,
			(hVdec->ulCbGain != ulCbGain) ? "*":""));
		BDBG_MSG(("bChromaPresent:[%d -> %d]%s",
			hVdec->bChromaPresent, bChromaPresent,
			(hVdec->bChromaPresent != bChromaPresent) ? "*":""));
		BDBG_MSG(("bManualColorMode:[%d -> %d]%s",
			hVdec->bManualColorMode, bManualColorMode,
			(hVdec->bManualColorMode != bManualColorMode) ? "*":""));
		BDBG_MSG(("bClearSubCarAccum:[%d -> %d]%s",
			hVdec->bClearSubCarAccum, bClearSubCarAccum,
			(hVdec->bClearSubCarAccum != bClearSubCarAccum) ? "*":""));
		BDBG_MSG(("ulColorBurstGain:[0x%x -> 0x%x]%s",
			hVdec->ulColorBurstGain, ulColorBurstGain,
			(hVdec->ulColorBurstGain != ulColorBurstGain) ? "*":""));
#endif
		/* If gain change reset the averaging to remove old amplitude history. */
		if(hVdec->ulColorBurstGain  != ulColorBurstGain)
		{
			hVdec->stColorBurstAmp.ulIndex = 0;
		}

		hVdec->bChromaLock = bChromaLock;
		hVdec->bChromaPresent = bChromaPresent;
		hVdec->bManualColorMode = bManualColorMode;
		hVdec->bClearSubCarAccum = bClearSubCarAccum;
		hVdec->ulColorBurstGain = ulColorBurstGain;
		pCurDirty->stBits.bManualAcgc = BVDC_P_DIRTY;
	}

#if (BVDC_P_ACGC_CSC_GAIN)
	if((hVdec->ulCrGain != ulCrGain) ||
	   (hVdec->ulCbGain != ulCbGain))
	{
		hVdec->ulCrGain = ulCrGain;
		hVdec->ulCbGain = ulCbGain;
		pCurDirty->stBits.bColorspace = BVDC_P_DIRTY;
	}
#endif

	/*fine tuning VDEC video quality*/
	if(bHdBilevelDetected != hVdec->bHdBilevelDetected)
	{
		BDBG_MSG(("BilevelDetection:[%d -> %d]", hVdec->bHdBilevelDetected, bHdBilevelDetected));
		hVdec->bHdBilevelDetected = bHdBilevelDetected;
		pCurDirty->stBits.bVdecTuning = BVDC_P_DIRTY;
		hVdec->bVdecTuningDirtyBitBilevel = true;
	}

	if(bResetHStart != hVdec->bResetHStart)
	{
		hVdec->bResetHStart = bResetHStart;
		pCurDirty->stBits.bVdecTuning = BVDC_P_DIRTY;
		hVdec->bVdecTuningDirtyBitResetHStart = true;
	}
	if(bCompEnableSliceLPFHpllCtrl != hVdec->bCompEnableSliceLPFHpllCtrl)
	{
		hVdec->bCompEnableSliceLPFHpllCtrl = bCompEnableSliceLPFHpllCtrl;
		{
			pCurDirty->stBits.bVdecTuning = BVDC_P_DIRTY;
			hVdec->bVdecTuningDirtyBitEnableSliceLPFHpllCtrl= true;
		}
	}
	if(bResetHsyncPllAccum != hVdec->bResetHsyncPllAccum)
	{
		hVdec->bResetHsyncPllAccum = bResetHsyncPllAccum;
		if(bResetHsyncPllAccum)
		{
			pCurDirty->stBits.bVdecTuning = BVDC_P_DIRTY;
			hVdec->bVdecTuningDirtyBitResetHsync= true;
		}
	}

	/* SW3556-804: Function select updated, need to trigger callback. */
	if(/*(hVdec->bFeAgcLevelsUpdated) ||*/
	   (hVdec->bScartFuncSelectUpdated))
	{
		pCurDirty->stBits.bVdecTuning = BVDC_P_DIRTY;
	}

	/* SW3556-804: Function select updated, need to trigger callback. */
	if(hVdec->b240pMode != b240pMode)
	{
		hVdec->b240pMode = b240pMode;
		pCurDirty->stBits.bVdecTuning = BVDC_P_DIRTY;
	}

	/* Handling changes! */
	if(BVDC_P_IS_DIRTY(pCurDirty))
	{
		BVDC_P_Source_Info *pNewInfo = &hVdec->hSource->stNewInfo;

		/* Update format related parameters to be used */
		if(pCurDirty->stBits.bInputFormat)
		{
			/* Vdec configurating to new format! */
			BVDC_P_BUF_MSG(("Vsync[%d] SubCarAcc[%d] changes format %s -> %s",
			hVdec->ulVsyncCnt, hVdec->ulSubCarrLoopAccAbs,
			pCurInfo->pFmtInfo->pchFormatStr, pNewFmtInfo->pchFormatStr));

			/* Raster type changed? */
			if(pCurInfo->pFmtInfo->bInterlaced != pNewFmtInfo->bInterlaced)
			{
				hVdec->hSource->bRasterChanged = true;
			}

			/* Start the new format */
			hVdec->hSource->bPictureChanged = true;
			hVdec->hSource->ulVertFreq      = pNewFmtInfo->ulVertFreq;
			pCurInfo->pFmtInfo       = pNewInfo->pFmtInfo = pNewFmtInfo;
			pCurInfo->pFdEntry       = pNewInfo->pFdEntry = pFdEntry;
			pCurInfo->eAspectRatio   = pCurInfo->pFmtInfo->eAspectRatio;
			hVdec->hSource->bSrcIs444       = pCurInfo->bPcInput && (!pFdEntry->bAtscFormat);

			/* Get vdc base fmt information */
			pCurInfo->pVdcFmt        = pNewInfo->pVdcFmt = BVDC_P_GetFormatInfoPtr_isr(pNewFmtInfo);

			/* Get configuration for PC Input if any. */
			BVDC_P_Vdec_GetFmtCfg_isr(pNewInfo->pFmtInfo, pNewInfo->stVdecSettings.e3DGameMode,
				&pNewInfo->pCdCfg, &pNewInfo->pYcCfg, &pNewInfo->pPcInCfg);
			BVDC_P_Vdec_GetFmtCfg_isr(pCurInfo->pFmtInfo, pCurInfo->stVdecSettings.e3DGameMode,
				&pCurInfo->pCdCfg, &pCurInfo->pYcCfg, &pCurInfo->pPcInCfg);
			hVdec->ulPcHsPol = pFdEntry->ulHsPol;
			hVdec->ulPcVsPol = pFdEntry->ulVsPol;

			/* Format changes, so set dirty to rebuild RUL. */
			pCurDirty->stBits.bInputFormat    = BVDC_P_DIRTY;
			hVdec->ulFormatChangeTimer = 0;

			/* inform next ApplyChanges to copy activated isr setting into new info */
			hVdec->hSource->stIsrInfo.stActivated.stBits.bInputFormat = BVDC_P_DIRTY;
			hVdec->hSource->stIsrInfo.stActivated.stBits.bAspectRatio = BVDC_P_DIRTY;

			if(pCurInfo->bPcInput)
			{
				hVdec->ulStdrHFreq = pCurInfo->pFmtInfo->ulScanWidth;
			}
			else
			{
				uint32_t ulSamplingFreq =
					  pCurInfo->pVdcFmt->bSd ? 27  * BFMT_FREQ_FACTOR  /* 27Mhz */
					: pCurInfo->pVdcFmt->bEd ? 54  * BFMT_FREQ_FACTOR  /* 54Mhz */
					: (pCurInfo->pFmtInfo->ulPxlFreq <= 7425) ?
					                                       108 * BFMT_FREQ_FACTOR  /* 108Mhz */
					:                                      216 * BFMT_FREQ_FACTOR; /* 216Mhz */

				hVdec->ulStdrHFreq = (pCurInfo->pFmtInfo->ulScanWidth * ulSamplingFreq /
					pCurInfo->pFmtInfo->ulPxlFreq);
			}

			/* Default the program value with the standard!  ulProgHFreq will be
			 * updated by Fh loop */
			hVdec->ulProgHFreq = hVdec->ulStdrHFreq;

			/* VDEC_TUNING: Format detection threshold */
			if(VIDEO_FORMAT_IS_625_LINES(pCurInfo->pFmtInfo->eVideoFmt))
			{
				hVdec->ulAutoDetectionThreshold = 5;
			}
			else
			{
				hVdec->ulAutoDetectionThreshold = 15;
			}

			/* Refresh color burst threshold. */
			BVDC_P_Vdec_GetColorBurstThreshold_isr(hVdec, pCurInfo);
		}

		/* For component 480i, the EIA770.1 standard states that there may be a
		 * pedtestal on Y, but EIA770.2 states that there is no pedestal. Set
		 * this register to 0, but please be aware that this register may need
		 * to be set to 42 for some customers. */
		if((pCurDirty->stBits.bMiscCtrl) ||
		   (pCurDirty->stBits.bAdcSetting) ||
		   (pCurDirty->stBits.bInputFormat))
		{
			uint32_t ulSampleFactor;

			/* Init default blank level and offset */
			BVDC_P_Vdec_InitBlackOffset(&hVdec->ulFeBlankLevel, &hVdec->ulBeBlankLevel,
				&hVdec->ulBlackOffset, &hVdec->ulBlackOffsetInCmp, pCurInfo,
				hVdec->eMacrovisionType);

			if(pCurInfo->bCustomCfg)
			{
#if (BVDC_P_REMOVE_PEDESTAL_IN_CMP)
				hVdec->ulBlackOffset      = 0;
				hVdec->ulBlackOffsetInCmp = 0x11c;
#else
				hVdec->ulBlackOffset      = pCurInfo->stVdecSettings.ulBlackOffset;
				hVdec->ulBlackOffsetInCmp = 0;
#endif
			}

			/* Figure out if we need to do double sample or not */
			hVdec->bOverSample = (
				(pCurInfo->stVdecSettings.bOverSample) &&
#if (BVDC_P_SUPPORT_VDEC_VER <= BVDC_P_VDEC_VER_3)
				(!VIDEO_FORMAT_IS_PAL(pCurInfo->pVdcFmt->eVideoFmt)) &&
#endif
				(pCurInfo->pVdcFmt->bSd || pCurInfo->pVdcFmt->bEd) &&
				(!pCurInfo->bPcInput));
			ulSampleFactor = hVdec->bOverSample ? 1 : 0;

			if(hVdec->hSource->ulSampleFactor != ulSampleFactor)
			{
				hVdec->hSource->ulSampleFactor  = ulSampleFactor;
				hVdec->hSource->bPictureChanged = true;
				hVdec->ulDelayStart      = BVDEC_P_BVB_OUT_SLOW_START_COUNT;
				hVdec->bOversampleDirty  = true;
			}

			/* Re evaluate the sample rate */
			BVDC_P_Vdec_SelectSampleRate_isr(hVdec, pCurInfo);
		}

		/* Re-evauate for vdec's CSCes. */
		if((pCurDirty->stBits.bAdcSetting) ||
		   (pCurDirty->stBits.bColorspace) ||
		   (pCurDirty->stBits.bInputFormat))
		{
			if(pCurInfo->bUserFrontendCsc)
			{
				BVDC_P_Csc_FromMatrix_isr(&hVdec->stFrontendCsc,
					pCurInfo->pl32_FrontendMatrix, pCurInfo->ulUserFrontendShift);
			}
			else
			{
				/* If needs arise to store many matrices move them to csc_priv.c */
				static const BVDC_P_CscCoeffs s_stScartFrontendCsc = BVDC_P_MAKE_VDEC_CSC
					(0.9206, 0.4695, 0.1791,  16.0000,
					 7.3277, 0.8020, 7.8702, 128.0000,
					 7.4684, 7.7296, 0.8020, 128.0000 );
				hVdec->stFrontendCsc = s_stScartFrontendCsc;
			}
		}

		/* Reset vcr detection on source change or input format change!  To
		 * recover from the large undetect threshold of VCR.  And a host
		 * of other things. */
		if((pCurDirty->stBits.bAdcSetting) ||
		   (pCurDirty->stBits.bInputFormat) ||
		   (pCurDirty->stBits.bHPllLocked))
		{
			/* PR43823: Fh test by ShibaSoku-color flashing*/
			if((pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_ePAL_G) &&
			    (pCurDirty->stBits.bAdcSetting || pCurDirty->stBits.bInputFormat) &&
			    (pCurInfo->bCvbsInput))
			{
				hVdec->ulYcCrtlLineDelay = 65;
				hVdec->bYcCrtlColorsyncInvert = false;
			}

			/* Delay util go back to normal mode of noise averaging. */
			hVdec->ulNoiseSwitchTimer = BVDC_P_VDEC_NOISE_DELAY_TO_NORMAL;

			/*Noise conditon, clean noise histroy when HPLL status change
			 and use fast times to get a noise level value quickly*/
			hVdec->stNoiseLevel.ulIndex    = 0;
			hVdec->stNoiseLevel.ulTheshold = BVDC_P_VDEC_NOISE_LEVEL_FAST_COUNT;

			/* Each time HPLL change status, clear vsync counter */
			hVdec->ulVsyncCnt = 0;

			/* Pc use different format table with Component*/
			/* PR37669: when input source or analog channel change, reset the counter*/
			/* PR36465, reset DisableRfAutoDetection when channel change */
			if(pCurDirty->stBits.bAdcSetting)
			{
				hVdec->ulFmtId = 0;
				hVdec->ulAdcChangCnt = 0;
				hVdec->bDisableRfAutoDetection = false;
			}

			/* Since for cvbs/svideo/ifd input, framerate detection is disabled,
			 * so need to force framerate detection be to dirty whenever input
			 * source change so that framerate callback will be called to set
			 * correct timebase settings. */
			if((pCurDirty->stBits.bAdcSetting) &&
			   (pCurInfo->bCvbsInput || pCurInfo->bSvidInput || pCurInfo->bIfdInput))
			{
				pCurDirty->stBits.bFrameRateCode = BVDC_P_DIRTY;
			}

			/*delay start for some vsync, don't delay start when RF HPLL status toggle */
			if((!pCurInfo->bIfdInput) && (!hVdec->bVcrHeadSwitch))
			{
				if(pCurInfo->bCompInput || pCurInfo->bPcInput)
				{
					if (pCurInfo->pFmtInfo->ulVertFreq < (50*BFMT_FREQ_FACTOR))
					{
						hVdec->ulDelayStart = BVDEC_P_BVB_OUT_SLOW_START_COUNT_CMP_LOW_FREQ;
					}
					else
					{
						hVdec->ulDelayStart = BVDEC_P_BVB_OUT_SLOW_START_COUNT_CMP;
					}
				}
				else
				{
					hVdec->ulDelayStart = BVDEC_P_BVB_OUT_SLOW_START_COUNT;
				}
			}
			else
			{
				if((hVdec->bVcrHeadSwitch) && (pCurDirty->stBits.bAdcSetting))
				{
					hVdec->ulDelayStart = BVDEC_P_BVB_OUT_SLOW_START_COUNT;
				}
				if((pCurInfo->bIfdInput) && (pCurDirty->stBits.bAdcSetting || pCurDirty->stBits.bInputFormat))
				{
#if (BVDC_P_VDEC_FAST_SECAM_PAL_DETECTION)
					/* Faster format detection between SECAM <-> PAL */
					if(!(VIDEO_FORMAT_IS_PAL(pCurInfo->pFmtInfo->eVideoFmt) ||
					     VIDEO_FORMAT_IS_SECAM(pCurInfo->pFmtInfo->eVideoFmt)))
#endif
					{
						hVdec->ulDelayStart = BVDEC_P_BVB_OUT_SLOW_START_COUNT;
					}
				}
			}

			if(hVdec->bHPllLocked)
			{
				if(pCurInfo->bCvbsInput || pCurInfo->bSvidInput || pCurInfo->bIfdInput)
				{
					hVdec->ulColorBurstRef  = pCurInfo->pCdCfg->ulAcgcRef;
					hVdec->bManualColorMode = true;
					hVdec->ulColorBurstGain = BVDC_P_ACGC_GAIN_NOMINAL;
					hVdec->stColorBurstAmp.ulIndex    = 0;
					hVdec->stColorBurstAmp.ulValue    = hVdec->ulColorBurstRef;
					hVdec->stColorBurstAmp.ulTheshold = BVDC_P_VDEC_COLORBURST_AMP_FAST_COUNT;
				}
			}

			/* VCR threshold. */
			hVdec->ulVcrTheshold  = 0;
			hVdec->bVcrHeadSwitch = false;

			/*PR40449: Workaround for special 1080i/720p component streams with
			 * bi-levle Hsync insteadof tri-level.*/
			if(pCurDirty->stBits.bAdcChange || pCurDirty->stBits.bInputFormat)
			{
				hVdec->bHdBilevelDetected = false;
				hVdec->ulHdBilevelSwitchCnt = 0;
				hVdec->bFieldAvgErr = false;
				hVdec->ulFieldAvgErrCnt = 0;
			}
		}

		if((pCurDirty->stBits.bMiscCtrl)   ||
		   (pCurDirty->stBits.bVdecTuning) ||
		   (pCurDirty->stBits.bAdcSetting) ||
		   (pCurDirty->stBits.bVcrHeadSwitch))
		{
			/* Turn off if user turn on auto or in vcr mode. */
			hVdec->bManualAcgc = (
				/* (0) SW3556-1191: forced into auto mode for rf_frame detected. */
				(!hVdec->b240pMode || !pCurInfo->bIfdInput) &&

				/* (1) forced into auto mode for vcr detected. */
				(!hVdec->bVcrHeadSwitch) &&

				/* (2) user must turn off auto */
				( pCurInfo->stVdecSettings.bBypassAcgc) &&

				/* (3) user must turn off auto */
				(!VIDEO_FORMAT_IS_SECAM(pCurInfo->pFmtInfo->eVideoFmt)) &&

				/* (4) only apply for these inputs */
				(pCurInfo->bIfdInput || pCurInfo->bCvbsInput || pCurInfo->bSvidInput));

			/* In HW Acgc we need to restore things to HW control */
			if(!hVdec->bManualAcgc)
			{
				hVdec->bChromaPresent    = true;
				hVdec->bChromaLock       = true;
				hVdec->bClearSubCarAccum = false;
			}
			pCurDirty->stBits.bManualAcgc = BVDC_P_DIRTY;
		}

		/* Need to restore to standard h frequency when by pass Fh. */
		if(((pCurDirty->stBits.bVcrHeadSwitch) && (hVdec->bVcrHeadSwitch)) || (pCurDirty->stBits.bAdcSetting))
		{
			hVdec->ulProgHFreq = hVdec->ulStdrHFreq;
		}

		/* Adjust Digital Gain Reference Level base on copy protection.  And the
		 * blank select.  ulBlankSelLines specifies which lines in the top field
		 * use the Blank_Level measured during VBI region instead of the expected
		 * blank level specified by REFERENCE_LEVEL. */
		if(pCurDirty->stBits.bMvDetected || pCurDirty->stBits.bHPllLocked ||
		   pCurDirty->stBits.bAdcSetting || pCurDirty->stBits.bInputFormat)
		{
			/* ulBlankSelLines adjustment; */
			if((!hVdec->bHPllLocked) ||
			   (!hVdec->eMacrovisionType))
			{
				hVdec->ulBlankSelLines = 0;
			}
			else
			{
				hVdec->ulBlankSelLines   =
					 (pCurInfo->pVdcFmt->bEd) ? 0xfffff  /* 20-lines */
					:(pCurInfo->pVdcFmt->bSd) ? 0x7ffff  /* 19-lines */
					: 0; /* copy protection support for hd */
			}

			/* Update Dcr, YCrCb delay, Cti. */
			if(!pCurInfo->stVdecSettings.pAwc)
			{
				BVDC_P_Vdec_InitAwc(&pNewInfo->stAwc, pNewInfo);
				BVDC_P_Vdec_InitAwc(&pCurInfo->stAwc, pCurInfo);
			}
			if(!pCurInfo->stVdecSettings.pCti)
			{
				BVDC_P_Vdec_InitCti(&pNewInfo->stCti, pNewInfo);
				BVDC_P_Vdec_InitCti(&pCurInfo->stCti, pCurInfo);
			}
			if(!pCurInfo->stVdecSettings.pDelay)
			{
				BVDC_P_Vdec_InitDelay(&pNewInfo->stDelay, pNewInfo);
				BVDC_P_Vdec_InitDelay(&pCurInfo->stDelay, pCurInfo);
			}
			if(!pCurInfo->stVdecSettings.pDGain)
			{
				BVDC_P_Vdec_InitBackend(&pNewInfo->stBackend, pNewInfo);
				BVDC_P_Vdec_InitBackend(&pCurInfo->stBackend, pCurInfo);
			}
			if(!pCurInfo->stVdecSettings.pDcr)
			{
				BVDC_P_Vdec_InitDcr(&pNewInfo->stDcr, pNewInfo, hVdec->eMacrovisionType);
				BVDC_P_Vdec_InitDcr(&pCurInfo->stDcr, pCurInfo, hVdec->eMacrovisionType);
			}
			if(!pCurInfo->stVdecSettings.pDGain)
			{
				BVDC_P_Vdec_InitDigitalGain(&pNewInfo->stDGain, pNewInfo, hVdec->eMacrovisionType);
				BVDC_P_Vdec_InitDigitalGain(&pCurInfo->stDGain, pCurInfo, hVdec->eMacrovisionType);
			}
		}

		/* If format detection is dirty and user turn it off; need to put
		 * dc restore back to vdec. */
		if((pCurDirty->stBits.bAutoDetectFmt) &&
		   (!pCurInfo->bAutoFmtDetection))
		{
			hVdec->ulClampSel = BCHP_VD_TOP_0_VD_VDEC_0_CTL_FD_CLAMP_SEL_VDEC;
		}

		/* Adjust the YC settings base on game mode */
		if(pCurDirty->stBits.bMiscCtrl)
		{
			BVDC_P_Vdec_GetFmtCfg_isr(pCurInfo->pFmtInfo, pCurInfo->stVdecSettings.e3DGameMode,
				&pCurInfo->pCdCfg, &pCurInfo->pYcCfg, &pCurInfo->pPcInCfg);
		}

		/* Do we need to alloc/free 3D comb memory.  Currently pi alloc
		 * comb memory if:
		 * (1) input is cvbs/ifd input
		 * (2) user enable combing
		 * (3) at least one window connected.
		 * (4) format change require different mem size. */
		if((pCurDirty->stBits.b3DComb) ||
		   (pCurDirty->stBits.bMiscCtrl) ||
		   (pCurDirty->stBits.bWindowNum) ||
		   (pCurDirty->stBits.bAdcSetting) ||
		   (pCurDirty->stBits.bInputFormat))
		{
			#define BVDC_P_3D_COMB_SAMPLES_CNT_PER_BURST     (176)
			bool bNeedCombMem;
			uint32_t ul3DCombBufCnt;
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4) /* 3548 */
			uint32_t ul3DCombBufSize; /* in bytes */

			if(VIDEO_FORMAT_IS_525_LINES(pCurInfo->pFmtInfo->eVideoFmt))
			{
				/* oversampled 1716 samples per line;
				    525 lines per frame;
				    plus +/-BVDC_P_VDEC_FH_CLK_THRESHOLD samples of tolerance for sample count;
				    256 bytes per burst;
				    176 samples per burst (10-bit per sample packed in 256 bytes burst);
				  */
				ul3DCombBufSize = ((1716 * 525 + 2 * BVDC_P_VDEC_FH_CLK_THRESHOLD)
					* 256 + BVDC_P_3D_COMB_SAMPLES_CNT_PER_BURST - 1) /
					BVDC_P_3D_COMB_SAMPLES_CNT_PER_BURST;
			}
			else /* 625 lines per frame */
			{
				/* oversampled 1728 samples per line;
				    625 lines per frame;
				    plus extra tolerance;
				    256 bytes per burst;
				    176 samples per burst (10-bit per sample packed in 256 bytes burst);
				  */
				ul3DCombBufSize = ((1728 * 625 + 2 * BVDC_P_VDEC_FH_CLK_THRESHOLD)
					* 256 + BVDC_P_3D_COMB_SAMPLES_CNT_PER_BURST - 1) /
					BVDC_P_3D_COMB_SAMPLES_CNT_PER_BURST;
			}

			/* 1/2 for saving mode */
			if((BVDC_Vdec_3DGameMode_eGameSave == pCurInfo->stVdecSettings.e3DGameMode) ||
			   (BVDC_Vdec_3DGameMode_eLegacySave == pCurInfo->stVdecSettings.e3DGameMode))
			{
				ul3DCombBufSize /= 2;
			}

			/* NTSC/PAL: frame stores */
			hVdec->ul3DCombVsyncDelay = ul3DCombBufCnt = VIDEO_FORMAT_IS_NTSC(pCurInfo->pFmtInfo->eVideoFmt)
				? 2 : 4;

			/* What heap id to use? */
			hVdec->e3DCombHeapId = BVDC_P_BufferHeapId_eHD;

			/* Guardband total 3D comb buffer size */
			ul3DCombBufSize = ul3DCombBufSize * ul3DCombBufCnt +
				(BVDC_P_RESET_YC_FOR_3DCOMB ? 256 : 0);

			ul3DCombBufCnt = (ul3DCombBufSize +
				hVdec->hSource->hHeap->aulHeapSize[hVdec->e3DCombHeapId] - 1) /
				hVdec->hSource->hHeap->aulHeapSize[hVdec->e3DCombHeapId];
#else
			ul3DCombBufCnt = VIDEO_FORMAT_IS_PAL(pCurInfo->pFmtInfo->eVideoFmt)
				? BVDC_P_MAX_3DCOMB_SD_BUFFER_COUNT
				: BVDC_P_MAX_3DCOMB_SD_BUFFER_COUNT / 2;

			/* Only need half the number of buffer if buffer is 444 */
			ul3DCombBufCnt /= BPXL_IS_YCbCr444_10BIT_FORMAT(
				hVdec->hSource->hHeap->stSettings.ePixelFormat_SD) ? 2 : 1;
			hVdec->e3DCombHeapId = BVDC_P_BufferHeapId_eSD;
#endif
			bNeedCombMem = (
				(!hVdec->hSource->bPending) &&
				(pCurInfo->ulWindows) &&
				(pCurInfo->bEnable3DComb) &&
				(pCurInfo->bCvbsInput || pCurInfo->bIfdInput));

			/* (1) Different memsize free so can be realloc */
			if(hVdec->ul3DCombBufCnt != ul3DCombBufCnt)
			{
				if(hVdec->apHeapNode[0])
				{
					BVDC_P_BUF_MSG(("Vdec[%d] free  %d buffers (%s)", hVdec->eId, hVdec->ul3DCombBufCnt,
						BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hVdec->e3DCombHeapId)));
					BVDC_P_BufferHeap_FreeBuffers_isr(hVdec->hSource->hHeap,
						hVdec->apHeapNode, hVdec->ul3DCombBufCnt, true);
					hVdec->apHeapNode[0] = NULL;
				}
				pCurDirty->stBits.b3DComb = BVDC_P_DIRTY;
				hVdec->ul3DCombBufCnt = ul3DCombBufCnt;
			}

			/* (2) Now alloc */
			if(!hVdec->apHeapNode[0] && bNeedCombMem)
			{
				BERR_Code  err = BERR_SUCCESS;
				BVDC_P_BUF_MSG(("Vdec[%d] alloc %d buffers (%s) ", hVdec->eId, hVdec->ul3DCombBufCnt,
					BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hVdec->e3DCombHeapId)));
				err = BVDC_P_BufferHeap_AllocateBuffers_isr(hVdec->hSource->hHeap,
					hVdec->apHeapNode, hVdec->ul3DCombBufCnt, true,
					hVdec->e3DCombHeapId, BVDC_P_BufferHeapId_eUnknown);
					/* Not enough memory, dump out configuration */
					if(err == BERR_OUT_OF_DEVICE_MEMORY)
					{
						BDBG_ERR(("Vdec[%d] Not enough memory for 3D Comb! Configuration:", hVdec->eId));
						BDBG_ERR(("Vdec[%d] ul3DCombBufCnt: %d",
							hVdec->eId, hVdec->ul3DCombBufCnt));
					}
					BDBG_ASSERT(BERR_SUCCESS == err);
			}
			else if(hVdec->apHeapNode[0] && !bNeedCombMem)
			{
				BVDC_P_BUF_MSG(("Vdec[%d] free  %d buffers (%s)", hVdec->eId, hVdec->ul3DCombBufCnt,
					BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hVdec->e3DCombHeapId)));
				BVDC_P_BufferHeap_FreeBuffers_isr(hVdec->hSource->hHeap,
					hVdec->apHeapNode, hVdec->ul3DCombBufCnt, true);
				hVdec->apHeapNode[0] = NULL;
			}
		}

		/* manual clock and manual phase in PC input */
		if(pCurDirty->stBits.bManualClk)
		{
			hVdec->bToggleHthreshold = true;
			hVdec->ulCountToggleHthreshold = 30;

			ulCurrentLineLength = BREG_Read32_isr(hVdec->hReg, BCHP_PC_IN_0_DPLL_CTRL);
			ulCurrentLineLength = BCHP_GET_FIELD_DATA(ulCurrentLineLength, PC_IN_0_DPLL_CTRL, LINE_LENGTH);
			ulNewLineLength = ((pCurInfo->pFmtInfo->ulScanWidth) *
			(BVDC_P_PC_CLOCK_ADJ_FACTOR + pCurInfo->stPcInSyncSettings.lClockAdj) +
			 BVDC_P_PC_CLOCK_ADJ_FACTOR / 2) / BVDC_P_PC_CLOCK_ADJ_FACTOR;
			if(ulCurrentLineLength != ulNewLineLength)
			{
				hVdec->bPcFieldSyncTypeToggle = true;
				hVdec->bPcPositionLocked = false;
				hVdec->ulPcPositionLockdelay = 0;
			}
		}

		/* SW3556-1026: Horizontal dot noise in AV/SCART.*/
		if((pCurDirty->stBits.bMvDetected) ||
		   (pCurDirty->stBits.bVcrHeadSwitch))
		{
			hVdec->ulLineKAccGain = (
				(hVdec->bVcrHeadSwitch) &&
				(hVdec->eMacrovisionType)) ? 3 : 1;
		}
	}

	/* Countdown until switch back to normal. average times to normal value*/
	if(hVdec->ulNoiseSwitchTimer)
	{
		hVdec->ulNoiseSwitchTimer--;
		if(!hVdec->ulNoiseSwitchTimer)
		{
			hVdec->stNoiseLevel.ulTheshold = BVDC_P_VDEC_NOISE_LEVEL_NORM_COUNT;
		}
	}

	/* Countdown before start feeding out */
	if(hVdec->ulDelayStart)
	{
		hVdec->ulDelayStart--;
	}

	if(hVdec->ulFormatChangeTimer < 100)
	{
		hVdec->ulFormatChangeTimer++;
	}

	if(hVdec->ulHdBilevelSwitchCnt < 300)
	{
		hVdec->ulHdBilevelSwitchCnt++;
	}
	/* Increase vsync counter each vsync */
	if(hVdec->ulVsyncCnt < BVDEC_P_VSYNC_COUNTER_MAX)
	{
		hVdec->ulVsyncCnt++;
	}

	/* Increase ADC change counter each vsync */
	if(hVdec->ulAdcChangCnt < BVDEC_P_VSYNC_COUNTER_MAX)
	{
		hVdec->ulAdcChangCnt++;
	}

	/* Start feed? */
	hVdec->hSource->bStartFeed = (
		!hVdec->ulDelayStart &&
		 hVdec->bVideoDetected);

	return;
}


/***************************************************************************
 * {private}
 *
 * Get Vdec status.
 */
void BVDC_P_Vdec_GetStatus_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  uint32_t                        *pulVdecPhase,
	  bool                            *pbVideoDetected,
	  bool                            *pbVcrHeadSwitch,
	  bool                            *pb240pMode,
	  bool                            *pbDrTf )
{
	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);

	/* (1) VDEC's Phase */
	if(pulVdecPhase)
	{
		*pulVdecPhase = hVdec->ulVdecPhase;
	}

	/* (2) VDEC's input video detected? */
	if(pbVideoDetected)
	{
		*pbVideoDetected = hVdec->bVideoDetected && (!hVdec->ulDelayStart);
	}

	/* (3) VDEC's input video is VCR or noise? */
	if(pbVcrHeadSwitch)
	{
		*pbVcrHeadSwitch = hVdec->bVcrHeadSwitch || hVdec->bNoisy;
	}

	/* (4) VDEC's input video is 240p? */
	if(pb240pMode)
	{
		*pb240pMode = hVdec->b240pMode;
	}

	if(pbDrTf)
	{
		*pbDrTf = hVdec->bDrTf;
	}

	return;
}


/***************************************************************************
 *
 */
void BVDC_P_Vdec_Bringup_isr
	( const BVDC_P_Vdec_Handle         hVdec )
{
	uint32_t i;
	BRDC_List_Handle hList;
	BRDC_Slot_Handle hSlot;
	BVDC_P_ListInfo stList, *pList;
	BVDC_P_Source_Info *pCurInfo;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	BDBG_OBJECT_ASSERT(hVdec->hSource, BVDC_SRC);

	BVDC_P_SRC_NEXT_RUL(hVdec->hSource, BAVC_Polarity_eTopField);
	hSlot = BVDC_P_SRC_GET_SLOT(hVdec->hSource, BAVC_Polarity_eTopField);
	hList = BVDC_P_SRC_GET_LIST(hVdec->hSource, BAVC_Polarity_eTopField);
	pCurInfo = &hVdec->hSource->stCurInfo;

	/* Force execute the first RUL, since VDEC isn't not yet running. */
	for(i = 0; i < hVdec->hSource->ulSlotUsed; i++)
	{
		BRDC_Slot_ExecuteOnTrigger_isr(hVdec->hSource->ahSlot[i],
			hVdec->hSource->aeTrigger[i], true);
	}

	/* This initialized the list to be contained the full VDEC registers
	 * initialization, sub-sequences build RUL only build what need. */
	pList = &stList;
	BVDC_P_ReadListInfo_isr(pList, hList);

	/* Build RULs here */
	BVDC_P_Vdec_ResetRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
	/* KLUDGE: This need be build in certain order, the PC-PLL need
	 * to be program before blocks below.*/
	if(pCurInfo->bUsePllClk)
	{
		BVDC_P_Vdec_BuildPllRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
		BVDC_P_Vdec_BuildPcInRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
	}

	BVDC_P_Vdec_BuildAdcRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
	BVDC_P_Vdec_BuildTopCtrlRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
	BVDC_P_Vdec_BuildFeRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
	BVDC_P_Vdec_BuildCdRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
	BVDC_P_Vdec_BuildAcgcRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
	BVDC_P_Vdec_BuildYcRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
	BVDC_P_Vdec_BuildBeCscRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
	BVDC_P_Vdec_BuildBeRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
	BVDC_P_Vdec_BuildFdRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);

#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
	BVDC_P_Vdec_BuildPassThruRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
#endif

	/* Build drain RUL .  Only need to build it once. */
	BVDC_P_Drain_BuildRul_isr(&hVdec->hSource->stDrain, pList);

	/* Update the list, and commit to slot.  Kick start. */
	BVDC_P_WriteListInfo_isr(pList, hList);
	BRDC_Slot_SetList_isr(hSlot, hList);
	BRDC_Slot_Execute_isr(hSlot);

	BDBG_MSG(("Bring up VDEC[%d]source.", hVdec->eId));
	return;
}


/***************************************************************************
 * {private}
 *
 * Build vdec RUL.
 */
void BVDC_P_Vdec_BuildRul_isr
	( BVDC_P_Vdec_Handle               hVdec,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldId )
{
	BVDC_P_Source_Info *pCurInfo;
	BVDC_P_Source_DirtyBits *pCurDirty, *pOldDirty;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);
	BDBG_OBJECT_ASSERT(hVdec->hSource, BVDC_SRC);

	pOldDirty = &hVdec->hSource->astOldDirty[eFieldId];
	pCurInfo  = &hVdec->hSource->stCurInfo;
	pCurDirty = &pCurInfo->stDirty;

	/* Clear old dirty bits. */
	if(BVDC_P_IS_DIRTY(pOldDirty))
	{
		if(!pList->bLastExecuted)
		{
			BVDC_P_OR_ALL_DIRTY(pCurDirty, pOldDirty);
		}
		else
		{
			BVDC_P_CLEAN_ALL_DIRTY(pOldDirty);

			/*clean VDEC fine tuning flag*/
			if(BVDC_P_IS_CLEAN(pCurDirty))
			{
				hVdec->bOversampleDirty               = false;
				hVdec->bVdecTuningDirtyBitCcr         = false;
				hVdec->bVdecTuningDirtyBitBilevel     = false;
				hVdec->bVdecTuningDirtyBitResetHsync  = false;
				hVdec->bVdecTuningDirtyBitResetHStart = false;
			}
		}
	}

	/* Let's see what we need to build. */
	if(BVDC_P_IS_DIRTY(pCurDirty))
	{
		BDBG_MSG(("pCurDirty[%d] = 0x%08x", hVdec->ulVsyncCnt, pCurDirty->aulInts[0]));

		if(pCurDirty->stBits.bAdcSetting)
		{
			BDBG_MSG(("aeAdcInput[0]  = %x  dirty = %d",
				pCurInfo->aeAdcInput[0], pCurDirty->stBits.bAdcSetting));
		}
		if(pCurDirty->stBits.b3DComb)
		{
			BDBG_MSG(("b3DComb = %d, dirty = %d, fh_ok = %d, fv_ok = %d, clk_ok = %d, ccr = %d, wins = %d, noise_ok = %d, vcr_ok = %d",
				hVdec->b3DComb, pCurDirty->stBits.b3DComb, hVdec->bFh3DComb,
				hVdec->bFv3DComb, hVdec->bClk3DComb, hVdec->bCcrEnable,
				pCurInfo->ulWindows, !hVdec->bNoisy, !hVdec->bVcrHeadSwitch));
		}
		if(pCurDirty->stBits.bVcrHeadSwitch)
		{
			BDBG_MSG(("bVcrHeadSwitch = %d, dirty = %d, thshd = %d",
				hVdec->bVcrHeadSwitch, pCurDirty->stBits.bVcrHeadSwitch, hVdec->ulVcrTheshold));
		}
		if(pCurDirty->stBits.bVideoDetected)
		{
			BDBG_MSG(("bVideoDetected = %d, dirty = %d",
				hVdec->bVideoDetected, pCurDirty->stBits.bVideoDetected));
		}
		if(pCurDirty->stBits.bMvDetected)
		{
			BDBG_MSG(("bMvDetected    = %d, dirty = %d, bMvAgc = %d, ulMvCs = %d",
				hVdec->eMacrovisionType, pCurDirty->stBits.bMvDetected,
				hVdec->bMvAgc, hVdec->ulMvCs));
		}
		if(pCurDirty->stBits.bHPllLocked)
		{
			BDBG_MSG(("bHPllLocked    = %d, dirty = %d,",
				hVdec->bHPllLocked, pCurDirty->stBits.bHPllLocked));
		}
		if(pCurDirty->stBits.bFrameRateCode)
		{
			BDBG_MSG(("eFrameRateCode = %d, dirty = %d,",
				hVdec->hSource->eFrameRateCode, pCurDirty->stBits.bFrameRateCode));
		}
		if(pCurDirty->stBits.bNoisy)
		{
			BDBG_MSG(("bNoisy         = %d, dirty = %d,",
				hVdec->bNoisy, pCurDirty->stBits.bNoisy));
		}
		if(pCurDirty->stBits.bFvFhShift)
		{
			BDBG_MSG(("ulLinePerField = %d, dirty = %d, s_hfreq = %d, p_hfreq = %d",
				hVdec->ulLinePerField, pCurDirty->stBits.bFvFhShift,
				hVdec->ulStdrHFreq, hVdec->ulProgHFreq));
		}
		if(pCurDirty->stBits.bManualAcgc)
		{
			BDBG_MSG(("bManualAcgc    = %d, dirty = %d, timer = %d, gain = %d",
				hVdec->bManualAcgc, pCurDirty->stBits.bManualAcgc,
				hVdec->stColorBurstAmp.ulTheshold, hVdec->ulColorBurstGain));
		}
		if(pCurDirty->stBits.bManualSync)
		{
			BDBG_MSG(("bManualSync dirty = %d,", pCurDirty->stBits.bManualSync));
		}
		if(pCurDirty->stBits.bManualClk)
		{
			BDBG_MSG(("bManualClk  dirty = %d,", pCurDirty->stBits.bManualClk));
		}
		if(pCurDirty->stBits.bManualPos)
		{
			BDBG_MSG(("bManualPos     = %d, dirty = %d,",
				pCurInfo->stPcInSyncSettings.bManualPosition, pCurDirty->stBits.bManualPos));
		}
		if(pCurDirty->stBits.bAutoDetectFmt)
		{
			BDBG_MSG(("AutoDetect     = %d, dirty = %d, ulNumFormats = %d",
				pCurInfo->bAutoFmtDetection, pCurDirty->stBits.bAutoDetectFmt, pCurInfo->ulNumFormats));
		}
		if(pCurDirty->stBits.bInputFormat)
		{
			BDBG_MSG(("eVideoFmt      = %d, dirty = %d, %s",
				pCurInfo->pFmtInfo->eVideoFmt, pCurDirty->stBits.bInputFormat,
				pCurInfo->pFmtInfo->pchFormatStr));
		}
		if(pCurDirty->stBits.bColorspace)
		{
			BDBG_MSG(("bUserCsc       = %d, dirty = %d",
				pCurInfo->bUserCsc, pCurDirty->stBits.bColorspace));
		}
		BDBG_MSG(("------------------------------intP%d", eFieldId));
	}

	if(BVDC_P_IS_DIRTY(pCurDirty) || hVdec->hSource->bDeferSrcPendingCb)
	{
		/* Selectively build only needed registers */
		if((pCurDirty->stBits.bAdcSetting) ||
		   (pCurDirty->stBits.bInputFormat))
		{
			BVDC_P_Vdec_ResetRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
			/* KLUDGE: This need be build in certain order, the PC-PLL need
			 * to be program before blocks below.*/
			if(pCurInfo->bUsePllClk)
			{
				BVDC_P_Vdec_BuildPllRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
				BVDC_P_Vdec_BuildPcInRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
			}

			BVDC_P_Vdec_BuildAdcRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
			BVDC_P_Vdec_BuildTopCtrlRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
			BVDC_P_Vdec_BuildFeRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
			BVDC_P_Vdec_BuildCdRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
			BVDC_P_Vdec_BuildAcgcRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
			BVDC_P_Vdec_BuildYcRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
			BVDC_P_Vdec_BuildBeCscRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
			BVDC_P_Vdec_BuildBeRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
			BVDC_P_Vdec_BuildFdRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
			BVDC_P_Drain_BuildFormatRul_isr(&hVdec->hSource->stDrain, &hVdec->hSource->stScanOut,
				pCurInfo->pFmtInfo, pList);
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
			BVDC_P_Vdec_BuildPassThruRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
#endif
		}
		else
		{
#if (BVDC_P_USE_WORK_AROUNDS)
			/* Individually trigger */
			if((pCurDirty->stBits.bNoisy) ||
			   (pCurDirty->stBits.bWinChanges) ||
			   (pCurDirty->stBits.bMiscCtrl) )
			{
				if(hVdec->bOversampleDirty)
				{
					BVDC_P_Vdec_ResetBeRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
				}
				BVDC_P_Vdec_BuildAdcRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
				BVDC_P_Vdec_BuildBeRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
			}
			if((pCurDirty->stBits.bMiscCtrl) ||
			   (pCurDirty->stBits.bWinChanges) ||
			   (pCurDirty->stBits.bAutoDetectFmt))
			{
				BVDC_P_Vdec_BuildTopCtrlRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
			}
			if((pCurDirty->stBits.bManualSync) ||
			   (pCurDirty->stBits.bManualClk))
			{
				if(pCurDirty->stBits.bManualSync && hVdec->bPcPositionLocked && !hVdec->bPcFieldSyncTypeToggle)
				{
					BVDC_P_Vdec_BuildFeFiledSyncRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
				}
				BVDC_P_Vdec_CalManualClock_isr(hVdec, &hVdec->hSource->stCurInfo);
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
				BVDC_P_Vdec_BuildPcInRaRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
#endif
				BVDC_P_Vdec_BuildPcInSyncRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
				BVDC_P_Vdec_BuildPllSyncRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
			}
			if((pCurDirty->stBits.bMiscCtrl) ||
			   (pCurDirty->stBits.bWinChanges) ||
			   (pCurDirty->stBits.bMvDetected) ||
			   (pCurDirty->stBits.bHPllLocked) ||
			   (pCurDirty->stBits.bVcrHeadSwitch) ||
			   (pCurDirty->stBits.bNoisy))
			{
				BVDC_P_Vdec_BuildFeRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
			}
			if((pCurDirty->stBits.bFvFhShift) ||
			   (pCurDirty->stBits.bManualClk && hVdec->bPcFieldSyncTypeToggle))
			{
				BVDC_P_Vdec_BuildFeSyncRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
			}
			if(pCurDirty->stBits.bManualClk && hVdec->bPcFieldSyncTypeToggle)
			{
				BVDC_P_Vdec_BuildFeFiledSyncRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
			}
			if(pCurDirty->stBits.bManualPos)
			{
				BVDC_P_Vdec_BuildFeHVStartRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
				BVDC_P_Vdec_BuildPcInRaRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
#endif
			}
			if(pCurDirty->stBits.bManualAcgc)
			{
				BVDC_P_Vdec_BuildAcgcRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
			}
			if((pCurDirty->stBits.bMiscCtrl) ||
			   (pCurDirty->stBits.bManualAcgc) ||
			   (pCurDirty->stBits.bVcrHeadSwitch))
			{
				BVDC_P_Vdec_BuildCdRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
			}
			if(pCurDirty->stBits.bColorspace)
			{
				BVDC_P_Vdec_BuildFeCscRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
				BVDC_P_Vdec_BuildBeCscRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
			}
#endif
			if((pCurDirty->stBits.b3DComb) ||
			   (pCurDirty->stBits.bMiscCtrl) ||
			   (pCurDirty->stBits.bMvDetected) ||
			   (pCurDirty->stBits.bVcrHeadSwitch))
			{
				BVDC_P_Vdec_BuildYcRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
			}

			if(pCurDirty->stBits.bVdecTuning)
			{
				if(hVdec->bVdecTuningDirtyBitBilevel)
				{
					BVDC_P_Vdec_BuildFeRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
					BVDC_P_Vdec_BuildFeSyncRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
				}
				if(hVdec->bVdecTuningDirtyBitResetHStart)
				{
					BVDC_P_Vdec_BuildCdRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
					BVDC_P_Vdec_BuildBeRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
				}
				if(hVdec->bVdecTuningDirtyBitResetHsync)
				{
					BVDC_P_Vdec_BuildResetHsync_isr(hVdec, pList);
				}
				if(hVdec->bVdecTuningDirtyBitEnableSliceLPFHpllCtrl)
				{
					BVDC_P_Vdec_BuildFeRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
				}
				if(hVdec->bVdecTuningDirtyBitCcr)
				{
					BVDC_P_Vdec_BuildYcRul_isr(hVdec, pList, &hVdec->hSource->stCurInfo);
				}
			}
		}

		/* Callback when following things changes */
		if((pCurInfo->pfGenericCallback) &&
		   ((pCurDirty->stBits.bNoisy)         ||
		    (pCurDirty->stBits.bMvDetected)    ||
		    (pCurDirty->stBits.bGenCallback)   ||
		    (pCurDirty->stBits.bInputFormat)   ||
		    (pCurDirty->stBits.bVideoDetected) ||
		    (pCurDirty->stBits.bFrameRateCode) ||
		    (hVdec->bFeAgcLevelsUpdated) ||
		    (hVdec->bColorBurstAmpUpdated) ||
		    (hVdec->bScartFuncSelectUpdated) ||
		    (hVdec->hSource->bDeferSrcPendingCb) ||
		    (pCurDirty->stBits.bAddWin && pCurInfo->eResumeMode)))
		{
			BVDC_Source_CallbackData *pCbData = &hVdec->hSource->stSourceCbData;
			BVDC_Source_DirtyBits *pCbDirty = &pCbData->stDirty;

			/* Clear dirty bits */
			BVDC_P_CB_CLEAN_ALL_DIRTY(pCbDirty);

			/* Issue src pending call back when shutdown BVN completed. */
			if(hVdec->hSource->bDeferSrcPendingCb)
			{
				BVDC_P_Source_CheckAndIssueCallback(hVdec->hSource, pCbDirty);
			}

			/* Make sure the callback happen at least once, on first
			 * installation of callback to report the current status. */
			if(pCurDirty->stBits.bGenCallback)
			{
				/* Which one triggers callback? */
				pCbDirty->bMacrovision   = BVDC_P_DIRTY;
				pCbDirty->bActive        = BVDC_P_DIRTY;
				pCbDirty->bFmtInfo       = BVDC_P_DIRTY;
				pCbDirty->bFrameRate     = BVDC_P_DIRTY;
				pCbDirty->bNoisy         = BVDC_P_DIRTY;
				pCbDirty->bColorBurstAmp = BVDC_P_DIRTY;
				hVdec->hSource->bDeferSrcPendingCb = true;
			}
			else
			{
				/* Which one triggers callback? */
				pCbDirty->bMacrovision     = pCurDirty->stBits.bMvDetected;
				pCbDirty->bActive          = pCurDirty->stBits.bVideoDetected;
				pCbDirty->bFmtInfo         = pCurDirty->stBits.bInputFormat;
				pCbDirty->bFrameRate       = pCurDirty->stBits.bFrameRateCode;
				pCbDirty->bNoisy           = pCurDirty->stBits.bNoisy;
				pCbDirty->bFeAgcLevels     = hVdec->bFeAgcLevelsUpdated;
				pCbDirty->bColorBurstAmp   = hVdec->bColorBurstAmpUpdated;
				pCbDirty->bScartFuncSelect = hVdec->bScartFuncSelectUpdated;

				/* defer source pending callback until all its windows are shutdown! */
				if((pCurDirty->stBits.bInputFormat || pCurDirty->stBits.bAddWin) &&
				   (pCurInfo->eResumeMode))
				{
					hVdec->hSource->bDeferSrcPendingCb = true;
				}
			}

			/* callback only if something changed */
			if(BVDC_P_CB_IS_DIRTY(pCbDirty))
			{
				/* Update Callback data */
				pCbData->eMacrovisionType   = hVdec->eMacrovisionType;
				pCbData->bCopyProtectionAGC = hVdec->bMvAgc;
				pCbData->ulColorStripe      = hVdec->ulMvCs;
				pCbData->bActive            = hVdec->bVideoDetected;
				pCbData->pFmtInfo           = pCurInfo->pFmtInfo;
				pCbData->eFrameRateCode     = hVdec->hSource->eFrameRateCode;
				pCbData->bNoisy             = hVdec->bNoisy;
				pCbData->ulColorBurstAmp    = hVdec->stColorBurstAmp.ulValue;
				pCbData->eScartFuncSelect0  = hVdec->eScartFuncSelect0;
				pCbData->eScartFuncSelect1  = hVdec->eScartFuncSelect1;

				BKNI_Memcpy(pCbData->aulFeAgcGain, hVdec->aulFeAgcGain,
					sizeof(hVdec->aulFeAgcGain));

				/* Callback application with the above data */
				pCurInfo->pfGenericCallback(pCurInfo->pvGenericParm1,
					pCurInfo->iGenericParm2, (void*)pCbData);
			}
		}

		/* Color Burst Updated to notify callback so application can use
		 * this value to do its manual acgc. */
		hVdec->bFeAgcLevelsUpdated = false;
		hVdec->bColorBurstAmpUpdated = false;
		hVdec->bScartFuncSelectUpdated = false;

		/* Clear dirty bits. */
		*pOldDirty = *pCurDirty;
		BVDC_P_CLEAN_ALL_DIRTY(pCurDirty);
	}

	/* These entries always get build vsync. */
	BVDC_P_Vdec_BuildVsynRul_isr(hVdec, pList, pCurInfo);

	return ;
}

#else /* BVDC_P_SUPPORT_VDEC */

/***************************************************************************
 * Stub function for chipset with no VDEC.
 */
BERR_Code BVDC_P_Vdec_Create
	( BVDC_P_Vdec_Handle              *phVdec,
	  BVDC_P_VdecId                    eVdecId,
	  BREG_Handle                      hReg,
	  BVDC_Source_Handle               hSource )
{
	BSTD_UNUSED(phVdec);
	BSTD_UNUSED(eVdecId);
	BSTD_UNUSED(hReg);
	BSTD_UNUSED(hSource);
	return BERR_SUCCESS;
}
void BVDC_P_Vdec_Destroy
	( BVDC_P_Vdec_Handle               hVdec )
{
	BSTD_UNUSED(hVdec);
	return;
}
void BVDC_P_Vdec_Init
	( BVDC_P_Vdec_Handle               hVdec )
{
	BSTD_UNUSED(hVdec);
	return;
}

BERR_Code BVDC_P_Vdec_ValidateChanges
	( BVDC_P_Vdec_Handle               hVdec )
{
	BSTD_UNUSED(hVdec);
	return BERR_SUCCESS;
}

BERR_Code BVDC_P_Vdec_ValidateAdc
	(  const BVDC_Source_Handle        ahSource[] )
{
	BSTD_UNUSED(ahSource);
	return BERR_SUCCESS;
}
#endif

/* End of File */
