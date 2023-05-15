/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: cmd_misc.c $
* $brcm_Revision: Hydra_Software_Devel/158 $
* $brcm_Date: 10/18/12 10:53a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/portinginterface/vdc/cmd_misc.c $
* 
* Hydra_Software_Devel/158   10/18/12 10:53a vanessah
* SW7445-60: fix the typo
* 
* Hydra_Software_Devel/157   10/17/12 4:12p vanessah
* SW7445-60: add MRC monitor flag to speedup emulation
* 
* Hydra_Software_Devel/156   10/17/12 3:39p vanessah
* SW7445-60: add 7445 loadrts function
* 
* Hydra_Software_Devel/155   10/11/12 12:08p syang
* SW7346-572: add long-time-lock for vdc_test
* 
* Hydra_Software_Devel/154   10/10/12 11:24a vanessah
* SW7445-10: add emu support for 7445
* 
* Hydra_Software_Devel/153   10/9/12 3:58p syang
* SW7346-572: rm TV related (vdb, dynamic rts, ...); rm symbolic link to
* files in applications/bmetest/grc_test
* 
* Hydra_Software_Devel/152   10/9/12 3:40p vanessah
* SW7425-4046: add custom fmt stress test from 1080p to 30x30 with step
* size 30
* 
* Hydra_Software_Devel/151   10/8/12 3:30p syang
* SW7346-572: use BTST_P_ApplyChanges to get uniform error handling;
* avoid holding hVdcCfgLock too long
* 
* Hydra_Software_Devel/150   9/26/12 3:23p nilesh
* SW7425-2568: Added TSM mode support for playback decode
* 
* Hydra_Software_Devel/149   9/26/12 11:46a vanessah
* SW7425-2501: add buffer configuration default due to nightly build test
* failure
* 
* Hydra_Software_Devel/148   9/25/12 5:35p vanessah
* SW7425-2501: vdc_test clean up
* 
* Hydra_Software_Devel/147   9/10/12 6:27p tdo
* SW7231-837: Remove band gap calibration option
*
* Hydra_Software_Devel/146   9/6/12 10:43a vanessah
* SW7425-2501: fix 7400/7410/7420/7342 compile error
*
* Hydra_Software_Devel/145   9/5/12 6:21p vanessah
* SW7425-2501: vdc_test clean up
*
* Hydra_Software_Devel/144   8/16/12 5:46p vanessah
* SW7425-2501: clean up unnecessary include file
*
* Hydra_Software_Devel/143   8/16/12 3:02p vanessah
* SW7425-2571: revise mask function and remove old mbvn
*
* Hydra_Software_Devel/142   8/9/12 11:21a syang
* SW7425-2172: add SetSurface support for MFD gfx feeding
*
* Hydra_Software_Devel/141   7/31/12 11:38a vanessah
* SW7360-12: add stoprul toggle  to disable rul output
*
* Hydra_Software_Devel/140   7/16/12 4:35p tdo
* SW7563-9: Add support for VDC
*
* Hydra_Software_Devel/139   7/9/12 5:17p vanessah
* SW7425-2501: removal 3548/3556 support
*
* Hydra_Software_Devel/138   7/6/12 12:48p tdo
* SW7231-837: Rename to add option to control band gap calibration
*
* Hydra_Software_Devel/137   7/5/12 12:07p tdo
* SW7231-837:  Add option to turn on/off Dac Gain Override
*
* Hydra_Software_Devel/136   7/5/12 11:33a vanessah
* SW7425-3432: add custom format stress test on transcode path
*
* Hydra_Software_Devel/135   6/21/12 1:25p tdo
* SW7584-5: Merge to mainline
*
* Hydra_Software_Devel/SW7584-5/1   6/19/12 5:14p chengs
* SW7584-5: Add 7584 support.
*
* Hydra_Software_Devel/134   5/22/12 3:49p vanessah
* SW7435-198: add load_rts function to avoid reboot system for different
* RTS config
*
* Hydra_Software_Devel/133   5/18/12 3:44p vanessah
* SW7425-3069: add resource leak change test
*
* Hydra_Software_Devel/132   5/15/12 3:06p vanessah
* SW7425-2571: set the default value bStopRul as true to delete redundant
* RUL
*
* Hydra_Software_Devel/131   5/3/12 12:18p vanessah
* SW7425-2990: load different RTS for 7425 without reboot system
*
* Hydra_Software_Devel/130   4/26/12 11:38a tdo
* SW7425-2571: Add VDC API to query BVN module error
*
* Hydra_Software_Devel/129   4/10/12 3:45p tdo
* SW7435-97: Format change on the SD path with progressive stream cause
* HD path to flicker
*
* Hydra_Software_Devel/128   3/29/12 4:28p pntruong
* SW7405-4296, SW7425-2526: Added some initial capabilities query at the
* vdc top level.
*
* Hydra_Software_Devel/127   3/29/12 2:26p yuxiaz
* SW7552-195: Update mbvn.
*
* Hydra_Software_Devel/126   3/15/12 10:17a tdo
* SW7435-9: Add dbg for mcvp/mcdi.  Add support for err handling for
* 7435A0
*
* Hydra_Software_Devel/125   2/29/12 12:46p tdo
* SW7425-2330: Add videolist cmd
*
* Hydra_Software_Devel/124   2/17/12 11:21a vanessah
* SW7425-2327: add Bar data handling test
*
* Hydra_Software_Devel/123   2/9/12 4:50p yuxiaz
* SW7425-2181: Added test case in vdc_test
*
* Hydra_Software_Devel/122   1/17/12 5:11p tdo
* SW7231-479:Fix cappic to work with psf case
*
* Hydra_Software_Devel/121   1/13/12 12:04p vanessah
* SW7552-182: support stressfmt test on more CMPs
*
* Hydra_Software_Devel/120   1/11/12 3:08p darnstein
* SW7125-1124: merge DCS Hybrid+ to main branch. DCS almost ready for
* production.
*
* Hydra_Software_Devel/SW7125-1124/1   1/10/12 5:02p darnstein
* SW7125-1124: Fix SOME of the bugs in RUL capture. Add a message "Begin
* RUL capture" to same.
*
* Hydra_Software_Devel/119   1/3/12 1:44p darnstein
* SW7125-1124: Write a diagnostic message when unable to open a file for
* writing.
*
* Hydra_Software_Devel/118   10/14/11 2:26p tdo
* SW7425-1416, SW7358-159: Add feature to control automatic DAC
* detection.  Default is currently OFF until it's fully functional.
*
* Hydra_Software_Devel/117   10/10/11 1:12p vanessah
* SW7425-1398: adding dual transcoding test cases.
*
* Hydra_Software_Devel/116   10/3/11 3:05p darnstein
* SW7400-3059: fix error in BDBG module handling for module vdc_test.
*
* Hydra_Software_Devel/115   9/29/11 3:22p vanessah
* SW7425-1078: A0 and P0 format support on GFD test
*
* Hydra_Software_Devel/114   9/16/11 1:21p yuxiaz
* SW7425-1283: Added 3d video stress test.
*
* Hydra_Software_Devel/113   8/25/11 10:45a tdo
* SW7420-2030: Add apply changes error tracking capability
*
* Hydra_Software_Devel/112   5/18/11 5:57p vanessah
* SW7425-32: add bvn middle error status monitor
*
* Hydra_Software_Devel/111   4/15/11 1:53p tdo
* SW7422-417: Add MVC/SVC support to vdc_test
*
* Hydra_Software_Devel/110   4/8/11 1:47p vanessah
* SW7425-32: add a new command to sleep multiple seconds
*
* Hydra_Software_Devel/109   3/30/11 10:25a yuxiaz
* SW7400-2882: Added test for BVDC_Window_EnableBufLog
*
* Hydra_Software_Devel/108   2/24/11 6:03p albertl
* SW7125-13: Fixed annoying infinite loop when entering ascii characters
* as input in BTST_P_DbgLevel().
*
* Hydra_Software_Devel/107   2/9/11 4:26p yuxiaz
* SW7400-2882: Added multi-buffering log to vdc_test.
*
* Hydra_Software_Devel/106   1/26/11 3:17p yuxiaz
* SW7400-2998, SW7425-32: Only reduce cdb/itb buffer size for mosaic
* mode.
*
* Hydra_Software_Devel/105   11/24/10 12:55p pntruong
* SW7552-17: Added initial 7552 support.
*
* Hydra_Software_Devel/104   11/11/10 6:45p pntruong
* SW7425-31: Refactored hdm/lvds/hdr/dvo and added support hdmi input on
* stb chips.  Removed dead codes.
*
* Hydra_Software_Devel/103   11/11/10 4:49p tdo
* SW7422-13: Add dbg msg for HDMI TX
*
* Hydra_Software_Devel/102   11/5/10 2:44p yuxiaz
* SW7422-33, SW7422-77: Added 3D support in static buffer test.
*
* Hydra_Software_Devel/101   11/3/10 11:25a yuxiaz
* SW7422-33: Added format selection for static buffer.
*
* Hydra_Software_Devel/100   10/29/10 5:01p vanessah
* SW7422-43:  3D graphics render, API change
*
* Hydra_Software_Devel/99   10/29/10 3:08p tdo
* SW7422-13: add 7422 support to vdc_test
*
* Hydra_Software_Devel/98   10/20/10 12:36p jessem
* SW7420-173: Added regression tests for VF as source feature.
*
* Hydra_Software_Devel/97   10/19/10 11:32a yuxiaz
* SW7422-39: Added test for BAVC_SourceId_eMpeg2 support in VDC.
*
* Hydra_Software_Devel/96   10/19/10 10:50a pntruong
* SW7425-16, SW3548-1650: Adapted to the removal of
* BXVD_ChannelSettings.bFGTEnable.  Also get rid of all reference to fgt
* in vdctest app.
*
* Hydra_Software_Devel/95   10/15/10 3:25p hongtaoz
* SW7425-1, SW7425-10: added MEMC1 support to emulation framwork;
*
* Hydra_Software_Devel/94   9/28/10 10:38p hongtaoz
* SW7425-32: customize debug module level;
*
* Hydra_Software_Devel/93   9/27/10 4:06p hongtaoz
* SW7425-32: added VDC_TEST_ONLY compile option;
*
* Hydra_Software_Devel/92   9/23/10 3:34p tdo
* SW7422-13: add 7422 support to vdc_test
*
* Hydra_Software_Devel/91   2/1/10 6:55p hongtaoz
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
* Hydra_Software_Devel/90   2/1/10 3:27p yuxiaz
* SW7405-3184: Added more print out for stress tests.
*
* Hydra_Software_Devel/89   1/26/10 3:52p yuxiaz
* SW7405-3184: Convert stress test to use playback stream.
*
* Hydra_Software_Devel/88   1/15/10 5:13p yuxiaz
* SW7405-3290: Added test to detect if MPEG input.
*
* Hydra_Software_Devel/87   1/7/10 6:12p yuxiaz
* SW7405-3184: Added channel change, format change resize stress test.
*
* Hydra_Software_Devel/86   1/7/10 11:26a yuxiaz
* SW7405-3184: Added auto format stress test.
*
* Hydra_Software_Devel/85   12/23/09 2:06p rpan
* SW7468-22: Enable "cappic" command for 7468.
*
* Hydra_Software_Devel/84   12/15/09 5:47p yuxiaz
* SW7408-13: Get vdc_test compile for 7408. Clean up compile flags.
*
* Hydra_Software_Devel/83   12/4/09 3:56p tdo
* SW7408-23: Add playback feature for digital source
*
* Hydra_Software_Devel/82   12/4/09 2:51p syang
* SW7550-70: add mem alloc msg
*
* Hydra_Software_Devel/81   12/1/09 4:46p rpan
* SW7468-22: Got vdc_test compiled for 7468.
*
* Hydra_Software_Devel/80   11/23/09 3:59p rpan
* SW7468-22: Initial work for 7468.
*
* Hydra_Software_Devel/79   10/14/09 4:47p tdo
* SW7420-376: Bringup appframework for 7410
*
* Hydra_Software_Devel/78   9/16/09 4:41p darnstein
* SW7342-39: check a function return value for error.
*
* Hydra_Software_Devel/77   8/31/09 5:56p albertl
* SW7125-13: Added initial 7125 support.
*
* Hydra_Software_Devel/76   8/4/09 1:01p darnstein
* PR55225: mechanical updates for 7342 chipset.
*
* Hydra_Software_Devel/75   7/31/09 6:48p syang
* PR 55812:  add 7550 support
*
* Hydra_Software_Devel/74   7/27/09 4:57p pntruong
* PR55861: Rid of old non-portable code.
*
* Hydra_Software_Devel/73   7/24/09 3:02p pntruong
* PR54512: Removed duplicated code, and just use the existing command
* 'pxlfmt' for setting for capture pixel format.
*
* Hydra_Software_Devel/72   7/10/09 5:54p rpan
* PR55188: Convert scanf() to BTST_P_Scanf().
*
* Hydra_Software_Devel/71   6/8/09 4:52p yuxiaz
* PR54992: Reset delay offset to 0 in lip sync test.
*
* Hydra_Software_Devel/70   6/8/09 3:36p yuxiaz
* PR54992: Added automated lipsync stress test.
*
* Hydra_Software_Devel/69   6/3/09 11:49a yuxiaz
* PR54992: Fixed compile error on 3549.
*
* Hydra_Software_Devel/68   6/2/09 10:56a rpan
* PR54992: Added thread synchronization for graceful exit.
*
* Hydra_Software_Devel/67   6/1/09 1:36p rpan
* PR54992: Clear BVN status register prior to starting the BVN stress
* test.
*
* Hydra_Software_Devel/66   5/15/09 4:35p yuxiaz
* PR 54512: 10-bit YCbCr is not a supported format for output of BLT.
*
* Hydra_Software_Devel/65   5/15/09 10:30a rpan
* PR55188: Added support for capturing and repeating comamnds.
*
* Hydra_Software_Devel/64   5/14/09 3:00p rpan
* PR54992: Exit with non-zero value if BVN stress test failed. This is
* for BAT integration.
*
* Hydra_Software_Devel/63   5/12/09 4:24p rpan
* PR54992: Add loop count to bvn stress test.
*
* Hydra_Software_Devel/62   5/12/09 3:21p yuxiaz
* PR 54512: More on graphics compression test for captured video surface.
*
* Hydra_Software_Devel/61   5/8/09 4:05p yuxiaz
* PR 54512: Added graphics compression test for captured video surface.
*
* Hydra_Software_Devel/60   5/5/09 4:29p rpan
* PR52203: Log BVN errors to a file. This is for automated overnight
* test.
*
* Hydra_Software_Devel/59   4/24/09 5:24p jessem
* PR 53506: Added VDB dbg.
*
* Hydra_Software_Devel/58   4/9/09 11:37a syang
* PR53239: fix compile error for 7400
*
* Hydra_Software_Devel/57   4/7/09 2:33p tdo
* PR53239: Fix compiling errors
*
* Hydra_Software_Devel/56   4/7/09 10:04a tdo
* PR53239: Remove usage of private definitions in vdc unittest
*
* Hydra_Software_Devel/55   4/6/09 5:12p hongtaoz
* PR53037, PR53038, PR53687, PR53688: fixed compile warning for 74xx;
*
* Hydra_Software_Devel/54   4/6/09 5:01p hongtaoz
* PR53037, PR53038, PR53687, PR53688: monitor all BVN errors; used PCI L2
* interrupt instead of CPU interrupts to avoid confliction with VDC BVN
* error debug code;
*
* Hydra_Software_Devel/53   4/6/09 2:56p yuxiaz
* PR46252: Need to expose 444 format in static buffers case.
*
* Hydra_Software_Devel/52   3/30/09 4:14p rpan
* PR53677: Added command for NTSC->PAL picture drop mode selection.
*
* Hydra_Software_Devel/51   3/16/09 12:59p tdo
* PR52203: Fixed compiling errors for 7420
*
* Hydra_Software_Devel/50   3/13/09 5:46p rpan
* PR52203: Abstract VEC disable to a function for better portability.
*
* Hydra_Software_Devel/49   3/9/09 7:14p pntruong
* PR52203: Fixed compiling errors for 3548 and 3556.
*
* Hydra_Software_Devel/48   3/9/09 4:41p rpan
* PR52203: Implemented some stress tests and debugging facility targeting
* VNET reconfiguration and multi-buffering.
*
* Hydra_Software_Devel/47   2/9/09 4:00p tdo
* PR50391: Support 7420 A1 compiling
*
* Hydra_Software_Devel/46   2/4/09 1:40p tdo
* PR51627: add vdc_test support for 7336
*
* Hydra_Software_Devel/45   1/14/09 4:09p syang
* PR 50717:  resume deom src-pending after src becomes "active"
*
* Hydra_Software_Devel/44   10/24/08 5:45p albertl
* PR48199, PR48046: Removed BTST_P_EnableRdcCache because BMEM_SetCache
* is obsolete.
*
* Hydra_Software_Devel/43   10/7/08 4:14p syang
* PR35549: allow run-time choice of crc-enabling when src callback is
* enabled
*
* Hydra_Software_Devel/42   9/24/08 6:49p pntruong
* PR47113, PR45308: Allow flexibility in selecting builtins and user
* defined filter coefficients.
*
* Hydra_Software_Devel/41   8/21/08 11:06a rpan
* PR45381: Wait 100ms before try getting user capture buffer again.
*
* Hydra_Software_Devel/40   8/20/08 3:40p pntruong
* PR45641: Added all the 10-bit 422 pixel format selection.
*
* Hydra_Software_Devel/39   8/19/08 2:44p pntruong
* PR45533: See rts client list.
*
* Hydra_Software_Devel/38   7/23/08 7:02p tdo
* PR43508, PR43509: Mapping individual bandgap adjustment for each DAC
*
* Hydra_Software_Devel/37   7/16/08 6:14p jessem
* PR 39237: Added conditional compile for BVDB_SUPPORT.
*
* Hydra_Software_Devel/36   7/16/08 1:47p jessem
* PR 39237: Integrated VDB to use with dynamic RTS and source pending.
*
* Hydra_Software_Devel/35   6/13/08 6:09p jessem
* PR 43516: Updated RTS B, C, D, and F.
*
* Hydra_Software_Devel/34   6/12/08 11:00a yuxiaz
* PR 43516: Fixed compile error on 7405 and 7325.
*
* Hydra_Software_Devel/33   6/11/08 11:38a jessem
* PR 43516: Revised RTS sets G, J, M, and R.
*
* Hydra_Software_Devel/32   6/10/08 6:09p syang
* PR 39209:  update to use new InitializeGfx2Mfd
*
* Hydra_Software_Devel/31   6/10/08 4:37p tdo
* PR 39152: Fix compiling error for 7335
*
* Hydra_Software_Devel/30   6/9/08 6:44p jessem
* PR 39152: Added option for RTS R in dynamic RTS test.
*
* Hydra_Software_Devel/29   6/9/08 4:14p jessem
* PR 39152: Enclosed register write to BCHP_MEMC_0_1_CLIENT_INFO_0 to
* 3548 only.
*
* Hydra_Software_Devel/28   6/7/08 10:44a jessem
* PR 39152: Removed bvdb.h and bvdb_query.h.
*
* Hydra_Software_Devel/27   6/7/08 10:01a jessem
* PR 39152: Added RTS R table.
*
* Hydra_Software_Devel/26   6/7/08 9:31a jessem
* PR 39152: Added dynamic rts test.
*
* Hydra_Software_Devel/25   5/19/08 1:51p yuxiaz
* PR39251: Added test for 10bit 444 and 10bit 422 capture.
*
* Hydra_Software_Devel/24   5/14/08 12:57p syang
* PR35549:  continue to resume from src pending as  src callback fires
* with bSrcPending
*
* Hydra_Software_Devel/23   5/13/08 4:27p jessem
* PR 39237: Modified conditional when triggering the source pending event
* in BTST_P_SourceCb_isr.
*
* Hydra_Software_Devel/22   5/12/08 5:16p jessem
* PR 39237: Added support for source pending task.
*
* Hydra_Software_Devel/21   4/11/08 2:15p tdo
* PR39333: Add debug option for HIST
*
* Hydra_Software_Devel/20   3/31/08 3:40p jessem
* PR 39335: Added CM3D tests.
*
* Hydra_Software_Devel/19   3/21/08 2:25p hongtaoz
* PR39154: added MAD and ANR support for 3548;
*
* Hydra_Software_Devel/18   2/29/08 11:15a jessem
* PR 38623: Replaced rdc_block_out.h with brdc.h.
*
* Hydra_Software_Devel/17   2/28/08 5:04p jessem
* PR 38623: Added RDMA blockout support.
*
* Hydra_Software_Devel/16   2/11/08 9:21p pntruong
* PR39177: [VEC] Need new display api to specify framedrop at runtime.
*
* Hydra_Software_Devel/15   2/5/08 11:12a pntruong
* PR34870: Added get handle debug functions for video tool library.
*
* Hydra_Software_Devel/14   12/13/07 11:24p pntruong
* PR34089: Adapt to additional enum.
*
* Hydra_Software_Devel/13   11/15/07 6:04p pntruong
* PR32896: Prevent false vcr detection with copy protection contents.
*
* Hydra_Software_Devel/12   10/25/07 3:30p yuxiaz
* PR29569, PR36290: Add FGT support on 7405.
*
* Hydra_Software_Devel/11   10/19/07 11:06a rpan
* PR34857: Fixed a window handle assignment issue.
*
* Hydra_Software_Devel/10   10/18/07 11:08a rpan
* PR34857: Removed redundant stopping decode logic during shutdown
* process.
*
* Hydra_Software_Devel/9   10/17/07 10:03p pntruong
* PR34857: Fixed return of cappic.
*
* Hydra_Software_Devel/8   10/16/07 2:44p rpan
* PR34857: First version after merging the latest xvd_vdc_test and
* mvd_vdc_test as of Oct. 16, 2007, 12pm.
*
*
* Hydra_Software_Devel/1   9/21/07 4:40p rpan
* PR34857: Initial revision.
*
*
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "config.h"
#include "test_util.h"
#include "commands.h"
#include "mpg_decoder.h"
#include "memview.h"
#include "vdec_input.h"
#include "interactive.h"
#include "bvdc_dbg.h"
#include "bkni_multi.h"
#include "bmem_debug.h"
#include "bdbg.h"
#include "bstd_defs.h"
#include "brdc_dbg.h"
#include "brdc.h"

#if ( MEMC_1_MEM_PBASE && MEMC_0_MEM_UBASE)
#include "bchp_memc_arb_0.h"
#include "bchp_memc_arb_1.h"
#include "rts.h"
#if BCHP_MEMC_ARB_2_REG_START
#include "bchp_memc_arb_2.h"
#endif
#endif


#if defined BRDC_USE_CAPTURE_BUFFER
BKNI_EventHandle s_hCaptureDoneEvent = NULL;
#endif
int g_bTestStreamClip = 0;
int g_bRandomStrmCrop = 0;
uint32_t g_ulCropLeft = 0, g_ulCropTop = 0;
uint32_t g_ulCropRight = 0, g_ulCropBot = 0;
bool g_bEnablePicListForMosaicMode = false;
bool g_bMosaicWorst = false;
bool g_bCapture = false;
bool g_bInterceptFrame = false;
extern uint32_t  g_minY, g_maxY;
extern BPXL_Format       g_ePxlFmt;

BTST_P_PlaybackStreamInfo astPlaybackStream[] =
{
	/* 480i stream */
	{"/data/videos/italyriviera-spiderman2_cc_short.mpg", 0x11, 0x11, 0x1FFF, 0, 1},
	/* 480p stream */
	{"/data/videos/trp_002_japan_480p_q64.mpg",           0x11, 0x11, 0x1FFF, 0, 1},
	/* 720p stream */
	{"/data/videos/trp_001_colrbeat_720p_q64.mpg",        0x11, 0x11, 0x1FFF, 0, 1},
	/* 1080i stream */
	{"/data/videos/italyriviera-spiderman2_cc_short.mpg", 0x31, 0x31, 0x1FFF, 0, 1}

};

#define BTST_PLAYBACK_STREAM_COUNT    \
	(sizeof(astPlaybackStream)/sizeof(BTST_P_PlaybackStreamInfo))

BDBG_MODULE(BTST);

/***************************************************************************
*
*/
void BTST_P_GetHandles
( BTST_P_Context          *pContext )
{
	uint32_t i, j;
	BVDC_Source_Handle hSrc;
	BVDC_Window_Handle hWin;

	BDBG_MSG(("------------SRC-----------"));
	for(i = 0; i <= BAVC_SourceId_eDs0; i++)
	{
		hSrc = BVDC_Dbg_GetSourceHandle(pContext->hVdc, i);
		if(hSrc)
		{
			BDBG_MSG(("src[%02d] = 0x%08x", i, hSrc));
		}
	}

	BDBG_MSG(("------------WIN-----------"));
	for(i = 0; i <= BVDC_CompositorId_eCompositor2; i++)
	{
		for(j = 0; j <= BVDC_WindowId_eGfx2; j++)
		{
			hWin = BVDC_Dbg_GetWindowHandle(pContext->hVdc, i, j);
			if(hWin)
			{
				BDBG_MSG(("Cmp[%d]Win[%d] = 0x%08x", i, j, hWin));
			}
		}
	}

	return;
}

/***************************************************************************
*
*/
void BTST_P_Stub
( BTST_P_Context          *pContext )
{
#if (BTST_P_USE_MRC_MONITOR)
	BMRC_Client eClienId;
	BMRC_ClientInfo stClientInfo;

	for(eClienId = BMRC_Client_eXPT_0;
		eClienId < BMRC_Client_eMaxCount;
		eClienId++)
	{
		BMRC_Checker_GetClientInfo(pContext->hMrc, eClienId, &stClientInfo);

		if(BMRC_Client_eInvalid != stClientInfo.usClientId)
		{
			BDBG_ERR(("\t%-15s = %-d", stClientInfo.pchClientName, stClientInfo.usClientId));
		}
	}

	system("ifconfig");
#else
	BSTD_UNUSED(pContext);
#endif
	return;
}

/***************************************************************************
*
*/
void BTST_P_VideoList
( BTST_P_Context          *pContext )
{
	char achDir[256];
	char achCmd[256];
	printf("Enter the directory name ['.' for current dir]: ");
	BTST_P_Scanf(pContext, "%s", achDir);
	sprintf(achCmd, "ls %s", achDir);

	system(achCmd);
	return;
}

/***************************************************************************
*
*/
void BTST_P_Cat
( BTST_P_Context          *pContext )
{
	char achDir[256];
	char achCmd[256];
	printf("Enter the file name: ");
	BTST_P_Scanf(pContext, "%s", achDir);
	sprintf(achCmd, "cat %s", achDir);

	system(achCmd);
	return;
}

/***************************************************************************
*
*/
void BTST_P_ChipFeature
( BTST_P_Context          *pContext )
{
	bool ulFeature = 0;
	BCHP_GetFeature(pContext->hChp,
		BCHP_Feature_e3DGraphicsCapable,
		(void *)&ulFeature);
	printf("BCHP_Feature_e3DGraphicsCapable  = %d\n", ulFeature);

	BCHP_GetFeature(pContext->hChp,
		BCHP_Feature_eDvoPortCapable,
		(void *)&ulFeature);
	printf("BCHP_Feature_eDvoPortCapable     = %d\n", ulFeature);

	BCHP_GetFeature(pContext->hChp,
		BCHP_Feature_eMacrovisionCapable,
		(void *)&ulFeature);
	printf("BCHP_Feature_eMacrovisionCapable = %d\n", ulFeature);

	BCHP_GetFeature(pContext->hChp,
		BCHP_Feature_eHdcpCapable,
		(void *)&ulFeature);
	printf("BCHP_Feature_eHdcpCapable        = %d\n", ulFeature);

	BCHP_GetFeature(pContext->hChp,
		BCHP_Feature_e3desCapable,
		(void *)&ulFeature);
	printf("BCHP_Feature_e3desCapable        = %d\n", ulFeature);

	BCHP_GetFeature(pContext->hChp,
		BCHP_Feature_e1080pCapable,
		(void *)&ulFeature);
	printf("BCHP_Feature_e1080pCapable       = %d\n", ulFeature);

	BCHP_GetFeature(pContext->hChp,
		BCHP_Feature_eMemCtrl1Capable,
		(void *)&ulFeature);
	printf("BCHP_Feature_eMemCtrl1Capable    = %d\n", ulFeature);

	BCHP_GetFeature(pContext->hChp,
		BCHP_Feature_eMemCtrl2Capable,
		(void *)&ulFeature);
	printf("BCHP_Feature_eMemCtrl2Capable    = %d\n", ulFeature);

	BCHP_GetFeature(pContext->hChp,
		BCHP_Feature_eMemCtrl0DDR3ModeCapable,
		(void *)&ulFeature);
	printf("MEMC_0 DDR3 Mode Capable         = %d\n", ulFeature);

	BCHP_GetFeature(pContext->hChp,
		BCHP_Feature_eMemCtrl0DDRDeviceTechCount,
		(void *)&ulFeature);
	printf("MEMC_0 Device Tech in Mbits      = %d\n", ulFeature);

	BCHP_GetFeature(pContext->hChp,
		BCHP_Feature_eMemCtrl0DramWidthCount,
		(void *)&ulFeature);
	printf("MEMC_0 DRAM data width in bits   = %d\n", ulFeature);

	BCHP_GetFeature(pContext->hChp,
		BCHP_Feature_eMemCtrl1DDR3ModeCapable,
		(void *)&ulFeature);
	printf("MEMC_1 DDR3 Mode Capable         = %d\n", ulFeature);

	BCHP_GetFeature(pContext->hChp,
		BCHP_Feature_eMemCtrl1DDRDeviceTechCount,
		(void *)&ulFeature);
	printf("MEMC_1 Device Tech in Mbits      = %d\n", ulFeature);

	BCHP_GetFeature(pContext->hChp,
		BCHP_Feature_eMemCtrl1DramWidthCount,
		(void *)&ulFeature);
	printf("MEMC_1 DRAM data width in bits   = %d\n", ulFeature);

	BCHP_GetFeature(pContext->hChp,
		BCHP_Feature_eMpegDecoderCount,
		(void *)&ulFeature);
	printf("BCHP_Feature_eMpegDecoderCount   = %d\n", ulFeature);

	return;
}


/***************************************************************************
*
*/
void BTST_P_VdcDbgDump
( BTST_P_Context          *pContext )
{
	BVDC_Dbg_Dump(pContext->hVdc);
	return;
}

/***************************************************************************
*
*/
void BTST_P_CleanUp
( BTST_P_Context          *pContext )
{
	uint32_t     ulCmpNum ;

	ulCmpNum = pContext->stCapabilities.ulNumCmp;
	/* Clean up windows. */
	pContext->iSelectedWin = 0;
	pContext->iSelectedComp = 0;
	pContext->iSelectedVidSrc = 0;

	if(!pContext->hVdc)
	{
		goto done;
	}

	for(pContext->iSelectedComp = 0;
		pContext->iSelectedComp < ulCmpNum;
		pContext->iSelectedComp++)
	{
		for(pContext->iSelectedWin = 0;
			pContext->iSelectedWin < BTST_P_MAX_WINDOW;
			pContext->iSelectedWin++)
		{
			if(pContext->iSelectedWin == 2)
			{
				BTST_P_CloseGfxWin(pContext);
			}
			else
			{
				BTST_P_CloseVidWin(pContext);
			}
		}
		BTST_P_ApplyChanges(pContext);
	}

	/* Close Sources. */
	pContext->iSelectedWin = 0;
	pContext->iSelectedComp = 0;
	pContext->iSelectedVidSrc = 0;

	for(pContext->iSelectedVidSrc = 0;
		pContext->iSelectedVidSrc < BAVC_SourceId_eMax;
		pContext->iSelectedVidSrc++)
	{
		BTST_P_CloseVidSrc(pContext);
		BTST_P_ApplyChanges(pContext);
	}

#if !BTST_VDC_ONLY
	/* Stop and close MPEG channels */
	for(pContext->eSelectedMDec = BTST_XVD_0; pContext->eSelectedMDec < MAX_XVD_CORES; pContext->eSelectedMDec++)
	{
		for(pContext->iSelectedMDecCh=0; pContext->iSelectedMDecCh < BTST_P_MAX_CHANNELS; pContext->iSelectedMDecCh++)
		{
			if (pContext->astMDecInfo[pContext->eSelectedMDec].abChnDecoded[pContext->iSelectedMDecCh])
			{
				BTST_CloseDecoderChannel(pContext);
				pContext->astMDecInfo[pContext->eSelectedMDec].abChnDecoded[pContext->iSelectedMDecCh] = false;
			}

		}
	}
#endif
	/* Close Compositors. */
	pContext->iSelectedWin = 0;
	pContext->iSelectedComp = 0;
	pContext->iSelectedVidSrc = 0;

	for(pContext->iSelectedComp = 0;
		pContext->iSelectedComp < ulCmpNum;
		pContext->iSelectedComp++)
	{
		BTST_P_CloseComp(pContext);
		BTST_P_ApplyChanges(pContext);

		if(pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface)
		{
			BSUR_Surface_Destroy(pContext->aSrcContext[pContext->iSelectedVidSrc].hGfxSurface);
		}
		if (pContext->aSrcContext[pContext->iSelectedVidSrc].pGfxBuf)
		{
			BKNI_Memset((void*)pContext->aSrcContext[pContext->iSelectedVidSrc].pGfxBuf, 0xA3, sizeof(BAVC_Gfx_Picture));
			BKNI_Free((void*)pContext->aSrcContext[pContext->iSelectedVidSrc].pGfxBuf);
		}
	}
	pContext->iSelectedComp = 0;

	/* Close VDC */
	BTST_P_CloseVdcMisc(pContext);

done:
	/* Close HDMI if opened. */
	BTST_P_CloseHdmi(pContext);

	/* Close RFM if opened. */
	BTST_P_CloseRfm(pContext);

	/* Close other handles. */

	return;
}

void BTST_P_GetCbStats
( BTST_P_Context          *pContext )
{
	BINT_Stats_Dump(pContext->hInt);
	return;
}

void BTST_P_ResetCbStats
( BTST_P_Context          *pContext )
{
	BINT_CallbackHandle cbHandle;
	int iCallbackNum;

	BTST_P_GetCbStats( pContext);

	cbHandle = BINT_GetCallbackFirst( pContext->hInt );

	printf("Enter callback to reset: ");
	BTST_P_Scanf(pContext, "%d", &iCallbackNum);
	while (iCallbackNum > 1)
	{
		cbHandle = BINT_GetCallbackNext( cbHandle );
		if (!cbHandle)
		{
			printf("Invalid callback number!\n");
			return;
		}

		iCallbackNum--;
	}

	BINT_Stats_Reset(cbHandle);

	return;
}

void BTST_P_AddCbBin
( BTST_P_Context          *pContext )
{
	BINT_CallbackHandle cbHandle = NULL;
	uint32_t ulCallbackNum;
	uint32_t ulBinRangeMin = 0;
	uint32_t ulBinRangeMax = 0;

	BTST_P_GetCbStats( pContext);

	printf("Enter callback to add bin to: ");
	BTST_P_Scanf(pContext, "%u", &ulCallbackNum);

	if (ulCallbackNum < 1)
	{
		printf("Invalid callback number!\n");
		return;
	}

	cbHandle = BINT_GetCallbackFirst( pContext->hInt );

	while (ulCallbackNum > 1)
	{
		cbHandle = BINT_GetCallbackNext( cbHandle );
		if (!cbHandle)
		{
			printf("Invalid callback number!\n");
			return;
		}

		ulCallbackNum--;
	}

	printf("Enter minimum range: ");
	BTST_P_Scanf(pContext, "%u", &ulBinRangeMin);
	printf("Enter maximum range: ");
	BTST_P_Scanf(pContext, "%u", &ulBinRangeMax);

	BINT_Stats_AddBin(cbHandle, ulBinRangeMin, ulBinRangeMax);

	return;
}

/***************************************************************************
*
*/
void BTST_P_MemMonitor
( BTST_P_Context          *pContext )
{
#if BTST_P_USE_MRC_MONITOR
	BMRC_Monitor_JailUpdate(pContext->hMonitor);
	printf("memmonitor\n");
#else
	BSTD_UNUSED(pContext);
#endif
	return;
}

void BTST_P_CaptureRulHandler
( BTST_TaskHandle          hTask,
 void                    *pContext )
{
	BRDC_Handle rdc = ((BTST_P_Context*)pContext)->hRdc;
	int total;
	uint8_t mem[4096];
	char achFile[] = "rul_data";
	static FILE *file = NULL;
	static int filesize = 0;
	static int filecnt = 0;
	char buf[256];
	int maxfilesize;

	BSTD_UNUSED(hTask);

	BKNI_EnterCriticalSection();
	BRDC_DBG_LogErrorCode_isr (rdc, BRDC_DBG_BVN_ERROR, "Begin RUL capture");
	BKNI_LeaveCriticalSection();

	while(((BTST_P_Context*)pContext)->bEnableCaptureRul)
	{
		if (!file) {
			BKNI_Snprintf(buf, 256, "%s%d", achFile, filecnt++);
			printf("creating RUL log file %s\n", buf);
			file = fopen(buf, "w+");
			if (!file)
			{
				fprintf (stderr, "Failed to open file \"%s\" for writing\n",
					buf);
			}
		}

		do {
			BKNI_EnterCriticalSection();
			BRDC_DBG_ReadCapture_isr(rdc, mem, 4096, &total);
			BKNI_LeaveCriticalSection();
			if (total) {
				fwrite(mem, total, 1, file);
				filesize += total;
			}
		} while (total);

		if (((BTST_P_Context*)pContext)->bStopRul)
			maxfilesize = 2*1024*1024;
		else
			maxfilesize = 100*1024*1024;

		if (filesize >= 2*1024*1024) {
			fclose(file);
			file = NULL;
			filesize = 0;

			/* on the next write, a new file will be opened. */

			if (((BTST_P_Context*)pContext)->bStopRul)
			{
				/* Delete the previously generated file when
				* stress testing BVN. This is to avoid too
				* many log files generated.
				*/
				if (filecnt >= 2)
				{
					BKNI_Snprintf(buf, 256, "%s%d", achFile, (filecnt-2));
					printf("delete RUL log file %s\n", buf);
					remove(buf);
				}
			}
		}
		BKNI_Sleep(100);
	}

	if (file)
	{
		fclose(file);
		file = NULL;
		filesize = 0;
	}
	/* BKNI_SetEvent(((BTST_P_Context*)pContext)->hCaptureRulEvent); */

	printf("Exit RUL capture thread\n");

}

void BTST_P_CaptureRul
( BTST_P_Context          *pContext )
{
	/* toggle it */
	pContext->bEnableCaptureRul ^= true;

	if(pContext->bEnableCaptureRul)
	{
		printf("start to capture RULs\n");
		/* BKNI_CreateEvent(&pContext->hCaptureRulEvent); */
		/* BKNI_ResetEvent(pContext->hCaptureRulEvent); */
		BTST_CreateTask(&pContext->hTaskCaptureRul, BTST_P_CaptureRulHandler, (void*)pContext);
	}
	else
	{
		printf("stop to capture RULs\n");
		BKNI_Sleep(100);
		/* TEST_CLOSE_HANDLE(BKNI_DestroyEvent, pContext->hCaptureRulEvent); */
		TEST_CLOSE_HANDLE(BTST_DestroyTask, pContext->hTaskCaptureRul);
	}
	return;
}

#if (BVDC_BUF_LOG == 1)
static void BTST_P_MultiBufLogCallBack
( void                           *pvParm1,
 int                             iParm2,
 void                           *pvVdcData )
{
	BSTD_UNUSED(pvParm1);
	BSTD_UNUSED(iParm2);
	BSTD_UNUSED(pvVdcData);

	printf("\nDUMP OUT MULTI_BUFFERING LOG\n\n");
	BVDC_DumpBufLog();

}

void BTST_P_EnableMultiBufLog
( BTST_P_Context          *pContext )
{
	BVDC_BufLogState   eBufLogState;

	/* toggle it */
	pContext->bEnableMultiBufLog ^= true;

	if(pContext->bEnableMultiBufLog)
	{
		printf("start Multi-buffering Log\n");
		printf("Select how to dump log: Manual(1), Auto(2), AutoReduced(3)  ");
		BTST_P_Scanf(pContext, "%d", &eBufLogState);

		BVDC_SetBufLogStateAndDumpTrigger(eBufLogState,
			BTST_P_MultiBufLogCallBack, (void *)pContext, 0);
	}
	else
	{
		printf("stop Multi-buffering Log\n");
		BVDC_SetBufLogStateAndDumpTrigger(BVDC_BufLogState_eReset,
			NULL, NULL, 0);
	}

	return;
}

void BTST_P_MultiBufLogManualTrigger
( BTST_P_Context          *pContext )
{

	if(pContext->bEnableMultiBufLog)
	{
		BVDC_SetBufLogManualTrigger();
	}

	return;
}

void BTST_P_ToggleMultiBufLog
( BTST_P_Context          *pContext )
{
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
		return;

	printf("Cmp[%d]Win[%d] Toggle Multi-buffering Log (current: %d):  \n",
		pContext->iSelectedComp, pContext->iSelectedWin,
		pContext->abWinEnableBufLog[pContext->iSelectedComp][pContext->iSelectedWin]);

	pContext->abWinEnableBufLog[pContext->iSelectedComp][pContext->iSelectedWin] =
		!pContext->abWinEnableBufLog[pContext->iSelectedComp][pContext->iSelectedWin];

	BVDC_Window_EnableBufLog(hVidWin,
		pContext->abWinEnableBufLog[pContext->iSelectedComp][pContext->iSelectedWin]);

	return;
}

#endif
/***************************************************************************
*
*/
void BTST_P_ToggleStopRul
( BTST_P_Context          *pContext )
{
	pContext->bStopRul^=1;
	printf("\n StopRul %s\n ", pContext->bStopRul? "true":"false");
}
void BTST_P_InstallBvnErrCb
( BTST_P_Context          *pContext )
{
	/* toggle it */
	pContext->bEnableCheckBvnErr ^= true;
	pContext->bStopRul = true;

	if(pContext->bEnableCheckBvnErr)
	{
		printf("start to check BVN errors \n");
		BVDC_Dbg_CreateBvnErrorCb(pContext->hVdc);
	}
	else
	{
		printf("stop checking BVN errors\n");
		BVDC_Dbg_DestroyBvnErrorCb(pContext->hVdc);
	}
	return;
}

void BTST_P_ClearBvnErr
( BTST_P_Context          *pContext )
{
	BVDC_Dbg_ClearBvnError(pContext->hVdc);
	return;
}

void BTST_P_CheckBvnErr
( BTST_P_Context          *pContext )
{
	BVDC_Dbg_GetBvnErrorStatus(pContext->hVdc);
	return;
}

void BTST_P_MaskBvnErr
( BTST_P_Context          *pContext )
{
	uint32_t ulModule, ulIdx, iMask;

	while(true)
	{
		printf("\nEnter BVN module to mask [%d to exit]: \n ", BVDC_Bvn_eMaxCount);
		printf("\n ( 0) RDC  ( 1) MPEG ( 2) VFD  ( 3) SCL ( 4) DNR    ( 5) MAD    ( 6) MVP (7) MCDI ");
		printf("\n ( 8) MCTF ( 9) HSCL (10) CAP  (11) GFD (12) CMP_V0 (13) CMP_V1 (14) CMP_G");
		BTST_P_Scanf(pContext, "%d", &ulModule);
		if(ulModule >= ((uint32_t)BVDC_Bvn_eMaxCount))
		{
			printf("\n invalid parameters ");
			break;
		}

		printf("\nEnter module index ");
		BTST_P_Scanf(pContext, "%d", &ulIdx);


		printf("Enter mask option [1=OFF/0=ON]: ");
		BTST_P_Scanf(pContext, "%d", &iMask);
		BVDC_Dbg_MaskBvnErrorCb(pContext->hVdc, (BVDC_BvnModule)ulModule, ulIdx, (bool)iMask);
	}
}

static void BTST_P_ScaleDestHandler
( BTST_TaskHandle          hTask,
 void                    *pTestContext )
{
	BTST_P_Context *pContext = (BTST_P_Context *)pTestContext;

	uint32_t ulWidth_full_0, ulHeight_full_0, ulWidth_full_1, ulHeight_full_1;
	uint32_t ulX_full_0, ulY_full_0, ulX_full_1, ulY_full_1;

	uint32_t ulWidth_down_0, ulHeight_down_0, ulWidth_down_1, ulHeight_down_1;
	uint32_t ulX_down_0, ulY_down_0, ulX_down_1, ulY_down_1;

	const BFMT_VideoInfo *pFmtInfo_0 = pContext->apFmtInfo[0];
	BVDC_Window_Handle hVidWin_0 = pContext->aWinContext[0][0].hWindow;

	const BFMT_VideoInfo *pFmtInfo_1 = pContext->apFmtInfo[1];
	BVDC_Window_Handle hVidWin_1 = pContext->aWinContext[1][0].hWindow;

	BSTD_UNUSED(hTask);

	printf("Start BVN stress test scaling thread \n");


	/* Obtain full screen size */
	if (hVidWin_0)
	{
		BVDC_Window_GetDstRect(hVidWin_0, (int32_t*)&ulX_full_0,
			(int32_t*)&ulY_full_0, &ulWidth_full_0, &ulHeight_full_0);
	}

	if (hVidWin_1)
	{
		BVDC_Window_GetDstRect(hVidWin_1, (int32_t*)&ulX_full_1,
			(int32_t*)&ulY_full_1, &ulWidth_full_1, &ulHeight_full_1);
	}

	ulWidth_down_0  = BTST_P_MIN(pFmtInfo_0->ulWidth,	360);
	ulHeight_down_0  = BTST_P_MIN(pFmtInfo_0->ulHeight, 240);
	ulX_down_0 = BTST_P_MIN((pFmtInfo_0->ulWidth  - 360), 120);
	ulY_down_0 = BTST_P_MIN((pFmtInfo_0->ulHeight - 240), 100);

	ulWidth_down_1  = BTST_P_MIN(pFmtInfo_1->ulWidth,	360);
	ulHeight_down_1  = BTST_P_MIN(pFmtInfo_1->ulHeight, 240);
	ulX_down_1 = BTST_P_MIN((pFmtInfo_1->ulWidth  - 360), 120);
	ulY_down_1 = BTST_P_MIN((pFmtInfo_1->ulHeight - 240), 100);


	while(pContext->ulStressBvnCnt)
	{
		/* Scale down */
		if (hVidWin_0)
		{
			BVDC_Window_SetDstRect(hVidWin_0, ulX_down_0, ulY_down_0, ulWidth_down_0, ulHeight_down_0);
			BVDC_Window_SetScalerOutput(hVidWin_0, 0, 0, ulWidth_down_0, ulHeight_down_0);
			BTST_P_ApplyChanges(pContext);
		}

		if (hVidWin_1)
		{
			BVDC_Window_SetDstRect(hVidWin_1, ulX_down_1, ulY_down_1, ulWidth_down_1, ulHeight_down_1);
			BVDC_Window_SetScalerOutput(hVidWin_1, 0, 0, ulWidth_down_1, ulHeight_down_1);
			BTST_P_ApplyChanges(pContext);
		}

		BKNI_Sleep(BKNI_Sleep((rand() % 2000) + 500));

		/* Scale up */
		if (hVidWin_0)
		{
			BVDC_Window_SetDstRect(hVidWin_0, ulX_full_0, ulY_full_0, ulWidth_full_0, ulHeight_full_0);
			BVDC_Window_SetScalerOutput(hVidWin_0, 0, 0, ulWidth_full_0, ulHeight_full_0);
			BTST_P_ApplyChanges(pContext);
		}

		if (hVidWin_1)
		{
			BVDC_Window_SetDstRect(hVidWin_1, ulX_full_1, ulY_full_1, ulWidth_full_1, ulHeight_full_1);
			BVDC_Window_SetScalerOutput(hVidWin_1, 0, 0, ulWidth_full_1, ulHeight_full_1);
			BTST_P_ApplyChanges(pContext);
		}

		BKNI_Sleep(BKNI_Sleep((rand() % 2000) + 500));

	}

	printf("Exit BVN stress test scaling thread \n");
	pContext->bScaleDestEnabled = false;
}



void BTST_P_StressTestBVN
( BTST_P_Context          *pContext )
{
	int                      iStreamType_0 ;
	int                      iVideoCmprStd_0;
	uint16_t                 ui16VideoPid_0;
	uint16_t                 ui16PcrPid_0;

	int                      iStreamType_1 ;
	int                      iVideoCmprStd_1;
	uint16_t                 ui16VideoPid_1;
	uint16_t                 ui16PcrPid_1;
	int                      iScaling;
	int                      bScalingStarted = 0;
	int						 iStressLoop;
	uint32_t                 i;

	printf("Enter first channel Video Pid : ") ;
	BTST_P_Scanf(pContext, "%hx", &ui16VideoPid_0) ;
	printf("Enter first channel Pcr Pid : ") ;
	BTST_P_Scanf(pContext, "%hx", &ui16PcrPid_0) ;
	printf("Enter first channel Stream Type : TS(0),DSSES(1),PES(2),ES(3),BES(4),DTVPES(5)") ;
	BTST_P_Scanf(pContext, "%d", &iStreamType_0) ;
	printf("Enter first channel Video Compression: H.264(0), MPEG2(1), H.261 (2), H.263(3), VC1(4), MPEG1(5)") ;
	BTST_P_Scanf(pContext, "%d", &iVideoCmprStd_0) ;

	printf("Enter second channel Video Pid : ") ;
	BTST_P_Scanf(pContext, "%hx", &ui16VideoPid_1) ;
	printf("Enter second channel Pcr Pid : ") ;
	BTST_P_Scanf(pContext, "%hx", &ui16PcrPid_1) ;
	printf("Enter second channel Stream Type : TS(0),DSSES(1),PES(2),ES(3),BES(4),DTVPES(5)") ;
	BTST_P_Scanf(pContext, "%d", &iStreamType_1) ;
	printf("Enter second channel Video Compression: H.264(0), MPEG2(1), H.261 (2), H.263(3), VC1(4), MPEG1(5)") ;
	BTST_P_Scanf(pContext, "%d", &iVideoCmprStd_1) ;

	printf("Stress test with scale up/down: (1 - yes, 0 - no)\n");
	BTST_P_Scanf(pContext, "%d", &iScaling);

	printf("Stress test loop count : \n");
	BTST_P_Scanf(pContext, "%d", &iStressLoop);

	pContext->ulStressBvnCnt = iStressLoop;
	pContext->bScaleDestEnabled = false;

	while (pContext->ulStressBvnCnt)
	{
		printf("BVN stress test count: %d\n", pContext->ulStressBvnCnt);

		if(pContext->bPlayback)
		{
			for(i = 0; i < BTST_PLAYBACK_STREAM_COUNT; i++)
			{
				BTST_P_BetterMpegDecode(pContext, astPlaybackStream[i].fn,
					astPlaybackStream[i].ui16VidPid, astPlaybackStream[i].ui16PcrPid,
					astPlaybackStream[i].ui16EnhPid,
					astPlaybackStream[i].eStreamType, astPlaybackStream[i].eVideoCmprStd);

				BKNI_Sleep(BKNI_Sleep((rand() % 6000) + 800));
			}
		}
		else
		{
			pContext->eStreamType = (BAVC_StreamType) iStreamType_0;
			pContext->eVideoCmprStd = (BAVC_VideoCompressionStd) iVideoCmprStd_0;
			pContext->aSrcContext[pContext->iSelectedVidSrc].ui16VidPid = ui16VideoPid_0;
			pContext->aSrcContext[pContext->iSelectedVidSrc].ui16PcrPid = ui16PcrPid_0;

			BTST_P_BetterMpegDecode(pContext, NULL, ui16VideoPid_0, ui16PcrPid_0, 0x1FFF,
				(BAVC_StreamType) iStreamType_0, (BAVC_VideoCompressionStd) iVideoCmprStd_0);

			BKNI_Sleep(BKNI_Sleep((rand() % 3000) + 800));

			pContext->eStreamType = (BAVC_StreamType) iStreamType_1;
			pContext->eVideoCmprStd = (BAVC_VideoCompressionStd) iVideoCmprStd_1;
			pContext->aSrcContext[pContext->iSelectedVidSrc].ui16VidPid = ui16VideoPid_1;
			pContext->aSrcContext[pContext->iSelectedVidSrc].ui16PcrPid = ui16PcrPid_1;

			BTST_P_BetterMpegDecode(pContext, NULL, ui16VideoPid_1, ui16PcrPid_1, 0x1FFF,
				(BAVC_StreamType) iStreamType_1, (BAVC_VideoCompressionStd) iVideoCmprStd_1);

			BKNI_Sleep(BKNI_Sleep((rand() % 6000) + 800));
		}

		if (iScaling && !bScalingStarted)
		{
			bScalingStarted = 1;
			pContext->bScaleDestEnabled = true;
			BTST_CreateTask(&pContext->hTaskScaleDest, BTST_P_ScaleDestHandler, (void*)pContext);
		}

		pContext->ulStressBvnCnt--;

	}

	/* wait for window scaling thread to exit */
	if (bScalingStarted)
	{
		while (pContext->bScaleDestEnabled)
			BKNI_Sleep(500);

		TEST_CLOSE_HANDLE(BTST_DestroyTask, pContext->hTaskScaleDest);
	}

	printf("Exit BVN stress test\n ");
	return;
}

static void BTST_P_LipSyncTestHandler
( BTST_TaskHandle          hTask,
 void                    *pTestContext )
{
	uint32_t           ulVsyncDelay0, ulVsyncDelay1;
	BTST_P_Context    *pContext = (BTST_P_Context *)pTestContext;
	BVDC_Window_Handle hVidWin_0 = pContext->aWinContext[0][0].hWindow;
	BVDC_Window_Handle hVidWin_1 = pContext->aWinContext[1][0].hWindow;

	BSTD_UNUSED(hTask);

	printf("Start Lip Sync test monitor thread ..............\n");
	while(pContext->bEnableLipSync && hVidWin_0 && hVidWin_1)
	{
		ulVsyncDelay0 = pContext->aulVsyncDelay[0][0];
		ulVsyncDelay1 = pContext->aulVsyncDelay[1][0];

		if(pContext->bVsyncDelay)
		{
			if(ulVsyncDelay0 > ulVsyncDelay1)
			{
				BVDC_Window_SetDelayOffset(hVidWin_0, 0);
				BTST_P_ApplyChanges(pContext);

				BVDC_Window_SetDelayOffset(hVidWin_1, ulVsyncDelay0 - ulVsyncDelay1);
				BTST_P_ApplyChanges(pContext);

			}
			else if(ulVsyncDelay0 < ulVsyncDelay1)
			{
				BVDC_Window_SetDelayOffset(hVidWin_1, 0);
				BTST_P_ApplyChanges(pContext);

				BVDC_Window_SetDelayOffset(hVidWin_0, ulVsyncDelay1 - ulVsyncDelay0);
				BTST_P_ApplyChanges(pContext);
			}

			pContext->bVsyncDelay = false;
		}

		BKNI_Sleep(100);
	};

	BKNI_SetEvent(((BTST_P_Context*)pContext)->hLipSyncEvent);
	printf("Exit Lip Sync test monitor thread \n");

}

void BTST_P_LipSyncStressTest
( BTST_P_Context          *pContext )
{
	/* toggle it */
	pContext->bEnableLipSync ^= true;

	if(pContext->bEnableLipSync)
	{
		printf("start lip sync test \n");

		BKNI_CreateEvent(&pContext->hLipSyncEvent);
		BKNI_ResetEvent(pContext->hLipSyncEvent);
		BTST_CreateTask(&pContext->hTaskLipSync, BTST_P_LipSyncTestHandler, (void*)pContext);
	}
	else
	{
		printf("stop lip sync test\n");
		BKNI_WaitForEvent(pContext->hLipSyncEvent, 1000);

		TEST_CLOSE_HANDLE(BKNI_DestroyEvent, pContext->hLipSyncEvent);
		TEST_CLOSE_HANDLE(BTST_DestroyTask, pContext->hTaskLipSync);
	}
	return;
}

void BTST_P_StressTestAutoFormat
( BTST_P_Context          *pContext )
{
	uint32_t                 i;
	int                      iStressLoop;
	BVDC_Source_Handle       hVidSrc;
	BFMT_VideoFmt            eSrcFmt;
	const BFMT_VideoInfo    *pSrcFmt;

	printf("Stress test loop count : \n");
	BTST_P_Scanf(pContext, "%d", &iStressLoop);

	pContext->ulStressBvnCnt = iStressLoop;
	hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;

	while (pContext->ulStressBvnCnt)
	{
		printf("AutoFormat stress test count: %d\n", pContext->ulStressBvnCnt);

		for(i = 0; i < BTST_PLAYBACK_STREAM_COUNT; i++)
		{
			BTST_P_BetterMpegDecode(pContext, astPlaybackStream[i].fn,
				astPlaybackStream[i].ui16VidPid, astPlaybackStream[i].ui16PcrPid,
				astPlaybackStream[i].ui16EnhPid,
				astPlaybackStream[i].eStreamType, astPlaybackStream[i].eVideoCmprStd);

			BVDC_Source_GetVideoFormat(hVidSrc, &eSrcFmt);
			pContext->apFmtInfo[0] = pSrcFmt = BFMT_GetVideoFormatInfoPtr(eSrcFmt);

			BVDC_Display_SetVideoFormat(pContext->ahDisplay[0], eSrcFmt);
			BVDC_Window_SetDstRect(pContext->aWinContext[0][0].hWindow,
				0, 0, pSrcFmt->ulWidth, pSrcFmt->ulHeight);
			BVDC_Window_SetScalerOutput(pContext->aWinContext[0][0].hWindow,
				0, 0, pSrcFmt->ulWidth, pSrcFmt->ulHeight);
			BTST_P_ApplyChanges(pContext);

			BKNI_Sleep(BKNI_Sleep((rand() % 3000) + 800));

		}

		pContext->ulStressBvnCnt--;
	}

	printf("Exit Autoformat stress test\n ");
	return;
}


void BTST_P_StressTestResizeFormat
( BTST_P_Context          *pContext )
{
	bool                     bResize = false;
	int                      iStressLoop;
	uint32_t                 ulChannelId, ulFormatId, ulFmtCnt,iSdComp;
	BVDC_Source_Handle       hVidSrc;
	BVDC_Display_Handle      hDisplay;
	BVDC_Window_Handle       hHdWindow, hSdWindow;
	const BFMT_VideoInfo    *pHdFmt, *pSdFmt;
	BFMT_VideoFmt            aeVideoFmts[] =
	{
		BFMT_VideoFmt_eNTSC, BFMT_VideoFmt_e480p,
		BFMT_VideoFmt_e720p, BFMT_VideoFmt_e1080i,
		BFMT_VideoFmt_ePAL_G
	};

	iSdComp = (pContext->iSelectedComp + 1);
	iSdComp = (iSdComp <pContext->stCapabilities.ulNumCmp)?iSdComp:(pContext->stCapabilities.ulNumCmp-1);
	hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	hDisplay = pContext->ahDisplay[pContext->iSelectedComp];
	hHdWindow = pContext->aWinContext[pContext->iSelectedComp][0].hWindow;
	hSdWindow = pContext->aWinContext[iSdComp][0].hWindow;
	hSdWindow = (iSdComp==pContext->iSelectedComp)?NULL:hSdWindow;

	pSdFmt= pContext->apFmtInfo[iSdComp];
	ulFmtCnt = sizeof(aeVideoFmts) / sizeof(BFMT_VideoFmt);

	printf("Stress test loop count : \n");
	BTST_P_Scanf(pContext, "%d", &iStressLoop);
	pContext->ulStressBvnCnt = iStressLoop;

	while (pContext->ulStressBvnCnt)
	{
		printf("Resize Format stress test count: %d\n", pContext->ulStressBvnCnt);

		/* Channel change */
		ulChannelId = rand() % BTST_PLAYBACK_STREAM_COUNT;
		BTST_P_BetterMpegDecode(pContext, astPlaybackStream[ulChannelId].fn,
			astPlaybackStream[ulChannelId].ui16VidPid, astPlaybackStream[ulChannelId].ui16PcrPid,
			astPlaybackStream[ulChannelId].ui16EnhPid,
			astPlaybackStream[ulChannelId].eStreamType, astPlaybackStream[ulChannelId].eVideoCmprStd);

		BKNI_Sleep(rand() % 300);

		/* Format change */
		ulFormatId = rand() % ulFmtCnt;
		BVDC_Display_SetVideoFormat(hDisplay, aeVideoFmts[ulFormatId]);
		pContext->apFmtInfo[pContext->iSelectedComp] = pHdFmt = BFMT_GetVideoFormatInfoPtr(aeVideoFmts[ulFormatId]);
		printf("New display format %s chanId %d bReSize %d \n", pHdFmt->pchFormatStr, ulChannelId, bResize);

		/* Resize */
		if(bResize)
		{
			BVDC_Window_SetDstRect(hHdWindow,
				pHdFmt->ulWidth/4, pHdFmt->ulHeight/4,
				pHdFmt->ulWidth/2, pHdFmt->ulHeight/2);
			BVDC_Window_SetScalerOutput(hHdWindow,
				0, 0, pHdFmt->ulWidth/2, pHdFmt->ulHeight/2);

			if(hSdWindow)
			{
				BVDC_Window_SetDstRect(hSdWindow,
					pSdFmt->ulWidth/4, pSdFmt->ulHeight/4,
					pSdFmt->ulWidth/2, pSdFmt->ulHeight/2);
				BVDC_Window_SetScalerOutput(hSdWindow,
					0, 0, pSdFmt->ulWidth/2, pSdFmt->ulHeight/2);
			}
		}
		else
		{
			BVDC_Window_SetDstRect(hHdWindow,
				0, 0, pHdFmt->ulWidth, pHdFmt->ulHeight);
			BVDC_Window_SetScalerOutput(hHdWindow,
				0, 0, pHdFmt->ulWidth, pHdFmt->ulHeight);

			if(hSdWindow)
			{
				BVDC_Window_SetDstRect(hSdWindow,
					0, 0, pSdFmt->ulWidth, pSdFmt->ulHeight);
				BVDC_Window_SetScalerOutput(hSdWindow,
					0, 0, pSdFmt->ulWidth, pSdFmt->ulHeight);
			}
		}
		BTST_P_ApplyChanges(pContext);

		bResize = !bResize;
		BKNI_Sleep(rand() % 3000 + 1000);

		pContext->ulStressBvnCnt--;
	}

	printf("Exit Resize and format change stress test\n ");
	return;
}




void BTST_P_StressTest3DVideo
( BTST_P_Context          *pContext )
{
	uint32_t                 ulWidth, ulHeight, ulStressLoop;
	BFMT_Orientation         eSrcOrientation, eDispOrientation;
	BVDC_Window_Handle       hVidWin;
	BVDC_Source_Handle       hVidSrc;
	BVDC_Display_Handle      hDisplay;
	const BFMT_VideoInfo    *pFmtInfo;

	printf("Stress test loop count : \n");
	BTST_P_Scanf(pContext, "%d", &ulStressLoop);

	hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	hDisplay = pContext->ahDisplay[pContext->iSelectedComp];
	hVidWin = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	pFmtInfo = pContext->apFmtInfo[pContext->iSelectedComp];

	while (ulStressLoop)
	{

		eSrcOrientation = rand() % 3;
		BVDC_Source_SetOrientation(hVidSrc, true, eSrcOrientation);
		BTST_P_ApplyChanges(pContext);

		BKNI_Sleep(BKNI_Sleep((rand() % 6000) + 500));

		eDispOrientation = rand() % 3;
		printf("\n3D stress test[%d]: SrcOrientation: %d, DispOrientation: %d",
			ulStressLoop, eSrcOrientation, eDispOrientation);

		if(eDispOrientation == BFMT_Orientation_e3D_OverUnder)
		{
			ulWidth  = pFmtInfo->ulWidth;
			ulHeight = pFmtInfo->ulHeight/2;
		}
		else if(eDispOrientation == BFMT_Orientation_e3D_LeftRight)
		{
			ulWidth  = pFmtInfo->ulWidth/2;
			ulHeight = pFmtInfo->ulHeight;
		}
		else
		{
			ulWidth  = pFmtInfo->ulWidth;
			ulHeight = pFmtInfo->ulHeight;
		}
		BVDC_Display_SetOrientation(hDisplay, eDispOrientation);
		BVDC_Window_SetDstRect(hVidWin, 0, 0, ulWidth, ulHeight);
		BVDC_Window_SetScalerOutput(hVidWin, 0, 0, ulWidth, ulHeight);
		BTST_P_ApplyChanges(pContext);

		BKNI_Sleep(BKNI_Sleep((rand() % 6000) + 800));

		ulStressLoop--;

	}

	printf("\nExit 3D stress test\n ");
	return;
}

void BTST_P_StressTestCustomFormat
( BTST_P_Context          *pContext )
{
	static BFMT_VideoInfo s_stFormatInfoCustom =
	{
		BFMT_VideoFmt_eCustom2,
		352,
		288,
		352,
		288,
		352,
		288,
		0,
		0,
		0,
		0,
		BFMT_VERT_50Hz,
		5000,
		BFMT_PXL_27MHz,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D,
		2700,
		"BFMT_VideoFmt_eCustom2",
		NULL
	};
	BVDC_Window_Handle hWindow = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	BVDC_Display_Handle hDisplay  = pContext->ahDisplay[pContext->iSelectedComp];
	BTST_MDEC_ChannelHandle hMpgChn;
	int i;
	BVDC_Window_Status stWinStatus;
	uint32_t              ulMaxCount, ulWidth, ulHeight;
	const  uint32_t ulMinHSize = 64;
	const  uint32_t ulMinVSize = 64;
	const  uint32_t ulMaxHSize = 1920;
	const  uint32_t ulMaxVSize = 1080;
	bool   bResize = false;

	BVDC_DisplayTg  eMasterTg   = pContext->eMasterTg[pContext->iSelectedComp];

	if (BTST_P_USED_STG(eMasterTg))
	{
		printf("\n random resultion change times \n");
		BTST_P_Scanf(pContext, "%d", &ulMaxCount);


		while(ulMaxCount--)
		{

			/* Get the random width and height */
			ulWidth   = (rand()%(ulMaxHSize+1))& (~0xf);/* MB aligned*/
			ulWidth  += (ulWidth<ulMinHSize)?ulMinHSize:0;
			ulHeight  = (rand()%(ulMaxVSize+1))& (~0xf);/* MB aligned*/
			ulHeight += (ulHeight<ulMinVSize)?ulMinVSize:0;


			s_stFormatInfoCustom.bInterlaced        = false;
			s_stFormatInfoCustom.ulDigitalWidth     = ulWidth;
			s_stFormatInfoCustom.ulDigitalHeight    = ulHeight;
			s_stFormatInfoCustom.ulWidth            = ulWidth;
			s_stFormatInfoCustom.ulHeight           = ulHeight;
			s_stFormatInfoCustom.ulScanWidth        = ulWidth;
			s_stFormatInfoCustom.ulScanHeight       = ulHeight;
			s_stFormatInfoCustom.ulVertFreq         = 3000;
			s_stFormatInfoCustom.ulPxlFreq          = ulWidth*ulHeight*3000 / (1000000/BFMT_FREQ_FACTOR);
			s_stFormatInfoCustom.ulPxlFreqMask      = BFMT_PXL_27MHz |BFMT_PXL_27MHz_MUL_1_001;
			s_stFormatInfoCustom.eAspectRatio       = BFMT_AspectRatio_eSAR;
			printf("\n iteration %d  resolution %d x %d", ulMaxCount, ulWidth, ulHeight);


			/* only set refresh rate for xvd with sync lock window */
			for(i = 0; i < BTST_P_MAX_WINDOW; i++)
			{
				hWindow = pContext->aWinContext[pContext->iSelectedComp][i].hWindow;
				if(hWindow)
				{
					BVDC_Window_GetStatus(hWindow, &stWinStatus);
					if(stWinStatus.bSyncLock == true)
					{
						BTST_MDEC_Id eSelectedMDec = pContext->aulSelectedMDec[pContext->iSelectedComp][i];
						uint32_t iSelectedMDecCh = pContext->aulSelectedMDecCh[pContext->iSelectedComp][i];
						hMpgChn = pContext->astMDecInfo[eSelectedMDec].ahMDecChn[iSelectedMDecCh];
						if (IS_XVD(eSelectedMDec) && hMpgChn)
						{
#if (BFramework_NUM_XVD_CORES > 0) && !BTST_VDC_ONLY
							BXVD_SetMonitorRefreshRate((BXVD_ChannelHandle)hMpgChn, BXVD_MONITOR_REFRESH_RATE_30Hz);
#endif
						}
					}
				}
			}

			hWindow = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
			BDBG_ASSERT(hWindow);
			BVDC_Display_SetCustomVideoFormat(hDisplay, &s_stFormatInfoCustom);
			pContext->apFmtInfo[pContext->iSelectedComp] = &s_stFormatInfoCustom;



			/* Resize */
			if(bResize)
			{
				BVDC_Window_SetDstRect(hWindow,
					ulWidth>>2, ulHeight>>2,
					ulWidth>>1, ulHeight>>1);
				BVDC_Window_SetScalerOutput(hWindow,
					0, 0, ulWidth>>1, ulHeight>>1);
			}
			else
			{
				BVDC_Window_SetDstRect(hWindow,
					0, 0,
					ulWidth, ulHeight);
				BVDC_Window_SetScalerOutput(hWindow,
					0, 0, ulWidth, ulHeight);
			}

			bResize = !bResize;

			BTST_P_ApplyChanges(pContext);
			BKNI_Sleep(3000);
		}
	}
}
/***************************************************************************
*
*/
void BTST_P_StressCustomFmtResize
( BTST_P_Context          *pContext )
{
	uint32_t    ulLeft, ulTop, ulDispWidth, ulDispHeight;
	uint32_t    ulMinWidth, ulMinHeight, ulMaxWidth, ulMaxHeight;
	uint32_t    ulWidth, ulHeight, ulDeltaWidth, ulDeltaHeight;
	bool        bScaleDown = true;

	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetScalerOutput(hVidWin, &ulLeft, &ulTop, &ulDispWidth, &ulDispHeight);
	printf("window width = %d, height = %d\n", ulDispWidth, ulDispHeight);

	printf("Enter minimum tile size: (w h): ");
	BTST_P_Scanf(pContext, "%d %d", &ulMinWidth, &ulMinHeight);
	printf("Enter delta size: (w h): ");
	BTST_P_Scanf(pContext, "%d %d", &ulDeltaWidth, &ulDeltaHeight);

	ulMaxWidth = ulDispWidth ;
	ulMaxHeight = ulDispHeight;
	ulWidth = ulMaxWidth - ulDeltaWidth;
	ulHeight = ulMaxHeight - ulDeltaHeight;

	while((ulHeight != ulMaxHeight) || (ulWidth  != ulMaxWidth))
	{
		printf("Current (w, h) = %u %u\n", ulWidth, ulHeight);

		ulLeft = (ulMaxWidth - ulWidth)>>1;
		ulTop  = (ulMaxHeight - ulHeight) >> 1;
		

		BVDC_Window_SetDstRect(hVidWin, ulLeft, ulTop, ulWidth, ulHeight);
		BVDC_Window_SetScalerOutput(hVidWin,
						0, 0, ulWidth, ulHeight);
		BTST_P_ApplyChanges(pContext);
		BKNI_Sleep(1000);

		if(bScaleDown)
		{
			if((ulWidth - ulDeltaWidth) >= ulMinWidth)
				ulWidth -= ulDeltaWidth;
			if((ulHeight - ulDeltaHeight) >= ulMinHeight)
				ulHeight -= ulDeltaHeight;
		}
		else
		{
			if((ulWidth + ulDeltaWidth) <= ulMaxWidth)
				ulWidth += ulDeltaWidth;
			if((ulHeight + ulDeltaHeight) <= ulMaxHeight)
				ulHeight += ulDeltaHeight;
		}

		if(((ulWidth - ulDeltaWidth) < ulMinWidth) &&
			((ulHeight - ulDeltaHeight) < ulMinHeight))
		{
			bScaleDown = false;
		}
	}

done:
	return;
}
void BTST_P_CapturePicture
( BTST_P_Context		  *pContext )
{
	BVDC_Window_Handle hVidWin = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	BVDC_Window_Handle hGfxWin = NULL;
	BVDC_Source_Handle hGfxSrc = NULL;
	BVDC_Compositor_Handle hCompositor = pContext->ahCompositor[pContext->iSelectedComp];
	BAVC_Gfx_Picture           stGFXPicSetting;
	int iSelectedWin = 2;
	uint32_t ulWidth, ulHeight;
	static bool bGotPic;
	bool bGotBuffer = false;

	/* Toggle */
	pContext->bEnableCapturePicture ^= true;

	if(pContext->bEnableCapturePicture)
	{
		pContext->pic.hCaptureBuffer = NULL;
		BVDC_Window_SetUserCaptureBufferCount(hVidWin, 2);

		BTST_P_ApplyChanges(pContext);

		while (!bGotBuffer)
		{
			if (BVDC_Window_GetBuffer(hVidWin, &pContext->pic) == BERR_SUCCESS)
			{
				bGotBuffer = true;
			}

			BKNI_Sleep(100);
		}

		if (pContext->pic.hCaptureBuffer)
		{
			if(pContext->bShowCapPic)
			{
				uint32_t ulCompWidth = 0, ulCompHeight = 0;
				BSUR_Surface_Handle hCompressSurface = 0;
				uint32_t bEnable = 0;
				BPXL_Format eFormat;

				/* Source surface */
				BSUR_Surface_GetDimensions(pContext->pic.hCaptureBuffer, &ulWidth, &ulHeight);
				BSUR_Surface_GetFormat(pContext->pic.hCaptureBuffer, &eFormat );
				printf("capture surface size: %d %d \n", ulWidth, ulHeight);

#if (BTST_P_SUPPORT_GRC)
				/* create compressed surface */
				printf( "Enable compression: Disable(0), Enable(1)" );
				BTST_P_Scanf(pContext,  "%u", &bEnable);
				if(bEnable)
				{
					BGFX_Rect stRect;
					BSUR_Surface_Settings SurfaceSettings;
					uint32_t ulBitsPerPixel, ulPredictionMode;

					printf( "Bits per pixel: " );
					BTST_P_Scanf(pContext,  "%u", &ulBitsPerPixel );
					printf( "Prediction mode (0.Normal, 1.NOLEFT, 2.NO2LEFT): " );
					BTST_P_Scanf(pContext,  "%u", &ulPredictionMode );
					printf( "Width: " );
					BTST_P_Scanf(pContext,  "%u", &ulCompWidth );
					printf( "Height: " );
					BTST_P_Scanf(pContext,  "%u", &ulCompHeight );

					if(!pContext->hGrc)
					{
						BGRC_Open( &pContext->hGrc, pContext->hChp, pContext->hReg,
							pContext->astMemInfo[0].hMem, pContext->hInt, NULL );
					}

					BSUR_Surface_GetDefaultSettings( &SurfaceSettings );
					SurfaceSettings.stTestFeature1Settings.bEnable = (bool)bEnable;
					SurfaceSettings.stTestFeature1Settings.ulBitsPerPixel = ulBitsPerPixel;
					SurfaceSettings.stTestFeature1Settings.ulPredictionMode = ulPredictionMode;

					BSUR_Surface_Create( pContext->astMemInfo[0].hMem, ulCompWidth, ulCompHeight,
						0, NULL, BPXL_eA8_R8_G8_B8, 0, BSUR_CONSTRAINT_DCX_TESTFEATURE1_FORMAT,
						&SurfaceSettings, &hCompressSurface );

					BKNI_Sleep(2000);

					/* compression blit */
					stRect.ulX = stRect.ulY = stRect.ulWidth = stRect.ulHeight = 0;

					if((ulWidth != ulCompWidth) || (ulHeight != ulCompHeight))
					{
						BSUR_Surface_Handle hTempSurface = 0;

						BSUR_Surface_Create( pContext->astMemInfo[0].hMem,
							ulCompWidth, ulCompHeight, 0, NULL, BPXL_eA8_R8_G8_B8, 0, 0,
							NULL, &hTempSurface );

						/* Blt from uncompressed surface to uncompressed surface with different size */
						BGRC_ResetState( pContext->hGrc );
						BGRC_Source_SetSurface( pContext->hGrc, pContext->pic.hCaptureBuffer );
						BGRC_Output_SetSurface( pContext->hGrc, hTempSurface );
						BGFX_Blit_ColorSpaceConvert(pContext->hGrc, pContext->pic.hCaptureBuffer,
							hTempSurface, &stRect, &stRect, 0xffffffff);
						printf( "Done with M2MC blit from uncompressed surface to uncompressed surface\n" );

						BKNI_Sleep(2000);
						/* Blt from uncompressed surface to compressed surface */
						BGRC_ResetState( pContext->hGrc );
						BGRC_Source_SetSurface( pContext->hGrc, hTempSurface);
						BGRC_Output_SetSurface( pContext->hGrc, hCompressSurface );
						BGRC_IssueStateAndWait( pContext->hGrc );
						printf( "Done with M2MC blit from uncompressed surface to compressed surface\n" );
					}
					else
					{
						/* Blt from uncompressed surface to compressed surface */
						BGRC_ResetState( pContext->hGrc );
						BGRC_Source_SetSurface( pContext->hGrc, pContext->pic.hCaptureBuffer );
						BGRC_Output_SetSurface( pContext->hGrc, hCompressSurface );
						BGFX_Blit_ColorSpaceConvert(pContext->hGrc, pContext->pic.hCaptureBuffer,
							hCompressSurface, &stRect, &stRect, 0xffffffff);
						printf( "Done with M2MC blit from uncompressed surface to compressed surface\n" );
					}
				}
#endif

				BVDC_Source_Create(pContext->hVdc, &hGfxSrc,
					(pContext->iSelectedComp) ? BAVC_SourceId_eGfx1 : BAVC_SourceId_eGfx0, NULL);
				BVDC_Window_Create(hCompositor, &hGfxWin, BVDC_WindowId_eGfx0, hGfxSrc, NULL);
				BVDC_Window_SetAlpha(hGfxWin, 0x80);
				BVDC_Window_SetBlendFactor (hGfxWin, BVDC_BlendFactor_eConstantAlpha,
					BVDC_BlendFactor_eOneMinusSrcAlpha, BVDC_ALPHA_MAX);
				BVDC_Window_SetZOrder(hGfxWin, iSelectedWin);
				BVDC_Window_SetVisibility(hGfxWin, true);

				stGFXPicSetting.eInOrientation = BFMT_Orientation_e2D;
				if(bEnable)
				{
					BVDC_Window_SetScalerOutput(hGfxWin, 0, 0, ulCompWidth, ulCompHeight);
					BVDC_Window_SetDstRect(hGfxWin, 0, 0, ulCompWidth, ulCompHeight);
					stGFXPicSetting.hSurface = hCompressSurface;
					BVDC_Source_SetSurface(hGfxSrc, &stGFXPicSetting);
				}
				else
				{
					BVDC_Window_SetScalerOutput(hGfxWin, 0, 0, ulWidth, ulHeight);
					BVDC_Window_SetDstRect(hGfxWin, 0, 0, ulWidth, ulHeight);

					stGFXPicSetting.hSurface = pContext->pic.hCaptureBuffer;
					BVDC_Source_SetSurface(hGfxSrc, &stGFXPicSetting);
				}

				BTST_P_ApplyChanges(pContext);
				pContext->hUserCaptureGfxSource = hGfxSrc;
				pContext->hUserCaptureGfxWin = hGfxWin;

			}
			else
			{
				/* Do we want to dump to file for MemView-ing! */
				printf("Dumping capture memory to file!\n");
				BTST_DumpToMemViewFile(pContext->pic.hCaptureBuffer, "capmem.mem");

				/* Return the buffer */
				BVDC_Window_ReturnBuffer(hVidWin, &pContext->pic);
				BVDC_Window_SetUserCaptureBufferCount(hVidWin, 0);
				BTST_P_ApplyChanges(pContext);
			}
			bGotPic = true;
		}
		else
		{
			bGotPic = false;
			printf("No captured picture\n");
		}
	}
	else
	{
		if((bGotPic) && (pContext->bShowCapPic))
		{
			BVDC_Window_SetVisibility(pContext->hUserCaptureGfxWin, false);
			BVDC_Window_Destroy(pContext->hUserCaptureGfxWin);
			BVDC_Source_Destroy(pContext->hUserCaptureGfxSource);
			BVDC_Window_ReturnBuffer(hVidWin, &pContext->pic);
			BVDC_Window_SetUserCaptureBufferCount(hVidWin, 0);
			BTST_P_ApplyChanges(pContext);
		}
		else
		{
			printf("no buffer to return\n");
		}
	}

	return;
}

void BTST_P_ShowCapPic
( BTST_P_Context          *pContext )
{
	/* toggle flag */
	pContext->bShowCapPic ^= true;
	return;
}

void BTST_P_UseStaticBuffer
( BTST_P_Context          *pContext )
{
	/* toggle flag */
	pContext->bStatic[pContext->iSelectedVidSrc] ^= true;
	return;
}

void BTST_P_GraphicsFormat
( BTST_P_Context          *pContext )
{
	uint32_t ans;
	printf("\n Input Graphics Format (0) A_0 (1)P_0 (2)RGB565\n");
	BTST_P_Scanf(pContext, "%d", &ans);

	/* graphics surface format */
	switch (ans)
	{
		case 0: pContext->eGraphicsFormat = BPXL_eA0; break;
		case 1: pContext->eGraphicsFormat = BPXL_eP0; break;
		case 2:
		default:
			pContext->eGraphicsFormat = BPXL_eR5_G6_B5; break;
	}
}
void BTST_P_UseGfxForMfd
( BTST_P_Context          *pContext )
{
	if (pContext->ulGfx2Mfd[pContext->iSelectedVidSrc])
	{
		printf("Toggle off gfx_to_mfd\n");
		pContext->ulGfx2Mfd[pContext->iSelectedVidSrc] = false;
	}
	else
	{
		printf("Toggle on gfx_to_mfd\n");
		pContext->ulGfx2Mfd[pContext->iSelectedVidSrc] = true;
	}

	return;
}

void BTST_P_InterceptFrame
( BTST_P_Context          *pContext )
{
	BSTD_UNUSED(pContext);
	g_bInterceptFrame = true;
	return;
}

void BTST_P_GlobalLevel
(
 BTST_P_Context          *pContext
 )
{
#if BDBG_DEBUG_BUILD
	int iDbgLevel;
	static bool bFlag = false;

	BSTD_UNUSED(pContext);
	bFlag ^= true;
	if(bFlag)
	{
		printf("Trace(1) Message(2) Warning(3) Error(4)\n");
		printf("Which debug level do you want to set it to? ");
		BTST_P_Scanf(pContext, "%d", &iDbgLevel);
		if(iDbgLevel > 4)
		{
			printf("Wrong debug level! reset to Error level.\n");
			iDbgLevel = 4;
		}

		BDBG_SetLevel((BDBG_Level) iDbgLevel);
	}
	else
	{
		BDBG_SetLevel(BDBG_eWrn);
	}
#else
	BSTD_UNUSED(pContext);
#endif
}

void BTST_P_DbgLevel
(
 BTST_P_Context          *pContext
 )
{
#if BDBG_DEBUG_BUILD
	int         iDbgModule = 0;
	int         iDbgLevel = 0;
	static bool bFlag = false;
	static char achName[256];
	static char achInput[256];
	static bool bCustom = false;

	BSTD_UNUSED(pContext);
	bFlag ^= true;
	if(bFlag)
	{
		while(true)
		{
			printf("\n");
			printf("(0)  VDC     (1)  SRC    (2)  CAP     (3)  SCL    (4)  WIN\n");
			printf("(5)  CMP     (6)  Test   (7)  DISP    (8)  MVD    (9)  MAD\n");
			printf("(10) PRIV    (11) BUF    (12) VNET    (13) VDEC   (14) RDC\n");
			printf("(15) HDDVI   (16) XVD    (17) VFD     (18) DBG    (19) DNR\n");
			printf("(20) PEP     (21) HEAP   (22) HDMI RX (23) GFX    (24) MRC\n");
			printf("(25) RTS     (26) CSC    (27) ANR     (28) HSCL   (29) RDCBO\n");
			printf("(30) CM3D    (31) HIST   (32) BMEM    (33) HDMI TX\n");
			printf("(34) customized name\n");

			while(true)
			{
				printf("Which module do you want to set the debug level? (99 - Exit)");
				BTST_P_Scanf(pContext, "%s", achInput);
				/* make sure string really is 0 */
				if (!strcmp(achInput, "0") || atoi(achInput))
				{
					iDbgModule = atoi(achInput);
					break;
				}
			}

			if(iDbgModule == 99) break;

			while(true)
			{
				printf("Trace(0) Message(1) Warning(2) Error(3)\n");
				printf("Which debug level do you want to set it to? ");
				BTST_P_Scanf(pContext, "%s", &achInput);
				/* make sure string really is 0 */
				if (!strcmp(achInput, "0") || atoi(achInput))
				{
					iDbgLevel = atoi(achInput);
					break;
				}
			}

			switch(iDbgLevel)
			{
			case 0:
				iDbgLevel = BDBG_eTrace;
				break;
			case 1:
				iDbgLevel = BDBG_eMsg;
				break;
			case 2:
				iDbgLevel = BDBG_eWrn;
				break;
			case 3:
			default:
				iDbgLevel = BDBG_eErr;
				break;
			}

			switch(iDbgModule)
			{
			case 0:
				BDBG_SetModuleLevel("BVDC", (BDBG_Level) iDbgLevel); break;
			case 1:
				BDBG_SetModuleLevel("BVDC_SRC", (BDBG_Level) iDbgLevel); break;
			case 2:
				BDBG_SetModuleLevel("BVDC_CAP", (BDBG_Level) iDbgLevel); break;
			case 3:
				BDBG_SetModuleLevel("BVDC_SCL", (BDBG_Level) iDbgLevel); break;
			case 4:
				BDBG_SetModuleLevel("BVDC_WIN", (BDBG_Level) iDbgLevel); break;
			case 5:
				BDBG_SetModuleLevel("BVDC_CMP", (BDBG_Level) iDbgLevel); break;
			case 6:
				BDBG_SetModuleLevel("vdc_test", (BDBG_Level) iDbgLevel);
				BDBG_SetModuleLevel("static_buffer", (BDBG_Level) iDbgLevel);
				BDBG_SetModuleLevel("BTST", (BDBG_Level) iDbgLevel); break;
			case 7:
				BDBG_SetModuleLevel("BVDC_DISP", (BDBG_Level) iDbgLevel); break;
			case 8:
				BDBG_SetModuleLevel("mvd", (BDBG_Level) iDbgLevel); break;
			case 9:
				BDBG_SetModuleLevel("BVDC_MAD", (BDBG_Level) iDbgLevel);
				BDBG_SetModuleLevel("BVDC_MCVP", (BDBG_Level) iDbgLevel);
				BDBG_SetModuleLevel("BVDC_MCDI", (BDBG_Level) iDbgLevel); break;
			case 10:
				BDBG_SetModuleLevel("BVDC_PRIV", (BDBG_Level) iDbgLevel); break;
			case 11:
				BDBG_SetModuleLevel("BVDC_BUF", (BDBG_Level) iDbgLevel);
				BDBG_SetModuleLevel("BVDC_HEAP", (BDBG_Level) iDbgLevel); break;
			case 12:
				BDBG_SetModuleLevel("VNET", (BDBG_Level) iDbgLevel); break;
			case 13:
				BDBG_SetModuleLevel("BVDC_VDEC", (BDBG_Level) iDbgLevel); break;
			case 14:
				BDBG_SetModuleLevel("BRDC", (BDBG_Level) iDbgLevel);
				BDBG_SetModuleLevel("BRDC_DBG", (BDBG_Level) iDbgLevel); break;
			case 15:
				BDBG_SetModuleLevel("BVDC_HDDVI", (BDBG_Level) iDbgLevel); break;
			case 16:
				BDBG_SetModuleLevel("BXVD", (BDBG_Level) iDbgLevel);
				BDBG_SetModuleLevel("BXVD_PRIV", (BDBG_Level) iDbgLevel);
				BDBG_SetModuleLevel("dmgr", (BDBG_Level) iDbgLevel); break;
			case 17:
				BDBG_SetModuleLevel("BVDC_VFD", (BDBG_Level) iDbgLevel); break;
			case 18:
				BDBG_SetModuleLevel("BVDC_DBG", (BDBG_Level) iDbgLevel); break;
			case 19:
				BDBG_SetModuleLevel("BVDC_DNR", (BDBG_Level) iDbgLevel); break;
			case 20:
				BDBG_SetModuleLevel("BVDC_PEP", (BDBG_Level) iDbgLevel); break;
			case 21:
				BDBG_SetModuleLevel("BVDC_HEAP", (BDBG_Level) iDbgLevel); break;
			case 22:
				BDBG_SetModuleLevel("BLVD", (BDBG_Level) iDbgLevel);
				BDBG_SetModuleLevel("BHDR", (BDBG_Level)iDbgLevel);
				BDBG_SetModuleLevel("BHDR_FE", (BDBG_Level)iDbgLevel); break;
			case 23:
				BDBG_SetModuleLevel("BVDC_GFX", (BDBG_Level) iDbgLevel); break;
			case 24:
				BDBG_SetModuleLevel("BMRC_MONITOR", (BDBG_Level) iDbgLevel); break;
			case 25:
				BDBG_SetModuleLevel("BVDC_RTS", (BDBG_Level)iDbgLevel); break;
			case 26:
				BDBG_SetModuleLevel("BVDC_CSC", (BDBG_Level)iDbgLevel); break;
			case 27:
				BDBG_SetModuleLevel("BVDC_ANR", (BDBG_Level)iDbgLevel); break;
			case 28:
				BDBG_SetModuleLevel("BVDC_HSCL", (BDBG_Level)iDbgLevel); break;
			case 29:
				BDBG_SetModuleLevel("BRDC_BLOCKOUT", (BDBG_Level)iDbgLevel); break;
			case 30:
				BDBG_SetModuleLevel("BVDC_CM3D", (BDBG_Level)iDbgLevel); break;
			case 31:
				BDBG_SetModuleLevel("BVDC_HIST", (BDBG_Level)iDbgLevel); break;
			case 32:
				BDBG_SetModuleLevel("BMEM", (BDBG_Level)iDbgLevel); break;
			case 33:
				BDBG_SetModuleLevel("BHDM", (BDBG_Level) iDbgLevel);
				BDBG_SetModuleLevel("BHDM_EDID", (BDBG_Level) iDbgLevel); break;
			case 34:
				printf("\nPlease enter the debug module name: ");
				scanf("%s", achName);
				bCustom = true;
				BDBG_SetModuleLevel(achName, (BDBG_Level)iDbgLevel); break;
			default:
				printf("Won't reach here\n");
			}
		}
	}
	else
	{
		BDBG_SetModuleLevel("BTST",          BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC",          BDBG_eWrn);
		BDBG_SetModuleLevel("VNET",          BDBG_eWrn);
		BDBG_SetModuleLevel("BRDC",          BDBG_eWrn);
		BDBG_SetModuleLevel("BRDC_DBG",      BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_BUF",      BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_HEAP",     BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_CAP",      BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_CMP",      BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_DISP",     BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_HDDVI",    BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_MAD",      BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_MCVP",     BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_MCDI",     BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_PRIV",     BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_SCL",      BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_HSCL",     BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_SRC",      BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_VDEC",     BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_WIN",      BDBG_eWrn);
		BDBG_SetModuleLevel("vdc_test",  	 BDBG_eWrn);
		BDBG_SetModuleLevel("static_buffer", BDBG_eWrn);
		BDBG_SetModuleLevel("BXVD",          BDBG_eWrn);
		BDBG_SetModuleLevel("BXVD_PRIV",     BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_VFD",      BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_DBG",      BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_DNR",      BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_PEP",      BDBG_eWrn);
		BDBG_SetModuleLevel("BHDM",          BDBG_eWrn);
		BDBG_SetModuleLevel("BHDM_EDID",     BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_GFX",      BDBG_eWrn);
		BDBG_SetModuleLevel("BMRC_MONITOR",  BDBG_eWrn);
		BDBG_SetModuleLevel("dmgr",          BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_HEAP",     BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_RTS",      BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_CSC",      BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_DNR",      BDBG_eWrn);
		BDBG_SetModuleLevel("BHDM",          BDBG_eWrn);
		BDBG_SetModuleLevel("BLVD",          BDBG_eWrn);
		BDBG_SetModuleLevel("BRDC_BLOCKOUT", BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_CM3D",     BDBG_eWrn);
		BDBG_SetModuleLevel("BVDC_HIST",     BDBG_eWrn);
		BDBG_SetModuleLevel("BHDR",          BDBG_eWrn);
		BDBG_SetModuleLevel("BHDR_FE",       BDBG_eWrn);
		if(bCustom)
		{
			BDBG_SetModuleLevel(achName,         BDBG_eWrn);
			bCustom = false;
		}
	}
#else
	BSTD_UNUSED(pContext);
#endif
}


void BTST_P_Sleep
( BTST_P_Context          *pContext )
{
	BSTD_UNUSED(pContext);
	BKNI_Sleep(1000);
}

void BTST_P_Break
( BTST_P_Context		  *pContext )
{
	int n = 0;
	BSTD_UNUSED(pContext);
	printf("\n How many sec to sleep");
	scanf("%d", &n);

	BKNI_Sleep(n*1000);
}

/***************************************************************************
*
*/
void BTST_P_ToggleStrmClip
( BTST_P_Context          *pContext )
{
	BSTD_UNUSED(pContext);
	printf("Do you want stream cropping?[1=y/0=n]\n");
	BTST_P_Scanf(pContext, "%d", &g_bTestStreamClip);
	if(g_bTestStreamClip)
	{
		printf("Do you want random stream crop?[1=y/0=n]\n");
		BTST_P_Scanf(pContext, "%d", &g_bRandomStrmCrop);
		if(g_bRandomStrmCrop == 0)
		{
			printf("Please enter (ulLeft, ulRight, ulTop, ulBot):\n");
			BTST_P_Scanf(pContext, "%u %u %u %u", &g_ulCropLeft, &g_ulCropRight, &g_ulCropTop, &g_ulCropBot);
		}
	}
}

/***************************************************************************
*
*/
void BTST_P_MpgDisplayMode
( BTST_P_Context          *pContext )
{
#if BTST_VDC_ONLY
	BSTD_UNUSED(pContext);
#else
	int iChoice;
	BTST_MDEC_ChannelHandle hMpgChn = pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh];

	/* Not open yet or non-mpeg source. */
	if(!hMpgChn || pContext->iSelectedVidSrc > 1)
	{
		goto done;
	}

	/* set display field mode */
	printf("Mpeg channel %d display mode: BothFields(0) TopFieldOnly(1) BottomFieldOnly(2)\n",
		pContext->iSelectedVidSrc);
	BTST_P_Scanf(pContext, "%d", &iChoice);

	if (IS_XVD(pContext->eSelectedMDec))
	{
#if (BFramework_NUM_XVD_CORES > 0)
		BXVD_DisplayFieldType eType;

		if      (iChoice == 1) { eType = BXVD_DisplayFieldType_eTopFieldOnly; }
		else if (iChoice == 2) { eType = BXVD_DisplayFieldType_eBottomFieldOnly; }
		else                   { eType = BXVD_DisplayFieldType_eBothField; }

		BXVD_SetDisplayFieldMode((BXVD_ChannelHandle)hMpgChn, eType);
#endif
	}

done:
#endif
	return;
}

void BTST_P_SetPicDropMode( BTST_P_Context  *pContext )
{
#if BTST_VDC_ONLY
	BSTD_UNUSED(pContext);
#else
	int iChoice;
	BTST_MDEC_ChannelHandle hMpgChn = pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh];
	BXVD_PictureDropMode eMode;

	/* Not open yet or non-mpeg source. */
	if(!hMpgChn || pContext->iSelectedVidSrc > 1)
	{
		goto done;
	}

	/* set display field mode */
	printf("Mpeg channel %d 60Hz src -> 50Hz display picture drop mode: Field Drop(0) Frame Drop(1)\n",
		pContext->iSelectedVidSrc);
	BTST_P_Scanf(pContext, "%d", &iChoice);


	if		(iChoice == 1) { eMode = BXVD_PictureDropMode_eFrame; }
	else				   { eMode = BXVD_PictureDropMode_eField; }

	BXVD_SetPictureDropMode((BXVD_ChannelHandle)hMpgChn, eMode);
done:
#endif
	return;
}


void BTST_P_StartCmdRecord( BTST_P_Context  *pContext )
{
	char sRmFile[128];

	sprintf(sRmFile, "rm -rf %s", CMD_RECORD_FILE);

	/* Remove previous recording file */
	system(sRmFile);

	pContext->bRecordCmd = true;
	pContext->iNumCmds = 0;

	pContext->fp_cmd = fopen(CMD_RECORD_FILE, "w+");

	if (pContext->fp_cmd == NULL)
		printf("Failed to opend file for recording commands\n");
}


void BTST_P_EndCmdRecord( BTST_P_Context  *pContext )
{
	pContext->bRecordCmd = false;

	printf("Enter recorded commands loop count : ");
	BTST_P_Scanf(pContext, "%d", &pContext->iLoopCnt);

	/* Init NumCmdsXed so that we know we need to start a round */
	pContext->iNumCmdsXed = pContext->iNumCmds;

	/* Start the iteration */
	pContext->bRepeatCmd = true;
}


/***************************************************************************
*
*/
static void BTST_P_SourceCb_isr
( void                            *pvContext,
 int                              iSrcId,
 void                            *pvSrcData )
{
	BTST_P_Context *pContext = (BTST_P_Context*)pvContext;
	const BVDC_Source_CallbackData *pSrcData = (const BVDC_Source_CallbackData*)pvSrcData;
	const BVDC_Source_DirtyBits *pDirty = &pSrcData->stDirty;
	BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];
	BFMT_VideoFmt eFormat;
	BVDC_ResumeMode eResumeMode;

	/* VDC's souce just changed something. */
	printf("-------------------------------\n");
	if(pDirty->bFmtInfo)
	{
		printf("pSrcData->SrcFormat        = %s %s\n",
			pSrcData->pFmtInfo->pchFormatStr,
			pDirty->bFmtInfo ? "(changed)" : "");
	}
	if (pDirty->bSrcPending)
	{
		printf("pDirty.bSrcPending = 1\n");
	}

	printf("pSrcData->bActive          = %d %s\n",
		pSrcData->bActive,
		pDirty->bActive ? "(changed)" : "");
	printf("pSrcData->ulLumaCrc        = %d %s\n",
		pSrcData->ulLumaCrc,
		pDirty->bCrcValue ? "(changed)" : "");
	printf("pSrcData->ulChromaCrc      = %d %s\n",
		pSrcData->ulChromaCrc,
		pDirty->bCrcValue ? "(changed)" : "");
	printf("pSrcData->ColorStripe      = %s, AGC = %s, ACP Type = %d %s\n",
		(pSrcData->ulColorStripe == 0) ? "No" :
		((pSrcData->ulColorStripe == 2) ? "2-Line" : "4-Line"),
		pSrcData->bCopyProtectionAGC ? "Yes" : "No",
		pSrcData->eMacrovisionType,
		pDirty->bMacrovision ? "(changed)" : "");
	printf("pSrcData->eFrameRateCode   = %d %s\n",
		pSrcData->eFrameRateCode,
		pDirty->bFrameRate ? "(changed)" : "");
	printf("pSrcData->eSourcePolarity  = %d %s\n",
		pSrcData->eSourcePolarity,
		pDirty->bCrcValue? "(changed)" : "");

	/* adjust Timebase clock accordingly */
	if(pDirty->bFrameRate)
	{
		uint32_t ulPcrInc;

		switch(pSrcData->eFrameRateCode)
		{
		case BAVC_FrameRateCode_e23_976:
			ulPcrInc = 1126126;
			break;
		case BAVC_FrameRateCode_e24:
			ulPcrInc = 1125000;
			break;
		case BAVC_FrameRateCode_e29_97:
			ulPcrInc = 900900;
			break;
		case BAVC_FrameRateCode_e30:
			ulPcrInc = 900000;
			break;
		case BAVC_FrameRateCode_e59_94:
			ulPcrInc = 450450;
			break;
		case BAVC_FrameRateCode_e60:
			ulPcrInc = 450000;
			break;
		case BAVC_FrameRateCode_e25:
			ulPcrInc = 1080000;
			break;
		case BAVC_FrameRateCode_e50:
			ulPcrInc = 540000;
			break;
		default:
			ulPcrInc = 900900;
		}
		BDBG_MSG(("Src %d, DPCR0 PCR_INC = %d", iSrcId, ulPcrInc));
#if 0
		BREG_Write32_isr(pContext->hReg, BCHP_XPT_DPCR0_REF_PCR, ulPcrInc);
#endif
	}

	/* If frame rate change, adjust display format for panel output */
	BVDC_Display_GetVideoFormat(hDisplay, &eFormat);
	if((pDirty->bFrameRate) &&
		(eFormat == BFMT_VideoFmt_eCustom0 || eFormat == BFMT_VideoFmt_eCustom1))
	{
		printf("Change display format for panel output \n");
		if(pSrcData->pFmtInfo->ulVertFreq == 6000)
		{
			BVDC_Display_SetVideoFormat(hDisplay, BFMT_VideoFmt_eCustom0);
			pContext->apFmtInfo[pContext->iSelectedComp] =
				BFMT_GetVideoFormatInfoPtr(BFMT_VideoFmt_eCustom0);
		}
		else if(pSrcData->pFmtInfo->ulVertFreq == 5000)
		{
			BVDC_Display_SetVideoFormat(hDisplay, BFMT_VideoFmt_eCustom1);
			pContext->apFmtInfo[pContext->iSelectedComp] =
				BFMT_GetVideoFormatInfoPtr(BFMT_VideoFmt_eCustom1);
		}
	}

	BVDC_Source_GetResumeMode(pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc, &eResumeMode );

	/* Source pending */
	if ((pSrcData->bActive) &&
		(pDirty->bSrcPending) &&
		(NULL != pContext->hSourcePendingEvent))
	{
		printf("   send resume event\n");
		BKNI_SetEvent(pContext->hSourcePendingEvent);
	}

	return;
}

/***************************************************************************
*
*/
void BTST_P_ToggleSrcCb
( BTST_P_Context          *pContext )
{
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	bool *pbSrcCb =
		&(pContext->aSrcContext[pContext->iSelectedVidSrc].bSrcCbEnable);
	int  iChoice;

	if(!hVidSrc)
	{
		goto done;
	}

	if(!*pbSrcCb)
	{
		printf("Enabled generic source callback. Enter CRC capture (0) off, (1) on :\n");
		BTST_P_Scanf(pContext, "%d", &iChoice);
		BVDC_Source_InstallCallback(hVidSrc,
			BTST_P_SourceCb_isr, (void*)pContext, pContext->iSelectedWin);
		g_bCapture = (iChoice==1)? true : false;
	}
	else
	{
		printf("Disabled generic source callback.\n");
		BVDC_Source_InstallCallback(hVidSrc, NULL, NULL, 0);
		g_bCapture = false;
	}

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
		*pbSrcCb = !*pbSrcCb;
	}

done:
	return;
}



/***************************************************************************
*
*/
static void BTST_P_DetectMpegFmt_isr
( void                            *pvContext,
 int                              iSrcId,
 void                            *pvSrcData )
{
	BTST_P_Context *pContext = (BTST_P_Context*)pvContext;
	const BVDC_Source_CallbackData *pSrcData = (const BVDC_Source_CallbackData*)pvSrcData;
	const BVDC_Source_DirtyBits *pDirty = &pSrcData->stDirty;

	BSTD_UNUSED(iSrcId);

	if(pDirty->bFmtInfo)
	{
		pContext->ulMpegSrcFmtChangeCnt++;
		printf("New Src Format = %s %s (%d)\n",
			pSrcData->pFmtInfo->pchFormatStr,
			pDirty->bFmtInfo ? "(changed)" : "", pContext->ulMpegSrcFmtChangeCnt);
	}

	return;
}

/***************************************************************************
*
*/
void BTST_P_DetectMpegInputTest
( BTST_P_Context          *pContext )
{
	uint32_t    i;
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[0].hVidSrc;

	if(!hVidSrc)
	{
		goto done;
	}

	BVDC_Source_InstallCallback(hVidSrc,
		BTST_P_DetectMpegFmt_isr, (void*)pContext, pContext->iSelectedWin);

	pContext->ulMpegSrcFmtChangeCnt = 0;
	for(i = 0; i < BTST_PLAYBACK_STREAM_COUNT; i++)
	{
		BTST_P_BetterMpegDecode(pContext, astPlaybackStream[i].fn,
			astPlaybackStream[i].ui16VidPid, astPlaybackStream[i].ui16PcrPid,
			astPlaybackStream[i].ui16EnhPid,
			astPlaybackStream[i].eStreamType, astPlaybackStream[i].eVideoCmprStd);

		BTST_P_ApplyChanges(pContext);

		BKNI_Sleep(rand() % 3000);
	}

	BKNI_Sleep(10000);

	if(pContext->ulMpegSrcFmtChangeCnt != BTST_PLAYBACK_STREAM_COUNT)
	{
		printf("-------------------------------------------\n");
		printf("Something is wrong with input stream\n");
		printf("Check if streamer is working \n");
		printf("Check if the playback stream is available\n");
		printf("Check if XPT is working \n");
		printf("-------------------------------------------\n");
		exit(1);
	}
	else
	{
		printf("MPEG input is OK \n");
	}

	BVDC_Source_InstallCallback(hVidSrc, NULL, NULL, 0);
	BTST_P_ApplyChanges(pContext);

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_TogglePicList
( BTST_P_Context          *pContext )
{
	int iMode, i;

	BSTD_UNUSED(pContext);
	if(pContext->iSelectedVidSrc > 1)
	{
		printf("Please select an mpeg source to toggle picture list!\n");
		return;
	}

	if(!g_bEnablePicListForMosaicMode)
	{
		printf("Enabled Mosaic Mode picture list.\n");
		printf("Select opened channels: (0) all; (1) 1st half; (2) 2nd half; (3) alternate; (4) none; (5) custom;\n");
		BTST_P_Scanf(pContext, "%d", &iMode);
		switch(iMode)
		{
		case 0:
			for(i = 0; i < BAVC_MOSAIC_MAX; i++)
			{
				pContext->abChanOpened[pContext->iSelectedVidSrc][i]= true;
			}
			break;
		case 1:
			for(i = 0; i < BAVC_MOSAIC_MAX; i++)
			{
				pContext->abChanOpened[pContext->iSelectedVidSrc][i]= i<BAVC_MOSAIC_MAX/2;
			}
			break;
		case 2:
			for(i = 0; i < BAVC_MOSAIC_MAX; i++)
			{
				pContext->abChanOpened[pContext->iSelectedVidSrc][i]= i>=BAVC_MOSAIC_MAX/2;
			}
			break;
		case 3:
			for(i = 0; i < BAVC_MOSAIC_MAX; i++)
			{
				pContext->abChanOpened[pContext->iSelectedVidSrc][i]= i&1;
			}
			break;
		case 4:
			for(i = 0; i < BAVC_MOSAIC_MAX; i++)
			{
				pContext->abChanOpened[pContext->iSelectedVidSrc][i]= false;
			}
			break;
		case 5:
			for(i = 0; i < BAVC_MOSAIC_MAX; i++)
			{
				printf("channel[%d] open? (1=y/0=n): ", i);
				BTST_P_Scanf(pContext, "%d", (int*)&pContext->abChanOpened[pContext->iSelectedVidSrc][i]);
				printf("Continue? [1=y/0=n]");
				BTST_P_Scanf(pContext, "%d", &iMode);
				if(iMode == 0)
				{
					int j;
					for(j = i+1; j < BAVC_MOSAIC_MAX; j++)
					{
						pContext->abChanOpened[pContext->iSelectedVidSrc][j]= false;
					}
					g_bEnablePicListForMosaicMode = true;
					goto done;
				}
			}
			break;
		default: ;
		}
		g_bEnablePicListForMosaicMode = true;
	}
	else
	{
		printf("Disabled Mosaic Mode picture list.\n");
		g_bEnablePicListForMosaicMode = false;
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_WorstPicList
( BTST_P_Context          *pContext )
{
	BSTD_UNUSED(pContext);
	if(!g_bMosaicWorst)
	{
		printf("Enabled Worst Mosaic Mode picture list.");
		g_bMosaicWorst = true;
	}
	else
	{
		printf("Disabled Worst Mosaic Mode picture list.\n");
		g_bMosaicWorst = false;
	}

	return;
}


/***************************************************************************
*
*/
void BTST_P_ManualApplyChanges
( BTST_P_Context          *pContext )
{
	BTST_P_ApplyChanges(pContext);
}

/***************************************************************************
*
*/
void BTST_P_AbortChanges
( BTST_P_Context          *pContext )
{
	if(!pContext->hVdc)
	{
		goto done;
	}

	printf("Abort changes (settings failed to applied).\n");
	BVDC_AbortChanges(pContext->hVdc);

done:
	return;
}


/***************************************************************************
*
*/
void BTST_P_ToggleApplyImm
( BTST_P_Context          *pContext )
{

	pContext->bApplyImmediately = !pContext->bApplyImmediately;
	printf("Toggle apply changes immediately is: %s\n",
		pContext->bApplyImmediately ? "On" : "Off");
	return;
}


/***************************************************************************
*
*/
void BTST_P_ToggleApplyErr
( BTST_P_Context          *pContext )
{

	pContext->bApplyError = !pContext->bApplyError;
	printf("Toggle apply changes error catching is: %s\n",
		pContext->bApplyError ? "On" : "Off");
	return;
}


/***************************************************************************
*
*/
void BTST_P_ToggleSclDown
( BTST_P_Context          *pContext )
{
	pContext->bEnableScaleDown = !pContext->bEnableScaleDown;
	printf("Toggle between full screen and scale down is: %s\n",
		pContext->bEnableScaleDown? "Scale Down" : "Full Screen");
	return;
}


/***************************************************************************
*
*/
static void BTST_P_PrintHeapSettings
( const BVDC_Heap_Settings          *pHeapSettings )
{
	const BFMT_VideoInfo *pFmtInfo;

	pFmtInfo = BFMT_GetVideoFormatInfoPtr(pHeapSettings->eBufferFormat_2HD);
	printf("\n2HD buffer settings:\n");
	printf("ulBufferCnt_2HD      = %d\n", pHeapSettings->ulBufferCnt_2HD);
	printf("ulAdditionalLines_2HD= %d\n", pHeapSettings->ulAdditionalLines_2HD);
	printf("ulBufferCnt_2HD_Pip  = %d\n", pHeapSettings->ulBufferCnt_2HD_Pip);
	printf("eBufferFormat_2HD    = %s\n", pFmtInfo->pchFormatStr);


	printf("\n2HD PIP buffer settings:\n");
	printf("ulBufferCnt_2HD_Pip  = %d\n", pHeapSettings->ulBufferCnt_2HD_Pip);

	pFmtInfo = BFMT_GetVideoFormatInfoPtr(pHeapSettings->eBufferFormat_HD);
	printf("\nHD buffer settings:\n");
	printf("ulBufferCnt_HD       = %d\n", pHeapSettings->ulBufferCnt_HD);
	printf("ulBufferCnt_HD_Pip   = %d\n", pHeapSettings->ulBufferCnt_HD_Pip);
	printf("ulAdditionalLines_HD = %d\n", pHeapSettings->ulAdditionalLines_HD);
	printf("eBufferFormat_HD     = %s\n", pFmtInfo->pchFormatStr);


	printf("\nHD PIP buffer settings:\n");
	printf("ulBufferCnt_HD_Pip   = %d\n", pHeapSettings->ulBufferCnt_HD_Pip);

	pFmtInfo = BFMT_GetVideoFormatInfoPtr(pHeapSettings->eBufferFormat_SD);
	printf("\nSD buffer settings:\n");
	printf("ulBufferCnt_SD       = %d\n", pHeapSettings->ulBufferCnt_SD);
	printf("ulBufferCnt_SD_Pip   = %d\n", pHeapSettings->ulBufferCnt_SD_Pip);
	printf("ulAdditionalLines_SD = %d\n", pHeapSettings->ulAdditionalLines_SD);
	printf("eBufferFormat_SD     = %s\n", pFmtInfo->pchFormatStr);


	printf("\nSD PIP buffer settings:\n");
	printf("ulBufferCnt_SD_Pip   = %d\n", pHeapSettings->ulBufferCnt_SD_Pip);
}

/***************************************************************************
*
*/
void BTST_P_PrintVdcSettings
( BTST_P_Context          *pContext )
{
	const BFMT_VideoInfo *pFmtInfo;
	const BVDC_Heap_Settings *pHeapSettings = NULL;

	printf("\nVDC settings are: \n");

	pFmtInfo = BFMT_GetVideoFormatInfoPtr(pContext->stSettings.eVideoFormat);
	printf("eVideoFormat         = %s\n", pFmtInfo->pchFormatStr);
	printf("eDisplayFrameRate    = %d\n", pContext->stSettings.eDisplayFrameRate);
	printf("eColorMatrix_HD      = %d\n", pContext->stSettings.eColorMatrix_HD);
	printf("eColorMatrix_SD      = %d\n", pContext->stSettings.eColorMatrix_SD);
	printf("bVecSwap             = %d\n", pContext->stSettings.bVecSwap);
	printf("eDacDetection        = %d\n", pContext->stSettings.eDacDetection);

	switch(pContext->iSelectedMem)
	{
	case 0:
		pHeapSettings = &pContext->stSettings.stHeapSettings;
		printf("\nMem0 heap settings:\n");
		break;
	case 1:
		pHeapSettings = &pContext->astMemInfo[1].stVdcHeapSettings;
		printf("\nMem1 heap settings:\n");
		break;
	case 2:
		pHeapSettings = &pContext->stSharedHeapSettings;
		printf("\nShared mem heap settings:\n");
		break;
	}


	BTST_P_PrintHeapSettings(pHeapSettings);

}


/***************************************************************************
*
*/
static void BTST_P_GetBufferFmt
( BTST_P_Context		   *pContext,
 char                     *string,
 BFMT_VideoFmt            *peVidFmt,
 BPXL_Format              *pePxlFmt )
{
	uint32_t i;
	BFMT_VideoFmt   eVidFmt = BFMT_VideoFmt_e1080i;
	BPXL_Format     ePxlFmt = BPXL_eY08_Cb8_Y18_Cr8;
	const BFMT_VideoInfo *pFmtInfo;

	pFmtInfo = BFMT_GetVideoFormatInfoPtr(*peVidFmt);

	printf("Select %s buffer video format (Default = %s)\
		   \n\t480i(0), 480p(1), 720p(2), 1080i(3), \
		   \n\tNTSC_J(4), PAL_G(5), PAL_M(6), PAL_N(7), PAL_NC(8), \
		   \n\t576p_50Hz(9), 720p_50Hz(10), 1080i_50Hz(11), 1080p_30(12), \
		   \n\t1250i_50Hz(13), 640x480p(14), 800x600p(15), 1024x768p(16), \
		   \n\t1280x720p(17), 1280x768p(18), 1366x768p(19):",
		   string, pFmtInfo->pchFormatStr);


	BTST_P_Scanf(pContext, "%u", &i);
	if     (0 == i){eVidFmt = BFMT_VideoFmt_eNTSC;}
	else if(1 == i){eVidFmt = BFMT_VideoFmt_e480p;}
	else if(2 == i){eVidFmt = BFMT_VideoFmt_e720p;}
	else if(3 == i){eVidFmt = BFMT_VideoFmt_e1080i;}
	else if(4 == i){eVidFmt = BFMT_VideoFmt_eNTSC_J;}
	else if(5 == i){eVidFmt = BFMT_VideoFmt_ePAL_G;}
	else if(6 == i){eVidFmt = BFMT_VideoFmt_ePAL_M;}
	else if(7 == i){eVidFmt = BFMT_VideoFmt_ePAL_N;}
	else if(8 == i){eVidFmt = BFMT_VideoFmt_ePAL_NC;}
	else if(9 == i) {eVidFmt = BFMT_VideoFmt_e576p_50Hz;}
	else if(10 == i){eVidFmt = BFMT_VideoFmt_e720p_50Hz;}
	else if(11 == i){eVidFmt = BFMT_VideoFmt_e1080i_50Hz;}
	else if(12 == i){eVidFmt = BFMT_VideoFmt_e1080p_30Hz;}
	else if(13 == i){eVidFmt = BFMT_VideoFmt_e1250i_50Hz;}
	else if(14 == i){eVidFmt = BFMT_VideoFmt_eDVI_640x480p;}
	else if(15 == i){eVidFmt = BFMT_VideoFmt_eDVI_800x600p;}
	else if(16 == i){eVidFmt = BFMT_VideoFmt_eDVI_1024x768p;}
	else if(17 == i){eVidFmt = BFMT_VideoFmt_eDVI_1280x720p;}
	else if(18 == i){eVidFmt = BFMT_VideoFmt_eDVI_1280x768p;}
	else if(19 == i){eVidFmt = BFMT_VideoFmt_eCUSTOM_1366x768p;}
	*peVidFmt = eVidFmt;

	printf("Select %s buffer pixel format (Default = %s)\
		   \n\t8-bit 422(0), 10-bit 422(1), 10-bit 444(2):",
		   string,
		   (*pePxlFmt == BPXL_eY08_Cb8_Y18_Cr8) ? "8-bit 422" :
		   (*pePxlFmt == BPXL_eX2_Y010_Cb10_Y110_X2_Cr10_Y010_Cb10_X2_Y110_Cr10_Y010_X2_Cb10_Y110_Cr10) ? "10-bit 422" : "10-bit 444");
	BTST_P_Scanf(pContext, "%u", &i);
	if     (0 == i){ePxlFmt = BPXL_eY08_Cb8_Y18_Cr8;}
	else if(1 == i){ePxlFmt = BPXL_eX2_Y010_Cb10_Y110_X2_Cr10_Y010_Cb10_X2_Y110_Cr10_Y010_X2_Cb10_Y110_Cr10;}
	else if(2 == i){ePxlFmt = BPXL_eX2_Cr10_Y10_Cb10;}
	*pePxlFmt = ePxlFmt;

}


/***************************************************************************
*
*/
static void BTST_P_ChangeHeapSettings
(BTST_P_Context *pContext,
 BVDC_Heap_Settings    *pHeapSettings )
{
	/* Double HD buffer settings */
	printf("Select number of 2HD buffers(Default = %d)\n",
		pHeapSettings->ulBufferCnt_2HD);
	BTST_P_Scanf(pContext, "%u", &pHeapSettings->ulBufferCnt_2HD);

	/* Double HD PIP buffer settings */
	printf("Select number of 2HD Pip buffers(Default = %d)\n",
		pHeapSettings->ulBufferCnt_2HD_Pip);
	BTST_P_Scanf(pContext, "%u", &pHeapSettings->ulBufferCnt_2HD_Pip);

	if( pHeapSettings->ulBufferCnt_2HD || pHeapSettings->ulBufferCnt_2HD_Pip )
	{
		BTST_P_GetBufferFmt(pContext, "2HD", &pHeapSettings->eBufferFormat_2HD,
			&pHeapSettings->ePixelFormat_2HD);
	}

	/* HD buffer settings */
	printf("Select number of HD buffers(Default = %d)\n",
		pHeapSettings->ulBufferCnt_HD);
	BTST_P_Scanf(pContext, "%u", &pHeapSettings->ulBufferCnt_HD);

	/* HD PIP buffer settings */
	printf("Select number of HD PIP buffers(Default = %d)\n",
		pHeapSettings->ulBufferCnt_HD_Pip);
	BTST_P_Scanf(pContext, "%u", &pHeapSettings->ulBufferCnt_HD_Pip);

	if( pHeapSettings->ulBufferCnt_HD || pHeapSettings->ulBufferCnt_HD_Pip )
	{
		BTST_P_GetBufferFmt(pContext, "HD", &pHeapSettings->eBufferFormat_HD,
			&pHeapSettings->ePixelFormat_HD);
	}

	/* SD buffer settings */
	printf("Select number of SD buffers(Default= %d)\n",
		pHeapSettings->ulBufferCnt_SD);
	BTST_P_Scanf(pContext, "%u", &pHeapSettings->ulBufferCnt_SD);

	/* SD PIP buffer settings */
	printf("Select number of SD PIP buffers(Default= %d)\n",
		pHeapSettings->ulBufferCnt_SD_Pip);
	BTST_P_Scanf(pContext, "%u", &pHeapSettings->ulBufferCnt_SD_Pip);

	if( pHeapSettings->ulBufferCnt_SD || pHeapSettings->ulBufferCnt_SD_Pip )
	{
		BTST_P_GetBufferFmt(pContext, "SD", &pHeapSettings->eBufferFormat_SD,
			&pHeapSettings->ePixelFormat_SD);
	}

}

/***************************************************************************
*
*/
void BTST_P_ChangeVdcSettings
( BTST_P_Context          *pContext )
{
	uint32_t i;
	BVDC_Heap_Settings *pHeapSettings = NULL;

	if(pContext->hVdc)
	{
		printf("VDC is already open\n");
		return;
	}

	switch(pContext->iSelectedMem)
	{
	case 0:
		pHeapSettings = &pContext->stSettings.stHeapSettings;
		printf("Change VDC default settings:\n");

		/* TODO: Add more */
		printf("Swap CMP/VEC (Default = %d): no(0), yes(1)?\n",
			pContext->stSettings.bVecSwap);
		BTST_P_Scanf(pContext, "%u", &i);
		pContext->stSettings.bVecSwap= (i) ? true : false;
		break;
	case 1:
		pHeapSettings = &pContext->astMemInfo[1].stVdcHeapSettings;
		printf("Change mem1 heap settings:\n");
		break;
	case 2:
		pHeapSettings = &pContext->stSharedHeapSettings;
		printf("Change shared mem heap settings:\n");
		break;
	}

	BTST_P_ChangeHeapSettings(pContext, pHeapSettings);

	printf("Change Dac Detection (Default = %d): Auto(0), Off(1), On(2)\n",
		pContext->stSettings.eDacDetection);
	BTST_P_Scanf(pContext, "%u", &pContext->stSettings.eDacDetection);

	BTST_P_PrintVdcSettings(pContext);

	return;
}

/***************************************************************************
*
*/
void BTST_P_ChangeVdcHeapSettings
( BTST_P_Context          *pContext )
{
	BVDC_Heap_Settings *pHeapSettings = &pContext->astMemInfo[1].stVdcHeapSettings;

	BTST_P_ChangeHeapSettings(pContext, pHeapSettings);

	BTST_P_PrintHeapSettings(pHeapSettings);
}

/***************************************************************************
*
*/
void BTST_P_ChangeVdcBandGapSettings
( BTST_P_Context          *pContext )
{
	uint32_t id;

	if(pContext->hVdc)
	{
		printf("VDC is already open\n");
		return;
	}

	for(id = 0; id < BVDC_MAX_DACS; id++)
	{
		printf("Dac %d [%d]: ", id, pContext->stSettings.aulDacBandGapAdjust[id]);
		BTST_P_Scanf(pContext, "%u", &pContext->stSettings.aulDacBandGapAdjust[id]);
	}
}

/***************************************************************************
*
*/
void BTST_P_FormatChange
( BTST_P_Context          *pContext )
{
	uint32_t i, j;
	uint32_t ulCount, ulCountMax;
	BFMT_VideoFmt aaeTestFormats[][2] =
	{
		{BFMT_VideoFmt_eNTSC, BFMT_VideoFmt_e480p },
		{BFMT_VideoFmt_eNTSC, BFMT_VideoFmt_e720p },
		{BFMT_VideoFmt_eNTSC, BFMT_VideoFmt_e1080i},
		{BFMT_VideoFmt_e480p, BFMT_VideoFmt_e720p },
		{BFMT_VideoFmt_e480p, BFMT_VideoFmt_e1080i},
		{BFMT_VideoFmt_e720p, BFMT_VideoFmt_e1080i}
	};

	/* Stress format change. */
	printf("Enter number of times to change format:");
	BTST_P_Scanf(pContext, "%u", &ulCountMax);

	for(i = 0; i < (sizeof(aaeTestFormats)/sizeof(BFMT_VideoFmt))/2; i++)
	{
		const BFMT_VideoInfo *pFmtInfoA, *pFmtInfoB;
		j = 0;
		ulCount = ulCountMax;
		pFmtInfoA = BFMT_GetVideoFormatInfoPtr(aaeTestFormats[i][0]);
		pFmtInfoB = BFMT_GetVideoFormatInfoPtr(aaeTestFormats[i][1]);
		printf("Changing format from %s <-> %s\n",
			pFmtInfoA->pchFormatStr, pFmtInfoB->pchFormatStr);

		while(ulCount > 0)
		{
			/* ulIdx = (rand() % (sizeof(aeTestFormats)/sizeof(BFMT_VideoFmt))); */
			j = j ? 0 : 1;
			pContext->peNewFormat = &aaeTestFormats[i][j];
			BTST_P_ChgDispFmt(pContext);

			/* Print out new format */
			printf("Display[%d]'s new format (%d/%d): %-21s\n",
				pContext->iSelectedComp, ulCount, ulCountMax,
				pContext->apFmtInfo[pContext->iSelectedComp]->pchFormatStr);
			ulCount--;

			/* Format changed failed, end now. */
			if(!pContext->peNewFormat)
			{
				break;
			}

			/* Delay until changing to the next one. */
			BKNI_Sleep((rand() % 500) + 1000);
			/*BKNI_Sleep(100);*/
		}
	}

	/* Restore format change input from stdin */
	pContext->peNewFormat = NULL;

	return;
}

/***************************************************************************
* Channel change between channel 0, 1, 2. Need to open the selected channels
* before test.
*/
void BTST_P_PR14134
( BTST_P_Context          *pContext )
{
	uint32_t ulCount, ulCountMax;

	/* Stress channel change of selected channel */
	printf("Enter number of times to change channel: ");
	BTST_P_Scanf(pContext, "%u", &ulCountMax);
	ulCount = ulCountMax;

	while(ulCount > 0)
	{
		/* 1) open the selected window */
		pContext->iSelectedWin = 0;
		pContext->iSelectedVidSrc = 0;
		printf("Open window[%d] with src[%d] (%d/%d)\n",
			pContext->iSelectedWin,
			pContext->iSelectedVidSrc, ulCount, ulCountMax);
		BTST_P_OpenVidWin(pContext);
		BKNI_Sleep(500);

		/* 2) close */
		printf("Close window[%d] with src[%d]\n",
			pContext->iSelectedWin,
			pContext->iSelectedVidSrc);
		BTST_P_CloseVidWin(pContext);
		BKNI_Sleep(500);

		/* 3) open win0 with src2 */
		pContext->iSelectedWin = 0;
		pContext->iSelectedVidSrc = 2;
		printf("Open window[%d] with src[%d]\n",
			pContext->iSelectedWin,
			pContext->iSelectedVidSrc);
		BTST_P_OpenVidWin(pContext);
		BKNI_Sleep(500);

		/* 4) open win1 with src1 */
		pContext->iSelectedWin = 1;
		pContext->iSelectedVidSrc = 1;
		printf("Open window[%d] with src[%d]\n",
			pContext->iSelectedWin,
			pContext->iSelectedVidSrc);
		BTST_P_OpenVidWin(pContext);
		BKNI_Sleep(1000);

		/* 5) close win0 */
		pContext->iSelectedWin = 0;
		pContext->iSelectedVidSrc = 2;
		printf("Close window[%d] with src[%d]\n",
			pContext->iSelectedWin,
			pContext->iSelectedVidSrc);
		BTST_P_CloseVidWin(pContext);
		BKNI_Sleep(1000);

		/* 6) open win0 with src0 */
		pContext->iSelectedWin = 0;
		pContext->iSelectedVidSrc = 0;
		printf("Open window[%d] with src[%d]\n",
			pContext->iSelectedWin,
			pContext->iSelectedVidSrc);
		BTST_P_OpenVidWin(pContext);
		BKNI_Sleep(1000);

		/* 7) close windows */
		pContext->iSelectedWin = 0;
		printf("Close window[%d]\n", pContext->iSelectedWin);
		BTST_P_CloseVidWin(pContext);
		BKNI_Sleep(1000);
		pContext->iSelectedWin = 1;
		printf("Close window[%d]\n", pContext->iSelectedWin);
		BTST_P_CloseVidWin(pContext);
		BKNI_Sleep(1000);

		ulCount--;
	}

	return;
}

/***************************************************************************
* Channel change between channel 0, 1, 2. Need to open the selected channels
* before test.
*/
void BTST_P_ChannelChange
( BTST_P_Context          *pContext )
{
	uint32_t ulCount, ulCountMax;
	uint32_t ulIndex, ulChannelNum;
	/* TODO: Add HD_DVI source */
	uint32_t aulTestChannels[6];
	BVDC_Source_Handle hVidSrc;
	bool bIfd = false;
	const BFMT_VideoInfo *pFmtInfo     = pContext->apFmtInfo[pContext->iSelectedComp];
	BVDC_Window_Handle hVidWin         =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	/* Stress channel change of selected channel */
	printf("Enter number of times to change channel: ");
	BTST_P_Scanf(pContext, "%u", &ulCountMax);
	ulCount = ulCountMax;

	/* Edit channels */
	printf("Enter number of channels: ");
	BTST_P_Scanf(pContext, "%u", &ulChannelNum);
	/* Use selected source if only one channel */
	if( ulChannelNum > 1 )
	{
		printf("Edit channel map:");
		for( ulIndex = 0; ulIndex < ulChannelNum; ulIndex++ )
		{
			printf("Select channel[%d]: m0(0), m1(1), a0(2), 656(3) ifd(4)\n", ulIndex);
			BTST_P_Scanf(pContext, "%u", &aulTestChannels[ulIndex]);
		}
	}

	while(ulCount > 0)
	{
		/* Close the selected window */
		printf("Close window[%d] on display[%d] (%d/%d)\n",
			pContext->iSelectedWin,
			pContext->iSelectedComp, ulCount, ulCountMax);
		BTST_P_CloseVidWin(pContext);
		if(BERR_SUCCESS != BTST_P_ApplyChanges(pContext))
		{
			/* stop on failure */
			break;
		}

		/* Delay until changing to the next one. */
		BKNI_Sleep((rand() % 1000) + 500);

		/* Select new channel */
		if( ulChannelNum > 1 )
		{
#if 0
			ulIndex = (rand() % ulChannelNum);
#else
			ulIndex = (ulCount % ulChannelNum);
#endif
			if(aulTestChannels[ulIndex] == 4)
			{/* ifd shares the same vdec source handle as cvbs line-in */
				bIfd = true;
				pContext->iSelectedVidSrc = 2;
			}
			else
			{
				bIfd = false;
				pContext->iSelectedVidSrc = aulTestChannels[ulIndex];
			}
		}

		if(pContext->iSelectedVidSrc == 2)
		{
			hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;

			if(!hVidSrc)
			{
				return;
			}

			if(bIfd)
			{
				printf("Select RF input\n");

				/* set BVDC_Adc_0, BVDC_Adc_1 and BVDC_Adc_2 to unused */
				BVDC_Source_SetAdcConfiguration(hVidSrc,
					BVDC_Adc_0 | BVDC_Adc_1 | BVDC_Adc_2, BVDC_AdcInput_eUnused);

				/* set BVDC_Ibafe_0 to accept RF from IFDemodulator */
				BVDC_Source_SetAdcConfiguration(hVidSrc,
					BVDC_Ibafe_0,  BVDC_AdcInput_eIfDemod);
			}
			else
			{
				printf("Select CVBS VDEC input\n");

				/* set BVDC_Adc_0 to accept Composite (or CVBS) from line-in */
				BVDC_Source_SetAdcConfiguration(hVidSrc,
					BVDC_Adc_0, BVDC_AdcInput_eComposite);

				/* set BVDC_Adc_1, BVDC_Adc_2 and BVDC_Ibafe_0 to unused */
				BVDC_Source_SetAdcConfiguration(hVidSrc,
					BVDC_Ibafe_0 | BVDC_Adc_1 | BVDC_Adc_2, BVDC_AdcInput_eUnused);
			}
		}

		/* set 656in source format */
		if(pContext->iSelectedVidSrc == 3)
		{
			BVDC_Source_SetVideoFormat(pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc,
				BFMT_VideoFmt_eNTSC);
		}

		/* Open the selected window */
		printf("Open window[%d] with src[%d] on display[%d] (%d/%d)\n",
			pContext->iSelectedWin, pContext->iSelectedVidSrc,
			pContext->iSelectedComp, ulCount, ulCountMax);
		BTST_P_OpenVidWin(pContext);

		/* scaledown */
		BVDC_Window_SetScalerOutput(hVidWin, 0, 0,
			pFmtInfo->ulWidth/2, pFmtInfo->ulHeight/2);

		BVDC_Window_SetDstRect(hVidWin,
			pFmtInfo->ulWidth/4, pFmtInfo->ulHeight/4,
			pFmtInfo->ulWidth/2, pFmtInfo->ulHeight/2);
		if(BERR_SUCCESS != BTST_P_ApplyChanges(pContext))
		{
			/* stop on failure */
			break;
		}

		/* Delay until changing to the next one. */
		BKNI_Sleep((rand() % 3000));

		ulCount--;
	}

	return;
}

/***************************************************************************
* Format change and Channel change between channel 0, 1, 2.
* Need to open selected channels before test.
*/
void BTST_P_StressTest
( BTST_P_Context          *pContext )
{
	uint32_t ulIdx = 0;
	uint32_t ulCount, ulCountMax;
	uint32_t ulIndex, ulChannelNum;
	/* TODO: Add HD_DVI source */
	uint32_t aulTestChannels[3];
	BFMT_VideoFmt aeTestFormats[] =
	{
		BFMT_VideoFmt_eNTSC,
		BFMT_VideoFmt_e480p,
		BFMT_VideoFmt_e720p,
		BFMT_VideoFmt_e1080i
	};

	/* Stress format and channel change of selected channel */
	printf("Enter number of times to change format and channel: ");
	BTST_P_Scanf(pContext, "%u", &ulCountMax);
	ulCount = ulCountMax;

	/* Edit channels */
	printf("Enter number of channels: ");
	BTST_P_Scanf(pContext, "%u", &ulChannelNum);
	/* Use selected source if only one channel */
	if( ulChannelNum > 1 )
	{
		printf("Edit channel map:");
		for( ulIndex = 0; ulIndex < ulChannelNum; ulIndex++ )
		{
			printf("Select channel[%d]: m0(0), m1(1), a0(2)\n", ulIndex);
			BTST_P_Scanf(pContext, "%u", &aulTestChannels[ulIndex]);
		}
	}

	while(ulCount > 0)
	{
		if( (rand() % 5) == 0 )
		{
			/* Change format */
			ulIdx = (rand() % (sizeof(aeTestFormats)/sizeof(BFMT_VideoFmt)));
			pContext->peNewFormat = &aeTestFormats[ulIdx];
			BTST_P_ChgDispFmt(pContext);
			if(BERR_SUCCESS != BTST_P_ApplyChanges(pContext))
			{
				/* stop on failure */
				break;
			}
			/* Delay until changing to the next one. */
			BKNI_Sleep((rand() % 5000) + 100);

			/* Print out new format */
			printf("Display[%d]'s new format (%d/%d): %-21s\n",
				pContext->iSelectedComp, ulCount, ulCountMax,
				pContext->apFmtInfo[pContext->iSelectedComp]->pchFormatStr);
		}
		else
		{
			/* Change channel */
			/* Close the selected window */
			printf("Close window[%d] on display[%d] (%d/%d)\n",
				pContext->iSelectedWin,
				pContext->iSelectedComp, ulCount, ulCountMax);
			BTST_P_CloseVidWin(pContext);
			if(BERR_SUCCESS != BTST_P_ApplyChanges(pContext))
			{
				/* stop on failure */
				break;
			}

			/* Delay until changing to the next one. */
			/*BKNI_Sleep((rand() % 1000) + 500);*/

			/* Select new channel */
			if( ulChannelNum > 1 )
			{
				ulIndex = (rand() % ulChannelNum);
				pContext->iSelectedVidSrc = aulTestChannels[ulIndex];
			}

			/* Open the selected window */
			printf("Open window[%d] with src[%d] on display[%d] (%d/%d)\n",
				pContext->iSelectedWin, pContext->iSelectedVidSrc,
				pContext->iSelectedComp, ulCount, ulCountMax);
			BTST_P_OpenVidWin(pContext);

			/* Delay until changing to the next one. */
			BKNI_Sleep((rand() % 5000) + 500);
		}

		ulCount--;
	}

	return;
}


/***************************************************************************
*
*/
void BTST_P_LineCntTest
( BTST_P_Context          *pContext )
{
	BDBG_WRN(("Not supported"));
	BSTD_UNUSED(pContext);
}


/***************************************************************************
*
*/
void BTST_P_MirModeChnChg
( BTST_P_Context          *pContext )
{
	BVDC_Display_Handle hDisplay0, hDisplay1;
	BVDC_Window_Handle hCmp0_V0, hCmp1_V0;
	int i, iCount, iOrder;
	int iFormatSwitch = 0;
	bool bIfd = false;
	uint32_t ulIndex, ulChannelNum;
	uint32_t aulTestChannels[7];
	char *aacChanName[] =
	{
		"m0[ch0]", "m1[ch1]", "a0[ch2]", "a1[ch3]", "656in0[ch4]", "656in1[ch5]", "ifd[ch6]"
	};

	BFMT_VideoFmt aeTestFormats[] =
	{
		BFMT_VideoFmt_eNTSC,
		BFMT_VideoFmt_e480p,
		BFMT_VideoFmt_e720p,
		BFMT_VideoFmt_e1080i
	};

	hDisplay0 = pContext->ahDisplay[0];
	hDisplay1 = pContext->ahDisplay[1];
	hCmp0_V0  = pContext->aWinContext[0][0].hWindow;
	hCmp1_V0  = pContext->aWinContext[0][0].hWindow;

	/* Get the handle to the two main windows on primary and secondary display. */
	if(!hDisplay0 || !hDisplay1 || !hCmp0_V0 || !hCmp1_V0)
	{
		goto done;
	}

	if(pContext->abSwapMainPip[pContext->iSelectedComp])
	{
		pContext->iSelectedWin = 1;
	}
	else
	{
		pContext->iSelectedWin = 0;
	}

	printf("How many times for channel change: ");
	BTST_P_Scanf(pContext, "%d", &iCount);
	printf("Do you want format switch: [1=y/0=n] ");
	BTST_P_Scanf(pContext, "%d", &iFormatSwitch);

	/* Edit channels */
	printf("Enter number of channels: ");
	BTST_P_Scanf(pContext, "%u", &ulChannelNum);
	/* Use selected source if only one channel */
	if( ulChannelNum > 1 )
	{
		printf("Edit channel map:");
		for( ulIndex = 0; ulIndex < ulChannelNum; ulIndex++ )
		{
			printf("Select channel[%d]: m0(0), m1(1), a0(2), 656(3) ifd(4)\n", ulIndex);
			BTST_P_Scanf(pContext, "%u", &aulTestChannels[ulIndex]);
		}
	}

	for(i = iCount; i ; i--)
	{
		if( (iFormatSwitch) && ((rand() % 5) == 0) )
		{
			/* Change format */
			iOrder = (rand() % (sizeof(aeTestFormats)/sizeof(BFMT_VideoFmt)));
			pContext->peNewFormat = &aeTestFormats[iOrder];
			pContext->iSelectedComp = 0;
			BTST_P_ChgDispFmt(pContext);
			if(BERR_SUCCESS != BTST_P_ApplyChanges(pContext))
			{
				/* stop on failure */
				break;
			}
			/* Delay until changing to the next one. */
			BKNI_Sleep((rand() % 500) + 100);

			/* Print out new format */
			printf("Display[%d]'s new format (%d/%d): %-21s\n",
				pContext->iSelectedComp, i, iCount,
				pContext->apFmtInfo[pContext->iSelectedComp]->pchFormatStr);
		}
		else
		{
#if (BTST_P_DESTROY_CLONED_FIRST)
			iOrder = (pContext->iSelectedVidSrc == 2) ? 1 : 0;
#else
			iOrder = (rand() % 2);
#endif

#if !BTST_VDC_ONLY
			if((pContext->iSelectedVidSrc == 0) ||
				(pContext->iSelectedVidSrc == 1))
			{
				if(pContext->astMDecInfo[pContext->eSelectedMDec].abChnDecoded[pContext->iSelectedMDecCh])
				{
					BTST_StopDecode(pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh] );
					printf("BTST_StopDecode BTST_P_DESTROY_CLONED_FIRST\n");
					pContext->astMDecInfo[pContext->eSelectedMDec].abChnDecoded[pContext->iSelectedMDecCh] = false;
				}
			}
#endif

			/* Select new channel */
			if( ulChannelNum > 1 )
			{
				ulIndex = (i % ulChannelNum);
				printf("[%d/%d]%s\n", i, iCount, aacChanName[aulTestChannels[ulIndex]]);

				if(aulTestChannels[ulIndex] != 4)
				{
					bIfd = false;
					pContext->iSelectedVidSrc = aulTestChannels[ulIndex];

					/* set 656in source format */
					if(aulTestChannels[ulIndex] == 3)
					{
						BVDC_Source_SetVideoFormat(pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc,
							BFMT_VideoFmt_eNTSC);
					}
					else if(aulTestChannels[ulIndex] == 2)
					{
						printf("Select VDEC input\n");
						/* set BVDC_Adc_x to accept VDEC input */
						BVDC_Source_SetAdcConfiguration(pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc,
							BVDC_Adc_0, pContext->aeAdcInput[0]);
						BVDC_Source_SetAdcConfiguration(pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc,
							BVDC_Adc_1, pContext->aeAdcInput[1]);
						BVDC_Source_SetAdcConfiguration(pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc,
							BVDC_Adc_2, pContext->aeAdcInput[2]);
						/* set BVDC_Ibafe_0 to unused */
						BVDC_Source_SetAdcConfiguration(pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc,
							BVDC_Ibafe_0, BVDC_AdcInput_eUnused);
					}
				}
				else
				{/* ifd shares the same vdec source handle as cvbs line-in */
					bIfd = true;
					pContext->iSelectedVidSrc = 2;
					printf("Select RF input\n");

					/* set BVDC_Adc_0, BVDC_Adc_1 and BVDC_Adc_2 to unused */
					BVDC_Source_SetAdcConfiguration(pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc,
						BVDC_Adc_0 | BVDC_Adc_1 | BVDC_Adc_2, BVDC_AdcInput_eUnused);

					/* set BVDC_Ibafe_0 to accept RF from IFDemodulator */
					BVDC_Source_SetAdcConfiguration(pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc,
						BVDC_Ibafe_0,  BVDC_AdcInput_eIfDemod);
				}
			}

			if((pContext->iSelectedVidSrc == 0) ||
				(pContext->iSelectedVidSrc == 1))
			{
				bool bSD = rand() % 2;

				if(bSD)
				{
					printf("SD channel SD display format\n");
					BTST_P_BetterMpegDecode(pContext, NULL, pContext->aSrcContext[pContext->iSelectedVidSrc].ui16VidPid,
						pContext->aSrcContext[pContext->iSelectedVidSrc].ui16PcrPid,
						pContext->aSrcContext[pContext->iSelectedVidSrc].ui16EnhPid,
						pContext->eStreamType, pContext->eVideoCmprStd);
					pContext->peNewFormat = &aeTestFormats[0];
				}
				else
				{
					printf("HD channel HD display format\n");
					BTST_P_BetterMpegDecode(pContext, NULL, pContext->aSrcContext[pContext->iSelectedVidSrc].ui16VidPid,
						pContext->aSrcContext[pContext->iSelectedVidSrc].ui16PcrPid,
						pContext->aSrcContext[pContext->iSelectedVidSrc].ui16EnhPid,
						pContext->eStreamType, pContext->eVideoCmprStd);
					pContext->peNewFormat = &aeTestFormats[3];
				}
				/* change display format according to mpeg channel definition */
				pContext->iSelectedComp = 0;
				BTST_P_ChgDispFmt(pContext);
				if(BERR_SUCCESS != BTST_P_ApplyChanges(pContext))
				{
					/* stop on failure */
					break;
				}
				BKNI_Sleep(600);
			}

			/* Close old channel */
			printf("Destroy order: CMP %d - %d\n", iOrder, 1 - iOrder);
			pContext->iSelectedComp = iOrder;
			BTST_P_CloseVidWin(pContext);
			if(BERR_SUCCESS != BTST_P_ApplyChanges(pContext))
			{
				/* stop on failure */
				break;
			}

			pContext->iSelectedComp = 1 - iOrder;
			BTST_P_CloseVidWin(pContext);
			if(BERR_SUCCESS != BTST_P_ApplyChanges(pContext))
			{
				/* stop on failure */
				break;
			}

			/* Change to new channel */
#if (BTST_P_DESTROY_CLONED_FIRST)
			iOrder = (pContext->iSelectedVidSrc == 2) ? 0 : 1;
#else
			iOrder = (rand() % 2);
#endif

			printf("Create order: CMP %d - %d\n", iOrder, 1 - iOrder);
			pContext->iSelectedComp = iOrder;
			BTST_P_OpenVidWin(pContext);

			pContext->iSelectedComp = 1 - iOrder;
			BTST_P_OpenVidWin(pContext);

			BKNI_Sleep(500);
		}
	}

done:
	return;
}


/***************************************************************************
*
*/
void BTST_P_SwapWindows
( BTST_P_Context          *pContext )
{
	BVDC_Window_Handle hWindow0, hWindow1;
	uint8_t ucZOrderW0, ucZOrderW1;

	int32_t lLeftScreen_W0, lLeftScreen_W1;
	int32_t lTopScreen_W0, lTopScreen_W1;
	uint32_t ulWidth_W0, ulWidth_W1;
	uint32_t ulHeight_W0, ulHeight_W1;

	hWindow0  = pContext->aWinContext[pContext->iSelectedComp][0].hWindow;
	hWindow1  = pContext->aWinContext[pContext->iSelectedComp][1].hWindow;

	/* Get the handle to the two main windows on primary and secondary display. */
	if(!hWindow0 || !hWindow1)
	{
		goto done;
	}

	pContext->abSwapMainPip[pContext->iSelectedComp] =
		pContext->abSwapMainPip[pContext->iSelectedComp] ? false : true;

	BVDC_Window_GetZOrder(hWindow0, &ucZOrderW0);
	BVDC_Window_GetZOrder(hWindow1, &ucZOrderW1);

	BVDC_Window_GetDstRect(hWindow0, &lLeftScreen_W0, &lTopScreen_W0, &ulWidth_W0, &ulHeight_W0);
	BVDC_Window_GetDstRect(hWindow1, &lLeftScreen_W1, &lTopScreen_W1, &ulWidth_W1, &ulHeight_W1);

	/* Swap Z-Order */
	BVDC_Window_SetZOrder(hWindow1, ucZOrderW0);
	BVDC_Window_SetZOrder(hWindow0, ucZOrderW1);

	/* Swap size as well */
	BVDC_Window_SetDstRect(hWindow1, lLeftScreen_W0, lTopScreen_W0, ulWidth_W0, ulHeight_W0);
	BVDC_Window_SetDstRect(hWindow0, lLeftScreen_W1, lTopScreen_W1, ulWidth_W1, ulHeight_W1);
	BVDC_Window_SetScalerOutput(hWindow1, 0, 0, ulWidth_W0, ulHeight_W0);
	BVDC_Window_SetScalerOutput(hWindow0, 0, 0, ulWidth_W1, ulHeight_W1);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

void BTST_P_MpegDecodeMode
( BTST_P_Context          *pContext )
{
#if BTST_VDC_ONLY
	BSTD_UNUSED(pContext);
#else
	int i;
	BERR_Code err;
	BTST_MDEC_ChannelHandle hMpgChn = pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh];

	printf("Do you want to use vsync mode or TSM mode decode? [0=vsync/1=tsm]\n");
	BTST_P_Scanf(pContext, "%d", &i);

	if (IS_XVD(pContext->eSelectedMDec))
	{
#if (BFramework_NUM_XVD_CORES > 0)
		TestError (BXVD_SetVideoDisplayMode(
			(BXVD_ChannelHandle)hMpgChn,
			i? BXVD_DisplayMode_eTSMMode : BXVD_DisplayMode_eVSYNCMode),
			"BXVD_SetVideoDisplayMode");

      pContext->bTSMMode = (i == 1);
#endif
	}

Done:
#endif
	return;
}


/***************************************************************************
*
*/
void BTST_P_ReadWriteISRMisorders
( BTST_P_Context          *pContext )
{
	uint32_t i;
	BSTD_UNUSED(pContext);

	for (i=0; i<=100; i++)
	{
		BKNI_EnterCriticalSection();

		BKNI_Delay(10000);
		printf("in CS\n");
		BKNI_LeaveCriticalSection();

		BKNI_Sleep(500);
	}
}


void BTST_P_SetPixelFormat
( BTST_P_Context		  *pContext )
{

	BVDC_Window_Handle hVidWin = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	uint32_t ulPxlFmt;
	BPXL_Format ePxlFmt = BPXL_eY18_Cb8_Y08_Cr8;
	printf("\nChoose a pixel format\n");
	printf("0. BPXL_eY18_Cr8_Y08_Cb8\n");
	printf("1. BPXL_eCr8_Y18_Cb8_Y08\n");
	printf("2. BPXL_eY18_Cb8_Y08_Cr8\n");
	printf("3. BPXL_eCb8_Y18_Cr8_Y08\n");
	printf("4. BPXL_eY08_Cr8_Y18_Cb8\n");
	printf("5. BPXL_eCr8_Y08_Cb8_Y18\n");
	printf("6. BPXL_eY08_Cb8_Y18_Cr8\n");
	printf("7. BPXL_eCb8_Y08_Cr8_Y18\n");

	printf("8. BPXL_eCr10_Y110_Cb10_Y010\n");
	printf("9. BPXL_eY110_Cr10_Y010_Cb10\n");
	printf("10 BPXL_eY010_Cb10_Y110_Cr10\n");
	printf("11 BPXL_eCb10_Y010_Cr10_Y110\n");
	printf("12 BPXL_eCb10_Y110_Cr10_Y010\n");
	printf("13 BPXL_eY110_Cb10_Y010_Cr10\n");
	printf("14 BPXL_eY010_Cr10_Y110_Cb10\n");
	printf("15 BPXL_eCr10_Y010_Cb10_Y110\n");

	printf("16. BPXL_eX2_Cr10_Y10_Cb10\n");
	printf("17. BPXL_eX2_Y010_Cb10_Y110_X2_Cr10_Y010_Cb10_X2_Y110_Cr10_Y010_X2_Cb10_Y110_Cr10 \n");

	BTST_P_Scanf(pContext, "%u", &ulPxlFmt);

	switch (ulPxlFmt)
	{
	case 0: ePxlFmt = BPXL_eY18_Cr8_Y08_Cb8; break;
	case 1: ePxlFmt = BPXL_eCr8_Y18_Cb8_Y08; break;
	case 2: ePxlFmt = BPXL_eY18_Cb8_Y08_Cr8; break;
	case 3: ePxlFmt = BPXL_eCb8_Y18_Cr8_Y08; break;
	case 4: ePxlFmt = BPXL_eY08_Cr8_Y18_Cb8; break;
	case 5: ePxlFmt = BPXL_eCr8_Y08_Cb8_Y18; break;
	case 6: ePxlFmt = BPXL_eY08_Cb8_Y18_Cr8; break;
	case 7: ePxlFmt = BPXL_eCb8_Y08_Cr8_Y18; break;
	case 8: ePxlFmt = BPXL_eCr10_Y110_Cb10_Y010; break;
	case 9: ePxlFmt = BPXL_eY110_Cr10_Y010_Cb10; break;
	case 10: ePxlFmt = BPXL_eY010_Cb10_Y110_Cr10; break;
	case 11: ePxlFmt = BPXL_eCb10_Y010_Cr10_Y110; break;
	case 12: ePxlFmt = BPXL_eCb10_Y110_Cr10_Y010; break;
	case 13: ePxlFmt = BPXL_eY110_Cb10_Y010_Cr10; break;
	case 14: ePxlFmt = BPXL_eY010_Cr10_Y110_Cb10; break;
	case 15: ePxlFmt = BPXL_eCr10_Y010_Cb10_Y110; break;

	case 16: ePxlFmt = BPXL_eX2_Cr10_Y10_Cb10; break;
	case 17: ePxlFmt = BPXL_eX2_Y010_Cb10_Y110_X2_Cr10_Y010_Cb10_X2_Y110_Cr10_Y010_X2_Cb10_Y110_Cr10; break;
	}

	BVDC_Window_SetPixelFormat(hVidWin, ePxlFmt);
	BTST_P_ApplyChanges(pContext);

	return;
}


/***************************************************************************
*
*/
void BTST_P_ToggleScaleDown
( BTST_P_Context          *pContext )
{
	pContext->bScaleDown ^= true;
	printf("%s\n", pContext->bScaleDown? "ScaleDown":"Full-Screen");
}

/***************************************************************************
*
*/
void BTST_P_SetCoeffTblIndex
( BTST_P_Context          *pContext )
{
	int iCoeffTblIndex;
	BVDC_CoefficientIndex stIndex;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	printf("Enter Coeff Index:  ");
	BTST_P_Scanf(pContext, "%d", &iCoeffTblIndex);
	stIndex.ulSclHorzLuma    = iCoeffTblIndex;
	stIndex.ulSclHorzChroma  = iCoeffTblIndex;
	stIndex.ulSclVertLuma    = iCoeffTblIndex;
	stIndex.ulSclVertChroma  = iCoeffTblIndex;
	stIndex.ulMadHorzLuma    = iCoeffTblIndex;
	stIndex.ulMadHorzChroma  = iCoeffTblIndex;
	stIndex.ulHsclHorzLuma   = iCoeffTblIndex;
	stIndex.ulHsclHorzChroma = iCoeffTblIndex;

	BVDC_Window_SetCoefficientIndex(hVidWin, &stIndex);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

#ifdef BTST_MFD_SIZE
uint32_t g_ulSizeX = 64;
uint32_t g_ulSizeY = 64;
void BTST_P_ChangeMfdSize
( BTST_P_Context          *pContext)
{
	uint32_t x, y;
	printf("Please enter the MFD size [x, y]:\n");
	BTST_P_Scanf(pContext, "%u", &x);
	BTST_P_Scanf(pContext, "%u", &y);
	BKNI_EnterCriticalSection();
	g_ulSizeX = x;
	g_ulSizeY = y;
	BKNI_LeaveCriticalSection();
	printf("x = %u, y = %u\n", g_ulSizeX, g_ulSizeY);
	BSTD_UNUSED(pContext);
}
#endif

BAVC_BarDataType eBarDataType = BAVC_BarDataType_eInvalid;
uint32_t         ulTopLeftBarValue  = 0;
uint32_t         ulBotRightBarValue = 0;
uint32_t         ulAfd              = 0;
void BTST_P_ChangeBarData
( BTST_P_Context          *pContext)
{
	uint32_t        ulTopLeft, ulBotRight, ulType;
	printf("Please enter the Bar Data Type, TopLeftBarValue and BotRightBarValue:\n");
	BTST_P_Scanf(pContext, "%u", &ulType);
	BTST_P_Scanf(pContext, "%u", &ulTopLeft);
	BTST_P_Scanf(pContext, "%u", &ulBotRight);
	BKNI_EnterCriticalSection();
	eBarDataType = (BAVC_BarDataType)(ulType%3);
	ulTopLeftBarValue = ulTopLeft;
	ulBotRightBarValue = ulBotRight;
	ulAfd              = pContext->ulAfd;
	BKNI_LeaveCriticalSection();
	printf("BarType = %u, ulTopLeftBarValue = %u ulBotRightBarValue=%u ulAfd %d \n", ulType%3, ulTopLeftBarValue, ulBotRightBarValue, ulAfd);
}
void BTST_P_RdcBlockOut
( BTST_P_Context          *pContext)
{
	uint32_t i, j, k, ulBlocks = 0;
	BRDC_BlockOut astBlockOut[BRDC_MAX_RDC_BLOCKOUT_LIST_COUNT];

	if (pContext->bRdcBlockOutSet)
	{
		printf("Do you want to change several register blocks or only 1? (single=0/multiple=1)");
		BTST_P_Scanf(pContext, "%u", &j);

		if (!j)
		{
			printf("Which block:\n");
			BTST_P_Scanf(pContext, "%x", &i);

			printf("Enter starting register address of block %d:\n", i);
			BTST_P_Scanf(pContext, "%x", &(astBlockOut[i].ulStartRegAddr));

			printf("Enter block size:\n");
			BTST_P_Scanf(pContext, "%u", &(astBlockOut[i].ulBlockSize));

			printf("Enable? (n=0/y=1)");
			BTST_P_Scanf(pContext, "%u", &k);
			astBlockOut[i].bEnable = (k == 1) ? true : false;

			if (BRDC_SetRdcBlockOut(pContext->hRdc, &astBlockOut[i], i) != BERR_SUCCESS)
				printf("Failed to set register blocks for RDC block out\n");
		}
		else
		{
			printf("Enter the number of register blocks (max is 8) to block out:\n");
			BTST_P_Scanf(pContext, "%u", &ulBlocks);

			printf("Starting on which block:\n");
			BTST_P_Scanf(pContext, "%x", &j);

			for (i=j;i<ulBlocks+j;i++)
			{
				if (i<=7)
				{
					printf("Enter starting register address of block %d:\n", i);
					BTST_P_Scanf(pContext, "%x", &(astBlockOut[i].ulStartRegAddr));

					printf("Enter block size:\n");
					BTST_P_Scanf(pContext, "%u", &(astBlockOut[i].ulBlockSize));

					printf("Enable? (n=0/y=1)");
					BTST_P_Scanf(pContext, "%u", &k);
					astBlockOut[i].bEnable = (k == 1) ? true : false;

					if (BRDC_SetRdcBlockOut(pContext->hRdc, &astBlockOut[i], i) != BERR_SUCCESS)
						printf("Failed to set register blocks for RDC block out\n");
				}
				else
				{
					printf("Max blocks allowed exceeded\n");
				}
			}
		}
	}

	if (!pContext->bRdcBlockOutSet)
	{

		BKNI_Memset((void *)astBlockOut, 0x0, sizeof(BRDC_BlockOut) * BRDC_MAX_RDC_BLOCKOUT_LIST_COUNT);

		printf("Enter the number of register blocks (max is 8) to block out:\n");
		BTST_P_Scanf(pContext, "%u", &ulBlocks);


		for (i=0; i<ulBlocks; i++)
		{
			printf("Enter starting register address of block %d:\n", i);
			BTST_P_Scanf(pContext, "%x", &(astBlockOut[i].ulStartRegAddr));

			printf("Enter block size:\n");
			BTST_P_Scanf(pContext, "%u", &(astBlockOut[i].ulBlockSize));

			printf("Enable? (n=0/y=1)");
			BTST_P_Scanf(pContext, "%u", &j);
			astBlockOut[i].bEnable = (j == 1) ? true : false;

			if (BRDC_SetRdcBlockOut(pContext->hRdc, &astBlockOut[i], i) != BERR_SUCCESS)
				printf("Failed to set register blocks for RDC block out\n");

			pContext->bRdcBlockOutSet = true;
		}
	}

	return;
}

void BTST_P_VfdAsSource_AutoTest
( BTST_P_Context          *pContext)
{
	BFMT_VideoFmt eFmt;

	pContext->bVfdAsSourceAutoTest = true;

	/* Open VDC */
	BTST_P_OpenVdcMisc(pContext);

	/* Open COMP */
	BTST_P_OpenComp(pContext);

	/* Change output format */
	eFmt = BFMT_VideoFmt_e1080i;
	pContext->peNewFormat = &eFmt;
	BTST_P_ChgDispFmt(pContext);

	/* Open video feeder 0 as source */
	BTST_P_Vfd0Select(pContext);
	BTST_P_OpenVidSrc(pContext);

	/* Open window */
	BTST_P_Win0Select(pContext);
	BTST_P_OpenVidWin(pContext);

	/* pause */
	BKNI_Sleep(5000);

	/* Close window */
	BTST_P_CloseVidWin(pContext);

#if 0
	/* Change VFD as source mode to interrupt */
	pContext->aSrcContext[pContext->iSelectedVidSrc].eGfxSurSetting = eGfxSurIsrMode;

	/* Open window */
	BTST_P_OpenVidWin(pContext);

	/* pause */
	BKNI_Sleep(5000);

	/* Close window */
	BTST_P_CloseVidWin(pContext);
#endif

	/* Change VFD as source mode to pull mode */
	pContext->aSrcContext[pContext->iSelectedVidSrc].eGfxSurSetting = eGfxSurPullMode;

	/* Open window */
	BTST_P_OpenVidWin(pContext);

	BTST_P_ApplyChanges(pContext);

	/* pause */
	BKNI_Sleep(5000);

	/* Close window */
	BTST_P_CloseVidWin(pContext);

	/* Change VFD as source mode to double buffer mode */
	pContext->aSrcContext[pContext->iSelectedVidSrc].eGfxSurSetting = eGfxSurDoubleBuf;

	/* Open window */
	BTST_P_OpenVidWin(pContext);

	/* pause */
	BKNI_Sleep(5000);

	/* Close window */
	BTST_P_CloseVidWin(pContext);

	/* Close source */
	BTST_P_CloseVidSrc(pContext);

	/* close COMP */
	BTST_P_CloseComp(pContext);

	/* Close VDC */
	BTST_P_CloseVdcMisc(pContext);

	pContext->bVfdAsSourceAutoTest = false;
}
/***************************************************************************
 *
 */
void BTST_P_ResourceLeak_AutoTest
( BTST_P_Context          *pContext)
{
#if (!BTST_VDC_ONLY)
	uint32_t                 ulStressLoop, ulStressCnt;
	BVDC_Display_Handle      hDisplay;
	const BFMT_VideoInfo    *pHdFmt;
	uint32_t                 ulChannelId=0, ulFormatId=0, ulFmtCnt=0;

	BFMT_VideoFmt            aeVideoFmts[] =
	{
		BFMT_VideoFmt_eNTSC, BFMT_VideoFmt_e480p,
		BFMT_VideoFmt_e720p, BFMT_VideoFmt_e1080i,
		BFMT_VideoFmt_ePAL_G
	};

	printf("Stress test loop count : \n");
	BTST_P_Scanf(pContext, "%d", &ulStressLoop);
	ulStressCnt = ulStressLoop;
	pContext->bCustomDisplay  = false;
	pContext->bPlayback       = true;
	pContext->bResourceLeakAutoTest = true;
	ulFmtCnt = sizeof(aeVideoFmts) / sizeof(BFMT_VideoFmt);

	while(ulStressCnt--) {
		/* choose HD/SD component to open */
		pContext->iSelectedComp   = BVDC_CompositorId_eCompositor0;
		pContext->iSelectedVidSrc = BAVC_SourceId_eMpeg0;
		pContext->eSelectedMDec  = BTST_XVD_0;


		/***************** HD path *****************/
		/* Open VDC */
		BTST_P_OpenVdcMisc(pContext);
		/* Open COMP */
		BTST_P_OpenComp(pContext);

		BTST_OpenDecoderChannel(pContext);

		hDisplay = pContext->ahDisplay[BVDC_CompositorId_eCompositor0];
		BVDC_Display_SetDacConfiguration(hDisplay,BFramework_Dac_Composite_0, BVDC_DacOutput_eUnused);
		BTST_P_ApplyChanges(pContext);

		ulFormatId = rand() % ulFmtCnt;
		pContext->apFmtInfo[pContext->iSelectedComp] = pHdFmt = BFMT_GetVideoFormatInfoPtr(aeVideoFmts[ulFormatId]);
		printf("\n Iteration %d New display format %s chanId %d \n",ulStressCnt, pHdFmt->pchFormatStr, ulChannelId);


		BVDC_Display_SetVideoFormat(hDisplay, aeVideoFmts[ulFormatId]);
		BTST_P_ApplyChanges(pContext);

		/* Open mpeg feeder 0 as source */
		BTST_P_OpenVidSrc(pContext);

		/* Open video window */
		BTST_P_Win0Select(pContext);
		BTST_P_OpenVidWin(pContext);

		/***************** SD path *****************/
		BTST_P_Cmp1Select(pContext);
		BTST_P_OpenComp(pContext);

		/* Open video window */
		BTST_P_Win0Select(pContext);
		BTST_P_OpenVidWin(pContext);


		/* BVN monitor */
		BTST_P_InstallBvnErrCb(pContext);


		/* Channel change */
		ulChannelId = rand() % BTST_PLAYBACK_STREAM_COUNT;
		BTST_P_BetterMpegDecode(pContext, astPlaybackStream[ulChannelId].fn,
			astPlaybackStream[ulChannelId].ui16VidPid, astPlaybackStream[ulChannelId].ui16PcrPid,
			astPlaybackStream[ulChannelId].ui16EnhPid,
			astPlaybackStream[ulChannelId].eStreamType, astPlaybackStream[ulChannelId].eVideoCmprStd);

		/* pause */
		BKNI_Sleep(2000);


		/***************** Graphics window open *****************/
		pContext->iSelectedVidSrc = BAVC_SourceId_eGfx0;
		BTST_P_Cmp0Select(pContext);
		BTST_P_OpenGfxWin(pContext);

		BTST_P_Cmp1Select(pContext);
		BTST_P_OpenGfxWin(pContext);

		/* pause */
		BKNI_Sleep(rand() % 5000 + 1000);

		/* BVN monitor removal */
		BTST_P_InstallBvnErrCb(pContext);

		/***************** Graphics window close *****************/
		BTST_P_CloseGfxWin(pContext);
		BTST_P_Cmp0Select(pContext);
		BTST_P_CloseGfxWin(pContext);

		/***************** video window close *****************/
		BTST_P_Cmp1Select(pContext);
		BTST_P_Win0Select(pContext);
		BTST_P_CloseVidWin(pContext);
		BTST_P_CloseComp(pContext);

		BTST_P_Cmp0Select(pContext);
		BTST_P_Win0Select(pContext);
		BTST_P_CloseVidWin(pContext);


		/* pause */
		BKNI_Sleep(1000);


		/* Close video source */
		pContext->iSelectedVidSrc = BAVC_SourceId_eMpeg0;
		BTST_CloseDecoderChannel(pContext);
		BTST_P_CloseVidSrc(pContext);


		/* close COMP */
		BTST_P_CloseComp(pContext);

		/* Close VDC */
		BTST_P_CloseVdcMisc(pContext);

		BTST_P_CleanUp(pContext);
	}
	pContext->bResourceLeakAutoTest = false;
#else
	BSTD_UNUSED(pContext);
#endif
}
/***************************************************************************
 *
 */
void BTST_P_GetCap
	( BTST_P_Context          *pContext )
{
	BVDC_Capabilities          *pCapabilities;

	if(!pContext->hVdc) goto done;

	pCapabilities    = &pContext->stCapabilities;

	BVDC_GetCapabilities(pContext->hVdc, pCapabilities);

	printf("ulNumCmp = %d\n", pCapabilities->ulNumCmp);
	printf("ulNumMad = %d\n", pCapabilities->ulNumMad);
	printf("ulNumDnr = %d\n", pCapabilities->ulNumDnr);
	printf("ulNumPep = %d\n", pCapabilities->ulNumPep);
	printf("ulNumTab = %d\n", pCapabilities->ulNumTab);
	printf("ulNumDac = %d\n", pCapabilities->ulNumDac);
	printf("ulNumRfm = %d\n", pCapabilities->ulNumRfm);
	printf("ulNumStg = %d\n", pCapabilities->ulNumStg);

	printf("ulNum656Input   = %d\n", pCapabilities->ulNum656Input);
	printf("ulNum656Output  = %d\n", pCapabilities->ulNum656Output);

	printf("ulNumHdmiInput  = %d\n", pCapabilities->ulNumHdmiInput);
	printf("ulNumHdmiOutput = %d\n", pCapabilities->ulNumHdmiOutput);

done:
	return;
}

/***************************************************************************
 *
 */
void BTST_P_Loadrts
	( BTST_P_Context          *pContext )
{
#if (BTST_P_SUPPORT_VICE)
	uint32_t ulEncode=0, i=0;
	static uint32_t* pRtsMemc0, *pRtsMemc1;
#if BCHP_MEMC_ARB_2_REG_START
	static uint32_t* pRtsMemc2=NULL;
#endif

	printf("\n ViCE system supported stg %d", pContext->stCapabilities.ulNumStg);
	if(pContext->stCapabilities.ulNumStg)
	{
#if (!BTST_P_MULTI_RTS) /* 7425*/
	printf("RTS: Single encode(1) or dual encode(2): ");
	BTST_P_Scanf(pContext, "%d", &ulEncode);


	if (ulEncode==1)
	{
		pRtsMemc0 = &aulSingleEncodeRts_Memc0[0];
		pRtsMemc1 = &aulSingleEncodeRts_Memc1[0];
	}
	else
	{
		pRtsMemc0 = &aulDualEncodeRts_Memc0[0];
		pRtsMemc1 = &aulDualEncodeRts_Memc1[0];
	}
#else /* 7435 */ 
	printf("RTS: Single encode(1) or dual encode(2) or quad encode (3) ");
	BTST_P_Scanf(pContext, "%d", &ulEncode);

	if (ulEncode==1)
	{
		pRtsMemc0 = &aulSingleEncodeRts_Memc0[0];
		pRtsMemc1 = &aulSingleEncodeRts_Memc1[0];
#if BCHP_MEMC_ARB_2_REG_START
		pRtsMemc2 = &aulSingleEncodeRts_Memc2[0];
#endif
	}
	else if (ulEncode==2)
	{
		pRtsMemc0 = &aulDualEncodeRts_Memc0[0];
		pRtsMemc1 = &aulDualEncodeRts_Memc1[0];
#if BCHP_MEMC_ARB_2_REG_START
		pRtsMemc2 = &aulDualEncodeRts_Memc2[0];
#endif
	}
	else if (ulEncode==3)
	{
		pRtsMemc0 = &aulQuadEncodeRts_Memc0[0];
		pRtsMemc1 = &aulQuadEncodeRts_Memc1[0];
#if BCHP_MEMC_ARB_2_REG_START
		pRtsMemc2 = &aulQuadEncodeRts_Memc2[0];
#endif
	}
#endif

	for (i=0; i<BTST_LOAD_RTS_RTS_ARRAY_SIZE; i++)
	{
		BREG_Write32(pContext->hReg, BCHP_MEMC_ARB_0_CLIENT_INFO_0+(i*4), pRtsMemc0[i]);
		BREG_Write32(pContext->hReg, BCHP_MEMC_ARB_1_CLIENT_INFO_0+(i*4), pRtsMemc1[i]);
#if BCHP_MEMC_ARB_2_REG_START
		BREG_Write32(pContext->hReg, BCHP_MEMC_ARB_2_CLIENT_INFO_0+(i*4), pRtsMemc2[i]);
#endif
	}
	}
#else
	printf("No multiple RTS supported!");
	BSTD_UNUSED(pContext);
#endif
}
/***************************************************************************
 *
 */
void BTST_P_SetupHeapFromFile
	( BTST_P_Context          *pContext )
{
	char filename[256];
	char sMemc[30], sHeapId[30], sdummy[8];
	int32_t ulHeap, i=0;
	FILE * fHeap;
	const BFMT_VideoInfo *pFmtInfo;
	const BVDC_Heap_Settings *pHeapSettings = NULL;

	printf("\n Please specify the buffer/heap configuration file path");
	BTST_P_Scanf(pContext, "%s", filename);

	fHeap = fopen (filename,"r");
	/* error handle */
	if(!fHeap)
	{
		printf("\n unable to open heap-buffer configuration file     %s\n ", filename);
		BDBG_ASSERT(fHeap);
	}
	else
	{
		printf("\n open heap-buffer configuration file %s ", filename);
	}
	
	while ((!feof(fHeap)) && (i<BTST_P_MEMC_MAX))
	{
		if(!pContext->astMemInfo[i].hMem)
			break;
		
		/* Memc */
		pContext->astMemInfo[i].stVdcHeapSettings = pContext->stSettings.stHeapSettings;

		/* 2HD & 2HD_PIP*/
		fscanf(fHeap, "%s %s %d", sMemc, sHeapId, &ulHeap);
		pContext->astMemInfo[i].stVdcHeapSettings.ulBufferCnt_2HD = ulHeap;
		fscanf(fHeap, "%s %s %d", sMemc, sHeapId,&ulHeap);   
		pContext->astMemInfo[i].stVdcHeapSettings.ulBufferCnt_2HD_Pip = ulHeap;
		
					
		/* HD & HD_PIP*/
		fscanf(fHeap, "%s %s %d", sMemc, sHeapId,&ulHeap);
		pContext->astMemInfo[i].stVdcHeapSettings.ulBufferCnt_HD = ulHeap;
		fscanf(fHeap, "%s %s %d", sMemc, sHeapId,&ulHeap);
		pContext->astMemInfo[i].stVdcHeapSettings.ulBufferCnt_HD_Pip = ulHeap;

		/* SD & SD_PIP*/
		fscanf(fHeap, "%s %s %d", sMemc, sHeapId,&ulHeap);
		pContext->astMemInfo[i].stVdcHeapSettings.ulBufferCnt_SD = ulHeap;
		fscanf(fHeap, "%s %s %d", sMemc, sHeapId,&ulHeap);   
		pContext->astMemInfo[i].stVdcHeapSettings.ulBufferCnt_SD_Pip = ulHeap;

		/* Default second DDR heap settings */
		pContext->astMemInfo[i].stVdcHeapSettings.ePixelFormat_2HD    = BTST_P_MEM_PIXFMT_2HD;
		pContext->astMemInfo[i].stVdcHeapSettings.ePixelFormat_HD     = BTST_P_MEM_PIXFMT_HD;
		pContext->astMemInfo[i].stVdcHeapSettings.ePixelFormat_SD     = BTST_P_MEM_PIXFMT_SD;

		/* blank line */
		fgets(sdummy, 8, fHeap);

		if( pContext->astMemInfo[i].stVdcHeapSettings.ulBufferCnt_2HD ||
			pContext->astMemInfo[i].stVdcHeapSettings.ulBufferCnt_2HD_Pip ||
			pContext->astMemInfo[i].stVdcHeapSettings.ulBufferCnt_HD ||
			pContext->astMemInfo[i].stVdcHeapSettings.ulBufferCnt_HD_Pip ||
			pContext->astMemInfo[i].stVdcHeapSettings.ulBufferCnt_SD ||
			pContext->astMemInfo[i].stVdcHeapSettings.ulBufferCnt_SD_Pip)
		{
			BDBG_MSG(("\n Memc%d addr: %x", i, (uint32_t)pContext->astMemInfo[i].hMem));
			BDBG_MSG(("\n 2HD %4d    2HD_Pip %4d", pContext->astMemInfo[i].stVdcHeapSettings.ulBufferCnt_2HD, pContext->astMemInfo[i].stVdcHeapSettings.ulBufferCnt_2HD_Pip));
			BDBG_MSG(("\n  HD %4d     HD_Pip %4d", pContext->astMemInfo[i].stVdcHeapSettings.ulBufferCnt_HD,  pContext->astMemInfo[i].stVdcHeapSettings.ulBufferCnt_HD_Pip));
			BDBG_MSG(("\n  SD %4d     SD_Pip %4d", pContext->astMemInfo[i].stVdcHeapSettings.ulBufferCnt_SD,  pContext->astMemInfo[i].stVdcHeapSettings.ulBufferCnt_SD_Pip));
			
		}
		i++;
	}
	fclose(fHeap);

	pContext->stSettings.stHeapSettings = pContext->astMemInfo[0].stVdcHeapSettings;

	printf("\nVDC settings are: \n");

	pFmtInfo = BFMT_GetVideoFormatInfoPtr(pContext->stSettings.eVideoFormat);
	printf("eVideoFormat         = %s\n", pFmtInfo->pchFormatStr);
	printf("eDisplayFrameRate    = %d\n", pContext->stSettings.eDisplayFrameRate);
	printf("eColorMatrix_HD      = %d\n", pContext->stSettings.eColorMatrix_HD);
	printf("eColorMatrix_SD      = %d\n", pContext->stSettings.eColorMatrix_SD);
	printf("bVecSwap             = %d\n", pContext->stSettings.bVecSwap);
	printf("eDacDetection        = %d\n", pContext->stSettings.eDacDetection);

	switch(pContext->iSelectedMem)
	{
	case 0:
		pHeapSettings = &pContext->stSettings.stHeapSettings;
		printf("\nMem0 heap settings:\n");
		break;
	case 1:
		pHeapSettings = &pContext->astMemInfo[1].stVdcHeapSettings;
		printf("\nMem1 heap settings:\n");
		break;
	case 2:
		pHeapSettings = &pContext->stSharedHeapSettings;
		printf("\nShared mem heap settings:\n");
		break;
	}

	pHeapSettings = &pContext->astMemInfo[0].stVdcHeapSettings;
	printf("\nMem0 heap settings:\n");
	BTST_P_PrintHeapSettings(pHeapSettings);

	if(pContext->astMemInfo[1].hMem)
	{
		
		pHeapSettings = &pContext->astMemInfo[1].stVdcHeapSettings;
		printf("\nMem1 heap settings:\n");
		BTST_P_PrintHeapSettings(pHeapSettings);
	}

	if(pContext->astMemInfo[2].hMem)
	{
		
		pHeapSettings = &pContext->astMemInfo[2].stVdcHeapSettings;
		printf("\nMem2 heap settings:\n");
		BTST_P_PrintHeapSettings(pHeapSettings);
	}
}
/* end of file */
