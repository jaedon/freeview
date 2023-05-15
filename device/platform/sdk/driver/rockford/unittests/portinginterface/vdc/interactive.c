/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: interactive.c $
* $brcm_Revision: Hydra_Software_Devel/198 $
* $brcm_Date: 10/17/12 4:13p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/portinginterface/vdc/interactive.c $
* 
* Hydra_Software_Devel/198   10/17/12 4:13p vanessah
* SW7445-60: add MRC monitor flag to speedup emulation
* 
* Hydra_Software_Devel/197   10/17/12 3:40p vanessah
* SW7445-60: add 7445 loadrts function
* 
* Hydra_Software_Devel/196   10/11/12 4:26p vanessah
* SW7425-2501: remove vdec input support
* 
* Hydra_Software_Devel/195   10/11/12 12:08p syang
* SW7346-572: add long-time-lock for vdc_test
* 
* Hydra_Software_Devel/194   10/9/12 3:59p syang
* SW7346-572: rm TV related (vdb, dynamic rts, ...); rm symbolic link to
* files in applications/bmetest/grc_test
* 
* Hydra_Software_Devel/193   10/9/12 3:41p vanessah
* SW7425-4046: add custom fmt stress test from 1080p to 30x30 with step
* size 30
* 
* Hydra_Software_Devel/192   10/8/12 3:29p syang
* SW7346-572: use BTST_P_ApplyChanges to get uniform error handling;
* avoid holding hVdcCfgLock too long
* 
* Hydra_Software_Devel/191   9/28/12 3:37p vanessah
* SW7425-2501: fix 7335 execution error
* 
* Hydra_Software_Devel/190   9/27/12 11:13a vanessah
* SW7425-2501: fix 7425/7420/7435 error
* 
* Hydra_Software_Devel/189   9/26/12 4:50p vanessah
* SW7425-2501: fix memory client index error
* 
* Hydra_Software_Devel/188   9/26/12 11:47a vanessah
* SW7425-2501: add buffer configuration default due to nightly build test
* failure
* 
* Hydra_Software_Devel/187   9/25/12 5:36p vanessah
* SW7425-2501: vdc_test clean up
* 
* Hydra_Software_Devel/186   9/5/12 6:22p vanessah
* SW7425-2501: vdc_test clean up
* 
* Hydra_Software_Devel/185   8/16/12 3:02p vanessah
* SW7425-2571: revise mask function and remove old mbvn
* 
* Hydra_Software_Devel/184   7/31/12 11:39a vanessah
* SW7360-12: add stoprul toggle  to disable rul output
* 
* Hydra_Software_Devel/183   7/27/12 3:48p vanessah
* SW7425-3150: add video processing skip support
* 
* Hydra_Software_Devel/182   7/26/12 5:29p vanessah
* SW7360-3: change vdc mem/heap setting for 7360
* 
* Hydra_Software_Devel/181   7/11/12 1:46p vanessah
* SW7425-3460: rate tracking test
* 
* Hydra_Software_Devel/180   7/9/12 5:18p vanessah
* SW7425-2501: removal 3548/3556 support
* 
* Hydra_Software_Devel/179   7/5/12 11:33a vanessah
* SW7425-3432: add custom format stress test on transcode path
* 
* Hydra_Software_Devel/178   5/18/12 3:46p vanessah
* SW7425-3069: add resource leak change test
* 
* Hydra_Software_Devel/177   5/3/12 12:18p vanessah
* SW7425-2990: load different RTS for 7425 without reboot system
* 
* Hydra_Software_Devel/176   4/26/12 11:38a tdo
* SW7425-2571: Add VDC API to query BVN module error
*
* Hydra_Software_Devel/175   4/10/12 3:45p tdo
* SW7435-97: Format change on the SD path with progressive stream cause
* HD path to flicker
*
* Hydra_Software_Devel/174   3/29/12 4:29p pntruong
* SW7405-4296, SW7425-2526: Added some initial capabilities query at the
* vdc top level.
*
* Hydra_Software_Devel/173   3/7/12 4:42p vanessah
* SW7435-9: correct typo
*
* Hydra_Software_Devel/172   3/7/12 3:46p nilesh
* SW7435-9: Fixed 7425 MEMC assignment
*
* Hydra_Software_Devel/171   3/5/12 9:44p vanessah
* SW7435-9: fix ViCE memory allocation problem
*
* Hydra_Software_Devel/170   3/5/12 9:26a yuxiaz
* SW7425-2526: Added tests for runtime capabilities query.
*
* Hydra_Software_Devel/169   2/29/12 7:00p tdo
* SW7435-9: Bring up VDC for 7435A0
*
* Hydra_Software_Devel/168   2/29/12 4:36p tdo
* SW7435-9: add support for 7435A0 in VDC
*
* Hydra_Software_Devel/167   2/29/12 12:46p tdo
* SW7425-2330: Add videolist cmd
*
* Hydra_Software_Devel/166   2/17/12 11:28a vanessah
* SW7425-2327: add Bar data handling test
*
* Hydra_Software_Devel/165   2/15/12 10:00a nilesh
* SW7425-2217: Merged Pid2Buffer Support
*
* Hydra_Software_Devel/SW7425-2217/1   1/26/12 5:29p nilesh
* SW7425-2217: Added PID2Buffer support
*
* Hydra_Software_Devel/164   2/13/12 11:38a vanessah
* SW7425-2219: add stop decode cmd upon jira testing
*
* Hydra_Software_Devel/163   1/20/12 6:41p vanessah
* SW7435-9: add STG2/3 support
*
* Hydra_Software_Devel/162   1/18/12 10:31a yuxiaz
* sw7358-218: Update buffer count for 7358 and 7552.
*
* Hydra_Software_Devel/161   1/13/12 4:33p vanessah
* SW7425-1115: add unittest for MADR/MCVP seamless transactions
*
* Hydra_Software_Devel/160   1/11/12 3:08p darnstein
* SW7125-1124: merge DCS Hybrid+ to main branch. DCS almost ready for
* production.
*
* Hydra_Software_Devel/SW7125-1124/1   1/10/12 5:02p darnstein
* SW7125-1124: Fix SOME of the bugs in RUL capture. Add a message "Begin
* RUL capture" to same.
*
* Hydra_Software_Devel/159   12/14/11 10:58a yuxiaz
* SW7425-1875: Added test for 3d source buffer selection.
*
* Hydra_Software_Devel/158   12/14/11 10:47a nilesh
* SW7425-1110: Added CC Dump option toggle
*
* Hydra_Software_Devel/157   11/29/11 11:42a vanessah
* SW7425-1783: add unittest for stg picture id call back
*
* Hydra_Software_Devel/156   10/21/11 5:45p syang
* SW7425-1429: add test for Vsync callback and GetBuf_isr/ReturnBuf_isr
*
* Hydra_Software_Devel/155   9/30/11 5:15p darnstein
* SW7400-2665: merge from JIRA branch.
*
* Hydra_Software_Devel/SW7400-2665/2   9/30/11 5:09p darnstein
* SW7400-2665: merge from main branch.
*
* Hydra_Software_Devel/SW7400-2665/1   9/28/11 7:28p darnstein
* SW7400-2665: Before printing out VEC IT microcode identity, wait a
* while. This is necessary because there is a variable delay until BVDC
* loads new microcode into registers.
*
*
* Hydra_Software_Devel/153   9/29/11 11:28a nilesh
* SW7425-891: Moved VCE commands to vce_encoder_cmd.inc
*
* Hydra_Software_Devel/152   9/16/11 1:21p yuxiaz
* SW7425-1283: Added 3d video stress test.
*
* Hydra_Software_Devel/151   8/25/11 10:46a tdo
* SW7420-2030: Add apply changes error tracking capability
*
* Hydra_Software_Devel/150   8/16/11 1:58p nilesh
* SW7425-448: Added VCE Standby/Resume Commands
*
* Hydra_Software_Devel/149   8/15/11 4:47p tdo
* SW7420-2017: Add test to scan all size for scaler enable error
*
* Hydra_Software_Devel/148   7/25/11 5:56p vanessah
* SW7425-465: add cmd to adjust minimum display and src fmt
*
* Hydra_Software_Devel/147   7/6/11 6:42p albertl
* SW7408-291, SW7425-745:: Added tests for vf filter settings and
* selective output mute.
*
* Hydra_Software_Devel/146   5/4/11 11:28a tdo
* SW7422-417: Add cmd to toggle 3d cap for decode channel open
*
* Hydra_Software_Devel/145   4/27/11 4:14p tdo
* SW7231-133, SW7422-417: Add support for multiple channel decode within
* a decoder with streamer input and playback  Add option for users to
* manually force SVD to only decode AVC, this is needed to do multi
* channel decode with SVD due to memory allocation issue.  Cleanup old
* code.
*
* Hydra_Software_Devel/144   4/25/11 11:47a tdo
* SW7425-315: Add rockford/appframework support for upper memory
* range(>256MB) of MEMC0
*
* Hydra_Software_Devel/143   4/8/11 1:48p vanessah
* SW7425-32: add a new command to sleep multiple seconds
*
* Hydra_Software_Devel/142   4/1/11 12:27p vanessah
* SW7422-118: change gfx double buffer and get rid of 7425 CFE v3.1
* support
*
* Hydra_Software_Devel/141   3/30/11 10:25a yuxiaz
* SW7400-2882: Added test for BVDC_Window_EnableBufLog
*
* Hydra_Software_Devel/140   3/28/11 9:58a yuxiaz
* SW7420-1741: Added game mode delay support.
*
* Hydra_Software_Devel/139   3/9/11 7:03p vanessah
* SW7425-32: compatible with CFE3.1 and 3.2. Follow BTST_CFE31
*
* Hydra_Software_Devel/138   3/4/11 11:51a vanessah
* SW7425-32: change memory and heap due to new CFE
*
* Hydra_Software_Devel/137   2/10/11 4:06p hongtaoz
* SW7425-1: added VCE channel open/close test;
*
* Hydra_Software_Devel/136   2/9/11 4:26p yuxiaz
* SW7400-2882: Added multi-buffering log to vdc_test.
*
* Hydra_Software_Devel/135   1/26/11 3:17p yuxiaz
* SW7400-2998, SW7425-32: Only reduce cdb/itb buffer size for mosaic
* mode.
*
* Hydra_Software_Devel/134   1/13/11 5:29p albertl
* SW7422-168: Changed to use BMRC_GetMaxCheckers for monitor settings.
*
* Hydra_Software_Devel/133   1/12/11 3:10p albertl
* SW7422-168: Updated to match interface.
*
* Hydra_Software_Devel/132   12/21/10 2:56p yuxiaz
* SW7422-34: Put back BTST_P_Source3D
*
* Hydra_Software_Devel/131   12/21/10 11:12a tdo
* SW7422-28: Adjust to 3D format size change.  Add test for single MEMC
* scheme.
*
* Hydra_Software_Devel/129   12/20/10 3:51p nilesh
* SW7425-1: Added VCE ITB/CDB buffer dump command "ice"
*
* Hydra_Software_Devel/128   12/13/10 1:25p hongtaoz
* SW7425-32: updated VDC default heap allocation for 7422/25; added
* open/close decoder commands for emulation test;
*
* Hydra_Software_Devel/127   12/13/10 12:04p tdo
* SW7422-13: Update 7422 buffer allocation to match new spreadsheet
*
* Hydra_Software_Devel/126   12/10/10 2:10p hongtaoz
* SW7425-32: updated HD PIP window heap allocation for 7425;
*
* Hydra_Software_Devel/125   12/10/10 2:09p hongtaoz
* SW7425-32: updated heap allocation for 7425;
*
* Hydra_Software_Devel/124   12/7/10 2:49p tdo
* SW7422-13: Add new command to set display 3D orientation
*
* Hydra_Software_Devel/123   12/3/10 3:24p tdo
* SW7422-13: Add new command to set 3D orientation to XVD
*
* Hydra_Software_Devel/122   12/3/10 12:26a hongtaoz
* SW7425-32: added DM selection for multi-decode test;
*
* Hydra_Software_Devel/121   11/24/10 12:55p pntruong
* SW7552-17: Added initial 7552 support.
*
* Hydra_Software_Devel/120   11/16/10 5:02p pntruong
* SW7231-16: Initial support for 7231.
*
* Hydra_Software_Devel/119   11/11/10 6:44p pntruong
* SW7425-31: Refactored hdm/lvds/hdr/dvo and added support hdmi input on
* stb chips.  Removed dead codes.
*
* Hydra_Software_Devel/118   11/11/10 2:52p tdo
* SW7422-13: Setup proper heap for AVD
*
* Hydra_Software_Devel/117   11/5/10 11:36p hongtaoz
* SW7425-32: enable xpt/xvd for emulation;
*
* Hydra_Software_Devel/116   11/3/10 2:39p nilesh
* SW7425-38: Added TS Muxlib syslib support
*
* Hydra_Software_Devel/115   11/2/10 12:24p nilesh
* SW7425-1: Added encoder debug log command "le"
*
* Hydra_Software_Devel/114   11/1/10 5:23p tdo
* SW7422-54: VDC: Update vdc_test with MEMC/window partitioning
*
* Hydra_Software_Devel/113   10/27/10 11:17a pntruong
* SW7358-4: Removed unnessary timer allocation.
*
* Hydra_Software_Devel/113   10/27/10 9:57a pntruong
* SW7358-4: Removed unnessary timer allocation.
*
* Hydra_Software_Devel/113   10/27/10 9:56a pntruong
* SW7358-4: Removed unnessary timer allocation.
*
* Hydra_Software_Devel/112   10/21/10 4:59p nilesh
* SW7425-1: Implemented GetStatus. Added GetEncodedFrames.
*
* Hydra_Software_Devel/111   10/21/10 11:19a nilesh
* SW7425-1: Added VCE Encoder Support (enable by compiling with
* SUPPORT_VCE=y)
*
* Hydra_Software_Devel/110   10/20/10 12:36p jessem
* SW7420-173: Added regression tests for VF as source feature.
*
* Hydra_Software_Devel/109   10/19/10 11:33a yuxiaz
* SW7422-39: Added test for BAVC_SourceId_eMpeg2 support in VDC.
*
* Hydra_Software_Devel/108   10/19/10 10:51a pntruong
* SW7425-16, SW3548-1650: Adapted to the removal of
* BXVD_ChannelSettings.bFGTEnable.  Also get rid of all reference to fgt
* in vdctest app.
*
* Hydra_Software_Devel/107   10/18/10 6:30p tdo
* SW7420-1157: Revert back memory allocation so that EC tests can run
*
* Hydra_Software_Devel/106   10/15/10 3:25p hongtaoz
* SW7425-1, SW7425-10: added MEMC1 support to emulation framwork;
*
* Hydra_Software_Devel/105   10/15/10 1:04p tdo
* SW7420-1157: Match the VDC memory usage in the reference software to
* memory worksheet
*
* Hydra_Software_Devel/104   10/12/10 10:54a vanessah
* SW7550-598:  Rework vdctest to specify display ouput
*
* Hydra_Software_Devel/103   10/11/10 1:11p jessem
* SW7420-173: Added support for VFD as source feature.
*
* Hydra_Software_Devel/102   10/4/10 11:33p hongtaoz
* SW7425-33, SW7425-29: added cmp3 test; added STG setting test; STG
* master should not set DACs;
*
* Hydra_Software_Devel/101   9/27/10 4:07p hongtaoz
* SW7425-32: added VDC_TEST_ONLY compile option;
*
* Hydra_Software_Devel/100   9/2/10 1:50p vanessah
* SW7422-52:  Two Implementations (MAD + MCVP) on the same chip
*
* Hydra_Software_Devel/99   6/25/10 1:43p darnstein
* SW7468-272: remove do-nothing "VBI passthrough" command.
*
* Hydra_Software_Devel/98   6/17/10 5:59p hongtaoz
* SW3548-2912: added support for pseudo-1080i input for 3DTV; allow API
* to disable SCL vertical phase shift; added 540p60/50 custom formats to
* test pseudo-1080i 3DTV;
*
* Hydra_Software_Devel/97   5/11/10 2:12p yuxiaz
* SW7405-4230: Added test to do vec swap at display create.
*
* Hydra_Software_Devel/96   2/19/10 11:09a pntruong
* SW3548-2539: Add "regression" command to the vdc_test.
*
* Hydra_Software_Devel/95   2/2/10 11:13a pntruong
* SW3548-2678, SW3548-2680, SW3548-2681: Fixed build errors for chipset
* that does not have hdr.
*
* Hydra_Software_Devel/94   2/1/10 6:57p hongtaoz
* SW3548-2678, SW3548-2680, SW3548-2681: added HDMI1.4 3D formats support
* for 3548; added multi-buffer low delay mode support for 1to2 and 2to5
* rate gap cases; reduced capture buffer allocation for interlaced
* pulldown case;
*
* Hydra_Software_Devel/BCM3D/1   12/17/09 6:30p hongtaoz
* SW3548-2680, SW3548-2681: added new special display format 2160i48 to
* support HDMi1.4 3D format for 11-bit BVN; added BHDR frontend config
* to support 3D video testing;
*
* Hydra_Software_Devel/93   1/28/10 2:41p syang
* SW7405-3829: add option to shrink width to fit mad limit
*
* Hydra_Software_Devel/92   1/15/10 5:13p yuxiaz
* SW7405-3290: Added test to detect if MPEG input.
*
* Hydra_Software_Devel/91   1/7/10 6:13p yuxiaz
* SW7405-3184: Added channel change, format change resize stress test.
*
* Hydra_Software_Devel/90   1/7/10 11:27a yuxiaz
* SW7405-3184: Added auto format stress test.
*
* Hydra_Software_Devel/89   12/22/09 5:17p tdo
* SW7401-3828: Add AFD support
*
* Hydra_Software_Devel/88   12/16/09 3:28p tdo
* SW7342-126: Add command to turn on/off DACs
*
* Hydra_Software_Devel/87   12/4/09 3:57p tdo
* SW7408-23: Add playback feature for digital source
*
* Hydra_Software_Devel/86   12/3/09 3:13p yuxiaz
* SW7405-3120: Added test for overlapping mosaic videos with z-ordering
*
* Hydra_Software_Devel/85   12/2/09 10:14a yuxiaz
* SW7420-462: Added tests for new tuning functions.
*
* Hydra_Software_Devel/84   10/14/09 4:49p tdo
* SW7420-376: Bringup appframework for 7410
*
* Hydra_Software_Devel/83   10/14/09 3:56p yuxiaz
* SW7405-3186: Added source clip stress test.
*
* Hydra_Software_Devel/82   10/13/09 1:17p yuxiaz
* SW7405-3184: Added resize stress test for mosaic mode.
*
* Hydra_Software_Devel/81   9/14/09 4:24p hongtaoz
* SW3548-2471: added test case for MAD allocation option in window
* setting;
*
* Hydra_Software_Devel/80   8/28/09 4:42p yuxiaz
* SW3556-672: Added test to get MAD dynamic default settings.
*
* Hydra_Software_Devel/79   7/31/09 6:48p syang
* PR 55812:  add 7550 support
*
* Hydra_Software_Devel/78   6/8/09 3:37p yuxiaz
* PR54992: Added automated lipsync stress test.
*
* Hydra_Software_Devel/77   6/5/09 2:04p yuxiaz
* PR55330, PR55323, PR53197, PR55158, PR55604: Increase 2HD buffer count
* for ANR DCX.
*
* Hydra_Software_Devel/76   5/15/09 10:31a rpan
* PR55188: Added support for capturing and repeating comamnds.
*
* Hydra_Software_Devel/75   4/29/09 11:12a jessem
* PR 54380: Constrained tests that uses tuner in non-EMULATION mode only.
*
* Hydra_Software_Devel/74   4/7/09 10:07a tdo
* PR53373: Add HDMI deep color support and update buffer heap allocation
* to match 7420 spreadsheet
*
* Hydra_Software_Devel/73   3/31/09 6:49p hongtaoz
* PR53037, PR53038, PR53687, PR53688: added random isr drop test to
* vdc_test;
*
* Hydra_Software_Devel/72   3/30/09 4:15p rpan
* PR53677: Added command for NTSC->PAL picture drop mode selection.
*
* Hydra_Software_Devel/71   3/30/09 3:22p hongtaoz
* PR53037, PR53038: added forced synclock mode and double-buffer support;
* added more VEC locking rates support;
*
* Hydra_Software_Devel/70   3/25/09 5:54p pntruong
* PR52524: Used the index determined by upstream block.
*
* Hydra_Software_Devel/69   3/20/09 4:37p pntruong
* PR52524: Pass RgbQuantization info from AviInfoFrame to hddvi for
* proper colorimetry loading.
*
* Hydra_Software_Devel/68   3/19/09 4:05p jessem
* PR 53388: Added more VDB tests.
*
* Hydra_Software_Devel/67   3/16/09 11:01p tdo
* PR45785, PR45789: Merge from MCVP branch
*
* Hydra_Software_Devel/7420_mcvp/3   3/13/09 11:16a tdo
* PR45785, PR45789: merge from main branch on 3/13/09
*
* Hydra_Software_Devel/66   3/11/09 9:48p pntruong
* PR51930, PR52072, PR52364: Added "spread spectrum" - ss toggle command.
*
* Hydra_Software_Devel/7420_mcvp/2   3/10/09 9:09a tdo
* PR45785, PR45789: merge from main branch on 3/10/09
*
* Hydra_Software_Devel/65   3/9/09 4:41p rpan
* PR52203: Implemented some stress tests and debugging facility targeting
* VNET reconfiguration and multi-buffering.
*
* Hydra_Software_Devel/7420_mcvp/1   3/6/09 1:34p syang
* PR 45796: add gfd scl (coeff) mode test
*
* Hydra_Software_Devel/64   2/4/09 1:41p tdo
* PR51627: add vdc_test support for 7336
*
* Hydra_Software_Devel/63   1/21/09 5:05p darnstein
* PR46356: Use only one DAC for 93549 and 93556 reference boards.
*
* Hydra_Software_Devel/62   12/23/08 9:16p tdo
* PR50391: Bringup MEMC0 and MEMC1 in vdc_test
*
* Hydra_Software_Devel/61   12/21/08 2:07p tdo
* PR50391: Handle correct number of XVD for 7420
*
* Hydra_Software_Devel/60   12/17/08 11:30p tdo
* PR50391: Remove debug code
*
* Hydra_Software_Devel/59   12/17/08 7:45p pntruong
* PR50002: Added force source pending for api test.
*
* Hydra_Software_Devel/58   12/17/08 12:11p yuxiaz
* PR50391: Fixed seg fault on 7405.
*
* Hydra_Software_Devel/57   12/15/08 10:02p tdo
* PR50391: Basic VDC bringup
*
* Hydra_Software_Devel/56   12/3/08 5:08p jessem
* PR 48992: Added Chroma Histogram tests.
*
* Hydra_Software_Devel/55   11/13/08 5:39p darnstein
* PR48884: microcode dump command "udmp" is ready for evaluation.
*
* Hydra_Software_Devel/54   11/7/08 6:47p darnstein
* PR46484: Command ddmp dumps tables of register values from BVDC display
* software.
*
* Hydra_Software_Devel/53   10/24/08 5:44p albertl
* PR48199, PR48046: Removed BTST_P_EnableRdcCache because BMEM_SetCache
* is obsolete.
*
* Hydra_Software_Devel/52   10/24/08 3:44p albertl
* PR48023: Added BTST_P_DvoAttenuationRGB.  Removed bit shifting tweaks
* that are no longer necessary.  Help menu no longer invoked by wrong
* commands.
*
* Hydra_Software_Devel/51   10/22/08 10:57p nissen
* PR 46310: Added GRC and P3D tests.
*
* Hydra_Software_Devel/50   10/14/08 3:44p yuxiaz
* PR46305, PR46307, PR46309: Added unit test.
*
* Hydra_Software_Devel/49   8/20/08 4:13p tdo
* PR45641: Update default memory allocation for 3548 and 3556
*
* Hydra_Software_Devel/48   8/5/08 8:12p pntruong
* PR39163: Added xvycc toggle.
*
* Hydra_Software_Devel/47   8/5/08 7:57p pntruong
* PR44362: Added HDR callback to get correct colorspace from avi
* information frame packet.
*
* Hydra_Software_Devel/46   7/28/08 9:48a rpan
* PR36501: Save the work for parsing PSI messages.
*
* Hydra_Software_Devel/45   7/23/08 7:02p tdo
* PR43508, PR43509: Mapping individual bandgap adjustment for each DAC
*
* Hydra_Software_Devel/44   7/18/08 3:22p rpan
* PR36501: Hook up tuner and demodulator with xport.
*
* Hydra_Software_Devel/43   7/14/08 1:35p rpan
* PR36501: Check-in point for tuner work. Tuner input is not working yet.
*
* Hydra_Software_Devel/42   6/19/08 12:25p rpan
* PR43712: Add SCART test cases.
*
* Hydra_Software_Devel/41   6/18/08 1:26p jessem
* PR 39237, PR 43759: Added BVDB (Video Usage Modes Database) tests.
*
* Hydra_Software_Devel/40   6/18/08 12:10p syang
* PR35549:  add mutex to serialize access to vdc api
*
* Hydra_Software_Devel/39   6/17/08 11:01a rpan
* PR43614: Added bandwidth equation parameter tests.
*
* Hydra_Software_Devel/38   6/7/08 5:41p jessem
* PR 39152, PR 39237, PR 35549: Revised enabling/disabling of source
* pending and handling of source pending event.
*
* Hydra_Software_Devel/37   6/7/08 3:46p jessem
* PR 39152: Added dynamic RTS test.
*
* Hydra_Software_Devel/36   6/7/08 3:11p rpan
* PR39337: Added compositor OSD setting test cases.
*
* Hydra_Software_Devel/35   6/5/08 1:05p rpan
* PR39807: Correct 3556/3548 HDMI DVO output format.
*
* Hydra_Software_Devel/34   6/4/08 11:44a yuxiaz
* PR39807: 3548 bring up.
*
* Hydra_Software_Devel/33   5/27/08 7:22p hongtaoz
* PR42248, PR42494: added display alignment test;
*
* Hydra_Software_Devel/32   5/19/08 2:26p pntruong
* PR34711: Corrected enumeration indexing, and added sanity check for
* table.  Matched up create/destroy.
*
* Hydra_Software_Devel/31   4/30/08 5:30p pntruong
* PR35549: Added api test for source pending.
*
* Hydra_Software_Devel/30   3/31/08 3:42p jessem
* PR 39335: Added CM3D tests.
*
* Hydra_Software_Devel/29   3/20/08 1:40p yuxiaz
* PR39807: Static buffer bring up on 3548.
*
* Hydra_Software_Devel/28   3/10/08 8:32p darnstein
* PR25971: first try at DCS for 7400
*
* Hydra_Software_Devel/27   3/10/08 4:07p yuxiaz
* PR39807: Static buffer bring up on 3548.
*
* Hydra_Software_Devel/26   2/28/08 6:15p jessem
* PR 38623: Removed rdc_block_out.h
*
* Hydra_Software_Devel/25   2/28/08 5:05p jessem
* PR 38623: Added RDMA blockout support.
*
* Hydra_Software_Devel/24   2/11/08 9:22p pntruong
* PR39177: [VEC] Need new display api to specify framedrop at runtime.
*
* Hydra_Software_Devel/23   1/24/08 5:27p tdo
* PR38710: Add blue stretch support
*
* Hydra_Software_Devel/22   12/14/07 11:16a tdo
* PR36898: Bringup 7335
*
* Hydra_Software_Devel/21   11/28/07 2:02p tdo
* PR37413: Provide new Api to set offset and size of histogram window and
* read corresponding parameters from specific window
*
* Hydra_Software_Devel/20   11/10/07 2:50p yuxiaz
* PR34540: 7325 bringup.
*
* Hydra_Software_Devel/19   10/30/07 7:30p pntruong
* PR34239: Allow dynamically loading of vec custom timing.
*
* Hydra_Software_Devel/18   10/29/07 4:34p rpan
* PR34857: Fixed a typo when checking for 2nd memory controller.
*
* Hydra_Software_Devel/17   10/25/07 3:31p yuxiaz
* PR29569, PR36290: Add FGT support on 7405.
*
* Hydra_Software_Devel/16   10/18/07 12:08a pntruong
* PR36137: [PC]:Set pc input video format without disable auto format
* detection.
*
* Hydra_Software_Devel/15   10/17/07 1:41p rpan
* PR34857: Fixed the 97440 2nd XVD heap assignment.
*
* Hydra_Software_Devel/14   10/16/07 2:45p rpan
* PR34857: First version after merging the latest xvd_vdc_test and
* mvd_vdc_test as of Oct. 16, 2007, 12pm.
*
*
* Hydra_Software_Devel/1   9/17/07 4:39p rpan
* PR34857: Initial revision. Ported from xvd_vdc_test.
*
*
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "framework.h"
#include "framework_board.h"

#include "config.h"
#include "test_util.h"
#include "commands.h"
#include "interactive.h"

BDBG_MODULE(BTST);


#ifdef VXWORKS
extern void b_vxworks_Isr( BINT_Handle intHandle, int intId );
#define BINT_Isr b_vxworks_Isr
#endif


/* The one global pointer to context. */
BTST_P_Context *g_pContext;

extern uint32_t g_ulAdjQp;
extern bool g_bCapture;
extern bool g_bInterceptFrame;
extern BRDC_Handle g_hRdc;



/***************************************************************************
* Function prototypes.  Some compilers are picky and need prototypes.
*
*/
static void BTST_P_KeyStrokeHandler
( BTST_TaskHandle          hTask,
 void                    *pvParams );
static void BTST_P_InitInt
( BINT_Handle              hInt );
static void BTST_P_UnInitInt
( BINT_Handle              hInt );



/***************************************************************************
*
*                       --- I M P O R T A N T ---
*
* ADD: Your command here.  Do not litter the while statement.  Make a
* function and add the key here.  This is to make it easy to see and
* modify.
*
* INTRUCTIONS: For adding new commands.
*
* 1) Chose a non-duplicate commands string and shortcut key.
* 2) Create a function for that command.
* 3) Added to the command table / functions listing, and etc.
* 4) Refrain from adding any global variables.
* 5) Maintain sanity. :)
*
* --- pntruong Fri 18 Jun 2004
**************************************************************************/
typedef void (*BTST_P_CmdFunc)(BTST_P_Context *pContext);
typedef struct
{
	BTST_P_CmdFunc             pfCmdFunc0;
	BTST_P_CmdFunc             pfCmdFunc1;
	const char                *pchCmdStrLong;
	const char                *pchCmdStrShrt;
	const char                *pchCmdDesc;
} BTST_P_CmdEntry;

static const BTST_P_CmdEntry s_CmdTable[] =
{
	/* Handle Selection. */
	{BTST_P_Cmp0Select              , NULL           , "select_cmp0"      , "c0"            , "Select compositor0 handle"                               },
	{BTST_P_Cmp1Select              , NULL           , "select_cmp1"      , "c1"            , "Select compositor1 handle"                               },
	{BTST_P_Cmp2Select              , NULL           , "select_cmp2"      , "c2"            , "Select compositor2 handle (bypass compositor)"           },
	{BTST_P_Cmp3Select              , NULL           , "select_cmp3"      , "c3"            , "Select compositor3 handle (bypass compositor)"           },
	{BTST_P_Cmp4Select              , NULL           , "select_cmp4"      , "c4"            , "Select compositor4 handle (bypass compositor)"           },
	{BTST_P_Cmp5Select              , NULL           , "select_cmp5"      , "c5"            , "Select compositor5 handle (bypass compositor)"           },
	{BTST_P_Mpeg0Select             , NULL           , "select_mpg0"      , "m0"            , "Select mpeg0 source handle"                              },
	{BTST_P_Mpeg1Select             , NULL           , "select_mpg1"      , "m1"            , "Select mpeg1 source handle"                              },
	{BTST_P_Mpeg2Select             , NULL           , "select_mpg2"      , "m2"            , "Select mpeg2 source handle"                              },
	{BTST_P_Mpeg3Select             , NULL           , "select_mpg3"      , "m3"            , "Select mpeg3 source handle"                              },
	{BTST_P_Vfd0Select              , NULL           , "select_vfd0"      , "v0"            , "Select vfd0 source handle"                               },
	{BTST_P_Vfd1Select              , NULL           , "select_vfd1"      , "v1"            , "Select vfd1 source handle"                               },
	{BTST_P_Vfd2Select              , NULL           , "select_vfd2"      , "v2"            , "Select vfd2 source handle"                               },
	{BTST_P_Vfd3Select              , NULL           , "select_vfd3"      , "v3"            , "Select vfd3 source handle"                               },
	{BTST_P_Vfd4Select              , NULL           , "select_vfd4"      , "v4"            , "Select vfd4 source handle"                               },
	{BTST_P_Vfd4Select              , NULL           , "select_vfd5"      , "v5"            , "Select vfd5 source handle"                               },
	{BTST_P_Analog0Select           , NULL           , "select_anlg0"     , "a0"            , "Select analog0 source handle"                            },
	{BTST_P_Analog1Select           , NULL           , "select_anlg1"     , "a1"            , "Select analog1 source handle"                            },
	{BTST_P_Gfx0Select              , NULL           , "select_gfx0"      , "g0"            , "Select gfx0 source handle"                               },
	{BTST_P_Gfx1Select              , NULL           , "select_gfx1"      , "g1"            , "Select gfx1 source handle"                               },
	{BTST_P_Gfx2Select              , NULL           , "select_gfx2"      , "g2"            , "Select gfx2 source handle"                               },
	{BTST_P_Gfx3Select              , NULL           , "select_gfx3"      , "g3"            , "Select gfx3 source handle"                               },
	{BTST_P_Gfx4Select              , NULL           , "select_gfx4"      , "g4"            , "Select gfx4 source handle"                               },
	{BTST_P_Gfx5Select              , NULL           , "select_gfx5"      , "g5"            , "Select gfx5 source handle"                               },
	{BTST_P_656in0Select            , NULL           , "select_656in0"    , "656in0"        , "Select 656in0 source handle"                             },
	{BTST_P_656in1Select            , NULL           , "select_656in1"    , "656in1"        , "Select 656in1 source handle"                             },
	{BTST_P_HdrFe0Select            , NULL           , "select_hfe0"      , "hfe0"          , "Select HDMI0 Rx frontend"                                },
	{BTST_P_HdrFe1Select            , NULL           , "select_hfe1"      , "hfe1"          , "Select HDMI0 Rx frontend"                                },
	{BTST_P_Hddvi0Select            , NULL           , "select_dvi0"      , "h0"            , "Select hddvi0 source handle"                             },
	{BTST_P_Hddvi1Select            , NULL           , "select_dvi1"      , "h1"            , "Select hddvi1 source handle"                             },
	{BTST_P_Ds0Select               , NULL           , "select_ds0"       , "ds0"           , "Select downsample0 source handle"                        },
	{BTST_P_Win0Select              , NULL           , "select_win0"      , "w0"            , "Select window0 (video) handle"                           },
	{BTST_P_Win1Select              , NULL           , "select_win1"      , "w1"            , "Select window1 (video) handle"                           },
	{BTST_P_Win2Select              , NULL           , "select_win2"      , "w2"            , "Select window2 (analog) handle"                          },
	{BTST_P_Win3Select              , NULL           , "select_win3"      , "w3"            , "Select window3 (video) handle"                           },
	{BTST_P_Win4Select              , NULL           , "select_win4"      , "w4"            , "Select window4 (video) handle"                           },
#if (BFramework_NUM_XVD_CORES > 0) && !BTST_VDC_ONLY
	{BTST_P_Xvd0Select              , NULL           , "select_xvd0"      , "x0"            , "Select XVD_0"                                            },
	{BTST_P_Xvd1Select              , NULL           , "select_xvd1"      , "x1"            , "Select XVD_1"                                            },
#endif
	{BTST_P_Mem0Select              , NULL           , "select_mem0"      , "mem0"          , "Select memory 0 controller"                              },
#if (MEMC_1_MEM_PBASE)
	{BTST_P_Mem1Select              , NULL           , "select_mem1"      , "mem1"          , "Select memory 1 controller"                              },
#endif
	{BTST_P_SharedMemSelect         , NULL           , "select_shared_mem", "shmem"         , "Select shared memory"			},
	{BTST_P_MpgChannelSelect        , NULL           , "select_mpgch"     , "ch"            , "Select MPEG Decoder Channel handle"                      },
	{BTST_P_MpgDmSelect             , NULL           , "select_mpgdm"     , "dmn"           , "Select MPEG Decoder Display Manager Number"              },

	/* Open/Close commands */
	{BTST_P_OpenVdcMisc             , BTST_P_OpenComp, "open_vdc_cmp"     , "ovc"           , "Open VDC and selected compositor handle"                 },
	{BTST_P_OpenVdcMisc             , NULL           , "open_vdc"         , "ov"            , "Open VDC handle"                                         },
	{BTST_P_CloseVdcMisc            , NULL           , "close_vdc"        , "cv"            , "Close VDC handle"                                        },
	{BTST_P_OpenComp                , NULL           , "open_comp"        , "oc"            , "Open selected compositor handle"                         },
	{BTST_P_CloseComp               , NULL           , "close_comp"       , "cc"            , "Close selected compositor handle"                        },
	{BTST_P_OpenDisp                , NULL           , "open_disp"        , "od"            , "Open selected display handle"                            },
	{BTST_P_CloseDisp               , NULL           , "close_disp"       , "cd"            , "Close selected display handle"                           },
	{BTST_P_OpenGfxWin              , NULL           , "open_gfx"         , "og"            , "Open gfx src and window handle"                          },
	{BTST_P_CloseGfxWin             , NULL           , "close_gfx"        , "cg"            , "Close gfx src and window handle"                         },
	{BTST_P_GfxWinTest              , NULL           , "gfx_win_tst"      , "gwt"           , "Gfx window test"                                         },
	{BTST_P_GfxSclModeTest          , NULL           , "gfx_scl_mode"     , "gsclm"         , "Gfx scale (coeff) mode test"                             },
	{BTST_P_ChgGfxSurSettingMode    , NULL           , "change_gss"       , "gss"           , "Change gfx surface setting mode"                         },
	{BTST_P_ChgVfdSurSettingMode    , NULL           , "change_vss"       , "vss"           , "Change VFD surface setting mode"                         },
	{BTST_P_OpenVidSrc              , NULL           , "open_source"      , "os"            , "Open selected video src"                                 },
	{BTST_P_CloseVidSrc             , NULL           , "close_source"     , "cs"            , "Close selected video src"                                },
	{BTST_P_OpenVidWin              , NULL           , "open_win"         , "ow"            , "Open selected video window with selected src"            },
	{BTST_P_CloseVidWin             , NULL           , "close_win"        , "cw"            , "Close selected video window with selected src"           },
	{BTST_P_OpenRfm                 , NULL           , "open_rfm"         , "orf"           , "Open RFM module"                                         },
	{BTST_P_CloseRfm                , NULL           , "close_rfm"        , "crf"           , "Close RFM module"                                        },
#if (BTST_P_SUPPORT_HDR)
	{BTST_P_OpenHdrFe               , NULL           , "open_hfe"         , "ohfe"          , "Open HDMI Rx FE module"                                  },
	{BTST_P_CloseHdrFe              , NULL           , "close_hfe"        , "chfe"          , "Close HDMI Rx FE module"                                 },
#endif
	{BTST_P_OpenHdmi                , NULL           , "open_hdm"         , "ohd"           , "Open HDMI module"                                        },
	{BTST_P_CloseHdmi               , NULL           , "close_hdm"        , "chd"           , "Close HDMI module"                                       },
	{BTST_P_OpenMpgChannel          , NULL           , "open_mpgch"       , "och"           , "Open selected MPG decoder channel"                       },
	{BTST_P_CloseMpgChannel         , NULL           , "close_mpgch"      , "cch"           , "Close selected MPG decoder channel"                      },
#if (BTST_P_SUPPORT_GRC)
	{BTST_P_OpenGrc                 , NULL           , "open_grc"         , "ogrc"          , "Open graphics compositor (GRC)"                          },
	{BTST_P_CloseGrc                , NULL           , "close_grc"        , "cgrc"          , "Close graphics compositor (GRC)"                         },
#endif
#if (BTST_P_SUPPORT_P3D)
	{BTST_P_OpenP3d                 , NULL           , "open_p3d"         , "op3d"          , "Open 3D module (P3D)"                                    },
	{BTST_P_CloseP3d                , NULL           , "close_p3d"        , "cp3d"          , "Close 3D module (P3D)"                                   },
#endif
#if !BTST_VDC_ONLY && defined(EMULATION)
	{BTST_OpenDecoder               , NULL           , "open_decoder"     , "ox"            , "Open xvd module (P3D)"                                   },
	{BTST_CloseDecoder              , NULL           , "close_decoder"    , "cx"            , "Close xvd module (P3D)"                                  },
#endif
	{BTST_P_GetCap                  , NULL           , "getcap"           , "gcap"          , "Get capability of vdc"                                    },

	/* Display / Compositor functions */
	{BTST_P_PrintHelp               , NULL           , "help"             , "hp"            , "Print usage and help"                                    },
	{BTST_P_PrintStatus             , NULL           , "print"            , "ps"            , "Print handle values"                                     },
	{BTST_P_PrintRfmStatus          , NULL           , "print_rfm"        , "prf"           , "Print RFM status"                                        },
	{BTST_P_CustomDisplay           , NULL           , "custom"           , "custom"        , "Customize display creation with display ID and master Tg"},
	{BTST_P_ChgDispFmt_Interactive  , NULL           , "change_fmt"       , "cf"            , "Change selected comp/disp output format"                 },
	{BTST_P_ChgDispArc              , NULL           , "change_arc"       , "arc"           , "Change selected comp/disp output aspec ratio"            },
	{BTST_P_ChgDispTimebase         , NULL           , "change_tb"        , "tb"            , "Change selected comp/disp timebase"                      },
	{BTST_P_RgbOutput               , NULL           , "rgb_output"       , "rgb"           , "Change selected comp/disp output to RGB"                 },
	{BTST_P_HdmiRgbOutput           , NULL           , "rgb_hdmi"         , "rgbh"          , "Change selected comp/disp output to HDMI RGB"            },
	{BTST_P_CvbsOutput              , NULL           , "cvbs_svideo"      , "cvbs"          , "Change selected comp/disp output to CVBS+Svideo"         },
	{BTST_P_ToggleHsync             , NULL           , "ext_sync"         , "syn"           , "Change selected comp/disp output to RGB + Ext H/V syncs" },
	{BTST_P_ChgBgColor              , NULL           , "change_bgc"       , "bg"            , "Change selected comp/disp background color"              },
	{BTST_P_ToggleXvYcc             , NULL           , "xvycc"            , "xv"            , "Change selected comp/disp xvycc color"                   },
	{BTST_P_ToggleSS                , NULL           , "spectrum"         , "ss"            , "Toggle selected comp/disp spread spectrum"               },
	{BTST_P_ToggleStgOut            , NULL           , "set_stg"          , "stg"           , "Set selected comp/disp STG output"                    },
	{BTST_P_Toggle656Out            , NULL           , "toggle_656"       , "656"           , "Toggle selected comp/disp 656 output"                    },
	{BTST_P_ToggleRfmOut            , NULL           , "toggle_rfm"       , "rfm"           , "Toggle selected comp/disp RF output"                     },
	{BTST_P_ToggleHdmiOut           , NULL           , "toggle_hdm"       , "hdm"           , "Toggle selected comp/disp HDMI output"                   },
	{BTST_P_ToggleFrameDrop         , NULL           , "toggle_drop"      , "drop"          , "Toggle selected comp/disp frame drop"                    },
	{BTST_P_ToggleHdmiSync          , NULL           , "hdmi_sync"        , "sync"          , "Toggle selected comp/disp HDMI output sync"              },
	{BTST_P_ToggleHdmiCb            , NULL           , "hdmi_cb"          , "hcb"           , "Toggle selected comp/disp HDMI callback"                 },
	{BTST_P_ToggleStgCb             , NULL           , "stg_cb"           , "stgcb"         , "Toggle selected comp/disp stg callback"                 },
	{BTST_P_ToggleRateCb            , NULL           , "rate_cb"          , "ratecb"        , "Toggle selected comp/disp rate change callback"          },
	{BTST_P_MuteOutput              , NULL           , "mute"             , "mute"          , "Mute output through csc"                                 },
	{BTST_P_SetMacrovision          , NULL           , "Macrovision"      , "mac"           , "Set selected comp/disp Macrovision output"               },
	{BTST_P_SetDcs                  , NULL           , "DCS"              , "dcs"           , "Set selected comp/disp DCS output"                       },
	{BTST_P_SetDac                  , NULL           , "DAC"              , "dacs"          , "Set DACs"                                                },
	{BTST_P_SetMpaaDeci             , NULL           , "mpaa_deci"        , "mpaa"          , "Set MPAA decimation"                                     },
	{BTST_P_SetHdmiDropLines        , NULL           , "hdmi_drop"        , "hdrp"          , "Set Hdmi Drop Lines"                                     },
	{BTST_P_SetHdmiDeepColor        , NULL           , "hdmi_deep"        , "deep"          , "Set Hdmi Deep Color"                                     },
	{BTST_P_ChangeDispVideo         , NULL           , "change_dac"       , "cdsp"          , "Change DAC output"                                       },
	{BTST_P_SetLCDDimCtrl           , NULL           , "lcd_dim"          , "dim"           , "Set LCD panel dimming control"                           },
	{BTST_P_LoadMicrocode           , NULL           , "load_ucode"       , "ucode"         , "Load VEC custom microcode"                               },
	{BTST_P_ChgDispAlign            , NULL           , "align"            , "align"         , "Align display timing"                                    },
	{BTST_P_SetOsdConfig            , NULL           , "set_cmp_OSD"      , "set_cmp_osd"   , "Set compositor OSD"                                      },
	{BTST_P_GetOsdConfig            , NULL           , "get_cmp_OSD"      , "get_cmp_osd"   , "Get compositor OSD settings"                             },
	{BTST_P_DisplayGetCap           , NULL           , "getdispcap"       , "gdcap"         , "Get capability of display"                               },
	{BTST_P_CompGetCap              , NULL           , "getcmpcap"        , "gccap"         , "Get capability of compositor"                            },
	/* Window functions */
	{BTST_P_ShowHideWin             , NULL           , "show"             , "sh"            , "Toggle visibility of selected window"                    },
	{BTST_P_ResizeWin               , NULL           , "resize"           , "re"            , "Resize selected window"                                  },
	{BTST_P_Sweep                   , NULL           , "sweep"            , "sweep"          , "Resize selected window"                                  },
	{BTST_P_OverScan                , NULL           , "overscan"         , "ovs"           , "DTV overscan"                                            },
	{BTST_P_PictureByPicture        , NULL           , "pbp"              , "pbp"           , "Picture-by-picture"                                      },
	{BTST_P_Zoom                    , NULL           , "zooming"          , "zoom"          , "Zoom in/out selected window"                             },
	{BTST_P_ChgSclFctrRndToler      , NULL           , "scl_fctr_rnd"     , "sfr"           , "change scl fctr rounding toler"                          },
	{BTST_P_SetAlpha                , NULL           , "alpha"            , "alp"           , "Set alpha for selected window"                           },
	{BTST_P_ChgArcMode              , NULL           , "change_am"        , "am"            , "Change selected window ARC mode"                         },
	{BTST_P_ChgChnChgMode           , NULL           , "chn_mode"         , "ccm"           , "Change selected window channel change mode"              },
	{BTST_P_ClipSource              , NULL           , "clipsrc"          , "clp"           , "Clip selected window source"                             },
	{BTST_P_ClipSourceStress        , NULL           , "clipsrcstress"    , "clpstress"     , "Clip selected window source"                             },
	{BTST_P_DstClipWin              , NULL           , "clipdst"          , "dclp"          , "Clip selected window destination"                        },
	{BTST_P_SclOutCutWin            , NULL           , "scloutcut"        , "dstcut"        , "Cut with scaler-out rect"                                },
	{BTST_P_DstCutMove              , NULL           , "dstcutmove"       , "dcmove"        , "Move window by dst cut"                                  },
	{BTST_P_AspRatRectClip          , NULL           , "arrectclip"       , "arrc"          , "Asp ratio canvas clip"                                   },
	{BTST_P_MpegDecode              , NULL           , "decode"           , "de"            , "Decode selected mpeg source (select pid-pcr)"            },
	{BTST_P_StopMpegDecode          , NULL           , "stopdecode"       , "xde"           , "Stop decode selected mpeg source"                        },
	{BTST_P_MpegDecode3d            , NULL           , "decode_3d"        , "de3d"          , "Decode selected 3d mpeg source (select pid-pcr)"         },
	{BTST_P_Toggle3dDecodeChan      , NULL           , "decodechan_3d"    , "ch3d"          , "Decode channel to open with 3d cap"                      },
	{BTST_P_TogglePlayback          , NULL           , "playback"         , "pb"            , "Toggle playback video source"                            },
	{BTST_P_ToggleCCDump            , NULL           , "ccdump"           , "ccd"           , "Toggle video source CC Dump"                             },
	{BTST_P_EnablePID2Buffer        , NULL           , "pid2buf"          , "p2b"           , "Enable PID filtering to buffer"                          },
	{BTST_P_ChgSrcFmt               , NULL           , "change_sfmt"      , "csf"           , "Change selected analog 656/vdec input format"            },
	{BTST_P_ChgAutoSrcFmt           , NULL           , "change_asfmt"     , "casf"          , "Change auto detect input format"                         },
	{BTST_P_PhaseAdjust             , NULL           , "phase"            , "p"             , "Toggle phase adjustment"                                 },
	{BTST_P_Toggle3dComb            , NULL           , "comb"             , "com"           , "Toggle 3d comb"                                          },
	{BTST_P_ToggleLipSync           , NULL           , "lipsync"          , "lips"          , "Toggle lip sync"                                         },
	{BTST_P_ToggleGetBufIsr         , NULL           , "getbuf_isr"       , "getbuf"        , "Toggle get buf isr"                                      },
	{BTST_P_ToggleMaster            , NULL           , "master_rate"      , "master"        , "Toggle master frame rate"                                },
	{BTST_P_ChgMad                  , NULL           , "deinterlace"      , "mad"           , "Change deinterlacer configuration"                       },
	{BTST_P_ToggleMadOsd            , NULL           , "mad_osd"          , "osd"           , "Toggle deinterlacer OSD"                                 },
	{BTST_P_StressMad               , NULL           , "stress_mad"       , "mads"          , "Toggle deinterlacer stressfully"                         },
	{BTST_P_StressMadGame           , NULL           , "mad_game"         , "madgame"          , "Toggle deinterlacer game stressfully"                    },
	{BTST_P_StressAnrMad            , NULL           , "stress_anrmad"    , "anrs"          , "Toggle ANR/deinterlacer stressfully"                     },
	{BTST_P_SetAfd                  , NULL           , "setafd"           , "afd"           , "Set AFD settings"                                        },
	{BTST_P_ToggleLBox              , NULL           , "letter_box"       , "lbox"          , "Toggle letter box detect"                                },
	{BTST_P_TogglePanScan           , NULL           , "panscan"          , "pan"           , "Toggle stream panscan"                                   },
	{BTST_P_TestUserPanScan         , NULL           , "user_panscan"     , "upan"          , "Test user panscan"                                       },
	{BTST_P_ToggleRgbMatch          , NULL           , "rgbmatch"         , "match"         , "Toggle RGB matching for color space conversion"          },
	{BTST_P_RandomMove              , NULL           , "random_move"      , "move"          , "Randomly move a selected window"                         },
	{BTST_P_Contrast                , NULL           , "contrast"         , "con"           , "Set contrast level"                                      },
	{BTST_P_Saturation              , NULL           , "saturation"       , "sat"           , "Set saturation level"                                    },
	{BTST_P_Hue                     , NULL           , "hue"              , "hue"           , "Set hue level"                                           },
	{BTST_P_Brightness              , NULL           , "brightness"       , "bri"           , "Set brightness level"                                    },
	{BTST_P_Sharpness               , NULL           , "sharpness"        , "shrp"          , "Set sharpness level"                                     },
	{BTST_P_SetTune                 , NULL           , "settune"          , "stune"         , "Set tune parameters"                                     },
	{BTST_P_GetTune                 , NULL           , "gettune"          , "gtune"         , "Get tune parameters"                                     },
	{BTST_P_ResetTune               , NULL           , "resettune"        , "rtune"         , "Reset tune parameters"                                   },
	{BTST_P_WhiteBalance            , NULL           , "wb"               , "wb"            , "Set white balance"                                       },
	{BTST_P_ColorTemp               , NULL           , "colortemp"        , "ctemp"         , "Set color temp"                                          },
	{BTST_P_AttenuationRGB          , NULL           , "colorrgb"         , "crgb"          , "Set color attenuation RGB"                               },
	{BTST_P_DvoAttenuationRGB       , NULL           , "dvocolorrgb"      , "dvocrgb"       , "Set dvo color attenuation RGB"                           },
	{BTST_P_ColorClip               , NULL           , "colorclip"        , "cclp"          , "Change color clip setting"                               },
	{BTST_P_LoadCabTable            , NULL           , "loadcab"          , "lcab"          , "Load user customized CAB table"                          },
	{BTST_P_LoadLabTable            , NULL           , "loadlab"          , "llab"          , "Load user customized LAB table"                          },
	{BTST_P_SetCabParams            , NULL           , "setcab"           , "cabs"          , "Change cab settings"                                     },
	{BTST_P_SetCmsParams            , NULL           , "setcms"           , "cms"           , "Change CMS settings"                                     },
	{BTST_P_ToggleAllPep            , NULL           , "allpeps"          , "pep"           , "Toggle all PEP settings"                                 },
	{BTST_P_GetLumaStatus           , NULL           , "lumastatus"       , "lstat"         , "Get luma status"                                         },
	{BTST_P_SetLumaStatConfig       , NULL           , "lumastatconf"     , "lstatcfg"      , "Set Luma status configuration"                           },
	{BTST_P_GetChromaStatus         , NULL           , "chromastatus"     , "cstat"         , "Get chroma status"                                       },
	{BTST_P_SetChromaStatConfig     , NULL           , "chromastatconf"   , "cstatcfg"      , "Set Chroma status configuration"                         },
	{BTST_P_WinSplitScreen          , NULL           , "wspltscreen"      , "wsplit"        , "Change window split screen demo mode setting"            },
	{BTST_P_ContrastStretch         , NULL           , "labcons"          , "labcs"         , "Change lab contrast stretch setting"                     },
	{BTST_P_BlueStretch             , NULL           , "labblue"          , "blues"         , "Change lab blue stretch setting"                         },
	{BTST_P_ScaleMode               , NULL           , "scale_mode"       , "sclm"          , "Toggle horizontal scaling mode (linear or non-linear)"   },
	{BTST_P_MuteMode                , NULL           , "mute_mode"        , "mm"            , "Set Video Source Mute Mode"                              },
#if (BFramework_NUM_XVD_CORES > 0) && !BTST_VDC_ONLY
	{BTST_P_ToggleXvdFreeze         , NULL           , "xvd_freeze"       , "freeze"        , "Toggle XVD channel freeze"                               },
	{BTST_P_SetXvdRefreshRate       , NULL           , "ch_refresh_rate"  , "xr"            , "Set xvd channel monitor refresh rate"                    },
	{BTST_P_SetXvd1080pMode         , NULL           , "1080pmode"        , "1080p"         , "Set xvd channel 1080p mode"                              },
#endif
	{BTST_P_DelayOffset             , NULL           , "buf_delay"        , "delay"         , "Set video window buffer delay"                           },
	{BTST_P_SetLumaKey              , NULL           , "lumakey"          , "lkey"          , "Set Luma Key settings for selected video window "        },
	{BTST_P_GetLumaKey              , NULL           , "getlumakey"       , "glkey"         , "Get Luma Key settings for selected video window "        },
	{BTST_P_SetForceCapture         , NULL           , "forcecap"         , "cap"           , "Set force capture for selected video window "            },
	{BTST_P_ChgFixColor             , NULL           , "fixcolor"         , "fcl"           , "Change source fix color"                                 },
	{BTST_P_ChgSrcCsc               , NULL           , "SrcCsc"           , "srccsc"        , "Change source color matrix"                              },
	{BTST_P_ChgWinCsc               , NULL           , "WinCsc"           , "wincsc"        , "Change window color matrix"                              },
	{BTST_P_ChgDvoCsc               , NULL           , "DvoCsc"           , "dvocsc"        , "Change dvo ouput color matrix"                           },
	{BTST_P_ChgVfFilter             , NULL           , "vffilter"         , "vff"           , "Change vf filter coefficients"                           },
	{BTST_P_SetGameDelay            , NULL           , "gamedelay"        , "game"          , "Change game mode delay setting"                          },
	{BTST_P_DnrFilter               , NULL           , "dnr"              , "dnr"           , "Set video window DNR filter level"                       },
	{BTST_P_SetMosaicMode           , NULL           , "mosaic"           , "mosaic"        , "Set window Mosaic mode"                                  },
	{BTST_P_SetMosaicZorder         , NULL           , "mosaiczorder"     , "mzorder"       , "Set window Mosaic mode z order"                          },
	{BTST_P_MosaicModeResize        , NULL           , "mosaicresize"     , "mosaicres"     , "Stress test window Mosaic mode resize"                   },
	{BTST_P_SetMosaicTracking       , NULL           , "mosaictrack"      , "mtrack"        , "Set window Mosaic tracking channel"                      },
	{BTST_P_MosaicMove              , NULL           , "mosaic_move"      , "mmove"         , "Randomly move a window's Mosaics"                        },
	{BTST_P_SrcSplitScreen          , NULL           , "sspltscreen"      , "ssplit"        , "Change source split screen demo mode setting"            },
	{BTST_P_ToggleAnr               , NULL           , "anr"              , "anr"           , "Toggle anr"                                              },
	{BTST_P_PcInSyncConfig          , NULL           , "pc_syncset"       , "pcsync"        , "Change PC source sync settings"                          },
	{BTST_P_PcInSyncStatus          , NULL           , "pc_syncstatus"    , "pcstatus"      , "Change PC source sync settings"                          },
	{BTST_P_HVStart                 , NULL           , "hvstart"          , "hv"            , "Change HV start settings for PC and HD_DVI source "      },
	{BTST_P_HdDviConfig             , NULL           , "hddviconfig"      , "dviconfig"     , "Change HD_DVI settings"                                  },
	{BTST_P_GetSrcStatus            , NULL           , "srcstatus"        , "srcstatus"     , "Get source status"                                       },
	{BTST_P_CustomVdec              , NULL           , "customvdec"       , "vdec"          , "Cusomize vdec settings"                                  },
	{BTST_P_TogglePsF               , NULL           , "psf"              , "psf"           , "Toggle PsF feature on the selected source"               },
	{BTST_P_GetBwParams             , NULL           , "get_bw"           , "get_bw"        , "Get bandwidth equation parameters"                       },
	{BTST_P_SetBwParams             , NULL           , "set_bw"           , "set_bw"        , "Set bandwidth equation parameters"                       },
	{BTST_P_ChgCompression          , NULL           , "compression"      , "comp"          , "Change video compression configuration"                  },
	{BTST_P_ForceSyncLock           , NULL           , "synclock"         , "lock"          , "Force window to be sync-locked"                          },
	{BTST_P_MadAllocFull            , NULL           , "madallocfull"     , "maf"           , "Force MAD full buffers allocation"                       },
	{BTST_P_MadShrinkWidth          , NULL           , "madshrinkwidth"   , "msw"           , "Shrink src width to fit mad limitation"                  },
	{BTST_P_SetMinSrcFmt            , NULL			 , "minsrcfmt"        , "msf"			, "minimum source fmt to avoid buffer reallocation"         },
	{BTST_P_SetMinDspFmt            , NULL			 , "mindspfmt"        , "mdf"			, "minimum display fmt to avoid buffer reallocation"        },
	{BTST_P_ToggleByPassVideoProcessing, NULL        , "toggle_vprocess"  , "vprocess"      , "toggle bypass video processing"                          },
	{BTST_P_GetMadDynamicDefault    , NULL           , "getmaddefault"    , "getmad"        , "Get Dynamic default settings for MAD"                    },
	{BTST_P_SetSclConfig            , NULL           , "sclcfg"            , "sclcfg"        , "Set SCL configuration"                    },
	{BTST_P_WindowGetCap            , NULL           , "getwincap"        , "gwcap"         , "Get capability of window"                                },

	/* Source commands */
	{BTST_P_ReMapDetectFmt          , NULL           , "fmt_remap"        , "remap"         , "Change source format remap"                              },
	{BTST_P_SourcePending           , NULL           , "srcpending"       , "srcpd"         , "Change source pending mode"                              },
	{BTST_P_SourceResume            , NULL           , "srcresume"        , "resume"        , "Resume from source pending"                              },
	{BTST_P_ForcePending            , NULL           , "forcesrcpd"       , "fsrcpd"        , "Force from source pending"                               },
	{BTST_P_Source3D                , NULL           , "source3d"         , "src3d"         , "Override source orientation"                             },
	{BTST_P_SourceGetCap            , NULL           , "getsrccap"        , "gscap"         , "Get capability of source"                                },

#ifndef IKOS_EMULATION
#if BTST_P_SUPPORT_TUNER
	/* Frontend commands */
	{BTST_P_Tune                    , NULL           , "set_tuner"        , "tune"          , "Set up tuner and demodulator"                            },
	{BTST_P_GetVsbStatus            , NULL           , "get_vsb_stat"     , "vsb_stat"      , "Get VSB demodulator status"                              },
	{BTST_P_XptSrc                  , NULL           , "xpt_source"       , "xpt_src"       , "Select source for transport processor"                   },
	{BTST_P_Xpt_PSI                 , NULL           , "xpt_PSI"          , "xpt_psi"       , "Parse the PSI section"                                   },
#endif
#endif

#if (BTST_P_SUPPORT_GRC)
	/* Graphics compositor (GRC) commands */
	{BTST_P_GrcCompressDCE          , NULL           , "grc_dce"          , "dce"           , "GRC DCE compress surface"                                },
#endif

#if (BTST_P_SUPPORT_P3D)
	/* PX3D tests */
	{BTST_P_P3dRenderTest           , NULL           , "p3d_test"         , "p3d"           , "PX3D render test"                                        },
#endif

	/* Other commands */
	{BTST_P_Stub                    , NULL           , "stub"             , "stub"          , "Stub command that does nothing"                          },
	{BTST_P_VideoList               , NULL           , "videolist"        , "videolist"     , "List the files in the directory"                         },
	{BTST_P_Cat                     , NULL           , "cat"              , "cat"           , "Cat the file"                                            },
	{BTST_P_RandDropIsr             , NULL           , "rand_drop"        , "rdrop"         , "Randomly drop MPEG ready isr"                            },
	{BTST_P_ChipFeature             , NULL           , "feature"          , "feat"          , "Print out chip features"                                 },
	{BTST_P_VdcDbgDump              , NULL           , "vdcdbg"           , "vdbg"          , "Dump out VDC debug information"                          },
#ifdef BVDC_P_DISPLAY_DUMP
	{BTST_P_DispTblDump             , NULL           , "dispdump"         , "ddmp"          , "Dump out VDC display tables"                             },
#endif
	{BTST_P_DispUcodeDump           , NULL           , "itUcodeDump"      , "udmp"          , "Dump out VEC IT microdode (all)"                         },
	{BTST_P_UseStaticBuffer         , NULL           , "static_buffer"    , "static"        , "Mpeg feeder feeds from static frame stores"              },
	{BTST_P_GraphicsFormat          , NULL           , "graphics_format"  , "gformat"       , "Graphics format in the surface"                          },
	{BTST_P_UseGfxForMfd            , NULL           , "gfx_2_mfd"        , "gfxmfd"        , "Mpeg feeder feeds from gfx surface"                      },
	{BTST_P_InterceptFrame          , NULL           , "intercept"        , "int"           , "Intercept a mpeg frame from frame stores"                },
	{BTST_P_SwapWindows             , NULL           , "swap"             , "swap"          , "Swap main/pip"                                           },
	{BTST_P_MirModeChnChg           , NULL           , "mchannel"         , "mchn"          , "Mirror mode channel change"                              },
	{BTST_P_MpgDisplayMode          , NULL           , "mpg_disp_mode"    , "mdm"           , "Select mpeg channel display field mode"                  },
	{BTST_P_MpegDecodeMode          , NULL           , "mvd_dec_mode"     , "dm"            , "Select mvd channel TSM/Vsync mode"                       },
	{BTST_P_FormatChange            , NULL           , "format"           , "form"          , "Stress test format change of selected display"           },
	{BTST_P_ChannelChange           , NULL           , "channel"          , "chan"          , "Stress test channel change of selected display"          },
	{BTST_P_PR14134                 , NULL           , "pr14134"          , "14134"         , "PR14134 channel change of selected display"              },
	{BTST_P_StressTest              , NULL           , "stress"           , "test"          , "Stress test format + channel change of selected display" },
	{BTST_P_LineCntTest             , NULL           , "line_count"       , "line"          , "Test line count register of selected vec"                },
	{BTST_P_DbgLevel                , NULL           , "debug"            , "dbg"           , "Change debug level"                                      },
	{BTST_P_GlobalLevel             , NULL           , "global"           , "glb"           , "Change global debug level"                               },
	{BTST_P_ManualApplyChanges      , NULL           , "apply"            , "apl"           , "Applychanges to hardware"                                },
	{BTST_P_AbortChanges            , NULL           , "abort"            , "abo"           , "Abort changes that failed to applied"                    },
	{BTST_P_ToggleApplyImm          , NULL           , "apply_imm"        , "imm"           , "Toggle Applychanges to hardware immediately"             },
	{BTST_P_ToggleApplyErr          , NULL           , "apply_err"        , "err"           , "Toggle Applychanges error catching"                      },
	{BTST_P_MemMonitor              , NULL           , "mem_monitor"      , "monit"         , "Update memory monitor status"                            },
	{BTST_P_ToggleSclDown           , NULL           , "scl_down"         , "scl"           , "Toggle between full screen an scale down"                },
	{BTST_P_ReadWriteISRMisorders   , NULL           , "rw_isr"           , "rwisr"         , "Test read/write ISR misorders"                           },
	{BTST_P_ChangeVdcSettings       , NULL           , "setting"          , "set"           , "Change VDC default settings"                             },
	{BTST_P_ChangeVdcHeapSettings   , NULL           , "heapsetting"      , "setheap"       , "Change VDC heap settings"                                },
	{BTST_P_ChangeVdcBandGapSettings, NULL           , "bgsetting"        , "bgset"         , "Change VDC default bandgap settings"                     },
	{BTST_P_CleanUp                 , NULL           , "quit"             , "q"             , "Clean up and quit application"                           },
	{BTST_P_GetCbStats              , NULL           , "cbstat"           , "cbs"           , "Get registered callbacks' stats"                         },
	{BTST_P_ResetCbStats            , NULL           , "rstat"            , "rcbs"          , "Reset registered callbacks' stats"                       },
	{BTST_P_AddCbBin                , NULL           , "addbin"           , "abin"          , "Add a stat bin to a callback"                            },
	{BTST_P_CaptureRul              , NULL           , "captureRul"       , "rul"           , "Capture RUL's"                                           },
	{BTST_P_SetPixelFormat          , NULL           , "setPxlFmt"        , "pxlfmt"        , "Set window's pixel format"                               },
	{BTST_P_CapturePicture          , NULL           , "capturePicture"   , "cappic"        , "Capture a field or frame"                                },
	{BTST_P_ShowCapPic              , NULL           , "showPicture"      , "showpic"       , "Toggle the flag for capturing a field or frame"          },
	{BTST_P_ToggleSrcCb             , NULL           , "srccb"            , "srccb"         , "Toggle source callback"                                  },
	{BTST_P_ToggleScaleDown         , NULL           , "scaledown"        , "scld"          , "Toggle scale up/down"                                    },
	{BTST_P_TogglePicList           , NULL           , "pic_list"         , "list"          , "Toggle mosaic test mode"                                 },
	{BTST_P_ToggleStrmClip          , NULL           , "strmclip"         , "sclp"          , "Toggle stream clip test mode"                            },
	{BTST_P_WorstPicList            , NULL           , "worst"            , "worst"         , "Toggle every other picture size mosaic test mode"        },
	{BTST_P_Sleep                   , NULL           , "sleep"            , "sleep"         , "Sleep the user task"                                     },
	{BTST_P_Break                   , NULL           , "break"            , "break"         , "break the user task for multiple second"                 },
	{BTST_P_SetCoeffTblIndex        , NULL           , "coeffindex"       , "cti"           , "Coefficient table index"                                 },
	{BTST_P_ToggleStopRul           , NULL           , "stoprul"          , "srul"          , "Toggle StopRul"                                          },
	{BTST_P_InstallBvnErrCb         , NULL           , "bvnCb"            , "bvncb"         , "Install BVN errors callback"                             },
	{BTST_P_CheckBvnErr             , NULL           , "checkBvn"         , "check"         , "Check BVN errors"                                        },
	{BTST_P_ClearBvnErr             , NULL           , "clearBvn"         , "clear"         , "Clear BVN errors"                                        },
	{BTST_P_MaskBvnErr              , NULL           , "maskBvn"          , "mask"          , "Mask Off BVN errors"                                     },
	{BTST_P_StressTestBVN           , NULL           , "stressBvn"        , "sbvn"          , "Stress test BVN"                                         },
	{BTST_P_LipSyncStressTest       , NULL           , "lipsyncStress"    , "lipstress"     , "Stress test lipsync"                                     },
	{BTST_P_StressTestAutoFormat    , NULL           , "autoformatstress" , "autofmtstress" , "Stress test auto format on C0"                           },
	{BTST_P_StressTestResizeFormat  , NULL           , "resizefmtstress"  , "resizefmtstress", "Stress test Resize and format change"                   },
	{BTST_P_StressTest3DVideo       , NULL           , "stress3dvideo"    , "s3dvideo"       , "Stress test 3d video"                                         },
	{BTST_P_StressTestCustomFormat  , NULL           , "stresscustomfmt"  , "scfmt"          , "Stress test customer format"                            },
	{BTST_P_DetectMpegInputTest     , NULL           , "detectmpeg"       , "detmpeg"       , "Detect MPEG source input"                                },
	{BTST_P_StressCustomFmtResize   , NULL           , "cfmtstress"        , "cfmt"         , "custom format stress test on STG"                        },
	{BTST_P_SetPicDropMode          , NULL           , "picDropMode"      , "pdrop"         , "Picture drop mode"                                       },
	{BTST_P_StartCmdRecord          , NULL           , "startRecord"      , "srecord"       , "Start command recording"                                 },
	{BTST_P_EndCmdRecord            , NULL           , "endRecord"        , "erecord"       , "End command recording"                                   },
	{BTST_P_DisplayVecSwap          , NULL           , "dispvecswap"      , "dvswap"        , "Vec Swap using BVDC_Display_Create"                      },
	{BTST_P_Display3D               , NULL           , "disp3d"           , "disp3d"        , "Set display 3D Orientation"                              },
	{BTST_P_Display3DSrcBufSelect   , NULL           , "3dsrcbufsel"      , "3dsrc"         , "Set 3d source buffer select"                             },
#if (BVDC_BUF_LOG == 1)
	{BTST_P_EnableMultiBufLog       , NULL           , "buflog"           , "buflog"        , "Get multibuffering log"                                  },
	{BTST_P_MultiBufLogManualTrigger, NULL           , "buflogtrigger"    , "buflogtrig"    , "Manually trigger multibuffering log"                     },
	{BTST_P_ToggleMultiBufLog       , NULL           , "togglewinbuflog"  , "togwinbuflog"  , "Toggle multibuffering log for selected window"           },
#endif

#ifdef BTST_MFD_SIZE
	{BTST_P_ChangeMfdSize           , NULL           , "msize"            , "msize"         , "Change MFD size"                                         },
#endif
	{BTST_P_ChangeBarData           , NULL           , "bardata"          , "bar"           , "Change Bar Data setting"                                 },
	{BTST_P_RdcBlockOut             , NULL           , "rdc"              , "rdc_block_out" , "RDC register blockout"                                   },
	{BTST_P_Cm3d                    , NULL           , "cm3d"             , "cm3d"          , "CM3D test"                                               },
	{BTST_P_Loadrts                 , NULL           , "loadrts"          , "load_rts"      , "load different RTS"                                      },
	{BTST_P_SetupHeapFromFile       , NULL           , "setheapfile"      , "setfile"       , "load different heap config from file"                    },

	{BTST_P_RegressionTests         , NULL           , "regression"       , "doit"          , "Run VDC Regression Tests"                                },
	{BTST_P_ResourceLeak_AutoTest   , NULL           , "resourceleak"     , "srclk"         , "Run VDC Resource Leak Tests"                             },
#ifdef BTST_P_SUPPORT_VCE
#include "vce_encoder_cmds.inc"
#endif
#ifdef BTST_P_SUPPORT_MUX
#include "mux_cmds.inc"
#endif
};

#define BTST_P_CMD_COUNT (sizeof(s_CmdTable)/sizeof(BTST_P_CmdEntry))


/***************************************************************************
* Beef of Execution loop and User Interaction.
*
*/
static void BTST_P_KeyStrokeHandler
( BTST_TaskHandle          hTask,
 void                    *pvParams )
{
	BTST_P_Context *pContext;
	char achCmd[BTST_P_CMD_LENGHT];
	uint32_t i;
	BERR_Code   eResult;

	BSTD_UNUSED(hTask);
	pContext = (BTST_P_Context*)pvParams;

	eResult = BTST_P_InitLongTimeLock(&pContext->stVdcCfgLock, 2*60*60);
	if (BERR_SUCCESS != eResult)
	{
		printf("Failed to create mux for VDC configuring lock!!!");
		return;
	}

	printf("\nPress q and then Enter to exit the test:\n");

	while(BTST_P_INFITITE_WAIT)
	{
		printf("Enter command (or help)>");

		if(pContext->bRepeatCmd)
		{
			if (pContext->iNumCmds == pContext->iNumCmdsXed)
			{
				fclose(pContext->fp_cmd);

				if (pContext->iLoopCnt > 0)
				{
					/* start another iteration  */
					pContext->fp_cmd = fopen(CMD_RECORD_FILE, "r+");

					pContext->iNumCmdsXed = 0;

				}
				else
				{
					pContext->bRepeatCmd = false;
					pContext->iNumCmds = 0;
				}

				pContext->iLoopCnt--;
			}

			if (pContext->bRepeatCmd)
			{
				fscanf(pContext->fp_cmd, "%s", achCmd);
				pContext->iNumCmdsXed ++;
			}

		}

		if (!pContext->bRepeatCmd)
		{
			scanf("%s", achCmd);

			if (pContext->bRecordCmd)
			{
				/* don't record the stop command */
				if (strcmp("endRecord", achCmd) && strcmp("erecord", achCmd))
				{
					fprintf(pContext->fp_cmd, "%s\n", achCmd);
					pContext->iNumCmds++;
				}
			}
		}

		for(i = 0; i < BTST_P_CMD_COUNT; i++)
		{
			/* If there is command available and match string execute them. */
			if(((s_CmdTable[i].pfCmdFunc0) ||
				(s_CmdTable[i].pfCmdFunc1)) &&
				(!strcmp(s_CmdTable[i].pchCmdStrLong, achCmd) ||
				!strcmp(s_CmdTable[i].pchCmdStrShrt, achCmd)))
			{
				BTST_P_AcquireLongTimeLock(&pContext->stVdcCfgLock);

				/* Execute first function. */
				if(s_CmdTable[i].pfCmdFunc0)
				{
					s_CmdTable[i].pfCmdFunc0(pContext);
				}
				/* Execute second function. */
				if(s_CmdTable[i].pfCmdFunc1)
				{
					s_CmdTable[i].pfCmdFunc1(pContext);
				}
				
				BTST_P_ReleaseLongTimeLock(&pContext->stVdcCfgLock);
				break;
			}
		}

		if(BTST_P_CMD_COUNT == i)
		{
			printf("Unknown command: \"%s\".  Type \"help\" for list of commands.\n", achCmd);
		}
		else if(!strcmp(s_CmdTable[i].pchCmdStrLong, "quit"))
		{
			break;
		}
	}

	BTST_P_UninitLongTimeLock(&pContext->stVdcCfgLock);

	printf("\nTest is done...\n");
	BKNI_SetEvent(pContext->hTestDoneEvent);
	return;
}


/***************************************************************************
*
*/
static void BTST_P_InitInt
( BINT_Handle              hInt )
{
	BSTD_UNUSED(hInt);
	return;
}


/***************************************************************************
*
*/
static void BTST_P_UnInitInt
( BINT_Handle              hInt )
{
	BSTD_UNUSED(hInt);

	return;
}


/***************************************************************************
*
*/
void BTST_P_PrintHelp
( BTST_P_Context          *pContext )
{
	uint32_t i;
	BSTD_UNUSED(pContext);

	printf("Available commands.\n");
	for(i = 0; i < BTST_P_CMD_COUNT; i++)
	{
		printf(" %-5s\t - %-60s\n", s_CmdTable[i].pchCmdStrShrt,
			s_CmdTable[i].pchCmdDesc);
	}
	printf("\n");
	return;
}


/***************************************************************************
*
*/
void BTST_P_PrintStatus
( BTST_P_Context          *pContext )
{
#if BDBG_DEBUG_BUILD
	BDBG_MSG(("hVdc[%d]        = 0x%08x", 0, pContext->hVdc));

	BDBG_MSG(("hCompositor[%d] = 0x%08x", pContext->iSelectedComp,
		pContext->ahCompositor[pContext->iSelectedComp]));

	BDBG_MSG(("hVidSrc[%d]     = 0x%08x", pContext->iSelectedVidSrc,
		pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc));

	BDBG_MSG(("hVidWin[%d][%d]  = 0x%08x",
		pContext->iSelectedComp, pContext->iSelectedWin,
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow));
	BDBG_MSG(("--------------------------"));
#else
	BSTD_UNUSED(pContext);
#endif
	return;
}


/***************************************************************************
*
*/

static void BTST_SetupHeap
( const BFramework_Info   *pFrameworkInfo,
 const BSystem_Info      *pSysInfo,
 BTST_P_Context          *pContext)
{
	BSTD_UNUSED(pFrameworkInfo);
	BSTD_UNUSED(pSysInfo);

	/* Default second DDR heap settings */
	pContext->astMemInfo[0].stVdcHeapSettings = pContext->stSettings.stHeapSettings;

	pContext->astMemInfo[0].stVdcHeapSettings.ulBufferCnt_2HD	  = s_BufCnts[BTST_P_MEM0_BUFCNT_2HD].ulBufCnt;
	pContext->astMemInfo[0].stVdcHeapSettings.ulBufferCnt_2HD_Pip = s_BufCnts[BTST_P_MEM0_BUFCNT_2HD_PIP].ulBufCnt;;
	pContext->astMemInfo[0].stVdcHeapSettings.ulBufferCnt_HD	  = s_BufCnts[BTST_P_MEM0_BUFCNT_HD].ulBufCnt;
	pContext->astMemInfo[0].stVdcHeapSettings.ulBufferCnt_HD_Pip  = s_BufCnts[BTST_P_MEM0_BUFCNT_HD_PIP].ulBufCnt;;
	pContext->astMemInfo[0].stVdcHeapSettings.ulBufferCnt_SD	  = s_BufCnts[BTST_P_MEM0_BUFCNT_SD].ulBufCnt;
	pContext->astMemInfo[0].stVdcHeapSettings.ulBufferCnt_SD_Pip  = s_BufCnts[BTST_P_MEM0_BUFCNT_SD_PIP].ulBufCnt;
	pContext->astMemInfo[0].stVdcHeapSettings.ePixelFormat_2HD    = BTST_P_MEM_PIXFMT_2HD;
	pContext->astMemInfo[0].stVdcHeapSettings.ePixelFormat_HD     = BTST_P_MEM_PIXFMT_HD;
	pContext->astMemInfo[0].stVdcHeapSettings.ePixelFormat_SD     = BTST_P_MEM_PIXFMT_SD;


	if((pContext->astMemInfo[1].hMem) && 
		(BTST_P_INVALID_BUFFER != (int)s_BufCnts[BTST_P_MEM1_BUFCNT_2HD].ulBufCnt))
	{
		pContext->astMemInfo[1].stVdcHeapSettings = pContext->stSettings.stHeapSettings;

		pContext->astMemInfo[1].stVdcHeapSettings.ulBufferCnt_2HD       = s_BufCnts[BTST_P_MEM1_BUFCNT_2HD].ulBufCnt;
		pContext->astMemInfo[1].stVdcHeapSettings.ulBufferCnt_2HD_Pip   = s_BufCnts[BTST_P_MEM1_BUFCNT_2HD_PIP].ulBufCnt;
		pContext->astMemInfo[1].stVdcHeapSettings.ulBufferCnt_HD        = s_BufCnts[BTST_P_MEM1_BUFCNT_HD].ulBufCnt;
		pContext->astMemInfo[1].stVdcHeapSettings.ulBufferCnt_HD_Pip    = s_BufCnts[BTST_P_MEM1_BUFCNT_HD_PIP].ulBufCnt;
		pContext->astMemInfo[1].stVdcHeapSettings.ulBufferCnt_SD        = s_BufCnts[BTST_P_MEM1_BUFCNT_SD].ulBufCnt;
		pContext->astMemInfo[1].stVdcHeapSettings.ulBufferCnt_SD_Pip    = s_BufCnts[BTST_P_MEM1_BUFCNT_SD_PIP].ulBufCnt;
		pContext->astMemInfo[1].stVdcHeapSettings.ePixelFormat_2HD      = BTST_P_MEM_PIXFMT_2HD;
		pContext->astMemInfo[1].stVdcHeapSettings.ePixelFormat_HD       = BTST_P_MEM_PIXFMT_HD;
		pContext->astMemInfo[1].stVdcHeapSettings.ePixelFormat_SD       = BTST_P_MEM_PIXFMT_SD;
	}
	else
	{
		/* invalid memc1*/
		pContext->astMemInfo[1].hMem = NULL;
	}

	
	if((pContext->astMemInfo[2].hMem) && 
		(BTST_P_INVALID_BUFFER != (int)s_BufCnts[BTST_P_MEM2_BUFCNT_2HD].ulBufCnt))
	{
		pContext->astMemInfo[2].stVdcHeapSettings = pContext->stSettings.stHeapSettings;

		pContext->astMemInfo[2].stVdcHeapSettings.ulBufferCnt_2HD       = s_BufCnts[BTST_P_MEM2_BUFCNT_2HD].ulBufCnt;
		pContext->astMemInfo[2].stVdcHeapSettings.ulBufferCnt_2HD_Pip   = s_BufCnts[BTST_P_MEM2_BUFCNT_2HD_PIP].ulBufCnt;
		pContext->astMemInfo[2].stVdcHeapSettings.ulBufferCnt_HD        = s_BufCnts[BTST_P_MEM2_BUFCNT_HD].ulBufCnt;
		pContext->astMemInfo[2].stVdcHeapSettings.ulBufferCnt_HD_Pip    = s_BufCnts[BTST_P_MEM2_BUFCNT_HD_PIP].ulBufCnt;
		pContext->astMemInfo[2].stVdcHeapSettings.ulBufferCnt_SD        = s_BufCnts[BTST_P_MEM2_BUFCNT_SD].ulBufCnt;
		pContext->astMemInfo[2].stVdcHeapSettings.ulBufferCnt_SD_Pip    = s_BufCnts[BTST_P_MEM2_BUFCNT_SD_PIP].ulBufCnt;
		pContext->astMemInfo[2].stVdcHeapSettings.ePixelFormat_2HD      = BTST_P_MEM_PIXFMT_2HD;
		pContext->astMemInfo[2].stVdcHeapSettings.ePixelFormat_HD       = BTST_P_MEM_PIXFMT_HD;
		pContext->astMemInfo[2].stVdcHeapSettings.ePixelFormat_SD       = BTST_P_MEM_PIXFMT_SD;
	}
	else
	{
		/* invalid memc2*/
		pContext->astMemInfo[2].hMem = NULL;
	}
	pContext->stSettings.stHeapSettings = pContext->astMemInfo[s_CoreMemId[BTST_P_eVDC_HEAP].ulMemcId].stVdcHeapSettings;
}

static void BTST_FreeHeap
( const BFramework_Info   *pFrameworkInfo,
 BTST_P_Context          *pContext)
{
	BSTD_UNUSED(pFrameworkInfo);
	BSTD_UNUSED(pContext);
}


/***************************************************************************
* Main program (exposed API)
*
*/
void BTST_P_RegressionTests
( BTST_P_Context          *pContext )
{
	/* (1) : Close all opened handles */
	BTST_P_CleanUp(pContext);
	/* TODO: Add a lot more here */

	BTST_P_VfdAsSource_AutoTest(pContext);

	return;
}
/***************************************************************************
 *
 */
#if (BTST_P_MULTI_RTS)
static bool BTST_P_Is4Xcode
    ( BTST_P_Context          *pContext )
{
    /* Read RTS settings to find the number of hardware (ViCE) video encode channels */
    uint32_t ulRegValue = 0;
    bool bIs4Xcode = true;

#if BCHP_MEMC_ARB_2_REG_START /* 7445*/
#include "bchp_memc_arb_2.h"
    /* VFD0 on memc2/memc1 for quad/1u */
    ulRegValue = BREG_Read32(pContext->hReg, BCHP_MEMC_ARB_2_CLIENT_INFO_118);
    if (0x1DD == BCHP_GET_FIELD_DATA(ulRegValue, MEMC_ARB_2_CLIENT_INFO_118, BO_VAL))
        bIs4Xcode = true;
    else
        bIs4Xcode = false;    
#else
#include "bchp_memc_arb_0.h"
    /* Read RTS settings to find the number of hardware (ViCE) video encode channels */
    ulRegValue = BREG_Read32(pContext->hReg, BCHP_MEMC_ARB_0_CLIENT_INFO_18);
    if (0x7fff == BCHP_GET_FIELD_DATA(ulRegValue, MEMC_ARB_0_CLIENT_INFO_18, BO_VAL))
        bIs4Xcode = true;
    else
        bIs4Xcode = false;
#endif
	printf("\n 1u4t RTS? %s\n", bIs4Xcode? "true":"false");
    return (bIs4Xcode);
}
#endif

/***************************************************************************
* Main program (exposed API)
*
*/
void BTST_Interactive
( const BFramework_Info   *pFrameworkInfo,
 const BSystem_Info      *pSysInfo,
 bool                     bSharedMem,
 unsigned long            ulSharedMemSize,
 bool                     bIntercept,
 bool                     b422 )
{
	/* Application persistent data. */
	BTST_P_Context stContext, *pContext;
#if BTST_P_USE_MRC_MONITOR
	BMEM_MonitorInterface hMonitorInterface;
	BMRC_Monitor_Settings monitorSettings;
#endif
	BREG_Handle              hReg = pFrameworkInfo->hReg;
	BCHP_Handle              hChp = pFrameworkInfo->hChp;
	BMEM_Handle              hMem = pFrameworkInfo->hMem;
	BINT_Handle              hInt = pFrameworkInfo->hInt;
	bool                     bMem1 = false, bMem2 = false;
	bool                     b4xcodes = false;
	uint32_t i, j;

	g_pContext = pContext = &stContext;
	BDBG_ASSERT(hReg);
	BDBG_ASSERT(hChp);
	BDBG_ASSERT(hMem);
	BDBG_ASSERT(hInt);

	/* Clear out content of app data */
	BKNI_Memset((void*)pContext, 0x0, sizeof(BTST_P_Context));

#if (BTST_P_SUPPORT_HDR)
	{
		for(i = 0; i < 2; i++)
		{
			pContext->stHdDviPic[i].eCscMode                 = BAVC_CscMode_e709RgbFullRange;
			pContext->stHdDviPic[i].eColorSpace              = BAVC_Colorspace_eRGB;
			pContext->stHdDviPic[i].eMatrixCoefficients      = BAVC_MatrixCoefficients_eItu_R_BT_709;
			pContext->stHdDviPic[i].eTransferCharacteristics = BAVC_TransferCharacteristics_eItu_R_BT_709;
		}
	}
#endif

	/* (0) */
#if BTST_P_USE_MRC_MONITOR
	BMRC_Open(&pContext->hMrc, hReg, hInt, NULL);
	BMRC_Monitor_GetDefaultSettings(&monitorSettings);
	BMRC_GetMaxCheckers(pContext->hMrc, &monitorSettings.ulNumCheckersToUse);
	BMRC_Monitor_Open(&pContext->hMonitor, hReg, hInt, hChp, pContext->hMrc, 0, DRAM_SIZE, &monitorSettings); /* Control all pmem window */
	BMRC_Monitor_GetMemoryInterface(pContext->hMonitor, &hMonitorInterface);
	BMEM_InstallMonitor(hMem, &hMonitorInterface);
#endif

	/* Store system related pointer. */
	pContext->hReg = hReg;
	pContext->hChp = hChp;
	pContext->hTmr = pFrameworkInfo->hTmr;
	pContext->astMemInfo[0].hMem = hMem;
	pContext->hInt = hInt;
	pContext->bApplyImmediately = true;
	pContext->bEnableScaleDown  = false;
	pContext->bEnableMosaicMode = false;
	pContext->bEnableCaptureRul = false;
	pContext->bEnableMultiBufLog = false;
	pContext->hSourcePendingEvent = NULL;
	pContext->hSourcePendingTask = NULL;
	pContext->bManualResume = false;
	if ((pContext->bSharedMem = bSharedMem))
		pContext->ulSharedMemSize = ulSharedMemSize;
	for(j = 0; j < BTST_P_MAX_CMP; j++)
		for(i = 0; i < BTST_P_MAX_WINDOW; i++)
			pContext->abWinEnableBufLog[j][i] = true;

	pContext->bIntercept        = bIntercept;
	pContext->b422              = b422;

	pContext->eSelectedMDec = BFramework_NUM_XVD_CORES?BTST_XVD_0: BTST_MVD_0; /* always default to XVD_0 if it exists */

	pContext->eXptSrc = BTST_XPT_SRC_Streamer;

	/* Detect secondary memory */
	BCHP_GetFeature(pFrameworkInfo->hChp, BCHP_Feature_eMemCtrl1Capable, (void *)&bMem1);

	if (bMem1)
	{
		pContext->astMemInfo[1].hMem = pFrameworkInfo->hFrmWorkBoard->hMemc1;
	}

#if MEMC_2_MEM_PBASE
	BCHP_GetFeature(pFrameworkInfo->hChp, BCHP_Feature_eMemCtrl2Capable, (void *)&bMem2);

	if(bMem2)
	{
		/* 7400 only */
		pContext->astMemInfo[2].hMem = pFrameworkInfo->hFrmWorkBoard->hMemc2;
	}
#else
	BSTD_UNUSED(bMem2);
#endif

#if (MEMC_0_MEM_UBASE && (!MEMC_2_MEM_PBASE))
	/* 7422/25/35/45 only */
	pContext->astMemInfo[2].hMem = pFrameworkInfo->hFrmWorkBoard->hMemcU;
#endif 

#if (BTST_P_MULTI_RTS)
	b4xcodes = BTST_P_MULTI_RTS?BTST_P_Is4Xcode(pContext):false;

	for (i=0; i<BAVC_SourceId_eMax; i++)
		pContext->uSrcMemIdx[ i] = b4xcodes ? s_MemcIds[BTST_P_4U_eMPEG0_MEM + i].ulMemcId : s_MemcIds[BTST_P_eMPEG0_MEM + i].ulMemcId;

	for (i=0; i<BTST_P_MAX_CMP*BTST_P_MAX_WINDOW; i++)
		pContext->uWinMemIdx[ i] = b4xcodes ? s_WinMemId[BTST_P_4U_eCMP0_W0_MEM + i].ulMemcId : s_WinMemId[BTST_P_eCMP0_W0_MEM + i].ulMemcId;
#else
	BSTD_UNUSED(b4xcodes);
	for (i=0; i<BAVC_SourceId_eMax; i++)
	{
		pContext->uSrcMemIdx[ i] = s_MemcIds[BTST_P_eMPEG0_MEM + i].ulMemcId;
	}

	for (i=0; i<BTST_P_MAX_CMP*BTST_P_MAX_WINDOW; i++)
		pContext->uWinMemIdx[ i] = s_WinMemId[BTST_P_eCMP0_W0_MEM + i].ulMemcId;
#endif

	/* 7405 support both memc0 + memc1 and memc0 only mode */
	if(pContext->astMemInfo[s_CoreMemId[BTST_P_eXVD0_MEM].ulMemcId].hMem)
		pContext->astMDecInfo[BTST_XVD_0].hHeap  = pContext->astMemInfo[s_CoreMemId[BTST_P_eXVD0_MEM].ulMemcId].hMem;
	else
		pContext->astMDecInfo[BTST_XVD_0].hHeap  = pContext->astMemInfo[0].hMem;
	BDBG_ASSERT(pContext->astMDecInfo[BTST_XVD_0].hHeap);

#if (BFramework_NUM_XVD_CORES >1)
	pContext->astMDecInfo[BTST_XVD_1].hHeap  = pContext->astMemInfo[s_CoreMemId[BTST_P_eXVD1_MEM].ulMemcId].hMem;
	BDBG_ASSERT(pContext->astMDecInfo[BTST_XVD_1].hHeap);
#endif

#if (BTST_P_SUPPORT_VICE)
	BDBG_ASSERT(s_CoreMemId[BTST_P_eViCE0_MEM].ulMemcId!=-1);
	pContext->astMViCEInfo[0].hMem= pContext->astMemInfo[s_CoreMemId[BTST_P_eViCE0_MEM].ulMemcId].hMem;
	BDBG_ASSERT(pContext->astMViCEInfo[0].hMem);

	BDBG_ASSERT(s_CoreMemId[BTST_P_eViCE1_MEM].ulMemcId!=-1);
	pContext->astMViCEInfo[1].hMem= pContext->astMemInfo[s_CoreMemId[BTST_P_eViCE1_MEM].ulMemcId].hMem;
	BDBG_ASSERT(pContext->astMViCEInfo[1].hMem);
#endif

	BVDC_GetDefaultSettings(&pContext->stSettings);
	printf("=== bVecSwap= %d\n", pContext->stSettings.bVecSwap);

	/* (1) */
	BTST_SetupHeap(pFrameworkInfo, pSysInfo, pContext);

	/* (2) */
	BTST_P_InitInt(hInt);
	BINT_Stats_Enable(hInt, pFrameworkInfo->hTmr);

	/* (3) */
	BKNI_CreateEvent(&pContext->hTestDoneEvent);
	BKNI_ResetEvent(pContext->hTestDoneEvent);

	/* (4) */
	BRDC_Open(&pContext->hRdc, hChp, hReg, hMem, NULL);
	g_hRdc = pContext->hRdc;

#if defined(EMULATION)
	BDBG_SetModuleLevel("xpt_playback", BDBG_eMsg);
	BDBG_SetModuleLevel("xpt", BDBG_eMsg);
	BDBG_SetModuleLevel("BXVD", BDBG_eMsg);
	BDBG_SetModuleLevel("BXDM", BDBG_eMsg);
#endif

	/* (5) */
#if !BTST_VDC_ONLY
	InitXPT(pFrameworkInfo);

	/* (6) */
#if !defined(EMULATION) /* to save emulation startup time if xvd is not needed */
	BTST_OpenDecoder(pContext);
#endif

#if (BTST_P_SUPPORT_TUNER)
	frontend_init(pFrameworkInfo, pContext);
#endif
#endif

	/* (7) keystroke task should be spawned after xvd init done to avoid race condition; */
	BTST_CreateTask(&pContext->hTaskKeyStroke, BTST_P_KeyStrokeHandler, (void*)pContext);

	/* Running loop */
	while(BERR_TIMEOUT == BKNI_WaitForEvent(
		pContext->hTestDoneEvent, BTST_P_INFITITE_WAIT));

	/* [7] keystroke task should be spawned after xvd init done to avoid race condition; */
	BTST_DestroyTask(pContext->hTaskKeyStroke);

	/* [6] */
#if !BTST_VDC_ONLY

#if (BTST_P_SUPPORT_TUNER)
	frontend_exit();
#endif

#if !defined(EMULATION) /* to save emulation startup time if xvd is not needed */
	BTST_CloseDecoder(pContext);
#endif

	/* [5] */
	ShutdownXPT();
#endif

	/* [4] */
	BRDC_Close(pContext->hRdc);

	/* [3] */
	BKNI_DestroyEvent(pContext->hTestDoneEvent);
	TEST_CLOSE_HANDLE(BTST_DestroyTask, pContext->hTaskCaptureRul);
	/* TEST_CLOSE_HANDLE(BKNI_DestroyEvent, pContext->hCaptureRulEvent); */

	/* [2] */
	BINT_Stats_Disable(hInt);
	BTST_P_UnInitInt(hInt);

	/* [1] */
	BTST_FreeHeap(pFrameworkInfo, pContext);

	/* [0] */
#if BTST_P_USE_MRC_MONITOR
	BMRC_Monitor_Close(pContext->hMonitor);
	BMRC_Close(pContext->hMrc);
#endif

	BSTD_UNUSED(pSysInfo);
	return;
}

/* end of file */
