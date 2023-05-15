/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_display.c $
 * $brcm_Revision: Hydra_Software_Devel/195 $
 * $brcm_Date: 8/16/12 2:42p $
 *
 * Module Description:
 *  Main module for BVDC_Display_ functions.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/modularvec/bvdc_display.c $
 * 
 * Hydra_Software_Devel/195   8/16/12 2:42p yuxiaz
 * SW7425-3619: Add const to read only settings.
 * 
 * Hydra_Software_Devel/194   8/10/12 3:26p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/193   6/20/12 4:42p pntruong
 * SW7405-5488: Added support for querying display capabilties of vdc
 * supportable format.
 *
 * Hydra_Software_Devel/192   6/14/12 9:46a pntruong
 * SW7425-3226: Fixed Coverity errors involving suspicious tests for NULL
 * pointer.
 *
 * Hydra_Software_Devel/191   3/2/12 4:00p pntruong
 * SW7231-584: Added artificial display vsync generation to support dsp
 * transcoding.
 *
 * Hydra_Software_Devel/190   3/2/12 2:24p yuxiaz
 * SW7425-2526: Added runtime query capabilities for compositor, window
 * and display.
 *
 * Hydra_Software_Devel/189   1/16/12 2:55p yuxiaz
 * SW7405-5587: Fixed mosaic format change stress test.
 *
 * Hydra_Software_Devel/188   12/13/11 5:11p yuxiaz
 * SW7425-1875: Added support for source buffer selection in 3d mode.
 *
 * Hydra_Software_Devel/187   11/10/11 1:46p tdo
 * SW7400-3064: Aspect ratio incorrect (narrow) on 7400
 *
 * Hydra_Software_Devel/186   8/29/11 2:25p syang
 * SW7425-1170: more clean up
 *
 * Hydra_Software_Devel/185   8/28/11 5:00p yuxiaz
 * SW7425-1170: Fixed build on 7405
 *
 * Hydra_Software_Devel/184   8/5/11 8:24p albertl
 * SW7425-745: Added blanking to compositor color for all outputs and all
 * colorspaces.
 *
 * Hydra_Software_Devel/183   7/18/11 5:40p albertl
 * SW7408-291: Relaxed VF filter requirements to be able to take any
 * register array size greater than platform minimum.
 *
 * Hydra_Software_Devel/182   7/11/11 2:16p tdo
 * SW7420-1971: Video Pause seen when VEC alignment is going on.  Add flag
 * to keep BVN connected while doing alignment.
 *
 * Hydra_Software_Devel/181   7/8/11 6:01p albertl
 * SW7408-291: Updated VF filter API to use Dac enums for channel
 * selection.
 *
 * Hydra_Software_Devel/180   7/8/11 3:18p yuxiaz
 * SW7425-592: Added API to get and set display sample aspect ratio
 * settings.
 *
 * Hydra_Software_Devel/179   7/7/11 2:02p albertl
 * SW7408-291: Updated API for VF set function.
 *
 * Hydra_Software_Devel/178   7/7/11 1:59p albertl
 * SW7408-291: Updated VF filter API to include display output.
 *
 * Hydra_Software_Devel/177   6/22/11 5:58p albertl
 * SW7408-291: Updated interface of get vf filter API and renamed
 * variables for clarity.
 *
 * Hydra_Software_Devel/176   6/22/11 4:17p albertl
 * SW7408-291: Added implementation of custom vf filter API for older
 * chips.
 *
 * Hydra_Software_Devel/175   6/17/11 4:35p tdo
 * SW7405-5339: Correctly move state when destroy
 *
 * Hydra_Software_Devel/174   12/30/10 10:03a pntruong
 * SW3548-1660: Turn BVDC_Test_Display_GetInterruptName into official api
 * BVDC_Display_GetInterruptName.
 *
 * Hydra_Software_Devel/173   12/8/10 12:36a tdo
 * SW7422-92: Add stub function
 *
 * Hydra_Software_Devel/172   10/6/10 10:37a hongtaoz
 * SW7425-29: added STG support;
 *
 * Hydra_Software_Devel/171   5/7/10 7:09p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings.
 *
 * Hydra_Software_Devel/170   4/19/10 10:14p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/169   4/7/10 11:49a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Fix typo
 *
 * Hydra_Software_Devel/168   4/7/10 11:28a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/167   4/5/10 4:04p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/166   2/23/10 2:56p darnstein
 * SW7400-2620: add support for double rate 480P.
 *
 * Hydra_Software_Devel/165   1/11/10 1:16p rpan
 * SW7400-2620: Undo HDMI pixel replication. The 7400/7405 HDMI core
 * doesn't support this feature.
 *
 * Hydra_Software_Devel/164   12/18/09 8:59a rpan
 * SW3556-935: New interface for setting spread spectrum.
 *
 * Hydra_Software_Devel/163   12/11/09 3:41p rpan
 * SW7400-2620: Added HDMI pixel replication for 480p display. This is for
 * chips such as 7400 and 7405 only.
 *
 * Hydra_Software_Devel/162   10/14/09 10:35a tdo
 * SW7405-3004: Fix coverity error
 *
 * Hydra_Software_Devel/161   10/13/09 5:47p darnstein
 * SW7405-3004: install 480P ARIB output via BVDC display settings.
 *
 * Hydra_Software_Devel/160   4/30/09 3:03p albertl
 * PR54593: Fixed Set and GetDvoMatrix to take and return matrices in
 * YCbCr->RGB and YCbCr->YCbCr format and convert to hardware format
 * internally.
 *
 * Hydra_Software_Devel/159   4/7/09 10:21a tdo
 * PR53373: Add HDMI deep color support
 *
 * Hydra_Software_Devel/158   3/9/09 5:58p pntruong
 * PR52991: No CVBS out on DAC (AH20) from NTSC Demod.  Added ability to
 * output ifd to dac.
 *
 * Hydra_Software_Devel/157   3/3/09 8:38p pntruong
 * PR51930: Moved spread spectrum function to vdc.  Refactored duplicate
 * pll build rul.
 *
 * Hydra_Software_Devel/156   2/20/09 2:44p pntruong
 * PR52072: Allow adjustment of dvo rate manager offset.
 *
 * Hydra_Software_Devel/155   2/10/09 5:46p pntruong
 * PR51976: [GDVR-BR] Frame Skip is happened at Analog RF.  Also used
 * vertical frequency as another condition for detecting new custom ucode
 * loaded.
 *
 * Hydra_Software_Devel/154   1/5/09 8:11p darnstein
 * PR45737: If a chipset is not capable of 1080P@60Hz output, then it is
 * not capable of 1080P@50Hz output either.
 *
 * Hydra_Software_Devel/153   11/26/08 4:02p yuxiaz
 * PR47495: Enable DVI_CSC Dithering for 8 bit Outputs
 *
 * Hydra_Software_Devel/152   11/19/08 6:09p pntruong
 * PR49412: Coverity Defect ID:11584 DEADCODE bvdc_display.c.
 *
 * Hydra_Software_Devel/151   11/19/08 5:37p pntruong
 * PR48776: Need to initialize the new refresh rate on new format to by
 * used by applychanges.
 *
 * Hydra_Software_Devel/150   10/31/08 11:54a pntruong
 * PR48023: Don't need to set dirty for get.
 *
 * Hydra_Software_Devel/149   10/27/08 11:55p tdo
 * PR46484: Clean up VEC code
 *
 * Hydra_Software_Devel/148   10/24/08 1:17p albertl
 * PR48023: Fixed missing initialization value for  dvo rgb attenuation
 * causing purple dvo output.
 *
 * Hydra_Software_Devel/147   10/24/08 10:44a pntruong
 * PR48023: Temporary bypassed rgb attenuation adjustment for further
 * investigation of operation.
 *
 * Hydra_Software_Devel/146   10/24/08 2:45a albertl
 * PR48023:  Added BVDC_Display_SetDvoAttenuationRGB and
 * BVDC_Display_GetDvoAttenuationRGB, enabling CSC adjustment on DVO
 * path.  Further cleaned up CSC matrix multiplication code.
 *
 * Hydra_Software_Devel/145   10/22/08 6:12p pntruong
 * PR47816: Make sure the dvo configuration flags are used in PI.
 *
 * Hydra_Software_Devel/144   10/17/08 2:47p tdo
 * PR47744: remove duplicate code
 *
 * Hydra_Software_Devel/143   10/17/08 2:33p tdo
 * PR47744: Remove warnings and optimize data copying
 *
 * Hydra_Software_Devel/142   10/17/08 2:12p tdo
 * PR47744: Provide smooth updating of CCB table
 *
 * Hydra_Software_Devel/141   9/29/08 9:20p pntruong
 * PR45196: Fixed triggering of reloading ucode for custom timing.
 *
 * Hydra_Software_Devel/140   9/29/08 12:57p pntruong
 * PR47072: Moving 3563 vdc dedicated branch.
 *
 * Hydra_Software_Devel/139   9/10/08 7:51p pntruong
 * PR46546, PR46584: Dynamic power management of video DAC blocks.
 *
 * Hydra_Software_Devel/138   9/4/08 8:30p pntruong
 * PR46502: Moving 7403 vdc dedicated branch.
 *
 * Hydra_Software_Devel/137   8/8/08 4:09p yuxiaz
 * PR45484: More Dithering enable in VDC.
 *
 * Hydra_Software_Devel/136   8/8/08 3:28p yuxiaz
 * PR45484: Enable Dithering in VDC.
 *
 * Hydra_Software_Devel/135   7/2/08 6:25p rpan
 * PR43611: Allow setting VDEC bypass DAC to be unused.
 *
 * Hydra_Software_Devel/134   6/25/08 4:00p rpan
 * PR42486: Configuring DAC for VDEC->DAC pass-thru is allowed for a
 * bypass display handle.
 *
 * Hydra_Software_Devel/133   6/11/08 1:39p yuxiaz
 * PR39158: More dither settings in VEC.
 *
 * Hydra_Software_Devel/132   6/9/08 9:46p tdo
 * PR43462: Support CCB for 3549/3556
 *
 * Hydra_Software_Devel/131   5/27/08 7:04p hongtaoz
 * PR42248, PR42494: added display alignment API;
 *
 * Hydra_Software_Devel/130   5/1/08 2:38p rpan
 * PR42109: Initial work for SECAM variants.
 *
 * Hydra_Software_Devel/129   4/24/08 2:45p hongtaoz
 * PR32130: optimized RM adjustment for game mode support;
 *
 * Hydra_Software_Devel/128   3/11/08 10:45p pntruong
 * PR40280: Added support for vdec PAL-60.
 *
 * Hydra_Software_Devel/PR40133/1   3/11/08 10:11a mlei
 * PR40280: need PAL-60 support on 3563
 *
 * Hydra_Software_Devel/127   2/20/08 4:11p rpan
 * PR39421: BCM3548 DVI color converter block programming.
 *
 * Hydra_Software_Devel/126   2/11/08 8:43p pntruong
 * PR39177: [VEC] Need new display api to specify 60.00Hz or 59.94Hz
 * (framedrop).
 *
 * Hydra_Software_Devel/125   1/13/08 3:28a albertl
 * PR38150: Added 7440 HDMI xvYCC CSC support.
 *
 * Hydra_Software_Devel/124   11/1/07 10:00p pntruong
 * PR34239: Synchronized format information prior to validate of source
 * and compositor.
 *
 * Hydra_Software_Devel/123   10/31/07 3:06p pntruong
 * PR28459: Need heap added to VDC's display or compositor.
 *
 * Hydra_Software_Devel/122   10/30/07 7:12p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/121   10/22/07 3:09p pntruong
 * PR36351: BVDC_Display_SetVideoFormat() unnecessarily restrictive.
 *
 * Hydra_Software_Devel/120   10/15/07 3:14p pntruong
 * PR35137: Add NTSC443 support for cvbs/Svideo.
 *
 * Hydra_Software_Devel/119   10/11/07 2:06p albertl
 * PR35135:  Fixed user csc macros and added 10-bit/8-bit video data
 * conversion.
 *
 * Hydra_Software_Devel/118   10/10/07 9:01p pntruong
 * PR35896: DVO output setup for PAL/SECAM SD.
 *
 * Hydra_Software_Devel/117   10/2/07 11:22a pntruong
 * PR35136: Some cleanup of unused code.
 *
 * Hydra_Software_Devel/116   9/20/07 1:14a albertl
 * PR35135:  Cleaned up color space matrices and changed them to use same
 * macro system.  Added color space conversion functionality to graphics
 * windows.
 *
 * Hydra_Software_Devel/115   6/28/07 7:25p hongtaoz
 * PR32130, PR29189: added buffer game mode delay control; fixed picture
 * timestamp update bug when isr reorder occurs; invalid timestamp/drift
 * delay should disappear now; lipsync delay reflects the correct game
 * mode effect;
 *
 * Hydra_Software_Devel/VDC_Game_Mode/1   6/26/07 2:21p hongtaoz
 * PR32130: added window game mode buffer delay control;
 *
 * Hydra_Software_Devel/114   4/9/07 11:24a pntruong
 * PR28266: Provide API to customize DVO CSC.
 *
 * Hydra_Software_Devel/113   3/27/07 5:35p pntruong
 * PR28836: API to control H/V-Sync polarity control of DVO independently.
 *
 * Hydra_Software_Devel/112   3/26/07 11:03p tdo
 * PR 28402: Provide API to load customized CCB table for whitebalance
 *
 * Hydra_Software_Devel/111   3/9/07 10:50a maivu
 * PR 26051: Added proper restrictions for Modular vec with cross bar
 * support.
 *
 * Hydra_Software_Devel/110   2/6/07 3:26p tdo
 * PR 27576: Fix typo in debug statement
 *
 * Hydra_Software_Devel/109   2/6/07 3:21p tdo
 * PR 27576: Move CAB table and DVI_CCB table program out of critical
 * section
 *
 * Hydra_Software_Devel/108   1/22/07 8:48p tdo
 * PR 24457:  Avoid making WB commit change if input is invalid
 *
 * Hydra_Software_Devel/107   1/11/07 4:29p yuxiaz
 * PR 27067: Remove old checking in BVDC_Display_SetVideoFormat.
 *
 * Hydra_Software_Devel/106   12/21/06 7:12p hongtaoz
 * PR25668, PR22768: added API access of RDC scratch registers;
 *
 * Hydra_Software_Devel/105   12/21/06 1:08p hongtaoz
 * PR25668: h/w changed RDC scratch registers layout; bring up 7403;
 *
 * Hydra_Software_Devel/104   12/18/06 11:25p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/12   12/14/06 1:58p pntruong
 * PR26322: For modular VEC, BVDC needs to disclose choice of VEC to BVBI.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/11   11/29/06 5:23p yuxiaz
 * PR26136: Added BVDC_Display_GetDisplayId to get display Id.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/10   11/3/06 4:13p pntruong
 * PR25113: Show blue screen on HDMI without affecting component output
 * when HDCP fails.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/9   11/1/06 2:29p tdo
 * PR 24457:  Add color temp to white balance support for DVO output
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/8   10/26/06 5:12p hongtaoz
 * PR24995: merge to 3563 branch;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/7   10/23/06 4:29p hongtaoz
 * PR23280: add custom format support for dvo master mode;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/6   10/23/06 2:33p tdo
 * PR 24457: Add white balance support
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/5   10/19/06 6:59p hongtaoz
 * PR23260: add dvo master mode support; add support for cmp/display
 * connection at create time; restructured display code to be scalable
 * for 7400B support;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/4   10/9/06 4:26p maivu
 * PR 23173: Added BVDC_Display_SetCallbackSettings/GetCallbackSettings
 * support
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   9/19/06 10:35p pntruong
 * PR23222, PR23225: Fixed compile errors for heap based created.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   9/19/06 4:35p pntruong
 * PR23222, PR23225: VDEC - ATSC & PC input support.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   9/19/06 1:46p pntruong
 * PR23222, PR23225: VDEC - ATSC & PC input support.
 *
 * Hydra_Software_Devel/100   9/13/06 2:14p jessem
 * PR 24229: Added checks for buffer availability in BVDC_Set_VideoFormat.
 *
 * Hydra_Software_Devel/99   9/6/06 4:28p hongtaoz
 * PR22568, PR23188: add 1080p format for 3563;
 *
 * Hydra_Software_Devel/98   8/29/06 11:01a hongtaoz
 * PR23246: add new QDAC support for 3563; centralize some chip-specific
 * display macroes into bvdc_common_priv.h;
 *
 * Hydra_Software_Devel/97   8/14/06 7:56p pntruong
 * PR23177: RDC module bringup.  More cleanup of nested ifdefs.
 *
 * Hydra_Software_Devel/96   8/7/06 5:28p pntruong
 * PR22577: Initial bringup of VDC.
 *
 * Hydra_Software_Devel/95   6/30/06 5:46p hongtaoz
 * PR22081, PR20395, PR22062, PR22246: added SECAM Macrovision support;
 * fixed PAL_M dvi microcode; fixed PAL_M sub-carrier frequency setting;
 * PAL_NC should use the same microcode as normal PAL; fixed SECAM and
 * various PAL's csc matrices according to BT.470-6 spec; changed SRC
 * filter setting to beneifit higher freq response for HD output on
 * PRIM_VEC; fixed PAL sync tip setting for non-Macrovision build;
 *
 * Hydra_Software_Devel/94   6/14/06 1:35p syang
 * PR 21689: add support for 7118
 *
 * Hydra_Software_Devel/93   6/7/06 7:15p hongtaoz
 * PR21955, PR21541: 7401B0 SEC_VEC_SM needs to adjust delay to bringup
 * CVBS output on sec_vec; initial bringup SECAM, no color yet;
 *
 * Hydra_Software_Devel/92   5/26/06 3:35p pntruong
 * PR21605: Possible NULL pointer deref in bvdc_window.c.
 *
 * Hydra_Software_Devel/91   4/20/06 5:51p pntruong
 * PR20750: Please update bavc.h to reflect presence of third VEC.
 *
 * Hydra_Software_Devel/90   4/5/06 2:57p yuxiaz
 * PR20344: More work for 2HD buffer support: modify BVDC_Settings
 * : remove bUseHDBuffer4SD, change bNtscOnly_SD to eBufferFormat_SD.
 *
 * Hydra_Software_Devel/89   4/4/06 1:47p hongtaoz
 * PR20403: add triple displays support for 7400;
 *
 * Hydra_Software_Devel/88   3/28/06 1:23p yuxiaz
 * PR20344: Added double HD buffer support in VDC.
 *
 * Hydra_Software_Devel/87   2/21/06 4:26p yuxiaz
 * PR19258: Need to increase memory for 1366x768 case. Added
 * eBufferFormat_HD in BVDC_Settings.
 *
 * Hydra_Software_Devel/86   2/2/06 12:12p hongtaoz
 * PR19082: bring up 7400;
 *
 * Hydra_Software_Devel/85   1/25/06 10:57p pntruong
 * PR19172: Black-screen with latest VDC on 97398.  The mosaic added 4
 * more slots for capture compositing surface  this pushes the hddvi slot
 * out, and not all slot has track execution.  This causes the update
 * format rul of hddvi not properly execute and cause hddvi not to lock.
 * Fixes by freeing non-used frame slot in vdec/656/hddvi, and free up
 * more rdc vars to be used for track execution.
 *
 * Hydra_Software_Devel/84   1/17/06 4:17p hongtaoz
 * PR19082: first compiled for 7400;
 *
 * Hydra_Software_Devel/83   11/29/05 2:52p yuxiaz
 * PR18295: Fixed timebase selection in VDC.
 *
 * Hydra_Software_Devel/82   10/12/05 10:56a pntruong
 * PR10547, PR11588, PR12856, PR14875, PR15241:  Fixed uninstall callback.
 * Need to allow null pointer since we're using this function to disable
 * callback.
 *
 * Hydra_Software_Devel/81   10/6/05 4:16p pntruong
 * PR10547, PR11588, PR12856, PR14875, PR15241:  Added generic source
 * callback that return the structure pointer with all the requested
 * information from the above PRs.
 *
 * Hydra_Software_Devel/80   9/21/05 6:03p albertl
 * PR16459:  Display format code restructured, moved to
 * bvdc_displayfmt_priv.c and bvdc_displayfmt.h, and changed to be  table
 * driven  to allow ease of adding new formats.
 *
 * Hydra_Software_Devel/79   9/21/05 5:11p hongtaoz
 * PR15245: added error checking for bypass display and window;
 *
 * Hydra_Software_Devel/78   9/21/05 10:28a pntruong
 * PR17153:  Fixed 7401 build error.
 *
 * Hydra_Software_Devel/77   9/20/05 4:39p pntruong
 * PR17153: Request to implement a full SCART solution for European
 * market. Added flag to control vec swap.
 *
 * Hydra_Software_Devel/76   8/18/05 1:14p pntruong
 * PR15757, PR16391, PR16411, PR12519, PR14791, PR15535, PR15206, PR15778:
 * Improved bandwidth for cropping/scaler/capture/playback.   Unified
 * window shutdown sequence for destroy, reconfigure mad/scaler, and/or
 * reconfigure result of source changes.  And miscellances fixes from
 * above PRs.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/1   8/16/05 2:01p pntruong
 * PR12519: Take in additional fixes from mainline.
 *
 * Hydra_Software_Devel/75   8/10/05 6:28p syang
 * PR 12132, PR 14862, PR 15146: half coding checkin for adding src and
 * dst aspect ratio canvas control
 *
 * Hydra_Software_Devel/74   7/22/05 3:33p hongtaoz
 * PR15630, PR16218: removed rdb from bavc.h;
 *
 * Hydra_Software_Devel/73   7/18/05 6:11p hongtaoz
 * PR15630, PR16218: sychronize VBI encoder control registers programming
 * through scratch registers between BVDC and BVBI;
 *
 * Hydra_Software_Devel/72   6/28/05 11:02a jasonh
 * PR 16012: Adding support for 7401
 *
 * Hydra_Software_Devel/71   6/13/05 4:28p albertl
 * PR7662:  Code for setting bypass compositor state to NotActive in
 * BVDC_Display_Set656Configuration moved to bvdc_display.c.
 *
 * Hydra_Software_Devel/70   5/3/05 2:50p hongtaoz
 * PR14976: added full range RGB coloremitry outputs support for DVI
 * monitor; fixed DVI response to the source dynamic color space change;
 *
 * Hydra_Software_Devel/69   4/14/05 10:23a syang
 * PR 14692: re-organize picture's rectangles, fixed duplicated clips in
 * diff modules, changed to do clip as soon as possible, and added
 * capture clipping for C0
 *
 * Hydra_Software_Devel/68   3/17/05 6:36p pntruong
 * PR14494: Add preliminary software support to 3560 A0.
 *
 * Hydra_Software_Devel/67   3/7/05 2:01p albertl
 * PR13274:  Moved interlaced compositor line drop conversion to
 * bvdc_display_priv.c.
 *
 * Hydra_Software_Devel/66   3/4/05 2:06p albertl
 * PR13274:  Added BVDC_Display_SetHdmiDropLines and
 * BVDC_Display_GetHdmiDropLines for setting the number of compositor
 * lines to drop for hdmi output.
 *
 * Hydra_Software_Devel/65   2/4/05 4:55p hongtaoz
 * PR13760: allow PAL 656 bypass output;
 *
 * Hydra_Software_Devel/64   12/20/04 2:11p darnstein
 * PR 13506: Adapt to change BAVC_AspectRatio->BFMT_AspectRatio.
 *
 ***************************************************************************/
#include "bstd.h"          /* standard types */
#include "bkni.h"          /* For malloc */

#include "bvdc.h"
#include "bvdc_priv.h"
#include "bvdc_display_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bvdc_compositor_priv.h"

BDBG_MODULE(BVDC_DISP);

/*************************************************************************
 *	BVDC_Display_GetDefaultSettings
 *
 *************************************************************************/
BERR_Code BVDC_Display_GetDefaultSettings
	( BVDC_DisplayId                   eDisplayId,
	  BVDC_Display_Settings           *pDefSettings )
{
	/* Clear out first */
	BKNI_Memset(pDefSettings, 0, sizeof(BVDC_Display_Settings));

	if(BVDC_DisplayId_eAuto == eDisplayId)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

#if !BVDC_P_SUPPORT_TER_VEC
	if(BVDC_DisplayId_eDisplay2 == eDisplayId)
	{
		pDefSettings->eMasterTg = BVDC_DisplayTg_e656Dtg;
	}
	else
#endif
	{
		pDefSettings->eMasterTg = (BVDC_DisplayTg)eDisplayId;
	}

	/* Heap NULL heap means using default main-heap.  If user would like
	 * to use a separate heap then this field should set accordingly */
	pDefSettings->hHeap = NULL;

	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_Create
 *
 *	Creates a display handle. Assumes that it has not been created yet.
 *	Unlike the compositor handle, we do not know which display it is
 *	associated with until the user selects the video format.
 *
 *************************************************************************/
BERR_Code BVDC_Display_Create
(
	BVDC_Compositor_Handle           hCompositor,
	BVDC_Display_Handle             *phDisplay,
	BVDC_DisplayId                   eDisplayId,
	const BVDC_Display_Settings     *pDefSettings
)
{
	BVDC_Display_Handle hDisplay;
	BVDC_DisplayId eId;

	BDBG_ENTER(BVDC_Display_Create);
	BDBG_ASSERT(phDisplay);

	BSTD_UNUSED(pDefSettings);

	/* Check internally if we created. */
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);
	BDBG_OBJECT_ASSERT(hCompositor->hVdc, BVDC_VDC);

	/* determine display ID:
	   - display 0 is prim_vec path;
	   - display 1 is sec_vec path;
	   - display 2 is bypass path; must be driven by either
	     656 or dvo master tg; or 7400 can be driven by tert_vec; */
	if(BVDC_DisplayId_eAuto == eDisplayId)
	{
#if BVDC_P_SUPPORT_SEC_VEC
		if(BVDC_CompositorId_eCompositor2 > hCompositor->eId)
		{
			if(hCompositor->hVdc->bSwapVec)
			{
				eId = (BVDC_CompositorId_eCompositor0 == hCompositor->eId)?
					BVDC_DisplayId_eDisplay1 : BVDC_DisplayId_eDisplay0;
			}
			else
			{
				eId = (BVDC_DisplayId)hCompositor->eId;
			}
		}
		else
#endif
		{
			eId = (BVDC_DisplayId)hCompositor->eId;
		}
	}
	else
	{
		eId = eDisplayId;
	}

#if !BVDC_P_SUPPORT_SEC_VEC
	if(BVDC_DisplayId_eDisplay1 == eId)
	{
		BDBG_ERR(("This chipset doesn't support display 1!"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
#endif

#if !BVDC_P_SUPPORT_TER_VEC && !BVDC_P_SUPPORT_VBI_ENC_656
	if(BVDC_DisplayId_eDisplay2 == eId)
	{
		BDBG_ERR(("This chipset doesn't support display 2!"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
#endif

	hDisplay = hCompositor->hVdc->ahDisplay[eId];
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* Check if display is created or not. */
	if(BVDC_P_STATE_IS_ACTIVE(hDisplay) ||
	   BVDC_P_STATE_IS_CREATE(hDisplay) ||
	   BVDC_P_STATE_IS_DESTROY(hDisplay))
	{
		return BERR_TRACE(BVDC_ERR_DISPLAY_ALREADY_CREATED);
	}

	BDBG_MSG(("Creating display%d", hDisplay->eId));
	BDBG_ASSERT(BVDC_P_STATE_IS_INACTIVE(hDisplay));

	/* determine the master timing generator */
	if(pDefSettings)
	{
		if(hDisplay->bIsBypass &&
		   (BVDC_DisplayTg_eDviDtg != pDefSettings->eMasterTg) &&
		   (BVDC_DisplayTg_e656Dtg != pDefSettings->eMasterTg))
		{
			/* bypass display cannot be driven by analog TG */
			BDBG_ERR(("Bypass display has to run 656/DVO timing generator in master mode!"));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
#if !BVDC_P_SUPPORT_SEC_VEC
		if(BVDC_DisplayTg_eSecIt == pDefSettings->eMasterTg)
		{
			BDBG_ERR(("This chipset doesn't support SEC timing generator!"));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
#endif

#if !BVDC_P_SUPPORT_TER_VEC
		if(BVDC_DisplayTg_eTertIt == pDefSettings->eMasterTg)
		{
			BDBG_ERR(("This chipset doesn't support TERT timing generator!"));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
#endif

#if !BVDC_P_SUPPORT_656_MASTER_MODE
		if(BVDC_DisplayTg_e656Dtg == pDefSettings->eMasterTg)
		{
			BDBG_ERR(("This chipset doesn't support 656 timing generator in master mode!"));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
#endif

#if !BVDC_P_SUPPORT_DVO_MASTER_MODE
		if(BVDC_DisplayTg_eDviDtg == pDefSettings->eMasterTg)
		{
			BDBG_ERR(("This chipset doesn't support DVO timing generator in master mode!"));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
#endif

#if !BVDC_P_SUPPORT_VEC_MUXING
		if(!hDisplay->bIsBypass &&
		   ((BVDC_DisplayTg)eId != pDefSettings->eMasterTg))
		{
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
#endif
		hDisplay->eMasterTg = pDefSettings->eMasterTg;

		/* Select specific flavor of 480P output */
		hDisplay->bArib480p = pDefSettings->bArib480p;
	}
	else
	{
		/* legacy usage */
		if(hDisplay->bIsBypass)
		{
			hDisplay->eMasterTg = BVDC_DisplayTg_e656Dtg;
		}
		else
		{
			hDisplay->eMasterTg = (BVDC_DisplayTg)eId;
		}

		hDisplay->bArib480p = false;
	}

	/* assing triggers */
	switch(hDisplay->eMasterTg)
	{
		case BVDC_DisplayTg_eDviDtg:
			hDisplay->eTopTrigger = BRDC_Trigger_eDvoTrig0;
			hDisplay->eBotTrigger = BRDC_Trigger_eDvoTrig1;
			break;
		case BVDC_DisplayTg_e656Dtg:
			hDisplay->eTopTrigger = BRDC_Trigger_eDtgTrig0;
			hDisplay->eBotTrigger = BRDC_Trigger_eDtgTrig1;
			break;
		case BVDC_DisplayTg_ePrimIt:
			hDisplay->eTopTrigger = BRDC_Trigger_eVec0Trig0;
			hDisplay->eBotTrigger = BRDC_Trigger_eVec0Trig1;
			break;
		case BVDC_DisplayTg_eSecIt:
			hDisplay->eTopTrigger = BRDC_Trigger_eVec1Trig0;
			hDisplay->eBotTrigger = BRDC_Trigger_eVec1Trig1;
			break;
		case BVDC_DisplayTg_eTertIt:
			hDisplay->eTopTrigger = BRDC_Trigger_eVec2Trig0;
			hDisplay->eBotTrigger = BRDC_Trigger_eVec2Trig1;
			break;
		default:
			break;
	}

	/* TODO: other preliminary validations? */

	/* Which heap to use? */
	hDisplay->hHeap = ((pDefSettings) && (pDefSettings->hHeap))
		? pDefSettings->hHeap : hCompositor->hVdc->hBufferHeap;

	/* link compsoitor and display */
	hDisplay->hCompositor       = hCompositor;
	hCompositor->hDisplay       = hDisplay;

	/* Reinitialize context.  But not make it active until apply. */
	BVDC_P_Display_Init(hDisplay);

	/* Mark as create, awating for apply. */
	hDisplay->eState = BVDC_P_State_eCreate;
	*phDisplay = hDisplay;

	BDBG_LEAVE(BVDC_Display_Create);
	return BERR_SUCCESS;
}


/*************************************************************************
 *	BVDC_Display_Destroy
 *************************************************************************/
BERR_Code BVDC_Display_Destroy
(
	BVDC_Display_Handle              hDisplay
)
{
	BDBG_ENTER(BVDC_Display_Destroy);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(BVDC_P_STATE_IS_DESTROY(hDisplay) ||
	   BVDC_P_STATE_IS_INACTIVE(hDisplay))
	{
		goto done;
	}

	if(BVDC_P_STATE_IS_ACTIVE(hDisplay))
	{
		hDisplay->eState = BVDC_P_State_eDestroy;
	}

	if(BVDC_P_STATE_IS_CREATE(hDisplay))
	{
		hDisplay->eState = BVDC_P_State_eInactive;
	}

done:
	BDBG_LEAVE(BVDC_Display_Destroy);
	return BERR_SUCCESS;
}


/*************************************************************************
 *
 */
BERR_Code BVDC_Display_GetDisplayId
	( const BVDC_Display_Handle        hDisplay,
	  BVDC_DisplayId                  *peDisplayId )
{
	BDBG_ENTER(BVDC_Display_GetDisplayId);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(peDisplayId)
	{
		*peDisplayId = hDisplay->eId;
	}

	BDBG_LEAVE(BVDC_Display_GetDisplayId);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_GetVbiPath
 *************************************************************************/
BERR_Code BVDC_Display_GetVbiPath
(
	const BVDC_Display_Handle        hDisplay,
	BAVC_VbiPath                    *peVbiPath
)
{
	BDBG_ENTER(BVDC_Display_GetVbiPath);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(peVbiPath)
	{
		*peVbiPath = hDisplay->eVbiPath;
	}

	BDBG_LEAVE(BVDC_Display_GetVbiPath);
	return BERR_SUCCESS;
}


/*************************************************************************
 *	BVDC_Display_GetAnalogVbiPath
 *************************************************************************/
BERR_Code BVDC_Display_GetAnalogVbiPath
(
	const BVDC_Display_Handle        hDisplay,
	BAVC_VbiPath                    *peVbiPath
)
{
	BDBG_ENTER(BVDC_Display_GetAnalogVbiPath);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(peVbiPath)
	{
		*peVbiPath = (BAVC_VbiPath_eBypass0 == hDisplay->eVbiPath)
			? BAVC_VbiPath_eUnknown : hDisplay->eVbiPath;
	}

	BDBG_LEAVE(BVDC_Display_GetAnalogVbiPath);
	return BERR_SUCCESS;
}


/*************************************************************************
 *	BVDC_Display_SetCustomVideoFormat
 *
 *	Sets video format
 *************************************************************************/
BERR_Code BVDC_Display_SetCustomVideoFormat
(
	BVDC_Display_Handle              hDisplay,
	const BFMT_VideoInfo            *pFmtInfo
)
{
	BERR_Code eStatus = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Display_SetCustomVideoFormat);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* Valid parameters */
	if((NULL == pFmtInfo) ||
	   (pFmtInfo->eVideoFmt >= BFMT_VideoFmt_eMaxCount))
	{
		BDBG_ERR(("Invalid parameter"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(!BVDC_P_IsVidfmtSupported(pFmtInfo->eVideoFmt))
	{
		BDBG_ERR(("Format not supported: %s", pFmtInfo->pchFormatStr));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if((pFmtInfo->eVideoFmt == BFMT_VideoFmt_eCustom0) ||
	   (pFmtInfo->eVideoFmt == BFMT_VideoFmt_eCustom1) ||
	   (pFmtInfo->eVideoFmt == BFMT_VideoFmt_eCustom2))
	{
		const uint32_t *pTable = NULL;
		if(!pFmtInfo->pCustomInfo)
		{
			BDBG_ERR(("No custom formats info tables are provided in bfmt_custom.c!"));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
		if(!hDisplay->bIsBypass)
		{
			BDBG_ERR(("Custom format can only be set on the DVO master mode bypass display!"));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}

		/* Get check sum of custom ucode */
		pTable = pFmtInfo->pCustomInfo->pDvoMicrocodeTbl;
		hDisplay->stNewInfo.ulCheckSum = pTable[BVDC_P_DTRAM_TABLE_CHECKSUM_IDX];
	}
	else
	{
		hDisplay->stNewInfo.ulCheckSum = 0;
	}

	/* The following is now fixed in VDC, due to hw restrictions:
	 * Comp0(HD/SD) => Secondary(HD/SD)
	 * Comp1(SD)    => Primary(HD/SD).
	 *
	 * Note that Primary VEC can do HD or SD.  Though on the Primary VEC
	 * the analog output filter is not as good as Secondary VEC and using
	 * it to drive analog output (YPrPb) may see a less disirable output
	 * than using Seconardy vec's. */
	if(VIDEO_FORMAT_IS_HD(pFmtInfo->eVideoFmt) &&
	  (BVDC_CompositorId_eCompositor0 != hDisplay->hCompositor->eId))
	{
		return BVDC_ERR_INVALID_MODE_PATH;
	}

	/* Update new video format */
	hDisplay->stNewInfo.pFmtInfo = pFmtInfo;
	hDisplay->hCompositor->stNewInfo.pFmtInfo = pFmtInfo;
	hDisplay->stNewInfo.ulVertFreq = pFmtInfo->ulVertFreq;

	/* New timing? */
	if((pFmtInfo->eVideoFmt != hDisplay->stCurInfo.pFmtInfo->eVideoFmt) ||
	   (pFmtInfo->ulWidth  != hDisplay->stCurInfo.pFmtInfo->ulWidth) ||
	   (pFmtInfo->ulHeight != hDisplay->stCurInfo.pFmtInfo->ulHeight) ||
	   (hDisplay->stNewInfo.ulCheckSum != hDisplay->stCurInfo.ulCheckSum) ||
	   (pFmtInfo->ulVertFreq != hDisplay->stCurInfo.ulVertFreq))
	{
		hDisplay->stNewInfo.stDirty.stBits.bTiming = BVDC_P_DIRTY;
		hDisplay->stNewInfo.stDirty.stBits.bAspRatio = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_SetCustomVideoFormat);
	return eStatus;
}


/*************************************************************************
 *	BVDC_Display_GetCustomVideoFormat
 *************************************************************************/
const BFMT_VideoInfo* BVDC_Display_GetCustomVideoFormat
(
	const BVDC_Display_Handle        hDisplay
)
{
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	return hDisplay->stCurInfo.pFmtInfo;
}


/*************************************************************************
 *	BVDC_Display_SetVideoFormat
 *
 *	Sets video format
 *************************************************************************/
BERR_Code BVDC_Display_SetVideoFormat
(
	BVDC_Display_Handle              hDisplay,
	BFMT_VideoFmt                    eVideoFormat
)
{
	return BVDC_Display_SetCustomVideoFormat(hDisplay,
		BFMT_GetVideoFormatInfoPtr(eVideoFormat));
}


/*************************************************************************
 *	BVDC_Display_GetVideoFormat
 *************************************************************************/
BERR_Code BVDC_Display_GetVideoFormat
(
	const BVDC_Display_Handle        hDisplay,
	BFMT_VideoFmt                   *peVideoFormat
)
{
	BDBG_ENTER(BVDC_Display_GetVideoFormat);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(peVideoFormat)
	{
		*peVideoFormat = hDisplay->stCurInfo.pFmtInfo->eVideoFmt;
	}

	BDBG_LEAVE(BVDC_Display_GetVideoFormat);
	return BERR_SUCCESS;
}

/*************************************************************************
 * BVDC_Display_SetDacConfiguration
 *
 * Enable Dac(s) to specific output, or disable Dac(s). Use the following
 *  defines to identify specific Dacs:
 * BVDC_Dac_0
 * BVDC_Dac_1
 * BVDC_Dac_2
 * BVDC_Dac_3
 * BVDC_Dac_4
 * BVDC_Dac_5
 *
 *  A DAC can only be used by one display at a time. A display is not using
 *  a specifc DAC if its corresponding state is BVDC_DacOutput_eUnused.
 *  A DAC that is unused by all displays is disabled. Initially, all DACs
 *  are unused for all displays.
 *************************************************************************/
BERR_Code  BVDC_Display_SetDacConfiguration
(
    BVDC_Display_Handle              hDisplay,
    uint32_t                         ulDacs,
    BVDC_DacOutput                   eDacOutput
)
{
	BVDC_P_DisplayInfo *pNewInfo;
	BVDC_P_DisplayInfo *pCurInfo;
	uint32_t            i;

	BDBG_ENTER(BVDC_Display_SetDacConfiguration);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(ulDacs > BVDC_Dac_0+BVDC_Dac_1+BVDC_Dac_2+BVDC_Dac_3+BVDC_Dac_4+BVDC_Dac_5+BVDC_Dac_6)
	{
		BDBG_ERR(("Invalid parameter"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	pNewInfo = &hDisplay->stNewInfo;
	pCurInfo = &hDisplay->stCurInfo;

	/* Other than VDEC pass-through, bypass path does not support Dac outputs */
	if((hDisplay->bIsBypass) &&
	   (eDacOutput != BVDC_DacOutput_eVdec0) &&
	   (eDacOutput != BVDC_DacOutput_eIfdm0) &&
	   (eDacOutput != BVDC_DacOutput_eIfdm1) &&
	   (eDacOutput != BVDC_DacOutput_eUnused))
	{
		return BVDC_ERR_INVALID_DAC_SETTINGS;
	}

	/* Update new outputs for each Dac */
	for(i=0; i < BVDC_P_MAX_DACS; i++)
	{
		if((ulDacs >> i) & 1)
		{
			pNewInfo->aDacOutput[i] = eDacOutput;
		}

		if(pNewInfo->aDacOutput[i] != pCurInfo->aDacOutput[i])
		{
			pNewInfo->stDirty.stBits.bDacSetting = BVDC_P_DIRTY;
		}
	}

	BDBG_LEAVE(BVDC_Display_SetDacConfiguration);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_GetDacConfiguration
 *
 *	Returns a specific Dac output setting.
 *************************************************************************/
BERR_Code BVDC_Display_GetDacConfiguration
(
	const BVDC_Display_Handle        hDisplay,
	uint32_t                         ulDac,
	BVDC_DacOutput                  *peDacOutput
)
{
	uint32_t     i;

	BDBG_ENTER(BVDC_Display_GetDacConfiguration);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if((!peDacOutput) || (ulDac > (BVDC_Dac_6 << 1)))
	{
		BDBG_ERR(("Invalid parameter"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Get index to Dac table */
	for (i=0; i < BVDC_P_MAX_DACS; i++)
	{
		if((ulDac >> i) & 1)
		{
			*peDacOutput = hDisplay->stCurInfo.aDacOutput[i];
			break;
		}
	}

	BDBG_LEAVE(BVDC_Display_GetDacConfiguration);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_SetHdmiConfiguration
 *************************************************************************/
BERR_Code BVDC_Display_SetHdmiConfiguration
(
	BVDC_Display_Handle              hDisplay,
	uint32_t                         ulHdmi,
	BAVC_MatrixCoefficients          eHdmiOutput
)
{
	BDBG_ENTER(BVDC_Display_SetHdmiConfiguration);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* Check for valid parameters */
	if(ulHdmi != BVDC_Hdmi_0)
	{
		BDBG_ERR(("Display handle invalid"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Bypass path does not support HDMI */
#if !BVDC_P_SUPPORT_DVO_MASTER_MODE
	if(hDisplay->bIsBypass)
	{
		return BERR_TRACE(BVDC_ERR_INVALID_HDMI_PATH);
	}
#endif

	switch(eHdmiOutput)
	{
		case BAVC_MatrixCoefficients_eUnknown:
			hDisplay->stNewInfo.eHdmiOutput = eHdmiOutput;
			hDisplay->stNewInfo.bEnableHdmi = false;
			break;

		default:
			hDisplay->stNewInfo.eHdmiOutput = eHdmiOutput;
			hDisplay->stNewInfo.bEnableHdmi = true;
			BDBG_MSG(("Display %d enables HDMI", hDisplay->eId));
			break;
	}

	/* Dirty bit to re-program HDMI color matrix. */
	hDisplay->stNewInfo.stDirty.stBits.bCscAdjust = BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_Display_SetHdmiConfiguration);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_GetHdmiConfiguration
 *
 *	Query the HDMI output for a specific HDMI
 *************************************************************************/
BERR_Code BVDC_Display_GetHdmiConfiguration
(
	const BVDC_Display_Handle        hDisplay,
	uint32_t                         ulHdmi,
	BAVC_MatrixCoefficients         *peHdmiOutput
)
{
	BDBG_ENTER(BVDC_Display_GetHdmiConfiguration);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(ulHdmi != BVDC_Hdmi_0)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(peHdmiOutput)
	{
		*peHdmiOutput = hDisplay->stCurInfo.eHdmiOutput;
	}

	BDBG_LEAVE(BVDC_Display_GetHdmiConfiguration);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_SetHdmiDropLines
 *
 *	Set the number of compositor video lines that Hdmi drops for a
 *  given format.
 *************************************************************************/
BERR_Code BVDC_Display_SetHdmiDropLines
(
	const BVDC_Display_Handle        hDisplay,
	uint32_t                         ulHdmi,
	BFMT_VideoFmt                    eVideoFormat,
	uint32_t                         ulHdmiDropLines
)
{
	BDBG_ENTER(BVDC_Display_SetHdmiDropLines);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(ulHdmi != BVDC_Hdmi_0)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(!VIDEO_FORMAT_SUPPORTS_DROP_LINE(eVideoFormat))
	{
		BDBG_ERR(("Dropping compositor lines is supported for this format!"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	switch(eVideoFormat)
	{
	case BFMT_VideoFmt_e720x482_NTSC:
	case BFMT_VideoFmt_e720x482_NTSC_J:
		if (ulHdmiDropLines > BVDC_P_480i_DROP_LINES_MAX)
		{
			BDBG_ERR(("Exceeds number of dropped lines supported by NTSC, NTSCJ."));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}

		if ((ulHdmiDropLines != 0) && (ulHdmiDropLines != 2))
		{
			BDBG_ERR(("NTSC, NTSCJ and PAL_M only support dropping of 0 or 2 lines."));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}

		break;

	case BFMT_VideoFmt_e720x483p:
		if (ulHdmiDropLines > BVDC_P_480p_DROP_LINES_MAX)
		{
			BDBG_ERR(("Exceeds number of dropped lines supported by 480p."));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
		break;

	default:
		/* Assert if format isn't handled yet. */
		BDBG_ASSERT(false);
		break;
	}

	hDisplay->stNewInfo.aulHdmiDropLines[eVideoFormat] = ulHdmiDropLines;

	BDBG_LEAVE(BVDC_Display_SetHdmiDropLines);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_GetHdmiDropLines
 *
 *	Get the number of compositor video lines that Hdmi drops for a given
 *  format.
 *************************************************************************/
BERR_Code BVDC_Display_GetHdmiDropLines
(
	const BVDC_Display_Handle        hDisplay,
	uint32_t                         ulHdmi,
	BFMT_VideoFmt                    eVideoFormat,
	uint32_t                        *pulHdmiDropLines
)
{
	BDBG_ENTER(BVDC_Display_SetHdmiDropLines);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(ulHdmi != BVDC_Hdmi_0)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if (pulHdmiDropLines)
	{
		*pulHdmiDropLines = hDisplay->stCurInfo.aulHdmiDropLines[eVideoFormat];
	}

	BDBG_LEAVE(BVDC_Display_SetHdmiDropLines);
	return BERR_SUCCESS;
}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_SetHdmiSyncOnly
(
	const BVDC_Display_Handle        hDisplay,
	bool                             bSyncOnly
)
{
	BDBG_ENTER(BVDC_Display_SetHdmiSyncOnly);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	BVDC_Display_SetMuteMode(hDisplay, BVDC_DisplayOutput_eDvo,
		                     bSyncOnly ? BVDC_MuteMode_eConst : BVDC_MuteMode_eDisable);
	hDisplay->stNewInfo.bSyncOnly = bSyncOnly;

	BDBG_LEAVE(BVDC_Display_SetHdmiSyncOnly);
	return BERR_SUCCESS;
}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_GetHdmiSyncOnly
(
	const BVDC_Display_Handle        hDisplay,
	bool                            *pbSyncOnly
)
{
	BDBG_ENTER(BVDC_Display_GetHdmiSyncOnly);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(pbSyncOnly)
	{
		*pbSyncOnly = hDisplay->stCurInfo.bSyncOnly;
	}

	BDBG_LEAVE(BVDC_Display_GetHdmiSyncOnly);
	return BERR_SUCCESS;
}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_SetHdmiXvYcc
(
	const BVDC_Display_Handle        hDisplay,
	bool                             bXvYcc
)
{
	BDBG_ENTER(BVDC_Display_SetHdmiXvYcc);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	hDisplay->stNewInfo.bXvYcc = bXvYcc;

	BDBG_LEAVE(BVDC_Display_SetHdmiXvYcc);
	return BERR_SUCCESS;
}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_GetHdmiXvYcc
(
	const BVDC_Display_Handle        hDisplay,
	bool                            *bXvYcc
)
{
	BDBG_ENTER(BVDC_Display_GetHdmiXvYcc);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(bXvYcc)
	{
		*bXvYcc = hDisplay->stCurInfo.bXvYcc;
	}

	BDBG_LEAVE(BVDC_Display_GetHdmiXvYcc);
	return BERR_SUCCESS;
}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_SetHdmiColorDepth
(
	const BVDC_Display_Handle        hDisplay,
	BAVC_HDMI_BitsPerPixel           eColorDepth
)
{
	BDBG_ENTER(BVDC_Display_SetHdmiColorDepth);

	BDBG_ERR(("Unsupported !!!"));
	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(eColorDepth);

	BDBG_LEAVE(BVDC_Display_SetHdmiColorDepth);
	return BERR_SUCCESS;
}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_GetHdmiColorDepth
(
	const BVDC_Display_Handle        hDisplay,
	BAVC_HDMI_BitsPerPixel          *eColorDepth
)
{
	BDBG_ENTER(BVDC_Display_GetHdmiColorDepth);

	BDBG_ERR(("Unsupported !!!"));
	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(eColorDepth);

	BDBG_LEAVE(BVDC_Display_GetHdmiColorDepth);
	return BERR_SUCCESS;
}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_SetHdmiPixelRepetition
(
	const BVDC_Display_Handle        hDisplay,
	BAVC_HDMI_PixelRepetition        ePixelRepetition
)
{
	BDBG_ENTER(BVDC_Display_SetHdmiPixelRepetition);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* Support is limited to 2x for 480P */
	if(ePixelRepetition != BAVC_HDMI_PixelRepetition_eNone &&
	   ePixelRepetition != BAVC_HDMI_PixelRepetition_e2x )
	{
		return BERR_TRACE(BVDC_ERR_INVALID_HDMI_MODE);
	}

	if(hDisplay->stCurInfo.eHdmiPixelRepetition != ePixelRepetition)
	{
		hDisplay->stNewInfo.eHdmiPixelRepetition = ePixelRepetition;
		hDisplay->stNewInfo.stDirty.stBits.bTiming = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_SetHdmiPixelRepetition);
	return BERR_SUCCESS;
}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_GetHdmiPixelRepetition
(
	const BVDC_Display_Handle        hDisplay,
	BAVC_HDMI_PixelRepetition       *ePixelRepetition
)
{
	BDBG_ENTER(BVDC_Display_GetHdmiPixelRepetition);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(ePixelRepetition)
	{
		*ePixelRepetition = hDisplay->stCurInfo.eHdmiPixelRepetition;
	}

	BDBG_LEAVE(BVDC_Display_GetHdmiPixelRepetition);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_Set656Configuration
 *************************************************************************/
BERR_Code BVDC_Display_Set656Configuration
(
	BVDC_Display_Handle              hDisplay,
	uint32_t                         ul656Output,
	bool                             bEnable
)
{
	BDBG_ENTER(BVDC_Display_Set656Configuration);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(ul656Output != BVDC_Itur656Output_0)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(hDisplay->bIsBypass && !bEnable &&
	   hDisplay->hCompositor->ahWindow[BVDC_P_WindowId_eComp2_V0] &&
	  !BVDC_P_STATE_IS_INACTIVE(hDisplay->hCompositor->ahWindow[BVDC_P_WindowId_eComp2_V0]))
	{
		/* Check to see if there are any active windows. */
		BDBG_ERR(("Bypass display cannot disable 656 output!"));
		BDBG_ERR(("Bypass window %d has not been destroyed, state is %d",
			  hDisplay->hCompositor->ahWindow[BVDC_P_WindowId_eComp2_V0]->eId,
			  hDisplay->hCompositor->ahWindow[BVDC_P_WindowId_eComp2_V0]->eState));
		return BERR_TRACE(BERR_LEAKED_RESOURCE);
	}

	hDisplay->stNewInfo.bEnable656 = bEnable;

	BDBG_LEAVE(BVDC_Display_Set656Configuration);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_Get656Configuration
 *************************************************************************/
BERR_Code BVDC_Display_Get656Configuration
(
	const BVDC_Display_Handle        hDisplay,
	uint32_t                         ul656Output,
	bool                             *pbEnable656Output
)
{
	BDBG_ENTER(BVDC_Display_Get656Configuration);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(ul656Output != BVDC_Itur656Output_0)
	{
		BDBG_ERR(("Invalid parameter"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(pbEnable656Output)
	{
		*pbEnable656Output = hDisplay->stCurInfo.bEnable656;
	}

	BDBG_LEAVE(BVDC_Display_Get656Configuration);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_SetStgConfiguration
 *************************************************************************/
	BERR_Code BVDC_Display_SetStgConfiguration
		( BVDC_Display_Handle			   hDisplay,
		  bool                             bEnable,
		  const BVDC_Display_StgSettings  *pStgSettings )

{
	BDBG_ENTER(BVDC_Display_SetStgConfiguration);

#if (BVDC_P_SUPPORT_STG == 0)

	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(bEnable);
	BSTD_UNUSED(pStgSettings);

	BDBG_ERR(("This chip does not support STG"));

	BDBG_LEAVE(BVDC_Display_SetStgConfiguration);

	return BERR_NOT_SUPPORTED;
#else

	/* Check for valid parameters */
	if(hDisplay ==NULL)
	{
		BDBG_LEAVE(BVDC_Display_SetStgConfiguration);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	hDisplay->stNewInfo.bEnableStg = bEnable;
	if(pStgSettings)
	{
		if(pStgSettings->bNonRealTime &&
		   hDisplay->eMasterTg != BVDC_DisplayTg_eStg)
		{
			BDBG_ERR(("Display %d eMasterTg=%d is not STG, cannot be non-realtime!",
				hDisplay->eId, hDisplay->eMasterTg));
			BDBG_LEAVE(BVDC_Display_SetStgConfiguration);
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}

		hDisplay->stNewInfo.bStgNonRealTime = pStgSettings->bNonRealTime;

		if((bEnable != hDisplay->stCurInfo.bEnableStg) ||
           (pStgSettings->bNonRealTime != hDisplay->stCurInfo.bStgNonRealTime))
		{
			hDisplay->stNewInfo.stDirty.stBits.bStgEnable = BVDC_P_DIRTY;
		}
	}

	BDBG_LEAVE(BVDC_Display_SetStgConfiguration);

	return BERR_SUCCESS;
#endif
}

/*************************************************************************
 *	BVDC_Display_GetStgConfiguration
 *************************************************************************/
	BERR_Code BVDC_Display_GetStgConfiguration
		( const BVDC_Display_Handle 	   hDisplay,
		  bool                            *pbEnable,
		  BVDC_Display_StgSettings		  *pStgSettings )
{
	BDBG_ENTER(BVDC_Display_GetStgConfiguration);

#if (BVDC_P_SUPPORT_STG == 0)
	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(pbEnable);
	BSTD_UNUSED(pStgSettings);

	BDBG_LEAVE(BVDC_Display_GetStgConfiguration);
	return BERR_NOT_SUPPORTED;

#else

	if(hDisplay == NULL)
	{
		BDBG_ERR(("Invalid parameter"));
		BDBG_LEAVE(BVDC_Display_GetStgConfiguration);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(pbEnable)
	{
		*pbEnable = hDisplay->stCurInfo.bEnableStg;
	}

	if(pStgSettings)
	{
		pStgSettings->bNonRealTime = hDisplay->stCurInfo.bStgNonRealTime;
	}

	BDBG_LEAVE(BVDC_Display_GetStgConfiguration);
	return BERR_SUCCESS;
#endif
}

/*************************************************************************
 *	BVDC_Display_GetInterruptName
 *************************************************************************/
BERR_Code BVDC_Display_GetInterruptName
	( BVDC_Display_Handle             hDisplay,
	  const BAVC_Polarity             eFieldId,
	  BINT_Id                        *pInterruptName )
{
	BDBG_ENTER(BVDC_Display_GetInterruptName);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	BDBG_OBJECT_ASSERT(hDisplay->hCompositor, BVDC_CMP);

	if(pInterruptName)
	{
		*pInterruptName = BRDC_Slot_GetIntId(
			hDisplay->hCompositor->ahSlot[eFieldId == BAVC_Polarity_eBotField]);
	}

	BDBG_LEAVE(BVDC_Display_GetInterruptName);
	return BERR_SUCCESS;
}


/*************************************************************************
 *	BVDC_Display_SetRfmConfiguration
 *************************************************************************/
BERR_Code BVDC_Display_SetRfmConfiguration
(
	BVDC_Display_Handle              hDisplay,
	uint32_t                         ulRfmOutput,
	BVDC_RfmOutput                   eRfmOutput,
	uint32_t                         ulConstantValue
)
{
	BDBG_ENTER(BVDC_Display_SetRfmConfiguration);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(ulRfmOutput != BVDC_Rfm_0)
	{
		BDBG_ERR(("Invalid parameter"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Bypass path does not support Rfm */
	if(hDisplay->bIsBypass)
	{
		return BERR_TRACE(BVDC_ERR_INVALID_RFM_PATH);
	}

	hDisplay->stNewInfo.eRfmOutput = eRfmOutput;

	hDisplay->stNewInfo.ulRfmConst =
		(eRfmOutput == BVDC_RfmOutput_eUnused) ? 0 : ulConstantValue;

	BDBG_LEAVE(BVDC_Display_SetRfmConfiguration);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_GetRfmConfiguration
 *************************************************************************/
BERR_Code BVDC_Display_GetRfmConfiguration
(
	const BVDC_Display_Handle        hDisplay,
	uint32_t                         ulRfmOutput,
	BVDC_RfmOutput                  *peRfmOutput,
	uint32_t                        *pulConstantValue
)
{
	BDBG_ENTER(BVDC_Display_GetRfmConfiguration);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(ulRfmOutput != BVDC_Rfm_0)
	{
		BDBG_ERR(("Invalid parameter"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(peRfmOutput)
	{
		*peRfmOutput = hDisplay->stCurInfo.eRfmOutput;
	}

	if(pulConstantValue)
	{
		*pulConstantValue = hDisplay->stCurInfo.ulRfmConst;
	}

	BDBG_LEAVE(BVDC_Display_GetRfmConfiguration);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_SetMpaa
 *************************************************************************/
BERR_Code BVDC_Display_SetMpaaDecimation
( 	BVDC_Display_Handle              hDisplay,
	BVDC_MpaaDeciIf                  eMpaaDeciIf,
	uint32_t                         ulOutPorts,
	bool                             bEnable
)
{
	BDBG_ENTER(BVDC_Display_SetMpaaDecimation);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* Bypass path does not support MPAA */
	if(hDisplay->bIsBypass)
	{
		return BERR_TRACE(BVDC_ERR_INVALID_MPAA_PATH);
	}

	if ((BVDC_MpaaDeciIf_eUnused <= eMpaaDeciIf) ||
		((~hDisplay->aulMpaaDeciIfPortMask[eMpaaDeciIf]) & ulOutPorts))
	{
		BDBG_ERR(("Invalid parameter"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if (bEnable)
	{
		hDisplay->stNewInfo.aulEnableMpaaDeci[eMpaaDeciIf] |= ulOutPorts;
	}
	else
	{
		hDisplay->stNewInfo.aulEnableMpaaDeci[eMpaaDeciIf] &= ~ulOutPorts;
	}

	BDBG_LEAVE(BVDC_Display_SetMpaaDecimation);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_GetMpaaDecimation
 *************************************************************************/
BERR_Code BVDC_Display_GetMpaaDecimation
(
	const BVDC_Display_Handle        hDisplay,
	BVDC_MpaaDeciIf                  eMpaaDeciIf,
	uint32_t                         ulOutPort,
	bool                            *pbEnable
)
{
	BDBG_ENTER(BVDC_Display_GetMpaaDecimation);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if ((BVDC_MpaaDeciIf_eUnused <= eMpaaDeciIf) ||
		((~hDisplay->aulMpaaDeciIfPortMask[eMpaaDeciIf]) & ulOutPort) ||
		(NULL == pbEnable))
	{
		BDBG_ERR(("Invalid parameter"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	*pbEnable = hDisplay->stCurInfo.aulEnableMpaaDeci[eMpaaDeciIf] & ulOutPort;

	BDBG_LEAVE(BVDC_Display_GetMpaaDecimation);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_SetAspectRatio
 *************************************************************************/
BERR_Code BVDC_Display_SetAspectRatio
(
	BVDC_Display_Handle              hDisplay,
	BFMT_AspectRatio                 eAspectRatio
)
{
	BDBG_ENTER(BVDC_Display_SetAspectRatio);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	BDBG_OBJECT_ASSERT(hDisplay->hCompositor, BVDC_CMP);

	if(BVDC_P_IS_UNKNOWN_ASPR(eAspectRatio, 0, 0))
	{
		BDBG_WRN(("Invalid aspect ratio: %u", eAspectRatio));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if (hDisplay->stCurInfo.eAspectRatio != eAspectRatio)
	{
		hDisplay->stNewInfo.eAspectRatio = eAspectRatio;
		hDisplay->stNewInfo.stDirty.stBits.bAspRatio = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_SetAspectRatio);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_SetSampleAspectRatio
 *************************************************************************/
BERR_Code BVDC_Display_GetSampleAspectRatio
	( BVDC_Display_Handle              hDisplay,
	  uint16_t                        *puiSampleAspectRatioX,
	  uint16_t                        *puiSampleAspectRatioY )
{
	BDBG_ENTER(BVDC_Display_GetSampleAspectRatio);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	BDBG_OBJECT_ASSERT(hDisplay->hCompositor, BVDC_CMP);

	if(puiSampleAspectRatioX)
	{
		*puiSampleAspectRatioX = hDisplay->stCurInfo.uiSampleAspectRatioX;
	}

	if(puiSampleAspectRatioY)
	{
		*puiSampleAspectRatioY = hDisplay->stCurInfo.uiSampleAspectRatioY;
	}

	BDBG_LEAVE(BVDC_Display_GetSampleAspectRatio);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_SetSampleAspectRatio
 *************************************************************************/
BERR_Code BVDC_Display_SetSampleAspectRatio
	( BVDC_Display_Handle              hDisplay,
	  uint16_t                         uiSampleAspectRatioX,
	  uint16_t                         uiSampleAspectRatioY )
{
	BDBG_ENTER(BVDC_Display_SetSampleAspectRatio);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	BDBG_OBJECT_ASSERT(hDisplay->hCompositor, BVDC_CMP);

	if(BVDC_P_IS_UNKNOWN_ASPR(BFMT_AspectRatio_eSAR,
	   uiSampleAspectRatioX, uiSampleAspectRatioY))
	{
		BDBG_WRN(("Invalid sample aspect ratio: %u %u",
			uiSampleAspectRatioX, uiSampleAspectRatioY));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if ((hDisplay->stCurInfo.eAspectRatio != BFMT_AspectRatio_eSAR) ||
		(hDisplay->stCurInfo.uiSampleAspectRatioX != uiSampleAspectRatioX) ||
		(hDisplay->stCurInfo.uiSampleAspectRatioY != uiSampleAspectRatioY))
	{
		hDisplay->stNewInfo.eAspectRatio = BFMT_AspectRatio_eSAR;
		hDisplay->stNewInfo.uiSampleAspectRatioX = uiSampleAspectRatioX;
		hDisplay->stNewInfo.uiSampleAspectRatioY = uiSampleAspectRatioY;
		hDisplay->stNewInfo.stDirty.stBits.bAspRatio = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_SetSampleAspectRatio);
	return BERR_SUCCESS;
}


/*************************************************************************
 *	BVDC_Display_SetAspectRatioRectangle
 *************************************************************************/
BERR_Code BVDC_Display_SetAspectRatioCanvasClip
(
	BVDC_Display_Handle              hDisplay,
	  uint32_t                       ulLeft,
	  uint32_t                       ulRight,
	  uint32_t                       ulTop,
	  uint32_t                       ulBottom )
{
	BDBG_ENTER(BVDC_Display_SetAspectRatioCanvasClip);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	BDBG_OBJECT_ASSERT(hDisplay->hCompositor, BVDC_CMP);

	if ((hDisplay->stCurInfo.stAspRatRectClip.ulLeft   != ulLeft) ||
		(hDisplay->stCurInfo.stAspRatRectClip.ulRight  != ulRight) ||
		(hDisplay->stCurInfo.stAspRatRectClip.ulTop    != ulTop) ||
		(hDisplay->stCurInfo.stAspRatRectClip.ulBottom != ulBottom))
	{
		hDisplay->stNewInfo.stAspRatRectClip.ulLeft   = ulLeft;
		hDisplay->stNewInfo.stAspRatRectClip.ulRight  = ulRight;
		hDisplay->stNewInfo.stAspRatRectClip.ulTop    = ulTop;
		hDisplay->stNewInfo.stAspRatRectClip.ulBottom = ulBottom;

		hDisplay->stNewInfo.stDirty.stBits.bAspRatio = BVDC_P_DIRTY;
	}


	BDBG_LEAVE(BVDC_Display_SetAspectRatioCanvasClip);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_GetAspectRatio
 *************************************************************************/
BERR_Code BVDC_Display_GetAspectRatio
(
	const BVDC_Display_Handle        hDisplay,
	BFMT_AspectRatio                *peAspectRatio
)
{
	BDBG_ENTER(BVDC_Display_GetAspectRatio);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	BDBG_OBJECT_ASSERT(hDisplay->hCompositor, BVDC_CMP);

	if(peAspectRatio)
	{
		*peAspectRatio = hDisplay->stCurInfo.eAspectRatio;
	}

	BDBG_LEAVE(BVDC_Display_GetAspectRatio);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_SetForceFrameDrop
 *************************************************************************/
BERR_Code BVDC_Display_SetForceFrameDrop
	( BVDC_Display_Handle              hDisplay,
	  bool                             bForceFrameDrop )
{
	BDBG_ENTER(BVDC_Display_SetForceFrameDrop);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	hDisplay->stNewInfo.bForceFrameDrop = bForceFrameDrop;

	/* Set display dirty bit */
	hDisplay->stNewInfo.stDirty.stBits.bRateChange = BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_Display_SetForceFrameDrop);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_GetForceFrameDrop
 *************************************************************************/
BERR_Code BVDC_Display_GetForceFrameDrop
	( BVDC_Display_Handle              hDisplay,
	  bool                            *pbForceFrameDrop )
{
	BDBG_ENTER(BVDC_Display_GetForceFrameDrop);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(pbForceFrameDrop)
	{
		*pbForceFrameDrop = hDisplay->stCurInfo.bForceFrameDrop;
	}

	BDBG_LEAVE(BVDC_Display_GetForceFrameDrop);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_SetTimebase
 *************************************************************************/
BERR_Code BVDC_Display_SetTimebase
	( BVDC_Display_Handle              hDisplay,
	  BAVC_Timebase                    eTimeBase )
{
	BDBG_ENTER(BVDC_Display_SetTimebase);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* Note: which timebase the bypass video 656 output is locked to is
	 * out of control of VDC PI; it's up to the upper level software to
	 * determine what timebase to use for vec 656 output clock by
	 * programming CLK_MISC register and VCXO_x_RM block; */
	if(hDisplay->bIsBypass)
	{
		BDBG_WRN(("Application needs to program the timebase for bypass video"));
	}

	if(eTimeBase > BAVC_Timebase_e3)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	hDisplay->stNewInfo.eTimeBase = eTimeBase;

	BDBG_LEAVE(BVDC_Display_SetTimebase);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_GetTimebase
 *************************************************************************/
BERR_Code BVDC_Display_GetTimebase
	( const BVDC_Display_Handle        hDisplay,
	  BAVC_Timebase                   *peTimeBase )
{
	BDBG_ENTER(BVDC_Display_GetTimebase);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* Note: which timebase the bypass video 656 output is locked to is
	 * out of control of VDC PI; it's up to the upper level software to
	 * determine what timebase to use for vec 656 output clock by
	 * programming CLK_MISC register and VCXO_x_RM block; */
	if(hDisplay->bIsBypass)
	{
		BDBG_ERR(("Application needs to program the timebase for bypass video"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(peTimeBase)
	{
		*peTimeBase = hDisplay->stCurInfo.eTimeBase;
	}

	BDBG_LEAVE(BVDC_Display_GetTimebase);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_SetCallbackSettings
 *************************************************************************/
BERR_Code BVDC_Display_SetCallbackSettings
(
	BVDC_Display_Handle                  hDisplay,
	const BVDC_Display_CallbackSettings *pSettings
)
{
	BDBG_ENTER(BVDC_Display_SetCallbackSettings);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(!pSettings)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	hDisplay->stCallbackSettings.stMask = pSettings->stMask;

	BDBG_LEAVE(BVDC_Display_SetCallbackSettings);
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_GetCallbackSettings
 *************************************************************************/
BERR_Code BVDC_Display_GetCallbackSettings
(
	BVDC_Display_Handle                  hDisplay,
	BVDC_Display_CallbackSettings       *pSettings
)
{
	BDBG_ENTER(BVDC_Display_GetCallbackSettings);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if (pSettings)
	{
		*pSettings = hDisplay->stCallbackSettings;
	}

	BDBG_LEAVE(BVDC_Display_GetCallbackSettings);
	return BERR_SUCCESS;
}

/*************************************************************************
 * BVDC_Display_InstallCallback
 *************************************************************************/
BERR_Code BVDC_Display_InstallCallback
	( BVDC_Display_Handle              hDisplay,
	  const BVDC_CallbackFunc_isr      pfCallback,
	  void                            *pvParm1,
	  int                              iParm2 )
{
	BDBG_ENTER(BVDC_Display_InstallCallback);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* Store the new infos */
	hDisplay->stNewInfo.pfGenericCallback = pfCallback;
	hDisplay->stNewInfo.pvGenericParm1    = pvParm1;
	hDisplay->stNewInfo.iGenericParm2     = iParm2;

	BDBG_LEAVE(BVDC_Display_InstallCallback);
	return BERR_SUCCESS;

}

/*************************************************************************
 * BVDC_Display_EnableColorCorrection
 *************************************************************************/
BERR_Code BVDC_Display_EnableColorCorrection
	( BVDC_Display_Handle              hDisplay,
	  bool                             bEnable )
{
	BVDC_P_DisplayInfo        *pNewInfo;
	BVDC_P_DisplayInfo        *pCurInfo;

	BDBG_ENTER(BVDC_Display_EnableColorCorrection);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	pNewInfo = &hDisplay->stNewInfo;
	pCurInfo = &hDisplay->stCurInfo;

	BDBG_MSG(("disp[%d] - %s color correction",
		hDisplay->eId, (bEnable) ? "Enable" : "Disable"));

	if(bEnable != pCurInfo->bCCEnable)
	{
		pNewInfo->bCCEnable = bEnable;
		pNewInfo->stDirty.stBits.bCcbAdjust = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_EnableColorCorrection);
	return BERR_SUCCESS;
}

/*************************************************************************
 * BVDC_Display_SetColorCorrectionTable
 *************************************************************************/
BERR_Code BVDC_Display_SetColorCorrectionTable
	( BVDC_Display_Handle              hDisplay,
	  uint32_t                         ulGammaTableId,
	  uint32_t                         ulColorTempId )
{
	BVDC_P_DisplayInfo        *pNewInfo;
	BVDC_P_DisplayInfo        *pCurInfo;

	BDBG_ENTER(BVDC_Display_SetColorCorrectionTable);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	pNewInfo = &hDisplay->stNewInfo;
	pCurInfo = &hDisplay->stCurInfo;
	pNewInfo->bUserCCTable = false;

	if(ulGammaTableId != pCurInfo->ulGammaTableId ||
	   ulColorTempId  != pCurInfo->ulColorTempId  ||
	   pNewInfo->bUserCCTable != pCurInfo->bUserCCTable)
	{
		pNewInfo->ulGammaTableId = ulGammaTableId;
		pNewInfo->ulColorTempId  = ulColorTempId;
		pNewInfo->stDirty.stBits.bCcbAdjust = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_SetColorCorrectionTable);
	return BERR_SUCCESS;
}

/*************************************************************************
 * BVDC_Display_GetColorCorrectionTable
 *************************************************************************/
BERR_Code BVDC_Display_GetColorCorrectionTable
	( const BVDC_Display_Handle        hDisplay,
	  bool                            *pbEnable,
	  uint32_t                        *pulGammaTableId,
	  uint32_t                        *pulColorTempId )
{

	BDBG_ENTER(BVDC_Display_GetColorCorrectionTable);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(pbEnable)
	{
		*pbEnable = hDisplay->stCurInfo.bCCEnable;
	}

	if(hDisplay->stCurInfo.bUserCCTable)
	{
		return BERR_NOT_SUPPORTED;
	}
	else
	{
		if(pulGammaTableId)
		{
			*pulGammaTableId = hDisplay->stCurInfo.ulGammaTableId;
		}
		if(pulColorTempId)
		{
			*pulColorTempId = hDisplay->stCurInfo.ulColorTempId;
		}
	}

	BDBG_LEAVE(BVDC_Display_GetColorCorrectionTable);
	return BERR_SUCCESS;
}

/*************************************************************************
 * BVDC_Display_LoadColorCorrectionTable
 *************************************************************************/
BERR_Code BVDC_Display_LoadColorCorrectionTable
	( BVDC_Display_Handle              hDisplay,
	  const uint32_t                  *pulCcbTable )
{
	BDBG_ENTER(BVDC_Display_LoadColorCorrectionTable);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(pulCcbTable == NULL)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	else
	{
		BDBG_MSG(("disp[%d] - Loading User CCB Table", hDisplay->eId));
		BKNI_Memcpy((void*)&(hDisplay->stNewInfo.aulUserCCTable[0]), (void*)pulCcbTable, BVDC_P_CCB_TABLE_SIZE * sizeof(uint32_t));

		hDisplay->stNewInfo.bUserCCTable = true;
		hDisplay->stNewInfo.stDirty.stBits.bCcbAdjust = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_LoadColorCorrectionTable);
	return BERR_SUCCESS;
}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_SetDvoConfiguration
	( BVDC_Display_Handle              hDisplay,
	  const BVDC_Display_DvoSettings  *pDvoSettings )
{
	BVDC_P_DisplayInfo *pNewInfo;
	const BVDC_Display_DvoSettings *pCurDvoSettings;

	BDBG_ENTER(BVDC_Display_SetDvoConfiguration);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	pNewInfo = &hDisplay->stNewInfo;
	pCurDvoSettings = &hDisplay->stCurInfo.stDvoCfg;

	/* Changes these require the DVO timing block to reconfig */
	pNewInfo->stDvoCfg  = *pDvoSettings;

	if((pDvoSettings->eDePolarity != pCurDvoSettings->eDePolarity) ||
	   (pDvoSettings->eHsPolarity != pCurDvoSettings->eHsPolarity) ||
	   (pDvoSettings->eVsPolarity != pCurDvoSettings->eVsPolarity) ||
	   (pDvoSettings->stSpreadSpectrum.bEnable != pCurDvoSettings->stSpreadSpectrum.bEnable) ||
	   (pDvoSettings->stSpreadSpectrum.ulFrequency != pCurDvoSettings->stSpreadSpectrum.ulFrequency) ||
	   (pDvoSettings->stSpreadSpectrum.ulDelta != pCurDvoSettings->stSpreadSpectrum.ulDelta))
	{
		pNewInfo->stDirty.stBits.bTiming = BVDC_P_DIRTY;
	}

	if(pDvoSettings->b8BitPanel != pCurDvoSettings->b8BitPanel)
	{
		pNewInfo->stDirty.stBits.bCscAdjust = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Display_SetDvoConfiguration);
	return BERR_SUCCESS;
}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_GetDvoConfiguration
	( const BVDC_Display_Handle        hDisplay,
	  BVDC_Display_DvoSettings        *pDvoSettings )
{
	BDBG_ENTER(BVDC_Display_GetDvoConfiguration);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(pDvoSettings)
	{
		*pDvoSettings = hDisplay->stCurInfo.stDvoCfg;
	}

	BDBG_LEAVE(BVDC_Display_GetDvoConfiguration);
	return BERR_SUCCESS;
}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_SetDvoAttenuationRGB
(
	BVDC_Display_Handle                hDisplay,
	int32_t                            nAttentuationR,
	int32_t                            nAttentuationG,
	int32_t                            nAttentuationB,
	int32_t                            nOffsetR,
	int32_t                            nOffsetG,
	int32_t                            nOffsetB
)
{
	int32_t ulShiftBits;

	BDBG_ENTER(BVDC_Display_SetDvoAttenuationRGB);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	ulShiftBits = BVDC_P_CSC_DVO_CX_F_BITS - 11;

	hDisplay->stNewInfo.lDvoAttenuationR = nAttentuationR << ulShiftBits;
	hDisplay->stNewInfo.lDvoAttenuationG = nAttentuationG << ulShiftBits;
	hDisplay->stNewInfo.lDvoAttenuationB = nAttentuationB << ulShiftBits;
	hDisplay->stNewInfo.lDvoOffsetR      = nOffsetR       << ulShiftBits;
	hDisplay->stNewInfo.lDvoOffsetG      = nOffsetG       << ulShiftBits;
	hDisplay->stNewInfo.lDvoOffsetB      = nOffsetB       << ulShiftBits;

	/* Set display dirty bit!  Trigger update of CSC */
	hDisplay->stNewInfo.stDirty.stBits.bCscAdjust = BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_Display_SetDvoAttenuationRGB);
	return BERR_SUCCESS;
}

/*************************************************************************
 *
 *************************************************************************/
BERR_Code BVDC_Display_GetDvoAttenuationRGB
(
	BVDC_Display_Handle                hDisplay,
	int32_t                           *plAttenuationR,
	int32_t                           *plAttenuationG,
	int32_t                           *plAttenuationB,
	int32_t                           *plOffsetR,
	int32_t                           *plOffsetG,
	int32_t                           *plOffsetB
)
{
	int32_t ulShiftBits;

	BDBG_ENTER(BVDC_Display_GetDvoAttenuationRGB);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	ulShiftBits = BVDC_P_CSC_DVO_CX_F_BITS - 11;

	(*plAttenuationR) = hDisplay->stNewInfo.lDvoAttenuationR >> ulShiftBits;
	(*plAttenuationG) = hDisplay->stNewInfo.lDvoAttenuationG >> ulShiftBits;
	(*plAttenuationB) = hDisplay->stNewInfo.lDvoAttenuationB >> ulShiftBits;
	(*plOffsetR)      = hDisplay->stNewInfo.lDvoOffsetR      >> ulShiftBits;
	(*plOffsetG)      = hDisplay->stNewInfo.lDvoOffsetG      >> ulShiftBits;
	(*plOffsetB)      = hDisplay->stNewInfo.lDvoOffsetB      >> ulShiftBits;

	BDBG_LEAVE(BVDC_Display_GetDvoAttenuationRGB);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Display_SetDvoColorMatrix
	( BVDC_Display_Handle              hDisplay,
	  bool                             bOverride,
	  const int32_t                    pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  uint32_t                         ulShift )
{
	BDBG_ENTER(BVDC_Display_SetDvoColorMatrix);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* set new values */
	hDisplay->stNewInfo.bUserCsc = bOverride;
	if(bOverride)
	{
		uint32_t ulIndex;
		hDisplay->stNewInfo.ulUserShift = ulShift;
		for(ulIndex = 0; ulIndex < BVDC_CSC_COEFF_COUNT; ulIndex++)
		{
			hDisplay->stNewInfo.pl32_Matrix[ulIndex] = pl32_Matrix[ulIndex];
		}
	}

	/* Set display dirty bit */
	hDisplay->stNewInfo.stDirty.stBits.bCscAdjust = BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_Display_SetDvoColorMatrix);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Display_GetDvoColorMatrix
	( BVDC_Display_Handle              hDisplay,
	  bool                            *pbOverride,
	  int32_t                          pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  uint32_t                        *pulShift )
{
	BDBG_ENTER(BVDC_Display_GetDvoColorMatrix);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(pbOverride)
	{
		*pbOverride = hDisplay->stCurInfo.bUserCsc;
	}

	if(hDisplay->stCurInfo.bUserCsc)
	{
		uint32_t ulIndex;
		for(ulIndex = 0; ulIndex < BVDC_CSC_COEFF_COUNT; ulIndex++)
		{
			pl32_Matrix[ulIndex] = hDisplay->stCurInfo.pl32_Matrix[ulIndex];
		}

		if(pulShift)
		{
			*pulShift = hDisplay->stCurInfo.ulUserShift;
		}
	}
	else
	{
		BKNI_EnterCriticalSection();
		BVDC_P_Csc_ToMatrixDvo_isr(pl32_Matrix, &hDisplay->stDvoCscMatrix.stCscCoeffs,
			BVDC_P_FIX_POINT_SHIFT,
			((hDisplay->stCurInfo.eHdmiOutput == BAVC_MatrixCoefficients_eHdmi_RGB) ||
			 (hDisplay->stCurInfo.eHdmiOutput == BAVC_MatrixCoefficients_eDvi_Full_Range_RGB))? true : false);
		BKNI_LeaveCriticalSection();

		if(pulShift)
		{
			*pulShift = BVDC_P_FIX_POINT_SHIFT;
		}
	}

	BDBG_LEAVE(BVDC_Display_GetDvoColorMatrix);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Display_SetAlignment
	( BVDC_Display_Handle                           hDisplay,
	  BVDC_Display_Handle                           hTargetDisplay,
	  const BVDC_Display_AlignmentSettings         *pAlignSettings )
{
	BDBG_ENTER(BVDC_Display_SetAlignment);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* set new values */
	if(hTargetDisplay && pAlignSettings)
	{
		BDBG_OBJECT_ASSERT(hTargetDisplay, BVDC_DSP);

		/* set dirty flag */
		if(!hDisplay->stNewInfo.hTargetDisplay ||
		   (pAlignSettings->eDirection != hDisplay->stNewInfo.stAlignCfg.eDirection) ||
		   (pAlignSettings->bKeepBvnConnected != hDisplay->stNewInfo.stAlignCfg.bKeepBvnConnected))
		{
			/* Set display dirty bit */
			hDisplay->stNewInfo.stDirty.stBits.bAlignChange = BVDC_P_DIRTY;
		}

		hDisplay->stNewInfo.hTargetDisplay = hTargetDisplay;
		hDisplay->stNewInfo.stAlignCfg        = *pAlignSettings;
	}
	else
	{
		/* set dirty flag */
		if(hDisplay->stNewInfo.hTargetDisplay )
		{
			/* Set display dirty bit */
			hDisplay->stNewInfo.stDirty.stBits.bAlignChange = BVDC_P_DIRTY;
		}
		hDisplay->stNewInfo.hTargetDisplay = NULL;
	}

	BDBG_LEAVE(BVDC_Display_SetAlignment);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Display_GetAlignment
	( const BVDC_Display_Handle                  hDisplay,
	  BVDC_Display_Handle                         *phTargetDisplay,
	  BVDC_Display_AlignmentSettings         *pAlignSettings )
{
	BDBG_ENTER(BVDC_Display_GetAlignment);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* set new values */
	if(pAlignSettings)
	{
		*pAlignSettings = hDisplay->stNewInfo.stAlignCfg;
	}

	if(phTargetDisplay)
	{
		*phTargetDisplay = hDisplay->stNewInfo.hTargetDisplay;
	}

	BDBG_LEAVE(BVDC_Display_GetAlignment);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Display_SetOrientation
	( const BVDC_Display_Handle        hDisplay,
	  BFMT_Orientation                 eOrientation )
{
	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(eOrientation);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Display_GetOrientation
	( const BVDC_Display_Handle        hDisplay,
	  BFMT_Orientation                *peOrientation )
{
	BSTD_UNUSED(hDisplay);

	if(peOrientation)
		*peOrientation = BFMT_Orientation_e2D;

	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Display_Set3dSourceBufferSelect
	( const BVDC_Display_Handle        hDisplay,
	  BVDC_3dSourceBufferSelect        e3dSrcBufSel )
{
	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(e3dSrcBufSel);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Display_Get3dSourceBufferSelect
	( const BVDC_Display_Handle        hDisplay,
	  BVDC_3dSourceBufferSelect       *pe3dSrcBufSel )
{
	BSTD_UNUSED(hDisplay);

	if(pe3dSrcBufSel)
		*pe3dSrcBufSel = BVDC_3dSourceBufferSelect_eNormal;

	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Display_SetVfFilter
	( BVDC_Display_Handle              hDisplay,
	  BVDC_DisplayOutput               eDisplayOutput,
	  BVDC_DacOutput                   eDacOutput,
	  bool                             bOverride,
	  const uint32_t                  *paulFilterRegs,
	  uint32_t                         ulNumFilterRegs )
{
	uint32_t *paulUserVfFilter = NULL;
	int32_t lChannel;

	BDBG_ENTER(BVDC_Display_SetVfFilter);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if((eDisplayOutput != BVDC_DisplayOutput_eComponent) &&
	   (eDisplayOutput != BVDC_DisplayOutput_eComposite) &&
	   (eDisplayOutput != BVDC_DisplayOutput_eSVideo))
	{
		BDBG_ERR(("VF filter can only be set on Component, Cvbs, or Svideo outputs."));
		return BERR_INVALID_PARAMETER;
	}

	if (!BVDC_P_DISP_IS_VALID_DISPOUTPUT_AND_DAC(eDispOutput, eDacOutput))
	{
		BDBG_ERR(("Invalid display output and dac output combination."));
		return BERR_INVALID_PARAMETER;
	}

	lChannel = BVDC_P_DISP_GET_VF_CH_FROM_DAC(eDacOutput);

	if (lChannel == BVDC_P_DISP_INVALID_VF_CH)
	{
		BDBG_ERR(("Invalid VF channel."));
		return BERR_INVALID_PARAMETER;
	}

	/* set new values */
	if ((eDisplayOutput == BVDC_DisplayOutput_eComposite) ||
		(eDisplayOutput == BVDC_DisplayOutput_eSVideo))
	{
		hDisplay->stNewInfo.abUserVfFilterCvbs[lChannel] = bOverride;
		paulUserVfFilter = (uint32_t *)hDisplay->stNewInfo.aaulUserVfFilterCvbs[lChannel];
	}
	else if (eDisplayOutput == BVDC_DisplayOutput_eComponent)
	{
		hDisplay->stNewInfo.abUserVfFilterCo[lChannel] = bOverride;
		paulUserVfFilter = (uint32_t *)hDisplay->stNewInfo.aaulUserVfFilterCo[lChannel];
	}

	if(bOverride)
	{
		if (ulNumFilterRegs != BVDC_P_CHROMA_TABLE_SIZE)
		{
			BDBG_ERR(("Incorrect VF filter table size %d.  Table size should be %d or greater for this platform. ", ulNumFilterRegs, BVDC_P_CHROMA_TABLE_SIZE));
			return BERR_INVALID_PARAMETER;
		}

		BKNI_Memcpy(paulUserVfFilter, paulFilterRegs, BVDC_P_CHROMA_TABLE_SIZE * sizeof(uint32_t));
	}

	/* Set display dirty bit */
	hDisplay->stNewInfo.stDirty.stBits.bVfFilter= BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_Display_SetVfFilter);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Display_GetVfFilter
	( BVDC_Display_Handle              hDisplay,
	  BVDC_DisplayOutput               eDisplayOutput,
	  BVDC_DacOutput                   eDacOutput,
	  bool                            *pbOverride,
	  uint32_t                        *paulFilterRegs,
	  uint32_t                         ulNumFilterRegs )
{
	uint32_t *paulUserVfFilter = NULL;
	const uint32_t *pVfFilter = NULL;
	int32_t lChannel;
	bool bOverride = false;

	BDBG_ENTER(BVDC_Display_GetVfFilter);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if((eDisplayOutput != BVDC_DisplayOutput_eComponent) &&
	   (eDisplayOutput != BVDC_DisplayOutput_eComposite) &&
	   (eDisplayOutput != BVDC_DisplayOutput_eSVideo))
	{
		BDBG_ERR(("VF filter can only be retrieved from Component, Cvbs, or Svideo outputs."));
		return BERR_INVALID_PARAMETER;
	}

	if (ulNumFilterRegs < BVDC_P_CHROMA_TABLE_SIZE)
	{
		BDBG_ERR(("Incorrect VF filter table size %d.  Table size should be %d or greater for this platform. ", ulNumFilterRegs, BVDC_P_CHROMA_TABLE_SIZE));
		return BERR_INVALID_PARAMETER;
	}

	if (!BVDC_P_DISP_IS_VALID_DISPOUTPUT_AND_DAC(eDispOutput, eDacOutput))
	{
		BDBG_ERR(("Invalid display output and dac output combination."));
		return BERR_INVALID_PARAMETER;
	}

	lChannel = BVDC_P_DISP_GET_VF_CH_FROM_DAC(eDacOutput);

	if (lChannel == BVDC_P_DISP_INVALID_VF_CH)
	{
		BDBG_ERR(("Invalid VF channel."));
		return BERR_INVALID_PARAMETER;
	}

	if ((eDisplayOutput == BVDC_DisplayOutput_eComposite) ||
		(eDisplayOutput == BVDC_DisplayOutput_eSVideo))
	{
		bOverride = hDisplay->stCurInfo.abUserVfFilterCvbs[lChannel];
		paulUserVfFilter = (uint32_t *)hDisplay->stCurInfo.aaulUserVfFilterCvbs[lChannel];
		pVfFilter = hDisplay->apVfFilterCvbs[lChannel];
	}
	else if (eDisplayOutput == BVDC_DisplayOutput_eComponent)
	{
		bOverride = hDisplay->stCurInfo.abUserVfFilterCo[lChannel];
		paulUserVfFilter = (uint32_t *)hDisplay->stCurInfo.aaulUserVfFilterCo[lChannel];
		pVfFilter = hDisplay->apVfFilterCo[lChannel];
	}

	if(pbOverride)
	{
		*pbOverride = bOverride;
	}

	if(bOverride)
	{
		/* TODO: Is this necessary or can be merged with below? */
		BKNI_Memcpy(paulFilterRegs, paulUserVfFilter, BVDC_P_CHROMA_TABLE_SIZE * sizeof(uint32_t));
	}
	else
	{
		BKNI_EnterCriticalSection();
		if(pVfFilter)
		{
			BKNI_Memcpy(paulFilterRegs, hDisplay->apVfFilterCvbs[lChannel], BVDC_P_CHROMA_TABLE_SIZE * sizeof(uint32_t));
		}
		else
		{
			BDBG_ERR(("Display output does not exist.  Cannot get internal VF filter settings."));
			return BERR_INVALID_PARAMETER;
		}
		BKNI_LeaveCriticalSection();
	}

	BDBG_LEAVE(BVDC_Display_GetDvoColorMatrix);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Display_SetMuteMode
	( BVDC_Display_Handle              hDisplay,
	  BVDC_DisplayOutput               eDisplayOutput,
	  BVDC_MuteMode                    eMuteMode )
{
	bool bMute;

	BDBG_ENTER(BVDC_Display_SetMuteMode);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if (eMuteMode == BVDC_MuteMode_eConst)
	{
		bMute = true;
	}
	else if (eMuteMode == BVDC_MuteMode_eDisable)
	{
		bMute = false;
	}
	else
	{
		BDBG_ERR(("Selected mute mode not supported for output displays"));
		return BERR_INVALID_PARAMETER;
	}

	/* set new values */
	hDisplay->stNewInfo.abOutputMute[eDisplayOutput] = bMute;

	/* tie Cvbs and Svideo together */
	if ((eDisplayOutput == BVDC_DisplayOutput_eComposite) || (eDisplayOutput == BVDC_DisplayOutput_eSVideo))
	{
		hDisplay->stNewInfo.abOutputMute[BVDC_DisplayOutput_eSVideo] = bMute;
		hDisplay->stNewInfo.abOutputMute[BVDC_DisplayOutput_eComposite] = bMute;
	}

	/* Set display dirty bit */
	hDisplay->stNewInfo.stDirty.stBits.bOutputMute = BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_Display_SetMuteMode);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Display_GetMuteMode
	( BVDC_Display_Handle              hDisplay,
	  BVDC_DisplayOutput               eDisplayOutput,
	  BVDC_MuteMode                   *peMuteMode )
{
	bool bMute;

	BDBG_ENTER(BVDC_Display_GetMuteMode);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	if(peMuteMode)
	{
		bMute = hDisplay->stCurInfo.abOutputMute[eDisplayOutput];
		*peMuteMode = bMute ? BVDC_MuteMode_eConst : BVDC_MuteMode_eDisable;
	}

	BDBG_LEAVE(BVDC_Display_GetMuteMode);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Display_GetCapabilities
	( BVDC_Display_Handle              hDisplay,
	  BVDC_Display_Capabilities       *pCapabilities )
{
	BSTD_UNUSED(hDisplay);

	if(pCapabilities)
	{
		/* To make sure thing get initialize */
		BKNI_Memset(pCapabilities, 0, sizeof(*pCapabilities));
		pCapabilities->pfIsVidfmtSupported = BVDC_P_IsVidfmtSupported;
	}

	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Display_SetArtificticalVsync
	( BVDC_Display_Handle              hDisplay,
	  bool                             bEnable,
	  uint32_t                         ulVsyncRegAddr,
	  uint32_t                         ulVsyncMask )
{
	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(bEnable);
	BSTD_UNUSED(ulVsyncRegAddr);
	BSTD_UNUSED(ulVsyncMask);
	return BERR_TRACE(BVDC_ERR_FEATURE_NOT_SUPPORTED);
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Display_GetArtificticalVsync
	( const BVDC_Display_Handle        hDisplay,
	  bool                            *pbEnable,
	  uint32_t                        *pulVsyncRegAddr,
	  uint32_t                        *pulVsyncMask )
{
	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(pbEnable);
	BSTD_UNUSED(pulVsyncRegAddr);
	BSTD_UNUSED(pulVsyncMask);
	return BERR_TRACE(BVDC_ERR_FEATURE_NOT_SUPPORTED);
}

/* End of File */
