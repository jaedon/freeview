/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bvdc_mad_priv.c $
* $brcm_Revision: Hydra_Software_Devel/236 $
* $brcm_Date: 3/30/12 3:35p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_mad_priv.c $
* 
* Hydra_Software_Devel/236   3/30/12 3:35p pntruong
* SW7420-2286: Added tri-state option for pq policies for mad.
*
* Hydra_Software_Devel/235   1/12/12 5:39p vanessah
* SW7425-2162: change 7425 MADR QM buffer allocation
*
* Hydra_Software_Devel/234   7/20/11 3:00p albertl
* SW7125-1004: Added MAD hardstart reset workaround to fix transition
* between game mode and 5 field force spatial.
*
* Hydra_Software_Devel/233   6/20/11 5:49p tdo
* SW7425-104: Rename the old repeat field flag to match avc structure and
* use the new added bRepeatField flag for ANR.
*
* Hydra_Software_Devel/232   1/12/11 4:34p yuxiaz
* SW7335-1133: Added more debug messages for VDC heap debugging.
*
* Hydra_Software_Devel/231   1/4/11 4:07p yuxiaz
* SW7422-174: Refactor DCX macros.
*
* Hydra_Software_Devel/230   12/2/10 11:51a vanessah
* SW7335-1010:  Mad buffer corruption
*
* Hydra_Software_Devel/229   11/9/10 5:02p hongtaoz
* SW3548-3112: fixed hard start mode -> normal mode transition issue;
*
* Hydra_Software_Devel/228   11/5/10 5:10p hongtaoz
* SW7125-556: disable G_FEED for 4-field game modes; always enable debug
* control pixel feeders since mode_control_0 has finer control;
*
* Hydra_Software_Devel/227   10/21/10 5:58p albertl
* SW7125-656: Added workaround for format change with compression
* enabled.
*
* Hydra_Software_Devel/225   8/26/10 4:28p vanessah
* SW7420-965:  Game mode enum consistency
*
* Hydra_Software_Devel/224   8/26/10 2:39p vanessah
* SW7125-556: SW7420-965:  Support for MAD 4 fields 1 delay and MCVP game
* mode
*
* Hydra_Software_Devel/SW7420-965/1   8/20/10 1:26p vanessah
* SW7420-965:  MCVP game mode support
*
* Hydra_Software_Devel/223   8/4/10 10:28p vanessah
* SW3548-2981:  Merge into main
*
* Hydra_Software_Devel/222   8/3/10 1:57p pntruong
* SW7125-424: Initialized the fields for game mode.  Corrected the
* deinterlacer version.
*
* Hydra_Software_Devel/221   7/13/10 11:48a vanessah
* SW3548-2981:  still optimization applied to 3:2 as well, not only to
* 2:2 pulldown
*
* Hydra_Software_Devel/220   7/9/10 5:47p pntruong
* SW3556-1144: Optimize deinterlacer vertical moving pattern detection.
*
* Hydra_Software_Devel/219   6/18/10 4:21p rpan
* SW7400-2763: New BVN resource management.
*
* Hydra_Software_Devel/218   5/7/10 7:11p albertl
* SW7125-364: Changed dirty bits to use union structure to avoid type-pun
* warnings.
*
* Hydra_Software_Devel/217   4/19/10 10:16p tdo
* SW3548-2814: Improvements to VDC ulBlackMagic. Move
* BDBG_OBJECT_ID_DECLARE private header files instead of .c.
*
* Hydra_Software_Devel/216   4/7/10 11:32a tdo
* SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
*
* Hydra_Software_Devel/215   4/5/10 4:09p tdo
* SW3548-2814: Improvements to VDC ulBlackMagic
*
* Hydra_Software_Devel/214   2/11/10 11:00a pntruong
* SW7405-3829: Moved shrinkwidth to mad settings.
*
* Hydra_Software_Devel/213   2/3/10 6:49p albertl
* SW7125-180: Changed to test if TESTFEATURE1 is supported in addition to
* version number for mad.  Changed define to use capture's define in
* feeder code.
*
* Hydra_Software_Devel/212   2/3/10 6:24p albertl
* SW7125-180: Updated TESTFEATURE1 defines for 7125.
*
* Hydra_Software_Devel/211   1/28/10 11:10a syang
* SW7405-3829: add option for app to shrink src width to use deinterlace
*
* Hydra_Software_Devel/210   12/17/09 10:33a hongtaoz
* SW3548-2672: MAD source is always interlaced, so is the cropping size;
*
* Hydra_Software_Devel/209   12/16/09 10:48a hongtaoz
* SW3548-2672: fixed MAD chroma 420 init phase error when MAD is after
* CAP and SCL cropped odd lines;
*
* Hydra_Software_Devel/208   12/9/09 11:11a pntruong
* SW7550-108, SW7125-125: Default 2:2 pulldown on since it's now tuned.
*
* Hydra_Software_Devel/207   10/23/09 3:03p pntruong
* SW7400-2576: Udpate MAD 32 pulldown register setting to improve a
* customer stream performance.
*
* Hydra_Software_Devel/206   10/22/09 10:52a hongtaoz
* SW7043-233: added MAD file log tool to help debug sw algorithm; window
* writer should not set repeat flag for source repeat mute mode;
*
* Hydra_Software_Devel/205   9/15/09 4:52p hongtaoz
* SW3548-2471: slight adjusted pixel constellation for game mode
* transition;
*
* Hydra_Software_Devel/204   9/15/09 10:14a hongtaoz
* SW3548-2471: fixed compile error for 7405;
*
* Hydra_Software_Devel/203   9/14/09 5:31p hongtaoz
* SW3548-2471: fixed compile error;
*
* Hydra_Software_Devel/202   9/14/09 4:12p hongtaoz
* SW3548-2471: added window API to allow smooth transition for MAD game
* modes; updated MAD 3-field game modes QM constellations;
*
* Hydra_Software_Devel/201   9/3/09 10:31p pntruong
* SW3548-2373: Rolled back PR56964.
*
* Hydra_Software_Devel/200   9/1/09 9:44a yuxiaz
* SW3556-672: More update on dynamic default.
*
* Hydra_Software_Devel/199   8/31/09 10:40a yuxiaz
* SW3556-672: Fixed build on 7420.
*
* Hydra_Software_Devel/198   8/28/09 4:32p yuxiaz
* SW3556-672: Use BDBG_MSG.
*
* Hydra_Software_Devel/197   8/28/09 1:40p hongtaoz
* SW7342-25: hardwired hscl needs to update info before eSrcPolarity is
* modified;
*
* Hydra_Software_Devel/196   8/28/09 1:24p yuxiaz
* SW3556-672: BVDC_Window_GetDeinterlaceDefaultConfiguration should
* Return Dynamic Defaults.
*
* Hydra_Software_Devel/195   7/22/09 1:33p hongtaoz
* PR56938: added MAD hard start sw workaround to avoid stalling when test
* feature1 is on; basically disable MAD feeders when the corresponding
* field stores are uninitialized;
*
* Hydra_Software_Devel/194   7/21/09 2:17p syang
* PR 56964: take off the "connect drainer, en and then dis" step  in
* vnet-drain code
*
* Hydra_Software_Devel/193   6/18/09 5:51p syang
* PR 55877, PR 55871, PR 56127, PR 55812: update to support 7340 and 7550
*
* Hydra_Software_Devel/192   6/9/09 2:37p hongtaoz
* PR55795: tuned 1080i hdmi input 3:2 bad weave settings;
*
* Hydra_Software_Devel/191   5/28/09 2:19p yuxiaz
* PR55364: Disable dithering when any testfeature1 is enabled
*
* Hydra_Software_Devel/190   4/3/09 4:33p yuxiaz
* PR53460: Optimal 444/422 conversion settings
*
* Hydra_Software_Devel/189   3/16/09 10:48p tdo
* PR45785, PR45789: Merge from MCVP branch
*
* Hydra_Software_Devel/7420_mcvp/1   2/26/09 11:17p tdo
* PR 45785, PR 45789: merge from main branch on 2/26/09
*
* Hydra_Software_Devel/188   2/26/09 6:10p yuxiaz
* PR52287: 422-444 and 444-422 converters should use decimate/duplicate.
*
* Hydra_Software_Devel/187   2/20/09 2:50p yuxiaz
* PR51178: Change DCX bpp requires hard start for MAD.
*
* Hydra_Software_Devel/186   2/13/09 11:15a yuxiaz
* PR52030: MAD QM do not use DCX logic.
*
* Hydra_Software_Devel/185   2/11/09 7:06p hongtaoz
* PR52018: 3:2 source material showed garbage transition when changing
* source cropping; fix for game mode is to skip the forced weave during
* hard_start; fix for normal mode is to clear the IT_DEBUG_MODE_FORCE in
* debug control register when exiting game mode since that bit seems
* sticky and in effect even IT hw takes back control of the field state
* update logic;
*
* Hydra_Software_Devel/184   1/22/09 2:50p hongtaoz
* PR51366: remember the previous field store and qm store capture
* pointers to have smooth transition when switching between game modes;
* only hard_start when necessary;
*
* Hydra_Software_Devel/183   1/16/09 6:32p hongtaoz
* PR51079: added new chroma setting;
*
* Hydra_Software_Devel/182   1/16/09 6:28p hongtaoz
* PR51079: added new deinterlacer custom chroma settings;
*
* Hydra_Software_Devel/181   1/16/09 11:43a yuxiaz
* PR51160: Updated throughput calculation for MAD/ANR.
*
* Hydra_Software_Devel/180   12/18/08 10:50a hongtaoz
* PR50459, PR45707: revert back the 422 chroma setting for NTSC;
*
* Hydra_Software_Devel/179   12/9/08 6:43p hongtaoz
* PR48213, PR46738: changed pixel constellations for 5-field and 3-field
* 1-delay game mode and associated programming to avoid hang condition
* with memory saving mode; 3:2 soft algorithm is modified to adapt to
* the new constellation;
*
* Hydra_Software_Devel/178   12/5/08 9:58p pntruong
* PR49848: Fine tuned motion calculation for if/cvbs source.
*
* Hydra_Software_Devel/177   11/25/08 11:13a syang
* PR 49562: add reset to output channel ping-pong buf of sub-modules and
* loopbacks and free-channels
*
* Hydra_Software_Devel/176   11/24/08 4:19p yuxiaz
* pr45484: Fixed Dither settings in MAD_SIOB_0.
*
* Hydra_Software_Devel/175   11/18/08 6:56p hongtaoz
* PR48213: implemented rev2:2 pulldown soft algorithm for 5-field 1-delay
* game mode;
*
* Hydra_Software_Devel/174   11/12/08 3:02p hongtaoz
* PR48213: implemented 3:2 reverse pulldown soft algorithm for 5-field 1-
* delay MAD game mode; re-ordered the pixel constellation for 5-field 1-
* delay game mode;
*
* Hydra_Software_Devel/173   10/27/08 5:38p syang
* PR 46477: back off SetEnable in in BuildRul_isr according to GetOps (it
* causes window to disapper on 7400 D0)
*
* Hydra_Software_Devel/172   10/24/08 4:07p syang
* PR 46477: in BuildRul_isr only disable in shadow regs  according to
* GetOps (some SetEnable_isr does HW reset)
*
* Hydra_Software_Devel/171   10/24/08 11:05a syang
* PR 46477: in BuildRul_isr reset enable bit according to GetOps
*
* Hydra_Software_Devel/170   10/22/08 1:59p hongtaoz
* PR48143: only build necessary CAP/DNR/HSCL/SCL RULs when their
* operational flags are enabled; this fixed SCL/HSCL enable error;
*
* Hydra_Software_Devel/169   10/20/08 1:48p pntruong
* PR48032: Apply Kell factor for progressive to interlaced scaling case.
*
* Hydra_Software_Devel/168   10/6/08 1:12p pntruong
* PR47595, PR47072: Fixed pixel formats for cap/mad.
*
* Hydra_Software_Devel/167   10/3/08 11:00a yuxiaz
* PR46305, PR46307, PR46309: Use new TestFeature1 function from BSUR.
* Change BVDC_P_SUPPORT_VIDEO_TestFeature1 to
* BVDC_P_SUPPORT_VIDEO_TESTFEATURE1.
*
* Hydra_Software_Devel/166   10/3/08 9:25a yuxiaz
* PR46305, PR46307, PR46309: More Video TestFeature1 support.
*
* Hydra_Software_Devel/165   10/1/08 8:33p pntruong
* PR47303, PR46309: Reused the TestFeature1 type in surface.
*
* Hydra_Software_Devel/164   10/1/08 3:56p hongtaoz
* PR47490, PR46826: disable dithering when ANR is enabled; 3548B0 moved
* MAD/ANR dither to MAD_SIOB_0 block;
*
* Hydra_Software_Devel/163   9/30/08 6:30p hongtaoz
* PR46738: fixed MAD/ANR memory saving mode;
*
* Hydra_Software_Devel/162   9/30/08 4:56p yuxiaz
* PR46305, PR46307: Video TestFeature1 support: Fix throughput
* calculation, combine MAD and ANR set TestFeature1 function.
*
* Hydra_Software_Devel/161   9/26/08 3:24p yuxiaz
* PR46305, PR46307: More video TestFeature1 support for 3548 B0.
*
* Hydra_Software_Devel/160   9/25/08 5:10p hongtaoz
* PR46842: MAD custom setting is overwrittern;
*
* Hydra_Software_Devel/159   9/24/08 6:20p pntruong
* PR47113, PR45308: Allow flexibility in selecting builtins and user
* defined filter coefficients.
*
* Hydra_Software_Devel/158   9/24/08 5:22p syang
* PR 46822:  Add New MAD Motion Detection Register Settings for Scene
* Change Performance for 3548 B0
*
* Hydra_Software_Devel/157   9/17/08 8:43p pntruong
* PR47013: B0: Initial vdc support.
*
* Hydra_Software_Devel/156   9/9/08 9:04p tdo
* PR45802: Remove unnecessary macros and unused code
*
* Hydra_Software_Devel/155   9/5/08 4:46p tdo
* PR46484: Bringup appframework for7420
*
* Hydra_Software_Devel/154   9/2/08 6:05p pntruong
* PR46314, PR46316: Updated 3dcomb memory allocation, and added buffers
* alloc debug messages.
*
* Hydra_Software_Devel/153   9/2/08 11:53a rpan
* PR45319: Covert picture node flags to bit-fields to reduce structure
* size.
*
* Hydra_Software_Devel/152   8/19/08 11:26a tdo
* PR45707: Update XCHROMA_MODE for both NTSC and PAL input
*
* Hydra_Software_Devel/151   8/18/08 3:29p tdo
* PR45707: Update MAD: 3:2 pulldown setting with ANR fix (for demo
* release)
*
* Hydra_Software_Devel/150   8/8/08 3:29p yuxiaz
* PR45484: Enable Dithering in VDC.
*
* Hydra_Software_Devel/149   8/6/08 11:51a yuxiaz
* PR45367: Enable Dering in VDC.
*
* Hydra_Software_Devel/148   8/4/08 12:06p hongtaoz
* PR45232: updated some of soft22 settings; added MAD soft22 debug
* compile option to use RDC scratch registers to debug soft22 algorithm
* settings;
*
* Hydra_Software_Devel/147   7/31/08 11:02a hongtaoz
* PR45232: fixed compile error for 7403;
*
* Hydra_Software_Devel/146   7/30/08 4:50p hongtaoz
* PR45231, PR45232, PR45224: updated rev32 settings for NTSC oversample
* mode; increased rev22 lock enter/exit/saturation levels for NTSC
* input; disable PCC_BWV_ENABLE for SECAM input;
*
* Hydra_Software_Devel/145   7/29/08 5:11p hongtaoz
* PR45231: Update 3:2 settings in oversampling mode;
*
* Hydra_Software_Devel/144   7/1/08 4:46p hongtaoz
* PR43932: disable hw scene change detection by threshold settings and
* leave the hw PCC bad weave feature on to improve motion quality;
*
* Hydra_Software_Devel/143   6/26/08 4:31p hongtaoz
* PR43932: added sw workaround for MAD scene change issue;
*
* Hydra_Software_Devel/142   6/20/08 1:43p tdo
* PR43919: MAD settings are not updated when source format changes
*
* Hydra_Software_Devel/141   6/11/08 4:24p hongtaoz
* PR43460: add HSCL for ANR or MAD usage mode; HSCL seems to shield BVB
* error going to ANR;
*
* Hydra_Software_Devel/140   6/5/08 5:03p hongtaoz
* PR39807: 3548A0 bringup; put SCL in always_enable mode; separate
* anr+mad combo mode and memory saving mode in macroes;
*
* Hydra_Software_Devel/139   5/23/08 11:10a tdo
* PR42798: Strobe cause break picture
*
* Hydra_Software_Devel/138   5/19/08 2:25p yuxiaz
* PR40122: Use new 10-bit 422 packed pixel format for MAD on 3548.
*
* Hydra_Software_Devel/137   4/18/08 1:42p pntruong
* PR41898: Need better synchronization of window states.  Synchronize the
* releases of resource and vnetmode together in writer task.
*
* Hydra_Software_Devel/136   3/24/08 6:51p hongtaoz
* PR39202: fixed typo of previous checkin;
*
* Hydra_Software_Devel/135   3/24/08 6:42p hongtaoz
* PR39202: added new MAD registers (scene change and PCC-based bad weave
* detection changes) for 3548;
*
* Hydra_Software_Devel/134   3/19/08 7:37p hongtaoz
* PR39154, PR39202, PR39203: combined ANR and MAD in memory saving mode
* on 3548; implemented MAD specific change; updated SCL precision for
* 3548;
*
* Hydra_Software_Devel/133   3/3/08 2:04p yuxiaz
* PR39158, PR39155: Added dithering in HSCL. Move De-ring from MAD to
* HSCL for 3548.
*
* Hydra_Software_Devel/132   2/29/08 4:18p yuxiaz
* PR39158: Implement dithering in various BVN components for 3548.
*
* Hydra_Software_Devel/131   2/28/08 10:11p pntruong
* PR39154: Resolved clearcase merged.
*
* Hydra_Software_Devel/130   2/28/08 6:22p hongtaoz
* PR39154, PR36273, PR39202: added HSCL block; enabled MAD/ANR for 3548;
* fixed non-linear scaling when MAD is on;
*
* Hydra_Software_Devel/129   2/22/08 2:32p yuxiaz
* PR39155: Added de-ringing filters in BVN.
*
* Hydra_Software_Devel/128   2/21/08 2:13p hongtaoz
* PR39154: added a new flag to signal MAD user changes;
*
* Hydra_Software_Devel/127   2/19/08 10:35p pntruong
* PR39154: Removed 3548 warnings.
*
* Hydra_Software_Devel/126   2/19/08 3:36p hongtaoz
* PR39154: re-organize deinterlacer code to be more readable;
*
* Hydra_Software_Devel/125   2/13/08 10:43p pntruong
* PR39421: Added stub for non-mad support.
*
* Hydra_Software_Devel/124   2/13/08 3:10p pntruong
* PR39421: Removed unused code to improve readability.
*
* Hydra_Software_Devel/123   2/12/08 8:19p pntruong
* PR38929: Make more files compile.
*
* Hydra_Software_Devel/123   2/12/08 8:18p pntruong
* PR38929: Make more files compile.
*
* Hydra_Software_Devel/122   1/18/08 11:20a hongtaoz
* PR38736: added force_spatial 3-field MAD game mode support; re-grouped
* MAD game modes definition and added more comments;
*
* Hydra_Software_Devel/121   1/14/08 5:52p tdo
* PR36591: Properly disable MAD 2:2 pulldown for 1080i source
*
* Hydra_Software_Devel/120   1/11/08 10:07a tdo
* PR36591: Fix a bug that cause 2:2 pulldown to oscillate in the logic to
* disable 2:2 pulldown for 1080i source
*
* Hydra_Software_Devel/119   12/14/07 4:09p pntruong
* PR38098:Support MAD game mode trick mode.
*
* Hydra_Software_Devel/118   12/12/07 9:55a pntruong
* PR37862: Fine tune for PAL demo.
*
* Hydra_Software_Devel/117   12/10/07 10:33p pntruong
* PR31084: Add code to get 2:2 pulldown default settings.
*
* Hydra_Software_Devel/116   12/6/07 2:21p pntruong
* PR37754: Need to update MAD_0_HORIZ_FIR_LUMA_SRC_PIC_OFFSET on offset
* changes.
*
* Hydra_Software_Devel/115   11/19/07 3:50p tdo
* PR36898: Add VDC PI support for 7335
*
* Hydra_Software_Devel/114   11/14/07 10:55a rpan
* PR31084: Consider NULL pointers as valid values when getting
* VDEC/Deinterlace default settings.
*
* Hydra_Software_Devel/113   11/13/07 4:07p rpan
* PR31084: Added support for getting default de-interlace settings.
*
* Hydra_Software_Devel/112   11/8/07 2:16p tdo
* PR36591: Disable MAD 2:2 pulldown for 1080i source
*
* Hydra_Software_Devel/111   11/7/07 4:10p pntruong
* PR36539: Added for field freeze after hardstart, to avoid display old
* pictures in mad fifo.
*
* Hydra_Software_Devel/110   11/5/07 11:47a syang
* PR 29617:  Set SCL's top/left cut with vsync delay according to MAD
*
* Hydra_Software_Devel/109   11/2/07 2:19p syang
* PR 29617:  prepare to set clip in SCL with delay according to MAD
*
* Hydra_Software_Devel/108   11/2/07 10:05a syang
* PR 34773, PR 35313: improve msg for anr/mad buffer alloc
*
* Hydra_Software_Devel/107   10/30/07 7:22p pntruong
* PR34239: Allow dynamically loading of vec custom timing.
*
* Hydra_Software_Devel/106   10/29/07 2:38p hongtaoz
* PR35308: reversed match_UM and unmatch_UM for 2:2 soft cadence to fix
* the normal mode bad weave of the 'splendor' clip;
*
* Hydra_Software_Devel/105   10/19/07 6:17p hongtaoz
* PR35308:Support 2:2 pulldown detect algorithm for MAD 3 field low
* bandwidth game mode;
*
* Hydra_Software_Devel/PR35308/1   10/10/07 5:41p xhuang
* PR35308:Support 2:2 pulldown detect algorithm for MAD 3 field low
* bandwidth game mode
*
* Hydra_Software_Devel/103   9/21/07 4:35p yuxiaz
* PR34523: Add 7325 support for VDC.
*
* Hydra_Software_Devel/102   9/20/07 5:25p hongtaoz
* PR34569: update 2:2 lock bit when toggling the software 2:2;
*
* Hydra_Software_Devel/101   9/18/07 4:22p pntruong
* PR35012: Need to correct MAD registers' default value.
*
* Hydra_Software_Devel/100   8/30/07 3:13p pntruong
* PR34348: MAD optimizing Low Angle Settings for reduce gray noise.
*
* Hydra_Software_Devel/PR34348/1   8/29/07 4:02p xhuang
* PR34348:add low angle customer settings
*
* Hydra_Software_Devel/99   8/21/07 6:01p hongtaoz
* PR32942: fixed software 3:2 algorithm lock drop problem; also increased
* chroma 3:2 bad weave threshold to avoid lock drop for speedway clip;
*
* Hydra_Software_Devel/98   8/15/07 10:41a pntruong
* PR32942, PR33348: Fixed 7403 build errors.
*
* Hydra_Software_Devel/97   8/13/07 5:59p hongtaoz
* PR32942, PR33348: rollback change on game mode hard_start to avoid
* diagonal artifact;
*
* Hydra_Software_Devel/96   8/13/07 5:00p hongtaoz
* PR32942, PR33348: update IT_OUTPUT_CONTROL on every vsync for low
* bandwidth game mode 3:2 pulldown software algorithm; some MAD settings
* updated;
*
* Hydra_Software_Devel/95   8/7/07 12:44p tdo
* PR33792: Addtional Parameter for MAD chroma settings
*
* Hydra_Software_Devel/94   8/2/07 11:31p hongtaoz
* PR32942: added reverse 3:2 pulldown software algorithm to 3-field low
* bandwidth game mode;
*
* Hydra_Software_Devel/93   8/2/07 5:34p syang
* PR 33237: corrected MAD horizontal scale setup
*
* Hydra_Software_Devel/92   8/2/07 1:51p pntruong
* PR31983: C0: Need to enable 10-bit MAD capture mode when source is
* 480i.
*
* Hydra_Software_Devel/91   7/31/07 1:57p pntruong
* PR32487: Vertical scaler needs to be programmed to low-pass filter
* chroma for MAD source.  Added debug to print out name of selected
* coeffs set.
*
* Hydra_Software_Devel/90   7/30/07 2:11p hongtaoz
* PR26785, PR33560, PR33020: added MAD game mode hard start;
*
* Hydra_Software_Devel/89   7/23/07 6:44p hongtaoz
* PR33345: added 3-field 0-delay MAD game mode;
*
* Hydra_Software_Devel/88   7/19/07 6:46p tdo
* PR31512: More fine tuning to 2:2 pulldown algorithm
*
* Hydra_Software_Devel/87   7/19/07 3:11p pntruong
* PR32503: Fixed build warnings.
*
* Hydra_Software_Devel/86   7/16/07 1:50p pntruong
* PR32503: Updated mad settings for oversampling.
*
* Hydra_Software_Devel/85   7/8/07 9:10p tdo
* PR31512: Fine tuning sw 2:2 algorithm for repeated buffer case
*
* Hydra_Software_Devel/84   7/3/07 2:51p syang
* PR32503: print msg before mem alloc
*
* Hydra_Software_Devel/83   6/29/07 11:47a pntruong
* PR32503: Updated mad settings for oversampling.
*
* Hydra_Software_Devel/82   6/28/07 3:58p syang
* PR 32576: always re-calculate mad/anr bufHeapId inside
* NeedReconfiguring_isr if mad/anr is to be sued
*
* Hydra_Software_Devel/81   6/21/07 5:00p tdo
* PR31512:Update Mad setting for reduce flicker
*
* Hydra_Software_Devel/80   6/21/07 5:23a albertl
* PR31821:  Changed MAD game modes to allocate only the necessary number
* of buffers and to reconfigure the vnet when buffer numbers change.
*
* Hydra_Software_Devel/79   6/18/07 3:22p maivu
* PR 31342: Enable PCC bad weave detection/correction, and use new
* Alternate REPF_VETO_LEVEL to fix bad weaving lines.
*
* Hydra_Software_Devel/78   6/8/07 11:32a syang
* PR 31849: separate drain from disable for MAD, ANR and CAP
*
* Hydra_Software_Devel/77   5/29/07 12:14p syang
* PR30364: fix a typo with bDnRingSuprresion setting
*
* Hydra_Software_Devel/76   5/18/07 4:20p albertl
* PR28894:  Added new 3-field low bandwidth MAD game mode for 3563 C0.
*
* Hydra_Software_Devel/75   5/17/07 3:44p pntruong
* PR30364: Provide API to control MAD up/down sampling.
*
* Hydra_Software_Devel/74   5/10/07 11:37a syang
* PR 29846: remove enum eDrainVnet; clean up
*
* Hydra_Software_Devel/73   5/10/07 9:57a syang
* PR 28895, PR 28898, PR 29846 , PR 28550: 1). add frame for C0 ANR PI
* support; 2). move MAD/ANR buf alloc/free to MAD/ANR; 3). remove
* drainVnet state
*
* Hydra_Software_Devel/72   5/1/07 4:42p syang
* PR 28541: 1) changed non-linear scl api; 2). carry sub-pxl accuracy
* through the pipe line by SrcHStep and SrcVStep
*
* Hydra_Software_Devel/71   4/24/07 1:45p yuxiaz
* PR27644: Added 7405 support.
*
* Hydra_Software_Devel/70   4/17/07 6:46p albertl
* PR26785:  Fixed pMadCfg const pointer warning.
*
* Hydra_Software_Devel/69   4/17/07 5:45p albertl
* PR26785:  Mad game modes are turned off for a few vsyncs on source clip
* change.
*
* Hydra_Software_Devel/68   4/11/07 10:16a pntruong
* PR28863: Video shaking vertically when toggling 3:2 pulldown on/off.
*
* Hydra_Software_Devel/67   4/6/07 11:36a pntruong
* PR29194: Added the new coefficients.
*
* Hydra_Software_Devel/66   4/5/07 4:59p albertl
* PR29194:  Added BVDC_Window_SetCoefficientIndex and input and index
* matching support to scaler coefficient tables.
*
* Hydra_Software_Devel/65   4/5/07 2:02p pntruong
* PR28538, PR28539, PR28540, PR25462: Average pixel luma and customized
* deinterlacer support.
*
* Hydra_Software_Devel/64   4/4/07 3:45p syang
* PR 29243: change VnetMode def (now use bit-fields)
*
* Hydra_Software_Devel/63   4/2/07 11:28a syang
* PR 29243: clean up: take off the obselete old-vnet-code
*
* Hydra_Software_Devel/PR28538/2   3/29/07 3:06p xhuang
* PR28538:modify to get correct current setting of mad
*
* Hydra_Software_Devel/PR28538/1   3/28/07 6:30p xhuang
* PR28538:Add new api for 3:2 pulldown, 2:2 pulldown and mad setting.
* Also for PR28539,PR28540
*
* Hydra_Software_Devel/62   3/23/07 11:41a yuxiaz
* PR25181: Back out previous changes.
*
* Hydra_Software_Devel/61   3/22/07 4:38p yuxiaz
* PR25181: Release MAD buffers when re-allocating capture buffers to
* avoid buffer heap fragmentaion.
*
* Hydra_Software_Devel/60   3/14/07 6:32p tdo
* PR 28684: Fix state machine to force MAD block to hard start when MAD
* is toggled
*
* Hydra_Software_Devel/59   2/23/07 6:29p tdo
* PR 27970: Share LPB and FCH if source is the same to eliminate the out
* of resource error when number of LPB and FCH are limited
*
* Hydra_Software_Devel/58   2/16/07 10:30p albertl
* PR20874:  Added new 3563 MAD registers.  Fixed MAD game mode compile
* errors for other chips.
*
* Hydra_Software_Devel/57   2/16/07 7:48p albertl
* PR23521: Added new 3563 MAD Game Modes BVDC_MadGameMode_e4Fields_0Delay
* and BVDC_MadGameMode_e5Fields_0Delay.
*
* Hydra_Software_Devel/56   2/12/07 1:50p pntruong
* PR27821:  Remove unused lagacy and work-around code (7038) from
* mainline.
*
* Hydra_Software_Devel/54   2/9/07 5:32p syang
* PR 27054: don't build rul when op=eReleaseHandle. (why we see some
* module enabled in shadow register when other modules in vnet are not
* yet???)
*
* Hydra_Software_Devel/53   2/6/07 2:13p hongtaoz
* PR23521, PR25497: added MAD game mode support;
*
* Hydra_Software_Devel/52   1/22/07 3:14p syang
* PR 27054: 1). only 1 win build rul, 2). reader's modules don't build
* rul before video sur enabled; 3). writer's modules don't build rul
* before (GetReconfiguring_isr==true )
*
* Hydra_Software_Devel/51   1/13/07 12:39p syang
* PR 23216: turns on chroma coeff in mad horizontal scaler
*
* Hydra_Software_Devel/50   1/10/07 10:59a syang
* PR 22569:  add new vnet algorithm code to sub-modules such as scl, mad,
* dnr, mfd, vfd, cap, ...
*
* Hydra_Software_Devel/49   1/9/07 11:14a tdo
* PR 26115:  Break out BLOCK_WRITE_TO_RUL due to register shifting in
* 7400 B0 and 3563 A0
*
* Hydra_Software_Devel/48   12/22/06 11:54a syang
* PR 22569: added BVDC_P_PicComRulInfo as 1st step to do more data
* isolating between modules. Picture will not be passed to module in
* future
*
* Hydra_Software_Devel/47   12/18/06 11:32p pntruong
* PR22577: Merged back to mainline.
*
* Hydra_Software_Devel/Refsw_Devel_3563/11   11/22/06 3:51p syang
* PR 26151: re-write shared resource manager. update letter box, mad, dnr
* shared-scl acquire/release code accordingly
*
* Hydra_Software_Devel/Refsw_Devel_3563/10   11/16/06 1:55p hongtaoz
* PR25668: merge in vdc support for 7403;
*
* Hydra_Software_Devel/Refsw_Devel_3563/9   11/15/06 7:31p albertl
* PR20886:  Added MAD reset for both MAD blocks for 3563.
*
* Hydra_Software_Devel/Refsw_Devel_3563/8   11/3/06 10:22a hongtaoz
* PR25497: rotate currentQM every other bottom field for game mode;
*
* Hydra_Software_Devel/Refsw_Devel_3563/7   10/20/06 1:41p pntruong
* PR23521:  Removed duplicate code, and move game mode into existing
* deinterlace settings structures.
*
* Hydra_Software_Devel/Refsw_Devel_3563/6   10/19/06 7:28p albertl
* PR23521:  Fixed compile errors for 7401 and 7038.
*
* Hydra_Software_Devel/Refsw_Devel_3563/5   10/19/06 3:53p albertl
* PR23521:  Added MAD game mode functionality.
*
* Hydra_Software_Devel/Refsw_Devel_3563/4   10/16/06 1:22p tdo
* PR 23377: Fix compiling error on unsupported chip sets
*
* Hydra_Software_Devel/Refsw_Devel_3563/3   10/12/06 5:47p tdo
* PR 23377:  Add support for MAD 3:2 and 2:2 reverse pulldown
*
* Hydra_Software_Devel/Refsw_Devel_3563/2   9/27/06 4:19p pntruong
* PR23222, PR23225:  Added vdec's input validations.
*
* Hydra_Software_Devel/Refsw_Devel_3563/1   9/20/06 12:47p syang
* PR 20884: add SCL chroma coeff
*
* Hydra_Software_Devel/45   9/16/06 7:06p syang
* PR 23216: partial preparing for moving half band horiz scl to MAD
*
* Hydra_Software_Devel/44   9/14/06 8:39p albertl
* PR20874:  Added dual had core support for 3563:
*
* Hydra_Software_Devel/43   9/14/06 3:05p hongtaoz
* PR21703: disable MAD register double buffering feature;
*
* Hydra_Software_Devel/42   9/7/06 6:47p pntruong
* PR23744: Update RDB with MAD_0, DNR_0, and TAB_0.
*
* Hydra_Software_Devel/41   8/28/06 1:45p yuxiaz
* PR20875: Renamed BVDC_P_SUPPORT_10BIT_BVN to
* BVDC_P_SUPPORT_BVN_10BIT_444. Removed BVDC_P_BVNDataMode, use window
* and buffer node's pixel format. Integrated with new 10 bit pixel
* formats.
*
* Hydra_Software_Devel/40   8/23/06 11:48a hongtaoz
* PR23378: round down the MAD source picture size;
*
* Hydra_Software_Devel/39   6/14/06 1:41p syang
* PR 21689: add support for 7118
*
* Hydra_Software_Devel/38   6/6/06 11:29a hongtaoz
* PR21714: put constant settings to init function;
*
* Hydra_Software_Devel/37   6/5/06 6:20p hongtaoz
* PR21714: MAD config for chroma source;
*
* Hydra_Software_Devel/36   5/26/06 3:41p pntruong
* PR20642: Refactored handling of hList.
*
* Hydra_Software_Devel/35   5/19/06 1:48p pntruong
* PR21560: Global symbols without BVDC prefix in the BVDC module.  Reduce
* unnecessary global exported symbols.  Tighten const params.
*
* Hydra_Software_Devel/34   3/16/06 6:15p hongtaoz
* PR19977: clear field repeat flag after use by MAD;
*
* Hydra_Software_Devel/33   2/10/06 6:18p hongtaoz
* PR16654: disable TRICK_MODE_SEL if compile option doesn't support it;
*
* Hydra_Software_Devel/32   10/13/05 11:51a hongtaoz
* PR17532: added a test API to turn on OSD for deinterlacer;
*
* Hydra_Software_Devel/31   10/4/05 3:47p hongtaoz
* PR12851: reduce MAD RUL size if muted;
*
* Hydra_Software_Devel/30   10/3/05 4:35p pntruong
* PR17385: Added support for vertical/horizontal size matching in
* bvdc_scaler_priv.c for coeffs selection.  Fixed top panscan for block-
* averaging mode.
*
* Hydra_Software_Devel/29   8/18/05 1:17p pntruong
* PR15757, PR16391, PR16411, PR12519, PR14791, PR15535, PR15206, PR15778:
* Improved bandwidth for cropping/scaler/capture/playback.   Unified
* window shutdown sequence for destroy, reconfigure mad/scaler, and/or
* reconfigure result of source changes.  And miscellances fixes from
* above PRs.
*
* Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/5   8/17/05 3:44p pntruong
* PR16411: Check 3D comb delay reported by VDC.
*
* Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/4   8/5/05 12:14p pntruong
* PR12519: Clear unused mad dirty so that applychanges will set event on
* non-detected analog video.
*
* Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/3   8/4/05 1:56p pntruong
* PR16568: Need to drain MAD's old data on channel change.
*
* Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/2   8/2/05 1:07p pntruong
* PR16484: (1241) Progressive outputs give flickery picture.
*
* Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/1   7/14/05 7:57p pntruong
* PR12519: Take in changes from mainline before merge back.
*
* Hydra_Software_Devel/28   4/21/05 1:07p pntruong
* PR14691: Protect again missing rul execution.
*
* Hydra_Software_Devel/27   4/15/05 8:09p hongtaoz
* PR14691: reset video window's path when shutdown;
*
* Hydra_Software_Devel/26   4/14/05 10:24a syang
* PR 14692: re-organize picture's rectangles, fixed duplicated clips in
* diff modules, changed to do clip as soon as possible, and added
* capture clipping for C0
*
* Hydra_Software_Devel/25   4/1/05 5:13p pntruong
* PR14686: Refresh mad mstart addresses on re-enable and fixed vdec 3d
* comb mstart alignment.
*
* Hydra_Software_Devel/24   3/28/05 7:58p pntruong
* PR14117: VCR mode - recommended register settings.
*
* Hydra_Software_Devel/23   3/17/05 6:40p pntruong
* PR14494: Add preliminary software support to 3560 A0.
*
* Hydra_Software_Devel/22   1/27/05 12:12p hongtaoz
* PR13740: added vdc internal detection of repeated field for MAD trick
* mode support;
*
* Hydra_Software_Devel/21   1/7/05 12:53p hongtaoz
* PR13740: stops MAD trick mode if mpeg picture is muted to prevent green
* screen;
*
* Hydra_Software_Devel/20   1/6/05 12:13p hongtaoz
* PR13740: added MAD support for 7038 C0;
*
***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "brdc.h"

#include "bvdc.h"
#include "bvdc_mad_priv.h"
#include "bvdc_buffer_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_window_priv.h"
#include "bvdc_vnet_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bvdc_compositor_priv.h"

#if BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED
#include "bvdc_hscaler_priv.h"
#endif

/* TODO: remove it if PR43932 is fixed */
#define BVDC_P_MAD_SCENE_CHANGE_WORKAROUND 1
/* TODO: remove if JIRA SW7125-656 is fixed */
#define BVDC_P_MAD_TEST_FEATURE_FORMAT_CHANGE_WORKAROUND 1

/* Dither settings for MAD */
#define BVDC_P_DITHER_MAD_LFSR_CTRL_T0               (0x7)
#define BVDC_P_DITHER_MAD_LFSR_CTRL_T1               (0x7)
#define BVDC_P_DITHER_MAD_LFSR_CTRL_T2               (0x5)
#define BVDC_P_DITHER_MAD_LFSR_VALUE                 (0xA2706)

#define BVDC_P_MAD_REV22_LEVEL_ADJ         (10) /* PR45232 */

/* Default settings */
#define BVDC_P_MAD_REV32_PHASE_MATCH_THRESH          (0x4)
#define BVDC_P_MAD_REV32_ENTER_LOCK_LEVEL            (0x6)
#define BVDC_P_MAD_REV32_LOCK_SAT_LEVEL              (0xd2)
#define BVDC_P_MAD_REV32_LUMA_32_THRESH              (0x44c)
#define BVDC_P_MAD_REV32_LUMA_32_THRESH_OVERSAMPLE   (0x60e)

/* Nicer formating and ensure correct location for enum indexing. */
#define BVDC_P_MAKE_GAMEMODE_INFO(mode, delay, buffer_cnt)                   \
{                                                                            \
	(BVDC_MadGameMode_##mode), (delay), (buffer_cnt), (#mode)                \
}

/* Mad game mode delay tables */
static const BVDC_P_MadGameModeInfo s_aMadGameModeInfo[] =
{
	BVDC_P_MAKE_GAMEMODE_INFO(eOff,                  3, 5),
	BVDC_P_MAKE_GAMEMODE_INFO(e5Fields_2Delay,       2, 5),
	BVDC_P_MAKE_GAMEMODE_INFO(e5Fields_1Delay,       1, 5),
	BVDC_P_MAKE_GAMEMODE_INFO(e5Fields_0Delay,       0, 5),
	BVDC_P_MAKE_GAMEMODE_INFO(e5Fields_ForceSpatial, 0, 5),
	BVDC_P_MAKE_GAMEMODE_INFO(e4Fields_2Delay,       2, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e4Fields_1Delay,       1, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e4Fields_0Delay,       0, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e4Fields_ForceSpatial, 0, 4),
	BVDC_P_MAKE_GAMEMODE_INFO(e3Fields_2Delay,       2, 3),
	BVDC_P_MAKE_GAMEMODE_INFO(e3Fields_1Delay,       1, 3),
	BVDC_P_MAKE_GAMEMODE_INFO(e3Fields_0Delay,       0, 3),
	BVDC_P_MAKE_GAMEMODE_INFO(e3Fields_ForceSpatial, 0, 3)
};

#if (BVDC_P_SUPPORT_MAD)

#if (BVDC_P_SUPPORT_DMISC)
#include "bchp_dmisc.h"
#endif

#include "bchp_mad_0.h"
#include "bchp_mmisc.h"
#include "bchp_bvnf_intr2_5.h"
#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR)
#include "bchp_mad_siob_0.h"

/* PR56938 workaround */
#define BVDC_P_MAD_HARDSTART_COUNTDOWN    (4)
#endif

#if BCHP_BVNF_INTR2_5_R5F_STATUS_MAD_0_IT_READY_INTR_MASK
#define BVDC_P_MAD_0_IT_READY_INT_MASK \
	BCHP_BVNF_INTR2_5_R5F_STATUS_MAD_0_IT_READY_INTR_MASK
#if BCHP_BVNF_INTR2_5_R5F_STATUS_MAD_1_IT_READY_INTR_MASK
#define BVDC_P_MAD_1_IT_READY_INT_MASK \
	BCHP_BVNF_INTR2_5_R5F_STATUS_MAD_1_IT_READY_INTR_MASK
#endif
#elif BCHP_BVNF_INTR2_5_R5F_STATUS_MAD_IT_READY_INTR_MASK
#define BVDC_P_MAD_0_IT_READY_INT_MASK \
	BCHP_BVNF_INTR2_5_R5F_STATUS_MAD_IT_READY_INTR_MASK
#else
#error "Port required for VDC"
#endif

BDBG_MODULE(BVDC_MAD);
BDBG_OBJECT_ID(BVDC_MAD);

/***************************************************************************
* Static functions
***************************************************************************/
static void BVDC_P_Mad_Init_Rev32_DynamicDefault
( const BFMT_VideoInfo               *pFmtInfo,
 uint32_t                            ulSampleFactor,
 BVDC_Deinterlace_Reverse32Settings *pReverse32Settings );

static void BVDC_P_Mad_Init_Rev22_DynamicDefault
( const BFMT_VideoInfo               *pFmtInfo,
 BVDC_Deinterlace_Reverse22Settings *pReverse22Settings );

static void BVDC_P_Mad_Init_Chroma_DynamicDefault
( const BFMT_VideoInfo               *pFmtInfo,
 BVDC_MadGameMode                    eGameMode,
 bool                                bMfdSrc,
 bool                                bCvbsSrc,
 BVDC_Deinterlace_ChromaSettings    *pChromaSettings );

/***************************************************************************
* File log utility to debug MAD sw cadence algorithm.
* This needs to be compiled out by default
*/
#ifndef BVDC_P_MAD_DBG_FLOG
#define BVDC_P_MAD_DBG_FLOG                      (0)
#endif
#if BVDC_P_MAD_DBG_FLOG
static uint32_t BVDC_P_Mad_Flog( const char *fmt, ...) ;
#define BVDC_P_MAD_FLOG(x)                       BVDC_P_Mad_Flog x
#else
#define BVDC_P_MAD_FLOG(x)
#endif

#if BVDC_P_MAD_DBG_FLOG
#include <stdio.h>
static FILE *pfMadLog = 0 ;

static uint32_t BVDC_P_Mad_Flog( const char *fmt, ...)
{
	va_list arglist ;
	static uint32_t fopen_failed = 0;

	if (fopen_failed)
	{
		return 0;
	}

	if(pfMadLog==0)
	{
		BDBG_ERR(("Opening Mad.log for write"));
		pfMadLog = fopen("Mad.log", "w") ;
	}

	if ( pfMadLog == 0)
	{
		printf("fopen failed to create Mad.log\n");
		fopen_failed = 1;
	}
	else
	{
		va_start( arglist, fmt ) ;
		vfprintf(pfMadLog, fmt, arglist) ;
		fprintf(pfMadLog, "\n") ;
		va_end(arglist);
	}

	return 0;
}
#endif

/***************************************************************************
* {private}
*
*/
BERR_Code BVDC_P_Mad_Create
( BVDC_P_Mad_Handle            *phMad,
 BREG_Handle                   hRegister,
 BVDC_P_MadId                  eMadId,
 BVDC_P_Resource_Handle        hResource )
{
	BERR_Code  eResult = BERR_SUCCESS;
	BVDC_P_MadContext *pMad;
	uint32_t i;

	BDBG_ENTER(BVDC_P_Mad_Create);

	BDBG_ASSERT(phMad);

	/* (1) Alloc the context. */
	pMad = (BVDC_P_MadContext*)
		(BKNI_Malloc(sizeof(BVDC_P_MadContext)));
	if(!pMad)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pMad, 0x0, sizeof(BVDC_P_MadContext));
	BDBG_OBJECT_SET(pMad, BVDC_MAD);

	pMad->eId              = eMadId;
	pMad->ulRegOffset      = BVDC_P_MAD_GET_REG_OFFSET(eMadId);
	pMad->hRegister        = hRegister;

#if BVDC_P_SUPPORT_MAD_SRC_1080I
	pMad->ulMaxWidth = BFMT_1080I_WIDTH;
	pMad->ulMaxHeight = BFMT_1080I_HEIGHT;
#else
	pMad->ulMaxWidth = BFMT_PAL_WIDTH;
	pMad->ulMaxHeight = BFMT_PAL_HEIGHT;
#endif

	/* Init to the default filter coeffficient tables. */
	BVDC_P_GetFirCoeffs_isr(&pMad->pHorzFirCoeffTbl, NULL);
	BVDC_P_GetChromaFirCoeffs_isr(&pMad->pChromaHorzFirCoeffTbl, NULL);

	/* MAD reset address */
#if BCHP_DMISC_SOFT_RESET_MAD_0_MASK
	pMad->ulResetRegAddr = BCHP_DMISC_SOFT_RESET;
	if (pMad->eId == BVDC_P_MadId_eMad0)
	{
		pMad->ulResetMask = BCHP_DMISC_SOFT_RESET_MAD_0_MASK;
	}
#if (BVDC_P_SUPPORT_MAD > 1)
	else if (pMad->eId == BVDC_P_MadId_eMad1)
	{
		pMad->ulResetMask = BCHP_DMISC_SOFT_RESET_MAD_1_MASK;
	}
#endif
#elif BCHP_MMISC_SOFT_RESET_MAD_0_MASK
	pMad->ulResetRegAddr = BCHP_MMISC_SOFT_RESET;
	pMad->ulResetMask    = BCHP_MMISC_SOFT_RESET_MAD_0_MASK;
#elif BCHP_MMISC_SOFT_RESET_MAD_MASK
	pMad->ulResetRegAddr = BCHP_MMISC_SOFT_RESET;
	pMad->ulResetMask    = BCHP_MMISC_SOFT_RESET_MAD_MASK;
#else
#error "Port required for MAD."
#endif

	for(i = 0; i < BVDC_P_MAD_PIXEL_SD_BUFFER_COUNT; i++)
	{
		pMad->apPixelHeapNode[i] = NULL;
	}

	for(i = 0; i < BVDC_P_MAD_QM_BUFFER_COUNT; i++)
	{
		pMad->apQmHeapNode[i] = NULL;
	}

#if BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED
	eResult = BERR_TRACE(BVDC_P_Hscaler_Create(&pMad->hHscaler,
		pMad->eId, hResource, hRegister));
	if (BERR_SUCCESS != eResult)
	{
		BKNI_Free((void*)pMad);
		return eResult;
	}
#endif

	pMad->usFeedCapture = 0;
	pMad->usCurQm = 0;
	pMad->usGameModeStartDelay = 0;
	pMad->usPixelBufferCnt = 0;

	/* init the SubRul sub-module */
	BVDC_P_SubRul_Init(&(pMad->SubRul), BVDC_P_Mad_MuxAddr(pMad),
		BVDC_P_Mad_PostMuxValue(pMad), BVDC_P_DrainMode_eBack, 0, hResource);

	/* All done. now return the new fresh context to user. */
	*phMad = (BVDC_P_Mad_Handle)pMad;

	BVDC_P_MAD_FLOG(("MAD%d is created", pMad->eId));
	BDBG_LEAVE(BVDC_P_Mad_Create);
	return eResult;
}


/***************************************************************************
* {private}
*
*/
void BVDC_P_Mad_Destroy
( BVDC_P_Mad_Handle             hMad )
{
	BDBG_ENTER(BVDC_P_Mad_Destroy);
	BDBG_OBJECT_ASSERT(hMad, BVDC_MAD);

#if BVDC_P_MAD_DBG_FLOG
	if(pfMadLog)
	{
		BDBG_ERR(("Closing Mad.log..."));
		fclose(pfMadLog) ;
	}
#endif

#if BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED
	if (hMad->hHscaler)
		BVDC_P_Hscaler_Destroy(hMad->hHscaler);
#endif

	BDBG_OBJECT_DESTROY(hMad, BVDC_MAD);
	/* [1] Free the context. */
	BKNI_Free((void*)hMad);

	BDBG_LEAVE(BVDC_P_Mad_Destroy);
	return;
}


/***************************************************************************
* {private}
*
*/
static void BVDC_P_Mad_Init
( BVDC_P_Mad_Handle             hMad )
{
	uint32_t  ulLfsrCtrlT0, ulLfsrCtrlT1, ulLfsrCtrlT2, ulLfsrValue;

	BDBG_ENTER(BVDC_P_Mad_Init);
	BDBG_OBJECT_ASSERT(hMad, BVDC_MAD);

	/* Clear out shadow registers. */
	BKNI_Memset((void*)hMad->aulRegs, 0x0, sizeof(hMad->aulRegs));

	/* Initialize state. */
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_7)
	hMad->bMemSaving    = false;
#endif
	hMad->ulGameModeTransCnt        = 0;
	hMad->bInitial                  = true;
	hMad->ulUpdateAll               = BVDC_P_RUL_UPDATE_THRESHOLD;
	hMad->ulUpdateMisc              = BVDC_P_RUL_UPDATE_THRESHOLD;
	hMad->ulUpdatePullDown          = BVDC_P_RUL_UPDATE_THRESHOLD;
	hMad->ulUpdate22PullDown        = BVDC_P_RUL_UPDATE_THRESHOLD;
	hMad->ulUpdateChromaSettings    = BVDC_P_RUL_UPDATE_THRESHOLD;
	hMad->ulUpdateMotionSettings    = BVDC_P_RUL_UPDATE_THRESHOLD;
	hMad->bEnableOsd                = false;
	hMad->bUsrChanges               = true; /* force to evaluate on first run */
	hMad->ulOsdHpos                 = 0;
	hMad->ulOsdVpos                 = 0;

	hMad->ulPhaseCounter            = 0;
	hMad->ulPhaseCounter22          = 0;
	BKNI_Memset((void*)hMad->alPhase, 0x0, sizeof(hMad->alPhase));
	BKNI_Memset((void*)hMad->alPhase22, 0x0, sizeof(hMad->alPhase22));
	hMad->ulRev22NonMatchMatchRatio = BVDC_P_MAD_NONMATCH_MATCH_RATIO;
	hMad->ulRev22EnterLockLevel     = BVDC_P_MAD_REV22_ENTER_LOCK_LEVEL;
	hMad->ulRev22LocksatLevel       = BVDC_P_MAD_REV22_LOCK_SAT_LEVEL;
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_5)
	BKNI_Memset((void*)&hMad->stRev32Statistics, 0x0, sizeof(hMad->stRev32Statistics));
#endif
	BKNI_Memset((void*)&hMad->stTestFeature1, 0x0, sizeof(BVDC_P_TestFeature1_Settings));

	hMad->ulRev22Candidate      = 0;
	hMad->bReverse22Pulldown    = false;
	hMad->bReverse32Pulldown    = false;
	hMad->bRev32Custom          = false;
	hMad->bRev22Custom          = false;
	hMad->bChromaCustom         = false;
	hMad->bMotionCustom         = false;
	hMad->bOptimizeStill        = false;
	hMad->ulPreFieldMotion      = 0;
	hMad->ulStillFieldNum       = 0;
	hMad->ulChangeFieldNum      = 0;
	hMad->b5FieldMode           = false;
	hMad->ulPccFwd              = 0;
	hMad->ulPrePccFwd           = 0;
	hMad->usTrickModeStartDelay = 0;

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR) /* PR56938: software workaround */
	hMad->ulHardStartCountdown  = BVDC_P_MAD_HARDSTART_COUNTDOWN;
#endif

	/* Dither */
	ulLfsrCtrlT0 = BVDC_P_DITHER_MAD_LFSR_CTRL_T0;
	ulLfsrCtrlT1 = BVDC_P_DITHER_MAD_LFSR_CTRL_T1;
	ulLfsrCtrlT2 = BVDC_P_DITHER_MAD_LFSR_CTRL_T2;
	ulLfsrValue  = BVDC_P_DITHER_MAD_LFSR_VALUE;
	BVDC_P_Dither_Init(&hMad->stDither, ulLfsrCtrlT0,
		ulLfsrCtrlT1, ulLfsrCtrlT2, ulLfsrValue);

	/* the followings come from architecture group (MAD_shortlock.bss),
	to overwrite reset values. Not changed at runtime. */
	BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_0) = 0x4006005;
	BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_1) = 0x27100bb;
	BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_2) = 0xd20500;

	BVDC_P_MAD_GET_REG_DATA(MAD_0_XCHROMA_CONTROL_0) =
		BCHP_FIELD_DATA(MAD_0_XCHROMA_CONTROL_0, SIMILAR_MAX,      0xff) |
		BCHP_FIELD_DATA(MAD_0_XCHROMA_CONTROL_0, MAX_XCHROMA,      0xff) |
		BCHP_FIELD_DATA(MAD_0_XCHROMA_CONTROL_0, CHROMA_MA_OFFSET, 0   );

#if (BVDC_P_SUPPORT_MAD_VER < BVDC_P_MAD_VER_2)
	BVDC_P_MAD_GET_REG_DATA(MAD_0_XCHROMA_CONTROL_1) =  0x1b101810;
	BVDC_P_MAD_GET_REG_DATA(MAD_0_PPC_CONTROL) =  0x30003;

	/* enable all field stores of MAD32 */
	BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) =  (
		BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_G_ENABLE, ON) |
		BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_J_ENABLE, ON) |
		BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_B_ENABLE, ON) |
		BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_D_ENABLE, ON) |
		BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, PIXEL_CAP_ENABLE,      ON) |
		BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_K_ENABLE, ON) |
		BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_L_ENABLE, ON) |
		BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_M_ENABLE, ON) );

	BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_OUTPUT_CONTROL) =  0xa1;
#else /* phase >= 2 */
	BVDC_P_MAD_GET_REG_DATA(MAD_0_PPC_CONTROL_0) =  0x30003;

	BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_4) =
		BCHP_FIELD_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_4, REV22_LOCK_SAT_LEVEL,    32) |
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
		BCHP_FIELD_ENUM(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_4, AUTOPP_BWV_ENABLE,      OFF) |
		BCHP_FIELD_ENUM(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_4, PCC_BWV_DETECT_ENABLE,  ON) |
#endif /* phase >= 3 */
		BCHP_FIELD_ENUM(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_4, BAD_EDIT_DETECT_ENABLE,  ON) |
		BCHP_FIELD_ENUM(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_4, BAD_WEAVE_DETECT_ENABLE, ON);

	BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_5) = 0x27101d4;

	BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_6) =
		BCHP_FIELD_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_6, BAD_WEAVE_DETECT_THRESH,     0x3fff) |
		BCHP_FIELD_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_6, BAD_WEAVE_DETECT_PH4_THRESH, 0x3fff);

	BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_7) =
		BCHP_FIELD_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_7, BAD_WEAVE_UNLOCK_THRESH,     0x3fff) |
		BCHP_FIELD_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_7, BAD_WEAVE_UNLOCK_PH4_THRESH, 0x3fff);

	BVDC_P_MAD_GET_REG_DATA(MAD_0_QM_MAPPING_RANGE) =  0x40201000;
	BVDC_P_MAD_GET_REG_DATA(MAD_0_MH_MAPPING_VALUE) =  0xffff2000;

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
	BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_8) =
		BCHP_FIELD_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_8, REV32_MIN_SIGMA_RANGE, 0x500) |
		BCHP_FIELD_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_8, REV32_LOCK_SAT_THRESH, 0xd2 );

	BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_0) =
		BCHP_FIELD_DATA(MAD_0_BWV_CONTROL_0, LUMA_CORING_THRESH, 4) |
		BCHP_FIELD_DATA(MAD_0_BWV_CONTROL_0, LUMA_22_THRESH,  0x96);

	BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_2) =
		BCHP_FIELD_DATA(MAD_0_BWV_CONTROL_2, CHROMA_CORING_THRESH, 4) |
		BCHP_FIELD_DATA(MAD_0_BWV_CONTROL_2, CHROMA_22_THRESH, 0x19c8);

	BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_3) =
		BCHP_FIELD_DATA(MAD_0_BWV_CONTROL_3, CHROMA_32_THRESH, 0x1c20) |
		BCHP_FIELD_DATA(MAD_0_BWV_CONTROL_3, CHROMA_32_AVG_THRESH, 0x19c8);

	BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_4) =
		BCHP_FIELD_DATA(MAD_0_BWV_CONTROL_4, TKR_CORING_THRESH, 0x62);

	BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_5) =
		BCHP_FIELD_DATA(MAD_0_BWV_CONTROL_5, TKR_PCC_MULT, 0xffff) |
		BCHP_FIELD_DATA(MAD_0_BWV_CONTROL_5, TKR_MIN_REPF_VETO_LEVEL, 0x80);

	/* Note: this block of registers are not really programmed by RUL for now since the reset
	values are the desired default; we still explicitly intitialize soft copy here.
	We may add these into the "update all" RUL in future if fine tuning finds out different
	settings are required. */
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_4)
	BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_REPF_HISTOGRAM_THRESH_0) =
		BCHP_FIELD_DATA(MAD_0_IT_REPF_HISTOGRAM_THRESH_0, BIN_0, 4) |
		BCHP_FIELD_DATA(MAD_0_IT_REPF_HISTOGRAM_THRESH_0, BIN_1, 8) |
		BCHP_FIELD_DATA(MAD_0_IT_REPF_HISTOGRAM_THRESH_0, BIN_2, 16) |
		BCHP_FIELD_DATA(MAD_0_IT_REPF_HISTOGRAM_THRESH_0, BIN_3, 32);

	BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_REPF_HISTOGRAM_THRESH_1) =
		BCHP_FIELD_DATA(MAD_0_IT_REPF_HISTOGRAM_THRESH_1, BIN_4, 64);

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_6)
	BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_6) =
		BCHP_FIELD_DATA(MAD_0_BWV_CONTROL_6, LUMA_PCC_BOUND, 150) |
		BCHP_FIELD_DATA(MAD_0_BWV_CONTROL_6, CORING_SHIFT,          2) |
		BCHP_FIELD_DATA(MAD_0_BWV_CONTROL_6, WEAVE_THRESHOLD,   4) |
		BCHP_FIELD_DATA(MAD_0_BWV_CONTROL_6, PCC_THRESHOLD,       4);

	BVDC_P_MAD_GET_REG_DATA(MAD_0_SCENE_CHANGE) =
		BCHP_FIELD_DATA(MAD_0_SCENE_CHANGE, SCENE_CHANGE_THRESHOLD, 16) |
#if BVDC_P_MAD_SCENE_CHANGE_WORKAROUND
		/* effectively disable hw scene change detection for now and resort to SW workaround; */
		BCHP_FIELD_DATA(MAD_0_SCENE_CHANGE, SAME_SCENE_THRESHOLD,     0) |
		BCHP_FIELD_DATA(MAD_0_SCENE_CHANGE, DIFF_SCENE_THRESHOLD,      0xff) ;
#else
		BCHP_FIELD_DATA(MAD_0_SCENE_CHANGE, SAME_SCENE_THRESHOLD,     16) |
		BCHP_FIELD_DATA(MAD_0_SCENE_CHANGE, DIFF_SCENE_THRESHOLD,      32) ;
#endif

	BVDC_P_MAD_GET_REG_DATA(MAD_0_LUMA_BOUND) =
		BCHP_FIELD_DATA(MAD_0_LUMA_BOUND, LUMA_BOUND_4,  120) |
		BCHP_FIELD_DATA(MAD_0_LUMA_BOUND, LUMA_BOUND_3,  110) |
		BCHP_FIELD_DATA(MAD_0_LUMA_BOUND, LUMA_BOUND_2,  100) |
		BCHP_FIELD_DATA(MAD_0_LUMA_BOUND, LUMA_BOUND_1,   90) ;

	BVDC_P_MAD_GET_REG_DATA(MAD_0_MOTION_THRESHOLD) =
		BCHP_FIELD_DATA(MAD_0_MOTION_THRESHOLD, DIFF_THRESHOLD,  50) |
		BCHP_FIELD_DATA(MAD_0_MOTION_THRESHOLD, M_THRESHOLD_5,    8) |
		BCHP_FIELD_DATA(MAD_0_MOTION_THRESHOLD, M_THRESHOLD_4,    6) |
		BCHP_FIELD_DATA(MAD_0_MOTION_THRESHOLD, M_THRESHOLD_3,    4) |
		BCHP_FIELD_DATA(MAD_0_MOTION_THRESHOLD, M_THRESHOLD_2,    2) |
		BCHP_FIELD_DATA(MAD_0_MOTION_THRESHOLD, M_THRESHOLD_1,    1) ;

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_7)
	BVDC_P_MAD_GET_REG_DATA(MAD_0_SCHG_MOTION_THRESHOLD_0) =
		BCHP_FIELD_DATA(MAD_0_SCHG_MOTION_THRESHOLD_0, SCHG_M_THRESHOLD_3, 4) |
		BCHP_FIELD_DATA(MAD_0_SCHG_MOTION_THRESHOLD_0, SCHG_M_THRESHOLD_2, 4) |
		BCHP_FIELD_DATA(MAD_0_SCHG_MOTION_THRESHOLD_0, SCHG_M_THRESHOLD_1, 8) |
		BCHP_FIELD_DATA(MAD_0_SCHG_MOTION_THRESHOLD_0, SCHG_M_THRESHOLD_0, 8);
	BVDC_P_MAD_GET_REG_DATA(MAD_0_SCHG_MOTION_THRESHOLD_1) =
		BCHP_FIELD_DATA(MAD_0_SCHG_MOTION_THRESHOLD_1, SCHG_MF_ENABLE,     1) |
		BCHP_FIELD_DATA(MAD_0_SCHG_MOTION_THRESHOLD_1, SCHG_2_MS_ENABLE,   1) |
		BCHP_FIELD_DATA(MAD_0_SCHG_MOTION_THRESHOLD_1, SCHG_2_MTF_ENABLE,  1) |
		BCHP_FIELD_DATA(MAD_0_SCHG_MOTION_THRESHOLD_1, SCHG_5_QK_ENABLE,   1) |
		BCHP_FIELD_DATA(MAD_0_SCHG_MOTION_THRESHOLD_1, SCHG_0_QK_ENABLE,   1) |
		BCHP_FIELD_DATA(MAD_0_SCHG_MOTION_THRESHOLD_1, SCHG_M_THRESHOLD_5, 8) |
		BCHP_FIELD_DATA(MAD_0_SCHG_MOTION_THRESHOLD_1, SCHG_M_THRESHOLD_4, 8);
	BVDC_P_MAD_GET_REG_DATA(MAD_0_MOTION_CAL_CTRL) =
		BCHP_FIELD_DATA(MAD_0_MOTION_CAL_CTRL, MTF_LUMA_WIN_SIZE,    2) |
		BCHP_FIELD_DATA(MAD_0_MOTION_CAL_CTRL, MTC_LUMA_WIN_SIZE,    2) |
		BCHP_FIELD_DATA(MAD_0_MOTION_CAL_CTRL, BLEND_NO_BAD_WEAVE,   2) |
		BCHP_FIELD_DATA(MAD_0_MOTION_CAL_CTRL, PCC_BAD_WEAVE_DIAG,   1) |
		BCHP_FIELD_DATA(MAD_0_MOTION_CAL_CTRL, PCC_BAD_WEAVE_CHROMA, 1) |
		BCHP_FIELD_DATA(MAD_0_MOTION_CAL_CTRL, NORMAL_WEAVE_DIREC,   0) |
		BCHP_FIELD_DATA(MAD_0_MOTION_CAL_CTRL, NORMAL_NO_BAD_WEAVE,  2) |
		BCHP_FIELD_DATA(MAD_0_MOTION_CAL_CTRL, MS_3548,              1) |
		BCHP_FIELD_DATA(MAD_0_MOTION_CAL_CTRL, MT_3548,              1) |
		BCHP_FIELD_DATA(MAD_0_MOTION_CAL_CTRL, MTF_CHROMA_SFT,       1) |
		BCHP_FIELD_DATA(MAD_0_MOTION_CAL_CTRL, MTC_CHROMA_SFT,       1) |
		BCHP_FIELD_DATA(MAD_0_MOTION_CAL_CTRL, MS_CHROMA_SFT,        1) |
		BCHP_FIELD_DATA(MAD_0_MOTION_CAL_CTRL, MT_CHROMA_SFT,        1);
	BVDC_P_MAD_GET_REG_DATA(MAD_0_FAST_MOTION_CTRL) =
		BCHP_FIELD_DATA(MAD_0_FAST_MOTION_CTRL, SCHG_FAST_FORCE_BOB,    1) |
		BCHP_FIELD_DATA(MAD_0_FAST_MOTION_CTRL, FAST_MOTION_THRESHOLD,  1) |
		BCHP_FIELD_DATA(MAD_0_FAST_MOTION_CTRL, FAST_CHROMA_THRESHOLD,  8) |
		BCHP_FIELD_DATA(MAD_0_FAST_MOTION_CTRL, FAST_LUMA_THRESHOLD,   64);
#else
	BVDC_P_MAD_GET_REG_DATA(MAD_0_SCHG_MOTION_THRESHOLD) =
		BCHP_FIELD_DATA(MAD_0_SCHG_MOTION_THRESHOLD, SCHG_M_THRESHOLD_5,  8) |
		BCHP_FIELD_DATA(MAD_0_SCHG_MOTION_THRESHOLD, SCHG_M_THRESHOLD_4,  8) |
		BCHP_FIELD_DATA(MAD_0_SCHG_MOTION_THRESHOLD, SCHG_M_THRESHOLD_3,  4) |
		BCHP_FIELD_DATA(MAD_0_SCHG_MOTION_THRESHOLD, SCHG_M_THRESHOLD_2,  4) |
		BCHP_FIELD_DATA(MAD_0_SCHG_MOTION_THRESHOLD, SCHG_M_THRESHOLD_1,  8) |
		BCHP_FIELD_DATA(MAD_0_SCHG_MOTION_THRESHOLD, SCHG_M_THRESHOLD_0,  8) ;
#endif /* phase >= 7 */
#endif /* phase >= 6 */
#endif /* phase >= 4 */
#endif /* phase >= 3 */
#endif /* phase >= 2 */

#if (!BVDC_P_SUPPORT_HSCL) && (!BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED)
	/* clear horizontal panscan */
	BVDC_P_MAD_GET_REG_DATA(MAD_0_SRC_PIC_HORIZ_PAN_SCAN) &= ~(
		BCHP_MASK(MAD_0_SRC_PIC_HORIZ_PAN_SCAN, OFFSET));

	/* Always use Horizontal Scaling! */
	BVDC_P_MAD_GET_REG_DATA(MAD_0_HORIZ_CONTROL) &= ~(
		BCHP_MASK(MAD_0_HORIZ_CONTROL, FIR_ENABLE));
	BVDC_P_MAD_GET_REG_DATA(MAD_0_HORIZ_CONTROL) |=  (
		BCHP_FIELD_ENUM(MAD_0_HORIZ_CONTROL, FIR_ENABLE, ON  ));

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_2)
	BVDC_P_MAD_GET_REG_DATA(MAD_0_HORIZ_CONTROL) &= ~(
		BCHP_MASK(MAD_0_HORIZ_CONTROL, STALL_DRAIN_ENABLE));
	BVDC_P_MAD_GET_REG_DATA(MAD_0_HORIZ_CONTROL) |=  (
		BCHP_FIELD_ENUM(MAD_0_HORIZ_CONTROL, STALL_DRAIN_ENABLE, ON ));

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_4)
	BVDC_P_MAD_GET_REG_DATA(MAD_0_HORIZ_CONTROL) &=  ~(
		BCHP_MASK(MAD_0_HORIZ_CONTROL, MASK_HSCL_LONG_LINE ) |
		BCHP_MASK(MAD_0_HORIZ_CONTROL, MASK_HSCL_SHORT_LINE ));
	BVDC_P_MAD_GET_REG_DATA(MAD_0_HORIZ_CONTROL) |=  (
		BCHP_FIELD_ENUM(MAD_0_HORIZ_CONTROL, MASK_HSCL_LONG_LINE, OFF ) |
		BCHP_FIELD_ENUM(MAD_0_HORIZ_CONTROL, MASK_HSCL_SHORT_LINE, OFF ));
#endif /* phase >= 4 */
#endif /* phase >= 2 */
#endif /* (!BVDC_P_SUPPORT_HSCL) && (!BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED)
		*/

	/* clear MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2 */
	BVDC_P_MAD_GET_REG_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2) =  (
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_2)
		BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2, IT_PPBWV_ENABLE, OFF) |
#endif
		BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2, IT_RF_SEL ,RF_OFF) |
		BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2, IT_HL_PAT_SEL , 0) |
		BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2, IT_PPUFM_ENABLE , OFF) ) ;

#if (BVDC_P_SUPPORT_MAD_VER == BVDC_P_MAD_VER_6)
	BVDC_P_MAD_GET_REG_DATA(MAD_0_DITHER_CTRL) &=  ~(
		BCHP_MASK(MAD_0_DITHER_CTRL, MODE      ) |
		BCHP_MASK(MAD_0_DITHER_CTRL, OFFSET_CH1) |
		BCHP_MASK(MAD_0_DITHER_CTRL, SCALE_CH1 ) |
		BCHP_MASK(MAD_0_DITHER_CTRL, OFFSET_CH0) |
		BCHP_MASK(MAD_0_DITHER_CTRL, SCALE_CH0 ));
	BVDC_P_MAD_GET_REG_DATA(MAD_0_DITHER_CTRL) |=  (
		BCHP_FIELD_DATA(MAD_0_DITHER_CTRL, MODE,       hMad->stDither.ulMode     ) |
		BCHP_FIELD_DATA(MAD_0_DITHER_CTRL, OFFSET_CH1, hMad->stDither.ulCh1Offset) |
		BCHP_FIELD_DATA(MAD_0_DITHER_CTRL, SCALE_CH1,  hMad->stDither.ulCh1Scale ) |
		BCHP_FIELD_DATA(MAD_0_DITHER_CTRL, OFFSET_CH0, hMad->stDither.ulCh0Offset) |
		BCHP_FIELD_DATA(MAD_0_DITHER_CTRL, SCALE_CH0,  hMad->stDither.ulCh0Scale ));

	BVDC_P_MAD_GET_REG_DATA(MAD_0_DITHER_LFSR_INIT) &= ~(
		BCHP_MASK(MAD_0_DITHER_LFSR_INIT, SEQ) |
		BCHP_MASK(MAD_0_DITHER_LFSR_INIT, VALUE));
	BVDC_P_MAD_GET_REG_DATA(MAD_0_DITHER_LFSR_INIT) |=  (
		BCHP_FIELD_DATA(MAD_0_DITHER_LFSR_INIT, SEQ,   hMad->stDither.ulLfsrSeq  ) |
		BCHP_FIELD_DATA(MAD_0_DITHER_LFSR_INIT, VALUE, hMad->stDither.ulLfsrValue));

	BVDC_P_MAD_GET_REG_DATA(MAD_0_DITHER_LFSR_CTRL) &= ~(
		BCHP_MASK(MAD_0_DITHER_LFSR_CTRL, T0) |
		BCHP_MASK(MAD_0_DITHER_LFSR_CTRL, T1) |
		BCHP_MASK(MAD_0_DITHER_LFSR_CTRL, T2));
	BVDC_P_MAD_GET_REG_DATA(MAD_0_DITHER_LFSR_CTRL) |=  (
		BCHP_FIELD_DATA(MAD_0_DITHER_LFSR_CTRL, T0, hMad->stDither.ulLfsrCtrlT0) |
		BCHP_FIELD_DATA(MAD_0_DITHER_LFSR_CTRL, T1, hMad->stDither.ulLfsrCtrlT1) |
		BCHP_FIELD_DATA(MAD_0_DITHER_LFSR_CTRL, T2, hMad->stDither.ulLfsrCtrlT2));
#endif

#if BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED
	BVDC_P_Hscaler_Init(hMad->hHscaler);
#endif

	BDBG_LEAVE(BVDC_P_Mad_Init);
	return;
}


/***************************************************************************
* {private}
*
* BVDC_P_Mad_AcquireConnect
*
* It is called by BVDC_Window_Validate after changing from diable mad to
* enable mad, .
*/
BERR_Code BVDC_P_Mad_AcquireConnect
( BVDC_P_Mad_Handle                 hMad,
 BVDC_Heap_Handle                  hHeap,
 BVDC_P_WindowId                   eWinId)
{
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Mad_AcquireConnect);

	hMad->eWinId = eWinId;
	hMad->hHeap = hHeap;

	/* init mad */
	BVDC_P_Mad_Init(hMad);

	BDBG_LEAVE(BVDC_P_Mad_AcquireConnect);
	return BERR_TRACE(eResult);
}

/***************************************************************************
* {private}
*
* BVDC_P_Mad_ReleaseConnect_isr
*
* It is called after window decided that mad is no-longer used by HW in its
* vnet mode (i.e. it is really shut down and teared off from vnet).
*/
BERR_Code BVDC_P_Mad_ReleaseConnect_isr
( BVDC_P_Mad_Handle         *phMad )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Mad_ReleaseConnect_isr);

	/* handle validation */
	if (NULL != *phMad)
	{
		BDBG_OBJECT_ASSERT(*phMad, BVDC_MAD);
		BDBG_ASSERT(0 == (*phMad)->SubRul.ulWinsActFlags);

		BVDC_P_Resource_ReleaseHandle_isr(
			BVDC_P_SubRul_GetResourceHandle_isr(&(*phMad)->SubRul),
			BVDC_P_ResourceType_eMad, (void *)(*phMad));

		(*phMad)->eWinId = BVDC_P_WindowId_eUnknown;

		/* this makes win to stop calling mad code */
		*phMad = NULL;
	}

	BDBG_LEAVE(BVDC_P_Mad_ReleaseConnect_isr);
	return BERR_TRACE(eResult);
}

/***************************************************************************
* {private}
*
* BVDC_P_Mad_SetVnetAllocBuf_isr
*
* Called by BVDC_P_*_BuildRul_isr to setup for joinning into vnet (including
* optionally acquiring loop-back) and allocate buffers
*/
void BVDC_P_Mad_SetVnetAllocBuf_isr
( BVDC_P_Mad_Handle          hMad,
 uint32_t                   ulSrcMuxValue,
 BVDC_P_VnetPatch           eVnetPatchMode,
 BVDC_P_BufferHeapId        eMadPixelBufHeapId,
 BVDC_P_BufferHeapId        eMadQmBufHeapId,
 uint16_t                   usPixelBufferCnt )
{
	BERR_Code  eResult = BERR_SUCCESS;
	int ii;
	uint32_t ulDeviceAddr, ulSize;

	BDBG_ENTER(BVDC_P_Mad_SetVnetAllocBuf_isr);
	BDBG_MSG(("SetVnet for mad"));
	BDBG_OBJECT_ASSERT(hMad, BVDC_MAD);

	/* set up for joining vnet, including acquiring loop-back */
	BVDC_P_SubRul_SetVnet_isr(&(hMad->SubRul), ulSrcMuxValue, eVnetPatchMode);

	/* allocate memory buffers */
	BDBG_ASSERT(NULL == hMad->apPixelHeapNode[0]);
	BDBG_ASSERT(NULL == hMad->apQmHeapNode[0]);

	hMad->usPixelBufferCnt = usPixelBufferCnt;

	/* Allocate pixel field buffers */
	BVDC_P_BUF_MSG(("Mad[%d] alloc %d pixel field buffers (%s)",
		hMad->eId, hMad->usPixelBufferCnt,
		BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(eMadPixelBufHeapId)));
	eResult = BVDC_P_BufferHeap_AllocateBuffers_isr(hMad->hHeap,
		hMad->apPixelHeapNode, usPixelBufferCnt, false,
		eMadPixelBufHeapId, BVDC_P_BufferHeapId_eUnknown);
	/* Not enough memory, dump out configuration */
	if(eResult == BERR_OUT_OF_DEVICE_MEMORY)
	{
		BDBG_ERR(("MAD[%d] Not enough memory for MAD pixel field buffers! Configuration:", hMad->eId));
		BDBG_ERR(("MAD[%d] usPixelBufferCnt: %d", hMad->eId, hMad->usPixelBufferCnt));
	}
	BDBG_ASSERT(BERR_SUCCESS == eResult);

	/* Allocate QM field buffers */
	BVDC_P_BUF_MSG(("Mad[%d] alloc %d QM filed buffers (%s)",
		hMad->eId, BVDC_P_MAD_QM_BUFFER_COUNT,
		BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(eMadQmBufHeapId)));
	eResult = BVDC_P_BufferHeap_AllocateBuffers_isr(hMad->hHeap,
		hMad->apQmHeapNode, BVDC_P_MAD_QM_BUFFER_COUNT, false,
		eMadQmBufHeapId, BVDC_P_BufferHeapId_eUnknown);
	/* Not enough memory, dump out configuration */
	if(eResult == BERR_OUT_OF_DEVICE_MEMORY)
	{
		BDBG_ERR(("MAD[%d] Not enough memory for MAD QM field buffers! Configuration:", hMad->eId));
		BDBG_ERR(("MAD[%d] BufferCnt: %d", hMad->eId, BVDC_P_MAD_QM_BUFFER_COUNT));
	}
	BDBG_ASSERT(BERR_SUCCESS == eResult);

	for(ii = 0; ii < BVDC_P_MAD_PIXEL_SD_BUFFER_COUNT; ii++)
	{
		BVDC_P_MAD_GET_REG_DATA_I(MAD_0_PIXEL_FIELD_MSTART_1, ii) = 0;
	}

	for(ii = 0; ii < BVDC_P_MAD_QM_FIELD_STORE_COUNT; ii++)
	{
		BVDC_P_MAD_GET_REG_DATA_I(MAD_0_QM_FIELD_MSTART_4, ii) = 0;
	}

	/* write buf addr into hw reg (indeed the shadow for now) */
	for(ii = 0; ii < usPixelBufferCnt; ii++)
	{
		BVDC_P_MAD_GET_REG_DATA_I(MAD_0_PIXEL_FIELD_MSTART_1, ii) =
			BVDC_P_BUFFERHEAP_GetDeviceOffset(hMad->apPixelHeapNode[ii]);
	}

	ulDeviceAddr = BVDC_P_BUFFERHEAP_GetDeviceOffset(
		hMad->apQmHeapNode[0]);
	ulSize = hMad->apQmHeapNode[0]->pHeapInfo->ulBufSize / BVDC_P_MAD_QM_FIELD_STORE_COUNT;

	for(ii = 0; ii < BVDC_P_MAD_QM_FIELD_STORE_COUNT; ii++)
	{
		BVDC_P_MAD_GET_REG_DATA_I(MAD_0_QM_FIELD_MSTART_4, ii) = ulDeviceAddr;

		/* Make sure address is 32 byte alligned */
		ulDeviceAddr += ulSize;
		ulDeviceAddr = (ulDeviceAddr + 31) & (0xffffffe0);
	}

	BDBG_LEAVE(BVDC_P_Mad_SetVnetAllocBuf_isr);
}

/***************************************************************************
* {private}
*
* BVDC_P_Mad_UnsetVnetFreeBuf_isr
*
* called by BVDC_P_Window_UnsetWriter(Reader)Vnet_isr to to release the
* potentially used loop-back, and free buffers
*/
void BVDC_P_Mad_UnsetVnetFreeBuf_isr
( BVDC_P_Mad_Handle          hMad )
{
	BDBG_ENTER(BVDC_P_Mad_UnsetVnetFreeBuf_isr);
	BDBG_MSG(("Un-SetVnet for mad"));
	BDBG_OBJECT_ASSERT(hMad, BVDC_MAD);

	/* release free-channel or loop-back */
	BVDC_P_SubRul_UnsetVnet_isr(&(hMad->SubRul));

	/* Release memory for MAD if allocated */
	if(NULL != hMad->apPixelHeapNode[0])
	{
		BVDC_P_BUF_MSG(("Mad[%d] free  %d buffers (%s)", hMad->eId, hMad->usPixelBufferCnt,
			BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hMad->apPixelHeapNode[0]->pHeapInfo->eBufHeapId)));

		BVDC_P_BufferHeap_FreeBuffers_isr(hMad->hHeap, hMad->apPixelHeapNode,
			hMad->usPixelBufferCnt, false);
		hMad->apPixelHeapNode[0] = NULL;
	}

	if(NULL != hMad->apQmHeapNode[0])
	{
		BVDC_P_BUF_MSG(("Mad[%d] free  %d buffers (%s)", hMad->eId, BVDC_P_MAD_QM_BUFFER_COUNT,
			BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hMad->apQmHeapNode[0]->pHeapInfo->eBufHeapId)));

		BVDC_P_BufferHeap_FreeBuffers_isr(hMad->hHeap, hMad->apQmHeapNode,
			BVDC_P_MAD_QM_BUFFER_COUNT, false);
		hMad->apQmHeapNode[0] = NULL;
	}

	hMad->usFeedCapture = 0;
	hMad->usCurQm = 0;

	BDBG_LEAVE(BVDC_P_Mad_UnsetVnetFreeBuf_isr);
}


/***************************************************************************
* {private}
*
* BVDC_P_Mad_BuildRul_DrainVnet_isr
*
* called by BVDC_P_Mad_BuildRul_isr after resetting to drain the module and
* its pre-patch FreeCh or LpBack
*/
static void BVDC_P_Mad_BuildRul_DrainVnet_isr
( BVDC_P_Mad_Handle              hMad,
 BVDC_P_ListInfo               *pList )
{
	uint32_t  ulChnResetReg = 0, ulChnResetMask = 0;
	BDBG_OBJECT_ASSERT(hMad, BVDC_MAD);

	/* reset sub and connect the module to a drain */
	BVDC_P_SubRul_StartDrain_isr(&(hMad->SubRul), pList,
		hMad->ulResetRegAddr, hMad->ulResetMask);

	/* drian the  module */
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MAD_0_ENABLE_CONTROL, hMad->ulRegOffset,
		BCHP_FIELD_ENUM(MAD_0_ENABLE_CONTROL, ENABLE, ON));
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MAD_0_ENABLE_CONTROL, hMad->ulRegOffset,
		BCHP_FIELD_ENUM(MAD_0_ENABLE_CONTROL, ENABLE, OFF));

	/* MAD dithers only if not memory saving mode (ANR is OFF); need to disable
	* it when MAD is disabled;
	* Note dither is OFF by reset/drain process; Reset dither mode is ROUNDING; */

	/* disconnect the module from the drain; reset it again; drain
	* the free-channel or loop-back used to to patch the vnet; and
	* also reset the channel ping-pong buf of the scaler and the patcher. */
#ifdef BCHP_MMISC_VNET_B_CHANNEL_RESET
	ulChnResetReg  = BCHP_MMISC_VNET_B_CHANNEL_RESET;
	ulChnResetMask = BCHP_MMISC_VNET_B_CHANNEL_RESET_MAD_0_RESET_MASK <<
		(hMad->eId - BVDC_P_MadId_eMad0);
#endif
	BVDC_P_SubRul_FinishDrain_isr(&(hMad->SubRul), pList,
		hMad->ulResetRegAddr, hMad->ulResetMask, ulChnResetReg, ulChnResetMask);
}

/***************************************************************************
* {private}
*
*/
void BVDC_P_Mad_BuildRul_isr
( const BVDC_P_Mad_Handle       hMad,
 BVDC_P_ListInfo              *pList,
 BVDC_P_State                  eVnetState,
 BVDC_P_PicComRulInfo         *pPicComRulInfo )
{
	uint32_t  ulRulOpsFlags;
#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR)
	BVDC_P_TestFeature1_Settings    *pTestFeature1;
#endif

	BDBG_ENTER(BVDC_P_Mad_BuildRul_isr);
	BDBG_OBJECT_ASSERT(hMad, BVDC_MAD);

	/* currently this is only for vnet building / tearing-off*/

	ulRulOpsFlags = BVDC_P_SubRul_GetOps_isr(
		&(hMad->SubRul), pPicComRulInfo->eWin, eVnetState, pList->bLastExecuted);

	if ((0 == ulRulOpsFlags) ||
		(ulRulOpsFlags & BVDC_P_RulOp_eReleaseHandle))
		return;
	else if (ulRulOpsFlags & BVDC_P_RulOp_eDisable)
	{
		BVDC_P_SubRul_DropOffVnet_isr(&(hMad->SubRul), pList);
		BVDC_P_Mad_SetEnable_isr(hMad, false);
		BVDC_P_MAD_WRITE_TO_RUL(MAD_0_ENABLE_CONTROL, pList->pulCurrent);
#if BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED
		BVDC_P_Hscaler_SetEnable_isr(hMad->hHscaler, false);
		BVDC_P_Hscaler_BuildRul_SetEnable_isr(hMad->hHscaler, pList);
#endif
	}

	/* If rul failed to execute last time we'd re reprogrammed possible
	* missing registers. */
	if((!pList->bLastExecuted) || (hMad->bInitial))
	{
		hMad->ulUpdateAll      = BVDC_P_RUL_UPDATE_THRESHOLD;
		hMad->ulUpdateMisc     = BVDC_P_RUL_UPDATE_THRESHOLD;
		hMad->ulUpdatePullDown = BVDC_P_RUL_UPDATE_THRESHOLD;
		hMad->ulUpdate22PullDown     = BVDC_P_RUL_UPDATE_THRESHOLD;
		hMad->ulUpdateChromaSettings = BVDC_P_RUL_UPDATE_THRESHOLD;
		hMad->ulUpdateMotionSettings = BVDC_P_RUL_UPDATE_THRESHOLD;
	}

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR) /* PR56938: SW workaround */
	if(hMad->ulHardStartCountdown && pList->bLastExecuted)
	{
		hMad->ulHardStartCountdown--;
	}
#endif

	/* reset MAD */
	if(hMad->bInitial)
	{
#if BVDC_P_MAD_TEST_FEATURE_FORMAT_CHANGE_WORKAROUND
		if(hMad->stTestFeature1.bEnable)
		{
			BVDC_P_BUILD_RESET(pList->pulCurrent,
				hMad->ulResetRegAddr, hMad->ulResetMask);
		}
#endif
		hMad->bInitial = false;
	}

	if (ulRulOpsFlags & BVDC_P_RulOp_eEnable)
	{
		/* game mode */
		if(BVDC_P_MAD_COMPARE_FIELD_NAME(MAD_0_MODE_CONTROL_0, FIELD_STATE_UPDATE_SEL_0, CPU_OR_RDMA))
		{
			BVDC_P_MAD_BLOCK_WRITE_TO_RUL(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2, pList->pulCurrent);
			if(hMad->bReverse32Pulldown &&
				((hMad->eGameMode == BVDC_MadGameMode_e5Fields_1Delay) ||
				(hMad->eGameMode == BVDC_MadGameMode_e4Fields_1Delay)||
				(hMad->eGameMode == BVDC_MadGameMode_e3Fields_1Delay)))
			{
				BVDC_P_MAD_WRITE_TO_RUL(MAD_0_IT_OUTPUT_CONTROL, pList->pulCurrent);
			}
		}

		/* TODO: group registers to take advantage of block write */
		if(hMad->ulUpdateAll)
		{
			BDBG_MSG(("MAD32 update RUL"));
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_IT_OUTPUT_CONTROL, pList->pulCurrent);
			/* necessary to turn off forced weave mode when game mode is turned off */
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2, pList->pulCurrent);
			BVDC_P_MAD_BLOCK_WRITE_TO_RUL(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_0, MAD_0_IT_FIELD_PHASE_CALC_CONTROL_2, pList->pulCurrent);

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR) && (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER <= 2)
			/* MAD_SIOB_0_MODE */
			BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MAD_SIOB_0_MODE, 0,
				/* 2 - SEL_MAD_MCTF; 0 - SEL_MAD_ONLY; */
				BCHP_FIELD_DATA(MAD_SIOB_0_MODE, MAD_MCTF_SEL, hMad->bMemSaving? 2:0 ));
#endif

#if (BVDC_P_SUPPORT_MAD_VER < BVDC_P_MAD_VER_2)
			BVDC_P_MAD_BLOCK_WRITE_TO_RUL(MAD_0_XCHROMA_CONTROL_0, MAD_0_DEST_PIC_SIZE, pList->pulCurrent);
#else
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_PPC_CONTROL_0, pList->pulCurrent);
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_SRC_PIC_SIZE, pList->pulCurrent);
#if (!BVDC_P_SUPPORT_HSCL) && (!BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED)
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_DEST_PIC_SIZE, pList->pulCurrent);
#endif
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_CONSTANT_COLOR, pList->pulCurrent);
			BVDC_P_MAD_BLOCK_WRITE_TO_RUL(MAD_0_PIXEL_FIELD_MSTART_1, MAD_0_QM_FIELD_MSTART_11, pList->pulCurrent);
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_QM_MAPPING_RANGE, pList->pulCurrent);
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_MH_MAPPING_VALUE, pList->pulCurrent);
			BVDC_P_MAD_BLOCK_WRITE_TO_RUL(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_4, MAD_0_IT_FIELD_PHASE_CALC_CONTROL_7, pList->pulCurrent);
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_OSD_POSITION, pList->pulCurrent);
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_XCHROMA_MODE, pList->pulCurrent);
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_XCHROMA_CONTROL_0, pList->pulCurrent);
			BVDC_P_MAD_BLOCK_WRITE_TO_RUL(MAD_0_BWV_CONTROL_0, MAD_0_BWV_CONTROL_5, pList->pulCurrent);
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_0, pList->pulCurrent);
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_1, pList->pulCurrent);
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_2, pList->pulCurrent);
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_8, pList->pulCurrent);
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_STATS_RANGE, pList->pulCurrent);
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_LA_MIN_NORTH_STRENGTH, pList->pulCurrent);
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_LA_STRENGTH_CONTROL, pList->pulCurrent);
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_6)
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_SCENE_CHANGE, pList->pulCurrent);
#endif /* phase >= 6 */

#endif /* phase >= 3 */
#endif /* phase >= 2 */

#if (BVDC_P_SUPPORT_BVN_10BIT_444)
			BVDC_P_MAD_BLOCK_WRITE_TO_RUL(MAD_0_TOP_444_TO_422_DS_CONTROL, MAD_0_TOP_422_TO_444_US_CONTROL, pList->pulCurrent);
#endif

#if (!BVDC_P_SUPPORT_HSCL) && (!BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED)
			BVDC_P_MAD_BLOCK_WRITE_TO_RUL(MAD_0_HORIZ_CONTROL, MAD_0_HORIZ_FIR_CHROMA_COEFF_PHASE7_06_07, pList->pulCurrent);
#endif

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_7)
			BVDC_P_MAD_BLOCK_WRITE_TO_RUL(MAD_0_SCHG_MOTION_THRESHOLD_0, MAD_0_FAST_MOTION_CTRL, pList->pulCurrent);
#endif
			/* Keep this as last write for mode changes */
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_MODE_CONTROL_0, pList->pulCurrent);
			hMad->ulUpdateAll--;
			if(hMad->ulUpdateMisc > 0)
				hMad->ulUpdateMisc--;
			if(hMad->ulUpdatePullDown > 0)
				hMad->ulUpdatePullDown--;
			if(hMad->ulUpdate22PullDown > 0)
				hMad->ulUpdate22PullDown--;
			if(hMad->ulUpdateChromaSettings > 0)
				hMad->ulUpdateChromaSettings--;
			if(hMad->ulUpdateMotionSettings > 0)
				hMad->ulUpdateMotionSettings--;
		}
		else if(hMad->ulUpdateMisc)
		{
#if(BVDC_P_SUPPORT_BVN_10BIT_444)
			BVDC_P_MAD_BLOCK_WRITE_TO_RUL(MAD_0_TOP_444_TO_422_DS_CONTROL, MAD_0_TOP_422_TO_444_US_CONTROL, pList->pulCurrent);
#endif

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_LA_MIN_NORTH_STRENGTH, pList->pulCurrent);
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_LA_STRENGTH_CONTROL, pList->pulCurrent);
#endif
			hMad->ulUpdateMisc--;
		}
		else if(hMad->ulUpdatePullDown &&
			BVDC_P_MAD_COMPARE_FIELD_NAME(MAD_0_ENABLE_CONTROL, ENABLE, ON))
		{
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_IT_OUTPUT_CONTROL, pList->pulCurrent);
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_0, pList->pulCurrent);
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_1, pList->pulCurrent);
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_2, pList->pulCurrent);
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_BWV_CONTROL_1, pList->pulCurrent);
#endif
			hMad->ulUpdatePullDown--;
		}
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
		else if((hMad->ulUpdate22PullDown || hMad->bReverse22Pulldown) &&
			BVDC_P_MAD_COMPARE_FIELD_NAME(MAD_0_ENABLE_CONTROL, ENABLE, ON))
		{
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_IT_OUTPUT_CONTROL, pList->pulCurrent);
			hMad->ulUpdate22PullDown--;
		}
		else if(hMad->ulUpdateChromaSettings &&
			BVDC_P_MAD_COMPARE_FIELD_NAME(MAD_0_ENABLE_CONTROL, ENABLE, ON))
		{
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_MODE_CONTROL_0, pList->pulCurrent);
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_XCHROMA_MODE, pList->pulCurrent);
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_XCHROMA_CONTROL_0, pList->pulCurrent);
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_7)
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_MOTION_CAL_CTRL, pList->pulCurrent);
#endif
			hMad->ulUpdateChromaSettings--;
		}
		else if(hMad->ulUpdateMotionSettings &&
			BVDC_P_MAD_COMPARE_FIELD_NAME(MAD_0_ENABLE_CONTROL, ENABLE, ON))
		{
			BVDC_P_MAD_WRITE_TO_RUL(MAD_0_MODE_CONTROL_0, pList->pulCurrent);
			hMad->ulUpdateMotionSettings--;
		}
#endif

#if (BVDC_P_SUPPORT_MAD_VER == BVDC_P_MAD_VER_6)
		BVDC_P_MAD_BLOCK_WRITE_TO_RUL(MAD_0_DITHER_CTRL, MAD_0_DITHER_LFSR_CTRL, pList->pulCurrent);
#elif (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR) && (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER <= 2)
		/* MAD dithers only if not memory saving mode (ANR is OFF) */
		if(hMad->bMemSaving)
		{
			BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MAD_SIOB_0_DITHER_CTRL, 0, 0);/* disable dither */
		}
		else
		{
			/* Gap between MAD_SIOB_0_DITHER_CTRL and MAD_SIOB_0_DITHER_LFSR_INIT */
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MAD_SIOB_0_DITHER_CTRL);
			*pList->pulCurrent++ =
				BCHP_FIELD_DATA(MAD_SIOB_0_DITHER_CTRL, MODE,       hMad->stDither.ulMode     ) |
				BCHP_FIELD_DATA(MAD_SIOB_0_DITHER_CTRL, OFFSET_CH1, hMad->stDither.ulCh1Offset) |
				BCHP_FIELD_DATA(MAD_SIOB_0_DITHER_CTRL, SCALE_CH1,  hMad->stDither.ulCh1Scale ) |
				BCHP_FIELD_DATA(MAD_SIOB_0_DITHER_CTRL, OFFSET_CH0, hMad->stDither.ulCh0Offset) |
				BCHP_FIELD_DATA(MAD_SIOB_0_DITHER_CTRL, SCALE_CH0,  hMad->stDither.ulCh0Scale );

			BVDC_P_SUBRUL_START_BLOCK(pList, BCHP_MAD_SIOB_0_DITHER_LFSR_INIT, 0,
				((BCHP_MAD_SIOB_0_DITHER_LFSR_CTRL - BCHP_MAD_SIOB_0_DITHER_LFSR_INIT) / 4) + 1);

			*pList->pulCurrent++ =
				BCHP_FIELD_DATA(MAD_SIOB_0_DITHER_LFSR_INIT, SEQ,   hMad->stDither.ulLfsrSeq  ) |
				BCHP_FIELD_DATA(MAD_SIOB_0_DITHER_LFSR_INIT, VALUE, hMad->stDither.ulLfsrValue);

			*pList->pulCurrent++ =
				BCHP_FIELD_DATA(MAD_SIOB_0_DITHER_LFSR_CTRL, T0, hMad->stDither.ulLfsrCtrlT0) |
				BCHP_FIELD_DATA(MAD_SIOB_0_DITHER_LFSR_CTRL, T1, hMad->stDither.ulLfsrCtrlT1) |
				BCHP_FIELD_DATA(MAD_SIOB_0_DITHER_LFSR_CTRL, T2, hMad->stDither.ulLfsrCtrlT2);
		}
#endif

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR)
		pTestFeature1 = &hMad->stTestFeature1;
		BVDC_P_SUBRUL_START_BLOCK(pList, BCHP_MAD_SIOB_0_DCX_PRED_CFG, hMad->ulRegOffset,
			((BCHP_MAD_SIOB_0_DCX_COMPR_CFG1 - BCHP_MAD_SIOB_0_DCX_PRED_CFG) / 4) + 1);

		/* MAD_SIOB_0_DCX_PRED_CFG */
		*pList->pulCurrent++ = (
			BCHP_FIELD_DATA(MAD_SIOB_0_DCX_PRED_CFG, ENABLE,          pTestFeature1->bEnable    ) |
			BCHP_FIELD_ENUM(MAD_SIOB_0_DCX_PRED_CFG, CONVERT_RGB,     Disable ) |
			BCHP_FIELD_DATA(MAD_SIOB_0_DCX_PRED_CFG, PREDICTION_MODE, pTestFeature1->ulPredictionMode) |
			BCHP_FIELD_ENUM(MAD_SIOB_0_DCX_PRED_CFG, EDGE_PRED_ENA,   Enable  ) |
			BCHP_FIELD_ENUM(MAD_SIOB_0_DCX_PRED_CFG, LEFT_PRED_ENA,   Enable  ) |
			BCHP_FIELD_ENUM(MAD_SIOB_0_DCX_PRED_CFG, ABCD_PRED_ENA,   Enable  ) |
			BCHP_FIELD_ENUM(MAD_SIOB_0_DCX_PRED_CFG, LS_PRED_ENA,     Enable  ));

		/* MAD_SIOB_0_DCX_COMPR_CFG1 */
		*pList->pulCurrent++ = (
			BCHP_FIELD_DATA(MAD_SIOB_0_DCX_COMPR_CFG1, PIXELS_PER_GROUP,  pTestFeature1->ulPixelPerGroup) |
			BCHP_FIELD_DATA(MAD_SIOB_0_DCX_COMPR_CFG1, TGT_OFFSET_HI,     0xfa   ) |
			BCHP_FIELD_DATA(MAD_SIOB_0_DCX_COMPR_CFG1, TGT_OFFSET_LO,     12     ) |
			BCHP_FIELD_DATA(MAD_SIOB_0_DCX_COMPR_CFG1, TGT_BPG, pTestFeature1->ulBitsPerGroup));
#endif

		/* must be the last, and every field */
		BVDC_P_MAD_WRITE_TO_RUL(MAD_0_MODE_CONTROL_0, pList->pulCurrent);
		BVDC_P_MAD_WRITE_TO_RUL(MAD_0_MODE_CONTROL_1, pList->pulCurrent);
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_5)
		BVDC_P_MAD_WRITE_TO_RUL(MAD_0_MODE_CONTROL_2, pList->pulCurrent);
#endif
		BVDC_P_MAD_WRITE_TO_RUL(MAD_0_ENABLE_CONTROL, pList->pulCurrent);

#if BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED
		/* enable HSCL, HSCL cfg and enabling have already set in shadow */
		BVDC_P_Hscaler_BuildRul_SetEnable_isr(hMad->hHscaler, pList);
#endif

		/* join in vnet after enable. note: its src mux is initialed as disabled */
		if (ulRulOpsFlags & BVDC_P_RulOp_eVnetInit)
		{
			BVDC_P_SubRul_JoinInVnet_isr(&(hMad->SubRul), pList);
		}
	}
	else if (ulRulOpsFlags & BVDC_P_RulOp_eDrainVnet)
	{
		BVDC_P_Mad_BuildRul_DrainVnet_isr(hMad, pList);
	}

	BDBG_LEAVE(BVDC_P_Mad_BuildRul_isr);
	return;
}


#if (!BVDC_P_SUPPORT_HSCL) && (!BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED)
/***************************************************************************
* {private}
*
*/
static void BVDC_P_Mad_SetFirCoeff
( BVDC_P_Mad_Handle             hMad,
 const uint32_t               *pulHorzFirCoeff )
{
	int i;
	BDBG_OBJECT_ASSERT(hMad, BVDC_MAD);

	/* write 32 hor entries into registers */
	for(i = 0; (pulHorzFirCoeff) && (*pulHorzFirCoeff != BVDC_P_MAD_LAST); i++)
	{
		BVDC_P_MAD_GET_REG_DATA_I(MAD_0_HORIZ_FIR_COEFF_PHASE0_00_01, i) =
			*pulHorzFirCoeff;
		pulHorzFirCoeff++;
	}
}

/***************************************************************************
* {private}
*
*/
static void BVDC_P_Mad_SetChromaFirCoeff
( BVDC_P_Mad_Handle             hMad,
 const uint32_t               *pulHorzFirCoeff )
{
	int i;
	BDBG_OBJECT_ASSERT(hMad, BVDC_MAD);

	/* write 32 hor entries into registers */
	for(i = 0; (pulHorzFirCoeff) && (*pulHorzFirCoeff != BVDC_P_MAD_LAST); i++)
	{
		BVDC_P_MAD_GET_REG_DATA_I(MAD_0_HORIZ_FIR_CHROMA_COEFF_PHASE0_00_01, i) =
			*pulHorzFirCoeff;
		pulHorzFirCoeff++;
	}
}
#endif

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
#define BVDC_P_MAD_OPT_SCREENCHANGE_THRESHOLD   1000000
#define BVDC_P_MAD_OPT_SCREENSTILL_THRESHOLD    0x200
#define BVDC_P_MAD_OPT_N_FIELD                  4
#define BVDC_P_MAD_OPT_STILL_FILED_NUM          2
/***************************************************************************
* {private}
*
*/
BERR_Code BVDC_P_Mad_Optimize_isr
( BVDC_P_Mad_Handle                hMad,
 uint32_t                         ulFieldMotion )
{
	uint32_t                   ulFieldMotionChange;

	BDBG_ENTER(BVDC_P_Mad_Optimize_isr);
	BDBG_OBJECT_ASSERT(hMad, BVDC_MAD);

	ulFieldMotionChange = (ulFieldMotion > hMad->ulPreFieldMotion) ?
		(ulFieldMotion - hMad->ulPreFieldMotion) : 0;
	hMad->ulPreFieldMotion = ulFieldMotion;
	if(ulFieldMotion < BVDC_P_MAD_OPT_SCREENSTILL_THRESHOLD)
	{
		hMad->ulStillFieldNum++;
		if(hMad->ulChangeFieldNum > 0)
		{
			hMad->ulChangeFieldNum --;
		}
		if(hMad->ulStillFieldNum > BVDC_P_MAD_OPT_STILL_FILED_NUM)
		{
			hMad->ulStillFieldNum --;
			hMad->ulChangeFieldNum = BVDC_P_MAD_OPT_N_FIELD;
			if(!hMad->b5FieldMode)
			{
				hMad->b5FieldMode = true;
				BVDC_P_MAD_FLOG(("Mad Detect Still Screen"));
				BVDC_P_MAD_FLOG(("Enter 5 Field Mode for screen still"));
				hMad->bOptimizeStill = true;
			}
		}
		if(hMad->ulChangeFieldNum == 0 && hMad->b5FieldMode)
		{
			hMad->b5FieldMode = false;
			BVDC_P_MAD_FLOG(("Exit 5 Field Mode0"));
			hMad->bOptimizeStill = false;
		}
	}
	else
	{
		if(hMad->ulChangeFieldNum > 0)
		{
			hMad->ulChangeFieldNum --;
		}
		if(hMad->ulChangeFieldNum == 0)
		{
			hMad->ulStillFieldNum = 0;
		}
		if(hMad->ulChangeFieldNum == 0 && hMad->b5FieldMode)
		{
			hMad->b5FieldMode = false;
			BVDC_P_MAD_FLOG(("Exit 5 Field Mode1"));
			hMad->bOptimizeStill = false;
		}
	}

	BDBG_LEAVE(BVDC_P_Mad_Optimize_isr);
	return BERR_SUCCESS;
}

/***************************************************************************
* {private}
*
*/
static BERR_Code BVDC_P_Mad_SoftwareCadence22_isr
( BVDC_P_Mad_Handle                hMad,
 uint32_t                         ulWidth,
 const BFMT_VideoInfo            *pFmtInfo )
{
	BREG_Handle                hReg;
	BVDC_P_MadRev22Statistics  stPhaseStatistics[2];

	uint32_t                   ulReg;
	uint32_t                   index;
	uint32_t                   ulPhaseToUse;
	uint32_t                   ulHistogramBin4Reg;
	uint32_t                   ulLoopsReset;
	uint32_t                   ulRev22Locked;
	uint32_t                   ulRev22LockIndex = 0;
	uint32_t                   ulSelDetect;
	uint32_t                   ulRmCheckThresh;
	uint32_t                   ulRmCheckRatio;
	uint32_t                   ulSelectedFwdPCC;
	uint32_t                   ulSelectedPCC;
	uint32_t                   ulNonSelectedFwdPCC;
	uint32_t                   ulNonSelectedPCC;
	uint32_t                   ulUnlockIT = 0;
	uint32_t                   ulHistogramBin4Offset;
	uint32_t                   ulHistogramBin4Ratio;
	uint32_t                   ulMinUsablePcc, ulMaxPccChange;
	uint32_t                   ulPwMatchMul, ulPccNmMatchRatio, ulWeave22Threshold;
	uint32_t                   ulUpperMatchThresh, ulLowerNmThresh;
	int32_t                    lRev22ExitLockLevel;
	bool                       bBadWeaveEnable;

	BDBG_ENTER(BVDC_P_Mad_SoftwareCadence22_isr);
	BDBG_OBJECT_ASSERT(hMad, BVDC_MAD);
	hReg = hMad->hRegister;

	/* PR45232: rev22 increases enter/exit/sat 22lock levels; */
#if (BVDC_P_MAD_DBG_MSG == 0)
	if(VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt))
	{
		lRev22ExitLockLevel = BVDC_P_MAD_REV22_EXIT_LOCK_LEVEL + BVDC_P_MAD_REV22_LEVEL_ADJ;
		ulHistogramBin4Offset = BVDC_P_MAD_HISTOGRAM4_OFFSET_NTSC * ulWidth / BFMT_NTSC_WIDTH;
		ulHistogramBin4Ratio  = BVDC_P_MAD_HISTOGRAM4_RATIO_NTSC;
		ulRmCheckRatio = BVDC_P_MAD_RM_CHECK_RATIO_NTSC;
		ulRmCheckThresh = BVDC_P_MAD_RM_CHECK_THRESH_NTSC * ulWidth / BFMT_NTSC_WIDTH;
	}
	else
	{
		lRev22ExitLockLevel = BVDC_P_MAD_REV22_EXIT_LOCK_LEVEL;
		ulHistogramBin4Offset = (ulWidth >= BFMT_PAL_WIDTH) ?
BVDC_P_MAD_HISTOGRAM4_OFFSET_PAL_OVERSAMPLE :
		BVDC_P_MAD_HISTOGRAM4_OFFSET_PAL;
		ulHistogramBin4Ratio  = BVDC_P_MAD_HISTOGRAM4_RATIO_PAL;
		ulRmCheckRatio = BVDC_P_MAD_RM_CHECK_RATIO_PAL;
		ulRmCheckThresh = BVDC_P_MAD_RM_CHECK_THRESH_PAL * ulWidth / BFMT_PAL_WIDTH;
	}
	ulMinUsablePcc = BVDC_P_MAD_MIN_USABLE_PCC;
	ulPwMatchMul = BVDC_P_MAD_PW_MATCH_MULTIPLIER;
	ulPccNmMatchRatio = BVDC_P_MAD_PCC_NONMATCH_MATCH_RATIO;
	ulWeave22Threshold = (ulWidth >= BFMT_PAL_WIDTH) ?
BVDC_P_MAD_WEAVE_22_THRESHOLD_OVERSAMPLE : BVDC_P_MAD_WEAVE_22_THRESHOLD;
	ulUpperMatchThresh = BVDC_P_MAD_UPPER_MATCH_THRESH;
	ulLowerNmThresh = BVDC_P_MAD_LOWER_NONMATCH_THRESH;
	ulMaxPccChange = BVDC_P_MAD_MAX_PCC_CHANGE;
#else /* debug mode when BVDC_P_MAD_DBG_MSG = 1;
	and user can OVERRIDE soft macroes using RDC scratch registers; */
	if(BREG_Read32_isr(hReg, 0x00302918) == 0) /* RDC_scratch_70 */
	{
		if(VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt))
		{
			lRev22ExitLockLevel = BVDC_P_MAD_REV22_EXIT_LOCK_LEVEL + BVDC_P_MAD_REV22_LEVEL_ADJ;
			ulHistogramBin4Offset = BREG_Read32_isr(hReg, 0x00302940) * ulWidth / BFMT_NTSC_WIDTH;/* RDC_scratch_80 */
			ulHistogramBin4Ratio  = BREG_Read32_isr(hReg, 0x00302944);/* RDC_scratch_81 */
			ulRmCheckRatio = BREG_Read32_isr(hReg, 0x00302948);/* RDC_scratch_82 */
			ulRmCheckThresh = BREG_Read32_isr(hReg, 0x0030294c) * ulWidth / BFMT_NTSC_WIDTH;/* RDC_scratch_83 */
		}
		else
		{
			lRev22ExitLockLevel = BVDC_P_MAD_REV22_EXIT_LOCK_LEVEL;
			ulHistogramBin4Offset = BREG_Read32_isr(hReg, 0x00302950) * ulWidth / 720;/* RDC_scratch_84 */
			ulHistogramBin4Ratio  = BREG_Read32_isr(hReg, 0x00302954);/* RDC_scratch_85 */
			ulRmCheckRatio = BREG_Read32_isr(hReg, 0x00302958);/* RDC_scratch_86 */
			ulRmCheckThresh = BREG_Read32_isr(hReg, 0x0030295c) * ulWidth / BFMT_PAL_WIDTH;/* RDC_scratch_87 */
		}
		ulMinUsablePcc = BREG_Read32_isr(hReg, 0x00302960);/* RDC_scratch_88 */;
		ulPwMatchMul = BREG_Read32_isr(hReg, 0x00302964);/* RDC_scratch_89 */
		ulPccNmMatchRatio = BREG_Read32_isr(hReg, 0x00302968);/* RDC_scratch_90 */
		ulWeave22Threshold = BREG_Read32_isr(hReg, 0x0030296c);/* RDC_scratch_91 */

		ulUpperMatchThresh = BREG_Read32_isr(hReg, 0x00302970);/* RDC_scratch_92 */
		ulLowerNmThresh = BREG_Read32_isr(hReg, 0x00302974);/* RDC_scratch_93 */
		ulMaxPccChange = BREG_Read32_isr(hReg, 0x00302978);/* RDC_scratch_94 */
		hMad->ulRev22NonMatchMatchRatio = BREG_Read32_isr(hReg, 0x0030297c);/* RDC_scratch_95 */;
	}
	else
	{
		if(VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt))
		{
			lRev22ExitLockLevel = BVDC_P_MAD_REV22_EXIT_LOCK_LEVEL + BVDC_P_MAD_REV22_LEVEL_ADJ;
			ulHistogramBin4Offset = BVDC_P_MAD_HISTOGRAM4_OFFSET_NTSC * ulWidth / BFMT_NTSC_WIDTH;
			ulHistogramBin4Ratio  = BVDC_P_MAD_HISTOGRAM4_RATIO_NTSC;
			ulRmCheckRatio = BVDC_P_MAD_RM_CHECK_RATIO_NTSC;
			ulRmCheckThresh = BVDC_P_MAD_RM_CHECK_THRESH_NTSC * ulWidth / BFMT_NTSC_WIDTH;
		}
		else
		{
			lRev22ExitLockLevel = BVDC_P_MAD_REV22_EXIT_LOCK_LEVEL;
			ulHistogramBin4Offset = BVDC_P_MAD_HISTOGRAM4_OFFSET_PAL * ulWidth / BFMT_PAL_WIDTH;
			ulHistogramBin4Ratio  = BVDC_P_MAD_HISTOGRAM4_RATIO_PAL;
			ulRmCheckRatio = BVDC_P_MAD_RM_CHECK_RATIO_PAL;
			ulRmCheckThresh = BVDC_P_MAD_RM_CHECK_THRESH_PAL * ulWidth / BFMT_PAL_WIDTH;
		}
		ulMinUsablePcc = BVDC_P_MAD_MIN_USABLE_PCC;
		ulPwMatchMul = BVDC_P_MAD_PW_MATCH_MULTIPLIER;
		ulPccNmMatchRatio = BVDC_P_MAD_PCC_NONMATCH_MATCH_RATIO;
		ulWeave22Threshold = (ulWidth >= BFMT_PAL_WIDTH) ?
BVDC_P_MAD_WEAVE_22_THRESHOLD_OVERSAMPLE : BVDC_P_MAD_WEAVE_22_THRESHOLD;
		ulUpperMatchThresh = BVDC_P_MAD_UPPER_MATCH_THRESH;
		ulLowerNmThresh = BVDC_P_MAD_LOWER_NONMATCH_THRESH;
		ulMaxPccChange = BVDC_P_MAD_MAX_PCC_CHANGE;

		hMad->ulRev22NonMatchMatchRatio = BVDC_P_MAD_NONMATCH_MATCH_RATIO;
		/* resume */
		BREG_Write32_isr(hReg, 0x00302940, BVDC_P_MAD_HISTOGRAM4_OFFSET_NTSC);/* RDC_scratch_80 */
		BREG_Write32_isr(hReg, 0x00302944, BVDC_P_MAD_HISTOGRAM4_RATIO_NTSC);/* RDC_scratch_81 */
		BREG_Write32_isr(hReg, 0x00302948, BVDC_P_MAD_RM_CHECK_RATIO_NTSC);/* RDC_scratch_82 */
		BREG_Write32_isr(hReg, 0x0030294c, BVDC_P_MAD_RM_CHECK_THRESH_NTSC);/* RDC_scratch_83 */

		BREG_Write32_isr(hReg, 0x00302950, BVDC_P_MAD_HISTOGRAM4_OFFSET_PAL);/* RDC_scratch_84 */
		BREG_Write32_isr(hReg, 0x00302954, BVDC_P_MAD_HISTOGRAM4_RATIO_PAL);/* RDC_scratch_85 */
		BREG_Write32_isr(hReg, 0x00302958, BVDC_P_MAD_RM_CHECK_RATIO_PAL);/* RDC_scratch_86 */
		BREG_Write32_isr(hReg, 0x0030295c, BVDC_P_MAD_RM_CHECK_THRESH_PAL);/* RDC_scratch_87 */

		BREG_Write32_isr(hReg, 0x00302960, BVDC_P_MAD_MIN_USABLE_PCC);/* RDC_scratch_88 */
		BREG_Write32_isr(hReg, 0x00302964, BVDC_P_MAD_PW_MATCH_MULTIPLIER);/* RDC_scratch_89 */
		BREG_Write32_isr(hReg, 0x00302968, BVDC_P_MAD_PCC_NONMATCH_MATCH_RATIO);/* RDC_scratch_90 */
		BREG_Write32_isr(hReg, 0x0030296c, BVDC_P_MAD_WEAVE_22_THRESHOLD);/* RDC_scratch_91 */

		BREG_Write32_isr(hReg, 0x00302970, BVDC_P_MAD_UPPER_MATCH_THRESH);/* RDC_scratch_92 */
		BREG_Write32_isr(hReg, 0x00302974, BVDC_P_MAD_LOWER_NONMATCH_THRESH);/* RDC_scratch_93 */
		BREG_Write32_isr(hReg, 0x00302978, BVDC_P_MAD_MAX_PCC_CHANGE);/* RDC_scratch_94 */
		BREG_Write32_isr(hReg, 0x0030297c, BVDC_P_MAD_NONMATCH_MATCH_RATIO);/* RDC_scratch_95 */
	}
#endif

	/* Compute phase counters after every MAD_IT_READY interrupt */
	ulReg = BREG_Read32(hReg, BCHP_BVNF_INTR2_5_R5F_STATUS);
	if(hMad->eId == BVDC_P_MadId_eMad0)
	{
		if(!(ulReg & BVDC_P_MAD_0_IT_READY_INT_MASK))
		{
			BDBG_MSG(("MAD0 IT not ready!"));
			return BERR_SUCCESS;
		}

		/* Clear interrupt */
		ulReg |= BVDC_P_MAD_0_IT_READY_INT_MASK;
	}
#if BVDC_P_MAD_1_IT_READY_INT_MASK
	else if(hMad->eId == BVDC_P_MadId_eMad1)
	{
		if(!(ulReg & BVDC_P_MAD_1_IT_READY_INT_MASK))
		{
			return BERR_SUCCESS;
		}

		/* Clear interrupt */
		ulReg |= BVDC_P_MAD_1_IT_READY_INT_MASK;
	}
#endif
	BREG_Write32(hReg, BCHP_BVNF_INTR2_5_R5F_CLEAR, ulReg);

	/* Gather statistics for two indexes */
	switch(hMad->eGameMode)
	{
		case (BVDC_MadGameMode_eOff):
		stPhaseStatistics[0].ulMatchWeave    = BREG_Read32(hReg, BCHP_MAD_0_IT_PCC_LUMA_FFWD);
		stPhaseStatistics[0].ulMatchUM       = BREG_Read32(hReg, BCHP_MAD_0_IT_FRAME_UNEXPECTED_MOTION_3);
		stPhaseStatistics[0].ulNonMatchUM    = BREG_Read32(hReg, BCHP_MAD_0_IT_FRAME_UNEXPECTED_MOTION_2);
		stPhaseStatistics[0].ulPixelWeave    = BREG_Read32(hReg, BCHP_MAD_0_IT_BAD_22_PHASE_DETECT);
		break;

		case (BVDC_MadGameMode_e5Fields_1Delay):
		case (BVDC_MadGameMode_e3Fields_LowBandwidth):
		case(BVDC_MadGameMode_e4Fields_1Delay):
		stPhaseStatistics[0].ulMatchWeave    = hMad->ulPccFwd;
		stPhaseStatistics[0].ulMatchUM       = 0;
		stPhaseStatistics[0].ulNonMatchUM    = 0;
		stPhaseStatistics[0].ulPixelWeave    = 0;
		break;

		default:
		stPhaseStatistics[0].ulMatchWeave    = 0;
		stPhaseStatistics[0].ulMatchUM       = 0;
		stPhaseStatistics[0].ulNonMatchUM    = 0;
		stPhaseStatistics[0].ulPixelWeave    = 0;
	}

    switch(hMad->eGameMode)
    {
		case (BVDC_MadGameMode_e5Fields_1Delay):
			stPhaseStatistics[0].ulNonMatchWeave = BREG_Read32(hReg, BCHP_MAD_0_IT_PCC_LUMA_BWD);
			break;
		case (BVDC_MadGameMode_e4Fields_1Delay):
		case (BVDC_MadGameMode_e3Fields_LowBandwidth):
		default:
			stPhaseStatistics[0].ulNonMatchWeave = BREG_Read32(hReg, BCHP_MAD_0_IT_PCC_LUMA_FWD);
			break;
    }

	stPhaseStatistics[1].ulMatchWeave    = stPhaseStatistics[0].ulNonMatchWeave;
	stPhaseStatistics[1].ulNonMatchWeave = stPhaseStatistics[0].ulMatchWeave;
	stPhaseStatistics[1].ulMatchUM       = stPhaseStatistics[0].ulNonMatchUM;
	stPhaseStatistics[1].ulNonMatchUM    = stPhaseStatistics[0].ulMatchUM;

	stPhaseStatistics[0].ulAvgWeave      = BREG_Read32(hReg, BCHP_MAD_0_IT_PCC_LUMA_FAVG);

	stPhaseStatistics[1].ulRepfMotion	 = stPhaseStatistics[0].ulRepfMotion = hMad->ulRepfMotion;
	stPhaseStatistics[1].ulAvgWeave      = stPhaseStatistics[0].ulAvgWeave;
	stPhaseStatistics[1].ulPixelWeave    = stPhaseStatistics[0].ulPixelWeave;


	ulReg = BREG_Read32(hReg, BCHP_MAD_0_IT_OUTPUT_CONTROL);
	ulLoopsReset = BCHP_GET_FIELD_DATA(ulReg, MAD_0_IT_OUTPUT_CONTROL, LOOPS_RESET);
	ulRev22Locked = BCHP_GET_FIELD_DATA(ulReg, MAD_0_IT_OUTPUT_CONTROL, REV22_LOCKED);
	ulHistogramBin4Reg = BREG_Read32(hReg, BCHP_MAD_0_IT_HISTOGRAM_BIN_4);
	ulReg = BREG_Read32(hReg, BCHP_MAD_0_IT_FIELD_PHASE_CALC_CONTROL_4);
	bBadWeaveEnable = BCHP_GET_FIELD_DATA(ulReg, MAD_0_IT_FIELD_PHASE_CALC_CONTROL_4, BAD_WEAVE_DETECT_ENABLE);
	if(BVDC_MadGameMode_eOff == hMad->eGameMode)
	{
		ulReg = BREG_Read32(hReg, BCHP_MAD_0_IT_PCC_LUMA_BWD);
	}
	else if((BVDC_MadGameMode_e5Fields_1Delay == hMad->eGameMode) ||
		(BVDC_MadGameMode_e3Fields_LowBandwidth == hMad->eGameMode))
	{
		ulReg = hMad->ulPrePccFwd;
	}

	/* Calculate the phase counters for index=0 and index=1 */
	for(index = 0; index < 2; index++)
	{
		ulPhaseToUse = (hMad->ulPhaseCounter22 + index) % 2;

		/* if(LOOPS_RESET & 0x2) */
		if(ulLoopsReset == BCHP_MAD_0_IT_OUTPUT_CONTROL_LOOPS_RESET_REV22)
		{
			hMad->alPhase22[ulPhaseToUse] = 0;
		}
		else if((stPhaseStatistics[index].ulMatchWeave >
			ulMinUsablePcc) ||
			(stPhaseStatistics[index].ulNonMatchWeave >
			ulMinUsablePcc))
		{
			if((stPhaseStatistics[0].ulNonMatchWeave >
				stPhaseStatistics[0].ulMatchWeave) &&
				((stPhaseStatistics[index].ulPixelWeave << 8) >
				(stPhaseStatistics[index].ulMatchWeave *
				ulPwMatchMul)))
			{
				hMad->alPhase22[ulPhaseToUse]--;
				BVDC_P_MAD_FLOG(("(1)ulPhaseToUse = %d, fnct=%d, phase[0,1]=[%d, %d], pccFWD=%d, pccFFWD=%d, pxlWv=%d, matchWv=%d",
					ulPhaseToUse, hMad->ulPhaseCounter22,
					hMad->alPhase22[0], hMad->alPhase22[1],
					stPhaseStatistics[0].ulNonMatchWeave, stPhaseStatistics[0].ulMatchWeave,
					stPhaseStatistics[index].ulPixelWeave, stPhaseStatistics[index].ulMatchWeave));
			}
			else if((stPhaseStatistics[index].ulNonMatchWeave << 1) >
				(stPhaseStatistics[index].ulMatchWeave *
				ulPccNmMatchRatio))
			{
				if(stPhaseStatistics[index].ulMatchWeave >
					(ulHistogramBin4Reg * ulHistogramBin4Ratio + ulHistogramBin4Offset))
				{
					hMad->alPhase22[ulPhaseToUse] = hMad->alPhase22[ulPhaseToUse] -
						BVDC_P_MAD_HISTOCHECK_DEC;
					BVDC_P_MAD_FLOG(("(2)ulPhaseToUse = %d, fnct=%d, phase[0,1]=[%d, %d], nmWv=%d, matchWv=%d, histBin=%d",
						ulPhaseToUse, hMad->ulPhaseCounter22,
						hMad->alPhase22[0], hMad->alPhase22[1],
						stPhaseStatistics[index].ulNonMatchWeave, stPhaseStatistics[index].ulMatchWeave, ulHistogramBin4Reg));
				}
				else if((stPhaseStatistics[index].ulMatchWeave > ulRmCheckThresh) &&
					((stPhaseStatistics[index].ulMatchWeave * ulRmCheckRatio) >
					stPhaseStatistics[index].ulRepfMotion))
				{
					hMad->alPhase22[ulPhaseToUse] = hMad->alPhase22[ulPhaseToUse] -
						BVDC_P_MAD_HISTOCHECK_DEC;
					BVDC_P_MAD_FLOG(("(3)ulPhaseToUse = %d, fnct=%d, phase[0,1]=[%d, %d], matchWv=%d, rmThresh=%d, rmRatio=%d, rpfMot=%d",
						ulPhaseToUse, hMad->ulPhaseCounter22,
						hMad->alPhase22[0], hMad->alPhase22[1],
						stPhaseStatistics[index].ulMatchWeave, ulRmCheckThresh, ulRmCheckRatio, stPhaseStatistics[index].ulRepfMotion));
				}
				else
				{
					hMad->alPhase22[ulPhaseToUse]++;
					hMad->ulRev22Candidate = ulPhaseToUse;
				}
			}
			else if(stPhaseStatistics[index].ulMatchWeave >
				stPhaseStatistics[index].ulNonMatchWeave +
				ulWeave22Threshold)
			{
				hMad->alPhase22[ulPhaseToUse]--;
#if BVDC_P_MAD_DBG_FLOG
				if(hMad->alPhase22[ulPhaseToUse] >= 0)
				{
					BVDC_P_MAD_FLOG(("(4)ulPhaseToUse = %d, fnct=%d, phase[0,1]=[%d, %d], nmWv=%d, matchWv=%d",
						ulPhaseToUse, hMad->ulPhaseCounter22,
						hMad->alPhase22[0], hMad->alPhase22[1],
						stPhaseStatistics[index].ulNonMatchWeave, stPhaseStatistics[index].ulMatchWeave));
				}
#endif
			}
		}
		else if((stPhaseStatistics[index].ulMatchUM > ulUpperMatchThresh) ||
			(stPhaseStatistics[index].ulMatchUM > stPhaseStatistics[index].ulNonMatchUM))
		{
			hMad->alPhase22[ulPhaseToUse] = 0;
		}
		else if((stPhaseStatistics[index].ulMatchUM < ulUpperMatchThresh) &&
			(stPhaseStatistics[index].ulNonMatchUM > ulLowerNmThresh))
		{
			if((stPhaseStatistics[index].ulNonMatchUM << 1) >
				(hMad->ulRev22NonMatchMatchRatio *
				stPhaseStatistics[index].ulMatchUM))
			{
				hMad->alPhase22[ulPhaseToUse]++;
				hMad->ulRev22Candidate = ulPhaseToUse;
			}
			else if(ulRev22Locked == 0)
			{
				hMad->alPhase22[ulPhaseToUse]--;
				BVDC_P_MAD_FLOG(("(5)ulPhaseToUse = %d, fnct=%d, phase[0,1]=[%d, %d], nmUM=%d, matchUM=%d",
					ulPhaseToUse, hMad->ulPhaseCounter22,
					hMad->alPhase22[0], hMad->alPhase22[1],
					stPhaseStatistics[index].ulNonMatchUM, stPhaseStatistics[index].ulMatchUM));
			}
		}
		else
		{
			hMad->alPhase22[ulPhaseToUse]--;
			BVDC_P_MAD_FLOG(("(6)ulPhaseToUse = %d, fnct=%d, phase[0,1]=[%d, %d], nmUM=%d, matchUM=%d, nmWv=%d, matWv=%d",
				ulPhaseToUse, hMad->ulPhaseCounter22,
				hMad->alPhase22[0], hMad->alPhase22[1],
				stPhaseStatistics[index].ulNonMatchUM, stPhaseStatistics[index].ulMatchUM,
				stPhaseStatistics[index].ulNonMatchWeave, stPhaseStatistics[index].ulMatchWeave));
		}

		/* Clip the phase counter values */
		if(hMad->alPhase22[ulPhaseToUse] > hMad->ulRev22LocksatLevel)
		{
			hMad->alPhase22[ulPhaseToUse] = hMad->ulRev22LocksatLevel;
		}
		if(hMad->alPhase22[ulPhaseToUse] < 0)
		{
			hMad->alPhase22[ulPhaseToUse] = 0;
		}

		/* Determine if lock state Rev22_locked nees to change */
		if(hMad->alPhase22[ulPhaseToUse] >= hMad->ulRev22EnterLockLevel)
		{
			ulRev22Locked = 1;
			ulRev22LockIndex = ulPhaseToUse;
		}
		else if(hMad->alPhase22[ulPhaseToUse] <= lRev22ExitLockLevel)
		{
			ulRev22Locked = 0;
		}
		BVDC_P_MAD_FLOG(("(*)ulPhaseToUse = %d, phase[%d]=[%d], nmUM=%d, matchUM=%d, nmWv=%d, matWv=%d, ulHistogramBin4Reg=%d,ulRepfMotion=%d",
			ulPhaseToUse, index,
			hMad->alPhase22[index],
			stPhaseStatistics[index].ulNonMatchUM, stPhaseStatistics[index].ulMatchUM,
			stPhaseStatistics[index].ulNonMatchWeave, stPhaseStatistics[index].ulMatchWeave, ulHistogramBin4Reg,
			stPhaseStatistics[index].ulRepfMotion));
	} /* end of for loop */

	/* If one counter goes up, the other counter will go down, both phases */
	/* will never set ulRev22Locked=1.  The value for ulRev22Locked only */
	/* indicates that one or the other phase has achieved the lock state */
	if(hMad->alPhase22[0] >= hMad->ulRev22EnterLockLevel ||
		hMad->alPhase22[1] >= hMad->ulRev22EnterLockLevel)
	{
		ulRev22Locked = 1;
	}

	/* The next part of the algorithm is to determine if there is a bad weave */
	/* that would throw the 2:2 cadence out of lock. */
	/* We define the REV22_candidate as the alst phase_to_use that was */
	/* incremented using the above logic.  First we need to determine */
	/* which direction is intended by the current locked phase (if we're */
	/* locked) or REV22_candidate (if we're not locked).  Rev22_candidate is */
	/* similar to rev22_locked_index but it is set before lock is actually */
	/* achieved */
	ulSelDetect = (ulRev22Locked == 1) ?
		((hMad->ulPhaseCounter22 + ulRev22LockIndex + 1) % 2) :
	((hMad->ulPhaseCounter22 + hMad->ulRev22Candidate + 1) % 2);

	if(BVDC_MadGameMode_eOff == hMad->eGameMode)
	{
		if(ulSelDetect == 0)
		{
			/* sel_detect == 0 => IT_DETECT_MODE_SEL = BWD */
			ulSelectedFwdPCC    = stPhaseStatistics[0].ulNonMatchWeave; /* BCHP_MAD_0_IT_PCC_LUMA_FWD  */
			ulSelectedPCC       = ulSelectedFwdPCC;                     /* BCHP_MAD_0_IT_PCC_LUMA_FWD  */
			ulNonSelectedFwdPCC = stPhaseStatistics[0].ulMatchWeave;    /* BCHP_MAD_0_IT_PCC_LUMA_FFWD */
			ulNonSelectedPCC    = ulReg;                                /* BCHP_MAD_0_IT_PCC_LUMA_BWD  */
		}
		else
		{
			/* sel_detect == 1 => IT_DETECT_MODE_SEL = FWD */
			ulSelectedFwdPCC    = stPhaseStatistics[0].ulMatchWeave;    /* BCHP_MAD_0_IT_PCC_LUMA_FFWD */
			ulSelectedPCC       = ulReg;                                /* BCHP_MAD_0_IT_PCC_LUMA_BWD  */
			ulNonSelectedFwdPCC = stPhaseStatistics[0].ulNonMatchWeave; /* BCHP_MAD_0_IT_PCC_LUMA_FWD  */
			ulNonSelectedPCC    = ulNonSelectedFwdPCC;                  /* BCHP_MAD_0_IT_PCC_LUMA_FWD  */
		}

		/* Skip bad weave checking if the weaves in the "Selected" direction are */
		/* sufficiently small */
		if(ulSelectedPCC >= BVDC_P_MAD_REV22_DONTCARE)
		{
			if(bBadWeaveEnable &&
				(ulSelectedPCC >= ulNonSelectedPCC + ulWeave22Threshold))
			{
				ulUnlockIT = 1;
				BVDC_P_MAD_FLOG(("Unlock IT (a)."));
			}
		}
		if(ulSelectedFwdPCC >= BVDC_P_MAD_REV22_DONTCARE)
		{
			if(bBadWeaveEnable &&
				(ulSelectedFwdPCC >= ulNonSelectedFwdPCC + ulWeave22Threshold))
			{
				ulUnlockIT = 1;
				BVDC_P_MAD_FLOG(("Unlock IT (b)."));
			}
			if(bBadWeaveEnable &&
				(ulSelectedFwdPCC > ulSelectedPCC + ulMaxPccChange))
			{
				ulUnlockIT = 1;
				BVDC_P_MAD_FLOG(("Unlock IT (c)."));
			}
		}
		BVDC_P_MAD_FLOG(("fnct=%d, selectPCC=%d, unselectPCC=%d, selectFwdPCC=%d, unselectFwdPCC=%d",
			hMad->ulPhaseCounter22, ulSelectedPCC, ulNonSelectedPCC, ulSelectedFwdPCC, ulNonSelectedFwdPCC));
	}

	/* If the unlock_IT variable is set, clear the phase counters and the */
	/* lock state */
	if(ulUnlockIT)
	{
		ulRev22Locked = 0;
		hMad->alPhase22[0] = 0;
		hMad->alPhase22[1] = 0;
	}

	/* skip weave during hard start when usTrickModeStartDelay != 0;
	Note: hard start should clear the cadence lock as well; */
	if(!hMad->usTrickModeStartDelay)
	{
		BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_OUTPUT_CONTROL) &= ~(
			BCHP_MASK(MAD_0_IT_OUTPUT_CONTROL, REV22_LOCKED) |
			BCHP_MASK(MAD_0_IT_OUTPUT_CONTROL, REV22_FIELD_PHASE));
		BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_OUTPUT_CONTROL) |= (
			BCHP_FIELD_DATA(MAD_0_IT_OUTPUT_CONTROL, REV22_LOCKED, ulRev22Locked) |
			BCHP_FIELD_DATA(MAD_0_IT_OUTPUT_CONTROL, REV22_FIELD_PHASE, ulSelDetect));

		if(ulRev22Locked==1)
		{
			BVDC_P_MAD_FLOG(("REV22_FIELD_PHASE = %d, fnct=%d, phase[%d]=%d",
				ulSelDetect, hMad->ulPhaseCounter22,
				ulRev22LockIndex, hMad->alPhase22[ulRev22LockIndex]));
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_5)
			if((BVDC_MadGameMode_e5Fields_1Delay == hMad->eGameMode) ||
				(BVDC_MadGameMode_e3Fields_LowBandwidth == hMad->eGameMode))
			{
				/* this control the rev22 weave in game mode */
				/* this will overwrite previous game mode 3:2 weave decision */
				BVDC_P_MAD_GET_REG_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2) &= ~(
					BCHP_MASK(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2, IT_DEBUG_MODE_FORCE) |
					BCHP_MASK(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2, IT_MODE_SEL));
				BVDC_P_MAD_GET_REG_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2) |= (
					BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2, IT_DEBUG_MODE_FORCE, ON) |
					(ulSelDetect ?
					BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2, IT_MODE_SEL,   BWD) :
				BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2, IT_MODE_SEL,   FWD)));
			}
#endif
		}
	}

	hMad->ulPhaseCounter22++;
	hMad->ulPrePccFwd = hMad->ulPccFwd;
	hMad->ulPccFwd = stPhaseStatistics[0].ulNonMatchWeave;


	BDBG_LEAVE(BVDC_P_Mad_SoftwareCadence22_isr);
	return BERR_SUCCESS;
}
#endif

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_5)
/***************************************************************************
* {private}
*
*/
static void BVDC_P_Mad_SoftwareCadence32_isr
( BVDC_P_Mad_Handle                hMad )
{
	BREG_Handle        hReg;
	uint32_t           ulReg;
	uint32_t           ulFcnt;
	uint32_t           index;
	uint32_t           ulLoopsReset;
	uint32_t           ulRev32Locked = 0;
	uint32_t           ulRev32LockIndex = 0;
	bool               bAllDecrement;
	uint32_t           ulSumX = 0;
	uint32_t           ulMinSigma = ~1, ulMaxSigma = 0;
	uint32_t           ulRepfMotion;
	uint32_t           ulRepfVetoLevel;
	int32_t            D;
	uint32_t           ulRev32_Field_Phase;
	uint32_t           ulItModeSel = 0;
	uint32_t           ulShift = 0;
	BVDC_P_MadRev32Statistics *pStats;

	BDBG_ENTER(BVDC_P_Mad_SoftwareCadence32_isr);
	BDBG_OBJECT_ASSERT(hMad, BVDC_MAD);
	hReg = hMad->hRegister;
	pStats = &hMad->stRev32Statistics;

	/* Gather statistics for 5 indexes */
	switch(hMad->eGameMode)
	{
		case BVDC_MadGameMode_e5Fields_1Delay:
			pStats->ulPccLumaPcc   = BREG_Read32_isr(hReg, BCHP_MAD_0_IT_PCC_LUMA_BWD     + hMad->ulRegOffset);
		    pStats->ulPccChromaPcc = BREG_Read32_isr(hReg, BCHP_MAD_0_IT_PCC_CHROMA_BWD   + hMad->ulRegOffset);
			break;
		case BVDC_MadGameMode_e3Fields_1Delay:
		case BVDC_MadGameMode_e4Fields_1Delay:
		default:
			pStats->ulPccLumaPcc   = BREG_Read32_isr(hReg, BCHP_MAD_0_IT_PCC_LUMA_FWD     + hMad->ulRegOffset);
		    pStats->ulPccChromaPcc = BREG_Read32_isr(hReg, BCHP_MAD_0_IT_PCC_CHROMA_FWD   + hMad->ulRegOffset);
	}

	pStats->ulWndBias = BREG_Read32_isr(hReg, BCHP_MAD_0_IT_PCC_TKR_FWD + hMad->ulRegOffset);

	pStats->ulPhaseCalc0 = BREG_Read32_isr(hReg, BCHP_MAD_0_IT_FIELD_PHASE_CALC_CONTROL_0);
	pStats->ulPhaseCalc1 = BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_1);
	pStats->ulPhaseCalc2 = BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_2);
	pStats->ulPhaseCalc8 = BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_8);
	pStats->ulBwvCtrl5   = BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_5);

	ulFcnt = hMad->ulPhaseCounter % 5;
	pStats->aulSigma[ulFcnt] = hMad->aulSigma;
	ulRepfMotion     = hMad->ulRepfMotion;
	ulReg = BREG_Read32_isr(hReg, BCHP_MAD_0_IT_OUTPUT_CONTROL);
	ulLoopsReset = BCHP_GET_FIELD_DATA(ulReg, MAD_0_IT_OUTPUT_CONTROL, LOOPS_RESET);

	pStats->ulWndBias = (pStats->ulWndBias *
		BCHP_GET_FIELD_DATA(pStats->ulBwvCtrl5, MAD_0_BWV_CONTROL_5, TKR_PCC_MULT)) >> 4;

	ulRepfVetoLevel =
		BCHP_GET_FIELD_DATA(pStats->ulPhaseCalc1, MAD_0_IT_FIELD_PHASE_CALC_CONTROL_1, REV32_REPF_VETO_LEVEL) << 5;
	ulRepfVetoLevel = (ulRepfVetoLevel > pStats->ulWndBias) ? (ulRepfVetoLevel - pStats->ulWndBias) : 0;
	ulRepfVetoLevel = BVDC_P_MAX(ulRepfVetoLevel,
		BCHP_GET_FIELD_DATA(pStats->ulBwvCtrl5, MAD_0_BWV_CONTROL_5, TKR_MIN_REPF_VETO_LEVEL) << 5);

	pStats->aulP[ulFcnt] = ulRepfMotion > (
		BCHP_GET_FIELD_DATA(pStats->ulPhaseCalc1, MAD_0_IT_FIELD_PHASE_CALC_CONTROL_1, REV32_REPF_PIX_CORRECT_LEVEL) << 5);
	pStats->aulV[ulFcnt] = ulRepfMotion > ulRepfVetoLevel;

	for(index = 0; index < 5; index++)
	{
		pStats->aulX[index] = pStats->aulSigma[index] >> 5;
		ulSumX += pStats->aulX[index];
		ulMinSigma  = BVDC_P_MIN(ulMinSigma, pStats->aulX[index]);
		ulMaxSigma  = BVDC_P_MAX(ulMaxSigma, pStats->aulX[index]);
	}

	bAllDecrement = (ulMaxSigma - ulMinSigma) < BCHP_GET_FIELD_DATA(pStats->ulPhaseCalc8,
		MAD_0_IT_FIELD_PHASE_CALC_CONTROL_8, REV32_MIN_SIGMA_RANGE);

	/* Calculate the phase counters for index=[0, 4] */
	for(index = 0; index < 5; index++)
	{
		/* if(LOOPS_RESET) */
		if(ulLoopsReset == BCHP_MAD_0_IT_OUTPUT_CONTROL_LOOPS_RESET_REV32)
		{
			hMad->alPhase[index] = 0;
			pStats->aulP[index] = 1;
			pStats->aulV[index] = 1;
		}
		else
		{
			uint32_t ulNum;
			uint32_t ulMinSigma_ex = pStats->aulX[(index+1) % 5];
			uint32_t ulMaxSigma_ex = ulMinSigma_ex;
			int32_t lRev32EnterLockLevel = BCHP_GET_FIELD_DATA(pStats->ulPhaseCalc0,
				MAD_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV32_ENTER_LOCK_LEVEL);
			int32_t lRev32ExitockLevel = BCHP_GET_FIELD_DATA(pStats->ulPhaseCalc0,
				MAD_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV32_EXIT_LOCK_LEVEL);
			int32_t N = ulSumX - 5 * pStats->aulX[index];
			bool bCorr;

			for(ulNum = 0; ulNum < 5; ulNum++)
			{
				if(index != ulNum)
				{
					ulMinSigma_ex = BVDC_P_MIN(ulMinSigma_ex, pStats->aulX[ulNum]);
					ulMaxSigma_ex = BVDC_P_MAX(ulMaxSigma_ex, pStats->aulX[ulNum]);
				}
			}
			D = (ulMaxSigma_ex - ulMinSigma_ex) * BCHP_GET_FIELD_DATA(
				pStats->ulPhaseCalc0, MAD_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV32_PHASE_MATCH_THRESH);
			bCorr = N > D;

			if(bCorr && !bAllDecrement)
			{
				if(++hMad->alPhase[index] > (int32_t)BCHP_GET_FIELD_DATA(pStats->ulPhaseCalc8,
					MAD_0_IT_FIELD_PHASE_CALC_CONTROL_8, REV32_LOCK_SAT_THRESH))
				{
					hMad->alPhase[index] = BCHP_GET_FIELD_DATA(pStats->ulPhaseCalc2,
						MAD_0_IT_FIELD_PHASE_CALC_CONTROL_2, REV32_LOCK_SAT_LEVEL);
				}
			}
			else
			{
				if(--hMad->alPhase[index] < 0)
				{
					hMad->alPhase[index] = 0;
				}
			}

			if(pStats->aulV[index])
			{
				hMad->alPhase[index] = 0;
			}

			/* find the locked phase */
			if(hMad->alPhase[index] >= lRev32EnterLockLevel)
			{
				pStats->abRev32Locked[index] = true;
				ulRev32Locked         = 1;
				ulRev32LockIndex      = index;

				/* set all other phase[] to REV32_ENTER_LOCK_LEVEL-1 and all Rev32_locked[] to 0;
				ONLY ONE can be locked! */
				for(ulNum = 0; ulNum < 5; ulNum++)
				{
					if((ulNum != index) && pStats->abRev32Locked[ulNum])
					{
						uint32_t i;
						for(i = 0; i < 5; i++)
						{
							if(hMad->alPhase[i] >= lRev32EnterLockLevel)
							{
								hMad->alPhase[i] = lRev32EnterLockLevel - 1;
							}
							pStats->abRev32Locked[i] = false;
						}
						BVDC_P_MAD_FLOG(("Phase %d and %d are both locked! Clear %d", index, ulNum, ulNum));
					}
				}
			}
			else if(hMad->alPhase[index] <= lRev32ExitockLevel)
			{
				pStats->abRev32Locked[index] = false;
			}
		}
	} /* end of for loop */

	/* Tricky: + 1 here! Due to 1 field processing latency to read statistics
	for the previous picture? */
	if ((BVDC_MadGameMode_e3Fields_1Delay==hMad->eGameMode) ||
		(BVDC_MadGameMode_e4Fields_1Delay==hMad->eGameMode))
		ulShift = 1; /* this used to work with 3-field 1-delay mode */
	else if(BVDC_MadGameMode_e5Fields_1Delay==hMad->eGameMode)
		ulShift = 2; /* shifting phase by 2 seems to work with 5-field 1-delay mode. */

	ulRev32_Field_Phase = (hMad->ulPhaseCounter + ulShift
		- ulRev32LockIndex) % 5;

	/* The next part of the algorithm is to determine if there is a bad weave */
	/* that would throw the 3:2 cadence out of lock. */
	/* First we need to determine */
	/* which direction is intended by the current locked phase (if we're */
	/* locked) or REV32_candidate (if we're not locked).  Rev32_candidate is */
	/* similar to rev32_locked_index but it is set before lock is actually */
	/* achieved */
	/* unlocked REV32 doesn't need to do bad weave detection */
	if(ulRev32Locked)
	{
		uint32_t           ulSelDetect = ulRev32_Field_Phase;
		uint32_t           ulUnlockIT = 0;
		uint32_t           ulSetVeto  = 0;
		uint32_t           ulSelectedPCC_y = 0;
		uint32_t           ulNonSelectedPCC_y = 0;
		uint32_t           ulSelectedPCC_c = 0;
		uint32_t           ulNonSelectedPCC_c = 0;

		/* Select/NonSelect_PCC table entries got rotated by 2 phases because
		ulSelDetect got shifted by 1 and 3-field mode needs to read PCC at the
		end of a picture;
		So the case 2 here corresponds to phase 0 in algorithm document; */
		switch(ulSelDetect)
		{
		case 0:
			/* sel_detect == 0 => IT_DETECT_MODE_SEL = FWD */
			ulItModeSel           = BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2,
				IT_MODE_SEL,   FWD);
			ulSelectedPCC_y       = pStats->ulPrevLumaPcc;                  /* prev_pcc luma   */
			ulSelectedPCC_c       = pStats->ulPrevChromaPcc;                /* prev_pcc chroma */
			ulNonSelectedPCC_y    = pStats->ulPccLumaPcc;                   /* curr_pcc luma   */
			ulNonSelectedPCC_c    = pStats->ulPccChromaPcc;                 /* curr_pcc chroma */
			break;
		case 1:
			/* sel_detect == 1 => IT_DETECT_MODE_SEL = BWD */
			ulItModeSel           = BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2,
				IT_MODE_SEL,   BWD);
			break;
		case 2:
			/* sel_detect == 2 => IT_DETECT_MODE_SEL = FWD */
			ulItModeSel           = BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2,
				IT_MODE_SEL,   FWD);

			ulSelectedPCC_y       = pStats->ulPccLumaPcc;                   /* curr_pcc luma   */
			ulSelectedPCC_c       = pStats->ulPccChromaPcc;                 /* curr_pcc chroma */
			ulNonSelectedPCC_y    = pStats->ulPrevLumaPcc;                  /* prev_pcc luma   */
			ulNonSelectedPCC_c    = pStats->ulPrevChromaPcc;                /* prev_pcc chroma */
			break;
		case 3:
			/* sel_detect == 3 => IT_DETECT_MODE_SEL = BWD */
			ulItModeSel           = BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2,
				IT_MODE_SEL,   BWD);
			ulSelectedPCC_y       = pStats->ulPrevLumaPcc;                  /* prev_pcc luma   */
			ulSelectedPCC_c       = pStats->ulPrevChromaPcc;                /* prev_pcc chroma */

			ulNonSelectedPCC_y    = pStats->ulPccLumaPcc;                   /* curr_pcc luma   */
			ulNonSelectedPCC_c    = pStats->ulPccChromaPcc;                 /* curr_pcc chroma */
			break;
		case 4:
			ulItModeSel           = BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2,
				IT_MODE_SEL,   AVG);

			ulSelectedPCC_y       = pStats->ulPccLumaPcc;                   /* curr_pcc luma   */
			ulSelectedPCC_c       = pStats->ulPccChromaPcc;                 /* curr_pcc chroma */
			ulNonSelectedPCC_y    = pStats->ulPrevLumaPcc;                  /* prev_pcc luma   */
			ulNonSelectedPCC_c    = pStats->ulPrevChromaPcc;                /* prev_pcc chroma */

		default:
			break;
		}

		/* Skip bad weave checking if the sel_detect = 4 due to limited info */
		if(ulSelDetect != 1)
		{
			uint32_t ulBwvCtrl1  = BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_1);
			uint32_t ulBwvCtrl3  = BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_3);
			uint32_t ulModeCtrl0 = BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0);

			if((ulSelectedPCC_y >= ulNonSelectedPCC_y + BCHP_GET_FIELD_DATA(ulBwvCtrl1,
				MAD_0_BWV_CONTROL_1, LUMA_32_THRESH)) ||
				(ulSelectedPCC_c >= ulNonSelectedPCC_c + BCHP_GET_FIELD_DATA(ulBwvCtrl3,
				MAD_0_BWV_CONTROL_3, CHROMA_32_THRESH)))
			{
				ulUnlockIT = 1;
				ulSetVeto  = BCHP_GET_FIELD_DATA(ulModeCtrl0,
					MAD_0_MODE_CONTROL_0, REV32_BAD_EDIT_PHASE_VETO);
			}
		}

		/* If the unlock_IT variable is set, clear the phase counters and the */
		/* lock state */
		if(ulUnlockIT)
		{
			BVDC_P_MAD_FLOG(("Unlock IT"));
			ulRev32Locked = 0;

			for(index = 0; index < 5; index++)
			{
				hMad->alPhase[index] = 0;
				pStats->aulV[index]    = ulSetVeto;
			}
		}
	}

	/* skip weave during hard start when usTrickModeStartDelay != 0 */
	if(!hMad->usTrickModeStartDelay)
	{
		/* this doesn't control weave in game mode, but affect OSD */
		BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_OUTPUT_CONTROL) &= ~(
			BCHP_MASK(MAD_0_IT_OUTPUT_CONTROL, PPBWV_MODE) |
			BCHP_MASK(MAD_0_IT_OUTPUT_CONTROL, PPUFM_MODE) |
			BCHP_MASK(MAD_0_IT_OUTPUT_CONTROL, PPRFM_MODE) |
			BCHP_MASK(MAD_0_IT_OUTPUT_CONTROL, AUTOREV32_ENABLE) |
			BCHP_MASK(MAD_0_IT_OUTPUT_CONTROL, REV32_LOCKED) |
			BCHP_MASK(MAD_0_IT_OUTPUT_CONTROL, REV32_FIELD_PHASE));
		BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_OUTPUT_CONTROL) |= (
			BCHP_FIELD_DATA(MAD_0_IT_OUTPUT_CONTROL, PPRFM_MODE, pStats->aulP[ulRev32LockIndex]) |
			BCHP_FIELD_DATA(MAD_0_IT_OUTPUT_CONTROL, REV32_LOCKED,              ulRev32Locked) |
			BCHP_FIELD_DATA(MAD_0_IT_OUTPUT_CONTROL, REV32_FIELD_PHASE,   ulRev32_Field_Phase));

		if(ulRev32Locked==1)
		{
			BVDC_P_MAD_FLOG(("REV32_FIELD_PHASE = %d, fnct=%d, phase[%d]=%d",
				ulRev32_Field_Phase, hMad->ulPhaseCounter,
				ulRev32LockIndex, hMad->alPhase[ulRev32LockIndex]));

			/* this control the rev32 weave in game mode */
			BVDC_P_MAD_GET_REG_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2) |= (
				BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2, IT_DEBUG_MODE_FORCE, ON) |
				ulItModeSel);
		}
	}

	hMad->ulPhaseCounter++;
	pStats->ulPrevLumaPcc     = pStats->ulPccLumaPcc;
	pStats->ulPrevChromaPcc = pStats->ulPccChromaPcc;

	BDBG_LEAVE(BVDC_P_Mad_SoftwareCadence32_isr);
	return;
}
#endif

/***************************************************************************
* {private}
* This functions should only be called when user changes deinterlace settings;
*
*/
static void BVDC_P_Mad_UserConfig_isr
( BVDC_P_Mad_Handle                hMad,
 const BVDC_Window_Handle         hWindow,
 const BFMT_VideoInfo            *pFmtInfo )
{
	BVDC_MadGameMode eGameMode;
	const BVDC_P_Deinterlace_Settings *pMadCfg;

	BDBG_ENTER(BVDC_P_Mad_UserConfig_isr);
	BDBG_OBJECT_ASSERT(hMad, BVDC_MAD);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);
	eGameMode = hMad->eGameMode;
	pMadCfg   = &hWindow->stCurInfo.stMadSettings;

	/* detect changes */
	if((hMad->bReverse32Pulldown != pMadCfg->bReverse32Pulldown) ||
		(hMad->ulUpdateAll)
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
		||
		((!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV32_ENTER_LOCK_LEVEL, pMadCfg->stReverse32Settings.ulEnterLockLevel) ||
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV32_PHASE_MATCH_THRESH, pMadCfg->stReverse32Settings.ulPhaseMatchThreshold) ||
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_1, REV32_REPF_PIX_CORRECT_LEVEL, pMadCfg->stReverse32Settings.ulRepfPxlCorrectLevel) ||
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_1, REV32_REPF_VETO_LEVEL, pMadCfg->stReverse32Settings.ulRepfVetoLevel) ||
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_2, REV32_BAD_EDIT_LEVEL, pMadCfg->stReverse32Settings.ulBadEditLevel) ||
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_2, REV32_LOCK_SAT_LEVEL, pMadCfg->stReverse32Settings.ulLockSatLevel) ||
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_BWV_CONTROL_1, LUMA_32_THRESH, pMadCfg->stReverse32Settings.ulBwvLuma32Threshold)) &&
		hMad->bRev32Custom && hWindow->stCurInfo.bRev32Custom) ||
		hMad->bRev32Custom != hWindow->stCurInfo.bRev32Custom)
#else
		)
#endif
	{
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
		uint32_t ulLuma32AvgThresh = 0x60e;
		BVDC_Deinterlace_Reverse32Settings   stRev32Settings;
#endif

		hMad->bReverse32Pulldown = pMadCfg->bReverse32Pulldown;
		hMad->ulUpdatePullDown = BVDC_P_RUL_UPDATE_THRESHOLD;
		hMad->bRev32Custom = hWindow->stCurInfo.bRev32Custom;

		/* adjust */
		BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_OUTPUT_CONTROL) &= ~(
			/* clear lock status */
			BCHP_MASK(MAD_0_IT_OUTPUT_CONTROL, REV32_LOCKED) |
			BCHP_MASK(MAD_0_IT_OUTPUT_CONTROL, AUTOREV32_ENABLE) );
		BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_OUTPUT_CONTROL) |=  (
			/* set rev32 and rev22 */
			BCHP_FIELD_DATA(MAD_0_IT_OUTPUT_CONTROL, AUTOREV32_ENABLE,
			((BVDC_MadGameMode_e3Fields_1Delay!= eGameMode) &&
			 (BVDC_MadGameMode_e5Fields_1Delay!= eGameMode) &&
			 (BVDC_MadGameMode_e4Fields_1Delay!= eGameMode))
			? pMadCfg->bReverse32Pulldown : 0));
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
		/* 3:2 pulldown user setting */
		BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_0) &= ~(
			BCHP_MASK(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV32_ENTER_LOCK_LEVEL) |
			BCHP_MASK(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV32_PHASE_MATCH_THRESH));
		BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_1) &= ~(
			BCHP_MASK(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_1, REV32_REPF_PIX_CORRECT_LEVEL) |
			BCHP_MASK(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_1, REV32_REPF_VETO_LEVEL));
		BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_2) &= ~(
			BCHP_MASK(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_2, REV32_BAD_EDIT_LEVEL) |
			BCHP_MASK(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_2, REV32_LOCK_SAT_LEVEL));
		BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_1) &= ~(
			BCHP_MASK(MAD_0_BWV_CONTROL_1, LUMA_32_AVG_THRESH) |
			BCHP_MASK(MAD_0_BWV_CONTROL_1, LUMA_32_THRESH));

		if(!hWindow->stCurInfo.bRev32Custom) /* Use default set */
		{
			/* Oversample */
			/* ulLuma32AvgThresh is not in BVDC_Deinterlace_Reverse32Settings */
			if((hWindow->stCurInfo.hSource->ulSampleFactor) && VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt))
			{
				/* PR45231, PR45707 */
				ulLuma32AvgThresh = 0x1100;
			}

			/* Get dynamic default settings */
			BVDC_P_Mad_Init_Rev32_DynamicDefault(pFmtInfo,
				hWindow->stCurInfo.hSource->ulSampleFactor, &stRev32Settings);
		}
		else
		{
			stRev32Settings = pMadCfg->stReverse32Settings;
		}

		BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_0) |=  (
			BCHP_FIELD_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV32_ENTER_LOCK_LEVEL,  stRev32Settings.ulEnterLockLevel) |
			BCHP_FIELD_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_0, REV32_PHASE_MATCH_THRESH, stRev32Settings.ulPhaseMatchThreshold));

		BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_1) |=  (
			BCHP_FIELD_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_1, REV32_REPF_PIX_CORRECT_LEVEL, stRev32Settings.ulRepfPxlCorrectLevel) |
			BCHP_FIELD_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_1, REV32_REPF_VETO_LEVEL, stRev32Settings.ulRepfVetoLevel));

		BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_2) |=  (
			BCHP_FIELD_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_2, REV32_BAD_EDIT_LEVEL, stRev32Settings.ulBadEditLevel) |
			BCHP_FIELD_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_2, REV32_LOCK_SAT_LEVEL, stRev32Settings.ulLockSatLevel));

		BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_1) |=  (
			BCHP_FIELD_DATA(MAD_0_BWV_CONTROL_1, LUMA_32_AVG_THRESH,  ulLuma32AvgThresh) |
			BCHP_FIELD_DATA(MAD_0_BWV_CONTROL_1, LUMA_32_THRESH, stRev32Settings.ulBwvLuma32Threshold));
#endif /* phase >= 3 */
	}

	if(pFmtInfo->ulDigitalHeight <= BFMT_PAL_HEIGHT)
	{
		if(hMad->bReverse22Pulldown != pMadCfg->bReverse22Pulldown)
		{
			hMad->bReverse22Pulldown = pMadCfg->bReverse22Pulldown;
			hMad->ulUpdate22PullDown = BVDC_P_RUL_UPDATE_THRESHOLD;
			if (BVDC_MadGameMode_eOff == eGameMode)
			{
				hMad->alPhase22[0] = 0;
				hMad->alPhase22[1] = 0;

				BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_OUTPUT_CONTROL) &= ~(
					BCHP_MASK(MAD_0_IT_OUTPUT_CONTROL, REV22_LOCKED) |
					BCHP_MASK(MAD_0_IT_OUTPUT_CONTROL, REV22_FIELD_PHASE));
			}
		}
	}

	if(hWindow->stCurInfo.bRev22Custom) /*custome setting */
	{
		hMad->ulRev22NonMatchMatchRatio = pMadCfg->stReverse22Settings.ulNonMatchMatchRatio;
		hMad->ulRev22EnterLockLevel = pMadCfg->stReverse22Settings.ulEnterLockLevel;
		hMad->ulRev22LocksatLevel = pMadCfg->stReverse22Settings.ulLockSatLevel;
		hMad->ulRev22BwvLumaThreshold = pMadCfg->stReverse22Settings.ulBwvLuma22Threshold;
	}
	else
	{
		BVDC_Deinterlace_Reverse22Settings  stRev22Settings;
		BVDC_P_Mad_Init_Rev22_DynamicDefault(pFmtInfo, &stRev22Settings);

		hMad->ulRev22NonMatchMatchRatio = stRev22Settings.ulNonMatchMatchRatio;
		hMad->ulRev22EnterLockLevel     = stRev22Settings.ulEnterLockLevel;
		hMad->ulRev22LocksatLevel       = stRev22Settings.ulLockSatLevel;
	}

	hMad->bRev22Custom = hWindow->stCurInfo.bRev22Custom;

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
	if(((!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_MODE_CONTROL_0, CHROMA_FIELD_420_EDGE_DET_MODE, pMadCfg->stChromaSettings.bChromaField420EdgeDetMode)||
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_MODE_CONTROL_0, CHROMA_FIELD_420_INITIAL_PHASE, pMadCfg->stChromaSettings.bChromaField420InitPhase) ||
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_MODE_CONTROL_0, CHROMA_FIELD_420_INV_METHOD, pMadCfg->stChromaSettings.eChromaField420InvMethod) ||
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_MODE_CONTROL_0, CHROMA_422_MOTION_MODE, pMadCfg->stChromaSettings.eChroma422MotionMode) ||
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_MODE_CONTROL_0, CHROMA_FIELD_420_MOTION_MODE, pMadCfg->stChromaSettings.eChroma420MotionMode) ||
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_XCHROMA_MODE, PIC_422_MA, pMadCfg->stChromaSettings.eChroma422MotionAdaptiveMode) ||
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_XCHROMA_MODE, PIC_422_IT, pMadCfg->stChromaSettings.eChroma422InverseTelecineMode) ||
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_XCHROMA_MODE, FIELD_420_MA, pMadCfg->stChromaSettings.eChroma420MotionAdaptiveMode) ||
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_7)
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_MOTION_CAL_CTRL, MS_3548, pMadCfg->stChromaSettings.bMS_3548) ||
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_MOTION_CAL_CTRL, MT_3548, pMadCfg->stChromaSettings.bMT_3548) ||
#endif
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_XCHROMA_CONTROL_0, MAX_XCHROMA, pMadCfg->stChromaSettings.ulMaxXChroma)) &&
		hMad->bChromaCustom && hWindow->stCurInfo.bChromaCustom) ||
		hMad->bChromaCustom != hWindow->stCurInfo.bChromaCustom)
	{
		hMad->ulUpdateChromaSettings = BVDC_P_RUL_UPDATE_THRESHOLD;
		hMad->bChromaCustom = hWindow->stCurInfo.bChromaCustom;

		BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) &= ~(
			BCHP_MASK(MAD_0_MODE_CONTROL_0, CHROMA_FIELD_420_EDGE_DET_MODE) |
			BCHP_MASK(MAD_0_MODE_CONTROL_0, CHROMA_FIELD_420_INV_METHOD));
		if(!hWindow->stCurInfo.bChromaCustom)	/* use default set */
		{
			BVDC_Deinterlace_ChromaSettings  stChromaSettings;

			/* bCvbsSrc doesn't matter */
			BVDC_P_Mad_Init_Chroma_DynamicDefault(pFmtInfo, eGameMode,
				BVDC_P_SRC_IS_MPEG(hWindow->stCurInfo.hSource->eId), false, &stChromaSettings);

			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
				BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, CHROMA_FIELD_420_EDGE_DET_MODE,
				stChromaSettings.bChromaField420EdgeDetMode) |
				BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, CHROMA_FIELD_420_INV_METHOD,
				stChromaSettings.eChromaField420InvMethod));

			hMad->ulUpdateAll = BVDC_P_RUL_UPDATE_THRESHOLD;
		}
		else
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) &= ~(
				BCHP_MASK(MAD_0_MODE_CONTROL_0, CHROMA_422_MOTION_MODE) |
				BCHP_MASK(MAD_0_MODE_CONTROL_0, CHROMA_FIELD_420_MOTION_MODE) |
				BCHP_MASK(MAD_0_MODE_CONTROL_0, CHROMA_FIELD_420_INITIAL_PHASE));
			BVDC_P_MAD_GET_REG_DATA(MAD_0_XCHROMA_MODE) &= ~(
				BCHP_MASK(MAD_0_XCHROMA_MODE, FIELD_420_MA) |
				BCHP_MASK(MAD_0_XCHROMA_MODE, PIC_422_MA) |
				BCHP_MASK(MAD_0_XCHROMA_MODE, PIC_422_IT));
			BVDC_P_MAD_GET_REG_DATA(MAD_0_XCHROMA_CONTROL_0) &= ~(
				BCHP_MASK(MAD_0_XCHROMA_CONTROL_0, MAX_XCHROMA));

			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
				BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, CHROMA_422_MOTION_MODE,  pMadCfg->stChromaSettings.eChroma422MotionMode) |
				BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, CHROMA_FIELD_420_MOTION_MODE,  pMadCfg->stChromaSettings.eChroma420MotionMode) |
				BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, CHROMA_FIELD_420_EDGE_DET_MODE,  pMadCfg->stChromaSettings.bChromaField420EdgeDetMode) |
				BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, CHROMA_FIELD_420_INITIAL_PHASE, pMadCfg->stChromaSettings.bChromaField420InitPhase) |
				BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, CHROMA_FIELD_420_INV_METHOD, pMadCfg->stChromaSettings.eChromaField420InvMethod));
			BVDC_P_MAD_GET_REG_DATA(MAD_0_XCHROMA_MODE) |=  (
				BCHP_FIELD_DATA(MAD_0_XCHROMA_MODE, FIELD_420_MA,  pMadCfg->stChromaSettings.eChroma420MotionAdaptiveMode) |
				BCHP_FIELD_DATA(MAD_0_XCHROMA_MODE, PIC_422_MA,  pMadCfg->stChromaSettings.eChroma422MotionAdaptiveMode) |
				BCHP_FIELD_DATA(MAD_0_XCHROMA_MODE, PIC_422_IT, pMadCfg->stChromaSettings.eChroma422InverseTelecineMode));
			BVDC_P_MAD_GET_REG_DATA(MAD_0_XCHROMA_CONTROL_0) |=  (
				BCHP_FIELD_DATA(MAD_0_XCHROMA_CONTROL_0, MAX_XCHROMA, pMadCfg->stChromaSettings.ulMaxXChroma & 0xFF));
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_7)
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MOTION_CAL_CTRL) &= ~(
				BCHP_MASK(MAD_0_MOTION_CAL_CTRL, MS_3548) |
				BCHP_MASK(MAD_0_MOTION_CAL_CTRL, MT_3548));
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MOTION_CAL_CTRL) |=  (
				BCHP_FIELD_DATA(MAD_0_MOTION_CAL_CTRL, MS_3548,  pMadCfg->stChromaSettings.bMS_3548) |
				BCHP_FIELD_DATA(MAD_0_MOTION_CAL_CTRL, MT_3548, pMadCfg->stChromaSettings.bMT_3548));
#endif
		}
	}

	if(((!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_MODE_CONTROL_0, TM_MODE_SEL, pMadCfg->stMotionSettings.eTmMode) ||
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_MODE_CONTROL_0, SM_MODE_SEL, pMadCfg->stMotionSettings.eSmMode)) &&
		hMad->bMotionCustom && hWindow->stCurInfo.bMotionCustom) ||
		hMad->bMotionCustom != hWindow->stCurInfo.bMotionCustom)
	{
		BDBG_MSG(("mad motion update set"));
		hMad->ulUpdateMotionSettings = BVDC_P_RUL_UPDATE_THRESHOLD;
		hMad->bMotionCustom = hWindow->stCurInfo.bMotionCustom;
	}

	if(((!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_M_ENABLE, pMadCfg->stMotionSettings.bEnableQmM) ||
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_L_ENABLE, pMadCfg->stMotionSettings.bEnableQmL) ||
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_K_ENABLE, pMadCfg->stMotionSettings.bEnableQmK)) &&
		hMad->bMotionCustom && hWindow->stCurInfo.bMotionCustom &&
		((BVDC_MadGameMode_eOff == eGameMode) ||
		(BVDC_MadGameMode_eHardStart == eGameMode))) ||
		hMad->bMotionCustom != hWindow->stCurInfo.bMotionCustom)
	{
		BDBG_MSG(("mad motion update set2"));
		hMad->ulUpdateMotionSettings = BVDC_P_RUL_UPDATE_THRESHOLD;
		hMad->bMotionCustom = hWindow->stCurInfo.bMotionCustom;
	}
#endif

#if(BVDC_P_SUPPORT_BVN_10BIT_444)
	{
		bool bUsRingMode, bUsRingSuprresion;

		bool bDnRingSuprresion = (BVDC_444To422Filter_eStandard == pMadCfg->stDnSampler.eFilterType)
			? BCHP_MAD_0_TOP_444_TO_422_DS_CONTROL_RING_SUPPRESSION_ENABLE
			: BCHP_MAD_0_TOP_444_TO_422_DS_CONTROL_RING_SUPPRESSION_DISABLE;

		if((pMadCfg->stUpSampler.eFilterType == BVDC_422To444Filter_eTenTaps) ||
			(pMadCfg->stUpSampler.eFilterType == BVDC_422To444Filter_eSixTaps))
		{
			bUsRingMode = BCHP_MAD_0_TOP_422_TO_444_US_CONTROL_RING_SUPPION_MODE_DOUBLE;
			bUsRingSuprresion = BCHP_MAD_0_TOP_422_TO_444_US_CONTROL_RING_SUPPION_ENABLE;
		}
		else
		{
			bUsRingMode = BCHP_MAD_0_TOP_422_TO_444_US_CONTROL_RING_SUPPION_MODE_NORMAL;
			bUsRingSuprresion = BCHP_MAD_0_TOP_422_TO_444_US_CONTROL_RING_SUPPION_DISABLE;
		}
		if(!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_TOP_422_TO_444_US_CONTROL, FILT_CTRL, pMadCfg->stUpSampler.eFilterType) ||
			!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_TOP_422_TO_444_US_CONTROL, RING_SUPPION, bUsRingSuprresion) ||
			!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_TOP_422_TO_444_US_CONTROL, RING_SUPPION_MODE, bUsRingMode) ||
			!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_TOP_444_TO_422_DS_CONTROL, FILTER_TYPE, pMadCfg->stDnSampler.eFilterType) ||
			!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_TOP_444_TO_422_DS_CONTROL, RING_SUPPRESSION, bDnRingSuprresion))
		{
			/* filter down sample pixels */
			BVDC_P_MAD_GET_REG_DATA(MAD_0_TOP_422_TO_444_US_CONTROL) = (
				BCHP_FIELD_DATA(MAD_0_TOP_422_TO_444_US_CONTROL, RING_SUPPION_MODE,
				bUsRingMode                                                          ) |
				BCHP_FIELD_DATA(MAD_0_TOP_422_TO_444_US_CONTROL, RING_SUPPION,
				bUsRingSuprresion                                                    ) |
				BCHP_FIELD_DATA(MAD_0_TOP_422_TO_444_US_CONTROL, UNBIASED_ROUND_ENABLE,
				pMadCfg->stUpSampler.bUnbiasedRound                                  ) |
				BCHP_FIELD_DATA(MAD_0_TOP_422_TO_444_US_CONTROL, FILT_CTRL,
				pMadCfg->stUpSampler.eFilterType                                     ));

			/* filter up sample pixels */
			BVDC_P_MAD_GET_REG_DATA(MAD_0_TOP_444_TO_422_DS_CONTROL) = (
				BCHP_FIELD_DATA(MAD_0_TOP_444_TO_422_DS_CONTROL, RING_SUPPRESSION,
				bDnRingSuprresion                                                    ) |
				BCHP_FIELD_DATA(MAD_0_TOP_444_TO_422_DS_CONTROL, FILTER_TYPE,
				pMadCfg->stDnSampler.eFilterType                                     ));
			hMad->ulUpdateMisc = BVDC_P_RUL_UPDATE_THRESHOLD;
		}
	}
#endif

	/* Low angle setting */
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
	if(!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_LA_MIN_NORTH_STRENGTH, VALUE, pMadCfg->stLowAngles.ulLaMinNorthStrength ) ||
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_LA_STRENGTH_CONTROL, RANGE_LIMIT_SCALE, pMadCfg->stLowAngles.ulLaControlRangeLimitScale) ||
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_LA_STRENGTH_CONTROL, DIR_RATIO, pMadCfg->stLowAngles.ulLaControlDirRatio))
	{
		BDBG_MSG(("=== update low angles setting"));
		BVDC_P_MAD_GET_REG_DATA(MAD_0_LA_STRENGTH_CONTROL) = (
			BCHP_FIELD_DATA(MAD_0_LA_STRENGTH_CONTROL, RANGE_LIMIT_SCALE,
			pMadCfg->stLowAngles.ulLaControlRangeLimitScale                      ) |
			BCHP_FIELD_DATA(MAD_0_LA_STRENGTH_CONTROL, DIR_RATIO_NORTH,
			3                                                                    ) |
			BCHP_FIELD_DATA(MAD_0_LA_STRENGTH_CONTROL, DIR_RATIO,
			pMadCfg->stLowAngles.ulLaControlDirRatio                             ) |
			BCHP_FIELD_DATA(MAD_0_LA_STRENGTH_CONTROL, USE_CHROMA_LUMA_STRENGTH,
			8                                                                    ));
		/* filter up sample pixels */
		BVDC_P_MAD_GET_REG_DATA(MAD_0_LA_MIN_NORTH_STRENGTH) = (
			BCHP_FIELD_DATA(MAD_0_LA_MIN_NORTH_STRENGTH, VALUE,
			pMadCfg->stLowAngles.ulLaMinNorthStrength                            ));
		hMad->ulUpdateMisc = BVDC_P_RUL_UPDATE_THRESHOLD;
	}
#endif

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
	if(hMad->ulUpdateMotionSettings)
	{

		BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) &= ~(
			BCHP_MASK(MAD_0_MODE_CONTROL_0, TM_MODE_SEL) |
			BCHP_MASK(MAD_0_MODE_CONTROL_0, SM_MODE_SEL));
		if(!hWindow->stCurInfo.bMotionCustom)
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, TM_MODE_SEL, BOOST2) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, SM_MODE_SEL, BOOST1));
		}
		else
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
				BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, TM_MODE_SEL,  pMadCfg->stMotionSettings.eTmMode) |
				BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, SM_MODE_SEL, pMadCfg->stMotionSettings.eSmMode));
		}

		if((BVDC_MadGameMode_eOff == eGameMode) ||
			(BVDC_MadGameMode_eHardStart == eGameMode))
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) &= ~(
				BCHP_MASK(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_M_ENABLE) |
				BCHP_MASK(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_L_ENABLE) |
				BCHP_MASK(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_K_ENABLE));
			if(!hWindow->stCurInfo.bMotionCustom)
			{
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_K_ENABLE, ON) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_L_ENABLE, ON) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_M_ENABLE, ON));
			}
			else
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
				BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_M_ENABLE, pMadCfg->stMotionSettings.bEnableQmM) |
				BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_L_ENABLE, pMadCfg->stMotionSettings.bEnableQmL) |
				BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_K_ENABLE, pMadCfg->stMotionSettings.bEnableQmK));
		}
	}
#endif /* phase >= 3 */
	BDBG_LEAVE(BVDC_P_Mad_UserConfig_isr);
	return;
}

/***************************************************************************
* {private}
* This function mainly rotate MAD game mode field stores and QM stores pointers;
*/
static void BVDC_P_Mad_GameMode_isr
( BVDC_P_Mad_Handle                hMad,
 const BVDC_Window_Handle         hWindow,
 const BVDC_P_PictureNodePtr      pPicture )
{
	BVDC_MadGameMode eGameMode;
	const BVDC_P_Deinterlace_Settings *pMadCfg;
	uint16_t feed_J = 0, feed_G = 0, feed_B = 0, feed_D = 0, cur_field = 0;
	uint16_t qm_feed_K = 0, qm_feed_L = 0, qm_feed_M = 0;
	uint32_t qm_capture = 0, feed_capture = 0, cur_qm = 0;

	BDBG_OBJECT_ASSERT(hMad, BVDC_MAD);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	eGameMode = hMad->eGameMode;
	pMadCfg   = &hWindow->stCurInfo.stMadSettings;

	/* get the previous pointers */
	feed_capture = hMad->usFeedCapture;
	cur_qm = hMad->usCurQm;

	/* 0) get cur_field type;
	Note: eSrcPolarity is the next field type to be programmed; */
	cur_field = (pPicture->eSrcPolarity == BAVC_Polarity_eBotField) ?
BAVC_Polarity_eTopField : BAVC_Polarity_eBotField;

	/* 1) calculate fields store feed counters */
	/* rotate the feed_capture pointer to the next field store */
	switch(eGameMode)
	{
	case BVDC_MadGameMode_e5Fields_1Delay:
		feed_capture  = ( feed_capture + 1 ) % 5;

		if(hMad->ulGameModeTransCnt == 2)
		{
			/* transition from 1-delay to 5-field 2-delay state HERE! */
			feed_J =  ( feed_capture - 2 + 5 ) % 5;
			feed_B =  ( feed_capture - 1 + 5 ) % 5;
			feed_D =  ( feed_capture - 4 + 5 ) % 5;
			feed_G =  ( feed_capture - 3 + 5 ) % 5;

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_5)
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) &=  ~(
				BCHP_MASK(MAD_0_MODE_CONTROL_2, ABS_DIFF_SRC_SEL) |
				BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_A_SRC_SEL) |
				BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_D_SRC_SEL) |
				BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_B_SRC_SEL) |
				BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL));
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) |=  (
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, ABS_DIFF_SRC_SEL, USE_ORIGINAL_PIXEL ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_A_SRC_SEL,  USE_FIELD_G ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_D_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_B_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL,  USE_ORIGINAL_PIXEL ));
#endif

			/* next VSYNC will resume to normal mode */
			hMad->eGameMode = pMadCfg->eGameMode;
			hMad->ulUpdateAll = BVDC_P_RUL_UPDATE_THRESHOLD;

			/* count down */
			hMad->ulGameModeTransCnt = 1;

			BDBG_MSG(("ulGameModeTransCnt = %d ", hMad->ulGameModeTransCnt));
		}
		else
		{
			feed_J =  ( feed_capture - 1 + 5 ) % 5;
			feed_B =  ( feed_capture - 2 + 5 ) % 5;
			feed_D =  ( feed_capture - 3 + 5 ) % 5;
			feed_G =  ( feed_capture - 4 + 5 ) % 5;
		}
		break;

	case BVDC_MadGameMode_e5Fields_0Delay:
		feed_capture  = ( feed_capture + 1 ) % 5;

		if(hMad->ulGameModeTransCnt == 3)
		{
			/* transition from 0-delay to 5-field 1-delay state HERE! */
			feed_J =  ( feed_capture - 1 + 5 ) % 5;
			feed_B =  ( feed_capture - 2 + 5 ) % 5;
			feed_D =  ( feed_capture - 3 + 5 ) % 5;
			feed_G =  ( feed_capture - 4 + 5 ) % 5;

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_5)
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) &=  ~(
				BCHP_MASK(MAD_0_MODE_CONTROL_2, ABS_DIFF_SRC_SEL) |
				BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_A_SRC_SEL) |
				BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_D_SRC_SEL) |
				BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_B_SRC_SEL) |
				BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL));
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) |=  (
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, ABS_DIFF_SRC_SEL, USE_ORIGINAL_PIXEL ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_A_SRC_SEL,  USE_FIELD_G ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_D_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_B_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL,  USE_FIELD_A));
#endif

			/* count down */
			hMad->ulGameModeTransCnt = 2;

			BDBG_MSG(("ulGameModeTransCnt = %d", hMad->ulGameModeTransCnt));
		}
		else if(hMad->ulGameModeTransCnt == 2)
		{
			/* transition from 1-delay to 5-field 2-delay state HERE! */
			feed_J =  ( feed_capture - 2 + 5 ) % 5;
			feed_B =  ( feed_capture - 1 + 5 ) % 5;
			feed_D =  ( feed_capture - 4 + 5 ) % 5;
			feed_G =  ( feed_capture - 3 + 5 ) % 5;
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_5)
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) &=  ~(
				BCHP_MASK(MAD_0_MODE_CONTROL_2, ABS_DIFF_SRC_SEL) |
				BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_A_SRC_SEL) |
				BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_D_SRC_SEL) |
				BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_B_SRC_SEL) |
				BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL));
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) |=  (
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, ABS_DIFF_SRC_SEL, USE_ORIGINAL_PIXEL ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_A_SRC_SEL,  USE_FIELD_G ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_D_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_B_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL,  USE_ORIGINAL_PIXEL ));
#endif

			/* next VSYNC will resume to normal mode */
			hMad->eGameMode = pMadCfg->eGameMode;
			hMad->ulUpdateAll = BVDC_P_RUL_UPDATE_THRESHOLD;

			/* count down */
			hMad->ulGameModeTransCnt = 1;
			BDBG_MSG(("ulGameModeTransCnt = %d", hMad->ulGameModeTransCnt));
		}
		else
		{
			feed_J =  ( feed_capture - 1 + 5 ) % 5;
			feed_G =  ( feed_capture - 0 + 5 ) % 5;
			feed_B =  ( feed_capture - 3 + 5 ) % 5;
			feed_D =  ( feed_capture - 2 + 5 ) % 5;
		}
		break;

	case BVDC_MadGameMode_e4Fields_1Delay:
			if(hWindow->stSettings.bDeinterlacerAllocFull)
			{
				feed_capture  = ( feed_capture + 1 ) % 5;

				if(hMad->ulGameModeTransCnt == 2)
				{
					/* transition from 1-delay to 4-field 2-delay state HERE! */

					feed_B  =  ( feed_capture - 1 + 5 ) % 5;
					feed_J  =  ( feed_capture - 2 + 5 ) % 5;
					feed_G  =  ( feed_capture - 3 + 5 ) % 5;
					feed_D  =  ( feed_capture - 4 + 5 ) % 5;


					hMad->eGameMode = pMadCfg->eGameMode;
					hMad->ulUpdateAll = BVDC_P_RUL_UPDATE_THRESHOLD;
					hMad->ulGameModeTransCnt--;

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_5)
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) &=  ~(
					BCHP_MASK(MAD_0_MODE_CONTROL_2, ABS_DIFF_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_A_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_D_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_B_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL));
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) |=  (
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, ABS_DIFF_SRC_SEL, USE_PIXEL_B_AND_G ) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_A_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_D_SRC_SEL,  USE_FIELD_J ) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_B_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL,  USE_ORIGINAL_PIXEL ));
#endif

				/* enable feed G */
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) &=  ~(
					BCHP_MASK(MAD_0_MODE_CONTROL_0, FIELD_G_ENABLE));
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_G_ENABLE, ON));
				}
				else
				{

				    feed_D =  ( feed_capture - 3 + 5 ) % 5;
				    feed_B =  ( feed_capture - 2 + 5 ) % 5;
				    feed_J =  ( feed_capture - 1 + 5 ) % 5;

				}

			}
			else /* only 4-buffers allocated */
			{
				feed_capture  = ( feed_capture + 1 ) & 3;

				feed_D =  ( feed_capture - 3 + 4 ) & 3;
				feed_B =  ( feed_capture - 2 + 4 ) & 3;
				feed_J =  ( feed_capture - 1 + 4 ) & 3;

				hMad->ulGameModeTransCnt = 0;

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_5)

				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) &=  ~(
					BCHP_MASK(MAD_0_MODE_CONTROL_2, ABS_DIFF_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_A_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_D_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_B_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL));
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) |=  (
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, ABS_DIFF_SRC_SEL, USE_PIXEL_B_AND_G ) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_A_SRC_SEL,  USE_ORIGINAL_PIXEL) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_D_SRC_SEL,  USE_ORIGINAL_PIXEL) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_B_SRC_SEL,  USE_ORIGINAL_PIXEL) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL,  USE_FIELD_A ));

                /*Disable G feeder*/
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) &=  ~(
					BCHP_MASK(MAD_0_MODE_CONTROL_0, FIELD_G_ENABLE));
#endif

			}
			break;
	case BVDC_MadGameMode_e4Fields_0Delay:
	case BVDC_MadGameMode_e4Fields_ForceSpatial:
		if(hWindow->stSettings.bDeinterlacerAllocFull)
		{
			feed_capture  = ( feed_capture + 1 ) % 5;
			if(hMad->ulGameModeTransCnt == 3)
			{
				/* transition from 0-delay to 5-field 1-delay state HERE! */
				feed_J =  ( feed_capture - 1 + 5 ) % 5;
				feed_B =  ( feed_capture - 2 + 5 ) % 5;
				feed_D =  ( feed_capture - 3 + 5 ) % 5;
				feed_G =  ( feed_capture - 4 + 5 ) % 5;
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_5)
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) &=  ~(
					BCHP_MASK(MAD_0_MODE_CONTROL_2, ABS_DIFF_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_A_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_D_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_B_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL));
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) |=  (
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, ABS_DIFF_SRC_SEL, USE_ORIGINAL_PIXEL ) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_A_SRC_SEL,  USE_FIELD_G ) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_D_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_B_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL,  USE_FIELD_A));
#endif
				/* enable feed G */
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) &=  ~(
					BCHP_MASK(MAD_0_MODE_CONTROL_0, FIELD_G_ENABLE));
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_G_ENABLE, ON));

				/* count down */
				hMad->ulGameModeTransCnt = 2;
				BDBG_MSG(("ulGameModeTransCnt = %d", hMad->ulGameModeTransCnt));
			}
			else if(hMad->ulGameModeTransCnt == 2)
			{
				/* transition from 1-delay to 5-field 2-delay state HERE! */
				feed_J =  ( feed_capture - 2 + 5 ) % 5;
				feed_B =  ( feed_capture - 1 + 5 ) % 5;
				feed_D =  ( feed_capture - 4 + 5 ) % 5;
				feed_G =  ( feed_capture - 3 + 5 ) % 5;

				/* enable feed G */
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) &=  ~(
					BCHP_MASK(MAD_0_MODE_CONTROL_0, FIELD_G_ENABLE));
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_G_ENABLE, ON));

				/* next VSYNC will resume to normal mode */
				hMad->eGameMode = pMadCfg->eGameMode;
				hMad->ulUpdateAll = BVDC_P_RUL_UPDATE_THRESHOLD;

				/* count down */
				hMad->ulGameModeTransCnt = 1;
				BDBG_MSG(("ulGameModeTransCnt = %d", hMad->ulGameModeTransCnt));
			}
			else
			{
				feed_J =  ( feed_capture - 1 + 5 ) % 5;
				feed_G =  ( feed_capture - 1 + 5 ) % 5;
				feed_B =  ( feed_capture - 3 + 5 ) % 5;
				feed_D =  ( feed_capture - 2 + 5 ) % 5;
			}
		}
		else /* only 4-buffers allocated */
		{
			feed_capture  = ( feed_capture + 1 ) % 4;
			feed_J =  ( feed_capture - 1 + 4 ) % 4;
			feed_G =  ( feed_capture - 1 + 4 ) % 4;
			feed_B =  ( feed_capture - 3 + 4 ) % 4;
			feed_D =  ( feed_capture - 2 + 4 ) % 4;
		}
		break;

	case BVDC_MadGameMode_e3Fields_1Delay:
		if(hWindow->stSettings.bDeinterlacerAllocFull)
		{
			feed_capture  = ( feed_capture + 1 ) % 5;
			if(hMad->ulGameModeTransCnt == 2)
			{
				/* transition from 1-delay to 4-field 2-delay state HERE! */
				feed_B =  ( feed_capture - 1 + 5 ) % 5;
				feed_J =  ( feed_capture - 2 + 5 ) % 5;
				feed_G =  ( feed_capture - 3 + 5 ) % 5;
				/* next VSYNC will resume to normal mode */
				hMad->eGameMode = pMadCfg->eGameMode;
				hMad->ulUpdateAll = BVDC_P_RUL_UPDATE_THRESHOLD;

				/* count down */
				hMad->ulGameModeTransCnt = 1;

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_5)
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) &=  ~(
					BCHP_MASK(MAD_0_MODE_CONTROL_2, ABS_DIFF_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_A_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_D_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_B_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL));
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) |=  (
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, ABS_DIFF_SRC_SEL, USE_PIXEL_B_AND_G ) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_A_SRC_SEL,  USE_FIELD_G ) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_D_SRC_SEL,  USE_FIELD_J ) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_B_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL,  USE_ORIGINAL_PIXEL ));
#endif

				/* enable feed G */
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) &=  ~(
					BCHP_MASK(MAD_0_MODE_CONTROL_0, FIELD_G_ENABLE));
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_G_ENABLE, ON));

				BDBG_MSG(("ulGameModeTransCnt = %d", hMad->ulGameModeTransCnt));
			}
			else
			{
				/* cycle through all 5-field stores with 3-field active at a time */
				feed_J =  ( feed_capture - 1 + 5 ) % 5;
				feed_B =  ( feed_capture - 2 + 5 ) % 5;
			}
		}
		else /* only 3-buffers allocated */
		{
			feed_capture  = ( feed_capture + 1 ) % 3;

			feed_J =  ( feed_capture - 1 + 3 ) % 3;
			feed_B =  ( feed_capture - 2 + 3 ) % 3;
		}
		break;

	case BVDC_MadGameMode_e3Fields_0Delay:
	case BVDC_MadGameMode_e3Fields_ForceSpatial:
		if(hWindow->stSettings.bDeinterlacerAllocFull)
		{
			feed_capture  = ( feed_capture + 1 ) % 5;
			if(hMad->ulGameModeTransCnt == 3)
			{
				/* transition from 0-delay to 4-field 1-delay state HERE! */
				feed_J =  ( feed_capture - 1 + 5 ) % 5;
				feed_B =  ( feed_capture - 2 + 5 ) % 5;
				feed_D =  ( feed_capture - 3 + 5 ) % 5;
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_5)
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) &=  ~(
					BCHP_MASK(MAD_0_MODE_CONTROL_2, ABS_DIFF_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_A_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_D_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_B_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL));
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) |=  (
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, ABS_DIFF_SRC_SEL, USE_PIXEL_B_AND_G ) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_A_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_D_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_B_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL,  USE_FIELD_A ));
#endif

				/* enable feed B */
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) &=  ~(
					BCHP_MASK(MAD_0_MODE_CONTROL_0, FIELD_B_ENABLE));
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_B_ENABLE, ON));

				/* count down */
				hMad->ulGameModeTransCnt = 2;
				BDBG_MSG(("ulGameModeTransCnt = %d", hMad->ulGameModeTransCnt));
			}
			else if(hMad->ulGameModeTransCnt == 2)
			{
				/* transition from 1-delay to 5-field 2-delay state HERE! */
				feed_J =  ( feed_capture - 2 + 5 ) % 5;
				feed_B =  ( feed_capture - 1 + 5 ) % 5;
				feed_D =  ( feed_capture - 4 + 5 ) % 5;
				feed_G =  ( feed_capture - 3 + 5 ) % 5;

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_5)
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) &=  ~(
					BCHP_MASK(MAD_0_MODE_CONTROL_2, ABS_DIFF_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_A_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_D_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_B_SRC_SEL) |
					BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL));
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) |=  (
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, ABS_DIFF_SRC_SEL, USE_PIXEL_B_AND_G ) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_A_SRC_SEL,  USE_FIELD_G ) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_D_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_B_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL,  USE_ORIGINAL_PIXEL ));
#endif

				/* enable feed G */
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) &=  ~(
					BCHP_MASK(MAD_0_MODE_CONTROL_0, FIELD_B_ENABLE) |
					BCHP_MASK(MAD_0_MODE_CONTROL_0, FIELD_G_ENABLE));
				BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_B_ENABLE, ON) |
					BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_G_ENABLE, ON));

				/* next VSYNC will resume to normal mode */
				hMad->eGameMode = pMadCfg->eGameMode;
				hMad->ulUpdateAll = BVDC_P_RUL_UPDATE_THRESHOLD;

				/* count down */
				hMad->ulGameModeTransCnt = 1;
				BDBG_MSG(("ulGameModeTransCnt = %d", hMad->ulGameModeTransCnt));
			}
			else
			{
				/* cycle through all 5-field stores with 3-field active at a time */
				feed_J =  ( feed_capture - 1 + 5 ) % 5;
				feed_D =  ( feed_capture - 2 + 5 ) % 5;
			}
		}
		else /* only 3-buffers allocated */
		{
			feed_capture  = ( feed_capture + 1 ) % 3;

			feed_J =  ( feed_capture - 1 + 3 ) % 3;
			feed_D =  ( feed_capture - 2 + 3 ) % 3;
		}
		break;

	default:
		/* should not reach here */
		BDBG_ASSERT(false);
		break;
	}

	/* 3) set field stores */
	if(hMad->usGameModeStartDelay)
	{
		/* NOTE: FORCE_SPATIAL is turned on during game mode transition; */
		BVDC_P_MAD_GET_REG_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0) =  (
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, FIELD_J_CHROMA_TYPE, CHROMA_422) |
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_4)
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, PDB_LOW_DELAY_MODE , LOW_DELAY) |
#endif /* phase >= 4 */
#endif /* phase >= 3 */
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, PDB_HARDSTART,          OFF) |
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, FORCE_SPATIAL,           ON) |
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, FORCE_FLUSH,            OFF) |
			BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, FIELD_CAPTURE, feed_capture) |
			(((BVDC_MadGameMode_e5Fields_0Delay == eGameMode) ||
			(BVDC_MadGameMode_e4Fields_0Delay == eGameMode) ||
			  (BVDC_MadGameMode_e4Fields_ForceSpatial == eGameMode) ||
			(BVDC_MadGameMode_e3Fields_0Delay == eGameMode) ||
			(BVDC_MadGameMode_e3Fields_ForceSpatial == eGameMode)) ?
			BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, FIELD_J_TYPE , !cur_field) :
		BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, FIELD_J_TYPE , cur_field)) |
			BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, FIELD_FEED_J , feed_J) |
			BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, FIELD_FEED_G , feed_G) |
			BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, FIELD_FEED_B , feed_B) |
			BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, FIELD_FEED_D , feed_D) |
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, FORCE_CONST_COLOR , OFF) );

		hMad->usGameModeStartDelay--;
		BDBG_MSG(("Mad %d start delay = %d", hMad->eId, hMad->usGameModeStartDelay));
	}
	else
	{
		BVDC_P_MAD_GET_REG_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0) =  (
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, FIELD_J_CHROMA_TYPE, CHROMA_422) |
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_4)
			((((BVDC_MadGameMode_e5Fields_0Delay == eGameMode) ||
			(BVDC_MadGameMode_e4Fields_0Delay == eGameMode) ||
		  (BVDC_MadGameMode_e4Fields_ForceSpatial == eGameMode)||
			(BVDC_MadGameMode_e3Fields_0Delay == eGameMode) ||
			(BVDC_MadGameMode_e3Fields_ForceSpatial == eGameMode)) &&
			(hMad->ulGameModeTransCnt == 0))?
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, PDB_LOW_DELAY_MODE , LOW_DELAY) :
		BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, PDB_LOW_DELAY_MODE , NORMAL_DELAY)) |
#endif /* phase >= 4 */
#endif /* phase >= 3 */
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, PDB_HARDSTART , OFF) |
			BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, FORCE_SPATIAL ,
			((BVDC_MadGameMode_e3Fields_ForceSpatial == eGameMode) ||
			 (BVDC_MadGameMode_e4Fields_ForceSpatial == eGameMode))) |
		BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, FORCE_FLUSH , OFF) |
		BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, FIELD_CAPTURE , feed_capture) |
		(((((BVDC_MadGameMode_e5Fields_0Delay == eGameMode) ||
		  (BVDC_MadGameMode_e4Fields_0Delay == eGameMode) ||
		  (BVDC_MadGameMode_e4Fields_ForceSpatial == eGameMode) ||
		  (BVDC_MadGameMode_e3Fields_0Delay == eGameMode) ||
			(BVDC_MadGameMode_e3Fields_ForceSpatial == eGameMode)) &&
			(hMad->ulGameModeTransCnt != 2)) ||
			(((BVDC_MadGameMode_e5Fields_1Delay == eGameMode) ||
			(BVDC_MadGameMode_e4Fields_1Delay == eGameMode) ||
			(BVDC_MadGameMode_e3Fields_1Delay == eGameMode)) &&
			(hMad->ulGameModeTransCnt == 1))) ?
			BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, FIELD_J_TYPE , !cur_field) :
		BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, FIELD_J_TYPE , cur_field)) |
			BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, FIELD_FEED_J , feed_J) |
			BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, FIELD_FEED_G , feed_G) |
			BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, FIELD_FEED_B , feed_B) |
			BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, FIELD_FEED_D , feed_D) |
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, FORCE_CONST_COLOR , OFF) );
	}

	if(hMad->ulGameModeTransCnt == 1)
	{
		/* 2-field-delay state uses feed J as reference picture for ANR */
#if BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE /* memory saving mode */
		BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_1) &=  ~(
			BCHP_MASK(MAD_0_MODE_CONTROL_1, MEM_SAVING_DATA_SEL));
		BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_1) |=  (
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_1, MEM_SAVING_DATA_SEL, USE_FIELD_J));
#endif
		hMad->ulGameModeTransCnt = 0;
		BDBG_MSG(("ulGameModeTransCnt = %d", hMad->ulGameModeTransCnt));
	}
#if BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE /* memory saving mode */
	else if(hMad->ulGameModeTransCnt == 2)
	{
		/* 1-field-delay state uses feed B as reference picture for ANR */
		BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_1) &=  ~(
			BCHP_MASK(MAD_0_MODE_CONTROL_1, MEM_SAVING_DATA_SEL));
		BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_1) |=  (
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_1, MEM_SAVING_DATA_SEL, USE_FIELD_B));
	}
#endif

	/* 4) calculate qm_capture: 0/2/4/6 for top field; 1/3/5/7 for bottom */
	qm_capture = cur_qm * 2 +  cur_field;

	/* 5) calculate qm store feed counters AND rotate fields store capture counter */
	switch(eGameMode)
	{
	case BVDC_MadGameMode_e5Fields_1Delay:
		qm_feed_K = ( qm_capture - 2 + 8) % 8;
		qm_feed_L = ( qm_capture - 4 + 8) % 8;
		qm_feed_M = ( qm_capture - 6 + 8) % 8;
		break;

	case BVDC_MadGameMode_e5Fields_0Delay:
		qm_feed_K = ( qm_capture - 2 + 8) % 8;
		qm_feed_L = ( qm_capture - 4 + 8) % 8;
		qm_feed_M = ( qm_capture - 6 + 8) % 8;
		break;

	case BVDC_MadGameMode_e4Fields_1Delay:
	case BVDC_MadGameMode_e4Fields_0Delay:
	case BVDC_MadGameMode_e4Fields_ForceSpatial:
		qm_feed_K = ( qm_capture - 2 + 8) % 8;
		qm_feed_L = ( qm_capture - 4 + 8) % 8;
		qm_feed_M = ( qm_capture - 6 + 8) % 8;
		break;

	case BVDC_MadGameMode_e3Fields_1Delay:
		qm_feed_K = ( qm_capture - 1 + 8) % 8;
		qm_feed_L = ( qm_capture - 2 + 8) % 8;
		qm_feed_M = ( qm_capture - 4 + 8) % 8;
		break;

	case BVDC_MadGameMode_e3Fields_0Delay:
	case BVDC_MadGameMode_e3Fields_ForceSpatial:
		qm_feed_K = ( qm_capture - 1 + 8) % 8;
		qm_feed_L = ( qm_capture - 2 + 8) % 8;
		qm_feed_M = ( qm_capture - 4 + 8) % 8;
		break;

	default:
		/* should not reach here */
		BDBG_ASSERT(false);
		break;
	}

	/* 6) set qm stores */
	if(hMad->usGameModeQmDelay)
	{
		BVDC_P_MAD_GET_REG_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1) =  (
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_8)
			/* Since MODE_CONTROL_0 already controls these feeders, just hard-code to 1 here */
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, PIXEL_FEED_G_ENABLE, ON) |
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, PIXEL_FEED_J_ENABLE, ON) |
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, PIXEL_FEED_B_ENABLE, ON) |
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, PIXEL_FEED_D_ENABLE, ON) |
#endif
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, QM_WRITE_ENABLE, ON     ) |
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, QM_FEED_M_ENABLE, OFF   ) |
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, QM_FEED_L_ENABLE, OFF   ) |
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, QM_FEED_K_ENABLE, OFF   ) |
			BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, QM_CAPTURE, qm_capture  ) |
			BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, QM_FEED_K, qm_feed_K    ) |
			BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, QM_FEED_L, qm_feed_L    ) |
			BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, QM_FEED_M, qm_feed_M    ));
		hMad->usGameModeQmDelay--;
		BDBG_MSG(("Mad %d Qm start delay = %d", hMad->eId, hMad->usGameModeQmDelay));
	}
	else
	{
		BVDC_P_MAD_GET_REG_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1) =  (
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_8)
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, PIXEL_FEED_G_ENABLE, ON) |
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, PIXEL_FEED_J_ENABLE, ON) |
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, PIXEL_FEED_B_ENABLE, ON) |
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, PIXEL_FEED_D_ENABLE, ON) |
#endif
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, QM_WRITE_ENABLE, ON    ) |
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, QM_FEED_M_ENABLE, ON   ) |
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, QM_FEED_L_ENABLE, ON   ) |
			BCHP_FIELD_ENUM(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, QM_FEED_K_ENABLE, ON   ) |
			BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, QM_CAPTURE , qm_capture) |
			BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, QM_FEED_K , qm_feed_K  ) |
			BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, QM_FEED_L , qm_feed_L  ) |
			BCHP_FIELD_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, QM_FEED_M , qm_feed_M  ));
	}

	/* 7) rotate cur_qm counter on every other bottom field */
	if(BAVC_Polarity_eBotField == cur_field)
	{
		cur_qm = (cur_qm + 1) % 4;
	}
	/* 8) save feedCapture and cur_qm for next vsync */
	hMad->usFeedCapture = feed_capture;
	hMad->usCurQm = cur_qm;
}

/***************************************************************************
* {private}
*
*/
BERR_Code BVDC_P_Mad_SetInfo_isr
( BVDC_P_Mad_Handle                hMad,
 BVDC_Window_Handle               hWindow,
 const BVDC_P_PictureNodePtr      pPicture )
{
	uint32_t ulSrcVSize;               /* pixel unit */
	uint32_t ulSrcHSize;               /* pixel unit */
#if (!BVDC_P_SUPPORT_HSCL) && (!BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED)
	uint32_t ulDstHSize;               /* pixel unit */
	uint32_t ulFirHrzStep;             /* hw register format: U4.17, U5.17, or U5.26 */
	uint32_t lHrzPhsAccInit;           /* hw register format: S1.6 or S5.26 */
	const BVDC_P_FirCoeffTbl *pHorzFirCoeff;
#endif
	bool bBufferRepeated = false;
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
	bool bMfdSrc = false;
	bool bCvbsSrc = false;
	bool bOddCrop = false;
	BVDC_Deinterlace_ChromaSettings   stChromaSettings;
#endif

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_4)
	bool bMad10BitMode = false;
#endif


	bool bHardStart = false;
	BVDC_MadGameMode eGameMode;
	BREG_Handle                hReg;

	const BVDC_P_Deinterlace_Settings *pMadCfg;
	const BFMT_VideoInfo *pFmtInfo;
	uint32_t ulHistogramBin0Reg = 0;
	uint32_t ulHistogramBin1Reg = 0;
	uint32_t ulHistogramBin2Reg = 0;
	uint32_t ulHistogramBin3Reg = 0;
	uint32_t ulHistogramBin4Reg = 0;

	BDBG_ENTER(BVDC_P_Mad_SetInfo_isr);
	BDBG_OBJECT_ASSERT(hMad, BVDC_MAD);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);

	pMadCfg   = &hWindow->stCurInfo.stMadSettings;
	pFmtInfo  = hWindow->stCurInfo.hSource->stCurInfo.pFmtInfo;

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
	/* Check source */
	if(BVDC_P_SRC_IS_MPEG(hWindow->stCurInfo.hSource->eId))
	{
		bMfdSrc = true;
	}
	else if(BVDC_P_SRC_IS_ANALOG(hWindow->stCurInfo.hSource->eId))
	{
		bCvbsSrc = (
			hWindow->stCurInfo.hSource->stCurInfo.bCvbsInput ||
			hWindow->stCurInfo.hSource->stCurInfo.bIfdInput);
	}
	/* normally 656in source comes from external VDEC cvbs input */
	else if(BVDC_P_SRC_IS_ITU656(hWindow->stCurInfo.hSource->eId))
	{
		bCvbsSrc = true;
	}

	/* (0.1) and source clipping
	BVN source cropping only occurs at Input(MFD), CAP and SCL;
	SCL is always after MAD; */
	if (hWindow->stVnetMode.stBits.bUseCap && !hWindow->stVnetMode.stBits.bSclBeforeCap)
	{
		bOddCrop = (pPicture->pSrcOut->lTop + pPicture->pCapOut->lTop) & BVDC_P_FIELD_PER_FRAME;
	}
	else
	{
		bOddCrop = pPicture->pSrcOut->lTop & BVDC_P_FIELD_PER_FRAME;
	}
#endif

	/* (1) the Mad source picture size is the feeder's scanout size */
	ulSrcVSize = (pPicture->pMadIn->ulHeight + 1) / BVDC_P_FIELD_PER_FRAME;
	ulSrcHSize = (pPicture->pMadIn->ulWidth & ~1);

#if (!BVDC_P_SUPPORT_HSCL) && (!BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED)
	/* no need to set dest vertical size */
	ulDstHSize = (pPicture->pMadOut->ulWidth & ~1);
#endif

	BDBG_ASSERT(BAVC_Polarity_eFrame!=pPicture->eSrcPolarity);
#if BVDC_P_SUPPORT_MAD_SRC_1080I
	BDBG_ASSERT(ulSrcVSize > 0);
#else
	if (pMadCfg->bShrinkWidth)
	{
		BDBG_ASSERT(ulSrcVSize > 0);
	}
	else
	{
		BDBG_ASSERT((ulSrcVSize > 0) && (ulSrcVSize <= BFMT_PAL_HEIGHT/2));
		BDBG_ASSERT(ulSrcHSize <= BFMT_PAL_WIDTH);
	}
#endif

	/* (2) Collect all source changes and user changes related to MAD */
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_7)
	/* (2.0) If ANR on/off toggling causes mem saving mode toggle, detect it here; */
	if(hMad->bMemSaving != hWindow->stVnetMode.stBits.bUseAnr)
	{
		/* mem saving mode? */
		hMad->bMemSaving = hWindow->stVnetMode.stBits.bUseAnr;
		hMad->ulUpdateAll = BVDC_P_RUL_UPDATE_THRESHOLD;
	}
#endif

	/* (2.1) MAD game mode change involved with buffer change, defer until buffer re-alloc; */
	if((hMad->eGameMode != pMadCfg->eGameMode) &&
		(hWindow->stSettings.bDeinterlacerAllocFull ||
		(hWindow->usMadPixelBufferCnt == s_aMadGameModeInfo[pMadCfg->eGameMode].usPixelBufferCnt)))
	{
		BDBG_ASSERT(s_aMadGameModeInfo[hMad->eGameMode].eMode == hMad->eGameMode);
		BDBG_MSG(("Mad%d game mode changed %s->%s", hMad->eId,
			s_aMadGameModeInfo[hMad->eGameMode].pchModeName,
			s_aMadGameModeInfo[pMadCfg->eGameMode].pchModeName));

        bHardStart = true;

#if BVDC_P_MAD_TEST_FEATURE_FORMAT_CHANGE_WORKAROUND
		if(hMad->stTestFeature1.bEnable)
		{
			hMad->bInitial = true;
		}
#endif

		/* game mode -> normal mode transition can be handled jitterless */
		if(pMadCfg->eGameMode == BVDC_MadGameMode_eOff &&
		   hMad->eGameMode != BVDC_MadGameMode_eHardStart &&
			(hWindow->stSettings.bDeinterlacerAllocFull ||
			(s_aMadGameModeInfo[hMad->eGameMode].usPixelBufferCnt ==
			s_aMadGameModeInfo[pMadCfg->eGameMode].usPixelBufferCnt)))
		{
			if(hMad->ulGameModeTransCnt==0)
			{
				/* calculate the smooth transition state machine counter */
				hMad->ulGameModeTransCnt = s_aMadGameModeInfo[pMadCfg->eGameMode].usDelay -
					s_aMadGameModeInfo[hMad->eGameMode].usDelay;
			}
		}
		else
		{
			/* clear the counter; */
			hMad->ulGameModeTransCnt = 0;
			hMad->eGameMode = pMadCfg->eGameMode;
			hMad->ulUpdateAll = BVDC_P_RUL_UPDATE_THRESHOLD;

			/* store the current field stores and QM stores pointers for smooth transition */
			hMad->usFeedCapture = BVDC_P_GET_FIELD(
				BREG_Read32_isr(hMad->hRegister, BCHP_MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0),
				MAD_0_DEBUG_CURRENT_FIELD_CONTROL_0, FIELD_CAPTURE);
			hMad->usCurQm       = BVDC_P_GET_FIELD(
				BREG_Read32_isr(hMad->hRegister, BCHP_MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1),
				MAD_0_DEBUG_CURRENT_FIELD_CONTROL_1, QM_CAPTURE);
			BDBG_MSG(("current field_cap = %d, qm_cap = %d", hMad->usFeedCapture, hMad->usCurQm));
		}
	}
	eGameMode = hMad->eGameMode;

	/* (2.2) size change or on/off toggle.   Detecting dynamics format (size)
	* change.  Base on these information we can further bypass
	* unnecessary computations. */
	if(!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_SRC_PIC_SIZE, HSIZE, ulSrcHSize)  ||
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_SRC_PIC_SIZE, VSIZE, ulSrcVSize)  ||
#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR)
		(hMad->stTestFeature1.ulBitsPerPixel != pMadCfg->stTestFeature1.ulBitsPerPixel) ||
#endif
#if (!BVDC_P_SUPPORT_HSCL) && (!BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED)
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_DEST_PIC_SIZE, HSIZE, ulDstHSize) ||
#endif
		!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_ENABLE_CONTROL, ENABLE, 1)
#if (!BVDC_P_SUPPORT_HSCL) && (!BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED)
		|| (hMad->lMadCutLeft != pPicture->lHsclCutLeft)
#endif
		)
	{
		/* this transition requires hard start */
		bHardStart = true;

#if BVDC_P_MAD_TEST_FEATURE_FORMAT_CHANGE_WORKAROUND
		if(hMad->stTestFeature1.bEnable)
		{
			hMad->bInitial = true;
		}
#endif

		/* when size changed in game mode, use force_spatial for transition
		and delay the QM feed as well as the resume of the current game mode; */
		/* delay game mode resume by 6 vsyncs, worst case for 5-field 0-delay;
		another 6 vsyncs delay for QM feeds, K/L/M with each for a pair of
		top/bottom (3 x 2 = 6); */
		if((BVDC_MadGameMode_eOff != eGameMode) &&
			(BVDC_MadGameMode_eHardStart != eGameMode))
		{
			hMad->usGameModeStartDelay  = BVDC_P_MAD_GAME_MODE_START_DELAY;
			hMad->usGameModeQmDelay     = BVDC_P_MAD_GAME_MODE_START_DELAY + 6;
		}
		hMad->usTrickModeStartDelay = BVDC_P_MAD_TRICK_MODE_START_DELAY;

		/* Always re-enable after set info. */
		BVDC_P_MAD_GET_REG_DATA(MAD_0_ENABLE_CONTROL) =
			BCHP_FIELD_ENUM(MAD_0_ENABLE_CONTROL, MODE, STOP_ON_FIELD_COMPLETION) |
			BCHP_FIELD_ENUM(MAD_0_ENABLE_CONTROL, ENABLE, ON);

		/* 1080i turns off bad weave. */
		BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_4) &=  ~(
			BCHP_MASK(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_4, BAD_WEAVE_DETECT_ENABLE ));
		BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_4) |=  (
			(!VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt)
			? BCHP_FIELD_ENUM(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_4, BAD_WEAVE_DETECT_ENABLE, OFF)
			: BCHP_FIELD_ENUM(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_4, BAD_WEAVE_DETECT_ENABLE, ON)));

		/* PR45231: oversample NTSC input adjusts phase_calc_7 */
		if(hWindow->stCurInfo.hSource->ulSampleFactor && VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt))
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_7) =  (
				BCHP_FIELD_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_7, BAD_WEAVE_UNLOCK_THRESH,     0x7800) |
				BCHP_FIELD_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_7, BAD_WEAVE_UNLOCK_PH4_THRESH, 0x7fff));
		}
		else
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_7) =  (
				BCHP_FIELD_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_7, BAD_WEAVE_UNLOCK_THRESH,     0x3fff) |
				BCHP_FIELD_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_7, BAD_WEAVE_UNLOCK_PH4_THRESH, 0x3fff));
		}

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
		BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_4) &= ~(
			BCHP_MASK(MAD_0_BWV_CONTROL_4, TKR_CORING_THRESH));
		BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_4) |= (
			VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt)
			? BCHP_FIELD_DATA(MAD_0_BWV_CONTROL_4, TKR_CORING_THRESH, 0x62)
			: BCHP_FIELD_DATA(MAD_0_BWV_CONTROL_4, TKR_CORING_THRESH, 0x68));
#endif
		hMad->ulUpdateAll = BVDC_P_RUL_UPDATE_THRESHOLD;
		hMad->lMadCutLeft = pPicture->lHsclCutLeft;
	}

#if (BVDC_P_SUPPORT_VCR_MODE)
	/* (2.3) Put MAD into VCR mode, disable 3:2 pulldown? */
	if(((pPicture->stFlags.bVcrHeadSwitch)&&
		BVDC_P_MAD_COMPARE_FIELD_NAME(MAD_0_IT_OUTPUT_CONTROL, LOOPS_RESET, NEITHER)) ||
		((!pPicture->stFlags.bVcrHeadSwitch)&&
		BVDC_P_MAD_COMPARE_FIELD_NAME(MAD_0_IT_OUTPUT_CONTROL, LOOPS_RESET, BOTH)))
	{
		hMad->ulUpdateAll = BVDC_P_RUL_UPDATE_THRESHOLD;
	}
#endif

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_2)
	/* (2.4) MAD OSD feature */
	if(!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_MODE_CONTROL_0, ON_SCREEN_STATUS_ENABLE, hMad->bEnableOsd))
	{
		hMad->ulUpdateAll = BVDC_P_RUL_UPDATE_THRESHOLD;
	}
#endif

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR) /* PR56938: software workaround */
	if(bHardStart)
	{
		hMad->ulHardStartCountdown = BVDC_P_MAD_HARDSTART_COUNTDOWN;
	}
#endif

	/* (3) Update all registers for significant changes */
	if(hMad->ulUpdateAll)
	{
		uint32_t ulChroma32Threshold = BVDC_P_OVER_SAMPLE(hWindow->stCurInfo.hSource->ulSampleFactor, 7700);

		/* PR45231: oversampled NTSC input */
		if(hWindow->stCurInfo.hSource->ulSampleFactor && VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt))
		{
			ulChroma32Threshold = 0x5000;
		}

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
		/* PR55795: 1080i hdmi input tuned differently */
		if((BFMT_VideoFmt_e1080i == pFmtInfo->eVideoFmt) &&
			BVDC_P_SRC_IS_HDDVI(hWindow->stCurInfo.hSource->eId))
		{
			ulChroma32Threshold = 0x3800;

			BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_0) &= ~(
				BCHP_MASK(MAD_0_BWV_CONTROL_0, LUMA_CORING_THRESH));
			BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_0) |= (
				BCHP_FIELD_DATA(MAD_0_BWV_CONTROL_0, LUMA_CORING_THRESH, 8));

			BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_2) &= ~(
				BCHP_MASK(MAD_0_BWV_CONTROL_2, CHROMA_CORING_THRESH));
			BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_2) |= (
				BCHP_FIELD_DATA(MAD_0_BWV_CONTROL_2, CHROMA_CORING_THRESH, 8));
		}
		else
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_0) &= ~(
				BCHP_MASK(MAD_0_BWV_CONTROL_0, LUMA_CORING_THRESH));
			BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_0) |= (
				BCHP_FIELD_DATA(MAD_0_BWV_CONTROL_0, LUMA_CORING_THRESH, 4));

			BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_2) &= ~(
				BCHP_MASK(MAD_0_BWV_CONTROL_2, CHROMA_CORING_THRESH));
			BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_2) |= (
				BCHP_FIELD_DATA(MAD_0_BWV_CONTROL_2, CHROMA_CORING_THRESH, 4));
		}
#endif

		/* (3.1) basic info: mode, polarity;  */
		BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_1) =  (
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_1, CHROMA_MODE_SEL, pPicture->eChromaType) |
#endif
#if BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE /* memory saving mode */
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_1, MEM_SAVING_DATA_SEL,
			(0 == s_aMadGameModeInfo[pMadCfg->eGameMode].usDelay) ?
			1 :  /* USE_FIELD_D */
		0)/* USE_FIELD_B by default */                       |
#endif
			((bHardStart || (BVDC_MadGameMode_eHardStart == eGameMode)) ?
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_1, MODE_SEL, HARD_START):
		BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_1, MODE_SEL, NORMAL)) |
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_1, FIELD_A_TYPE,
			pPicture->PicComRulInfo.eSrcOrigPolarity));

		/* (3.2) Set src size. */
		BVDC_P_MAD_GET_REG_DATA(MAD_0_SRC_PIC_SIZE) =  (
			BCHP_FIELD_DATA(MAD_0_SRC_PIC_SIZE, HSIZE, ulSrcHSize) |
			BCHP_FIELD_DATA(MAD_0_SRC_PIC_SIZE, VSIZE, ulSrcVSize));

#if (!BVDC_P_SUPPORT_HSCL) && (!BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED)
		/* (3.3) dst size. */
		BVDC_P_MAD_GET_REG_DATA(MAD_0_DEST_PIC_SIZE) =  (
			BCHP_FIELD_DATA(MAD_0_DEST_PIC_SIZE, HSIZE, ulDstHSize));
#endif

		/* (3.4) cross chroma mode */
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_2)
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
		BVDC_P_Mad_Init_Chroma_DynamicDefault(pFmtInfo, eGameMode,
			bMfdSrc, bCvbsSrc, &stChromaSettings);

		BVDC_P_MAD_GET_REG_DATA(MAD_0_XCHROMA_MODE) &= ~(
			BCHP_MASK(MAD_0_XCHROMA_MODE, PIC_422_MA) |
			BCHP_MASK(MAD_0_XCHROMA_MODE, PIC_422_IT) |
			BCHP_MASK(MAD_0_XCHROMA_MODE, FIELD_420_MA) |
			BCHP_MASK(MAD_0_XCHROMA_MODE, FIELD_420_IT) |
			BCHP_MASK(MAD_0_XCHROMA_MODE, FRAME_420_MA) |
			BCHP_MASK(MAD_0_XCHROMA_MODE, FRAME_420_IT));

		BVDC_P_MAD_GET_REG_DATA(MAD_0_XCHROMA_MODE) |= (
			BCHP_FIELD_DATA(MAD_0_XCHROMA_MODE, PIC_422_MA, stChromaSettings.eChroma422MotionAdaptiveMode)   |
			BCHP_FIELD_DATA(MAD_0_XCHROMA_MODE, PIC_422_IT, stChromaSettings.eChroma422InverseTelecineMode)   |
			BCHP_FIELD_DATA(MAD_0_XCHROMA_MODE, FIELD_420_MA, stChromaSettings.eChroma420MotionAdaptiveMode));

		if((eGameMode == BVDC_MadGameMode_e3Fields_1Delay)       ||
			(eGameMode == BVDC_MadGameMode_e3Fields_0Delay)       ||
			(eGameMode == BVDC_MadGameMode_e3Fields_ForceSpatial))
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_XCHROMA_MODE) |= (
				BCHP_FIELD_ENUM(MAD_0_XCHROMA_MODE, FIELD_420_IT, MA_CHROMA) |
				BCHP_FIELD_ENUM(MAD_0_XCHROMA_MODE, FRAME_420_MA, MA_CHROMA) |
				BCHP_FIELD_ENUM(MAD_0_XCHROMA_MODE, FRAME_420_IT, MA_CHROMA));
		}
		else if(bMfdSrc)
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_XCHROMA_MODE) |= (
				BCHP_FIELD_ENUM(MAD_0_XCHROMA_MODE, FIELD_420_IT, INT_420)   |
				BCHP_FIELD_ENUM(MAD_0_XCHROMA_MODE, FRAME_420_MA, MA_CHROMA) |
				BCHP_FIELD_ENUM(MAD_0_XCHROMA_MODE, FRAME_420_IT, IT_DIRECT));
		}
		else if(pPicture->stFlags.bVcrHeadSwitch)
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_XCHROMA_MODE) |= (
				BCHP_FIELD_ENUM(MAD_0_XCHROMA_MODE, FIELD_420_IT, INT_420)   |
				BCHP_FIELD_ENUM(MAD_0_XCHROMA_MODE, FRAME_420_MA, MA_CHROMA) |
				BCHP_FIELD_ENUM(MAD_0_XCHROMA_MODE, FRAME_420_IT, MA_CHROMA));
		}
		else if(bCvbsSrc)
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_XCHROMA_MODE) |= (
				BCHP_FIELD_ENUM(MAD_0_XCHROMA_MODE, FIELD_420_IT, INT_420)   |
				BCHP_FIELD_ENUM(MAD_0_XCHROMA_MODE, FRAME_420_MA, MA_CHROMA) |
				BCHP_FIELD_ENUM(MAD_0_XCHROMA_MODE, FRAME_420_IT, IT_XCHROMA));
		}
		else/* if(bComponentSrc || bSvideo || bHddvi) */
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_XCHROMA_MODE) |= (
				BCHP_FIELD_ENUM(MAD_0_XCHROMA_MODE, FIELD_420_IT, INT_420)   |
				BCHP_FIELD_ENUM(MAD_0_XCHROMA_MODE, FRAME_420_MA, MA_CHROMA) |
				BCHP_FIELD_ENUM(MAD_0_XCHROMA_MODE, FRAME_420_IT, IT_DIRECT));
		}
#endif	/* phase >= 3 */

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_5)
		/* (3.5) mode control 2 */
		if(eGameMode == BVDC_MadGameMode_e3Fields_1Delay)
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) =  (
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, ABS_DIFF_SRC_SEL, USE_PIXEL_B_AND_G ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_A_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_D_SRC_SEL,  USE_FIELD_J ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_B_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL,  USE_FIELD_A ));
		}
		else if((eGameMode == BVDC_MadGameMode_e3Fields_0Delay) ||
			(eGameMode == BVDC_MadGameMode_e3Fields_ForceSpatial))
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) =  (
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, ABS_DIFF_SRC_SEL, USE_ORIGINAL_PIXEL ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_A_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_D_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_B_SRC_SEL,  USE_FIELD_A ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL,  USE_FIELD_A ));
		}
		else if(eGameMode == BVDC_MadGameMode_e5Fields_1Delay)
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) =  (
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, ABS_DIFF_SRC_SEL, USE_ORIGINAL_PIXEL ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_A_SRC_SEL,  USE_FIELD_G ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_D_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_B_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL,  USE_FIELD_A ));
		}
		else
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) =  (
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, ABS_DIFF_SRC_SEL, USE_ORIGINAL_PIXEL ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_A_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_D_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_B_SRC_SEL,  USE_ORIGINAL_PIXEL ) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL,  USE_ORIGINAL_PIXEL ));
		}

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_6)
		BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) |=  (
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_2, PCC_BWV_ENABLE,
			(eGameMode == BVDC_MadGameMode_eOff) &&
			!VIDEO_FORMAT_IS_SECAM(pFmtInfo->eVideoFmt))); /* PR45224 */
#endif /* phase 6 */
#endif /* phase >= 5 */

		/* (3.6) mode control 0 */
		/* enable all field stores of MAD block and 5 field buffers */
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_6)
		bMad10BitMode = (
			BPXL_IS_YCbCr422_10BIT_FORMAT(pMadCfg->ePixelFmt) ||
			BPXL_IS_YCbCr422_10BIT_PACKED_FORMAT(pMadCfg->ePixelFmt));
#elif (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_4)
		bMad10BitMode =
			BPXL_IS_YCbCr422_10BIT_FORMAT(pMadCfg->ePixelFmt);
#endif

		/* MAD_0_MODE_CONTROL_0 */
		BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) =  (
#ifdef BVDC_MAD_NOT_SUPPORT_TRICK_MODE
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, BUFFER_SEL, 1) |/* 5 fields store */
#else
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, BUFFER_SEL, 1) |/* 5 fields store */
#endif
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, IT_PPRFM_CR_ENABLE,             ON ) |
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, IT_PPUFM_CR_ENABLE,             ON ) |
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, CHROMA_422_MA_EDGE_DET_MODE,    OFF) |
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, CHROMA_FIELD_420_EDGE_DET_MODE, stChromaSettings.bChromaField420EdgeDetMode) |
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, ALT_REPF_VETO_LEVEL_ENABLE,     ON ) |
			/* NOTE: mfd source has known type of chroma; */
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, CHROMA_FIELD_420_INV_METHOD,   stChromaSettings.eChromaField420InvMethod) |
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_4)
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, REV32_BAD_EDIT_PHASE_VETO, ON) |
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, UPDATE_SEL, ALWAYS_UPDATE    ) |
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, ALT_ENTER_LOCK_ENABLE, OFF   ) |
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, SCB_MODE_SEL,   bMad10BitMode) |
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, CHROMA_422_MOTION_MODE, stChromaSettings.eChroma422MotionMode) |
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, CHROMA_FIELD_420_MOTION_MODE, stChromaSettings.eChroma420MotionMode) |

#endif /* phase >= 4 */
#endif /* phase >= 3 */
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, ON_SCREEN_STATUS_ENABLE, hMad->bEnableOsd) |
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, TM_MODE_SEL, BOOST2) |
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, SM_MODE_SEL, BOOST1) |
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_J_ENABLE, ON)  |
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, FIELD_B_ENABLE,
			(eGameMode != BVDC_MadGameMode_e3Fields_0Delay) &&
			(eGameMode != BVDC_MadGameMode_e3Fields_ForceSpatial)) |
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, FIELD_D_ENABLE,
			(eGameMode != BVDC_MadGameMode_e3Fields_1Delay))       |
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, FIELD_G_ENABLE,
			(s_aMadGameModeInfo[eGameMode].usPixelBufferCnt == 5)) |
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, PIXEL_CAP_ENABLE,ON) |
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_K_ENABLE, ON) |
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_L_ENABLE, ON) |
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_M_ENABLE, ON)
			);

		/* (3.7) mode control 0: hw or sw to control field store state machine */
		if((BVDC_MadGameMode_eOff != eGameMode) &&
			(BVDC_MadGameMode_eHardStart != eGameMode))
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) &= ~(
				BCHP_MASK(MAD_0_MODE_CONTROL_0, FIELD_STATE_UPDATE_SEL_0) |
				BCHP_MASK(MAD_0_MODE_CONTROL_0, FIELD_STATE_UPDATE_SEL_1));

			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_STATE_UPDATE_SEL_0, CPU_OR_RDMA) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_STATE_UPDATE_SEL_1, CPU_OR_RDMA) );

#if BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE && BVDC_P_MAD_SUPPORT_10_8_BIT_TRANSITION
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) &= ~(
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, STORAGE_TYPE_UPDATE_SEL));
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, STORAGE_TYPE_UPDATE_SEL, CPU_OR_RDMA));
#endif
		}
		else/* restore h/w internal control of field state */
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) &= ~ (
				BCHP_MASK(MAD_0_MODE_CONTROL_0, FIELD_STATE_UPDATE_SEL_0) |
				BCHP_MASK(MAD_0_MODE_CONTROL_0, FIELD_STATE_UPDATE_SEL_1));

			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_STATE_UPDATE_SEL_0, FIELD_STATE_FIFO) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_STATE_UPDATE_SEL_1, IT_BLOCK) );

#if BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE && BVDC_P_MAD_SUPPORT_10_8_BIT_TRANSITION
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) &= ~(
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, STORAGE_TYPE_UPDATE_SEL));
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, STORAGE_TYPE_UPDATE_SEL, STORAGE_TYPE_FIFO));
#endif
		}

		/* (3.8) OSD */
		BVDC_P_MAD_GET_REG_DATA(MAD_0_OSD_POSITION) =
			BCHP_FIELD_DATA(MAD_0_OSD_POSITION, HPOS, hMad->ulOsdHpos) |
			BCHP_FIELD_DATA(MAD_0_OSD_POSITION, VPOS, hMad->ulOsdVpos);

		/* (3.9) MAD_0_IT_OUTPUT_CONTROL */
		BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_OUTPUT_CONTROL) &= ~(
			BCHP_MASK(MAD_0_IT_OUTPUT_CONTROL, LOOPS_RESET) |
			BCHP_MASK(MAD_0_IT_OUTPUT_CONTROL, PPBWV_MODE) |
			/* clear lock status */
			BCHP_MASK(MAD_0_IT_OUTPUT_CONTROL, REV22_LOCKED) |
			/* clear auto rev22 */
			BCHP_MASK(MAD_0_IT_OUTPUT_CONTROL, AUTOREV22_ENABLE) |
			BCHP_MASK(MAD_0_IT_OUTPUT_CONTROL, PPUFM_MODE) |
			BCHP_MASK(MAD_0_IT_OUTPUT_CONTROL, PPRFM_MODE) |
			/* clear lock status */
			BCHP_MASK(MAD_0_IT_OUTPUT_CONTROL, REV32_LOCKED) |
			BCHP_MASK(MAD_0_IT_OUTPUT_CONTROL, AUTOREV32_ENABLE) );
		BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_OUTPUT_CONTROL) |=  (
			BCHP_FIELD_ENUM(MAD_0_IT_OUTPUT_CONTROL, PPBWV_MODE,  OFF) |
			BCHP_FIELD_ENUM(MAD_0_IT_OUTPUT_CONTROL, PPUFM_MODE, AUTO) |
			BCHP_FIELD_ENUM(MAD_0_IT_OUTPUT_CONTROL, PPRFM_MODE, AUTO) |
			/* VCR head switch mode? */
			((pPicture->stFlags.bVcrHeadSwitch)
			? BCHP_FIELD_ENUM(MAD_0_IT_OUTPUT_CONTROL, LOOPS_RESET, BOTH)
			: BCHP_FIELD_ENUM(MAD_0_IT_OUTPUT_CONTROL, LOOPS_RESET, NEITHER)) |
			/* set rev32 and rev22 */
			(((BVDC_MadGameMode_e3Fields_1Delay != eGameMode) &&
			  (BVDC_MadGameMode_e5Fields_1Delay != eGameMode) &&
			  (BVDC_MadGameMode_e4Fields_1Delay != eGameMode))
			? BCHP_FIELD_DATA(MAD_0_IT_OUTPUT_CONTROL, AUTOREV32_ENABLE, pMadCfg->bReverse32Pulldown)
			: BCHP_FIELD_DATA(MAD_0_IT_OUTPUT_CONTROL, AUTOREV32_ENABLE, 0)) |
			BCHP_FIELD_ENUM(MAD_0_IT_OUTPUT_CONTROL, AUTOREV22_ENABLE, OFF));
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
		/* (3.10) Oversample? */
		BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_3) &= ~(
			BCHP_MASK(MAD_0_BWV_CONTROL_3, CHROMA_32_THRESH));
		BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_3) |= (
			BCHP_FIELD_DATA(MAD_0_BWV_CONTROL_3, CHROMA_32_THRESH, ulChroma32Threshold));

		BVDC_P_MAD_GET_REG_DATA(MAD_0_STATS_RANGE) &= ~(BCHP_MASK(MAD_0_STATS_RANGE, END_LINE));
		BVDC_P_MAD_GET_REG_DATA(MAD_0_STATS_RANGE) |=  (
			VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt) ?
			BCHP_FIELD_DATA(MAD_0_STATS_RANGE, END_LINE,  0xf0) :
		BCHP_FIELD_DATA(MAD_0_STATS_RANGE, END_LINE,  0x21c));
#endif /* phase >= 3 */
#endif /* phase >= 2 */
#if (BVDC_P_SUPPORT_MAD_VER < BVDC_P_MAD_VER_3)
		BSTD_UNUSED(ulChroma32Threshold);
#endif

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_7)
		/* (3.11) Motion Calculation Control. */
		BVDC_P_MAD_GET_REG_DATA(MAD_0_MOTION_CAL_CTRL) &= ~(
			BCHP_MASK(MAD_0_MOTION_CAL_CTRL, MS_3548) |
			BCHP_MASK(MAD_0_MOTION_CAL_CTRL, MT_3548));

		BVDC_P_MAD_GET_REG_DATA(MAD_0_MOTION_CAL_CTRL) |=
			BCHP_FIELD_DATA(MAD_0_MOTION_CAL_CTRL, MS_3548, stChromaSettings.bMS_3548) |
			BCHP_FIELD_DATA(MAD_0_MOTION_CAL_CTRL, MT_3548, stChromaSettings.bMT_3548);
#endif
	}
	/* (4) VSYNC control */
	else
	{
		/* normal vsync RUL only needs to re-enable and set field polarity */
		BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_1) &= ~(
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
			BCHP_MASK(MAD_0_MODE_CONTROL_1, CHROMA_MODE_SEL) |
#endif
			BCHP_MASK(MAD_0_MODE_CONTROL_1, MODE_SEL) |
			BCHP_MASK(MAD_0_MODE_CONTROL_1, FIELD_A_TYPE));

		BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_1) |=  (
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_1, CHROMA_MODE_SEL, pPicture->eChromaType) |
#endif
			((BVDC_MadGameMode_eHardStart == eGameMode)
			? BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_1, MODE_SEL, HARD_START)
			: BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_1, MODE_SEL, NORMAL)) |
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_1, FIELD_A_TYPE, pPicture->eSrcPolarity));

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_2)
		/* customer might not want trick mode to save memory */
#ifndef BVDC_MAD_NOT_SUPPORT_TRICK_MODE
		/* Note: don't enable trick mode in case mpeg picture is muted and
		repeated with the same polarity from beginning
		that MAD is first turned on, to prevent green screen; */
		if(pPicture->stFlags.bPictureRepeatFlag && (!pPicture->stFlags.bMuteMad) && (!hMad->usTrickModeStartDelay))
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_1) &= ~(
				BCHP_MASK(MAD_0_MODE_CONTROL_1, TRICK_MODE_SEL));
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_1) |=  (
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_1, TRICK_MODE_SEL, FIELD_FREEZE));
			/* clear flag after consume it; */
			pPicture->stFlags.bPictureRepeatFlag = false;
			bBufferRepeated = true;
			BDBG_MSG(("Repeat field %d at buffer %d", pPicture->eSrcPolarity, pPicture->ulBufferId));
		}
		else
#endif /* not asupport trick mode */
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_1) &= ~(
				BCHP_MASK(MAD_0_MODE_CONTROL_1, TRICK_MODE_SEL));
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_1) |=  (
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_1, TRICK_MODE_SEL, OFF));
			if(hMad->usTrickModeStartDelay)
				hMad->usTrickModeStartDelay--;
		}
#endif /* phase >= 2 */
	}

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR) /* PR56938: software workaround */
	BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) &=  ~(
		BCHP_MASK(MAD_0_MODE_CONTROL_0, FIELD_J_ENABLE) |
		BCHP_MASK(MAD_0_MODE_CONTROL_0, FIELD_B_ENABLE) |
		BCHP_MASK(MAD_0_MODE_CONTROL_0, FIELD_D_ENABLE) |
		BCHP_MASK(MAD_0_MODE_CONTROL_0, FIELD_G_ENABLE));
	if((BVDC_MadGameMode_eHardStart == eGameMode) ||
		(BVDC_P_MAD_HARDSTART_COUNTDOWN == hMad->ulHardStartCountdown))
	{
		BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_J_ENABLE, OFF) |
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_B_ENABLE, OFF) |
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_D_ENABLE, OFF) |
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_G_ENABLE, OFF));

		BDBG_MSG(("MAD hs cnt = %d", hMad->ulHardStartCountdown));
	}
	else if((BVDC_P_MAD_HARDSTART_COUNTDOWN - 1) == hMad->ulHardStartCountdown)
	{
		BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_J_ENABLE,  ON) |
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_B_ENABLE, OFF) |
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_D_ENABLE, OFF) |
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_G_ENABLE, OFF));
		BDBG_MSG(("MAD hs cnt = %d", hMad->ulHardStartCountdown));
	}
	else if((BVDC_P_MAD_HARDSTART_COUNTDOWN - 2) == hMad->ulHardStartCountdown)
	{
		BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_J_ENABLE,  ON) |
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, FIELD_B_ENABLE,
			(eGameMode == BVDC_MadGameMode_eOff)            ||
			(eGameMode == BVDC_MadGameMode_e5Fields_1Delay) ||
				(eGameMode == BVDC_MadGameMode_e4Fields_1Delay) ||
			(eGameMode == BVDC_MadGameMode_e3Fields_1Delay))         |
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, FIELD_D_ENABLE,
			(eGameMode == BVDC_MadGameMode_e5Fields_0Delay) ||
			(eGameMode == BVDC_MadGameMode_e4Fields_0Delay) ||
			(eGameMode == BVDC_MadGameMode_e3Fields_0Delay))         |
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_G_ENABLE, OFF));
		BDBG_MSG(("MAD hs cnt = %d", hMad->ulHardStartCountdown));
	}
	else if((BVDC_P_MAD_HARDSTART_COUNTDOWN - 3) == hMad->ulHardStartCountdown)
	{
		BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_J_ENABLE,  ON) |
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, FIELD_B_ENABLE,
			(eGameMode != BVDC_MadGameMode_e3Fields_0Delay) &&
			(eGameMode != BVDC_MadGameMode_e3Fields_ForceSpatial))   |
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, FIELD_D_ENABLE,
			(eGameMode != BVDC_MadGameMode_e3Fields_1Delay))         |
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_G_ENABLE, OFF));
		BDBG_MSG(("MAD hs cnt = %d", hMad->ulHardStartCountdown));
	}
	else
	{
		BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, FIELD_J_ENABLE, ON)  |
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, FIELD_B_ENABLE,
			(eGameMode != BVDC_MadGameMode_e3Fields_0Delay) &&
			(eGameMode != BVDC_MadGameMode_e3Fields_ForceSpatial))   |
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, FIELD_D_ENABLE,
			(eGameMode != BVDC_MadGameMode_e3Fields_1Delay))         |
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, FIELD_G_ENABLE,
			(s_aMadGameModeInfo[eGameMode].usPixelBufferCnt == 5)));
	}
#endif

	/* (5) settings about dynamic changes */
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
	/* (5.1) init phase */
	if(!hWindow->stCurInfo.bChromaCustom)
	{
		BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) &=  ~(
			BCHP_MASK(MAD_0_MODE_CONTROL_0, CHROMA_FIELD_420_INITIAL_PHASE ));
		BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
			BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, CHROMA_FIELD_420_INITIAL_PHASE, bOddCrop ));
	}
#endif

	/* TODO: verify this is true for 2:2 software alg; */
	/* (5.2) clear 3:2/2:2 software weave decision; let hw do it by default; */
	BVDC_P_MAD_GET_REG_DATA(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2) &= ~(
		BCHP_MASK(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2, IT_DEBUG_MODE_FORCE) |
		BCHP_MASK(MAD_0_DEBUG_CURRENT_FIELD_CONTROL_2, IT_MODE_SEL));

	/* (6) if user changes MAD custom settings, adjust it here */
	if(hMad->bUsrChanges || hMad->ulUpdateAll)
	{
		BDBG_MSG(("User changed[%d] Deinterlace %d's settings.", hMad->bUsrChanges,hMad->eId));
		BVDC_P_Mad_UserConfig_isr(hMad, hWindow, pFmtInfo);
		hMad->bUsrChanges = false;
	}

	/* (7) Temporarily disable MAD 2:2 pulldown if source is 1080i */
	if(pFmtInfo->ulDigitalHeight > BFMT_PAL_HEIGHT)
	{
		hMad->bReverse22Pulldown = false;
		hMad->ulUpdate22PullDown = BVDC_P_RUL_UPDATE_THRESHOLD;
		hMad->alPhase22[0] = 0;
		hMad->alPhase22[1] = 0;

		BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_OUTPUT_CONTROL) &= ~(
			BCHP_MASK(MAD_0_IT_OUTPUT_CONTROL, REV22_LOCKED) |
			BCHP_MASK(MAD_0_IT_OUTPUT_CONTROL, REV22_FIELD_PHASE));
	}

	/* (8) Game Mode */
	hReg = hMad->hRegister;
	ulHistogramBin0Reg = BREG_Read32_isr(hReg, BCHP_MAD_0_IT_HISTOGRAM_BIN_0 + hMad->ulRegOffset);
	ulHistogramBin1Reg = BREG_Read32_isr(hReg, BCHP_MAD_0_IT_HISTOGRAM_BIN_1 + hMad->ulRegOffset);
	ulHistogramBin2Reg = BREG_Read32_isr(hReg, BCHP_MAD_0_IT_HISTOGRAM_BIN_2 + hMad->ulRegOffset);
	ulHistogramBin3Reg = BREG_Read32_isr(hReg, BCHP_MAD_0_IT_HISTOGRAM_BIN_3 + hMad->ulRegOffset);
	ulHistogramBin4Reg = BREG_Read32_isr(hReg, BCHP_MAD_0_IT_HISTOGRAM_BIN_4 + hMad->ulRegOffset);

	hMad->ulRepfMotion = ulHistogramBin1Reg +
		(ulHistogramBin2Reg << 1) +
		(ulHistogramBin3Reg << 2) +
		(ulHistogramBin4Reg << 3);

	hMad->aulSigma = ulHistogramBin0Reg +
		(ulHistogramBin1Reg << 1) +
		(ulHistogramBin2Reg << 2) +
		((ulHistogramBin3Reg + ulHistogramBin4Reg) << 3);
	if((BVDC_MadGameMode_eOff != eGameMode) &&
		(BVDC_MadGameMode_eHardStart != eGameMode) &&
		(!bBufferRepeated))
	{
		/* MAD game mode software algorithm */
		BVDC_P_Mad_GameMode_isr(hMad, hWindow, pPicture);

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_5)
		/* Compute 3:2 pulldown for 3-field 1-delay low-bandwidth game mode; */
		/* This needs to be done as late as possible so that the histogram data */
		/* is ready for reading and computing */
		if(((eGameMode == BVDC_MadGameMode_e5Fields_1Delay) ||
			(eGameMode == BVDC_MadGameMode_e4Fields_1Delay) ||
			(eGameMode == BVDC_MadGameMode_e3Fields_1Delay)) &&
			(pMadCfg->bReverse32Pulldown))
		{
			BVDC_P_Mad_SoftwareCadence32_isr(hMad);
		}
#endif
	}

#if (!BVDC_P_SUPPORT_HSCL) && (!BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED)
	/* (9) HSCL settings: may be pulled out of per VSYNC computation; */
	/* horizontal scaler in MAD is always a cut version of the one in SCL, so we use
	* the same macro */

	/* Horizantal step HW reg uses U5.17 in older arch, U5.26 after smooth non-linear is
	* suported. Since CPU uses 32 bits int, calc step with 26 bits frac needs special
	* handling (see the delta calcu in the case of nonlinear scaling). It is the step
	* delta and internal step accum reg, not the initial step value, that really need 26
	* frac bits, therefore we use 21 bits for trade off */
	ulFirHrzStep = BVDC_P_SCL_H_STEP_NRM_TO_SPEC(pPicture->ulHsclNrmHrzSrcStep); /* U4.17, U5.17, U5.26 */
	BVDC_P_MAD_SET_HORZ_RATIO(ulFirHrzStep);

	/* Update coeffs? */
	if((hMad->ulUpdateAll) || /* Size/raster changes or other hardstart; pick up new */
		(hWindow->stCurInfo.stCtIndex.ulMadHorzLuma != hMad->ulPrevCtIndexLuma) ||
		(hWindow->stCurInfo.stCtIndex.ulMadHorzChroma != hMad->ulPrevCtIndexChroma) ||
		(hWindow->stCurInfo.hSource->stCurInfo.eCtInputType != hMad->ePrevCtInputType))
	{
		/* We need to choose good fir coefficents base on adjusted
		* fir scale down ratio. */
		pHorzFirCoeff = BVDC_P_SelectFirCoeff_isr(hMad->pHorzFirCoeffTbl,
			hWindow->stCurInfo.stCtIndex.ulMadHorzLuma, hWindow->stCurInfo.hSource->stCurInfo.eCtInputType,
			pPicture->PicComRulInfo.eSrcOrigPolarity, ulFirHrzStep,
			BVDC_P_CT_8_TAP, ulSrcHSize, ulDstHSize);

		BVDC_P_Mad_SetFirCoeff(hMad, pHorzFirCoeff->pulCoeffs);
		BDBG_MSG(("Selected coeffs luma_h=%s", pHorzFirCoeff->pchName));

		/* We need to choose good fir coefficents base on adjusted
		* fir scale down ratio. */
		pHorzFirCoeff = BVDC_P_SelectFirCoeff_isr(hMad->pChromaHorzFirCoeffTbl,
			hWindow->stCurInfo.stCtIndex.ulMadHorzChroma, hWindow->stCurInfo.hSource->stCurInfo.eCtInputType,
			pPicture->PicComRulInfo.eSrcOrigPolarity, ulFirHrzStep,
			BVDC_P_CT_8_TAP, ulSrcHSize, ulDstHSize);

		BVDC_P_Mad_SetChromaFirCoeff(hMad, pHorzFirCoeff->pulCoeffs);
		BDBG_MSG(("Selected coeffs chroma_h=%s", pHorzFirCoeff->pchName));

		hMad->ulPrevCtIndexLuma   = hWindow->stCurInfo.stCtIndex.ulMadHorzLuma;
		hMad->ulPrevCtIndexChroma = hWindow->stCurInfo.stCtIndex.ulMadHorzChroma;
		hMad->ePrevCtInputType = hWindow->stCurInfo.hSource->stCurInfo.eCtInputType;
	}

	/* Compute the phase accumulate intial value in S11.6 or S5.26:
	* luma_horizontal_initial_phase   = (ulFirHrzStep/2) - 0.5;
	* chroma_horizontal_initial_phase = (ulFirHrzStep/2) - 0.5;
	*/
	lHrzPhsAccInit = BVDC_P_FIXED_A_MINUS_FIXED_B(
		BVDC_P_H_INIT_PHASE_RATIO_ADJ(ulFirHrzStep) / 2,
		BVDC_P_H_INIT_PHASE_0_POINT_5);

	BVDC_P_MAD_GET_REG_DATA(MAD_0_HORIZ_FIR_LUMA_SRC_PIC_OFFSET) &= ~(
		BCHP_MASK(MAD_0_HORIZ_FIR_LUMA_SRC_PIC_OFFSET, VALUE));
	BVDC_P_MAD_GET_REG_DATA(MAD_0_HORIZ_FIR_CHROMA_SRC_PIC_OFFSET) &= ~(
		BCHP_MASK(MAD_0_HORIZ_FIR_CHROMA_SRC_PIC_OFFSET, VALUE));
	BVDC_P_MAD_GET_REG_DATA(MAD_0_HORIZ_FIR_LUMA_SRC_PIC_OFFSET) |=  (
		BCHP_FIELD_DATA(MAD_0_HORIZ_FIR_LUMA_SRC_PIC_OFFSET, VALUE,
		pPicture->lHsclCutLeft));
	BVDC_P_MAD_GET_REG_DATA(MAD_0_HORIZ_FIR_CHROMA_SRC_PIC_OFFSET) |=  (
		BCHP_FIELD_DATA(MAD_0_HORIZ_FIR_CHROMA_SRC_PIC_OFFSET, VALUE,
		pPicture->lHsclCutLeft));
	BVDC_P_MAD_GET_REG_DATA(MAD_0_HORIZ_FIR_INIT_PHASE_ACC) &= ~(
		BCHP_MASK(MAD_0_HORIZ_FIR_INIT_PHASE_ACC, SIZE));
	BVDC_P_MAD_GET_REG_DATA(MAD_0_HORIZ_FIR_INIT_PHASE_ACC) |=  (
		BCHP_FIELD_DATA(MAD_0_HORIZ_FIR_INIT_PHASE_ACC, SIZE,
		lHrzPhsAccInit));
#endif

	/* (10) reverse 2:2 soft algorithm; */
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
	/* Compute 2:2 pulldown; only non-game-mode; */
	/* This needs to be done as late as possible so that the histogram data */
	/* is ready for reading and computing */
	if((BVDC_MadGameMode_eOff == eGameMode) ||
		(BVDC_MadGameMode_e5Fields_1Delay == eGameMode) ||
	    (BVDC_MadGameMode_e4Fields_1Delay == eGameMode) ||
		(BVDC_MadGameMode_e3Fields_1Delay == eGameMode))
	{
		if((!(bBufferRepeated /*&& BVDC_P_VNET_USED_MAD_AT_READER(hWindow->stVnetMode)*/))&&(hMad->bReverse22Pulldown ))
			BVDC_P_Mad_SoftwareCadence22_isr(hMad, pPicture->pMadOut->ulWidth, hWindow->stCurInfo.hSource->stCurInfo.pFmtInfo);
		else
		{
			BVDC_P_MAD_FLOG(("Repeat"));
		}



		/* optimize for flicker reduction algorithm */
		/* To minimize the modification, temp use 2:2 pulldown to control this algorithm enable/disable */

		BVDC_P_Mad_Optimize_isr(hMad, hMad->ulRepfMotion);
	}

	if(hMad->bOptimizeStill && !hWindow->stCurInfo.bMotionCustom)
	{
		BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) &= ~(
			BCHP_MASK(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_M_ENABLE) |
			BCHP_MASK(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_L_ENABLE) |
			BCHP_MASK(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_K_ENABLE));
		BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_M_ENABLE, OFF) |
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_L_ENABLE, OFF) |
			BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_K_ENABLE, OFF));
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_5) && BVDC_P_MAD_SCENE_CHANGE_WORKAROUND
		/* mode control 2 */
		/* TODO: what about 3-field 1-delay game mode? */
		if(eGameMode == BVDC_MadGameMode_eOff)
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) &= ~(
				BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL));
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) |=  (
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL, USE_FIELD_B));
		}
#endif
	}
	else
	{
		BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) &= ~(
			BCHP_MASK(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_M_ENABLE) |
			BCHP_MASK(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_L_ENABLE) |
			BCHP_MASK(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_K_ENABLE));
		if(!hWindow->stCurInfo.bMotionCustom)
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_K_ENABLE, ON) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_L_ENABLE, ON) |
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_M_ENABLE, ON));
		}
		else
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0) |=  (
				BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_M_ENABLE, pMadCfg->stMotionSettings.bEnableQmM) |
				BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_L_ENABLE, pMadCfg->stMotionSettings.bEnableQmL) |
				BCHP_FIELD_DATA(MAD_0_MODE_CONTROL_0, GLOBAL_FIELD_K_ENABLE, pMadCfg->stMotionSettings.bEnableQmK));
		}
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_5) && BVDC_P_MAD_SCENE_CHANGE_WORKAROUND
		/* mode control 2 */
		/* TODO: what about 3-field 1-delay game mode? */
		if(eGameMode == BVDC_MadGameMode_eOff)
		{
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) &= ~(
				BCHP_MASK(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL));
			BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_2) |=  (
				BCHP_FIELD_ENUM(MAD_0_MODE_CONTROL_2, FIELD_G_SRC_SEL, USE_ORIGINAL_PIXEL));
		}
#endif
	}
#endif

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER)
	/* Dither */
	if((hMad->stTestFeature1.bEnable) ||
		(BVDC_P_VNET_USED_MAD_AT_WRITER(hWindow->stVnetMode) &&
		hWindow->stCurInfo.stCapTestFeature1.bEnable))
	{
		/* Dither off when:
		*  1) TestFeature1 is enabled on MAD
		*  2) TestFeature1 is enabled on CAP for MAD->SCL->CAP
		*/
		hMad->stDither.ulMode = 0;
	}
	else
	{
		hMad->stDither.ulMode = 2;
	}

#if (BVDC_P_SUPPORT_MAD_VER == BVDC_P_MAD_VER_6)
	BVDC_P_MAD_GET_REG_DATA(MAD_0_DITHER_CTRL) &=  ~(
		BCHP_MASK(MAD_0_DITHER_CTRL, MODE));
	BVDC_P_MAD_GET_REG_DATA(MAD_0_DITHER_CTRL) |=  (
		BCHP_FIELD_DATA(MAD_0_DITHER_CTRL, MODE, hMad->stDither.ulMode));
#endif

#endif

	/* (11) Debug message */
	/* If horizontal or verical are changed update all misc as well. */
	if(BVDC_P_RUL_UPDATE_THRESHOLD == hMad->ulUpdateAll)
	{
		/* Printing out ratio in float format would be nice, but PI
		* code does permit float. */
		BDBG_MSG(("----------------------MAD32"));
		BDBG_MSG(("Fmt               : %s", pFmtInfo->pchFormatStr));
		BDBG_MSG(("Deinterlace       : %dx%d to %dx%d",
			pPicture->pMadIn->ulWidth,  pPicture->pMadIn->ulHeight,
			pPicture->pMadOut->ulWidth, pPicture->pMadOut->ulHeight));
#if (!BVDC_P_SUPPORT_HSCL) && (!BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED)
		BDBG_MSG(("ulFirHorzDownRatio: %-8lx", ulFirHrzStep));
		BDBG_MSG(("H_YInitPhase      : %-8lx", lHrzPhsAccInit));
		BDBG_MSG(("H_CInitPhase      : %-8lx", lHrzPhsAccInit));
#endif
		BDBG_MSG(("SrcPolarity       : %d", pPicture->eSrcPolarity));
		BDBG_MSG(("DstPolarity       : %d", pPicture->eDstPolarity));
	}

#if BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED
	/* must be prior to eSrcPolarity change; */
	BVDC_P_Hscaler_SetInfo_isr(hMad->hHscaler, hWindow, pPicture);
#endif

	/* (12) set src polarity to frame in case feeder and/or scaler after MAD32
	needs detect the frame picture; no harm otherwise. */
	/*if(pPicture->bScalerAfterMad32)*/
	{
		/*BDBG_MSG(("turn on frame flag after MAD32 deinterlace"));*/
		pPicture->eSrcPolarity = BAVC_Polarity_eFrame;
	}

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR)
	BVDC_P_Window_SetMadAnrTestFeature1_isr(hWindow,
		pMadCfg->ePixelFmt, pPicture->pMadIn,
		pPicture, &pMadCfg->stTestFeature1,
		BVDC_P_VNET_USED_MAD_AT_WRITER(hWindow->stVnetMode),
		&hMad->stTestFeature1);
#endif

	BDBG_LEAVE(BVDC_P_Mad_SetInfo_isr);
	return BERR_SUCCESS;
}

/***************************************************************************
* {private}
*
*/
BERR_Code BVDC_P_Mad_SetEnable_isr
( BVDC_P_Mad_Handle                hMad,
 bool                             bEnable )
{
	uint32_t           ulIndex;

	BDBG_OBJECT_ASSERT(hMad, BVDC_MAD);

#if BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED
	BVDC_P_Hscaler_SetEnable_isr(hMad->hHscaler, bEnable);
#endif

	if(!BVDC_P_MAD_COMPARE_FIELD_DATA(MAD_0_ENABLE_CONTROL, ENABLE, bEnable))
	{
		hMad->ulUpdateAll = BVDC_P_RUL_UPDATE_THRESHOLD;
	}

	/* Reset 2:2 & 3:2 pulldown software counters */
	for(ulIndex = 0; ulIndex < 5; ulIndex++)
	{
		hMad->alPhase[ulIndex] = 0;
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_5)
		hMad->stRev32Statistics.aulP[ulIndex]    = 1;
		hMad->stRev32Statistics.aulV[ulIndex]    = 1;
#endif
	}
	hMad->ulPhaseCounter = 0;
	hMad->ulPhaseCounter22 = 0;
	hMad->alPhase22[0] = 0;
	hMad->alPhase22[1] = 0;

	/* Turn on/off the scaler. */
	BVDC_P_MAD_GET_REG_DATA(MAD_0_ENABLE_CONTROL) &= ~(
		BCHP_MASK(MAD_0_ENABLE_CONTROL, ENABLE));

	BVDC_P_MAD_GET_REG_DATA(MAD_0_ENABLE_CONTROL) |=  (bEnable
		? BCHP_FIELD_ENUM(MAD_0_ENABLE_CONTROL, ENABLE, ON)
		: BCHP_FIELD_ENUM(MAD_0_ENABLE_CONTROL, ENABLE, OFF));

	/* Need to reset loops when going from mute to/from unmute to drain
	* left over data. */
	BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_OUTPUT_CONTROL) &= ~(
		BCHP_MASK(MAD_0_IT_OUTPUT_CONTROL, LOOPS_RESET));

	BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_OUTPUT_CONTROL) |=  (
		BCHP_FIELD_ENUM(MAD_0_IT_OUTPUT_CONTROL, LOOPS_RESET, BOTH));

	return BERR_SUCCESS;
}

/***************************************************************************
* Return the user set of mad chroma
*/
void BVDC_P_Mad_GetUserChroma_isr
( BVDC_P_Mad_Handle                hMad,
 BVDC_Deinterlace_ChromaSettings *pChromaSettings )
{
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
	BDBG_OBJECT_ASSERT(hMad, BVDC_MAD);
	if(pChromaSettings)
	{
		pChromaSettings->bChromaField420EdgeDetMode =
			BCHP_GET_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0),
			MAD_0_MODE_CONTROL_0,
			CHROMA_FIELD_420_EDGE_DET_MODE);
		pChromaSettings->bChromaField420InitPhase =
			BCHP_GET_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0),
			MAD_0_MODE_CONTROL_0,
			CHROMA_FIELD_420_INITIAL_PHASE);
		pChromaSettings->eChromaField420InvMethod =
			BCHP_GET_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0),
			MAD_0_MODE_CONTROL_0,
			CHROMA_FIELD_420_INV_METHOD);
		pChromaSettings->eChroma422MotionMode     =
			BCHP_GET_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0),
			MAD_0_MODE_CONTROL_0,
			CHROMA_422_MOTION_MODE);
		pChromaSettings->eChroma420MotionMode     =
			BCHP_GET_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0),
			MAD_0_MODE_CONTROL_0,
			CHROMA_FIELD_420_MOTION_MODE);
		pChromaSettings->eChroma422InverseTelecineMode =
			BCHP_GET_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(MAD_0_XCHROMA_MODE),
			MAD_0_XCHROMA_MODE,
			PIC_422_IT);
		pChromaSettings->eChroma422MotionAdaptiveMode =
			BCHP_GET_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(MAD_0_XCHROMA_MODE),
			MAD_0_XCHROMA_MODE,
			PIC_422_MA);
		pChromaSettings->ulMaxXChroma =
			BCHP_GET_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(MAD_0_XCHROMA_CONTROL_0),
			MAD_0_XCHROMA_CONTROL_0,
			MAX_XCHROMA);
		pChromaSettings->eChroma420MotionAdaptiveMode =
			BCHP_GET_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(MAD_0_XCHROMA_MODE),
			MAD_0_XCHROMA_MODE,
			FIELD_420_MA);
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_7)
		pChromaSettings->bMS_3548 =
			BCHP_GET_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(MAD_0_MOTION_CAL_CTRL),
			MAD_0_MOTION_CAL_CTRL,
			MS_3548);
		pChromaSettings->bMT_3548 =
			BCHP_GET_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(MAD_0_MOTION_CAL_CTRL),
			MAD_0_MOTION_CAL_CTRL,
			MT_3548);
#endif
	}
#else
	BSTD_UNUSED(hMad);
	BSTD_UNUSED(pChromaSettings);
#endif

	return;
}

/***************************************************************************
* Return the user set of mad motino
*/
void BVDC_P_Mad_GetUserMotion_isr
( BVDC_P_Mad_Handle                hMad,
 BVDC_Deinterlace_MotionSettings *pMotionSettings )
{
	BDBG_OBJECT_ASSERT(hMad, BVDC_MAD);
	if(pMotionSettings)
	{
		pMotionSettings->eSmMode =
			BCHP_GET_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0),
			MAD_0_MODE_CONTROL_0,
			SM_MODE_SEL);
		pMotionSettings->eTmMode =
			BCHP_GET_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0),
			MAD_0_MODE_CONTROL_0,
			TM_MODE_SEL);
		pMotionSettings->bEnableQmK =
			BCHP_GET_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0),
			MAD_0_MODE_CONTROL_0,
			GLOBAL_FIELD_K_ENABLE);
		pMotionSettings->bEnableQmL =
			BCHP_GET_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0),
			MAD_0_MODE_CONTROL_0,
			GLOBAL_FIELD_L_ENABLE);
		pMotionSettings->bEnableQmM =
			BCHP_GET_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(MAD_0_MODE_CONTROL_0),
			MAD_0_MODE_CONTROL_0,
			GLOBAL_FIELD_M_ENABLE);
	}

	return;
}
/***************************************************************************
* Return the user set of mad 3:2 pulldown
*/
void BVDC_P_Mad_GetUserReverse32_isr
( BVDC_P_Mad_Handle                   hMad,
 BVDC_Deinterlace_Reverse32Settings *pRev32Settings )
{
	BDBG_OBJECT_ASSERT(hMad, BVDC_MAD);
	if(pRev32Settings)
	{
		pRev32Settings->ulBadEditLevel =
			BCHP_GET_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_2),
			MAD_0_IT_FIELD_PHASE_CALC_CONTROL_2,
			REV32_BAD_EDIT_LEVEL);
#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_3)
		pRev32Settings->ulBwvLuma32Threshold =
			BCHP_GET_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(MAD_0_BWV_CONTROL_1),
			MAD_0_BWV_CONTROL_1,
			LUMA_32_THRESH);
#endif
		pRev32Settings->ulEnterLockLevel =
			BCHP_GET_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_0),
			MAD_0_IT_FIELD_PHASE_CALC_CONTROL_0,
			REV32_ENTER_LOCK_LEVEL);
		pRev32Settings->ulLockSatLevel =
			BCHP_GET_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_2),
			MAD_0_IT_FIELD_PHASE_CALC_CONTROL_2,
			REV32_LOCK_SAT_LEVEL);
		pRev32Settings->ulPhaseMatchThreshold =
			BCHP_GET_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_0),
			MAD_0_IT_FIELD_PHASE_CALC_CONTROL_0,
			REV32_PHASE_MATCH_THRESH);
		pRev32Settings->ulRepfPxlCorrectLevel =
			BCHP_GET_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_1),
			MAD_0_IT_FIELD_PHASE_CALC_CONTROL_1,
			REV32_REPF_PIX_CORRECT_LEVEL);
		pRev32Settings->ulRepfVetoLevel =
			BCHP_GET_FIELD_DATA(BVDC_P_MAD_GET_REG_DATA(MAD_0_IT_FIELD_PHASE_CALC_CONTROL_1),
			MAD_0_IT_FIELD_PHASE_CALC_CONTROL_1,
			REV32_REPF_VETO_LEVEL);
	}

	return;
}
/***************************************************************************
* Return the user set of mad chroma
*/
void BVDC_P_Mad_GetUserReverse22_isr
( BVDC_P_Mad_Handle                   hMad,
 BVDC_Deinterlace_Reverse22Settings *pRev22Settings )
{
	BDBG_OBJECT_ASSERT(hMad, BVDC_MAD);
	if(pRev22Settings)
	{
		pRev22Settings->ulNonMatchMatchRatio = hMad->ulRev22NonMatchMatchRatio;
		pRev22Settings->ulEnterLockLevel = hMad->ulRev22EnterLockLevel;
		pRev22Settings->ulLockSatLevel = hMad->ulRev22LocksatLevel;
	}

	return;
}

bool BVDC_P_Mad_BeHardStart_isr
( BVDC_P_Mad_Handle                 hMad )
{
	BDBG_OBJECT_ASSERT(hMad, BVDC_MAD);
	return BVDC_P_MAD_COMPARE_FIELD_NAME(MAD_0_MODE_CONTROL_1, MODE_SEL, HARD_START);
}

/***************************************************************************
*
*/
static void BVDC_P_Mad_Init_Rev32_DynamicDefault
( const BFMT_VideoInfo               *pFmtInfo,
 uint32_t                            ulSampleFactor,
 BVDC_Deinterlace_Reverse32Settings *pReverse32Settings )
{
	BDBG_ASSERT(pFmtInfo);
	BDBG_ASSERT(pReverse32Settings);

	/* MAD_0_IT_FIELD_PHASE_CALC_CONTROL_0.REV32_PHASE_MATCH_THRESH */
	pReverse32Settings->ulPhaseMatchThreshold = BVDC_P_MAD_REV32_PHASE_MATCH_THRESH;

	/* MAD_0_IT_FIELD_PHASE_CALC_CONTROL_0.REV32_ENTER_LOCK_LEVEL */
	pReverse32Settings->ulEnterLockLevel = BVDC_P_MAD_REV32_ENTER_LOCK_LEVEL;

	/* MAD_0_IT_FIELD_PHASE_CALC_CONTROL_2.REV32_LOCK_SAT_LEVEL */
	pReverse32Settings->ulLockSatLevel = BVDC_P_MAD_REV32_LOCK_SAT_LEVEL;

	/* MAD_0_IT_FIELD_PHASE_CALC_CONTROL_1.REV32_REPF_VETO_LEVEL */
	/* MAD_0_IT_FIELD_PHASE_CALC_CONTROL_1.REV32_REPF_PIX_CORRECT_LEVEL */
	/* MAD_0_IT_FIELD_PHASE_CALC_CONTROL_2.REV32_BAD_EDIT_LEVEL */
	/* Oversample */
	if(ulSampleFactor)
	{
		if(VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt))
		{
			/* PR45231, PR45707 */
			pReverse32Settings->ulBadEditLevel        = 0x880;
			pReverse32Settings->ulRepfPxlCorrectLevel = 0x180;
			pReverse32Settings->ulRepfVetoLevel       = 0x290;
		}
		else
		{
			pReverse32Settings->ulBadEditLevel        = 0x2800;
			pReverse32Settings->ulRepfPxlCorrectLevel = 0x400;
			pReverse32Settings->ulRepfVetoLevel       = 0xa00;
		}
	}
	else
	{
		if(VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt))
		{
			pReverse32Settings->ulBadEditLevel        = 0x500;
			pReverse32Settings->ulRepfPxlCorrectLevel = 0x120;
			pReverse32Settings->ulRepfVetoLevel       = 0x271;
		}
		else
		{
			pReverse32Settings->ulBadEditLevel        = 0x1400;
			pReverse32Settings->ulRepfPxlCorrectLevel = 0x400;
			pReverse32Settings->ulRepfVetoLevel       = 0xa00;
		}
	}

	/* MAD_0_BWV_CONTROL_1.LUMA_32_THRESH */
	if((ulSampleFactor) && VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt))
	{
		pReverse32Settings->ulBwvLuma32Threshold = BVDC_P_MAD_REV32_LUMA_32_THRESH_OVERSAMPLE;
	}
	else
	{
		pReverse32Settings->ulBwvLuma32Threshold = BVDC_P_MAD_REV32_LUMA_32_THRESH;
	}
}

/***************************************************************************
*
*/
static void BVDC_P_Mad_Init_Rev22_DynamicDefault
( const BFMT_VideoInfo               *pFmtInfo,
 BVDC_Deinterlace_Reverse22Settings *pReverse22Settings )
{
	BDBG_ASSERT(pFmtInfo);
	BDBG_ASSERT(pReverse22Settings);

	pReverse22Settings->ulNonMatchMatchRatio = BVDC_P_MAD_NONMATCH_MATCH_RATIO;
	pReverse22Settings->ulEnterLockLevel = BVDC_P_MAD_REV22_ENTER_LOCK_LEVEL;
	pReverse22Settings->ulLockSatLevel = BVDC_P_MAD_REV22_LOCK_SAT_LEVEL;
	pReverse22Settings->ulBwvLuma22Threshold = 0; /* unused */

	/* Not used in current 2:2 pulldown alg */
	/*pMad->ulRev22BwvLumaThreshold = pMadCfg->stReverse22Settings.ulBwvLuma22Threshold;*/
	/* PR45232: increase enter/exit/sat 22lock level */
	if(VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt))
	{
		pReverse22Settings->ulEnterLockLevel += BVDC_P_MAD_REV22_LEVEL_ADJ;
		pReverse22Settings->ulLockSatLevel   += BVDC_P_MAD_REV22_LEVEL_ADJ;
	}

}

/***************************************************************************
*
*/
static void BVDC_P_Mad_Init_Chroma_DynamicDefault
( const BFMT_VideoInfo               *pFmtInfo,
 BVDC_MadGameMode                    eGameMode,
 bool                                bMfdSrc,
 bool                                bCvbsSrc,
 BVDC_Deinterlace_ChromaSettings    *pChromaSettings )
{
#if (BVDC_P_SUPPORT_MAD_VER < BVDC_P_MAD_VER_4)
	BSTD_UNUSED(bCvbsSrc);
#endif

	BDBG_ASSERT(pFmtInfo);
	BDBG_ASSERT(pChromaSettings);

	/* MAD_0_MODE_CONTROL_0.CHROMA_FIELD_420_EDGE_DET_MODE */
	pChromaSettings->bChromaField420EdgeDetMode = true;
	/* MAD_0_MODE_CONTROL_0.CHROMA_FIELD_420_INITIAL_PHASE */
	pChromaSettings->bChromaField420InitPhase = false;
	/* MAD_0_MODE_CONTROL_0.CHROMA_FIELD_420_INV_METHOD */
	pChromaSettings->eChromaField420InvMethod   = !bMfdSrc;

	/* MAD_0_XCHROMA_MODE.PIC_422_MA */
	if(VIDEO_FORMAT_IS_PAL(pFmtInfo->eVideoFmt))
	{
		pChromaSettings->eChroma422MotionAdaptiveMode
			= BCHP_MAD_0_XCHROMA_MODE_PIC_422_MA_INT_420;
	}
	else
	{
		pChromaSettings->eChroma422MotionAdaptiveMode
			= BCHP_MAD_0_XCHROMA_MODE_PIC_422_MA_MA_CHROMA;
	}

	/* MAD_0_XCHROMA_MODE.PIC_422_IT */
	if(VIDEO_FORMAT_IS_PAL(pFmtInfo->eVideoFmt))
	{
		pChromaSettings->eChroma422InverseTelecineMode
			= BCHP_MAD_0_XCHROMA_MODE_PIC_422_IT_INT_420;
	}
	else
	{
		if(bMfdSrc)
		{
			pChromaSettings->eChroma422InverseTelecineMode
				= BCHP_MAD_0_XCHROMA_MODE_PIC_422_IT_IT_DIRECT;
		}
		else
		{
			pChromaSettings->eChroma422InverseTelecineMode
				= BCHP_MAD_0_XCHROMA_MODE_PIC_422_IT_MA_CHROMA;
		}
	}

	/* MAD_0_XCHROMA_CONTROL_0.MAX_XCHROMA */
	pChromaSettings->ulMaxXChroma = 0xff;

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_4)
	if((bCvbsSrc) &&
		(VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt) || VIDEO_FORMAT_IS_PAL(pFmtInfo->eVideoFmt)))
	{
		/* MAD_0_MODE_CONTROL_0.CHROMA_422_MOTION_MODE */
		pChromaSettings->eChroma422MotionMode = BVDC_Deinterlace_ChromaMotionMode_eXchromaAware;

		/* MAD_0_MODE_CONTROL_0.CHROMA_FIELD_420_MOTION_MODE */
		pChromaSettings->eChroma420MotionMode = BVDC_Deinterlace_ChromaMotionMode_eXchromaAware;
	}
	else
	{
		/* MAD_0_MODE_CONTROL_0.CHROMA_422_MOTION_MODE */
		pChromaSettings->eChroma422MotionMode = BVDC_Deinterlace_ChromaMotionMode_eDigitalChroma;

		/* MAD_0_MODE_CONTROL_0.CHROMA_FIELD_420_MOTION_MODE */
		pChromaSettings->eChroma420MotionMode = BVDC_Deinterlace_ChromaMotionMode_eDigitalChroma;
	}
#endif

#if (BVDC_P_SUPPORT_MAD_VER >= BVDC_P_MAD_VER_7)
	/* MAD_0_MOTION_CAL_CTRL.MS_3548 */
	pChromaSettings->bMS_3548 = !bCvbsSrc;
	/* MAD_0_MOTION_CAL_CTRL.MT_3548 */
	pChromaSettings->bMT_3548 = !bCvbsSrc;
#endif

	/* MAD_0_XCHROMA_MODE.FIELD_420_MA */
	if((eGameMode == BVDC_MadGameMode_e3Fields_1Delay)       ||
		(eGameMode == BVDC_MadGameMode_e3Fields_0Delay)       ||
		(eGameMode == BVDC_MadGameMode_e3Fields_ForceSpatial))
	{
		pChromaSettings->eChroma420MotionAdaptiveMode =
			BCHP_MAD_0_XCHROMA_MODE_FIELD_420_MA_MA_CHROMA;
	}
	else
	{
		pChromaSettings->eChroma420MotionAdaptiveMode =
			BCHP_MAD_0_XCHROMA_MODE_FIELD_420_MA_INT_420;
	}

}

/***************************************************************************
*
*/
BERR_Code BVDC_P_Mad_Init_DynamicDefault
( BVDC_Window_Handle                  hWindow,
 BVDC_Deinterlace_Reverse32Settings *pReverse32Settings,
 BVDC_Deinterlace_Reverse22Settings *pReverse22Settings,
 BVDC_Deinterlace_ChromaSettings    *pChromaSettings )
{
	bool              bMfdSrc = false, bCvbsSrc = false;
	BVDC_MadGameMode  eGameMode;
	BVDC_P_Deinterlace_Settings       *pNewSettings;
	const BFMT_VideoInfo              *pFmtInfo;

	/* check parameters */
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);
	pNewSettings = &hWindow->stNewInfo.stMadSettings;
	pFmtInfo  = hWindow->stCurInfo.hSource->stCurInfo.pFmtInfo;

	BDBG_MSG(("Source is %s (ulSampleFactor = %d)", pFmtInfo->pchFormatStr,
		hWindow->stCurInfo.hSource->ulSampleFactor));

	if(!hWindow->stCurResource.hMad32)
	{
		eGameMode = BVDC_MadGameMode_eOff;
	}
	else
	{
		eGameMode = hWindow->stCurResource.hMad32->eGameMode;
	}

	/* Check source */
	if(BVDC_P_SRC_IS_MPEG(hWindow->stCurInfo.hSource->eId))
	{
		bMfdSrc = true;
	}
	else if(BVDC_P_SRC_IS_ANALOG(hWindow->stCurInfo.hSource->eId))
	{
		bCvbsSrc = (
			hWindow->stCurInfo.hSource->stCurInfo.bCvbsInput ||
			hWindow->stCurInfo.hSource->stCurInfo.bIfdInput);
	}
	/* normally 656in source comes from external VDEC cvbs input */
	else if(BVDC_P_SRC_IS_ITU656(hWindow->stCurInfo.hSource->eId))
	{
		bCvbsSrc = true;
	}

	if(pReverse32Settings)
	{
		BVDC_P_Mad_Init_Rev32_DynamicDefault(pFmtInfo,
			hWindow->stCurInfo.hSource->ulSampleFactor, pReverse32Settings);
	}

	if(pReverse22Settings)
	{
		BVDC_P_Mad_Init_Rev22_DynamicDefault(pFmtInfo, pReverse22Settings);
	}

	if(pChromaSettings)
	{
		BVDC_P_Mad_Init_Chroma_DynamicDefault(pFmtInfo, eGameMode,
			bMfdSrc, bCvbsSrc, pChromaSettings);
	}

	return BERR_SUCCESS;
}

/***************************************************************************/
/* No support for any mad */
#else
BERR_Code BVDC_P_Mad_Create
( BVDC_P_Mad_Handle            *phMad,
 BREG_Handle                   hRegister,
 BVDC_P_MadId                  eMadId,
 BVDC_P_Resource_Handle        hResource )
{
	BSTD_UNUSED(phMad);
	BSTD_UNUSED(hRegister);
	BSTD_UNUSED(eMadId);
	BSTD_UNUSED(hResource);
	return BERR_SUCCESS;
}

void BVDC_P_Mad_Destroy
( BVDC_P_Mad_Handle             hMad )
{
	BSTD_UNUSED(hMad);
}

void BVDC_P_Mad_BuildRul_isr
( const BVDC_P_Mad_Handle       hMad,
 BVDC_P_ListInfo              *pList,
 BVDC_P_State                  eVnetState,
 BVDC_P_PicComRulInfo         *pPicComRulInfo )
{
	BSTD_UNUSED(hMad);
	BSTD_UNUSED(pList);
	BSTD_UNUSED(eVnetState);
	BSTD_UNUSED(pPicComRulInfo);
}

BERR_Code BVDC_P_Mad_SetInfo_isr
( BVDC_P_Mad_Handle             hMad,
 const BVDC_Window_Handle      hWindow,
 const BVDC_P_PictureNodePtr   pPicture )
{
	BSTD_UNUSED(hMad);
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pPicture);
	return BERR_SUCCESS;
}

BERR_Code BVDC_P_Mad_SetEnable_isr
( BVDC_P_Mad_Handle             hMad,
 bool                          bEnable )
{
	BSTD_UNUSED(hMad);
	BSTD_UNUSED(bEnable);
	return BERR_SUCCESS;
}

/***************************************************************************
* Return the user set of mad chroma
*/
void BVDC_P_Mad_GetUserChroma_isr
( BVDC_P_Mad_Handle                hMad,
 BVDC_Deinterlace_ChromaSettings *pChromaSettings )
{
	BSTD_UNUSED(hMad);
	BSTD_UNUSED(pChromaSettings);
}

/***************************************************************************
* Return the user set of mad motino
*/
void BVDC_P_Mad_GetUserMotion_isr
( BVDC_P_Mad_Handle                  hMad,
 BVDC_Deinterlace_MotionSettings   *pMotionSettings )
{
	BSTD_UNUSED(hMad);
	BSTD_UNUSED(pMotionSettings);
}
/***************************************************************************
* Return the user set of mad 3:2 pulldown
*/
void BVDC_P_Mad_GetUserReverse32_isr
( BVDC_P_Mad_Handle                   hMad,
 BVDC_Deinterlace_Reverse32Settings *pRev32Settings )
{
	BSTD_UNUSED(hMad);
	BSTD_UNUSED(pRev32Settings);
}
/***************************************************************************
* Return the user set of mad chroma
*/
void BVDC_P_Mad_GetUserReverse22_isr
( BVDC_P_Mad_Handle                   hMad,
 BVDC_Deinterlace_Reverse22Settings *pRev22Settings )
{
	BSTD_UNUSED(hMad);
	BSTD_UNUSED(pRev22Settings);
}

void BVDC_P_Mad_SetVnetAllocBuf_isr
( BVDC_P_Mad_Handle                hMad,
 uint32_t                         ulSrcMuxValue,
 BVDC_P_VnetPatch                 eVnetPatchMode,
 BVDC_P_BufferHeapId              eMadPixelBufHeapId,
 BVDC_P_BufferHeapId              eMadQmBufHeapId,
 uint16_t                         usPixelBufferCnt )
{
	BSTD_UNUSED(hMad);
	BSTD_UNUSED(ulSrcMuxValue);
	BSTD_UNUSED(eVnetPatchMode);
	BSTD_UNUSED(eMadPixelBufHeapId);
	BSTD_UNUSED(eMadQmBufHeapId);
	BSTD_UNUSED(usPixelBufferCnt);
}

void BVDC_P_Mad_UnsetVnetFreeBuf_isr
( BVDC_P_Mad_Handle          hMad )
{
	BSTD_UNUSED(hMad);
}

bool BVDC_P_Mad_BeHardStart_isr
( BVDC_P_Mad_Handle                 hMad )
{
	BSTD_UNUSED(hMad);
	return false;
}

BERR_Code BVDC_P_Mad_Init_DynamicDefault
( BVDC_Window_Handle                  hWindow,
 BVDC_Deinterlace_Reverse32Settings *pReverse32Settings,
 BVDC_Deinterlace_Reverse22Settings *pReverse22Settings,
 BVDC_Deinterlace_ChromaSettings    *pChromaSettings )
{
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pReverse32Settings);
	BSTD_UNUSED(pReverse22Settings);
	BSTD_UNUSED(pChromaSettings);
	return BERR_SUCCESS;
}

#endif /* MAD SUPPORT */

/***************************************************************************
* Initialized back to default whatever user did not customized.
*
*/
void BVDC_P_Mad_Init_Default
( BVDC_MadGameMode                      *peGameMode,
 BPXL_Format                           *pePxlFormat,
 BVDC_Mode                             *pePqEnhancement,
 BVDC_Video_TestFeature1_Settings      *pVideoTestFeature1,
 bool                                  *pbShrinkWidth,
 bool                                  *pbReverse32Pulldown,
 BVDC_Deinterlace_Reverse32Settings    *pReverse32Settings,
 bool                                  *pbReverse22Pulldown,
 BVDC_Deinterlace_Reverse22Settings    *pReverse22Settings,
 BVDC_Deinterlace_ChromaSettings       *pChromaSettings,
 BVDC_Deinterlace_MotionSettings       *pMotionSettings )
{
	if(peGameMode)
	{
		*peGameMode = BVDC_MadGameMode_eOff;
	}

	if(pePxlFormat)
	{
		*pePxlFormat = BVDC_P_CAP_PIXEL_FORMAT_8BIT422;
	}

	if(pePqEnhancement)
	{
		*pePqEnhancement = BVDC_Mode_eAuto;
	}

	if(pbReverse22Pulldown)
	{
		*pbReverse22Pulldown = true;
	}

	if(pbReverse32Pulldown)
	{
		*pbReverse32Pulldown = true;
	}

	if(pbShrinkWidth)
	{
		*pbShrinkWidth = false;
	}

	if (pReverse32Settings)
	{
		pReverse32Settings->ulBadEditLevel = 0x500;
		pReverse32Settings->ulEnterLockLevel = BVDC_P_MAD_REV32_ENTER_LOCK_LEVEL;
		pReverse32Settings->ulLockSatLevel   = BVDC_P_MAD_REV32_LOCK_SAT_LEVEL;
		pReverse32Settings->ulPhaseMatchThreshold = BVDC_P_MAD_REV32_PHASE_MATCH_THRESH;
		pReverse32Settings->ulRepfPxlCorrectLevel = 0xbb;
		pReverse32Settings->ulRepfVetoLevel = 0x271;
		pReverse32Settings->ulBwvLuma32Threshold = BVDC_P_MAD_REV32_LUMA_32_THRESH;
	}

	if (pReverse22Settings)
	{
		pReverse22Settings->ulNonMatchMatchRatio = BVDC_P_MAD_NONMATCH_MATCH_RATIO;
		pReverse22Settings->ulEnterLockLevel     = BVDC_P_MAD_REV22_ENTER_LOCK_LEVEL;
		pReverse22Settings->ulLockSatLevel       = BVDC_P_MAD_REV22_LOCK_SAT_LEVEL;
		pReverse22Settings->ulBwvLuma22Threshold = 0; /* unused */
	}

	if (pChromaSettings)
	{
		pChromaSettings->bChromaField420EdgeDetMode = true;
		pChromaSettings->bChromaField420InitPhase = false;
		pChromaSettings->eChroma422InverseTelecineMode = BVDC_Deinterlace_Chroma422ItMode_eMotionAdaptive;
		pChromaSettings->eChroma422MotionAdaptiveMode = BVDC_Deinterlace_Chroma422MaMode_eMotionAdaptive;
		pChromaSettings->eChromaField420InvMethod = BVDC_Deinterlace_Chroma420InvMethod_ePoly;
		pChromaSettings->ulMaxXChroma = 0xFF;
		pChromaSettings->eChroma420MotionAdaptiveMode = BVDC_Deinterlace_Chroma422MaMode_eMotionAdaptive;
		pChromaSettings->eChroma420MotionMode = BVDC_Deinterlace_ChromaMotionMode_eXchromaAware;
		pChromaSettings->eChroma422MotionMode = BVDC_Deinterlace_ChromaMotionMode_eXchromaAware;
		pChromaSettings->bMS_3548 = false;
		pChromaSettings->bMT_3548 = false;
	}

	if (pMotionSettings)
	{
		pMotionSettings->eTmMode = 2;
		pMotionSettings->eSmMode = 1;
		pMotionSettings->bEnableQmM = true;
		pMotionSettings->bEnableQmL = true;
		pMotionSettings->bEnableQmK = true;
	}

	if(pVideoTestFeature1)
	{
		pVideoTestFeature1->bEnable = false;
		pVideoTestFeature1->ulBitsPerPixel = 0;
	}

	return;
}

void BVDC_P_Mad_Init_Custom
( BVDC_422To444UpSampler            *pUpSampler,
 BVDC_444To422DnSampler            *pDnSampler,
 BVDC_Deinterlace_LowAngleSettings *pLowAngles )
{
	if(pUpSampler)
	{
		pUpSampler->eFilterType = BVDC_422To444Filter_eTenTaps;
		pUpSampler->eRingRemoval   = BVDC_RingSuppressionMode_eNormal;
		pUpSampler->bUnbiasedRound = true;
	}

	if(pDnSampler)
	{
		pDnSampler->eFilterType    = BVDC_444To422Filter_eDecimate;
		pDnSampler->eRingRemoval   = BVDC_RingSuppressionMode_eNormal;
	}

	if(pLowAngles)
	{
		pLowAngles->ulLaControlDirRatio = 8;
		pLowAngles->ulLaControlRangeLimitScale = 0x11;
		pLowAngles->ulLaMinNorthStrength = 0;
	}

	return;
}

/***************************************************************************
*
*/
uint16_t BVDC_P_Mad_GetVsyncDelayNum_isr
( BVDC_MadGameMode                 eGameMode )
{
	return s_aMadGameModeInfo[eGameMode].usDelay;
}

/***************************************************************************
*
*/
uint16_t BVDC_P_Mad_GetPixBufCnt_isr
( BVDC_MadGameMode                 eGameMode )
{
	return s_aMadGameModeInfo[eGameMode].usPixelBufferCnt;
}

/* End of file. */
