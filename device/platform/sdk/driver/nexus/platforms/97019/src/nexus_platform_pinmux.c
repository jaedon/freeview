/***************************************************************************
*     (c)2004-2011 Broadcom Corporation
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
* $brcm_Workfile: nexus_platform_pinmux.c $
* $brcm_Revision: 13 $
* $brcm_Date: 11/16/11 12:07p $
*
* API Description:
*   This file contains pinmux initialization for the 97400 reference board.
*
* Revision History:
 *
 * $brcm_Log: /nexus/platforms/97019/src/nexus_platform_pinmux.c $
 * 
 * 13   11/16/11 12:07p mward
 * SW7125-396:  Make sure UART routing is not set for AVD.
 * 
 * 12   12/14/10 3:46p mward
 * SW7125-729: Add NEXUS_Platform_P_IsHostFrontendPinmux() to check the
 *  state of I2C control.
 * 
 * 11   8/26/10 6:07p mward
 * SW7125-460:  Pinmux changes needed for DVB-CI.
 * 
 * 10   6/11/10 3:40p mward
 * SW7125-35: Eliminate the NEXUS_97125_HOST_3112 build option, always
 *  build 97125 with support for host control of BCM3112 QAM DS. Add
 *  NEXUS_Platform_P_HostFrontendPinmux() to switch BSC_M3 between host
 *  and BNM.
 * 
 * 9   5/10/10 6:59p mward
 * SW7125-396: Set SPI pins for use by BNM for VoIP.
 * 
 * 8   5/6/10 2:38p mward
 * SW7125-389: Leave gpio_26, gpio_27 alone to be set by CFE for BNM.
 * 
 * 7   4/28/10 7:02p mward
 * SW7125-33: Enable LEDs for use by software.
 * 
 * 6   3/24/10 4:54p mward
 * SW7125-176: 7019C0 has no gpio_85..88_pad_ctrl.
 * 
 * 5   11/5/09 6:23p mward
 * SW7125-33: For now, hard-code PWR LED on, others off.
 * 
 * 4   9/18/09 10:40a mward
 * SW7125-4: Allow CFE to set pinmux for BSC_M3 to BNM use.
 * 
***************************************************************************/

#include "nexus_platform.h"
#include "nexus_platform_priv.h"
#include "priv/nexus_core.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_gio.h"

BDBG_MODULE(nexus_platform_pinmux);

/***************************************************************************
Summary:
	Configure pin muxes for a 97125 reference platform
Description:
    The core module must be initialized for this to be called
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_InitPinmux(void)
{
    BREG_Handle hReg = g_pCoreHandles->reg;
    uint32_t reg;

    BDBG_ASSERT(NULL != hReg);

    /* Writing pinmux registers in numerical order.  For reference, and as a check,
	   explicitly set all pinmuxes here, even those with default setting. (d) 
	   Conditioned out if set elsewhere. */

    reg = BREG_Read32(hReg,
                      BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0);
    reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_0, pci_ad04 ) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_0, pci_ad03 ) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_0, pci_ad02 ) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_0, pci_ad01 ) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_0, pci_ad00 ) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_0, clk_acc  ) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_0, gpio_12  ) |
            BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_0, gpio_11  )
            );
    reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_0, pci_ad04, 0 ) | /*(d) pci_ad04 [p.18] */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_0, pci_ad03, 0 ) | /*(d) pci_ad03 [p.18] */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_0, pci_ad02, 0 ) | /*(d) pci_ad02 [p.18] */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_0, pci_ad01, 0 ) | /*(d) pci_ad01 [p.18] */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_0, pci_ad00, 0 ) | /*(d) pci_ad00 [p.18] */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_0, clk_acc,  0 ) | /*(d) clk_acc [p.22] */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_0, gpio_12,  1 ) | /* ENET_ACTIVITY [p.26] */
        BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_0, gpio_11,  1 )	 /* ENET_LINK [p.26] */
    ;
    BREG_Write32(hReg,
                 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0,
                 reg);

	reg = BREG_Read32(hReg,
					  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1);
	reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_1, pci_ad12 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_1, pci_ad11 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_1, pci_ad10 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_1, pci_ad09 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_1, pci_ad08 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_1, pci_ad07 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_1, pci_ad06 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_1, pci_ad05 )
			);
	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_1, pci_ad12, 0 ) | /*(d) pci_ad12 [p.20] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_1, pci_ad11, 0 ) | /*(d) pci_ad11 [p.20] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_1, pci_ad10, 0 ) | /*(d) pci_ad10 [p.20] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_1, pci_ad09, 0 ) | /*(d) pci_ad09 [p.20] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_1, pci_ad08, 0 ) | /*(d) pci_ad08 [p.20] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_1, pci_ad07, 0 ) | /*(d) pci_ad07 [p.20] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_1, pci_ad06, 0 ) | /*(d) pci_ad06 [p.20] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_1, pci_ad05, 0 )	 /*(d) pci_ad05 [p.20] */
	;
	BREG_Write32(hReg,
				 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2,
				 reg);

	reg = BREG_Read32(hReg,
					  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2);
	reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_2, pci_ad20 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_2, pci_ad19 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_2, pci_ad18 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_2, pci_ad17 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_2, pci_ad16 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_2, pci_ad15 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_2, pci_ad14 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_2, pci_ad13 )
			);
	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_2, pci_ad20, 0 ) | /*(d) pci_ad20 [p.20] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_2, pci_ad19, 0 ) | /*(d) pci_ad19 [p.20] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_2, pci_ad18, 0 ) | /*(d) pci_ad18 [p.20] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_2, pci_ad17, 0 ) | /*(d) pci_ad17 [p.20] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_2, pci_ad16, 0 ) | /*(d) pci_ad16 [p.20] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_2, pci_ad15, 0 ) | /*(d) pci_ad15 [p.20] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_2, pci_ad14, 0 ) | /*(d) pci_ad14 [p.20] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_2, pci_ad13, 0 )	 /*(d) pci_ad13 [p.20] */
	;
	BREG_Write32(hReg,
				 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2,
				 reg);

	reg = BREG_Read32(hReg,
					  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3);
	reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_3, pci_ad28 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_3, pci_ad27 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_3, pci_ad26 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_3, pci_ad25 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_3, pci_ad24 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_3, pci_ad23 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_3, pci_ad22 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_3, pci_ad21 )
			);
	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_3, pci_ad28, 0 ) | /*(d) pci_ad28 [p.20] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_3, pci_ad27, 0 ) | /*(d) pci_ad27 [p.20] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_3, pci_ad26, 0 ) | /*(d) pci_ad26 [p.20] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_3, pci_ad25, 0 ) | /*(d) pci_ad25 [p.20] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_3, pci_ad24, 0 ) | /*(d) pci_ad24 [p.20] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_3, pci_ad23, 0 ) | /*(d) pci_ad23 [p.20] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_3, pci_ad22, 0 ) | /*(d) pci_ad22 [p.20] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_3, pci_ad21, 0 )	 /*(d) pci_ad21 [p.20] */
	;
	BREG_Write32(hReg,
				 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3,
				 reg);

	reg = BREG_Read32(hReg,
					  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);
	reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, pkt_clk0 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_01  ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_00  ) |
			#if 0 /* CFE will set for BNM */
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_27  ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_26  ) |
			#endif
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, pci_ad31 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, pci_ad30 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, pci_ad29 )
			);
	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_4, pkt_clk0, 0 ) | /*(d) pkt_clk0 [p.27] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_01,  0 ) | /*(d) gpio_01 / BCM3112B_UART_TXD [p.32] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_00,  0 ) | /*(d) gpio_00 [p.32] */
		#if 0 /* CFE will set for BNM */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_27,  1 ) | /*    GP_IRQ1 / IRQ_BCM3112A_N [p.14] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_26,  1 ) | /*    GP_IRQ0 / IRQ_BCM3112B_N [p.14] */
		#endif
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_4, pci_ad31, 0 ) | /*(d) pci_ad31 [p.20] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_4, pci_ad30, 0 ) | /*(d) pci_ad30 [p.20] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_4, pci_ad29, 0 )	 /*(d) pci_ad29 [p.20] */
	;
	BREG_Write32(hReg,
				 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4,
				 reg);

	reg = BREG_Read32(hReg,
					  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5);
	reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_103  ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, rmx_sync0 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, rmx_data0 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, rmx_clk0  ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, pkt_error0) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, pkt_valid0) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, pkt_sync0 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, pkt_data0 )
			);
	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_103,  1 ) | /*    RMX_CLK1/RMX_CLK1_FPGA [p.27] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_5, rmx_sync0, 0 ) | /*(d) rmx_sync0 / 1394_TS0_IN_SYNC [p.27] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_5, rmx_data0, 0 ) | /*(d) rmx_data0 / 1394_TS0_IN_DATA [p.27] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_5, rmx_clk0,  0 ) | /*(d) rmx_clk0 / 1394_TS0_IN_CLK [p.27] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_5, pkt_error0,0 ) | /*(d) pkt_error0 [p.27] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_5, pkt_valid0,0 ) | /*(d) pkt_valid0 [p.27] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_5, pkt_sync0, 0 ) | /*(d) pkt_sync0 [p.27] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_5, pkt_data0, 0 )	  /*(d) pkt_data0 [p.27] */
	;
	BREG_Write32(hReg,
				 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5,
				 reg);

	reg = BREG_Read32(hReg,
					  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6);
	reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_07  ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_06  ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_05  ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_04  ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_03  ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_02  ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_105 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_104 )
			);
	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_07, 1 ) | /* PCM_CLK [p.14] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_06, 0 ) | /*(d) gpio_06 [p.32] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_05, 0 ) | /*(d) gpio_05 [p.32] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_04, 0 ) | /*(d) gpio_04 [p.32] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_03, 0 ) | /*(d) gpio_03 [p.32] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_02, 0 ) | /*(d) gpio_02 [p.32] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_105,1 ) | /* GP_RMX_SYNC1 [p.14] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_104,1 )   /* GP_RMX_DATA1 [p.14] */
	;
	BREG_Write32(hReg,
				 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6,
				 reg);

	reg = BREG_Read32(hReg,
					  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);
	reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, sc0_vcc     ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, sc0_pres    ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, sc0_rst     ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, sc0_clk_out ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, sc0_io      ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_10     ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_09     ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_08     )
			);
	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, sc0_vcc, 0     ) | /*(d) sc0_vcc [p.24] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, sc0_pres, 0    ) | /*(d) sc0_pres [p.24] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, sc0_rst, 0     ) | /*(d) sc0_rst [p.24] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, sc0_clk_out, 0 ) | /*(d) sc0_clk_out [p.24] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, sc0_io, 0      ) | /*(d) sc0_io [p.24] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_10, 1     ) | /* PCM_SDOUT [p.14] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_09, 1     ) | /* PCM_SDIN [p.14] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_08, 1     )   /* PCM_SDFS [p.14] */
	;
	BREG_Write32(hReg,
				 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7,
				 reg);

	reg = BREG_Read32(hReg,
					  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);
	reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, uart_1_rxd ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, uart_0_rtsb) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, uart_0_ctsb) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, uart_0_txd ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, uart_0_rxd ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_15    ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_14    ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_13    )
			);
	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, uart_1_rxd, 1 ) | /* BNM_SPI2_CLK [p.14] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, uart_0_rtsb,0 ) | /*(d) uart_0_rtsb [p.22] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, uart_0_ctsb,0 ) | /*(d) uart_0_ctsb [p.22] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, uart_0_txd, 0 ) | /*(d) uart_0_txd [p.22] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, uart_0_rxd, 0 ) | /*(d) uart_0_rxd [p.22] */
#if NEXUS_HAS_DVB_CI
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_15,    0 ) | /* DVB-CI: VccEn1 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_14,    0 ) | /* DVB-CI: VppEn0  */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_13,    0 )   /* DVB-CI:  VppEn1 */
#else
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_15,    1 ) | /* SC_VPP_0 [p.24] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_14,    1 ) | /* SC_AUX1_0 [p.24] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_13,    1 )   /* SC_AUX1_1 [p.24] */
#endif
	;
	BREG_Write32(hReg,
				 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8,
				 reg);

	reg = BREG_Read32(hReg,
					  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9);
	reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_20     ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_19     ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_18     ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_17     ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_16     ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, uart_1_rtsb ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, uart_1_ctsb ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, uart_1_txd  )
			);
	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_20,     1 ) | /* GP_UART_RXD_BNM [p.22] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_19,     1 ) | /* GP_UART_2_RTS [p.22] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_18,     1 ) | /* GP_UART_2_CTS [p.22] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_17,     1 ) | /* GP_UART_2_TXD [p.22] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_16,     1 ) | /* GP_UART_2_RXD [p.22] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, uart_1_rtsb, 2 ) | /* BNM_SPI2_SSB_0 [p.14] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, uart_1_ctsb, 2 ) | /* BNM_SPI2_MISO [p.14] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, uart_1_txd,  1 )   /* BNM_SPI2_MOSI [p.14] */
	;
	BREG_Write32(hReg,
				 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9,
				 reg);

	reg = BREG_Read32(hReg,
					  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10);
	reg &= ~(
			#if 0 /* CFE will set for BNM */
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_06 ) |
			#endif
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_05 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_04 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_03 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_02 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_01 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_00 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_21 )
			);
	reg |=
		#if 0 /* CFE will set for BNM */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_06, 2 ) | /* BNM_M_SCL [p.14] */
		#endif
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_05, 1 ) | /* BSC_M2_SDA [p.14] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_04, 1 ) | /* BSC_M2_SCL [p.14] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_03, 1 ) | /* BSC_M1_SDA [p.14] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_02, 1 ) | /* BSC_M1_SCL [p.14] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_01, 1 ) | /* SGP_BSC_M0_SDA [p.14] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_00, 1 ) | /* SGP_BSC_M0_SCL [p.14] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_21,  1 )   /* GP_UART_TXD_BNM [p.22] */
	;
	BREG_Write32(hReg,
				 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10,
				 reg);

	reg = BREG_Read32(hReg,
					  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);
	reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_30 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_29 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_28 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_25 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_24 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_23 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_22 )
		#if 0 /* CFE will set for BNM */
			| BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, sgpio_07 )
		#endif 
			);
	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_30, 1 ) | /* IR_IN1 [p.23] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_29, 1 ) | /* IR_IN0 [p.23] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_28, 1 ) | /* IR_OUT [p.23] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_25, 1 ) | /* SPI_M_SSb_0 [p.25] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_24, 1 ) | /* SPI_M_MISO [p.25] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_23, 1 ) | /* SPI_M_MOSI [p.25] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, gpio_22, 1 )   /* SPI_M_SCK [p.25] */
		#if 0 /* CFE will set for BNM */
		| BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, sgpio_07,  2 )   /*  BNM_M_SDA [p.2] */
		#endif
	;
	BREG_Write32(hReg,
				 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11,
				 reg);

	reg = BREG_Read32(hReg,
					  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);
	reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_38 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_37 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_36 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_35 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_34 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_33 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_32 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_31 )
			);
	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_38, 1 ) | /* GP_VI0_656_D6 [p.30] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_37, 1 ) | /* GP_VI0_656_D5 [p.30] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_36, 1 ) | /* GP_VI0_656_D4 [p.30] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_35, 1 ) | /* GP_VI0_656_D3 [p.30] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_34, 1 ) | /* GP_VI0_656_D2 [p.30] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_33, 1 ) | /* GP_VI0_656_D1 [p.30] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_32, 1 ) | /* GP_VI0_656_D0 [p.30] */
#if NEXUS_HAS_DVB_CI
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_31, 0 )  /* DVB-CI: Ireq/Ready */
#else
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_31, 1 )   /* GP_VI0_656_CLK [p.30] */
#endif
	;
	BREG_Write32(hReg,
				 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12,
				 reg);

	reg = BREG_Read32(hReg,
					  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13);
	reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_100 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_99 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_98 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_97 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_42 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_41 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_40 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_39 )
			);
	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_100, 1 ) | /* MII_RXD_0_NB [p.25] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_99, 1 ) | /* MII_RXD_1_NB [p.25] */
#if NEXUS_HAS_DVB_CI
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_98, 0 ) | /* DVB-CI: CD1 */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_97, 0 ) | /* DVB-CI: CD2 */
#else
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_98, 1 ) | /* MII_RXD_2_NB [p.25] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_97, 1 ) | /* MII_RXD_3_NB [p.25] */
#endif
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_42, 1 ) | /* GP_I2S_I_LR [p.15] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_41, 1 ) | /* GP_I2S_I_DATA [p.15] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_40, 1 ) | /* GP_I2S_I_CLK [p.15] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_13, gpio_39, 1 )   /* GP_VI0_656_D7 [p.30] */
	;
	BREG_Write32(hReg,
				 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13,
				 reg);

	reg = BREG_Read32(hReg,
					  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14);
	reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_48 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_47 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_46 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_45 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_44 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_43 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_102 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_101 )
			);
	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_48, 1 ) | /* GP_CHIP2CC_CTX [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_47, 1 ) | /* GP_POD_EBI_ADDR_3 [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_46, 1 ) | /* GP_CHIP2CC_SCTL [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_45, 1 ) | /* GP_CHIP2CC_SCLK [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_44, 1 ) | /* GP_CHIP2CC_SDO [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_43, 1 ) | /* GP_CC2CHIP_SDI [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_102, 1 ) | /* MII_RXD_ERR_NB [p.25] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_14, gpio_101, 1 )   /* MII_RXD_EN_NB [p.25] */
	;
	BREG_Write32(hReg,
				 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14,
				 reg);

	reg = BREG_Read32(hReg,
					  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15);
	reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_56 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_55 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_54 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_53 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_52 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_51 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_50 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_49 )
			);
	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_56, 1 ) | /* GP_CHIP2CC_MOCLK [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_55, 1 ) | /* GP_POD_EBI_ADDR_11 [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_54, 1 ) | /* GP_POD_EBI_ADDR_10 [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_53, 1 ) | /* GP_CHIP2CC_DRX [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_52, 1 ) | /* GP_CHIP2CC_CRX [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_51, 1 ) | /* GP_CC2CHIP_QTX [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_50, 1 ) | /* GP_CC2CHIP_ETX [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_15, gpio_49, 1 )   /* GP_CC2CHIP_ITX [p.29] */
	;
	BREG_Write32(hReg,
				 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15,
				 reg);

	reg = BREG_Read32(hReg,
					  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16);
	reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_64 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_63 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_62 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_61 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_60 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_59 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_58 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_57 )
			);
	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_64, 1 ) | /* GP_CHIP2CC_MDO2 [p.29] (note:  some of these don't match RDB) */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_63, 1 ) | /* GP_CHIP2CC_MDO1 [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_62, 1 ) | /* GP_CHIP2CC_MDO0 [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_61, 1 ) | /* GP_CHIP2CC_MOSTRT [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_60, 1 ) | /* GP_CHIP2CC_MOVAL [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_59, 1 ) | /* GP_CHIP2CC_MCLKO [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_58, 1 ) | /* GP_CC2CHIP_MCLKI [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_16, gpio_57, 1 )   /* GP_CC2CHIP_MICLK [p.29] */
	;
	BREG_Write32(hReg,
				 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16,
				 reg);

	reg = BREG_Read32(hReg,
					  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17);
	reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_72 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_71 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_70 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_69 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_68 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_67 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_66 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_65 )
			);
	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_72, 1 ) | /* GP_CC2CHIP_MDI0 [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_71, 1 ) | /* GP_CC2CHIP_MISTRT [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_70, 1 ) | /* GP_CC2CHIP_MIVAL [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_69, 1 ) | /* GP_CHIP2CC_MDO7 [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_68, 1 ) | /* GP_CHIP2CC_MDO6 [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_67, 1 ) | /* GP_CHIP2CC_MDO5 [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_66, 1 ) | /* GP_CHIP2CC_MDO4 [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_17, gpio_65, 1 )   /* GP_CHIP2CC_MDO5 [p.29] */
	;
	BREG_Write32(hReg,
				 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17,
				 reg);

	reg = BREG_Read32(hReg,
					  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18);
	reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_80 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_79 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_78 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_77 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_76 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_75 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_74 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_73 )
			);
	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_80, 1 ) | /* GP_LED_LS_0 [p.23] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_79, 1 ) | /* GP_CC2CHIP_MDI7 [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_78, 1 ) | /* GP_CC2CHIP_MDI6 [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_77, 1 ) | /* GP_CC2CHIP_MDI5 [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_76, 1 ) | /* GP_CC2CHIP_MDI4 [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_75, 1 ) | /* GP_CC2CHIP_MDI3 [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_74, 1 ) | /* GP_CC2CHIP_MDI2 [p.29] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_18, gpio_73, 1 )   /* GP_CC2CHIP_MDI1 [p.29] */
	;
	BREG_Write32(hReg,
				 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18,
				 reg);

	reg = BREG_Read32(hReg,
					  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19);
	reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_19, gpio_88 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_19, gpio_87 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_19, gpio_86 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_19, gpio_85 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_19, gpio_84 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_19, gpio_83 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_19, gpio_82 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_19, gpio_81 )
			);
	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_19, gpio_88, 0 ) | /* GPIO_88 [p.23] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_19, gpio_87, 0 ) | /* GPIO_87 [p.23] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_19, gpio_86, 0 ) | /* GPIO_86 [p.23] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_19, gpio_85, 0 ) | /* GPIO_85 [p.23] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_19, gpio_84, 1 ) | /* GP_LED_LS_4 [p.23] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_19, gpio_83, 1 ) | /* GP_LED_LS_3 [p.23] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_19, gpio_82, 1 ) | /* GP_LED_LS_2 [p.23] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_19, gpio_81, 1 )   /* GP_LED_LS_1 [p.23] */
	;
	BREG_Write32(hReg,
				 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19,
				 reg);

	#if ((BCHP_CHIP==7125) && (BCHP_VER<BCHP_VER_C0))
	reg = BREG_Read32(hReg,
					  BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_11);
	reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_11 , gpio_85_pad_ctrl ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_11 , gpio_86_pad_ctrl ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_11 , gpio_87_pad_ctrl ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_11 , gpio_88_pad_ctrl )
			);
	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_11 , gpio_85_pad_ctrl, 2 ) | /* PULL_UP (keypad line) */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_11 , gpio_86_pad_ctrl, 2 ) | /* PULL_UP (keypad line) */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_11 , gpio_87_pad_ctrl, 2 ) | /* PULL_UP (keypad line) */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_11 , gpio_88_pad_ctrl, 2 )   /* PULL_UP (keypad line) */
	;
	BREG_Write32(hReg,
				 BCHP_SUN_TOP_CTRL_PIN_MUX_PAD_CTRL_11,
				 reg);
	#endif

	/* For now, just hard-code the LEDs off, except for green PWR one */
	reg = BREG_Read32(hReg,
					  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20);
	reg &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_20, gpio_96 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_20, gpio_95 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_20, gpio_94 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_20, gpio_93 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_20, gpio_92 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_20, gpio_91 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_20, gpio_90 ) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_20, gpio_89 )
			);
	reg |=
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_20, gpio_96, 1 ) | /* GP_LED_LD_7 [p.23] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_20, gpio_95, 1 ) | /* GP_LED_LD_6 [p.23] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_20, gpio_94, 1 ) | /* GP_LED_LD_5 [p.23] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_20, gpio_93, 1 ) | /* GP_LED_LD_4 [p.23] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_20, gpio_92, 1 ) | /* GP_LED_LD_3 [p.23] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_20, gpio_91, 1 ) | /* GP_LED_LD_2 [p.23] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_20, gpio_90, 1 ) | /* GP_LED_LD_1 [p.23] */
		BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_20, gpio_89, 1 )   /* GP_LED_LD_0 [p.23] */
	;
	BREG_Write32(hReg,
				 BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_20,
				 reg);

    /* Disable the AVD UART */
    reg = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL);  
	reg &= ~(BCHP_MASK(SUN_TOP_CTRL_UART_ROUTER_SEL, port_1_cpu_sel));
	reg |= BCHP_FIELD_ENUM(SUN_TOP_CTRL_UART_ROUTER_SEL, port_1_cpu_sel, NO_CPU);
    BREG_Write32(hReg,BCHP_SUN_TOP_CTRL_UART_ROUTER_SEL,reg);

    reg = BREG_Read32(hReg, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL);
    reg &= ~(BCHP_MASK(SUN_TOP_CTRL_TEST_PORT_CTRL, encoded_tp_enable));
    reg |= BCHP_FIELD_DATA(SUN_TOP_CTRL_TEST_PORT_CTRL,encoded_tp_enable,
                          BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL_encoded_tp_enable_UNUSED_127);
    
    BREG_Write32(hReg, BCHP_SUN_TOP_CTRL_TEST_PORT_CTRL, reg);

    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_Platform_P_HostFrontendPinmux(bool host)
{
	BREG_Handle hReg = g_pCoreHandles->reg;
	uint32_t reg;

	BDBG_ASSERT(NULL != hReg);
	reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10);
	reg &= ~BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_06 );
	reg |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_06, host ? 1 : 2 );	/* BSC_M3_SCL : BNM_M_SCL */
	BREG_Write32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10,reg);

	reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);
	reg &= ~BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_11, sgpio_07 );
	reg |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_11, sgpio_07,  host ? 1 : 2 ); /*  BSC_M3_SDA : BNM_M_SDA */
	BREG_Write32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11,reg);

	return BERR_SUCCESS;
}

bool NEXUS_Platform_P_IsHostFrontendPinmux(void)
{
	BREG_Handle hReg = g_pCoreHandles->reg;
	uint32_t reg;

	BDBG_ASSERT(NULL != hReg);
	reg = BREG_Read32(hReg,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10);
	reg &= BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_06 );
    return (reg == BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_06, 1 ));
}

