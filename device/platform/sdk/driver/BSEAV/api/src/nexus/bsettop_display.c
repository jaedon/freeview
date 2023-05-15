/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_display.c $
 * $brcm_Revision: 226 $
 * $brcm_Date: 10/10/12 4:50p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_display.c $
 * 
 * 226   10/10/12 4:50p chengs
 * SW7584-42: Add Brutus support for 7584.
 * 
 * 225   10/1/12 11:19a randyjew
 * SW7425-3388, SW7425-3546: Fix mistake with earlier checkin which cause
 * memory leak
 * 
 * 224   9/28/12 2:52p vle
 * SW7425-3388, SW7425-3546: Fix mistake with earlier checkin which cause
 * memory leak
 * 
 * 223   9/27/12 4:38p vle
 * SW7425-3388, SW7425-3546: Fix coverity issues.
 * 
 * 222   8/30/12 5:44p vle
 * SW7425-2666, SW7405-5150: Add new state to shim-layer
 * 
 * 221   8/23/12 5:02p erickson
 * SW7425-3743: fix warning
 * 
 * 220   8/23/12 4:52p erickson
 * SW7425-3743: remove old code
 * 
 * 219   7/20/12 11:04a erickson
 * SW7405-3222: allow 16x9 and 4x3 for 480p/576p
 * 
 * 218   7/11/12 3:02p vle
 * SW7435-230,SW7346-915,SW7344-348: Fix seg. fault in Brutus
 * 
 * 217   6/29/12 2:23p vle
 * SW7425-3388: Fix coverity issue.
 * 
 * 216   3/26/12 3:18p vle
 * SW7425-2515: Merge to mainline
 * 
 * SW7425-2515/1   3/23/12 6:19p vle
 * SW7425-2515: Update Brutus to use new NEXUS_Cec module. Remove all CEC
 * related code out of hdmi_output module
 * 
 * 215   3/15/12 12:22p erickson
 * SW7425-1747: remove old code
 * 
 * 214   3/15/12 11:42a erickson
 * SW7420-2278: use nexus defaults for enable32Pulldown and
 * enable22Pulldown
 * 
 * 213   2/24/12 12:11p jtna
 * SW7425-2387: coverity defect STACK_USE
 * 
 * 212   2/8/12 3:41p erickson
 * SW7425-2356: teardown hdmi before display in bdisplay_p_uninit() to
 * avoid callback race conditions
 * 
 * 211   2/2/12 5:06p katrep
 * SW7429-71:enabled video image controls for 7429
 * 
 * 210   1/10/12 3:47p dlwin
 * SW7425-2077: Fix to remove setting MPAA for HDMI output.
 * 
 * 209   12/21/11 3:47p mward
 * SW7435-7:  Support Brutus build on (new, 7435-based) platform 97435.
 * 
 * 208   11/7/11 11:56p nickh
 * SW7420-331: Cleanup 2nd compositor output for 7410
 * 
 * 207   9/15/11 1:22p xhuang
 * SW7552-115: correct build error
 * 
 * 206   9/14/11 5:57p jrubio
 * SW7552-115: cover 7574 in bchp define for RFM output code
 * 
 * 205   9/14/11 5:50p jrubio
 * SW7552-115: cover 7574 in bchp define
 * 
 * 204   9/14/11 11:12a xhuang
 * SW7552-115: Add 7574 hardware RFM channel switch support
 * 
 * SW7552-115/1   9/9/11 4:39p xhuang
 * SW7552-115: support 7574 rfm 3/4 hardware switch
 * 
 * 203   8/3/11 12:45p bselva
 * SW7358-66: Enabled LAB and CAB settings for 7358 and 7552 platform.
 * 
 * 202   7/19/11 3:42p jtna
 * SW7344-126: NEXUS_VideoWindow_SetSettings() on a mosaic parent window
 * is no longer supported. Get the basic Brutus 3DTV working again, but
 * some obscure 3D features have been broken on purpose.
 * 
 * 201   6/14/11 6:50p randyjew
 * SW7346-237: Force composite and svideo output on display 1 for 7468
 * 
 * 200   6/6/11 1:41p jessem
 * SW7425-417: Enabled only those HDMI TX audio formats supported by the
 * connected HDMI device.
 *
 * 199   4/29/11 6:36p bselva
 * SW7358-39: Reverting the changes for 7358 and 7552
 *
 * 199   4/29/11 6:35p bselva
 * SW7358-39: Reverting the changes for 7358 and 7552
 *
 * 197   4/27/11 4:01p bselva
 * SW7358-39: Added the support for 7358
 *
 * 196   4/21/11 11:03a jtna
 * SW7425-365: fix build warnings for 2.6.37 kernel
 *
 * 195   4/20/11 10:09a nickh
 * SW7425-381: Adjust ANR slider settings
 *
 * 194   3/3/11 2:23p jrubio
 * SW7344-9: add mad
 *
 * 193   2/28/11 7:32p hongtaoz
 * SW7425-130: use 4-field 0-delay for the newer chips;
 *
 * 192   2/28/11 6:22p hongtaoz
 * SW7425-130: use forced spatial mode instead of disabling deinterlacer
 * for pause/resume;
 *
 * 191   2/11/11 3:12p jrubio
 * SW7346-36: set mad to second window do not force main
 *
 * 190   12/28/10 3:03p hongtaoz
 * SW7425-49: add 7425 brutus support;
 *
 * 189   12/15/10 11:30a jrubio
 * SW7346-5: disable mad for 7344/7346 boards
 *
 * 188   12/15/10 10:14a jrubio
 * SW7344-9: add 7344/7346 support
 *
 * 187   12/6/10 3:21p katrep
 * SW7231-4:add initial support for 7231
 *
 * 186   12/3/10 12:11p VISHK
 * SW7208-114: "Unable to set HD path display settings" if building with
 * MACROVISION ON
 *
 * 185   11/30/10 2:06p VISHK
 * SW7208-114: "Unable to set HD path display settings" if building with
 * MACROVISION ON
 *
 * 184   11/23/10 8:55p nickh
 * SW7422-80: Put composite on display1
 *
 * 183   11/22/10 7:11p VISHK
 * SW7208-114: "Unable to set HD path display settings" if building with
 * MACROVISION ON
 *
 * 182   11/10/10 11:26a jrubio
 * SW7335-819: Do not send CEC messages if  DVI->connected is false
 *
 * 181   10/27/10 5:35p mward
 * SW7125-681: Remove inputs on HDMI and SPDIF in bdisplay_close().
 *
 * 180   10/20/10 4:24p nickh
 * SW7422-80: Add correct MCVP functionality and PQ support for 7422
 *
 * 179   10/18/10 2:10p VISHK
 * SW7401-4457: Settop API shim causes crash when macrovision is enabled
 * for component via Brutus
 *
 * 178   10/14/10 2:13p jgarrett
 * SW7405-4934: Fixing mute of PCM HDMI output when SPDIF is set to
 * compressed
 *
 * 177   8/3/10 5:12p spothana
 * SW7420-732: Use application defined Off DNR/ANR value and VDC ANR/DNR
 * enable flag to determine on and off state.
 *
 * 176   7/20/10 5:27p spothana
 * SW7420-732: The lowest level in DNR and ANR levels should not be zero.
 * Using the max -ve as a level for turning off ANR and DNR blocks.
 *
 * 175   7/20/10 2:57p spothana
 * SW7420-732: 7208 build fix.
 *
 * 174   7/19/10 7:24p spothana
 * SW7420-732: Fixing the flashes made ANR/DNR settings ineffective. In
 * get window settings, get the ANR/DNR levels
 *
 * 173   7/15/10 5:19p spothana
 * SW7420-732: Correcting the ANR setting condition.
 *
 * 172   7/15/10 4:52p spothana
 * SW7420-732: All display outputs flashing when picture quality controls
 * are modified in the Brutus GUI. Don't apply DNR and ANR settings if
 * they have not changed.
 *
 * 171   6/11/10 2:23p gskerl
 * SW7468-254: Fixed segfault when Brutus exits without DVI or HDMI device
 * attached
 *
 * 170   6/8/10 6:52p mward
 * SW7400-2793: When moving MAD from one window to another, must update to
 * apply the changes after removing it from the first, so that the
 * resource will be available when adding it to the second.
 *
 * 169   5/14/10 12:29p jtna
 * SW7125-277: Coverity Defect ID:21062 STACK_USE
 *
 * 168   4/15/10 11:15a jtna
 * SW7125-277: fix brackets
 *
 * 167   4/14/10 11:20a calvinho
 * SW7125-276: Fix Coverity Defect ID:21017 FORWARD_NULL bsettop_display.c
 *
 * 166   4/12/10 11:38a calvinho
 * SW7125-277: Fix Coverity flags 21062 and 20422
 *
 * 165   3/29/10 2:19p erickson
 * SW7405-2724: don't switch MAD off for playback pause. use GameMode
 * instead. this prevents the black flash due to BVN reconfig.
 *
 * 164   3/19/10 5:59p jgarrett
 * SW7405-4040: Ensuring audio and video are on the same timebase
 *
 * 163   3/3/10 3:02p nickh
 * SW7420-565: Refactor dynamic contrast settings
 *
 * 162   2/12/10 3:41p nickh
 * SW7420-309: Add deringing and dejagging support in the shim layer for
 * split screen demo mode
 *
 * 161   2/3/10 10:48a jgarrett
 * SW7405-3858: Handling recursive audio display disable calls
 *
 * 160   1/28/10 11:43a gmohile
 * SW7405-3635 : Fix compile error for non-rfm platforms
 *
 * 159   1/26/10 6:02p jgarrett
 * SW7405-3635: Allowing fixed audio decode->display mapping to avoid re-
 * attaching outputs on every bdecode_stop/bdecode_start
 *
 * 158   1/25/10 3:11p vle
 * SW7405-3486: Add feature to display bluescreen and mute audio when HDCP
 * authentication fail.
 *
 * 157   12/20/09 4:55p randyjew
 * SW7468-6:Add 7468 Support
 *
 * 156   12/16/09 5:34p jtna
 * SW7335-547: update dynamic contrast when set to 0
 *
 * 155   12/11/09 4:30p mphillip
 * SW7550-112: Merge 7550 changes to main branch
 *
 * Refsw_7550/4   12/4/09 6:43p chengli
 * SW7550-108 : Enable 3:2 and 2:2 pull down when MAD is enabled
 *
 * Refsw_7550/3   11/30/09 4:47p chengli
 * SW7550-64: remove HDCP error message
 *
 * Refsw_7550/2   11/25/09 3:27p chengli
 * SW7550-64: remove compile warning
 *
 * Refsw_7550/1   11/19/09 12:02p chengli
 * SW7550-64 : do not set DNR is the chip does not support DNR
 *
 * 154   12/3/09 2:45p jgarrett
 * SW7405-3511: Adding compressed mute for outputs
 *
 * 153   11/19/09 8:02p jgarrett
 * SW7405-3357: Adding audio output routing for decode and pcm playback
 * prior to start
 *
 * 152   11/16/09 12:16p nickh
 * SW7420-331: Dont open 2nd composite output for 7410
 *
 * 152   11/16/09 12:16p nickh
 * SW7420-331: Dont open 2nd composite output for 7410
 *
 * 152   11/16/09 12:16p nickh
 * SW7420-331: Dont open 2nd composite output for 7410
 *
 * 151   11/10/09 5:54p mward
 * SW7400-2574: HD/SD sharpness optimizations incorporated in VDC,
 * NEXUS_PictureCtrl_SetSharpnessValue() is no longer needed for simple
 * sharpness setting.
 *
 * 150   11/10/09 4:58p mward
 * SW7400-2574: When using the  NEXUS_PictureCtrl_SetSharpnessValue(), the
 * quantity of sharpness must be set using lumaControlGain and
 * chromaControlGain, the nPictureCtrlCommonSettings.sharpness is ignored
 * by BVDC_Window_SetSharpness.
 *
 * 149   11/9/09 5:54p mward
 * SW7400-2574:  Sharpness control optimizations.
 *
 * 148   11/4/09 1:59p mward
 * SWDEPRECATED-3881: Restore 3 display operation.
 *
 * 147   10/23/09 5:49p mward
 * SW7125-45: Enable AdvColorSettings on 7125.
 *
 * 146   10/22/09 10:51a jrubio
 * SW7340-54: Add PEP for 7340/7342
 *
 * 145   10/20/09 4:09p erickson
 * SW7405-3222: nexus no longer forces HD displays to be 16x9. it now has
 * a NEXUS_DisplayAspectRatio_eAuto option. settop api does not have
 * this, so we need to force HD to 16x9 for backward compatibility.
 *
 * 144   10/15/09 4:34p erickson
 * SWDEPRECATED-3881: use NEXUS_PlatformConfiguration members to learn #
 * of displays and windows available based on memory strapping options
 *
 * 143   10/9/09 7:56p nickh
 * SW7420-382: Update BNR/MNR with optimized values
 *
 * 142   9/24/09 3:19p jgarrett
 * SW7405-3072: Leaving HDMI audio always enabled regardless of hotplug
 * state
 *
 * 141   9/24/09 2:24p vle
 * SW7405-3078: Fix incorrect hdcp disable when setting hdmi output.
 *
 * 140   9/23/09 5:36p ahulse
 * SW7405-2724: Switch MAD off when in playback pause to allow accurate
 * frame stepping
 *
 * 139   9/1/09 5:39p erickson
 * SW7420-303: set dynamic contrast gain shift based on Settop API units
 *
 * 138   8/18/09 6:44p katrep
 * PR56109: Use calllbacks instead of using events for callbacks to the
 * app
 *
 * 137   8/17/09 3:09p gmohile
 * PR 56400 : Merge legacy vdc support to mainline
 *
 * 136   8/14/09 1:02p katrep
 * PR57184: Coveirity issues
 *
 * 135   8/13/09 7:28p vle
 * PR57708: Fix DVI_VALIDATE_EDID_FORMAT brutus configuration
 *
 * 134   8/4/09 1:28p vle
 * PR49558: Add support to control av mute delay before and after format
 * change separately.
 *
 * 133   7/29/09 12:19p katrep
 * PR57184: Fixed brutus segfault on hdmi hdcp callback during
 * shutdown.This could be the case of race condtion between scheduler
 * event and uninit.
 *
 * 132   7/21/09 6:28p jrubio
 * PR56905: revert back setting mad for Display 0 7335/7325
 *
 * 131   7/21/09 10:06a jrubio
 * PR56905: Mad must be on first display for 7335/7325
 *
 * 130   7/7/09 3:45p katrep
 * PR55769: hdmi_edid_bypass need to work with display
 *
 * 129   6/25/09 7:06p mward
 * PR 47739:  Add composite2 setting to bdisplay_set().
 *
 * 128   6/24/09 3:44p erickson
 * PR56233: call NEXUS_PictureCtrl_SetAdvColorSettings for 7325 and 7335
 *
 * 127   6/23/09 3:23p mward
 * PR 56276: Coverity Defect ID:16503 DEADCODE
 *
 * 126   6/18/09 7:38p mward
 * PR 53475: Failure of NEXUS_HdmiOutput_GetCecConfiguration() is normal
 * if a compatible device is not connected.  Downgrade the associated
 * error messages.
 *
 * 125   6/18/09 4:31p vishk
 * PR 55748: (NEXUS) Clipping problem with bsettop Nexus shim
 *
 * 124   6/5/09 2:10p vishk
 * PR 55748: (NEXUS) Clipping problem with bsettop Nexus shim
 *
 * 123   5/11/09 7:41p vle
 * PR53373: Add HDMI 1.3a support, deep color, to Brutus/nexus
 *
 * 122   5/1/09 12:41p katrep
 * PR54525: incorrect ar setting were getting applied to SD windows.
 *
 * 121   4/17/09 9:22a erickson
 * PR54139: removed false ERR
 *
 * 120   4/14/09 12:02p katrep
 * PR54139: Fixed coveiry issues
 *
 * 119   4/14/09 10:28a jtna
 * PR51960: Fix memleak from hdmi cecEvent
 *
 * PR53373_DeepColor/3   5/11/09 6:28p vle
 * PR53373: boutput_hdmi_get_color_depth is not needed. Use
 * boutput_hdmi_get instead.
 *
 * PR53373_DeepColor/2   5/8/09 5:39p vle
 * PR53373: Add Deep Color support to Brutus. Use GetSettings to get color
 * depth
 *
 * PR53373_DeepColor/1   4/16/09 3:56p vle
 * PR53373: Add HDMI 1.3a support, deep color, to Brutus/nexus
 *
 * 118   4/14/09 9:28a nickh
 * PR52746: Only allow ANR and MAD settings to take effect on main window
 * on main display for 7420
 *
 * 116   4/9/09 5:23p katrep
 * PR52197: Lettter box and aspect ratio settings not getting applied
 * correctly
 *
 * 115   3/16/09 4:35p erickson
 * PR53271: set hdmi colorspace based on component colorspace (backward
 * compatible)
 *
 * 114   3/12/09 11:47a nickh
 * PR52746: Enable dynamic contrast
 *
 * 113   3/10/09 7:04p nickh
 * PR52746: Add Advanced Picture controls for 7420, refine ANR values, and
 * add demo mode
 *
 * 112   3/5/09 12:07p erickson
 * PR48785: fix warnings for DTV systems
 *
 * 111   2/23/09 4:57p vle
 * PR48785: Add CEC support to Nexus.
 *
 * 110   2/23/09 10:34a erickson
 * PR52355: wrap ANR code with #if B_HAS_ANR
 *
 * 109   2/20/09 1:44p erickson
 * PR52355: add ANR support
 *
 * 108   2/9/09 7:26p vle
 * PR 45656, PR 51414: Do not assert when initializing HDCP settings. This
 * prevents incorrect assertion if security is disabled
 *
 * 107   2/9/09 2:03p erickson
 * PR51925: support 2 displays on 3548/3556
 *
 * 106   1/20/09 6:34p mward
 * PR51333: Remove extra debug.
 *
 * 105   1/20/09 6:27p mward
 * PR51333: Apply aspect ratio conversion to full-screen window each
 * bdecode_window_set().  Can't assume display's window 0 is main and 1
 * is PIP, could be swapped.
 *
 * 104   1/13/09 8:56a erickson
 * PR50411: fix CRC for 3548
 *
 * 103   1/8/09 12:21p katrep
 * PR50063: Add suppport dor Ac3 encode in Settop shim and burtus.
 *
 * 102   1/5/09 2:31p vishk
 * PR 50411: CRC do not match on a few Divx streams - Undo previous
 * changes.
 *
 * 101   12/30/08 12:30p vishk
 * PR 50411: CRC do not match on a few Divx streams
 *
 * 100   12/17/08 11:42p erickson
 * PR50501: HdmiOutputSettings now has pre and post format change delays
 *
 * 99   12/15/08 4:49p katrep
 * PR50222: Add independent delay support for audio dacs. dac_output_delay
 *
 * 98   11/26/08 5:51p vle
 * PR49583: Merge to main branch.
 *
 * PR49583/1   11/24/08 9:26p vle
 * PR49583: Fix HDCP certification issue.
 *
 * 97   11/19/08 6:47p mward
 * PR47739: Enable AdvColorSettings for 7400 (dynamic contrast not
 * working).
 *
 * 96   11/13/08 1:47p jgarrett
 * PR 48965: Allowing composite output on 3549b0
 *
 * 95   10/27/08 1:22p katrep
 * PR48292: Fixed coveiry
 *
 * 94   10/22/08 3:12p erickson
 * PR48023: settop pq api remains the same
 *
 * 92   10/20/08 4:07p erickson
 * PR47994: remove debug code
 *
 * 91   10/20/08 3:51p erickson
 * PR47994: fix panel output on 3548 B0 (no component)
 *
 * 90   10/20/08 12:38p erickson
 * PR47994: default outputs connected to display0, avoid redundant
 * RemoveOutput for rf
 *
 * 89   10/16/08 6:26p vsilyaev
 * PR 47994: Fixed hdmi/dvi typo
 *
 * 88   10/16/08 12:15p jgarrett
 * PR 47994: Fixing default dissplay configuration to match old settop api
 *
 * 87   9/23/08 1:12p katrep
 * PR47150: Remove run time warning message
 *
 * 86   9/22/08 7:15p katrep
 * PR47198:add support for spdif_output_delay,hdmi_output_delay run time
 * variables
 *
 * 85   9/19/08 5:05p vishk
 * PR 47150: Coverity Issues.
 *
 * 84   9/17/08 6:20p katrep
 * PR47061: Add support for DTS encode
 *
 * 83   9/15/08 9:52a erickson
 * PR45723: set forceCapture = true, set scaler bias for CRC
 *
 * 82   9/15/08 9:35a erickson
 * PR45009: fix crash if hdmi not present
 *
 * 81   9/10/08 7:57p katrep
 * PR46732: Force pcm for wma compressed. Fixed no audio on hdmi during
 * pcm pb and i2s capture.
 *
 * 80   9/8/08 12:58p katrep
 * PR44168: Removed run time error due to unimplemented cec code
 *
 * 79   9/2/08 7:44p katrep
 * PR46457: Configure settop at run time, based on resources exposed by
 * nexus platfrom.
 *
 * 78   8/26/08 8:30p katrep
 * PR45577: Moved the HDMI audio ouput port settings from dispaly to
 * decode.
 *
 * 77   8/14/08 11:46a jgarrett
 * PR 44168: Adding cec_enable_receive function
 *
 * 76   8/13/08 7:08p katrep
 * PR45577: Add supoort for 5.1 pcm audio on hdmi
 *
 * 75   8/6/08 3:36p katrep
 * PR45009: display should not be connected to hdmi if its already
 * connected. Fixed problem introduced by previous checkin.
 *
 * 74   8/4/08 10:20a vishk
 * PR 45009: Video output handles are not being retained between bdisplay
 * contexts.
 *
 * 73   7/31/08 6:58p katrep
 * PR45082: Fixed broken PIP functionality
 *
 * 72   7/25/08 7:33p vishk
 * PR 45082: bdecode_window_settings structure member 'visible' is not
 * being auto-set to true.
 *
 * 71   7/23/08 9:39a erickson
 * PR45059: protect DBG code against null settings->dvi
 *
 * 70   7/14/08 1:59p ssood
 * PR42739: getting the audio dac handle via display structure
 *
 * 69   7/11/08 9:07a ssood
 * PR42739: PR42739: commenting out debug code
 * PR42739: Live IP High Jitter Support: store handles for I2S output &
 * Audio DACs
 *
 * PR42739/2   7/10/08 10:51p ssood
 * PR42739: commenting out debug code
 *
 * PR42739/1   7/10/08 10:21p ssood
 * PR42739: Live IP High Jitter Support: store handles for I2S output &
 * Audio DACs
 *
 * 68   7/3/08 6:09p erickson
 * PR36068: impl vesa
 *
 * 67   6/27/08 9:30a erickson
 * PR43653: add vesa support
 *
 * 66   6/25/08 5:16p jgarrett
 * PR 36068: Enabling SCART output for 3556
 *
 * 65   6/25/08 12:56p erickson
 * PR36068: DTV panel output for 93556
 *
 * 64   6/25/08 8:37a erickson
 * PR43785: turn off forceCapture for CRC tests
 *
 * 63   6/23/08 2:44p erickson
 * PR36068: fix warning
 *
 * 62   6/20/08 3:15p katrep
 * PR42123: Print errors but do not return if the macrovision/dcs settings
 * fail. Board may not opt options enabled.
 *
 * 61   6/19/08 3:16p vsilyaev
 * PR 40921: There is no component output on the 93556 platform
 *
 * 60   6/18/08 10:13p erickson
 * PR36068: add DTV panel support to shim (use component api)
 *
 * 59   6/13/08 5:04p erickson
 * PR43110: reapply fix for PIP content mode
 *
 * 58   6/13/08 11:00a erickson
 * PR43651: default some VESA values until NEXUS_HdmiOutputStatus is
 * improved
 *
 * 57   6/5/08 5:59p jgarrett
 * PR 43365: Fixing volume scaling
 *
 * 56   6/3/08 9:59a ahulse
 * PR43266: Add support for DCS in nexus
 *
 * 55   5/29/08 11:58a erickson
 * PR43117: apply default settings to nexus if settop api defaults are
 * different
 *
 * 54   5/29/08 11:25a erickson
 * PR43110: only apply content mode to Main, not PIP
 *
 * 53   5/23/08 11:50a erickson
 * PR42998: set NEXUS_VideoWindowSettings.letterBoxAutoCut
 *
 * 52   5/22/08 5:30p erickson
 * PR42982: default deinterlacer on
 *
 * 51   5/21/08 1:48p erickson
 * PR42781: set correct default content_mode
 *
 * 50   5/21/08 1:38p erickson
 * PR42783: fix component output RGB
 *
 * 49   5/16/08 3:38p erickson
 * PR36068: clean up DBG output
 *
 * 48   5/12/08 2:10p erickson
 * PR42266: set NEXUS_VideoWindowSettings.contentMode
 *
 * 47   4/24/08 2:59p jgarrett
 * PR 42122: Setting HDMI volume with SPDIF
 *
 * 46   4/17/08 6:26p katrep
 * PR41565: Adding more HDMI debug messages
 *
 * 45   4/13/08 10:03p katrep
 * PR40824: Apply the VDC settings after all the window settings are
 * updated,added debug
 *
 * 44   4/10/08 6:43p jgarrett
 * PR 41565: Fixing lost audio on HDMI hotplug
 *
 * 43   4/10/08 10:07a erickson
 * PR36068: coverity fix
 *
 * 42   4/9/08 1:19p erickson
 * PR36068: allow format change without decode
 *
 * 41   4/9/08 9:16a erickson
 * PR36068: fix 93563
 *
 * 40   4/8/08 3:56p katrep
 * PR41122: Add support more display menu items
 *
 * 39   4/8/08 11:21a jgarrett
 * PR 40024: Converting decode volume to linear scaling, clipping values
 *
 * 38   4/7/08 1:35p erickson
 * PR40024: impl bdecode_set_audio_volume
 *
 * 37   4/2/08 6:55p jgarrett
 * PR 41233: Adding HDCP Pj Checking
 *
 * 36   4/2/08 5:06p jgarrett
 * PR 41233: Adding all HDMI settings
 *
 * 36   4/2/08 4:54p jgarrett
 * PR 41233: Adding all HDMI settings
 *
 * 35   4/2/08 3:52p jrubio
 * PR41191: add uninit func
 *
 * 34   4/1/08 6:59p jgarrett
 * PR 41191: Revising shutdown logic
 *
 * 33   4/1/08 6:02p katrep
 * PR41191: Add various input/output shutdowns
 *
 * 32   3/31/08 6:04p jgarrett
 * PR 40606: Fixing HDMI behavior
 *
 * 31   3/25/08 7:43p vsilyaev
 * PR 40862: Use nexus configuration to set supported formats
 *
 * 30   3/24/08 6:20p katrep
 * PR40839: Added support for hdmi capabilities
 *
 * 29   3/21/08 8:39p katrep
 * PR40824: Implemented support for PIP swapping.
 *
 * 28   3/21/08 12:12p erickson
 * PR36068: set zorder
 *
 * 27   3/20/08 2:19p erickson
 * PR36068: fix 3563
 *
 * 26   3/20/08 11:51a erickson
 * PR36068: must remove RF output before changing format
 *
 * 25   3/20/08 9:42a jrubio
 * PR40015: reworking some hdmi code
 *
 * 24   3/19/08 3:08p jrubio
 * PR40015: add hdmi video/audio only
 *
 * 23   3/19/08 1:48p katrep
 * PR40311: Added debug messages
 *
 * 22   3/17/08 11:37a erickson
 * PR36068: stub new HDMI CEC functions
 *
 * 21   3/12/08 3:17p jgarrett
 * PR 40017: Adding PCM support
 *
 * 20   3/11/08 11:10a erickson
 * PR36068: impl vbi
 *
 * 19   3/10/08 5:51p erickson
 * PR36068: added boutput_rf
 *
 * 18   1/31/08 1:53p erickson
 * PR36068: default component output
 *
 * 17   1/24/08 3:13p jgarrett
 * PR 38919: Renaming NEXUS_VideoWindow_Open
 *
 * 16   1/22/08 10:10a erickson
 * PR36068: impl audio dac volume
 *
 * 15   1/9/08 2:53p erickson
 * PR38535: change audio units
 *
 * 14   1/3/08 10:36a erickson
 * PR36068: test num of audio dacs
 *
 * 13   1/2/08 9:55a erickson
 * PR36068: Playback changes
 *
 * 12   12/3/07 4:59p erickson
 * PR36068: rework audio, defer playback start until after decode start
 *
 * 11   11/27/07 11:16p erickson
 * PR36068: don't default to DVO for 3563
 *
 * 10   11/27/07 9:20a erickson
 * PR36068: update
 *
 * 9   11/14/07 1:27p erickson
 * PR36068: remove unneeded g_graphic
 *
 * 8   11/12/07 3:57p erickson
 * PR36068: update
 *
 * 7   11/12/07 2:34p erickson
 * PR36068: update
 *
 * 6   10/19/07 12:25p erickson
 * PR36068: added clone support
 *
 * 5   10/17/07 10:10a erickson
 * PR36068: fix bad array index
 *
 * 4   10/17/07 9:45a erickson
 * PR36068: don't terminate on failed RemoveOutput
 *
 * 3   10/16/07 1:17p erickson
 * PR36068: update
 *
 * 2   10/16/07 12:35p erickson
 * PR36068: brutus up over settop api/nexus
 *
 * 1   10/15/07 2:35p erickson
 * PR36068: initial
 *
 ***************************************************************************/
#include <stdlib.h>
#include "bsettop_impl.h"
#include "nexus_core_utils.h"

#if B_N_DVI_OUTPUTS
#include "nexus_hdmi_output_hdcp.h"
#endif

#if NEXUS_HAS_CEC
#include "nexus_cec.h"
#endif


BDBG_MODULE(display);

struct bdisplay g_display[B_N_DISPLAYS];
unsigned g_numDisplays=B_N_DISPLAYS;

#if B_N_SPDIF_OUTPUTS
struct boutput_spdif g_spdif[B_N_SPDIF_OUTPUTS];
#endif
#if B_N_COMPONENT_OUTPUTS
struct boutput_component g_component[B_N_COMPONENT_OUTPUTS];
#endif
#if B_N_COMPOSITE_OUTPUTS
struct boutput_composite g_composite[B_N_COMPOSITE_OUTPUTS];
#endif
#if B_N_SVIDEO_OUTPUTS
struct boutput_svideo g_svideo[B_N_SVIDEO_OUTPUTS];
#endif
#if B_N_DVI_OUTPUTS
struct boutput_hdmi g_hdmi[B_N_DVI_OUTPUTS];
#endif
#if B_N_RF_OUTPUTS
struct boutput_rf g_rf[B_N_RF_OUTPUTS];
static bresult b_set_rfm_audio_source(boutput_rf_t rf, bdisplay_t display);
#endif

#if B_N_AUDIO_DACS
struct boutput_audio_dac g_audioDacs[B_N_AUDIO_DACS];
#endif

#ifdef B_HAS_IP
#if B_N_I2S_OUTPUTS
struct boutput_i2s g_i2sOutput[B_N_I2S_OUTPUTS];
#endif
#endif


BDBG_OBJECT_ID(bdisplay);

/* max video format on HD path. this is
also setable with bsettop_get_config("max_hd_display_format").
see below for values. */
#if B_HAS_1080P_60
NEXUS_VideoFormat g_max_hd_display_format = NEXUS_VideoFormat_e1080p;
#elif B_HAS_1080P
NEXUS_VideoFormat g_max_hd_display_format = NEXUS_VideoFormat_e1080p30hz;
#elif B_DISPLAY_NO_HD_SUPPORT
NEXUS_VideoFormat g_max_hd_display_format = NEXUS_VideoFormat_ePalG;
#else
NEXUS_VideoFormat g_max_hd_display_format = NEXUS_VideoFormat_e1080i;
#endif

static bresult bdisplay_p_set_output_volume(NEXUS_AudioOutput output, const baudio_volume *volume, baudio_volume *out_volume, bool force_mute);
static void bsettop_p_display_apply_zorder(bdisplay_t display);
static void bsettop_p_display_zorder_add(bdisplay_t display, bdecode_window_t window, unsigned zorder);
static void bsettop_p_display_zorder_remove(bdisplay_t display , bdecode_window_t window);
static void bsettop_p_display_zorder_set(bdisplay_t display , bdecode_window_t window, unsigned zorder);
static bresult b_set_audio_output_timebase(NEXUS_AudioOutput output, NEXUS_Timebase outputTimebase);
static void bdisplay_p_set_framerate_master(bdisplay_t display, bdecode_t master);
static void bdisplay_p_set_output_timebase(bdisplay_t display, unsigned timebase);

static bdecode_window_t gWindowMadOwner = NULL;

#define SETTOP_MAX_PQ_LEVELS 6
#define SETTOP_ANR_DNR_OFF_LEVEL -99

#if B_N_DNR
int32_t mnr_level[SETTOP_MAX_PQ_LEVELS] = {SETTOP_ANR_DNR_OFF_LEVEL,-75,-33,0,100,200};
int32_t bnr_level[SETTOP_MAX_PQ_LEVELS] = {SETTOP_ANR_DNR_OFF_LEVEL,-75,-33,0,100,500};
int32_t dcr_level[SETTOP_MAX_PQ_LEVELS] = {SETTOP_ANR_DNR_OFF_LEVEL,-90,-50,0,50,90};
#endif

#if B_HAS_ANR
int32_t anr_level[6] = {SETTOP_ANR_DNR_OFF_LEVEL,-12,-6,0,6,12};
#endif

#if B_HAS_ANR || B_N_DNR
typedef enum bsettop_pq_type
{
 pq_mnr=0,
 pq_bnr,
 pq_dcr,
 pq_anr
}bsettop_pq_type;

int32_t bnexus_to_settop_pq_levels(int32_t value,bsettop_pq_type type)
{
   int32_t i=0;
   int32_t *level;
   switch (type) {
#if B_N_DNR
   case pq_mnr:
        level = mnr_level;
        break;
   case pq_bnr:
        level = bnr_level;
        break;
   case pq_dcr:
        level = dcr_level;
        break;
#endif
#if B_HAS_ANR
   case pq_anr:
        level = anr_level;
        break;
#endif
   default: level = NULL;
   }
   if(level==NULL)
   {
     BDBG_ERR(("bnexus_to_magnum_pq_levels : Incorrect PQ type passed"));
     goto bnexus_to_magnum_pq_levels_err;
   }

   for (i=0;i<SETTOP_MAX_PQ_LEVELS;i++) {
       if(level[i]==value)
       break;
   }
bnexus_to_magnum_pq_levels_err:
   return (i%SETTOP_MAX_PQ_LEVELS);
}
#endif

#if B_N_DVI_OUTPUTS

bresult boutput_hdmi_audio_mute(boutput_hdmi_t hdmi, bool mute)
{
    NEXUS_AudioOutput audioOutput;
    NEXUS_AudioOutputSettings audioSettings;

    audioOutput = NEXUS_HdmiOutput_GetAudioConnector(hdmi->handle);
    NEXUS_AudioOutput_GetSettings(audioOutput, &audioSettings);
    audioSettings.muted = mute;
    NEXUS_AudioOutput_SetSettings(audioOutput, &audioSettings);

    return b_ok;
}

static void bdisplay_p_disconnect_hdmi(bdisplay_t display, boutput_hdmi_t hdmi)
{
    NEXUS_Error rc;
    NEXUS_DisplayHandle nDisplay;
    NEXUS_DisplaySettings displaySettings;

    BDBG_ASSERT(NULL != hdmi);
    BDBG_ASSERT(NULL != display);

    if ( hdmi->display != display || !hdmi->connected )
    {
        BDBG_ERR(("Invalid disconnect HDMI request"));
        return;
    }

    nDisplay = display->nDisplay;

    BDBG_MSG(("Detaching HDMI from display %p", display));

    if ( hdmi->desired.hdcp )
    {
        (void)NEXUS_HdmiOutput_DisableHdcpAuthentication(hdmi->handle);
    }

    NEXUS_Display_GetSettings(nDisplay, &displaySettings);

    rc= NEXUS_Display_RemoveOutput(nDisplay, NEXUS_HdmiOutput_GetVideoConnector(hdmi->handle));
    if (rc) BSETTOP_ERROR(berr_external_error); /* keep going */

#if 0 /* Leave audio alone on hotplug */
    bdisplay_p_enable_audio(display, false);
    rc = NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(hdmi->handle));
    if(rc) {BSETTOP_ERROR(BERR_UNKNOWN);}
    bdisplay_p_enable_audio(display, true);
#endif

    /* We have to do Display SetSettings because the HDMI calls do not refresh the SetSettings */
    rc = NEXUS_Display_SetSettings(nDisplay, &displaySettings);
    if(rc){rc = BERR_TRACE(rc);}

    hdmi->connected = false;
}

static void bdisplay_p_connect_hdmi(bdisplay_t display, boutput_hdmi_t hdmi)
{
    NEXUS_Error rc;
    NEXUS_DisplayHandle nDisplay;
    NEXUS_DisplaySettings displaySettings;


    BDBG_ASSERT(NULL != hdmi);
    BDBG_ASSERT(NULL != display);

    nDisplay = display->nDisplay;

    NEXUS_Display_GetSettings(nDisplay, &displaySettings);

    BDBG_MSG(("Attaching HDMI to display %p", display));

    rc = NEXUS_Display_AddOutput(nDisplay, NEXUS_HdmiOutput_GetVideoConnector(hdmi->handle));
    if (rc)
    {
        BSETTOP_ERROR(rc); return;
    }

#if 0 /* Leave audio alone on hotplug */
    bdisplay_p_enable_audio(display, false);

    /* remove any inputs if connected */
    rc = NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(hdmi->handle));
    if(rc) {BSETTOP_ERROR(BERR_UNKNOWN);}

    if ( hdmi->desired.hdmi_audio_mode == boutput_hdmi_audio_mode_pcm )
    {
        BDBG_MSG(("Attaching HDMI audio to mixer"));
        rc = NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(hdmi->handle),
                                        NEXUS_AudioMixer_GetConnector(display->nAudioMixer));
        if (rc)
        {
            rc = BERR_TRACE(rc);
        }
    }
    else
    {
        BDBG_MSG(("NOT Attaching HDMI audio to mixer,audio must be multichannel or compressed %d",hdmi->desired.hdmi_audio_mode));
    }

    bdisplay_p_enable_audio(display, true);
#endif

    /* We have to do Display SetSettings because the HDMI calls do not refresh the SetSettings */
    rc = NEXUS_Display_SetSettings(nDisplay, &displaySettings);
    if (rc)
    {
        rc = BERR_TRACE(rc);
    }

    hdmi->connected = true;

    if ( hdmi->desired.hdcp )
    {
        /* Start HDCP if connected */
        rc = NEXUS_HdmiOutput_StartHdcpAuthentication(hdmi->handle);
        if (rc)
        {
            rc = BERR_TRACE(rc);
        }
    }

    if ( display->settings.spdif )
    {
        boutput_spdif_set_audio_volume(display->settings.spdif, &display->settings.spdif->volume);
    }
}

static void bdisplay_p_hdmi_hotplug_callback(void *pParam, int iParam)
{
    BSTD_UNUSED(iParam);
    B_Event_Set(pParam);
}

#if BCHP_CHIP == 7552
static void bdisplay_p_rfm_gpio_callback(void *pParam, int iParam)
{
#if B_N_RF_OUTPUTS
    NEXUS_GpioStatus gpioStatus;
    NEXUS_RfmSettings rfmSettings;

    boutput_rf_t rf = pParam;

    NEXUS_Gpio_GetStatus(rf->pin, &gpioStatus);
    BDBG_MSG(("AON_GPIO20: value %d\n",gpioStatus.value));

    BDBG_MSG(("setting rf"));
    NEXUS_Rfm_GetSettings(rf->handle, &rfmSettings);
    if(gpioStatus.value)
        rfmSettings.channel = 4;
    else
        rfmSettings.channel = 3;
    /* settings.country is ignored. Nexus RFM follows the format of the SD VEC. */
    NEXUS_Rfm_SetSettings(rf->handle, &rfmSettings);
#else
    BSTD_UNUSED(pParam);
#endif
    BSTD_UNUSED(iParam);

}
#endif


void bdisplay_p_hdmi_hotplug_handler(void *context)
{
    boutput_hdmi_t hdmi = context;
    NEXUS_HdmiOutputStatus status;
    NEXUS_HdmiOutputHandle nHdmi;
    bdisplay_t display;
#if NEXUS_HAS_CEC
    NEXUS_Error rc; 
    NEXUS_CecSettings cecSettings;
#endif

    BDBG_ASSERT(NULL != hdmi);

    display = hdmi->display;
    nHdmi = hdmi->handle;


    NEXUS_HdmiOutput_GetStatus(nHdmi, &status);

    BDBG_MSG(("HDMI Hotplug Event status %d connected %d", status.connected, hdmi->connected));

    /* Check for connection status change */
    if ( hdmi->connected != status.connected )
    {
        if ( status.connected )
        {
            /* set preferred format from new receiver */
            hdmi->desired.edid_preferred_format = b_nexus2displayformat(status.preferredVideoFormat, &hdmi->desired.vesa_settings);

            BDBG_MSG(("HDMI Hotplug - Notifying application"));
            if ( hdmi->desired.hdmi_hotplug_callback )
            {
#if 0
                b_unlock();
                hdmi->desired.hdmi_hotplug_callback(hdmi->desired.callback_context);
                b_lock();
#endif
                b_callback_fire(hdmi->hotplugCallback);
            }

#if NEXUS_HAS_CEC
            NEXUS_Cec_GetSettings(hdmi->cecHandle, &cecSettings);
            /* Force restart searching for logical address process by 
                disable then re-enable CEC */
            if (cecSettings.enabled)
            {
                hdmi->deviceReady = false;
                cecSettings.enabled = false;        
                rc = NEXUS_Cec_SetSettings(hdmi->cecHandle, &cecSettings);
                BDBG_ASSERT(rc == NEXUS_SUCCESS);       
            }       
            
            /* enable CEC */
            cecSettings.enabled = true;
            cecSettings.physicalAddress[0]= (status.physicalAddressA << 4) 
                    | status.physicalAddressB;
            cecSettings.physicalAddress[1]= (status.physicalAddressC << 4) 
                    | status.physicalAddressD;              
            cecSettings.deviceType = NEXUS_CecDeviceType_eTuner;
            rc = NEXUS_Cec_SetSettings(hdmi->cecHandle, &cecSettings);
            BDBG_ASSERT(rc == NEXUS_SUCCESS);
#endif
        }
        else
        {
            bdisplay_p_disconnect_hdmi(display, hdmi);
            BDBG_MSG(("HDMI Disconnect - Notifying application"));
            if ( hdmi->desired.hdmi_disconnect_callback )
            {
#if 0
                b_unlock();
                hdmi->desired.hdmi_disconnect_callback(hdmi->desired.callback_context);
                b_lock();
#endif
                b_callback_fire(hdmi->disconnectCalllback);
            }
        }
    }
}

static void bdisplay_p_hdmi_hotplug_app_handler(void *context,int param)
{
    boutput_hdmi_t hdmi = context;
    BSTD_UNUSED(param);
    hdmi->desired.hdmi_hotplug_callback(hdmi->desired.callback_context);
}
static void bdisplay_p_hdmi_disconnect_app_handler(void *context,int param)
{
    boutput_hdmi_t hdmi = context;
    BSTD_UNUSED(param);
    hdmi->desired.hdmi_disconnect_callback(hdmi->desired.callback_context);
}

static void bdisplay_p_hdcp_status_callback(void *pParam, int iParam)
{
    BSTD_UNUSED(iParam);
    B_Event_Set(pParam);
}

static void bdisplay_p_hdcp_retry(void *context)
{
    boutput_hdmi_t hdmi = context;
    BDBG_ASSERT(NULL != context);
    hdmi->hdcpTimer = NULL;
    if ( hdmi->desired.hdcp  )
    {
        BDBG_WRN(("Restarting HDCP authentication"));
        NEXUS_HdmiOutput_StartHdcpAuthentication(hdmi->handle);
    }
}

static void bdisplay_p_hdmi_hdcp_handler(void *context)
{
    boutput_hdmi_t hdmi = context;
    NEXUS_HdmiOutputHdcpStatus hdcpStatus;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_HdmiOutputSettings hdmiOutputSettings;

    BDBG_ASSERT(NULL != context);

    BDBG_MSG(("HDCP status change"));

    if ( !hdmi->desired.hdcp )
    {
        return;     /* Bail out if HDCP is disabled now */
    }

    NEXUS_HdmiOutput_GetHdcpStatus(hdmi->handle, &hdcpStatus);

    switch ( hdcpStatus.hdcpState )
    {
    case NEXUS_HdmiOutputHdcpState_eUnauthenticated:
        /* Display bluescreen and mute audio unless user disable this feature */
        if (!hdmi->desired.hdcp_disable_blue_screen)
        {
            NEXUS_Display_GetSettings(hdmi->display->nDisplay, &displaySettings);
            displaySettings.background = 0xff0000ff; /* blue */
            NEXUS_Display_SetSettings(hdmi->display->nDisplay, &displaySettings);

            NEXUS_HdmiOutput_GetSettings(hdmi->handle, &hdmiOutputSettings);
            hdmiOutputSettings.syncOnly = true;
            NEXUS_HdmiOutput_SetSettings(hdmi->handle, &hdmiOutputSettings);

            boutput_hdmi_audio_mute(hdmi, true);
        }
        break;

    case NEXUS_HdmiOutputHdcpState_eR0LinkFailure:
    case NEXUS_HdmiOutputHdcpState_ePjLinkIntegrityFailure:
    case NEXUS_HdmiOutputHdcpState_eRiLinkIntegrityFailure:
    case NEXUS_HdmiOutputHdcpState_eRepeaterAuthenticationFailure:

        /* Display bluescreen and mute audio unless user disable this feature */
        if (!hdmi->desired.hdcp_disable_blue_screen)
        {
            NEXUS_Display_GetSettings(hdmi->display->nDisplay, &displaySettings);
            displaySettings.background = 0xff0000ff; /* blue */
            NEXUS_Display_SetSettings(hdmi->display->nDisplay, &displaySettings);

            NEXUS_HdmiOutput_GetSettings(hdmi->handle, &hdmiOutputSettings);
            hdmiOutputSettings.syncOnly = true;
            NEXUS_HdmiOutput_SetSettings(hdmi->handle, &hdmiOutputSettings);

            boutput_hdmi_audio_mute(hdmi, true);
        }

        /* Retry auth if requested */
        if ( hdmi->desired.hdcp_failure_retry_delay > 0 )
        {
            if ( NULL != hdmi->hdcpTimer )
            {
                b_timer_cancel(hdmi->hdcpTimer);
            }
            b_timer_schedule(hdmi->desired.hdcp_failure_retry_delay, bdisplay_p_hdcp_retry, hdmi);
        }
        break;

    case NEXUS_HdmiOutputHdcpState_eLinkAuthenticated:
    case NEXUS_HdmiOutputHdcpState_eEncryptionEnabled:
        /* clear bluescreen */
        if (!hdmi->desired.hdcp_disable_blue_screen)
        {
            NEXUS_Display_GetSettings(hdmi->display->nDisplay, &displaySettings);
            displaySettings.background = 0xff000000; /* clear blue screen*/
            NEXUS_Display_SetSettings(hdmi->display->nDisplay, &displaySettings);

            NEXUS_HdmiOutput_GetSettings(hdmi->handle, &hdmiOutputSettings);
            hdmiOutputSettings.syncOnly = false;
            NEXUS_HdmiOutput_SetSettings(hdmi->handle, &hdmiOutputSettings);

            boutput_hdmi_audio_mute(hdmi, false);
        }
        break;

    default:
        break;
    }

    if ( hdmi->desired.hdmi_hdcp_status_callback )
    {
#if 0
        b_unlock();
        hdmi->desired.hdmi_hdcp_status_callback(hdmi->desired.callback_context);
        b_lock();
#endif
        b_callback_fire(hdmi->hdcpCallback);
    }
}

static void bdisplay_p_hdmi_hdcp_app_handler(void *context,int param)
{
    boutput_hdmi_t hdmi = context;
    BSTD_UNUSED(param);
    BDBG_WRN(("hdcp callback"));
    hdmi->desired.hdmi_hdcp_status_callback(hdmi->desired.callback_context);
}

#if NEXUS_HAS_CEC
static void bdisplay_p_hdmi_cec_callback(void *pParam, int iParam)
{
    BSTD_UNUSED(iParam);
    b_callback_fire(pParam);
}

static void bdisplay_p_cec_transmitted_handler(void *context,int param)
{
    boutput_hdmi_t hdmi = context;

    BDBG_ASSERT(NULL != hdmi);
    BSTD_UNUSED(param);

    BDBG_MSG(("CEC Transmitted Message Callback - Notifying application"));
    hdmi->messageReceived = false; /* transmitted message callback */
    if ( hdmi->desired.hdmi_cec_message_callback )
    {
        hdmi->desired.hdmi_cec_message_callback(hdmi->desired.callback_context);
    }
}

static void bdisplay_p_cec_received_handler(void *context,int param)
{
    boutput_hdmi_t hdmi = context;

    BDBG_ASSERT(NULL != hdmi);
    BSTD_UNUSED(param);

    BDBG_MSG(("CEC Received Message Callback - Notifying application"));
    hdmi->messageReceived = true;   /* received message callback */
    if ( hdmi->desired.hdmi_cec_message_callback )
        hdmi->desired.hdmi_cec_message_callback(hdmi->desired.callback_context);
}

static void bdisplay_p_cec_device_ready_handler(void *context,int param)
{
    boutput_hdmi_t hdmi = context;
    BDBG_ASSERT(NULL != hdmi);
    BSTD_UNUSED(param);

    BDBG_MSG(("CEC Device Ready Callback"));
    hdmi->deviceReady = true;
}

#endif

#endif /* B_N_DVI_OUTPUTS */

void bdisplay_p_init()
{
    /* coverity[stack_use_local_overflow] */
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    const char *outputDelay=NULL;
    unsigned i,j;

    NEXUS_Platform_GetConfiguration(&platformConfig);
    NEXUS_Platform_GetSettings(&platformSettings);

    BKNI_Memset(g_display, 0, sizeof(g_display));
    g_numDisplays = (platformConfig.supportedDisplay[0]?1:0) +
                    ((B_N_DISPLAYS > 1) ? (platformConfig.supportedDisplay[1]?1:0):0) +
                    ((B_N_DISPLAYS > 2) ? (platformConfig.supportedDisplay[2]?1:0):0);

    for (i=0;i<g_numDisplays;i++) {
        g_display[i].index = i;
        g_display[i].settings.content_mode = bdisplay_content_mode_box;
        g_display[i].numWindowsSupported = platformConfig.numWindowsPerDisplay;
        g_display[i].outputTimebase = 0xffffffff;
        for (j=0;j<g_display[i].numWindowsSupported;j++) {
            bdecode_window_t window = &g_display[i].window[j];
            window->display = &g_display[i];
            window->settings.position.width = 720;
            window->settings.position.height = 480;
            window->settings.zorder = j;
            window->settings.deinterlacer = (j==0 && i==0);
            window->playbackDeinterlacer = false;
        }
    }

#if B_N_SPDIF_OUTPUTS
    BKNI_Memset(g_spdif, 0, sizeof(g_spdif));
    for (i=0;i<B_N_SPDIF_OUTPUTS;i++) {
        g_spdif[i].handle = platformConfig.outputs.spdif[i];
        g_display[i].settings.spdif = &g_spdif[i];
        g_spdif[i].display = &g_display[i];
        g_spdif[i].volume.left = g_spdif[i].volume.right = BAUDIO_LEVEL_MAX;
        outputDelay = bsettop_get_config("spdif_output_delay");
        if(NULL != outputDelay)
        {
            NEXUS_AudioOutputSettings nOutputSettings;
            NEXUS_AudioOutput_GetSettings(NEXUS_SpdifOutput_GetConnector(g_spdif[i].handle),&nOutputSettings);
            nOutputSettings.additionalDelay = atoi(outputDelay);
            BDBG_MSG(("Spdif output delay %d",nOutputSettings.additionalDelay));
            NEXUS_AudioOutput_SetSettings(NEXUS_SpdifOutput_GetConnector(g_spdif[i].handle),&nOutputSettings);
        }
    }
#endif

#if B_N_AUDIO_DACS
    BKNI_Memset(g_audioDacs, 0, sizeof(g_audioDacs));
    for (i=0;i<B_N_AUDIO_DACS;i++) {
        g_audioDacs[i].handle = platformConfig.outputs.audioDacs[i];
        outputDelay = bsettop_get_config("dac_output_delay");
        if(NULL != outputDelay)
        {
            NEXUS_AudioOutputSettings nOutputSettings;
            NEXUS_AudioOutput_GetSettings(NEXUS_AudioDac_GetConnector(g_audioDacs[i].handle),&nOutputSettings);
            nOutputSettings.additionalDelay = atoi(outputDelay);
            BDBG_MSG(("Dac output delay %d",nOutputSettings.additionalDelay));
            NEXUS_AudioOutput_SetSettings(NEXUS_AudioDac_GetConnector(g_audioDacs[i].handle),&nOutputSettings);
        }
    }
#endif

#if B_N_COMPONENT_OUTPUTS
    BKNI_Memset(g_component, 0, sizeof(g_component));
    for (i=0;i<B_N_COMPONENT_OUTPUTS;i++) {
        g_component[i].handle = platformConfig.outputs.component[i];
        g_component[i].desired.type = boutput_component_type_yprpb;
        g_component[i].videoOutput = NEXUS_ComponentOutput_GetConnector(g_component[i].handle);
    }
#endif
#if B_N_COMPOSITE_OUTPUTS
    BKNI_Memset(g_composite, 0, sizeof(g_composite));
    for (i=0;i<B_N_COMPOSITE_OUTPUTS;i++) {
        g_composite[i].handle = platformConfig.outputs.composite[i];
    }
#endif
#if B_N_SVIDEO_OUTPUTS
    BKNI_Memset(g_svideo, 0, sizeof(g_svideo));
    for (i=0;i<B_N_SVIDEO_OUTPUTS;i++) {
        g_svideo[i].handle = platformConfig.outputs.svideo[i];
    }
#endif
#if B_N_DVI_OUTPUTS
    {
        /* NEXUS_HdmiOutputHdcpSettings >  1024 bytes */
        NEXUS_HdmiOutputHdcpSettings * pHdcpSettings=NULL;
        pHdcpSettings = BKNI_Malloc(sizeof(NEXUS_HdmiOutputHdcpSettings));
        if(pHdcpSettings == NULL)
        {
            BDBG_ERR(("Unable to allocate memory"));
            BDBG_ASSERT(NULL);
            return;
        }

        BKNI_Memset(g_hdmi, 0, sizeof(g_hdmi));

        for (i=0;i<B_N_DVI_OUTPUTS;i++)
        {
            NEXUS_Error rc;
            NEXUS_HdmiOutputSettings hdmiSettings;
#if NEXUS_HAS_CEC
            NEXUS_CecSettings cecSettings;
            NEXUS_HdmiOutputStatus hdmiStatus;
            
            g_hdmi[i].cecHandle = platformConfig.outputs.cec[i];
#endif

            g_hdmi[i].handle = platformConfig.outputs.hdmi[i];
            /* set HDMI handles here */
            g_display[i].settings.dvi= &g_hdmi[i];

            g_hdmi[i].event = B_Event_Create(NULL);
            BDBG_ASSERT(NULL != g_hdmi[i].event);

            g_hdmi[i].hdcpEvent = B_Event_Create(NULL);
            BDBG_ASSERT(NULL != g_hdmi[i].hdcpEvent);

            g_hdmi[i].eventId = b_event_register(g_hdmi[i].event, bdisplay_p_hdmi_hotplug_handler, &g_hdmi[i]);
            g_hdmi[i].hdcpEventId = b_event_register(g_hdmi[i].hdcpEvent, bdisplay_p_hdmi_hdcp_handler, &g_hdmi[i]);

            g_hdmi[i].hotplugCallback = b_callback_create(&g_hdmi[i],bdisplay_p_hdmi_hotplug_app_handler,&g_hdmi[i],0);
            BDBG_ASSERT(NULL != g_hdmi[i].hotplugCallback);
            g_hdmi[i].disconnectCalllback = b_callback_create(&g_hdmi[i],bdisplay_p_hdmi_disconnect_app_handler,&g_hdmi[i],0);
            BDBG_ASSERT(NULL != g_hdmi[i].disconnectCalllback );

            g_hdmi[i].hdcpCallback = b_callback_create(&g_hdmi[i],bdisplay_p_hdmi_hdcp_app_handler,&g_hdmi[i],0);
            BDBG_ASSERT(NULL != g_hdmi[i].hdcpCallback);

#if NEXUS_HAS_CEC
            g_hdmi[i].deviceReady = false;
            g_hdmi[i].cecTransmittedCallback = b_callback_create(&g_hdmi[i],bdisplay_p_cec_transmitted_handler,&g_hdmi[i],0);
            BDBG_ASSERT(NULL != g_hdmi[i].cecTransmittedCallback);
            g_hdmi[i].cecReceivedCallback = b_callback_create(&g_hdmi[i],bdisplay_p_cec_received_handler,&g_hdmi[i],0);
            BDBG_ASSERT(NULL != g_hdmi[i].cecReceivedCallback);
            g_hdmi[i].cecDeviceReadyCallback = b_callback_create(&g_hdmi[i],bdisplay_p_cec_device_ready_handler,&g_hdmi[i],0);
            BDBG_ASSERT(NULL != g_hdmi[i].cecDeviceReadyCallback);


            /* Prepare CEC settings  - enable CEC at hotplug connect */
            NEXUS_HdmiOutput_GetStatus(g_hdmi[i].handle, &hdmiStatus);
            NEXUS_Cec_GetSettings(g_hdmi[i].cecHandle, &cecSettings);           
            cecSettings.enabled = true;
            cecSettings.deviceType = NEXUS_CecDeviceType_eTuner;
            cecSettings.messageTransmittedCallback.callback = bdisplay_p_hdmi_cec_callback;
            cecSettings.messageTransmittedCallback.context = g_hdmi[i].cecTransmittedCallback;
            cecSettings.messageReceivedCallback.callback = bdisplay_p_hdmi_cec_callback;
            cecSettings.messageReceivedCallback.context = g_hdmi[i].cecReceivedCallback;
            cecSettings.logicalAddressAcquiredCallback.callback = bdisplay_p_hdmi_cec_callback;
            cecSettings.logicalAddressAcquiredCallback.context = g_hdmi[i].cecDeviceReadyCallback;
            cecSettings.physicalAddress[0]= (hdmiStatus.physicalAddressA << 4) 
                    | hdmiStatus.physicalAddressB;
            cecSettings.physicalAddress[1]= (hdmiStatus.physicalAddressC << 4) 
                    | hdmiStatus.physicalAddressD;
            cecSettings.deviceType = NEXUS_CecDeviceType_eTuner;
            rc = NEXUS_Cec_SetSettings(g_hdmi[i].cecHandle, &cecSettings);
            if (rc) BERR_TRACE(rc);
#endif          

            NEXUS_HdmiOutput_GetSettings(g_hdmi[i].handle, &hdmiSettings);
            hdmiSettings.hotplugCallback.callback = bdisplay_p_hdmi_hotplug_callback;
            hdmiSettings.hotplugCallback.context = g_hdmi[i].event;
            rc = NEXUS_HdmiOutput_SetSettings(g_hdmi[i].handle, &hdmiSettings);
            BDBG_ASSERT(rc == NEXUS_SUCCESS);

            /* Initialize/install HDCP callback to notify app */
            NEXUS_HdmiOutput_GetHdcpSettings(g_hdmi[i].handle, pHdcpSettings);
            pHdcpSettings->stateChangedCallback.callback = bdisplay_p_hdcp_status_callback;
            pHdcpSettings->stateChangedCallback.context = g_hdmi[i].hdcpEvent;
            pHdcpSettings->failureCallback.callback = bdisplay_p_hdcp_status_callback;
            pHdcpSettings->failureCallback.context = g_hdmi[i].hdcpEvent;
            /* TO DO: Add successCallback */
            rc = NEXUS_HdmiOutput_SetHdcpSettings(g_hdmi[i].handle, pHdcpSettings);
            if (rc != NEXUS_SUCCESS)
                BDBG_ERR(("Error initializing hdmi_output_hdcp settings"));

            outputDelay = bsettop_get_config("hdmi_output_delay");
            if (NULL != outputDelay)
            {
                NEXUS_AudioOutputSettings nOutputSettings;
                NEXUS_AudioOutput_GetSettings(NEXUS_HdmiOutput_GetAudioConnector(g_hdmi[i].handle),&nOutputSettings);
                nOutputSettings.additionalDelay = atoi(outputDelay);
                BDBG_MSG(("HDMI output delay %d",nOutputSettings.additionalDelay));
                NEXUS_AudioOutput_SetSettings(NEXUS_HdmiOutput_GetAudioConnector(g_hdmi[i].handle),&nOutputSettings);
            }
        }
        if(pHdcpSettings)
            BKNI_Free(pHdcpSettings);
    }
#endif
#if B_N_RF_OUTPUTS
    BKNI_Memset(g_rf, 0, sizeof(g_rf));
    for (i=0;i<B_N_RF_OUTPUTS;i++) {
        g_rf[i].handle = platformConfig.outputs.rfm[i];
        g_rf[i].desired.channel = 3;
        g_rf[i].desired.country[0] = 'U';
        g_rf[i].desired.country[1] = 'S';
    }
#if BCHP_CHIP == 7552
    {
        NEXUS_PlatformStatus platformStatus;    
        NEXUS_Error rc;

        rc = NEXUS_Platform_GetStatus(&platformStatus);
        if(rc != NEXUS_SUCCESS)
            BDBG_ERR(("Unable to get chip id"));        
        BDBG_MSG(("Chip ID = 0x%x", platformStatus.chipId));

        if(platformStatus.chipId == 0x7574)
        {
            NEXUS_GpioSettings gpioSettings;
            NEXUS_GpioStatus gpioStatus;

            NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &gpioSettings);
            gpioSettings.mode = NEXUS_GpioMode_eInput;
            gpioSettings.interruptMode = NEXUS_GpioInterrupt_eEdge;
            gpioSettings.interrupt.callback = bdisplay_p_rfm_gpio_callback;
            gpioSettings.interrupt.context = &g_rf[0];
            g_rf[0].pin = NEXUS_Gpio_Open(NEXUS_GpioType_eAonStandard, 20, &gpioSettings);

            NEXUS_Gpio_GetStatus(g_rf[0].pin, &gpioStatus);
            if(gpioStatus.value)
                g_rf[0].desired.channel = 4;
        }
    }
#endif /* BCHP_CHIP */
#endif /* B_N_RF_OUTPUTS */

#if B_N_COMPONENT_OUTPUTS
    g_display[0].settings.component = &g_component[0];
#endif
#if B_N_COMPOSITE_OUTPUTS
#if BCHP_CHIP == 7422 || BCHP_CHIP == 7425  || BCHP_CHIP == 7435  || BCHP_CHIP == 7346 || BCHP_CHIP == 7344 || BCHP_CHIP == 7468
    g_display[1].settings.composite = &g_composite[0];
#else
    g_display[0].settings.composite = &g_composite[0];
#endif
#endif
#if B_N_SVIDEO_OUTPUTS
#if BCHP_CHIP == 7468
    g_display[1].settings.svideo = &g_svideo[0];
#else
    g_display[0].settings.svideo = &g_svideo[0];
#endif
#endif
#if B_N_RF_OUTPUTS
    g_display[0].settings.rf = &g_rf[0];
#endif
}

void bdisplay_p_uninit()
{
    unsigned i;

    /* must teardown HDMI first so that HDMI events don't access closed display handles */
#if B_N_DVI_OUTPUTS
    {
        /* NEXUS_HdmiOutputHdcpSettings > 1024 bytes */ 
        NEXUS_HdmiOutputHdcpSettings * pHdcpSettings=NULL;		
        pHdcpSettings = BKNI_Malloc(sizeof(NEXUS_HdmiOutputHdcpSettings));
        if (pHdcpSettings == NULL)
        {
            BDBG_ERR(("Unable to allocate memory"));
            BDBG_ASSERT(NULL);
            return;
        }

        for (i=0;i<B_N_DVI_OUTPUTS;i++)
        {
            NEXUS_Error rc;
            NEXUS_HdmiOutputSettings hdmiSettings;
#if NEXUS_HAS_CEC
            NEXUS_CecSettings cecSettings;
#endif
            
            if (!g_hdmi[i].handle) continue;

            NEXUS_StopCallbacks(g_hdmi[i].handle);

            /* Initialize/install HDCP callback to notify app */
            NEXUS_HdmiOutput_GetHdcpSettings(g_hdmi[i].handle, pHdcpSettings);
            pHdcpSettings->stateChangedCallback.callback = NULL;
            pHdcpSettings->failureCallback.callback = NULL;
            pHdcpSettings->stateChangedCallback.context = NULL;
            pHdcpSettings->failureCallback.context = NULL;
            rc = NEXUS_HdmiOutput_SetHdcpSettings(g_hdmi[i].handle, pHdcpSettings);
            if (rc != NEXUS_SUCCESS)
                BDBG_ERR(("Error uninit hdmi_output_hdcp settings"));

            NEXUS_HdmiOutput_GetSettings(g_hdmi[i].handle, &hdmiSettings);
            hdmiSettings.hotplugCallback.callback = NULL;
            hdmiSettings.hotplugCallback.context = NULL;
            rc = NEXUS_HdmiOutput_SetSettings(g_hdmi[i].handle, &hdmiSettings);

            if ( NULL != g_hdmi[i].hdcpTimer )
            {
                b_timer_cancel(g_hdmi[i].hdcpTimer);
            }

            b_event_unregister(g_hdmi[i].eventId);
            b_event_unregister(g_hdmi[i].hdcpEventId);
            B_Event_Destroy(g_hdmi[i].event);
            B_Event_Destroy(g_hdmi[i].hdcpEvent);


            b_callback_destroy(g_hdmi[i].hotplugCallback);
            b_callback_destroy(g_hdmi[i].disconnectCalllback);

#if NEXUS_HAS_CEC
            NEXUS_Cec_GetSettings(g_hdmi[i].cecHandle, &cecSettings);
            cecSettings.logicalAddressAcquiredCallback.callback = NULL;
            cecSettings.logicalAddressAcquiredCallback.context = NULL;
            cecSettings.messageReceivedCallback.callback = NULL;
            cecSettings.messageReceivedCallback.context = NULL;
            cecSettings.messageTransmittedCallback.callback = NULL;
            cecSettings.messageTransmittedCallback.context = NULL;
                
            b_callback_destroy(g_hdmi[i].cecTransmittedCallback);
            b_callback_destroy(g_hdmi[i].cecReceivedCallback);
            b_callback_destroy(g_hdmi[i].cecDeviceReadyCallback);           
#endif          
            b_callback_destroy(g_hdmi[i].hdcpCallback);
        }
        BKNI_Free(pHdcpSettings);
        BKNI_Memset(g_hdmi, 0, sizeof(g_hdmi));
    }
#endif

    for (i=0;i<g_numDisplays;i++) {
        if (g_display[i].nDisplay) {
            bdisplay_close(&g_display[i]);
        }
    }
}

bdisplay_t bdisplay_open(bobject_t display_id)
{
    unsigned index = B_ID_GET_INDEX(display_id);
    bdisplay_t display;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_DisplaySettings displaySettings;
    bdisplay_settings temp_settings;

    if (index >= g_numDisplays) {
        BSETTOP_ERROR(berr_not_available);
        return NULL;
    }
    display = &g_display[index];
    if (display->nDisplay) {
        BSETTOP_ERROR(berr_not_available);
        return NULL;
    }

    BDBG_OBJECT_SET(display, bdisplay);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display->nDisplay = NEXUS_Display_Open(index, &displaySettings);
    if (!display->nDisplay) {
        BSETTOP_ERROR(berr_not_available);
        goto error;
    }
    display->nAudioMixer = NEXUS_AudioMixer_Open(NULL);
    if (!display->nAudioMixer) {
        BSETTOP_ERROR(berr_not_available);
        goto error;
    }

    /* DAC's are always connected to pcm mixer */
    NEXUS_Platform_GetConfiguration(&platformConfig);
    if (index < NEXUS_NUM_AUDIO_DACS) {
        display->audioDac.handle = platformConfig.outputs.audioDacs[index];
    }
    if (display->audioDac.handle) {
        NEXUS_AudioOutput_AddInput(
            NEXUS_AudioDac_GetConnector(display->audioDac.handle),
            NEXUS_AudioMixer_GetConnector(display->nAudioMixer)
            );
    }

#if B_N_DVI_OUTPUTS
    if (display->settings.dvi)
    {
        display->settings.dvi->display = display;
        display->settings.dvi->connected = false;
        /* If PCM, add HDMI to the mixer now before hotplug */
        if ( display->settings.dvi->desired.hdmi_audio_mode == boutput_hdmi_audio_mode_pcm )
        {
            NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(display->settings.dvi->handle),
                                       NEXUS_AudioMixer_GetConnector(display->nAudioMixer));
        }
    }
#endif /* B_N_DVI_OUTPUTS */

#if 1 /* Cannot set dac audio level here */
    display->volume.left = BAUDIO_LEVEL_MAX;
    display->volume.right = BAUDIO_LEVEL_MAX;
    display->volume.muted = false;
    bdisplay_set_dac_audio_volume(display, &display->volume);
#endif

    temp_settings = display->settings;
    display->settings.component = NULL;
    display->settings.composite = NULL;
    display->settings.composite2 = NULL;
    display->settings.svideo = NULL;
    display->settings.rf = NULL;
    bdisplay_set(display, &temp_settings);

    return display;

error:
    bdisplay_close(display);
    return NULL;
}

void bdisplay_close(bdisplay_t display)
{
    BDBG_OBJECT_ASSERT(display, bdisplay);

    if (display->audioDac.handle) {
        NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(display->audioDac.handle));
    }
#if B_N_DVI_OUTPUTS
    if (display->settings.dvi && display->settings.dvi->handle) {
        NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(display->settings.dvi->handle));
    }
#endif
    if (display->settings.spdif && display->settings.spdif->handle) {
        NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(display->settings.spdif->handle));
    }

    if (display->nDisplay) {
        NEXUS_Display_RemoveAllOutputs(display->nDisplay);
        NEXUS_Display_Close(display->nDisplay);
        display->nDisplay = NULL;
    }
    if (display->nAudioMixer) {
        NEXUS_AudioInput_Shutdown(NEXUS_AudioMixer_GetConnector(display->nAudioMixer));
        NEXUS_AudioMixer_Close(display->nAudioMixer);
        display->nAudioMixer = NULL;
    }
    BDBG_OBJECT_UNSET(display, bdisplay);
}

void nexus2rect(bsettop_rect *settop_rect, const NEXUS_Rect *pRect)
{
    settop_rect->x = pRect->x;
    settop_rect->y = pRect->y;
    settop_rect->width = pRect->width;
    settop_rect->height = pRect->height;
}

bresult bdecode_window_get(bdecode_window_t window, bdecode_window_settings *settings)
{
    NEXUS_VideoWindowSettings nWindowSettings;
    NEXUS_PictureCtrlCommonSettings nPictureCtrlCommonSettings;
    #if B_N_DNR
    NEXUS_VideoWindowDnrSettings nDnrSettings;
    #endif
    #if B_HAS_ANR
     NEXUS_VideoWindowAnrSettings nAnrSettings;
    #endif
    BDBG_MSG(("bdecode_window_get"));
    window->settings.cloned = window->parent?1:0;

    *settings = window->settings;

    NEXUS_VideoWindow_GetSettings(window->nWindow, &nWindowSettings);
    nexus2rect(&settings->position, &nWindowSettings.position);
    nexus2rect(&settings->cliprect, &nWindowSettings.clipRect);
    settings->visible = nWindowSettings.visible;
    settings->box_detect = nWindowSettings.letterBoxDetect;
    
    NEXUS_PictureCtrl_GetCommonSettings(window->nWindow,&nPictureCtrlCommonSettings);
    settings->brightness = nPictureCtrlCommonSettings.brightness;
    settings->contrast = nPictureCtrlCommonSettings.contrast;
    settings->hue = nPictureCtrlCommonSettings.hue;
    settings->saturation = nPictureCtrlCommonSettings.saturation;
    settings->sharpness = nPictureCtrlCommonSettings.sharpness;

    #if B_N_DNR
    NEXUS_VideoWindow_GetDnrSettings(window->nWindow,&nDnrSettings);

    settings->dcr_level = bnexus_to_settop_pq_levels(((nDnrSettings.dcr.mode == NEXUS_VideoWindowFilterMode_eBypass)?
                                                     (int32_t)SETTOP_ANR_DNR_OFF_LEVEL:nDnrSettings.dcr.level),pq_dcr);
    settings->mnr_level = bnexus_to_settop_pq_levels(((nDnrSettings.mnr.mode == NEXUS_VideoWindowFilterMode_eBypass)?
                                                     (int32_t)SETTOP_ANR_DNR_OFF_LEVEL:nDnrSettings.mnr.level),pq_mnr);
    settings->bnr_level = bnexus_to_settop_pq_levels(((nDnrSettings.bnr.mode == NEXUS_VideoWindowFilterMode_eBypass)?
                                                     (int32_t)SETTOP_ANR_DNR_OFF_LEVEL:nDnrSettings.bnr.level),pq_bnr);
    #endif

    #if B_HAS_ANR
    if (window == gWindowMadOwner)
    {
      NEXUS_VideoWindow_GetAnrSettings(window->nWindow, &nAnrSettings);
      settings->anr_level = bnexus_to_settop_pq_levels(((nAnrSettings.anr.mode == NEXUS_VideoWindowFilterMode_eBypass)?
                                                        (int32_t)SETTOP_ANR_DNR_OFF_LEVEL:nAnrSettings.anr.level),pq_anr);
    }
    #endif
    return 0;
}

void rect2nexus(NEXUS_Rect *pRect, const bsettop_rect *settop_rect)
{
    pRect->x = settop_rect->x;
    pRect->y = settop_rect->y;
    pRect->width = settop_rect->width;
    pRect->height = settop_rect->height;
}





/* use this api to set biox detect on both main and clone windows in HDSD simul mode*/
static bresult bdecode_p_window_set_boxdetect(bdecode_window_t window,bool box_detect)
{
    NEXUS_Error rc=0;
    NEXUS_VideoWindowSettings nWindowSettings;
    bdecode_window_t parent=NULL;
    bdecode_window_t clone=NULL;
    if(window->clone)
    {
        clone=window->clone;
        parent=window;
    }
    else if(window->parent)
    {
        clone = window;
        parent=window->parent;
    }
    else
    {
        parent=window;
        clone=NULL;
    }
    if(clone )
    {
        NEXUS_VideoWindow_GetSettings(clone->nWindow, &nWindowSettings);
        nWindowSettings.letterBoxDetect = box_detect;
        nWindowSettings.letterBoxAutoCut = true;
        rc = NEXUS_VideoWindow_SetSettings(clone->nWindow, &nWindowSettings);
        if (rc){rc  = BSETTOP_ERROR(berr_external_error); return rc;}
        /* save applied box detect settings */
        clone->settings.box_detect=box_detect;
    }
    if(parent)
    {
        NEXUS_VideoWindow_GetSettings(parent->nWindow, &nWindowSettings);
        nWindowSettings.letterBoxDetect = box_detect;
        nWindowSettings.letterBoxAutoCut = true;
        rc = NEXUS_VideoWindow_SetSettings(parent->nWindow, &nWindowSettings);
        if (rc && rc!=NEXUS_NOT_AVAILABLE){rc  = BSETTOP_ERROR(berr_external_error); return rc;}
        /* save applied box detect settings */
        parent->settings.box_detect=box_detect;
    }
    return rc;
}

/* set window contecnt mode according to setting in the correcponding displays */
static bresult bdecode_p_window_set_content_mode(bdecode_window_t window)
{
    NEXUS_Error rc=0;
    NEXUS_VideoWindowSettings nWindowSettings;
    bdecode_window_t parent=NULL;
    bdecode_window_t clone=NULL;
    if(window->clone)
    {
        clone=window->clone;
        parent=window;
    }
    else if(window->parent)
    {
        clone = window;
        parent=window->parent;
    }
    else
    {
        parent=window;
        clone=NULL;
    }
#if 0
    if(window->parent == NULL && parent && clone)
    {
        if (parent->display->settings.content_mode == bdisplay_content_mode_zoom || parent->display->settings.content_mode == bdisplay_content_mode_panscan  )
        {
            BDBG_WRN(("forcing %d content mode on SD display",parent->display->settings.content_mode));
            clone->display->settings.content_mode= parent->display->settings.content_mode;
        }
    }
#endif
    if(parent)
    {
        NEXUS_VideoWindow_GetSettings(parent->nWindow, &nWindowSettings);
        nWindowSettings.contentMode=parent->display->settings.content_mode;
        rc = NEXUS_VideoWindow_SetSettings(parent->nWindow, &nWindowSettings);
        if (rc){rc  = BSETTOP_ERROR(berr_external_error); return rc;}
    }
    if(clone )
    {
        NEXUS_VideoWindow_GetSettings(clone->nWindow, &nWindowSettings);
        nWindowSettings.contentMode=clone->display->settings.content_mode;
        rc = NEXUS_VideoWindow_SetSettings(clone->nWindow, &nWindowSettings);
        if (rc){rc  = BSETTOP_ERROR(berr_external_error); return rc;}
    }
    return rc;
}
/* force the content modes on windows for both displays */
static bresult bdecode_p_window_set_force_content_mode(bdecode_window_t window,bdisplay_content_mode content_mode)
{
    NEXUS_Error rc=0;
    NEXUS_VideoWindowSettings nWindowSettings;
    bdecode_window_t parent=NULL;
    bdecode_window_t clone=NULL;
    if(window->clone)
    {
        clone=window->clone;
        parent=window;
    }
    else if(window->parent)
    {
        clone = window;
        parent=window->parent;
    }
    else
    {
        parent=window;
        clone=NULL;
    }
    if(parent)
    {
        NEXUS_VideoWindow_GetSettings(parent->nWindow, &nWindowSettings);
        nWindowSettings.contentMode=content_mode;
        rc = NEXUS_VideoWindow_SetSettings(parent->nWindow, &nWindowSettings);
        if (rc){rc  = BSETTOP_ERROR(berr_external_error); return rc;}
    }
    if(clone )
    {
        NEXUS_VideoWindow_GetSettings(clone->nWindow, &nWindowSettings);
        nWindowSettings.contentMode=content_mode;
        rc = NEXUS_VideoWindow_SetSettings(clone->nWindow, &nWindowSettings);
        if (rc){rc  = BSETTOP_ERROR(berr_external_error); return rc;}
    }
    return rc;
}

bresult bdecode_window_set(bdecode_window_t window, const bdecode_window_settings *settings)
{
    NEXUS_VideoWindowSettings nWindowSettings;
    NEXUS_PictureCtrlCommonSettings nPictureCtrlCommonSettings;
#if B_N_DNR
    NEXUS_VideoWindowDnrSettings nDnrSettings;
#endif
#if B_HAS_ANR
    NEXUS_VideoWindowAnrSettings nAnrSettings;
#endif
    NEXUS_VideoWindowMadSettings nMadSettings;
    NEXUS_Error rc=0;

    BDBG_MSG(("bdecode_window_set window %p %d,%d,%d,%d",window,settings->position.x, settings->position.y, settings->position.width, settings->position.height));

    /* switch to display manual mode */
    NEXUS_DisplayModule_SetUpdateMode(NEXUS_DisplayUpdateMode_eManual);

    NEXUS_VideoWindow_GetSettings(window->nWindow, &nWindowSettings);
    rect2nexus(&nWindowSettings.position, &settings->position);
    rect2nexus(&nWindowSettings.clipRect, &settings->cliprect);
    nWindowSettings.clipBase.x = 0;
    nWindowSettings.clipBase.y = 0;
    nWindowSettings.clipBase.width = settings->position.width;
    nWindowSettings.clipBase.height = settings->position.height;
    nWindowSettings.visible = settings->visible;
    /* nWindowSettings.zorder is set in bsettop_p_display_apply_zorder */
#if B_DECODE_CRC_CAPTURE
    nWindowSettings.forceCapture = true;
#endif

    rc = NEXUS_VideoWindow_SetSettings(window->nWindow, &nWindowSettings);
#if 0
    if (rc && rc!=NEXUS_NOT_AVAILABLE){rc  = BSETTOP_ERROR(berr_external_error); goto error;}
#endif
    rc = bdecode_p_window_set_boxdetect(window,settings->box_detect);
    if (rc && rc!=NEXUS_NOT_AVAILABLE){rc  = BSETTOP_ERROR(berr_external_error); goto error;}

#if B_DECODE_CRC_CAPTURE
    {
    NEXUS_VideoWindowScalerSettings scalerSettings;
    NEXUS_VideoWindow_GetScalerSettings(window->nWindow, &scalerSettings);
    scalerSettings.bandwidthEquationParams.delta = 1000000;
    scalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eScalerBeforeCapture;
    rc = NEXUS_VideoWindow_SetScalerSettings(window->nWindow, &scalerSettings);
    if (rc){rc  = BSETTOP_ERROR(berr_external_error); goto error;}
    }
#endif

    bsettop_p_display_zorder_set(window->display, window, settings->zorder);

    if (b_window_is_full_screen(&window->display->settings, &settings->position) && settings->visible && window->decode)
    {
        BDBG_MSG(("Window is full screen set frame master %p",window));
        bdisplay_p_set_framerate_master(window->display, window->decode);
    }

    if (b_window_is_full_screen(&window->display->settings, &settings->position) && settings->visible)
    {
        /* apply aspect ratio conversion to full-screen window and clone */
        rc = bdecode_p_window_set_content_mode(window);
        if (rc){rc  = BSETTOP_ERROR(berr_external_error); goto error;}

    }
    else
    {
        /* don't apply aspect ratio conversion for scaled down windows */
        /* force content mode on this window and clone */
        rc = bdecode_p_window_set_force_content_mode(window,NEXUS_VideoWindowContentMode_eFull);
        if (rc){rc  = BSETTOP_ERROR(berr_external_error); goto error;}
    }

/* TODO:change this logic based on number of mads not based on chip */
#if ( (BCHP_CHIP != 7420) && (BCHP_CHIP != 7422) && (BCHP_CHIP != 7425) && (BCHP_CHIP != 7429) && (BCHP_CHIP != 7435) && (BCHP_CHIP != 7231) &&\
      (BCHP_CHIP != 7344) && (BCHP_CHIP != 7346) && (BCHP_CHIP != 7584))
    /* we have one MAD, make sure we give it to bigger window */
    if(settings->deinterlacer && settings->visible )
    {
        if(gWindowMadOwner == NULL )
        {
            NEXUS_VideoWindow_GetMadSettings(window->nWindow,&nMadSettings);
            nMadSettings.deinterlace = true;
            nMadSettings.gameMode = window->playbackDeinterlacer ? NEXUS_VideoWindowGameMode_e5Fields_ForceSpatial : NEXUS_VideoWindowGameMode_eOff;
            rc = NEXUS_VideoWindow_SetMadSettings(window->nWindow,&nMadSettings);
            if (rc){rc  = BSETTOP_ERROR(berr_external_error); goto error;}
            gWindowMadOwner = window;
            BDBG_MSG(("Mad enabled on window %p",window->nWindow));

        }
        else if(gWindowMadOwner != window && settings->position.height >=  gWindowMadOwner->settings.position.height)
        {
            BDBG_MSG(("Mad disabled on window %p enabled on window%p",gWindowMadOwner->nWindow,window->nWindow));
            NEXUS_VideoWindow_GetMadSettings(gWindowMadOwner->nWindow,&nMadSettings);
            nMadSettings.deinterlace = false;
            rc = NEXUS_VideoWindow_SetMadSettings(gWindowMadOwner->nWindow,&nMadSettings);
            if (rc){rc  = BSETTOP_ERROR(berr_external_error); goto error;}
            rc = NEXUS_DisplayModule_SetUpdateMode(NEXUS_DisplayUpdateMode_eNow);
            if (rc){rc  = BSETTOP_ERROR(berr_external_error); goto error;}

            NEXUS_VideoWindow_GetMadSettings(window->nWindow,&nMadSettings);
            nMadSettings.deinterlace = true;
            nMadSettings.gameMode = window->playbackDeinterlacer ? NEXUS_VideoWindowGameMode_e5Fields_ForceSpatial : NEXUS_VideoWindowGameMode_eOff;
            rc = NEXUS_VideoWindow_SetMadSettings(window->nWindow,&nMadSettings);
            if (rc){rc  = BSETTOP_ERROR(berr_external_error); goto error;}
            gWindowMadOwner = window;
        }
    }
    else if(gWindowMadOwner == window)
    {
        NEXUS_VideoWindow_GetMadSettings(window->nWindow,&nMadSettings);
        nMadSettings.deinterlace = false;
        rc = NEXUS_VideoWindow_SetMadSettings(window->nWindow,&nMadSettings);
        if (rc){rc  = BSETTOP_ERROR(berr_external_error); goto error;}
        BDBG_MSG(("Mad disabled on window %p",window->nWindow));
        gWindowMadOwner = NULL;
    }
#else
#if B_HAS_MAD
    /* Apply settings only to the main window of the main display */
    if (window == gWindowMadOwner)
    {
        if(settings->deinterlacer )
        {
            NEXUS_VideoWindow_GetMadSettings(window->nWindow,&nMadSettings);
            nMadSettings.deinterlace = true;
            /* TODO: force spatial is bad quality for pause; however some people favored the same delay on HD/SD simul output than HD quality; */
            nMadSettings.gameMode = window->playbackDeinterlacer ? NEXUS_VideoWindowGameMode_e4Fields_0Delay: NEXUS_VideoWindowGameMode_eOff;
            rc = NEXUS_VideoWindow_SetMadSettings(window->nWindow,&nMadSettings);
            if (rc){rc  = BSETTOP_ERROR(berr_external_error); goto error;}
            BDBG_MSG(("Mad enabled on window %p",window->nWindow));
        }
        else
        {
            NEXUS_VideoWindow_GetMadSettings(window->nWindow,&nMadSettings);
            nMadSettings.deinterlace = false;
            rc = NEXUS_VideoWindow_SetMadSettings(window->nWindow,&nMadSettings);
            if (rc){rc  = BSETTOP_ERROR(berr_external_error); goto error;}
            BDBG_MSG(("Mad disabled on window %p",window->nWindow));
        }
    }
#endif
#endif

    /**
    * DNR
    **/
#if B_N_DNR
#if B_HAS_LEGACY_VDC
    if(!window->settings.cloned){
      NEXUS_VideoWindow_GetDnrSettings(window->nWindow,&nDnrSettings);
      nDnrSettings.bnr.level = settings->bnr_level;
      nDnrSettings.dcr.level = settings->dcr_level;
      nDnrSettings.mnr.level = settings->mnr_level;
      rc = NEXUS_VideoWindow_SetDnrSettings(window->nWindow, &nDnrSettings);
      if (rc){rc  = BSETTOP_ERROR(berr_external_error); goto error;}
    }
#else
    NEXUS_VideoWindow_GetDnrSettings(window->nWindow,&nDnrSettings);
    BDBG_MSG(("-> DNR Settings: %d bnr_level[settings->bnr_level] %d  nDnrSettings.bnr.mode  %d nDnrSettings.bnr.level %d",settings->bnr_level,bnr_level[settings->bnr_level], nDnrSettings.bnr.mode,nDnrSettings.bnr.level));
    BDBG_MSG(("-> DNR Settings: %d dcr_level[settings->dcr_level] %d  nDnrSettings.dcr.mode  %d nDnrSettings.dcr.level %d",settings->dcr_level,dcr_level[settings->dcr_level], nDnrSettings.dcr.mode,nDnrSettings.dcr.level));
    BDBG_MSG(("-> DNR Settings: %d mnr_level[settings->mnr_level]%d  nDnrSettings.mnr.mode  %d nDnrSettings.mnr.level %d",settings->mnr_level,mnr_level[settings->mnr_level], nDnrSettings.mnr.mode,nDnrSettings.mnr.level));

    if( nDnrSettings.bnr.level != bnr_level[settings->bnr_level]
        ||  nDnrSettings.dcr.level != dcr_level[settings->dcr_level]
        || nDnrSettings.mnr.level != mnr_level[settings->mnr_level] )
     {
       nDnrSettings.bnr.mode = NEXUS_VideoWindowFilterMode_eBypass;
       nDnrSettings.dcr.mode = NEXUS_VideoWindowFilterMode_eBypass;
       nDnrSettings.mnr.mode = NEXUS_VideoWindowFilterMode_eBypass;

       if(settings->bnr_level != bwindow_dnr_level_off)
          nDnrSettings.bnr.mode = NEXUS_VideoWindowFilterMode_eEnable;
       nDnrSettings.bnr.level = bnr_level[settings->bnr_level];

      if(settings->dcr_level != bwindow_dnr_level_off)
        nDnrSettings.dcr.mode = NEXUS_VideoWindowFilterMode_eEnable;
      nDnrSettings.dcr.level = dcr_level[settings->dcr_level];

      if(settings->mnr_level != bwindow_dnr_level_off)
        nDnrSettings.mnr.mode = NEXUS_VideoWindowFilterMode_eEnable;
      nDnrSettings.mnr.level = mnr_level[settings->mnr_level];

      rc = NEXUS_VideoWindow_SetDnrSettings(window->nWindow, &nDnrSettings);
      if (rc){rc  = BSETTOP_ERROR(berr_external_error); goto error;}
      BDBG_MSG(("<- DNR Settings:  nDnrSettings.bnr.mode  %d nDnrSettings.bnr.level %d",  nDnrSettings.bnr.mode,nDnrSettings.bnr.level));
      BDBG_MSG(("<- DNR Settings:  nDnrSettings.dcr.mode  %d nDnrSettings.dcr.level %d", nDnrSettings.dcr.mode,nDnrSettings.dcr.level));
      BDBG_MSG(("<- DNR Settings:  nDnrSettings.mnr.mode  %d nDnrSettings.mnr.level %d", nDnrSettings.mnr.mode,nDnrSettings.mnr.level));
    }
#endif
#endif

#if B_HAS_ANR
    /**
    * ANR
    **/
    if (window == gWindowMadOwner)
    {
        NEXUS_VideoWindow_GetAnrSettings(window->nWindow, &nAnrSettings);
        BDBG_MSG(("-> ANR Settings: %d anr_level[settings->anr_level] %d nAnrSettings.anr.mode  %d  nAnrSettings.anr.level %d",settings->anr_level,anr_level[settings->anr_level],nAnrSettings.anr.mode, nAnrSettings.anr.level));
        if(nAnrSettings.anr.level != anr_level[settings->anr_level])
        {
          nAnrSettings.anr.mode = NEXUS_VideoWindowFilterMode_eBypass;
          if(settings->anr_level != bwindow_dnr_level_off)
            nAnrSettings.anr.mode = NEXUS_VideoWindowFilterMode_eEnable;
          nAnrSettings.anr.level = anr_level[settings->anr_level];
          rc = NEXUS_VideoWindow_SetAnrSettings(window->nWindow, &nAnrSettings);
          if (rc){rc  = BSETTOP_ERROR(berr_external_error); goto error;}
          BDBG_MSG(("<- ANR Settings: nAnrSettings.anr.mode  %d  nAnrSettings.anr.level %d",nAnrSettings.anr.mode, nAnrSettings.anr.level));
        }
    }
#endif

    NEXUS_PictureCtrl_GetCommonSettings(window->nWindow,&nPictureCtrlCommonSettings);
    nPictureCtrlCommonSettings.brightness = settings->brightness;
    nPictureCtrlCommonSettings.contrast = settings->contrast;
    nPictureCtrlCommonSettings.hue = settings->hue;
    nPictureCtrlCommonSettings.saturation = settings->saturation;
    nPictureCtrlCommonSettings.sharpness = settings->sharpness;
    nPictureCtrlCommonSettings.sharpnessEnable = 1;/* settings->sharpness?1:0; */
    rc =NEXUS_PictureCtrl_SetCommonSettings(window->nWindow,&nPictureCtrlCommonSettings);
    if (rc){rc  = BSETTOP_ERROR(berr_external_error); goto error;}
    
#if NEXUS_NUM_CAB
{
    NEXUS_PictureCtrlAdvColorSettings nPictureAdvSettings;
    /* coverity[stack_use_local_overflow] */
    NEXUS_PictureCtrlContrastStretch contrastStretchSettings;

    NEXUS_PictureCtrl_GetAdvColorSettings(window->nWindow,&nPictureAdvSettings);
    nPictureAdvSettings.fleshTone = settings->auto_flesh_tone;
    nPictureAdvSettings.blueBoost = settings->blue_stretch;
    nPictureAdvSettings.greenBoost = settings->green_stretch;
    rc = NEXUS_PictureCtrl_SetAdvColorSettings(window->nWindow,&nPictureAdvSettings);
    if (rc){rc  = BSETTOP_ERROR(berr_external_error); goto error;}
    /* settings->display_rate_master; */

    NEXUS_PictureCtrl_GetContrastStretch(window->nWindow, &contrastStretchSettings);
    contrastStretchSettings.gain = settings->dynamic_contrast;
    contrastStretchSettings.gainShift = 8;
    contrastStretchSettings.enabled = true;
    rc = NEXUS_PictureCtrl_SetContrastStretch(window->nWindow, &contrastStretchSettings);
    if (rc){rc  = BSETTOP_ERROR(berr_external_error); goto error;}
}
#endif

{
    NEXUS_VideoWindowSplitScreenSettings splitScreen;
    NEXUS_VideoWindow_GetSplitScreenSettings(window->nWindow, &splitScreen);

    splitScreen.hue = settings->splitscreen_mode;
    splitScreen.contrast = settings->splitscreen_mode;
    splitScreen.brightness = settings->splitscreen_mode;
    splitScreen.colorTemp = settings->splitscreen_mode;
    splitScreen.sharpness = settings->splitscreen_mode;
    splitScreen.contrastStretch = settings->splitscreen_mode;
    splitScreen.dnr = settings->splitscreen_mode;
    splitScreen.anr = settings->splitscreen_mode;
    splitScreen.autoFlesh = settings->splitscreen_mode;
    splitScreen.blueBoost = settings->splitscreen_mode;
    splitScreen.greenBoost = settings->splitscreen_mode;
    splitScreen.blueStretch = settings->splitscreen_mode;
    splitScreen.dejagging = settings->splitscreen_mode;
    splitScreen.deringing = settings->splitscreen_mode;

    NEXUS_VideoWindow_SetSplitScreenSettings(window->nWindow, &splitScreen);
}

    window->settings = *settings;
    rc = NEXUS_DisplayModule_SetUpdateMode(NEXUS_DisplayUpdateMode_eNow);
    if (rc){rc  = BSETTOP_ERROR(berr_external_error); goto error;}
error:
    /* switch back to auto mode and apply the settings */
    NEXUS_DisplayModule_SetUpdateMode(NEXUS_DisplayUpdateMode_eAuto);
    return rc;

}

bresult bdecode_p_window_set_no_delay( bdecode_window_t window, bool noDelay ) {

    bdecode_window_settings settings;
    NEXUS_Error rc=0;

    bdecode_window_get(window, &settings);
    if ( noDelay ) {
        if ( settings.deinterlacer ) {
            /* Turn it off */
            BDBG_MSG(("Turning de-interlacer OFF in playback pause"));
            window->playbackDeinterlacer = true;
            rc = bdecode_window_set(window, &settings);
        }
    }
    else {
        /* Restore old setting */
        if( settings.deinterlacer ) {
            BDBG_MSG(("Turning de-interlacer ON in normal play"));
            window->playbackDeinterlacer = false;
            rc = bdecode_window_set(window, &settings);
        }
    }
    return rc;
}

bresult boutput_rf_set(boutput_rf_t rf, const boutput_rf_settings *settings)
{
    rf->desired = *settings;
    return 0;
}

static bool b_is_hd(bvideo_format video_format)
{
    bvideo_format_settings format_settings;
    bvideo_get_format_settings(video_format, &format_settings);
    return (format_settings.height > 576) || !format_settings.interlaced;
}

void
bdisplay_get(bdisplay_t display, bdisplay_settings *settings)
{
    BDBG_OBJECT_ASSERT(display, bdisplay);
    *settings = display->settings;
}

bresult
bdisplay_set(bdisplay_t display, const bdisplay_settings *settings)
{
    NEXUS_DisplaySettings nSettings;
    NEXUS_ComponentOutputSettings nComponentOutputSettings;
#if B_N_DVI_OUTPUTS
    NEXUS_HdmiOutputSettings nHdmiOutputSettings;
    NEXUS_HdmiOutputStatus nHdmiOutputStatus;
#endif
    NEXUS_VideoFormat prevFormat;
    NEXUS_Error rc;
    bresult result = b_ok;
#if MACROVISION_SUPPORT
    NEXUS_DisplayVbiSettings nDisplayVbiSettings;
#endif
    NEXUS_VideoFormatInfo videoFormatInfo;
    NEXUS_VideoFormat format;

#if B_N_RF_OUTPUTS > 0
    bool change_rf =
        settings->rf != display->settings.rf ||
        (settings->rf &&
         BKNI_Memcmp(&settings->rf->cfg, &settings->rf->desired,
            sizeof(settings->rf->cfg)));
#endif
#if B_N_DVI_OUTPUTS > 0
    bool change_dvi =
        settings->dvi != display->settings.dvi ||
        (settings->dvi && !settings->dvi->connected) ||
        settings->aspect_ratio != display->settings.aspect_ratio ||
        (settings->dvi &&
         BKNI_Memcmp(&settings->dvi->cfg, &settings->dvi->desired,
            sizeof(settings->dvi->cfg)));
#endif

#if B_N_SPDIF_OUTPUTS > 0
    bool change_spdif =
        display->settings.spdif != settings->spdif ||
        (settings->spdif &&
        BKNI_Memcmp(&settings->spdif->cfg, &settings->spdif->desired,
            sizeof(settings->spdif->cfg)));
#endif

    BDBG_OBJECT_ASSERT(display, bdisplay);

    /* must remove before changing format, otherwise nexus will auto-remove */
    if (settings->composite != display->settings.composite) {
        if (display->settings.composite) {
            rc = NEXUS_Display_RemoveOutput(display->nDisplay, NEXUS_CompositeOutput_GetConnector(display->settings.composite->handle));
            if (rc) BSETTOP_ERROR(berr_external_error); /* keep going */
        }
    }

    if ( settings->svideo != display->settings.svideo )
    {
        if (display->settings.svideo) {
            rc = NEXUS_Display_RemoveOutput(display->nDisplay, NEXUS_SvideoOutput_GetConnector(display->settings.svideo->handle));
            if (rc) BSETTOP_ERROR(berr_external_error); /* keep going */
        }
    }

    if (settings->component != display->settings.component) {
        if (display->settings.component) {
            rc = NEXUS_Display_RemoveOutput(display->nDisplay, display->settings.component->videoOutput);
            if (rc) BSETTOP_ERROR(berr_external_error); /* keep going */
        }
    }
#if B_N_RF_OUTPUTS
    if (settings->rf != display->settings.rf) {
        if (display->settings.rf) {
            rc = NEXUS_Display_RemoveOutput(display->nDisplay, NEXUS_Rfm_GetVideoConnector(display->settings.rf->handle));
            if (rc) {rc = BSETTOP_ERROR(berr_external_error);}

            b_set_rfm_audio_source(display->settings.rf, NULL);
        }
    }
#endif

    format = b_displayformat2nexus(settings->format, &settings->vesa_settings);

#if B_N_DVI_OUTPUTS
    /* DVI_VALIDATE_EDID_FORMAT=1 */
    if (settings->dvi && settings->dvi->desired.validate_edid_format) {
        if ( settings->dvi->connected && settings->dvi->handle) {
            rc = NEXUS_HdmiOutput_GetStatus(settings->dvi->handle, &nHdmiOutputStatus);
            if (rc) return BSETTOP_ERROR(berr_external_error);

            if (!nHdmiOutputStatus.videoFormatSupported[format]) {
                BDBG_ERR(("Attached HDMI receiver does not support the requested format"));
                return BSETTOP_ERROR(berr_not_supported);
            }
        }
    }
#endif
    NEXUS_Display_GetSettings(display->nDisplay, &nSettings);
    prevFormat = nSettings.format;
    nSettings.format = format;

#if MACROVISION_SUPPORT
    if(prevFormat != nSettings.format)
    {
        rc = NEXUS_Display_SetMacrovision(display->nDisplay, bmacrovision_type_none, NULL);
        if ( rc ) return BSETTOP_ERROR(berr_external_error);
    }
#endif
    /* for HD (not including 480p/576p), force to 16x9. Nexus allows for 4x3 HD, but settop api does not. */
    if (b_is_hd(settings->format) && settings->format != bvideo_format_480p && settings->format != bvideo_format_576p) {
        nSettings.aspectRatio = NEXUS_DisplayAspectRatio_e16x9;
    }
    else {
        nSettings.aspectRatio = b_displayaspectratio2nexus(settings->aspect_ratio);
    }
    nSettings.background = settings->background_color;
    rc = NEXUS_Display_SetSettings(display->nDisplay, &nSettings);
    if (rc) return BSETTOP_ERROR(berr_external_error);

    if (prevFormat != nSettings.format && display->graphics) {
        bgraphics_settings graphics_settings;

        NEXUS_VideoFormat_GetInfo(format, &videoFormatInfo);
        bgraphics_get(display->graphics, &graphics_settings);
        graphics_settings.destination_height = videoFormatInfo.height;
        graphics_settings.destination_width = videoFormatInfo.width;
        result = bgraphics_set(display->graphics, &graphics_settings);
        if (result) return result;
    }

    if (settings->content_mode != display->settings.content_mode) {
        /* only apply content mode to main window. PIP should stay Full */
        NEXUS_VideoWindowHandle nWindow = display->window[0].nWindow;
        if (nWindow) {
            NEXUS_VideoWindowSettings nWindowSettings;
            BDBG_CASSERT(bdisplay_content_mode_full_nonlinear == (bdisplay_content_mode)NEXUS_VideoWindowContentMode_eFullNonLinear);

            NEXUS_VideoWindow_GetSettings(nWindow, &nWindowSettings);
            nWindowSettings.contentMode = settings->content_mode;
            rc = NEXUS_VideoWindow_SetSettings(nWindow, &nWindowSettings);
            if (rc) return BSETTOP_ERROR(berr_external_error);

        }
    }

    if (settings->composite != display->settings.composite) {
        if (settings->composite) {
            rc = NEXUS_Display_AddOutput(display->nDisplay, NEXUS_CompositeOutput_GetConnector(settings->composite->handle));
            if (rc) return BSETTOP_ERROR(berr_external_error);
        }
    }

    if (settings->composite2 != display->settings.composite2) {
        if (settings->composite2) {
            rc = NEXUS_Display_AddOutput(display->nDisplay, NEXUS_CompositeOutput_GetConnector(settings->composite2->handle));
            if (rc) return BSETTOP_ERROR(berr_external_error);
        }
    }
    if (settings->svideo != display->settings.svideo) {
        if (settings->svideo) {
            rc = NEXUS_Display_AddOutput(display->nDisplay, NEXUS_SvideoOutput_GetConnector(settings->svideo->handle));
            if (rc) return BSETTOP_ERROR(berr_external_error);
        }
    }
    if (settings->component != display->settings.component) {
        if (settings->component) {
            rc = NEXUS_Display_AddOutput(display->nDisplay, settings->component->videoOutput);
            if (rc) return BSETTOP_ERROR(berr_external_error);
        }
    }
#if B_N_RF_OUTPUTS
    if (settings->rf && change_rf) {
        NEXUS_RfmSettings rfmSettings;

        BDBG_MSG(("setting rf"));
        NEXUS_Rfm_GetSettings(settings->rf->handle, &rfmSettings);
        rfmSettings.channel = settings->rf->desired.channel;
        /* settings.country is ignored. Nexus RFM follows the format of the SD VEC. */
        rc = NEXUS_Rfm_SetSettings(settings->rf->handle, &rfmSettings);
        if (rc) return BSETTOP_ERROR(berr_external_error);

        if (!display->settings.rf) {
            int i;
            bdisplay_t audio_display = display;

            rc = NEXUS_Display_AddOutput(display->nDisplay, NEXUS_Rfm_GetVideoConnector(settings->rf->handle));
            if (rc) return BSETTOP_ERROR(berr_external_error);

            /* Look for a cloned window */
            for ( i = 0; i < B_N_WINDOWS; i++ )
            {
                if ( display->window[i].parent )
                {
                    audio_display = display->window[i].parent->display;
                    break;
                }
            }

            rc = b_set_rfm_audio_source(settings->rf, audio_display);
            if (rc) return BSETTOP_ERROR(berr_external_error);
        }
        /* the NEXUS_Display_RemoveOutput code is before any possible display format change */
    }
#endif


#if B_N_DVI_OUTPUTS
/* Format Changes must be done to Display not HDMI */
     if (change_dvi)
     {
        BDBG_MSG(("changing hdmi %p %d",settings->dvi, settings->dvi?settings->dvi->connected:false));
        if ( settings->dvi && !settings->dvi->connected )
        {
            NEXUS_HdmiOutputStatus nHdmiStatus;
            NEXUS_HdmiOutput_GetStatus(settings->dvi->handle, &nHdmiStatus);
            if ( nHdmiStatus.connected )
            {
                /* Attaching HDMI */
                BDBG_MSG(("Attaching HDMI"));
                settings->dvi->display = display;
                bdisplay_p_connect_hdmi(display, settings->dvi);
            }
            else
            {
                BDBG_MSG(("hdmi not connected"));
            }
        }
        else if ( display->settings.dvi && !settings->dvi && display->settings.dvi->connected )
        {
            /* Removing HDMI */
            BDBG_MSG(("Detaching HDMI"));
            bdisplay_p_disconnect_hdmi(display, display->settings.dvi);
        }
        if ( settings->dvi && settings->dvi->connected )
        {
            BDBG_MSG(("HDMI Settings Changed"));
            /* Settings Changed */
            if ( settings->dvi->desired.hdmi_audio_mode != settings->dvi->cfg.hdmi_audio_mode
#if B_HAS_DTS_ENCODE || B_HAS_AC3_ENCODE
                 || settings->dvi->desired.compressed_audio_format != settings->dvi->cfg.compressed_audio_format
#endif
                )
            {
                /* Audio mode change */
                bdisplay_p_enable_audio(display, false);
                NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(settings->dvi->handle));

                if ( settings->dvi->desired.hdmi_audio_mode == boutput_hdmi_audio_mode_pcm )
                {
                    BDBG_MSG(("Attaching HDMI to mixer"));
                    NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(settings->dvi->handle),
                                               NEXUS_AudioMixer_GetConnector(display->nAudioMixer));
                }
                else
                {
                    BDBG_MSG(("HDMI audio not attached to mixer,audio cound be compressed or multichannel %d",settings->dvi->desired.hdmi_audio_mode));
                }
                bdisplay_p_enable_audio(display, true);
            }

            settings->dvi->cfg = settings->dvi->desired;
        }
     }
#endif /* B_N_DVI_OUTPUTS */


#if B_N_SPDIF_OUTPUTS
    if ( change_spdif )
    {
        BDBG_MSG(("changing spdif"));
        /* IMPORTANT: Do not bail out while this is diabled! */
        bdisplay_p_enable_audio(display, false);
        rc = 0;
        if ( display->settings.spdif )
        {
            /* Remove SPDIF from any existing connection */
            NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(display->settings.spdif->handle));
            display->settings.spdif->display = NULL;

            if ( settings->spdif )
            {
                /* Add to mixer only if set to PCM mode -- otherwise enabling audio will make the compressed connection */
                if ( settings->spdif->desired.pcm )
                {
                    rc = NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(settings->spdif->handle),
                                                    NEXUS_AudioMixer_GetConnector(display->nAudioMixer));
                }
                settings->spdif->cfg = settings->spdif->desired;
                display->settings.spdif->display = display;
            }
            bdisplay_p_enable_audio(display, true);
            if ( rc ) return BSETTOP_ERROR(berr_external_error);
        }
    }
#endif
    if(settings->component && settings->component->handle)
    {
        NEXUS_ComponentOutput_GetSettings(settings->component->handle,&nComponentOutputSettings);
        nComponentOutputSettings.mpaaDecimationEnabled = settings->mpaa_enabled;
        nComponentOutputSettings.type = settings->component->desired.type == boutput_component_type_rgb ? NEXUS_ComponentOutputType_eRGB : NEXUS_ComponentOutputType_eYPrPb;
        rc = NEXUS_ComponentOutput_SetSettings(settings->component->handle,&nComponentOutputSettings);
        if ( rc ) return BSETTOP_ERROR(berr_external_error);
    }
#if B_N_DVI_OUTPUTS
    if(settings->dvi)
    {
        NEXUS_HdmiOutput_GetSettings(settings->dvi->handle,&nHdmiOutputSettings);
        if (settings->component) {
            nHdmiOutputSettings.autoColorSpace = false;
            nHdmiOutputSettings.colorSpace = settings->component->desired.type == boutput_component_type_rgb ? NEXUS_ColorSpace_eRgb : NEXUS_ColorSpace_eYCbCr444;
        }
        else {
            nHdmiOutputSettings.autoColorSpace = true;
        }
        rc = NEXUS_HdmiOutput_SetSettings(settings->dvi->handle,&nHdmiOutputSettings);
        if ( rc ) return BSETTOP_ERROR(berr_external_error);
    }
#endif

#if MACROVISION_SUPPORT
    if(settings->format < bvideo_format_secam) {
        if((display->settings.macrovision_type != settings->macrovision_type) || (prevFormat != nSettings.format))
        {
            if(settings->macrovision_type == bmacrovision_type_custom && settings->macrovision_tables)
            {
                rc = NEXUS_Display_SetMacrovision(display->nDisplay,settings->macrovision_type,(NEXUS_DisplayMacrovisionTables*)&settings->macrovision_tables);
                if ( rc ) BSETTOP_ERROR(berr_external_error);
            }
            else
            {
                rc = NEXUS_Display_SetMacrovision(display->nDisplay,settings->macrovision_type,NULL);
                if ( rc ) BSETTOP_ERROR(berr_external_error);
            }
            NEXUS_Display_GetVbiSettings(display->nDisplay,&nDisplayVbiSettings);
            nDisplayVbiSettings.macrovisionEnabled =  (settings->macrovision_type==bmacrovision_type_none)?0:1;
            rc = NEXUS_Display_SetVbiSettings(display->nDisplay,&nDisplayVbiSettings);
            if ( rc ) BSETTOP_ERROR(berr_external_error);
            display->settings.macrovision_type = settings->macrovision_type;
        }
    }
    else {
        NEXUS_Display_GetVbiSettings(display->nDisplay,&nDisplayVbiSettings);
        nDisplayVbiSettings.macrovisionEnabled =  false;
        rc = NEXUS_Display_SetVbiSettings(display->nDisplay,&nDisplayVbiSettings);
        if ( rc ) BSETTOP_ERROR(berr_external_error);
    }

#endif

#if DCS_SUPPORT
    if(settings->dcs_type != display->settings.dcs_type)
    {
        NEXUS_DisplayVbiSettings nDisplayVbiSettings;
        rc = NEXUS_Display_SetDcs(display->nDisplay,settings->dcs_type);
        if ( rc ) BSETTOP_ERROR(berr_external_error);

        NEXUS_Display_GetVbiSettings(display->nDisplay,&nDisplayVbiSettings);
        nDisplayVbiSettings.dcsEnabled =  (settings->dcs_type==bdcs_type_dcs_off)?0:1;
        rc = NEXUS_Display_SetVbiSettings(display->nDisplay,&nDisplayVbiSettings);
        if ( rc ) BSETTOP_ERROR(berr_external_error);
    }
#endif

    display->settings = *settings;
    /* Potentially refresh HDMI volume */
    if ( display->settings.spdif )
    {
        boutput_spdif_set_audio_volume(display->settings.spdif, &display->settings.spdif->volume);
    }
    return 0;
}

void boutput_rf_get(boutput_rf_t rf, boutput_rf_settings *settings)
{
    *settings = rf->desired;
}

bresult boutput_hdmi_set(boutput_hdmi_t hdmi, const boutput_hdmi_settings *settings)
{
#if B_N_DVI_OUTPUTS
    NEXUS_Error errCode;
    bvideo_format preferredFormat;

    /* Don't overwrite preferred format */
    preferredFormat = hdmi->desired.edid_preferred_format;

    hdmi->desired = *settings;
    hdmi->desired.edid_preferred_format = preferredFormat;

#if NEXUS_HAS_SECURITY
    if ( settings->hdcp )
    {
        NEXUS_HdmiOutputHdcpStatus hdcpStatus;
        NEXUS_HdmiOutput_GetHdcpStatus(hdmi->handle, &hdcpStatus);

        /* Re-authenticate if link is not yet authenticated */
        if (!hdcpStatus.transmittingEncrypted)
        {
            NEXUS_HdmiOutputHdcpSettings *pHdcpSettings;
            pHdcpSettings = BKNI_Malloc(sizeof(*pHdcpSettings));
            NEXUS_HdmiOutput_GetHdcpSettings(hdmi->handle, pHdcpSettings);
            if ( pHdcpSettings )
            {
                pHdcpSettings->pjCheckEnabled = settings->hdcp_pj_checking_enable;
                errCode = NEXUS_HdmiOutput_SetHdcpSettings(hdmi->handle, pHdcpSettings);
                BKNI_Free(pHdcpSettings);
                if ( errCode )
                {
                    return BSETTOP_ERROR(berr_external_error);
                }
                /* Start HDCP if connected */
                if ( hdmi->connected )
                {
                    errCode = NEXUS_HdmiOutput_StartHdcpAuthentication(hdmi->handle);
                    if ( errCode )
                    {
                        return BSETTOP_ERROR(berr_external_error);
                    }
                }
            }
            else
            {
                return BSETTOP_ERROR(berr_external_error);
            }
        }
    }
    else
    {
        errCode = NEXUS_HdmiOutput_DisableHdcpAuthentication(hdmi->handle);
        if ( errCode )
        {
            return BSETTOP_ERROR(berr_external_error);
        }
    }
#endif

    /* Check for audio settings change */
    if ( settings->audio_delay != hdmi->cfg.audio_delay )
    {
        NEXUS_AudioOutputSettings audioSettings;
        NEXUS_AudioOutput audioOutput;

        audioOutput = NEXUS_HdmiOutput_GetAudioConnector(hdmi->handle);
        NEXUS_AudioOutput_GetSettings(audioOutput, &audioSettings);
        audioSettings.additionalDelay = settings->audio_delay;
        errCode = NEXUS_AudioOutput_SetSettings(audioOutput, &audioSettings);
        if ( errCode )
        {
            return BSETTOP_ERROR(berr_external_error);
        }
    }

    if ( settings->preemphasis_support != hdmi->cfg.preemphasis_support ||
         settings->hdmi_avmute_delay_pre_format_change != hdmi->cfg.hdmi_avmute_delay_pre_format_change ||
         settings->hdmi_avmute_delay_post_format_change != hdmi->cfg.hdmi_avmute_delay_post_format_change ||
         settings->hdmi_color_depth != hdmi->cfg.hdmi_color_depth ||
         !BKNI_Memcmp(&settings->hdmi_spdif_channel_status, &hdmi->cfg.hdmi_spdif_channel_status, sizeof(boutput_spdif_channel_status)) )
    {
        NEXUS_HdmiOutputSettings hdmiSettings;
        NEXUS_HdmiOutput_GetSettings(hdmi->handle, &hdmiSettings);
        hdmiSettings.preFormatChangeAvMuteDelay = settings->hdmi_avmute_delay_pre_format_change;
        hdmiSettings.postFormatChangeAvMuteDelay = settings->hdmi_avmute_delay_post_format_change;
        hdmiSettings.preemphasisEnabled = settings->preemphasis_support;
        hdmiSettings.colorDepth = settings->hdmi_color_depth;
        hdmiSettings.audioChannelStatusInfo.categoryCode = settings->hdmi_spdif_channel_status.category_code;
        hdmiSettings.audioChannelStatusInfo.clockAccuracy = settings->hdmi_spdif_channel_status.clock_accuracy;
        hdmiSettings.audioChannelStatusInfo.professionalMode = settings->hdmi_spdif_channel_status.professional_mode;
        hdmiSettings.audioChannelStatusInfo.swCopyRight = settings->hdmi_spdif_channel_status.copyright;
        errCode = NEXUS_HdmiOutput_SetSettings(hdmi->handle, &hdmiSettings);
        if ( errCode )
        {
            return BSETTOP_ERROR(berr_external_error);
        }
    }

#else
    BSTD_UNUSED(hdmi);
    BSTD_UNUSED(settings);
#endif

    return b_ok;
}

void boutput_hdmi_get(boutput_hdmi_t hdmi, boutput_hdmi_settings *settings)
{
    *settings = hdmi->desired;
}

bresult boutput_hdmi_get_capabilities(boutput_hdmi_t dvi, boutput_hdmi_capabilities *caps)
{
#if B_N_DVI_OUTPUTS > 0
    NEXUS_HdmiOutputStatus *pHdmiStatus;
    int i;

    BDBG_MSG(("HDMI Get Capabilities"));

    pHdmiStatus = BKNI_Malloc(sizeof(NEXUS_HdmiOutputStatus)); 

    NEXUS_HdmiOutput_GetStatus(dvi->handle, pHdmiStatus);
    if(!pHdmiStatus->connected)
    {
    	BKNI_Free(pHdmiStatus);
        return berr_not_available;  /* Soft error -- does not print */
    }

    if (bsettop_get_config("hdmi_bypass_edid"))
    {
        BDBG_MSG(("bypass edid "));
		BKNI_Free(pHdmiStatus);
        return berr_not_available;
    }

    caps->hdmi = pHdmiStatus->hdmiDevice;
    caps->preferred_video_format = b_nexus2displayformat(pHdmiStatus->preferredVideoFormat, &dvi->desired.vesa_settings);

    /* Get all supported video formats */
    BKNI_Memset(caps->video_format_is_supported, 0, sizeof(caps->video_format_is_supported));
    for(i=0;i<NEXUS_VideoFormat_eMax;i++)
    {
        if(pHdmiStatus->videoFormatSupported[i])
            caps->video_format_is_supported[b_nexus2displayformat(i, NULL)]=true;
    }

    /* Get all supported audio formats (hdmi only) */
    BKNI_Memset(caps->audio_mode_is_supported, 0, sizeof(caps->audio_mode_is_supported));
    if(caps->hdmi)
    {
        caps->audio_mode_is_supported[boutput_hdmi_audio_mode_pcm] = pHdmiStatus->maxAudioPcmChannels?1:0;
        if(pHdmiStatus->maxAudioPcmChannels >= 6)
            caps->audio_mode_is_supported[boutput_hdmi_audio_mode_pcm_6ch] = true;

        /* For compressed, use AC3 to indicate */
        if(pHdmiStatus->audioCodecSupported[NEXUS_AudioCodec_eAc3])
            caps->audio_mode_is_supported[boutput_hdmi_audio_mode_compressed] = true;
        if(pHdmiStatus->audioCodecSupported[NEXUS_AudioCodec_eAc3Plus])
            caps->audio_mode_is_supported[boutput_hdmi_audio_mode_ac3_plus] = true;

        /* For DTS */
        if(pHdmiStatus->audioCodecSupported[NEXUS_AudioCodec_eDts])
            caps->audio_mode_is_supported[boutput_hdmi_audio_mode_dts] = true;
        if(pHdmiStatus->audioCodecSupported[NEXUS_AudioCodec_eDtsHd])
            caps->audio_mode_is_supported[boutput_hdmi_audio_mode_dts_hd] = true;
        if(pHdmiStatus->audioCodecSupported[NEXUS_AudioCodec_eDtsLegacy])
            caps->audio_mode_is_supported[boutput_hdmi_audio_mode_dts_legacy] = true;

        /* HDMI Rx EDID Info */
        caps->rx_edid_info.phys_addr_A = pHdmiStatus->physicalAddressA;
        caps->rx_edid_info.phys_addr_B = pHdmiStatus->physicalAddressB;
        caps->rx_edid_info.phys_addr_C = pHdmiStatus->physicalAddressC;
        caps->rx_edid_info.phys_addr_D = pHdmiStatus->physicalAddressD;
    }

    /* Priority order for preferred audio format */
    if ( caps->audio_mode_is_supported[boutput_hdmi_audio_mode_pcm_6ch] )
        caps->preferred_audio_mode = boutput_hdmi_audio_mode_pcm_6ch;
    else if ( caps->audio_mode_is_supported[boutput_hdmi_audio_mode_compressed] )
        caps->preferred_audio_mode = boutput_hdmi_audio_mode_compressed;
    else
        caps->preferred_audio_mode = boutput_hdmi_audio_mode_pcm;

    /* Free memory */
    BKNI_Free(pHdmiStatus);
    return b_ok;
#else
    BSTD_UNUSED(dvi);
    BSTD_UNUSED(caps);
    return BSETTOP_ERROR(berr_not_supported);
#endif

}

bresult boutput_hdmi_get_status(boutput_hdmi_t dvi, boutput_hdmi_status *status)
{
    BDBG_ASSERT(NULL != status);

    /* Return status as disabled by default */
    status->hdcp_state = boutput_hdmi_hdcp_state_disabled;

    #if B_N_DVI_OUTPUTS
    if ( dvi->desired.hdcp )
    {
        NEXUS_HdmiOutputHdcpStatus hdcpStatus;

        NEXUS_HdmiOutput_GetHdcpStatus(dvi->handle, &hdcpStatus);
        switch ( hdcpStatus.hdcpState )
        {
        case NEXUS_HdmiOutputHdcpState_eUnpowered:
            status->hdcp_state = boutput_hdmi_hdcp_state_disabled;
            break;
        case NEXUS_HdmiOutputHdcpState_eWaitForValidVideo:
        case NEXUS_HdmiOutputHdcpState_eInitializedAuthentication:
        case NEXUS_HdmiOutputHdcpState_eWaitForReceiverAuthentication:
        case NEXUS_HdmiOutputHdcpState_eReceiverR0Ready:
        case NEXUS_HdmiOutputHdcpState_eReceiverAuthenticated:
        case NEXUS_HdmiOutputHdcpState_eWaitForRepeaterReady:
        case NEXUS_HdmiOutputHdcpState_eCheckForRepeaterReady:
        case NEXUS_HdmiOutputHdcpState_eRepeaterReady:
        case NEXUS_HdmiOutputHdcpState_eLinkAuthenticated:      /* Includes down stream devices */
            status->hdcp_state = boutput_hdmi_hdcp_state_init;
            break;
        case NEXUS_HdmiOutputHdcpState_eEncryptionEnabled:
            status->hdcp_state = boutput_hdmi_hdcp_state_enabled;
            break;
        case NEXUS_HdmiOutputHdcpState_eUnauthenticated:
        case NEXUS_HdmiOutputHdcpState_eR0LinkFailure:
            status->hdcp_state = boutput_hdmi_hdcp_state_auth_fail;
            break;
        case NEXUS_HdmiOutputHdcpState_eRepeaterAuthenticationFailure:
            status->hdcp_state = boutput_hdmi_hdcp_state_repeater_fail;
            break;
        case NEXUS_HdmiOutputHdcpState_eRiLinkIntegrityFailure:
            status->hdcp_state = boutput_hdmi_hdcp_state_ri_fail;
            break;
        case NEXUS_HdmiOutputHdcpState_ePjLinkIntegrityFailure:
            status->hdcp_state = boutput_hdmi_hdcp_state_pj_fail;
            break;
        default:
            status->hdcp_state = boutput_hdmi_hdcp_state_internal_err;
            break;
        }
    }
    #else
    BSTD_UNUSED(dvi);
    #endif

    return b_ok;
}

bresult boutput_component_set(boutput_component_t component, const boutput_component_settings *settings)
{
    component->desired = *settings;
    return 0;
}

void boutput_component_get(boutput_component_t component, boutput_component_settings *settings)
{
    *settings = component->desired;
}

bresult boutput_spdif_set(boutput_spdif_t spdif, const boutput_spdif_settings *settings)
{
    #if B_N_SPDIF_OUTPUTS
    NEXUS_SpdifOutputSettings spdifSettings;
    BDBG_ASSERT(NULL != spdif);
    BDBG_ASSERT(NULL != settings);
    NEXUS_SpdifOutput_GetSettings(spdif->handle, &spdifSettings);
    spdifSettings.channelStatusInfo.categoryCode = settings->spdif_channel_status.category_code;
    spdifSettings.channelStatusInfo.clockAccuracy = settings->spdif_channel_status.clock_accuracy;
    spdifSettings.channelStatusInfo.professionalMode = settings->spdif_channel_status.professional_mode;
    spdifSettings.channelStatusInfo.swCopyRight = settings->spdif_channel_status.copyright;
    if ( NEXUS_SpdifOutput_SetSettings(spdif->handle, &spdifSettings) )
    {
        return BSETTOP_ERROR(berr_external_error);
    }
    spdif->desired = *settings;
    #else
    BSTD_UNUSED(spdif);
    BSTD_UNUSED(settings);
    #endif
    return b_ok;
}

void boutput_spdif_get(boutput_spdif_t spdif, boutput_spdif_settings *settings)
{
    BDBG_ASSERT(NULL != spdif);
    BDBG_ASSERT(NULL != settings);
    *settings = spdif->desired;
}

boutput_spdif_t
boutput_spdif_open(bobject_t id)
{
#if B_N_SPDIF_OUTPUTS
    unsigned index = B_ID_GET_INDEX(id);
    if (index >= B_N_SPDIF_OUTPUTS) {
        BSETTOP_ERROR(berr_not_available);
        return NULL;
    }
    return &g_spdif[index];
#else
    BSTD_UNUSED(id);
    return NULL;
#endif
}

boutput_rf_t
boutput_rf_open(bobject_t id)
{
#if B_N_RF_OUTPUTS
    unsigned index = B_ID_GET_INDEX(id);
    if (index >= B_N_RF_OUTPUTS) {
        BSETTOP_ERROR(berr_not_available);
        return NULL;
    }
    return &g_rf[index];
#else
    BSTD_UNUSED(id);
    return NULL;
#endif
}

boutput_composite_t
boutput_composite_open(bobject_t id)
{
    unsigned index = B_ID_GET_INDEX(id);
#if B_N_COMPOSITE_OUTPUTS
#if BCHP_CHIP==7420 
    if (index == 1 && B_N_COMPOSITE_OUTPUTS == 1)
    {
        BSTD_UNUSED(id);
        return NULL;
    }
#endif
    if (index >= B_N_COMPOSITE_OUTPUTS) {
        BSETTOP_ERROR(berr_not_available);
        return NULL;
    }
    return &g_composite[index];
#else
    BSTD_UNUSED(id);
    return NULL;
#endif
}

boutput_svideo_t
boutput_svideo_open(bobject_t id)
{
#if B_N_SVIDEO_OUTPUTS
    unsigned index = B_ID_GET_INDEX(id);
    if (index >= B_N_SVIDEO_OUTPUTS) {
        BSETTOP_ERROR(berr_not_available);
        return NULL;
    }
    return &g_svideo[index];
#else
    BSTD_UNUSED(id);
    return NULL;
#endif
}

boutput_hdmi_t
boutput_hdmi_open(bobject_t id)
{
#if B_N_DVI_OUTPUTS
    unsigned index = B_ID_GET_INDEX(id);
    if (index >= B_N_DVI_OUTPUTS) {
        BSETTOP_ERROR(berr_not_available);
        return NULL;
    }
    return &g_hdmi[index];
#else
    BSTD_UNUSED(id);
    return NULL;
#endif
}

boutput_component_t
boutput_component_open(bobject_t id)
{
#if B_N_COMPONENT_OUTPUTS
    unsigned index = B_ID_GET_INDEX(id);
    if (index >= B_N_COMPONENT_OUTPUTS) {
        BSETTOP_ERROR(berr_not_available);
        return NULL;
    }
    return &g_component[index];
#else
    BSTD_UNUSED(id);
    return NULL;
#endif
}

void bdisplay_get_video_format_settings(const bdisplay_settings *settings, bvideo_format_settings *format_settings)
{
    /* get format settings from enum or vesa_settings */
    if (settings->format == bvideo_format_vesa) {
        *format_settings = settings->vesa_settings;
    }
    else {
        bvideo_get_format_settings(settings->format, format_settings);
    }
}

void bvideo_get_format_settings(bvideo_format format, bvideo_format_settings *settings)
{
    NEXUS_VideoFormatInfo formatInfo;

    BKNI_Memset(settings, 0, sizeof(*settings));

    if (format == bvideo_format_vesa) {
        BDBG_ERR(("There are multiple options for vesa settings, so you shold use bdisplay_get. "
            "See bdisplay_settings.vesa_settings."));
        /* TODO: This is a bit ugly. Just pick one vesa mode instead of sending 0's.
        The caller should be used bdisplay_get for vesa. */
        settings->width = 640;
        settings->height = 480;
        settings->refresh_rate = 60;
        /* 20061213 bandrews - added to record drop_frame status */
        settings->frame_rate = bvideo_frame_rate_60; /* TODO: correct??? */
        return;
    }

    NEXUS_VideoFormat_GetInfo(b_displayformat2nexus(format, NULL), &formatInfo );
    settings->width = (unsigned) formatInfo.digitalWidth;
    settings->height = (unsigned) formatInfo.digitalHeight;
    settings->interlaced = formatInfo.interlaced;
    settings->refresh_rate = formatInfo.verticalFreq == 5000 ? 50 : 60;
    settings->frame_rate = formatInfo.verticalFreq;
}

/* For settop api defaults which don't match nexus defaults, force them here. */
bresult bdecode_window_p_set_initial_settings(bdecode_window_t window)
{
    NEXUS_VideoWindowSettings nWindowSettings;
    NEXUS_Error rc;

    NEXUS_VideoWindow_GetSettings(window->nWindow, &nWindowSettings);
    if (b_window_is_full_screen(&window->display->settings,
                                &window->settings.position)) {
        nWindowSettings.contentMode = window->display->settings.content_mode; /* CASSERT for this mapping is elsewhere */
    }
    else {
        /* PIP is always full */
        nWindowSettings.contentMode = NEXUS_VideoWindowContentMode_eFull;
    }
    rc = NEXUS_VideoWindow_SetSettings(window->nWindow, &nWindowSettings);
    if (rc) return BSETTOP_ERROR(rc);

    return 0;
}

bdecode_window_t bdecode_window_open(bobject_t window_id, bdisplay_t display)
{
    unsigned index = B_ID_GET_INDEX(window_id);
    bdecode_window_t window;

    if (index >= display->numWindowsSupported) {
        BSETTOP_ERROR(berr_not_available);
        return NULL;
    }
    window = &display->window[index];
    if (window->nWindow) {
        BSETTOP_ERROR(berr_not_available);
        return NULL;
    }

    window->nWindow = NEXUS_VideoWindow_Open(display->nDisplay, index);
    if (!window->nWindow) {
        BSETTOP_ERROR(berr_not_available);
        return NULL;
    }

#if ( (BCHP_CHIP == 7420) || (BCHP_CHIP == 7422) || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7435) || (BCHP_CHIP == 7231) || \
      (BCHP_CHIP == 7346) || (BCHP_CHIP == 7344) || (BCHP_CHIP == 7429) || (BCHP_CHIP == 7584))
    /* Set Mad Owner to be the main window on main display */
    if (index == 0)
    {
        gWindowMadOwner = window;
    }
#endif

    /* do not apply setting as we are not ready */
    NEXUS_DisplayModule_SetUpdateMode(NEXUS_DisplayUpdateMode_eManual);
    bsettop_p_display_zorder_add(display,window,window->settings.zorder);
    NEXUS_DisplayModule_SetUpdateMode(NEXUS_DisplayUpdateMode_eAuto);

    bdecode_window_p_set_initial_settings(window);

    return window;
}

bdecode_window_t bdecode_window_open_mosaic(bdecode_window_t parent_window, bobject_t mosaic_id)
{
    /* TODO */
    BSTD_UNUSED(parent_window);
    BSTD_UNUSED(mosaic_id);
    BSETTOP_ERROR(berr_not_supported);
    return NULL;
}

void
bdecode_window_close(bdecode_window_t window)
{
    bsettop_p_display_zorder_remove( window->display,window);

    if ( window->nWindow ) {
        NEXUS_VideoWindow_Close(window->nWindow);
        window->nWindow = NULL;
    }
    if (window->clone) {
        BDBG_ERR(("must close the clone before closing the parent"));
        /* make the best of it. */
        window->clone->parent = NULL;
        window->clone = NULL;
    }
    if (window->parent) {
        window->parent->clone = NULL;
        window->parent = NULL;
#if B_N_RF_OUTPUTS
        if ( window->display->settings.rf )
        {
            int i;

            for ( i = 0; i < B_N_WINDOWS; i++ )
            {
                if ( window->display->window[i].parent )
                {
                    return;
                }
            }

            /* Move RFM back to main window */
            b_set_rfm_audio_source(window->display->settings.rf, window->display);
        }
#endif
    }
}

bdecode_window_t bdecode_window_clone(bdecode_window_t parent_window, bobject_t clone_window_id, bdisplay_t display)
{
    unsigned index = B_ID_GET_INDEX(clone_window_id);
    bdecode_window_t window;

    if (index >= display->numWindowsSupported * g_numDisplays) {
        BSETTOP_ERROR(berr_not_available);
        return NULL;
    }
    index -= display->index * display->numWindowsSupported;
    window = &display->window[index];
    if (window->nWindow) {
        BSETTOP_ERROR(berr_not_available);
        return NULL;
    }

    window->nWindow = NEXUS_VideoWindow_Open(display->nDisplay, index);
    if (!window->nWindow) {
        BSETTOP_ERROR(berr_not_available);
        return NULL;
    }
    window->parent = parent_window;
    parent_window->clone = window;

    /* do not apply setting at this time */
    NEXUS_DisplayModule_SetUpdateMode(NEXUS_DisplayUpdateMode_eManual);
    bsettop_p_display_zorder_add(display,window,window->settings.zorder);
    NEXUS_DisplayModule_SetUpdateMode(NEXUS_DisplayUpdateMode_eAuto);

    window->settings.cloned = true;

    bdecode_window_p_set_initial_settings(window);

#if B_N_RF_OUTPUTS
    /* If we're cloning, we need to make sure audio for the RFM comes from the primary display's mixer */
    if ( display->settings.rf )
    {
        b_set_rfm_audio_source(display->settings.rf, parent_window->display);
    }
#endif

    return window;
}

bresult bdisplay_set_dac_audio_volume(bdisplay_t display, const baudio_volume *volume)
{
    bresult rc;
    if (display->audioDac.handle) {
        BDBG_MSG(("Setting DAC volume"));
        rc = bdisplay_p_set_output_volume(NEXUS_AudioDac_GetConnector(display->audioDac.handle), volume, &display->volume, false);
        if ( rc ) { return BSETTOP_ERROR(rc); }
    }
    else
    {
        BDBG_MSG(("Not Setting DAC volume -- No DAC on this display"));
    }

    return b_ok;
}

bresult boutput_rf_set_audio_volume(boutput_rf_t rf, const baudio_volume *volume)
{
#if B_N_RF_OUTPUTS
    NEXUS_RfmSettings rfmSettings;
    NEXUS_Error rc;

    NEXUS_Rfm_GetSettings(rf->handle, &rfmSettings);
    rfmSettings.muted = volume->muted;
    /* volume level is ignored. it is set with the dac. */
    rc = NEXUS_Rfm_SetSettings(rf->handle, &rfmSettings);
    if (rc) return BSETTOP_ERROR(berr_external_error);
    return 0;
#else
    BSTD_UNUSED(rf);
    BSTD_UNUSED(volume);
    return BSETTOP_ERROR(berr_not_supported);
#endif
}

bresult boutput_rf_get_audio_volume(boutput_rf_t rf, baudio_volume *volume)
{
#if B_N_RF_OUTPUTS
    NEXUS_RfmSettings rfmSettings;
    NEXUS_Rfm_GetSettings(rf->handle, &rfmSettings);
    volume->muted = rfmSettings.muted;
    volume->left = volume->right = 0; /* unused */
    return 0;
#else
    BSTD_UNUSED(rf);
    BSTD_UNUSED(volume);
    return BSETTOP_ERROR(berr_not_supported);
#endif
}

bresult boutput_spdif_set_audio_volume(boutput_spdif_t spdif, const baudio_volume *volume)
{
    #if B_N_SPDIF_OUTPUTS || B_N_DVI_OUTPUTS
    bresult rc;
    bool force_mute;
    #endif
    #if B_N_SPDIF_OUTPUTS
    force_mute = (false == spdif->cfg.pcm && spdif->compressed_mute)?true:false;    /* If in compressed mode and decoder wants a mute, mute. */
    rc = bdisplay_p_set_output_volume(NEXUS_SpdifOutput_GetConnector(spdif->handle), volume, &spdif->volume, force_mute);
    if ( rc ) return BSETTOP_ERROR(rc);
    #endif
    #if B_N_DVI_OUTPUTS
    BDBG_MSG(("Checking HDMI display %p dvi %p", spdif->display, (spdif->display)?spdif->display->settings.dvi:NULL));
    if ( spdif->display && spdif->display->settings.dvi )
    {
        boutput_hdmi_t hdmi = spdif->display->settings.dvi;
        NEXUS_HdmiOutputHdcpStatus hdcpStatus;

        force_mute = (spdif->compressed_mute && hdmi->cfg.hdmi_audio_mode == boutput_hdmi_audio_mode_compressed)?true:false;

        if ( hdmi->desired.hdcp && !hdmi->desired.hdcp_disable_blue_screen)
        {
            NEXUS_HdmiOutput_GetHdcpStatus(hdmi->handle, &hdcpStatus);
            BDBG_MSG(("hdcpStatus=%d (left=%d right=%d mute=%d)", hdcpStatus.hdcpState, volume->left, volume->right, volume->muted));

            switch ( hdcpStatus.hdcpState )
            {
            case NEXUS_HdmiOutputHdcpState_eLinkAuthenticated:
            case NEXUS_HdmiOutputHdcpState_eEncryptionEnabled:
                BDBG_MSG(("HDCP Succeeded. Setting HDMI volume "));
                rc = bdisplay_p_set_output_volume(NEXUS_HdmiOutput_GetAudioConnector(spdif->display->settings.dvi->handle), volume, &spdif->volume, force_mute);
                if ( rc ) return BSETTOP_ERROR(rc);
                break;

            default:
                /* leave audio intact - muted */
                break;
            }
        }
        else
        {
            BDBG_MSG(("Setting HDMI volume"));
            rc = bdisplay_p_set_output_volume(NEXUS_HdmiOutput_GetAudioConnector(spdif->display->settings.dvi->handle), volume, &spdif->volume, force_mute);
            if ( rc ) return BSETTOP_ERROR(rc);
        }
    }
    #endif
    return b_ok;
}

bresult bdisplay_get_dac_audio_volume(bdisplay_t display, baudio_volume *volume)
{
    *volume = display->volume;
    return b_ok;
}

bresult boutput_spdif_get_audio_volume(boutput_spdif_t spdif, baudio_volume *volume)
{
    *volume = spdif->volume;
    return b_ok;
}

void bdisplay_p_enable_audio(bdisplay_t display, bool enabled)
{
    unsigned i;

    B_LOCK_ASSERT();

    /* Handle recursion. */
    if ( enabled )
    {
        BDBG_ASSERT(display->audioDisableCount > 0);
        display->audioDisableCount--;
        if ( display->audioDisableCount > 0 )
        {
            return;
        }
    }
    else
    {
        display->audioDisableCount++;
        if ( display->audioDisableCount > 1 )
        {
            return;
        }
    }

    /* Disable any pcm playback operations -- must do this before decode. */
    bpcm_play_p_enable(display, enabled);

    /* Loop through windows with decode active and enable/disable audio decoder */
    for (i = 0; i < display->numWindowsSupported; i++)
    {
        bdecode_window_t window = &display->window[i];
        if ( window && window->decode )
        {
            /* Found window with active audio decode */
            BDBG_MSG(("setting audio decode %p enable state to %d", window->decode, enabled));
            bdecode_p_enable_audio(window->decode, enabled);
        }
    }
}

int b_volume2nexus(int settop_vol, bool decibel, int *out_volume)
{
    BDBG_ASSERT(NULL != out_volume);
    if (settop_vol > BAUDIO_LEVEL_MAX) settop_vol = BAUDIO_LEVEL_MAX;
    if (settop_vol < BAUDIO_LEVEL_MIN) settop_vol = BAUDIO_LEVEL_MIN;
    *out_volume = settop_vol;
    if (decibel) {
        /* TODO: when AudioMixer supports linear, get rid of the decibel option in this function */
        /* Volume = bvol * nmax / bmin */
        settop_vol = BAUDIO_LEVEL_MAX-settop_vol;   /* DB is an inverted scale - higher numbers = smaller volume */
        return settop_vol * NEXUS_AUDIO_VOLUME_DB_MIN / BAUDIO_LEVEL_MAX;
    }
    else {
        return ((settop_vol + NEXUS_AUDIO_VOLUME_LINEAR_MIN - BAUDIO_LEVEL_MIN) * (NEXUS_AUDIO_VOLUME_LINEAR_NORMAL - NEXUS_AUDIO_VOLUME_LINEAR_MIN)) / (BAUDIO_LEVEL_MAX - BAUDIO_LEVEL_MIN);
    }
}

/* This is identical for all outputs except RFM */
static bresult bdisplay_p_set_output_volume(NEXUS_AudioOutput output, const baudio_volume *volume, baudio_volume *out_volume, bool force_mute)
{
    NEXUS_Error rc;
    NEXUS_AudioOutputSettings nSettings;

    BDBG_ASSERT(NULL != output);
    BDBG_ASSERT(NULL != volume);
    BDBG_ASSERT(NULL != out_volume);

    *out_volume = *volume;

    NEXUS_AudioOutput_GetSettings(output, &nSettings);
    nSettings.volumeType = NEXUS_AudioVolumeType_eLinear;
    nSettings.leftVolume = b_volume2nexus(volume->left, false, &out_volume->left);
    nSettings.rightVolume = b_volume2nexus(volume->right, false, &out_volume->right);
    BDBG_MSG(("set volume %d -> %d", volume->left, nSettings.leftVolume));
    nSettings.muted = volume->muted || force_mute;
    rc = NEXUS_AudioOutput_SetSettings(output, &nSettings);
    if (rc) return BSETTOP_ERROR(berr_external_error);

    return b_ok;
}


bresult boutput_hdmi_cec_get_message(boutput_hdmi_t dvi, boutput_hdmi_cec_message_data *cec_message_data )
{
    bresult src = b_ok;

#if B_N_DVI_OUTPUTS > 0 && NEXUS_HAS_CEC
    NEXUS_Error rc = NEXUS_SUCCESS;

    /* zero out the cec message data structure before proceeding */
    BKNI_Memset(cec_message_data, 0, sizeof(boutput_hdmi_cec_message_data));

    if (!dvi || !dvi->connected)
        return(b_ok);       /* no DVI/HDMI device connected */

    /* RECEIVED Messages/Status */
    if (dvi->messageReceived)
    {
        NEXUS_CecReceivedMessage stRecvCecMsg;
        if ((rc = NEXUS_Cec_ReceiveMessage(dvi->cecHandle, &stRecvCecMsg))
            != NEXUS_SUCCESS)
        {
            src = berr_external_error;
            goto done;
        }

        /* parse CEC message data */
        cec_message_data->message_type = boutput_hdmi_cec_message_type_receive;
        cec_message_data->initiator_addr = stRecvCecMsg.data.initiatorAddr;
        cec_message_data->destination_addr = stRecvCecMsg.data.destinationAddr;
        cec_message_data->opcode = stRecvCecMsg.data.buffer[0];
        cec_message_data->message_length = stRecvCecMsg.data.length;
        BKNI_Memcpy(&cec_message_data->message_buffer, &stRecvCecMsg.data.buffer,
            (sizeof(uint8_t) * stRecvCecMsg.data.length));
    }

    /* TRANSMITTED Message Status */
    else
    {
        cec_message_data->message_type = boutput_hdmi_cec_message_type_transmit;

        BDBG_MSG(("HDMI CEC Xmit Msg done")) ;
        /* process accordingly */
    }

done:
#else
    BSTD_UNUSED(dvi);
    BSTD_UNUSED(cec_message_data);
#endif

    return src;
}


bresult boutput_hdmi_cec_send_message(boutput_hdmi_t dvi, const boutput_hdmi_cec_message_data *cec_message_data )
{
    bresult src = b_ok;

#if B_N_DVI_OUTPUTS > 0 && NEXUS_HAS_CEC
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_CecMessage stXmitCecMsg;

    if (!dvi || !dvi->connected)
        return(b_ok);       /* no DVI/HDMI device connected */

    /* Copy CEC transmit message data */
    stXmitCecMsg.destinationAddr = cec_message_data->destination_addr;
    stXmitCecMsg.length = cec_message_data->message_length;
    BKNI_Memcpy(&stXmitCecMsg.buffer, &cec_message_data->message_buffer,
        (sizeof(uint8_t) * cec_message_data->message_length));

    rc = NEXUS_Cec_TransmitMessage(dvi->cecHandle, &stXmitCecMsg);
    if (rc != NEXUS_SUCCESS)
    {
        BDBG_WRN(("Error sending CEC message"));
        src = BSETTOP_ERROR(rc);
    }

#else
    BSTD_UNUSED(dvi);
    BSTD_UNUSED(cec_message_data);
#endif

    return src;
}


bresult boutput_hdmi_cec_get_configuration(boutput_hdmi_t dvi, boutput_hdmi_cec_configuration *cec_configuration )
{
    bresult src = b_ok;

#if B_N_DVI_OUTPUTS > 0 && NEXUS_HAS_CEC
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_CecStatus stCecStatus;

    /* zero out the cec data structure before proceeding */
    BKNI_Memset(cec_configuration, 0, sizeof(boutput_hdmi_cec_configuration));

    /* get cec configuration */
    rc = NEXUS_Cec_GetStatus(dvi->cecHandle, &stCecStatus);
    if (rc != NEXUS_SUCCESS)
    {
        goto done;
    }

    cec_configuration->logical_address = stCecStatus.logicalAddress;
    BKNI_Memcpy(&cec_configuration->physical_address, &stCecStatus.physicalAddress,
        sizeof(stCecStatus.physicalAddress));
    cec_configuration->device_type = stCecStatus.deviceType;

done:
#else
    BSTD_UNUSED(dvi);
    BSTD_UNUSED(cec_configuration);
#endif

    return src;
}


bool boutput_hdmi_cec_is_device_ready(boutput_hdmi_t dvi)
{

#if B_N_DVI_OUTPUTS > 0 && NEXUS_HAS_CEC
	if (dvi == NULL)
		return false;
	else	
		return dvi->deviceReady;
#else
    BSTD_UNUSED(dvi);
    return false;
#endif

}


/* must be in manual update mode before calling this */
static void
bsettop_p_display_apply_zorder(bdisplay_t display)
{
    int i;
    BERR_Code rc;
    NEXUS_VideoWindowSettings nSettings;
    int numZorder = sizeof(display->zorderList)/sizeof(*display->zorderList);

    for(i=0; i<numZorder; i++) {
        if (display->zorderList[i].window==NULL) {
            break;
        }
        BDBG_MSG(("display %p window %#x zorder %u(%u)", display,display->zorderList[i].window, display->zorderList[i].zorder, i));
        NEXUS_VideoWindow_GetSettings(display->zorderList[i].window->nWindow, &nSettings);
        nSettings.zorder = i;
        rc =NEXUS_VideoWindow_SetSettings(display->zorderList[i].window->nWindow, &nSettings);
        if (rc!=BERR_SUCCESS) { BSETTOP_ERROR(berr_external_error);break;}
    }
    return;
}


static void
bsettop_p_display_zorder_add(bdisplay_t display, bdecode_window_t window, unsigned zorder)
{
    int i;
    int index;
    int numZorder = sizeof(display->zorderList)/sizeof(*display->zorderList);

    BDBG_MSG(("zorder_add display %p window %p zorder %d",display,window,zorder));

    for(i=0; i<numZorder; i++) {
        if (display->zorderList[i].window == window) {
            BDBG_MSG(("zorder: display %#x duplicate window %#x", (unsigned)display, (unsigned)window));
            return;
        }
    }

    for(index=-1,i=0; i<numZorder; i++) {
        if (display->zorderList[i].window == NULL || display->zorderList[i].zorder > zorder) {
            index = i;
            break;
        }
    }
    if (index==-1) {
        BDBG_ERR(("zorder: display %#x overflow", (unsigned)display));
        return;
    }
    /* shift windows up */
    for(i=numZorder-1; i>index;i--) {
        if (display->zorderList[i-1].window == NULL) {
            continue;
        }
        display->zorderList[i] = display->zorderList[i-1];
    }
    display->zorderList[index].window = window;
    display->zorderList[index].zorder = zorder;
    bsettop_p_display_apply_zorder(display);
    return;
}

static void
bsettop_p_display_zorder_remove(bdisplay_t display , bdecode_window_t window)
{
    int i;
    int index;
    int numZorder = sizeof(display->zorderList)/sizeof(*display->zorderList);

    BDBG_MSG(("zorder_remove display %p window %p",display,window));

    for(index=-1,i=0; i<numZorder; i++) {
        if (display->zorderList[i].window == window) {
            index = i;
            break;
        }
    }
    if (index == -1 ) {
        BDBG_ERR(("zorder: display %#x invalid window %#x", (unsigned)display, (unsigned)window));
        return;
    }
    /* shift windows down */
    display->zorderList[index].window = NULL;
    for(i=index;i<numZorder-1; i++) {
        if (display->zorderList[i+1].window == NULL) {
            break;
        }
        display->zorderList[i] = display->zorderList[i+1];
    }
    /* don't reapply zorder on remove */
    return;
}

static void
bsettop_p_display_zorder_set(bdisplay_t display , bdecode_window_t window , unsigned zorder)
{
    int i;
    int index;
    struct zorder_item temp;
    int numZorder = sizeof(display->zorderList)/sizeof(*display->zorderList);

    BDBG_MSG(("zorder_set display %p window %p zorder %d",display,window,zorder));

    for(index=-1,i=0; i<numZorder; i++) {
        if (display->zorderList[i].window == window) {
            index = i;
            break;
        }
    }
    if (index==-1) {
        BDBG_ERR(("zorder: display %#x unknown window %#x", (unsigned)display, (unsigned)window));
        return;
    }
    if (zorder > display->zorderList[i].zorder) { /* we should move window up */
        display->zorderList[i].zorder = zorder;
        for(i=index+1; i<numZorder;  i++) {
            if (display->zorderList[i].window==NULL) {
                /* reached last entry */
                break;
            }
            if (display->zorderList[i].zorder<zorder) {
                /* swap two entries */
                temp = display->zorderList[index];
                display->zorderList[index] = display->zorderList[i];
                display->zorderList[i] = temp;
                index = i;
            } else {
                break;
            }
        }
    } else if (zorder < display->zorderList[i].zorder) { /* we should move window down */
        display->zorderList[i].zorder = zorder;
        for(i=index-1; i>=0; i--) {
            BDBG_ASSERT(display->zorderList[i].window); /* this shall never happen */
            if (display->zorderList[i].zorder>zorder) {
                /* swap two entries */
                temp = display->zorderList[index];
                display->zorderList[index] = display->zorderList[i];
                display->zorderList[i] = temp;
                index = i;
            } else {
                break;
            }
        }
    }
    bsettop_p_display_apply_zorder(display);
    return;
}

bool
b_window_is_full_screen(const bdisplay_settings *display_settings, const bsettop_rect *position)
{
    bvideo_format_settings format_settings;

    bdisplay_get_video_format_settings(display_settings, &format_settings);

    /* NTSC height is now 482, but we need to treat 480 as full-screen,
    therefore just mask off the low order bits on height comparison. */
    if (position->x == 0 &&
        position->y == 0 &&
        position->width == format_settings.width &&
        (position->height&0xFFFD) == (format_settings.height&0xFFFD)) {
        return true;
    }
    return false;
}

void bdisplay_p_set_compressed_mute(bdisplay_t display, bool compressed_mute)
{
    if ( display->settings.dvi )
    {
        display->settings.dvi->compressed_mute = compressed_mute;
    }
    if ( display->settings.spdif )
    {
        display->settings.spdif->compressed_mute = compressed_mute;
        /* This sets both SPDIF and HDMI volumes */
        boutput_spdif_set_audio_volume(display->settings.spdif, &display->settings.spdif->volume);
    }
}

#if B_N_RF_OUTPUTS
static bresult b_set_rfm_audio_source(boutput_rf_t rf, bdisplay_t display)
{
    NEXUS_Error rc;

    if ( display != rf->master_display )
    {
        if ( rf->master_display )
        {
            bdisplay_p_enable_audio(rf->master_display, false);
            rc = NEXUS_AudioOutput_RemoveAllInputs(NEXUS_Rfm_GetAudioConnector(rf->handle));
            bdisplay_p_enable_audio(rf->master_display, true);
            if (rc) return BSETTOP_ERROR(berr_external_error);
            rf->master_display = NULL;
        }
        if ( display )
        {
            bdisplay_p_enable_audio(display, false);
            rc = NEXUS_AudioOutput_AddInput(NEXUS_Rfm_GetAudioConnector(rf->handle),
                                            NEXUS_AudioMixer_GetConnector(display->nAudioMixer));
            bdisplay_p_enable_audio(display, true);
            if (rc) return BSETTOP_ERROR(berr_external_error);
            rf->master_display = display;
        }
    }
    return 0;
}
#endif

void bdisplay_p_decode_starting(bdisplay_t display, bdecode_window_t window, bdecode_t decode)
{
    unsigned outputTimebase = display->outputTimebase;
    bdecode_t master = NULL;

    /* If this is a full-screen window or the primary window, proceed... */
    if ( &display->window[0] == window || b_window_is_full_screen(&display->settings, &window->settings.position) )
    {
        outputTimebase = decode->output_timebase;
        master = decode;
    }

    /* Only update if the timeabase is actually changing */
    if ( outputTimebase != display->outputTimebase )
    {
        bdisplay_p_set_output_timebase(display, outputTimebase);
    }

    if ( window->clone )
    {
        /* Clone window.  Check for timebase update. */
        bdisplay_p_decode_starting(window->clone->display, window->clone, decode);
    }
    if ( window->parent )
    {
        /* This is a cloned window.  No frame rate master */
        bdisplay_p_set_framerate_master(display, NULL);
    }
    else
    {
        /* Not cloned.  Set master */
        bdisplay_p_set_framerate_master(display, master);
    }
}

static void bdisplay_p_set_output_timebase(bdisplay_t display, unsigned timebase)
{
    NEXUS_DisplaySettings displaySettings;

    /* Update display timebase */
    NEXUS_Display_GetSettings(display->nDisplay, &displaySettings);
    displaySettings.timebase = timebase;
    NEXUS_Display_SetSettings(display->nDisplay, &displaySettings);

    /* Update audio timebase/pll */
    if (display->settings.spdif)
    {
        BDBG_MSG(("%s: Setting SPDIF Output to timebase %d", __FUNCTION__, timebase));
        b_set_audio_output_timebase(NEXUS_SpdifOutput_GetConnector(display->settings.spdif->handle), timebase);
    }

#if B_N_DVI_OUTPUTS
    if (display->settings.dvi)
    {
        BDBG_MSG(("%s: Setting DVI/HDMI Output to timebase %d", __FUNCTION__, timebase));
        b_set_audio_output_timebase(NEXUS_HdmiOutput_GetAudioConnector(display->settings.dvi->handle), timebase);
    }
#endif

    if (display->audioDac.handle)
    {
        BDBG_MSG(("%s: Setting Audio DACs Output to timebase %d", __FUNCTION__, timebase));
        b_set_audio_output_timebase(NEXUS_AudioDac_GetConnector(display->audioDac.handle), timebase);
    }

    display->outputTimebase = timebase;
}

static void bdisplay_p_set_framerate_master(bdisplay_t display, bdecode_t master)
{
    NEXUS_VideoInput masterInput = NULL;
    NEXUS_DisplaySettings displaySettings;

    if ( master && master->video_decode )
    {
        masterInput = NEXUS_VideoDecoder_GetConnector(master->video_decode->nVideoDecoder);
    }

    NEXUS_Display_GetSettings(display->nDisplay, &displaySettings);
    if ( displaySettings.frameRateMaster == masterInput )
    {
        return;
    }
    displaySettings.frameRateMaster = masterInput;
    NEXUS_Display_SetSettings(display->nDisplay, &displaySettings);
}

static bresult b_set_audio_output_timebase(NEXUS_AudioOutput output, NEXUS_Timebase outputTimebase)
{
    NEXUS_Error rc;
    NEXUS_AudioOutputSettings nSettings;

    BDBG_ASSERT(NULL != output);

    NEXUS_AudioOutput_GetSettings(output, &nSettings);
    nSettings.timebase = outputTimebase;
    #if B_MAX_DECODES > 1
    nSettings.pll = (outputTimebase & 0x1)?NEXUS_AudioOutputPll_e1:NEXUS_AudioOutputPll_e0;
    #endif
    rc = NEXUS_AudioOutput_SetSettings(output, &nSettings);
    if (rc) return BSETTOP_ERROR(berr_external_error);

    return b_ok;
}

