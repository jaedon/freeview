/***************************************************************************
*     Copyright (c) 2007-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: commands.h $
* $brcm_Revision: Hydra_Software_Devel/175 $
* $brcm_Date: 10/17/12 4:12p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/portinginterface/vdc/commands.h $
* 
* Hydra_Software_Devel/175   10/17/12 4:12p vanessah
* SW7445-60: add MRC monitor flag to speedup emulation
* 
* Hydra_Software_Devel/174   10/11/12 4:26p vanessah
* SW7425-2501: remove vdec input support
* 
* Hydra_Software_Devel/173   10/11/12 12:08p syang
* SW7346-572: add long-time-lock for vdc_test
* 
* Hydra_Software_Devel/172   10/9/12 3:58p syang
* SW7346-572: rm TV related (vdb, dynamic rts, ...); rm symbolic link to
* files in applications/bmetest/grc_test
* 
* Hydra_Software_Devel/171   10/9/12 3:41p vanessah
* SW7425-4046: add custom fmt stress test from 1080p to 30x30 with step
* size 30
* 
* Hydra_Software_Devel/170   10/8/12 3:29p syang
* SW7346-572: use BTST_P_ApplyChanges to get uniform error handling;
* avoid holding hVdcCfgLock too long
* 
* Hydra_Software_Devel/169   9/27/12 2:02p vanessah
* SW7425-2501: fix ulGfx2Mfd size error
* 
* Hydra_Software_Devel/168   9/26/12 3:23p nilesh
* SW7425-2568: Added TSM mode support for playback decode
* 
* Hydra_Software_Devel/167   9/26/12 11:47a vanessah
* SW7425-2501: add buffer configuration default due to nightly build test
* failure
* 
* Hydra_Software_Devel/166   9/25/12 5:36p vanessah
* SW7425-2501: vdc_test clean up
* 
* Hydra_Software_Devel/165   8/16/12 5:46p vanessah
* SW7425-2501: clean up unnecessary include file
* 
* Hydra_Software_Devel/164   8/16/12 3:02p vanessah
* SW7425-2571: revise mask function and remove old mbvn
* 
* Hydra_Software_Devel/163   7/31/12 11:39a vanessah
* SW7360-12: add stoprul toggle  to disable rul output
* 
* Hydra_Software_Devel/162   7/27/12 3:48p vanessah
* SW7425-3150: add video processing skip support
* 
* Hydra_Software_Devel/161   7/11/12 1:46p vanessah
* SW7425-3460: rate tracking test
* 
* Hydra_Software_Devel/160   7/5/12 11:33a vanessah
* SW7425-3432: add custom format stress test on transcode path
* 
* Hydra_Software_Devel/159   5/18/12 3:45p vanessah
* SW7425-3069: add resource leak change test
* 
* Hydra_Software_Devel/158   5/3/12 12:18p vanessah
* SW7425-2990: load different RTS for 7425 without reboot system
* 
* Hydra_Software_Devel/157   4/26/12 11:38a tdo
* SW7425-2571: Add VDC API to query BVN module error
*
* Hydra_Software_Devel/156   4/10/12 3:45p tdo
* SW7435-97: Format change on the SD path with progressive stream cause
* HD path to flicker
*
* Hydra_Software_Devel/155   3/29/12 4:29p pntruong
* SW7405-4296, SW7425-2526: Added some initial capabilities query at the
* vdc top level.
*
* Hydra_Software_Devel/154   3/5/12 9:43p vanessah
* SW7435-9: fix ViCE memory allocation problem
*
* Hydra_Software_Devel/153   3/5/12 9:26a yuxiaz
* SW7425-2526: Added tests for runtime capabilities query.
*
* Hydra_Software_Devel/152   2/29/12 12:46p tdo
* SW7425-2330: Add videolist cmd
*
* Hydra_Software_Devel/151   2/17/12 11:28a vanessah
* SW7425-2327: add Bar data handling test
*
* Hydra_Software_Devel/150   2/15/12 10:00a nilesh
* SW7425-2217: Merged Pid2Buffer Support
*
* Hydra_Software_Devel/SW7425-2217/1   1/26/12 5:29p nilesh
* SW7425-2217: Added PID2Buffer support
*
* Hydra_Software_Devel/149   2/13/12 11:33a vanessah
* SW7425-2219: add stop decode cmd upon jira testing
*
* Hydra_Software_Devel/148   2/9/12 4:50p yuxiaz
* SW7425-2181: Added test case in vdc_test
*
* Hydra_Software_Devel/147   1/20/12 6:41p vanessah
* SW7435-9: add STG2/3 support
*
* Hydra_Software_Devel/146   1/19/12 3:27p tdo
* SW7435-9: Bringup VDC_TEST_ONLY for 7435 emulation
*
* Hydra_Software_Devel/145   1/13/12 4:33p vanessah
* SW7425-1115: add unittest for MADR/MCVP seamless transactions
*
* Hydra_Software_Devel/144   12/14/11 10:58a yuxiaz
* SW7425-1875: Added test for 3d source buffer selection.
*
* Hydra_Software_Devel/143   12/14/11 10:47a nilesh
* SW7425-1110: Added CC Dump option toggle
*
* Hydra_Software_Devel/142   11/29/11 11:41a vanessah
* SW7425-1783: add unittest for stg picture id call back
*
* Hydra_Software_Devel/141   10/21/11 5:45p syang
* SW7425-1429: add test for Vsync callback and GetBuf_isr/ReturnBuf_isr
*
* Hydra_Software_Devel/140   10/10/11 1:12p vanessah
* SW7425-1398: adding dual transcoding test cases.
*
* Hydra_Software_Devel/139   9/30/11 5:16p darnstein
* SW7400-2665: merge from JIRA branch.
*
* Hydra_Software_Devel/SW7400-2665/2   9/30/11 5:09p darnstein
* SW7400-2665: merge from main branch.
*
* Hydra_Software_Devel/SW7400-2665/1   9/28/11 7:29p darnstein
* SW7400-2665: Before printing out VEC IT microcode identity, wait a
* while. This is necessary because there is a variable delay until BVDC
* loads new microcode into registers.
*
* Hydra_Software_Devel/137   9/16/11 1:21p yuxiaz
* SW7425-1283: Added 3d video stress test.
*
* Hydra_Software_Devel/136   8/25/11 10:45a tdo
* SW7420-2030: Add apply changes error tracking capability
*
* Hydra_Software_Devel/135   8/15/11 4:47p tdo
* SW7420-2017: Add test to scan all size for scaler enable error
*
* Hydra_Software_Devel/134   7/25/11 5:55p vanessah
* SW7425-465: add cmd to adjust minimum display and src fmt
*
* Hydra_Software_Devel/133   7/6/11 6:41p albertl
* SW7408-291, SW7425-745:: Added tests for vf filter settings and
* selective output mute.
*
* Hydra_Software_Devel/132   5/4/11 11:28a tdo
* SW7422-417: Add cmd to toggle 3d cap for decode channel open
*
* Hydra_Software_Devel/131   4/27/11 4:15p tdo
* SW7231-133, SW7422-417: Add support for multiple channel decode within
* a decoder with streamer input and playback  Add option for users to
* manually force SVD to only decode AVC, this is needed to do multi
* channel decode with SVD due to memory allocation issue.  Cleanup old
* code.
*
* Hydra_Software_Devel/130   4/15/11 1:53p tdo
* SW7422-417: Add MVC/SVC support to vdc_test
*
* Hydra_Software_Devel/129   4/8/11 1:47p vanessah
* SW7425-32: add a new command to sleep multiple seconds
*
* Hydra_Software_Devel/128   3/30/11 10:25a yuxiaz
* SW7400-2882: Added test for BVDC_Window_EnableBufLog
*
* Hydra_Software_Devel/127   2/9/11 4:26p yuxiaz
* SW7400-2882: Added multi-buffering log to vdc_test.
*
* Hydra_Software_Devel/126   1/26/11 3:17p yuxiaz
* SW7400-2998, SW7425-32: Only reduce cdb/itb buffer size for mosaic
* mode.
*
* Hydra_Software_Devel/125   12/21/10 10:57a yuxiaz
* SW7422-34: Added test for BVDC_Source_SetOrientation and
* BVDC_Source_GetOrientation.
*
* Hydra_Software_Devel/124   12/7/10 2:49p tdo
* SW7422-13: Add new command to set display 3D orientation
*
* Hydra_Software_Devel/123   12/3/10 3:24p tdo
* SW7422-13: Add new command to set 3D orientation to XVD
*
* Hydra_Software_Devel/122   12/3/10 12:24a hongtaoz
* SW7425-32: add multi-decode test feature to 7420/22/25 etc newer chips;
*
* Hydra_Software_Devel/121   11/24/10 12:55p pntruong
* SW7552-17: Added initial 7552 support.
*
* Hydra_Software_Devel/120   11/16/10 5:02p pntruong
* SW7231-16: Initial support for 7231.
*
* Hydra_Software_Devel/119   11/12/10 1:45p jessem
* SW7422-82: Added test for stereo 3D support for VFD as source.
*
* Hydra_Software_Devel/118   11/11/10 11:56p pntruong
* SW7425-31: Fixed 7125 build errors.
*
* Hydra_Software_Devel/117   11/11/10 11:35p pntruong
* SW7425-31: Fixed 3556 build errors.
*
* Hydra_Software_Devel/116   11/11/10 6:44p pntruong
* SW7425-31: Refactored hdm/lvds/hdr/dvo and added support hdmi input on
* stb chips.  Removed dead codes.
*
* Hydra_Software_Devel/115   11/5/10 2:44p yuxiaz
* SW7422-33, SW7422-77: Added 3D support in static buffer test.
*
* Hydra_Software_Devel/114   11/3/10 2:39p nilesh
* SW7425-38: Added TS Muxlib syslib support
*
* Hydra_Software_Devel/113   10/27/10 9:19a vanessah
* SW7425-46:  Digital resolution instead of the analog one
*
* Hydra_Software_Devel/112   10/21/10 11:19a nilesh
* SW7425-1: Added VCE Encoder Support (enable by compiling with
* SUPPORT_VCE=y)
*
* Hydra_Software_Devel/111   10/20/10 12:36p jessem
* SW7420-173: Added regression tests for VF as source feature.
*
* Hydra_Software_Devel/110   10/19/10 11:33a yuxiaz
* SW7422-39: Added test for BAVC_SourceId_eMpeg2 support in VDC.
*
* Hydra_Software_Devel/109   10/19/10 10:50a pntruong
* SW7425-16, SW3548-1650: Adapted to the removal of
* BXVD_ChannelSettings.bFGTEnable.  Also get rid of all reference to fgt
* in vdctest app.
*
* Hydra_Software_Devel/108   10/15/10 2:22p vanessah
* SW7358-15:  VDC_test compile w.r.t DMA
*
* Hydra_Software_Devel/107   10/12/10 12:01p vanessah
* SW7550-598:  Localize display setting
* Hydra_Software_Devel/106   10/12/10 10:54a vanessah
* SW7550-598:  Rework vdctest to specify display ouput
*
* Hydra_Software_Devel/105   10/11/10 1:11p jessem
* SW7420-173: Added support for VFD as source feature.
*
* Hydra_Software_Devel/104   10/7/10 5:04p vanessah
* SW7550-577:  vdc_test change on DAC settings
*
* Hydra_Software_Devel/103   10/4/10 11:33p hongtaoz
* SW7425-33, SW7425-29: added cmp3 test; added STG setting test; STG
* master should not set DACs;
*
* Hydra_Software_Devel/102   9/27/10 4:07p hongtaoz
* SW7425-32: added VDC_TEST_ONLY compile option;
*
* Hydra_Software_Devel/101   9/23/10 3:35p tdo
* SW7422-13: add 7422 support to vdc_test
*
* Hydra_Software_Devel/100   9/2/10 1:49p vanessah
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
* Hydra_Software_Devel/96   2/19/10 11:08a pntruong
* SW3548-2539: Add "regression" command to the vdc_test.
*
* Hydra_Software_Devel/95   2/2/10 11:13a pntruong
* SW3548-2678, SW3548-2680, SW3548-2681: Fixed build errors for chipset
* that does not have hdr.
*
* Hydra_Software_Devel/94   2/1/10 6:56p hongtaoz
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
* Hydra_Software_Devel/90   1/7/10 11:26a yuxiaz
* SW7405-3184: Added auto format stress test.
*
* Hydra_Software_Devel/89   12/22/09 5:17p tdo
* SW7401-3828: Add AFD support
*
* Hydra_Software_Devel/88   12/16/09 3:28p tdo
* SW7342-126: Add command to turn on/off DACs
*
* Hydra_Software_Devel/87   12/15/09 5:47p yuxiaz
* SW7408-13: Get vdc_test compile for 7408. Clean up compile flags.
*
* Hydra_Software_Devel/86   12/4/09 3:57p tdo
* SW7408-23: Add playback feature for digital source
*
* Hydra_Software_Devel/85   12/3/09 3:13p yuxiaz
* SW7405-3120: Added test for overlapping mosaic videos with z-ordering
*
* Hydra_Software_Devel/84   12/2/09 10:13a yuxiaz
* SW7420-462: Added tests for new tuning functions.
*
* Hydra_Software_Devel/83   11/23/09 3:59p rpan
* SW7468-22: Initial work for 7468.
*
* Hydra_Software_Devel/82   10/14/09 4:48p tdo
* SW7420-376: Bringup appframework for 7410
*
* Hydra_Software_Devel/81   10/14/09 3:56p yuxiaz
* SW7405-3186: Added source clip stress test.
*
* Hydra_Software_Devel/80   10/13/09 1:16p yuxiaz
* SW7405-3184: Added resize stress test for mosaic mode.
*
* Hydra_Software_Devel/79   9/14/09 4:23p hongtaoz
* SW3548-2471: added test case for the MAD allocation option in window
* settings;
*
* Hydra_Software_Devel/78   8/31/09 5:03p albertl
* SW7125-13: Added 7125 support.
*
* Hydra_Software_Devel/77   8/28/09 4:41p yuxiaz
* SW3556-672: Added test to get MAD dynamic default settings.
*
* Hydra_Software_Devel/76   8/4/09 1:01p darnstein
* PR55225: mechanical updates for 7342 chipset.
*
* Hydra_Software_Devel/75   7/31/09 6:48p syang
* PR 55812:  add 7550 support
*
* Hydra_Software_Devel/74   7/10/09 5:54p rpan
* PR55188: Convert scanf() to BTST_P_Scanf().
*
* Hydra_Software_Devel/73   6/22/09 4:35p darnstein
* PR55219: adapt to 7340 chipset. More to come.
*
* Hydra_Software_Devel/72   6/16/09 9:08p tdo
* PR55956: Add VDC support for 7420B0.
*
* Hydra_Software_Devel/71   6/8/09 3:37p yuxiaz
* PR54992: Added automated lipsync stress test.
*
* Hydra_Software_Devel/70   6/2/09 10:56a rpan
* PR54992: Added thread synchronization for graceful exit.
*
* Hydra_Software_Devel/69   5/22/09 11:21a tdo
* PR54768: Support dual decode streams of different compression type
*
* Hydra_Software_Devel/68   5/21/09 3:13p tdo
* PR54768: Fully support dual decode with same PID
*
* Hydra_Software_Devel/67   5/15/09 10:31a rpan
* PR55188: Added support for capturing and repeating comamnds.
*
* Hydra_Software_Devel/66   5/12/09 4:24p rpan
* PR54992: Add loop count to bvn stress test.
*
* Hydra_Software_Devel/65   4/7/09 10:05a tdo
* PR53373, PR53239: Add HDMI deep color support and remove usage of
* private definitions in vdc unittest
*
* Hydra_Software_Devel/64   4/6/09 5:01p hongtaoz
* PR53037, PR53038, PR53687, PR53688: monitor all BVN errors; used PCI L2
* interrupt instead of CPU interrupts to avoid confliction with VDC BVN
* error debug code;
*
* Hydra_Software_Devel/63   3/31/09 6:49p hongtaoz
* PR53037, PR53038, PR53687, PR53688: added random isr drop test to
* vdc_test;
*
* Hydra_Software_Devel/62   3/30/09 4:14p rpan
* PR53677: Added command for NTSC->PAL picture drop mode selection.
*
* Hydra_Software_Devel/61   3/30/09 3:22p hongtaoz
* PR53037, PR53038: added forced synclock mode and double-buffer support;
* added more VEC locking rates support;
*
* Hydra_Software_Devel/60   3/19/09 4:11p jessem
* PR 53388: Added more VDB tests.
*
* Hydra_Software_Devel/59   3/17/09 1:58p darnstein
* PR53265: Remove BVBI, BVBIlib software. It never worked.
*
* Hydra_Software_Devel/58   3/16/09 11:00p tdo
* PR45785, PR45789: Merge from MCVP branch
*
* Hydra_Software_Devel/7420_mcvp/4   3/16/09 5:37p darnstein
* PR53265: Remove BVBI private references.
*
* Hydra_Software_Devel/7420_mcvp/3   3/13/09 11:16a tdo
* PR45785, PR45789: merge from main branch on 3/13/09
*
* Hydra_Software_Devel/57   3/11/09 9:48p pntruong
* PR51930, PR52072, PR52364: Added "spread spectrum" - ss toggle command.
*
* Hydra_Software_Devel/7420_mcvp/2   3/10/09 9:09a tdo
* PR45785, PR45789: merge from main branch on 3/10/09
*
* Hydra_Software_Devel/56   3/9/09 4:41p rpan
* PR52203: Implemented some stress tests and debugging facility targeting
* VNET reconfiguration and multi-buffering.
*
* Hydra_Software_Devel/7420_mcvp/1   3/6/09 1:34p syang
* PR 45796: add gfd scl (coeff) mode test
*
* Hydra_Software_Devel/55   1/15/09 5:08p syang
* PR 50717: allow user to set delay to resume from src-pending
*
* Hydra_Software_Devel/54   12/17/08 7:45p pntruong
* PR50002: Added force source pending for api test.
*
* Hydra_Software_Devel/53   12/3/08 5:50p jessem
* PR 48992: Added Chroma histogram tests.
*
* Hydra_Software_Devel/52   11/13/08 5:39p darnstein
* PR48884: microcode dump command "udmp" is ready for evaluation.
*
* Hydra_Software_Devel/51   11/7/08 6:47p darnstein
* PR46484: Command ddmp dumps tables of register values from BVDC display
* software.
*
* Hydra_Software_Devel/50   10/24/08 5:44p albertl
* PR48199, PR48046: Removed BTST_P_EnableRdcCache because BMEM_SetCache
* is obsolete.
*
* Hydra_Software_Devel/49   10/24/08 3:44p albertl
* PR48023: Added BTST_P_DvoAttenuationRGB.  Removed bit shifting tweaks
* that are no longer necessary.  Help menu no longer invoked by wrong
* commands.
*
* Hydra_Software_Devel/48   10/22/08 10:44p nissen
* PR 46310: Added GRC and P3D tests.
*
* Hydra_Software_Devel/47   10/14/08 3:44p yuxiaz
* PR46305, PR46307, PR46309: Added unit test.
*
* Hydra_Software_Devel/46   9/15/08 5:55p pntruong
* PR45482: Adapt to bhdr.h and explicitly include bhdr_fe.h.
*
* Hydra_Software_Devel/45   8/5/08 8:12p pntruong
* PR39163: Added xvycc toggle.
*
* Hydra_Software_Devel/44   7/28/08 9:47a rpan
* PR36501: Save the work for parsing PSI messages.
*
* Hydra_Software_Devel/43   7/23/08 7:02p tdo
* PR43508, PR43509: Mapping individual bandgap adjustment for each DAC
*
* Hydra_Software_Devel/42   7/18/08 3:21p rpan
* PR36501: Hook up tuner and demodulator with xport.
*
* Hydra_Software_Devel/41   7/14/08 1:35p rpan
* PR36501: Check-in point for tuner work. Tuner input is not working yet.
*
* Hydra_Software_Devel/40   7/1/08 5:16p rpan
* PR36501: Initial work for adding tuner support fror 93549.
*
* Hydra_Software_Devel/39   6/19/08 2:36p rpan
* PR43712: Add SCART-SVideo test case.
*
* Hydra_Software_Devel/38   6/19/08 12:25p rpan
* PR43712: Add SCART test cases.
*
* Hydra_Software_Devel/37   6/18/08 1:29p jessem
* PR 39237, PR 43759: Added BVDB (Video Usage Modes Database) tests.
*
* Hydra_Software_Devel/36   6/18/08 12:10p syang
* PR35549:  add mutex to serialize access to vdc api
*
* Hydra_Software_Devel/35   6/17/08 2:29p rpan
* PR43668: Adopt the new LVD PI for 3548/3556.
*
* Hydra_Software_Devel/34   6/17/08 11:01a rpan
* PR43614: Added bandwidth equation parameter tests.
*
* Hydra_Software_Devel/33   6/11/08 8:30p pntruong
* PR39321: Use the correct handle to attached.
*
* Hydra_Software_Devel/32   6/7/08 6:12p pntruong
* PR39321: Adapt to new HDR api changes.
*
* Hydra_Software_Devel/31   6/7/08 5:38p jessem
* PR 39152, PR 39237, PR 35549: Revised enabling/disabling of source
* pending and handling of source pending event.
*
* Hydra_Software_Devel/30   6/7/08 3:46p jessem
* PR 39152: Added dynamic RTS test.
*
* Hydra_Software_Devel/29   6/7/08 3:11p rpan
* PR39337: Added compositor OSD setting test cases.
*
* Hydra_Software_Devel/28   5/27/08 7:22p hongtaoz
* PR42248, PR42494: added display alignment test;
*
* Hydra_Software_Devel/27   5/12/08 5:20p jessem
* PR 39237: Added hSourcePendingEvent and hSourcePendingTask to
* BTST_P_Context.
*
* Hydra_Software_Devel/26   4/30/08 5:30p pntruong
* PR35549: Added api test for source pending.
*
* Hydra_Software_Devel/25   4/8/08 10:54a tdo
* PR41488:  Add 3556 build support
*
* Hydra_Software_Devel/24   3/31/08 3:41p jessem
* PR 39335: Added CM3D tests.
*
* Hydra_Software_Devel/23   3/10/08 8:35p darnstein
* PR25971: first try at DCS for 7400
*
* Hydra_Software_Devel/22   2/28/08 5:00p jessem
* PR 38623: Added RDMA blockout support.
*
* Hydra_Software_Devel/21   2/21/08 3:49p rpan
* PR39807: Work for bcm3548 bringup.
*
* Hydra_Software_Devel/20   2/11/08 9:22p pntruong
* PR39177: [VEC] Need new display api to specify framedrop at runtime.
*
* Hydra_Software_Devel/19   1/24/08 5:27p tdo
* PR38710: Add blue stretch support
*
* Hydra_Software_Devel/18   12/10/07 4:03p pntruong
* PR38046: Added work-around by preventing HD_DVI_1 connecting to drain 1
* and 2.
*
* Hydra_Software_Devel/17   11/28/07 2:02p tdo
* PR37413: Provide new Api to set offset and size of histogram window and
* read corresponding parameters from specific window
*
* Hydra_Software_Devel/16   10/30/07 7:29p pntruong
* PR34239: Allow dynamically loading of vec custom timing.
*
* Hydra_Software_Devel/15   10/29/07 2:58p yuxiaz
* PR34540: Add support for 7325.
*
* Hydra_Software_Devel/14   10/25/07 3:31p yuxiaz
* PR29569, PR36290: Add FGT support on 7405.
*
* Hydra_Software_Devel/13   10/19/07 11:06a rpan
* PR34857: Fixed a window handle assignment issue.
*
* Hydra_Software_Devel/12   10/18/07 12:07a pntruong
* PR36137: [PC]:Set pc input video format without disable auto format
* detection.
*
* Hydra_Software_Devel/11   10/16/07 2:44p rpan
* PR34857: First version after merging the latest xvd_vdc_test and
* mvd_vdc_test as of Oct. 16, 2007, 12pm.
*
*
* Hydra_Software_Devel/1   9/21/07 1:55p rpan
* PR34857: Initial revision.
*
***************************************************************************/

#ifndef VDC_TEST_COMMANDS_H__
#define VDC_TEST_COMMANDS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "framework.h"
#include "bvdc.h"
#include "bvdc_dbg.h"
#include "bkni_multi.h"
#include "bmem_debug.h"
#include "bdbg.h"
#include "bint_plat.h"
#include "bint_dump.h"
#include "bint_stats.h"
#include "btst_kni.h"
#include "config.h"

#include "btmr.h"
#include "int1.h"
#include "platform.h"
#include "bvdc_test.h"

#include "bmrc_monitor.h"

#if (BTST_P_SUPPORT_GRC) /* m2mc */
#include "bgrc.h"
#endif
#include "bgfx_blit.h"

#if (BTST_P_SUPPORT_P3D)
#include "bp3d.h"
#endif

#if (FRAMEWORK_DO_I2C)
#include "bi2c.h"
#endif

#ifndef BTST_VDC_ONLY
#if (BFramework_NUM_XVD_CORES > 0)
#include "bxvd.h"
#endif

#if (BTST_P_SUPPORT_RFM_OUTPUT)
#include "brfm.h"
#endif

#if (BTST_P_SUPPORT_LVD)
#include "bpwm.h"
#include "blvd.h"
#endif

#if (BTST_P_SUPPORT_HDM)
#include "bhdm.h"
#include "bhdm_edid.h"
#endif

#if (BTST_P_SUPPORT_HDR)
#include "bhdr.h"
#include "bhdr_fe.h"
#endif

#if (BTST_P_SUPPORT_TUNER)
#include "btnr.h"
#include "btnr_dtt76800.h" /* Thomson tuner on 93549 board */
#include "bvsb.h"
#include "bvsb_3548.h"
#endif
#endif

#include <stdio.h>

	/***************************************************************************
	* Application Private Graphics Context
	*
	*/
	typedef enum
	{
		eGfxSurUserMode,
		eGfxSurIsrMode,
		eGfxSurPullMode,
		eGfxSurDoubleBuf
	} BTST_P_GfxSurSetting;

	typedef struct BTST_P_SrcContext
	{
		BVDC_Source_Handle         hVidSrc;
		bool                       bSrcCbEnable;

		/* for gfx sources */
		BSUR_Surface_Handle        hGfxSurface;
		BSUR_Surface_Handle        hIntrSurface;
		BSUR_Surface_Handle        hGoldSurface;

		/* for stereo 3D graphics */
		BSUR_Surface_Handle        hGfxSurface_R;
		BSUR_Surface_Handle        hIntrSurface_R;
		BSUR_Surface_Handle        hGoldSurface_R;

		BFMT_Orientation           eInOrientation;

		BTST_P_GfxSurSetting       eGfxSurSetting;
		BAVC_Gfx_Picture   *       pGfxBuf;
		uint32_t                   ulCallBckMsgCnt;

		/* for mpeg sources */
		uint16_t                   ui16VidPid;
		uint16_t                   ui16PcrPid;
		uint16_t                   ui16EnhPid;
	} TST_P_SrcContext;

	typedef struct BTST_P_WinContext
	{
		BVDC_Window_Handle         hWindow;
		int                        iVidSrcId;
		bool                       bLipSync;
		bool                       bGetBufIsr;
		bool                       bBoxDetect;
	} TST_P_WinContext;

	/***************************************************************************
	* Application Private Context
	*
	*/

	typedef void * BTST_MDEC_Handle;
	typedef void * BTST_MDEC_ChannelHandle;

	typedef struct _BTST_MDEC_Info
	{
		BTST_MDEC_Handle                hMDec;
		bool                            bSVD;
		BMEM_Handle                     hHeap;
		BTST_MDEC_ChannelHandle         ahMDecChn[BTST_P_MAX_CHANNELS];
		bool                            abChnDecoded[BTST_P_MAX_CHANNELS];
		bool                            abChnFrozen[BTST_P_MAX_CHANNELS];
		int                             aiXptChNum[BTST_P_MAX_CHANNELS];
		/* for mpeg sources */
		uint16_t                        aui16VidPid[BTST_P_MAX_CHANNELS];
		uint16_t                        aui16PcrPid[BTST_P_MAX_CHANNELS];
		uint16_t                        aui16EnhPid[BTST_P_MAX_CHANNELS];
		BAVC_VideoCompressionStd        aeVideoCmprStd[BTST_P_MAX_CHANNELS];

		struct
		{
		   struct
		   {
		      FILE *hFile;
		      bool bHeader;
		      bool bFilterNull;
		   } output608,output708;

#ifndef BTST_VDC_ONLY
		   BXVD_Userdata_Handle hXvdUserData;
#endif
		} userdata[BTST_P_MAX_CHANNELS];
	} BTST_MDEC_Info;

	typedef struct _BTST_MemInfo
	{
		BMEM_Handle                hMem;
		BVDC_Heap_Handle           hVdcHeap;
		BVDC_Heap_Settings         stVdcHeapSettings;
	} BTST_MemInfo;


	typedef enum _BTST_MDEC_Id
	{
		BTST_XVD_0 = 0,
		BTST_XVD_1,
		BTST_MVD_0 = MAX_XVD_CORES,
		BTST_MVD_1
	} BTST_MDEC_Id;


#if (BCHP_CHIP == 3548) || (BCHP_CHIP == 3556)

	/* 3548/3556 has only 3 DACs */
	typedef enum BTST_P_Output_Fmt
	{
		BTST_OUTPUT_YPrPb,
		BTST_OUTPUT_Composite,
		BTST_OUTPUT_PassThru,
		BTST_OUTPUT_SCART_CVBS,
		BTST_OUTPUT_SCART_YPrPb,
		BTST_OUTPUT_SCART_RGB,
		BTST_OUTPUT_SCART_SVideo
	} BTST_P_Output_Fmt;
#endif


	typedef enum BTST_P_Xpt_Src
	{
		BTST_XPT_SRC_Streamer,
		BTST_XPT_SRC_Demodulator
	} BTST_P_Xpt_Src;

#define CMD_RECORD_FILE "cmd_recorded.txt"

	typedef struct BTST_P_PlaybackStreamInfo
	{
		char                         fn[1024];
		uint16_t                     ui16VidPid;
		uint16_t                     ui16PcrPid;
		uint16_t                     ui16EnhPid;
		BAVC_StreamType              eStreamType;
		BAVC_VideoCompressionStd     eVideoCmprStd;

	} BTST_P_PlaybackStreamInfo;

	typedef struct BTST_P_LongTimeLock
	{
		BKNI_MutexHandle           hMutex;
		bool                       bLocked;
		const char                *fileName;  /* info of caller who has the lock */
		unsigned                   uiLine;
		unsigned                   uiMaxTrys;
	} BTST_P_LongTimeLock;
	
	typedef struct BTST_P_Context
	{
		/* KNI - singleton no handle needed */
		/* DBG - singleton no_handle needed */
		BREG_Handle                   hReg;
		BCHP_Handle                   hChp;
		BTST_MemInfo                  astMemInfo[BTST_P_MEMC_MAX];
		BTST_MemInfo                  astMViCEInfo[MAX_ViCE_CORES];
		BINT_Handle                   hInt;
		BRDC_Handle                   hRdc;
		BTMR_Handle                   hTmr;
#if (BTST_P_USE_MRC_MONITOR)
		BMRC_Handle                   hMrc;
		BMRC_Monitor_Handle           hMonitor;
#endif
		BVDC_Vdec_AwcSettings         stAwc;
		BVDC_Vdec_CtiSettings         stCti;
		BVDC_Vdec_DcrSettings         stDcr;
		BVDC_Vdec_CombSettings        stComb;
		BVDC_Vdec_AnalogGainSettings  stAGain;
		BVDC_Vdec_DigitalGainSettings stDGain;
		BVDC_Vdec_DelaySettings       stDelay;
		BVDC_Vdec_BackendSettings     stBackend;
		BVDC_Vdec_Settings            stVdecCfg;

		/* AFD code*/
		uint32_t                      ulAfd;
		/* System Configurations */

		/* This array contains info for XVD and MVD cores
		* Note: XVD info always start from element 0,
		*       MVD info always start from element MAX_MVD_CORES.
		*/
		BTST_MDEC_Info             astMDecInfo[MAX_XVD_CORES];

#if (BTST_P_SUPPORT_RFM_OUTPUT)
		BRFM_Handle                hRfm;
#endif

		/* Hdmi and related handles. */
#if (BTST_P_SUPPORT_LVD)
		BLVD_Handle                hLvd;
		BLVD_Settings              stLvdSettings;
		BPWM_Handle                hPwm;
		BPWM_ChannelHandle         hPwmChn;
#endif

#if (BTST_P_SUPPORT_HDM)
		BHDM_Handle                hHdm;
		BHDM_Settings              stHdmiSettings;
		BI2C_Handle                hI2c;
		BI2C_ChannelHandle         hI2cChn;
		BREG_I2C_Handle            hRegI2c;
#endif

#if (BTST_P_SUPPORT_HDR)
		BHDR_Handle                hHdr[2];
		BHDR_Settings              stHdrSettings[2];
		BAVC_VDC_HdDvi_Picture     stHdDviPic[2];
		BHDR_FE_Handle             hHdrFe[2];
		BHDR_FE_Settings           stHdrFeSettings[2];
		BHDR_FE_ChannelHandle      hHdrFeCh[2];
		BHDR_FE_ChannelSettings    stHdrFeChSettings[2];
#endif

#if BTST_P_SUPPORT_TUNER
		BTNR_Handle                hTuner;
		BVSB_Handle                hDemodulator; /* VSB/QAM demodulator */
#endif

#if (BTST_P_SUPPORT_GRC)
		BGRC_Handle                hGrc;
#endif

#if (BTST_P_SUPPORT_P3D)
		BP3D_Handle                hP3d;
		BP3D_Context               hP3dContext;
#endif

		BVDC_Handle                hVdc;
		BVDC_Settings              stSettings;
		void                      *pvCachedAddr;
		bool                       bEnableCache;

		/* Shared Memory using 1st memroy controller */
		bool                       bSharedMem;  /* only applies to hSharedMem */
		BMEM_Heap_Handle           hSharedMem;  /* shared Mem */
		BVDC_Heap_Handle           hSharedHeap; /* shared heap */
		BVDC_Heap_Settings         stSharedHeapSettings;
		void                      *pvSharedAddr;
		unsigned long              ulSharedMemSize;

		/* TODO: Consider dynamically allocated these handles.
		* Max window/compositor/source handle count can be query from
		* VDC. */
		BVDC_Compositor_Handle     ahCompositor[BTST_P_MAX_CMP];
		BVDC_Display_Handle        ahDisplay[BTST_P_MAX_CMP];
		BVDC_DisplayTg             eMasterTg[BTST_P_MAX_CMP];
		const BFMT_VideoInfo      *apFmtInfo[BTST_P_MAX_CMP];
		bool                       abHdmiCb[BTST_P_MAX_CMP];
		bool                       abStgCb[BTST_P_MAX_CMP];
		bool                       abRateCb[BTST_P_MAX_CMP];


		BVDC_Display_Settings      stDispSettings[BTST_P_MAX_CMP];

		TST_P_SrcContext           aSrcContext[BAVC_SourceId_eMax];
		TST_P_WinContext           aWinContext[BTST_P_MAX_CMP][BTST_P_MAX_WINDOW];
		uint32_t                   aulVsyncDelay[BTST_P_MAX_CMP][BTST_P_MAX_WINDOW];
		uint32_t                   aulSelectedMDec[BTST_P_MAX_CMP][BTST_P_MAX_WINDOW];
		uint32_t                   aulSelectedMDecCh[BTST_P_MAX_CMP][BTST_P_MAX_WINDOW];

		/* Video output Setting */
		bool                       bComposite [BTST_P_MAX_CMP];
		bool                       bComponent [BTST_P_MAX_CMP];
		bool                       bSVideo    [BTST_P_MAX_CMP];
		bool                       bVdec      [BTST_P_MAX_CMP];

#if (BTST_P_HACK_MFD_RTS)
		uint32_t                   aulMfdRts[MAX_XVD_CORES];
#endif

		/* Test Configurations */
		BKNI_EventHandle           hTestDoneEvent;
		BKNI_EventHandle           hCaptureRulEvent;
		BKNI_EventHandle           hMonitorBvnErrEvent;
		BKNI_EventHandle           hLipSyncEvent;

		BKNI_EventHandle           hMTPTWatchdogEvent[MAX_XVD_CORES];
		BKNI_EventHandle           hMTPMVWatchdogEvent[MAX_XVD_CORES];
		BTST_TaskHandle            hTaskKeyStroke;
		BTST_TaskHandle            hTaskCaptureRul;
		BTST_TaskHandle            hTaskMonitorBvnErr;
		BTST_TaskHandle            hTaskLipSync;
		BTST_TaskHandle            hTaskScaleDest;

		BTST_TaskHandle            hTaskMTPTTask[MAX_XVD_CORES];
		BTST_TaskHandle            hTaskMTPMVTask[MAX_XVD_CORES];
		BKNI_MutexHandle           hMVDMutex[MAX_XVD_CORES];
		BTST_P_LongTimeLock        stVdcCfgLock;
		uint32_t                   iSelectedComp;
		uint32_t                   iSelectedWin;
		uint32_t                   iSelectedVidSrc; /* and decoder chn */
		uint32_t                   iSelectedHfe;
		BTST_MDEC_Id               eSelectedMDec;
		uint32_t                   iSelectedMDecCh;
		uint32_t                   iSelectedMDecDm;
		uint32_t                   iSelectedMem;
		bool                       bPlayback;
		bool                       bTSMMode;
		bool                       bDecodeChan3d;
		bool                       bCCDump;
		uint8_t                    uSrcMemIdx[BAVC_SourceId_eMax];
		uint8_t                    uWinMemIdx[BTST_P_MAX_CMP *BTST_P_MAX_WINDOW];
		struct
		{
		   uint16_t uiInputPID;
		   uint16_t uiOutputPID;

		   unsigned uiPIDChannel;
		   void *hRaveCtx;
		} pid2buf[8];
		unsigned uiNumValidPid2Buf;

		bool                       abSwapMainPip[BTST_P_MAX_CMP];
		bool                       bApplyImmediately;
		bool                       bApplyError;
		bool                       bEnableScaleDown;
		bool                       bEnableMosaicMode;
		bool                       bEnableUserSetting;
		bool                       bStatic[BTST_P_MAX_MFD];
		uint32_t                   ulGfx2Mfd[BTST_P_MAX_MFD];
		bool                       bIntercept;
		bool                       b422;
		bool                       bEnableCaptureRul;
		bool                       bEnableMultiBufLog;
		bool                       abWinEnableBufLog[BTST_P_MAX_CMP][BTST_P_MAX_WINDOW];
		bool                       bEnableCapturePicture;
		BVDC_AdcInput              aeAdcInput[4];
		bool                       bEnableCheckBvnErr;
		bool                       bEnableMonitorBvnErr;
		bool                       bScaleDestEnabled;
		bool                       bEnableLipSync;
		bool                       bVsyncDelay;
		uint32_t                   ulBvnfErrMask;
		uint32_t                   ulBvnmErrMask;
		uint32_t                   ulBvnm1ErrMask;
		uint32_t                   ulBvnbErrMask;
		int                        bStopRul;
		int                        bStopVecTrigger;
		uint32_t                   ulStressBvnCnt;
		BPXL_Format                eGraphicsFormat;

		/* loop control */
		bool                       bRecordCmd;
		bool                       bRepeatCmd;
		int                        iLoopCnt;
		FILE                      *fp_cmd;
		int                        iNumCmds;
		int                        iNumCmdsXed;

		bool                       bCustomDisplay;

		/* Due to limitation of passing param to command. */
		BFMT_VideoFmt             *peNewFormat;

		/* For user capture */
		BVDC_Window_CapturedImage  pic;
		BVDC_Source_Handle         hUserCaptureGfxSource;
		BVDC_Window_Handle         hUserCaptureGfxWin;
		bool                       bShowCapPic;

		bool                       bScaleDown;

		/* stream info */
		BAVC_StreamType            eStreamType ;
		BAVC_VideoCompressionStd   eVideoCmprStd;

		/* MosaicMode test */
		BAVC_XVD_Picture           astMosaicPictures[MAX_XVD_CORES][BAVC_MOSAIC_MAX];
		bool                       abChanOpened[MAX_XVD_CORES][BAVC_MOSAIC_MAX];

		uint32_t                   aulMosaicCount[BTST_P_MAX_CMP][BTST_P_MAX_WINDOW];
		BVDC_Rect                  astRect[BTST_P_MAX_CMP][BTST_P_MAX_WINDOW][BAVC_MOSAIC_MAX];

		/* RDC BLock out */
		bool                       bRdcBlockOutInit;
		bool                       bRdcBlockOutSet;

		/* Source Pending */
		int32_t                    lMilliSeconds; /* resume delay after src active */
		BKNI_EventHandle           hSourcePendingEvent;
		BTST_TaskHandle            hSourcePendingTask;
		bool                       bManualResume;

		BTST_P_Xpt_Src             eXptSrc;

		BVDC_Capabilities          stCapabilities;

		bool                       bForceSyncLock;

		bool                       bDeinterlaceAllocFull;
		bool                       bShrinkWidthForMad;
		const BFMT_VideoInfo             *pMinSrcFmt;
		const BFMT_VideoInfo             *pMinDspFmt;
		bool                       bBypassVideoProcessings;

		uint32_t                   ulRandIsrDropCnt;
		uint32_t                   ulMinIsrDropCnt;

		uint32_t                   ulMpegSrcFmtChangeCnt;

		bool                       abMasterStg[BTST_P_MAX_CMP];

		bool                       bVfdAsSourceAutoTest;
		bool                       bResourceLeakAutoTest;
		BFMT_Orientation           eSrcOrientation;

#if (BTST_P_SUPPORT_VCE)
		void * pEncoderInfo;
#endif
#if (BTST_P_SUPPORT_MUX)
		void * pMuxerInfo;
#endif
	} BTST_P_Context;

#if (BTST_P_SUPPORT_VCE)
#include "vce_encoder.h"
#endif
#if (BTST_P_SUPPORT_MUX)
#include "muxer.h"
#endif

	/*
	* Command handler function prototypes
	*/

	/*
	* Handle selection function prototypes
	*/
	void BTST_P_Cmp0Select( BTST_P_Context *pContext );
	void BTST_P_Cmp1Select( BTST_P_Context *pContext );
	void BTST_P_Cmp2Select( BTST_P_Context *pContext );
	void BTST_P_Cmp3Select( BTST_P_Context *pContext );
	void BTST_P_Cmp4Select( BTST_P_Context *pContext );
	void BTST_P_Cmp5Select( BTST_P_Context *pContext );
	void BTST_P_Mpeg0Select( BTST_P_Context *pContext );
	void BTST_P_Mpeg1Select( BTST_P_Context *pContext );
	void BTST_P_Mpeg2Select( BTST_P_Context *pContext );
	void BTST_P_Mpeg3Select( BTST_P_Context *pContext );
	void BTST_P_Vfd0Select( BTST_P_Context *pContext );
	void BTST_P_Vfd1Select( BTST_P_Context *pContext );
	void BTST_P_Vfd2Select( BTST_P_Context *pContext );
	void BTST_P_Vfd3Select( BTST_P_Context *pContext );
	void BTST_P_Vfd4Select( BTST_P_Context *pContext );
	void BTST_P_Vfd5Select( BTST_P_Context *pContext );
	void BTST_P_Analog0Select( BTST_P_Context *pContext );
	void BTST_P_Analog1Select( BTST_P_Context *pContext );
	void BTST_P_656in0Select( BTST_P_Context *pContext );
	void BTST_P_656in1Select( BTST_P_Context *pContext );
	void BTST_P_Hddvi0Select( BTST_P_Context *pContext );
	void BTST_P_Hddvi1Select( BTST_P_Context *pContext );
	void BTST_P_HdrFe0Select( BTST_P_Context *pContext );
	void BTST_P_HdrFe1Select( BTST_P_Context *pContext );
	void BTST_P_Ds0Select( BTST_P_Context *pContext );
	void BTST_P_Gfx0Select( BTST_P_Context *pContext );
	void BTST_P_Gfx1Select( BTST_P_Context *pContext );
	void BTST_P_Gfx2Select( BTST_P_Context *pContext );
	void BTST_P_Gfx3Select( BTST_P_Context *pContext );
	void BTST_P_Gfx4Select( BTST_P_Context * pContext);
	void BTST_P_Gfx5Select( BTST_P_Context *pContext );
	void BTST_P_Win0Select( BTST_P_Context *pContext );
	void BTST_P_Win1Select( BTST_P_Context *pContext );
	void BTST_P_Win2Select( BTST_P_Context *pContext );
	void BTST_P_Win3Select( BTST_P_Context *pContext );
	void BTST_P_Win4Select( BTST_P_Context *pContext );
	void BTST_P_Xvd0Select( BTST_P_Context *pContext );
	void BTST_P_Xvd1Select( BTST_P_Context *pContext );
	void BTST_P_Mvd0Select( BTST_P_Context *pContext );
	void BTST_P_MpgChannelSelect( BTST_P_Context *pContext );
	void BTST_P_MpgDmSelect( BTST_P_Context *pContext );
	void BTST_P_Mem0Select( BTST_P_Context		  *pContext );
	void BTST_P_Mem1Select( BTST_P_Context		  *pContext );
	void BTST_P_Mem3Select( BTST_P_Context		  *pContext );
	void BTST_P_SharedMemSelect( BTST_P_Context		  *pContext );

	/*
	* Open/Close function prototypes
	*/
	void BTST_P_OpenVdcMisc( BTST_P_Context *pContext );
	void BTST_P_CloseVdcMisc( BTST_P_Context *pContext );
	void BTST_P_OpenComp( BTST_P_Context *pContext );
	void BTST_P_CloseComp( BTST_P_Context *pContext );
	void BTST_P_OpenDisp( BTST_P_Context *pContext );
	void BTST_P_CloseDisp( BTST_P_Context *pContext );
	void BTST_P_OpenGfxWin( BTST_P_Context *pContext );
	void BTST_P_GfxWinTest( BTST_P_Context *pContext );
	void BTST_P_GfxSclModeTest( BTST_P_Context *pContext );
	void BTST_P_CloseGfxWin( BTST_P_Context *pContext );
	void BTST_P_ChgGfxSurSettingMode( BTST_P_Context *pContext );
	void BTST_P_ChgVfdSurSettingMode( BTST_P_Context *pContext );
	void BTST_P_OpenVidSrc( BTST_P_Context *pContext );
	void BTST_P_CloseVidSrc( BTST_P_Context *pContext );
	void BTST_P_OpenVidWin( BTST_P_Context *pContext );
	void BTST_P_CloseVidWin( BTST_P_Context *pContext );
	void BTST_P_OpenRfm( BTST_P_Context *pContext );
	void BTST_P_CloseRfm( BTST_P_Context *pContext );
	void BTST_P_OpenHdmi( BTST_P_Context *pContext );
	void BTST_P_CloseHdmi( BTST_P_Context *pContext );
#if (BTST_P_SUPPORT_HDR)
	void BTST_P_OpenHdrFe( BTST_P_Context *pContext );
	void BTST_P_CloseHdrFe( BTST_P_Context *pContext );
#endif
	void BTST_P_OpenMpgChannel( BTST_P_Context *pContext );
	void BTST_P_CloseMpgChannel( BTST_P_Context *pContext );
#if (BTST_P_SUPPORT_GRC)
	void BTST_P_OpenGrc( BTST_P_Context *pContext );
	void BTST_P_CloseGrc( BTST_P_Context *pContext );
	void BTST_P_GrcCompressDCE( BTST_P_Context *pContext );
#endif
#if (BTST_P_SUPPORT_P3D)
	void BTST_P_OpenP3d( BTST_P_Context *pContext );
	void BTST_P_CloseP3d( BTST_P_Context *pContext );
	void BTST_P_P3dRenderTest( BTST_P_Context *pContext );
#endif
	void BTST_P_GetCap (BTST_P_Context  *pContext);
	/*
	* Display/Compositor function prototypes
	*/
	void BTST_P_PrintRfmStatus( BTST_P_Context *pContext );
	void BTST_P_ChgDispFmt( BTST_P_Context *pContext );
	void BTST_P_ChgDispFmt_Interactive( BTST_P_Context *pContext );
	void BTST_P_ChgDispArc( BTST_P_Context *pContext );
	void BTST_P_ChgDispTimebase( BTST_P_Context *pContext );
	void BTST_P_ChgSrcFmt( BTST_P_Context *pContext );
	void BTST_P_ChgAutoSrcFmt( BTST_P_Context *pContext );
	void BTST_P_RgbOutput( BTST_P_Context *pContext );
	void BTST_P_HdmiRgbOutput( BTST_P_Context *pContext );
	void BTST_P_CvbsOutput( BTST_P_Context *pContext );
	void BTST_P_ToggleHsync( BTST_P_Context *pContext );
	void BTST_P_ChgBgColor( BTST_P_Context *pContext );
	void BTST_P_ToggleXvYcc( BTST_P_Context *pContext );
	void BTST_P_ToggleSS( BTST_P_Context *pContext );
	void BTST_P_ToggleStgOut( BTST_P_Context *pContext );
	void BTST_P_ToggleStgCb ( BTST_P_Context *pContext );
	void BTST_P_ToggleRateCb( BTST_P_Context *pContext );
	void BTST_P_Toggle656Out( BTST_P_Context *pContext );
	void BTST_P_ToggleRfmOut( BTST_P_Context *pContext );
	void BTST_P_ToggleHdmiOut( BTST_P_Context *pContext );
	void BTST_P_ToggleFrameDrop( BTST_P_Context *pContext );
	void BTST_P_ToggleHdmiCb( BTST_P_Context *pContext );
	void BTST_P_ToggleHdmiSync( BTST_P_Context *pContext );
	void BTST_P_MuteOutput( BTST_P_Context *pContext );
	void BTST_P_SetMacrovision( BTST_P_Context *pContext );
	void BTST_P_SetDcs( BTST_P_Context *pContext );
	void BTST_P_SetDac( BTST_P_Context *pContext );
	void BTST_P_SetMpaaDeci( BTST_P_Context *pContext );
	void BTST_P_SetHdmiDropLines( BTST_P_Context *pContext );
	void BTST_P_SetHdmiDeepColor( BTST_P_Context *pContext );
	void BTST_P_CustomDisplay( BTST_P_Context *pContext);
	void BTST_P_SetLCDDimCtrl( BTST_P_Context *pContext );
	void BTST_P_LoadMicrocode( BTST_P_Context *pContext );
	void BTST_P_SetOsdConfig( BTST_P_Context *pContext );
	void BTST_P_GetOsdConfig( BTST_P_Context *pContext );
	void BTST_P_OutputFmt( BTST_P_Context *pContext );
	void BTST_P_ChangeDispVideo (BTST_P_Context  *pContext);
	void BTST_P_DisplayGetCap (BTST_P_Context  *pContext);
	void BTST_P_CompGetCap (BTST_P_Context  *pContext);


	/*
	* Window function prototypes
	*/

	void BTST_P_ResizeWin( BTST_P_Context *pContext );
	void BTST_P_Sweep( BTST_P_Context *pContext );
	void BTST_P_PictureByPicture( BTST_P_Context *pContext );
	void BTST_P_OverScan( BTST_P_Context *pContext );
	void BTST_P_DstClipWin( BTST_P_Context *pContext );
	void BTST_P_SclOutCutWin( BTST_P_Context *pContext );
	void BTST_P_DstCutMove( BTST_P_Context *pContext );
	void BTST_P_AspRatRectClip( BTST_P_Context *pContext );
	void BTST_P_ChgSclFctrRndToler( BTST_P_Context *pContext );
	void BTST_P_ClipSource( BTST_P_Context *pContext );
	void BTST_P_ClipSourceStress( BTST_P_Context *pContext );
	void BTST_P_ColorClip( BTST_P_Context *pContext );
	void BTST_P_LoadCabTable( BTST_P_Context *pContext );
	void BTST_P_LoadLabTable( BTST_P_Context *pContext );
	void BTST_P_ContrastStretch( BTST_P_Context *pContext );
	void BTST_P_BlueStretch( BTST_P_Context *pContext );
	void BTST_P_SetCabParams( BTST_P_Context *pContext );
	void BTST_P_ToggleAllPep( BTST_P_Context *pContext );
	void BTST_P_WinSplitScreen( BTST_P_Context *pContext );
	void BTST_P_SrcSplitScreen( BTST_P_Context *pContext );
	void BTST_P_ShowHideWin( BTST_P_Context *pContext );
	void BTST_P_SetAlpha( BTST_P_Context *pContext );
	void BTST_P_PhaseAdjust( BTST_P_Context *pContext );
	void BTST_P_ToggleLipSync( BTST_P_Context *pContext );
	void BTST_P_ToggleGetBufIsr( BTST_P_Context *pContext );
	void BTST_P_ToggleMaster( BTST_P_Context *pContext );
	void BTST_P_ChgMad( BTST_P_Context *pContext );
	void BTST_P_ToggleMadOsd( BTST_P_Context *pContext );
	void BTST_P_StressMad( BTST_P_Context *pContext );
	void BTST_P_StressAnrMad( BTST_P_Context *pContext );
	void BTST_P_StressMadGame( BTST_P_Context *pContext );
	void BTST_P_SetAfd( BTST_P_Context *pContext );
	void BTST_P_ToggleLBox( BTST_P_Context *pContext );
	void BTST_P_MpegDecode( BTST_P_Context *pContext );
	void BTST_P_StopMpegDecode( BTST_P_Context *pContext );
	void BTST_P_MpegDecode3d( BTST_P_Context *pContext );
	void BTST_P_Toggle3dDecodeChan( BTST_P_Context *pContext );
	void BTST_P_TogglePlayback( BTST_P_Context *pContext );
	void BTST_P_ToggleCCDump( BTST_P_Context *pContext );
	void BTST_P_EnablePID2Buffer( BTST_P_Context *pContext );
	void BTST_P_ChgArcMode( BTST_P_Context *pContext );
	void BTST_P_ChgChnChgMode( BTST_P_Context *pContext );
	void BTST_P_Toggle3dComb( BTST_P_Context *pContext );
	void BTST_P_TogglePanScan( BTST_P_Context *pContext );
	void BTST_P_TestUserPanScan( BTST_P_Context *pContext );
	void BTST_P_RandomMove( BTST_P_Context *pContext );
	void BTST_P_ToggleRgbMatch( BTST_P_Context *pContext );
	void BTST_P_Contrast( BTST_P_Context *pContext );
	void BTST_P_Saturation( BTST_P_Context *pContext );
	void BTST_P_Hue( BTST_P_Context *pContext );
	void BTST_P_Brightness( BTST_P_Context *pContext );
	void BTST_P_Sharpness( BTST_P_Context *pContext );
	void BTST_P_SetTune( BTST_P_Context *pContext );
	void BTST_P_GetTune( BTST_P_Context *pContext );
	void BTST_P_ResetTune( BTST_P_Context *pContext );
	void BTST_P_ScaleMode( BTST_P_Context *pContext );
	void BTST_P_DelayOffset( BTST_P_Context *pContext );
	void BTST_P_DnrFilter( BTST_P_Context *pContext );
	void BTST_P_MuteMode( BTST_P_Context *pContext );
	void BTST_P_SetMosaicMode( BTST_P_Context *pContext );
	void BTST_P_SetMosaicZorder( BTST_P_Context *pContext );
	void BTST_P_MosaicModeResize( BTST_P_Context *pContext );
	void BTST_P_SetMosaicTracking( BTST_P_Context *pContext );
	void BTST_P_MosaicMove( BTST_P_Context *pContext );
	void BTST_P_Zoom( BTST_P_Context *pContext );
#if (BFramework_NUM_XVD_CORES > 0)
	void BTST_P_ToggleXvdFreeze( BTST_P_Context *pContext );
	void BTST_P_SetXvdRefreshRate( BTST_P_Context *pContext );
	void BTST_P_SetXvd1080pMode( BTST_P_Context *pContext );
#endif
	void BTST_P_AttenuationRGB( BTST_P_Context *pContext );
	void BTST_P_DvoAttenuationRGB( BTST_P_Context *pContext );
	void BTST_P_ColorTemp( BTST_P_Context *pContext );
	void BTST_P_WhiteBalance( BTST_P_Context  *pContext );
	void BTST_P_SetCmsParams( BTST_P_Context  *pContext );
	void BTST_P_GetLumaStatus( BTST_P_Context *pContext );
	void BTST_P_SetLumaStatConfig( BTST_P_Context *pContext );
	void BTST_P_GetChromaStatus( BTST_P_Context *pContext );
	void BTST_P_SetChromaStatConfig( BTST_P_Context *pContext );
	void BTST_P_SetForceCapture( BTST_P_Context	 *pContext);
	void BTST_P_SetLumaKey( BTST_P_Context	*pContext);
	void BTST_P_GetLumaKey( BTST_P_Context  *pContext);
	void BTST_P_ChgFixColor( BTST_P_Context	*pContext );
	void BTST_P_ChgSrcCsc( BTST_P_Context *pContext );
	void BTST_P_ChgDvoCsc( BTST_P_Context *pContext );
	void BTST_P_ChgWinCsc( BTST_P_Context *pContext );
	void BTST_P_ChgVfFilter( BTST_P_Context *pContext );
	void BTST_P_SetGameDelay( BTST_P_Context *pContext );
	void BTST_P_ToggleAnr( BTST_P_Context  *pContext );
	void BTST_P_PcInSyncConfig( BTST_P_Context  *pContext );
	void BTST_P_PcInSyncStatus( BTST_P_Context  *pContext );
	void BTST_P_GetSrcStatus( BTST_P_Context  *pContext );
	void BTST_P_CustomVdec( BTST_P_Context  *pContext );
	void BTST_P_HVStart( BTST_P_Context	*pContext );
	void BTST_P_HdDviConfig( BTST_P_Context  *pContext );
	void BTST_P_TogglePsF( BTST_P_Context *pContext );
	void BTST_P_GetBwParams( BTST_P_Context *pContext );
	void BTST_P_SetBwParams( BTST_P_Context *pContext );
	void BTST_P_ChgCompression( BTST_P_Context *pContext );
	void BTST_P_ForceSyncLock( BTST_P_Context *pContext );
	void BTST_P_GetMadDynamicDefault( BTST_P_Context *pContext );
	void BTST_P_MadAllocFull( BTST_P_Context *pContext );
	void BTST_P_MadShrinkWidth( BTST_P_Context *pContext );
	void BTST_P_SetSclConfig( BTST_P_Context          *pContext );
	void BTST_P_SetMinDspFmt(BTST_P_Context * pContext);
	void BTST_P_SetMinSrcFmt(BTST_P_Context * pContext);
	void BTST_P_ToggleByPassVideoProcessing(BTST_P_Context * pContext);
	void BTST_P_WindowGetCap(BTST_P_Context * pContext);



	/*
	* Source function prototypes
	*/
	void BTST_P_ReMapDetectFmt( BTST_P_Context  *pContext );
	void BTST_P_SourcePending( BTST_P_Context  *pContext );
	void BTST_P_SourceResume( BTST_P_Context  *pContext );
	void BTST_P_ForcePending( BTST_P_Context  *pContext );
	void BTST_P_Source3D( BTST_P_Context  *pContext );
	void BTST_P_SourceGetCap( BTST_P_Context  *pContext );


	/*
	* Miscellaneous function prototypes
	*/
	void BTST_P_Stub( BTST_P_Context  *pContext );
	void BTST_P_VideoList( BTST_P_Context  *pContext );
	void BTST_P_Cat( BTST_P_Context  *pContext );
	void BTST_P_ChipFeature( BTST_P_Context	*pContext );
	void BTST_P_VdcDbgDump( BTST_P_Context  *pContext );
#ifdef BVDC_P_DISPLAY_DUMP
	void BTST_P_DispTblDump( BTST_P_Context  *pContext );
#endif
	void BTST_P_DispUcodeDump( BTST_P_Context  *pContext );
	void BTST_P_UseBuffer( BTST_P_Context  *pContext );
	void BTST_P_UseStaticBuffer( BTST_P_Context  *pContext );
	void BTST_P_GraphicsFormat( BTST_P_Context  *pContext );
	void BTST_P_UseGfxForMfd( BTST_P_Context  *pContext );
	void BTST_P_InterceptFrame( BTST_P_Context  *pContext );
	void BTST_P_SwapWindows( BTST_P_Context  *pContext );
	void BTST_P_MirModeChnChg( BTST_P_Context  *pContext );
	void BTST_P_MpgDisplayMode( BTST_P_Context  *pContext );
	void BTST_P_FormatChange( BTST_P_Context  *pContext );
	void BTST_P_ChannelChange( BTST_P_Context  *pContext );
	void BTST_P_PR14134( BTST_P_Context  *pContext );
	void BTST_P_StressTest( BTST_P_Context  *pContext );
	void BTST_P_LineCntTest( BTST_P_Context  *pContext );
	void BTST_P_DbgLevel( BTST_P_Context  *pContext );
	void BTST_P_GlobalLevel( BTST_P_Context  *pContext );
	void BTST_P_ManualApplyChanges( BTST_P_Context  *pContext );
	void BTST_P_AbortChanges( BTST_P_Context  *pContext );
	void BTST_P_ToggleApplyImm( BTST_P_Context  *pContext );
	void BTST_P_ToggleApplyErr( BTST_P_Context  *pContext );
	void BTST_P_MemMonitor( BTST_P_Context  *pContext );
	void BTST_P_ToggleSclDown( BTST_P_Context  *pContext );
	void BTST_P_ChangeVdcSettings( BTST_P_Context  *pContext );
	void BTST_P_ChangeVdcHeapSettings( BTST_P_Context  *pContext );
	void BTST_P_ChangeVdcBandGapSettings( BTST_P_Context  *pContext );
	void BTST_P_CleanUp( BTST_P_Context  *pContext );
	void BTST_P_GetCbStats( BTST_P_Context  *pContext );
	void BTST_P_ResetCbStats( BTST_P_Context  *pContext );
	void BTST_P_AddCbBin( BTST_P_Context  *pContext );
	void BTST_P_CaptureRul( BTST_P_Context  *pContext );
	void BTST_P_CapturePicture( BTST_P_Context  *pContext );
	void BTST_P_DetectMpegInputTest( BTST_P_Context  *pContext );
	void BTST_P_ToggleSrcCb( BTST_P_Context  *pContext );
	void BTST_P_TogglePicList( BTST_P_Context  *pContext );
	void BTST_P_ToggleStrmClip( BTST_P_Context  *pContext );
	void BTST_P_WorstPicList( BTST_P_Context  *pContext );
	void BTST_P_Sleep( BTST_P_Context  *pContext );
	void BTST_P_Break( BTST_P_Context  *pContext );
	void BTST_P_ToggleStopRul( BTST_P_Context  *pContext );
	void BTST_P_InstallBvnErrCb( BTST_P_Context  *pContext );
	void BTST_P_CheckBvnErr( BTST_P_Context  *pContext );
	void BTST_P_ClearBvnErr( BTST_P_Context  *pContext );
	void BTST_P_MaskBvnErr( BTST_P_Context  *pContext );
	void BTST_P_StressTestBVN( BTST_P_Context  *pContext );
	void BTST_P_LipSyncStressTest( BTST_P_Context  *pContext );
	void BTST_P_StressTestAutoFormat( BTST_P_Context *pContext );
	void BTST_P_StressTestResizeFormat( BTST_P_Context *pContext );
	void BTST_P_StressTest3DVideo( BTST_P_Context  *pContext );
	void BTST_P_StressTestCustomFormat( BTST_P_Context  *pContext );
	void BTST_P_StressCustomFmtResize(BTST_P_Context *pContext);
	void BTST_P_SetPicDropMode( BTST_P_Context  *pContext );
	void BTST_P_StartCmdRecord( BTST_P_Context  *pContext );
	void BTST_P_EndCmdRecord( BTST_P_Context  *pContext );
	void BTST_P_DisplayVecSwap( BTST_P_Context  *pContext );
	void BTST_P_Display3D( BTST_P_Context  *pContext );
	void BTST_P_Display3DSrcBufSelect( BTST_P_Context  *pContext );
	void BTST_P_MpegDecodeMode( BTST_P_Context *pContext );
	void BTST_P_ReadWriteISRMisorders( BTST_P_Context *pContext );
	void BTST_P_ShowCapPic( BTST_P_Context *pContext );
	void BTST_P_SetPixelFormat( BTST_P_Context *pContext );
	void BTST_P_ToggleScaleDown( BTST_P_Context *pContext );
	void BTST_P_PrintHelp( BTST_P_Context  *pContext );
	void BTST_P_PrintStatus( BTST_P_Context	*pContext );
	void BTST_P_SetCoeffTblIndex( BTST_P_Context *pContext );
	void BTST_P_ChangeMfdSize( BTST_P_Context *pContext);
	void BTST_P_ChangeBarData( BTST_P_Context *pContext);
	void BTST_P_SetXvdRefreshRate( BTST_P_Context  *pContext );
	void BTST_P_RdcBlockOut( BTST_P_Context   *pContext);
	void BTST_P_DynamicRts( BTST_P_Context          *pContext);
	void BTST_P_Cm3d( BTST_P_Context   *pContext);
	void BTST_P_Loadrts( BTST_P_Context   *pContext);
	void BTST_P_SetupHeapFromFile( BTST_P_Context   *pContext );

#if (BVDC_BUF_LOG == 1)
	void BTST_P_EnableMultiBufLog( BTST_P_Context   *pContext);
	void BTST_P_MultiBufLogManualTrigger( BTST_P_Context   *pContext);
	void BTST_P_ToggleMultiBufLog( BTST_P_Context  *pContext);
#endif

	void BTST_P_BetterMpegDecode(
		BTST_P_Context           *pContext,
		char                     *fn,
		uint16_t                  ui16VidPid,
		uint16_t                  ui16PcrPid,
		uint16_t                  ui16EnhPid,
		BAVC_StreamType           eStreamType,
		BAVC_VideoCompressionStd  eVideoCmprStd );
	void BTST_P_ChgDispAlign( BTST_P_Context   *pContext);
	void BTST_P_RandDropIsr( BTST_P_Context  *pContext );


	void BTST_P_MpegDataReady_isr
		( void                            *pvSourceHandle,
		int                              iParm2,
		void                            *pvMvdField );
#if (BTST_P_ENABLE_MVD_WD)
	void BTST_P_MTPMVWatchdog_isr
		( void                            *pParm1,
		int                              iVidSrc,
		void                            *pParm3 );
#endif

#if BTST_P_SUPPORT_TUNER
	void BTST_P_Tune( BTST_P_Context  *pContext );
	void BTST_P_GetVsbStatus(BTST_P_Context  *pContext );
	void BTST_P_XptSrc(BTST_P_Context  *pContext );
	void BTST_P_Xpt_PSI(BTST_P_Context  *pContext );
#endif

	int BTST_P_Scanf(BTST_P_Context *pContext, const char * fmt, ...);
	void BTST_P_RegressionTests( BTST_P_Context  *pContext );
	void BTST_P_VfdAsSource_AutoTest ( BTST_P_Context *pContext);
	void BTST_P_ResourceLeak_AutoTest( BTST_P_Context  *pContext);

#ifdef __cplusplus
}
#endif

#endif /* VDC_TEST_COMMANDS_H__ */

/* end of file */
