/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: cmd_window.c $
* $brcm_Revision: Hydra_Software_Devel/125 $
* $brcm_Date: 10/11/12 4:26p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/portinginterface/vdc/cmd_window.c $
* 
* Hydra_Software_Devel/125   10/11/12 4:26p vanessah
* SW7425-2501: remove vdec input support
* 
* Hydra_Software_Devel/124   10/10/12 11:25a vanessah
* SW7445-10: add emu support for 7445
* 
* Hydra_Software_Devel/123   10/8/12 3:30p syang
* SW7346-572: use BTST_P_ApplyChanges to get uniform error handling;
* avoid holding hVdcCfgLock too long
* 
* Hydra_Software_Devel/122   8/17/12 4:04p vanessah
* SW7425-2501: further cleanup xport file
* 
* Hydra_Software_Devel/121   7/27/12 3:48p vanessah
* SW7425-3150: add video processing skip support
* 
* Hydra_Software_Devel/120   7/9/12 5:28p vanessah
* SW7425-2501: 3548/3556 support removal
* 
* Hydra_Software_Devel/119   6/18/12 4:44p chengs
* SW7584-38: Add VP6/VP8/RV9 support for vdc_test
* 
* Hydra_Software_Devel/118   3/5/12 9:26a yuxiaz
* SW7425-2526: Added tests for runtime capabilities query.
* 
* Hydra_Software_Devel/117   2/17/12 11:29a vanessah
* SW7425-2327: add Bar data handling test
* 
* Hydra_Software_Devel/116   2/15/12 10:00a nilesh
* SW7425-2217: Merged Pid2Buffer Support
* 
* Hydra_Software_Devel/SW7425-2217/1   1/26/12 5:29p nilesh
* SW7425-2217: Added PID2Buffer support
* 
* Hydra_Software_Devel/115   2/13/12 11:37a vanessah
* SW7425-2219: add stop decode cmd upon jira testing
* 
* Hydra_Software_Devel/114   1/13/12 4:33p vanessah
* SW7425-1115: add unittest for MADR/MCVP seamless transactions
* 
* Hydra_Software_Devel/113   1/5/12 3:31p vanessah
* SW7552-182: add min src fmt and dsp fmt config for 1080i
* 
* Hydra_Software_Devel/112   12/14/11 7:47a nilesh
* SW7425-1110: Added CC Dump option toggle
* 
* Hydra_Software_Devel/111   10/27/11 4:40p syang
* SW7425-1429: move per vsync call back from window to display
* 
* Hydra_Software_Devel/110   10/27/11 4:31p syang
* SW7425-1429: move per vsync call back from window to display
* 
* Hydra_Software_Devel/109   10/24/11 4:32p syang
* SW7425-1429: check in after debug
* 
* Hydra_Software_Devel/108   10/21/11 5:45p syang
* SW7425-1429: add test for Vsync callback and GetBuf_isr/ReturnBuf_isr
* 
* Hydra_Software_Devel/107   9/15/11 2:30p pntruong
* SW7425-982: Updated test command for deringing.
*
* Hydra_Software_Devel/106   8/25/11 10:45a tdo
* SW7420-2030: Add apply changes error tracking capability
*
* Hydra_Software_Devel/105   8/15/11 4:47p tdo
* SW7420-2017: Add test to scan all size for scaler enable error
*
* Hydra_Software_Devel/104   7/25/11 5:55p vanessah
* SW7425-465: add cmd to adjust minimum display and src fmt
*
* Hydra_Software_Devel/103   5/4/11 11:28a tdo
* SW7422-417: Add cmd to toggle 3d cap for decode channel open
*
* Hydra_Software_Devel/102   4/27/11 4:15p tdo
* SW7231-133, SW7422-417: Add support for multiple channel decode within
* a decoder with streamer input and playback  Add option for users to
* manually force SVD to only decode AVC, this is needed to do multi
* channel decode with SVD due to memory allocation issue.  Cleanup old
* code.
*
* Hydra_Software_Devel/101   4/22/11 11:05a pntruong
* SW7231-130: Added command to enable/disable scaler configuration.
*
* Hydra_Software_Devel/100   4/20/11 3:38p tdo
* SW7425-365: Remove compiler warnings with new kernel build
*
* Hydra_Software_Devel/99   4/15/11 1:53p tdo
* SW7422-417: Add MVC/SVC support to vdc_test
*
* Hydra_Software_Devel/98   3/29/11 4:51p darnstein
* SW7405-3191: Use BXVD enums when calling a BXVD function.
*
* Hydra_Software_Devel/97   1/26/11 3:17p yuxiaz
* SW7400-2998, SW7425-32: Only reduce cdb/itb buffer size for mosaic
* mode.
*
* Hydra_Software_Devel/96   12/21/10 11:08a tdo
* SW7422-28: Adjust to 3D format size change.  Add test for single MEMC
* scheme.
*
* Hydra_Software_Devel/95   12/12/10 11:19p hongtaoz
* SW7425-32: fixed VDC_TEST_ONLY compile error;
*
* Hydra_Software_Devel/94   12/3/10 3:24p tdo
* SW7422-13: Add new command to set 3D orientation to XVD
*
* Hydra_Software_Devel/93   11/16/10 5:02p pntruong
* SW7231-16: Initial support for 7231.
*
* Hydra_Software_Devel/92   11/11/10 6:45p pntruong
* SW7425-31: Refactored hdm/lvds/hdr/dvo and added support hdmi input on
* stb chips.  Removed dead codes.
*
* Hydra_Software_Devel/91   10/19/10 10:50a pntruong
* SW7425-16, SW3548-1650: Adapted to the removal of
* BXVD_ChannelSettings.bFGTEnable.  Also get rid of all reference to fgt
* in vdctest app.
*
* Hydra_Software_Devel/90   10/18/10 5:12p vanessah
* SW7420-965:   MCVP support for game mode
*
* Hydra_Software_Devel/89   9/27/10 4:07p hongtaoz
* SW7425-32: added VDC_TEST_ONLY compile option;
*
* Hydra_Software_Devel/88   9/7/10 3:19p yuxiaz
* SW7422-28: Add 3D formats to FMT plus additional data structures
*
* Hydra_Software_Devel/87   9/2/10 1:49p vanessah
* SW7422-52:  Two Implementations (MAD + MCVP) on the same chip
*
* Hydra_Software_Devel/86   8/26/10 4:34p vanessah
* SW7420-965:  Game modes consistency for Mad and MCVP
*
* Hydra_Software_Devel/85   8/26/10 2:28p vanessah
* SW7420-965:  MCVP game mode support
*
* Hydra_Software_Devel/84   6/25/10 1:43p darnstein
* SW7468-272: remove do-nothing "VBI passthrough" command.
*
* Hydra_Software_Devel/83   6/17/10 5:59p hongtaoz
* SW3548-2912: added support for pseudo-1080i input for 3DTV; allow API
* to disable SCL vertical phase shift; added 540p60/50 custom formats to
* test pseudo-1080i 3DTV;
*
* Hydra_Software_Devel/82   4/15/10 5:17p pntruong
* SW3548-2889: Added option in vdec setting to allow user to select 11-
* bit fullrange processing.
*
* Hydra_Software_Devel/81   2/16/10 4:25p yuxiaz
* SW7405-3184: Added delta width and height for mosaic resize stress
* test.
*
* Hydra_Software_Devel/80   2/11/10 10:49a pntruong
* SW7405-3829: Moved shrinkwidth to mad settings.
*
* Hydra_Software_Devel/79   2/1/10 6:56p hongtaoz
* SW3548-2678, SW3548-2680, SW3548-2681: added HDMI1.4 3D formats support
* for 3548; added multi-buffer low delay mode support for 1to2 and 2to5
* rate gap cases; reduced capture buffer allocation for interlaced
* pulldown case;
*
* Hydra_Software_Devel/BCM3D/2   1/29/10 7:02p hongtaoz
* SW3548-2680, SW3548-2681: ermoved 1470p custom formats; wrapped 2160i
* custom format with compile flag for legacy 3DTV chipsets;
*
* Hydra_Software_Devel/BCM3D/1   12/17/09 6:29p hongtaoz
* SW3548-2680, SW3548-2681: added new special display format 2160i48 to
* support HDMi1.4 3D format for 11-bit BVN; added BHDR frontend config
* to support 3D video testing;
*
* Hydra_Software_Devel/78   1/28/10 2:41p syang
* SW7405-3829: add option to shrink width to fit mad limit
*
* Hydra_Software_Devel/77   12/22/09 9:36p tdo
* SW7401-3828: Use BTST_P_Scanf() instead of scanf()
*
* Hydra_Software_Devel/76   12/22/09 5:17p tdo
* SW7401-3828: Add AFD support
*
* Hydra_Software_Devel/75   12/4/09 3:56p tdo
* SW7408-23: Add playback feature for digital source
*
* Hydra_Software_Devel/74   12/3/09 3:13p yuxiaz
* SW7405-3120: Added test for overlapping mosaic videos with z-ordering
*
* Hydra_Software_Devel/73   12/2/09 10:13a yuxiaz
* SW7420-462: Added tests for new tuning functions.
*
* Hydra_Software_Devel/72   10/20/09 9:58a yuxiaz
* SW7405-3184: user provides min size for mosaic mode resize test.
*
* Hydra_Software_Devel/71   10/14/09 4:48p tdo
* SW7420-376: Bringup appframework for 7410
*
* Hydra_Software_Devel/70   10/14/09 3:55p yuxiaz
* SW7405-3186: Added source clip stress test.
*
* Hydra_Software_Devel/69   10/13/09 1:16p yuxiaz
* SW7405-3184: Added resize stress test for mosaic mode.
*
* Hydra_Software_Devel/68   9/14/09 4:23p hongtaoz
* SW3548-2471: added test case for MAD allocation option in window
* settings;
*
* Hydra_Software_Devel/67   9/1/09 5:22p tdo
* SW7420-303: Cleanup unused parameters in contrast stretch structure
*
* Hydra_Software_Devel/66   8/28/09 4:41p yuxiaz
* SW3556-672: Added test to get MAD dynamic default settings.
*
* Hydra_Software_Devel/65   8/5/09 6:10p pntruong
* PR57216: Added support for scaler split screen demo modes.
*
* Hydra_Software_Devel/64   7/10/09 5:54p rpan
* PR55188: Convert scanf() to BTST_P_Scanf().
*
* Hydra_Software_Devel/63   7/9/09 10:55a syang
* PR 56526: add un-zoom version for dcmove
*
* Hydra_Software_Devel/62   6/8/09 3:37p yuxiaz
* PR54992: Added automated lipsync stress test.
*
* Hydra_Software_Devel/61   5/22/09 11:21a tdo
* PR54768: Support dual decode streams of different compression type
*
* Hydra_Software_Devel/60   5/21/09 3:13p tdo
* PR54768: Fully support dual decode with same PID
*
* Hydra_Software_Devel/59   5/18/09 11:12a tdo
* PR54768: Fix decode and channel change issue in vdc_test
*
* Hydra_Software_Devel/58   5/12/09 5:32p tdo
* PR54944: Allow bypass of LAB_SAT_GAIN even when DynCont is enabled
*
* Hydra_Software_Devel/57   4/30/09 4:49p albertl
* PR54593: Updated user dvo function to input user matrices in YCbCr->RGB
* and YCbCr->YCbCr format.
*
* Hydra_Software_Devel/56   4/7/09 2:33p tdo
* PR53239: Fix compiling errors
*
* Hydra_Software_Devel/55   3/30/09 3:21p hongtaoz
* PR53037, PR53038: added forced synclock mode and double-buffer support;
* added more VEC locking rates support;
*
* Hydra_Software_Devel/54   3/25/09 5:54p pntruong
* PR52524: Used the index determined by upstream block.
*
* Hydra_Software_Devel/53   3/17/09 1:58p darnstein
* PR53265: Remove BVBI, BVBIlib software. It never worked.
*
* Hydra_Software_Devel/52   3/16/09 10:59p tdo
* PR45785, PR45789: Merge from MCVP branch
*
* Hydra_Software_Devel/7420_mcvp/3   3/4/09 5:25p tdo
* PR45785, PR45789: merge from main branch on 3/04/09
*
* Hydra_Software_Devel/51   3/4/09 4:21p tdo
* PR51106: Support table base dynamic contrast algorithm. Change
* precision of level threshold for histogram data.
*
* Hydra_Software_Devel/7420_mcvp/2   2/11/09 11:32a tdo
* PR 45785, PR 45789: merge from main branch on 2/11/09
*
* Hydra_Software_Devel/49   12/3/08 5:07p jessem
* PR 48992: Added Chroma Histogram tests.
*
* Hydra_Software_Devel/48   11/12/08 3:15p tdo
* PR48642: Provide clipping rect for histo luma region
*
* Hydra_Software_Devel/47   11/4/08 3:49p tdo
* PR48643: Return correct pixel count to nexus for APL calculation
*
* Hydra_Software_Devel/46   10/24/08 3:44p albertl
* PR48023: Added BTST_P_DvoAttenuationRGB.  Removed bit shifting tweaks
* that are no longer necessary.  Help menu no longer invoked by wrong
* commands.
*
* Hydra_Software_Devel/4 * Hydra_Software_Devel/50   2/11/09 9:07a rpan
* PR52001: Fixed a compile warning.
*
* Hydra_Software_Devel/7420_mcvp/1   1/24/09 11:53a syang
* PR 45785, PR 45789: add PI support for new the new module
* MCVP(MCTF+MCDI)
*
5   10/22/08 1:55p jessem
* PR 46489: Removed BSTD_UNUSED(ulTemp) in BTST_P_Sharpness.
*
* Hydra_Software_Devel/44   10/19/08 11:02a yuxiaz
* PR46307: Get previous ANR settings when toggle ANR.
*
* Hydra_Software_Devel/43   10/15/08 4:21p jessem
* PR 46489: Added test for custom sharpness settings.
*
* Hydra_Software_Devel/42   10/14/08 3:43p yuxiaz
* PR46305, PR46307, PR46309: Added unit test.
*
* Hydra_Software_Devel/41   9/30/08 12:35p jessem
* PR 46489: Re-added TN2T support using tighter compiler directive to
* compile only when BVDC_P_SUPPORT_TNT_VER >= 5.
*
* Hydra_Software_Devel/40   9/29/08 4:41p jessem
* PR 46489: Added TN2T support.
*
* Hydra_Software_Devel/39   8/28/08 4:12p tdo
* PR45896: Add new dynamic back-light callback
*
* Hydra_Software_Devel/38   7/21/08 1:12p rpan
* PR36501: Free up allocated RAVE memory before openning a new context.
* More xpt call clean-up.
*
* Hydra_Software_Devel/37   7/18/08 3:21p rpan
* PR36501: Hook up tuner and demodulator with xport.
*
* Hydra_Software_Devel/36   7/11/08 4:48p rpan
* PR43614: Adopted the new bandwidth equation API.
*
* Hydra_Software_Devel/35   6/26/08 4:49p rpan
* PR39529: Added uCode and RM settings for 1080p48Hz panel.
*
* Hydra_Software_Devel/34   6/17/08 11:01a rpan
* PR43614: Added bandwidth equation parameter tests.
*
* Hydra_Software_Devel/33   6/11/08 11:46p tdo
* PR39331: Bringup dynamic contrast for 3548/3556
*
* Hydra_Software_Devel/32   6/11/08 10:13a tdo
* PR41488: Cleaning warnings
*
* Hydra_Software_Devel/31   6/10/08 10:52a tdo
* PR43462: Support CCB for 3549/3556
*
* Hydra_Software_Devel/30   6/9/08 9:18p tdo
* PR39333: Hist bringup
*
* Hydra_Software_Devel/29   6/5/08 8:40p pntruong
* PR41315: Initial secam bringup.
*
* Hydra_Software_Devel/28   6/4/08 11:40a yuxiaz
* PR39807: 3548 bring up.
*
* Hydra_Software_Devel/27   5/21/08 3:46p albertl
* PR39163:  Changed
* * BVDC_Window_Set/GetColorClip to BVDC_Compositor_Set/GetColorClip.
*
* Hydra_Software_Devel/26   5/19/08 5:06p yuxiaz
* PR40122: Add test to use new 10-bit 422 40-bit packed format for ANR on
* 3548.
*
* Hydra_Software_Devel/25   4/14/08 5:23p albertl
* PR39163:  Fixed BTST_P_ColorClip compile warning.
*
* Hydra_Software_Devel/24   4/8/08 7:40p albertl
* PR39163:  Changed to use new color clip interface.
*
* Hydra_Software_Devel/23   4/3/08 2:07p tdo
* PR39333: Add error checking for invalid bin number
*
* Hydra_Software_Devel/22   4/2/08 5:19p tdo
* PR39333: clear unused memory for luma stats
*
* Hydra_Software_Devel/21   3/20/08 1:39p yuxiaz
* PR39807: Static buffer bring up on 3548.
*
* Hydra_Software_Devel/20   3/20/08 10:26a hongtaoz
* PR39154: updated MAD/ANR settings with pixel format;
*
* Hydra_Software_Devel/19   2/20/08 6:24p hongtaoz
* PR39154: added window based API for ANR;
*
* Hydra_Software_Devel/18   2/19/08 4:07p hongtaoz
* PR39154: added MAD game mode test;
*
* Hydra_Software_Devel/17   1/24/08 5:27p tdo
* PR38710: Add blue stretch support
*
* Hydra_Software_Devel/16   1/23/08 11:14a tdo
* PR38710: Rename blue stretch and green stretch to blue boost and green
* boost to match their functionality
*
* Hydra_Software_Devel/15   11/28/07 2:02p tdo
* PR37413: Provide new Api to set offset and size of histogram window and
* read corresponding parameters from specific window
*
* Hydra_Software_Devel/14   11/26/07 11:29p pntruong
* PR35411: Need to honor user matrix.
*
* Hydra_Software_Devel/13   10/30/07 10:46a syang
* PR 29617: modify dcmove so that input size not change as moving
*
* Hydra_Software_Devel/12   10/26/07 8:17p pntruong
* PR35136: adapt new matrix with alpha.
*
* Hydra_Software_Devel/11   10/25/07 3:30p yuxiaz
* PR29569, PR36290: Add FGT support on 7405.
*
* Hydra_Software_Devel/10   10/19/07 11:06a rpan
* PR34857: Fixed a window handle assignment issue.
*
* Hydra_Software_Devel/9   10/16/07 2:44p rpan
* PR34857: First version after merging the latest xvd_vdc_test and
* mvd_vdc_test as of Oct. 16, 2007, 12pm.
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

#include "config.h"
#include "test_util.h"
#include "commands.h"
#include "mpg_decoder.h"
#include "interactive.h"
#include "vdec_input.h"
#include "bvdc_dbg.h"
#include "bkni_multi.h"
#include "bmem_debug.h"
#include "bdbg.h"
#include "bstd_defs.h"
#include "bvdc_tune.h"

BDBG_MODULE(BTST);


#define BTST_SUPPORT_TNT2   0



uint32_t          g_ulAdjQp = 0;

#if (BTST_SUPPORT_TNT2)            /* TNT2 HW base */
static BVDC_SharpnessSettings g_astSharpness[2] =
{
	{ /* HD */
		{ { {0,0},     {8,8},   {8,8},   {24,24},   {8,8},   {8,8},    {8,8},
		{8,8},   {24,24}, {24,24},   {24,24}, {24,24}, {24,24}, {24,24} },
		0, {10,10,5}, true,
		{ {0,0,0,0}, {200,400} }, { {8, 4, -8, -16}, {50,400} }, true, {2,820}, {2,200} },  /* peaking */
		{true, {-32, -32, -16, 0}, {100,200}, 32, true, 0, 0, 12, 12, 4, 0},  /* LTI */
		{32, 10, 1}, /* CTI Hor filter */
		{32, 10, 0}, /* CTI Ver filter */
		true,  /* bChromaSharpnessEnable */
		{ {1, 720, 590, 730, 1, 3,
		{{3, {{590,0}, {670,64}, {690,64}, { 721,0}}},
		{4, {{  1,0}, { 50,64}, {200,64}, { 712,0}}},
		{2, {{400,0}, {520,64}, {780,64}, {1019,0}}}},
		0, true, -32, -48,  10},    /* Region 1 */
		{1, 329, 170, 729, 1, 3,
		{{3, {{190,0}, {222,51}, {250,64}, { 291,0}}},
		{4, {{ 30,0}, { 80,64}, {130,64}, { 190,0}}},
		{2, {{230,0}, {360,64}, {512,64}, {1019,0}}}},
		0, true, -32, -48,  10},    /* Region 2 */
		{1, 550, 350, 550, 20, 3,
		{{3, {{350,0}, {370,38}, {530,38}, { 551,0}}},
		{4, {{ 36,0}, {100,64}, {500,64}, { 564,0}}},
		{2, {{  0,0}, {128,64}, {900,64}, {1019,0}}}},
		0, true,   0,  10, -10},    /* Region 3 */
		{1,   0,   0,   0,  0, 3,
		{{3, {{0,0}, {1,0}, {2,0}, {3,0}}},
		{4, {{0,0}, {3,0}, {4,0}, {5,0}}},
		{2, {{0,0}, {6,0}, {7,0}, {8,0}}}},
		0, true, 0,0,0} },    /* Region 4 */
		0       /* Luma Statistics */
	},
	{ /* SD */
		{ { {0,0},       {0x20,0x20}, {0x20,0x20}, {0,0}, {0x20,0x20}, {0x20,0x20}, {0x20,0x20},
		{0x20,0x20},       {0,0},       {0,0}, {0,0},       {0,0},       {0,0},       {0,0} },
		0x0, {5,5,3}, true,
		{ {8,4,-4,-8}, {50,200} }, { {0,0,0,0}, {50,400} }, true, {1,820}, {1,200} },  /* peaking */
		{true, {-16, -16, 0, 0}, {40, 400}, 32, true, 0, 1, 12, 12, 4, 0},  /* LTI */
		{16, 10, 1}, /* CTI Hor filter */
		{16, 10, 0}, /* CTI Ver filter */
		true,  /* bChromaSharpnessEnable */
		{ {1, 720, 590, 730,  1, 3,
		{{3, {{590,0}, {670,64}, {690,64}, { 721,0}}},
		{4, {{  1,0}, { 50,64}, {200,64}, { 712,0}}},
		{2, {{400,0}, {520,64}, {780,64}, {1019,0}}}},
		0, true, -32, -48, 10},
		{1, 329, 170, 729,  1, 3,
		{{3, {{190,0}, {222,51}, {250,64}, { 291,0}}},
		{4, {{ 30,0}, { 80,64}, {130,64}, { 190,0}}},
		{2, {{230,0}, {360,64}, {512,64}, {1019,0}}}},
		0, true, -32, -48, 10},
		{1, 550, 350, 550, 20, 3,
		{{3, {{350,0}, {370,38}, {530,38}, { 551,0}}},
		{4, {{ 36,0}, {100,64}, {500,64}, { 564,0}}},
		{2, {{  0,0}, {128,64}, {900,64}, {1019,0}}}},
		0, true,  10, -10,  0},
		{1,   0,   0,   0,  0, 3,
		{{3, {{0,0}, {1,0}, {2,0}, {3,0}}},
		{4, {{0,0}, {3,0}, {4,0}, {5,0}}},
		{2, {{0,0}, {6,0}, {7,0}, {8,0}}}},
		0, true,  0,    0,  0} },	 /* 4 Chroma Sharpness regions */
		0     /* Luma Statistics */
		}
};
#endif

/***************************************************************************
*
*/
void BTST_P_MuteMode
( BTST_P_Context          *pContext )
{
	int i;
	BVDC_Source_Handle hSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;

	if(!pContext->hVdc || !hSrc)
	{
		goto Done;
	}

	printf("Please select the source mute mode:\n");
	printf("(0) Unmute; (1) ConstantColor; (2) RepeatMode\n");
	BTST_P_Scanf(pContext, "%d", &i);
	BVDC_Source_SetMuteMode(hSrc,
		(i==0)? BVDC_MuteMode_eDisable:
		(i==1)? BVDC_MuteMode_eConst:
		(i==2)? BVDC_MuteMode_eRepeat:
		BVDC_MuteMode_eDisable);
	if(i==1)
	{
		uint32_t ulR, ulG, ulB;

		printf("Enter new background color (0xRR, 0xGG, 0xBB):");
		BTST_P_Scanf(pContext, "%x %x %x", &ulR, &ulG, &ulB);
		ulR = BTST_P_MIN(ulR, BTST_P_BG_COLOR_RANGE_MAX);
		ulG = BTST_P_MIN(ulG, BTST_P_BG_COLOR_RANGE_MAX);
		ulB = BTST_P_MIN(ulB, BTST_P_BG_COLOR_RANGE_MAX);
		BVDC_Source_SetVideoMuteColor( hSrc, ulR, ulG, ulB);
	}

	BTST_P_ApplyChanges(pContext);
Done:
	return;
}


extern BAVC_XptContextMap       CtxMap[];
/***************************************************************************
*
*/
void BTST_P_BetterMpegDecode
( BTST_P_Context          *pContext,
  char                    *fn,
  uint16_t                 ui16VidPid,
  uint16_t                 ui16PcrPid,
  uint16_t                 ui16EnhPid,
  BAVC_StreamType          eStreamType,
  BAVC_VideoCompressionStd eVideoCmprStd )
{
#if BTST_VDC_ONLY
	BSTD_UNUSED(pContext);
	BSTD_UNUSED(fn);
	BSTD_UNUSED(ui16VidPid);
	BSTD_UNUSED(ui16PcrPid);
	BSTD_UNUSED(ui16EnhPid);
	BSTD_UNUSED(eStreamType);
	BSTD_UNUSED(eVideoCmprStd);
#else
	int iXptChNum = pContext->astMDecInfo[pContext->eSelectedMDec].aiXptChNum[pContext->iSelectedMDecCh];

	/* Not open yet. */
	if(!pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh])
	{
		goto Done;
	}
#ifndef IKOS_EMULATION

	if(pContext->astMDecInfo[pContext->eSelectedMDec].abChnDecoded[pContext->iSelectedMDecCh])
	{
		BTST_StopDecode(pContext);
		StopXPT_Pcr(iXptChNum);
		pContext->astMDecInfo[pContext->eSelectedMDec].abChnDecoded[pContext->iSelectedMDecCh] = false;
	}

	StartXPT(pContext, fn, eStreamType, ui16VidPid, ui16PcrPid, ui16EnhPid, iXptChNum);
	BTST_StartDecode(pContext, ui16VidPid, ui16PcrPid, ui16EnhPid, eStreamType, eVideoCmprStd, iXptChNum);
	pContext->astMDecInfo[pContext->eSelectedMDec].abChnDecoded[pContext->iSelectedMDecCh] = true;

	/* search for slave channel */
	pContext->astMDecInfo[pContext->eSelectedMDec].aui16VidPid[pContext->iSelectedMDecCh] = ui16VidPid;
	pContext->astMDecInfo[pContext->eSelectedMDec].aui16PcrPid[pContext->iSelectedMDecCh] = ui16PcrPid;
	pContext->astMDecInfo[pContext->eSelectedMDec].aui16EnhPid[pContext->iSelectedMDecCh] = ui16EnhPid;
	pContext->astMDecInfo[pContext->eSelectedMDec].aeVideoCmprStd[pContext->iSelectedMDecCh] = eVideoCmprStd;
#endif /* BTST_VDC_ONLY */

Done:
#endif
	return;
}


void BTST_P_MpegDecode
( BTST_P_Context          *pContext )
{
	int                      iStreamType ;
	int                      iVideoCmprStd;
	uint16_t                 ui16VideoPid;
	uint16_t                 ui16PcrPid;
	uint16_t                 ui16EnhPid = 0x1FFF;
	char                     fn[1024];

	if(pContext->bPlayback)
	{
		printf("Enter playback filename: ");
		BTST_P_Scanf(pContext, "%s", fn);
	}

	printf("Enter  Video Pid : ") ;
	BTST_P_Scanf(pContext, "%hx", &ui16VideoPid) ;
	printf("Enter Pcr Pid : ") ;
	BTST_P_Scanf(pContext, "%hx", &ui16PcrPid) ;
	printf("Enter Stream Type : TS(0),DSSES(1),PES(2),ES(3),BES(4),DTVPES(5)") ;
	BTST_P_Scanf(pContext, "%d", &iStreamType) ;
	printf("Enter Video Compression: H.264(0), MPEG2(1), H.261 (2), H.263(3), VC1(4), MPEG1(5), SVC(11), MVC(13), VP6(14), VP8(16), RV9(17) ") ;
	BTST_P_Scanf(pContext, "%d", &iVideoCmprStd) ;
	if(iVideoCmprStd == BAVC_VideoCompressionStd_eSVC ||
	   iVideoCmprStd == BAVC_VideoCompressionStd_eMVC)
	{
		printf("Enter Enhanced Video Pid : ") ;
		BTST_P_Scanf(pContext, "%hx", &ui16EnhPid) ;
	}

	pContext->eStreamType = (BAVC_StreamType) iStreamType;
	pContext->eVideoCmprStd = (BAVC_VideoCompressionStd) iVideoCmprStd;
	pContext->aSrcContext[pContext->iSelectedVidSrc].ui16VidPid = ui16VideoPid;
	pContext->aSrcContext[pContext->iSelectedVidSrc].ui16PcrPid = ui16PcrPid;
	pContext->aSrcContext[pContext->iSelectedVidSrc].ui16EnhPid = ui16EnhPid;

	BTST_P_BetterMpegDecode(pContext, fn, ui16VideoPid, ui16PcrPid, ui16EnhPid,
		(BAVC_StreamType) iStreamType, (BAVC_VideoCompressionStd) iVideoCmprStd);
	return;
}

void BTST_P_StopMpegDecode
( BTST_P_Context *pContext )
{
#if (!BTST_VDC_ONLY)
	int iXptChNum = pContext->astMDecInfo[pContext->eSelectedMDec].aiXptChNum[pContext->iSelectedMDecCh];

	if(pContext->astMDecInfo[pContext->eSelectedMDec].abChnDecoded[pContext->iSelectedMDecCh])
	{
		BTST_StopDecode(pContext);
		StopXPT_Pcr(iXptChNum);
		pContext->astMDecInfo[pContext->eSelectedMDec].abChnDecoded[pContext->iSelectedMDecCh] = false;
	}
#else
	BSTD_UNUSED(pContext);
#endif
}

void BTST_P_MpegDecode3d
( BTST_P_Context          *pContext )
{
#if BTST_VDC_ONLY
	BSTD_UNUSED(pContext);
#else
	BXVD_3DSetting           st3DSettings;
	BTST_MDEC_ChannelHandle  hMpgChn = pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh];

	BXVD_GetDefault3D((BXVD_ChannelHandle)hMpgChn, &st3DSettings );

	printf("Overwrite Source Orientation? [1=Yes/0=No]: ");
	BTST_P_Scanf(pContext, "%d", &st3DSettings.bOverrideOrientation);

	if(st3DSettings.bOverrideOrientation)
	{
		printf("Enter Orientation : 2D(0),LR(1),OU(2),L(3),R(4) ") ;
		BTST_P_Scanf(pContext, "%d", &st3DSettings.eOrientation) ;

		if((BFMT_Orientation)st3DSettings.eOrientation == BFMT_Orientation_e3D_Left ||
		   (BFMT_Orientation)st3DSettings.eOrientation == BFMT_Orientation_e3D_Right)
		{
			printf("Set Next Pointer? [0=false/1=true]: ");
			BTST_P_Scanf(pContext, "%d", &st3DSettings.bSetNextPointer);
		}
	}

	BXVD_Set3D((BXVD_ChannelHandle)hMpgChn, &st3DSettings );
#endif
	return;
}

void BTST_P_Toggle3dDecodeChan
( BTST_P_Context          *pContext )
{
	printf("Toggle Decode Chan 3D: ");
	if(pContext->bDecodeChan3d == false)
	{
		pContext->bDecodeChan3d = true;
		printf("ON \n");
	}
	else
	{
		pContext->bDecodeChan3d = false;
		printf("OFF \n");
	}
}

void BTST_P_TogglePlayback
( BTST_P_Context          *pContext )
{
	printf("Toggle playback: ");
	if(pContext->bPlayback == false)
	{
		pContext->bPlayback = true;
		printf("ON \n");
	}
	else
	{
		pContext->bPlayback = false;
		printf("OFF \n");
	}
}

void BTST_P_ToggleCCDump
( BTST_P_Context          *pContext )
{
   printf("Toggle CC Dump: ");
   if(pContext->bCCDump == false)
   {
      pContext->bCCDump = true;
      printf("ON \n");
   }
   else
   {
      pContext->bCCDump = false;
      printf("OFF \n");
   }
}

void BTST_P_EnablePID2Buffer
( BTST_P_Context          *pContext )
{
   unsigned i;

   printf("Select number of PID2Buffer Channels: ");
   BTST_P_Scanf(pContext, "%u", &pContext->uiNumValidPid2Buf);

   for ( i = 0; i < pContext->uiNumValidPid2Buf; i++ )
   {
      printf("PID2Buffer[%d] Input  PID: ", i);
      BTST_P_Scanf(pContext, "%hx", &pContext->pid2buf[i].uiInputPID);

      pContext->pid2buf[i].uiOutputPID = pContext->pid2buf[i].uiInputPID;

      printf("PID2Buffer[%d] Output PID: ", i);
      BTST_P_Scanf(pContext, "%hx", &pContext->pid2buf[i].uiOutputPID);
   }
}
/***************************************************************************
*
*/
void BTST_P_TogglePsF
( BTST_P_Context          *pContext )
{
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;

	bool bPsF;

	if(!hVidSrc)
	{
		goto done;
	}
	BVDC_Source_GetPsfMode( hVidSrc, &bPsF );
	printf("Old PsF mode: %d. Toggle it!\n", bPsF);

	BVDC_Source_SetPsfMode(hVidSrc, !bPsF);

	if(pContext->bApplyImmediately)
	{
		if(BERR_SUCCESS != BTST_P_ApplyChanges(pContext))
		{
			printf("Source PsF mode change failed!\n");
		}
	}
done:
	return;
}


/***************************************************************************
*
*/
void BTST_P_ChgSrcFmt
( BTST_P_Context          *pContext )
{
	BFMT_VideoFmt eFmt;
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;

	int i;

	if(!hVidSrc)
	{
		goto done;
	}

	BVDC_Source_GetVideoFormat( hVidSrc, &eFmt );
		printf("Enter new input format NTSC(0), 480p(1), 720p(2), 1080i(3), PAL_G(4),\
			\n\tPAL_M(5), PAL_NC(6), SECAM(7)\
			\n\t640x480p(10), 800x600p(11), 640x400p@85Hz(12), 640x480p@72Hz(13), \
			\n\t640x480p@75Hz(14), 640x480p@85Hz(15), 720x400p@85Hz(16), \
			\n\t800x600p@56Hz(17), 800x600p@72Hz(18), 800x600p@75Hz(19), \
			\n\t1024x768i@43Hz(20), 1080p(21), 1024x768p(22), 1600x1200p@60Hz(23), \
			\n\t1080p@24Hz(24), 1080p@25Hz(25), 1080p@30Hz(26), 1080p@50Hz(27), \
			\n\tNTSC_J(28), 576p@50Hz(29), 720p@50Hz(30), 1080i@50Hz(31),");
	printf("\
		   \n\3D_1080p24(32), 3D_720p@60Hz(33), 3D_720p@50Hz(34):");
	BTST_P_Scanf(pContext, "%d", &i);
	if	   (0 == i){eFmt = BFMT_VideoFmt_eNTSC;}
	else if(1 == i){eFmt = BFMT_VideoFmt_e480p;}
	else if(2 == i){eFmt = BFMT_VideoFmt_e720p;}
	else if(3 == i){eFmt = BFMT_VideoFmt_e1080i;}
	else if(4 == i){eFmt = BFMT_VideoFmt_ePAL_G;}
	else if(5 == i){eFmt = BFMT_VideoFmt_ePAL_M;}
	else if(6 == i){eFmt = BFMT_VideoFmt_ePAL_NC;}
	else if(7 == i){eFmt = BFMT_VideoFmt_eSECAM_L;}
	else if(10 == i){eFmt = BFMT_VideoFmt_eDVI_640x480p;}
	else if(11 == i){eFmt = BFMT_VideoFmt_eDVI_800x600p;}
	else if(12 == i){eFmt = BFMT_VideoFmt_eDVI_640x400p_85Hz;}
	else if(13 == i){eFmt = BFMT_VideoFmt_eDVI_640x480p_72Hz;}
	else if(14 == i){eFmt = BFMT_VideoFmt_eDVI_640x480p_75Hz;}
	else if(15 == i){eFmt = BFMT_VideoFmt_eDVI_640x480p_85Hz;}
	else if(16 == i){eFmt = BFMT_VideoFmt_eDVI_720x400p_85Hz;}
	else if(17 == i){eFmt = BFMT_VideoFmt_eDVI_800x600p_56Hz;}
	else if(18 == i){eFmt = BFMT_VideoFmt_eDVI_800x600p_72Hz;}
	else if(19 == i){eFmt = BFMT_VideoFmt_eDVI_800x600p_75Hz;}
	else if(20 == i){eFmt = BFMT_VideoFmt_eDVI_1024x768i_87Hz;}
	else if(21 == i){eFmt = BFMT_VideoFmt_e1080p;}
	else if(22 == i){eFmt = BFMT_VideoFmt_eDVI_1024x768p;}
	else if(23 == i){eFmt = BFMT_VideoFmt_eDVI_1600x1200p_60Hz;}
	else if(24 == i){eFmt = BFMT_VideoFmt_e1080p_24Hz;}
	else if(25 == i){eFmt = BFMT_VideoFmt_e1080p_25Hz;}
	else if(26 == i){eFmt = BFMT_VideoFmt_e1080p_30Hz;}
	else if(27 == i){eFmt = BFMT_VideoFmt_e1080p_50Hz;}
	else if(28 == i){eFmt = BFMT_VideoFmt_eNTSC_J;}
	else if(29 == i){eFmt = BFMT_VideoFmt_e576p_50Hz;}
	else if(30 == i){eFmt = BFMT_VideoFmt_e720p_50Hz;}
	else if(31 == i){eFmt = BFMT_VideoFmt_e1080i_50Hz;}
	else if(32 == i){eFmt = BFMT_VideoFmt_e1080p_24Hz_3DOU_AS;}
	else if(33 == i){eFmt = BFMT_VideoFmt_e720p_60Hz_3DOU_AS;}
	else if(34 == i){eFmt = BFMT_VideoFmt_e720p_50Hz_3DOU_AS;}

	BVDC_Source_SetVideoFormat(hVidSrc, eFmt);

	if(pContext->bApplyImmediately)
	{
		if(BERR_SUCCESS != BTST_P_ApplyChanges(pContext))
		{
			printf("Source format change failed!\n");
			pContext->peNewFormat = NULL;
		}
	}

done:
	return;

}


/***************************************************************************
*
*/
void BTST_P_ChgAutoSrcFmt
( BTST_P_Context          *pContext )
{
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	uint32_t i, ulNumFormats = 0;
#if 0
	uint32_t ulIndex;
#endif
	bool bAutoDetect;
	uint32_t aeFormats[BFMT_VideoFmt_eMaxCount];

	if(!hVidSrc)
	{
		goto done;
	}

	aeFormats[0] = BFMT_VideoFmt_eNTSC;

	printf("Auto Detect Source Format Disable(0), Enable(1): ");
	BTST_P_Scanf(pContext, "%u", &i);
	bAutoDetect = (i == 1) ? true : false;
	printf("Auto Detect %s \n", (bAutoDetect) ? "Enable" : "Disable");


#if 0
	if(bAutoDetect)
	{
		printf("Enter Number of Formats to detect: ");
		BTST_P_Scanf(pContext, "%u", &ulNumFormats);
		printf("Formats supported: NTSC(0), 480p(16), 720p(15), 1080i(14)\n");
	}

	for(ulIndex = 0; ulIndex < ulNumFormats; ulIndex++)
	{
		printf("Enter Format number %d: ", ulIndex+1);
		BTST_P_Scanf(pContext, "%u", &(aeFormats[ulIndex]));
	}
#endif

	BVDC_Source_SetAutoFormat(hVidSrc, bAutoDetect, aeFormats, ulNumFormats);

	if(pContext->bApplyImmediately)
	{
		if(BERR_SUCCESS != BTST_P_ApplyChanges(pContext))
		{
			printf("Source auto format change failed!\n");
			pContext->peNewFormat = NULL;
		}
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_ResizeWin
( BTST_P_Context          *pContext )
{
	uint32_t ulX, ulY, ulWidth, ulHeight;
	const BFMT_VideoInfo *pFmtInfo = pContext->apFmtInfo[pContext->iSelectedComp];
	BVDC_Window_Handle hVidWin     =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	/* Resize, and do some bound checks. */
	printf("Please enter (x, y, width, height):");
	BTST_P_Scanf(pContext, "%u %u %u %u", &ulX, &ulY, &ulWidth, &ulHeight);

	ulWidth  = BTST_P_MIN(pFmtInfo->ulWidth,  ulWidth);
	ulHeight = BTST_P_MIN(pFmtInfo->ulHeight, ulHeight);
	ulX      = BTST_P_MIN((pFmtInfo->ulWidth  - ulWidth), ulX);
	ulY      = BTST_P_MIN((pFmtInfo->ulHeight - ulHeight), ulY);

	BVDC_Window_SetDstRect(hVidWin, ulX, ulY, ulWidth, ulHeight);
	BVDC_Window_SetScalerOutput(hVidWin, 0, 0, ulWidth, ulHeight);

	if(pContext->bApplyImmediately)
	{
		if(BERR_SUCCESS == BTST_P_ApplyChanges(pContext))
		{
			/* Test for PR12682: BTST_P_ApplyChanges still requires sleep afterward*/
			uint32_t ulAppliedX, ulAppliedY, ulAppliedWidth, ulAppliedHeight;
			BVDC_Window_GetDstRect(hVidWin, (int32_t*)&ulAppliedX,
				(int32_t*)&ulAppliedY, &ulAppliedWidth, &ulAppliedHeight);
			BDBG_ASSERT(ulX == ulAppliedX);
			BDBG_ASSERT(ulY == ulAppliedY);
			BDBG_ASSERT(ulWidth  == ulAppliedWidth);
			BDBG_ASSERT(ulHeight == ulAppliedHeight);
		}
	}

done:
	return;
}

/***************************************************************************
*
*/
#include "bchp_scl_0.h"
void BTST_P_Sweep
( BTST_P_Context          *pContext )
{
	uint32_t ulWidth, ulHeight;
	const BFMT_VideoInfo *pFmtInfo = pContext->apFmtInfo[pContext->iSelectedComp];
	BVDC_Window_Handle hVidWin     =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	/* Sweeping through size to find the failing points */
	for(ulWidth = 2; ulWidth < pFmtInfo->ulWidth; ulWidth++)
	{
		for(ulHeight = 2; ulHeight < pFmtInfo->ulHeight; ulHeight++)
		{
			BVDC_Window_SetDstRect(hVidWin, 0, 0, ulWidth, ulHeight);
			BVDC_Window_SetScalerOutput(hVidWin, 0, 0, ulWidth, ulHeight);
			BTST_P_ApplyChanges(pContext);

			/* Read SCL_x_BVB_IN_STATUS and check for ENABLE_ERROR */
			BKNI_Sleep(160);
			if(BVDC_Dbg_Window_GetScalerStatus(hVidWin) & BCHP_SCL_0_BVB_IN_STATUS_ENABLE_ERROR_MASK)
			{
				printf("%d x %d => ENABLE ERROR\n", ulWidth, ulHeight);
			}

			BVDC_Window_SetDstRect(hVidWin, 0, 0, pFmtInfo->ulWidth, pFmtInfo->ulHeight);
			BVDC_Window_SetScalerOutput(hVidWin, 0, 0, pFmtInfo->ulWidth, pFmtInfo->ulHeight);
			BTST_P_ApplyChanges(pContext);

			/* Read SCL_x_BVB_IN_STATUS and check for ENABLE_ERROR */
			BKNI_Sleep(160);
			if(BVDC_Dbg_Window_GetScalerStatus(hVidWin) & BCHP_SCL_0_BVB_IN_STATUS_ENABLE_ERROR_MASK)
			{
				printf("Should get no error at full screen\n");
			}
		}
		printf("Finishing... %d x %d \n", ulWidth, ulHeight);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_Zoom
( BTST_P_Context          *pContext )
{
	int32_t lLeft, lTop;
	uint32_t ulWidth, ulHeight;
	BVDC_Window_Handle hWindow;
	const BFMT_VideoInfo *pFmtInfo;
	bool bGoDown;
	int32_t inc, v_inc, h_inc;
	char ch = ' ';

	hWindow = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	pFmtInfo = pContext->apFmtInfo[pContext->iSelectedComp];

	/* FullScreen? */
	BVDC_Window_GetDstRect(hWindow, &lLeft, &lTop, &ulWidth, &ulHeight);

	printf("Please enter stepping:");
	BTST_P_Scanf(pContext, "%d", &inc);
	inc     = (inc >= 0) ? inc : -inc;

	v_inc = inc * pFmtInfo->ulHeight / pFmtInfo->ulWidth;
	h_inc = inc;

	v_inc = v_inc & ~(pFmtInfo->bInterlaced ? 3 : 1);
	h_inc = h_inc & ~(1);

	/* At least v=4 or h=2 */
	v_inc = BTST_P_MAX(v_inc, 4);
	h_inc = BTST_P_MAX(h_inc, 2);

	if((ulHeight == pFmtInfo->ulHeight) ||
		(ulWidth == pFmtInfo->ulWidth))
	{
		v_inc   = -v_inc;
		h_inc   = -h_inc;
		bGoDown = true;
	}
	else
	{
		bGoDown = false;
	}

	/* Zooming In/Out */
	while(((ulHeight > (pFmtInfo->ulHeight/16)) &&  bGoDown) ||
		((ulHeight < (pFmtInfo->ulHeight   )) && !bGoDown) ||
		((ulWidth  > (pFmtInfo->ulWidth/16)) &&  bGoDown) ||
		((ulWidth  < (pFmtInfo->ulWidth   )) && !bGoDown))
	{
		/* ulWidth  += h_inc;
		ulHeight  = (pFmtInfo->ulHeight * ulWidth) / pFmtInfo->ulWidth; */
		ulHeight  += v_inc;
		ulWidth    = (pFmtInfo->ulWidth * ulHeight) / pFmtInfo->ulHeight;

		ulHeight = BSTT_P_CLP(pFmtInfo->ulHeight/16, pFmtInfo->ulHeight, ulHeight);
		ulWidth  = BSTT_P_CLP(pFmtInfo->ulWidth/16, pFmtInfo->ulWidth, ulWidth);

		lLeft     = (pFmtInfo->ulWidth- ulWidth) / 2;
		lTop      = (pFmtInfo->ulHeight- ulHeight) / 2;

		BVDC_Window_SetDstRect(hWindow, 0, 0, ulWidth, ulHeight);
		/*BVDC_Window_SetDstRect(hWindow, lLeft, lTop, ulWidth, ulHeight); */
		BVDC_Window_SetScalerOutput(hWindow, 0, 0, ulWidth, ulHeight);
		BTST_P_ApplyChanges(pContext);

		if('g' != ch)
		{
			ch = getchar();
		}
	}
	return;
}

/***************************************************************************
*
*/
void BTST_P_DstClipWin
( BTST_P_Context          *pContext )
{
	uint32_t ulX, ulY, ulWidth, ulHeight;
	uint32_t ulAppliedX, ulAppliedY, ulAppliedWidth, ulAppliedHeight;
	const BFMT_VideoInfo *pFmtInfo = pContext->apFmtInfo[pContext->iSelectedComp];
	BVDC_Window_Handle hVidWin     =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetDstRect(hVidWin, (int32_t*)&ulAppliedX,
		(int32_t*)&ulAppliedY, &ulAppliedWidth, &ulAppliedHeight);
	printf("\nDestination rectangle  ( x, y, w, h ): %d, %d, %d, %d\n",
		ulAppliedX, ulAppliedY, ulAppliedWidth, ulAppliedHeight);
	BVDC_Window_GetScalerOutput(hVidWin, &ulAppliedX,
		&ulAppliedY, &ulAppliedWidth, &ulAppliedHeight);
	printf("Scaler output rectangle( x, y, w, h ): %d, %d, %d, %d\n",
		ulAppliedX, ulAppliedY, ulAppliedWidth, ulAppliedHeight);

	/* Resize, and do some bound checks. */
	printf("Please enter new destination rectangle (x, y, width, height):");
	BTST_P_Scanf(pContext, "%u %u %u %u", &ulX, &ulY, &ulWidth, &ulHeight);

	ulWidth  = BTST_P_MIN(ulAppliedWidth - ulAppliedX,  ulWidth);
	ulHeight = BTST_P_MIN(ulAppliedHeight - ulAppliedY, ulHeight);
	ulX      = BTST_P_MIN((pFmtInfo->ulWidth  - ulWidth), ulX);
	ulY      = BTST_P_MIN((pFmtInfo->ulHeight - ulHeight), ulY);

	BVDC_Window_SetDstRect(hVidWin, ulX, ulY, ulWidth, ulHeight);

	if(pContext->bApplyImmediately)
	{
		if(BERR_SUCCESS == BTST_P_ApplyChanges(pContext))
		{
			/* Test for PR12682: BTST_P_ApplyChanges still requires sleep afterward*/
			BVDC_Window_GetDstRect(hVidWin, (int32_t*)&ulAppliedX,
				(int32_t*)&ulAppliedY, &ulAppliedWidth, &ulAppliedHeight);
			BDBG_ASSERT(ulX == ulAppliedX);
			BDBG_ASSERT(ulY == ulAppliedY);
			BDBG_ASSERT(ulWidth  == ulAppliedWidth);
			BDBG_ASSERT(ulHeight == ulAppliedHeight);
		}
	}

done:
	return;
}

#define TST_SCLOUT_CUT_CALC_DST_RECT 0
/***************************************************************************
*
*/
void BTST_P_SclOutCutWin
( BTST_P_Context          *pContext )
{
	uint32_t ulX, ulY, ulWidth, ulHeight;
	uint32_t ulAppliedX, ulAppliedY, ulAppliedWidth, ulAppliedHeight;
	uint32_t ulSclOutX, ulSclOutY, ulSclOutWidth, ulSclOutHeight;
#if (1==TST_SCLOUT_CUT_CALC_DST_RECT)
	const BFMT_VideoInfo *pFmtInfo = pContext->apFmtInfo[pContext->iSelectedComp];
#endif
	BVDC_Window_Handle hVidWin     =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetScalerOutput(hVidWin, &ulAppliedX,
		&ulAppliedY, &ulAppliedWidth, &ulAppliedHeight);
	printf("\nScaler output rectangle( x, y, w, h ): %d, %d, %d, %d\n",
		ulAppliedX, ulAppliedY, ulAppliedWidth, ulAppliedHeight);
	BVDC_Window_GetDstRect(hVidWin, (int32_t*)&ulAppliedX,
		(int32_t*)&ulAppliedY, &ulAppliedWidth, &ulAppliedHeight);
	printf("Destination rectangle  ( x, y, w, h ): %d, %d, %d, %d\n",
		ulAppliedX, ulAppliedY, ulAppliedWidth, ulAppliedHeight);

	/* Resize, and do some bound checks. */
	printf("Please enter new scaler-out rectangle (x, y, width, height):");
	BTST_P_Scanf(pContext, "%u %u %u %u", &ulSclOutX, &ulSclOutY, &ulSclOutWidth, &ulSclOutHeight);
	ulSclOutX = BTST_P_MIN(ulSclOutWidth,  ulSclOutX);
	ulSclOutY = BTST_P_MIN(ulSclOutHeight, ulSclOutY);
	BVDC_Window_SetScalerOutput(hVidWin, ulSclOutX, ulSclOutY, ulSclOutWidth, ulSclOutHeight);

#if (1==TST_SCLOUT_CUT_CALC_DST_RECT)
	ulWidth  = BTST_P_MIN(ulSclOutWidth - ulSclOutX,  pFmtInfo->ulWidth);
	ulHeight = BTST_P_MIN(ulSclOutHeight - ulSclOutY, pFmtInfo->ulHeight);
	ulX      = 0;
	ulY      = 0;
	printf("\nNew destination rectangle  ( x, y, w, h ): %d, %d, %d, %d\n",
		ulX, ulY, ulWidth, ulHeight);
#else
	printf("Please enter new destination rectangle (x, y, width, height):");
	BTST_P_Scanf(pContext, "%u %u %u %u", &ulX, &ulY, &ulWidth, &ulHeight);
#endif
	BVDC_Window_SetDstRect(hVidWin, ulX, ulY, ulWidth, ulHeight);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_DstCutMove
( BTST_P_Context          *pContext )
{
	uint32_t ulBeZoom, ulMode, ulMaxMove;
	uint32_t ulSclOutX, ulSclOutY, ulSclOutWidth, ulSclOutHeight;
	uint32_t ulDstX, ulDstY, ulDstWidth, ulDstHeight;
	uint32_t ulSaveSclOutX, ulSaveSclOutY, ulSaveSclOutWidth, ulSaveSclOutHeight;
	uint32_t ulSaveDstX, ulSaveDstY, ulSaveDstWidth, ulSaveDstHeight;
	uint32_t ulCntr;

	BVDC_Window_Handle hVidWin     =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetScalerOutput(hVidWin, &ulSaveSclOutX,
		&ulSaveSclOutY, &ulSaveSclOutWidth, &ulSaveSclOutHeight);
	BVDC_Window_GetDstRect(hVidWin, (int32_t*)&ulSaveDstX,
		(int32_t*)&ulSaveDstY, &ulSaveDstWidth, &ulSaveDstHeight);

	printf("Enter zoom mode: '1' for zoom, '0' for not-zoom\n");
	BTST_P_Scanf(pContext, "%u", &ulBeZoom);

	printf("set move mode: 1 --> move up\n");
	printf("               2 --> move down\n");
	printf("               3 --> move left\n");
	printf("               4 --> move right\n");
	printf("               5 --> move left-up\n");
	printf("               6 --> move left-down\n");
	printf("               7 --> move right-up\n");
	printf("               8 --> move right-down\n");
	printf("               9 --> zoom\n");
	printf("              10 --> exit this test\n");
	BTST_P_Scanf(pContext, "%u", &ulMode);

	if (ulMode < 9)
	{
		printf("set max move size\n");
		BTST_P_Scanf(pContext, "%u", &ulMaxMove);
	}

	BVDC_Window_GetScalerOutput(hVidWin, &ulSclOutX,
		&ulSclOutY, &ulSclOutWidth, &ulSclOutHeight);
	BVDC_Window_GetDstRect(hVidWin, (int32_t*)&ulDstX,
		(int32_t*)&ulDstY, &ulDstWidth, &ulDstHeight);

	if ((ulBeZoom != 0) && (ulMode != 9))
	{
		ulSclOutWidth  = ulSclOutWidth  * 150 / 100;  /* overscan */
		ulSclOutHeight = ulSclOutHeight * 150 / 100;
		ulSclOutX = (ulSclOutWidth  - ulDstWidth)  / 2;
		ulSclOutY = (ulSclOutHeight - ulDstHeight) / 2;
	}
	else
	{
		ulSclOutX = 0;
		ulSclOutY = 0;
	}

	ulCntr = ulMaxMove;
	while (1)
	{
		switch (ulMode)
		{
		case 1:
			ulSclOutY ++;
			if ((ulDstHeight + ulSclOutY) > ulSclOutHeight)
				ulDstHeight --;
			if ((ulDstHeight < 16) || (ulCntr <= 1))
				ulMode = 10;
			break;
		case 2:
			if (ulSclOutY)
				ulSclOutY --;
			else
			{
				ulDstY ++;
				ulDstHeight --;
			}
			if ((ulDstHeight < 16) || (ulCntr <= 1))
				ulMode = 10;
			break;
		case 3:
			ulSclOutX ++;
			if ((ulDstWidth + ulSclOutX) > ulSclOutWidth)
				ulDstWidth --;
			if ((ulDstWidth < 16) || (ulCntr <= 1))
				ulMode = 10;
			break;
		case 4:
			if (ulSclOutX)
				ulSclOutX --;
			else
			{
				ulDstX ++;
				ulDstWidth --;
			}
			if ((ulDstWidth < 16) || (ulCntr <= 1))
				ulMode = 10;
			break;
		case 5:
			ulSclOutY ++;
			if ((ulDstHeight + ulSclOutY) > ulSclOutHeight)
				ulDstHeight --;
			ulSclOutX ++;
			if ((ulDstWidth + ulSclOutX) > ulSclOutWidth)
				ulDstWidth --;
			if ((ulDstHeight < 16) || (ulDstWidth < 16) || (ulCntr <= 1))
				ulMode = 10;
			break;
		case 6:
			if (ulSclOutY)
				ulSclOutY --;
			else
			{
				ulDstY ++;
				ulDstHeight --;
			}
			ulSclOutX ++;
			if ((ulDstWidth + ulSclOutX) > ulSclOutWidth)
				ulDstWidth --;
			if ((ulDstHeight < 16) || (ulDstWidth < 16) || (ulCntr <= 1))
				ulMode = 10;
			break;
		case 7:
			ulSclOutY ++;
			if ((ulDstHeight + ulSclOutY) > ulSclOutHeight)
				ulDstHeight --;
			if (ulSclOutX)
				ulSclOutX --;
			else
			{
				ulDstX ++;
				ulDstWidth --;
			}
			if ((ulDstHeight < 16) || (ulDstWidth < 16) || (ulCntr <= 1))
				ulMode = 10;
			break;
		case 8:
			if (ulSclOutY)
				ulSclOutY --;
			else
			{
				ulDstY ++;
				ulDstHeight --;
			}
			if (ulSclOutX)
				ulSclOutX --;
			else
			{
				ulDstX ++;
				ulDstWidth --;
			}
			if ((ulDstHeight < 16) || (ulDstWidth < 16) || (ulCntr <= 1))
				ulMode = 10;
			break;
		case 9:
			ulSclOutWidth += 2;
			ulSclOutHeight += 2;
			ulSclOutX ++;
			ulSclOutY ++;
			if ((ulSclOutHeight > 1400) || /* avoid to be too big to cause vdc calcu overflow */
				(ulCntr <= 1))
				ulMode = 10;
			break;
		default:
			ulMode = 10;
			break;
		}

		ulCntr --;
		if (ulMode == 10)
			break;

		printf("dstcut %d %d %d %d %d %d %d %d\n",
			ulSclOutX, ulSclOutY, ulSclOutWidth, ulSclOutHeight,
			ulDstX, ulDstY, ulDstWidth, ulDstHeight);

		BVDC_Window_SetScalerOutput(hVidWin, ulSclOutX, ulSclOutY, ulSclOutWidth, ulSclOutHeight);
		BVDC_Window_SetDstRect(hVidWin, ulDstX, ulDstY, ulDstWidth, ulDstHeight);
		BTST_P_ApplyChanges(pContext);

		BKNI_Sleep(40);
	}

	BVDC_Window_SetScalerOutput(hVidWin, ulSaveSclOutX, ulSaveSclOutY, ulSaveSclOutWidth, ulSaveSclOutHeight);
	BVDC_Window_SetDstRect(hVidWin, ulSaveDstX, ulSaveDstY, ulSaveDstWidth, ulSaveDstHeight);
	BTST_P_ApplyChanges(pContext);

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_AspRatRectClip
( BTST_P_Context          *pContext )
{
	uint32_t ulLeft, ulRight, ulTop, ulBottom, ulSetInIsr;
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];

	if(!hVidSrc || !hDisplay)
	{
		goto done;
	}

	printf("Please enter '1' for _isr setting and '0' for user-mode setting:");
	BTST_P_Scanf(pContext, "%u", &ulSetInIsr);

	printf("Please enter source aspect ratio canvas clip (left, right, top, bottom):");
	BTST_P_Scanf(pContext, "%u %u %u %u", &ulLeft, &ulRight, &ulTop, &ulBottom);
	if (ulSetInIsr)
		BVDC_Source_SetAspectRatioCanvasClip_isr(hVidSrc, ulLeft, ulRight, ulTop, ulBottom);
	else
		BVDC_Source_SetAspectRatioCanvasClip(hVidSrc, ulLeft, ulRight, ulTop, ulBottom);

	printf("Please enter display aspect ratio canvas clip (left, right, top, bottom):");
	BTST_P_Scanf(pContext, "%u %u %u %u", &ulLeft, &ulRight, &ulTop, &ulBottom);
	BVDC_Display_SetAspectRatioCanvasClip(hDisplay, ulLeft, ulRight, ulTop, ulBottom);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_ChgSclFctrRndToler
( BTST_P_Context          *pContext )
{
	uint32_t ul100HrzToler, ul100VrtToler;
	BVDC_Window_Handle hWin     =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hWin)
	{
		goto done;
	}

	printf("Enter horizontal and vertical scale factor rounding tolerance (HFT, VFT):");
	BTST_P_Scanf(pContext, "%u %u", &ul100HrzToler, &ul100VrtToler);
	ul100HrzToler = BTST_P_MIN(ul100HrzToler, 50);
	ul100VrtToler = BTST_P_MIN(ul100VrtToler, 50);
	BVDC_Window_SetScaleFactorRounding(hWin, ul100HrzToler, ul100VrtToler);
	BTST_P_ApplyChanges(pContext);

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_SetAlpha
( BTST_P_Context          *pContext )
{
	uint32_t ulAlpha;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	/* Resize, and do some bound checks. */
	printf("Please enter alpha value (0 - 255):");
	BTST_P_Scanf(pContext, "%u", &ulAlpha);

	BVDC_Window_SetAlpha(hVidWin, (uint8_t)ulAlpha);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_ScaleMode
( BTST_P_Context          *pContext )
{
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	uint32_t  ulNonlinearSrcWidth, ulNonlinearSclOutWidth;

	if(!hVidWin)
	{
		goto done;
	}

	printf("Enter Nonlinear region src and sclOut width, all 0 for linear, one 0 for VDC to calcu\n");
	printf("Enter Nonlinear region src width (0 or bigger than 1/4 of total):\n");
	BTST_P_Scanf(pContext, "%u", &ulNonlinearSrcWidth);
	printf("Enter Nonlinear region SclOut width (0 or bigger than 1/4 of total):\n");
	BTST_P_Scanf(pContext, "%u", &ulNonlinearSclOutWidth);

	BVDC_Window_SetNonLinearScl(hVidWin, ulNonlinearSrcWidth, ulNonlinearSclOutWidth);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
*
*/
void BTST_P_ChgArcMode
( BTST_P_Context          *pContext )
{
	uint32_t ulArcMode;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	/* Resize, and do some bound checks. */
	printf("Aspect ratio correction mode Bypass(0), AllSource(1), AllDestination(2), Bypass_isr(3):");
	BTST_P_Scanf(pContext, "%u", &ulArcMode);

	BVDC_Window_SetAspectRatioMode(hVidWin,
		(1 == ulArcMode) ? BVDC_AspectRatioMode_eUseAllSource :
		(2 == ulArcMode) ? BVDC_AspectRatioMode_eUseAllDestination :
		(3 == ulArcMode) ? BVDC_AspectRatioMode_eBypass_isr
		: BVDC_AspectRatioMode_eBypass);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_ChgChnChgMode
( BTST_P_Context          *pContext )
{
#if BTST_VDC_ONLY
	BSTD_UNUSED(pContext);
#else
	uint32_t ulChnChgMode;
	BTST_MDEC_ChannelHandle hMpgChn = pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh];

	if(!hMpgChn)
	{
		goto done;
	}

	/* Select diffrent channel change mode. */
	printf("Channel change mode Mute(0), PrevChannel(1), NewChannel(2):");
	BTST_P_Scanf(pContext, "%u", &ulChnChgMode);

	if (IS_XVD(pContext->eSelectedMDec))
	{
#if (BFramework_NUM_XVD_CORES > 0)
		BXVD_SetChannelChangeMode((BXVD_ChannelHandle)hMpgChn,
			(0 == ulChnChgMode) ?
BXVD_ChannelChangeMode_eMute : BXVD_ChannelChangeMode_eLastFramePreviousChannel );
#endif
	}

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
#endif
	return;
}


/***************************************************************************
*
*/
void BTST_P_ClipSource
( BTST_P_Context          *pContext )
{
	uint32_t ulLeft, ulRight, ulTop, ulBot;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	/* Clip the source. */
	printf("Please enter clip values (Left, Right, Top, Bot):");
	BTST_P_Scanf(pContext, "%u %u %u %u", &ulLeft, &ulRight, &ulTop, &ulBot);

	BVDC_Window_SetSrcClip(hVidWin, ulLeft, ulRight, ulTop, ulBot);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}




/***************************************************************************
*
*/
void BTST_P_ClipSourceStress
( BTST_P_Context          *pContext )
{
	uint32_t   ulLeft, ulRight, ulTop, ulBot;
	uint32_t   ulDeltaWidth, ulDeltaHeight;
	uint32_t   ulHorzMin, ulHorzMax, ulVertMin, ulVertMax;
	uint32_t   ulHdWidth, ulHdHeight, ulSdWidth, ulSdHeight;
	BVDC_Window_Handle    hHdWin = pContext->aWinContext[0][0].hWindow;
	BVDC_Window_Handle    hSdWin = pContext->aWinContext[1][0].hWindow;
	const BFMT_VideoInfo *pHdFmtInfo, *pSdFmtInfo;

	if(!hHdWin && !hSdWin)
	{
		goto done;
	}

	if(hHdWin)
	{
		pHdFmtInfo = pContext->apFmtInfo[0];
		ulHdWidth = pHdFmtInfo->ulWidth;
		ulHdHeight = pHdFmtInfo->ulHeight;
		printf("HD size: %d %d\n", ulHdWidth, ulHdHeight);
	}

	if(hSdWin)
	{
		pSdFmtInfo = pContext->apFmtInfo[1];
		ulSdWidth = pSdFmtInfo->ulWidth;
		ulSdHeight = pSdFmtInfo->ulHeight;
		printf("SD size: %d %d\n", ulSdWidth, ulSdHeight);
	}

	/* Clip the source. */
	printf("Please enter horizontal clip values (min, max): ");
	BTST_P_Scanf(pContext, "%u %u", &ulHorzMin, &ulHorzMax);

	printf("Please enter vertical clip values (min, max): ");
	BTST_P_Scanf(pContext, "%u %u", &ulVertMin, &ulVertMax);

	printf("Please enter delta clip values (horizontal, vertical): ");
	BTST_P_Scanf(pContext, "%u %u", &ulDeltaWidth, &ulDeltaHeight);

	ulLeft = ulRight = ulHorzMin;
	ulTop  = ulBot   = ulVertMin;

	while((ulLeft < ulHorzMax) || (ulTop < ulVertMax))
	{
		printf("Current clip values: %d %d %d %d\n", ulLeft, ulRight, ulTop, ulBot);

		if(hHdWin)
		{
			BVDC_Window_SetSrcClip(hHdWin, ulLeft, ulRight, ulTop, ulBot);
			BTST_P_ApplyChanges(pContext);
			BKNI_Sleep(500);
		}

		if(hSdWin)
		{
			BVDC_Window_SetSrcClip(hSdWin, ulLeft, ulRight, ulTop, ulBot);
			BTST_P_ApplyChanges(pContext);
			BKNI_Sleep(1000);
		}

		if(ulLeft < ulHorzMax)
		{
			ulLeft += ulDeltaWidth;
			ulRight+= ulDeltaWidth;
		}

		if(ulTop < ulVertMax)
		{
			ulTop += ulDeltaHeight;
			ulBot += ulDeltaHeight;
		}
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_ColorClip
( BTST_P_Context          *pContext )
{
	BVDC_ColorClipSettings stColorClipSettings;
	uint32_t               ulMode;
	BVDC_Compositor_Handle hCompositor = pContext->ahCompositor[pContext->iSelectedComp];
	if(!hCompositor)
	{
		goto done;
	}

	BVDC_Compositor_GetColorClip(hCompositor, &stColorClipSettings);

	printf("Current color clip mode: %d\n", stColorClipSettings.eColorClipMode);
	printf("Please enter color clip mode: Disable(0), White(1), Black(2), Both(3): ");
	BTST_P_Scanf(pContext, "%u", &ulMode);
	stColorClipSettings.eColorClipMode = (BVDC_ColorClipMode)ulMode;

	BVDC_Compositor_SetColorClip(hCompositor, &stColorClipSettings);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
*
*/
void BTST_P_LoadCabTable
( BTST_P_Context          *pContext )
{
	uint32_t ulCabMode;
	uint32_t ulOffset = 0;
	uint32_t ulSize = 0;
	uint32_t *pulCabTable;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	printf("Load CAB table? [yes=1/no=0]: ");
	BTST_P_Scanf(pContext, "%u", &ulCabMode);

	if(ulCabMode == 0)
	{
		pulCabTable = NULL;
	}
	else
	{
		uint32_t id;

		printf("Enter Offset of CAB table to load: ");
		BTST_P_Scanf(pContext, "%u", &ulOffset);
		printf("Enter size of CAB table to load: ");
		BTST_P_Scanf(pContext, "%u", &ulSize);

		pulCabTable = BKNI_Malloc(sizeof(uint32_t) * ulSize);
		for(id = 0; id < ulSize; id++)
		{
			/* for testing, load cab table with the bypass bit and its index */
			*(pulCabTable + id) = 0x100000 + id + ulOffset;
		}
	}

	BVDC_Window_LoadCabTable(hVidWin, pulCabTable, ulOffset, ulSize);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
*
*/
void BTST_P_LoadLabTable
( BTST_P_Context          *pContext )
{
	uint32_t ulLabMode;
	uint32_t ulOffset = 0;
	uint32_t ulSize = 0;
	uint32_t *pulLabTable;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	printf("Load LAB luma table? [yes=1/no=0]: ");
	BTST_P_Scanf(pContext, "%u", &ulLabMode);

	if(ulLabMode == 0)
	{
		pulLabTable = NULL;
	}
	else
	{
		uint32_t id;

		printf("Enter Offset of LAB table to load: ");
		BTST_P_Scanf(pContext, "%u", &ulOffset);
		printf("Enter size of LAB table to load: ");
		BTST_P_Scanf(pContext, "%u", &ulSize);

		pulLabTable = BKNI_Malloc(sizeof(uint32_t) * ulSize);
		for(id = 0; id < ulSize; id++)
		{
			/* for testing, load Lab table with the value = its index */
			*(pulLabTable + id) = id + ulOffset;
		}
	}

	BVDC_Window_LoadLabTableCustomized(hVidWin, pulLabTable, NULL, NULL, ulOffset, ulSize);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
static void BTST_P_GetBacklightCb_isr
( void                            *pvParam1,
 int                              iParam2,
 void                            *pvVdcData )
{
	const BVDC_Backlight_CallbackData *pCbData = (const BVDC_Backlight_CallbackData*)pvVdcData;

	BSTD_UNUSED(pvParam1);
	BSTD_UNUSED(iParam2);

	printf("Back light Scaling Factor: 0x%x (S1.%d)\n",
		pCbData->iScalingFactor, pCbData->ulShift);

	return;
}

/***************************************************************************
*
*/
void BTST_P_ContrastStretch
( BTST_P_Context          *pContext )
{
	uint32_t ulLabStretchMode;
	bool bCSEnable;
	int iNumerator = 1, iDenominator = 1;
	uint32_t ulCallBack;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	BVDC_ContrastStretch stContrastStretch;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetContrastStretch(hVidWin, &stContrastStretch);

	printf("Enable Contrast Stretch?: [Disable=0/Enable=1]: ");
	BTST_P_Scanf(pContext, "%u", &ulLabStretchMode);
	bCSEnable = (ulLabStretchMode) ? true : false;
	BVDC_Window_EnableContrastStretch(hVidWin, bCSEnable);

	printf("Configure contrast stretch? [No=0/Yes=1]: ");
	BTST_P_Scanf(pContext, "%u", &ulLabStretchMode);
	if(ulLabStretchMode)
	{
		uint32_t ulBypassSat;

		printf("Current fixed point precision = %d\n", stContrastStretch.ulShift);
		printf("Enter new precision for gain: ");
		BTST_P_Scanf(pContext, "%u", &stContrastStretch.ulShift);

		printf("Current gain value = 0x%x\n", stContrastStretch.iGain);
		printf("Format for gain can be in fractional. i.e: 0.5 = 1/2\n");
		printf("Gain numerator: ");
		BTST_P_Scanf(pContext, "%d", &iNumerator);
		printf("Gain denominator: ");
		BTST_P_Scanf(pContext, "%d", &iDenominator);
		stContrastStretch.iGain = (iNumerator << stContrastStretch.ulShift) / iDenominator;

		printf("Bypass Sat (Current=%s)[Normal=0/Bypass=1]: ", stContrastStretch.bBypassSat ? "BYPASS" : "NORMAL");
		BTST_P_Scanf(pContext, "%u", &ulBypassSat);
		stContrastStretch.bBypassSat = (ulBypassSat) ? true : false;

		printf("Install backlight callback? [No=0/Yes=1]: ");
		BTST_P_Scanf(pContext, "%u", &ulCallBack);
		if(ulCallBack != 0)
		{
			stContrastStretch.pfCallback = BTST_P_GetBacklightCb_isr;
		}
		else
		{
			stContrastStretch.pfCallback = NULL;
		}

		BVDC_Window_SetContrastStretch(hVidWin, &stContrastStretch);
	}

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
*
*/
void BTST_P_BlueStretch
( BTST_P_Context          *pContext )
{
	uint32_t ulBlueStretch;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	BVDC_BlueStretch stBlueStretch;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetBlueStretch(hVidWin, &stBlueStretch);

	printf("Enable Blue Stretch?: [Disable=0/Enable=1]: ");
	BTST_P_Scanf(pContext, "%u", &ulBlueStretch);

	if(ulBlueStretch)
	{
		printf("Enter blue stretch offset (Current=%d): ", stBlueStretch.ulBlueStretchOffset);
		BTST_P_Scanf(pContext, "%u", &stBlueStretch.ulBlueStretchOffset);
		printf("Enter blue stretch slope  (Current=%d): ", stBlueStretch.ulBlueStretchSlope);
		BTST_P_Scanf(pContext, "%u", &stBlueStretch.ulBlueStretchSlope);
		BVDC_Window_SetBlueStretch(hVidWin, &stBlueStretch);
	}
	else
	{
		BVDC_Window_SetBlueStretch(hVidWin, NULL);
	}

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
*
*/
void BTST_P_SetCabParams
( BTST_P_Context          *pContext )
{
	uint32_t ulFleshtone, ulBlueBoost, ulGreenBoost;

	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	printf("Please enter value from 0 to 4 for these settings, 0 to disable\n");

	BVDC_Window_GetAutoFlesh(hVidWin, &ulFleshtone);
	printf("Flesh tone [%d]: ", ulFleshtone);
	BTST_P_Scanf(pContext, "%u", &ulFleshtone);

	BVDC_Window_GetGreenBoost(hVidWin, &ulGreenBoost);
	printf("Green Boost level [%d]: ", ulGreenBoost);
	BTST_P_Scanf(pContext, "%u", &ulGreenBoost);

	BVDC_Window_GetBlueBoost(hVidWin, &ulBlueBoost);
	printf("Blue Boost level [%d]: ", ulBlueBoost);
	BTST_P_Scanf(pContext, "%u", &ulBlueBoost);

	BVDC_Window_SetAutoFlesh(hVidWin, ulFleshtone);
	BVDC_Window_SetGreenBoost(hVidWin, ulGreenBoost);
	BVDC_Window_SetBlueBoost(hVidWin, ulBlueBoost);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_SetCmsParams
( BTST_P_Context          *pContext )
{
	BVDC_ColorBar      stSatGain;
	BVDC_ColorBar      stHueGain;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetCmsControl(hVidWin, &stSatGain, &stHueGain);

	printf("Saturation level [-140 ~ 140] (0 = Disable):\n");
	printf("Green   [%3d]: ", stSatGain.lGreen);
	BTST_P_Scanf(pContext, "%d", &stSatGain.lGreen);
	printf("Yellow  [%3d]: ", stSatGain.lYellow);
	BTST_P_Scanf(pContext, "%d", &stSatGain.lYellow);
	printf("Red     [%3d]: ", stSatGain.lRed);
	BTST_P_Scanf(pContext, "%d", &stSatGain.lRed);
	printf("Magenta [%3d]: ", stSatGain.lMagenta);
	BTST_P_Scanf(pContext, "%d", &stSatGain.lMagenta);
	printf("Blue    [%3d]: ", stSatGain.lBlue);
	BTST_P_Scanf(pContext, "%d", &stSatGain.lBlue);
	printf("Cyan    [%3d]: ", stSatGain.lCyan);
	BTST_P_Scanf(pContext, "%d", &stSatGain.lCyan);

	printf("Hue level [-50 ~ 50] (0 = Disable):\n");
	printf("Green   [%3d]: ", stHueGain.lGreen);
	BTST_P_Scanf(pContext, "%d", &stHueGain.lGreen);
	printf("Yellow  [%3d]: ", stHueGain.lYellow);
	BTST_P_Scanf(pContext, "%d", &stHueGain.lYellow);
	printf("Red     [%3d]: ", stHueGain.lRed);
	BTST_P_Scanf(pContext, "%d", &stHueGain.lRed);
	printf("Magenta [%3d]: ", stHueGain.lMagenta);
	BTST_P_Scanf(pContext, "%d", &stHueGain.lMagenta);
	printf("Blue    [%3d]: ", stHueGain.lBlue);
	BTST_P_Scanf(pContext, "%d", &stHueGain.lBlue);
	printf("Cyan    [%3d]: ", stHueGain.lCyan);
	BTST_P_Scanf(pContext, "%d", &stHueGain.lCyan);

	BVDC_Window_SetCmsControl(hVidWin, &stSatGain, &stHueGain);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
*
*/
void BTST_P_ToggleAllPep
( BTST_P_Context          *pContext )
{
	uint32_t ulEnable;
	BVDC_Window_SplitScreenSettings stSplitScreen;
	BVDC_BlueStretch stBlueStretch;
	BVDC_ColorBar stSatGain, stHueGain;

	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	printf("Enable all PEP blocks? Disable(0), Enable(1): ");
	BTST_P_Scanf(pContext, "%u", &ulEnable);

	/* LAB */
	BVDC_Window_EnableContrastStretch(hVidWin, (bool)ulEnable);

	/* CAB */
	BVDC_Window_LoadCabTable(hVidWin, NULL, 0, 0);
	stSatGain.lGreen   = 0;
	stSatGain.lYellow  = 0;
	stSatGain.lRed     = 0;
	stSatGain.lMagenta = 0;
	stSatGain.lBlue    = 0;
	stSatGain.lCyan    = 0;
	stHueGain.lGreen   = 0;
	stHueGain.lYellow  = 0;
	stHueGain.lRed     = 0;
	stHueGain.lMagenta = 0;
	stHueGain.lBlue    = 0;
	stHueGain.lCyan    = 0;
	BVDC_Window_SetCmsControl(hVidWin, &stSatGain, &stHueGain);
	BVDC_Window_SetAutoFlesh(hVidWin, ulEnable);
	BVDC_Window_SetGreenBoost(hVidWin, ulEnable);
	BVDC_Window_SetBlueBoost(hVidWin, ulEnable);

	/* TAB/TNT */
	BVDC_Window_SetSharpness(hVidWin, ulEnable, 10000);

	stBlueStretch.ulBlueStretchOffset = 68;
	stBlueStretch.ulBlueStretchSlope = 4;
	BVDC_Window_SetBlueStretch(hVidWin, &stBlueStretch);

	/* Demo mode */
	stSplitScreen.eHue = 0;
	stSplitScreen.eBrightness = 0;
	stSplitScreen.eContrast = 0;
	stSplitScreen.eColorTemp= 0;
	stSplitScreen.eSharpness = ulEnable;
	stSplitScreen.eAutoFlesh = ulEnable;
	stSplitScreen.eBlueBoost = ulEnable;
	stSplitScreen.eGreenBoost = ulEnable;
	stSplitScreen.eCms = ulEnable;
	stSplitScreen.eContrastStretch = ulEnable;
	stSplitScreen.eBlueStretch = ulEnable;
	stSplitScreen.eCm3d = 0;
	stSplitScreen.eDeJagging = ulEnable;
	stSplitScreen.eDeJagging = ulEnable;

	BVDC_Window_SetSplitScreenMode(hVidWin, &stSplitScreen);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_SetLumaStatConfig
( BTST_P_Context          *pContext )
{
	BVDC_LumaSettings  stLumaConfig;
	uint32_t           ulStatConfig;
	uint32_t           ulNumBin;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	BVDC_Compositor_Handle hCompositor = pContext->ahCompositor[pContext->iSelectedComp];

	if(!hVidWin)
	{
		goto done;
	}

	printf("Set Comp Luma Stats Configuration? [no=0/yes=1]: ");
	BTST_P_Scanf(pContext, "%d", (int *)&ulStatConfig);
	if(ulStatConfig)
	{
		printf("Please enter Luma Clip Rect(left, right, top, bottom):");
		BTST_P_Scanf(pContext, "%u %u %u %u", &stLumaConfig.stRegion.ulLeft,
			&stLumaConfig.stRegion.ulRight,
			&stLumaConfig.stRegion.ulTop,
			&stLumaConfig.stRegion.ulBottom);
		BVDC_Compositor_SetLumaStatsConfiguration(hCompositor, &stLumaConfig);
	}
	else
	{
		BVDC_Compositor_SetLumaStatsConfiguration(hCompositor, NULL);
	}

	printf("Set Window Luma Stats Configuration? [no=0/yes=1]: ");
	BTST_P_Scanf(pContext, "%d", (int *)&ulStatConfig);
	if(ulStatConfig)
	{
		BVDC_Window_GetLumaStatsConfiguration(hVidWin, &stLumaConfig);

		printf("Current Histo Clip Rect (%u, %u, %u, %u)\n",
			stLumaConfig.stRegion.ulLeft,
			stLumaConfig.stRegion.ulRight,
			stLumaConfig.stRegion.ulTop,
			stLumaConfig.stRegion.ulBottom);
		printf("Please enter Histo Clip Rect(left, right, top, bottom):");
		BTST_P_Scanf(pContext, "%u %u %u %u", &stLumaConfig.stRegion.ulLeft,
			&stLumaConfig.stRegion.ulRight,
			&stLumaConfig.stRegion.ulTop,
			&stLumaConfig.stRegion.ulBottom);

		printf("Please enter Histo numbin option[Current = %d]: ",
			stLumaConfig.eNumBins);
		BTST_P_Scanf(pContext, "%u", &ulNumBin);
		stLumaConfig.eNumBins = (BVDC_HistBinSelect)ulNumBin;

		printf("Please enter 4 threshold levels[Current = %d %d %d %d]: ",
			stLumaConfig.aulLevelThres[0],
			stLumaConfig.aulLevelThres[1],
			stLumaConfig.aulLevelThres[2],
			stLumaConfig.aulLevelThres[3]);
		BTST_P_Scanf(pContext, "%u %u %u%u", &stLumaConfig.aulLevelThres[0],
			&stLumaConfig.aulLevelThres[1],
			&stLumaConfig.aulLevelThres[2],
			&stLumaConfig.aulLevelThres[3]);

		BVDC_Window_SetLumaStatsConfiguration(hVidWin, &stLumaConfig);
	}
	else
	{
		BVDC_Window_SetLumaStatsConfiguration(hVidWin, NULL);
	}

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
*
*/
void BTST_P_GetLumaStatus
( BTST_P_Context          *pContext )
{
	BVDC_LumaStatus    stLumaStatus;
	BVDC_LumaSettings  stLumaConfig;
	uint32_t           i;
	uint32_t           ulTotalCnt = 0;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	BVDC_Compositor_Handle hCompositor = pContext->ahCompositor[pContext->iSelectedComp];

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetLumaStatsConfiguration(hVidWin, &stLumaConfig);
	BKNI_Memset((void*)&stLumaStatus, 0x0, sizeof(BVDC_LumaStatus));

	if(BERR_SUCCESS == BVDC_Window_GetLumaStatus(hVidWin, &stLumaStatus))
	{
		printf("\nCurrent Luma status for window %d:\n", pContext->iSelectedWin);
		printf("	Sum = %d\n", stLumaStatus.ulSum);
		printf("	Min = %d\n", stLumaStatus.ulMin);
		printf("	Max = %d\n", stLumaStatus.ulMax);
		printf("	Pixel Cnt = %d\n", stLumaStatus.ulPixelCnt);
		for(i = 0; i < BVDC_LUMA_HISTOGRAM_COUNT; i++)
		{
			printf("	Histo[%2d]   = %d\n", i, stLumaStatus.aulHistogram[i]);
			ulTotalCnt += stLumaStatus.aulHistogram[i];
		}
		printf("	Total = %d, Expect = %d, delta = %d\n",
			ulTotalCnt, stLumaStatus.ulPixelCnt, ulTotalCnt - stLumaStatus.ulPixelCnt);

		for(i = 0; i < BVDC_LUMA_HISTOGRAM_LEVELS; i++)
		{
			printf("	Thresh[%d] = %d\n", stLumaConfig.aulLevelThres[i], stLumaStatus.aulLevelStats[i]);
		}
	}

	if(BERR_SUCCESS == BVDC_Compositor_GetLumaStatus(hCompositor, &stLumaStatus))
	{
		printf("\nCurrent Luma status for compositor %d:\n", pContext->iSelectedComp);
		printf("	Sum = %d\n", stLumaStatus.ulSum);
		printf("	Min = %d\n", stLumaStatus.ulMin);
		printf("	Max = %d\n", stLumaStatus.ulMax);
		printf("	Pixel Cnt = %d\n", stLumaStatus.ulPixelCnt);
		for(i = 0; i < BVDC_LUMA_HISTOGRAM_COUNT; i++)
			printf("	Histo[%2d]   = %d\n", i, stLumaStatus.aulHistogram[i]);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_SetChromaStatConfig
( BTST_P_Context          *pContext )
{
	BVDC_ChromaSettings  stChromaConfig;
	uint32_t             ulAns;
	BVDC_Window_Handle   hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	printf("Set Window Chroma Stats Configuration? [no=0/yes=1]: ");
	BTST_P_Scanf(pContext, "%d", (int *)&ulAns);
	if(ulAns)
	{
		BVDC_Window_GetChromaStatsConfiguration(hVidWin, &stChromaConfig);

		printf("Current Chroma Histogram Clip Rect (%u, %u, %u, %u)\n",
			stChromaConfig.stRegion.ulLeft,
			stChromaConfig.stRegion.ulRight,
			stChromaConfig.stRegion.ulTop,
			stChromaConfig.stRegion.ulBottom);
		printf("Please enter Chroma Histogram Clip Rect amount(left, right, top, bottom):");
		BTST_P_Scanf(pContext, "%u %u %u %u", &stChromaConfig.stRegion.ulLeft,
			&stChromaConfig.stRegion.ulRight,
			&stChromaConfig.stRegion.ulTop,
			&stChromaConfig.stRegion.ulBottom);

		printf("Please enter Chroma Histogram type (0=Hue_Sat/1=Cr_Cb): ");
		BTST_P_Scanf(pContext, "%u", &ulAns);
		stChromaConfig.eType = (BVDC_ChromaHistType)ulAns;

		printf("Please enter Minimum Saturation : ");
		BTST_P_Scanf(pContext, "%u", &ulAns);
		stChromaConfig.ulSatMin = ulAns;

		printf("Please enter Minimum Hue : ");
		BTST_P_Scanf(pContext, "%u", &ulAns);
		stChromaConfig.ulHueMin = ulAns;

		printf("Please enter Maximum Hue : ");
		BTST_P_Scanf(pContext, "%u", &ulAns);
		stChromaConfig.ulHueMax = ulAns;

		BVDC_Window_SetChromaStatsConfiguration(hVidWin, &stChromaConfig);
	}
	else
	{
		BVDC_Window_SetChromaStatsConfiguration(hVidWin, NULL);
	}

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
*
*/
void BTST_P_GetChromaStatus
( BTST_P_Context          *pContext )
{
	BVDC_ChromaStatus    stChromaStatus;
	BVDC_ChromaSettings  stChromaConfig;
	uint32_t           i;
	uint32_t           ulTotalCnt = 0;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetChromaStatsConfiguration(hVidWin, &stChromaConfig);
	BKNI_Memset((void*)&stChromaStatus, 0x0, sizeof(BVDC_ChromaStatus));

	if(BERR_SUCCESS == BVDC_Window_GetChromaStatus(hVidWin, &stChromaStatus))
	{
		printf("\nCurrent Chroma status for window %d:\n", pContext->iSelectedWin);
		printf("	Type = %s\n",
			(stChromaConfig.eType == BVDC_ChromaHistType_eCrCb) ? "CR_CB" : "HUE_SAT");

		ulTotalCnt = (stChromaConfig.eType == BVDC_ChromaHistType_eCrCb) ? 32 : 24;
		for(i = 0; i < ulTotalCnt; i++)
		{
			printf("	%s Histogram[%2d]   = %d\n",
				(stChromaConfig.eType == BVDC_ChromaHistType_eCrCb) ? "Cr" : "Hue",
				i, stChromaStatus.stCrCbHist.aulCrHistogram[i]);

			printf("	%s Histogram[%2d]   = %d\n",
				(stChromaConfig.eType == BVDC_ChromaHistType_eCrCb) ? "Cb" : "Sat",
				i, stChromaStatus.stCrCbHist.aulCrHistogram[i]);
		}
	}

done:
	return;
}
/***************************************************************************
*
*/
void BTST_P_WinSplitScreen
( BTST_P_Context          *pContext )
{
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	BVDC_Window_SplitScreenSettings stSplitScreen;
	char *aMode[] = {"Disable", "Left", "Right"};
	int i;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetSplitScreenMode(hVidWin, &stSplitScreen);

	printf("Legend for split screen setting: Disable(%d), Left(%d), Right(%d)\n",
		BVDC_SplitScreenMode_eDisable, BVDC_SplitScreenMode_eLeft, BVDC_SplitScreenMode_eRight);
	printf("Hue & Sat [%s]: ", aMode[stSplitScreen.eHue]);
	BTST_P_Scanf(pContext, "%d", &i);
	stSplitScreen.eHue = i;

	printf("Brightness [%s]: ", aMode[stSplitScreen.eBrightness]);
	BTST_P_Scanf(pContext, "%d", &i);
	stSplitScreen.eBrightness = i;

	printf("Contrast [%s]: ", aMode[stSplitScreen.eContrast]);
	BTST_P_Scanf(pContext, "%d", &i);
	stSplitScreen.eContrast = i;

	printf("Color Temp [%s]: ", aMode[stSplitScreen.eColorTemp]);
	BTST_P_Scanf(pContext, "%d", &i);
	stSplitScreen.eColorTemp= i;

	printf("Sharpness [%s]: ", aMode[stSplitScreen.eSharpness]);
	BTST_P_Scanf(pContext, "%d", &i);
	stSplitScreen.eSharpness = i;

	printf("Auto Flesh [%s]: ", aMode[stSplitScreen.eAutoFlesh]);
	BTST_P_Scanf(pContext, "%d", &i);
	stSplitScreen.eAutoFlesh = i;

	printf("Blue Boost [%s]: ", aMode[stSplitScreen.eBlueBoost]);
	BTST_P_Scanf(pContext, "%d", &i);
	stSplitScreen.eBlueBoost = i;

	printf("Green Boost [%s]: ", aMode[stSplitScreen.eGreenBoost]);
	BTST_P_Scanf(pContext, "%d", &i);
	stSplitScreen.eGreenBoost = i;

	printf("CMS [%s]: ", aMode[stSplitScreen.eCms]);
	BTST_P_Scanf(pContext, "%d", &i);
	stSplitScreen.eCms = i;

	printf("CM3D [%s]: ", aMode[stSplitScreen.eCm3d]);
	BTST_P_Scanf(pContext, "%d", &i);
	stSplitScreen.eCm3d = i;

	printf("Contrast Stretch [%s]: ", aMode[stSplitScreen.eContrastStretch]);
	BTST_P_Scanf(pContext, "%d", &i);
	stSplitScreen.eContrastStretch = i;

	printf("Blue Stretch [%s]: ", aMode[stSplitScreen.eBlueStretch]);
	BTST_P_Scanf(pContext, "%d", &i);
	stSplitScreen.eBlueStretch = i;

	printf("ANR [%s]: ", aMode[stSplitScreen.eAnr]);
	BTST_P_Scanf(pContext, "%d", &i);
	stSplitScreen.eAnr = i;

	printf("SCL's eDeJagging [%s]: ", aMode[stSplitScreen.eDeJagging]);
	BTST_P_Scanf(pContext, "%d", &i);
	stSplitScreen.eDeJagging = i;

	printf("SCL's eDeRinging [%s]: ", aMode[stSplitScreen.eDeRinging]);
	BTST_P_Scanf(pContext, "%d", &i);
	stSplitScreen.eDeRinging = i;

	BVDC_Window_SetSplitScreenMode(hVidWin, &stSplitScreen);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_SrcSplitScreen
( BTST_P_Context          *pContext )
{
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	BVDC_Source_SplitScreenSettings stSplitScreen;
	char *aMode[] = {"Disable", "Left", "Right"};
	int i;

	if(!hVidSrc)
	{
		goto done;
	}

	BVDC_Source_GetSplitScreenMode(hVidSrc, &stSplitScreen);

	printf("Legend for split screen setting: Disable(%d), Left(%d), Right(%d)\n",
		BVDC_SplitScreenMode_eDisable, BVDC_SplitScreenMode_eLeft, BVDC_SplitScreenMode_eRight);

	printf("DNR [%s]: ", aMode[stSplitScreen.eDnr]);
	BTST_P_Scanf(pContext, "%d", &i);
	stSplitScreen.eDnr = i;

	BVDC_Source_SetSplitScreenMode(hVidSrc, &stSplitScreen);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}



/***************************************************************************
*
*/
void BTST_P_ShowHideWin
( BTST_P_Context          *pContext )
{
	bool bVisible;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetVisibility(hVidWin, &bVisible);
	BVDC_Window_SetVisibility(hVidWin, !bVisible);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_PhaseAdjust
( BTST_P_Context          *pContext )
{
	int i;
	BVDC_PhaseAdjustMode ePhaseAdjustMode;
	BVDC_Window_Handle hVidWin     =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetPhaseAdjustMode(hVidWin, &ePhaseAdjustMode);
	printf("Enter new mode disable(0), track(1), clamp(2), and trackclamp(4):");
	BTST_P_Scanf(pContext, "%d", &i);
	if     (0 == i){ePhaseAdjustMode = BVDC_PhaseAdjustMode_eDisable;}
	else if(1 == i){ePhaseAdjustMode = BVDC_PhaseAdjustMode_eTrack;}
	else if(2 == i){ePhaseAdjustMode = BVDC_PhaseAdjustMode_eClamp;}
	else if(3 == i){ePhaseAdjustMode = BVDC_PhaseAdjustMode_eTrackAndClamp;}
	else           {ePhaseAdjustMode = BVDC_PhaseAdjustMode_eDisable; }

	BVDC_Window_SetPhaseAdjustMode(hVidWin, ePhaseAdjustMode);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_SetSclConfig
( BTST_P_Context          *pContext )
{
	int i;
	BVDC_Scaler_Settings stSclcfg;
	BVDC_Window_Handle hVidWin     =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetScalerConfiguration(hVidWin, &stSclcfg);
	printf("Ignore SCL vertical phase adjustment(%d)? no(0), yes(1): ", stSclcfg.bSclVertPhaseIgnore);
	BTST_P_Scanf(pContext, "%d", &i);
	stSclcfg.bSclVertPhaseIgnore = (i) ? true : false;

	printf("SCL vertical bSclVertDejagging(%d)? no(0), yes(1): ", stSclcfg.bSclVertDejagging);
	BTST_P_Scanf(pContext, "%d", &i);
	stSclcfg.bSclVertDejagging = (i) ? true : false;

	printf("SCL vertical bSclVertDeRinging(%d)? no(0), yes(1): ", stSclcfg.bSclHorzLumaDeringing);
	BTST_P_Scanf(pContext, "%d", &i);

	stSclcfg.bSclHorzLumaDeringing   = (i) ? true : false;
	stSclcfg.bSclVertLumaDeringing   = (i) ? true : false;
	stSclcfg.bSclHorzChromaDeringing = (i) ? true : false;
	stSclcfg.bSclVertChromaDeringing = (i) ? true : false;

	BVDC_Window_SetScalerConfiguration(hVidWin, &stSclcfg);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_Contrast
( BTST_P_Context          *pContext )
{
	int16_t sCurContrast;
	int iContrast;
	BVDC_Window_Handle hVidWin     =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetContrast(hVidWin, &sCurContrast);
	printf("Current contrast level: %d,   Enter new contrast level:", sCurContrast);
	BTST_P_Scanf(pContext, "%d", &iContrast);

	BVDC_Window_SetContrast(hVidWin, iContrast & 0xffff);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_Saturation
( BTST_P_Context          *pContext )
{
	int16_t sCurSaturation;
	int iSaturation;
	BVDC_Window_Handle hVidWin     =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetSaturation(hVidWin, &sCurSaturation);
	printf("Current saturation level: %d,   Enter new saturation level:", sCurSaturation);
	BTST_P_Scanf(pContext, "%d", &iSaturation);

	BVDC_Window_SetSaturation(hVidWin, iSaturation & 0xffff);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_Hue
( BTST_P_Context          *pContext )
{
	int16_t sCurHue;
	int iHue;
	BVDC_Window_Handle hVidWin     =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetHue(hVidWin, &sCurHue);
	printf("Current hue level: %d,   Enter new hue level:", sCurHue);
	BTST_P_Scanf(pContext, "%d", &iHue);

	BVDC_Window_SetHue(hVidWin, iHue & 0xffff);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_Brightness
( BTST_P_Context          *pContext )
{
	int16_t sCurBrightness;
	int iBrightness;
	BVDC_Window_Handle hVidWin     =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetBrightness(hVidWin, &sCurBrightness);
	printf("Current brightness level: %d,   Enter new brightness level:", sCurBrightness);
	BTST_P_Scanf(pContext, "%d", &iBrightness);

	BVDC_Window_SetBrightness(hVidWin, iBrightness & 0xffff);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_Sharpness
( BTST_P_Context          *pContext )
{
	bool     bSharpnessEnable;
	uint32_t ulSharpnessEn;
	int16_t  sSharpness;
	int      sNewSharpness;
	uint32_t ulSharpnessConfig;

	BVDC_Window_Handle hVidWin     =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetSharpness(hVidWin, &bSharpnessEnable, &sSharpness);
	printf("Enable Sharpness (Current = %s): [Disable=0/Enable=1]: ",
		(bSharpnessEnable) ? "Enable" : "Disable");
	BTST_P_Scanf(pContext, "%u", &ulSharpnessEn);
	bSharpnessEnable = (ulSharpnessEn) ? true : false;

	printf("How to config sharpness? [internal=0/disable custom=1/enable custom=2]: ");
	BTST_P_Scanf(pContext, "%u", &ulSharpnessConfig);
	if(ulSharpnessConfig == 0)
	{
		printf("Enter sharpness level (Current = %d) [-32768, 32767]: ",
			sSharpness);
		BTST_P_Scanf(pContext, "%d", &sNewSharpness);
	}
	else if(ulSharpnessConfig == 1)
	{
		/* disable custom sharpness config */
		BVDC_Window_SetSharpnessConfig(hVidWin, NULL);
	}
	else
	{
		BVDC_SharpnessSettings stSharpnessSettings;
		uint32_t ulTemp;

		/* testing user shaprness config */
		BVDC_Window_GetSharpnessConfig(hVidWin, &stSharpnessSettings);

#if (BTST_SUPPORT_TNT2)            /* TNT2 HW base */
		printf("Is window SD or HD (0=SD/1=HD)?");
		BTST_P_Scanf(pContext, "%d", (int *)&ulTemp);

		stSharpnessSettings = (ulTemp == 1) ? g_astSharpness[0] : g_astSharpness[1];

#else
		printf("Luma Ctrl Core (Default = %d): ", stSharpnessSettings.ulLumaCtrlCore);
		BTST_P_Scanf(pContext, "%u", &stSharpnessSettings.ulLumaCtrlCore);
		printf("Luma Ctrl Gain (Default = %d): ", stSharpnessSettings.ulLumaCtrlGain);
		BTST_P_Scanf(pContext, "%u", &stSharpnessSettings.ulLumaCtrlGain);
		printf("Luma Ctrl Blur (Default = %d): ", stSharpnessSettings.ulLumaCtrlBlur);
		BTST_P_Scanf(pContext, "%u", &stSharpnessSettings.ulLumaCtrlBlur);
		printf("Luma Ctrl Soften (Default = %d): ", stSharpnessSettings.bLumaCtrlSoften);
		BTST_P_Scanf(pContext, "%u", &ulTemp);
		stSharpnessSettings.bLumaCtrlSoften = (bool)ulTemp;
		printf("Luma Ctrl H Only (Default = %d): ", stSharpnessSettings.bLumaCtrlHOnly);
		BTST_P_Scanf(pContext, "%u", &ulTemp);
		stSharpnessSettings.bLumaCtrlHOnly = (bool)ulTemp;
		printf("Luma Peaking HAvoid (Default = %d): ", stSharpnessSettings.ulLumaPeakingHAvoid);
		BTST_P_Scanf(pContext, "%u", &stSharpnessSettings.ulLumaPeakingHAvoid);
		printf("Luma Peaking VAvoid (Default = %d): ", stSharpnessSettings.ulLumaPeakingVAvoid);
		BTST_P_Scanf(pContext, "%u", &stSharpnessSettings.ulLumaPeakingVAvoid);
		printf("Luma Peaking Limit (Default = %d): ", stSharpnessSettings.ulLumaPeakingPeakLimit);
		BTST_P_Scanf(pContext, "%u", &stSharpnessSettings.ulLumaPeakingPeakLimit);
		printf("Luma Peaking Value (Default = %d): ", stSharpnessSettings.ulLumaPeakingPeakValue);
		BTST_P_Scanf(pContext, "%u", &stSharpnessSettings.ulLumaPeakingPeakValue);
		printf("Chroma Ctrl Core (Default = %d): ", stSharpnessSettings.ulChromaCtrlCore);
		BTST_P_Scanf(pContext, "%u", &stSharpnessSettings.ulChromaCtrlCore);
		printf("Chroma Ctrl Wide Chroma (Default = %d): ", stSharpnessSettings.bChromaCtrlWideChroma);
		BTST_P_Scanf(pContext, "%u", &ulTemp);
		stSharpnessSettings.bChromaCtrlWideChroma = (bool)ulTemp;
		printf("Chroma Ctrl False Color (Default = %d): ", stSharpnessSettings.ulChromaCtrlFalseColor);
		BTST_P_Scanf(pContext, "%u", &stSharpnessSettings.ulChromaCtrlFalseColor);
		printf("Chroma Ctrl Gain (Default = %d): ", stSharpnessSettings.ulChromaCtrlGain);
		BTST_P_Scanf(pContext, "%u", &stSharpnessSettings.ulChromaCtrlGain);
		printf("Chroma Ctrl H Only (Default = %d): ", stSharpnessSettings.bChromaCtrlHOnly);
		BTST_P_Scanf(pContext, "%u", &ulTemp);
		stSharpnessSettings.bChromaCtrlHOnly = (bool)ulTemp;
		printf("Wide Luma Ctrl Core (Default = %d): ", stSharpnessSettings.ulWideLumaCtrlCore);
		BTST_P_Scanf(pContext, "%u", &stSharpnessSettings.ulWideLumaCtrlCore);
		printf("Wide Luma Ctrl Mode (Default = %d): ", stSharpnessSettings.ulWideLumaCtrlMode);
		BTST_P_Scanf(pContext, "%u", &stSharpnessSettings.ulWideLumaCtrlMode);

#endif
		BVDC_Window_SetSharpnessConfig(hVidWin, &stSharpnessSettings);
	}

	BVDC_Window_SetSharpness(hVidWin, bSharpnessEnable, sNewSharpness & 0xffff);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
*
*/
void BTST_P_GetTune
( BTST_P_Context          *pContext )
{
	uint32_t  i, ulCount, aulField[18], aulValue[18];
	BVDC_Window_Handle hVidWin     =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	/* Get sharpness settings */
	ulCount = 18;
	for(i = 0; i < ulCount; i++)
	{
		aulField[i] = BVDC_TUNE_FIELD_ID(BVDC_TUNE_INTERFACE_WINDOW,
			BVDC_TUNE_STRUCTURE_SHARPNESS_SETTINGS, i);

	}

	BVDC_Window_GetTuneParam(hVidWin, aulField, aulValue, ulCount);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

	printf("==================== Current Tune Field Settings ===================\n");
	printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_LUMA_CTRL_CORE         : %d\n", aulValue[0]);
	printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_LUMA_CTRL_GAIN         : %d\n", aulValue[1]);
	printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_LUMA_CTRL_BLUR         : %d\n", aulValue[2]);
	printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_LUMA_CTRL_SOFTEN       : %d\n", aulValue[3]);
	printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_LUMA_CTRL_H_ONLY       : %d\n", aulValue[4]);
	printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_LUMA_PEAKING_H_AVOID   : %d\n", aulValue[5]);
	printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_LUMA_PEAKING_V_AVOID   : %d\n", aulValue[6]);
	printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_LUMA_PEAKING_PEAK_LIMIT: %d\n", aulValue[7]);
	printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_UMA_PEAKING_PEAK_VALUE : %d\n", aulValue[8]);
	printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_CHROMA_CTRL_CORE       : %d\n", aulValue[9]);
	printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_CHROMA_CTRL_WIDE_CHROMA: %d\n", aulValue[10]);
	printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_CHROMA_CTRL_FALSE_COLOR: %d\n", aulValue[11]);
	printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_CHROMA_CTRL_GAIN       : %d\n", aulValue[12]);
	printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_CHROMA_CTRL_H_ONLY     : %d\n", aulValue[13]);
	printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_WIDE_LUMA_CTRL_CORE    : %d\n", aulValue[14]);
	printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_WIDE_LUMA_CTRL_MODE    : %d\n", aulValue[15]);
	printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_SIMPLE_LUMA_CTRL_CORE  : %d\n", aulValue[16]);
	printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_SIMPLE_LUMA_CTRL_MODE  : %d\n", aulValue[17]);

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_SetTune
( BTST_P_Context          *pContext )
{
	uint32_t  i, ulCount = 18;
	uint32_t  aulField[18], aulValue[18], aulCurValue[18];
	uint32_t  ulRegField;

	BVDC_Window_Handle hVidWin     =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	/* Get current value */
	for(i = 0; i < ulCount; i++)
	{
		aulField[i] = BVDC_TUNE_FIELD_ID(BVDC_TUNE_INTERFACE_WINDOW,
			BVDC_TUNE_STRUCTURE_SHARPNESS_SETTINGS, i);
	}
	BVDC_Window_GetTuneParam(hVidWin, aulField, aulCurValue, ulCount);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

	printf("Enter number of fields to tune (Max: 18):  ");
	BTST_P_Scanf(pContext, "%u", &ulCount);

	for(i = 0; i < ulCount; i++)
	{
		printf("Select field[%2d]: \n", i);
		printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_LUMA_CTRL_CORE         ( 0)\n");
		printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_LUMA_CTRL_GAIN         ( 1)\n");
		printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_LUMA_CTRL_BLUR         ( 2)\n");
		printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_LUMA_CTRL_SOFTEN       ( 3)\n");
		printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_LUMA_CTRL_H_ONLY       ( 4)\n");
		printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_LUMA_PEAKING_H_AVOID   ( 5)\n");
		printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_LUMA_PEAKING_V_AVOID   ( 6)\n");
		printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_LUMA_PEAKING_PEAK_LIMIT( 7)\n");
		printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_UMA_PEAKING_PEAK_VALUE ( 8)\n");
		printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_CHROMA_CTRL_CORE       ( 9)\n");
		printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_CHROMA_CTRL_WIDE_CHROMA(10)\n");
		printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_CHROMA_CTRL_FALSE_COLOR(11)\n");
		printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_CHROMA_CTRL_GAIN       (12)\n");
		printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_CHROMA_CTRL_H_ONLY     (13)\n");
		printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_WIDE_LUMA_CTRL_CORE    (14)\n");
		printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_WIDE_LUMA_CTRL_MODE    (15)\n");
		printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_SIMPLE_LUMA_CTRL_CORE  (16)\n");
		printf("BVDC_WINDOW_TUNE_SHARPNESS_SETTINGS_SIMPLE_LUMA_CTRL_MODE  (17)\n");
		BTST_P_Scanf(pContext, "%u", &ulRegField);
		aulField[i] = BVDC_TUNE_FIELD_ID(BVDC_TUNE_INTERFACE_WINDOW,
			BVDC_TUNE_STRUCTURE_SHARPNESS_SETTINGS, ulRegField);

		printf("Enter data for this field [%2d] (current %d): ",
			i, aulCurValue[ulRegField]);
		BTST_P_Scanf(pContext, "%u", &aulValue[i]);
	}

	BVDC_Window_SetTuneParam(hVidWin, aulField, aulValue, ulCount);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_ResetTune
( BTST_P_Context          *pContext )
{
	BVDC_Window_Handle hVidWin     =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_ResetTuneParam(hVidWin);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_WhiteBalance
( BTST_P_Context          *pContext )
{
/* Color Correction for Non-Orthongonal VEC only */
#if (!BCHP_IT_0_REG_START)
	BERR_Code eStatus = BERR_SUCCESS;
	bool bWBEnable;
	uint32_t ulGammaTableId;
	uint32_t ulColorTempId;
	uint32_t ulWBEnable;
	BVDC_Display_Handle hDisplay  = pContext->ahDisplay[pContext->iSelectedComp];

	if(!hDisplay)
	{
		goto done;
	}

	eStatus = BVDC_Display_GetColorCorrectionTable(hDisplay, &bWBEnable, &ulGammaTableId, &ulColorTempId);
	printf("Current Color Correction setting: %s\n", (bWBEnable) ? "enable" : "disable");
	if(bWBEnable)
	{
		if(eStatus == BERR_SUCCESS)
		{
			printf("Current gamma table ID: %d\n", ulGammaTableId);
			printf("Current color temp ID : %d\n", ulColorTempId);
		}
		else
		{
			printf("Loading user color correction table\n");
		}
	}

	printf("Enable white balance? [no=0/yes=1]: ");
	BTST_P_Scanf(pContext, "%d", (int *)&ulWBEnable);
	bWBEnable = (ulWBEnable == 1) ? true : false;

	if(bWBEnable)
	{
		printf("Enter new gamma table index: ");
		BTST_P_Scanf(pContext, "%u", &ulGammaTableId);
		printf("Enter new color temp index: ");
		BTST_P_Scanf(pContext, "%u", &ulColorTempId);

		BVDC_Display_SetColorCorrectionTable(hDisplay, ulGammaTableId, ulColorTempId);
	}

	BVDC_Display_EnableColorCorrection(hDisplay, bWBEnable);
	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
#else
	BSTD_UNUSED(pContext);
#endif
}

/***************************************************************************
*
*/
void BTST_P_ColorTemp
( BTST_P_Context          *pContext )
{
	int16_t sColorTemp;
	int iNewColorTemp;
	BVDC_Window_Handle hVidWin     =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetColorTemp(hVidWin, &sColorTemp);
	printf("Current color temp: %d,   Enter new color temp:", sColorTemp);
	BTST_P_Scanf(pContext, "%d", &iNewColorTemp);

	BVDC_Window_SetColorTemp(hVidWin, iNewColorTemp);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_AttenuationRGB
( BTST_P_Context          *pContext )
{

	int32_t	                      lAttenuationR;
	int32_t	                      lAttenuationG;
	int32_t	                      lAttenuationB;
	int32_t	                      lOffsetR;
	int32_t	                      lOffsetG;
	int32_t	                      lOffsetB;

	int32_t	                      lNewAttenuationR;
	int32_t	                      lNewAttenuationG;
	int32_t	                      lNewAttenuationB;
	int32_t	                      lNewOffsetR;
	int32_t	                      lNewOffsetG;
	int32_t	                      lNewOffsetB;

	BVDC_Window_Handle hVidWin     =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetAttenuationRGB(hVidWin, &lAttenuationR, &lAttenuationG, &lAttenuationB,
		&lOffsetR, &lOffsetG, &lOffsetB);
	printf("Current Attenuation R: %d, G: %d, B: %d\n", lAttenuationR, lAttenuationG, lAttenuationB);
	printf("Current Offset      R: %d, G: %d, B: %d\n", lOffsetR, lOffsetG, lOffsetB);
	printf("Enter new Attenuation [R, G, B]: ");
	BTST_P_Scanf(pContext, "%d %d %d", &lNewAttenuationR, &lNewAttenuationG, &lNewAttenuationB);
	printf("Enter new Offset [R, G, B]: ");
	BTST_P_Scanf(pContext, "%d %d %d", &lNewOffsetR, &lNewOffsetG, &lNewOffsetB);

	BVDC_Window_SetAttenuationRGB(hVidWin, lNewAttenuationR, lNewAttenuationG, lNewAttenuationB,
		lNewOffsetR, lNewOffsetG, lNewOffsetB);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
*
*/
void BTST_P_DvoAttenuationRGB
( BTST_P_Context          *pContext )
{

	int32_t	                      lAttenuationR;
	int32_t	                      lAttenuationG;
	int32_t	                      lAttenuationB;
	int32_t	                      lOffsetR;
	int32_t	                      lOffsetG;
	int32_t	                      lOffsetB;

	int32_t	                      lNewAttenuationR;
	int32_t	                      lNewAttenuationG;
	int32_t	                      lNewAttenuationB;
	int32_t	                      lNewOffsetR;
	int32_t	                      lNewOffsetG;
	int32_t	                      lNewOffsetB;

	BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];

	if(!hDisplay)
	{
		goto done;
	}

	BVDC_Display_GetDvoAttenuationRGB(hDisplay, &lAttenuationR, &lAttenuationG, &lAttenuationB,
		&lOffsetR, &lOffsetG, &lOffsetB);
	printf("Current Attenuation R: %d, G: %d, B: %d\n", lAttenuationR, lAttenuationG, lAttenuationB);
	printf("Current Offset      R: %d, G: %d, B: %d\n", lOffsetR, lOffsetG, lOffsetB);
	printf("Enter new Attenuation [R, G, B]: ");
	BTST_P_Scanf(pContext, "%d %d %d", &lNewAttenuationR, &lNewAttenuationG, &lNewAttenuationB);
	printf("Enter new Offset [R, G, B]: ");
	BTST_P_Scanf(pContext, "%d %d %d", &lNewOffsetR, &lNewOffsetG, &lNewOffsetB);

	BVDC_Display_SetDvoAttenuationRGB(hDisplay, lNewAttenuationR, lNewAttenuationG, lNewAttenuationB,
		lNewOffsetR, lNewOffsetG, lNewOffsetB);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_Toggle3dComb
( BTST_P_Context          *pContext )
{
	bool b3DComb;
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;

	if(!hVidSrc)
	{
		goto done;
	}

	BVDC_Source_Get3DComb(hVidSrc, &b3DComb);

	b3DComb = !b3DComb;
	printf("%s 3d Comb\n", (b3DComb ? "Enable" : "Disable"));

	BVDC_Source_Set3DComb(hVidSrc, b3DComb);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
* iParam2 = pContext->iSelectedComp << 8 | pContext->iSelectedWin
*/
static void BTST_P_GenLipSyncDelayCb_isr
( void                            *pvParam1,
 int                              iParam2,
 void                            *pvVdcData )
{
	BTST_P_Context                 *pContext = (BTST_P_Context *)pvParam1;
	const BVDC_Window_CallbackData *pCbData = (const BVDC_Window_CallbackData*)pvVdcData;
	const BVDC_Window_CallbackMask *pDirty;
	uint32_t   ulCmpId, ulWinId;

	ulWinId = iParam2 & 0xf;
	ulCmpId = iParam2 >> 8;
	pDirty = &pCbData->stMask;

	/* VDC's window just changed something. */
	printf("Gen win[%d] on Cmp[%d] callback\n", ulWinId, ulCmpId);

	if( pDirty->bDriftDelay )
	{
		printf("New drift delay: %d (%d fields)\n", pCbData->ulDriftDelay,
			pCbData->ulDriftDelay / 16666);
	}

	if( pDirty->bVsyncDelay )
	{

		printf("New vsync delay: %d\n", pCbData->ulVsyncDelay);
		pContext->aulVsyncDelay[ulCmpId][ulWinId] = pCbData->ulVsyncDelay;
		pContext->bVsyncDelay = true;
	}

	return;
}


/***************************************************************************
*
*/
void BTST_P_ToggleLipSync
( BTST_P_Context          *pContext )
{
	int      iParam2;
	uint32_t ulChocie;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	bool *pbLipSync =
		&(pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].bLipSync);

	if(!hVidWin)
	{
		goto done;
	}

	if(*pbLipSync)
	{
		printf("Disabled lip sync.\n");
		BVDC_Window_InstallCallback(hVidWin,
			NULL, NULL, pContext->iSelectedWin);
	}
	else
	{
		BVDC_Window_CallbackSettings	stCbSettings;

		BVDC_Window_GetCallbackSettings(hVidWin, &stCbSettings);
		BTST_P_ApplyChanges(pContext);

		printf("Enable vsync change callback?\n");
		BTST_P_Scanf(pContext, "%u", &ulChocie);
		if(ulChocie)
		{
			stCbSettings.stMask.bVsyncDelay = true;
		}
		else
		{
			stCbSettings.stMask.bVsyncDelay = false;
		}
		printf("Enable drift delay callback?\n");
		BTST_P_Scanf(pContext, "%u", &ulChocie);
		if(ulChocie)
		{
			stCbSettings.stMask.bDriftDelay = true;
			printf("How much tolerance in usec? current[%u]\n", stCbSettings.ulLipSyncTolerance);
			BTST_P_Scanf(pContext, "%u", &ulChocie);
			stCbSettings.ulLipSyncTolerance = ulChocie;
		}
		else
		{
			stCbSettings.stMask.bDriftDelay = false;
		}
		printf("Enable game delay callback?\n");
		BTST_P_Scanf(pContext, "%u", &ulChocie);
		if(ulChocie)
		{
			stCbSettings.stMask.bGameModeDelay = true;
			printf("Target in usec? current[%d]\n", stCbSettings.ulGameModeReadWritePhaseDiff);
			BTST_P_Scanf(pContext, "%u", &ulChocie);
			stCbSettings.ulGameModeReadWritePhaseDiff = ulChocie;
			printf("How much tolerance in usec? current[%d]\n", stCbSettings.ulGameModeTolerance);
			BTST_P_Scanf(pContext, "%u", &ulChocie);
			stCbSettings.ulGameModeTolerance = ulChocie;
		}
		else
		{
			stCbSettings.stMask.bGameModeDelay = false;
		}
		BVDC_Window_SetCallbackSettings(hVidWin, &stCbSettings);
		iParam2 = pContext->iSelectedComp << 8 | pContext->iSelectedWin;
		BVDC_Window_InstallCallback(hVidWin,
			BTST_P_GenLipSyncDelayCb_isr, pContext, iParam2);

	}

	*pbLipSync = !(*pbLipSync);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;

}

static void BTST_P_ReturnBuf_isr
( BVDC_Window_Handle               hWindow,
  BVDC_Test_Window_CapturedImage  *pCapturedImage )
{
	BERR_Code eRes;

	eRes = BVDC_Test_Window_ReturnBuffer_isr(hWindow, pCapturedImage);
	if (eRes != BERR_SUCCESS)
	{
		printf("  Failed to return buf 0x%x\n", (uint32_t) pCapturedImage->pvBufAddr);
	}
	else
	{
		printf("  Return buf 0x%x\n", (uint32_t) pCapturedImage->pvBufAddr);
	}	
}

#define CAP_NUM_BUF    22
#define NUM_BUF_HOLD   20
/***************************************************************************
* iParam2 = pContext->iSelectedComp << 8 | pContext->iSelectedWin
*/
static void BTST_P_GetBufIsrCb_isr
( void                            *pvParam1,
 int                              iParam2,
 void                            *pvVdcData )
{
	BTST_P_Context *pContext = (BTST_P_Context *)pvParam1;
	const BVDC_Display_CallbackData *pCbData = (const BVDC_Display_CallbackData*)pvVdcData;
	const BVDC_Display_CallbackMask *pDirty;
	BVDC_Window_Handle hVidWin;
	BERR_Code eRes;
	uint32_t   ulCmpId, ulWinId;
	BVDC_Test_Window_CapturedImage *pCurCapBuf;
	int ii;
	static BVDC_Test_Window_CapturedImage stCapBuf[CAP_NUM_BUF];
	static uint32_t  ulLeftBufIdx = 0, ulRightBufIdx = 0; 	/* keep bufs between them */
	
	ulWinId = iParam2 & 0xf;
	ulCmpId = iParam2 >> 8;
	pDirty = &pCbData->stMask;
	hVidWin = pContext->aWinContext[ulCmpId][ulWinId].hWindow;

	if( pDirty->bPerVsync )
	{
		printf("Display vsync callback polarity %d\n", pCbData->ePolarity);

		/* return used bufs */
		if (ulRightBufIdx >= ulLeftBufIdx + NUM_BUF_HOLD)
		{				
			/* return middle bufs */
			for (ii=ulLeftBufIdx+1; ii<(int)(ulRightBufIdx-1); ii++)
			{
				BTST_P_ReturnBuf_isr(hVidWin, &stCapBuf[ii % CAP_NUM_BUF]);
			}
			/* return right and left buf */
			BTST_P_ReturnBuf_isr(hVidWin, &stCapBuf[(ulRightBufIdx-1) % CAP_NUM_BUF]);
			BTST_P_ReturnBuf_isr(hVidWin, &stCapBuf[ulLeftBufIdx % CAP_NUM_BUF]);

			ulLeftBufIdx = ulRightBufIdx;
		}
		
		/* get current buff */
		pCurCapBuf = &stCapBuf[ulRightBufIdx % CAP_NUM_BUF];
		eRes = BVDC_Test_Window_GetBuffer_isr(hVidWin, pCurCapBuf);
		if (eRes != BERR_SUCCESS)
		{
			printf("  Failed to get new buf\n");
		}
		else
		{
			ulRightBufIdx++;

			printf("  Get new buf 0x%x, polarity %d\n",
				   (uint32_t) pCurCapBuf->pvBufAddr, pCurCapBuf->eCapturePolarity);
		}
	}
}


/***************************************************************************
*
*/
void BTST_P_ToggleGetBufIsr
( BTST_P_Context          *pContext )
{
	int      iParam2;
	BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	bool *pbGetBufIsr =
		&(pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].bGetBufIsr);

	if(!hDisplay)
	{
		goto done;
	}

	if(*pbGetBufIsr)
	{
		printf("Disabled GetBuf_isr.\n");
		BVDC_Display_InstallCallback(hDisplay, NULL, NULL, 0);
	}
	else
	{
		BVDC_Display_CallbackSettings	stCbSettings;

		BVDC_Window_SetUserCaptureBufferCount(hVidWin, CAP_NUM_BUF);
		BVDC_Display_GetCallbackSettings(hDisplay, &stCbSettings);
		stCbSettings.stMask.bPerVsync = 1;
		BVDC_Display_SetCallbackSettings(hDisplay, &stCbSettings);
		iParam2 = pContext->iSelectedComp << 8 | pContext->iSelectedWin;
		BVDC_Display_InstallCallback(hDisplay,
			BTST_P_GetBufIsrCb_isr, pContext, iParam2);
		printf("Enabled GetBuf_isr.\n");
	}

	*pbGetBufIsr = !(*pbGetBufIsr);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;

}

/***************************************************************************
*
*/
void BTST_P_SetMosaicMode
( BTST_P_Context          *pContext )
{
	int iEnable, iVideoInMosaics, iClearRectByColor;
	int iChoice, iMode;
	uint32_t i, ulCnt, ulWidth, ulHeight;
	uint32_t ulLeft, ulTop, ulW, ulH;
	uint32_t ulDh, ulDw, ulCol, ulRow;
	uint32_t ulR;
	double dTheta;
	BVDC_MosaicConfiguration stSettings;
	bool abMosaicMode[BAVC_MOSAIC_MAX];
	uint8_t aucZOrder[BAVC_MOSAIC_MAX];
	BVDC_Rect *astRect = pContext->astRect[pContext->iSelectedComp][pContext->iSelectedWin];
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	if(!hVidWin)
	{
		goto done;
	}

	BKNI_Memset((void*)abMosaicMode, 1, sizeof(bool)*BAVC_MOSAIC_MAX);
	BKNI_Memset((void*)aucZOrder, 0, sizeof(uint8_t)*BAVC_MOSAIC_MAX);
	BVDC_Window_GetScalerOutput(hVidWin, &ulLeft, &ulTop, &ulW, &ulH);
	printf("window width = %d, height = %d\n", ulW, ulH);
	printf("Please enter new Mosaic settings: \n");
	printf("Enable Mosaics? [1=y/0=n]: \n");
	BTST_P_Scanf(pContext, "%d", &iEnable);
	pContext->bEnableMosaicMode = iEnable;

	if(iEnable)
	{
		printf("Video in Mosaics? [1=y/0=n]: \n");
		BTST_P_Scanf(pContext, "%d", &iVideoInMosaics);
		stSettings.bVideoInMosaics = iVideoInMosaics;
		printf("ClearRect Alpha in 8-bit? [0~255]: \n");
		BTST_P_Scanf(pContext, "%u", &stSettings.ulClearRectAlpha);
		printf("ClearRect by Mask Color? [1=y/0=n]: \n");
		BTST_P_Scanf(pContext, "%d", &iClearRectByColor);
		stSettings.bClearRectByMaskColor = iClearRectByColor;
		if(stSettings.bClearRectByMaskColor)
		{
			printf("ClearRect Mask Color in 8-bit R, G, B? [R, G, B]: \n");
			BTST_P_Scanf(pContext, "%u %u %u", &stSettings.ulMaskColorRed,
				&stSettings.ulMaskColorGreen, &stSettings.ulMaskColorBlue);
		}
		printf("Number of mosaic rectangles   = ");
		BTST_P_Scanf(pContext, "%u", &ulCnt);
		pContext->aulMosaicCount[pContext->iSelectedComp][pContext->iSelectedWin] = ulCnt;
		if(ulCnt > BAVC_MOSAIC_MAX) ulCnt = BAVC_MOSAIC_MAX;
		printf("\n(w, h) = ");
		BTST_P_Scanf(pContext, "%u %u", &ulWidth, &ulHeight);
		printf("\nWhat placement do you want for mosaics? (0) circle; (1) matrix; \n");
		BTST_P_Scanf(pContext, "%d", &iChoice);

		printf("Which rectangles do you want visible?\n");
		printf("(0) All; (1) 1st Half; (2) 2nd Half; (3) Alternate; (4) None: ");
		BTST_P_Scanf(pContext, "%d", &iMode);
		switch(iMode)
		{
		case 1:
			for(i = ulCnt/2; i < ulCnt; i++)
			{
				abMosaicMode[i] = false;
			}
			break;
		case 2:
			for(i = 0; i < ulCnt/2; i++)
			{
				abMosaicMode[i] = false;
			}
			break;
		case 3:
			for(i = 1; i < ulCnt; i+=2)
			{
				abMosaicMode[i] = false;
			}
			break;
		case 4:
			for(i = 0; i < ulCnt; i++)
			{
				abMosaicMode[i] = false;
			}
			break;
		default: ;
		}

		switch(iChoice)
		{
		case 1: /* matrix */
			printf("\nHow many colomns?\n");
			BTST_P_Scanf(pContext, "%u", &ulCol);
			if(ulCol > ulCnt) ulCol = ulCnt;
			if(ulCol * ulWidth <= ulW)
			{
				ulDw = (ulW / ulCol - ulWidth);
				astRect[0].lLeft = ulDw / 2;
				astRect[0].ulWidth = ulWidth;

				for(i = 1; i < ulCnt; i++)
				{
					if(i % ulCol)
					{
						astRect[i].lLeft = astRect[i-1].lLeft + ulWidth + ulDw;
					}
					else
					{
						astRect[i].lLeft = astRect[0].lLeft;
					}
					astRect[i].ulWidth = ulWidth;
				}
			}
			else
			{
				printf("\nMosaic Width x Mosaic Counts > Window Width\n");
				return;
			}

			ulRow = ulCnt/ulCol + ((ulCnt % ulCol) != 0);
			if(ulRow * ulHeight <= ulH)
			{
				ulDh = (ulH / ulRow - ulHeight);
				astRect[0].lTop = ulDh / 2;
				astRect[0].ulHeight = ulHeight;

				for(i = 1; i < ulCnt; i++)
				{
					if(i % ulCol)
					{
						astRect[i].lTop = astRect[i-1].lTop;
					}
					else
					{
						astRect[i].lTop = astRect[i - 1].lTop + ulHeight + ulDh;
					}
					astRect[i].ulHeight = ulHeight;
				}
			}
			else
			{
				printf("\nMosaic Height x Mosaic Counts > Window Height\n");
				return;
			}
			break;
		case 0:	 /* circle */
#ifndef M_PI
#define M_PI (3.1415926)
#endif
#ifndef sin
#define sin(x) ((x) - ((x)*(x)*(x)/6) + ((x)*(x)*(x)*(x)*(x)/120) - ((x)*(x)*(x)*(x)*(x)*(x)*(x)/5040))
#define cos(x) (1 - (x)*(x)/2 + ((x)*(x)*(x)*(x)/24) - ((x)*(x)*(x)*(x)*(x)*(x)/720))
#endif
			dTheta = 2 * M_PI / ulCnt;

			printf("\nPlease enter the circle radius:\n");
			BTST_P_Scanf(pContext, "%u", &ulR);
			if((ulR * 2 + ulHeight > ulH) || (ulR * 2 + ulWidth > ulW))
			{
				printf("\nMosaic Height + Diameter > Window Size\n");
				return;
			}
			for(i = 0; i < ulCnt; i++)
			{
				double dA = dTheta * i;
				double dHproj = 0, dVproj = 0;

				if(dA <= M_PI/2)
				{
					dHproj = cos(dA);
					dVproj = sin(dA);
				}
				else if(dA > M_PI/2 && dA <= M_PI)
				{
					dA = M_PI - dA;
					dHproj = -cos(dA);
					dVproj = sin(dA);
				}
				else if(dA > M_PI && dA <= M_PI*3/2)
				{
					dA = dA - M_PI;
					dHproj = -cos(dA);
					dVproj = -sin(dA);
				}
				else if(dA > M_PI*3/2 && dA <= M_PI*2)
				{
					dA = 2 * M_PI - dA;
					dHproj = cos(dA);
					dVproj = -sin(dA);
				}
				astRect[i].lTop  = (uint32_t)(ulR * dVproj) + (ulH - ulHeight) / 2;
				astRect[i].lLeft = (uint32_t)(ulR * dHproj) + (ulW - ulWidth) / 2;
				astRect[i].ulWidth = ulWidth;
				astRect[i].ulHeight = ulHeight;
			}
			break;
		default:
			for(i = 0; i < ulCnt; i++)
			{
				printf("\nMosaic %d: \n", i);
				printf("(x, y, w, h) = ");
				BTST_P_Scanf(pContext, "%d %d %u %u", &astRect[i].lLeft, &astRect[i].lTop,
					&astRect[i].ulWidth, &astRect[i].ulHeight);
			}
		}
		printf("\nDo you want to modify all the mosaics size? [1=y/0=n]\n");
		BTST_P_Scanf(pContext, "%d", &iChoice);
		if(iChoice)
		{
			for(i = 0; i < ulCnt; i++)
			{
				astRect[i].ulWidth = ulWidth + 4*i;
				astRect[i].ulHeight = ulHeight + 4*i;
			}
		}
		printf("\nDo you want to modify all the mosaics z-order? [1=y/0=n]: ");
		BTST_P_Scanf(pContext, "%d", &iChoice);
		if(iChoice)
		{
			printf("\nEnter z-order");
			for(i = 0; i < ulCnt; i++)
			{
				printf("\nMosaic %d: (z-order) = ", i);
				BTST_P_Scanf(pContext, "%d", &aucZOrder[i]);
			}
		}

		BVDC_Window_SetMosaicConfiguration(hVidWin, true, &stSettings);
		BVDC_Window_SetMosaicRectsVisibility(hVidWin, ulCnt, abMosaicMode);
		BVDC_Window_SetMosaicDstRects(hVidWin, ulCnt, astRect);
		BVDC_Window_SetMosaicRectsZOrder(hVidWin, ulCnt, aucZOrder);

	}
	else
	{
		BVDC_Window_SetMosaicConfiguration(hVidWin, false, NULL);
	}

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
*
*/
void BTST_P_SetMosaicZorder
( BTST_P_Context          *pContext )
{
	bool  bEnable;
	uint32_t i, ulCount, ulWidth, ulHeight;
	uint32_t ulLeft, ulTop, ulW, ulH;
	uint8_t aucZOrder[BAVC_MOSAIC_MAX];
	uint32_t ulMosaicCnt = pContext->aulMosaicCount[pContext->iSelectedComp][pContext->iSelectedWin];
	BVDC_Rect *astRect = pContext->astRect[pContext->iSelectedComp][pContext->iSelectedWin];
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetMosaicConfiguration(hVidWin, &bEnable, NULL);
	if(!bEnable)
		goto done;

	BKNI_Memset((void*)aucZOrder, 0, sizeof(uint8_t)*BAVC_MOSAIC_MAX);

	BVDC_Window_GetScalerOutput(hVidWin, &ulLeft, &ulTop, &ulW, &ulH);
	printf("window width = %d, height = %d\n", ulW, ulH);
	ulWidth = ulW / 2;
	ulHeight = ulH / 2;

	printf("Enter number of mosaic zorder test: ");
	BTST_P_Scanf(pContext, "%d", &ulCount);

	while(ulCount-- > 0)
	{
		for(i = 0; i < ulMosaicCnt; i++)
		{
			if((ulCount % 10) == 0)
			{
				if( i % 2 )
				{
					astRect[i].lLeft = ulWidth - rand() % 100;
				}
				else
				{
					astRect[i].lLeft = rand() % 100;

				}
				if( i / 2 )
				{
					astRect[i].lTop = ulHeight - rand() % 100;
				}
				else
				{
					astRect[i].lTop = rand() % 100;

				}

				astRect[i].ulWidth = ulWidth;
				astRect[i].ulHeight = ulHeight;
			}
			aucZOrder[i] = rand() % ulMosaicCnt;
		}

		BVDC_Window_SetMosaicDstRects(hVidWin, ulMosaicCnt, astRect);
		BVDC_Window_SetMosaicRectsZOrder(hVidWin, ulMosaicCnt, aucZOrder);
		BTST_P_ApplyChanges(pContext);

		BKNI_Sleep(2000);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_MosaicModeResize
( BTST_P_Context          *pContext )
{
	bool        bMosaicEnable, bScaleDown = true;
	uint32_t    i, ulMosaicCnt;
	uint32_t    ulLeft, ulTop, ulDispWidth, ulDispHeight;
	uint32_t    ulMinWidth, ulMinHeight, ulMaxWidth, ulMaxHeight;
	uint32_t    ulWidth, ulHeight, ulDeltaWidth, ulDeltaHeight;
	BVDC_MosaicConfiguration stMosaicSettings;

	BVDC_Rect *astRect;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	if(!hVidWin)
	{
		goto done;
	}

	/* Get Mosaic settings */
	BVDC_Window_GetMosaicConfiguration(hVidWin, &bMosaicEnable, &stMosaicSettings);
	if(!bMosaicEnable)
		goto done;

	BVDC_Window_GetScalerOutput(hVidWin, &ulLeft, &ulTop, &ulDispWidth, &ulDispHeight);
	printf("window width = %d, height = %d\n", ulDispWidth, ulDispHeight);

	ulMosaicCnt = pContext->aulMosaicCount[pContext->iSelectedComp][pContext->iSelectedWin];
	astRect = pContext->astRect[pContext->iSelectedComp][pContext->iSelectedWin];
	printf("Mosaic count: %d\n", ulMosaicCnt);

	printf("Enter minimum mosaic tile size: (w h): ");
	BTST_P_Scanf(pContext, "%d %d", &ulMinWidth, &ulMinHeight);
	printf("Enter delta size: (w h): ");
	BTST_P_Scanf(pContext, "%d %d", &ulDeltaWidth, &ulDeltaHeight);

	ulMaxWidth = ulDispWidth / 2;
	ulMaxHeight = ulDispHeight / 2;
	ulWidth = ulMaxWidth - ulDeltaWidth;
	ulHeight = ulMaxHeight - ulDeltaHeight;

	while((ulHeight != ulMaxHeight) || (ulWidth  != ulMaxWidth))
	{
		printf("Current (w, h) = %u %u\n", ulWidth, ulHeight);

		for(i = 0; i < ulMosaicCnt; i++)
		{
			astRect[i].lLeft = (ulMaxWidth - ulWidth) / 2 + ulMaxWidth * (i%2);
			astRect[i].lTop = (ulMaxHeight - ulHeight) / 2 + ulMaxHeight * (i/2);

			astRect[i].ulWidth  = ulWidth;
			astRect[i].ulHeight = ulHeight;
		}

		BVDC_Window_SetMosaicDstRects(hVidWin, ulMosaicCnt, astRect);
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

/***************************************************************************
*
*/
void BTST_P_SetMosaicTracking
( BTST_P_Context          *pContext )
{
	uint32_t ulTrackChannel;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	if(!hVidWin)
	{
		goto done;
	}
	BVDC_Window_GetMosaicTrack(hVidWin, &ulTrackChannel);
	printf("The current mosaic tracking channel of this window: %u\n", ulTrackChannel);
	printf("Please enter the new tracking channel: ");
	BTST_P_Scanf(pContext, "%u", &ulTrackChannel);
	BVDC_Window_SetMosaicTrack(hVidWin, ulTrackChannel);
	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_MosaicMove
( BTST_P_Context          *pContext )
{
	uint32_t ulLeft, ulTop, ulW, ulH, i;
	uint32_t ulCnt = pContext->aulMosaicCount[pContext->iSelectedComp][pContext->iSelectedWin];
	BVDC_Rect *astRect = pContext->astRect[pContext->iSelectedComp][pContext->iSelectedWin];
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		return;
	}

	BVDC_Window_GetScalerOutput(hVidWin, &ulLeft, &ulTop, &ulW, &ulH);
	/* Bounce the window around */
	if(ulCnt > 1)
	{
		uint32_t ulCount = 500;
		int32_t lHorzVel = BTST_P_RANDOM_VELOCITY;
		int32_t lVertVel = BTST_P_RANDOM_VELOCITY;

		while(ulCount-- > 0)
		{
			for(i = 0; i < ulCnt; i++)
			{
				/* Add the current velocity vectors to the position. */
				astRect[i].lLeft += lHorzVel;
				astRect[i].lTop  += lVertVel;

				/* Have we gone off the left edge? */
				if(astRect[i].lLeft <= 0)
				{
					astRect[i].lLeft = 0;
					lHorzVel = BTST_P_RANDOM_VELOCITY;
				}
				/* Have we gone off the right edge? */
				if((astRect[i].lLeft+astRect[i].ulWidth) >= ulW)
				{
					astRect[i].lLeft = ulW - astRect[i].ulWidth;
					lHorzVel = -BTST_P_RANDOM_VELOCITY;
				}
				/* Have we gone off the top edge? */
				if(astRect[i].lTop < 0)
				{
					astRect[i].lTop = 0;
					lVertVel = BTST_P_RANDOM_VELOCITY;
				}
				/* Have we gone off the bottom edge? */
				if((astRect[i].lTop+astRect[i].ulHeight) > ulH)
				{
					astRect[i].lTop = ulH - astRect[i].ulHeight;
					lVertVel = -BTST_P_RANDOM_VELOCITY;
				}
			}
			BVDC_Window_SetMosaicDstRects(hVidWin, ulCnt, astRect);

			if(pContext->bApplyImmediately)
			{
				BTST_P_ApplyChanges(pContext);
			}
		}
	}
}

/***************************************************************************
*
*/
void BTST_P_DelayOffset
( BTST_P_Context          *pContext )
{
	unsigned int ulDelay;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetDelayOffset(hVidWin, &ulDelay);
	printf("Current buffer delay offset = %d\n", ulDelay);
	printf("Please enter new offset: \n");
	BTST_P_Scanf(pContext, "%u", &ulDelay);
	BVDC_Window_SetDelayOffset(hVidWin, ulDelay);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_DnrFilter
( BTST_P_Context          *pContext )
{
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	BVDC_Dnr_Settings stDnrSettings;
	int  iMode = 0;

	if(!hVidSrc)
	{
		goto done;
	}

#if BTST_DNR_PR17879_NOT_FIXED
	printf("Please enter uint32_t adj_Qp: \n");
	BTST_P_Scanf(pContext, "%u", &g_ulAdjQp);
#endif

	BVDC_Source_GetDnrConfiguration(hVidSrc, &stDnrSettings);
	printf("Enter user Qp value? [current = %u]: ", stDnrSettings.ulQp);
	BTST_P_Scanf(pContext, "%u", &stDnrSettings.ulQp);

	printf("Please select Block Noise Reduction mode [0=Disable/1=Bypass/2=Enable] and level: ");
	BTST_P_Scanf(pContext, "%d %d", &iMode, &stDnrSettings.iBnrLevel);
	iMode = (iMode > BVDC_FilterMode_eEnable)?  BVDC_FilterMode_eEnable  : iMode;
	iMode = (iMode < BVDC_FilterMode_eDisable)? BVDC_FilterMode_eDisable : iMode;
	stDnrSettings.eBnrMode = iMode;
	printf("Please select Mosquito Noise Reduction mode [0=Disable/1=Bypass/2=Enable] and level: ");
	BTST_P_Scanf(pContext, "%d %d", &iMode, &stDnrSettings.iMnrLevel);
	iMode = (iMode > BVDC_FilterMode_eEnable)?  BVDC_FilterMode_eEnable  : iMode;
	iMode = (iMode < BVDC_FilterMode_eDisable)? BVDC_FilterMode_eDisable : iMode;
	stDnrSettings.eMnrMode = iMode;
	printf("Please select Digital Contour Reduction mode [0=Disable/1=Bypass/2=Enable] and level: ");
	BTST_P_Scanf(pContext, "%d %d", &iMode, &stDnrSettings.iDcrLevel);
	iMode = (iMode > BVDC_FilterMode_eEnable)?  BVDC_FilterMode_eEnable  : iMode;
	iMode = (iMode < BVDC_FilterMode_eDisable)? BVDC_FilterMode_eDisable : iMode;
	stDnrSettings.eDcrMode = iMode;

	BVDC_Source_SetDnrConfiguration(hVidSrc, &stDnrSettings);
	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
*
*/
void BTST_P_ChgMad
( BTST_P_Context          *pContext )
{
	bool bMad;
	uint32_t ulMad, ulMad32, ulMad22, ulMadGameMode, ulPixelFmt;
	uint32_t ulCompression;
	BVDC_Deinterlace_Settings stMadSetting;

	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	stMadSetting.pChromaSettings = NULL;
	stMadSetting.pMotionSettings = NULL;
	stMadSetting.pReverse22Settings = NULL;
	stMadSetting.pReverse32Settings = NULL;

	printf("Please enter new setting for MAD: Enable(1), Disable(0)\n");
	BVDC_Window_GetDeinterlaceConfiguration(hVidWin, &bMad, &stMadSetting);
	printf("Win %d deinterlacer or MAD [%s]: ",
		pContext->iSelectedWin, (bMad) ? "Enable" : "Disable");
	BTST_P_Scanf(pContext, "%u", &ulMad);
	bMad = (ulMad == 1) ? true : false;
	if(bMad)
	{
		printf("MAD 3:2 pulldown [%s]: ", (stMadSetting.bReverse32Pulldown) ? "Enable" : "Disable");
		BTST_P_Scanf(pContext, "%u", &ulMad32);
		printf("MAD 2:2 pulldown [%s]: ", (stMadSetting.bReverse22Pulldown) ? "Enable" : "Disable");
		BTST_P_Scanf(pContext, "%u", &ulMad22);
		printf("\n");
		printf("MAD Game Mode: 0) off;\n");
		printf("               1) 5-field 2-delay;\n");
		printf("               2) 5-field 1-delay;\n");
		printf("               3) 5-field 0-delay;\n");
		printf("               4) 5-field ForceSpatial;\n");
		printf("               5) 4-field 2-Delay;\n");
		printf("               6) 4-field 1-Delay;\n");
		printf("               7) 4-field 0-Delay;\n");
		printf("               8) 4-field ForceSpatial;\n");
		printf("               9) 3-field 2-Delay;\n");
		printf("              10) 3-field 1-Delay;\n");
		printf("              11) 3-field 0-delay;\n");
		printf("              12) 3-field ForceSpatial;\n");
		BTST_P_Scanf(pContext, "%u", &ulMadGameMode);
		switch(ulMadGameMode)
		{
		case 0:
			stMadSetting.eGameMode = BVDC_MadGameMode_eOff; break;
		case 1:
			stMadSetting.eGameMode = BVDC_MadGameMode_e5Fields_2Delay; break;
		case 2:
			stMadSetting.eGameMode = BVDC_MadGameMode_e5Fields_1Delay; break;
		case 3:
			stMadSetting.eGameMode = BVDC_MadGameMode_e5Fields_0Delay; break;
		case 4:
			stMadSetting.eGameMode = BVDC_MadGameMode_e5Fields_ForceSpatial; break;
		case 5:
			stMadSetting.eGameMode = BVDC_MadGameMode_e4Fields_2Delay; break;
		case 6:
			stMadSetting.eGameMode = BVDC_MadGameMode_e4Fields_1Delay; break;
		case 7:
			stMadSetting.eGameMode = BVDC_MadGameMode_e4Fields_0Delay; break;
		case 8:
			stMadSetting.eGameMode = BVDC_MadGameMode_e4Fields_ForceSpatial; break;
		case 9:
			stMadSetting.eGameMode = BVDC_MadGameMode_e3Fields_2Delay; break;
		case 10:
			stMadSetting.eGameMode = BVDC_MadGameMode_e3Fields_1Delay; break;
		case 11:
			stMadSetting.eGameMode = BVDC_MadGameMode_e3Fields_0Delay; break;
		case 12:
			stMadSetting.eGameMode = BVDC_MadGameMode_e3Fields_ForceSpatial; break;
		default:
			stMadSetting.eGameMode = BVDC_MadGameMode_eOff; break;
		}

		stMadSetting.bShrinkWidth = pContext->bShrinkWidthForMad;
		stMadSetting.bReverse22Pulldown = (ulMad22 == 1) ? true : false;
		stMadSetting.bReverse32Pulldown = (ulMad32 == 1) ? true : false;

		printf("What's the pixel format of the deinterlacer storage? 0)8-bit 422; 1) or else 10-bit 422;\n");
		BTST_P_Scanf(pContext, "%u", &ulPixelFmt);

		stMadSetting.ePxlFormat = ulPixelFmt ?
		BPXL_eX2_Y010_Cb10_Y110_X2_Cr10_Y010_Cb10_X2_Y110_Cr10_Y010_X2_Cb10_Y110_Cr10 :
		BPXL_eY18_Cb8_Y08_Cr8;

		printf("Compression [%s]: ", (stMadSetting.stVideoTestFeature1.bEnable) ? "Enable" : "Disable");
		BTST_P_Scanf(pContext, "%u", &ulCompression);
		if(ulCompression)
		{
			printf("ulBitsPerPixel (U31.1) [%d]: ", stMadSetting.stVideoTestFeature1.ulBitsPerPixel);
			BTST_P_Scanf(pContext, "%u", &stMadSetting.stVideoTestFeature1.ulBitsPerPixel);
		}
		stMadSetting.stVideoTestFeature1.bEnable = ulCompression;

#if (0!=BVDC_P_SUPPORT_MAD)
		if (stMadSetting.eGameMode == BVDC_MadGameMode_e4Fields_2Delay)
		{
			printf("!!! Error chipset does not support 4 fields 2 Delay\n");
			return;
		}
#endif
		if ((stMadSetting.eGameMode == BVDC_MadGameMode_e5Fields_2Delay)
			||(stMadSetting.eGameMode == BVDC_MadGameMode_e3Fields_2Delay))
		{
			printf("!!! Error chipset does not support this game mode\n");
			return;
		}

	}

	if(BERR_SUCCESS == BVDC_Window_SetDeinterlaceConfiguration(hVidWin, bMad, &stMadSetting))
	{
		if(pContext->bApplyImmediately)
		{
			BTST_P_ApplyChanges(pContext);
		}
	}
done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_ToggleMadOsd
( BTST_P_Context          *pContext )
{
	bool bOsd;
	uint32_t ulHpos, ulVpos;

	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Test_Window_GetMadOsd(hVidWin, &bOsd, &ulHpos, &ulVpos);
	if(bOsd)
	{
		BVDC_Test_Window_SetMadOsd(hVidWin, false, 0, 0);
		printf("Disable OSD with MAD.\n");
	}
	else
	{
		printf("Please enter the position of OSD (H, V):\n");
		BTST_P_Scanf(pContext, "%u %u", &ulHpos, &ulVpos);
		BVDC_Test_Window_SetMadOsd(hVidWin, true, ulHpos, ulVpos);
		printf("Enable OSD with MAD at position(%u, %u).\n", ulHpos, ulVpos);
	}

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}
done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_StressMad
( BTST_P_Context          *pContext )
{
	bool bMad;
	int i, count;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	printf("How many times: ");
	BTST_P_Scanf(pContext, "%d", &i);
	if(i < 0) i = 0;
	count = i;
	while(i--) {
		BVDC_Window_GetDeinterlaceConfiguration(hVidWin, &bMad, NULL);
		if(BERR_SUCCESS == BVDC_Window_SetDeinterlaceConfiguration(hVidWin, !bMad, NULL))
		{
			printf("[%d/%d]: %s deinterlacer or MAD.\n", i, count, (bMad) ? "Disabled" : "Enabled");

			if(pContext->bApplyImmediately)
			{
				BTST_P_ApplyChanges(pContext);
			}
		}
		BKNI_Sleep(rand() % 100);
	}
done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_StressAnrMad
( BTST_P_Context          *pContext )
{
	bool bMad, bSign=false;
	int i, count, iSnDbAdjust, iAnrMode;
	BVDC_Anr_Settings  AnrConfigure;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		return;
	}

	printf("How many times: ");
	BTST_P_Scanf(pContext, "%d", &i);
	if(i < 0) i = 0;
	count = i;
	
	while(i--) {

		/* set MAD */
		BVDC_Window_GetDeinterlaceConfiguration(hVidWin, &bMad, NULL);
		if(BERR_SUCCESS == BVDC_Window_SetDeinterlaceConfiguration(hVidWin, !bMad, NULL))
		{
			printf("\n\n\n [%d/%d]: %s deinterlacer \n", i, count, (bMad) ? "Disabled" : "Enabled");
		}

		/* set Anr */
		BVDC_Window_GetAnrConfiguration(hVidWin, &AnrConfigure);
		iAnrMode = rand()%3;
		printf("Anr Mode %s \n", iAnrMode==0? "Disable" :
								(iAnrMode==1? "ByPass":"Enable"));
		
		if (iAnrMode == (int)BVDC_FilterMode_eEnable)
		{
			iSnDbAdjust = rand()%30;
			AnrConfigure.iSnDbAdjust = bSign?(0-iSnDbAdjust):iSnDbAdjust;
			
			printf("Anr DB Adjustment [%d],  8-bit storage,  %s \n", AnrConfigure.iSnDbAdjust, bSign?"Compression BitsPerPixel 18":"NO Compression");

			AnrConfigure.ePxlFormat = BPXL_eY18_Cb8_Y08_Cr8;
			AnrConfigure.stVideoTestFeature1.bEnable = bSign;
			if(bSign)
			{
				AnrConfigure.stVideoTestFeature1.ulBitsPerPixel = 18;
			}
			bSign =!bSign;
		
		}
	
		BVDC_Window_SetAnrConfiguration(hVidWin, &AnrConfigure);

		
		if(pContext->bApplyImmediately)
		{
			BTST_P_ApplyChanges(pContext);
		}
		BKNI_Sleep(rand() % 100);
	}
}

void BTST_P_StressMadGame
(BTST_P_Context           *pContext)
{
	BVDC_Deinterlace_Settings stMadSetting;
	uint32_t i = 0;


	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		return;
	}



	stMadSetting.pChromaSettings             = NULL;
	stMadSetting.pMotionSettings             = NULL;
	stMadSetting.pReverse22Settings          = NULL;
	stMadSetting.pReverse32Settings          = NULL;
	stMadSetting.pUpSampler                  = NULL;
	stMadSetting.pDnSampler                  = NULL;
	stMadSetting.pLowAngles                  = NULL;

	stMadSetting.stVideoTestFeature1.bEnable = false;
	stMadSetting.bReverse22Pulldown          = false;
	stMadSetting.bReverse32Pulldown          = false;

	stMadSetting.bShrinkWidth                = pContext->bShrinkWidthForMad;



	stMadSetting.ePxlFormat = BPXL_eY18_Cb8_Y08_Cr8;


	printf("\n No pull down test");
	while (i < BVDC_MadGameMode_eMaxCount)
	{
        printf("\n GameMode idx %d", i);
		stMadSetting.eGameMode = i++;

		switch(stMadSetting.eGameMode)
		{
		case (BVDC_MadGameMode_e5Fields_2Delay):
		case (BVDC_MadGameMode_e3Fields_2Delay):
			printf("\n Error chipset does not support BVDC_MadGameMode_e5Fields_2Delay or BVDC_MadGameMode_e3Fields_2Delay \n");
			break;
#if (0!=BVDC_P_SUPPORT_MAD)
		case (BVDC_MadGameMode_e4Fields_2Delay):
			printf("\n Error chipset does not support BVDC_MadGameMode_e4Fields_2Delay\n");
			break;
#endif
		default:
			if(BERR_SUCCESS == BVDC_Window_SetDeinterlaceConfiguration(hVidWin, true, &stMadSetting))
			{
				BTST_P_ApplyChanges(pContext);
				BKNI_Sleep(5000);
				printf("......Done");
			}
			break;
		}
	}


	i= 0;
	stMadSetting.bReverse22Pulldown 		 = true;
	stMadSetting.bReverse32Pulldown 		 = true;
	printf("\n Pull down test");


	while (i < BVDC_MadGameMode_eMaxCount)
	{
		printf("\n GameMode idx %d", i);
		stMadSetting.eGameMode = i++;

		switch(stMadSetting.eGameMode)
		{
		case (BVDC_MadGameMode_e5Fields_2Delay):
		case (BVDC_MadGameMode_e3Fields_2Delay):
		case (BVDC_MadGameMode_e4Fields_2Delay):
			printf("\n Error chipset does not support BVDC_MadGameMode_e5Fields_2Delay or BVDC_MadGameMode_e4Fields_2Delay or BVDC_MadGameMode_e3Fields_2Delay \n");
				break;
		default:
			if(BERR_SUCCESS == BVDC_Window_SetDeinterlaceConfiguration(hVidWin, true, &stMadSetting))
			{
				BTST_P_ApplyChanges(pContext);
				printf("......Done");
				BKNI_Sleep(5000);
			}
			break;
		}
	}

	printf("\n Finish Mad Game Mode Testing\n");
}


/***************************************************************************
*
*/
void BTST_P_ToggleMaster
( BTST_P_Context          *pContext )
{
	bool bMaster;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetMasterFrameRate(hVidWin, &bMaster);
	if(BERR_SUCCESS == BVDC_Window_SetMasterFrameRate(hVidWin, !bMaster))
	{
		printf("%s master frame rate.\n", (bMaster) ? "Disabled" : "Enabled");

		if(pContext->bApplyImmediately)
		{
			BTST_P_ApplyChanges(pContext);
		}
	}
done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_SetAfd
( BTST_P_Context          *pContext )
{
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	BVDC_AfdSettings stAfdSettings;
	uint32_t ulInput;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetAfdSettings(hVidWin, &stAfdSettings);

	printf("Set AFD mode (Disable=0/Stream=1/User=2) [Current = %d]: ", stAfdSettings.eMode);
	BTST_P_Scanf(pContext, "%u", &ulInput);
	stAfdSettings.eMode = (BVDC_AfdMode)ulInput;


	if(ulInput != BVDC_AfdMode_eDisabled)
	{
		printf("Set AFD clip (Norminal=0/Level1=1/Level2=2) [Current= %d]: ", stAfdSettings.eClip);
		BTST_P_Scanf(pContext, "%u", &ulInput);
		stAfdSettings.eClip = (BVDC_AfdClip)ulInput;

		printf("Set AFD value [Current = %d]: ", stAfdSettings.ulAfd);
		BTST_P_Scanf(pContext, "%u", &stAfdSettings.ulAfd);
	}

	BVDC_Window_SetAfdSettings(hVidWin, &stAfdSettings);

	pContext->ulAfd = stAfdSettings.ulAfd;
	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_ToggleLBox
( BTST_P_Context          *pContext )
{
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	bool  bBoxDetect =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].bBoxDetect;


	if (!bBoxDetect)
	{
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].bBoxDetect = true;
		BVDC_Window_EnableBoxDetect(hVidWin, NULL, NULL, 0, true);
		printf("Enable BoxDetect\n");
	}

	else
	{
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].bBoxDetect = false;
		BVDC_Window_DisableBoxDetect(hVidWin);
		printf("Disable BoxDetect\n");
	}

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

	return;
}

/***************************************************************************
*
*/
void BTST_P_TogglePanScan
( BTST_P_Context          *pContext )
{
	BVDC_PanScanType ePanScanType;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetPanScanType(hVidWin, &ePanScanType);
	ePanScanType = (BVDC_PanScanType_eDisable != ePanScanType)
		? BVDC_PanScanType_eDisable : BVDC_PanScanType_eStream;

	BVDC_Window_SetPanScanType(hVidWin, ePanScanType);

	if(pContext->bApplyImmediately)
	{
		printf("Stream-PanScan is %s\n",
			(BVDC_PanScanType_eDisable != ePanScanType)
			? "Enabled" : "Disabled");

		BTST_P_ApplyChanges(pContext);
	}
done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_TestUserPanScan
( BTST_P_Context          *pContext )
{
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	int32_t lPanX, lPanY;
	uint32_t ulWidthCut;
	bool bChgSize;
	char cKey;

	if(!hVidWin)
	{
		goto done;
	}

	/* start user pan scan */
	BVDC_Window_SetPanScanType(hVidWin, BVDC_PanScanType_eUser);
	BVDC_Window_SetUserPanScan(hVidWin, 0, 0);
	BVDC_Window_SetSrcClip(hVidWin, 0, 0, 0, 0);
	BTST_P_ApplyChanges(pContext);

	/* fine adjust */
	bChgSize = true;
	ulWidthCut = 0;
	lPanX = 0;
	lPanY = 0;
	BVDC_Window_SetAspectRatioMode(hVidWin, BVDC_AspectRatioMode_eUseAllDestination);
	printf("\n");
	printf("Use key 's' to start size change, 'v' to start shift, 'e' to exit this test\n");
	while (true)
	{
		BTST_P_Scanf(pContext, "%c", &cKey);
		switch (cKey)
		{
		case 'e':
			goto done;
			break;

		case 's':
			bChgSize = true;
			printf("\n");
			printf("Use key 'h' 'l' to change size\n");
			break;

		case 'v':
			bChgSize = false;
			printf("\n");
			printf("Use key 'h' 'l' to shift horizontally, key 'j' 'k' to shift vertically:\n");
			break;

		case 'h':
			/* <- */
			if (bChgSize)
			{
				ulWidthCut += 4;
				BVDC_Window_SetSrcClip(hVidWin, ulWidthCut/2, ulWidthCut/2, 0, 0);
			}
			else
			{
				lPanX -= 4;
				BVDC_Window_SetUserPanScan(hVidWin, lPanX, lPanY);
			}
			BTST_P_ApplyChanges(pContext);
			break;

		case 'l':
			/* -> */
			if (bChgSize)
			{
				if (ulWidthCut > 4) ulWidthCut -= 4;
				BVDC_Window_SetSrcClip(hVidWin, ulWidthCut/2, ulWidthCut/2, 0, 0);
			}
			else
			{
				lPanX += 4;
				BVDC_Window_SetUserPanScan(hVidWin, lPanX, lPanY);
			}
			BTST_P_ApplyChanges(pContext);
			break;

		case 'j':
			/* v */
			if (! bChgSize)
			{
				lPanY -= 4;
				BVDC_Window_SetUserPanScan(hVidWin, lPanX, lPanY);
			}
			BTST_P_ApplyChanges(pContext);
			break;

		case 'k':
			/* ^ */
			if (! bChgSize)
			{
				lPanY += 4;
				BVDC_Window_SetUserPanScan(hVidWin, lPanX, lPanY);
			}
			BTST_P_ApplyChanges(pContext);
			break;

		default:
			break;

		}
	}

done:
	/* return to prev pan scan */
	BVDC_Window_SetSrcClip(hVidWin, 0, 0, 0, 0);
	BVDC_Window_SetPanScanType(hVidWin, BVDC_PanScanType_eDisable);
	BVDC_Window_SetAspectRatioMode(hVidWin, BVDC_AspectRatioMode_eBypass);
	BTST_P_ApplyChanges(pContext);

	return;
}

/***************************************************************************
*
*/
void BTST_P_ToggleRgbMatch
( BTST_P_Context          *pContext )
{
	bool bRgbMatch;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetRgbMatching(hVidWin, &bRgbMatch);
	bRgbMatch ^= true;

	printf("RGB matching? %s\n", bRgbMatch? "yes" : "no");
	BVDC_Window_SetRgbMatching(hVidWin, bRgbMatch);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}
done:
	return;
}


/***************************************************************************
*
*/
void BTST_P_RandomMove
( BTST_P_Context          *pContext )
{
	int32_t lLeft, lTop, lHorzVel, lVertVel;
	uint32_t ulWidth, ulHeight;
	BVDC_Window_Handle hWindow;
	const BFMT_VideoInfo *pFmtInfo;
	uint32_t ulCount;

	hWindow = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	pFmtInfo = pContext->apFmtInfo[pContext->iSelectedComp];

	/* Bounce the graphics window around */
	if(hWindow)
	{
		ulCount = 500;
		lHorzVel = BTST_P_RANDOM_VELOCITY;
		lVertVel = BTST_P_RANDOM_VELOCITY;
		BVDC_Window_GetDstRect(hWindow, &lLeft, &lTop, &ulWidth, &ulHeight);

		while(ulCount-- > 0)
		{
			/* Add the current velocity vectors to the position. */
			lLeft += lHorzVel;
			lTop  += lVertVel;

			/* Have we gone off the left edge? */
			if(lLeft <= 0)
			{
				lLeft = 0;
				lHorzVel = BTST_P_RANDOM_VELOCITY;
			}
			/* Have we gone off the right edge? */
			if((lLeft+ulWidth) >= pFmtInfo->ulWidth)
			{
				lLeft = pFmtInfo->ulWidth-ulWidth;
				lHorzVel = -BTST_P_RANDOM_VELOCITY;
			}
			/* Have we gone off the top edge? */
			if(lTop < 0)
			{
				lTop = 0;
				lVertVel = BTST_P_RANDOM_VELOCITY;
			}
			/* Have we gone off the bottom edge? */
			if((lTop+ulHeight) > pFmtInfo->ulHeight)
			{
				lTop = pFmtInfo->ulHeight - ulHeight;
				lVertVel = -BTST_P_RANDOM_VELOCITY;
			}
			BVDC_Window_SetDstRect(hWindow, lLeft, lTop, ulWidth, ulHeight);
			if(pContext->bApplyImmediately)
			{
				BTST_P_ApplyChanges(pContext);
			}
		}
	}

	return;
}

/***************************************************************************
*
*/
void BTST_P_OverScan
( BTST_P_Context          *pContext )
{
	uint32_t ulDstWidth, ulDstHeight;
	uint32_t ulX, ulY, ulWidth, ulHeight, ulPercentOverScan;
	const BFMT_VideoInfo *pFmtInfo = pContext->apFmtInfo[pContext->iSelectedComp];
	BVDC_Window_Handle hVidWin     =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	/* Resize, and do some bound checks. */
	printf("Please enter overscan percentage (eg. 2-50 percent):");
	BTST_P_Scanf(pContext, "%u", &ulPercentOverScan);

	/* clamp Low/High */
	ulPercentOverScan = BSTT_P_CLP(2, 50, ulPercentOverScan);

	ulPercentOverScan *= 2; /* each size top/bot, left/right */
	ulPercentOverScan += 100;

	/* Current Width/Height */
	BVDC_Window_GetDstRect(hVidWin, NULL, NULL, &ulDstWidth, &ulDstHeight);
	ulDstWidth  = BSTT_P_CLP(64, pFmtInfo->ulWidth, ulDstWidth);
	ulDstHeight = BSTT_P_CLP(64, pFmtInfo->ulHeight, ulDstHeight);

	ulWidth  = ulDstWidth  * ulPercentOverScan / 100;
	ulHeight = ulDstHeight * ulPercentOverScan / 100;
	ulX      = (ulWidth  - ulDstWidth)  / 2;
	ulY      = (ulHeight - ulDstHeight) / 2;

	BVDC_Window_SetScalerOutput(hVidWin, ulX, ulY, ulWidth, ulHeight);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_PictureByPicture
( BTST_P_Context          *pContext )
{
	BVDC_Window_Handle hVidWin0, hVidWin1;
	const BFMT_VideoInfo *pFmtInfo;

	uint32_t ulHeight0, ulHeight1, ulWidth0, ulWidth1;
	uint32_t ulX0, ulX1, ulY0, ulY1;

	BSTD_UNUSED(pContext);

	hVidWin0 = pContext->aWinContext[pContext->iSelectedComp][0].hWindow;
	hVidWin1 = pContext->aWinContext[pContext->iSelectedComp][1].hWindow;

	/* Get the handle to the two  on primary display. */
	if(!hVidWin0 || !hVidWin1)
	{
		goto done;
	}

	pFmtInfo = pContext->apFmtInfo[pContext->iSelectedComp];

	/* Set dimensions and location for window 0 */
	ulWidth0  = pFmtInfo->ulWidth/2;
	ulHeight0 = pFmtInfo->ulHeight;
	ulX0      = 0;
	ulY0      = 0;

	BVDC_Window_SetDstRect(hVidWin0, ulX0, ulY0, ulWidth0, ulHeight0);
	BVDC_Window_SetScalerOutput(hVidWin0, 0, 0, ulWidth0, ulHeight0);

	/* Set dimensions and location for window 1 */
	ulWidth1  = pFmtInfo->ulWidth/2;
	ulHeight1 = pFmtInfo->ulHeight;
	ulX1      = pFmtInfo->ulWidth/2;
	ulY1      = 0;

	BVDC_Window_SetDstRect(hVidWin1, ulX1, ulY1, ulWidth1, ulHeight1);
	BVDC_Window_SetScalerOutput(hVidWin1, 0, 0, ulWidth1, ulHeight1);

	if(pContext->bApplyImmediately)
	{
		if(BERR_SUCCESS == BTST_P_ApplyChanges(pContext))
		{
			uint32_t ulAppliedX, ulAppliedY, ulAppliedWidth, ulAppliedHeight;
			BVDC_Window_GetDstRect(hVidWin0, (int32_t*)&ulAppliedX,
				(int32_t*)&ulAppliedY, &ulAppliedWidth, &ulAppliedHeight);
			BDBG_ASSERT(ulX0 == ulAppliedX);
			BDBG_ASSERT(ulY0 == ulAppliedY);
			BDBG_ASSERT(ulWidth0  == ulAppliedWidth);
			BDBG_ASSERT(ulHeight0 == ulAppliedHeight);

			BVDC_Window_GetDstRect(hVidWin1, (int32_t*)&ulAppliedX,
				(int32_t*)&ulAppliedY, &ulAppliedWidth, &ulAppliedHeight);
			BDBG_ASSERT(ulX1 == ulAppliedX);
			BDBG_ASSERT(ulY1 == ulAppliedY);
			BDBG_ASSERT(ulWidth1  == ulAppliedWidth);
			BDBG_ASSERT(ulHeight1 == ulAppliedHeight);
		}
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_SetLumaKey
( BTST_P_Context          *pContext)
{
	uint32_t  bEnable, ucLumaKeyMask, ucLumaKeyHigh, ucLumaKeyLow;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	printf("Please enter new Luma Key settings for window[%d]:\n", pContext->iSelectedWin);
	printf("Enter bLumaKey: Disable(0), Enable(1): ");
	BTST_P_Scanf(pContext, "%u", &bEnable);
	printf("Enter ucLumaKeyMask: ");
	BTST_P_Scanf(pContext, "%u", &ucLumaKeyMask);
	printf("Enter ucLumaKeyHigh: ");
	BTST_P_Scanf(pContext, "%u", &ucLumaKeyHigh);
	printf("Enter ucLumaKeyLow : ");
	BTST_P_Scanf(pContext, "%u", &ucLumaKeyLow);

	BVDC_Window_SetLumaKey(hVidWin, (bool)bEnable, (uint8_t)ucLumaKeyMask,
		(uint8_t)ucLumaKeyHigh, (uint8_t)ucLumaKeyLow);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;

}

/***************************************************************************
*
*/
void BTST_P_GetLumaKey
( BTST_P_Context          *pContext)
{
	bool      bEnable;
	uint8_t   ucLumaKeyMask, ucLumaKeyHigh, ucLumaKeyLow;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetLumaKey(hVidWin, &bEnable, &ucLumaKeyMask, &ucLumaKeyHigh, &ucLumaKeyLow);
	printf("Luma Key settings for window[%d]:\n", pContext->iSelectedWin);
	printf("bLumaKey     : %d\n", bEnable);
	printf("ucLumaKeyMask: %d\n", ucLumaKeyMask);
	printf("ucLumaKeyHigh: %d\n", ucLumaKeyHigh);
	printf("ucLumaKeyLow : %d\n", ucLumaKeyLow);

done:
	return;

}

/***************************************************************************
*
*/
void BTST_P_SetForceCapture
( BTST_P_Context          *pContext)
{
	bool      bForceCapture;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetForceCapture(hVidWin, &bForceCapture);

	bForceCapture = !bForceCapture;

	printf("%s force capture for window[%d]:\n",
		bForceCapture ? "Enable" : "Disable",
		pContext->iSelectedWin);

	BVDC_Window_SetForceCapture(hVidWin, bForceCapture);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;

}

/***************************************************************************
*
*/
void BTST_P_ChgFixColor
( BTST_P_Context          *pContext )
{
	BVDC_Source_Handle hSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	bool bEnable;
	uint32_t ulEnable;
	uint32_t ulR, ulG, ulB;
	uint32_t eFieldId;

	if(!hSrc)
	{
		goto done;
	}

	printf("Please enter the field polarity [T=%d/B=%d/F=%d]: ",
		BAVC_Polarity_eTopField, BAVC_Polarity_eBotField, BAVC_Polarity_eFrame);
	BTST_P_Scanf(pContext, "%u", &eFieldId);

	BVDC_Test_Source_GetFixColor(hSrc, eFieldId, &bEnable, &ulR, &ulG, &ulB);
	printf("Current fix color setting: %s\n", (bEnable) ? "true" : "false");
	printf("R G B = 0x%x 0x%x 0x%x \n", ulR, ulG, ulB);
	printf("Do you want to enable? [y=1/n=0]: ");
	BTST_P_Scanf(pContext, "%u", &ulEnable);
	bEnable = (ulEnable == 1) ? true : false;
	if(bEnable)
	{
		printf("Enter new fix color (0xRR, 0xGG, 0xBB): ");
		BTST_P_Scanf(pContext, "%x %x %x", &ulR, &ulG, &ulB);
	}
	BVDC_Test_Source_SetFixColor(hSrc, eFieldId, bEnable, ulR, ulG, ulB);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_PrintCsc
( int32_t                *pCsc )
{
	printf("0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
		pCsc[0], pCsc[1], pCsc[2], pCsc[3], pCsc[4]);
	printf("0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
		pCsc[5], pCsc[6], pCsc[7], pCsc[8], pCsc[9]);
	printf("0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
		pCsc[10], pCsc[11], pCsc[12], pCsc[13], pCsc[14]);
}

/***************************************************************************
*
*/
void BTST_P_ChgSrcCsc
( BTST_P_Context          *pContext )
{
	BVDC_Source_Handle hSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	uint32_t ulEnable;
	bool bEnable;
	uint32_t ulShift;
	int16_t sY, sCb, sCr;
	int32_t stCsc[BVDC_CSC_COEFF_COUNT];
	int i;

	if(!hSrc)
	{
		goto done;
	}

	BVDC_Source_GetColorMatrix(hSrc, &bEnable, stCsc, &ulShift);
	printf("Current SRC CSC: %s shift=%d \n", (
		bEnable) ? "User Matrix" : "Interal CSC", ulShift);
	BTST_P_PrintCsc(stCsc);

	printf("Override SRC CSC? [yes=1/no=0]: ");
	BTST_P_Scanf(pContext, "%u", &ulEnable);
	bEnable = (ulEnable == 1) ? true : false;

	for(i = 0; i < BVDC_CSC_COEFF_COUNT; i++)
	{
		stCsc[i] = 0;
	}

	if(bEnable)
	{
		printf("Enter shift: ");
		BTST_P_Scanf(pContext, "%u", &ulShift);
		printf("Enter color code \
			   \n\tWhite(0), Yellow(1), Cyan(2), Green(3), \
			   \n\tMagenta(4), Red(5), Blue(6), Black(7): ");

		BTST_P_Scanf(pContext, "%d", &i);
		if     (0 == i){sY = 180; sCb = 128; sCr = 128;}
		else if(1 == i){sY = 162; sCb =  44; sCr = 142;}
		else if(2 == i){sY = 131; sCb = 156; sCr =  44;}
		else if(3 == i){sY = 112; sCb =  72; sCr =  58;}
		else if(4 == i){sY =  84; sCb = 184; sCr = 198;}
		else if(5 == i){sY =  65; sCb = 100; sCr = 212;}
		else if(6 == i){sY =  35; sCb = 212; sCr = 114;}
		else if(7 == i){sY =  16; sCb = 128; sCr = 128;}
		else return;

		stCsc[4]  = sY  * (1 << ulShift);
		stCsc[9]  = sCb * (1 << ulShift);
		stCsc[14] = sCr * (1 << ulShift);
	}

	BVDC_Source_SetColorMatrix(hSrc, bEnable, stCsc, ulShift);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
*
*/
void BTST_P_ChgDvoCsc
( BTST_P_Context          *pContext )
{
	BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];
	BAVC_MatrixCoefficients eColorMatrix;

	uint32_t ulEnable;
	bool bEnable;
	uint32_t ulShift;
	int32_t stCsc[BVDC_CSC_COEFF_COUNT];
	int i;

	if(!hDisplay)
	{
		goto done;
	}

	BVDC_Display_GetHdmiConfiguration(hDisplay, BVDC_Hdmi_0, &eColorMatrix);
	BVDC_Display_GetDvoColorMatrix(hDisplay, &bEnable, stCsc, &ulShift);
	printf("Current DVO CSC: %s shift=%d \n", (
		bEnable) ? "User Matrix" : "Interal CSC", ulShift);
	BTST_P_PrintCsc(stCsc);

	printf("Override DVO CSC? [yes=1/no=0]: ");
	BTST_P_Scanf(pContext, "%u", &ulEnable);
	bEnable = (ulEnable == 1) ? true : false;

	for(i = 0; i < BVDC_CSC_COEFF_COUNT; i++)
	{
		stCsc[i] = 0;
	}

	if(bEnable)
	{
		printf("Enter shift: ");
		BTST_P_Scanf(pContext, "%u", &ulShift);

		if ((eColorMatrix == BAVC_MatrixCoefficients_eHdmi_RGB) ||
			(eColorMatrix == BAVC_MatrixCoefficients_eDvi_Full_Range_RGB))
		{
			uint32_t ulR, ulG, ulB;

			printf("Enter fixed color (0xRR, 0xGG, 0xBB):");
			BTST_P_Scanf(pContext, "%x %x %x", &ulR, &ulG, &ulB);
			ulR = BTST_P_MIN(ulR, BTST_P_BG_COLOR_RANGE_MAX);
			ulG = BTST_P_MIN(ulG, BTST_P_BG_COLOR_RANGE_MAX);
			ulB = BTST_P_MIN(ulB, BTST_P_BG_COLOR_RANGE_MAX);

			stCsc[4]  = ulR * (1 << ulShift);
			stCsc[9]  = ulG * (1 << ulShift);
			stCsc[14] = ulB * (1 << ulShift);
		}
		else
		{
			uint16_t sY, sCb, sCr;

			printf("Enter color code \
				   \n\tWhite(0), Yellow(1), Cyan(2), Green(3), \
				   \n\tMagenta(4), Red(5), Blue(6), Black(7): ");
			BTST_P_Scanf(pContext, "%d", &i);
			if     (0 == i){sY = 180; sCb = 128; sCr = 128;}
			else if(1 == i){sY = 162; sCb =  44; sCr = 142;}
			else if(2 == i){sY = 131; sCb = 156; sCr =  44;}
			else if(3 == i){sY = 112; sCb =  72; sCr =  58;}
			else if(4 == i){sY =  84; sCb = 184; sCr = 198;}
			else if(5 == i){sY =  65; sCb = 100; sCr = 212;}
			else if(6 == i){sY =  35; sCb = 212; sCr = 114;}
			else if(7 == i){sY =  16; sCb = 128; sCr = 128;}
			else return;

			stCsc[4]  = sY  * (1 << ulShift);
			stCsc[9]  = sCb * (1 << ulShift);
			stCsc[14] = sCr * (1 << ulShift);
		}
	}

	BVDC_Display_SetDvoColorMatrix(hDisplay, bEnable, stCsc, ulShift);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
*
*/
void BTST_P_ChgWinCsc
( BTST_P_Context          *pContext )
{
	int iVidSrcId =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].iVidSrcId;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	uint32_t ulEnable;
	bool bEnable;
	uint32_t ulShift;
	int16_t sY, sCb, sCr;
	int32_t stCsc[BVDC_CSC_COEFF_COUNT];
	int i;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetColorMatrix(hVidWin, &bEnable, stCsc, &ulShift);

	printf("Current CMP CSC: %s shift=%d \n", (
		bEnable) ? "User Matrix" : "Interal CSC", ulShift);
	BTST_P_PrintCsc(stCsc);

	printf("Override CMP CSC? [yes=1/no=0]: ");
	BTST_P_Scanf(pContext, "%u", &ulEnable);
	bEnable = (ulEnable == 1) ? true : false;

	for(i = 0; i < BVDC_CSC_COEFF_COUNT; i++)
	{
		stCsc[i] = 0;
	}

	if(bEnable)
	{
		if((BAVC_SourceId_eGfx0 == iVidSrcId) ||
			(BAVC_SourceId_eGfx1 == iVidSrcId) ||
			(BAVC_SourceId_eGfx2 == iVidSrcId) ||
			(BAVC_SourceId_eGfx3 == iVidSrcId))
		{
			uint32_t ulR, ulG, ulB;
			printf("Enter shift: ");
			BTST_P_Scanf(pContext, "%u", &ulShift);
			printf("Enter fixed color (0xRR, 0xGG, 0xBB):");
			BTST_P_Scanf(pContext, "%x %x %x", &ulR, &ulG, &ulB);
			ulR = BTST_P_MIN(ulR, BTST_P_BG_COLOR_RANGE_MAX);
			ulG = BTST_P_MIN(ulG, BTST_P_BG_COLOR_RANGE_MAX);
			ulB = BTST_P_MIN(ulB, BTST_P_BG_COLOR_RANGE_MAX);

			stCsc[4]  = ulG * (1 << ulShift);
			stCsc[9]  = ulB * (1 << ulShift);
			stCsc[14] = ulR * (1 << ulShift);
		}
		else
		{
			printf("Enter shift: ");
			BTST_P_Scanf(pContext, "%u", &ulShift);
			printf("Enter color code \
				   \n\tWhite(0), Yellow(1), Cyan(2), Green(3), \
				   \n\tMagenta(4), Red(5), Blue(6), Black(7): ");

			BTST_P_Scanf(pContext, "%d", &i);
			if     (0 == i){sY = 180; sCb = 128; sCr = 128;}
			else if(1 == i){sY = 162; sCb =  44; sCr = 142;}
			else if(2 == i){sY = 131; sCb = 156; sCr =  44;}
			else if(3 == i){sY = 112; sCb =  72; sCr =  58;}
			else if(4 == i){sY =  84; sCb = 184; sCr = 198;}
			else if(5 == i){sY =  65; sCb = 100; sCr = 212;}
			else if(6 == i){sY =  35; sCb = 212; sCr = 114;}
			else if(7 == i){sY =  16; sCb = 128; sCr = 128;}
			else return;

			stCsc[4]  = sY  * (1 << ulShift);
			stCsc[9]  = sCb * (1 << ulShift);
			stCsc[14] = sCr * (1 << ulShift);
		}
	}

	BVDC_Window_SetColorMatrix(hVidWin, bEnable, stCsc, ulShift);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_SetGameDelay
( BTST_P_Context          *pContext )
{
	int i;
	BVDC_Window_GameModeSettings stGameSettings;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetGameModeDelay(hVidWin, &stGameSettings);

	if(stGameSettings.bEnable)
	{
		printf("Current game mode target buffer delay = %d [usec]\n",
			stGameSettings.ulBufferDelayTarget);
		printf("Current game mode delay tolerance     = %d [usec]\n",
			stGameSettings.ulBufferDelayTolerance);
	}

	printf("Do you want to enable game mode delay control?[1=y/0=n]\n");
	BTST_P_Scanf(pContext, "%d", &i);
	if(i)
	{
		printf("Do you want to enable coarse source rate tracking (e.g. PC input)?[1=y/0=n]\n");
		BTST_P_Scanf(pContext, "%d", &i);
		stGameSettings.bForceCoarseTrack = i;
		printf("Please enter game mode target buffer delay[usec]:\n");
		BTST_P_Scanf(pContext, "%d", &i);
		stGameSettings.ulBufferDelayTarget = i;
		printf("Please enter game mode delay tolerance[usec]:\n");
		BTST_P_Scanf(pContext, "%d", &i);
		stGameSettings.ulBufferDelayTolerance = i;
		stGameSettings.bEnable = true;
	}
	else
	{
		stGameSettings.bEnable = false;
	}

	BVDC_Window_SetGameModeDelay(hVidWin, &stGameSettings);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_ToggleAnr
( BTST_P_Context          *pContext )
{
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	BVDC_Anr_Settings  AnrConfigure;
	int  iMode = 0, iSnDbAdj = 0, iPixelFmt;
	uint32_t ulCompression;

	if(!hVidSrc)
	{
		goto done;
	}

	BVDC_Window_GetAnrConfiguration(hVidWin, &AnrConfigure);

	printf("Please select ANR mode for window %d:\n", pContext->iSelectedWin);
	printf("(0) disable; (1) bypass; (2) enable\n");
	BTST_P_Scanf(pContext, "%d", &iMode);
	iMode = (iMode > BVDC_FilterMode_eEnable)?  BVDC_FilterMode_eEnable  : iMode;
	iMode = (iMode < BVDC_FilterMode_eDisable)? BVDC_FilterMode_eDisable : iMode;
	AnrConfigure.eMode = iMode;

	if (iMode == BVDC_FilterMode_eEnable)
	{
		printf("Please input AnDb adjustment (-30 to 30):\n");
		BTST_P_Scanf(pContext, "%d", &iSnDbAdj);
		AnrConfigure.iSnDbAdjust = iSnDbAdj;

		printf("What's the pixel format of the ANR storage? 0)8-bit 422; 1) or else 10-bit 422;\n");
		BTST_P_Scanf(pContext, "%d", &iPixelFmt);

		AnrConfigure.ePxlFormat = iPixelFmt ?
		BPXL_eX2_Y010_Cb10_Y110_X2_Cr10_Y010_Cb10_X2_Y110_Cr10_Y010_X2_Cb10_Y110_Cr10 :
		BPXL_eY18_Cb8_Y08_Cr8;


		printf("Compression [%s]: ", (AnrConfigure.stVideoTestFeature1.bEnable) ? "Enable" : "Disable");
		BTST_P_Scanf(pContext, "%u", &ulCompression);
		if(ulCompression)
		{
			printf("ulBitsPerPixel (U31.1) [%d]: ", AnrConfigure.stVideoTestFeature1.ulBitsPerPixel);
			BTST_P_Scanf(pContext, "%u", &AnrConfigure.stVideoTestFeature1.ulBitsPerPixel);
		}
		AnrConfigure.stVideoTestFeature1.bEnable = ulCompression;
	}

	BVDC_Window_SetAnrConfiguration(hVidWin, &AnrConfigure);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

void BTST_P_PcInSyncConfig
( BTST_P_Context          *pContext )
{
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	BVDC_Source_SyncSettings stSyncSettings;
	int bChoice;

	if(!hVidSrc)
	{
		goto done;
	}

	BVDC_Source_GetSyncConfiguration(hVidSrc, &stSyncSettings);
	printf("Current sync config:\n");
	printf("	bManualClock     = %d\n", stSyncSettings.bManualClock);
	printf("	bManualPhase     = %d\n", stSyncSettings.bManualPhase);
	printf("	bManualPosition  = %d\n", stSyncSettings.bManualPosition);
	printf("	lClockAdj        = %d\n", stSyncSettings.lClockAdj);
	printf("	aulPhase[0]      = %d\n", stSyncSettings.aulPhase[0]);
	printf("	aulPhase[1]      = %d\n", stSyncSettings.aulPhase[1]);
	printf("	aulPhase[2]      = %d\n", stSyncSettings.aulPhase[2]);

	printf("\nPlease enter your choice:\n");
	printf("	bManualClock     = [1=true/0=false]\n");
	BTST_P_Scanf(pContext, "%d", &bChoice);
	stSyncSettings.bManualClock = (bool)bChoice;
	if(bChoice)
	{
		printf("	lClockAdj        = \n");
		BTST_P_Scanf(pContext, "%d", &stSyncSettings.lClockAdj);
	}
	printf("	bManualPhase     = [1=true/0=false]\n");
	BTST_P_Scanf(pContext, "%d", &bChoice);
	stSyncSettings.bManualPhase = (bool)bChoice;
	if(bChoice)
	{
		printf("	aulPhase[0]      = \n");
		BTST_P_Scanf(pContext, "%u", &stSyncSettings.aulPhase[0]);
		printf("	aulPhase[1]      = \n");
		BTST_P_Scanf(pContext, "%u", &stSyncSettings.aulPhase[1]);
		printf("	aulPhase[2]      = \n");
		BTST_P_Scanf(pContext, "%u", &stSyncSettings.aulPhase[2]);
	}
	printf("	bManualPosition  = [1=true/0=false]\n");
	BTST_P_Scanf(pContext, "%d", &bChoice);
	stSyncSettings.bManualPosition = (bool)bChoice;
	BVDC_Source_SetSyncConfiguration(hVidSrc, &stSyncSettings);
	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

void BTST_P_PcInSyncStatus
( BTST_P_Context          *pContext )
{
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	BVDC_Source_SyncStatus stSyncStatus;

	if(!hVidSrc)
	{
		goto done;
	}

	BVDC_Source_GetSyncStatus(hVidSrc, &stSyncStatus);
	printf("Current sync status:\n");
	printf("	bClockLocked     = %d\n", stSyncStatus.bClockLocked);
	printf("	abPhaseLocked[0] = %d\n", stSyncStatus.abPhaseLocked[0]);
	printf("	abPhaseLocked[1] = %d\n", stSyncStatus.abPhaseLocked[1]);
	printf("	abPhaseLocked[2] = %d\n", stSyncStatus.abPhaseLocked[2]);
	printf("	aulPhase[0]      = %d\n", stSyncStatus.aulPhase[0]);
	printf("	aulPhase[1]      = %d\n", stSyncStatus.aulPhase[1]);
	printf("	aulPhase[2]      = %d\n", stSyncStatus.aulPhase[2]);

done:
	return;
}


void BTST_P_HVStart
( BTST_P_Context          *pContext )
{
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	bool bOverride;
	int bChoice;
	uint32_t ulHstart;
	uint32_t ulVstart;

	if(!hVidSrc)
	{
		goto done;
	}

	BVDC_Source_GetHVStart(hVidSrc, &bOverride, &ulHstart, &ulVstart);
	printf("Current HV start config:\n");
	printf("	bOverride = %d\n", bOverride);
	printf("	ulHstart  = %d\n", ulHstart);
	printf("	ulVstart  = %d\n", ulVstart);

	printf("\nPlease enter your choice:\n");
	printf("	bOverride = [1=true/0=false]\n");
	BTST_P_Scanf(pContext, "%d", &bChoice);
	bOverride = (bool)bChoice;
	if(bOverride)
	{
		printf("	ulHstart  = \n");
		BTST_P_Scanf(pContext, "%u", &ulHstart);
		printf("	ulVstart  = \n");
		BTST_P_Scanf(pContext, "%u", &ulVstart);
	}
	BVDC_Source_SetHVStart(hVidSrc, bOverride, ulHstart, ulVstart);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

void BTST_P_HdDviConfig
( BTST_P_Context          *pContext )
{
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	uint32_t ulInputPort;
	BVDC_HdDvi_Settings  stHdDviSettings;
	int      iChoice, iWidth, iHeight;

	if(!hVidSrc)
	{
		goto done;
	}

	BVDC_Source_GetInputPort(hVidSrc, &ulInputPort);
	BVDC_Source_GetHdDviConfiguration(hVidSrc, &stHdDviSettings);

	printf("\nPlease enter your choice:\n");
	printf("\t Inpur port[%s]: Ext(0), Hdr(1):  ",
		(ulInputPort == BVDC_HdDviInput_Ext) ? "Ext" : "Hdr");
	BTST_P_Scanf(pContext, "%u", &ulInputPort);
	printf("\t bEnableDe[%d]: DisableDe(0), EnableDe(1):  ", stHdDviSettings.bEnableDe);
	BTST_P_Scanf(pContext, "%d", &iChoice);
	stHdDviSettings.bEnableDe = (bool)iChoice;
	printf("\t eInputDataMode[%d]: 36bit (0), 24bit(1), 30bit(2):  ",
		stHdDviSettings.eInputDataMode);
	BTST_P_Scanf(pContext, "%d", &iChoice);
	stHdDviSettings.eInputDataMode = iChoice;

	/* Tolerance */
	printf("\t Tolerance:");
	BTST_P_Scanf(pContext, "%d %d", &iWidth, &iHeight);
	stHdDviSettings.stFmtTolerence.ulWidth  = iWidth;
	stHdDviSettings.stFmtTolerence.ulHeight = iHeight;

	BVDC_Source_SetInputPort(hVidSrc, ulInputPort);
	BVDC_Source_SetHdDviConfiguration(hVidSrc, &stHdDviSettings);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_GetSrcStatus
( BTST_P_Context          *pContext )
{
	BVDC_Source_Handle hVidSrc = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	BVDC_Source_InputStatus   stInputStatus;

	if(!hVidSrc)
	{
		goto done;
	}

	if(BERR_SUCCESS == BVDC_Source_GetInputStatus(hVidSrc, &stInputStatus))
	{
		printf("\nCurrent Input status for source %d:\n", pContext->iSelectedVidSrc);
		printf("	bNoSignal      = %d\n", stInputStatus.bNoSignal);
		printf("	bVsyncDetected = %d\n", stInputStatus.bVsyncDetected);
		printf("	bInterlaced    = %d\n", stInputStatus.bInterlaced);
		printf("	ulHPolarity    = %d\n", stInputStatus.ulHPolarity);
		printf("	ulVPolarity    = %d\n", stInputStatus.ulVPolarity);
		printf("	ulAvWidth      = %d\n", stInputStatus.ulAvWidth);
		printf("	ulAvHeight     = %d\n", stInputStatus.ulAvHeight);
		printf("	ulHBlank       = %d\n", stInputStatus.ulHBlank);
		printf("	ulVBlank       = %d\n", stInputStatus.ulVBlank);
		printf("	ulVertFreq     = %d\n", stInputStatus.ulVertFreq);
		printf("	Format         = %s\n", stInputStatus.pFmtInfo
			? stInputStatus.pFmtInfo->pchFormatStr : "Unknown");
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_CustomVdec
( BTST_P_Context          *pContext )
{
	int iCustomize = 0;
	bool bCustomized = false;
	BVDC_Source_Handle hSrc  = pContext->aSrcContext[pContext->iSelectedVidSrc].hVidSrc;
	BVDC_Vdec_Settings *pSettings = &pContext->stVdecCfg;

	/* Get default */
	BVDC_Source_GetVdecConfiguration(hSrc, pSettings);

	/* Customized VDEC's settings here! */
	printf("Enter choice to customized PEDESTAL/ACGC/OVERSAMPLE vdec settings Yes(1), No(0):");
	BTST_P_Scanf(pContext, "%d", &iCustomize);
	if(iCustomize)
	{
		iCustomize      = 0;
		bCustomized     = true;
		pSettings->bFullRange    = true;
		pSettings->bOverSample   = true;
		pSettings->bBypassAcgc   = false;
		pSettings->ulBlackOffset = 21;
	}

	printf("Enter choice to customized AWC vdec settings Yes(1), No(0):");
	BTST_P_Scanf(pContext, "%d", &iCustomize);
	if(iCustomize)
	{
		iCustomize      = 0;
		bCustomized     = true;
		pSettings->pAwc = &pContext->stAwc;
	}

	printf("Enter choice to customized Backend vdec settings Yes(1), No(0):");
	BTST_P_Scanf(pContext, "%d", &iCustomize);
	if(iCustomize)
	{
		iCustomize          = 0;
		bCustomized         = true;
		pSettings->pBackend = &pContext->stBackend;
		pSettings->pBackend->bCoring = false;
	}

	printf("Enter choice to customized CTI vdec settings Yes(1), No(0):");
	BTST_P_Scanf(pContext, "%d", &iCustomize);
	if(iCustomize)
	{
		iCustomize      = 0;
		bCustomized     = true;
		pSettings->pCti = &pContext->stCti;
		pSettings->pCti->eBandFilter = BVDC_Vdec_BandLimitFilter_eBw_120;

		pSettings->pCti->eIirIfDelay = BVDC_Vdec_IirIfDelay_eTwo;
		pSettings->pCti->eIirFilter1 = BVDC_Vdec_IirFilter_eIir_1;
		pSettings->pCti->eFirFilter1 = BVDC_Vdec_FirFilter_eFir_1;
		pSettings->pCti->eFirFilter2 = BVDC_Vdec_FirFilter_eFir_1;
	}

	printf("Enter choice to customized COMB vdec settings Yes(1), No(0):");
	BTST_P_Scanf(pContext, "%d", &iCustomize);
	if(iCustomize)
	{
		iCustomize       = 0;
		bCustomized      = true;
		pSettings->pComb = &pContext->stComb;
	}

	printf("Enter choice to customized DCR vdec settings Yes(1), No(0):");
	BTST_P_Scanf(pContext, "%d", &iCustomize);
	if(iCustomize)
	{
		iCustomize      = 0;
		bCustomized     = true;
		pSettings->pDcr = &pContext->stDcr;
		pSettings->pDcr->ulStart   = 10;
		pSettings->pDcr->ulEnd     = 125;
		pSettings->pDcr->aulRef[0] = 199;
		pSettings->pDcr->aulRef[1] = 200;
		pSettings->pDcr->aulRef[2] = 201;
	}

	printf("Enter choice to customized ANALOG_GAIN vdec settings Yes(1), No(0):");
	BTST_P_Scanf(pContext, "%d", &iCustomize);
	if(iCustomize)
	{
		iCustomize        = 0;
		bCustomized       = true;
		pSettings->pAGain = &pContext->stAGain;
		pSettings->pAGain->aulGain[0] = 1;
		pSettings->pAGain->aulGain[1] = 2;
		pSettings->pAGain->aulGain[2] = 3;
	}

	printf("Enter choice to customized DIGITAL_GAIN vdec settings Yes(1), No(0):");
	BTST_P_Scanf(pContext, "%d", &iCustomize);
	if(iCustomize)
	{
		iCustomize        = 0;
		bCustomized       = true;
		pSettings->pDGain = &pContext->stDGain;
		pSettings->pDGain->aulGain[0] = 220;
		pSettings->pDGain->aulGain[1] = 221;
		pSettings->pDGain->aulGain[2] = 222;
	}

	printf("Enter choice to customized YCRCB_DELAY vdec settings Yes(1), No(0):");
	BTST_P_Scanf(pContext, "%d", &iCustomize);
	if(iCustomize)
	{
		iCustomize        = 0;
		bCustomized       = true;
		pSettings->pDelay = &pContext->stDelay;
		pSettings->pDelay->ulYDelay  = 1;
		pSettings->pDelay->ulCrDelay = 2;
		pSettings->pDelay->ulCbDelay = 3;
	}

	BVDC_Source_SetVdecConfiguration(hSrc,
		bCustomized ? &pContext->stVdecCfg : NULL);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

	return;
}

#if (BFramework_NUM_XVD_CORES > 0) && !BTST_VDC_ONLY
/***************************************************************************
*
*/
void BTST_P_ToggleXvdFreeze
( BTST_P_Context          *pContext )
{
	bool bFreeze = pContext->astMDecInfo[pContext->eSelectedMDec].abChnFrozen[pContext->iSelectedMDecCh];

	if(!pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh])
	{
		BDBG_ERR(("The selected channel is not opened yet!"));
		return;
	}

	if(bFreeze)
	{
		BXVD_DisableVideoFreeze((BXVD_ChannelHandle)pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh]);
		printf("Disable xvd freeze.\n");
	}
	else
	{
		BXVD_EnableVideoFreeze((BXVD_ChannelHandle)pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh]);
		printf("Enable xvd freeze.\n");
	}

	pContext->astMDecInfo[pContext->eSelectedMDec].abChnFrozen[pContext->iSelectedMDecCh] = !bFreeze;
	return;
}

void BTST_P_SetXvdRefreshRate
( BTST_P_Context          *pContext )
{
	BXVD_ChannelHandle     hXvdCh = (BXVD_ChannelHandle)pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh];
	uint32_t               ulChoice;

	BXVD_GetMonitorRefreshRate(hXvdCh, &ulChoice);
	printf("Current channel monitor refresh rate: 0x%x\n", ulChoice);
	printf("Please choose new monitor rate: 0) 60Hz; 1) 50Hz; 2) 24Hz; 3) 25 Hz; 4) 30Hz: 5) 48 Hz \n");
	BTST_P_Scanf(pContext, "%u", &ulChoice);
	switch(ulChoice)
	{
	case 5:
		ulChoice = BXVD_MONITOR_REFRESH_RATE_48Hz; break;
	case 4:
		ulChoice = BXVD_MONITOR_REFRESH_RATE_30Hz; break;
	case 3:
		ulChoice = BXVD_MONITOR_REFRESH_RATE_25Hz; break;
	case 2:
		ulChoice = BXVD_MONITOR_REFRESH_RATE_24Hz; break;
	case 1:
		ulChoice = BXVD_MONITOR_REFRESH_RATE_50Hz; break;
	case 0:
	default:
		ulChoice = BXVD_MONITOR_REFRESH_RATE_60Hz; break;
	}
	BXVD_SetMonitorRefreshRate(hXvdCh, ulChoice);
}

void BTST_P_SetXvd1080pMode
( BTST_P_Context          *pContext )
{
	BXVD_ChannelHandle     hXvdCh = (BXVD_ChannelHandle)pContext->astMDecInfo[pContext->eSelectedMDec].ahMDecChn[pContext->iSelectedMDecCh];
	uint32_t               ulChoice;
	BXVD_1080pScanMode     e1080pMode;

	BXVD_Get1080pScanMode(hXvdCh, &e1080pMode);
	printf("Current channel 1080 scan mode: %d\n", e1080pMode);
	printf("Please choose new 1080p mode: 0) Default (1080i or 1080p pass-thru); 1) BXVD_1080pScanMode_eAdvanced (PsF); \n");
	BTST_P_Scanf(pContext, "%u", &ulChoice);
	switch(ulChoice)
	{
	case 1:
		e1080pMode = BXVD_1080pScanMode_eAdvanced; break;
	case 0:
	default:
		e1080pMode = BXVD_1080pScanMode_eDefault; break;
	}
	BXVD_Set1080pScanMode(hXvdCh, e1080pMode);
}
#endif

void BTST_P_ForceSyncLock
( BTST_P_Context *pContext )
{
	pContext->bForceSyncLock ^= true;
	printf("toggles bForceSyncLock = %d\n", pContext->bForceSyncLock);
}

void BTST_P_GetMadDynamicDefault
( BTST_P_Context *pContext )
{
	BVDC_Window_Handle  hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	BVDC_Deinterlace_Settings          stMadSettings;
	BVDC_Deinterlace_Reverse22Settings stReverse22Setting;
	BVDC_Deinterlace_Reverse32Settings stReverse32Setting;
	BVDC_Deinterlace_ChromaSettings    stChromaSettings;
	BVDC_Deinterlace_MotionSettings    stMotionSettings;
	BVDC_422To444UpSampler             stupSampler;
	BVDC_444To422DnSampler             stdownSampler;
	BVDC_Deinterlace_LowAngleSettings  stlowAngles;

	if(!hVidWin)
	{
		goto done;
	}

	BKNI_Memset(&stMadSettings, 0, sizeof(stMadSettings));
	stMadSettings.pReverse22Settings = &stReverse22Setting;
	stMadSettings.pReverse32Settings = &stReverse32Setting;
	stMadSettings.pChromaSettings    = &stChromaSettings;
	stMadSettings.pMotionSettings    = &stMotionSettings;
	stMadSettings.pUpSampler         = &stupSampler;
	stMadSettings.pDnSampler         = &stdownSampler;
	stMadSettings.pLowAngles         = &stlowAngles;
	BVDC_Window_GetDeinterlaceDefaultConfiguration(hVidWin, &stMadSettings);

	printf("==================================================================\n");
	printf("Dynamic default settings:\n");
	printf("pUpSampler:\n");
	printf("\t\t bUnbiasedRound: %d\n", stMadSettings.pUpSampler->bUnbiasedRound);
	printf("\t\t eFilterType   : %d\n", stMadSettings.pUpSampler->eFilterType);
	printf("\t\t eRingRemoval  : %d\n", stMadSettings.pUpSampler->eRingRemoval);

	printf("pDnSampler:\n");
	printf("\t\t eFilterType   : %d\n", stMadSettings.pDnSampler->eFilterType);
	printf("\t\t eRingRemoval  : %d\n", stMadSettings.pDnSampler->eRingRemoval);

	/* Low angles */
	printf("pLowAngles:\n");
	printf("\t\t ulLaControlDirRatio         : %d\n", stMadSettings.pLowAngles->ulLaControlDirRatio);
	printf("\t\t ulLaControlRangeLimitScale  : %d\n", stMadSettings.pLowAngles->ulLaControlRangeLimitScale);
	printf("\t\t ulLaMinNorthStrength        : %d\n", stMadSettings.pLowAngles->ulLaMinNorthStrength);

	printf("pMotionSettings:\n");
	printf("\t\t eTmMode     : %d\n", stMadSettings.pMotionSettings->eTmMode);
	printf("\t\t eSmMode     : %d\n", stMadSettings.pMotionSettings->eSmMode);
	printf("\t\t bEnableQmK  : %d\n", stMadSettings.pMotionSettings->bEnableQmK);
	printf("\t\t bEnableQmL  : %d\n", stMadSettings.pMotionSettings->bEnableQmL);
	printf("\t\t bEnableQmM  : %d\n", stMadSettings.pMotionSettings->bEnableQmM);

	printf("pChromaSettings:\n");
	printf("\t\t bChromaField420EdgeDetMode    : %d\n", stMadSettings.pChromaSettings->bChromaField420EdgeDetMode);
	printf("\t\t bChromaField420InitPhase      : %d\n", stMadSettings.pChromaSettings->bChromaField420InitPhase);
	printf("\t\t eChromaField420InvMethod      : %d\n", stMadSettings.pChromaSettings->eChromaField420InvMethod);
	printf("\t\t eChroma422MotionAdaptiveMode  : %d\n", stMadSettings.pChromaSettings->eChroma422MotionAdaptiveMode);
	printf("\t\t eChroma422InverseTelecineMode : %d\n", stMadSettings.pChromaSettings->eChroma422InverseTelecineMode);
	printf("\t\t ulMaxXChroma                  : %d\n", stMadSettings.pChromaSettings->ulMaxXChroma);
	printf("\t\t eChroma422MotionMode          : %d\n", stMadSettings.pChromaSettings->eChroma422MotionMode);
	printf("\t\t eChroma420MotionMode          : %d\n", stMadSettings.pChromaSettings->eChroma420MotionMode);
	printf("\t\t bMS_3548                      : %d\n", stMadSettings.pChromaSettings->bMS_3548);
	printf("\t\t bMT_3548                      : %d\n", stMadSettings.pChromaSettings->bMT_3548);
	printf("\t\t eChroma420MotionAdaptiveMode  : %d\n", stMadSettings.pChromaSettings->eChroma420MotionAdaptiveMode);

	printf("pReverse32Settings:\n");
	printf("\t\t ulPhaseMatchThreshold : %x\n", stMadSettings.pReverse32Settings->ulPhaseMatchThreshold);
	printf("\t\t ulEnterLockLevel      : %x\n", stMadSettings.pReverse32Settings->ulEnterLockLevel);
	printf("\t\t ulRepfVetoLevel       : %x\n", stMadSettings.pReverse32Settings->ulRepfVetoLevel);
	printf("\t\t ulRepfPxlCorrectLevel : %x\n", stMadSettings.pReverse32Settings->ulRepfPxlCorrectLevel);
	printf("\t\t ulLockSatLevel        : %x\n", stMadSettings.pReverse32Settings->ulLockSatLevel);
	printf("\t\t ulBadEditLevel        : %x\n", stMadSettings.pReverse32Settings->ulBadEditLevel);
	printf("\t\t ulBwvLuma32Threshold  : %x\n", stMadSettings.pReverse32Settings->ulBwvLuma32Threshold);

	printf("pReverse22Settings:\n");
	printf("\t\t ulNonMatchMatchRatio  : %x\n", stMadSettings.pReverse22Settings->ulNonMatchMatchRatio);
	printf("\t\t ulEnterLockLevel      : %x\n", stMadSettings.pReverse22Settings->ulEnterLockLevel);
	printf("\t\t ulLockSatLevel        : %x\n", stMadSettings.pReverse22Settings->ulLockSatLevel);
	printf("\t\t ulBwvLuma22Threshold  : %x\n", stMadSettings.pReverse22Settings->ulBwvLuma22Threshold);

done:
	return;
}

void BTST_P_MadAllocFull
( BTST_P_Context *pContext )
{
	pContext->bDeinterlaceAllocFull ^= true;
	printf("toggles bDeinterlaceAllocFull = %d\n", pContext->bDeinterlaceAllocFull);
}

void BTST_P_MadShrinkWidth
( BTST_P_Context *pContext )
{
	pContext->bShrinkWidthForMad ^= true;
	printf("toggles ShrinkWidthForMad = %d\n", pContext->bShrinkWidthForMad);
}

void BTST_P_SetMinDspFmt
	( BTST_P_Context *pContext )
{
	uint32_t      ulFmt;
	printf("\n  Win[%d] min Display Fmt:", pContext->iSelectedWin);
	printf("\n  (%d)1080p (%d)1080i (%d)720p (%d)480p \n",
		(uint32_t)BFMT_VideoFmt_e1080p, 
		(uint32_t)BFMT_VideoFmt_e1080i,
		(uint32_t)BFMT_VideoFmt_e720p,
		(uint32_t)BFMT_VideoFmt_e480p);
	BTST_P_Scanf(pContext, "%d", &ulFmt);

	switch (ulFmt)
	{
		case ((uint32_t) BFMT_VideoFmt_e480p):
			printf("\n min display fmt 480p\n");
			pContext->pMinDspFmt = BFMT_GetVideoFormatInfoPtr(BFMT_VideoFmt_e480p);
			break;
		case ((uint32_t) BFMT_VideoFmt_e720p):
			pContext->pMinDspFmt = BFMT_GetVideoFormatInfoPtr(BFMT_VideoFmt_e720p);
			printf("\n min display fmt 720p\n");
			break;
		case ((uint32_t) BFMT_VideoFmt_e1080i):
			pContext->pMinDspFmt = BFMT_GetVideoFormatInfoPtr(BFMT_VideoFmt_e1080i);
			printf("\n min display fmt 1080i\n");
			break;
		case ((uint32_t) BFMT_VideoFmt_e1080p):
		default:
			pContext->pMinDspFmt = BFMT_GetVideoFormatInfoPtr(BFMT_VideoFmt_e1080p);
			printf("\n min display fmt 1080p\n");
			break;
	}

}

void BTST_P_SetMinSrcFmt
	( BTST_P_Context *pContext )
{
	uint32_t      ulFmt;
	printf("\n  Win[%d] min Source Fmt:", pContext->iSelectedWin);
	printf("\n  (%d)1080p (%d)1080i (%d)720p (%d)480p ",
		(uint32_t)BFMT_VideoFmt_e1080p,
		(uint32_t)BFMT_VideoFmt_e1080i,
		(uint32_t)BFMT_VideoFmt_e720p,
		(uint32_t)BFMT_VideoFmt_e480p);
	BTST_P_Scanf(pContext, "%d", &ulFmt);

	switch (ulFmt)
	{
		case ((uint32_t) BFMT_VideoFmt_e480p):
		{
			pContext->pMinSrcFmt = BFMT_GetVideoFormatInfoPtr(BFMT_VideoFmt_e480p);
			printf("\n min src fmt 480p\n");
			break;
		}
		case ((uint32_t) BFMT_VideoFmt_e720p):
		{
			pContext->pMinSrcFmt = BFMT_GetVideoFormatInfoPtr(BFMT_VideoFmt_e720p);
			printf("\n min src fmt 720p\n");
			break;
		}
		case ((uint32_t) BFMT_VideoFmt_e1080i):
			pContext->pMinSrcFmt = BFMT_GetVideoFormatInfoPtr(BFMT_VideoFmt_e1080i);
			printf("\n min display fmt 1080i\n");
			break;
		case ((uint32_t) BFMT_VideoFmt_e1080p):
		default:
		{
			pContext->pMinSrcFmt = BFMT_GetVideoFormatInfoPtr(BFMT_VideoFmt_e1080p);
			printf("\n min src fmt 1080p\n");
			break;
		}
	}

}

void BTST_P_ToggleByPassVideoProcessing
	( BTST_P_Context *pContext )
{
	pContext->bBypassVideoProcessings ^=1;
	printf("\n video process %s\n",pContext->bBypassVideoProcessings?"off":"on");
}

/***************************************************************************
*
*/
void BTST_P_GetBwParams
( BTST_P_Context          *pContext )
{
	uint32_t            ulDelta;
	BVDC_SclCapBias		eSclCapBias;

	BVDC_Window_Handle  hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetBandwidthEquationParams(hVidWin, &ulDelta, &eSclCapBias);


	printf("Win[%d] Bandwidth equation: delta[%d/1000], ",
		pContext->iSelectedWin, ulDelta);

	if (eSclCapBias == BVDC_SclCapBias_eAuto)
	{
		printf("Auto\n");
	}
	else if(eSclCapBias == BVDC_SclCapBias_eSclBeforeCap)
	{
		printf("SclBeforeCap\n");
	}
	else
	{
		printf("SclAfterCap\n");
	}

done:
	return;
}

/***************************************************************************
*
*/
void BTST_P_SetBwParams
( BTST_P_Context          *pContext )
{
	uint32_t            ulDelta, ulSclBias;
	BVDC_Window_Handle  hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	printf("Please enter new delta for win[%d] bandwidth equation: \n",
		pContext->iSelectedWin);
	BTST_P_Scanf(pContext, "%u", &ulDelta);

	printf("Please enter new scaler/capture bias settings for win[%d] bandwidth equation: auto[0], SclBeforeCap[1], SclAfterCap[2] \n",
		pContext->iSelectedWin);
	BTST_P_Scanf(pContext, "%u", &ulSclBias);


	if(BERR_SUCCESS == BVDC_Window_SetBandwidthEquationParams(hVidWin, ulDelta, (BVDC_SclCapBias)ulSclBias))
	{
		if(pContext->bApplyImmediately)
		{
			BTST_P_ApplyChanges(pContext);
		}
	}

done:
	return;

}


/***************************************************************************
*
*/
void BTST_P_ChgCompression
( BTST_P_Context          *pContext )
{
	uint32_t ulData;
	BVDC_Video_TestFeature1_Settings stCompressionSetting;

	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetTestFeature1(hVidWin, &stCompressionSetting);

	printf("Win[%d] Please enter new setting for Video Compression: \n", pContext->iSelectedWin);
	printf("bEnable[%d]: Enable(1), Disable(0)\n", stCompressionSetting.bEnable);
	BTST_P_Scanf(pContext, "%u", &ulData);
	stCompressionSetting.bEnable = (ulData == 1) ? true : false;
	if(stCompressionSetting.bEnable)
	{
		printf("ulBitsPerPixel [%d]: ", stCompressionSetting.ulBitsPerPixel);
		BTST_P_Scanf(pContext, "%u", &ulData);
		stCompressionSetting.ulBitsPerPixel = ulData;
	}

	if(BERR_SUCCESS == BVDC_Window_SetTestFeature1(hVidWin, &stCompressionSetting))
	{
		if(pContext->bApplyImmediately)
		{
			BTST_P_ApplyChanges(pContext);
		}
	}
done:
	return;
}

/***************************************************************************
 *
 */
void BTST_P_WindowGetCap
	( BTST_P_Context          *pContext )
{
	BVDC_Window_Handle hVidWin =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	BVDC_Window_Capabilities   stCapabilities;

	if(!hVidWin)
	{
		goto done;
	}

	BVDC_Window_GetCapabilities(hVidWin, &stCapabilities);

done:
	return;
}

/* end of file */
