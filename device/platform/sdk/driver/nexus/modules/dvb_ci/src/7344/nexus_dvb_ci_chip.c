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
* $brcm_Revision: 3 $
* $brcm_Date: 12/17/10 3:49p $
*
* API Description:
*   API name: DvbCi
*    Specific APIs related DVB Common Interface
*
* Revision History:
*
* $brcm_Log: /nexus/modules/dvb_ci/7344/src/nexus_dvb_ci_chip.c $
* 
* 3   12/17/10 3:49p garylin
* SW7344-18: fix comment, the io direction is based on DVB-CI spec. chip
*  to CI is input.
* 
* 2   12/17/10 2:24p garylin
* SW7344-18: fix address line GPIOs
* 
* 1   12/17/10 10:46a garylin
* SW7344-18: init version
* 
* 
***************************************************************************/

#include "nexus_dvb_ci_module.h"
#include "bchp_sun_top_ctrl.h"
#include "nexus_gpio.h"

BDBG_MODULE(nexus_dvb_ci);

/* Sanity check */
#if BCHP_CHIP != 7344
#error This file is not for the current chip.  Please make sure this file is unique per-chip.
#endif

static NEXUS_GpioHandle g_gpioA15, g_gpioA16, g_gpioA17, g_gpioA18;
static NEXUS_GpioHandle g_gpioA19, g_gpioA20, g_gpioA21, g_gpioA22;
static NEXUS_GpioHandle g_gpioA23, g_gpioA24, g_gpioA25;
static NEXUS_GpioHandle g_gpioEBI;

NEXUS_Error NEXUS_DvbCi_P_SetupPinmuxes(NEXUS_DvbCiHandle handle, bool pcmciaMode)
{
    uint32_t regVal;

    /* Only one interface is supported on the 7344 */
    BSTD_UNUSED(handle);

    /* Do this in critical section, these are shared registers */

    BKNI_EnterCriticalSection();

    if ( pcmciaMode )
    {
        /* From the schematic, the 7344 has limited EBI addressing for the CI card.  A15..A25 are not connected, nor is D8..D15 
           Switch these pins to GPIO for this mode, address pins should be driven low because there are no pull down resistors */
           
        /* Chip to CI */
        regVal = BREG_Read32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6);

        regVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_06_MASK| /* MDI_6/A24 -> A24 (GPIO - must drive to 0) */
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_05_MASK|        /* MDI_5/A23 -> A23 (GPIO - must drive to 0) */
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_04_MASK|        /* MDI_4/A22 -> A22 (GPIO - must drive to 0) */
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_03_MASK|        /* MDI_3/A21 -> A21 (GPIO - must drive to 0) */
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_02_MASK|        /* MDI_2/A20 -> A20 (GPIO - must drive to 0) */ 
		      BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_01_MASK|        /* MDI_1/A19 -> A19 (GPIO - must drive to 0) */ 
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_00_MASK);        /* MDI_0/A18 -> A18 (GPIO - must drive to 0) */

        BREG_Write32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, regVal); 
		
        regVal = BREG_Read32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);

        regVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_10_MASK|  /* MCLKI/A15 -> A15 (GPIO - must drive to 0) */
                     BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_09_MASK|         /* MIVAL/A16 -> A16 (GPIO - must drive to 0) */
			BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_08_MASK|         /* MISTRT/A17 -> A17 (GPIO - must drive to 0) */
                     BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_07_MASK);        /* MDI_7/A25 -> A25 (GPIO - must drive to 0) */

        BREG_Write32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, regVal);

        /* CI to Chip */
        regVal = BREG_Read32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7); 

        regVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_14_MASK|  /* MCLKO/VS2 -> VS2 (ignored) */
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_13_MASK|          /* MOVAL/BVD2 -> BVD2 (ignored) */
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_12_MASK);         /* MOSTRT/BVD1 -> BVD1 (ignored) */

        BREG_Write32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, regVal);

        regVal = BREG_Read32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);
		
        regVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_50_MASK|   /* MDI_6/D14 -> D14 (ignored) */
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_49_MASK|          /* MDI_5/D13 -> D13 (ignored) */
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_48_MASK|          /* MDI_4/D12 -> D12 (ignored) */
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_47_MASK|          /* MDI_3/D11 -> D11 (ignored) */
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_46_MASK|          /* MDI_2/D10 -> D10 (ignored) */
		      BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_45_MASK|          /* MDI_1/D9 -> D10 (ignored) */
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_44_MASK);         /* MDI_0/D8 -> D10 (ignored) */

        BREG_Write32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, regVal);

        regVal = BREG_Read32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);
		
        regVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_51_MASK);  /* MDI_7/D15 -> D15 (ignored) */
		
        BREG_Write32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, regVal);
    }
    else
    {
        /* Chip to CI */
        regVal = BREG_Read32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6);

        regVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_06_MASK|
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_05_MASK|
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_04_MASK|
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_03_MASK|
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_02_MASK|
		      BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_01_MASK|
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_00_MASK); 

        regVal |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_06, 1) |
                    BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_05, 1) | 
                    BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_04, 1) |
                    BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_03, 1) | 
                    BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_02, 1) | 
		      BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_01, 1) |
                    BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_00, 1); 
		
        BREG_Write32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, regVal);

        regVal = BREG_Read32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);

        regVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_10_MASK|
                     BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_09_MASK|
			BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_08_MASK|
                     BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_07_MASK);
        
        regVal |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_10, 1) |
		      BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_09, 1) |
                    BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_08, 1) | 
                    BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_07, 1);
                    
        BREG_Write32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, regVal);

        /* CI to Chip */
        regVal = BREG_Read32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7); 

        regVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_14_MASK|
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_13_MASK|
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_12_MASK);

        regVal |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_14, 1) | 
                    BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_13, 1) |  
                    BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_12, 1); 
		
        BREG_Write32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, regVal);

        regVal = BREG_Read32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);
		
        regVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_50_MASK|
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_49_MASK| 
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_48_MASK| 
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_47_MASK|
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_46_MASK|   
		      BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_45_MASK|
                    BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_44_MASK);

        regVal |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_50, 2) | 
                    BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_49, 2) | 
                    BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_48, 2) | 
                    BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_47, 2) |
                    BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_46, 2) |	
		      BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_45, 2) |
                    BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_44, 2); 

        BREG_Write32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, regVal);

        regVal = BREG_Read32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);
		
        regVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_51_MASK);

        regVal |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_51, 2);
			
        BREG_Write32(g_pCoreHandles->reg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, regVal);
    }

    BKNI_LeaveCriticalSection();

    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_DvbCi_P_InitChip(NEXUS_DvbCiHandle handle)
{
    NEXUS_GpioSettings gpioSettings;
    NEXUS_Error errCode = NEXUS_SUCCESS;

    /* All pins will be setup to drive 0 on unused address lines */

    NEXUS_Gpio_GetDefaultSettings(NEXUS_GpioType_eStandard, &gpioSettings);
    gpioSettings.mode = NEXUS_GpioMode_eOutputPushPull;
    gpioSettings.value = NEXUS_GpioValue_eLow;

    g_gpioA15 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 10, &gpioSettings);
    if ( NULL == g_gpioA15 ) {errCode = BERR_TRACE(BERR_NOT_SUPPORTED); goto error;}
    g_gpioA16 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 9, &gpioSettings);
    if ( NULL == g_gpioA16 ) {errCode = BERR_TRACE(BERR_NOT_SUPPORTED); goto error;}
    g_gpioA17 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 8, &gpioSettings);
    if ( NULL == g_gpioA17 ) {errCode = BERR_TRACE(BERR_NOT_SUPPORTED); goto error;}
    g_gpioA18 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 0, &gpioSettings);
    if ( NULL == g_gpioA18 ) {errCode = BERR_TRACE(BERR_NOT_SUPPORTED); goto error;}
    g_gpioA19 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 1, &gpioSettings);
    if ( NULL == g_gpioA19 ) {errCode = BERR_TRACE(BERR_NOT_SUPPORTED); goto error;}
    g_gpioA20 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 2, &gpioSettings);
    if ( NULL == g_gpioA20 ) {errCode = BERR_TRACE(BERR_NOT_SUPPORTED); goto error;}
    g_gpioA21 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 3, &gpioSettings);
    if ( NULL == g_gpioA21 ) {errCode = BERR_TRACE(BERR_NOT_SUPPORTED); goto error;}
    g_gpioA22 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 4, &gpioSettings);
    if ( NULL == g_gpioA22 ) {errCode = BERR_TRACE(BERR_NOT_SUPPORTED); goto error;}
    g_gpioA23 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 5, &gpioSettings);
    if ( NULL == g_gpioA23 ) {errCode = BERR_TRACE(BERR_NOT_SUPPORTED); goto error;}
    g_gpioA24 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 6, &gpioSettings);
    if ( NULL == g_gpioA24 ) {errCode = BERR_TRACE(BERR_NOT_SUPPORTED); goto error;}
    g_gpioA25 = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 7, &gpioSettings);
    if ( NULL == g_gpioA25 ) {errCode = BERR_TRACE(BERR_NOT_SUPPORTED); goto error;}
        
    g_gpioEBI = NEXUS_Gpio_Open(NEXUS_GpioType_eStandard, 15, &gpioSettings);
    if ( NULL == g_gpioEBI ) {errCode = BERR_TRACE(BERR_NOT_SUPPORTED); goto error;}       

    NEXUS_Gpio_SetSettings(g_gpioEBI, &gpioSettings);

    return NEXUS_SUCCESS;
error:
    (void)NEXUS_DvbCi_P_UninitChip(handle);
    return errCode;    
}

NEXUS_Error NEXUS_DvbCi_P_UninitChip(NEXUS_DvbCiHandle handle)
{
    BSTD_UNUSED(handle);

    if ( g_gpioA25 ) {NEXUS_Gpio_Close(g_gpioA25); g_gpioA25 = NULL;}
    if ( g_gpioA24 ) {NEXUS_Gpio_Close(g_gpioA24); g_gpioA24 = NULL;}
    if ( g_gpioA23 ) {NEXUS_Gpio_Close(g_gpioA23); g_gpioA23 = NULL;}
    if ( g_gpioA22 ) {NEXUS_Gpio_Close(g_gpioA22); g_gpioA22 = NULL;}
    if ( g_gpioA21 ) {NEXUS_Gpio_Close(g_gpioA21); g_gpioA21 = NULL;}
    if ( g_gpioA20 ) {NEXUS_Gpio_Close(g_gpioA20); g_gpioA20 = NULL;}
    if ( g_gpioA19 ) {NEXUS_Gpio_Close(g_gpioA19); g_gpioA19 = NULL;}
    if ( g_gpioA18 ) {NEXUS_Gpio_Close(g_gpioA18); g_gpioA18 = NULL;}
    if ( g_gpioA17 ) {NEXUS_Gpio_Close(g_gpioA17); g_gpioA17 = NULL;}
    if ( g_gpioA16 ) {NEXUS_Gpio_Close(g_gpioA16); g_gpioA16 = NULL;}
    if ( g_gpioA15 ) {NEXUS_Gpio_Close(g_gpioA15); g_gpioA15 = NULL;}

    if ( g_gpioEBI ) {NEXUS_Gpio_Close(g_gpioEBI); g_gpioEBI = NULL;}

    return NEXUS_SUCCESS;
}

