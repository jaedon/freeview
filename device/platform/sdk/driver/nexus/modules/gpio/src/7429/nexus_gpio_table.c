/***************************************************************************
*     (c)2008-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_gpio_table.c $
* $brcm_Revision: 4 $
* $brcm_Date: 7/6/12 5:07p $
*
* API Description:
*   API name: Gpio
*    Specific APIs related to Gpio Control.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/gpio/7429/src/nexus_gpio_table.c $
* 
* 4   7/6/12 5:07p katrep
* SW7429-127:DVB CI updates from Ashwin
* 
* 3   4/13/12 10:33a erickson
* SW7425-2379: use internally defined macros for number of gpio pins
* 
* 2   10/25/11 5:28p katrep
* SW7429-1:updated for 7429 bringup
* 
* 1   9/22/11 6:03p vsilyaev
* SW7429-23: Merge
* 
* 7429_bringup/1   8/9/11 8:01p katrep
* SW7429-1:initial checkin
* 
* 6   8/9/11 5:28p katrep
* SW7231-253,SW7231-317:updated gpio table for unused pins,add support
*  for 7231B0
* 
* 
* 5   5/9/11 5:26p katrep
* SW7231-137:add support for aon, aons goio
* 
* 4   4/29/11 5:01p katrep
* SW7231-4:gpio 17,18 and 100 do not exist
* 
* 3   4/29/11 1:12p katrep
* SW7231-4:updated missing gpio numbers in the table
* 
* 2   12/9/10 5:56p katrep
* SW7231-4:updated to meet new requirements
* 
* 1   12/1/10 7:12p katrep
* SW7231-4:add initial support
* 
*
***************************************************************************/
#include "nexus_gpio_module.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_aon_pin_ctrl.h"
#include "bchp_gio.h"
#include "bchp_gio_aon.h"
#include "priv/nexus_core.h"

BDBG_MODULE(nexus_gpio_table);

typedef struct NEXUS_GpioTable
{
    uint32_t addr;
    unsigned mask;
    unsigned shift;
} NEXUS_GpioTable;

static NEXUS_GpioTable g_gpioTable[] = {
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0_gpio_000_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0_gpio_000_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0_gpio_001_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0_gpio_001_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0_gpio_002_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0_gpio_002_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0_gpio_003_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0_gpio_003_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0_gpio_004_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0_gpio_004_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0_gpio_005_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0_gpio_005_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0_gpio_006_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0_gpio_006_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0_gpio_007_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0_gpio_007_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1_gpio_008_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1_gpio_008_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1_gpio_009_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1_gpio_009_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1_gpio_010_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1_gpio_010_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1_gpio_011_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1_gpio_011_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1_gpio_012_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1_gpio_012_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1_gpio_013_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1_gpio_013_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1_gpio_014_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1_gpio_014_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1_gpio_015_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1_gpio_015_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2_gpio_016_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2_gpio_016_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2_gpio_017_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2_gpio_017_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2_gpio_018_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2_gpio_018_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2_gpio_019_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2_gpio_019_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2_gpio_020_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2_gpio_020_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2_gpio_021_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2_gpio_021_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2_gpio_022_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2_gpio_022_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2_gpio_023_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2_gpio_023_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3_gpio_024_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3_gpio_024_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3_gpio_025_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3_gpio_025_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3_gpio_026_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3_gpio_026_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3_gpio_027_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3_gpio_027_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3_gpio_028_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3_gpio_028_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3_gpio_029_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3_gpio_029_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3_gpio_030_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3_gpio_030_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3_gpio_031_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3_gpio_031_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_032_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_032_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_033_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_033_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_034_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_034_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_035_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_035_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_036_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_036_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_037_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_037_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_038_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_038_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_039_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_039_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_040_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_040_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_041_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_041_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_042_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_042_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_043_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_043_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_044_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_044_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_045_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_045_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_046_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_046_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_047_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_047_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_048_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_048_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_049_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_049_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_050_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_050_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_051_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_051_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_052_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_052_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_053_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_053_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_054_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_054_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_055_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_055_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_056_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_056_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_057_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_057_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_058_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_058_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_059_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_059_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_060_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_060_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_061_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_061_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_062_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_062_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_063_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_063_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_064_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_064_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_065_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_065_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_066_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_066_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_067_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_067_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_068_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_068_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_069_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_069_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_070_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_070_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_071_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_071_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_072_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_072_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_073_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_073_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_074_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_074_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_075_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_075_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_076_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_076_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_077_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_077_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_078_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_078_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_079_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_079_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_080_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_080_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_081_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_081_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_082_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_082_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_083_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_083_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_084_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_084_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_085_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_085_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_086_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_086_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_087_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_087_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_088_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_088_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_089_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_089_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_090_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_090_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_091_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_091_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_092_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_092_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_093_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_093_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_094_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_094_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_095_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_095_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_096_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_096_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_097_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_097_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_098_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_098_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_099_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_099_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, 0, 0}, /* gpio_0100 missing */
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_101_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_101_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_102_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_102_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_103_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_103_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_104_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_104_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_105_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_105_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_106_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_106_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_107_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_107_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_108_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_108_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_109_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_109_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_110_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_110_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_111_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_111_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_112_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_112_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_113_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_113_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_114_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_114_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, 0, 0}, /* 115 missing */
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_116_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_116_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_117_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_117_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_118_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_118_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, 0, 0}, /* 119 missing */
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, 0, 0}, /* 120 missing */
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, 0, 0}, /* 121 missing */
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_122_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_122_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_123_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_123_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_124_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_124_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_125_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_125_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_126_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_126_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_127_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_127_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_128_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_128_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_129_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_129_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_130_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_130_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_131_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_131_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_132_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_132_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_133_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_133_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_134_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_134_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_135_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_135_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_136_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_136_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_137_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_137_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_138_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_138_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_139_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_139_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_140_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_140_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_141_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_141_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_142_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_142_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_143_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_143_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_144_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_144_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_145_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_145_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_146_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_146_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_147_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_147_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_148_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_148_SHIFT},

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_gpio_149_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_gpio_149_SHIFT}
};


static NEXUS_GpioTable g_sgpioTable[] = {

    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_sgpio_00_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_sgpio_00_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_sgpio_01_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_sgpio_01_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_sgpio_02_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_sgpio_02_SHIFT},
    {BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_sgpio_03_MASK, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18_sgpio_03_SHIFT},
};

NEXUS_GpioTable g_aonGpioTable[] = {
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_00_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_00_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_01_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_01_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_02_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_02_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_03_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_03_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_04_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_04_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_05_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_05_SHIFT},

    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_06_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_06_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_07_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_07_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_08_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_08_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_09_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_09_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_10_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_10_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_11_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_11_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_12_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_12_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_13_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_13_SHIFT},

    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_14_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_14_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_15_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_15_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_16_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_16_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_17_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_17_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_18_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_18_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_19_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_19_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_20_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_20_SHIFT},
};

NEXUS_GpioTable g_aonSgpioTable[] = {

    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_00_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_00_SHIFT},

    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3_aon_sgpio_01_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3_aon_sgpio_01_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3_aon_sgpio_02_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3_aon_sgpio_02_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3_aon_sgpio_03_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3_aon_sgpio_03_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3_aon_sgpio_04_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3_aon_sgpio_04_SHIFT},
    {BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3_aon_sgpio_05_MASK, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3_aon_sgpio_05_SHIFT}

};

/* if these NEXUS_NUM gpio macros are defined in nexus_platform_features.h, it is only for application convenience.
this internal definition is more reliable for implementation. */
#undef NEXUS_NUM_GPIO_PINS
#define NEXUS_NUM_GPIO_PINS (sizeof(g_gpioTable)/sizeof(g_gpioTable[0]))
#undef NEXUS_NUM_SGPIO_PINS
#define NEXUS_NUM_SGPIO_PINS (sizeof(g_sgpioTable)/sizeof(g_sgpioTable[0]))
#undef NEXUS_NUM_AON_GPIO_PINS
#define NEXUS_NUM_AON_GPIO_PINS (sizeof(g_aonGpioTable)/sizeof(g_aonGpioTable[0]))
#undef NEXUS_NUM_AON_SGPIO_PINS
#define NEXUS_NUM_AON_SGPIO_PINS (sizeof(g_aonSgpioTable)/sizeof(g_aonSgpioTable[0]))

/* These functions must be implemented per-chip */
NEXUS_Error NEXUS_Gpio_P_GetPinMux(NEXUS_GpioType type, unsigned pin, uint32_t *pAddr, uint32_t *pMask, unsigned *pShift )
{
    NEXUS_GpioTable *pEntry=NULL;

    switch (type)
    {
        case NEXUS_GpioType_eStandard:
            if ( pin >= NEXUS_NUM_GPIO_PINS)
            {
                return BERR_TRACE(BERR_INVALID_PARAMETER);
            }
            pEntry = g_gpioTable+pin;
            break;
        case NEXUS_GpioType_eSpecial:
            if ( pin >= NEXUS_NUM_SGPIO_PINS )
            {
                return BERR_TRACE(BERR_INVALID_PARAMETER);
            }
            pEntry = g_sgpioTable+pin;
            break; 
        case NEXUS_GpioType_eAonStandard:
            if ( pin >= NEXUS_NUM_AON_GPIO_PINS)
            {
                return BERR_TRACE(BERR_INVALID_PARAMETER);
            }
            pEntry = g_aonGpioTable+pin;
            break; 
        case NEXUS_GpioType_eAonSpecial:
            if ( pin >= NEXUS_NUM_AON_SGPIO_PINS)
            {
                return BERR_TRACE(BERR_INVALID_PARAMETER);
            }
            pEntry = g_aonSgpioTable+pin;
            break; 
        default:
            return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    *pAddr = pEntry->addr;
    *pMask = pEntry->mask;
    *pShift = pEntry->shift;

    return BERR_SUCCESS;
}



NEXUS_Error NEXUS_Gpio_P_CheckPinmux(NEXUS_GpioType type, unsigned pin)
{
    uint32_t addr, mask, shift, val;
    NEXUS_Error rc;
    
    rc = NEXUS_Gpio_P_GetPinMux(type, pin, &addr, &mask, &shift);
    if (rc) return BERR_TRACE(rc);

    val = BREG_Read32(g_pCoreHandles->reg, addr);
    /* this code assumes a pinmux value of 0 is GPIO */
    if ( val & mask ) {
        /* Pin is not configured as GPIO */
        BDBG_ERR(("Pin mux register for %s %u is not properly configured - value %u should be 0",
                  (type == NEXUS_GpioType_eSpecial)?"sgpio":"gpio", pin, val>>shift));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    return BERR_SUCCESS;
}


NEXUS_Error NEXUS_Gpio_P_GetPinData(NEXUS_GpioType type, unsigned pin, uint32_t *pAddress, uint32_t *pShift)
{

  switch (type)
  /* SGPIO Pins 9..0 are in ODEN_EXT */
  {
  case NEXUS_GpioType_eStandard:
    if ( pin < 32 )
    {
      /* GPIO Pins 31..0 are in ODEN_LO */
      *pAddress = BCHP_GIO_ODEN_LO;
      *pShift = pin;
    }
    else if ( pin < 64 )
    {
      /* GPIO Pins 63..32 are in ODEN_HI */
      *pAddress = BCHP_GIO_ODEN_HI;
      *pShift = pin-32;
    }
    else if ( pin < 92 )
    {
      /* DATA [95:64] (GPIO[91:64], SGPIO[3:0]) are in ODEN_EXT, above sgpio bits */
      *pAddress = BCHP_GIO_ODEN_EXT;
      *pShift = (pin-64) + NEXUS_NUM_SGPIO_PINS; /* MF ToDo: Are this and the following values correct? */
    }
    else if ( pin < 124 )
    {
      /* DATA [127:96] (GPIO[123:92]) are in ODEN_EXT_HI */
      *pAddress = BCHP_GIO_ODEN_EXT_HI;
      *pShift = (pin-92);
    }
    else if ( pin < 150 )
    {
      /* OPEN DRAIN ENABLE DATA [153:128] (GPIO[149:124]) are in ODEN_EXT2 */
      *pAddress = BCHP_GIO_ODEN_EXT2;
      *pShift = (pin-124);
    }
    else
    {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    break;
  case NEXUS_GpioType_eSpecial:
    if ( pin >= NEXUS_NUM_SGPIO_PINS)
    {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    else
    {
      *pAddress = BCHP_GIO_ODEN_EXT;
      *pShift = pin;
    }
    break;
  case NEXUS_GpioType_eAonStandard:
    if ( pin >= NEXUS_NUM_AON_GPIO_PINS)
    {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    else
    {
      *pAddress = BCHP_GIO_AON_ODEN_LO;
      *pShift = pin;
    }
    break;
  case NEXUS_GpioType_eAonSpecial:
    if ( pin >= NEXUS_NUM_AON_SGPIO_PINS)
    {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    else
    {
      *pAddress = BCHP_GIO_AON_ODEN_EXT;
      *pShift = pin;
    }
    break;
  default:
    return BERR_TRACE(BERR_INVALID_PARAMETER);
  }
  return BERR_SUCCESS;
}


