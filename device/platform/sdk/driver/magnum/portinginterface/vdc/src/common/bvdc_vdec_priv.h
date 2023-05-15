/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_vdec_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/258 $
 * $brcm_Date: 10/11/10 5:30p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_vdec_priv.h $
 * 
 * Hydra_Software_Devel/258   10/11/10 5:30p yuxiaz
 * SW7405-4936: Move VDEC revision to top.
 * 
 * Hydra_Software_Devel/257   4/19/10 10:41p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/256   4/7/10 11:36a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/255   4/5/10 4:16p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/254   2/3/10 4:44p pntruong
 * SW3548-2751: Added feature to report vdec agc level.
 *
 * Hydra_Software_Devel/253   2/1/10 3:14p pntruong
 * SW3556-1026: Horizontal dot noise in AV/SCART.
 *
 * Hydra_Software_Devel/252   1/13/10 9:33p pntruong
 * SW3556-967: Corrected the 3dcomb delay reports in callback for pal
 * formats.
 *
 * Hydra_Software_Devel/251   12/22/09 3:48p pntruong
 * SW3556-909: Additional threshold fine tuned for detecting no signal.
 *
 * Hydra_Software_Devel/250   12/15/09 8:27p pntruong
 * SW3556-886: TTX operation is wrong in SECAM signal.  Dynamically turn
 * on/off secam filter in sop/eop. Also use vfd clipping for this case.
 *
 * Hydra_Software_Devel/249   12/9/09 10:26a pntruong
 * SW3556-914, SW3556-880: Improved auto format detection speed.
 *
 * Hydra_Software_Devel/248   12/8/09 6:55p pntruong
 * SW3556-916: Improved default bandfilter for secam.  Refactored the
 * compiling flag for fast format detection but default off.
 *
 * Hydra_Software_Devel/247   12/1/09 4:51p pntruong
 * SW3556-880: Optimized the rf input format detection time.
 *
 * Hydra_Software_Devel/246   11/17/09 10:01p pntruong
 * SW3556-866: Expose vdec frontend csc for adc calibration for scart
 * input.
 *
 * Hydra_Software_Devel/245   10/1/09 6:20p pntruong
 * SW3556-804: Added scart function select callback.
 *
 * Hydra_Software_Devel/244   8/28/09 11:13p pntruong
 * SW3548-2438: [GDVR] The video image shakes at the Composite signal.
 *
 * Hydra_Software_Devel/243   5/12/09 10:45p pntruong
 * PR51338: Added support for adc fullrange raw 11-bit.
 *
 * Hydra_Software_Devel/242   3/31/09 10:26p pntruong
 * PR53597: VDEC failed to quickly lock the tuner output in AFT.
 *
 * Hydra_Software_Devel/241   3/18/09 9:17p pntruong
 * PR52420: Fine tuned color kill.  Put acgc/color mode into auto when
 * detected vcr head switch mode.  Separate out update of cross-chroma
 * and 3dcomb.
 *
 * Hydra_Software_Devel/240   3/11/09 11:41a pntruong
 * PR52952: When force pending for VDEC sources from application, source
 * pending callback happens twice and video present status toggles.
 *
 * Hydra_Software_Devel/239   3/10/09 6:58p pntruong
 * PR52999: Fine tuned color kill parameters.
 *
 * Hydra_Software_Devel/238   3/2/09 6:35p pntruong
 * PR39711: Used the colorstripe status in cd block instead of fe.  New hw
 * implementation routed signal to cd.
 *
 * Hydra_Software_Devel/237   2/26/09 6:12p yuxiaz
 * PR52287: 422-444 and 444-422 converters should use decimate/duplicate.
 *
 * Hydra_Software_Devel/236   2/26/09 3:10p pntruong
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
 * Hydra_Software_Devel/235   2/18/09 1:22p pntruong
 * PR51196: Fined tune kill color in RF to reach burst=7% target.
 *
 * Hydra_Software_Devel/pr51196-take2/1   2/18/09 12:14p xkliu
 * PR51196:[customer] tune kill color in RF to reach burst=7% target.
 *
 * Hydra_Software_Devel/234   2/13/09 4:50p pntruong
 * PR52086: Need toggle Hthreshold for RGB input manual phase adjustment.
 *
 * Hydra_Software_Devel/PR52086/1   2/13/09 4:57p gorgon
 * PR52086: Need toggle Hthreshold for RGB input Manual Phase adjustment
 *
 * Hydra_Software_Devel/233   2/2/09 5:03p yuxiaz
 * PR45484: Updated Dither settings for VDEC_BE.
 *
 * Hydra_Software_Devel/232   1/23/09 6:23p pntruong
 * PR49561: Vertical black bar in ATV snow noise.
 *
 * Hydra_Software_Devel/231   1/21/09 11:33p pntruong
 * PR51336, PR51337, PR51338:  Enabled long/short field fix.  Enabled
 * secam yc separation filter bypass. Enabled full 11-bit full precision.
 *
 * Hydra_Software_Devel/230   1/20/09 7:54p pntruong
 * PR51196 : Tuned color busrt threshold for RF PALM.
 *
 * Hydra_Software_Devel/229   1/16/09 3:54p pntruong
 * PR50097: Toggle SLICE_LPF_HPLL_LOCK_CTL to improved detection of full
 * white pattern.
 *
 * Hydra_Software_Devel/228   1/7/09 11:48a pntruong
 * PR49556: Initialize vdc with adc/pll power off.
 *
 * Hydra_Software_Devel/227   1/5/09 8:24p pntruong
 * PR50722, PR50724: Improve the speed of RGB input auto phase adjustment.
 *
 * Hydra_Software_Devel/226   1/5/09 6:48p pntruong
 * PR49165: FD is not stable when meet some RF channels.
 *
 * Hydra_Software_Devel/225   12/11/08 8:05p pntruong
 * PR50097: Iinvalid format on component input when using 1080p24+full
 * white pattern.
 *
 * Hydra_Software_Devel/224   12/8/08 7:29p pntruong
 * PR49561: PR49849: 1) Rewrite workaround for vertical black bar on the
 * white noise channel. 2) Fine tuned the threshold for kill color and no
 * burst judgement.
 *
 * Hydra_Software_Devel/223   12/1/08 4:44p pntruong
 * PR48892: Use fine tuned value for bandlimit filter setting.
 * PR49257: PR49304: 1) component input 1080p/23.97 and 1080p/24 locked
 * slower than other 1080p formats. 2) It takes long time before HPLL get
 * locked for Component inuput, 1080p@60Hz, full white pattern.
 * PR49192: Remove the workaround for PR40810.
 * PR49561: [VDEC]: Vertical black bar on the white noise channel
 * PR48639: [customer]1line on/off pattern is bluring on component 1080p
 * signals.  Change the bandlimit filter for 1080P component.
 *
 * Hydra_Software_Devel/PR48639/1   11/28/08 7:59p mlei
 * PR49561: [VDEC]: Vertical black bar on the white noise channel
 *
 * Hydra_Software_Devel/222   11/20/08 6:30p pntruong
 * PR46325: Change some register and toggle VDEC_FE_0_FIELD_SYNC.TYPE when
 * manual clock.
 *
 * Hydra_Software_Devel/221   11/19/08 8:45p pntruong
 * PR43661, PR48734: Reset be when oversample is toggle.
 *
 * Hydra_Software_Devel/220   11/17/08 9:48p pntruong
 * PR48892: Allow customization of backend band limiting filter. Refactors
 * blanklevel.
 *
 * Hydra_Software_Devel/219   11/14/08 6:09p pntruong
 * PR47952: PR48969: VDEC: Support HD component signals with bi-level
 * hsync.
 *
 * Hydra_Software_Devel/PR47952/5   11/14/08 5:45p mlei
 * PR48969: VDEC: Support HD component signals with bi-level hsync
 *
 * Hydra_Software_Devel/218   11/7/08 3:02p pntruong
 * PR48538: PAL 60 CVBS is detected as NTSC-M.
 *
 * Hydra_Software_Devel/217   11/6/08 8:03p pntruong
 * PR48538: Don't need clear subcarrier when hpll unlocked.
 * PR46500, PR48679:1) Some fine tuning for PAL/SECAM VCR 2) fine tuing
 * kill color threshold.
 *
 * Hydra_Software_Devel/216   10/28/08 9:31p pntruong
 * PR47895: Send out source pending callback when video is stable.
 *
 * Hydra_Software_Devel/215   10/24/08 6:18p pntruong
 * PR48192: Enable full sample rate and enable 256 phase.
 *
 * Hydra_Software_Devel/PR47952/1   10/23/08 4:48p mlei
 * PR48192: enable full sample rate and 256 phase.
 *
 * Hydra_Software_Devel/214   10/22/08 8:37p pntruong
 * PR48192: Enable full sample rate and enable 256 phase.  216M sample
 * rate in FE
 * PR45566: fix hang up issue in PAL, PAL-Nc Legacy Save mode.
 * PR48196: B0: Fine tuning Y/C delay in 3549B0.
 * PR43517: Fine tune secam auto detection on 3549B0.
 *
 * Hydra_Software_Devel/213   10/20/08 7:36p darnstein
 * PR42419: 3D comb/TTD problem is gone in new VDEC.
 *
 * Hydra_Software_Devel/212   10/17/08 6:24p hongtaoz
 * PR43918: 3548B0 fixed 3D comb mermy corruption;
 *
 * Hydra_Software_Devel/211   10/8/08 12:18p pntruong
 * PR46735: B0: Need SECAM Dr/Db adjustment implemented.
 *
 * Hydra_Software_Devel/210   9/26/08 1:33p pntruong
 * PR46515: Adapted to new power management.
 *
 * Hydra_Software_Devel/209   9/25/08 7:35p pntruong
 * PR46325: Fine tune the RA lock status, and improved locking with
 * remapped formats.
 *
 * Hydra_Software_Devel/208   9/22/08 3:57p pntruong
 * PR46796: Fine tuning HPLL setting for CVBS, Svideo and analog RF.
 *
 * Hydra_Software_Devel/207   9/16/08 10:31a pntruong
 * PR43517: Enable 2d comb for secam auto detection.
 *
 * Hydra_Software_Devel/206   9/15/08 9:17p pntruong
 * PR46514: Upon destroyed and disabled slot disable adc if not used.
 *
 * Hydra_Software_Devel/205   9/2/08 6:06p pntruong
 * PR46314, PR46316: Updated 3dcomb memory allocation, and added buffers
 * alloc debug messages.
 *
 * Hydra_Software_Devel/204   8/12/08 9:31p pntruong
 * PR45566: Added support for standard (legacy) mode of combing.
 *
 * Hydra_Software_Devel/203   8/11/08 5:08p pntruong
 * PR45576: Wrong signal present information for component.
 *
 * Hydra_Software_Devel/PR45214/1   8/11/08 5:35p xkliu
 * PR45576: Wrong signal present information for Component
 *
 * Hydra_Software_Devel/202   8/6/08 4:44p pntruong
 * PR45219:  Added new RGB formats.
 *
 * Hydra_Software_Devel/201   7/30/08 10:13a pntruong
 * PR40213: Fine tuned the hstart of auto raster, and use macro to improve
 * code readability.
 *
 * Hydra_Software_Devel/200   7/29/08 7:50p pntruong
 * PR40213: Used correct alignment macro.
 *
 * Hydra_Software_Devel/199   7/16/08 10:12a pntruong
 * PR44664: compensate scart csc in compositor.
 *
 * Hydra_Software_Devel/198   6/26/08 7:41p pntruong
 * PR44243: Don't ADC Common Mode in 3548/3563D0.
 *
 * Hydra_Software_Devel/197   6/26/08 11:21a pntruong
 * PR44203: [VDEC][PC]:Ensure the H positon is right when finding new
 * format, adjusting manua phase and adjusting manual clock. Change some
 * parameters for 93549/93556.
 *
 * Hydra_Software_Devel/196   6/20/08 2:06p pntruong
 * PR43821, PR43823: Fh test by ShibaSoku-color flashing.
 *
 * Hydra_Software_Devel/195   6/17/08 10:57a pntruong
 * PR43720, PR43719: Remove the workaround in 3548 and 3556. VDEC: Offset
 * for negative CSC coefficients lower than it should be.
 *
 * Hydra_Software_Devel/194   6/11/08 4:39p pntruong
 * PR34711: Put back common mode, since rdb definition is now correct.
 *
 * Hydra_Software_Devel/193   6/10/08 7:37p hongtaoz
 * PR43456: added 3D comb sample count tolerance;
 *
 * Hydra_Software_Devel/192   6/9/08 7:34p pntruong
 * PR34711: Re-enabled 216mhz clock.
 *
 * Hydra_Software_Devel/191   6/9/08 4:18p pntruong
 * PR34806, PR40963: Fine tuned secam and 3dcomb settings.
 *
 * Hydra_Software_Devel/190   6/6/08 7:29p pntruong
 * PR40963: Intial 3DComb bringup.
 *
 * Hydra_Software_Devel/189   6/5/08 3:07p pntruong
 * PR34711: Initial 3548 vdec bringup and added more default for new bits.
 *
 * Hydra_Software_Devel/188   5/29/08 4:41p pntruong
 * PR40449: [VDEC] support 1080i component signals with bi-level hsync.
 *
 * Hydra_Software_Devel/187   5/13/08 10:14a pntruong
 * PR42424: Generalized the digital gain initialization.
 *
 * Hydra_Software_Devel/186   5/12/08 2:56p darnstein
 * PR42416: Implement Grossman's work-around for the conflict between VDEC
 * frame comb filter and teletext decoder (TTD).
 *
 * Hydra_Software_Devel/185   5/2/08 5:47p pntruong
 * PR34806: More scart input works.
 *
 * Hydra_Software_Devel/184   5/1/08 10:38a pntruong
 * PR42070: Optimized PLL setting for pc input.
 *
 * Hydra_Software_Devel/183   4/25/08 6:38p pntruong
 * PR40963: [VDEC] Add 3D Comb Game Mode Support.
 *
 * Hydra_Software_Devel/182   4/23/08 4:04p pntruong
 * PR40213, PR41315, PR41529: Added initial support SECAM and pc auto
 * PR38861: Improved autophase settings.
 *
 * Hydra_Software_Devel/179   3/18/08 7:08p pntruong
 * PR34806: Added initial adc type for scart input.
 * raster.
 *
 * Hydra_Software_Devel/181   4/21/08 11:15p pntruong
 * PR34711: Prepared to add new features into vdec.  Removed vdec1
 * support.
 *
 * Hydra_Software_Devel/180   4/21/08 5:38p pntruong
 *
 * Hydra_Software_Devel/178   3/17/08 10:55a pntruong
 * PR36256: [VDEC][PC]:Ensure the H positon is right when finding new
 * format, adjusting manua phase and adjusting manual clock. Find the
 * issue again when plug on/off.
 *
 * Hydra_Software_Devel/177   3/11/08 10:46p pntruong
 * PR40280: Added support for vdec PAL-60.
 *
 * Hydra_Software_Devel/PR40133/1   3/11/08 10:12a mlei
 * PR40280: need PAL-60 support on 3563
 *
 * Hydra_Software_Devel/176   3/2/08 6:21p pntruong
 * PR38714: PR38714: [93563c0 VDEC] FSC test fail in CVBS with shibasoku
 * tune BVDC_P_VDEC_SUBCARRIER_LOOP_ACC_UNLOCK_NORMAL to 0xB00 from 0x800.
 *
 * Hydra_Software_Devel/175   2/29/08 4:20p yuxiaz
 * PR39158: Implement dithering in various BVN components for 3548.
 *
 * Hydra_Software_Devel/174   2/25/08 2:42p pntruong
 * PR38861: [3563D0]: PC - Auto phase is too slow when use multi vsync
 * phase control fix the streaky.  Merged from branch.
 *
 * Hydra_Software_Devel/173   1/28/08 7:04p pntruong
 * PR36323: Reduce the DCR hand over delay to 3 vsyncs.  Merged from pr
 * branch.
 *
 * Hydra_Software_Devel/172   1/22/08 6:10p pntruong
 * PR38635, PR38742: Bring up 93563 D0 PC input. Disable some workarounds
 * that fixed in D0 pc input. See pr27640, pr34153, pr36256.  Fine tune
 * new settings for d0.
 *
 * Hydra_Software_Devel/171   1/15/08 4:08p pntruong
 * PR38639: [VDEC] The video repeats blinking, when video color burst is
 * too high.
 *
 * Hydra_Software_Devel/170   12/17/07 5:05p pntruong
 * PR37992: Added main/pip with mixing 216Mhz and 108Mhz supports.
 *
 * Hydra_Software_Devel/169   12/17/07 1:21p pntruong
 * PR37992: Added validation of duplicated adc usage, turn off unused adc,
 * protect usage of pll and pc_in block.  Main/Pip work but more work
 * needed for mixing 216Mhz and 108Mhz.
 *
 * Hydra_Software_Devel/168   12/14/07 3:37p pntruong
 * PR37582: Cvbs/svideo PAL-M from Leader can not be detected.
 *
 * Hydra_Software_Devel/167   12/7/07 3:39p pntruong
 * PR37978: Add Intial support for 3563D0.
 *
 * Hydra_Software_Devel/166   12/6/07 11:23p pntruong
 * PR36462: PR36323: account for the macro BVDC_P_VDEC_PR36323_WORK_AROUND
 *
 * Hydra_Software_Devel/165   12/6/07 1:45p pntruong
 * PR36462: PR37669: PR36323: 1, Add source/channel timer. 2: Enable
 * MF_USE_LINE_LENGTH_LOCK.
 * PR37417:some channel was detected as no burst channel, need fine tuning
 * the threshold.
 * PR36465:In analog RF input, auto detection cost too much time.
 *
 * Hydra_Software_Devel/164   12/4/07 1:33p pntruong
 * PR37175: Disabled frame before update address to prevent buffer
 * overrun.
 *
 * Hydra_Software_Devel/163   11/29/07 1:49p pntruong
 * PR36216: [PC]:Support component input formats with PC input.  Tighten
 * const.
 *
 * Hydra_Software_Devel/162   11/28/07 4:48p pntruong
 * PR37175: [VDEC][WXGA] 3Dcomb fails when input is PAL/PAL-N/PAL-M format
 * with WXGA.  Need to allocate correct amount of memory for PAL.
 *
 * Hydra_Software_Devel/161   11/26/07 4:37p pntruong
 * PR36462: PR37417:some channel was detected as no burst channel, need
 * fine tuning the threshold.
 * PR36465:In analog RF input, auto detection cost too much time.
 *
 * Hydra_Software_Devel/160   11/15/07 6:03p pntruong
 * PR32896: Prevent false vcr detection with copy protection contents.
 *
 * Hydra_Software_Devel/159   11/13/07 6:44p pntruong
 * PR36465: In analog RF input, auto detection cost too much time.
 *
 * Hydra_Software_Devel/158   11/13/07 5:35p rpan
 * PR31084: Added support for getting VDEC default settings.
 *
 * Hydra_Software_Devel/157   11/5/07 8:19p pntruong
 * PR36462: PR33011:If CVBS signal doesn't contain burst signal, need S/W
 * assume a default format
 * PR36563:for RF channel, mute the video 20 vysnc when video format
 * change. This is to fix the transition that cause by RF auto detection:
 * PAL-M/PAL-Nc/NTSC
 * PR33797:to fix the transtion and anr hanging up during the switch
 * channel
 * PR36462: decrease the delay for VCR detection
 *
 * Hydra_Software_Devel/156   10/23/07 6:54p pntruong
 * PR36051: If RF signal doesn't conatin color burst signal, there is
 * toggling for format autodetection.
 *
 * Hydra_Software_Devel/155   10/19/07 8:35a pntruong
 * PR36256: [VDEC][PC]:Ensure the H positon is right when finding new
 * format, adjusting manua phase and adjusting manual clock.
 *
 * Hydra_Software_Devel/Gorgon_temp_20071019/1   10/19/07 2:56p gorgon
 * PR36256:[VDEC][PC]:Ensure the H positon is right when finding new
 * format, adjusting manua phase and adjusting manual clock.
 *
 * Hydra_Software_Devel/154   10/15/07 1:31p pntruong
 * PR34153: More fine tune of pc input streaky-noise.
 *
 * Hydra_Software_Devel/Gorgon_temp_PR34153_20071012/2   10/15/07 11:36a gorgon
 * PR34153:fix PC input streaky noise. Change the name of the Macro.
 * Set the default to 0.
 *
 * Hydra_Software_Devel/Gorgon_temp_PR34153_20071012/1   10/15/07 11:07a gorgon
 * PR34153:fix PC input streaky noise. Disable the 8 Phase PLL change, DCR
 * filter, Dither. Use the different phase setting to kill the streaky.
 *
 * Hydra_Software_Devel/153   9/10/07 8:57p pntruong
 * PR34153: Adjust dcr/adc/pll setting to reduce pc noise.  Tablelize pll
 * for ease of customize.
 *
 * Hydra_Software_Devel/152   9/10/07 1:57p pntruong
 * PR34690: [PC]:Smooth the performance of manual clock. Toggle the
 * H_threshold when manual clock to make sure the position accurate.
 *
 * Hydra_Software_Devel/PR34690/2   9/10/07 9:03p gorgon
 * PR32296:[VDEC]: PC input: There is different H positon performance with
 * the same Hstart. Change the delay time to 300 vsyncs.
 *
 * Hydra_Software_Devel/PR34690/1   9/10/07 8:58p gorgon
 * PR34690:[PC]:Smooth the performance of manual clock. Toggle the
 * H_threshold when manual clock to make sure the position accurate.
 *
 * Hydra_Software_Devel/151   9/7/07 7:24p pntruong
 * PR34681: Video mute one second when sync level decreased to 30%.
 *
 * Hydra_Software_Devel/150   9/6/07 4:27p pntruong
 * PR34153: Support customized pll, adc, dither, filter, to improve pc-
 * streaky noise.
 *
 * Hydra_Software_Devel/149   9/4/07 1:49p pntruong
 * PR34428, PR30691, PR31623, PR33981, PR33545, PR32402: Merged fixes from
 * field tests with customers.
 *
 * Hydra_Software_Devel/PR34428/1   9/3/07 7:26p mlei
 * PR33545:[VDEC] Optimize HPLL programming to improve noise handling.
 *
 * Hydra_Software_Devel/148   8/30/07 12:07p pntruong
 * PR33614:  Removed vnet reset, and delay auto-trigger to avoid re-enable
 * errors in anr.
 *
 * Hydra_Software_Devel/147   8/15/07 2:06p pntruong
 * PR33940: Request to open ColorBurstAmp value to user.
 *
 * Hydra_Software_Devel/146   8/13/07 5:43p syang
 * PR 33614: add a vnet-reconfig for 3D comb off to on if anr is enabeld
 *
 * Hydra_Software_Devel/145   8/10/07 4:31p pntruong
 * PR33933: PC format detection can not work after switch from NTSC
 * several times.
 *
 * Hydra_Software_Devel/144   8/8/07 4:28p tdo
 * PR32896: More adjustment to copy protection detection process
 *
 * Hydra_Software_Devel/143   8/2/07 5:27p tdo
 * PR32896:  [VDEC] Need Macrovision signal detection software
 *
 * Hydra_Software_Devel/142   8/1/07 1:54p pntruong
 * PR32296: [VDEC]: PC input: There is different H positon performance
 * with the same Hstart.
 *
 * Hydra_Software_Devel/141   8/1/07 1:19p pntruong
 * PR33011, PR33011, PR31623: [VDEC] auto detection of PAL-M/PAL/PAL-
 * Nc/NTSC for VCR input.
 *
 * Hydra_Software_Devel/140   7/24/07 5:13p pntruong
 * PR31869, PR31907: [VDEC] snow noise blink between normal snow noise and
 * black screen.
 *
 * Hydra_Software_Devel/139   7/23/07 5:27p pntruong
 * PR32146: [VDEC]: [PC] Program polarity according to input format real
 * polarity.
 *
 * Hydra_Software_Devel/138   7/23/07 10:16a pntruong
 * PR31869: [VDEC] snow noise blink between normal snow noise and black
 * screen.
 *
 * Hydra_Software_Devel/137   7/20/07 3:24p pntruong
 * PR33011, PR33012, PR33173: Merged from pr branch and refactored extra
 * codes.
 *
 * Hydra_Software_Devel/PR33173/1   7/20/07 11:35a mlei
 * PR33173 :need adjust ACGC reference level for PAL input according to
 * programme guider
 *
 * Hydra_Software_Devel/136   7/18/07 10:25a pntruong
 * PR32677: Fixed the fcw factor precision.
 *
 * Hydra_Software_Devel/135   7/13/07 12:14p pntruong
 * PR32080: [VDEC]: PC: Check all setting in code, and removed duplicate
 * elements pll object.
 *
 * Hydra_Software_Devel/134   7/11/07 8:24p pntruong
 * PR32939: [VDEC]: The ATSC and PC FD tables should be separated.
 *
 * Hydra_Software_Devel/133   7/11/07 12:10p pntruong
 * PR32296, PR32913, PR32919: Added support for 1080p@60Hz_Cea,
 * 1366x768@60Hz.
 *
 * Hydra_Software_Devel/PR32296/2   7/11/07 9:35p gorgon
 * PR32919:[PC]: Support format 1366x768@60.
 *
 * Hydra_Software_Devel/PR32296/1   7/11/07 10:56a gorgon
 * PR32296:[VDEC]: PC input: There is different H positon performance with
 * the same Hstart.
 *
 * Hydra_Software_Devel/132   7/9/07 4:44p pntruong
 * PR32824: Video Shifting for analog RF signal when swtich channel caused
 * by the toggling between noisy setting and normal setting.
 *
 * Hydra_Software_Devel/131   7/9/07 3:48p pntruong
 * PR32775: Hstart user setting is overwritten in VDEC.
 *
 * Hydra_Software_Devel/130   7/9/07 11:37a pntruong
 * PR32589: Need to improve the FD stability during transition
 *
 * Hydra_Software_Devel/PR32737/1   7/6/07 8:13p dyzhang
 * PR32589: Need to improve the FD stability during transition
 *
 * Hydra_Software_Devel/129   7/2/07 3:06p pntruong
 * PR32677: [VDEC]:PC: implement manual clock with new algorithm.
 *
 * Hydra_Software_Devel/PR32677/1   7/2/07 3:59p gorgon
 * PR32677:[VDEC]:PC: implement manual clock with new algorithm.
 *
 * Hydra_Software_Devel/128   6/27/07 10:04p pntruong
 * PR32503: [VDEC] Add oversampling support.
 *
 * Hydra_Software_Devel/127   6/27/07 1:47p pntruong
 * PR32503: Cleanup unused code.  Prepare for 480i/480p vdec oversample.
 *
 * Hydra_Software_Devel/126   6/20/07 10:20p pntruong
 * PR32080: PR32080: modify PC input configuration according to script.
 *
 * Hydra_Software_Devel/PR32080/1   6/19/07 8:45p honglang
 * PR32080: modify PC input configuration according to script
 *
 * Hydra_Software_Devel/125   6/18/07 6:43p pntruong
 * PR31679: [VDEC]: video show horizontal bar that climb from bottom to
 * top for VDEC input.
 *
 * Hydra_Software_Devel/124   6/12/07 4:32p pntruong
 * PR32076, PR32078, PR32081: Updated pc-input pll settings.
 *
 * Hydra_Software_Devel/123   6/5/07 8:38p pntruong
 * PR29670: C0: VDEC: PAL-B vs. PAL-Nc auto-format detection.
 *
 * Hydra_Software_Devel/PR29670/1   5/29/07 11:33a mlei
 * PR29670:C0: VDEC: PAL-B vs. PAL-Nc auto-format detection
 *
 * Hydra_Software_Devel/122   5/25/07 12:36p pntruong
 * PR29659: C0: Support 1080p @ 60 through component input, and runnning
 * adc at 216Mhz.
 *
 * Hydra_Software_Devel/121   5/24/07 4:20p pntruong
 * PR30691: [VDEC]some VCR tape(landscaping shows) can not detect some VCR
 * headswitch.
 *
 * Hydra_Software_Devel/120   5/23/07 9:40p pntruong
 * PR29782: Hw not yet fixed in 3563c0.  Need to enable sw-workaround.
 *
 * Hydra_Software_Devel/119   5/23/07 9:22p pntruong
 * PR31558: Add support for new PC input format 1600x1200p@60Hz.
 *
 * Hydra_Software_Devel/118   5/22/07 7:27p pntruong
 * PR31403: Fixed 7403 build errors.
 *
 * Hydra_Software_Devel/117   5/21/07 3:55p pntruong
 * PR31403: Correctly set clamp and adc1 connector on/off.
 *
 * Hydra_Software_Devel/116   5/21/07 2:33p pntruong
 * PR31403: Preliminary bringup Vdec for c0.
 *
 * Hydra_Software_Devel/115   5/16/07 1:43p pntruong
 * PR30357: [VDEC]snow noise continue to flash and sometimes show color.
 *
 * Hydra_Software_Devel/114   5/14/07 2:02p pntruong
 * PR30363: Provide API to enable/disable ACGC.
 *
 * Hydra_Software_Devel/113   5/14/07 1:13p pntruong
 * PR30427: Intialized black offset for vdec.
 *
 * Hydra_Software_Devel/112   5/14/07 11:41a pntruong
 * PR30427: Provide API to control black offset in VDEC.
 *
 * Hydra_Software_Devel/111   5/10/07 8:55a pntruong
 * PR30071: when source change,  delay 3 vsyncs to update SNR and use fast
 * average for SNR,  after 40 Vsyncs, recover to use normal average.
 *
 * Hydra_Software_Devel/PR29983/5   5/9/07 9:42p mlei
 * PR30071: when source change,  delay 3 vsyncs to update SNR and use fast
 * average for SNR,  after 40 Vsyncs, recover to use normal average.
 *
 * Hydra_Software_Devel/110   5/9/07 10:38a pntruong
 * PR30071: [VDEC]Picture has horizontal shifting then goes back one
 * second later after switch input source to CVBS or NTSC.
 *
 * Hydra_Software_Devel/109   5/7/07 2:04p pntruong
 * PR26033: VDEC: Auto-detect PAL-M, PAL-Nc, PAL-BDGHIN, NTSC, for CVBS
 * and Svideo input.
 *
 * Hydra_Software_Devel/108   5/4/07 3:49p pntruong
 * PR30373, PR29999: Improve noise and vcr input handling.
 *
 * Hydra_Software_Devel/PR29983/2   5/3/07 10:25p mlei
 * PR29999: improve kill color for weak signal
 *
 * Hydra_Software_Devel/107   5/3/07 7:38p pntruong
 * PR29983: Improved noise signal handling.
 *
 * Hydra_Software_Devel/106   5/3/07 7:02p pntruong
 * PR29662: Added c0 dynamics rts settings.
 *
 * Hydra_Software_Devel/105   5/3/07 2:48p tdo
 * PR29670: C0: VDEC: PAL-B vs. PAL-Nc auto-format detection
 *
 * Hydra_Software_Devel/104   4/30/07 4:30p tdo
 * PR26033: Adjust CSC MIN and MAX to fix the color for PAL
 *
 * Hydra_Software_Devel/103   4/27/07 5:51p pntruong
 * PR29999: [VDEC] kill color for weak signal. change plll lock to quick.
 *
 * Hydra_Software_Devel/102   4/26/07 8:44p pntruong
 * PR30072: [VDEC] Fv test from Shibasoku, when shifting is +4.6~+5.3Hz
 * the video will disappear, Fv minus shifting is OK from 0~-7Hz.
 *
 * Hydra_Software_Devel/101   4/26/07 5:48p tdo
 * PR26033: Making auto format detection for PALs more stable
 *
 * Hydra_Software_Devel/100   4/26/07 3:44p pntruong
 * PR30071: [VDEC]Picture has horizontal shifting then goes back one
 * second later after switch input source to CVBS or NTSC.
 *
 * Hydra_Software_Devel/99   4/25/07 5:34p pntruong
 * PR28773: Don't use mf for Fh it's not reliable with ifd source.
 *
 * Hydra_Software_Devel/98   4/19/07 12:24a pntruong
 * PR29962: Synchronized rdb header files.
 *
 * Hydra_Software_Devel/97   4/18/07 7:03p pntruong
 * PR29782: VDEC: Offset for negative CSC coefficients lower than it
 * should be.
 *
 * Hydra_Software_Devel/96   4/17/07 4:08p tdo
 * PR26033: Add support for PAL_M and PAL_NC
 *
 * Hydra_Software_Devel/PR_26033/7   4/17/07 3:39p tdo
 * PR26033: More merging
 *
 * Hydra_Software_Devel/95   4/13/07 6:25p pntruong
 * PR29573: PC input. Manual clock adjustment will lead vertical ladder.
 *
 * Hydra_Software_Devel/94   4/12/07 1:14p pntruong
 * PR29612: Added delay start to prevent garbage from feeding out on input
 * change or channel change.
 *
 * Hydra_Software_Devel/93   4/11/07 7:34p pntruong
 * PR29483: PC input. Add two supported formats:720x400@70Hz and
 * 640x350@70Hz.  Also not include the dummy adc/edsafe regs in rul.
 *
 * Hydra_Software_Devel/92   4/10/07 4:09p pntruong
 * PR28773: Also need to turn of 3d comb when VDEC_FE_0_FE_SAMPLE_COUNT is
 * driff off from standard value.
 *
 * Hydra_Software_Devel/91   4/9/07 9:28p pntruong
 * PR29612: Need reset VDEC value when swich to analog channel, switch to
 * analog input source or cable plugging in/out.
 *
 * Hydra_Software_Devel/90   4/9/07 6:37p pntruong
 * PR28773: Don't touch hpll accumulator as it cause hpll trying to relock
 * and cause the jagged line on fh test.  Restore the way we do fh
 * averaging.
 *
 * Hydra_Software_Devel/89   4/3/07 8:40p pntruong
 * PR28773: Use tolerance of 2 for tracking Fh.
 *
 * Hydra_Software_Devel/88   4/3/07 6:23p pntruong
 * PR28773: Use RDC read for VDEC_FE_0_FE_FIELD_SIZE to get consistence
 * values.  Use median averaging to remove toggling.  Update statics
 * settings to better match 3560bx.
 *
 * Hydra_Software_Devel/87   3/31/07 1:41a pntruong
 * PR28395: Init the custom structures, some more work needed.
 *
 * Hydra_Software_Devel/86   3/30/07 4:54p pntruong
 * PR28773: Average sample count as it is not stable on Fh test.  For
 * match filter don't need to compare the line length for the case of
 * cvbs testing.
 *
 * Hydra_Software_Devel/85   3/30/07 10:42a yuxiaz
 * PR28350, PR27180: Added BVDC_Source_GetInputStatus to get input status
 * for VDEC and HD_DVI.
 *
 * Hydra_Software_Devel/84   3/29/07 3:25p pntruong
 * PR28824: Optimize manual ACGC adjust algorithm.  Use macro for
 * precision bits.  Use csc to adjust cr/cb gain for acgc.
 *
 * Hydra_Software_Devel/83   3/29/07 1:44p pntruong
 * PR28773: Use the fe's size when hpll is good otherwise use the mf's
 * size.
 *
 * Hydra_Software_Devel/82   3/25/07 4:06p pntruong
 * PR28380: PR28380: Can not lock full white feild consistently on cvbs
 * and RF analog
 *
 * Hydra_Software_Devel/PR28380/1   3/25/07 9:43p dyzhang
 * PR28380: Can not lock full white feild consistently on cvbs and RF
 * analog
 *
 * Hydra_Software_Devel/81   3/25/07 2:47p pntruong
 * PR28904: Do pedestal removal (for VDEC source) in CMP.
 *
 * Hydra_Software_Devel/80   3/23/07 6:46p pntruong
 * PR28773: Optimize settings for ShibaSoku FH testing.
 *
 * Hydra_Software_Devel/79   3/21/07 5:43p pntruong
 * PR28884: Used VDEC_FE_0_DCR_CONTROL_CH0.REFERENCE_LEVEL when HPLL
 * unlocked and macrovision-free signal for vbi lines.  This is to
 * removed the black line with noise signal.
 *
 * Hydra_Software_Devel/78   3/17/07 11:56p pntruong
 * PR28773, PR23226: Allow vcr to stay locked long before unlocked.  Reset
 * vdec state-machine on hpll re-locked.  Ignore spurious unknown format
 * reports by FD on noisy/vcr signal and not set format dirty bit.
 *
 * Hydra_Software_Devel/77   3/17/07 8:32p hongtaoz
 * PR27640, PR28829: added software workaround for pc input auto phase
 * hysteresis control;
 *
 * Hydra_Software_Devel/76   3/17/07 5:25p pntruong
 * PR28773, PR23226: Restored SIGNAL_TO_MONITOR to AUTO.  Fixed VCR
 * settings to work with standard tape.  Need additional improvements for
 * bad vcr tape.
 *
 * Hydra_Software_Devel/75   3/16/07 4:04p pntruong
 * PR28824: Optimized manual ACGC adjust algorithm for noisy signal.
 *
 * Hydra_Software_Devel/74   3/16/07 12:42p pntruong
 * PR28773:  Enabled Fh code and used FD's match filter line-length status
 * as it's more tolerance to low sync level.
 *
 * Hydra_Software_Devel/73   3/15/07 11:33p pntruong
 * PR28773: Not to turn on Fh yet as it still need to be tested with low
 * sync level.  Need to find reliable horizontal frequency status.
 *
 * Hydra_Software_Devel/72   3/15/07 11:22p pntruong
 * PR28773: Optimize settings for ShibaSoku FH testing.  Can do +/- 450Hz
 * for Fh.
 *
 * Hydra_Software_Devel/71   3/14/07 4:12p hongtaoz
 * PR27640: move the PC sync status registers read from isr to user thread
 * to reduce isr load since there is no callback requirement;
 *
 * Hydra_Software_Devel/70   3/14/07 2:53p hongtaoz
 * PR27640: added PC input auto/manual adjustment support;
 *
 * Hydra_Software_Devel/69   3/14/07 12:37p pntruong
 * PR28726, PR23673, PR23430: Merged in changes from PR23430 branch.
 *
 * Hydra_Software_Devel/68   3/13/07 4:49p hongtaoz
 * PR27640: added PC input auto/manual sync adjustment first cut;
 *
 * Hydra_Software_Devel/67   3/12/07 7:19p pntruong
 * PR28359: VDEC: Video quality improvements for
 * IF/CVBS/YPrPb/480i/480p/720p/1080i.  Further improved manual acgc.
 *
 * Hydra_Software_Devel/66   3/8/07 11:02p pntruong
 * PR23496: VDEC: fix ShibaSohu issues(FH, FV, FSC, low sync level, low
 * burst level, burst on/off).
 *
 * Hydra_Software_Devel/65   3/8/07 6:29p pntruong
 * PR23497: Enabled manual acgc, and color kill.
 *
 * Hydra_Software_Devel/64   3/3/07 5:54p pntruong
 * PR23497: Manual color burst is for for cvbs, svideo, and ifd.
 *
 * Hydra_Software_Devel/63   3/3/07 12:49p pntruong
 * PR23424: Correct digital gain reference settings.  Refactored code, so
 * that vdec fmt table is higher precedence.  This will ease the adding
 * of new format/input modes.
 *
 * Hydra_Software_Devel/62   3/2/07 12:06p pntruong
 * PR23497: Adjust the gain precision in 3563.  Not yet turn manual acgc
 * on.
 *
 * Hydra_Software_Devel/61   2/15/07 4:08p pntruong
 * PR25235: Need to use actual pixel frequency and vertical frequency
 * fields in BFMT instead of the MASK.
 *
 * Hydra_Software_Devel/60   2/5/07 4:18p yuxiaz
 * PR 23498: Use VDEC_CD_0_CD_STATUS2.FIELD_AVERAGED_COLOR_BURST_AMP for
 * average color burst amplitude.  VDEC_CD_0_CD_STATUS.COLOR_BURST_AMP is
 * still broken on 3563.
 *
 * Hydra_Software_Devel/59   2/2/07 7:37p pntruong
 * PR23482: Added VDEC/HDDVI Frame Rate Detection in VDC.  Also refactored
 * detection to use in both hddvi and vdec.  Added support for all
 * framerate code suported in bavc.h.
 *
 ***************************************************************************/
#ifndef BVDC_VDEC_PRIV_H__
#define BVDC_VDEC_PRIV_H__

#include "bvdc.h"
#include "bvdc_common_priv.h"
#include "bvdc_bufferheap_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

BDBG_OBJECT_ID_DECLARE(BVDC_DEC);

/***************************************************************************
 * Defines
 ***************************************************************************/
/* Different revision of VDEC! */
#define BVDC_P_VDEC_VER_0                    (0) /* 3560Bx */
#define BVDC_P_VDEC_VER_1                    (1) /* 3563-Ax,Bx */
#define BVDC_P_VDEC_VER_2                    (2) /* 3563-Cx */
#define BVDC_P_VDEC_VER_3                    (3) /* 3563-Dx */
#define BVDC_P_VDEC_VER_4                    (4) /* 3548-Ax */
#define BVDC_P_VDEC_VER_5                    (5) /* 3548-B0/B1 */
#define BVDC_P_VDEC_VER_6                    (6) /* 3548-B2, 3556Cx */

/* This flag enables RF input to quickly detect the PAL/SECAM, at the same time.
 * bypass the BVN reconfiguration.
 * TODO: Additional tuning and testing needed. */
#ifndef BVDC_P_VDEC_FAST_SECAM_PAL_DETECTION
#define BVDC_P_VDEC_FAST_SECAM_PAL_DETECTION       (1)
#endif

/* To do pedestal removal in VDEC/CMP */
#define BVDC_P_REMOVE_PEDESTAL_IN_CMP              (0)

/* maximum vsync counter number */
#define BVDEC_P_VSYNC_COUNTER_MAX                  UINT32_MAX

/* PC Input based macros */
#define BVDC_P_PC_CLOCK_ADJ_FACTOR                 (100000)
#define BVDC_P_PC_CLOCK_HALF_RANGE_SIZE            (50) /* BVDC_P_PC_CLOCK_ADJ_FACTOR/200 */
#define BVDC_P_PC_CLOCK_FCW_FACTOR                 (24) /* 24 bits float */
#define BVDC_P_PC_POSITION_LOCKED_TIME             (80) /* confirmed by customer.*/
#define BVDC_P_PC_800X600_75_AUTO_PHASE_FREEZE     (0)  /* for 800x600@75, freeze the phase to 616. */
#define BVDC_P_PC_RA_SEARCH_NOISE_NUMERATOR        (1 << 24)
#define BVDC_P_PC_RA_HSTART_OFFSET                 (2)  /* Descrepency of RA & FE offset */

/* Array size and alignments, and misc table macros. */
#define BVDC_P_FDT_ENTRY_SIZE                      (2) /* 2 dwords */
#define BVDC_P_FDT_ALIGNMENT                       (8) /* 2^8 256 byte-align */
#define BVDC_P_VDEC_H_COUNT_FACTOR                 (2) /* H_COUNT factor */
#define BVDC_P_VDEC_H_COUNT_FACTOR_2X              (1) /* H_COUNT factor */
#define BVDC_P_VDEC_SD_TRIGGER_OFFSET              (1) /* number of line after av */
#define BVDC_P_VDEC_HD_TRIGGER_OFFSET              (5) /* number of line after av */
#define BVDC_P_ADC_INVALID                         UINT32_C(-1)
#define BVDC_P_ADDR_LAST                           UINT32_C(-1)

/* VCR Head switch threshold */
#define BVDC_P_VDEC_HEAD_SWITCH_MIN                (8)
#define BVDC_P_VDEC_HEAD_SWITCH_GOOD               (24)
#define BVDC_P_VDEC_HEAD_SWITCH_MAX                (10000)

/* Noise level threshold */
#define BVDC_P_VDEC_NOISE_LEVEL_33DB               (132)
#define BVDC_P_VDEC_NOISE_LEVEL_28DB               (112)
#define BVDC_P_VDEC_NOISE_LEVEL_23DB               (92)
#define BVDC_P_VDEC_NOISE_LEVEL_18DB               (72)
#define BVDC_P_VDEC_NOISE_LEVEL_13DB               (56)
#define BVDC_P_VDEC_NOISE_LEVEL_10DB               (40)
#define BVDC_P_VDEC_NOISE_LEVEL_TOGGLING_THRESHOLD (10)

/* Noisy on/off threshold */
#define BVDC_P_VDEC_NOISE_LEVEL_NOISY_TURN_ON      (120)
#define BVDC_P_VDEC_NOISE_LEVEL_NOISY_TURN_OFF     (140)
#define BVDC_P_VDEC_NOISE_LEVEL_DELAY_TOGGLE       (29)
/*Auto detection for RF signal*/
#define BVDC_P_VDEC_NOISE_AUTO_DETECTION_TURN_ON   (70)
#define BVDC_P_VDEC_NOISE_AUTO_DETECTION_TURN_OFF  (50)

/*PR33797: Disable PAL Separation*/
#define BVDC_P_VDEC_NOISE_PAL_SEPARATION_ON        (0x30)
#define BVDC_P_VDEC_NOISE_PAL_SEPARATION_OFF       (0x2A)

/* Auto Color Mode ON/OFF threshold */
/* RF settings */
#define BVDC_P_VDEC_RF_ABOVE_33DB_AUTO_COLOR_MODE_OFF             (94)
#define BVDC_P_VDEC_RF_ABOVE_33DB_AUTO_COLOR_MODE_ON              (84)
#define BVDC_P_VDEC_RF_28DB_33DB_AUTO_COLOR_MODE_OFF              (67)
#define BVDC_P_VDEC_RF_28DB_33DB_AUTO_COLOR_MODE_ON               (53)
#define BVDC_P_VDEC_RF_23DB_28DB_AUTO_COLOR_MODE_OFF              (44)
#define BVDC_P_VDEC_RF_23DB_28DB_AUTO_COLOR_MODE_ON               (36)
#define BVDC_P_VDEC_RF_18DB_23DB_AUTO_COLOR_MODE_OFF              (32)
#define BVDC_P_VDEC_RF_18DB_23DB_AUTO_COLOR_MODE_ON               (27)
#define BVDC_P_VDEC_RF_13DB_18DB_AUTO_COLOR_MODE_OFF              (26)
#define BVDC_P_VDEC_RF_13DB_18DB_AUTO_COLOR_MODE_ON               (22)
#define BVDC_P_VDEC_RF_10DB_13DB_AUTO_COLOR_MODE_OFF              (18)
#define BVDC_P_VDEC_RF_10DB_13DB_AUTO_COLOR_MODE_ON               (15)
#define BVDC_P_VDEC_RF_BELOW_10DB_AUTO_COLOR_MODE_OFF             (15)
#define BVDC_P_VDEC_RF_BELOW_10DB_AUTO_COLOR_MODE_ON              (13)

/* VCR */
#define BVDC_P_VDEC_VCR_ABOVE_33DB_AUTO_COLOR_MODE_OFF            (94)
#define BVDC_P_VDEC_VCR_ABOVE_33DB_AUTO_COLOR_MODE_ON             (84)
#define BVDC_P_VDEC_VCR_28DB_33DB_AUTO_COLOR_MODE_OFF             (76)
#define BVDC_P_VDEC_VCR_28DB_33DB_AUTO_COLOR_MODE_ON              (64)
#define BVDC_P_VDEC_VCR_23DB_28DB_AUTO_COLOR_MODE_OFF             (62)
#define BVDC_P_VDEC_VCR_23DB_28DB_AUTO_COLOR_MODE_ON              (53)
#define BVDC_P_VDEC_VCR_18DB_23DB_AUTO_COLOR_MODE_OFF             (50)
#define BVDC_P_VDEC_VCR_18DB_23DB_AUTO_COLOR_MODE_ON              (42)
#define BVDC_P_VDEC_VCR_13DB_18DB_AUTO_COLOR_MODE_OFF             (38)
#define BVDC_P_VDEC_VCR_13DB_18DB_AUTO_COLOR_MODE_ON              (31)
#define BVDC_P_VDEC_VCR_10DB_13DB_AUTO_COLOR_MODE_OFF             (30)
#define BVDC_P_VDEC_VCR_10DB_13DB_AUTO_COLOR_MODE_ON              (23)
#define BVDC_P_VDEC_VCR_BELOW_10DB_AUTO_COLOR_MODE_OFF            (30)
#define BVDC_P_VDEC_VCR_BELOW_10DB_AUTO_COLOR_MODE_ON             (23)

/* Normal */
#define BVDC_P_VDEC_ABOVE_33DB_AUTO_COLOR_MODE_OFF (254)
#define BVDC_P_VDEC_ABOVE_33DB_AUTO_COLOR_MODE_ON  (232)
#define BVDC_P_VDEC_28DB_33DB_AUTO_COLOR_MODE_OFF  (140)
#define BVDC_P_VDEC_28DB_33DB_AUTO_COLOR_MODE_ON   (126)
#define BVDC_P_VDEC_23DB_28DB_AUTO_COLOR_MODE_OFF  (88)
#define BVDC_P_VDEC_23DB_28DB_AUTO_COLOR_MODE_ON   (79)
#define BVDC_P_VDEC_18DB_23DB_AUTO_COLOR_MODE_OFF  (54)
#define BVDC_P_VDEC_18DB_23DB_AUTO_COLOR_MODE_ON   (48)
#define BVDC_P_VDEC_13DB_18DB_AUTO_COLOR_MODE_OFF  (34)
#define BVDC_P_VDEC_13DB_18DB_AUTO_COLOR_MODE_ON   (30)
#define BVDC_P_VDEC_10DB_13DB_AUTO_COLOR_MODE_OFF                 (28)
#define BVDC_P_VDEC_10DB_13DB_AUTO_COLOR_MODE_ON                  (24)
#define BVDC_P_VDEC_BELOW_10DB_AUTO_COLOR_MODE_OFF                (28)
#define BVDC_P_VDEC_BELOW_10DB_AUTO_COLOR_MODE_ON                 (24)

/* HPLL unlock history */
#define BVDC_P_VDEC_HPLL_UNLOCK_MAX                               (100)
#define BVDC_P_VDEC_HPLL_TOGGLING_COLOR_DELAY                     (30)
#define BVDC_P_VDEC_HPLL_TOGGLING_VCR_DELAY                       (20)
#define BVDC_P_VDEC_HPLL_TOGGLING_NORMAL_DELAY                    (3)

/* Video not detected history */
#define BVDC_P_VDEC_VIDEO_NOT_DETECT_THRESHOLD                    (60)
#define BVDC_P_VDEC_VIDEO_NOT_DETECT_THRESHOLD_VCR                (60)
#define BVDC_P_VDEC_VIDEO_NOT_DETECT_THRESHOLD_NORMAL             (3)

/* Manual Color Mode ON/OFF threshold*/
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)/* 3548Ax */
/*For base band*/
#define BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_ON_AMP              (14)
#define BVDC_P_VDEC_NOISE_LEVEL_28DB_33DB_COLOR_ON_AMP            (34)
#define BVDC_P_VDEC_NOISE_LEVEL_23DB_28DB_COLOR_ON_AMP            (48)
#define BVDC_P_VDEC_NOISE_LEVEL_18DB_23DB_COLOR_ON_AMP            (62)
#define BVDC_P_VDEC_NOISE_LEVEL_13DB_18DB_COLOR_ON_AMP            (90)
#define BVDC_P_VDEC_NOISE_LEVEL_DOWN_13DB_COLOR_ON_AMP            (110)

#define BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_OFF_AMP             (10)
#define BVDC_P_VDEC_NOISE_LEVEL_28DB_33DB_COLOR_OFF_AMP           (26)
#define BVDC_P_VDEC_NOISE_LEVEL_23DB_28DB_COLOR_OFF_AMP           (36)
#define BVDC_P_VDEC_NOISE_LEVEL_18DB_23DB_COLOR_OFF_AMP           (50)
#define BVDC_P_VDEC_NOISE_LEVEL_13DB_18DB_COLOR_OFF_AMP           (78)
#define BVDC_P_VDEC_NOISE_LEVEL_DOWN_13DB_COLOR_OFF_AMP           (98)

/*For PAL-G RF signal*/
#define BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_ON_AMP_RF           (9)
#define BVDC_P_VDEC_NOISE_LEVEL_28DB_33DB_COLOR_ON_AMP_RF         (38)
#define BVDC_P_VDEC_NOISE_LEVEL_23DB_28DB_COLOR_ON_AMP_RF         (45)
#define BVDC_P_VDEC_NOISE_LEVEL_18DB_23DB_COLOR_ON_AMP_RF         (55)
#define BVDC_P_VDEC_NOISE_LEVEL_13DB_18DB_COLOR_ON_AMP_RF         (70)
#define BVDC_P_VDEC_NOISE_LEVEL_DOWN_13DB_COLOR_ON_AMP_RF         (90)

#define BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_OFF_AMP_RF          (6)
#define BVDC_P_VDEC_NOISE_LEVEL_28DB_33DB_COLOR_OFF_AMP_RF        (30)
#define BVDC_P_VDEC_NOISE_LEVEL_23DB_28DB_COLOR_OFF_AMP_RF        (35)
#define BVDC_P_VDEC_NOISE_LEVEL_18DB_23DB_COLOR_OFF_AMP_RF        (40)
#define BVDC_P_VDEC_NOISE_LEVEL_13DB_18DB_COLOR_OFF_AMP_RF        (50)
#define BVDC_P_VDEC_NOISE_LEVEL_DOWN_13DB_COLOR_OFF_AMP_RF        (78)

/*For PAL-M RF signal*/
#define BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_ON_AMP_RF_PALM      (11)
#define BVDC_P_VDEC_NOISE_LEVEL_23DB_28DB_COLOR_ON_AMP_RF_PALM    (50)
#define BVDC_P_VDEC_NOISE_LEVEL_18DB_23DB_COLOR_ON_AMP_RF_PALM    (50)
#define BVDC_P_VDEC_NOISE_LEVEL_13DB_18DB_COLOR_ON_AMP_RF_PALM    (55)
#define BVDC_P_VDEC_NOISE_LEVEL_DOWN_13DB_COLOR_ON_AMP_RF_PALM    (70)

#define BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_OFF_AMP_RF_PALM     (8)
#define BVDC_P_VDEC_NOISE_LEVEL_23DB_28DB_COLOR_OFF_AMP_RF_PALM   (45)
#define BVDC_P_VDEC_NOISE_LEVEL_18DB_23DB_COLOR_OFF_AMP_RF_PALM   (45)
#define BVDC_P_VDEC_NOISE_LEVEL_13DB_18DB_COLOR_OFF_AMP_RF_PALM   (48)
#define BVDC_P_VDEC_NOISE_LEVEL_DOWN_13DB_COLOR_OFF_AMP_RF_PALM   (60)

/*For PAL-Nc RF signal*/
#define BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_ON_AMP_RF_PALNC     (11)
#define BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_OFF_AMP_RF_PALNC    (8)

/*For NTSC RF signal*/
#define BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_ON_AMP_RF_NTSC      (11)
#define BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_OFF_AMP_RF_NTSC     (8)

#else /*3548x*/
#define BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_ON_AMP              (7)
#define BVDC_P_VDEC_NOISE_LEVEL_28DB_33DB_COLOR_ON_AMP            (17)
#define BVDC_P_VDEC_NOISE_LEVEL_23DB_28DB_COLOR_ON_AMP            (24)
#define BVDC_P_VDEC_NOISE_LEVEL_18DB_23DB_COLOR_ON_AMP            (31)
#define BVDC_P_VDEC_NOISE_LEVEL_13DB_18DB_COLOR_ON_AMP            (38)
#define BVDC_P_VDEC_NOISE_LEVEL_DOWN_13DB_COLOR_ON_AMP            (45)

#define BVDC_P_VDEC_NOISE_LEVEL_UP_33DB_COLOR_OFF_AMP             (5)
#define BVDC_P_VDEC_NOISE_LEVEL_28DB_33DB_COLOR_OFF_AMP           (13)
#define BVDC_P_VDEC_NOISE_LEVEL_23DB_28DB_COLOR_OFF_AMP           (18)
#define BVDC_P_VDEC_NOISE_LEVEL_18DB_23DB_COLOR_OFF_AMP           (25)
#define BVDC_P_VDEC_NOISE_LEVEL_13DB_18DB_COLOR_OFF_AMP           (32)
#define BVDC_P_VDEC_NOISE_LEVEL_DOWN_13DB_COLOR_OFF_AMP           (39)
#endif
/* Limit ACGC gain maximum value and do the gain in CSC to avoid ripple noise.
   Currently, only for NTSC format*/
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)/* 3548Ax */
#define BVDC_P_ACGC_CSC_GAIN                                      (0) /* on/off */
#else
#define BVDC_P_ACGC_CSC_GAIN                                      (1) /* on/off */
#endif

#if !(BVDC_P_ACGC_CSC_GAIN)
#define BVDC_P_ACGC_GAIN_MAX                                      (368)
#else
#define BVDC_P_ACGC_GAIN_MAX                                      (272)
#endif

/* Manual ACGC aka Manual Color Burst Gain */
#define BVDC_P_ACGC_CR_DEF_GAIN                                   (0xfdf)
#define BVDC_P_ACGC_CB_DEF_GAIN                                   (0xb3f)
#define BVDC_P_ACGC_CSC_GAIN_MAX                                  (4)
#define BVDC_P_ACGC_REF_TOLERANCE                                 (3)

#define BVDC_P_ACGC_DBG_MANUAL_ACGC                               (1) /* on/off */
#define BVDC_P_ACGC_GAIN_MIN                                      (0)
#define BVDC_P_ACGC_GAIN_NOMINAL                                  (128)
#define BVDC_P_ACGC_GAIN_DELTA                                    (80)

#define BVDC_P_ACGC_SML_BURST_INC                                 (1)
#define BVDC_P_ACGC_FIN_BURST_INC                                 (2)
#define BVDC_P_ACGC_MSL_BURST_INC                                 (8)
#define BVDC_P_ACGC_MED_BURST_INC                                 (32)
#define BVDC_P_ACGC_BIG_BURST_INC                                 (64)
#define BVDC_P_ACGC_LRG_BURST_INC                                 (96)
/*add one treshold, when very noise condition, burst level is unstable,
treshold 1 is too sensitive */
#define BVDC_P_ACGC_SML_CMP_THRESHOLD                             (1)
#define BVDC_P_ACGC_MSL_FIN_THRESHOLD                             (6)
#define BVDC_P_ACGC_MSL_CMP_THRESHOLD                             (12)
#define BVDC_P_ACGC_MED_CMP_THRESHOLD                             (36)
#define BVDC_P_ACGC_BIG_CMP_THRESHOLD                             (70)
#define BVDC_P_ACGC_LRG_CMP_THRESHOLD                             (100)
/*If signal is very noise, chroma should be killed as well.
 *both noise level and field average error are used to identify the
 * noise condition*/
#define BVDC_P_VDEC_NOISE_LEVEL_SET_MANUAL_COLOR_MODE_BW          (30)
#define BVDC_P_VDEC_NOISE_LEVEL_SET_MANUAL_COLOR_MODE_COLOR       (35)
#define BVDC_P_VDEC_FIELD_AVG_ERR_SET_MANUAL_COLOR_MODE_BW        (60)
#define BVDC_P_VDEC_FIELD_AVG_ERR_SET_MANUAL_COLOR_MODE_COLOR     (20)

/* Fv,Fh changes theshold. */
#define BVDC_P_VDEC_FH_MANUAL                                     (1) /* on/off Fh */
#define BVDC_P_VDEC_FV_LINE_THRESHOLD                             (4)
#define BVDC_P_VDEC_FH_STDR_THRESHOLD                             (3)
#define BVDC_P_VDEC_FH_SYNC_THRESHOLD                             (2)
#define BVDC_P_VDEC_FH_DELAY_TO_SAMPLE                            (BVDC_P_VDEC_HEAD_SWITCH_GOOD + 14)
#define BVDC_P_VDEC_FH_DISABLE_VCR         (12)

/* VCR FIELD AVG ERROR */
#define BVDC_P_FIELD_AVG_ERR_THRESHOLD                            (3)
#define BVDC_P_VDEC_NOISE_LEVEL_ENABLE_LINE_ADJUST                (32)

/* Get noise level average */
#define BVDC_P_VDEC_NOISE_LEVEL_FAST_COUNT                        (3)
#define BVDC_P_VDEC_NOISE_LEVEL_NORM_COUNT                        (30)
#define BVDC_P_VDEC_NOISE_LEVEL_MAX                               (255)
#define BVDC_P_VDEC_NOISE_DELAY_TO_NORMAL                         (40)

/*when HPLL status toggle, delay some vsyncs to update*/
#define BVDC_P_VDEC_NOISE_DELAY_TO_SAMPLE                         (5)

/* sample count average. */
#define BVDC_P_VDEC_SAMPLE_COUNT                                  (8)

/* Get line per field value average */
#define BVDC_P_VDEC_LINE_PER_FIELD_COUNT                          (8)

/* Get sample per line value average */
#define BVDC_P_VDEC_SAMPLER_PER_LINE_COUNT                        (8)

/* Get color burst amplitude average */
#define BVDC_P_VDEC_COLORBURST_AMP_FAST_COUNT                     (2)
#define BVDC_P_VDEC_COLORBURST_AMP_NORM_COUNT                     (2)
#define BVDC_P_VDEC_COLORBURST_AMP_MDSL_COUNT                     (5)
#define BVDC_P_VDEC_COLORBURST_AMP_SLOW_COUNT                     (10)
#define BVDC_P_VDEC_COLORBURST_AMP_DELTA_THRESH                   (0xFF)

#define BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH_UP_33DB              (7)
#define BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH_33_28DB              (30)
#define BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH_28_23DB              (40)
#define BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH_23_18DB              (88)
#define BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH_18_13DB              (99)
#define BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH_DOWN_13DB            (120)

#define BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH_23_18DB_RF_PALM      (65)
#define BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH_18_13DB_RF_PALM      (65)
#define BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH_DOWN_13DB_RF_PALM    (85)

/* Sub carrier loop accumulator average */
#define BVDC_P_VDEC_SUBCARRIER_LOOP_ACCU_COUNT                    (5)

/* Auto Acgc Gain average*/
#define BVDC_P_VDEC_AUTO_ACGC_GAIN_COUNT                          (10)

/* Auto detection for NTSC and PAL */
#define BVDC_P_VDEC_SUBCARRIER_LOOP_ACC_UNLOCK_NORMAL             (0xB00)
#define BVDC_P_VDEC_SUBCARRIER_LOOP_ACC_UNLOCK_VCR                (0x4000)
#define BVDC_P_VDEC_SUBCARRIER_LOOP_ACC_UNLOCK_FAST               (1000)
#define BVDC_P_VDEC_AUTO_DETECTION_USE_FAST_THRESH_TIMER          (40)

/* Clear subcarrier in some interval*/
#define BVDC_P_VDEC_CLEAR_SUBCARRIER_TIMER                        (20)

/* Secam CD unlock*/
#define BVDC_P_VDEC_SECAM_CSYNC_UNLOCK_THRESH                     (20)
#define BVDC_P_VDEC_SECAM_LOCK_THRESHOLD                          (10)

/*PR33011, PR33012, PR50097*/
#define BVDC_P_VDEC_CLEAR_SUBCARRIER_THRESH                       (5)
#define BVDC_P_VDEC_CHROMA_UNLOCK_THRESH                          (5)
#define BVDC_P_VDEC_AUTO_MAX_ACGC_GAIN                            (400)
#define BVDC_P_VDEC_CHROMA_BLACKWHITE_THRESH                      (20)
#define BVDC_P_VDEC_LINES_PER_FIELD_THRESH                        (30)

/* Delay several vsyncs to enable CCR detection after clear subcarrer */
#define BVDC_P_VDEC_ENABLE_CCR_DETECTION                          (10)

/* Delay start auto detection according to Subcarrier*/
#define BVDC_P_VDEC_FORMAT_CHANGE_DELAY_START_SUB                 (10)

/* Delay identify PAL Vs PAL-Nc*/
#define BVDC_P_VDEC_FORMAT_CHANGE_DELAY_START_AMP                 (5)

/* VDEC channels YPrPb/Rgb/etc. */
#define BVDC_P_VDEC_CLEAR_SUBCARRIER_ACC_THRESHHOLD               (32767)

/* Switch clamp between FD and VDEC*/
#define BVDC_P_VDEC_SWITCH_CLAMP_LEVEL                            (10)
/* counter of new FD formatID*/
#define BVDC_P_VDEC_FD_FMTID_CNT                                  (3)
#define BVDC_P_VDEC_LINES_PER_FIELD_THRESHOLD                     (30)

/*PR53597*/
#define BVDC_P_VDEC_FD_LINE_LENGTH_THRESHOLD                      (120)

#define BVDC_P_VDEC_MET_THESHOLD(pRegSample) \
	(((pRegSample)->ulTheshold) == (pRegSample)->ulIndex)

#define BVDC_P_VDEC_RESET_SAMPLING(pRegSample) \
	((pRegSample)->ulIndex = 0)

/* Make sure the gain will be bigger than maximum when increasing it. */
#define BVDC_P_ACGC_INCREASE_MANUAL_GAIN(manual_gain, increase, max_gain) \
{ \
	if (manual_gain + increase < max_gain) \
	{ \
		manual_gain += increase; \
	} \
	else \
	{ \
		manual_gain = max_gain; \
	} \
}

/* Make sure the gain will be smaller than minimum when decreasing it. */
#define BVDC_P_ACGC_DECREASE_MANUAL_GAIN(manual_gain, decrease, min_gain) \
{ \
	if ((manual_gain > decrease) && \
		((manual_gain - decrease) > min_gain))\
	{ \
		manual_gain -= decrease; \
	} \
	else \
	{ \
		manual_gain = min_gain; \
	} \
}

#if (BVDC_P_SUPPORT_VDEC_VER < BVDC_P_VDEC_VER_5)
#define BVDC_P_3DCOMB_TTD_CONFLICT           (1)
#endif

/* This flag enables TTD to work in SECAM mode.  By disable the chroma filter
 * in EOP-1, and enable at SOP+1 picture.  This is needed to passthru all
 * ttx line. */
#if (BVDC_P_SUPPORT_VDEC_VER == BVDC_P_VDEC_VER_6)
#define BVDC_P_VDEC_SECAM_TTD_BYPASS_FILTER_IN_VBI (1)
#else
#define BVDC_P_VDEC_SECAM_TTD_BYPASS_FILTER_IN_VBI (0)
#endif

/* Various work-around for this specific version! */
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_2)  /* 3563 C0 and beyond */

/* PR32896: copy protection agc detection work-around. */
#if ((BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_2) && \
     (BVDC_P_SUPPORT_VDEC_VER <= BVDC_P_VDEC_VER_3))
#define BVDC_P_VDEC_PR32896_WORK_AROUND      (1)
#else
#define BVDC_P_VDEC_PR32896_WORK_AROUND      (0)
#endif

/* PR27640: Adjust hysteresis acorrding to pc phase lock status. */
/* PR34153: Set phase offset to lighten pc streaky noise. */
/* PR36256, PR34690, PR32296: Speed H sync lock. There is not any fix in D0 */
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_3)/* 3563Dx */
#define BVDC_P_VDEC_PR27640_HYSTERESIS       (0)
#define BVDC_P_VDEC_PR34153_PHASE_OFFSET     (0)
#define BVDC_P_VDEC_PR36256_SYNC_LOCK        (1)
#define BVDC_P_VDEC_PR38861_EVERY_VSYNC      (1)
#else
#define BVDC_P_VDEC_PR27640_HYSTERESIS       (1)
#define BVDC_P_VDEC_PR34153_PHASE_OFFSET     (1)
#define BVDC_P_VDEC_PR36256_SYNC_LOCK        (1)
#define BVDC_P_VDEC_PR38861_EVERY_VSYNC      (0)
#endif

#define BVDC_P_VDEC_PR28267_WORK_AROUND      (0)

#elif (BVDC_P_SUPPORT_VDEC)
#error "Port Required!"
#endif

/* Copy protection detection */
#if (BVDC_P_VDEC_PR32896_WORK_AROUND)
#define BVDC_P_VDEC_MV_STATUS_COUNT          (16)
#define BVDC_P_VDEC_MV_STATUS_MIN_GOOD       (6)
#define BVDC_P_VDEC_MV_STATUS_MAX_BAD        (3)
#else
#define BVDC_P_VDEC_MV_STATUS_COUNT          (1)
#endif

/* Auto detection for PAL-60/NTSC443, for this two format, they has same
 * vertical line and subcarrier frequency.  Sofeware has no way to distinguish
 * them. Customer can only support one of format for auto detection */
#define BVDC_P_VDEC_HAS_NTSC443_NO_PAL60     (0)

/* Ease bringup, this will remove software work-arounds. */
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)/* 3548 & beyond */
#define BVDC_P_USE_WORK_AROUNDS              (1)
#define BVDC_P_USE_PLL_AND_SYS_CLK           (1)
#define BVDC_P_USE_COMMON_MODE               (1)
#define BVDC_P_SECAM_AUTO_DETECION_SUPPORT   (1)
#if (BVDC_P_SUPPORT_VDEC_VER == BVDC_P_VDEC_VER_4)/* 3548Ax */
#define BVDC_P_RESET_YC_FOR_3DCOMB           (1)
#else
#define BVDC_P_RESET_YC_FOR_3DCOMB           (0)
#endif
#define BVDC_P_VDEC_FH_CLK_THRESHOLD         (100) /* 3548 3D comb tolerates 256 extra samples */
#define BVDC_P_VDEC_PR29782_WORK_AROUND      (0)
#define BVDC_P_VDEC_WORK_AROUND_DISABLE_COMB (0)
#else
#define BVDC_P_USE_WORK_AROUNDS              (1)
#define BVDC_P_USE_PLL_AND_SYS_CLK           (1)
#define BVDC_P_USE_COMMON_MODE               (1)
#define BVDC_P_SECAM_AUTO_DETECION_SUPPORT   (0)
#define BVDC_P_RESET_YC_FOR_3DCOMB           (0)
#define BVDC_P_VDEC_FH_CLK_THRESHOLD         (100)
#define BVDC_P_VDEC_PR29782_WORK_AROUND      (1)
#define BVDC_P_VDEC_WORK_AROUND_DISABLE_COMB (1)
#endif

/* PR44664: For ver 4 only */
#if (BVDC_P_SUPPORT_VDEC_VER == BVDC_P_VDEC_VER_4)/* 3548Ax */
#define BVDC_P_COMPENSATE_SCART_CSC_IN_CMP   (1)
#else
#define BVDC_P_COMPENSATE_SCART_CSC_IN_CMP   (0)
#endif

#if (BVDC_P_VDEC_PR29782_WORK_AROUND)
#define BVDC_P_VDEC_COEFF_IS_NEGATIVE(coef)  (0x8000 & (coef))
#define BVDC_P_VDEC_COEFF_COMPENSATE         (0xfc) /* 15.75 */
#endif

#if (BVDC_P_VDEC_PR36256_SYNC_LOCK)
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)/* 3548Ax */
#define BVDC_P_VDEC_COEFAB_TOGGLE             (511)
#define BVDC_P_VDEC_TOGGLE_TIME               (11)
#else
#define BVDC_P_VDEC_COEFAB_TOGGLE             (7)
#define BVDC_P_VDEC_TOGGLE_TIME               (45)
#endif
#endif

#define BVDC_P_VDEC_DBG_ADC_CONFIG            (0)

/***************************************************************************
 * Vdec Context
 ***************************************************************************/
typedef enum
{
	BVDC_P_VdecId_eVdec0 = 0,
	BVDC_P_VdecId_eVdec1

} BVDC_P_VdecId;

typedef enum
{
	BVDC_P_Pll_e13_500= 0,
	BVDC_P_Pll_e25_175,
	BVDC_P_Pll_e27_000,
	BVDC_P_Pll_e28_320 ,
	BVDC_P_Pll_e28_324,
	BVDC_P_Pll_e30_240,
	BVDC_P_Pll_e31_500,
	BVDC_P_Pll_e35_500,
	BVDC_P_Pll_e36_000,
	BVDC_P_Pll_e38_250,
	BVDC_P_Pll_e40_000,
	BVDC_P_Pll_e44_900,
	BVDC_P_Pll_e49_500,
	BVDC_P_Pll_e50_000,
	BVDC_P_Pll_e56_250,
	BVDC_P_Pll_e65_000,
	BVDC_P_Pll_e68_250,
	BVDC_P_Pll_e71_640,
	BVDC_P_Pll_e72_000 ,
	BVDC_P_Pll_e72_014,
	BVDC_P_Pll_e73_250 ,
	BVDC_P_Pll_e74_250,
	BVDC_P_Pll_e74_481 ,
	BVDC_P_Pll_e75_000,
	BVDC_P_Pll_e78_750,
	BVDC_P_Pll_e79_500,
	BVDC_P_Pll_e83_462 ,
	BVDC_P_Pll_e83_500 ,
	BVDC_P_Pll_e84_750 ,
	BVDC_P_Pll_e85_500,
	BVDC_P_Pll_e94_500,
	BVDC_P_Pll_e101_000,
	BVDC_P_Pll_e102_250,
	BVDC_P_Pll_e106_500,
	BVDC_P_Pll_e108_000,
	BVDC_P_Pll_e117_500,
	BVDC_P_Pll_e119_000,
	BVDC_P_Pll_e121_750,
	BVDC_P_Pll_e125_000,
	BVDC_P_Pll_e135_000,
	BVDC_P_Pll_e136_750,
	BVDC_P_Pll_e138_625,
	BVDC_P_Pll_e146_250,
	BVDC_P_Pll_e148_500,
	BVDC_P_Pll_e156_000,
	BVDC_P_Pll_e157_500,
	BVDC_P_Pll_e162_000,
	BVDC_P_Pll_e216_000,

	/* Must be last */
	BVDC_P_Pll_eUnknown
}BVDC_P_Pll;

typedef enum
{
	BVDC_P_eInteger = 0,
	BVDC_P_eMash,
	BVDC_P_eMfb,
	BVDC_P_eMash_1_8,
	BVDC_P_eMfb_1_8,
	/* Must be last */
	BVDC_P_eUnknown
}BVDC_P_eNDivMode;

/*algorithm for calculating of register value*/
typedef enum
{
	BVDC_P_SampleAlg_eMin = 0,
	BVDC_P_SampleAlg_eMax,
	BVDC_P_SampleAlg_eMean,
	BVDC_P_SampleAlg_eMedian

} BVDC_P_SampleAlg;

/* Input type. */
typedef enum
{
	BVDC_P_VdecInput_eIfd = 0,
	BVDC_P_VdecInput_eCvbs,
	BVDC_P_VdecInput_eSvideo,
	BVDC_P_VdecInput_eYPrPb,
	BVDC_P_VdecInput_ePcIn,
	BVDC_P_VdecInput_eScart,    /* Cvbs + Rgb */
	BVDC_P_VdecInput_eUnknown

} BVDC_P_VdecInput;

/* Noise level. */
typedef enum
{
	BVDC_P_Vdec_NoiseLevel_eNormal = 0,
	BVDC_P_Vdec_NoiseLevel_eVcr,
	BVDC_P_Vdec_NoiseLevel_eNoisy,
	BVDC_P_Vdec_NoiseLevel_eUnknown

} BVDC_P_Vdec_NoiseLevel;

/* PLL settings */
typedef struct
{
	BVDC_P_eNDivMode                eNmode;
	uint32_t                        ulIcpx0;
	uint32_t                        ulIcpx1;
	uint32_t                        ulRz;
	uint32_t                        ulCz;
	uint32_t                        ulCp;
	uint32_t                        ulRn;
	uint32_t                        ulCn;
	uint32_t                        ulLfOrder;
	const char                     *pchNmode;

} BVDC_P_NModeEntry;

/* PLL_Freq = ((54Mhz * N * P1) / (M * P2)) * (1 + (N_Fract)/(2^24)).
 * PLut some other Pll settings for specific frequency. */
typedef struct
{
	BVDC_P_Pll                      ePll;
	uint32_t                        ulM;
	uint32_t                        ulN;
	uint32_t                        ulP1;
	uint32_t                        ulP2;
	uint32_t                        ulFcwOffset;    /* FCW_OFFSET. */
	uint32_t                        ulByenPhi;
	uint32_t                        ulVcoRange; /* PLL_DIV_CTL.VCO_RANGE. */
	uint32_t                        ulIcpx;
	uint32_t                        ulI;
	uint32_t                        ulDivMuxSel; /* PLL_PLL_CTL_1.I_DIVMUX_SEL */

	/* VDEC's PLL_CTL_0 for PC input formats */
	const BVDC_P_NModeEntry        *pNDivMode;

	/* Name to help print out */
	const char                     *pchFreq;

} BVDC_P_PllEntry;

/* Entry of a format is two dword, and 256-byte alignment. */
typedef struct
{
	bool                            bAtscFormat;

	/* An ATSC or PC supported format! */
	BFMT_VideoFmt                   eFmt;

	/* HW Format identification parameters */
	uint32_t                        aulFdParms[BVDC_P_FDT_ENTRY_SIZE];

	/* So software can access it easily. */
	uint32_t                        ulHsPol;       /* FT_HSYNC_POLARITY; */
	uint32_t                        ulVsPol;       /* FT_VSYNC_POLARITY; */
	uint32_t                        ulDeltaLl;     /* FT_DELTA_LL; */
	uint32_t                        ulHsWidth;     /* FT_HSYNC_WIDTH; */
	uint32_t                        ulDeltaHw;     /* FT_DELTA_HW; */
	uint32_t                        ulRasterT;     /* FT_RASTER_TYPE; */
	uint32_t                        ulLpf;         /* FT_LINES_PER_FIELD; */
	uint32_t                        ulDeltaLpf;    /* FT_DELTA_LPF; */
	uint32_t                        ulLineLength;  /* FT_LINE_LENGTH; */

	/* VDEC's PLL freq for PC input formats */
	const BVDC_P_PllEntry          *pPll;

	/* Nominal value is back porch + hsync */
	uint32_t                        ulHStart;       /* AV/VBI start */
} BVDC_P_FdtEntry;

/* Entry for selecting ADC and connector or input. */
typedef struct
{
	uint32_t                        ulAdc;        /* Adc from bvdc.h */
	uint32_t                        ulId;         /* 0-N */
	uint32_t                        ulOffset;     /* Offset into ADC_1 */
	uint32_t                        ulChnSel;     /* VDTopCtrl chn select */
	uint32_t                        ulInputSel;   /* Input selector */
	uint32_t                        ulCalibSel;   /* PC Input calibration channel. */
	uint32_t                        ulInOffMask;  /* ADC Common Mode Register */
	uint32_t                        ulInOffShift; /* ADC Common Mode Register */

} BVDC_P_AdcEntry;

/* Entry for selecting PC Input configuration. */
typedef struct
{
	/* An ATSC or PC supported format! */
	BFMT_VideoFmt                   eFmt;
	uint32_t                        ulDcrCtrl;      /* DCR_CONTROL_CHx / CAL_HSYNC_OFFSET */
	uint32_t                        ulBlankAvg;     /* BLANK_AVG. */
	uint32_t                        ulDiffThreshold;
	bool                            bAdcAdj;        /* do Adc adjustment for streaky noise. */
	bool                            bDcrAdj;        /* do Dcr adjustment for streaky noise. */
	bool                            bDitAdj;        /* do Dither adjustment for streaky noise. */
	bool                            bPllAdj;        /* do Pll adjustment for streaky noise*/

} BVDC_P_PcInCfgEntry;

/*register value structure for workaroud*/
typedef struct
{
	/*this value can be mean_value, max_value, min_value... depend on eSampleAlg*/
	bool                            bUpdated;
	uint32_t                        ulValue;
	uint32_t                        ulIndex;      /* increamenting idx */
	uint32_t                        ulTheshold;   /* array size. */
	BVDC_P_SampleAlg                eSampleAlg;   /* algorithm of sampling */

} BVDC_P_RegSample;

typedef struct
{
	BFMT_VideoFmt                   eFmt;
	BVDC_P_VdecInput                eInputType;
	const BVDC_P_AddrDataPair      *pFmtTbl;

} BVDC_P_VdecFmtTbl;

typedef struct
{
	uint32_t                        ulIndex;
	const char                     *pchName;
	const BVDC_P_AddrDataPair      *pFmtTbl;

} BVDC_P_VdecFmtTbl_Entry;

typedef struct
{
	BFMT_VideoFmt                   eFmt;
	uint32_t                        ulFreqStep;
	uint32_t                        ulPerCnt;
	uint32_t                        ulAcgcRef;
	uint32_t                        ulAcgcFieldStart;
	uint32_t                        ulAcgcFieldEnd;
	uint32_t                        ulActiveRegionEnd;
} BVDC_P_CdCfgEntry;

typedef struct
{
	BFMT_VideoFmt                   eFmt;
	uint32_t                        ulFilterCoef;
	uint32_t                        ulLineDelay;
	uint32_t                        ulPhaseShift;
	uint32_t                        ulMemBurst;
	uint32_t                        ulMemBurstFrame1;
	uint32_t                        ulMemBurstFrame2;
	uint32_t                        ulSyncLine;
	uint32_t                        ulFrame1Offset;
	uint32_t                        ulFrame2Offset;
	uint32_t                        ulDeltaLastActiveLine;
	uint32_t                        ulDeltaLine;
	uint32_t                        ulDelayMode;
	uint32_t                        ulDecimation;
	BVDC_Vdec_3DGameMode            e3DGameMode;
} BVDC_P_YcCfgEntry;

/* Main vdec context. */
typedef struct BVDC_P_VdecContext
{
	BDBG_OBJECT(BVDC_DEC)

	BREG_Handle                     hReg;
	BVDC_Source_Handle              hSource;

	BVDC_P_VdecId                   eId;

	/* Vsync counter */
	uint32_t                        ulVsyncCnt;

	/*when input source or analog channel change, reset the counter*/
	uint32_t                        ulAdcChangCnt;

	/* To support multiple intances of VDEC!  To begin with 3563, and eventually
	 * port back to 3560x */
	uint32_t                        ulTopOffset;
	uint32_t                        ulRstOffset;
	uint32_t                        ulFeOffset;
	uint32_t                        ulBeOffset;
	uint32_t                        ulCdOffset;
	uint32_t                        ulYcOffset;
	uint32_t                        ulFdOffset;

	/* Reserved scratch var use for VEC/VDEC phase adjust.
	 * need to be read during VBI time. */
	uint32_t                        ulPhaseAddr;

	/* Fe size */
	uint32_t                        ulFeSizeAddr;

	/* VDEC color space conversion!  Trigger changes thru source/user */
	BVDC_P_CscCoeffs                stCsc;
	BVDC_P_CscCoeffs                stFrontendCsc;
	BVDC_P_AdcEntry                 stSifAdc;
	BVDC_P_AdcEntry                 stCalibAdc;

	/* Used for 3D Comb */
	BVDC_P_BufferHeapId             e3DCombHeapId;
	uint32_t                        ul3DCombBufCnt;
	uint32_t                        ul3DCombVsyncDelay;
	BVDC_P_HeapNodePtr              apHeapNode[BVDC_P_MAX_3DCOMB_SD_BUFFER_COUNT];

	/* Vdec Format Table for detection!  */
	uint32_t                       *pulAtscFdtMem;
	uint32_t                        ulAtscFdtMemOffset;
	uint32_t                       *pulVesaFdtMem;
	uint32_t                        ulVesaFdtMemOffset;
	uint32_t                        ulClampSel;
	uint32_t                        ulClampSelFdCnt;
	uint32_t                        ulClkPerVsyncDelta;
	uint32_t                        ulFmtId;
	uint32_t                        bNewFmt;
	uint32_t                        ulFdFmtCnt;
	uint32_t                        ulLastFmtId;
	BVDC_P_FdtEntry                 stFdStatus; /* fd's status */

	/*Auto detection for RF signal*/
	bool                            bDisableRfAutoDetection;

	/*PR33797: Disable PAL Separation*/
	bool                            bPalSeparation;

	/* VDEC hw/sw status */
	/* Vdec status: use by                                cback   vdec  vdc */
	bool                            b240pMode;        /*    0      0     1  */
	uint32_t                        ulVdecPhase;      /*    0      0     1  */
	bool                            bNoInput;         /*    0      1     0  */
	bool                            b3DComb;          /*    0      1     0  */
	bool                            bHPllLocked;      /*    0      1     0  */
	bool                            bVcrHeadSwitch;   /*    0      1     1  */
	bool                            bVideoDetected;   /*    1      1     1  */
	BVDC_MacrovisionType            eMacrovisionType; /*    1      0     0  */
	bool                            bNoisy;           /*    1      0     0  */
	bool                            bManualColorMode; /*    0      0     1  */
	uint32_t                        ulMvCs;
	bool                            bMvAgc;
	bool                            bDrTf;

	/* PR23496: VDEC: fix ShibaSohu issues(FH, FV, FSC, low sync level,
	 * low burst level, burst on/off) */
	bool                            bFh3DComb;
	bool                            bFv3DComb;
	bool                            bClk3DComb;
	uint32_t                        ulProgHFreq;
	uint32_t                        ulStdrHFreq;

	/* PR28904: Do pedestal removal (for VDEC source) in CMP. */
	uint32_t                        ulFeBlankLevel;
	uint32_t                        ulBeBlankLevel;
	uint32_t                        ulBlackOffset;
	uint32_t                        ulBlackOffsetInCmp;

	/* PR23673: artifact when Fsc shifting >+/- 400Hz */
	bool                            bCcrEnable;
	bool                            bVdecTuningDirtyBitCcr;

	/* PR31907:detects wrong field mode when input source is component 720p
	 * from some kind of settop box */
	bool                            bFieldModeErr;

	/* PR28380: Full White Field can't be locked*/
	/* PR31869: snow noise blink between normal snow noise and black screen*/
	bool                            bUseCommonMode;
	uint32_t                        ulAdcOffset;

	uint32_t                        ulNoiseSwitchTimer;

	/* PR28884: black bar at the bottom of screen. */
	uint32_t                        ulBlankSelLines;

	/* Auxilaries and hysterisis or threshold in determing status. */
	uint32_t                        ulVcrTheshold;
	uint32_t                        ulAutoDetectionThreshold;


	/* HPLL unlock counter, used for VCR mode */
	uint32_t                        ulHpllUnLock;

	/* Video not detect used for VCR mode */
	uint32_t                        ulVideoNotDetect;

	/* PR23484: VDEC lost lock when hsync is 20% of normal */
	uint32_t                        ulAutoColorModeOffThreshold;
	uint32_t                        ulAutoColorModeOnThreshold;

	/* Chroma Present Status*/
	bool                            bChromaLock;
	bool                            bChromaPresent;
	bool                            bClearSubCarAccum;
	uint32_t                        ulSubCarrLoopAccAbs;
	uint32_t                        ulManualColorModeColorOnAmp;
	uint32_t                        ulManualColorModeColorOffAmp;
	uint32_t                        ulNoiseHistory;
	uint32_t                        ulFormatChangeTimer;
	uint32_t                        ulSubCarrUnlockTimer;
	uint32_t                        ulBlackWhiteCnt;
	uint32_t                        ulClearSubcarrierCnt;
	uint32_t                        ulChromaUnlockCnt;
	uint32_t                        ulNoBurstThreshold;

	/* PR23497: Acgc; Manual chroma gain */
	bool                            bManualAcgc;
	uint32_t                        ulColorBurstRef;
	uint32_t                        ulColorBurstGain;      /* SW derived this from */
	uint32_t                        ulCrGain;
	uint32_t                        ulCbGain;

	/* PR23496: Hsync frequency shift, Vsync frequency shift*/
	uint32_t                        ulLinePerField;
	uint32_t                        ulSamplePerLine;

	/* vdec output slow start countdown to avoid prematurely output garbage */
	uint32_t                        ulDelayStart;

	/*PR25863: Save related register sample value*/
	uint32_t                        ulFeStatus0Reg;
	uint32_t                        ulFeStatusReg;
	uint32_t                        aulNoiseLevel[BVDC_P_VDEC_NOISE_LEVEL_NORM_COUNT];
	BVDC_P_RegSample                stNoiseLevel;

	uint32_t                        ulFeSampleCntReg;
	uint32_t                        aulSampleCount[BVDC_P_VDEC_SAMPLE_COUNT];
	BVDC_P_RegSample                stSampleCount;

	uint32_t                        ulFeFieldSizeReg;
	uint32_t                        aulLinePerField[BVDC_P_VDEC_LINE_PER_FIELD_COUNT];
	BVDC_P_RegSample                stLinePerField;
	uint32_t                        aulSamplePerLine[BVDC_P_VDEC_SAMPLER_PER_LINE_COUNT];
	BVDC_P_RegSample                stSamplePerLine;

	uint32_t                        ulCdStatusReg;
	uint32_t                        ulCdStatus2Reg;
	uint32_t                        ulCdColorStripeReg;
	uint32_t                        aulColorBurstAmp[BVDC_P_VDEC_COLORBURST_AMP_SLOW_COUNT];
	BVDC_P_RegSample                stColorBurstAmp;
	uint32_t                        aulSubCarLoopAcc[BVDC_P_VDEC_SUBCARRIER_LOOP_ACCU_COUNT];
	BVDC_P_RegSample                stSubCarLoopAcc;
	uint32_t                        aulAutoAcgcGain[BVDC_P_VDEC_AUTO_ACGC_GAIN_COUNT];
	BVDC_P_RegSample                stAutoAcgcGain;
	bool                            bSubCarrLoopAccAbsUpdated;
	uint32_t                        ulColorBurstAmpDelta;
	uint32_t                        ulsubcarrierloopunlock;

	uint32_t                        ulDpllStatusReg;
	bool                            bDpllLocked;
	/* PR27640, PR28829: PC input adjustment status */
	bool                            bPcInPhaseLocked;
	bool                            abPcPhaseLocked[BVDC_COLOR_CHANNELS];
	uint32_t                        ulPcInPhaseStatusReg;

	/* PR32296. pc h_shreshold timer */
	uint32_t                        bPcPositionLocked;
	uint32_t                        ulPcPositionLockdelay;

	uint32_t                        bPcFieldSyncTypeToggle;

	/* PR32677: manual clock parameters*/
	uint32_t                        ulPcManualClockNI;/* pll.DIV_CTL.NDIV_INT               */
	uint32_t                        ulPcManualClockNF;/* PC_IN_0.DPLL_FILTER_GAIN.FCW_OFFSET*/

	/* PR32146: pc h/v sync polarity*/
	uint32_t                        ulPcHsPol;
	uint32_t                        ulPcVsPol;

	/* PR32896: Copy protection detection */
	uint32_t                        ulMvDetectedCount;
	uint32_t                        ulMvId;
	uint32_t                        aulMvDetected[BVDC_P_VDEC_MV_STATUS_COUNT];

	/* Other that does not need averaging */
	uint32_t                        ulCdSubPhaseReg;
	uint32_t                        ulFdFormatReg;
	uint32_t                        ulFdLpfMetric0Reg;
	uint32_t                        ulFdLpfMetric1Reg;
	uint32_t                        ulFdMfMetric0Reg;
	uint32_t                        ulFdMfMetric1Reg;
	uint32_t                        ulFdFuncSelect0Reg;
	uint32_t                        ulFdFuncSelect1Reg;
	uint32_t                        ulFeSyncSliceReg;
	uint32_t                        ulFeTipLevelsReg;
	uint32_t                        ulFeAgcLevelsReg;

	/* SW3548-2751  */
	uint32_t                        aulFeAgcGain[BVDC_COLOR_CHANNELS];
	bool                            bFeAgcLevelsUpdated;

	/* CSC Min and Max */
	uint32_t                        ulCscMin;
	uint32_t                        ulCscMax;

	/* PR32503: over-sample (x2) support for NTSC/480p, PAL/576p. */
	bool                            bOverSample;
	bool                            bOversampleDirty;  /* dirty bit */

	/* PR33940: Request to open ColorBurstAmp value to User. */
	bool                            bColorBurstAmpUpdated;

	/* SW3556-804: SCART function select callback. */
	bool                            bScartFuncSelectUpdated;
	BVDC_ScartFuncSelect            eScartFuncSelect0;
	BVDC_ScartFuncSelect            eScartFuncSelect1;

	/* PR41315: [VDEC] Add support for SECAM format. */
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
	uint32_t                        ulSecamAcgcLoopGain;      /*for SECAM only*/
	uint32_t                        ulSecamAccEnd;            /*for SECAM only*/
	uint32_t                        ulSecamAccStart;          /*for SECAM only*/
	uint32_t                        ulSecamActiveEndLine;     /*for SECAM only*/
	uint32_t                        ulSecamActiveStartLine;   /*for SECAM only*/
	bool                            bSecamColorResetAccum;    /*for SECAM only*/
	uint32_t                        ulSecamColorCsyncSat;     /*for SECAM only*/
	uint32_t                        ulSecamColorCsyncThresh;  /*for SECAM only*/
	uint32_t                        ulSecamColorChromaWidth;  /*for SECAM only*/
	uint32_t                        ulSecamColorLimit;        /*for SECAM only*/
	uint32_t                        ulSecamSyncUnlockTimer;   /*for SECAM only*/
	uint32_t                        ulSecamSyncStatusReg;     /*for SECAM only*/
	bool                            bRaLocked;/* Ra lock status */
	uint32_t                        ulRaLockTimer;/* Ra lock status timer */
#endif
	/* Dither */
	BVDC_P_DitherSetting            stFEDither;
	BVDC_P_DitherSetting            stBEDither;

	/* PR38861: toggle sync mode for pc auto phase lock */
	uint32_t                        ulVsyncMode;

	/* PR40449 */
	bool                            bNoInputSignal;

	/* PR43823: Fh test by ShibaSoku-color flashing*/
	bool                            bYcCrtlColorsyncInvert;
	uint32_t                        ulYcCrtlLineDelay;

	/* SW3556-1026: Horizontal dot noise in AV/SCART.*/
	uint32_t                        ulLineKAccGain;

	/* Base on input/format changes */
	uint32_t                        ulAaFilter;
	uint32_t                        ulSampleRate;
	bool                            bFullSampleRate;

	/*vdec tuning*/
	/*PR40449 bilevel support for component HD format */
	bool                            bVdecTuningDirtyBitBilevel;  /*dirty bit for bi level*/
	bool                            bHdBilevelDetected;
	uint32_t                        ulHdBilevelSwitchCnt;
	uint32_t                        ulFieldAvgErrCnt;
	bool                            bFieldAvgErr;

	/*PR49561: [VDEC]: Vertical black bar on the white noise channel */
	bool                            bVdecTuningDirtyBitResetHStart;  /*dirty bit for reset*/
	bool                            bResetHStart;
	bool                            bVdecTuningDirtyBitEnableSliceLPFHpllCtrl;  /*dirty bit for hsync reset*/
	bool                            bCompEnableSliceLPFHpllCtrl;
	bool                            bVdecTuningDirtyBitResetHsync;  /*dirty bit for hsync reset*/
	bool                            bResetHsyncPllAccum;

	/* Toggle Hthreshold */
	bool                            bToggleHthreshold;/*toggle H threshold*/
	uint32_t                        ulCountToggleHthreshold;

#if (BVDC_P_VDEC_SECAM_TTD_BYPASS_FILTER_IN_VBI)
	BRDC_Slot_Handle                hCapSlot[2];
	BRDC_List_Handle                hCapList[2];
#endif
} BVDC_P_VdecContext;


/***************************************************************************
 * Private function prototypes
 ***************************************************************************/
BERR_Code BVDC_P_Vdec_Create
	( BVDC_P_Vdec_Handle              *phVdec,
	  BVDC_P_VdecId                    eVdecId,
	  BREG_Handle                      hReg,
	  BVDC_Source_Handle               hSource );

void BVDC_P_Vdec_Destroy
	( BVDC_P_Vdec_Handle               hVdec );

void BVDC_P_Vdec_Init
	( BVDC_P_Vdec_Handle               hVdec );

BERR_Code BVDC_P_Vdec_ValidateAdc
	( const BVDC_Source_Handle         ahSource[] );

BERR_Code BVDC_P_Vdec_ValidateChanges
	( BVDC_P_Vdec_Handle               hVdec );

#if (BVDC_P_SUPPORT_VDEC)
void BVDC_P_Vdec_BuildRul_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldId );

void BVDC_P_Vdec_GetStatus_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  uint32_t                        *pulVdecPhase,
	  bool                            *pbVideoDetected,
	  bool                            *pbVcrHeadSwitch,
	  bool                            *pb240pMode,
	  bool                            *pbDrTf );

void BVDC_P_Vdec_Bringup_isr
	( const BVDC_P_Vdec_Handle         hVdec );


void BVDC_P_Vdec_UpdateStatus_isr
	( BVDC_P_Vdec_Handle               hVdec );

void BVDC_P_Vdec_SetPowerMode_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  bool                             bPowerModeOn );

void BVDC_P_Vdec_Reset
	( BREG_Handle                      hReg );

void BVDC_P_Vdec_GetFormatTable_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  const BFMT_VideoInfo            *pFmtInfo,
	  BVDC_P_VdecInput                 eInputType,
	  const BVDC_P_VdecFmtTbl_Entry  **ppFmtEntry);

void BVDC_P_Vdec_GetPcInSyncStatus
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_Source_SyncStatus          *pSyncStatus );

void BVDC_P_Vdec_GetInputStatus
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_Source_InputStatus         *pInputStatus );

void BVDC_P_Vdec_InitMisc
	( BVDC_Vdec_Settings              *pVdecSetting,
	  const BVDC_P_Source_InfoPtr      pUserInfo );

void BVDC_P_Vdec_GetDefaultSettings
	( const BVDC_P_Vdec_Handle         hVdec,
	  BVDC_Vdec_Settings              *pVdecSetting );
#else
#define BVDC_P_Vdec_Bringup_isr(hVdec )                           BDBG_ASSERT(0)
#define BVDC_P_Vdec_BuildRul_isr(hVdec, pList, eFieldId)          BDBG_ASSERT(0)
#define BVDC_P_Vdec_UpdateStatus_isr(hVdec)                       BDBG_ASSERT(0)
#define BVDC_P_Vdec_SetPowerMode_isr(hVdec, bPowerModeOn)         BDBG_ASSERT(0)
#define BVDC_P_Vdec_Reset(hReg)                                   BDBG_ASSERT(0)
#define BVDC_P_Vdec_GetStatus_isr(hVdec, pulVdecPhase, pbVideoDetected, \
	pbVcrHeadSwitch, pb240pMode, pbDrTf)                          BDBG_ASSERT(0)
#define BVDC_P_Vdec_GetFormatTable_isr(hVdec, pFmtInfo, \
	eInputType, ppADPair, ppADPairVcr)                            BDBG_ASSERT(0)
#define BVDC_P_Vdec_GetPcInSyncStatus(hVdec, pSyncStatus)         BDBG_ASSERT(0)
#define BVDC_P_Vdec_GetInputStatus(hVdec, pInputStatus)           BDBG_ASSERT(0)
#define BVDC_P_Vdec_InitMisc(pVdecSetting, pUserInfo)             BDBG_ASSERT(0)
#define BVDC_P_Vdec_GetDefaultSettings(hVdec, pVdecSetting)       BDBG_ASSERT(0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_VDEC_PRIV_H__ */
/* End of file. */
