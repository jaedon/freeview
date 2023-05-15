/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: config.h $
 * $brcm_Revision: Hydra_Software_Devel/109 $
 * $brcm_Date: 10/17/12 3:40p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/config.h $
 * 
 * Hydra_Software_Devel/109   10/17/12 3:40p vanessah
 * SW7445-60: add 7445 loadrts function
 * 
 * Hydra_Software_Devel/108   9/26/12 11:47a vanessah
 * SW7425-2501: add buffer configuration default due to nightly build test
 * failure
 * 
 * Hydra_Software_Devel/107   9/25/12 5:36p vanessah
 * SW7425-2501: vdc_test clean up
 * 
 * Hydra_Software_Devel/106   9/12/12 6:06p vanessah
 * SW7425-2501: vdc_test clean up
 * 
 * Hydra_Software_Devel/105   7/9/12 5:18p vanessah
 * SW7425-2501: removal 3548/3556 support
 * 
 * Hydra_Software_Devel/104   6/21/12 1:25p tdo
 * SW7584-5: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7584-5/1   6/19/12 5:17p chengs
 * SW7584-5: Add 7584 support.
 * 
 * Hydra_Software_Devel/103   3/5/12 9:44p vanessah
 * SW7435-9: fix ViCE memory allocation problem
 * 
 * Hydra_Software_Devel/102   2/29/12 4:36p tdo
 * SW7435-9: add support for 7435A0 in VDC
 *
 * Hydra_Software_Devel/101   1/20/12 6:41p vanessah
 * SW7435-9: add STG2/3 support
 *
 * Hydra_Software_Devel/100   1/10/12 5:28p vanessah
 * SW7429-42: removal SW_INIT for GFD version e
 *
 * Hydra_Software_Devel/99   10/12/11 11:01a pntruong
 * SW7429-16: Initial port.
 *
 * Hydra_Software_Devel/98   8/25/11 10:45a tdo
 * SW7420-2030: Add apply changes error tracking capability
 *
 * Hydra_Software_Devel/97   6/10/11 4:00p darnstein
 * SW7231-187: Use a new macro symbol BTST_P_SUPPORT_3D_GFD to indicate
 * that a chipset has a graphics feeder with 3D capability.
 *
 * Hydra_Software_Devel/96   4/27/11 4:15p tdo
 * SW7231-133, SW7422-417: Add support for multiple channel decode within
 * a decoder with streamer input and playback  Add option for users to
 * manually force SVD to only decode AVC, this is needed to do multi
 * channel decode with SVD due to memory allocation issue.  Cleanup old
 * code.
 *
 * Hydra_Software_Devel/95   1/5/11 5:53p darnstein
 * SW7231-16: newer chipsets all do multi-decode.
 *
 * Hydra_Software_Devel/93   12/27/10 3:08p pntruong
 * SW7231-16: Corrected the i2c channel for hdmi output.
 *
 * Hydra_Software_Devel/92   12/21/10 11:08a tdo
 * SW7422-28: Adjust to 3D format size change.  Add test for single MEMC
 * scheme.
 *
 * Hydra_Software_Devel/91   12/16/10 11:09a darnstein
 * SW7231-2: For 7344, use I2C channel 0 for HDMI output.
 *
 * Hydra_Software_Devel/90   12/10/10 2:25p pntruong
 * SW7231-16: Default exclude hsm.
 *
 * Hydra_Software_Devel/89   12/10/10 2:02p hongtaoz
 * SW7425-32: use EMU utility to access between file and device memory;
 * tried to separate xpt muxer PID/rave channels from playback decoder's
 * channels;
 *
 * Hydra_Software_Devel/88   12/3/10 12:24a hongtaoz
 * SW7425-32: add multi-decode test feature to 7420/22/25 etc newer chips;
 *
 * Hydra_Software_Devel/87   11/24/10 12:55p pntruong
 * SW7552-17: Added initial 7552 support.
 *
 * Hydra_Software_Devel/86   11/16/10 5:02p pntruong
 * SW7231-16: Initial support for 7231.
 *
 * Hydra_Software_Devel/85   11/15/10 3:46p hongtaoz
 * SW7425-32: added 3rd MFD test;
 *
 * Hydra_Software_Devel/84   11/11/10 6:44p pntruong
 * SW7425-31: Refactored hdm/lvds/hdr/dvo and added support hdmi input on
 * stb chips.  Removed dead codes.
 *
 * Hydra_Software_Devel/83   11/10/10 11:44a tdo
 * SW7422-13: 7422 bringup
 *
 * Hydra_Software_Devel/82   11/5/10 11:33p hongtaoz
 * SW7425-32: add 7425;
 *
 * Hydra_Software_Devel/81   10/19/10 10:50a pntruong
 * SW7425-16, SW3548-1650: Adapted to the removal of
 * BXVD_ChannelSettings.bFGTEnable.  Also get rid of all reference to fgt
 * in vdctest app.
 *
 * Hydra_Software_Devel/80   10/15/10 2:23p vanessah
 * SW7358-15:  VDC_test compile w.r.t DMA
 *
 * Hydra_Software_Devel/79   10/15/10 11:10a hongtaoz
 * SW7425-33: added the 4th CMP;
 *
 * Hydra_Software_Devel/78   10/13/10 11:38a jessem
 * SW7420-173: Removed BTST_ENABLE_VFD_GFX from being enclosed by
 * DONT_SUPPORT_GRC.
 *
 * Hydra_Software_Devel/77   10/12/10 11:10a jessem
 * SW7420-173: Added support for VFD as source feature.
 *
 * Hydra_Software_Devel/76   9/24/10 2:14p pntruong
 * SW7422-13: Fixed build error for 7408.
 *
 * Hydra_Software_Devel/75   9/23/10 3:36p tdo
 * SW7422-13: add 7422 support to vdc_test
 *
 * Hydra_Software_Devel/74   1/14/10 5:44p yuxiaz
 * SW7405-3290: Enable HSM so vdc_test will run after reboot.
 *
 * Hydra_Software_Devel/73   12/28/09 5:26p syang
 * SW7408-13: correct i2c channel for 7408
 *
 * Hydra_Software_Devel/72   12/15/09 5:48p yuxiaz
 * SW7408-13: Get vdc_test compile for 7408. Clean up compile flags.
 *
 * Hydra_Software_Devel/71   12/14/09 2:10p rpan
 * SW7468-22: Set I2C channel for 7468.
 *
 * Hydra_Software_Devel/70   12/1/09 4:46p rpan
 * SW7468-22: Got vdc_test compiled for 7468.
 *
 * Hydra_Software_Devel/69   11/23/09 3:59p rpan
 * SW7468-22: Initial work for 7468.
 *
 * Hydra_Software_Devel/68   11/19/09 1:38p syang
 * SW7550-70: use I2c channel 2 for 7550
 *
 * Hydra_Software_Devel/67   11/17/09 2:06p syang
 * SW7550-70: turn on HDM out
 *
 * Hydra_Software_Devel/66   10/14/09 4:48p tdo
 * SW7420-376: Bringup appframework for 7410
 *
 * Hydra_Software_Devel/65   9/29/09 4:35p tdo
 * SW7125-27: Correctly handle DAC assignment for new chipsets
 *
 * Hydra_Software_Devel/64   9/1/09 8:04p albertl
 * SW7125-13: Enabled HDMI output for 7125.
 *
 * Hydra_Software_Devel/63   8/31/09 5:56p albertl
 * SW7125-13: Added initial 7125 support.
 *
 * Hydra_Software_Devel/62   8/28/09 11:43a darnstein
 * SW7342-26: previous check-in broke build for 3548.
 *
 * Hydra_Software_Devel/61   8/27/09 7:10p darnstein
 * SW7342-26: DAC assignments to reference board go through
 * BFramework_GetDacAssignments().
 *
 * Hydra_Software_Devel/60   8/14/09 10:27a darnstein
 * PR55219: fix DAC assignments for 7340 and 7342, S-Video.
 *
 * Hydra_Software_Devel/59   8/5/09 4:13p syang
 * PR 55812: update 7550 DAC mapping
 *
 * Hydra_Software_Devel/58   8/5/09 3:05p darnstein
 * PR55225: Fix DAC assignments for 7342.
 *
 * Hydra_Software_Devel/57   8/4/09 2:05p darnstein
 * PR55225: no RFM hardware in 7342 chipset.
 *
 * Hydra_Software_Devel/56   8/4/09 1:01p darnstein
 * PR55225: mechanical updates for 7342 chipset.
 *
 * Hydra_Software_Devel/55   7/31/09 6:48p syang
 * PR 55812:  add 7550 support
 *
 * Hydra_Software_Devel/54   6/22/09 4:19p darnstein
 * PR55219: port to 7340 chipset.
 *
 * Hydra_Software_Devel/53   4/8/09 1:56p tdo
 * PR45785, PR45789: Add ANR support for 7420
 *
 * Hydra_Software_Devel/52   4/7/09 10:06a tdo
 * PR50391: Fix DAC assignment for 7420 S-video
 *
 * Hydra_Software_Devel/51   3/17/09 1:59p darnstein
 * PR53265: Remove BVBI, BVBIlib software. It never worked.
 *
 * Hydra_Software_Devel/50   3/16/09 11:01p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/4   2/23/09 12:18p tdo
 * PR 45785, PR 45789: merge from main branch on 2/23/09
 *
 * Hydra_Software_Devel/49   2/18/09 4:47p pntruong
 * PR52270, PR46356: Fixed dac mapping for 93549.
 *
 * Hydra_Software_Devel/7420_mcvp/3   2/11/09 2:50p darnstein
 * PR46356: Apparently, I got the CVBS DAC wrong for 3548.
 *
 * Hydra_Software_Devel/7420_mcvp/2   2/5/09 11:16a syang
 * PR 45785, PR 45789: merge from main branch on 2/5/09
 *
 * Hydra_Software_Devel/47   2/4/09 1:41p tdo
 * PR51627: add vdc_test support for 7336
 *
 * Hydra_Software_Devel/7420_mcvp/1   1/24/09 11:53a syang
 * PR 45785, PR 45789: add PI support for new the new module
 * MCVP(MCTF+MCDI)
 *
 * Hydra_Software_Devel/46   1/21/09 5:05p darnstein
 * PR46356: Use only one DAC for 93549 and 93556 reference boards.
 *
 * Hydra_Software_Devel/45   1/20/09 9:17a rpan
 * PR50391: Bring up HDMI output.
 *
 * Hydra_Software_Devel/44   12/24/08 2:01p tdo
 * PR50391: Bringup dual decode for 7420
 *
 * Hydra_Software_Devel/43   12/22/08 12:11p tdo
 * PR50391: Adjust component DAC setting to match the connector on board.
 *
 * Hydra_Software_Devel/42   12/19/08 9:54p tdo
 * PR50391: Define correct number of XVD for 7420
 *
 * Hydra_Software_Devel/41   12/17/08 7:48p tdo
 * PR50391: Temporarily remove MAD support for 7420 until MCVP is added
 *
 * Hydra_Software_Devel/40   12/16/08 7:11p tdo
 * PR50391: Adjust DAC settings for 7420
 *
 * Hydra_Software_Devel/39   12/15/08 10:02p tdo
 * PR50391: Basic VDC bringup
 *
 * Hydra_Software_Devel/38   12/3/08 10:28p tdo
 * PR46484: Add p3d support for 7420
 *
 * Hydra_Software_Devel/37   12/2/08 5:42p tdo
 * PR46484: Bringup vdc_test
 *
 * Hydra_Software_Devel/36   10/22/08 10:50p nissen
 * PR 46310: Added GRC and P3D tests.
 *
 * Hydra_Software_Devel/35   8/21/08 10:02p pntruong
 * PR35322: Put back mrc for 7405.
 *
 * Hydra_Software_Devel/34   7/31/08 5:33p pntruong
 * PR36876: Re-enabled mrc for 3548/3556.
 *
 * Hydra_Software_Devel/33   7/1/08 5:16p rpan
 * PR36501: Initial work for adding tuner support fror 93549.
 *
 * Hydra_Software_Devel/32   6/20/08 10:18a rpan
 * PR43712: Changed SCART-Component DAC configuration.
 *
 * Hydra_Software_Devel/31   6/19/08 2:36p rpan
 * PR43712: Add SCART-SVideo test case.
 *
 * Hydra_Software_Devel/30   6/19/08 12:25p rpan
 * PR43712: Add SCART test cases.
 *
 * Hydra_Software_Devel/29   6/17/08 2:29p rpan
 * PR43668: Adopt the new LVD PI for 3548/3556.
 *
 * Hydra_Software_Devel/28   6/11/08 11:49p tdo
 * PR39331: Bringup dynamic contrast for 3548/3556
 *
 * Hydra_Software_Devel/27   6/11/08 9:27p pntruong
 * PR39321: Reenabled hdr compilations.
 *
 * Hydra_Software_Devel/26   6/7/08 11:40a rpan
 * PR39159: Added ADC-DAC pass through test case.
 *
 * Hydra_Software_Devel/25   6/6/08 11:24a rpan
 * PR42109: Added tests for all SECAM variants.
 *
 * Hydra_Software_Devel/24   6/5/08 9:52p pntruong
 * PR39321: Reenabled hdr compilations.
 *
 * Hydra_Software_Devel/23   6/4/08 9:21a pntruong
 * PR41488: Need to include main/pip/gfx windows (3wins).
 *
 * Hydra_Software_Devel/22   5/5/08 12:04p rpan
 * PR39807: Enabled DVPO support for 3548/3556.
 *
 * Hydra_Software_Devel/21   4/8/08 10:54a tdo
 * PR41488:  Add 3556 build support
 *
 * Hydra_Software_Devel/20   3/21/08 2:25p hongtaoz
 * PR39154: added MAD and ANR support for 3548;
 *
 * Hydra_Software_Devel/19   3/10/08 8:33p darnstein
 * PR25971: first try at DCS for 7400
 *
 * Hydra_Software_Devel/18   3/10/08 4:07p yuxiaz
 * PR39807: Static buffer bring up on 3548.
 *
 * Hydra_Software_Devel/17   2/21/08 3:49p rpan
 * PR39807: Work for bcm3548 bringup.
 *
 * Hydra_Software_Devel/16   12/5/07 11:22a tdo
 * PR36898: Add VDC PI support for 7335
 *
 * Hydra_Software_Devel/15   11/16/07 9:42a yuxiaz
 * PR34540: 7325 bringup: enable MAD.
 *
 * Hydra_Software_Devel/14   11/10/07 2:50p yuxiaz
 * PR34540: 7325 bringup.
 *
 * Hydra_Software_Devel/13   11/9/07 11:58a yuxiaz
 * PR34540: 7325 bringup.
 *
 * Hydra_Software_Devel/12   10/31/07 3:29p yuxiaz
 * PR29569: Added FGT memory in BXVD_Open.
 *
 * Hydra_Software_Devel/11   10/29/07 2:58p yuxiaz
 * PR34540: Add support for 7325.
 *
 * Hydra_Software_Devel/10   10/16/07 5:23p rpan
 * PR34857: Turn off the 7400 Multi_Decode Support.
 *
 * Hydra_Software_Devel/9   10/16/07 2:45p rpan
 * PR34857: First version after merging the latest xvd_vdc_test and
 * mvd_vdc_test as of Oct. 16, 2007, 12pm.
 *
 *
 * Hydra_Software_Devel/1   9/21/07 1:54p rpan
 * PR34857: Initial revision.
 *
 ***************************************************************************/

#ifndef VDC_TEST_CONFIG_H__
#define VDC_TEST_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bavc.h"
#include "bvdc.h"
#include "framework_board_bvdc.h"

#if MACROVISION_SUPPORT
#include "bvdc_macrovision.h"
#endif

#if DCS_SUPPORT
#include "bvdc_dcs.h"
#endif

#include "bvn_client.h"
#include "buffer.h"

#define MAX_XVD_CORES        (2)
#define BTST_P_MEMC_MAX      (3)
#define MAX_ViCE_CORES       (2)


#if (BFramework_NUM_XVD_CORES == 0)
#error No MPEG decoder!
#endif

#if (BFramework_NUM_XVD_CORES > MAX_XVD_CORES)
#error Invalid number of MPEG decoder cores!
#endif

/* XPT playback, PID channel, rave channel ID maps for vdc_test */
#define BTST_XVD_PB_CHANNEL_ID_BASE     0
#define BTST_XVD_PID_CHANNEL_ID_BASE    0
#define BTST_XVD_RAVE_CHANNEL_ID_BASE   0
#define BTST_MUXER_PB_CHANNEL_ID_BASE   MAX_XVD_CORES

/* newer chips all support mosaic decode */

/* One decode goes to multiple MFD */
#define BTST_P_SUPPORT_MULTI_DECODE                    (1)
#define BTST_P_MAX_CHANNELS                            (16)


#define BVDC_Dac_Unused 0

/* maximam value among all the available platforms */
#define BTST_P_MAX_MFD                               (BAVC_SourceId_eMpegMax - BAVC_SourceId_eMpeg0 + 1)
#define BTST_P_MAX_CMP                               (BVDC_CompositorId_eCompositorMax - BVDC_CompositorId_eCompositor0 + 1)
#define BTST_P_MAX_WINDOW                            (3)


#if (BCHP_CHIP == 7400)
/* Usage:
 * DRAM-1: decoder 0 + MFD_0.
 * DRAM-2: decoder 1 + MFD_1.
 * CPU can only access DRAM-0 on 7400 A0.
 * CPU can access DRAM-0, DRAM-1, DRAM-2 on 7400 B0.
 *
 * By default, MFD_0 and MFD_1 are blocked out on DRAM-0.
 * Static buffers use memory on DRAM-0. Need to adjust
 * RTS for MFD_0 and MFD_1 to make static buffer test working.
 */
#define BTST_P_HACK_MFD_RTS       (1)
#else
#define BTST_P_HACK_MFD_RTS       (0)
#endif


/* ViCE definition */
#if ((BCHP_CHIP==7425)||(BCHP_CHIP==7435)||(BCHP_CHIP==7445))
#define BTST_P_SUPPORT_VICE                          (1)
#else
#define BTST_P_SUPPORT_VICE                          (0)
#endif

/* RTS 7435 RTS clients in  different controller */
#if ((BCHP_CHIP==7435)||(BCHP_CHIP==7445))
#define BTST_P_MULTI_RTS                             (1)
#else
#define BTST_P_MULTI_RTS                             (0)
#endif

/* Miscellances constants. */
#define BTST_P_CMD_LENGHT                   (1024)
#define BTST_P_INFITITE_WAIT                (0xffffffff)
#define BTST_P_DEFAUL_GFX_WIDTH             (200)
#define BTST_P_DEFAUL_GFX_HEIGHT            (200)
#define BTST_P_BG_COLOR_RANGE_MAX           (0xff)


/* Random velocity for movements. */
#define BTST_P_RANDOM_VELOCITY              ((rand() % 5) + 1)

/* Mirror mode channel change destroy cloned first. */
#define BTST_P_DESTROY_CLONED_FIRST         (1)

/* Use custom aspect ratio correct _isr functions for rects. */
#define BTST_P_USE_CUSTOM_ASPECT_RATIO      (0)

#ifdef VXWORKS
#define USE_INTMGR 1
#endif



#ifdef __cplusplus
}
#endif

#endif /* VDC_TEST_CONFIG_H__ */

/* end of file */
