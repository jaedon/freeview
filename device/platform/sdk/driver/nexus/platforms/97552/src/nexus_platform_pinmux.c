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
*  $brcm_Workfile: nexus_platform_pinmux.c $
*  $brcm_Revision: 14 $
*  $brcm_Date: 4/26/12 5:33p $
* 
*  Module Description:
* 
*  Revision History:
* 
*  $brcm_Log: /nexus/platforms/97552/src/nexus_platform_pinmux.c $
* 
* 14   4/26/12 5:33p xhuang
* SW7552-261: change aon_gpio_04 for external LNA
* 
* 13   2/29/12 3:34p xhuang
* SW7552-148: enable SDIO for no MTSIF case
* 
* 12   2/1/12 3:19p xhuang
* SW7552-141: remove unused code for B0
* 
* 11   12/20/11 2:46p vishk
* SW7552-148: 7552B0: Add 3128 support over MTSIF for 7552B0 SVB board
* 
* 10   11/25/11 4:14p xhuang
* SW7552-141: update pinmux for B0
* 
* 9   10/18/11 6:10p xhuang
* SW7552-135: update pinmux for 7574 board RFM switch
* 
* 8   8/23/11 6:07p xhuang
* SW7552-77: correct pinmux settings for DVB-CI
* 
* 7   8/12/11 1:36p xhuang
* SW7552-75: Enable AVD debug console
* 
* 6   8/8/11 4:34p xhuang
* SW7552-77: Add DVB-CI support
* 
* 5   7/18/11 2:54p xhuang
* SW7552-39: change pinmux settings for smartcard
* 
* 4   6/8/11 7:10p xhuang
* SW7552-34: remove pinmux setting for SDIO
* 
* 3   4/28/11 6:46p xhuang
* SW7552-25: update pinmux settings
* 
* 2   3/17/11 10:02a jgarrett
* SW7552-24: Fixing register handle
* 
* 1   1/25/11 6:13p xhuang
* SW7552-25: Initial version
* 
* 
*  Module Description:
*
*
*  API Description:
*   This file contains pinmux initialization for the 97422 reference board.
*
* 
***************************************************************************/

#include "nexus_platform.h"
#include "nexus_platform_priv.h"
#include "priv/nexus_core.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_aon_pin_ctrl.h"

BDBG_MODULE(nexus_platform_pinmux);

/***************************************************************************
Summary:
    Configure pin muxes for the 97358 reference platform
Description:
    The core module must be initialized for this to be called
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_InitPinmux(void)
{
    BREG_Handle hReg = g_pCoreHandles->reg;
    uint32_t reg;
    NEXUS_PlatformStatus platformStatus;    
    NEXUS_Error rc;

    rc = NEXUS_Platform_GetStatus(&platformStatus);
    if(rc != NEXUS_SUCCESS)
        BDBG_ERR(("Unable to get chip id"));        
    BDBG_MSG(("Chip ID = 0x%x", platformStatus.chipId));

    /* GPIO_00, GPIO_01, default to GIO */
    /* GPIO 02..45 EBI set by OS/default */

    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6
    * GPIO_46    : POD2CHIP_MCLKI
    * GPIO_47    : POD2CHIP_MDI0
    * GPIO_48    : POD2CHIP_MDI1
    * GPIO_49    : POD2CHIP_MDI2
    * GPIO_50    : POD2CHIP_MDI3
    * GPIO_51    : POD2CHIP_MDI4
    * GPIO_52    : POD2CHIP_MDI5
    */

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6);

    reg &= ~(
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_46) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_47) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_48) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_49) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_50) |       
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_51) |       
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_52) 
        );
#if NEXUS_HAS_DVB_CI
    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_46, 1) |  /* POD2CHIP_MCLKI */
#else
    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_46, 3) |  /* EBI Addr 14 */
#endif
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_47, 1) |      /* POD2CHIP_MDI0 */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_48, 1) |      /* POD2CHIP_MDI1 */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_49, 1) |      /* POD2CHIP_MDI2 */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_50, 1) |      /* POD2CHIP_MDI3 */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_51, 1) |      /* POD2CHIP_MDI4 */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_52, 1) ;      /* POD2CHIP_MDI5 */        

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, reg);

    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7
    * GPIO_53    : POD2CHIP_MDI6
    * GPIO_54    : POD2CHIP_MDI7
    * GPIO_55    : POD2CHIP_MISTRT
    * GPIO_56    : POD2CHIP_MIVAL
    * GPIO_57    : CHIP2POD_MCLKO
    * GPIO_58    : CHIP2POD_MDO0
    * GPIO_59    : CHIP2POD_MDO1
    * GPIO_60    : CHIP2POD_MDO2
    */

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);

    reg &= ~(
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_53) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_54) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_55) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_56) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_57) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_58) |       
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_59) |       
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_60)                 
        );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_53, 1) |  /* POD2CHIP_MDI6 */        
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_54, 1) |      /* POD2CHIP_MDI7 */        
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_55, 1) |      /* POD2CHIP_MISTRT */        
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_56, 1) |      /* POD2CHIP_MIVAL */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_57, 1) |      /* CHIP2POD_MCLKO */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_58, 1) |      /* CHIP2POD_MDO0 */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_59, 1) |      /* CHIP2POD_MDO1 */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_60, 1) ;      /* CHIP2POD_MDO2 */        

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, reg);

    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8
    * GPIO_61    : CHIP2POD_MDO3
    * GPIO_62    : CHIP2POD_MDO4
    * GPIO_63    : CHIP2POD_MDO5
    * GPIO_64    : CHIP2POD_MDO6
    * GPIO_65    : CHIP2POD_MDO7
    * GPIO_66    : CHIP2POD_MOSTRT
    * GPIO_67    : CHIP2POD_MOVAL
    * GPIO_68    : EBI_ADDR13
    */

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);

    reg &= ~(
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_61) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_62) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_63) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_64) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_65) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_66) |       
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_67) |       
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_68) 
        );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_61, 1) |  /* CHIP2POD_MDO3 */        
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_62, 1) |      /* CHIP2POD_MDO4 */        
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_63, 1) |      /* CHIP2POD_MDO5 */        
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_64, 1) |      /* CHIP2POD_MDO6 */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_65, 1) |      /* CHIP2POD_MDO7 */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_66, 1) |      /* CHIP2POD_MOSTRT */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_67, 1) |      /* CHIP2POD_MOVAL */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_68, 3) ;      /* EBI Addr 13 */        

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, reg);

    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9
    * GPIO_69    : EBI_ADDR12
    * GPIO_70    : EBI_ADDR2
    * GPIO_71    : EBI_ADDR1
    * GPIO_72    : EBI_ADDR0
    * GPIO_73    : MPOD_M_SDI
    * GPIO_74    : RMX0_CLK
    * GPIO_75    : RMX0_DATA
    * GPIO_76    : RMX0_SYNC
    */

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9);

    reg &= ~(
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_69) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_70) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_71) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_72) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_73) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_74) |       
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_75) |       
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_76)                 
        );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_69, 3) |  /* EBI Addr 13 */        
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_70, 2) |      /* EBI Addr 2 */        
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_71, 2) |      /* EBI Addr 1 */        
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_72, 2) |      /* EBI Addr 0 */
#if NEXUS_HAS_DVB_CI
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_73, 1) ;      /* MPOD_M_SDI */
#else
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_73, 1) |      /* MPOD_M_SDI */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_74, 1) |      /* RMX0_CLK */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_75, 1) |      /* RMX0_DATA */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_76, 1) ;      /* RMX0_SYNC */        
#endif

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, reg);

    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10
    * GPIO_77    : RMX0_VALID
    * GPIO_78    : RMX0_PAUSE
    * GPIO_79    : SC0_VCC
    * GPIO_80    : SC0_CLK
    * GPIO_81    : SC0_RST
    * GPIO_82    : SC0_IO
    * GPIO_83    : SC0_PRES
    * GPIO_84    : GPIO
    */
    
    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10);

    reg &= ~(
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_77) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_78) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_79) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_80) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_81) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_82) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_83) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_84) 
        );

    reg |=  
#if !NEXUS_HAS_DVB_CI
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_77, 1) |  /* RMX0_VALID */        
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_78, 1) |      /* RMX0_PAUSE */    
#endif
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_79, 1) |      /* SC0_VCC */        
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_80, 1) |      /* SC0_CLK */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_81, 1) |      /* SC0_RST */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_82, 1) |      /* SC0_IO */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_83, 1) |      /* SC0_PRES */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_84, 0) ;      /* GPIO */
        
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, reg);

    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11
    * GPIO_85    : GPIO
    * GPIO_86    : SC_VPP
    * GPIO_87    : UART_TX0
    * GPIO_88    : UART_RX0
    * GPIO_89    : ALT_TP_OUT_01
    * GPIO_90    : ALT_TP_IN_01
    * GPIO_91    : ALT_TP_OUT_02
    * GPIO_92    : ALT_TP_IN_02
    */

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);

    reg &= ~(
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_85) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_86) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_87) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_88) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_89) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_90) |      
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_91) |      
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_92) 
        );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_85, 0) |  /* GPIO */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_86, 1) |      /* SC_VPP */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_87, 2) |      /* UART_TX0 */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_88, 2) |      /* UART_RX0 */
#if NEXUS_HAS_DVB_CI
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_89, 4) |      /* POD_EBI_RDB */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_90, 4) |      /* POD_EBI_WE0B */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_91, 4) |      /* POD_EBI_DSB */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_92, 4) ;      /* POD2CHIP_MCLKI */  
#else
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_89, 3) |      /* ALT_TP_OUT_01 */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_90, 3) |      /* ALT_TP_IN_01 */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_91, 3) |      /* ALT_TP_OUT_02 */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_92, 3) ;      /* ALT_TP_IN_02 */  
#endif
              

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, reg);

    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12
    * GPIO_93    : SF_HOLDB
    * GPIO_94    : SF_WPB
    * GPIO_96    : PKT2_CLK
    * GPIO_97     : PKT2_DATA
    * GPIO_98     : PKT2_SYNC
    * GPIO_99     : PKT2_VALID
    * GPIO_100    : PKT2_ERROR
    * GPIO_101    : default
    */

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);

    reg &= ~(
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_93) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_94) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_96) |  
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_97) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_98) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_99) |      
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_100)
        );

#if NEXUS_USE_3128_FRONTEND_DAUGHTER_CARD
    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_93, 2) | /* EXT_IRQ2 */ 
#else
    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_93, 1) | /* SF_HOLDB */ 
#endif
#if NEXUS_HAS_DVB_CI
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_94, 1);     /* SF_WPB */
#else
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_94, 1) |     /* SF_WPB */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_96, 1) |     /* PKT2_CLK */            
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_97, 1) |     /* PKT2_DATA */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_98, 1) |     /* PKT2_SYNC */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_99, 1) |     /* PKT2_VALID */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_100, 1);     /* PKT2_ERROR */
#endif
        
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, reg);

    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13
    * GPIO_102...GPIO_109: default
    * GPIO_110: default EBI 
    */

    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14
    * GPIO_111...GPIO_118: default EBI 
    */

    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15
    * GPIO_123    : RGMII_MDC 
    * GPIO_124    : RGMII_MDIO 
    * SGPIO_00    : BSC_M0_SCL 
    * SGPIO_01    : BSC_M0_SDA 
    */

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15);

    reg &= ~(
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_123) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_124) |     
#if BCHP_VER >= BCHP_VER_B0
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, sgpio_02) |     
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, sgpio_03) 
#else       
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, sgpio_00) |     
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, sgpio_01) 
#endif      
        );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_123, 1) |      /* RGMII_MDC */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_124, 1) |      /* RGMII_MDIO */
#if BCHP_VER >= BCHP_VER_B0
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, sgpio_02, 1) |      /* BSC_M1_SCL */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, sgpio_03, 1) ;      /* BSC_M1_SDA */   
#else
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, sgpio_00, 1) |      /* BSC_M0_SCL */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, sgpio_01, 1) ;      /* BSC_M0_SDA */        
#endif
    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, reg);

#if BCHP_VER == BCHP_VER_A0
    /* BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16
    * SGPIO_02    : BSC_M1_SCL 
    * SGPIO_03    : BSC_M1_SDA 
    */

    reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16);

    reg &= ~(
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, sgpio_02) |
        BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, sgpio_03)       
        );

    reg |=  BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, sgpio_02, 1) |  /* BSC_M1_SCL */        
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, sgpio_03, 1);       /* BSC_M1_SDA */            

    BREG_Write32 (hReg, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, reg);
#endif
    
    /* BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0
    * AON_IR_IN0     : AON_IR_IN0(0)
    * AON_S3_STANDBYB: AON_S3_STANDBYB(0)
    * AON_HDMI_HTPLG : AON_HDMI_HTPLG(0)
    * AON_GPIO_00    : AUD_SPDIF(1)
    * AON_GPIO_01    : ENET_LINK(1)
    * AON_GPIO_03    : LED_OUT(1)
    * AON_GPIO_04    : AON_GPIO_04(0)
    * AON_GPIO_05    : I2S_DATA0_OUT(3)
    */
    
    reg = BREG_Read32(hReg,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0);
    
    reg &=~(
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_ir_in0 ) |
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_s3_standbyb ) |
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_hdmi_htplg ) |       
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_00 ) |
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_01 ) |
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_03 ) |
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_04 ) |
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_05 ) 
        );

    reg |=( 
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_ir_in0, 0 ) |      /* AON_IR_IN0 */
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_s3_standbyb, 0 ) | /* AON_S3_STANDBYB */
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_hdmi_htplg, 0 ) | /* AON_HDMI_HTPLG */
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_00, 1 ) |     /* AUD_SPDIF */
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_01, 1 ) |     /* ENET_LINK */
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_03, 1 ) |     /* LED_OUT */
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_04, 0 ) |     /* I2S_CLK0_OUT */
#if NEXUS_USE_3128_FRONTEND_DAUGHTER_CARD
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_05, 5 )       /* MTSIF_ATS_RST */
#else
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_05, 3 )       /* I2S_DATA0_OUT */
#endif
        );

    BREG_Write32 (hReg, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, reg);

    /* BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1
    * AON_GPIO_06    : I2S_LR0_OUT(3)
    * AON_GPIO_07    : CODEC_SDI(3)
    * AON_GPIO_09    : CODEC_SCLK(3)
    * AON_GPIO_10    : CODEC_SDO(3)
    * AON_GPIO_11    : CODEC_MCLK(3)
    */

    reg = BREG_Read32(hReg,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1);
    
    reg &=~(
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_06 ) |
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_07 ) |
#if NEXUS_USE_3128_FRONTEND_DAUGHTER_CARD
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_08 ) |
#endif        
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_09 ) |
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_10 ) |
#if NEXUS_USE_3128_FRONTEND_DAUGHTER_CARD        
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_11 ) |
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_12 ) |
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_13 ) 
#else
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_11 )
#endif
        );

    reg |=( 
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_06, 3 ) |  /* I2S_LR0_OUT */
#if NEXUS_USE_3128_FRONTEND_DAUGHTER_CARD
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_07, 7 ) |  /* MTSIF0_VALID */
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_08, 7 ) |  /* MTSIF0_DATA1 */
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_09, 7 ) |  /* MTSIF0_DATA5 */
#else
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_07, 3 ) |  /* CODEC_SDI */
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_09, 3 ) |  /* CODEC_SCLK */
#endif
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_10, 3 ) |  /* CODEC_SDO */
#if NEXUS_USE_3128_FRONTEND_DAUGHTER_CARD
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_11, 3 ) |  /* CODEC_SDO */
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_12, 6 ) |  /* MTSIF0_CLK */
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_13, 7 )    /* MTSIF0_DATA0 */
#else
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_11, 3 )   /* CODEC_MCLK */
#endif
        );

    BREG_Write32 (hReg, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1, reg);

    /* BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2
    * AON_SGPIO_00   : BSC_M3_SCL(1)
    */

    reg = BREG_Read32(hReg,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2);

    if(platformStatus.chipId == 0x7574)
    {
        /* AON_GPIO_20 is used for RFM switch on 7574 board, no SDIO interface */
        reg &=~(
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_20 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_00 )
            );
        reg |=(
            BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_20, 0 ) |  /* AON_GPIO */
            BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_00,1 )    /* BSC_M3_SCL */
            );
    }
    else
    {
        reg &=~(
#if NEXUS_USE_3128_FRONTEND_DAUGHTER_CARD
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_14 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_15 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_16 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_17 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_18 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_19 ) |
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_20 ) |
#endif            
            BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_00 )
            );
        reg |=( 
#if NEXUS_USE_3128_FRONTEND_DAUGHTER_CARD
            BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_14, 6 ) |  /* MTSIF0_SYNC */
            BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_15, 6 ) |  /* MTSIF0_DATA2 */
            BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_16, 6 ) |  /* MTSIF0_DATA3 */
            BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_17, 7 ) |  /* MTSIF0_DATA4 */
            BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_18, 6 ) |  /* MTSIF0_ATS_INC */
            BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_19, 6 ) |  /* MTSIF0_DATA6 */
            BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_gpio_20, 7 ) |  /* MTSIF0_DATA7 */
#endif
            BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_00,1 )    /* BSC_M3_SCL */
            );
    }

    BREG_Write32 (hReg, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, reg);

    /* BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3
    * AON_SGPIO_01   : BSC_M3_SDA(1)
    */

    reg = BREG_Read32(hReg,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3);
    
    reg &=~(
#if BCHP_VER >= BCHP_VER_B0
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_01 ) |     
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, sgpio_00) |
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, sgpio_01)
#else           
        BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_01 ) 
#endif      
        );

    reg |=( 
#if BCHP_VER >= BCHP_VER_B0
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_01,1 ) |  /* BSC_M3_SDA */
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, sgpio_00,1 ) |      /* BSC_M0_SCL */
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, sgpio_01,1 )        /* BSC_M0_SDA */
#else           
        BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_01,1 )    /* BSC_M3_SDA */
#endif      
        );

    BREG_Write32 (hReg, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, reg);
    
    /* Configure the AVD UARTS to debug mode.  AVD0_OL -> UART1, AVD1_OL -> UART2. */
    reg = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL);
    reg &= ~(BCHP_MASK(SUN_TOP_CTRL_UART_ROUTER_SEL, port_2_cpu_sel) | BCHP_MASK(SUN_TOP_CTRL_UART_ROUTER_SEL, port_1_cpu_sel));
    reg |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_UART_ROUTER_SEL, port_2_cpu_sel, AVD0_OL);
    reg |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_UART_ROUTER_SEL, port_1_cpu_sel, AVD0_IL);
    BREG_Write32(hReg,BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL,reg);

    reg = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL);
    reg &= ~(BCHP_MASK(SUN_TOP_CTRL_TEST_PORT_CTRL, encoded_tp_enable));
    reg |= BCHP_FIELD_DATA(SUN_TOP_CTRL_TEST_PORT_CTRL,encoded_tp_enable, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL_encoded_tp_enable_SYS);
    BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL, reg);

    /* Configure the Input Band source select options */
    /* DS->IB1; PKT2->IB2 */
#if BCHP_VER == BCHP_VER_A0
    reg = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_GENERAL_CTRL_0);
    reg &= ~(BCHP_MASK(SUN_TOP_CTRL_GENERAL_CTRL_0, ib2_source));
    reg |= BCHP_FIELD_DATA(SUN_TOP_CTRL_GENERAL_CTRL_0,ib2_source, 1); /* PKT2 */
    BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_GENERAL_CTRL_0, reg);
#endif  

    return BERR_SUCCESS;
}


