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
* $brcm_Workfile: nexus_platform_vcxo.c $
* $brcm_Revision: 4 $
* $brcm_Date: 9/6/12 5:35p $
*
* API Description:
*   This file contains pinmux initialization for the 97400 reference board.
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97400/src/nexus_platform_vcxo.c $
* 
* 4   9/6/12 5:35p erickson
* SW7435-349: add standard header
* 
***************************************************************************/

#include "nexus_platform.h"
#include "nexus_platform_priv.h"
#include "priv/nexus_core.h"
#include "bchp_vcxo_0_rm.h"
#include "bchp_vcxo_1_rm.h"
#include "bchp_vcxo_ctl_misc.h"

BDBG_MODULE(nexus_platform_vcxo);

/***************************************************************************
Summary:
    Configure pin muxes for a platform
Description:
    The core module must be initialized for this to be called
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_InitVcxo(void)
{
    BREG_Handle hRegister = g_pCoreHandles->reg;

    uint32_t ui32RegVal;
    int index;

    for ( index = 0; index < 2; index++ )
    {
        unsigned offset = (BCHP_VCXO_1_RM_CONTROL - BCHP_VCXO_0_RM_CONTROL)*index;

        ui32RegVal = BCHP_FIELD_DATA(VCXO_0_RM_CONTROL, RESET, 1);
        BREG_Write32(hRegister, BCHP_VCXO_0_RM_CONTROL+offset, ui32RegVal);

        /* assert reset */
        if ( index == 0 )
        {
            ui32RegVal = BREG_Read32(hRegister, BCHP_VCXO_CTL_MISC_VC0_CTRL);
            ui32RegVal &= ~BCHP_MASK(VCXO_CTL_MISC_VC0_CTRL, DRESET);
            ui32RegVal |= BCHP_FIELD_DATA(VCXO_CTL_MISC_VC0_CTRL, DRESET, 1);
            BREG_Write32(hRegister, BCHP_VCXO_CTL_MISC_VC0_CTRL, ui32RegVal);
        }
        else
        {
            ui32RegVal = BREG_Read32(hRegister, BCHP_VCXO_CTL_MISC_VC1_CTRL);
            ui32RegVal &= ~BCHP_MASK(VCXO_CTL_MISC_VC1_CTRL, DRESET);
            ui32RegVal |= BCHP_FIELD_DATA(VCXO_CTL_MISC_VC1_CTRL, DRESET, 1);
            BREG_Write32(hRegister, BCHP_VCXO_CTL_MISC_VC1_CTRL, ui32RegVal);
        }

        ui32RegVal = BCHP_FIELD_DATA(VCXO_0_RM_RATE_RATIO, DENOMINATOR, 32);
        BREG_Write32(hRegister, BCHP_VCXO_0_RM_RATE_RATIO+offset, ui32RegVal);

        ui32RegVal =
            BCHP_FIELD_DATA(VCXO_0_RM_SAMPLE_INC, NUMERATOR, 29) |
            BCHP_FIELD_DATA(VCXO_0_RM_SAMPLE_INC, SAMPLE_INC, 3);
        BREG_Write32(hRegister, BCHP_VCXO_0_RM_SAMPLE_INC+offset, ui32RegVal);

        ui32RegVal = BCHP_FIELD_DATA(VCXO_0_RM_PHASE_INC, PHASE_INC, 0x7d634);
        BREG_Write32(hRegister, BCHP_VCXO_0_RM_PHASE_INC+offset, ui32RegVal);

        BREG_Write32(hRegister, BCHP_VCXO_0_RM_INTEGRATOR+offset, 0x0);

        ui32RegVal =
            BCHP_FIELD_DATA(VCXO_0_RM_FORMAT, SHIFT, 2) |
            BCHP_FIELD_DATA(VCXO_0_RM_FORMAT, STABLE_COUNT, 10000);
        BREG_Write32(hRegister, BCHP_VCXO_0_RM_FORMAT+offset, ui32RegVal);

        ui32RegVal =
            BCHP_FIELD_DATA(VCXO_0_RM_OFFSET, OFFSET_ONLY, 0) |
            BCHP_FIELD_DATA(VCXO_0_RM_OFFSET, OFFSET, 0x10000000);
        BREG_Write32(hRegister, BCHP_VCXO_0_RM_OFFSET+offset, ui32RegVal);

        ui32RegVal =
            BCHP_FIELD_DATA(VCXO_0_RM_CONTROL, TIMEBASE, index) |   /* Timebase 0 */
            BCHP_FIELD_DATA(VCXO_0_RM_CONTROL, FREE_RUN, 0) |
            BCHP_FIELD_DATA(VCXO_0_RM_CONTROL, DITHER, 1) |
            BCHP_FIELD_DATA(VCXO_0_RM_CONTROL, DIRECT_GAIN, 2) |
            BCHP_FIELD_DATA(VCXO_0_RM_CONTROL, INT_GAIN, 4) |
            BCHP_FIELD_DATA(VCXO_0_RM_CONTROL, RESET, 0);
        BREG_Write32(hRegister, BCHP_VCXO_0_RM_CONTROL+offset, ui32RegVal);

        /* Deassert reset */
        if ( index == 0 )
        {
            ui32RegVal = BREG_Read32(hRegister, BCHP_VCXO_CTL_MISC_VC0_CTRL);
            ui32RegVal &= ~BCHP_MASK(VCXO_CTL_MISC_VC0_CTRL, DRESET);
            BREG_Write32(hRegister, BCHP_VCXO_CTL_MISC_VC0_CTRL, ui32RegVal);
        }
        else
        {
            ui32RegVal = BREG_Read32(hRegister, BCHP_VCXO_CTL_MISC_VC1_CTRL);
            ui32RegVal &= ~BCHP_MASK(VCXO_CTL_MISC_VC1_CTRL, DRESET);
            BREG_Write32(hRegister, BCHP_VCXO_CTL_MISC_VC1_CTRL, ui32RegVal);
        }
    }

    return BERR_SUCCESS;
}

