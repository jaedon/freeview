/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
*
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
*  Except as expressly set forth in the Authorized License,
*
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
*  USE OR PERFORMANCE OF THE SOFTWARE.
*
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
*  ANY LIMITED REMEDY.
*
* $brcm_Workfile: nexus_platform_config.c $
* $brcm_Revision: 154 $
* $brcm_Date: 10/10/12 10:32a $
*
* API Description:
*   API name: Platform
*    Specific APIs to configure already initialized board.
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/common/src/nexus_platform_config.c $
* 
* 154   10/10/12 10:32a erickson
* SW7408-346: support graphics-only build
* 
* 153   10/2/12 3:59p erickson
* SW7435-364: still run platform timer if no video decoder
* 
* 152   9/25/12 9:47a erickson
* SW7435-364: monitor for runaway L1 interrupts
* 
* 151   9/10/12 6:16p randyjew
* SW7563-2:Add 7563 support.
* 
* 150   9/4/12 11:05a katrep
* SW7445-1:add 7445
* 
* 149   8/30/12 11:54a chengs
* SW7584-34: Add GisbMasters and video dac config for 7584.
* 
* 148   8/24/12 4:17p vishk
* SW7358-345: Merge changes to mainline. Change the channel indexing to
*  include spi core type.
* 
* 147   7/24/12 2:28p jrubio
* SW7346-848: merge in Vish Changes into mainline
* 
* SW7346-848/1   7/23/12 3:25p vishk
* SW7346-848: Add support for BCM3472 BCM97346I2SFF platform.
* 
* 146   7/20/12 12:22p erickson
* SW7420-1177: fix HDMI DVO warning
* 
* 145   7/3/12 3:10p vishk
* SW7429-183: Add support for 7241DCSFBTSFF board
* 
* 144   6/8/12 6:14p chengs
* SW7584-34: Add 7584 suppprt.
* 
* 143   5/23/12 2:29p bselva
* SW7360-19: Add support for 7360 platform
* 
* 142   5/21/12 1:50p hongtaoz
* SW7435-193: updated GISB master list for 7435;
* 
* 141   5/8/12 3:46p jgarrett
* SWDEPRECATED-3762: Fixing race condition if I2C is closed while HDMI is
*  still accessing EDID
* 
* 140   5/7/12 5:58p vishk
* SW3462-12: Enable DVB-C2 video demo by Jun 12 on BCM97241C2SFF board
* 
* 139   4/26/12 3:33p rgreen
* SW7346-793:  Add code showing the modifiction of the HDMI SPD InfoFrame
* 
* 138   3/27/12 4:20p erickson
* SW7425-2607: allow binary compatible platform api's
* 
* 137   3/12/12 4:41p gkamosa
* SW7425-2543: Merge support for 3383 SMS platform
* 
* Centaurus_Phase_1.0/2   3/6/12 6:03p gkamosa
* SW7425-2337: Merge branch to tip
* 
* 136   2/28/12 5:15p katrep
* SW7429-1:updated dacs for 7241
* 
* 135   2/26/12 11:35p hongtaoz
* SW7231-68: wrap spi code with its macro; fix spi memory leak for
*  shtdown;
* 
* 134   2/24/12 6:14p vishk
* SW7231-68: add SPI support on top of I2C for 3461 interface
* 
* 133   2/22/12 6:56p vle
* SW7425-2279: Fix compile warning and issue when re-open NEXUS CEC
*  module
* 
* 132   2/21/12 2:19p erickson
* SW7425-2130: fix debug warnings
* 
* 131   1/12/12 12:32p vishk
* SW7552-147: 7552B0: Add Nexus OOB support Bug fix.
* 
* 130   1/11/12 5:06p vishk
* SW7552-147: 7552B0: Add Nexus OOB support
* 
* 129   1/9/12 3:25p vishk
* SWNOOS-507: Improve i2c performance. Re-Enabling 4-byte I2C transfer
*  mode.
* 
* 128   1/5/12 3:19p vishk
* SWNOOS-507: Improve i2c performance. Reverting back since there is
*  runtime errors while running 4 byte transfer mode.
* 
* 127   1/3/12 2:27p vishk
* SWNOOS-507: Improve i2c performance.
* 
* Centaurus_Phase_1.0/1   2/6/12 6:18p jmiddelk
* SW7425-2337: Adding support for 93383sms
* 
* 126   12/20/11 6:02p katrep
* SW7429-1:SW i2c for hdmi
* 
* 126   12/20/11 5:15p katrep
* SW7429-1:enable sw i2c for hdmi
* 
* 125   12/20/11 2:44p vishk
* SW7552-148: 7552B0: Add 3128 support over MTSIF for 7552B0 SVB board
* 
* 124   12/15/11 7:45p rgreen
* SW7425-1140: Use NEXUS_HAS_CEC instead of NEXUS_NUM_CEC to
*  enable/disable CEC support
* 
* 123   12/15/11 4:26p vsilyaev
* SW7425-1140: Merge NEXUS_Cec module support into main-line
* 
* SW7425-1140/1   12/9/11 5:11p vle
* SW7425-1140: add NEXUS Cec module
*
* 122   12/1/11 4:13p mphillip
* SW7425-1304: Change 4528 to HAB
* 
* 121   11/29/11 5:40p mward
* SW7429-1: SW7435-7:  Update video DAC assignments for 7429 and 7435.
* 
* 120   11/11/11 5:19p erickson
* SW7425-1747: remove old code
* 
* 119   11/11/11 10:55a erickson
* SW7231-317: fix NEXUS_BASE_ONLY, clean up warnings, simplify
*  NEXUS_PlatformConfiguration if no inputs/outputs are defined
* 
* 118   11/3/11 12:04p vsilyaev
* SW7405-4477: Use BDBG_MODULE_LOG for AVD debug output
* 
* 117   11/3/11 11:51a vsilyaev
* SW7405-4477: Use BDBG_P_PrintString for debug output
* 
* 116   11/1/11 11:17a mward
* SW7435-7:  Add 7435.
* 
* 115   10/13/11 11:01p hongtaoz
* SW7425-1349: added audio dummy outputs;
* 
* 114   9/26/11 6:32p randyjew
* SW7344-140: Adding 3123 support for 7418SFF_H platform
* 
* 113   9/22/11 5:16p katrep
* SW7429-1:add support for 7429 & bondouts
* 
* 112   9/21/11 5:00p randyjew
* SW7418-39: Add External RFM support for 7418SFF_H
* 
* 111   9/13/11 7:00p jessem
* SW7425-1248: Updated pinmux and I2C usage for B0.
* 
* 110   9/7/11 5:14p katrep
* SW7231-355:with the latest kernel no longer need to ignore gisb timer
* 
* 109   9/7/11 3:48p ialauder
* SWDTV-8404: Added dac mute calls for gpio_2 and gpio_3 to unmute the
*  DACs on the SV 935233B0 boards.
* 
* 108   8/29/11 5:36p jgarrett
* SW7425-724: Adding external RFM control on 97425SV board
* 
* 107   8/29/11 5:29p jgarrett
* SW7425-724: Adding external RFM control on 97425SV board
* 
* 106   8/29/11 10:55a erickson
* SW7231-355: fix implementation of stubbed out
*  NEXUS_Platform_P_ConfigureGisbTimeout
* 
* 105   8/19/11 2:26p ialauder
* SWDTV-8296: Added code to open/close multichannel i2s.
* 
* 104   8/17/11 3:44p shyi
* SWDTV-8201: Added platform configuration for 935233B0 SV board
* 
* 103   8/3/11 10:59a agin
* SWNOOS-476:  For no-os and ucos diags, specifically for 97346 HR44,
*  need to initialize the moca i2c channel.
* 
* 102   6/16/11 3:36p akam
* SWDTV-6928: Corrected discrepancy found after merging to mainline.
* 
* 101   6/8/11 12:24p carson
* SWDTV-7460: Enable STA559 on 233 Card platform
* 
* 100   6/3/11 11:17a xhuang
* SW7552-34: update gisb table for 7358/7552
* 
* 99   6/1/11 4:39p shyi
* SWDTV-7356: Added 935126 support in NEXUS platform config
* 
* 98   5/17/11 6:12p jhaberf
* SWDTV-6298: resolve build issue for 7400
* 
* SWDTV-6928/1   5/4/11 6:25p mstefan
* SWDTV-6928: merge to main
* 
* 96   3/29/11 3:36p randyjew
* SW7231-33: enbable Gisb timer for 7418, moved to 2631-3.3 kernel
* 
* 95   3/11/11 3:19p katrep
* SW7231-66:Bumped up i2c clock for 7231 to 400khz
* 
* 94   1/31/11 1:49p hongtaoz
* SW7231-33: removed compile warning;
* 
* 93   1/29/11 2:42p mward
* SW7231-33: Use Sun Top Prod Id only if that register exists.
* 
* 92   1/28/11 3:28p randyjew
* SW7231-33:Diabled GISB timer for 97418 platforms. Using Sun Top Prod Id
* 
* 91   1/21/11 4:00p katrep
* SW7231-33:Diabled GISB timer for 97230 and 97418 platforms
* 
* 90   1/5/11 7:33p agin
* SW7344-20: Fixed bugs with previous version.
* 
* 89   1/5/11 6:54p agin
* SW7344-20: For 7344/7346, do not initialize the I2C channel reserved
*  for MOCA that has already been initialized and used by LINUX.
* 
* 88   1/3/11 2:22p jgarrett
* SW7550-485: Merge to main branch
* 
* 87   12/27/10 4:26p hongtaoz
* SW7425-9: add 7425 support;
* 
* 86   12/23/10 3:06p jrubio
* SW7344-9: change 7344 hdmi i2c channel to 0
* 
* 85   12/22/10 4:07p katrep
* SW7231-26:add support for new heap configuration, add support for 7230
* 
* SW7550-485/3   12/17/10 5:54p vle
* SW7550-485: Merge to latest. Ready for mainline check in.
* 
* 84   12/17/10 11:03a xhuang
* SW7358-21: Add 97358/97552 support
* 
* 83   12/16/10 6:05p katrep
* SW7231-4:7231 bringup
* 
* 82   12/15/10 7:31p jrubio
* SW7344-9: correct 7344 component settings
* 
* 81   12/13/10 10:35p jrubio
* SW7344-9: add 7344/7346 table
* 
* 80   12/9/10 4:36p spothana
* SW7422-96: Increase the i2c clock to 400Khz for front end tuners.
* 
* 79   12/9/10 10:45a jgarrett
* SW7231-4: Merge to main branch
* 
* 78   12/7/10 11:44a jrubio
* SW7344-9: add 7344/7346
* 
* 77   12/7/10 11:43a jrubio
* SW7344-9: add 7344/7346
* 
* 76   12/2/10 7:41p spothana
* SW7420-1177: Adding nexus hdmi dvo module.
* 
* SW7420-1177/1   11/1/10 8:38p spothana
* SW7420-1177: Adding nexus hdmi dvo module.
* 
* 7231_bringup/1   11/29/10 6:44p katrep
* SW7231-4:97231 uses i2c 0 for hdmi
* 
* 75   11/16/10 4:51p vsilyaev
* SW7422-64: Fixed tier-down for 656 outputs
* 
* 74   11/11/10 2:03p vle
* SW7422-12: Fix incorrect HDMI_OUTPUT_I2C_CHANNEL assignment
* 
* 73   10/15/10 7:46a agin
* SWNOOS-428:  Do not handle no-os uniquely with respect to clock
*  polarity.
* 
* 72   10/14/10 11:57a jgarrett
* SW7550-485: Merge to main branch
* 
* 71   10/5/10 2:03p jrubio
* SW7342-248: fix S-Video Dac issue with 7340/7342
* 
* 70   9/29/10 12:26p nickh
* SW7422-10: Add correct table mapping for GISB masters
* 
* 69   9/24/10 2:51p nickh
* SW7422-10: Add 7422 config settings for DAC and HDMI I2C
* 
* 68   9/14/10 6:46p hongtaoz
* SW7425-9: adding 7425 support;
* 
* 67   8/21/10 2:41p gskerl
* SW7125-237: Don't try to configure input band if transport module is
*  not running (for 7125 and 7468)
* 
* 66   8/6/10 6:07p randyjew
* SW7420-883: Modify 7208 BCHP_CHIP to 7468
* 
* 65   8/5/10 7:40p ssood
* SW7420-883: modify 7402 defines from PLATFORM to BCHP_CHIP (to allow
*  93380vms builds to work)
* 
* 64   7/30/10 12:18p erickson
* SW3548-2887: add common NEXUS_Platform_P_ConfigureGisbTimeout()
*
* 63   7/29/10 1:53p mward
* SW7125-4: Y and Pb DACs swapped for 97019.
*
* 62   7/26/10 11:46a mward
* SW7468-282:  New DAC assignment for S-Video for 7125.
*
* 61   7/16/10 2:25p spothana
* SW7420-897 : Correcting S-Video DAC assignment.
*
* 60   7/15/10 11:43a randyjew
* SW7468-279, SW7468-282: Revert back to 7468 Svideo Dac assignment to
*  match VDC PI.
*
* 59   7/13/10 3:15p randyjew
* SW7468-279: Swapping 97468 Svideo Dac for Luma and Chroma. Opposite
*  from bcm97468ipmbv10.pdf schematic.
*
* SW7550-485/2   12/7/10 4:18p vle
* SW7550-485: Fix issue with BREG_I2C_ReadNoAddr for HDMI. I2C channel
*  for HDMI should not be in burst mode. Otherwise, it'll break
*  BREG_I2C_ReadNoAddr which leads to incorrect HDCP Ri' values being
*  read.
* 
* SW7550-485/1   6/30/10 3:49p farshidf
* SW7550-485: enable the I2C burst mode for 7550 chip to be in sync with
*  BBS I2C mode reading
* 
* 58   6/11/10 1:04p randyjew
* SW7468-247: Add Svideo support for 7468 B0 boards
* 
* 57   4/26/10 4:17p mward
* SW7125-378:  Use NEXUS_InputBandSettings.clockActiveHigh=true for
*  DekTec LVDS streamer.
* 
* 56   3/19/10 10:09a erickson
* SW7405-3832: switch to NEXUS_PLATFORM macro
*
* 55   3/9/10 2:19p ismailk
* SW7420-618: I2C should run at 400MHz for the frontend initialization
*  for 93380sms
*
* 54   3/8/10 11:09a mward
* SW7400-2708: Default i2cSettings.softI2c based on
*  NEXUS_HAS_HDMI_SOFTI2C_SUPPORT.  Environment setting
* hdmi_i2c_software_mode=y or hdmi_i2c_software_mode=n can
* be used to override the default.
*
* 53   3/3/10 11:09a mward
* SW7405-3016:  7400E0 has fixed I2C, doesn't need softI2c.
*
* 52   1/21/10 5:54p garetht
* SW7420-553: Add platform for 93380vms
*
* 51   12/24/09 2:40p gmohile
* SW7408-1 : Set up 7408 video dacs
*
* 50   12/14/09 12:14p mphillip
* SW7550-112: Merge 7550 changes to main branch
*
* sw7550_112/1   12/13/09 4:40a nitinb
* sw7550_112: 7550 uses I2C channel 0 for HDMI
*
* 49   12/10/09 8:24p mphillip
* SW7550-112: Merge 7550 changes to main branch
*
* Refsw_7550/4   11/26/09 8:02a nitinb
* SW7550-29: Restricting sync detect disable only to IB0
*
* Refsw_7550/3   11/26/09 5:27a nitinb
* SW7550-29: Enabling sync detect on all xpt input bands
*
* Refsw_7550/2   11/17/09 5:37a nitinb
* SW7550: Corrected the video DAC configuration for 7550
*
* Refsw_7550/1   9/18/09 10:33a nitinb
* SW7550-29: Getting initial nexus build for 7550
*
* 48   12/7/09 12:08p randyjew
* SW7468-6:Use Platform 97208 and removed Bchp_chip 7208
*
* 47   12/1/09 4:18p randyjew
* SW7468-6: Add 7208 and 7468 support
*
* 46   11/12/09 3:37p gmohile
* SW7408-1 : Update 7408 support
*
* 45   11/12/09 2:29p gmohile
* SW7405-3395 : Fix warning
*
* 44   11/12/09 2:26p gmohile
* SW7405-3395 : Fix use of NEXUS_FPGA_SUPPORT
*
* 43   10/26/09 9:31a erickson
* SW7405-3016: rework HDMI I2C channel logic, added softI2c option
*
* 42   10/14/09 2:56p jrubio
* SW7342-50: make svideo dac for 7340 as well
*
* 41   10/14/09 2:50p jrubio
* SW7342-50: Fix S Video Dac configuration for 7342
*
* 40   10/7/09 1:52p lwhite
* SW7468-6: Add 7468 support
*
* 39   9/30/09 11:11a mward
* SW7125-4:  Condition with BCHP_CHIP==7125 to protect against non-
*  integer PLATFORMs.
*
* 38   9/29/09 6:16p mward
* SW7125-4:  Set correct input band clock polarity for 7125 streamer.
*
* 37   9/2/09 10:49a mward
* SW7125-4: Removed #error debug message.
*
* 36   9/1/09 11:19p pntruong
* SW7125-4: Fixed svideo dacs for 97125.
*
* 35   9/1/09 3:49p mward
* SW7125-4: 97125 DAC assignment was correct as it was.
*
* 34   8/31/09 7:27p mward
* SW7125-4: DACs are different for 97019 and 97125.
*
* 33   8/31/09 12:03p mward
* SW7125-4: 7125 FPGA is on BSC_M2.
*
* 32   8/13/09 5:18p jrubio
* PR55882: move 7340 i2c HDMI chan to 3
*
* 31   8/12/09 10:11p jrubio
* PR55232: add 7342 fixes
*
* 30   8/10/09 11:57a jrubio
* PR55232: adjust dacs for 7340/7342
*
* 29   8/5/09 4:57p jrubio
* PR55232: add 7342/7340 support
*
* 28   7/31/09 9:30a gmohile
* PR 56512 : Merge 7401/03 support to mainline
*
* 27   7/24/09 12:58p mward
* PR 55545: Set HDMI I2C for 7125.
*
* 26   7/14/09 4:08p agin
* PR56698: For 3255 rmagnum, set clock active high for mpeg output
*  interface.
*
* 25   5/12/09 4:18p erickson
* PR52109: move g_NEXUS_platformModule for more general use
*
* 24   4/27/09 5:22p jrubio
* PR40169: add 7335/7336/7325 into main line config
*
* 23   4/13/09 6:05p mward
* PR53623: Removed extra {.  Get connector for proper call to
*  NEXUS_VideoOutput_Shutdown().
*
* 22   4/10/09 1:02p shyam
* PR53323: wrap ifdef around code
*
* 21   4/7/09 11:42a erickson
* PR53623: change #define to NEXUS_NUM_656_OUTPUTS
*
* 20   4/2/09 6:43p nickh
* PR48963: Fix 7420 svideo DAC settings
*
* 20   4/2/09 6:42p nickh
* PR48963: Fix 7420 svideo DAC settings
*
* 19   4/1/09 12:15p erickson
* PR53623: merge 656 output support
*
* PR53623/1   3/31/09 12:44p mward
* PR 53623: 656 output support.
*
* 18   3/26/09 1:11p erickson
* PR53384: added avd sw uart
*
* 17   3/25/09 4:22p jtna
* PR51960: fix memory leak from not shutting down RFM audio output
*
* 16   3/19/09 4:59p erickson
* PR53323: add more #if so that more modules can be optionally removed
*
* 15   3/5/09 3:18p katrep
* PR52599: More support for 7205
*
* 14   1/26/09 1:54p erickson
* PR51468: global variable naming convention
*
* 13   1/26/09 12:36p erickson
* PR51468: global variable naming convention
*
* 12   12/22/08 1:00p nickh
* PR48963: FPGA init not required for 7420
*
* 11   11/25/08 11:34a mward
* PR47739: fix naming for second composite output.
*
* 10   11/19/08 1:46p nickh
* PR48963: Add 7420 support
*
* 9   5/12/08 4:08p erickson
* PR42628: add NEXUS_Rfm_Close
*
* 8   4/18/08 6:43p jgarrett
* PR 41623: Fixing hdmi shutdown
*
* 7   4/1/08 7:00p jgarrett
* PR 41191: Shutting down connectors on close
*
* 5   2/22/08 6:26p shyam
* PR39435 : Add remux capability to Nexus
*
* 4   2/20/08 1:45p erickson
* PR39405: added RFM
*
* 3   2/1/08 5:34p jgarrett
* PR 39017: Adding HdmiOutput
*
* 3   2/1/08 5:25p jgarrett
* PR 39017: Adding HdmiOutput
*
* 2   2/1/08 5:00p vsilyaev
* PR 38682: Fixed prototype for functions without argument
*
* 1   1/18/08 2:17p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/11   12/6/07 2:47p jgarrett
* PR 37971: Adding I2sOutput
*
* Nexus_Devel/10   12/4/07 12:02p erickson
* PR36802: close outputs on shutdown
*
* Nexus_Devel/9   11/19/07 1:20p erickson
* PR34925: split demux.h into separate interface files
*
* Nexus_Devel/8   11/15/07 10:39a erickson
* PR34925: use NEXUS_InputBand_GetSettings'
*
* Nexus_Devel/7   11/8/07 9:28a erickson
* PR36802: added NEXUS_Platform_P_Shutdown
*
* Nexus_Devel/6   11/6/07 4:23p jgarrett
* PR 34954: Migrating to audio connector model
*
* Nexus_Devel/5   11/5/07 4:16p erickson
* PR36725: merge 97405 platform_config.c
*
* Nexus_Devel/4   11/5/07 2:12p erickson
* PR36725: add NEXUS_PlatformConfiguration, share more code with 97401
*
* Nexus_Devel/3   10/10/07 11:29a jgarrett
* PR 35551: Adding on-board 3517
*
* Nexus_Devel/2   9/28/07 4:25p jgarrett
* PR 35002: Adding input band configuration for 7400
*
* Nexus_Devel/1   9/26/07 5:30p vsilyaev
* PR 34662:Added platform configuration
*
***************************************************************************/

#include "bchp_sun_top_ctrl.h"
#include "nexus_base.h"
#include "nexus_platform_priv.h"

#if NEXUS_HAS_DISPLAY
#include "nexus_display.h"
#include "nexus_component_output.h"
#include "nexus_composite_output.h"
#include "nexus_svideo_output.h"
#include "nexus_video_output.h"
#endif

#if NEXUS_NUM_RFM_OUTPUTS
#include "nexus_rfm.h"
#endif
#if NEXUS_HAS_TRANSPORT
#include "nexus_pid_channel.h"
#include "nexus_input_band.h"
#endif
#ifdef NEXUS_FPGA_SUPPORT
#include "nexus_platform_fpga.h"
#endif
#if NEXUS_HAS_AUDIO
#include "nexus_audio_output.h"
#endif
#if NEXUS_HAS_GPIO
#include "nexus_gpio.h"
#endif
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
#include "humax_i2c_bus_index.h"
#endif

BDBG_MODULE(nexus_platform_config);
#ifdef NEXUS_FPGA_SUPPORT
static bool g_fpgaInit = false;
#endif
static void NEXUS_Platform_P_StartMonitor(void);
static void NEXUS_Platform_P_StopMonitor(void);

#if (((BCHP_CHIP == 7325) && (BCHP_VER <= BCHP_VER_A1))  \
 ||  ((BCHP_CHIP == 7325) && (BCHP_VER == BCHP_VER_B0))  \
 ||  ((BCHP_CHIP == 7335) && (BCHP_VER == BCHP_VER_A0))  \
 ||  ((BCHP_CHIP == 7335) && (BCHP_VER == BCHP_VER_B0))  \
 ||  ((BCHP_CHIP == 7400) && (BCHP_VER < BCHP_VER_E0))  \
 ||  ((BCHP_CHIP == 7405) && (BCHP_VER <= BCHP_VER_B0))  \
 ||  ((BCHP_CHIP == 7443) && (BCHP_VER == BCHP_VER_A0)) \
 ||  ((BCHP_CHIP == 7429) && (BCHP_VER == BCHP_VER_A0)))
#define NEXUS_HAS_HDMI_SOFTI2C_SUPPORT 1
#endif

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
#define NEXUS_HDMI_OUTPUT_I2C_CHANNEL I2C_CHANNEL_HDMI
#else
#ifndef NEXUS_HDMI_OUTPUT_I2C_CHANNEL
#if BCHP_CHIP == 7405
#define NEXUS_HDMI_OUTPUT_I2C_CHANNEL 1
#elif (BCHP_CHIP == 7400) || (BCHP_CHIP == 7420) || (BCHP_CHIP==7335) || (BCHP_CHIP==7336) || (BCHP_CHIP==7125) || (BCHP_CHIP==7342) || (BCHP_CHIP==7408) || (BCHP_CHIP==7468) || (BCHP_CHIP==7550) || (BCHP_CHIP==7231) || (BCHP_CHIP==7346) || (BCHP_CHIP==7358) || (BCHP_CHIP==7552) \
|| (BCHP_CHIP==7344)|| (BCHP_CHIP==7360)|| (BCHP_CHIP==7563)
#define NEXUS_HDMI_OUTPUT_I2C_CHANNEL 0
#elif (BCHP_CHIP == 7325) ||  (BCHP_CHIP==7340)
#define NEXUS_HDMI_OUTPUT_I2C_CHANNEL 3
#elif (BCHP_CHIP==7422) || (BCHP_CHIP==7425) || (BCHP_CHIP==7435) || (BCHP_CHIP==7584) || (BCHP_CHIP==7445)
#define NEXUS_HDMI_OUTPUT_I2C_CHANNEL NEXUS_I2C_CHANNEL_HDMI_TX
#elif (BCHP_CHIP==7429) 
#define NEXUS_HDMI_OUTPUT_I2C_CHANNEL 2
#else
#error UNSUPPORTED CHIP
#endif
#endif
#endif

#if NEXUS_USE_7425_SV_BOARD || NEXUS_PLATFORM_7418SFF_H
#define NEXUS_HAS_EXTERNAL_RFM       (true)
#endif

#if NEXUS_HAS_EXTERNAL_RFM
extern NEXUS_Error NEXUS_Platform_P_InitExternalRfm(const NEXUS_PlatformConfiguration *pConfig);
#endif

static void NEXUS_Platform_P_StartMonitor(void);
static void NEXUS_Platform_P_StopMonitor(void);

NEXUS_Error NEXUS_Platform_P_Config(const NEXUS_PlatformSettings *pSettings)
{
    NEXUS_Error errCode;
    int i;
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;

#if NEXUS_BASE_ONLY
    /* remove unused warning */
    if (0) {goto error;} 
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(pConfig);
    BSTD_UNUSED(i);
#endif

#if SMS93383_SUPPORT
    /* remove unused warning */
    if (0) {goto error;} 
    BSTD_UNUSED(pConfig);
    BSTD_UNUSED(i);
#endif

#if NEXUS_NUM_COMPOSITE_INPUTS
    if (NULL == pCvbsAdcMap)
    {
        BDBG_ERR(("Porting required for CVBS input mapping, board type %d, "
            "chip ID %d", boardInfo.boardType, boardInfo.chipId));
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto error;
    }
#endif 

#if NEXUS_NUM_I2C_CHANNELS
    BDBG_CASSERT(NEXUS_NUM_I2C_CHANNELS <= NEXUS_MAX_CONFIG_HANDLES);
    /* Open I2C Channels */
    if (pSettings->openI2c)
    {
        for ( i = 0; i < NEXUS_NUM_I2C_CHANNELS; i++ )
        {
            NEXUS_I2cSettings i2cSettings;
            NEXUS_I2c_GetDefaultSettings(&i2cSettings);
            /* For now, run all I2C interfaces at low speed */
            i2cSettings.clockRate = NEXUS_I2cClockRate_e100Khz;

            /* Because of 7344  A0 issues we need to disable the I2C channel being used for Moca as well as channel  3 */
#if (BCHP_CHIP==7344)
#if NEXUS_PLATFORM_7418SFF_H
            i2cSettings.clockRate = NEXUS_I2cClockRate_e400Khz; 
            i2cSettings.softI2c = false;
            i2cSettings.interruptMode = false;
            if (i==3) {
                continue;
            }
#else
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
#else
            if ((i==3) || (i==NEXUS_MOCA_I2C_CHANNEL)) {
                continue;
            }
#endif
#endif
#elif (BCHP_CHIP==7346)
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
#else
    #if (!(defined(NO_OS_DIAGS) || defined(UCOS_DIAGS)) && defined(HR44_BOARD)) /* For HR44, MoCA and the 4505 share the same i2c bus, and no-os and ucos diags need to initialize the i2c bus */
            if (i==NEXUS_MOCA_I2C_CHANNEL) {
                continue;
            }
    #endif
#endif
#endif
            
            if ( i == NEXUS_HDMI_OUTPUT_I2C_CHANNEL) {
                const char *hdmi_i2c_software_mode = NEXUS_GetEnv("hdmi_i2c_software_mode");
                #if NEXUS_HAS_HDMI_SOFTI2C_SUPPORT
                bool forceHardI2c = (hdmi_i2c_software_mode && (hdmi_i2c_software_mode[0] == 'n' ||
                                                                hdmi_i2c_software_mode[0] == 'N'));
                i2cSettings.softI2c = !forceHardI2c;
                #else
                bool forceSoftI2c = (hdmi_i2c_software_mode && (hdmi_i2c_software_mode[0] == 'y' ||
                                                                hdmi_i2c_software_mode[0] == 'Y'));
                i2cSettings.softI2c = forceSoftI2c;
                #endif
                BDBG_WRN(("softI2c = %s",i2cSettings.softI2c? "true":"false"));
            }
            else {  
#if BCHP_CHIP == 7550
                /* For 7550 this is required to avoid a conflict with BBS reading some external registers */
                i2cSettings.burstMode = true;
#endif
#if NEXUS_PLATFORM_7425_CABLE
                i2cSettings.fourByteXferMode = true;
#endif
#if VMS93380_SUPPORT || SMS93380_SUPPORT || NEXUS_PLATFORM_7422_CABLE || NEXUS_PLATFORM_7425_CABLE || NEXUS_PLATFORM_4528_DBS || (NEXUS_PLATFORM == 97231) || (NEXUS_PLATFORM == 97230) || (NEXUS_PLATFORM == 97552) || NEXUS_PLATFORM_97346_I2SFF
                /* i2c driving the front end tuners should run at 400KHz */
                i2cSettings.clockRate = NEXUS_I2cClockRate_e400Khz;
#endif
            }
#if NEXUS_PLATFORM_7241_C2SFF || NEXUS_PLATFORM_7241_DCSFBTSFF
            i2cSettings.clockRate = NEXUS_I2cClockRate_e400Khz; 
            i2cSettings.interruptMode = false;
#endif          

/* TODO_ILEE defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FIX_MODIFY_TBD)*/
#if (BCHP_CHIP == 7429) && (NEXUS_FRONTEND_3461)
			if ((i == 0)||(i == 4))/* BSC_M0 & BSC_M4 are I2C bus for channel with BCM7241 */
			{
				i2cSettings.softI2c = false;/*true;*/
				i2cSettings.clockRate = NEXUS_I2cClockRate_e400Khz;
				/* i2cSettings.interruptMode = false; */
			}
#endif

#if (BCHP_CHIP == 7231) && (NEXUS_FRONTEND_3461)
			if ((i == 2)||(i == 3))/* BSC_M2 & BSC_M3 are I2C bus for channel with BCM7231 */
			{
				i2cSettings.softI2c = false;/*true;*/
				i2cSettings.clockRate = NEXUS_I2cClockRate_e400Khz;
				/* i2cSettings.interruptMode = false; */
			}
#endif

#if (BCHP_CHIP == 7435) && (NEXUS_FRONTEND_3128)
                    if (i == 4)/* BSC_M2 & BSC_M3 are I2C bus for channel with BCM7231 */
                    {
                        BDBG_ERR(("BCHP_CHIP = 7435, NEXUS_FRONTEND_3128 "));
                        i2cSettings.softI2c = false;/*true;*/
                        i2cSettings.clockRate = NEXUS_I2cClockRate_e400Khz;
                        /* i2cSettings.interruptMode = false; */
                    }
#endif


            pConfig->i2c[i] = NEXUS_I2c_Open(i, &i2cSettings);
            if(NULL == pConfig->i2c[i])
                BDBG_ERR(("pConfig->i2c[%d] is Null ", i));
            
            BDBG_ASSERT(NULL != pConfig->i2c[i]);
        }
    }
#endif


#if NEXUS_FPGA_SUPPORT
    if ( pSettings->openFpga )
    {
        if ( !pSettings->openI2c )
        {
            BDBG_ERR(("Cannot open FPGA without opening I2C"));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }

        BDBG_MSG(("FPGA"));
#if BCHP_CHIP == 7125
        errCode = NEXUS_Fpga_Init(pConfig->i2c[2]);
#elif BCHP_CHIP == 7468
        errCode = NEXUS_Fpga_Init(pConfig->i2c[1]);
#else
        errCode = NEXUS_Fpga_Init(pConfig->i2c[4]);
#endif
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto error;
        }
        g_fpgaInit = true;
    }
#endif


#if NEXUS_HAS_TRANSPORT
    /* The 97400 has no valid signal on TS inputs */
    for ( i = 0; i<NEXUS_NUM_INPUT_BANDS; i++ )
    {
        NEXUS_InputBandSettings inputBandSettings;

        #if (BCHP_CHIP == 7125 || BCHP_CHIP == 7468)
            if (NULL == g_NEXUS_platformHandles.transport)
                break;  /* Don't bother with any of this if transport module isn't running */
        #endif  /* if (BCHP_CHIP == 7125 || BCHP_CHIP == 7468) */

        NEXUS_InputBand_GetSettings(i, &inputBandSettings);
        inputBandSettings.validEnabled = false;
#if (BCHP_CHIP == 7550)
        if (0==i)
        {
            inputBandSettings.useInternalSync = false; /* IB0 needs this setting to get data from THD.*/
        }
#endif
#if (BCHP_CHIP == 7125)
#if (NEXUS_PLATFORM == 97125) || (NEXUS_PLATFORM == 97025)
        if (i == NEXUS_InputBand_e1)
        {   /* Correct polarity for streamer */
            inputBandSettings.clockActiveHigh= true;
        }
#endif
#endif
        NEXUS_InputBand_SetSettings(i, &inputBandSettings);
    }
#endif 


#if NEXUS_HAS_DISPLAY
    if (pSettings->openOutputs)
    {
#if NEXUS_NUM_COMPOSITE_OUTPUTS
        BDBG_CASSERT(NEXUS_NUM_COMPOSITE_OUTPUTS <= NEXUS_MAX_CONFIG_HANDLES);
        {
            NEXUS_CompositeOutputSettings compositeCfg;
            pConfig->outputs.composite[0] = NEXUS_CompositeOutput_Open(0, NULL);
            if(!pConfig->outputs.composite[0]) {errCode = BERR_TRACE(BERR_NOT_SUPPORTED); goto error; }
            NEXUS_CompositeOutput_GetSettings(pConfig->outputs.composite[0], &compositeCfg);
    #if BCHP_CHIP == 7400 || BCHP_CHIP == 7420
            compositeCfg.dac = NEXUS_VideoDac_e4;
    #elif  BCHP_CHIP == 7422 || BCHP_CHIP == 7425 || BCHP_CHIP == 7435 || BCHP_CHIP == 7445
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
            compositeCfg.dac = NEXUS_VideoDac_e0;
#else
            compositeCfg.dac = NEXUS_VideoDac_e3;
#endif
    #elif  BCHP_CHIP == 7468
    #if (NEXUS_PLATFORM == 97208)
            compositeCfg.dac = NEXUS_VideoDac_e3;
    #else
            compositeCfg.dac = NEXUS_VideoDac_e0;
    #endif
    #elif BCHP_CHIP == 7125 || BCHP_CHIP == 7550 || BCHP_CHIP == 7408 || BCHP_CHIP==7346 || BCHP_CHIP==7344 || BCHP_CHIP==7231 || BCHP_CHIP == 7358 || BCHP_CHIP == 7552 || BCHP_CHIP == 7360 || BCHP_CHIP == 7584
            compositeCfg.dac = NEXUS_VideoDac_e0;
    #elif BCHP_CHIP == 7429
            #if PLATFORM == 97241
            compositeCfg.dac = NEXUS_VideoDac_e0;
            #else
            compositeCfg.dac = NEXUS_VideoDac_e3;
            #endif
    #else
            compositeCfg.dac = NEXUS_VideoDac_e2;
    #endif


            errCode = NEXUS_CompositeOutput_SetSettings(pConfig->outputs.composite[0], &compositeCfg);
            if(errCode) {errCode = BERR_TRACE(BERR_NOT_SUPPORTED); goto error; }
    #if NEXUS_NUM_COMPOSITE_OUTPUTS >= 2
            pConfig->outputs.composite[1] = NEXUS_CompositeOutput_Open(1, NULL);
            if(!pConfig->outputs.composite[1]) {errCode = BERR_TRACE(BERR_NOT_SUPPORTED); goto error; }
            NEXUS_CompositeOutput_GetSettings(pConfig->outputs.composite[1], &compositeCfg);
            compositeCfg.dac = NEXUS_VideoDac_e3;
            errCode = NEXUS_CompositeOutput_SetSettings(pConfig->outputs.composite[1], &compositeCfg);
            if(errCode) {errCode = BERR_TRACE(BERR_NOT_SUPPORTED); goto error; }
    #endif
        }
#endif

#if NEXUS_NUM_RFM_OUTPUTS
        {
            pConfig->outputs.rfm[0] = NEXUS_Rfm_Open(0, NULL);
            if(!pConfig->outputs.rfm[0]) {errCode = BERR_TRACE(BERR_NOT_SUPPORTED); goto error; }
        }
#endif

#if NEXUS_NUM_COMPONENT_OUTPUTS
    BDBG_CASSERT(NEXUS_NUM_COMPONENT_OUTPUTS <= NEXUS_MAX_CONFIG_HANDLES);
        {
            NEXUS_ComponentOutputSettings componentCfg;
            pConfig->outputs.component[0] = NEXUS_ComponentOutput_Open(0, NULL);
            if(!pConfig->outputs.component[0]) { errCode = BERR_TRACE(BERR_NOT_SUPPORTED); goto error; }
            NEXUS_ComponentOutput_GetSettings(pConfig->outputs.component[0], &componentCfg);
            componentCfg.type = NEXUS_ComponentOutputType_eYPrPb;
    #if BCHP_CHIP == 7400 || BCHP_CHIP == 7420 || BCHP_CHIP == 7422 || BCHP_CHIP == 7425 || BCHP_CHIP == 7435 || BCHP_CHIP == 7445
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
            componentCfg.dacs.YPrPb.dacY = NEXUS_VideoDac_e2;
            componentCfg.dacs.YPrPb.dacPr = NEXUS_VideoDac_e3;
            componentCfg.dacs.YPrPb.dacPb = NEXUS_VideoDac_e1;  
#else
            componentCfg.dacs.YPrPb.dacY = NEXUS_VideoDac_e0;
            componentCfg.dacs.YPrPb.dacPr = NEXUS_VideoDac_e2;
            componentCfg.dacs.YPrPb.dacPb = NEXUS_VideoDac_e1;
#endif
    #elif (BCHP_CHIP == 7468)  /* only on 97208. 97208 uses 7468 BCHP_CHIP*/
            componentCfg.dacs.YPrPb.dacY = NEXUS_VideoDac_e1;
            componentCfg.dacs.YPrPb.dacPr = NEXUS_VideoDac_e2;
            componentCfg.dacs.YPrPb.dacPb = NEXUS_VideoDac_e0;
    #elif BCHP_CHIP == 7125
        #if (NEXUS_PLATFORM == 97019)
            componentCfg.dacs.YPrPb.dacY = NEXUS_VideoDac_e5;
            componentCfg.dacs.YPrPb.dacPr = NEXUS_VideoDac_e3;
            componentCfg.dacs.YPrPb.dacPb = NEXUS_VideoDac_e4;
        #else
            componentCfg.dacs.YPrPb.dacY = NEXUS_VideoDac_e4;
            componentCfg.dacs.YPrPb.dacPr = NEXUS_VideoDac_e5;
            componentCfg.dacs.YPrPb.dacPb = NEXUS_VideoDac_e3;
        #endif
    #elif BCHP_CHIP == 7550  || BCHP_CHIP==7346 || BCHP_CHIP==7231 || BCHP_CHIP==7584
            componentCfg.dacs.YPrPb.dacY = NEXUS_VideoDac_e2;
            componentCfg.dacs.YPrPb.dacPr = NEXUS_VideoDac_e3;
            componentCfg.dacs.YPrPb.dacPb = NEXUS_VideoDac_e1;  
    #elif  BCHP_CHIP==7344 
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
            componentCfg.dacs.YPrPb.dacY = NEXUS_VideoDac_e2;
            componentCfg.dacs.YPrPb.dacPr = NEXUS_VideoDac_e3;
            componentCfg.dacs.YPrPb.dacPb = NEXUS_VideoDac_e1;  
#else
            componentCfg.dacs.YPrPb.dacY = NEXUS_VideoDac_e1;
            componentCfg.dacs.YPrPb.dacPr = NEXUS_VideoDac_e3;
            componentCfg.dacs.YPrPb.dacPb = NEXUS_VideoDac_e2;  
#endif
    #elif BCHP_CHIP == 7408
            componentCfg.dacs.YPrPb.dacY = NEXUS_VideoDac_e1;
            componentCfg.dacs.YPrPb.dacPr = NEXUS_VideoDac_e2;
            componentCfg.dacs.YPrPb.dacPb = NEXUS_VideoDac_e3;
    #elif (BCHP_CHIP == 7358) || (BCHP_CHIP == 7552) || (BCHP_CHIP == 7360)
            componentCfg.dacs.YPrPb.dacY = NEXUS_VideoDac_e2;
            componentCfg.dacs.YPrPb.dacPr = NEXUS_VideoDac_e3;
            componentCfg.dacs.YPrPb.dacPb = NEXUS_VideoDac_e1;
    #elif  (BCHP_CHIP == 7429)
        #if (NEXUS_PLATFORM == 97241)
            componentCfg.dacs.YPrPb.dacY = NEXUS_VideoDac_e2;
            componentCfg.dacs.YPrPb.dacPr = NEXUS_VideoDac_e3;
            componentCfg.dacs.YPrPb.dacPb = NEXUS_VideoDac_e1;  
        #else
            componentCfg.dacs.YPrPb.dacY = NEXUS_VideoDac_e0;
            componentCfg.dacs.YPrPb.dacPr = NEXUS_VideoDac_e2;
            componentCfg.dacs.YPrPb.dacPb = NEXUS_VideoDac_e1;
        #endif
    #else
            componentCfg.dacs.YPrPb.dacY = NEXUS_VideoDac_e4;
            componentCfg.dacs.YPrPb.dacPr = NEXUS_VideoDac_e5;
            componentCfg.dacs.YPrPb.dacPb = NEXUS_VideoDac_e3;
    #endif
            errCode = NEXUS_ComponentOutput_SetSettings(pConfig->outputs.component[0], &componentCfg);
            if(errCode) {errCode = BERR_TRACE(BERR_NOT_SUPPORTED); goto error; }
        }
#endif

#if NEXUS_NUM_SVIDEO_OUTPUTS
        BDBG_CASSERT(NEXUS_NUM_SVIDEO_OUTPUTS <= NEXUS_MAX_CONFIG_HANDLES);
        {
            NEXUS_SvideoOutputSettings svideoCfg;
            pConfig->outputs.svideo[0] = NEXUS_SvideoOutput_Open(0, NULL);
            if(!pConfig->outputs.svideo[0]) {errCode = BERR_TRACE(BERR_NOT_SUPPORTED); goto error; }
            NEXUS_SvideoOutput_GetSettings(pConfig->outputs.svideo[0], &svideoCfg);
    #if BCHP_CHIP == 7400
            svideoCfg.dacY = NEXUS_VideoDac_e6;
            svideoCfg.dacC = NEXUS_VideoDac_e5;
    #elif BCHP_CHIP == 7420
            svideoCfg.dacY = NEXUS_VideoDac_e6;
            svideoCfg.dacC = NEXUS_VideoDac_e5;
    #elif BCHP_CHIP == 7125
            svideoCfg.dacY = NEXUS_VideoDac_e1;
            svideoCfg.dacC = NEXUS_VideoDac_e2;
    #elif BCHP_CHIP == 7342 || BCHP_CHIP==7340 
            svideoCfg.dacY = NEXUS_VideoDac_e0;
            svideoCfg.dacC = NEXUS_VideoDac_e1;
    #elif (BCHP_CHIP == 7468)  /*Svideo only on 7468 */
            svideoCfg.dacY = NEXUS_VideoDac_e1;
            svideoCfg.dacC = NEXUS_VideoDac_e2;
    #else
            svideoCfg.dacY = NEXUS_VideoDac_e0;
            svideoCfg.dacC = NEXUS_VideoDac_e1;
    #endif
            errCode = NEXUS_SvideoOutput_SetSettings(pConfig->outputs.svideo[0], &svideoCfg);
            if(errCode) {errCode = BERR_TRACE(BERR_NOT_SUPPORTED); goto error; }
        }
#endif

#if NEXUS_NUM_656_OUTPUTS
        BDBG_CASSERT(NEXUS_NUM_656_OUTPUTS <= NEXUS_MAX_CONFIG_HANDLES);
        for ( i = 0; i < NEXUS_NUM_656_OUTPUTS; i++ )
        {
            pConfig->outputs.ccir656[i] = NEXUS_Ccir656Output_Open(i, NULL);
            if(!pConfig->outputs.ccir656[i]) {errCode = BERR_TRACE(BERR_NOT_SUPPORTED); goto error; }
        }
#endif

#if NEXUS_NUM_AUDIO_DACS
        for ( i = 0; i < NEXUS_NUM_AUDIO_DACS; i++ )
        {
            pConfig->outputs.audioDacs[i] = NEXUS_AudioDac_Open(i, NULL);
            if ( NULL == pConfig->outputs.audioDacs[i] )
            {
                errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
                goto error;
            }
        }
#endif

#if NEXUS_NUM_SPDIF_OUTPUTS
        for ( i = 0; i < NEXUS_NUM_SPDIF_OUTPUTS; i++ )
        {
            pConfig->outputs.spdif[i] = NEXUS_SpdifOutput_Open(i, NULL);
            if ( NULL == pConfig->outputs.spdif[i] )
            {
                errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
                goto error;
            }
        }
#endif

#if NEXUS_NUM_AUDIO_DUMMY_OUTPUTS
        for ( i = 0; i < NEXUS_NUM_AUDIO_DUMMY_OUTPUTS; i++ )
        {
            pConfig->outputs.audioDummy[i] = NEXUS_AudioDummyOutput_Open(i, NULL);
            if ( NULL == pConfig->outputs.audioDummy[i] )
            {
                errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
                goto error;
            }
        }
#endif

#if NEXUS_NUM_I2S_OUTPUTS
        for ( i = 0; i < NEXUS_NUM_I2S_OUTPUTS; i++ )
        {
            pConfig->outputs.i2s[i] = NEXUS_I2sOutput_Open(i, NULL);
            if ( NULL == pConfig->outputs.i2s[i] )
            {
                errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
                goto error;
            }
        }
#if NEXUS_NUM_I2S_MULTI_OUTPUTS
        for ( i = 0; i < NEXUS_NUM_I2S_MULTI_OUTPUTS; i++ )
        {
            pConfig->outputs.i2sMulti[i] = NEXUS_I2sMultiOutput_Open(i, NULL);
            if ( NULL == pConfig->outputs.i2sMulti[i] )
            {
                errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
                goto error;
            }
        }
#endif
#endif

#if NEXUS_NUM_HDMI_OUTPUTS
        if ( pSettings->openI2c )
        {
            NEXUS_HdmiOutputOpenSettings hdmiSettings;
            NEXUS_HdmiOutput_GetDefaultOpenSettings(&hdmiSettings);

                hdmiSettings.i2c = pConfig->i2c[NEXUS_HDMI_OUTPUT_I2C_CHANNEL];

                /* SPD Infoframe settings */
                hdmiSettings.spd.deviceType = NEXUS_HdmiSpdSourceDeviceType_eDigitalStb ;
                BKNI_Memcpy(&hdmiSettings.spd.vendorName, "Broadcom", BAVC_HDMI_SPD_IF_VENDOR_LEN) ;
                BKNI_Memcpy(hdmiSettings.spd.description, "STB Refsw Design", BAVC_HDMI_SPD_IF_DESC_LEN) ;

            pConfig->outputs.hdmi[0] = NEXUS_HdmiOutput_Open(0, &hdmiSettings);
            if ( NULL == pConfig->outputs.hdmi[0] )
            {
                errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
                goto error;
            }
        }
        else
        {
            BDBG_WRN(("Cannot open HDMI without opening I2C"));
        }
#endif
#if NEXUS_NUM_HDMI_DVO
        {
            NEXUS_HdmiDvoSettings hdmiDvoSettings;
            NEXUS_HdmiDvo_GetDefaultSettings(&hdmiDvoSettings);
            pConfig->outputs.hdmiDvo[0] = NEXUS_HdmiDvo_Open(0, &hdmiDvoSettings);
            if ( NULL == pConfig->outputs.hdmiDvo[0] ) 
            {
                errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
                goto error;
            }
        }
#endif
#if NEXUS_HAS_CEC
        {
            NEXUS_CecSettings cecSettings;
            NEXUS_Cec_GetDefaultSettings(&cecSettings);
            pConfig->outputs.cec[0] = NEXUS_Cec_Open(0, &cecSettings);
            if (NULL == pConfig->outputs.cec[0])
            {
                errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
                goto error;
            }   
        }
#endif

#if NEXUS_NUM_SCART_OUTPUTS
            for ( i = 0; i < NEXUS_NUM_SCART_OUTPUTS; i++ )
            {
                BDBG_MSG(("NEXUS_Platform_P_Config: Outputs:SCART[%u]", i));
                pConfig->outputs.scart[i] = NEXUS_ScartOutput_Open(i, NULL);
                if ( NULL == pConfig->outputs.scart[i] )
                {
                    errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
                    goto error;
                }
            }
#endif
        }
#endif /* NEXUS_HAS_DISPLAY */

#if NEXUS_HAS_EXTERNAL_RFM
    errCode = NEXUS_Platform_P_InitExternalRfm(pConfig);
    if(errCode != NEXUS_SUCCESS) { BERR_TRACE(errCode); goto error; }
#endif

    NEXUS_Platform_P_StartMonitor();

    BDBG_MSG(("NEXUS_Platform_P_Config<< DONE"));
    return BERR_SUCCESS;

error:
    NEXUS_Platform_P_Shutdown();
    return errCode;
}

void NEXUS_Platform_P_Shutdown(void)
{
    unsigned i;
    NEXUS_PlatformConfiguration *pConfig = &g_NEXUS_platformHandles.config;

#if NEXUS_BASE_ONLY || SMS93383_SUPPORT
    /* remove unused warning */
    BSTD_UNUSED(pConfig);
    BSTD_UNUSED(i);
#endif

    NEXUS_Platform_P_StopMonitor();

#if NEXUS_NUM_I2C_CHANNELS
#ifdef NEXUS_FPGA_SUPPORT
    if (g_fpgaInit) {
        NEXUS_Fpga_Uninit();
        g_fpgaInit = false;
    }
#endif
#endif

#if NEXUS_HAS_DISPLAY
#if NEXUS_NUM_I2S_OUTPUTS && NEXUS_HAS_AUDIO
    for ( i = 0; i < NEXUS_NUM_I2S_OUTPUTS; i++ )
    {
        if ( NULL != pConfig->outputs.i2s[i] )
        {
            NEXUS_AudioOutput_Shutdown(NEXUS_I2sOutput_GetConnector(pConfig->outputs.i2s[i]));
            NEXUS_I2sOutput_Close(pConfig->outputs.i2s[i]);
            pConfig->outputs.i2s[i] = NULL;            
        }
    }

#if NEXUS_NUM_I2S_MULTI_OUTPUTS && NEXUS_HAS_AUDIO
        for ( i = 0; i < NEXUS_NUM_I2S_MULTI_OUTPUTS; i++ )
        {
            if ( NULL != pConfig->outputs.i2sMulti[i] )
            {
                NEXUS_AudioOutput_Shutdown(NEXUS_I2sMultiOutput_GetConnector(pConfig->outputs.i2sMulti[i]));
                NEXUS_I2sMultiOutput_Close(pConfig->outputs.i2sMulti[i]);
                pConfig->outputs.i2sMulti[i] = NULL;            
            }
        }
#endif

#endif
#if NEXUS_NUM_SPDIF_OUTPUTS && NEXUS_HAS_AUDIO
    for ( i = 0; i < NEXUS_NUM_SPDIF_OUTPUTS; i++ ) {
        if (pConfig->outputs.spdif[i]) {
            NEXUS_AudioOutput_Shutdown(NEXUS_SpdifOutput_GetConnector(pConfig->outputs.spdif[i]));
            NEXUS_SpdifOutput_Close(pConfig->outputs.spdif[i]);
            pConfig->outputs.spdif[i] = NULL;
        }
    }
#endif
#if NEXUS_NUM_AUDIO_DUMMY_OUTPUTS && NEXUS_HAS_AUDIO
    for ( i = 0; i < NEXUS_NUM_AUDIO_DUMMY_OUTPUTS; i++ ) {
        if (pConfig->outputs.audioDummy[i]) {
            NEXUS_AudioOutput_Shutdown(NEXUS_AudioDummyOutput_GetConnector(pConfig->outputs.audioDummy[i]));
            NEXUS_AudioDummyOutput_Close(pConfig->outputs.audioDummy[i]);
            pConfig->outputs.audioDummy[i] = NULL;
        }
    }
#endif
#if NEXUS_NUM_AUDIO_DACS && NEXUS_HAS_AUDIO
    for ( i = 0; i < NEXUS_NUM_AUDIO_DACS; i++ ) {
        if (pConfig->outputs.audioDacs[i]) {
            NEXUS_AudioOutput_Shutdown(NEXUS_AudioDac_GetConnector(pConfig->outputs.audioDacs[i]));
            NEXUS_AudioDac_Close(pConfig->outputs.audioDacs[i]);
            pConfig->outputs.audioDacs[i] = NULL;
        }
    }
#endif
#if NEXUS_NUM_SVIDEO_OUTPUTS
    for ( i = 0; i < NEXUS_NUM_SVIDEO_OUTPUTS; i++ ) {
        if (pConfig->outputs.svideo[i]) {
            NEXUS_VideoOutput_Shutdown(NEXUS_SvideoOutput_GetConnector(pConfig->outputs.svideo[i]));
            NEXUS_SvideoOutput_Close(pConfig->outputs.svideo[i]);
        }
    }
#endif
#if NEXUS_NUM_656_OUTPUTS
    for ( i = 0; i < NEXUS_NUM_656_OUTPUTS; i++ ) {
        if (pConfig->outputs.ccir656[i]) {
            NEXUS_VideoOutput_Shutdown(NEXUS_Ccir656Output_GetConnector(pConfig->outputs.ccir656[i]));
            NEXUS_Ccir656Output_Close(pConfig->outputs.ccir656[i]);
            pConfig->outputs.ccir656[i] = NULL;            
        }
    }
#endif
#if NEXUS_NUM_COMPONENT_OUTPUTS
    for ( i = 0; i < NEXUS_NUM_COMPONENT_OUTPUTS; i++ ) {
        if (pConfig->outputs.component[i]) {
            NEXUS_VideoOutput_Shutdown(NEXUS_ComponentOutput_GetConnector(pConfig->outputs.component[i]));
            NEXUS_ComponentOutput_Close(pConfig->outputs.component[i]);
            pConfig->outputs.component[i] = NULL;            
        }
    }
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    for ( i = 0; i < NEXUS_NUM_COMPOSITE_OUTPUTS; i++ ) {
        if (pConfig->outputs.composite[i]) {
            NEXUS_VideoOutput_Shutdown(NEXUS_CompositeOutput_GetConnector(pConfig->outputs.composite[i]));
            NEXUS_CompositeOutput_Close(pConfig->outputs.composite[i]);
            pConfig->outputs.composite[i] = NULL;            
        }
    }
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    for ( i = 0; i < NEXUS_NUM_HDMI_OUTPUTS; i++ ) {
        if (pConfig->outputs.hdmi[i]) {
            #if NEXUS_HAS_VIDEO_DECODER
            NEXUS_VideoOutput_Shutdown(NEXUS_HdmiOutput_GetVideoConnector(pConfig->outputs.hdmi[i]));
            #endif
            #if NEXUS_HAS_AUDIO
            NEXUS_AudioOutput_Shutdown(NEXUS_HdmiOutput_GetAudioConnector(pConfig->outputs.hdmi[i]));
            #endif
            NEXUS_HdmiOutput_Close(pConfig->outputs.hdmi[i]);
            pConfig->outputs.hdmi[i] = NULL;            
        }
    }
#endif
#if NEXUS_NUM_RFM_OUTPUTS
    if (pConfig->outputs.rfm[0]) {
        #if NEXUS_HAS_AUDIO
        NEXUS_AudioOutput_Shutdown(NEXUS_Rfm_GetAudioConnector(pConfig->outputs.rfm[0]));
        #endif
        NEXUS_Rfm_Close(pConfig->outputs.rfm[0]);
        pConfig->outputs.rfm[0] = NULL;   
    }
#endif
#endif /* NEXUS_HAS_DISPLAY */

#if NEXUS_HAS_CEC
        if (pConfig->outputs.cec[0])
        {
            NEXUS_Cec_Close(pConfig->outputs.cec[0]);
            pConfig->outputs.cec[0] = NULL;
        }
#endif

#if NEXUS_NUM_SCART_OUTPUTS
        for ( i = 0; i < NEXUS_NUM_SCART_OUTPUTS; i++ )
        {
            if ( NULL != pConfig->outputs.scart[i] )
            {
                NEXUS_VideoOutput_Shutdown(NEXUS_ScartOutput_GetConnector(pConfig->outputs.scart[i]));
                NEXUS_ScartOutput_Close(pConfig->outputs.scart[i]);
                pConfig->outputs.scart[i] = NULL;
            }
        }
#endif

#if NEXUS_NUM_COMPONENT_INPUTS
        for ( i = 0; i < NEXUS_NUM_COMPONENT_INPUTS; i++ )
        {
            if ( NULL != pConfig->inputs.component[i] )
            {
                NEXUS_VideoInput_Shutdown(NEXUS_ComponentInput_GetConnector(pConfig->inputs.component[i]));
                NEXUS_ComponentInput_Close(pConfig->inputs.component[i]);
                pConfig->inputs.component[i] = NULL;
            }
#if NEXUS_HAS_AUDIO
            if ( NULL != pConfig->inputs.componentAudio[i] )
            {
                NEXUS_AnalogAudioInput_Close(pConfig->inputs.componentAudio[i]);
                pConfig->inputs.componentAudio[i] = NULL;
            }
#endif
        }
#endif

#if NEXUS_NUM_COMPOSITE_INPUTS
        for ( i = 0; i < NEXUS_NUM_COMPOSITE_INPUTS; i++ )
        {
            if ( NULL != pConfig->inputs.composite[i] )
            {
                NEXUS_VideoInput_Shutdown(NEXUS_CompositeInput_GetConnector(pConfig->inputs.composite[i]));
                NEXUS_CompositeInput_Close(pConfig->inputs.composite[i]);
                pConfig->inputs.composite[i] = NULL;
            }
#if NEXUS_HAS_AUDIO
            if ( NULL != pConfig->inputs.compositeAudio[i] )
            {
                NEXUS_AnalogAudioInput_Close(pConfig->inputs.compositeAudio[i]);
                pConfig->inputs.compositeAudio[i] = NULL;
            }
#endif
        }
#endif

#if NEXUS_NUM_SVIDEO_INPUTS
        for ( i = 0; i < NEXUS_NUM_SVIDEO_INPUTS; i++ )
        {
            if ( NULL != pConfig->inputs.svideo[i] )
            {
                NEXUS_VideoInput_Shutdown(NEXUS_SvideoInput_GetConnector(pConfig->inputs.svideo[i]));
                NEXUS_SvideoInput_Close(pConfig->inputs.svideo[i]);
                pConfig->inputs.svideo[i] = NULL;
            }
#if NEXUS_HAS_AUDIO
            if ( NULL != pConfig->inputs.svideoAudio[i] )
            {
                NEXUS_AnalogAudioInput_Close(pConfig->inputs.svideoAudio[i]);
                pConfig->inputs.svideoAudio[i] = NULL;
            }
#endif
        }
#endif

#if NEXUS_NUM_PC_INPUTS
        for ( i = 0; i < NEXUS_NUM_PC_INPUTS; i++ )
        {
            if ( NULL != pConfig->inputs.pc[i] )
            {
                NEXUS_VideoInput_Shutdown(NEXUS_PcInput_GetConnector(pConfig->inputs.pc[i]));
                NEXUS_PcInput_Close(pConfig->inputs.pc[i]);
                pConfig->inputs.pc[i] = NULL;
            }
            
#if NEXUS_HAS_AUDIO
            if ( NULL != pConfig->inputs.pcAudio[i]  )
            {
                NEXUS_AnalogAudioInput_Close(pConfig->inputs.pcAudio[i] );
                pConfig->inputs.pcAudio[i]  = NULL;
            }
#endif          
        }
#endif

#if NEXUS_NUM_SCART_INPUTS
        for ( i = 0; i < NEXUS_NUM_SCART_INPUTS; i++ )
        {
            if ( NULL != pConfig->inputs.scart[i] )
            {
                NEXUS_VideoInput_Shutdown(NEXUS_ScartInput_GetVideoConnector(pConfig->inputs.scart[i]));
                NEXUS_ScartInput_Close(pConfig->inputs.scart[i]);
                pConfig->inputs.scart[i] = NULL;
            }
#if NEXUS_HAS_AUDIO
            if ( NULL != pConfig->inputs.scartAudio[i] )
            {
                NEXUS_AnalogAudioInput_Close(pConfig->inputs.scartAudio[i]);
                pConfig->inputs.scartAudio[i] = NULL;
            }
#endif
        }
#endif

#if NEXUS_NUM_I2C_CHANNELS
    for ( i = 0; i < NEXUS_NUM_I2C_CHANNELS; i++ ) {
        if (pConfig->i2c[i]) {
            NEXUS_I2c_Close(pConfig->i2c[i]);
            pConfig->i2c[i] = NULL;            
        }
    }
#endif
}

static NEXUS_TimerHandle g_platformTimer;

#define NEXUS_MODULE_NAME platform

#if NEXUS_HAS_VIDEO_DECODER
BDBG_FILE_MODULE(AVD);
#include "nexus_video_decoder_extra.h"
#endif

static void NEXUS_Platform_P_Timer(void *context)
{
    BSTD_UNUSED(context);

#if NEXUS_HAS_VIDEO_DECODER
    if (NEXUS_GetEnv("avd_monitor")) {
        unsigned avdCore = NEXUS_atoi(NEXUS_GetEnv("avd_monitor"));
        char buf[256];
        unsigned n;
        static bool init = false;

        if (!init) {
            if (!NEXUS_VideoDecoderModule_SetDebugLog(avdCore, true)) {
                init = true;
            }
        }
        if (init) {
            while (!NEXUS_VideoDecoderModule_ReadDebugLog(avdCore, buf, 255 /* one less */, &n) && n) {
                char *cur = buf;
                buf[n] = 0;

                /* print header for each line. makes for easier grep of logs. */
                while (*cur) {
                    char *next = cur;
                    bool more = false;
                    while (*next && (*next != '\n')) next++;
                    if (*next) {
                        *next = 0;
                        more = true;
                    }
                    BDBG_MODULE_LOG(AVD,("%d: %s", avdCore, cur));
                    if (more)
                        cur = ++next;
                    else
                        break;
                }
            }
        }
    }
#endif
    
    NEXUS_Platform_P_MonitorOS();
    
    g_platformTimer = NEXUS_ScheduleTimer(1000, NEXUS_Platform_P_Timer, NULL);
}

static void NEXUS_Platform_P_StartMonitor(void)
{
    g_platformTimer = NEXUS_ScheduleTimer(1000, NEXUS_Platform_P_Timer, NULL);
}

static void NEXUS_Platform_P_StopMonitor(void)
{
    if (g_platformTimer) {
        NEXUS_CancelTimer(g_platformTimer);
        g_platformTimer = NULL;
    }
}

#if BDBG_DEBUG_BUILD
#include "bchp_sun_gisb_arb.h"
#include "bchp_sun_l2.h"
#define BCHP_INT_ID_GISB_TIMEOUT_INTR         BCHP_INT_ID_CREATE(BCHP_SUN_L2_CPU_STATUS, BCHP_SUN_L2_CPU_STATUS_GISB_TIMEOUT_INTR_SHIFT)

static BINT_CallbackHandle g_gisbInterrupt;

#if BCHP_CHIP == 7422 || BCHP_CHIP == 7425
static const char *g_pGisbMasters[] =
{
    "SSP",
    "CPU",
    "reserved",
    "pcie",
    "BSP",
    "RDC",
    "raaga",
    "reserved",
    "AVD1",
    "JTAG",
    "SVD0",
    "reserved",
    "VICE0",
    "reserved"
};
#elif BCHP_CHIP == 7435
static const char *g_pGisbMasters[] =
{
    "SSP",
    "CPU",
    "webCPU",
    "pcie",
    "BSP",
    "RDC",
    "raaga_0",
    "reserved",
    "AVD1",
    "JTAG",
    "SVD0",
    "reserved",
    "VICE0",
    "VICE_1",
    "raaga_1",
    "reserved"
};
#elif BCHP_CHIP == 7445
static const char *g_pGisbMasters[] =
{
    "bsp_0",
    "scpu_0",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
    "cpu_0",
    "webcpu_0",
    "jtag_0",
    "ssp_0",
    "rdc_0",
    "hvd_0",
    "hvd_1",
    "shvd_0",
    "reserved",
    "vice_0",
    "vice_1",
    "raaga_0",
    "raaga_1",
    "pcie_0",
	"reserved",
};
#elif BCHP_CHIP == 7344 || BCHP_CHIP==7346 || BCHP_CHIP==7231 || BCHP_CHIP==7429
static const char *g_pGisbMasters[] =
{
    "SSP",
    "CPU",
    "reserved",
    "BSP",
    "RDC",
    "raaga",
    "reserved",
    "JTAG",
    "SVD0",
    "reserved"
};
#elif BCHP_CHIP == 7358 || BCHP_CHIP == 7552 || BCHP_CHIP == 7360
static const char *g_pGisbMasters[] =
{
    "SSP",
    "CPU",
    "reserved",
    "reserved",
    "BSP",
    "RDC",
    "raaga",
    "AVD0",
    "reserved",
    "JTAG",
    "reserved"
};
#elif BCHP_CHIP == 7584
static const char *g_pGisbMasters[] =
{
    "SSP",
    "CPU",
    "reserved",
    "reserved",
    "BSP",
    "RDC",
    "raaga",
    "AVD0",
    "reserved",
    "JTAG",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "SCPU",
    "LEAP",
    "reserved"
};
#else
static const char *g_pGisbMasters[] =
{
    "SSB",
    "CPU",
    "PCI",
    "reserved",
    "BSP",
    "RDC",
    "RPTD",
    "AVD0",
    "reserved",
    "JTAG",
    "reserved"
};
#endif

static void NEXUS_Platform_P_GisbTimeout_isr(void *pParam, int iParam)
{
    uint32_t reg;
    BREG_Handle regHandle = pParam;
    reg = BREG_Read32(regHandle, BCHP_SUN_GISB_ARB_ERR_CAP_STATUS);
    if ( reg & BCHP_MASK(SUN_GISB_ARB_ERR_CAP_STATUS, valid) )
    {
        bool writing = (reg & BCHP_MASK(SUN_GISB_ARB_ERR_CAP_STATUS, write))?true:false;
        const char *pCore = "Unknown";
        unsigned i;
        reg = BREG_Read32(regHandle, BCHP_SUN_GISB_ARB_ERR_CAP_MASTER);
        for ( i = 0; i < sizeof(g_pGisbMasters)/sizeof(const char *); i++ )
        {
            if ( reg & 0x1 )
            {
                pCore = g_pGisbMasters[i];
                break;
            }
            reg >>= 1;
        }
        reg = BREG_Read32(regHandle, BCHP_SUN_GISB_ARB_ERR_CAP_ADDR);
        BSTD_UNUSED(writing);
        BDBG_ERR(("*****************************************************"));
        BDBG_ERR(("GISB Timeout %s addr 0x%08x by core %s", (writing)?"writing":"reading", reg, pCore));
        if (iParam) {
            BDBG_ERR(("This GISB timeout occured before nexus started."));
        }
        BDBG_ERR(("*****************************************************"));
    }
    /* Clear error status */
    BREG_Write32(regHandle, BCHP_SUN_GISB_ARB_ERR_CAP_CLR, 0x1);
    
}
#endif




void NEXUS_Platform_P_ConfigureGisbTimeout()
{
#if BDBG_DEBUG_BUILD
    BERR_Code rc;

    /* This can produce a lot of console output if a block is failing. */
    /* Reduce GISB ARB timer to a small enough value for the CPU to trap errors.
    100 milliseconds is preferred by the HW architecture team. */
    BREG_Write32(g_pCoreHandles->reg, BCHP_SUN_GISB_ARB_TIMER, 10800000);    /* 100 milliseconds @ 108 MHz */
    /* INT is valid now.  Register for GISB interrupt to see if anyone is accessing invalid registers */
    rc = BINT_CreateCallback(&g_gisbInterrupt, g_pCoreHandles->bint, BCHP_INT_ID_GISB_TIMEOUT_INTR, NEXUS_Platform_P_GisbTimeout_isr, g_pCoreHandles->reg, 0);
    if (rc) {
        rc = BERR_TRACE(rc);
    }
    else {
        /* Poll for a pending bad access */
        BKNI_EnterCriticalSection();
        NEXUS_Platform_P_GisbTimeout_isr(g_pCoreHandles->reg, 1);
        BKNI_LeaveCriticalSection();
        /* Enable the interrupt */
        rc = BINT_EnableCallback(g_gisbInterrupt);
        if (rc) {
             rc = BERR_TRACE(rc);
        }
    }
#endif
}
