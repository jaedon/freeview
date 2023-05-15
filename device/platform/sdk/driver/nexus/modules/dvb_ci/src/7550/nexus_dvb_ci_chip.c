/***************************************************************************
*     (c)2004-2010 Broadcom Corporation
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
* $brcm_Workfile: nexus_dvb_ci_chip.c $
* $brcm_Revision: 1 $
* $brcm_Date: 3/30/10 11:04a $
*
* API Description:
*   API name: DvbCi
*    Specific APIs related DVB Common Interface
*
* Revision History:
*
* $brcm_Log: /nexus/modules/dvb_ci/7550/src/nexus_dvb_ci_chip.c $
* 
* 1   3/30/10 11:04a garylin
* SW7550-137: merge
* 
* SW7550_137/4   3/24/10 4:23p xkliu
* SW7550_137: porting to 7550 release3.0
* 
* SW7550_137/3   2/9/10 9:54p xkliu
* SW7550-137: rework the code structure.
* 
* SW7550_137/2   1/11/10 1:57p xkliu
* SW7550-137: porting dvb-ci on 7550 CVB boards.
* 
* 2   4/24/09 9:59a garylin
* PR50410: fix pin mux
* 
* 1   4/17/09 5:02p jgarrett
* PR 50410: Adding 97405 DVB-CI support
* 
* 2   12/30/08 12:23p jgarrett
* PR 50409: Adding ChipInit/ChipUninit stubs
* 
***************************************************************************/

#include "nexus_dvb_ci_module.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_hif_top_ctrl.h"

BDBG_MODULE(nexus_dvb_ci);

/* Sanity check */
#if BCHP_CHIP != 7550
#error This file is not for the current chip.  Please make sure this file is unique per-chip.
#endif

NEXUS_Error NEXUS_DvbCi_P_SetupPinmuxes(NEXUS_DvbCiHandle handle, bool pcmciaMode)
{
    uint32_t regVal;
    uint8_t dirtyEnableEBI = 0;
    BREG_Handle hReg = g_pCoreHandles->reg;
    /* Only one interface is supported on the 7405 */
    BSTD_UNUSED(handle);

    /* Do this in critical section, these are shared registers */

    BKNI_EnterCriticalSection();

    regVal = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0);
    regVal &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_0, gpio_00) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_0, gpio_01) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_0, gpio_02)
            );
    regVal |= 
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_0, gpio_00, EBI_ADDR0) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_0, gpio_01, EBI_ADDR1) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_0, gpio_02, EBI_ADDR2);           
    BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0, regVal);

    regVal = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1);
    regVal &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_1, gpio_03) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_1, gpio_04) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_1, gpio_05) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_1, gpio_06) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_1, gpio_07) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_1, gpio_08) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_1, gpio_09) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_1, gpio_10) 
            );
    regVal |= 
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_1, gpio_03, EBI_ADDR3) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_1, gpio_04, EBI_ADDR4) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_1, gpio_05, EBI_ADDR5) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_1, gpio_06, EBI_ADDR6) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_1, gpio_07, EBI_ADDR7) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_1, gpio_08, EBI_ADDR8) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_1, gpio_09, EBI_ADDR9) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_1, gpio_10, EBI_ADDR10);           
    BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1, regVal);
    
    regVal = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2);
    regVal &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_2, gpio_15) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_2, gpio_11));
    regVal |= 
            BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_2, gpio_15, EBI_TAB) |
            BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_2, gpio_11, EBI_ADDR11);
    BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2, regVal);

    regVal = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3);
    regVal &= ~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_3, gpio_19));
    regVal |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_3, gpio_19, EBI_RWB);
    BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3, regVal);


    regVal = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);
    regVal &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_91) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_92));
    regVal |= 
            BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_91, EBI_DATA8) |
            BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_92, EBI_DATA9);
    BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4, regVal);

    regVal = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5);
    regVal &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_93) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_94) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_95) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_96) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_97) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_98) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_31) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_32) 
            );
    regVal |= 
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_93, EBI_DATA10) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_94, EBI_DATA11) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_95, EBI_DATA12) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_96, EBI_DATA13) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_97, EBI_DATA14) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_98, EBI_DATA15) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_31, EBI_RDB) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_32, EBI_WE0B);           
    BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5, regVal);

    regVal = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9);
    regVal &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_57) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_58) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_59) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_60) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_61) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_62) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_63)
            );
    regVal |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_57, CHIP2POD_MOVAL) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_58, POD_EBI_ADDR13) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_59, POD_EBI_ADDR12) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_60, POD_EBI_ADDR2) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_61, POD_EBI_ADDR1) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_62, POD_EBI_ADDR0) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_63, MPOD_M_SDI);
    BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, regVal);

    regVal = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);
    regVal &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_74) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_75) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_76)
            );
    regVal |= 
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_74, POD_EBI_RDB) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_75, POD_EBI_WE0B) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_76, POD_EBI_DSB);
    BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, regVal);

    if ( pcmciaMode )
    {
    regVal = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6);
    regVal &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_36) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_37) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_38) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_39) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_40)
            );
    BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, regVal);
	
    regVal = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);
    regVal &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_41) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_42) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_43) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_44) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_45) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_46) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_47) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_48)
            );
    BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, regVal);

    regVal = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);
    regVal &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_49) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_50) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_51) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_52) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_53) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_54) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_55) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_56)
        );
    BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, regVal); 

    regVal = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);
    regVal &= ~ (BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_81));
    BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, regVal);
    }
    else
    { /*dvb-ci mode*/
    regVal = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6);
    regVal &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_36) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_37) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_38) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_39) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_40)
            );
    regVal |= 
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_36, POD_EBI_ADDR14) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_37, POD2CHIP_MDI0) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_38, POD2CHIP_MDI1) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_39, POD2CHIP_MDI2) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_40, POD2CHIP_MDI3);
    BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, regVal);

    regVal = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);
    regVal &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_41) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_42) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_43) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_44) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_45) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_46) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_47) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_48)
            );
    regVal |= 
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_41, POD2CHIP_MDI4) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_42, POD2CHIP_MDI5) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_43, POD2CHIP_MDI6) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_44, POD2CHIP_MDI7) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_45, POD2CHIP_MISTRT) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_46, POD2CHIP_MIVAL) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_47, CHIP2POD_MCLKO) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_48, CHIP2POD_MDO0);
    BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, regVal);

    regVal = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);
    regVal &= ~(
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_49) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_50) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_51) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_52) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_53) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_54) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_55) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_56)
            );
    regVal |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_49, CHIP2POD_MDO1) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_50, CHIP2POD_MDO2) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_51, CHIP2POD_MDO3) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_52, CHIP2POD_MDO4) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_53, CHIP2POD_MDO5) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_54, CHIP2POD_MDO6) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_55, CHIP2POD_MDO7) |
           BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_56, CHIP2POD_MOSTRT);
    BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, regVal);   

    regVal = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);
    regVal &= ~ (BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_81));
    regVal |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_81, POD2CHIP_MCLKI);
    BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, regVal);
    }

    /*This bit must be set active by MIPS boot code to fully use the EBI. 
      Software should wait 500nS after setting this bit before using the EBI.*/
    regVal = BREG_Read32(hReg, BCHP_HIF_TOP_CTRL_HIF_CTRL);
    if((regVal & BCHP_HIF_TOP_CTRL_HIF_CTRL_ENABLE_FULL_USE_OF_EBI_MASK) == 0)
    {
        BDBG_ERR(("ENABLE full use of EBI"));
        dirtyEnableEBI = 1;
        regVal &= ~(BCHP_HIF_TOP_CTRL_HIF_CTRL_ENABLE_FULL_USE_OF_EBI_MASK);
        regVal |= BCHP_FIELD_DATA(HIF_TOP_CTRL_HIF_CTRL, ENABLE_FULL_USE_OF_EBI, 1);
        BREG_Write32(hReg, BCHP_HIF_TOP_CTRL_HIF_CTRL, regVal);
    }    
    BKNI_LeaveCriticalSection();
    if(dirtyEnableEBI)
    {
        BKNI_Sleep(1);
    }
    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_DvbCi_P_InitChip(NEXUS_DvbCiHandle handle)
{
    BSTD_UNUSED(handle);
    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_DvbCi_P_UninitChip(NEXUS_DvbCiHandle handle)
{
    BSTD_UNUSED(handle);
    return NEXUS_SUCCESS;
}

