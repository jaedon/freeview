/***************************************************************************
 *     (c)2005-2008 Broadcom Corporation
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
 * $brcm_Workfile: platformconfig.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 5/11/09 12:39p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/splashgen/93549/platformconfig.c $
 * 
 * 1   5/11/09 12:39p shyam
 * PR53637 : Add splash support for 93549
 * 
 * 2   10/9/08 8:58a erickson
 * PR46184: merge
 * 
 * PR46184/1   10/8/08 6:04p chengs
 * PR46184: Dump more registers.
 * 
 * 1   9/29/08 11:34a erickson
 * PR46184: added splash app
 * 
 * 1   10/3/07 2:39p shyam
 * PR 35680 : add support for 3563
 * 
 * 1   5/14/07 7:01p shyam
 * PR 30741 : Add reference support for generic portable splash
 * 
 * Hydra_Software_Devel/1   11/17/05 4:46p dkaufman
 * PR3481: Added file
 * 
 *
 ***************************************************************************/

#include "platformconfig.h"
 
#include "bchp_gio.h" /* For platform config */
#include "bchp_sun_top_ctrl.h"
#include "bchp_pwm.h"
#include "bchp_lvds_phy_0.h"
#include "bchp_dvpo_0.h"

 
 
#define DUTY_CYCLE          (90)   /* 90% in percentage */
 
#define ENABLE_REG_DUMP 
 
 BERR_Code ConfigPlatform(BREG_Handle hRegister)
 {
#ifdef ENABLE_REG_DUMP
    uint32_t    lval;
    bool        back_light_on = true; 

    /*1....RM_1366x768p_T1540x802_60Hz.bss*/

    /*2....extract from LCD_panel_ctrl.bss*/
    /* 
     setup for totem-pole output
     PWM.CTRL.pwm1_oeb = 0
     PWM.CTRL.pwm1_opendrainb = 1
 
     PWM.CTRL2.pwm1_out_select = 1
     PWM.CTRL2.pwm1_enable_sync = 0
     PWM.CTRL2.pwm1_out_polarity = 0
     PWM.CTRL2.pwm1_sync_edge_sel = 0
 
     PWM.PWM1_CWORD_MSB = &H10&   ' MS byte of 4096 (0x1000)
     PWM.PWM1_CWORD_LSB = &H00&   ' LS byte of 4096 (0x1000)
     PWM.PWM1_PERIOD = 42186
 
     PWM.PWM1_ON = CLng((DUTY_CYCLE / 100) * (42186+1))
     PWM.CTRL.pwm1_start = 1     
    */        
    lval = BREG_Read32 (hRegister, BCHP_PWM_CTRL);
 
    lval &= ~(
                BCHP_MASK(PWM_CTRL,  pwm1_oeb)  
              | BCHP_MASK(PWM_CTRL,  pwm1_opendrainb)
              );
    lval |= 
                BCHP_FIELD_DATA(PWM_CTRL, pwm1_oeb, 0)
              | BCHP_FIELD_DATA(PWM_CTRL, pwm1_opendrainb, 1);
 
    BREG_Write32 (hRegister, BCHP_PWM_CTRL, lval);
 
    APP_BREG_Write32 (hRegister, BCHP_PWM_CTRL, lval);
 
    lval = BREG_Read32 (hRegister, BCHP_PWM_CTRL2);
 
    lval &= ~(
                BCHP_MASK(PWM_CTRL2, pwm1_out_select)  
              | BCHP_MASK(PWM_CTRL2, pwm1_enable_sync)
              | BCHP_MASK(PWM_CTRL2, pwm1_out_polarity)
              | BCHP_MASK(PWM_CTRL2, pwm1_out_polarity)
              );
    lval |= 
                BCHP_FIELD_DATA(PWM_CTRL2, pwm1_out_select, 1)
              | BCHP_FIELD_DATA(PWM_CTRL2, pwm1_enable_sync, 0)
              | BCHP_FIELD_DATA(PWM_CTRL2, pwm1_out_polarity, 0)
              | BCHP_FIELD_DATA(PWM_CTRL2, pwm1_sync_edge_sel, 0);
 
    BREG_Write32 (hRegister, BCHP_PWM_CTRL2, lval);
    
    APP_BREG_Write32 (hRegister, BCHP_PWM_CTRL2, lval);
        
    BREG_Write32 (hRegister, BCHP_PWM_PWM1_CWORD_MSB,16);
 
    APP_BREG_Write32 (hRegister, BCHP_PWM_PWM1_CWORD_MSB, 16);
    
    BREG_Write32 (hRegister, BCHP_PWM_PWM1_CWORD_LSB,0);
    
    APP_BREG_Write32 (hRegister, BCHP_PWM_PWM1_CWORD_LSB, 0);
    
    BREG_Write32 (hRegister, BCHP_PWM_PWM1_PERIOD,42186);
    
    APP_BREG_Write32 (hRegister, BCHP_PWM_PWM1_PERIOD, 42186);

    BREG_Write32 (hRegister, BCHP_PWM_PWM1_ON,DUTY_CYCLE*(42186+1)/100);
 
    APP_BREG_Write32 (hRegister, BCHP_PWM_PWM1_ON,DUTY_CYCLE*(42186+1)/100);
 
    lval = BREG_Read32 (hRegister, BCHP_PWM_CTRL);
 
    lval &= ~(BCHP_MASK(PWM_CTRL,pwm1_start));
 
    lval |= BCHP_FIELD_DATA(PWM_CTRL, pwm1_start, 1);
    
    BREG_Write32 (hRegister, BCHP_PWM_CTRL,lval);
 
    APP_BREG_Write32 (hRegister, BCHP_PWM_CTRL,lval);
 
    /* pin muc control */
    lval = BREG_Read32 (hRegister, BCHP_SUN_TOP_CTRL_GENERAL_CTRL_0);
    APP_BREG_Write32 (hRegister, BCHP_SUN_TOP_CTRL_GENERAL_CTRL_0, lval);
    lval = BREG_Read32 (hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0);
    APP_BREG_Write32 (hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0, lval);
    lval = BREG_Read32 (hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1);
    APP_BREG_Write32 (hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1, lval);
    lval = BREG_Read32 (hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2);
    APP_BREG_Write32 (hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2, lval);
    lval = BREG_Read32 (hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3);
    APP_BREG_Write32 (hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3, lval);
    lval = BREG_Read32 (hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);
    APP_BREG_Write32 (hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4, lval);
    lval = BREG_Read32 (hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5);
    APP_BREG_Write32 (hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5, lval);
    lval = BREG_Read32 (hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6);
    APP_BREG_Write32 (hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, lval);
    lval = BREG_Read32 (hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);
    APP_BREG_Write32 (hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, lval);
    lval = BREG_Read32 (hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);
    APP_BREG_Write32 (hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, lval);

    /* sleep 500
           Panel Configuration 
    */    
    lval = BREG_Read32(hRegister, BCHP_GIO_IODIR_HI);
    lval &= ~(1 << 8);
    BREG_Write32(hRegister, BCHP_GIO_IODIR_HI,lval);
    APP_BREG_Write32 (hRegister, BCHP_GIO_IODIR_HI,lval);
        
    lval = BREG_Read32(hRegister, BCHP_GIO_ODEN_HI);
    lval &= ~(1 << 8);
    BREG_Write32(hRegister, BCHP_GIO_ODEN_HI, lval);
    APP_BREG_Write32 (hRegister, BCHP_GIO_ODEN_HI,lval);
    
    lval = BREG_Read32(hRegister, BCHP_GIO_DATA_HI);
    if (back_light_on == true)
        lval |=  1 << 8;
    else
        lval &= ~(1 << 8) ;
    BREG_Write32(hRegister, BCHP_GIO_DATA_HI, lval);
    APP_BREG_Write32 (hRegister, BCHP_GIO_DATA_HI,lval);
    
    /*3....extract from single_LVDS_config_1366x768p.bss */

    /* 3.1 LINKDIV_CTRL:*/
    
    lval = BREG_Read32(hRegister, BCHP_LVDS_PHY_0_LVDS_PLL_CFG) ;

    lval &= ~(
       BCHP_MASK(LVDS_PHY_0_LVDS_PLL_CFG, LINKDIV_CTRL));

    lval |=
       BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CFG,LINKDIV_CTRL,1);

    BREG_Write32(hRegister, BCHP_LVDS_PHY_0_LVDS_PLL_CFG, lval);
       
    APP_BREG_Write32 (hRegister, BCHP_LVDS_PHY_0_LVDS_PLL_CFG,lval);

    /*3.2   
        ----Enable the LVDS Encoders 
        ----Configure for 10-bit color mode
        ----Reverse the shift order out of the serializer 
        ----Configure for Open LDI format
     */

    lval = BREG_Read32(hRegister, BCHP_DVPO_0_LVDS_CFG_0) ;

    lval &= ~(
       BCHP_MASK(DVPO_0_LVDS_CFG_0, LVDS_ENCODE_ENABLE)
       | BCHP_MASK(DVPO_0_LVDS_CFG_0, COLOR_MODE)
       | BCHP_MASK(DVPO_0_LVDS_CFG_0, REVERSE_LVDS_SHIFT)
       | BCHP_MASK(DVPO_0_LVDS_CFG_0, JEIDA_MODE));

    lval |=
       BCHP_FIELD_DATA(DVPO_0_LVDS_CFG_0,LVDS_ENCODE_ENABLE,1)
       | BCHP_FIELD_DATA(DVPO_0_LVDS_CFG_0,COLOR_MODE,1)
       | BCHP_FIELD_DATA(DVPO_0_LVDS_CFG_0,REVERSE_LVDS_SHIFT,1)
       | BCHP_FIELD_DATA(DVPO_0_LVDS_CFG_0,JEIDA_MODE,0);

    BREG_Write32(hRegister, BCHP_DVPO_0_LVDS_CFG_0, lval);
       
    APP_BREG_Write32 (hRegister, BCHP_DVPO_0_LVDS_CFG_0,lval);

    /*3.3 Determines the LVDS link configuration */

    /*3.3.1 -- Bit mode */
    lval = BREG_Read32(hRegister, BCHP_DVPO_0_MISC_CONTROL) ;

    lval &= ~(
       BCHP_MASK(DVPO_0_MISC_CONTROL, USE_30_BIT_MODE));

    lval |=
       BCHP_FIELD_DATA(DVPO_0_MISC_CONTROL,USE_30_BIT_MODE,1);

    BREG_Write32(hRegister, BCHP_DVPO_0_MISC_CONTROL, lval);
       
    APP_BREG_Write32 (hRegister, BCHP_DVPO_0_MISC_CONTROL,lval);
    
    /*3.3.2 ....Link mode */
    lval = BREG_Read32(hRegister, BCHP_DVPO_0_MISC_CONTROL_2) ;

    lval &= ~(
       BCHP_MASK(DVPO_0_MISC_CONTROL_2, LVDS_MODE)
       | BCHP_MASK(DVPO_0_MISC_CONTROL_2, LINK_1_MAP)
       | BCHP_MASK(DVPO_0_MISC_CONTROL_2, LINK_2_MAP));

    lval |=
       BCHP_FIELD_DATA(DVPO_0_MISC_CONTROL_2,LVDS_MODE,0)
       | BCHP_FIELD_DATA(DVPO_0_MISC_CONTROL_2,LINK_1_MAP,0)
       | BCHP_FIELD_DATA(DVPO_0_MISC_CONTROL_2,LINK_2_MAP,0);

    BREG_Write32(hRegister, BCHP_DVPO_0_MISC_CONTROL_2, lval);
       
    APP_BREG_Write32 (hRegister, BCHP_DVPO_0_MISC_CONTROL_2,lval);

    /* 3.3.3 */
    lval = BREG_Read32 (hRegister, BCHP_DVPO_0_FIFO_CTL);
    APP_BREG_Write32 (hRegister, BCHP_DVPO_0_FIFO_CTL, lval);
    
    /* 3.4 Configure the PLL resets and powerdown control signals  */
    
    lval = BREG_Read32(hRegister, BCHP_LVDS_PHY_0_LVDS_RESET_CTL) ;
    lval &= ~(
       BCHP_MASK(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_DRESET)
       | BCHP_MASK(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_ARESET)
       | BCHP_MASK(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_2_CLK_RST)
       | BCHP_MASK(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_1_CLK_RST)
       | BCHP_MASK(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_REFAMP_PWRDN)

       | BCHP_MASK(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_REFCOMP_PWRDN)
       | BCHP_MASK(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_PLL_PWRDN)
       | BCHP_MASK(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_BG_PWRDN)) ;

    lval |=
       BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_DRESET,0)
       | BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_ARESET,0)
       | BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_2_CLK_RST,0)
       | BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_1_CLK_RST,0)
       | BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_REFAMP_PWRDN,0)

       | BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_REFCOMP_PWRDN,0)
       | BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_PLL_PWRDN,0)
       | BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_BG_PWRDN,0);

       BREG_Write32(hRegister, BCHP_LVDS_PHY_0_LVDS_RESET_CTL, lval);
       
       APP_BREG_Write32 (hRegister, BCHP_LVDS_PHY_0_LVDS_RESET_CTL,lval);

    /* 3.5 Configure LVDS PHY channel resets */
    lval = BREG_Read32(hRegister, BCHP_LVDS_PHY_0_LVDS_RESET_CTL) ;
    lval &= ~(
       BCHP_MASK(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_2_CH_4_PWRDN)
       | BCHP_MASK(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_2_CH_3_PWRDN)
       | BCHP_MASK(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_2_CH_2_PWRDN)
       | BCHP_MASK(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_2_CH_1_PWRDN)
       | BCHP_MASK(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_2_CH_0_PWRDN)

       | BCHP_MASK(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_1_CH_4_PWRDN)
       | BCHP_MASK(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_1_CH_3_PWRDN)
       | BCHP_MASK(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_1_CH_2_PWRDN)
       | BCHP_MASK(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_1_CH_1_PWRDN)
       | BCHP_MASK(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_1_CH_0_PWRDN)
       
       /* Configure CLK Signal */
       | BCHP_MASK(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_2_CLK_PWRDN)
       | BCHP_MASK(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_1_CLK_PWRDN)) ;

    lval |=
       BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_2_CH_4_PWRDN,0)
       | BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_2_CH_3_PWRDN,0)
       | BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_2_CH_2_PWRDN,0)
       | BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_2_CH_1_PWRDN,0)
       | BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_2_CH_0_PWRDN,0)

       | BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_1_CH_4_PWRDN,0)
       | BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_1_CH_3_PWRDN,0)
       | BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_1_CH_2_PWRDN,0)
       | BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_1_CH_1_PWRDN,0)
       | BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_1_CH_0_PWRDN,0)
       
       /* Configure CLK Signal */
       | BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_2_CLK_PWRDN,0)
       | BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_1_CLK_PWRDN,0);
    
       BREG_Write32(hRegister, BCHP_LVDS_PHY_0_LVDS_RESET_CTL, lval);
       
       APP_BREG_Write32 (hRegister, BCHP_LVDS_PHY_0_LVDS_RESET_CTL,lval);       

    lval = BREG_Read32(hRegister, BCHP_LVDS_PHY_0_LVDS_CH_1_CLK_CFG) ;
    APP_BREG_Write32 (hRegister, BCHP_LVDS_PHY_0_LVDS_CH_1_CLK_CFG, lval);       

    lval = BREG_Read32(hRegister, BCHP_LVDS_PHY_0_LVDS_CH_2_CLK_CFG) ;
    APP_BREG_Write32 (hRegister, BCHP_LVDS_PHY_0_LVDS_CH_2_CLK_CFG, lval);       

    lval = BREG_Read32(hRegister, BCHP_LVDS_PHY_0_LVDS_CH_1_DATA_0_CFG) ;
    APP_BREG_Write32 (hRegister, BCHP_LVDS_PHY_0_LVDS_CH_1_DATA_0_CFG, lval);
    
    lval = BREG_Read32(hRegister, BCHP_LVDS_PHY_0_LVDS_CH_1_DATA_1_CFG) ;
    APP_BREG_Write32 (hRegister, BCHP_LVDS_PHY_0_LVDS_CH_1_DATA_1_CFG, lval);       

    lval = BREG_Read32(hRegister, BCHP_LVDS_PHY_0_LVDS_CH_1_DATA_2_CFG) ;
    APP_BREG_Write32 (hRegister, BCHP_LVDS_PHY_0_LVDS_CH_1_DATA_2_CFG, lval);       

    lval = BREG_Read32(hRegister, BCHP_LVDS_PHY_0_LVDS_CH_1_DATA_3_CFG) ;
    APP_BREG_Write32 (hRegister, BCHP_LVDS_PHY_0_LVDS_CH_1_DATA_3_CFG, lval);

    lval = BREG_Read32(hRegister, BCHP_LVDS_PHY_0_LVDS_CH_1_DATA_4_CFG) ;
    APP_BREG_Write32 (hRegister, BCHP_LVDS_PHY_0_LVDS_CH_1_DATA_4_CFG, lval);
    
    lval = BREG_Read32(hRegister, BCHP_LVDS_PHY_0_LVDS_CH_2_DATA_0_CFG) ;
    APP_BREG_Write32 (hRegister, BCHP_LVDS_PHY_0_LVDS_CH_2_DATA_0_CFG, lval);       

    lval = BREG_Read32(hRegister, BCHP_LVDS_PHY_0_LVDS_CH_2_DATA_0_CFG) ;
    APP_BREG_Write32 (hRegister, BCHP_LVDS_PHY_0_LVDS_CH_2_DATA_0_CFG, lval);       

    lval = BREG_Read32(hRegister, BCHP_LVDS_PHY_0_LVDS_CH_2_DATA_1_CFG) ;
    APP_BREG_Write32 (hRegister, BCHP_LVDS_PHY_0_LVDS_CH_2_DATA_1_CFG, lval);       

    lval = BREG_Read32(hRegister, BCHP_LVDS_PHY_0_LVDS_CH_2_DATA_2_CFG) ;
    APP_BREG_Write32 (hRegister, BCHP_LVDS_PHY_0_LVDS_CH_2_DATA_2_CFG, lval);       

    lval = BREG_Read32(hRegister, BCHP_LVDS_PHY_0_LVDS_CH_2_DATA_3_CFG) ;
    APP_BREG_Write32 (hRegister, BCHP_LVDS_PHY_0_LVDS_CH_2_DATA_3_CFG, lval);       

    lval = BREG_Read32(hRegister, BCHP_LVDS_PHY_0_LVDS_CH_2_DATA_4_CFG) ;
    APP_BREG_Write32 (hRegister, BCHP_LVDS_PHY_0_LVDS_CH_2_DATA_4_CFG, lval);
#else
    BSTD_UNUSED(hRegister);
#endif
    return BERR_SUCCESS;
 }


