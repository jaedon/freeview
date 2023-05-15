/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: cmd_handle.c $
 * $brcm_Revision: Hydra_Software_Devel/208 $
 * $brcm_Date: 10/12/12 3:51p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/cmd_handle.c $
 * 
 * Hydra_Software_Devel/208   10/12/12 3:51p vanessah
 * SW7445-10: add VDC memory controller information
 * 
 * Hydra_Software_Devel/207   10/11/12 4:26p vanessah
 * SW7425-2501: remove vdec input support
 * 
 * Hydra_Software_Devel/206   10/10/12 11:24a vanessah
 * SW7445-10: add emu support for 7445
 * 
 * Hydra_Software_Devel/205   10/8/12 3:29p syang
 * SW7346-572: use BTST_P_ApplyChanges to get uniform error handling;
 * avoid holding hVdcCfgLock too long
 * 
 * Hydra_Software_Devel/204   9/25/12 5:35p vanessah
 * SW7425-2501: vdc_test clean up
 * 
 * Hydra_Software_Devel/203   9/13/12 9:56a vanessah
 * SW7360-30: disable hdmi timer for compile
 * 
 * Hydra_Software_Devel/202   9/12/12 6:06p vanessah
 * SW7425-2501: vdc_test clean up
 * 
 * Hydra_Software_Devel/201   9/5/12 6:20p vanessah
 * SW7425-2501: vdc_test clean up
 * 
 * Hydra_Software_Devel/200   8/9/12 11:20a syang
 * SW7425-2172: add SetSurface support for MFD gfx feeding
 * 
 * Hydra_Software_Devel/199   8/7/12 3:23p vanessah
 * SW7360-9: exclude 7550 from hTMR
 * 
 * Hydra_Software_Devel/198   7/27/12 3:47p vanessah
 * SW7425-3150: add video processing skip support
 * 
 * Hydra_Software_Devel/197   7/23/12 11:30a vanessah
 * SW7360-30: make 7360 compile until hdmi label move
 * 
 * Hydra_Software_Devel/196   7/16/12 12:36p darnstein
 * SW7425-2989: BHDM_Open() now requires that a BTMR handle be supplied
 * via settings.
 * 
 * Hydra_Software_Devel/195   7/10/12 9:55a vanessah
 * SW7425-2501: correct 7550 compile error
 * 
 * Hydra_Software_Devel/194   7/9/12 5:16p vanessah
 * SW7425-2501: removal 3548/3556 support
 * 
 * Hydra_Software_Devel/193   7/4/12 11:26a vanessah
 * SW7425-2501: clean up vdc
 * 
 * Hydra_Software_Devel/192   6/19/12 4:50p chengs
 * SW7584-5: Add 7584 support.
 * 
 * Hydra_Software_Devel/191   6/8/12 3:59p vanessah
 * SW7425-1078: restructure the code for A0/P0 format
 * 
 * Hydra_Software_Devel/190   6/1/12 7:14p vanessah
 * SW7435-201: add run time transcode rts selection
 * 
 * Hydra_Software_Devel/189   5/30/12 7:47p syang
 * SW7425-2093: take off extra ApplyChanges for BVDC_Source_SetSurface
 * 
 * Hydra_Software_Devel/188   5/25/12 5:50p syang
 * SW7425-2093: refactor vfd gfx feeding support for deep cleanup and to
 * use gfxsurface sub-module
 * 
 * Hydra_Software_Devel/187   5/18/12 3:44p vanessah
 * SW7425-3069: add resource leak change test
 * 
 * Hydra_Software_Devel/186   5/10/12 1:48p syang
 * SW7425-2093:  init BAVC_Gfx_Picture to 0 before using it
 * 
 * Hydra_Software_Devel/185   4/10/12 3:45p tdo
 * SW7435-97: Format change on the SD path with progressive stream cause
 * HD path to flicker
 *
 * Hydra_Software_Devel/184   3/12/12 4:06p vanessah
 * SW7425-465: change the chip dependent condition to independent
 *
 * Hydra_Software_Devel/183   3/7/12 4:40p vanessah
 * SW7435-9: add Quad support for GFD
 *
 * Hydra_Software_Devel/182   3/6/12 9:56p vanessah
 * SW7435-9: add Quad transcode support
 *
 * Hydra_Software_Devel/181   2/29/12 7:00p tdo
 * SW7435-9: Bring up VDC for 7435A0
 *
 * Hydra_Software_Devel/180   2/29/12 5:27p tdo
 * SW7435-9: add support for 7435A0 in VDC
 *
 * Hydra_Software_Devel/179   2/29/12 4:36p tdo
 * SW7435-9: add support for 7435A0 in VDC
 *
 * Hydra_Software_Devel/178   2/9/12 4:50p yuxiaz
 * SW7425-2181: Added test case in vdc_test
 *
 * Hydra_Software_Devel/177   1/20/12 6:40p vanessah
 * SW7435-9: add STG2/3 support
 *
 * Hydra_Software_Devel/176   12/16/11 4:09p yuxiaz
 * SW7425-967: Added support to use bitmap image for static buffer test.
 *
 * Hydra_Software_Devel/175   12/9/11 10:30a yuxiaz
 * SW7425-967: Added static buffer test for MRE.
 *
 * Hydra_Software_Devel/174   9/29/11 3:22p vanessah
 * SW7425-1078: A0 and P0 format support on GFD test
 *
 * Hydra_Software_Devel/173   8/25/11 10:45a tdo
 * SW7420-2030: Add apply changes error tracking capability
 *
 * Hydra_Software_Devel/172   8/17/11 9:56a vanessah
 * SW7425-922: b0 vdc_test bringup. VDC_TEST_ONLY for emulation
 *
 * Hydra_Software_Devel/171   8/15/11 4:47p tdo
 * SW7420-2017: Add test to scan all size for scaler enable error
 *
 * Hydra_Software_Devel/170   8/12/11 11:59a vanessah
 * SW7425-32: no hdmi transfer mode false set when hdmi handle is NULL
 *
 * Hydra_Software_Devel/169   7/29/11 2:02p pntruong
 * SW7400-3045: Fix build errors.
 *
 * Hydra_Software_Devel/168   7/25/11 5:55p vanessah
 * SW7425-465: add cmd to adjust minimum display and src fmt
 *
 * Hydra_Software_Devel/167   7/18/11 11:29a vanessah
 * SW7425-835: SW7425-923: fix ViCE2 channel id bug + B0 STG
 *
 * Hydra_Software_Devel/166   6/24/11 12:52p darnstein
 * SW7405-5081: Provide "custom" setting to select ARIB style video
 * output.
 *
 * Hydra_Software_Devel/165   6/24/11 11:29a vanessah
 * SW7425-761: add vdc_test min display  buffer to 1080p instead of 720p
 * to avoid buffer reallocation
 *
 * Hydra_Software_Devel/164   6/21/11 11:20a vanessah
 * SW7550-758: update the window's bShrinkWidth when window open
 *
 * Hydra_Software_Devel/163   6/10/11 4:00p darnstein
 * SW7231-187: Use a new macro symbol BTST_P_SUPPORT_3D_GFD to indicate
 * that a chipset has a graphics feeder with 3D capability.
 *
 * Hydra_Software_Devel/162   6/8/11 5:35p vanessah
 * SW7231-188: Extended output and input orientation to Left and right for
 * 3D GFX
 *
 * Hydra_Software_Devel/161   6/8/11 5:04p vanessah
 * SW7231-188: change the right surface offset due to PI change
 *
 * Hydra_Software_Devel/160   5/24/11 3:08p hongtaoz
 * SW7425-465: fixed segfault when STG display is destroyed;
 *
 * Hydra_Software_Devel/159   5/23/11 2:25p vanessah
 * SW7425-465: change application min display memory allocation to 720p
 * for transcode
 *
 * Hydra_Software_Devel/158   4/1/11 12:26p vanessah
 * SW7422-118: change gfx double buffer and get rid of 7425 CFE v3.1
 * support
 *
 * Hydra_Software_Devel/157   3/21/11 1:18p darnstein
 * SW7231-2: 7344 and 7346 use MAD core by default.
 *
 * Hydra_Software_Devel/156   3/9/11 7:02p vanessah
 * SW7425-32: compatible with CFE3.1 and 3.2. Follow BTST_CFE31
 *
 * Hydra_Software_Devel/155   3/4/11 11:50a vanessah
 * SW7425-32: change memory and heap due to new CFE
 *
 * Hydra_Software_Devel/154   3/1/11 2:43p darnstein
 * SW7335-868: restore CVBS output for chipsets 7344 and 7346.
 *
 * Hydra_Software_Devel/153   2/22/11 5:49p vanessah
 * SW7425-46: fix not able to open multiple compositor in Application.
 *
 * Hydra_Software_Devel/152   2/16/11 4:30p yuxiaz
 * SW7420-1178: Merge loopback support to mainline. Added compile option
 * to enable loopback support.
 *
 * Hydra_Software_Devel/151   2/8/11 5:31p vanessah
 * SW7425-46: skip picture when MFD is mute
 *
 * Hydra_Software_Devel/150   2/3/11 5:32p vanessah
 * SW7425-29:  non-real-time ViCE2 support
 *
 * Hydra_Software_Devel/149   1/27/11 5:30p vanessah
 * SW7425-32: unreleased heap issue for 7425
 *
 * Hydra_Software_Devel/148   1/26/11 3:16p yuxiaz
 * SW7400-2998, SW7425-32: Only reduce cdb/itb buffer size for mosaic
 * mode.
 *
 * Hydra_Software_Devel/147   1/20/11 11:44a vanessah
 * SW7422-43:  Adjust the window size according to the format
 *
 * Hydra_Software_Devel/146   1/18/11 5:28p vanessah
 * SW7422-43:  3D graphics support adding HDMI support
 *
 * Hydra_Software_Devel/145   1/18/11 4:57p vanessah
 * SW7422-43:  3D graphics support
 *
 * Hydra_Software_Devel/144   1/14/11 3:09p vanessah
 * SW7550-598:  Scart mode for 7550
 *
 * Hydra_Software_Devel/143   12/28/10 11:56a vanessah
 * SW7425-24:
 *
 * Add cvbs into 7425
 *
 * Hydra_Software_Devel/142   12/26/10 11:29a hongtaoz
 * SW7425-32: updated heap assignments for 7425 gfx sources;
 *
 * Hydra_Software_Devel/141   12/21/10 11:08a tdo
 * SW7422-28: Adjust to 3D format size change.  Add test for single MEMC
 * scheme.
 *
 * Hydra_Software_Devel/140   12/14/10 5:22p darnstein
 * SW7231-2: try to get "simul" working on 7344 and 7346.
 *
 * Hydra_Software_Devel/139   12/13/10 8:30p darnstein
 * SW7231-2: 7346 does not have a MAD.
 *
 * Hydra_Software_Devel/138   12/13/10 10:30a nilesh
 * SW7425-1: Fix compilation error when HDM not enabled
 *
 * Hydra_Software_Devel/137   12/10/10 5:37p vanessah
 * SW7550-577:  switch the hdmi master mode to slave mode when close
 * display
 *
 * Hydra_Software_Devel/136   12/10/10 2:09p hongtaoz
 * SW7425-32: updated heap allocation for 7425;
 *
 * Hydra_Software_Devel/135   12/9/10 7:17p hongtaoz
 * SW7425-32: updated window heap MEMC assignment for new 7425 RTS (800MHz
 * with 216 MHz SCB);
 *
 * Hydra_Software_Devel/134   12/8/10 4:31p yuxiaz
 * SW7422-77: Added half resolution 3d format in static buffer test.
 *
 * Hydra_Software_Devel/133   12/3/10 2:57p vanessah
 * SW7422-118:  Get rid of SetSurface_isr calling
 *
 * Hydra_Software_Devel/132   12/3/10 12:23a hongtaoz
 * SW7425-32: added DM instance select for multi-decode test; add
 * 7425/22/20 to the multi-decode list; added 7425 gfx support;
 *
 * Hydra_Software_Devel/131   12/2/10 3:08p katrep
 * SW7231-16:add more support for 7231
 *
 * Hydra_Software_Devel/130   11/30/10 9:08a vanessah
 * SW7422-43:  3D graphics verification in vdc_test
 *
 * Hydra_Software_Devel/129   11/24/10 1:44p jessem
 * SW7422-82: Fixed run-time bugs.
 *
 * Hydra_Software_Devel/128   11/24/10 1:24p hongtaoz
 * SW7425-29: digital display sets digital format size;
 *
 * Hydra_Software_Devel/127   11/22/10 11:48a pntruong
 * SW7422-13: Catch open window errors.
 *
 * Hydra_Software_Devel/126   11/18/10 5:05p darnstein
 * SW7231-16: port to 7344 and 7346.
 *
 * Hydra_Software_Devel/125   11/16/10 6:35p pntruong
 * SW7231-16: Fixed build error.
 *
 * Hydra_Software_Devel/124   11/16/10 5:01p pntruong
 * SW7231-16: Initial support for 7231.
 *
 * Hydra_Software_Devel/123   11/16/10 3:14p hongtaoz
 * SW7425-33: added 4th cmp and gfd support;
 *
 * Hydra_Software_Devel/122   11/15/10 7:44p hongtaoz
 * SW7422-13, SW7425-32: added circle-diagonal-cross-box pattern to static
 * buffer; this will help with BVN bringup test;
 *
 * Hydra_Software_Devel/121   11/15/10 5:15p hongtaoz
 * SW7422-13, SW7425-32: 7422 bringup with static buffer test; enables 2nd
 * xvd instance for 7425;
 *
 * Hydra_Software_Devel/120   11/15/10 12:00p vanessah
 * SW7550-577:  close master trigger for dvi
 *
 * Hydra_Software_Devel/119   11/12/10 1:45p jessem
 * SW7422-82: Added test for stereo 3D support for VFD as source.
 *
 * Hydra_Software_Devel/118   11/11/10 9:24p tdo
 * SW7422-13: Bringup dual decode for 7422
 *
 * Hydra_Software_Devel/117   11/11/10 7:43p hongtaoz
 * SW7425-32: added mosaic mode test for static buffer; added progressive
 * static buffer test;
 *
 * Hydra_Software_Devel/116   11/11/10 6:44p pntruong
 * SW7425-31: Refactored hdm/lvds/hdr/dvo and added support hdmi input on
 * stb chips.  Removed dead codes.
 *
 * Hydra_Software_Devel/115   11/10/10 11:44a tdo
 * SW7422-13: 7422 bringup
 *
 * Hydra_Software_Devel/114   11/5/10 2:44p yuxiaz
 * SW7422-33, SW7422-77: Added 3D support in static buffer test.
 *
 * Hydra_Software_Devel/113   11/3/10 6:56p darnstein
 * SW7425-46: CVBS for second compositor vanished on 7335.
 *
 * Hydra_Software_Devel/112   11/3/10 11:25a yuxiaz
 * SW7422-33: Added format selection for static buffer.
 *
 * Hydra_Software_Devel/111   11/1/10 5:23p tdo
 * SW7422-54: VDC: Update vdc_test with MEMC/window partitioning
 *
 * Hydra_Software_Devel/110   11/1/10 4:24p jessem
 * SW7422-82: Added stereo 3D support to the VFD as source feature.
 *
 * Hydra_Software_Devel/109   10/29/10 5:00p vanessah
 * SW7422-43:  3D graphics render, API change
 *
 * Hydra_Software_Devel/108   10/28/10 4:34p vanessah
 * SW7425-32:  vdc_test compile error
 *
 * Hydra_Software_Devel/107   10/27/10 9:19a vanessah
 * SW7425-46:  Digital resolution instead of the analog one
 *
 * Hydra_Software_Devel/106   10/25/10 4:10p vanessah
 * SW7425-46:  change vdc_test for 7425
 *
 * Hydra_Software_Devel/105   10/25/10 3:59p vanessah
 * SW7425-46:  change vdc_test for 7425
 *
 * Hydra_Software_Devel/104   10/20/10 12:36p jessem
 * SW7420-173: Added regression tests for VF as source feature.
 *
 * Hydra_Software_Devel/103   10/19/10 11:32a yuxiaz
 * SW7422-39: Added test for BAVC_SourceId_eMpeg2 support in VDC.
 *
 * Hydra_Software_Devel/102   10/18/10 3:51p tdo
 * SW7550-598: Fix segfault when toggling hdm output for 7420
 *
 * Hydra_Software_Devel/101   10/15/10 11:15a hongtaoz
 * SW7425-30: only C0_V0 window is on MEMC0 and the rest video windows are
 * on MEMC1 for 7425;
 *
 * Hydra_Software_Devel/100   10/15/10 9:44a vanessah
 * SW7550-598:  change DAC
 *
 * Hydra_Software_Devel/99   10/12/10 5:51p vanessah
 * SW7550-598:  EC fix
 *
 * Hydra_Software_Devel/98   10/12/10 2:30p vanessah
 * SW7550-598:  further fix SCART
 *
 * Hydra_Software_Devel/97   10/12/10 2:13p darnstein
 * SW7550-598: minor error handling input from user.
 *
 * Hydra_Software_Devel/96   10/12/10 12:02p vanessah
 * SW7550-598:  Localize display setting
 *
 * Hydra_Software_Devel/93   10/7/10 5:02p vanessah
 * SW7550-577:  vdc_test change on DAC settings
 *
 * Hydra_Software_Devel/92   10/6/10 10:18a hongtaoz
 * SW7425-29: fixed compile error for 3549;
 *
 * Hydra_Software_Devel/91   10/4/10 11:33p hongtaoz
 * SW7425-33, SW7425-29: added cmp3 test; added STG setting test; STG
 * master should not set DACs;
 *
 * Hydra_Software_Devel/90   9/27/10 4:06p hongtaoz
 * SW7425-32: added VDC_TEST_ONLY compile option;
 *
 * Hydra_Software_Devel/89   6/25/10 11:16a rpan
 * SW7400-2834: Address a race contion by applying display creation before
 * assigning DACs.
 *
 * Hydra_Software_Devel/88   5/11/10 4:29p yuxiaz
 * SW7405-4230: Fixed build error on 3548, 3556.
 *
 * Hydra_Software_Devel/87   5/11/10 2:11p yuxiaz
 * SW7405-4230: Added test to do vec swap at display create.
 *
 * Hydra_Software_Devel/86   2/11/10 10:49a pntruong
 * SW7405-3829: Moved shrinkwidth to mad settings.
 *
 * Hydra_Software_Devel/85   2/1/10 6:45p hongtaoz
 * SW3548-2678, SW3548-2680, SW3548-2681: added HDMI1.4 3D formats support
 * for 3548; added multi-buffer low delay mode support for 1to2 and 2to5
 * rate gap cases; reduced capture buffer allocation for interlaced
 * pulldown case;
 *
 * Hydra_Software_Devel/BCM3D/1   12/17/09 6:29p hongtaoz
 * SW3548-2680, SW3548-2681: added new special display format 2160i48 to
 * support HDMi1.4 3D format for 11-bit BVN; added BHDR frontend config
 * to support 3D video testing;
 *
 * Hydra_Software_Devel/84   1/28/10 2:41p syang
 * SW7405-3829: add option to shrink width to fit mad limit
 *
 * Hydra_Software_Devel/83   1/21/10 3:35p syang
 * SW7550-201: set dacs unused with "cc"
 *
 * Hydra_Software_Devel/82   12/22/09 12:05p rpan
 * SW7400-2646: Fixed a 7400 build issue.
 *
 * Hydra_Software_Devel/81   11/13/09 11:57a syang
 * SW7550-70: update msg for gfx vert scl
 *
 * Hydra_Software_Devel/80   10/14/09 4:47p tdo
 * SW7420-376: Bringup appframework for 7410
 *
 * Hydra_Software_Devel/79   9/30/09 12:32p tdo
 * SW7125-27: Fix runtime error
 *
 * Hydra_Software_Devel/78   9/30/09 11:04a yuxiaz
 * SW7125-27: Fixed vdc_test on 7405.
 *
 * Hydra_Software_Devel/77   9/30/09 10:12a pntruong
 * SW7125-27: Fixed more build errors.
 *
 * Hydra_Software_Devel/76   9/29/09 10:40p pntruong
 * SW7125-27: Fixed build errors.
 *
 * Hydra_Software_Devel/75   9/29/09 4:35p tdo
 * SW7125-27: Correctly handle DAC assignment for new chipsets
 *
 * Hydra_Software_Devel/74   9/15/09 3:38p tdo
 * SW7342-26: Fix Dac assignment for composite output in 2nd compositor
 *
 * Hydra_Software_Devel/73   9/14/09 4:22p hongtaoz
 * SW3548-2471: added test case for the MAD allocation option in window
 * settings;
 *
 * Hydra_Software_Devel/72   8/31/09 4:17p darnstein
 * SW7342-26: BFramework_GetDacAssignments() replaced by #define's.
 *
 * Hydra_Software_Devel/71   8/27/09 7:09p darnstein
 * SW7342-26: DAC assignments to reference board go through
 * BFramework_GetDacAssignments().
 *
 * Hydra_Software_Devel/70   8/21/09 3:07p tdo
 * PR57734: Add capability for display to handle DACs re-assignment
 *
 * Hydra_Software_Devel/69   8/5/09 4:12p syang
 * PR 55812: update 7550 DAC mapping
 *
 * Hydra_Software_Devel/68   7/31/09 6:48p syang
 * PR 55812:  add 7550 support
 *
 * Hydra_Software_Devel/67   7/10/09 5:53p rpan
 * PR55188: Convert scanf() to BTST_P_Scanf().
 *
 * Hydra_Software_Devel/66   5/7/09 5:01p jessem
 * PR 53506: Removed test feature when creating gfx surfaces.
 *
 * Hydra_Software_Devel/65   5/1/09 4:33p jessem
 * PR 53506: Added test feature when creating gfx surfaces.
 *
 * Hydra_Software_Devel/64   4/23/09 11:39a pntruong
 * PR53960: Inform HDDVI of HDMI Pixel Decimation and HdmiMode via the
 * HDDVI picture struct.
 *
 * Hydra_Software_Devel/63   4/8/09 1:56p tdo
 * PR45785, PR45789: Add ANR support for 7420
 *
 * Hydra_Software_Devel/62   4/7/09 10:03a tdo
 * PR53239: Remove usage of private definitions in vdc unittest
 *
 * Hydra_Software_Devel/61   4/6/09 2:56p yuxiaz
 * PR46252: Need to expose 444 format in static buffers case.
 *
 * Hydra_Software_Devel/60   3/31/09 6:49p hongtaoz
 * PR53037, PR53038, PR53687, PR53688: added random isr drop test to
 * vdc_test;
 *
 * Hydra_Software_Devel/59   3/31/09 5:25p hongtaoz
 * PR53687, PR53037, PR53688, PR53038: set VDC default display rate as
 * 59.94;
 *
 * Hydra_Software_Devel/58   3/30/09 3:21p hongtaoz
 * PR53037, PR53038: added forced synclock mode and double-buffer support;
 * added more VEC locking rates support;
 *
 * Hydra_Software_Devel/57   3/25/09 5:54p pntruong
 * PR52524: Used the index determined by upstream block.
 *
 * Hydra_Software_Devel/56   3/20/09 4:37p pntruong
 * PR52524: Pass RgbQuantization info from AviInfoFrame to hddvi for
 * proper colorimetry loading.
 *
 * Hydra_Software_Devel/55   3/17/09 1:58p darnstein
 * PR53265: Remove BVBI, BVBIlib software. It never worked.
 *
 * Hydra_Software_Devel/54   3/16/09 10:59p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/5   3/16/09 5:39p darnstein
 * PR53265: Remove BVBI private references.
 *
 * Hydra_Software_Devel/7420_mcvp/4   3/13/09 11:15a tdo
 * PR45785, PR45789: merge from main branch on 3/13/09
 *
 * Hydra_Software_Devel/53   3/13/09 10:33a pntruong
 * PR51930: Use the vdc default framerate.
 *
 * Hydra_Software_Devel/7420_mcvp/3   3/6/09 1:34p syang
 * PR 45796: add gfd scl (coeff) mode test
 *
 * Hydra_Software_Devel/7420_mcvp/2   2/18/09 6:40p syang
 * PR 45785: fix 3556 compile warning
 *
 * Hydra_Software_Devel/7420_mcvp/1   2/13/09 6:01p tdo
 * PR 45785, PR 45789: merge from main branch on 2/13/09
 *
 * Hydra_Software_Devel/52   2/13/09 4:23p tdo
 * PR48866: Fix warnings
 *
 * Hydra_Software_Devel/51   2/13/09 4:19p tdo
 * PR48866: Bringup gfxmfd in vdc_test for MFD on MEMC1
 *
 * Hydra_Software_Devel/50   12/24/08 9:28a tdo
 * PR50391: Tight CMP_1_V1 to MEMC_0
 *
 * Hydra_Software_Devel/49   12/23/08 9:16p tdo
 * PR50391: Bringup MEMC0 and MEMC1 in vdc_test
 *
 * Hydra_Software_Devel/48   12/17/08 7:46p tdo
 * PR50391: Remove debug code
 *
 * Hydra_Software_Devel/47   12/15/08 10:24p tdo
 * PR50391: Basic VDC bringup
 *
 * Hydra_Software_Devel/46   10/1/08 4:54p yuxiaz
 * PR47303: Add BSUR API to Support Gfx Compression Usage
 *
 * Hydra_Software_Devel/45   9/12/08 1:02p pntruong
 * PR46514: Dynamic power management of VAFE4 (ADCs).
 *
 * Hydra_Software_Devel/44   9/2/08 11:09a pntruong
 * PR44362: Corrected msg printing.
 *
 * Hydra_Software_Devel/43   8/5/08 8:04p pntruong
 * PR44362: Worked around hdr handing back bad pointers.
 *
 * Hydra_Software_Devel/42   8/5/08 7:57p pntruong
 * PR44362: Added HDR callback to get correct colorspace from avi
 * information frame packet.
 *
 * Hydra_Software_Devel/41   7/30/08 7:47p pntruong
 * PR44362: Added hdr callback to get colorspace for hddvi.
 *
 * Hydra_Software_Devel/40   7/23/08 7:02p tdo
 * PR43508, PR43509: Mapping individual bandgap adjustment for each DAC
 *
 * Hydra_Software_Devel/39   6/24/08 10:29a pntruong
 * PR34239:  Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/38   6/19/08 2:35p rpan
 * PR43712: Add SCART-SVideo test case.
 *
 * Hydra_Software_Devel/37   6/19/08 12:24p rpan
 * PR43712: Add SCART test cases.
 *
 * Hydra_Software_Devel/36   6/17/08 2:29p rpan
 * PR43668: Adopt the new LVD PI for 3548/3556.
 *
 * Hydra_Software_Devel/35   6/13/08 11:20a yuxiaz
 * PR 39209:  Temporary fix for stack problem on 7325.
 *
 * Hydra_Software_Devel/34   6/12/08 2:03p yuxiaz
 * PR 39209:  Temporary fix for stack problem on 7405.
 *
 * Hydra_Software_Devel/33   6/11/08 8:30p pntruong
 * PR39321: Use the correct handle to attached.
 *
 * Hydra_Software_Devel/32   6/10/08 3:17p syang
 * PR 39209: fix a typo (480 -> 1080)
 *
 * Hydra_Software_Devel/31   6/10/08 2:03p syang
 * PR 39209:  use gfxmfd to test lBOX with 1080p input
 *
 * Hydra_Software_Devel/30   6/9/08 10:53a rpan
 * PR39529: Added 1080p48 test case.
 *
 * Hydra_Software_Devel/29   6/7/08 6:12p pntruong
 * PR39321: Adapt to new HDR api changes.
 *
 * Hydra_Software_Devel/28   6/7/08 11:40a rpan
 * PR39159: Added ADC-DAC pass through test case.
 *
 * Hydra_Software_Devel/27   6/6/08 7:07p yuxiaz
 * PR39807: Use BFMT_CUSTOM_PANEL_TIMING instead of BVDC_DYNAMICS_RTS.
 *
 * Hydra_Software_Devel/26   6/6/08 6:10p yuxiaz
 * PR39807: More HDM output format setting for WXGA panel.
 *
 * Hydra_Software_Devel/25   6/6/08 11:34a rpan
 * PR39807: Updated 3548/3556 HDM output format setting.
 *
 * Hydra_Software_Devel/24   6/6/08 11:24a rpan
 * PR42109: Added tests for all SECAM variants.
 *
 * Hydra_Software_Devel/23   6/5/08 1:04p rpan
 * PR39807: Correct 3556/3548 HDMI DVO output format.
 *
 * Hydra_Software_Devel/22   5/27/08 7:22p hongtaoz
 * PR42248, PR42494: added display alignment test;
 *
 * Hydra_Software_Devel/21   5/5/08 12:04p rpan
 * PR39807: Enabled DVPO support for 3548/3556.
 *
 * Hydra_Software_Devel/20   3/21/08 2:24p hongtaoz
 * PR39154: added MAD/ANR support for 3548;
 *
 * Hydra_Software_Devel/19   3/20/08 11:23a pntruong
 * PR39154: Used PI default settings for mad/anr.
 *
 * Hydra_Software_Devel/18   3/20/08 10:26a hongtaoz
 * PR39154: updated MAD/ANR settings with pixel format;
 *
 * Hydra_Software_Devel/17   3/19/08 5:31p pntruong
 * PR38442: Turn on input format autodetect by default for hdmi/vdec
 * source.
 *
 * Hydra_Software_Devel/16   12/10/07 4:03p pntruong
 * PR38046: Added work-around by preventing HD_DVI_1 connecting to drain 1
 * and 2.
 *
 * Hydra_Software_Devel/15   10/25/07 9:46p pntruong
 * PR33014: Adapt to new tmr handle requirement from hdr.
 *
 * Hydra_Software_Devel/14   10/19/07 11:06a rpan
 * PR34857: Fixed a window handle assignment issue.
 *
 * Hydra_Software_Devel/13   10/17/07 10:03a rpan
 * PR34857: Fixed the "og" command.
 *
 * Hydra_Software_Devel/12   10/16/07 5:25p rpan
 * PR34857: Fixed an XVD channel handle assignment bug.
 *
 * Hydra_Software_Devel/11   10/16/07 2:44p rpan
 * PR34857: First version after merging the latest xvd_vdc_test and
 * mvd_vdc_test as of Oct. 16, 2007, 12pm.
 *
 *
 * Hydra_Software_Devel/1   9/21/07 4:39p rpan
 * PR34857: Initial revision.
 *
 *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "config.h"
#include "test_util.h"
#include "commands.h"
#include "mpg_decoder.h"
#include "bvdc_dbg.h"
#include "bkni_multi.h"
#include "bmem_debug.h"
#include "bdbg.h"
#include "bstd_defs.h"
#include "vdec_input.h"
#include "framework_board_bvdc.h"

#if (BTST_P_HACK_MFD_RTS)
#include "bchp_memc_0.h"
#endif

#if (BTST_P_USE_CUSTOM_ASPECT_RATIO)
#define BTST_P_CustomAspectRatioCorrection(hDisp0, hDisp1, hCmp0V0, hCmp1V0, pMvdField) \
    aspect_conversion(hCmp0V0, hCmp1V0, pMvdField)
#else
#define BTST_P_CustomAspectRatioCorrection(hDisp0, hDisp1, hCmp0V0, hCmp1V0, pMvdField)
#endif

BDBG_MODULE(BTST);


void BTST_P_Cmp0Select( BTST_P_Context  *pContext )
{
    pContext->iSelectedComp   = 0 /* Todo: Change to enum */ ;
}

void BTST_P_Cmp1Select( BTST_P_Context  *pContext )
{
    pContext->iSelectedComp   = 1;
}

void BTST_P_Cmp2Select( BTST_P_Context  *pContext )
{
    pContext->iSelectedComp   = 2;
}

void BTST_P_Cmp3Select( BTST_P_Context  *pContext )
{
    pContext->iSelectedComp   = 3;
}

void BTST_P_Cmp4Select( BTST_P_Context  *pContext )
{
    pContext->iSelectedComp   = 4;
}

void BTST_P_Cmp5Select( BTST_P_Context  *pContext )
{
    pContext->iSelectedComp   = 5;
}
void BTST_P_Mpeg0Select( BTST_P_Context *pContext )
{
    pContext->iSelectedVidSrc = BAVC_SourceId_eMpeg0;
}

void BTST_P_Mpeg1Select( BTST_P_Context *pContext )
{
    pContext->iSelectedVidSrc = BAVC_SourceId_eMpeg1;
}

void BTST_P_Mpeg2Select( BTST_P_Context *pContext )
{
    pContext->iSelectedVidSrc = BAVC_SourceId_eMpeg2;
}
void BTST_P_Mpeg3Select( BTST_P_Context *pContext )
{
    pContext->iSelectedVidSrc = BAVC_SourceId_eMpeg3;
}

void BTST_P_Vfd0Select( BTST_P_Context *pContext )
{
    pContext->iSelectedVidSrc = BAVC_SourceId_eVfd0;
}

void BTST_P_Vfd1Select( BTST_P_Context *pContext )
{
    pContext->iSelectedVidSrc = BAVC_SourceId_eVfd1;
}

void BTST_P_Vfd2Select( BTST_P_Context *pContext )
{
    pContext->iSelectedVidSrc = BAVC_SourceId_eVfd2;
}

void BTST_P_Vfd3Select( BTST_P_Context *pContext )
{
    pContext->iSelectedVidSrc = BAVC_SourceId_eVfd3;
}

void BTST_P_Vfd4Select( BTST_P_Context *pContext )
{
    pContext->iSelectedVidSrc = BAVC_SourceId_eVfd4;
}
void BTST_P_Vfd5Select( BTST_P_Context *pContext )
{
    pContext->iSelectedVidSrc = BAVC_SourceId_eVfd5;
}
void BTST_P_Analog0Select( BTST_P_Context *pContext )
{
    pContext->iSelectedVidSrc = BAVC_SourceId_eVdec0;
}

void BTST_P_Analog1Select( BTST_P_Context *pContext )
{
    pContext->iSelectedVidSrc = BAVC_SourceId_eVdec1;
}

void BTST_P_656in0Select( BTST_P_Context *pContext )
{
    pContext->iSelectedVidSrc = BAVC_SourceId_e656In0;
}

void BTST_P_656in1Select( BTST_P_Context *pContext )
{
    pContext->iSelectedVidSrc = BAVC_SourceId_e656In1;
}

void BTST_P_Gfx0Select( BTST_P_Context *pContext )
{
    pContext->iSelectedVidSrc = BAVC_SourceId_eGfx0;
}

void BTST_P_Gfx1Select( BTST_P_Context *pContext )
{
    pContext->iSelectedVidSrc = BAVC_SourceId_eGfx1;
}

void BTST_P_Gfx2Select( BTST_P_Context *pContext )
{
    pContext->iSelectedVidSrc = BAVC_SourceId_eGfx2;
}

void BTST_P_Gfx3Select( BTST_P_Context *pContext )
{
    pContext->iSelectedVidSrc = BAVC_SourceId_eGfx3;
}

void BTST_P_Gfx4Select( BTST_P_Context *pContext )
{
    pContext->iSelectedVidSrc = BAVC_SourceId_eGfx4;
}

void BTST_P_Gfx5Select( BTST_P_Context *pContext )
{
    pContext->iSelectedVidSrc = BAVC_SourceId_eGfx5;
}

void BTST_P_Hddvi0Select( BTST_P_Context *pContext )
{
    pContext->iSelectedVidSrc = BAVC_SourceId_eHdDvi0;
}

void BTST_P_Hddvi1Select( BTST_P_Context *pContext )
{
    pContext->iSelectedVidSrc = BAVC_SourceId_eHdDvi1;
}

void BTST_P_HdrFe0Select( BTST_P_Context *pContext )
{
    pContext->iSelectedHfe = 0;
}

void BTST_P_HdrFe1Select( BTST_P_Context *pContext )
{
    pContext->iSelectedHfe = 1;
}

void BTST_P_Ds0Select( BTST_P_Context *pContext )
{
    pContext->iSelectedVidSrc = BAVC_SourceId_eDs0;
}


void BTST_P_Win0Select ( BTST_P_Context *pContext )
{
    pContext->iSelectedWin = 0;
}

void BTST_P_Win1Select( BTST_P_Context *pContext )
{
    pContext->iSelectedWin = 1;
}

void BTST_P_Win2Select( BTST_P_Context *pContext )
{
    pContext->iSelectedWin = 2;
}

void BTST_P_Win3Select( BTST_P_Context *pContext )
{
    pContext->iSelectedWin = 3;
}

void BTST_P_Win4Select( BTST_P_Context *pContext )
{
    pContext->iSelectedWin = 4;
}

void BTST_P_Mvd0Select( BTST_P_Context *pContext )
{
    pContext->eSelectedMDec = BTST_MVD_0;
}

void BTST_P_Xvd0Select( BTST_P_Context *pContext )
{
    pContext->eSelectedMDec = BTST_XVD_0;
}

void BTST_P_Xvd1Select( BTST_P_Context *pContext )
{
#if (BFramework_NUM_XVD_CORES >1)
    pContext->eSelectedMDec = BTST_XVD_1;
#else
    BSTD_UNUSED(pContext);
    printf("Error: Chip only supports 1 XVD\n");
#endif
}

void BTST_P_MpgChannelSelect( BTST_P_Context *pContext )
{
    printf("Enter MPEG Decoder Channel Id: ");
    BTST_P_Scanf(pContext, "%u", &pContext->iSelectedMDecCh);
}

void BTST_P_MpgDmSelect( BTST_P_Context *pContext )
{
	printf("Enter MPEG Decoder Display Manager Number (multi-decode single core may feed two MFDs or DMs): ");
	BTST_P_Scanf(pContext, "%u", &pContext->iSelectedMDecDm);
	pContext->iSelectedMDecDm = pContext->iSelectedMDecDm? 1 : 0;
}
void BTST_P_Mem0Select( BTST_P_Context        *pContext )
{
    pContext->iSelectedMem  = 0;
}

void BTST_P_Mem1Select
    ( BTST_P_Context          *pContext )
{
    pContext->iSelectedMem  = 1;
}

void BTST_P_SharedMemSelect
    ( BTST_P_Context          *pContext )
{
    pContext->iSelectedMem  = 2;
}


/***************************************************************************
 *
 */
void BTST_P_OpenVdcMisc
    ( BTST_P_Context          *pContext )
{
    if(!pContext->hVdc)
    {
        BMEM_HeapInfo stHeapStart, stHeapEnd;
        BERR_Code eStatus;

        BTST_MemInfo *pDefMem = &pContext->astMemInfo[s_CoreMemId[BTST_P_eDEF_MEM].ulMemcId];
        BTST_MemInfo *pVdcMem = &pContext->astMemInfo[s_CoreMemId[BTST_P_eVDC_MEM].ulMemcId];

        BMEM_GetHeapInfo(pDefMem->hMem, &stHeapStart);
        if(!pContext->bResourceLeakAutoTest) {
            printf("Before: available memory = %d bytes\n", stHeapStart.ulLargestFree);
            printf("bVecSwap= %d\n", pContext->stSettings.bVecSwap);
        }

        pContext->stSettings.eDisplayFrameRate = BAVC_FrameRateCode_e59_94;
        eStatus = BVDC_Open(&pContext->hVdc, pContext->hChp, pContext->hReg,
            pDefMem->hMem, pContext->hInt, pContext->hRdc, pContext->hTmr,
            &pContext->stSettings);

        if(eStatus != BERR_SUCCESS)
            return;

        BVDC_GetCapabilities(pContext->hVdc, &pContext->stCapabilities);
        BMEM_GetHeapInfo(pDefMem->hMem, &stHeapEnd);
        if(!pContext->bResourceLeakAutoTest) {
        printf("After : available memory = %d bytes\n", stHeapEnd.ulLargestFree);
        printf("VDC memory usage: %d bytes\n",
            stHeapStart.ulLargestFree - stHeapEnd.ulLargestFree);
		}

        if(pVdcMem->hMem)
        {
            if(!pContext->bResourceLeakAutoTest) printf("\nSecondary Memory: \n");
            BMEM_GetHeapInfo(pVdcMem->hMem, &stHeapStart);
            if(!pContext->bResourceLeakAutoTest) printf("Before: available memory = %d bytes\n", stHeapStart.ulLargestFree);

            BVDC_Heap_Create(pContext->hVdc, &pVdcMem->hVdcHeap, pVdcMem->hMem,
                &pVdcMem->stVdcHeapSettings);

            BMEM_GetHeapInfo(pVdcMem->hMem, &stHeapEnd);
            if(!pContext->bResourceLeakAutoTest) {
            printf("After : available memory = %d bytes\n", stHeapEnd.ulLargestFree);
            printf("VDC memory usage: %d bytes\n",
                stHeapStart.ulLargestFree - stHeapEnd.ulLargestFree);
            }
        }
        else
        {
            pVdcMem->hVdcHeap = NULL;
            printf("set hVdcHeap to NULL\n");
        }
    }

    BTST_P_PrintStatus(pContext);
    return;
}


/***************************************************************************
 *
 */
void BTST_P_CloseVdcMisc
    ( BTST_P_Context          *pContext )
{
    BTST_MemInfo *pVdcMem = &pContext->astMemInfo[s_CoreMemId[BTST_P_eVDC_MEM].ulMemcId];

    if(pContext->hVdc)
    {
        if(pVdcMem->hVdcHeap)
        {
            BVDC_Heap_Destroy(pVdcMem->hVdcHeap);
        }

        BVDC_Close(pContext->hVdc);
        pContext->hVdc = NULL;
    }

    BTST_P_PrintStatus(pContext);
    return;
}


static void Change_Composite
    ( BTST_P_Context          *pContext,
      bool                    bOpen)
{

    BVDC_Display_Handle    hDisplay    = pContext->ahDisplay[pContext->iSelectedComp];
    BVDC_DacOutput         eDacOutput  = bOpen?  BVDC_DacOutput_eComposite : BVDC_DacOutput_eUnused;

    BVDC_Display_SetDacConfiguration( hDisplay,
        (pContext->iSelectedComp == BVDC_CompositorId_eCompositor2)? BFramework_Dac_Composite_1: BFramework_Dac_Composite_0,
        eDacOutput);

    pContext->bComposite[pContext->iSelectedComp] = bOpen;
}

static void Change_Component
    ( BTST_P_Context          *pContext,
      bool                    bOpen)
{
    BVDC_Display_Handle    hDisplay    = pContext->ahDisplay[pContext->iSelectedComp];
    BVDC_DacOutput         eDacOutputPr  = bOpen?  BVDC_DacOutput_ePr : BVDC_DacOutput_eUnused;
    BVDC_DacOutput         eDacOutputY   = bOpen?  BVDC_DacOutput_eY :  BVDC_DacOutput_eUnused;
    BVDC_DacOutput         eDacOutputPb  = bOpen?  BVDC_DacOutput_ePb : BVDC_DacOutput_eUnused;

    BVDC_Display_SetDacConfiguration(
            hDisplay, BFramework_Dac_Component_pr, eDacOutputPr);
    BVDC_Display_SetDacConfiguration(
        hDisplay, BFramework_Dac_Component_y,  eDacOutputY);
    BVDC_Display_SetDacConfiguration(
        hDisplay, BFramework_Dac_Component_pb, eDacOutputPb);

    pContext->bComponent[pContext->iSelectedComp] = bOpen;
}

static void Change_SVideo
    ( BTST_P_Context          *pContext,
      bool                    bOpen)
{
    BVDC_Display_Handle    hDisplay         = pContext->ahDisplay[pContext->iSelectedComp];
    BVDC_DacOutput         eDacOutputChroma = bOpen?  BVDC_DacOutput_eSVideo_Chroma : BVDC_DacOutput_eUnused;
    BVDC_DacOutput         eDacOutputLuma   = bOpen?  BVDC_DacOutput_eSVideo_Luma : BVDC_DacOutput_eUnused;

    BVDC_Display_SetDacConfiguration( hDisplay,
                    BFramework_Dac_Svideo_Chroma, eDacOutputChroma);
    BVDC_Display_SetDacConfiguration( hDisplay,
                    BFramework_Dac_Svideo_Luma, eDacOutputLuma);

    pContext->bSVideo[pContext->iSelectedComp] = bOpen;
}

static void Change_Vdec
    ( BTST_P_Context             *pContext,
      bool                       bOpen)
{
    pContext->bVdec[pContext->iSelectedComp] = bOpen;
}

static void Set_Display_BGColor(BTST_P_Context          *pContext)
{
    uint32_t                ulSelectedComp = pContext->iSelectedComp;
    BVDC_Compositor_Handle  hCompositor = pContext->ahCompositor[ulSelectedComp];
    uint32_t                ulRed =0, ulBlue = 0, ulGreen=0;

    switch(ulSelectedComp)
    {
        case BVDC_CompositorId_eCompositor0:
        default:
            ulRed   = 0xff;
            ulBlue  = 0x0;
            ulGreen = 0x0;
            break;
        case BVDC_CompositorId_eCompositor1:
            ulRed   = 0x0;
            ulBlue  = 0xff;
            ulGreen = 0x0;
            break;
        case BVDC_CompositorId_eCompositor2:
            ulRed   = 0xff;
            ulBlue  = 0x0;
            ulGreen = 0xff;
            break;
    }

    BVDC_Compositor_SetBackgroundColor(hCompositor, (uint8_t)ulRed, (uint8_t)ulGreen, (uint8_t)ulBlue);
}

void Open_OutputVideo
    ( BTST_P_Context          *pContext )
{
    uint32_t               ulSelectedComp = pContext->iSelectedComp;
	uint32_t               ulMaxDac = 0; /* dac number supported on this board */
    BVDC_DacOutput          eDacPr=BVDC_DacOutput_eUnused;
    BVDC_Display_Handle     hDisplay    = pContext->ahDisplay[ulSelectedComp];
    BVDC_Display_Settings *pDispSettings = &pContext->stDispSettings[ulSelectedComp];
    Set_Display_BGColor(pContext);

	if(BTST_P_USED_DIGTRIG (pDispSettings->eMasterTg))
		return;

	ulMaxDac =  pContext->stCapabilities.ulNumDac;

	
    switch(ulSelectedComp)
    {
        case BVDC_CompositorId_eCompositor0:
        /* Enable Component*/
        Change_Component(pContext, true);

        /*Enable Composite*/
        if(4<=ulMaxDac)
        {
            Change_Composite(pContext, true);
        }

        /*Enable SVideo*/
        if((6<=ulMaxDac) && (BFramework_Dac_Svideo_Luma))
        {
            Change_SVideo(pContext, true);
        }
        
        break;


        case BVDC_CompositorId_eCompositor1:
            /*Enable SVideo*/
            if((6<=ulMaxDac) && (BFramework_Dac_Svideo_Luma))
            {
                BVDC_Display_GetDacConfiguration(
                    hDisplay, BFramework_Dac_Svideo_Luma, &eDacPr);
                if (eDacPr == BVDC_DacOutput_eUnused)
                	Change_SVideo(pContext, true);
            }

            /* Enable Composite*/
            BVDC_Display_GetDacConfiguration(
                hDisplay, BFramework_Dac_Composite_0, &eDacPr);
            if (eDacPr == BVDC_DacOutput_eUnused)
                Change_Composite(pContext, true);

            break;

        case BVDC_CompositorId_eCompositor2:
        /*Enable Composite*/
        if((pContext->stCapabilities.ulNumCmp>=BVDC_CompositorId_eCompositor2) && 
           (BFramework_Dac_Composite_0))
        {
            BVDC_Display_GetDacConfiguration(
                hDisplay, BFramework_Dac_Composite_0, &eDacPr);
            if (eDacPr == BVDC_DacOutput_eUnused)
                Change_Composite(pContext, true);
        }
            break;

        case BVDC_CompositorId_eCompositor3:
		case BVDC_CompositorId_eCompositor4:
		case BVDC_CompositorId_eCompositor5:
            break;
        default:
            BDBG_ERR(("Unsupported Compositor "));

    }
}

void Close_OutputVideo
    ( BTST_P_Context          *pContext )
{
    uint32_t i;

/* set the display's dac as unused */
    for (i=0; i< pContext->stCapabilities.ulNumCmp; i++)
    {
        if(pContext->bComposite[i])
        {
            Change_Composite(pContext, false);
        }

        if(pContext->bComponent[i])
        {
            Change_Component(pContext, false);
        }

        if(pContext->bSVideo[i])
        {
            Change_SVideo(pContext, false);
        }
        if(pContext->bVdec[i])
        {
            Change_Vdec(pContext, false);
        }
    }
}


void BTST_P_ChangeDispVideo
    (BTST_P_Context           *pContext)
{
    uint32_t               ulSelectedComp = pContext->iSelectedComp;
    BVDC_Display_Handle    hDisplay       = pContext->ahDisplay[pContext->iSelectedComp];
    bool                   bCompositeDisp = false, bComponentDisp = false;
    bool                   bSVideoDisp    = false, bVdec = false;
    int                    iResponse;

    if(!hDisplay)
    {
        return;
    }

    printf("\n Please enter new setting for output: Enable(1), Disable(0)\n");
    printf("\n Enable Composite display?: \n");
    BTST_P_Scanf(pContext, "%d", &iResponse);
    bCompositeDisp = (iResponse != 0);
    pContext->bComposite[ulSelectedComp] = bCompositeDisp;
    Change_Composite (pContext, bCompositeDisp);

    printf("\n Enable Component display?: \n");
    BTST_P_Scanf(pContext, "%d", &iResponse);
    bComponentDisp = (iResponse != 0);
    pContext->bComponent[ulSelectedComp] = bComponentDisp;
    Change_Component (pContext, bComponentDisp);

    printf("\n Enable SVideo display?: \n");
    BTST_P_Scanf(pContext, "%d", &iResponse);
    bSVideoDisp = (iResponse != 0);
    pContext->bSVideo[ulSelectedComp] = bSVideoDisp;
    Change_SVideo(pContext, bSVideoDisp);

    printf("\n Enable Bypass Vdec?: \n");
    BTST_P_Scanf(pContext, "%d", &iResponse);
    bVdec = (iResponse != 0);
    pContext->bVdec[ulSelectedComp] = bVdec;
    Change_Vdec(pContext, bVdec);

    BTST_P_ApplyChanges(pContext);
}
/***************************************************************************
 *
 */
static void Open_Display
    ( BTST_P_Context          *pContext )
{
    int iChoice;
    BVDC_Display_Settings *pDispSettings = &pContext->stDispSettings[pContext->iSelectedComp];
    BVDC_Compositor_Handle hCompositor = pContext->ahCompositor[pContext->iSelectedComp];
    BVDC_Display_Handle    hDisplay    = pContext->ahDisplay[pContext->iSelectedComp];
    BVDC_DisplayId eDisplayId = BVDC_DisplayId_eAuto;

    if((!pContext->hVdc) || (hDisplay))
    {
        goto done;
    }

    pContext->abMasterStg[pContext->iSelectedComp] = false;
    if(pContext->bCustomDisplay)
    {
        printf("Please select display ID for compositor %d: [0 | 1 | 2 | 3 | 4 | 5 |]\n", pContext->iSelectedComp);
        BTST_P_Scanf(pContext, "%d", &iChoice);
        eDisplayId = (iChoice > BVDC_DisplayId_eAuto) ? BVDC_DisplayId_eAuto : (BVDC_DisplayId)iChoice;

        BVDC_Display_GetDefaultSettings(eDisplayId, pDispSettings);
        printf("Default master TG for display %d is %s\n", eDisplayId,
            (pDispSettings->eMasterTg == BVDC_DisplayTg_ePrimIt)?
            "BVDC_DisplayTg_ePrimIt" : ((pDispSettings->eMasterTg == BVDC_DisplayTg_eSecIt)?
            "BVDC_DisplayTg_eSecIt" :  ((pDispSettings->eMasterTg == BVDC_DisplayTg_eTertIt)?
            "BVDC_DisplayTg_eTertIt" : ((pDispSettings->eMasterTg == BVDC_DisplayTg_e656Dtg)?
            "BVDC_DisplayTg_e656Dtg" : ((pDispSettings->eMasterTg == BVDC_DisplayTg_eDviDtg)?
            "BVDC_DisplayTg_eDviDtg" : ((pDispSettings->eMasterTg == BVDC_DisplayTg_eStg0)?
            "BVDC_DisplayTg_eStg0"   : ((pDispSettings->eMasterTg == BVDC_DisplayTg_eStg1)?
            "BVDC_DisplayTg_eStg1"   : ((pDispSettings->eMasterTg == BVDC_DisplayTg_eStg2)?
            "BVDC_DisplayTg_eStg2"   :"BVDC_DisplayTg_eStg3"   ))))))));
        printf("Please select the master TG for Display %d: (0) PrimIt; (1) SecIt; (2) TertIt, (3)DviDtg; (4)656Dtg; (5) STG_0; (6) STG_1; (7) STG_2; (8) STG_3;\n", eDisplayId);
        BTST_P_Scanf(pContext, "%d", &iChoice);
        iChoice = (iChoice > BVDC_DisplayTg_eStg3)? 0 : iChoice;
        pContext->eMasterTg[pContext->iSelectedComp] =
			pDispSettings->eMasterTg = (BVDC_DisplayTg)iChoice;

		printf ("Default setting for ARIB style output for display %d is %s\n",
			eDisplayId, (pDispSettings->bArib480p ? "enabled" : "disabled"));
		printf ("Enable ARIB style output for Display %d? (0) no; (1) yes\n",
			eDisplayId);
        BTST_P_Scanf(pContext, "%d", &iChoice);
        iChoice = (iChoice > 1) ? 0 : iChoice;
		pDispSettings->bArib480p = (bool)iChoice;

        if(BERR_SUCCESS != BVDC_Display_Create(
			hCompositor, &hDisplay, eDisplayId, pDispSettings))
        {
            goto done;
        }

        if(BTST_P_USED_STG(pDispSettings->eMasterTg))
        {
			BVDC_Display_StgSettings stStgSettings;
			printf("\n Running transcoding at non-real-time (NRT) mode (0) false (1) true \n");
			BTST_P_Scanf(pContext, "%d", &iChoice);
			stStgSettings.bNonRealTime  = (bool)iChoice;
            pContext->abMasterStg[pContext->iSelectedComp] = true;
            BVDC_Display_SetStgConfiguration(hDisplay, true, &stStgSettings);
        }
    }
    else
    {
        /* Takes back */
        if(BVDC_CompositorId_eCompositor1 == pContext->iSelectedComp && pContext->ahDisplay[0])
        {
            BVDC_Display_SetDacConfiguration(pContext->ahDisplay[0],
                BFramework_Dac_Composite_0 | BFramework_Dac_Svideo_Chroma |
                    BFramework_Dac_Svideo_Luma,
                BVDC_DacOutput_eUnused);
            BTST_P_ApplyChanges(pContext);
        }
        else if(BVDC_CompositorId_eCompositor0 == pContext->iSelectedComp && pContext->ahDisplay[1])
        {
            BVDC_Display_SetDacConfiguration(pContext->ahDisplay[1],
                BFramework_Dac_Component_y | BFramework_Dac_Component_pr |
                    BFramework_Dac_Component_pb,
                BVDC_DacOutput_eUnused);
            BTST_P_ApplyChanges(pContext);
        }
        BVDC_Display_Create(hCompositor, &hDisplay, BVDC_DisplayId_eAuto, NULL);

        BTST_P_ApplyChanges(pContext);
    }

#if BTST_P_SUPPORT_LVD
    if(pDispSettings->eMasterTg == BVDC_DisplayTg_eDviDtg)
    {
        BVDC_Display_SetVideoFormat(hDisplay, BFMT_VideoFmt_eCustom0);
        pContext->apFmtInfo[pContext->iSelectedComp] =
            BFMT_GetVideoFormatInfoPtr(BFMT_VideoFmt_eCustom0);
    }
    else
#endif
    {
        BVDC_Display_SetVideoFormat(hDisplay, BFMT_VideoFmt_eNTSC);
        pContext->apFmtInfo[pContext->iSelectedComp] =
            BFMT_GetVideoFormatInfoPtr(BFMT_VideoFmt_eNTSC);
    }

    pContext->ahDisplay[pContext->iSelectedComp] = hDisplay;
    Open_OutputVideo(pContext);

done:
    return;
}


/***************************************************************************
 *
 */
static void Close_Display
    ( BTST_P_Context          *pContext )
{
    BVDC_Display_Handle    hDisplay    = pContext->ahDisplay[pContext->iSelectedComp];

    if((!hDisplay))
    {
        goto done;
    }

    if (pContext->eMasterTg[pContext->iSelectedComp] == BVDC_DisplayTg_eDviDtg)
    {
#if (BTST_P_SUPPORT_HDM)
		if(NULL != pContext->hHdm)
			BHDM_SetHdmiDataTransferMode(pContext->hHdm, false);
#endif
        BVDC_Display_SetHdmiConfiguration (hDisplay, BVDC_Hdmi_0,BAVC_MatrixCoefficients_eUnknown);
    }

    Close_OutputVideo(pContext);

    /* in order to make the above dac to unused setting effective, this applyChange
     * before destroying hDisplay and hCompositor is needed */
    BTST_P_ApplyChanges(pContext);

    BVDC_Display_Destroy(hDisplay);

    pContext->ahDisplay[pContext->iSelectedComp]    = NULL;
    pContext->abMasterStg[pContext->iSelectedComp]  = false;

done:

    return;
}



/***************************************************************************
 *
 */
void BTST_P_OpenComp
    ( BTST_P_Context          *pContext )
{
    BVDC_Compositor_Handle hCompositor = pContext->ahCompositor[pContext->iSelectedComp];
    BVDC_Display_Handle    hDisplay    = pContext->ahDisplay[pContext->iSelectedComp];
    BVDC_Compositor_Settings  stCompSettings;

    if((!pContext->hVdc) || (hCompositor) || (hDisplay))
    {
        goto done;
    }

    BVDC_Compositor_GetDefaultSettings(pContext->iSelectedComp, &stCompSettings);
    BVDC_Compositor_Create(pContext->hVdc, &hCompositor, pContext->iSelectedComp, &stCompSettings);

    pContext->ahCompositor[pContext->iSelectedComp] = hCompositor;
    pContext->eGraphicsFormat = BPXL_eR5_G6_B5;

    Open_Display(pContext);

    if(pContext->bApplyImmediately)
    {
        BTST_P_ApplyChanges(pContext);
    }

done:
    BTST_P_PrintStatus(pContext);
    return;
}


/***************************************************************************
 *
 */
void BTST_P_CloseComp
    ( BTST_P_Context          *pContext )
{
    BVDC_Compositor_Handle hCompositor = pContext->ahCompositor[pContext->iSelectedComp];
    BVDC_Display_Handle    hDisplay    = pContext->ahDisplay[pContext->iSelectedComp];

    if((!hCompositor) || (!hDisplay))
    {
        goto done;
    }

    Close_Display(pContext);

    BVDC_Compositor_Destroy(hCompositor);
    pContext->ahCompositor[pContext->iSelectedComp] = NULL;

    if(pContext->bApplyImmediately)
    {
        BTST_P_ApplyChanges(pContext);
    }

done:

    BTST_P_PrintStatus(pContext);
    return;
}


/***************************************************************************
 *
 */
void BTST_P_OpenDisp
    ( BTST_P_Context          *pContext )
{
    Open_Display(pContext);
    return;
}


/***************************************************************************
 *
 */
void BTST_P_CloseDisp
    ( BTST_P_Context          *pContext )
{
    Close_Display(pContext);
    return;
}

/***************************************************************************
 * Note: we can only use double buffer in one compositor!!!
 */
typedef struct BTST_P_GfxDoubleBufData {
    BVDC_Source_Handle source;
    BAVC_Gfx_Picture gfx_pic;
} BTST_P_GfxDoubleBufData;

static BTST_P_GfxDoubleBufData flip_data;
static BKNI_EventHandle flip_event;

static void BTST_P_GfxDoubleBufCallBack(
    void *pvparm1,
	int parm2,
	BAVC_Polarity ePolarity,
	BAVC_SourceState eState,
	void **ppvPicture)
{
    BTST_P_GfxDoubleBufData *data = (BTST_P_GfxDoubleBufData *)pvparm1;
    BERR_Code rc;
    BAVC_Gfx_Picture        stGfxPic;
    BSUR_Surface_Handle cur_surface;

    BSTD_UNUSED(parm2);
    BSTD_UNUSED(ePolarity);
    BSTD_UNUSED(eState);
    *ppvPicture = NULL;


    rc = BVDC_Source_GetSurface_isr(data->source, &stGfxPic);
	/*printf("\n done get surface");*/
    cur_surface = stGfxPic.hSurface;
    if ( (rc == BERR_SUCCESS) && (cur_surface == data->gfx_pic.hSurface))
    {
        BKNI_SetEvent(flip_event);
    }
    else if (NULL == cur_surface)
    {
        BDBG_ERR(("Get NULL sur handle"));
    }

    *ppvPicture = &data->gfx_pic;
}

#define BTST_P_GFX_BUF_CNT (4)
static void BTST_P_GfxDoubleBuf
    ( BTST_P_Context          *pContext )
{
    BVDC_Window_Handle hGfxWin;
    uint32_t ulWidth                   = BTST_P_DEFAUL_GFX_WIDTH;
    uint32_t ulHeight                  = BTST_P_DEFAUL_GFX_HEIGHT;
    int iVidSrcId                      = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].iVidSrcId;
    BVDC_Source_Handle hGfxSrc         = pContext->aSrcContext[iVidSrcId].hVidSrc;
    BSUR_Surface_Handle hGoldSur;
    BSUR_Surface_Handle hFrntSur;
    BAVC_Gfx_Picture    stGfxPic;
    BSUR_Surface_Handle hSur[BTST_P_GFX_BUF_CNT];
    int iCntr, ii;
    BERR_Code rc;
	void   *pvAddress0, *pvAddress1, *pvAddress2, *pvAddress3;
	uint32_t ulPitch;
	BERR_Code  eResult = BERR_SUCCESS;

	printf("\n GfxDoubleBuf Comp %d Win %d Src %d", pContext->iSelectedComp, pContext->iSelectedWin, pContext->iSelectedVidSrc);

    hGfxWin = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

    rc = BSUR_Surface_Create(pContext->astMemInfo[0].hMem, ulWidth, ulHeight, 0,
        NULL, BPXL_eA8_R8_G8_B8, NULL, 0, NULL, &hGoldSur);
    BGFX_MakeImageARGB8888 (hGoldSur);
    for (ii=0; ii<BTST_P_GFX_BUF_CNT; ii++)
    {
        rc |= BSUR_Surface_Create(pContext->astMemInfo[0].hMem, ulWidth, ulHeight,
            0, NULL, BPXL_eR5_G6_B5, NULL, 0, NULL, &hSur[ii]);
        BGFX_CopyImage(hSur[ii], hGoldSur);
/*        BGFX_DumpImage(hSur[ii], 0, 0);*/
    }

    ii = 0;
    hFrntSur = hSur[0];
    flip_data.gfx_pic.hSurface = hFrntSur;
    flip_data.source = hGfxSrc;

	BKNI_Memset((void*)&stGfxPic, 0x0, sizeof(BAVC_Gfx_Picture));
    stGfxPic.hSurface = hFrntSur;
    stGfxPic.eInOrientation = BFMT_Orientation_e2D;

	eResult = BSUR_Surface_GetAddress( hSur[0], &pvAddress0, &ulPitch );
	eResult = BSUR_Surface_GetAddress( hSur[1], &pvAddress1, &ulPitch );
	eResult = BSUR_Surface_GetAddress( hSur[2], &pvAddress2, &ulPitch );
	eResult = BSUR_Surface_GetAddress( hSur[3], &pvAddress3, &ulPitch );

	printf("\n double buffer hSur[0] %8x %8x \n hSur[1] %8x %8x \n hSur[2] %8x %8x \nhSur[3] %8x %8x ",
		(uint32_t)hSur[0], (uint32_t)pvAddress0,
		(uint32_t)hSur[1], (uint32_t)pvAddress1,
		(uint32_t)hSur[2], (uint32_t)pvAddress2,
		(uint32_t)hSur[3], (uint32_t)pvAddress3);


    rc |= BVDC_Source_SetSurface( hGfxSrc, &stGfxPic);
    rc |= BTST_P_ApplyChanges(pContext);
    BDBG_ASSERT(BERR_SUCCESS == rc);

    rc |= BKNI_CreateEvent(&flip_event);
	BKNI_ResetEvent(flip_event);

    rc |= BVDC_Source_InstallPictureCallback(hGfxSrc, BTST_P_GfxDoubleBufCallBack, (void *)&flip_data, 0);
    BDBG_ASSERT(BERR_SUCCESS == rc);

    BTST_P_ApplyChanges(pContext);
    iCntr = 1600;
    while (iCntr-- > 0)
    {
        if (0 == (iCntr & 0xff)) printf("\n--- iCntr %d flip gfx buf 256 times\n", iCntr);
        BKNI_Sleep(16);
        if ( ++ii >= BTST_P_GFX_BUF_CNT ) ii = 0;
        hFrntSur = hSur[ii];
		stGfxPic.hSurface = hFrntSur;
        BVDC_Source_SetSurface( hGfxSrc, &stGfxPic );
		/*printf("\n iCntr = %d ii %d", iCntr, ii);*/
        BTST_P_ApplyChanges(pContext);


        rc = BKNI_WaitForEvent(flip_event, 1000);
        if (rc == BERR_TIMEOUT)
        {
            BDBG_ERR(("BKNI_WaitForEvent on flip_event timed out!"));
        }
		/*
        else
        {
            printf("\nDOUBLE_BUF: got flip event\n");
        }
        */
    }

    BVDC_Source_SetSurface (hGfxSrc, &stGfxPic);
    BTST_P_ApplyChanges(pContext);
    flip_data.gfx_pic.hSurface = pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface;
    rc = BKNI_WaitForEvent(flip_event, 1000);
    if (rc == BERR_TIMEOUT)
    {
        BDBG_ERR(("\nBKNI_WaitForEvent on final flip_event timed out!"));
    }
    else
    {
        printf("\nDOUBLE_BUF: got final flip event\n");
    }

    BVDC_Source_InstallPictureCallback(hGfxSrc, NULL, (void *)&flip_data, 0);
	printf("\n Done callback");
    BSUR_Surface_Destroy(hGoldSur);
	printf("\n Done Surface Destroy");

    for (ii=0; ii<BTST_P_GFX_BUF_CNT; ii++)
    {
        BSUR_Surface_Destroy(hSur[ii]);
		printf("\n Done %d", ii);
    }


    BKNI_DestroyEvent(flip_event);
	printf("\n Done Destory flip_event");

	printf("\n Leave GfxDoubleBuf");

    return;
}

/***************************************************************************
 *
 */
void GfxPullBufCallBack_isr
    ( void             *pvParm1,
      int               iParm2,
      BAVC_Polarity     ePolarity,
      BAVC_SourceState  eSourceState,
      void            **ppvPicture )
{
    BTST_P_Context *pContext = (BTST_P_Context *) pvParm1;

    BSTD_UNUSED(ePolarity);
    BSTD_UNUSED(eSourceState);

    *ppvPicture = (void *) pContext->aSrcContext[iParm2].pGfxBuf;

    if((pContext->aSrcContext[iParm2].ulCallBckMsgCnt <600) &&
       (pContext->aSrcContext[iParm2].ulCallBckMsgCnt >580))
    {
        printf("Gfx Src ISR for gfx src %d\n", iParm2);
    }
}

static void BTST_P_GfxPullBuf
    ( BTST_P_Context          *pContext )
{
    BVDC_Window_Handle hGfxWin;
    uint32_t ulWidth                   = BTST_P_DEFAUL_GFX_WIDTH;
    uint32_t ulHeight                  = BTST_P_DEFAUL_GFX_HEIGHT;
    int iVidSrcId                      = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].iVidSrcId;
    BVDC_Source_Handle hGfxSrc         = pContext->aSrcContext[iVidSrcId].hVidSrc;
    BAVC_Gfx_Picture *pGfxBuf          = pContext->aSrcContext[iVidSrcId].pGfxBuf;
    BSUR_Surface_Handle hGoldSur;
    BSUR_Surface_Handle hFrntSur;
    BAVC_Gfx_Picture    stGfxPic;
    BSUR_Surface_Handle hSur[BTST_P_GFX_BUF_CNT];
    int iCntr, ii;
    BERR_Code rc;

    hGfxWin = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

    rc = BSUR_Surface_Create(pContext->astMemInfo[0].hMem, ulWidth, ulHeight, 0,
        NULL, BPXL_eA8_R8_G8_B8, NULL, 0, NULL, &hGoldSur);
    BGFX_MakeImageARGB8888 (hGoldSur);
    for (ii=0; ii<BTST_P_GFX_BUF_CNT; ii++)
    {
        rc |= BSUR_Surface_Create(pContext->astMemInfo[0].hMem, ulWidth, ulHeight,
            0, NULL, BPXL_eR5_G6_B5, NULL, 0, NULL, &hSur[ii]);
        BGFX_CopyImage(hSur[ii], hGoldSur);
    }
    ii = 0;
    hFrntSur = hSur[0];
	BKNI_Memset((void*)&stGfxPic, 0x0, sizeof(BAVC_Gfx_Picture));
    stGfxPic.hSurface = hFrntSur;
    stGfxPic.eInOrientation = BFMT_Orientation_e2D;

    rc |= BVDC_Source_SetSurface( hGfxSrc, &stGfxPic);
    rc |= BTST_P_ApplyChanges(pContext);
    BDBG_ASSERT(BERR_SUCCESS == rc);
    BKNI_Sleep(500);

    if (!pGfxBuf)
    {
        pGfxBuf = (BAVC_Gfx_Picture *)BKNI_Malloc( sizeof(BAVC_Gfx_Picture) );
        if ( NULL == pGfxBuf )
        {
            BDBG_ERR(("failed to alloc mem for BAVC_Gfx_Picture"));
            goto GfxPullBuf_done;
        }
        BKNI_Memset((void*)pGfxBuf, 0x0, sizeof(BAVC_Gfx_Picture));
        pContext->aSrcContext[pContext->iSelectedVidSrc].pGfxBuf = pGfxBuf;
    }

    pGfxBuf->hSurface = hFrntSur;
    pGfxBuf->hAlphaSurface = NULL;
    pGfxBuf->ucW0Alpha = 0;
    pGfxBuf->ucW1Alpha = 255;
    pContext->aSrcContext[pContext->iSelectedVidSrc].ulCallBckMsgCnt = 0;
    BVDC_Source_InstallPictureCallback( hGfxSrc, GfxPullBufCallBack_isr, (void *)pContext, pContext->iSelectedVidSrc);
    printf("install gfx call back for cmp %d\n", pContext->iSelectedComp);

    iCntr = 1600;
    while (iCntr-- > 0)
    {
        if (0 == (iCntr & 0x3f)) printf("\n--- flip gfx buf 64 times\n");

        pGfxBuf->hSurface = hFrntSur;
        BKNI_Sleep(20);

        /* current front buf is on display, flip the buf */
        if ( ++ii >= BTST_P_GFX_BUF_CNT ) ii = 0;
        hFrntSur = hSur[ii];
        flip_data.gfx_pic.hSurface = hFrntSur;
    }
    BVDC_Source_InstallPictureCallback( hGfxSrc, NULL, (void *)pContext, pContext->iSelectedComp);
    printf("uninstall gfx call back for cmp %d\n", pContext->iSelectedComp);

  GfxPullBuf_done:
    stGfxPic.hSurface =  pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface;
    stGfxPic.eInOrientation = BFMT_Orientation_e2D;
    BVDC_Source_SetSurface (hGfxSrc, &stGfxPic);
    BTST_P_ApplyChanges(pContext);
    BKNI_Sleep(1000);
    BSUR_Surface_Destroy(hGoldSur);
    for (ii=0; ii<BTST_P_GFX_BUF_CNT; ii++)
    {
        BSUR_Surface_Destroy(hSur[ii]);
    }

    return;
}

void BTST_P_OpenGfxWin
    ( BTST_P_Context          *pContext )
{
    int iTmpSelectedWin    = pContext->iSelectedWin;
    int iTmpSelectedVidSrc = pContext->iSelectedVidSrc;

	
    pContext->iSelectedWin = 2;
	
    pContext->iSelectedVidSrc = BAVC_SourceId_eGfx0 + (pContext->iSelectedComp);

    BTST_P_OpenVidSrc(pContext);
    BTST_P_OpenVidWin(pContext);

    pContext->iSelectedWin    = iTmpSelectedWin;
    pContext->iSelectedVidSrc = iTmpSelectedVidSrc;

    return;
}

/***************************************************************************
 *
 */
void BTST_P_GfxWinTest
    ( BTST_P_Context          *pContext )
{
    BVDC_Window_Handle hGfxWin;
    int iVidSrcId                      = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].iVidSrcId;
    BVDC_Source_Handle hGfxSrc         = pContext->aSrcContext[iVidSrcId].hVidSrc;
    BSUR_Surface_Handle hSurface       = pContext->aSrcContext[iVidSrcId].hGfxSurface;
    BVDC_Compositor_Handle hCompositor = pContext->ahCompositor[pContext->iSelectedComp];
    const BFMT_VideoInfo *pFmtInfo     = pContext->apFmtInfo[pContext->iSelectedComp];
    uint32_t ulWidth, ulHeight, ulStepW, ulStepH;
    uint32_t ulXStart, ulYStart, ulXOffset, ulYOffset;
    int iCntr;

    hGfxWin = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

    if((!hCompositor))
    {
        goto done;
    }

    if  ((!hGfxSrc) || (!hGfxWin))
    {
        BTST_P_OpenGfxWin(pContext);
        hGfxSrc  = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
        hSurface = pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface;
        hGfxWin  = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
    }

    BVDC_Window_SetSrcClip(hGfxWin, 0, 0, 0, 0);

    iCntr = 40;
    ulWidth  = pFmtInfo->ulWidth;
    ulHeight = BTST_P_DEFAUL_GFX_HEIGHT;
    ulStepW = (ulWidth / iCntr) & ~0x1;
    ulStepH = (ulHeight / iCntr +3) & ~0x1;
    ulWidth -= 80;
    ulXStart = 0;
    ulYStart = (pFmtInfo->ulHeight - BTST_P_DEFAUL_GFX_HEIGHT)/2 & ~1;
    while (iCntr-- > 0)
    {
        if (iCntr >= 20)
        {
            ulWidth -= ulStepW;
            ulHeight -= ulStepH;
        }
        else
        {
            ulWidth += ulStepW;
            ulHeight += ulStepH;
        }
        ulXOffset = (pFmtInfo->ulWidth - ulWidth)/2;
        ulYOffset = (BTST_P_DEFAUL_GFX_HEIGHT - ulHeight)/2;
        BVDC_Window_SetScalerOutput(hGfxWin, ulXOffset, ulYOffset,
                                    pFmtInfo->ulWidth, BTST_P_DEFAUL_GFX_HEIGHT);
        BVDC_Window_SetDstRect(hGfxWin, ulXOffset+ulXStart, ulYOffset+ulYStart,
                               ulWidth, ulHeight);
        BTST_P_ApplyChanges(pContext);
        BKNI_Sleep(500);
    }
    BVDC_Window_SetScalerOutput(hGfxWin, 0, 0,
                                BTST_P_DEFAUL_GFX_WIDTH, BTST_P_DEFAUL_GFX_HEIGHT);
    BVDC_Window_SetDstRect(hGfxWin,
                           (pFmtInfo->ulWidth - BTST_P_DEFAUL_GFX_WIDTH)/2,
                           (pFmtInfo->ulHeight - BTST_P_DEFAUL_GFX_HEIGHT)/2,
                           BTST_P_DEFAUL_GFX_WIDTH, BTST_P_DEFAUL_GFX_HEIGHT);
    BTST_P_ApplyChanges(pContext);

    if(pContext->bApplyImmediately)
    {
        BTST_P_ApplyChanges(pContext);
    }

done:
    BTST_P_PrintStatus(pContext);
    return;
}

/***************************************************************************
 *
 */
void BTST_P_GfxSclModeTest
    ( BTST_P_Context          *pContext )
{
    int iVidSrcId = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].iVidSrcId;
    BVDC_Source_Handle hGfxSrc = pContext->aSrcContext[iVidSrcId].hVidSrc;
    BVDC_Window_Handle hGfxWin = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
    int iChoice;

    if(!hGfxWin || !hGfxSrc)
    {
        goto done;
    }

    printf("Please select scale (coeff) mode: PtrSam(0), BiLin(1), Smooth(2), Sharp(3)\n");
    BTST_P_Scanf(pContext, "%d", &iChoice);

    BVDC_Source_SetHorizontalScaleCoeffs(hGfxSrc, (BVDC_FilterCoeffs) iChoice);
    BTST_P_ApplyChanges(pContext);

    if(pContext->bApplyImmediately)
    {
        BTST_P_ApplyChanges(pContext);
    }

done:
    BTST_P_PrintStatus(pContext);
    return;
}

/***************************************************************************
 *
 */
void BTST_P_CloseGfxWin
    ( BTST_P_Context          *pContext )
{
    int iTmpSelectedWin    = pContext->iSelectedWin;
    int iTmpSelectedVidSrc = pContext->iSelectedVidSrc;


    pContext->iSelectedWin = 2;

    pContext->iSelectedVidSrc = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].iVidSrcId;

    if(BTST_P_IS_GFX_SRC(pContext->iSelectedVidSrc))
       
    {
        BTST_P_CloseVidWin(pContext);
        BTST_P_CloseVidSrc(pContext);

        pContext->iSelectedWin    = iTmpSelectedWin;
        pContext->iSelectedVidSrc = iTmpSelectedVidSrc;
    }

    return;
}

/***************************************************************************
 *
 */
void BTST_P_ChgGfxSurSettingMode
    ( BTST_P_Context          *pContext )
{
    BVDC_Window_Handle hGfxWin;
    BTST_P_GfxSurSetting eNewSurSetting;
    uint32_t ulMode;
    int iVidSrcId                      = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].iVidSrcId;
    BVDC_Source_Handle hGfxSrc         = pContext->aSrcContext[iVidSrcId].hVidSrc;
    BSUR_Surface_Handle hSurface       = pContext->aSrcContext[iVidSrcId].hGfxSurface;
    BTST_P_GfxSurSetting eCurSurSetting= pContext->aSrcContext[iVidSrcId].eGfxSurSetting;
    BAVC_Gfx_Picture * pGfxBuf         = pContext->aSrcContext[iVidSrcId].pGfxBuf;
    BVDC_Compositor_Handle hCompositor = pContext->ahCompositor[pContext->iSelectedComp];
    BAVC_Gfx_Picture    stGfxPic;

	BKNI_Memset((void*)&stGfxPic, 0x0, sizeof(BAVC_Gfx_Picture));
    stGfxPic.hSurface           =  hSurface;
    stGfxPic.eInOrientation     = BFMT_Orientation_e2D;

    hGfxWin = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

    /* get sur setting mode from user */
    printf("Gfx surface setting mode UserMode(%d), IsrMode(%d), CallBackMode(%d), DoubleBuf(%d):",
           eGfxSurUserMode, eGfxSurIsrMode, eGfxSurPullMode, eGfxSurDoubleBuf);
    BTST_P_Scanf(pContext, "%u", &ulMode);
    eNewSurSetting = (BTST_P_GfxSurSetting) ulMode;
    pContext->aSrcContext[pContext->iSelectedVidSrc].eGfxSurSetting = eNewSurSetting;

    if((!hCompositor) || (!hGfxSrc) || (!hGfxWin) ||
       (eNewSurSetting == eCurSurSetting))
    {
        goto done;
    }

    /* uninstall the current call back */
    if ((eGfxSurPullMode == eCurSurSetting) && (pGfxBuf))
    {
        BVDC_Source_InstallPictureCallback( hGfxSrc, NULL, NULL, 0);
        BTST_P_ApplyChanges(pContext);

        BKNI_Memset((void*)pGfxBuf, 0xA3, sizeof(BAVC_Gfx_Picture));
        BKNI_Free((void*)pGfxBuf);
        pContext->aSrcContext[pContext->iSelectedVidSrc].pGfxBuf = NULL;
        BDBG_MSG(("uninstall gfx src call back"));
    }

    /* activate new setting */
    if ( eGfxSurUserMode == eNewSurSetting )
    {
        BVDC_Source_SetSurface (hGfxSrc, &stGfxPic);
        BTST_P_ApplyChanges(pContext);
    }
    else if ( eGfxSurIsrMode == eNewSurSetting )
    {
        BKNI_EnterCriticalSection();
        BVDC_Source_SetSurface(hGfxSrc, &stGfxPic);
        BKNI_LeaveCriticalSection();
    }
    else if ( eGfxSurPullMode == eNewSurSetting )
    {
        BTST_P_GfxPullBuf(pContext);
    }
    else /* eGfxSurDoubleBuf */
    {
        BTST_P_GfxDoubleBuf(pContext);
    }


done:
    return;
}


#define BTST_P_VFD_BUF_CNT (4)

/***************************************************************************
 * Note: we can only use double buffer in one compositor!!!
 */
typedef struct BTST_P_VfdDoubleBufData {
    BVDC_Source_Handle source;
    BAVC_Gfx_Picture gfx_pic;
} BTST_P_VfdDoubleBufData;

static BTST_P_VfdDoubleBufData vfd_flip_data;
static BKNI_EventHandle vfd_flip_event;

static void BTST_P_VfdDoubleBufCallBack
    ( void *pvparm1,
      int parm2,
      BAVC_Polarity ePolarity,
      BAVC_SourceState eState,
      void **ppvPicture)
{
    BTST_P_VfdDoubleBufData *data = (BTST_P_VfdDoubleBufData *)pvparm1;
    BERR_Code rc;
    BSUR_Surface_Handle cur_surface;
    BAVC_Gfx_Picture stCurrGfxPic;

    BSTD_UNUSED(parm2);
    BSTD_UNUSED(ePolarity);
    BSTD_UNUSED(eState);

    *ppvPicture = NULL;

    rc = BVDC_Source_GetSurface_isr(data->source, &stCurrGfxPic);

    cur_surface = stCurrGfxPic.hSurface;

    if ( (rc == BERR_SUCCESS) && (cur_surface == data->gfx_pic.hSurface))
    {
        BKNI_SetEvent(vfd_flip_event);
    }
    else if (NULL == cur_surface)
    {
        BDBG_ERR(("Get NULL surface handle"));
    }

    *ppvPicture = &data->gfx_pic;

}

static void BTST_P_VfdDoubleBuf
    ( BTST_P_Context          *pContext )
{
    uint32_t ulWidth                  = BTST_P_DEFAUL_GFX_WIDTH;
    uint32_t ulHeight                 = BTST_P_DEFAUL_GFX_HEIGHT;
    int iVidSrcId                     = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].iVidSrcId;
    BVDC_Source_Handle hVfdSrc        = pContext->aSrcContext[iVidSrcId].hVidSrc;

    BSUR_Surface_Handle hTmpSur       = pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface;
    BSUR_Surface_Handle hTmpSur_R     = pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface_R;
    BSUR_Surface_Handle hGoldSur      = pContext->aSrcContext[pContext->iSelectedVidSrc].hGoldSurface;
    BSUR_Surface_Handle hGoldSur_R    = pContext->aSrcContext[pContext->iSelectedVidSrc].hGoldSurface_R;
    BSUR_Surface_Handle hFrntSur, hFrntSur_R;
    BSUR_Surface_Handle hSur[BTST_P_VFD_BUF_CNT], hSur_R[BTST_P_VFD_BUF_CNT];
    BAVC_Gfx_Picture    stGfxPic;
    BPXL_Format ePxlFmt;
    int iCntr, ii;
    BERR_Code rc;

    BSUR_Surface_GetDimensions(hGoldSur, &ulWidth, &ulHeight);
    BSUR_Surface_GetFormat(hGoldSur, &ePxlFmt);

    for (ii=0; ii<BTST_P_VFD_BUF_CNT; ii++)
    {
        rc = BSUR_Surface_Create(pContext->astMemInfo[0].hMem, ulWidth, ulHeight,
            0, NULL, ePxlFmt, NULL, 0, NULL, &hSur[ii]);

        BDBG_ASSERT(BERR_SUCCESS == rc);

        BGFX_CopyImage(hSur[ii], hGoldSur);
        BGFX_DumpImage(hSur[ii], 0, 0);
    }

    if (pContext->aSrcContext[pContext->iSelectedVidSrc].eInOrientation != BFMT_Orientation_e2D)
    {
        BSUR_Surface_GetDimensions(hGoldSur_R, &ulWidth, &ulHeight);
        BSUR_Surface_GetFormat(hGoldSur_R, &ePxlFmt);

        for (ii=0; ii<BTST_P_VFD_BUF_CNT; ii++)
        {
            rc = BSUR_Surface_Create(pContext->astMemInfo[0].hMem, ulWidth, ulHeight,
                0, NULL, ePxlFmt, NULL, 0, NULL, &hSur_R[ii]);

            BDBG_ASSERT(BERR_SUCCESS == rc);

            BGFX_CopyImage(hSur_R[ii], hGoldSur_R);
            BGFX_DumpImage(hSur_R[ii], 0, 0);
        }
    }
    else
    {
        for (ii=0; ii<BTST_P_VFD_BUF_CNT; ii++)
        {
            hSur_R[ii] = NULL;
        }
    }

    ii = 0;
    hFrntSur = hSur[0];
    hFrntSur_R = hSur_R[0];
    vfd_flip_data.gfx_pic.hSurface = hFrntSur;
    vfd_flip_data.gfx_pic.hRSurface = hFrntSur_R;
    vfd_flip_data.gfx_pic.eInOrientation = pContext->aSrcContext[pContext->iSelectedVidSrc].eInOrientation;
    vfd_flip_data.source = hVfdSrc;

	BKNI_Memset((void*)&stGfxPic, 0x0, sizeof(BAVC_Gfx_Picture));
    stGfxPic.hSurface =  hTmpSur;
    stGfxPic.hRSurface =  hTmpSur_R;
    stGfxPic.eInOrientation = pContext->aSrcContext[pContext->iSelectedVidSrc].eInOrientation;

    rc = BVDC_Source_SetSurface( hVfdSrc, &stGfxPic);
    BDBG_ASSERT(BERR_SUCCESS == rc);

    rc = BTST_P_ApplyChanges(pContext);
    BDBG_ASSERT(BERR_SUCCESS == rc);

    rc = BKNI_CreateEvent(&vfd_flip_event);
    BDBG_ASSERT(BERR_SUCCESS == rc);

    BKNI_ResetEvent(vfd_flip_event);
    BDBG_ASSERT(BERR_SUCCESS == rc);

    rc = BVDC_Source_InstallPictureCallback(hVfdSrc, BTST_P_VfdDoubleBufCallBack, (void *)&vfd_flip_data, 0);
    BDBG_ASSERT(BERR_SUCCESS == rc);

    BTST_P_ApplyChanges(pContext);

    iCntr = 16;
    while (iCntr-- > 0)
    {
        BSUR_Surface_Handle hSurf, hSurf_R;
        BKNI_Sleep(3000);

        /* current front buf is on display, flip the buf */
        if ( ++ii >= BTST_P_VFD_BUF_CNT ) ii = 0;
        hFrntSur = hSur[ii];
        hFrntSur_R = hSur_R[ii];

        if (iCntr % 2)
        {
            hSurf = vfd_flip_data.gfx_pic.hSurface = hFrntSur;
            hSurf_R = vfd_flip_data.gfx_pic.hRSurface = hFrntSur_R;
        }
        else
        {
            hSurf = vfd_flip_data.gfx_pic.hSurface = hTmpSur;
            hSurf_R = vfd_flip_data.gfx_pic.hRSurface = hTmpSur_R;
        }

        stGfxPic.hSurface = hSurf;
        stGfxPic.hRSurface = hSurf_R;

        BVDC_Source_SetSurface( hVfdSrc, &stGfxPic);
        BTST_P_ApplyChanges(pContext);

        rc = BKNI_WaitForEvent(vfd_flip_event, 1000);
        if (rc == BERR_TIMEOUT)
        {
            BDBG_ERR(("DOUBLE_BUF: BKNI_WaitForEvent on vfd_flip_event timed out!"));
        }
        else
        {
            printf("DOUBLE_BUF: got vfd flip event\n");
        }
    }

    stGfxPic.hSurface =  pContext->aSrcContext[pContext->iSelectedVidSrc].hGoldSurface;
    stGfxPic.hRSurface =  pContext->aSrcContext[pContext->iSelectedVidSrc].hGoldSurface_R;

    BVDC_Source_SetSurface (hVfdSrc, &stGfxPic);
    BTST_P_ApplyChanges(pContext);
    vfd_flip_data.gfx_pic.hSurface = stGfxPic.hSurface;
    vfd_flip_data.gfx_pic.hRSurface = stGfxPic.hRSurface;

    rc = BKNI_WaitForEvent(vfd_flip_event, 1000);
    if (rc == BERR_TIMEOUT)
    {
        BDBG_ERR(("DOUBLE_BUF: BKNI_WaitForEvent on final vfd_flip_event timed out!"));
    }
    else
    {
        printf("DOUBLE_BUF: got final vfd flip event\n");
    }

    BKNI_DestroyEvent(vfd_flip_event);
    BVDC_Source_InstallPictureCallback(hVfdSrc, NULL, (void *)&vfd_flip_data, 0);
    for (ii=0; ii<BTST_P_VFD_BUF_CNT; ii++)
    {
        BSUR_Surface_Destroy(hSur[ii]);
        hSur[ii] = NULL;

        if (hSur_R[ii])
        {
            BSUR_Surface_Destroy(hSur_R[ii]);
            hSur_R[ii] = NULL;
        }
    }

    return;
}

/***************************************************************************
 *
 */
void VfdPullBufCallBack_isr
    ( void             *pvParm1,
      int               iParm2,
      BAVC_Polarity     ePolarity,
      BAVC_SourceState  eSourceState,
      void            **ppvPicture )
{
    BTST_P_VfdDoubleBufData *data = (BTST_P_VfdDoubleBufData *)pvParm1;
    BERR_Code rc;
    BSUR_Surface_Handle cur_surface;
    BAVC_Gfx_Picture    stCurrGfxPic;

    BSTD_UNUSED(iParm2);
    BSTD_UNUSED(ePolarity);
    BSTD_UNUSED(eSourceState);

    *ppvPicture = (void *)&vfd_flip_data.gfx_pic;

    rc = BVDC_Source_GetSurface_isr(data->source, &stCurrGfxPic);
    cur_surface = stCurrGfxPic.hSurface;

    if ( (rc != BERR_SUCCESS) || (cur_surface == vfd_flip_data.gfx_pic.hSurface))
    {
        vfd_flip_data.gfx_pic.hSurface = NULL;
        vfd_flip_data.gfx_pic.hRSurface = NULL;
    }

    BKNI_SetEvent(vfd_flip_event);
}

static void BTST_P_VfdPullBuf
    ( BTST_P_Context          *pContext )
{
    BVDC_Window_Handle hVfdWin;
    uint32_t ulWidth                   = BTST_P_DEFAUL_GFX_WIDTH;
    uint32_t ulHeight                  = BTST_P_DEFAUL_GFX_HEIGHT;
    int iVidSrcId                      = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].iVidSrcId;
    BVDC_Source_Handle hVfdSrc         = pContext->aSrcContext[iVidSrcId].hVidSrc;
    BSUR_Surface_Handle hGoldSur, hGoldSur_R;
    BSUR_Surface_Handle hFrntSur, hTmpSur;
    BSUR_Surface_Handle hFrntSur_R, hTmpSur_R;
    BSUR_Surface_Handle hSur[BTST_P_VFD_BUF_CNT];
    BSUR_Surface_Handle hSur_R[BTST_P_VFD_BUF_CNT];
    BAVC_Gfx_Picture    stGfxPic;
    BPXL_Format ePxlFmt;
    int iCntr, ii;
    BERR_Code rc;

    hVfdWin = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

    /*hGoldSur = pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface;*/
    hTmpSur       = pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface;
    hTmpSur_R     = pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface_R;
    hGoldSur      = pContext->aSrcContext[pContext->iSelectedVidSrc].hGoldSurface;
    hGoldSur_R    = pContext->aSrcContext[pContext->iSelectedVidSrc].hGoldSurface_R;
    BSUR_Surface_GetDimensions(hGoldSur, &ulWidth, &ulHeight);
    BSUR_Surface_GetFormat(hGoldSur, &ePxlFmt);

    for (ii=0; ii<BTST_P_VFD_BUF_CNT; ii++)
    {
        rc |= BSUR_Surface_Create(pContext->astMemInfo[0].hMem, ulWidth, ulHeight,
            0, NULL, ePxlFmt, NULL, 0, NULL, &hSur[ii]);
        BGFX_CopyImage(hSur[ii], hGoldSur);
    }

    if (pContext->aSrcContext[pContext->iSelectedVidSrc].eInOrientation != BFMT_Orientation_e2D)
    {
        BSUR_Surface_GetDimensions(hGoldSur_R, &ulWidth, &ulHeight);
        BSUR_Surface_GetFormat(hGoldSur_R, &ePxlFmt);

        for (ii=0; ii<BTST_P_VFD_BUF_CNT; ii++)
        {
            rc = BSUR_Surface_Create(pContext->astMemInfo[0].hMem, ulWidth, ulHeight,
                0, NULL, ePxlFmt, NULL, 0, NULL, &hSur_R[ii]);

            BDBG_ASSERT(BERR_SUCCESS == rc);

            BGFX_CopyImage(hSur_R[ii], hGoldSur_R);
            BGFX_DumpImage(hSur_R[ii], 0, 0);
        }
    }
    else
    {
        for (ii=0; ii<BTST_P_VFD_BUF_CNT; ii++)
        {
            hSur_R[ii] = NULL;
        }
    }

    ii = 0;
    hFrntSur = hSur[0];
    hFrntSur_R = hSur_R[0];

	BKNI_Memset((void*)&stGfxPic, 0x0, sizeof(BAVC_Gfx_Picture));
    stGfxPic.hSurface           =  hTmpSur;
    stGfxPic.hRSurface          =  hTmpSur_R;
    stGfxPic.eInOrientation     =  pContext->aSrcContext[pContext->iSelectedVidSrc].eInOrientation;;

    rc = BVDC_Source_SetSurface( hVfdSrc, &stGfxPic);
    BDBG_ASSERT(BERR_SUCCESS == rc);

    rc = BTST_P_ApplyChanges(pContext);
    BDBG_ASSERT(BERR_SUCCESS == rc);

    BKNI_Sleep(5000);

    rc = BKNI_CreateEvent(&vfd_flip_event);
    BDBG_ASSERT(BERR_SUCCESS == rc);

    BKNI_ResetEvent(vfd_flip_event);
    BDBG_ASSERT(BERR_SUCCESS == rc);

    /* set initial surface */
    vfd_flip_data.source = hVfdSrc;
    vfd_flip_data.gfx_pic.hSurface = hFrntSur;
    vfd_flip_data.gfx_pic.hRSurface = hFrntSur_R;
    vfd_flip_data.gfx_pic.eInOrientation = pContext->aSrcContext[pContext->iSelectedVidSrc].eInOrientation;

    BVDC_Source_InstallPictureCallback( hVfdSrc, VfdPullBufCallBack_isr, (void *)&vfd_flip_data, 0);
    BTST_P_ApplyChanges(pContext);

    iCntr = 16;
    while (iCntr-- > 0)
    {
        BSUR_Surface_Handle hSurf, hSurf_R;

        BKNI_Sleep(3000);

        /* current front buf is on display, flip the buf */
        if ( ++ii >= BTST_P_VFD_BUF_CNT ) ii = 0;
        hFrntSur = hSur[ii];
        hFrntSur_R = hSur_R[ii];

        if (iCntr % 2)
        {
            hSurf = vfd_flip_data.gfx_pic.hSurface = hFrntSur;
            hSurf_R = vfd_flip_data.gfx_pic.hRSurface = hFrntSur_R;
        }
        else
        {
            hSurf = vfd_flip_data.gfx_pic.hSurface = hTmpSur;
            hSurf_R = vfd_flip_data.gfx_pic.hRSurface = hTmpSur_R;
        }

        stGfxPic.hSurface           =  hSurf;
        stGfxPic.hRSurface          =  hSurf_R;
        rc = BKNI_WaitForEvent(vfd_flip_event, 1000);
        if (rc == BERR_TIMEOUT)
        {
            BDBG_ERR(("PULL_BUF: BKNI_WaitForEvent on vfd_flip_event timed out!"));
        }
        else
        {
            printf("PULL_BUF: got vfd flip event\n");
        }

        /* invisible test */
        if (0 == (iCntr % 4))
        {
            printf("\t+++ Mute gfx\n");
            BVDC_Window_SetVisibility(hVfdWin, false);
            BTST_P_ApplyChanges(pContext);

            /* make sure futher sur setting will not mess RUL */
            BVDC_Source_SetSurface( hVfdSrc, &stGfxPic);
            BTST_P_ApplyChanges(pContext);
            BKNI_Sleep(3000);

            BVDC_Source_SetSurface( hVfdSrc, &stGfxPic);
            BTST_P_ApplyChanges(pContext);
            BVDC_Window_SetVisibility(hVfdWin, true);
            BTST_P_ApplyChanges(pContext);
        }
    }

    rc = BKNI_WaitForEvent(vfd_flip_event, 1000);
    if (rc == BERR_TIMEOUT)
    {
        BDBG_ERR(("PULL_BUF: BKNI_WaitForEvent on final vfd_flip_event timed out!"));
    }

    BVDC_Source_InstallPictureCallback( hVfdSrc, NULL, (void *)&vfd_flip_data, 0);
    BTST_P_ApplyChanges(pContext);

    stGfxPic.hSurface = pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface;
    stGfxPic.hRSurface = pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface_R;
    BVDC_Source_SetSurface (hVfdSrc, &stGfxPic);
    BTST_P_ApplyChanges(pContext);
    BKNI_Sleep(1000);
    for (ii=0; ii<BTST_P_VFD_BUF_CNT; ii++)
    {
        BSUR_Surface_Destroy(hSur[ii]);
        hSur[ii] = NULL;

        if (hSur[ii])
        {
            BSUR_Surface_Destroy(hSur_R[ii]);
            hSur_R[ii] = NULL;
        }
    }

    return;
}

/***************************************************************************
 *
 */
void BTST_P_ChgVfdSurSettingMode
    ( BTST_P_Context          *pContext )
{
    BVDC_Window_Handle hVfdWin;
    BTST_P_GfxSurSetting eNewSurSetting;
    uint32_t ulMode;
    int iVidSrcId                      = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].iVidSrcId;
    BVDC_Source_Handle hVfdSrc         = pContext->aSrcContext[iVidSrcId].hVidSrc;
    BTST_P_GfxSurSetting eCurSurSetting= pContext->aSrcContext[iVidSrcId].eGfxSurSetting;
    BVDC_Compositor_Handle hCompositor = pContext->ahCompositor[pContext->iSelectedComp];
    BAVC_Gfx_Picture    stGfxPic;

	BKNI_Memset((void*)&stGfxPic, 0x0, sizeof(BAVC_Gfx_Picture));
	
    hVfdWin = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

    /* get sur setting mode from user */
    printf("VFD surface setting mode UserMode(%d), IsrMode(%d), CallBackMode(%d), DoubleBuf(%d):",
           eGfxSurUserMode, eGfxSurIsrMode, eGfxSurPullMode, eGfxSurDoubleBuf);
    scanf("%u", &ulMode);
    eNewSurSetting = (BTST_P_GfxSurSetting) ulMode;
    pContext->aSrcContext[pContext->iSelectedVidSrc].eGfxSurSetting = eNewSurSetting;

    if((!hCompositor) || (!hVfdSrc) || (!hVfdWin) ||
       (eNewSurSetting == eCurSurSetting))
    {
        goto done;
    }

    /* uninstall the current call back */
    if (eGfxSurPullMode == eCurSurSetting)
    {
        BVDC_Source_InstallPictureCallback( hVfdSrc, NULL, NULL, 0);
        BTST_P_ApplyChanges(pContext);

        BDBG_MSG(("uninstall VFD src call back"));
    }
    /* activate new setting */
    if ( eGfxSurUserMode == eNewSurSetting )
    {
        BVDC_Source_SetSurface (hVfdSrc, &stGfxPic);
        BTST_P_ApplyChanges(pContext);
    }
    else if ( eGfxSurIsrMode == eNewSurSetting )
    {
        printf("change to ISR surface mode\n");
        BKNI_EnterCriticalSection();
        BVDC_Source_SetSurface(hVfdSrc, &stGfxPic);
        BKNI_LeaveCriticalSection();
    }
    else if ( eGfxSurPullMode == eNewSurSetting )
    {
        BTST_P_VfdPullBuf(pContext);
    }
    else /* eGfxSurDoubleBuf */
    {
        BTST_P_VfdDoubleBuf(pContext);
    }
done:
    return;
}


/*
 * the utility function to create and fill surface with predefined pattern
 * Return: 0 - succeed; 1 - fail
 */
static uint32_t BTST_MakeSurfaces(
    BMEM_Handle               hMem,
    BSUR_Surface_Handle      *phSurface,
    uint32_t                  ulNumframes,
    uint32_t                  ulPattern,     /* frame buffer data pattern */
    BPXL_Format               ePixelFmt,
    uint32_t                  uiWidth,      /* picture width */
    uint32_t                  uiHeight,     /* picture height */
    bool                      bFldSur)      /* sur is fld or frame */
{
    uint32_t ii, jj = 0;
    uint8_t  Y, Cb, Cr;
    uint32_t uiFrmId;          /* frame index */
    uint32_t ulSurPitch;
    uint8_t *pvSurStartAddr;
    uint32_t  ulSurHeight;
    BAVC_Polarity ePolarity;
    uint32_t color = 0;
    BERR_Code    eResult;

    BDBG_ENTER(BTST_MakeSurfaces);

    if (ulPattern == 6)
    {
        printf("Select color 0(white), 1(yellow), 2(cyan), 3(green), 4(magenta), 5(red), 6(blue), 7(black)\n");
        scanf("%ul", &color);
    }

    /* Note: field surface only has half of frame height */
    ulSurHeight = (bFldSur)? (uiHeight / 2) : uiHeight;

    /* create surfaces */
    for (uiFrmId = 0; uiFrmId < ulNumframes; uiFrmId++ )
    {
        eResult = BSUR_Surface_Create(hMem, uiWidth, ulSurHeight, 0, NULL, ePixelFmt, NULL, 0, NULL, &(phSurface[uiFrmId]));
        if (eResult != BERR_SUCCESS)
        {
            BDBG_ERR(("ERROR: BMEM_AllocAligned out of heap memory!"));
            return 1;
        }

        if (bFldSur)
            ePolarity = (uiFrmId & 0x1)?
                BAVC_Polarity_eBotField: BAVC_Polarity_eTopField;
        else
            ePolarity = BAVC_Polarity_eFrame;
    }

    /* fill in each line of the frame */
    for( ii = 0; ii < uiHeight; ii++ )
    {
        /* for each pixel of the line */
        for( jj = 0; jj < uiWidth; jj++ )
        {
            if (ulPattern != 0)
            {
                /* which pattern to use? */
                switch( ulPattern )
                {
                case 1:
                case 6:
                    break;
                case 2:
                    TEST_checkboard_YCbCr_pixel( jj, uiWidth, ii, uiHeight, &Y, &Cb, &Cr );
                    break;
                case 3:
                    TEST_crosshatch_YCbCr_pixel( jj, ii, &Y, &Cb, &Cr );
                    break;
                case 4:
                    TEST_lumaramp_YCbCr_pixel( jj, uiWidth, ii, uiHeight, &Y, &Cb, &Cr, true );
                    break;
                case 5:
                    TEST_lumaramp_YCbCr_pixel( jj, uiWidth, ii, uiHeight, &Y, &Cb, &Cr, false );
                    break;
                default:
                    BDBG_ERR(("TODO: frame buffer filling Error handling."));
                }/* end of switch pattern */
            }

            /* traverse through each frame */
            for (uiFrmId = 0; uiFrmId < ulNumframes; uiFrmId++ )
            {
                /* Note: top fld surface only has even lines (including line 0), and
                 * botttom field surfaces only has odd lines */
                if ((!bFldSur) ||
                    ((ii & 0x1) == (uiFrmId && 0x1)))
                {
                    /* color bar depends on FrmId, it left-shift a region for each frame */
                    if (ulPattern == 1)
                    {
                        TEST_colorbar_YCbCr_pixel( jj, uiWidth, false, 0,
                                                   /*((bFldSur)? (uiFrmId/2): uiFrmId),*/ &Y, &Cb, &Cr );
                    }
                    else if (ulPattern == 6)
                    {

                        TEST_singlecolor_YCbCr_pixel( color, false, &Y, &Cb, &Cr );

                    }

                    BSUR_Surface_GetAddress( phSurface[uiFrmId],
                                             (void *) (&pvSurStartAddr), &ulSurPitch );

                    pvSurStartAddr += ((bFldSur)? ii/2: ii)  * ulSurPitch;
                    if( jj & 1 )
                    {
                        pvSurStartAddr[(jj-1) * 2 + BPXL_COMPONENT_POS(ePixelFmt, 2) / 8] = Y;
                    }
                    else
                    {
                        ((uint32_t *) pvSurStartAddr) [jj / 2] =
                            BPXL_MAKE_PIXEL(ePixelFmt, Y, Y, Cb, Cr);
                    }
                }
            }/* end of frame index for loop */
        }/* end of width for loop */
    }/* end of height for loop */

#if 1
                    printf("pixel [%dx%d]: Y[0x%x], Cb[0x%x], Cr[0x%x]\n", ii, jj, Y, Cb, Cr);
#endif

    BDBG_LEAVE(BTST_MakeSurfaces);
    return 0;
}
/***************************************************************************
 *
 */
static void BTST_P_OpenVidWin_GFX
    ( BTST_P_Context          *pContext )
{

	BPXL_Format 					    eGraphicsFormat;
	uint32_t ulWidth				    = BTST_P_DEFAUL_GFX_WIDTH;
	uint32_t ulHeight				    = BTST_P_DEFAUL_GFX_HEIGHT;
	BSUR_Surface_Handle hSurface	    = pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface;
	BSUR_Surface_Handle hSurface_R		= pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface_R;
	BTST_P_GfxSurSetting eGfxSurSetting = pContext->aSrcContext[pContext->iSelectedVidSrc].eGfxSurSetting;
	BVDC_Source_Handle hVidSrc          = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	BVDC_Display_Handle hDisplay        = pContext->ahDisplay[pContext->iSelectedComp];
	const BFMT_VideoInfo *pFmtInfo      = pContext->apFmtInfo[pContext->iSelectedComp];
	BVDC_Window_Handle hVidWin         =
        pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	bool                bDTg = false;
	int ans;
	BFMT_Orientation eOrientation;
	BFMT_VideoFmt eFmt;
	bool		  bIs3DOutput;
	BAVC_Gfx_Picture      stGfxPic;
	uint32_t              ulHSize, ulVSize, ulLeft, ulTop;
	
	
	BKNI_Memset((void*)&stGfxPic, 0x0, sizeof(BAVC_Gfx_Picture));

	stGfxPic.hSurface = hSurface;
	stGfxPic.eInOrientation = pContext->aSrcContext[pContext->iSelectedVidSrc].eInOrientation;
	bDTg                    = BTST_P_USED_DIGTRIG(pContext->eMasterTg[pContext->iSelectedComp]);
	
	ulHSize = bDTg? pFmtInfo->ulDigitalWidth :pFmtInfo->ulWidth;
	ulVSize = bDTg? pFmtInfo->ulDigitalHeight:pFmtInfo->ulHeight;
	BSUR_Surface_GetFormat(hSurface, &eGraphicsFormat);

	BVDC_Display_GetVideoFormat( hDisplay, &eFmt );
	bIs3DOutput = (eFmt == BFMT_VideoFmt_e720p_60Hz_3DOU_AS)  ||
				  (eFmt == BFMT_VideoFmt_e720p_50Hz_3DOU_AS)  ||
				  (eFmt == BFMT_VideoFmt_e1080p_24Hz_3DOU_AS) ||
				  (eFmt == BFMT_VideoFmt_e1080p_30Hz_3DOU_AS) ;

	if(pContext->bResourceLeakAutoTest)
	{
		eOrientation = BFMT_Orientation_e2D;
	}
	else
	{
		printf("Use default gfx window size? (0=n/1=y)");
		BTST_P_Scanf(pContext, "%d", &ans);
		if (!ans)
		{
			printf("Enter gfx window width and height: ");
			BTST_P_Scanf(pContext, "%u %u", &ulWidth, &ulHeight);
		}

		/*3D handling */
		BVDC_Display_GetOrientation(hDisplay, &eOrientation);
		printf("Enter Output Orientation [Current = %d]: 2D(0),LR(1),OU(2), L(3), R(4)", eOrientation) ;
		BTST_P_Scanf(pContext, "%d", &eOrientation);
	}

	BVDC_Window_SetAlpha	   (hVidWin, 0x80);
	BVDC_Window_SetBlendFactor (hVidWin, BVDC_BlendFactor_eConstantAlpha, BVDC_BlendFactor_eOneMinusSrcAlpha, BVDC_ALPHA_MAX);
	BVDC_Window_SetZOrder	   (hVidWin, pContext->iSelectedWin);
	BVDC_Display_SetOrientation(hDisplay, eOrientation);


	if ((eOrientation != BFMT_Orientation_e2D) && (!bIs3DOutput))	 /*Verify 3D on the 2D display) */
	{
		ulHSize >>= (eOrientation == BFMT_Orientation_e3D_LeftRight);
		ulWidth >>= (eOrientation == BFMT_Orientation_e3D_LeftRight);

		ulVSize >>= (eOrientation == BFMT_Orientation_e3D_OverUnder);
		ulHeight >>= (eOrientation == BFMT_Orientation_e3D_OverUnder);
	}


	ulLeft = ( ulHSize - ulWidth ) * (2 + pContext->iSelectedWin)/8;
	ulTop  = (ulVSize  - ulHeight) * (2 + pContext->iSelectedWin)/8;

	BVDC_Window_SetDstRect	   (hVidWin,  ulLeft,  ulTop, ulWidth, ulHeight);

	BVDC_Window_SetScalerOutput(hVidWin, 0, 0, ulWidth, ulHeight);


#if (BTST_P_SUPPORT_HDM)
	if(pContext->hHdm)
	{
		BHDM_GetVendorSpecificInfoFrame(pContext->hHdm, &pContext->stHdmiSettings.stVendorSpecificInfoFrame);
		if(eOrientation == BFMT_Orientation_e2D)
			pContext->stHdmiSettings.stVendorSpecificInfoFrame.eHdmiVideoFormat = BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_eNone;
		else
		{
			pContext->stHdmiSettings.stVendorSpecificInfoFrame.eHdmiVideoFormat = BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_e3DFormat;
			if(eOrientation == BFMT_Orientation_e3D_LeftRight)
				pContext->stHdmiSettings.stVendorSpecificInfoFrame.e3DStructure = BAVC_HDMI_VSInfoFrame_3DStructure_eSidexSideHalf;
			else if(eOrientation == BFMT_Orientation_e3D_OverUnder)
				pContext->stHdmiSettings.stVendorSpecificInfoFrame.e3DStructure = BAVC_HDMI_VSInfoFrame_3DStructure_eTopAndBottom;
			else
				pContext->stHdmiSettings.stVendorSpecificInfoFrame.e3DStructure = BAVC_HDMI_VSInfoFrame_3DStructure_eFieldAlternative;
		}
		BHDM_SetVendorSpecificInfoFrame(pContext->hHdm, &pContext->stHdmiSettings.stVendorSpecificInfoFrame);
	}
#endif

	BVDC_Window_SetVisibility(hVidWin, true);

	if ( eGfxSurUserMode == eGfxSurSetting )
	{
		stGfxPic.hSurface = hSurface;
		stGfxPic.hRSurface = hSurface_R;
		BVDC_Source_SetSurface (hVidSrc, &stGfxPic);
		if (!pContext->bResourceLeakAutoTest) printf("set surface in user mode\n");
	}
	else if ( eGfxSurIsrMode == eGfxSurSetting )
	{
		stGfxPic.hSurface = hSurface;
		stGfxPic.hRSurface = hSurface_R;
		BKNI_EnterCriticalSection();
		BVDC_Source_SetSurface (hVidSrc, &stGfxPic);
		BKNI_LeaveCriticalSection();
		printf("set surface with _isr\n");
	}
	else if ( eGfxSurPullMode == eGfxSurSetting )
	{
		BTST_P_GfxPullBuf(pContext);
	}
	else /* eGfxSurDoubleBuf */
	{
		BTST_P_GfxDoubleBuf(pContext);
	}
}

/***************************************************************************
 *
 */
static void BTST_P_OpenVidWin_GfxToMfdVfd
    ( BTST_P_Context          *pContext )
{
	
	uint32_t ulWidth					= BTST_P_DEFAUL_GFX_WIDTH;
	uint32_t ulHeight					= BTST_P_DEFAUL_GFX_HEIGHT;
	BSUR_Surface_Handle hSurface		= pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface;
	BSUR_Surface_Handle hSurface_R		= pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface_R;
	BSUR_Surface_Handle hIntrSurface	= pContext->aSrcContext[pContext->iSelectedVidSrc].hIntrSurface;
	BSUR_Surface_Handle hIntrSurface_R	= pContext->aSrcContext[pContext->iSelectedVidSrc].hIntrSurface_R;
	BTST_P_GfxSurSetting eGfxSurSetting = pContext->aSrcContext[pContext->iSelectedVidSrc].eGfxSurSetting;
	BVDC_Source_Handle hVidSrc          = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	BVDC_Window_Handle hVidWin         =
        pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	BAVC_Gfx_Picture      stGfxPic;
	uint32_t              ulHSize, ulVSize;
	const BFMT_VideoInfo *pFmtInfo      = pContext->apFmtInfo[pContext->iSelectedComp];
	bool                bDTg = false;
	
	stGfxPic.eInOrientation 	 = (hSurface_R) ? BFMT_Orientation_e3D_LeftRight : BFMT_Orientation_e2D;

	bDTg                    = BTST_P_USED_DIGTRIG(pContext->eMasterTg[pContext->iSelectedComp]);
	ulHSize = bDTg? pFmtInfo->ulDigitalWidth :pFmtInfo->ulWidth;
	ulVSize = bDTg? pFmtInfo->ulDigitalHeight:pFmtInfo->ulHeight;
	
	if (!pContext->bVfdAsSourceAutoTest)
	{
		int ans;
		printf("Use default VFD source window size? (0=n/1=y)");
		scanf("%d", &ans);
		if (!ans)
		{
			printf("Enter VFD source window width and height: ");
			scanf("%u %u", &ulWidth, &ulHeight);
		}
	}
	
	BVDC_Window_SetScalerOutput(hVidWin, 0, 0, ulWidth, ulHeight);
	
	switch(pContext->iSelectedWin)
	{
	case 0:
		BVDC_Window_SetDstRect	   (hVidWin, (ulHSize - ulWidth)/4, (ulVSize - ulHeight)/4, ulWidth, ulHeight);
		break;
	case 1:
		BVDC_Window_SetDstRect	   (hVidWin, ((ulHSize - ulWidth)*3)/8, ((ulVSize - ulHeight)*3)/8, ulWidth, ulHeight);
		break;
	case 3:
		BVDC_Window_SetDstRect	   (hVidWin, ((ulHSize - ulWidth)*5)/8, ((ulVSize - ulHeight)*5)/8, ulWidth, ulHeight);
		break;
	case 4:
		BVDC_Window_SetDstRect	   (hVidWin, ((ulHSize - ulWidth)*3)/4, ((ulVSize - ulHeight)*3)/4, ulWidth, ulHeight);
		break;
	case 2:
	default:
		BVDC_Window_SetDstRect	   (hVidWin, (ulHSize - ulWidth)/2, (ulVSize - ulHeight)/2, ulWidth, ulHeight);
		break;
	}
	
	/* Swap */
	BVDC_Window_SetZOrder(hVidWin, pContext->abSwapMainPip[pContext->iSelectedComp]
	? (1 - pContext->iSelectedWin)
		: (pContext->iSelectedWin));
	
	BVDC_Window_SetVisibility(hVidWin, true);
	
	if ( eGfxSurUserMode == eGfxSurSetting )
	{
		stGfxPic.hSurface = hSurface;
		stGfxPic.hRSurface = hSurface_R;
		BVDC_Source_SetSurface (hVidSrc, &stGfxPic);
	}
	else if ( eGfxSurIsrMode == eGfxSurSetting )
	{
		BKNI_EnterCriticalSection();
		stGfxPic.hSurface = hIntrSurface;
		stGfxPic.hRSurface = hIntrSurface_R;
		BVDC_Source_SetSurface(hVidSrc, &stGfxPic);
		BKNI_LeaveCriticalSection();
	
	}
	else if ( eGfxSurPullMode == eGfxSurSetting )
	{
		BTST_P_VfdPullBuf(pContext);
	}
	else
	{
		BTST_P_VfdDoubleBuf(pContext);
	}
}

/***************************************************************************
 *
 */
static void BTST_P_OpenVidWin_MPEG
    ( BTST_P_Context          *pContext,
      uint32_t                 ulHSize,
      uint32_t                 ulVSize)
{
	
	BVDC_Window_Handle hVidWin         =
        pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	/* Always enable deinterlacer if available. */
	if(!pContext->iSelectedWin && !pContext->iSelectedComp)
	{
		BVDC_Deinterlace_Settings stMadCfg;
#if (BTST_P_SUPPORT_ANR)
		BVDC_Anr_Settings stAnrCfg;
		BVDC_Window_GetAnrConfiguration(hVidWin, &stAnrCfg);
		stAnrCfg.eMode = BVDC_FilterMode_eEnable;
		stAnrCfg.ePxlFormat = BPXL_eY18_Cb8_Y08_Cr8;
		BVDC_Window_SetAnrConfiguration(hVidWin, &stAnrCfg);
#endif
		BVDC_Window_SetSharpness(hVidWin, true, 0);
		BVDC_Window_SetMasterFrameRate(hVidWin, true);
		BVDC_Window_GetDeinterlaceConfiguration(hVidWin, NULL, &stMadCfg);
		stMadCfg.ePxlFormat = BPXL_eY18_Cb8_Y08_Cr8;
		stMadCfg.bShrinkWidth = pContext->bShrinkWidthForMad;
		BVDC_Window_SetDeinterlaceConfiguration(
			hVidWin, true, &stMadCfg);
	}

	if((pContext->bEnableScaleDown) ||
	   ((!pContext->iSelectedWin &&  pContext->abSwapMainPip[pContext->iSelectedComp]) ||
		( pContext->iSelectedWin && !pContext->abSwapMainPip[pContext->iSelectedComp])))
	{
		BVDC_Window_SetScalerOutput(hVidWin, 0, 0, ulHSize/2, ulVSize/2);
		BVDC_Window_SetDstRect(hVidWin, ulHSize/4, ulVSize/4, ulHSize/2, ulVSize/2);
	}
	else
	{
		BVDC_Window_SetScalerOutput(hVidWin, 0, 0, ulHSize, ulVSize);
		BVDC_Window_SetDstRect(hVidWin, 0, 0, ulHSize, ulVSize);
	}


	/* Swap */
	BVDC_Window_SetZOrder(hVidWin, pContext->abSwapMainPip[pContext->iSelectedComp]
	? (1 - pContext->iSelectedWin)
		: (pContext->iSelectedWin));
}

/***************************************************************************
 *
 */
void BTST_P_OpenVidWin
    ( BTST_P_Context          *pContext )
{
    BVDC_Compositor_Handle hCompositor = pContext->ahCompositor[pContext->iSelectedComp];
    BVDC_Source_Handle hVidSrc         = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
    const BFMT_VideoInfo *pFmtInfo     = pContext->apFmtInfo[pContext->iSelectedComp];
    BVDC_DisplayTg   eMasterTg         = pContext->eMasterTg[pContext->iSelectedComp];
    BVDC_Display_Handle hDisplay       = pContext->ahDisplay[pContext->iSelectedComp];
    BVDC_Window_Handle hVidWin         =
        pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
    BVDC_Window_Settings  stWinSettings;
    BMEM_HeapInfo         stHeapStart, stHeapEnd;
    uint32_t              ulHSize, ulVSize;
    bool                  bDTg;
    BAVC_Gfx_Picture      stGfxPic;
	int32_t              uiWinMemIdx;

    BDBG_ASSERT(pContext->iSelectedWin < BTST_P_MAX_WINDOW);

    if((!hVidSrc) || (!hCompositor) || (hVidWin))
    {
        goto done;
    }

	BKNI_Memset((void*)&stGfxPic, 0x0, sizeof(BAVC_Gfx_Picture));
	
    BVDC_Window_GetDefaultSettings(pContext->iSelectedWin, &stWinSettings);

    stWinSettings.bForceSyncLock = pContext->bForceSyncLock;
    stWinSettings.bDeinterlacerAllocFull = pContext->bDeinterlaceAllocFull;
	stWinSettings.pMinSrcFmt     = pContext->pMinSrcFmt;
	stWinSettings.pMinDspFmt     = pContext->pMinDspFmt;
	stWinSettings.bBypassVideoProcessings = pContext->bBypassVideoProcessings;

	/* SW7425-465 smooth transcoding resolution transaction */
	if (BTST_P_USED_STG(eMasterTg) && (0==pContext->pMinDspFmt))
		stWinSettings.pMinDspFmt = BFMT_GetVideoFormatInfoPtr(BFMT_VideoFmt_e1080p);


    bDTg      =  BTST_P_USED_DIGTRIG(eMasterTg);

    ulHSize = bDTg? pFmtInfo->ulDigitalWidth :pFmtInfo->ulWidth;
    ulVSize = bDTg? pFmtInfo->ulDigitalHeight:pFmtInfo->ulHeight;

	if(!BFMT_IS_3D_MODE(pFmtInfo->eVideoFmt))
	{
		/* half resolution 3D cases */
		BFMT_Orientation eOrientation;
		BVDC_Display_GetOrientation(hDisplay, &eOrientation);
		if(eOrientation == BFMT_Orientation_e3D_LeftRight)
			ulHSize = ulHSize / 2;
		else if(eOrientation == BFMT_Orientation_e3D_OverUnder)
			ulVSize = ulVSize / 2;
	}

	uiWinMemIdx = pContext->uWinMemIdx[pContext->iSelectedComp*BTST_P_MAX_WINDOW + pContext->iSelectedWin];
	BDBG_ASSERT(uiWinMemIdx!=-1);
	stWinSettings.hHeap = pContext->astMemInfo[uiWinMemIdx].hVdcHeap;
	printf("\n Use default MEMC[%d] to open comp%d win%d\n", uiWinMemIdx,
			pContext->iSelectedComp, pContext->iSelectedWin);



    if (pContext->bSharedMem)
    {
        stWinSettings.hHeap = pContext->hSharedHeap;
        printf("Using shared heap [0x%x] for window %d\n", (uint32_t)pContext->hSharedHeap, pContext->iSelectedWin);

        BMEM_GetHeapInfo(pContext->hSharedMem, &stHeapStart);
        printf("Before: Available shared memory = %d bytes\n", stHeapStart.ulLargestFree);
    }

    if(BERR_SUCCESS != BVDC_Window_Create(hCompositor, &hVidWin, pContext->iSelectedWin, hVidSrc, &stWinSettings))
    {
        printf("Window[%d][%d] cannot be created.\n",
            pContext->iSelectedComp, pContext->iSelectedWin);
        goto done;
    }
    /* hold on to the handle. */
    pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow = hVidWin;
    pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].iVidSrcId = pContext->iSelectedVidSrc;
	pContext->aulSelectedMDec[pContext->iSelectedComp][pContext->iSelectedWin] = pContext->eSelectedMDec;
	pContext->aulSelectedMDecCh[pContext->iSelectedComp][pContext->iSelectedWin] = pContext->iSelectedMDecCh;

    /* Graphics window */
    if(BTST_P_IS_GFX_SRC(pContext->iSelectedVidSrc))
    {
		BTST_P_OpenVidWin_GFX(pContext);
    }
	else if (BTST_P_IS_VFD_SRC(pContext->iSelectedVidSrc))
    {
		BTST_P_OpenVidWin_GfxToMfdVfd(pContext);
    }
	else  /* MPEG Src */
    {
		if (BTST_P_IS_MPEG_SRC(pContext->iSelectedVidSrc) &&
			pContext->ulGfx2Mfd[pContext->iSelectedVidSrc])
		{
			BTST_P_OpenVidWin_GfxToMfdVfd(pContext);
		}
		else
		{
			BTST_P_OpenVidWin_MPEG(pContext, ulHSize, ulVSize);
		}
    }

    BVDC_Window_SetVisibility(hVidWin, true);

    if(pContext->bApplyImmediately)
    {
        if(BERR_SUCCESS != BTST_P_ApplyChanges(pContext))
        {
            BDBG_ERR(("Fail to open window: CMP[%d]WIN[%d] with SRC[%d]",
                pContext->iSelectedComp, pContext->iSelectedWin, pContext->iSelectedVidSrc));
            /* die */
            exit(1);
        }
    }

    if (pContext->bSharedMem)
    {
        BMEM_GetHeapInfo(pContext->hSharedMem, &stHeapEnd);
        printf("After: available shared memory = %d bytes\n", stHeapEnd.ulLargestFree);
        printf("Window memory usage: %d bytes\n",
            stHeapStart.ulLargestFree - stHeapEnd.ulLargestFree);
    }

done:
    BTST_P_PrintStatus(pContext);
    return;
}


/***************************************************************************
 *
 */
void BTST_P_CloseVidWin
    ( BTST_P_Context          *pContext )
{
    BVDC_Window_Handle hVidWin =
        pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
    BMEM_HeapInfo stHeapStart, stHeapEnd;

    if(!hVidWin)
    {
        goto done;
    }

    if (pContext->bSharedMem)
    {
        BMEM_GetHeapInfo(pContext->hSharedMem, &stHeapStart);
        printf("Before closing window: available shared memory = %d bytes\n", stHeapStart.ulLargestFree);
    }

    BVDC_Window_Destroy(hVidWin);

    pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow = NULL;
    pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].iVidSrcId = 0;
	pContext->aulSelectedMDec[pContext->iSelectedComp][pContext->iSelectedWin] = 0;
	pContext->aulSelectedMDecCh[pContext->iSelectedComp][pContext->iSelectedWin] = 0;

    if(pContext->bApplyImmediately)
    {
        BTST_P_ApplyChanges(pContext);
    }

    if (pContext->bSharedMem)
    {
        BMEM_GetHeapInfo(pContext->hSharedMem, &stHeapEnd);
        printf("After closing window: available shared memory = %d bytes\n", stHeapEnd.ulLargestFree);
    }

done:
    BTST_P_PrintStatus(pContext);
    return;
}

#ifdef LINUX
    #include <sys/time.h>

    #define TIMESTAMP_GET(ptime) gettimeofday(ptime, NULL)
    #define TIMESTAMP_SEC(time1, time2) (time2.tv_sec - time1.tv_sec)
    #define TIMESTAMP_USEC(time1, time2) (time2.tv_usec - time1.tv_usec)
    #define TIMESTAMP_USED_IN_MSEC(time1, time2) \
        ((time2.tv_sec-time1.tv_sec)*1000+(time2.tv_usec-time1.tv_usec)/1000)

    typedef struct timeval TIMESTAMP;
#endif

#ifdef VXWORKS
    #include <timers.h>
    #define TIMESTAMP_GET(ptime) clock_gettime(CLOCK_REALTIME, ptime)
    #define TIMESTAMP_SEC(time1, time2) (time2.tv_sec - time1.tv_sec)
    #define TIMESTAMP_USEC(time1, time2) ((time2.tv_nsec - time1.tv_nsec)*1000)

    typedef struct timespec TIMESTAMP;

#endif

#define BTST_DNR_PR17879_NOT_FIXED  0

extern BTST_P_Context *g_pContext;
extern uint32_t g_uiLumaBufSize, g_uiChromaBufSize;
extern void *g_pvLumaStartAddress, *g_pvChromaStartAddress;
extern bool g_bEnablePicListForMosaicMode;
extern bool g_bMosaicWorst;
extern int g_bTestStreamClip;
extern int g_bRandomStrmCrop;
extern uint32_t g_ulCropLeft, g_ulCropTop;
extern uint32_t g_ulCropRight, g_ulCropBot;
extern bool g_bInterceptFrame;

/***************************************************************************
 *
 */
void BTST_P_MpegDataReady_isr
    ( void                            *pvSourceHandle,
      int                              iParm2,
      void                            *pvMvdField )
{
    TIMESTAMP time1, time2;
    BAVC_XVD_Picture *pMvdField = (BAVC_XVD_Picture *)pvMvdField;
    uint32_t ulStripeWidth;

	BVDC_Display_Handle hDisplay = g_pContext->ahDisplay[g_pContext->iSelectedComp];
	BVDC_DisplayTg	 eMasterTg   = g_pContext->eMasterTg[g_pContext->iSelectedComp];
	bool bStgEnabled = false;
	BVDC_Display_StgSettings stStgSetting;

    ulStripeWidth = (pMvdField->eStripeWidth == BAVC_StripeWidth_e128Byte)
        ? 128 : 64;

    /* if requested => intercept a frame to be dumped later on when exiting */
    if(g_pContext->bIntercept && g_bInterceptFrame)
    {
        static uint32_t         uiCnt = 0;
        uint32_t                uiAvcFieldSize = sizeof(BAVC_XVD_Picture);
        uint32_t                bytesPerRow = (( pMvdField->ulSourceHorizontalSize + 15 ) >> 4) << 8;/* a row contains one frame's row */
        uint32_t                totalRows = (pMvdField->ulSourceVerticalSize + 15) >> 4;
        uint32_t                uiLumaBufSize = pMvdField->ulRowStride * (totalRows - 1) + bytesPerRow;/* interleaved frames buffer */
        uint32_t                uiChromaBufSize = pMvdField->ulRowStride * ((totalRows + 1) / 2 - 1) + bytesPerRow;/* interleaved frames buffer */

        BDBG_MSG(("Source Width  = %d\n", pMvdField->ulSourceHorizontalSize));
        BDBG_MSG(("Source Height = %d\n", pMvdField->ulSourceVerticalSize));
        BDBG_MSG(("size of AVC field data structure = %d\n", uiAvcFieldSize));
        BDBG_MSG(("Calculated luma buffer size = %d\n", uiLumaBufSize));
        BDBG_MSG(("Calculated chroma buffer size = %d\n", uiChromaBufSize));
        BDBG_ASSERT(((uiLumaBufSize + uiAvcFieldSize) < g_uiLumaBufSize));
        BDBG_ASSERT((uiChromaBufSize < g_uiChromaBufSize));
        if(++uiCnt == 1)
        {/* only capture the 20th frame from now on */
            BKNI_Memcpy(g_pvLumaStartAddress, pMvdField, uiAvcFieldSize);
            BKNI_Memcpy(
                (void*)((uint32_t)g_pvLumaStartAddress + uiAvcFieldSize),
                pMvdField->pLuminanceFrameBufferAddress,
                uiLumaBufSize);
            BKNI_Memcpy(
                g_pvChromaStartAddress,
                pMvdField->pChrominanceFrameBufferAddress,
                uiChromaBufSize);
        }
    }

    /* MosaicMode test: always feed 16 sub-pictures; */
    if(g_bEnablePicListForMosaicMode)
    {
        int i;
        uint32_t ulStripeNum;
        int iPicIdx = 0;

        uint32_t ulWidth  = pMvdField->ulSourceHorizontalSize / 4;
        uint32_t ulHeight = pMvdField->ulSourceVerticalSize   / 4;
        int iScambleIdx[BAVC_MOSAIC_MAX] = {
            10,  8, 14,  7,
             1,  6,  0, 13,
            11,  5,  4, 15,
             3,  2, 12,  9 };

        /* (0) replace picture size */
        pMvdField->ulSourceHorizontalSize  = ulWidth;
        pMvdField->ulSourceVerticalSize    = ulHeight;
        pMvdField->ulDisplayHorizontalSize = ulWidth;
        pMvdField->ulDisplayVerticalSize   = ulHeight;

        for(i = 0; i < BAVC_MOSAIC_MAX; i++)
        {
            uint32_t ulRow = iScambleIdx[i] / 4, ulCol = iScambleIdx[i] % 4;
            uint32_t ulLumFrameBufAddr   = (uint32_t)pMvdField->pLuminanceFrameBufferAddress;
            uint32_t ulChromFrameBufAddr = (uint32_t)pMvdField->pChrominanceFrameBufferAddress;

            /* skip closed channel(s) */
            if(!g_pContext->abChanOpened[iParm2][i]) continue;

            BDBG_MSG(("channel %d opened", i));

            /* (1) copy the field structure to the picture list */
            BKNI_Memcpy_isr((void*)&g_pContext->astMosaicPictures[iParm2][iPicIdx], pvMvdField, sizeof(BAVC_XVD_Picture));

            /* (2) re-calculate sub-pictures start address */
            /* a stripe is 64 pixels wide */
            ulStripeNum        = ulCol * ulWidth / ulStripeWidth;

            g_pContext->astMosaicPictures[iParm2][iPicIdx].pLuminanceFrameBufferAddress = (void*)
                (ulLumFrameBufAddr +
                 ulStripeNum * pMvdField->ulLuminanceNMBY * 16 * ulStripeWidth +
                 ulRow * ulHeight * ulStripeWidth +
                 ((ulCol * ulWidth) % ulStripeWidth));

            g_pContext->astMosaicPictures[iParm2][iPicIdx].pChrominanceFrameBufferAddress = (void*)
                (ulChromFrameBufAddr +
                /* stripe offset */
                 ulStripeNum * pMvdField->ulChrominanceNMBY * 16 * ulStripeWidth +

                /* vertical offset within a stripe (take care of 422 vs 420 chroma type) */
                 ((ulRow * ulHeight * ulStripeWidth /2) << (pMvdField->eYCbCrType - BAVC_YCbCrType_e4_2_0)) +

                /* horizontal offset within a stripe */
                 ((ulCol * ulWidth) % ulStripeWidth));

            g_pContext->astMosaicPictures[iParm2][iPicIdx].ulChannelId = i;

            /* (3) update picture list pointer */
            if(iPicIdx > 0)
            {
                g_pContext->astMosaicPictures[iParm2][iPicIdx - 1].pNext =
                    (void*)&g_pContext->astMosaicPictures[iParm2][iPicIdx];
            }
            /* (4) if dynamic picture change */
            if(g_bMosaicWorst)
            {
                if((i % 2) == 1)
                {
                    g_pContext->astMosaicPictures[iParm2][iPicIdx].ulSourceHorizontalSize = ulWidth * 2 / 3;
                    g_pContext->astMosaicPictures[iParm2][iPicIdx].ulSourceVerticalSize   = ulHeight * 2 / 3;
                }
            }

            /* increment pic index in the link-list */
            iPicIdx++;
        }

        /* (4) replace the callback picture with the first sub-picture; */
        if(iPicIdx)
        {
            pMvdField = &g_pContext->astMosaicPictures[iParm2][0];
            g_pContext->astMosaicPictures[iParm2][iPicIdx-1].pNext = NULL;
        }
    }

    /* Hi-jack mvdfield to do whatever might be needed. */
    /* if 4x3 do custome */
    BTST_P_CustomAspectRatioCorrection(
        g_pContext->ahDisplay[0],
        g_pContext->ahDisplay[1],
        g_pContext->aWinContext[0][0].hWindow,
        g_pContext->aWinContext[1][0].hWindow,
        pvMvdField);

#if BTST_DNR_PR17879_NOT_FIXED
    pMvdField->ulAdjQp = g_ulAdjQp;
#endif

    if(g_bTestStreamClip)
    {
        static uint32_t ulCnt = 0;
        if(g_bRandomStrmCrop)
        {
            if((ulCnt++) % ulStripeWidth == 0)
            {
                g_ulCropLeft = rand() % pMvdField->ulSourceHorizontalSize/4;
                g_ulCropTop  = rand() % pMvdField->ulSourceVerticalSize/4;
                g_ulCropRight= rand() % pMvdField->ulSourceHorizontalSize/4;
                g_ulCropBot  = rand() % pMvdField->ulSourceVerticalSize/4;
                BDBG_MSG(("left = %d", g_ulCropLeft));
                BDBG_MSG(("top  = %d", g_ulCropTop));
            }
        }
        pMvdField->ulSourceClipLeft = g_ulCropLeft;
        pMvdField->ulSourceClipTop  = g_ulCropTop;
        pMvdField->ulSourceHorizontalSize -= g_ulCropLeft + g_ulCropRight;
        pMvdField->ulSourceVerticalSize   -= g_ulCropTop + g_ulCropBot;
    }

	/* drop frame when it is mute for Transcode*/
	if( BTST_P_USED_STG(eMasterTg)&& hDisplay)
	{
		BVDC_Display_GetStgConfiguration (hDisplay, &bStgEnabled, &stStgSetting);

		if(bStgEnabled)
			pMvdField->bIgnorePicture = pMvdField->bIgnorePicture || pMvdField->bMute;
	}

    /* forced isr drop test */
    if (g_pContext->ulMinIsrDropCnt)
    {
        if(0 ==g_pContext->ulRandIsrDropCnt)
        {
            g_pContext->ulRandIsrDropCnt = rand() % 100 + g_pContext->ulMinIsrDropCnt;
            BDBG_MSG(("Random isr drop count = %d", g_pContext->ulRandIsrDropCnt));

            /* premature return = drop it! */
            return;
        }
        else
        {
            g_pContext->ulRandIsrDropCnt--;
        }
    }

    /* get time stamp */
    TIMESTAMP_GET(&time1);
    BVDC_Source_MpegDataReady_isr(pvSourceHandle, iParm2, (void*)pMvdField);
    TIMESTAMP_GET(&time2);
    /*BDBG_MSG(("Turn-around time for BVDC_Source_MpegDataReady_isr = \n%d seconds %d microseconds",
        TIMESTAMP_SEC(time1, time2), TIMESTAMP_USEC(time1, time2)));*/
    return;
}


/***************************************************************************
 * randomly drop MPEG ready isr to test VDC robustness
 */
void BTST_P_RandDropIsr( BTST_P_Context  *pContext )
{
    printf("Please enter the minimum number of isr has to be reached before a forced drop [0=stop drop]:\n");
    BTST_P_Scanf(pContext, "%u", &pContext->ulMinIsrDropCnt);
}

uint32_t          g_minY, g_maxY;
BPXL_Format       g_ePxlFmt;

/***************************************************************************
 *
 */
void BTST_P_OpenMpgChannel
    ( BTST_P_Context          *pContext )
{
#if BTST_VDC_ONLY
    BSTD_UNUSED(pContext);
#else
    BTST_OpenDecoderChannel(pContext);
#endif
}

/***************************************************************************
 *
 */
void BTST_P_CloseMpgChannel
    ( BTST_P_Context          *pContext )
{
#if BTST_VDC_ONLY
    BSTD_UNUSED(pContext);
#else
    BTST_CloseDecoderChannel(pContext);
#endif
}

#if (BTST_P_SUPPORT_HDR)
/***************************************************************************
 *
 */
static void BTST_P_HdDviCallback_isr(void *pvParm1, int iParm2,
      BAVC_Polarity ePolarity, BAVC_SourceState eSourceState, void **ppvPicture)
{
    BTST_P_Context *pContext = (BTST_P_Context*)pvParm1;

    BSTD_UNUSED(iParm2);
    BSTD_UNUSED(ePolarity);
    BSTD_UNUSED(eSourceState);

    /* Current one! */
    *ppvPicture = &pContext->stHdDviPic[iParm2];
    return;
}

/***************************************************************************
 *
 */
static void BTST_P_HdrCallback_isr(void *context, int iParm2, void *data)
{
    /* Get the my data and hdr data */
    BAVC_VDC_HdDvi_Picture *pCurPic;
    BTST_P_Context *pContext = (BTST_P_Context*)context;
    BAVC_HDMI_AviInfoFrame *pAviInfoFrame = (BAVC_HDMI_AviInfoFrame*)data;

    /* TODO: Why does HDR callbacks with NULL pointer? */
    if((!pAviInfoFrame) || (!pContext))
    {
        BDBG_MSG(("pAviInfoFrame = %d, pContext = %d",
            pAviInfoFrame, pContext));
        return;
    }

    /* Update the pictures */
    pCurPic = &pContext->stHdDviPic[iParm2];

    /* This is what we get from HDR */
    /* pCurPic->eCscMode    = (BAVC_CscMode)pAviInfoFrame->eCscMode; */
    pCurPic->ulPixelRepitionFactor    = pAviInfoFrame->PixelRepeat;
    pCurPic->eColorSpace              = (BAVC_Colorspace)pAviInfoFrame->ePixelEncoding;
    pCurPic->eMatrixCoefficients      = BAVC_MatrixCoefficients_eItu_R_BT_709;
    pCurPic->eTransferCharacteristics = BAVC_TransferCharacteristics_eItu_R_BT_709;

    pCurPic->bHdmiMode = (BAVC_HDMI_PacketStatus_eStopped == pAviInfoFrame->ePacketStatus)
        ? false : true;

    switch (pAviInfoFrame->ePictureAspectRatio) {
    case BAVC_HDMI_AviInfoFrame_PictureAspectRatio_eNoData:
        pCurPic->eAspectRatio = BFMT_AspectRatio_eUnknown;
        break;
    case BAVC_HDMI_AviInfoFrame_PictureAspectRatio_e4_3:
        pCurPic->eAspectRatio = BFMT_AspectRatio_e4_3;
        break;
    default:
        case BAVC_HDMI_AviInfoFrame_PictureAspectRatio_e16_9:
        pCurPic->eAspectRatio = BFMT_AspectRatio_e16_9;
    break;
    }

    BDBG_MSG(("bHdmiMode             = %d", pCurPic->bHdmiMode));
    BDBG_MSG(("eColorSpace           = %d", pCurPic->eColorSpace));
    BDBG_MSG(("ulPixelRepitionFactor = %d", pCurPic->ulPixelRepitionFactor));

    return;
}

/***************************************************************************
 *
 */
void BTST_P_OpenHdrFe
    ( BTST_P_Context          *pContext )
{
    uint32_t i = pContext->iSelectedHfe;

    BHDR_FE_GetDefaultSettings(pContext->hChp, &pContext->stHdrFeSettings[i]);
    if(NULL == pContext->hHdrFe[i])
    {
        BHDR_FE_Open(&pContext->hHdrFe[i],
            pContext->hChp,
            pContext->hReg,
            pContext->hInt,
            &pContext->stHdrFeSettings[i]);

        BHDR_FE_GetDefaultChannelSettings(pContext->hHdrFe[i], &pContext->stHdrFeChSettings[i]);
        pContext->stHdrFeChSettings[i].uiChannel = i ;
        BHDR_FE_OpenChannel(
            pContext->hHdrFe[i],
            &pContext->hHdrFeCh[i],
            &pContext->stHdrFeChSettings[i]);
    }

    return;
}

/***************************************************************************
 *
 */
void BTST_P_CloseHdrFe
    ( BTST_P_Context          *pContext )
{
    uint32_t i = pContext->iSelectedHfe;

    if(pContext->hHdrFe[i])
    {
        BHDR_FE_CloseChannel(pContext->hHdrFeCh[i]);
        pContext->hHdrFeCh[i] = NULL;
        BHDR_FE_Close(pContext->hHdrFe[i]);
        pContext->hHdrFe[i] = NULL;
    }

    return;
}

/***************************************************************************
 *
 */
static void BTST_P_OpenHdrPort
    ( BTST_P_Context          *pContext )
{
    uint32_t i = (pContext->iSelectedVidSrc == BAVC_SourceId_eHdDvi1) ? 1 : 0;

    if(NULL == pContext->hHdrFe[pContext->iSelectedHfe])
    {
        BHDR_FE_GetDefaultSettings(pContext->hChp, &pContext->stHdrFeSettings[pContext->iSelectedHfe]);
        BHDR_FE_Open(&pContext->hHdrFe[pContext->iSelectedHfe],
            pContext->hChp,
            pContext->hReg,
            pContext->hInt,
            &pContext->stHdrFeSettings[pContext->iSelectedHfe]);

        BHDR_FE_GetDefaultChannelSettings(pContext->hHdrFe[pContext->iSelectedHfe],
            &pContext->stHdrFeChSettings[pContext->iSelectedHfe]);
        pContext->stHdrFeChSettings[pContext->iSelectedHfe].uiChannel = pContext->iSelectedHfe ;
        BHDR_FE_OpenChannel(
            pContext->hHdrFe[pContext->iSelectedHfe],
            &pContext->hHdrFeCh[pContext->iSelectedHfe],
            &pContext->stHdrFeChSettings[pContext->iSelectedHfe]);
    }
    /*********************************************
     * open HDMI RX Device Handle
     *********************************************/
    BHDR_GetDefaultSettings(&pContext->stHdrSettings[i]);
    pContext->stHdrSettings[i].hTmr = pContext->hTmr;
    pContext->stHdrSettings[i].eCoreId = i;

    BHDR_Open(&pContext->hHdr[i],
        pContext->hChp,
        pContext->hReg,
        pContext->hInt,
        &pContext->stHdrSettings[i]);

    /**********************************************
     * Attach HDMI Rx Core to Front End Channel
     **********************************************/
    BHDR_FE_AttachHdmiRxCore(pContext->hHdrFeCh[pContext->iSelectedHfe], pContext->hHdr[i]);

    /*******************************************************
     * Attach HDMI Rx Core callback to get color space info.
     ********************************************************/
    BHDR_InstallVideoFormatChangeCallback(pContext->hHdr[i],
        BTST_P_HdrCallback_isr, (void*)pContext, i);

    return;
}


/***************************************************************************
 *
 */
static void BTST_P_CloseHdrPort
    ( BTST_P_Context          *pContext )
{
    uint32_t i = (pContext->iSelectedVidSrc == BAVC_SourceId_eHdDvi1) ? 1 : 0;
    BHDR_Close(pContext->hHdr[i]);
    return;
}
#else
#define BTST_P_OpenHdrPort(pContext)
#define BTST_P_CloseHdrPort(pContext)
#endif


#if (BTST_P_ENABLE_MVD_WD)
/***************************************************************************
 *
 */
/***************************************************************************
 *
 */
void BTST_P_MTPMVWatchdog_isr
    ( void                            *pParm1,
      int                              iVidSrc,
      void                            *pParm3 )
{
    BTST_P_Context          *pContext;
    BSTD_UNUSED(pParm3);

    pContext = (BTST_P_Context *)pParm1;

    BDBG_MSG(("BMVD_ProcessMTPMVWatchdogInterrupt"));
    BKNI_SetEvent(pContext->hMTPMVWatchdogEvent[iVidSrc]);
    return;
}
#endif
/***************************************************************************
 *
 */
static BERR_Code BTST_P_OpenVidSrc_GFX
    ( BTST_P_Context          *pContext,
    BMEM_Handle               hSurMem,
    BVDC_Source_Settings      *pSrcSettings)
{
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
    BSUR_Surface_Handle hTmpSur;
    uint32_t ulWidth                   = BTST_P_DEFAUL_GFX_WIDTH;
    uint32_t ulHeight                  = BTST_P_DEFAUL_GFX_HEIGHT;
    BSUR_Surface_Handle hSurface       = pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface;
    BSUR_Surface_Handle hSurface_R     = pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface_R;
    BVDC_Compositor_Handle hCompositor = pContext->ahCompositor[pContext->iSelectedComp];
    int ans;
    bool bIs3D=false;

    if(pContext->bResourceLeakAutoTest)
    {
         pContext->aSrcContext[pContext->iSelectedVidSrc].eInOrientation = BFMT_Orientation_e2D;
    }
    else 
    {
        printf("Use default gfx surface size? (0=n/1=y)");
        BTST_P_Scanf(pContext, "%d", &ans);
        if (!ans)
        {
            printf("Enter gfx surface width and height: ");
            BTST_P_Scanf(pContext, "%u %u", &ulWidth, &ulHeight);
        }
		if(pContext->stCapabilities.b3DSupport)
		{
	        printf("Is input surface 3D? (0=n/1=y)");
	        BTST_P_Scanf(pContext," %d", &ans);

	        bIs3D = (bool)ans;

			if(!ans)
				pContext->aSrcContext[pContext->iSelectedVidSrc].eInOrientation = BFMT_Orientation_e2D;
			else
			{
				pContext->aSrcContext[pContext->iSelectedVidSrc].eInOrientation = BFMT_Orientation_e3D_LeftRight;
			}
		}
    }

    if((!hCompositor) ||(hVidSrc))
    {
        printf("\ncmp %d[%p] not open\n", pContext->iSelectedComp, (void*)hCompositor);
        return BERR_INVALID_PARAMETER;
    }

    if(BPXL_eA0 == pContext->eGraphicsFormat)
    {
        if(!hSurface)
        {
            BSUR_Surface_Create    (hSurMem, ulWidth, ulHeight, 0, NULL, BPXL_eA0, NULL, 0, NULL, &hSurface);
            if(bIs3D)
                BSUR_Surface_Create    (hSurMem, ulWidth, ulHeight, 0, NULL, BPXL_eA0, NULL, 0, NULL, &hSurface_R);

            pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface   = hSurface;
            pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface_R = hSurface_R;
        }

        BVDC_Source_Create(pContext->hVdc, &hVidSrc, pContext->iSelectedVidSrc, pSrcSettings);
        BVDC_Source_SetConstantColor(hVidSrc, 0, 0,0xff);
    }
    else if (BPXL_eP0 == pContext->eGraphicsFormat)
    {
        if(!hSurface)
        {
            BERR_Code err;
            uint32_t *pvAddress;
            BSUR_Palette_Handle hPalette = 0;

            err = BSUR_Palette_Create( hSurMem, BPXL_NUM_PALETTE_ENTRIES(BPXL_eP0),
                NULL, BPXL_eA8_R8_G8_B8, BSUR_CONSTRAINT_ADDRESS_PIXEL_ALIGNED, &hPalette);
            if( err != BERR_SUCCESS )
            {
				printf("\n Palette create failure");
				return err;
            }

            /* Create surface */
            BSUR_Surface_Create (hSurMem, ulWidth, ulHeight, 0, NULL, BPXL_eP0, NULL, 0, NULL, &hSurface);
            if( err != BERR_SUCCESS )
            {
				printf("\n surface create failure");
				return (err);
            }

            /* Set palette */
            BSUR_Palette_GetAddress(hPalette,(void**)&pvAddress );
            BSUR_Surface_SetPalette( hSurface, hPalette );
            BGFX_MakePalette( pvAddress , BPXL_BITS_PER_PIXEL(BPXL_eP0));

            if(bIs3D)
            {
                /* Create R surface */
                BSUR_Surface_Create (hSurMem, ulWidth, ulHeight, 0, NULL, BPXL_eP0, NULL, 0, NULL, &hSurface_R);
                if( err != BERR_SUCCESS )
                {
					printf("\n right surface create failure");
					return (err);
                }

                /* Set palette */
                BSUR_Surface_SetPalette( hSurface_R, hPalette);
                BGFX_MakePalette(pvAddress , BPXL_BITS_PER_PIXEL(BPXL_eP0));
            }

            pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface   = hSurface;
            pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface_R = hSurface_R;
        }

        BVDC_Source_Create(pContext->hVdc, &hVidSrc, pContext->iSelectedVidSrc, pSrcSettings);
        BVDC_Source_SetConstantColor(hVidSrc, 0, 0,0xff);
    }
    else 
    {
            if(!hSurface)
            {
                 if(!pContext->bResourceLeakAutoTest) printf("Create hTmpSur %d x %d\n", ulWidth, ulHeight);
                BSUR_Surface_Create    (hSurMem, ulWidth, ulHeight, 0, NULL, BPXL_eA8_R8_G8_B8, NULL, 0, NULL, &hTmpSur);
                BGFX_MakeImageARGB8888 (hTmpSur);

                 if(!pContext->bResourceLeakAutoTest) printf("Create hSurface %d x %d\n", ulWidth, ulHeight);
                BSUR_Surface_Create    (hSurMem, ulWidth, ulHeight, 0, NULL, pContext->eGraphicsFormat, NULL, 0, NULL, &hSurface);
                BGFX_CopyImage         (hSurface, hTmpSur);
                hSurface_R = NULL;

                if(bIs3D)
                {
                    BSUR_Surface_Create    (hSurMem, ulWidth, ulHeight, 0, NULL, pContext->eGraphicsFormat, NULL, 0, NULL, &hSurface_R);
                    BGFX_CopyImage         (hSurface_R, hTmpSur);
                    printf("Create hSurface_R %d x %d %x %x\n", ulWidth, ulHeight, (uint32_t)hSurface, (uint32_t)hSurface_R);
                }
                BSUR_Surface_Destroy   (hTmpSur);

                pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface   = hSurface;
                pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface_R = hSurface_R;
            }

            BVDC_Source_Create(pContext->hVdc, &hVidSrc, pContext->iSelectedVidSrc, pSrcSettings);
    }

	pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc = hVidSrc;
	return (BERR_SUCCESS);
}
/***************************************************************************
 *
 */
static BERR_Code BTST_P_OpenVidSrc_MPEG
    ( BTST_P_Context          *pContext,
      BVDC_Source_Settings      *pSrcSettings)
{
	
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	BVDC_Display_Handle hDisplay  = pContext->ahDisplay[pContext->iSelectedComp];
    BMEM_HeapInfo stHeapStart, stHeapEnd;
	BFMT_VideoFmt eFmt;

	if (pContext->bSharedMem)
	{
		pSrcSettings->hHeap = pContext->hSharedHeap;
		printf("Using shared memory for MPEG source\n");

		BMEM_GetHeapInfo(pContext->hSharedMem, &stHeapStart);
		printf("Before: available shared memory = %d bytes\n", stHeapStart.ulLargestFree);
	}

	/* Open vdc mpeg-source. */
	pSrcSettings->bGfxSrc = false;
	BVDC_Source_Create(pContext->hVdc, &hVidSrc, pContext->iSelectedVidSrc, pSrcSettings);

	if (pContext->bSharedMem)
	{
		BMEM_GetHeapInfo(pContext->hSharedMem, &stHeapEnd);
		printf("After : available shared memory = %d bytes\n", stHeapEnd.ulLargestFree);
		printf("MPEG source memory usage: %d bytes\n",
			stHeapStart.ulLargestFree - stHeapEnd.ulLargestFree);
	}


	/* set mute color */
	BVDC_Source_SetVideoMuteColor( hVidSrc, 0x0, 0x0, 0xff);

	if (pContext->bStatic[pContext->iSelectedVidSrc])
	{
		uint32_t ulPattern;
		uint32_t ulFrames;
		uint32_t ulFormatIndex;
		BFMT_VideoFmt eaSrcFormat[] =
			{ BFMT_VideoFmt_eNTSC, BFMT_VideoFmt_e480p, BFMT_VideoFmt_e720p,
			  BFMT_VideoFmt_e1080i, BFMT_VideoFmt_e1080p};

		printf("\nHow many frames do you want to allocate static frame buffer for?\n");
		BTST_P_Scanf(pContext, "%u", &ulFrames);
		if(ulFrames > 4) ulFrames = 4;

		printf("\nSelect src format: 480i(0), 480p(1), 720p(2), 1080i(3), 1080p(4)\n");
		BTST_P_Scanf(pContext, "%u", &ulFormatIndex);

		printf("\nSelect orientation for input: 2D(0), 3D_LR(1), 3D_OU(2), 3D_L_R(3), 3D_LR_Enhanced(5)\n");
		BTST_P_Scanf(pContext, "%u", &pContext->eSrcOrientation);

		printf("\nWhat test pattern do you want?\n");
		printf("0. Fixed color\n");
		printf("1. Color Bar\n");
		printf("2. Check Board\n");
		printf("3. Cross Hatch\n");
		printf("4. Luma Ramp - Horizontal\n");
		printf("5. Luma Ramp - Vertical\n");
		printf("6. Color Bar - Horizontal\n");
		printf("7. Circle-Diag-Cross\n");
		printf("8: Load a 24-bit Bitmap Image \n");
		printf("Please select between 0 and 8: \n");
		BTST_P_Scanf(pContext, "%u", &ulPattern);
		if(ulPattern > 8) ulPattern = 8;

		if( (ulPattern == 4) || (ulPattern == 5) )
		{
			printf("Please set minLuma: \n");
			BTST_P_Scanf(pContext, "%u", &g_minY);
			printf("Please set maxLuma: \n");
			BTST_P_Scanf(pContext, "%u", &g_maxY);
		}

#if (BFramework_NUM_XVD_CORES > 0)
#if (BTST_P_HACK_MFD_RTS)
		{
			uint32_t  ulReg;

			/* Adjust RTS settings */
			/* MFD_0 */
			pContext->aulMfdRts[0] = BREG_Read32(pContext->hReg,
				BCHP_MEMC_0_CLIENT_INFO_40);
			ulReg = pContext->aulMfdRts[0];
			ulReg &= ~(BCHP_MASK(MEMC_0_CLIENT_INFO_40, BO_VAL));
			ulReg |= BCHP_FIELD_DATA(MEMC_0_CLIENT_INFO_40, BO_VAL, 0x8b);
			BREG_Write32(pContext->hReg, BCHP_MEMC_0_CLIENT_INFO_40, ulReg);

			/* MFD_1 */
			pContext->aulMfdRts[1] = BREG_Read32(pContext->hReg,
				BCHP_MEMC_0_CLIENT_INFO_39);
			ulReg = pContext->aulMfdRts[1];
			ulReg &= ~(BCHP_MASK(MEMC_0_CLIENT_INFO_39, BO_VAL));
			ulReg |= BCHP_FIELD_DATA(MEMC_0_CLIENT_INFO_39, BO_VAL, 0x8b);
			BREG_Write32(pContext->hReg, BCHP_MEMC_0_CLIENT_INFO_39, ulReg);
		}
#endif
#endif
		/* initialize static buffers  */
		InitializeMpegStaticSrc(pContext, hVidSrc, eaSrcFormat[ulFormatIndex],
			ulFrames, true, ulPattern);
	}
	else
	{
		if (IS_XVD(pContext->eSelectedMDec))
		{
#if  BTST_VDC_ONLY
			BSTD_UNUSED(hDisplay);
			BSTD_UNUSED(eFmt);
#else
#if (BFramework_NUM_XVD_CORES > 0)
			BINT_Id  TopInterruptName;
			BINT_Id  BotInterruptName;
			BINT_Id  ProgressiveInterruptName;
			BERR_Code			   err=BERR_SUCCESS;
#if BTST_P_SUPPORT_MULTI_DECODE
			BXVD_DeviceVdcInterruptSettings VDCDevIntrSettings;
			BXVD_DisplayInterrupt			eDisplayInterrupt;
			BXVD_Handle 					hXvd = (BXVD_Handle)pContext->astMDecInfo[pContext->eSelectedMDec].hMDec;
			BXVD_DeviceInterrupt			eXvdDeviceInterrupt;
#endif
			BXVD_ChannelHandle hXvdChn;

			/* Open a MPEG channel if none is openned */
			if(!pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh])
				BTST_OpenDecoderChannel(pContext);

			hXvdChn = (BXVD_ChannelHandle)pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh];

			/***************************************************************/
			/* hook Mpeg decoder and VDC together */
			/* get interrupt name for top field (to drive Mpeg decoder) */
			TestError( BVDC_Source_GetInterruptName(
				hVidSrc,
				BAVC_Polarity_eTopField,
				&TopInterruptName),
				"ERROR: BVDC_Source_GetInterruptName()" );

			/* get interrupt name for bottom field (to drive Mpeg decoder) */
			TestError( BVDC_Source_GetInterruptName(
				hVidSrc,
				BAVC_Polarity_eBotField,
				&BotInterruptName),
				"ERROR: BVDC_Source_GetInterruptName()" );

			/* get interrupt name for progressive frame (to drive Mpeg decoder) */
			TestError( BVDC_Source_GetInterruptName(
				hVidSrc,
				BAVC_Polarity_eFrame,
				&ProgressiveInterruptName),
				"ERROR: BVDC_Source_GetInterruptName()" );

#if BTST_P_SUPPORT_MULTI_DECODE
			BXVD_GetVdcDeviceInterruptDefaultSettings(hXvd, &VDCDevIntrSettings);
			VDCDevIntrSettings.VDCIntId_Topfield = TopInterruptName;
			VDCDevIntrSettings.VDCIntId_Botfield = BotInterruptName;
			VDCDevIntrSettings.VDCIntId_Frame = ProgressiveInterruptName;

			eDisplayInterrupt = (pContext->iSelectedMDecDm == 0) ? BXVD_DisplayInterrupt_eZero :
				(pContext->iSelectedMDecDm + BXVD_DisplayInterrupt_eZero);

			VDCDevIntrSettings.eDisplayInterrupt = eDisplayInterrupt;

			/* Register for each MFD */
			TestError( BXVD_RegisterVdcDeviceInterrupt(hXvd, &VDCDevIntrSettings), "BXVD_RegisterVdcDeviceInterrupt");

			eXvdDeviceInterrupt = (eDisplayInterrupt == BXVD_DisplayInterrupt_eZero) ?
				BXVD_DeviceInterrupt_ePictureDataReady0 : BXVD_DeviceInterrupt_ePictureDataReady1;

			/* Install Interrupt Callback */
			TestError( BXVD_InstallDeviceInterruptCallback(
						hXvd,
						eXvdDeviceInterrupt,
						BTST_P_MpegDataReady_isr,
						(void *)hVidSrc,
						0),
						"BXVD_InstallDeviceInterruptCallback");
#else
			/* pass interrupt names to XVD. They need to handle these interrupts
			and (eventually) call the VDC callback when they fire */
			TestError( BXVD_RegisterVdcInterrupt(
				hXvdChn,
				TopInterruptName,
				BAVC_Polarity_eTopField) ,
				"ERROR: BXVD_RegisterVdcInterrupt() TopField " );
			TestError( BXVD_RegisterVdcInterrupt(
				hXvdChn,
				BotInterruptName,
				BAVC_Polarity_eBotField) ,
				"ERROR: BXVD_RegisterVdcInterrupt() BotField" );
			TestError( BXVD_RegisterVdcInterrupt(
				hXvdChn,
				ProgressiveInterruptName,
				BAVC_Polarity_eFrame) ,
				"ERROR: BXVD_RegisterVdcInterrupt() ProgressiveFrame" );

			/* register VDC callback with XVD */
			TestError( BXVD_InstallInterruptCallback(
				hXvdChn,
				BXVD_Interrupt_ePictureDataReady,
				BTST_P_MpegDataReady_isr,
				(void *)hVidSrc,
				pContext->iSelectedVidSrc ),
				"ERROR: BXVD_InstallInterruptCallback()" );
#endif
			/* for 1080p content */
			if (hDisplay)
			{
				BVDC_Display_GetVideoFormat( hDisplay, &eFmt );
				if (eFmt == BFMT_VideoFmt_e1080p_24Hz)
					BXVD_SetMonitorRefreshRate(hXvdChn, BFMT_VERT_24Hz);
				else if(eFmt == BFMT_VideoFmt_e1080p_25Hz)
					BXVD_SetMonitorRefreshRate(hXvdChn, BFMT_VERT_25Hz);
				else if(eFmt == BFMT_VideoFmt_e1080p_30Hz)
					BXVD_SetMonitorRefreshRate(hXvdChn, BFMT_VERT_30Hz);
			}
#endif
#endif
		}
	}

	pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc = hVidSrc;
	return (BERR_SUCCESS);
#if  (!BTST_VDC_ONLY)
Done:
    BTST_P_PrintStatus(pContext);
    return (BERR_UNKNOWN);
#endif
}
/***************************************************************************
 *
 */
static BERR_Code BTST_P_OpenVidSrc_GfxToMfdVfd
    ( BTST_P_Context            *pContext,
      BMEM_Handle               hSurMem,
      BVDC_Source_Settings      *pSrcSettings)
{
    BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	BSUR_Surface_Handle hTmpSur;
	uint32_t ulWidth				   = BTST_P_DEFAUL_GFX_WIDTH;
	uint32_t ulHeight				   = BTST_P_DEFAUL_GFX_HEIGHT;
	bool bIs3D = false;
	BSUR_Surface_Handle hSurface = pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface;
	BSUR_Surface_Handle hSurface_R = pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface_R;
	BSUR_Surface_Handle hIntrSurface   = pContext->aSrcContext[pContext->iSelectedVidSrc].hIntrSurface;
	BSUR_Surface_Handle hGoldSurface   = pContext->aSrcContext[pContext->iSelectedVidSrc].hGoldSurface;
	BSUR_Surface_Handle hIntrSurface_R	 = pContext->aSrcContext[pContext->iSelectedVidSrc].hIntrSurface_R;
	BSUR_Surface_Handle hGoldSurface_R	 = pContext->aSrcContext[pContext->iSelectedVidSrc].hGoldSurface_R;
	BVDC_Compositor_Handle hCompositor = pContext->ahCompositor[pContext->iSelectedComp];
	int ans;
	pContext->aSrcContext[pContext->iSelectedVidSrc].eInOrientation = BFMT_Orientation_e2D;


	if (!pContext->bVfdAsSourceAutoTest)
	{
		printf("Use default surface size? (0=n/1=y)");
		scanf("%d", &ans);
		if (!ans)
		{
			printf("Enter surface surface width and height: ");
			scanf("%u %u", &ulWidth, &ulHeight);
		}
		if(pContext->stCapabilities.b3DSupport)
		{
				printf("Is surface 3D? (0=n/1=y)");
				scanf("%d", &ans);

			if (ans)
			{
				bIs3D = true;
				pContext->aSrcContext[pContext->iSelectedVidSrc].eInOrientation = BFMT_Orientation_e3D_LeftRight;
			}
			else
			{
				bIs3D = false;
				pContext->aSrcContext[pContext->iSelectedVidSrc].eInOrientation = BFMT_Orientation_e2D;
			}
		}
	}

	if((!hCompositor) ||(hVidSrc))
	{
		printf("\n cmp %d[%p] not open\n", pContext->iSelectedComp, (void*)hCompositor);
		return (BERR_INVALID_PARAMETER);
	}

	if (!pContext->bVfdAsSourceAutoTest)
	{
		printf("\nChoose a pixel format\n");

		printf("0. BPXL_eY18_Cr8_Y08_Cb8\n");
		printf("1. BPXL_eCr8_Y18_Cb8_Y08\n");
		printf("2. BPXL_eY18_Cb8_Y08_Cr8\n");
		printf("3. BPXL_eCb8_Y18_Cr8_Y08\n");
		printf("4. BPXL_eY08_Cb8_Y18_Cr8\n");
		printf("5. BPXL_eCb8_Y08_Cr8_Y18\n");
		printf("6. BPXL_eY08_Cr8_Y18_Cb8\n");
		printf("7. BPXL_eCr8_Y08_Cb8_Y18\n");
		printf("8. BPXL_eX2_Cr10_Y10_Cb10\n");
		BTST_P_Scanf(pContext, "%d", &ans);
		switch (ans)
		{
			case 0: g_ePxlFmt = BPXL_eY18_Cr8_Y08_Cb8; break;
			case 1: g_ePxlFmt = BPXL_eCr8_Y18_Cb8_Y08; break;
			case 2: g_ePxlFmt = BPXL_eY18_Cb8_Y08_Cr8; break;
			case 3: g_ePxlFmt = BPXL_eCb8_Y18_Cr8_Y08; break;
			case 4: g_ePxlFmt = BPXL_eY08_Cb8_Y18_Cr8; break;
			case 5: g_ePxlFmt = BPXL_eCb8_Y08_Cr8_Y18; break;
			case 6: g_ePxlFmt = BPXL_eY08_Cr8_Y18_Cb8; break;
			case 7: g_ePxlFmt = BPXL_eCr8_Y08_Cb8_Y18; break;
			case 8: g_ePxlFmt = BPXL_eX2_Cr10_Y10_Cb10; break;
			default: g_ePxlFmt = BPXL_eY18_Cr8_Y08_Cb8; break;
		}
	}
	else
	{
		g_ePxlFmt = BPXL_eY18_Cr8_Y08_Cb8;
	}

	if(!hSurface)
	{
		uint32_t ulPattern;

		if (!pContext->bVfdAsSourceAutoTest)
		{
			printf("\nWhat test pattern do you want?\n");
			printf("0. Fixed color\n");
			printf("1. Color Bar\n");
			printf("2. Check Board\n");
			printf("3. Cross Hatch\n");
			printf("4. Luma Ramp - Horizontal\n");
			printf("5. Luma Ramp - Vertical\n");
			printf("6. Single color bar\n");
			printf("7. VDC team pattern\n");
			printf("Please select between 0 and 7: \n");
			scanf("%u", &ulPattern);
		}
		else
		{
			ulPattern = 1;
		}

		if (ulPattern <= 6)
		{
			if (BTST_MakeSurfaces(hSurMem, &hTmpSur, 1,
					ulPattern, g_ePxlFmt,
					ulWidth, ulHeight, false) == 1)
			{
				printf("\n Failed to create surface\n");
				return (BERR_UNKNOWN);
			}
		}
		else 
		{
			BSUR_Surface_Create(hSurMem, ulWidth, ulHeight, 0, NULL, BPXL_eA8_R8_G8_B8, NULL, 0, NULL, &hTmpSur);
			BGFX_MakeImageARGB8888(hTmpSur);
		}
		BGFX_DumpImage(hTmpSur, 0, 0);
		
		BSUR_Surface_Create    (hSurMem, ulWidth, ulHeight, 0, NULL, g_ePxlFmt, NULL, 0, NULL, &hSurface);
		BGFX_CopyImage		   (hSurface, hTmpSur);
		if (bIs3D)
		{
			BSUR_Surface_Create    (hSurMem, ulWidth, ulHeight, 0, NULL, g_ePxlFmt, NULL, 0, NULL, &hSurface_R);
			BGFX_CopyImage		   (hSurface_R, hTmpSur);
		}
			
		/*printf("Create Interrupt hSurface %d x %d\n", ulWidth, ulHeight);*/
		BSUR_Surface_Create    (hSurMem, ulWidth, ulHeight, 0, NULL, g_ePxlFmt, NULL, 0, NULL, &hIntrSurface);
		BGFX_CopyImage		   (hIntrSurface, hTmpSur);
		if (bIs3D)
		{
			BSUR_Surface_Create    (hSurMem, ulWidth, ulHeight, 0, NULL, g_ePxlFmt, NULL, 0, NULL, &hIntrSurface_R);
			BGFX_CopyImage		   (hIntrSurface_R, hTmpSur);
		}
			
		BSUR_Surface_Destroy   (hTmpSur);
			
		BSUR_Surface_Create    (hSurMem, ulWidth, ulHeight, 0, NULL, BPXL_eA8_R8_G8_B8, NULL, 0, NULL, &hTmpSur);	
		BGFX_MakeImageARGB8888 (hTmpSur);
		BGFX_DumpImage(hTmpSur, 0, 0);

		/*printf("Create golden hSurface %d x %d\n", ulWidth, ulHeight);*/
		BSUR_Surface_Create    (hSurMem, ulWidth, ulHeight, 0, NULL, g_ePxlFmt, NULL, 0, NULL, &hGoldSurface);
		BGFX_CopyImage		   (hGoldSurface, hTmpSur);
		if (bIs3D)
		{
			BSUR_Surface_Create    (hSurMem, ulWidth, ulHeight, 0, NULL, g_ePxlFmt, NULL, 0, NULL, &hGoldSurface_R);
				BGFX_CopyImage		   (hGoldSurface_R, hTmpSur);
		}
		
		BSUR_Surface_Destroy   (hTmpSur);

		pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface = hSurface;
		pContext->aSrcContext[pContext->iSelectedVidSrc].hIntrSurface = hIntrSurface;
		pContext->aSrcContext[pContext->iSelectedVidSrc].hGoldSurface = hGoldSurface;

		if (bIs3D)
		{
			BDBG_ERR(("crated surface _R"));
			pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface_R = hSurface_R;
			pContext->aSrcContext[pContext->iSelectedVidSrc].hIntrSurface_R = hIntrSurface_R;
			pContext->aSrcContext[pContext->iSelectedVidSrc].hGoldSurface_R = hGoldSurface_R;
		}
	}

	pSrcSettings->bGfxSrc = true;
	BVDC_Source_Create(pContext->hVdc, &hVidSrc, pContext->iSelectedVidSrc, pSrcSettings);

	pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc = hVidSrc;
	return (BERR_SUCCESS);
	
}
/***************************************************************************
 *
 */
static BERR_Code BTST_P_OpenVidSrc_HDDVI
    ( BTST_P_Context            *pContext,
      BVDC_Source_Settings      *pSrcSettings)
{
    BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	
#if (BTST_P_SUPPORT_HDR)
	BTST_P_OpenHdrPort(pContext);
#endif
	BVDC_Source_Create(pContext->hVdc, &hVidSrc, pContext->iSelectedVidSrc, pSrcSettings);
	BVDC_Source_SetVideoFormat(hVidSrc, BFMT_VideoFmt_e1080i);
	BVDC_Source_SetAutoFormat(hVidSrc, true, NULL, 0);
#if (BTST_P_SUPPORT_HDR)
	BVDC_Source_SetInputPort(hVidSrc, BVDC_HdDviInput_Hdr);
	BVDC_Source_InstallPictureCallback(hVidSrc,
		BTST_P_HdDviCallback_isr, (void*)pContext,
		(BAVC_SourceId_eHdDvi0 == pContext->iSelectedVidSrc) ? 0 : 1);
#elif (BTST_SUPPORT_DVO_HDDVI_LOOPBACK)
	BVDC_Source_SetInputPort(hVidSrc, BVDC_HdDviInput_Ext);
#endif

	pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc = hVidSrc;
	return (BERR_SUCCESS);
}
/***************************************************************************
 *
 */
static BERR_Code BTST_P_OpenVidSrc_656
    ( BTST_P_Context            *pContext,
      BVDC_Source_Settings      *pSrcSettings)
{
    BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	
	BVDC_Source_Create(pContext->hVdc, &hVidSrc, pContext->iSelectedVidSrc, pSrcSettings);
	BVDC_Source_SetVideoFormat(hVidSrc, BFMT_VideoFmt_eNTSC);
#if (BCHP_CHIP == 7400)
#include "bchp_sun_top_ctrl.h"
	{
		uint32_t reg;
		/* Set GPIO 81->89 for 656in_0. */
		reg = BREG_Read32(pContext->hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10);
		reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_081 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_082 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_083 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_084 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_085 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_086 )
				);
		reg |=
			BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_081, 1 ) |
			BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_082, 1 ) |
			BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_083, 1 ) |
			BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_084, 1 ) |
			BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_085, 1 ) |
			BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_086, 1 );
		BREG_Write32(pContext->hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, reg);
	}
#endif

	pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc = hVidSrc;
	return (BERR_SUCCESS);

}
/***************************************************************************
 *
 */
static BERR_Code BTST_P_OpenVidSrc_DS
    ( BTST_P_Context          *pContext)
{
    BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
    BVDC_Source_Settings stDefaultSettings;
    int32_t iCompositorId = 1;

    stDefaultSettings.hHeap = NULL;

    printf("Select Downsample Source Compositor: Compositor 0(0), Compositor 1(1)\n");
    BTST_P_Scanf(pContext, "%d", &iCompositorId);

    stDefaultSettings.eDsSrcCompId = (iCompositorId == 1)
        ? BVDC_CompositorId_eCompositor1 : BVDC_CompositorId_eCompositor0;

    /* (1b) Open DS_0 source . */
    /* TODO: install callback */
    BVDC_Source_Create(pContext->hVdc, &hVidSrc, pContext->iSelectedVidSrc, &stDefaultSettings);
	pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc = hVidSrc;
	return (BERR_SUCCESS);
}
/***************************************************************************
 *
 */
void BTST_P_OpenVidSrc
    ( BTST_P_Context          *pContext )
{
    BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
    const BFMT_VideoInfo *pFmtInfo = pContext->apFmtInfo[pContext->iSelectedComp];
    BVDC_Source_Settings  stSrcSettings;
    int32_t uiMemIdx;
    BMEM_Handle hSurMem = pContext->astMemInfo[0].hMem;
    BERR_Code err = BERR_SUCCESS;

    if((!pContext->hVdc) || (!pFmtInfo) || (hVidSrc))
    {
        if (hVidSrc)
            printf("src[%p] already opened\n", (void*)hVidSrc);

        goto Done;
    }

    BVDC_Source_GetDefaultSettings(pContext->iSelectedVidSrc, &stSrcSettings);

	uiMemIdx = pContext->uSrcMemIdx[pContext->iSelectedVidSrc - BAVC_SourceId_eMpeg0];
	BDBG_ASSERT(uiMemIdx!=-1);
	stSrcSettings.hHeap = pContext->astMemInfo[uiMemIdx].hVdcHeap;
	hSurMem = pContext->astMemInfo[uiMemIdx].hMem;

	if(!pContext->bResourceLeakAutoTest)
		printf("\nUse MEMC[%d] to open source%d\n",uiMemIdx, pContext->iSelectedVidSrc);

    if(BTST_P_IS_GFX_SRC (pContext->iSelectedVidSrc))
    {
		err = BTST_P_OpenVidSrc_GFX(pContext, hSurMem, &stSrcSettings);
    }
	else if (BTST_P_IS_HDDVI_SRC(pContext->iSelectedVidSrc))
	{
		err = BTST_P_OpenVidSrc_HDDVI(pContext,&stSrcSettings);
	}
    else if (BTST_P_IS_DS_SRC(pContext->iSelectedVidSrc))
    {
		err = BTST_P_OpenVidSrc_DS(pContext);
    }
    else if (BTST_P_IS_656_SRC(pContext->iSelectedVidSrc))
    {
		err = BTST_P_OpenVidSrc_656(pContext, &stSrcSettings);
    }
	else if (BTST_P_IS_VDEC_SRC(pContext->iSelectedVidSrc))
	{
		printf("\n No VDEC input support %x", BCHP_CHIP);
	}
	else if (BTST_P_IS_MPEG_SRC(pContext->iSelectedVidSrc))
	{
		if (pContext->ulGfx2Mfd[pContext->iSelectedVidSrc])
		{
			err = BTST_P_OpenVidSrc_GfxToMfdVfd(pContext, hSurMem, &stSrcSettings);
		}
		else
		{
			err = BTST_P_OpenVidSrc_MPEG(pContext, &stSrcSettings);
		}
	}
	else if (BTST_P_IS_VFD_SRC(pContext->iSelectedVidSrc))
    {
		err = BTST_P_OpenVidSrc_GfxToMfdVfd(pContext, hSurMem, &stSrcSettings);
    }
	else
    {
		printf("Invalid source [%d]!\n", pContext->iSelectedVidSrc);
        BDBG_ASSERT(false);
     }

	if(err != BERR_SUCCESS)
		goto Done;

    if(pContext->bApplyImmediately)
    {
        BTST_P_ApplyChanges(pContext);
    }

Done:
    BTST_P_PrintStatus(pContext);
    return;
}

/***************************************************************************
 *
 */
static BERR_Code BTST_P_CloseVidSrc_GFX
    ( BTST_P_Context          *pContext )
{
    BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;

	BSUR_Surface_Handle hSurface = pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface;
    BSUR_Surface_Handle hSurface_R = pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface_R;
    BAVC_Gfx_Picture *pGfxBuf    = pContext->aSrcContext[pContext->iSelectedVidSrc].pGfxBuf;

    if((!hSurface) || (!hVidSrc))
    {
        return (BERR_UNKNOWN);
    }
    BSUR_Surface_Destroy(hSurface);
    pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface = NULL;
    if(hSurface_R)
    {
        BSUR_Surface_Destroy(hSurface_R);
    }
    pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface_R = NULL;

    if (pGfxBuf)
    {
        BKNI_Memset((void*)pGfxBuf, 0xA3, sizeof(BAVC_Gfx_Picture));
        BKNI_Free((void*)pGfxBuf);
        pContext->aSrcContext[pContext->iSelectedWin].pGfxBuf = NULL;
    }

    pContext->aSrcContext[pContext->iSelectedWin].hVidSrc =NULL;
	return (BERR_SUCCESS);
}

/***************************************************************************
 *
 */
static BERR_Code BTST_P_CloseVidSrc_MPEG
    ( BTST_P_Context          *pContext )
{
	if (pContext->bStatic[pContext->iSelectedVidSrc])
	{
#if (BTST_P_HACK_MFD_RTS)
		/* Restore default RTS settings */
		/* MFD_0 */
		BREG_Write32(pContext->hReg, BCHP_MEMC_0_CLIENT_INFO_40,
			pContext->aulMfdRts[0]);

		/* MFD_1 */
		BREG_Write32(pContext->hReg, BCHP_MEMC_0_CLIENT_INFO_39,
			pContext->aulMfdRts[1]);
#endif
		/* free frame buffer and destroy callbacks */
		TerminateMpegStaticSrc(pContext);
	}

#if  !BTST_VDC_ONLY
	else
	{
		BERR_Code err;
		BTST_MDEC_ChannelHandle hMpgChn = pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh];

#if BTST_P_SUPPORT_MULTI_DECODE
		BXVD_Handle  hXvd = (BXVD_Handle)pContext->astMDecInfo[pContext->eSelectedMDec].hMDec;
		BXVD_DeviceInterrupt			eXvdDeviceInterrupt;
		BXVD_DisplayInterrupt			eDisplayInterrupt;

		/* Un-Install Interrupt Callback */
		eDisplayInterrupt = (pContext->iSelectedMDecDm == 0) ? BXVD_DisplayInterrupt_eZero :
			(pContext->iSelectedMDecDm + BXVD_DisplayInterrupt_eZero);		
		eXvdDeviceInterrupt = (eDisplayInterrupt == BXVD_DisplayInterrupt_eZero) ?
			BXVD_DeviceInterrupt_ePictureDataReady0 : BXVD_DeviceInterrupt_ePictureDataReady1;
		
		TestError( BXVD_UnInstallDeviceInterruptCallback(
					   hXvd,
					   eXvdDeviceInterrupt),
				   "ERROR: BXVD_InstallInterruptCallback()" );
#else
		/* un-register VDC callback with XVD */
		TestError( BXVD_UnInstallInterruptCallback(
					   hMpgChn,
					   BXVD_Interrupt_ePictureDataReady),
				   "ERROR: BXVD_InstallInterruptCallback()" );
#endif		

		/* Close any openned MPEG channel*/
		if(pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh])
			BTST_CloseDecoderChannel(pContext);

		if (IS_XVD(pContext->eSelectedMDec))
		{
#if (BFramework_NUM_XVD_CORES > 0)
			BSTD_UNUSED(hMpgChn);
#endif
		}
	}
  Done:
#endif

	return (BERR_SUCCESS);
}
/***************************************************************************
 *
 */
static BERR_Code BTST_P_CloseVidSrc_GfxToMfdVfd
    ( BTST_P_Context          *pContext )
{
	BSUR_Surface_Handle hSurface = pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface;
	BSUR_Surface_Handle hIntrSurface = pContext->aSrcContext[pContext->iSelectedVidSrc].hIntrSurface;
	BSUR_Surface_Handle hGoldSurface = pContext->aSrcContext[pContext->iSelectedVidSrc].hGoldSurface;
	BSUR_Surface_Handle hSurface_R = pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface_R;
	BSUR_Surface_Handle hIntrSurface_R = pContext->aSrcContext[pContext->iSelectedVidSrc].hIntrSurface_R;
	BSUR_Surface_Handle hGoldSurface_R = pContext->aSrcContext[pContext->iSelectedVidSrc].hGoldSurface_R;
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	
	if (!hVidSrc)
	{
        return (BERR_UNKNOWN);
	}
	
	if (hSurface)
	{
		BSUR_Surface_Destroy(hSurface);
	}
	pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface = NULL;
	
	if (hIntrSurface)
	{
		BSUR_Surface_Destroy(hIntrSurface);
	}
	pContext->aSrcContext[pContext->iSelectedVidSrc].hIntrSurface = NULL;
	
	if (hGoldSurface)
	{
		BSUR_Surface_Destroy(hGoldSurface);
	}
	pContext->aSrcContext[pContext->iSelectedVidSrc].hGoldSurface = NULL;
	
	if (hSurface_R)
	{
		BSUR_Surface_Destroy(hSurface_R);
	}
	pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface_R = NULL;
	
	if (hIntrSurface_R)
	{
		BSUR_Surface_Destroy(hIntrSurface_R);
	}
	pContext->aSrcContext[pContext->iSelectedVidSrc].hIntrSurface_R = NULL;
	
	if (hGoldSurface_R)
	{
		BSUR_Surface_Destroy(hGoldSurface_R);
	}				
	pContext->aSrcContext[pContext->iSelectedVidSrc].hGoldSurface_R = NULL;
	
	BVDC_Source_Destroy(hVidSrc);
	pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc = NULL;
	return (BERR_SUCCESS);
}

/***************************************************************************
 *
 */
static BERR_Code BTST_P_CloseVidSrc_HDDVI
    ( BTST_P_Context          *pContext )
{
	
#if (BTST_P_SUPPORT_HDR)
	BTST_P_CloseHdrPort(pContext);
#else
	BSTD_UNUSED(pContext);
#endif
	return (BERR_SUCCESS);
}


/***************************************************************************
 *
 */
void BTST_P_CloseVidSrc
    ( BTST_P_Context          *pContext )
{
    BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
    BMEM_HeapInfo stHeapStart, stHeapEnd;
	BERR_Code     err = BERR_SUCCESS;

    if(!hVidSrc)
    {
        goto done;
    }

    if (pContext->bSharedMem)
    {
        BMEM_GetHeapInfo(pContext->hSharedMem, &stHeapStart);
        printf("Before closing source %d: available shared memory = %d bytes\n",
            pContext->iSelectedVidSrc, stHeapStart.ulLargestFree);
    }

    if(BTST_P_IS_GFX_SRC(pContext->iSelectedVidSrc))
    {
		err = BTST_P_CloseVidSrc_GFX(pContext);
    }
	else if(BTST_P_IS_HDDVI_SRC(pContext->iSelectedVidSrc))
	{
		err = BTST_P_CloseVidSrc_HDDVI(pContext);
	}
	else if(BTST_P_IS_VDEC_SRC(pContext->iSelectedVidSrc))
	{
		printf("\n No VDEC input support %x", BCHP_CHIP);
	}
	else if(BTST_P_IS_MPEG_SRC(pContext->iSelectedVidSrc))
	{
		if (pContext->ulGfx2Mfd[pContext->iSelectedVidSrc])
		{
			err = BTST_P_CloseVidSrc_GfxToMfdVfd(pContext);
		}
		else
		{
			err = BTST_P_CloseVidSrc_MPEG(pContext);
		}
	}
	else if(BTST_P_IS_VFD_SRC(pContext->iSelectedVidSrc))
	{
		err = BTST_P_CloseVidSrc_GfxToMfdVfd(pContext);
	}
    else
    {
        printf("**** Todo: Add support for closing source %d\n", pContext->iSelectedVidSrc);
		err = BERR_INVALID_PARAMETER;
    }

	if(err != BERR_SUCCESS)
		goto done;


    /* [2] Destroy vdc source. */
    BVDC_Source_Destroy(hVidSrc);
    pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc = NULL;

    if(pContext->bApplyImmediately)
    {
        BTST_P_ApplyChanges(pContext);
    }

    if (pContext->bSharedMem)
    {
        BMEM_GetHeapInfo(pContext->hSharedMem, &stHeapEnd);
        printf("After closing source %d : available shared memory = %d bytes\n",
            pContext->iSelectedVidSrc, stHeapEnd.ulLargestFree);
    }

done:
    BTST_P_PrintStatus(pContext);
    return;
}

/***************************************************************************
 *
 */
void BTST_P_OpenRfm
    ( BTST_P_Context          *pContext )
{
#if (BTST_P_SUPPORT_RFM_OUTPUT)
    BRFM_Settings rfmDevSettings;

    if(pContext->hRfm)
    {
        goto done;
    }

    BRFM_GetDefaultSettings( &rfmDevSettings, pContext->hChp );
    rfmDevSettings.audioEncoding = BRFM_AudioEncoding_eStereo;

    BRFM_Open( &pContext->hRfm, pContext->hChp, pContext->hReg,
        pContext->hInt, &rfmDevSettings );

    BRFM_SetModulationType( pContext->hRfm, BRFM_ModulationType_eNtscOpenCable,
        BRFM_OutputChannel_eCh3 );
done:
#else
    BSTD_UNUSED(pContext);
#endif
    return;
}

/***************************************************************************
 *
 */
void BTST_P_CloseRfm
    ( BTST_P_Context          *pContext )
{
#if (BTST_P_SUPPORT_RFM_OUTPUT)
    if(!pContext->hRfm)
    {
        goto done;
    }

    BRFM_Close( pContext->hRfm );
    pContext->hRfm = NULL;

done:
#else
    BSTD_UNUSED(pContext);
#endif
    return;
}

/* Adjust control word so we achieve 160Hz
   Fv = W * 27e6 / (2^16 * (Period + 1))
   Let Period = 255 and solve for W ==> W=99 or 0x63 */
#define BTST_P_PWM_PERIOD              (0xff)
#define BTST_P_PWM_DUTY_CYCLE          (70)   /* 70% in percentage */
#define BTST_P_PWM_FREQ                (160)  /* 160 Hz */
#define BTST_P_PWM_CTRL_WORD \
    (((uint32_t)BTST_P_PWM_FREQ * (BTST_P_PWM_PERIOD+1) * 0x10000) / 27000000)
/***************************************************************************
 *
 */
void BTST_P_SetLCDDimCtrl
    ( BTST_P_Context          *pContext )
{
#if (BTST_P_SUPPORT_LVD)
    uint32_t ulDuty;

    printf("Please enter the PWM duty cycle in percentage to dim the LCD panel[20-100 weak to bright]:\n");
    BTST_P_Scanf(pContext, "%u", &ulDuty);
    BPWM_SetOnInterval(pContext->hPwmChn, ulDuty*BTST_P_PWM_PERIOD/100);
#else
    BSTD_UNUSED(pContext);
#endif
}


/***************************************************************************
 *
 */
void BTST_P_OpenHdmi
    ( BTST_P_Context          *pContext )
{
#if (BTST_P_SUPPORT_LVD)
    BPWM_ChannelSettings stPwmChannelsettings;
    if(pContext->hLvd)
    {
        goto Done;
    }

    BLVD_GetDefaultSettings(&pContext->stLvdSettings);
    pContext->stLvdSettings.bFifoMasterMode = true;
    /* Different for WXGA vs FHD */
    pContext->stLvdSettings.eOutputFormat =
        (pContext->apFmtInfo[pContext->iSelectedComp]->ulDigitalHeight < BFMT_1080I_HEIGHT)
        ? BLVD_OutputFormat_eSingleLvdsMode_Link1
        : BLVD_OutputFormat_eDualLvdsMode_Link2_Link1;
    BLVD_Open(&pContext->hLvd, pContext->hChp, pContext->hReg, pContext->hInt,
        &pContext->stLvdSettings);
    BLVD_PowerLvdsPort(pContext->hLvd, true) ;
    BPWM_Open(&pContext->hPwm, pContext->hChp, pContext->hReg, NULL);
    BPWM_GetChannelDefaultSettings(pContext->hPwm, 0, &stPwmChannelsettings);
    stPwmChannelsettings.openDrainb = false;
    stPwmChannelsettings.FreqMode   = Constant_Freq_Mode;
    BPWM_OpenChannel(pContext->hPwm, &pContext->hPwmChn, 0, &stPwmChannelsettings);
    BPWM_SetControlWord(pContext->hPwmChn, BTST_P_PWM_CTRL_WORD);
    BPWM_SetPeriodInterval(pContext->hPwmChn, BTST_P_PWM_PERIOD);
    BPWM_SetOnInterval(pContext->hPwmChn, BTST_P_PWM_DUTY_CYCLE*BTST_P_PWM_PERIOD/100);
    BPWM_Start(pContext->hPwmChn);
#endif

#if (BTST_P_SUPPORT_HDM)
    uint8_t ucRxAttached;
    BI2C_ChannelSettings stI2cChannelSettings;
    BERR_Code err = BERR_SUCCESS;
    if(pContext->hHdm)
    {
        goto Done;
    }

	/* I2C handle */
    TestError(BI2C_Open(&pContext->hI2c, pContext->hChp, pContext->hReg, pContext->hInt, NULL), "BI2C_Open");
    TestError(BI2C_GetChannelDefaultSettings(pContext->hI2c, BFramework_HDMI_OUTPUT_I2C_CHANNEL, &stI2cChannelSettings), "BI2C_GetChannelDefaultSettings");
    TestError(BI2C_OpenChannel(pContext->hI2c, &pContext->hI2cChn, BFramework_HDMI_OUTPUT_I2C_CHANNEL, &stI2cChannelSettings), "BI2C_OpenChannel");
    TestError(BI2C_CreateI2cRegHandle(pContext->hI2cChn, &pContext->hRegI2c), "BI2C_CreateI2cRegHandle");

	TestError(BHDM_GetDefaultSettings(&pContext->stHdmiSettings), "BHDM_GetDefaultSettings");
	pContext->stHdmiSettings.hTMR = pContext->hTmr;
#if (BTST_SUPPORT_DVO_HDDVI_LOOPBACK)
	pContext->stHdmiSettings.eOutputFormat = BHDM_OutputFormat_e12BitDVOMode;
	pContext->stHdmiSettings.eOutputPort = BHDM_OutputPort_eDVO;
#endif
    TestError(BHDM_Open(&pContext->hHdm, pContext->hChp, pContext->hReg, pContext->hInt, pContext->hRegI2c, &pContext->stHdmiSettings), "BHDM_Open");
    TestError(BHDM_RxDeviceAttached(pContext->hHdm, &ucRxAttached), "BHDM_RxDeviceAttached");

    printf("+++++++++++++++++++++++++++++++++++\n");
    printf("+ (%d) HDMI RX device is attached! +\n", ucRxAttached);
    printf("+++++++++++++++++++++++++++++++++++\n");
#endif

#if !BTST_VDC_ONLY
Done:

    return;
#else
	BSTD_UNUSED(pContext);
#endif
}

/***************************************************************************
 *
 */
void BTST_P_CloseHdmi
    ( BTST_P_Context          *pContext )
{
#if (BTST_P_SUPPORT_LVD)
    if(!pContext->hLvd)
    {
        goto done;
    }
    BLVD_Close(pContext->hLvd);
    BPWM_Stop(pContext->hPwmChn);
    BPWM_CloseChannel(pContext->hPwmChn);
    BPWM_Close(pContext->hPwm);
    pContext->hPwmChn = NULL;
    pContext->hPwm    = NULL;
    pContext->hLvd    = NULL;
#endif

#if (BTST_P_SUPPORT_HDM)
    if(!pContext->hHdm)
    {
        goto done;
    }

    BHDM_Close(pContext->hHdm);
    BI2C_CloseI2cRegHandle(pContext->hRegI2c);
    BI2C_CloseChannel(pContext->hI2cChn);
    BI2C_Close(pContext->hI2c);
    pContext->hHdm    = NULL;
    pContext->hRegI2c = NULL;
    pContext->hI2cChn = NULL;
    pContext->hI2c    = NULL;
#endif

#if !BTST_VDC_ONLY
done:

    return;
#else
	BSTD_UNUSED(pContext);
#endif
}


void BTST_P_DisplayVecSwap( BTST_P_Context  *pContext )
{
    char      ch = 'c';
    bool      bVecSwap = true;
#ifdef LINUX
    TIMESTAMP stStartTime, stEndTime;
#endif
    BVDC_Handle            hVdc   = pContext->hVdc;
    BVDC_Compositor_Handle hComp0 = pContext->ahCompositor[0];
    BVDC_Display_Handle    hDisp0 = pContext->ahDisplay[0];
    BVDC_Window_Handle     hHdWin = pContext->aWinContext[0][0].hWindow;
    BVDC_Compositor_Handle hComp1 = pContext->ahCompositor[1];
    BVDC_Display_Handle    hDisp1 = pContext->ahDisplay[1];
    BVDC_Window_Handle     hSdWin = pContext->aWinContext[1][0].hWindow;
    BVDC_Source_Handle     hVidSrc = pContext->aSrcContext[BAVC_SourceId_eMpeg0].hVidSrc;
#if (BTST_P_SUPPORT_HDM)
    BHDM_Handle            hHdmi = pContext->hHdm;
    BHDM_Settings          stHdmiSettings;
#endif

    if(!hVdc || !hDisp0 || !hComp0 || !hHdWin ||
#if (BTST_P_SUPPORT_HDM)
       !hHdmi ||
#endif
       !hDisp1 || !hComp1 || !hSdWin || !hVidSrc)
    {
        printf("Required setup: Single decode HDSD simul + HDMI\n");
        return;
    }

    pContext->iSelectedVidSrc = BAVC_SourceId_eMpeg0;

    while(ch != 'q')
    {
        printf("==========================================================\n");
        printf("Enter to switch configuration: ");
        ch = getchar();

        /* Swap vec */
        bVecSwap = !bVecSwap;
        if(bVecSwap)
        {
            printf("\nSwitch to Config1: \n");
            printf("\t Secondary vec: Comp + HDMI, format 480p \n");
            printf("\t Primary vec  : Cvbs + Svideo, format 480i \n");
        }
        else
        {
            printf("Switch to Config2: \n");
            printf("\t Primary vec  HDMI, format 480p\n");
            printf("\t Secondary vec: Comp + Cvbs + Svideo, format 480i\n");
        }

#ifdef LINUX
        TIMESTAMP_GET(&stStartTime);
#endif

        /* Close old window, comp and display */
        if(hHdWin)
        {
            BVDC_Window_Destroy(hHdWin);
            BTST_P_ApplyChanges(pContext);
        }
        if(hDisp0)
        {
            BVDC_Display_Destroy(hDisp0);
            BTST_P_ApplyChanges(pContext);
        }
        if(hComp0)
        {
            BVDC_Compositor_Destroy(hComp0);
            BTST_P_ApplyChanges(pContext);
        }
        if(hSdWin)
        {
            BVDC_Window_Destroy(hSdWin);
            BTST_P_ApplyChanges(pContext);
        }
        if(hDisp1)
        {
            BVDC_Display_Destroy(hDisp1);
            BTST_P_ApplyChanges(pContext);
        }
        if(hComp1)
        {
            BVDC_Compositor_Destroy(hComp1);
            BTST_P_ApplyChanges(pContext);
        }

        /* Switch between 2 configurations:
         * Config 1:
         *      bVecSwap is true
         *      Secondary VEC: Comp + HDMI, format 1080i
         *      Primary VEC  : Cvbs + Svideo, format 480i
         * Config 2:
         *      bVecSwap is false
         *      Secondary VEC: Comp + Cvbs + Svideo, format 480i
         *      Primary VEC  : HDMI, format 480p
         */
        /* bring up c0 */
        BVDC_Compositor_Create(hVdc, &hComp0, BVDC_CompositorId_eCompositor0, NULL);
        if(bVecSwap)
            BVDC_Display_Create(hComp0, &hDisp0, BVDC_DisplayId_eDisplay1, NULL);
        else
            BVDC_Display_Create(hComp0, &hDisp0, BVDC_DisplayId_eDisplay0, NULL);
        BVDC_Compositor_SetBackgroundColor(hComp0, 0xff, 0x00, 0x00);
        BVDC_Display_SetVideoFormat(hDisp0, BFMT_VideoFmt_e480p);
        BTST_P_ApplyChanges(pContext);

        /* bring up c1 */
        BVDC_Compositor_Create(hVdc, &hComp1, BVDC_CompositorId_eCompositor1, NULL);
        if(bVecSwap)
            BVDC_Display_Create(hComp1, &hDisp1, BVDC_DisplayId_eDisplay0, NULL);
        else
            BVDC_Display_Create(hComp1, &hDisp1, BVDC_DisplayId_eDisplay1, NULL);
        BVDC_Compositor_SetBackgroundColor(hComp1, 0x00, 0x00, 0xff);
        BTST_P_ApplyChanges(pContext);

        /* bring up DAC */
        if(bVecSwap)
        {
            /* C0 */
            BVDC_Display_SetDacConfiguration(hDisp0,
                BFramework_Dac_Composite_0 | BFramework_Dac_Svideo_Chroma |
                BFramework_Dac_Svideo_Luma, BVDC_DacOutput_eUnused);

            BVDC_Display_SetDacConfiguration(hDisp0,
                BFramework_Dac_Component_pr, BVDC_DacOutput_ePr);
            BVDC_Display_SetDacConfiguration(hDisp0,
                BFramework_Dac_Component_y, BVDC_DacOutput_eY);
            BVDC_Display_SetDacConfiguration(hDisp0,
                BFramework_Dac_Component_pb, BVDC_DacOutput_ePb);

            /* c1 */
            BVDC_Display_SetDacConfiguration(hDisp1,
                BFramework_Dac_Component_y | BFramework_Dac_Component_pr |
                BFramework_Dac_Component_pb, BVDC_DacOutput_eUnused);

            BVDC_Display_SetDacConfiguration(hDisp1,
                BFramework_Dac_Composite_0, BVDC_DacOutput_eComposite);
            BVDC_Display_SetDacConfiguration(hDisp1,
                BFramework_Dac_Svideo_Chroma, BVDC_DacOutput_eSVideo_Chroma);
            BVDC_Display_SetDacConfiguration(hDisp1,
                BFramework_Dac_Svideo_Luma, BVDC_DacOutput_eSVideo_Luma);
        }
        else
        {
            /* c0 */
            BVDC_Display_SetDacConfiguration(hDisp0,
                BFramework_Dac_Composite_0 | BFramework_Dac_Svideo_Chroma |
                BFramework_Dac_Svideo_Luma | BFramework_Dac_Component_y |
                BFramework_Dac_Component_pr | BFramework_Dac_Component_pb,
                BVDC_DacOutput_eUnused);

            /* c1 */
            BVDC_Display_SetDacConfiguration(hDisp1,
                BFramework_Dac_Component_pr, BVDC_DacOutput_ePr);
            BVDC_Display_SetDacConfiguration(hDisp1,
                BFramework_Dac_Component_y, BVDC_DacOutput_eY);
            BVDC_Display_SetDacConfiguration(hDisp1,
                BFramework_Dac_Component_pb, BVDC_DacOutput_ePb);
            BVDC_Display_SetDacConfiguration(hDisp1,
                BFramework_Dac_Composite_0, BVDC_DacOutput_eComposite);
            BVDC_Display_SetDacConfiguration(hDisp1,
                BFramework_Dac_Svideo_Chroma, BVDC_DacOutput_eSVideo_Chroma);
            BVDC_Display_SetDacConfiguration(hDisp1,
                BFramework_Dac_Svideo_Luma, BVDC_DacOutput_eSVideo_Luma);

        }
        BTST_P_ApplyChanges(pContext);

        /* HDMI */
#if (BTST_P_SUPPORT_HDM)
        BVDC_Display_GetHdmiConfiguration(hDisp0, BVDC_Hdmi_0,
            &stHdmiSettings.eColorimetry);
        stHdmiSettings.eColorimetry =
            (stHdmiSettings.eColorimetry != BAVC_MatrixCoefficients_eUnknown)
            ? BAVC_MatrixCoefficients_eUnknown : BAVC_MatrixCoefficients_eItu_R_BT_709;
#if (BTST_SUPPORT_DVO_HDDVI_LOOPBACK)
        stHdmiSettings.eOutputPort = BHDM_OutputPort_eDVO;
#else
        stHdmiSettings.eOutputPort = BHDM_OutputPort_eHDMI;
#endif
        if(pContext->stHdmiSettings.eColorimetry != BAVC_MatrixCoefficients_eUnknown)
        {
            BVDC_Display_GetVideoFormat(hDisp0, &stHdmiSettings.eInputVideoFmt);
        }
        BVDC_Display_SetHdmiConfiguration(hDisp0, BVDC_Hdmi_0, stHdmiSettings.eColorimetry);
        BHDM_EnableDisplay(hHdmi, &stHdmiSettings);
        BTST_P_ApplyChanges(pContext);
#endif

        /* w0 */
        BVDC_Window_Create(hComp0, &hHdWin, BVDC_WindowId_eVideo0, hVidSrc, NULL);
        BVDC_Window_SetVisibility(hHdWin, true);
        BVDC_Window_Create(hComp1, &hSdWin, BVDC_WindowId_eVideo0, hVidSrc, NULL);
        BVDC_Window_SetVisibility(hSdWin, true);
        BTST_P_ApplyChanges(pContext);

#ifdef LINUX
        TIMESTAMP_GET(&stEndTime);
        printf("\nstartTime: %ld(sec) %ld(usec)\n",
            stStartTime.tv_sec, stStartTime.tv_usec);
        printf("endTime  : %ld(sec) %ld(usec)\n",
            stEndTime.tv_sec, stEndTime.tv_usec);
        printf("Turn-around time for vec swap is %ld milliseconds \n",
            TIMESTAMP_USED_IN_MSEC(stStartTime, stEndTime));
#endif
    }

}

/* end of file */
